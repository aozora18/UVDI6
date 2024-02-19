
#pragma once

#include "../comn/MacButton.h"

class CBtnCtrl : public CMacButton
{
/* 생성자 & 파괴자 */
public:

	/*
	 desc : 생성자
	 parm : None
	 retn : None
	*/
	CBtnCtrl();

	/*
	 desc : 파괴자
	 parm : None
	 retn : None
	*/
	virtual ~CBtnCtrl();


/* 가상 함수 */
protected:

	virtual BOOL		LButtonDn() = 0;
	virtual BOOL		LButtonUp() = 0;

/* 로컬 변수 */
protected:

	UINT32				m_wMsgBtnPull;			/* 버튼 메시지 팝업 알림 */
	BOOL				m_bLButtonDnClicked;	/* 마우스 왼쪽 버튼이 클릭했는지 여부 */

/* 로컬 함수 */
protected:

	VOID				PushBtnEvent();	/* 상위 부모에게 이벤트 알림 */
	VOID				PullBtnEvent();	/* 상위 부모에게 이벤트 알림 */

/* 공용 함수 */
public:

	VOID				PullBtnMsgID(UINT32 wparam)	{	m_wMsgBtnPull = wparam;	};

/* 메시지 맵*/
protected:

	DECLARE_MESSAGE_MAP()

	afx_msg VOID		OnLButtonDown(UINT32 flags, CPoint point);
	afx_msg VOID		OnLButtonUp(UINT32 flags, CPoint point);

};

/* --------------------------------------------------------------------------------------------- */
/*                                      Button Numbering                                         */
/* --------------------------------------------------------------------------------------------- */

class CBtnNum : public CBtnCtrl
{
/* 생성자 & 파괴자 */
public:

	CBtnNum()
	{
		m_i64Num	= 0;
		m_i64Max	= INT_MAX;
		m_i64Min	= INT_MIN;
	}
	virtual ~CBtnNum()
	{
	}

/* 가상 함수 */
protected:

	virtual BOOL		LButtonDn()	{	return TRUE;	};
	virtual BOOL		LButtonUp();


/* 로컬 변수 */
protected:

	INT64				m_i64Num;
	INT64				m_i64Max;
	INT64				m_i64Min;


/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	INT64				GetNum()	{	return m_i64Num;	}
	VOID				SetInit(INT64 start, INT64 max_i, INT64 min_i);
};

/* --------------------------------------------------------------------------------------------- */
/*                                 Melsec PLC Switch On / Off                                    */
/* --------------------------------------------------------------------------------------------- */

class CBtnPLCOnOff : public CBtnCtrl
{
/* 생성자 & 파괴자 */
public:

	CBtnPLCOnOff()
	{
		m_u32Addr		= 0x00000000;
		m_u8OnOff		= 0xff;	/* 무조건 최대 값으로 설정 */
		m_u32AddrLast	= 0xffffffff;
	}
	virtual ~CBtnPLCOnOff()
	{
	}

/* 가상 함수 */
protected:

	virtual VOID		PreSubclassWindow();
	virtual BOOL		LButtonDn()	{	return TRUE;	};
	virtual BOOL		LButtonUp();


/* 로컬 변수 */
protected:

	UINT8				m_u8OnOff;	/* 초기 값은 쓰레기 값으로 0xff 설정 */

	TCHAR				m_tzTitle[64];

	UINT32				m_u32Addr;
	UINT32				m_u32AddrLast;	/* 마지막으로 변경된 주소 */


/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	VOID				SetAddr(UINT32 addr)	{	m_u32Addr	= addr;	m_u8OnOff = 0xff;		};
	UINT32				GetAddr()				{	return m_u32Addr;		};
	VOID				SetOnOff(UINT8 value);
	VOID				UpdateValue(BOOL disable);
};


/* --------------------------------------------------------------------------------------------- */
/*                                             MC2 All                                           */
/* --------------------------------------------------------------------------------------------- */

class CBtnDevAll : public CBtnCtrl
{
/* 생성자 & 파괴자 */
public:

	CBtnDevAll()
	{
		m_enCtrl	= ENG_MCCW::en_none;
	}
	virtual ~CBtnDevAll()	{}

/* 가상 함수 */
protected:

	virtual BOOL		LButtonDn()	{	return TRUE;	};
	virtual BOOL		LButtonUp();


/* 로컬 변수 */
protected:

	ENG_MCCW			m_enCtrl;


/* 공용 함수 */
public:

	VOID				PutCtrl(ENG_MCCW ctrl)	{	m_enCtrl = ctrl;	};

};

/* --------------------------------------------------------------------------------------------- */
/*                                           MC2 Homing                                          */
/* --------------------------------------------------------------------------------------------- */

class CBtnDevHome : public CBtnCtrl
{
/* 생성자 & 파괴자 */
public:

	CBtnDevHome()
	{
		m_enDrvID	= ENG_MMDI(0xff);
	}
	virtual ~CBtnDevHome()	{}

/* 가상 함수 */
protected:

	virtual BOOL		LButtonDn()	{	return TRUE;	};
	virtual BOOL		LButtonUp();


/* 로컬 변수 */
protected:

	ENG_MMDI			m_enDrvID;

/* 공용 함수 */
public:

	VOID				PutCtrl(ENG_MMDI drv_id)	{	m_enDrvID = drv_id;	};

};


/* --------------------------------------------------------------------------------------------- */
/*                                       Stage Moving - XY                                       */
/* --------------------------------------------------------------------------------------------- */

class CBtnDevMove : public CBtnCtrl
{
/* 생성자 & 파괴자 */
public:

	CBtnDevMove()
	{
		m_enSysID	= ENG_EDIC::en_mc2;
		m_enDrvID	= ENG_MMDI::en_stage_x;
		m_enDirect	= ENG_MDMD::en_none;
		m_enMethod	= ENG_MMMM::en_none;
		m_bJogEnable= TRUE;
	}
	virtual ~CBtnDevMove()	{}

/* 가상 함수 */
protected:

	virtual BOOL		LButtonDn();
	virtual BOOL		LButtonUp();


/* 로컬 변수 */
protected:

	BOOL				m_bJogEnable;	/* Jog 기능 Enable 유무 */

	ENG_EDIC			m_enSysID;
	ENG_MMDI			m_enDrvID;
	ENG_MDMD			m_enDirect;
	ENG_MMMM			m_enMethod;		/* Jog or Step */

/* 로컬 함수 */
protected:



/* 공용 함수 */
public:

	VOID				PutMethod(ENG_MMMM method)	{	m_enMethod	= method;	};
	VOID				PutDirect(ENG_MDMD direct)	{	m_enDirect	= direct;	};
	VOID				PutDrvID(ENG_MMDI drv_id)	{	m_enDrvID	= drv_id;	};
	VOID				PutSysID(ENG_EDIC sys_id)	{	m_enSysID	= sys_id;	};
	VOID				EnableJog(BOOL enable)		{	m_bJogEnable= enable;	};
};

#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
/* --------------------------------------------------------------------------------------------- */
/*                                    Axis Moving - Up / Down                                    */
/* --------------------------------------------------------------------------------------------- */

class CBtnACamUpDn : public CBtnCtrl
{
/* 생성자 & 파괴자 */
public:

	CBtnACamUpDn()
	{
		m_enDrvID	= ENG_MMDI::en_axis_acam1;
		m_enDirect	= ENG_MDMD::en_none;
	}
	virtual ~CBtnACamUpDn()	{}

/* 가상 함수 */
protected:

	virtual BOOL		LButtonDn();
	virtual BOOL		LButtonUp()	{	return TRUE; };


/* 로컬 변수 */
protected:

	ENG_MMDI			m_enDrvID;
	ENG_MDMD			m_enDirect;

/* 로컬 함수 */
protected:



/* 공용 함수 */
public:

	VOID				PutDirect(ENG_MDMD direct)	{	m_enDirect	= direct;	};
	VOID				PutDrvID(ENG_MMDI drv_id)	{	m_enDrvID	= drv_id;	};
	BOOL				RunButtonDn()				{	return LButtonDn();		}
};
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                         String Change                                         */
/* --------------------------------------------------------------------------------------------- */
class CBtnString : public CBtnCtrl
{
/* 생성자 & 파괴자 */
public:

	CBtnString()
	{
		m_u8Num	= 1;
		m_u8Max	= 2;
		m_u8Min	= 1;
	}
	virtual ~CBtnString()
	{
		m_arrText.RemoveAll();
	}

/* 가상 함수 */
protected:

	virtual BOOL		LButtonDn()	{	return TRUE;	};
	virtual BOOL		LButtonUp();


/* 로컬 변수 */
protected:

	UINT8				m_u8Num;
	UINT8				m_u8Max;
	UINT8				m_u8Min;

	CStringArray		m_arrText;	/*  버튼 영역에 출력되는 문자열 저장 */

/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	INT64				GetNum()			{	return m_u8Num;	}
	VOID				SetNum(UINT8 num)	{	m_u8Num = num;	}
	VOID				PutString(TCHAR *title);
	VOID				SetInit();
	VOID				PutMin(UINT8 min)	{	m_u8Min	= min;	};
	VOID				PutMax(UINT8 max)	{	m_u8Max	= max;	};
};

/* --------------------------------------------------------------------------------------------- */
/*                                 Button Align Camera Number                                    */
/* --------------------------------------------------------------------------------------------- */

class CBtnACamNum : public CBtnCtrl
{
/* 생성자 & 파괴자 */
public:

	CBtnACamNum()
	{
		m_u8ACamNum		= 1;
	}
	virtual ~CBtnACamNum()
	{
	}

/* 가상 함수 */
protected:

	virtual BOOL		LButtonDn()	{	return TRUE;	};
	virtual BOOL		LButtonUp();


/* 로컬 변수 */
protected:

	UINT8				m_u8ACamNum;	/* 1 or 2 */


/* 로컬 함수 */
protected:

	VOID				SetTextACam(UINT8 cam_no);

/* 공용 함수 */
public:

	UINT8				GetACamNum()	{	return m_u8ACamNum;	}
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
	VOID				SelectACam(UINT8 cam_no);
#endif
};

/* --------------------------------------------------------------------------------------------- */
/*                                   Button Align Mark Index                                     */
/* --------------------------------------------------------------------------------------------- */

class CBtnMarkIdx : public CBtnCtrl
{
/* 생성자 & 파괴자 */
public:

	CBtnMarkIdx()
	{
		m_u8MarkIndex	= 1;
	}
	virtual ~CBtnMarkIdx()
	{
	}

/* 가상 함수 */
protected:

	virtual BOOL		LButtonDn()	{	return TRUE;	};
	virtual BOOL		LButtonUp();


/* 로컬 변수 */
protected:

	UINT8				m_u8MarkIndex;	/* 1 or 2 */


/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	UINT8				GetMarkIndex()	{	return m_u8MarkIndex;	}
};


/* --------------------------------------------------------------------------------------------- */
/*                                      Button Mark Model                                        */
/* --------------------------------------------------------------------------------------------- */

class CBtnMarkModel : public CBtnCtrl
{
/* 생성자 & 파괴자 */
public:

	CBtnMarkModel()
	{
		m_u32MarkModel	= 8;
	}
	virtual ~CBtnMarkModel()
	{
	}

/* 가상 함수 */
protected:

	virtual BOOL		LButtonDn()	{	return TRUE;	};
	virtual BOOL		LButtonUp();


/* 로컬 변수 */
protected:

	UINT32				m_u32MarkModel;	/* Circle, Cross, Rectangle, Square, Ellipse, etc */


/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	UINT32				GetMarkModel()	{	return m_u32MarkModel;	}
};

/* --------------------------------------------------------------------------------------------- */
/*                                       Button JOb TYpe                                         */
/* --------------------------------------------------------------------------------------------- */

class CBtnJobType : public CBtnCtrl
{
/* 생성자 & 파괴자 */
public:

	CBtnJobType()
	{
		m_u8JobId	= 0x00;
	}
	virtual ~CBtnJobType()
	{
	}

/* 가상 함수 */
protected:

	virtual BOOL		LButtonDn()	{	return TRUE;	};
	virtual BOOL		LButtonUp();


/* 로컬 변수 */
protected:

	UINT8				m_u8JobId;

	CStringArray		m_arrJobType;	/* Job Type이 등록될 배열 */


/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	BOOL				AddJobType(TCHAR *name);
	UINT8				GetJobType()	{	return m_u8JobId+1;	}
	BOOL				SetJobType(UINT8 job_id);
};


/* --------------------------------------------------------------------------------------------- */
/*                                  Button Mark Parameter 1                                      */
/* --------------------------------------------------------------------------------------------- */

class CBtnMarkParam1 : public CBtnCtrl
{
/* 생성자 & 파괴자 */
public:

	CBtnMarkParam1()
	{
		m_u16Param1	= 256;
	}
	virtual ~CBtnMarkParam1()
	{
	}

/* 가상 함수 */
protected:

	virtual BOOL		LButtonDn()	{	return TRUE;	};
	virtual BOOL		LButtonUp();


/* 로컬 변수 */
protected:

	UINT16				m_u16Param1;


/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	UINT16				GetParam1()	{	return m_u16Param1;	}
};

/* --------------------------------------------------------------------------------------------- */
/*                                 Button Mode (Step) 1 or 2                                     */
/* --------------------------------------------------------------------------------------------- */

class CBtnStepMode : public CBtnCtrl
{
/* 생성자 & 파괴자 */
public:

	CBtnStepMode()
	{
		m_u8Mode	= 1;
	}
	virtual ~CBtnStepMode()
	{
	}

/* 가상 함수 */
protected:

	virtual BOOL		LButtonDn()	{	return TRUE;	};
	virtual BOOL		LButtonUp();


/* 로컬 변수 */
protected:

	UINT8				m_u8Mode;


/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	UINT8				GetMode()	{	return m_u8Mode;	}
};


/* --------------------------------------------------------------------------------------------- */
/*                              Align Mode (Move or Step) 1 or 2                                 */
/* --------------------------------------------------------------------------------------------- */
class CBtnAlignMode : public CBtnCtrl
{
/* 생성자 & 파괴자 */
public:

	CBtnAlignMode()
	{
		m_u8Mode	= 2;	/* 0x01 : Align Move, 0x02 : Align Step */
	}
	virtual ~CBtnAlignMode()
	{
	}

/* 가상 함수 */
protected:

	virtual BOOL		LButtonDn()	{	return TRUE;	};
	virtual BOOL		LButtonUp();


/* 로컬 변수 */
protected:

	UINT8				m_u8Mode;


/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	UINT8				GetMode()	{	return m_u8Mode;	}
	VOID				SetInit();
};

/* --------------------------------------------------------------------------------------------- */
/*                                    Button Number (1 ~ 255)                                    */
/* --------------------------------------------------------------------------------------------- */

class CBtnNumber: public CBtnCtrl
{
/* 생성자 & 파괴자 */
public:

	CBtnNumber()
	{
		m_u32Num	= 0;
		m_u32Tot	= 0;
		wmemset(m_tzTitle, 0x00, 128);
	}
	virtual ~CBtnNumber()	{}

/* 가상 함수 */
protected:

	virtual BOOL		LButtonDn()	{	return TRUE;	};
	virtual BOOL		LButtonUp();


/* 로컬 변수 */
protected:

	UINT32				m_u32Num, m_u32Tot;

	TCHAR				m_tzTitle[128];

/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	VOID				PutNum(UINT32 num, UINT32 tot=0);
	VOID				SetTitle(TCHAR *title)	{	wcscpy_s(m_tzTitle, 127, title);	}
	UINT32				GetNum()	{	return m_u32Num;	};
};
