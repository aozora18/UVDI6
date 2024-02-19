
/*
 desc : Focus 측정 데이터 관리
*/

#include "pch.h"
#include "../MainApp.h"
#include "Measure.h"

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 생성자
 parm : None
 retn : None
*/
CMeasure::CMeasure()
{
	m_pstLastResult	= NULL;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CMeasure::~CMeasure()
{
	/* 가장 최근 분석된 데이터 메모리 해제 */
	ResetResult();
}

/*
 desc : 현재 임시 저장되어 있는 값 메모리 해제
 parm : None
 retn : None
*/
VOID CMeasure::ResetResult()
{
	UINT32 i	= 0, k = 0;

	/* 첫 번째 저장되어 있는 메모리 위치 얻기 */
	for (k=0; k<MAX_PH; k++)
	{
		for (i=0; i<m_arrData[k].GetCount(); i++)
		{
			if (m_arrData[k].GetAt(i))
			{
				m_arrData[k].GetAt(i)->RemoveArrayAll();
				delete m_arrData[k].GetAt(i);
			}
		}
		m_arrData[k].RemoveAll();
	}
	/* 기존 선택된 항목 초기화 */
	m_pstLastResult	= NULL;
	/* 기존 선택된 이미지가 있다면 제거 */
	if (!m_csImgMark.IsNull())	m_csImgMark.Destroy();
}

/*
 desc : Edge Detecting 결과 실행
 parm : cam_id	- [in]  Align Camera Index (1 or Later)
		ph_no	- [in]  Photohead Number (1 ~ 8)
		steps	- [in]  측정 구간 (1 ~ xxxx)
 retn : TRUE or FALSE
*/
BOOL CMeasure::SetResult(UINT8 cam_id, UINT8 ph_no, UINT16 steps)
{
	TCHAR tzFile[MAX_PATH_LEN], tzEdge[MAX_FILE_LEN];
	INT32 i32Index		= 0;
	UINT32 i			= 0;
	LPG_ACGR pstResult	= uvEng_Camera_GetLastGrabbedMark();
	DOUBLE dbValSum[COLLECT_DATA_COUNT]	= {NULL};
	SYSTEMTIME stTm		= {NULL};
	RECT rGrabArea;
	LPG_ZAAL pstFind	= NULL;
	LPG_CIEA pstCfg		= uvEng_GetConfig();

	/* 현재 시간 얻기 */
	GetLocalTime(&stTm);
	/* 현재 Edge 수행에 의해 검색된 이미지 임시 저장을 위한 파일 이름 설정 */
	swprintf_s(tzEdge, MAX_FILE_LEN, L"%d_%02d%02d%02d",
			   cam_id, stTm.wHour, stTm.wMinute, stTm.wSecond);
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\vdof\\%s.bmp", g_tzWorkDir, tzEdge);
	/* 임시 파일로 현재 Edge Detection 이미지 저장 */
	rGrabArea.left	= 0;
	rGrabArea.top	= 0;
	rGrabArea.right	= pstResult->grab_width;
	rGrabArea.bottom= pstResult->grab_height;
	if (!uvEng_Camera_SaveGrabbedMarkToFile(cam_id, &rGrabArea, 0x02, tzFile))
	{
		AfxMessageBox(L"Failed to save the edge detection file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 동일한 Z 축 높이 값이 존재하는지 검색 후, 있다면, 합산 후 평균 값 구하기 */
	pstFind	= FocusData(ph_no, steps);
	if (!pstFind)	/* 검색 실패의 경우, 새로 생성 */
	{
		/* Hash Map 에 등록 */
		pstFind	= new STG_ZAAL;
		ASSERT(pstFind);
		m_arrData[ph_no-1].Add(pstFind);
		pstFind->ph_no	= ph_no;
		pstFind->steps	= steps;
	}

	pstFind->arrValue[0].Add(pstResult->score_rate);		/* 매칭 검색률 (단위: %) */
	pstFind->arrValue[1].Add(pstResult->scale_rate);		/* 매칭된 이미지의 스케일 (배율) (단위: %) */
	pstFind->arrValue[2].Add(pstResult->coverage_rate);		/* 총 검색된 Edge들의 비율 값 (단위: %) */
	pstFind->arrValue[3].Add(pstResult->fit_error);			/* 검색된 엣지와 모델 엣지 간의 2차원 거리 값 (0.0 %에 가까울 수록 좋음) */

	/* 현재 추가된 위치 값 얻기 */
	i32Index	= FocusFind(ph_no, steps);
	if (i32Index < 0)	return FALSE;
	/* 등록된 List에서 Scale 값이 가장 큰 값에 Focus 되었다라고 설정 */
	FocusCalc(ph_no, i32Index);
	/* 정렬 작업 수행 */
	FocusSort(ph_no);	/* QSort */

	/* 가장 최근에 처리된 결과 데이터 임시 저장 */
	m_pstLastResult	= pstFind;

	return TRUE;
}


/*
 desc : 현재 저장되어 있는 데이터 중 동일한 Z Axis 높이 값 반환
 parm : ph_no	- [in]  Photohead Number (1 ~ 8)
		steps	- [in]  측정 구간 (1 ~ xxxx)
 retn : 검색된 구조체 포인터. 없다면 NULL
*/
LPG_ZAAL CMeasure::FocusData(UINT8 ph_no, UINT16 steps)
{
	INT32 i	= 0;
	for (; i<m_arrData[ph_no-1].GetCount(); i++)
	{
		if (steps == m_arrData[ph_no-1].GetAt(i)->steps)	return m_arrData[ph_no-1].GetAt(i);
	}

	return NULL;
}

/*
 desc : 현재 저장되어 있는 데이터 중 Z Axis 높이 값이 저장되어 있는 Array 위치 (Zero based) 반환
 parm : ph_no	- [in]  Photohead Number (1 ~ 8)
		steps	- [in]  측정 구간 (1 ~ xxxx)
 retn : Zero-based 위치 Index 반환, 실패의 경우 -1
*/
INT32 CMeasure::FocusFind(UINT8 ph_no, UINT16 steps)
{
	INT32 i	= 0;
	for (; i<m_arrData[ph_no-1].GetCount(); i++)
	{
		if (steps == m_arrData[ph_no-1].GetAt(i)->steps)	return i;
	}

	return -1;
}

/*
 desc : 현재 등록된 데이터들 중에서 값이 가장 높은 혹은 작은 (평균 값 중) 값에 Focus 설정
 parm : ph_no	- [in]  Photohead Number (1 ~ 8)
		item	- [in]  해당 위치의 데이터만 작업 수행
 retn : None
*/
VOID CMeasure::FocusCalc(UINT8 ph_no, INT32 index)
{
	INT32 i32Count	= (INT32)m_arrData[ph_no-1].GetCount();
	UINT32 i, j;
	/* (Score Rate, Scale Rate, Square Rate) */
	UINT32 i32Avg[COLLECT_DATA_COUNT]= {NULL};
	DOUBLE dbAvg[COLLECT_DATA_COUNT] = { FLT_MIN, FLT_MIN, FLT_MIN, FLT_MAX };
	LPG_ZAAL pstData	= NULL;

	/* 유효성 검사 */
	if (i32Count < 0 || i32Count < index)	return;

	/* 현재까지 측정된 데이터들의 평균 구함 (My Algorithm) */
#if 0
	m_arrData[ph_no-1].GetAt(index)->CalcAverage();
#else	/* 정규분포 알고리즘 적용 */
	pstData	= m_arrData[ph_no-1].GetAt(index);
	if (!pstData)	return;
	pstData->CalcAverageEx(2.0f);
#endif

	for (i=0; i<m_arrData[ph_no-1].GetCount(); i++)
	{
		for (j=0; j<COLLECT_DATA_COUNT; j++)
		{
			m_arrData[ph_no-1].GetAt(i)->set_value[j]	= 0x00;

			/* Score, Scale, Coverage Rate 값은 큰 값이 유효 */
			if (j != 3)
			{
				if (m_arrData[ph_no-1].GetAt(i)->avg_value[j] >= dbAvg[j])
				{
					dbAvg[j]	= m_arrData[ph_no-1].GetAt(i)->avg_value[j];
					i32Avg[j]	= i;
					TRACE(L"PH_%d : data_num=%4d [set_pos(%d)] = %d\n", ph_no, i, j, i);
				}
			}
			/* Fit Error 값은 작은 값이 유효 값 */
			else
			{
				if (m_arrData[ph_no-1].GetAt(i)->avg_value[j] <= dbAvg[j])
				{
					dbAvg[j]	= m_arrData[ph_no-1].GetAt(i)->avg_value[j];
					i32Avg[j]	= i;
					TRACE(L"PH_%d : data_num=%4d [set_pos(%d)] = %d\n\n", ph_no, i, j, i);
				}
			}
		}
	}

	/* 현재 각 항목들 중에서 가장 큰 값 혹은 작은 값에 플래그 설정 (Score Rate, Scale Rate, Coverage Rate, Fit Error) */
	for (j=0; j<COLLECT_DATA_COUNT; j++)
	{
		m_arrData[ph_no-1].GetAt(i32Avg[j])->set_value[j]= 0x01;
	}
}

/*
 desc : LPG_ZAAL 구조체 포인터 자료 비교 함수
 parm : elem1	- [in]  비교하기 위한 구조체 포인터
		elem2	- [in]  비교하기 위한 구조체 포인터
 retn : elem1 - elem2인 경우, 오름차순 정렬
		elem2 - elem1인 경우, 내림차순 정렬
*/
static int CompareZAxisFocus(const void *elem1, const void *elem2)
{
	UINT8 u16Elem1 = 0x0000, u16Elem2 = 0x0000;
	LPG_ZAAL *pstElem1	= (LPG_ZAAL*)elem1;
	LPG_ZAAL *pstElem2	= (LPG_ZAAL*)elem2;

	/* 각 값 저장 (상위1 바이트: ph_no, 하위 1바이트: steps) */
	u16Elem1	= (*pstElem1)->ph_no << 8 | (*pstElem1)->steps;
	u16Elem2	= (*pstElem2)->ph_no << 8 | (*pstElem2)->steps;

	return (u16Elem1 - u16Elem2);
}

/*
 desc : Quick Sort 정렬 알고리즘 적용하여 CArray 정렬 수행
 parm : ph_no	- [in]  Photohead Number (1 ~ 8)
 retn : None
*/
VOID CMeasure::FocusSort(UINT8 ph_no)
{
	qsort (m_arrData[ph_no-1].GetData(), m_arrData[ph_no-1].GetCount(), sizeof(LPG_ZAAL), CompareZAxisFocus);
}

/*
 desc : 현재까지 측정된 데이터 반환
 parm : ph_no	- [in]  Photohead Number (1 ~ 8)
		index	- [in]  가져오고자 하는 메모리 위치 (Zero-based)
 retn : index 위치에 저장된 측정 값
*/
LPG_ZAAL CMeasure::GetResult(UINT8 ph_no, UINT32 index)
{
	if (m_arrData[ph_no-1].GetCount() <= index)	return NULL;
	return m_arrData[ph_no-1].GetAt(index);
}