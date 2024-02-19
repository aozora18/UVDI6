#include "pch.h"
#include "FileManager.h"
#include <locale.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CFileManager::CFileManager(CString strFilePath, CString strFileName, CString strFileExt, CString strBackupPath)
{
	m_strFilePath	= strFilePath; //파일 경로
	m_strFileName	= strFileName; //파일명
	m_strFileExt	= strFileExt; //파일확장자
	m_strBackupPath = strBackupPath;
	m_bOpen			= FALSE;
	m_bChanged		= FALSE;

	if (_T("CSV") == strFileExt.MakeUpper())
	{
		LoadCSV();
	}
	else
	{
		CString strMsg;
		//LANGUAGE 추가완료
		strMsg.Format(_T("[%s] 지원되지 않는 확장자입니다"), strFileExt);
		AfxMessageBox(strMsg);
	}	
}

CFileManager::~CFileManager(void)
{
	if (TRUE == m_bChanged)
	{
		Backup();

		if (_T("CSV") == m_strFileExt.MakeUpper())
		{			
			SaveCSV();
		}
		else
		{

		}
	}	
	
	DeleteGroup();
}



void CFileManager::DeleteGroup()
{
	for (auto& group : m_vGroup)
	{
		group.vParameter.clear();
		group.vParameter.shrink_to_fit();
	}
	m_vGroup.clear();
	m_vGroup.shrink_to_fit();
}


void CFileManager::LoadCSV()
{
	CString strValue;
	CString strLine;
	CString strPath;
	CStdioFile clsFile;
	CFileException clsFileExcept;
	int nCol = 0;

	setlocale(LC_ALL, "korean");	

	strPath.Format(_T("%s%s.%s"), m_strFilePath, m_strFileName, m_strFileExt);
	
	if(clsFile.Open(strPath, CStdioFile::modeRead | CStdioFile::shareDenyNone, &clsFileExcept))
	{
		while (clsFile.ReadString(strLine))
		{
			nCol = 0;

			//ReadGroup(Section) 그룹(섹션)을 읽는다.			
			if (!AfxExtractSubString(strValue, strLine, nCol++, ','))
			{
				break;
			}

			//같은 이름을 검색하여 인덱스를 받아온다.
			ST_FILE_MANAGER_GROUP stGroup;
			ST_FILE_MANAGER_PARAM stParam;

			stGroup.strGroup = strValue;
			AfxExtractSubString(strValue, strLine, nCol++, ',');
			stParam.strParam = strValue;
			AfxExtractSubString(strValue, strLine, nCol++, ',');
			stParam.strValue = strValue;

			BOOL bIsExist = FALSE;
			for (auto& group : m_vGroup)
			{
				if (group.strGroup == stGroup.strGroup)
				{
					group.vParameter.push_back(stParam);
					bIsExist = TRUE;
					break;
				}
			}

			if (FALSE == bIsExist)
			{
				stGroup.vParameter.push_back(stParam);
				m_vGroup.push_back(stGroup);
			}
		}

		clsFile.Close();
		m_bOpen = TRUE;
	}
	else
	{			
		//파일이 없을경우 신규저장으로 판단
		if (CFileException::fileNotFound == clsFileExcept.m_cause)
		{
			m_bOpen = TRUE;
		}
		else
		{
			//2021117 by Choi:  메세지를 안띄우는 방법?
// 			if (TRUE == _station->IsAnyModuleRun())
// 			{
// 				return;
// 			}

			CString strMsg;
			//LANGUAGE 추가완료
			strMsg.Format(_T("[%s] 파일 열기 실패"), strPath);
			AfxMessageBox(strMsg);
		}				
	}
}

//************************************
// Method : SaveCSV
// Date   : 2021/05/21
// Author : JBP
// Coment : 정상적으로 열리지 않았다면, 데이터는 초기값으로 되어있을것이므로 절대로!!! 저장하지 않는다.
//************************************
void CFileManager::SaveCSV()
{
	if (FALSE == m_bOpen) //정상적으로 열리지 않았다면, 데이터는 초기값으로 되어있을것이므로 절대로!!! 저장하지 않는다.
		return;

	CString strValue;
	CString strLine;
	CString strPath;
	CStdioFile clsFile;
	CFileException clsFileExcept;
	int nCol = 0;

	setlocale(LC_ALL, "korean");

	strPath.Format(_T("%s%s.%s"), m_strFilePath, m_strFileName, m_strFileExt);
	
	if(clsFile.Open(strPath, CStdioFile::modeCreate | CStdioFile::modeWrite, &clsFileExcept))
	{
		clsFile.SeekToBegin();

		for (const auto& group : m_vGroup)
		{
			for (const auto& param : group.vParameter)
			{
				strLine.Format(_T("%s,%s,%s\n"), group.strGroup, param.strParam, param.strValue);
				clsFile.WriteString(strLine);
			}
		}

		clsFile.Close();
	}
	else
	{
		//2021117 by Choi 메세지를 안띄우는 방법?
// 		if (TRUE == _station->IsAnyModuleRun())
// 		{
// 			return;
// 		}

		CString strMsg;
		//LANGUAGE 추가완료
		strMsg.Format(_T("[%s] 파일 저장 실패"), strPath);
		AfxMessageBox(strMsg);		
	}
}

//************************************
// Method : Backup
// Date   : 2021/05/24
// Author : JBP
// Coment : 백업경로를 받아오지않았다면 백업하지 않는다.
//************************************
void CFileManager::Backup()
{
	if (_T("") == m_strBackupPath)
		return;

	if (CheckFileExists(m_strBackupPath) == FALSE)
	{
		CreateDir(m_strBackupPath);
	}

	CString strFilePath;
	CString strNewFilePath;
	strFilePath.Format(_T("%s%s.%s"), m_strFilePath, m_strFileName, m_strFileExt);

	//NewFile(Backup)	
	CTime clsTime = CTime::GetCurrentTime();
	CString strTime;
	strTime.Format(_T("%04d%02d%02d_%02d%02d%02d"), clsTime.GetYear(), clsTime.GetMonth(), clsTime.GetDay(),
		clsTime.GetHour(), clsTime.GetMinute(), clsTime.GetSecond());
	strNewFilePath.Format(_T("%s%s_%s.%s"), m_strBackupPath, m_strFileName, strTime, m_strFileExt);

	CopyFile(strFilePath, strNewFilePath, FALSE);
}

void CFileManager::Write(CString strGroup, CString strParam, CString strData)
{
	BOOL bGroupFind = FALSE;
	BOOL bParamFind = FALSE;
	
	for (auto& group : m_vGroup)
	{
		if (0 == group.strGroup.Compare(strGroup))
		{
			bGroupFind = TRUE;
			for (auto& param : group.vParameter)
			{
				if (0 == param.strParam.Compare(strParam))
				{
					bParamFind = TRUE;

					if (0 != param.strValue.Compare(strData))
					{
						param.strValue = strData;
						m_bChanged = TRUE;
					}
					break;
				}
			}
		}

		if (TRUE == bParamFind)
			break;
		else
		{
			if (bGroupFind)
			{
				ST_FILE_MANAGER_PARAM stParam;
				stParam.strParam = strParam;
				stParam.strValue = strData;
				group.vParameter.push_back(stParam);
				m_bChanged = TRUE;
				break;
			}
		}
	}

	if (FALSE == bGroupFind)
	{
		ST_FILE_MANAGER_GROUP stGroup;
		stGroup.strGroup = strGroup;
		ST_FILE_MANAGER_PARAM stParam;
		stParam.strParam = strParam;
		stParam.strValue = strData;
		stGroup.vParameter.push_back(stParam);
		m_vGroup.push_back(stGroup);
		m_bChanged = TRUE;
	}
}

void CFileManager::Write(CString strGroup, CString strParam, int nData)
{
	CString strData;
	strData.Format(_T("%d"), nData);
		
	Write(strGroup, strParam, strData);
}

void CFileManager::Write(CString strGroup, CString strParam, short nData)
{
	CString strData;
	strData.Format(_T("%d"), nData);

	Write(strGroup, strParam, strData);
}

void CFileManager::Write(CString strGroup, CString strParam, double dData)
{
	CString strData;
	strData.Format(_T("%.3f"), dData);

	Write(strGroup, strParam, strData);
}

void CFileManager::Read(CString strGroup, CString strParam, CString &strData, CString strDefaultData /*= _T("")*/)
{	
	BOOL bGroupFind = FALSE;
	BOOL bParamFind = FALSE;

	for (auto& group : m_vGroup)
	{
		if (0 == group.strGroup.Compare(strGroup))
		{
			bGroupFind = TRUE;
			for (auto& param : group.vParameter)
			{
				if (0 == param.strParam.Compare(strParam))
				{
					bParamFind = TRUE;
					strData = param.strValue;
					break;
				}
			}
		}

		if (TRUE == bParamFind)
			break;
		else
		{
			if (bGroupFind)
			{
				strData = strDefaultData;
				ST_FILE_MANAGER_PARAM stParam;
				stParam.strParam = strParam;
				stParam.strValue = strData;
				group.vParameter.push_back(stParam);
				break;
			}
		}
	}

	if (FALSE == bGroupFind)
	{
		strData = strDefaultData;
		ST_FILE_MANAGER_GROUP stGroup;
		stGroup.strGroup = strGroup;
		ST_FILE_MANAGER_PARAM stParam;
		stParam.strParam = strParam;
		stParam.strValue = strData;
		stGroup.vParameter.push_back(stParam);
		m_vGroup.push_back(stGroup);
	}
}

void CFileManager::Read(CString strGroup, CString strParam, int &nData, int nDefaultData /*= 0*/)
{
	BOOL bGroupFind = FALSE;
	BOOL bParamFind = FALSE;

	for (auto& group : m_vGroup)
	{
		if (0 == group.strGroup.Compare(strGroup))
		{
			bGroupFind = TRUE;
			for (auto& param : group.vParameter)
			{
				if (0 == param.strParam.Compare(strParam))
				{
					bParamFind = TRUE;
					nData = _ttoi(param.strValue);
					break;
				}
			}
		}

		if (TRUE == bParamFind)
			break;
		else
		{
			if (bGroupFind)
			{
				nData = nDefaultData;
				ST_FILE_MANAGER_PARAM stParam;
				stParam.strParam = strParam;
				stParam.strValue.Format(_T("%d"), nData);
				group.vParameter.push_back(stParam);
				break;
			}
		}
	}

	if (FALSE == bGroupFind)
	{
		nData = nDefaultData;
		ST_FILE_MANAGER_GROUP stGroup;
		stGroup.strGroup = strGroup;
		ST_FILE_MANAGER_PARAM stParam;
		stParam.strParam = strParam;
		stParam.strValue.Format(_T("%d"), nData);
		stGroup.vParameter.push_back(stParam);
		m_vGroup.push_back(stGroup);
	}
}

void CFileManager::Read(CString strGroup, CString strParam, short &nData, int nDefaultData /*= 0*/)
{
	BOOL bGroupFind = FALSE;
	BOOL bParamFind = FALSE;

	for (auto& group : m_vGroup)
	{
		if (0 == group.strGroup.Compare(strGroup))
		{
			bGroupFind = TRUE;
			for (auto& param : group.vParameter)
			{
				if (0 == param.strParam.Compare(strParam))
				{
					bParamFind = TRUE;
					nData = _ttoi(param.strValue);
					break;
				}
			}
		}

		if (TRUE == bParamFind)
			break;
		else
		{
			if (bGroupFind)
			{
				nData = nDefaultData;
				ST_FILE_MANAGER_PARAM stParam;
				stParam.strParam = strParam;
				stParam.strValue.Format(_T("%d"), nData);
				group.vParameter.push_back(stParam);
				break;
			}
		}
	}

	if (FALSE == bGroupFind)
	{
		nData = nDefaultData;
		ST_FILE_MANAGER_GROUP stGroup;
		stGroup.strGroup = strGroup;
		ST_FILE_MANAGER_PARAM stParam;
		stParam.strParam = strParam;
		stParam.strValue.Format(_T("%d"), nData);
		stGroup.vParameter.push_back(stParam);
		m_vGroup.push_back(stGroup);
	}
}

void CFileManager::Read(CString strGroup, CString strParam, double &dData, double dDefaultData /*= 0.0*/)
{
	BOOL bGroupFind = FALSE;
	BOOL bParamFind = FALSE;

	for (auto& group : m_vGroup)
	{
		if (0 == group.strGroup.Compare(strGroup))
		{
			bGroupFind = TRUE;
			for (auto& param : group.vParameter)
			{
				if (0 == param.strParam.Compare(strParam))
				{
					bParamFind = TRUE;
					dData = _ttof(param.strValue);
					break;
				}
			}
		}

		if (TRUE == bParamFind)
			break;
		else
		{
			if (bGroupFind)
			{
				dData = dDefaultData;
				ST_FILE_MANAGER_PARAM stParam;
				stParam.strParam = strParam;
				stParam.strValue.Format(_T("%.4f"), dData);
				group.vParameter.push_back(stParam);
				break;
			}
		}
	}

	if (FALSE == bGroupFind)
	{
		dData = dDefaultData;
		ST_FILE_MANAGER_GROUP stGroup;
		stGroup.strGroup = strGroup;
		ST_FILE_MANAGER_PARAM stParam;
		stParam.strParam = strParam;
		stParam.strValue.Format(_T("%.4f"), dData);
		stGroup.vParameter.push_back(stParam);
		m_vGroup.push_back(stGroup);
	}
}

BOOL CFileManager::CheckFileExists(LPCTSTR szFileName)
{
	WIN32_FIND_DATA  data;
	HANDLE handle = FindFirstFile(szFileName, &data);
	if (handle != INVALID_HANDLE_VALUE)
	{
		FindClose(handle);
		return TRUE;
	}

	return FALSE;
}

void CFileManager::CreateDir(LPCTSTR Path)
{
	TCHAR DirName[MAX_PATH];
	TCHAR szPath[MAX_PATH];
	swprintf_s(szPath, MAX_PATH, _T("%s"), Path);

	TCHAR* p = szPath;
	TCHAR* q = DirName;
	while (*p)
	{
		if (('\\' == *p) || ('/' == *p))
		{
			if (':' != *(p - 1))
			{
				CreateDirectory(DirName, NULL);
			}
		}
		*q++ = *p++;
		*q = '\0';
	}
	CreateDirectory(DirName, NULL);
}