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
	
	/* 타이틀 바 정의 */
	enum EnTitleCtrl
	{
		eTITLE_MOTOR = 0,
		eTITLE_CONTROL,
		eTITLE_OPERATION,
		eTITLE_MAX
	};

	/* 그리드명 정의 */
	enum EnGrdCtrl
	{
		eGRD_MOTOR = 0,					// DI Master에 연결된 Motor를 표시
		eGRD_CONTROL,					// Motor의 초기화, 에러 리셋을 수행한다.
		eGRD_OPERATION_TAB,				// Motor 이동 시 이동 방식을 선택한다.
		eGRD_OPERATION_INPUT,			// Motor 이동 동작의 기본 입력값(위치, 속도)을 입력받는다.
		eGRD_MAX
	};

	/* 모터 그리드의 셀 정의 */
	enum EnCellMotor
	{
		eCELL_MOTOR_LED_STATUS = 0,		// 현재 모터의 상태를 LED로 표시한다.
		eCELL_MOTOR_MOTOR_NAME,			// 모터의 이름을 표시한다.
		eCELL_MOTOR_POS_VALUE,			// 현재 모터의 위치를 표시한다.
		eCELL_MOTOR_POS_UNIT,			// 모터 위치의 단위 값을 표시한다.
		eCELL_MOTOR_MAX
	};

	/* 컨트롤 그리드의 셀 정의 */
	enum EnCellControl
	{
		eCELL_CTRL_INITIALIZE = 0,		// 선택된 모터 초기화
		eCELL_CTRL_SERVO_ON_OFF,		// 선택된 모터 On/Off
		eCELL_CTRL_ERROR_RESET,			// 선택된 모터 에러 리셋
		eCELL_CTRL_MAX
	};

	/* 동작 형태 설정 탭 그리드의 셀 정의 */
	enum EnCellTab
	{
		eCELL_TAB_ABSOLUTE_MOVE = 0,	// 절대 이동 모드
		eCELL_TAB_RELATIVE_MOVE,		// 상대 이동 모드
		eCELL_TAB_MAX
	};

	/* 입력란 그리드의 셀 정의 */
	enum EnCellInput
	{
		eCELL_INPUT_SPEED = 0,			// 설정 속도
		eCELL_INPUT_POSITION,			// 설정 위치 값
		eCELL_INPUT_ROW_MAX,

		eCELL_INPUT_NAME = 0,			// 입력란 라벨
		eCELL_INPUT_VALUE,				// 입력 칸
		eCELL_INPUT_UNIT,				// 입력될 파라미터의 단위 값
		eCELL_INPUT_COL_MAX
	};

	/* 동작 버튼명 정의 */
	enum EnMoveButton
	{
		eBTN_PLUS = 0,					// + 방향으로 이동 명령
		eBTN_MINUS,						// - 방향으로 이동 명령
		eBTN_STOP,						// 정지 명령
		eBTN_MAX
	};

	/* CDlgMotr에서만 사용할 기타 정의 내용 */
	enum EnDlgMotrDefine
	{
		DEF_NONE_SEL = -1,
		DEF_TIMER_MOTOR_CONSOLE = 1,
		DEF_DEFAULT_ACC = 300,
		DEF_RGB_TAB_NORMAL = RGB(166, 166, 166),	// Tab에서 선택되지 않았을 때의 RGB 값
		DEF_RGB_TAB_SELECT = RGB(89, 89, 89),		// Tab에서 선택되었을 때의 RGB 값
		DEF_RGB_LABEL = RGB(217, 225, 242),			// 일반적인 라벨 RGB 값
		DEF_RGB_LABEL_SEL = RGB(130, 156, 213),		// 선택된 라벨 RGB 값
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

/* 가상 함수 */
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


/* 로컬 변수 */
protected:

	CGridCtrl*			m_pGrid[eGRD_MAX];		/* 그리드 컨트롤 선언 */
	CMacButton*			m_pButton[eBTN_MAX];	/* 버튼 컨트롤 선언 */
	CMyStatic			m_sttTitle[eTITLE_MAX];	/* 타이틀 바 선언 */

	vector<ST_MOTION>	m_stVctMotion;			/* 모션 정보 */

	UINT8				m_u8ACamCount;			/* 설정된 카메라 개수 */
	UINT8				m_u8HeadCount;			/* 설정된 헤드 개수 */
	UINT8				m_u8StageCount;			/* 설정된 테이블 개수 */
	UINT8				m_u8AllMotorCount;		/* 전체 모터 개수 */

	UINT8				m_u8SelMotor;			/* 현재 선택된 모터 */

	BOOL				m_bMoveType;			/* TRUE : REL, FALSE : ABS  만약 JOG까지 사용하고자 한다면 int형으로 형변환 필요 */
	double				m_dSetSpeed;			/* 동작 시 사용될 설정 속도 */
	double				m_dSetPosition;			/* 동작 시 사용될 설정 위치 */

/* 로컬 함수 */
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

/* 공용 함수 */
public:
	VOID				MoveStart(ENG_MMDI drv_id, double dPosition, double dSpeed, BOOL bIsRel = FALSE);
	LOGFONT				GetLogFont(int nSize, BOOL bIsBold);

/* 메시지 함수 */
protected:

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	
	afx_msg void OnClickButtonEvent(UINT ID);
	afx_msg void OnGrdClickedEvent(UINT ID, NMHDR* pNotifyStruct, LRESULT* pResult);


/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
};
