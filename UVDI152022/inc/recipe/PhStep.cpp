
/*
 desc : ���а� ���� �ӵ� ��, �������� �̵� �ӵ��� ���� ���а���� ���� �� (������) ����
*/

#include "pch.h"
#include "PhStep.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : config	- [in]  ���� ȯ�� ����
		work	- [in]  �۾� �⺻ ��� (Full path)
		shmem	- [in]  �����ǰ� ����� �޸� ���� ��ġ (����ü ������)
 retn : None
*/
CPhStep::CPhStep(PTCHAR work_dir, LPG_OSSD shmem)
	: CBase(work_dir)
{
	m_pstSelRecipe	= NULL;
	m_pstShMemRecipe= shmem;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CPhStep::~CPhStep()
{
}

/*
 desc : ���� ��ϵ� ������ ��� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CPhStep::ResetRecipeAll()
{
	/* ���� ��ϵ� Recipe Data �޸� �ʱ�ȭ */
	memset(m_pstShMemRecipe, 0x00, sizeof(STG_OSSD) * MAX_PH_STEP_LINES/*GetConfig()->ph_step.max_ph_step*/);
	/* ���� ���õ� ������ �ʱ�ȭ */
	m_pstSelRecipe	= NULL;
}

/*
 desc : Calibration Recipe ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CPhStep::LoadFile()
{
	CHAR szData[1024]			= {NULL};
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	UINT32 u32Index	= 0;	/* �����ǰ� ����� �޸� ��ġ ��, ����ü �ε��� (Zero-based) */
	errno_t eRet	= 0;
	FILE *fp		= NULL;
	CUniToChar csCnv;

	/* ���� ��ϵ� ������ ���� */
	ResetRecipeAll();

	/* Recipe File ���� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\cali\\%s.dat",
			   m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.ph_step);
	/* ���� ���� */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "rt");
	if (0 != eRet)	return TRUE;
	while (!feof(fp))
	{
		/* ���� �о���� */
		memset(szData, 0x00, 1024);
		fgets(szData, 1024, fp);
		/* ���ڿ� ���̰� Ư�� ���� ������ ������� (���� ù ���ڰ� �ּ��� �����ݷ�(;)�̸� Skip) */
		if (strlen(szData) > 0 && ';' != szData[0] && '\n' != szData[0])
		{
			/* �м� �� ����ϱ� */
			if (!ParseAppend(u32Index, szData, (UINT32)strlen(szData)))
			{
				fclose(fp);
				return FALSE;
			}
			/* ��ϵ� ���� �̻����� ����Ǿ� ������, �� �̻� ����۾� ���� ���� */
			if (++u32Index >= MAX_PH_STEP_LINES/*GetConfig()->ph_step.max_ph_step*/)	break;
		}
	}
	/* ���� �ڵ� �ݱ� */
	fclose(fp);

	return TRUE;
}

/*
 desc : Recipe �м� �� ��� ����
 parm : index	- [in]  �����ǰ� ����� �޸� �ε��� (����ü �ε���. Zero-based)
		data	- [in]  Recipe ������ ����� ���ڿ� ����
		size	- [in]  'data' ���ۿ� ����� �������� ����
 retn : TRUE or FALSE
*/
BOOL CPhStep::ParseAppend(UINT32 index, CHAR *data, UINT32 size)
{
	UINT8 u8Val	= 0x00, u8Div = 21;
	UINT32 i, u32Find = 0;
	CHAR *pData	= data, *pFind, szValue[128];
	LPG_OSSD pstData	= &m_pstShMemRecipe[index];

	/* �ϴ�, �־��� ���ڿ� �߿��� �޸�(',') ������ 9������ Ȯ�� */
	for (i=0; i<size; i++)
	{
		if (',' == data[i])	u32Find++;
	}
	if (u32Find != u8Div)
	{
		AfxMessageBox(L"Failed to analyse the value from <ph_step> file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 00.speed (frame) rate */
	pFind	= strchr(pData, ',');
	memset(szValue, 0x00, 128);
	memcpy(szValue, pData, pFind - pData);
	pData	= ++pFind;
	pstData->frame_rate	= (UINT32)atol(szValue);

	/* Photohead �� X / Y ���� �� (0 ��°�� ���, ������ 0 ���̾�� �� */
	for (i=0; i<UINT32(MAX_PH)*2; i++)
	{
		pFind	= strchr(pData, ',');
		memset(szValue, 0x00, 128);
		memcpy(szValue, pData, pFind - pData);
		pData	= ++pFind;
		/* ���� �� ���� */
		if ((i%2) == 0)	pstData->step_x_nm[i/2] = atol(szValue);
		else			pstData->step_y_nm[i/2] = atoi(szValue);
	}
	for (i=0; i<4; i++)
	{
		pFind	= strchr(pData, ',');
		memset(szValue, 0x00, 128);
		memcpy(szValue, pData, pFind - pData);
		pData	= ++pFind;
		switch (i)
		{
		case 0x00	:	pstData->nega_offset_px	= (INT32)atoi(szValue);		break;
		case 0x01	:	pstData->delay_posi_nsec= (UINT32)atol(szValue);	break;
		case 0x02	:	pstData->delay_nega_nsec= (UINT32)atol(szValue);	break;
		case 0x03	:	pstData->scroll_mode	= (UINT8)atoi(szValue);		break;
		}
	}

	return TRUE;
}

/*
 desc : ������ ���� �˻�
 parm : frame_rate	- [in]  �˻��ϰ��� �ϴ� ���������� ���� �ӵ� ��, �뱤 �ӵ�
 retn : TRUE (Finded) or FALSE (Not find)
*/
BOOL CPhStep::RecipeFind(UINT16 frame_rate)
{
	UINT16 i	= 0;

	for (; i<MAX_PH_STEP_LINES/*GetConfig()->ph_step.max_ph_step*/; i++)
	{
		if (frame_rate == m_pstShMemRecipe[i].frame_rate)	return TRUE;
	}
	return FALSE;
}

/*
 desc : ������ �̸� �˻��� ���� ���� (�޸� ���Ŵ� �ƴ�)
 parm : frame_rate	- [in]  �����ϰ��� �ϴ� ���������� ���� �ӵ� ��, �뱤 �ӵ�
 retn : TRUE (Deleted) or FALSE (Not delete)
*/
BOOL CPhStep::ResetRecipe(UINT16 frame_rate)
{
	UINT16 i	= 0;

	for (; i<MAX_PH_STEP_LINES/*GetConfig()->ph_step.max_ph_step*/; i++)
	{
		if (frame_rate == m_pstShMemRecipe[i].frame_rate)
		{
			m_pstShMemRecipe[i].Reset();
			return TRUE;
		}
	}
	return FALSE;
}

/*
 desc : ������ �̸��� ���� ������ ��ȯ
 parm : frame_rate	- [in]  �˻��ϰ��� �ϴ� ���������� ���� �ӵ� ��, �뱤 �ӵ�
 retn : ��ȯ�� �����ǰ� ����� ����ü ������
*/
LPG_OSSD CPhStep::GetRecipeData(UINT16 frame_rate)
{
	UINT16 i	= 0;

	for (; i<MAX_PH_STEP_LINES/*GetConfig()->ph_step.max_ph_step*/; i++)
	{
		if (frame_rate == m_pstShMemRecipe[i].frame_rate)	return &m_pstShMemRecipe[i];
	}
	return NULL;
}

/*
 desc : ������ �̸��� ���� ������ ����
 parm : frame_rate	- [in]  �˻��ϰ��� �ϴ� ���������� ���� �ӵ� ��, �뱤 �ӵ�
 retn : TRUE (���� ����) or FALSE (���� ����)
*/
BOOL CPhStep::SetRecipeData(UINT16 frame_rate)
{
	UINT16 i	= 0;

	for (; i<MAX_PH_STEP_LINES/*GetConfig()->ph_step.max_ph_step*/; i++)
	{
		if (frame_rate == m_pstShMemRecipe[i].frame_rate)
		{
			m_pstSelRecipe	= &m_pstShMemRecipe[i];
			return TRUE;
		}
	}
	return FALSE;
}

/*
 desc : �����ǰ� ����� �ε��� ��ġ�� �ִ� ������ ��ȯ
 parm : index	- [in]  ���������� �ϴ� ������ �ε��� (Zero-based)
 retn : ��ȯ�� �����ǰ� ����� ����ü ������
*/
LPG_OSSD CPhStep::GetRecipeIndex(UINT32 index)
{
	if (index >= MAX_PH_STEP_LINES/*GetConfig()->ph_step.max_ph_step*/)	return NULL;

	/* ��ġ�� �ش�Ǵ� ��� �� ��ȯ */
	return &m_pstShMemRecipe[index];
}

/*
 desc : Calibration Recipe ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CPhStep::SaveFile()
{
	UINT32 i = 0, j	= 0;
	CHAR szData[256]= {NULL};
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	errno_t eRet	= 0;
	FILE *fp		= NULL;
	CUniToChar csCnv;

	/* Recipe File ���� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\cali\\%s.dat",
			   m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.ph_step);
	/* ���� ���� */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "wt");
	if (0 != eRet)	return TRUE;
	fputs("; frame_rate,step_x2/y2 ~ step_x8/y8(nm),nega_offset(pixel),delay_posi(nsec),delay_nega(nsec),scroll_mode(1~7)\r\n", fp);
	for (; i<MAX_PH_STEP_LINES/*GetConfig()->ph_step.max_ph_step*/; i++)
	{
		if (m_pstShMemRecipe[i].frame_rate > 0)
		{
			/* �� �������� */
			sprintf_s(szData, 256,	"%04u,", m_pstShMemRecipe[i].frame_rate);
			fputs(szData, fp);
			/* ���� : 09 */
			for (j=0; j<MAX_PH; j++)
			{
				sprintf_s(szData, 256, "%u,%d,",
						  m_pstShMemRecipe[i].step_x_nm[j], m_pstShMemRecipe[i].step_y_nm[j]);
				fputs(szData, fp);
			}
			sprintf_s(szData, 256, "%d,%u,%u,%u,",
					  m_pstShMemRecipe[i].nega_offset_px,
					  m_pstShMemRecipe[i].delay_posi_nsec,
					  m_pstShMemRecipe[i].delay_nega_nsec,
					  m_pstShMemRecipe[i].scroll_mode);
			fputs(szData, fp);
			fputs("\n", fp);
		}
	}
	/* ���� �ڵ� �ݱ� */
	fclose(fp);

	/* �ٽ� ���� ���� */
	return LoadFile();
}

/*
 desc : Recipe Append
 parm : step	- [in]  Recipe ������ ����� ����ü ������
 retn : TRUE or FALSE (���� �ʰ�. �ִ� 15��)
*/
BOOL CPhStep::RecipeAppend(LPG_OSSD step)
{
	UINT32 i		= 0;
	BOOL bAppended	= FALSE;
	LPG_OSSD pstData= NULL;

	/* �ʼ��� �ԷµǴ� ���� �ִ��� ���� */
	if (!step->IsValid())
	{
		AfxMessageBox(L"The input data is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ������ ��ϵ� �����ǰ� �ִ��� Ȯ�� */
	if (RecipeFind(step->frame_rate))
	{
		AfxMessageBox(L"There is a speed_rate for the same recipe", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ��� �ִ� ������ �� ���� */
	for (; i<MAX_PH_STEP_LINES/*GetConfig()->ph_step.max_ph_step*/; i++)
	{
		/* ��� �ִ� ������ �� ����*/
		if (m_pstShMemRecipe[i].frame_rate == 0)
		{
			/* ���� ������ ����ü ������ ���� ������ �� ���� */
			memcpy(&m_pstShMemRecipe[i], step, sizeof(STG_OSSD));
			bAppended	= TRUE;
			break;
		}
	}

	/* ������� ��ϵ� ��� Recipe ������ ���Ϸ� ���� */
	return SaveFile();
}

/*
 desc : Recipe Delete
 parm : step	- [in]  Recipe ������ ����� ����ü ������
 retn : TRUE or FALSE
*/
BOOL CPhStep::RecipeModify(LPG_OSSD step)
{
	LPG_OSSD pstData	= NULL;

	/* �ʼ��� �ԷµǴ� ���� �ִ��� ���� */
	if (!step->IsValid())
	{
		AfxMessageBox(L"The update data is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ������ ��ϵ� �����ǰ� �ִ��� Ȯ�� */
	pstData = GetRecipeData(step->frame_rate);
	if (!pstData)
	{
		AfxMessageBox(L"The target speed_rate was not found", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ���� ������ ����ü ������ ���� ������ �� ���� */
	memcpy(step, pstData, sizeof(STG_OSSD));

	/* ������� ��ϵ� ��� Recipe ������ ���Ϸ� ���� */
	return SaveFile();
}

/*
 desc : Recipe Delete
 parm : frame_rate	- [in]  �����Ϸ��� ���������� �̵� �ӵ� ��, �뱤 �ӵ�
 retn : TRUE or FALSE
*/
BOOL CPhStep::RecipeDelete(UINT16 frame_rate)
{
	/* ������ ��ϵ� �����ǰ� �ִ��� Ȯ�� */
	if (!GetRecipeData(frame_rate))
	{
		AfxMessageBox(L"The deleted speed_rate was not found", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* Recipe ���� ���� */
	ResetRecipe(frame_rate);

	/* ������� ��ϵ� ��� Recipe ������ ���Ϸ� ���� */
	return SaveFile();
}

/*
 desc : ���� ��ϵ� ������ (������� ����) ���� ��ȯ
 parm : None
 retn : ��ϵ� ���� ��ȯ
*/
UINT32 CPhStep::GetRecipeCount()
{
	UINT32 i	= 0, u32Count = 0;

	for (; i<MAX_PH_STEP_LINES/*GetConfig()->ph_step.max_ph_step*/; i++)
	{
		if (m_pstShMemRecipe[i].frame_rate != 0)	u32Count++;
	}
	return u32Count;
}

/*
 desc : �����Ǹ� ���������� �ϴ� ���� ��ġ �ʱ�ȭ
 parm : None
 retn : �����ǰ� ����� ó�� ��ġ �� (Zero-based) ��ȯ
		���� ���̸�, �� �̻� ��ϵ� �����ǰ� ����
*/
INT32 CPhStep::GetRecipeStart()
{
	UINT32 i	= 0;
	for (; i<MAX_PH_STEP_LINES/*GetConfig()->ph_step.max_ph_step*/; i++)
	{
		if (0 != m_pstShMemRecipe[i].frame_rate)
		{
			m_u32RecipeNext	= i;	/* ���� �ֱٿ� ��ȸ�� ������ ��ġ ���� */
			return i;
		}
	}

	return -1;
}

/*
 desc : ���� ��ġ���� ������ ����� �������� ���� ��ġ ��ȯ
 parm : None
 retn : ���� �˻��� �����ǰ� ����� ��ġ �� (Zero-based) ��ȯ
		���� ���̸�, �� �̻� ��ϵ� �����ǰ� ����
*/
INT32 CPhStep::GetRecipeNext()
{
	UINT32 i	= m_u32RecipeNext + 1;
	for (; i<MAX_PH_STEP_LINES/*GetConfig()->ph_step.max_ph_step*/; i++)
	{
		m_u32RecipeNext	= i;	/* ���� �ֱٿ� ��ȸ�� ������ ��ġ ���� */
		if (0 != m_pstShMemRecipe[i].frame_rate)	return i;
	}

	return -1;
}