
/*
 desc : Engine Configuration for UVDI15
*/

#include "pch.h"
#include "ConfUvdi15.h"
#include <string>
#include <tuple>
#include <vector>
#include <map>
#include <locale>
#include <iostream>
#include <sstream>
#include <regex>
#include <atlstr.h>

#include "../conf/conf_comn.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : Constructor / Destructor
 parm : config	- [in]  Configuration Structure
 retn : None
*/
CConfUvdi15::CConfUvdi15(LPG_CIEA config)
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	: CConfBase(L"uvdi15")
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	: CConfBase(L"hddi6")
#endif

{
	m_pstCfg	= config;

	
}
CConfUvdi15::~CConfUvdi15()
{
}

/*
 desc : Load or Save the config file
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfig()
{
	BOOL bSucc	= TRUE;

	

	/* 아래 함수 호출 순서는 반드시 지켜야 됨 */
	if (!LoadConfigFileName())	/* [FILE_NAME] */
	{
		AfxMessageBox(L"Failed to load the config for FILE_NAME", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	
	if (!LoadConfigSetupCamera())	/* [SETUP_CAMERA] */
	{
		AfxMessageBox(L"Failed to load the config for SETUP_CAMERA", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;

	}
	if (!LoadConfigCameraBasler())	/* [CAMERA_IDS or BASLER] */
	{
		AfxMessageBox(L"Failed to load the config for CAMERA_IDS (BASLER)", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	if (!LoadConfigSetupAlign())	/* [SETUP_ALIGN] */
	{
		AfxMessageBox(L"Failed to load the config for SETUP_ALIGN", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	if (!LoadConfigHeadOffsets())	/* [헤드옵셋] */
	{
		AfxMessageBox(L"Failed to load the config for head offset", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}

	if (!LoadConfigEnvirnomental())	/* [SETUP_enviroment] */
	{
		AfxMessageBox(L"Failed to load the config for envirimental features", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}


	if (!LoadConfigACamSpec())		/* [ACAM_SPEC] */
	{
		AfxMessageBox(L"Failed to load the config for ACAM_SPEC", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	if (!LoadConfigACamFocus())		/* [ACAM_FOCUS] */
	{
		AfxMessageBox(L"Failed to load the config for ACAM_FOCUS", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	if (!LoadConfigPhStepXY())		/* [PH_STEP] */
	{
		AfxMessageBox(L"Failed to load the config for PH_STEP", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	if (!LoadConfigMarkFind())		/* [MARK_FIND] */
	{
		AfxMessageBox(L"Failed to load the config for MARK_FIND", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	if (!LoadConfigEdgeFind())		/* [EDGE_FIND] */
	{
		AfxMessageBox(L"Failed to load the config for EDGE_FIND", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	if (!LoadConfigGrabProc())		/* [GRAB_PRE_PROC] */
	{
		AfxMessageBox(L"Failed to load the config for GRAB_PRE_PROC", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
#if 0
	if (!LoadConfigCaliFind())		/* [CALI_FIND] */
	{
		AfxMessageBox(L"Failed to load the config for CALI_FIND", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
#endif
	if (!LoadConfigLineFind())		/* [LINE_FIND] */
	{
		AfxMessageBox(L"Failed to load the config for LINE_FIND", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	if (!LoadConfigLedPower())		/* [LED_POWER] */
	{
		AfxMessageBox(L"Failed to load the config for LED_POWER", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	if (!LoadConfigPhFocus())		/* [PH_FOCUS] */
	{
		AfxMessageBox(L"Failed to load the config for PH_FOCUS", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	if (!LoadConfigACamCali())		/* [ACAM_CALI]	*/
	{
		AfxMessageBox(L"Failed to load the config for ACAM_CALI", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
// 	if (!LoadConfigStageST())		/* [STAGE_ST]	*/
// 	{
// 		AfxMessageBox(L"Failed to load the config for STAGE_ST", MB_ICONSTOP|MB_TOPMOST);
// 		return FALSE;
// 	}
	if (!LoadConfigTrigger(0x00))	/* [TRIG_GRAB ]	*/
	{
		AfxMessageBox(L"Failed to load the config for TRIG_GRAB", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
//	if (!LoadConfigTrigger(0x01))	/* [TRIG_STEP]	*/
//	{
//		AfxMessageBox(L"Failed to load the config for TRIG_STEP", MB_ICONSTOP|MB_TOPMOST);
//		return FALSE;
//	}
	if (!LoadConfigTemp())			/* [TEMP_RANGE]	*/
	{
		AfxMessageBox(L"Failed to load the config for TEMP_RANGE", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}
	if (!LoadConfigGlobalTrans())	/* [GLOBAL_TRANS] */
	{
		AfxMessageBox(L"Failed to load the config for EDGE_FIND", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	if (!LoadConfigFlatParam())		/* [FLATNESS] */
	{
		AfxMessageBox(L"Failed to load the config for FLATNESS", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}
	if (!LoadConfigUvDI15Common())	/* [UVDI15_COMN]	*/
	{
		AfxMessageBox(L"Failed to load the config for UVDI15_COMN", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	if (!LoadConfigStrobeLamp())	/* [STROBE_LAMP]	*/
	{
		AfxMessageBox(L"Failed to load the config for STROBE_LAMP", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}

	return TRUE;
}
BOOL CConfUvdi15::SaveConfig()
{
	if (!SaveConfigSetupAlign())	return FALSE;
	if (!SaveConfigLedPower())		return FALSE;
	if (!SaveConfigPhFocus())		return FALSE;
	if (!SaveConfigACamCali())		return FALSE;
	if (!SaveConfigACamSpec())		return FALSE;
	if (!SaveConfigACamFocus())		return FALSE;
#if 0
	if (!SaveConfigCaliFind())		return FALSE;
#endif
	if (!SaveConfigSetupCamera())	return FALSE;
	if (!SaveConfigFileName())		return FALSE;
	if (!SaveConfigGrabProc())		return FALSE;
	if (!SaveConfigLineFind())		return FALSE;
	if (!SaveConfigEdgeFind())		return FALSE;
	if (!SaveConfigMarkFind())		return FALSE;
	if (!SaveConfigPhStepXY())		return FALSE;
	if (!SaveConfigFlatParam())		return FALSE;
	if (!LoadConfigUvDI15Common())	return FALSE;	/* [UVDI15_COMN]	*/
	/* Trigger Grab 설정 값*/
	if (!SaveConfigTrigger(0x00))	return FALSE;
	if (!SaveConfigTrigger(0x01))	return FALSE;
	if (!SaveConfigCameraBasler())	return FALSE;
	if (!SaveConfigGlobalTrans())	return FALSE;

	if (!SaveConfigStrobeLamp())	return FALSE;
	//if (!SaveConfigAutoHotAir())	return FALSE;

	return TRUE;
}

/*
 desc : Load or Save the config file (FILE_NAME)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfigFileName()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"FILE_NAME");

	GetConfigStr(L"FILE_LED_POWER", m_pstCfg->file_dat.led_power, MAX_FILE_LEN);
	//GetConfigStr(L"FILE_GERB_RECIPE",	m_pstCfg->file_dat.gerb_recipe,	MAX_FILE_LEN);
	GetConfigStr(L"FILE_JOB_RECIPE", m_pstCfg->file_dat.job_recipe, MAX_FILE_LEN);
	GetConfigStr(L"FILE_EXPO_RECIPE", m_pstCfg->file_dat.expo_recipe, MAX_FILE_LEN);
	GetConfigStr(L"FILE_ALIGN_RECIPE", m_pstCfg->file_dat.align_recipe, MAX_FILE_LEN);
	GetConfigStr(L"FILE_MARK_RECIPE", m_pstCfg->file_dat.mark_recipe, MAX_FILE_LEN);
	GetConfigStr(L"FILE_MARK_MODEL", m_pstCfg->file_dat.mark_model, MAX_FILE_LEN);
	GetConfigStr(L"FILE_MARK_ROI", m_pstCfg->file_dat.mark_roi, MAX_FILE_LEN);
	GetConfigStr(L"FILE_PH_STEP", m_pstCfg->file_dat.ph_step, MAX_FILE_LEN);
	GetConfigStr(L"FILE_ACAM_CALI", m_pstCfg->file_dat.acam_cali, MAX_FILE_LEN);
	GetConfigStr(L"FILE_TRIG_CALI", m_pstCfg->file_dat.trig_cali, MAX_FILE_LEN);
	GetConfigStr(L"FILE_THICK_CALI", m_pstCfg->file_dat.thick_cali, MAX_FILE_LEN);
	GetConfigStr(L"FILE_CORRECT_Y", m_pstCfg->file_dat.correct_y, MAX_FILE_LEN);

	GetConfigStr(L"FILE_STATIC_ALIGNCALI_CAM" , m_pstCfg->file_dat.staticAcamAlignCali, MAX_FILE_LEN);
	GetConfigStr(L"FILE_STATIC_EXPOCALI_CAM" , m_pstCfg->file_dat.staticAcamExpoCali, MAX_FILE_LEN);
	


	return TRUE;
}
BOOL CConfUvdi15::SaveConfigFileName()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"FILE_NAME");

	SetConfigStr(L"FILE_LED_POWER", m_pstCfg->file_dat.led_power);
	//SetConfigStr(L"FILE_GERB_RECIPE",	m_pstCfg->file_dat.gerb_recipe);
	SetConfigStr(L"FILE_JOB_RECIPE", m_pstCfg->file_dat.job_recipe);
	SetConfigStr(L"FILE_EXPO_RECIPE", m_pstCfg->file_dat.expo_recipe);
	SetConfigStr(L"FILE_ALIGN_RECIPE", m_pstCfg->file_dat.align_recipe);
	SetConfigStr(L"FILE_MARK_RECIPE", m_pstCfg->file_dat.mark_recipe);
	SetConfigStr(L"FILE_MARK_MODEL", m_pstCfg->file_dat.mark_model);
	SetConfigStr(L"FILE_MARK_ROI", m_pstCfg->file_dat.mark_roi);
	SetConfigStr(L"FILE_PH_STEP", m_pstCfg->file_dat.ph_step);
	SetConfigStr(L"FILE_ACAM_CALI", m_pstCfg->file_dat.acam_cali);
	SetConfigStr(L"FILE_TRIG_CALI", m_pstCfg->file_dat.trig_cali);
	SetConfigStr(L"FILE_CORRECT_Y", m_pstCfg->file_dat.correct_y);

	return TRUE;
}



/*
 desc : Load or Save the config file (SETUP_CAMERA)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfigSetupCamera()
{
	TCHAR tzKey[64]	= {NULL};

	/* Subject Name 설정 */
	swprintf_s(m_tzSubj, MAX_SUBJ_STRING, L"SETUP_CAMERA");

	m_pstCfg->set_cams.acam_count		= GetConfigUint8(L"ACAM_COUNT");
	m_pstCfg->set_cams.acam_inst_angle	= GetConfigUint8(L"ACAM_INST_ANGLE");

	m_pstCfg->set_cams.acam_size[0]		= GetConfigUint16(L"ACAM_WIDTH");
	m_pstCfg->set_cams.acam_size[1]		= GetConfigUint16(L"ACAM_HEIGHT");

	m_pstCfg->set_cams.aoi_size[0]		= GetConfigUint16(L"AOI_WIDTH");
	m_pstCfg->set_cams.aoi_size[1]		= GetConfigUint16(L"AOI_HEIGHT");
	m_pstCfg->set_cams.soi_size[0]		= GetConfigUint16(L"SOI_WIDTH");
	m_pstCfg->set_cams.soi_size[1]		= GetConfigUint16(L"SOI_HEIGHT");
	m_pstCfg->set_cams.doi_size[0]		= GetConfigUint16(L"DOI_WIDTH");
	m_pstCfg->set_cams.doi_size[1]		= GetConfigUint16(L"DOI_HEIGHT");
	m_pstCfg->set_cams.ioi_size[0]		= GetConfigUint16(L"IOI_WIDTH");
	m_pstCfg->set_cams.ioi_size[1]		= GetConfigUint16(L"IOI_HEIGHT");
	m_pstCfg->set_cams.mes_size[0]		= GetConfigUint16(L"MES_WIDTH");
	m_pstCfg->set_cams.mes_size[1]		= GetConfigUint16(L"MES_HEIGHT");
	m_pstCfg->set_cams.spc_size[0]		= GetConfigUint16(L"SPC_WIDTH");
	m_pstCfg->set_cams.spc_size[1]		= GetConfigUint16(L"SPC_HEIGHT");
	m_pstCfg->set_cams.stg_size[0]		= GetConfigUint16(L"STG_WIDTH");
	m_pstCfg->set_cams.stg_size[1]		= GetConfigUint16(L"STG_HEIGHT");
	m_pstCfg->set_cams.hol_size[0]		= GetConfigUint16(L"HOL_WIDTH");
	m_pstCfg->set_cams.hol_size[1]		= GetConfigUint16(L"HOL_HEIGHT");

	m_pstCfg->set_cams.acam_up_limit	= GetConfigDouble(L"MOVE_UP_LIMIT");
	m_pstCfg->set_cams.acam_dn_limit	= GetConfigDouble(L"MOVE_DN_LIMIT");

	for (UINT8 i = 0; i < MAX_ALIGN_CAMERA; i++)
	{
		swprintf_s(tzKey, MAX_KEY_STRING, L"CAM%d_SAFETY_POS", i);
		m_pstCfg->set_cams.safety_pos[i] = GetConfigDouble(tzKey);
	}
	
	m_pstCfg->set_cams.trigMode = GetConfigUint8(L"TRIGGER_MODE"); //0 - LINE MODE (TRIGGER BOARD) , 1 = SW TRIGGER

	/* 카메라 크기보다 FOV 크기가 크면 안됨 */
	if (!m_pstCfg->set_cams.IsCamSizeValid())
	{
		AfxMessageBox(L"Invalid the Camera Size (Check the sizeof of the Camera FOV)", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	return TRUE;
}
BOOL CConfUvdi15::SaveConfigSetupCamera()
{
	TCHAR tzKey[64]	= {NULL};

	/* Subject Name 설정 */
	swprintf_s(m_tzSubj, MAX_SUBJ_STRING, L"SETUP_CAMERA");

	SetConfigUint32(L"ACAM_COUNT",			m_pstCfg->set_cams.acam_count);
	SetConfigUint32(L"ACAM_INST_ANGLE",		m_pstCfg->set_cams.acam_inst_angle);

	SetConfigUint32(L"ACAM_WIDTH",			m_pstCfg->set_cams.acam_size[0]);
	SetConfigUint32(L"ACAM_HEIGHT",			m_pstCfg->set_cams.acam_size[1]);

	SetConfigUint32(L"AOI_WIDTH",			m_pstCfg->set_cams.aoi_size[0]);
	SetConfigUint32(L"AOI_HEIGHT",			m_pstCfg->set_cams.aoi_size[1]);
	SetConfigUint32(L"SOI_WIDTH",			m_pstCfg->set_cams.soi_size[0]);
	SetConfigUint32(L"SOI_HEIGHT",			m_pstCfg->set_cams.soi_size[1]);
	SetConfigUint32(L"DOI_WIDTH",			m_pstCfg->set_cams.doi_size[0]);
	SetConfigUint32(L"DOI_HEIGHT",			m_pstCfg->set_cams.doi_size[1]);
	SetConfigUint32(L"IOI_WIDTH",			m_pstCfg->set_cams.ioi_size[0]);
	SetConfigUint32(L"IOI_HEIGHT",			m_pstCfg->set_cams.ioi_size[1]);
	SetConfigUint32(L"MES_WIDTH",			m_pstCfg->set_cams.mes_size[0]);
	SetConfigUint32(L"MES_HEIGHT",			m_pstCfg->set_cams.mes_size[1]);
	SetConfigUint32(L"SPC_WIDTH",			m_pstCfg->set_cams.spc_size[0]);
	SetConfigUint32(L"SPC_HEIGHT",			m_pstCfg->set_cams.spc_size[1]);
	SetConfigUint32(L"STG_WIDTH",			m_pstCfg->set_cams.stg_size[0]);
	SetConfigUint32(L"STG_HEIGHT",			m_pstCfg->set_cams.stg_size[1]);
	SetConfigUint32(L"HOL_WIDTH",			m_pstCfg->set_cams.hol_size[0]);
	SetConfigUint32(L"HOL_HEIGHT",			m_pstCfg->set_cams.hol_size[1]);

	SetConfigDouble(L"MOVE_UP_LIMIT",		m_pstCfg->set_cams.acam_up_limit,	4);
	SetConfigDouble(L"MOVE_DN_LIMIT",		m_pstCfg->set_cams.acam_dn_limit,	4);

	for (UINT8 i = 0; i < MAX_ALIGN_CAMERA; i++)
	{
		swprintf_s(tzKey, MAX_KEY_STRING, L"CAM%d_SAFETY_POS", i);
		SetConfigDouble(tzKey, m_pstCfg->set_cams.safety_pos[i], 4);
	}


/* 카메라 크기보다 FOV 크기가 크면 안됨 */
	if (!m_pstCfg->set_cams.IsCamSizeValid())
	{
		AfxMessageBox(L"Invalid the Camera Size (Check the sizeof of the Camera FOV)", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	return TRUE;
}

template <typename T>
void ParseAndFillVector(const TCHAR* str, TCHAR splitToken, vector<T>& destVec)
{
	destVec.clear();

	if (!str) throw invalid_argument("입력 문자열이 null입니다.");

	basic_string<TCHAR> input(str);
	basic_stringstream<TCHAR> ss(input);
	basic_string<TCHAR> token;

	while (getline(ss, token, splitToken))
	{
		if (token.empty()) continue;

		basic_stringstream<TCHAR> convertStream(token);
		T value;
		convertStream >> value;

		if (convertStream.fail())
			throw runtime_error("변환 실패: " + string(token.begin(), token.end()));

		destVec.push_back(value);
	}
}

void ParseAndFillVector(const TCHAR* str, std::vector<std::tuple<bool,int, double, double>>& vec)
{
	vec.clear();

	std::regex re(R"(\{([GgLl]),\s*(\d+),\s*([-+]?\d+(?:\.\d+)?),\s*([-+]?\d+(?:\.\d+)?)\})"); //이게 뭔지 의문을 갖지마세요. 정규식은 볼때마다 좆같습니다.
	std::smatch match;
	
	CT2A convertedStr(str, CP_UTF8);

	string input = string(convertedStr);
	if (input.empty()) return;;

	std::string::const_iterator searchStart(input.cbegin());

	// 입력 문자열에서 정규식으로 패턴을 찾아가며 파싱
	while (std::regex_search(searchStart, input.cend(), match, re)) 
	{
		bool isGlobal = match[1].str()[0] == 'g' || match[1].str()[0] == 'G';
		vec.push_back({ isGlobal ,  std::stoi(match[2]), std::stod(match[3]), std::stod(match[4])});
		searchStart = match.suffix().first; // 다음 검색 위치로 이동
	}

}

/*
 desc : Load or Save the config file (SETUP_ALIGN)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfigSetupAlign()
{
	UINT8 i;
	TCHAR tzKey[64]	= {NULL};
	TCHAR tzKey2[64] = { NULL };

	TCHAR longStrVal[2048] = { NULL };

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"SETUP_ALIGN");

	//m_pstCfg->set_align.align_method			= GetConfigUint8(L"ALIGN_METHOD");
	m_pstCfg->set_align.use_2d_cali_data		= GetConfigUint8(L"USE_2D_CALI_DATA");
	m_pstCfg->set_align.use_invalid_mark_cali	= GetConfigUint8(L"USE_INVALID_MARK_CALI");

	m_pstCfg->set_align.manualFixOffsetAtSequence = GetConfigUint8(L"MANUAL_FIX_OFFSET_AT_SEQUENCE") == 1 ? true : false;


	m_pstCfg->set_align.use_mark_offset			= GetConfigUint8(L"USE_MARK_OFFSET");
	m_pstCfg->set_align.dof_film_thick			= GetConfigDouble(L"DOF_FILM_THICK");
	m_pstCfg->set_align.mark2_org_gerb_xy[0]	= GetConfigDouble(L"MARK2_ORG_GERB_X");
	m_pstCfg->set_align.mark2_org_gerb_xy[1]	= GetConfigDouble(L"MARK2_ORG_GERB_Y");
	m_pstCfg->set_align.mark2_stage_x			= GetConfigDouble(L"MARK2_STAGE_X");

	m_pstCfg->set_align.centerMarkzeroOffsetX = GetConfigDouble(L"CENTERCAM_MARKZERO_OFFSET_X");
	m_pstCfg->set_align.centerMarkzeroOffsetY = GetConfigDouble(L"CENTERCAM_MARKZERO_OFFSET_Y");

	m_pstCfg->set_align.centerCamIdx = GetConfigUint8(L"CENTER_CAM_INDEX");
	
	for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
	{
		swprintf_s(tzKey2, MAX_KEY_STRING, L"TRIG_ONTHEFLY_DELAY_CH%d_INCREASE", i+1);
		m_pstCfg->set_align.trigOntheflyDelayIncrease[i] = GetConfigInt32(tzKey2);
		swprintf_s(tzKey2, MAX_KEY_STRING, L"TRIG_ONTHEFLY_DELAY_CH%d_DECREASE ", i + 1);
		m_pstCfg->set_align.trigOntheflyDelayDecrease[i] = GetConfigInt32(tzKey2);
		swprintf_s(tzKey2, MAX_KEY_STRING, L"TRIG_ONTHEFLY_OFFSET_CH%d_INCREASE", i + 1);
		m_pstCfg->set_align.trigOntheflyOffsetIncrease[i] = GetConfigInt32(tzKey2);
		swprintf_s(tzKey2, MAX_KEY_STRING, L"TRIG_ONTHEFLY_OFFSET_CH%d_DECREASE", i + 1);
		m_pstCfg->set_align.trigOntheflyOffsetDecrease[i] = GetConfigInt32(tzKey2);
	}

	//m_pstCfg->set_align.use_Localmark_offset = GetConfigDouble(L"USE_LOCAL_MARK_OFFSET");

	const int _1to3 = 0;
	const int _2to3 = 1;
	const int _1to3_Y_OFFSET = 2;
	const int X = 0;
	const int Y = 1;

	m_pstCfg->set_align.distCam2Cam[_1to3] = GetConfigDouble(L"DIST_CAM1TO3");
	m_pstCfg->set_align.distCam2Cam[_2to3] = GetConfigDouble(L"DIST_CAM2TO3");
	m_pstCfg->set_align.distCam2Cam[_1to3_Y_OFFSET] = GetConfigDouble(L"RELATE_Y_CAM1TO3");
	
	for (i=1; i<=m_pstCfg->luria_svc.table_count; i++)
	{
		swprintf_s(tzKey, MAX_KEY_STRING, L"TABLE_UNLOADER_X_%d", i);
		m_pstCfg->set_align.table_unloader_xy[i-1][0]	= GetConfigDouble(tzKey);
		swprintf_s(tzKey, MAX_KEY_STRING, L"TABLE_UNLOADER_Y_%d", i);
		m_pstCfg->set_align.table_unloader_xy[i-1][1]	= GetConfigDouble(tzKey);
	}

	
	if (m_pstCfg->set_align.use_mark_offset == (UINT8)ENG_ADRT::en_from_info)
	{
		const int txtLength = 2048;

		m_pstCfg->set_align.markOffsetPtr->Clear();

		vector<std::tuple<bool,int, double, double>> buff;

		GetConfigStr(L"MARK_OFFSET_FLY", longStrVal, txtLength);
		ParseAndFillVector(longStrVal, buff);
		
		std::for_each(buff.begin(), buff.end(), [&](const std::tuple < bool, int, double, double > & v)
		{
			m_pstCfg->set_align.markOffsetPtr->Push(ENG_AMOS::en_onthefly_2cam, std::get<0>(v), { std::get<1>(v), std::get<2>(v), std::get<3>(v) });
		});
		
		GetConfigStr(L"MARK_OFFSET_STA", longStrVal, txtLength);
		ParseAndFillVector(longStrVal, buff);
		std::for_each(buff.begin(), buff.end(), [&](const std::tuple<bool,int, double, double>& v)
		{
			m_pstCfg->set_align.markOffsetPtr->Push(ENG_AMOS::en_static_3cam, std::get<0>(v), { std::get<1>(v), std::get<2>(v), std::get<3>(v) });
		});
	}


	swprintf_s(tzKey, MAX_KEY_STRING, L"MARK_HORZ_DIFF");
	m_pstCfg->set_align.mark_horz_diff = GetConfigDouble(tzKey);
	swprintf_s(tzKey, MAX_KEY_STRING, L"MARK_VERT_DIFF");
	m_pstCfg->set_align.mark_vert_diff = GetConfigDouble(tzKey);

	//REFIND 관련 
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"REFIND");
	swprintf_s(tzKey, MAX_KEY_STRING, L"USE_REFIND");
	m_pstCfg->set_align.useRefind = GetConfigInt(tzKey) != 0 ? true : false;

	swprintf_s(tzKey, MAX_KEY_STRING, L"RETRY_COUNT");
	m_pstCfg->set_align.refindCnt = GetConfigInt(tzKey);

	swprintf_s(tzKey, MAX_KEY_STRING, L"THREASHOLD_DIST");
	m_pstCfg->set_align.thresholdDist = GetConfigDouble(tzKey);

	swprintf_s(tzKey, MAX_KEY_STRING, L"MOVE_OFFSET_X");
	m_pstCfg->set_align.refindOffsetX = GetConfigDouble(tzKey);

	swprintf_s(tzKey, MAX_KEY_STRING, L"MOVE_OFFSET_Y");
	m_pstCfg->set_align.refindOffsetY = GetConfigDouble(tzKey);


	return TRUE;
}

BOOL CConfUvdi15::LoadConfigHeadOffsets()
{
	m_pstCfg->headOffsets.Clear();
	TCHAR tzKey[64] = { NULL };
	TCHAR temp[MAX_PATH] = { 0, };

	swprintf_s(m_tzSubj, MAX_SUBJ_STRING, L"HEAD_OFFSET");

	GetConfigStr(L"OFFSET_TYPE", temp, MAX_PATH);

	int itemNum = 0;

	const int txtLength = 2048;

	vector<wstring> sep = {};  // wstring으로 변경

	ParseAndFillVector<basic_string<TCHAR>>(temp, _T(','), sep);


	itemNum = sep.size();
	for (const auto& v : sep)
	{
		basic_string<TCHAR> title = _T("OFFSET_") + v;
		vector<double> values = {};
		GetConfigStr(const_cast<TCHAR*>(title.c_str()), temp, MAX_PATH, _T(""));

		if (wcslen(temp) == 0)
			continue;

		ParseAndFillVector<double>(temp, _T(','), values);

		if (values.empty())
			continue;

		m_pstCfg->headOffsets.AddOffsets(std::string(v.begin(), v.end()), values);
	}


	return TRUE;
}


BOOL CConfUvdi15::LoadConfigEnvirnomental()
{

	TCHAR tzKey[64] = { NULL };

	/* Subject Name 설정 */
	swprintf_s(m_tzSubj, MAX_SUBJ_STRING, L"ENVIRONMENT");

	m_pstCfg->environmental.useEnviroment = GetConfigInt(L"USE_ENVIRONMENT") != 0 ? true : false;

	m_pstCfg->environmental.caliTempGab = GetConfigDouble(L"RE_CALIB_GAB_TEMP");
	m_pstCfg->environmental.caliTimeMinuteTerm = GetConfigDouble(L"RE_CALIB_TERM_MINUTE");

	m_pstCfg->environmental.lastCalibTemperature = GetConfigDouble(L"LAST_CALIB_TEMP");
	m_pstCfg->environmental.lastCalibDate = GetConfigDouble(L"LAST_CALIB_DATE");

	m_pstCfg->environmental.calibStageX = GetConfigDouble(L"CALIB_STAGE_X");
	m_pstCfg->environmental.calibStageY = GetConfigDouble(L"CALIB_STAGE_Y");

	m_pstCfg->environmental.calibCamIdx = GetConfigInt(L"CALIB_CAM_IDX");
	m_pstCfg->environmental.calibCamDriveAxisPos = GetConfigDouble(L"CALIB_CAM_X");
	m_pstCfg->environmental.calibCamZAxisPos = GetConfigDouble(L"CALIB_CAM_Z");

	m_pstCfg->environmental.indacatorOffsetX = GetConfigDouble(L"INDICATOR_OFFSET_X");
	m_pstCfg->environmental.indacatorOffsetY = GetConfigDouble(L"INDICATOR_OFFSET_Y");
 
	TCHAR temp[MAX_PATH] = { 0, };
	GetConfigStr(L"THC_LOG_LOCATION", temp,MAX_PATH,L"");
	strcpy_s(m_pstCfg->environmental.thcLogLocation, MAX_PATH, CT2A(temp));
	

	return TRUE;
}


BOOL CConfUvdi15::SaveConfigEnvirnomental()
{
	 
	 
	return TRUE;
}


BOOL CConfUvdi15::SaveConfigSetupAlign()
{
	UINT8 i;
	TCHAR tzKey[64]	= {NULL};

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"SETUP_ALIGN");

	//SetConfigUint32(L"ALIGN_METHOD",			m_pstCfg->set_align.align_method);
	SetConfigUint32(L"USE_2D_CALI_DATA",		m_pstCfg->set_align.use_2d_cali_data);
	SetConfigUint32(L"CENTER_CAM_INDEX", m_pstCfg->set_align.centerCamIdx);
	SetConfigDouble(L"DOF_FILM_THICK",			m_pstCfg->set_align.dof_film_thick,	4);

	SetConfigDouble(L"MARK2_ORG_GERB_X",		m_pstCfg->set_align.mark2_org_gerb_xy[0],	4);
	SetConfigDouble(L"MARK2_ORG_GERB_Y",		m_pstCfg->set_align.mark2_org_gerb_xy[1],	4);
	SetConfigDouble(L"MARK2_STAGE_X",			m_pstCfg->set_align.mark2_stage_x,			4);

	SetConfigDouble(L"DIST_CAM1TO3", m_pstCfg->set_align.distCam2Cam[0], 4);
	SetConfigDouble(L"DIST_CAM2TO3", m_pstCfg->set_align.distCam2Cam[1], 4);
	SetConfigDouble(L"RELATE_Y_CAM1TO3", m_pstCfg->set_align.distCam2Cam[2], 4);

	for (i=1; i<=m_pstCfg->luria_svc.table_count; i++)
	{
		swprintf_s(tzKey, MAX_KEY_STRING, L"TABLE_UNLOADER_X_%d", i);
		SetConfigDouble(tzKey,	m_pstCfg->set_align.table_unloader_xy[i-1][0],	4);
		swprintf_s(tzKey, MAX_KEY_STRING, L"TABLE_UNLOADER_Y_%d", i);
		SetConfigDouble(tzKey,	m_pstCfg->set_align.table_unloader_xy[i-1][1],	4);
	}

	swprintf_s(tzKey, MAX_KEY_STRING, L"MARK_HORZ_DIFF");
	SetConfigDouble(tzKey, m_pstCfg->set_align.mark_horz_diff, 4);
	swprintf_s(tzKey, MAX_KEY_STRING, L"MARK_VERT_DIFF");
	SetConfigDouble(tzKey, m_pstCfg->set_align.mark_vert_diff, 4);

	
	string flyStr = m_pstCfg->set_align.markOffsetPtr->GetRegexString(ENG_AMOS::en_onthefly_2cam);
	string staStr = m_pstCfg->set_align.markOffsetPtr->GetRegexString(ENG_AMOS::en_static_3cam);
	
	USES_CONVERSION;
	SetConfigStr(L"MARK_OFFSET_FLY", A2T(flyStr.c_str()));
	SetConfigStr(L"MARK_OFFSET_STA", A2T(staStr.c_str()));

	return TRUE;
}

/*
 desc : Load or Save the config file (GRAB_PRE_PROC)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfigGrabProc()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"GRAB_PRE_PROC");

	m_pstCfg->grab_proc.use_grab_pre_proc		= GetConfigUint8(L"USE_GRAB_PRE_PROC");
	m_pstCfg->grab_proc.use_noise_remove		= GetConfigUint8(L"USE_NOISE_REMOVE");
	m_pstCfg->grab_proc.use_thin_operation		= GetConfigUint8(L"USE_THIN_OPERATION");
	m_pstCfg->grab_proc.use_small_particle		= GetConfigUint8(L"USE_SMALL_PARTICLE");
	m_pstCfg->grab_proc.mim_bin_condition		= GetConfigUint8(L"MIM_BIN_CONDITION");
	m_pstCfg->grab_proc.remove_small_particle	= GetConfigUint8(L"REMOVE_SMALL_PARTICLE");
	m_pstCfg->grab_proc.remove_small_procmode	= GetConfigUint16(L"REMOVE_SMALL_PROCMODE");
	m_pstCfg->grab_proc.thin_proc_mode			= GetConfigUint16(L"THIN_PROC_MODE");

	return TRUE;
}
BOOL CConfUvdi15::SaveConfigGrabProc()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"GRAB_PRE_PROC");

	SetConfigUint32(L"USE_GRAB_PRE_PROC",		m_pstCfg->grab_proc.use_grab_pre_proc);
	SetConfigUint32(L"USE_NOISE_REMOVE",		m_pstCfg->grab_proc.use_noise_remove);
	SetConfigUint32(L"USE_THIN_OPERATION",		m_pstCfg->grab_proc.use_thin_operation);
	SetConfigUint32(L"USE_SMALL_PARTICLE",		m_pstCfg->grab_proc.use_small_particle);
	SetConfigUint32(L"MIM_BIN_CONDITION",		m_pstCfg->grab_proc.mim_bin_condition);
	SetConfigUint32(L"REMOVE_SMALL_PARTICLE",	m_pstCfg->grab_proc.remove_small_particle);
	SetConfigUint32(L"REMOVE_SMALL_PROCMODE",	m_pstCfg->grab_proc.remove_small_procmode);
	SetConfigUint32(L"THIN_PROC_MODE",			m_pstCfg->grab_proc.thin_proc_mode);

	return TRUE;
}
#if 0
/*
 desc : Load or Save the config file (CALI_FIND)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfigCaliFind()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"CALI_FIND");

	m_pstCfg->cali_find.model_type	= GetConfigUint8(L"MODEL_TYPE");
	m_pstCfg->cali_find.model_color	= GetConfigDouble(L"MODEL_COLOR");
	m_pstCfg->cali_find.model_cali	= GetConfigDouble(L"MODEL_CALI");

	return TRUE;
}
BOOL CConfUvdi15::SaveConfigCaliFind()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"CALI_FIND");

	SetConfigUint32(L"MODEL_TYPE",	m_pstCfg->cali_find.model_type);
	SetConfigDouble(L"MODEL_COLOR",	m_pstCfg->cali_find.model_color);
	SetConfigDouble(L"MODEL_CALI",	m_pstCfg->cali_find.model_cali);

	return TRUE;
}
#endif
/*
 desc : Load or Save the config file (MARK_FIND)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfigMarkFind()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"MARK_FIND");

	m_pstCfg->mark_find.max_mark_regist		= GetConfigUint8(L"MAX_MARK_REGIST");
	m_pstCfg->mark_find.max_mark_grab		= GetConfigUint8(L"MAX_MARK_GRAB");
	m_pstCfg->mark_find.max_mark_find		= GetConfigUint8(L"MAX_MARK_FIND");
	m_pstCfg->mark_find.model_speed			= GetConfigUint8(L"MODEL_SPEED");
	m_pstCfg->mark_find.mixed_cent_type		= GetConfigUint8(L"MIXED_MARK_CENT_TYPE");
	m_pstCfg->mark_find.detail_level		= GetConfigUint8(L"DETAIL_LEVEL");
	m_pstCfg->mark_find.mixed_squre_rate	= GetConfigUint16(L"MIXED_SQUARE_RATE");
	m_pstCfg->mark_find.mixed_weight_rate	= GetConfigUint16(L"MIXED_WEIGHT_RATE");
	m_pstCfg->mark_find.model_smooth		= GetConfigDouble(L"MODEL_SMOOTH");
	m_pstCfg->mark_find.score_rate			= GetConfigDouble(L"SCORE_RATE");
	m_pstCfg->mark_find.scale_rate			= GetConfigDouble(L"SCALE_RATE");
	m_pstCfg->mark_find.mixed_dist_limit	= GetConfigDouble(L"MIXED_DIST_LIMIT");
	m_pstCfg->mark_find.image_process		= GetConfigDouble(L"IMAGE_PROCESS");

	return TRUE;
}
BOOL CConfUvdi15::SaveConfigMarkFind()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"MARK_FIND");

	SetConfigUint32(L"MAX_MARK_REGIST",		m_pstCfg->mark_find.max_mark_regist);
	SetConfigUint32(L"MAX_MARK_GRAB",		m_pstCfg->mark_find.max_mark_grab);
	SetConfigUint32(L"MAX_MARK_FIND",		m_pstCfg->mark_find.max_mark_find);
	SetConfigUint32(L"MIXED_MARK_CENT_TYPE",m_pstCfg->mark_find.mixed_cent_type);
	SetConfigUint32(L"DETAIL_LEVEL",		m_pstCfg->mark_find.detail_level);
	SetConfigUint32(L"MIXED_SQUARE_RATE",	m_pstCfg->mark_find.mixed_squre_rate);
	SetConfigUint32(L"MIXED_WEIGHT_RATE",	m_pstCfg->mark_find.mixed_weight_rate);

	SetConfigDouble(L"MODEL_SMOOTH",		m_pstCfg->mark_find.model_smooth, 4);
	SetConfigDouble(L"SCORE_RATE",			m_pstCfg->mark_find.score_rate, 4);
	SetConfigDouble(L"SCALE_RATE",			m_pstCfg->mark_find.scale_rate, 4);
	SetConfigDouble(L"MIXED_DIST_LIMIT",	m_pstCfg->mark_find.mixed_dist_limit,	3);

	SetConfigUint32(L"IMAGE_PROCESS", m_pstCfg->mark_find.image_process);

	return TRUE;
}

/*
 desc : Load or Save the config file (EDGE_FIND)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfigEdgeFind()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"EDGE_FIND");

	m_pstCfg->edge_find.filter_type			= GetConfigUint8(L"FILTER_TYPE");
	m_pstCfg->edge_find.threshold			= GetConfigUint8(L"THRESHOLD");
	m_pstCfg->edge_find.gaussian_filer		= GetConfigUint8(L"GAUSSIAN_FILTER");
	m_pstCfg->edge_find.use_float_mode		= GetConfigUint8(L"USE_FLOAT_MODE");
	m_pstCfg->edge_find.use_closed_line		= GetConfigUint8(L"USE_CLOSED_LINE");
	m_pstCfg->edge_find.inc_internal_edge	= GetConfigUint8(L"INC_INTERNAL_EDGE");
	m_pstCfg->edge_find.edge_analysis_save	= GetConfigUint8(L"EDGE_ANALYSIS_SAVE");

	m_pstCfg->edge_find.m_accuracy			= GetConfigInt32(L"M_ACCURACY");

	m_pstCfg->edge_find.filter_smoothness	= GetConfigUint32(L"FILTER_SMOOTHNESS");
	m_pstCfg->edge_find.max_find_count		= GetConfigUint32(L"MAX_FIND_COUNT");

	m_pstCfg->edge_find.min_dia_size		= GetConfigDouble(L"MIN_DIA_LENGTH");
	m_pstCfg->edge_find.max_dia_size		= GetConfigDouble(L"MAX_DIA_LENGTH");
	m_pstCfg->edge_find.extraction_scale	= GetConfigDouble(L"M_EXTRACTION_SCALE");
	m_pstCfg->edge_find.moment_elongation	= GetConfigDouble(L"M_MOMENT_ELONGATION");
	m_pstCfg->edge_find.mark_spare_size		= GetConfigDouble(L"MARK_SPARE_SIZE");
	m_pstCfg->edge_find.fill_gap_continuity	= GetConfigDouble(L"FILL_GAP_CONTINUITY");

	return TRUE;
}
BOOL CConfUvdi15::SaveConfigEdgeFind()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"EDGE_FIND");

	/* Edge Detection */
	SetConfigInt32(L"M_ACCURACY",			m_pstCfg->edge_find.m_accuracy);

	SetConfigUint32(L"FILTER_TYPE",			m_pstCfg->edge_find.filter_type);
	SetConfigUint32(L"THRESHOLD",			m_pstCfg->edge_find.threshold);
	SetConfigUint32(L"GAUSSIAN_FILTER",		m_pstCfg->edge_find.gaussian_filer);
	SetConfigUint32(L"USE_FLOAT_MODE",		m_pstCfg->edge_find.use_float_mode);
	SetConfigUint32(L"USE_CLOSED_LINE",		m_pstCfg->edge_find.use_closed_line);
	SetConfigUint32(L"INC_INTERNAL_EDGE",	m_pstCfg->edge_find.inc_internal_edge);
	SetConfigUint32(L"EDGE_ANALYSIS_SAVE",	m_pstCfg->edge_find.edge_analysis_save);

	SetConfigUint32(L"FILTER_SMOOTHNESS",	m_pstCfg->edge_find.filter_smoothness);
	SetConfigUint32(L"MAX_FIND_COUNT",		m_pstCfg->edge_find.max_find_count);

	SetConfigDouble(L"MIN_DIA_LENGTH",		m_pstCfg->edge_find.min_dia_size,		3);
	SetConfigDouble(L"MAX_DIA_LENGTH",		m_pstCfg->edge_find.max_dia_size,		3);
	SetConfigDouble(L"M_EXTRACTION_SCALE",	m_pstCfg->edge_find.extraction_scale,	6);
	SetConfigDouble(L"M_MOMENT_ELONGATION",	m_pstCfg->edge_find.moment_elongation,	6);
	SetConfigDouble(L"MARK_SPARE_SIZE",		m_pstCfg->edge_find.mark_spare_size,	3);
	SetConfigDouble(L"FILL_GAP_CONTINUITY",	m_pstCfg->edge_find.fill_gap_continuity,3);

	return TRUE;
}

/*
 desc : Load or Save the config file (LINE_FIND)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfigLineFind()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"LINE_FIND");

	GetConfigStr(L"TEXT_FONT_NAME", m_pstCfg->line_find.text_font_name, LF_FACESIZE);

	m_pstCfg->line_find.part_expose_repeat		= GetConfigUint8(L"PART_EXPOSE_REPEAT");
	m_pstCfg->line_find.box_angle_mode			= GetConfigUint8(L"BOX_ANGLE_MODE");
	m_pstCfg->line_find.edge_find_option		= GetConfigUint8(L"EDGE_FIND_OPTION");
	m_pstCfg->line_find.search_region_clip		= GetConfigUint8(L"SEARCH_REGION_CLIP");
	m_pstCfg->line_find.filter_type				= GetConfigUint8(L"FILTER_TYPE");
	m_pstCfg->line_find.polarity				= GetConfigUint8(L"POLARITY");
	m_pstCfg->line_find.orientation				= GetConfigUint8(L"ORIENTATION");

	m_pstCfg->line_find.filter_smoothness		= GetConfigUint8(L"FILTER_SMOOTHNESS");
	m_pstCfg->line_find.edge_threshold			= GetConfigUint8(L"EDGE_THRESHOLD");
	m_pstCfg->line_find.sub_regions_size		= GetConfigUint8(L"SUB_REGIONS_SIZE");
	m_pstCfg->line_find.sub_regions_number		= GetConfigUint8(L"SUB_REGIONS_NUMBER");
	m_pstCfg->line_find.max_find_count			= GetConfigUint8(L"MAX_FIND_COUNT");
	m_pstCfg->line_find.text_font_size			= GetConfigUint8(L"TEXT_FONT_SIZE");
	m_pstCfg->line_find.box_size_width			= GetConfigUint8(L"BOX_SIZE_WIDTH");
	m_pstCfg->line_find.box_size_height			= GetConfigUint8(L"BOX_SIZE_HEIGHT");
	m_pstCfg->line_find.box_angle_delta_neg		= GetConfigUint8(L"BOX_ANGLE_DELTA_NEG");
	m_pstCfg->line_find.box_angle_delta_pos		= GetConfigUint8(L"BOX_ANGLE_DELTA_POS");

	m_pstCfg->line_find.sub_regions_offset		= GetConfigInt8(L"SUB_REGIONS_OFFSET");

	m_pstCfg->line_find.box_angle				= GetConfigUint16(L"BOX_ANGLE");

	m_pstCfg->line_find.grab_image_edge_angle	= GetConfigDouble(L"GRAB_IMAGE_EDGE_ANGLE");
	m_pstCfg->line_find.box_angle_accuracy		= GetConfigDouble(L"BOX_ANGLE_ACCURACY");
	m_pstCfg->line_find.print_diff_limit		= GetConfigDouble(L"PRINT_DIFF_LIMIT");

	return TRUE;
}
BOOL CConfUvdi15::SaveConfigLineFind()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"LINE_FIND");

	SetConfigStr(L"TEXT_FONT_NAME",				m_pstCfg->line_find.text_font_name);

	SetConfigInt32(L"SUB_REGIONS_OFFSET",		m_pstCfg->line_find.sub_regions_offset);

	SetConfigUint32(L"MAX_FIND_COUNT",			m_pstCfg->line_find.max_find_count);
	SetConfigUint32(L"PART_EXPOSE_REPEAT",		m_pstCfg->line_find.part_expose_repeat);
	SetConfigUint32(L"EDGE_FIND_OPTION",		m_pstCfg->line_find.edge_find_option);
	SetConfigUint32(L"TEXT_FONT_SIZE",			m_pstCfg->line_find.text_font_size);
	SetConfigUint32(L"SEARCH_REGION_CLIP",		m_pstCfg->line_find.search_region_clip);
	SetConfigUint32(L"BOX_SIZE_WIDTH",			m_pstCfg->line_find.box_size_width);
	SetConfigUint32(L"BOX_SIZE_HEIGHT",			m_pstCfg->line_find.box_size_height);
	SetConfigUint32(L"FILTER_TYPE",				m_pstCfg->line_find.filter_type);
	SetConfigUint32(L"FILTER_SMOOTHNESS",		m_pstCfg->line_find.filter_smoothness);
	SetConfigUint32(L"EDGE_THRESHOLD",			m_pstCfg->line_find.edge_threshold);
	SetConfigUint32(L"POLARITY",				m_pstCfg->line_find.polarity);
	SetConfigUint32(L"SUB_REGIONS_SIZE",		m_pstCfg->line_find.sub_regions_size);
	SetConfigUint32(L"SUB_REGIONS_NUMBER",		m_pstCfg->line_find.sub_regions_number);
	SetConfigUint32(L"ORIENTATION",				m_pstCfg->line_find.orientation);
	SetConfigUint32(L"BOX_ANGLE_MODE",			m_pstCfg->line_find.box_angle_mode);
	SetConfigUint32(L"BOX_ANGLE",				m_pstCfg->line_find.box_angle);
	SetConfigUint32(L"BOX_ANGLE_DELTA_NEG",		m_pstCfg->line_find.box_angle_delta_neg);
	SetConfigUint32(L"BOX_ANGLE_DELTA_POS",		m_pstCfg->line_find.box_angle_delta_pos);

	SetConfigDouble(L"GRAB_IMAGE_EDGE_ANGLE",	m_pstCfg->line_find.grab_image_edge_angle,	3);
	SetConfigDouble(L"BOX_ANGLE_ACCURACY",		m_pstCfg->line_find.box_angle_accuracy,		2);
	SetConfigDouble(L"PRINT_DIFF_LIMIT",		m_pstCfg->line_find.print_diff_limit,		3);

	return TRUE;
}

/*
 desc : Load or Save the config file (LED_POWER)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfigLedPower()
{
	TCHAR tzKey[64] = { NULL };
	CString strTemp;

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"LED_POWER");

	for (int i = 0; i < 6; i++)
	{
		strTemp.Format(L"MEASURE_POS%d_X", i + 1);
		m_pstCfg->led_power.measure_pos[i][0] = GetConfigDouble((PTCHAR)(LPCTSTR)strTemp);
		strTemp.Format(L"MEASURE_POS%d_Y", i + 1);
		m_pstCfg->led_power.measure_pos[i][1] = GetConfigDouble((PTCHAR)(LPCTSTR)strTemp);
		strTemp.Format(L"MEASURE_POS%d_Z", i + 1);
		m_pstCfg->led_power.measure_pos[i][2] = GetConfigDouble((PTCHAR)(LPCTSTR)strTemp);
	}

	m_pstCfg->led_power.begin_power_index	= GetConfigUint16(L"BEGIN_POWER_INDEX");
	m_pstCfg->led_power.measure_count		= GetConfigUint16(L"MEASURE_COUNT");
	m_pstCfg->led_power.interval_power		= GetConfigUint16(L"INTERVAL_POWER");
	m_pstCfg->led_power.max_points			= GetConfigUint8(L"MAX_POINTS");
	m_pstCfg->led_power.pitch_pos			= GetConfigDouble(L"PITCH_POS");
	m_pstCfg->led_power.stage_speed			= GetConfigDouble(L"STAGE_SPEED");
	m_pstCfg->led_power.head_speed			= GetConfigDouble(L"HEAD_SPEED");
	m_pstCfg->led_power.manual_power_index	= GetConfigUint16(L"MANUAL_POWER_INDEX");
	m_pstCfg->led_power.normal_boundary		= GetConfigDouble(L"NORMAL_BOUNDARY");
	m_pstCfg->led_power.warning_boundary	= GetConfigDouble(L"WARNING_BOUNDARY");

	return TRUE;
}
BOOL CConfUvdi15::SaveConfigLedPower()
{
	TCHAR tzKey[64] = { NULL };
	CString strTemp;

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"LED_POWER");

	for (int i = 0; i < 6; i++)
	{
		strTemp.Format(L"MEASURE_POS%d_X", i + 1);
		SetConfigDouble((PTCHAR)(LPCTSTR)strTemp, m_pstCfg->led_power.measure_pos[i][0], 4);
		strTemp.Format(L"MEASURE_POS%d_Y", i + 1);
		SetConfigDouble((PTCHAR)(LPCTSTR)strTemp, m_pstCfg->led_power.measure_pos[i][1], 4);
		strTemp.Format(L"MEASURE_POS%d_Z", i + 1);
		SetConfigDouble((PTCHAR)(LPCTSTR)strTemp, m_pstCfg->led_power.measure_pos[i][2], 4);
	}

	SetConfigUint32(L"BEGIN_POWER_INDEX", m_pstCfg->led_power.begin_power_index);
	SetConfigUint32(L"MEASURE_COUNT", m_pstCfg->led_power.measure_count);
	SetConfigUint32(L"INTERVAL_POWER", m_pstCfg->led_power.interval_power);
	SetConfigUint32(L"MAX_POINTS", m_pstCfg->led_power.max_points);
	SetConfigDouble(L"PITCH_POS", m_pstCfg->led_power.pitch_pos, 4);
	SetConfigDouble(L"STAGE_SPEED", m_pstCfg->led_power.stage_speed, 3);
	SetConfigDouble(L"HEAD_SPEED", m_pstCfg->led_power.head_speed, 3);
	SetConfigUint32(L"MANUAL_POWER_INDEX", m_pstCfg->led_power.manual_power_index);
	SetConfigDouble(L"NORMAL_BOUNDARY", m_pstCfg->led_power.normal_boundary, 3);
	SetConfigDouble(L"WARNING_BOUNDARY", m_pstCfg->led_power.warning_boundary, 3);

	return TRUE;
}

/*
 desc : Load or Save the config file (PH_FOCUS)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfigPhFocus()
{
	TCHAR tzKey[64]	= {NULL};

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"PH_FOCUS");

	m_pstCfg->ph_focus.print_type		= GetConfigUint8(L"PRINT_TYPE");
	
	m_pstCfg->ph_focus.step_x_count		= GetConfigUint16(L"STEP_X_COUNT");
	m_pstCfg->ph_focus.step_y_count		= GetConfigUint16(L"STEP_Y_COUNT");

	for (int i = 0; i < MAX_PH; i++)
	{
		swprintf_s(tzKey, 64, L"START_FOCUS_%d", i + 1);
		m_pstCfg->ph_focus.start_foucs[i] = GetConfigDouble(tzKey);
	}
//	m_pstCfg->ph_focus.start_foucs		= GetConfigUint16(L"START_FOCUS_");
	m_pstCfg->ph_focus.start_energy		= GetConfigUint16(L"START_ENERGY");
	m_pstCfg->ph_focus.step_foucs		= GetConfigDouble(L"STEP_FOCUS");
	m_pstCfg->ph_focus.step_energy		= GetConfigUint16(L"STEP_ENERGY");

	m_pstCfg->ph_focus.expo_stage[0]	= GetConfigDouble(L"EXPO_STAGE_X");
	m_pstCfg->ph_focus.expo_stage[1]	= GetConfigDouble(L"EXPO_STAGE_Y");

	m_pstCfg->ph_focus.mark2_stage[0]	= GetConfigDouble(L"MARK_STAGE_X");
	m_pstCfg->ph_focus.mark2_stage[1]	= GetConfigDouble(L"MARK_STAGE_Y");
	m_pstCfg->ph_focus.mark2_acam_x[0]	= GetConfigDouble(L"MARK_ACAM1_X");
	m_pstCfg->ph_focus.mark2_acam_x[1]	= GetConfigDouble(L"MARK_ACAM2_X");
	m_pstCfg->ph_focus.step_move_x		= GetConfigDouble(L"STEP_MOVE_X");
	m_pstCfg->ph_focus.step_move_y		= GetConfigDouble(L"STEP_MOVE_Y");

	m_pstCfg->ph_focus.mark_period		= GetConfigDouble(L"PH_VS_PH_DIST");

	m_pstCfg->ph_focus.model_dia_size	= GetConfigDouble(L"MODEL_DIA_SIZE");
	m_pstCfg->ph_focus.model_color		= GetConfigDouble(L"MODEL_COLOR");

	return TRUE;
}

BOOL CConfUvdi15::SaveConfigPhFocus()
{
	TCHAR tzKey[64]	= {NULL};

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"PH_FOCUS");

	SetConfigUint32(L"PRINT_TYPE", m_pstCfg->ph_focus.print_type);
	SetConfigUint32(L"STEP_X_COUNT",	m_pstCfg->ph_focus.step_x_count);
	SetConfigUint32(L"STEP_Y_COUNT",	m_pstCfg->ph_focus.step_y_count);

	for (int i = 0; i < MAX_PH; i++)
	{
		swprintf_s(tzKey, 64, L"START_FOCUS_%d", i + 1);
		SetConfigDouble(tzKey, m_pstCfg->ph_focus.start_foucs[i], 3);
	}
//	SetConfigDouble(L"START_FOCUS",		m_pstCfg->ph_focus.start_foucs, 3);
	SetConfigDouble(L"START_ENERGY",	m_pstCfg->ph_focus.start_energy, 3);
	SetConfigDouble(L"STEP_FOCUS",		m_pstCfg->ph_focus.step_foucs, 3);
	SetConfigDouble(L"STEP_ENERGY",		m_pstCfg->ph_focus.step_energy, 3);

	SetConfigDouble(L"EXPO_STAGE_X",	m_pstCfg->ph_focus.expo_stage[0],	3);
	SetConfigDouble(L"EXPO_STAGE_Y",	m_pstCfg->ph_focus.expo_stage[1],	3);

	SetConfigDouble(L"MARK_STAGE_X",	m_pstCfg->ph_focus.mark2_stage[0],	4);
	SetConfigDouble(L"MARK_STAGE_Y",	m_pstCfg->ph_focus.mark2_stage[1],	4);
	SetConfigDouble(L"MARK_ACAM1_X",	m_pstCfg->ph_focus.mark2_acam_x[0],	4);
	SetConfigDouble(L"MARK_ACAM2_X",	m_pstCfg->ph_focus.mark2_acam_x[1],	4);
	SetConfigDouble(L"STEP_MOVE_X",		m_pstCfg->ph_focus.step_move_x,		4);
	SetConfigDouble(L"STEP_MOVE_Y",		m_pstCfg->ph_focus.step_move_y,		4);

	SetConfigDouble(L"PH_VS_PH_DIST",	m_pstCfg->ph_focus.mark_period,		4);

	SetConfigDouble(L"MODEL_DIA_SIZE",	m_pstCfg->ph_focus.model_dia_size,	3);
	SetConfigDouble(L"MODEL_COLOR",		m_pstCfg->ph_focus.model_color,		3);

	return TRUE;
}

/*
 desc : Load or Save the config file (ACAM_CALI)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfigACamCali()
{
	TCHAR tzKey[64]	= {NULL};
	UINT8 i	= 0x00;

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"ACAM_CALI");

	m_pstCfg->acam_cali.set_match_mode		= GetConfigUint8(L"SET_MATCH_MODE");

	m_pstCfg->acam_cali.max_rows_count		= GetConfigUint16(L"MAX_ROWS_COUNT");
	m_pstCfg->acam_cali.max_cols_count		= GetConfigUint16(L"MAX_COLS_COUNT");

	m_pstCfg->acam_cali.model_shut_type		= GetConfigUint32(L"MODEL_SHUT_TYPE");
	m_pstCfg->acam_cali.model_ring_type		= GetConfigUint32(L"MODEL_RING_TYPE");   // 최외곽 도형이 꼭 링이된다는 보장은 없음 직사각형도 가능

	m_pstCfg->acam_cali.period_min_size		= GetConfigDouble(L"MIN_PERIOD_SIZE");

	m_pstCfg->acam_cali.model_shut_size		= GetConfigDouble(L"MODEL_SHUT_SIZE");
	m_pstCfg->acam_cali.model_ring_size1	= GetConfigDouble(L"MODEL_RING_SIZE1");
	m_pstCfg->acam_cali.model_ring_size2	= GetConfigDouble(L"MODEL_RING_SIZE2");

	m_pstCfg->acam_cali.model_shut_color	= GetConfigDouble(L"MODEL_SHUT_COLOR");
	m_pstCfg->acam_cali.model_ring_color	= GetConfigDouble(L"MODEL_RING_COLOR");

	m_pstCfg->acam_cali.period_wait_time	= GetConfigUint64(L"PERIOD_WAIT_TIME");

	m_pstCfg->acam_cali.score_valid_rate	= GetConfigDouble(L"SCORE_VALID_RATE");
	m_pstCfg->acam_cali.scale_valid_rate	= GetConfigDouble(L"SCALE_VALID_RATE");

	m_pstCfg->acam_cali.motion_speed		= GetConfigDouble(L"MOTION_SPEED");
	m_pstCfg->acam_cali.err_limit_value		= GetConfigDouble(L"ERR_LIMIT_VALUE");
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
	m_pstCfg->acam_cali.mark_stage_x		= GetConfigDouble(L"MARK_STAGE_X");
#endif
	for (i=0x00; i<m_pstCfg->set_cams.acam_count; i++)
	{
#if (0x01 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
		swprintf_s(tzKey, 64, L"MARK_STAGE_X%d", i+1);
		m_pstCfg->acam_cali.mark_stage_x[i]	= GetConfigDouble(tzKey);
#endif
		swprintf_s(tzKey, 64, L"MARK_STAGE_Y%d", i+1);
		m_pstCfg->acam_cali.mark_stage_y[i]	= GetConfigDouble(tzKey);
		swprintf_s(tzKey, 64, L"MARK_ACAM_%d", i+1);
		m_pstCfg->acam_cali.mark_acam[i]	= GetConfigDouble(tzKey);



	}

	return TRUE;
}
BOOL CConfUvdi15::SaveConfigACamCali()
{
	TCHAR tzKey[64]	= {NULL};
	UINT8 i	= 0x00;

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"ACAM_CALI");

	SetConfigUint32(L"SET_MATCH_MODE",		m_pstCfg->acam_cali.set_match_mode);

	SetConfigUint32(L"MAX_ROWS_COUNT",		m_pstCfg->acam_cali.max_rows_count);
	SetConfigUint32(L"MAX_COLS_COUNT",		m_pstCfg->acam_cali.max_cols_count);

	SetConfigUint32(L"MODEL_SHUT_TYPE",		m_pstCfg->acam_cali.model_shut_type);
	SetConfigUint32(L"MODEL_RING_TYPE",		m_pstCfg->acam_cali.model_ring_type);

	SetConfigUint64(L"PERIOD_WAIT_TIME",	m_pstCfg->acam_cali.period_wait_time);

	SetConfigDouble(L"MIN_PERIOD_SIZE",		m_pstCfg->acam_cali.period_min_size,	4);

	SetConfigDouble(L"MODEL_SHUT_SIZE",		m_pstCfg->acam_cali.model_shut_size,	4);
	SetConfigDouble(L"MODEL_RING_SIZE1",	m_pstCfg->acam_cali.model_ring_size1,	4);
	SetConfigDouble(L"MODEL_RING_SIZE2",	m_pstCfg->acam_cali.model_ring_size2,	4);

	SetConfigDouble(L"MODEL_SHUT_COLOR",	m_pstCfg->acam_cali.model_shut_color,	3);
	SetConfigDouble(L"MODEL_RING_COLOR",	m_pstCfg->acam_cali.model_ring_color,	3);

	SetConfigDouble(L"SCORE_VALID_RATE",	m_pstCfg->acam_cali.score_valid_rate,	4);
	SetConfigDouble(L"SCALE_VALID_RATE",	m_pstCfg->acam_cali.scale_valid_rate,	4);

	SetConfigDouble(L"MOTION_SPEED",		m_pstCfg->acam_cali.motion_speed,		4);
	SetConfigDouble(L"ERR_LIMIT_VALUE",		m_pstCfg->acam_cali.err_limit_value,	4);
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
	SetConfigDouble(L"MARK_STAGE_X",		m_pstCfg->acam_cali.mark_stage_x,		4);
#endif
	for (i=0x00; i<m_pstCfg->set_cams.acam_count; i++)
	{
#if (0x01 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
		swprintf_s(tzKey, 64, L"MARK_STAGE_X%d", i+1);
		SetConfigDouble(tzKey,	m_pstCfg->acam_cali.mark_stage_x[i],4);
#endif
		swprintf_s(tzKey, 64, L"MARK_STAGE_Y%d", i+1);
		SetConfigDouble(tzKey,	m_pstCfg->acam_cali.mark_stage_y[i],4);

		swprintf_s(tzKey, 64, L"MARK_ACAM_%d", i+1);
		SetConfigDouble(tzKey,	m_pstCfg->acam_cali.mark_acam[i],	4);
	}

	return TRUE;
}

/*
 desc : Load or Save the config file (ACAM_SPEC)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfigACamSpec()
{
	UINT8 i	= 0x00;
	TCHAR tzKey[64]	= {NULL};

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"ACAM_SPEC");

	m_pstCfg->acam_spec.step_move_z			= GetConfigDouble(L"STEP_MOVE_Z");
	m_pstCfg->acam_spec.quartz_stage_x		= GetConfigDouble(L"QUARTZ_STAGE_X");
	m_pstCfg->acam_spec.bracket_size_w		= GetConfigDouble(L"BRACKET_SIZE_W");
	m_pstCfg->acam_spec.bracket_size_d		= GetConfigDouble(L"BRACKET_SIZE_D");
	m_pstCfg->acam_spec.point_dist			= GetConfigDouble(L"POINT_DIST");
	m_pstCfg->acam_spec.in_ring_size		= GetConfigDouble(L"IN_RING_SIZE");
	m_pstCfg->acam_spec.in_ring_color		= GetConfigDouble(L"IN_RING_COLOR");
	m_pstCfg->acam_spec.in_ring_scale_min	= GetConfigDouble(L"IN_RING_SCALE_MIN");
	m_pstCfg->acam_spec.in_ring_scale_max	= GetConfigDouble(L"IN_RING_SCALE_MAX");

	for (i=0x0; i<m_pstCfg->set_cams.acam_count; i++)
	{
		swprintf_s(tzKey, 64, L"ACAM_Z_FOCUS_%d", i+1);
		m_pstCfg->acam_spec.acam_z_focus[i]	= GetConfigDouble(tzKey);

		swprintf_s(tzKey, 64, L"QUARTZ_STAGE_Y%d", i+1);
		m_pstCfg->acam_spec.quartz_stage_y[i]= GetConfigDouble(tzKey);

		swprintf_s(tzKey, 64, L"QUARTZ_ACAM_%d", i+1);
		m_pstCfg->acam_spec.quartz_acam[i]	= GetConfigDouble(tzKey);

		swprintf_s(tzKey, 64, L"ACAM_ANGLE_%d", i+1);
		m_pstCfg->acam_spec.spec_angle[i]	= GetConfigDouble(tzKey);
		swprintf_s(tzKey, 64, L"ACAM_PIXEL_UM_%d", i+1);
		m_pstCfg->acam_spec.spec_pixel_um[i]= GetConfigDouble(tzKey);
	}

	return TRUE;
}
BOOL CConfUvdi15::SaveConfigACamSpec()
{
	UINT8 i	= 0x00;
	TCHAR tzKey[64]	= {NULL};

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"ACAM_SPEC");

	SetConfigDouble(L"STEP_MOVE_Z",		m_pstCfg->acam_spec.step_move_z,	3);
	SetConfigDouble(L"QUARTZ_STAGE_X",	m_pstCfg->acam_spec.quartz_stage_x,	4);
	SetConfigDouble(L"BRACKET_SIZE_W",	m_pstCfg->acam_spec.bracket_size_w,	4);
	SetConfigDouble(L"BRACKET_SIZE_D",	m_pstCfg->acam_spec.bracket_size_d,	4);
	SetConfigDouble(L"IN_RING_SIZE",	m_pstCfg->acam_spec.in_ring_size,	3);

	for (i=0x0; i<m_pstCfg->set_cams.acam_count; i++)
	{
		swprintf_s(tzKey, 64,	L"ACAM_Z_FOCUS_%d", i+1);
		SetConfigDouble(tzKey,	m_pstCfg->acam_spec.acam_z_focus[i], 4);

		swprintf_s(tzKey, 64,	L"QUARTZ_STAGE_Y%d", i+1);
		SetConfigDouble(tzKey,	m_pstCfg->acam_spec.quartz_stage_y[i], 4);

		swprintf_s(tzKey, 64,	L"QUARTZ_ACAM_%d", i+1);
		SetConfigDouble(tzKey,	m_pstCfg->acam_spec.quartz_acam[i], 4);

		swprintf_s(tzKey, 64,	L"ACAM_ANGLE_%d", i+1);
		SetConfigDouble(tzKey,	m_pstCfg->acam_spec.spec_angle[i], 8);

		swprintf_s(tzKey, 64,	L"ACAM_PIXEL_UM_%d", i+1);
		SetConfigDouble(tzKey,	m_pstCfg->acam_spec.spec_pixel_um[i], 8);
	}

	return TRUE;
}

/*
 desc : Load or Save the config file (ACAM_FOCUS)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfigACamFocus()
{
	UINT8 i	= 0x00;
	TCHAR tzKey[64]	= {NULL};

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"ACAM_FOCUS");

	m_pstCfg->acam_focus.step_move_z	= GetConfigDouble(L"STEP_MOVE_Z");

	m_pstCfg->acam_focus.mark2_stage_x	= GetConfigDouble(L"MARK2_STAGE_X");
	m_pstCfg->acam_focus.model_dia_size	= GetConfigDouble(L"MODEL_DIA_SIZE");

	for (i=0x0; i<m_pstCfg->set_cams.acam_count; i++)
	{
		swprintf_s(tzKey, 64, L"ACAM_Z_FOCUS_%d", i+1);
		m_pstCfg->acam_focus.acam_z_focus[i]	= GetConfigDouble(tzKey);

		swprintf_s(tzKey, 64, L"MARK2_STAGE_Y%d", i+1);
		m_pstCfg->acam_focus.mark2_stage_y[i]	= GetConfigDouble(tzKey);

		swprintf_s(tzKey, 64, L"MARK2_ACAM_%d", i+1);
		m_pstCfg->acam_focus.mark2_acam[i]		= GetConfigDouble(tzKey);
	}

	return TRUE;
}
BOOL CConfUvdi15::SaveConfigACamFocus()
{
	UINT8 i	= 0x00;
	TCHAR tzKey[64]	= {NULL};

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"ACAM_FOCUS");

	SetConfigDouble(L"STEP_MOVE_Z",		m_pstCfg->acam_focus.step_move_z,	3);
	SetConfigDouble(L"MARK_STAGE_X",	m_pstCfg->acam_focus.mark2_stage_x,	4);
	SetConfigDouble(L"MODEL_DIA_SIZE",	m_pstCfg->acam_focus.model_dia_size,3);

	for (i=0x0; i<m_pstCfg->set_cams.acam_count; i++)
	{
		swprintf_s(tzKey, 64, L"ACAM_Z_FOCUS_%d", i+1);
		SetConfigDouble(tzKey,	m_pstCfg->acam_focus.acam_z_focus[i],	4);

		swprintf_s(tzKey, 64, L"MARK_STAGE_Y%d", i+1);
		SetConfigDouble(tzKey,	m_pstCfg->acam_focus.mark2_stage_y[i],	4);

		swprintf_s(tzKey, 64, L"MARK_ACAM_%d", i+1);
		SetConfigDouble(tzKey,	m_pstCfg->acam_focus.mark2_acam[i],	4);
	}

	return TRUE;
}

/*
 desc : Load or Save the config file (PH_STEP)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfigPhStepXY()
{
	UINT8 i	= 0x00;
	TCHAR tzKey[64]	= {NULL};

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"PH_STEP");

	m_pstCfg->ph_step.max_ph_step		= GetConfigUint8(L"MAX_PH_STEP");
	m_pstCfg->ph_step.scroll_mode		= GetConfigUint8(L"SCROLL_MODE");
	m_pstCfg->ph_step.ph_scan			= GetConfigUint8(L"PH_SCAN");
	m_pstCfg->ph_step.expose_round		= GetConfigUint8(L"EXPOSE_ROUND");
	m_pstCfg->ph_step.model_type		= GetConfigUint32(L"MODEL_TYPE");
	m_pstCfg->ph_step.start_xy[0]		= GetConfigDouble(L"MARK_STAGE_X");
	m_pstCfg->ph_step.start_xy[1]		= GetConfigDouble(L"MARK_STAGE_Y");
	m_pstCfg->ph_step.start_acam_x		= GetConfigDouble(L"MARK_ACAM1_X");
	m_pstCfg->ph_step.stripe_width		= GetConfigDouble(L"STRIPE_WIDTH");
	m_pstCfg->ph_step.mark_period		= GetConfigDouble(L"MARK_PERIOD");
	m_pstCfg->ph_step.model_color		= GetConfigDouble(L"MODEL_COLOR");
	m_pstCfg->ph_step.model_dia_size	= GetConfigDouble(L"MODEL_DIA_SIZE");
	m_pstCfg->ph_step.center_offset		= GetConfigDouble(L"CENTER_OFFSET");
	m_pstCfg->ph_step.y_pos_plus		= GetConfigDouble(L"Y_POS_PLUS");

	return TRUE;
}
BOOL CConfUvdi15::SaveConfigPhStepXY()
{
	UINT8 i	= 0x00;
	TCHAR tzKey[64]	= {NULL};

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"PH_STEP");

	SetConfigUint32(L"MAX_PH_STEP",		m_pstCfg->ph_step.max_ph_step);
	SetConfigUint32(L"MODEL_TYPE",		m_pstCfg->ph_step.model_type);
	SetConfigUint32(L"EXPOSE_ROUND",	m_pstCfg->ph_step.expose_round);
	SetConfigDouble(L"MARK_STAGE_X",	m_pstCfg->ph_step.start_xy[0],		4);
	SetConfigDouble(L"MARK_STAGE_Y",	m_pstCfg->ph_step.start_xy[1],		4);
	SetConfigDouble(L"MARK_ACAM1_X",	m_pstCfg->ph_step.start_acam_x,		4);
	SetConfigUint32(L"SCROLL_MODE",		m_pstCfg->ph_step.scroll_mode);
	SetConfigUint32(L"PH_SCAN",			m_pstCfg->ph_step.ph_scan);
	SetConfigDouble(L"STRIPE_WIDTH",	m_pstCfg->ph_step.stripe_width,		4);
	SetConfigDouble(L"MARK_PERIOD",		m_pstCfg->ph_step.mark_period,		4);
	SetConfigDouble(L"MODEL_COLOR",		m_pstCfg->ph_step.model_color,		3);
	SetConfigDouble(L"MODEL_DIA_SIZE",	m_pstCfg->ph_step.model_dia_size,	3);
	SetConfigDouble(L"CENTER_OFFSET",	m_pstCfg->ph_step.center_offset,	4);

	return TRUE;
}


/*
 desc : Load or Save the config file (FLATNESS)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfigFlatParam()
{
	UINT8 i = 0x00;
	TCHAR tzKey[64] = { NULL };

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"FLATNESS");

	m_pstCfg->measure_flat.u8UseMotorT = GetConfigUint8(L"USE_MOTOR_T");
	m_pstCfg->measure_flat.u8UseMotorZ = GetConfigUint8(L"USE_MOTOR_Z");
		
	m_pstCfg->measure_flat.dSpeedX = GetConfigDouble(L"SPEED_X");
	m_pstCfg->measure_flat.dSpeedY = GetConfigDouble(L"SPEED_Y");
	m_pstCfg->measure_flat.dSpeedT = GetConfigDouble(L"SPEED_T");
	m_pstCfg->measure_flat.dSpeedZ = GetConfigDouble(L"SPEED_Z");
	m_pstCfg->measure_flat.dStartXPos = GetConfigDouble(L"START_X");
	m_pstCfg->measure_flat.dStartYPos = GetConfigDouble(L"START_Y");
	m_pstCfg->measure_flat.dStartTPos = GetConfigDouble(L"START_T");
	m_pstCfg->measure_flat.dStartZPos = GetConfigDouble(L"START_Z");
	m_pstCfg->measure_flat.dEndXPos = GetConfigDouble(L"END_X");
	m_pstCfg->measure_flat.dEndYPos = GetConfigDouble(L"END_Y");

	m_pstCfg->measure_flat.u8CountOfXLine = GetConfigUint8(L"LINE_X");
	m_pstCfg->measure_flat.u8CountOfYLine = GetConfigUint8(L"LINE_Y");
	m_pstCfg->measure_flat.u8CountOfMeasure = GetConfigUint8(L"MEASURE_COUNT");
	m_pstCfg->measure_flat.u16DelayTime = GetConfigUint16(L"DELAY_TIME");

	m_pstCfg->measure_flat.u8UseThickCheck = GetConfigUint8(L"USE_THICK_CHECK");
	m_pstCfg->measure_flat.dRangStartYPos = GetConfigDouble(L"RANG_START_Y");
	m_pstCfg->measure_flat.dRangEndYPos = GetConfigDouble(L"RANG_END_Y");
	m_pstCfg->measure_flat.dOffsetZPOS = GetConfigDouble(L"OFFSET_Z");
	m_pstCfg->measure_flat.dLimitZPOS = GetConfigDouble(L"LIMIT_Z");

	m_pstCfg->measure_flat.bThieckOnOff = FALSE;
	
	return TRUE;
}
BOOL CConfUvdi15::SaveConfigFlatParam()
{
	UINT8 i = 0x00;
	TCHAR tzKey[64] = { NULL };

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"FLATNESS");

	SetConfigUint32(L"USE_MOTOR_T", m_pstCfg->measure_flat.u8UseMotorT);
	SetConfigUint32(L"USE_MOTOR_Z", m_pstCfg->measure_flat.u8UseMotorZ);

	SetConfigDouble(L"SPEED_X", m_pstCfg->measure_flat.dSpeedX, 3);
	SetConfigDouble(L"SPEED_Y", m_pstCfg->measure_flat.dSpeedY, 3);
	SetConfigDouble(L"SPEED_T", m_pstCfg->measure_flat.dSpeedT, 3);
	SetConfigDouble(L"SPEED_Z", m_pstCfg->measure_flat.dSpeedZ, 3);
	SetConfigDouble(L"START_X", m_pstCfg->measure_flat.dStartXPos, 4);
	SetConfigDouble(L"START_Y", m_pstCfg->measure_flat.dStartYPos, 4);
	SetConfigDouble(L"START_T", m_pstCfg->measure_flat.dStartTPos, 4);
	SetConfigDouble(L"START_Z", m_pstCfg->measure_flat.dStartZPos, 4);
	SetConfigDouble(L"END_X", m_pstCfg->measure_flat.dEndXPos, 4);
	SetConfigDouble(L"END_Y", m_pstCfg->measure_flat.dEndYPos, 4);

	SetConfigUint32(L"LINE_X", m_pstCfg->measure_flat.u8CountOfXLine);
	SetConfigUint32(L"LINE_Y", m_pstCfg->measure_flat.u8CountOfYLine);
	SetConfigUint32(L"MEASURE_COUNT", m_pstCfg->measure_flat.u8CountOfMeasure);
	SetConfigUint32(L"DELAY_TIME", m_pstCfg->measure_flat.u16DelayTime);

	SetConfigUint32(L"USE_THICK_CHECK", m_pstCfg->measure_flat.u8UseThickCheck);
	SetConfigDouble(L"RANG_START_Y", m_pstCfg->measure_flat.dRangStartYPos, 3);
	SetConfigDouble(L"RANG_END_Y", m_pstCfg->measure_flat.dRangEndYPos, 3);
	SetConfigDouble(L"OFFSET_Z", m_pstCfg->measure_flat.dOffsetZPOS, 3);
	SetConfigDouble(L"LIMIT_Z", m_pstCfg->measure_flat.dLimitZPOS, 3);


	return TRUE;
}

/*
 desc : Load or Save the config file (UVDI15_COMN)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfigUvDI15Common()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"UVDI15_COMN");

	m_pstCfg->set_uvdi15.check_hot_air		= GetConfigUint8(L"CHECK_HOT_AIR");
	m_pstCfg->set_uvdi15.check_query_expo	= GetConfigUint8(L"CHECK_QUERY_EXPO");
	m_pstCfg->set_uvdi15.check_chiller		= GetConfigUint8(L"CHECK_CHILLER");
	m_pstCfg->set_uvdi15.check_vacuum		= GetConfigUint8(L"CHECK_VACUUM");
	m_pstCfg->set_uvdi15.check_material		= GetConfigUint8(L"CHECK_MATERIAL");
	m_pstCfg->set_uvdi15.check_start_led		= GetConfigUint8(L"CHECK_START_LED");
	m_pstCfg->set_uvdi15.use_vision_lib		= GetConfigUint8(L"USE_VISION_LIBRARY");
	m_pstCfg->set_uvdi15.load_recipe_homing	= GetConfigUint8(L"LOAD_RECIPE_HOMING");
	GetConfigStr(L"RECIPE_NAME", m_pstCfg->set_uvdi15.recipe_name, MAX_FILE_LEN);

	return TRUE;
}
BOOL CConfUvdi15::SaveConfigUvDI15Common()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"UVDI15_COMN");

	SetConfigUint32(L"CHECK_HOT_AIR",		m_pstCfg->set_uvdi15.check_hot_air);
	SetConfigUint32(L"CHECK_QUERY_EXPO",	m_pstCfg->set_uvdi15.check_query_expo);
	SetConfigUint32(L"CHECK_CHILLER",		m_pstCfg->set_uvdi15.check_chiller);
	SetConfigUint32(L"CHECK_VACUUM",		m_pstCfg->set_uvdi15.check_vacuum);
	SetConfigUint32(L"CHECK_MATERIAL",		m_pstCfg->set_uvdi15.check_material);
	SetConfigUint32(L"CHECK_START_LED",		m_pstCfg->set_uvdi15.check_start_led);
	SetConfigUint32(L"USE_VISION_LIBRARY",	m_pstCfg->set_uvdi15.use_vision_lib);
	SetConfigUint32(L"LOAD_RECIPE_HOMING",	m_pstCfg->set_uvdi15.load_recipe_homing);
	SetConfigStr(L"RECIPE_NAME",			m_pstCfg->set_uvdi15.recipe_name);
	return TRUE;
}


/*
 desc : Load or Save the config file (TEMP_RANGE)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfigTemp()
{
	UINT8 i = 0x00;
	TCHAR tzKey[64] = { NULL };

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"TEMP_RANGE");

	m_pstCfg->temp_range.hot_air[0] =	GetConfigDouble(L"HOT_AIR_DEV_TEMP");
	m_pstCfg->temp_range.hot_air[1] =	GetConfigDouble(L"HOT_AIR_DEV_RANGE");
	m_pstCfg->temp_range.di_internal[0] = GetConfigDouble(L"HOT_DI_DEV_TEMP");
	m_pstCfg->temp_range.di_internal[1] = GetConfigDouble(L"HOT_DI_DEV_RANGE");
	m_pstCfg->temp_range.optic_led[0] = GetConfigDouble(L"HOT_PH_LED_TEMP");
	m_pstCfg->temp_range.optic_led[1] = GetConfigDouble(L"HOT_PH_LED_RANGE");
	m_pstCfg->temp_range.optic_board[0] = GetConfigDouble(L"HOT_PH_BOARD_TEMP");
	m_pstCfg->temp_range.optic_board[1] = GetConfigDouble(L"HOT_PH_BOARD_RANGE");

	return TRUE;
}
BOOL CConfUvdi15::SaveConfigTemp()
{
	UINT8 i = 0x00;
	TCHAR tzKey[64] = { NULL };

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"TEMP_RANGE");

	SetConfigDouble(L"HOT_AIR_DEV_TEMP",	m_pstCfg->temp_range.hot_air[0], 1);
	SetConfigDouble(L"HOT_AIR_DEV_RANGE",	m_pstCfg->temp_range.hot_air[1], 1);
	SetConfigDouble(L"HOT_DI_DEV_TEMP",		m_pstCfg->temp_range.di_internal[0], 1);
	SetConfigDouble(L"HOT_DI_DEV_RANGE",	m_pstCfg->temp_range.di_internal[1], 1);
	SetConfigDouble(L"HOT_PH_LED_TEMP",		m_pstCfg->temp_range.optic_led[0], 1);
	SetConfigDouble(L"HOT_PH_LED_RANGE",	m_pstCfg->temp_range.optic_led[1], 1);
	SetConfigDouble(L"HOT_PH_BOARD_TEMP",	m_pstCfg->temp_range.optic_board[0], 1);
	SetConfigDouble(L"HOT_PH_BOARD_RANGE",	m_pstCfg->temp_range.optic_board[1], 1);

	return TRUE;
}

/*
 desc : Load or Save the config file (TRIG_GRAB and TRIG_STEP) 
 parm : mode	- [in] 0x00: TRIG_GRAB, 0x01: TRIG_STEP
 retn : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfigTrigger(UINT8 mode)
{
	UINT8 i	= 0;
	TCHAR tzKey[MAX_KEY_STRING]	= {NULL};
	LPG_CTSP pstCfg	= NULL;

	/* Load the count of trigger channel */
	if (0x00 == mode)
	{
		pstCfg	= &m_pstCfg->trig_grab;
		swprintf_s(m_tzSubj, MAX_SUBJ_STRING, L"TRIG_GRAB");
		pstCfg->port_no = GetConfigUint8(L"PORT_NO");
		pstCfg->ch_count	= GetConfigUint8(L"CH_COUNT");
		if (pstCfg->ch_count != 4)	return FALSE;
	}
	else
	{
		pstCfg	= &m_pstCfg->trig_step;
		swprintf_s(m_tzSubj, MAX_SUBJ_STRING, L"TRIG_STEP");
		pstCfg->ch_count	= GetConfigUint8(L"CH_COUNT");
		if (pstCfg->ch_count != 2)	return FALSE;
	}

	for (i=0; i<pstCfg->ch_count; i++)
	{
		swprintf_s(tzKey, MAX_KEY_STRING, L"TRIG_ON_TIME_%d",	(i % 2)+1);
		pstCfg->trig_on_time[i]		= GetConfigUint32(tzKey);

		swprintf_s(tzKey, MAX_KEY_STRING, L"STROBE_ON_TIME_%d",	(i % 2)+1);
		pstCfg->strob_on_time[i]	= GetConfigUint32(tzKey);

		swprintf_s(tzKey, MAX_KEY_STRING, L"TRIG_DELAY_TIME_%d",(i % 2)+1);
		pstCfg->trig_delay_time[i]	= GetConfigUint32(tzKey);
	}
	pstCfg->trig_forward	= GetConfigUint32(L"TRIG_FORWARD");
	pstCfg->trig_backward	= GetConfigUint32(L"TRIG_BACKWARD");

	return TRUE;
}
BOOL CConfUvdi15::SaveConfigTrigger(UINT8 mode)
{
	UINT8 i	= 0;
	TCHAR tzKey[MAX_KEY_STRING]	= {NULL};
	LPG_CTSP pstCfg	= NULL;

	if (0x00 == mode)
	{
		pstCfg	= &m_pstCfg->trig_grab;
		swprintf_s(m_tzSubj, MAX_SUBJ_STRING, L"TRIG_GRAB");
	}
	else
	{
		pstCfg	= &m_pstCfg->trig_step;
		swprintf_s(m_tzSubj, MAX_SUBJ_STRING, L"TRIG_STEP");
	}

	/* Save the count of trigger channel */
	SetConfigUint32(L"CH_COUNT",	pstCfg->ch_count);

	for (i=0; i<pstCfg->ch_count; i++)
	{
		swprintf_s(tzKey, MAX_KEY_STRING, L"TRIG_ON_TIME_%d",	i+1);
		SetConfigUint32(tzKey,	pstCfg->trig_on_time[i]);
		swprintf_s(tzKey, MAX_KEY_STRING, L"STROBE_ON_TIME_%d",	i+1);
		SetConfigUint32(tzKey,	pstCfg->strob_on_time[i]);
		swprintf_s(tzKey, MAX_KEY_STRING, L"TRIG_DELAY_TIME_%d",i+1);
		SetConfigUint32(tzKey,	pstCfg->trig_delay_time[i]);
	}
	SetConfigInt32(L"TRIG_FORWARD",		pstCfg->trig_forward);
	SetConfigInt32(L"TRIG_BACKWARD",	pstCfg->trig_backward);

	return TRUE;
}

/*
 desc : Load or Save the config file (CAMERA_BASLER)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfigCameraBasler()
{
	UINT8 i	= 0x00;
	TCHAR tzKey[64]	= {NULL};

	/* Subject Name 설정 */
	swprintf_s(m_tzSubj, MAX_SUBJ_STRING, L"CAMERA_BASLER");

	/* Mirror Left/Right & Top / Bottom (카메라 영상. 상/하 그리고 좌/우 반전 여부) */
	m_pstCfg->set_basler.cam_reverse_x = GetConfigUint8(L"CAM_REVERSE_X");
	m_pstCfg->set_basler.cam_reverse_y = GetConfigUint8(L"CAM_REVERSE_Y");
	m_pstCfg->set_basler.z_axis_move_min = GetConfigDouble(L"Z_AXIS_MOVE_MIN");
	m_pstCfg->set_basler.z_axis_move_max = GetConfigDouble(L"Z_AXIS_MOVE_MAX");

	for (; i<m_pstCfg->set_cams.acam_count; i++)
	{
		swprintf_s(tzKey, 64, L"CAM_%d_GAIN_LEVEL", i+1);
		m_pstCfg->set_basler.cam_gain_level[i]	= GetConfigUint8(tzKey);
		swprintf_s(tzKey, 64, L"CAM_MC2_DRV_%d", i+1);
		m_pstCfg->set_basler.cam_mc2_drv[i]		= GetConfigUint8(tzKey);

		swprintf_s(tzKey, 64,	L"CAM_IPv4_%d", i+1);
		GetConfigIPv4(tzKey,	m_pstCfg->set_basler.cam_ip[i]);

		swprintf_s(tzKey, 64, L"FRAME_%d", i+1);
		m_pstCfg->set_basler.acq_frames[i]		= GetConfigUint16(tzKey);

		swprintf_s(tzKey, 64, L"TIME_GRAB_%d", i+1);
		m_pstCfg->set_basler.grab_times[i]		= GetConfigUint32(tzKey);

		swprintf_s(tzKey, 64, L"TIME_STEP_%d", i+1);
		m_pstCfg->set_basler.step_times[i]		= GetConfigUint32(tzKey);
	}
	
	return TRUE;
}
BOOL CConfUvdi15::SaveConfigCameraBasler()
{
	UINT8 i	= 0x00;
	TCHAR tzKey[64]	= {NULL};

	/* Subject Name 설정 */
	swprintf_s(m_tzSubj, MAX_SUBJ_STRING, L"CAMERA_BASLER");

	/* Mirror Left/Right & Top / Bottom (카메라 영상. 상/하 그리고 좌/우 반전 여부) */
	SetConfigUint32(L"CAM_REVERSE_X", m_pstCfg->set_basler.cam_reverse_x);
	SetConfigUint32(L"CAM_REVERSE_Y", m_pstCfg->set_basler.cam_reverse_y);
	SetConfigDouble(L"Z_AXIS_MOVE_MIN", m_pstCfg->set_basler.z_axis_move_min, 4);
	SetConfigDouble(L"Z_AXIS_MOVE_MAX", m_pstCfg->set_basler.z_axis_move_max, 4);


	for (; i<m_pstCfg->set_cams.acam_count; i++)
	{
		swprintf_s(tzKey, 64, L"CAM_%d_GAIN_LEVEL", i+1);
		SetConfigUint32(tzKey,	m_pstCfg->set_basler.cam_gain_level[i]);

		swprintf_s(tzKey, 64,	L"CAM_MC2_DRV_%d", i+1);
		SetConfigUint32(tzKey,	m_pstCfg->set_basler.cam_mc2_drv[i]);

		swprintf_s(tzKey, 64,	L"CAM_IPv4_%d", i+1);
		SetConfigIPv4(tzKey,	m_pstCfg->set_basler.cam_ip[i]);

		swprintf_s(tzKey, 64,	L"FRAME_%d", i+1);
		SetConfigUint32(tzKey,	m_pstCfg->set_basler.acq_frames[i]);

		swprintf_s(tzKey, 64,	L"TIME_GRAB_%d", i+1);
		SetConfigUint32(tzKey,	m_pstCfg->set_basler.grab_times[i]);

		swprintf_s(tzKey, 64,	L"TIME_STEP_%d", i+1);
		SetConfigUint32(tzKey,	m_pstCfg->set_basler.step_times[i]);
	}
	
	return TRUE;
}

/*
 설명 : 환경 파일 적재 (GLOBAL_TRANS)
 변수 : None
 반환 : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfigGlobalTrans()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"GLOBAL_TRANS");

	m_pstCfg->global_trans.use_trans		= GetConfigUint8(L"USE_TRANS");
	m_pstCfg->global_trans.use_rectangle	= GetConfigUint8(L"USE_RECTANGLE");
	m_pstCfg->global_trans.use_rotation_mode= GetConfigUint8(L"USE_ROTATION");
	m_pstCfg->global_trans.use_scaling_mode	= GetConfigUint8(L"USE_SCALING");
	m_pstCfg->global_trans.use_offset_mode	= GetConfigUint8(L"USE_OFFSET");

	m_pstCfg->global_trans.rotation		= (INT32)ROUNDED(GetConfigDouble(L"VAL_ROTATION") * 1000.0f, 0);
	m_pstCfg->global_trans.scaling[0]	= (UINT32)ROUNDED(GetConfigDouble(L"VAL_SCALING_X") * 1000000.0f, 0);
	m_pstCfg->global_trans.scaling[1]	= (UINT32)ROUNDED(GetConfigDouble(L"VAL_SCALING_Y") * 1000000.0f, 0);
	m_pstCfg->global_trans.offset[0]	= (INT32)ROUNDED(GetConfigDouble(L"VAL_OFFSET_X") * 1000000.0f, 0);
	m_pstCfg->global_trans.offset[1]	= (INT32)ROUNDED(GetConfigDouble(L"VAL_OFFSET_Y") * 1000000.0f, 0);
	
	return TRUE;
}

/*
 설명 : 환경 파일 저장 (GLOBAL_TRANS)
 변수 : None
 반환 : TRUE or FALSE
*/
BOOL CConfUvdi15::SaveConfigGlobalTrans()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"GLOBAL_TRANS");

	SetConfigUint32(L"USE_RECTANGLE",	m_pstCfg->global_trans.use_rectangle);
	SetConfigUint32(L"USE_ROTATION",	m_pstCfg->global_trans.use_rotation_mode);
	SetConfigUint32(L"USE_SCALING",		m_pstCfg->global_trans.use_scaling_mode);
	SetConfigUint32(L"USE_OFFSET",		m_pstCfg->global_trans.use_offset_mode);

	SetConfigDouble(L"VAL_ROTATION",	m_pstCfg->global_trans.rotation/1000.0f, 3);
	SetConfigDouble(L"VAL_SCALING_X",	m_pstCfg->global_trans.scaling[0]/1000000.0f, 6);
	SetConfigDouble(L"VAL_SCALING_Y",	m_pstCfg->global_trans.scaling[1]/1000000.0f, 6);
	SetConfigDouble(L"VAL_OFFSET_X",	m_pstCfg->global_trans.offset[0]/1000000.0f, 6);
	SetConfigDouble(L"VAL_OFFSET_Y",	m_pstCfg->global_trans.offset[1]/1000000.0f, 6);
	
	return TRUE;
}



/*
 설명 : 환경 파일 적재 (STROBE_LAMP)
 변수 : None
 반환 : TRUE or FALSE
*/
BOOL CConfUvdi15::LoadConfigStrobeLamp()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"STROBE_LAMP");

	UINT8 i = 0x00;
	TCHAR tzKey[64] = { NULL };
	for (; i < 8; i++)
	{
		swprintf_s(tzKey, 64, L"STROBE_LAMP[%d]", i);
		m_pstCfg->set_strobe_lamp.u16StrobeValue[i] = GetConfigUint16(tzKey);
	}


	return TRUE;
}

/*
 설명 : 환경 파일 저장 (STROBE_LAMP)
 변수 : None
 반환 : TRUE or FALSE
*/
BOOL CConfUvdi15::SaveConfigStrobeLamp()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"STROBE_LAMP");

	UINT8 i = 0x00;
	TCHAR tzKey[64] = { NULL };
	for (; i < 8; i++)
	{
		swprintf_s(tzKey, 64, L"STROBE_LAMP[%d]", i);
		SetConfigUint32(tzKey, m_pstCfg->set_strobe_lamp.u16StrobeValue[i]);
	}

	return TRUE;
}



