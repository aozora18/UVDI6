
#pragma once

#include "DlgMenu.h"

class CGridLuriaMC2;
class CGridLuriaPC;
class CGridLuriaOverall;
class CGridLuriaLedSource;
class CGridLuriaPHStat;
class CGridLuriaJob;
class CGridLuriaPHInfo;
class CGridDriveStat;
class CGridSD2Error;
class CGridMC2Error;
class CGridTempDI;

// by sys&j : 가독성을 위해 enum 추가
enum EnStateGrp
{
	eSTATE_GRP_PH_STATUS		,
	eSTATE_GRP_PH_MOTOR			,
	eSTATE_GRP_PH_PC			,
	eSTATE_GRP_PH_OVERALL		,
	eSTATE_GRP_PH_LIGHT			,
	eSTATE_GRP_PH_COMM			,
	eSTATE_GRP_JOB_STATUS		,
	eSTATE_GRP_PH_INFO			,
	eSTATE_GRP_PH_NO			,
	eSTATE_GRP_HDD_INFO			,
	eSTATE_GRP_MOTION_STATUS	,
	eSTATE_GRP_DRIVE_STATUS		,
	eSTATE_GRP_SD2_ERROR		,
	eSTATE_GRP_MC2_ERROR		,
	eSTATE_GRP_PLC_DI_TEMP		,
	eSTATE_GRP_MAX,
};

enum EnStateBtn
{
	eSTATE_BTN_PH_NO_UP,
	eSTATE_BTN_PH_NO_DN,
	eSTATE_BTN_REFRESH,
	eSTATE_BTN_MAX,
};

enum EnStateTxt
{
	eSTATE_TXT_HDD_TOTAL,
	eSTATE_TXT_HDD_SPARE,
	eSTATE_TXT_MAX,
};

enum EnStateEdtInt
{
	eSTATE_EDT_PH_NO,
	eSTATE_EDT_INT_MAX,
};

enum EnStateEdtFloat
{
	eSTATE_EDT_HDD_TOTAL,
	eSTATE_EDT_HDD_SPARE,
	eSTATE_EDT_FLOAT_MAX,
};
// by sys&j : 가독성을 위해 enum 추가


class CDlgStat : public CDlgMenu
{
public:

	/* 생성자 / 파괴자 */
	CDlgStat(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgStat();


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

	UINT8				m_u8NextCmd;
	UINT8				m_u8NextRefresh;
	UINT64				m_u64ReqTime;

	CMyGrpBox			m_grp_ctl[eSTATE_GRP_MAX];
	CMacButton			m_btn_ctl[eSTATE_BTN_MAX];
	CMyStatic			m_txt_ctl[eSTATE_TXT_MAX];
	CEditNum			m_edt_int[eSTATE_EDT_INT_MAX];
	CEditNum			m_edt_flt[eSTATE_EDT_FLOAT_MAX];

	CGridLuriaMC2		*m_pGridLuriaMC2;
	CGridLuriaPC		*m_pGridLuriaPC;
	CGridLuriaOverall	*m_pGridLuriaOverall;
	CGridLuriaLedSource	*m_pGridLuriaLedSource;
	CGridLuriaPHStat	*m_pGridLuriaPHStat;
	CGridLuriaJob		*m_pGridLuriaJob;
	CGridLuriaPHInfo	*m_pGridLuriaPHInfo;
	CGridDriveStat		*m_pGridDriveStat;
	CGridSD2Error		*m_pGridSD2Error;
	CGridMC2Error		*m_pGridMC2Error;
	CGridTempDI			*m_pGridTempDI;


/* 로컬 함수 */
protected:

	VOID				InitCtrl();

	BOOL				InitObject();
	VOID				CloseObject();

	BOOL				InitGrid();
	VOID				CloseGrid();
	VOID				PHRefresh(INT8 fact);
	VOID				HDDRefresh();
	VOID				UpdateRefresh();

/* 공용 함수 */
public:

	

/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
 	afx_msg VOID		OnBtnClick(UINT32 id);
};
