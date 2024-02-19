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

	TCHAR				m_tzLastSaveFile[MAX_PATH_LEN];
	DOUBLE				m_dbMinMax[2][COLLECT_DATA_COUNT];

	LPG_ZAAL			m_pstDataSel;		/* ���� �ֱٿ� ���õ� ���� */

	CArray <LPG_ZAAL, LPG_ZAAL>	m_arrData;	/* �� Z �� ���̺� ī�޶��� �м� ����(?) ���� Array�� ���� ���� */
	CImage				m_csImgMark;


/* ���� �Լ� */
protected:

	LPG_ZAAL			FocusData(INT32 z_pos);
	INT32				FocusFind(INT32 z_pos);
	VOID				FocusCalc(INT32 index);
	VOID				FocusSort();

/* ���� �Լ� */
public:

	DOUBLE				GetMinMax(UINT8 mode, UINT8 type);

	UINT32				GetCount()	{	return (UINT32)m_arrData.GetCount();	}
	BOOL				SetResult(UINT8 cam_id);
	VOID				ResetResult();
	LPG_ZAAL			GetResult(UINT32 index);
	VOID				SelectEdgeFile(PTCHAR file);
	VOID				SelectLastEdgeFile();

	CImage				*GetEdgeImage()	{	return &m_csImgMark;	}
	LPG_ZAAL			GetDataSel()	{	return m_pstDataSel;	}
};
