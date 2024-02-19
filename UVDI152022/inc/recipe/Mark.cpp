
/*
 desc : Mark Model�� Recipe ���� �� ����
*/

#include "pch.h"
#include "Mark.h"


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
CMark::CMark(PTCHAR work_dir)
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
CMark::~CMark()
{
	RemoveModelAll();
	RemoveMarkAll();
}

/*
 desc : ���� ��ϵ� �� ��� ����
 parm : None
 retn : None
*/
VOID CMark::RemoveModelAll()
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
VOID CMark::RemoveMarkAll()
{
	POSITION pPos	= NULL;
	LPG_CMRD pstData= NULL;

	/* ���� ��ϵ� Model Data �޸� ���� */
	pPos	= m_lstMark.GetHeadPosition();
	while (pPos)
	{
		pstData	= m_lstMark.GetNext(pPos);
		if (pstData)
		{
			if (pstData->m_name)	pstData->Close();
			::Free(pstData);
		}
	}
	/* ����Ʈ ��� */
	m_lstMark.RemoveAll();
}

/*
 desc : Mark Model �� Recipe ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMark::LoadFile()
{
	CHAR szData[1024]	= {NULL};
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	errno_t eRet	= 0;
	FILE *fp		= NULL;
	CUniToChar csCnv;

	/* ���� ��ϵ� Model ���� */
	RemoveModelAll();
	RemoveMarkAll();

	/* ----------------------------------------------------------------------------------------- */
	/*                                     Model File ����                                       */
	/* ----------------------------------------------------------------------------------------- */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\data\\recipe\\%s.dat",
			   m_tzWorkDir, GetConfig()->file_dat.mark_model);
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
	/*                                    Recipe File ����                                       */
	/* ----------------------------------------------------------------------------------------- */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\data\\recipe\\%s.dat",
			   m_tzWorkDir, GetConfig()->file_dat.mark_recipe);
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
			ParseMark(szData, (UINT32)strlen(szData));
		}
	}
	/* ���� �ڵ� �ݱ� */
	fclose(fp);

/* ----------------------------------------------------------------------------------------- */
/*                                    ROI File ����                                       	 */
/* ----------------------------------------------------------------------------------------- */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\data\\recipe\\%s.dat",
		m_tzWorkDir, GetConfig()->file_dat.mark_roi);
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
BOOL CMark::ParseModel(PCHAR data, UINT32 size)
{
	UINT32 u32Find	= 0, i = 0;
	CHAR *pData		= data, *pFind, szValue[256];
	LPG_CMPV pstData= {NULL};

	/* �ϴ�, �־��� ���ڿ� �߿��� �޸�(',') ������ 9������ Ȯ�� */
	for (; i<size; i++)
	{
		if (',' == data[i])	u32Find++;
	}
	if (u32Find != 8)
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
	memset(szValue, 0x00, 256);
	pFind = strchr(pData, ',');
	if (pFind)
	{
		memcpy(szValue, pData, pFind - pData);
		pData = ++pFind;
		pstData->find_type = (UINT32)atoi(szValue);
	}

	/* �� ������ MMF ������ �ƴ� ��� */
	if (pstData->find_type == 0 && ENG_MMDT(pstData->type) != ENG_MMDT::en_image)
	//if (ENG_MMDT(pstData->type) != ENG_MMDT::en_image)
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
BOOL CMark::ParseMark(PCHAR data, UINT32 size)
{
	UINT8 u8MarkType	= 0x00;
	UINT32 u32Find		= 0, i = 0;
	CHAR *pData			= data, *pFind, szValue[32], szName[MARK_MODEL_NAME_LENGTH] = {NULL};
	LPG_CMRD pstRecipe	= {NULL};

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
	pstRecipe	= (LPG_CMRD)::Alloc(sizeof(STG_CMRD));
	ASSERT(pstRecipe);
	/* 00.recipe name */
	pFind	= strchr(pData, ',');
	if (pFind)
	{
		memcpy(szName, pData, pFind - pData);
		pData = ++pFind;
	}
	/* 01.mark type */
	memset(szValue, 0x00, 32);
	pFind	= strchr(pData, ',');
	if (pFind)
	{
		memcpy(szValue, pData, pFind - pData);
		pData = ++pFind;
		u8MarkType	= (UINT8)atoi(szValue);
	}
	/* 02.mark count */
	memset(szValue, 0x00, 32);
	pFind	= strchr(pData, ',');
	if (pFind)
	{
		memcpy(szValue, pData, pFind - pData);
		pData = ++pFind;
		pstRecipe->count= (UINT8)atoi(szValue);
	}
	/* �� ���� �о���̱� */
	if (pstRecipe->count)
	{
		/* ������ ������ �� ���� �Ҵ�� Align Camera Index �� ������ ���� ���� �Ҵ� */
		pstRecipe->Init(pstRecipe->count);
		strcpy_s(pstRecipe->r_name, MARK_MODEL_NAME_LENGTH, szName);
		pstRecipe->type	= u8MarkType;
		/* ������ �̸��� ������ ��ü �޸� �Ҵ� */
		for (i=0; i<UINT16(pstRecipe->count); i++)
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
	m_lstMark.AddTail(pstRecipe);

	return TRUE;
}

/*
 desc : Model �̸� �˻�
 parm : m_name	- [in]  �˻��ϰ��� �ϴ� Model �̸�
 retn : TRUE (Finded) or FALSE (Not find)
*/
BOOL CMark::ModelFind(PCHAR m_name)
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
BOOL CMark::MarkFind(PCHAR r_name)
{
	POSITION pPos	= NULL;
	pPos	= m_lstMark.GetHeadPosition();
	while (pPos)
	{
		if (0 == strcmp(r_name, m_lstMark.GetNext(pPos)->r_name))	return TRUE;
	}
	return FALSE;
}

/*
 desc : Model �̸� �˻��� ���� ����
 parm : m_name	- [in]  �����ϰ��� �ϴ� Model �̸�
 retn : TRUE (Deleted) or FALSE (Not delete)
*/
BOOL CMark::RemoveModel(PCHAR m_name)
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
BOOL CMark::RemoveMark(PCHAR r_name)
{
	POSITION pPos	= NULL, pPrePos;
	LPG_CMRD pstData= NULL;

	/* ���� ���� ����� ������ �̸��� ���õ� ������ �̸� ������ Ȯ�� */
	if (m_pstSelected && 0 == strcmp(r_name, m_pstSelected->r_name))
	{
		m_pstSelected	= NULL;	/* ���õ� ������ ���� ó�� */
	}

	pPos	= m_lstMark.GetHeadPosition();
	while (pPos)
	{
		/* ��� ���� ã�� ��ġ ���� */
		pPrePos	= pPos;
		pstData	= m_lstMark.GetNext(pPos);
		if (0 == strcmp(r_name, pstData->r_name))
		{
			if (pstData->m_name)
			{
				::Free(pstData->m_name[0]);
				::Free(pstData->m_name);
			}
			::Free(pstData);
			m_lstMark.RemoveAt(pPrePos);
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
LPG_CMPV CMark::GetModelName(PCHAR m_name)
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
LPG_CMRD CMark::GetRecipeName(PCHAR r_name)
{
	POSITION pPos	= NULL;
	LPG_CMRD pstData= NULL;

	pPos	= m_lstMark.GetHeadPosition();
	while (pPos)
	{
		pstData	= m_lstMark.GetNext(pPos);
		if (0 == strcmp(r_name, pstData->r_name))	return pstData;
	}
	return NULL;
}

/*
 desc : Model�� ����� �ε��� ��ġ�� �ִ� Model ��ȯ
 parm : index	- [in]  ���������� �ϴ� Model �ε��� (Zero-based)
 retn : ��ȯ�� Model�� ����� ����ü ������
*/
LPG_CMPV CMark::GetModelIndex(UINT8 index)
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
LPG_CMRD CMark::GetRecipeIndex(UINT8 index)
{
	POSITION pPos	= NULL;

	if (index >= m_lstMark.GetCount())	return NULL;

	/* �ش� ��ġ �� ��� */
	pPos	= m_lstMark.FindIndex(index);
	if (!pPos)	return NULL;

	/* ��ġ�� �ش�Ǵ� ��� �� ��ȯ */
	return m_lstMark.GetAt(pPos);
}

/*
 desc : Model�� ����� �ε��� ��ġ�� �ִ� Model ��ȯ
 parm : index	- [in]  ���������� �ϴ� Model �ε��� (Zero-based)
 retn : �� �̸� ��ȯ
*/
PCHAR CMark::GetModelName(UINT8 index)
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
PCHAR CMark::GetRecipeName(UINT8 index)
{
	POSITION pPos	= NULL;

	if (index >= m_lstMark.GetCount())	return NULL;

	/* �ش� ��ġ �� ��� */
	pPos	= m_lstMark.FindIndex(index);
	if (!pPos)	return NULL;

	/* ��ġ�� �ش�Ǵ� ��� �� ��ȯ */
	return m_lstMark.GetAt(pPos)->r_name;
}

/*
 desc : Mark Model & Recipe ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMark::SaveFile()
{
	return SaveModel() && SaveRecipe();
}

/*
 desc : Recipe Save
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMark::SaveRecipe()
{
	CHAR szData[256]	= {NULL};
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	UINT16 i;
	errno_t eRet		= 0;
	FILE *fp			= NULL;
	POSITION pPos		= NULL;
	LPG_CMRD pstRecipe	= NULL;
	CUniToChar csCnv;

	/* Model File ���� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\data\\recipe\\%s.dat",
			   m_tzWorkDir, GetConfig()->file_dat.mark_recipe);
	/* ���� ���� */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "wt");
	if (0 != eRet)	return TRUE;
	
	/* �ּ� �Է� */
	fputs("; Recipe Name,Mark Type (0:Geomatrix,1:Pattern Image),Mark Count,Align Camera Number (1 or 2),Mark Model Name (Pattern File),,,\n", fp);
	pPos	= m_lstMark.GetHeadPosition();
	while (pPos)
	{
		/* �� �������� */
		pstRecipe	= m_lstMark.GetNext(pPos);
		/* ������ ���� */
		if (strlen(pstRecipe->r_name) > 0 && pstRecipe->count > 0)
		{
			/* ������ �̸� ���� */
			fputs(pstRecipe->r_name, fp);
			fputs(",", fp);
			/* ��ũ ���� (Type) ���� */
			sprintf_s(szData, 256, "%u,", pstRecipe->type);
			fputs(szData, fp);
			/* ��ϵ� �� ���� ���� */
			sprintf_s(szData, 256, "%u,", pstRecipe->count);
			fputs(szData, fp);
			/* �� ���� ���� */
			for (i=0; i<pstRecipe->count; i++)
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

/*
 desc : Recipe ���� ����ü �� ���� (����ü ���ο� �Ҵ�� �޸𸮰� �׸��� �����Ƿ�)
 parm : source	- [in]  ���� ���� ����� ��
		target	- [in]  ���ο� ���� ����(����)�� ��
 retn : None
*/
VOID CMark::CopyRecipe(LPG_CMRD source, LPG_CMRD target)
{
	UINT8 i	= 0x00;

	/* �޸� ���� */
	target->Init(source->count);

	/* ��� ���� �� ���� */
	target->type	= source->type;
	target->count	= source->count;
	memcpy(target->r_name, source->r_name, strlen(source->r_name));
	memcpy(target->acam_num, source->acam_num, source->count);
	for (; i<source->count; i++)
	{
		memcpy(target->m_name[i], source->m_name[i], strlen(source->m_name[i]));
	}
}

/*
 desc : Model Append
 parm : data	- [in]  Model ������ ����� ����ü ������
 retn : TRUE or FALSE (���� �ʰ�. �ִ� 15��)
*/
BOOL CMark::ModelAppend(LPG_CMPV value)
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
BOOL CMark::MarkAppend(LPG_CMRD recipe)
{
	LPG_CMRD pstRecipe	= NULL;

	if (m_lstMark.GetCount() >= MAX_REGIST_RECIPE_COUNT)
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
	if (MarkFind(recipe->r_name))
	{
		AfxMessageBox(L"There is a name for the same recipe", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* �� ������ ��ġ�� ��� */
	pstRecipe	= (LPG_CMRD)::Alloc(sizeof(STG_CMRD));
	ASSERT(pstRecipe);
	pstRecipe->Init(recipe->count);
	if (!pstRecipe)	return FALSE;
	/* ���� ������ ����ü ������ ���� ������ �� ���� */
	CopyRecipe(recipe, pstRecipe);
	/* ��� */
	m_lstMark.AddTail(pstRecipe);

	/* ������� ��ϵ� ��� Model ������ ���Ϸ� ���� */
	return SaveRecipe();
}

/*
 desc : Model Modify
 parm : value	- [in]  Model ������ ����� ����ü ������
 retn : TRUE or FALSE
*/
BOOL CMark::ModelModify(LPG_CMPV value)
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
BOOL CMark::MarkModify(LPG_CMRD recipe)
{
	LPG_CMRD pstRecipe	= NULL;

	/* �ʼ��� �ԷµǴ� ���� �ִ��� ���� */
	if (!recipe->IsValid())
	{
		AfxMessageBox(L"The update data is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ���� ���� ����� ���� ���õ� �����ǿ� �������� ���� */
	if (m_pstSelected && 0 == strcmp(recipe->r_name, m_pstSelected->r_name))
	{
		AfxMessageBox(L"The currently selected recipe cannot be deleted", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ������ ��ϵ� Model�� �ִ��� Ȯ�� */
	pstRecipe = GetRecipeName(recipe->r_name);
	if (!pstRecipe)
	{
		AfxMessageBox(L"The recipe name was not found in the mark file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ���� ������ ����ü ������ ���� ������ �� ���� */
	CopyRecipe(recipe, pstRecipe);

	/* ������� ��ϵ� ��� Model ������ ���Ϸ� ���� */
	return SaveRecipe();
}

/*
 desc : Model Modify
 parm : m_name	- [in]  �����Ϸ��� Model�� �̸�
 retn : TRUE or FALSE
*/
BOOL CMark::ModelDelete(PCHAR m_name)
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
BOOL CMark::MarkDelete(PCHAR r_name)
{
	/* ���� ����� ���� ���õ� �����ǿ� �������� ���� */
	if (m_pstSelected && 0 == strcmp(r_name, m_pstSelected->r_name))
	{
		AfxMessageBox(L"The currently selected recipe cannot be deleted", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* ������ ��ϵ� Model�� �ִ��� Ȯ�� */
	if (!GetRecipeName(r_name))
	{
		AfxMessageBox(L"The recipe name was not found in the mark file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* Model ���� ���� */
	RemoveMark(r_name);

	/* ������� ��ϵ� ��� Model ������ ���Ϸ� ���� */
	return SaveRecipe();
}

/*
 desc : �� Ÿ�Կ� ���� ���ڿ� ��ȯ
 parm : type	- [in]  �� Ÿ�� �ڵ� ��
 retn : ���ڿ� ��ȯ
*/
PTCHAR CMark::GetModelTypeToStr(UINT32 type)
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
BOOL CMark::SelRecipeName(PCHAR r_name)
{
	LPG_CMRD pstRecipe	= GetRecipeName(r_name);
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
BOOL CMark::FindModelInMark(PCHAR m_name)
{
	UINT8 i, j, u8Count	= (UINT8)m_lstMark.GetCount();
	LPG_CMRD pstRecipe	= NULL;

	for (i=0; i<u8Count; i++)
	{
		pstRecipe	= GetRecipeIndex(i);
		for (j=0; j<pstRecipe->count; j++)
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
BOOL CMark::SaveModel() 
{
	CHAR szData[256] = { NULL };
	TCHAR tzFile[MAX_PATH_LEN] = { NULL };
	errno_t eRet = 0;
	FILE* fp = NULL;
	POSITION pPos = NULL;
	LPG_CMPV pstModel = NULL;
	CUniToChar csCnv;

	/* Model File ���� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\data\\recipe\\%s.dat",
		m_tzWorkDir, GetConfig()->file_dat.mark_model);
	/* ���� ���� */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "wt");
	if (0 != eRet)	return TRUE;

	/* �ּ� �Է� */
	fputs("; Model Name, Model Type, Find Type, File Name OR Param 1, Param 2, Param 3, Param 4, Param 5,\n", fp);

	pPos = m_lstModel.GetHeadPosition();
	while (pPos)
	{
		/* �� �������� */
		pstModel = m_lstModel.GetNext(pPos);
		/* ���� : 01 ~ 07 */
		if (pstModel->find_type == 1 || ENG_MMDT(pstModel->type) == ENG_MMDT::en_image)
		{
			sprintf_s(szData, 256, "%s,%u,%u,%s,%d,%d,%d,%d,",
				pstModel->name, pstModel->type, pstModel->find_type, pstModel->file,
				pstModel->iSizeP.x, pstModel->iSizeP.y, pstModel->iOffsetP.x, pstModel->iOffsetP.y);
		}
		else
		{
			sprintf_s(szData, 256, "%s,%u,%u,%.1f,%.4f,%.4f,%.4f,%.4f,",
				pstModel->name,
				pstModel->type,
				pstModel->find_type,
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
BOOL CMark::SetMarkROI(LPG_CRD data, UINT8 index)
{
	m_pstROI->roi_Left[index] = data->roi_Left[index];
	m_pstROI->roi_Right[index] = data->roi_Right[index];
	m_pstROI->roi_Top[index] = data->roi_Top[index];
	m_pstROI->roi_Bottom[index] = data->roi_Bottom[index];

	return TRUE;
}

/* desc : Get Search ROI */
LPG_CRD CMark::GetMarkROI()
{
	return m_pstROI;
}

/* desc : Search ROI Save */
BOOL CMark::SaveROI()
{
	CHAR szData[256] = { NULL };
	TCHAR tzFile[MAX_PATH_LEN] = { NULL };
	UINT16 i;
	errno_t eRet = 0;
	FILE* fp = NULL;
	POSITION pPos = NULL;
	CUniToChar csCnv;

	/* Model File ���� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\data\\recipe\\%s.dat",
		m_tzWorkDir, GetConfig()->file_dat.mark_roi);
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
BOOL CMark::ParseSearchROI(PCHAR data, UINT32 size, UINT8 cnt)
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
