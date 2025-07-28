
/*
 desc : User Data Family : Photohead Z Focus
*/

#include "pch.h"
#include "MainApp.h"
#include "PhZFocus.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : Constructor
 parm : shmem	- [in]  Shared Memory (Luria Service)
		error	- [in]  Buffer where error messages are stored
 retn : None
*/
CPhZFocus::CPhZFocus(LPG_LDSM shmem, CCodeToStr *error)
	: CBaseFamily(shmem, error)
{
	m_enFamily	= ENG_LUDF::en_focus;
	m_pstShMemPF= &shmem->focus;
	m_pstShMemDP= &shmem->directph;
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CPhZFocus::~CPhZFocus()
{
}

/* --------------------------------------------------------------------------------------------- */
/*                                    Packet Function (Make)                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : This will initialize the z-axis
		The z-axis must be initialized in order to use any of the below focus functions
 parm : None
 retn : Returns a buffer pointer to stored in
 note : This may take up to 30 seconds to complete
		If initialize fails, the status code FocusMotorOperationFailed is given
		Valid from Luria version 2.1.0 and newer
*/
PUINT8 CPhZFocus::GetPktInitializeFocus()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 1, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPF::en_initialize_focus, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Set or get current focus position of a single head
		To set position for multiple heads simultaneously, see SetPositionMulti.
 parm : flag	- [in]  Function Type : Get or Set
		ph_no	- [in]  Photo head number of first axis to set position to (1 = ph #1, 2 = ph # 2, etc)
		ph_pos	- [in]  Z-position given in micrometers for given photo head.
 retn : Returns a buffer pointer to stored in
 note : Valid from Luria version 2.1.0 and newer
*/
PUINT8 CPhZFocus::GetPktPosition(ENG_LPGS flag, UINT8 ph_no, INT32 ph_pos)
{
	UINT8 u8Body[8] = { ph_no, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 1, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		ph_pos	= SWAP32(ph_pos);
		memcpy(u8Body, &ph_pos, 4);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPF::en_position,
							  pPktNext, u32Pkts, u8Body, 5);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPF::en_position,
							  pPktNext, u32Pkts-m_u32PktSize, u8Body, 1);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Start / Stop (Enable / Disable) photo head auto focus system
 parm : flag	- [in]  Function Type : Get or Set
		ph_no	- [in]  Photo head number (0 = All ph, 1 = ph #1, 2 = ph # 2, etc)
		enable	- [in]  0x01 : Enable auto focus, 0x00 = Disable Auto focus.
 retn : Returns a buffer pointer to stored in
 note : Valid from Luria version 2.1.0 and newer
*/
PUINT8 CPhZFocus::GetPktAutofocus(ENG_LPGS flag, UINT8 ph_no, UINT8 enable)
{
	UINT8 u8Body[8] = { ph_no, enable, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 1, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPF::en_auto_focus,
							  pPktNext, u32Pkts, u8Body, 2);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPF::en_auto_focus,
							  pPktNext, u32Pkts-m_u32PktSize, u8Body, 1);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Autofocus set-point trim value
 parm : flag	- [in]  Function Type : Get or Set
		ph_no	- [in]  Photo head number (1 = ph #1, 2 = ph # 2, etc)
		trim	- [in]  Focus trim in nanometers
 retn : Returns a buffer pointer to stored in
 note : Valid from Luria version 2.1.0 and newer
*/
PUINT8 CPhZFocus::GetPktTrim(ENG_LPGS flag, UINT8 ph_no, INT32 trim)
{
	UINT8 u8Body[8] = { ph_no, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 1, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		trim	= SWAP32(trim);
		memcpy(u8Body+1, &trim, 4);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPF::en_trim,
							  pPktNext, u32Pkts, u8Body, 5);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPF::en_trim,
							  pPktNext, u32Pkts-m_u32PktSize, u8Body, 1);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Autofocus will only operate inside the limits given here
		The AAQ enable/disable will be activated in the photo head(s) when the command is received by Luria
		The settings will also be sent to the photo head(s) when running Exposure:PrePrint
		and will hence over-write any AAQ-settings done using direct photo head commands
 parm : flag	- [in]  Function Type : Get or Set
		enable	- [in]  0x01: Enable, 0x00: Disable
		start	- [in]  Distance from exposure <start> where AF is inactive (um)
		end		- [in]  Distance from exposure <end> where AF is inactive (um)
 retn : Returns a buffer pointer to stored in
 note : Valid from Luria version 2.2.0 and newer
*/
PUINT8 CPhZFocus::GetPktActiveAreaQualifier(ENG_LPGS flag, UINT8 enable, UINT32 start, UINT32 end)
{
	UINT8 u8Body[16]= { NULL };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 2, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		u8Body[0]	= enable;
		start		= SWAP32(start);
		end			= SWAP32(end);
		memcpy(u8Body+1, &start, 4);
		memcpy(u8Body+5, &end, 4);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPF::en_active_area_qualifier,
							  pPktNext, u32Pkts, u8Body, 9);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPF::en_active_area_qualifier,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Autofocus set-point trim value
 parm : flag	- [in]  Function Type : Get or Set
		ph_no	- [in]  Photo head number (0 = All ph, 1 = ph #1, 2 = ph # 2, etc)
		abs_min	- [in]  The min absolute focus position that the AF will operate within (um)
		abs_max	- [in]  The min absolute focus position that the AF will operate within (um)
 retn : Returns a buffer pointer to stored in
 note : Setting AbsWorkRange will clear the AbsWorkRangeStatus for the given photo head(s).
		Valid from Luria version 2.2.0 and newer
*/
PUINT8 CPhZFocus::GetPktAbsWorkRange(ENG_LPGS flag, UINT8 ph_no, INT32 abs_min, INT32 abs_max)
{
	UINT8 u8Body[9] = { ph_no, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 2, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		abs_min	= SWAP32(abs_min);
		abs_max	= SWAP32(abs_max);
		memcpy(u8Body+1, &abs_min, 4);
		memcpy(u8Body+5, &abs_max, 4);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPF::en_abs_work_range,
							  pPktNext, u32Pkts, u8Body, 9);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPF::en_abs_work_range,
							  pPktNext, u32Pkts-m_u32PktSize, u8Body, 1);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : The status-bit for a given photo head will be cleared next time a new AbsWorkRange has been set
		!!! AbsWorkRange with photo head number = 0 (all photo heads), then all status bits will be cleared !!!
 parm : None
 retn : Returns a buffer pointer to stored in
 note : Valid from Luria version 2.2.0 and newer
*/
PUINT8 CPhZFocus::GetPktGetAbsWorkRangeStatus()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 2, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPF::en_abs_work_range_status, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : If the next focus point is further below or above the current focus point than the given value,
		then the focus will not be changed
		This can be used to avoid the focus following cut-outs, etc
 parm : flag	- [in]  Function Type : Get or Set
		dist	- [in]  The max distance below or above current AF set-point that AF will be active (um)
		enable	- [in]  Enable or disable the edge trigger function. 1 = enable, 0 = disable
 retn : Returns a buffer pointer to stored in
 note : Setting AbsWorkRange will clear the AbsWorkRangeStatus for the given photo head(s).
		Valid from Luria version 2.3.0 and newer
*/
PUINT8 CPhZFocus::GetPktAfEdgeTrigger(ENG_LPGS flag, UINT16 dist, UINT8 enable)
{
	UINT8 u8Body[8] = { NULL };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 3, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		dist		= SWAP16(dist);
		u8Body[2]	= enable;
		memcpy(u8Body, &dist, 2);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPF::en_af_edge_trigger,
							  pPktNext, u32Pkts, u8Body, 3);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPF::en_af_edge_trigger,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Set focus position for several photo heads simultaneously
		Example: Send position to head 2, 4 and 5. In this case N = 3,
				 first head = 2, second head = 4 and third head = 5.
 parm : count	- [in]  Number (N) of photo heads to send position to
		ph_no	- [in]  Photo head number of first axis to set position to (1 = ph #1, 2 = ph # 2, etc)
		ph_pos	- [in]  Z-position given in micrometers for given photo head.
 retn : Returns a buffer pointer to stored in
 note : Valid from Luria version 2.2.0 and newer
*/
PUINT8 CPhZFocus::GetPktSetPositionMulti(UINT8 count, PUINT8 ph_no, PINT32 ph_pos)
{
	UINT8 u8Body[96] = { NULL }, *pBody = u8Body, i = 0x00;
	UINT32 u32Pkts	= 256, u32Body;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 2, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Set the send packet */
	*pBody		= count;	pBody	+= 1;
	for (; i<count; i++)
	{
		*pBody		= ph_no[i];				pBody	+= 1;
		ph_pos[i]	= SWAP32(ph_pos[i]);	pBody	+= 4;
	}
	u32Body	= UINT32(pBody - u8Body);
	pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPF::en_set_position_multi,
						  pPktNext, u32Pkts, u8Body, u32Body);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Set or get current focus position of a single head
 parm : flag	- [in]  Function Type : Get or Set
		ph_no	- [in]  Photo head number of first axis to set position to (1 = ph #1, 2 = ph # 2, etc)
		ph_pos	- [in]  Z-position given in nanometers (nm)
 retn : Returns a buffer pointer to stored in
 note : This command is duplicating Focus:Position command, but with higher resolution
		Valid from Luria version 2.5.0 and newer
*/
PUINT8 CPhZFocus::GetPktPositionHiRes(ENG_LPGS flag, UINT8 ph_no, INT32 ph_pos)
{
	UINT8 u8Body[8] = { ph_no, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 5, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		ph_pos	= SWAP32(ph_pos);
		memcpy(u8Body+1, &ph_pos, 4);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPF::en_position_hi_res,
							  pPktNext, u32Pkts, u8Body, 5);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPF::en_position_hi_res,
							  pPktNext, u32Pkts-m_u32PktSize, u8Body, 1);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Reports the number of trigger pulses that autofocus position has been outside the given DOF (MachineConfig:DepthOfFocus)
 parm : None
 retn : Returns a buffer pointer to stored in
 note : The counter is reset when this command is finished,
		so the number reported is the number of trigger pulses outside DOF since last read.
		This is valid for linear z-motor only.
		If trying to read this number when connected to a stepper z-motor, then 0 will be returned.
		Valid from Luria version 2.6.0 and newer
*/
PUINT8 CPhZFocus::GetPktOutsideDOFStatus()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 6, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPF::en_outside_dof_status, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Get focus position for all photo heads simultaneously (단위: um)
 parm : None
 retn : Returns a buffer pointer to stored in
 note : Valid from Luria version 2.12.0 and newer
*/
PUINT8 CPhZFocus::GetPktGetPositionMulti()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 12, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPF::en_get_position_multi, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Autofocus Gain
 parm : flag	- [in]  Function Type : Get or Set
		gain	- [in]  Gain/damping factor. (Valid range 1 - 4095)
 retn : Returns a buffer pointer to stored in
 note : This will set the autofocus gain (damping factor) in the photo heads
		The default gain value is 10.
*/
PUINT8 CPhZFocus::GetPktAfGain(ENG_LPGS flag, UINT16 gain)
{
	UINT8 u8Body[8] = { NULL };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 12, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		gain	= SWAP16(gain);
		memcpy(u8Body, &gain, 2);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPF::en_af_gain,
							  pPktNext, u32Pkts, u8Body, 2);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPF::en_af_gain,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : This will start a scan for ethercat devices when using linear z-axis from Sieb&Meyer
		The z-axis must be initialized before using this function
 parm : None
 retn : Returns a buffer pointer to stored in
 note : Note that the z-axis controller will automatically re-start after the command has been received
		This may take more than 30 seconds
		A new z-axis initialization must be done after the scan for ethercat devices is complete
		and the controller has re-started.
		Valid from Luria version 3.2.0 and newer
*/
PUINT8 CPhZFocus::GetPktScanEthercat()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(3, 2, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPF::en_scan_ethercat, pPktNext, u32Pkts);

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
VOID CPhZFocus::SetRecvPacket(UINT8 uid, PUINT8 data, UINT32 size)
{
	switch (uid)
	{
	case (UINT8)ENG_LCPF::en_auto_focus				:
	case (UINT8)ENG_LCPF::en_af_gain				:	
		SetRecv2Bytes(uid, data);	
	break;

	case (UINT8)ENG_LCPF::en_abs_work_range_status	:	SetRecv4Bytes(uid, data);	break;

	case (UINT8)ENG_LCPF::en_trim					:
	case (UINT8)ENG_LCPF::en_position				:
	case (UINT8)ENG_LCPF::en_position_hi_res		:	SetRecv5Bytes(uid, data);	break;

	case (UINT8)ENG_LCPF::en_active_area_qualifier	:
	case (UINT8)ENG_LCPF::en_abs_work_range			:	SetRecv9Bytes(uid, data);	break;

	case (UINT8)ENG_LCPF::en_af_edge_trigger		:
	case (UINT8)ENG_LCPF::en_outside_dof_status		:
	case (UINT8)ENG_LCPF::en_get_position_multi		:	SetRecvEtcBytes(uid, data);	break;
	}
}

/*
 desc : 2 Bytes 수신 데이터 처리
 parm : uid		- [in]  서브 명령어
		data	- [in]  수신된 데이터
 retn : None
*/
VOID CPhZFocus::SetRecv2Bytes(UINT8 uid, PUINT8 data)
{
	UINT16 u16Data	= 0;

	switch (uid)
	{
	case (UINT8)ENG_LCPF::en_auto_focus	:
		if (data[0] == 0x00)	
			memset(m_pstShMemPF->auto_focus, data[1], MAX_PH);
		else					
			m_pstShMemPF->auto_focus[data[0]-1]	= data[1];
		break;

	case (UINT8)ENG_LCPF::en_af_gain	:
		memcpy(&u16Data, data, 2);
		m_pstShMemPF->af_gain	= SWAP16(u16Data);
		break;
	}
}

/*
 desc : 4 Bytes 수신 데이터 처리
 parm : uid		- [in]  서브 명령어
		data	- [in]  수신된 데이터
 retn : None
*/
VOID CPhZFocus::SetRecv4Bytes(UINT8 uid, PUINT8 data)
{
	UINT32 u32Data	= 0;

	switch (uid)
	{
	case (UINT8)ENG_LCPF::en_abs_work_range_status	:
		memcpy(&u32Data, data, 4);
		m_pstShMemPF->abs_work_range_status	= SWAP32(u32Data);
		break;
	}
}

/*
 desc : 5 Bytes 수신 데이터 처리
 parm : uid		- [in]  서브 명령어
		data	- [in]  수신된 데이터
 retn : None
*/
VOID CPhZFocus::SetRecv5Bytes(UINT8 uid, PUINT8 data)
{
	INT32 i32Data	= 0;
	UINT32 u32Data	= 0;

	switch (uid)
	{
	case (UINT8)ENG_LCPF::en_trim				:
		memcpy(&i32Data, data+1, 4);
		m_pstShMemPF->trim[data[0]-1]		= SWAP32(i32Data);	/* nm */
		break;

	case (UINT8)ENG_LCPF::en_position			:
		memcpy(&i32Data, data+1, 4);
		m_pstShMemPF->z_pos[data[0]-1]	= SWAP32(i32Data) * 1000;	/* um -> nm */
		m_pstShMemDP->focus_motor_move_abs_position[data[0]-1] = SWAP32(i32Data);
		break;

	case (UINT8)ENG_LCPF::en_position_hi_res	:
		memcpy(&i32Data, data+1, 4);
		m_pstShMemPF->z_pos[data[0]-1]	= SWAP32(i32Data);	/* nm */
		break;
	}
}

/*
 desc : 9 Bytes 수신 데이터 처리
 parm : uid		- [in]  서브 명령어
		data	- [in]  수신된 데이터
 retn : None
*/
VOID CPhZFocus::SetRecv9Bytes(UINT8 uid, PUINT8 data)
{
	INT32 i32Data	= 0;
	UINT32 u32Data	= 0;

	switch (uid)
	{
	case (UINT8)ENG_LCPF::en_active_area_qualifier	:
		m_pstShMemPF->aaq_actived	= data[0];
		memcpy(&u32Data, &data[1], 4);
		m_pstShMemPF->aaq_start_end[0]	= SWAP32(u32Data);
		memcpy(&u32Data, &data[5], 4);
		m_pstShMemPF->aaq_start_end[1]	= SWAP32(u32Data);
		break;

	case (UINT8)ENG_LCPF::en_abs_work_range			:
		if (data[0] == 0)
		{
			memcpy(&i32Data, &data[1], 4);
			i32Data	= SWAP32(i32Data);
			memset(m_pstShMemPF->abs_work_range_min, i32Data, sizeof(INT32) * MAX_PH);
			memcpy(&i32Data, &data[5], 4);
			i32Data	= SWAP32(i32Data);
			memset(m_pstShMemPF->abs_work_range_max, i32Data, sizeof(INT32) * MAX_PH);
		}
		else
		{
			memcpy(&i32Data, &data[1], 4);
			m_pstShMemPF->abs_work_range_min[data[0]-1]	= SWAP32(i32Data);
			memcpy(&i32Data, &data[5], 4);
			m_pstShMemPF->abs_work_range_max[data[0]-1]	= SWAP32(i32Data);
		}
		break;
	}
}

/*
 desc : Etc Bytes 수신 데이터 처리
 parm : uid		- [in]  서브 명령어
		data	- [in]  수신된 데이터
 retn : None
*/
VOID CPhZFocus::SetRecvEtcBytes(UINT8 uid, PUINT8 data)
{
	UINT16 u16Data	= 0;
	UINT32 u32Data	= 0, i = 0, u32PhCnt;
	INT32 i32Data	= 0;

	switch (uid)
	{
	case (UINT8)ENG_LCPF::en_af_edge_trigger	:
		memcpy(&u32Data, data, 2);
		m_pstShMemPF->af_edge_trig_dist	= SWAP16(u16Data);
		m_pstShMemPF->af_edge_trig		= data[2];
		break;

	case (UINT8)ENG_LCPF::en_outside_dof_status		:
		for (; i<MAX_PH; i++)
		{
			memcpy(&u32Data, data+(i*4), 4);
			m_pstShMemPF->out_dof_trig_pulse[i]	= SWAP32(u32Data);
		}
		break;
	case (UINT8)ENG_LCPF::en_get_position_multi		:
		u32PhCnt	= data[0];
		for (; i<u32PhCnt; i++)
		{
			memcpy(&i32Data, data+(i*4)+1, 4);
			m_pstShMemPF->z_pos[i]	= SWAP32(i32Data);
		}
		break;
	}
}