
/*
 desc : �ø��� ��� ������ (for Diamond Robot)
*/

#include "pch.h"
#include "MDRThread.h"


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
CMDRThread::CMDRThread(UINT8 port, UINT32 baud, UINT32 buff, LPG_MRDV shmem)
	: CMilaraThread(ENG_EDIC::en_lsdr, port, baud, buff)
{
	/* ���� �޸� ���� */
	m_pstShMem		= shmem;
	m_enLastCmd		= ENG_RSCC::rsc_inf;	/* !!! Important !!! */
	/* ���� �����Ǹ� �ݵ�� ���� */
	m_u8ReplySkip	= 0x00;	/* !!! Important !!! */
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CMDRThread::~CMDRThread()
{
}

/*
 desc : �ø��� ��� ��Ʈ ���� �����ϸ�, Prealigner���� �ʱ�ȭ ����� ������� ������
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMDRThread::Initialized()
{
	switch (m_u8InitStep)
	{
	case 0x00	:	/* ������ Info Mode �� 1�� ���� */
		m_enLastCmd	= ENG_RSCC::rsc_none;
		if (!CMilaraThread::SendData("INF 1"))	return FALSE;
		LOG_MESG(ENG_EDIC::en_lsdr, L"Succeeded in sending INF_1 command");
		m_u8InitStep++;
		break;
	case 0x01	:	/* Info Mode ������ ���� ���� ���� ���Դ��� ���� */
		if (ENG_MSAS::none == m_enLastSendAck)	return TRUE;
		m_u8InitStep++;
		break;
	case 0x02	:	/* ������ Info Mode �� ��û */
		m_pstShMem->info_mode	= 0xff;
		m_enLastCmd	= ENG_RSCC::rsc_inf;
		if (!CMilaraThread::SendData("INF"))	return FALSE;
		LOG_MESG(ENG_EDIC::en_lsdr, L"Succeeded in sending INF command");
		m_u8InitStep++;
		break;
	case 0x03	:	/* Info Mode ���� 1�� ���ŵǾ����� Ȯ�� */
		if (m_pstShMem->info_mode != 0x01)	return TRUE;
		m_u8InitStep++;
		break;
	case 0x04	:	/* Station ���� ���� ���� */
		memset(m_pstShMem->last_string_data, 0x00, LAST_STRING_DATA_SIZE);
		m_enLastCmd	= ENG_RSCC::rsc_ens;
		if (!CMilaraThread::SendData("ENS"))	return FALSE;
		LOG_MESG(ENG_EDIC::en_lsdr, L"Succeeded in sending ENS command");
		m_u8InitStep++;
		break;
	case 0x05	:	/* Station ���� ���� ���� Ȯ�� */
		if (strlen(m_pstShMem->last_string_data) < 1)	return TRUE;
		m_bIsInited	= TRUE;
		LOG_MESG(ENG_EDIC::en_lsdr, L"Succeeded in initialed the Robot");
		break;
	}

	return TRUE;
}

/*
 desc : ��� �۽� �� ���� �ޱ�
 parm : code	- [in]  Send ����   ��ɾ� ���� �ڵ� ��
		cmd		- [in]  Send ���ڿ� ��ɾ ����� ����
		ack		- [in]  Send ��ɿ� ���� Ack ���� �÷��� (Ack �Ǵ� ���, ���� ������ Reset ������)
 retn : Null or ���ŵ� �����Ͱ� ����� ���� ���� ������
*/
BOOL CMDRThread::SendData(ENG_RSCC code, PCHAR cmd, BOOL ack)
{
	CHAR szMesg[128]	= {NULL};
	CUniToChar csCnv;

	if (!m_bIsInited)
	{
		LOG_WARN(ENG_EDIC::en_lsdr, L"Milara communication (Robot) is not initialized.");
		return FALSE;
	}

	if (m_pstShMem->info_mode != 1 && ENG_RSCC::rsc_inf != code)
	{
		LOG_WARN(ENG_EDIC::en_lsdr, L"Send: The <INF 0/2/3/4/5> mode is not supported");
#ifdef _DEBUG
		AfxMessageBox(L"Send: The <INF 0/2/3/4/5> mode is not supported", MB_ICONSTOP|MB_TOPMOST);
#endif
		return FALSE;
	}

	/* ��� �۽� ��ɾ �α׿� ���� */
	sprintf_s(szMesg, 128,
			  "The sent Robot command has been saved [cmd_code : %02x] [cmd_str : %s] [ACK : %d]",
			  code, cmd, ack);
	LOG_MESG(ENG_EDIC::en_lsdr, csCnv.Ansi2Uni(szMesg));

	/* ���� Teaching Mode�� ���, Teching ���� �̿��� ����� �����ϵ��� ó�� */
	if (m_bIsTechMode)
	{
		if (!IsValidTechingSet(code))
		{
		LOG_WARN(ENG_EDIC::en_lsdr, L"Send: Currently, only commands related to Teching Mode are valid.");
#ifdef _DEBUG
		AfxMessageBox(L"Send: Currently, only commands related to Teching Mode are valid.", MB_ICONSTOP|MB_TOPMOST);
#endif
		return FALSE;
		}
	}

	/* ���� �ֱٿ� �۽��� ��ɾ� ���� �ڵ� �� ���� */
	m_enLastCmd	= code;

	/* ���� ���ŵ� ������ �ʱ�ȭ */
	if (ack)	ResetRecvData(code);
	/* Motion reply �ʱ�ȭ */
 	switch (code)
 	{
 		case ENG_RSCC::rsc__hm	:	/* Start homing the robot */
 		case ENG_RSCC::rsc_hom	:	/* Start homing the robot */
 		case ENG_RSCC::rsc_mvr	:	/* Move an axis to relative position */
 		case ENG_RSCC::rsc_mva	:	/* Move an axis to absolute position */
		case ENG_RSCC::rsc_get	:	/* Get (pick) a wafer from a station (by name), with end option */
		case ENG_RSCC::rsc_getn	:	/* Get (pick) a wafer from a station (by name), with end option */
		case ENG_RSCC::rsc_getr	:	/* Get (pick) a wafer from a station (by name), with retry option */
		case ENG_RSCC::rsc_getw	:	/* Get (pick) a wafer from a station, with wait option at pick-up position */
		case ENG_RSCC::rsc_put	:	/* Put (place) a wafer into a station (by name) */
		case ENG_RSCC::rsc_putn	:	/* Put (place) a wafer into a station (by number) */
		case ENG_RSCC::rsc_putw	:	/* Put (place) a wafer into a station, with wait option at drop-down */
		case ENG_RSCC::rsc_mss	:	/* Move a wafer from station to station (by name) */
		case ENG_RSCC::rsc_mta	:	/* Move to Station' Approach position */
		case ENG_RSCC::rsc_mtr	:	/* Move to Station' Retracted position */
		case ENG_RSCC::rsc_mts	:	/* Move to Station Coordinate */
			m_pstShMem->system_status = 0xffff;	/* �ý��� ���°� ��ȿ���� �ʴٰ� ���� */
			/* !!! important !!! */
			m_enLastCmd	= ENG_RSCC::rsc_sta;
			break;
 	}

	/* ��ɾ� ���� */
	return CMilaraThread::SendData(cmd);
}

/*
 desc : ���� ������ ó��
 parm : None
 retn : None
*/
VOID CMDRThread::RecvData()
{
	UINT32 u32Count	= 0;
	vector <string> vVals;

	/* �ϴ�. INF 0, 2, 3, 4, 5 ���� �������� �ʴ´�. (�� ����) */
	if (m_pstShMem->info_mode != 1 && ENG_RSCC::rsc_inf != m_enLastCmd)
	{
		memset(m_pstShMem->last_string_data, 0x00, LAST_STRING_DATA_SIZE);
		strcpy_s(m_pstShMem->last_string_data, MAX_CMD_MILARA_RECV, (PCHAR)m_pCmdRecv);
		LOG_WARN(ENG_EDIC::en_lsdr, L"Recv: The <INF 0, 2, 3, 4, 5> mode is not supported");
		return;
	}

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
		swprintf_s(tzMesg, 256, L"PODIS vs. Robot [Recv] [called_func=RecvData] : %s",
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
	/* ���� ���ŵ� ������ ���� */
	u32Count = (UINT32)vVals.size();
	/* �ܼ� ���� ����� ��������� ���� */
	if (u32Count < 1)
	{
		switch (m_enLastCmd)
		{
		case ENG_RSCC::rsc_hom	:
		case ENG_RSCC::rsc_mov	:
		case ENG_RSCC::rsc_mth	:
		case ENG_RSCC::rsc_mtb	:
		case ENG_RSCC::rsc_mva	:
		case ENG_RSCC::rsc_mvr	:
		case ENG_RSCC::rsc_mvt1	:
		case ENG_RSCC::rsc_mvt2	:
		case ENG_RSCC::rsc_mvtp	:
		case ENG_RSCC::rsc_rdw	:
		case ENG_RSCC::rsc_rup	:
		case ENG_RSCC::rsc_stp	:
		case ENG_RSCC::rsc_wmc	: m_pstShMem->motion_wait_finished	= 0x01;				break;	/* ������ �� ��ɿ� ���� ������ ���ŵǸ� 1 ������ ���� */
		/* Teaching Mode : Start or Stop (End) */
		case ENG_RSCC::rsc_tch	: 
		case ENG_RSCC::rsc_eot	: m_bIsTechMode = (m_enLastSendAck == ENG_MSAS::succ);	break;
		}
	}
	/* ���� �ֱٿ� �۽��� ��ɾ �Ʒ��� ���� ���, ������ ������ status ���� ���ŵǹǷ�... T.T */
	else if (u32Count == 1)
	{
		switch (m_enLastCmd)
		{
		case ENG_RSCC::rsc_get	:
		case ENG_RSCC::rsc_put	:
		case ENG_RSCC::rsc_hom	:
		case ENG_RSCC::rsc_mss	:
		case ENG_RSCC::rsc_mtr	:
		case ENG_RSCC::rsc_mva	:
		case ENG_RSCC::rsc_mvr	:
		case ENG_RSCC::rsc_mth	:
		case ENG_RSCC::rsc_mts	:	m_enLastCmd = ENG_RSCC::rsc_sta;
		}
	}

	/* ������ �۽��� ��ɾ� ����, ���� ������ ó�� */
	if (m_enLastCmd <= ENG_RSCC::rsc_ver)			RecvDataInfoCmds(vVals);		/* 1. Informational commands */
	else if (m_enLastCmd <= ENG_RSCC::rsc_ssp)		RecvDataHouseKeeping(vVals);	/* 2. HouseKeeping commands */
	else if (m_enLastCmd <= ENG_RSCC::rsc_sta)		RecvDataStatusDiagCmds(vVals);	/* 3. Status and diagnostic commands */
	else if (m_enLastCmd <= ENG_RSCC::rsc__ralmc)	RecvDataAbsEncCmds(vVals);		/* 4. Support for systems with absolute encoders */
	else if (m_enLastCmd <= ENG_RSCC::rsc_vst_b)	RecvDataBasicActCmds(vVals);	/* 5. Basic (non-axes, non-station) action commands */
	else if (m_enLastCmd <= ENG_RSCC::rsc_spd)		RecvDataBasicMotionParam(vVals);/* 6. Basic axis motion parameters */
	else if (m_enLastCmd <= ENG_RSCC::rsc_wsa)		RecvDataAxisParmSetCmds(vVals);	/* 7. Axis parameter sets, and custom parameters commands */
	else if (m_enLastCmd <= ENG_RSCC::rsc_wmc)		RecvDataBasicMotionCmds(vVals);	/* 8. Basic axis motions commands */
	else if (m_enLastCmd <= ENG_RSCC::rsc_vto)		RecvDataStaParamCmds(vVals);	/* 9. Station parameters commands */
	else if (m_enLastCmd <= ENG_RSCC::rsc_spg)		RecvDataStaMotionCmds(vVals);	/* 10.Station motion commands */
	else if (m_enLastCmd <= ENG_RSCC::rsc_tsp)		RecvDataStaScanCmds(vVals);		/* 11.Station scanning parameters and motion commands */
	else if (m_enLastCmd <= ENG_RSCC::rsc_rwic)		RecvDataOtFCmds(vVals);			/* 12.Align-on-Flight (On-The-Flight, OtF) parameters and commands */
	/* ��� ���� ���� */
	vVals.clear();
}

/*
 desc : ���� ������ ó�� - 1. Information Commands
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMDRThread::RecvDataInfoCmds(vector <string> &vals)
{
	switch (m_enLastCmd)
	{
	case ENG_RSCC::rsc__dt	:									/* Set system date and time */
	case ENG_RSCC::rsc__si	:									/* Report System Configuration Information */
	case ENG_RSCC::rsc_ver	: SetTempTextInfo(vals);	break;	/* Read Firmware Version Number */
	case ENG_RSCC::rsc_snr	: SetSerialNum(vals);		break;	/* Report Robot Serial Number */
	}
}

/*
 desc : ���� ������ ó�� - 2. HouseKeeping commands
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMDRThread::RecvDataHouseKeeping(vector <string> &vals)
{
}

/*
 desc : ���� ������ ó�� - 3. Status and diagnostic commands
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMDRThread::RecvDataStatusDiagCmds(vector <string> &vals)
{
	switch (m_enLastCmd)
	{
	case ENG_RSCC::rsc___cs	:																	/* Report current robot state (�� ���� ���, ���ڿ��� ...) */
	case ENG_RSCC::rsc_diag	:																	/* Report diagnostic information (�� ���� ���, ���ڿ��� ...) */
	case ENG_RSCC::rsc_per	:																	/* Report Previous Error Reason (robot) */
	case ENG_RSCC::rsc_est	:																	/* Extended Error Status */
	case ENG_RSCC::rsc_esta	: SetTempTextInfo(vals);									break;	/* Report Extended System Status */
	case ENG_RSCC::rsc_eerr	: SetValue(m_pstShMem->extended_error_code, vals);			break;	/* Report Extended Error Number */
	case ENG_RSCC::rsc_cpo	: SetAxisValues(m_pstShMem->axis_current_position, vals);	break;	/* Report Current Position for an Axis */
	case ENG_RSCC::rsc_inf	: SetValue(m_pstShMem->info_mode, vals);					break;	/* Set/Report inform-after-motion mode */
	case ENG_RSCC::rsc_rhs	: SetValue(m_pstShMem->home_switch_state, vals);			break;	/* Read Home Switches State */
	case ENG_RSCC::rsc_sta	: SetValue(m_pstShMem->system_status, vals);				break;	/* Command Execution Status */
	}
}

/*
 desc : ���� ������ ó�� - 4. Support for systems with absolute encoders
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMDRThread::RecvDataAbsEncCmds(vector <string> &vals)
{
	switch (m_enLastCmd)
	{
	case ENG_RSCC::rsc__aest	: SetValues(m_pstShMem->enc_status, vals);			break;	/* Report absolute encoder status */
	case ENG_RSCC::rsc__eo		: 															/* Set/Report encoder offset (zero position) for Single Axis */
	case ENG_RSCC::rsc__eor		: SetAxisValues(m_pstShMem->enc_zero_offset, vals);	break;	/* Set/Report encoder offset (zero position) for All Axes */
	case ENG_RSCC::rsc__eo_t	: SetValue(m_pstShMem->enc_zero_offset[0], vals);	break;	/* Set/Report encoder offset (zero position) for T Axis */
	case ENG_RSCC::rsc__eo_r1	: SetValue(m_pstShMem->enc_zero_offset[1], vals);	break;	/* Set/Report encoder offset (zero position) for R1 Axis */
	case ENG_RSCC::rsc__eo_r2	: SetValue(m_pstShMem->enc_zero_offset[2], vals);	break;	/* Set/Report encoder offset (zero position) for R2 Axis */
	case ENG_RSCC::rsc__eo_z	: SetValue(m_pstShMem->enc_zero_offset[3], vals);	break;	/* Set/Report encoder offset (zero position) for Z Axis */
	case ENG_RSCC::rsc__ralmc	: SetValue(m_pstShMem->low_level_enc_status, vals);	break;	/* Report low-level encoder status for an Axis */
	}
}

/*
 desc : ���� ������ ó�� - 5. Basic (non-axes, non-station) action commands
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMDRThread::RecvDataBasicActCmds(vector <string> &vals)
{
	switch (m_enLastCmd)
	{
	case ENG_RSCC::rsc__bb	: SetValues(m_pstShMem->backup_dc_power, vals);				break;	/* Turn Backup DC Power On or Off */
	case ENG_RSCC::rsc_cstp	: SetValues(m_pstShMem->controlled_stop, vals);				break;	/* Enable/Disable QuickStop feature */
	case ENG_RSCC::rsc_din	: SetValueHex(m_pstShMem->external_di_in_state, vals);		break;	/* Read State of External Digital Input */
	case ENG_RSCC::rsc_dout	: SetValueHex(m_pstShMem->external_di_out_state, vals);		break;	/* Set/Report State of External Digital Output */
	case ENG_RSCC::rsc_edg	:															break;	/* Get status or operate Edge Gripper */
	case ENG_RSCC::rsc_inp	: SetValueHex(m_pstShMem->digital_input_state, vals);		break;	/* Report the state of an Input */
	case ENG_RSCC::rsc_lenb	: SetValue(m_pstShMem->loaded_speed_enable, vals);			break;	/* Enable/Disable Loaded Speed Parameters */
	case ENG_RSCC::rsc_out	: SetValueHex(m_pstShMem->digital_output_state, vals);		break;	/* Report the state of an Output */
	case ENG_RSCC::rsc_vst_t: SetValue(m_pstShMem->end_effector_wafer_check[0], vals);	break;	/* Check for wafer presence on a paddle */
	case ENG_RSCC::rsc_vst_b: SetValue(m_pstShMem->end_effector_wafer_check[1], vals);	break;	/* Check for wafer presence on a paddle */
	}
}

/*
 desc : ���� ������ ó�� - 6. Basic axis motion parameters
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMDRThread::RecvDataBasicMotionParam(vector <string> &vals)
{
	switch (m_enLastCmd)
	{
	case ENG_RSCC::rsc__ch	: SetAxisValues(m_pstShMem->homed_position, vals);					break;	/* Set/Report customized home position */
	case ENG_RSCC::rsc__cl	: SetValues(m_pstShMem->current_limit, vals);						break;	/* Set/Report axis current (����:Ampere) limit */
	case ENG_RSCC::rsc__el	: SetValues(m_pstShMem->position_error_limit, vals);				break;	/* Set/Report axis position error limit */
	case ENG_RSCC::rsc__fl	: SetAxisValues(m_pstShMem->sw_positive_limit, vals);				break;	/* Set/Report Positive Direction Software Limit */
	case ENG_RSCC::rsc__fse	: SetAxisValues(m_pstShMem->axis_safe_envelop_forward, vals);		break;	/* Set/Report Axis Forward Safe Envelop */
	case ENG_RSCC::rsc__hd	: SetAxisValues(m_pstShMem->axis_diff_coeff_hold, vals);			break;	/* Set/Report Axis Forward Safe Envelop */
	case ENG_RSCC::rsc__he	: SetAxisValues(m_pstShMem->pos_err_limit_hold, vals);				break;	/* Set/Report Axis Position Error Limit during Axis Hold */
	case ENG_RSCC::rsc__hi	: SetAxisValues(m_pstShMem->axis_inte_coeff_hold, vals);			break;	/* Set/Report Loop Integral Coefficient for Axis Hold */
	case ENG_RSCC::rsc__hl	: SetAxisValues(m_pstShMem->axis_inte_limit_hold, vals);			break;	/* Set/Report Loop Integral Limit for Axis Hold */
	case ENG_RSCC::rsc__ho	:																	break;	/* Set/Report axis home offset */
	case ENG_RSCC::rsc__hp	: SetAxisValues(m_pstShMem->axis_prop_coeff_hold, vals);			break;	/* Set/Report Loop Proportational Coefficient for Axis Hold */
	case ENG_RSCC::rsc__il	: SetAxisValues(m_pstShMem->axis_integral_limit, vals);				break;	/* Set/Report Loop Integral Limit */
	case ENG_RSCC::rsc__kd	: SetAxisValues(m_pstShMem->axis_diff_coeff, vals);					break;	/* Set/Report Loop Diffierential Coefficient */
	case ENG_RSCC::rsc__ki	: SetAxisValues(m_pstShMem->axis_inte_coeff, vals);					break;	/* Set/Report Loop Integral Coefficient */
	case ENG_RSCC::rsc__kp	: SetAxisValues(m_pstShMem->axis_prop_coeff, vals);					break;	/* Set/Report Loop Proportational Coefficient */
	case ENG_RSCC::rsc__rl	: SetAxisValues(m_pstShMem->sw_negative_limit, vals);				break;	/* Set/Report Loop Integral Limit for Axis Hold */
	case ENG_RSCC::rsc__rse	: SetAxisValues(m_pstShMem->axis_safe_envelop_reverse, vals);		break;	/* Set/Report Axis Reverse Safe Envelop */
	case ENG_RSCC::rsc_acl	: SetAxisValues(m_pstShMem->axis_acceleration, vals);				break;	/* Set/Report Axis Acceleration */
	case ENG_RSCC::rsc_ajk	: SetAxisValues(m_pstShMem->axis_acceleration_jerk, vals);			break;	/* Set/Report Axis Acceleration Jerk */
	case ENG_RSCC::rsc_dcl	: SetAxisValues(m_pstShMem->axis_deceleration, vals);				break;	/* Set/Report Axis Acceleration */
	case ENG_RSCC::rsc_djk	: SetAxisValues(m_pstShMem->axis_deceleration_jerk, vals);			break;	/* Set/Report Axis Deceleration Jerk */
	case ENG_RSCC::rsc_hfp	: SetAxisValues(m_pstShMem->home_flip_position, vals);				break;	/* Set/Report axis safe flip position */
	case ENG_RSCC::rsc_lacl	: SetAxisValues(m_pstShMem->axis_loaded_acceleration, vals);		break;	/* Set/Report Axis Acceleration */
	case ENG_RSCC::rsc_lajk	: SetAxisValues(m_pstShMem->axis_loaded_acceleration_jerk, vals);	break;	/* Set/Report Axis Acceleration Jerk */
	case ENG_RSCC::rsc_ldcl	: SetAxisValues(m_pstShMem->axis_loaded_deceleration, vals);		break;	/* Set/Report Axis Acceleration */
	case ENG_RSCC::rsc_ldjk	: SetAxisValues(m_pstShMem->axis_loaded_deceleration_jerk, vals);	break;	/* Set/Report Axis Deceleration Jerk */
	case ENG_RSCC::rsc_lspd	: SetAxisValues(m_pstShMem->axis_loaded_speed, vals);				break;	/* Set/Report Loaded Speed */
	case ENG_RSCC::rsc_spd	: SetAxisValues(m_pstShMem->axis_working_speed, vals);				break;	/* Set/Report Working Speed */
	}
}

/*
 desc : ���� ������ ó�� - 7. Axis parameter sets, and custom parameters commands
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMDRThread::RecvDataAxisParmSetCmds(vector <string> &vals)
{
	switch (m_enLastCmd)
	{
	case ENG_RSCC::rsc__cax	: SetValue(m_pstShMem->custom_axis_param, vals);	break;	/* Custom Axis Parameter */
	case ENG_RSCC::rsc__cpa	: SetValue(m_pstShMem->custom_generic_param, vals);	break;	/* Custom Generic Parameter */
	case ENG_RSCC::rsc__cst	: SetValue(m_pstShMem->custom_station_param, vals);	break;	/* Custom Station Parameter */
	case ENG_RSCC::rsc_css	:													break;	/* Copy one parameter set into another */
	case ENG_RSCC::rsc_csw	:													break;	/* Copy parameter set to working values */
	case ENG_RSCC::rsc_cwp	:													break;	/* Copy PID parameters to all parameter sets */
	case ENG_RSCC::rsc_cws	:													break;	/* Copy working values as a parameter set */
	case ENG_RSCC::rsc_das	:															/* Report All Parameter Sets for a Single Axis */
	case ENG_RSCC::rsc_dps	: SetTempTextInfo(vals);							break;	/* Report a Parameter Set */
	case ENG_RSCC::rsc_hsa	:													break;	/* Set homing speed and acceleration */
	case ENG_RSCC::rsc_lsa	:													break;	/* Set loaded speed and acceleration */
	case ENG_RSCC::rsc_wsa	:													break;	/* Set working speed and acceleration */
	}
}

/*
 desc : ���� ������ ó�� - 8. Basic axis motions commands
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMDRThread::RecvDataBasicMotionCmds(vector <string> &vals)
{
	switch (m_enLastCmd)
	{
	case ENG_RSCC::rsc__afe	:															/* Find limit-switch to absolute encoder zero distance */
	case ENG_RSCC::rsc_afe	:															/* Find limit-switch to absolute encoder zero distance */
	case ENG_RSCC::rsc__fe	: SetTempTextInfo(vals);							break;	/* Find Edge (Report Home Switch to Index distance) */
	case ENG_RSCC::rsc__fh	:													break;	/* Find Home Switch */
	case ENG_RSCC::rsc__hm	:													break;	/* Home an Axis */
	case ENG_RSCC::rsc_hom	:													break;	/* Start homing the robot */
	case ENG_RSCC::rsc_mov	:													break;	/* Move relative over vector */
	case ENG_RSCC::rsc_mth	:													break;	/* Move all axes to home position */
	case ENG_RSCC::rsc_mtb	:													break;	/* Move an axis until sensor is triggered */
	case ENG_RSCC::rsc_mtp	:													break;	/* Move three axes to position */
	case ENG_RSCC::rsc_mva	:													break;	/* Move an axis to absolute position */
	case ENG_RSCC::rsc_mvr	:													break;	/* Move an axis to relative position */
	case ENG_RSCC::rsc_mvt1	:													break;	/* Move over line */
	case ENG_RSCC::rsc_mvt2	:													break;	/* Move over two lines */
	case ENG_RSCC::rsc_mvtp	:													break;	/* Set/Report End Effector Length for station-less motions */
	case ENG_RSCC::rsc_rdw	:													break;	/* Rotate end-effector down */
	case ENG_RSCC::rsc_rup	:													break;	/* Rotate end-effector up */
	case ENG_RSCC::rsc_stp	:													break;	/* Stop motion */
	case ENG_RSCC::rsc_wmc	:													break;	/* Wait for motion completed */
	}
}

/*
 desc : ���� ������ ó�� - 9. Station parameters commands
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMDRThread::RecvDataStaParamCmds(vector <string> &vals)
{
	switch (m_enLastCmd)
	{
	case ENG_RSCC::rsc__css		: SetValue(m_pstShMem->station_set_no, vals);						break;	/* Set/Report current station set number */
	case ENG_RSCC::rsc__pa		: SetInch01Value(m_pstShMem->r_axis_push_acc_sta, vals);			break;	/* Set/Report R Axis Pushing Acceleration (Last Station) */
	case ENG_RSCC::rsc__pa_def	: SetInch01Value(m_pstShMem->r_axis_push_acc_def, vals);			break;	/* Set/Report R Axis Pushing Acceleration (Default) */
	case ENG_RSCC::rsc__ps		: SetInch01Value(m_pstShMem->r_axis_push_spd_sta, vals);			break;	/* Set/Report R Axis Pushing Speed (Last Station) */
	case ENG_RSCC::rsc__ps_def	: SetInch01Value(m_pstShMem->r_axis_push_spd_def, vals);			break;	/* Set/Report R Axis Pushing Speed (Default) */
	case ENG_RSCC::rsc__zc		: SetAxisValues(m_pstShMem->coord_compensation_slot, vals);			break;	/* Set/Report Station Coordinate Compensation with Slot */
	case ENG_RSCC::rsc_app		: 																			/* Find Edge (Report Home Switch to Index distance) */
	case ENG_RSCC::rsc_ens		: SetTempTextInfo(vals);											break;	/* Enumerate Defined Stations */
	case ENG_RSCC::rsc_cof		: SetAxisZValue(m_pstShMem->station_center_offset, vals);			break;	/* Set/Report Station Center Offset for R Axis */
	case ENG_RSCC::rsc_cofp		: SetAxisZValue(m_pstShMem->station_center_offset_edge_grip, vals);	break;	/* Set/Report Station Center Offset for R Axis */
	case ENG_RSCC::rsc_csl		: SetValue(m_pstShMem->current_slot_no, vals);						break;	/* Set/Report Current Slot for a Station */
	case ENG_RSCC::rsc_csp		:																	break;	/* Clear single or all station parameters */
	case ENG_RSCC::rsc_cst		: SetValue(m_pstShMem->station_name, vals);							break;	/* Set/Report Current Station, by name */
	case ENG_RSCC::rsc_cstn		: SetValue(m_pstShMem->station_no, vals);							break;	/* Set/Report Current Station, by number (1 ~ 130) */
	case ENG_RSCC::rsc_dsp		:																	break;	/* Duplicate Station Parameters, by name */
	case ENG_RSCC::rsc_dspn		:																	break;	/* Duplicate Station Parameters, by number */
	case ENG_RSCC::rsc_een		: SetValue(m_pstShMem->end_effector_num, vals);						break;	/* Set/Report End Effector Number in Use for a Station */
	case ENG_RSCC::rsc_eeo		: SetValue(m_pstShMem->end_effector_up_down_station, vals);			break;	/* Set/Report Rotating End Effector Orientation for a Station */
	case ENG_RSCC::rsc_eot		:																	break;	/* End of teaching a station */
	case ENG_RSCC::rsc_ils		: SetValue(m_pstShMem->station_last_seg_speed_factor, vals);		break;	/* Set/Report inline station last segment speed factor */
	case ENG_RSCC::rsc_irr		: SetAxisZValue(m_pstShMem->station_retracted_r_position, vals);	break;	/* Set/Report Inline Station Retracted R Position */
	case ENG_RSCC::rsc_irt		: SetAxisTValue(m_pstShMem->station_retracted_t_position, vals);	break;	/* Set/Report Inline Station Retracted T Position */
	case ENG_RSCC::rsc_isa		: SetAxisTValue(m_pstShMem->station_exit_angle, vals);				break;	/* Set/Report Inline Station Exit Angle */
	case ENG_RSCC::rsc_isd		: SetAxisZValue(m_pstShMem->station_exit_distance, vals);			break;	/* Set/Report Inline Station Exit Distance */
	case ENG_RSCC::rsc_ise		: SetAxisZValue(m_pstShMem->station_end_effect_length, vals);		break;	/* Set/Report Inline Station End-Effector Length */
	case ENG_RSCC::rsc_ist		: SetValue(m_pstShMem->inline_station_type, vals);					break;	/* Set/Report Inline Station Type */
	case ENG_RSCC::rsc_nsl		: SetValue(m_pstShMem->station_slot_num, vals);						break;	/* Set/Report Number of Slots for a Station */
	case ENG_RSCC::rsc_nst		: SetValue(m_pstShMem->station_num, vals);							break;	/* Report Number of Stations */
	case ENG_RSCC::rsc_ofs		: SetAxisZValue(m_pstShMem->station_offset_z, vals);				break;	/* Set/Report Station Offset for Z Axis */
	case ENG_RSCC::rsc_pgd		: SetValue(m_pstShMem->put_get_delay_time, vals);					break;	/* Set/Report Station Put and Get Delay */
	case ENG_RSCC::rsc_pgf		: SetValues(m_pstShMem->put_get_failed_option, vals);				break;	/* Set/Report Put/Get Failure Option */
	case ENG_RSCC::rsc_pgo		: SetValues(m_pstShMem->put_get_option, vals);						break;	/* Set/Report Put/Get Option and Sync Move Option */
	case ENG_RSCC::rsc_pit		: SetInch0001Value(m_pstShMem->station_pitch, vals);				break;	/* Set/Report Station Pitch */
	case ENG_RSCC::rsc_psd		: SetValue(m_pstShMem->start_pushing_distance, vals);				break;	/* Set/Report Start Pushing Distance */
	case ENG_RSCC::rsc_psh		: SetValue(m_pstShMem->pusher_type, vals);							break;	/* Set/Report Pusher Type */
	case ENG_RSCC::rsc_pss		: SetAxisZValue(m_pstShMem->station_start_pushing_offset, vals);	break;	/* Set/Report Start Pushing Offset */
	case ENG_RSCC::rsc_psz		: SetAxisZValue(m_pstShMem->station_pushing_offset_z, vals);		break;	/* Set/Report Start Pushing Offset */
	case ENG_RSCC::rsc_pua		: SetAxisZValue(m_pstShMem->station_pickup_acceleration_z, vals);	break;	/* Set/Report Station Pick-Up Acceleration */
	case ENG_RSCC::rsc_pus		: SetAxisZValue(m_pstShMem->station_pickup_speed_z, vals);			break;	/* Set/Report Station Pick-Up Speed */
	case ENG_RSCC::rsc_rpo		: SetAxisZValue(m_pstShMem->station_retracted_position_r, vals);	break;	/* Set/Report Station Retracted Station Position */
	case ENG_RSCC::rsc_spo		: 																			/* Set/Report Station Position (By name) */
	case ENG_RSCC::rsc_spon		: SetAxisValues(m_pstShMem->station_position, vals);				break;	/* Set/Report Station Position (By number) */
	case ENG_RSCC::rsc_str		: SetAxisZValue(m_pstShMem->station_stroke_z, vals);				break;	/* Z Axis Stroke Position */
	case ENG_RSCC::rsc_tbl		: SetValue(m_pstShMem->station_axis_overlap_factor, vals);			break;	/* Set/Report Axes Motion Overlap Factor */
	case ENG_RSCC::rsc_tmd		: SetValue(m_pstShMem->station_trajectory_mode, vals);				break;	/* Enable/Disable Trajectory Mode */
	case ENG_RSCC::rsc_vchk		: SetValues(m_pstShMem->station_vacuum_check, vals);				break;	/* Enable/Disable wafer (vacuum) check before pick/place operations */
	case ENG_RSCC::rsc_vto		: SetValue(m_pstShMem->vacuum_search_time_out, vals);				break;	/* Set/Report Vacuum Search Time Out */
	}
}

/*
 desc : ���� ������ ó�� - 10. Station motion commands
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMDRThread::RecvDataStaMotionCmds(vector <string> &vals)
{
	switch (m_enLastCmd)
	{
	case ENG_RSCC::rsc_get	:															/* MOVING COMPLET rsc_get */
	case ENG_RSCC::rsc_put	:															/* MOVING COMPLET rsc_put */
	case ENG_RSCC::rsc_mss	:															/* MOVING COMPLET rsc_mss */
	case ENG_RSCC::rsc_mtr	:															/* MOVING COMPLET rsc_mtr */
	case ENG_RSCC::rsc_mts	:															break;	/* MOVING COMPLET rsc_mts */		

	case ENG_RSCC::rsc_sbusy	: SetValue(m_pstShMem->station_service_busy, vals);		break;	/* Check whether a station is being serviced at the moment */
	case ENG_RSCC::rsc_slck		: SetValue(m_pstShMem->station_locked_state, vals);		break;	/* Set/Report Station Locked State */
	}
}

/*
 desc : ���� ������ ó�� - 11. Station scanning parameters and motion commands
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMDRThread::RecvDataStaScanCmds(vector <string> &vals)
{
	switch (m_enLastCmd)
	{
	case ENG_RSCC::rsc__bt	: SetInch0001Values(m_pstShMem->beam_threshold, vals);					break;	/* Set /Report Global Beam Thresholds */
	case ENG_RSCC::rsc__mt	: SetInch001Value(m_pstShMem->maximum_wafer_thick, vals);				break;	/* Currently set maximum wafer thickness */
	case ENG_RSCC::rsc__sa	: SetValue(m_pstShMem->wafer_scan_z_axis_acc, vals);					break;	/* Current wafer scanning Z axis acceleration for given station */
	case ENG_RSCC::rsc__sl	:																		break;	/* Set current position as Left Scanning Position */
	case ENG_RSCC::rsc__sr	:																		break;	/* Set current position as Right Scanning Position */
	case ENG_RSCC::rsc__ss	: SetInch01Value(m_pstShMem->wafer_scan_z_axis_spd, vals);				break;	/* Current wafer scanning Z axis speed for given station */
	case ENG_RSCC::rsc__st	: SetValue(m_pstShMem->scanner_type, vals);								break;	/* Current scanner type */
	case ENG_RSCC::rsc_dscn	:																				/* Report Scanned Positions */
	case ENG_RSCC::rsc_tcs	: SetTempTextInfo(vals);												break;	/* None, or error if already in teach station position/teach scanning mode */
	case ENG_RSCC::rsc_map	: SetMapValues(m_pstShMem->wafer_map_loaded_state, vals);				break;	/* Print-out of wafer map from last scanning, if available */
	case ENG_RSCC::rsc_msc	:																		break;	/* Move to Scanning Coordinate */
	case ENG_RSCC::rsc_sbt	: SetInch0001Values(m_pstShMem->station_per_beam_threshold, vals);		break;	/* Set /Report Per-Station Beam Thresholds */
	case ENG_RSCC::rsc_scn	:																		break;	/* Start Scanning procedure */
	case ENG_RSCC::rsc_scp	: SetAxisValues(m_pstShMem->station_scanning_center_coord, vals);		break;	/* Set/Report Station�� Scanning Center Coordinate */
	case ENG_RSCC::rsc_sdbg	: SetValue(m_pstShMem->scan_debug_mode, vals);							break;	/* Currently set Scan Debug Mode */
	case ENG_RSCC::rsc_slp	: SetAxisValues(m_pstShMem->station_scanning_left_coord, vals);			break;	/* Set/Report Station�� Scanning Left Coordinate */
	case ENG_RSCC::rsc_smt	: SetInch001Value(m_pstShMem->station_wafer_max_thickness, vals);		break;	/* Set/Report Per-Station Maximum Wafer Thickness */
	case ENG_RSCC::rsc_srp	: SetAxisValues(m_pstShMem->station_scanning_right_coord, vals);		break;	/* Set/Report Station�� Scanning Right Coordinate */
	case ENG_RSCC::rsc_szh	: SetInch001Value(m_pstShMem->station_scaning_highest_z_coord, vals);	break;	/* Current Scanning Highest Z Position, for a given or default station */
	case ENG_RSCC::rsc_szl	: SetInch001Value(m_pstShMem->station_scaning_lowest_z_coord, vals);	break;	/* Current Scanning Lowest Z Position, for a given or default station */
	case ENG_RSCC::rsc_tsp	:																		break;	/* Test scanning position */
	}
}

/*
 desc : ���� ������ ó�� - 12. Align-on-Flight (On-The-Flight, OtF) parameters and commands
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMDRThread::RecvDataOtFCmds(vector <string> &vals)
{
	switch (m_enLastCmd)
	{
	case ENG_RSCC::rsc_acs		: SetValue(m_pstShMem->otf_set_number, vals);					break;	/* Set OtF configuration set number */
	case ENG_RSCC::rsc_aofcn2	: SetValue(m_pstShMem->otf_set_second_scan_notch, vals);		break;	/* Set Check for notch on second scan option */
	case ENG_RSCC::rsc_aofd		: SetValue(m_pstShMem->otf_debug_level, vals);					break;	/* Set OtF debug level */
	case ENG_RSCC::rsc_aofdump	: SetTempTextInfo(vals);										break;	/* Report Samples from Last OtF Operation */
	case ENG_RSCC::rsc_aofim	: SetValue(m_pstShMem->otf_ignore_middle_samples, vals);		break;	/* Set Ignore Middle Samples option */
	}
}

/*
 desc : ���� ������ ó�� - 13. 13. New Command for DUAL Arm
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMDRThread::RecvDataNewDualArm(vector <string> &vals)
{
	switch (m_enLastCmd)
	{
	case ENG_RSCC::rsc_see	: SetValue(m_pstShMem->arm_used_scan_spec_station, vals);			break;	/* Set OtF configuration set number */
	}
}

/*
 desc : ���� ���ŵ� ������ �ʱ�ȭ
 parm : code	- [in]  Send ����   ��ɾ� ���� �ڵ� ��
 retn : None
*/
VOID CMDRThread::ResetRecvData(ENG_RSCC code)
{
	/* ������ �۽��� ��ɾ� ����, ���� ������ ó�� */
	if (code <= ENG_RSCC::rsc_ver)			ResetDataInfoCmds(code);			/* 1. Informational commands */
	else if (code <= ENG_RSCC::rsc_ssp)		ResetDataHousekeepCmds(code);		/* 2. Housekeeping (Parameters save / restore) commands */
	else if (code <= ENG_RSCC::rsc_sta)		ResetDataStatusDiagCmds(code);		/* 3. Status and diagnostic commands */
	else if (code <= ENG_RSCC::rsc__ralmc)	ResetDataAbsEncCmds(code);			/* 4. Support for systems with absolute encoders */
	else if (code <= ENG_RSCC::rsc_vst_b)	ResetDataBasicActCmds(code);		/* 5. Basic (non-axes, non-station) action commands */
	else if (code <= ENG_RSCC::rsc_spd)		ResetDataBasicMotionParam(code);	/* 6. Basic axis motion parameters */
	else if (code <= ENG_RSCC::rsc_wsa)		ResetDataAxisParmSetCmds(code);		/* 7. Axis parameter sets, and custom parameters commands */
	else if (code <= ENG_RSCC::rsc_wmc)		ResetDataBasicMotionCmds(code);		/* 8. Basic axis motions commands */
	else if (code <= ENG_RSCC::rsc_vto)		ResetDataStaParamCmds(code);		/* 9. Station parameters commands */
	else if (code <= ENG_RSCC::rsc_spg)		ResetDataStaMotionCmds(code);		/* 10.Station motion commands */
	else if (code <= ENG_RSCC::rsc_tsp)		ResetDataStaScanCmds(code);			/* 11.Station scanning parameters and motion commands */
	else if (code <= ENG_RSCC::rsc_rwic)	ResetDataOtFCmds(code);				/* 12.Align-on-Flight (On-The-Flight, OtF) parameters and commands */
}
VOID CMDRThread::ResetDataInfoCmds(ENG_RSCC code)
{
	switch (code)
	{
	case ENG_RSCC::rsc__dt	:
	case ENG_RSCC::rsc__si	:
	case ENG_RSCC::rsc_res	:
	case ENG_RSCC::rsc_ver	: memset(m_pstShMem->last_string_data, 0x00, LAST_STRING_DATA_SIZE);	break;
	case ENG_RSCC::rsc_snr	: memset(m_pstShMem->serial_number, 0x00, 10);	break;
	}
}
VOID CMDRThread::ResetDataHousekeepCmds(ENG_RSCC code)
{
}
VOID CMDRThread::ResetDataStatusDiagCmds(ENG_RSCC code)
{
	UINT8 i	= 0x00;
	switch (code)
	{
	case ENG_RSCC::rsc___cs	:
	case ENG_RSCC::rsc_diag	:
	case ENG_RSCC::rsc_per	:
	case ENG_RSCC::rsc_est	:
	case ENG_RSCC::rsc_esta	: memset(m_pstShMem->last_string_data, 0x00, LAST_STRING_DATA_SIZE);					break;
	case ENG_RSCC::rsc_cpo	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->axis_current_position[i] = DBL_MAX;	break;
	case ENG_RSCC::rsc_eerr	: m_pstShMem->extended_error_code	= 0xffff;											break;
	case ENG_RSCC::rsc_inf	: m_pstShMem->info_mode				= 0xff;												break;
	case ENG_RSCC::rsc_rhs	: m_pstShMem->home_switch_state		= 0x0000;											break;
	case ENG_RSCC::rsc_sta	: m_pstShMem->system_status			= 0xffff;											break;
	}
}
VOID CMDRThread::ResetDataAbsEncCmds(ENG_RSCC code)
{
	UINT8 i	= 0x00;
	switch (code)
	{
	case ENG_RSCC::rsc__aest	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->enc_status[i]	= -1;			break;
	case ENG_RSCC::rsc__eo		:
	case ENG_RSCC::rsc__eor		: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->enc_zero_offset[i] = DBL_MAX;	break;
	case ENG_RSCC::rsc__eo_t	: m_pstShMem->enc_zero_offset[0] = DBL_MAX;											break;
	case ENG_RSCC::rsc__eo_r1	: m_pstShMem->enc_zero_offset[1] = DBL_MAX;											break;
	case ENG_RSCC::rsc__eo_r2	: m_pstShMem->enc_zero_offset[2] = DBL_MAX;											break;
	case ENG_RSCC::rsc__eo_z	: m_pstShMem->enc_zero_offset[3] = DBL_MAX;											break;
	case ENG_RSCC::rsc__ralmc	: memset(m_pstShMem->last_string_data, 0x00, LAST_STRING_DATA_SIZE);				break;
	}
}
VOID CMDRThread::ResetDataBasicActCmds(ENG_RSCC code)
{
	switch (code)
	{
	case ENG_RSCC::rsc_cstp	: m_pstShMem->controlled_stop[0]			= 0xff;		break;
	case ENG_RSCC::rsc_din	: m_pstShMem->external_di_in_state			= 0xff;		break;
	case ENG_RSCC::rsc_dout	: m_pstShMem->external_di_out_state			= 0xff;		break;
	case ENG_RSCC::rsc_inp	: m_pstShMem->digital_input_state			= 0xffff;	break;
	case ENG_RSCC::rsc_lenb	: m_pstShMem->loaded_speed_enable			= 0xff;		break;
	case ENG_RSCC::rsc_out	: m_pstShMem->digital_output_state			= 0xffff;	break;
	case ENG_RSCC::rsc_vst_t: m_pstShMem->end_effector_wafer_check[0]	= 0xff;		break;
	case ENG_RSCC::rsc_vst_b: m_pstShMem->end_effector_wafer_check[1]	= 0xff;		break;
	case ENG_RSCC::rsc__bb	: m_pstShMem->backup_dc_power[0]			= 0xffff;	break;
	}
}
VOID CMDRThread::ResetDataBasicMotionParam(ENG_RSCC code)
{
	UINT8 i	= 0x00;
	switch (code)
	{
	case ENG_RSCC::rsc__ch	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->homed_position[i] = DBL_MAX;					break;
	case ENG_RSCC::rsc__cl	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->current_limit[i] = UINT8_MAX;					break;
	case ENG_RSCC::rsc__el	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->position_error_limit[i] = INT32_MAX;			break;
	case ENG_RSCC::rsc__fl	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->sw_positive_limit[i] = DBL_MAX;					break;
	case ENG_RSCC::rsc__fse	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->axis_safe_envelop_forward[i] = DBL_MAX;			break;
	case ENG_RSCC::rsc__hd	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->axis_diff_coeff_hold[i] = UINT32_MAX;			break;
	case ENG_RSCC::rsc__he	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->pos_err_limit_hold[i] = DBL_MAX;				break;
	case ENG_RSCC::rsc__hi	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->axis_inte_coeff_hold[i] = UINT32_MAX;			break;
	case ENG_RSCC::rsc__hl	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->axis_inte_limit_hold[i] = DBL_MAX;				break;
	case ENG_RSCC::rsc__hp	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->axis_prop_coeff_hold[i] = UINT32_MAX;			break;
	case ENG_RSCC::rsc__il	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->axis_integral_limit[i] = DBL_MAX;				break;
	case ENG_RSCC::rsc__kd	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->axis_diff_coeff[i] = UINT32_MAX;				break;
	case ENG_RSCC::rsc__kp	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->axis_prop_coeff[i] = UINT32_MAX;				break;
	case ENG_RSCC::rsc__ki	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->axis_inte_coeff[i] = UINT32_MAX;				break;
	case ENG_RSCC::rsc__rl	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->sw_negative_limit[i] = DBL_MAX;					break;
	case ENG_RSCC::rsc__rse	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->axis_safe_envelop_reverse[i] = DBL_MAX;			break;
	case ENG_RSCC::rsc_acl	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->axis_acceleration[i] = DBL_MAX;					break;
	case ENG_RSCC::rsc_ajk	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->axis_acceleration_jerk[i] = UINT16_MAX;			break;
	case ENG_RSCC::rsc_dcl	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->axis_deceleration[i] = DBL_MAX;					break;
	case ENG_RSCC::rsc_djk	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->axis_deceleration_jerk[i] = UINT16_MAX;			break;
	case ENG_RSCC::rsc_hfp	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->home_flip_position[i] = DBL_MAX;				break;
	case ENG_RSCC::rsc_lacl	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->axis_loaded_acceleration[i] = DBL_MAX;			break;
	case ENG_RSCC::rsc_lajk	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->axis_loaded_acceleration_jerk[i] = UINT16_MAX;	break;
	case ENG_RSCC::rsc_ldcl	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->axis_loaded_deceleration[i] = DBL_MAX;			break;
	case ENG_RSCC::rsc_ldjk	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->axis_loaded_deceleration_jerk[i] = UINT16_MAX;	break;
	case ENG_RSCC::rsc_lspd	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->axis_loaded_speed[i] = DBL_MAX;					break;
	case ENG_RSCC::rsc_spd	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->axis_working_speed[i] = DBL_MAX;				break;
	}
}
VOID CMDRThread::ResetDataAxisParmSetCmds(ENG_RSCC code)
{
	switch (code)
	{
	case ENG_RSCC::rsc_das	:
	case ENG_RSCC::rsc_dps	: memset(m_pstShMem->last_string_data, 0x00, LAST_STRING_DATA_SIZE);	break;
	case ENG_RSCC::rsc__cax	: m_pstShMem->custom_axis_param		= INT32_MAX;						break;
	case ENG_RSCC::rsc__cpa	: m_pstShMem->custom_generic_param	= INT32_MAX;						break;
	case ENG_RSCC::rsc__cst	: m_pstShMem->custom_station_param	= INT32_MAX;						break;
	}
}
VOID CMDRThread::ResetDataBasicMotionCmds(ENG_RSCC code)
{
	switch (code)
	{
	case ENG_RSCC::rsc__afe	:
	case ENG_RSCC::rsc_afe	:
	case ENG_RSCC::rsc__fe	: memset(m_pstShMem->last_string_data, 0x00, LAST_STRING_DATA_SIZE);	break;
#if 0	/* �Ʒ� 3�� ����� ��� �۽ſ� ���� ������ �����Ƿ� ... */
	case ENG_RSCC::rsc_hom	:
	case ENG_RSCC::rsc_mvr	:
	case ENG_RSCC::rsc_mva	: m_pstShMem->motion_done_reply	= 0x00;	break;
#endif
	}
}
VOID CMDRThread::ResetDataStaParamCmds(ENG_RSCC code)
{
	UINT8 i	= 0x00;
	switch (code)
	{
	case ENG_RSCC::rsc__css		: m_pstShMem->station_set_no					= 0xff;				break;
	case ENG_RSCC::rsc__pa		: m_pstShMem->r_axis_push_acc_sta				= DBL_MAX;			break;
	case ENG_RSCC::rsc__pa_def	: m_pstShMem->r_axis_push_acc_def				= DBL_MAX;			break;
	case ENG_RSCC::rsc__ps		: m_pstShMem->r_axis_push_spd_sta				= DBL_MAX;			break;
	case ENG_RSCC::rsc__ps_def	: m_pstShMem->r_axis_push_spd_def				= DBL_MAX;			break;
	case ENG_RSCC::rsc__zc		: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->coord_compensation_slot[i] = DBL_MAX;			break;
	case ENG_RSCC::rsc_app		:
	case ENG_RSCC::rsc_ens		: memset(m_pstShMem->last_string_data, 0x00, LAST_STRING_DATA_SIZE);break;
	case ENG_RSCC::rsc_cof		: m_pstShMem->station_center_offset				= DBL_MAX;			break;
	case ENG_RSCC::rsc_cofp		: m_pstShMem->station_center_offset_edge_grip	= DBL_MAX;			break;
	case ENG_RSCC::rsc_csl		: m_pstShMem->current_slot_no					= UINT8_MAX;		break;
	case ENG_RSCC::rsc_cst		: m_pstShMem->station_name						= '@';				break;
	case ENG_RSCC::rsc_cstn		: m_pstShMem->station_no						= UINT8_MAX;		break;
	case ENG_RSCC::rsc_een		: m_pstShMem->end_effector_num					= UINT8_MAX;		break;
	case ENG_RSCC::rsc_eeo		: m_pstShMem->end_effector_up_down_station		= UINT8_MAX;		break;
	case ENG_RSCC::rsc_ils		: m_pstShMem->station_last_seg_speed_factor		= UINT8_MAX;		break;
	case ENG_RSCC::rsc_irr		: m_pstShMem->station_retracted_r_position		= DBL_MAX;			break;
	case ENG_RSCC::rsc_irt		: m_pstShMem->station_retracted_t_position		= DBL_MAX;			break;
	case ENG_RSCC::rsc_isa		: m_pstShMem->station_exit_angle				= DBL_MAX;			break;
	case ENG_RSCC::rsc_isd		: m_pstShMem->station_exit_distance				= DBL_MAX;			break;
	case ENG_RSCC::rsc_ise		: m_pstShMem->station_end_effect_length			= DBL_MAX;			break;
	case ENG_RSCC::rsc_isr		: m_pstShMem->station_corner_radius				= DBL_MAX;			break;
	case ENG_RSCC::rsc_ist		: m_pstShMem->inline_station_type				= UINT8_MAX;		break;
	case ENG_RSCC::rsc_nsl		: m_pstShMem->station_slot_num					= UINT8_MAX;		break;
	case ENG_RSCC::rsc_nst		: m_pstShMem->station_num						= UINT8_MAX;		break;
	case ENG_RSCC::rsc_ofs		: m_pstShMem->station_offset_z					= DBL_MAX;			break;
	case ENG_RSCC::rsc_pgd		: m_pstShMem->put_get_delay_time				= UINT32_MAX;		break;
	case ENG_RSCC::rsc_pgf		: memset(m_pstShMem->put_get_failed_option, 0xff, 2);				break;
	case ENG_RSCC::rsc_pgo		: memset(m_pstShMem->put_get_option, 0xff, 2);						break;
	case ENG_RSCC::rsc_pit		: m_pstShMem->station_pitch						= DBL_MAX;			break;
	case ENG_RSCC::rsc_psd		: m_pstShMem->start_pushing_distance			= DBL_MAX;			break;
	case ENG_RSCC::rsc_psh		: m_pstShMem->pusher_type						= UINT8_MAX;		break;
	case ENG_RSCC::rsc_pss		: m_pstShMem->station_start_pushing_offset		= DBL_MAX;			break;
	case ENG_RSCC::rsc_psz		: m_pstShMem->station_pushing_offset_z			= DBL_MAX;			break;
	case ENG_RSCC::rsc_pua		: m_pstShMem->station_pickup_acceleration_z		= DBL_MAX;			break;
	case ENG_RSCC::rsc_pus		: m_pstShMem->station_pickup_speed_z			= DBL_MAX;			break;
	case ENG_RSCC::rsc_rpo		: m_pstShMem->station_retracted_position_r		= DBL_MAX;			break;
	case ENG_RSCC::rsc_spo		:
	case ENG_RSCC::rsc_spon		: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->station_position[i]	= DBL_MAX;	break;
	case ENG_RSCC::rsc_str		: m_pstShMem->station_stroke_z					= DBL_MAX;			break;
	case ENG_RSCC::rsc_tbl		: m_pstShMem->station_axis_overlap_factor		= UINT8_MAX;		break;
	case ENG_RSCC::rsc_tmd		: m_pstShMem->station_trajectory_mode			= UINT8_MAX;		break;
	case ENG_RSCC::rsc_vchk		: memset(m_pstShMem->station_vacuum_check, 0xff, 2);				break;
	case ENG_RSCC::rsc_vto		: m_pstShMem->vacuum_search_time_out			= UINT32_MAX;		break;
	}
}
VOID CMDRThread::ResetDataStaMotionCmds(ENG_RSCC code)
{
	switch (code)
	{
	case ENG_RSCC::rsc_sbusy	: m_pstShMem->station_service_busy				= UINT8_MAX;		break;
	case ENG_RSCC::rsc_slck		: m_pstShMem->station_locked_state				= UINT8_MAX;		break;
	}
}
VOID CMDRThread::ResetDataStaScanCmds(ENG_RSCC code)
{
	UINT8 i	= 0x00;
	switch (code)
	{
	case ENG_RSCC::rsc__bt		: m_pstShMem->beam_threshold[0]					= DBL_MAX;
								  m_pstShMem->beam_threshold[1]					= DBL_MIN;			break;
	case ENG_RSCC::rsc__mt		: m_pstShMem->maximum_wafer_thick				= DBL_MAX;			break;
	case ENG_RSCC::rsc__sa		: m_pstShMem->wafer_scan_z_axis_acc				= DBL_MAX;			break;
	case ENG_RSCC::rsc__ss		: m_pstShMem->wafer_scan_z_axis_spd				= DBL_MAX;			break;
	case ENG_RSCC::rsc__st		: m_pstShMem->scanner_type						= UINT8_MAX;		break;
	case ENG_RSCC::rsc_dscn		:
	case ENG_RSCC::rsc_tcs		: 
	case ENG_RSCC::rsc_map		: memset(m_pstShMem->last_string_data, 0x00, LAST_STRING_DATA_SIZE);break;
	case ENG_RSCC::rsc_sbt		: m_pstShMem->station_per_beam_threshold[0]		= DBL_MAX;
								  m_pstShMem->station_per_beam_threshold[1]		= DBL_MIN;			break;
	case ENG_RSCC::rsc_scp		: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->station_scanning_center_coord[i]= DBL_MAX;	break;
	case ENG_RSCC::rsc_sdbg		: m_pstShMem->scan_debug_mode					= UINT8_MAX;		break;
	case ENG_RSCC::rsc_slp		: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->station_scanning_left_coord[i]	= DBL_MAX;	break;
	case ENG_RSCC::rsc_smt		: m_pstShMem->station_wafer_max_thickness		= DBL_MAX;			break;
	case ENG_RSCC::rsc_srp		: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	m_pstShMem->station_scanning_right_coord[i] = DBL_MAX;	break;
	case ENG_RSCC::rsc_szh		: m_pstShMem->station_scaning_highest_z_coord	= DBL_MIN;			break;
	case ENG_RSCC::rsc_szl		: m_pstShMem->station_scaning_lowest_z_coord	= DBL_MAX;			break;
	}
}
VOID CMDRThread::ResetDataOtFCmds(ENG_RSCC code)
{
	switch (code)
	{
	case ENG_RSCC::rsc_acs		: m_pstShMem->otf_set_number			= 0xffff;					break;
	case ENG_RSCC::rsc_aofcn2	: m_pstShMem->otf_set_second_scan_notch	= 0xff;						break;
	case ENG_RSCC::rsc_aofd		: m_pstShMem->otf_debug_level			= 0x7f;						break;
	case ENG_RSCC::rsc_aofim	: m_pstShMem->otf_ignore_middle_samples	= 0xff;						break;
	case ENG_RSCC::rsc_aofdump	: memset(m_pstShMem->last_string_data, 0x00, LAST_STRING_DATA_SIZE);break;
	}
}
VOID CMDRThread::ResetDataNewDualArm(ENG_RSCC code)
{
	switch (code)
	{
	case ENG_RSCC::rsc_see		: m_pstShMem->arm_used_scan_spec_station	= 0xff;		break;
	}
}

/*
 desc : 4�� ���� �� ��ȯ �� ����
 parm : host_val- [in]  ����� Host�� Double/UINT32/UINT16/UINT8/INT8 �迭 ���� ������
		vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMDRThread::SetAxisValues(DOUBLE *host_val, vector <string> &vals)
{
	UINT32 i = 0, u32Count = (UINT32)vals.size();
	if (u32Count > 4)	u32Count	= MAX_ROBOT_DRIVE_COUNT;	/* �ϴ�, K �� ���� �ʿ� ����, ���ڿ��� ��ȯ�Ǳ� ������ ... */
	for (; i<u32Count; i++)	host_val[i] = GetAxisPosApp(ENG_RANC(i), atoi(vals[i].c_str()));
}
VOID CMDRThread::SetAxisValues(UINT32 *host_val, vector <string> &vals)
{
	UINT32 i = 0, u32Count = (UINT32)vals.size();
	if (u32Count > 4)	u32Count	= MAX_ROBOT_DRIVE_COUNT;	/* �ϴ�, K �� ���� �ʿ� ����, ���ڿ��� ��ȯ�Ǳ� ������ ... */
	for (; i<u32Count; i++)	host_val[i] = UINT32(atoi(vals[i].c_str()));
}
VOID CMDRThread::SetAxisValues(UINT16 *host_val, vector <string> &vals)
{
	UINT32 i = 0, u32Count = (UINT32)vals.size();
	if (u32Count > 4)	u32Count	= MAX_ROBOT_DRIVE_COUNT;	/* �ϴ�, K �� ���� �ʿ� ����, ���ڿ��� ��ȯ�Ǳ� ������ ... */
	for (; i<u32Count; i++)	host_val[i] = UINT16(atoi(vals[i].c_str()));
}

/*
 desc : T / Z ���� �� ��ȯ �� ����
 parm : host_val- [in]  ����� Host�� Double/UINT32/UINT16/UINT8/INT8 �迭 ���� ������
		vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMDRThread::SetAxisZValue(DOUBLE &host_val, vector <string> &vals)
{
	host_val = GetAxisPosApp(ENG_RANC::rac_z, atoi(vals[0].c_str()));
}
VOID CMDRThread::SetAxisTValue(DOUBLE &host_val, vector <string> &vals)
{
	host_val = GetAxisPosApp(ENG_RANC::rac_t, atoi(vals[0].c_str()));
}

/*
 desc : ���ŵ� �߿��� �ؽ�Ʈ ������ Host ���� �ӽ� ���ڿ� ���ۿ� ����
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMDRThread::SetTempTextInfo(vector <string> &vals)
{
	memset(m_pstShMem->last_string_data, 0x00, LAST_STRING_DATA_SIZE);
	strcpy_s(m_pstShMem->last_string_data, LAST_STRING_DATA_SIZE, vals[0].c_str());
}

/*
 desc : �κ� �ø��� ��ȣ ����
 parm : vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMDRThread::SetSerialNum(vector <string> &vals)
{
	memset(m_pstShMem->serial_number, 0x00, 10);
	strcpy_s(m_pstShMem->serial_number, 10, vals[0].c_str());
}

/*
 desc : Station���� Map ���� �м� (�� Slot ���� Wafer�� �ִ���, �ִٸ� ������ �ִ��� ��)
 parm : host_val- [in]  ����� Host�� Double/UINT32/UINT16/UINT8/INT8 �迭 ���� ������
		vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
 note : 0	No wafer present in this slot
		1	Single wafer is present, no other cross-slotted
		2	Two wafers found in single slot
		4	Single Cross-slotted wafer
		5	Single wafer with cross-slotted wafer on next slot
		6	Double-slotted, or cross-slotted wafer with another wafer
*/
VOID CMDRThread::SetMapValues(UINT8 *host_val, vector <string> &vals)
{
	UINT32 i = 0, u32Count = (UINT32)strlen(vals[0].c_str());
	PCHAR pszMap	= (PCHAR)vals[0].c_str();
	if (u32Count > 25)	u32Count = 25;
	for (; i<u32Count; i++)	host_val[i] = pszMap[i] - 0x30 /* 0 */;
}

/*
 desc : Ȯ�� ���� ���� ���� ���� �� ���� ��ȯ (��, Error codes returned by EERR)
 parm : mesg	- [out] ���� �ڵ忡 ���� �� ������ ����� ��ȯ ����
		size	- [in]  'mesg' ���� ũ��
 retn : FALSE (���� ����), TRUE (���� ����)
*/
BOOL CMDRThread::GetExtErrorMesg(TCHAR *mesg, UINT32 size)
{
	if (0x0000 == m_pstShMem->extended_error_code)	return FALSE;
	if (size < 1)	return FALSE;

	switch (m_pstShMem->extended_error_code)
	{
	case 0x0000	: wcscpy_s(mesg, size, L"No error");									break;
	case 0x0001	: wcscpy_s(mesg, size, L"Invalid configuration");						break;
	case 0x0002	: wcscpy_s(mesg, size, L"Invalid parameter");							break;
	case 0x0003	: wcscpy_s(mesg, size, L"Invalid parameter set");						break;
	case 0x0004	: wcscpy_s(mesg, size, L"Invalid REE option");							break;
	case 0x0005	: wcscpy_s(mesg, size, L"Error during REE operation");					break;
	case 0x0006	: wcscpy_s(mesg, size, L"Invalid state for command execution");			break;
	case 0x0007	: wcscpy_s(mesg, size, L"REE not powered up");							break;
	case 0x0010	: wcscpy_s(mesg, size, L"Un-supported AE configuration");				break;
	case 0x0080	: wcscpy_s(mesg, size, L"Servo is off");								break;
	case 0x0081	: wcscpy_s(mesg, size, L"Wafer on paddle");								break;
	case 0x0082	: wcscpy_s(mesg, size, L"No wafer when needed");						break;
	case 0x0083	: wcscpy_s(mesg, size, L"Not homed");									break;
	case 0x0084	: wcscpy_s(mesg, size, L"In teach mode");								break;
	case 0x0085	: wcscpy_s(mesg, size, L"Edge Gripper error");							break;
	case 0x0100	: wcscpy_s(mesg, size, L"Invalid station");								break;
	case 0x0101	: wcscpy_s(mesg, size, L"Invalid slot");								break;
	case 0x0102	: wcscpy_s(mesg, size, L"Still moving : busy");							break;
	case 0x0103	: wcscpy_s(mesg, size, L"Invalid axis name");							break;
	case 0x0104	: wcscpy_s(mesg, size, L"Error during GET_PUT");						break;
	case 0x0105	: wcscpy_s(mesg, size, L"Station sets not enabled");					break;
	case 0x0106	: wcscpy_s(mesg, size, L"Invalid wait option for GETW_PUTW");			break;
	case 0x0107	: wcscpy_s(mesg, size, L"Invalid [where] option for MTS_MTR");			break;
	case 0x0108	: wcscpy_s(mesg, size, L"Zero or odd number of samples during SCN");	break;
	case 0x0109	: wcscpy_s(mesg, size, L"Scan sensor ON at start : finish SCN");		break;
	case 0x010A	: wcscpy_s(mesg, size, L"Scan feature not available");					break;
	case 0x010B	: wcscpy_s(mesg, size, L"Motion command for locked station");			break;
	case 0x0200	: wcscpy_s(mesg, size, L"Init in progress");							break;
	case 0x0201	: wcscpy_s(mesg, size, L"Emergency : interlock active (robot)");		break;
	case 0x0202	: wcscpy_s(mesg, size, L"Emergency : interlock active (preal)");		break;
	case 0x0203	: wcscpy_s(mesg, size, L"Controlled stop active");						break;
	case 0x0204	: wcscpy_s(mesg, size, L"File error during save_restore");				break;
	case 0x0205	: wcscpy_s(mesg, size, L"Motion error");								break;
	case 0x0206	: wcscpy_s(mesg, size, L"Wafer lost during transport");					break;
	case 0x0207	: wcscpy_s(mesg, size, L"System just switched on to battery power");	break;
	case 0x0208	: wcscpy_s(mesg, size, L"No mains power on robot");						break;
	case 0x0209	: wcscpy_s(mesg, size, L"System shutting down");						break;
	case 0x020A	: wcscpy_s(mesg, size, L"Mains power just returned");					break;
	case 0x020B	: wcscpy_s(mesg, size, L"No mains power on linear track");				break;
	case 0x0210	: wcscpy_s(mesg, size, L"Other error");									break;
	case 0x0240	: wcscpy_s(mesg, size, L"Absolute encoder(s) in faulty state");			break;
	case 0x0241	: wcscpy_s(mesg, size, L"Absolute encoder(s) in warning state");		break;
	case 0x0310	: wcscpy_s(mesg, size, L"Timeout waiting for prealigner");				break;
	case 0x0311	: wcscpy_s(mesg, size, L"Slave aligner busy");							break;
	case 0x0380	: wcscpy_s(mesg, size, L"No wafer map to display");						break;
	case 0x0381	: wcscpy_s(mesg, size, L"Invalid parameters for MAP");					break;
	case 0x0382	: wcscpy_s(mesg, size, L"Invalid safety scan parameters");				break;
	case 0x0390	: wcscpy_s(mesg, size, L"Crossed samples during mapping");				break;
	case 0x0391	: wcscpy_s(mesg, size, L"Samples more than 3 x _MT");					break;
	case 0x0400	: wcscpy_s(mesg, size, L"Error during AOFC");							break;
	case 0x0401	: wcscpy_s(mesg, size, L"Not enough samples (less than 4)");			break;
	case 0x0402	: wcscpy_s(mesg, size, L"Samples decoding invalid");					break;
	case 0x0403	: wcscpy_s(mesg, size, L"Calculated offset too big");					break;
	case 0x0404	: wcscpy_s(mesg, size, L"Invalid parameter set");						break;
	}

	return TRUE;
}

/*
 desc : �ý��� ���� ���� ���� ���� �޽��� ��ȯ
 parm : mesg	- [out] ���� �ڵ忡 ���� �� ������ ����� ��ȯ ����
		size	- [in]  'mesg' ���� ũ��
 retn : FALSE (Robot Idle ����), TRUE (���� ����)
 note : "mesg" ���۸� �ּ� 1024��ŭ ������ ��� ��
*/
BOOL CMDRThread::GetSystemStatus(TCHAR *mesg, UINT32 size)
{
	UINT8 i	= 0x00;
	UINT16 u16Bit	= 0x0001, u16Mask = 0x000, u16Status = m_pstShMem->system_status;

	if (u16Status == 0x0000 || u16Status == 0xffff)	return FALSE;

	/* 16 bit �̹Ƿ�, �� 15�� �̵� (Left Shift)�ϸ鼭 bit mask �� Ȯ�� */
	for (; i<16; i++)
	{
		u16Mask	= m_pstShMem->system_status & (u16Bit << i);
#if 1
		switch (u16Mask)
		{
		case 0x0001	: wcscat_s(mesg, size, L"Unable to execute motion commands");	break;
		case 0x0002	: wcscat_s(mesg, size, L"Previous command returned an error");	break;
		case 0x0004	: wcscat_s(mesg, size, L"Vacuum Sensor ON");					break;
		case 0x0008	: wcscat_s(mesg, size, L"Vacuum Switch ON");					break;
		case 0x0010	: wcscat_s(mesg, size, L"Motion or position error");			break;
		case 0x0020	: wcscat_s(mesg, size, L"Software Motion limits violation");	break;
		case 0x0040	: wcscat_s(mesg, size, L"Home procedure not executed");			break;
		case 0x0080	: wcscat_s(mesg, size, L"User macro running");					break;
		case 0x0100	: wcscat_s(mesg, size, L"Motion in progress");					break;
		case 0x0200	: wcscat_s(mesg, size, L"Single-axis prealigner servo off");	break;
		case 0x0400	: wcscat_s(mesg, size, L"Servo OFF on one or more axes");		break;
		case 0x0800	: wcscat_s(mesg, size, L"In teach mode (TCH/EOT)");				break;
		case 0x1000	: wcscat_s(mesg, size, L"In search mode (PGO)");				break;
		case 0x2000	: wcscat_s(mesg, size, L"I/O File error");						break;
		case 0x4000	: wcscat_s(mesg, size, L"In Teach Scan mode (TSC/EOT)");		break;
		case 0x8000	: wcscat_s(mesg, size, L"Prealigner Busy");						break;
		}
#else
		switch (u16Mask)
		{
		case 0x0001	: wcscat_s(mesg, size-(UINT32)wcslen(mesg), L"Unable to execute motion commands");									break;
		case 0x0002	: wcscat_s(mesg, size-(UINT32)wcslen(mesg), L"Previous command returned an error. See PER command for details");	break;
		case 0x0004	: wcscat_s(mesg, size-(UINT32)wcslen(mesg), L"Chuck Vacuum Sensor ON");												break;
		case 0x0008	: wcscat_s(mesg, size-(UINT32)wcslen(mesg), L"Chuck Vacuum Switch ON");												break;
		case 0x0010	: wcscat_s(mesg, size-(UINT32)wcslen(mesg), L"Motion error");														break;
		case 0x0020	: wcscat_s(mesg, size-(UINT32)wcslen(mesg), L"Motion limits violation");											break;
		case 0x0040	: wcscat_s(mesg, size-(UINT32)wcslen(mesg), L"Home procedure not executed");										break;
		case 0x0080	: wcscat_s(mesg, size-(UINT32)wcslen(mesg), L"User macro running");													break;
		case 0x0100	: wcscat_s(mesg, size-(UINT32)wcslen(mesg), L"Motion in progress");													break;
		case 0x0200	: wcscat_s(mesg, size-(UINT32)wcslen(mesg), L"Reserved");															break;
		case 0x0400	: wcscat_s(mesg, size-(UINT32)wcslen(mesg), L"Servo OFF on one or more axes");										break;
		case 0x0800	: wcscat_s(mesg, size-(UINT32)wcslen(mesg), L"Reserved");															break;
		case 0x1000	: wcscat_s(mesg, size-(UINT32)wcslen(mesg), L"Reserved");															break;
		case 0x2000	: wcscat_s(mesg, size-(UINT32)wcslen(mesg), L"I/O File error");														break;
		case 0x4000	: wcscat_s(mesg, size-(UINT32)wcslen(mesg), L"Reserved");															break;
		case 0x8000	: wcscat_s(mesg, size-(UINT32)wcslen(mesg), L"Pins Vacuum Switch ON");												break;
		}
#endif
		if (u16Mask && lstrcmpi(mesg, L"") != 0)
		{
			wcscat_s(mesg, size, L"/");
		}
	}

	return TRUE;
}

/*
 desc : Ȩ ����ġ���� ���¿� ���� ���� �޽��� ��ȯ (��, Error codes returned by RHS)
 parm : mesg	- [out] ���� �ڵ忡 ���� �� ������ ����� ��ȯ ����
		size	- [in]  'mesg' ���� ũ��
 retn : FALSE (���� ����), TRUE (���� ����)
*/
BOOL CMDRThread::GetHomeSwitchesState(TCHAR *mesg, UINT32 size)
{
	if (0x0000 == m_pstShMem->home_switch_state)	return FALSE;
	if (size < 1)	return FALSE;

	/* ���� �ʱ�ȭ */
	wmemset(mesg, 0x00, size);

	switch (m_pstShMem->home_switch_state)
	{
	case 0x0001	: wcscpy_s(mesg, size, L"T axis home switch is active");			break;
	case 0x0002	: wcscpy_s(mesg, size, L"T axis forward software limit violated");	break;
	case 0x0004	: wcscpy_s(mesg, size, L"T axis reverse software limit violated");	break;
	case 0x0010	: wcscpy_s(mesg, size, L"R axis home switch is active");			break;
	case 0x0020	: wcscpy_s(mesg, size, L"R axis forward software limit violated");	break;
	case 0x0040	: wcscpy_s(mesg, size, L"R axis reverse software limit violated");	break;
	case 0x0100	: wcscpy_s(mesg, size, L"Z-axis home switch is active");			break;
	case 0x0200	: wcscpy_s(mesg, size, L"Z axis forward software limit violated");	break;
	case 0x0400	: wcscpy_s(mesg, size, L"Z axis reverse software limit violated");	break;
	case 0x1000	: wcscpy_s(mesg, size, L"K-axis home switch is active");			break;
	case 0x2000	: wcscpy_s(mesg, size, L"K axis forward software limit violated");	break;
	case 0x4000	: wcscpy_s(mesg, size, L"K axis reverse software limit violated");	break;
	default		:	return FALSE;
	}

	return TRUE;
}

/*
 desc : ���� Teaching Mode �߿� ��� ������ ��ɾ����� ����
 parm : code	- [in]  �۽� ��ɾ� ������
 retn : ��ȿ�� ��ɾ� (TRUE), ��ȿ���� ���� ��ɾ� (FALSE)
*/
BOOL CMDRThread::IsValidTechingSet(ENG_RSCC code)
{
	switch (code)
	{
	case ENG_RSCC::rsc_spo	:	case ENG_RSCC::rsc_pit	:	case ENG_RSCC::rsc_ofs	:
	case ENG_RSCC::rsc_str	:	case ENG_RSCC::rsc_pus	:	case ENG_RSCC::rsc_pua	:
	case ENG_RSCC::rsc_pgd	:	case ENG_RSCC::rsc_ist	:	case ENG_RSCC::rsc_ise	:
	case ENG_RSCC::rsc_isr	:	case ENG_RSCC::rsc_isa	:	case ENG_RSCC::rsc_isd	:
	case ENG_RSCC::rsc_irt	:	case ENG_RSCC::rsc_irr	:	case ENG_RSCC::rsc_rpo	:
	case ENG_RSCC::rsc_ssp	:	case ENG_RSCC::rsc_eot	:	return TRUE;
	}

	return FALSE;
}