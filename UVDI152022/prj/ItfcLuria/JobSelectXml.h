
#pragma once

#include "../../inc/txml/tinyxml2.h"

class AlignMotion;

/* Panel Data */
class CFiducialData
{
// 생성자 & 파괴자
public:

	CFiducialData()
	{
		m_u16MarkType	= 0x1111;	/* 기본 4 점 Global Mark Type */
	}
	virtual ~CFiducialData()
	{
		m_lstData.RemoveAll();
	}


/* 로컬 변수 */
protected:

	UINT16				m_u16MarkType;	/* Global Mark Type */
	CAtlList <STG_XMXY>	m_lstData;

/* 내부 함수 */
protected:


/* 공용 함수 */
public:

	/*
	 desc : gerber x, y 입력
	 parm : mark_xy	- [in]  Gerber X / YPosition (unit: mm)
	 retn : None
	*/
	VOID AddData(STG_XMXY mark_xy)
	{
		m_lstData.AddTail(mark_xy);
	}

	/*
	 desc : Fiducial Data 리스트 메모리 해제
	 parm : None
	 retn : None
	*/
	VOID RemoveAll()
	{
		m_lstData.RemoveAll();
	}

	/*
 desc : D Code에 해당되는 Panel Data 반환
 parm : mark	- [in]  Mark Number (Zero based)
		data	- [out] Gerber XY & Motion XY Position (unit: mm)
 retn : TRUE or FALSE
*/
	BOOL GetTgtMark(UINT16 mark, STG_XMXY& data)
	{
		for (int i = 0; i < m_lstData.GetCount(); i++)
		{
			auto pPos = m_lstData.FindIndex(i);
			if (!pPos)	return FALSE;
			
			data = m_lstData.GetAt(pPos);

			if (data.tgt_id == mark)
				return TRUE;
		}
		return FALSE;
		
	}


	/*
	 desc : D Code에 해당되는 Panel Data 반환
	 parm : mark	- [in]  Mark Number (Zero based)
			data	- [out] Gerber XY & Motion XY Position (unit: mm)
	 retn : TRUE or FALSE
	*/
	BOOL GetMark(UINT16 mark, STG_XMXY &data)
	{
		POSITION pPos	= NULL;

		/* 만약 현재 Global Mark가 3점 Align Mark 방식일 경우 */
		if (m_lstData.GetCount() == 3)
		{
			/* !!! Important !!! */
			switch (m_u16MarkType)
			{
			case 0x0111	:	if (mark > 0)	mark--;	break;
			case 0x1011	:	if (mark > 1)	mark--;	break;
			case 0x1101	:	if (mark > 2)	mark--;	break;
			case 0x1110	:	if (mark > 3)	mark--;	break;
			}
		}

		/* 검색 인덱스 값이 등록된 개수보다 큰지 검사 */
		if (mark > (UINT16)m_lstData.GetCount())	return FALSE;
		/* 검색 성공 했는지 확인 */
		pPos	= m_lstData.FindIndex(mark);
		if (!pPos)	return FALSE;
		/* 결과 값 반환 */
		data	= m_lstData.GetAt(pPos);

		return TRUE;
	}

	/*
	 desc : 등록된 개수 반환
	 parm : None
	 retn : 개수
	*/
	UINT16 GetCount()
	{
		return (UINT16)m_lstData.GetCount();
	}

	/*
	 desc : Global Mark Type 값 설정
	 parm : type	- [in]  Mark Type 값
	 retn : None
	*/
	VOID SetMarkType(UINT16 type)
	{
		m_u16MarkType	= type;
	}
};	/* CFiducialData */

class CDynamicPanelData;

class CJobSelectXml
{
// 생성자 & 파괴자
public:

	CJobSelectXml();
	virtual ~CJobSelectXml();
	 
// 가상 함수
protected:

public:


// 로컬 변수
protected:

	CHAR				m_szJobName[MAX_GERBER_NAME];

	UINT8				m_u8StripeCount;	/* 거버 전체에 해당 됨 */
	UINT16				m_u16MarkType;		/* 2, 3 or 4점 Global Mark에 따라 배치도가 다름 */

	/* Gerber Size (단위: mm) */
	DOUBLE				m_dbGerberWidth, m_dbGerberHeight;
	DOUBLE				m_dbScaleX, m_dbScaleY;

	ENG_ATGL			m_enAlignType;

	STG_XMXY			m_stMarkCent;	/* 4 점 혹은 가상의 4점의 중심 좌표 (거버 중심 좌표) */

	DOUBLE				*m_pPhDistX;	/* XML 파일에 저장되어 있는 Photohead 간의 X Offset 값 임시 저장 */
	CFiducialData		*m_pFidGlobal;
	CFiducialData		*m_pFidLocal;
	LPG_XDPD			m_pstPanelData;	/* 반드시 순서 유지 : Scale[0], Text[1], Serial[2]...Serial[n] */

// 로컬 함수
protected:

	BOOL				SortMarks(UINT16 col, UINT16 row, UINT16 s_cnt, BOOL shared,
								  CAtlList <STG_XMXY> &mark_xy);
	BOOL				IsValidAlignType(ENG_ATGL align_type);

// 공용 함수
public:
	AlignMotion* alignMotion = nullptr;
	BOOL				LoadRegistrationXML(CHAR *job_name, ENG_ATGL align_type);
	UINT8				GetMarkCount(ENG_AMTF mark=ENG_AMTF::en_none);
	UINT8				GetLocalMarkCountPerScan();
	BOOL				GetGlobalMark(UINT16 index, STG_XMXY &data);
	BOOL				GetGlobalMark(ENG_GMDD direct, STG_XMXY &data1, STG_XMXY &data2);
	BOOL				GetLocalMark(UINT16 index, STG_XMXY &data);
	CFiducialData*		GetGlobalMark(); //소팅되어있는듯함.
	CFiducialData*		GetLocalMark(); //소팅되어있는듯함.

	UINT8				GetLocalBottomMark(UINT8 scan, UINT8 cam_id);
	BOOL				GetLocalBottomMark(UINT8 scan, UINT8 cam_id, STG_XMXY &data);

	BOOL				IsCompareJobName(CHAR *job_name);
	BOOL				IsMarkMixedGlobalLocal();
	BOOL				IsMarkExistGlobal();

	UINT8				GetStripeCount()	{	return m_u8StripeCount;	}
 
	DOUBLE				GetGlobalLeftRightBottomDiffY();
	DOUBLE				GetGlobalBaseMarkLocalDiffY(UINT8 direct, UINT8 index);
	/* Only Global */
	DOUBLE				GetGlobalMarkDist(ENG_GMDD direct);
	DOUBLE				GetGlobalMarkDiffX(UINT8 mark_x1, UINT8 mark_x2);
	DOUBLE				GetGlobalMarkDiffY(UINT8 mark_y1, UINT8 mark_y2);
	DOUBLE				GetGlobalMarkDiffVertX(UINT8 type);

	/* Only Local */
	DOUBLE				GetLocalMarkACam12DistX(UINT8 mode, UINT8 scan);
	DOUBLE				GetLocalMarkDiffY(UINT8 mark_y1, UINT8 mark_y2);

	UINT32				GetLocalMarkDiffVertX(UINT8 scan,
											  CAtlList <DOUBLE> &cam1, CAtlList <DOUBLE> &cam2);
 
	VOID				GetGerberSize(DOUBLE &width, DOUBLE &height);

	BOOL				IsSharedAlignType(ENG_ATGL align_type);

	UINT16				GetMarkPosDirect(STG_XMXY mark);
	UINT16				GetMarkType()				{	return m_u16MarkType;		}
	UINT16				GetSerialDCodeFieldCount();

	DOUBLE				GetPhDistX(UINT8 ph_no)		{	return m_pPhDistX[ph_no-1];	}
	DOUBLE				GetGerberWidth()			{	return m_dbGerberWidth;		};
	DOUBLE				GetGerberHeight()			{	return m_dbGerberHeight;	};
	LPG_XDPD			GetPanelData()				{	return m_pstPanelData;		};
	VOID				ResetData();
};
