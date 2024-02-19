
#pragma once

#include "../comn/MacButton.h"

class CChkCtrl : public CMacCheckBox
{
/* 생성자 & 파괴자 */
public:

	/*
	 desc : 생성자
	 parm : None
	 retn : None
	*/
	CChkCtrl();

	/*
	 desc : 파괴자
	 parm : None
	 retn : None
	*/
	virtual ~CChkCtrl();


/* 가상 함수 */
protected:

	virtual VOID		LButtonDn() = 0;
	virtual VOID		LButtonUp() = 0;

/* 로컬 변수 */
protected:


/* 로컬 함수 */
protected:



/* 공용 함수 */
public:


/* 메시지 맵*/
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
/* 생성자 & 파괴자 */
public:

	CChkServoLock()
	{
		m_enDrvID	= en_mmdi_stage_x;
	}
	virtual ~CChkServoLock()	{}

/* 가상 함수 */
protected:

	VOID				LButtonDn()	{};
	VOID				LButtonUp();


/* 로컬 변수 */
protected:

	ENG_MMDI			m_enDrvID;

/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	VOID				PutDrvID(ENG_MMDI drv_id)	{	m_enDrvID = drv_id;	}

};

