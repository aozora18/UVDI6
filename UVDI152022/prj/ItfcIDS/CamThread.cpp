
/*
 desc : TCP/IP Client Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "CamThread.h"
#include "CamMain.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 desc : 생성자
 parm : link		- [in]  카메라 연결 상태 정보
		cam_main	- [in]  Camera Control Object
 retn : None
*/
CCamThread::CCamThread(LPG_DLSM link, CCamMain *cam_main)
{
	m_u8MatchingRotate	= 0x00;
	m_dbMatchingRate	= 80.0f;

	m_pstLink			= link;

	m_pstGrabbed		= NULL;
	/* 가장 최근에 Grabbed 결과에 대해 얼라인 카메라 별로 저장하기 위함 임시 버퍼 */
	m_pstGrabACam		= (LPG_ACLR)::Alloc(sizeof(STG_ACLR));
	ASSERT(m_pstGrabACam);
	memset(m_pstGrabACam, 0x00, sizeof(STG_ACLR));

	// 카메라 이벤트 객체 생성
	m_pCamMain		= cam_main;
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CCamThread::~CCamThread()
{
	// 큐에 저장된 Grabbed 이미지 제거
	ResetGrabbedImage();
	/* Grabbed 카메라 별 임시 버퍼 메모리 해제 */
	if (m_pstGrabACam)	::Free(m_pstGrabACam);
}

/*
 desc : 스레드 실행시 초기 한번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamThread::StartWork()
{
	m_u64LiveTime	= GetTickCount64();
	m_u64AttachTime	= GetTickCount64();

	return TRUE;
}

/*
 desc : 스레드 실행시 주기적 호출됨
 parm : None
 retn : None
*/
VOID CCamThread::RunWork()
{
#if (0 == USE_EQUIPMENT_SIMULATION_MODE)
	/* 현재 카메라가 연결되어 있는지 확인 */
	if (!m_pCamMain->IsConnected())
	{
		/* 연결 해제 상태 */
		UpdateLinkTime(0x00);

		/* 특정 시간 마다 주기적으로 Open 수행 (5 초 마다 진행) */
		if (m_u64AttachTime+5000 < GetTickCount64())
		{
			/* 기존 연결해제 및 다시 연결 시도 */
			m_pCamMain->AttachDevice();	/* AttachDevice 내부 함수에서 DetechDevice 함수 호출함 */
			/* 최근 접속을 수행한 시간 저장 */
			m_u64AttachTime	= GetTickCount64();
		}
	}
	else
	{
		/* 연결 완료 상태 */
		UpdateLinkTime(0x01);

		// 연결되어 있다면, 현재 Grabbed Image가 존재하는지 확인 후, 검색 진행
		switch (m_pCamMain->GetCamMode())
		{
		case ENG_VCCM::en_grab_mode	:	ProcGrabbedImage();	break;
		case ENG_VCCM::en_live_mode	:
			if (GetTickCount64() > m_u64LiveTime +
				(UINT64)ROUNDUP(1000.0f / m_pCamMain->GetFramePerSecond(), 0))
			{
				m_u64LiveTime	= GetTickCount64();
				m_pCamMain->RunGrabbedImage();
			}
			break;
		}
	}
#else
	/* 연결 완료 상태 (무조건 연결되었다고 설정) */
	UpdateLinkTime(0x01);
	// 연결되어 있다면, 현재 Grabbed Image가 존재하는지 확인 후, 검색 진행
	switch (m_pCamMain->GetCamMode())
	{
	case ENG_VCCM::en_grab_mode	:	ProcGrabbedImage();	break;
	case ENG_VCCM::en_live_mode	:	break;
	}
#endif
}

/*
 desc : 스레드 종료시 한번 호출됨
 parm : None
 retn : None
*/
VOID CCamThread::EndWork()
{
}

/*
 desc : 카메라 연결 여부
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamThread::IsConnected()
{
	return m_pCamMain->IsConnected();
}

/*
 desc : 얼라인 카메라 부분만 재접속 수행
 parm : None
 retn : None
*/
VOID CCamThread::Reconnected()
{
	m_pCamMain->DetachDevice();
}

/*
 desc : 에러 발생 처리
 parm : mesg	- [in]  에러 메시지 (subject)
 retn : None
*/
VOID CCamThread::SetError(TCHAR *mesg)
{
	LOG_SAVED(ENG_LNWE::en_error, mesg);
}

/*
 desc : 기존 Grabbed 이미지 모두 제거
 parm : None
 retn : None
*/
VOID CCamThread::ResetGrabbedImage()
{
	// 동기화 진입
	if (m_syncGrab.Enter())
	{
		m_lstGrab.RemoveAll();
		// 동기화 해제
		m_syncGrab.Leave();
	}
}

/*
 desc : Grabbed Image가 존재하면, Align Mark 수행
 parm :	None
 retn : None
*/
VOID CCamThread::ProcGrabbedImage()
{
	BOOL bFinded		= FALSE;
	LPG_ACGR pstGrab	= {NULL};

	// 동기화 진입
	if (m_syncGrab.Enter())
	{
		// Grabbed Image가 존재하는 경우만 처리
		pstGrab	= m_pCamMain->GetGrabbedImage();

		// 현재 Grabbed Image가 존재한다면 ...
		if (pstGrab && pstGrab->grab_data)
		{
			/* Model Find */
			bFinded	= uvMIL_RunModelFind(pstGrab->cam_id, pstGrab->img_id, pstGrab->grab_data);
			/* 검색 성공 혹은 실패와 상관 없이 Mark 검색 결과 정보 얻기 */
			pstGrab	= uvMIL_GetLastGrabbedMark();
			if (pstGrab)	pstGrab->marked	= UINT8(bFinded);
			/* 검색 여부와 상관 없이 등록하지만, 특정 개수 이상이면 더 이상 등록하지 않음 */
			if (m_lstGrab.GetCount() < MAX_GRABBED_IMAGE)
			{
				m_lstGrab.AddTail(pstGrab);
			}
			else
			{
				uvLogs_SaveLogs(ENG_LNWE::en_warning, L"The number of images captured has been exceeded");
			}
		}
		// 동기화 해제
		m_syncGrab.Leave();
	}
}

/*
 desc : 사용자에 의해 수동으로 검색된 경우, 결과 값만 처리
 parm : grab	- [in]  사용자에 의해 수동으로 입력된 grabbed image 결과 정보가 저장된 구조체 포인터
		gmfr	- [in]  GMFR Data
		gmsr	- [in]  GMSR Data
 retn : TRUE or FALSE
*/
BOOL CCamThread::SetGrabbedMark(LPG_ACGR grab, LPG_GMFR gmfr, LPG_GMSR gmsr)
{
	STG_ACGR stGrab	= {NULL};

	if (!uvMIL_SetGrabbedMark(grab, gmfr, gmsr))	return FALSE;
	/* 검색 성공 혹은 실패와 상관 없이 Mark 검색 결과 정보 얻기 */
	if (!uvMIL_GetLastGrabbedMark())	return FALSE;
	memcpy(&stGrab, uvMIL_GetLastGrabbedMark(), sizeof(STG_ACGR)-sizeof(PUINT8));
	stGrab.marked	= 0x01;
	/* 변경된 값 반영 */
	SetGrabbedMark(&stGrab);

	return TRUE;
}

/*
 desc : Grabbed Image의 결과 개수
 parm : None
 retn : 개수
*/
UINT16 CCamThread::GetGrabbedCount()
{
	UINT16 u16Count	= 0;

	// 동기화 진입
	if (m_syncGrab.Enter())
	{
		u16Count = (UINT16)m_lstGrab.GetCount();
		// 동기화 해제
		m_syncGrab.Leave();
	}

	return u16Count;
}

/*
 desc : Grabbed Images 모두 Score 값이 유효한지 값 반환
 parm : None
 retn : TRUE : 유효함, FALSE : 유효하지 않음
*/
BOOL CCamThread::IsScoreValidAll()
{
	BOOL bValid			= TRUE;
	POSITION pPos		= NULL;
	LPG_ACGR pstGrab	= {NULL};

	// 동기화 진입
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.GetHeadPosition();
		while (pPos)
		{
			pstGrab	= m_lstGrab.GetNext(pPos);
			/* Score 값이 유효한지 확인 */
			if (!pstGrab->valid_score)
			{
				bValid	= FALSE;
				break;
			}
		}

		// 동기화 해제
		m_syncGrab.Leave();
	}

	return bValid;
}

/*
 desc : Grabbed Image의 데이터 반환
 parm : img_id	- [in]  Camera Grabbed Image Index (0 or Later)
		data	- [out] 반환될 구조체 참조 변수
 retn : TRUE or FALSE
*/
LPG_ACGR CCamThread::GetGrabbedMark(UINT8 img_id)
{
	POSITION pPos	= NULL;
	LPG_ACGR pstGrab= NULL;

	// 동기화 진입
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.GetHeadPosition();
		while (pPos)
		{
			pstGrab	= m_lstGrab.GetNext(pPos);
			if (pstGrab->img_id == img_id)	break;
			pstGrab	= NULL;	/* 찾지 못했으므로, 반드시 NULL 시켜야 됨 */
		}
		// 동기화 해제
		m_syncGrab.Leave();
	}

	return pstGrab;;
}

/*
 desc : Grabbed Image의 데이터 수정
 parm : cam_id	- [in]  카메라 번호 (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
		grab	- [in]  저장될 구조체 변수
 retn : TRUE or FALSE
*/
BOOL CCamThread::SetGrabbedMark(LPG_ACGR grab)
{
	BOOL bFinded	= FALSE;
	POSITION pPos	= NULL, pPrePos;
	LPG_ACGR pstGrab= {NULL};

	// 동기화 진입
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.GetHeadPosition();
		while (!bFinded && pPos)
		{
			pPrePos	= pPos;
			pstGrab	= m_lstGrab.GetNext(pPos);
			bFinded	= pstGrab->cam_id == grab->cam_id && pstGrab->img_id == grab->img_id;
			if (bFinded)
			{
				memcpy(pstGrab, grab, sizeof(STG_ACGR) - sizeof(PUINT8));
				break;
			}
		}
		// 동기화 해제
		m_syncGrab.Leave();
	}

	return bFinded;
}

/*
 desc : Grabbed Image의 데이터 반환
 parm : index	- [in]  가져오고자 하는 위치 (Zero based)
		data	- [out] 반환될 구조체 참조 변수
 retn : TRUE or FALSE
*/
LPG_ACGR CCamThread::GetGrabbedMarkIndex(UINT8 index)
{
	POSITION pPos	= NULL;
	LPG_ACGR pstGrab= NULL;

	// 동기화 진입
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.FindIndex(index);
		if (pPos)	pstGrab	= m_lstGrab.GetAt(pPos);
		// 동기화 해제
		m_syncGrab.Leave();
	}

	return pstGrab;
}

/*
 desc : Grabbed Image의 데이터 수정
 parm : index	- [in]  가져오고자 하는 위치 (Zero based)
		grab	- [in]  저장될 구조체 변수
 retn : TRUE or FALSE
*/
BOOL CCamThread::SetGrabbedMarkIndex(UINT8 index, LPG_ACGR grab)
{
	BOOL bFinded	= FALSE;
	POSITION pPos	= NULL;
	LPG_ACGR pstGrab= NULL;

	// 동기화 진입
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.FindIndex(index);
		if (pPos)
		{
			pstGrab	= m_lstGrab.GetAt(pPos);
			memcpy(pstGrab, grab, sizeof(STG_ACGR)-sizeof(PUINT8));
			bFinded	= TRUE;
		}
		// 동기화 해제
		m_syncGrab.Leave();
	}

	return bFinded;
}

/*
 desc : Mark Search가 모두 정상적으로 이루어 졌는지 여부
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamThread::IsGrabbedMarkValidAll()
{
	BOOL bSucc		= FALSE;
	POSITION pPos	= NULL;
	LPG_ACGR pstGrab= {NULL};

	// 동기화 진입
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.GetHeadPosition();
		while (pPos)
		{
			pstGrab	= m_lstGrab.GetNext(pPos);
			bSucc	= pstGrab->IsMarkValid() && pstGrab->marked;
			if (!bSucc)	break;
		}
		// 동기화 해제
		m_syncGrab.Leave();
	}

	return bSucc;
}

/*
 desc : Calibration Image가 존재하면, Align Mark 수행
 parm :	mode	- [in]  0xff : Calibration Mode, 0xfe : Align Camera Angle Measuring Mode
 retn : Calirbated Image의 정보가 저장된 구조체 포인터 반환
*/
LPG_ACGR CCamThread::RunModelCali(UINT8 mode)
{
	BOOL bFinded	= FALSE;

	/* 현재 Camera 동작 모드가 Calibration 모드인지 확인 */
	if (m_pCamMain->GetCamMode() != ENG_VCCM::en_cali_mode)
	{
		SetError(L"It is not calibration mode.");
		return NULL;
	}
	// Calibrated Image가 존재하는 경우만 처리
	m_pstGrabbed	= m_pCamMain->GetGrabbedImage();
	// 현재 Grabbed Image가 존재한다면 ...
	if (m_pstGrabbed->grab_size > 0 && m_pstGrabbed->grab_width > 0 && m_pstGrabbed->grab_height)
	{
		// 동기화 진입
		if (m_syncGrab.Enter())
		{
			/* Model Find */
			bFinded	= uvMIL_RunModelFind(m_pstGrabbed->cam_id,
										 mode,	/* In case of calibration mode, the value is fixed */
										 m_pstGrabbed->grab_data);
			/* 일단, 최근 측정된 얼라인 카메라의 Grabbed 정보가 잘못되었다고 플래그 설정 */
			m_pstGrabACam->marked	= 0x00;
			if (bFinded)
			{
				/* 검색 성공된 Mark 정보 얻기 */
				if (uvMIL_GetLastGrabbedMark())	m_pstGrabbed	= uvMIL_GetLastGrabbedMark();
				m_pstGrabbed->marked	= bFinded ? 0x01 /* success */ : 0x02 /* failed */;
				/* 최근 얼라인 카메라의 Grabbed 결과 값 갱신 */
				m_pstGrabACam->marked	= m_pstGrabbed->marked;
				m_pstGrabACam->move_mm_x= m_pstGrabbed->move_mm_x;
				m_pstGrabACam->move_mm_y= m_pstGrabbed->move_mm_y;
			}
			// 동기화 해제
			m_syncGrab.Leave();
		}
	}
	return m_pstGrabbed;
}

/*
 desc : Step (Vision 단차) Image가 존재하면, Align Mark 수행
 parm :	count	- [in]  검색 대상의 Mark 개수 (찾고자하는 마크 개수)
		angle	- [in]  각도 적용 여부 (TRUE : 각도 적용, FALSE : 각도 적용 없음)
						TRUE : 현재 카메라의 회전된 각도 구하기 위함, FALSE : 기존 각도 적용하여 회전된 각도 구함
		results	- [out] 검색된 결과 정보가 저장될 구조체 참조 포인터 (grabbed image buffer까지 복사해옴)
 retn : TRUE or FALSE
*/
BOOL CCamThread::RunModelStep(UINT16 count, BOOL angle, LPG_ACGR results)
{
	UINT16 i		= 0;
	BOOL bFinded	= FALSE;
	LPG_ACGR pstGrab= NULL;
	/* 반드시 설정 */
	for (i=0; i<count; i++)	results[i].cam_id	= 0x01;

	/* 현재 Camera 동작 모드가 Calibration 모드인지 확인 */
	if (m_pCamMain->GetCamMode() != ENG_VCCM::en_cali_mode)
	{
		SetError(L"It is not calibration mode");
		return NULL;
	}
	// Calibrated Image가 존재하는 경우만 처리
	pstGrab	= m_pCamMain->GetGrabbedImage();
	// 현재 Grabbed Image가 존재한다면 ...
	if (pstGrab->grab_size > 0 && pstGrab->grab_width > 0 && pstGrab->grab_height)
	{
		// 동기화 진입
		if (m_syncGrab.Enter())
		{
			/* Model Find */
			bFinded	= uvMIL_RunModelStep(0x01, pstGrab->grab_data,
										 pstGrab->grab_width, pstGrab->grab_height,
										 count, angle, results);
			if (bFinded)
			{
				for (i=0; i<count; i++)
				{
					results[i].marked	= bFinded ? 0x01 /* success */ : 0x02 /* failed */;
				}
			}
			// 동기화 해제
			m_syncGrab.Leave();
		}
	}

	return bFinded;
}

/*
 desc : Examination (Vision Align 검사 (측정)) Image가 존재하면, Align Shutting 검사 수행
 parm :	score		- [in]  크기 비율 값 (이 값보다 크거나 같으면 Okay)
		scale		- [in]  매칭 비율 값 (1.0 기준으로 +/- 이 값 범위 안에 있으면 Okay)
		results		- [out] 검색된 결과 정보가 저장될 구조체 참조 포인터 (grabbed image buffer까지 복사해옴)
 retn : TRUE or FALSE
*/
BOOL CCamThread::RunModelExam(DOUBLE score, DOUBLE scale, LPG_ACGR results)
{
	UINT16 i		= 0;
	BOOL bFinded	= FALSE;
	LPG_ACGR pstGrab= NULL;

	/* 반드시 설정 */
	for (i=0; i<2 && results; i++)	results[i].cam_id	= 0x01;

	/* 현재 Camera 동작 모드가 Calibration 모드인지 확인 */
	if (m_pCamMain->GetCamMode() != ENG_VCCM::en_cali_mode)
	{
		SetError(L"It is not calibration mode");
		return NULL;
	}
	// Calibrated Image가 존재하는 경우만 처리
	pstGrab	= m_pCamMain->GetGrabbedImage();
	// 현재 Grabbed Image가 존재한다면 ...
	if (pstGrab->grab_size > 0 && pstGrab->grab_width > 0 && pstGrab->grab_height)
	{
		// 동기화 진입
		if (m_syncGrab.Enter())
		{
			/* Model Find */
			bFinded	= uvMIL_RunModelExam(pstGrab->grab_data,
										 pstGrab->grab_width, pstGrab->grab_height,
										 score, scale, results);
			if (bFinded)
			{
				for (i=0; i<2 && results; i++)
				{
					results[i].marked	= bFinded ? 0x01 /* success */ : 0x02 /* failed */;
				}
			}
			// 동기화 해제
			m_syncGrab.Leave();
		}
	}

	return bFinded;
}

/*
 desc : Edge Image가 존재하면, Align Mark 수행
 parm :	cam_id	- [in]  Align Camera ID (1 or 2)
		saved	- [in]  분석된 edge image 저장 여부
 retn : TRUE or FALSE
*/
BOOL CCamThread::RunEdgeDetect(UINT8 cam_id, UINT8 saved)
{
	BOOL bFinded		= FALSE;

	/* 현재 Camera 동작 모드가 Calibration 모드인지 확인 */
	if (m_pCamMain->GetCamMode() != ENG_VCCM::en_edge_mode)
	{
		SetError(L"It is not edge detection mode");
		return NULL;
	}
	// Calibrated Image가 존재하는 경우만 처리
	m_pstGrabbed	= m_pCamMain->GetGrabbedImage();
	// 현재 Grabbed Image가 존재한다면 ...
	if (m_pstGrabbed->grab_size > 0 && m_pstGrabbed->grab_width > 0 && m_pstGrabbed->grab_height)
	{
		// 동기화 진입
		if (m_syncGrab.Enter())
		{
			bFinded	= uvMIL_RunEdgeDetect(cam_id, m_pstGrabbed, saved);
#ifdef _DEBUG
			if (bFinded)
			{
#if 0
				TRACE(L"Mark Edge <Detection> = %d find (%d) (%.3f%%) \n",
					  GetTickCount64() - u64Tick, bFinded, dbDiaMeter);
#endif
			}
#endif // DEBUG
			// 동기화 해제
			m_syncGrab.Leave();
		}
	}

	return bFinded;
}

/*
 desc : Matching Parameter 설정
 parm : rotate	- [in]  Rotate 사용 여부
		cornor	- [in]  Cornor 그리기 여부
		invert	- [in]  영상 이진화 적용 여부
		rate	- [in]  Matching Rate (단위: %)
 retn : None
*/
VOID CCamThread::SetMatchParam(UINT8 rotate, DOUBLE match)
{
	m_u8MatchingRotate	= rotate;
	m_dbMatchingRate	= match;
}

/*
 desc : 카메라 내부의 이미지 버퍼에 Single Grabbed Image 저장
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamThread::RunGrabbedImage()
{
	return m_pCamMain->RunGrabbedImage();
}

/*
 desc : 연결 상태 정보 갱신
 parm : flag	- [in]  0x00: 연결 해제 상태, 0x01: 연결된 상태
 retn : None
*/
VOID CCamThread::UpdateLinkTime(UINT8 flag)
{
	BOOL bUpate	= FALSE;

	/* 현재 연결 해제 상태이고, 이전에 연결된 상태라면 */
	if (!flag && m_pstLink->is_connected)		bUpate	= TRUE;
	/* 현재 연결 상태이고, 이전에 해제된 상태라면 */
	else if (flag && !m_pstLink->is_connected)	bUpate	= TRUE;

	/* 이전 상태와 다른 경우라면, 정보 변경 */
	if (bUpate)
	{
		SYSTEMTIME stLinkTime;
		GetLocalTime(&stLinkTime);
		m_pstLink->is_connected	= flag;
		m_pstLink->link_time[0]	= stLinkTime.wYear;
		m_pstLink->link_time[1]	= stLinkTime.wMonth;
		m_pstLink->link_time[2]	= stLinkTime.wDay;
		m_pstLink->link_time[3]	= stLinkTime.wHour;
		m_pstLink->link_time[4]	= stLinkTime.wMinute;
		m_pstLink->link_time[5]	= stLinkTime.wSecond;
		m_pstLink->last_sock_error	= 0;
	}
}