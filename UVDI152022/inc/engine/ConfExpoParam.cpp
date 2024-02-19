
/*
 desc : Engine Configuration for GEN2I - Exposure Parameter
*/

#include "pch.h"
#include "ConfExpoParam.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : config	- [in]  환경 정보
 retn : None
*/
CConfExpoParam::CConfExpoParam(LPG_CTEI config)
	: CConfBase(L"expo_para")
{
	m_pstCfg	= config;
	m_pstCfg->Reset();
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CConfExpoParam::~CConfExpoParam()
{
}

/*
 desc : 환경 파일 적재
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfExpoParam::LoadConfig()
{
	UINT8 u8Error		= 0x00;
	TCHAR tzMesg[1024]	= {NULL};
	TCHAR tzCode[6][16]	= { L"RECIPE", L"LURIA", L"MATRIX", L"EXPO_GERB", L"MEAS_GERB", L"RECIPE" };

	if (!LoadConfigTeachingTransRecipe())		{	u8Error = 0x01;	return FALSE;	}
	if (!LoadConfigTeachingExpoLuria())			{	u8Error = 0x02;	return FALSE;	}
	if (!LoadConfigTeachingMaxtrixExpo())		{	u8Error = 0x03;	return FALSE;	}
	if (!LoadConfigTeachingExpoGerberList())	{	u8Error = 0x04;	return FALSE;	}
	if (!LoadConfigTeachingMeasGerberList())	{	u8Error = 0x05;	return FALSE;	}
	if (!LoadConfigTeachingExpoRecipe())		{	u8Error = 0x06;	return FALSE;	}

	if (u8Error)
	{
		swprintf_s(tzMesg, 128, L"Failed to load the config for TEACHING (%s)", tzCode[u8Error-0x01]);
		AfxMessageBox(tzMesg, MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Teaching
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfExpoParam::SaveConfig()
{
	if (!SaveConfigTeachingTransRecipe())		return FALSE;
	if (!SaveConfigTeachingExpoLuria())			return FALSE;
	if (!SaveConfigTeachingMaxtrixExpo())		return FALSE;
	if (!SaveConfigTeachingExpoGerberList())	return FALSE;
	if (!SaveConfigTeachingMeasGerberList())	return FALSE;
	if (!SaveConfigTeachingExpoRecipe())		return FALSE;

	return TRUE;
}

/*
 desc : 환경 파일 적재 - Teaching - TRANSFORMATION RECIPE
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfExpoParam::LoadConfigTeachingTransRecipe()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"TRANSFORMATION_RECIPE");

	m_pstCfg->teach_pos.expo_set_rotation	= (INT32)ROUNDED(GetConfigDouble(L"SET_ROTATION") * 10000.0f, 0);
	m_pstCfg->teach_pos.expo_set_scale_x	= (INT32)ROUNDED(GetConfigDouble(L"SET_SCALE_X") * 10000.0f, 0);
	m_pstCfg->teach_pos.expo_set_scale_y	= (INT32)ROUNDED(GetConfigDouble(L"SET_SCALE_Y") * 10000.0f, 0);
	m_pstCfg->teach_pos.expo_set_offset_x	= (INT32)ROUNDED(GetConfigDouble(L"SET_OFFSET_X") * 10000.0f, 0);
	m_pstCfg->teach_pos.expo_set_offset_y	= (INT32)ROUNDED(GetConfigDouble(L"SET_OFFSET_Y") * 10000.0f, 0);
	m_pstCfg->teach_pos.expo_duty_cycle		= (INT32)GetConfigDouble(L"SET_DUTY_CYCLE");

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Teaching - TRANSFORMATION RECIPE
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfExpoParam::SaveConfigTeachingTransRecipe()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"TRANSFORMATION_RECIPE");

	SetConfigDouble(L"SET_ROTATION",	m_pstCfg->teach_pos.expo_set_rotation / 10000.0f, 4);
	SetConfigDouble(L"SET_SCALE_X",		m_pstCfg->teach_pos.expo_set_scale_x / 10000.0f, 4);
	SetConfigDouble(L"SET_SCALE_Y",		m_pstCfg->teach_pos.expo_set_scale_y / 10000.0f, 4);
	SetConfigDouble(L"SET_OFFSET_X",	m_pstCfg->teach_pos.expo_set_offset_x / 10000.0f, 4);
	SetConfigDouble(L"SET_OFFSET_Y",	m_pstCfg->teach_pos.expo_set_offset_y / 10000.0f, 4);
	SetConfigInt32(L"SET_DUTY_CYCLE",	m_pstCfg->teach_pos.expo_duty_cycle);

	return TRUE;
}

/*
 desc : 환경 파일 적재 - Teaching - EXPO_LURIA_SETTING RECIPE
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfExpoParam::LoadConfigTeachingExpoLuria()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"EXPO_LURIA_SETTING");

	m_pstCfg->teach_pos.expo_align			= GetConfigUint8(L"EXPO_ALIGN");
	m_pstCfg->teach_pos.expo_rectangle_lock	= GetConfigUint8(L"EXPO_RECTANGLE");
	m_pstCfg->teach_pos.expo_pos_delay_1	= (INT32)GetConfigDouble(L"EXPO_POS_DELAY_1");
	m_pstCfg->teach_pos.expo_neg_delay_1	= (INT32)GetConfigDouble(L"EXPO_NEG_DELAY_1");
	m_pstCfg->teach_pos.expo_pos_delay_2	= (INT32)GetConfigDouble(L"EXPO_POS_DELAY_2");
	m_pstCfg->teach_pos.expo_neg_delay_2	= (INT32)GetConfigDouble(L"EXPO_NEG_DELAY_2");
	m_pstCfg->teach_pos.expo_photo_height	= (INT32)ROUNDED(GetConfigDouble(L"EXPO_PHOTO_HEIGHT") * 10000.0f, 0);
	m_pstCfg->teach_pos.expo_neg_offset_1	= (INT32)ROUNDED(GetConfigDouble(L"EXPO_NEG_OFFSET_1") * 10000.0f, 0);
	m_pstCfg->teach_pos.expo_neg_offset_2	= (INT32)ROUNDED(GetConfigDouble(L"EXPO_NEG_OFFSET_2") * 10000.0f, 0);
	m_pstCfg->teach_pos.expo_start_pos_x	= (INT32)ROUNDED(GetConfigDouble(L"EXPO_START_POS_X") * 10000.0f, 0);
	m_pstCfg->teach_pos.expo_start_pos_y	= (INT32)ROUNDED(GetConfigDouble(L"EXPO_START_POS_Y") * 10000.0f, 0);

	m_pstCfg->teach_pos.expo_neg_offset_step	= (INT32)ROUNDED(GetConfigDouble(L"EXPO_NEG_OFFSET_STEP") * 10000.0f, 0);
	m_pstCfg->teach_pos.expo_pos_delay_step		= (INT32)GetConfigDouble(L"EXPO_POS_DELAY_STEP");
	m_pstCfg->teach_pos.expo_neg_delay_step		= (INT32)GetConfigDouble(L"EXPO_NEG_DELAY_STEP");
	m_pstCfg->teach_pos.expo_ph_offset_x_2		= (INT32)ROUNDED(GetConfigDouble(L"EXPO_PH_OFFSET_X_2") * 10000.0f, 0);
	m_pstCfg->teach_pos.expo_ph_offset_y_2		= (INT32)ROUNDED(GetConfigDouble(L"EXPO_PH_OFFSET_Y_2") * 10000.0f, 0);
	m_pstCfg->teach_pos.expo_ph_offset_x_step	= (INT32)ROUNDED(GetConfigDouble(L"EXPO_PH_OFFSET_X_STEP") * 10000.0f, 0);
	m_pstCfg->teach_pos.expo_ph_offset_y_step	= (INT32)ROUNDED(GetConfigDouble(L"EXPO_PH_OFFSET_Y_STEP") * 10000.0f, 0);

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Teaching - EXPO_LURIA_SETTING RECIPE
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfExpoParam::SaveConfigTeachingExpoLuria()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"EXPO_LURIA_SETTING");

	SetConfigInt32(L"EXPO_POS_DELAY_1",		m_pstCfg->teach_pos.expo_pos_delay_1);
	SetConfigInt32(L"EXPO_NEG_DELAY_1",		m_pstCfg->teach_pos.expo_neg_delay_1);
	SetConfigInt32(L"EXPO_POS_DELAY_2",		m_pstCfg->teach_pos.expo_pos_delay_2);
	SetConfigInt32(L"EXPO_NEG_DELAY_2",		m_pstCfg->teach_pos.expo_neg_delay_2);
	SetConfigUint32(L"EXPO_ALIGN",			m_pstCfg->teach_pos.expo_align);
	SetConfigUint32(L"EXPO_RECTANGLE",		m_pstCfg->teach_pos.expo_rectangle_lock);
	SetConfigDouble(L"EXPO_PHOTO_HEIGHT",	m_pstCfg->teach_pos.expo_photo_height / 10000.0f, 4);
	SetConfigDouble(L"EXPO_NEG_OFFSET_1",	m_pstCfg->teach_pos.expo_neg_offset_1 / 10000.0f, 4);
	SetConfigDouble(L"EXPO_NEG_OFFSET_2",	m_pstCfg->teach_pos.expo_neg_offset_2 / 10000.0f, 4);
	SetConfigDouble(L"EXPO_START_POS_X",	m_pstCfg->teach_pos.expo_start_pos_x / 10000.0f, 4);
	SetConfigDouble(L"EXPO_START_POS_Y",	m_pstCfg->teach_pos.expo_start_pos_y / 10000.0f, 4);

	SetConfigDouble(L"EXPO_NEG_OFFSET_STEP", m_pstCfg->teach_pos.expo_neg_offset_step / 10000.0f, 4);
	SetConfigInt32(L"EXPO_POS_DELAY_STEP", m_pstCfg->teach_pos.expo_pos_delay_step);
	SetConfigInt32(L"EXPO_NEG_DELAY_STEP", m_pstCfg->teach_pos.expo_neg_delay_step);
	SetConfigDouble(L"EXPO_PH_OFFSET_X_2", m_pstCfg->teach_pos.expo_ph_offset_x_2 / 10000.0f, 4);
	SetConfigDouble(L"EXPO_PH_OFFSET_Y_2", m_pstCfg->teach_pos.expo_ph_offset_y_2 / 10000.0f, 4);
	SetConfigDouble(L"EXPO_PH_OFFSET_X_STEP", m_pstCfg->teach_pos.expo_ph_offset_x_step / 10000.0f, 4);
	SetConfigDouble(L"EXPO_PH_OFFSET_Y_STEP", m_pstCfg->teach_pos.expo_ph_offset_y_step / 10000.0f, 4);

	return TRUE;
}

/*
 desc : 환경 파일 적재 - Teaching - Exposure Matrix
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfExpoParam::LoadConfigTeachingMaxtrixExpo()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"MATRIX_EXPO");

	m_pstCfg->teach_pos.expo_matrix_photo1_height		= (INT32)ROUNDED(GetConfigDouble(L"EXPO_MATRIX_Z1_HEIGHT") * 10000.0f, 0);
	m_pstCfg->teach_pos.expo_matrix_photo2_height		= (INT32)ROUNDED(GetConfigDouble(L"EXPO_MATRIX_Z2_HEIGHT") * 10000.0f, 0);
	m_pstCfg->teach_pos.expo_matrix_photo_height_step	= (INT32)ROUNDED(GetConfigDouble(L"EXPO_MATRIX_Z_HEIGHT_STEP") * 10000.0f, 0);
	m_pstCfg->teach_pos.expo_matrix_energy				= (INT32)ROUNDED(GetConfigDouble(L"EXPO_MATRIX_ENERGY")	* 10000.0f, 0);
	m_pstCfg->teach_pos.expo_matrix_energy_step			= (INT32)ROUNDED(GetConfigDouble(L"EXPO_MATRIX_ENERGY_STEP") * 10000.0f, 0);
	m_pstCfg->teach_pos.expo_matrix_start_pos_x			= (INT32)ROUNDED(GetConfigDouble(L"EXPO_MATRIX_START_POS_X") * 10000.0f, 0);
	m_pstCfg->teach_pos.expo_matrix_start_pos_y			= (INT32)ROUNDED(GetConfigDouble(L"EXPO_MATRIX_START_POS_Y") * 10000.0f, 0);
	m_pstCfg->teach_pos.serial_flip_h					= (UINT8)GetConfigUint8(L"EXPO_MATRIX_SERIAL_FLIP_H");
	m_pstCfg->teach_pos.serial_flip_v					= (UINT8)GetConfigUint8(L"EXPO_MATRIX_SERIAL_FLIP_V");
	m_pstCfg->teach_pos.scale_flip_h					= (UINT8)GetConfigUint8(L"EXPO_MATRIX_SCALE_FLIP_H");
	m_pstCfg->teach_pos.scale_flip_v					= (UINT8)GetConfigUint8(L"EXPO_MATRIX_SCALE_FLIP_V");
	GetConfigStr(L"EXPO_FOCUS_GERBER",	m_pstCfg->gerber_focus, MAX_GERBER_NAME, L"");
	GetConfigStr(L"EXPO_OFFSET_GERBER",	m_pstCfg->gerber_offset,MAX_GERBER_NAME, L"");

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Teaching - Exposure Matrix
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfExpoParam::SaveConfigTeachingMaxtrixExpo()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"MATRIX_EXPO");

	SetConfigStr(L"EXPO_FOCUS_GERBER",				m_pstCfg->gerber_focus);
	SetConfigStr(L"EXPO_OFFSET_GERBER",				m_pstCfg->gerber_offset);
	SetConfigDouble(L"EXPO_MATRIX_Z1_HEIGHT",		m_pstCfg->teach_pos.expo_matrix_photo1_height / 10000.0f, 4);
	SetConfigDouble(L"EXPO_MATRIX_Z2_HEIGHT",		m_pstCfg->teach_pos.expo_matrix_photo2_height / 10000.0f, 4);
	SetConfigDouble(L"EXPO_MATRIX_Z_HEIGHT_STEP",	m_pstCfg->teach_pos.expo_matrix_photo_height_step / 10000.0f, 4);
	SetConfigDouble(L"EXPO_MATRIX_ENERGY",			m_pstCfg->teach_pos.expo_matrix_energy / 10000.0f, 4);
	SetConfigDouble(L"EXPO_MATRIX_ENERGY_STEP",		m_pstCfg->teach_pos.expo_matrix_energy_step / 10000.0f, 4);
	SetConfigDouble(L"EXPO_MATRIX_START_POS_X",		m_pstCfg->teach_pos.expo_matrix_start_pos_x / 10000.0f, 4);
	SetConfigDouble(L"EXPO_MATRIX_START_POS_Y",		m_pstCfg->teach_pos.expo_matrix_start_pos_y / 10000.0f, 4);
	SetConfigUint32(L"EXPO_MATRIX_SERIAL_FLIP_H",	m_pstCfg->teach_pos.serial_flip_h);
	SetConfigUint32(L"EXPO_MATRIX_SERIAL_FLIP_V",	m_pstCfg->teach_pos.serial_flip_v);
	SetConfigUint32(L"EXPO_MATRIX_SCALE_FLIP_H",	m_pstCfg->teach_pos.scale_flip_h);
	SetConfigUint32(L"EXPO_MATRIX_SCALE_FLIP_V",	m_pstCfg->teach_pos.scale_flip_v);

	return TRUE;
}

/*
 desc : 환경 파일 적재 - Teaching - Exposure Gerber List
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfExpoParam::LoadConfigTeachingExpoGerberList()
{
	UINT32 i	= 0;
	TCHAR tzSub[64] = { NULL };
	CString strGerber;

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"EXPO_GERBER_LIST");
	
	/* 노광될 거버 이름 적재 */
	for (; i<m_pstCfg->gerb_expo_cnt; i++)
	{
		/* FOUP 1 or 2 */
		wmemset(m_pstCfg->gerber_expo[i], 0x00, MAX_GERBER_NAME);	
		swprintf_s(tzSub, 64, L"GERBER_NAME_%02d", i+1);
		GetConfigStr(tzSub, m_pstCfg->gerber_expo[i], MAX_GERBER_NAME, L"");
	}

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Teaching - Exposure Gerber List
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfExpoParam::SaveConfigTeachingExpoGerberList()
{
	UINT32 i	= 0;
	TCHAR tzSub[64] = { NULL };

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"EXPO_GERBER_LIST");

	/* 노광될 거버 이름 저장 */
	for (i=0; i<m_pstCfg->gerb_expo_cnt; i++)
	{
		swprintf_s(tzSub, 64, L"GERBER_NAME_%02d", i+1);
		SetConfigStr(tzSub, m_pstCfg->gerber_expo[i]);
	}

	return TRUE;
}

/*
 desc : 환경 파일 적재 - Teaching - Measure Gerber List
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfExpoParam::LoadConfigTeachingMeasGerberList()
{
	UINT32 i	= 0;
	TCHAR tzSub[64] = { NULL };

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"MEAS_GERBER_LIST");
	
	/* FOCUS 측정용 거버 파일 읽기 */
	for (; i<m_pstCfg->gerb_meas_cnt; i++)
	{
		/* FOUP 1 or 2 */
		wmemset(m_pstCfg->gerber_meas[i], 0x00, MAX_GERBER_NAME);
		swprintf_s(tzSub, 64, L"GERBER_MEAS_NAME_%02d", i+1);
		GetConfigStr(tzSub, m_pstCfg->gerber_meas[i], MAX_GERBER_NAME, L"");
	}

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Teaching - Measure Gerber List
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfExpoParam::SaveConfigTeachingMeasGerberList()
{
	UINT32 i	= 0;
	TCHAR tzSub[64] = { NULL };

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"MEAS_GERBER_LIST");

	/* 노광될 거버 이름 저장 */
	for (i=0; i<m_pstCfg->gerb_meas_cnt; i++)
	{
		swprintf_s(tzSub, 64, L"GERBER_MEAS_NAME_%02d", i + 1);
		SetConfigStr(tzSub, m_pstCfg->gerber_meas[i]);
	}

	return TRUE;
}

/*
 desc : 환경 파일 적재 - Teaching - Exposure Recipe
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfExpoParam::LoadConfigTeachingExpoRecipe()
{
	UINT32 i	= 0;
	TCHAR tzSub[64] = { NULL }, tzData[MAX_GERBER_NAME], tzTemp[32] = {NULL};

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"EXPO_RECIPE_LIST");
	
	/* 노광될 거버 Recipe 정보 적재 */
	for (i=1; i<m_pstCfg->teach_expo_cnt; i++)
	{
		wmemset(tzData, 0x00, MAX_GERBER_NAME);	
		swprintf_s(tzSub, 32, L"EXPO_RECIPE_%02d", i);
		GetConfigStr(tzSub, tzData, MAX_GERBER_NAME, L"");
		if (29 == wcslen(tzData)) // 21 // 27 // 33
		{
			wmemset(tzTemp, 0x00, 32);		wmemcpy(tzTemp, tzData, 2);
			m_pstCfg->teach_expo[i-1].gid	= UINT8(_wtoi(tzTemp));
			wmemset(tzTemp, 0x000, 32);		wmemcpy(tzTemp, tzData + 3, 3);
			m_pstCfg->teach_expo[i-1].mark1	= UINT8(_wtoi(tzTemp));
			wmemset(tzTemp, 0x000, 32);		wmemcpy(tzTemp, tzData + 7, 3);
			m_pstCfg->teach_expo[i-1].mark2	= UINT8(_wtoi(tzTemp));
			wmemset(tzTemp, 0x000, 32);		wmemcpy(tzTemp, tzData + 11, 3);
			m_pstCfg->teach_expo[i-1].material_thickness = UINT32(_wtoi(tzTemp));
			wmemset(tzTemp, 0x00, 32);		wmemcpy(tzTemp, tzData + 15, 6); // 16
			m_pstCfg->teach_expo[i-1].energy= DOUBLE(_wtof(tzTemp));
			wmemset(tzTemp, 0x00, 32);		wmemcpy(tzTemp, tzData + 22, 7);  // 23
			m_pstCfg->teach_expo[i-1].speed	= DOUBLE(_wtof(tzTemp));
		}
	}

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Teaching - Exposure Recipe
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfExpoParam::SaveConfigTeachingExpoRecipe()
{
	UINT32 i	= 0;
	TCHAR tzSub[64] = { NULL }, tzVal[128] = {NULL};

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"EXPO_RECIPE_LIST");

	/* 노광될 거버 이름 저장 */
	for (i=0; i<1; i++)
	{
		/* FOUP 1 or 2 */
		memset(&m_pstCfg->teach_expo[i], 0x00, sizeof(STG_EFWG));
		swprintf_s(tzSub, 64, L"EXPO_RECIPE_%02d", i+1);

		swprintf_s(tzVal, 128, L"%02d_%03d_%03d_%03d_%06.2f_%07.2f",
					m_pstCfg->teach_expo[i].gid,
					m_pstCfg->teach_expo[i].mark1,
					m_pstCfg->teach_expo[i].mark2,
					m_pstCfg->teach_expo[i].material_thickness,
					m_pstCfg->teach_expo[i].energy,
					m_pstCfg->teach_expo[i].speed);
		SetConfigStr(tzSub, tzVal);
	}

	return TRUE;
}
