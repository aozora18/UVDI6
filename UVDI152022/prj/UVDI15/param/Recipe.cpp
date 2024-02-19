#include "pch.h"
#include "Recipe.h"
#include <locale.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


// 20211014 mhbaek Add : ShowMsg�� ���� ���� ���� �� Run ���� �߿� �޽��� ����
void ST_RECIPE_TAB::MsgWrongIndex(int nIndex)
{
	CString strMsg;
	//LANGUAGE �߰��Ϸ�
	strMsg.Format(_T("[JHMI_RCP.CSV] ������ Ȯ���Ͻʽÿ�.\n[%s TAB]���� [NAME] ������ �ʰ��ϴ� [Index - %d]�� ������ �� �����ϴ�."), strName, nIndex);
	AfxMessageBox(strMsg); 
}
void ST_RECIPE_TAB::MsgWrongIndex(CString strParam)
{
	CString strMsg;
	//LANGUAGE �߰��Ϸ�
	strMsg.Format(_T("[JHMI_RCP.CSV] ������ Ȯ���Ͻʽÿ�.\n[%s TAB]���� [NAME]�� [%s]�� �Ķ���Ͱ� �������� �ʾ� ������ �� �����ϴ�."), strName, strParam);
	AfxMessageBox(strMsg);
}

CRecipe::CRecipe()
{

}

CRecipe::~CRecipe()
{
	DeleteRecipeForm();
}

BOOL CRecipe::IsInt(CString strText)
{
	strText.Remove(_T('-'));

	for (int nCnt = 0; nCnt < strText.GetLength(); nCnt++)
	{
		if ('0' > strText.GetAt(nCnt) || '9' < strText.GetAt(nCnt))
		{
			return FALSE;
		}
	}
	return TRUE;
}

//double, int����� TRUE, �������Խ� FALSE
BOOL CRecipe::IsDouble(CString strText)
{
	strText.Remove(_T('-'));

	int nPeriod = 0;
	for (int nCnt = 0; nCnt < strText.GetLength(); nCnt++)
	{
		if ('0' > strText.GetAt(nCnt) || '9' < strText.GetAt(nCnt))
		{
			//��ħǥ���,
			if ('.' == strText.GetAt(nCnt))
			{
				//��ħǥ�� �ѹ��� �����ϰ�, �Ǿ�, �ǵڰ� �ƴ϶�� �ѹ��� �Ҽ������� �Ǵ�
				if (0 == nPeriod && (0 != nCnt && nCnt != strText.GetLength() - 1))
				{
					nPeriod++;
					continue;
				}
			}
			return FALSE;
		}
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//�ӽ� �޼���
void CRecipe::MsgWrongIndexTab(int nIndexTab)
{
	CString strMsg;
	//LANGUAGE �߰��Ϸ�
	strMsg.Format(_T("[JHMI_RCP.CSV] ������ Ȯ���Ͻʽÿ�.\n[TAB]�� ������ �ʰ��ϴ� [Index - %d]�� ������ �� �����ϴ�."), nIndexTab);
	AfxMessageBox(strMsg);	
}
void CRecipe::MsgWrongIndexTab(CString strTab)
{
	CString strMsg;
	//LANGUAGE �߰��Ϸ�
	strMsg.Format(_T("[JHMI_RCP.CSV] ������ Ȯ���Ͻʽÿ�.\n[TAB]�� [%s]�� ������ �̸��� �������� �ʾ� ������ �� �����ϴ�."), strTab);
	AfxMessageBox(strMsg);
}
void CRecipe::MsgWrongIndexScan(int nIndexScan)
{
	CString strMsg;
	//LANGUAGE �߰��Ϸ�
	strMsg.Format(_T("[JHMI_RCP.CSV] ������ Ȯ���Ͻʽÿ�.\n[TYPE = SCAN]�� [TAB]�� ������ �ʰ��ϴ� [Index - %d]�� ������ �� �����ϴ�."), nIndexScan);
	AfxMessageBox(strMsg);
}
void CRecipe::MsgWrongIndexScan(CString strScan)
{
	CString strMsg;
	//LANGUAGE �߰��Ϸ�
	strMsg.Format(_T("[JHMI_RCP.CSV] ������ Ȯ���Ͻʽÿ�.\n[TYPE = SCAN]�� [TAB]�� [%s]�� ������ �̸��� �������� �ʾ� ������ �� �����ϴ�."), strScan);
	AfxMessageBox(strMsg);
}
//////////////////////////////////////////////////////////////////////////
//Recipe
void CRecipe::MakeRecipeForm()
{
	DeleteRecipeForm();
 	//////////////////////////////////////////////////////////////////////////
 	// USER DEFINE : Enum ���ǵ� Recipe Parameter ����
 	for (int i = 0; i < (int)EN_RECIPE_TAB::_size(); i++)
 	{
 		ST_RECIPE_TAB stTab;
 		stTab.strName = (CString)EN_RECIPE_TAB::_from_index(i)._to_string();
 		stTab.nIndexTab = i;
 
 		switch (i)
 		{
 		case  EN_RECIPE_TAB::JOB:
 			for (int nParam = 0; nParam < (int)EN_RECIPE_JOB::_size(); nParam++)
 			{
 				ST_RECIPE_PARAM stParam;
 				stParam.nIndexParam = nParam;
 				stParam.strName = (CString)EN_RECIPE_JOB::_from_index(nParam)._to_string();
				stParam.strDisplayName = (CString)EN_RECIPE_JOB::_from_index(nParam)._to_string();
 				stTab.vParameter.push_back(stParam);
 			}
 			break;
 		case  EN_RECIPE_TAB::EXPOSE:
 			for (int nParam = 0; nParam < (int)EN_RECIPE_EXPOSE::_size(); nParam++)
 			{
 				ST_RECIPE_PARAM stParam;
 				stParam.nIndexParam = nParam;
 				stParam.strName = (CString)EN_RECIPE_EXPOSE::_from_index(nParam)._to_string();
				stParam.strDisplayName = (CString)EN_RECIPE_EXPOSE::_from_index(nParam)._to_string();
 				stTab.vParameter.push_back(stParam);
 			}
 			break;
 		case  EN_RECIPE_TAB::ALIGN:
 			for (int nParam = 0; nParam < (int)EN_RECIPE_ALIGN::_size(); nParam++)
 			{
 				ST_RECIPE_PARAM stParam;
 				stParam.nIndexParam = nParam;
 				stParam.strName = (CString)EN_RECIPE_ALIGN::_from_index(nParam)._to_string();
				stParam.strDisplayName = (CString)EN_RECIPE_ALIGN::_from_index(nParam)._to_string();
 				stTab.vParameter.push_back(stParam);
 			}
 			break;
 		
 		default:
 			continue;
 			break;
 		}
 
 		m_vTab.push_back(stTab);
 	}
	// USER DEFINE : Enum ���ǵ� Recipe Parameter ����
	//////////////////////////////////////////////////////////////////////////
}

void CRecipe::LoadRecipeForm()
{
	CString strValue;
	CString strLine;
	CString strPath;
	CString strOldName;
	CStdioFile clsFile;
	int nTemp = 0;
	int nCol = 0;
	int	nRowIndex = 0;
	CString strTitle;
	strPath.Format(_T("%s\\%s\\recipe\\RECIPE_FORM.csv"), g_tzWorkDir, CUSTOM_DATA_CONFIG);

	MakeRecipeForm();

	setlocale(LC_ALL, "korean");

	int			nArrCnt = 1;

	if (clsFile.Open(strPath, CStdioFile::modeRead | CStdioFile::shareDenyNone))
	{
		clsFile.ReadString(strTitle);  // Menu;

		while (clsFile.ReadString(strLine))
		{
			nCol = 0;

			if (!AfxExtractSubString(strValue, strLine, nCol++, ','))
			{
				break;
			}
			
			//���� �̸��� �˻��Ͽ� �ε����� �޾ƿ´�.
			for (auto& tab : m_vTab)
			{
				if (0 == strValue.Compare(tab.strName))
				{
					CString strGroup;
					AfxExtractSubString(strGroup, strLine, nCol++, ',');
					AfxExtractSubString(strValue, strLine, nCol++, ',');

					int nArrFind = -1;
					CString strArrIndex;
					nArrFind = strValue.ReverseFind(_T('_'));

					if (0 <= nArrFind)
					{
						strArrIndex = strValue.Mid(nArrFind + 1);

						if (0 >= IsInt(strArrIndex))
						{
							nArrFind = -1;
						}
					}

					for (auto &param : tab.vParameter)
					{
						if (0 == strValue.Compare(param.strName))
						{
							//////////////////////////////////////////////////////////////////////////
							//�׷쳻�� ���� �̸� ���Ͷ� ���� �˻�
							//strOldName�� ���� ���ӵ� �̸��϶��� �迭ó���ϴµ� �̰��� ���Ѵ�.
							if (FALSE == param.strGroup.IsEmpty() && strOldName != strValue)
							{
								CString strMsg;
								//LANGUAGE �߰��Ϸ�
								strMsg.Format(_T("[TAB : %s, NAME : %s] - �Ķ���� �ߺ�\n����, �ҷ����� �� ������ �˴ϴ�."), tab.strName, strValue);
								AfxMessageBox(strMsg);
							}
							//////////////////////////////////////////////////////////////////////////

							param.strGroup = strGroup;
							param.strName = strValue;

							//�����̸��� ���, �迭ó�� Ȱ���Ҽ� �ֵ����Ѵ�.
							//ó�� ������ �����̸��� Cnt�� �����Ǹ�,
							//[MARK X , Cnt = 4] [MARK X2, Cnt = 1] [MARK X3, Cnt = 1] [MARK X4, Cnt = 1] ������ �����Ͽ� �����Ѵ�.
							if (0 < nArrFind)
							{
								param.nArrCnt = _ttoi(strArrIndex);
							}
							else
							{
								param.nArrCnt = 1;
							}

							strOldName = strValue;

							AfxExtractSubString(strValue, strLine, nCol++, ',');
							param.strDisplayName = strValue;

							AfxExtractSubString(strValue, strLine, nCol++, ',');
							param.strDataType = strValue.MakeUpper();

							AfxExtractSubString(strValue, strLine, nCol++, ',');
							param.strUnit = strValue;

							AfxExtractSubString(strValue, strLine, nCol++, ',');
							param.bShow = _ttoi(strValue);

							//////////////////////////////////////////////////////////////////////////
							//Use�߰�
							AfxExtractSubString(strValue, strLine, nCol++, ',');
							param.bUse = _ttoi(strValue);							

							//�Ķ���Ͱ� �ϳ��� Ʈ���̸� Tab�� bUse�� TRUE
							if (TRUE == param.bUse)
							{
								tab.bUse = TRUE;
							}
							//////////////////////////////////////////////////////////////////////////

							AfxExtractSubString(strValue, strLine, nCol++, ',');
							//Default �߰� //Form�� �������� strValue�� ���� ���� ���
							param.strDefaultValue = strValue;
							param.strValue = strValue;

							AfxExtractSubString(strValue, strLine, nCol++, ',');
							param.dScale = _ttof(strValue);

							AfxExtractSubString(strValue, strLine, nCol++, ',');
							param.dChangeLimit = _ttof(strValue);

							AfxExtractSubString(strValue, strLine, nCol++, ',');
							param.dMin = _ttof(strValue);

							AfxExtractSubString(strValue, strLine, nCol++, ',');
							param.dMax = _ttof(strValue);

							AfxExtractSubString(strValue, strLine, nCol++, ',');
							param.eUILevel = (EN_LOGIN_LEVEL)_ttoi(strValue);

							AfxExtractSubString(strValue, strLine, nCol++, ',');
							param.strDescript = strValue;

							break;
						}
					}
				}
			}
		}

		clsFile.Close();
	}
	else
	{
		CString strMsg;
		//LANGUAGE �߰��Ϸ�
		strMsg.Format(_T("[%s] ���� ���� ����"), strPath);
		AfxMessageBox(strMsg);		
		return;
	}

	CStringArray strArrLine;

	for (const auto& tab : m_vTab)
	{
		for (const auto& param : tab.vParameter)
		{
			if (param.nArrCnt == 0)
			{
				CString strMsg;
				//LANGUAGE �߰��Ϸ�
				strMsg.Format(_T("[TAB : %s, NAME : %s] - JHMI_RCP.csv �ۼ��� �ʿ��մϴ�."), tab.strName, param.strName);
				AfxMessageBox(strMsg);
			}

			strLine.Format(_T("%s,"),			tab.strName);
			strLine.AppendFormat(_T("%s,"),		param.strGroup);
			strLine.AppendFormat(_T("%s,"),		param.strName);
			strLine.AppendFormat(_T("%s,"),		param.strDisplayName);
			strLine.AppendFormat(_T("%s,"),		param.strDataType);
			strLine.AppendFormat(_T("%s,"),		param.strUnit);
			strLine.AppendFormat(_T("%d,"),		param.bShow);
			strLine.AppendFormat(_T("%d,"),		param.bUse);
			strLine.AppendFormat(_T("%s,"),		param.strDefaultValue);
			strLine.AppendFormat(_T("%.3f,"),	param.dScale);
			strLine.AppendFormat(_T("%.3f,"),	param.dChangeLimit);
			strLine.AppendFormat(_T("%.3f,"),	param.dMin);
			strLine.AppendFormat(_T("%.3f,"),	param.dMax);
			strLine.AppendFormat(_T("%d,"),		param.eUILevel);
			strLine.AppendFormat(_T("%s\n"),	param.strDescript);

			strArrLine.Add(strLine);
		}
	}

	// ������
	if (clsFile.Open(strPath, CStdioFile::modeCreate | CStdioFile::modeWrite))
	{
		clsFile.SeekToBegin();
		//Header
 		clsFile.WriteString(strTitle + _T("\n"));

		for (int i = 0; i < (int)strArrLine.GetCount(); i++)
		{
			clsFile.WriteString(strArrLine.GetAt(i));
		}
	}

	strArrLine.RemoveAll();
}

void CRecipe::DeleteRecipeForm()
{	
	for (auto& tab : m_vTab)
	{
		tab.vParameter.clear();
		tab.vParameter.shrink_to_fit();
	}

	m_vTab.clear();
	m_vTab.shrink_to_fit();
}

int CRecipe::GetTabCount()
{
	return (int)m_vTab.size();
}

//************************************
// Method : GetTab
// Date   : 2021/02/24
// Author : JBP
// Coment : ���н� NULL�� �����Ѵ�.
//************************************
ST_RECIPE_TAB CRecipe::GetTab(int nIndexTab)
{		
	ST_RECIPE_TAB stTab;
	if (nIndexTab < 0 || nIndexTab >= GetTabCount()) { MsgWrongIndexTab(nIndexTab); return stTab; }

	return m_vTab.at(nIndexTab);
}
ST_RECIPE_TAB CRecipe::GetTab(CString strTab)
{
	ST_RECIPE_TAB stTab;
	ST_FIND_RECIPE stFindTab(strTab);
	auto iparam = std::find_if(m_vTab.begin(), m_vTab.end(), stFindTab);
	if (m_vTab.end() == iparam) { MsgWrongIndexTab(strTab); return stTab; }

	return (*iparam);
}
//************************************
// Method : GetTabIndex
// Date   : 2021/02/24
// Author : JBP
// Coment : ���н� 0�� �����Ѵ�.
//			�߸��� �ε����� �������� �ʵ��� �ϱ�����
//************************************
int	CRecipe::GetTabIndex(CString strTab)
{
	ST_FIND_RECIPE stFindTab(strTab);
	auto iparam = std::find_if(m_vTab.begin(), m_vTab.end(), stFindTab);
	if (m_vTab.end() == iparam) { MsgWrongIndexTab(strTab); return -1; }

	return iparam->nIndexTab;
}

//************************************
// Method : GetTabName
// Date   : 2021/02/24
// Author : JBP
// Coment : ���� �ε����� ������ _T("")�� �����Ѵ�.
//			GetTab�� �ε��� �߸������Ѱ�� NULL ����
//************************************
CString	CRecipe::GetTabName(int nIndexTab)
{
	ST_RECIPE_TAB stTab = GetTab(nIndexTab);

	return stTab.strName;
}

BOOL CRecipe::GetTabUse(CString strTab)
{
	ST_RECIPE_TAB stTab = GetTab(strTab);

	return stTab.bUse;
}

BOOL CRecipe::GetTabUse(int nIndexTab)
{
	ST_RECIPE_TAB stTab = GetTab(nIndexTab);

	return stTab.bUse;
}

int	CRecipe::GetParamArrCount(int nIndexTab, int nIndexParam)
{
	ST_RECIPE_TAB stTab = GetTab(nIndexTab);
	
	return stTab.GetArrCount(nIndexParam);
}
int	CRecipe::GetParamArrCount(int nIndexTab, CString strParam)
{
	ST_RECIPE_TAB stTab = GetTab(nIndexTab);
	
	return stTab.GetArrCount(strParam);
}
int	CRecipe::GetParamArrCount(CString strTab, int nIndexParam)
{
	ST_RECIPE_TAB stTab = GetTab(strTab);
	
	return stTab.GetArrCount(nIndexParam);
}
int	CRecipe::GetParamArrCount(CString strTab, CString strParam)
{
	ST_RECIPE_TAB stTab = GetTab(strTab);
	
	return stTab.GetArrCount(strParam);
}

int	CRecipe::GetParamCount(int nIndexTab)
{
	ST_RECIPE_TAB stTab = GetTab(nIndexTab);
	
	return (int)stTab.vParameter.size();
}

int	CRecipe::GetParamCount(CString strTab)
{
	ST_RECIPE_TAB stTab = GetTab(strTab);
	
	return (int)stTab.vParameter.size();
}

ST_RECIPE_PARAM CRecipe::GetParam(int nIndexTab, int nIndexParam)
{	
	ST_RECIPE_PARAM stParam = GetTab(nIndexTab).GetParam(nIndexParam);
	
	return stParam;
}

ST_RECIPE_PARAM CRecipe::GetParam(int nIndexTab, CString strParam)
{		
	ST_RECIPE_PARAM stParam = GetTab(nIndexTab).GetParam(strParam);

	return stParam;
}

ST_RECIPE_PARAM CRecipe::GetParam(CString strTab, int nIndexParam)
{		
	ST_RECIPE_PARAM stParam = GetTab(strTab).GetParam(nIndexParam);

	return stParam;
}

ST_RECIPE_PARAM CRecipe::GetParam(CString strTab, CString strParam)
{
	ST_RECIPE_PARAM stParam = GetTab(strTab).GetParam(strParam);

	return stParam;
}

void CRecipe::SetParam(int nIndexTab, int nIndexParam, ST_RECIPE_PARAM stParam)
{
	ST_FIND_RECIPE stFindTab(nIndexTab);
	auto itab = std::find_if(m_vTab.begin(), m_vTab.end(), stFindTab);
	if (itab == m_vTab.end()) return;

	ST_FIND_RECIPE stFindParam(nIndexParam);
	auto iparam = std::find_if(itab->vParameter.begin(), itab->vParameter.end(), stFindParam);
	if (iparam == itab->vParameter.end()) return;

	iparam->Set(stParam);
}

void CRecipe::SetParam(int nIndexTab, ST_RECIPE_PARAM stParam)
{
	ST_FIND_RECIPE stFindTab(nIndexTab);
	auto itab = std::find_if(m_vTab.begin(), m_vTab.end(), stFindTab);
	if (itab == m_vTab.end()) return;

	ST_FIND_RECIPE stFindParam(stParam.nIndexParam);
	auto iparam = std::find_if(itab->vParameter.begin(), itab->vParameter.end(), stFindParam);
	if (iparam == itab->vParameter.end()) return;

	iparam->Set(stParam);
}

void CRecipe::SetParam(int nIndexTab, CString strParam, ST_RECIPE_PARAM stParam)
{
	ST_FIND_RECIPE stFindTab(nIndexTab);
	auto itab = std::find_if(m_vTab.begin(), m_vTab.end(), stFindTab);
	if (itab == m_vTab.end()) return;

	ST_FIND_RECIPE stFindParam(strParam);
	auto iparam = std::find_if(itab->vParameter.begin(), itab->vParameter.end(), stFindParam);
	if (iparam == itab->vParameter.end()) return;

	iparam->Set(stParam);
}
void CRecipe::SetParam(CString strTab, int nIndexParam, ST_RECIPE_PARAM stParam)
{
	ST_FIND_RECIPE stFindTab(strTab);
	auto itab = std::find_if(m_vTab.begin(), m_vTab.end(), stFindTab);
	if (itab == m_vTab.end()) return;

	ST_FIND_RECIPE stFindParam(nIndexParam);
	auto iparam = std::find_if(itab->vParameter.begin(), itab->vParameter.end(), stFindParam);
	if (iparam == itab->vParameter.end()) return;

	iparam->Set(stParam);
}
void CRecipe::SetParam(CString strTab, CString strParam, ST_RECIPE_PARAM stParam)
{
	ST_FIND_RECIPE stFindTab(strTab);
	auto itab = std::find_if(m_vTab.begin(), m_vTab.end(), stFindTab);
	if (itab == m_vTab.end()) return;

	ST_FIND_RECIPE stFindParam(strParam);
	auto iparam = std::find_if(itab->vParameter.begin(), itab->vParameter.end(), stFindParam);
	if (iparam == itab->vParameter.end()) return;

	iparam->Set(stParam);
}

//************************************
// Method : GetParamIndex
// Date   : 2021/02/25
// Author : JBP
// Coment : �ش� �Ķ������ �ε����� ����
//			���� �ε����� ������ 0 ����
//************************************
int CRecipe::GetParamIndex(int nIndexTab, CString strParam)
{		
	return GetParam(nIndexTab, strParam).nIndexParam;
}
int CRecipe::GetParamIndex(CString strTab, CString strParam)
{
	return GetParam(strTab, strParam).nIndexParam;
}

BOOL CRecipe::GetParamIndex(CString strTab, CString strParam, int &nIndex)
{
	int nIndexParam = GetParam(strTab, strParam).nIndexParam;;

	if (0 > nIndexParam)
	{
		return FALSE;
	}

	nIndex = nIndexParam;

	return TRUE;
}

//************************************
// Method : GetParamName
// Date   : 2021/02/25
// Author : JBP
// Coment : �ش� �Ķ������ �ε����� �̸��� ����
//			���� �ε����� ������ _T("") ����
//************************************
CString CRecipe::GetParamName(int nIndexTab, int nIndexParam)
{
	return GetParam(nIndexTab, nIndexParam).strName;
}
CString CRecipe::GetParamName(CString strTab, int nIndexParam)
{
	return GetParam(strTab, nIndexParam).strName;
}

CString CRecipe::GetParamDataType(int nIndexTab, int nIndexParam)
{
	return GetParam(nIndexTab, nIndexParam).strDataType;
}

CString	CRecipe::GetParamDataType(int nIndexTab, CString strParam)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, strParam);

	return stParam.strDataType;
}

CString	CRecipe::GetParamDataType(CString strTab, int nIndexParam)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, nIndexParam);
	
	return stParam.strDataType;
}

CString	CRecipe::GetParamDataType(CString strTab, CString strParam)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, strParam);
	
	return stParam.strDataType;
}

//************************************
// Method : GetCount
// Date   : 2021/02/25
// Author : JBP
// Coment : nArrCnt�� �����Ѵ�.
//			Recipe ������ �����̸��� ���������� N�� �ݺ��ȴٸ� ù��° �Ķ���ʹ� nArrCnt = N���� ���´�.(�������Ķ���ʹ� 1)
//	  ����) MARK X�� ������ 4���� �������� ������� [strName, nArrCnt]�� �Ʒ��� ���� ����Ǿ��ִ�.
//			[MARK X , Cnt = 4] [MARK X2, Cnt = 1] [MARK X3, Cnt = 1] [MARK X4, Cnt = 1] ������ �����Ͽ� �����Ѵ�.
//			���� �ε����� ������ 0 ����
//************************************
int CRecipe::GetCount(int nIndexTab, int nIndexParam)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	

	return stParam.nArrCnt;
}
int CRecipe::GetCount(int nIndexTab, CString strParam)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, strParam);
	

	return stParam.nArrCnt;
}
int CRecipe::GetCount(CString strTab, int nIndexParam)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, nIndexParam);
	

	return stParam.nArrCnt;
}
int CRecipe::GetCount(CString strTab, CString strParam)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, strParam);
	

	return stParam.nArrCnt;
}

CString	CRecipe::GetDefaultValue(int nIndexTab, int nIndexParam)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	

	return stParam.GetDefaultValue();
}
CString	CRecipe::GetDefaultValue(int nIndexTab, CString strParam)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, strParam);
	

	return stParam.GetDefaultValue();
}
CString	CRecipe::GetDefaultValue(CString strTab, int nIndexParam)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, nIndexParam);
	

	return stParam.GetDefaultValue();
}
CString	CRecipe::GetDefaultValue(CString strTab, CString strParam)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, strParam);
	

	return stParam.GetDefaultValue();
}

int	CRecipe::GetDefaultInt(int nIndexTab, int nIndexParam)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	

	return stParam.GetDefaultInt();
}
int	CRecipe::GetDefaultInt(int nIndexTab, CString strParam)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, strParam);
	

	return stParam.GetDefaultInt();
}
int	CRecipe::GetDefaultInt(CString strTab, int nIndexParam)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, nIndexParam);
	

	return stParam.GetDefaultInt();
}
int	CRecipe::GetDefaultInt(CString strTab, CString strParam)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, strParam);
	

	return stParam.GetDefaultInt();
}

double CRecipe::GetDefaultDouble(int nIndexTab, int nIndexParam)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	

	return stParam.GetDefaultDouble();
}
double CRecipe::GetDefaultDouble(int nIndexTab, CString strParam)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, strParam);
	

	return stParam.GetDefaultDouble();
}
double CRecipe::GetDefaultDouble(CString strTab, int nIndexParam)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, nIndexParam);
	

	return stParam.GetDefaultDouble();
}
double CRecipe::GetDefaultDouble(CString strTab, CString strParam)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, strParam);
	

	return stParam.GetDefaultDouble();
}

//************************************
// Method : GetValue
// Date   : 2021/02/25
// Author : JBP
// Coment : �ش� �Ķ������ ���� String���� �����Ѵ�.
//			���� �ε��� ������ _T("") ����
//************************************
CString	CRecipe::GetValue(int nIndexTab, int nIndexParam)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	

	return stParam.GetValue();
}
CString	CRecipe::GetValue(int nIndexTab, CString strParam)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, strParam);
	

	return stParam.GetValue();
}
CString	CRecipe::GetValue(CString strTab, int nIndexParam)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, nIndexParam);
	

	return stParam.GetValue();
}
CString	CRecipe::GetValue(CString strTab, CString strParam)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, strParam);
	

	return stParam.GetValue();
}

CString	CRecipe::GetValue(CString strParam)
{
	for (const auto &tab : m_vTab)
	{
		for (const auto& param : tab.vParameter)
		{
			if (0 == strParam.Compare(param.strName))
			{
				return param.strValue;
			}
		}
	}

	return _T("INVALID DATA");;
}

//************************************
// Method : GetInt
// Date   : 2021/02/25
// Author : JBP
// Coment : �ش� �Ķ������ ���� int�� �����Ѵ�.
//			���� �ε��� ������ 0 ����
//************************************
int	CRecipe::GetInt(int nIndexTab, int nIndexParam)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, nIndexParam);

	return stParam.GetInt();
}
int	CRecipe::GetInt(int nIndexTab, CString strParam)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, strParam);

	return stParam.GetInt();
}
int	CRecipe::GetInt(CString strTab, int nIndexParam)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, nIndexParam);

	return stParam.GetInt();
}
int	CRecipe::GetInt(CString strTab, CString strParam)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, strParam);

	return stParam.GetInt();
}

//************************************
// Method : GetDouble
// Date   : 2021/02/25
// Author : JBP
// Coment : �ش� �Ķ������ ���� double�� �����Ѵ�.
//			���� �ε��� ������ 0 ����
//************************************
double CRecipe::GetDouble(int nIndexTab, int nIndexParam)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	
	return stParam.GetDouble();
}
double CRecipe::GetDouble(int nIndexTab, CString strParam)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, strParam);

	return stParam.GetDouble();
}
double CRecipe::GetDouble(CString strTab, int nIndexParam)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, nIndexParam);

	return stParam.GetDouble();
}
double CRecipe::GetDouble(CString strTab, CString strParam)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, strParam);

	return stParam.GetDouble();
}

//************************************
// Method : SetValue
// Date   : 2021/02/25
// Author : JBP
// Coment : �ش� �Ķ���Ϳ� ���� �ִ´�.
//			���� �ε��� ������ FALSE ����
//			���˻� �߰��Ͽ�, SetValue�ȿ��� ���˻� ���нõ� FALSE ����
//************************************
BOOL CRecipe::SetValue(int nIndexTab, int nIndexParam, CString strValue)
{	
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(strValue);
	if (bReturn)
	{
		SetParam(nIndexTab, nIndexParam, stParam);
	}

	return bReturn;
}
BOOL CRecipe::SetValue(int nIndexTab, CString strParam, CString strValue)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(strValue);
	if (bReturn)
	{
		SetParam(nIndexTab, strParam, stParam);
	}

	return bReturn;
}
BOOL CRecipe::SetValue(CString strTab, int nIndexParam, CString strValue)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(strValue);
	if (bReturn)
	{
		SetParam(strTab, nIndexParam, stParam);
	}
	
	return bReturn;
}
BOOL CRecipe::SetValue(CString strTab, CString strParam, CString strValue)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(strValue);
	if (bReturn)
	{
		SetParam(strTab, strParam, stParam);
	}
	
	return bReturn;
}

//************************************
// Method : SetValue
// Date   : 2021/02/25
// Author : JBP
// Coment : �ش� �Ķ���Ϳ� ���� �ִ´�.
//			���� �ε��� ������ FALSE ����
//			���˻� �߰��Ͽ�, SetValue�ȿ��� ���˻� ���нõ� FALSE ����
//************************************
BOOL CRecipe::SetValue(int nIndexTab, int nIndexParam, int nValue)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(nValue);
	SetParam(nIndexTab, nIndexParam, stParam);
	return bReturn;
}
BOOL CRecipe::SetValue(int nIndexTab, CString strParam, int nValue)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(nValue);
	SetParam(nIndexTab, strParam, stParam);
	return bReturn;
}
BOOL CRecipe::SetValue(CString strTab, int nIndexParam, int nValue)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(nValue);
	SetParam(strTab, nIndexParam, stParam);
	return bReturn;
}
BOOL CRecipe::SetValue(CString strTab, CString strParam, int nValue)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(nValue);
	SetParam(strTab, strParam, stParam);
	return bReturn;
}

//************************************
// Method : SetValue
// Date   : 2021/02/25
// Author : JBP
// Coment : �ش� �Ķ���Ϳ� ���� �ִ´�.
//			���� �ε��� ������ FALSE ����
//************************************
BOOL CRecipe::SetValue(int nIndexTab, int nIndexParam, double dValue)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(dValue);
	SetParam(nIndexTab, nIndexParam, stParam);
	return bReturn;
}
BOOL CRecipe::SetValue(int nIndexTab, CString strParam, double dValue)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(dValue);
	SetParam(nIndexTab, strParam, stParam);
	return bReturn;
}
BOOL CRecipe::SetValue(CString strTab, int nIndexParam, double dValue)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(dValue);
	SetParam(strTab, nIndexParam, stParam);
	return bReturn;
}
BOOL CRecipe::SetValue(CString strTab, CString strParam, double dValue)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(dValue);
	SetParam(strTab, strParam, stParam);
	return bReturn;
}

BOOL CRecipe::GetShow(int nIndexTab, int nIndexParam)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	return stParam.GetShow();
}
BOOL CRecipe::GetShow(int nIndexTab, CString strParam)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	return stParam.GetShow();
}
BOOL CRecipe::GetShow(CString strTab, int nIndexParam)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	return stParam.GetShow();
}
BOOL CRecipe::GetShow(CString strTab, CString strParam)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	return stParam.GetShow();
}

BOOL CRecipe::GetUse(int nIndexTab, int nIndexParam)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	return stParam.GetUse();
}
BOOL CRecipe::GetUse(int nIndexTab, CString strParam)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	return stParam.GetUse();
}
BOOL CRecipe::GetUse(CString strTab, int nIndexParam)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	return stParam.GetUse();
}
BOOL CRecipe::GetUse(CString strTab, CString strParam)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	return stParam.GetUse();
}

BOOL CRecipe::SetUse(int nIndexTab, int nIndexParam, BOOL bUse)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	stParam.SetUse(bUse);
	SetParam(nIndexTab, nIndexParam, stParam);
	return TRUE;
}
BOOL CRecipe::SetUse(int nIndexTab, CString strParam, BOOL bUse)
{
	ST_RECIPE_PARAM stParam = GetParam(nIndexTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	stParam.SetUse(bUse);
	SetParam(nIndexTab, strParam, stParam);
	return TRUE;
}
BOOL CRecipe::SetUse(CString strTab, int nIndexParam, BOOL bUse)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	stParam.SetUse(bUse);
	SetParam(strTab, nIndexParam, stParam);
	return TRUE;
}
BOOL CRecipe::SetUse(CString strTab, CString strParam, BOOL bUse)
{
	ST_RECIPE_PARAM stParam = GetParam(strTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	stParam.SetUse(bUse);
	SetParam(strTab, strParam, stParam);
	return TRUE;
}

CString CRecipe::GetCIMValue(CString strTab, CString strCIMName)
{
	for (auto& tab : m_vTab)
	{
		for (auto& param : tab.vParameter)
		{
			if (0 == strCIMName.Compare(param.strName))
			{
				return param.GetScaledValue();
			}
		}
	}

	return _T("INVALID DATA");;
}

BOOL CRecipe::SetCIMValue(CString strTab, CString strCIMName, CString strValue)
{
	for (auto& tab : m_vTab)
	{
		for (auto& param : tab.vParameter)
		{
			if (0 == strCIMName.Compare(param.strName))
			{
				return param.SetScaledValue(strValue);
			}
		}
	}

	return FALSE;
}
