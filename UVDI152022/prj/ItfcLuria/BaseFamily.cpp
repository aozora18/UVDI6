
#include "pch.h"
#include "MainApp.h"
#include "BaseFamily.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : shmem	- [in]  Shared Memory (Luria Service)
		error	- [in]  각종 에러 정보를 관리하는 객체
 retn : None
*/
CBaseFamily::CBaseFamily(LPG_LDSM shmem, CCodeToStr *error)
{
	/* 가장 최근에 생성 및 반환된 패킷 크기 값 저장 (임시) */
	m_u32PktSize	= 0;
	/* 기본 Family ID 설정 */
	m_enFamily		= ENG_LUDF::en_none;
	/* 공유 메모리 연결 */
	m_pstShMemLuria	= shmem;
	/* 에러 관리 객체 연결 */
	m_pCodeToStr	= error;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CBaseFamily::~CBaseFamily()
{
}

/*
 desc : Validatae the family id
 parm : f_id	- [in]  Family ID (ENG_LUDF)
 retn : TRUE or FALSE
*/
BOOL CBaseFamily::ValidFamilyID(ENG_LUDF f_id)
{
	switch (f_id)
	{
	case ENG_LUDF::en_machine_config		:
	case ENG_LUDF::en_job_management		:
	case ENG_LUDF::en_panel_preparation		:
	case ENG_LUDF::en_exposure				:
	case ENG_LUDF::en_direct_photo_comm		:
	case ENG_LUDF::en_luria_com_management	:
	case ENG_LUDF::en_announcement			:
	case ENG_LUDF::en_system				:
	case ENG_LUDF::en_focus					:
	case ENG_LUDF::en_calibration			:
	case ENG_LUDF::en_vision				:
	case ENG_LUDF::en_toolbox				:	return TRUE;
	}

	return FALSE;
}

/*
 desc : Set the packet buffer to be sent
 parm : type	- [in]  명령어 종류
		cmd		- [in]  Command Types (Read or Read Reply or Write or Write Reply)
		p_data	- [out] Buffer pointer where the packet data is stored
		p_size	- [in]  The size of 'p_data' buffer
		u_data	- [in]  user data
		u_size	- [in] The size of 'u_data' buffer
 retn : Returns a buffer pointer to stored in
*/
PUINT8 CBaseFamily::GetPktBase(ENG_LTCT type, ENG_LCMC cmd, PUINT8 p_data, UINT32 p_size, PUINT8 u_data, UINT32 u_size)
{	return GetPktBase(type, UINT8(cmd), p_data, p_size, u_data, u_size);	}
PUINT8 CBaseFamily::GetPktBase(ENG_LTCT type, ENG_LCSS cmd, PUINT8 p_data, UINT32 p_size, PUINT8 u_data, UINT32 u_size)
{	return GetPktBase(type, UINT8(cmd), p_data, p_size, u_data, u_size);	}
PUINT8 CBaseFamily::GetPktBase(ENG_LTCT type, ENG_LCJM cmd, PUINT8 p_data, UINT32 p_size, PUINT8 u_data, UINT32 u_size)
{	return GetPktBase(type, UINT8(cmd), p_data, p_size, u_data, u_size);	}
PUINT8 CBaseFamily::GetPktBase(ENG_LTCT type, ENG_LCPP cmd, PUINT8 p_data, UINT32 p_size, PUINT8 u_data, UINT32 u_size)
{	return GetPktBase(type, UINT8(cmd), p_data, p_size, u_data, u_size);	}
PUINT8 CBaseFamily::GetPktBase(ENG_LTCT type, ENG_LCEP cmd, PUINT8 p_data, UINT32 p_size, PUINT8 u_data, UINT32 u_size)
{	return GetPktBase(type, UINT8(cmd), p_data, p_size, u_data, u_size);	}
PUINT8 CBaseFamily::GetPktBase(ENG_LTCT type, ENG_LCDP cmd, PUINT8 p_data, UINT32 p_size, PUINT8 u_data, UINT32 u_size)
{	return GetPktBase(type, UINT8(cmd), p_data, p_size, u_data, u_size);	}
PUINT8 CBaseFamily::GetPktBase(ENG_LTCT type, ENG_LCCM cmd, PUINT8 p_data, UINT32 p_size, PUINT8 u_data, UINT32 u_size)
{	return GetPktBase(type, UINT8(cmd), p_data, p_size, u_data, u_size);	}
PUINT8 CBaseFamily::GetPktBase(ENG_LTCT type, ENG_LCPF cmd, PUINT8 p_data, UINT32 p_size, PUINT8 u_data, UINT32 u_size)
{	return GetPktBase(type, UINT8(cmd), p_data, p_size, u_data, u_size);	}
PUINT8 CBaseFamily::GetPktBase(ENG_LTCT type, UINT8 cmd,
							   PUINT8 p_data, UINT32 p_size,
							   PUINT8 u_data, UINT32 u_size)
{
	UINT32 u32PktSize	= 0;
	STG_PCLS stPktData	= {NULL};

	/* Calculate the size of the final packet to be sent */
	u32PktSize	= sizeof(STG_PCLS) - sizeof(PUINT8) + u_size;
	if (u32PktSize >= p_size)
	{
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128, L"The buffer to store the packet is too small "
								L"(buff_size (%u) < pkt_size (%u)", p_size, u32PktSize);
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
		return NULL;
	}
	/* Initialize the packet buffer */
	memset(p_data, 0x00, p_size);

	/* Family ID 설정 여부 */
	if (!ValidFamilyID(m_enFamily))
	{
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128, L"Unknown Family ID (0x%02x)", UINT8(m_enFamily));
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
		return NULL;
	}

	/* 기본 패킷 구성 */
	stPktData.protocol_version	= LURIA_PROTOCOL_VERSION;
	stPktData.command_type		= UINT8(type);
	stPktData.user_data_family	= UINT8(m_enFamily);
	stPktData.user_data_id		= cmd;
	stPktData.num_bytes			= UINT16(u32PktSize);
	/* Network Bytes Ordering (Big-Endian) */
	stPktData.SwapHostToNetwork();
	/* User Data */
	memcpy(p_data, &stPktData, u32PktSize-u_size);
	if (u_size)	memcpy(p_data+(u32PktSize-u_size), u_data, u_size);

	/* Calculates the entire packet generated so far */
	m_u32PktSize	+= u32PktSize;

	/* Returns the pointer to the next buffer to be stored */
	return p_data+u32PktSize;
}


/*
 desc : If it is a lower version after comparing the versions, return an error
 parm : major	- [in]  Major number
 		minor	- [in]  Minor number
 		build	- [in]  Build number
		mode	- [in]  0x00 : high (More than : '>='), 0x01 : low (Less than : '<'), 0x02 : same (equal : '==')
		logs	- [in]  Set whether to save the the log file
 retn: TRUE (Same or higher version), FALSE (Lower version)
 note: Use compatible commands for Luria Protocol
*/
BOOL CBaseFamily::ValidVersion(UINT16 major, UINT16 minor, UINT16 build, UINT8 mode, BOOL logs)
{
	LPG_CLSI pstLuria	= &GetConfig()->luria_svc;

	/* If the current protocol version is higher */
	if (0x00 == mode)
	{
		if (pstLuria->luria_ver_major > major)	return TRUE;
		if (pstLuria->luria_ver_major == major)
		{
			if (pstLuria->luria_ver_minor > minor)	return TRUE;
			if (pstLuria->luria_ver_minor == minor &&
				pstLuria->luria_ver_build >= build)	return TRUE;
		}
	}
	/* If the current protocol version is lower */
	else if (0x01 == mode)
	{
		if (pstLuria->luria_ver_major < major)	return TRUE;
		if (pstLuria->luria_ver_major == major)
		{
			if (pstLuria->luria_ver_minor < minor)	return TRUE;
			if (pstLuria->luria_ver_minor == minor &&
				pstLuria->luria_ver_build < build)	return TRUE;	/* not '<=' */
		}
	}
	/* If the current protocol version is equal */
	else if (0x02 == mode)
	{
		if (pstLuria->luria_ver_major == major &&
			pstLuria->luria_ver_minor == minor &&
			pstLuria->luria_ver_build == build)	return TRUE;
	}
	
	if (logs)
	{
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128, L"Protocol is no longer available (set:%u.%u.%u, req:%u.%u.%u)",
				   pstLuria->luria_ver_major, pstLuria->luria_ver_minor, pstLuria->luria_ver_build,
				   major, minor, build);
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
	}
	return FALSE;
}
