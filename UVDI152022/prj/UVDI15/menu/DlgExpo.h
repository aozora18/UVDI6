
#pragma once

#include "DlgMenu.h"

class CDrawMark;
class CDrawDev;
class CMarkDist;
class CExpoVal;
class CTempAlarm;

// by sys&j : 가독성을 위해 enum 추가
enum EnExpoGrp
{
	eEXPO_GRP_TACT_TIME		,
	eEXPO_GRP_ALIGN_MARK	,
	eEXPO_GRP_EXPO_INFO		,
	//eEXPO_GRP_EQUIPMENT		,
	eEXPO_GRP_DIST_ERROR	,
	eEXPO_GRP_DI_TEMP		,
	//eEXPO_GRP_LB_TEMP		,
	//eEXPO_GRP_TEMP_ALARM	,
	eEXPO_GRP_MARK_PAGE		,
	eEXPO_GRP_MAX,
};

enum EnExpoPicMak
{
	eEXPO_PIC_MARK_1            ,
	eEXPO_PIC_MARK_2            ,
	eEXPO_PIC_MARK_3            ,
	eEXPO_PIC_MARK_4            ,
	eEXPO_PIC_MAK_MAX
};

enum EnExpoPicDev
{
	eEXPO_PIC_SERVICE,
	eEXPO_PIC_MC2,
	eEXPO_PIC_PLC,
	eEXPO_PIC_ACAM1,
	eEXPO_PIC_ACAM2,
	eEXPO_PIC_HOTAIR,
	eEXPO_PIC_CHILLER,
	eEXPO_PIC_VACUUM,
	eEXPO_PIC_TRIGGER,
	eEXPO_PIC_DEV_MAX
};

enum EnExpoPgr
{
	eEXPO_PGR_EXPO_RATE,
	eEXPO_PGR_MAX
};

enum EnExpoTxtCtl
{
	eEXPO_TXT_EXPO_WORKS,
	eEXPO_TXT_CTL_MAX,
};

enum EnExpoTxtDev
{
	eEXPO_TXT_SERVICE,
	eEXPO_TXT_MC2,
	eEXPO_TXT_PLC,
	eEXPO_TXT_ACAM1,
	eEXPO_TXT_ACAM2,
	eEXPO_TXT_HOTAIR,
	eEXPO_TXT_CHILLER,
	eEXPO_TXT_VACUUM,
	eEXPO_TXT_TRIGGER,
	eEXPO_TXT_DEV_MAX
};

enum EnExpoTxtMak
{
	eEXPO_TXT_MARK1,
	eEXPO_TXT_MARK2,
	eEXPO_TXT_MARK3,
	eEXPO_TXT_MARK4,
	eEXPO_TXT_MAK_MAX
};

enum EnExpoVal
{
	eEXPO_TXT_TACT_LAST,
	eEXPO_TXT_TACT_AVERAGE,
	eEXPO_TXT_EXPO_COUNT,
	//eEXPO_TXT_DI_TEMP1,
	//eEXPO_TXT_DI_TEMP2,
	//eEXPO_TXT_DI_TEMP3,
	//eEXPO_TXT_DI_TEMP4,
	eEXPO_TXT_LB_TEMP1,
	eEXPO_TXT_LB_TEMP2,
	eEXPO_TXT_LB_TEMP3,
	eEXPO_TXT_LB_TEMP4,
	eEXPO_TXT_LB_TEMP5,
	eEXPO_TXT_LB_TEMP6,
	eEXPO_TXT_VAL_MAX
};

enum EnExpoDst
{
	eEXPO_TXT_MARK_TOP_HORZ,
	eEXPO_TXT_MARK_BTM_HORZ,
	eEXPO_TXT_MARK_LEFT_VERT,
	eEXPO_TXT_MARK_RIGHT_VERT,
	eEXPO_TXT_MARK_LEFT_DIAG,
	eEXPO_TXT_MARK_RIGHT_DIAG,
	eEXPO_TXT_DST_MAX
};

enum EnExpoTmp
{
	eEXPO_TXT_ALARM_DI,
	eEXPO_TXT_ALARM_LED,
	eEXPO_TXT_ALARM_BOARD,
	eEXPO_TXT_TMP_MAX
};

enum EnExpoBtn
{
	eEXPO_BTN_RESET				,
	eEXPO_BTN_RECIPE_LOAD		,
	eEXPO_BTN_VACUMM_ONOFF		,
	eEXPO_BTN_EXPO_ALIGN			,
	eEXPO_BTN_EXPO_DIRECT		,
	eEXPO_BTN_CHECK_ALIGN		,
	eEXPO_BTN_MARK_NO			,
	eEXPO_BTN_MARK_PREV			,
	eEXPO_BTN_MARK_NEXT			,
	eEXPO_BTN_MAX,
};
// by sys&j : 가독성을 위해 enum 추가

class CDlgExpo : public CDlgMenu
{
public:

	/* 생성자 / 파괴자 */
	CDlgExpo(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgExpo();


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
	virtual void		RegisterUILevel() {}
	virtual VOID		UpdatePeriod(UINT64 tick, BOOL is_busy);


/* 로컬 변수 */
protected:

	UINT8				m_u8MarkPage;	/* 기본 1 값. 최대 5 (Global 4 x 1 = 4 ea, Local 4 x 4 = 16 ea) */
	UINT8				m_u8MarkNo;		/* 0x00 (All), 0x01 ~ 0x04: Only Global Mark Number */

	UINT64				m_u64ReqTime;	/* 통신 명령어 요청 시간 임시 저장 */

	CMyGrpBox			m_grp_ctl[eEXPO_GRP_MAX];
	CMacButton			m_btn_ctl[eEXPO_BTN_MAX];
	CMacProgress		m_pgr_ctl[eEXPO_PGR_MAX];

	CMyStatic			m_txt_ctl[eEXPO_TXT_CTL_MAX];	/* Text - Normal */
	//CMyStatic			m_txt_dev[eEXPO_TXT_DEV_MAX];	/* Text - Device Status (TCP/IP or Power) */
	CMyStatic			m_txt_mak[eEXPO_TXT_MAK_MAX];	/* Text - Mark Search Results */
	CMyStatic			m_txt_val[eEXPO_TXT_VAL_MAX];	/* Text - Data Value */
	CMyStatic			m_txt_dst[eEXPO_TXT_DST_MAX];	/* Text - Distance between marks */
	//CMyStatic			m_txt_tmp[eEXPO_TXT_TMP_MAX];	/* Text - Temperature Alarm */

	CMyStatic			m_pic_mak[eEXPO_PIC_MAK_MAX];	/* Picture - Mark Search Results */
	//CMyStatic			m_pic_dev[eEXPO_PIC_DEV_MAX];	/* Picture - Device Status (TCP/IP or Power) */

	CDrawMark			*m_pDrawMark;	/* Mark Search Results Object */
	CDrawDev			*m_pDrawDev;	/* TCP/IP or Power Status for Device */
	CMarkDist			*m_pMarkDist;	/* Distance between marks */
	CExpoVal			*m_pExpoVal;	/* Exposure 화면에서 실시간으로 갱신되는 값 표현 */
	CTempAlarm			*m_pTempAlarm;


/* 로컬 함수 */
protected:

	VOID				InitCtrl();

	BOOL				InitObject();
	VOID				CloseObject();
	BOOL				IsCheckTransRecipe();

	VOID				RedrawMarkPage(UINT8 mode);
	VOID				NextCheckMarkNo();
	VOID				MoveCheckMarkNo();

	VOID				RecipeLoad();
	VOID				VacuumOnOff();
	VOID				ExposeDirect();
	VOID				ExposeAlign();
	VOID				ResetData();
	VOID				InitDispMark();
	VOID				DispResize(CWnd* pWnd[4]);

/* 공용 함수 */
public:

	VOID				DrawMarkData();	/* 마크 측정 결과를 화면에 출력 */
	

/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
 	afx_msg VOID		OnBtnClick(UINT32 id);
};
