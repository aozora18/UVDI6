
/*
 desc : GFSC (Client) ������ ����
*/

#include "pch.h"
#include "MainApp.h"
#include "GFSC.h"


#ifdef _DEBUG
#define new	DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : None
 retn : None
*/
CGFSC::CGFSC(LPG_CSID config)
	: CConfig()
{
	m_pstConfig	= config;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CGFSC::~CGFSC()
{
}

