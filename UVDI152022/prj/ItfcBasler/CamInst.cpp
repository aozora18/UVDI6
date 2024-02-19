
/*
 desc : 카메라 인스턴트 객체 (CBaslerGigEInstantCamera)
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
 desc : 생성자
 parm : cam_id	- [in]  Align Camera Index (1 or Later)
 retn : None
 이력 : 2021-05-25 (Modified by JinSoo.Kang)
*/
CCamInst::CCamInst(UINT8 cam_id)
	: CBaslerGigEInstantCamera()
{
	m_u8CamIndex	= cam_id;
}

/*
 desc : 파괴자
 parm : None
 retn : None
 이력 : 2021-05-25 (Modified by JinSoo.Kang)
*/
CCamInst::~CCamInst()
{
}

