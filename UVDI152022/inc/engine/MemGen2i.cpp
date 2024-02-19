
/*
 desc : 장비 (혹은 장치) 기본 객체
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
 desc : 생성자
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		config	- [in]  전체 환경 정보
		dev_type- [in]  ENG_MMDC 즉, MC2 / LURIA / PLC / TRIGGER / STROBE / MIL / Basler / IDS / Etc
		mem_size- [in]  Shared Memory Size (0 값이면, 기본 값으로 설정. 단위: bytes)
 retn : None
*/
CMemBase::CMemBase(BOOL engine, LPG_CIEA config, ENG_MMDC dev_type)
{
	m_pShMemKey	= NULL;

	/* 호출한 부모 프로세스가 Engine or Normal (GUI) Program */
	m_bIsEngine	= engine;
	m_enDevType	= dev_type;
	m_u32MemSize= 0;
	/* 전체 환경 정보 연결 */
	m_pstConfig	= config;
	/* Memory Mapped Object 생성 */
	m_pMemMap	= new CMemMap;
	ASSERT(m_pMemMap);
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CMemBase::~CMemBase()
{
	/* Memory Mapped Object 제거 */
	if (m_pMemMap)
	{
		CloseMap();
		delete m_pMemMap;
		m_pMemMap	= NULL;
	}
}

/*
 desc : 공유 메모리 생성 및 연결
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemBase::CreateMap()
{
	UINT32 u32MemSize	= 0;

	/* 공유 메모리 키 설정 했는지 확인 */
	if (!m_pShMemKey)	return FALSE;

	if (m_bIsEngine)
	{
		/* Memory Mapped 에서 할당하려는 크기 계산 */
		u32MemSize	= CalcMemSize();
		if (u32MemSize < 8)	return FALSE;
		if (!m_pMemMap->CreateMap(m_pShMemKey, u32MemSize))
		{
			/* 혹시 모르니까 기존 열린 Map이 있을 수도 있기 때문에 강제 종료 */
			CloseMap();
			/* 그리고 에러 반환 */
			return FALSE;
		}
	}
	else
	{
		if (!m_pMemMap->OpenMap(m_pShMemKey))	return FALSE;
	}
	
	/* 각 할당된 메모리 영역에 멤버 변수가 가리키는 주소 연결 (뭐라 설명을 해야 하나...) */
	return LinkMemMap();
}

/*
 desc : 공유 메모리 연결
 parm : None
 retn : TRUE or FALSE
*/
VOID CMemBase::CloseMap()
{
	if (m_pMemMap)	m_pMemMap->CloseMap();
}

/*
 desc : 공유 메모리 반환
 parm : None
 retn : 공유 메모리 포인터
*/
PUINT8 CMemBase::GetMemMap()
{
	return m_pMemMap->GetMemMap();
}

/*
 desc : 공유 메모리 생성 크기 계산
 parm : None
 retn : 최소한 8 이상 값이 반환되어야 함. 8 이하 값은 실패
		공유 메모리 영역 크기 (단위: Bytes. 8 Bytes 배수로 입력해 줘야 함)
		만약 AppType 값이 Engine이 아니면, 입력하지 않아도 됨
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
	/* 메모리 할당 크기 계산 실패인 경우인지 확인 */
	if (m_u32MemSize < 1)
	{
		AfxMessageBox(_T("The allocation size of the memory must be a least 8 bytes"), MB_ICONSTOP|MB_TOPMOST);
		return 0;
	}

	/* u32MemSize가 8의 배수보다 작으면, 무조건 8의 배수로 만들어줘야 됨 */
	if (0 != (m_u32MemSize % 8))	m_u32MemSize	= (m_u32MemSize / 8) * 8 + 8;
#ifdef _DEBUG
	TRACE(L"The structure size of shared memory [shmem_id : %02x] <size : %u bytes>\n",
		  (UINT8)m_enDevType, m_u32MemSize);
#endif
	/* 최종 생성되는 공유 메모리 크기 값 반환 */
	return m_u32MemSize;
}

/*
 desc : MC2 공유 메모리 생성 크기 계산
 parm : None
 retn : 최소한 8 이상 값이 반환되어야 함. 8 이하 값은 실패
*/
UINT32 CMemBase::CalcMemSizeOfMC2()
{
#if 0
	/* STG_MDSM 에 8 개의 SD2S 드라이브의 모든 정보가 저장될 메모리 공간 크기 */ 
	return UINT32(sizeof(STG_RVCH) + sizeof(STG_AVCH) +
				  (sizeof(STG_RVCD) * MAX_MC2_DRIVE) + (sizeof(STG_AVCD) * MAX_MC2_DRIVE) +
				  sizeof(STG_MPRD) + sizeof(STG_DLSM));
#else
	return UINT32(sizeof(STG_MDSM));
#endif
}

/*
 desc : Luria 공유 메모리 생성 크기 계산
 parm : None
 retn : 최소한 8 이상 값이 반환되어야 함. 8 이하 값은 실패
*/
UINT32 CMemBase::CalcMemSizeOfBSA()
{
	return UINT32(sizeof(STG_RBVC));
}

/*
 desc : Luria 공유 메모리 생성 크기 계산
 parm : None
 retn : 최소한 8 이상 값이 반환되어야 함. 8 이하 값은 실패
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
 desc : PLC_Q 공유 메모리 생성 크기 계산
 parm : None
 retn : 최소한 8 이상 값이 반환되어야 함. 8 이하 값은 실패
*/
UINT32 CMemBase::CalcMemSizeOfPLCQ()
{
	/* Input & Output 등 아무리 많아야 128 Word는 넘지 않는다 */ 
#if 0
	return (sizeof(STG_PDSM) - sizeof(PUINT8)) +
			UINT32(m_pstConfig->melsec_q_svc.addr_d_count * sizeof(UINT16));
#else
	/* Input & Output 등 아무리 많아야 128 Word는 넘지 않는다 */
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
 desc : Config 공유 메모리 생성 크기 계산
 parm : None
 retn : 최소한 8 이상 값이 반환되어야 함. 8 이하 값은 실패
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
	/* 공유 메모리 포인터 연결 */
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
	/* 공유 메모리 포인터 연결 */
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
	/* 공유 메모리 포인터 연결 */
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
	
	/* 공유 메모리 포인터 연결 */
	pMemMap		= m_pMemMap->GetMemMap();
	m_pstMemMap	= (LPG_CWTI)pMemMap;

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                 Shared Memory - Luria Object                                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		config	- [in]  전체 환경 정보
 retn : None
*/
CMemLuria::CMemLuria(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_luria)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_LURIA;

}

/*
 desc : 생성 혹은 열렸던 메모리 영역에 각 필요한 구조체 멤버 변수를 주소에 연결
		즉, 값이 저장될(된) 영역에 필요한 변수 연결 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemLuria::LinkMemMap()
{
	/* 공유 메모리 포인터 연결 */
	m_pstMemMap	= (LPG_LDSM)m_pMemMap->GetMemMap();
	if (!m_pstMemMap)	return FALSE;

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                  Shared Memory - MC2 Object                                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		config	- [in]  전체 환경 정보
 retn : None
*/
CMemMC2::CMemMC2(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_mc2)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_MC2;
}

/*
 desc : 생성 혹은 열렸던 메모리 영역에 각 필요한 구조체 멤버 변수를 주소에 연결
		즉, 값이 저장될(된) 영역에 필요한 변수 연결 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemMC2::LinkMemMap()
{
	/* 공유 메모리 포인터 연결 */
	m_pstMemMap	= (LPG_MDSM)m_pMemMap->GetMemMap();
	if (!m_pstMemMap)	return FALSE;

	/* 정상적으로 연결되어 있는지 확인? 검증 루틴 ? */

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                  Shared Memory - PLC Object                                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		config	- [in]  환경 설정 정보
 retn : None
*/
CMemPLC::CMemPLC(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_plc_q)
{
	m_pShMemKey	= SH_MEM_PLC;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CMemPLC::~CMemPLC()
{
	::Free(m_pstMemMap);
}

/*
 desc : 생성 혹은 열렸던 메모리 영역에 각 필요한 구조체 멤버 변수를 주소에 연결
		즉, 값이 저장될(된) 영역에 필요한 변수 연결 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemPLC::LinkMemMap()
{
	PUINT8 pMemNext		= NULL;

	/* 공유 메모리 포인터 연결 */
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
 desc : 특정 시작 주소의 메모리 데이터 값 [ 1 Bit 값 ] 반환
 parm : addr	- [in]  비트 정보가 저장된 주소 (BIT Index 값. 실제 메모리 상의 주소 값)
						Zero based 기준. 0 번지부터 시작 (100번지 요청인 경우, 101번째 메모리 값)
		bits	- [in]  Bit Position (0x00 ~ 0x0f)
		value	- [out] 반환되어 저장될 참조 값
 retn : TRUE or FALSE
*/
BOOL CMemPLC::GetBitValue(UINT32 addr, UINT8 bits, UINT8 &value)
{
	UINT16 u16Value	= 0;
	UINT32 u32Index	= 0;	/* 물리적인 메모리의 주소 인덱스 (Zero-based ~ ) */
	PUINT16 pMemMap	= (PUINT16)m_pstMemMap->data;	/* 2 Bytes (WORD)로 포인터 연결 */

	/* bits 위치가 0x0f보다 큰 값인지 그리고 주소 값이 유효한지 확인 */
	if (bits > 0x0f || m_pstConfig->melsec_q_svc.start_d_addr > addr)	return FALSE;
	/* 입력된 주소 값이 큰 경우라면 */
	if ((addr - m_pstConfig->melsec_q_svc.start_d_addr) >= m_pstConfig->melsec_q_svc.addr_d_count)	return FALSE;

	/* 공유 메모리의 WORD 기반의 인덱스 값 얻기 */
	u32Index	= addr - m_pstConfig->melsec_q_svc.start_d_addr;
	/* 먼저 WORD 단위의 주소 값 얻기 */
	u16Value	= pMemMap[u32Index];

	/* Word 값 중에서 해당 위치의 Bit 값 추출 */
	value		= (0x0001 & (u16Value >> bits));

	return TRUE;
}

/*
 desc : 특정 시작 주소의 메모리 데이터 값 [ 1 Bytes 값 ] 반환
 parm : addr	- [in]  비트 정보가 저장된 주소 (BIT Index 값. 실제 메모리 상의 주소 값)
						Zero based 기준. 0 번지부터 시작 (100번지 요청인 경우, 101번째 메모리 값)
		flag	- [in]  0x00: Low, 0x01: High
		value	- [out] 반환되어 저장될 참조 값
 retn : TRUE or FALSE
*/
BOOL CMemPLC::GetByteValue(UINT32 addr, UINT8 flag, UINT8 &value)
{
	UINT16 u16Value	= 0;
	UINT32 u32Index	= 0;	/* 물리적인 메모리의 주소 인덱스 (Zero-based ~ ) */
	PUINT16 pMemMap	= (PUINT16)m_pstMemMap->data;	/* 2 Bytes (WORD)로 포인터 연결 */

	/* bits 위치가 0x0f보다 큰 값인지 그리고 주소 값이 유효한지 확인 */
	if (m_pstConfig->melsec_q_svc.start_d_addr > addr)	return FALSE;
	/* 입력된 주소 값이 큰 경우라면 */
	if ((addr - m_pstConfig->melsec_q_svc.start_d_addr) >= m_pstConfig->melsec_q_svc.addr_d_count)	return FALSE;

	/* 공유 메모리의 WORD 기반의 인덱스 값 얻기 */
	u32Index	= addr - m_pstConfig->melsec_q_svc.start_d_addr;
	/* 먼저 WORD 단위의 주소 값 얻기 */
	u16Value	= pMemMap[u32Index];

	/* Word 값 중에서 해당 위치의 Bit 값 추출 */
	if (flag)	value	= (0xff00 & u16Value) >> 8;	/* High 1 byte */
	else		value	= (0x00ff & u16Value);		/* Low 1 byte */

	return TRUE;
}

/*
 desc : 특정 시작 주소의 메모리 데이터 값 [ 2 Bytes 값 ] 반환
 parm : addr	- [in]  비트 정보가 저장된 주소 (BIT Index 값. 실제 메모리 상의 주소 값)
						Zero based 기준. 0 번지부터 시작 (100번지 요청인 경우, 101번째 메모리 값)
		flag	- [in]  0x00: Low, 0x01: High
		value	- [out] 반환되어 저장될 참조 값
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
	PUINT16 pMemMap	= (PUINT16)m_pstMemMap->data;	/* 2 Bytes (WORD)로 포인터 연결 */

	/* bits 위치가 0x0f보다 큰 값인지 그리고 주소 값이 유효한지 확인 */
	if (m_pstConfig->melsec_q_svc.start_d_addr > addr)
	{
		LOG_ERROR(ENG_EDIC::en_melsec, L"The address value is too small.");
		return FALSE;
	}
	/* 입력된 주소 값이 큰 경우라면 */
	if ((addr - m_pstConfig->melsec_q_svc.start_d_addr) >= m_pstConfig->melsec_q_svc.addr_d_count)
	{
		LOG_ERROR(ENG_EDIC::en_melsec, L"The address value is too large.");
		return FALSE;
	}

	/* 먼저 WORD 단위의 주소 값 얻기 */
	value	= pMemMap[addr - m_pstConfig->melsec_q_svc.start_d_addr];

	return TRUE;
}

/*
 desc : 임의 메모리 주소의 메모리 데이터 값 [ 2 Bytes 값 ] 반환
 parm : index	- [in]  메모리의 위치 값 (0 or Later)
		value	- [out] 반환되어 저장될 참조 값
 retn : TRUE or FALSE
*/
BOOL CMemPLC::GetWordValue(UINT16 index, UINT16 &value)
{
	PUINT16 pMemMap	= (PUINT16)m_pstMemMap->data;	/* 2 Bytes (WORD)로 포인터 연결 */

	/* 메모리 인덱스 위치가 메모리 크기 (2 bytes 단위)보다 크거나 같으면 에러 */
	if (m_pstConfig->melsec_q_svc.addr_d_count <= index)	return FALSE;

	/* WORD 단위의 값 얻기 */
	value	= pMemMap[index];

	return TRUE;
}

/*
 desc : 특정 시작 주소의 메모리 데이터 값 [ 4 Bytes 값 ] 반환
 parm : addr	- [in]  비트 정보가 저장된 주소 (BIT Index 값. 실제 메모리 상의 주소 값)
						Zero based 기준. 0 번지부터 시작 (100번지 요청인 경우, 101번째 메모리 값)
		flag	- [in]  0x00: Low, 0x01: High
		value	- [out] 반환되어 저장될 참조 값
 retn : TRUE or FALSE
*/
BOOL CMemPLC::GetDWordValue(UINT32 addr, UINT32 &value)
{
	PUINT16 pMemMap	= (PUINT16)m_pstMemMap->data;	/* 2 Bytes (WORD)로 포인터 연결 */

	/* bits 위치가 0x0f보다 큰 값인지 그리고 주소 값이 유효한지 확인 */
	if (m_pstConfig->melsec_q_svc.start_d_addr <= addr)	return FALSE;
	/* 입력된 주소 값이 큰 경우라면 */
	if ((addr + 1 /* for DWord */ - m_pstConfig->melsec_q_svc.start_d_addr) >=
		m_pstConfig->melsec_q_svc.addr_d_count)	return FALSE;

	/* 먼저 WORD 단위의 주소 값 얻기 */
	memcpy(&value, &pMemMap[addr - m_pstConfig->melsec_q_svc.start_d_addr], 4);

	return TRUE;
}

/*
 desc : 임의 메모리 주소의 메모리 데이터 값 [ 4 Bytes 값 ] 반환
 parm : index	- [in]  메모리의 위치 값 (0 or Later)
		value	- [out] 반환되어 저장될 참조 값
 retn : TRUE or FALSE
*/
BOOL CMemPLC::GetDWordValue(UINT16 index, UINT32 &value)
{
	PUINT16 pMemMap	= (PUINT16)m_pstMemMap->data;	/* 2 Bytes (WORD)로 포인터 연결 */

	/* 메모리 인덱스 위치가 메모리 크기 (2 bytes 단위)보다 크거나 같으면 에러 */
	if (m_pstConfig->melsec_q_svc.addr_d_count <= index)	return FALSE;

	/* WORD 단위의 값 얻기 */
	memcpy(&value, pMemMap+index, 4);

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                    Shared Memory - PreAligner Data for Logosol with serial                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		config	- [in]  전체 환경 정보
 retn : None
*/
CMemMPA::CMemMPA(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_lspa)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_LSPA;
}

/*
 desc : 생성 혹은 열렸던 메모리 영역에 각 필요한 구조체 멤버 변수를 주소에 연결
		즉, 값이 저장될(된) 영역에 필요한 변수 연결 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemMPA::LinkMemMap()
{
	PUINT8 pMemMap	= NULL;
	
	/* 공유 메모리 포인터 연결 */
	pMemMap		= m_pMemMap->GetMemMap();
	m_pstMemMap	= (LPG_MPDV)pMemMap;

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                  Shared Memory - Diamond Robot Data for Logosol with serial                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		config	- [in]  전체 환경 정보
 retn : None
*/
CMemMDR::CMemMDR(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_lsdr)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_LSDR;
}

/*
 desc : 생성 혹은 열렸던 메모리 영역에 각 필요한 구조체 멤버 변수를 주소에 연결
		즉, 값이 저장될(된) 영역에 필요한 변수 연결 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemMDR::LinkMemMap()
{
	PUINT8 pMemMap	= NULL;
	
	/* 공유 메모리 포인터 연결 */
	pMemMap		= m_pMemMap->GetMemMap();
	m_pstMemMap	= (LPG_MRDV)pMemMap;

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                               Shared Memory - EFU for Shinsung                                */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		config	- [in]  전체 환경 정보
 retn : None
*/
CMemEFU::CMemEFU(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_efu_ss)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_EFU_SS;
}

/*
 desc : 생성 혹은 열렸던 메모리 영역에 각 필요한 구조체 멤버 변수를 주소에 연결
		즉, 값이 저장될(된) 영역에 필요한 변수 연결 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemEFU::LinkMemMap()
{
	PUINT8 pMemMap	= NULL;
	
	/* 공유 메모리 포인터 연결 */
	pMemMap		= m_pMemMap->GetMemMap();
	m_pstMemMap	= (LPG_EPAS)pMemMap;

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                       Shared Memory - BSA                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		config	- [in]  전체 환경 정보
 retn : None
*/
CMemBSA::CMemBSA(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_bsa)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_BSA;
}

/*
 desc : 생성 혹은 열렸던 메모리 영역에 각 필요한 구조체 멤버 변수를 주소에 연결
		즉, 값이 저장될(된) 영역에 필요한 변수 연결 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemBSA::LinkMemMap()
{
	PUINT8 pMemMap	= NULL;
	
	/* 공유 메모리 포인터 연결 */
	pMemMap		= m_pMemMap->GetMemMap();
	m_pstMemMap	= (LPG_RBVC)pMemMap;

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                             Shared Memory - LED Power Measurement                             */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		config	- [in]  전체 환경 정보
 retn : None
*/
CMemPM100D::CMemPM100D(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_pm100d)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_PM100D;
}

/*
 desc : 생성 혹은 열렸던 메모리 영역에 각 필요한 구조체 멤버 변수를 주소에 연결
		즉, 값이 저장될(된) 영역에 필요한 변수 연결 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemPM100D::LinkMemMap()
{
	PUINT8 pMemMap	= NULL;
	
	/* 공유 메모리 포인터 연결 */
	pMemMap		= m_pMemMap->GetMemMap();
	m_pstMemMap	= (LPG_PADV)pMemMap;

	return TRUE;
}
