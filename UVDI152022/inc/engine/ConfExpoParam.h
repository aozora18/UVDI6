
#pragma once

#include "ConfBase.h"

class CConfExpoParam : public CConfBase
{
/* ������ & �ı��� */
public:

	CConfExpoParam(LPG_CTEI config);
	virtual ~CConfExpoParam();

/* ���� �Լ� */
protected:

public:


/* ���� ���� */
protected:

	LPG_CTEI			m_pstCfg;


/* ���� �Լ� */
protected:

	BOOL				LoadConfigTeachingTransRecipe();
	BOOL				SaveConfigTeachingTransRecipe();
	BOOL				LoadConfigTeachingExpoLuria();
	BOOL				SaveConfigTeachingExpoLuria();
	BOOL				LoadConfigTeachingMaxtrixExpo();
	BOOL				SaveConfigTeachingMaxtrixExpo();
	BOOL				LoadConfigTeachingExpoGerberList();
	BOOL				SaveConfigTeachingExpoGerberList();
	BOOL				LoadConfigTeachingMeasGerberList();
	BOOL				SaveConfigTeachingMeasGerberList();
	BOOL				LoadConfigTeachingExpoRecipe();
	BOOL				SaveConfigTeachingExpoRecipe();


/* ���� �Լ� */
public:

	BOOL				LoadConfig();
	BOOL				SaveConfig();

	LPG_CTEI			GetConfig()	{	return m_pstCfg;	}
};