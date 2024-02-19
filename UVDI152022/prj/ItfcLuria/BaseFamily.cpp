
#include "pch.h"
#include "MainApp.h"
#include "BaseFamily.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : shmem	- [in]  Shared Memory (Luria Service)
		error	- [in]  ���� ���� ������ �����ϴ� ��ü
 retn : None
*/
CBaseFamily::CBaseFamily(LPG_LDSM shmem, CCodeToStr *error)
{
	/* ���� �ֱٿ� ���� �� ��ȯ�� ��Ŷ ũ�� �� ���� (�ӽ�) */
	m_u32PktSize	= 0;
	/* �⺻ Family ID ���� */
	m_enFamily		= ENG_LUDF::en_none;
	/* ���� �޸� ���� */
	m_pstShMemLuria	= shmem;
	/* ���� ���� ��ü ���� */
	m_pCodeToStr	= error;
}

/*
 desc : �ı���
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
 parm : type	- [in]  ��ɾ� ����
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

	/* Family ID ���� ���� */
	if (!ValidFamilyID(m_enFamily))
	{
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128, L"Unknown Family ID (0x%02x)", UINT8(m_enFamily));
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
		return NULL;
	}

	/* �⺻ ��Ŷ ���� */
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
#if 0
/*
 desc : <Request : Get or Set> - 1 ~ 8 byte sized packet
 parm : cmd		- [in]  Command
		flag	- [in]  0x00 : Set, 0x01 : Get
		data	- [in]  1 ~ 4 bytes data
		size	- [in]  'data' ũ�� (!!! Important !!!) (one of 1, 2, 4)
 retn : NULL or Packet Data
*/
PUINT8 CBaseFamily::GetPktCommon(ENG_LCMC cmd, ENG_LPGS flag, UINT64 data, UINT8 size)
{
	return GetPktCommon(UINT8(cmd), flag, data, size);
}
PUINT8 CBaseFamily::GetPktCommon(UINT8 cmd, ENG_LPGS flag, UINT64 data, UINT8 size)
{
	UINT8 u8Data		= UINT8(data);
	UINT16 u16Data		= (UINT16)htons(UINT16(data));
	UINT32 u32Data		= (UINT32)htonl(UINT32(data));
	UINT32 u32PktSize	= 0, u32Size[2] = { size, 0 }, i = 1 /* !!! important !!! */, i32Cnt = 2 /* !!! important !!! */;
	PUINT8 pPktBase		= NULL, pPktTotal = NULL, pPktNext, pPktData = NULL;
	ENG_LTCT enType[2]	= { ENG_LTCT::en_write, ENG_LTCT::en_read };

	switch (size)
	{
	case 1 : pPktData = (PUINT8)&u8Data;	break;
	case 2 : pPktData = (PUINT8)&u16Data;	break;
	case 4 : pPktData = (PUINT8)&u32Data;	break;
	}
	/* �۽� �������� ũ�Ⱑ ��ȿ���� Ȯ�� */
	if (!pPktData && size > 0)	return NULL;

	/* ��ü �۽��� ��Ŷ ũ�� ���Ƿ� �Ҵ� */
	pPktTotal = (PUINT8)::Alloc(sizeof(UINT8) * 128);
	memset(pPktTotal, 0x00, 128);
	pPktNext	= pPktTotal;

	/* ������ Only write ��ɸ� �����ϴ°�. ��, ������ ���� ��ɾ�� Read ��û�� �ʿ� ���� */
	if (ENG_LPGS::en_set == flag)
	{
		i = 0 /* !!! important !!! */;
		i32Cnt = IsWriteOnlyCmd(UINT8(m_enFamily), cmd) ? 1 : 2;	/* Important */
	}

	/* Get or Set */
	for (; i<i32Cnt; i++)
	{
		pPktBase	= GetPktBase(enType[i], cmd, pPktData, u32Size[i]);
		memcpy(pPktNext, pPktBase, m_u32PktLastSize);
		pPktNext	+= m_u32PktLastSize;
		::Free(pPktBase);
		/* �� ��Ŷ ũ�� ���� */
		u32PktSize	+= m_u32PktLastSize;
	}
	/* ��ü ��Ŷ ũ�� ���� */
	m_u32PktLastSize	= u32PktSize;

	/* ��ü ��Ŷ ��ȯ */
	return pPktTotal;
}
#endif

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
