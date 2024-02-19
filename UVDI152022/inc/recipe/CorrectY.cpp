
/*
 desc : ���а� ���� �ӵ� ��, �������� �̵� �ӵ��� ���� ���а���� ���� �� (������) ����
*/

#include "pch.h"
#include "CorrectY.h"


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
CCorrectY::CCorrectY(PTCHAR work_dir, LPG_OSCY shmem)
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
CCorrectY::~CCorrectY()
{
}

/*
 desc : ���� ��ϵ� ������ ��� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CCorrectY::ResetRecipeAll()
{
	/* ���� ��ϵ� Recipe Data �޸� �ʱ�ȭ */
	memset(m_pstShMemRecipe, 0x00, sizeof(STG_OSCY) * MAX_PH_CORRECT_Y);
	/* ���� ���õ� ������ �ʱ�ȭ */
	m_pstSelRecipe	= NULL;
}

/*
 desc : Calibration Recipe ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCorrectY::LoadFile()
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
			   m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.correct_y);
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
		}
		/* ��ϵ� ���� �̻����� ����Ǿ� ������, �� �̻� ����۾� ���� ���� */
		if (++u32Index >= MAX_PH_CORRECT_Y)	break;
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
BOOL CCorrectY::ParseAppend(UINT32 index, CHAR *data, UINT32 size)
{
	UINT8 u8Val		= 0x00;
	UINT32 u32Find	= 0, i = 0;
	CHAR *pData	= data, *pFind, szValue[128];
	LPG_OSCY pstData	= &m_pstShMemRecipe[index];

	/* �ϴ�, �־��� ���ڿ� �߿��� �޸�(',') ������ 9������ Ȯ�� */
	for (i=0; i<size; i++)
	{
		if (',' == data[i])	u32Find++;
	}

	/* 00.speed (frame) rate */
	pFind	= strchr(pData, ',');
	memset(szValue, 0x00, 128);
	memcpy(szValue, pData, pFind - pData);
	pData	= ++pFind;
	pstData->frame_rate	= (UINT32)atol(szValue);
	/* 01.Total Count */
	pFind	= strchr(pData, ',');
	memset(szValue, 0x00, 128);
	memcpy(szValue, pData, pFind - pData);
	pData	= ++pFind;
	pstData->stripe_count	= (UINT16)atol(szValue);

	if (u32Find != (pstData->stripe_count + 2))
	{
		AfxMessageBox(L"Failed to analyse the value from <correct_y> file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ��ü Stripe�� Y ���� �� */
	for (i=0; i<pstData->stripe_count; i++)
	{
		pFind	= strchr(pData, ',');
		memset(szValue, 0x00, 128);
		memcpy(szValue, pData, pFind - pData);
		pData	= ++pFind;
		/* Stripe ���� �� ���� (����: nm) */
		pstData->adjust[i] = (INT32)ROUNDED(atof(szValue) * 1000.0f*-1.0f, 0);
	}

	return TRUE;
}

/*
 desc : ������ ���� �˻�
 parm : frame_rate	- [in]  �˻��ϰ��� �ϴ� ���������� ���� �ӵ� ��, �뱤 �ӵ�
 retn : TRUE (Finded) or FALSE (Not find)
*/
BOOL CCorrectY::RecipeFind(UINT16 frame_rate)
{
	UINT16 i	= 0;

	for (; i<MAX_PH_CORRECT_Y; i++)
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
BOOL CCorrectY::ResetRecipe(UINT16 frame_rate)
{
	UINT16 i	= 0;

	for (; i<MAX_PH_CORRECT_Y; i++)
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
LPG_OSCY CCorrectY::GetRecipeData(UINT16 frame_rate)
{
	UINT16 i	= 0;

	for (; i<MAX_PH_CORRECT_Y; i++)
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
BOOL CCorrectY::SetRecipeData(UINT16 frame_rate)
{
	UINT16 i	= 0;

	for (; i<MAX_PH_CORRECT_Y; i++)
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
LPG_OSCY CCorrectY::GetRecipeIndex(UINT32 index)
{
	if (index >= MAX_PH_CORRECT_Y)	return NULL;

	/* ��ġ�� �ش�Ǵ� ��� �� ��ȯ */
	return &m_pstShMemRecipe[index];
}

/*
 desc : Calibration Recipe ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCorrectY::SaveFile()
{
	UINT32 i = 0, j	= 0;
	CHAR szData[256]= {NULL};
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	errno_t eRet	= 0;
	FILE *fp		= NULL;
	CUniToChar csCnv;

	/* Recipe File ���� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\cali\\%s.dat",
			   m_tzWorkDir, CUSTOM_DATA_CONFIG,GetConfig()->file_dat.correct_y);
	/* ���� ���� */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "wt");
	if (0 != eRet)	return TRUE;
	fputs("; frame_rate,total,correction_y1 ~ n (max 200. unit:um)\r\n", fp);
	for (; i<MAX_PH_CORRECT_Y; i++)
	{
		if (m_pstShMemRecipe[i].frame_rate > 0)
		{
			/* 00.Frame Rate */
			sprintf_s(szData, 256,	"%04u,", m_pstShMemRecipe[i].frame_rate);
			fputs(szData, fp);
			/* 01.Stripe Count */
			sprintf_s(szData, 256,	"%03u,", m_pstShMemRecipe[i].stripe_count);
			fputs(szData, fp);
			/* 02. ~ ... Stripe Correct Y �� */
			for (j=0; j<m_pstShMemRecipe[i].stripe_count; j++)
			{
				sprintf_s(szData, 256, "%.1f", (m_pstShMemRecipe[i].adjust[j] / 1000.0f) * -1.0f);
				fputs(szData, fp);
			}
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
 parm : adjust	- [in]  Recipe ������ ����� ����ü ������
 retn : TRUE or FALSE (���� �ʰ�. �ִ� 15��)
*/
BOOL CCorrectY::RecipeAppend(LPG_OSCY adjust)
{
	UINT32 i		= 0;
	BOOL bAppended	= FALSE;
	LPG_OSCY pstData= NULL;

	/* �ʼ��� �ԷµǴ� ���� �ִ��� ���� */
	if (!adjust->IsValid())
	{
		AfxMessageBox(L"The input data is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ������ ��ϵ� �����ǰ� �ִ��� Ȯ�� */
	if (RecipeFind(adjust->frame_rate))
	{
		AfxMessageBox(L"There is a speed_rate for the same recipe", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ��� �ִ� ������ �� ���� */
	for (; i<MAX_PH_CORRECT_Y; i++)
	{
		/* ��� �ִ� ������ �� ����*/
		if (m_pstShMemRecipe[i].frame_rate == 0)
		{
			/* ���� ������ ����ü ������ ���� ������ �� ���� */
			memcpy(&m_pstShMemRecipe[i], adjust, sizeof(STG_OSCY));
			bAppended	= TRUE;
			break;
		}
	}

	/* ������� ��ϵ� ��� Recipe ������ ���Ϸ� ���� */
	return SaveFile();
}

/*
 desc : Recipe Delete
 parm : adjust	- [in]  Recipe ������ ����� ����ü ������
 retn : TRUE or FALSE
*/
BOOL CCorrectY::RecipeModify(LPG_OSCY adjust)
{
	LPG_OSCY pstData	= NULL;

	/* �ʼ��� �ԷµǴ� ���� �ִ��� ���� */
	if (!adjust->IsValid())
	{
		AfxMessageBox(L"The update data is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ������ ��ϵ� �����ǰ� �ִ��� Ȯ�� */
	pstData = GetRecipeData(adjust->frame_rate);
	if (!pstData)
	{
		AfxMessageBox(L"The target speed_rate was not found", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ���� ������ ����ü ������ ���� ������ �� ���� */
	memcpy(adjust, pstData, sizeof(STG_OSCY));

	/* ������� ��ϵ� ��� Recipe ������ ���Ϸ� ���� */
	return SaveFile();
}

/*
 desc : Recipe Delete
 parm : frame_rate	- [in]  �����Ϸ��� ���������� �̵� �ӵ� ��, �뱤 �ӵ�
 retn : TRUE or FALSE
*/
BOOL CCorrectY::RecipeDelete(UINT16 frame_rate)
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
UINT32 CCorrectY::GetRecipeCount()
{
	UINT32 i	= 0, u32Count = 0;

	for (; i<MAX_PH_CORRECT_Y; i++)
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
INT32 CCorrectY::GetRecipeStart()
{
	UINT32 i	= 0;
	for (; i<MAX_PH_CORRECT_Y; i++)
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
INT32 CCorrectY::GetRecipeNext()
{
	UINT32 i	= m_u32RecipeNext + 1;
	for (; i<MAX_PH_CORRECT_Y; i++)
	{
		m_u32RecipeNext	= i;	/* ���� �ֱٿ� ��ȸ�� ������ ��ġ ���� */
		if (0 != m_pstShMemRecipe[i].frame_rate)	return i;
	}

	return -1;
}