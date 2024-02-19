
#pragma once

#include "../../inc/comn/ThinThread.h"

class CMeasure;

class CMainThread : public CThinThread
{
/* Structure / Destructure */
public:
	CMainThread(HWND parent);
	virtual ~CMainThread();


/* Virtual Function */
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();


/* Local Parameter */
protected:

	/* The periodic refresh interval */
	UINT64				m_u64Tick;
	UINT64				m_u64Wait;

/* Local Function */
protected:

	VOID				SendMesgParent(UINT8 type, UINT32 timeout=100);


/* Public Function */
public:


};