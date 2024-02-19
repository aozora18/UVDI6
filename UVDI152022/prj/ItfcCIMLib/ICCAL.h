
#pragma once

#include "EBase.h"

class CICCAL : public CEBase
{
public:
	/* 생성자 & 파괴자 */
	CICCAL(CLogData *logs, CSharedData *share);
	~CICCAL();

/* 구조체 */
protected:

/* 가상 함수 */
protected:
public:


/* 로컬 변수 */
protected:


/* 로컬 함수 */
protected:

/* 공용 함수 */
public:

	BOOL				Init(LONG equip_id, LONG conn_id);
	VOID				Close();

	BOOL				GetLogToCIMConnect(BOOL &value);
	BOOL				SetLogToCIMConnect(BOOL value);
};
