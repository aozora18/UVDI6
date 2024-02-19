#pragma once

#include "FileManager.h"
#include "Setting.h"

enum EN_SETTING_FILE_STATUS
{
	// STATUS
	// IDLE : 대기상태
	// SAVE, WAIT : FILE SAVE를 수행한다.

	// STATUS CHANGE
	// IDLE -> SAVE : SAVE 요청을 받음
	// SAVE -> WAIT : SAVE 중인데 SAVE 요청 받음
	// WAIT -> WAIT : WAIT상태일때는 SAVE 요청을 받아도 WAIT임
	// WAIT -> SAVE : WAIT상태일때 SAVE가 완료되면 SAVE 상태로 넘어감
	// SAVE -> IDLE : SAVE상태일때 SAVE가 완료되면 IDLE 상태로 넘어감
	eSETTING_IDLE = 0,
	eSETTING_SAVE,
	eSETTING_WAIT,
};

class CSettingManager
{
private:
	CSetting			m_clsSetting; //0: Select 1:View	

	//DefaultSetting은 일단 Manager에서 관리 - Setting은 구조를 만들고, 구조에 포함되는 변수를 꺼내오는 역할
	//DefaultSetting도 Setting구조에 넣을까 고민했지만, 오히려 관리가 복잡해질것 같아 따로 관리하는것으로 선택
	//다만, 아래의 변수들이 DefaultSetting로써 구조화될지 여부나, Setting 안쪽으로 옮겨가는것에 대한 고민이 필요할 수 있다.
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
	//개별저장이 필요한경우 SaveRecipeName() 참고하세요
	//Save관련 함수를 만들때 수직구조로 만들면 문제될수 있음. FileManager.h 참고
	//////////////////////////////////////////////////////////////////////////
	//Save & Load 관련 함수	
	CSetting*			GetSetting();
	void				Init();
	void				InitAfterStation();
	void				LoadSettingForm();
	CString				CheckName(CString strName);
	BOOL				MakeSettingDefine();
	void				InitDirectory(); //20211222 JBP 셋팅디렉토리 추가
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
	//20220110 taein : File Save Thread 추가
	//////////////////////////////////////////////////////////////////////////
public:
	// Station 등에서 File Save를 요청한다.
	void						RequestSave();
	
	// Thread 관련 변수
	CWinThread					*m_pThread;
	BOOL						m_bThreadRun;
	int							m_nThreadStatus;
	
	// Thread 관련 함수
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

