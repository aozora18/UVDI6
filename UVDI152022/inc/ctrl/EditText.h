
#pragma once

#include "../comn/MyEdit.h"

class CEditText : public CMyEdit
{
/* ������ & �ı��� */
public:

	/*
	 desc : ������
	 parm : None
	 retn : None
	*/
	CEditText();

	/*
	 desc : �ı���
	 parm : None
	 retn : None
	*/
	virtual ~CEditText();


/* ���� �Լ� */
protected:

	virtual VOID		PreSubclassWindow();


/* �޽��� ��*/
protected:

	DECLARE_MESSAGE_MAP()
};

/* --------------------------------------------------------------------------------------------- */
/*                       Object 3801 PDO (Reference Value Collective Object)                     */
/* --------------------------------------------------------------------------------------------- */

class CEditMC2RefCmd : public CEditText
{
/* ������ & �ı��� */
public:

	CEditMC2RefCmd()	{}
	virtual ~CEditMC2RefCmd()	{}

/* ���� �Լ� */
public:

	VOID				PutCmd(UINT8 cmd_num);
};

class CEditMC2RefParam : public CEditText
{
/* ������ & �ı��� */
public:

	CEditMC2RefParam()	{}
	virtual ~CEditMC2RefParam()	{}


/* ���� �Լ� */
public:

	VOID				PutParam(UINT8 cmd, UINT8 pos, INT32 val);
};

/* --------------------------------------------------------------------------------------------- */
/*                        Object 3802 PDO (Actual Value Collective Object)                       */
/* --------------------------------------------------------------------------------------------- */


class CEditMC2ActPos : public CEditText
{
/* ������ & �ı��� */
public:

	CEditMC2ActPos()	{}
	virtual ~CEditMC2ActPos()	{}

/* ���� ���� */
protected:

	DOUBLE				m_dbPos;

/* ���� �Լ� */
public:

	VOID				PutPos(DOUBLE val);
	DOUBLE				GetPos()	{	return m_dbPos;	}
};

class CEditMC2ActFlag : public CEditText
{
/* ������ & �ı��� */
public:

	CEditMC2ActFlag()	{}
	virtual ~CEditMC2ActFlag()	{}

/* ���� ���� */
protected:

	UINT8				m_u8OnOff;

/* ���� �Լ� */
public:

	VOID				PutOnOff(UINT8 val);
	UINT8				GetOnOff()	{	return m_u8OnOff;	}
};

class CEditMC2ActResult : public CEditText
{
/* ������ & �ı��� */
public:

	CEditMC2ActResult()	{}
	virtual ~CEditMC2ActResult()	{}

/* ���� ���� */
protected:

	INT32				m_i32Result;

/* ���� �Լ� */
public:

	VOID				PutResult(INT32 val);
	INT32				GetResult()	{	return m_i32Result;	}
};

class CEditMC2ActError : public CEditText
{
/* ������ & �ı��� */
public:

	CEditMC2ActError()	{}
	virtual ~CEditMC2ActError()	{}

/* ���� ���� */
protected:

	INT16				m_i16Error;

/* ���� �Լ� */
public:

	VOID				PutError(UINT16 val);
	INT16				GetError()	{	return m_i16Error;	}
};

class CEditMC2PosErr : public CEditText
{
/* ������ & �ı��� */
public:

	CEditMC2PosErr()
	{
		m_u32MaxErr	= 0;
		m_i32PrvPos	= INT32_MAX;
		m_enDrvID	= en_mmdi_stage_x;
	}
	virtual ~CEditMC2PosErr()	{}

/* ���� ���� */
protected:

	ENG_MMDI			m_enDrvID;

	INT32				m_i32PrvPos;	/* ������ ������ ��ġ �� (����: nm) */
	UINT32				m_u32MaxErr;	/* ������� ���� ��ġ ���� �� �� ���� ū �� (����: um) */

/* ���� �Լ� */
public:

	VOID				SetDrive(ENG_MMDI drv_id)	{	m_enDrvID	= drv_id;	}
	VOID				UpdatePos();
	VOID				ResetPos();
};
