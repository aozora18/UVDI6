
/*
 desc : 현재 선택된 Job의 XML 데이터 관리
*/

#include "pch.h"
#include "MainApp.h"
#include "JobSelectXml.h"
#include "../UVDI15/GlobalVariables.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 desc : 생성자
 parm : None
 retn : None
*/
CJobSelectXml::CJobSelectXml()
{
	m_pFidGlobal = NULL;
	m_pFidLocal = NULL;
	m_pstPanelData = NULL;
	m_pPhDistX = NULL;
	m_u8StripeCount = 0x00;
	m_enAlignType = ENG_ATGL::en_global_0_local_0x0_n_point;

	memset(m_szJobName, 0x00, MAX_GERBER_NAME);
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CJobSelectXml::~CJobSelectXml()
{
	ResetData();
}

/*
 desc : Reset Data
 parm : None
 retn : None
*/
VOID CJobSelectXml::ResetData()
{
	if (m_pFidGlobal)	delete m_pFidGlobal;
	if (m_pFidLocal)	delete m_pFidLocal;
	if (m_pstPanelData)	::Free(m_pstPanelData);
	if (m_pPhDistX)		::Free(m_pPhDistX);

	m_pstPanelData = NULL;
	m_pFidGlobal = NULL;
	m_pFidLocal = NULL;
	m_pPhDistX = NULL;
}

/*
 desc : 정렬 콜백 함수 (오름 차순)
		X 좌표 기준으로 정렬하되, X 좌표가 동일하면, Y 좌표 오름 차순으로 정렬
 parm : first	- [in]  첫번째 비교를 위한 구조체 참조
		second	- [in]  두번째 비교를 위한 구조체 참조
 retn : TRUE or FALSE
*/
BOOL CbfSortX(STG_XMXY& first, STG_XMXY& second)
{
	/* 2개의 X 좌표가 동일하면, Y 좌표 내림차 (Y 좌표가 큰 값이 위에 배치) 순으로 정렬 */
	if (first.mark_x == second.mark_x)	return first.mark_y > second.mark_y;
	/* X 좌표 오름차 (X 좌표들 중에서 작은 값이 위에 배치) 순으로 정렬 */
	return first.mark_x < second.mark_x;
}

/*
 desc : 정렬 콜백 함수 (오름 차순)
		Y 좌표 기준으로 정렬하되, Y 좌표가 동일하면, X 좌표 내림 차순으로 정렬
 parm : first	- [in]  첫번째 비교를 위한 구조체 참조
		second	- [in]  두번째 비교를 위한 구조체 참조
 retn : TRUE or FALSE
*/
BOOL CbfSortY(STG_XMXY& first, STG_XMXY& second)
{
	/* 2개의 Y 좌표가 동일하면, X 좌표 내림차 (X 좌표가 큰 값이 위에 배치) 순으로 정렬 */
	/* 참고로, X 좌표 오름 차순으로 한 경우, Y 좌표는 Scan 마다 동일한 값이 존재할 수 없는 구조 */
	if (first.mark_y == second.mark_y)	return first.mark_x > second.mark_x;
	/* Y 좌표 내림차 (Y 좌표들 중에서 큰 값이 위에 배치) 순으로 정렬 */
	return first.mark_y > second.mark_y;
}

/*
 desc : 입력된 좌표가 Mark 1, 2, 3, 4 번 중 어디에 위치하는지 판단
 parm : cent	- [in]  중심 좌표
		mark	- [in]  Mark 위치
 retn : Mark 위치
		0x1000 - 1번 마크 위치
		0x0100 - 2번 마크 위치
		0x0010 - 3번 마크 위치
		0x0001 - 4번 마크 위치
*/
UINT16 CJobSelectXml::GetMarkPosDirect(STG_XMXY mark)
{
	UINT16 u16Pos = 0x0000;

	if (m_stMarkCent.mark_x < mark.mark_x)
	{
		if (m_stMarkCent.mark_y < mark.mark_y)		u16Pos = 0x0010;	/* Right_Top */
		else if (m_stMarkCent.mark_y > mark.mark_y)	u16Pos = 0x0001;	/* Right_Bottom */
	}
	else
	{
		if (m_stMarkCent.mark_y < mark.mark_y)		u16Pos = 0x1000;	/* Left_Top */
		else if (m_stMarkCent.mark_y > mark.mark_y)	u16Pos = 0x0100;	/* Left_Bottom */
	}
	return u16Pos;
}

/*
 desc : 좌표 정렬 작업 수행
		3행 2열의 각 Strip (조각 즉, 가로; stripe : 줄무니. 즉, 세로) 당 Local Fiducial 개수가 4개
 parm : col		- [in]  Strip의 행의 개수 (보통 2개)
		row		- [in]  Strip의 열의 개수 (보통 2 ~ 6개)
		s_cnt	- [in]  Stripe 내의 1 Block 마다 Fiducial 개수 (보통 4개이고, 2개인 경우 Shared 방식임)
		shared	- [in]  Align Mark Type이 Shared Type인지 여부
		x, y	- [out] 각 X / Y 좌표가 저장된 리스트 참조. 반환될 때, 정렬된 채로 저장됨
 retn : TRUE or FALSE
*/
BOOL CJobSelectXml::SortMarks(UINT16 col, UINT16 row, UINT16 s_cnt, BOOL shared,
	CAtlList <STG_XMXY>& mark_xy)
{
	INT32 i, i32Next = 0, i32Loop = 0;
	INT32 i32Count = 0;
	DOUBLE dbMinX = +999999.0f;
	DOUBLE dbMaxX = -999999.0f;
	DOUBLE dbMinY = +999999.0f;
	DOUBLE dbMaxY = -999999.0f;
	LPG_XMXY pstXY = NULL, pstNext;
	STG_XMXY stXY = { NULL };

	map<double, double> mx, my;

	for (int i = 0; i < mark_xy.GetCount(); i++)
	{
		auto fid = mark_xy.GetAt(mark_xy.FindIndex(i));
		mx[std::floor(fid.mark_x * 100.0f) / 100.0f] = fid.mark_x;
		my[std::floor(fid.mark_y * 100.0f) / 100.0f] = fid.mark_y;
	}
	row = my.size();
	col = mx.size();


	if (shared)
	{
		i32Count = row * col;	/* Shared 방식의 Stripe의 Block 마다 2개씩 Fiducial이 있지만, 전체 Mark 개수 계산할 때는 0 으로 취급 */
		i32Next = row;			/* Shared 방식의 Row 개수를 Block 개수로 보지 않고, Line 개념으로 보면 되므로, 각 Line 마다 Fiducial이 존재함 */
		i32Loop = col;
	}
	else
	{
		/* Global Mark이지만, 마크 개수가 3개인 경우 */
		if (mark_xy.GetCount() == 3)	i32Count = 3;
		else
		{
			i32Count = row * col;// *s_cnt;
			i32Next = row;		/* 1 Strip 마다 상/하 2개씩 Fiducial이 있고, row 개수만큼 있으므로 */
			i32Loop = col;
		}
	}
	/* Align Mark Type 값 초기화 */
	m_u16MarkType = 0x0000;
	/* X, Y 좌표 정보를 저장할 구조체 메모리 할당 */
	pstXY = new STG_XMXY[i32Count];
	/* 전체 등록된 X/Y 좌표를 구조체 배열에 저장 */
	for (i = 0; i < i32Count; i++)
	{
		pstXY[i] = mark_xy.GetAt(mark_xy.FindIndex(i));

		if (pstXY[i].mark_x < dbMinX)	dbMinX = pstXY[i].mark_x;
		if (pstXY[i].mark_x > dbMaxX)	dbMaxX = pstXY[i].mark_x;
		if (pstXY[i].mark_y < dbMinY)	dbMinY = pstXY[i].mark_y;
		if (pstXY[i].mark_y > dbMaxY)	dbMaxY = pstXY[i].mark_y;
	}
	/* 가상의 사각형 중심 좌표 구하기 */
	m_stMarkCent.mark_x = dbMinX + (dbMaxX - dbMinX) / 2.0f;
	m_stMarkCent.mark_y = dbMinY + (dbMaxY - dbMinY) / 2.0f;

	if (i32Count == 2)
	{
		if (pstXY[0].mark_x >= pstXY[1].mark_x)
		{
			memcpy(&stXY, &pstXY[0], sizeof(STG_DBXY));
			memcpy(&pstXY[0], &pstXY[1], sizeof(STG_DBXY));
			memcpy(&pstXY[1], &stXY, sizeof(STG_DBXY));
		}
		m_u16MarkType = 0x1010;	/* 1번과 2번 마크가 좌/우 배치되어 있으므로 */
	}
	else if (i32Count == 3)
	{
		pstNext = pstXY;
		/* X 좌표 <오름> 차순으로 전체 Mark 개수만큼 정렬 진행 */
		std::sort(pstNext, pstNext + i32Count, CbfSortX);
		/* --------------------------- */
		/* Align Mark 배치 타입 구하기 */
		/* --------------------------- */
		for (i = 0; i < 3; i++)
		{
			m_u16MarkType |= GetMarkPosDirect(pstXY[i]);
		}
		if (0x1100 == (0x1100 & m_u16MarkType))	/* Mark 2개가 왼쪽 영역에 있는 경우 */
		{
			/* Mark 1번과 2번의 Y 좌표 값 비교 후 정렬 여부 결정 */
			if (pstXY[0].mark_y < pstXY[1].mark_y)
			{
				memcpy(&stXY, &pstXY[0], sizeof(STG_XMXY));
				memcpy(&pstXY[0], &pstXY[1], sizeof(STG_XMXY));
				memcpy(&pstXY[1], &stXY, sizeof(STG_XMXY));
			}
		}
		else								/* Mark 2개가 오른쪽 영역에 있는 경우 */
		{
			/* Mark 2번과 3번의 Y 좌표 값 비교 후 정렬 여부 결정 */
			if (pstXY[1].mark_y < pstXY[2].mark_y)
			{
				memcpy(&stXY, &pstXY[1], sizeof(STG_XMXY));
				memcpy(&pstXY[1], &pstXY[2], sizeof(STG_XMXY));
				memcpy(&pstXY[2], &stXY, sizeof(STG_XMXY));
			}
		}

		/* 결과 리스트에서 각 x개씩 Y 좌표 <내림> 차순으로 정렬 진행 */
		for (i = 0; i < i32Loop; i++)
		{
			std::sort(pstNext, pstNext + i32Next, CbfSortY);
			pstNext += i32Next;
		}
	}
	else
	{
		m_u16MarkType = 0x1111;	/* Global or Local Mark가 4점씩  Mark가 존재하므로 */
		pstNext = pstXY;
		/* X 좌표 <오름> 차순으로 전체 Mark 개수만큼 정렬 진행 */
		std::sort(pstNext, pstNext + i32Count, CbfSortX);
		/* 결과 리스트에서 각 x개씩 Y 좌표 <내림> 차순으로 정렬 진행 */
		for (i = 0; i < i32Loop; i++)
		{
			std::sort(pstNext, pstNext + i32Next, CbfSortY);
			pstNext += i32Next;
		}
	}

	/* 다시 원래 리스트에 등록하여 반환 진행 */
	mark_xy.RemoveAll();
	for (i = 0; i < i32Count; i++)
	{
		mark_xy.AddTail(pstXY[i]);
	}

	/* 임시로 생성한 X/Y 버퍼 메모리 해제 */
	delete[] pstXY;

	return TRUE;
}

/*
 desc : Align Type 유효성 검증
 parm : align_type	- [in]  Gerber Data에 저장된 Mark Type (ENG_ATGL)
 retn : TRUE or FALSE
*/
BOOL CJobSelectXml::IsValidAlignType(ENG_ATGL align_type)
{
	switch (align_type)
	{
	case ENG_ATGL::en_global_4_local_0x0_n_point:
	case ENG_ATGL::en_global_3_local_0x0_n_point:
	case ENG_ATGL::en_global_2_local_0x0_n_point:

	case ENG_ATGL::en_global_4_local_2x1_n_point:

	case ENG_ATGL::en_global_4_local_2x2_n_point:
	case ENG_ATGL::en_global_4_local_3x2_n_point:
	case ENG_ATGL::en_global_4_local_4x2_n_point:
	case ENG_ATGL::en_global_4_local_5x2_n_point:

	case ENG_ATGL::en_global_4_local_2x2_s_point:
	case ENG_ATGL::en_global_4_local_3x2_s_point:
	case ENG_ATGL::en_global_4_local_4x2_s_point:
	case ENG_ATGL::en_global_4_local_5x2_s_point:
	case ENG_ATGL::en_not_defined:	// by sysandj : Preview를 위해 추가
		return TRUE;
	}
	return FALSE;
}

/*
 desc : Align Type이 Shared Type인지 아닌지 여부
 parm : align_type	- [in]  Gerber Data에 저장된 Mark Type (ENG_ATGL)
 retn : TRUE or FALSE
*/
BOOL CJobSelectXml::IsSharedAlignType(ENG_ATGL align_type)
{
	switch (align_type)
	{
	case ENG_ATGL::en_global_4_local_0x0_n_point:
	case ENG_ATGL::en_global_3_local_0x0_n_point:
	case ENG_ATGL::en_global_2_local_0x0_n_point:

	case ENG_ATGL::en_global_4_local_2x1_n_point:

	case ENG_ATGL::en_global_4_local_2x2_n_point:
	case ENG_ATGL::en_global_4_local_3x2_n_point:
	case ENG_ATGL::en_global_4_local_4x2_n_point:
	case ENG_ATGL::en_global_4_local_5x2_n_point:
	case ENG_ATGL::en_not_defined:	// by sysandj : Preview를 위해 추가
		return FALSE;
	}
	return TRUE;
}

/*
 desc : xml file 적재 - Global & Local Fiducial
 parm : job_name- [in]  절대 경로가 포함된 선택된 job name의 경로
		type	- [in]  Mark 구성 정보 (ENG_ATGL) (Global or Local Mark 의미가 아님)
 retn : TRUE or FALS
*/
BOOL CJobSelectXml::LoadRegistrationXML(CHAR* job_name, ENG_ATGL align_type)
{
	CHAR szJobPath[MAX_PATH_LEN] = { NULL };
	UINT16 i, u16Row = 0, u16Col = 0, u16Cnt = 0/* Stripe 내의 1 Block 마다 Fiducial 개수 */;
	BOOL bSharedType = FALSE;
	/* Gerber X, Y 좌표 저장 임시 리스트 */
	STG_XMXY stOrgXY = { NULL }, stBmpXY = { NULL }, stGbrXY = { NULL };
	CAtlList <STG_XMXY> lstGbrXY;

	tinyxml2::XMLDocument xmlDoc;
	tinyxml2::XMLError xmlErr;
	tinyxml2::XMLNode* xmlNode;

	tinyxml2::XMLElement* xmlElem1, * xmlElem2, * xmlElem3, * xmlData;
	tinyxml2::XMLAttribute* xmlAttr;

	/* Align Type 유효성 검사 */
	if (align_type != ENG_ATGL::en_global_0_local_0x0_n_point && !IsValidAlignType(align_type))	return FALSE;
	/* Create the panel object */
	ResetData();

	/* Local to Global & Local Fiducial */
	m_pFidGlobal = new CFiducialData;
	m_pFidLocal = new CFiducialData;
	ASSERT(m_pFidGlobal && m_pFidLocal);
	m_pstPanelData = (LPG_XDPD)::Alloc(sizeof(STG_XDPD) * MAX_PANEL_STRING_COUNT);
	ASSERT(m_pstPanelData);
	memset(m_pstPanelData, 0x00, sizeof(STG_XDPD) * (MAX_PANEL_STRING_COUNT));
	/* Photohead 간의 X 떨어진 거리 값 임시 저장 */
	m_pPhDistX = (DOUBLE*)::Alloc(sizeof(DOUBLE) * MAX_PH);
	ASSERT(m_pPhDistX);
	memset(m_pPhDistX, 0x00, sizeof(DOUBLE) * MAX_PH);
	/* job name 절대 경로 설정 */
	sprintf_s(szJobPath, MAX_PATH_LEN, "%s\\rlt_settings.xml", job_name);
	// XML 열기
	xmlErr = xmlDoc.LoadFile(szJobPath);
	if (0 != xmlErr)	return FALSE;
	/* 첫 번째 노드 얻기 */
	xmlNode = (tinyxml2::XMLNode*)xmlDoc.FirstChild();
	if (!xmlNode)	return FALSE;
	/* search for <rlt> */
	xmlNode = (tinyxml2::XMLNode*)xmlNode->NextSibling();
	if (0 != strcmp("rlt", xmlNode->Value()))
	{
		ResetData();
		return FALSE;
	}
	/* search for <BmpImage> */
	xmlElem1 = (tinyxml2::XMLElement*)xmlNode->FirstChildElement("BmpImage");
	if (!xmlElem1 || 0 != strcmp(xmlElem1->Value(), "BmpImage"))
	{
		ResetData();
		return FALSE;
	}
	/* row_size */
	xmlAttr = (tinyxml2::XMLAttribute*)xmlElem1->FirstAttribute();
#if 0	/* 1.5.0 에 있으나, 2.0.0 버전부터는 없어짐 */
	/* row_count */
	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
#endif
	/* shiftX */
	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
	/* shiftY */
	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
	/* gerber_width */
	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
	m_dbGerberWidth = xmlAttr->DoubleValue();
	/* gerber_height */
	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
	m_dbGerberHeight = xmlAttr->DoubleValue();
	/* scale x */
	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
	m_dbScaleX = xmlAttr->DoubleValue();
	/* scale y */
	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
	m_dbScaleY = xmlAttr->DoubleValue();

	/* search for <Stripes> */
	xmlElem1 = (tinyxml2::XMLElement*)xmlNode->FirstChildElement("Stripes");
	if (!xmlElem1 || 0 != strcmp(xmlElem1->Value(), "Stripes"))
	{
		ResetData();
		AfxMessageBox(L"Can't find the <stripes> field to xml_file", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}
	/* stripe_count */
	xmlAttr = (tinyxml2::XMLAttribute*)xmlElem1->FirstAttribute();
	m_u8StripeCount = (UINT8)xmlAttr->IntValue();
	/* search for <Fiducials> */
	xmlElem1 = (tinyxml2::XMLElement*)xmlNode->FirstChildElement("Fiducials");
	if (!xmlElem1 || 0 != strcmp(xmlElem1->Value(), "Fiducials"))
	{
		ResetData();
		AfxMessageBox(L"Can't find the <fiducials> field to xml_file", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}
	/* search for <Global> */
	xmlElem2 = (tinyxml2::XMLElement*)xmlElem1->FirstChildElement("Global");
	if (!xmlElem2)
	{
		ResetData();
		AfxMessageBox(L"Can't find the <global> field to xml_file", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}
	/* search for <field> */
	xmlElem3 = (tinyxml2::XMLElement*)xmlElem2->FirstChildElement();
	for (stGbrXY.org_id = 0; xmlElem3; xmlElem3 = (tinyxml2::XMLElement*)xmlElem3->NextSiblingElement())
	{
		/* find the <org> */
		xmlData = (tinyxml2::XMLElement*)xmlElem3->FirstChildElement();
		xmlAttr = (tinyxml2::XMLAttribute*)xmlData->FirstAttribute();
		stOrgXY.mark_x = (DOUBLE)xmlAttr->DoubleValue();
		stOrgXY.mark_y = (DOUBLE)xmlAttr->Next()->DoubleValue();

		/* find the <bmp> */
		xmlData = (tinyxml2::XMLElement*)xmlData->NextSiblingElement();
		xmlAttr = (tinyxml2::XMLAttribute*)xmlData->FirstAttribute();
		stBmpXY.mark_x = (DOUBLE)xmlAttr->DoubleValue();
		stBmpXY.mark_y = (DOUBLE)xmlAttr->Next()->DoubleValue();

		/* find the <gbr> */
		xmlData = (tinyxml2::XMLElement*)xmlData->NextSiblingElement();
		xmlAttr = (tinyxml2::XMLAttribute*)xmlData->FirstAttribute();
		stGbrXY.mark_x = (DOUBLE)xmlAttr->DoubleValue();
		stGbrXY.mark_y = (DOUBLE)xmlAttr->Next()->DoubleValue();

		/* Add */
		lstGbrXY.AddTail(stGbrXY);	/* OrgID 값이 Zero 부터 저장되기 위함 */
		stGbrXY.org_id++;
	}
	/* Global Mark가 최소 2개 이상이어야 됨 */
	if (lstGbrXY.GetCount() < 2 || lstGbrXY.GetCount() > 4)
	{
		lstGbrXY.RemoveAll();
		return TRUE;	/* 굳이 FALSE 할 필요 없음. */
	}
	if (lstGbrXY.GetCount() && lstGbrXY.GetCount())
	{
		/* Global Fiducial 정렬 작업 진행 */
		if (!SortMarks(1, 1, UINT16(lstGbrXY.GetCount()), FALSE, lstGbrXY))	return FALSE;
		/* 최종 Global Fiducial에 등록된 개수만큼 저장 */
		for (i = 0; i < lstGbrXY.GetCount(); i++)
		{
			stGbrXY = lstGbrXY.GetAt(lstGbrXY.FindIndex(i));
			stGbrXY.tgt_id = i;	/* !!! 정렬 후 위치 값 !!! (Zero-based) */
			m_pFidGlobal->AddData(stGbrXY);
		}
		/* Global Mark TYpe 값 설정 */
		m_pFidGlobal->SetMarkType(m_u16MarkType);
		/* 기존 등록된 임시 메모리 해제 */
		lstGbrXY.RemoveAll();
	}

	/* search for <Local Fiducials> */
	if (align_type != ENG_ATGL::en_global_0_local_0x0_n_point &&
		align_type != ENG_ATGL::en_global_4_local_0x0_n_point &&
		align_type != ENG_ATGL::en_global_3_local_0x0_n_point &&
		align_type != ENG_ATGL::en_global_2_local_0x0_n_point)
	{
		xmlElem1 = (tinyxml2::XMLElement*)xmlNode->FirstChildElement("Fiducials");
		if (!xmlElem1 || 0 != strcmp(xmlElem1->Value(), "Fiducials"))	return FALSE;
		/* search for <Local> */
		xmlElem2 = (tinyxml2::XMLElement*)xmlElem1->FirstChildElement("Local");
		if (xmlElem2)
		{
			/* search for <field> */
			xmlElem3 = (tinyxml2::XMLElement*)xmlElem2->FirstChildElement();
			for (stGbrXY.org_id = 0; xmlElem3; xmlElem3 = (tinyxml2::XMLElement*)xmlElem3->NextSiblingElement())
			{
				/* find the <org> */
				xmlData = (tinyxml2::XMLElement*)xmlElem3->FirstChildElement();
				xmlAttr = (tinyxml2::XMLAttribute*)xmlData->FirstAttribute();
				stOrgXY.mark_x = (DOUBLE)xmlAttr->DoubleValue();
				stOrgXY.mark_y = (DOUBLE)xmlAttr->Next()->DoubleValue();

				/* find the <bmp> */
				xmlData = (tinyxml2::XMLElement*)xmlData->NextSiblingElement();
				xmlAttr = (tinyxml2::XMLAttribute*)xmlData->FirstAttribute();
				stBmpXY.mark_x = (DOUBLE)xmlAttr->DoubleValue();
				stBmpXY.mark_y = (DOUBLE)xmlAttr->Next()->DoubleValue();

				/* find the <gbr> */
				xmlData = (tinyxml2::XMLElement*)xmlData->NextSiblingElement();
				xmlAttr = (tinyxml2::XMLAttribute*)xmlData->FirstAttribute();
				stGbrXY.mark_x = (DOUBLE)xmlAttr->DoubleValue();
				stGbrXY.mark_y = (DOUBLE)xmlAttr->Next()->DoubleValue();
				/* Add */
				lstGbrXY.AddTail(stGbrXY);
				stGbrXY.org_id++;	/* OrgID가 Zero 부터 저장되기 위함 */
			}

			/* Align Mark Type이 Shared Type인지 여부 */

 

			// by sysandj : 주석
			//switch (align_type)
		 

			if (lstGbrXY.GetCount())
			{
				/* Local Fiducial 정렬 작업 진행 */
				// by sysandj : Preview를 위해 define 되지 않았을경우 sorting안함.
				//abh1000 Local
				//if (FALSE == bNotDefined)
				//{
				if (!SortMarks(u16Col, u16Row, u16Cnt, bSharedType, lstGbrXY))	return FALSE;
				//}
				// by sysandj : Preview를 위해 define 되지 않았을경우 sorting안함.

				/* 최종 Local Fiducial에 등록된 개수만큼 저장 */
				for (i = 0; i < lstGbrXY.GetCount(); i++)
				{
					stGbrXY = lstGbrXY.GetAt(lstGbrXY.FindIndex(i));
					stGbrXY.tgt_id = i;	/* !!! 정렬 후 위치 값 !!! (Zero-based) */
					m_pFidLocal->AddData(stGbrXY);
				}
				/* 기존 등록된 임시 메모리 해제 */
				lstGbrXY.RemoveAll();
			}
		}
	}

	/* Search For <PanelData> */
	xmlElem1 = (tinyxml2::XMLElement*)xmlNode->FirstChildElement("PanelData");
	if (xmlElem1 && 0 == strcmp(xmlElem1->Value(), "PanelData"))
	{
		/* Search for <Dynamic PanelData> */
		xmlElem2 = (tinyxml2::XMLElement*)xmlElem1->FirstChildElement();
		for (i = 0; xmlElem2;
			(xmlElem2 = (tinyxml2::XMLElement*)xmlElem2->NextSiblingElement()) && (i < MAX_PANEL_STRING_COUNT),
			i++)
		{
			if (0 == strcmp(xmlElem2->Value(), "Dynamic"))
			{
				m_pstPanelData[i].field_cnt = 0x00;
				/* Attribute .. d-code, type, content, rotation, sizeX, sizeY, inverse */
				xmlAttr = (tinyxml2::XMLAttribute*)xmlElem2->FirstAttribute();
				m_pstPanelData[i].d_code = (UINT8)xmlAttr->IntValue();	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
				m_pstPanelData[i].type = (UINT8)xmlAttr->IntValue();	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
				m_pstPanelData[i].content = (UINT8)xmlAttr->IntValue();	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
				m_pstPanelData[i].rotation = (UINT16)xmlAttr->IntValue();	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
				m_pstPanelData[i].size_x = (UINT32)xmlAttr->IntValue();	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
				m_pstPanelData[i].size_y = (UINT32)xmlAttr->IntValue();	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
				m_pstPanelData[i].inverse = (UINT8)xmlAttr->IntValue();	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();

				/* search for <field> */
				xmlElem3 = (tinyxml2::XMLElement*)xmlElem2->FirstChildElement();
				/* field 정보가 있는지 여부 즉, 좌표 정보가 있는지 여부 확인 */
				if (xmlElem3 && 0 == strcmp(xmlElem3->Value(), "field"))
				{
					while (xmlElem3)
					{
						m_pstPanelData[i].field_cnt++;	/* field 개수가 총 몇개 있는지 카운팅 */
						xmlElem3 = (tinyxml2::XMLElement*)xmlElem3->NextSiblingElement();
					}
				}
			}
		}
	}

	/* Gerber Name 저장 (Included Path) */
	sprintf_s(m_szJobName, MAX_GERBER_NAME, "%s", job_name);
	/* 현재 선택된 Mark Type 값 저장 */
	if (align_type != ENG_ATGL::en_global_0_local_0x0_n_point)	m_enAlignType = align_type;

	/* search for <Photohead> */
	xmlElem1 = (tinyxml2::XMLElement*)xmlNode->FirstChildElement("NominalPhotoheadOffset");
	if (!xmlElem1)
	{
		AfxMessageBox(L"Can't find the <NominalPhotoheadOffset> field to xml_file", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}
	/* Search for <Photohead> */
	xmlElem2 = (tinyxml2::XMLElement*)xmlElem1->FirstChildElement("Photohead");
	if (!xmlElem2)
	{
		ResetData();
		AfxMessageBox(L"Can't find the <Photohead> field to xml_file", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}
	for (i = 0; xmlElem2; xmlElem2 = (tinyxml2::XMLElement*)xmlElem2->NextSiblingElement(), i++)
	{
		/* find the <nominal_photohead_offset> */
		xmlData = (tinyxml2::XMLElement*)xmlElem2->FirstChildElement();
		/* find the <nominal_photohead_distance> */
		xmlData = (tinyxml2::XMLElement*)xmlData->NextSiblingElement();
		xmlAttr = (tinyxml2::XMLAttribute*)xmlData->FirstAttribute();
		/* 기본 Offset 값 저장 */
		m_pPhDistX[i] = xmlAttr->IntValue();
	}

	return TRUE;
}

/*
 desc : Global Fiducial 좌표 값 얻기
 parm : index	- [in]  저장되어 있는 Mark Number (Zero based)
		data	- [out] Gerber XY & Motion XY Position (unit: mm)
 retn : TRUE or FALSE
*/
BOOL CJobSelectXml::GetGlobalMark(UINT16 index, STG_XMXY& data)
{
	/* 결과 값 반환 */
	if (!m_pFidGlobal)	return FALSE;
	return m_pFidGlobal->GetMark(index, data);
}

CFiducialData* CJobSelectXml::GetGlobalMark()
{
	/* 결과 값 반환 */
	if (!m_pFidGlobal)	return nullptr;
	return m_pFidGlobal;
}

CFiducialData* CJobSelectXml::GetLocalMark()
{
	/* 결과 값 반환 */
	if (!m_pFidLocal)	return nullptr;
	return m_pFidLocal;
}


/*
 desc : Global Fiducial 좌표 두 개의 위치 값 얻기
 parm : direct	- [in]  두 개의 좌표를 얻고자하는 방향 정보 구분 값
		data1	- [out] Gerber XY & Motion XY Position (unit: mm)
		data2	- [out] Gerber XY & Motion XY Position (unit: mm)
 retn : TRUE or FALSE
*/
BOOL CJobSelectXml::GetGlobalMark(ENG_GMDD direct, STG_XMXY& data1, STG_XMXY& data2)
{
	/* 결과 값 반환 */
	if (!m_pFidGlobal)	return FALSE;
	/* 마크 개수가 부족한지 여부 확인 */
	if (m_pFidGlobal->GetCount() < 4)	return FALSE;

	/* 현재 거버의 좌표 값의 단위가 nm(나노)이므로, 100으로 나누어야 100 nm or 0.1 um가 된다.*/
	switch (direct)
	{
	case ENG_GMDD::en_top_horz: if (!m_pFidGlobal->GetMark(0, data1))	return 0;
		if (!m_pFidGlobal->GetMark(2, data2))	return 0;	break;
	case ENG_GMDD::en_btm_horz: if (!m_pFidGlobal->GetMark(1, data1))	return 0;
		if (!m_pFidGlobal->GetMark(3, data2))	return 0;	break;
	case ENG_GMDD::en_lft_vert: if (!m_pFidGlobal->GetMark(0, data1))	return 0;
		if (!m_pFidGlobal->GetMark(1, data2))	return 0;	break;
	case ENG_GMDD::en_rgt_vert: if (!m_pFidGlobal->GetMark(2, data1))	return 0;
		if (!m_pFidGlobal->GetMark(3, data2))	return 0;	break;
	case ENG_GMDD::en_lft_diag: if (!m_pFidGlobal->GetMark(0, data1))	return 0;
		if (!m_pFidGlobal->GetMark(3, data2))	return 0;	break;
	case ENG_GMDD::en_rgt_diag: if (!m_pFidGlobal->GetMark(1, data1))	return 0;
		if (!m_pFidGlobal->GetMark(2, data2))	return 0;	break;
	}

	return TRUE;
}

/*
 desc : Local Fiducial 좌표 값 얻기
 parm : index	- [in]  저장되어 있는 위치 (Zero based)
		data	- [out] Gerber XY & Motion XY Position (unit: mm)
 retn : TRUE or FALSE
*/
BOOL CJobSelectXml::GetLocalMark(UINT16 index, STG_XMXY& data)
{
	/* 결과 값 반환 */
	if (!m_pFidLocal)	return FALSE;
	return m_pFidLocal->GetMark(index, data);
}

/*
 desc : Local Fiducal 위치 정보가 저장된 메모리 Index (Zero-based) 값 반환
 parm : scan	- [in]  스캔하려는 위치 (번호. 0 or 1)
		cam_id	- [in]  Align Camera Index (1 or 2)
 retn : Zero-based Index 값 반환
*/
UINT8 CJobSelectXml::GetLocalBottomMark(UINT8 scan, UINT8 cam_id)
{
	UINT8 u8Index = 0x00;

 


	if (!m_pFidLocal || std::clamp((int)cam_id, 1, 2) != cam_id)	return 0xff; //이제 스캔제한이 없어야한다. 

	auto status = alignMotion->status;

	int centerCol = status.GetCenterColumn();

	u8Index = cam_id == 1 ? status.markMapConst[scan].back().tgt_id : status.markMapConst[centerCol + scan].back().tgt_id;


	return u8Index;
}

/*
 desc : Local Fiducial 좌표 값 얻기 - Left / Bottom
 parm : scan	- [in]  스캔하려는 위치 (번호. 0 or 1)
		cam_id	- [in]  Align Camera Index (1 or 2)
		data	- [out] Fiducial 값이 저장될 참조 변수
 retn : TRUE or FALSE
*/
BOOL CJobSelectXml::GetLocalBottomMark(UINT8 scan, UINT8 cam_id, STG_XMXY& data)
{
	UINT8 u8Index = 0x00;

	
	if (!m_pFidLocal || std::clamp((int)cam_id, 1, 2) != cam_id)	return 0xff; //이제 스캔제한이 없어야한다. 


	/* Left or Right / Bottom Index 값 얻기 */
	u8Index = GetLocalBottomMark(scan, cam_id);
	if (0xff == u8Index)	return FALSE;

	/* 두 개의 X 지점 간의 떨어진 간격 구하기 (절대 값) */
	if (!m_pFidLocal->GetMark(u8Index, data))	return FALSE;

	return TRUE;
}

/*
 desc : 거버에 등록된 Global Fiducial 개수 반환
 parm : mark	- [in]  Mark Type (ENG_AMTF)
 retn : 개수
*/
UINT8 CJobSelectXml::GetMarkCount(ENG_AMTF mark)
{
	if (mark == ENG_AMTF::en_global)
	{
		if (!m_pFidGlobal)	return 0;
		return (UINT8)m_pFidGlobal->GetCount();
	}
	else if (mark == ENG_AMTF::en_local)
	{
		if (!m_pFidLocal)	return 0;
		return (UINT8)m_pFidLocal->GetCount();
	}
	else
	{
		if (!m_pFidGlobal || !m_pFidLocal)	return 0;
		return (UINT8)(m_pFidGlobal->GetCount() + m_pFidLocal->GetCount());
	}
}

/*
 desc : 1 Scan 기준으로 저장된 Local Mark 개수 반환
 parm : None
 retn : 개수
*/

//타격위치.
UINT8 CJobSelectXml::GetLocalMarkCountPerScan()
{
	UINT8 u8Count = alignMotion->status.gerberRowCnt;// GlobalVariables::getInstance()->GetAlignMotion().status.gerberRowCnt;
	return u8Count;

 
}

/*
 desc : Job Name 비교
 parm : job_name	- [in]  비교하고자 하는 Job Name이 저장된 버퍼 포인터
 retn : TRUE (동일), FALSE (다르다)
*/
BOOL CJobSelectXml::IsCompareJobName(CHAR* job_name)
{
	return 0 == strcmp(job_name, m_szJobName) ? TRUE : FALSE;
}

/*
 desc : 거버 데이터 내에 Local과 Global Mark 모두 등록되어 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
BOOL CJobSelectXml::IsMarkMixedGlobalLocal()
{
	if (!m_pFidGlobal || !m_pFidLocal)	return FALSE;
	return m_pFidGlobal->GetCount() > 0 && m_pFidLocal->GetCount() > 0;
}

/*
 desc : 거버 데이터 내에 Global Mark 등록되어 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
BOOL CJobSelectXml::IsMarkExistGlobal()
{
	if (!m_pFidGlobal)	return FALSE;
	return m_pFidGlobal->GetCount() > 0;
}

/*
 desc : 전역 기준점 : X 축 기준으로 2 개의 마크가 떨어진 위치 반환 (단위 : mm)
 parm : direct	- [in]  축의 마크 간격 (0 - 1번 마크와 3번 마크 간의 넓이 값)
									   (1 - 2번 마크와 4번 마크 간의 넓이 값)
									   (2 - 1번 마크와 2번 마크 간의 넓이 값)
									   (3 - 3번 마크와 4번 마크 간의 넓이 값)
									   (4 - 1번 마크와 4번 마크 간의 넓이 값)
									   (5 - 2번 마크와 3번 마크 간의 넓이 값)
 retn : X 축의 2개의 마크 떨어진 간격 (단위: mm) 반환
*/
DOUBLE CJobSelectXml::GetGlobalMarkDist(ENG_GMDD direct)
{
	STG_XMXY stData[2] = { NULL };

	if (!GetGlobalMark(direct, stData[0], stData[1]))	return 0.0f;
	/* 절대 값 반환 */
	return sqrt(pow(stData[0].mark_x - stData[1].mark_x, 2) + pow(stData[0].mark_y - stData[1].mark_y, 2));
}

/*
 desc : 두 Mark 간의 좌/우 X 축 떨어진 간격 즉, X 축 간의 오차 (거리) 값
 parm : mark_x1	- [in]  X1 축 Mark Number (1-based. 0x01 ~ 0x04)
		mark_x2	- [in]  X2 축 Mark Number (1-based. 0x01 ~ 0x04)
 retn : 두 지점의 Y 축 간의 높이 차이 (단위: mm)
*/
DOUBLE CJobSelectXml::GetGlobalMarkDiffX(UINT8 mark_x1, UINT8 mark_x2)
{
	if (!m_pFidGlobal)	return 0;

	UINT8 u8Mark = (UINT8)m_pFidGlobal->GetCount();
	STG_XMXY stData[2] = { NULL };

	if ((u8Mark != 4) && (u8Mark != 2) && (u8Mark != 3))	return 0;
	if (mark_x1 < 1 || mark_x2 < 1 || mark_x1 > 4 || mark_x2 > 4 || mark_x1 == mark_x2)	return 0;

	/* 해당 위치의 각각 X/Y 좌표 값 얻기 */
	if (!m_pFidGlobal->GetMark(mark_x1 - 1, stData[0]))	return 0;
	if (!m_pFidGlobal->GetMark(mark_x2 - 1, stData[1]))	return 0;

	/* 두 Y 좌표의 떨어진 간격 값 반환 */
	return (stData[0].mark_x - stData[1].mark_x);
}

/*
 desc : 두 Mark 간의 상/하 Y 축 떨어진 간격 즉, Y 축 간의 오차 (높이) 값
 parm : mark_y1	- [in]  Y1 축 Mark Number (1-based. 0x01 ~ 0x04)
		mark_y2	- [in]  Y2 축 Mark Number (1-based. 0x01 ~ 0x04)
 retn : 두 지점의 Y 축 간의 높이 차이 (단위: mm)
*/
DOUBLE CJobSelectXml::GetGlobalMarkDiffY(UINT8 mark_y1, UINT8 mark_y2)
{
	if (!m_pFidGlobal)	return 0;

	UINT8 u8Mark = (UINT8)m_pFidGlobal->GetCount();
	STG_XMXY stData[2] = { NULL };

	if ((u8Mark != 4) && (u8Mark != 2) && (u8Mark != 3))	return 0;
	if (mark_y1 < 1 || mark_y2 < 1 || mark_y1 > 4 || mark_y2 > 4 || mark_y1 == mark_y2)	return 0;

	/* 해당 위치의 각각 X/Y 좌표 값 얻기 */
	if (!m_pFidGlobal->GetMark(mark_y1 - 1, stData[0]))	return 0;
	if (!m_pFidGlobal->GetMark(mark_y2 - 1, stData[1]))	return 0;

	/* 두 Y 좌표의 떨어진 간격 값 반환 */
	return (stData[0].mark_y - stData[1].mark_y);
}

/*
 desc : Mark 1번과 3번, Mark 2번과 4번의 X 좌표 오차 값을 반환 (수직 오차 값)
 parm : type	- [in]  0 : Mark 2번 기준으로 Mark 1번의 X 오차 값 반환
						1 : Mark 4번 기준으로 Mark 3번의 X 오차 값 반환
						2 : Mark 1번 기준으로 Mark 2번의 X 오차 값 반환
						3 : Mark 3번 기준으로 Mark 4번의 X 오차 값 반환
 retn : 두 마크의 X 위치 시작 오차 값 반환 (단위: mm) 반환
*/
DOUBLE CJobSelectXml::GetGlobalMarkDiffVertX(UINT8 type)
{
	if (!m_pFidGlobal)	return 0;

	UINT8 u8Mark = (UINT8)m_pFidGlobal->GetCount();
	STG_XMXY stData[2] = { NULL };

#if 0
	/* 마크 개수가 부족한지 여부 확인 */
	if (m_pFidGlobal->GetCount() < 4)	return 0;
#else
	if ((u8Mark != 4) && (u8Mark != 2) && (u8Mark != 3))	return 0.0f;
#endif
	if (0 == type)
	{
		if (!m_pFidGlobal->GetMark(1, stData[0]))	return 0;
		if (!m_pFidGlobal->GetMark(0, stData[1]))	return 0;
	}
	else if (0x01 == type)
	{
		if (!m_pFidGlobal->GetMark(3, stData[0]))	return 0;
		if (!m_pFidGlobal->GetMark(2, stData[1]))	return 0;
	}
	else if (0x02 == type)
	{
		if (!m_pFidGlobal->GetMark(0, stData[0]))	return 0;
		if (!m_pFidGlobal->GetMark(1, stData[1]))	return 0;
	}
	else /*if (0x03 == type)*/
	{
		if (!m_pFidGlobal->GetMark(2, stData[0]))	return 0;
		if (!m_pFidGlobal->GetMark(3, stData[1]))	return 0;
	}
	/* 값 반환 */
	return (stData[0].mark_x - stData[1].mark_x);
}



/*
 desc : Align Camera 1번과 2번 간의 Mark X 축 떨어진 간격
 parm : mode	- [in]  0x00 : Width, 0x01 : Height
		scan	- [in]  스캔하려는 위치 (번호. 0 or 1)
 retn : 두 지점의 X 축 간의 떨어진 거리 (단위: mm)
*/
DOUBLE CJobSelectXml::GetLocalMarkACam12DistX(UINT8 mode, UINT8 scan)
{
	UINT8 u8CamX1 = 0x00, u8CamX2 = 0x00;
	DOUBLE dbDiff = 0.0f;
	STG_XMXY stData[2] = { NULL };

 

	auto status = alignMotion->status;

	
	int centerCol = status.GetCenterColumn();

	u8CamX1 = status.markMapConst[scan].back().tgt_id; //1캠
	u8CamX2 = status.markMapConst[centerCol + scan].back().tgt_id; //2캠



	/* 마크 개수가 부족한지 여부 확인 */
	if (m_pFidLocal->GetCount() <= u8CamX2)			return 0;
	/* 두 개의 X 지점 간의 떨어진 간격 구하기 (절대 값) */
	if (!m_pFidLocal->GetMark(u8CamX1, stData[0]))	return 0;
	if (!m_pFidLocal->GetMark(u8CamX2, stData[1]))	return 0;

	/* 절대 값 반환 (mm를 100 nm or 0.1 um 단위로 반환하기 위해 10000 곱하기) */
	if (0x00 == mode)	dbDiff = abs(stData[0].mark_x - stData[1].mark_x);
	else 				dbDiff = abs(stData[0].mark_y - stData[1].mark_y);

	return dbDiff;
}

/*
 desc : 두 Mark 간의 상/하 Y 축 떨어진 간격 즉, Y 축 간의 오차 (높이) 값
 parm : mark_y1	- [in]  Y1 축 Mark Number (Zero based)
		mark_y2	- [in]  Y2 축 Mark Number (Zero based)
 retn : 두 지점의 Y 축 간의 높이 차이 (단위: mm)
*/
DOUBLE CJobSelectXml::GetLocalMarkDiffY(UINT8 mark_y1, UINT8 mark_y2)
{
	STG_XMXY stData[2] = { NULL };

	if (m_pFidLocal->GetCount() <= mark_y1 ||
		m_pFidLocal->GetCount() <= mark_y2)	return 0;

	/* 해당 위치의 각각 X/Y 좌표 값 얻기 */
	if (!m_pFidLocal->GetMark(mark_y1, stData[0]))	return 0;
	if (!m_pFidLocal->GetMark(mark_y2, stData[1]))	return 0;

	/* 두 Y 좌표의 떨어진 간격 값 반환 */
	return (stData[0].mark_y - stData[1].mark_y);
}

/*
 desc : Scan 기준으로 Left.Top과 Right.Top의 X 좌표 기준으로, 그 이하의 X 좌표들이 얼마나 거버 상에서
		오차가 있게 배치되어 있는지 각 오차 값들을 구하기 위함
 parm : scan	- [in]  Align Scan하려는 위치 (번호. 0 or 1)
		cam1	- [in]  Left/Bottom에 위치한 Mark X 위치 기준으로 오차 값 반환 (Left/Top 순으로 저장) (단위: mm)
		cam2	- [in]  Right/Bottom에 위치한 Mark X 위치 기준으로 오차 값 반환 (Right/Top 순으로 저장) (단위: mm)
 retn : 저장된 데이터 개수 반환 (실패인 경우, 0)
*/
UINT32 CJobSelectXml::GetLocalMarkDiffVertX(UINT8 scan,
	CAtlList <DOUBLE>& cam1, CAtlList <DOUBLE>& cam2)
{
	UINT8 i, u8Count = 0x00, u8CamX1 = 0x00, u8CamX2 = 0x00;
	STG_XMXY stTgt[2] = { NULL }, stOrg[2] = { NULL };



	auto status = alignMotion->status;

	int cols = status.gerberColCnt;

	if (cols % 2 != 0)
	{
		LOG_ERROR(ENG_EDIC::en_luria, L"column must be devide by 2");
		return 0;
	}

	int centerCol = cols / status.acamCount;

	auto cam1Scan = status.markMapConst[scan];
	auto cam2Scan = status.markMapConst[centerCol + scan];

	if (cam1Scan.size() != cam2Scan.size())
	{
		LOG_ERROR(ENG_EDIC::en_luria, L"scan count must be same");
		return 0;
	}


	u8CamX1 = cam1Scan.front().tgt_id; //1캠
	u8CamX2 = cam2Scan.front().tgt_id; //2캠


	u8Count = cam1Scan.size();

	/* 마크 개수가 부족한지 여부 확인 */
	if (1 > u8Count)
	{
		LOG_ERROR(ENG_EDIC::en_luria, L"Not enough local marks (1 > marks)");
		return 0;
	}


		if (!m_pFidLocal->GetMark(u8CamX1, stOrg[0]) ||
			!m_pFidLocal->GetMark(u8CamX2, stOrg[1]))
		{
			LOG_ERROR(ENG_EDIC::en_luria, L"Failed to get the mark of Left or Bottom");
			return 0;
		}


	/* Scan 방향으로 원점 X (Left.Top과 Right.Top의 X 좌표) 기준으로 얼마큼 떨어졌는지 구함 */
	for (i = u8Count; i > 0; i--)
	{
		/* 첫 번째 인덱스 부터 순차적으로 증가해서 각 카메라의 Left / Bottom에서 오차 값 반환 */
		if (!m_pFidLocal->GetMark(cam1Scan[i - 1].tgt_id, stTgt[0]) ||
			!m_pFidLocal->GetMark(cam2Scan[i - 1].tgt_id, stTgt[1]))
		{
			cam1.RemoveAll();
			cam2.RemoveAll();
			LOG_ERROR(ENG_EDIC::en_luria, L"Failed to get the mark of Left or Bottom");
			return 0;
		}
		cam1.AddTail(stTgt[0].mark_x - stOrg[0].mark_x);
		cam2.AddTail(stTgt[1].mark_x - stOrg[1].mark_x);
	}

	return (UINT32)cam1.GetCount();
}

/*
 desc : Mark Type 기준으로, Camera 1번의 Left/Bottom과 Camera 2번의 Right/Bottom의 Y 좌표 간의 높이 차 반환
 parm : None
 retn : 두 지점의 Y 축 간의 높이 차이 (단위: mm)
*/
DOUBLE CJobSelectXml::GetGlobalLeftRightBottomDiffY()
{
	UINT16 u16Index[2] = { 1, 3 };	/* 기본 Global Mark가 4개인 경우 */
	STG_XMXY stData[2] = { NULL };

	if (!m_pFidGlobal)	return 0;
	/* Global Mark가 2 or 4개인 여부에 따라 */
	if (m_pFidGlobal->GetCount() == 2)
	{
		u16Index[0] = 0;	/* Mark 2개인 경우, Left 쪽이 1번 마크 */
		u16Index[1] = 1;	/* Mark 2개인 경우, Right 쪽이 2번 마크 */
	}
	if (!m_pFidGlobal->GetMark(u16Index[0], stData[0]))	return 0;
	if (!m_pFidGlobal->GetMark(u16Index[1], stData[1]))	return 0;

	return (stData[0].mark_y - stData[1].mark_y);
}

/*
 desc : Global Fiducial의 Left/Bottom or Right/Bottom Mark와 Local Fiducial의 임의 위치에 해당되는
		높이 차이 값 즉, 떨어진 간격 값
 parm : direct	- [in]  Global Fiducial의 위치 정보. 0x00 : Left/Bottom, 0x01 : Right/Bottom
		index	- [in]  Local Fiducial의 위치 정보가 저장된 인덱스 (Zero-based)
 retn : 두 마크 간의 떨어진 오차 값 (단위: mm)
*/
DOUBLE CJobSelectXml::GetGlobalBaseMarkLocalDiffY(UINT8 direct, UINT8 index)
{
	BOOL bSucc = FALSE;
	STG_XMXY stGlobal = { NULL }, stLocal = { NULL };;

	if (!m_pFidGlobal)	return 0;
	/* Global Fiducial의 Base Mark인 Left/Bottom의 거버 위치 정보 */
	if (0x00 == direct)	bSucc = m_pFidGlobal->GetMark(1, stGlobal);
	//if (0x00 == direct)	bSucc = m_pFidGlobal->GetMark(0, stGlobal);
	else				bSucc = m_pFidGlobal->GetMark(3, stGlobal);
	//else				bSucc = m_pFidGlobal->GetMark(4, stGlobal);
	if (!bSucc)	return 0;
	/* Local Fiducial의 임의 위치 정보 얻기 */
	bSucc = m_pFidLocal->GetMark(index, stLocal);
	if (!bSucc)	return 0;

	/* 2 마크 간의 높이 차이 값 반환 */
	return (stLocal.mark_y - stGlobal.mark_y);
}


/*
 desc : 거버 크기 반환
 parm : width	- [out] 넓이 값 반환 (단위: mm)
		height	- [out] 높이 값 반환 (단위: mm)
 retn : None
*/
VOID CJobSelectXml::GetGerberSize(DOUBLE& width, DOUBLE& height)
{
	width = m_dbGerberWidth;
	height = m_dbGerberHeight;
}

/*
 desc : Serial String (Serial D-Code)의 하위 Field 개수 반환
 parm : None
 retn : 0 (검색된 Serial D-Code 값이 없거나 Field가 없는 경우) or Later
*/
UINT16 CJobSelectXml::GetSerialDCodeFieldCount()
{
	UINT16 i = 0, u16Count = 0;

	do {

		/* 유효한 값이 있는지 확인. 즉, 입력된 마지막 필드인지 여부 */
		if (!m_pstPanelData)			break;
		if (!m_pstPanelData[i].d_code)	break;
		/* Serial DCode 영역인 경우만 해당됨 */
		if (0 == m_pstPanelData[i].content && m_pstPanelData[i].field_cnt > 0)
		{
			u16Count += m_pstPanelData[i].field_cnt;
		}
		i++;

	} while (i < MAX_PANEL_STRING_COUNT);

	return u16Count;
}