#pragma once

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

	UINT32				CalcMemSizeOfConfig();
	UINT32				CalcMemSizeOfPM100D();
	UINT32				CalcMemSizeOfPhilhmi();
	UINT32				CalcMemSizeOfLuria();
	UINT32				CalcMemSizeOfMC2();
	UINT32				CalcMemSizeOfBSA();
	UINT32				CalcMemSizeOfPLCQ();
	UINT32				CalcMemSizeOfEFU();
	UINT32				CalcMemSizeOfTeaching();
	UINT32				CalcMemSizeOfTracking();
	UINT32				CalcMemSizeOfExpoParam();
	UINT32				CalcMemSizeOfLSPA();
	UINT32				CalcMemSizeOfLSDR();


/* 공용 함수 */
public:

	BOOL				CreateMap();
	VOID				CloseMap();

	PUINT8				GetMemMap();

	UINT32				CalcMemSize();
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

class CMemConfTeaching : public CMemBase
{
public:

	CMemConfTeaching(BOOL engine);
	virtual ~CMemConfTeaching()	{};

/* 가상 함수 */
protected:

	virtual BOOL		LinkMemMap();	/* Memory 영역에 각 주소 연결 작업 */

/* 멤버 변수 */
protected:

	LPG_CTPI			m_pstMemMap;


/* 공용 함수 */
public:

	LPG_CTPI			GetMemMap()	{	return m_pstMemMap;	}	/* 공유 메모리 반환 */
};

class CMemConfExpoParam : public CMemBase
{
public:

	CMemConfExpoParam(BOOL engine);
	virtual ~CMemConfExpoParam()	{};

/* 가상 함수 */
protected:

	virtual BOOL		LinkMemMap();	/* Memory 영역에 각 주소 연결 작업 */

/* 멤버 변수 */
protected:

	LPG_CTEI			m_pstMemMap;


/* 공용 함수 */
public:

	LPG_CTEI			GetMemMap()	{	return m_pstMemMap;	}	/* 공유 메모리 반환 */
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
/*                                  Shared Memory - PLC Object                                   */
/* --------------------------------------------------------------------------------------------- */

class CMemPLC : public CMemBase
{
public:

	CMemPLC(BOOL engine, LPG_CIEA config);
	virtual ~CMemPLC();

/* 가상 함수 */
protected:

	virtual BOOL		LinkMemMap();	/* Memory 영역에 각 주소 연결 작업 */


/* 멤버 변수 */
protected:

	LPG_PDSM			m_pstMemMap;


/* 공용 함수 */
public:

	BOOL				GetBitValue(UINT32 addr, UINT8 bits, UINT8 &value);
	BOOL				GetByteValue(UINT32 addr, UINT8 flag, UINT8 &value);
	BOOL				GetWordValue(ENG_PIOA addr, UINT16 &value);
	BOOL				GetWordValue(UINT32 addr, UINT16 &value);
	BOOL				GetWordValue(UINT16 index, UINT16 &value);
	BOOL				GetDWordValue(UINT32 addr, UINT32 &value);
	BOOL				GetDWordValue(UINT16 index, UINT32 &value);

	LPG_PDSM			GetMemMap()	{	return m_pstMemMap;	}	/* 공유 메모리 반환 */
};

/* --------------------------------------------------------------------------------------------- */
/*                          Shared Memory - Wafer Tracking Information                           */
/* --------------------------------------------------------------------------------------------- */

class CMemConfTracking : public CMemBase
{
public:

	CMemConfTracking(BOOL engine);
	virtual ~CMemConfTracking()	{};

/* 가상 함수 */
protected:

	virtual BOOL		LinkMemMap();	/* Memory 영역에 각 주소 연결 작업 */

/* 멤버 변수 */
protected:

	LPG_CWTI			m_pstMemMap;	/* 전체 패킷 정보*/


/* 공용 함수 */
public:

	LPG_CWTI			GetMemMap()	{	return m_pstMemMap;	}	/* 공유 메모리 반환 */
};

/* --------------------------------------------------------------------------------------------- */
/*                    Shared Memory - PreAligner Data for Logosol with serial                    */
/* --------------------------------------------------------------------------------------------- */

class CMemMPA : public CMemBase
{
public:

	CMemMPA(BOOL engine, LPG_CIEA config);
	virtual ~CMemMPA()	{};

/* 가상 함수 */
protected:

	virtual BOOL		LinkMemMap();	/* Memory 영역에 각 주소 연결 작업 */

/* 멤버 변수 */
protected:

	LPG_MPDV			m_pstMemMap;


/* 공용 함수 */
public:

	LPG_MPDV			GetMemMap()	{	return m_pstMemMap;	}	/* 공유 메모리 반환 */
};

/* --------------------------------------------------------------------------------------------- */
/*                  Shared Memory - Robot Diamond Data for Logosol with serial                   */
/* --------------------------------------------------------------------------------------------- */

class CMemMDR : public CMemBase
{
public:

	CMemMDR(BOOL engine, LPG_CIEA config);
	virtual ~CMemMDR()	{};

/* 가상 함수 */
protected:

	virtual BOOL		LinkMemMap();	/* Memory 영역에 각 주소 연결 작업 */

/* 멤버 변수 */
protected:

	LPG_MRDV			m_pstMemMap;


/* 공용 함수 */
public:

	LPG_MRDV			GetMemMap()	{	return m_pstMemMap;	}	/* 공유 메모리 반환 */
};

/* --------------------------------------------------------------------------------------------- */
/*                             Shared Memory - EFU Data for Shinsung                             */
/* --------------------------------------------------------------------------------------------- */

class CMemEFU : public CMemBase
{
public:

	CMemEFU(BOOL engine, LPG_CIEA config);
	virtual ~CMemEFU()	{};

/* 가상 함수 */
protected:

	virtual BOOL		LinkMemMap();	/* Memory 영역에 각 주소 연결 작업 */

/* 멤버 변수 */
protected:

	LPG_EPAS			m_pstMemMap;


/* 공용 함수 */
public:

	LPG_EPAS			GetMemMap()	{	return m_pstMemMap;	}	/* 공유 메모리 반환 */
};

/* --------------------------------------------------------------------------------------------- */
/*                                   Shared Memory - BSA Object                                  */
/* --------------------------------------------------------------------------------------------- */

class CMemBSA : public CMemBase
{
public:

	CMemBSA(BOOL engine, LPG_CIEA config);
	virtual ~CMemBSA()	{};

/* 가상 함수 */
protected:

	virtual BOOL		LinkMemMap();	/* Memory 영역에 각 주소 연결 작업 */


/* 멤버 변수 */
protected:

	LPG_RBVC			m_pstMemMap;


/* 공용 함수 */
public:

	LPG_RBVC			GetMemMap()	{	return m_pstMemMap;	}	/* 공유 메모리 반환 */
};

/* --------------------------------------------------------------------------------------------- */
/*                             Shared Memory - LED Power Measurement                             */
/* --------------------------------------------------------------------------------------------- */

class CMemPM100D : public CMemBase
{
public:

	CMemPM100D(BOOL engine, LPG_CIEA config);
	virtual ~CMemPM100D()	{};

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

