#include "pch.h"
#include "Setting.h"
#include <locale.h>

#define		DEF_SETTING_CSV				_T("SETTING_FORM.csv")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

void ST_SETTING_TAB::MsgWrongIndex(int nIndex)
{

	CString strMsg;
	//LANGUAGE 추가완료
	strMsg.Format(_T("[%s] 문서를 확인하십시오.\n[%s TAB]에서 [NAME] 갯수를 초과하는 [Index - %d]에 접근할 수 없습니다."), DEF_SETTING_CSV, strName, nIndex);
	AfxMessageBox(strMsg);
}

void ST_SETTING_TAB::MsgWrongIndex(CString strParam)
{

	CString strMsg;
	strMsg.Format(_T("[%s] 문서를 확인하십시오.\n[%s TAB]에서 [NAME]이 [%s]인 파라미터가 존재하지 않아 접근할 수 없습니다."), DEF_SETTING_CSV, strName, strParam);
	AfxMessageBox(strMsg);
}

CSetting::CSetting()
{
}

CSetting::~CSetting()
{
	DeleteSettingForm();
}


void CSetting::MsgWrongIndexTab(int nIndexTab)
{

	CString strMsg;
	strMsg.Format(_T("[%s] 문서를 확인하십시오.\n[TAB]의 개수를 초과하는 [Index - %d]에 접근할 수 없습니다."), DEF_SETTING_CSV, nIndexTab);
	AfxMessageBox(strMsg);
}

void CSetting::MsgWrongIndexTab(CString strTab)
{

	CString strMsg;
	//LANGUAGE 추가완료
	strMsg.Format(_T("[%s] 문서를 확인하십시오.\n[TAB]에 [%s]와 동일한 이름이 존재하지 않아 접근할 수 없습니다."), DEF_SETTING_CSV, strTab);
	AfxMessageBox(strMsg);
}

//////////////////////////////////////////////////////////////////////////
//Setting
void CSetting::MakeSettingForm()
{
	DeleteSettingForm();

	for (int i = 0; i < EN_SETTING_TAB::MAX; i++)
	{
		ST_SETTING_TAB stTab;
		stTab.strName = (CString)EN_SETTING_TAB::_from_index(i)._to_string();
		stTab.nIndexTab = i;

		switch (i)
		{
		case  EN_SETTING_TAB::OPTION:
			for (int nParam = 0; nParam < EN_SETTING_OPTION::MAX; nParam++)
			{
				ST_SETTING_PARAM stParam;
				stParam.nIndexParam = nParam;
				stParam.strName = (CString)EN_SETTING_OPTION::_from_index(nParam)._to_string();
				stTab.vParameter.push_back(stParam);
			}
			break;
		default:
			continue;
			break;
		}

		m_vTab.push_back(stTab);
	}
}

void CSetting::LoadSettingForm()
{
	CString strValue;
	CString strLine;
	CString strPath;
	CString strTitle;
	CStdioFile clsFile;
	int nTemp = 0;
	int nCol = 0;
	int	nRowIndex = 0;

	//strPath.Format(_T("%s%s%s"), g_tzWorkDir,_T("\\%s\\setting\\", CUSTOM_DATA_CONFIG), DEF_SETTING_CSV);
	strPath.Format(_T("%s\\%s\\\setting\\%s"), g_tzWorkDir, CUSTOM_DATA_CONFIG, DEF_SETTING_CSV);

	
	MakeSettingForm();

	setlocale(LC_ALL, "korean");

	CString strOldName = _T("");
	
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
			
			//같은 이름을 검색하여 인덱스를 받아온다.
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

					for (auto& param : tab.vParameter)
					{
						if (0 == strValue.Compare(param.strName))
						{
							//////////////////////////////////////////////////////////////////////////
											//그룹내에 같은 이름 인터락 같이 검사
											//strOldName에 의해 연속된 이름일때는 배열처리하는데 이경우는 피한다.
							if (FALSE == param.strGroup.IsEmpty() && strOldName != strValue)
							{
								CString strMsg;
								//LANGUAGE 추가완료
								strMsg.Format(_T("[TAB : %s, NAME : %s] - 파라미터 중복\n저장, 불러오기 시 문제가 됩니다."), tab.strName, strValue);
								AfxMessageBox(strMsg);
							}
							//////////////////////////////////////////////////////////////////////////

							param.strGroup = strGroup;
							for (int nGroupCnt = 0; nGroupCnt < EN_SETTING_GROUP::MAX; nGroupCnt++)
							{
								if (0 == strGroup.Compare((CString)EN_SETTING_GROUP::_from_index(nGroupCnt)._to_string()))
								{
									param.nIndexGroup = nGroupCnt;
									break;
								}

							}

							param.strName = strValue;

							//같은이름일 경우, 배열처럼 활용할수 있도록한다.
							//처음 등장한 같은이름의 Cnt가 누적되며,
							//[MARK X , Cnt = 4] [MARK X2, Cnt = 1] [MARK X3, Cnt = 1] [MARK X4, Cnt = 1] 과같이 변경하여 저장한다.
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
							param.strCIMName = strValue;

							AfxExtractSubString(strValue, strLine, nCol++, ',');
							param.strDataType = strValue.MakeUpper();

							AfxExtractSubString(strValue, strLine, nCol++, ',');
							param.strUnit = strValue;

							AfxExtractSubString(strValue, strLine, nCol++, ',');
							param.bShow = _ttoi(strValue);

							//////////////////////////////////////////////////////////////////////////
							//Use추가
							AfxExtractSubString(strValue, strLine, nCol++, ',');
							param.bUse = _ttoi(strValue);

							//파라미터가 하나라도 트루이면 Tab의 bUse는 TRUE
							if (TRUE == param.bUse)
							{
								tab.bUse = TRUE;
							}
							//////////////////////////////////////////////////////////////////////////

							AfxExtractSubString(strValue, strLine, nCol++, ',');
							//Default 추가 //Form을 읽을때는 strValue와 같은 값을 사용
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
		//LANGUAGE 추가완료
		strMsg.Format(_T("[%s] 파일 열기 실패"), strPath);
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
				//LANGUAGE 추가완료
				strMsg.Format(_T("[TAB : %s, NAME : %s] - %s 작성이 필요합니다."), tab.strName, param.strName, DEF_SETTING_CSV);
				AfxMessageBox(strMsg);
			}

			strLine.Format(_T("%s,"), tab.strName);
			strLine.AppendFormat(_T("%s,"), param.strGroup);
			strLine.AppendFormat(_T("%s,"), param.strName);
			strLine.AppendFormat(_T("%s,"), param.strDisplayName);
			strLine.AppendFormat(_T("%s,"), param.strCIMName);
			strLine.AppendFormat(_T("%s,"), param.strDataType);
			strLine.AppendFormat(_T("%s,"), param.strUnit);
			strLine.AppendFormat(_T("%d,"), param.bShow);
			strLine.AppendFormat(_T("%d,"), param.bUse);
			strLine.AppendFormat(_T("%s,"), param.strDefaultValue);
			strLine.AppendFormat(_T("%.3f,"), param.dScale);
			strLine.AppendFormat(_T("%.3f,"), param.dChangeLimit);
			strLine.AppendFormat(_T("%.3f,"), param.dMin);
			strLine.AppendFormat(_T("%.3f,"), param.dMax);
			strLine.AppendFormat(_T("%d,"), param.eUILevel);
			strLine.AppendFormat(_T("%s\n"), param.strDescript);

			strArrLine.Add(strLine);
		}
	}

	// 재저장
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

void CSetting::DeleteSettingForm()
{	
	for (auto& tab : m_vTab)
	{
		tab.vParameter.clear();
		tab.vParameter.shrink_to_fit();
	}

	m_vTab.clear();
	m_vTab.shrink_to_fit();
}

int CSetting::GetTabCount()
{
	return (int)m_vTab.size();
}

//************************************
// Method : GetTab
// Date   : 2021/02/24
// Author : JBP
// Coment : 실패시 NULL을 리턴한다.
//************************************
ST_SETTING_TAB CSetting::GetTab(int nIndexTab)
{		
	ST_SETTING_TAB stDefault;
	if (nIndexTab < 0 || nIndexTab >= GetTabCount()) { MsgWrongIndexTab(nIndexTab); return stDefault; }
	return m_vTab.at(nIndexTab);
}
ST_SETTING_TAB CSetting::GetTab(CString strTab)
{	
	ST_SETTING_TAB stDefault;
	ST_FIND_SETTING stFind(strTab);
	auto iparam = std::find_if(m_vTab.begin(), m_vTab.end(), stFind);

	if (iparam == m_vTab.end()) { MsgWrongIndexTab(strTab); return stDefault; }
	return (*iparam);
}
//************************************
// Method : GetTabIndex
// Date   : 2021/02/24
// Author : JBP
// Coment : 실패시 0을 리턴한다.
//			잘못된 인덱스를 참조하지 않도록 하기위해
//************************************
int	CSetting::GetTabIndex(CString strTab)
{
	ST_FIND_SETTING stFind(strTab);
	auto iparam = std::find_if(m_vTab.begin(), m_vTab.end(), stFind);

	if (iparam == m_vTab.end()) { MsgWrongIndexTab(strTab); return DEF_INVALID_DATA; }
	return iparam->nIndexTab;
}

//************************************
// Method : GetTabName
// Date   : 2021/02/24
// Author : JBP
// Coment : 없는 인덱스를 참조시 _T("")를 리턴한다.
//			GetTab은 인덱스 잘못참조한경우 NULL 리턴
//************************************
CString	CSetting::GetTabName(int nIndexTab)
{
	ST_SETTING_TAB stTab = GetTab(nIndexTab);
	if (0 > stTab.nIndexTab) { return _T(""); }
	return stTab.strName;
}

BOOL CSetting::GetTabUse(CString strTab)
{
	ST_SETTING_TAB stTab = GetTab(strTab);
	if (0 > stTab.nIndexTab) { return FALSE; }
	return stTab.bUse;
}

BOOL CSetting::GetTabUse(int nIndexTab)
{
	ST_SETTING_TAB stTab = GetTab(nIndexTab);
	if (0 > stTab.nIndexTab) { return FALSE; }
	return stTab.bUse;
}

//************************************
// Method : GetParamCount
// Date   : 2021/02/24
// Author : JBP
// Coment : 해당탭의 파라미터 갯수를 리턴
//			없는 인덱스를 참조시 0를 리턴한다.
//			GetTab은 인덱스 잘못참조한경우 NULL 리턴
//************************************
int	CSetting::GetParamCount(int nIndexTab)
{	
	ST_SETTING_TAB stTab = GetTab(nIndexTab);
	if (0 > stTab.nIndexTab) { return 0; }
	return (int)stTab.vParameter.size();
}
int	CSetting::GetParamCount(CString strTab)
{
	ST_SETTING_TAB stTab = GetTab(strTab);
	if (0 > stTab.nIndexTab) { return 0; }
	return (int)stTab.vParameter.size();
}

//************************************
// Method : GetParam
// Date   : 2021/02/24
// Author : JBP
// Coment : 해당 파라미터구조체의 포인터를 리턴
//			없는 인덱스를 참조시 NULL 리턴
//************************************
ST_SETTING_PARAM CSetting::GetParam(int nIndexTab, int nIndexParam)
{	
	ST_SETTING_PARAM stParam;
	ST_SETTING_TAB stTab = GetTab(nIndexTab);
	if (0 > stTab.nIndexTab)							{ return stParam; }
	stParam = stTab.GetParam(nIndexParam);
	if (0 > stParam.nIndexParam)						{ return stParam; }

	return stParam;
}
ST_SETTING_PARAM CSetting::GetParam(int nIndexTab, CString strParam)
{	
	ST_SETTING_PARAM stParam;
	ST_SETTING_TAB stTab = GetTab(nIndexTab);
	if (0 > stTab.nIndexTab) { return stParam; }
	stParam = stTab.GetParam(strParam);
	if (0 > stParam.nIndexParam) { return stParam; }

	return stParam;
}
ST_SETTING_PARAM CSetting::GetParam(CString strTab, int nIndexParam)
{		
	ST_SETTING_PARAM stParam;
	ST_SETTING_TAB stTab = GetTab(strTab);
	if (0 > stTab.nIndexTab) { return stParam; }
	stParam = stTab.GetParam(nIndexParam);
	if (0 > stParam.nIndexParam) { return stParam; }

	return stParam;
}
ST_SETTING_PARAM CSetting::GetParam(CString strTab, CString strParam)
{
	ST_SETTING_PARAM stParam;
	ST_SETTING_TAB stTab = GetTab(strTab);
	if (0 > stTab.nIndexTab) { return stParam; }
	stParam = stTab.GetParam(strParam);
	if (0 > stParam.nIndexParam) { return stParam; }

	return stParam;
}

void CSetting::SetParam(int nIndexTab, int nIndexParam, ST_SETTING_PARAM stParam)
{
	ST_FIND_SETTING stFindTab(nIndexTab);
	auto itab = std::find_if(m_vTab.begin(), m_vTab.end(), stFindTab);
	if (itab == m_vTab.end()) return;

	ST_FIND_SETTING stFindParam(nIndexParam);
	auto iparam = std::find_if(itab->vParameter.begin(), itab->vParameter.end(), stFindParam);
	if (iparam == itab->vParameter.end()) return;

	iparam->Set(stParam);
}

void CSetting::SetParam(int nIndexTab, CString strParam, ST_SETTING_PARAM stParam)
{
	ST_FIND_SETTING stFindTab(nIndexTab);
	auto itab = std::find_if(m_vTab.begin(), m_vTab.end(), stFindTab);
	if (itab == m_vTab.end()) return;

	ST_FIND_SETTING stFindParam(strParam);
	auto iparam = std::find_if(itab->vParameter.begin(), itab->vParameter.end(), stFindParam);
	if (iparam == itab->vParameter.end()) return;

	iparam->Set(stParam);
}
void CSetting::SetParam(CString strTab, int nIndexParam, ST_SETTING_PARAM stParam)
{
	ST_FIND_SETTING stFindTab(strTab);
	auto itab = std::find_if(m_vTab.begin(), m_vTab.end(), stFindTab);
	if (itab == m_vTab.end()) return;

	ST_FIND_SETTING stFindParam(nIndexParam);
	auto iparam = std::find_if(itab->vParameter.begin(), itab->vParameter.end(), stFindParam);
	if (iparam == itab->vParameter.end()) return;

	iparam->Set(stParam);
}
void CSetting::SetParam(CString strTab, CString strParam, ST_SETTING_PARAM stParam)
{
	ST_FIND_SETTING stFindTab(strTab);
	auto itab = std::find_if(m_vTab.begin(), m_vTab.end(), stFindTab);
	if (itab == m_vTab.end()) return;

	ST_FIND_SETTING stFindParam(strParam);
	auto iparam = std::find_if(itab->vParameter.begin(), itab->vParameter.end(), stFindParam);
	if (iparam == itab->vParameter.end()) return;

	iparam->Set(stParam);
}

//************************************
// Method : GetParamIndex
// Date   : 2021/02/25
// Author : JBP
// Coment : 해당 파라미터의 인덱스를 리턴
//			없는 인덱스를 참조시 0 리턴
//************************************
int CSetting::GetParamIndex(int nIndexTab, CString strParam)
{		
	ST_SETTING_TAB stTab = GetTab(nIndexTab);
	if (0 > stTab.nIndexTab) { return DEF_INVALID_DATA; }
	
	ST_FIND_SETTING stFindParam(strParam);
	auto iparam = std::find_if(stTab.vParameter.begin(), stTab.vParameter.end(), stFindParam);
	if(iparam == stTab.vParameter.end()) { return DEF_INVALID_DATA; }

	return iparam->nIndexParam;
}
int CSetting::GetParamIndex(CString strTab, CString strParam)
{
	ST_SETTING_TAB stTab = GetTab(strTab);
	if (0 > stTab.nIndexTab) { return DEF_INVALID_DATA; }

	ST_FIND_SETTING stFindParam(strParam);
	auto iparam = std::find_if(stTab.vParameter.begin(), stTab.vParameter.end(), stFindParam);
	if (iparam == stTab.vParameter.end()) { return DEF_INVALID_DATA; }

	return iparam->nIndexParam;
}

//************************************
// Method : GetParamName
// Date   : 2021/02/25
// Author : JBP
// Coment : 해당 파라미터의 인덱스를 이름을 리턴
//			없는 인덱스를 참조시 _T("") 리턴
//************************************
CString CSetting::GetParamName(int nIndexTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return _T(""); }
	
	return stParam.strName;
}
CString CSetting::GetParamName(CString strTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return _T(""); }

	return stParam.strName;
}


CString CSetting::GetParamDataType(int nIndexTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return _T(""); }

	return stParam.strDataType;
}

CString	CSetting::GetParamDataType(int nIndexTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, strParam);
	if (0 > stParam.nIndexParam) { return _T(""); }

	return stParam.strDataType;
}

CString	CSetting::GetParamDataType(CString strTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return _T(""); }

	return stParam.strDataType;
}

CString	CSetting::GetParamDataType(CString strTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, strParam);
	if (0 > stParam.nIndexParam) { return _T(""); }

	return stParam.strDataType;
}

//************************************
// Method : GetCount
// Date   : 2021/02/25
// Author : JBP
// Coment : nArrCnt을 리턴한다.
//			Setting 문서에 같은이름이 연속적으로 N번 반복된다면 첫번째 파라미터는 nArrCnt = N개를 갖는다.(나머지파라미터는 1)
//	  예시) MARK X가 문서에 4개가 연속으로 있을경우 [strName, nArrCnt]은 아래와 같이 저장되어있다.
//			[MARK X , Cnt = 4] [MARK X2, Cnt = 1] [MARK X3, Cnt = 1] [MARK X4, Cnt = 1] 과같이 변경하여 저장한다.
//			없는 인덱스를 참조시 0 리턴
//************************************
int CSetting::GetCount(int nIndexTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return 0; }

	return stParam.nArrCnt;
}
int CSetting::GetCount(int nIndexTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, strParam);
	if (0 > stParam.nIndexParam) { return 0; }

	return stParam.nArrCnt;
}
int CSetting::GetCount(CString strTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return 0; }

	return stParam.nArrCnt;
}
int CSetting::GetCount(CString strTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, strParam);
	if (0 > stParam.nIndexParam) { return 0; }

	return stParam.nArrCnt;
}

CString	CSetting::GetDefaultValue(int nIndexTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return _T(""); }

	return stParam.GetDefaultValue();
}
CString	CSetting::GetDefaultValue(int nIndexTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, strParam);
	if (0 > stParam.nIndexParam) { return _T(""); }

	return stParam.GetDefaultValue();
}
CString	CSetting::GetDefaultValue(CString strTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return _T(""); }

	return stParam.GetDefaultValue();
}
CString	CSetting::GetDefaultValue(CString strTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, strParam);
	if (0 > stParam.nIndexParam) { return _T(""); }

	return stParam.GetDefaultValue();
}

int	CSetting::GetDefaultInt(int nIndexTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return DEF_INVALID_DATA; }

	return stParam.GetDefaultInt();
}
int	CSetting::GetDefaultInt(int nIndexTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, strParam);
	if (0 > stParam.nIndexParam) { return DEF_INVALID_DATA; }

	return stParam.GetDefaultInt();
}
int	CSetting::GetDefaultInt(CString strTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return DEF_INVALID_DATA; }

	return stParam.GetDefaultInt();
}
int	CSetting::GetDefaultInt(CString strTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, strParam);
	if (0 > stParam.nIndexParam) { return DEF_INVALID_DATA; }

	return stParam.GetDefaultInt();
}

double CSetting::GetDefaultDouble(int nIndexTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return DEF_INVALID_DATA; }

	return stParam.GetDefaultDouble();
}
double CSetting::GetDefaultDouble(int nIndexTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, strParam);
	if (0 > stParam.nIndexParam) { return DEF_INVALID_DATA; }

	return stParam.GetDefaultDouble();
}
double CSetting::GetDefaultDouble(CString strTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return DEF_INVALID_DATA; }

	return stParam.GetDefaultDouble();
}
double CSetting::GetDefaultDouble(CString strTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, strParam);
	if (0 > stParam.nIndexParam) { return DEF_INVALID_DATA; }

	return stParam.GetDefaultDouble();
}

//************************************
// Method : GetValue
// Date   : 2021/02/25
// Author : JBP
// Coment : 해당 파라미터의 값을 String으로 리턴한다.
//			없는 인덱스 참조시 _T("INVALID DATA"); 리턴
//************************************
CString	CSetting::GetValue(int nIndexTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return _T("INVALID DATA");; }

	return stParam.GetValue();
}
CString	CSetting::GetValue(int nIndexTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, strParam);
	if (0 > stParam.nIndexParam) { return _T("INVALID DATA");; }

	return stParam.GetValue();
}
CString	CSetting::GetValue(CString strTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return _T("INVALID DATA");; }

	return stParam.GetValue();
}
CString	CSetting::GetValue(CString strTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, strParam);
	if (0 > stParam.nIndexParam) { return _T("INVALID DATA");; }

	return stParam.GetValue();
}

CString	CSetting::GetValue(CString strParam)
{
	for (auto &tab : m_vTab)
	{
		for (auto& param : tab.vParameter)
		{
			if (0 == param.strName.Compare(strParam))
			{
				return param.GetValue();
			}
		}
	}

	return _T("INVALID");
}

//************************************
// Method : GetInt
// Date   : 2021/02/25
// Author : JBP
// Coment : 해당 파라미터의 값을 int로 리턴한다.
//			없는 인덱스 참조시 0 리턴
//************************************
int	CSetting::GetInt(int nIndexTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return DEF_INVALID_DATA; }

	return stParam.GetInt();	
}
int	CSetting::GetInt(int nIndexTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, strParam);
	if (0 > stParam.nIndexParam) { return DEF_INVALID_DATA; }

	return stParam.GetInt();
}
int	CSetting::GetInt(CString strTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return DEF_INVALID_DATA; }

	return stParam.GetInt();
}
int	CSetting::GetInt(CString strTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, strParam);
	if (0 > stParam.nIndexParam) { return DEF_INVALID_DATA; }

	return stParam.GetInt();
}

//************************************
// Method : GetDouble
// Date   : 2021/02/25
// Author : JBP
// Coment : 해당 파라미터의 값을 double로 리턴한다.
//			없는 인덱스 참조시 0 리턴
//************************************
double CSetting::GetDouble(int nIndexTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return DEF_INVALID_DATA; }

	return stParam.GetDouble();
}
double CSetting::GetDouble(int nIndexTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, strParam);
	if (0 > stParam.nIndexParam) { return DEF_INVALID_DATA; }

	return stParam.GetDouble();
}
double CSetting::GetDouble(CString strTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return DEF_INVALID_DATA; }

	return stParam.GetDouble();
}
double CSetting::GetDouble(CString strTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, strParam);
	if (0 > stParam.nIndexParam) { return DEF_INVALID_DATA; }

	return stParam.GetDouble();
}

BOOL CSetting::SetValue(int nIndexTab, int nIndexParam, CString strValue)
{	
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(strValue);
	SetParam(nIndexTab, stParam.nIndexParam, stParam);
	return bReturn;
}
BOOL CSetting::SetValue(int nIndexTab, CString strParam, CString strValue)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(strValue);
	SetParam(nIndexTab, stParam.nIndexParam, stParam);
	return bReturn;
}
BOOL CSetting::SetValue(CString strTab, int nIndexParam, CString strValue)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(strValue);
	SetParam(strTab, stParam.nIndexParam, stParam);
	return bReturn;
}
BOOL CSetting::SetValue(CString strTab, CString strParam, CString strValue)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(strValue);
	SetParam(strTab, stParam.nIndexParam, stParam);
	return bReturn;
}

BOOL CSetting::SetValue(int nIndexTab, int nIndexParam, int nValue)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(nValue);
	SetParam(nIndexTab, stParam.nIndexParam, stParam);
	return bReturn;
}
BOOL CSetting::SetValue(int nIndexTab, CString strParam, int nValue)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(nValue);
	SetParam(nIndexTab, stParam.nIndexParam, stParam);
	return bReturn;
}
BOOL CSetting::SetValue(CString strTab, int nIndexParam, int nValue)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(nValue);
	SetParam(strTab, stParam.nIndexParam, stParam);
	return bReturn;
}
BOOL CSetting::SetValue(CString strTab, CString strParam, int nValue)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(nValue);
	SetParam(strTab, stParam.nIndexParam, stParam);
	return bReturn;
}

BOOL CSetting::SetIncrease(int nIndexTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(stParam.GetInt() + 1);
	SetParam(nIndexTab, stParam.nIndexParam, stParam);
	return bReturn;
}
BOOL CSetting::SetIncrease(int nIndexTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(stParam.GetInt() + 1);
	SetParam(nIndexTab, stParam.nIndexParam, stParam);
	return bReturn;
}
BOOL CSetting::SetIncrease(CString strTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(stParam.GetInt() + 1);
	SetParam(strTab, stParam.nIndexParam, stParam);
	return bReturn;
}
BOOL CSetting::SetIncrease(CString strTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(stParam.GetInt() + 1);
	SetParam(strTab, stParam.nIndexParam, stParam);
	return bReturn;
}
BOOL CSetting::SetDecrease(int nIndexTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(stParam.GetInt() - 1);
	SetParam(nIndexTab, stParam.nIndexParam, stParam);
	return bReturn;
}
BOOL CSetting::SetDecrease(int nIndexTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(stParam.GetInt() - 1);
	SetParam(nIndexTab, stParam.nIndexParam, stParam);
	return bReturn;
}
BOOL CSetting::SetDecrease(CString strTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(stParam.GetInt() - 1);
	SetParam(strTab, stParam.nIndexParam, stParam);
	return bReturn;
}
BOOL CSetting::SetDecrease(CString strTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(stParam.GetInt() - 1);
	SetParam(strTab, stParam.nIndexParam, stParam);
	return bReturn;
}

BOOL CSetting::SetValue(int nIndexTab, int nIndexParam, double dValue)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(dValue);
	SetParam(nIndexTab, stParam.nIndexParam, stParam);
	return bReturn;
}
BOOL CSetting::SetValue(int nIndexTab, CString strParam, double dValue)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(dValue);
	SetParam(nIndexTab, stParam.nIndexParam, stParam);
	return bReturn;
}
BOOL CSetting::SetValue(CString strTab, int nIndexParam, double dValue)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(dValue);
	SetParam(strTab, stParam.nIndexParam, stParam);
	return bReturn;
}
BOOL CSetting::SetValue(CString strTab, CString strParam, double dValue)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	BOOL bReturn = stParam.SetValue(dValue);
	SetParam(strTab, stParam.nIndexParam, stParam);
	return bReturn;
}

BOOL CSetting::GetShow(int nIndexTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	return stParam.GetShow();
}
BOOL CSetting::GetShow(int nIndexTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	return stParam.GetShow();
}
BOOL CSetting::GetShow(CString strTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	return stParam.GetShow();
}
BOOL CSetting::GetShow(CString strTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	return stParam.GetShow();
}

BOOL CSetting::GetUse(int nIndexTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	return stParam.GetUse();
}
BOOL CSetting::GetUse(int nIndexTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(nIndexTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	return stParam.GetUse();
}
BOOL CSetting::GetUse(CString strTab, int nIndexParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, nIndexParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	return stParam.GetUse();
}
BOOL CSetting::GetUse(CString strTab, CString strParam)
{
	ST_SETTING_PARAM stParam = GetParam(strTab, strParam);
	if (0 > stParam.nIndexParam) { return FALSE; }

	return stParam.GetUse();
}

CString CSetting::GetCIMValue(CString strTab, CString strCIMName)
{
	int nMaxParam = GetParamCount(strTab);

	for (int nCnt = 0; nCnt < nMaxParam; nCnt++)
	{
		ST_SETTING_PARAM stParam = GetParam(strTab, nCnt);
		if (0 == strCIMName.Compare(stParam.strCIMName))
		{
			return stParam.GetScaledValue();
		}
	}

	return _T("INVALID DATA");;
}

BOOL CSetting::SetCIMValue(CString strTab, CString strCIMName, CString strValue)
{
	BOOL bReturn = FALSE;
	int nMaxParam = GetParamCount(strTab);
	for (int nCnt = 0; nCnt < nMaxParam; nCnt++)
	{
		ST_SETTING_PARAM stParam = GetParam(strTab, nCnt);
		if (0 == strCIMName.Compare(stParam.strCIMName))
		{
			bReturn = stParam.SetScaledValue(strValue);
			SetParam(strTab, stParam.nIndexParam, stParam);
			break;
		}
	}

	return bReturn;
}

BOOL CSetting::GetGroupList(int nTab, int nGroup, vSettingParam& stArrGroup)
{
	for (auto& tab : m_vTab)
	{
		if (tab.nIndexTab == nTab)
		{
			for (auto param : tab.vParameter)
			{
				if (param.nIndexGroup == nGroup)
				{
					stArrGroup.push_back(param);
				}
			}
		}
	}

	if (0 < stArrGroup.size())
	{
		return TRUE;
	}

	return FALSE;
}


BOOL CSetting::GetGroupList(CString strTab, CString strGroup, vSettingParam &stArrGroup)
{
	for (auto& tab : m_vTab)
	{
		if (0 == strTab.Compare(tab.strName))
		{
			for (auto param : tab.vParameter)
			{
				if (0 == strGroup.Compare(param.strGroup))
				{
					stArrGroup.push_back(param);
				}
			}
		}
	}

	if (0 < stArrGroup.size())
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CSetting::IsInt(CString strText)
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

BOOL CSetting::IsDouble(CString strText)
{
	strText.Remove(_T('-'));

	int nPeriod = 0;
	for (int nCnt = 0; nCnt < strText.GetLength(); nCnt++)
	{
		if ('0' > strText.GetAt(nCnt) || '9' < strText.GetAt(nCnt))
		{
			//마침표라면,
			if ('.' == strText.GetAt(nCnt))
			{
				//마침표가 한번만 등장하고, 맨앞, 맨뒤가 아니라면 한번은 소수점으로 판단
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