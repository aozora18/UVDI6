#pragma once

class CRecipeGen2i
{
/* ������ & �ı��� */
public:

	CRecipeGen2i(TCHAR *work_dir, LPG_CIEA config);
	virtual ~CRecipeGen2i();


/* ���� ���� */
protected:

	TCHAR				m_tzWorkDir[_MAX_PATH];

	CAtlList <LPG_RIGP>	m_lstRecipe;

	LPG_RIGP			m_pstSelRecipe;	/* ���� ���õ� ������ */
	LPG_CIEA			m_pstCfg;

/* ���� �Լ� */
protected:

	BOOL				ParseAppend(PTCHAR file_name);
	BOOL				RemoveRecipe(PTCHAR file_name);
	VOID				RecipeRemoveAll();
	VOID				CopyRecipe(LPG_RIGP source, LPG_RIGP target);

	VOID				SetConfigDouble(PTCHAR subj, PTCHAR key, DOUBLE val, UINT8 pts, PTCHAR file);

	BOOL				InvalidSetMesg(PTCHAR file, PTCHAR item);


/* ���� �Լ� */
public:

	LPG_RIGP			GetSelectRecipe()		{	return m_pstSelRecipe;	}

	BOOL				RecipeLoad();
	BOOL				RecipeSave(PTCHAR recipe);
	BOOL				RecipeDelete(PTCHAR recipe);

	BOOL				RecipeAppend(LPG_RIGP recipe);
	BOOL				RecipeModify(LPG_RIGP recipe);
	
	BOOL				RecipeFind(PTCHAR recipe);
	LPG_RIGP			GetRecipeName(PTCHAR recipe);
	LPG_RIGP			GetRecipeIndex(INT32 index);
	BOOL				SetRecipeName(PTCHAR recipe);
	
	VOID				ResetSelectRecipe()		{	m_pstSelRecipe	= NULL;	};
	UINT32				GetRecipeCount()		{ return (UINT32)m_lstRecipe.GetCount(); }
	VOID				CalcSpeedEnergy(LPG_RIGP recipe);
};
