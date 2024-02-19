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

	LPG_ZAAL			m_pstLastResult;			/* 가장 최근에 선택된 정보 */

	CArray <LPG_ZAAL, LPG_ZAAL>	m_arrData[MAX_PH];	/* 각 Z 축 높이별 카메라의 분석 정도(?) 값을 Array로 저장 관리 */
	CImage				m_csImgMark;


/* 로컬 함수 */
protected:

	LPG_ZAAL			FocusData(UINT8 ph_no, UINT16 steps);
	INT32				FocusFind(UINT8 ph_no, UINT16 steps);
	VOID				FocusCalc(UINT8 ph_no, INT32 index);
	VOID				FocusSort(UINT8 ph_no);

/* 공용 함수 */
public:

	UINT32				GetCount(UINT8 ph_no)	{	return (UINT32)m_arrData[ph_no-1].GetCount();	}
	BOOL				SetResult(UINT8 cam_id, UINT8 ph_no, UINT16 steps);
	VOID				ResetResult();
	LPG_ZAAL			GetResult(UINT8 ph_no, UINT32 index);

	CImage				*GetEdgeImage()			{	return &m_csImgMark;	}
	LPG_ZAAL			GetLastResult()			{	return m_pstLastResult;	}
};
