#pragma once

// #include "../../../inc/conf/global.h"
// #include "../../../inc/conf/define.h"
// #include "../../../inc/comn/AniGif.h/"

#include "../../../inc/comn/MyDialog.h"
#include "../../../inc/grid/GridCtrl.h"
#include "../../../inc/kybd/DlgKBDN.h"

using namespace std;

class CDlgMotr : public CMyDialog
{
	DECLARE_DYNAMIC(CDlgMotr)

public:
	CDlgMotr(CWnd* pParent = NULL);
	virtual ~CDlgMotr();

	enum { IDD = IDD_MOTR };
	
	/* Ÿ��Ʋ �� ���� */
	enum EnTitleCtrl
	{
		eTITLE_MOTOR = 0,
		eTITLE_CONTROL,
		eTITLE_OPERATION,
		eTITLE_MAX
	};

	/* �׸���� ���� */
	enum EnGrdCtrl
	{
		eGRD_MOTOR = 0,					// DI Master�� ����� Motor�� ǥ��
		eGRD_CONTROL,					// Motor�� �ʱ�ȭ, ���� ������ �����Ѵ�.
		eGRD_OPERATION_TAB,				// Motor �̵� �� �̵� ����� �����Ѵ�.
		eGRD_OPERATION_INPUT,			// Motor �̵� ������ �⺻ �Է°�(��ġ, �ӵ�)�� �Է¹޴´�.
		eGRD_MAX
	};

	/* ���� �׸����� �� ���� */
	enum EnCellMotor
	{
		eCELL_MOTOR_LED_STATUS = 0,		// ���� ������ ���¸� LED�� ǥ���Ѵ�.
		eCELL_MOTOR_MOTOR_NAME,			// ������ �̸��� ǥ���Ѵ�.
		eCELL_MOTOR_POS_VALUE,			// ���� ������ ��ġ�� ǥ���Ѵ�.
		eCELL_MOTOR_POS_UNIT,			// ���� ��ġ�� ���� ���� ǥ���Ѵ�.
		eCELL_MOTOR_MAX
	};

	/* ��Ʈ�� �׸����� �� ���� */
	enum EnCellControl
	{
		eCELL_CTRL_INITIALIZE = 0,		// ���õ� ���� �ʱ�ȭ
		eCELL_CTRL_SERVO_ON_OFF,		// ���õ� ���� On/Off
		eCELL_CTRL_ERROR_RESET,			// ���õ� ���� ���� ����
		eCELL_CTRL_MAX
	};

	/* ���� ���� ���� �� �׸����� �� ���� */
	enum EnCellTab
	{
		eCELL_TAB_ABSOLUTE_MOVE = 0,	// ���� �̵� ���
		eCELL_TAB_RELATIVE_MOVE,		// ��� �̵� ���
		eCELL_TAB_MAX
	};

	/* �Է¶� �׸����� �� ���� */
	enum EnCellInput
	{
		eCELL_INPUT_SPEED = 0,			// ���� �ӵ�
		eCELL_INPUT_POSITION,			// ���� ��ġ ��
		eCELL_INPUT_ROW_MAX,

		eCELL_INPUT_NAME = 0,			// �Է¶� ��
		eCELL_INPUT_VALUE,				// �Է� ĭ
		eCELL_INPUT_UNIT,				// �Էµ� �Ķ������ ���� ��
		eCELL_INPUT_COL_MAX
	};

	/* ���� ��ư�� ���� */
	enum EnMoveButton
	{
		eBTN_PLUS = 0,					// + �������� �̵� ���
		eBTN_MINUS,						// - �������� �̵� ���
		eBTN_STOP,						// ���� ���
		eBTN_MAX
	};

	/* CDlgMotr������ ����� ��Ÿ ���� ���� */
	enum EnDlgMotrDefine
	{
		DEF_NONE_SEL = -1,
		DEF_TIMER_MOTOR_CONSOLE = 1,
		DEF_DEFAULT_ACC = 300,
		DEF_RGB_TAB_NORMAL = RGB(166, 166, 166),	// Tab���� ���õ��� �ʾ��� ���� RGB ��
		DEF_RGB_TAB_SELECT = RGB(89, 89, 89),		// Tab���� ���õǾ��� ���� RGB ��
		DEF_RGB_LABEL = RGB(217, 225, 242),			// �Ϲ����� �� RGB ��
		DEF_RGB_LABEL_SEL = RGB(130, 156, 213),		// ���õ� �� RGB ��
	};

	enum EN_MSG_BOX_TYPE
	{
		eINFO,
		eSTOP,
		eWARN,
		eQUEST,
	};

	struct ST_MOTION
	{
		ENG_MMDI DeviceNum;
		CString strMotorName;
	};

/* ���� �Լ� */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* pDX);
	virtual VOID		OnExitDlg()		{};
	virtual VOID		OnPaintDlg(CDC *pDc);
	virtual VOID		OnResizeDlg()	{};
	virtual void		RegisterUILevel() {}
public:

	virtual VOID		UpdatePeriod()	{};


/* ���� ���� */
protected:

	CGridCtrl*			m_pGrid[eGRD_MAX];		/* �׸��� ��Ʈ�� ���� */
	CMacButton*			m_pButton[eBTN_MAX];	/* ��ư ��Ʈ�� ���� */
	CMyStatic			m_sttTitle[eTITLE_MAX];	/* Ÿ��Ʋ �� ���� */

	vector<ST_MOTION>	m_stVctMotion;			/* ��� ���� */

	UINT8				m_u8ACamCount;			/* ������ ī�޶� ���� */
	UINT8				m_u8HeadCount;			/* ������ ��� ���� */
	UINT8				m_u8StageCount;			/* ������ ���̺� ���� */
	UINT8				m_u8AllMotorCount;		/* ��ü ���� ���� */

	UINT8				m_u8SelMotor;			/* ���� ���õ� ���� */

	BOOL				m_bMoveType;			/* TRUE : REL, FALSE : ABS  ���� JOG���� ����ϰ��� �Ѵٸ� int������ ����ȯ �ʿ� */
	double				m_dSetSpeed;			/* ���� �� ���� ���� �ӵ� */
	double				m_dSetPosition;			/* ���� �� ���� ���� ��ġ */

/* ���� �Լ� */
protected:

	VOID				CreateControl();

	VOID				InitMotionIndex();
	VOID				InitMotorGrid();
	VOID				InitControlGrid();
	VOID				InitOpTabGrid();
	VOID				InitOpInputGrid();

	VOID				ChangeMoveTpye(UINT8 u8Type);
	VOID				InputParameter(UINT8 u8Sel);
	VOID				MotorSelect(UINT8 u8Sel);
	VOID				MotorControl(UINT8 u8Sel);
	VOID				UpdateMotorStatus();
	int					ShowMultiSelectMsg(EN_MSG_BOX_TYPE mType, CString strTitle, CStringArray& strArrCommand);

/* ���� �Լ� */
public:
	VOID				MoveStart(ENG_MMDI drv_id, double dPosition, double dSpeed, BOOL bIsRel = FALSE);
	LOGFONT				GetLogFont(int nSize, BOOL bIsBold);

/* �޽��� �Լ� */
protected:

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	
	afx_msg void OnClickButtonEvent(UINT ID);
	afx_msg void OnGrdClickedEvent(UINT ID, NMHDR* pNotifyStruct, LRESULT* pResult);


/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
};
