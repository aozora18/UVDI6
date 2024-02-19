#pragma once

class CMeasure
{
/* 생성자 & 파괴자 */
public:
	CMeasure();
	virtual ~CMeasure();

/* 가상 함수 */
protected:
public:

/* 로컬 변수 */
protected:

	TCHAR				m_tzLastSaveFile[MAX_PATH_LEN];
	DOUBLE				m_dbMinMax[2][COLLECT_DATA_COUNT];

	LPG_ZAAL			m_pstDataSel;		/* 가장 최근에 선택된 정보 */

	CArray <LPG_ZAAL, LPG_ZAAL>	m_arrData;	/* 각 Z 축 높이별 카메라의 분석 정도(?) 값을 Array로 저장 관리 */
	CImage				m_csImgMark;


/* 로컬 함수 */
protected:

	LPG_ZAAL			FocusData(INT32 z_pos);
	INT32				FocusFind(INT32 z_pos);
	VOID				FocusCalc(INT32 index);
	VOID				FocusSort();

/* 공용 함수 */
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
