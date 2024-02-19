#pragma once

#include "DBDefine.h"
#include "Sqlite/CppSQLite3.h"

class CDBData
{
public:
	CDBData();
	~CDBData();

public:
	BOOL																	Initialize();
	BOOL																	OpenDB(EN_DB_TYPE eDBType, EN_DB_TABLE eDBTable);
	BOOL																	ExecQuery(EN_DB_TYPE eDBType, CString strQuery, BOOL& bEOF);
	BOOL																	ExecDML(ST_DB_DATA_INFO stData);
	void																	AddExecDML(ST_DB_DATA_INFO stData);
	int																		GetCount(EN_DB_TABLE eDBTable);
	BOOL																	GetExecQuery(EN_DB_TYPE eDBType, CString strQuery, CppSQLite3Query& dbQuery);

	void																	DeleteDB(EN_DB_TYPE eDBType);
	CppSQLite3DB*															GetDB(EN_DB_TYPE eDBType) { return &m_clsDB[eDBType]; }

	void																	StartExecThread();
	void																	DestoryThread();

	// Thread 관련
public:
	void																	SetExecDMLThreadRun(BOOL bRun) { m_bExecDMLThreadRun = bRun; }
	BOOL																	GetExecDMLThreadRun() { return m_bExecDMLThreadRun; }
public:
	std::vector<ST_DB_DATA_INFO>											m_vecExecDML;

	// Export 관련
public:
	// Export Data

	std::vector<ST_DB_LOGIN>												GetLoginInfo() { return m_vLoginInfo; }


private:
	std::vector<ST_DB_LOGIN>												m_vLoginInfo;



private:
	//	DDL
	CString																	GetCreateTableStr(EN_DB_TABLE eDBTable);
	CString																	GetDropTableStr(EN_DB_TABLE eDBTable);

	//	DML
	CString																	GetInsertRecordStr(EN_DB_TABLE eDBTable);
	CString																	GetDeleteRecordStr(EN_DB_TABLE eDBTable);
	CString																	GetSelectRecordStr(EN_DB_TABLE eDBTable);

	//	DCL

private:
	BOOL																	m_bExecDMLThreadRun;
	
public:
	CWinThread																*m_pResetDBThread;
	CWinThread																*m_pExecDMLThread;

private:
	CppSQLite3DB															m_clsDB[eDB_TYPE_MAX];
	CppSQLite3Statement														m_clsStm[eDB_RECORD_MAX];

public:
	CCriticalSection/*CMclCritSec*/											m_csDbAccess;
	CCriticalSection/*CMclCritSec*/											m_csDbVector;

};

