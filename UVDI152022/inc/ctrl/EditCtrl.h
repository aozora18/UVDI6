
#pragma once

#include "../comn/MyEdit.h"

class CEditCtrl : public CMyEdit
{
/* 생성자 & 파괴자 */
public:

	/*
	 desc : 생성자
	 parm : None
	 retn : None
	*/
	CEditCtrl();

	/*
	 desc : 파괴자
	 parm : None
	 retn : None
	*/
	virtual ~CEditCtrl();


/* 가상 함수 */
protected:

	virtual VOID		LButtonDn() = 0;
	virtual VOID		PreSubclassWindow();

/* 로컬 변수 */
protected:


/* 로컬 함수 */
protected:
	

/* 공용 함수 */
public:

	VOID				SetReadOnly(BOOL enable);


/* 메시지 맵*/
protected:

	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnLButtonDown(UINT32 flags, CPoint point);
};

/* --------------------------------------------------------------------------------------------- */
/*                                   Number Input Dialog Box                                     */
/* --------------------------------------------------------------------------------------------- */

class CEditNum : public CEditCtrl
{
/* 생성자 & 파괴자 */
public:

	CEditNum()
	{

#if USE_KEYBOARD_TYPE_TOUCH
		m_bPopKBDN	= TRUE;
#endif
	}
	virtual ~CEditNum()	{}


/* 가상 함수 */
protected:

	virtual VOID		LButtonDn();

/* 열거형 */
protected:


/* 로컬 변수 */
protected:

#if USE_KEYBOARD_TYPE_TOUCH
	BOOL				m_bPopKBDN;	/* 키보드 메시지 박스 사용 유무 */
#endif


/* 로컬 함수 */
protected:

#if USE_KEYBOARD_TYPE_TOUCH
	BOOL				PopupKBDN(BOOL output);
#endif

/* 공용 함수 */
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
/* 생성자 & 파괴자 */
public:

	CEditString()
	{
	}
	virtual ~CEditString()	{}


/* 가상 함수 */
protected:

	virtual VOID		LButtonDn()	{};


/* 로컬 변수 */
protected:


/* 공용 함수 */
public:

};

/* --------------------------------------------------------------------------------------------- */
/*                                      Grabbed Mark Method                                      */
/* --------------------------------------------------------------------------------------------- */

class CEditGrab : public CEditNum
{
/* 생성자 & 파괴자 */
public:

	CEditGrab()
	{
		m_u8GrabMode	= 0x00;
	}
	virtual ~CEditGrab()	{}


/* 가상 함수 */
protected:

	virtual VOID		LButtonDn();
	virtual VOID		PreSubclassWindow();

/* 로컬 변수 */
protected:

	UINT8				m_u8GrabMode;	/* 0x00 : Pattern Match, 0x01 : Edge Detected, 0x03 : Viewing Mode */

/* 공용 함수 */
public:

	UINT8				GetGrabMode()	{	return m_u8GrabMode;	};

};

/* --------------------------------------------------------------------------------------------- */
/*                                   String Input Dialog Box                                     */
/* --------------------------------------------------------------------------------------------- */

class CEditTxt : public CEditCtrl
{
/* 생성자 & 파괴자 */
public:

	CEditTxt()
	{
		m_i32MaxStr	= 32;
	}
	virtual ~CEditTxt()	{}


/* 가상 함수 */
protected:

	virtual VOID		LButtonDn();


/* 로컬 변수 */
protected:

	INT32				m_i32MaxStr;


/* 공용 함수 */
public:

	VOID				SetInputMaxStr(INT32 value)			{	m_i32MaxStr	= value;	}

};

/* --------------------------------------------------------------------------------------------- */
/*                                         Unit Display                                          */
/* --------------------------------------------------------------------------------------------- */

class CEditUnit : public CEditNum
{
/* 생성자 & 파괴자 */
public:

	CEditUnit()
	{
		m_dbValue	= 0.0f;
		wmemset(m_tzUnit, 0x00, 32);
	}
	virtual ~CEditUnit()	{}


/* 가상 함수 */
protected:

	virtual VOID		LButtonDn();


/* 로컬 변수 */
protected:

	TCHAR				m_tzUnit[32];

/* 공용 함수 */
public:

	VOID				SetUnit(TCHAR *unit)		{	wcscpy_s(m_tzUnit, 32, unit);	}
};


/* --------------------------------------------------------------------------------------------- */
/*                                   Expose Step Size (Luria)                                    */
/* --------------------------------------------------------------------------------------------- */

class CEditStepSize : public CEditCtrl
{
/* 생성자 & 파괴자 */
public:

	CEditStepSize()
	{
		m_u8Step	= 0x01;
	}
	virtual ~CEditStepSize()
	{
	}


/* 가상 함수 */
protected:

	virtual VOID		LButtonDn();
	virtual VOID		PreSubclassWindow();


/* 로컬 변수 */
protected:

	UINT8				m_u8Step;	/* 0x01 or 0x02 - Step Size */

/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	UINT8				GetStepSize()	{	return m_u8Step;	};
	VOID				SetStepSize(UINT8 step);
};

/* --------------------------------------------------------------------------------------------- */
/*                                   Vision Mark Reverse Setup                                   */
/* --------------------------------------------------------------------------------------------- */

class CEditMarkReverse : public CEditCtrl
{
/* 생성자 & 파괴자 */
public:

	CEditMarkReverse()
	{
		m_u8Type	= 0x00;
	}
	virtual ~CEditMarkReverse()
	{
	}


/* 가상 함수 */
protected:

	virtual VOID		LButtonDn();
	virtual VOID		PreSubclassWindow();


/* 로컬 변수 */
protected:

	UINT8				m_u8Type;

/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	UINT8				GetType()	{	return m_u8Type;	};
};

/* --------------------------------------------------------------------------------------------- */
/*                                   Match Option - Speed Type                                   */
/* --------------------------------------------------------------------------------------------- */

class CEditSpeedType : public CEditCtrl
{
/* 생성자 & 파괴자 */
public:

	CEditSpeedType()
	{
		m_u8Type	= 0x00;
	}
	virtual ~CEditSpeedType()
	{
	}


/* 가상 함수 */
protected:

	virtual VOID		LButtonDn();
	virtual VOID		PreSubclassWindow();


/* 로컬 변수 */
protected:

	UINT8				m_u8Type;	/* 0x00 : Very Low, 0x01 : Low, 0x02 : Medium, 0x03 : High, 0x04 : Very High */

/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	UINT8				GetType()	{	return m_u8Type;	};
	VOID				SetType(UINT8 type);
};

/* --------------------------------------------------------------------------------------------- */
/*                                 Match Option - Accuracy Type                                  */
/* --------------------------------------------------------------------------------------------- */

class CEditAccuracyType : public CEditCtrl
{
/* 생성자 & 파괴자 */
public:

	CEditAccuracyType()
	{
		m_u8Type	= 0x01;
	}
	virtual ~CEditAccuracyType()
	{
	}


/* 가상 함수 */
protected:

	virtual VOID		LButtonDn();
	virtual VOID		PreSubclassWindow();


/* 로컬 변수 */
protected:

	UINT8				m_u8Type;	/* 0x01 : Low, 0x02 : Medium, 0x03 : High */

/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	UINT8				GetType()	{	return m_u8Type;	};
	VOID				SetType(UINT8 type);
};

/* --------------------------------------------------------------------------------------------- */
/*                               Edge Detection - Threshold Type                                 */
/* --------------------------------------------------------------------------------------------- */

class CEditThreshold : public CEditCtrl
{
/* 생성자 & 파괴자 */
public:

	CEditThreshold()
	{
		m_u8Type	= 0x01;
	}
	virtual ~CEditThreshold()
	{
	}


/* 가상 함수 */
protected:

	virtual VOID		LButtonDn();
	virtual VOID		PreSubclassWindow();


/* 로컬 변수 */
protected:

	UINT8				m_u8Type;	/* 0x01 : Very High, 0x02 : High, 0x03 : Medium, 4 : Low */

/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	UINT8				GetType()	{	return m_u8Type;	};
	VOID				SetType(UINT8 type);
};

/* --------------------------------------------------------------------------------------------- */
/*                                    Mark Model Definition                                      */
/* --------------------------------------------------------------------------------------------- */

class CEditMarkType : public CEditCtrl
{
/* 생성자 & 파괴자 */
public:

	CEditMarkType()
	{
		m_enType	= ENG_MMDT::en_circle;
	}
	virtual ~CEditMarkType()
	{
	}


/* 가상 함수 */
protected:

	virtual VOID		LButtonDn();
	virtual VOID		PreSubclassWindow();


/* 로컬 변수 */
protected:

	ENG_MMDT			m_enType;

/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	ENG_MMDT			GetType()	{	return m_enType;	};
	VOID				SetType(ENG_MMDT type);
};

/* --------------------------------------------------------------------------------------------- */
/*                                      Mark Model Color                                         */
/* --------------------------------------------------------------------------------------------- */

class CEditMarkColor : public CEditCtrl
{
/* 생성자 & 파괴자 */
public:

	CEditMarkColor()
	{
		m_u32Color	= 0x00000100 /* Black */;
	}
	virtual ~CEditMarkColor()
	{
	}


/* 가상 함수 */
protected:

	virtual VOID		LButtonDn();
	virtual VOID		PreSubclassWindow();


/* 로컬 변수 */
protected:

	UINT32				m_u32Color;	/* 128 : White, 256 : Black */

/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	UINT32				GetColor()	{	return m_u32Color;	};
	VOID				SetColor(UINT32 color);
};


/* --------------------------------------------------------------------------------------------- */
/*                                Auto increment of numbers                                      */
/* --------------------------------------------------------------------------------------------- */

class CEditAutoNum : public CEditNum
{
/* 생성자 & 파괴자 */
public:

	CEditAutoNum()
	{
		m_u8Type	= 0x00;

		m_i32Period	= 1;
		m_u32Period	= 1;
		m_dbPeriod	= 1.0f;
	}
	virtual ~CEditAutoNum()	{}


/* 가상 함수 */
protected:

	virtual VOID		LButtonDn();


/* 로컬 변수 */
protected:

	INT32				m_i32Period;	/* 증가되는 값 (양수 or 음수) */
	UINT32				m_u32Period;	/* 증가되는 값 (양수 or 음수) */
	DOUBLE				m_dbPeriod;		/* 증가되는 값 (양수 or 음수) */

	UINT8				m_u8Type;	/* 0x00 - INT32, 0x01 - UINT32, 0x02 - DOUBLE */
	UINT8				m_u8Sign;	/* Sing 사용 여부 */
	UINT8				m_u8Point;	/* Floating Points Count */


/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	/* 증감 값 설정 */
	VOID				PutPeriod(INT32 value)	{	m_i32Period	= value;	}
	VOID				PutPeriod(UINT32 value)	{	m_u32Period	= value;	}
	VOID				PutPeriod(DOUBLE value)	{	m_dbPeriod	= value;	}
	/* 조건 설정 */
	VOID				PutType(UINT8 type)		{	m_u8Type	= type;		}
	VOID				PutPoint(UINT8 point)	{	m_u8Point	= point;	}
	VOID				PutSign(UINT8 sign)		{	m_u8Sign	= sign;		}
};


/* --------------------------------------------------------------------------------------------- */
/*                                  Output String : Yes or No                                    */
/* --------------------------------------------------------------------------------------------- */

class CEditYesNo : public CEditCtrl
{
/* 생성자 & 파괴자 */
public:

	CEditYesNo()
	{
		m_u8YesNo	= 0x00;	/* Default */
	}
	virtual ~CEditYesNo()
	{
	}


/* 가상 함수 */
protected:

	virtual VOID		LButtonDn();
	virtual VOID		PreSubclassWindow();


/* 로컬 변수 */
protected:

	UINT8				m_u8YesNo;	/* 0x01 : Yes, 0x00 : No */

/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	UINT8				GetYesNo()	{	return m_u8YesNo;	};
	VOID				PutYesNo(UINT8 yesno);
};
