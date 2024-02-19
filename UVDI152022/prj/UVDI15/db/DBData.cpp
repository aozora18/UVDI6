#include "pch.h"
#include "DBData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif



UINT ExecDMLThread(LPVOID lpParam)
{
	CDBData *pData = (CDBData*)lpParam;

	while (pData->GetExecDMLThreadRun())
	{
		if (pData->m_vecExecDML.size() > 0)
		{
			CSingleLock/*CMclAutoLock*/ autolock(&pData->m_csDbAccess);
			//TRACE(_T("ExecDMLThread:진입\n"));
			bool bExec = false;
			for (int i = 0; i < pData->m_vecExecDML.size(); i++)
			{
				pData->ExecDML(pData->m_vecExecDML[i]);
				bExec = true;
				TRACE(_T("ExecDMLThread:ExecDML[%d]\n"), i);
			}

			if (bExec)
			{
				pData->m_vecExecDML.clear();
				pData->m_vecExecDML.shrink_to_fit();
				TRACE(_T("ExecDMLThread:Clear\n"));
			}
			//TRACE(_T("ExecDMLThread:종료\n"));
		}

		Sleep(500);
		//TRACE(_T("ExecDMLThread:대기\n"));
	}
	pData->m_pExecDMLThread = NULL;

	return 0;
}
CDBData::CDBData()
{
}
CDBData::~CDBData()
{
	DestoryThread();

	if (m_pResetDBThread)
	{
		m_pResetDBThread = NULL;
	}

	if (m_pExecDMLThread)
	{
		m_pExecDMLThread = NULL;
	}
}
BOOL CDBData::Initialize()
{
	BOOL bRet = TRUE;

	m_pExecDMLThread = NULL;
	m_bExecDMLThreadRun = FALSE;

	m_vecExecDML.clear();
	m_pExecDMLThread = NULL;

	CString strErrMsg;
	if (OpenDB(eDB_TYPE_LOGIN, eDB_TABLE_LOGIN_INFO))
	{
	}
	else
	{
		bRet = FALSE;
		//LANGUAGE 추가완료
		//strErrMsg.Format(_T("[DB] 오픈 실패. [%s] 파일을 확인하십시오."), sstrDBPath[eDB_TYPE_LOGIN]);
		//ShowMsg(eWARN, strErrMsg);
	}

	if (bRet)
	{
		StartExecThread();
	}

	return bRet;
}
BOOL CDBData::OpenDB(EN_DB_TYPE eDBType, EN_DB_TABLE eDBTable)
{
	CSingleLock/*CMclAutoLock*/ autolock(&m_csDbAccess);

	if (eDBType >= eDB_TYPE_MAX || eDBTable >= eDB_TABLE_MAX)
		return FALSE;

	BOOL bRet = FALSE;
	CString strQuery;

	try {
		USES_CONVERSION;
		CString strPath;

		//strPath.Format(_T("%s%s%s"), g_tzWorkDir, _T("\\%s\\db\\"), sstrDBPath[eDBType]);
		strPath.Format(_T("%s\\%s\\db\\%s"), g_tzWorkDir, CUSTOM_DATA_CONFIG, sstrDBPath[eDBType]);
		m_clsDB[eDBType].open(CT2A(strPath));

		if (!m_clsDB[eDBType].tableExists(CW2A(sstrDBTable[eDBTable])))
		{
			strQuery = GetCreateTableStr(eDBTable);
			m_clsDB[eDBType].execDML(CW2A(strQuery));
		}

		/* Statement별로 DML 명령 등록 */
		// Insert
		strQuery = GetInsertRecordStr(eDBTable);
		m_clsStm[eDB_RECORD_LOGIN_ADD] = m_clsDB[eDBType].compileStatement(CW2A(strQuery));
		// Delete
		strQuery = GetDeleteRecordStr(eDBTable);
		m_clsStm[eDB_RECORD_LOGIN_DEL] = m_clsDB[eDBType].compileStatement(CW2A(strQuery));
		// Select
		strQuery = GetSelectRecordStr(eDBTable);
		m_clsStm[eDB_RECORD_LOGIN_SEL] = m_clsDB[eDBType].compileStatement(CW2A(strQuery));

		bRet = TRUE;
	}
	catch (CppSQLite3Exception& e)
	{
		printf("FAIL : CDBData::OpenDB %d\n", e.errorCode());
		//AddLog(eLogTypeNormal, eLogClassDebug, _T("CppSQLite3Exception! CDBData::OpenDB()"));
		//AddLog(eLogTypeNormal, eLogClassDebug, _T("CppSQLite3Exception(%d) CDBData::OpenDB()"), e.errorCode());
		bRet = FALSE;
	}

	return bRet;
}

BOOL CDBData::ExecQuery(EN_DB_TYPE eDBType, CString strQuery, BOOL& bEOF)
{
	CSingleLock/*CMclAutoLock*/ autolock(&m_csDbAccess);
	BOOL bRet = TRUE;
	CppSQLite3Query dbQuery;
	//TRACE(_T("ExecQuery : %s\n"),strQuery);
	try
	{
		USES_CONVERSION;
		dbQuery = m_clsDB[eDBType].execQuery(CW2A(strQuery));
		TRACE(_T("ExecQuery : %s\n"), strQuery);
	}
	catch (CppSQLite3Exception& e)
	{
		printf("FAIL : ExecQuery %d\n", e.errorCode());
		//AddLog(eLogTypeNormal, eLogClassDebug, _T("CppSQLite3Exception(%d) CDBData::ExecQuery()"), e.errorCode(), strQuery);
		bRet = FALSE;
	}

	if (bRet && dbQuery.eof())
	{
		//printf("FAIL : ExecQuery eof\n");
		bEOF = TRUE;
	}

	return bRet;
}

BOOL CDBData::ExecDML(ST_DB_DATA_INFO stData)
{
	CSingleLock/*CMclAutoLock*/ autolock(&m_csDbAccess);

	BOOL bRet = TRUE;
	USES_CONVERSION;
	try
	{
		if (eDB_RECORD_LOGIN_ADD == stData.eRecord)
		{
			m_clsStm[stData.eRecord].bind(1, CW2A(stData.stLogin.strID, CP_UTF8));
			m_clsStm[stData.eRecord].bind(2, CW2A(stData.stLogin.strPwd, CP_UTF8));
			m_clsStm[stData.eRecord].bind(3, CW2A(stData.stLogin.strLevel, CP_UTF8));
			m_clsStm[stData.eRecord].execDML();
			m_clsStm[stData.eRecord].reset();
		}
		else if (eDB_RECORD_LOGIN_DEL == stData.eRecord)
		{
			m_clsStm[stData.eRecord].bind(1, CW2A(stData.stLogin.strID, CP_UTF8));
			m_clsStm[stData.eRecord].execDML();
			m_clsStm[stData.eRecord].reset();
		}
		else if (eDB_RECORD_LOGIN_SEL == stData.eRecord)
		{
			m_vLoginInfo.clear();

			CppSQLite3Query query = m_clsStm[stData.eRecord].execQuery();
			ST_DB_LOGIN stLogin;

			while (!query.eof())
			{
				stLogin.strID = query.getStringField("ID");
				stLogin.strPwd = query.getStringField("PASSWORD");
				stLogin.strLevel = query.getStringField("LEVEL");

				m_vLoginInfo.push_back(stLogin);

				query.nextRow();
			}
		}
	}
	catch (CppSQLite3Exception& e)
	{
		printf("FAIL : ExecDML(LoginInfo) %d\n", e.errorCode());
		//AddLog(eLogTypeNormal, eLogClassDebug, _T("CppSQLite3Exception(%d) CDBData::ExecDML(LoginInfo)"), e.errorCode());
		bRet = FALSE;
	}

	return bRet;
}

void CDBData::AddExecDML(ST_DB_DATA_INFO stData)
{
	CSingleLock/*CMclAutoLock*/ autolock(&m_csDbVector);

	//	Login의 경우, Alarm과 달리 Thread를 이용한 일괄 처리 방식을 사용하지 않는다.

	m_vecExecDML.push_back(stData);
}

int CDBData::GetCount(EN_DB_TABLE eDBTable)
{
	CSingleLock/*CMclAutoLock*/ autolock(&m_csDbAccess);

	int nCount = 0;
	CString strQuery;
	strQuery.Format(_T("select count(*) from %s;"), sstrDBTable[eDBTable]);

	if (eDB_TABLE_LOGIN_INFO == eDBTable )
		nCount = m_clsDB[eDB_TYPE_LOGIN].execScalar(CW2A(strQuery));
	
	return nCount;
}

BOOL CDBData::GetExecQuery(EN_DB_TYPE eDBType, CString strQuery, CppSQLite3Query& dbQuery)
{
	BOOL bRet = TRUE;
	CSingleLock/*CMclAutoLock*/ autolock(&m_csDbAccess);

	try
	{
		USES_CONVERSION;

		if (eDB_TYPE_LOGIN == eDBType)
		{
			dbQuery = m_clsDB[eDBType].execQuery(CW2A(strQuery, CP_UTF8));
		}
		else
		{
			dbQuery = m_clsDB[eDBType].execQuery(CW2A(strQuery));
		}
		//dbQuery = m_clsDB[eDBType].execQuery(CW2A(strQuery));
	}
	catch (CppSQLite3Exception& e)
	{
		e.errorCode();
		//AddLog(eLogTypeNormal, eLogClassDebug, _T("CppSQLite3Exception(%d) CDBData::GetExecQuery"), e.errorCode());
		bRet = FALSE;
	}

	return bRet;
}

void CDBData::DeleteDB(EN_DB_TYPE eDBType)
{
	// 기존 파일 백업 후 삭제
// 	CString strBackupPath;
// 	strBackupPath.Format(_T("%s%s"), DEF_ROOT_PATH, DEF_DB_BAKUP_PATH);
// 	if (FALSE == ::CheckFileExists(strBackupPath))
// 	{
// 		::CreateDir(strBackupPath);
// 	}^
// 
// 	CTime clsTime = CTime::GetCurrentTime();
// 	strBackupPath.AppendFormat(_T("%s_db_%04d%02d%02d%02d%02d%02d.db")
// 		, sstrDBName[eDBType]
// 		, clsTime.GetYear()
// 		, clsTime.GetMonth()
// 		, clsTime.GetDay()
// 		, clsTime.GetHour()
// 		, clsTime.GetMinute()
// 		, clsTime.GetSecond());

	CString strPath;
	//strPath.Format(_T("%s%s%s"), g_tzWorkDir, _T("\\%s\\db\\", CUSTOM_DATA_CONFIG), sstrDBPath[eDBType]);
	strPath.Format(_T("%s\\%s\\db\\%s"), g_tzWorkDir, CUSTOM_DATA_CONFIG, sstrDBPath[eDBType]);

//	CopyFile(strPath, strBackupPath, FALSE);

	DeleteFile(strPath);
}

CString CDBData::GetCreateTableStr(EN_DB_TABLE eDBTable)
{
	CString strDML;
	if (eDB_TABLE_LOGIN_INFO == eDBTable)
	{
		strDML.Format(_T("create table %s( ID char(256), PASSWORD char(512), LEVEL char(64) );"), sstrDBTable[eDBTable]);
	}

	return strDML;
}


CString CDBData::GetDropTableStr(EN_DB_TABLE eDBTable)
{
	CString strDML;
	if (eDB_TABLE_LOGIN_INFO == eDBTable)
	{
		strDML.Format(_T("drop table %s;"), sstrDBTable[eDBTable]);
	}

	return strDML;
}

CString CDBData::GetInsertRecordStr(EN_DB_TABLE eDBTable)
{
	CString strDML;
	if (eDB_TABLE_LOGIN_INFO == eDBTable)
	{
		// id, password, level
		strDML.Format(_T("insert into %s values(?, ?, ?);"), sstrDBTable[eDBTable]);
	}

	return strDML;
}

CString CDBData::GetDeleteRecordStr(EN_DB_TABLE eDBTable)
{
	CString strDML;
	if (eDB_TABLE_LOGIN_INFO == eDBTable)
	{
		// id
		strDML.Format(_T("delete from %s WHERE ID = ?;"), sstrDBTable[eDBTable]);
	}

	return strDML;
}

CString CDBData::GetSelectRecordStr(EN_DB_TABLE eDBTable)
{
	CString strDML;
	if (eDB_TABLE_LOGIN_INFO == eDBTable)
	{
		//	All
		strDML.Format(_T("select * from %s order by LEVEL, ID"), sstrDBTable[eDBTable]);
	}

	return strDML;
}

void CDBData::StartExecThread()
{
	m_bExecDMLThreadRun = TRUE;
	m_pExecDMLThread = AfxBeginThread(ExecDMLThread, this);
}

void CDBData::DestoryThread()
{
	MSG   message;

	if (TRUE == m_bExecDMLThreadRun || m_pExecDMLThread != NULL)
	{
		m_bExecDMLThreadRun = FALSE;
		while (m_pExecDMLThread != NULL)
		{
			if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&message);
				::DispatchMessage(&message);
			}
			Sleep(10);
		}
	}
	return;
}
