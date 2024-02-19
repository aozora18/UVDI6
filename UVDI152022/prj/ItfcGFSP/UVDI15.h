#pragma once

#include "Config.h"

class CPODIS : public CConfig
{
/* 생성자 & 파괴자 */
public:

	CPODIS(PTCHAR work_dir);
	virtual ~CPODIS();

/* 가상함수 재정의 */
protected:
public:


/* 로컬 변수 */
protected:

	LPG_CSID			m_pstConfig;


/* 로컬 함수 */
protected:

	BOOL				LoadInit();
	BOOL				SaveInit();

	BOOL				IsValidConfig();


/* 공용 함수 */
public:

	BOOL				LoadConfig();
	BOOL				SaveConfig();

	LPG_CSID			GetConfig()			{	return m_pstConfig;					}
};
