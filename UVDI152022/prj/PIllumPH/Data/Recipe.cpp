
/*
 desc : The recipe for illuminance measurement
*/

#include "pch.h"
#include "../MainApp.h"
#include "Recipe.h"


#ifdef	_DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : Constructor
 parm : None
 retn : None
*/
CRecipe::CRecipe()
{
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CRecipe::~CRecipe()
{
}

/*
 desc : Load the recipe data
 parm : name	- [in]  The recipe file (Full Path)
		recipe	- [out] The structure pointer to store the values
 retn : None
*/
BOOL CRecipe::LoadRecipe(PTCHAR name, LPG_MICL recipe)
{
	UINT8 u8Index		= 0x00;
	INT32 i32Find		= -1;
	STG_MICL stMeas		= {NULL};
	CString strData;
	CStdioFile csFile;

	/* Open the file based on string */
	if (!csFile.Open(name, CFile::modeRead|CFile::typeText, NULL))
	{
		AfxMessageBox(L"Failed open the file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* Read the data one by one */
	while (csFile.ReadString(strData))
	{
		switch (u8Index)
		{
		case 0x00 : recipe->ph_no			= (UINT8)GetRecipeValueI(strData);	break;	/* Photohead Number (0x01 ~ 0x08) for Area/Focus/Measurement */
		case 0x01 : recipe->led_no			= (UINT8)GetRecipeValueI(strData);	break;	/* The measured LED Number (0x01 ~ 0x04, en_365nm ~ en_...) for Area/Focus Measurement */
		case 0x02 : recipe->mat_cols		= (UINT8)GetRecipeValueI(strData);	break;	/* Axis-X (Horz) Movement Count */
		case 0x03 : recipe->mat_rows		= (UINT8)GetRecipeValueI(strData);	break;	/* Axis-Y (Vert) Movement Count */
		case 0x04 : recipe->mat_updn		= (UINT8)GetRecipeValueI(strData);	break;	/* Axis-Z (UpDown) Movement Count */
		case 0x05 : recipe->drop_min_cnt[0]	= (UINT16)GetRecipeValueI(strData);	break;	/* The minimum number of discarded values among measured values (Area) */
		case 0x06 : recipe->drop_max_cnt[0]	= (UINT16)GetRecipeValueI(strData);	break;	/* The maximum number of discarded values among measured values (Area) */
		case 0x07 : recipe->pass_cnt[0]		= (UINT16)GetRecipeValueI(strData);	break;	/* This is a count to ignore when measuring LEDs (Area) */
		case 0x08 : recipe->valid_cnt[0]	= (UINT16)GetRecipeValueI(strData);	break;	/* Effective count when measuring LEDs (Area) */
		case 0x09 : recipe->drop_min_cnt[1]	= (UINT16)GetRecipeValueI(strData);	break;	/* The minimum number of discarded values among measured values (Focus) */
		case 0x0a : recipe->drop_max_cnt[1]	= (UINT16)GetRecipeValueI(strData);	break;	/* The maximum number of discarded values among measured values (Focus) */
		case 0x0b : recipe->pass_cnt[1]		= (UINT16)GetRecipeValueI(strData);	break;	/* This is a count to ignore when measuring LEDs (Focus) */
		case 0x0c : recipe->valid_cnt[1]	= (UINT16)GetRecipeValueI(strData);	break;	/* Effective count when measuring LEDs (Focus) */
		case 0x0d : recipe->drop_min_cnt[2]	= (UINT16)GetRecipeValueI(strData);	break;	/* The minimum number of discarded values among measured values (Power) */
		case 0x0e : recipe->drop_max_cnt[2]	= (UINT16)GetRecipeValueI(strData);	break;	/* The maximum number of discarded values among measured values (Power) */
		case 0x0f : recipe->pass_cnt[2]		= (UINT16)GetRecipeValueI(strData);	break;	/* This is a count to ignore when measuring LEDs (Power) */
		case 0x10 : recipe->valid_cnt[2]	= (UINT16)GetRecipeValueI(strData);	break;	/* Effective count when measuring LEDs (Power) */
		case 0x11 : recipe->sensor_rate[0]	= (DOUBLE)GetRecipeValueF(strData);	break;	/* Filter Magnification for illuminance measurement (Normal (365 nm) : 10x -> 1/10) */
		case 0x12 : recipe->sensor_rate[1]	= (DOUBLE)GetRecipeValueF(strData);	break;	/* Filter Magnification for illuminance measurement (Normal (385 nm) : 10x -> 1/10) */
		case 0x13 : recipe->sensor_rate[2]	= (DOUBLE)GetRecipeValueF(strData);	break;	/* Filter Magnification for illuminance measurement (Normal (395 nm) : 10x -> 1/10) */
		case 0x14 : recipe->sensor_rate[3]	= (DOUBLE)GetRecipeValueF(strData);	break;	/* Filter Magnification for illuminance measurement (Normal (405 nm) : 10x -> 1/10) */
		case 0x15 : recipe->amp_idx[0]		= (UINT16)GetRecipeValueI(strData);	break;	/* LED Power Index (0 ~ 4095) for Area Measurement */
		case 0x16 : recipe->amp_idx[1]		= (UINT16)GetRecipeValueI(strData);	break;	/* LED Power Index (0 ~ 4095) for Focus Measurement */
		case 0x17 : recipe->move_dist_col	= (DOUBLE)GetRecipeValueF(strData);	break;	/* Axis-X (Horz; Column) Movement interval (unit: mm) */
		case 0x18 : recipe->move_dist_row	= (DOUBLE)GetRecipeValueF(strData);	break;	/* Axis-Y (Vert; Row) Movement interval (unit: mm) */
		case 0x19 : recipe->move_dist_z		= (DOUBLE)GetRecipeValueF(strData);	break;	/* Axis-Z (Up/Down) Movement interval (unit: mm) */
		case 0x1a : recipe->correct			= (DOUBLE)GetRecipeValueF(strData);	break;	/* Illuminance value collected from sensor in idle sate */
		case 0x1b : recipe->cent_pos_x		= (DOUBLE)GetRecipeValueF(strData);	break;	/* measurement start X position (unit: mm) */
		case 0x1c : recipe->cent_pos_y		= (DOUBLE)GetRecipeValueF(strData);	break;	/* measurement start Y position (unit: mm) */
		case 0x1d : recipe->focus_pos_z		= (DOUBLE)GetRecipeValueF(strData);	break;	/* measurement focs Z position (unit: mm) */
		}
		u8Index++;
	}
	/* Close the file */
	csFile.Close();

	return (u8Index == 0x1e);
}

/*
 desc : Save the recipe data
 parm : name	- [in]  The recipe file Only
		recipe	- [in]  The structure pointer with stored values
 retn : TRUE or FALSE
*/
BOOL CRecipe::SaveRecipe(PTCHAR name, LPG_MICL recipe)
{
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	CString strValue;
	CStdioFile csFile;

	/* Set the path where the recipe file is stored */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\data\\recipe\\%s.im", g_tzWorkDir, name);
	/* Check if the same recipe file is exists */
	if (uvCmn_FindFile(tzFile))
	{
		if (IDYES != AfxMessageBox(L"The same recipe file Already exist\nShould I overwrite it?", MB_YESNO))
			return FALSE;
	}
	if (!recipe->IsValidArea() || !recipe->IsValidFocus())
	{
		AfxMessageBox(L"Invalid value exists", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* Save the recipe file */
	if (!csFile.Open(tzFile, CFile::modeCreate|CFile::modeWrite|CFile::typeText, NULL))
	{
		AfxMessageBox(L"Failed to create the recipe file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	strValue.Format(L"%-12u ; Photohead number (0x01 ~ 0x08) for Area/Focus/Measurement\n", recipe->ph_no);									csFile.WriteString(strValue);
	strValue.Format(L"%-12u ; Led number (0x01 ~ 0x04) for Area/Focus Measurement\n", recipe->led_no);										csFile.WriteString(strValue);
	strValue.Format(L"%-12u ; Axis-X (Horz; Column) Movement Count\n", recipe->mat_cols);													csFile.WriteString(strValue);
	strValue.Format(L"%-12u ; Axis-Y (Vert; Row) Movement Count\n", recipe->mat_rows);														csFile.WriteString(strValue);
	strValue.Format(L"%-12u ; Axis-Y (UpDown) Movement Count\n", recipe->mat_updn);															csFile.WriteString(strValue);
	strValue.Format(L"%-12u ; The min. number of discarded values among measured values - Area Measurement\n", recipe->drop_min_cnt[0]);	csFile.WriteString(strValue);
	strValue.Format(L"%-12u ; The max. number of discarded values among measured values - Area Measurement\n", recipe->drop_max_cnt[0]);	csFile.WriteString(strValue);
	strValue.Format(L"%-12u ; This is a count to ignore when measuring LEDs - Area Measurement\n", recipe->pass_cnt[0]);					csFile.WriteString(strValue);
	strValue.Format(L"%-12u ; Effective count when measuring LEDs - Area Measurement\n", recipe->valid_cnt[0]);								csFile.WriteString(strValue);
	strValue.Format(L"%-12u ; The min. number of discarded values among measured values - Focus Measurement\n", recipe->drop_min_cnt[1]);	csFile.WriteString(strValue);
	strValue.Format(L"%-12u ; The max. number of discarded values among measured values - Focus Measurement\n", recipe->drop_max_cnt[1]);	csFile.WriteString(strValue);
	strValue.Format(L"%-12u ; This is a count to ignore when measuring LEDs - Focus Measurement\n", recipe->pass_cnt[1]);					csFile.WriteString(strValue);
	strValue.Format(L"%-12u ; Effective count when measuring LEDs - Focus Measurement\n", recipe->valid_cnt[1]);							csFile.WriteString(strValue);
	strValue.Format(L"%-12u ; The min. number of discarded values among measured values - Power Measurement\n", recipe->drop_min_cnt[2]);	csFile.WriteString(strValue);
	strValue.Format(L"%-12u ; The max. number of discarded values among measured values - Power Measurement\n", recipe->drop_max_cnt[2]);	csFile.WriteString(strValue);
	strValue.Format(L"%-12u ; This is a count to ignore when measuring LEDs - Power Measurement\n", recipe->pass_cnt[2]);					csFile.WriteString(strValue);
	strValue.Format(L"%-12u ; Effective count when measuring LEDs - Power Measurement\n", recipe->valid_cnt[2]);							csFile.WriteString(strValue);
	strValue.Format(L"%-12.4f ; Filter Magnification (Factor : 365nm) (Normal : 10x -> 1/10)\n", recipe->sensor_rate[0]);					csFile.WriteString(strValue);
	strValue.Format(L"%-12.4f ; Filter Magnification (Factor : 385nm) (Normal : 10x -> 1/10)\n", recipe->sensor_rate[1]);					csFile.WriteString(strValue);
	strValue.Format(L"%-12.4f ; Filter Magnification (Factor : 395nm) (Normal : 10x -> 1/10)\n", recipe->sensor_rate[2]);					csFile.WriteString(strValue);
	strValue.Format(L"%-12.4f ; Filter Magnification (Factor : 405nm) (Normal : 10x -> 1/10)\n", recipe->sensor_rate[3]);					csFile.WriteString(strValue);
	strValue.Format(L"%-12u ; LED Power Index (0 ~ 4095) for Area Measurement\n", recipe->amp_idx[0]);										csFile.WriteString(strValue);
	strValue.Format(L"%-12u ; LED Power Index (0 ~ 4095) for Focus Measurement\n", recipe->amp_idx[1]);										csFile.WriteString(strValue);
	strValue.Format(L"%-12.4f ; Axis-X Horz Movement interval (Column) (unit: mm)\n", recipe->move_dist_col);								csFile.WriteString(strValue);
	strValue.Format(L"%-12.4f ; Axis-Y Horz Movement interval (Row) (unit: mm)\n", recipe->move_dist_row);									csFile.WriteString(strValue);
	strValue.Format(L"%-12.4f ; Axis-Z UpDn Movement interval (Row) (unit: mm)\n", recipe->move_dist_z);									csFile.WriteString(strValue);
	strValue.Format(L"%-12.6f ; Illum. value collected from sensor in idle sate (unit: W)\n", recipe->correct);								csFile.WriteString(strValue);
	strValue.Format(L"%-12.4f ; Measurement start X position (unit: mm)\n", recipe->cent_pos_x);											csFile.WriteString(strValue);
	strValue.Format(L"%-12.4f ; Measurement start Y position (unit: mm)\n", recipe->cent_pos_y);											csFile.WriteString(strValue);
	strValue.Format(L"%-12.4f ; Measurement start Z position (unit: mm)", recipe->focus_pos_z);												csFile.WriteString(strValue);

	/* Close the file */
	csFile.Close();

	return TRUE;
}

/*
 desc : Get the string data from the recipe file
 parm : data	- [in]  The recipe data based on string
 retn : Integer or Float value
*/
BOOL CRecipe::GetRecipeValue(CString data, CString &value)
{
	INT32 i32Find	= -1;

	/* Search for space or tab characters */
	i32Find	= data.Find(0x20/*space*/);
	if (i32Find < 0)
	{
		i32Find	= data.Find(0x09/*TAB*/);
		if (i32Find < 0)	return FALSE;
	}
	/* Extranct only value from the string */
	value	= data.Left(i32Find);
	/* Validate the value */
	return (value.GetLength() > 0);
}
INT64 CRecipe::GetRecipeValueI(CString data)
{
	CString strVal;
	if (!GetRecipeValue(data, strVal))	return 0;
	return _ttoi64(strVal);
}
DOUBLE CRecipe::GetRecipeValueF(CString data)
{
	CString strVal;
	if (!GetRecipeValue(data, strVal))	return 0;
	return _ttof(strVal);
}
