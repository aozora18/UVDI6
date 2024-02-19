
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
	m_pstDataSel	= NULL;
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
 desc : 현재 임시 저장되어 있는 Strength 값 메모리 해제
 parm : None
 retn : None
*/
VOID CMeasure::ResetResult()
{
	UINT32 i	= 0;

	/* 첫 번째 저장되어 있는 메모리 위치 얻기 */
	for (; i<m_arrData.GetCount(); i++)
	{
		if (m_arrData.GetAt(i))
		{
			m_arrData.GetAt(i)->RemoveArrayAll();
			delete m_arrData.GetAt(i);
		}
	}
	m_arrData.RemoveAll();
	/* 기존 선택된 항목 초기화 */
	m_pstDataSel	= NULL;
	/* 기존 선택된 이미지가 있다면 제거 */
	if (!m_csImgMark.IsNull())	m_csImgMark.Destroy();

	/* 가장 최근 저장된 이미지 제거 */
	wmemset(m_tzLastSaveFile, 0x00, MAX_PATH_LEN);

	/* 최대 / 최소 값 초기화 */
	for (i=0; i<COLLECT_DATA_COUNT; i++)
	{
		m_dbMinMax[0][i]	= DBL_MAX;
		m_dbMinMax[1][i]	= DBL_MIN;
	}
}

/*
 desc : Edge Detecting 결과 실행
 parm : cam_id	- [in]  Align Camera Index (1 or Later)
 retn : TRUE or FALSE
*/
BOOL CMeasure::SetResult(UINT8 cam_id)
{
	TCHAR tzFile[MAX_PATH_LEN], tzEdge[MAX_FILE_LEN];
	INT32 i32Index		= 0, i32PosZ = 0;
	UINT32 i, u32Find	= uvEng_Camera_GetEdgeDetectCount(cam_id);
	DOUBLE dbValSum[3]	= {NULL};
	SYSTEMTIME stTm		= {NULL};
	RECT rGrabArea		= {NULL};
	LPG_ZAAL pstEdge	= NULL;
	LPG_EDFR pstData	= uvEng_Camera_GetEdgeDetectResults(cam_id);
	LPG_CIEA pstCfg		= uvEng_GetConfig();

	/* 현재 시간 얻기 */
	GetLocalTime(&stTm);
	/* 현재 Edge 수행에 의해 검색된 이미지 임시 저장을 위한 파일 이름 설정 */
	swprintf_s(tzEdge, MAX_FILE_LEN, L"%d_%02d%02d%02d",
			   cam_id, stTm.wHour, stTm.wMinute, stTm.wSecond);
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\vdof\\%s.bmp", g_tzWorkDir, tzEdge);
	/* 임시 파일로 현재 Edge Detection 이미지 저장 */
	rGrabArea.right	= uvEng_Camera_GetLastGrabbedMark()->grab_width;
	rGrabArea.bottom= uvEng_Camera_GetLastGrabbedMark()->grab_height;
	if (!uvEng_Camera_SaveGrabbedMarkToFile(cam_id, &rGrabArea, 0x00, tzFile))
	{
		AfxMessageBox(L"Failed to save the edge detection file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* Align Camera Z 축 높이 값 (단위: mm) */
	i32PosZ	= (INT32)ROUNDED(uvCmn_MCQ_GetACamCurrentPosZ(cam_id) * 10000.0f, 0);
	/* 동일한 Z 축 높이 값이 존재하는지 검색 후, 있다면, 합산 후 평균 값 구하기 */
	pstEdge	= FocusData(i32PosZ);
	if (!pstEdge)	/* 검색 실패의 경우, 새로 생성 */
	{
		/* Hash Map 에 등록 */
		pstEdge	= new STG_ZAAL;
		ASSERT(pstEdge);
		m_arrData.Add(pstEdge);
		pstEdge->find_count	= 0;
	}

	/* 여기서 검색된 */
	for (i=0; i<u32Find; i++)
	{
		dbValSum[0]	+= pstData[i].strength;
		dbValSum[1]	+= pstData[i].length;
		dbValSum[2]	+= pstData[i].dia_meter_um;
	}

	pstEdge->z_pos		= i32PosZ;
	pstEdge->find_count	+= INT32(u32Find);
	pstEdge->arrValue[0].Add(dbValSum[0]);
	pstEdge->arrValue[1].Add(dbValSum[1]);
	pstEdge->arrValue[2].Add(dbValSum[2]);

	/* 현재 추가된 위치 값 얻기 */
	i32Index	= FocusFind(i32PosZ);
	if (i32Index < 0)	return FALSE;
	/* 등록된 List에서 Strength 값이 가장 큰 값에 Focus 되었다라고 설정 */
	FocusCalc(i32Index);
	/* 정렬 작업 수행 */
	FocusSort();	/* QSort */
	/* 가장 최근에 저장된 파일 입력 */
	swprintf_s(pstEdge->file, MAX_PATH_LEN, L"%s", tzEdge);
	wmemset(m_tzLastSaveFile, 0x00, MAX_PATH_LEN);
	wcscpy_s(m_tzLastSaveFile, MAX_PATH_LEN, pstEdge->file);

	return TRUE;
}


/*
 desc : 현재 저장되어 있는 데이터 중 동일한 Z Axis 높이 값 반환
 parm : z_pos	- [in]  검색을 위한 Z Axis 높이 값 (단위: 0.1 um or 100 nm)
 retn : 검색된 구조체 포인터. 없다면 NULL
*/
LPG_ZAAL CMeasure::FocusData(INT32 z_pos)
{
	INT32 i	= 0;
	for (; i<m_arrData.GetCount(); i++)
	{
		if (z_pos == m_arrData.GetAt(i)->z_pos)	return m_arrData.GetAt(i);
	}

	return NULL;
}

/*
 desc : 현재 저장되어 있는 데이터 중 Z Axis 높이 값이 저장되어 있는 Array 위치 (Zero based) 반환
 parm : z_pos	- [in]  검색을 위한 Z Axis 높이 값 (단위: 0.1 um or 100 nm)
 retn : Zero-based 위치 Index 반환, 실패의 경우 -1
*/
INT32 CMeasure::FocusFind(INT32 z_pos)
{
	INT32 i	= 0;
	for (; i<m_arrData.GetCount(); i++)
	{
		if (z_pos == m_arrData.GetAt(i)->z_pos)	return i;
	}

	return -1;
}

/*
 desc : 현재 등록된 데이터들 중에서 엣지 강도가 가장 높은 (평균 값 중) 값에 Focus 설정
 parm : item	- [in]  해당 위치의 데이터만 작업 수행
 retn : None
*/
VOID CMeasure::FocusCalc(INT32 index)
{
	UINT32 i, j;
	UINT32 i32Avg[COLLECT_DATA_COUNT]= {NULL};
	UINT32 i32Max[COLLECT_DATA_COUNT]= {NULL};
	UINT32 i32Min[COLLECT_DATA_COUNT]= {NULL};
	DOUBLE dbAvg[COLLECT_DATA_COUNT] = { FLT_MIN, FLT_MIN, FLT_MIN/*FLT_MAX*/ };
	DOUBLE dbMax[COLLECT_DATA_COUNT] = { FLT_MIN, FLT_MIN, FLT_MIN };
	DOUBLE dbMin[COLLECT_DATA_COUNT] = { FLT_MAX, FLT_MAX, FLT_MAX };

	/* 최대 / 최소 값 초기화 */
	for (i=0; i<2; i++)
	{
		/* i == 0 -> Min, i == 1 -> Max */
		for (j=0; j<COLLECT_DATA_COUNT; j++)
		{
			m_dbMinMax[i][j] = (i == 0) ? DBL_MAX : DBL_MIN;
		}
	}

	/* 유효성 검사 */
	if (m_arrData.GetCount() < index)	return;
	/* 현재까지 측정된 데이터들의 평균 구함 (My Algorithm) */
#if 0
	m_arrData.GetAt(index)->CalcAverage();
#else	/* 정규분포 알고리즘 적용 */
	m_arrData.GetAt(index)->CalcAverageEx(2.0f);
#endif
#if 1
	/* 가장 큰 값 위치 확인 */
	for (i=0; i<m_arrData.GetCount(); i++)
	{
		/* 엣지들의 Strength, Length, Feret, Diameter 값 중 가장 큰 값에 항목 설정 */
		for (j=0; j<COLLECT_DATA_COUNT; j++)
		{
			m_arrData.GetAt(i)->set_value[j]	= 0x00;
#if 0
			if (j < 2)	/* Max:Strength, Length */
			{
				if (m_arrData.GetAt(i)->avg_value[j] > dbAvg[j])
				{
					dbAvg[j]	= m_arrData.GetAt(i)->avg_value[j];
					i32Avg[j]	= i;
				}
			}
			else	/* Min : Diameter */
			{
				if (m_arrData.GetAt(i)->avg_value[j] < dbAvg[j])
				{
					dbAvg[j]	= m_arrData.GetAt(i)->avg_value[j];
					i32Avg[j]	= i;
				}
			}
#else
			if (m_arrData.GetAt(i)->avg_value[j] > dbAvg[j])
			{
				dbAvg[j]	= m_arrData.GetAt(i)->avg_value[j];
				i32Avg[j]	= i;
			}
#endif
			if (m_arrData.GetAt(i)->max_value[j] > dbMax[j])
			{
				dbMax[j]	= m_arrData.GetAt(i)->max_value[j];
				i32Max[j]	= i;
			}
			if (m_arrData.GetAt(i)->min_value[j] < dbMin[j])
			{
				dbMin[j]	= m_arrData.GetAt(i)->min_value[j];
				i32Min[j]	= i;
			}
		}
	}

	/* 현재 각 항목들 중에서 가장 큰 값에 플래그 설정 */
	for (j=0; j<COLLECT_DATA_COUNT; j++)
	{
		m_arrData.GetAt(i32Avg[j])->set_value[j]= 0x01;
		/* 각 항목별 최소 / 최대 값 재설정 */
		m_dbMinMax[0][j]	= m_arrData.GetAt(i32Min[j])->min_value[j];
		m_dbMinMax[1][j]	= m_arrData.GetAt(i32Max[j])->max_value[j];
	}
#endif
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
	LPG_ZAAL *pstElem1	= (LPG_ZAAL*)elem1;
	LPG_ZAAL *pstElem2	= (LPG_ZAAL*)elem2;
	return ((*pstElem1)->z_pos - (*pstElem2)->z_pos);
}

/*
 desc : Quick Sort 정렬 알고리즘 적용하여 CArray 정렬 수행
 parm : None
 retn : None
*/
VOID CMeasure::FocusSort()
{
	qsort (m_arrData.GetData(), m_arrData.GetCount(), sizeof(LPG_ZAAL), CompareZAxisFocus);
}

/*
 desc : 각 항목 별 최소 혹은 최대 값 반환
 parm : mode	- [in]  0x00: Min, 0x01: Max
		type	- [in]  0x00 : strength, 0x01 : length, 0x02 : Feret, 0x03 : Diameter
 retn : 임시 저장된 값 반환
*/
DOUBLE CMeasure::GetMinMax(UINT8 mode, UINT8 type)
{
	if (mode > 0x01 || type > 0x03)	return 0.0f;
	return m_dbMinMax[mode][type];
}

/*
 desc : 현재까지 측정된 데이터 반환
 parm : index	- [in]  가져오고자 하는 메모리 위치 (Zero-based)
 retn : index 위치에 저장된 측정 값
*/
LPG_ZAAL CMeasure::GetResult(UINT32 index)
{
	if (m_arrData.GetCount() <= index)	return NULL;
	return (m_pstDataSel = m_arrData.GetAt(index));
}

/*
 desc : 리스트 컨트롤에서 임의 항목을 선택한 경우, 이미지 출력
 parm : file	- [in]  선택된 파일
 retn : None
*/
VOID CMeasure::SelectEdgeFile(PTCHAR file)
{
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	
	/* 기존 적재된 이미지가 있으면 해제 처리 */
	if (!m_csImgMark.IsNull())	m_csImgMark.Destroy();
	/* 선택된 위치의 파일 이름 얻기 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\vdof\\%s.bmp", g_tzWorkDir, file);
	/* Bitmap Image 적재 */
	if (S_OK != m_csImgMark.Load(tzFile))
	{
		AfxMessageBox(L"Failed to load the edge file", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : 가장 마지막에 측정 (검출)된 엣지 이미지 선택
 parm : None
 retn : None
*/
VOID CMeasure::SelectLastEdgeFile()
{
	INT32 i32Index	= (INT32)m_arrData.GetCount();

	/* 등록된 개수가 없는지 확인 */
	if (i32Index < 1)	return;

	/* 가장 최근에 선택된 결과 설정 */
	SelectEdgeFile(m_tzLastSaveFile);
	m_pstDataSel	= m_arrData.GetAt(i32Index-1);
}