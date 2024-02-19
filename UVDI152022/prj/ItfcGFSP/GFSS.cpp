
/*
 desc : GFSS (Server) 데이터 관리
*/

#include "pch.h"
#include "MainApp.h"
#include "GFSS.h"


#ifdef _DEBUG
#define new	DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : None
 retn : None
*/
CGFSS::CGFSS(LPG_CSID config)
	: CConfig()
{
	m_pstConfig	= config;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CGFSS::~CGFSS()
{
}
