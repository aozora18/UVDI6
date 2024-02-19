#pragma once

class CExpoVal
{
/* ������ / �ı��� */
public:
	CExpoVal(HWND *h_wnd, HWND* h_wndPgr);
	virtual ~CExpoVal();


/* ���� ���� */
protected:

	UINT32				m_u16TempLed[2][4];		/* ���� �ֱ��� PH �µ� �� �ӽ� ���� (1 = 0.1��C) */
	UINT32				m_u16TempBoard[2][4];	/* ���� �ֱ��� PH �µ� �� �ӽ� ���� (1 = 0.1��C) */
	UINT32				m_u32JobCount;			/* ���� �ֱ��� �뱤�� Ƚ�� ���� */
	UINT32				m_u32TempDI[4];			/* ���� �ֱ��� DI �µ� �� �ӽ� ���� (1 = 0.001��C) */
	UINT64				m_u64JobTime;			/* ���� �ֱ��� �۾� �ð� ���� */

	//HWND				m_hProc;				/* Operation Process ���� �ڵ� */		=> MainDlg �̵�
	//HWND				m_hGerb;				/* Gerber Recipe ���� �ڵ� */			=> MainDlg �̵�
	HWND				m_hTact[2];				/* Tact Time ���� �ڵ� */
	HWND				m_hCount;				/* Exposure Count ���� �ڵ� */
	//HWND				m_hMark;				/* Mark Recipe ���� �ڵ� */				=> MainDlg �̵�
	HWND				m_hTempDI[4];			/* DI ���� �µ� ���� �ڵ� */
	HWND				m_hTempPH[2];			/* PH ���� �µ� ���� �ڵ� */
	HWND				m_hRate;				/* �۾� ��ô�� ���� �ڵ� */

	CString				m_strProcess;
/* ���� �Լ� */
protected:

	VOID				DrawValue();
	VOID				DrawText();


/* ���� �Լ� */
public:

	/* �θ� �����忡 ���� �ֱ������� ȣ�� */
	VOID				DrawRecipe();
	VOID				DrawExpo();
	VOID				ResetData();
};
