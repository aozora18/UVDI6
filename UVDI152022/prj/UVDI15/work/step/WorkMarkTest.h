
#pragma once

#include "../WorkStep.h"

class CWorkMarkTest : public CWorkStep
{
/* ������ & �ı��� */
public:

	CWorkMarkTest(LPG_CELA expo);
	virtual ~CWorkMarkTest();

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

	ENG_JWNS			SetHomingACamSide();
	ENG_JWNS			IsHomedACamSide();

	VOID				SaveExpoResult(UINT8 state);
/* ���� �Լ� */
public:


};
