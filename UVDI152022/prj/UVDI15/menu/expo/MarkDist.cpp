
/*
 desc : Global 4 �� Mark ���� 6 ������ ���� ���� ��ȿ���� ����
 */

#include "pch.h"
#include "../../MainApp.h"
#include "MarkDist.h"

#include "../../../../inc/comn/MyStatic.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : ������
 parm : h_dist	- [in]  Global 4 �� Mark ���� 6 ������ ���� ������ ����� �ڵ�
 retn : None
*/
CMarkDist::CMarkDist(HWND *h_dist)
{
	/* ���� �ڵ� ���� */
	memcpy(m_hDist, h_dist, sizeof(HWND) * 6);
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CMarkDist::~CMarkDist()
{
}

/*
 desc : ���� Ȥ�� ���� �ֱٿ� �˻��� ��ũ ��� ���� ��� (����)
 parm : None
 retn : None
*/
VOID CMarkDist::DrawDist()
{
	UINT8 u8Mode	= uvEng_Camera_GetGrabbedCount() > 0 ? 0x01 : 0x00;
	MarkDist(u8Mode);
}

/*
 desc : �˻��� ��ũ �̹��� ���
 parm : mode	- [in]  0x00 : �˻��� ��ũ ����, 0x01 : �˻��� ��ũ ����
 retn : None
*/
VOID CMarkDist::MarkDist(UINT8 mode)
{
	UINT8 u8Index, i	= 0x00;
	DOUBLE dbMarkDiff	= 0.0f;
	COLORREF clrText[3]	= { RGB(128, 128, 128), RGB(0, 0, 255), RGB(255, 0, 0) };
	CMyStatic *pTxt		= NULL;
#if 1	/* �����ǿ� ��ϵ� ��ũ 6���� ���� ���� �� �� �Ǵ��ϵ��� ���� */
	for (; i<6; i++)
	{
		u8Index	= 0x00;
		pTxt	= (CMyStatic *)CWnd::FromHandle(m_hDist[i]);
		ASSERT(pTxt);

		/* Gerber File (XML) ���� Global Mark ���� �� �Ÿ� (6 ��) ���ϱ� */
		if (mode && uvEng_Luria_GetMarkCount(ENG_AMTF::en_global) == 0x04)	/* �˻��� ��ũ�� �ִ� ��� (Only Global) */
		{
			/* ����� ī�޶� ���� ������ ��ũ �߽ɵ� ���� �Ÿ� ���� ���� �Ź��� �����ϴ� */
			/* ��ũ�� ���� �Ÿ� �� �� �� ȯ�� ���Ͽ� ����� ���� �� ���� ũ�� ������ �Ǵ� */
			u8Index	= uvEng_GetConfig()->mark_diff.result[i].len_valid;	/* 0x00 : Not set or Reset, 0x01 : Set (Succ), 0x02 : Set (Fail) */
		}
		/* ��Ʈ�� ��, ���� (�ؽ�Ʈ) ���� ���� */
		pTxt->SetTextColor(clrText[u8Index]);
		pTxt->Invalidate(FALSE);
	}
#endif
}