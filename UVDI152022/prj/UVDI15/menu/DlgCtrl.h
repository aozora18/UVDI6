
#pragma once

#include "DlgMenu.h"
#include "../param/enum.h"
#include "../param/IOManager.h"

#define DEF_UI_CTRL_STT		IDC_CTRL_STT_ALL_MOTOR
#define DEF_UI_CTRL_GRD		(IDC_CTRL_STT_ALL_MOTOR + 10)
#define DEF_UI_CTRL_BTN		(IDC_CTRL_STT_ALL_MOTOR + 30)

#define DEF_UI_OFFSET	5

// by sys&j : 가독성을 위해 enum 추가
JHMI_ENUM(EN_CTRL_BTN, int
	, ALL_STOP
	, ALL_HOMING
	, ALL_RESET
	, OPEN_CONTROL_PANEL
	, RESET_TRIGGER
	, HEAD_HWINIT
	, STROBE_LAMP
	, PREV
	, UPDATE
	, NEXT
)


JHMI_ENUM(EN_CTRL_STT, int
	, ALL_MOTOR
	, MOTOR_STATUS
	, MOTOR_CONTROL
	, ETC_CONTROL
	, IO_QUICK
	, IO_STATUS
)

/* 모터 그리드의 셀 정의 */
JHMI_ENUM(EN_MOTOR_STATUS, int,
	MOTOR_NAME,
	POS,
	STATUS,
	SERVO,
	HOME
	)


JHMI_ENUM(EN_IO_STATUS, int,
	ADDR,
	NAME,
	STATUS
)

JHMI_ENUM(EN_IO_QUICK, int,
	PARAM1,
	PARAM2
)

// by sys&j : 가독성을 위해 enum 추가

class CDlgCtrl : public CDlgMenu
{
	enum EN_CTRL_GRD
	{
		eGRD_MOTOR_LIST,
		eGRD_IO_LIST,
		eGRD_IO_QUICK1,
		eGRD_IO_QUICK2,
		eGRD_IO_QUICK3,
		eGRD_IO_QUICK4,
		eGRD_IO_QUICK5,
		eGRD_IO_QUICK6,
		eGRD_IO_QUICK7,
		eGRD_IO_QUICK8,
		eGRD_IO_QUICK9,
		eGRD_IO_QUICK10,
		eGRD_MAX
	};

	struct ST_MOTION
	{
		ENG_MMDI DeviceNum;
		CString strMotorName;
	};


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
	virtual void		RegisterUILevel() {}
	virtual VOID		UpdatePeriod(UINT64 tick, BOOL is_busy);


/* 로컬 변수 */
protected:

	CGridCtrl*			m_pGrd[eGRD_MAX];
	CMacButton*			m_pBtn[EN_CTRL_BTN::_size()];		/* Normal */
	CMyStatic*			m_pStt[EN_CTRL_STT::_size()];		/* Text - Normal */

	UINT8				m_u8ACamCount;			/* 설정된 카메라 개수 */
	UINT8				m_u8HeadCount;			/* 설정된 헤드 개수 */
	UINT8				m_u8StageCount;			/* 설정된 테이블 개수 */
	UINT8				m_u8AllMotorCount;		/* 전체 모터 개수 */

	vector<ST_MOTION>	m_vMotor;				/* 모션 정보 */
	vector<ST_IO>		m_vQuickIO;
	vector<ST_QUICK_IO_ACT>		m_vQuickIO2;

	int					m_nPage;
	int					m_nMaxPage;
	
/* 로컬 함수 */
protected:

	VOID				InitCtrl();
	VOID				InitValue();

	BOOL				InitGrid();
	VOID				CloseGrid();

	VOID				InitMotionIndex();
	VOID				InitMotorGrid();
	VOID				UpdateMotorStatus();

	COLORREF			GetIOColorBg(CString strColor);
	COLORREF			GetIOColorFg(CString strColor);
	VOID				InitIOGrid();
	VOID				UpdateIOStatus();
	VOID				InitQuickIOGrid();
	VOID				UpdateQuickIOStatus();

	VOID				SetMotionHoming(ENG_MMDI drv_id);
	VOID				SetTriggerReset();
	VOID				SetHeadHWInit();

	void				ShowCtrlPanel();

	void				StrobeLampSampleFunction();


	void				OnGrdMotorList(NMHDR* pNotifyStruct);
	void				OnGrdIOQuick(int nGrdIndex, NMHDR* pNotifyStruct);
/* 공용 함수 */
public:

	

/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
 	afx_msg VOID	OnBtnClick(UINT32 id);
	afx_msg void	OnGrdClick(UINT ID, NMHDR* pNotifyStruct, LRESULT* pResult);
};
