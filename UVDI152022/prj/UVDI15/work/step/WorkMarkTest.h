
#pragma once

#include "../WorkStep.h"
#include <map>
#include "../../../../inc/itfe/EItfcLuria.h"

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

	enum class AlignMotionMode : UINT8
	{
		toInitialMoving = 0, //���� ��ĵ�� ���� �˻� �ʱ���ġ�� �̵��ϴ� ����
		toScanMoving = 1,    //��ĵ�� ���� �̵��ϴ� ����
	};
	
	
	/* ���� ���� */
protected:

	int globalMarkCnt, localMarkCnt = 0;
	UINT8				m_u8MarkCount;	/* �� �˻��� ��ũ ���� */
	UINT32				m_u32ExpoCount;		/* ���� �뱤 �ݺ� Ƚ�� */
	STG_CELA			m_stExpoLog;
	int scanCount = 0;

	/* ���� �Լ� */
protected:

	VOID				SetWorkNext();

	ENG_JWNS			SetHomingACamSide();
	ENG_JWNS			IsHomedACamSide();

	VOID				SaveExpoResult(UINT8 state);

	/* ���� �Լ� */
public:




};
