
#pragma once

#include "../WorkStep.h"

class CWorkExpoAlign : public CWorkStep
{
/* ������ & �ı��� */
public:

	CWorkExpoAlign();
	virtual ~CWorkExpoAlign();

/* �����Լ� ������ */
protected:
public:

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();


/* ���� ���� */
protected:

	UINT8				m_u8MarkCount;	/* �� �˻��� ��ũ ���� */
	UINT32				m_u32ExpoCount;		/* ���� �뱤 �ݺ� Ƚ�� */
	STG_CELA			m_stExpoLog;

/* ���� �Լ� */
protected:

	VOID				SetWorkNext();
	VOID				SaveExpoResult(UINT8 state);

	CString				m_strLog;
	VOID				txtWrite(CString msg);
	VOID				SetPhilProcessCompelet();
/* ���� �Լ� */
public:


};
