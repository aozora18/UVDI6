
#pragma once

#include "ConfBase.h"

class CConfExpoParam : public CConfBase
{
/* 생성자 & 파괴자 */
public:

	CConfExpoParam(LPG_CTEI config);
	virtual ~CConfExpoParam();

/* 가상 함수 */
protected:

public:


/* 로컬 변수 */
protected:

	LPG_CTEI			m_pstCfg;


/* 로컬 함수 */
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


/* 공용 함수 */
public:

	BOOL				LoadConfig();
	BOOL				SaveConfig();

	LPG_CTEI			GetConfig()	{	return m_pstCfg;	}
};