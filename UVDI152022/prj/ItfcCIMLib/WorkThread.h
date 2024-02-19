
#pragma once

#include "../../inc/comn/ThinThread.h"

class CSharedData;
class CLogData;

class CE30GEM;
class CE40PJM;
class CE87CMS;
class CE90STS;
class CE94CJM;

class CWorkThread : public CThinThread
{
/* ������/�ı��� */
public:

	CWorkThread(CSharedData *shared, CLogData *logs,
				CE30GEM *e30gem, CE40PJM *e40pjm, CE87CMS *e87cms,
				CE90STS *e90sts, CE94CJM *e94cjm);
	virtual ~CWorkThread();

/* �����Լ� ������ */
protected:

	virtual BOOL		StartWork()	{	return TRUE;	};
	virtual VOID		RunWork();
	virtual VOID		EndWork()	{};


/* ���� ���� */
protected:

	UINT8				m_u8UpdateStep;
	UINT64				m_u64TickState;		/* Substate State ���� �ֱ� */

	CSharedData			*m_pSharedData;
	CLogData			*m_pLogData;

	CE30GEM				*m_pE30GEM;
	CE40PJM				*m_pE40PJM;
	CE87CMS				*m_pE87CMS;
	CE90STS				*m_pE90STS;
	CE94CJM				*m_pE94CJM;

/* ���� �Լ� */
protected:

	VOID				UpdateListPRJobID();
	VOID				UpdateListCRJobID();
	VOID				UpdatePortCarrier();
	BOOL				IsServiceRun();


/* ���� �Լ� */
public:


};
