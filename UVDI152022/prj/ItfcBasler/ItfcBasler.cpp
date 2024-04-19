
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

UINT64						g_u64DrawDelayTime[MAX_ALIGN_CAMERA]	= {NULL};	/* 연속적으로 화면 출력하는데 한계가 있으므로, 일정 시간 지연 */

ENG_VLPK					g_enVisionLib	= ENG_VLPK::en_vision_none;

LPG_VDSM					g_pstShMem		= NULL;

CCamThread					*g_pCamThread	= NULL;
CCamMain					**g_pCamMain	= NULL;
CDrawMark					g_csDrawMark;


/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 기존 Grabbed Image의 마크 검색 정보 초기화
 parm : None
 retn : None
*/
VOID ResetGrabbedMark()
{
	UINT8 i	= 0x00;

	if (!g_pCamMain)	return;
	for (; i<g_pstConfig->set_cams.acam_count; i++)	g_pCamMain[i]->ResetGrabbedMark();
}

/*
 desc : 기존 Grabbed 이미지 모두 제거
 parm : None
 retn : None
*/
VOID ResetGrabbedImage()
{
	UINT8 i	= 0;

	for (; i<g_pstConfig->set_cams.acam_count; i++)
	{
		g_pCamMain[i]->ResetGrabbedImage();
	}
	g_pCamThread->ResetGrabbedImage();

	/* 기존 Grabbed Image의 마크 검색 정보 초기화 */
	ResetGrabbedMark();
	/* Match Template Image 메모리 해제 */
	uvMIL_ResetGrabAll();
}

/*
 desc : Global 4개의 Mark에 대해서 모두 유효한지 검사
 변수 :	cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : TRUE (유효함) or FALSE (4개 중 1개라도 제대로 인식 안됨)
*/
BOOL IsValidGrabMarkNum(UINT8 cam_id, UINT8 img_id)
{
	LPG_ACGR pstGrab;

	if (!g_pCamThread)	return FALSE;
	pstGrab	= g_pCamThread->GetGrabbedMark(cam_id, img_id);
	if (!pstGrab->valid_scale || !pstGrab->valid_score)	return FALSE;

	return TRUE;
}

/*
 desc : Global Mark 대상으로, 2 개의 검색된 마크 정보 반환
 parm : direct	- [in]  측정지점 (0 - 1번 마크와 3번 마크 간의 길이 값)
								 (1 - 2번 마크와 4번 마크 간의 길이 값)
								 (2 - 1번 마크와 2번 마크 간의 길이 값)
								 (3 - 3번 마크와 4번 마크 간의 길이 값)
								 (4 - 1번 마크와 4번 마크 간의 길이 값)
								 (5 - 2번 마크와 3번 마크 간의 길이 값)
		data1	- [out] 검색된 첫 번째 마크의 중심 위치에서 오차 값 (단위: mm)
		data2	- [out] 검색된 두 번째 마크의 중심 위치에서 오차 값 (단위: mm)
 retn : TRUE or FALSE
*/
BOOL GetGrabbedMarkDirect(ENG_GMDD direct, STG_DBXY &data1, STG_DBXY &data2)
{
	LPG_ACGR pstGrab[2]	= {NULL};

	/* 현재 거버의 좌표 값의 단위가 nm(나노)이므로, 100으로 나누어야 100 nm or 0.1 um가 된다.*/
	switch (direct)
	{
	case ENG_GMDD::en_top_horz	:	pstGrab[0]	= g_pCamThread->GetGrabbedMark(1, 0);
									pstGrab[1]	= g_pCamThread->GetGrabbedMark(2, 0);	break;
	case ENG_GMDD::en_btm_horz	:	pstGrab[0]	= g_pCamThread->GetGrabbedMark(1, 1);
									pstGrab[1]	= g_pCamThread->GetGrabbedMark(2, 1);	break;
	case ENG_GMDD::en_lft_vert	:	pstGrab[0]	= g_pCamThread->GetGrabbedMark(1, 0);
									pstGrab[1]	= g_pCamThread->GetGrabbedMark(1, 1);	break;
	case ENG_GMDD::en_rgt_vert	:	pstGrab[0]	= g_pCamThread->GetGrabbedMark(2, 0);
									pstGrab[1]	= g_pCamThread->GetGrabbedMark(2, 1);	break;
	case ENG_GMDD::en_lft_diag	:	pstGrab[0]	= g_pCamThread->GetGrabbedMark(1, 0);
									pstGrab[1]	= g_pCamThread->GetGrabbedMark(2, 1);	break;
	case ENG_GMDD::en_rgt_diag	:	pstGrab[0]	= g_pCamThread->GetGrabbedMark(1, 1);
									pstGrab[1]	= g_pCamThread->GetGrabbedMark(2, 0);	break;
	}
	if (!pstGrab[0] || !pstGrab[1])	return FALSE;

	/* 검색된 마크의 오차 값 반환 */
	//data1.x	= pstGrab[0]->mark_cent_mm_x;
	//data1.y	= pstGrab[0]->mark_cent_mm_y;
	//data2.x	= pstGrab[1]->mark_cent_mm_x;
	//data2.y	= pstGrab[1]->mark_cent_mm_y;
	data1.x = pstGrab[0]->move_mm_x;
	data1.y = pstGrab[0]->move_mm_y;
	data2.x = pstGrab[1]->move_mm_x;
	data2.y = pstGrab[1]->move_mm_y;

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
 parm : config		- [in]  전체 환경 정보
		shmem		- [in]  Shared Memory
		kind		- [in]  Vision Library 종류
		work_dir	- [in]  작업 경로 (실행 파일 경로)
		time_out	- [in]  기존 카메라와의 연결해제가 비정상적으로 종료되었을 때,
							카메라 내부에서 기존 연결된 클라이언트의 응답이 없을 때, 해제 대기 시간
							클라이언트로부터 임의 설정 시간(단위: 밀리초)동안 응답 없으면, 강제 해제
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_Init(LPG_CIEA config, LPG_VDSM shmem, ENG_VLPK kind, PTCHAR work_dir,
							  UINT64 time_out)
{
	UINT8 i	= 0;
	CHAR szPutEnv[128]	= {NULL};

	/* 전체 환경 정보 연결 */
	g_pstConfig		= config;
	g_pstShMem		= shmem;
	g_enVisionLib	= kind;

	/* 작업 경로 저장 */
	wcscpy_s(g_tzWorkDir, MAX_PATH_LEN, work_dir);

	/* Vision : MIL / Cognex / OpenCV / Halcon */
	switch (kind)
	{
	case ENG_VLPK::en_vision_opencv	:	return FALSE;	break;
	case ENG_VLPK::en_vision_mil	:	if (!uvMIL_Init(config, shmem, work_dir))	return FALSE;	break;
	case ENG_VLPK::en_vision_cognex	:	return FALSE;	break;
	case ENG_VLPK::en_vision_halcon	:	return FALSE;	break;
	}

	/* ----------------------------------------------------------------------------------------- */
	/* GigE Type 의카메라 사용시, Connection 이 임의로 Disconnect 되었을 경우 Disconnect 이후에  */
	/* 얼마의 시간이 지난 후 Connection을 받아들일 것인지를 설정할수있다.(Debugging 시에 유용함) */
	/* 좀더 구체적으로 말씀 드리면 카메라 연결 해제 함수 이전에 강제로 종료가 된 경우라면 프로그 */
	/* 램은 종료가 되더라도 카메라가 이전 연결을 계속 잡고 있어서 새로운 연결을 할 수 없는 상태가*/
	/* 되게 됩니다.	이때에는 아래 코드를 카메라 연결 전 초기화 때 호출해 주셔서 프로그램 실행을  */
	/* 해 주셔야 합니다. 만약 아래 코드를 써주시지 않았다면 카메라는 연결이 되지 않고 에러코드가 */
	/* 뜰 수 있습니다.                                                                           */
	/* ----------------------------------------------------------------------------------------- */
	sprintf_s(szPutEnv, 128, "PYLON_GIGE_HEARTBEAT=%llu", time_out);
	_putenv(szPutEnv);	/* set to 5000 ms = 5 sec */
    // Before using any pylon methods, the pylon runtime must be initialized. 
    PylonInitialize();
	/* Grabbed Image 수신 및 저장 관리 클래스 생성 */
	g_pCamMain	= (CCamMain **) new CCamMain * [g_pstConfig->set_cams.acam_count];
	for (i=0; i<g_pstConfig->set_cams.acam_count; i++)
	{
		g_pCamMain[i]	= new CCamMain(config, i+1 /* Align Camera Index : 1 or Later */);
		ASSERT(g_pCamMain[i]);
	}
	/* Grabbed Image 처리하는 스레드 */
	g_pCamThread	= new CCamThread(g_pstConfig->set_cams.acam_count,
									 g_pstConfig->mark_find.max_mark_grab,
									 g_pstShMem->link, g_pCamMain, g_pstConfig->mark_find.image_process);
	ASSERT(g_pCamThread);
	if (!g_pCamThread->CreateThread(0, 0, NORMAL_THREAD_SPEED))	return FALSE;

	return TRUE;
}

/*
 desc : BIPS Server와 통신을 담당하는 Client 스레드 생성 및 실행
 parm : config		- [in]  전체 환경 정보
		shmem		- [in]  Shared Memory
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_Open(LPG_CIEA config, LPG_VDSM shmem)
{
	UINT8 i	= 0;
	CHAR szPutEnv[128]	= {NULL};

	/* 전체 환경 정보 연결 */
	g_pstConfig	= config;
	g_pstShMem	= shmem;

	return TRUE;
}

/*
 desc : 라이브러리 해제
 parm : None
 retn : None
*/
API_EXPORT VOID uvBasler_Close()
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
	if (g_pstConfig && g_pCamMain)
	{
		for (; i<g_pstConfig->set_cams.acam_count; i++)	delete g_pCamMain[i];
		delete [] g_pCamMain;
		g_pCamMain	= NULL;
	}
	// Releases all pylon resources. 
    PylonTerminate();

	/* Vision : MIL / Cognex / OpenCV / Halcon */
	switch (g_enVisionLib)
	{
	case ENG_VLPK::en_vision_opencv	:	break;
	case ENG_VLPK::en_vision_mil	:	uvMIL_Close();
	case ENG_VLPK::en_vision_cognex	:	break;
	case ENG_VLPK::en_vision_halcon	:	break;
	}
	/* 반드시 해줘야 됨 */
	g_pstConfig	= NULL;
}

/*
 desc : 얼라인 카메라 부분만 재접속 수행
 parm : None
 retn : None
*/
API_EXPORT VOID uvBasler_Reconnected()
{
	g_pCamThread->Reconnected();
	uvMIL_ResetGrabAll();
}

/*
 desc : Mark Pattern 생성 윈도 핸들
 parm : hwnd	- [in]  마크 생성 윈도 핸들
 retn : None
*/
API_EXPORT VOID uvBasler_SetMarkHwnd(HWND hwnd)
{
	if (!g_pCamThread)	return;
	g_csDrawMark.SetDrawHwnd(hwnd);
}

/*
 desc : BIPS Server와 통신을 담당하는 Client 스레드 생성 및 실행
 parm : time_out	- [in]  기존 카메라와의 연결해제가 비정상적으로 종료되었을 때,
							카메라 내부에서 기존 연결된 클라이언트의 응답이 없을 때, 해제 대기 시간
							클라이언트로부터 임의 설정 시간(단위: 밀리초)동안 응답 없으면, 강제 해제
		work_dir	- [in]  작업 경로 (실행 파일 경로)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_UpdateSetParam(PTCHAR work_dir, UINT64 time_out)
{
	if (!g_pCamThread)	return	FALSE;
	uvBasler_Close();
	return uvBasler_Init(g_pstConfig, g_pstShMem, g_enVisionLib, work_dir, time_out);
}

/*
 desc : Mark Pattern 출력
 parm : inner	- [in]  내부 크기 (단위: pixel)
		outer	- [in]  외부 크기 (단위: pixel)
		reverse	- [in]  흑백 반전 여부 (0x00 - In:White/Out:Black, 0x01 - In:Black/Out:White)
		pattern	- [in]  마크 패턴 타입 (0x00 - Circle, 0x01 - Rectangle, 0x02 - Cross)
 retn : None
*/
API_EXPORT VOID uvBasler_CreateMark(LPRECT inner, LPRECT outer, UINT8 reverse, UINT8 pattern)
{
	if (!g_pCamThread)	return;
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
API_EXPORT BOOL uvBasler_SaveMark(TCHAR *file, LPRECT inner, LPRECT outer, UINT8 reverse, UINT8 pattern)
{
	if (!g_pCamThread)	return	FALSE;
	return g_csDrawMark.Save(file, inner, outer, reverse, pattern);
}

/*
 desc : Vision Camera Mode 설정
 parm : mode	- [in]  ENG_VCCM::en_xxx
 retn : None
*/
API_EXPORT VOID uvBasler_SetCamMode(ENG_VCCM mode)
{
	UINT8 i	= 0;
	if (!g_pCamMain)	return;

#if 1
	for (; i<g_pstConfig->set_cams.acam_count; i++)
	{
		g_pCamMain[i]->SetCamMode(mode);
	}
#else
	g_pCamMain[0]->SetCamMode(mode);
#endif
}

/*
 desc : 현재 Camera의 동작 모드 값 반환
 parm : None
 retn : ENG_VCCM::en_xxx
*/
API_EXPORT ENG_VCCM uvBasler_GetCamMode()
{
	return g_pCamMain == NULL ? ENG_VCCM::en_none : g_pCamMain[0]->GetCamMode();
}

/*
 desc : 현재 PLC가 연결되어 있는지 여부 확인
 변수 :	cam_id	- [in]  Camera Index (1 or 2)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_IsConnected(UINT8 cam_id)
{
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return FALSE;
	if (!g_pCamThread || !g_pCamMain)								return FALSE;

	return g_pCamMain[cam_id-1]->IsConnected();
}

/*
 desc : 현재 모든 Align Camera가 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_IsConnectedAll()
{
	UINT8 i	= 0;

	if (!g_pCamMain)	return FALSE;
	for (; i<g_pstConfig->set_cams.acam_count; i++)
	{
		if (!g_pCamMain[i]->IsConnected())	return FALSE;
	}
	return TRUE;
}

/*
 desc : 현재 등록된 Grabbed 이미지 개수 반환
 parm : None
 retn : 저장된 이미지 개수 반환
*/
API_EXPORT UINT16 uvBasler_GetGrabbedCount(int* CamNum = nullptr)
{
	if (!g_pCamThread)	return 0x0000;
	return g_pCamThread->GetGrabbedCount(CamNum);
}

/*
 desc : Grabbed Images 모두 Score 값이 유효한지 값 반환
 parm : set_score	- [in]  If the score of the searched mark is higher than this set score, it is valid
  							(If 0.0f is unchecked) 
 retn : TRUE : 유효함, FALSE : 유효하지 않음
*/
API_EXPORT BOOL uvBasler_IsScoreValidAll(DOUBLE set_score)
{
	return !g_pCamThread ? FALSE : g_pCamThread->IsScoreValidAll(set_score);
}

/*
 desc : Global Mark 대상으로, 2 개의 검색된 마크 정보 반환
 parm : direct	- [in]  측정지점 (0 - 1번 마크와 3번 마크 간의 길이 값)
								 (1 - 2번 마크와 4번 마크 간의 길이 값)
								 (2 - 1번 마크와 2번 마크 간의 길이 값)
								 (3 - 3번 마크와 4번 마크 간의 길이 값)
								 (4 - 1번 마크와 4번 마크 간의 길이 값)
								 (5 - 2번 마크와 3번 마크 간의 길이 값)
		data1	- [out] 검색된 첫 번째 마크의 중심 위치에서 오차 값 (단위: mm)
		data2	- [out] 검색된 두 번째 마크의 중심 위치에서 오차 값 (단위: mm)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_GetGrabbedMarkDirect(ENG_GMDD direct, STG_DBXY &data1, STG_DBXY &data2)
{
	return GetGrabbedMarkDirect(direct, data1, data2);
}

/*
 desc : Global Mark 대상으로, 2 개의 마크가 떨어진 거리 값 반환 (단위 : mm)
 parm : direct	- [in]  측정지점 (0 - 1번 마크와 3번 마크 간의 길이 값)
								 (1 - 2번 마크와 4번 마크 간의 길이 값)
								 (2 - 1번 마크와 2번 마크 간의 길이 값)
								 (3 - 3번 마크와 4번 마크 간의 길이 값)
								 (4 - 1번 마크와 4번 마크 간의 길이 값)
								 (5 - 2번 마크와 3번 마크 간의 길이 값)
 retn : 두 마크의 떨어진 거리 (단위: mm)
*/
API_EXPORT DOUBLE uvBasler_GetGrabbedMarkDist(ENG_GMDD direct)
{
	STG_DBXY stData[2]	= {NULL};

	if (!GetGrabbedMarkDirect(direct, stData[0], stData[1]))	return 0.0f;
	/* 절대 값 반환 */
	return sqrt(pow(stData[0].x - stData[1].x, 2) + pow(stData[0].y - stData[1].y, 2));
}

/*
 desc : 검색된 마크 이미지가 포함된 구조체 포인터 반환
 parm : cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later) (if img_id == 0xff then Calibration Method)
 retn : 구조체 포인터 반환
*/
API_EXPORT LPG_ACGR uvBasler_GetGrabbedMark(UINT8 cam_id, UINT8 img_id)
{
	if (!g_pCamThread)	return	NULL;
	return g_pCamThread->GetGrabbedMark(cam_id, img_id);
}


/*
 desc : 검색된 마크 이미지가 포함된 구조체 포인터 반환
 parm : cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later) (if img_id == 0xff then Calibration Method)
 retn : 구조체 포인터 반환
*/
API_EXPORT CAtlList <LPG_ACGR>*  uvBasler_GetGrabbedMarkAll()
{
	if (!g_pCamThread)	return	NULL;
	return g_pCamThread->GetGrabbedMarkAll();
}


API_EXPORT BOOL uvBasler_TryEnterCS()
{
	if (!g_pCamThread)	return	FALSE;
	return g_pCamThread->TryEnterCS();
}


API_EXPORT void uvBasler_ExitCS()
{
	if (!g_pCamThread)	return;
	g_pCamThread->ExitCS();
}


/*
 desc : Grabbed Image의 데이터 반환
 parm : index	- [in]  가져오고자 하는 위치 (Zero based)
 retn : grab data 반환
*/
API_EXPORT LPG_ACGR uvBasler_GetGrabbedMarkIndex(UINT8 index)
{
	return !g_pCamThread ? NULL : g_pCamThread->GetGrabbedMarkIndex(index);
}

/*
 desc : Grabbed Image의 결과 수정
 parm : grab	- [in]  저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_SetGrabbedMark(LPG_ACGR grab)
{
	if (!g_pCamThread || grab->cam_id < 1 || g_pstConfig->set_cams.acam_count < grab->cam_id)	return FALSE;
	return g_pCamThread->SetGrabbedMark(grab);
}

/*
 desc : 사용자에 의해 수동으로 검색된 경우, 결과 값만 처리
 parm : grab	- [in]  사용자에 의해 수동으로 입력된 grabbed image 결과 정보가 저장된 구조체 포인터
		gmfr	- [in]  GMFR Data
		gmsr	- [in]  GMSR Data
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_SetGrabbedMarkEx(LPG_ACGR grab, LPG_GMFR gmfr, LPG_GMSR gmsr)
{
	if (!g_pCamThread)	return FALSE;
	return g_pCamThread->SetGrabbedMark(grab, gmfr, gmsr);
}

/*
 desc : Grabbed Image의 데이터 수정
 parm : index	- [in]  가져오고자 하는 위치 (Zero based)
 parm : grab	- [in]  저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_SetGrabbedMarkIndex(UINT8 index, LPG_ACGR grab)
{
	return !g_pCamThread ? FALSE : g_pCamThread->SetGrabbedMarkIndex(index, grab);
}

/*
 desc : Global 4개의 Mark에 대해서 모두 유효한지 검사
 parm : multi_mark	- [in]  Multi Mark (다중 마크) 적용 여부
		set_score	- [in]  If the score of the searched mark is higher than this set score, it is valid
		 					(If 0.0f is unchecked) 
 retn : TRUE (유효함) or FALSE (4개 중 1개라도 제대로 인식 안됨)
*/
API_EXPORT BOOL uvBasler_IsGrabbedMarkValidAll(BOOL multi_mark, DOUBLE set_score, int* camNum)
{
	return !g_pCamThread ? FALSE : g_pCamThread->IsGrabbedMarkValidAll(multi_mark, set_score,camNum);
}

/*
 desc : Global 4개의 Mark에 대해서 모두 유효한지 검사
 변수 :	cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : TRUE (유효함) or FALSE (4개 중 1개라도 제대로 인식 안됨)
*/
API_EXPORT BOOL uvBasler_IsValidGrabMarkNum(UINT8 cam_id, UINT8 img_id)
{
	LPG_ACGR pstGrab	= NULL;

	if (!g_pCamThread)	return FALSE;
	pstGrab	= g_pCamThread->GetGrabbedMark(cam_id, img_id);
	if (!pstGrab)	return FALSE;
#if (USE_MARK_VALID_SCALE)
	if (!pstGrab->valid_scale || !pstGrab->valid_score || !pstGrab->valid_dist)	return FALSE;
#else
	if (!pstGrab->valid_score)	return FALSE;
#endif

	return TRUE;
}
#if 0
/*
 desc : 기존 Grabbed Image의 마크 검색 정보 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvBasler_ResetGrabbedMark()
{
	UINT8 i	= 0x00;

	if (!g_pCamMain)	return;
	for (; i<g_pstConfig->set_cams.acam_count; i++)	g_pCamMain[i]->ResetGrabbedMark();
}
#endif
/*
 desc : 기존 Grabbed 이미지 모두 제거
 parm : None
 retn : None
*/
API_EXPORT VOID uvBasler_ResetGrabbedImage()
{
	if (!g_pCamThread)	return;
	ResetGrabbedImage();
}

/*
 desc : Calibration Image가 존재하면, Align Mark 수행
 변수 :	cam_id	- [in]  Camera Index (1 or 2)
		mode	- [in]  0xff : Calibration Mode (얼라인 카메라 각도 측정하지 않음)
						0xfe : Align Camera Angle Measuring Mode (얼라인 카메라 각도 측정하기 위함)
 retn : Calirbated Image의 정보가 저장된 구조체 포인터 반환
*/
API_EXPORT LPG_ACGR uvBasler_RunModelCali(UINT8 cam_id, UINT8 mode, UINT8 dlg_id, UINT8 mark_no, BOOL useMilDisp, UINT8 img_proc,int flipDir)
{
	/* New Grabbed Image 저장 */
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return NULL;
	return g_pCamThread->RunModelFind(cam_id, mode, dlg_id, mark_no, useMilDisp, img_proc,flipDir);
}

/*
 desc : Geometric Model Find
 parm : cam_id	- [in]  Camera Index (1 or 2)
 retn : TRUE or FALSE
*/
//API_EXPORT BOOL uvBasler_RunModelFind(UINT8 cam_id, UINT8 mark_no) // 미사용
//{
//	//LPG_ACGR pstGrab	= NULL;
//	//
//	//if (!g_pCamThread)	return FALSE;
//	//pstGrab	= g_pCamMain[cam_id-1]->GetGrabbedImage();
//	//if (!pstGrab)	return FALSE;
//	/* 검색 작업 진행 */
//	//return uvMIL_RunModelFind(cam_id, pstGrab->img_id, pstGrab->img_id, pstGrab->grab_data, g_pCamMain[cam_id - 1]->GetDispType(), mark_no);
//	return FALSE;
//}

/*
 desc : 가장 최근에 Calibrated된 결과 데이터 반환
 parm : None
 retn : 매칭된 결과 값 반환
*/
API_EXPORT LPG_ACGR uvBasler_GetLastGrabbedMark()
{
	if (!g_pCamThread)	return NULL;
	return g_pCamThread->GetLastGrabbedMark();
}

/*
 desc : 가장 최근 Grabbed Image의 매칭 검색 결과 구조체 포인터 반환
 parm : None
 retn : 객체 포인터 (매칭이 실패한 경우 NULL)
*/
API_EXPORT LPG_ACGR uvBasler_GetLastGrabbedMarkEx()
{
	return uvMIL_GetLastGrabbedMark();
}

/*
 desc : 가장 최근에 카메라 별로 Calibrated된 결과 중 Error 값이 저장된 데이터 반환
 parm : None
 retn : 매칭된 결과 값 반환
*/
API_EXPORT LPG_ACLR uvBasler_GetLastGrabbedACam()
{
	if (!g_pCamThread)	return NULL;
	return g_pCamThread->GetLastGrabbedACam();
}

/*
 desc : Step (Vision 단차) Image가 존재하면, Align Mark 수행
 변수 :	cam_id	- [in]  Camera Index (1 or 2)
		counts	- [in]  검색 대상의 Mark 개수 (찾고자하는 마크 개수)
		angle	- [in]  각도 적용 여부 (TRUE : 각도 적용, FALSE : 각도 적용 없음)
						TRUE : 현재 카메라의 회전된 각도 구하기 위함, FALSE : 기존 각도 적용하여 회전된 각도 구함
		results	- [out] 검색된 결과 정보가 저장될 구조체 참조 포인터 (grabbed image buffer까지 복사해옴)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_RunModelStep(UINT8 cam_id, UINT16 count, BOOL angle, LPG_ACGR results, UINT8 dlg_id, UINT8 mark_no, UINT8 img_proc)
{
	if (!g_pCamThread)	return FALSE;
	return g_pCamThread->RunModelStep(cam_id, count, angle, results, dlg_id, mark_no, img_proc);
}

/*
 desc : Examination (Vision Align 검사 (측정)) Image가 존재하면, Align Shutting 검사 수행
 변수 :	score		- [in]  크기 비율 값 (이 값보다 크거나 같으면 Okay)
		scale		- [in]  매칭 비율 값 (1.0 기준으로 +/- 이 값 범위 안에 있으면 Okay)
		results		- [out] 검색된 결과 정보가 저장될 구조체 참조 포인터 (grabbed image buffer까지 복사해옴)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_RunModelExam(DOUBLE score, DOUBLE scale, LPG_ACGR results)
{
	return g_pCamThread->RunModelExam(score, scale, results);
}

/*
 desc : Grabbed Image가 존재하면, Edge Detection 수행
 변수 :	cam_id	- [in]  Camera Index (1 or 2)
		saved	- [in]  분석된 edge image 저장 여부
 retn : Edge Detected된 영역의 직경 (지름) 값 반환 (단위: Pixel)
*/
API_EXPORT BOOL uvBasler_RunEdgeDetect(UINT8 cam_id, UINT8 saved)
{
	/* 기존 Grabbed Image 제거 */
	ResetGrabbedImage();
	/* New Grabbed Image 처리 */
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return FALSE;
	return g_pCamThread->RunEdgeDetect(cam_id, saved);
}

/*
 desc : Set the parameters of the strip maker and find in an image and take the specified measurements
 parm : cam_id	- [in]  Align Camera ID (1 or 2 or later)
		param	- [in]  Structures pointer with information to set up is stored
		result	- [out] Buffer in which the resulting value will be returned and stored.
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_RunMarkerStrip(UINT8 cam_id, LPG_MSMP param, STG_MSMR &results)
{
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return FALSE;
	return g_pCamThread->RunMarkerStrip(cam_id, param, results);
}

/*
 desc : 카메라로 캡처 했을 때, 1 픽셀의 실제 크기 계산 (Width & Height 값)
 변수 :	cam_id	- [in]  Camera Index (1 or 2)
 retn : 크기 값 반환 (단위: um)
*/
API_EXPORT DOUBLE uvBasler_CalcCamPixelToUM(UINT8 cam_id)
{
	return g_pstConfig->acam_spec.spec_pixel_um[cam_id-1];
}

/*
 desc : 현재 Grabbed Image에 대한 각 Bytes 별로 저장된 Gray 값 비중 계산
 parm : cam_id	- [in]  Camera Index (1 or 2)
		mode	- [in]  Grabbed Image를 캡처하는 방식 (모드)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_SetGrabGrayRate(UINT8 cam_id, ENG_VCCM mode)
{
	if (!g_pCamMain)	return FALSE;
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return FALSE;
	return g_pCamMain[cam_id-1]->SetGrabGrayRate(mode);
}

/*
 desc : 현재 Grabbed Image에 대한 각 Gray Level 값 (비율) 반환
 parm : cam_id	- [in]  Camera Index (1 or 2)
		index	- [in]  0 ~ 255에 해당되는 Gray Level Index
 retn : 0 ~ 255 들이 Grabbed Image에서 비율 값 반환
*/
API_EXPORT DOUBLE uvBasler_GetGrabGrayRate(UINT8 cam_id, UINT8 index)
{
	if (!g_pCamMain)	return 0.0f;
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return 0.0f;
	return g_pCamMain[cam_id-1]->GetGrabGrayRate(index);
}

/*
 desc : 전체 Grabbed Image에서 색깔이 있는 값의 비율 반환
 parm : cam_id	- [in]  Camera Index (1 or 2)
 retn : White 비율 값 반환
*/
API_EXPORT DOUBLE uvBasler_GetGrabGrayRateTotal(UINT8 cam_id)
{
	if (!g_pCamMain)	return 0.0f;
	return g_pCamMain[cam_id-1]->GetGrabGrayRateTotal();
}

/*
 desc : 현재 Grabbed Image에 대한 각 Bytes 별로 저장된 Gray 값 저장
		넓이에 대해 높이에 있는 모든 Gray 값이 합산하여 저장
 parm : cam_id	- [in]  Camera Index (1 or 2)
		mode	- [in]  Grabbed Image를 캡처하는 방식 (모드)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_SetGrabHistLevel(UINT8 cam_id, ENG_VCCM mode)
{
	if (!g_pCamMain)	return FALSE;
	return g_pCamMain[cam_id-1]->SetGrabHistLevel(mode);
}

/*
 desc : 현재 Grabbed Image에 대한 각 Bytes 별로 저장된 Gray Level 반환
 parm : cam_id	- [in]  Camera Index (1 or 2)
 retn : 현재 Grabbed Image의 넓이 (Column)의 1 픽셀마다 높이의 Level 값들이 저장되어 있는 버퍼 반환
*/
API_EXPORT PUINT64 uvBasler_GetGrabHistLevel(UINT8 cam_id)
{
	if (!g_pCamMain)	return NULL;
	return g_pCamMain[cam_id-1]->GetGrabHistLevel();
}

/*
 desc : 현재 Grabbed Image에 대한 각 Bytes 별로 저장된 Gray Level들의 총 합 값 반환
 parm : cam_id	- [in]  Camera Index (1 or 2)
 retn : Grabbed Image의 전체 Gray Level 값들의 합 반환
*/
API_EXPORT UINT64 uvBasler_GetGrabHistTotal(UINT8 cam_id)
{
	if (!g_pCamMain)	return 0;
	return g_pCamMain[cam_id-1]->GetGrabHistTotal();
}

/*
 desc : 현재 Grabbed Image에 대해 Column 별로 Gray Level 값 중 가장 큰 값
 parm : cam_id	- [in]  Camera Index (1 or 2)
 retn : Grabbed Image의 Column 별로 Gray Level 합 중 가장 큰 값 반환
*/
API_EXPORT UINT64 uvBasler_GetGrabHistMax(UINT8 cam_id)
{
	if (!g_pCamMain)	return 0;
	return g_pCamMain[cam_id-1]->GetGrabHistMax();
}

/*
 desc : 얼라인 카메라의 Gain Level 값 설정
 parm : cam_id	- [in]  Camera Index (1 or 2)
		level	- [in] 0 ~ 255 (값이 클수록 밝게 처리)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_SetGainLevel(UINT8 cam_id, UINT8 level)
{
	if (!g_pCamMain)	return FALSE;
	if (!g_pCamMain[cam_id-1]->IsConnected())	return FALSE;
	return g_pCamMain[cam_id-1]->SetGainLevel(level);
}

/*
 desc : 수동으로 이미지를 불러와서 데이터를 적재 합니다. (데모 모드에서 주로 사용 됨)
 parm : cam_id	- [in]  Camera Index (1 or 2)
		file	- [in]  전체 경로가 포함됨 265 grayscale bitmap 파일
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_LoadImageFromFile(UINT8 cam_id, PTCHAR file)
{
	return g_pCamMain[cam_id-1]->LoadImageFromFile(file, cam_id);
}


/* --------------------------------------------------------------------------------------------- */
/*                             영상 처리 연결 (Gateway) 함수 부분                                */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Outputs the most recently generated Buffer ID value to the window
 parm : hwnd	- [in]  Handle of window where the image is output
		draw	- [in]  The area of window where the image is output
		cam_id	- [in]  Align Camera ID (1 or 2)
		type	- [in]  0x00: Edge Detection, 0x01: Line Detection, 0x02: Strip Detection (Measurement)
 retn : None
*/
API_EXPORT VOID uvBasler_DrawMBufID(HWND hwnd, RECT draw, UINT8 cam_id, UINT8 type)
{
	if (!g_pCamThread)	return;
	uvMIL_DrawMBufID(hwnd, draw, cam_id, type);
}



/*
 desc : 현재 Grabbed된 이미지 출력 및 저장 (Bitmap을 이용하여 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		cam_id	- [in]  Camera Index (1 or 2)
 retn : None
*/
API_EXPORT VOID uvBasler_DrawGrabBitmap(HDC hdc, RECT draw, UINT8 cam_id)
{
	DOUBLE dbAngle	= g_pstConfig->set_cams.acam_inst_angle == 0 ? 0.0f : 180.0f;
	ENG_VCCM enMode	= g_pCamMain[cam_id-1]->GetCamMode();
	LPG_ACGR pstGrab= NULL;

	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)				return;
	if (ENG_VCCM::en_grab_mode != enMode && ENG_VCCM::en_hole_mode != enMode)	return;

	pstGrab	= g_pCamMain[cam_id-1]->GetGrabbedImage();
	if (!pstGrab)	return;
	// 현재 Grabbed Image 1장 출력
	if (!pstGrab->grab_width || !pstGrab->grab_height)	return;
	
		/* 현재 Grabbed Image 출력 */
		uvMIL_DrawLiveBitmap(hdc, draw, pstGrab, dbAngle);
	
}

/*
 desc : Draw the strip information in the current grabbed image
 parm : hdc		- [in]  The handle of context
		draw	- [in]  The area in which the image is output
		cam_id	- [in]  Camera Index (1 or 2)
		param	- [in]  Strip output information
 retn : None
*/
API_EXPORT VOID uvBasler_DrawStripBitmap(HDC hdc, RECT draw, UINT8 cam_id, LPG_MSMP param)
{
	DOUBLE dbAngle	= g_pstConfig->set_cams.acam_inst_angle == 0 ? 0.0f : 180.0f;
	ENG_VCCM enMode	= g_pCamMain[cam_id-1]->GetCamMode();
	LPG_ACGR pstGrab= NULL;

	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)				return;
	if (ENG_VCCM::en_grab_mode != enMode && ENG_VCCM::en_hole_mode != enMode)	return;

	pstGrab	= g_pCamMain[cam_id-1]->GetGrabbedImage();
	if (!pstGrab)	return;
	// 현재 Grabbed Image 1장 출력
	if (!pstGrab->grab_width || !pstGrab->grab_height)	return;

	/* 현재 Grabbed Image 출력 */
	uvMIL_DrawStripBitmap(hdc, draw, pstGrab, dbAngle, param);
}




/*
 desc : Calibration 이미지 (검색된 결과)를 윈도 영역에 출력 수행 (Bitmap을 이용하여 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
 retn : None
*/
API_EXPORT VOID uvBasler_DrawCaliMarkBitmap(HDC hdc, RECT draw)
{
	if (!g_pCamThread)	return;
	uvMIL_DrawCaliMarkBitmap(hdc, draw);
}

API_EXPORT VOID uvBasler_Camera_SetAlignMotionPtr(AlignMotion& ptr)
{
	if (!g_pCamThread)	return;

	g_pCamThread->SetAlignMotionPtr(ptr);
	
	uvMIL_SetAlignMotionPtr(ptr);
	
}


/*
 desc : 가장 마지막에 Grabbed Image를 출력
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
 retn : None
*/
API_EXPORT VOID uvBasler_DrawLastGrabImage(HDC hdc, RECT draw)
{
	if (!g_pCamThread)	return;
	uvMIL_DrawCaliMarkBitmap(hdc, draw);
}

/*
 desc : 검색된 Mark 이미지 윈도 영역에 출력 수행 (DC를 이용하여 bitmap 이미지로 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_DrawMarkBitmap(HDC hdc, RECT draw, UINT8 cam_id, UINT8 img_id)
{
	UINT8 u8FindMark	= 0x00;

	if (!g_pCamThread)	return FALSE;
	/* 수집된 Camera 정보 유효성 확인 */
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return FALSE;
	/* 검색된 Mark가 유효한지 여부 */
	u8FindMark	= IsValidGrabMarkNum(cam_id, img_id) ? 0x01 : 0x00;
	//u8FindMark = 0x01; // lk91!! 이미지 test 할 때만 사용!! 주석!!!
	/* Grabbed Image에 대한 Mark 정보 그리기 */
	return uvMIL_DrawMarkBitmap(hdc, draw, cam_id, img_id, u8FindMark);
}

/*
 desc : 검색된 Mark 이미지 윈도 영역에 출력 수행 (MIL Buffer ID를 이용한 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_DrawMarkMBufID(HWND hwnd, RECT draw, UINT8 cam_id,UINT8 hwndIdx, UINT8 img_id)
{
	if (!g_pCamThread)	return FALSE;
	/* 수집된 Camera 정보 유효성 확인 */
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return FALSE;
	/* Grabbed Image에 대한 Mark 정보 그리기 */
	uvMIL_DrawMarkMBufID(hwnd, draw, cam_id, hwndIdx,img_id);

	return TRUE;
}

/*
 desc : Calibration 이미지 윈도 영역에 출력 수행 (Bitmap을 이용하여 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		grab	- [in]  grab 이미지가 저장된 구조체 포인터 (grab_data에 이미지가 정보가 반드시 저장되어 있어야 됨)
		find	- [in]  마크 검색 성공 여부 (검색됐더라도, 검색 결과 값에 따라 달라짐)
						0x00 - 검색 결과 실패, 0x01 - 검색 결과 성공
 retn : None
*/
API_EXPORT VOID uvBasler_DrawMarkDataBitmap(HDC hdc, RECT draw, LPG_ACGR grab, UINT8 find,bool drawForce,UINT8 flipFlag)
{
	if (!g_pCamThread)	return;
	uvMIL_DrawMarkDataBitmap(hdc, draw, grab, find,drawForce, flipFlag);
}

/*
 desc : Drawing - Examination Object Image (Bitmap을 이용하여 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		grab_out- [in]  Grabbed Image 정보가 저장된 구조체 포인터 (바깥 링)
		grab_in	- [in]  Grabbed Image 정보가 저장된 구조체 포인터 (안쪽 원)
 retn : None
*/
API_EXPORT VOID uvBasler_DrawGrabExamBitmap(HDC hdc, RECT draw, LPG_ACGR grab_out, LPG_ACGR grab_in)
{
	if (!g_pCamThread)	return;
	uvMIL_DrawGrabExamBitmap(hdc, draw, grab_out, grab_in);
}

/*
 desc : Mark 정보 설정
 parm : cam_id		- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		detail_level- [in]  등록된 모델 이미지와 Grabbed Image이미지의 Edge 영역에 대한 비교 정도 값
							값이 클수록 상세히 비교하지만, 속도는 많이 느려지짐
							LOW보다 HIGH로 갈수록 더 많은 Edge를 가지고 비교하게 됨
							0 (M_MEDIUM), 1 (M_HIGH), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - 기본 값, 1.0f ~ 100.0f
							기본 값은 50이지만, 100으로 갈수록 엣지의 강도가 센것만 나타납니다.
		model		- [in]  Model Type, Param 1 ~ 5개 정보까지 모두 포함된 구조체 포인터
		count		- [in]  등록하고자 하는 모델의 개수	
		scale_min	- [in]  검색 대상의 크기 범위 값 설정 (최소한 이 값 비율보다 커야 됨. 범위: 0.5 ~ 1.0)
		scale_max	- [in]  검색 대상의 크기 범위 값 설정 (최대한 이 값 비율보다 작아야 됨. 범위: 1.0 ~ 1.0)
		score_min	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0)
		score_tgt	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0))
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_SetModelDefine(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
										LPG_CMPV model, UINT8 mark_no,
										DOUBLE scale_min, DOUBLE scale_max,
										DOUBLE score_min, DOUBLE score_tgt)
{
	return uvMIL_SetModelDefine(cam_id, speed, level, count, smooth, model, mark_no,
								scale_min, scale_max, score_min, score_tgt);
}

/*
 desc : 로컬 시스템 (저장소)에 저장되어 있는 패턴 매칭 원본 이미지 적재
 parm : cam_id		- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		detail_level- [in]  등록된 모델 이미지와 Grabbed Image이미지의 Edge 영역에 대한 비교 정도 값
							값이 클수록 상세히 비교하지만, 속도는 많이 느려지짐
							LOW보다 HIGH로 갈수록 더 많은 Edge를 가지고 비교하게 됨
							0 (M_MEDIUM), 1 (M_HIGH), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - 기본 값, 1.0f ~ 100.0f
							기본 값은 50이지만, 100으로 갈수록 엣지의 강도가 센것만 나타납니다.
		model		- [in]  Model Type 즉, circle, square, rectangle, cross, diamond, triangle
		param		- [in]  총 5개의 Parameter Values (unit : um)
		count		- [in]  등록하고자 하는 모델의 개수	
		scale_min	- [in]  검색 대상의 크기 범위 값 설정 (최소한 이 값 비율보다 커야 됨. 범위: 0.5 ~ 1.0)
		scale_max	- [in]  검색 대상의 크기 범위 값 설정 (최대한 이 값 비율보다 작아야 됨. 범위: 1.0 ~ 1.0)
		score_min	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0)
		score_tgt	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0))
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_SetModelDefineEx(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
	PUINT32 model, DOUBLE* param1, DOUBLE* param2, DOUBLE* param3,
	DOUBLE* param4, DOUBLE* param5, UINT8 mark_no,
	DOUBLE scale_min, DOUBLE scale_max,
	DOUBLE score_min, DOUBLE score_tgt, bool sameMark)
{
	if (!g_pCamThread)	return FALSE;
	if (!uvMIL_SetModelDefineEx(cam_id, speed, level, count, smooth, model, param1, param2, param3,
		param4, param5, mark_no,
		scale_min, scale_max, score_min, score_tgt, sameMark))	return FALSE;
	return TRUE;
}

/*
 desc : Mark 정보 설정 - 이미지 데이터
 parm : cam_id		- [in]  Align Camera Index (1 or 2)
		speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		level		- [in]  등록된 모델 이미지와 Grabbed Image이미지의 Edge 영역에 대한 비교 정도 값
							값이 클수록 상세히 비교하지만, 속도는 많이 느려지짐
							LOW보다 HIGH로 갈수록 더 많은 Edge를 가지고 비교하게 됨
							0 (M_MEDIUM), 1 (M_HIGH), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - 기본 값, 1.0f ~ 100.0f
							기본 값은 50이지만, 100으로 갈수록 엣지의 강도가 센것만 나타납니다.
		scale_min	- [in]  검색 대상의 크기 범위 값 설정 (최소한 이 값 비율보다 커야 됨. 범위: 0.5 ~ 1.0)
		scale_max	- [in]  검색 대상의 크기 범위 값 설정 (최대한 이 값 비율보다 작아야 됨. 범위: 1.0 ~ 1.0)
		score_min	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0)
		score_tgt	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0))
		name		- [in]  Model Name
		file		- [in]  모델 이미지가 저장된 파일 이름 (전체 경로 포함. Image File)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_SetModelDefineLoad(UINT8 cam_id, UINT8 speed, UINT8 level, DOUBLE smooth,
											DOUBLE scale_min, DOUBLE scale_max,
											DOUBLE score_min, DOUBLE score_tgt,
											PTCHAR name, CStringArray &file)
{
	if (!g_pCamThread)	return FALSE;
	return uvMIL_SetModelDefineLoad(cam_id, speed, level, smooth,
									scale_min, scale_max, score_min, score_tgt, name, file);
}

/*
 desc : 현재 등록된 Mark Model 정보 반환
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : 등록된 구조체 포인터 반환
*/
API_EXPORT LPG_CMPV uvBasler_GetModelDefine(UINT8 cam_id)
{
	return uvMIL_GetModelDefine(cam_id);
}

/*
 desc : 현재 검색을 위해 등록된 모델의 개수 반환
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : 등록된 개수
*/
API_EXPORT UINT32 uvBasler_GetModelRegistCount(UINT8 cam_id)
{
	if (!g_pCamThread)	return 0;
	return uvMIL_GetModelRegistCount(cam_id);
}

/*
 desc : 기존에 적재 (등록)되어 있던 모든 Mark 해제 수행
 parm : None
 retn : None
*/
API_EXPORT VOID uvBasler_ResetMarkModel()
{
	if (!g_pCamThread)	return;
	uvMIL_ResetMarkModel();
}

/*
 desc : Mark Template가 등록되어 있는지 확인
 parm : mode	- [in]  0x00 : Expose, 0x01 : Vstep
		cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : TRUE - 등록되어 있다. FALSE - 등록되지 않았다.
*/
API_EXPORT BOOL uvBasler_IsSetMarkModel(UINT8 mode, UINT8 cam_id, UINT8 fi_No = 0x00)
{
	if (!g_pCamThread)	return FALSE;
	return uvMIL_IsSetMarkModel(mode, cam_id, fi_No);
}

/*
 desc : Mark Pattern 등록 여부
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_IsSetMarkModelACam(UINT8 cam_id, UINT8 mark_no)
{
	if (!g_pCamThread)	return FALSE;
	return uvMIL_IsSetMarkModelACam(cam_id, mark_no);
}

/*
 desc : 모든 카메라 (카메라 개수에 따라)에 Mark Model이 설정되었는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_IsSetMarkModelAll(UINT8 mark_no)
{
	UINT8 i	= 0x00;

	if (!g_pCamThread)	return FALSE;
	for (; i<g_pstConfig->set_cams.acam_count; i++)
	{
		if (!uvMIL_IsSetMarkModelACam(i+1, mark_no))	return FALSE;
	}
	return TRUE;
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
API_EXPORT BOOL uvBasler_SaveGrabbedMarkToFile(UINT8 cam_id, LPRECT area, UINT8 type, PTCHAR file)
{
	if (!g_pCamThread)	return FALSE;
	return uvMIL_SaveGrabbedMarkToFile(cam_id, area, type, file);
}

/*
 desc : Edge Detection 검색된 결과 반환
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
 retn : 결과 값이 저장된 포인터 반환
*/
API_EXPORT LPG_EDFR uvBasler_GetEdgeDetectResults(UINT8 cam_id)
{
	if (!g_pCamThread)	return NULL;
	return uvMIL_GetEdgeDetectResults(cam_id);
}

/*
 desc : Edge Detection 검색된 개수 반환
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
 변환 : 개수 반환. 실패할 경우, 0 이하 값
*/
API_EXPORT INT32 uvBasler_GetEdgeDetectCount(UINT8 cam_id)
{
	if (!g_pCamThread)	return -1;
	return uvMIL_GetEdgeDetectCount(cam_id);
}

/*
 desc : 현재 등록된 Mark Model Type 반환
 parm : cam_id	- [in]  Align Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		mark_id	- [in]  Align Mark Index (0x00 ~ MAX_REGIST_MODEL-1)
 retn : Model Type 값
*/
API_EXPORT UINT32 uvBasler_GetMarkModelType(UINT8 cam_id, UINT8 index)
{
	if (!g_pCamThread)	return 0;
	return uvMIL_GetMarkModelType(cam_id, index);
}

/*
 desc : 모델 크기 반환
 parm : cam_id	- [in]  Grabbed Image 정보가 발생된 Align Camera Index (1 or 2)
		index	- [in]  요청하고자 하는 모델의 위치 (Zero-based)
		flag	- [in]  0x00 : 가로  크기, 0x01 : 세로 크기
		unit	- [in]  0x00 : um, 0x01 : pixel
 retn : 크기 반환 (단위: um)
*/
API_EXPORT DOUBLE uvBasler_GetMarkModelSize(UINT8 cam_id, UINT8 index, UINT8 flag, UINT8 unit)
{
	if (!g_pCamThread)	return 0.0f;
	return uvMIL_GetMarkModelSize(cam_id, index, flag, unit);
}

/*
 desc : 이미지의 가로 혹은 세로의 경계선 중심 위치 얻기
 parm : cam_id		- [in]  Align Camera ID (1 or 2)
		grab_data	- [in]  Grabbed Image 정보가 저장된 구조체 포인터
		mil_result	- [in]  MIL 내부 검색 결과 대화 상자 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_RunLineCentXY(UINT8 cam_id, LPG_ACGR grab_data, BOOL mil_result)
{
	return uvMIL_RunLineCentXY(cam_id, grab_data, mil_result);
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
API_EXPORT VOID uvBasler_ImageRotate(PUINT8 img_src, PUINT8 img_dst,
									 UINT32 width, UINT32 height, UINT32 angle)
{
	if (!g_pCamThread)	return;
	uvMIL_ImageRotate(img_src, img_dst, width, height, angle);
}

/*
 desc : 현재 MIL 라이선스가 유효한지 여부 확인
		Pattern, Model Finder, Edge Finder, Read, OCR, Image Processing 등 유효한지 확인
 parm : None
 retn : TRUE or FALS
*/
API_EXPORT BOOL uvBasler_IsLicenseValid()
{
	if (!g_pCamThread)	return FALSE;
	return uvMIL_IsLicenseValid();
}

/*
 desc : Align Mark 검색 방식 설정
 parm : method	- [in]  0x00 : 최적의 1개 반환, 0x01 : 다점 마크 기준으로 가중치를 부여하여 결과 반환
		count	- [in]  'method' 값이 1인 경우, 최종 검색될 Mark 개수 값 2 이상 값이어야 됨
 retn : None
*/
API_EXPORT VOID uvBasler_SetMarkMethod(ENG_MMSM method, UINT8 count)
{
	uvMIL_SetMarkMethod(method, count);
}


API_EXPORT UINT8 uvBasler_SetMarkFoundCount(int camNum)
{
	return uvMIL_SetMarkFoundCount(camNum);
}


/*
 desc : 노광 모드 설정 즉, 직접 노광, 얼라인 노광, 보정 후 얼라인 노광
 parm : mode	- [in]  직접 노광 (0x00), 얼라인 노광 (0x01), 얼라인 카메라 보정 값 적용 후 얼라인 노광 (0x02)
 retn : None
*/
API_EXPORT VOID uvBasler_SetAlignMode(ENG_AOEM mode)
{
	if (!g_pCamThread)	return;
	uvMIL_SetAlignMode(mode);
}

/*
 desc : 현재 적재된 레시피의 마크 검색 조건 값 설정
 parm : score	- [in]  Score Accpetance (대상 이미지에서 검색된 마크의 Score 값이 이 값보다 작으면 버린다)
		scale	- [in]  Scale Range (대상 이미지에서 등록된 마크를 검색 결과들 중에서 이 비율 범위 내에 )
									(포함된 결과들만 유효하고, 그 중에서 Score 값이 가장 높은 것만 추출)
									(가령. 이 값이 50.0000 이면, 실제 검색에 사용되는 마크의 크기 범위)
									(검색범위 = 1.0f - (100.0 - 50.0) / 100 임 즉, 0.5 ~ 1.5 임)
 retn : None
*/
API_EXPORT VOID uvBasler_SetRecipeMarkRate(DOUBLE score, DOUBLE scale)
{
	return uvMIL_SetRecipeMarkRate(score, scale);
}

/*
 desc : 스테이지의 이동 방향 설정 (정방향 이동인지 / 역방향 이동인지 여부)
 parm : direct	- [in]  TRUE (정방향 이동 : 앞에서 뒤로 이동), FALSE (역방향 이동)
 retn : None
*/
API_EXPORT VOID uvBasler_SetMoveStateDirect(BOOL direct)
{
	uvMIL_SetMoveStateDirect(direct);
}

/*
 desc : Grabbed Image 마다 표현된 마크의 개수가 여러 개인 경우, 마크 들이 분포된 영역의 크기 값 설정
 parm : width	- [in]  사각형 영역의 넓이 값 (단위: um)
		height	- [in]  사각형 영역의 높이 값 (단위: um)
 retn : None
*/
API_EXPORT VOID uvBasler_SetMultiMarkArea(UINT32 width, UINT32 height)
{
	uvMIL_SetMultiMarkArea(width, height);
}

/* --------------------------------------------------------------------------------------------- */
/*                                 lk91 VISION 추가 함수                                         */
/* --------------------------------------------------------------------------------------------- */

/* desc : Mark 정보 설정 - MMF (MIL Model Find File) */
API_EXPORT BOOL uvBasler_SetModelDefineMMF(UINT8 cam_id, PTCHAR name, PTCHAR mmf, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no)
{
	if (!g_pCamThread)	return FALSE;
	return uvMIL_SetModelDefineMMF(cam_id, name, mmf, m_MarkSizeP, m_MarkCenterP, mark_no);
}

/* desc : Mark 정보 설정 - PAT (MIL Pattern Matching File) */
API_EXPORT BOOL uvBasler_SetModelDefinePAT(UINT8 cam_id, PTCHAR name, PTCHAR pat, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no)
{
	if (!g_pCamThread)	return FALSE;
	return uvMIL_SetModelDefinePAT(cam_id, name, pat, m_MarkSizeP, m_MarkCenterP, mark_no);
}

/* desc : Set Display Type */
API_EXPORT VOID uvBasler_SetDispType(UINT8 dispType)
{
	UINT8 i = 0;
	if (!g_pCamMain)	return;

#if 1
	for (; i < g_pstConfig->set_cams.acam_count; i++)
	{
		g_pCamMain[i]->SetDispType(dispType);
	}
#else
	g_pCamMain[0]->SetCamMode(mode);
#endif
}

/* desc : MarkROI 초기화 */
API_EXPORT VOID uvBasler_initMarkROI(LPG_CRD fi_MarkROI)
{
	uvMIL_InitMarkROI(fi_MarkROI);
}

/* desc : Set Live Disp Size */
API_EXPORT VOID uvBasler_SetMarkLiveDispSize(CSize fi_size)
{
	uvMIL_SetMarkLiveDispSize(fi_size);
}

/* desc : Set CalbCamSpec Disp Size */
API_EXPORT VOID uvBasler_SetCalbCamSpecDispSize(CSize fi_size)
{
	uvMIL_SetCalbCamSpecDispSize(fi_size);
}

API_EXPORT VOID uvBasler_SetAccuracyMeasureDispSize(CSize fi_size)
{
	uvMIL_SetAccuracyMeasureDispSize(fi_size);
}

/* desc : Set CalbStep Disp Size */
API_EXPORT VOID uvBasler_SetCalbStepDispSize(CSize fi_size)
{
	uvMIL_SetCalbStepDispSize(fi_size);
}

/* desc : Set MMPM Disp Size */
API_EXPORT VOID uvBasler_SetMMPMDispSize(CSize fi_size)
{
	uvMIL_SetMMPMDispSize(fi_size);
}

/* desc : 최종 검색된 Live 이미지 윈도 영역에 출력 수행 (Bitmap을 이용하여 출력) */
API_EXPORT VOID uvBasler_DrawLiveBitmap(HDC hdc, RECT draw, UINT8 cam_id, BOOL save)
{
	DOUBLE dbAngle = g_pstConfig->set_cams.acam_inst_angle == 0 ? 0.0f : 180.0f;
	LPG_ACGR pstGrab = NULL;

	if (!g_pCamThread)	return;
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return;
	if (ENG_VCCM::en_grab_mode != g_pCamMain[cam_id - 1]->GetCamMode())
	{
		pstGrab = g_pCamMain[cam_id - 1]->GetGrabbedImage();
		if (!pstGrab)	return;
		// 현재 Grabbed Image 1장 출력
		if (pstGrab->grab_width && pstGrab->grab_height)
		{
			/* 이전에 그렸던 시간과 비교해서 일정 시간 지났는지 확인 */
			if (GetTickCount64() > g_u64DrawDelayTime[cam_id - 1] + GRAB_DRAW_DELAY_TIME)
			{
				uvMIL_DrawLiveBitmap(hdc, draw, pstGrab, dbAngle);
				/* 다음에 그릴 때, 일정 시간 지연을 위해서 */
				g_u64DrawDelayTime[cam_id - 1] = GetTickCount64();

				/* 파일로 저장 여부 */
				if (save)
				{
					TCHAR tzFile[MAX_PATH_LEN] = { NULL };
					/* 현재 시간 얻기 */
					SYSTEMTIME stTime = { NULL };
					GetLocalTime(&stTime);
					/* Live 파일 저장 */
					swprintf_s(tzFile, L"%s\\live\\%04d-%02d-%02d %02d%02d%02d.bmp",
						g_tzWorkDir, stTime.wYear, stTime.wMonth, stTime.wDay,
						stTime.wHour, stTime.wMinute, stTime.wSecond);
					uvMIL_SaveGrabbedToFile(tzFile, pstGrab->grab_width, pstGrab->grab_height, pstGrab->grab_data);
				}
			}
		}
	}
}

/* desc : MIL ID 로 저장된 이미지 화면 출력 */
API_EXPORT VOID uvBasler_DrawImageBitmap(int dispType, int Num, UINT8 cam_id, BOOL save,int flipDir)
{
	DOUBLE dbAngle = g_pstConfig->set_cams.acam_inst_angle == 0 ? 0.0f : 180.0f;
	LPG_ACGR pstGrab = NULL;

	if (!g_pCamThread)	return;
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return;
	//if (ENG_VCCM::en_image_mode == g_pCamMain[cam_id - 1]->GetCamMode())
	{
		pstGrab = g_pCamMain[cam_id - 1]->GetGrabbedImage();
		if (!pstGrab)	return;
		// 현재 Grabbed Image 1장 출력
		if (pstGrab->grab_width && pstGrab->grab_height)
		{
			/* 이전에 그렸던 시간과 비교해서 일정 시간 지났는지 확인 */
			//if (GetTickCount64() > g_u64DrawDelayTime[cam_id - 1] + GRAB_DRAW_DELAY_TIME)
			//{

			uvMIL_DrawImageBitmapFlip(dispType, 0, pstGrab, dbAngle, cam_id - 1,flipDir);
			
			/* 다음에 그릴 때, 일정 시간 지연을 위해서 */
			//g_u64DrawDelayTime[cam_id - 1] = GetTickCount64();

			/* 파일로 저장 여부 */
			if (save)
			{
				TCHAR tzFile[MAX_PATH_LEN] = { NULL };
				/* 현재 시간 얻기 */
				SYSTEMTIME stTime = { NULL };
				GetLocalTime(&stTime);
				/* Live 파일 저장 */
				swprintf_s(tzFile, L"%s\\live\\%04d-%02d-%02d %02d%02d%02d.bmp",
					g_tzWorkDir, stTime.wYear, stTime.wMonth, stTime.wDay,
					stTime.wHour, stTime.wMinute, stTime.wSecond);
				uvMIL_SaveGrabbedToFile(tzFile, pstGrab->grab_width, pstGrab->grab_height, pstGrab->grab_data);
			}
			//}
		}
	}
}

/* desc : Set Mark ROI */
API_EXPORT VOID uvBasler_MilSetMarkROI(UINT8 cam_id, CRect fi_rectSearhROI)
{
	uvMIL_MilSetMarkROI(cam_id, fi_rectSearhROI);
}

/* desc: pat Mark 등록 */
API_EXPORT BOOL uvBasler_RegistPat(UINT8 cam_id, CRect fi_rectArea, CString fi_filename, UINT8 mark_no)
{
	LPG_ACGR pstGrab = NULL;
	pstGrab = g_pCamMain[cam_id - 1]->GetGrabbedImage();
	if (!pstGrab)
		return FALSE;
	return uvMIL_RegistPat(cam_id, pstGrab, fi_rectArea, fi_filename, mark_no);
}

/* desc: mmf Mark 등록 */
API_EXPORT BOOL uvBasler_RegistMod(UINT8 cam_id, CRect fi_rectArea, CString fi_filename, UINT8 mark_no)
{
	LPG_ACGR pstGrab = NULL;
	pstGrab = g_pCamMain[cam_id - 1]->GetGrabbedImage();
	if (!pstGrab)
		return FALSE;
	return uvMIL_RegistMod(cam_id, pstGrab, fi_rectArea, fi_filename, mark_no);
}

/* desc: MMPM AutoCenter 이미지 등록 */
API_EXPORT BOOL uvBasler_RegistMMPM_AutoCenter(CRect fi_rectArea, UINT8 cam_id, UINT8 img_id)
{
	return uvMIL_RegistMMPM_AutoCenter(fi_rectArea, cam_id, img_id);
}

/* desc: Mark Size, Offset 초기화 */
API_EXPORT VOID uvBasler_InitSetMarkSizeOffset(UINT8 cam_id, TCHAR* file, UINT8 fi_findType, UINT8 mark_no)
{
	uvMIL_InitSetMarkSizeOffset(cam_id, file, fi_findType, mark_no);
}

/* desc: Draw Mark Display */
API_EXPORT VOID uvBasler_PutMarkDisp(HWND hwnd, int fi_iNo, RECT draw, UINT8 cam_id, TCHAR* file, int fi_findType)
{
	uvMIL_PutMarkDisp(hwnd, fi_iNo, draw, cam_id, file, fi_findType);
}

/* desc: Draw Set Mark Display */
API_EXPORT BOOL uvBasler_PutSetMarkDisp(HWND hwnd, RECT draw, UINT8 cam_id, TCHAR* file, int fi_findType, DOUBLE fi_dRate)
{
	return uvMIL_PutSetMarkDisp(hwnd, draw, cam_id, file, fi_findType, fi_dRate);
}

/* desc: Get Mark Size */
API_EXPORT CPoint uvBasler_GetMarkSize(UINT8 cam_id, int fi_No)
{
	return uvMIL_GetMarkSize(cam_id, fi_No);
}

/* desc: Get Mark Setting Disp Rate */
API_EXPORT DOUBLE uvBasler_GetMarkSetDispRate()
{
	return uvMIL_GetMarkSetDispRate();
}

/* desc: Set Mark Setting Disp Rate */
API_EXPORT DOUBLE uvBasler_SetMarkSetDispRate(DOUBLE fi_dRate)
{
	return uvMIL_SetMarkSetDispRate(fi_dRate);
}

/* desc: Get Mark Offset */
API_EXPORT CPoint uvBasler_GetMarkOffset(UINT8 cam_id, BOOL bNewOffset, int fi_No)
{
	return uvMIL_GetMarkOffset(cam_id, bNewOffset, fi_No);
}

/* desc: Get Mask Buf */
API_EXPORT BOOL uvBasler_MaskBufGet()
{
	return uvMIL_MaskBufGet();
}

/* desc: Get Mark Find Mode */
API_EXPORT UINT8 uvBasler_GetMarkFindMode(UINT8 cam_id, UINT8 mark_no)
{
	return uvMIL_GetMarkFindMode(cam_id, mark_no);
}

/* desc: Set Mark Find Mode */
API_EXPORT BOOL uvBasler_SetMarkFindMode(UINT8 cam_id, UINT8 find_mode, UINT8 mark_no)
{
	return uvMIL_SetMarkFindMode(cam_id, find_mode, mark_no);
}

/* desc: Clear Mask(MMF) */
API_EXPORT VOID uvBasler_MaskClear_MOD(UINT8 cam_id, CPoint fi_iSizeP, UINT8 mark_no)
{
	return uvMIL_MaskClear_MOD(cam_id, fi_iSizeP, mark_no);
}

/* desc: Clear Mask(PAT) */
API_EXPORT VOID uvBasler_MaskClear_PAT(UINT8 cam_id, CPoint fi_iSizeP, UINT8 mark_no)
{
	return uvMIL_MaskClear_PAT(cam_id, fi_iSizeP, mark_no);
}

/* desc: Find Center (Mark Set에서만 사용) */
API_EXPORT VOID uvBasler_CenterFind(int cam_id, int fi_length, int fi_curSmoothness, double* fi_NumEdgeMIN_X, double* fi_NumEdgeMAX_X, double* fi_NumEdgeMIN_Y, double* fi_NumEdgeMAX_Y, int* fi_NumEdgeFound, int fi_Mode)
{
	return uvMIL_CenterFind(cam_id, fi_length, fi_curSmoothness, fi_NumEdgeMIN_X, fi_NumEdgeMAX_X, fi_NumEdgeMIN_Y, fi_NumEdgeMAX_Y, fi_NumEdgeFound, fi_Mode);
}

/* desc: Set Mark Size */
API_EXPORT VOID uvBasler_SetMarkSize(UINT8 cam_id, CPoint fi_MarkSize, int setMode, int fi_No)
{
	uvMIL_SetMarkSize(cam_id, fi_MarkSize, setMode, fi_No);
}

/* desc: Set Mark Offset */
API_EXPORT VOID uvBasler_SetMarkOffset(UINT8 cam_id, CPoint fi_MarkCenter, int setOffsetMode, int fi_No)
{
	uvMIL_SetMarkOffset(cam_id, fi_MarkCenter, setOffsetMode, fi_No);
}

/* desc: MMF 파일에 MASK 정보 저장, Mark Set에서만 사용 */
API_EXPORT VOID uvBasler_SaveMask_MOD(UINT8 cam_id, UINT8 mark_no)
{
	uvMIL_SaveMask_MOD(cam_id, mark_no);
}

/* desc: PAT 파일에 MASK 정보 저장, Mark Set에서만 사용 */
API_EXPORT VOID uvBasler_SaveMask_PAT(UINT8 cam_id, UINT8 mark_no)
{
	uvMIL_SaveMask_PAT(cam_id, mark_no);
}

/* desc: PAT MARK 저장 */
API_EXPORT VOID uvBasler_PatMarkSave(UINT8 cam_id, CString fi_strFileName, UINT8 mark_no)
{
	uvMIL_PatMarkSave(cam_id, fi_strFileName, mark_no);
}

/* desc: MMF MARK 저장 */
API_EXPORT VOID uvBasler_ModMarkSave(UINT8 cam_id, CString fi_strFileName, UINT8 mark_no)
{
	uvMIL_ModMarkSave(cam_id, fi_strFileName, mark_no);
}

/* desc: Mask 초기화 */
API_EXPORT VOID uvBasler_InitMask(UINT8 cam_id)
{
	uvMIL_InitMask(cam_id);
}

/* desc: Mil Main 할당 변수 해제 */
API_EXPORT VOID uvBasler_CloseSetMark()
{
	uvMIL_CloseSetMark();
}

/* desc: MARK DISP ID 할당 */
API_EXPORT VOID uvBasler_SetDispMark(CWnd *pWnd)
{
	uvMIL_SetDispMark(pWnd);
}

/* desc: MARK DISP ID 할당 */
API_EXPORT VOID uvBasler_SetDispRecipeMark(CWnd* pWnd[2])
{
	uvMIL_SetDispRecipeMark(pWnd);
}

/* desc: MARK DISP ID 할당 */
API_EXPORT VOID uvBasler_SetDispExpo(CWnd* pWnd[4])
{
	uvMIL_SetDispExpo(pWnd);
}


/* desc: Mark 정보 그릴 때 사용하는 MIL 함수 */
API_EXPORT VOID uvBasler_DrawMarkInfo_UseMIL(UINT8 cam_id, UINT8 fi_smooth, UINT8 mark_no)
{
	uvMIL_DrawMarkInfo_UseMIL(cam_id, fi_smooth, mark_no);
}

/* desc: Set Mask */
API_EXPORT VOID uvBasler_Mask_MarkSet(UINT8 cam_id, CRect rectTmp, CPoint iTmpSizeP, CRect rectFill, int fi_color, bool bMask)
{
	uvMIL_Mask_MarkSet(cam_id, rectTmp, iTmpSizeP, rectFill, fi_color, bMask);
}

/*
	desc : Overlay 관련 함수 - 전체 Overlay Clear or DC 출력
	fi_bDrawFlag : FALSE - CLEAR
	fi_iDispType : 0:Expo, 1:mark, 2 : Live, 3 : mark set
	fi_iNo : Cam Num 혹은 Grab Mark Num (각자 Disp Type 에 맞춰서 사용)
*/
API_EXPORT VOID uvBasler_DrawOverlayDC(bool fi_bDrawFlag, int fi_iDispType, int fi_iNo)
{
	uvMIL_DrawOverlayDC(fi_bDrawFlag, fi_iDispType, fi_iNo);
}


API_EXPORT VOID uvBasler_Camera_ClearShapes(int fi_iDispType)
{
	uvMIL_Camera_ClearShapes(fi_iDispType);
}





/* desc : Overlay 관련 함수 - Box List 추가 */
API_EXPORT VOID uvBasler_OverlayAddBoxList(int fi_iDispType, int fi_iNo, int fi_iLeft, int fi_iTop, int fi_iRight, int fi_iBottom, int fi_iStyle, int fi_color)
{
	uvMIL_OverlayAddBoxList(fi_iDispType, fi_iNo, fi_iLeft, fi_iTop, fi_iRight, fi_iBottom, fi_iStyle, fi_color);
}

/* desc : Overlay 관련 함수 - Cross List 추가 */
API_EXPORT VOID uvBasler_OverlayAddCrossList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, int fi_iWdt1, int fi_iWdt2, int fi_color)
{
	uvMIL_OverlayAddCrossList(fi_iDispType, fi_iNo, fi_iX, fi_iY, fi_iWdt1, fi_iWdt2, fi_color);
}

/* desc : Overlay 관련 함수 - Text List 추가 */
API_EXPORT VOID uvBasler_OverlayAddTextList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, CString fi_sText, int fi_color, int fi_iSizeX, int fi_iSizeY, CString fi_sFont, bool fi_bEgdeFlag)
{
	uvMIL_OverlayAddTextList(fi_iDispType, fi_iNo, fi_iX, fi_iY, fi_sText, fi_color, fi_iSizeX, fi_iSizeY, fi_sFont, fi_bEgdeFlag);
}

/* desc : Overlay 관련 함수 - Line List 추가 */
API_EXPORT VOID uvBasler_OverlayAddLineList(int fi_iDispType, int fi_iNo, int fi_iSx, int fi_iSy, int fi_iEx, int fi_iEy, int fi_iStyle, int fi_color)
{
	uvMIL_OverlayAddLineList(fi_iDispType, fi_iNo, fi_iSx, fi_iSy, fi_iEx, fi_iEy, fi_iStyle, fi_color);
}

/* desc: MARK SET DISP ID 할당 */
API_EXPORT VOID uvBasler_SetDispMarkSet(CWnd* pWnd)
{
	uvMIL_SetDispMarkSet(pWnd);
}

/* desc: MMPM AutoCenter DISP ID 할당 */
API_EXPORT VOID uvBasler_SetDispMMPM_AutoCenter(CWnd* pWnd)
{
	uvMIL_SetDispMMPM_AutoCenter(pWnd);
}

/* desc: LIVE DISP ID 할당 */
API_EXPORT VOID uvBasler_SetDisp(CWnd** pWnd, UINT8 fi_Mode)
{
	uvMIL_SetDisp(pWnd, fi_Mode);
}

/* desc: MMPM DISP ID 할당 */
API_EXPORT VOID uvBasler_SetDispMMPM(CWnd* pWnd)
{
	uvMIL_SetDispMMPM(pWnd);
}

/* desc : Zoom 관련 함수 - Zoom In */
API_EXPORT VOID uvBasler_MilZoomIn(int fi_iDispType, int cam_id, CRect rc)
{
	uvMIL_MilZoomIn(fi_iDispType, cam_id, rc);
}

/* desc : Zoom 관련 함수 - Zoom Out */
API_EXPORT BOOL uvBasler_MilZoomOut(int fi_iDispType, int cam_id, CRect rc)
{
	return uvMIL_MilZoomOut(fi_iDispType, cam_id, rc);
}

/* desc : Zoom 관련 함수 - Zoom Fit, 이미지 비율 1.0 */
API_EXPORT VOID uvBasler_MilAutoScale(int fi_iDispType, int cam_id)
{
	uvMIL_MilAutoScale(fi_iDispType, cam_id);
}

/* desc : Zoom 관련 함수 - Mouse Point 저장, Zoom으로 확대되었을 때 이미지 이동하기 위함 */
API_EXPORT VOID uvBasler_SetZoomDownP(int fi_iDispType, int cam_id, CPoint fi_point)
{
	uvMIL_SetZoomDownP(fi_iDispType, cam_id, fi_point);
}

/* desc : Zoom 관련 함수 - 이미지 이동 */
API_EXPORT VOID uvBasler_MoveZoomDisp(int fi_iDispType, int cam_id, CPoint fi_point, CRect fi_rect)
{
	uvMIL_MoveZoomDisp(fi_iDispType, cam_id, fi_point, fi_rect);
}

/* desc : Zoom 관련 함수 - Zoom 된 영상에서 클릭된 좌표를 이미지 좌표로 변환시켜주는 작업 */
API_EXPORT CDPoint uvBasler_trZoomPoint(int fi_iDispType, int cam_id, CPoint fi_point)
{
	return uvMIL_trZoomPoint(fi_iDispType, cam_id, fi_point);
}

API_EXPORT BOOL uvBasler_SetModelDefine_tot(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
	LPG_CMPV model, UINT8 fi_No, TCHAR* file,
	DOUBLE scale_min, DOUBLE scale_max,
	DOUBLE score_min, DOUBLE score_tgt)
{
	return uvMIL_SetModelDefine_tot(cam_id, speed, level, count, smooth, model, fi_No, file,
		scale_min, scale_max, score_min, score_tgt);
}

API_EXPORT BOOL uvBasler_MergeMark(UINT8 cam_id, LPG_CMPV value, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, UINT8 mark_no, TCHAR* file1, TCHAR* file2,
	TCHAR* RecipeName, DOUBLE scale_min, DOUBLE scale_max,
	DOUBLE score_min, DOUBLE score_tgt)
{
	return uvMIL_MergeMark(cam_id, value, speed, level, count, smooth, mark_no, file1, file2,
		RecipeName, scale_min, scale_max, score_min, score_tgt);
}

API_EXPORT BOOL uvBasler_ProcImage(UINT8 cam_id, UINT8 imgProc)
{
	return uvMIL_ProcImage(cam_id, imgProc);
	//return g_pCamMain[cam_id - 1]->LoadImageFromFile(cam_id);
}

API_EXPORT CDPoint uvBasler_RunModel_VisionCalib(UINT8 cam_id, UINT8 dlg_id, UINT8 mark_no, int* roi_left, int* roi_right, int* roi_top, int* roi_bottom, int row, int col)
{
	CDPoint MarkP;
	MarkP.x = 0;
	MarkP.y = 0;
	/* New Grabbed Image 저장 */
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return MarkP;
	return g_pCamThread->RunModel_VisionCalib(cam_id, dlg_id, mark_no, roi_left, roi_right, roi_top, roi_bottom, row, col);
}
#ifdef __cplusplus
}
#endif
