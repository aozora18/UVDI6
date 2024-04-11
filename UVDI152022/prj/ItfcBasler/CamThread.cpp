
/*
 desc : TCP/IP Client Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "CamThread.h"
#include "CamMain.h"
#include <timeapi.h>

#include "../UVDI15/GlobalVariables.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 desc : 생성자
 parm : cam_count	- [in]  카메라 개수
		max_grab	- [in]  카메라 마다 한 번에 캡처할 수 이미지의 최대 개수
		link		- [in]  카메라 연결 상태 정보
		cam_main	- [in]  Camera Control Object
 retn : None
*/
CCamThread::CCamThread(UINT8 cam_count, UINT8 max_grab, STG_DLSM **link, CCamMain **cam_main, UINT8 img_proc)
{
	m_u8CamNext			= 0x00;
	m_u8MatchingRotate	= 0x00;
	m_dbMatchingRate	= 80.0f;

	m_u64AttachTime		= 0;
	m_u8CamCount		= cam_count;
	m_u8MaxGrab			= max_grab;
	m_u8SizeMul			= 4;	/* 기본 값은 4배로 Grabbed Image와 Template Image의 크기를 키움 */
	m_pstLink			= link;
	m_pstGrabbed		= NULL;
	m_u8ImgProc			= img_proc;

	/* 가장 최근에 Grabbed 결과에 대해 얼라인 카메라 별로 저장하기 위함 임시 버퍼 */
	m_pstGrabACam		= new STG_ACLR[cam_count];
	ASSERT(m_pstGrabACam);
	memset(m_pstGrabACam, 0x00, sizeof(STG_ACLR) * UINT32(cam_count));

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
	if (m_pstGrabACam)	delete [] m_pstGrabACam;
}

/*
 desc : 스레드 실행시 초기 한번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamThread::StartWork()
{
	return TRUE;
}

/*
 desc : 스레드 실행시 주기적 호출됨
 parm : None
 retn : None
*/
VOID CCamThread::RunWork() 
{
	UINT8 u8CamID	= (m_u8CamNext % m_u8CamCount);

	/* 현재 카메라가 연결되어 있는지 확인 */
	if (!m_pCamMain[u8CamID]->IsConnected())
	{
		/* 연결 해제 상태 */
		UpdateLinkTime(u8CamID+1, 0x00);

		/* 기존 연결되어 있는 카메라 연결 강제로 해제 시킴 */
		m_pCamMain[u8CamID]->DetachDevice();
		/* 특정 시간 마다 주기적으로 Open 수행 */
		if (m_u64AttachTime+1000 < GetTickCount64())
		{
			/* 연결 시도 */
			m_pCamMain[u8CamID]->AttachDevice();
			m_u64AttachTime	= GetTickCount64();
		}
	}
	else
	{
		/* 연결 완료 상태 */
		UpdateLinkTime(u8CamID+1, 0x01);

		/* 연결되어 있다면, 현재 Grabbed Image가 존재하는지 확인 후, 검색 진행 */
		if (ENG_VCCM::en_grab_mode == m_pCamMain[u8CamID]->GetCamMode())
		{
			ProcGrabbedImage(u8CamID+1, m_pCamMain[u8CamID]->GetDispType(), m_u8ImgProc);
		}
	}

	/* 등록된 카메라 대수보다 크면, 0 값으로 초기화 수행 */
	if (++m_u8CamNext == m_u8CamCount)	m_u8CamNext	= 0x00;
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
 desc : 얼라인 카메라 부분만 재접속 수행
 parm : None
 retn : None
*/
VOID CCamThread::Reconnected()
{
	UINT8 i	= 0x00;
	if (m_pCamMain)
	{
		for (; i<m_u8CamCount; i++)	m_pCamMain[i]->DetachDevice();
	}
}

/*
 desc : 카메라 연결 여부 상태 반환
 parm :	cam_id	- [in]  Camera Index (1 or 2)
 retn : TRUE or FALSE
*/
BOOL CCamThread::IsConnected(UINT8 cam_id)
{
	if (!m_pCamMain[cam_id-1])	return FALSE;
	return m_pCamMain[cam_id-1]->IsConnected();
}

/*
 desc : 기존 Grabbed 이미지 모두 제거
 parm : None
 retn : None
*/
VOID CCamThread::ResetGrabbedImage()
{
	/* 동기 진입 */
	if (m_syncGrab.Enter())
	{
		m_lstGrab.RemoveAll();
		/* 동기 해제 */
		m_syncGrab.Leave();
	}
}

/*
 desc : Grabbed Image가 존재하면, Align Mark 수행
 parm :	cam_id	- [in]  Camera Index (1 or 2)
 retn : None
*/
VOID CCamThread::ProcGrabbedImage(UINT8 cam_id, UINT8 dlg_id, UINT8 img_proc)
{
	BOOL bFinded		= FALSE;
	LPG_ACGR pstGrab	= {NULL};

	/* 동기화 진입 */
	if (m_syncGrab.Enter())
	{
		// Grabbed Image가 존재하는 경우만 처리
		pstGrab	= m_pCamMain[cam_id-1]->GetGrabbedImage();
		
		bool globalGrab = true;
		// 현재 Grabbed Image가 존재한다면 ...
		if (pstGrab && pstGrab->grab_data)
		{
			//여기서 또 바꿔줘야하네 ...
			ENG_AMOS alignMotion = alignMotionPtr->markParams.alignMotion;

			switch (alignMotion)
			{
				case ENG_AMOS::en_onthefly_2cam:
				{
					const int SIDE_2CAM_NUM = 2;
					globalGrab = pstGrab->img_id < SIDE_2CAM_NUM ? true : false;
					bFinded = uvMIL_RunModelFind(pstGrab->cam_id, pstGrab->img_id, pstGrab->img_id, pstGrab->grab_data, dlg_id, pstGrab->img_id < SIDE_2CAM_NUM ? GLOBAL_MARK : LOCAL_MARK, FALSE, img_proc); // global mark
				}
				break;

				case ENG_AMOS::en_static_3cam:
				{
					globalGrab = alignMotionPtr->status.globalMarkCnt - 1 > pstGrab->img_id ? true : false;
					bFinded = uvMIL_RunModelFind(pstGrab->cam_id, pstGrab->img_id, pstGrab->img_id, pstGrab->grab_data, dlg_id, globalGrab == true ? GLOBAL_MARK : LOCAL_MARK, FALSE, img_proc); // global mark
				}
				break;

				default:
				{
					throw ACCESS_EXCEPTION("not implement.");
				}
				break;
			}

			/* Get the search result value of mark regardless of success or failure of the search */
			pstGrab	= uvMIL_GetLastGrabbedMark();

			if (!pstGrab)	LOG_ERROR(ENG_EDIC::en_basler, L"The number of images captured has been exceeded");
			else
			{
				pstGrab->grabTime = timeGetTime();
				pstGrab->fiducialMarkIndex = -1818;
				pstGrab->marked = UINT8(bFinded);


				if ((UINT8)m_lstGrab.GetCount() < m_u8MaxGrab)
				{
					m_lstGrab.AddTail(pstGrab);
					pstGrab->fiducialMarkIndex = alignMotionPtr == NULL ? -1818 : alignMotionPtr->GetFiducialIndex(cam_id, globalGrab, &m_lstGrab);
				}
				else
				{
					LOG_ERROR(ENG_EDIC::en_basler, L"The number of images captured has been exceeded");
				}
			}
		}
		/* 동기화 해제 */
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
	STG_ACGR stGrab;
	stGrab.Init();
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
UINT16 CCamThread::GetGrabbedCount(int* camNum)
{
	UINT16 u16Count	= 0;

	/* 동기화 진입 */
	if (m_syncGrab.Enter())
	{

		u16Count = camNum == nullptr ? (UINT16)m_lstGrab.GetCount() : [&]()->int
	{
		int grab = 0;

		for (int i = 0; i < m_lstGrab.GetCount(); i++)
		{
			auto val = m_lstGrab.GetAt(m_lstGrab.FindIndex(i));
			grab = val->cam_id == *camNum ? grab + 1: grab;
		}
		return grab;
	}();

		/* 동기화 해제 */
		m_syncGrab.Leave();
	}

	return u16Count;
}

/*
 desc : Grabbed Images 모두 Score 값이 유효한지 값 반환
 parm :	set_score	- [in]  If the score of the searched mark is higher than this set score, it is valid
 							(If 0.0f is unchecked) 
 retn : TRUE : 유효함, FALSE : 유효하지 않음
*/
BOOL CCamThread::IsScoreValidAll(DOUBLE set_score)
{
	BOOL bValid			= TRUE;
	POSITION pPos		= NULL;
	LPG_ACGR pstGrab	= {NULL};

	/* 동기화 진입 */
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.GetHeadPosition();
		if (!pPos)	bValid	= FALSE;
		else
		{
			while (pPos && bValid)
			{
				pstGrab	= m_lstGrab.GetNext(pPos);
				if (pstGrab)
				{
					/* Score 값이 유효한지 확인 */
					if (!pstGrab->valid_score)	bValid	= FALSE;
					else
					{
						if (set_score > pstGrab->score_rate)	bValid = FALSE;
					}
				}
			}
		}
		/* 동기화 해제 */
		m_syncGrab.Leave();
	}

	return bValid;
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

	/* 동기화 진입 */
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.GetHeadPosition();
		while (!bFinded && pPos)
		{
			pPrePos	= pPos;
			pstGrab	= m_lstGrab.GetNext(pPos);
			if (pstGrab)
			{
				bFinded	= pstGrab->cam_id == grab->cam_id && pstGrab->img_id == grab->img_id;
				if (bFinded)	memcpy(pstGrab, grab, sizeof(STG_ACGR) - sizeof(PUINT8));
			}
		}
		/* 동기화 해제 */
		m_syncGrab.Leave();
	}

	return bFinded;
}

/*
 desc : Grabbed Image의 데이터 반환
 parm : index	- [in]  가져오고자 하는 위치 (Zero based)
 retn : TRUE or FALSE
*/
LPG_ACGR CCamThread::GetGrabbedMarkIndex(UINT8 index)
{
	POSITION pPos	= NULL;
	LPG_ACGR pstGrab= NULL;

	/* 동기 진입 */
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.FindIndex(index);
		if (pPos)	pstGrab	= m_lstGrab.GetAt(pPos);
#ifdef _DEBUG
		if (!pstGrab)
		{
			INT32 i = 0;
			i++;
		}
#endif
		/* 동기 해제 */
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

	/* 동기 진입 */
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.FindIndex(index);
		if (pPos)
		{
			pstGrab	= m_lstGrab.GetAt(pPos);
			memcpy(pstGrab, grab, sizeof(STG_ACGR)-sizeof(PUINT8));
			bFinded	= TRUE;
		}
		/* 동기 해제 */
		m_syncGrab.Leave();
	}

	return bFinded;
}

/*
 desc : Mark Search가 모두 정상적으로 이루어 졌는지 여부
 parm : multi_mark	- [in]  Multi Mark (다중 마크) 적용 여부
		set_score	- [in]  If the score of the searched mark is higher than this set score, it is valid
							(If 0.0f is unchecked)
 retn : TRUE or FALSE
*/
BOOL CCamThread::IsGrabbedMarkValidAll(BOOL multi_mark, DOUBLE set_score,int* camNum)
{
	BOOL bIsValid	= TRUE;
	POSITION pPos	= NULL;
	LPG_ACGR pstGrab= {NULL};

	/* 동기 진입 */
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.GetHeadPosition();
		if (!pPos)	bIsValid	= FALSE;
		else
		{
			while (bIsValid && pPos)
			{
				pstGrab	= m_lstGrab.GetNext(pPos);
				if (camNum != nullptr && pstGrab->cam_id != *camNum)
					continue;

				bIsValid= pstGrab->IsMarkValid(multi_mark) && pstGrab->marked;
				if (bIsValid)
				{
					if (set_score > 0.0f && set_score > pstGrab->score_rate)	bIsValid	= FALSE;
				}
			}
		}
		/* 동기 해제 */
		m_syncGrab.Leave();
	}

	return bIsValid;
}

/*
 desc : Grabbed Image의 데이터 반환
 parm : cam_id	- [in]  카메라 번호 (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
		data	- [out] 반환될 구조체 참조 변수
 retn : TRUE or FALSE
*/
LPG_ACGR CCamThread::GetGrabbedMark(UINT8 cam_id, UINT8 img_id)
{
	POSITION pPos	= NULL;
	LPG_ACGR pstGrab= NULL;

	/* 동기 진입 */
	if (m_syncGrab.Enter())
	{
		pPos = m_lstGrab.GetHeadPosition();
		while (pPos)
		{
			pstGrab	= m_lstGrab.GetNext(pPos);
			if (pstGrab->cam_id == cam_id && pstGrab->img_id == img_id)	break;
			/* 찾지 못했으므로, 반드시 NULL 시켜야 됨 */
			pstGrab	= NULL;
		}
		/* 동기 해제 */
		m_syncGrab.Leave();
	}

	return pstGrab;
}

/*
 desc : Grabbed Image의 데이터 반환
 parm : cam_id	- [in]  카메라 번호 (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
		data	- [out] 반환될 구조체 참조 변수
 retn : TRUE or FALSE
*/
CAtlList <LPG_ACGR>* CCamThread::GetGrabbedMarkAll()
{
	return GetGrabImage();
}

/*
 desc : Grabbed Image가 존재하면, Align Mark 수행
 parm :	cam_id	- [in]  Camera Index (1 or 2)
		mode	- [in]  0xff : Calibration Mode, 0xfe : Align Camera Angle Measuring Mode
 retn : Calirbated Image의 정보가 저장된 구조체 포인터 반환
*/
LPG_ACGR CCamThread::RunModelFind(UINT8 cam_id, UINT8 mode, UINT8 dlg_id, UINT8 mark_no, BOOL useMilDisp, UINT8 img_proc , int flipDir)
{
	BOOL bFinded	= FALSE;
#if 0
	/* 현재 Camera 동작 모드가 Calibration 모드인지 확인 */
	if (m_pCamMain[cam_id-1]->GetCamMode() != ENG_VCCM::en_cali_mode)
	{
		LOG_ERROR(L"It is not model find mode.");
		return NULL;
	}
#endif
	// Calibrated Image가 존재하는 경우만 처리
	m_pstGrabbed	= m_pCamMain[cam_id-1]->GetGrabbedImage();
	if (!m_pstGrabbed)
		return NULL;
	// 현재 Grabbed Image가 존재한다면 ...
	if (m_pstGrabbed->grab_size > 0 && m_pstGrabbed->grab_width> 0 && m_pstGrabbed->grab_height)
	{
		/* 동기화 진입 */
		if (m_syncGrab.Enter())
		{
			// lk91 임시 주석!!
			//uvMIL_RegistMILImg(m_pstGrabbed->cam_id - 1, m_pstGrabbed->grab_width, m_pstGrabbed->grab_height, m_pstGrabbed->grab_data);
			//int tmpDispType;
			//if (dlg_id == DISP_TYPE_MARK_LIVE || dlg_id == DISP_TYPE_MARK) {
			//	tmpDispType = DISP_TYPE_MARK_LIVE;
			//}
			//else {
			//	tmpDispType = dlg_id;
			//}

			uvMIL_DrawImageBitmapFlip(dlg_id, m_pstGrabbed->cam_id - 1, m_pstGrabbed, 0.0, m_pstGrabbed->cam_id - 1, flipDir); // lk91 img_id 가 0부터 시작하는지 체크

			/* Model Find */
			bFinded	= uvMIL_RunModelFind(m_pstGrabbed->cam_id,
										 mode,	/* In case of calibration mode, the value is fixed */
											m_pstGrabbed->img_id, m_pstGrabbed->grab_data, dlg_id, mark_no, useMilDisp, img_proc);
			/* 일단, 최근 측정된 얼라인 카메라의 Grabbed 정보가 잘못되었다고 플래그 설정 */
			m_pstGrabACam[cam_id-1].marked	= 0x00;
			if (bFinded)
			{
				/* 검색 성공된 Mark 정보 얻기 */
				if (uvMIL_GetLastGrabbedMark())	m_pstGrabbed = uvMIL_GetLastGrabbedMark();
				m_pstGrabbed->marked = bFinded ? 0x01 /* success */ : 0x02 /* failed */;
				/* 최근 얼라인 카메라의 Grabbed 결과 값 갱신 */
				m_pstGrabACam[cam_id - 1].marked = m_pstGrabbed->marked;
				m_pstGrabACam[cam_id - 1].move_mm_x = m_pstGrabbed->move_mm_x;
				m_pstGrabACam[cam_id - 1].move_mm_y = m_pstGrabbed->move_mm_y;
			}

			/* 동기화 해제 */
			m_syncGrab.Leave();
		}
	}

	return m_pstGrabbed;
}

/*
 desc : Step (Vision 단차) Image가 존재하면, Align Mark 수행
 parm :	cam_id	- [in]  Camera Index (1 or 2)
		count	- [in]  검색 대상의 Mark 개수 (찾고자하는 마크 개수)
		angle	- [in]  각도 적용 여부 (TRUE : 각도 적용, FALSE : 각도 적용 없음)
						TRUE : 현재 카메라의 회전된 각도 구하기 위함, FALSE : 기존 각도 적용하여 회전된 각도 구함
		results	- [out] 검색된 결과 정보가 저장될 구조체 참조 포인터 (grabbed image buffer까지 복사해옴)
 retn : TRUE or FALSE
*/
BOOL CCamThread::RunModelStep(UINT8 cam_id, UINT16 count, BOOL angle, LPG_ACGR results, UINT8 dlg_id, UINT8 mark_no, UINT8 img_proc)
{
	UINT16 i		= 0;
	BOOL bFinded	= FALSE;
	LPG_ACGR pstGrab= NULL;

	/* 반드시 설정 */
	for (i=0; i<count; i++)	results[i].cam_id	= cam_id;
	//results.cam_id = cam_id;

	/* 현재 Camera 동작 모드가 Calibration 모드인지 확인 */
	if (m_pCamMain[cam_id-1]->GetCamMode() != ENG_VCCM::en_cali_mode)
	{
		LOG_ERROR(ENG_EDIC::en_basler, L"It is not model step mode");
		return NULL;
	}
	// Calibrated Image가 존재하는 경우만 처리
	pstGrab	= m_pCamMain[cam_id-1]->GetGrabbedImage();
	// 현재 Grabbed Image가 존재한다면 ...
	if (pstGrab->grab_size > 0 && pstGrab->grab_width > 0 && pstGrab->grab_height)
	{
		/* 동기화 진입 */
		if (m_syncGrab.Enter())
		{
			uvMIL_RegistMILImg(pstGrab->cam_id - 1, pstGrab->grab_width, pstGrab->grab_height, pstGrab->grab_data);
			int tmpDispType;
			if (dlg_id == DISP_TYPE_MARK_LIVE || dlg_id == DISP_TYPE_MARK) {
				tmpDispType = DISP_TYPE_MARK_LIVE;
			}
			else {
				tmpDispType = dlg_id;
			}
			uvMIL_DrawImageBitmap(tmpDispType, pstGrab->cam_id - 1, pstGrab, 0.0, pstGrab->cam_id - 1); // lk91 img_id 가 0부터 시작하는지 체크


			/* Model Find */
			bFinded = uvMIL_RunModelStep(cam_id, pstGrab->grab_data,
				pstGrab->grab_width, pstGrab->grab_height,
				count, angle, results, pstGrab->img_id, dlg_id, mark_no, img_proc);
			if (bFinded)
			{
				for (i = 0; i < count; i++)
				{
					results[i].marked = bFinded ? 0x01 /* success */ : 0x02 /* failed */;
				}
			}

			/* 동기화 해제 */
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
	UINT16 i = 0;
	BOOL bFinded = FALSE;
	LPG_ACGR pstGrab = NULL;

	/* 반드시 설정 */
	//for (i = 0; i < 2 && results; i++)	results[i].cam_id = 0x01;

	int tgtCamId = results[0].cam_id - 1;
	/* 현재 Camera 동작 모드가 Calibration 모드인지 확인 */
	if (m_pCamMain[tgtCamId]->GetCamMode() != ENG_VCCM::en_cali_mode)
	{
		LOG_ERROR(ENG_EDIC::en_basler, L"It is not model exam mode");
		return NULL;
	}
	// Calibrated Image가 존재하는 경우만 처리
	int timeAccum = 0;
	while (pstGrab == nullptr)
	{
		pstGrab = m_pCamMain[tgtCamId]->GetGrabbedImage();
		timeAccum += 300;
		Sleep(300);
		if (timeAccum > 3000)break;
	}
	
	// 현재 Grabbed Image가 존재한다면 ...
	if (pstGrab->grab_size > 0 && pstGrab->grab_width > 0 && pstGrab->grab_height > 0)
	{
		/* 동기화 진입 */
		if (m_syncGrab.Enter())
		{
			/* Model Find */
			bFinded = uvMIL_RunModelExam2(pstGrab->grab_data,
				pstGrab->grab_width, pstGrab->grab_height,
				score, scale, results);
			if (bFinded)
			{
				for (i = 0; i < 2 && results; i++)
				{
					results[i].marked = bFinded ? 0x01 /* success */ : 0x02 /* failed */;
				}
			}

			if (results[0].marked == true && results[0].marked == results[1].marked)
			{
				if (uvMIL_GetLastGrabbedMark())	m_pstGrabbed = uvMIL_GetLastGrabbedMark();
				m_pstGrabbed->marked = bFinded ? 0x01 /* success */ : 0x02 /* failed */;
				/* 최근 얼라인 카메라의 Grabbed 결과 값 갱신 */
				m_pstGrabACam[0].marked = m_pstGrabbed->marked;
				m_pstGrabACam[0].move_mm_x = m_pstGrabbed->move_mm_x;
				m_pstGrabACam[0].move_mm_y = m_pstGrabbed->move_mm_y;
			}

			/* 동기화 해제 */
			m_syncGrab.Leave();
		}
	}
	else
	{
		int deug = 0;
	}
	return bFinded;
}

/*
 desc : Set the parameters of the strip maker and find in an image and take the specified measurements
 parm : cam_id	- [in]  Align Camera ID (1 or 2 or later)
		param	- [in]  Structures pointer with information to set up is stored
		result	- [out] Buffer in which the resulting value will be returned and stored.
 retn : TRUE or FALSE
*/
BOOL CCamThread::RunMarkerStrip(UINT8 cam_id, LPG_MSMP param, STG_MSMR &results)
{
	UINT16 i		= 0;
	BOOL bFinded	= FALSE;
	LPG_ACGR pstGrab= NULL;

	/* 현재 Camera 동작 모드가 Calibration 모드인지 확인 */
	if (m_pCamMain[0]->GetCamMode() != ENG_VCCM::en_hole_mode)
	{
		LOG_ERROR(ENG_EDIC::en_basler, L"It is not hole measurement mode");
		return NULL;
	}
	// Calibrated Image가 존재하는 경우만 처리
	pstGrab	= m_pCamMain[0]->GetGrabbedImage();
	// 현재 Grabbed Image가 존재한다면 ...
 	if (pstGrab->grab_size > 0 && pstGrab->grab_width > 0 && pstGrab->grab_height > 0)
	{
		/* Enter sync. */
		if (m_syncGrab.Enter())
		{
			/* Model Find */
			bFinded	= uvMIL_RunMarkerStrip(cam_id, pstGrab->grab_data, param, results);
			/* Leave sync */
			m_syncGrab.Leave();
		}
	}

	return bFinded;
}

/*
 desc : Edge Image가 존재하면, Align Mark 수행
 parm :	cam_id	- [in]  Camera Index (1 or 2)
		saved	- [in]  분석된 edge image 저장 여부
 retn : TRUE or FALSE
*/
BOOL CCamThread::RunEdgeDetect(UINT8 cam_id, UINT8 saved)
{
	BOOL bFinded		= FALSE;

	/* 현재 Camera 동작 모드가 Calibration 모드인지 확인 */
	if (m_pCamMain[cam_id-1]->GetCamMode() != ENG_VCCM::en_edge_mode)
	{
		LOG_ERROR(ENG_EDIC::en_basler, L"It is not edge detection mode");
		return NULL;
	}
	// Calibrated Image가 존재하는 경우만 처리
	m_pstGrabbed	= m_pCamMain[cam_id-1]->GetGrabbedImage();
	// 현재 Grabbed Image가 존재한다면 ...
	if (!m_pstGrabbed->IsEmpty())
	{
		/* 동기화 진입 */
		if (m_syncGrab.Enter())
		{
			bFinded	= uvMIL_RunEdgeDetect(cam_id, m_pstGrabbed, saved);

			/* 동기화 해제 */
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
 desc : 연결 상태 정보 갱신
 parm : cam_id	- [in]  Align Camera ID (1 or Later)
		flag	- [in]  0x00: 연결 해제 상태, 0x01: 연결된 상태
 retn : None
*/
VOID CCamThread::UpdateLinkTime(UINT8 cam_id, UINT8 flag)
{
	BOOL bUpate	= FALSE;

	/* 현재 연결 해제 상태이고, 이전에 연결된 상태라면 */
	if (!flag && m_pstLink[cam_id-1]->is_connected)		bUpate	= TRUE;
	/* 현재 연결 상태이고, 이전에 해제된 상태라면 */
	else if (flag && !m_pstLink[cam_id-1]->is_connected)	bUpate	= TRUE;

	/* 이전 상태와 다른 경우라면, 정보 변경 */
	if (bUpate)
	{
		SYSTEMTIME stLinkTime;
		GetLocalTime(&stLinkTime);
		m_pstLink[cam_id-1]->is_connected	= flag;
		m_pstLink[cam_id-1]->link_time[0]	= stLinkTime.wYear;
		m_pstLink[cam_id-1]->link_time[1]	= stLinkTime.wMonth;
		m_pstLink[cam_id-1]->link_time[2]	= stLinkTime.wDay;
		m_pstLink[cam_id-1]->link_time[3]	= stLinkTime.wHour;
		m_pstLink[cam_id-1]->link_time[4]	= stLinkTime.wMinute;
		m_pstLink[cam_id-1]->link_time[5]	= stLinkTime.wSecond;
		m_pstLink[cam_id-1]->last_sock_error= 0;
	}
}


CDPoint CCamThread::RunModel_VisionCalib(UINT8 cam_id, UINT8 dlg_id, UINT8 mark_no, int* roi_left, int* roi_right, int* roi_top, int* roi_bottom, int row, int col)
{
	CDPoint MarkP;
	MarkP.x = 0;
	MarkP.y = 0;
	BOOL bFinded = FALSE;
#if 0
	/* 현재 Camera 동작 모드가 Calibration 모드인지 확인 */
	if (m_pCamMain[cam_id - 1]->GetCamMode() != ENG_VCCM::en_cali_mode)
	{
		LOG_ERROR(L"It is not model find mode.");
		return NULL;
	}
#endif
	// Calibrated Image가 존재하는 경우만 처리
	m_pstGrabbed = m_pCamMain[cam_id - 1]->GetGrabbedImage();
	if (!m_pstGrabbed)
		return MarkP;
	// 현재 Grabbed Image가 존재한다면 ...
	if (m_pstGrabbed->grab_size > 0 && m_pstGrabbed->grab_width > 0 && m_pstGrabbed->grab_height)
	{
		/* 동기화 진입 */
		if (m_syncGrab.Enter())
		{
			uvMIL_DrawImageBitmap(dlg_id, m_pstGrabbed->cam_id - 1, m_pstGrabbed, 0.0, m_pstGrabbed->cam_id - 1); // lk91 img_id 가 0부터 시작하는지 체크

			/* Model Find */
			//bFinded = uvMIL_RunModelFind(m_pstGrabbed->cam_id,
			//	mode,	/* In case of calibration mode, the value is fixed */
			//	m_pstGrabbed->img_id, m_pstGrabbed->grab_data, dlg_id, mark_no, useMilDisp, img_proc);
			uvMIL_RunModel_VisionCalib(m_pstGrabbed->cam_id, m_pstGrabbed->img_id, m_pstGrabbed->grab_data, dlg_id, mark_no, roi_left, roi_right, roi_top, roi_bottom, row, col);

			/* 동기화 해제 */
			m_syncGrab.Leave();
		}
	}

	return MarkP;
}
