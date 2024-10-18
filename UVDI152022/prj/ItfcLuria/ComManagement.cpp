
/*
 desc : User Data Family : Com Management
*/

#include "pch.h"
#include "MainApp.h"
#include "ComManagement.h"


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
CComManagement::CComManagement(LPG_LDSM shmem, CCodeToStr *error)
	: CBaseFamily(shmem, error)
{
	m_u8Status	= 0x00;
	/* 기본 Family ID 설정 */
	m_enFamily	= ENG_LUDF::en_luria_com_management;
	/* Luria Communication Management Structure */
	m_pstCommMgt= &shmem->commmgt;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CComManagement::~CComManagement()
{
}

/* --------------------------------------------------------------------------------------------- */
/*                                    Packet Function (Make)                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : The IP address of the UI (announce server) that the Luria (announce client) will send announcements to
		The default IP address at Luria power-on is 0.0.0.0. Luria will send announcements only
		if the IP address is different from 0.0.0.0
		Hence, if UI do not want the Luria to generate announcements,
		the IP address should be set to 0.0.0.0
 parm : flag	- [in]  Function Type : Get or Set
		ipv4	- [in]  Server IP address (ex>192.168.0.100, 0:192, 1:168, 2:0, 3:100)
 retn : Returns a buffer pointer to stored in
*/
PUINT8 CComManagement::GetPktAnnounceServerIpAddr(ENG_LPGS flag, PUINT8 ipv4)
{
	UINT8 u8Body[8] = { NULL };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff = new UINT8[u32Pkts]; // new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		memcpy(u8Body, ipv4, 4);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCCM::en_announce_server_ipaddr,
							  pPktNext, u32Pkts, u8Body, 4);
	}
#if 0	/* It exists in the protocol documentation, but doesn't actually work */
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCCM::en_announce_server_ipaddr,
							  pPktNext, u32Pkts-m_u32PktSize);
	}
#endif
	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Force a “TestAnnouncement” to be sent from Luria. This will be sent if AnnounceServerIpAddr is
		different from 0.0.0.0
		If the “TestAnnouncement” was unsuccessfully sent for the third time in a row
		(i.e. the reply to “GenerateTestAnnouncement” was not OK three times),
		all announcements will be disabled, irrespective of AnnounceServerIpAddr not being 0.0.0.0
		The announcements will be enabled again as soon as a “TestAnnouncement” was successfully sent
		(i.e. if “GenerateTestAnnouncement” is ok)
 parm : flag	- [in]  Function Type : Get or Set
		ipv4	- [in]  Server IP address (ex>192.168.0.100, 0:192, 1:168, 2:0, 3:100)
 retn : Returns a buffer pointer to stored in
*/
PUINT8 CComManagement::GetPktGenerateTestAnnouncement()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCCM::en_test_announcement, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : The announcements will be automatically disabled by Luria if no announcements could be sent
		three times in a row. The only way it will be enabled again is by successfully sending a
		“GenerateTestAnnouncement”.
 parm : flag	- [in]  Function Type : Get or Set
		ipv4	- [in]  Server IP address (ex>192.168.0.100, 0:192, 1:168, 2:0, 3:100)
 retn : Returns a buffer pointer to stored in
 note : The status will be “disabled” if AnnounceServerIpAddr is 0.0.0.0
*/
PUINT8 CComManagement::GetPktAnnouncementStatus()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCCM::en_announcement_status, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/* --------------------------------------------------------------------------------------------- */
/*                                    Packet Function (Save)                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 수신받은 데이터 처리 - Machine Config
 parm : uid		- [in]  서브 명령어
		data	- [in]  데이터가 저장된 버퍼 포인터
		size	- [in]  'data' 의 크기
 retn : None
*/
VOID CComManagement::SetRecvPacket(UINT8 uid, PUINT8 data, UINT32 size)
{
	switch (uid)
	{
	case ENG_LCCM::en_announce_server_ipaddr	:
		if (4 == size)	memcpy(m_pstCommMgt->server_ip, data, size);	break;
	case ENG_LCCM::en_announcement_status	:
		m_pstCommMgt->announce_status= data[0];							break;
	}
}