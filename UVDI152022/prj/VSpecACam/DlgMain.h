
#pragma once

#include "../../inc/comn/MyDialog.h"
#include "../../inc/comn/MacButton.h"
#include "../../inc/comn/MyGrpBox.h"
#include "../../inc/comn/MyStatic.h"
#include "../../inc/comn/MyEdit.h"
#include "../../inc/grid/GridCtrl.h"

class CMainThread;

class CMeasure;

class CDlgMain : public CMyDialog
{
public:
	CDlgMain(CWnd* parent = NULL);
	enum { IDD = IDD_MAIN };

/* ���� �Լ� */
protected:

	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();
	virtual VOID		UpdatePeriod();

/* ����ü ���� */
protected:

#pragma pack (push, 8)
	typedef struct __st_result_angle_pixel_value__
	{
		DOUBLE			value[4];	/* ���� �ε��� �������, ȸ�� ���� (����: degree), �ȼ� ũ�� (����: um), ��� ���� (����: %), ũ�� ���� (����: %) */

	}	STM_RAPV,	*LPG_RAPV;
#pragma pack (pop)

/* ���� ���� */
protected:

	INT32				m_i32ACamZAxisPos;	/* Align Camera Z Axis �̵� ��ġ (Position �̵��� �ش�) */

	UINT32				m_u32ACamZAxisRow;	/* Grid Control�� Row ���� �ε��� �� (1-based)*/

	UINT32				m_u32SkipCount;		/* Skip Counting �� ���� */
	UINT32				m_u32FocusRepeat;	/* ���� ������ Z Axis ��ġ���� ��Ŀ�� �ݺ� ������ Ƚ�� ���� */
	UINT32				m_u32UpDnRepeat;	/* m_u32FocusRepeat ���� ������, ���� Z Axis ��ġ ������ Ƚ�� ���� */

	UINT64				m_u64DrawTime;		/* ���� �ֱ��� Draw Time �ӽ� ���� */

	HWND				m_hWndView;			/* Picture Control�� ���� �ڵ� */

	/* ���� ��� ����� ���� �ӽ� ���� */
	CAtlList <STM_RAPV>	m_lstResult;		/* ����� ī�޶� ��ġ ���� */

	/* Normal */
	CMyGrpBox			m_grp_ctl[10];
	CMyStatic			m_txt_ctl[15];
	CMyStatic			m_pic_ctl[1];
	CMacButton			m_btn_ctl[12];

	/* for RunMode */
	CMacCheckBox		m_chk_run[2];

	/* for MC2 */
	CMacCheckBox		m_chk_mc2[8];	/* Drv No 1 ~ 8 */
	CMyEdit				m_edt_mc2[2];	/* Dist / Velo */

	/* for Align Camera */
	CMacCheckBox		m_chk_cam[5];	/* Drv No 1 ~ 4, Live */
	CMyEdit				m_edt_cam[1];

	/* for Cali Result */
	CMyEdit				m_edt_rst[8];

	/* for Trigger Time */
	CMyEdit				m_edt_trg[3];	/* strobe, triger, delay */


	CMainThread			*m_pMainThread;
	CGridCtrl			*m_pGridPos;	/* for Motor Position */


/* ���� �Լ� */
protected:

	VOID				InitCtrl();

	VOID				InitSetup();

	BOOL				InitLib();
	VOID				CloseLib();

	VOID				InitGrid();
	VOID				CloseGrid();

	VOID				UpdateCtrlACam();
	VOID				UpdateMotorPos();
	VOID				UpdatePLCVal();
	VOID				UpdateEnableCtrl();
	VOID				UpdateLiveView();
	VOID				UpdateStepRate();

	VOID				UpdateCaliInit();
	VOID				UpdateCaliResult(BOOL flag);

	VOID				MC2Recovery();
	VOID				ACamZAxisRecovery();

	VOID				MotionMove(ENG_MDMD direct);
	VOID				ACamMove(ENG_MDMD direct);

	VOID				StartSpec();
	VOID				StopSpec();

	VOID				MovePosMeasure();
	VOID				SetLiveView();

	VOID				LoadDataConfig();
	VOID				SaveDataToFile();

	UINT8				GetCheckACam();
	UINT8				GetTrigChNo();
	VOID				LoadTriggerTime(UINT8 cam_id);

	VOID				CalcNormalDist(DOUBLE rate=2.0f);

	VOID				MarkGrabbedResult();
	VOID				SetRegistModel();

	VOID				InvalidateView();


/* ���� �Լ� */
public:



/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT		OnMsgThread(WPARAM wparam, LPARAM lparam);
	afx_msg VOID		OnSysCommand(UINT32 id, LPARAM lparam);
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnChkClickACam(UINT32 id);
};
