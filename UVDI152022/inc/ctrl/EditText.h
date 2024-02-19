
#pragma once

#include "../comn/MyEdit.h"

class CEditText : public CMyEdit
{
/* 생성자 & 파괴자 */
public:

	/*
	 desc : 생성자
	 parm : None
	 retn : None
	*/
	CEditText();

	/*
	 desc : 파괴자
	 parm : None
	 retn : None
	*/
	virtual ~CEditText();


/* 가상 함수 */
protected:

	virtual VOID		PreSubclassWindow();


/* 메시지 맵*/
protected:

	DECLARE_MESSAGE_MAP()
};

/* --------------------------------------------------------------------------------------------- */
/*                       Object 3801 PDO (Reference Value Collective Object)                     */
/* --------------------------------------------------------------------------------------------- */

class CEditMC2RefCmd : public CEditText
{
/* 생성자 & 파괴자 */
public:

	CEditMC2RefCmd()	{}
	virtual ~CEditMC2RefCmd()	{}

/* 공용 함수 */
public:

	VOID				PutCmd(UINT8 cmd_num);
};

class CEditMC2RefParam : public CEditText
{
/* 생성자 & 파괴자 */
public:

	CEditMC2RefParam()	{}
	virtual ~CEditMC2RefParam()	{}


/* 공용 함수 */
public:

	VOID				PutParam(UINT8 cmd, UINT8 pos, INT32 val);
};

/* --------------------------------------------------------------------------------------------- */
/*                        Object 3802 PDO (Actual Value Collective Object)                       */
/* --------------------------------------------------------------------------------------------- */


class CEditMC2ActPos : public CEditText
{
/* 생성자 & 파괴자 */
public:

	CEditMC2ActPos()	{}
	virtual ~CEditMC2ActPos()	{}

/* 로컬 변수 */
protected:

	DOUBLE				m_dbPos;

/* 공용 함수 */
public:

	VOID				PutPos(DOUBLE val);
	DOUBLE				GetPos()	{	return m_dbPos;	}
};

class CEditMC2ActFlag : public CEditText
{
/* 생성자 & 파괴자 */
public:

	CEditMC2ActFlag()	{}
	virtual ~CEditMC2ActFlag()	{}

/* 로컬 변수 */
protected:

	UINT8				m_u8OnOff;

/* 공용 함수 */
public:

	VOID				PutOnOff(UINT8 val);
	UINT8				GetOnOff()	{	return m_u8OnOff;	}
};

class CEditMC2ActResult : public CEditText
{
/* 생성자 & 파괴자 */
public:

	CEditMC2ActResult()	{}
	virtual ~CEditMC2ActResult()	{}

/* 로컬 변수 */
protected:

	INT32				m_i32Result;

/* 공용 함수 */
public:

	VOID				PutResult(INT32 val);
	INT32				GetResult()	{	return m_i32Result;	}
};

class CEditMC2ActError : public CEditText
{
/* 생성자 & 파괴자 */
public:

	CEditMC2ActError()	{}
	virtual ~CEditMC2ActError()	{}

/* 로컬 변수 */
protected:

	INT16				m_i16Error;

/* 공용 함수 */
public:

	VOID				PutError(UINT16 val);
	INT16				GetError()	{	return m_i16Error;	}
};

class CEditMC2PosErr : public CEditText
{
/* 생성자 & 파괴자 */
public:

	CEditMC2PosErr()
	{
		m_u32MaxErr	= 0;
		m_i32PrvPos	= INT32_MAX;
		m_enDrvID	= en_mmdi_stage_x;
	}
	virtual ~CEditMC2PosErr()	{}

/* 로컬 변수 */
protected:

	ENG_MMDI			m_enDrvID;

	INT32				m_i32PrvPos;	/* 이전에 측정된 위치 값 (단위: nm) */
	UINT32				m_u32MaxErr;	/* 현재까지 계산된 위치 에러 값 중 가장 큰 값 (단위: um) */

/* 공용 함수 */
public:

	VOID				SetDrive(ENG_MMDI drv_id)	{	m_enDrvID	= drv_id;	}
	VOID				UpdatePos();
	VOID				ResetPos();
};
