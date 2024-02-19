
#pragma once

#include "DlgMenu.h"

class CGridMotor;
class CGridPLC;

// by sys&j : 가독성을 위해 enum 추가
enum EnCtrlGrp
{
	eCTRL_GRP_MC2			,
	eCTRL_GRP_SERVO_CTRL	,
	eCTRL_GRP_MOTION_CTRL	,
	eCTRL_GRP_VACUUM_CTRL	,
	eCTRL_GRP_MOTOR_ALL		,
	eCTRL_GRP_MOTOR_HOMING	,
	eCTRL_GRP_SHUTTER_CTRL	,
	eCTRL_GRP_CHILLER_POWER	,
	eCTRL_GRP_RESET_CTRL	,
	eCTRL_GRP_VACUUM_POWER	,
	eCTRL_GRP_HOTAIR_POWER	,
	eCTRL_GRP_MOTOR_POS		,
	eCTRL_GRP_VACUUM_FREQ	,
	eCTRL_GRP_PLC_IO		,
	eCTRL_GRP_MOTOR_Z_AXIS	,
	eCTRL_GRP_HOMING_Z_AXIS	,
	eCTRL_GRP_RESET_Z_AXIS	,
	eCTRL_GRP_BUZZER		,
	eCTRL_GRP_PH_INIT		,
	eCTRL_GRP_MAX
};
enum EnCtrlBtnMov 
{
	eCTRL_BTN_MOV_MC2_LEFT	 ,
	eCTRL_BTN_MOV_MC2_RIGHT	 ,
	eCTRL_BTN_MOV_MC2_UP	 ,
	eCTRL_BTN_MOV_MC2_DOWN	 ,
	eCTRL_BTN_MOV_PLC_UP	 ,
	eCTRL_BTN_MOV_PLC_DOWN	 ,
	eCTRL_BTN_MOV_MAX
};
enum EnCtrlBtnMot 
{
	eCTRL_BTN_MOTOR_STOP	 ,
	eCTRL_BTN_MOTOR_HOMING	 ,
	eCTRL_BTN_MOTOR_RESET	 ,
	eCTRL_BTN_MOTOR_MAX
};
enum EnCtrlBtnHos 
{
	eCTRL_BTN_HOMING_STAGE_X,	
	eCTRL_BTN_HOMING_STAGE_Y,
	eCTRL_BTN_HOMING_ACAM_X1,
	eCTRL_BTN_HOMING_ACAM_X2,
	eCTRL_BTN_HOMING_MAX	,
};
enum EnCtrlBtnHop 
{
	eCTRL_BTN_HOMING_PH_1	,
	eCTRL_BTN_HOMING_PH_2	,
	eCTRL_BTN_HOMING_PH_3	,
	eCTRL_BTN_HOMING_PH_4	,
	eCTRL_BTN_HOMING_PH_5	,
	eCTRL_BTN_HOMING_PH_MAX	,
};
enum EnCtrlBtnHoz 
{
	eCTRL_BTN_HOMING_ACAM_Z1		,
	eCTRL_BTN_HOMING_ACAM_Z2		,
	eCTRL_BTN_HOMING_ACAM_Z_ALL		,
	eCTRL_BTN_HOMING_ACAM_MAX,
};
enum EnCtrlBtnRsz 
{
	eCTRL_BTN_RESET_ACAM_Z1		,
	eCTRL_BTN_RESET_ACAM_Z2		,
	eCTRL_BTN_RESET_ACAM_Z_ALL	,
	eCTRL_BTN_RESET_ACAM_MAX
};
enum EnCtrlBtn	  
{
	eCTRL_BTN_RESET_TRIGGER	,
	eCTRL_BTN_RESET_SAFELOCK,
	eCTRL_BTN_PH_INIT		,
	eCTRL_BTN_MAX			,
};
enum EnCtrlBtnPlc 
{
	eCTRL_BTN_PLC_PREV		,
	eCTRL_BTN_PLC_NEXT		,
	eCTRL_BTN_PLC_UPDATE	,
	eCTRL_BTN_PLC_MAX		,
};
enum EnCtrlChkAxs 
{
	eCTRL_CHK_AXS_STAGE_X,
	eCTRL_CHK_AXS_STAGE_Y,
	eCTRL_CHK_AXS_ACAM_X1,
	eCTRL_CHK_AXS_ACAM_X2,
	eCTRL_CHK_AXS_MAX,
};
enum EnCtrlChkAxz 
{
	eCTRL_CHK_ACAM_Z1,
	eCTRL_CHK_ACAM_Z2,
	eCTRL_CHK_ACAM_MAX
};
enum EnCtrlChkPh 
{
	eCTRL_CHK_PH_1	  ,
	eCTRL_CHK_PH_2	  ,
	eCTRL_CHK_PH_3	  ,
	eCTRL_CHK_PH_4	  ,
	eCTRL_CHK_PH_5	  ,
	eCTRL_CHK_PH_MAX  ,
};
enum EnCtrlChkSvs 
{
	eCTRL_CHK_SERVO_STAGE_X	 ,
	eCTRL_CHK_SERVO_STAGE_Y	 ,
	eCTRL_CHK_SERVO_ACAM_X1	 ,
	eCTRL_CHK_SERVO_ACAM_X2	 ,
	eCTRL_CHK_SERVO_MAX		 ,
};
enum EnCtrlChkSvp 
{
	eCTRL_CHK_SERVO_PH_1 ,
	eCTRL_CHK_SERVO_PH_2 ,
	eCTRL_CHK_SERVO_PH_3 ,
	eCTRL_CHK_SERVO_PH_4 ,
	eCTRL_CHK_SERVO_PH_5 ,
	eCTRL_CHK_SERVO_PH_MAX
};
enum EnCtrlChkMvt 
{
	eCTRL_CHK_MOVE_METHOD_MC2	 ,
	eCTRL_CHK_MOVE_METHOD_ACAM	 ,
	eCTRL_CHK_MOVE_MAX			 ,
};
enum EnCtrlChk	  
{
	eCTRL_CHK_VACUUM_BLOW		,
	eCTRL_CHK_VACUUM_VAC		,
	eCTRL_CHK_SHUTTER_CLOSE		,
	eCTRL_CHK_SHUTTER_OPEN		,
	eCTRL_CHK_CHILLER_OFF		,
	eCTRL_CHK_CHILLER_ON		,
	eCTRL_CHK_VACUUM_OFF		,
	eCTRL_CHK_VACUUM_ON			,
	eCTRL_CHK_HOTAIR_OFF		,
	eCTRL_CHK_HOTAIR_ON			,
	eCTRL_CHK_BUZZER_OFF		,
	eCTRL_CHK_BUZZER_ON			,
	eCTRL_CHK_MAX				,
};
enum EnCtrlTxt	  
{
	eCTRL_TXT_MC2_DIST			,	
	eCTRL_TXT_MC2_SPEED			,
	eCTRL_TXT_VACUUM_FREQ_UNIT	,
	eCTRL_TXT_PLC_DIST			,
	eCTRL_TXT_MAX,
};
enum EnCtrlEdtFlt 
{
	eCTRL_EDT_FLOAT_MC2_SPEED	,
	eCTRL_EDT_FLOAT_MC2_DIST	,
	eCTRL_EDT_FLOAT_PLC_DIST			,
	eCTRL_EDT_FLOAT_MAX,
};
enum EnCtrlEdtTxt 
{
	eCTRL_EDT_VACUUM_FREQ,
	eCTRL_EDT_MAX,
};
// by sys&j : 가독성을 위해 enum 추가

class CDlgCtrl : public CDlgMenu
{
public:

	/* 생성자 / 파괴자 */
	CDlgCtrl(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgCtrl();


/* 가상함수 선언 */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		UpdateControl(UINT64 tick, BOOL is_busy);


public:

	virtual BOOL		OnInitDlg();
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();
	virtual VOID		UpdatePeriod(UINT64 tick, BOOL is_busy);


/* 로컬 변수 */
protected:

	CMyGrpBox			m_grp_ctl[eCTRL_GRP_MAX];
	
	CMacButton			m_btn_mov[eCTRL_BTN_MOV_MAX];			/* Motion Move */
	CMacButton			m_btn_mot[eCTRL_BTN_MOTOR_MAX];			/* Motor All (Stop, Homing, Reset) */
	CMacButton			m_btn_hos[eCTRL_BTN_HOMING_MAX];		/* Homing - Stage */
	CMacButton			m_btn_hop[eCTRL_BTN_HOMING_PH_MAX];		/* Homing - Photohead */
	CMacButton			m_btn_hoz[eCTRL_BTN_HOMING_ACAM_MAX];	/* Homing - Z Axis for camera */
	CMacButton			m_btn_rsz[eCTRL_BTN_RESET_ACAM_MAX];	/* Reset - Z Axis for camera */
	CMacButton			m_btn_ctl[eCTRL_BTN_MAX];				/* Normal */
	CMacButton			m_btn_plc[eCTRL_BTN_PLC_MAX];			/* PLC */

	CMacCheckBox		m_chk_axs[eCTRL_CHK_AXS_MAX];		/* Motor Axis - Stage X / Y, Camera 1, 2*/
	CMacCheckBox		m_chk_axz[eCTRL_CHK_ACAM_MAX];		/* Motor Axis - Align camera Z Axis */
	CMacCheckBox		m_chk_mph[eCTRL_CHK_PH_MAX];		/* Motor Axis - Photohead */
	CMacCheckBox		m_chk_svs[eCTRL_CHK_SERVO_MAX];		/* Motor Servo - stage */
	CMacCheckBox		m_chk_svp[eCTRL_CHK_SERVO_PH_MAX];	/* Motor Servo - photohead */
	CMacCheckBox		m_chk_mvt[eCTRL_CHK_MOVE_MAX];		/* Motion Moving Type : Absolute or Relative for MC2, PLC */
	CMacCheckBox		m_chk_ctl[eCTRL_CHK_MAX];			/* Device ON or OFF / Open or Close / etc */

	CMyStatic			m_txt_ctl[eCTRL_TXT_MAX];	/* Text - Normal */

	// by sys&j : out of range
	//CEditNum			m_edt_flt[2];
	CEditNum			m_edt_flt[eCTRL_EDT_FLOAT_MAX];
	CEditNum			m_edt_txt[eCTRL_EDT_MAX];	/* Pressure / Frequency */

	CGridMotor			*m_pGridMotor;
	CGridPLC			*m_pGridPLC;


/* 로컬 함수 */
protected:

	VOID				InitCtrl();
	VOID				InitValue();

	BOOL				InitGrid();
	VOID				CloseGrid();

	VOID				SetPageRead(UINT8 flag);
	VOID				SetMotionMoving(ENG_MDMD direct);
	VOID				SetPLCMoving(UINT8 up_down);

	VOID				UpdateControlError(LPG_PMRW data);
	VOID				UpdateServoStatus();
	VOID				UpdateCheckStatus();

	VOID				SetMotionHoming(ENG_MMDI drv_id);
	VOID				SetPhHoming(UINT8 drv_id);
	VOID				SetACamZHoming(UINT8 cam_id);
	VOID				SetACamZReset(UINT8 cam_id);
	VOID				SetTriggerReset();

	VOID				SetVacuumCtrl(UINT8 enable);
	VOID				SetShutterCtrl(UINT8 enable);
	VOID				SetChillerPower(UINT8 enable);
	VOID				SetVacuumPower(UINT8 enable);
	VOID				SetBuzzerCtrl(UINT8 enable);

	VOID				SetLuriaPHInited();


/* 공용 함수 */
public:

	

/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
 	afx_msg VOID		OnBtnClick(UINT32 id);
 	afx_msg VOID		OnChkClick(UINT32 id);
};
