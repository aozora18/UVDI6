
/*
 desc : Focus ���� ������ ����
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
 desc : ������
 parm : None
 retn : None
*/
CMeasure::CMeasure()
{
	m_pstLastResult	= NULL;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CMeasure::~CMeasure()
{
	/* ���� �ֱ� �м��� ������ �޸� ���� */
	ResetResult();
}

/*
 desc : ���� �ӽ� ����Ǿ� �ִ� �� �޸� ����
 parm : None
 retn : None
*/
VOID CMeasure::ResetResult()
{
	UINT32 i	= 0, k = 0;

	/* ù ��° ����Ǿ� �ִ� �޸� ��ġ ��� */
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
	/* ���� ���õ� �׸� �ʱ�ȭ */
	m_pstLastResult	= NULL;
	/* ���� ���õ� �̹����� �ִٸ� ���� */
	if (!m_csImgMark.IsNull())	m_csImgMark.Destroy();
}

/*
 desc : Edge Detecting ��� ����
 parm : cam_id	- [in]  Align Camera Index (1 or Later)
		ph_no	- [in]  Photohead Number (1 ~ 8)
		steps	- [in]  ���� ���� (1 ~ xxxx)
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

	/* ���� �ð� ��� */
	GetLocalTime(&stTm);
	/* ���� Edge ���࿡ ���� �˻��� �̹��� �ӽ� ������ ���� ���� �̸� ���� */
	swprintf_s(tzEdge, MAX_FILE_LEN, L"%d_%02d%02d%02d",
			   cam_id, stTm.wHour, stTm.wMinute, stTm.wSecond);
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\vdof\\%s.bmp", g_tzWorkDir, tzEdge);
	/* �ӽ� ���Ϸ� ���� Edge Detection �̹��� ���� */
	rGrabArea.left	= 0;
	rGrabArea.top	= 0;
	rGrabArea.right	= pstResult->grab_width;
	rGrabArea.bottom= pstResult->grab_height;
	if (!uvEng_Camera_SaveGrabbedMarkToFile(cam_id, &rGrabArea, 0x02, tzFile))
	{
		AfxMessageBox(L"Failed to save the edge detection file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ������ Z �� ���� ���� �����ϴ��� �˻� ��, �ִٸ�, �ջ� �� ��� �� ���ϱ� */
	pstFind	= FocusData(ph_no, steps);
	if (!pstFind)	/* �˻� ������ ���, ���� ���� */
	{
		/* Hash Map �� ��� */
		pstFind	= new STG_ZAAL;
		ASSERT(pstFind);
		m_arrData[ph_no-1].Add(pstFind);
		pstFind->ph_no	= ph_no;
		pstFind->steps	= steps;
	}

	pstFind->arrValue[0].Add(pstResult->score_rate);		/* ��Ī �˻��� (����: %) */
	pstFind->arrValue[1].Add(pstResult->scale_rate);		/* ��Ī�� �̹����� ������ (����) (����: %) */
	pstFind->arrValue[2].Add(pstResult->coverage_rate);		/* �� �˻��� Edge���� ���� �� (����: %) */
	pstFind->arrValue[3].Add(pstResult->fit_error);			/* �˻��� ������ �� ���� ���� 2���� �Ÿ� �� (0.0 %�� ����� ���� ����) */

	/* ���� �߰��� ��ġ �� ��� */
	i32Index	= FocusFind(ph_no, steps);
	if (i32Index < 0)	return FALSE;
	/* ��ϵ� List���� Scale ���� ���� ū ���� Focus �Ǿ��ٶ�� ���� */
	FocusCalc(ph_no, i32Index);
	/* ���� �۾� ���� */
	FocusSort(ph_no);	/* QSort */

	/* ���� �ֱٿ� ó���� ��� ������ �ӽ� ���� */
	m_pstLastResult	= pstFind;

	return TRUE;
}


/*
 desc : ���� ����Ǿ� �ִ� ������ �� ������ Z Axis ���� �� ��ȯ
 parm : ph_no	- [in]  Photohead Number (1 ~ 8)
		steps	- [in]  ���� ���� (1 ~ xxxx)
 retn : �˻��� ����ü ������. ���ٸ� NULL
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
 desc : ���� ����Ǿ� �ִ� ������ �� Z Axis ���� ���� ����Ǿ� �ִ� Array ��ġ (Zero based) ��ȯ
 parm : ph_no	- [in]  Photohead Number (1 ~ 8)
		steps	- [in]  ���� ���� (1 ~ xxxx)
 retn : Zero-based ��ġ Index ��ȯ, ������ ��� -1
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
 desc : ���� ��ϵ� �����͵� �߿��� ���� ���� ���� Ȥ�� ���� (��� �� ��) ���� Focus ����
 parm : ph_no	- [in]  Photohead Number (1 ~ 8)
		item	- [in]  �ش� ��ġ�� �����͸� �۾� ����
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

	/* ��ȿ�� �˻� */
	if (i32Count < 0 || i32Count < index)	return;

	/* ������� ������ �����͵��� ��� ���� (My Algorithm) */
#if 0
	m_arrData[ph_no-1].GetAt(index)->CalcAverage();
#else	/* ���Ժ��� �˰��� ���� */
	pstData	= m_arrData[ph_no-1].GetAt(index);
	if (!pstData)	return;
	pstData->CalcAverageEx(2.0f);
#endif

	for (i=0; i<m_arrData[ph_no-1].GetCount(); i++)
	{
		for (j=0; j<COLLECT_DATA_COUNT; j++)
		{
			m_arrData[ph_no-1].GetAt(i)->set_value[j]	= 0x00;

			/* Score, Scale, Coverage Rate ���� ū ���� ��ȿ */
			if (j != 3)
			{
				if (m_arrData[ph_no-1].GetAt(i)->avg_value[j] >= dbAvg[j])
				{
					dbAvg[j]	= m_arrData[ph_no-1].GetAt(i)->avg_value[j];
					i32Avg[j]	= i;
					TRACE(L"PH_%d : data_num=%4d [set_pos(%d)] = %d\n", ph_no, i, j, i);
				}
			}
			/* Fit Error ���� ���� ���� ��ȿ �� */
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

	/* ���� �� �׸�� �߿��� ���� ū �� Ȥ�� ���� ���� �÷��� ���� (Score Rate, Scale Rate, Coverage Rate, Fit Error) */
	for (j=0; j<COLLECT_DATA_COUNT; j++)
	{
		m_arrData[ph_no-1].GetAt(i32Avg[j])->set_value[j]= 0x01;
	}
}

/*
 desc : LPG_ZAAL ����ü ������ �ڷ� �� �Լ�
 parm : elem1	- [in]  ���ϱ� ���� ����ü ������
		elem2	- [in]  ���ϱ� ���� ����ü ������
 retn : elem1 - elem2�� ���, �������� ����
		elem2 - elem1�� ���, �������� ����
*/
static int CompareZAxisFocus(const void *elem1, const void *elem2)
{
	UINT8 u16Elem1 = 0x0000, u16Elem2 = 0x0000;
	LPG_ZAAL *pstElem1	= (LPG_ZAAL*)elem1;
	LPG_ZAAL *pstElem2	= (LPG_ZAAL*)elem2;

	/* �� �� ���� (����1 ����Ʈ: ph_no, ���� 1����Ʈ: steps) */
	u16Elem1	= (*pstElem1)->ph_no << 8 | (*pstElem1)->steps;
	u16Elem2	= (*pstElem2)->ph_no << 8 | (*pstElem2)->steps;

	return (u16Elem1 - u16Elem2);
}

/*
 desc : Quick Sort ���� �˰��� �����Ͽ� CArray ���� ����
 parm : ph_no	- [in]  Photohead Number (1 ~ 8)
 retn : None
*/
VOID CMeasure::FocusSort(UINT8 ph_no)
{
	qsort (m_arrData[ph_no-1].GetData(), m_arrData[ph_no-1].GetCount(), sizeof(LPG_ZAAL), CompareZAxisFocus);
}

/*
 desc : ������� ������ ������ ��ȯ
 parm : ph_no	- [in]  Photohead Number (1 ~ 8)
		index	- [in]  ���������� �ϴ� �޸� ��ġ (Zero-based)
 retn : index ��ġ�� ����� ���� ��
*/
LPG_ZAAL CMeasure::GetResult(UINT8 ph_no, UINT32 index)
{
	if (m_arrData[ph_no-1].GetCount() <= index)	return NULL;
	return m_arrData[ph_no-1].GetAt(index);
}