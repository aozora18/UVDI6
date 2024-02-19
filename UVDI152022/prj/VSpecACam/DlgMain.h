
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

/* 가상 함수 */
protected:

	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();
	virtual VOID		UpdatePeriod();

/* 구조체 변수 */
protected:

#pragma pack (push, 8)
	typedef struct __st_result_angle_pixel_value__
	{
		DOUBLE			value[4];	/* 각각 인덱스 순서대로, 회전 각도 (단위: degree), 픽셀 크기 (단위: um), 모양 비율 (단위: %), 크기 비율 (단위: %) */

	}	STM_RAPV,	*LPG_RAPV;
#pragma pack (pop)

/* 로컬 변수 */
protected:

	INT32				m_i32ACamZAxisPos;	/* Align Camera Z Axis 이동 위치 (Position 이동만 해당) */

	UINT32				m_u32ACamZAxisRow;	/* Grid Control의 Row 행의 인덱스 값 (1-based)*/

	UINT32				m_u32SkipCount;		/* Skip Counting 값 저장 */
	UINT32				m_u32FocusRepeat;	/* 현재 동일한 Z Axis 위치에서 포커싱 반복 측정할 횟수 저장 */
	UINT32				m_u32UpDnRepeat;	/* m_u32FocusRepeat 끝날 때마다, 다음 Z Axis 위치 측정할 횟수 저장 */

	UINT64				m_u64DrawTime;		/* 가장 최근의 Draw Time 임시 저장 */

	HWND				m_hWndView;			/* Picture Control의 윈도 핸들 */

	/* 측정 결과 계산을 위한 임시 버퍼 */
	CAtlList <STM_RAPV>	m_lstResult;		/* 얼라인 카메라 설치 각도 */

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


/* 로컬 함수 */
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


/* 공용 함수 */
public:



/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT		OnMsgThread(WPARAM wparam, LPARAM lparam);
	afx_msg VOID		OnSysCommand(UINT32 id, LPARAM lparam);
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnChkClickACam(UINT32 id);
};
