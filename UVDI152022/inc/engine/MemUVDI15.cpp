
/*
 desc : 장비 (혹은 장치) 기본 객체
*/

#include "pch.h"
#include "MemUVDI15.h"

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
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: Monitoring S/W에서 호출)
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
	if (m_pMemMap)	delete m_pMemMap;
	m_pMemMap	= NULL;
}

/*
 desc : 공유 메모리 생성 및 연결
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemBase::CreateMap()
{
	BOOL bSucc			= FALSE;
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
		case ENG_MMDC::en_conf			:	m_u32MemSize = CalcMemSizeOfConfig();		break;
		case ENG_MMDC::en_luria			:	m_u32MemSize = CalcMemSizeOfLuria();		break;
		case ENG_MMDC::en_mc2			:	m_u32MemSize = CalcMemSizeOfMC2();			break;
		case ENG_MMDC::en_plc_q			:	m_u32MemSize = CalcMemSizeOfPLCQ();			break;
		case ENG_MMDC::en_trig			:	m_u32MemSize = CalcMemSizeOfTrig();			break;
		case ENG_MMDC::en_visi			:	m_u32MemSize = CalcMemSizeOfVisi();			break;
		case ENG_MMDC::en_ph_step		:	m_u32MemSize = CalcMemSizeOfPhStep();		break;
		case ENG_MMDC::en_ph_correct_y	:	m_u32MemSize = CalcMemSizeOfPhCorrectY();	break;
		case ENG_MMDC::en_pm100d		:	m_u32MemSize = CalcMemSizeOfPM100D();		break;
		case ENG_MMDC::en_philhmi		:	m_u32MemSize = CalcMemSizeOfPhilhmi();		break;
		case ENG_MMDC::en_strobe_lamp	:	m_u32MemSize = CalcMemSizeOfStrobeLamp();	break;

		case ENG_MMDC::en_gentec		:	m_u32MemSize = CalcMemSizeOfPM100D();		break;
		case ENG_MMDC::en_mvenc			:	m_u32MemSize = CalcMemSizeOfTrig();			break;
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
				  (sizeof(STG_RVCD) * MAX_MC2_DRIVE) + (sizeof(STG_AVCD) * MAX_MC2_DRIVE));
#else
	return (UINT32)sizeof(STG_MDSM);
#endif
}

/*
 desc : Luria 공유 메모리 생성 크기 계산
 parm : None
 retn : 최소한 8 이상 값이 반환되어야 함. 8 이하 값은 실패
*/
UINT32 CMemBase::CalcMemSizeOfLuria()
{
#if 0
	UINT32 u32MachineConfig, u32JobManagement, u32PanelPrepare;
	UINT32 u32Exposure, u32PhDirectCommand, u32CommMgt, u32System;

	/* MachineConfig Size */
	u32MachineConfig	= sizeof(STG_LDMC);
	u32JobManagement	= sizeof(STG_LDJM);
	u32PanelPrepare		= sizeof(STG_LDPP);
	u32Exposure			= sizeof(STG_LDEW);
	u32PhDirectCommand	= sizeof(STG_LDDP);
	u32CommMgt			= sizeof(STG_LDCM);
	u32System			= sizeof(STG_LDSS);

	return UINT32(u32MachineConfig + u32JobManagement + u32PanelPrepare + 
				  u32Exposure + u32PhDirectCommand + u32CommMgt + u32System);
#else
	return (UINT32)sizeof(STG_LDSM);
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
	return (sizeof(STG_DLSM) + sizeof(STG_PVSM)) +
			UINT32(m_pstConfig->melsec_q_svc.addr_d_count * sizeof(UINT16));
#endif
}

/*
 desc : Trigger Board 공유 메모리 생성 크기 계산
 parm : None
 retn : 최소한 8 이상 값이 반환되어야 함. 8 이하 값은 실패
*/
UINT32 CMemBase::CalcMemSizeOfTrig()
{
	return UINT32(sizeof(STG_TPQR) + sizeof(STG_DLSM));
}

/*
 desc : Vision 공유 메모리 생성 크기 계산
 parm : None
 retn : 최소한 8 이상 값이 반환되어야 함. 8 이하 값은 실패
*/
UINT32 CMemBase::CalcMemSizeOfVisi()
{
	UINT32 u32Size		= 0, u32Temp;
	UINT32 u32ACamCount	= m_pstConfig->set_cams.acam_count;
	UINT32 u32ACamSize	= m_pstConfig->set_cams.GetCamSizeBytes();

	u32Size	+= sizeof(STG_CMPV) * m_pstConfig->mark_find.max_mark_regist * u32ACamCount;	/* Align Camera 별 Grabbed Image의 검색을 위한 등록된 모델 정보 */
	u32Size	+= sizeof(STG_DBXY) * m_pstConfig->mark_find.max_mark_regist * u32ACamCount;	/* 등록된 모델 크기 정보 */
	
	/* Find Result */
	u32Size	+= sizeof(STG_GMFR) * m_pstConfig->mark_find.max_mark_find * u32ACamCount;		/* find : mod_result */
	u32Size	+= sizeof(STG_EDFR) * m_pstConfig->edge_find.max_find_count * u32ACamCount;		/* find : edge_result */
	u32Size	+= sizeof(STG_ELFR) * m_pstConfig->line_find.max_find_count * u32ACamCount;		/* find : line_result */
	
	/* Align Camera의 X/Y 축 보정 오차 정보 값 */
	u32Temp	= MAX_GLOBAL_MARKS / u32ACamCount;
	u32Temp	+= (MAX_GLOBAL_MARKS % u32ACamCount) > 0 ? 0x01 : 0x00;
	u32Size	+= sizeof(STG_ACCE) * u32Temp * u32ACamCount;
	u32Temp	= MAX_LOCAL_MARKS / u32ACamCount;
	u32Temp	+= (MAX_LOCAL_MARKS % u32ACamCount) > 0 ? 0x01 : 0x00;
	u32Size	+= sizeof(STG_ACCE) * u32Temp * u32ACamCount;

	/* Link Status */
	u32Size	+= sizeof(STG_DLSM) * u32ACamCount;

	/* Drawing Image */
	u32Size	+= sizeof(UINT8) * m_pstConfig->mark_find.max_mark_grab * u32ACamSize * u32ACamCount;		/* find : mod_draw */
	u32Size	+= sizeof(UINT8) * u32ACamSize * u32ACamCount;												/* find : edge_draw */
	u32Size	+= sizeof(UINT8) * m_pstConfig->line_find.max_find_count * u32ACamSize * u32ACamCount;		/* find : line_draw */

	return u32Size;
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

/*
 desc : Recipe 공유 메모리 생성 크기 계산 (Photohead Offset;Step)
 parm : None
 retn : 최소한 8 이상 값이 반환되어야 함. 8 이하 값은 실패
*/
UINT32 CMemBase::CalcMemSizeOfPhStep()
{
	return UINT32(sizeof(STG_OSSD) * UINT32(MAX_PH_STEP_LINES/*m_pstConfig->ph_step.max_ph_step*/));
}

/*
 desc : Recipe 공유 메모리 생성 크기 계산 (Photohead Offset;Correction Table Y)
 parm : None
 retn : 최소한 8 이상 값이 반환되어야 함. 8 이하 값은 실패
*/
UINT32 CMemBase::CalcMemSizeOfPhCorrectY()
{
	return UINT32(sizeof(STG_OSCY) * UINT32(MAX_PH_CORRECT_Y));
}

/*
 desc : PM100D 공유 메모리 생성 크기 계산
 parm : None
 retn : 최소한 8 이상 값이 반환되어야 함. 8 이하 값은 실패
*/
UINT32 CMemBase::CalcMemSizeOfPM100D()
{
	return UINT32(sizeof(STG_PADV));
}

/*
 desc : Philhmi 공유 메모리 생성 크기 계산
 parm : None
 retn : 최소한 8 이상 값이 반환되어야 함. 8 이하 값은 실패
*/
UINT32 CMemBase::CalcMemSizeOfPhilhmi()
{
	return UINT32(sizeof(STG_PPR));
}

/*
 desc : Strobe Lamp 공유 메모리 생성 크기 계산
 parm : None
 retn : 최소한 8 이상 값이 반환되어야 함. 8 이하 값은 실패
*/
UINT32 CMemBase::CalcMemSizeOfStrobeLamp()
{
	return UINT32(sizeof(STG_SLPR_RECV));
}

/* --------------------------------------------------------------------------------------------- */
/*                                Shared Memory - Configuration                                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
 retn : None
*/
CMemConf::CMemConf(BOOL engine)
	: CMemBase(engine, NULL, ENG_MMDC::en_conf)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_CONFIG;

}

/*
 desc : 생성 혹은 열렸던 메모리 영역에 각 필요한 구조체 멤버 변수를 주소에 연결
		즉, 값이 저장될(된) 영역에 필요한 변수 연결 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemConf::LinkMemMap()
{
	/* 공유 메모리 포인터 연결 */
	m_pstMemMap	= (LPG_CIEA)m_pMemMap->GetMemMap();
	if (!m_pstMemMap)	return FALSE;

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
	m_pstMemMap->link.ResetData();
	/* Luria Status 초기화 */
	m_pstMemMap->system.ResetStatus();

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
	m_pShMemKey	= SH_MEM_MC2B;
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
	m_pstMemMap->link.ResetData();

	/* 정상적으로 연결되어 있는지 확인? 검증 루틴 ? */

	return TRUE;
}
/* --------------------------------------------------------------------------------------------- */
/*                                  Shared Memory - MC2b Object                                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		config	- [in]  전체 환경 정보
 retn : None
*/
CMemMC2b::CMemMC2b(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_mc2)
{
	m_pstMemMap = NULL;
	m_pShMemKey = SH_MEM_MC2;
}

/*
 desc : 생성 혹은 열렸던 메모리 영역에 각 필요한 구조체 멤버 변수를 주소에 연결
		즉, 값이 저장될(된) 영역에 필요한 변수 연결 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemMC2b::LinkMemMap()
{
	/* 공유 메모리 포인터 연결 */
	m_pstMemMap = (LPG_MDSM)m_pMemMap->GetMemMap();
	if (!m_pstMemMap)	return FALSE;
	m_pstMemMap->link.ResetData();

	/* 정상적으로 연결되어 있는지 확인? 검증 루틴 ? */

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                Shared Memory - Trigger Object                                 */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		config	- [in]  전체 환경 정보
 retn : None
*/
CMemTrig::CMemTrig(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_trig)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_TRIG;
}

/*
 desc : 생성 혹은 열렸던 메모리 영역에 각 필요한 구조체 멤버 변수를 주소에 연결
		즉, 값이 저장될(된) 영역에 필요한 변수 연결 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemTrig::LinkMemMap()
{
	PUINT8 pMemMap	= NULL;
	
	/* 공유 메모리 포인터 연결 */
	pMemMap		= m_pMemMap->GetMemMap();
	m_pstMemMap	= (LPG_TPQR)pMemMap;
 	pMemMap		+= sizeof(STG_TPQR);
 	m_pstLink	= (LPG_DLSM)pMemMap;
	m_pstLink->ResetData();	/* 통신 연결 상태 정보 초기화 */

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                 Shared Memory - Vision Object                                 */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		config	- [in]  전체 환경 정보
 retn : None
*/
CMemVisi::CMemVisi(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_visi)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_VISI;}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CMemVisi::~CMemVisi()
{
	UINT32 i;
	if (m_pstMemMap)
	{
		::Free(m_pstMemMap->mod_define);
		::Free(m_pstMemMap->mod_size);
		::Free(m_pstMemMap->mark_result);
		::Free(m_pstMemMap->edge_result);
		::Free(m_pstMemMap->line_result);

		for (i=0; i<m_pstConfig->set_cams.acam_count; i++)
		{
			::Free(m_pstMemMap->mark_image[i]);
			::Free(m_pstMemMap->line_image[i]);
		}

		for (i=0; i<m_pstConfig->set_cams.acam_count; i++)
		{
			::Free(m_pstMemMap->cali_global[i]);
			::Free(m_pstMemMap->cali_local[i]);
		}
		::Free(m_pstMemMap->cali_global);
		::Free(m_pstMemMap->cali_local);

		::Free(m_pstMemMap->mark_image);
		::Free(m_pstMemMap->edge_image);
		::Free(m_pstMemMap->line_image);
		::Free(m_pstMemMap->link);
		::Free(m_pstMemMap);
	}
}

/*
 desc : 생성 혹은 열렸던 메모리 영역에 각 필요한 구조체 멤버 변수를 주소에 연결
		즉, 값이 저장될(된) 영역에 필요한 변수 연결 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemVisi::LinkMemMap()
{
	UINT32 i, j, u32Size= 0, u8Temp;
	UINT32 u32ACamCnt	= m_pstConfig->set_cams.acam_count;
	PUINT8 pMemNext		= NULL;
	LPG_EDFR pstEdge	= NULL;
	LPG_ELFR pstLine	= NULL;

	/* 메모리 할당 */
	m_pstMemMap	= (LPG_VDSM)::Alloc(sizeof(STG_VDSM));
	ASSERT(m_pstMemMap);
	/* 공유 메모리 포인터 연결 */
	pMemNext	= m_pMemMap->GetMemMap();
	if (!m_pstMemMap)	return FALSE;

	/* ModeDefine 즉, 모델 등록 후 설정된 값 저장 영역 연결 */
	m_pstMemMap->mod_define	= (STG_CMPV**)::Alloc(sizeof(LPG_CMPV) * u32ACamCnt);
	for (i=0; i<u32ACamCnt; i++)
	{
		m_pstMemMap->mod_define[i]	= (LPG_CMPV)pMemNext;
		pMemNext	+= sizeof(STG_CMPV) * m_pstConfig->mark_find.max_mark_regist;
	}
	/* Model Size 즉, 등록된 모델 크기 */
	m_pstMemMap->mod_size	= (STG_DBXY**)::Alloc(sizeof(LPG_DBXY) * u32ACamCnt);
	for (i=0; i<u32ACamCnt; i++)
	{
		m_pstMemMap->mod_size[i]	= (LPG_DBXY)pMemNext;
		pMemNext	+= sizeof(STG_DBXY) * m_pstConfig->mark_find.max_mark_regist;
	}

	/* ----------- */
	/* Find Result */
	/* ----------- */

	/* Model Find */
	m_pstMemMap->mark_result	= (STG_GMFR**)::Alloc(sizeof(LPG_GMFR) * u32ACamCnt);
	for (i=0; i<u32ACamCnt; i++)
	{
		m_pstMemMap->mark_result[i]	= (LPG_GMFR)pMemNext;
		pMemNext	+= sizeof(STG_GMFR) * m_pstConfig->mark_find.max_mark_find;
	}
	/* Edge Find */
	m_pstMemMap->edge_result	= (STG_EDFR**)::Alloc(sizeof(LPG_EDFR) * u32ACamCnt);
	for (i=0; i<u32ACamCnt; i++)
	{
		m_pstMemMap->edge_result[i]	= (LPG_EDFR)pMemNext;
		pMemNext	+= sizeof(STG_EDFR) * m_pstConfig->edge_find.max_find_count;
	}
	/* Line Find */
	m_pstMemMap->line_result	= (STG_ELFR**)::Alloc(sizeof(LPG_ELFR) * u32ACamCnt);
	for (i=0; i<u32ACamCnt; i++)
	{
		m_pstMemMap->line_result[i]	= (LPG_ELFR)pMemNext;
		pMemNext	+= sizeof(STG_ELFR) * m_pstConfig->line_find.max_find_count;
	}

	/* Align Camera의 Calibration XY */
	m_pstMemMap->cali_global	= (STG_ACCE ***)::Alloc(sizeof(LPG_ACCE) * u32ACamCnt);
	for (i=0; i<u32ACamCnt; i++)
	{
		u8Temp	= MAX_GLOBAL_MARKS / u32ACamCnt;
		u8Temp	+= (MAX_GLOBAL_MARKS % u32ACamCnt) > 0 ? 0x01 : 0x00;
		m_pstMemMap->cali_global[i]	= (STG_ACCE **)::Alloc(sizeof(LPG_ACCE) * u8Temp);
		for (j=0; j<u8Temp; j++)
		{
			m_pstMemMap->cali_global[i][j]	= (LPG_ACCE)pMemNext;
			pMemNext	+= sizeof(STG_ACCE);
		}
	}
	m_pstMemMap->cali_local	= (STG_ACCE ***)::Alloc(sizeof(LPG_ACCE) * u32ACamCnt);
	for (i=0; i<u32ACamCnt; i++)
	{
		u8Temp	= MAX_LOCAL_MARKS / u32ACamCnt;
		u8Temp	+= (MAX_LOCAL_MARKS % u32ACamCnt) > 0 ? 0x01 : 0x00;
		m_pstMemMap->cali_local[i]	= (STG_ACCE **)::Alloc(sizeof(LPG_ACCE) * u8Temp);
		for (j=0; j<u8Temp; j++)
		{
			m_pstMemMap->cali_local[i][j]	= (LPG_ACCE)pMemNext;
			pMemNext	+= sizeof(STG_ACCE);
		}
	}

	/* Camera Link Status */
	m_pstMemMap->link	= (STG_DLSM**)::Alloc(sizeof(LPG_DLSM) * u32ACamCnt);
	for (i=0; i<u32ACamCnt; i++)
	{
		m_pstMemMap->link[i]	= (LPG_DLSM)pMemNext;
		pMemNext	+= sizeof(STG_DLSM);
		m_pstMemMap->link[i]->ResetData();
	}

	/* ------------- */
	/* Image Drawing */
	/* ------------- */
	u32Size	= m_pstConfig->set_cams.GetCamSizeBytes();

	/* Mark Find */
	m_pstMemMap->mark_image	= (UINT8***)::Alloc(sizeof(PUINT8) * u32ACamCnt);
	for (i=0; i<u32ACamCnt; i++)
	{
		m_pstMemMap->mark_image[i]	= (UINT8**)::Alloc(sizeof(PUINT8) * m_pstConfig->mark_find.max_mark_grab);
		for (j=0; j<m_pstConfig->mark_find.max_mark_grab; j++)
		{
			m_pstMemMap->mark_image[i][j]	= (PUINT8)pMemNext;
			pMemNext	+= sizeof(UINT8) * u32Size;
		}
	}
	/* Edge Find (!!! 주의 !!! 카메라 마다 이미지 버퍼 1개씩만 존재) */
	m_pstMemMap->edge_image	= (UINT8**)::Alloc(sizeof(PUINT8) * u32ACamCnt);
	for (i=0; i<u32ACamCnt; i++)
	{
		m_pstMemMap->edge_image[i]	= (PUINT8)pMemNext;
		pMemNext	+= sizeof(UINT8) * u32Size/* * m_pstConfig->edge_find.max_find_count*/;
	}
	/* Line Find */
	m_pstMemMap->line_image	= (UINT8***)::Alloc(sizeof(PUINT8) * u32ACamCnt);
	for (i=0; i<u32ACamCnt; i++)
	{
		m_pstMemMap->line_image[i]	= (UINT8**)::Alloc(sizeof(PUINT8) * m_pstConfig->line_find.max_find_count);
		for (j=0; j<m_pstConfig->line_find.max_find_count; j++)
		{
			m_pstMemMap->line_image[i][j]	= (PUINT8)pMemNext;
			pMemNext	+= sizeof(UINT8) * u32Size;
		}
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                       Shared Memory - Recipe for Photohead Offset (step)                      */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		config	- [in]  전체 환경 정보
 retn : None
*/
CMemPhStep::CMemPhStep(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_ph_step)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_PH_STEP;
}

/*
 desc : 생성 혹은 열렸던 메모리 영역에 각 필요한 구조체 멤버 변수를 주소에 연결
		즉, 값이 저장될(된) 영역에 필요한 변수 연결 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemPhStep::LinkMemMap()
{
	PUINT8 pMemMap	= NULL;
	
	/* 공유 메모리 포인터 연결 */
	pMemMap		= m_pMemMap->GetMemMap();
	m_pstMemMap	= (LPG_OSSD)pMemMap;

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                Shared Memory - Recipe for Photohead Correction (Strip) Y Table                */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		config	- [in]  전체 환경 정보
 retn : None
*/
CMemPhCorrectY::CMemPhCorrectY(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_ph_correct_y)
{
	m_pstMemMap	= NULL;
	m_pShMemKey	= SH_MEM_PH_CORRECT_Y;
}

/*
 desc : 생성 혹은 열렸던 메모리 영역에 각 필요한 구조체 멤버 변수를 주소에 연결
		즉, 값이 저장될(된) 영역에 필요한 변수 연결 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemPhCorrectY::LinkMemMap()
{
	PUINT8 pMemMap	= NULL;
	
	/* 공유 메모리 포인터 연결 */
	pMemMap		= m_pMemMap->GetMemMap();
	m_pstMemMap	= (LPG_OSCY)pMemMap;

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
CMemGentec::CMemGentec(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_gentec)
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
BOOL CMemGentec::LinkMemMap()
{
	PUINT8 pMemMap	= NULL;
	
	/* 공유 메모리 포인터 연결 */
	pMemMap		= m_pMemMap->GetMemMap();
	m_pstMemMap	= (LPG_PADV)pMemMap;

	return TRUE;
}


/* --------------------------------------------------------------------------------------------- */
/*                                Shared Memory - Trigger Object                                 */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		config	- [in]  전체 환경 정보
 retn : None
*/
CMemMvenc::CMemMvenc(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_mvenc)
{
	m_pstMemMap = NULL;
	m_pShMemKey = SH_MEM_TRIG;
}

/*
 desc : 생성 혹은 열렸던 메모리 영역에 각 필요한 구조체 멤버 변수를 주소에 연결
		즉, 값이 저장될(된) 영역에 필요한 변수 연결 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemMvenc::LinkMemMap()
{
	PUINT8 pMemMap = NULL;

	/* 공유 메모리 포인터 연결 */
	pMemMap = m_pMemMap->GetMemMap();
	m_pstMemMap = (LPG_TPQR)pMemMap;
	pMemMap += sizeof(STG_TPQR);
	m_pstLink = (LPG_DLSM)pMemMap;
	m_pstLink->ResetData();	/* 통신 연결 상태 정보 초기화 */

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                Shared Memory - Philhmi Object                                 */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		config	- [in]  전체 환경 정보
 retn : None
*/
CMemPhilhmi::CMemPhilhmi(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_philhmi)
{
	m_pstMemMap = NULL;
	m_pShMemKey = SH_MEM_PHILHMI;
}

/*
 desc : 생성 혹은 열렸던 메모리 영역에 각 필요한 구조체 멤버 변수를 주소에 연결
		즉, 값이 저장될(된) 영역에 필요한 변수 연결 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemPhilhmi::LinkMemMap()
{
	PUINT8 pMemMap = NULL;

	/* 공유 메모리 포인터 연결 */
	pMemMap = m_pMemMap->GetMemMap();
	m_pstMemMap = (LPG_PPR)pMemMap;
	pMemMap += sizeof(STG_PPR);
	m_pstLink = (LPG_DLSM)pMemMap;
	m_pstLink->ResetData();	/* 통신 연결 상태 정보 초기화 */

	return TRUE;
}


/* --------------------------------------------------------------------------------------------- */
/*                                Shared Memory - Strobe Lamp Object                                 */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		config	- [in]  전체 환경 정보
 retn : None
*/
CMemStrobeLamp::CMemStrobeLamp(BOOL engine, LPG_CIEA config)
	: CMemBase(engine, config, ENG_MMDC::en_strobe_lamp)
{
	m_pstMemMap = NULL;
	m_pShMemKey = SH_MEM_STROBE_LAMP;
}

/*
 desc : 생성 혹은 열렸던 메모리 영역에 각 필요한 구조체 멤버 변수를 주소에 연결
		즉, 값이 저장될(된) 영역에 필요한 변수 연결 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMemStrobeLamp::LinkMemMap()
{
	PUINT8 pMemMap = NULL;

	/* 공유 메모리 포인터 연결 */
	pMemMap = m_pMemMap->GetMemMap();
	m_pstMemMap = (LPG_SLPR)pMemMap;
	pMemMap += sizeof(LPG_SLPR);
	m_pstLink = (LPG_DLSM)pMemMap;
	m_pstLink->ResetData();	/* 통신 연결 상태 정보 초기화 */

	return TRUE;
}