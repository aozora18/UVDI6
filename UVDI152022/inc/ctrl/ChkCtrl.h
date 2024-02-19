
#pragma once

#include "../comn/MacButton.h"

class CChkCtrl : public CMacCheckBox
{
/* ������ & �ı��� */
public:

	/*
	 desc : ������
	 parm : None
	 retn : None
	*/
	CChkCtrl();

	/*
	 desc : �ı���
	 parm : None
	 retn : None
	*/
	virtual ~CChkCtrl();


/* ���� �Լ� */
protected:

	virtual VOID		LButtonDn() = 0;
	virtual VOID		LButtonUp() = 0;

/* ���� ���� */
protected:


/* ���� �Լ� */
protected:



/* ���� �Լ� */
public:


/* �޽��� ��*/
protected:

	DECLARE_MESSAGE_MAP()

	afx_msg VOID		OnLButtonDown(UINT32 flags, CPoint point);
	afx_msg VOID		OnLButtonUp(UINT32 flags, CPoint point);
};

/* --------------------------------------------------------------------------------------------- */
/*                                         MC2 Motor Control                                     */
/* --------------------------------------------------------------------------------------------- */

class CChkServoLock: public CChkCtrl
{
/* ������ & �ı��� */
public:

	CChkServoLock()
	{
		m_enDrvID	= en_mmdi_stage_x;
	}
	virtual ~CChkServoLock()	{}

/* ���� �Լ� */
protected:

	VOID				LButtonDn()	{};
	VOID				LButtonUp();


/* ���� ���� */
protected:

	ENG_MMDI			m_enDrvID;

/* ���� �Լ� */
protected:


/* ���� �Լ� */
public:

	VOID				PutDrvID(ENG_MMDI drv_id)	{	m_enDrvID = drv_id;	}

};

