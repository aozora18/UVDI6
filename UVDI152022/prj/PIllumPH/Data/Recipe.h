#pragma once

class CRecipe
{
/* Constructor & Destructor */
public:

	CRecipe();
	~CRecipe();

/* Local Paramter */
protected:


/* Local Function */
protected:

	BOOL				GetRecipeValue(CString data, CString &value);
	INT64				GetRecipeValueI(CString data);
	DOUBLE				GetRecipeValueF(CString data);


/* Public Function */
public:

	BOOL				LoadRecipe(PTCHAR name, LPG_MICL recipe);
	BOOL				SaveRecipe(PTCHAR name, LPG_MICL recipe);

};