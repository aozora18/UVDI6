#pragma once

#include "../../inc/conf/trig_uvdi15.h"
#include "../../inc/conf/recipe_uvdi15.h"
#include "../../inc/conf/vision_uvdi15.h"	//버젼 에러 이슈
#include "../../inc/conf/pm100.h"
#include "MemMap.h"

class CMemBase
{
public:

	CMemBase(BOOL engine, LPG_CIEA config, ENG_MMDC dev_type);
	virtual ~CMemBase();

/* 가상 함수 */
protected:

	virtual BOOL		LinkMemMap() = 0;	/* Memory 영역에 각 주소 연결 작업 */


/* 멤버 변수 */
protected:

	BOOL				m_bIsEngine;	/* 현재 Engine 모드로 동작 중인지 제어 S/W UI 모드로 동작 중인지 */
	UINT32				m_u32MemSize;	/* Shared Memory Size (0 값이면, 기본 크기. 단위: Bytes) */
	PTCHAR				m_pShMemKey;	/* Shared Memory Key */
	ENG_MMDC			m_enDevType;	/* Memory Mapped 관련 장치 코드 값 */
	LPG_CIEA			m_pstConfig;	/* 전체 환경 변수 */
	CMemMap				*m_pMemMap;


/* 멤버 함수 */
protected:

	UINT32				CalcMemSize();
	UINT32				CalcMemSizeOfConfig();
	UINT32				CalcMemSizeOfLuria();
	UINT32				CalcMemSizeOfMC2();
	UINT32				CalcMemSizeOfPLCQ();
	UINT32				CalcMemSizeOfTrig();
	UINT32				CalcMemSizeOfVisi();
	/* For Recipe : PH Offset (Step) */
	UINT32				CalcMemSizeOfPhStep();
	UINT32				CalcMemSizeOfPhCorrectY();
	UINT32				CalcMemSizeOfPM100D();
	UINT32				CalcMemSizeOfPhilhmi();
	UINT32				CalcMemSizeOfStrobeLamp();


/* 공용 함수 */
public:

	BOOL				CreateMap();
	VOID				CloseMap();

	PUINT8				GetMemMap();
};

/* --------------------------------------------------------------------------------------------- */
/*                                Shared Memory - Configuration                                  */
/* --------------------------------------------------------------------------------------------- */

class CMemConf : public CMemBase
{
public:

	CMemConf(BOOL engine);
	virtual ~CMemConf()	{};

/* 가상 함수 */
protected:

	virtual BOOL		LinkMemMap();	/* Memory 영역에 각 주소 연결 작업 */

/* 멤버 변수 */
protected:

	LPG_CIEA			m_pstMemMap;


/* 공용 함수 */
public:

	LPG_CIEA			GetMemMap()	{	return m_pstMemMap;	}	/* 공유 메모리 반환 */
};

/* --------------------------------------------------------------------------------------------- */
/*                                 Shared Memory - Luria Object                                  */
/* --------------------------------------------------------------------------------------------- */

class CMemLuria : public CMemBase
{
public:

	CMemLuria(BOOL engine, LPG_CIEA config);
	virtual ~CMemLuria()	{};

/* 가상 함수 */
protected:

	virtual BOOL		LinkMemMap();	/* Memory 영역에 각 주소 연결 작업 */

/* 멤버 변수 */
protected:

	LPG_LDSM			m_pstMemMap;


/* 공용 함수 */
public:

	LPG_LDSM			GetMemMap()	{	return m_pstMemMap;	}	/* 공유 메모리 반환 */
};

/* --------------------------------------------------------------------------------------------- */
/*                                  Shared Memory - MC2 Object                                   */
/* --------------------------------------------------------------------------------------------- */

class CMemMC2 : public CMemBase
{
public:

	CMemMC2(BOOL engine, LPG_CIEA config);
	virtual ~CMemMC2()	{};

/* 가상 함수 */
protected:

	virtual BOOL		LinkMemMap();	/* Memory 영역에 각 주소 연결 작업 */


/* 멤버 변수 */
protected:

	LPG_MDSM			m_pstMemMap;


/* 공용 함수 */
public:

	LPG_MDSM			GetMemMap()	{	return m_pstMemMap;	}	/* 공유 메모리 반환 */
};

/* --------------------------------------------------------------------------------------------- */
/*                                  Shared Memory - MC2b Object                                   */
/* --------------------------------------------------------------------------------------------- */

class CMemMC2b : public CMemBase
{
public:

	CMemMC2b(BOOL engine, LPG_CIEA config);
	virtual ~CMemMC2b() {};

	/* 가상 함수 */
protected:

	virtual BOOL		LinkMemMap();	/* Memory 영역에 각 주소 연결 작업 */


/* 멤버 변수 */
protected:

	LPG_MDSM			m_pstMemMap;


	/* 공용 함수 */
public:

	LPG_MDSM			GetMemMap() { return m_pstMemMap; }	/* 공유 메모리 반환 */
};


/* --------------------------------------------------------------------------------------------- */
/*                                Shared Memory - Trigger Object                                 */
/* --------------------------------------------------------------------------------------------- */

class CMemTrig : public CMemBase
{
public:

	CMemTrig(BOOL engine, LPG_CIEA config);
	virtual ~CMemTrig()	{};

/* 가상 함수 */
protected:

	virtual BOOL		LinkMemMap();	/* Memory 영역에 각 주소 연결 작업 */


/* 멤버 변수 */
protected:

	LPG_TPQR			m_pstMemMap;
	LPG_DLSM			m_pstLink;


/* 공용 함수 */
public:

	LPG_TPQR			GetMemMap()		{	return m_pstMemMap;	}	/* 공유 메모리 반환 */
	LPG_DLSM			GetMemLink()	{	return m_pstLink;	}	/* 통신 연결 상태 */
};

/* --------------------------------------------------------------------------------------------- */
/*                                 Shared Memory - Vision Object                                 */
/* --------------------------------------------------------------------------------------------- */

class CMemVisi : public CMemBase
{
public:

	CMemVisi(BOOL engine, LPG_CIEA config);
	virtual ~CMemVisi();

/* 가상 함수 */
protected:

	virtual BOOL		LinkMemMap();	/* Memory 영역에 각 주소 연결 작업 */


/* 멤버 변수 */
protected:

	LPG_VDSM			m_pstMemMap;


/* 공용 함수 */
public:

	LPG_VDSM			GetMemMap()	{	return m_pstMemMap;	}	/* 공유 메모리 반환 */
};

/* --------------------------------------------------------------------------------------------- */
/*                       Shared Memory - Recipe for Photohead Offset (step)                      */
/* --------------------------------------------------------------------------------------------- */

class CMemPhStep : public CMemBase
{
public:

	CMemPhStep(BOOL engine, LPG_CIEA config);
	virtual ~CMemPhStep()	{};

/* 가상 함수 */
protected:

	virtual BOOL		LinkMemMap();	/* Memory 영역에 각 주소 연결 작업 */

/* 멤버 변수 */
protected:

	LPG_OSSD			m_pstMemMap;


/* 공용 함수 */
public:

	LPG_OSSD			GetMemMap()	{	return m_pstMemMap;	}	/* 공유 메모리 반환 */
};

/* --------------------------------------------------------------------------------------------- */
/*                Shared Memory - Recipe for Photohead Correction (Strip) Y Table                */
/* --------------------------------------------------------------------------------------------- */

class CMemPhCorrectY : public CMemBase
{
public:

	CMemPhCorrectY(BOOL engine, LPG_CIEA config);
	virtual ~CMemPhCorrectY()	{};

/* 가상 함수 */
protected:

	virtual BOOL		LinkMemMap();	/* Memory 영역에 각 주소 연결 작업 */

/* 멤버 변수 */
protected:

	LPG_OSCY			m_pstMemMap;


/* 공용 함수 */
public:

	LPG_OSCY			GetMemMap()	{	return m_pstMemMap;	}	/* 공유 메모리 반환 */
};

/* --------------------------------------------------------------------------------------------- */
/*                             Shared Memory - LED Power Measurement(Gentec)                     */
/* --------------------------------------------------------------------------------------------- */

class CMemGentec : public CMemBase
{
public:

	CMemGentec(BOOL engine, LPG_CIEA config);
	virtual ~CMemGentec()	{};

/* 가상 함수 */
protected:

	virtual BOOL		LinkMemMap();	/* Memory 영역에 각 주소 연결 작업 */

/* 멤버 변수 */
protected:

	LPG_PADV			m_pstMemMap;


/* 공용 함수 */
public:

	LPG_PADV			GetMemMap()	{	return m_pstMemMap;	}	/* 공유 메모리 반환 */
};

/* --------------------------------------------------------------------------------------------- */
/*                                Shared Memory - Trigger Object(Mvence)                         */
/* --------------------------------------------------------------------------------------------- */

class CMemMvenc : public CMemBase
{
public:

	CMemMvenc(BOOL engine, LPG_CIEA config);
	virtual ~CMemMvenc() {};

	/* 가상 함수 */
protected:

	virtual BOOL		LinkMemMap();	/* Memory 영역에 각 주소 연결 작업 */


/* 멤버 변수 */
protected:

	LPG_TPQR			m_pstMemMap;
	LPG_DLSM			m_pstLink;


	/* 공용 함수 */
public:

	LPG_TPQR			GetMemMap() { return m_pstMemMap; }	/* 공유 메모리 반환 */
	LPG_DLSM			GetMemLink() { return m_pstLink; }	/* 통신 연결 상태 */
};

/* --------------------------------------------------------------------------------------------- */
/*                                Shared Memory - Trigger Object(Mvence)                         */
/* --------------------------------------------------------------------------------------------- */

class CMemPhilhmi : public CMemBase
{
public:

	CMemPhilhmi(BOOL engine, LPG_CIEA config);
	virtual ~CMemPhilhmi() {};

	/* 가상 함수 */
protected:

	virtual BOOL		LinkMemMap();	/* Memory 영역에 각 주소 연결 작업 */


/* 멤버 변수 */
protected:

	LPG_PPR				m_pstMemMap;
	LPG_DLSM			m_pstLink;


	/* 공용 함수 */
public:

	LPG_PPR			GetMemMap() { return m_pstMemMap; }	/* 공유 메모리 반환 */
	LPG_DLSM		GetMemLink() { return m_pstLink; }	/* 통신 연결 상태 */
};

/* --------------------------------------------------------------------------------------------- */
/*                                Shared Memory - Trigger Object(Mvence)                         */
/* --------------------------------------------------------------------------------------------- */

class CMemStrobeLamp : public CMemBase
{
public:

	CMemStrobeLamp(BOOL engine, LPG_CIEA config);
	virtual ~CMemStrobeLamp() {};

	/* 가상 함수 */
protected:

	virtual BOOL		LinkMemMap();	/* Memory 영역에 각 주소 연결 작업 */


	/* 멤버 변수 */
protected:

	LPG_SLPR			m_pstMemMap;
	LPG_DLSM			m_pstLink;


	/* 공용 함수 */
public:

	LPG_SLPR		GetMemMap() { return m_pstMemMap; }	/* 공유 메모리 반환 */
	LPG_DLSM		GetMemLink() { return m_pstLink; }	/* 통신 연결 상태 */
};