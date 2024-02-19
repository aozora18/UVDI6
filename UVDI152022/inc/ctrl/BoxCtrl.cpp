
/*
 desc : List Box Ȯ��
*/

#include "stdafx.h"
#include "BoxCtrl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CBoxCtrl, CListBox)

/*
 desc : ������
 parm : None
 retn : None
*/
CBoxCtrl::CBoxCtrl()
{
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CBoxCtrl::~CBoxCtrl()
{
	if (m_csFont.GetSafeHandle())	m_csFont.DeleteObject();
}

/* Message Map */
BEGIN_MESSAGE_MAP(CBoxCtrl, CListBox)
END_MESSAGE_MAP()

/*
 desc : ��Ʈ ����
 parm : lf	- [in]  LogFont ����ü ������
 retn : None
*/
VOID CBoxCtrl::SetBoxFont(LOGFONT *lf)
{
	// ���� ��Ʈ ��ü�� ����
	if (m_csFont.GetSafeHandle())		m_csFont.DeleteObject();
#if 1
	if (m_csFont.CreateFontIndirect(lf))	SetFont(&m_csFont);
#else
	if (m_csFont.CreateFont(lf->lfHeight,
							0,
						    lf->lfEscapement,
						    lf->lfOrientation,
						    lf->lfWeight,
						    lf->lfItalic,
						    lf->lfUnderline,
						    lf->lfStrikeOut,
						    lf->lfCharSet,
						    lf->lfOutPrecision,
						    lf->lfClipPrecision,
						    lf->lfQuality,
						    lf->lfPitchAndFamily,
						    lf->lfFaceName))	SetFont(&m_csFont);
#endif
}
