
#pragma once

#include "../../../inc/comn/MyDialog.h"

// by sys&j : �������� ���� enum �߰�
enum EnEdgeGrp
{
	eEDGE_GRP_MODEL_SIZE,
	eEDGE_GRP_MAX,
};

enum EnEdgeText
{
	eEDGE_TXT_SIZE_1,
	eEDGE_TXT_SIZE_2,
	eEDGE_TXT_SIZE_3,
	eEDGE_TXT_MAX,
};

enum EnEdgeBtn
{
	eEDGE_BTN_CANCEL,
	eEDGE_BTN_MAX
};

enum EnEdgePic
{
	eEDGE_PIC_MARK,
	eEDGE_PIC_MAX
};

enum EnEdgeEdt
{
	eEDGE_EDT_SIZE_1,
	eEDGE_EDT_SIZE_2,
	eEDGE_EDT_SIZE_3,
	eEDGE_EDT_MAX
};

enum EnEdgeBox
{
	eEDGE_BOX_RESULTS,
	eEDGE_BOX_MAX
};
// by sys&j : �������� ���� enum �߰�

class CDlgEdge : public CMyDialog
{
public:

	CDlgEdge(CWnd* parent = NULL);
	enum { IDD = IDD_EDGE };

// �����Լ� ����
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();
	virtual void		RegisterUILevel() {}

public:

	virtual VOID		UpdatePeriod()	{};

/* ���� ���� */
protected:

	UINT8				m_u8ACamID;		/* 1 or 2*/

	CMyGrpBox			m_grp_ctl[eEDGE_GRP_MAX];
	CMyStatic			m_pic_ctl[eEDGE_PIC_MAX];	/* Mark Drawing Area */
	CMyStatic			m_txt_ctl[eEDGE_TXT_MAX];
	CMyListBox			m_box_ctl[eEDGE_BOX_MAX];	/* Edge Detection Results */
	CMacButton			m_btn_ctl[eEDGE_BTN_MAX];	/* Normal */
	CMyEdit				m_edt_ctl[eEDGE_EDT_MAX];

	CImage				m_csImgMark;


/* ���� �Լ� */
protected:

	BOOL				InitCtrl();

	VOID				LoadData();
	VOID				DrawPattern(CDC *dc);
	VOID				InvalidateMark();


/* ���� �Լ� */
public:

	INT_PTR				MyDoModal(UINT8 cam_id);
	

/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnBoxSelChangeMark();
};
