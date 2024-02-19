
/*
 desc : �ø��� ��� ������ (for Prealigner)
*/

#include "pch.h"
#include "MPAThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : port	- [in]  ��� ��Ʈ ��ȣ (1 ~ 255)
		baud	- [in]  ��� ��Ʈ �ӵ� (CBR_100 ~ CBR_CBR_256000 �� �Է�)
						300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 1152000
		buff	- [in]  ��� ��Ʈ�� ��/���� ���� ũ��
		shmem	- [in]  ���� �޸�
 retn : None
*/
CMPAThread::CMPAThread(UINT8 port, UINT32 baud, UINT32 buff, LPG_MPDV shmem)
	: CMilaraThread(ENG_EDIC::en_lspa, port, baud, buff)
{
	m_pstShMem		= shmem;
	m_enLastCmd		= ENG_PSCC::psc_inf;	/* !!! Important !!! */
	/* ���� �����Ǹ� �ݵ�� ���� */
	m_u8ReplySkip	= 0x00;	/* !!! Important ('r'+'n'�� �ߺ��ؼ� ������ 0x02) !!! */
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CMPAThread::~CMPAThread()
{
}

/*
 desc : �ø��� ��� ��Ʈ ���� �����ϸ�, Prealigner���� �ʱ�ȭ ����� ������� ������
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMPAThread::Initialized()
{
	switch (m_u8InitStep)
	{
	case 0x00	:	/* ������ Info Mode �� 1�� ���� */
		m_enLastCmd	= ENG_PSCC::psc_none;
		if (!CMilaraThread::SendData("INF 1"))	return FALSE;
		LOG_MESG(ENG_EDIC::en_lspa, L"Succeeded in sending INF_1 command");
		m_u8InitStep++;
		break;
	case 0x01	:	/* Info Mode ������ ���� ���� ���� ���Դ��� ���� */
		if (ENG_MSAS::none == m_enLastSendAck)	return TRUE;
		m_u8InitStep++;
		break;
	case 0x02	:	/* ������ Info Mode �� ��û */
		m_pstShMem->info_mode	= UINT8_MAX;
		m_enLastCmd	= ENG_PSCC::psc_inf;
		if (!CMilaraThread::SendData("INF"))	return FALSE;
		LOG_MESG(ENG_EDIC::en_lspa, L"Succeeded in sending INF command");
		m_u8InitStep++;
		break;
	case 0x03	:	/* Info Mode ���� 1�� ���ŵǾ����� Ȯ�� */
		if (m_pstShMem->info_mode != 0x01)		return FALSE;
		m_bIsInited	= TRUE;	/* The end */
		LOG_MESG(ENG_EDIC::en_lspa, L"Succeeded in initialized the Pre-Aligner");
		break;
	}

	return TRUE;
}

/*
 desc : �� ��(R, Z, T) �� �Էµ� ���� ���α׷��� �����Ǵ� ������ ��ȯ
 parm : axis	- [in]  Axis Name (R : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  ���� R�� Z�� ���, inch
						���� T�� ���, degree
 retn : �� ��ȯ (������ inch or degree --> �Ǽ��� um or degree)
 */
DOUBLE CMPAThread::GetAxisPosApp(ENG_PANC axis, INT32 value)
{
	DOUBLE dbValue	= 0.0f;

	/* �ԷµǴ� ���� ���� ���� ��ȯ�� �ٸ� */
	switch (axis)
	{
	case ENG_PANC::pac_t	:	dbValue = value * 0.1;	break;	/* 1 = 0.1�� */
	case ENG_PANC::pac_r	:
	case ENG_PANC::pac_z	:	dbValue = value * 2.54;	break;	/* 1 = 0.0001 inch, 0.0001 inc = 2.54 um */
	}

	return dbValue;
}

/*
 desc : Wafer �߽� ��ġ�� ���� �� ��� (RWI)
 parm : index	- [in]  0x00 (Inch), 0x01 (Degree), 0x02 (Degree)
		value	- [in]  ���� R�� Z�� ���, inch
						���� T�� ���, degree
 retn : �� ��ȯ (������ inch or degree --> �Ǽ��� um or degree)
 */
DOUBLE CMPAThread::GetWaferCentDirect(UINT8 index, INT32 value)
{
	DOUBLE dbValue	= 0.0f;

	/* �ԷµǴ� ���� ���� ���� ��ȯ�� �ٸ� */
	switch (index)
	{
	case 0x00	:	dbValue = value * 2.54;	break;	/* 1 = 0.0001 inch, 0.0001 inc = 2.54 um */
	case 0x01	:	dbValue = value * 0.1;	break;	/* 1 = 0.1�� */
	case 0x02	:	dbValue = value * 0.01;	break;	/* 1 = 0.1�� */
	}

	return dbValue;
}

/*
 desc : ��� �۽� �� ���� �ޱ�
 parm : code- [in]  Send ����   ��ɾ� ���� �ڵ� ��
		cmd	- [in]  Send ���ڿ� ��ɾ ����� ����
 retn : Null or ���ŵ� �����Ͱ� ����� ���� ���� ������
*/
BOOL CMPAThread::SendData(ENG_PSCC code, PCHAR cmd)
{
	CHAR szMesg[128]	= {NULL};
	CUniToChar csCnv;

	if (!m_bIsInited)
	{
		LOG_WARN(ENG_EDIC::en_lspa, L"Milara communication (Prealigner) is not initialized.");
		return FALSE;
	}

	/* Info Mode ��ɾ��� ���� �ش� ���� ���� */
	if (ENG_PSCC::psc_inf != code && m_pstShMem->info_mode != 1)
	{
		LOG_WARN(ENG_EDIC::en_lspa, L"Send: The <INF 0/2/3> mode is not supported");
#ifdef _DEBUG
		AfxMessageBox(L"Send: The <INF 0/2/3> mode is not supported", MB_ICONSTOP|MB_TOPMOST);
#endif
		return FALSE;
	}

	/* ��� �۽� ��ɾ �α׿� ���� */
	sprintf_s(szMesg, 128,
				"The sent prealigner command has been saved [cmd_code : %02x] [cmd_str : %s]",
				code, cmd);
	LOG_MESG(ENG_EDIC::en_lspa, csCnv.Ansi2Uni(szMesg));

	/* ���� Teaching Mode�� ���, Teching ���� �̿��� ����� �����ϵ��� ó�� */
	if (m_bIsTechMode)
	{
		if (!IsValidTechingSet(code))
		{
			LOG_WARN(ENG_EDIC::en_lspa,
					 L"Send: Currently, only commands related to Teching Mode are valid.");
#ifdef _DEBUG
			AfxMessageBox(L"Send: Currently, only commands related to Teching Mode are valid.", MB_ICONSTOP|MB_TOPMOST);
#endif
		return FALSE;
		}
	}

	/* ���� �ֱٿ� �۽��� ��ɾ� ���� �ڵ� �� ���� */
	m_enLastCmd	= code;

	/* Motion reply �ʱ�ȭ */
 	switch (code)
 	{
	case ENG_PSCC::psc_hom	:
	case ENG_PSCC::psc_mth	:
	case ENG_PSCC::psc_mva	:
	case ENG_PSCC::psc_mvr	:
	case ENG_PSCC::psc_mmtl	:
	case ENG_PSCC::psc_mmtm	:
	case ENG_PSCC::psc_sof	:
	case ENG_PSCC::psc_son	:
	case ENG_PSCC::psc_stp	:
	case ENG_PSCC::psc_bal	:
		m_pstShMem->system_status = 0xffff;	/* �ý��� ���°� ��ȿ���� �ʴٰ� ���� */
		/* !!! important !!! */
		m_enLastCmd	= ENG_PSCC::psc_sta;
		break;
	}

	/* ���� ���ŵ� ������ �ʱ�ȭ */
	ResetRecvData(code);

	/* ��ɾ� ���� */
	return CMilaraThread::SendData(cmd);
}

/*
 desc : ���� ������ ó��
 parm : None
 retn : None
*/
VOID CMPAThread::RecvData()
{
	CHAR szMesg[LOG_MESG_SIZE]	= {NULL};
	UINT32 u32Count	= 0;
	vector <string> vVals;
	CUniToChar csCnv;

#if 0	/* �ϴ�, �� ����� ���� (info ��忡 ���� ������ �޴� ���� ���͸��� ������ �ִ��� ?) */
	/* �ϴ�. INF 0, 2, 3, 4, 5 ���� �������� �ʴ´�. (�� ����) */
	if (m_pstShMem->info_mode != 1 && m_enLastCmd != ENG_PSCC::psc_inf)
	{
		memset(m_pstShMem->last_string_data, 0x00, LAST_STRING_DATA_SIZE);
		strcpy_s(m_pstShMem->last_string_data, MAX_CMD_MILARA_RECV, (PCHAR)m_pCmdRecv);
		LOG_WARN(ENG_EDIC::en_lspa, L"The <INF 1> mode is only supported");
		return;
	}
#endif

	if (GetConfig()->set_comn.comm_log_milara)
	{
		UINT32 u32Read		= m_u32RecvSize, i = 0;
		CHAR szPkt[4]		= {NULL};
		TCHAR tzMesg[256]	= {NULL};
		CUniToChar csCnv;
		/* ���Ź��� ��Ŷ ���ڿ��� �ӽ� ���� (16����) */
		memset(m_pszLastPkt, 0x00, MAX_CMD_MILARA_RECV * 3 + 1);
		if (u32Read > MAX_CMD_MILARA_RECV)	u32Read = MAX_CMD_MILARA_RECV;
		for (; i<u32Read; i++)
		{
			sprintf_s(szPkt, 4, "%02x ", m_pCmdRecv[i]);
			strcat_s(m_pszLastPkt, MAX_CMD_MILARA_RECV*3, szPkt);
		}
		swprintf_s(tzMesg, 256, L"PODIS vs. Aligner [Recv] [called_func=RecvData] : %s",
				   csCnv.Ansi2Uni(m_pszLastPkt));
		LOG_MESG(m_enAppsID, tzMesg);
	}
#if 0
	CUniToChar csCnv;
	TRACE(L"RS232_PORT[%d]: apps_id (%02x) %s\n",
			m_u8Port, UINT8(m_enAppsID), csCnv.Ansi2Uni(m_pszLastPkt));
#endif


	/* ������ �м� */
	CMilaraThread::GetValues(vVals);
	/* ���ŵ� ������ ���� */
	u32Count = (UINT32)vVals.size();
	if (u32Count < 1)	return;
	/* psc_val ��ɾ ���ŵǸ� ������ sta ��ɾ ���� �������� ���� */
	else if (u32Count == 1)
	{
		switch (m_enLastCmd)
		{
		case ENG_PSCC::psc_bal:	m_enLastCmd = ENG_PSCC::psc_sta;	break;
		}
	}

	/* ������ �۽��� ��ɾ� ����, ���� ������ ó�� */
	if (m_enLastCmd <= ENG_PSCC::psc_ver)				RecvStatusInfoCmd(vVals);					/* 1. Status and Information Commands */
	else if (m_enLastCmd <= ENG_PSCC::psc_spd)			RecvAxisParamSetRetrieveCmd(vVals);			/* 2. Axis Parameters Set and Retrieve */
	else if (m_enLastCmd <= ENG_PSCC::psc_wmc)			RecvBasicControlCmd(vVals);					/* 3. Basic Axis Control */
	else if (m_enLastCmd <= ENG_PSCC::psc_wsz2)			RecvAlignmentCmd(vVals);					/* 4. Alignment Commands */
	else if (m_enLastCmd <= ENG_PSCC::psc_wps)			RecvHousekeepingCmd(vVals);					/* 5. Housekeeping Commands */
	else if (m_enLastCmd <= ENG_PSCC::psc_drps)			RecvCaliSvcCmd(vVals);						/* 6. Calibration and Service Commands */
	else if (m_enLastCmd <= ENG_PSCC::psc__sfz)			RecvEdgeHandlingPreAlignerSpecCmd(vVals);	/* 7. Edge Handling Prealigner-Specific Commands */
	else if (m_enLastCmd <= ENG_PSCC::psc__notchnavg)	RecvExoticCmd(vVals);						/* 8. Exotic Commands */
	else if (m_enLastCmd <= ENG_PSCC::psc_res)			RecvMiscCmd(vVals);							/* 9. Miscellaneous Commands */

	/* ��� ���� ���� */
	vVals.clear();
}

/*
 desc : ���� ������ ó�� - 1. Status and Information Commands
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMPAThread::RecvStatusInfoCmd(vector <string> &vals)
{
	switch (m_enLastCmd)
	{
	case ENG_PSCC::psc_mcpo	: SetAxisValues(m_pstShMem->axis_pos, vals);		break;	/* Read current position */
							  
	case ENG_PSCC::psc_diag	: 															/* Display current/last error diagnostics */
	case ENG_PSCC::psc_est	: 															/* Extended error status */
	case ENG_PSCC::psc_ver	: SetTempTextInfo(vals);							break;	/* Read firmware version number */
	case ENG_PSCC::psc_per	: SetValue(m_pstShMem->previous_cmd_fail, vals);	break;	/* Display reason for previous error */
	case ENG_PSCC::psc_inf	: SetValue(m_pstShMem->info_mode, vals);			break;	/* Set/Display inform mode */
	case ENG_PSCC::psc_rhs	: SetValue(m_pstShMem->axis_switch_active, vals);	break;	/* Read home switches state */
	case ENG_PSCC::psc_sta	: SetValue(m_pstShMem->system_status, vals);		break;	/* Command execution status */
	}
}

/*
 desc : ���� ������ ó�� - 2. Axis Parameters Set and Retrieve
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMPAThread::RecvAxisParamSetRetrieveCmd(vector <string> &vals)
{
	switch (m_enLastCmd)
	{
	case ENG_PSCC::psc__ch	: SetAxisValues(m_pstShMem->homed_pos, vals);		break;	/* Set/Display customized home position */
	case ENG_PSCC::psc__cl	: SetValues(m_pstShMem->current_limit, vals);		break;	/* Set/Display current limit (1 ~ 251) */
	case ENG_PSCC::psc__el	: SetValues(m_pstShMem->error_limit, vals);			break;	/* Set/Display position error limit (~ 56635) */
	case ENG_PSCC::psc__fl	: SetAxisValues(m_pstShMem->direct_f_limit, vals);	break;	/* Set/Display forward direction software limit */
	case ENG_PSCC::psc__ho	: SetAxisValues(m_pstShMem->home_offset, vals);		break;	/* Set/Display home offset */
	case ENG_PSCC::psc__rl	: SetAxisValues(m_pstShMem->direct_r_limit, vals);	break;	/* Set/Display reverse direction software limit */
	case ENG_PSCC::psc_acl	: SetAxisValues(m_pstShMem->axis_acl, vals);		break;	/* Set/Display axis acceleration */
	case ENG_PSCC::psc_ajk	: SetAxisValues(m_pstShMem->axis_ajk, vals);		break;	/* Set/Display axis acceleration jerk */
	case ENG_PSCC::psc_dcl	: SetAxisValues(m_pstShMem->axis_dcl, vals);		break;	/* Set/Display axis acceleration */
	case ENG_PSCC::psc_djk	: SetAxisValues(m_pstShMem->axis_djk, vals);		break;	/* Set/Display axis deceleration jerk */
	case ENG_PSCC::psc_pro	: SetValues(m_pstShMem->velocity_profile, vals);	break;	/* Set/Display axis velocity profile mode */
	case ENG_PSCC::psc_spd	: SetAxisValues(m_pstShMem->axis_speed, vals);		break;	/* Set/Display speed */
	}
}

/*
 desc : ���� ������ ó�� - 3. Basic Axis Control
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMPAThread::RecvBasicControlCmd(vector <string> &vals)
{
	switch (m_enLastCmd)
	{
	case ENG_PSCC::psc_hom	: ;	break;
	case ENG_PSCC::psc_mth	: ;	break;
	case ENG_PSCC::psc_mva	: ;	break;
	case ENG_PSCC::psc_mvr	: ;	break;
	case ENG_PSCC::psc_sof	: ;	break;
	case ENG_PSCC::psc_son	: ;	break;
	case ENG_PSCC::psc_stp	: ;	break;
	case ENG_PSCC::psc_wmc	: ;	break;
	}
}

/*
 desc : ���� ������ ó�� - 4. Alignment Commands
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMPAThread::RecvAlignmentCmd(vector <string> &vals)
{
	switch (m_enLastCmd)
	{
	case ENG_PSCC::psc__ao2				:	SetValue(m_pstShMem->align_offset_twice, vals);					break;	/* Set/Display align offset twice */
	case ENG_PSCC::psc__aof				:	SetValue(m_pstShMem->align_offset_accept, vals);				break;	/* Set/Display acceptable offset */
	case ENG_PSCC::psc__asz				:	SetValue(m_pstShMem->wafer_size_detection, vals);				break;	/* Set/Display automatic wafer Size detection */
	case ENG_PSCC::psc__autoscanagain	:	SetValue(m_pstShMem->wafer_scan_again, vals);					break;	/* Set/Display align again in case of CCD data error */
	case ENG_PSCC::psc__finalvacc		:	SetValue(m_pstShMem->vacuum_state_after_align, vals);			break;	/* Set/Display vacuum on chuck after BAL state */
	case ENG_PSCC::psc_mglm				:	SetValue(m_pstShMem->wafer_glass_type, vals);					break;	/* Set/Display glass mode operation */
	case ENG_PSCC::psc__ld				:	SetAxisValues(m_pstShMem->wafer_loading_down_pos, vals);		break;	/* Set/Display load down position */
	case ENG_PSCC::psc__lu				:	SetAxisValues(m_pstShMem->wafer_loading_up_pos, vals);			break;	/* Set/Display load up position */
	case ENG_PSCC::psc__ma				:	SetAxisValues(m_pstShMem->measure_acceleration, vals);			break;	/* Set/Display measure acceleration */
	case ENG_PSCC::psc__md				:	SetAxisValues(m_pstShMem->measure_deceleration, vals);			break;	/* Set/Display measure deceleration */
	case ENG_PSCC::psc__mgd				:	SetValue(m_pstShMem->min_good_sample_count, vals);				break;	/* Set/Display minimum good data samples */
	case ENG_PSCC::psc__maxwsz			:	SetValue(m_pstShMem->wafer_max_size, vals);						break;	/* Set/Display maximum wafer size */
	case ENG_PSCC::psc__minwsz			:	SetValue(m_pstShMem->wafer_min_size, vals);						break;	/* Set/Display minimum wafer size */
	case ENG_PSCC::psc__notchdepthmode	:	SetValue(m_pstShMem->notch_depth_mode, vals);					break;	/* Set/Display notch / flat depth calculation mode */
	case ENG_PSCC::psc__mnd				:	SetValue(m_pstShMem->notch_range_depth_min, vals);				break;	/* Set/Display minimum notch depth */
	case ENG_PSCC::psc__mnw				:	SetValue(m_pstShMem->notch_range_width_min, vals);				break;	/* Set/Display minimum notch width */
	case ENG_PSCC::psc__mtpmode			:	SetValue(m_pstShMem->mode_z_after_move_pins, vals);				break;	/* Set/Display MTP command mode of operation */
	case ENG_PSCC::psc__mxd				:	SetValue(m_pstShMem->notch_range_depth_max, vals);				break;	/* Set/Display maximum notch depth */
	case ENG_PSCC::psc__mxw				:	SetValue(m_pstShMem->notch_range_width_max, vals);				break;	/* Set/Display maximum notch width */
	case ENG_PSCC::psc__ms				:	SetAxisValues(m_pstShMem->measure_speed, vals);					break;	/* Set/Display measure speed */
	case ENG_PSCC::psc__pp				:	SetAxisZValue(m_pstShMem->level_chuck_pins_pos_z, vals);		break;	/* Set/Display Z-axis coordinate at pins position */
	case ENG_PSCC::psc__ppcd			:	SetAxisZValue(m_pstShMem->offset_chuck_pin_pos_up, vals);		break;	/* Set/Display pins position correction when going down */
	case ENG_PSCC::psc__ppcu			:	SetAxisZValue(m_pstShMem->offset_chuck_pin_pos_down, vals);		break;	/* Set/Display pins position correction when going up */
	case ENG_PSCC::psc__sa				:	SetAxisTValue(m_pstShMem->ccd_r_axis_angle, vals);				break;	/* Set/Display CCD sensor angle */
	case ENG_PSCC::psc__sam				:	SetValue(m_pstShMem->notch_average_mode, vals);					break;	/* Set/Display sample averaging mode */
	case ENG_PSCC::psc__sc				:	SetValue(m_pstShMem->ccd_sensor_center, vals);					break;	/* Set/Display CCD sensor center value */
	case ENG_PSCC::psc__sm				:	SetValue(m_pstShMem->rotation_sample_mode, vals);				break;	/* Set/Display sampling mode */
	case ENG_PSCC::psc__sp				:	SetValue(m_pstShMem->stop_on_pins, vals);						break;	/* Set/Display stop on pins mode */
	case ENG_PSCC::psc__sqmin			:	SetValue(m_pstShMem->ccd_grab_size_min, vals);					break;	/* Set/Display minimum CCD value for square substrates */
	case ENG_PSCC::psc__sqmax			:	SetValue(m_pstShMem->ccd_grab_size_max, vals);					break;	/* Set/Display maximum CCD value for square substrates */
	case ENG_PSCC::psc__td				:	SetAxisZValue(m_pstShMem->wafer_trans_safe_pos_up, vals);		break;	/* Set/Display transfer down position */
	case ENG_PSCC::psc__to				:	SetValue(m_pstShMem->last_param_timeout, vals);					break;	/* Set/Display timeouts */
	case ENG_PSCC::psc__tu				:	SetAxisZValue(m_pstShMem->wafer_trans_safe_pos_up, vals);		break;	/* Set/Display transfer up position */
	case ENG_PSCC::psc__wszautoset		:	SetValue(m_pstShMem->wafer_size_inch, vals);					break;	/* Assign a set of parameters to wafer size */
	case ENG_PSCC::psc__wt				:	SetValue(m_pstShMem->wafer_type, vals);							break;	/* Set/Display ��wafer�� type */
	case ENG_PSCC::psc_ccd				:	SetValue(m_pstShMem->ccd_pixels, vals);							break;	/* Read CCD sensor */
	case ENG_PSCC::psc_doc				:	SetValue(m_pstShMem->wafer_loaded, vals);						break;	/* Detect an object using CCD sensors (Wafer Loaded) */
	case ENG_PSCC::psc_mfwo				:	SetAxisTValue(m_pstShMem->rotate_after_align, vals);			break;	/* Set/Display final wafer orientation */
	case ENG_PSCC::psc_ltc				:	SetValue(m_pstShMem->wafer_loading_type, vals);					break;	/* Set/Display loading type (on chuck/on pins) */
	case ENG_PSCC::psc_nbn				:	SetValue(m_pstShMem->last_align_fail, vals);					break;	/* Display number of bad notches */
	case ENG_PSCC::psc_ngn				:	SetValue(m_pstShMem->last_align_succ, vals);					break;	/* Display number of good notches */
	case ENG_PSCC::psc_ndw				:	SetValues(m_pstShMem->find_depth_width, vals);					break;	/* Display notch depth and width */
	case ENG_PSCC::psc_mrwi				:	SetAxisMRWI(m_pstShMem->rwi_wafer_info, vals);					break;	/* Read wafer information */
	case ENG_PSCC::psc_wsz				:	SetValue(m_pstShMem->wafer_work_size, vals);					break;	/* Set/Display wafer size */
	case ENG_PSCC::psc_wsz2				:	SetValue(m_pstShMem->notch_light_source, vals);					break;	/* Set/Display wafer size when two LEDs are available */
	}
}

/*
 desc : ���� ������ ó�� - 5. Housekeeping Commands
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMPAThread::RecvHousekeepingCmd(vector <string> &vals)
{
	switch (m_enLastCmd)
	{
	case ENG_PSCC::psc_lps	:	;	break;
	case ENG_PSCC::psc_rmp	:	;	break;
	case ENG_PSCC::psc_rps	:	;	break;
	case ENG_PSCC::psc_smp	:	;	break;
	case ENG_PSCC::psc_sps	:	;	break;
	case ENG_PSCC::psc_wps	:	;	break;
	}
}

/*
 desc : ���� ������ ó�� - 6. Calibration and Service Commands
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMPAThread::RecvCaliSvcCmd(vector <string> &vals)
{
	switch (m_enLastCmd)
	{
	case ENG_PSCC::psc__autodumps	:	SetValue(m_pstShMem->align_fail_auto_dumps, vals);	break;	/* Set/Display wafer size when two LEDs are available */
	case ENG_PSCC::psc__autodumpsf	:	SetValue(m_pstShMem->auto_samples_dump_file, vals);	break;	/* Specifies the file name for CCD samples from unsuccessful alignment */
	case ENG_PSCC::psc_ddpf			:																/* Saves CCD samples to .BAL file */
	case ENG_PSCC::psc_ddumpsamples	:																/* Saves CCD samples to .CSV file */
	case ENG_PSCC::psc_drps			:	SetTempTextInfo(vals);								break;	/* Prepares CCD samples for saving to .BAL file */
	}
}

/*
 desc : ���� ������ ó�� - 7. Edge Handling Prealigner-Specific Commands
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMPAThread::RecvEdgeHandlingPreAlignerSpecCmd(vector <string> &vals)
{
	switch (m_enLastCmd)
	{
	case ENG_PSCC::psc__eh		:	SetValue(m_pstShMem->is_edge_handling, vals);			break;	/* Verifies the prealigner is edge handling */
	case ENG_PSCC::psc__ehc		:	SetValue(m_pstShMem->ccd_chuck_sensing_value, vals);	break;	/* Maximum chuck CCD value for Normal Size */
	case ENG_PSCC::psc__ehc1	:	SetValue(m_pstShMem->ccd_chuck_sensing_small, vals);	break;	/* Maximum chuck CCD value for Smaller Size */
	case ENG_PSCC::psc__ehc2	:	SetValue(m_pstShMem->ccd_chuck_sensing_great, vals);	break;	/* Maximum chuck CCD value for Greater Size */
	case ENG_PSCC::psc__ehp		:	SetValue(m_pstShMem->ccd_pins_sensing_value, vals);		break;	/* Maximum chuck pins CCD value for Normal Size */
	case ENG_PSCC::psc__ehp1	:	SetValue(m_pstShMem->ccd_pins_sensing_small, vals);		break;	/* Maximum chuck pins CCD value for Smaller Size */
	case ENG_PSCC::psc__ehp2	:	SetValue(m_pstShMem->ccd_pins_sensing_great, vals);		break;	/* Maximum chuck pins CCD value for Greater Size */
	case ENG_PSCC::psc__sfz		:	SetAxisZValue(m_pstShMem->safe_rotate_z_pos, vals);		break;	/* Set/Display safe Z position above which which the chuck is clear of the stationary pins can rotate freely */
	}
}

/*
 desc : ���� ������ ó�� - 8. Exotic Commands
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMPAThread::RecvExoticCmd(vector <string> &vals)
{
	switch (m_enLastCmd)
	{
	case ENG_PSCC::psc__mindepthmain		:	SetValue(m_pstShMem->minimum_notch_depth, vals);break;
	case ENG_PSCC::psc__notchfindmode		:	SetValue(m_pstShMem->notch_find_mode, vals);	break;
	case ENG_PSCC::psc__notchmaxderval		:	SetValue(m_pstShMem->notch_max_der_val, vals);	break;
	case ENG_PSCC::psc__notchminderval		:	SetValue(m_pstShMem->notch_min_der_val, vals);	break;
	case ENG_PSCC::psc__notchminderwidth	:	SetValue(m_pstShMem->notch_min_der_width, vals);break;
	case ENG_PSCC::psc__notchnavg			:	SetValue(m_pstShMem->notch_num_average, vals);	break;
	}
}

/*
 desc : ���� ������ ó�� - 9. Miscellaneous Commands
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMPAThread::RecvMiscCmd(vector <string> &vals)
{
	switch (m_enLastCmd)
	{
	case ENG_PSCC::psc__dt	:	SetTempTextInfo(vals);		break;	/* Set system date and time */
	case ENG_PSCC::psc_hsa	:	;	break;	/* HSA */
	case ENG_PSCC::psc_wsa	:	;	break;	/* WSA */
	case ENG_PSCC::psc_res	:	;	break;	/* Reset the controller */
	}
}

/*
 desc : ���� ���ŵ� ������ �ʱ�ȭ
 parm : code	- [in]  Send ����   ��ɾ� ���� �ڵ� ��
 retn : None
*/
VOID CMPAThread::ResetRecvData(ENG_PSCC code)
{
	if (code <= ENG_PSCC::psc_ver)				ResetStatusInfoCmd(code);					/* 1. Status and Information Commands */
	else if (code <= ENG_PSCC::psc_spd)			ResetAxisParamSetRetrieveCmd(code);			/* 2. Axis Param eters Set and Retrieve */
	else if (code <= ENG_PSCC::psc_wmc)			ResetBasicControlCmd(code);					/* 3. Basic Axis Control */
	else if (code <= ENG_PSCC::psc_wsz2)		ResetAlignmentCmd(code);					/* 4. Alignment Commands */
	else if (code <= ENG_PSCC::psc_wps)			ResetHousekeepingCmd(code);					/* 5. Housekeeping Commands */
	else if (code <= ENG_PSCC::psc_drps)		ResetCaliSvcCmd(code);						/* 6. Calibration and Service Commands */
	else if (code <= ENG_PSCC::psc__sfz)		ResetEdgeHandlingPreAlignerSpecCmd(code);	/* 7. Edge Handling Prealigner-Specific Commands */
	else if (code <= ENG_PSCC::psc__notchnavg)	ResetExoticCmd(code);						/* 8. Exotic Commands */
	else if (code <= ENG_PSCC::psc_res)			ResetMiscCmd(code);							/* 9. Miscellaneous Commands */
}
VOID CMPAThread::ResetStatusInfoCmd(ENG_PSCC code)
{
	UINT8 i	= 0x00;
	switch (code)
	{
	case ENG_PSCC::psc_mcpo	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->axis_pos[i] = DBL_MAX;	break;
	case ENG_PSCC::psc_diag	: 
	case ENG_PSCC::psc_ver	:
	case ENG_PSCC::psc_est	: memset(m_pstShMem->last_string_data, 0x00, LAST_STRING_DATA_SIZE);			break;
	case ENG_PSCC::psc_per	: m_pstShMem->previous_cmd_fail		= UINT16_MAX;								break;
	case ENG_PSCC::psc_inf	: m_pstShMem->info_mode				= UINT8_MAX;								break;
	case ENG_PSCC::psc_rhs	: m_pstShMem->axis_switch_active	= 0xfeee;									break;	/* ������ �ʴ� ��ġ�� ��Ʈ�鸸 �ʱ�ȭ ? */
	case ENG_PSCC::psc_sta	: m_pstShMem->system_status			= 0xffff;									break;
	}
}
VOID CMPAThread::ResetAxisParamSetRetrieveCmd(ENG_PSCC code)
{
	UINT8 i	= 0x00;
	switch (code)
	{
	case ENG_PSCC::psc__ch	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->homed_pos[i]		= DBL_MAX;		break;
	case ENG_PSCC::psc__cl	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->current_limit[i]	= UINT8_MAX;	break;
	case ENG_PSCC::psc__el	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->error_limit[i]		= UINT16_MAX;	break;
	case ENG_PSCC::psc__fl	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->direct_f_limit[i]	= DBL_MAX;		break;
	case ENG_PSCC::psc__ho	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->home_offset[i]		= DBL_MAX;		break;
	case ENG_PSCC::psc__rl	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->direct_r_limit[i]	= DBL_MAX;		break;
	case ENG_PSCC::psc_acl	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->axis_acl[i]			= DBL_MAX;		break;
	case ENG_PSCC::psc_ajk	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->axis_ajk[i]			= DBL_MAX;		break;
	case ENG_PSCC::psc_dcl	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->axis_dcl[i]			= DBL_MAX;		break;
	case ENG_PSCC::psc_djk	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->axis_djk[i]			= DBL_MAX;		break;
	case ENG_PSCC::psc_pro	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->velocity_profile[i] = UINT8_MAX;	break;
	case ENG_PSCC::psc_spd	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->axis_speed[i]		= DBL_MAX;		break;
	}
}
VOID CMPAThread::ResetBasicControlCmd(ENG_PSCC code)
{
	switch (code)
	{
	case ENG_PSCC::psc_hom	: ;	break;
	case ENG_PSCC::psc_mth	: ;	break;
	case ENG_PSCC::psc_mva	: ;	break;
	case ENG_PSCC::psc_mvr	: ;	break;
	case ENG_PSCC::psc_sof	: ;	break;
	case ENG_PSCC::psc_son	: ;	break;
	case ENG_PSCC::psc_stp	: ;	break;
	case ENG_PSCC::psc_wmc	: ;	break;
	}
}
VOID CMPAThread::ResetAlignmentCmd(ENG_PSCC code)
{
	UINT8 i	= 0x00;
	switch (code)
	{
	case ENG_PSCC::psc__ao2				:	m_pstShMem->align_offset_twice			= UINT16_MAX;	break;	/* Set/Display limit (1 -> 0.0001 inch) */
	case ENG_PSCC::psc__aof				:	m_pstShMem->align_offset_accept			= UINT16_MAX;	break;	/* Set/Display acceptable offset (1 -> 0.0001 inch) */
	case ENG_PSCC::psc__asz				:	m_pstShMem->wafer_size_detection		= UINT8_MAX;	break;	/* Set/Display automatic wafer Size detection */
	case ENG_PSCC::psc__autoscanagain	:	m_pstShMem->wafer_scan_again			= UINT8_MAX;	break;	/* Set/Display align again in case of CCD data error */
	case ENG_PSCC::psc__finalvacc		:	m_pstShMem->vacuum_state_after_align	= UINT8_MAX;	break;	/* Set/Display vacuum on chuck after BAL state */
	case ENG_PSCC::psc_mglm				:	m_pstShMem->wafer_glass_type			= UINT8_MAX;	break;	/* Set/Display glass mode operation */
	case ENG_PSCC::psc__ld				:	for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->wafer_loading_down_pos[i]	= DBL_MAX;	break;	/* Set/Display load down position */
	case ENG_PSCC::psc__lu				:	for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->wafer_loading_up_pos[i]		= DBL_MAX;	break;	/* Set/Display load up position */
	case ENG_PSCC::psc__ma				:	for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->measure_acceleration[i]		= DBL_MAX;	break;	/* Set/Display measure acceleration */
	case ENG_PSCC::psc__md				:	for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->measure_deceleration[i]		= DBL_MAX;	break;	/* Set/Display measure deceleration */
	case ENG_PSCC::psc__mgd				:	m_pstShMem->min_good_sample_count		= UINT16_MAX;	break;	/* Set/Display minimum good data samples */
	case ENG_PSCC::psc__maxwsz			:	m_pstShMem->wafer_max_size				= UINT8_MAX;	break;	/* Set/Display maximum wafer size */
	case ENG_PSCC::psc__minwsz			:	m_pstShMem->wafer_min_size				= UINT8_MAX;	break;	/* Set/Display minimum wafer size */
	case ENG_PSCC::psc__notchdepthmode	:	m_pstShMem->notch_depth_mode			= UINT8_MAX;	break;	/* Set/Display notch / flat depth calculation mode */
	case ENG_PSCC::psc__mnd				:	m_pstShMem->notch_range_depth_min		= UINT32_MAX;	break;	/* Set/Display minimum notch depth */
	case ENG_PSCC::psc__mnw				:	m_pstShMem->notch_range_width_min		= UINT32_MAX;	break;	/* Set/Display minimum notch width */
	case ENG_PSCC::psc__mtpmode			:	m_pstShMem->mode_z_after_move_pins		= UINT8_MAX;	break;	/* Set/Display MTP command mode of operation */
	case ENG_PSCC::psc__mxd				:	m_pstShMem->notch_range_depth_max		= UINT32_MAX;	break;	/* Set/Display maximum notch depth */
	case ENG_PSCC::psc__mxw				:	m_pstShMem->notch_range_width_max		= UINT32_MAX;	break;	/* Set/Display maximum notch width */
	case ENG_PSCC::psc__ms				:	for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->measure_speed[i]			= DBL_MAX;	break;	/* Set/Display measure speed */
	case ENG_PSCC::psc__pp				:	for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->level_chuck_pins_pos_z		= DBL_MAX;	break;	/* Set/Display Z-axis coordinate at pins position */
	case ENG_PSCC::psc__ppcd			:	for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->offset_chuck_pin_pos_up		= DBL_MAX;	break;	/* Set/Display pins position correction when going down */
	case ENG_PSCC::psc__ppcu			:	for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->offset_chuck_pin_pos_down	= DBL_MAX;	break;	/* Set/Display pins position correction when going up */
	case ENG_PSCC::psc__sa				:	for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->ccd_r_axis_angle			= DBL_MAX;	break;	/* Set/Display CCD sensor angle */
	case ENG_PSCC::psc__sam				:	m_pstShMem->notch_average_mode			= UINT8_MAX;	break;	/* Set/Display sample averaging mode */
	case ENG_PSCC::psc__sc				:	m_pstShMem->ccd_sensor_center			= UINT32_MAX;	break;	/* Set/Display CCD sensor center value */
	case ENG_PSCC::psc__sm				:	m_pstShMem->rotation_sample_mode		= UINT8_MAX;	break;	/* Set/Display sampling mode */
	case ENG_PSCC::psc__sp				:	m_pstShMem->stop_on_pins				= UINT8_MAX;	break;	/* Set/Display stop on pins mode */
	case ENG_PSCC::psc__sqmin			:	m_pstShMem->ccd_grab_size_min			= UINT32_MAX;	break;	/* Set/Display minimum CCD value for square substrates */
	case ENG_PSCC::psc__sqmax			:	m_pstShMem->ccd_grab_size_max			= UINT32_MAX;	break;	/* Set/Display maximum CCD value for square substrates */
	case ENG_PSCC::psc__td				:	m_pstShMem->wafer_trans_safe_pos_up		= DBL_MAX;		break;	/* Set/Display transfer down position */
	case ENG_PSCC::psc__to				:	m_pstShMem->last_param_timeout			= UINT32_MAX;	break;	/* Set/Display timeouts */
	case ENG_PSCC::psc__tu				:	m_pstShMem->wafer_trans_safe_pos_up		= DBL_MAX;		break;	/* Set/Display transfer up position */
	case ENG_PSCC::psc__wszautoset		:	m_pstShMem->wafer_size_inch				= INT8_MAX;		break;	/* Assign a set of parameters to wafer size */
	case ENG_PSCC::psc__wt				:	m_pstShMem->wafer_type					= INT8_MAX;		break;	/* Set/Display ��wafer�� type */
	case ENG_PSCC::psc_ccd				:	m_pstShMem->ccd_pixels					= UINT16_MAX;	break;	/* Read CCD sensor */
	case ENG_PSCC::psc_doc				:	m_pstShMem->wafer_loaded				= UINT8_MAX;	break;	/* Detect an object using CCD sensors (Wafer Loaded) */
	case ENG_PSCC::psc_mfwo				:	m_pstShMem->rotate_after_align			= DBL_MAX;		break;	/* Set/Display final wafer orientation */
	case ENG_PSCC::psc_ltc				:	m_pstShMem->wafer_loading_type			= UINT8_MAX;	break;	/* Set/Display loading type (on chuck/on pins) */
	case ENG_PSCC::psc_nbn				:	m_pstShMem->last_align_fail				= UINT16_MAX;	break;	/* Display number of bad notches */
	case ENG_PSCC::psc_ngn				:	m_pstShMem->last_align_succ				= UINT16_MAX;	break;	/* Display number of good notches */
	case ENG_PSCC::psc_ndw				:	for (; i<2; i++)	m_pstShMem->find_depth_width[i]	= UINT32_MAX;					break;	/* Display notch depth and width */
	case ENG_PSCC::psc_mrwi				:	for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	m_pstShMem->rwi_wafer_info[i]	= DBL_MAX;	break;	/* Read wafer information */
	case ENG_PSCC::psc_wsz				:	m_pstShMem->wafer_work_size				= UINT8_MAX;	break;	/* Set/Display wafer size */
	case ENG_PSCC::psc_wsz2				:	m_pstShMem->notch_light_source			= UINT8_MAX;	break;	/* Set/Display wafer size when two LEDs are available */
	}
}
VOID CMPAThread::ResetHousekeepingCmd(ENG_PSCC code)
{
	switch (code)
	{
	case ENG_PSCC::psc_lps	:	;	break;
	case ENG_PSCC::psc_rmp	:	;	break;
	case ENG_PSCC::psc_rps	:	;	break;
	case ENG_PSCC::psc_smp	:	;	break;
	case ENG_PSCC::psc_sps	:	;	break;
	case ENG_PSCC::psc_wps	:	;	break;
	}
}
VOID CMPAThread::ResetCaliSvcCmd(ENG_PSCC code)
{
	switch (code)
	{
	case ENG_PSCC::psc__autodumps	:	m_pstShMem->align_fail_auto_dumps	= UINT8_MAX;	break;	/* Set/Display wafer size when two LEDs are available */
	case ENG_PSCC::psc__autodumpsf	:	m_pstShMem->auto_samples_dump_file	= UINT8_MAX;	break;	/* Specifies the file name for CCD samples from unsuccessful alignment */
	case ENG_PSCC::psc_ddpf			:																/* Saves CCD samples to .BAL file */
	case ENG_PSCC::psc_ddumpsamples	:																/* Saves CCD samples to .CSV file */
	case ENG_PSCC::psc_drps			:	memset(m_pstShMem->last_string_data, 0x00, LAST_STRING_DATA_SIZE);	break;
	}
}
VOID CMPAThread::ResetEdgeHandlingPreAlignerSpecCmd(ENG_PSCC code)
{
	switch (code)
	{
	case ENG_PSCC::psc__eh		:	m_pstShMem->is_edge_handling		= UINT8_MAX;	break;	/* Verifies the prealigner is edge handling */
	case ENG_PSCC::psc__ehc		:	m_pstShMem->ccd_chuck_sensing_value	= UINT16_MAX;	break;	/* Maximum chuck CCD value for Normal Size */
	case ENG_PSCC::psc__ehc1	:	m_pstShMem->ccd_chuck_sensing_small	= UINT16_MAX;	break;	/* Maximum chuck CCD value for Smaller Size */
	case ENG_PSCC::psc__ehc2	:	m_pstShMem->ccd_chuck_sensing_great	= UINT16_MAX;	break;	/* Maximum chuck CCD value for Greater Size */
	case ENG_PSCC::psc__ehp		:	m_pstShMem->ccd_pins_sensing_value	= UINT16_MAX;	break;	/* Maximum chuck pins CCD value for Normal Size */
	case ENG_PSCC::psc__ehp1	:	m_pstShMem->ccd_pins_sensing_small	= UINT16_MAX;	break;	/* Maximum chuck pins CCD value for Smaller Size */
	case ENG_PSCC::psc__ehp2	:	m_pstShMem->ccd_pins_sensing_great	= UINT16_MAX;	break;	/* Maximum chuck pins CCD value for Greater Size */
	case ENG_PSCC::psc__sfz		:	m_pstShMem->safe_rotate_z_pos		= DBL_MAX;		break;	/* Set/Display safe Z position above which which the chuck is clear of the stationary pins can rotate freely */
	}
}
VOID CMPAThread::ResetExoticCmd(ENG_PSCC code)
{
	switch (code)
	{
	case ENG_PSCC::psc__mindepthmain		:	m_pstShMem->minimum_notch_depth	= UINT8_MAX;	break;
	case ENG_PSCC::psc__notchfindmode		:	m_pstShMem->notch_find_mode		= UINT8_MAX;	break;
	case ENG_PSCC::psc__notchmaxderval		:	m_pstShMem->notch_max_der_val	= UINT8_MAX;	break;
	case ENG_PSCC::psc__notchminderval		:	m_pstShMem->notch_min_der_val	= UINT8_MAX;	break;
	case ENG_PSCC::psc__notchminderwidth	:	m_pstShMem->notch_min_der_width	= UINT8_MAX;	break;
	case ENG_PSCC::psc__notchnavg			:	m_pstShMem->notch_num_average	= UINT8_MAX;	break;
	}
}
VOID CMPAThread::ResetMiscCmd(ENG_PSCC code)
{
	switch (code)
	{
	case ENG_PSCC::psc__dt	:	;	break;
	case ENG_PSCC::psc_hsa	:	;	break;
	case ENG_PSCC::psc_wsa	:	;	break;
	case ENG_PSCC::psc_res	:	;	break;
	}
}

/*
 desc : 3�� ���� �� ��ȯ �� ����
 parm : host_val- [in]  ����� Host�� Double/UINT32/UINT16/UINT8/INT8 �迭 ���� ������
		vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMPAThread::SetAxisValues(DOUBLE *host_val, vector <string> &vals)
{
	UINT32 i = 0, u32Count = (UINT32)vals.size();
	if (u32Count > MAX_ALIGNER_DRIVE_COUNT)	u32Count = MAX_ALIGNER_DRIVE_COUNT;	/* �ϴ�, K �� ���� �ʿ� ����, ���ڿ��� ��ȯ�Ǳ� ������ ... */
	for (; i<u32Count; i++)	host_val[i] = GetAxisPosApp(ENG_PANC(i), atoi(vals[i].c_str()));
}

/*
 desc : MRWI �� ��û �� ����
 parm : host_val- [in]  ����� Host�� Double/UINT32/UINT16/UINT8/INT8 �迭 ���� ������
		vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMPAThread::SetAxisMRWI(DOUBLE *host_val, vector <string> &vals)
{
	UINT32 i = 0, u32Count = (UINT32)vals.size();
	if (u32Count > MAX_ALIGNER_DRIVE_COUNT)	u32Count = MAX_ALIGNER_DRIVE_COUNT;	/* �ϴ�, K �� ���� �ʿ� ����, ���ڿ��� ��ȯ�Ǳ� ������ ... */
	for (; i<u32Count; i++)	host_val[i] = GetWaferCentDirect(UINT8(i), atoi(vals[i].c_str()));
}

/*
 desc : T / Z ���� �� ��ȯ �� ����
 parm : host_val- [in]  ����� Host�� Double/UINT32/UINT16/UINT8/INT8 �迭 ���� ������
		vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMPAThread::SetAxisZValue(DOUBLE &host_val, vector <string> &vals)
{
	host_val = GetAxisPosApp(ENG_PANC(2), atoi(vals[0].c_str()));
}
VOID CMPAThread::SetAxisTValue(DOUBLE &host_val, vector <string> &vals)
{
	host_val = GetAxisPosApp(ENG_PANC(0), atoi(vals[0].c_str()));
}

/*
 desc : ���ŵ� �߿��� �ؽ�Ʈ ������ Host ���� �ӽ� ���ڿ� ���ۿ� ����
 parm : fw_str	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMPAThread::SetTempTextInfo(vector <string> &vals)
{
	memset(m_pstShMem->last_string_data, 0x00, LAST_STRING_DATA_SIZE);
	strcpy_s(m_pstShMem->last_string_data, LAST_STRING_DATA_SIZE, vals[0].c_str());
}

/*
 desc : �ý��� ���� ���� ���� ���� �޽��� ��ȯ
 parm : mesg	- [out] ���� �ڵ忡 ���� �� ������ ����� ��ȯ ����
		size	- [in]  'mesg' ���� ũ��
 retn : FALSE (���� ����), TRUE (���� ����)
 note : "mesg" ���۸� �ּ� 1024��ŭ ������ ��� ��
*/
BOOL CMPAThread::GetSystemStatus(TCHAR *mesg, UINT32 size)
{
	UINT8 i	= 0x00;
	UINT16 u16Bit	= 0x0001, u16Status = m_pstShMem->system_status, u16Mask = 0x000;

	if (u16Status == 0x0000 || u16Status == 0xffff)	return FALSE;

	/* 16 bit �̹Ƿ�, �� 15�� �̵� (Left Shift)�ϸ鼭 bit mask �� Ȯ�� */
	for (; i<16; i++)
	{
		u16Mask	= u16Status & (u16Bit << i);
		switch (u16Mask)
		{
		case 0x0001	: wcscat_s(mesg, size, L"Unable to execute motion commands");	break;
		case 0x0002	: wcscat_s(mesg, size, L"Previous command returned an error");	break;
		case 0x0004	: wcscat_s(mesg, size, L"Chuck Vacuum Sensor ON");				break;
		case 0x0008	: wcscat_s(mesg, size, L"Chuck Vacuum Switch ON");				break;
		case 0x0010	: wcscat_s(mesg, size, L"Motion or position error");			break;
		case 0x0020	: wcscat_s(mesg, size, L"Software Motion limits violation");	break;
		case 0x0040	: wcscat_s(mesg, size, L"Home procedure not executed");			break;
		case 0x0080	: wcscat_s(mesg, size, L"User macro running");					break;
		case 0x0100	: wcscat_s(mesg, size, L"Motion in progress");					break;
		case 0x0200	: wcscat_s(mesg, size, L"Pins vacuum sensor on");				break;
		case 0x0400	: wcscat_s(mesg, size, L"Servo OFF on one or more axes");		break;
		case 0x0800	: wcscat_s(mesg, size, L"Reserved");							break;
		case 0x1000	: wcscat_s(mesg, size, L"Reserved");							break;
		case 0x2000	: wcscat_s(mesg, size, L"Reserved");							break;
		case 0x4000	: wcscat_s(mesg, size, L"Reserved");							break;
		case 0x8000	: wcscat_s(mesg, size, L"Reserved");							break;
		}

		if (u16Mask)	wcscat_s(mesg, size, L"\n");
	}

	return TRUE;
}

/*
 desc : ������ �۽ŵ� ��ɾ��� ���� �� (�ڵ�)�� ���� �� ���ڿ� ���� ��ȯ
 parm : mesg	- [out] ���� �ڵ忡 ���� �� ������ ����� ��ȯ ����
		size	- [in]  'mesg' ���� ũ��
 retn : FALSE (���� ����), TRUE (���� ����)
*/
BOOL CMPAThread::GetPreviousError(TCHAR *mesg, UINT32 size)
{
	if (0x0000 == m_pstShMem->previous_cmd_fail ||
		0xffff == m_pstShMem->previous_cmd_fail)	return FALSE;
	if (size < 1)	return FALSE;

	/* ���� �ʱ�ȭ */
	wmemset(mesg, 0x00, size);

	switch (m_pstShMem->previous_cmd_fail)
	{
	case 0x0001	: wcscpy_s(mesg, size, L"Wrong wafer size");													break;
	case 0x0004	: wcscpy_s(mesg, size, L"Vacuum not sensed on chuck");											break;
	case 0x0005	: wcscpy_s(mesg, size, L"Data Capture error");													break;
	case 0x0008	: wcscpy_s(mesg, size, L"Data not available");													break;
	case 0x0009	: wcscpy_s(mesg, size, L"Wafer lost during align operation");									break;
	case 0x0010	: wcscpy_s(mesg, size, L"Alignment algorithm interrupted by host");								break;
	case 0x001F	: wcscpy_s(mesg, size, L"Alignment algorithm failed: Data not received on time");				break;
	case 0x0020	: wcscpy_s(mesg, size, L"R-axis home switch position error");									break;
	case 0x0021	: wcscpy_s(mesg, size, L"Z-axis home switch position error");									break;
	case 0x002F	: wcscpy_s(mesg, size, L"Alignment algorithm failed: Index not received on time");				break;
	case 0x004F	: wcscpy_s(mesg, size, L"Alignment algorithm failed: Measurement speed not reached on time");	break;
	case 0x008F	: wcscpy_s(mesg, size, L"Alignment algorithm failed: Synchronization error");					break;
	case 0x010F	: wcscpy_s(mesg, size, L"Alignment algorithm failed: Notch/Flat error");						break;
	case 0x020F	: wcscpy_s(mesg, size, L"Alignment algorithm failed: Notch/Flat not found");					break;
	case 0x040F	: wcscpy_s(mesg, size, L"Alignment algorithm failed: Calculated offset out of limits");			break;
	case 0x080F	: wcscpy_s(mesg, size, L"Alignment algorithm failed: Invalid CCD sensor data");					break;
	case 0x100F	: wcscpy_s(mesg, size, L"Alignment algorithm failed: Invalid parameters");						break;
	case 0x200F	: wcscpy_s(mesg, size, L"Alignment algorithm failed: Cannot align within required offset");		break;
	case 0x400F	: wcscpy_s(mesg, size, L"Alignment algorithm failed: Cannot align within required notch angle");break;
	default		: return FALSE;
	}

	return TRUE;
}

/*
 desc : ���� Teaching Mode �߿� ��� ������ ��ɾ����� ����
 parm : code	- [in]  �۽� ��ɾ� ������
 retn : ��ȿ�� ��ɾ� (TRUE), ��ȿ���� ���� ��ɾ� (FALSE)
*/
BOOL CMPAThread::IsValidTechingSet(ENG_PSCC code)
{

	return TRUE;
}