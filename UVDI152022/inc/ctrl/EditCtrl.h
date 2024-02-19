
#pragma once

#include "../comn/MyEdit.h"

class CEditCtrl : public CMyEdit
{
/* ������ & �ı��� */
public:

	/*
	 desc : ������
	 parm : None
	 retn : None
	*/
	CEditCtrl();

	/*
	 desc : �ı���
	 parm : None
	 retn : None
	*/
	virtual ~CEditCtrl();


/* ���� �Լ� */
protected:

	virtual VOID		LButtonDn() = 0;
	virtual VOID		PreSubclassWindow();

/* ���� ���� */
protected:


/* ���� �Լ� */
protected:
	

/* ���� �Լ� */
public:

	VOID				SetReadOnly(BOOL enable);


/* �޽��� ��*/
protected:

	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnLButtonDown(UINT32 flags, CPoint point);
};

/* --------------------------------------------------------------------------------------------- */
/*                                   Number Input Dialog Box                                     */
/* --------------------------------------------------------------------------------------------- */

class CEditNum : public CEditCtrl
{
/* ������ & �ı��� */
public:

	CEditNum()
	{

#if USE_KEYBOARD_TYPE_TOUCH
		m_bPopKBDN	= TRUE;
#endif
	}
	virtual ~CEditNum()	{}


/* ���� �Լ� */
protected:

	virtual VOID		LButtonDn();

/* ������ */
protected:


/* ���� ���� */
protected:

#if USE_KEYBOARD_TYPE_TOUCH
	BOOL				m_bPopKBDN;	/* Ű���� �޽��� �ڽ� ��� ���� */
#endif


/* ���� �Լ� */
protected:

#if USE_KEYBOARD_TYPE_TOUCH
	BOOL				PopupKBDN(BOOL output);
#endif

/* ���� �Լ� */
public:


#if USE_KEYBOARD_TYPE_TOUCH
	VOID				SetDisableKBDN()				{	m_bPopKBDN	= FALSE;	}
	VOID				SetEnableKBDN()					{	m_bPopKBDN	= TRUE;		}
#endif
};

/* --------------------------------------------------------------------------------------------- */
/*                                         String Display                                        */
/* --------------------------------------------------------------------------------------------- */

class CEditString : public CEditCtrl
{
/* ������ & �ı��� */
public:

	CEditString()
	{
	}
	virtual ~CEditString()	{}


/* ���� �Լ� */
protected:

	virtual VOID		LButtonDn()	{};


/* ���� ���� */
protected:


/* ���� �Լ� */
public:

};

/* --------------------------------------------------------------------------------------------- */
/*                                      Grabbed Mark Method                                      */
/* --------------------------------------------------------------------------------------------- */

class CEditGrab : public CEditNum
{
/* ������ & �ı��� */
public:

	CEditGrab()
	{
		m_u8GrabMode	= 0x00;
	}
	virtual ~CEditGrab()	{}


/* ���� �Լ� */
protected:

	virtual VOID		LButtonDn();
	virtual VOID		PreSubclassWindow();

/* ���� ���� */
protected:

	UINT8				m_u8GrabMode;	/* 0x00 : Pattern Match, 0x01 : Edge Detected, 0x03 : Viewing Mode */

/* ���� �Լ� */
public:

	UINT8				GetGrabMode()	{	return m_u8GrabMode;	};

};

/* --------------------------------------------------------------------------------------------- */
/*                                   String Input Dialog Box                                     */
/* --------------------------------------------------------------------------------------------- */

class CEditTxt : public CEditCtrl
{
/* ������ & �ı��� */
public:

	CEditTxt()
	{
		m_i32MaxStr	= 32;
	}
	virtual ~CEditTxt()	{}


/* ���� �Լ� */
protected:

	virtual VOID		LButtonDn();


/* ���� ���� */
protected:

	INT32				m_i32MaxStr;


/* ���� �Լ� */
public:

	VOID				SetInputMaxStr(INT32 value)			{	m_i32MaxStr	= value;	}

};

/* --------------------------------------------------------------------------------------------- */
/*                                         Unit Display                                          */
/* --------------------------------------------------------------------------------------------- */

class CEditUnit : public CEditNum
{
/* ������ & �ı��� */
public:

	CEditUnit()
	{
		m_dbValue	= 0.0f;
		wmemset(m_tzUnit, 0x00, 32);
	}
	virtual ~CEditUnit()	{}


/* ���� �Լ� */
protected:

	virtual VOID		LButtonDn();


/* ���� ���� */
protected:

	TCHAR				m_tzUnit[32];

/* ���� �Լ� */
public:

	VOID				SetUnit(TCHAR *unit)		{	wcscpy_s(m_tzUnit, 32, unit);	}
};


/* --------------------------------------------------------------------------------------------- */
/*                                   Expose Step Size (Luria)                                    */
/* --------------------------------------------------------------------------------------------- */

class CEditStepSize : public CEditCtrl
{
/* ������ & �ı��� */
public:

	CEditStepSize()
	{
		m_u8Step	= 0x01;
	}
	virtual ~CEditStepSize()
	{
	}


/* ���� �Լ� */
protected:

	virtual VOID		LButtonDn();
	virtual VOID		PreSubclassWindow();


/* ���� ���� */
protected:

	UINT8				m_u8Step;	/* 0x01 or 0x02 - Step Size */

/* ���� �Լ� */
protected:


/* ���� �Լ� */
public:

	UINT8				GetStepSize()	{	return m_u8Step;	};
	VOID				SetStepSize(UINT8 step);
};

/* --------------------------------------------------------------------------------------------- */
/*                                   Vision Mark Reverse Setup                                   */
/* --------------------------------------------------------------------------------------------- */

class CEditMarkReverse : public CEditCtrl
{
/* ������ & �ı��� */
public:

	CEditMarkReverse()
	{
		m_u8Type	= 0x00;
	}
	virtual ~CEditMarkReverse()
	{
	}


/* ���� �Լ� */
protected:

	virtual VOID		LButtonDn();
	virtual VOID		PreSubclassWindow();


/* ���� ���� */
protected:

	UINT8				m_u8Type;

/* ���� �Լ� */
protected:


/* ���� �Լ� */
public:

	UINT8				GetType()	{	return m_u8Type;	};
};

/* --------------------------------------------------------------------------------------------- */
/*                                   Match Option - Speed Type                                   */
/* --------------------------------------------------------------------------------------------- */

class CEditSpeedType : public CEditCtrl
{
/* ������ & �ı��� */
public:

	CEditSpeedType()
	{
		m_u8Type	= 0x00;
	}
	virtual ~CEditSpeedType()
	{
	}


/* ���� �Լ� */
protected:

	virtual VOID		LButtonDn();
	virtual VOID		PreSubclassWindow();


/* ���� ���� */
protected:

	UINT8				m_u8Type;	/* 0x00 : Very Low, 0x01 : Low, 0x02 : Medium, 0x03 : High, 0x04 : Very High */

/* ���� �Լ� */
protected:


/* ���� �Լ� */
public:

	UINT8				GetType()	{	return m_u8Type;	};
	VOID				SetType(UINT8 type);
};

/* --------------------------------------------------------------------------------------------- */
/*                                 Match Option - Accuracy Type                                  */
/* --------------------------------------------------------------------------------------------- */

class CEditAccuracyType : public CEditCtrl
{
/* ������ & �ı��� */
public:

	CEditAccuracyType()
	{
		m_u8Type	= 0x01;
	}
	virtual ~CEditAccuracyType()
	{
	}


/* ���� �Լ� */
protected:

	virtual VOID		LButtonDn();
	virtual VOID		PreSubclassWindow();


/* ���� ���� */
protected:

	UINT8				m_u8Type;	/* 0x01 : Low, 0x02 : Medium, 0x03 : High */

/* ���� �Լ� */
protected:


/* ���� �Լ� */
public:

	UINT8				GetType()	{	return m_u8Type;	};
	VOID				SetType(UINT8 type);
};

/* --------------------------------------------------------------------------------------------- */
/*                               Edge Detection - Threshold Type                                 */
/* --------------------------------------------------------------------------------------------- */

class CEditThreshold : public CEditCtrl
{
/* ������ & �ı��� */
public:

	CEditThreshold()
	{
		m_u8Type	= 0x01;
	}
	virtual ~CEditThreshold()
	{
	}


/* ���� �Լ� */
protected:

	virtual VOID		LButtonDn();
	virtual VOID		PreSubclassWindow();


/* ���� ���� */
protected:

	UINT8				m_u8Type;	/* 0x01 : Very High, 0x02 : High, 0x03 : Medium, 4 : Low */

/* ���� �Լ� */
protected:


/* ���� �Լ� */
public:

	UINT8				GetType()	{	return m_u8Type;	};
	VOID				SetType(UINT8 type);
};

/* --------------------------------------------------------------------------------------------- */
/*                                    Mark Model Definition                                      */
/* --------------------------------------------------------------------------------------------- */

class CEditMarkType : public CEditCtrl
{
/* ������ & �ı��� */
public:

	CEditMarkType()
	{
		m_enType	= ENG_MMDT::en_circle;
	}
	virtual ~CEditMarkType()
	{
	}


/* ���� �Լ� */
protected:

	virtual VOID		LButtonDn();
	virtual VOID		PreSubclassWindow();


/* ���� ���� */
protected:

	ENG_MMDT			m_enType;

/* ���� �Լ� */
protected:


/* ���� �Լ� */
public:

	ENG_MMDT			GetType()	{	return m_enType;	};
	VOID				SetType(ENG_MMDT type);
};

/* --------------------------------------------------------------------------------------------- */
/*                                      Mark Model Color                                         */
/* --------------------------------------------------------------------------------------------- */

class CEditMarkColor : public CEditCtrl
{
/* ������ & �ı��� */
public:

	CEditMarkColor()
	{
		m_u32Color	= 0x00000100 /* Black */;
	}
	virtual ~CEditMarkColor()
	{
	}


/* ���� �Լ� */
protected:

	virtual VOID		LButtonDn();
	virtual VOID		PreSubclassWindow();


/* ���� ���� */
protected:

	UINT32				m_u32Color;	/* 128 : White, 256 : Black */

/* ���� �Լ� */
protected:


/* ���� �Լ� */
public:

	UINT32				GetColor()	{	return m_u32Color;	};
	VOID				SetColor(UINT32 color);
};


/* --------------------------------------------------------------------------------------------- */
/*                                Auto increment of numbers                                      */
/* --------------------------------------------------------------------------------------------- */

class CEditAutoNum : public CEditNum
{
/* ������ & �ı��� */
public:

	CEditAutoNum()
	{
		m_u8Type	= 0x00;

		m_i32Period	= 1;
		m_u32Period	= 1;
		m_dbPeriod	= 1.0f;
	}
	virtual ~CEditAutoNum()	{}


/* ���� �Լ� */
protected:

	virtual VOID		LButtonDn();


/* ���� ���� */
protected:

	INT32				m_i32Period;	/* �����Ǵ� �� (��� or ����) */
	UINT32				m_u32Period;	/* �����Ǵ� �� (��� or ����) */
	DOUBLE				m_dbPeriod;		/* �����Ǵ� �� (��� or ����) */

	UINT8				m_u8Type;	/* 0x00 - INT32, 0x01 - UINT32, 0x02 - DOUBLE */
	UINT8				m_u8Sign;	/* Sing ��� ���� */
	UINT8				m_u8Point;	/* Floating Points Count */


/* ���� �Լ� */
protected:


/* ���� �Լ� */
public:

	/* ���� �� ���� */
	VOID				PutPeriod(INT32 value)	{	m_i32Period	= value;	}
	VOID				PutPeriod(UINT32 value)	{	m_u32Period	= value;	}
	VOID				PutPeriod(DOUBLE value)	{	m_dbPeriod	= value;	}
	/* ���� ���� */
	VOID				PutType(UINT8 type)		{	m_u8Type	= type;		}
	VOID				PutPoint(UINT8 point)	{	m_u8Point	= point;	}
	VOID				PutSign(UINT8 sign)		{	m_u8Sign	= sign;		}
};


/* --------------------------------------------------------------------------------------------- */
/*                                  Output String : Yes or No                                    */
/* --------------------------------------------------------------------------------------------- */

class CEditYesNo : public CEditCtrl
{
/* ������ & �ı��� */
public:

	CEditYesNo()
	{
		m_u8YesNo	= 0x00;	/* Default */
	}
	virtual ~CEditYesNo()
	{
	}


/* ���� �Լ� */
protected:

	virtual VOID		LButtonDn();
	virtual VOID		PreSubclassWindow();


/* ���� ���� */
protected:

	UINT8				m_u8YesNo;	/* 0x01 : Yes, 0x00 : No */

/* ���� �Լ� */
protected:


/* ���� �Լ� */
public:

	UINT8				GetYesNo()	{	return m_u8YesNo;	};
	VOID				PutYesNo(UINT8 yesno);
};
