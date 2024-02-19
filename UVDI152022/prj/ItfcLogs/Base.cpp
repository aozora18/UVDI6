/*
 desc : 로그 기본 객체
*/

#include "pch.h"
#include "MainApp.h"
#include "Base.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

/*
 desc : 생성자
 parm : saved	- [in]  로그 파일 저장 여부
		type	- [in]  Log Type (0 : text, 1 : DB (SQLite3)
		mode	- [in]  로그 저장 기준 (None, All, Warning and Error, Only Error)
 retn : None
*/
CBase::CBase(BOOL saved, UINT8 type, ENG_LNWE mode)
{
	//m_pSqlDB			= NULL;

	m_bLogSaved			= saved;
	m_u8LogType			= type;
	m_enSavedLevel		= mode;

	m_u64HddCheckTime	= GetTickCount64();
	m_u64LogCheckTime	= GetTickCount64();

	wmemset(m_tzDrvName,0x00, 0x04);

	for (int i = 0; i < MAX_PATH_COUNT; i++)
	{
		wmemset(m_tzLogDir[i], 0x00, MAX_PATH_LEN);
		wmemset(m_tzLogFile[i], 0x00, MAX_PATH_LEN);
	}
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CBase::~CBase()
{
	Close();
}

/*
 desc : 초기화
 parm : path	- [in]  로그 데이터 저장 경로
 retn : TRUE or FALSE
*/
BOOL CBase::Init(PTCHAR path)
{
	/* 로그 드라이브 및 로그 전체 경로 설정 */
	wmemset(m_tzDrvName, 0x00, 4);
	wmemcpy(m_tzDrvName, path, 3);	/* c:\ or d:\ or e:\, or etc */
	/* 로그 파일 이름 새로 설정 */
	for (int i = 0; i < MAX_PATH_COUNT; i++)
	{
		wmemset(m_tzLogDir[i], 0x00, MAX_PATH_LEN);
		//wcscpy_s(m_tzLogDir, MAX_PATH_LEN, path);
		swprintf_s(m_tzLogDir[i], MAX_PATH_LEN, L"%s\\%s", path, m_tzDir[i]);

		SetFile(i);
	}

	return TRUE;
}

/*
 desc : 해제
 parm : None
 retn : None
*/
VOID CBase::Close()
{
	/* 임시 Database 해제 */
	//if (m_pSqlDB)	sqlite3_close(m_pSqlDB);
	/* 임시 추출된 Data 제거 */
	m_lstSqlDB.RemoveAll();
}

/*
 desc : 로그가 저장되는 HDD의 남아 있는 용량이 여유가 있는지 여부
 parm : limit	- [in]  주어진 값 보다 남아 있는 공간이 작으면 안됨 (단위: MBytes)
 retn : TRUE or FALSE (HDD가 FULL이 아님)
*/
BOOL CBase::IsHDDFull(UINT64 limit)
{
	UINT64 u64Total, u64Free;

	/* 현재 HDD 용량 확인 (특성 시간 (5 minutes)마다 확인) */
	if ((GetTickCount64() - m_u64HddCheckTime) > 60 * 1000 * 5)
	{
		/* HDD 용량 확인한 시간 저장 */
		m_u64HddCheckTime	= GetTickCount64();
		/* HDD 용량 정보 얻기 */
		if (uvCmn_GetHDDInfo(u64Total, u64Free, m_tzDrvName))
		{
			/* 남아 있는 공간이 512MBytes 이하인지 여부 */
			if (u64Free < (1024 * 1024 * limit))	return TRUE;
		}
	}

	return FALSE;
}

/*
 desc : 로그 파일 크기 확인
 parm : limit	- [in]  주어진 값 보다 로그 파일의 크기가 큰지 여부 (단위: MBytes)
 retn : TRUE (현재 파일이 주어진 값보다 클 때 / 주어진 값보다 작더라도 생성된지 5분이 지났을 때 )
*/
BOOL CBase::IsLogFull(UINT8 type, UINT64 limit)
{
	/* 현재 파일의 용량 확인 */
	if (uvCmn_GetFileSize(m_tzLogFile[type]) > (1024 * 1024 * limit))
	{
		/* LogFile 크기 확인한 시간 저장 */
		m_u64LogCheckTime = GetTickCount64();
		return TRUE;
	}

	/* 현재 파일 생성 시간 확인 (특성 시간 (5 minutes)마다 확인) */
	if ((GetTickCount64() - m_u64LogCheckTime) > 60 * 1000 * 5)
	{
		/* LogFile 크기 확인한 시간 저장 */
		m_u64LogCheckTime	= GetTickCount64();
		return TRUE;
	}

	return FALSE;
}

/*
 desc : 로그 파일 이름 설정
 parm : None
 retn : None
*/
VOID CBase::SetFile(UINT8 type)
{
	TCHAR tzExt[2][4]	= { L"log", L"db" };
	SYSTEMTIME stTm		= {NULL};
	CUniToChar csCnv;

	/* 현재 시간 얻기 */
	GetLocalTime(&stTm);
	swprintf_s(m_tzLogFile[type], MAX_PATH_LEN, L"%s\\%04d%02d%02d_%02d%02d%02d.%s",
			   m_tzLogDir[type], stTm.wYear, stTm.wMonth, stTm.wDay,
			   stTm.wHour, stTm.wMinute, stTm.wSecond, tzExt[m_u8LogType]);
}

/*
 desc : 로그 데이터 저장 및 관리 - Text File
 parm : mesg	- [in]  로그 데이터 저장된 버퍼
 retn : TRUE or FALSE
*/
BOOL CBase::SaveTxt(UINT8 type, PTCHAR mesg)
{
	UINT8 u8Mode	= 0x00;

	/* 로그 파일이 저장되는 로컬 하드 디스크 용량 확인 */
	if (IsHDDFull(1024 /* MBytes */))	return FALSE;
	/* 로그 파일이 일정 이상 저장되어 있다면, 새로운 파일 이름 얻기 */
	if (IsLogFull(type, 1))	SetFile(type);
	/* 해당 파일에 대해 기존에 존재하는지 여부 확인 */
	u8Mode	= uvCmn_FindFile(m_tzLogFile[type]) == FALSE ? 0x00 : 0x01;
	/* 로그 파일 저장 (0x00: 새로 쓰기, 0x01: 추가하기 (없으면 생성?)) */
	uvCmn_SaveTxtFileW(mesg, (UINT32)_tcslen(mesg), m_tzLogFile[type], u8Mode);

	return TRUE;
}

/*
 desc : 로그 데이터 저장 및 관리 - Database (SQLite3)
 parm : apps	- [in]  Application Code (ENG_EDIC)
		level	- [in]  메시지 성격 (normal, warning, error)
		mesg	- [in]  기타 발생된 값이 저장된 버퍼
		file	- [in]  메시지가 발생된 소스 코드 파일명 (Only file name. Included extension)
		func	- [in]  메시지가 발생된 소스 코드 함수명
		line	- [in]  메시지가 발생된 소스 코드 위치 (Line Number)
 retn : TRUE or FALSE
*/
static int cbkSqlQrySelect(void *parent, int argc, char **argv, char **col_name)
{
	UINT64 u64Tick	= GetTickCount64();
	CBase *pBase	= (CBase*)parent;

	for (int i=0; i<argc; i++)
	{
		TRACE("%s = %s\n", col_name[i], argv[i] ? argv[i] : "NULL");

		/* 너무 장시간 대기하면 안되므로, 최소 5초 이상 소요되면 무조건 빠져 나감 */
#ifdef _DEBUG
		if (u64Tick + 10*1000 < GetTickCount64())	break;
#else
		if (u64Tick + 5*1000 < GetTickCount64())	break;
#endif
		/* 검색된 결과 값을 임시 메모리 저장 */
	}

	return 0;
}
BOOL CBase::SaveDB(UINT8 type, UINT8 apps, UINT8 level, PTCHAR mesg, PTCHAR file, PTCHAR func, UINT32 line)
{
	UINT8 u8Mode	= 0x00;
	INT32 i32Result	= 0;
	CHAR *pzErrMsg	= NULL;
	TCHAR tzQry[512]= {NULL};
	CUniToChar csCnv;

	/* 로그 파일이 저장되는 로컬 하드 디스크 용량 확인 */
	if (IsHDDFull(1024 /* MBytes */))	return FALSE;
	/* 로그 파일이 일정 이상 저장되어 있다면, 새로운 파일 이름 얻기 */
	if (IsLogFull(type, 32))	SetFile(type);

	/* 해당 파일에 대해 기존에 존재하는지 여부 확인 */
	u8Mode	= uvCmn_FindFileA(csCnv.Uni2Ansi(m_tzLogFile[type])) == FALSE ? 0x00 : 0x01;

	/* Open DB */
	sqlite3 *pSqlDB	= NULL;
	i32Result	= sqlite3_open(csCnv.Uni2Ansi(m_tzLogFile[type]), &pSqlDB);
	if (i32Result)
	{
#ifdef _DEBUG
		TRACE(L"Can't open database: %s\n", sqlite3_errmsg(pSqlDB));
#endif
		return FALSE;
	}

	/* Creates a Table */
	if (!u8Mode)
	{
		swprintf_s(tzQry, 512,	L"CREATE TABLE logs ("
//								L"DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL PRIMARY KEY,"
								L"DATETIME DEFAULT (STRFTIME('%%Y-%%m-%%d %%H:%%M:%%f','NOW', 'localtime')) NOT NULL PRIMARY KEY,"
								L"APPS INT NOT NULL,"
								L"LEVEL INT NOT NULL,"
								L"MESG VARCHAR(%u) NOT NULL,"
								L"FILE VARCHAR(%u) NOT NULL,"
								L"FUNC VARCHAR(%u) NOT NULL,"
								L"LINE INT NOT NULL);", LOG_MESG_SIZE, MAX_FILE_LEN, FUNC_NAME_SIZE);
		i32Result = sqlite3_exec(pSqlDB, csCnv.Uni2Ansi(tzQry), 0, 0, &pzErrMsg);
		if (i32Result)
		{
#ifdef _DEBUG
			TRACE(L"Can't create a database: %s (err_msg=%s\n", sqlite3_errmsg(pSqlDB), pzErrMsg);
#endif
			sqlite3_free(pzErrMsg);	/* Release memory */
			sqlite3_close(pSqlDB);	/* Database closed */
			return FALSE;
		}
	}

	/* Begin Transaction */
	i32Result = sqlite3_exec(pSqlDB, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	if (i32Result == SQLITE_OK)
	{
		/* Inert a record */
		swprintf_s(tzQry, 512,	L"INSERT INTO logs (apps, level, mesg, file, func, line) "
								L"VALUES (%u, %u, \"%s\", \"%s\", \"%s\", %u);",
								apps, level, mesg, file, func, line);
		i32Result = sqlite3_exec(pSqlDB, csCnv.Uni2Ansi(tzQry), NULL, NULL, &pzErrMsg);
		if (i32Result != SQLITE_OK)
		{
#ifdef _DEBUG
			TRACE(L"Can't insert a record: %s (err_msg=%s\n", sqlite3_errmsg(pSqlDB), pzErrMsg);
#endif
			sqlite3_free(pzErrMsg);	/* Release memory */
		}

		/* End Transaction */
		sqlite3_exec(pSqlDB, "END TRANSACTION;", NULL, NULL, NULL);
	}

	/* Database closed */
	sqlite3_close(pSqlDB);

	return TRUE;
}

/*
 desc : 데이터 조회 후 임시 버퍼에 저장
 parm : file	- [in]  검색 대상의 Database (local file db) -> 전체 경로 포함된 파일명 (확장자 포함)
		start	- [in]  검색 시작 날짜 및 시간 (ex> yyyy-mm-ss hh-mm-ss)
		end		- [in]  검색 종료 날짜 및 시간 (ex> yyyy-mm-ss hh-mm-ss)
 retn : 총 검색된 개수 반환 (-1 : Database 접근 실패, 0 : 검색된 값이 없음, 1 or Later)
 note : start와 end 값이 NULL이면 전체 검색 진행
*/
INT32 CBase::GetLogData(PTCHAR file, PTCHAR start, PTCHAR end)
{
	INT32 i32Result	= 0;
	CHAR *pzErrMsg	= NULL;
	TCHAR tzQry[512]= {NULL};
	CUniToChar csCnv;

	/* 해당 파일에 대해 기존에 존재하는지 여부 확인 */
	if (!uvCmn_FindFile(file))	return -1;

	/* Open DB */
	sqlite3 *pSqlDB	= NULL;
	i32Result	= sqlite3_open(csCnv.Uni2Ansi(file), &pSqlDB);
	if (i32Result)
	{
#ifdef _DEBUG
		TRACE(L"Can't open database: %s\n", sqlite3_errmsg(pSqlDB));
#endif
		return -1;
	}

	/* Begin Transaction */
	i32Result = sqlite3_exec(pSqlDB, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	if (i32Result == SQLITE_OK)
	{
		/* 기존에 저장된 임시 검색 데이터 메모리 해제 */
		m_lstSqlDB.RemoveAll();

		/* Inert a record */
		if (start && end)
		{
			swprintf_s(tzQry, 512,	L"SELECT * FROM logs "
									L"WHERE (DATETIME >= \"%s\" and DATETIME <= \"%s\");",
									start, end);
		}
		else
		{
			swprintf_s(tzQry, 512,	L"SELECT * FROM logs");
		}
		i32Result = sqlite3_exec(pSqlDB, csCnv.Uni2Ansi(tzQry), cbkSqlQrySelect, this, &pzErrMsg);
		if (i32Result != SQLITE_OK)
		{
#ifdef _DEBUG
			TRACE(L"Can't select a record: %s (err_msg=%s\n", sqlite3_errmsg(pSqlDB), pzErrMsg);
#endif
			sqlite3_free(pzErrMsg);	/* Release memory */
		}

		/* End Transaction */
		sqlite3_exec(pSqlDB, "END TRANSACTION;", NULL, NULL, NULL);
	}

	/* Database closed */
	sqlite3_close(pSqlDB);

	return (i32Result == 0) ? INT32(m_lstSqlDB.GetCount()) : -1;
}

/*
 desc : 검색된 결과를 임시 메모리 저장
 parm : apps	- [in]  로그 발생 주체 구분 코드
		level	- [in]  로그 에러 레벨 코드
		line	- [in]  로그 발생 위치 (Line Number)
		func	- [in]  로그 발생 함수 명
		file	- [in]  로그 파일 이름 (Only Filename. Included extension)
		time	- [in]  검색된 레코드의 시간 (yyyy-mm-dd hh:mm:ss.ms)
		mesg	- [in]  로그 데이터
 retn : None
*/
VOID CBase::SetLogData(UINT8 apps, UINT8 level, UINT32 line,
					   PTCHAR func, PTCHAR file, PTCHAR time, PTCHAR mesg)
{
	STM_LDRI stRecord	= {NULL};

	stRecord.apps	= apps;
	stRecord.level	= level;
	stRecord.line	= line;
	wcscpy_s(stRecord.date_time, DATE_TIME_SIZE, time);
	wcscpy_s(stRecord.func_name, FUNC_NAME_SIZE, func);
	wcscpy_s(stRecord.file_name, MAX_FILE_LEN, file);
	wcscpy_s(stRecord.message, LOG_MESG_SIZE, mesg);
}
