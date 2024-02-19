
#pragma once

#include "ConfBase.h"

class CConfTracking : public CConfBase
{
/* 생성자 & 파괴자 */
public:

	CConfTracking(LPG_CWTI config);
	virtual ~CConfTracking();

/* 가상 함수 */
protected:

public:


/* 로컬 변수 */
protected:

	LPG_CWTI			m_pstCfg;


/* 로컬 함수 */
protected:

	BOOL				LoadConfigAligner();
	BOOL				SaveConfigAligner();
	BOOL				LoadConfigLoader();
	BOOL				SaveConfigLoader();
	BOOL				LoadConfigHandler();
	BOOL				SaveConfigHandler();
	BOOL				LoadConfigBuffer();
	BOOL				SaveConfigBuffer();


/* 공용 함수 */
public:

	BOOL				LoadConfig();
	BOOL				SaveConfig();

	LPG_CWTI			GetConfig()	{	return m_pstCfg;	}
};