
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
	m_pstDataSel	= NULL;
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
 desc : ���� �ӽ� ����Ǿ� �ִ� Strength �� �޸� ����
 parm : None
 retn : None
*/
VOID CMeasure::ResetResult()
{
	UINT32 i	= 0;

	/* ù ��° ����Ǿ� �ִ� �޸� ��ġ ��� */
	for (; i<m_arrData.GetCount(); i++)
	{
		if (m_arrData.GetAt(i))
		{
			m_arrData.GetAt(i)->RemoveArrayAll();
			delete m_arrData.GetAt(i);
		}
	}
	m_arrData.RemoveAll();
	/* ���� ���õ� �׸� �ʱ�ȭ */
	m_pstDataSel	= NULL;
	/* ���� ���õ� �̹����� �ִٸ� ���� */
	if (!m_csImgMark.IsNull())	m_csImgMark.Destroy();

	/* ���� �ֱ� ����� �̹��� ���� */
	wmemset(m_tzLastSaveFile, 0x00, MAX_PATH_LEN);

	/* �ִ� / �ּ� �� �ʱ�ȭ */
	for (i=0; i<COLLECT_DATA_COUNT; i++)
	{
		m_dbMinMax[0][i]	= DBL_MAX;
		m_dbMinMax[1][i]	= DBL_MIN;
	}
}

/*
 desc : Edge Detecting ��� ����
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

	/* ���� �ð� ��� */
	GetLocalTime(&stTm);
	/* ���� Edge ���࿡ ���� �˻��� �̹��� �ӽ� ������ ���� ���� �̸� ���� */
	swprintf_s(tzEdge, MAX_FILE_LEN, L"%d_%02d%02d%02d",
			   cam_id, stTm.wHour, stTm.wMinute, stTm.wSecond);
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\vdof\\%s.bmp", g_tzWorkDir, tzEdge);
	/* �ӽ� ���Ϸ� ���� Edge Detection �̹��� ���� */
	rGrabArea.right	= uvEng_Camera_GetLastGrabbedMark()->grab_width;
	rGrabArea.bottom= uvEng_Camera_GetLastGrabbedMark()->grab_height;
	if (!uvEng_Camera_SaveGrabbedMarkToFile(cam_id, &rGrabArea, 0x00, tzFile))
	{
		AfxMessageBox(L"Failed to save the edge detection file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* Align Camera Z �� ���� �� (����: mm) */
	i32PosZ	= (INT32)ROUNDED(uvCmn_MCQ_GetACamCurrentPosZ(cam_id) * 10000.0f, 0);
	/* ������ Z �� ���� ���� �����ϴ��� �˻� ��, �ִٸ�, �ջ� �� ��� �� ���ϱ� */
	pstEdge	= FocusData(i32PosZ);
	if (!pstEdge)	/* �˻� ������ ���, ���� ���� */
	{
		/* Hash Map �� ��� */
		pstEdge	= new STG_ZAAL;
		ASSERT(pstEdge);
		m_arrData.Add(pstEdge);
		pstEdge->find_count	= 0;
	}

	/* ���⼭ �˻��� */
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

	/* ���� �߰��� ��ġ �� ��� */
	i32Index	= FocusFind(i32PosZ);
	if (i32Index < 0)	return FALSE;
	/* ��ϵ� List���� Strength ���� ���� ū ���� Focus �Ǿ��ٶ�� ���� */
	FocusCalc(i32Index);
	/* ���� �۾� ���� */
	FocusSort();	/* QSort */
	/* ���� �ֱٿ� ����� ���� �Է� */
	swprintf_s(pstEdge->file, MAX_PATH_LEN, L"%s", tzEdge);
	wmemset(m_tzLastSaveFile, 0x00, MAX_PATH_LEN);
	wcscpy_s(m_tzLastSaveFile, MAX_PATH_LEN, pstEdge->file);

	return TRUE;
}


/*
 desc : ���� ����Ǿ� �ִ� ������ �� ������ Z Axis ���� �� ��ȯ
 parm : z_pos	- [in]  �˻��� ���� Z Axis ���� �� (����: 0.1 um or 100 nm)
 retn : �˻��� ����ü ������. ���ٸ� NULL
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
 desc : ���� ����Ǿ� �ִ� ������ �� Z Axis ���� ���� ����Ǿ� �ִ� Array ��ġ (Zero based) ��ȯ
 parm : z_pos	- [in]  �˻��� ���� Z Axis ���� �� (����: 0.1 um or 100 nm)
 retn : Zero-based ��ġ Index ��ȯ, ������ ��� -1
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
 desc : ���� ��ϵ� �����͵� �߿��� ���� ������ ���� ���� (��� �� ��) ���� Focus ����
 parm : item	- [in]  �ش� ��ġ�� �����͸� �۾� ����
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

	/* �ִ� / �ּ� �� �ʱ�ȭ */
	for (i=0; i<2; i++)
	{
		/* i == 0 -> Min, i == 1 -> Max */
		for (j=0; j<COLLECT_DATA_COUNT; j++)
		{
			m_dbMinMax[i][j] = (i == 0) ? DBL_MAX : DBL_MIN;
		}
	}

	/* ��ȿ�� �˻� */
	if (m_arrData.GetCount() < index)	return;
	/* ������� ������ �����͵��� ��� ���� (My Algorithm) */
#if 0
	m_arrData.GetAt(index)->CalcAverage();
#else	/* ���Ժ��� �˰��� ���� */
	m_arrData.GetAt(index)->CalcAverageEx(2.0f);
#endif
#if 1
	/* ���� ū �� ��ġ Ȯ�� */
	for (i=0; i<m_arrData.GetCount(); i++)
	{
		/* �������� Strength, Length, Feret, Diameter �� �� ���� ū ���� �׸� ���� */
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

	/* ���� �� �׸�� �߿��� ���� ū ���� �÷��� ���� */
	for (j=0; j<COLLECT_DATA_COUNT; j++)
	{
		m_arrData.GetAt(i32Avg[j])->set_value[j]= 0x01;
		/* �� �׸� �ּ� / �ִ� �� �缳�� */
		m_dbMinMax[0][j]	= m_arrData.GetAt(i32Min[j])->min_value[j];
		m_dbMinMax[1][j]	= m_arrData.GetAt(i32Max[j])->max_value[j];
	}
#endif
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
	LPG_ZAAL *pstElem1	= (LPG_ZAAL*)elem1;
	LPG_ZAAL *pstElem2	= (LPG_ZAAL*)elem2;
	return ((*pstElem1)->z_pos - (*pstElem2)->z_pos);
}

/*
 desc : Quick Sort ���� �˰��� �����Ͽ� CArray ���� ����
 parm : None
 retn : None
*/
VOID CMeasure::FocusSort()
{
	qsort (m_arrData.GetData(), m_arrData.GetCount(), sizeof(LPG_ZAAL), CompareZAxisFocus);
}

/*
 desc : �� �׸� �� �ּ� Ȥ�� �ִ� �� ��ȯ
 parm : mode	- [in]  0x00: Min, 0x01: Max
		type	- [in]  0x00 : strength, 0x01 : length, 0x02 : Feret, 0x03 : Diameter
 retn : �ӽ� ����� �� ��ȯ
*/
DOUBLE CMeasure::GetMinMax(UINT8 mode, UINT8 type)
{
	if (mode > 0x01 || type > 0x03)	return 0.0f;
	return m_dbMinMax[mode][type];
}

/*
 desc : ������� ������ ������ ��ȯ
 parm : index	- [in]  ���������� �ϴ� �޸� ��ġ (Zero-based)
 retn : index ��ġ�� ����� ���� ��
*/
LPG_ZAAL CMeasure::GetResult(UINT32 index)
{
	if (m_arrData.GetCount() <= index)	return NULL;
	return (m_pstDataSel = m_arrData.GetAt(index));
}

/*
 desc : ����Ʈ ��Ʈ�ѿ��� ���� �׸��� ������ ���, �̹��� ���
 parm : file	- [in]  ���õ� ����
 retn : None
*/
VOID CMeasure::SelectEdgeFile(PTCHAR file)
{
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	
	/* ���� ����� �̹����� ������ ���� ó�� */
	if (!m_csImgMark.IsNull())	m_csImgMark.Destroy();
	/* ���õ� ��ġ�� ���� �̸� ��� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\vdof\\%s.bmp", g_tzWorkDir, file);
	/* Bitmap Image ���� */
	if (S_OK != m_csImgMark.Load(tzFile))
	{
		AfxMessageBox(L"Failed to load the edge file", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : ���� �������� ���� (����)�� ���� �̹��� ����
 parm : None
 retn : None
*/
VOID CMeasure::SelectLastEdgeFile()
{
	INT32 i32Index	= (INT32)m_arrData.GetCount();

	/* ��ϵ� ������ ������ Ȯ�� */
	if (i32Index < 1)	return;

	/* ���� �ֱٿ� ���õ� ��� ���� */
	SelectEdgeFile(m_tzLastSaveFile);
	m_pstDataSel	= m_arrData.GetAt(i32Index-1);
}