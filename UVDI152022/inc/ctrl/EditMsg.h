
#pragma once

#include "../comn/MyEdit.h"

class CEditMsg : public CMyEdit
{
/* 생성자 & 파괴자 */
public:

	/*
	 desc : 생성자
	 parm : None
	 retn : None
	*/
	CEditMsg();

	/*
	 desc : 파괴자
	 parm : None
	 retn : None
	*/
	virtual ~CEditMsg();


/* 가상 함수 */
protected:

	virtual VOID		MouseMove(UINT32 flags, CPoint point) = 0;
	virtual LRESULT		NcHitTest(CPoint point) = 0;

	virtual VOID		PreSubclassWindow();


/* 메시지 맵*/
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
/* 생성자 & 파괴자 */
public:

	CEditSD2SErr()
	{
		m_pEdtMsg	= NULL;
	}
	virtual ~CEditSD2SErr()	{}

/* 가상 함수 */
protected:

	virtual VOID		MouseMove(UINT32 flags, CPoint point)	{};
	virtual LRESULT		NcHitTest(CPoint point);

/* 로컬 변수 */
protected:

	CMyEdit				*m_pEdtMsg;

/* 공용 함수 */
public:

	VOID				SetMsgEditObj(CMyEdit *edt_obj)	{	m_pEdtMsg	= edt_obj;	}
};
