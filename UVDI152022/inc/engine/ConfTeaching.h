
#pragma once

#include "ConfBase.h"

class CConfTeaching : public CConfBase
{
/* 생성자 & 파괴자 */
public:

	CConfTeaching(LPG_CTPI config);
	virtual ~CConfTeaching();

/* 가상 함수 */
protected:

public:


/* 로컬 변수 */
protected:

	LPG_CTPI			m_pstCfg;


/* 로컬 함수 */
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


/* 공용 함수 */
public:

	BOOL				LoadConfig();
	BOOL				SaveConfig();

	LPG_CTPI			GetConfig()	{	return m_pstCfg;	}
};