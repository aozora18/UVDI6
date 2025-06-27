
#pragma once

#include "resource.h"

#include "../../inc/comn/MyDialog.h"
#include "../../inc/comn/MyGrpBox.h"
#include "../../inc/comn/MyStatic.h"
#include "../../inc/comn/MacButton.h"
#include "../../inc/comn/ProgressCtrlX.h"
#include "../../inc/grid/GridCellCombo.h"
#include "../../inc/kybd/DlgKBDN.h"
#include "pops/DlgMotr.h"
#include "pops/DlgLoginMain.h"
#include "pops/DlgLoginAdmin.h"
#include "pops/DlgLoginManager.h"


class MsgRecver
{
public:
	virtual bool ProcessAction(UINT32 action) { return true; }
};

// by sys&j : 가독성을 위해 enum 추가
enum EnMainGrp
{
	eMAIN_GRP_JOB_RECIPE,
	eMAIN_GRP_EXPOSE_RECIPE,
	eMAIN_GRP_ALIGN_RECIPE,
	eMAIN_GRP_MAX,
};

enum EnMainPic
{
	eMAIN_PIC_CUSTOMER		,
	//eMAIN_PIC_PHIL		,
	eMAIN_PIC_MENU		,
	eMAIN_PIC_MAX
};

enum EnMainTxt
{
	eMAIN_TXT_STATUS	 ,
	eMAIN_TXT_MESSAGE	 ,
	eMAIN_TXT_VERSION	 ,
	/*eMAIN_TXT_GERB_RECIPE ,
	eMAIN_TXT_EXPOSE_RECIPE,
	eMAIN_TXT_ALIGN_RECIPE ,*/
	eMAIN_TXT_SERVER,
	eMAIN_TXT_MOTION,
	eMAIN_TXT_TRIGGER,
	eMAIN_TXT_PHILHMI,
	eMAIN_TXT_CAM1,
	eMAIN_TXT_CAM2,
	eMAIN_TXT_GENTEC,
	eMAIN_TXT_STROBE_LAMP,
	eMAIN_TXT_MAX
};

enum EnRichTxt
{
	eMAIN_RICHTXT_GERB_RECIPE =0,
	eMAIN_RICHTXT_EXPOSE_RECIPE,
	eMAIN_RICHTXT_ALIGN_RECIPE ,
	eMAIN_RICHTXT_MAX
};


enum EnMainBtn
{
	eMAIN_BTN_EXIT	 ,
	eMAIN_BTN_AUTO,
	eMAIN_BTN_MANUAL ,
	eMAIN_BTN_EXPO,
	eMAIN_BTN_GERB	 ,
	eMAIN_BTN_MARK	 ,
	eMAIN_BTN_CTRL	 ,
	eMAIN_BTN_LOGS	 ,
	eMAIN_BTN_CONF	 ,
	eMAIN_BTN_CALB	 ,
	eMAIN_BTN_PHILHMI	 ,
	eMAIN_BTN_STOP	 ,
	eMAIN_BTN_RESET	 ,
	eMAIN_BTN_LOGIN	 ,
	eMAIN_BTN_MODE	,
	eMAIN_BTN_MAX	 ,
};
// by sys&j : 가독성을 위해 enum 추가

class CImgUvdi15;
class CImgCustomer;
class CImgPhil;
class CMainThread;

class CDlgMenu;
class CDlgWait;

class CDlgLoginMain;
class CDlgLoginAdmin;
class CDlgLoginManager;

class CDlgMain : public CMyDialog
{
public:
	CDlgMain(CWnd* parent = NULL);
	enum { IDD = IDD_MAIN };

/* 가상 함수 */
protected:

	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();
	virtual void		RegisterUILevel();

	void RefreshRecipeTitleCaption();
	void SetRichTextColored(CRichEditCtrl* pCtrl, const CString& text,
		COLORREF colorLeft = RGB(255, 0, 0),
		COLORREF colorRight = RGB(0, 0, 255),
		TCHAR delimiter = _T('/'),bool boldLeft = true);
/* 로컬 변수 */
protected:

	UINT8				m_u8NextReqCmd;		/* 다음의 상태 정보 요청 순서 값 */
	UINT8				m_u8MsgLogNext;		/* 다음에 출력될 메시지 인덱스 */

	UINT64				m_u64TickPeriod;	/* 주기적으로 갱신한 최근 시간 저장 */
	UINT64				m_u64ReqCmdTime;	/* 가장 최근의 상태 정보 요청 시간 임시 저장 */
	UINT64				m_u64MsgLogTime;	/* 에러 로그 출력하는데 일정시간 대기 하기 위함 */

	UINT64				m_u64DelayTimeHMI;
	UINT64				m_u64StartTimeHMI;			/* 작업 시작 시간 저장 */

	CMainThread			*m_pMainThread;

	CMainThread* GetMainthreadPtr() 
	{
		return m_pMainThread;
	}

	BOOL				m_bMainBusy;
	/* Logo Images 출력 */
	CImgCustomer		*m_pImgCustomer;
	CImgPhil			*m_pImgPhil;

	CMyGrpBox			m_grp_ctl[eMAIN_GRP_MAX];
	CMyStatic			m_txt_ctl[eMAIN_TXT_MAX];
	CMyStatic			m_pic_ctl[eMAIN_PIC_MAX];
	CMacButton			m_btn_ctl[eMAIN_BTN_MAX];
	
	CRichEditCtrl		newNameList[eMAIN_RICHTXT_MAX];

	/* 자식 메뉴 화면 */
	ENG_CMDI			m_enDlgID;		/* 가장 최근에 선택된 다이얼로그 ID */
	CDlgMenu			*m_pDlgMenu;
	CDlgWait			*m_pDlgWait;
	CDlgMotr			*m_pDlgMotor;
	CDlgLoginMain		*m_pDlgLoginMain;
	CDlgLoginAdmin		*m_pDlgLoginAdmin;
	CDlgLoginManager	*m_pDlgLoginMgr;

public:
	/*노광 결과 기록*/
	STG_CELA			m_stExpoLog;
	STG_CPVE			m_stPhilStatus;

/* 로컬 함수 */
protected:

	VOID				InitCtrl();

	BOOL				LoadImages();

	BOOL				InitLib();
	VOID				CloseLib();

	VOID				UpdatePeriod(UINT64 tick, BOOL busy);
	VOID				UpdateControl(UINT64 tick, BOOL busy);
	VOID				UpdateReqState(UINT64 tick, BOOL is_busy);
	VOID				UpdateErrorMessage(UINT64 tick);
	VOID				ResetErrorMessage();
	VOID				UpdateCommState();
	VOID				UpdateLDSMeasure();
	VOID				UpdateSafePosCheck();
	VOID				UpdatePhilState();

	//BOOL				CreateMenu(UINT32 id);
	VOID				DeleteMenu();

	VOID				ExitApp();
	void				CreateUserFont();

	CString				m_strLog;
	VOID				txtWrite(CString msg);

/* 공용 함수 */
public:

	BOOL				RunWorkJob(ENG_BWOK job_id, PUINT64 data=NULL);
	VOID				WorkInit();
	VOID				WorkStop();
	BOOL				IsBusyWorkJob();
	ENG_BWOK			GetWorkJobID();
	BOOL				CreateMenu(UINT32 id);
	//VOID				PhilSendCreateRecipe(STG_PP_PACKET_RECV* stRecv);
	//VOID				PhilSendDelectRecipe(STG_PP_PACKET_RECV* stRecv);
	//VOID				PhilSendModifyRecipe(STG_PP_PACKET_RECV* stRecv);
	//VOID				PhilSendSelectRecipe(STG_PP_PACKET_RECV* stRecv);
	//VOID				PhilSendInfoRecipe(STG_PP_PACKET_RECV* stRecv);
	BOOL				PhilSendMoveRecvAck(STG_PP_PACKET_RECV* stRecv, BOOL is_busy);
	VOID				PhilSendMove(STG_PP_PACKET_RECV* stRecv, int AxisCount);
	VOID				PhilSendMoveComplete(STG_PP_PACKET_RECV* stRecv);
	//VOID				PhilSendStatusValue(STG_PP_PACKET_RECV* stRecv);
	VOID				PhilSendProcessExecute(STG_PP_PACKET_RECV* stRecv, BOOL is_busy);
	VOID				PhilSendChageMode(STG_PP_PACKET_RECV* stRecv);
	VOID				PhilSendInitialExecute(STG_PP_PACKET_RECV* stRecv, BOOL is_busy);
	VOID				PhilSendSubProcessExecute(STG_PP_PACKET_RECV* stRecv);
	VOID				PhilSendEventStatus(STG_PP_PACKET_RECV* stRecv);
	VOID				PhilSendEventNotify(STG_PP_PACKET_RECV* stRecv);
	VOID				PhilSendTimeSync(STG_PP_PACKET_RECV* stRecv);
	VOID				PhilSendInterruptStop(STG_PP_PACKET_RECV* stRecv);

/* 사용자 메시지 함수 */
protected:
	LRESULT				OnOpenMotorConsole(WPARAM wParam, LPARAM lParam);
	LRESULT				OnOpenLoginConsole(WPARAM wParam, LPARAM lParam);
	LRESULT				OnOpenAdminConsole(WPARAM wParam, LPARAM lParam);
	LRESULT				OnOpenManagerConsole(WPARAM wParam, LPARAM lParam);

/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT		OnMsgMainThread(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT		OnMsgMainChild(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT		OnMsgMainRecipeUpdate(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT		OnMsgMainProcessUpdate(WPARAM wparam, LPARAM lparam);
	// by sysandj : philhmi 관련 메세지 요청
	afx_msg LRESULT		OnMsgMainPHILHMI(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT		OnMsgMainPhilMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT		OnMsgMainStrobeLamp(WPARAM wparam, LPARAM lparam);
	// by sysandj : philhmi 관련 메세지 요청
	afx_msg LRESULT		OnMsgMainRecipeCreate(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT		OnMsgMainRecipeDelete(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT		OnMsgMainRecipeChange(WPARAM wparam, LPARAM lparam);
	afx_msg VOID		OnSysCommand(UINT32 id, LPARAM lparam);
	afx_msg VOID		OnBtnClicked(UINT32 id);
};
