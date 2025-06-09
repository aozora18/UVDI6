#pragma once
#include "Recipe.h"

class CDlgMain;

enum EN_RECIPE_MODE
{
	eRECIPE_MODE_SEL = 0,
	eRECIPE_MODE_VIEW,
	eRECIPE_MODE_LOCAL,
	eRECIPE_MODE_MAX
};

enum EN_RECIPE_SELECT_TYPE
{
	eRECIPE_MODE_SEL_FROM_HOST,
	eRECIPE_MODE_SEL_FROM_INITIAL,
	eRECIPE_MODE_SEL_FROM_LOCAL,
	eRECIPE_MODE_SEL_MAX,
};


typedef struct ST_GRD_PARAM
{
	ST_GRD_PARAM()
	{
		bIsGroup = FALSE;
		strValue = _T("");
	}
	BOOL	bIsGroup;
	int		nIndexParam;
	CString strValue;
}ST_GRD_PARAM;

typedef CArray <ST_GRD_PARAM*, ST_GRD_PARAM*&> CArrGrdParam;
typedef CArray <CArrGrdParam*, CArrGrdParam*&> CArrGrdPage;

class CRecipeManager
{
private:
	CString					m_strRecipeName[eRECIPE_MODE_MAX];
	CString					m_strExpoRecipeName[eRECIPE_MODE_MAX];
	CString					m_strAlignRecipeName[eRECIPE_MODE_MAX];
	CRecipe					m_clsRcp[eRECIPE_MODE_MAX]; //0: Select 1:View
	
	CStringArray			m_strArrRecipeList;
	CString					m_strRecipePath;
	HWND					m_hMainWnd;
	CDlgMain* mainDlgPtr = nullptr;
public:
	//Save & Load 관련 함수
	CRecipe*			GetRecipe(EN_RECIPE_MODE eRecipeMode = eRECIPE_MODE_SEL);
	void				Init(HWND hWnd, CDlgMain* maindlgPtr);
	void				Destroy();
	CString				GetRecipePath();
	void				LoadRecipeList();
	void				LoadRecipeForm(EN_RECIPE_MODE eRecipeMode);
	BOOL				CreateRecipe(CString strRecipeName);
	BOOL				CreateRecipe(STG_RJAF stRecipe);
	BOOL				CreateExpoRecipe(CString strRecipeName);
	BOOL				CreateAlignRecipe(CString strRecipeName);
	BOOL				DeleteRecipe(CString strRecipeName);
	BOOL				DeleteExpoRecipe(CString strRecipeName);
	BOOL				DeleteAlignRecipe(CString strRecipeName);
	BOOL				SelectRecipe(CString strRecipeName, EN_RECIPE_SELECT_TYPE selType);
	BOOL				LoadSelectRecipe();
	void				SetRecipeName(CString strRecipeName, EN_RECIPE_MODE eRecipeMode = eRECIPE_MODE_SEL);
	CString				GetRecipeName(EN_RECIPE_MODE eRecipeMode = eRECIPE_MODE_SEL);
	void				SetExpoRecipeName(CString strRecipeName, EN_RECIPE_MODE eRecipeMode = eRECIPE_MODE_SEL);
	CString				GetExpoRecipeName(EN_RECIPE_MODE eRecipeMode = eRECIPE_MODE_SEL);
	void				SetAlignRecipeName(CString strRecipeName, EN_RECIPE_MODE eRecipeMode = eRECIPE_MODE_SEL);
	CString				GetAlignRecipeName(EN_RECIPE_MODE eRecipeMode = eRECIPE_MODE_SEL);
	BOOL				SaveRecipe(CString strName, EN_RECIPE_MODE eRecipeMode = eRECIPE_MODE_SEL);
	BOOL				UpdateRecipe(STG_RJAF& stRecipe, EN_RECIPE_MODE eRecipeMode = eRECIPE_MODE_SEL);
	BOOL				UpdateExpoRecipe(STG_REAF& stRecipe, EN_RECIPE_MODE eRecipeMode = eRECIPE_MODE_SEL);
	BOOL				UpdateAlignRecipe(STG_RAAF& stRecipe, EN_RECIPE_MODE eRecipeMode = eRECIPE_MODE_SEL);
	BOOL				LoadRecipe(CString strName, EN_RECIPE_MODE eRecipeMode = eRECIPE_MODE_SEL);
	BOOL				LoadExpoRecipe(CString strName, EN_RECIPE_MODE eRecipeMode = eRECIPE_MODE_SEL);
	BOOL				LoadAlignRecipe(CString strName, EN_RECIPE_MODE eRecipeMode = eRECIPE_MODE_SEL);
	BOOL				LoadRecipe(CString strJobName, CString strExpoName, CString strAlignName, EN_RECIPE_MODE eRecipeMode = eRECIPE_MODE_SEL);
	int					GetRecipeTabIndex(CString strTabName);
	int					GetRecipeIndex(CString strName);
	int					GetSelectRecipeIndex();
	CString				GetRecipeTabName(int nIndexTab);
	BOOL				GetLEDFrameRate();
	BOOL				CalcMarkDist();

	VOID				PhilSendCreateRecipe(LPG_RJAF stRecipe);
	VOID				PhilSendDeleteRecipe(CString strRecipeName);
	VOID				PhilSendModifyRecipe(LPG_RJAF stRecipe);
	VOID				PhilSendSelectRecipe(CString strRecipeName);
	

	//////////////////////////////////////////////////////////////////////////
	//Default
	BOOL				WriteRecipeName(CString strOldRecipeName, CString strNewRecipeName);


	//////////////////////////////////////////////////////////////////////////
	//View와 Select의 레시피 비교
	BOOL				CompareRecipeDifference(CStringArray& strArrTab, CStringArray& strArrParam);


	//////////////////////////////////////////////////////////////////////////
	//기타함수
	int					GetRecipeList(CStringArray& strArrRecipeList);
	int					GetRecipeCount() { return (int)m_strArrRecipeList.GetCount(); }

protected:
	CRecipeManager();
	CRecipeManager(const CRecipeManager &); // Prohibit Copy Constructor
	CRecipeManager& operator =(const CRecipeManager&);

public:
	virtual ~CRecipeManager(void);

	static CRecipeManager* GetInstance()
	{
		static CRecipeManager _inst;
		return &_inst;
	}
};


