
/*
 desc : Mark Model�� Recipe ���� �� ����
*/

#include "pch.h"
#include "MarkUVDI15.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : work	- [in]  �۾� �⺻ ��� (Full path)
 retn : None
*/
CMarkUVDI15::CMarkUVDI15(PTCHAR work_dir)
	: CBase(work_dir)
{
	m_pstSelected	= NULL;
	m_pstROI = (LPG_CRD)::Alloc(sizeof(STG_CRD));
	for (int i = 0; i < 2; i++) {
		m_pstROI->roi_Left[i] = 5;
		m_pstROI->roi_Right[i] = ACA1920_SIZE_X - 5;
		m_pstROI->roi_Top[i] = 5;
		m_pstROI->roi_Bottom[i] = ACA1920_SIZE_Y - 5;
	}
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CMarkUVDI15::~CMarkUVDI15()
{
	RemoveModelAll();
	RemoveAlignRecipeAll();
}

/*
 desc : ���� ��ϵ� �� ��� ����
 parm : None
 retn : None
*/
VOID CMarkUVDI15::RemoveModelAll()
{
	POSITION pPos	= NULL;
	LPG_CMPV pstData= NULL;

	/* ���� ��ϵ� Model Data �޸� ���� */
	pPos	= m_lstModel.GetHeadPosition();
	while (pPos)
	{
		pstData	= m_lstModel.GetNext(pPos);
		if (pstData)	::Free(pstData);
	}
	/* ����Ʈ ��� */
	m_lstModel.RemoveAll();
}


/*
 desc : ���� ��ϵ� ������ ��� ����
 parm : None
 retn : None
*/
VOID CMarkUVDI15::RemoveAlignRecipeAll()
{
	POSITION pPos	= NULL;
	LPG_RAAF pstData= NULL;

	/* ���� ��ϵ� Model Data �޸� ���� */
	pPos	= m_lstAlignRecipe.GetHeadPosition();
	while (pPos)
	{
		pstData	= m_lstAlignRecipe.GetNext(pPos);
		if (pstData)
		{
			if (pstData->m_name)	pstData->Close();
			::Free(pstData);
		}
	}
	/* ����Ʈ ��� */
	m_lstAlignRecipe.RemoveAll();
}

/*
 desc : Mark Model �� Recipe ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMarkUVDI15::LoadFile()
{
	CHAR szData[1024]	= {NULL};
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	errno_t eRet	= 0;
	FILE *fp		= NULL;
	CUniToChar csCnv;

	/* ���� ��ϵ� Model ���� */
	RemoveModelAll();
	RemoveAlignRecipeAll();

	/* ----------------------------------------------------------------------------------------- */
	/*                                     Model File ����                                       */
	/* ----------------------------------------------------------------------------------------- */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\recipe\\%s.dat",
			   m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.mark_model);
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
			ParseModel(szData, (UINT32)strlen(szData));
		}
	}
	/* ���� �ڵ� �ݱ� */
	fclose(fp);

	/* ----------------------------------------------------------------------------------------- */
	/*                                   Align Recipe File ����                                  */
	/* ----------------------------------------------------------------------------------------- */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\recipe\\%s.dat",
			   m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.align_recipe);
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
			ParseAlignRecipe(szData, (UINT32)strlen(szData));
		}
	}
	/* ���� �ڵ� �ݱ� */
	fclose(fp);

/* ----------------------------------------------------------------------------------------- */
/*                                    ROI File ����                                       	 */
/* ----------------------------------------------------------------------------------------- */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\recipe\\%s.dat",
		m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.mark_roi);
	/* ���� ���� */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "rt");
	if (0 != eRet) {
		SaveROI();
		eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "rt");
	}
	int cnt = 0;
	while (!feof(fp))
	{
		/* ���� �о���� */
		memset(szData, 0x00, 1024);
		fgets(szData, 1024, fp);
		/* ���ڿ� ���̰� Ư�� ���� ������ ������� (���� ù ���ڰ� �ּ��� �����ݷ�(;)�̸� Skip) */
		if (strlen(szData) > 0 && ';' != szData[0] && '\n' != szData[0])
		{
			/* �м� �� ����ϱ� */
			ParseSearchROI(szData, (UINT32)strlen(szData), cnt);
			cnt++;
		}
	}
	/* ���� �ڵ� �ݱ� */
	fclose(fp);

	return TRUE;
}

/*
 desc : Model �м� �� ��� ����
 parm : data	- [in]  Model ������ ����� ���ڿ� ����
		size	- [in]  'data' ���ۿ� ����� �������� ����
 retn : TRUE or FALSE
*/
BOOL CMarkUVDI15::ParseModel(PCHAR data, UINT32 size)
{
	UINT32 u32Find	= 0, i = 0;
	CHAR *pData		= data, *pFind, szValue[256];
	LPG_CMPV pstData= {NULL};

	/* �ϴ�, �־��� ���ڿ� �߿��� �޸�(',') ������ 9������ Ȯ�� */
	for (; i<size; i++)
	{
		if (',' == data[i])	u32Find++;
	}
	if (u32Find != 7)
	{
		AfxMessageBox(L"Failed to analyse the value from model <mark_model> file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ������� �о ����ü�� ���� */
	pstData	= (LPG_CMPV)::Alloc(sizeof(STG_CMPV));
	ASSERT(pstData);
	/* �޸� �Ҵ� �� �ʱ�ȭ */
	pstData->Reset();
	/* 00.model name */
	pFind	= strchr(pData, ',');
	if (pFind)
	{
		memcpy(pstData->name, pData, pFind - pData);
		pData = ++pFind;
	}
	/* 01.model type */
	memset(szValue, 0x00, 256);
	pFind	= strchr(pData, ',');
	if (pFind)
	{
		memcpy(szValue, pData, pFind - pData);
		pData = ++pFind;
		pstData->type	= (UINT32)atoi(szValue);
	}

	/* 01. find type */
	//memset(szValue, 0x00, 256);
	//pFind = strchr(pData, ',');
	//if (pFind)
	//{
	//	memcpy(szValue, pData, pFind - pData);
	//	pData = ++pFind;
	//	pstData->find_type = (UINT32)atoi(szValue);
	//}

	/* �� ������ MMF ������ �ƴ� ��� */
	//if (pstData->find_type == 0 && ENG_MMDT(pstData->type) != ENG_MMDT::en_image)
	if (ENG_MMDT(pstData->type) != ENG_MMDT::en_image)
	{
		/* 02.param - 1 */
		memset(szValue, 0x00, 256);
		pFind	= strchr(pData, ',');
		if (pFind)
		{
			memcpy(szValue, pData, pFind - pData);
			pData = ++pFind;
			pstData->param[0]	= (DOUBLE)atof(szValue);
		}
		/* 03.param - 2 */
		memset(szValue, 0x00, 256);
		pFind	= strchr(pData, ',');
		if (pFind)
		{
			memcpy(szValue, pData, pFind - pData);
			pData = ++pFind;
			pstData->param[1]	= (DOUBLE)atof(szValue);
		}
		/* 04.param - 3 */
		memset(szValue, 0x00, 256);
		pFind	= strchr(pData, ',');
		if (pFind)
		{
			memcpy(szValue, pData, pFind - pData);
			pData = ++pFind;
			pstData->param[2]	= (DOUBLE)atof(szValue);
		}
		/* 05.param - 4 */
		memset(szValue, 0x00, 256);
		pFind	= strchr(pData, ',');
		if (pFind)
		{
			memcpy(szValue, pData, pFind - pData);
			pData = ++pFind;
			pstData->param[3]	= (DOUBLE)atof(szValue);
		}
		/* 06.param - 5 */
		memset(szValue, 0x00, 256);
		pFind	= strchr(pData, ',');
		if (pFind)
		{
			memcpy(szValue, pData, pFind - pData);
			pData = ++pFind;
			pstData->param[4]	= (DOUBLE)atof(szValue);
		}
	}
	else
	{
		/* 02.mmf,pat file name (Ȯ���� ����) */
		memset(szValue, 0x00, 256);
		pFind	= strchr(pData, ',');
		if (pFind)
		{
			memcpy(pstData->file, pData, pFind - pData);
			pData = ++pFind;
		}
		/* 03.Mark Size x */
		memset(szValue, 0x00, 256);
		pFind = strchr(pData, ',');
		if (pFind)
		{
			memcpy(szValue, pData, pFind - pData);
			pData = ++pFind;
			pstData->iSizeP.x = (UINT32)atof(szValue);
		}
		/* 04.Mark Size y */
		memset(szValue, 0x00, 256);
		pFind = strchr(pData, ',');
		if (pFind)
		{
			memcpy(szValue, pData, pFind - pData);
			pData = ++pFind;
			pstData->iSizeP.y = (UINT32)atof(szValue);
		}
		/* 05.Mark Center x */
		memset(szValue, 0x00, 256);
		pFind = strchr(pData, ',');
		if (pFind)
		{
			memcpy(szValue, pData, pFind - pData);
			pData = ++pFind;
			pstData->iOffsetP.x = (UINT32)atof(szValue);
		}
		/* 06.Mark Center y */
		memset(szValue, 0x00, 256);
		pFind = strchr(pData, ',');
		if (pFind)
		{
			memcpy(szValue, pData, pFind - pData);
			pData = ++pFind;
			pstData->iOffsetP.y = (UINT32)atof(szValue);
		}
	}

	/* �޸𸮿� �м��� Model ������ ��� */
	m_lstModel.AddTail(pstData);

	return TRUE;
}

/*
 desc : Recipe �м� �� ��� ����
 parm : data	- [in]  Model ������ ����� ���ڿ� ����
		size	- [in]  'data' ���ۿ� ����� �������� ����
 retn : TRUE or FALSE
*/
BOOL CMarkUVDI15::ParseAlignRecipe(PCHAR data, UINT32 size)
{
	UINT8 u8MarkType	= 0x00;
	UINT32 u32Find		= 0, i = 0;
	CHAR *pData			= data, *pFind, szValue[32], szName[MARK_MODEL_NAME_LENGTH] = {NULL};
	STG_RAAF stTempRecipe;
	LPG_RAAF pstRecipe = { NULL };

	if (NULL == data)
	{
		return FALSE;
	}

	/* �ϴ�, �־��� ���ڿ� �߿��� �޸�(',') ������ 9������ Ȯ�� */
	for (; i<size; i++)
	{
		if (',' == data[i])	u32Find++;
	}
	if (u32Find < 4)
	{
		AfxMessageBox(L"Failed to analyse the value from model <mark_recipe> file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ������� �о ����ü�� ���� */
	stTempRecipe.Init(2);
	pstRecipe	= (LPG_RAAF)::Alloc(sizeof(STG_RAAF));
	ASSERT(pstRecipe);
	
	/* 00.recipe name */
	pFind	= strchr(pData, ',');
	if (pFind)
	{
		memcpy(stTempRecipe.align_name, pData, pFind - pData);
		pData = ++pFind;
	}

	/* flip series*/
	for (i = 0; i < 10; i++)
	{
		pFind = strchr(pData, ',');
		memset(szValue, 0x00, _countof(szValue));
		memcpy(szValue, pData, pFind - pData);
		switch (i)
		{
		case 0x00:	stTempRecipe.save_count		= (UINT8)atoi(szValue);	break;
		case 0x01:	stTempRecipe.mark_type		= (UINT8)atoi(szValue);	break;
		case 0x02:	stTempRecipe.align_type		= (UINT8)atoi(szValue);	break;
		case 0x03:	stTempRecipe.lamp_type		= (UINT8)atoi(szValue);	break;
		case 0x04:	stTempRecipe.gain_level[0]	= (UINT8)atoi(szValue);	break;
		case 0x05:	stTempRecipe.gain_level[1]	= (UINT8)atoi(szValue);	break;
		case 0x06:	stTempRecipe.search_type	= (UINT8)atoi(szValue);	break;
		case 0x07:	stTempRecipe.search_count	= (UINT8)atoi(szValue);	break;
		case 0x08:	stTempRecipe.mark_area[0]	= (UINT32)atoi(szValue);	break;
		case 0x09:	stTempRecipe.mark_area[1]	= (UINT32)atoi(szValue);	break;
		}
		pData = ++pFind;
	}

	/* �� ���� �о���̱� */
	if (stTempRecipe.save_count)
	{
		CopyAlignRecipe(&stTempRecipe, pstRecipe);

		//pstRecipe->type	= u8MarkType;
		/* ������ �̸��� ������ ��ü �޸� �Ҵ� */
		for (i=0; i<UINT16(pstRecipe->save_count); i++)
		{
			/* align camera number (1 or 2) */
			memset(szValue, 0x00, 32);
			pFind	= strchr(pData, ',');
			if (pFind)
			{
				memcpy(szValue, pData, pFind - pData);
				pData = ++pFind;
				pstRecipe->acam_num[i]	= (UINT8)atoi(szValue);
			}

			/* Model Name */
			pFind	= strchr(pData, ',');
			/* ��ũ �� �̸� ������ ���� ���� �Ҵ� �� �ʱ�ȭ  */
			memcpy(pstRecipe->m_name[i], pData, pFind - pData);
			pData = ++pFind;
		}
	}

	/* �޸𸮿� �м��� Model ������ ��� */
	m_lstAlignRecipe.AddTail(pstRecipe);
	stTempRecipe.Close();

	return TRUE;
}

/*
 desc : Model �̸� �˻�
 parm : m_name	- [in]  �˻��ϰ��� �ϴ� Model �̸�
 retn : TRUE (Finded) or FALSE (Not find)
*/
BOOL CMarkUVDI15::ModelFind(PCHAR m_name)
{
	POSITION pPos	= NULL;

	pPos	= m_lstModel.GetHeadPosition();
	while (pPos)
	{
		if (0 == strcmp(m_name, m_lstModel.GetNext(pPos)->name))	return TRUE;
	}
	return FALSE;
}

/*
 desc : Recipe �̸� �˻�
 parm : r_name	- [in]  �˻��ϰ��� �ϴ� Recipe �̸�
 retn : TRUE (Finded) or FALSE (Not find)
*/
BOOL CMarkUVDI15::AlignRecipeFind(PCHAR r_name)
{
	POSITION pPos	= NULL;
	pPos	= m_lstAlignRecipe.GetHeadPosition();
	while (pPos)
	{
		if (0 == strcmp(r_name, m_lstAlignRecipe.GetNext(pPos)->align_name))	return TRUE;
	}
	return FALSE;
}

/*
 desc : Model �̸� �˻��� ���� ����
 parm : m_name	- [in]  �����ϰ��� �ϴ� Model �̸�
 retn : TRUE (Deleted) or FALSE (Not delete)
*/
BOOL CMarkUVDI15::RemoveModel(PCHAR m_name)
{
	POSITION pPos	= NULL, pPrePos;
	LPG_CMPV pstData= NULL;

	pPos	= m_lstModel.GetHeadPosition();
	while (pPos)
	{
		/* ��� ���� ã�� ��ġ ���� */
		pPrePos	= pPos;
		pstData	= m_lstModel.GetNext(pPos);
		if (pstData && 0 == strcmp(m_name, pstData->name))
		{
			::Free(pstData);
			m_lstModel.RemoveAt(pPrePos);
			return TRUE;
		}
	}
	return FALSE;
}

/*
 desc : Recipe �̸� �˻��� ���� ����
 parm : r_name	- [in]  �����ϰ��� �ϴ� Recipe �̸�
 retn : TRUE (Deleted) or FALSE (Not delete)
*/
BOOL CMarkUVDI15::RemoveAlignRecipe(PCHAR r_name)
{
	POSITION pPos	= NULL, pPrePos;
	LPG_RAAF pstData= NULL;

	/* ���� ���� ����� ������ �̸��� ���õ� ������ �̸� ������ Ȯ�� */
	if (m_pstSelected && 0 == strcmp(r_name, m_pstSelected->align_name))
	{
		m_pstSelected	= NULL;	/* ���õ� ������ ���� ó�� */
	}

	pPos	= m_lstAlignRecipe.GetHeadPosition();
	while (pPos)
	{
		/* ��� ���� ã�� ��ġ ���� */
		pPrePos	= pPos;
		pstData	= m_lstAlignRecipe.GetNext(pPos);
		if (0 == strcmp(r_name, pstData->align_name))
		{
			if (pstData->m_name)
			{
				::Free(pstData->m_name[0]);
				::Free(pstData->m_name);
			}
			::Free(pstData);
			m_lstAlignRecipe.RemoveAt(pPrePos);
			return TRUE;
		}
	}
	return FALSE;
}

/*
 desc : Model �̸��� ���� Model ��ȯ
 parm : m_name	- [in]  �˻��ϰ��� �ϴ� Model �̸�
 retn : ��ȯ�� Model�� ����� ����ü ������
*/
LPG_CMPV CMarkUVDI15::GetModelName(PCHAR m_name)
{
	POSITION pPos	= NULL;
	LPG_CMPV pstData= NULL;

	pPos	= m_lstModel.GetHeadPosition();
	while (pPos)
	{
		pstData	= m_lstModel.GetNext(pPos);
		if (0 == strcmp(m_name, pstData->name))	return pstData;
	}
	return NULL;
}

/*
 desc : Model �̸��� ���� Model ��ȯ
 parm : r_name	- [in]  �˻��ϰ��� �ϴ� Recipe �̸�
 retn : ��ȯ�� Model�� ����� ����ü ������
*/
LPG_RAAF CMarkUVDI15::GetAlignRecipeName(PCHAR r_name)
{
	POSITION pPos	= NULL;
	LPG_RAAF pstData= NULL;

	pPos	= m_lstAlignRecipe.GetHeadPosition();
	while (pPos)
	{
		pstData	= m_lstAlignRecipe.GetNext(pPos);
		if (0 == strcmp(r_name, pstData->align_name))	return pstData;
	}
	return NULL;
}

/*
 desc : Model�� ����� �ε��� ��ġ�� �ִ� Model ��ȯ
 parm : index	- [in]  ���������� �ϴ� Model �ε��� (Zero-based)
 retn : ��ȯ�� Model�� ����� ����ü ������
*/
LPG_CMPV CMarkUVDI15::GetModelIndex(UINT8 index)
{
	POSITION pPos	= NULL;

	if (index >= m_lstModel.GetCount())	return NULL;

	/* �ش� ��ġ �� ��� */
	pPos	= m_lstModel.FindIndex(index);
	if (!pPos)	return NULL;

	/* ��ġ�� �ش�Ǵ� ��� �� ��ȯ */
	return m_lstModel.GetAt(pPos);
}

/*
 desc : Recipe�� ����� �ε��� ��ġ�� �ִ� Model ��ȯ
 parm : index	- [in]  ���������� �ϴ� Recipe �ε��� (Zero-based)
 retn : ��ȯ�� Model�� ����� ����ü ������
*/
LPG_RAAF CMarkUVDI15::GetAlignRecipeIndex(UINT8 index)
{
	POSITION pPos	= NULL;

	if (index >= m_lstAlignRecipe.GetCount())	return NULL;

	/* �ش� ��ġ �� ��� */
	pPos	= m_lstAlignRecipe.FindIndex(index);
	if (!pPos)	return NULL;

	/* ��ġ�� �ش�Ǵ� ��� �� ��ȯ */
	return m_lstAlignRecipe.GetAt(pPos);
}

/*
 desc : Model�� ����� �ε��� ��ġ�� �ִ� Model ��ȯ
 parm : index	- [in]  ���������� �ϴ� Model �ε��� (Zero-based)
 retn : �� �̸� ��ȯ
*/
PCHAR CMarkUVDI15::GetModelName(UINT8 index)
{
	POSITION pPos	= NULL;

	if (index >= m_lstModel.GetCount())	return NULL;

	/* �ش� ��ġ �� ��� */
	pPos	= m_lstModel.FindIndex(index);
	if (!pPos)	return NULL;

	/* ��ġ�� �ش�Ǵ� ��� �� ��ȯ */
	return m_lstModel.GetAt(pPos)->name;
}

/*
 desc : Recipe�� ����� �ε��� ��ġ�� �ִ� Model ��ȯ
 parm : index	- [in]  ���������� �ϴ� Model �ε��� (Zero-based)
 retn : �� �̸� ��ȯ
*/
PCHAR CMarkUVDI15::GetAlignRecipeName(UINT8 index)
{
	POSITION pPos	= NULL;

	if (index >= m_lstAlignRecipe.GetCount())	return NULL;

	/* �ش� ��ġ �� ��� */
	pPos	= m_lstAlignRecipe.FindIndex(index);
	if (!pPos)	return NULL;

	/* ��ġ�� �ش�Ǵ� ��� �� ��ȯ */
	return m_lstAlignRecipe.GetAt(pPos)->align_name;
}

/*
 desc : Mark Model & Recipe ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMarkUVDI15::SaveFile()
{
	return SaveModel() && SaveAlignRecipe();
}

/*
 desc : Model Save
 parm : None
 retn : TRUE or FALSE
*/
// BOOL CMark::SaveModel()
// {
// 	CHAR szData[256]	= {NULL};
// 	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
// 	errno_t eRet		= 0;
// 	FILE *fp			= NULL;
// 	POSITION pPos		= NULL;
// 	LPG_CMPV pstModel	= NULL;
// 	CUniToChar csCnv;
// 
// 	/* Model File ���� */
// 	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\data\\recipe\\%s.dat",
// 			   m_tzWorkDir, GetConfig()->file_dat.mark_model);
// 	/* ���� ���� */
// 	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "wt");
// 	if (0 != eRet)	return TRUE;
// 
// 	/* �ּ� �Է� */
// 	fputs("; Model Name, Model Type, Param 1, Param 2, Param 3, Param 4, Param 5,\n", fp);
// 	
// 	pPos	= m_lstModel.GetHeadPosition();
// 	while (pPos)
// 	{
// 		/* �� �������� */
// 		pstModel	= m_lstModel.GetNext(pPos);
// 		/* ���� : 01 ~ 07 */
// 		if (ENG_MMDT(pstModel->type) == ENG_MMDT::en_image)
// 		{
// 			sprintf_s(szData, 256,	"%s,%u,%s,0.0000,0.0000,0.0000,0.0000,",
// 						pstModel->name, pstModel->type, pstModel->file);
// 		}
// 		else
// 		{
// 			sprintf_s(szData, 256,	"%s,%u,%.1f,%.4f,%.4f,%.4f,%.4f,",
// 						pstModel->name,
// 						pstModel->type,
// 						pstModel->param[0],
// 						pstModel->param[1],
// 						pstModel->param[2],
// 						pstModel->param[3],
// 						pstModel->param[4]);
// 		}
// 		fputs(szData, fp);
// 		fputs("\n", fp);
// 	}
// 	/* ���� �ڵ� �ݱ� */
// 	fclose(fp);
// 
// 	return TRUE;
// }

/*
 desc : Recipe Save
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMarkUVDI15::SaveAlignRecipe()
{
	CHAR szData[256]	= {NULL};
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	UINT16 i;
	errno_t eRet		= 0;
	FILE *fp			= NULL;
	POSITION pPos		= NULL;
	LPG_RAAF pstRecipe	= NULL;
	CUniToChar csCnv;

	/* Model File ���� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\recipe\\%s.dat",
			   m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.align_recipe);
	/* ���� ���� */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "wt");
	if (0 != eRet)	return TRUE;
	
	/* �ּ� �Է� */
	//fputs("; Recipe Name,Mark Type (0:Geomatrix,1:Pattern Image),Mark Count,Align Camera Number (1 or 2),Mark Model Name (Pattern File),,,\n", fp);
	fputs(	";Recipe Name, Save Count, Mark Type (0:Geomatrix,1:Pattern Image), Align Type, Lamp Type, Gain Level[2]," 
			"Search Type, Search Count, mark_area_w, Align Camera Number (1 or 2),Mark Model Name (Pattern File),,,\n", fp);

	pPos	= m_lstAlignRecipe.GetHeadPosition();
	while (pPos)
	{
		/* �� �������� */
		pstRecipe	= m_lstAlignRecipe.GetNext(pPos);
		/* ������ ���� */
		if (strlen(pstRecipe->align_name) > 0 && pstRecipe->save_count > 0)
		{
			/* ������ �̸� ���� */
			//fputs(pstRecipe->align_name, fp);
			//fputs(",", fp);
			sprintf_s(szData, 256, "%s,", pstRecipe->align_name);
			fputs(szData, fp);
			/* ��ϵ� �� ���� ���� */
			sprintf_s(szData, 256, "%u,", pstRecipe->save_count);
			fputs(szData, fp);
			/* ��ũ ���� (Type) ���� */
			sprintf_s(szData, 256, "%u,", pstRecipe->mark_type);
			fputs(szData, fp);

			/* Align Mark Array(Global, Local) Type */
			sprintf_s(szData, 256, "%u,", pstRecipe->align_type);
			fputs(szData, fp);
			/* ���� ���� Ÿ��(ring, coaxial) */
			sprintf_s(szData, 256, "%u,", pstRecipe->lamp_type);
			fputs(szData, fp);
			/* Align Camera�� ���� Gain Level �� (0 ~ 255) */
			sprintf_s(szData, 256, "%u,", pstRecipe->gain_level[0]);
			fputs(szData, fp);
			/* Align Camera�� ���� Gain Level �� (0 ~ 255) */
			sprintf_s(szData, 256, "%u,", pstRecipe->gain_level[1]);
			fputs(szData, fp);

			/* Mark �˻� ���(0~5) single, cent_side, multi_only, ring_cirecle, ph_step [mark_model] */
			sprintf_s(szData, 256, "%u,", pstRecipe->search_type);
			fputs(szData, fp);
			/* grab�� ��ũ �˻� �� [mark_count] */
			sprintf_s(szData, 256, "%u,", pstRecipe->search_count);
			fputs(szData, fp);
			/* ����� ��ũ (Mixed Type�� ���)�� �����ϰ� �ִ� ����, ���� ���� ũ��(um) */
			sprintf_s(szData, 256, "%u,", pstRecipe->mark_area[0]);
			fputs(szData, fp);
			/* ����� ��ũ (Mixed Type�� ���)�� �����ϰ� �ִ� ����, ���� ���� ũ��(um) */
			sprintf_s(szData, 256, "%u,", pstRecipe->mark_area[1]);
			fputs(szData, fp);

			/* �� ���� ���� */
			for (i=0; i<pstRecipe->save_count; i++)
			{
				sprintf_s(szData, 256,	"%u,", pstRecipe->acam_num[i]);	fputs(szData, fp);
				sprintf_s(szData, 256,	"%s,", pstRecipe->m_name[i]);	fputs(szData, fp);
			}
			fputs("\n", fp);
		}
	}
	/* ���� �ڵ� �ݱ� */
	fclose(fp);

	return TRUE;
}

BOOL CMarkUVDI15::FindAlignRecipe(PCHAR align_name)
{
	POSITION pPos = NULL;
	pPos = m_lstAlignRecipe.GetHeadPosition();
	while (pPos)
	{
		if (0 == strcmp(align_name, m_lstAlignRecipe.GetNext(pPos)->align_name))	return TRUE;
	}
	return FALSE;
}

/*
 desc : Recipe ���� ����ü �� ���� (����ü ���ο� �Ҵ�� �޸𸮰� �׸��� �����Ƿ�)
 parm : source	- [in]  ���� ���� ����� ��
		target	- [in]  ���ο� ���� ����(����)�� ��
 retn : None
*/
VOID CMarkUVDI15::CopyAlignRecipe(LPG_RAAF source, LPG_RAAF target)
{
	UINT8 i	= 0x00;

	/* �޸� ���� */
	target->Init(source->save_count);

	/* ��� ���� �� ���� */
	target->save_count		= source->save_count;
	target->mark_type		= source->mark_type;
	target->align_type		= source->align_type;
	target->lamp_type		= source->lamp_type;
	target->gain_level[0]	= source->gain_level[0];
	target->gain_level[1]	= source->gain_level[1];
	target->search_type		= source->search_type;
	target->search_count	= source->search_count;
	target->mark_area[0]	= source->mark_area[0];
	target->mark_area[1]	= source->mark_area[1];


	memcpy(target->align_name, source->align_name, strlen(source->align_name));
	memcpy(target->acam_num, source->acam_num, source->save_count);
	for (; i<source->save_count; i++)
	{
		memcpy(target->m_name[i], source->m_name[i], strlen(source->m_name[i]));
	}
}

/*
 desc : Model Append
 parm : data	- [in]  Model ������ ����� ����ü ������
 retn : TRUE or FALSE (���� �ʰ�. �ִ� 15��)
*/
BOOL CMarkUVDI15::ModelAppend(LPG_CMPV value)
{
	LPG_CMPV pstValue	= NULL;

	if (m_lstModel.GetCount() >= MAX_REGIST_MODEL)
	{
		AfxMessageBox(L"The registered Model is full (MAX. 30)", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* �ʼ��� �ԷµǴ� ���� �ִ��� ���� */
	if (!value->IsValid())
	{
		AfxMessageBox(L"The input data is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ������ ��ϵ� Model�� �ִ��� Ȯ�� */
	if (ModelFind(value->name))
	{
		AfxMessageBox(L"There is a name for the same model", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* �� ������ ��ġ�� ��� */
	pstValue	= (LPG_CMPV)::Alloc(sizeof(STG_CMPV));
	ASSERT(pstValue);
	pstValue->Reset();
	memset(pstValue->name, 0x00, MARK_MODEL_NAME_LENGTH);
	/* ����ü ���� �޸� �Ҵ�� �׸��� ������ ���� ���� */
	memcpy(pstValue, value, sizeof(STG_CMPV));
	/* ��� */
	m_lstModel.AddTail(pstValue);

	/* ������� ��ϵ� ��� Model ������ ���Ϸ� ���� */
	return SaveModel();
}

/*
 desc : Recipe Append
 parm : recipe	- [in]  Model ������ ����� ����ü ������
 retn : TRUE or FALSE (���� �ʰ�. �ִ� 15��)
*/
BOOL CMarkUVDI15::AlignRecipeAppend(LPG_RAAF recipe)
{
	LPG_RAAF pstRecipe	= NULL;

	if (m_lstAlignRecipe.GetCount() >= MAX_REGIST_RECIPE_COUNT)
	{
		AfxMessageBox(L"The registered Recipe is full", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* �ʼ��� �ԷµǴ� ���� �ִ��� ���� */
	if (!recipe->IsValid())
	{
		AfxMessageBox(L"The input data is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ������ ��ϵ� Model�� �ִ��� Ȯ�� */
	if (FindAlignRecipe(recipe->align_name))
	{
		AfxMessageBox(L"There is a name for the same recipe", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* �� ������ ��ġ�� ��� */
	pstRecipe	= (LPG_RAAF)::Alloc(sizeof(STG_RAAF));
	ASSERT(pstRecipe);
	pstRecipe->Init(recipe->save_count);
	if (!pstRecipe)	return FALSE;
	/* ���� ������ ����ü ������ ���� ������ �� ���� */
	CopyAlignRecipe(recipe, pstRecipe);
	/* ��� */
	m_lstAlignRecipe.AddTail(pstRecipe);

	/* ������� ��ϵ� ��� Model ������ ���Ϸ� ���� */
	return SaveAlignRecipe();
}

/*
 desc : Model Modify
 parm : value	- [in]  Model ������ ����� ����ü ������
 retn : TRUE or FALSE
*/
BOOL CMarkUVDI15::ModelModify(LPG_CMPV value)
{
	LPG_CMPV pstValue	= NULL;

	/* �ʼ��� �ԷµǴ� ���� �ִ��� ���� */
	if (!value->IsValid())
	{
		AfxMessageBox(L"The update data is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* Mark Recipe�� ���� ����� Model�� ��ϵǾ� �ִ��� Ȯ�� */
	if (FindModelInMark(value->name))
	{
		AfxMessageBox(L"The model name exists inside the mark file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ������ ��ϵ� Model�� �ִ��� Ȯ�� */
	pstValue = GetModelName(value->name);
	if (!pstValue)
	{
		AfxMessageBox(L"The model name was not found in the model file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ����ü ���� �޸� �Ҵ�� �׸��� ������ ���� ���� */
	memcpy(pstValue, value, sizeof(STG_CMPV));

	/* ������� ��ϵ� ��� Model ������ ���Ϸ� ���� */
	return SaveModel();
}

/*
 desc : Model Modify
 parm : recipe	- [in]  Model ������ ����� ����ü ������
 retn : TRUE or FALSE
*/
BOOL CMarkUVDI15::AlignRecipeModify(LPG_RAAF recipe)
{
	LPG_RAAF pstRecipe	= NULL;

	/* �ʼ��� �ԷµǴ� ���� �ִ��� ���� */
	if (!recipe->IsValid())
	{
		AfxMessageBox(L"The update data is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ���� ���� ����� ���� ���õ� �����ǿ� �������� ���� */
	if (m_pstSelected && 0 == strcmp(recipe->align_name, m_pstSelected->align_name))
	{
		//AfxMessageBox(L"The currently selected recipe cannot be deleted", MB_ICONSTOP|MB_TOPMOST);
		//return FALSE;
	}

	/* ������ ��ϵ� Model�� �ִ��� Ȯ�� */
	pstRecipe = GetAlignRecipeName(recipe->align_name);
	if (!pstRecipe)
	{
		AfxMessageBox(L"The recipe name was not found in the mark file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ���� ������ ����ü ������ ���� ������ �� ���� */
	CopyAlignRecipe(recipe, pstRecipe);

	/* ������� ��ϵ� ��� Model ������ ���Ϸ� ���� */
	return SaveAlignRecipe();
}

/*
 desc : Model Modify
 parm : m_name	- [in]  �����Ϸ��� Model�� �̸�
 retn : TRUE or FALSE
*/
BOOL CMarkUVDI15::ModelDelete(PCHAR m_name)
{
	/* ������ ��ϵ� Model�� �ִ��� Ȯ�� */
	if (!GetModelName(m_name))
	{
		AfxMessageBox(L"The model name was not found in the model file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* Mark Recipe�� ���� ����� Model�� ��ϵǾ� �ִ��� Ȯ�� */
	if (FindModelInMark(m_name))
	{
		AfxMessageBox(L"The model name exists inside the mark file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* Model ���� ���� */
	RemoveModel(m_name);

	/* ������� ��ϵ� ��� Model ������ ���Ϸ� ���� */
	return SaveModel();
}

/*
 desc : Model Modify
 parm : r_name	- [in]  �����Ϸ��� Recipe�� �̸�
 retn : TRUE or FALSE
*/
BOOL CMarkUVDI15::AlignRecipeDelete(PCHAR r_name)
{
	/* ���� ����� ���� ���õ� �����ǿ� �������� ���� */
	if (m_pstSelected && 0 == strcmp(r_name, m_pstSelected->align_name))
	{
		AfxMessageBox(L"The currently selected recipe cannot be deleted", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* ������ ��ϵ� Model�� �ִ��� Ȯ�� */
	if (!GetAlignRecipeName(r_name))
	{
		AfxMessageBox(L"The recipe name was not found in the mark file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* Model ���� ���� */
	RemoveAlignRecipe(r_name);

	/* ������� ��ϵ� ��� Model ������ ���Ϸ� ���� */
	return SaveAlignRecipe();
}

/*
 desc : �� Ÿ�Կ� ���� ���ڿ� ��ȯ
 parm : type	- [in]  �� Ÿ�� �ڵ� ��
 retn : ���ڿ� ��ȯ
*/
PTCHAR CMarkUVDI15::GetModelTypeToStr(UINT32 type)
{
	switch (type)
	{
	case ENG_MMDT::en_none		:	return L"None";
	case ENG_MMDT::en_circle	:	return L"Circle";
	case ENG_MMDT::en_ellipse	:	return L"Ellipse";
	case ENG_MMDT::en_square	:	return L"Square";
	case ENG_MMDT::en_rectangle	:	return L"Rect";
	case ENG_MMDT::en_ring		:	return L"Ring";
	case ENG_MMDT::en_cross		:	return L"Cross";
	case ENG_MMDT::en_diamond	:	return L"Diamond";
	case ENG_MMDT::en_triangle	:	return L"Triangle";
	case ENG_MMDT::en_image		:	return L"Image";
	}
	return L"";
}

/*
 desc : ������ �̸��� ���� ������ ����
 parm : r_name	- [in]  Mark ������ �̸�
 retn : TRUE or FALSE
*/
BOOL CMarkUVDI15::SelAlignRecipeName(PCHAR r_name)
{
	LPG_RAAF pstRecipe	= GetAlignRecipeName(r_name);
	if (!pstRecipe)	return FALSE;

	/* ������ ���� ���� */
	m_pstSelected	= pstRecipe;

	return TRUE;
}

/*
 desc : Mark Recipe ���� �˻� ����� Model Name�� �����ϴ��� ����
 parm : m_name	- [in]  ã�����ϴ� Model Name
 retn : TRUE or FALSE
*/
BOOL CMarkUVDI15::FindModelInMark(PCHAR m_name)
{
	UINT8 i, j, u8Count	= (UINT8)m_lstAlignRecipe.GetCount();
	LPG_RAAF pstRecipe	= NULL;

	for (i=0; i<u8Count; i++)
	{
		pstRecipe	= GetAlignRecipeIndex(i);
		for (j=0; j<pstRecipe->save_count; j++)
		{
			if (0 == strcmp(pstRecipe->m_name[j], m_name))	return TRUE;
		}
	}

	return FALSE;
}
/* ----------------------------------------------------------------------------------------- */
/*                                 lk91 VISION �߰� �Լ�                                     */
/* ----------------------------------------------------------------------------------------- */

/* desc : Mark ���� file Save */
BOOL CMarkUVDI15::SaveModel()
{
	CHAR szData[256] = { NULL };
	TCHAR tzFile[MAX_PATH_LEN] = { NULL };
	errno_t eRet = 0;
	FILE* fp = NULL;
	POSITION pPos = NULL;
	LPG_CMPV pstModel = NULL;
	CUniToChar csCnv;

	/* Model File ���� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\recipe\\%s.dat",
		m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.mark_model);
	/* ���� ���� */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "wt");
	if (0 != eRet)	return TRUE;

	/* �ּ� �Է� */
	//fputs("; Model Name, Model Type, Find Type, File Name OR Param 1, Param 2, Param 3, Param 4, Param 5,\n", fp);
	fputs("; Model Name, Model Type, File Name OR Param 1, Param 2, Param 3, Param 4, Param 5,\n", fp);

	pPos = m_lstModel.GetHeadPosition();
	while (pPos)
	{
		/* �� �������� */
		pstModel = m_lstModel.GetNext(pPos);
		/* ���� : 01 ~ 07 */
		//if (pstModel->find_type == 1 || ENG_MMDT(pstModel->type) == ENG_MMDT::en_image)
		if (ENG_MMDT(pstModel->type) == ENG_MMDT::en_image)
		{
			sprintf_s(szData, 256, "%s,%u,%s,%d,%d,%d,%d,",
				pstModel->name, pstModel->type, pstModel->file,
				pstModel->iSizeP.x, pstModel->iSizeP.y, pstModel->iOffsetP.x, pstModel->iOffsetP.y);
				//pstModel->name, pstModel->type, pstModel->find_type, pstModel->file,
				//pstModel->iSizeP.x, pstModel->iSizeP.y, pstModel->iOffsetP.x, pstModel->iOffsetP.y);
		}
		else
		{
			//sprintf_s(szData, 256, "%s,%u,%u,%.1f,%.4f,%.4f,%.4f,%.4f,",
			sprintf_s(szData, 256, "%s,%u,%.1f,%.4f,%.4f,%.4f,%.4f,",
				pstModel->name,
				pstModel->type,
				//pstModel->find_type,
				pstModel->param[0],
				pstModel->param[1],
				pstModel->param[2],
				pstModel->param[3],
				pstModel->param[4]);
		}
		fputs(szData, fp);
		fputs("\n", fp);
	}
	/* ���� �ڵ� �ݱ� */
	fclose(fp);

	return TRUE;
}

/* desc : Set Search ROI */
BOOL CMarkUVDI15::SetMarkROI(LPG_CRD data, UINT8 index)
{
	m_pstROI->roi_Left[index] = data->roi_Left[index];
	m_pstROI->roi_Right[index] = data->roi_Right[index];
	m_pstROI->roi_Top[index] = data->roi_Top[index];
	m_pstROI->roi_Bottom[index] = data->roi_Bottom[index];

	return TRUE;
}

/* desc : Get Search ROI */
LPG_CRD CMarkUVDI15::GetMarkROI()
{
	return m_pstROI;
}

/* desc : Search ROI Save */
BOOL CMarkUVDI15::SaveROI()
{
	CHAR szData[256] = { NULL };
	TCHAR tzFile[MAX_PATH_LEN] = { NULL };
	errno_t eRet = 0;
	FILE* fp = NULL;
	POSITION pPos = NULL;
	CUniToChar csCnv;

	/* Model File ���� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\recipe\\%s.dat",
		m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.mark_roi);
	/* ���� ���� */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "wt");
	if (0 != eRet)	return TRUE;

	/* �ּ� �Է� */
	fputs("; ROI_LEFT, ROI_RIGHT, ROI_TOP, ROI_BOTTOM,,,\n", fp);

	for (int i = 0; i < 2; i++) {
		sprintf_s(szData, 256, "%d,%d,%d,%d,\n",
			m_pstROI->roi_Left[i],
			m_pstROI->roi_Right[i],
			m_pstROI->roi_Top[i],
			m_pstROI->roi_Bottom[i]);

		fputs(szData, fp);
	}

	/* ���� �ڵ� �ݱ� */
	fclose(fp);

	return TRUE;
}


/* desc : Search ROI ��� ���� */
BOOL CMarkUVDI15::ParseSearchROI(PCHAR data, UINT32 size, UINT8 cnt)
{
	UINT8 u8MarkType = 0x00;
	UINT32 u32Find = 0, i = 0;
	CHAR* pData = data, * pFind, szValue[32], szName[MARK_MODEL_NAME_LENGTH] = { NULL };

	for (; i < size; i++)
	{
		if (',' == data[i])	u32Find++;
	}
	if (u32Find < 4)
	{
		AfxMessageBox(L"Failed to analyse the value from model <search_roi> file", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}

	if (!m_pstROI)
		m_pstROI = (LPG_CRD)::Alloc(sizeof(STG_CRD));
	ASSERT(m_pstROI);

	memset(szValue, 0x00, 32);
	pFind = strchr(pData, ',');
	if (pFind)
	{
		memcpy(szValue, pData, pFind - pData);
		pData = ++pFind;
		m_pstROI->roi_Left[cnt] = (INT32)atoi(szValue);
	}
	memset(szValue, 0x00, 32);
	pFind = strchr(pData, ',');
	if (pFind)
	{
		memcpy(szValue, pData, pFind - pData);
		pData = ++pFind;
		m_pstROI->roi_Right[cnt] = (INT32)atoi(szValue);
	}
	memset(szValue, 0x00, 32);
	pFind = strchr(pData, ',');
	if (pFind)
	{
		memcpy(szValue, pData, pFind - pData);
		pData = ++pFind;
		m_pstROI->roi_Top[cnt] = (INT32)atoi(szValue);
	}
	memset(szValue, 0x00, 32);
	pFind = strchr(pData, ',');
	if (pFind)
	{
		memcpy(szValue, pData, pFind - pData);
		pData = ++pFind;
		m_pstROI->roi_Bottom[cnt] = (INT32)atoi(szValue);
	}

	/* �޸𸮿� �м��� Model ������ ��� */
	return TRUE;
}


/* ----------------------------------------------------------------------------------------- */
/*                                 Align_type			                                     */
/* ----------------------------------------------------------------------------------------- */


/*
 desc : ���� ���õ� Recipe�� Mark ���� ����� Shared Type���� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMarkUVDI15::IsRecipeSharedType()
{
	if (!m_pstSelected)	return FALSE;
	switch (m_pstSelected->align_type)
	{
	case ENG_ATGL::en_global_4_local_0x0_n_point:
	case ENG_ATGL::en_global_3_local_0x0_n_point:
	case ENG_ATGL::en_global_2_local_0x0_n_point:

	case ENG_ATGL::en_global_4_local_2x2_n_point:
	case ENG_ATGL::en_global_4_local_3x2_n_point:
	case ENG_ATGL::en_global_4_local_4x2_n_point:
	case ENG_ATGL::en_global_4_local_5x2_n_point:
		return FALSE;
	}
	return TRUE;
}

/*
 desc : ���� ���õ� Recipe�� Mark ���� ����� Local Mark�� �����ϴ��� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMarkUVDI15::IsExistLocalMark()
{
	if (!m_pstSelected)	return FALSE;
	switch (m_pstSelected->align_type)
	{
	case ENG_ATGL::en_global_4_local_0x0_n_point:
	case ENG_ATGL::en_global_4_local_2x2_n_point:
	case ENG_ATGL::en_global_4_local_3x2_n_point:
	case ENG_ATGL::en_global_4_local_4x2_n_point:
	case ENG_ATGL::en_global_4_local_5x2_n_point:
	case ENG_ATGL::en_global_4_local_2x2_s_point:
	case ENG_ATGL::en_global_4_local_3x2_s_point:
	case ENG_ATGL::en_global_4_local_4x2_s_point:
	case ENG_ATGL::en_global_4_local_5x2_s_point:
		return TRUE;
	}
	return FALSE;
}

/*
 desc : ���� ���õ� �����ǿ� ���Ե� ��ü ��ũ ����
 parm : None
 retn : ���� ��ȯ
*/
UINT8 CMarkUVDI15::GetSelectRecipeLocalMarkCount()
{
	if (!m_pstSelected)	return 0x00;
	switch (m_pstSelected->align_type)
	{
	case ENG_ATGL::en_global_4_local_0x0_n_point:
	case ENG_ATGL::en_global_3_local_0x0_n_point:
	case ENG_ATGL::en_global_2_local_0x0_n_point:	return 0;

	case ENG_ATGL::en_global_4_local_2x2_n_point:	return 16;
	case ENG_ATGL::en_global_4_local_3x2_n_point:	return 24;
	case ENG_ATGL::en_global_4_local_4x2_n_point:	return 32;
	case ENG_ATGL::en_global_4_local_5x2_n_point:	return 40;

	case ENG_ATGL::en_global_4_local_2x2_s_point:	return 9;
	case ENG_ATGL::en_global_4_local_3x2_s_point:	return 12;
	case ENG_ATGL::en_global_4_local_4x2_s_point:	return 15;
	case ENG_ATGL::en_global_4_local_5x2_s_point:	return 18;
	}
	return 0x00;
}

/*
 desc : ���������� ��ġ�� Mark Index ���� Camera �� ���̰� �� ��° �̹��� �������� ��ȯ
 parm : mark	- [in]  Mark Index (Zero based)
		cam_id	- [out] Camera Number (1 or 2)
		img_id	- [out] Camera Grabbed Image Number (Zero based)
 retn : TRUE or FALSE
*/
BOOL CMarkUVDI15::GetLocalMarkToGrabNum(UINT8 mark, UINT8& cam_id, UINT8& img_id)
{
	UINT8 u8Count = GetSelectRecipeLocalMarkCount();
	if (!m_pstSelected || u8Count < 1)	return FALSE;

	/* �� �ʱ�ȭ (�ݵ�� Max ������ �ʱ�ȭ) */
	cam_id = 0xff;
	img_id = 0xff;

	UINT8 u8Coun4 = u8Count / 4;

	UINT8 matrix2_2[16] = { 5, 4, 3, 2,
							6, 7, 8, 9,
							5, 4, 3, 2,
							6, 7, 8, 9 };

	UINT8 matrix3_2[24] = { 7,6,5,4,
							3,2,8,9,
							10,11,12,13,
							7,6,5,4,
							3,2,8,9,
							10,11,12,13 };

	UINT8 matrix4_2[32] = { 9, 8, 7, 6,
							5, 4, 3, 2,
							10, 11, 12, 13,
							14, 15, 16, 17,
							9, 8, 7, 6,
							5, 4, 3, 2,
							10, 11, 12, 13,
							14, 15, 16, 17 };

	UINT8 matrix5_2[40] = { 11,10,9,8,
							7,6,5,4,
							3,2,12,13,
							14,15,16,17,
							18,19,20,21,
							11,10,9,8,
							7,6,5,4,
							3,2,12,13,
							14,15,16,17,
							18,19,20,21 };


	cam_id = (mark < (u8Count / 2)) ? 0x01 : 0x02;;

	switch (m_pstSelected->align_type)
	{
		//abh1000 0417
	case ENG_ATGL::en_global_4_local_2x2_n_point:
		img_id = matrix2_2[mark];	break;
	case ENG_ATGL::en_global_4_local_3x2_n_point:
		img_id = matrix3_2[mark];	break;
	case ENG_ATGL::en_global_4_local_4x2_n_point:
		img_id = matrix4_2[mark];	break;
	case ENG_ATGL::en_global_4_local_5x2_n_point:
		img_id = matrix5_2[mark];	break;

	case ENG_ATGL::en_global_4_local_2x2_s_point:
	case ENG_ATGL::en_global_4_local_3x2_s_point:
	case ENG_ATGL::en_global_4_local_4x2_s_point:
	case ENG_ATGL::en_global_4_local_5x2_s_point:
		if (mark < (u8Count / 3))
		{
			cam_id = 0x01;
			img_id = mark + 2 /* Global Mark Number 0, 1�� �ֱ� ������ */;
		}
		else
		{
			cam_id = 0x02;
			img_id = (mark - ((m_pstSelected->align_type & 0x0f) + 2)) + 2 /* Global Mark Number 0, 1�� �ֱ� ������ */;
		}
		break;

	default:	return FALSE;
	}

	return TRUE;
}

/* ����
 desc : ���� Local Mark Index �� ���� �ش�� Scan ��ȣ �� ��ȯ
 parm : mark_id	- [in]  Local Mark Index �� (0 or Later)
 retn : Scan Number (Zero Based)
*/
UINT8 CMarkUVDI15::GetLocalMarkToScanNum(UINT8 mark_id)
{
	UINT8 u8Mark = 0;
	if (!m_pstSelected || mark_id < 4)	return 0;

	switch (m_pstSelected->align_type)
	{
	case ENG_ATGL::en_global_4_local_2x2_n_point:
	case ENG_ATGL::en_global_4_local_3x2_n_point:
	case ENG_ATGL::en_global_4_local_4x2_n_point:
	case ENG_ATGL::en_global_4_local_5x2_n_point:
		u8Mark = ((m_pstSelected->align_type & 0x0f) + 1) * 4;
		break;

	case ENG_ATGL::en_global_4_local_2x2_s_point:
	case ENG_ATGL::en_global_4_local_3x2_s_point:
	case ENG_ATGL::en_global_4_local_4x2_s_point:
	case ENG_ATGL::en_global_4_local_5x2_s_point:
		u8Mark = ((m_pstSelected->align_type & 0x0f) + 1) * 3;
		break;
	}

	return (u8Mark > 0 && u8Mark <= mark_id) ? 0x01 : 0x00;
}

/* ����
 desc : ���� Camera Index�� Grabbed Image�� ������ �� ��° �ε����� ����Ǿ� �ִ��� ��ȯ
 parm : cam_id	- [in]  Camera Number (1 or 2)
		img_id	- [in]  Camera Grabbed Image Number (Zero based)
		mark	- [out] Mark Index (Zero based) ��ȯ
 retn : TRUE or FALSE
*/
BOOL CMarkUVDI15::GetGrabNumToLocalMark(UINT8 cam_id, UINT8 img_id, UINT8& mark)
{
	UINT8 u8Count = GetSelectRecipeLocalMarkCount();

	if (!m_pstSelected || u8Count < 1)	return FALSE;
	switch (m_pstSelected->align_type)
	{
	case ENG_ATGL::en_global_4_local_2x2_n_point:
	case ENG_ATGL::en_global_4_local_3x2_n_point:
	case ENG_ATGL::en_global_4_local_4x2_n_point:
	case ENG_ATGL::en_global_4_local_5x2_n_point:
		mark = 4 * ((m_pstSelected->align_type & 0x0f) + 1) * (cam_id - 1) + img_id;
		break;
	case ENG_ATGL::en_global_4_local_2x2_s_point:
	case ENG_ATGL::en_global_4_local_3x2_s_point:
	case ENG_ATGL::en_global_4_local_4x2_s_point:
	case ENG_ATGL::en_global_4_local_5x2_s_point:
#if 0
		mark = 3 * ((m_pstSelected->align_type & 0x0f) + 1) * (cam_id - 1) + img_id;
#else
		if (0x01 == cam_id)	mark = img_id;
		else				mark = (m_pstSelected->align_type & 0x0f) + 2 + img_id;
#endif
		break;
	default:	return FALSE;
	}
	return TRUE;
}

/* ����
 desc : ���� ī�޶󸶴� Grabbed Image ��ȣ�� �ش�Ǵ� ���� (�������� �̵� ����) ���� ��ȯ
 parm : img_id	- [in]  Camera Grabbed Image Number (Zero based)
 retn : TRUE (Normal : ������; ����) or FALSE (������; ����)
*/
BOOL CMarkUVDI15::GetImageToStageDirect(UINT8 img_id)
{
	if (!m_pstSelected || img_id < 2)	return TRUE;	/* Global Mark�� ��� ������ ������ */
	switch (m_pstSelected->align_type)
	{
		/* Normal Type */
	case ENG_ATGL::en_global_4_local_2x2_n_point:	if (5 >= img_id)	return FALSE;	break;
	case ENG_ATGL::en_global_4_local_3x2_n_point:	if (7 >= img_id)	return FALSE;	break;
	case ENG_ATGL::en_global_4_local_4x2_n_point:	if (9 >= img_id)	return FALSE;	break;
	case ENG_ATGL::en_global_4_local_5x2_n_point:	if (11 >= img_id)	return FALSE;	break;
		/* Shared Type */
	case ENG_ATGL::en_global_4_local_2x2_s_point:	if (4 >= img_id)	return FALSE;	break;
	case ENG_ATGL::en_global_4_local_3x2_s_point:	if (5 >= img_id)	return FALSE;	break;
	case ENG_ATGL::en_global_4_local_4x2_s_point:	if (6 >= img_id)	return FALSE;	break;
	case ENG_ATGL::en_global_4_local_5x2_s_point:	if (7 >= img_id)	return FALSE;	break;
	}

	return TRUE;
}

/*
 desc : ����� ��ũ �˻��� ���� 1 Scan �ϰ� �Ǹ�, �˻��Ǵ� ��ũ�� ���� ��,
		1 Scan �� ��ϵ� ��ũ�� ���� (���� ���õ� ������(�Ź�)�� ����)
 parm : type	- [in]  ��ũ ���� ����
 retn : 1 Scan �� ��ϵ� ��ũ�� ���� ��ȯ �ִ� 10 �� (����. 0x00)
*/
UINT8 CMarkUVDI15::GetScanLocalMarkCount()
{
	switch (m_pstSelected->align_type)
	{
		/* Normal Type */
	case ENG_ATGL::en_global_4_local_2x2_n_point:	return 0x04;
	case ENG_ATGL::en_global_4_local_3x2_n_point:	return 0x06;
	case ENG_ATGL::en_global_4_local_4x2_n_point:	return 0x08;
	case ENG_ATGL::en_global_4_local_5x2_n_point:	return 0x0a;
		/* Shared Type */
	case ENG_ATGL::en_global_4_local_2x2_s_point:	return 0x03;
	case ENG_ATGL::en_global_4_local_3x2_s_point:	return 0x04;
	case ENG_ATGL::en_global_4_local_4x2_s_point:	return 0x05;
	case ENG_ATGL::en_global_4_local_5x2_s_point:	return 0x06;
	}

	return 0x00;
}