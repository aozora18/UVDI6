#pragma once
#pragma pack(push, 1)
//////////////////////////////////////////////////////////////////////////
// define
//////////////////////////////////////////////////////////////////////////






//////////////////////////////////////////////////////////////////////////
// enum
//////////////////////////////////////////////////////////////////////////
enum EN_DB_TYPE
{
	eDB_TYPE_LOGIN,
	eDB_TYPE_MAX,
};

static CString sstrDBName[] =
{
	_T("Login"),
	_T("UNKNOWN"),
};

static CString sstrDBPath[] =
{
	_T("Login.db"),
	_T("UNKNOWN"),
};

enum EN_DB_TABLE
{
	eDB_TABLE_LOGIN_INFO,
	eDB_TABLE_MAX,
};

enum EN_DB_RECORD
{
	eDB_RECORD_LOGIN_ADD,
	eDB_RECORD_LOGIN_DEL,
	eDB_RECORD_LOGIN_SEL,
	eDB_RECORD_MAX,
};

static CString sstrDBTable[] =
{
	_T("LoginInfo"),
	_T("UNKNOWN"),
};

enum EN_DB_DATA
{
	eDB_VAL_LOGIN_ID = 0,
	eDB_VAL_LOGIN_PWD,
	eDB_VAL_LOGIN_LEVEL,
};

//////////////////////////////////////////////////////////////////////////
// struct
//////////////////////////////////////////////////////////////////////////
typedef struct tagLoginInfo
{
	CString		strID;
	CString		strPwd;
	CString		strLevel;

	tagLoginInfo()
	{
		strID = _T("");
		strPwd = _T("");
		strLevel = _T("");
	}

}ST_DB_LOGIN;

typedef struct tagDBDataInfo
{
	EN_DB_TABLE				eTable;
	EN_DB_RECORD			eRecord;
	ST_DB_LOGIN				stLogin;

	void copy(ST_DB_LOGIN stSrc)
	{
		stLogin.strID	 = stSrc.strID;
		stLogin.strPwd   = stSrc.strPwd;
		stLogin.strLevel = stSrc.strLevel;
	}


	tagDBDataInfo()
	{
		
	}

}ST_DB_DATA_INFO;

#pragma pack(pop)