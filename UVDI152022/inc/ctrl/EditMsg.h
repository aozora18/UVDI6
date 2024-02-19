
#pragma once

#include "../comn/MyEdit.h"

class CEditMsg : public CMyEdit
{
/* ������ & �ı��� */
public:

	/*
	 desc : ������
	 parm : None
	 retn : None
	*/
	CEditMsg();

	/*
	 desc : �ı���
	 parm : None
	 retn : None
	*/
	virtual ~CEditMsg();


/* ���� �Լ� */
protected:

	virtual VOID		MouseMove(UINT32 flags, CPoint point) = 0;
	virtual LRESULT		NcHitTest(CPoint point) = 0;

	virtual VOID		PreSubclassWindow();


/* �޽��� ��*/
protected:

	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnMouseMove(UINT32 flags, CPoint point);
	afx_msg LRESULT		OnNcHitTest(CPoint point);
};

/* --------------------------------------------------------------------------------------------- */
/*                                        MC2 Error Message                                      */
/* --------------------------------------------------------------------------------------------- */

class CEditSD2SErr : public CEditMsg
{
/* ������ & �ı��� */
public:

	CEditSD2SErr()
	{
		m_pEdtMsg	= NULL;
	}
	virtual ~CEditSD2SErr()	{}

/* ���� �Լ� */
protected:

	virtual VOID		MouseMove(UINT32 flags, CPoint point)	{};
	virtual LRESULT		NcHitTest(CPoint point);

/* ���� ���� */
protected:

	CMyEdit				*m_pEdtMsg;

/* ���� �Լ� */
public:

	VOID				SetMsgEditObj(CMyEdit *edt_obj)	{	m_pEdtMsg	= edt_obj;	}
};
