
/*
 desc : Data Library
*/

#include "pch.h"
#include "MainApp.h"

/* User Header */
#include "PODIS.h"
#include "GFSS.h"
#include "GFSC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/* --------------------------------------------------------------------------------------------- */
/*                                           전역 변수                                           */
/* --------------------------------------------------------------------------------------------- */

/* 현재 Admin 계정으로 로그인을 수행했는지 여부 */
BOOL					g_bAdminLogin		= FALSE;

/* 무한 루프 여부 판단을 위한 시간 값 */
UINT64					g_u64LoopTimeOut	= 0;

/* 각 데이터를 저장 및 관리하는 객체 */
CPODIS					*g_pPODIS;
CGFSS					*g_pGFSS;
CGFSC					*g_pGFSC;


/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */



/* --------------------------------------------------------------------------------------------- */
/*                                           외부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : 라이브러리 초기화
 parm : work_dir	- [in]  작업 기본 경로 (Full path)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSP_Init(PTCHAR work_dir)
{
	LPG_CSID pstConfig	= NULL;

	/* Information Config File */
	g_pPODIS	= new CPODIS(work_dir);
	ASSERT(g_pPODIS);
	if (!g_pPODIS->LoadConfig())
	{
 		AfxMessageBox(L"Failed to load the config file for <Init>", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* 젹재된 환경 정보 구조체 포인터 얻기 */
	pstConfig	= g_pPODIS->GetConfig();
	/* Gerber File Sync. Server 객체 생성 */
	g_pGFSS	= new CGFSS(pstConfig);
	ASSERT(g_pGFSS);
	/* Gerber File Sync. Client 객체 생성 */
	g_pGFSC	= new CGFSC(pstConfig);
	ASSERT(g_pGFSC);

	return TRUE;
}

/*
 desc : 라이브러리 해제
 parm : None
 retn : None
*/
API_EXPORT VOID uvGFSP_Close()
{
	if (g_pGFSS)		delete g_pGFSS;
	if (g_pGFSC)		delete g_pGFSC;
	if (g_pPODIS)		delete g_pPODIS;
}

/*
 desc : 환경 파일 저장
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSP_Save()
{
	return g_pPODIS ? g_pPODIS->SaveConfig() : FALSE;
}

/*
 desc : 환경 파일 재적재
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSP_ReloadConfig()
{
	return g_pPODIS ? g_pPODIS->LoadConfig() : FALSE;
}

/*
 desc : 무한 루프 시간 비교를 위한 시간 값 초기화 (현재 CPU Tick Count로 초기화)
 parm : None
 retn : None
*/
API_EXPORT VOID uvGFSP_ResetLoopTime()
{
#if (USE_CHECK_LIMIT_LOOP_TIME)
	g_u64LoopTimeOut	= GetTickCount64();
#endif
}

/*
 desc : 주어진 시간 (Time Tick) 기준으로 무한 루프에 빠져 있는지 여부 확인
 parm : file	- [in]  호출된 Source File Name
		line	- [in]  호출된 Source Code의 Line Number
		limit	- [in]  이 시간 (CPU Time Tick) 이후 만큼 대기 시간을 초과 했는지 검사하기 위한 Time Tick 값
 retn : TRUE or FALSE
*/
#if (USE_CHECK_LIMIT_LOOP_TIME)
API_EXPORT BOOL uvGFSP_IsLoopTimeOut(PTCHAR file, UINT32 line, UINT64 limit)
{
	if (GetTickCount64() > (g_u64LoopTimeOut + limit))
	{
#ifdef _DEBUG
		TCHAR tzMesg[296]	= {NULL};
		swprintf_s(tzMesg, 296, L"%s / %d", file, line);
		uvLogs_SaveMesg(en_edic_proc, en_lnwe_error, tzMesg);
#endif
		return TRUE;
	}
	return FALSE;
}
#else
API_EXPORT BOOL uvGFSP_IsLoopTimeOut()
{
	return FALSE;
}
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                             Init                                              */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Init 설정 정보 반환
 parm : None
 retn : Init 설정 정보가 저장된 구조체 포인터 반환
*/
API_EXPORT LPG_CSID uvGFSP_Info_GetData()
{
	return g_pPODIS ? g_pPODIS->GetConfig() : NULL;
}

/* --------------------------------------------------------------------------------------------- */
/*                                            Common                                             */
/* --------------------------------------------------------------------------------------------- */


#ifdef __cplusplus
}
#endif