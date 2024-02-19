
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
/* 생성자/파괴자 */
public:

	CWorkThread(CSharedData *shared, CLogData *logs,
				CE30GEM *e30gem, CE40PJM *e40pjm, CE87CMS *e87cms,
				CE90STS *e90sts, CE94CJM *e94cjm);
	virtual ~CWorkThread();

/* 가상함수 재정의 */
protected:

	virtual BOOL		StartWork()	{	return TRUE;	};
	virtual VOID		RunWork();
	virtual VOID		EndWork()	{};


/* 로컬 변수 */
protected:

	UINT8				m_u8UpdateStep;
	UINT64				m_u64TickState;		/* Substate State 갱신 주기 */

	CSharedData			*m_pSharedData;
	CLogData			*m_pLogData;

	CE30GEM				*m_pE30GEM;
	CE40PJM				*m_pE40PJM;
	CE87CMS				*m_pE87CMS;
	CE90STS				*m_pE90STS;
	CE94CJM				*m_pE94CJM;

/* 로컬 함수 */
protected:

	VOID				UpdateListPRJobID();
	VOID				UpdateListCRJobID();
	VOID				UpdatePortCarrier();
	BOOL				IsServiceRun();


/* 공용 함수 */
public:


};
