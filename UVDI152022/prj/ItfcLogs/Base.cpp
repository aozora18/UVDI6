/*
 desc : �α� �⺻ ��ü
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
 desc : ������
 parm : saved	- [in]  �α� ���� ���� ����
		type	- [in]  Log Type (0 : text, 1 : DB (SQLite3)
		mode	- [in]  �α� ���� ���� (None, All, Warning and Error, Only Error)
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
 desc : �ı���
 parm : None
 retn : None
*/
CBase::~CBase()
{
	Close();
}

/*
 desc : �ʱ�ȭ
 parm : path	- [in]  �α� ������ ���� ���
 retn : TRUE or FALSE
*/
BOOL CBase::Init(PTCHAR path)
{
	/* �α� ����̺� �� �α� ��ü ��� ���� */
	wmemset(m_tzDrvName, 0x00, 4);
	wmemcpy(m_tzDrvName, path, 3);	/* c:\ or d:\ or e:\, or etc */
	/* �α� ���� �̸� ���� ���� */
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
 desc : ����
 parm : None
 retn : None
*/
VOID CBase::Close()
{
	/* �ӽ� Database ���� */
	//if (m_pSqlDB)	sqlite3_close(m_pSqlDB);
	/* �ӽ� ����� Data ���� */
	m_lstSqlDB.RemoveAll();
}

/*
 desc : �αװ� ����Ǵ� HDD�� ���� �ִ� �뷮�� ������ �ִ��� ����
 parm : limit	- [in]  �־��� �� ���� ���� �ִ� ������ ������ �ȵ� (����: MBytes)
 retn : TRUE or FALSE (HDD�� FULL�� �ƴ�)
*/
BOOL CBase::IsHDDFull(UINT64 limit)
{
	UINT64 u64Total, u64Free;

	/* ���� HDD �뷮 Ȯ�� (Ư�� �ð� (5 minutes)���� Ȯ��) */
	if ((GetTickCount64() - m_u64HddCheckTime) > 60 * 1000 * 5)
	{
		/* HDD �뷮 Ȯ���� �ð� ���� */
		m_u64HddCheckTime	= GetTickCount64();
		/* HDD �뷮 ���� ��� */
		if (uvCmn_GetHDDInfo(u64Total, u64Free, m_tzDrvName))
		{
			/* ���� �ִ� ������ 512MBytes �������� ���� */
			if (u64Free < (1024 * 1024 * limit))	return TRUE;
		}
	}

	return FALSE;
}

/*
 desc : �α� ���� ũ�� Ȯ��
 parm : limit	- [in]  �־��� �� ���� �α� ������ ũ�Ⱑ ū�� ���� (����: MBytes)
 retn : TRUE (���� ������ �־��� ������ Ŭ �� / �־��� ������ �۴��� �������� 5���� ������ �� )
*/
BOOL CBase::IsLogFull(UINT8 type, UINT64 limit)
{
	/* ���� ������ �뷮 Ȯ�� */
	if (uvCmn_GetFileSize(m_tzLogFile[type]) > (1024 * 1024 * limit))
	{
		/* LogFile ũ�� Ȯ���� �ð� ���� */
		m_u64LogCheckTime = GetTickCount64();
		return TRUE;
	}

	/* ���� ���� ���� �ð� Ȯ�� (Ư�� �ð� (5 minutes)���� Ȯ��) */
	if ((GetTickCount64() - m_u64LogCheckTime) > 60 * 1000 * 5)
	{
		/* LogFile ũ�� Ȯ���� �ð� ���� */
		m_u64LogCheckTime	= GetTickCount64();
		return TRUE;
	}

	return FALSE;
}

/*
 desc : �α� ���� �̸� ����
 parm : None
 retn : None
*/
VOID CBase::SetFile(UINT8 type)
{
	TCHAR tzExt[2][4]	= { L"log", L"db" };
	SYSTEMTIME stTm		= {NULL};
	CUniToChar csCnv;

	/* ���� �ð� ��� */
	GetLocalTime(&stTm);
	swprintf_s(m_tzLogFile[type], MAX_PATH_LEN, L"%s\\%04d%02d%02d_%02d%02d%02d.%s",
			   m_tzLogDir[type], stTm.wYear, stTm.wMonth, stTm.wDay,
			   stTm.wHour, stTm.wMinute, stTm.wSecond, tzExt[m_u8LogType]);
}

/*
 desc : �α� ������ ���� �� ���� - Text File
 parm : mesg	- [in]  �α� ������ ����� ����
 retn : TRUE or FALSE
*/
BOOL CBase::SaveTxt(UINT8 type, PTCHAR mesg)
{
	UINT8 u8Mode	= 0x00;

	/* �α� ������ ����Ǵ� ���� �ϵ� ��ũ �뷮 Ȯ�� */
	if (IsHDDFull(1024 /* MBytes */))	return FALSE;
	/* �α� ������ ���� �̻� ����Ǿ� �ִٸ�, ���ο� ���� �̸� ��� */
	if (IsLogFull(type, 1))	SetFile(type);
	/* �ش� ���Ͽ� ���� ������ �����ϴ��� ���� Ȯ�� */
	u8Mode	= uvCmn_FindFile(m_tzLogFile[type]) == FALSE ? 0x00 : 0x01;
	/* �α� ���� ���� (0x00: ���� ����, 0x01: �߰��ϱ� (������ ����?)) */
	uvCmn_SaveTxtFileW(mesg, (UINT32)_tcslen(mesg), m_tzLogFile[type], u8Mode);

	return TRUE;
}

/*
 desc : �α� ������ ���� �� ���� - Database (SQLite3)
 parm : apps	- [in]  Application Code (ENG_EDIC)
		level	- [in]  �޽��� ���� (normal, warning, error)
		mesg	- [in]  ��Ÿ �߻��� ���� ����� ����
		file	- [in]  �޽����� �߻��� �ҽ� �ڵ� ���ϸ� (Only file name. Included extension)
		func	- [in]  �޽����� �߻��� �ҽ� �ڵ� �Լ���
		line	- [in]  �޽����� �߻��� �ҽ� �ڵ� ��ġ (Line Number)
 retn : TRUE or FALSE
*/
static int cbkSqlQrySelect(void *parent, int argc, char **argv, char **col_name)
{
	UINT64 u64Tick	= GetTickCount64();
	CBase *pBase	= (CBase*)parent;

	for (int i=0; i<argc; i++)
	{
		TRACE("%s = %s\n", col_name[i], argv[i] ? argv[i] : "NULL");

		/* �ʹ� ��ð� ����ϸ� �ȵǹǷ�, �ּ� 5�� �̻� �ҿ�Ǹ� ������ ���� ���� */
#ifdef _DEBUG
		if (u64Tick + 10*1000 < GetTickCount64())	break;
#else
		if (u64Tick + 5*1000 < GetTickCount64())	break;
#endif
		/* �˻��� ��� ���� �ӽ� �޸� ���� */
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

	/* �α� ������ ����Ǵ� ���� �ϵ� ��ũ �뷮 Ȯ�� */
	if (IsHDDFull(1024 /* MBytes */))	return FALSE;
	/* �α� ������ ���� �̻� ����Ǿ� �ִٸ�, ���ο� ���� �̸� ��� */
	if (IsLogFull(type, 32))	SetFile(type);

	/* �ش� ���Ͽ� ���� ������ �����ϴ��� ���� Ȯ�� */
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
 desc : ������ ��ȸ �� �ӽ� ���ۿ� ����
 parm : file	- [in]  �˻� ����� Database (local file db) -> ��ü ��� ���Ե� ���ϸ� (Ȯ���� ����)
		start	- [in]  �˻� ���� ��¥ �� �ð� (ex> yyyy-mm-ss hh-mm-ss)
		end		- [in]  �˻� ���� ��¥ �� �ð� (ex> yyyy-mm-ss hh-mm-ss)
 retn : �� �˻��� ���� ��ȯ (-1 : Database ���� ����, 0 : �˻��� ���� ����, 1 or Later)
 note : start�� end ���� NULL�̸� ��ü �˻� ����
*/
INT32 CBase::GetLogData(PTCHAR file, PTCHAR start, PTCHAR end)
{
	INT32 i32Result	= 0;
	CHAR *pzErrMsg	= NULL;
	TCHAR tzQry[512]= {NULL};
	CUniToChar csCnv;

	/* �ش� ���Ͽ� ���� ������ �����ϴ��� ���� Ȯ�� */
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
		/* ������ ����� �ӽ� �˻� ������ �޸� ���� */
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
 desc : �˻��� ����� �ӽ� �޸� ����
 parm : apps	- [in]  �α� �߻� ��ü ���� �ڵ�
		level	- [in]  �α� ���� ���� �ڵ�
		line	- [in]  �α� �߻� ��ġ (Line Number)
		func	- [in]  �α� �߻� �Լ� ��
		file	- [in]  �α� ���� �̸� (Only Filename. Included extension)
		time	- [in]  �˻��� ���ڵ��� �ð� (yyyy-mm-dd hh:mm:ss.ms)
		mesg	- [in]  �α� ������
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
