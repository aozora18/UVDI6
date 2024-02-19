
/*
 desc : Engine Configuration for GEN2I - Teaching
*/

#include "pch.h"
#include "ConfTeaching.h"

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
CConfTeaching::CConfTeaching(LPG_CTPI config)
	: CConfBase(L"teaching")
{
	m_pstCfg	= config;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CConfTeaching::~CConfTeaching()
{
}

/*
 desc : 환경 파일 적재
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::LoadConfig()
{
	UINT8 u8Error		= 0x00;
	TCHAR tzMesg[1024]	= {NULL};
	TCHAR tzCode[11][16]= { L"AUTO", L"ZPOS", L"OFFSET", L"VELO", L"ILLUM", L"CENTERING",
							L"LIGHT", L"EQUIP", L"IN_LOCK", L"RECIPE", L"LURIA" };

	if (!LoadConfigTeachingAutoRun())		{	u8Error = 0x01;	return FALSE;	}
	if (!LoadConfigTeachingZPos())			{	u8Error = 0x02;	return FALSE;	}
	if (!LoadConfigTeachingOffset())		{	u8Error = 0x03;	return FALSE;	}
	if (!LoadConfigTeachingVelo())			{	u8Error = 0x04;	return FALSE;	}
	if (!LoadConfigTeachingIllum())			{	u8Error = 0x05;	return FALSE;	}
	if (!LoadConfigTeachingCentering())		{	u8Error = 0x06;	return FALSE;	}
	if (!LoadConfigTeachingLight())			{	u8Error = 0x07;	return FALSE;	}
	if (!LoadConfigTeachingEquipment())		{	u8Error = 0x08;	return FALSE;	}
	if (!LoadConfigTeachingInterlock())		{	u8Error = 0x09;	return FALSE;	}
	if (!LoadConfigTeachingTransRecipe())	{	u8Error = 0x0a;	return FALSE;	}
	if (!LoadConfigTeachingExpoLuria())		{	u8Error = 0x0b;	return FALSE;	}

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
BOOL CConfTeaching::SaveConfig()
{
	if (!SaveConfigTeachingAutoRun())		return FALSE;
	if (!SaveConfigTeachingZPos())			return FALSE;
	if (!SaveConfigTeachingOffset())		return FALSE;
	if (!SaveConfigTeachingVelo())			return FALSE;
	if (!SaveConfigTeachingIllum())			return FALSE;
	if (!SaveConfigTeachingCentering())		return FALSE;
	if (!SaveConfigTeachingLight())			return FALSE;
	if (!SaveConfigTeachingEquipment())		return FALSE;
	if (!SaveConfigTeachingInterlock())		return FALSE;
	if (!SaveConfigTeachingTransRecipe())	return FALSE;
	if (!SaveConfigTeachingExpoLuria())		return FALSE;

	return TRUE;
}

/*
 desc : 환경 파일 적재 - Teaching - AUTO
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::LoadConfigTeachingAutoRun()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"AUTO");

	/* auto run */
	m_pstCfg->stage_y_acc_dist		= GetConfigUint32(L"STAGEY_ACC_DIST");
	m_pstCfg->expo_start_x_pos		= (INT32)ROUNDED(GetConfigDouble(L"EXPO_START_X") * 10000.0f, 0);
	m_pstCfg->expo_start_y_pos		= (INT32)ROUNDED(GetConfigDouble(L"EXPO_START_Y") * 10000.0f, 0);
	m_pstCfg->gerber_0_x_pos		= (INT32)ROUNDED(GetConfigDouble(L"GERBER_0_X") * 10000.0f, 0);
	m_pstCfg->gerber_0_y_pos		= (INT32)ROUNDED(GetConfigDouble(L"GERBER_0_Y") * 10000.0f, 0);
	m_pstCfg->load_x_pos			= (INT32)ROUNDED(GetConfigDouble(L"LOAD_X") * 10000.0f, 0);
	m_pstCfg->load_y_pos			= (INT32)ROUNDED(GetConfigDouble(L"LOAD_Y") * 10000.0f, 0);
	m_pstCfg->unload_x_pos			= (INT32)ROUNDED(GetConfigDouble(L"UNLOAD_X") * 10000.0f, 0);
	m_pstCfg->unload_y_pos			= (INT32)ROUNDED(GetConfigDouble(L"UNLOAD_Y") * 10000.0f, 0);
	m_pstCfg->orgin_x_pos			= (INT32)ROUNDED(GetConfigDouble(L"ORIGIN_X") * 10000.0f, 0);
	m_pstCfg->orgin_y_pos			= (INT32)ROUNDED(GetConfigDouble(L"ORIGIN_Y") * 10000.0f, 0);

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Teaching - AUTO
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::SaveConfigTeachingAutoRun()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"AUTO");

	SetConfigUint32(L"STAGEY_ACC_DIST",			m_pstCfg->stage_y_acc_dist);
	SetConfigDouble(L"EXPO_START_X",			m_pstCfg->expo_start_x_pos / 10000.0f, 4);
	SetConfigDouble(L"EXPO_START_Y",			m_pstCfg->expo_start_y_pos / 10000.0f, 4);
	SetConfigDouble(L"GERBER_0_X",				m_pstCfg->gerber_0_x_pos / 10000.0f, 4);
	SetConfigDouble(L"GERBER_0_Y",				m_pstCfg->gerber_0_y_pos / 10000.0f, 4);
	SetConfigDouble(L"LOAD_X",					m_pstCfg->load_x_pos / 10000.0f, 4);
	SetConfigDouble(L"LOAD_Y",					m_pstCfg->load_y_pos / 10000.0f, 4);
	SetConfigDouble(L"UNLOAD_X",				m_pstCfg->unload_x_pos / 10000.0f, 4);
	SetConfigDouble(L"UNLOAD_Y",				m_pstCfg->unload_y_pos / 10000.0f, 4);
	SetConfigDouble(L"ORIGIN_X",				m_pstCfg->orgin_x_pos / 10000.0f, 4);
	SetConfigDouble(L"ORIGIN_Y",				m_pstCfg->orgin_y_pos / 10000.0f, 4);

	return TRUE;
}

/*
 desc : 환경 파일 적재 - Teaching - Z_POS
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::LoadConfigTeachingZPos()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"Z_POS");

	/* z position (focus & 동작제어) */
	m_pstCfg->focus_quaz_bsa_z_pos	= (INT32)ROUNDED(GetConfigDouble(L"FOCUS_QUARTZ_BSA_Z") * 10000.0f, 0);
	m_pstCfg->focus_mark1_bsa_z_pos	= (INT32)ROUNDED(GetConfigDouble(L"FOCUS_MARK1_BSA_Z") * 10000.0f, 0);
	m_pstCfg->focus_mark2_bsa_z_pos	= (INT32)ROUNDED(GetConfigDouble(L"FOCUS_MARK2_BSA_Z") * 10000.0f, 0);
	m_pstCfg->idle_bsa_z_pos		= (INT32)ROUNDED(GetConfigDouble(L"IDLE_BSA_Z") * 10000.0f, 0);
	m_pstCfg->focus_quaz_up_z_pos	= (INT32)ROUNDED(GetConfigDouble(L"FOCUS_QUARTZ_UP_Z") * 10000.0f, 0);
	m_pstCfg->focus_mark1_up_z_pos	= (INT32)ROUNDED(GetConfigDouble(L"FOCUS_MARK1_UP_Z") * 10000.0f, 0);
	m_pstCfg->focus_mark2_up_z_pos	= (INT32)ROUNDED(GetConfigDouble(L"FOCUS_MARK2_UP_Z") * 10000.0f, 0);
	m_pstCfg->pin_stage_z_up		= (INT32)ROUNDED(GetConfigDouble(L"PIN_STAGE_Z_UP") * 10000.0f, 0);
	m_pstCfg->pin_stage_z_down		= (INT32)ROUNDED(GetConfigDouble(L"PIN_STAGE_Z_DOWN") * 10000.0f, 0);
	m_pstCfg->pin_stage_x_input		= (INT32)ROUNDED(GetConfigDouble(L"PIN_STAGE_X_INPUT") * 10000.0f, 0);
	m_pstCfg->pin_stage_x_output	= (INT32)ROUNDED(GetConfigDouble(L"PIN_STAGE_X_OUTPUT")	* 10000.0f, 0);

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Teaching - Z_POS
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::SaveConfigTeachingZPos()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"Z_POS");

	SetConfigDouble(L"FOCUS_QUARTZ_BSA_Z",	m_pstCfg->focus_quaz_bsa_z_pos / 10000.0f, 4);
	SetConfigDouble(L"FOCUS_MARK1_BSA_Z",	m_pstCfg->focus_mark1_bsa_z_pos / 10000.0f, 4);
	SetConfigDouble(L"FOCUS_MARK2_BSA_Z",	m_pstCfg->focus_mark2_bsa_z_pos / 10000.0f, 4);
	SetConfigDouble(L"IDLE_BSA_Z",			m_pstCfg->idle_bsa_z_pos / 10000.0f, 4);
	SetConfigDouble(L"FOCUS_QUARTZ_UP_Z",	m_pstCfg->focus_quaz_up_z_pos / 10000.0f, 4);
	SetConfigDouble(L"FOCUS_MARK1_UP_Z",	m_pstCfg->focus_mark1_up_z_pos / 10000.0f, 4);
	SetConfigDouble(L"FOCUS_MARK2_UP_Z",	m_pstCfg->focus_mark2_up_z_pos / 10000.0f, 4);
	SetConfigDouble(L"PIN_STAGE_Z_UP",		m_pstCfg->pin_stage_z_up / 10000.0f, 4);
	SetConfigDouble(L"PIN_STAGE_Z_DOWN",	m_pstCfg->pin_stage_z_down / 10000.0f, 4);
	SetConfigDouble(L"PIN_STAGE_X_INPUT",	m_pstCfg->pin_stage_x_input / 10000.0f, 4);
	SetConfigDouble(L"PIN_STAGE_X_OUTPUT",	m_pstCfg->pin_stage_x_output / 10000.0f, 4);

	return TRUE;
}

/*
 desc : 환경 파일 적재 - Teaching - OFFSET
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::LoadConfigTeachingOffset()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"OFFSET");

	m_pstCfg->expo_offset_x				= (INT32)ROUNDED(GetConfigDouble(L"EXPO_OFFSET_X") * 10000.0f, 0);
	m_pstCfg->expo_offset_y				= (INT32)ROUNDED(GetConfigDouble(L"EXPO_OFFSET_Y") * 10000.0f, 0);
	m_pstCfg->cam_offset_x				= (INT32)ROUNDED(GetConfigDouble(L"CAM_OFFSET_X") * 10000.0f, 0);
	m_pstCfg->cam_offset_y				= (INT32)ROUNDED(GetConfigDouble(L"CAM_OFFSET_Y") * 10000.0f, 0);
	m_pstCfg->bsa_up_down_offset_x		= (INT32)ROUNDED(GetConfigDouble(L"BSA_UP_DOWN_OFFSET_X") * 10000.0f, 0);
	m_pstCfg->bsa_up_down_offset_y		= (INT32)ROUNDED(GetConfigDouble(L"BSA_UP_DOWN_OFFSET_Y") * 10000.0f, 0);
	m_pstCfg->quartz_center_offset_x	= (INT32)ROUNDED(GetConfigDouble(L"QUARTZ_CENTER_OFFSET_X")	* 10000.0f, 0);
	m_pstCfg->quartz_center_offset_y	= (INT32)ROUNDED(GetConfigDouble(L"QUARTZ_CENTER_OFFSET_Y")	* 10000.0f, 0);
	m_pstCfg->offset_mark1_x			= (INT32)ROUNDED(GetConfigDouble(L"MARK1_OFFSET_X") * 10000.0f, 0);
	m_pstCfg->offset_mark1_y			= (INT32)ROUNDED(GetConfigDouble(L"MARK1_OFFSET_Y") * 10000.0f, 0);
	m_pstCfg->offset_mark2_x			= (INT32)ROUNDED(GetConfigDouble(L"MARK2_OFFSET_X") * 10000.0f, 0);
	m_pstCfg->offset_mark2_y			= (INT32)ROUNDED(GetConfigDouble(L"MARK2_OFFSET_Y") * 10000.0f, 0);
	m_pstCfg->focus_ph_meas_height		= (INT32)ROUNDED(GetConfigDouble(L"FOCUS_PH_MEASUREMENT") * 10000.0f, 0);
	m_pstCfg->material_thick			= (INT32)ROUNDED(GetConfigDouble(L"MATERIAL_THICK") * 10000.0f, 0);
	m_pstCfg->focus_ld_sensor_p1		= (DOUBLE)ROUNDED(GetConfigDouble(L"FOCUS_LD_VALUE_P1") , 2);
	m_pstCfg->focus_ld_sensor_p2		= (DOUBLE)ROUNDED(GetConfigDouble(L"FOCUS_LD_VALUE_P2") , 2);
	m_pstCfg->focus_ld_sensor_p3		= (DOUBLE)ROUNDED(GetConfigDouble(L"FOCUS_LD_VALUE_P3") , 2);
	m_pstCfg->focus_ld_sensor_avg		= (DOUBLE)ROUNDED(GetConfigDouble(L"FOCUS_LD_VALUE_AVG") , 4);

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Teaching - OFFSET
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::SaveConfigTeachingOffset()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"OFFSET");

	SetConfigDouble(L"EXPO_OFFSET_X",			m_pstCfg->expo_offset_x / 10000.0f, 4);
	SetConfigDouble(L"EXPO_OFFSET_Y",			m_pstCfg->expo_offset_y / 10000.0f, 4);
	SetConfigDouble(L"CAM_OFFSET_X",			m_pstCfg->cam_offset_x / 10000.0f, 4);
	SetConfigDouble(L"CAM_OFFSET_Y",			m_pstCfg->cam_offset_y / 10000.0f, 4);
	SetConfigDouble(L"BSA_UP_DOWN_OFFSET_X",	m_pstCfg->bsa_up_down_offset_x / 10000.0f, 4);
	SetConfigDouble(L"BSA_UP_DOWN_OFFSET_Y",	m_pstCfg->bsa_up_down_offset_y / 10000.0f, 4);
	SetConfigDouble(L"QUARTZ_CENTER_OFFSET_X",	m_pstCfg->quartz_center_offset_x / 10000.0f, 4);
	SetConfigDouble(L"QUARTZ_CENTER_OFFSET_Y",	m_pstCfg->quartz_center_offset_y / 10000.0f, 4);
	SetConfigDouble(L"MARK1_OFFSET_X",			m_pstCfg->offset_mark1_x / 10000.0f, 4);
	SetConfigDouble(L"MARK1_OFFSET_Y",			m_pstCfg->offset_mark1_y / 10000.0f, 4);
	SetConfigDouble(L"MARK2_OFFSET_X",			m_pstCfg->offset_mark2_x / 10000.0f, 4);
	SetConfigDouble(L"MARK2_OFFSET_Y",			m_pstCfg->offset_mark2_y / 10000.0f, 4);
	SetConfigDouble(L"FOCUS_PH_MEASUREMENT",	m_pstCfg->focus_ph_meas_height / 10000.0f, 4);
	SetConfigUint32(L"MATERIAL_THICK",			m_pstCfg->material_thick);
	SetConfigDouble(L"FOCUS_LD_VALUE_P1",		m_pstCfg->focus_ld_sensor_p1, 2);
	SetConfigDouble(L"FOCUS_LD_VALUE_P2",		m_pstCfg->focus_ld_sensor_p2, 2);
	SetConfigDouble(L"FOCUS_LD_VALUE_P3",		m_pstCfg->focus_ld_sensor_p3, 2);
	SetConfigDouble(L"FOCUS_LD_VALUE_AVG",		m_pstCfg->focus_ld_sensor_avg, 4);

	return TRUE;
}

/*
 desc : 환경 파일 적재 - Teaching - VELO
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::LoadConfigTeachingVelo()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"VELO");

	m_pstCfg->velo_align_stage	= (UINT32)ROUNDED(GetConfigUint32(L"VELO_ALIGN_STAGE") * 100.0f, 0);
	m_pstCfg->velo_align_bsa	= (UINT32)ROUNDED(GetConfigUint32(L"VELO_ALIGN_BSA") * 100.0f, 0);
	m_pstCfg->velo_align_pin_x	= (UINT32)ROUNDED(GetConfigUint32(L"VELO_ALIGN_PIN_X") * 100.0f, 0);
	m_pstCfg->velo_align_pin_z	= (UINT32)ROUNDED(GetConfigUint32(L"VELO_ALIGN_PIN_Z") * 100.0f, 0);

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Teaching - VELO
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::SaveConfigTeachingVelo()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"VELO");

	SetConfigDouble(L"VELO_ALIGN_STAGE",	m_pstCfg->velo_align_stage / 100.0f, 2);
	SetConfigDouble(L"VELO_ALIGN_BSA",		m_pstCfg->velo_align_bsa / 100.0f, 2);
	SetConfigDouble(L"VELO_ALIGN_PIN_X",	m_pstCfg->velo_align_pin_x / 100.0f, 2);
	SetConfigDouble(L"VELO_ALIGN_PIN_Z",	m_pstCfg->velo_align_pin_z / 100.0f, 2);

	return TRUE;
}

/*
 desc : 환경 파일 적재 - Teaching - VELO
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::LoadConfigTeachingIllum()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"ILLUM");

	m_pstCfg->illum_ph_x_axis_pos[1]= (INT32)ROUNDED(GetConfigDouble(L"ILLUM_PH1_X") * 10000.0f, 0);
	m_pstCfg->illum_ph_y_axis_pos[1]= (INT32)ROUNDED(GetConfigDouble(L"ILLUM_PH1_Y") * 10000.0f, 0);
	m_pstCfg->illum_ph_x_axis_pos[2]= (INT32)ROUNDED(GetConfigDouble(L"ILLUM_PH2_X") * 10000.0f, 0);
	m_pstCfg->illum_ph_y_axis_pos[2]= (INT32)ROUNDED(GetConfigDouble(L"ILLUM_PH2_Y") * 10000.0f, 0);
	m_pstCfg->led0_power_ph			= (INT32)ROUNDED(GetConfigDouble(L"LED0_POWER_PH") * 10000.0f, 0);
 	m_pstCfg->led1_power_ph			= (INT32)ROUNDED(GetConfigDouble(L"LED1_POWER_PH") * 10000.0f, 0);
 	m_pstCfg->led2_power_ph			= (INT32)ROUNDED(GetConfigDouble(L"LED2_POWER_PH") * 10000.0f, 0);
 	m_pstCfg->led3_power_ph			= (INT32)ROUNDED(GetConfigDouble(L"LED3_POWER_PH") * 10000.0f, 0);
	m_pstCfg->led0_index_ph1		= (UINT32)ROUNDED(GetConfigUint32(L"LED0_INDEX_PH1"), 0);
	m_pstCfg->led1_index_ph1		= (UINT32)ROUNDED(GetConfigUint32(L"LED1_INDEX_PH1"), 0);
	m_pstCfg->led2_index_ph1		= (UINT32)ROUNDED(GetConfigUint32(L"LED2_INDEX_PH1"), 0);
	m_pstCfg->led3_index_ph1		= (UINT32)ROUNDED(GetConfigUint32(L"LED3_INDEX_PH1"), 0);
	m_pstCfg->led0_index_ph2		= (UINT32)ROUNDED(GetConfigUint32(L"LED0_INDEX_PH2"), 0);
	m_pstCfg->led1_index_ph2		= (UINT32)ROUNDED(GetConfigUint32(L"LED1_INDEX_PH2"), 0);
	m_pstCfg->led2_index_ph2		= (UINT32)ROUNDED(GetConfigUint32(L"LED2_INDEX_PH2"), 0);
	m_pstCfg->led3_index_ph2		= (UINT32)ROUNDED(GetConfigUint32(L"LED3_INDEX_PH2"), 0);

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Teaching - VELO
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::SaveConfigTeachingIllum()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"ILLUM");

	SetConfigDouble(L"ILLUM_PH1_X",		m_pstCfg->illum_ph_x_axis_pos[1] / 10000.0f, 4);
	SetConfigDouble(L"ILLUM_PH1_Y",		m_pstCfg->illum_ph_y_axis_pos[1] / 10000.0f, 4);
	SetConfigDouble(L"ILLUM_PH2_X",		m_pstCfg->illum_ph_x_axis_pos[2] / 10000.0f, 4);
	SetConfigDouble(L"ILLUM_PH2_Y",		m_pstCfg->illum_ph_y_axis_pos[2] / 10000.0f, 4);
	SetConfigDouble(L"LED0_POWER_PH",	m_pstCfg->led0_power_ph / 1000.0f, 3);
	SetConfigDouble(L"LED1_POWER_PH",	m_pstCfg->led1_power_ph / 1000.0f, 3);
	SetConfigDouble(L"LED2_POWER_PH",	m_pstCfg->led2_power_ph / 1000.0f, 3);
	SetConfigDouble(L"LED3_POWER_PH",	m_pstCfg->led3_power_ph / 1000.0f, 3);
	SetConfigUint32(L"LED0_INDEX_PH1",	m_pstCfg->led0_index_ph1);
	SetConfigUint32(L"LED1_INDEX_PH1",	m_pstCfg->led1_index_ph1);
	SetConfigUint32(L"LED2_INDEX_PH1",	m_pstCfg->led2_index_ph1);
	SetConfigUint32(L"LED3_INDEX_PH1",	m_pstCfg->led3_index_ph1);
	SetConfigUint32(L"LED0_INDEX_PH2",	m_pstCfg->led0_index_ph2);
	SetConfigUint32(L"LED1_INDEX_PH2",	m_pstCfg->led1_index_ph2);
	SetConfigUint32(L"LED2_INDEX_PH2",	m_pstCfg->led2_index_ph2);
	SetConfigUint32(L"LED3_INDEX_PH2",	m_pstCfg->led3_index_ph2);

	return TRUE;
}

/*
 desc : 환경 파일 적재 - Teaching - CENTERING
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::LoadConfigTeachingCentering()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"CENTERING");

	m_pstCfg->cent_quaz_bsa_x_pos	= (INT32)ROUNDED(GetConfigDouble(L"QUARTZ_BSA_X") * 10000.0f, 0);
	m_pstCfg->cent_quaz_bsa_y_pos	= (INT32)ROUNDED(GetConfigDouble(L"QUARTZ_BSA_Y") * 10000.0f, 0);
	m_pstCfg->cent_mark1_bsa_x_pos	= (INT32)ROUNDED(GetConfigDouble(L"MARK1_BSA_X") * 10000.0f, 0);
	m_pstCfg->cent_mark1_bsa_y_pos	= (INT32)ROUNDED(GetConfigDouble(L"MARK1_BSA_Y") * 10000.0f, 0);
	m_pstCfg->cent_mark2_bsa_x_pos	= (INT32)ROUNDED(GetConfigDouble(L"MARK2_BSA_X") * 10000.0f, 0);
	m_pstCfg->cent_mark2_bsa_y_pos	= (INT32)ROUNDED(GetConfigDouble(L"MARK2_BSA_Y") * 10000.0f, 0);
	m_pstCfg->cent_quaz_up_x_pos	= (INT32)ROUNDED(GetConfigDouble(L"QUARTZ_UP_X") * 10000.0f, 0);
	m_pstCfg->cent_quaz_up_y_pos	= (INT32)ROUNDED(GetConfigDouble(L"QUARTZ_UP_Y") * 10000.0f, 0);
	m_pstCfg->cent_mark1_up_x_pos	= (INT32)ROUNDED(GetConfigDouble(L"MARK1_UP_X") * 10000.0f, 0);
	m_pstCfg->cent_mark1_up_y_pos	= (INT32)ROUNDED(GetConfigDouble(L"MARK1_UP_Y") * 10000.0f, 0);
	m_pstCfg->cent_mark2_up_x_pos	= (INT32)ROUNDED(GetConfigDouble(L"MARK2_UP_X") * 10000.0f, 0);
	m_pstCfg->cent_mark2_up_y_pos	= (INT32)ROUNDED(GetConfigDouble(L"MARK2_UP_Y") * 10000.0f, 0);

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Teaching - CENTERING
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::SaveConfigTeachingCentering()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"CENTERING");

	SetConfigDouble(L"QUARTZ_BSA_X",m_pstCfg->cent_quaz_bsa_x_pos / 10000.0f, 4); 
	SetConfigDouble(L"QUARTZ_BSA_Y",m_pstCfg->cent_quaz_bsa_y_pos / 10000.0f, 4);
	SetConfigDouble(L"MARK1_BSA_X",	m_pstCfg->cent_mark1_bsa_x_pos / 10000.0f, 4);
	SetConfigDouble(L"MARK1_BSA_Y",	m_pstCfg->cent_mark1_bsa_y_pos / 10000.0f, 4);
	SetConfigDouble(L"MARK2_BSA_X",	m_pstCfg->cent_mark2_bsa_x_pos / 10000.0f, 4);
	SetConfigDouble(L"MARK2_BSA_Y",	m_pstCfg->cent_mark2_bsa_y_pos / 10000.0f, 4);
	SetConfigDouble(L"QUARTZ_UP_X",	m_pstCfg->cent_quaz_up_x_pos / 10000.0f, 4);
	SetConfigDouble(L"QUARTZ_UP_Y",	m_pstCfg->cent_quaz_up_y_pos / 10000.0f, 4);
	SetConfigDouble(L"MARK1_UP_X",	m_pstCfg->cent_mark1_up_x_pos / 10000.0f, 4);
	SetConfigDouble(L"MARK1_UP_Y",	m_pstCfg->cent_mark1_up_y_pos / 10000.0f, 4);
	SetConfigDouble(L"MARK2_UP_X",	m_pstCfg->cent_mark2_up_x_pos / 10000.0f, 4);
	SetConfigDouble(L"MARK2_UP_Y",	m_pstCfg->cent_mark2_up_y_pos / 10000.0f, 4);

	return TRUE;
}

/*
 desc : 환경 파일 적재 - Teaching - LIGHT
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::LoadConfigTeachingLight()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"LIGHT");

	m_pstCfg->ring_light_value		= GetConfigUint32(L"RING_LIGHT_VALUE");
	m_pstCfg->coaxial_light_value	= GetConfigUint32(L"COAXIAL_LIGHT_VALUE");

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Teaching - LIGHT
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::SaveConfigTeachingLight()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"LIGHT");

	SetConfigUint32(L"RING_LIGHT_VALUE",	m_pstCfg->ring_light_value);
	SetConfigUint32(L"COAXIAL_LIGHT_VALUE",	m_pstCfg->coaxial_light_value);

	return TRUE;
}

/*
 desc : 환경 파일 적재 - Teaching - EQUIPMENT
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::LoadConfigTeachingEquipment()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"EQUIPMENT");

	m_pstCfg->e_tower_lamp			= GetConfigUint8(L"TOWERLAMP");
	m_pstCfg->e_shutter				= GetConfigUint8(L"SHUTTER");
	m_pstCfg->e_align_expo_select	= GetConfigUint8(L"ALIGN_EXPO");
	m_pstCfg->e_error_skip			= GetConfigUint8(L"ERR_SKIP");
	m_pstCfg->e_cal_offset			= GetConfigUint8(L"CAL_OFFSET");
	m_pstCfg->e_auto_focus			= GetConfigUint8(L"AUTO_FOCUS");
	m_pstCfg->e_stitch_calib		= GetConfigUint8(L"STITCH_CALIB");
	m_pstCfg->mark_refind_count		= GetConfigUint8(L"MARK_REFIND_CNT");
	m_pstCfg->e_robot_info_mode		= GetConfigUint8(L"RBT_INFO_MODE");
	m_pstCfg->mark_refind_range		= (INT32)ROUNDED(GetConfigDouble(L"MARK_REFIND_RANGE") * 10000.0f, 0);

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Teaching - EQUIPMENT
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::SaveConfigTeachingEquipment()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"EQUIPMENT");

	SetConfigUint32(L"TOWERLAMP",			m_pstCfg->e_tower_lamp);
	SetConfigUint32(L"SHUTTER",				m_pstCfg->e_shutter);
	SetConfigUint32(L"ALIGN_EXPO",			m_pstCfg->e_align_expo_select);
	SetConfigUint32(L"ERR_SKIP",			m_pstCfg->e_error_skip);
	SetConfigUint32(L"CAL_OFFSET",			m_pstCfg->e_cal_offset);
	SetConfigUint32(L"AUTO_FOCUS",			m_pstCfg->e_auto_focus);
	SetConfigUint32(L"STITCH_CALIB",		m_pstCfg->e_stitch_calib);
	SetConfigUint32(L"MARK_REFIND_CNT",		m_pstCfg->mark_refind_count);
	SetConfigUint32(L"RBT_INFO_MODE",		m_pstCfg->e_robot_info_mode);
	SetConfigDouble(L"MARK_REFIND_RANGE",	m_pstCfg->mark_refind_range / 10000.0f, 2);

	return TRUE;
}

/*
 desc : 환경 파일 적재 - Teaching - INTERLOCK
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::LoadConfigTeachingInterlock()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"INTERLOCK");

	m_pstCfg->inlock_stage_pos_min_x		= (INT32)ROUNDED(GetConfigDouble(L"INTL_STAGE_MIN_X") * 10000.0f, 0);
	m_pstCfg->inlock_stage_pos_min_y		= (INT32)ROUNDED(GetConfigDouble(L"INTL_STAGE_MIN_Y") * 10000.0f, 0);
	m_pstCfg->inlock_stage_pos_max_x		= (INT32)ROUNDED(GetConfigDouble(L"INTL_STAGE_MAX_X") * 10000.0f, 0);
	m_pstCfg->inlock_stage_pos_max_y		= (INT32)ROUNDED(GetConfigDouble(L"INTL_STAGE_MAX_Y") * 10000.0f, 0);
	m_pstCfg->inlock_bsa_pos_min_z			= (INT32)ROUNDED(GetConfigDouble(L"INTL_BSA_MIN_Z")	* 10000.0f, 0);
	m_pstCfg->inlock_bsa_pos_max_z			= (INT32)ROUNDED(GetConfigDouble(L"INTL_BSA_MAX_Z")	* 10000.0f, 0);
	m_pstCfg->inlock_pin_pos_min_x			= (INT32)ROUNDED(GetConfigDouble(L"INTL_PIN_MIN_X") * 10000.0f, 0);
	m_pstCfg->inlock_pin_pos_max_x			= (INT32)ROUNDED(GetConfigDouble(L"INTL_PIN_MAX_X") * 10000.0f, 0);
	m_pstCfg->inlock_pin_pos_min_z			= (INT32)ROUNDED(GetConfigDouble(L"INTL_PIN_MIN_Z")	* 10000.0f, 0);
	m_pstCfg->inlock_pin_pos_max_z			= (INT32)ROUNDED(GetConfigDouble(L"INTL_PIN_MAX_Z")	* 10000.0f, 0);
	m_pstCfg->inlock_up_pos_min_z		 	= (INT32)ROUNDED(GetConfigDouble(L"INTL_UP_MIN_Z")	* 1000.0f, 0);
	m_pstCfg->inlock_up_pos_max_z			= (INT32)ROUNDED(GetConfigDouble(L"INTL_UP_MAX_Z")	* 1000.0f, 0);
	m_pstCfg->inlock_stage_max_speed		= GetConfigDouble(L"INTL_STAGE_MAX_SPEED");
	m_pstCfg->inlock_stage_min_speed		= GetConfigDouble(L"INTL_STAGE_MIN_SPEED");
	m_pstCfg->inlock_bsa_max_speed			= GetConfigDouble(L"INTL_BSA_MAX_SPEED");
	m_pstCfg->inlock_bsa_min_speed			= GetConfigDouble(L"INTL_BSA_MIN_SPEED");
	m_pstCfg->inlock_pin_max_speed	 		= GetConfigDouble(L"INTL_PIN_MAX_SPEED");
	m_pstCfg->inlock_pin_min_speed			= GetConfigDouble(L"INTL_PIN_MIN_SPEED");

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Teaching - INTERLOCK
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::SaveConfigTeachingInterlock()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"INTERLOCK");

	SetConfigDouble(L"INTL_STAGE_MIN_X",	m_pstCfg->inlock_stage_pos_min_x / 10000.0f, 4);
	SetConfigDouble(L"INTL_STAGE_MIN_Y",	m_pstCfg->inlock_stage_pos_min_y / 10000.0f, 4);
	SetConfigDouble(L"INTL_STAGE_MAX_X",	m_pstCfg->inlock_stage_pos_max_x / 10000.0f, 4);
	SetConfigDouble(L"INTL_STAGE_MAX_Y",	m_pstCfg->inlock_stage_pos_max_y / 10000.0f, 4);
	SetConfigDouble(L"INTL_BSA_MIN_Z",		m_pstCfg->inlock_bsa_pos_min_z / 10000.0f, 4);
	SetConfigDouble(L"INTL_BSA_MAX_Z",		m_pstCfg->inlock_bsa_pos_max_z / 10000.0f, 4);
	SetConfigDouble(L"INTL_PIN_MIN_Z",		m_pstCfg->inlock_pin_pos_min_z / 10000.0f, 4);
	SetConfigDouble(L"INTL_PIN_MAX_Z",		m_pstCfg->inlock_pin_pos_max_z / 10000.0f, 4);
	SetConfigDouble(L"INTL_PIN_MIN_X",		m_pstCfg->inlock_pin_pos_min_x / 10000.0f, 4);
	SetConfigDouble(L"INTL_PIN_MAX_X",		m_pstCfg->inlock_pin_pos_max_x / 10000.0f, 4);
	SetConfigDouble(L"INTL_PIN_MAX_SPEED",	m_pstCfg->inlock_pin_max_speed);
	SetConfigDouble(L"INTL_PIN_MIN_SPEED",	m_pstCfg->inlock_pin_min_speed);

	return TRUE;
}

/*
 desc : 환경 파일 적재 - Teaching - TRANSFORMATION RECIPE
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::LoadConfigTeachingTransRecipe()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"TRANSFORMATION_RECIPE");

	m_pstCfg->expo_set_rotation	= (INT32)ROUNDED(GetConfigDouble(L"SET_ROTATION") * 10000.0f, 0);
	m_pstCfg->expo_set_scale_x	= (INT32)ROUNDED(GetConfigDouble(L"SET_SCALE_X") * 10000.0f, 0);
	m_pstCfg->expo_set_scale_y	= (INT32)ROUNDED(GetConfigDouble(L"SET_SCALE_Y") * 10000.0f, 0);
	m_pstCfg->expo_set_offset_x	= (INT32)ROUNDED(GetConfigDouble(L"SET_OFFSET_X") * 10000.0f, 0);
	m_pstCfg->expo_set_offset_y	= (INT32)ROUNDED(GetConfigDouble(L"SET_OFFSET_Y") * 10000.0f, 0);

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Teaching - TRANSFORMATION RECIPE
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::SaveConfigTeachingTransRecipe()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"TRANSFORMATION_RECIPE");

	SetConfigDouble(L"SET_ROTATION",	m_pstCfg->expo_set_rotation / 10000.0f, 4);
	SetConfigDouble(L"SET_SCALE_X",		m_pstCfg->expo_set_scale_x / 10000.0f, 4);
	SetConfigDouble(L"SET_SCALE_Y",		m_pstCfg->expo_set_scale_y / 10000.0f, 4);
	SetConfigDouble(L"SET_OFFSET_X",	m_pstCfg->expo_set_offset_x / 10000.0f, 4);
	SetConfigDouble(L"SET_OFFSET_Y",	m_pstCfg->expo_set_offset_y / 10000.0f, 4);

	return TRUE;
}

/*
 desc : 환경 파일 적재 - Teaching - EXPO_LURIA_SETTING RECIPE
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::LoadConfigTeachingExpoLuria()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"EXPO_MATRIX_GERBER");

	m_pstCfg->expo_align			= GetConfigUint8(L"MARK_ALIGN");
	m_pstCfg->expo_rectangle_lock	= GetConfigUint8(L"EXPO_RECTANGLE");
	m_pstCfg->expo_scroll_mode_1	= GetConfigUint8(L"SCROLL_MODE_1");
	m_pstCfg->expo_neg_offset_1		= GetConfigUint8(L"EXPO_NEG_OFFSET_1");
	m_pstCfg->expo_photo1_height	= (INT32)ROUNDED(GetConfigDouble(L"PHOTO1_HEIGHT") * 10000.0f, 0);
	m_pstCfg->expo_photo2_height	= (INT32)ROUNDED(GetConfigDouble(L"PHOTO2_HEIGHT") * 10000.0f, 0);
	m_pstCfg->expo_ph2_offset_x		= (INT32)ROUNDED(GetConfigDouble(L"PH_OFFSET_X_2") * 10000.0f, 0);
	m_pstCfg->expo_ph2_offset_y		= (INT32)ROUNDED(GetConfigDouble(L"PH_OFFSET_Y_2") * 10000.0f, 0);
	m_pstCfg->expo_pos_delay_1		= (INT32)GetConfigDouble(L"DELAY_POSITIVE_1");
	m_pstCfg->expo_neg_delay_1		= (INT32)GetConfigDouble(L"DELAY_NEGATIVE_1");
	m_pstCfg->expo_duty_cycle		= (INT32)GetConfigUint8(L"SET_DUTY_CYCLE");

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Teaching - EXPO_LURIA_SETTING RECIPE
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTeaching::SaveConfigTeachingExpoLuria()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"EXPO_MATRIX_GERBER");

	SetConfigUint32(L"MARK_ALIGN",		m_pstCfg->expo_align);
	SetConfigUint32(L"EXPO_RECTANGLE",	m_pstCfg->expo_rectangle_lock);
	SetConfigDouble(L"PHOTO1_HEIGHT",	m_pstCfg->expo_photo1_height / 10000.0f, 4);
	SetConfigDouble(L"PHOTO2_HEIGHT",	m_pstCfg->expo_photo2_height / 10000.0f, 4);
	SetConfigInt32(L"SCROLL_MODE_1",	m_pstCfg->expo_scroll_mode_1);
	SetConfigInt32(L"EXPO_NEG_OFFSET_1",m_pstCfg->expo_neg_offset_1);
	SetConfigInt32(L"DELAY_POSITIVE_1",	m_pstCfg->expo_pos_delay_1);
	SetConfigInt32(L"DELAY_POSITIVE_1",	m_pstCfg->expo_neg_delay_1);
	SetConfigDouble(L"PH_OFFSET_X_2",	m_pstCfg->expo_ph2_offset_x / 10000.0f, 4);
	SetConfigDouble(L"PH_OFFSET_Y_2",	m_pstCfg->expo_ph2_offset_y / 10000.0f, 4);
	SetConfigInt32(L"SET_DUTY_CYCLE",	m_pstCfg->expo_duty_cycle);

	return TRUE;
}

