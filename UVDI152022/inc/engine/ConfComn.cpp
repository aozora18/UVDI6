
/*
 desc : Engine Configuration for Common
*/

#include "pch.h"
#include "ConfComn.h"

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
CConfComn::CConfComn(LPG_CIEA config)
#if (CUSTOM_CODE_UVDI15_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_UVDI15_LLS06 == DELIVERY_PRODUCT_ID)
	: CConfBase(L"uvdi15")
#elif (CUSTOM_CODE_GEN2I == DELIVERY_PRODUCT_ID)
	: CConfBase(L"gen2i")
#elif (CUSTOM_CODE_UVDI15 == DELIVERY_PRODUCT_ID)
	: CConfBase(L"uvdi15")
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	: CConfBase(L"hddi6")
#endif
{
	m_pstCfg	= config;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CConfComn::~CConfComn()
{
}

/*
 desc : 환경 파일 적재
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfComn::LoadConfig()
{
	/* 아래 함수 호출 순서는 반드시 지켜야 됨 */
	if (!LoadConfigCommon())		return FALSE;	/* [COMMON]			*/
	if (!LoadConfigMonitor())		return FALSE;	/* [MONITOR]		*/
	if (!LoadConfigCommunication())	return FALSE;	/* [COMMUNICATION]	*/
	if (!LoadConfigLuriaSvc())		return FALSE;	/* [OPTIC_SERVICE]	*/
	if (!LoadConfigMC2Svc())		return FALSE;	/* [MC2_SERVICE]	*/
	if (!LoadConfigMelsecQSvc())	return FALSE;	/* [PLC_SERVICE]	*/

	return TRUE;
}

/*
 desc : 환경 파일 저장
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfComn::SaveConfig()
{
	if (!SaveConfigCommon())		return FALSE;
	if (!SaveConfigMonitor())		return FALSE;
	if (!SaveConfigLuriaSvc())		return FALSE;
	if (!SaveConfigMC2Svc())		return FALSE;
	if (!SaveConfigMelsecQSvc())	return FALSE;

	return TRUE;
}

/*
 desc : 모니터링 시작 시간 및 에러 그리고 기타 정보 갱신
 parm : None
 retn : None
*/
VOID CConfComn::UpdateMonitorData()
{
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	return;
#endif // 
	SYSTEMTIME stTm	= {NULL};

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"MONITOR");

	/* 현재 로컬 컴퓨터의 시간 갱신 */
	GetLocalTime(&stTm);
	swprintf_s(GetConfig()->monitor.mon_time, 24, L"%04d-%02d-%02d %02d:%02d:%02d",
				stTm.wYear, stTm.wMonth, stTm.wDay, stTm.wHour, stTm.wMinute, stTm.wSecond);
	/* 에러/경고 개수 초기화 */
	m_pstCfg->monitor.error_count	= 0;
	m_pstCfg->monitor.warn_count	= 0;

	/* 모니터링 시작 시간 갱신 */
	SetConfigStr(L"START_TIME",		m_pstCfg->monitor.mon_time);
	SetConfigUint32(L"ERROR_COUNT",	m_pstCfg->monitor.error_count);
	SetConfigUint32(L"WARN_COUNT",	m_pstCfg->monitor.warn_count);
}

/*
 desc : 환경 파일 적재
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfComn::LoadConfigCommon()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"COMMON");

	m_pstCfg->set_comn.run_develop_mode		= GetConfigUint8(L"RUN_DEVELOPE_MODE");
	m_pstCfg->set_comn.run_emulate_mode		= GetConfigUint8(L"RUN_EMULATE_MODE");
	m_pstCfg->set_comn.run_engine_monitor	= GetConfigUint8(L"RUN_ENGINE_MONITOR");
	m_pstCfg->set_comn.run_material_detect	= GetConfigUint8(L"RUN_MATERIAL_DETECT");
	m_pstCfg->set_comn.grab_mark_saved		= GetConfigUint8(L"GRAB_MARK_SAVED");
	m_pstCfg->set_comn.log_file_saved		= GetConfigUint8(L"LOG_FILE_SAVED");
	m_pstCfg->set_comn.log_file_type		= GetConfigUint8(L"LOG_FILE_TYPE");
	m_pstCfg->set_comn.align_camera_kind	= GetConfigUint8(L"ALIGN_CAMERA_KIND");
	m_pstCfg->set_comn.strobe_lamp_type		= GetConfigUint8(L"STROBE_LAMP_TYPE");
	m_pstCfg->set_comn.all_alarm_ignore		= GetConfigUint8(L"ALL_ALARM_IGNORE");
	m_pstCfg->set_comn.all_warn_ignore		= GetConfigUint8(L"ALL_WARN_IGNORE");
	m_pstCfg->set_comn.use_auto_align		= GetConfigUint8(L"USE_AUTO_ALIGN");

	m_pstCfg->set_comn.comm_log_optic		= GetConfigUint8(L"COMM_LOG_OPTIC");
	m_pstCfg->set_comn.comm_log_mc2			= GetConfigUint8(L"COMM_LOG_MC2");
	m_pstCfg->set_comn.comm_log_plc			= GetConfigUint8(L"COMM_LOG_PLC");
	m_pstCfg->set_comn.comm_log_efu			= GetConfigUint8(L"COMM_LOG_EFU");
#if (CUSTOM_CODE_UVDI15_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_UVDI15_LLS06 == DELIVERY_PRODUCT_ID)
#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
	m_pstCfg->set_comn.comm_log_milara		= GetConfigUint8(L"COMM_LOG_MILARA");
	m_pstCfg->set_comn.comm_log_pm100d		= GetConfigUint8(L"COMM_LOG_PM100D");
	m_pstCfg->set_comn.comm_log_litho		= GetConfigUint8(L"COMM_LOG_LITHO");
#endif
	m_pstCfg->set_comn.mesg_box_optic		= GetConfigUint8(L"MESG_BOX_OPTIC");

	GetConfigStr(L"GERBER_PATH",		m_pstCfg->set_comn.gerber_path, MAX_PATH_LEN, L"");
	GetConfigStr(L"PREPROC_DLL_PATH",	m_pstCfg->set_comn.preproc_dll_path, MAX_PATH_LEN, L"");

	return TRUE;
}

/*
 desc : 환경 파일 적재
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfComn::SaveConfigCommon()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"COMMON");

	SetConfigUint32(L"RUN_ENGINE_MONITOR",	m_pstCfg->set_comn.run_engine_monitor);
	SetConfigUint32(L"RUN_MATERIAL_DETECT",	m_pstCfg->set_comn.run_material_detect);
	SetConfigUint32(L"GRAB_MARK_SAVED",		m_pstCfg->set_comn.grab_mark_saved);
	SetConfigUint32(L"LOG_FILE_SAVED",		m_pstCfg->set_comn.log_file_saved);
	SetConfigUint32(L"LOG_FILE_TYPE",		m_pstCfg->set_comn.log_file_type);
	SetConfigUint32(L"ALIGN_CAMERA_KIND",	m_pstCfg->set_comn.align_camera_kind);
	SetConfigUint32(L"STROBE_LAMP_TYPE",	m_pstCfg->set_comn.strobe_lamp_type);
	SetConfigUint32(L"ALL_ALARM_IGNORE",	m_pstCfg->set_comn.all_alarm_ignore);
	SetConfigUint32(L"ALL_WARN_IGNORE",		m_pstCfg->set_comn.all_warn_ignore);
	SetConfigUint32(L"USE_AUTO_ALIGN",		m_pstCfg->set_comn.use_auto_align);
	SetConfigStr(L"GERBER_PATH",			m_pstCfg->set_comn.gerber_path);

	SetConfigUint32(L"COMM_LOG_OPTIC",		m_pstCfg->set_comn.comm_log_optic);
	SetConfigUint32(L"COMM_LOG_MC2",		m_pstCfg->set_comn.comm_log_mc2);
	SetConfigUint32(L"COMM_LOG_PLC",		m_pstCfg->set_comn.comm_log_plc);
	SetConfigUint32(L"COMM_LOG_EFU",		m_pstCfg->set_comn.comm_log_efu);
#if (CUSTOM_CODE_UVDI15_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_UVDI15_LLS06 == DELIVERY_PRODUCT_ID)
#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
	SetConfigUint32(L"COMM_LOG_MILARA",		m_pstCfg->set_comn.comm_log_milara);
	SetConfigUint32(L"COMM_LOG_PM100D",		m_pstCfg->set_comn.comm_log_pm100d);
	SetConfigUint32(L"COMM_LOG_LITHO",		m_pstCfg->set_comn.comm_log_litho);
#endif
	SetConfigUint32(L"MESG_BOX_OPTIC",		m_pstCfg->set_comn.mesg_box_optic);

	return TRUE;
}

/*
 desc : 환경 파일 적재 (MONITOR)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfComn::LoadConfigMonitor()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"MONITOR");

	m_pstCfg->monitor.hdd_drive		= GetConfigUint8(L"HDD_DRIVE");

	m_pstCfg->monitor.log_delect_day = GetConfigUint8(L"LOG_DELECT_DAY");
	m_pstCfg->monitor.bmp_delect_day = GetConfigUint8(L"BMP_DELECT_DAY");
	/*삭제 기간이 설정 되어 있지 않으면 7일 설정 */
	if (m_pstCfg->monitor.log_delect_day == 0)	m_pstCfg->monitor.log_delect_day = 7;
	if (m_pstCfg->monitor.bmp_delect_day == 0)	m_pstCfg->monitor.bmp_delect_day = 7;

	m_pstCfg->monitor.error_count	= GetConfigUint16(L"ERROR_COUNT");
	m_pstCfg->monitor.warn_count	= GetConfigUint16(L"WARN_COUNT");

	GetConfigStr(L"START_TIME", m_pstCfg->monitor.mon_time, 32);
	/* 0000-00-00 00:00:00 형식이 아닌 경우 즉, 길이가 맞지 않은 경우 */
	if (_tcslen(m_pstCfg->monitor.mon_time) != 19)
	{
		SYSTEMTIME stTm	= {NULL};
		GetLocalTime(&stTm);
		swprintf_s(m_pstCfg->monitor.mon_time, 32, L"%04d-%02d-%02d %02d:%02d:%02d",
				   stTm.wYear, stTm.wMonth, stTm.wDay, stTm.wHour, stTm.wMinute, stTm.wSecond);
	}

	return TRUE;
}

/*
 desc : 환경 파일 저장 (MONITOR)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfComn::SaveConfigMonitor()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"MONITOR");

	SetConfigUint32(L"HDD_DRIVE",	m_pstCfg->monitor.hdd_drive);
	SetConfigUint32(L"LOG_DELECT_DAY", m_pstCfg->monitor.log_delect_day);
	SetConfigUint32(L"BMP_DELECT_DAY", m_pstCfg->monitor.bmp_delect_day);
	SetConfigUint32(L"ERROR_COUNT",	m_pstCfg->monitor.error_count);
	SetConfigUint32(L"WARN_COUNT",	m_pstCfg->monitor.warn_count);
	SetConfigStr(L"START_TIME",		m_pstCfg->monitor.mon_time);

	return TRUE;
}

/*
 desc : 환경 파일 적재
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfComn::LoadConfigCommunication()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"COMMUNICATION");

	GetConfigIPv4(L"CMPS_IPv4",	m_pstCfg->set_comm.cmps_ipv4);
	GetConfigIPv4(L"EFEM_IPv4",	m_pstCfg->set_comm.efem_ipv4);
	GetConfigIPv4(L"MC2_IPv4",	m_pstCfg->set_comm.mc2_ipv4);
	GetConfigIPv4(L"MC2B_IPv4", m_pstCfg->set_comm.mc2b_ipv4);
	GetConfigIPv4(L"PLC_IPv4",	m_pstCfg->set_comm.plc_ipv4);
	GetConfigIPv4(L"SVC_IPv4",	m_pstCfg->set_comm.luria_ipv4);
	GetConfigIPv4(L"TRIG_IPv4",	m_pstCfg->set_comm.trig_ipv4);
	GetConfigIPv4(L"VISI_IPv4",	m_pstCfg->set_comm.visi_ipv4);
	GetConfigIPv4(L"PHILHMI_IPv4", m_pstCfg->set_comm.philhmi_ipv4);	// by sysandj
	GetConfigIPv4(L"STROBELAMP_IPv4", m_pstCfg->set_comm.strb_lamp_ipv4);	// by sysandj
	GetConfigIPv4(L"GENTEC_IPv4", m_pstCfg->set_comm.gentec_ipv4);
	GetConfigIPv4(L"KEYENCELDS_IPv4", m_pstCfg->set_comm.keyence_lds_ipv4);	// 230925 mhbaek Add

	m_pstCfg->set_comm.efem_port	= GetConfigUint16(L"EFEM_PORT");
	m_pstCfg->set_comm.luria_port	= GetConfigUint16(L"SVC_PORT");
	m_pstCfg->set_comm.mc2c_port	= GetConfigUint16(L"MC2C_PORT");
	m_pstCfg->set_comm.mc2s_port	= GetConfigUint16(L"MC2S_PORT");
	m_pstCfg->set_comm.mc2bc_port	= GetConfigUint16(L"MC2BC_PORT");
	m_pstCfg->set_comm.mc2bs_port	= GetConfigUint16(L"MC2BS_PORT");
	m_pstCfg->set_comm.plc_port		= GetConfigUint16(L"PLC_PORT");
	m_pstCfg->set_comm.trig_port	= GetConfigUint16(L"TRIG_PORT");
	m_pstCfg->set_comm.visi_port	= GetConfigUint16(L"VISI_PORT");
	m_pstCfg->set_comm.philhmi_port	= GetConfigUint16(L"PHILHMI_PORT");	// by sysandj
	m_pstCfg->set_comm.strb_lamp_port = GetConfigUint16(L"STROBELAMP_PORT");	// by sysandj
	m_pstCfg->set_comm.gentec_port = GetConfigUint16(L"GENTEC_PORT");
	m_pstCfg->set_comm.keyence_lds_port = GetConfigUint16(L"KEYENCELDS_PORT");	// 230925 mhbaek Add

	m_pstCfg->set_comm.link_time	= GetConfigUint32(L"LINK_TIME");
	m_pstCfg->set_comm.idle_time	= GetConfigUint32(L"IDLE_TIME");
	m_pstCfg->set_comm.sock_time	= GetConfigUint32(L"SOCK_TIME");
	m_pstCfg->set_comm.live_time	= GetConfigUint32(L"LIVE_TIME");

	m_pstCfg->set_comm.port_buff	= GetConfigUint32(L"PORT_BUFF");
	m_pstCfg->set_comm.recv_buff	= GetConfigUint32(L"RECV_BUFF");
	m_pstCfg->set_comm.ring_buff	= GetConfigUint32(L"RING_BUFF");

	/* 링 버퍼는 항상 수신 버퍼보다 2배 이상 커야 됨 */
	if (m_pstCfg->set_comm.ring_buff < m_pstCfg->set_comm.recv_buff * 2)
		m_pstCfg->set_comm.ring_buff= m_pstCfg->set_comm.recv_buff * 2;

	return TRUE;
}

/*
 desc : 환경 파일 적재 (OPTIC_SERVICE)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfComn::LoadConfigLuriaSvc()
{
	UINT8 i	= 0;
	TCHAR tzKey[MAX_KEY_STRING]	= {NULL};

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"PH_SERVICE");

#if (USE_SIMULATION_DIR)
	GetConfigStr(L"PRINT_SIMULATION_OUT_DIR",		m_pstCfg->luria_svc.print_simulation_out_dir, MAX_PATH_LEN);
#endif
	GetConfigIPv4(L"MOTION_CONTROL_IPv4",			m_pstCfg->luria_svc.motion_control_ip);

	m_pstCfg->luria_svc.table_count					= GetConfigUint8(L"TABLE_COUNT");
	m_pstCfg->luria_svc.ph_count					= GetConfigUint8(L"TOTAL_PH");
	m_pstCfg->luria_svc.led_count					= GetConfigUint8(L"LED_COUNT");
	m_pstCfg->luria_svc.ph_pitch					= GetConfigUint8(L"PH_PITCH");
	m_pstCfg->luria_svc.ph_rotate					= GetConfigUint8(L"PH_ROTATE");
	m_pstCfg->luria_svc.use_announcement			= GetConfigUint8(L"USE_ANNOUNCEMENT");
	m_pstCfg->luria_svc.use_hw_inited				= GetConfigUint8(L"USE_HW_INITED");
	
	m_pstCfg->luria_svc.motion_control_type			= GetConfigUint8(L"MOTION_CONTROL_TYPE");
	m_pstCfg->luria_svc.hys_type_1_scroll_mode		= GetConfigUint8(L"HYS_TYPE_1_SCROLL_MODE");
	m_pstCfg->luria_svc.hys_type_1_negative_offset	= GetConfigUint8(L"HYS_TYPE_1_NEGATIVE_OFFSET");
	m_pstCfg->luria_svc.table_1_print_direction		= GetConfigUint8(L"TABLE_1_PRINT_DIRECTION");
	m_pstCfg->luria_svc.table_2_print_direction		= GetConfigUint8(L"TABLE_2_PRINT_DIRECTION");
	m_pstCfg->luria_svc.active_table_no				= GetConfigUint8(L"ACTIVE_TABLE");
	if (m_pstCfg->set_comn.run_emulate_mode)
	{
		m_pstCfg->luria_svc.emulate_motion			= 0x01;
		m_pstCfg->luria_svc.emulate_ph				= 0x01;
		m_pstCfg->luria_svc.emulate_trigger			= 0x01;
	}
	else
	{
		m_pstCfg->luria_svc.emulate_motion			= GetConfigUint8(L"EMULATE_MOTION");
		m_pstCfg->luria_svc.emulate_ph				= GetConfigUint8(L"EMULATE_PH");
		m_pstCfg->luria_svc.emulate_trigger			= GetConfigUint8(L"EMULATE_TRIGGER");
	}
	m_pstCfg->luria_svc.debug_print_level			= GetConfigUint8(L"DEBUG_PRINT_LEVEL");
	m_pstCfg->luria_svc.x_drive_id_1				= GetConfigUint8(L"X_DRIVE_ID_1");
	m_pstCfg->luria_svc.y_drive_id_1				= GetConfigUint8(L"Y_DRIVE_ID_1");
	m_pstCfg->luria_svc.x_drive_id_2				= GetConfigUint8(L"X_DRIVE_ID_2");
	m_pstCfg->luria_svc.y_drive_id_2				= GetConfigUint8(L"Y_DRIVE_ID_2");
	m_pstCfg->luria_svc.artwork_complexity			= GetConfigUint8(L"ARTWORK_COMPLEXITY");
	m_pstCfg->luria_svc.z_drive_type				= GetConfigUint8(L"Z_DRIVE_TYPE");
	m_pstCfg->luria_svc.over_pressure_mode			= GetConfigUint8(L"OVER_PRESSURE_MODE");
	m_pstCfg->luria_svc.illum_meas_type				= GetConfigUint8(L"ILLUM_MEAS_TYPE");
	m_pstCfg->luria_svc.scroll_rate					= GetConfigUint16(L"SCROLL_RATE");
	m_pstCfg->luria_svc.mirror_count_xy[0]			= GetConfigUint16(L"DMD_MIRROR_COUNT_X");
	m_pstCfg->luria_svc.mirror_count_xy[1]			= GetConfigUint16(L"DMD_MIRROR_COUNT_Y");
	
	m_pstCfg->luria_svc.luria_ver_major				= GetConfigUint16(L"OPTIC_VER_MAJOR");
	m_pstCfg->luria_svc.luria_ver_minor				= GetConfigUint16(L"OPTIC_VER_MINOR");
	m_pstCfg->luria_svc.luria_ver_build				= GetConfigUint16(L"OPTIC_VER_BUILD");
	m_pstCfg->luria_svc.product_id					= GetConfigUint32(L"PRODUCT_ID");
	m_pstCfg->luria_svc.sensor_diameter				= GetConfigDouble(L"SENSOR_DIAMETER");
	m_pstCfg->luria_svc.correction_factor			= GetConfigDouble(L"CORRECTION_FACTOR");
	m_pstCfg->luria_svc.max_y_motion_speed			= GetConfigDouble(L"MAX_Y_MOTION_SPEED");
	m_pstCfg->luria_svc.x_motion_speed				= GetConfigDouble(L"X_MOTION_SPEED");
	m_pstCfg->luria_svc.y_acceleration_distance		= GetConfigDouble(L"Y_ACCELERATION_DISTANCE");
	m_pstCfg->luria_svc.hys_type_1_delay_offset[0]	= GetConfigDouble(L"HYS_TYPE_1_DELAY_POSITIVE");
	m_pstCfg->luria_svc.hys_type_1_delay_offset[1]	= GetConfigDouble(L"HYS_TYPE_1_DELAY_NEGATIVE");
	m_pstCfg->luria_svc.ph_z_move_max				= GetConfigDouble(L"PH_Z_AXIS_MOVE_MAX");
	m_pstCfg->luria_svc.ph_z_move_min				= GetConfigDouble(L"PH_Z_AXIS_MOVE_MIN");
	m_pstCfg->luria_svc.pixel_size					= GetConfigDouble(L"PIXEL_SIZE");

	for (i = 0; i < MAX_SELECT_VELO; i++)
	{
		swprintf_s(tzKey, MAX_KEY_STRING, L"PH_Z_SELECT_VELO_%d", i);
		m_pstCfg->luria_svc.ph_z_select_velo[i] = GetConfigDouble(tzKey);
	}

	

	GetConfigIPv4(L"Z_DRIVE_IPv4", m_pstCfg->luria_svc.z_drive_ip);
	

	for (i=0; i<m_pstCfg->luria_svc.table_count; i++)
	{
		swprintf_s(tzKey, MAX_KEY_STRING, L"X_CORRECTION_TABLE_%u", i+1);
		switch (i)
		{
		case 0x00 : m_pstCfg->luria_svc.x_correction_table_1 = GetConfigUint8(tzKey);	break;
		case 0x01 : m_pstCfg->luria_svc.x_correction_table_2 = GetConfigUint8(tzKey);	break;
		}
		swprintf_s(tzKey, MAX_KEY_STRING, L"Y_CORRECTION_TABLE_%u", i+1);
		switch (i)
		{
		case 0x00 : m_pstCfg->luria_svc.y_correction_table_1 = GetConfigUint8(tzKey);	break;
		case 0x01 : m_pstCfg->luria_svc.y_correction_table_2 = GetConfigUint8(tzKey);	break;
		}
	}

	for (i=1; i<=m_pstCfg->luria_svc.ph_count; i++)
	{
		swprintf_s(tzKey, MAX_KEY_STRING, L"PH_IPv4_%02d", i);
		GetConfigIPv4(tzKey, m_pstCfg->luria_svc.ph_ipv4[i-1]);

		swprintf_s(tzKey, MAX_KEY_STRING, L"PH_OFFSET_X_%d", i+1);
		m_pstCfg->luria_svc.ph_offset_x[i]		= GetConfigDouble(tzKey);
		swprintf_s(tzKey, MAX_KEY_STRING, L"PH_OFFSET_Y_%d", i+1);
		m_pstCfg->luria_svc.ph_offset_y[i]		= GetConfigDouble(tzKey);

		swprintf_s(tzKey, 64, L"PH_Z_AXIS_FOCUS_%d", i);
		m_pstCfg->luria_svc.ph_z_focus[i-1]		= GetConfigDouble(tzKey);

		swprintf_s(tzKey, 64, L"Z_DRIVE_SD2S_PH_%d", i);
		m_pstCfg->luria_svc.z_drive_sd2s_ph[i-1]= GetConfigUint8(tzKey);
	}
	for (i=1; i<=m_pstCfg->luria_svc.table_count; i++)
	{
		swprintf_s(tzKey, MAX_KEY_STRING, L"PARALLELOGRAM_ADJUST_%d", i);
		m_pstCfg->luria_svc.paralleogram_motion_adjust[i-1]	= GetConfigUint32(tzKey);
	}
	for (i=1; i<=m_pstCfg->luria_svc.table_count; i++)
	{
		swprintf_s(tzKey, MAX_KEY_STRING, L"TABLE_LIMIT_MIN_X_%d", i);
		m_pstCfg->luria_svc.table_limit_min_xy[i-1][0]	= GetConfigDouble(tzKey);
		swprintf_s(tzKey, MAX_KEY_STRING, L"TABLE_LIMIT_MIN_Y_%d", i);
		m_pstCfg->luria_svc.table_limit_min_xy[i-1][1]	= GetConfigDouble(tzKey);
		swprintf_s(tzKey, MAX_KEY_STRING, L"TABLE_LIMIT_MAX_X_%d", i);
		m_pstCfg->luria_svc.table_limit_max_xy[i-1][0]	= GetConfigDouble(tzKey);
		swprintf_s(tzKey, MAX_KEY_STRING, L"TABLE_LIMIT_MAx_Y_%d", i);
		m_pstCfg->luria_svc.table_limit_max_xy[i-1][1]	= GetConfigDouble(tzKey);

		swprintf_s(tzKey, MAX_KEY_STRING, L"TABLE_EXPO_START_X_%d", i);
		m_pstCfg->luria_svc.table_expo_start_xy[i-1][0]	= GetConfigDouble(tzKey);
		swprintf_s(tzKey, MAX_KEY_STRING, L"TABLE_EXPO_START_Y_%d", i);
		m_pstCfg->luria_svc.table_expo_start_xy[i-1][1]	= GetConfigDouble(tzKey);
	}
	for (i=0; i<m_pstCfg->luria_svc.led_count; i++)
	{
		swprintf_s(tzKey, 64, L"ILLUM_FILT_RATE_LED_%u", i);
		m_pstCfg->luria_svc.illum_filter_rate[i]	= GetConfigDouble(tzKey);
	}
	m_pstCfg->luria_svc.illum_garbage_pwr	= GetConfigDouble(L"ILLUM_GARBAGE_PWR");


	//AF관련
	m_pstCfg->luria_svc.useAF = GetConfigUint8(L"USE_AF");
	m_pstCfg->luria_svc.useAAQ = GetConfigUint8(L"USE_AAQ");
	m_pstCfg->luria_svc.afGain = GetConfigUint16(L"AF_GAIN");
	m_pstCfg->luria_svc.DOFofPh = GetConfigUint8(L"PH_DOF");
	
	m_pstCfg->luria_svc.afWorkRangeWithinMicrometer[0] = GetConfigDouble(L"AF_RANGE_MIN_ALL");
	m_pstCfg->luria_svc.afWorkRangeWithinMicrometer[1] = GetConfigDouble(L"AF_RANGE_MAX_ALL");

	m_pstCfg->luria_svc.useEdgeTrigger = GetConfigUint8(L"USE_EDGE_TRIGGER");

	m_pstCfg->luria_svc.afPanelMode = GetConfigUint8(L"PANELMODE");
	m_pstCfg->luria_svc.afAAQInactivateWithinMicrometer[0] = GetConfigDouble(L"AF_AAQ_RANGE_MIN");
	m_pstCfg->luria_svc.afAAQInactivateWithinMicrometer[1] = GetConfigDouble(L"AF_AAQ_RANGE_MAX");
	
	for (i = 0; i < m_pstCfg->luria_svc.ph_count; i++)
	{
		swprintf_s(tzKey, 64, L"AF_TRIM_PH%d", i + 1);
		m_pstCfg->luria_svc.afTrimMicrometer[i] = GetConfigDouble(tzKey);		
	}

	return TRUE;
}

/*
 desc : 환경 파일 저장 (OPTIC_SERVICE)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfComn::SaveConfigLuriaSvc()
{
	UINT8 i	= 0;
	TCHAR tzKey[MAX_KEY_STRING]	= {NULL};

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"PH_SERVICE");

#if (USE_SIMULATION_DIR)
	SetConfigStr(L"PRINT_SIMULATION_OUT_DIR",		m_pstCfg->luria_svc.print_simulation_out_dir);
#endif
	SetConfigIPv4(L"MOTION_CONTROL_IPv4",			m_pstCfg->luria_svc.motion_control_ip);

	SetConfigUint32(L"TABLE_COUNT",					m_pstCfg->luria_svc.table_count);
	SetConfigUint32(L"LED_COUNT",					m_pstCfg->luria_svc.led_count);
	SetConfigUint32(L"TOTAL_PH",					m_pstCfg->luria_svc.ph_count);
	SetConfigUint32(L"PH_PITCH",					m_pstCfg->luria_svc.ph_pitch);
	SetConfigUint32(L"PH_ROTATE",					m_pstCfg->luria_svc.ph_rotate);
	SetConfigUint32(L"MOTION_CONTROL_TYPE",			m_pstCfg->luria_svc.motion_control_type);
	SetConfigUint32(L"HYS_TYPE_1_SCROLL_MODE",		m_pstCfg->luria_svc.hys_type_1_scroll_mode);
	SetConfigUint32(L"HYS_TYPE_1_NEGATIVE_OFFSET",	m_pstCfg->luria_svc.hys_type_1_negative_offset);
	SetConfigUint32(L"TABLE_1_PRINT_DIRECTION",		m_pstCfg->luria_svc.table_1_print_direction);
	SetConfigUint32(L"TABLE_2_PRINT_DIRECTION",		m_pstCfg->luria_svc.table_2_print_direction);
	SetConfigUint32(L"ACTIVE_TABLE",				m_pstCfg->luria_svc.active_table_no);
	SetConfigUint32(L"EMULATE_MOTION",				m_pstCfg->luria_svc.emulate_motion);
	SetConfigUint32(L"EMULATE_PH",					m_pstCfg->luria_svc.emulate_ph);
	SetConfigUint32(L"EMULATE_TRIGGER",				m_pstCfg->luria_svc.emulate_trigger);
	SetConfigUint32(L"DEBUG_PRINT_LEVEL",			m_pstCfg->luria_svc.debug_print_level);
	SetConfigUint32(L"ARTWORK_COMPLEXITY",			m_pstCfg->luria_svc.artwork_complexity);
	SetConfigUint32(L"Z_DRIVE_TYPE",				m_pstCfg->luria_svc.z_drive_type);
	SetConfigUint32(L"OVER_PRESSURE_MODE",			m_pstCfg->luria_svc.over_pressure_mode);
	SetConfigUint32(L"ILLUM_MEAS_TYPE",				m_pstCfg->luria_svc.illum_meas_type);
	
	SetConfigUint32(L"SCROLL_RATE",					m_pstCfg->luria_svc.scroll_rate);
	SetConfigUint32(L"DMD_MIRROR_COUNT_X",			m_pstCfg->luria_svc.mirror_count_xy[0]);
	SetConfigUint32(L"DMD_MIRROR_COUNT_Y",			m_pstCfg->luria_svc.mirror_count_xy[1]);
	

	SetConfigUint32(L"PRODUCT_ID",					m_pstCfg->luria_svc.product_id);

	SetConfigUint32(L"OPTIC_VER_MAJOR",				m_pstCfg->luria_svc.luria_ver_major);
	SetConfigUint32(L"OPTIC_VER_MINOR",				m_pstCfg->luria_svc.luria_ver_minor);
	SetConfigUint32(L"OPTIC_VER_BUILD",				m_pstCfg->luria_svc.luria_ver_build);

	

	SetConfigDouble(L"MAX_Y_MOTION_SPEED",			m_pstCfg->luria_svc.max_y_motion_speed,			3);
	SetConfigDouble(L"X_MOTION_SPEED",				m_pstCfg->luria_svc.x_motion_speed,				3);
	SetConfigDouble(L"Y_ACCELERATION_DISTANCE",		m_pstCfg->luria_svc.y_acceleration_distance,	3);
	SetConfigDouble(L"HYS_TYPE_1_DELAY_POSITIVE",	m_pstCfg->luria_svc.hys_type_1_delay_offset[0],	6);
	SetConfigDouble(L"HYS_TYPE_1_DELAY_NEGATIVE",	m_pstCfg->luria_svc.hys_type_1_delay_offset[1],	6);
#if (CUSTOM_CODE_UVDI15_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_UVDI15_LLS06 == DELIVERY_PRODUCT_ID)
	SetConfigDouble(L"PH_Z_AXIS_MOVE_MAX",			m_pstCfg->luria_svc.ph_z_move_max,				3);
	SetConfigDouble(L"PH_Z_AXIS_MOVE_MIN",			m_pstCfg->luria_svc.ph_z_move_min,				3);
#endif
	SetConfigDouble(L"PIXEL_SIZE",					m_pstCfg->luria_svc.pixel_size,					6);
	
	for (i = 0; i < MAX_SELECT_VELO; i++)
	{
		swprintf_s(tzKey, MAX_KEY_STRING, L"PH_Z_SELECT_VELO_%d", i);
		SetConfigDouble(tzKey, m_pstCfg->luria_svc.ph_z_select_velo[i], 4);
	}

	SetConfigDouble(L"SENSOR_DIAMETER",				m_pstCfg->luria_svc.sensor_diameter,			3);
	SetConfigDouble(L"CORRECTION_FACTOR",			m_pstCfg->luria_svc.correction_factor,			3);

	for (i=0; i<m_pstCfg->luria_svc.table_count; i++)
	{
		swprintf_s(tzKey, MAX_KEY_STRING, L"X_CORRECTION_TABLE_%u", i+1);
		switch (i)
		{
		case 0x00 : SetConfigUint32(tzKey, m_pstCfg->luria_svc.x_correction_table_1);	break;
		case 0x01 : SetConfigUint32(tzKey, m_pstCfg->luria_svc.x_correction_table_2);	break;
		}
		swprintf_s(tzKey, MAX_KEY_STRING, L"Y_CORRECTION_TABLE_%u", i+1);
		switch (i)
		{
		case 0x00 : SetConfigUint32(tzKey, m_pstCfg->luria_svc.y_correction_table_1);	break;
		case 0x01 : SetConfigUint32(tzKey, m_pstCfg->luria_svc.y_correction_table_2);	break;
		}
	}
	for (i=1; i<=m_pstCfg->luria_svc.ph_count; i++)
	{
		swprintf_s(tzKey, MAX_KEY_STRING, L"PH_IPv4_%02d", i);
		SetConfigIPv4(tzKey, m_pstCfg->luria_svc.ph_ipv4[i-1]);

		swprintf_s(tzKey, 64, L"PH_Z_AXIS_FOCUS_%d", i);
		SetConfigDouble(tzKey,	m_pstCfg->luria_svc.ph_z_focus[i-1],4);
	}

	for (i=1; i<m_pstCfg->luria_svc.ph_count; i++)
	{
		swprintf_s(tzKey, MAX_KEY_STRING, L"PH_OFFSET_X_%d", i+1);
		SetConfigDouble(tzKey,	m_pstCfg->luria_svc.ph_offset_x[i],	6);
		swprintf_s(tzKey, MAX_KEY_STRING, L"PH_OFFSET_Y_%d", i+1);
		SetConfigDouble(tzKey,	m_pstCfg->luria_svc.ph_offset_y[i],	6);
	}

	for (i=1; i<=m_pstCfg->luria_svc.table_count; i++)
	{
		swprintf_s(tzKey, MAX_KEY_STRING, L"PARALLELOGRAM_ADJUST_%d", i);
		SetConfigUint32(tzKey,	m_pstCfg->luria_svc.paralleogram_motion_adjust[i-1]);
		swprintf_s(tzKey, 64, L"X_DRIVE_ID_%d", i);
		if (i==1)	SetConfigUint32(tzKey,	m_pstCfg->luria_svc.x_drive_id_1);
		else		SetConfigUint32(tzKey,	m_pstCfg->luria_svc.x_drive_id_2);
		swprintf_s(tzKey, 64, L"Y_DRIVE_ID_%d", i);
		if (i==1)	SetConfigUint32(tzKey,	m_pstCfg->luria_svc.y_drive_id_1);
		else		SetConfigUint32(tzKey,	m_pstCfg->luria_svc.y_drive_id_2);
	}
	for (i=1; i<=m_pstCfg->luria_svc.table_count; i++)
	{
		swprintf_s(tzKey, MAX_KEY_STRING, L"TABLE_LIMIT_MIN_X_%d", i);
		SetConfigDouble(tzKey,	m_pstCfg->luria_svc.table_limit_min_xy[i-1][0],		3);
		swprintf_s(tzKey, MAX_KEY_STRING, L"TABLE_LIMIT_MIN_Y_%d", i);
		SetConfigDouble(tzKey,	m_pstCfg->luria_svc.table_limit_min_xy[i-1][1],		3);
		swprintf_s(tzKey, MAX_KEY_STRING, L"TABLE_LIMIT_MAX_X_%d", i);
		SetConfigDouble(tzKey,	m_pstCfg->luria_svc.table_limit_max_xy[i-1][0],		3);
		swprintf_s(tzKey, MAX_KEY_STRING, L"TABLE_LIMIT_MAX_Y_%d", i);
		SetConfigDouble(tzKey,	m_pstCfg->luria_svc.table_limit_max_xy[i-1][1],		3);

		swprintf_s(tzKey, MAX_KEY_STRING, L"TABLE_EXPO_START_X_%d", i);
		SetConfigDouble(tzKey,	m_pstCfg->luria_svc.table_expo_start_xy[i-1][0],	3);
		swprintf_s(tzKey, MAX_KEY_STRING, L"TABLE_EXPO_START_Y_%d", i);
		SetConfigDouble(tzKey,	m_pstCfg->luria_svc.table_expo_start_xy[i-1][1],	3);
	}
	for (i=0; i<m_pstCfg->luria_svc.led_count; i++)
	{
		swprintf_s(tzKey, 64, L"ILLUM_FILT_RATE_LED_%u", i);
		SetConfigDouble(tzKey,	m_pstCfg->luria_svc.illum_filter_rate[i],	5);
	}
	SetConfigDouble(L"ILLUM_GARBAGE_PWR",	m_pstCfg->luria_svc.illum_garbage_pwr,	4);


	////
	SetConfigInt32(L"USE_AF", (INT32)m_pstCfg->luria_svc.useAF);
	SetConfigInt32(L"USE_AAQ", (INT32)m_pstCfg->luria_svc.useAAQ);
	SetConfigInt32(L"USE_EDGE_TRIGGER", (INT32)m_pstCfg->luria_svc.useEdgeTrigger);

	SetConfigInt32(L"PANELMODE", (INT32)m_pstCfg->luria_svc.afPanelMode);
	SetConfigInt32(L"AF_GAIN", (INT32)m_pstCfg->luria_svc.afGain);

	SetConfigDouble(L"AF_RANGE_MIN_ALL",	m_pstCfg->luria_svc.afWorkRangeWithinMicrometer[0],	3);
	SetConfigDouble(L"AF_RANGE_MAX_ALL",	m_pstCfg->luria_svc.afWorkRangeWithinMicrometer[1],	3);

	SetConfigDouble(L"AF_AAQ_RANGE_MIN", m_pstCfg->luria_svc.afAAQInactivateWithinMicrometer[0], 3);
	SetConfigDouble(L"AF_AAQ_RANGE_MAX", m_pstCfg->luria_svc.afAAQInactivateWithinMicrometer[1], 3);

	for (i = 0; i < m_pstCfg->luria_svc.ph_count; i++)
	{
		swprintf_s(tzKey, 64, L"AF_TRIM_PH%d", i+1);
		SetConfigDouble(tzKey, m_pstCfg->luria_svc.afTrimMicrometer[i], 3);
	}

	return TRUE;
}

/*
 desc : 환경 파일 적재 (MC2_SERVICE)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfComn::LoadConfigMC2Svc()
{
	UINT8 i	= 0;
	TCHAR tzKey[MAX_KEY_STRING]	= {NULL};

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"MC2_SERVICE");

	m_pstCfg->mc2_svc.drive_count	= GetConfigUint8(L"DRIVE_COUNT");
	m_pstCfg->mc2_svc.origin_point	= GetConfigUint8(L"ORIGIN_POINT");
	m_pstCfg->mc2_svc.mc2_serial	= GetConfigUint32(L"MC2_SERIAL");
	m_pstCfg->mc2_svc.move_velo		= GetConfigDouble(L"MOVE_VELO");
	m_pstCfg->mc2_svc.mark_velo		= GetConfigDouble(L"MARK_VELO");
	m_pstCfg->mc2_svc.step_velo		= GetConfigDouble(L"STEP_VELO");
	m_pstCfg->mc2_svc.move_dist		= GetConfigDouble(L"MOVE_DIST");

	for (; i<m_pstCfg->mc2_svc.drive_count; i++)
	{
		swprintf_s(tzKey, MAX_KEY_STRING, L"AXIS_ID_%d", i+1);
		m_pstCfg->mc2_svc.axis_id[i]	= GetConfigUint8(tzKey);
		swprintf_s(tzKey, MAX_KEY_STRING, L"MAX_AXIS_%d_VELO", m_pstCfg->mc2_svc.axis_id[i]);
		m_pstCfg->mc2_svc.max_velo[m_pstCfg->mc2_svc.axis_id[i]]= GetConfigDouble(tzKey);
		swprintf_s(tzKey, MAX_KEY_STRING, L"MAX_AXIS_%d_DIST", m_pstCfg->mc2_svc.axis_id[i]);
		m_pstCfg->mc2_svc.max_dist[m_pstCfg->mc2_svc.axis_id[i]]= GetConfigDouble(tzKey);
		swprintf_s(tzKey, MAX_KEY_STRING, L"MIN_AXIS_%d_DIST", m_pstCfg->mc2_svc.axis_id[i]);
		m_pstCfg->mc2_svc.min_dist[m_pstCfg->mc2_svc.axis_id[i]]= GetConfigDouble(tzKey);

		for (int j=0; j< MAX_SELECT_VELO; j++)
		{
			swprintf_s(tzKey, MAX_KEY_STRING, L"AXIS_%d_VELO_SELECT_%d", m_pstCfg->mc2_svc.axis_id[i], j);
			m_pstCfg->mc2_svc.select_velo[m_pstCfg->mc2_svc.axis_id[i]][j] = GetConfigDouble(tzKey);

		}
	}

#if (MC2_DRIVE_2SET == 1)
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"MC2B_SERVICE");

	m_pstCfg->mc2b_svc.drive_count = GetConfigUint8(L"DRIVE_COUNT");
	m_pstCfg->mc2b_svc.origin_point = GetConfigUint8(L"ORIGIN_POINT");
	m_pstCfg->mc2b_svc.mc2_serial = GetConfigUint32(L"MC2_SERIAL");
	m_pstCfg->mc2b_svc.move_velo = GetConfigDouble(L"MOVE_VELO");
	m_pstCfg->mc2b_svc.mark_velo = GetConfigDouble(L"MARK_VELO");
	m_pstCfg->mc2b_svc.step_velo = GetConfigDouble(L"STEP_VELO");
	m_pstCfg->mc2b_svc.move_dist = GetConfigDouble(L"MOVE_DIST");

	for (i=0; i < m_pstCfg->mc2b_svc.drive_count; i++)
	{
		swprintf_s(tzKey, MAX_KEY_STRING, L"AXIS_ID_%d", i + 1);
		m_pstCfg->mc2b_svc.axis_id[i] = GetConfigUint8(tzKey);
		swprintf_s(tzKey, MAX_KEY_STRING, L"MAX_AXIS_%d_VELO", m_pstCfg->mc2b_svc.axis_id[i]);
		m_pstCfg->mc2b_svc.max_velo[m_pstCfg->mc2b_svc.axis_id[i]] = GetConfigDouble(tzKey);
		swprintf_s(tzKey, MAX_KEY_STRING, L"MAX_AXIS_%d_DIST", m_pstCfg->mc2b_svc.axis_id[i]);
		m_pstCfg->mc2b_svc.max_dist[m_pstCfg->mc2b_svc.axis_id[i]] = GetConfigDouble(tzKey);
		swprintf_s(tzKey, MAX_KEY_STRING, L"MIN_AXIS_%d_DIST", m_pstCfg->mc2b_svc.axis_id[i]);
		m_pstCfg->mc2b_svc.min_dist[m_pstCfg->mc2b_svc.axis_id[i]] = GetConfigDouble(tzKey);
	}
#endif


	return TRUE;
}

/*
 desc : 환경 파일 적재 (MC2_SERVICE)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfComn::SaveConfigMC2Svc()
{
	UINT8 i	= 0;
	TCHAR tzKey[MAX_KEY_STRING]	= {NULL};

	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"MC2_SERVICE");

	SetConfigUint32(L"DRIVE_COUNT",	m_pstCfg->mc2_svc.drive_count);
	SetConfigUint32(L"ORIGIN_POINT",m_pstCfg->mc2_svc.origin_point);
	SetConfigUint32(L"MC2_SERIAL",	m_pstCfg->mc2_svc.mc2_serial);
	SetConfigDouble(L"MOVE_VELO",	m_pstCfg->mc2_svc.move_velo,	4);
	SetConfigDouble(L"MARK_VELO",	m_pstCfg->mc2_svc.mark_velo,	4);
	SetConfigDouble(L"MOVE_DIST",	m_pstCfg->mc2_svc.move_dist,	4);

	for (; i<m_pstCfg->mc2_svc.drive_count; i++)
	{
		swprintf_s(tzKey, MAX_KEY_STRING, L"AXIS_ID_%d", i+1);
		SetConfigUint32(tzKey,	m_pstCfg->mc2_svc.axis_id[i]);
		swprintf_s(tzKey, MAX_KEY_STRING, L"MAX_AXIS_%d_VELO", m_pstCfg->mc2_svc.axis_id[i]);
		SetConfigDouble(tzKey,	m_pstCfg->mc2_svc.max_velo[m_pstCfg->mc2_svc.axis_id[i]],	4);
		swprintf_s(tzKey, MAX_KEY_STRING, L"MAX_AXIS_%d_DIST", m_pstCfg->mc2_svc.axis_id[i]);
		SetConfigDouble(tzKey,	m_pstCfg->mc2_svc.max_dist[m_pstCfg->mc2_svc.axis_id[i]],	4);

		for (int j = 0; j < MAX_SELECT_VELO; j++)
		{
			swprintf_s(tzKey, MAX_KEY_STRING, L"AXIS_%d_VELO_SELECT_%d", m_pstCfg->mc2_svc.axis_id[i], j);
			SetConfigDouble(tzKey, m_pstCfg->mc2_svc.select_velo[m_pstCfg->mc2_svc.axis_id[i]][j], 4);

		}
	}

#if (MC2_DRIVE_2SET == 1)
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"MC2B_SERVICE");

	SetConfigUint32(L"DRIVE_COUNT", m_pstCfg->mc2b_svc.drive_count);
	SetConfigUint32(L"ORIGIN_POINT", m_pstCfg->mc2b_svc.origin_point);
	SetConfigUint32(L"MC2_SERIAL", m_pstCfg->mc2b_svc.mc2_serial);
	SetConfigDouble(L"MOVE_VELO", m_pstCfg->mc2b_svc.move_velo, 4);
	SetConfigDouble(L"MARK_VELO", m_pstCfg->mc2b_svc.mark_velo, 4);
	SetConfigDouble(L"MOVE_DIST", m_pstCfg->mc2b_svc.move_dist, 4);

	for (; i < m_pstCfg->mc2b_svc.drive_count; i++)
	{
		swprintf_s(tzKey, MAX_KEY_STRING, L"AXIS_ID_%d", i + 1);
		SetConfigUint32(tzKey, m_pstCfg->mc2b_svc.axis_id[i]);
		swprintf_s(tzKey, MAX_KEY_STRING, L"MAX_AXIS_%d_VELO", m_pstCfg->mc2b_svc.axis_id[i]);
		SetConfigDouble(tzKey, m_pstCfg->mc2b_svc.max_velo[m_pstCfg->mc2b_svc.axis_id[i]], 4);
		swprintf_s(tzKey, MAX_KEY_STRING, L"MAX_AXIS_%d_DIST", m_pstCfg->mc2b_svc.axis_id[i]);
		SetConfigDouble(tzKey, m_pstCfg->mc2b_svc.max_dist[m_pstCfg->mc2b_svc.axis_id[i]], 4);
	}
#endif


	return TRUE;
}

/*
 desc : 환경 파일 적재 (MELSEC_Q_SERVICE)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfComn::LoadConfigMelsecQSvc()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"MELSEC_Q_SERVICE");

	m_pstCfg->melsec_q_svc.start_io		= GetConfigHex16(L"START_IO");
	m_pstCfg->melsec_q_svc.group_no		= GetConfigUint8(L"GROUP_NO");
	m_pstCfg->melsec_q_svc.network_no	= GetConfigUint8(L"NETWORK_NO");
	m_pstCfg->melsec_q_svc.station_no	= GetConfigUint8(L"STATION_NO");
	m_pstCfg->melsec_q_svc.plc_no		= GetConfigHex8(L"PLC_NO");
	m_pstCfg->melsec_q_svc.addr_x_count	= GetConfigUint16(L"ADDR_X_COUNT");
	m_pstCfg->melsec_q_svc.addr_y_count	= GetConfigUint16(L"ADDR_Y_COUNT");
	m_pstCfg->melsec_q_svc.addr_m_count	= GetConfigUint16(L"ADDR_M_COUNT");
	m_pstCfg->melsec_q_svc.addr_d_count	= GetConfigUint16(L"ADDR_D_COUNT");
	m_pstCfg->melsec_q_svc.start_d_addr	= GetConfigUint32(L"ADDR_D_START");

	return TRUE;
}

/*
 desc : 환경 파일 저장 (MELSEC_Q_SERVICE)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfComn::SaveConfigMelsecQSvc()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"MELSEC_Q_SERVICE");

	SetConfigUint32(L"ADDR_D_COUNT",	m_pstCfg->melsec_q_svc.addr_d_count);
	SetConfigUint32(L"ADDR_D_START",	m_pstCfg->melsec_q_svc.start_d_addr);

	return TRUE;
}

#if (USE_SIMULATION_DIR)
/*
 desc : Print Simulation Out Dir 생성
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfComn::CreateSimulationOutDir()
{
	if (_tcslen(m_pstCfg->luria_svc.print_simulation_out_dir) < 4)	return TRUE;	/* 생성하지 않고 TRUE 반환 */

	/* 강제로 경로 생성 */
	return uvCmn_CreateDirectory(m_pstCfg->luria_svc.print_simulation_out_dir);
}
#endif

