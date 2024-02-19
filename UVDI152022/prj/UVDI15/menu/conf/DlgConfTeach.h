#pragma once
#include "../DlgSubMenu.h"
#include "../DlgSubTab.h"
#include "../../param/enum.h"
#include <vector>

#define DEF_UI_GRD_ROW_OFFSET	40
#define DEF_MODULE_NAME_SIZE_OF_MAX					40
#define DEF_MODULE_MOTOR_DIR_NAME_SIZE_OF_MAX		6
#define DEF_MODULE_MOTOR_PROCUCT_COUNT_OF_MAX		2
#define DEF_ACTION_IF_OPERATOR_COUNT_OF_MAX			10


#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
JHMI_ENUM(EN_SETTING_TEACH, int
	, TABLE_EXPO_START_X_1
	, TABLE_EXPO_START_Y_1
	, TABLE_UNLOADER_X_1
	, TABLE_UNLOADER_Y_1
	, PH_Z_AXIS_FOCUS_1
	, PH_Z_AXIS_FOCUS_2
	, PH_Z_AXIS_FOCUS_3
	, PH_Z_AXIS_FOCUS_4
	, PH_Z_AXIS_FOCUS_5
	, PH_Z_AXIS_FOCUS_6
	, ACAM_Z_FOCUS_1
	, ACAM_Z_FOCUS_2
	, ALIGN_MOVE_SPEED
	, MAX
)
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
JHMI_ENUM(EN_SETTING_TEACH, int
	, TABLE_EXPO_START_X_1
	, TABLE_EXPO_START_Y_1
	, TABLE_UNLOADER_X_1
	, TABLE_UNLOADER_Y_1
	, PH_Z_AXIS_FOCUS_1
	, PH_Z_AXIS_FOCUS_2
	, ACAM_Z_FOCUS_1
	, ACAM_Z_FOCUS_2
	, ACAM_Z_FOCUS_3
	, ALIGN_MOVE_SPEED
	, MAX
)
#endif


enum EnConfTeachBtn
{
	eCONF_TEACH_BTN_SAVE,
	eCONF_TEACH_BTN_CONTROL_PAANEL,
	eCONF_TEACH_BTN_PREV,
	eCONF_TEACH_BTN_NEXT,
	eCONF_TEACH_BTN_MAX,
};

enum EnConfTeachGrid
{
	eCONF_TEACH_GRID_MAIN1,
	eCONF_TEACH_GRID_MAIN2,
	eCONF_TEACH_GRID_MAX,
};

enum EN_SETTING_TEACH_LABEL
{
	eCONF_TEACH_LABEL_POSITION_NAME,
	eCONF_TEACH_LABEL_POSITION,
	eCONF_TEACH_LABEL_MAX
};

typedef struct _ST_TEACH_MOTOR_
{
	_ST_TEACH_MOTOR_()
	{
		Init();
	}

	BOOL	bIsUse;
	BOOL	bIsLock;
	BOOL	bIsHomming;
	WCHAR	strPositionName[DEF_MODULE_NAME_SIZE_OF_MAX];
	WCHAR	strMotorName[DEF_MODULE_NAME_SIZE_OF_MAX];

	int		nAxisID;
	int		nMotorType;
	int		nDirType;							// X:0, Z:1, Y:2
	int		nProductCount;
	int		nArrProductIndex[DEF_MODULE_MOTOR_PROCUCT_COUNT_OF_MAX];
	double	dPosition;
	double	dOffset;
	double	dRecipeOffset;
	double	dSpeed;
	double	dAcc;
	int		nScurve;
	int		nTimeout;

	void Init()
	{
		bIsUse = TRUE;
		bIsLock = FALSE;
		bIsHomming = FALSE;
		memset(strPositionName, 0, sizeof(strPositionName));
		memset(strMotorName, 0, sizeof(strMotorName));

		nAxisID = 0;
		nMotorType = 0;
		nDirType = 0;
		nProductCount = 0;
		memset(nArrProductIndex, -1, sizeof(int) * DEF_MODULE_MOTOR_PROCUCT_COUNT_OF_MAX);
		dPosition = 0;
		dOffset = 0;
		dRecipeOffset = 0;
		dSpeed = 10.0;
		dAcc = 100.0;
		nScurve = 0;
		nTimeout = 1000;
	}

	CString GetPositionName()
	{
		return strPositionName;
	};

	void SetPositionName(LPCTSTR strName)
	{
		swprintf_s(strPositionName, DEF_MODULE_NAME_SIZE_OF_MAX, _T("%s"), (wchar_t*)(const wchar_t*)strName);
	};

	CString GetMotorName()
	{
		return strMotorName;
	};

	void SetMotorName(LPCTSTR strName)
	{
		swprintf_s(strMotorName, DEF_MODULE_NAME_SIZE_OF_MAX, _T("%s"), (wchar_t*)(const wchar_t*)strName);
	};

	BOOL IsValidProductIndex(int nIndex)
	{
		for (int i = 0; i < nProductCount; i++)
		{
			if (nIndex == nArrProductIndex[i])
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	double GetPosition(double dAddOffset = 0.0, BOOL bIsReferOffset = FALSE)
	{
		if (bIsReferOffset)
		{
			return dPosition + dOffset + dRecipeOffset + dAddOffset;
		}
		else
		{
			return dPosition + dOffset + dAddOffset;
		}
	};
}ST_TEACH_MOTOR;

typedef std::vector < ST_TEACH_MOTOR > vLIST_MOTOR;

// CDlgConfTeach 대화 상자

class CDlgConfTeach : public CDlgSubMenu
{
	struct ST_GRID_TEACH_COL
	{
		CString strTitle;
		UINT	nFormat;
		int		nWidth;
	};
public:

	/* 생성자 / 파괴자 */
	CDlgConfTeach(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgConfTeach();


	/* 가상함수 선언 */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		UpdateControl(UINT64 tick, BOOL is_busy);


public:

	virtual BOOL		OnInitDlg();
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC* dc);
	virtual VOID		OnResizeDlg();
	virtual void		RegisterUILevel() {}
	virtual VOID		UpdatePeriod(UINT64 tick, BOOL is_busy);



	/* 로컬 변수 */
protected:

 	CMacButton			m_btn_ctl[eCONF_TEACH_BTN_MAX];
	CGridCtrl			m_pGrd[eCONF_TEACH_GRID_MAX];
	CDlgSubTab*			m_pDlgMenu;
	int					m_nMaxPage;
	int					m_nSelectPage;
	/* 로컬 함수 */
private:
	
protected:

	VOID				InitCtrl();

	BOOL				InitObject();
	VOID				CloseObject();

	VOID				InitGridTeach();
	VOID				UpdateGridTeach();

	BOOL				CreateMenu(UINT32 id);
	VOID				DeleteMenu();
	BOOL				PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts = 0);
	VOID				LoadSettingTeach();
	VOID				SaveSettingTeach();
	BOOL				GetConfigToTeachFile(int nIndex, double &dPosition);
	BOOL				ConfigToTeachFile(BOOL bLoadSave);
	void				ShowCtrlPanel();
	/* 공용 함수 */
public:
	vLIST_MOTOR			m_vTeachPosition;
	double				m_dTeachData[EN_SETTING_TEACH::MAX];

	/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClick(UINT32 id);
	afx_msg VOID		OnGrdTeachClick(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg VOID		OnGrdTeachClick2(NMHDR* pNotifyStruct, LRESULT* pResult);
};

