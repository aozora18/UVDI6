
#pragma once

#include "ConfBase.h"

class CConfTeaching : public CConfBase
{
/* ������ & �ı��� */
public:

	CConfTeaching(LPG_CTPI config);
	virtual ~CConfTeaching();

/* ���� �Լ� */
protected:

public:


/* ���� ���� */
protected:

	LPG_CTPI			m_pstCfg;


/* ���� �Լ� */
protected:

	BOOL				LoadConfigTeachingAutoRun();
	BOOL				SaveConfigTeachingAutoRun();
	BOOL				LoadConfigTeachingZPos();
	BOOL				SaveConfigTeachingZPos();
	BOOL				LoadConfigTeachingOffset();
	BOOL				SaveConfigTeachingOffset();
	BOOL				LoadConfigTeachingVelo();
	BOOL				SaveConfigTeachingVelo();
	BOOL				LoadConfigTeachingIllum();
	BOOL				SaveConfigTeachingIllum();
	BOOL				LoadConfigTeachingCentering();
	BOOL				SaveConfigTeachingCentering();
	BOOL				LoadConfigTeachingLight();
	BOOL				SaveConfigTeachingLight();
	BOOL				LoadConfigTeachingEquipment();
	BOOL				SaveConfigTeachingEquipment();
	BOOL				LoadConfigTeachingInterlock();
	BOOL				SaveConfigTeachingInterlock();
	BOOL				LoadConfigTeachingTransRecipe();
	BOOL				SaveConfigTeachingTransRecipe();
	BOOL				LoadConfigTeachingExpoLuria();
	BOOL				SaveConfigTeachingExpoLuria();


/* ���� �Լ� */
public:

	BOOL				LoadConfig();
	BOOL				SaveConfig();

	LPG_CTPI			GetConfig()	{	return m_pstCfg;	}
};