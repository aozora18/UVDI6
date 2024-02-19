#pragma once

#include "../../inc/conf/trig_uvdi15.h"
#include "../../inc/conf/recipe_uvdi15.h"
#include "../../inc/conf/vision_uvdi15.h"	//���� ���� �̽�
#include "../../inc/conf/pm100.h"
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


/* ���� �Լ� */
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
/*                                  Shared Memory - MC2b Object                                   */
/* --------------------------------------------------------------------------------------------- */

class CMemMC2b : public CMemBase
{
public:

	CMemMC2b(BOOL engine, LPG_CIEA config);
	virtual ~CMemMC2b() {};

	/* ���� �Լ� */
protected:

	virtual BOOL		LinkMemMap();	/* Memory ������ �� �ּ� ���� �۾� */


/* ��� ���� */
protected:

	LPG_MDSM			m_pstMemMap;


	/* ���� �Լ� */
public:

	LPG_MDSM			GetMemMap() { return m_pstMemMap; }	/* ���� �޸� ��ȯ */
};


/* --------------------------------------------------------------------------------------------- */
/*                                Shared Memory - Trigger Object                                 */
/* --------------------------------------------------------------------------------------------- */

class CMemTrig : public CMemBase
{
public:

	CMemTrig(BOOL engine, LPG_CIEA config);
	virtual ~CMemTrig()	{};

/* ���� �Լ� */
protected:

	virtual BOOL		LinkMemMap();	/* Memory ������ �� �ּ� ���� �۾� */


/* ��� ���� */
protected:

	LPG_TPQR			m_pstMemMap;
	LPG_DLSM			m_pstLink;


/* ���� �Լ� */
public:

	LPG_TPQR			GetMemMap()		{	return m_pstMemMap;	}	/* ���� �޸� ��ȯ */
	LPG_DLSM			GetMemLink()	{	return m_pstLink;	}	/* ��� ���� ���� */
};

/* --------------------------------------------------------------------------------------------- */
/*                                 Shared Memory - Vision Object                                 */
/* --------------------------------------------------------------------------------------------- */

class CMemVisi : public CMemBase
{
public:

	CMemVisi(BOOL engine, LPG_CIEA config);
	virtual ~CMemVisi();

/* ���� �Լ� */
protected:

	virtual BOOL		LinkMemMap();	/* Memory ������ �� �ּ� ���� �۾� */


/* ��� ���� */
protected:

	LPG_VDSM			m_pstMemMap;


/* ���� �Լ� */
public:

	LPG_VDSM			GetMemMap()	{	return m_pstMemMap;	}	/* ���� �޸� ��ȯ */
};

/* --------------------------------------------------------------------------------------------- */
/*                       Shared Memory - Recipe for Photohead Offset (step)                      */
/* --------------------------------------------------------------------------------------------- */

class CMemPhStep : public CMemBase
{
public:

	CMemPhStep(BOOL engine, LPG_CIEA config);
	virtual ~CMemPhStep()	{};

/* ���� �Լ� */
protected:

	virtual BOOL		LinkMemMap();	/* Memory ������ �� �ּ� ���� �۾� */

/* ��� ���� */
protected:

	LPG_OSSD			m_pstMemMap;


/* ���� �Լ� */
public:

	LPG_OSSD			GetMemMap()	{	return m_pstMemMap;	}	/* ���� �޸� ��ȯ */
};

/* --------------------------------------------------------------------------------------------- */
/*                Shared Memory - Recipe for Photohead Correction (Strip) Y Table                */
/* --------------------------------------------------------------------------------------------- */

class CMemPhCorrectY : public CMemBase
{
public:

	CMemPhCorrectY(BOOL engine, LPG_CIEA config);
	virtual ~CMemPhCorrectY()	{};

/* ���� �Լ� */
protected:

	virtual BOOL		LinkMemMap();	/* Memory ������ �� �ּ� ���� �۾� */

/* ��� ���� */
protected:

	LPG_OSCY			m_pstMemMap;


/* ���� �Լ� */
public:

	LPG_OSCY			GetMemMap()	{	return m_pstMemMap;	}	/* ���� �޸� ��ȯ */
};

/* --------------------------------------------------------------------------------------------- */
/*                             Shared Memory - LED Power Measurement(Gentec)                     */
/* --------------------------------------------------------------------------------------------- */

class CMemGentec : public CMemBase
{
public:

	CMemGentec(BOOL engine, LPG_CIEA config);
	virtual ~CMemGentec()	{};

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

/* --------------------------------------------------------------------------------------------- */
/*                                Shared Memory - Trigger Object(Mvence)                         */
/* --------------------------------------------------------------------------------------------- */

class CMemMvenc : public CMemBase
{
public:

	CMemMvenc(BOOL engine, LPG_CIEA config);
	virtual ~CMemMvenc() {};

	/* ���� �Լ� */
protected:

	virtual BOOL		LinkMemMap();	/* Memory ������ �� �ּ� ���� �۾� */


/* ��� ���� */
protected:

	LPG_TPQR			m_pstMemMap;
	LPG_DLSM			m_pstLink;


	/* ���� �Լ� */
public:

	LPG_TPQR			GetMemMap() { return m_pstMemMap; }	/* ���� �޸� ��ȯ */
	LPG_DLSM			GetMemLink() { return m_pstLink; }	/* ��� ���� ���� */
};

/* --------------------------------------------------------------------------------------------- */
/*                                Shared Memory - Trigger Object(Mvence)                         */
/* --------------------------------------------------------------------------------------------- */

class CMemPhilhmi : public CMemBase
{
public:

	CMemPhilhmi(BOOL engine, LPG_CIEA config);
	virtual ~CMemPhilhmi() {};

	/* ���� �Լ� */
protected:

	virtual BOOL		LinkMemMap();	/* Memory ������ �� �ּ� ���� �۾� */


/* ��� ���� */
protected:

	LPG_PPR				m_pstMemMap;
	LPG_DLSM			m_pstLink;


	/* ���� �Լ� */
public:

	LPG_PPR			GetMemMap() { return m_pstMemMap; }	/* ���� �޸� ��ȯ */
	LPG_DLSM		GetMemLink() { return m_pstLink; }	/* ��� ���� ���� */
};

/* --------------------------------------------------------------------------------------------- */
/*                                Shared Memory - Trigger Object(Mvence)                         */
/* --------------------------------------------------------------------------------------------- */

class CMemStrobeLamp : public CMemBase
{
public:

	CMemStrobeLamp(BOOL engine, LPG_CIEA config);
	virtual ~CMemStrobeLamp() {};

	/* ���� �Լ� */
protected:

	virtual BOOL		LinkMemMap();	/* Memory ������ �� �ּ� ���� �۾� */


	/* ��� ���� */
protected:

	LPG_SLPR			m_pstMemMap;
	LPG_DLSM			m_pstLink;


	/* ���� �Լ� */
public:

	LPG_SLPR		GetMemMap() { return m_pstMemMap; }	/* ���� �޸� ��ȯ */
	LPG_DLSM		GetMemLink() { return m_pstLink; }	/* ��� ���� ���� */
};