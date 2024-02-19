#pragma once

class CMeasure
{
/* ������ & �ı��� */
public:
	CMeasure();
	virtual ~CMeasure();

/* ���� �Լ� */
protected:
public:

/* ���� ���� */
protected:

	LPG_ZAAL			m_pstLastResult;			/* ���� �ֱٿ� ���õ� ���� */

	CArray <LPG_ZAAL, LPG_ZAAL>	m_arrData[MAX_PH];	/* �� Z �� ���̺� ī�޶��� �м� ����(?) ���� Array�� ���� ���� */
	CImage				m_csImgMark;


/* ���� �Լ� */
protected:

	LPG_ZAAL			FocusData(UINT8 ph_no, UINT16 steps);
	INT32				FocusFind(UINT8 ph_no, UINT16 steps);
	VOID				FocusCalc(UINT8 ph_no, INT32 index);
	VOID				FocusSort(UINT8 ph_no);

/* ���� �Լ� */
public:

	UINT32				GetCount(UINT8 ph_no)	{	return (UINT32)m_arrData[ph_no-1].GetCount();	}
	BOOL				SetResult(UINT8 cam_id, UINT8 ph_no, UINT16 steps);
	VOID				ResetResult();
	LPG_ZAAL			GetResult(UINT8 ph_no, UINT32 index);

	CImage				*GetEdgeImage()			{	return &m_csImgMark;	}
	LPG_ZAAL			GetLastResult()			{	return m_pstLastResult;	}
};
