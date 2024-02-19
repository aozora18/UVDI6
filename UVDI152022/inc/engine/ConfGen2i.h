
#pragma once

#include "ConfBase.h"

class CConfGen2i : public CConfBase
{
/* 생성자 & 파괴자 */
public:

	CConfGen2i(LPG_CIEA config);
	virtual ~CConfGen2i();

/* 가상 함수 */
protected:

public:


/* 로컬 변수 */
protected:

	LPG_CIEA			m_pstCfg;	/* 환경 파일 정보 연결 */


/* 로컬 함수 */
protected:

	BOOL				LoadConfigFileName();
	BOOL				LoadConfigPreAligner();
	BOOL				LoadConfigDiamondRobot();
	BOOL				LoadConfigEFU();


/* 공용 함수 */
public:

	BOOL				LoadConfig();
	BOOL				SaveConfig();

	LPG_CIEA			GetConfig()	{	return m_pstCfg;	}
};