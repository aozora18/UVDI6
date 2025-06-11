
/*
 desc : Led Power Data ���� �� ����
*/

#include "pch.h"
#include "LedPower.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : None
 retn : None
*/
CLedPower::CLedPower(TCHAR *work_dir)
	: CBase(work_dir)
{
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CLedPower::~CLedPower()
{
	RemoveAll();
}

/*
 desc : ���� ��ϵ� Led Power ��� ����
 parm : None
 retn : None
*/
VOID CLedPower::RemoveAll()
{
	POSITION pPos	= NULL;
	LPG_PLPI pPower	= NULL;

	/* ���� ��ϵ� Recipe Data �޸� ���� */
	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		pPower	= m_lstRecipe.GetNext(pPos);
		if (pPower)	delete pPower;
	}
	m_lstRecipe.RemoveAll();
}

/*
 desc : Led Power ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLedPower::LoadFile()
{
	CHAR szData[1024]			= {NULL};
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	errno_t eRet				= 0;
	FILE *fp					= NULL;
	CUniToChar csCnv;

	/* ���� ������ ��� ���� */
	RemoveAll();

	/* Recipe File ���� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\recipe\\%s.dat",
			   m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.led_power);
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
			ParseAppend(szData, (UINT32)strlen(szData));
		}
	}

	/* ���� �ڵ� �ݱ� */
	fclose(fp);

	return TRUE;
}


/*
 desc : Led Power ����
 parm : strData	- [in]  ������ Led Power ������ ����� ���ڿ�
 retn : TRUE or FALSE
*/
BOOL CLedPower::SaveFile(CString strData)
{
	/* ���� ���� */
	strData.TrimLeft();
	strData.TrimRight();

	int nCommaCount = (MAX_PH * /*MAX_LED*/4) * 2 + 1;
	int nCount = (int)std::count((LPCTSTR)strData, (LPCTSTR)strData + strData.GetLength(), _T(','));

	/* ���� �˻� */
	if (nCommaCount != nCount)
	{
		/* �ҿ��� ���� */
		return FALSE;
	}

	else if (_T("\n") != strData.Right(2))
	{
		/* �������� �ݵ�� �ٹٲ� ���ڰ� �����ؾ� �Ѵ�. */
		strData += _T("\n");
	}

	CStdioFile sFile;
	CString strPath;

	strPath.Format(_T("%s\\%s\\recipe\\%s.dat"), m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.led_power);
	if (FALSE == sFile.Open(strPath, CFile::modeCreate | CFile::modeReadWrite | CFile::modeNoTruncate | CFile::typeText))
	{
		/* ������ �� �� ���� */
		return FALSE;
	}

	sFile.SeekToEnd();
	sFile.WriteString(strData);
	sFile.Close();

	return TRUE;
}


/*
 desc : Led Power ����
 parm : strData	- [in]  ������ Led Power ����ü
 retn : TRUE or FALSE
*/
BOOL CLedPower::SaveFile(LPG_PLPI pstData)
{
	CString strLine;
	CString strText;

	strLine = pstData->led_name;

	for (int nPH = 0; nPH < MAX_PH; nPH++)
	{
		for (int nLED = 0; nLED < /*MAX_LED*/4; nLED++)
		{
			if (0 == pstData->led_index[nPH][nLED])
			{
				strText = _T(",,");
			}
			else
			{
				strText.Format(_T(",%.3f,%d"), pstData->led_watt[nPH][nLED], pstData->led_index[nPH][nLED]);
			}

			strLine += strText; 
		}
	}

	strLine += _T(",");
	return SaveFile(strLine);
}


/*
 desc : Recipe �м� �� ��� ����
 parm : data	- [in]  Led Power ������ ����� ���ڿ� ����
		size	- [in]  'data' ���ۿ� ����� �������� ����
 retn : TRUE or FALSE
*/
BOOL CLedPower::ParseAppend(CHAR *data, UINT32 size)
{
	UINT32 i, j, k, u32Find = 0;
	CHAR *pData	= data, *pFind, szValue[256];
	LPG_PLPI pstData	= {NULL};

	/* �ϴ�, �־��� ���ڿ� �߿��� �޸�(',') ������ 9������ Ȯ�� */
	for (i=0; i<size; i++)
	{
		if (',' == data[i])	u32Find++;
	}
	if (u32Find != 65)
	{
		AfxMessageBox(L"Failed to analyse the value from led_power file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ������� �о ����ü�� ���� */
	pstData	= new STG_PLPI();
	ASSERT(pstData);
	/* �޸� �Ҵ� �� �ʱ�ȭ */
	pstData->Init();
	/* 01.led power name */
	pFind	= strchr(pData, ',');
	if (pFind)	{	memcpy(pstData->led_name, pData, pFind - pData);	pData = ++pFind;	}

	for (i=0,j=0,k=0; i<64 && pFind; i++)
	{
		pFind	= strchr(pData, ',');
		memset(szValue, 0x00, 256);
		memcpy(szValue, pData, pFind - pData);
		pData	= ++pFind;
		/* Led Power (W) */
		if (0 == (i%2))
		{
			pstData->led_watt[k][j] = (FLOAT)atof(szValue);
		}
		else
		{
			pstData->led_index[k][j] = (UINT16)atoi(szValue);
			j++;
		}
		/* 4���� ��� ó�� ������ */
		if (4 == j)
		{
			j = 0;	/* Photohead �� Led Number ���� */
			k++;	/* Photohead Number ���� */
		}
	}
	/* �м��� �����͸� ����Ʈ ���ۿ� ���� */
	if (64 != i)
	{
		AfxMessageBox(L"Failed to register the recipe data in list memory", MB_ICONSTOP|MB_TOPMOST);
		delete pstData;
		return FALSE;
	}

	/* �޸𸮿� �м��� ������ ������ ��� */
	m_lstRecipe.AddTail(pstData);

	return TRUE;
}

/*
 desc : Led Power �̸� �˻�
 parm : name	- [in]  �˻��ϰ��� �ϴ� ������ �̸�
 retn : TRUE (Finded) or FALSE (Not find)
*/
BOOL CLedPower::FindLedPower(CHAR *name)
{
	POSITION pPos	= NULL;

	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		if (0 == strcmp(name, m_lstRecipe.GetNext(pPos)->led_name))	return TRUE;
	}
	return FALSE;
}

/*
 desc : Led Power �̸��� ���� �� ��° ����Ǿ� �ִ���
 parm : name	- [in]  �˻��ϰ��� �ϴ� Led Power �̸�
 retn : ����Ʈ�� ����Ǿ� �ִ� ��ġ ��ȯ (�����̸� �˻� ����)
*/
INT32 CLedPower::GetLedPowerPos(CHAR *name)
{
	UINT32 u32Index	= 0;
	POSITION pPos	= NULL;

	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		if (0 == strcmp(name, m_lstRecipe.GetNext(pPos)->led_name))
		{
			return u32Index;
		}
		u32Index++;
	}
	return -1;
}

/*
 desc : Led Power �̸��� ���� ������ ��ȯ
 parm : name	- [in]  �˻��ϰ��� �ϴ� Led Power �̸�
 retn : ��ȯ�� Led Power ����� ����ü ������
*/
LPG_PLPI CLedPower::GetLedPowerName(CHAR *name)
{
	POSITION pPos	= NULL;
	LPG_PLPI pstData= NULL;

	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		pstData	= m_lstRecipe.GetNext(pPos);
		if (0 == strcmp(name, pstData->led_name))	return pstData;
	}
	return NULL;
}

/*
 desc : Led Power�� ����� �ε��� ��ġ�� �ִ� Led Power ��ȯ
 parm : index	- [in]  ���������� �ϴ� Led Power �ε��� (Zero-based)
 retn : ��ȯ�� Led Power�� ����� ����ü ������
*/
LPG_PLPI CLedPower::GetLedPowerIndex(UINT16 index)
{
	POSITION pPos	= NULL;

	/* ��ϵ� �������� ���������� �ϴ� ��ġ ���� ū ��� */
	if (index >= m_lstRecipe.GetCount())	return NULL;

	/* �ش� ��ġ �� ��� */
	pPos	= m_lstRecipe.FindIndex(index);
	if (!pPos)	return NULL;

	/* ��ġ�� �ش�Ǵ� ��� �� ��ȯ */
	return m_lstRecipe.GetAt(pPos);
}
