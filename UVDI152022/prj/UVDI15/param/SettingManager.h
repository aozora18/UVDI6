#pragma once

#include "FileManager.h"
#include "Setting.h"

enum EN_SETTING_FILE_STATUS
{
	// STATUS
	// IDLE : ������
	// SAVE, WAIT : FILE SAVE�� �����Ѵ�.

	// STATUS CHANGE
	// IDLE -> SAVE : SAVE ��û�� ����
	// SAVE -> WAIT : SAVE ���ε� SAVE ��û ����
	// WAIT -> WAIT : WAIT�����϶��� SAVE ��û�� �޾Ƶ� WAIT��
	// WAIT -> SAVE : WAIT�����϶� SAVE�� �Ϸ�Ǹ� SAVE ���·� �Ѿ
	// SAVE -> IDLE : SAVE�����϶� SAVE�� �Ϸ�Ǹ� IDLE ���·� �Ѿ
	eSETTING_IDLE = 0,
	eSETTING_SAVE,
	eSETTING_WAIT,
};

class CSettingManager
{
private:
	CSetting			m_clsSetting; //0: Select 1:View	

	//DefaultSetting�� �ϴ� Manager���� ���� - Setting�� ������ �����, ������ ���ԵǴ� ������ �������� ����
	//DefaultSetting�� Setting������ ������ ���������, ������ ������ ���������� ���� ���� �����ϴ°����� ����
	//�ٸ�, �Ʒ��� �������� DefaultSetting�ν� ����ȭ���� ���γ�, Setting �������� �Űܰ��°Ϳ� ���� ����� �ʿ��� �� �ִ�.
	CString				m_strCurRecipe;
	CString				m_strOldRecipe;
	
	CString				m_strLoginLevel1;
	CString				m_strLoginLevel2;
	CString				m_strLoginLevel3;

	int					m_nScanStageMax;
	int					m_nScanCellMax;
	
	int					m_nStyleRecipeName;
	int					m_nStyleRecipeNameLength;

	CString					m_strSettingPath;
	
public:	
	//////////////////////////////////////////////////////////////////////////	
	//���������� �ʿ��Ѱ�� SaveRecipeName() �����ϼ���
	//Save���� �Լ��� ���鶧 ���������� ����� �����ɼ� ����. FileManager.h ����
	//////////////////////////////////////////////////////////////////////////
	//Save & Load ���� �Լ�	
	CSetting*			GetSetting();
	void				Init();
	void				InitAfterStation();
	void				LoadSettingForm();
	CString				CheckName(CString strName);
	BOOL				MakeSettingDefine();
	void				InitDirectory(); //20211222 JBP ���õ��丮 �߰�
	BOOL				Save(BOOL bBackup = TRUE);
	BOOL				Load();
	CString				GetSettingPath();
	BOOL				SaveBasic(CFileManager* pFileMgr);
	BOOL				LoadBasic(CFileManager* pFileMgr);
	CString				GetCurRecipeName();
	CString				GetOldRecipeName();

	//////////////////////////////////////////////////////////////////////////
	CString				GetLoginLevel1();
	CString				GetLoginLevel2();
	CString				GetLoginLevel3();




	//////////////////////////////////////////////////////////////////////////
	//20220110 taein : File Save Thread �߰�
	//////////////////////////////////////////////////////////////////////////
public:
	// Station ��� File Save�� ��û�Ѵ�.
	void						RequestSave();
	
	// Thread ���� ����
	CWinThread					*m_pThread;
	BOOL						m_bThreadRun;
	int							m_nThreadStatus;
	
	// Thread ���� �Լ�
	void						InitThread();
	void						StartThread();
	void						DestoryThread();
	int							GetThreadStatus() { return m_nThreadStatus; }

	//CMclCritSec					m_clsMclCritSec;

protected:
	CSettingManager();
	CSettingManager(const CSettingManager &); // Prohibit Copy Constructor
	CSettingManager& operator =(const CSettingManager&);

public:
	virtual ~CSettingManager(void);

	static CSettingManager* GetInstance()
	{
		static CSettingManager _inst;
		return &_inst;
	}
};

