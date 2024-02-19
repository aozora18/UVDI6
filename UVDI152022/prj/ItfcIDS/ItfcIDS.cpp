
/*
 desc : IP Camera Communication Libarary (Basler pylon sdk ACA1300-30GM)
*/

#include "pch.h"
#include "MainApp.h"

#include "CamThread.h"
#include "CamMain.h"
#include "DrawMark.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GRAB_DRAW_DELAY_TIME	50	/* 단위: msec */

/* --------------------------------------------------------------------------------------------- */
/*                                           전역 변수                                           */
/* --------------------------------------------------------------------------------------------- */

UINT32						g_u32LastACamSizeBytes	= 0;	/* 가장 최근에 설정된 얼라인 카메라의 해상도 (이미지)를 저장할 버퍼 크기 */

UINT64						g_u64DrawDelayTime		= 0;	/* 연속적으로 화면 출력하는데 한계가 있으므로, 일정 시간 지연 */

LPG_CIEA					g_pstConfig				= NULL;
LPG_VDSM					g_pstShMem				= NULL;

CCamThread					*g_pCamThread			= NULL;
CCamMain					*g_pCamMain				= NULL;
CDrawMark					g_csDrawMark;


/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 기존 Grabbed 이미지 모두 제거
 parm : None
 retn : None
*/
VOID ResetGrabbedImage()
{
	g_pCamMain->ResetGrabbedImage();
	g_pCamThread->ResetGrabbedImage();

	// Match Template Image 메모리 해제
	uvMIL_ResetGrabAll();
}

/*
 desc : Global 4개의 Mark에 대해서 모두 유효한지 검사
 변수 :	img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : TRUE (유효함) or FALSE (4개 중 1개라도 제대로 인식 안됨)
*/
BOOL IsValidGrabMarkNum(UINT8 img_id)
{
	LPG_ACGR pstGrab;

	if (!g_pCamThread)	return FALSE;
	pstGrab	= g_pCamThread->GetGrabbedMark(img_id);
	if (!pstGrab->valid_scale || !pstGrab->valid_score)	return FALSE;

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                           외부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/*
 desc : BIPS Server와 통신을 담당하는 Client 스레드 생성 및 실행
 parm : config		- [in]  환경 정보
		shmem		- [in]  Shared Memory
		work_dir	- [in]  작업 경로 (실행 파일 경로)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_Init(LPG_CIEA config, LPG_VDSM shmem, PTCHAR work_dir)
{
	g_pstConfig	= config;
	g_pstShMem	= shmem;

	/* Vision : MIL / Cognex / OpenCV / Halcon */
	if (!config->use_libs.use_vision_kind)	return TRUE;
	switch (config->use_libs.use_vision_kind)
	{
	case ENG_VLPK::en_vision_opencv	:	return FALSE;	break;
	case ENG_VLPK::en_vision_mil		:	if (!uvMIL_Init(config, shmem, work_dir))	return FALSE;	break;
	case ENG_VLPK::en_vision_cognex	:	return FALSE;	break;
	case ENG_VLPK::en_vision_halcon	:	return FALSE;	break;
	}

	/* 환경 파일에 설정된 얼라인 카메라 해상도의 크기를 저장해 놓음 (단위: Bytes) */
	g_u32LastACamSizeBytes	= g_pstConfig->set_cams.GetCamSizeBytes();
	/* Grabbed Image 수신 및 저장 관리 클래스 생성 */
	g_pCamMain	= new CCamMain(config);
	ASSERT(g_pCamMain);
	/* Grabbed Image 처리하는 스레드 (IDS의 경우, 무조건 카메라 1개 이므로) */
	g_pCamThread	= new CCamThread(shmem->link[0], g_pCamMain);
	ASSERT(g_pCamThread);
	if (!g_pCamThread->CreateThread(0, 0, NORMAL_THREAD_SPEED))	return FALSE;

	return TRUE;
}

/*
 desc : BIPS Server와 통신을 담당하는 Client 스레드 생성 및 실행
 parm : config		- [in]  환경 정보
		shmem		- [in]  Shared Memory
		work_dir	- [in]  작업 경로 (실행 파일 경로)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_Open(LPG_CIEA config, LPG_VDSM shmem)
{
	g_pstConfig	= config;
	g_pstShMem	= shmem;

	return TRUE;
}

/*
 desc : 라이브러리 해제
 parm : None
 retn : None
*/
API_EXPORT VOID uvIDS_Close()
{
	UINT8 i	= 0;

	// IP Camera와 통신을 담당하는 객체 메모리 해제
	if (g_pCamThread)
	{
		/* Grabbed Image 매칭 관련 스레드 중지 및 메모리 해제 */
		g_pCamThread->Stop();
		while (g_pCamThread->IsBusy())	g_pCamThread->Sleep(100);
		delete g_pCamThread;
	}
	/* Grabbed Image 관리 객체 메모리 해제 */
	delete g_pCamMain;
	g_pCamMain	= NULL;

	/* Vision : MIL / Cognex / OpenCV / Halcon */
	if (!g_pstConfig->use_libs.use_vision_kind)	return;
	switch (g_pstConfig->use_libs.use_vision_kind)
	{
	case ENG_VLPK::en_vision_opencv	:	break;
	case ENG_VLPK::en_vision_mil	:	uvMIL_Close();
	case ENG_VLPK::en_vision_cognex	:	break;
	case ENG_VLPK::en_vision_halcon	:	break;
	}
}

/*
 desc : 얼라인 카메라 부분만 재접속 수행
 parm : None
 retn : None
*/
API_EXPORT VOID uvIDS_Reconnected()
{
	g_pCamThread->Reconnected();
	uvMIL_ResetGrabAll();
}

/*
 desc : 에러 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvIDS_ResetError()
{
	g_pCamMain->ResetError();
}

/*
 desc : Mark Pattern 생성 윈도 핸들
 parm : hwnd	- [in]  마크 생성 윈도 핸들
 retn : None
*/
API_EXPORT VOID uvIDS_SetMarkHwnd(HWND hwnd)
{
	g_csDrawMark.SetDrawHwnd(hwnd);
}

/*
 desc : BIPS Server와 통신을 담당하는 Client 스레드 생성 및 실행
 parm : time_out	- [in]  기존 카메라와의 연결해제가 비정상적으로 종료되었을 때,
							카메라 내부에서 기존 연결된 클라이언트의 응답이 없을 때, 해제 대기 시간
							클라이언트로부터 임의 설정 시간(단위: 밀리초)동안 응답 없으면, 강제 해제
		work_dir	- [in]  작업 경로 (실행 파일 경로)
		run_mode	- [in]  0x01 : CMPS, 0x01 : Vision Step
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_UpdateSetParam(TCHAR *work_dir, ENG_ERVM run_mode, UINT64 time_out)
{
	if (g_u32LastACamSizeBytes != g_pstConfig->set_cams.GetCamSizeBytes())
	{
		/* 기존 영상 라이브러리 해제 및 초기화 수행 */
		uvMIL_Close();
		/* 영상 라이브러리 초기화 */
		if (!uvMIL_Init(g_pstConfig, g_pstShMem, work_dir))	return FALSE;
		/* 가장 최근에 변경된 얼라인 카메라 해상도의 크기 값 다시 저장 */
		g_u32LastACamSizeBytes	= g_pstConfig->set_cams.GetCamSizeBytes();
	}

	return g_pCamMain->SetParamUpdate();
}

/*
 desc : Mark Pattern 출력
 parm : inner	- [in]  내부 크기 (단위: pixel)
		outer	- [in]  외부 크기 (단위: pixel)
		reverse	- [in]  흑백 반전 여부 (0x00 - In:White/Out:Black, 0x01 - In:Black/Out:White)
		pattern	- [in]  마크 패턴 타입 (0x00 - Circle, 0x01 - Rectangle, 0x02 - Cross)
 retn : None
*/
API_EXPORT VOID uvIDS_CreateMark(LPRECT inner, LPRECT outer, UINT8 reverse, UINT8 pattern)
{
	g_csDrawMark.Draw(inner, outer, reverse, pattern);
}

/*
 desc : 마크 그리기
 parm : file	- [in]  저장 파일 (전체 경로 포함)
		inner	- [in]  내부 크기 (단위: pixel)
		outer	- [in]  외부 크기 (단위: pixel)
		reverse	- [in]  흑백 반전 여부 (0x00 - In:White/Out:Black, 0x01 - In:Black/Out:White)
		pattern	- [in]  마크 패턴 타입 (0x00 - Circle, 0x01 - Rectangle, 0x02 - Cross)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SaveMark(TCHAR *file, LPRECT inner, LPRECT outer, UINT8 reverse, UINT8 pattern)
{
	return g_csDrawMark.Save(file, inner, outer, reverse, pattern);
}

/*
 desc : Vision Camera Mode 설정
 parm : mode	- [in]  ENG_VCCM::en_xxx
 retn : None
*/
API_EXPORT VOID uvIDS_SetCamMode(ENG_VCCM mode)
{
	if (g_pCamMain)
	{
		g_pCamMain->SetCamMode(mode);
	}
}

/*
 desc : 현재 Camera의 동작 모드 값 반환
 parm : None
 retn : ENG_VCCM::en_xxx
*/
API_EXPORT ENG_VCCM uvIDS_GetCamMode()
{
	return g_pCamMain == NULL ? ENG_VCCM::en_none : g_pCamMain->GetCamMode();
}

/*
 desc : 현재 PLC가 연결되어 있는지 여부 확인
 변수 :	None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_IsConnected()
{
	if (!g_pCamThread || !g_pCamMain)	return FALSE;

	return g_pCamMain->IsConnected();
}

/*
 desc : 현재 모든 Align Camera가 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_IsConnectedAll()
{
	if (!g_pCamMain)				return FALSE;
	if (!g_pCamMain->IsConnected())	return FALSE;

	return TRUE;
}

/*
 desc : 현재 등록된 Grabbed 이미지 개수 반환
 parm : None
 retn : 저장된 이미지 개수 반환
*/
API_EXPORT UINT16 uvIDS_GetGrabbedCount()
{
	return g_pCamThread->GetGrabbedCount();
}

/*
 desc : Grabbed Images 모두 Score 값이 유효한지 값 반환
 parm : None
 retn : TRUE : 유효함, FALSE : 유효하지 않음
*/
API_EXPORT BOOL uvIDS_IsScoreValidAll()
{
	return !g_pCamThread ? FALSE : g_pCamThread->IsScoreValidAll();
}

/*
 desc : Grabbed Image의 결과 반환
 parm : img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : grab data 반환
*/
API_EXPORT LPG_ACGR uvIDS_GetGrabbedFind(UINT8 img_id)
{
	if (!g_pCamThread)	return FALSE;
	return g_pCamThread->GetGrabbedMark(img_id);
}

/*
 desc : Grabbed Image의 결과 수정
 parm : grab	- [in]  저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SetGrabbedFind(LPG_ACGR grab)
{
	if (!g_pCamThread || grab->cam_id < 1)	return FALSE;
	return g_pCamThread->SetGrabbedMark(grab);
}

/*
 desc : Grabbed Image의 데이터 반환
 parm : index	- [in]  가져오고자 하는 위치 (Zero based)
		data	- [out] 저장될 구조체 참조 변수
 retn : grab data 반환
*/
API_EXPORT LPG_ACGR uvIDS_GetGrabbedFindIndex(UINT8 index)
{
	return !g_pCamThread ? NULL : g_pCamThread->GetGrabbedMarkIndex(index);
}

/*
 desc : Grabbed Image의 데이터 수정
 parm : index	- [in]  가져오고자 하는 위치 (Zero based)
 parm : grab	- [in]  저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SetGrabbedFindIndex(UINT8 index, LPG_ACGR grab)
{
	return !g_pCamThread ? FALSE : g_pCamThread->SetGrabbedMarkIndex(index, grab);
}

/*
 desc : Global 4개의 Mark에 대해서 모두 유효한지 검사
 parm : None
 retn : TRUE (유효함) or FALSE (4개 중 1개라도 제대로 인식 안됨)
*/
API_EXPORT BOOL uvIDS_IsGrabbedMarkValidAll()
{
	return g_pCamThread->IsGrabbedMarkValidAll();
}

/*
 desc : Global 4개의 Mark에 대해서 모두 유효한지 검사
 변수 :	img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : TRUE (유효함) or FALSE (4개 중 1개라도 제대로 인식 안됨)
*/
API_EXPORT BOOL uvIDS_IsValidGrabMarkNum(UINT8 img_id)
{
	LPG_ACGR pstGrab	= NULL;

	if (!g_pCamThread)	return FALSE;
	pstGrab	= g_pCamThread->GetGrabbedMark(img_id);
	if (!pstGrab)	return FALSE;
#if (USE_MARK_VALID_SCALE)
	if (!pstGrab->valid_scale || !pstGrab->valid_score || !pstGrab->valid_dist)	return FALSE;
#else
	if (!pstGrab->valid_score)	return FALSE;
#endif

	return TRUE;
}

/*
 desc : 기존 Grabbed 이미지 모두 제거
 parm : None
 retn : None
*/
API_EXPORT VOID uvIDS_ResetGrabbedImage()
{
	ResetGrabbedImage();
}

/*
 desc : Calibration Image가 존재하면, Align Mark 수행
 변수 :	mode	- [in]  0xff : Calibration Mode (얼라인 카메라 각도 측정하지 않음)
						0xfe : Align Camera Angle Measuring Mode (얼라인 카메라 각도 측정하기 위함)
 retn : Calirbated Image의 정보가 저장된 구조체 포인터 반환
*/
API_EXPORT LPG_ACGR uvIDS_RunModelCali(UINT8 mode)
{
	/* New Grabbed Image 저장 */
	return g_pCamThread->RunModelCali(mode);
}

/*
 desc : 가장 최근에 Calibrated된 결과 데이터 반환
 parm : None
 retn : 매칭된 결과 값 반환
*/
API_EXPORT LPG_ACGR uvIDS_GetLastGrabbedMark()
{
	if (!g_pCamThread)	return NULL;
	return g_pCamThread->GetLastGrabbedMark();
}

/*
 desc : 가장 최근에 카메라 별로 Calibrated된 결과 중 Error 값이 저장된 데이터 반환
 parm : None
 retn : 매칭된 결과 값 반환
*/
API_EXPORT LPG_ACLR uvIDS_GetLastGrabbedACam()
{
	if (!g_pCamThread)	return NULL;
	return g_pCamThread->GetLastGrabbedACam();
}

/*
 desc : Step (Vision 단차) Image가 존재하면, Align Mark 수행
 변수 :	counts	- [in]  검색 대상의 Mark 개수 (찾고자하는 마크 개수)
		angle	- [in]  각도 적용 여부 (TRUE : 각도 적용, FALSE : 각도 적용 없음)
						TRUE : 현재 카메라의 회전된 각도 구하기 위함, FALSE : 기존 각도 적용하여 회전된 각도 구함
		results	- [out] 검색된 결과 정보가 저장될 구조체 참조 포인터 (grabbed image buffer까지 복사해옴)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_RunModelStep(UINT16 counts, BOOL angle, LPG_ACGR results)
{
	if (!g_pCamThread)	return FALSE;
	return g_pCamThread->RunModelStep(counts, angle, results);
}

/*
 desc : Examination (Vision Align 검사 (측정)) Image가 존재하면, Align Shutting 검사 수행
 변수 :	score		- [in]  크기 비율 값 (이 값보다 크거나 같으면 Okay)
		scale		- [in]  매칭 비율 값 (1.0 기준으로 +/- 이 값 범위 안에 있으면 Okay)
		results		- [out] 검색된 결과 정보가 저장될 구조체 참조 포인터 (grabbed image buffer까지 복사해옴)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_RunModelExam(DOUBLE score, DOUBLE scale, LPG_ACGR results)
{
	if (!g_pCamThread)	return FALSE;
	return g_pCamThread->RunModelExam(score, scale, results);
}

/*
 desc : Grabbed Image가 존재하면, Edge Detection 수행
 변수 :	cam_id	- [in]  Align Camera ID (1 or 2)
		saved	- [in]  분석된 edge image 저장 여부
 retn : Edge Detected된 영역의 직경 (지름) 값 반환 (단위: Pixel)
*/
API_EXPORT BOOL uvIDS_RunEdgeDetect(UINT8 cam_id, UINT8 saved)
{
	/* 기존 Grabbed Image 제거 */
	ResetGrabbedImage();
	/* New Grabbed Image 저장 */
	return g_pCamThread->RunEdgeDetect(cam_id, saved);
}

/*
 desc : Geometric Model Find
 parm : cam_id	- [in]  Camera Index (1 or 2)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_RunModelFind(UINT8 cam_id)
{
	LPG_ACGR pstGrab	= g_pCamThread->GetLastGrabbedMark();
	if (!pstGrab)	return FALSE;
	/* 검색 작업 진행 */
	return uvMIL_RunModelFind(cam_id, pstGrab->img_id, pstGrab->grab_data);

}

/*
 desc : 기존 Grabbed Image 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvIDS_ResetGrabbedMark()
{
	if (!g_pCamMain)	return;
	g_pCamMain->ResetGrabbedMark();
}

/*
 desc : 현재 Grabbed Image에 대한 각 Bytes 별로 저장된 Gray 값 비중 계산
 parm : mode	- [in]  Grabbed Image를 캡처하는 방식 (모드)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SetGrabGrayRate(ENG_VCCM mode)
{
	if (!g_pCamMain)	return FALSE;
	return g_pCamMain->SetGrabGrayRate(mode);
}

/*
 desc : 현재 Grabbed Image에 대한 각 Gray Level 값 (비율) 반환
 parm : index	- [in]  0 ~ 255에 해당되는 Gray Level Index
 retn : 0 ~ 255 들이 Grabbed Image에서 비율 값 반환
*/
API_EXPORT DOUBLE uvIDS_GetGrabGrayRate(UINT8 index)
{
	if (!g_pCamMain)	return 0.0f;
	return g_pCamMain->GetGrabGrayRate(index);
}

/*
 desc : 전체 Grabbed Image에서 색깔이 있는 값의 비율 반환
 parm : None
 retn : White 비율 값 반환
*/
API_EXPORT DOUBLE uvIDS_GetGrabGrayRateTotal()
{
	if (!g_pCamMain)	return 0.0f;
	return g_pCamMain->GetGrabGrayRateTotal();
}

/*
 desc : 현재 Grabbed Image에 대한 각 Bytes 별로 저장된 Gray 값 저장
		넓이에 대해 높이에 있는 모든 Gray 값이 합산하여 저장
 parm : mode	- [in]  Grabbed Image를 캡처하는 방식 (모드)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SetGrabHistLevel(ENG_VCCM mode)
{
	if (!g_pCamMain)	return FALSE;
	return g_pCamMain->SetGrabHistLevel(mode);
}

/*
 desc : 현재 Grabbed Image에 대한 각 Bytes 별로 저장된 Gray Level 반환
 parm : None
 retn : 현재 Grabbed Image의 넓이 (Column)의 1 픽셀마다 높이의 Level 값들이 저장되어 있는 버퍼 반환
*/
API_EXPORT PUINT64 uvIDS_GetGrabHistLevel()
{
	if (!g_pCamMain)	return NULL;
	return g_pCamMain->GetGrabHistLevel();
}

/*
 desc : 현재 Grabbed Image에 대한 각 Bytes 별로 저장된 Gray Level들의 총 합 값 반환
 parm : None
 retn : Grabbed Image의 전체 Gray Level 값들의 합 반환
*/
API_EXPORT UINT64 uvIDS_GetGrabHistTotal()
{
	if (!g_pCamMain)	return 0;
	return g_pCamMain->GetGrabHistTotal();
}

/*
 desc : 현재 Grabbed Image에 대해 Column 별로 Gray Level 값 중 가장 큰 값
 parm : None
 retn : Grabbed Image의 Column 별로 Gray Level 합 중 가장 큰 값 반환
*/
API_EXPORT UINT64 uvIDS_GetGrabHistMax()
{
	if (!g_pCamMain)	return 0;
	return g_pCamMain->GetGrabHistMax();
}


/* --------------------------------------------------------------------------------------------- */
/*                             영상 처리 연결 (Gateway) 함수 부분                                */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 최종 검색된 이미지 (Edge Detect or Line Detect)를 윈도 영역에 출력 수행
 parm : hwnd	- [in]  이미지를 출력할 윈도 핸들
		type	- [in]  0x00: Edge Detection, 0x01: Line Detection
 retn : None
*/
API_EXPORT VOID uvIDS_DrawDetect(HWND hwnd, UINT8 type)
{
	uvMIL_DrawDetect(hwnd, type);
}

/*
 desc : Live 모드일 경우, 현재 Grabbed된 이미지 출력 수행
 변수 :	hwnd	- [in]  이미지가 출력될 윈도 핸들
 retn : None
*/
API_EXPORT VOID uvIDS_DrawLive(HWND hwnd)
{
	LPG_ACGR pstGrab	= NULL;

	if (ENG_VCCM::en_grab_mode != g_pCamMain->GetCamMode())
	{
		/* Check to Grabbed Image Size */
		pstGrab	= g_pCamMain->GetGrabbedImage();
		if (pstGrab->IsEmpty())	return;
		/* 이전에 그렸던 시간과 비교해서 일정 시간 지났는지 확인 */
		if (GetTickCount64() > g_u64DrawDelayTime + GRAB_DRAW_DELAY_TIME)
		{
			/* 현재 Grabbed Image 1장 출력 */
			uvMIL_DrawLive(hwnd, pstGrab);
			/* 다음에 그릴 때, 일정 시간 지연을 위해서 */
			g_u64DrawDelayTime	= GetTickCount64();
		}
	}
}

/*
 desc : Live 모드일 경우, 가장 최근에 Grabbed된 이미지 출력 수행
 변수 :	hwnd	- [in]  이미지가 출력될 윈도 핸들
 retn : None
*/
API_EXPORT VOID uvIDS_DrawLastLive(HWND hwnd)
{
	LPG_ACGR pstGrab	= NULL;

	pstGrab	= g_pCamMain->GetGrabbedLiveImage();
	if (!pstGrab)	return;
	// 현재 Grabbed Image 1장 출력
	if (pstGrab->grab_width && pstGrab->grab_height)
	{
		/* 이전에 그렸던 시간과 비교해서 일정 시간 지났는지 확인 */
		if (GetTickCount64() > g_u64DrawDelayTime + GRAB_DRAW_DELAY_TIME)
		{
			uvMIL_DrawLive(hwnd, pstGrab);
			/* 다음에 그릴 때, 일정 시간 지연을 위해서 */
			g_u64DrawDelayTime	= GetTickCount64();
		}
	}
}

/*
 desc : 가장 최근에 Live로 Grabbed된 이미지 존재 여부
 변수 :	None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_IsGetLastLiveImage()
{
	return g_pCamMain->GetGrabbedLiveImage() ? TRUE : FALSE;
}

/*
 desc : Calibration 이미지 (검색된 결과)를 윈도 영역에 출력 수행
 변수 :	hwnd	- [in]  이미지가 출력될 윈도 핸들
 retn : None
*/
API_EXPORT VOID uvIDS_DrawCaliMark(HWND hwnd)
{
	uvMIL_DrawCaliMark(hwnd);
}
/*
 desc : 검색된 마크 이미지가 포함된 구조체 포인터 반환
 parm : img_id	- [in]  Camera Grabbed Image Index (0 or Later) (if img_id == 0xff then Calibration Method)
 retn : 구조체 포인터 반환
*/
API_EXPORT LPG_ACGR uvIDS_GetGrabbedData(UINT8 img_id)
{
#if 0
	return uvMIL_GetGrabbedData(img_id);
#else
	return g_pCamThread->GetGrabbedMark(img_id);
#endif
}

/*
 desc : 최종 검색된 마크 이미지 윈도 영역에 출력 수행
 변수 :	img_id	- [in]  Camera Grabbed Image Index (0 or Later)
		hwnd	- [in]  이미지가 출력될 윈도 핸들
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_DrawMark(UINT8 img_id, HWND hwnd)
{
	UINT8 u8FindMark	= 0x00;

	/* 검색된 Mark가 유효한지 여부 */
	u8FindMark	= IsValidGrabMarkNum(img_id) ? 0x01 : 0x00;
	/* Grabbed Image에 대한 Mark 정보 그리기 */
	return uvMIL_DrawMark(hwnd, 0x01, img_id, u8FindMark);
}

/*
 desc : Calibration 이미지 윈도 영역에 출력 수행
 parm : hwnd	- [in]  이미지가 출력될 윈도 핸들
		grab	- [in]  grab 이미지가 저장된 구조체 포인터 (grab_data에 이미지가 정보가 반드시 저장되어 있어야 됨)
		find	- [in]  마크 검색 성공 여부 (검색됐더라도, 검색 결과 값에 따라 달라짐)
						0x00 - 검색 결과 실패, 0x01 - 검색 결과 성공
 retn : None
*/
API_EXPORT VOID uvIDS_DrawMarkData(HWND hwnd, LPG_ACGR grab, UINT8 find)
{
	uvMIL_DrawMarkData(hwnd, grab, find);
}

/*
 desc : Drawing - Examination Object Image
 parm : hwnd	- [in]  이미지가 출력될 윈도 핸들
		grab_out- [in]  Grabbed Image 정보가 저장된 구조체 포인터 (바깥 링)
		grab_in	- [in]  Grabbed Image 정보가 저장된 구조체 포인터 (안쪽 원)
 retn : None
*/
API_EXPORT VOID uvIDS_DrawGrabExamImage(HWND hwnd, LPG_ACGR grab_out, LPG_ACGR grab_in)
{
	uvMIL_DrawGrabExamImage(hwnd, grab_out, grab_in);
}

/*
 desc : Mark 정보 설정
 parm : speed	- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		smooth	- [in]  0.0f - 기본 값, 1.0f ~ 100.0f
		model	- [in]  Model Type, Param 1 ~ 5개 정보까지 모두 포함된 구조체 포인터
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SetModelDefine(UINT8 speed, DOUBLE smooth, LPG_CMPV model)
{
	return uvMIL_SetModelDefine(0x01, speed, smooth, model);
}

/*
 desc : 로컬 시스템 (저장소)에 저장되어 있는 패턴 매칭 원본 이미지 적재
 parm : speed	- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		smooth	- [in]  0.0f - 기본 값, 1.0f ~ 100.0f
		model	- [in]  Model Type 즉, circle, square, rectangle, cross, diamond, triangle
		param	- [in]  총 5개의 Parameter Values (unit : um)
		find	- [in]  검색하고자하는 모델의 개수 (반드시 찾고자하는 모델의 개수임)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SetModelDefineEx(UINT8 speed, DOUBLE smooth, PUINT32 model,
									   DOUBLE *param1, DOUBLE *param2, DOUBLE *param3,
									   DOUBLE *param4, DOUBLE *param5, UINT32 find)
{
	if (!uvMIL_SetModelDefineEx(0x01, speed, smooth, model,
								param1, param2, param3, param4, param5, find))	return FALSE;
	return TRUE;
}

/*
 desc : Mark 정보 설정 - 이미지 데이터
 parm : speed	- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		file	- [in]  모델 이미지가 저장된 파일 이름 (전체 경로 포함)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SetModelDefineLoad(UINT8 speed, CStringArray &file)
{
	return uvMIL_SetModelDefineLoad(0x01, speed, file);
}

/*
 desc : 현재 등록된 Mark Model 정보 반환
 parm : cam_idNone
 retn : 등록된 구조체 포인터 반환
*/
API_EXPORT LPG_CMPV uvIDS_GetModelDefine()
{
	return uvMIL_GetModelDefine(0x01);
}

/*
 desc : 현재 검색을 위해 등록된 모델의 개수 반환
 parm : None
 retn : 등록된 개수
*/
API_EXPORT UINT32 uvIDS_GetModelRegistCount()
{
	return uvMIL_GetModelRegistCount(0x01);
}

/*
 desc : 기존에 적재 (등록)되어 있던 모든 Mark 해제 수행
 parm : None
 retn : None
*/
API_EXPORT VOID uvIDS_ResetMarkModel()
{
	uvMIL_ResetMarkModel();
}

/*
 desc : Mark Template가 등록되어 있는지 확인
 parm : mode	- [in]  0x00 : Expose, 0x01 : Vstep
 retn : TRUE - 등록되어 있다. FALSE - 등록되지 않았다.
*/
API_EXPORT BOOL uvIDS_IsSetMarkModel(UINT8 mode)
{
	return uvMIL_IsSetMarkModel(mode, 0x01);
}

/*
 desc : Mark Pattern 등록 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_IsSetMarkModelNumber()
{
	return uvMIL_IsSetMarkModelNumber(0x01);
}

/*
 desc : 모든 카메라 (카메라 개수에 따라)에 Mark Model이 설정되었는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_IsSetMarkModelAll()
{
	return	uvMIL_IsSetMarkModelNumber(0x01);
}

/*
 desc : 가장 최근에 Edge Detection된 이미지에서 특정 영역만 추출해서 Bitmap Image로 등록
		즉, Mark Template 이미지로 등록
 parm : cam_id	- [in]  Align Camera ID (1 or Later)
		area	- [in]  추출하고자 하는 영역 정보가 저장된 픽셀 위치 값
		type	- [in]  0x00 : Edge, 0x02 : Line, 0x02 : Match, 0x03 : Grab
		file	- [in]  Mark Template를 저장하려는 파일 이름 (전체 경로 포함)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SaveGrabbedMarkToFile(UINT8 cam_id, LPRECT area, UINT8 type, TCHAR *file)
{
	return uvMIL_SaveGrabbedMarkToFile(cam_id, area, type, file);
}

/*
 desc : Edge Detection 검색된 결과 반환
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
 retn : 결과 값이 저장된 포인터 반환
*/
API_EXPORT LPG_EDFR uvIDS_GetEdgeDetectResults(UINT8 cam_id)
{
	return uvMIL_GetEdgeDetectResults(cam_id);
}

/*
 desc : Edge Detection 검색된 개수 반환
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
 변환 : 개수 반환. 실패할 경우, 0 이하 값
*/
API_EXPORT INT32 uvIDS_GetEdgeDetectCount(UINT8 cam_id)
{
	return uvMIL_GetEdgeDetectCount(cam_id);
}

/*
 desc : 현재 등록된 Mark Model Type 반환
 parm : mark_id	- [in]  Align Mark Index (0x00 ~ MAX_REGIST_MODEL-1)
 retn : Model Type 값
*/
API_EXPORT UINT32 uvIDS_GetMarkModelType(UINT8 index)
{
	return uvMIL_GetMarkModelType(0x01, index);
}

/*
 desc : 모델 크기 반환
 parm : index	- [in]  요청하고자 하는 모델의 위치 (Zero-based)
		flag	- [in]  0x00 : 가로  크기, 0x01 : 세로 크기
		unit	- [in]  0x00 : um, 0x01 : pixel
 retn : 크기 반환 (단위: um)
*/
API_EXPORT DOUBLE uvIDS_GetMarkModelSize(UINT8 index, UINT8 flag, UINT8 unit)
{
	return uvMIL_GetMarkModelSize(0x01, index, flag, unit);
}

/*
 desc : 사용자에 의해 수동으로 검색된 경우, 결과 값만 처리
 parm : grab	- [in]  사용자에 의해 수동으로 입력된 grabbed image 결과 정보가 저장된 구조체 포인터
		gmfr	- [in]  GMFR Data
		gmsr	- [in]  GMSR Data
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SetGrabbedMark(LPG_ACGR grab, LPG_GMFR gmfr, LPG_GMSR gmsr)
{
	if (!g_pCamThread)	return FALSE;
	return g_pCamThread->SetGrabbedMark(grab, gmfr, gmsr);
}

/*
 desc : Align Mark 검색 방식 설정
 parm : method	- [in]  0x00 : 최적의 1개 반환, 0x01 : 다점 마크 기준으로 가중치를 부여하여 결과 반환
		count	- [in]  'method' 값이 1인 경우, 최종 검색될 Mark 개수 값 2 이상 값이어야 됨
 retn : None
*/
API_EXPORT VOID uvIDS_SetMarkMethod(ENG_MMSM method, UINT8 count)
{
	if (method == ENG_MMSM::en_single)	count	= 0x00;
	uvMIL_SetMarkMethod(method, count);
}

/*
 desc : 카메라 내부의 이미지 버퍼에 Single Grabbed Image 저장
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_RunGrabbedImage()
{
	/* Grabbed Image */
	return g_pCamThread->RunGrabbedImage();
}

/*
 desc : 카메라로부터 Live Image 1장이 Grabbed 되었는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_IsLiveGrabbedImage()
{
	return g_pCamMain->IsLiveGrabbedImage();
}

/*
 desc : 현재 Live 영상 이미지가 출력되는 영역 (Client 화면 좌표계) 좌표 값 반환
 parm : live	- [out] Client 영역에서 영상이 출력되고 있는 위치 반환
 retn : Live 영상 이미지가 출력된 영역 값 반환
*/
API_EXPORT VOID uvIDS_GetLiveArea(RECT &live)
{
	uvMIL_GetLiveArea(live);
}

/*
 desc : Iamge 회전 (이미지의 중심을 기준으로 회전)
 parm : img_src		- [in]  원본 이미지 버퍼
		img_dst		- [out] 회전된 이미지 버퍼
		width		- [in]	원본 이미지의 넓이
		height		- [in]	원본 이미지의 높이
		angle		- [in]	회전 하고자 하는 각도
 retn : None
*/
API_EXPORT VOID uvIDS_ImageRotate(PUINT8 img_src, PUINT8 img_dst,
								  UINT32 width, UINT32 height, UINT32 angle)
{
	uvMIL_ImageRotate(img_src, img_dst, width, height, angle);
}

/*
 desc : 현재 영상 카메라로부터 Grab되는 이미지의 크기 (Pixel) 반환
 parm : flag	- [in]  0x00 : Width, 0x01 : Height
 retn : 이미지 크기 반환
*/
API_EXPORT UINT32 uvIDS_GetFovSize(UINT8 flag)
{
	if (flag > 2)	return 0;
	return g_pstConfig->GetACamGrabSize(flag);
}

/*
 desc : 가장 최근에 저장된 Live Image를 파일로 저장
 parm : file	- [in]  저장될 파일 이름 (전체 경로 포함)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SaveLiveImageToFile(TCHAR *file)
{
	return g_pCamMain->SaveLiveImageToFile(file);
}

/*
 desc : 가장 최근에 저장된 Live Image에서 Drag 영역 이미지 정보 얻기
 parm : drag	- [in]  Live Image 영역에 Drag 영역을 수행한 좌표 정보 저장 (단위: 픽셀)
 retn : drag 이미지 정보를 저장하고 있는 버퍼 포인터
*/
API_EXPORT PUINT8 uvIDS_GetLastLiveDragImage(RECT drag)
{
	return g_pCamMain->GetLastLiveDragImage(drag);
}

/*
 desc : 현재 MIL 라이선스가 유효한지 여부 확인
		Pattern, Model Finder, Edge Finder, Read, OCR, Image Processing 등 유효한지 확인
 parm : None
 retn : TRUE or FALS
*/
API_EXPORT BOOL uvIDS_IsLicenseValid()
{
	return uvMIL_IsLicenseValid();
}

/*
 desc : 노광 모드 설정 즉, 직접 노광, 얼라인 노광, 보정 후 얼라인 노광
 parm : mode	- [in]  직접 노광 (0x00), 얼라인 노광 (0x01), 얼라인 카메라 보정 값 적용 후 얼라인 노광 (0x02)
 retn : None
*/
API_EXPORT VOID uvIDS_SetAlignMode(ENG_AOEM mode)
{
	uvMIL_SetAlignMode(mode);
}

/*
 desc : 이미지의 가로 혹은 세로의 경계선 중심 위치 얻기
 parm : cam_id		- [in]  Align Camera ID (1 or 2)
		type		- [in]  0x00: Camera Grabbed Image
							0x01: File Grabbed Image
		mil_result	- [in]  MIL 내부 검색 결과 대화 상자 출력 여부
 retn : TRUE or FALSE
*/
#if (USE_EQUIPMENT_SIMULATION_MODE)
API_EXPORT BOOL uvIDS_RunLineDetectCentXY(UINT8 cam_id, UINT8 type, BOOL mil_result)
#else
API_EXPORT BOOL uvIDS_RunLineDetectCentXY(UINT8 cam_id, BOOL mil_result)
#endif
{
	LPG_ACGR pstGrab	= NULL;
	/* Check to Grabbed Image Size */
#if (USE_EQUIPMENT_SIMULATION_MODE)
	if (0x00 == type)	pstGrab	= g_pCamMain->GetGrabbedImage();
	else				pstGrab	= g_pCamMain->GetFileGrabbedImage();
#else
	pstGrab	= g_pCamMain->GetGrabbedImage();
#endif
	if (!pstGrab || pstGrab->IsEmpty())	return FALSE;
	return uvMIL_RunLineDetectCentXY(cam_id, pstGrab, mil_result);
}

#if (USE_EQUIPMENT_SIMULATION_MODE)
/*
 desc : File Grabbed Image 처리
 parm : file	- [in]  전체 경로가 포함된 이미지 파일 (256 비트맵. 8 bit gray)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SetFileGrabbedImage(PTCHAR file)
{
	UINT32 u32FileSize	= 0, i = 0, j = 0;
	BITMAPFILEHEADER stBmpFile;
	BITMAPINFOHEADER stBmpInfo;
	PUINT8 pImage		= NULL;
	FILE *fpRead		= NULL;
	CUniToChar csCnv;

	/* 파일 열기 */
	if (0 != fopen_s(&fpRead, csCnv.Uni2Ansi(file), "rb"))	return FALSE;
	/* Header 정보 읽기 */
	fread_s(&stBmpFile, sizeof(BITMAPFILEHEADER), sizeof(BITMAPFILEHEADER), 1, fpRead);
	if (stBmpFile.bfType  != 0x4D42)
	{
		fclose(fpRead);
		return FALSE;
	}
	fread_s(&stBmpInfo, sizeof(BITMAPINFOHEADER), sizeof(BITMAPINFOHEADER), 1, fpRead);
	if (stBmpInfo.biBitCount != 8)
	{
		fclose(fpRead);
		return FALSE;
	}
#if 0	/* 256 gray color 모드이므로, 팔레트는 필요 없다 */
	/* BMP Pallete */
	fread_s(stRGB, sizeof(RGBQUAD)*2, sizeof(RGBQUAD)*2, 1, fpRead);
#endif
	fseek(fpRead, stBmpFile.bfOffBits, SEEK_SET);
	/* 본문 이미지 읽기 */
	if (stBmpInfo.biSizeImage < 1)
	{
		stBmpInfo.biSizeImage	= stBmpFile.bfSize - stBmpFile.bfOffBits;
	}
	pImage	= (PUINT8)::Alloc(stBmpInfo.biSizeImage+1);
	pImage[stBmpInfo.biSizeImage]	= 0x00;
	fread_s(pImage, stBmpInfo.biSizeImage+1, 1, stBmpInfo.biSizeImage, fpRead);
	fclose(fpRead);

	/* 비트맵 이미지 상하 반전 */
#if 0
	pInvert	= (PUINT8)::Alloc(stBmpInfo.biSizeImage+1);
	pInvert[stBmpInfo.biSizeImage]	= 0x00;
	for (i=0; i<stBmpInfo.biHeight; i++)
	{
		for (j=0; j<stBmpInfo.biWidth; j++)
		{
			pInvert[(stBmpInfo.biHeight - i - 1) * stBmpInfo.biWidth + j] = 
				pImage[j + i * stBmpInfo.biWidth];
		}
	}
	::Free(pInvert);
#else
	uvCmn_BmpDataUpDownChange(stBmpInfo.biWidth, stBmpInfo.biHeight, pImage);
#endif
	/* 파일 업로드 진행 */
	g_pCamMain->SetFileGrabbedImage(stBmpInfo.biSizeImage,
									stBmpInfo.biWidth,
									stBmpInfo.biHeight,
									pImage);
	/* 메모리 해제 */
	::Free(pImage);

	return TRUE;
}
#endif

#ifdef __cplusplus
}
#endif
