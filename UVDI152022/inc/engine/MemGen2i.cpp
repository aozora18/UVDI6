
/*
 desc : ��� (Ȥ�� ��ġ) �⺻ ��ü
*/

#include "pch.h"
#include "MemGen2i.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/* --------------------------------------------------------------------------------------------- */
/*                                 Shared Memory - Base Object                                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ������
 parm : engine	- [in]  Engine ���� (TRUE: Engine ���� ȣ��, FALSE: �Ϲ� ����UI S/W���� ȣ��)
		config	- [in]  ��ü ȯ�� ����
		dev_type- [in]  ENG_MMDC ��, MC2 / LURIA / PLC / TRIGGER / STROBE / MIL / Basler / IDS / Etc
		mem_size- [in]  Shared Memory Size (0 ���̸�, �⺻ ������ ����. ����: bytes)
 retn : None
*/
CMemBase::CMemBase(BOOL engine, LPG_CIEA config, ENG_MMDC dev_type)
{
	m_pShMemKey	= NULL;

	/* ȣ���� �θ� ���μ����� Engine or Normal (GUI) Program */
	m_bIsEngine	= engine;
	m_enDevType	= dev_type;
	m_u32MemSize= 0;
	/* ��ü ȯ�� ���� ���� */
	m_pstConfig	= config;
	/* Memory Mapped Object ���� */
	m_pMemMap	= new CMemMap;
	ASSERT(m_pMemMap);
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CMemBase::~CMemBase()
{
	/* Memory Mapped Object ���� */
	if (m_pMemMap)
	{
		CloseMap();
		delete m_pMemMap;
		m_pMemMap	= NULL;
	}
}

/*
 desc : ���� �޸� ���� �� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemBase::CreateMap()
{
	UINT32 u32MemSize	= 0;

	/* ���� �޸� Ű ���� �ߴ��� Ȯ�� */
	if (!m_pShMemKey)	return FALSE;

	if (m_bIsEngine)
	{
		/* Memory Mapped ���� �Ҵ��Ϸ��� ũ�� ��� */
		u32MemSize	= CalcMemSize();
		if (u32MemSize < 8)	return FALSE;
		if (!m_pMemMap->CreateMap(m_pShMemKey, u32MemSize))
		{
			/* Ȥ�� �𸣴ϱ� ���� ���� Map�� ���� ���� �ֱ� ������ ���� ���� */
			CloseMap();
			/* �׸��� ���� ��ȯ */
			return FALSE;
		}
	}
	else
	{
		if (!m_pMemMap->OpenMap(m_pShMemKey))	return FALSE;
	}
	
	/* �� �Ҵ�� �޸� ������ ��� ������ ����Ű�� �ּ� ���� (���� ������ �ؾ� �ϳ�...) */
	return LinkMemMap();
}

/*
 desc : ���� �޸� ����
 parm : None
 retn : TRUE or FALSE
*/
VOID CMemBase::CloseMap()
{
	if (m_pMemMap)	m_pMemMap->CloseMap();
}

/*
 desc : ���� �޸� ��ȯ
 parm : None
 retn : ���� �޸� ������
*/
PUINT8 CMemBase::GetMemMap()
{
	return m_pMemMap->GetMemMap();
}

/*
 desc : ���� �޸� ���� ũ�� ���
 parm : None
 retn : �ּ��� 8 �̻� ���� ��ȯ�Ǿ�� ��. 8 ���� ���� ����
		���� �޸� ���� ũ�� (����: Bytes. 8 Bytes ����� �Է��� ��� ��)
		���� AppType ���� Engine�� �ƴϸ�, �Է����� �ʾƵ� ��
*/
UINT32 CMemBase::CalcMemSize()
{
	if (m_u32MemSize == 0)
	{
		switch (m_enDevType)
		{
		case ENG_MMDC::en_conf				:	m_u32MemSize = CalcMemSizeOfConfig();		break;
		case ENG_MMDC::en_conf_teaching		:	m_u32MemSize = CalcMemSizeOfTeaching();		break;
		case ENG_MMDC::en_conf_expo_param	:	m_u32MemSize = CalcMemSizeOfExpoParam();	break;
		case ENG_MMDC::en_conf_tracking		:	m_u32MemSize = CalcMemSizeOfTracking();		break;
		case ENG_MMDC::en_luria				:	m_u32MemSize = CalcMemSizeOfLuria();		break;
		case ENG_MMDC::en_mc2				:	m_u32MemSize = CalcMemSizeOfMC2();			break;
		case ENG_MMDC::en_bsa				:	m_u32MemSize = CalcMemSizeOfBSA();			break;
		case ENG_MMDC::en_efu_ss			:	m_u32MemSize = CalcMemSizeOfEFU();			break;
		case ENG_MMDC::en_plc_q				:	m_u32MemSize = CalcMemSizeOfPLCQ();			break;
		case ENG_MMDC::en_lspa				:	m_u32MemSize = CalcMemSizeOfLSPA();			break;
		case ENG_MMDC::en_lsdr				:	m_u32MemSize = CalcMemSizeOfLSDR();			break;
		case ENG_MMDC::en_pm100d			:	m_u32MemSize = CalcMemSizeOfPM100D();		break;
		}
	}
	/* �޸� �Ҵ� ũ�� ��� ������ ������� Ȯ�� */
	if (m_u32MemSize < 1)
	{
		AfxMessageBox(_T("The allocation size of the memory must be a least 8 bytes"), MB_ICONSTOP|MB_TOPMOST);
		return 0;
	}

	/* u32MemSize�� 8�� ������� ������, ������ 8�� ����� �������� �� */
	if (0 != (m_u32MemSize % 8))	m_u32MemSize	= (m_u32MemSize / 8) * 8 + 8;
#ifdef _DEBUG
	TRACE(L"The structure size of shared memory [shmem_id : %02x] <size : %u bytes>\n",
		  (UINT8)m_enDevType, m_u32MemSize);
#endif
	/* ���� �����Ǵ� ���� �޸� ũ�� �� ��ȯ */
	return m_u32MemSize;
}

/*
 desc : MC2 ���� �޸� ���� ũ�� ���
 parm : None
 retn : �ּ��� 8 �̻� ���� ��ȯ�Ǿ�� ��. 8 ���� ���� ����
*/
UINT32 CMemBase::CalcMemSizeOfMC2()
{
#if 0
	/* STG_MDSM �� 8 ���� SD2S ����̺��� ��� ������ ����� �޸� ���� ũ�� */ 
	return UINT32(sizeof(STG_RVCH) + sizeof(STG_AVCH) +
				  (sizeof(STG_RVCD) * MAX_MC2_DRIVE) + (sizeof(STG_AVCD) * MAX_MC2_DRIVE) +
				  sizeof(STG_MPRD) + sizeof(STG_DLSM));
#else
	return UINT32(sizeof(STG_MDSM));
#endif
}

/*
 desc : Luria ���� �޸� ���� ũ�� ���
 parm : None
 retn : �ּ��� 8 �̻� ���� ��ȯ�Ǿ�� ��. 8 ���� ���� ����
*/
UINT32 CMemBase::CalcMemSizeOfBSA()
{
	return UINT32(sizeof(STG_RBVC));
}

/*
 desc : Luria ���� �޸� ���� ũ�� ���
 parm : None
 retn : �ּ��� 8 �̻� ���� ��ȯ�Ǿ�� ��. 8 ���� ���� ����
*/
UINT32 CMemBase::CalcMemSizeOfLuria()
{
#if 0
	UINT32 u32RecvCheck, u32Link;
	UINT32 u32MachineConfig, u32JobManagement, u32PanelPrepare;
	UINT32 u32Exposure, u32PhDirectCommand, u32CommMgt, u32System;

	/* recv check */
	u32RecvCheck		= MAX_CMD_FAMILY_ID * MAX_CMD_USER_ID * sizeof(UINT8);
	/* MachineConfig Size */
	u32MachineConfig	= sizeof(STG_LDMC);
	u32JobManagement	= sizeof(STG_LDJM);
	u32PanelPrepare		= sizeof(STG_LDPP);
	u32Exposure			= sizeof(STG_LDEW);
	u32PhDirectCommand	= sizeof(STG_LDDP);
	u32CommMgt			= sizeof(STG_LDCM);
	u32System			= sizeof(STG_LDSS);
	/* Comms.Setup */
	u32Link				= sizeof(STG_DLSM);

	return UINT32(u32RecvCheck + u32MachineConfig + u32JobManagement + u32PanelPrepare + 
				  u32Exposure + u32PhDirectCommand + u32CommMgt + u32System + u32Link);
#else
	return UINT32(sizeof(STG_LDSM));
#endif
}

/*
 desc : PLC_Q ���� �޸� ���� ũ�� ���
 parm : None
 retn : �ּ��� 8 �̻� ���� ��ȯ�Ǿ�� ��. 8 ���� ���� ����
*/
UINT32 CMemBase::CalcMemSizeOfPLCQ()
{
	/* Input & Output �� �ƹ��� ���ƾ� 128 Word�� ���� �ʴ´� */ 
#if 0
	return (sizeof(STG_PDSM) - sizeof(PUINT8)) +
			UINT32(m_pstConfig->melsec_q_svc.addr_d_count * sizeof(UINT16));
#else
	/* Input & Output �� �ƹ��� ���ƾ� 128 Word�� ���� �ʴ´� */
#ifdef _DEBUG
	INT32 i32Size1 = sizeof(STG_DLSM);
	INT32 i32Size2 = sizeof(STG_PVSM);
	INT32 i32Size3 = UINT32(m_pstConfig->melsec_q_svc.addr_d_count * sizeof(UINT16));
#endif
	return (sizeof(STG_DLSM) + sizeof(STG_PVSM)) +
			UINT32(m_pstConfig->melsec_q_svc.addr_d_count * sizeof(UINT16));
#endif
}

/*
 desc : Config ���� �޸� ���� ũ�� ���
 parm : None
 retn : �ּ��� 8 �̻� ���� ��ȯ�Ǿ�� ��. 8 ���� ���� ����
*/
UINT32 CMemBase::CalcMemSizeOfConfig()
{
	return UINT32(sizeof(STG_CIEA));
}
UINT32 CMemBase::CalcMemSizeOfEFU()
{
	return UINT32(sizeof(STG_EPAS) * MAX_EFU_COUNT);
}
UINT32 CMemBase::CalcMemSizeOfTeaching()
{
	return UINT32(sizeof(STG_CTPI));
}
UINT32 CMemBase::CalcMemSizeOfExpoParam()
{
	return UINT32(sizeof(STG_CTEI));
}
UINT32 CMemBase::CalcMemSizeOfTracking()
{
	return UINT32(sizeof(STG_CWTI));
}
UINT32 CMemBase::CalcMemSizeOfLSPA()
{
	return UINT32(sizeof(STG_MPDV));
}
UINT32 CMemBase::CalcMemSizeOfLSDR()
{
	return UINT32(sizeof(STG_MRDV));
}
UINT32 CMemBase::CalcMemSizeOfPM100D()
{
	return UINT32(sizeof(STG_PADV));
}
UINT32 CMemBase::CalcMemSizeOfPhilhmi()
{
	return UINT32(sizeof(LPG_PPR));
}

/* --------------------------------------------------------------------------------------------- */
/*                                Shared Memory - Configuration                                  */
/* --------------------------------------------------------------------------------------------- */

CMemConf::CMemConf(BOOL engine)
	: CMemBase(engine, NULL, ENG_MMDC::en_conf)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_CONFIG;
}
BOOL CMemConf::LinkMemMap()
{
	/* ���� �޸� ������ ���� */
	m_pstMemMap	= (LPG_CIEA)m_pMemMap->GetMemMap();
	if (!m_pstMemMap)	return FALSE;

	return TRUE;
}
CMemConfTeaching::CMemConfTeaching(BOOL engine)
	: CMemBase(engine, NULL, ENG_MMDC::en_conf_teaching)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_CONFIG_TEACH;
}
BOOL CMemConfTeaching::LinkMemMap()
{
	/* ���� �޸� ������ ���� */
	m_pstMemMap	= (LPG_CTPI)m_pMemMap->GetMemMap();
	if (!m_pstMemMap)	return FALSE;

	return TRUE;
}
CMemConfExpoParam::CMemConfExpoParam(BOOL engine)
	: CMemBase(engine, NULL, ENG_MMDC::en_conf_expo_param)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_CONFIG_EXPO_PARAM;
}
BOOL CMemConfExpoParam::LinkMemMap()
{
	/* ���� �޸� ������ ���� */
	m_pstMemMap	= (LPG_CTEI)m_pMemMap->GetMemMap();
	if (!m_pstMemMap)	return FALSE;

	return TRUE;
}
CMemConfTracking::CMemConfTracking(BOOL engine)
	: CMemBase(engine, NULL, ENG_MMDC::en_conf_tracking)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_TRACKING;
}
BOOL CMemConfTracking::LinkMemMap()
{
	PUINT8 pMemMap	= NULL;
	
	/* ���� �޸� ������ ���� */
	pMemMap		= m_pMemMap->GetMemMap();
	m_pstMemMap	= (LPG_CWTI)pMemMap;

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                 Shared Memory - Luria Object                                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ������
 parm : engine	- [in]  Engine ���� (TRUE: Engine ���� ȣ��, FALSE: �Ϲ� ����UI S/W���� ȣ��)
		config	- [in]  ��ü ȯ�� ����
 retn : None
*/
CMemLuria::CMemLuria(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_luria)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_LURIA;

}

/*
 desc : ���� Ȥ�� ���ȴ� �޸� ������ �� �ʿ��� ����ü ��� ������ �ּҿ� ����
		��, ���� �����(��) ������ �ʿ��� ���� ���� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemLuria::LinkMemMap()
{
	/* ���� �޸� ������ ���� */
	m_pstMemMap	= (LPG_LDSM)m_pMemMap->GetMemMap();
	if (!m_pstMemMap)	return FALSE;

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                  Shared Memory - MC2 Object                                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ������
 parm : engine	- [in]  Engine ���� (TRUE: Engine ���� ȣ��, FALSE: �Ϲ� ����UI S/W���� ȣ��)
		config	- [in]  ��ü ȯ�� ����
 retn : None
*/
CMemMC2::CMemMC2(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_mc2)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_MC2;
}

/*
 desc : ���� Ȥ�� ���ȴ� �޸� ������ �� �ʿ��� ����ü ��� ������ �ּҿ� ����
		��, ���� �����(��) ������ �ʿ��� ���� ���� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemMC2::LinkMemMap()
{
	/* ���� �޸� ������ ���� */
	m_pstMemMap	= (LPG_MDSM)m_pMemMap->GetMemMap();
	if (!m_pstMemMap)	return FALSE;

	/* ���������� ����Ǿ� �ִ��� Ȯ��? ���� ��ƾ ? */

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                  Shared Memory - PLC Object                                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ������
 parm : engine	- [in]  Engine ���� (TRUE: Engine ���� ȣ��, FALSE: �Ϲ� ����UI S/W���� ȣ��)
		config	- [in]  ȯ�� ���� ����
 retn : None
*/
CMemPLC::CMemPLC(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_plc_q)
{
	m_pShMemKey	= SH_MEM_PLC;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CMemPLC::~CMemPLC()
{
	::Free(m_pstMemMap);
}

/*
 desc : ���� Ȥ�� ���ȴ� �޸� ������ �� �ʿ��� ����ü ��� ������ �ּҿ� ����
		��, ���� �����(��) ������ �ʿ��� ���� ���� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemPLC::LinkMemMap()
{
	PUINT8 pMemNext		= NULL;

	/* ���� �޸� ������ ���� */
	pMemNext	= m_pMemMap->GetMemMap();
	m_pstMemMap	= (LPG_PDSM)::Alloc(sizeof(STG_PDSM));
	ASSERT(m_pstMemMap);
	m_pstMemMap->link	= (LPG_DLSM)pMemNext;
	pMemNext	+= sizeof(STG_DLSM);
	m_pstMemMap->setv	= (LPG_PVSM)pMemNext;
	pMemNext	+= sizeof(STG_PVSM);
	m_pstMemMap->data	= (PUINT8)pMemNext;

	return TRUE;
}

/*
 desc : Ư�� ���� �ּ��� �޸� ������ �� [ 1 Bit �� ] ��ȯ
 parm : addr	- [in]  ��Ʈ ������ ����� �ּ� (BIT Index ��. ���� �޸� ���� �ּ� ��)
						Zero based ����. 0 �������� ���� (100���� ��û�� ���, 101��° �޸� ��)
		bits	- [in]  Bit Position (0x00 ~ 0x0f)
		value	- [out] ��ȯ�Ǿ� ����� ���� ��
 retn : TRUE or FALSE
*/
BOOL CMemPLC::GetBitValue(UINT32 addr, UINT8 bits, UINT8 &value)
{
	UINT16 u16Value	= 0;
	UINT32 u32Index	= 0;	/* �������� �޸��� �ּ� �ε��� (Zero-based ~ ) */
	PUINT16 pMemMap	= (PUINT16)m_pstMemMap->data;	/* 2 Bytes (WORD)�� ������ ���� */

	/* bits ��ġ�� 0x0f���� ū ������ �׸��� �ּ� ���� ��ȿ���� Ȯ�� */
	if (bits > 0x0f || m_pstConfig->melsec_q_svc.start_d_addr > addr)	return FALSE;
	/* �Էµ� �ּ� ���� ū ����� */
	if ((addr - m_pstConfig->melsec_q_svc.start_d_addr) >= m_pstConfig->melsec_q_svc.addr_d_count)	return FALSE;

	/* ���� �޸��� WORD ����� �ε��� �� ��� */
	u32Index	= addr - m_pstConfig->melsec_q_svc.start_d_addr;
	/* ���� WORD ������ �ּ� �� ��� */
	u16Value	= pMemMap[u32Index];

	/* Word �� �߿��� �ش� ��ġ�� Bit �� ���� */
	value		= (0x0001 & (u16Value >> bits));

	return TRUE;
}

/*
 desc : Ư�� ���� �ּ��� �޸� ������ �� [ 1 Bytes �� ] ��ȯ
 parm : addr	- [in]  ��Ʈ ������ ����� �ּ� (BIT Index ��. ���� �޸� ���� �ּ� ��)
						Zero based ����. 0 �������� ���� (100���� ��û�� ���, 101��° �޸� ��)
		flag	- [in]  0x00: Low, 0x01: High
		value	- [out] ��ȯ�Ǿ� ����� ���� ��
 retn : TRUE or FALSE
*/
BOOL CMemPLC::GetByteValue(UINT32 addr, UINT8 flag, UINT8 &value)
{
	UINT16 u16Value	= 0;
	UINT32 u32Index	= 0;	/* �������� �޸��� �ּ� �ε��� (Zero-based ~ ) */
	PUINT16 pMemMap	= (PUINT16)m_pstMemMap->data;	/* 2 Bytes (WORD)�� ������ ���� */

	/* bits ��ġ�� 0x0f���� ū ������ �׸��� �ּ� ���� ��ȿ���� Ȯ�� */
	if (m_pstConfig->melsec_q_svc.start_d_addr > addr)	return FALSE;
	/* �Էµ� �ּ� ���� ū ����� */
	if ((addr - m_pstConfig->melsec_q_svc.start_d_addr) >= m_pstConfig->melsec_q_svc.addr_d_count)	return FALSE;

	/* ���� �޸��� WORD ����� �ε��� �� ��� */
	u32Index	= addr - m_pstConfig->melsec_q_svc.start_d_addr;
	/* ���� WORD ������ �ּ� �� ��� */
	u16Value	= pMemMap[u32Index];

	/* Word �� �߿��� �ش� ��ġ�� Bit �� ���� */
	if (flag)	value	= (0xff00 & u16Value) >> 8;	/* High 1 byte */
	else		value	= (0x00ff & u16Value);		/* Low 1 byte */

	return TRUE;
}

/*
 desc : Ư�� ���� �ּ��� �޸� ������ �� [ 2 Bytes �� ] ��ȯ
 parm : addr	- [in]  ��Ʈ ������ ����� �ּ� (BIT Index ��. ���� �޸� ���� �ּ� ��)
						Zero based ����. 0 �������� ���� (100���� ��û�� ���, 101��° �޸� ��)
		flag	- [in]  0x00: Low, 0x01: High
		value	- [out] ��ȯ�Ǿ� ����� ���� ��
 retn : TRUE or FALSE
*/
BOOL CMemPLC::GetWordValue(ENG_PIOA addr, UINT16 &value)
{
	UINT32 u32Addr	= UINT32(addr);
	u32Addr	>>= 4;	/* u32Addr= u32Addr >> 4; */
	return GetWordValue(UINT32(u32Addr), value);
}
BOOL CMemPLC::GetWordValue(UINT32 addr, UINT16 &value)
{
	PUINT16 pMemMap	= (PUINT16)m_pstMemMap->data;	/* 2 Bytes (WORD)�� ������ ���� */

	/* bits ��ġ�� 0x0f���� ū ������ �׸��� �ּ� ���� ��ȿ���� Ȯ�� */
	if (m_pstConfig->melsec_q_svc.start_d_addr > addr)
	{
		LOG_ERROR(ENG_EDIC::en_melsec, L"The address value is too small.");
		return FALSE;
	}
	/* �Էµ� �ּ� ���� ū ����� */
	if ((addr - m_pstConfig->melsec_q_svc.start_d_addr) >= m_pstConfig->melsec_q_svc.addr_d_count)
	{
		LOG_ERROR(ENG_EDIC::en_melsec, L"The address value is too large.");
		return FALSE;
	}

	/* ���� WORD ������ �ּ� �� ��� */
	value	= pMemMap[addr - m_pstConfig->melsec_q_svc.start_d_addr];

	return TRUE;
}

/*
 desc : ���� �޸� �ּ��� �޸� ������ �� [ 2 Bytes �� ] ��ȯ
 parm : index	- [in]  �޸��� ��ġ �� (0 or Later)
		value	- [out] ��ȯ�Ǿ� ����� ���� ��
 retn : TRUE or FALSE
*/
BOOL CMemPLC::GetWordValue(UINT16 index, UINT16 &value)
{
	PUINT16 pMemMap	= (PUINT16)m_pstMemMap->data;	/* 2 Bytes (WORD)�� ������ ���� */

	/* �޸� �ε��� ��ġ�� �޸� ũ�� (2 bytes ����)���� ũ�ų� ������ ���� */
	if (m_pstConfig->melsec_q_svc.addr_d_count <= index)	return FALSE;

	/* WORD ������ �� ��� */
	value	= pMemMap[index];

	return TRUE;
}

/*
 desc : Ư�� ���� �ּ��� �޸� ������ �� [ 4 Bytes �� ] ��ȯ
 parm : addr	- [in]  ��Ʈ ������ ����� �ּ� (BIT Index ��. ���� �޸� ���� �ּ� ��)
						Zero based ����. 0 �������� ���� (100���� ��û�� ���, 101��° �޸� ��)
		flag	- [in]  0x00: Low, 0x01: High
		value	- [out] ��ȯ�Ǿ� ����� ���� ��
 retn : TRUE or FALSE
*/
BOOL CMemPLC::GetDWordValue(UINT32 addr, UINT32 &value)
{
	PUINT16 pMemMap	= (PUINT16)m_pstMemMap->data;	/* 2 Bytes (WORD)�� ������ ���� */

	/* bits ��ġ�� 0x0f���� ū ������ �׸��� �ּ� ���� ��ȿ���� Ȯ�� */
	if (m_pstConfig->melsec_q_svc.start_d_addr <= addr)	return FALSE;
	/* �Էµ� �ּ� ���� ū ����� */
	if ((addr + 1 /* for DWord */ - m_pstConfig->melsec_q_svc.start_d_addr) >=
		m_pstConfig->melsec_q_svc.addr_d_count)	return FALSE;

	/* ���� WORD ������ �ּ� �� ��� */
	memcpy(&value, &pMemMap[addr - m_pstConfig->melsec_q_svc.start_d_addr], 4);

	return TRUE;
}

/*
 desc : ���� �޸� �ּ��� �޸� ������ �� [ 4 Bytes �� ] ��ȯ
 parm : index	- [in]  �޸��� ��ġ �� (0 or Later)
		value	- [out] ��ȯ�Ǿ� ����� ���� ��
 retn : TRUE or FALSE
*/
BOOL CMemPLC::GetDWordValue(UINT16 index, UINT32 &value)
{
	PUINT16 pMemMap	= (PUINT16)m_pstMemMap->data;	/* 2 Bytes (WORD)�� ������ ���� */

	/* �޸� �ε��� ��ġ�� �޸� ũ�� (2 bytes ����)���� ũ�ų� ������ ���� */
	if (m_pstConfig->melsec_q_svc.addr_d_count <= index)	return FALSE;

	/* WORD ������ �� ��� */
	memcpy(&value, pMemMap+index, 4);

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                    Shared Memory - PreAligner Data for Logosol with serial                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ������
 parm : engine	- [in]  Engine ���� (TRUE: Engine ���� ȣ��, FALSE: �Ϲ� ����UI S/W���� ȣ��)
		config	- [in]  ��ü ȯ�� ����
 retn : None
*/
CMemMPA::CMemMPA(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_lspa)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_LSPA;
}

/*
 desc : ���� Ȥ�� ���ȴ� �޸� ������ �� �ʿ��� ����ü ��� ������ �ּҿ� ����
		��, ���� �����(��) ������ �ʿ��� ���� ���� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemMPA::LinkMemMap()
{
	PUINT8 pMemMap	= NULL;
	
	/* ���� �޸� ������ ���� */
	pMemMap		= m_pMemMap->GetMemMap();
	m_pstMemMap	= (LPG_MPDV)pMemMap;

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                  Shared Memory - Diamond Robot Data for Logosol with serial                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ������
 parm : engine	- [in]  Engine ���� (TRUE: Engine ���� ȣ��, FALSE: �Ϲ� ����UI S/W���� ȣ��)
		config	- [in]  ��ü ȯ�� ����
 retn : None
*/
CMemMDR::CMemMDR(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_lsdr)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_LSDR;
}

/*
 desc : ���� Ȥ�� ���ȴ� �޸� ������ �� �ʿ��� ����ü ��� ������ �ּҿ� ����
		��, ���� �����(��) ������ �ʿ��� ���� ���� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemMDR::LinkMemMap()
{
	PUINT8 pMemMap	= NULL;
	
	/* ���� �޸� ������ ���� */
	pMemMap		= m_pMemMap->GetMemMap();
	m_pstMemMap	= (LPG_MRDV)pMemMap;

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                               Shared Memory - EFU for Shinsung                                */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ������
 parm : engine	- [in]  Engine ���� (TRUE: Engine ���� ȣ��, FALSE: �Ϲ� ����UI S/W���� ȣ��)
		config	- [in]  ��ü ȯ�� ����
 retn : None
*/
CMemEFU::CMemEFU(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_efu_ss)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_EFU_SS;
}

/*
 desc : ���� Ȥ�� ���ȴ� �޸� ������ �� �ʿ��� ����ü ��� ������ �ּҿ� ����
		��, ���� �����(��) ������ �ʿ��� ���� ���� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemEFU::LinkMemMap()
{
	PUINT8 pMemMap	= NULL;
	
	/* ���� �޸� ������ ���� */
	pMemMap		= m_pMemMap->GetMemMap();
	m_pstMemMap	= (LPG_EPAS)pMemMap;

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                       Shared Memory - BSA                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ������
 parm : engine	- [in]  Engine ���� (TRUE: Engine ���� ȣ��, FALSE: �Ϲ� ����UI S/W���� ȣ��)
		config	- [in]  ��ü ȯ�� ����
 retn : None
*/
CMemBSA::CMemBSA(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_bsa)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_BSA;
}

/*
 desc : ���� Ȥ�� ���ȴ� �޸� ������ �� �ʿ��� ����ü ��� ������ �ּҿ� ����
		��, ���� �����(��) ������ �ʿ��� ���� ���� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemBSA::LinkMemMap()
{
	PUINT8 pMemMap	= NULL;
	
	/* ���� �޸� ������ ���� */
	pMemMap		= m_pMemMap->GetMemMap();
	m_pstMemMap	= (LPG_RBVC)pMemMap;

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                             Shared Memory - LED Power Measurement                             */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ������
 parm : engine	- [in]  Engine ���� (TRUE: Engine ���� ȣ��, FALSE: �Ϲ� ����UI S/W���� ȣ��)
		config	- [in]  ��ü ȯ�� ����
 retn : None
*/
CMemPM100D::CMemPM100D(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_pm100d)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_PM100D;
}

/*
 desc : ���� Ȥ�� ���ȴ� �޸� ������ �� �ʿ��� ����ü ��� ������ �ּҿ� ����
		��, ���� �����(��) ������ �ʿ��� ���� ���� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemPM100D::LinkMemMap()
{
	PUINT8 pMemMap	= NULL;
	
	/* ���� �޸� ������ ���� */
	pMemMap		= m_pMemMap->GetMemMap();
	m_pstMemMap	= (LPG_PADV)pMemMap;

	return TRUE;
}
