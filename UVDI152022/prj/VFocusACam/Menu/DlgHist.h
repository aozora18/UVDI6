
#pragma once

#include "DlgMenu.h"

class CDlgHist : public CDlgMenu
{
public:

	CDlgHist(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgHist();


/* 가상함수 */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();

public:

	virtual VOID		UpdateCtrl();
	virtual VOID		ResetData();


/* 로컬 변수 */
protected:

	CHAR				m_szFileDOF[MAX_PATH_LEN];		/* DOF 측정 데이터 저장을 위한 파일 명 (경로 포함) */

	DOUBLE				m_dbMinMax[2][COLLECT_DATA_COUNT];

	/* Double Buffering 사용 */
	CDC					m_dcMemHist;
	CBitmap				m_bmMemHist, *m_bmHistOld;
	CRect				m_rMemHist;

	CMyStatic			m_pic_ctl[1];


/* 로컬 함수 */
protected:

	VOID				InitCtrl();

	BOOL				InitMemDC();
	VOID				CloseMemDC();
	VOID				DrawHist();


/* 공용 함수 */
public:



/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()

};
