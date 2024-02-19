#include "pch.h"
#include "SettingManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

UINT FileSaveThread(LPVOID lpParam)
{
	CSettingManager *pSettingMgr = (CSettingManager *)lpParam;
	pSettingMgr->m_bThreadRun = TRUE;

	while (pSettingMgr->m_bThreadRun)
	{
		if (eSETTING_IDLE != pSettingMgr->GetThreadStatus())
		{
			//20220111 JBP 백업 FALSE
			pSettingMgr->Save(FALSE);

			//pSettingMgr->DownThreadStatus();
// 			if (eSETTING_SAVE == pSettingMgr->GetThreadStatus())
// 				pSettingMgr->SetThreadStatus(eSETTING_IDLE);
// 			else if (eSETTING_WAIT == pSettingMgr->GetThreadStatus())
// 				pSettingMgr->SetThreadStatus(eSETTING_SAVE);
		}
		Sleep(100);
	}
	pSettingMgr->m_pThread = NULL;

	return 0;
}

CSettingManager::CSettingManager()
{
	//m_strSettingPath = _T("\\data\\setting\\");
	m_strSettingPath.Format(_T("\\%s\\setting\\"), CUSTOM_DATA_CONFIG);
	InitThread();
}

CSettingManager::~CSettingManager()
{
	DestoryThread();
}

CSetting* CSettingManager::GetSetting()
{
	return &m_clsSetting;
}

void CSettingManager::Init()
{
	//셋팅 디렉토리 생성 
	InitDirectory();
	LoadSettingForm();
	Load();

	//StartThread();
}

// 20221020 mhbaek Add : Seq Mode 기능 추가 건으로 인해 Station 접근 동작을 Init에서부터 분리
void CSettingManager::InitAfterStation()
{	

}

void CSettingManager::LoadSettingForm()
{
	m_clsSetting.LoadSettingForm();
}

//Space가 있으면 언더바로 변환하여 반환
CString CSettingManager::CheckName(CString strName)
{
	for (int nCnt = 0; nCnt < strName.GetLength(); nCnt++)
	{
		if (' ' == strName.GetAt(nCnt))
		{
			strName.SetAt(nCnt, '_');
		}
	}

	return strName;
}

BOOL CSettingManager::MakeSettingDefine()
{


	return TRUE;
}

void CSettingManager::InitDirectory()
{
	CFileStatus status;
	if (!CFile::GetStatus(GetSettingPath(), status))
		CreateDirectory(GetSettingPath(), NULL);
}

//************************************
BOOL CSettingManager::Save(BOOL bBackup)
{
	CString strBackupPath = _T("");
	if (TRUE == bBackup)
	{
		strBackupPath.Format(_T("%s%s"), GetSettingPath(), _T("BACKUP\\SETTING\\"));
	}
	CFileManager fileMgr(GetSettingPath(), _T("SETTING_DATA"), _T("CSV"));

	SaveBasic(&fileMgr);	
	return TRUE;
}


BOOL CSettingManager::Load()
{	
	CFileManager fileMgr(GetSettingPath(), _T("SETTING_DATA"), _T("CSV"));

	LoadBasic(&fileMgr);

	return TRUE;
}

CString CSettingManager::GetSettingPath()
{
	return g_tzWorkDir + m_strSettingPath;
}


BOOL CSettingManager::SaveBasic(CFileManager* pFileMgr)
{	
	CString strGroup;
	CString strParam;
	CString strValue;

	int nIndexScan = 0;
	int nMaxTab = GetSetting()->GetTabCount();
	for (int nCntTab = 0; nCntTab < nMaxTab; nCntTab++)
	{
		//사용하지 않는 탭은 건너 뛴다.
		if (FALSE == GetSetting()->GetTabUse(nCntTab))
			continue;

		int nMaxParam = GetSetting()->GetParamCount(nCntTab);
		strGroup = GetSetting()->GetTabName(nCntTab);
				
		for (int nCntParam = 0; nCntParam < nMaxParam; nCntParam++)
		{
			//사용하지 않는 파라미터는 저장하지 않는다.
			if (FALSE == GetSetting()->GetUse(nCntTab, nCntParam))
				continue;

			strParam = GetSetting()->GetParamName(nCntTab, nCntParam);
			strValue = GetSetting()->GetValue(nCntTab, nCntParam);
			
			//Section, Key, Data
			pFileMgr->Write(strGroup, strParam, strValue);
		}
	}
	return TRUE;
}


BOOL CSettingManager::LoadBasic(CFileManager* pFileMgr)
{		
	CString strGroup;
	CString strValue;
	
	int nIndexScan = 0;
	int nMaxTab = GetSetting()->GetTabCount();
	for (int nCntTab = 0; nCntTab < nMaxTab; nCntTab++)
	{
		//사용하지 않는 탭은 건너 뛴다.
		if (FALSE == GetSetting()->GetTabUse(nCntTab))
			continue;

		int nMaxParam = GetSetting()->GetParamCount(nCntTab);
		strGroup = GetSetting()->GetTabName(nCntTab);

		for (int nCntParam = 0; nCntParam < nMaxParam; nCntParam++)
		{
			ST_SETTING_PARAM stParam = GetSetting()->GetParam(nCntTab, nCntParam);
			//사용하지 않는 파라미터는 읽지 않는다.(LoadRecipeForm의 Value값이 Value,DefaultValue로 설정되어있는상태) 
			if (FALSE == stParam.bUse)
				continue;

			//Section, Key, Data, DefaultData
			pFileMgr->Read(strGroup, stParam.strName, strValue, stParam.strDefaultValue);
			//직접 값을 입력으로 변경 레시피를 읽어올때, GetParam(nCntTab, nCntParam)->strValue는 Default값으로 되어있는데
			//파일에서 읽어온 값과 Default값이 차이가 크면 changeLimit에 의해 값이 안들어감
			//GetSetting()->SetValue(nCntTab, nCntParam, strValue);
			stParam.strValue = strValue;

			GetSetting()->SetParam(nCntTab, nCntParam, stParam);
		}		
	}

	return TRUE;
}

CString CSettingManager::GetCurRecipeName()
{
	return m_strCurRecipe;
}

CString CSettingManager::GetOldRecipeName()
{
	return m_strOldRecipe;
}


CString	CSettingManager::GetLoginLevel1() 
{
	return m_strLoginLevel1;
}

CString	CSettingManager::GetLoginLevel2()
{
	return m_strLoginLevel2;
}

CString	CSettingManager::GetLoginLevel3()
{
	return m_strLoginLevel3;
}

void CSettingManager::RequestSave()
{
// 	if (eSETTING_IDLE == GetThreadStatus())
// 	{
// 		SetThreadStatus(eSETTING_SAVE);
// 	}
// 	else if (eSETTING_SAVE == GetThreadStatus())
// 	{
// 		SetThreadStatus(eSETTING_WAIT);
// 	}
}

void CSettingManager::InitThread()
{
	m_pThread = NULL;
	m_bThreadRun = FALSE;
	m_nThreadStatus = eSETTING_IDLE;
}

void CSettingManager::StartThread()
{
	m_bThreadRun = TRUE;
	m_pThread = AfxBeginThread(FileSaveThread, this);
}

void CSettingManager::DestoryThread()
{
	m_bThreadRun = FALSE;
	MSG   message;

	while (m_pThread != NULL)
	{
		if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
		Sleep(10);
	}

	return;
}
