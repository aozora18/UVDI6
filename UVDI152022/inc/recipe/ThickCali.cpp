
/*
 desc : ���� �β��� ���� ����� ī�޶� 2���� Mark 2�� �Ź� ��ǥ�� ���� ��� ���� ��ġ ����
*/

#include "pch.h"
#include "ThickCali.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : work_dir- [in]  ���� ������ ����Ǵ� ��� (��ü ���, '\' ����)
 retn : None
*/
CThickCali::CThickCali(PTCHAR work_dir)
	: CBase(work_dir)
{
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CThickCali::~CThickCali()
{
	RemoveAll();
}

/*
 desc : ���� ��ϵ� ������ ��� ����
 parm : None
 retn : None
*/
VOID CThickCali::RemoveAll()
{
	POSITION pPos	= NULL;
	LPG_MACP pRecipe= NULL;

	/* ���� ��ϵ� Recipe Data �޸� ���� */
	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		pRecipe	= m_lstRecipe.GetNext(pPos);
		if (pRecipe)	::Free(pRecipe);
	}
	/* ����Ʈ ��� */
	m_lstRecipe.RemoveAll();
}

/*
 desc : Calibration Recipe ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CThickCali::LoadFile()
{
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	CHAR szData[1024]	= {NULL};
	errno_t eRet		= 0;
	FILE *fp			= NULL;
	CUniToChar csCnv;

	/* ���� ��ϵ� ������ ���� */
	RemoveAll();

	/* Recipe File ���� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\cali\\%s.dat",
			   m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.thick_cali);
	/* ���� ���� */
	
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "rt");
	if (0 != eRet)	return TRUE;
	while (!feof(fp))
	{
		
		/* ���� �о���� */
		memset(szData, 0x00, 1024);
		fgets(szData, 1024, fp);
		/* ���ڿ� ���̰� Ư�� ���� ������ ������� (���� ù ���ڰ� �ּ��� �����ݷ�(;)�̸� Skip) */
		if (strlen(szData) == 0 || ';' == szData[0]) 
			continue;

		ParseAppend(szData, (UINT32)strlen(szData));
		
	}

	/* ���� �ڵ� �ݱ� */
	fclose(fp);

	return TRUE;
}

/*
 desc : Recipe �м� �� ��� ����
 parm : data	- [in]  Recipe ������ ����� ���ڿ� ����
		size	- [in]  'data' ���ۿ� ����� �������� ����
 retn : TRUE or FALSE
*/
BOOL CThickCali::ParseAppend(CHAR *data, UINT32 size)
{
	UINT8 u8Div			= 5;
	INT32 i32Value		= 0;
	UINT32 i, u32Find	= 0;
	CHAR *pData	= data, *pFind, szValue[64];
	LPG_MACP pstRecipe	= {NULL};

	/* �ϴ�, �־��� ���ڿ� �߿��� �޸�(',') ������ xxx ������ Ȯ�� */
	for (i=0; i<size; i++)
	{
		if (',' == data[i])	u32Find++;
	}
	if (u32Find < u8Div)
	{
		AfxMessageBox(L"Failed to analyse the value from <thick_cali> file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ������� �о ����ü�� ���� */
	pstRecipe	= (LPG_MACP)::Alloc(sizeof(STG_MACP));
	ASSERT(pstRecipe);

	for (i=0; i< u32Find;i++)
	{
		pFind	= strchr(pData, ',');
		if (!pFind)
		{
			::Free(pstRecipe);
			return FALSE;
		}

		memset(szValue, 0x00, 64);
		memcpy(szValue, pData, pFind - pData);
		pData	= ++pFind;

		switch (i)
		{
			case 0x00:	pstRecipe->film_thick_um	= (UINT32)atoi(szValue);break;
			case 0x01:	pstRecipe->mark2_stage_y[0]	= atof(szValue);		break;
			case 0x02:	pstRecipe->mark2_stage_y[1]	= atof(szValue);		break;
			case 0x03:	pstRecipe->mark2_acam_x[0]	= atof(szValue);		break;
			case 0x04:	pstRecipe->mark2_acam_x[1]	= atof(szValue);		break;
			case 0x05:	pstRecipe->mark2CentercamOffsetXY[0] = atof(szValue);		break;
			case 0x06:	pstRecipe->mark2CentercamOffsetXY[1] = atof(szValue);		break;
		}
	}

	/* �޸𸮿� �м��� ������ ������ ��� */
	m_lstRecipe.AddTail(pstRecipe);

	return TRUE;
}

/*
 desc : ������ �ӵ� �˻�
 parm : thick	- [in]  �˻��ϰ��� �ϴ� ������ �ε��� (Thickness: um)
 retn : TRUE (Finded) or FALSE (Not find)
*/
BOOL CThickCali::RecipeFind(UINT32 thick)
{
	POSITION pPos		= NULL;
	LPG_MACP pstRecipe	= NULL;

	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		pstRecipe	= m_lstRecipe.GetNext(pPos);
		if (thick == pstRecipe->film_thick_um)	return TRUE;
	}
	return FALSE;
}

/*
 desc : ������ �̸��� ���� ������ ��ȯ
 parm : thick	- [in]  �˻��ϰ��� �ϴ� ������ �ε��� (Thickness: um)
 retn : ��ȯ�� �����ǰ� ����� ����ü ������
*/
LPG_MACP CThickCali::GetRecipe(UINT32 thick)
{
	POSITION pPos		= NULL;
	LPG_MACP pstRecipe	= NULL;

	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		pstRecipe	= m_lstRecipe.GetNext(pPos);
		if (thick == pstRecipe->film_thick_um)	return pstRecipe;
	}
	return NULL;
}

/*
 desc : �����ǰ� ����� �ε��� ��ġ�� �ִ� ������ ��ȯ
 parm : index	- [in]  ���������� �ϴ� ������ �ε��� (Zero-based)
 retn : ��ȯ�� �����ǰ� ����� ����ü ������
*/
LPG_MACP CThickCali::GetRecipeIndex(UINT32 index)
{
	POSITION pPos	= NULL;

	/* �ش� ��ġ �� ��� */
	pPos	= m_lstRecipe.FindIndex(index);
	if (!pPos)	return NULL;

	/* ��ġ�� �ش�Ǵ� ��� �� ��ȯ */
	return m_lstRecipe.GetAt(pPos);
}



//221219 abh1000
/*
 desc : Calibration Recipe ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CThickCali::SaveFile()
{
	UINT32 i = 0, j = 0;
	CHAR szData[256] = { NULL };
	TCHAR tzFile[MAX_PATH_LEN] = { NULL };
	errno_t eRet = 0;
	FILE* fp = NULL;
	CUniToChar csCnv;
	POSITION pPos = NULL;
	LPG_MACP pstRecipe = { NULL };

	/* Recipe File ���� */
// 	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\data\\cali\\%s.dat",
// 		m_tzWorkDir, GetConfig()->file_dat.ph_step);
	/* Recipe File ���� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\cali\\%s.dat",
		m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.thick_cali);
	/* ���� ���� */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "wt");
	if (0 != eRet)	return TRUE;


	fputs("; film_thick (um),mark2_stage_y_1/2 (mm),mark2_acam_x_1/2 (mm),\r\n", fp);

	pPos = m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		/* �� �������� */
		pstRecipe = (LPG_MACP)m_lstRecipe.GetNext(pPos);


		/* material_thick, calibration_thick */
		sprintf_s(szData, 256, "%05u,%.4f,%.4f,%.4f,%.4f,\n",
			pstRecipe->film_thick_um,
			pstRecipe->mark2_stage_y[0], pstRecipe->mark2_stage_y[1],
			pstRecipe->mark2_acam_x[0], pstRecipe->mark2_acam_x[1],
			pstRecipe->mark2CentercamOffsetXY[0], pstRecipe->mark2CentercamOffsetXY[1]);
		fputs(szData, fp);
	}

	fclose(fp);

	/* �ٽ� ���� ���� */
	return LoadFile();
}

/*
 desc : Recipe ����
 parm : step	- [in]  Recipe ������ ����� ����ü ������
 retn : TRUE or FALSE
*/
BOOL CThickCali::RecipeModify(LPG_MACP step)
{
	LPG_MACP pstData = NULL;

	/* �ʼ��� �ԷµǴ� ���� �ִ��� ���� */
	if (!step->IsValid())
	{
		AfxMessageBox(L"The update data is invalid", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}

	/* ������ ��ϵ� �����ǰ� �ִ��� Ȯ�� */
	pstData = GetRecipe(step->film_thick_um);
	if (!pstData)
	{
		AfxMessageBox(L"The target file thick um was not found", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}

	/* ���� ������ ����ü ������ ���� ������ �� ���� */
	memcpy(pstData, step, sizeof(STG_MACP));

	/* ������� ��ϵ� ��� Recipe ������ ���Ϸ� ���� */
	return SaveFile();
}