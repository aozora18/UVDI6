
/*
 desc : Logs Library
*/

#include "pch.h"
#include "MainApp.h"

#include "LogData.h"

#include "../../prj/UVDI15/param/LogManager.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/* --------------------------------------------------------------------------------------------- */
/*                                         구조체 변수                                           */
/* --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- */
/*                                           전역 변수                                           */
/* --------------------------------------------------------------------------------------------- */

UINT8					g_u8LogType;	/* 0x00 : Text File, 0x01 : Database File (SQLite3) */
CLogData				*g_pLogData;
CMySection				g_syncLogs;


/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */


#ifdef __cplusplus
extern "C"
{
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                외부 함수 - Only Shared Memory                                 */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 라이브러리 초기화
 parm : path	- [in]  로그 저장 경로
		save	- [in]  로그 저장 여부
		type	- [in]  Log Type (0 : text, 1 : DB (SQLite3)
		mode	- [in]  로그 저장 기준 (None, All, Warning and Error, Only Error)		
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvLogs_Init(PTCHAR path, BOOL saved, UINT8 type, ENG_LNWE mode)
{
	/* 로그 파일 객체 생성 및 초기화 */
	g_pLogData	= new CLogData (saved, type, mode);
	ASSERT(g_pLogData);
	if (!g_pLogData->Init(path))
	{
		g_pLogData->Close();
		return FALSE;
	}
	/* 로그 파일 포맷 저장 */
	g_u8LogType	= type;

	return TRUE;
}

/*
 desc : 라이브러리 해제
 parm : None
 retn : None
*/
API_EXPORT VOID uvLogs_Close()
{
	/* 동기 진입 */
	if (g_pLogData && g_syncLogs.Enter())
	{
		/* 로그 파일 닫기 */
		g_pLogData->Close();
		delete g_pLogData;
		g_pLogData	= NULL;

		/* 동기 해제 */
		g_syncLogs.Leave();
	}
}

/*
 desc : 로그 저장 - 디버깅
 parm : apps	- [in]  Application Code (ENG_EDIC)
		level	- [in]  메시지 성격 (normal, warning, error)
		mesg	- [in]  기타 발생된 값이 저장된 버퍼
		file	- [in]  메시지가 발생된 소스 코드 파일명 (전체 경로가 포함된 파일 ... )
		func	- [in]  메시지가 발생된 소스 코드 함수명
		line	- [in]  메시지가 발생된 소스 코드 위치 (Line Number)
 retn : None
*/
API_EXPORT VOID uvLogs_SaveLogs(ENG_EDIC apps, ENG_LNWE level, PTCHAR mesg, PTCHAR file, PTCHAR func, UINT32 line)
{
	INT32 i32MsgLen	= 0;

	/* 로그 메시지 유효성 검사 */
	if (!mesg)	return;
	i32MsgLen	= (INT32)_tcslen(mesg);
	if (i32MsgLen < 1)	return;

	/* 동기 진입 */
	if (g_syncLogs.Enter())
	{
		/* 로그 데이터 (메시지) 저장 */
		g_pLogData->SaveLogs(apps, level, mesg, file, func, line);
#if 0
		if (level >= g_pLogData->GetSaveLogLevel())
		{
			TCHAR tzMesg[1024]	= {NULL};	/* 검색된 메시지 저장 버퍼 */
			swprintf_s(tzMesg, 1024, L"%s %s(%d) %s", file, func, line, mesg);
			TRACE(L"%s\n", tzMesg);
		}
#endif
		/* 동기 진입 해제 */
		g_syncLogs.Leave();
	}
}
API_EXPORT VOID uvLogs_SaveLogsEx(ENG_LNWE level, PTCHAR mesg, PTCHAR file, PTCHAR func, UINT32 line)
{
	INT32 i32MsgLen	= 0;

	/* 로그 메시지 유효성 검사 */
	if (!mesg)	return;
	i32MsgLen	= (INT32)_tcslen(mesg);
	if (i32MsgLen < 1)	return;

	/* 동기 진입 */
	if (g_syncLogs.Enter())
	{
		/* 로그 데이터 (메시지) 저장 */
		g_pLogData->SaveLogs(ENG_EDIC::en_engine, level, mesg, file, func, line);
#ifdef _DEBUG
		if (level >= g_pLogData->GetSaveLogLevel())
		{
			TCHAR tzMesg[1024]	= {NULL};	/* 검색된 메시지 저장 버퍼 */
			swprintf_s(tzMesg, 1024, L"%s %s(%d) %s", file, func, line, mesg);
			TRACE(L"%s\n", tzMesg);
		}
#endif
		/* 동기 진입 해제 */
		g_syncLogs.Leave();
	}
}

/*
 desc : 가장 마지막에 발생된 로그 메시지 반환
 parm : None
 retn : Error message가 저장된 전역 버퍼 포인터
*/
API_EXPORT PTCHAR uvLogs_GetErrorMesgLast()
{
	PTCHAR ptzMesg	= NULL;
	/* 동기 진입 */
	if (g_syncLogs.Enter())
	{
		ptzMesg	= g_pLogData->GetErrorMesgLast();
		/* 동기 해제 */
		g_syncLogs.Leave();
	}
	/* 공유 메모리 영역에 저장된 문자열 시작 위치 반환 */
	return ptzMesg;
}

/*
 desc : 에러 혹은 경고 로그 존재 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvLogs_IsErrorMesg()
{
	BOOL bIsError	= FALSE;
	/* 동기 진입 */
	if (g_syncLogs.Enter())
	{
		bIsError	= g_pLogData->IsErrorMesg();
		/* 동기 해제 */
		g_syncLogs.Leave();
	}

	return bIsError;
}

/*
 desc : 마지막 에러 메시지 리셋
 parm : None
 retn : None
*/
API_EXPORT VOID uvLogs_ResetErrorMesg()
{
	/* 동기 진입 */
	if (g_syncLogs.Enter())
	{
		g_pLogData->ResetErrorMesg();
		/* 동기 해제 */
		g_syncLogs.Leave();
	}
}

/*
 desc : 임의의 위치에 있는 에러 로그 메시지 반환
 parm : index	- [in]  특정 위치에 있는 에러 로그 반환 (0 ~ 7)
						0x00: 가장 최근에 발생된 에러 로그 요청
						[참고] 값이 커질수록 오래된 에러 로그 요청 (0 ~ 7)
 retn : 로그 메시지
		NULL이 반환된 경우, 더 이상 에러 로그 없음
*/
API_EXPORT PTCHAR uvLogs_GetErrorMesg(UINT8 index)
{
	if (index >= LAST_MESG_COUNT)	return NULL;
	return g_pLogData->GetErrorMesg(index);
}

/*
 desc : 전체 경로가 포함된 로그 파일 목록 반환
 parm : files	- [out] 로그 파일 목록이 저장될 Array Buffer
 retn : None
*/
API_EXPORT VOID uvLogs_GetLogFiles(UINT8 type, CStringArray &files)
{
	CUniToChar csCnv;
	CString strPath		= g_pLogData->GetLogPath(type);

	if (g_u8LogType)	uvCmn_SearchFile(strPath, L"*.db", files);
	else				uvCmn_SearchFile(strPath, L"*.log", files);
}

#ifdef __cplusplus
}
#endif