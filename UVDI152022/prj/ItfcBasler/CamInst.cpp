
/*
 desc : ī�޶� �ν���Ʈ ��ü (CBaslerGigEInstantCamera)
*/

#include "pch.h"
#include "MainApp.h"
#include "CamInst.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : cam_id	- [in]  Align Camera Index (1 or Later)
 retn : None
 �̷� : 2021-05-25 (Modified by JinSoo.Kang)
*/
CCamInst::CCamInst(UINT8 cam_id)
	: CBaslerGigEInstantCamera()
{
	m_u8CamIndex	= cam_id;
}

/*
 desc : �ı���
 parm : None
 retn : None
 �̷� : 2021-05-25 (Modified by JinSoo.Kang)
*/
CCamInst::~CCamInst()
{
}

