#include "pch.h"
#include "DBMgr.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CDBMgr::CDBMgr()
{
}


CDBMgr::~CDBMgr()
{
}

BOOL CDBMgr::Create(CWnd *pWnd)
{
	ASSERT(pWnd);
	m_hParentWnd = pWnd->GetSafeHwnd();

	if (TRUE == m_clsDBData.Initialize())
	{
// 		StartThread();
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

void CDBMgr::AddLoginInfo(ST_DB_LOGIN stInfo)
{
	ST_DB_DATA_INFO stDBInfo;
	stDBInfo.eTable = eDB_TABLE_LOGIN_INFO;
	stDBInfo.eRecord = eDB_RECORD_LOGIN_ADD;
	stDBInfo.copy(stInfo);
	m_clsDBData.ExecDML(stDBInfo);
}

void CDBMgr::DelLoginInfo(ST_DB_LOGIN stInfo)
{
	ST_DB_DATA_INFO stDBInfo;
	stDBInfo.eTable = eDB_TABLE_LOGIN_INFO;
	stDBInfo.eRecord = eDB_RECORD_LOGIN_DEL;
	stDBInfo.copy(stInfo);
	m_clsDBData.ExecDML(stDBInfo);
}


std::vector < ST_DB_LOGIN > CDBMgr::GetLoginInfo()
{
	ST_DB_DATA_INFO stDBInfo;
	stDBInfo.eTable = eDB_TABLE_LOGIN_INFO;
	stDBInfo.eRecord = eDB_RECORD_LOGIN_SEL;
	m_clsDBData.ExecDML(stDBInfo);

	return m_clsDBData.GetLoginInfo();
}

bool CDBMgr::GetDBData(CppSQLite3Query* dbQuery, EN_DB_TABLE eTable, EN_DB_DATA eData, CString& strValue)
{
	bool bRet = true;
	USES_CONVERSION;
	if (eDB_TABLE_LOGIN_INFO == eTable)
	{
		if (eDB_VAL_LOGIN_ID == eData)
		{
			strValue = CA2W(dbQuery->getStringField(0), CP_UTF8);
		}
		else if (eDB_VAL_LOGIN_PWD == eData)
		{
			strValue = CA2W(dbQuery->getStringField(1), CP_UTF8);
		}
		else if (eDB_VAL_LOGIN_LEVEL == eData)
		{
			strValue = CA2W(dbQuery->getStringField(2), CP_UTF8);
		}
		else
		{
			bRet = false;
		}
	}
	else
	{
		bRet = false;
	}

	return bRet;
}