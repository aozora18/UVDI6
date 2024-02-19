#pragma once

#include "DBData.h"

class CDBMgr
{
public:
	BOOL																	Create(CWnd *pWnd);
	int																		GetCount(EN_DB_TABLE eDBTable) { return m_clsDBData.GetCount(eDBTable); }

	void																	AddLoginInfo(ST_DB_LOGIN stInfo);
	void																	DelLoginInfo(ST_DB_LOGIN stInfo);
	std::vector < ST_DB_LOGIN >												GetLoginInfo();

	bool																	GetDBData(CppSQLite3Query* dbQuery, EN_DB_TABLE eTable, EN_DB_DATA eValue, CString& strValue);



private:
	HWND						m_hParentWnd;
	CDBData						m_clsDBData;


protected:
	CDBMgr();
	CDBMgr(const CDBMgr &); // Prohibit Copy Constructor
	CDBMgr& operator =(const CDBMgr&);

public:
	virtual ~CDBMgr(void);

	static CDBMgr* GetInstance()
	{
		static CDBMgr _inst;
		return &_inst;
	}
};

