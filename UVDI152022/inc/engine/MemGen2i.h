#pragma once

#include "MemMap.h"


class CMemBase
{
public:

	CMemBase(BOOL engine, LPG_CIEA config, ENG_MMDC dev_type);
	virtual ~CMemBase();

/* ���� �Լ� */
protected:

	virtual BOOL		LinkMemMap() = 0;	/* Memory ������ �� �ּ� ���� �۾� */


/* ��� ���� */
protected:

	BOOL				m_bIsEngine;	/* ���� Engine ���� ���� ������ ���� S/W UI ���� ���� ������ */
	UINT32				m_u32MemSize;	/* Shared Memory Size (0 ���̸�, �⺻ ũ��. ����: Bytes) */
	PTCHAR				m_pShMemKey;	/* Shared Memory Key */
	ENG_MMDC			m_enDevType;	/* Memory Mapped ���� ��ġ �ڵ� �� */
	LPG_CIEA			m_pstConfig;	/* ��ü ȯ�� ���� */
	CMemMap				*m_pMemMap;


/* ��� �Լ� */
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


/* ���� �Լ� */
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

/* ���� �Լ� */
protected:

	virtual BOOL		LinkMemMap();	/* Memory ������ �� �ּ� ���� �۾� */

/* ��� ���� */
protected:

	LPG_CIEA			m_pstMemMap;


/* ���� �Լ� */
public:

	LPG_CIEA			GetMemMap()	{	return m_pstMemMap;	}	/* ���� �޸� ��ȯ */
};

class CMemConfTeaching : public CMemBase
{
public:

	CMemConfTeaching(BOOL engine);
	virtual ~CMemConfTeaching()	{};

/* ���� �Լ� */
protected:

	virtual BOOL		LinkMemMap();	/* Memory ������ �� �ּ� ���� �۾� */

/* ��� ���� */
protected:

	LPG_CTPI			m_pstMemMap;


/* ���� �Լ� */
public:

	LPG_CTPI			GetMemMap()	{	return m_pstMemMap;	}	/* ���� �޸� ��ȯ */
};

class CMemConfExpoParam : public CMemBase
{
public:

	CMemConfExpoParam(BOOL engine);
	virtual ~CMemConfExpoParam()	{};

/* ���� �Լ� */
protected:

	virtual BOOL		LinkMemMap();	/* Memory ������ �� �ּ� ���� �۾� */

/* ��� ���� */
protected:

	LPG_CTEI			m_pstMemMap;


/* ���� �Լ� */
public:

	LPG_CTEI			GetMemMap()	{	return m_pstMemMap;	}	/* ���� �޸� ��ȯ */
};

/* --------------------------------------------------------------------------------------------- */
/*                                 Shared Memory - Luria Object                                  */
/* --------------------------------------------------------------------------------------------- */

class CMemLuria : public CMemBase
{
public:

	CMemLuria(BOOL engine, LPG_CIEA config);
	virtual ~CMemLuria()	{};

/* ���� �Լ� */
protected:

	virtual BOOL		LinkMemMap();	/* Memory ������ �� �ּ� ���� �۾� */

/* ��� ���� */
protected:

	LPG_LDSM			m_pstMemMap;


/* ���� �Լ� */
public:

	LPG_LDSM			GetMemMap()	{	return m_pstMemMap;	}	/* ���� �޸� ��ȯ */
};

/* --------------------------------------------------------------------------------------------- */
/*                                  Shared Memory - MC2 Object                                   */
/* --------------------------------------------------------------------------------------------- */

class CMemMC2 : public CMemBase
{
public:

	CMemMC2(BOOL engine, LPG_CIEA config);
	virtual ~CMemMC2()	{};

/* ���� �Լ� */
protected:

	virtual BOOL		LinkMemMap();	/* Memory ������ �� �ּ� ���� �۾� */


/* ��� ���� */
protected:

	LPG_MDSM			m_pstMemMap;


/* ���� �Լ� */
public:

	LPG_MDSM			GetMemMap()	{	return m_pstMemMap;	}	/* ���� �޸� ��ȯ */
};

/* --------------------------------------------------------------------------------------------- */
/*                                  Shared Memory - PLC Object                                   */
/* --------------------------------------------------------------------------------------------- */

class CMemPLC : public CMemBase
{
public:

	CMemPLC(BOOL engine, LPG_CIEA config);
	virtual ~CMemPLC();

/* ���� �Լ� */
protected:

	virtual BOOL		LinkMemMap();	/* Memory ������ �� �ּ� ���� �۾� */


/* ��� ���� */
protected:

	LPG_PDSM			m_pstMemMap;


/* ���� �Լ� */
public:

	BOOL				GetBitValue(UINT32 addr, UINT8 bits, UINT8 &value);
	BOOL				GetByteValue(UINT32 addr, UINT8 flag, UINT8 &value);
	BOOL				GetWordValue(ENG_PIOA addr, UINT16 &value);
	BOOL				GetWordValue(UINT32 addr, UINT16 &value);
	BOOL				GetWordValue(UINT16 index, UINT16 &value);
	BOOL				GetDWordValue(UINT32 addr, UINT32 &value);
	BOOL				GetDWordValue(UINT16 index, UINT32 &value);

	LPG_PDSM			GetMemMap()	{	return m_pstMemMap;	}	/* ���� �޸� ��ȯ */
};

/* --------------------------------------------------------------------------------------------- */
/*                          Shared Memory - Wafer Tracking Information                           */
/* --------------------------------------------------------------------------------------------- */

class CMemConfTracking : public CMemBase
{
public:

	CMemConfTracking(BOOL engine);
	virtual ~CMemConfTracking()	{};

/* ���� �Լ� */
protected:

	virtual BOOL		LinkMemMap();	/* Memory ������ �� �ּ� ���� �۾� */

/* ��� ���� */
protected:

	LPG_CWTI			m_pstMemMap;	/* ��ü ��Ŷ ����*/


/* ���� �Լ� */
public:

	LPG_CWTI			GetMemMap()	{	return m_pstMemMap;	}	/* ���� �޸� ��ȯ */
};

/* --------------------------------------------------------------------------------------------- */
/*                    Shared Memory - PreAligner Data for Logosol with serial                    */
/* --------------------------------------------------------------------------------------------- */

class CMemMPA : public CMemBase
{
public:

	CMemMPA(BOOL engine, LPG_CIEA config);
	virtual ~CMemMPA()	{};

/* ���� �Լ� */
protected:

	virtual BOOL		LinkMemMap();	/* Memory ������ �� �ּ� ���� �۾� */

/* ��� ���� */
protected:

	LPG_MPDV			m_pstMemMap;


/* ���� �Լ� */
public:

	LPG_MPDV			GetMemMap()	{	return m_pstMemMap;	}	/* ���� �޸� ��ȯ */
};

/* --------------------------------------------------------------------------------------------- */
/*                  Shared Memory - Robot Diamond Data for Logosol with serial                   */
/* --------------------------------------------------------------------------------------------- */

class CMemMDR : public CMemBase
{
public:

	CMemMDR(BOOL engine, LPG_CIEA config);
	virtual ~CMemMDR()	{};

/* ���� �Լ� */
protected:

	virtual BOOL		LinkMemMap();	/* Memory ������ �� �ּ� ���� �۾� */

/* ��� ���� */
protected:

	LPG_MRDV			m_pstMemMap;


/* ���� �Լ� */
public:

	LPG_MRDV			GetMemMap()	{	return m_pstMemMap;	}	/* ���� �޸� ��ȯ */
};

/* --------------------------------------------------------------------------------------------- */
/*                             Shared Memory - EFU Data for Shinsung                             */
/* --------------------------------------------------------------------------------------------- */

class CMemEFU : public CMemBase
{
public:

	CMemEFU(BOOL engine, LPG_CIEA config);
	virtual ~CMemEFU()	{};

/* ���� �Լ� */
protected:

	virtual BOOL		LinkMemMap();	/* Memory ������ �� �ּ� ���� �۾� */

/* ��� ���� */
protected:

	LPG_EPAS			m_pstMemMap;


/* ���� �Լ� */
public:

	LPG_EPAS			GetMemMap()	{	return m_pstMemMap;	}	/* ���� �޸� ��ȯ */
};

/* --------------------------------------------------------------------------------------------- */
/*                                   Shared Memory - BSA Object                                  */
/* --------------------------------------------------------------------------------------------- */

class CMemBSA : public CMemBase
{
public:

	CMemBSA(BOOL engine, LPG_CIEA config);
	virtual ~CMemBSA()	{};

/* ���� �Լ� */
protected:

	virtual BOOL		LinkMemMap();	/* Memory ������ �� �ּ� ���� �۾� */


/* ��� ���� */
protected:

	LPG_RBVC			m_pstMemMap;


/* ���� �Լ� */
public:

	LPG_RBVC			GetMemMap()	{	return m_pstMemMap;	}	/* ���� �޸� ��ȯ */
};

/* --------------------------------------------------------------------------------------------- */
/*                             Shared Memory - LED Power Measurement                             */
/* --------------------------------------------------------------------------------------------- */

class CMemPM100D : public CMemBase
{
public:

	CMemPM100D(BOOL engine, LPG_CIEA config);
	virtual ~CMemPM100D()	{};

/* ���� �Լ� */
protected:

	virtual BOOL		LinkMemMap();	/* Memory ������ �� �ּ� ���� �۾� */

/* ��� ���� */
protected:

	LPG_PADV			m_pstMemMap;


/* ���� �Լ� */
public:

	LPG_PADV			GetMemMap()	{	return m_pstMemMap;	}	/* ���� �޸� ��ȯ */
};

