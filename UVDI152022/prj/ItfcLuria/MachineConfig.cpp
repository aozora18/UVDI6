
/*
 desc :User Data Family : Machine Config
*/

#include "pch.h"
#include "MainApp.h"
#include "MachineConfig.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc :생성자
 parm : shmem	- [in]  Shared Memory (Luria Service)
		error	- [in]  각종 에러 정보를 관리하는 객체
 retn : None
*/
CMachineConfig::CMachineConfig(LPG_LDSM shmem, CCodeToStr *error)
	: CBaseFamily(shmem, error)
{
	/* 기본 Family ID 설정 */
	m_enFamily	= ENG_LUDF::en_machine_config;
	/* Machine Config Structure */
	m_pstShMem	= &shmem->machine;
}

/*
 desc :파괴자
 parm : None
 retn : None
*/
CMachineConfig::~CMachineConfig()
{
}

/* --------------------------------------------------------------------------------------------- */
/*                                    Packet Function (Make)                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Total number of photoheads connected to Luria
 parm : flag	- [in]  Get (Read) or Set (Write)
		ph_count- [in]  Total number of photoheads in system (1 ~ 8)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CMachineConfig::GetPktTotalPhotoheads(ENG_LPGS flag, UINT8 ph_count)
{
	UINT8 u8Body[8]	= { ph_count, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_total_photo_heads,
							  pPktNext, u32Pkts, u8Body, 1);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_total_photo_heads,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : The IP addresses for the photoheads connected to Luria
 parm : flag	- [in]  Get (Read) or Set (Write)
		ph_no	- [in]  Photohead Number (1 ~ 8)
		ipv4	- [in]  IP address (ex> 192.168.0.1 => 0:192,1:168,2:0,3:1)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CMachineConfig::GetPktPhotoheadIpAddr(ENG_LPGS flag, UINT8 ph_no, PUINT8 ipv4)
{
	UINT8 u8Body[8]	= { ph_no, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		memcpy(u8Body+1, ipv4, 4);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_photo_head_ip_addr,
							  pPktNext, u32Pkts, u8Body, 5);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_photo_head_ip_addr,
							  pPktNext, u32Pkts, (PUINT8)&ph_no, 1);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Number of strips between two photoheads
 parm : flag	- [in]  Get (Read) or Set (Write)
		pitch	- [in]  Pitch between photoheads, given in number of exposure strips
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CMachineConfig::GetPktPhotoheadPitch(ENG_LPGS flag, UINT8 pitch)
{
	UINT8 u8Body[8]	= { pitch, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_photo_head_pitch,
							  pPktNext, u32Pkts, u8Body, 1);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_photo_head_pitch,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : The alignment of the photoheads
 parm : flag	- [in]  Get (Read) or Set (Write)
		rotate	- [in]  Orientation of photoheads. 0x00 = No rotation. 0x01 = Rotated 180 degrees.
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CMachineConfig::GetPktPhotoheadRotate(ENG_LPGS flag, UINT8 rotate)
{
	UINT8 u8Body[8]	= { rotate, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_photo_head_rotate,
							  pPktNext, u32Pkts, u8Body, 1);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_photo_head_rotate,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : The paralleogram motion angle of the motion controller will be adjusted by this factor
 parm : flag	- [in]  Get (Read) or Set (Write)
		tbl_no	- [in]  Table number (1 or 2)
		factor	- [in]  Adjustment factor, multiplied by 1000.
						(Eg. factor 0.97 is given as 970, factor 1.025 is given as 1025)
						Factor 1.0 (1000) means no adjustment, i.e. use the default angle for parallelogram motion.
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CMachineConfig::GetPktParallelogramMotionAdjust(ENG_LPGS flag, UINT8 tbl_no, UINT32 factor)
{
	UINT8 u8Body[8]	= { tbl_no, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		factor	= SWAP32(factor);
		memcpy(u8Body+1, &factor, 4);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_parallelogram_motion_adjust,
							  pPktNext, u32Pkts, u8Body, 5);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_parallelogram_motion_adjust,
							  pPktNext, u32Pkts-m_u32PktSize, u8Body, 1);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : The maximum scroll rate that a photo head can tolerate
 parm : flag	- [in]  Get (Read) or Set (Write)
		scroll	- [in]  The maximum scroll rate of the system. (Default 19000)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CMachineConfig::GetPktScrollRate(ENG_LPGS flag, UINT16 scroll)
{
	UINT8 u8Body[8]	= {NULL};
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		scroll	= SWAP16(scroll);
		memcpy(u8Body, &scroll, 2);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_scroll_rate,
							  pPktNext, u32Pkts, u8Body, 2);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_scroll_rate,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : The xy-motion controller connected to the system
 parm : flag	- [in]  Get (Read) or Set (Write)
		type	- [in]  The motion controller type used. 1 = Sieb & Meyer, 2 = Newport
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CMachineConfig::GetPktMotionControlType(ENG_LPGS flag, UINT8 type)
{
	UINT8 u8Body[8]	= {type, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_motion_control_type,
							  pPktNext, u32Pkts, u8Body, 1);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_motion_control_type,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : IP address of the motion controller
 parm : flag	- [in]  Get (Read) or Set (Write)
		ipv4	- [in]  ip address xxx.xxx.xxx.xxx
 retn : NULL or Packet Data
*/
PUINT8 CMachineConfig::GetPktMotionControlIpAddr(ENG_LPGS flag, PUINT8 ipv4)
{
	UINT8 u8Body[8]	= {NULL};
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		memcpy(u8Body, ipv4, 4);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_motion_control_ipaddr,
							  pPktNext, u32Pkts, u8Body, 4);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_motion_control_ipaddr,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : X-correction table for the x-motion.
		The first entry must have 0 adjustment.
		The correction given for the last table entry will be used for x-positions
		at higher x-positions than the x-position for the last table entry.
 parm : flag	- [in]  Get (Read) or Set (Write)
		tbl_no	- [in]  Table number: 1 or 2
		e_count	- [in]  Number of table entries (N). 0 < N <= 200
		e_x_pos	- [in]  X-position on table (in nanometers)
		e_x_adj	- [in]  Adjustment (in nanometers) in the given x-position
 retn : NULL or Packet Data
*/
PUINT8 CMachineConfig::GetPktXcorrectionTable(ENG_LPGS flag, UINT8 tbl_no,
											  UINT16 e_count, PINT32 e_x_pos, PINT32 e_x_adj)
{
	UINT32 u32Pkts	= 5120, u32Body = 0;
	PUINT8 pPktBuff	= NULL, pPktNext, pBody = NULL;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
		
	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		pBody	= (PUINT8)::Alloc(sizeof(UINT8) * 4096);
		PUINT8 pNext	= pBody;
		UINT16 u16Count	= SWAP16(e_count), i;
		INT32 i32Value	= 0;
		memset(pBody, 0x00, 4096);
		*pNext	= tbl_no;					pNext++;
		memcpy(pNext, &u16Count, 2);		pNext	+= 2;
		for (i=0; i<e_count; i++)
		{
			i32Value	= SWAP32(e_x_pos[i]);
			memcpy(pNext, &i32Value, 4);	pNext	+= 4;
			i32Value	= SWAP32(e_x_adj[i]);
			memcpy(pNext, &i32Value, 4);	pNext	+= 4;
		}
		u32Body	= UINT32(pNext - pBody);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_x_correction_table,
							  pPktNext, u32Pkts, pBody, u32Body);
		::Free(pBody);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		/* Set the send packet */
		pBody	= (PUINT8)::Alloc(sizeof(UINT8) * 8);
		memset(pBody, 0x00, 8);
		pBody[0]= tbl_no;
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_x_correction_table,
							  pPktNext, u32Pkts-m_u32PktSize, pBody, 1);
		::Free(pBody);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : The minimum and maximum coordinate of the motion controller.
		This tells the Luria maximum allowable motion position
 parm : flag	- [in]  Get (Read) or Set (Write)
		tbl_no	- [in]  Table number: 1 or 2
		max_x	- [in]  Maximum absolute x-position of table. In micrometers
		max_y	- [in]  Maximum absolute y-position of table. In micrometers
		min_x	- [in]  Minimum absolute x-position of table. In micrometers
		min_y	- [in]  Minimum absolute y-position of table. In micrometers
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CMachineConfig::GetPktTablePositionLimits(ENG_LPGS flag, UINT8 tbl_no,
												 INT32 max_x, INT32 max_y,
												 INT32 min_x, INT32 min_y)
{
	UINT8 u8Body[32]= { tbl_no, }, *pBody = u8Body+1;
	UINT32 u32Pkts	= 256;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		min_x	= SWAP32(min_x);
		min_y	= SWAP32(min_y);
		max_x	= SWAP32(max_x);
		max_y	= SWAP32(max_y);
		memcpy(pBody, &min_x, 4);	pBody	+= 4;
		memcpy(pBody, &min_y, 4);	pBody	+= 4;
		memcpy(pBody, &max_x, 4);	pBody	+= 4;
		memcpy(pBody, &max_y, 4);	pBody	+= 4;
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_table_position_limits,
							  pPktNext, u32Pkts, u8Body, 17);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_table_position_limits,
							  pPktNext, u32Pkts-m_u32PktSize, u8Body, 1);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : The start position for the artwork for the given table.
		This is the position where the actual print will start.
		For the motion start position, see GetTableMotionStartPosition
 parm : flag	- [in]  Get (Read) or Set (Write)
		tbl_no	- [in]  Table number: 1 or 2
		start_x	- [in]  X-position table exposure start position. In micrometers
		start_y	- [in]  Y-position table exposure start position. In micrometers
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CMachineConfig::GetPktTableExposureStartPos(ENG_LPGS flag, UINT8 tbl_no,
												   INT32 start_x, INT32 start_y)
{
	UINT8 u8Body[16]= { tbl_no, }, *pBody = u8Body+1;
	UINT32 u32Pkts	= 256;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		start_x	= SWAP32(start_x);
		start_y	= SWAP32(start_y);
		memcpy(pBody, &start_x, 4);	pBody	+= 4;
		memcpy(pBody, &start_y, 4);	pBody	+= 4;
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_table_exposure_start_pos,
							  pPktNext, u32Pkts, u8Body, 9);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_table_exposure_start_pos,
							  pPktNext, u32Pkts-m_u32PktSize, u8Body, 1);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : This value limits the maximum speed that the motion controller will run in y-direction
 parm : flag	- [in]  Get (Read) or Set (Write)
		speed	- [in]  The max speed of Y motion. Given in micrometers/sec
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CMachineConfig::GetPktMaxYMotionSpeed(ENG_LPGS flag, UINT32 speed)
{
	UINT8 u8Body[8]	= { NULL };
	UINT32 u32Pkts	= 256;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		speed	= SWAP32(speed);
		memcpy(u8Body, &speed, 4);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_max_y_motion_speed,
							  pPktNext, u32Pkts, u8Body, 4);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_max_y_motion_speed,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : This is the speed to be used for x-movements.
		This speed will also be used to move to start position during pre-print
 parm : flag	- [in]  Get (Read) or Set (Write)
		speed	- [in]  The motion speed to be used for x-movements (and to move to print start position)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CMachineConfig::GetPktXMotionSpeed(ENG_LPGS flag, UINT32 speed)
{
	UINT8 u8Body[8]	= { NULL };
	UINT32 u32Pkts	= 256;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		speed	= SWAP32(speed);
		memcpy(u8Body, &speed, 4);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_x_motion_speed,
							  pPktNext, u32Pkts, u8Body, 4);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_x_motion_speed,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Y-axis acceleration/deceleration distance.
		Note! Valid if MotionControlType = 1 only. Ignored for other types
 parm : flag	- [in]  Get (Read) or Set (Write)
		dist	- [in]  Motion controller Y-direction acceleration/deceleration distance. Given in micrometers
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CMachineConfig::GetPktYaccelerationDistance(ENG_LPGS flag, UINT32 dist)
{
	UINT8 u8Body[8]	= { NULL };
	UINT32 u32Pkts	= 256;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		dist	= SWAP32(dist);
		memcpy(u8Body, &dist, 4);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_y_acceleration_distance,
							  pPktNext, u32Pkts, u8Body, 4);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_y_acceleration_distance,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Y-axis acceleration/deceleration distance.
		Note! Valid if MotionControlType = 1 only. Ignored for other types
 parm : flag	- [in]  Get (Read) or Set (Write)
		scroll	- [in]  Scroll-mode (1 – 4).
						The hysteresis values below will be used with the corresponding scroll-mode when printing.
		offset	- [in]  Negative offset. Given in number of trigger pulses
		d_pos	- [in]  Delay in positive moving direction. In nanoseconds
		d_neg	- [in]  Delay in negative moving direction. In nanoseconds
 retn : Pointer where the packet buffer is stored
 note : Motion controller hysteresis. Note! Valid if MotionControlType = 1 only.
		Ignored for other types
*/
PUINT8 CMachineConfig::GetPktHysteresisType1(ENG_LPGS flag, UINT8 scroll, UINT16 offset,
											 UINT32 d_pos, UINT32 d_neg)
{
	UINT8 u8Body[32]= { scroll, }, *pBody = u8Body+1;
	UINT32 u32Pkts	= 256, u32Body;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		offset	= SWAP16(offset);
		d_pos	= SWAP32(d_pos);
		d_neg	= SWAP32(d_neg);
		memcpy(pBody, &offset, 2);	pBody	+= 2;
		memcpy(pBody, &d_pos, 4);	pBody	+= 4;
		memcpy(pBody, &d_neg, 4);	pBody	+= 4;
		u32Body	= UINT32(pBody - u8Body);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_hysteresis_type1,
							  pPktNext, u32Pkts, u8Body, u32Body);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_hysteresis_type1,
							  pPktNext, u32Pkts-m_u32PktSize, u8Body, 1);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Measured offset in x- and y directions compared to photo head number 1
 parm : flag	- [in]  Get (Read) or Set (Write)
		ph_no	- [in]  Photohead number (2 - 8)
		x_pos	- [in]  X-position relative to Photohead 1. Given in nanometers
		y_pos	- [in]  Y-position relative to Photohead 1. Given in nanometers
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CMachineConfig::GetPktPhotoheadOffset(ENG_LPGS flag, UINT8 ph_no,
											 UINT32 x_pos, INT32 y_pos)
{
	UINT8 u8Body[16]= { ph_no, }, *pBody = u8Body+1;
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Validate the Photohead Number */
	if (ph_no < 2 || ph_no > MAX_PH)	return NULL;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		x_pos	= SWAP32(x_pos);
		y_pos	= SWAP32(y_pos);
		memcpy(pBody, &x_pos, 4);	pBody	+= 4;
		memcpy(pBody, &y_pos, 4);	pBody	+= 4;
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_photo_head_offset,
							  pPktNext, u32Pkts, u8Body, 9);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_photo_head_offset,
							  pPktNext, u32Pkts-m_u32PktSize, u8Body, 1);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}
PUINT8 CMachineConfig::GetPktPhotoHeadOffsetAll()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Read */
	for (UINT8 i=2; i<=GetConfig()->luria_svc.ph_count; i++)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_photo_head_offset,
							  pPktNext, u32Pkts-m_u32PktSize, (PUINT8)&i, 1);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Start-direction of y axis when printing
 parm : flag	- [in]  Get (Read) or Set (Write)
		tbl_no	- [in]  Table number: 1 or 2
		direct	- [in]  0 = Y axis starts in negative direction, 1 = Y axis starts in positive direction
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CMachineConfig::GetPktTablePrintDirect(ENG_LPGS flag, UINT8 tbl_no, UINT8 direct)
{
	UINT8 u8Body[8]	= { tbl_no, direct };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_table_print_direction,
							  pPktNext, u32Pkts, u8Body, 2);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_table_print_direction,
							  pPktNext, u32Pkts-m_u32PktSize, u8Body, 1);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Select which table that is the active table
 parm : flag	- [in]  Get (Read) or Set (Write)
		tbl_no	- [in]  Table number: 1 or 2
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CMachineConfig::GetPktActiveTable(ENG_LPGS flag, UINT8 tbl_no)
{
	UINT8 u8Body[8]	= { tbl_no, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_active_table,
							  pPktNext, u32Pkts, u8Body, 1);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_active_table,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : This will give the exact position where the motor will be placed when starting a print
		This position will be based on the current TableExposureStartPos and YaccelerationDistance
 parm : tbl_no	- [in]  Table number: 1 or 2
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CMachineConfig::GetPktGetTableMotionStartPosition(UINT8 tbl_no)
{
	UINT8 u8Body[8]	= { tbl_no, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Read */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_get_table_motion_start_position,
						  pPktNext, u32Pkts, u8Body, 1);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Enables use of the Luria software without a motion controller
		Enables use of the Luria software without photo head(s)
		Enables use of the Luria software without trigger input signals to the photo head(s)
 parm : flag	- [in]  Get (Read) or Set (Write)
		mc2		- [in]  0 = System requires motor controller, 1 = Emulate the motor controller (no need for motor controller hardware)
		ph		- [in]  0 =System requires photo head(s) to be connected, 1 = Emulate the photo head(s) (no need for photo heads to be connected)
		trig	- [in]  = System requires trigger signal inputs, 1 = Emulate the trigger inputs signals (no need for trigger input signals to photo head(s))
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CMachineConfig::GetPktEmulate(ENG_LPGS flag, UINT8 mc2, UINT8 ph, UINT8 trig)
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_emulate_motor_controller,
							  pPktNext, u32Pkts, &mc2, 1);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_emulate_photo_heads,
							  pPktNext, u32Pkts, &ph, 1);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_emulate_triggers,
							  pPktNext, u32Pkts, &trig, 1);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_emulate_motor_controller,
							  pPktNext, u32Pkts-m_u32PktSize);
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_emulate_photo_heads,
							  pPktNext, u32Pkts-m_u32PktSize);
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_emulate_triggers,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Sets the level of debug output in the log file
 parm : flag	- [in]  Get (Read) or Set (Write)
		level	- [in]  0 = Print info only, 1 = Print info and debug, 2 = Print info, debug and trace messages
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CMachineConfig::GetPktDebugPrintLevel(ENG_LPGS flag, UINT8 level)
{
	UINT8 u8Body[8]	= { level, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_debug_print,
							  pPktNext, u32Pkts, u8Body, 1);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_debug_print,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : The files generated during simulated print will be written to this directory
 parm : flag	- [in]  Get (Read) or Set (Write)
		out_dir	- [in]  Output path name text string (without null-termination) of simulation result files
						The path must be absolute. A trailing backslash (\) is optional
 retn : Pointer where the packet buffer is stored
 note : The default directory is c:\lls_simulation
*/
PUINT8 CMachineConfig::GetPktPrintSimulationOutDir(ENG_LPGS flag, PCHAR out_dir, UINT32 size)
{
	UINT32 u32Pkts	= MAX_PATH_LEN*2;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_print_simulation_out_dir,
							  pPktNext, u32Pkts, PUINT8(out_dir), size);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_print_simulation_out_dir,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Y-correction table, one for each table
		Each printed strip can be adjusted individually, in positive or negative direction Max y-adjustment is +/-500 um (500000 nm)
 parm : flag	- [in]  Get (Read) or Set (Write)
		tbl_no	- [in]  Table number: 1 or 2
		count	- [in]  Number of strip (N). 0 < N <= 200
		strip_no- [in]  Strip number (entry 1 ~ ). (Note: First strip number is 1)
		y_adj	- [in]  Y-adjustment for strip number given in entry N. Given in nanometers
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CMachineConfig::GetPktYcorrectionTable(ENG_LPGS flag, UINT8 tbl_no,
											  UINT16 count, PUINT8 strip_no, PINT32 y_adj)
{
	UINT32 u32Pkts	= 5120, u32Body = 0;
	PUINT8 pPktBuff	= NULL, pPktNext, pBody = NULL;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		pBody	= (PUINT8)::Alloc(sizeof(UINT8) * 4096);
		PUINT8 pNext	= pBody;
		UINT16 u16Count	= SWAP16(count), i;
		INT32 i32Value	= 0;
		memset(pBody, 0x00, 4096);
		*pNext	= tbl_no;					pNext	+= 1;
		memcpy(pNext, &u16Count, 2);		pNext	+= 2;
		for (i=0; i<count; i++)
		{
			*pNext	= strip_no[i];			pNext	+= 1;
			i32Value= SWAP32(y_adj[i]);
			memcpy(pNext, &i32Value, 4);	pNext	+= 4;
		}
		u32Body	= UINT32(pNext - pBody);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_y_correction_table,
							  pPktNext, u32Pkts, pBody, u32Body);
		::Free(pBody);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		/* Set the send packet */
		pBody	= (PUINT8)::Alloc(sizeof(UINT8) * 8);
		memset(pBody, 0x00, 8);
		pBody[0]= tbl_no;
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_y_correction_table,
							  pPktNext, u32Pkts-m_u32PktSize, pBody, 1);
		::Free(pBody);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : The ID to be set for the z-axis in the motion controller for this specific photo head
 parm : flag	- [in]  Get (Read) or Set (Write)
		ph_no	- [in]  Photo head number
		drv_no	- [in]  ID number of the z-motion controller for the given photo head
 retn : Pointer where the packet buffer is stored
 note : Valid if ZdriveType = 2 only
*/
PUINT8 CMachineConfig::GetPktLinearZdriveSetting(ENG_LPGS flag, UINT8 ph_no, UINT8 drv_no)
{
	UINT8 u8Body[8] = { ph_no, drv_no, 1 };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 1, 0, 0x00))	return NULL;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_linear_z_drive_settings,
							  pPktNext, u32Pkts, u8Body, 3);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_linear_z_drive_settings,
							  pPktNext, u32Pkts-m_u32PktSize, u8Body, 1);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : IP address of the Z drive (Focus drive)
 parm : flag	- [in]  Get (Read) or Set (Write)
		addr	- [in]  IP address (ex> 192.168.0.50 -> 0:192,1:168, 2:0, 3:50)
 retn : Pointer where the packet buffer is stored
 note : Valid if ZdriveType = 2 only
		Valid from Luria version 2.1.0 and newer
*/
PUINT8 CMachineConfig::GetPktZdriveIpAddr(ENG_LPGS flag, PUINT8 addr)
{
	UINT8 u8Body[8] = { NULL };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 1, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		memcpy(pPktNext, addr, 4);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_z_drive_ip_addr,
							  pPktNext, u32Pkts, u8Body, 4);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_z_drive_ip_addr,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : The ID to be set for the x and y-axis in the motion controller for up to two tables
 parm : flag	- [in]  Get (Read) or Set (Write)
		tbl_no	- [in]  Work Table Number 1 or 2
		xdrvid	- [in]  ID number of the x-motion controller for the given table
		ydrvid	- [in]  ID number of the y-motion controller for the given table
 retn : Pointer where the packet buffer is stored
 note : Valid if MotionControlType = 1 and 3 only. Ignored for other types
*/
PUINT8 CMachineConfig::GetPktXYDriveId(ENG_LPGS flag, UINT8 tbl_no, UINT8 xdrvid, UINT8 ydrvid)
{
	UINT8 u8Body[8] = { tbl_no, xdrvid, ydrvid };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_xy_drive_id,
							  pPktNext, u32Pkts, u8Body, 3);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_xy_drive_id,
							  pPktNext, u32Pkts-m_u32PktSize, u8Body, 1);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : ID identifying the LLS-system
 parm : flag- [in]  Get (Read) or Set (Write)
		pid	- [in]  Product ID of the system
 retn : Pointer where the packet buffer is stored
 note : 16702	= LLS2500
		25002	= LLS04
		50002	= LLS06
		50001	= LLS10
		117502	= LLS15
		100001	= LLS25
		235002	= LLS30
		200001	= LLS50
*/
PUINT8 CMachineConfig::GetPktProductId(ENG_LPGS flag, UINT32 pid)
{
	UINT8 u8Body[8] = { NULL };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		pid	= SWAP32(pid);
		memcpy(u8Body, &pid, 4);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_product_id,
							  pPktNext, u32Pkts, u8Body, 4);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_product_id,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : ArtworkComplexity reflects how fine-pitched the artwork is
		A complex artwork will result in larger preprocessed files
		Selecting a higher. ArtworkComplexity will allow larger,
		more complex preprocessed files to be accepted by Luria,
		by reserving more space inside the photo head(s) storage.
 parm : flag	- [in]  Get (Read) or Set (Write)
		level	- [in]  0 = Normal, 1 = high, 2 = extreme
 retn : Pointer where the packet buffer is stored
 note : Higher complexity setting will result in fewer jobs to be held simultaneously in Luria
		Therefore, ArtworkComplexity should be held as low as possible
		It should only be increased if loading a preprocessed job results in StatusCode “StripFileSizeTooLarge”
		No longer in use from Luria version 3.0.0 and newer
*/
PUINT8 CMachineConfig::GetPktArtworkComplexity(ENG_LPGS flag, UINT8 level)
{
	UINT8 u8Body[8] = { level, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(3, 0, 0, 0x01))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_artwork_complexity,
							  pPktNext, u32Pkts, u8Body, 1);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_artwork_complexity,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Focus Z drive type (This setting is equal for all photo heads)
 parm : flag	- [in]  Get (Read) or Set (Write)
		type	- [in]  Drive Type: 1 = Stepper motor (built-in), 2 = Linear drive (external)
						1 = Stepper motor (use built in driver in photo heads)
						2 = Linear drive from ACS
						3 = Linear drive from Sieb&Meyer (valid from version 2.10.0 and newer)
 retn : Pointer where the packet buffer is stored
 note : Valid from Luria version 2.1.0 and newer
*/
PUINT8 CMachineConfig::GetPktZdriveType(ENG_LPGS flag, UINT8 type)
{
	UINT8 u8Body[8] = { type, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 1, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_z_drive_type,
							  pPktNext, u32Pkts, u8Body, 1);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_z_drive_type,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : When enabled, this mode will configure the fans so that an over-pressure is created inside the photo head
 parm : flag	- [in]  Get (Read) or Set (Write)
		mode	- [in]  1 = Over pressure mode ON, 0 = Over pressure mode OFF
 retn : Pointer where the packet buffer is stored
 note : This requires that a fan is mounted in the front of the photo head!
		Valid from Luria version 2.2.0 and newer
*/
PUINT8 CMachineConfig::GetPktOverPressureMode(ENG_LPGS flag, UINT8 mode)
{
	UINT8 u8Body[8] = { mode, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 2, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_over_pressure_mode,
							  pPktNext, u32Pkts, u8Body, 1);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_over_pressure_mode,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Valid for linear Z-motor only, ignored for stepper motor
		The given DOF will be used by the photo heads to determine if the current focus position during
		autofocus is within the DOF area or not
		To determine if outside DOF, the Focus:OutsideDOFStatus can be read
 parm : flag	- [in]  Get (Read) or Set (Write)
		step	- [in]  Depth of focus (DOF). Given in number of steps
 retn : Pointer where the packet buffer is stored
 note : Valid from Luria version 2.6.0 and newer
*/
PUINT8 CMachineConfig::GetPktDepthOfFocus(ENG_LPGS flag, UINT16 step)
{
	UINT8 u8Body[8] = { NULL };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 6, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		step= SWAP16(step);
		memcpy(u8Body, &step, 2);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_depth_of_focus,
							  pPktNext, u32Pkts, u8Body, 2);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_depth_of_focus,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : When this is enabled the length of a jobs artwork stripes is approximately twice the length of normal stripes
		It is required that the job is preprocessed with the extra long stripe option
		If there is no need for extra long stripe support, this option should be left disabled due to extra processing time during printing.
 parm : flag	- [in]  Get (Read) or Set (Write)
		type	- [in]  0 = Normal length stripes, 1 = Support for extra long stripes (valid for LLS2500 only)
 retn : Pointer where the packet buffer is stored
 note : Obsolete from Luria version 3.0.0 and newer (Support for long stripes default, without extra processing time)
*/
PUINT8 CMachineConfig::GetPktExtraLongStripes(ENG_LPGS flag, UINT8 type)
{
	UINT8 u8Body[8] = { type, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(3, 0, 0, 0x01))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_extra_long_stripes,
							  pPktNext, u32Pkts, u8Body, 1);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_extra_long_stripes,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Enabling MTC will result in smoother edges in cases where the motion of the y-axis is not perfect
		It is required that the job is preprocessed with the MTC mode on
 parm : flag	- [in]  Get (Read) or Set (Write)
		mode	- [in]  0 = Normal exposure mode, 1 = MTC (Machine Tolerance Compensation) mode
 retn : Pointer where the packet buffer is stored
 note : Note that enabling MTC will reduce the maximum scroll rate that can be used
		Valid from Luria version 3.0.0 and newer
*/
PUINT8 CMachineConfig::GetPktMTCMode(ENG_LPGS flag, UINT8 mode)
{
	UINT8 u8Body[8] = { mode, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(3, 0, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_mtc_mode,
							  pPktNext, u32Pkts, u8Body, 1);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_mtc_mode,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : When not using ethercat for AF Luria will not try to communicate over ethercat with the z-motion controller
 parm : flag	- [in]  Get (Read) or Set (Write)
		enable	- [in]  0 = Disable use of ethercat for AF, 1 = Use ethercat for AF
 retn : Pointer where the packet buffer is stored
 note : Valid from Luria version 2.12.0 and newer
*/
PUINT8 CMachineConfig::GetPkUseEthercatForAF(ENG_LPGS flag, UINT8 enable)
{
	UINT8 u8Body[8] = { enable, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 12, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_use_ethercat_for_af,
							  pPktNext, u32Pkts, u8Body, 1);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_use_ethercat_for_af,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : The spx-level from preprocessed files must correspond to the value set here
 parm : flag	- [in]  Get (Read) or Set (Write)
		level	- [in]  Spx-level to be used. Default is 36
 retn : Pointer where the packet buffer is stored
 note : Valid from Luria version 3.0.0 and newer
*/
PUINT8 CMachineConfig::GetPktSpxLevel(ENG_LPGS flag, UINT16 level)
{
	UINT8 u8Body[8] = { NULL };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(3, 0, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		level	= SWAP16(level);
		memcpy(u8Body, &level, 2);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_spx_level,
							  pPktNext, u32Pkts, u8Body, 2);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_spx_level,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : The spx-level from preprocessed files must correspond to the value set here
 parm : flag	- [in]  Get (Read) or Set (Write)
		mask	- [in]  Time that OCP errors should be ignored, due to false OCP values
						In microseconds. Valid values: 6 – 50
 retn : Pointer where the packet buffer is stored
 note : Valid from Luria version 2.12.2 and 3.1.0 and newer
*/
PUINT8 CMachineConfig::GetPktOcpErrorMask(ENG_LPGS flag, UINT16 mask)
{
	UINT8 u8Body[8] = { NULL };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!(ValidVersion(2, 12, 2, 0x02) || ValidVersion(3, 1, 0, 0x00)))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		mask	= SWAP16(mask);
		memcpy(u8Body, &mask, 2);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_ocp_error_mask,
							  pPktNext, u32Pkts, u8Body, 2);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_ocp_error_mask,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Motion controller hysteresis. Note! Valid if MotionControlType = 1 only
		Ignored for other types. This hysteresis is position based, and will not be affected by the print speed.
 parm : flag		- [in]  Function Type : Get or Set
		scroll		- [in]  Scroll-mode (1 – max)
							The hysteresis values below will be used with the corresponding scroll-mode when printing.
							(Max scroll mode may vary for different products)
		p_delay		- [in]  Position-based delay in positive moving direction. In picometers
		n_delay		- [in]  Position-based delay in negative moving direction. In picometers
 retn : Pointer where the packet buffer is stored
 note : This hysteresis is added to the hysteresis done by HysteresisType1
		Valid from Luria version 2.13.0 and 3.2.0 and newer
*/
PUINT8 CMachineConfig::GetPktPositionHysteresisType1(ENG_LPGS flag, UINT8 scroll,
													 UINT32 p_delay, UINT32 n_delay)
{
	UINT8 u8Body[16]= { scroll }, *pBody = u8Body+1;
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!(ValidVersion(2, 13, 0, 0x02) || ValidVersion(3, 2, 0, 0x00)))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		p_delay	= SWAP32(p_delay);
		memcpy(pBody, &p_delay, 4);	pBody	+= 4;
		n_delay	= SWAP32(n_delay);
		memcpy(pBody, &n_delay, 4);	pBody	+= 4;
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_position_hysteresis_type1,
							  pPktNext, u32Pkts, u8Body, 9);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_position_hysteresis_type1,
							  pPktNext, u32Pkts-m_u32PktSize, u8Body, 1);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : This makes it possible to use another bitmap than the built-in default for the edge blending on the left and right side of the DMD
		Note that this command must be re-sent if the bitmap file on the Luria PC is changed, even if the filename is unchanged
 parm : flag	- [in]  Get (Read) or Set (Write)
		file	- [in]	Path and filename (without null termination) of custom edge blend bitmap
						The given bitmap file must exist on the Luria-PC disc and must have the format of 8x1600 pixels for 9k5-based products and 8x1080 for 4k8 products
						The left half of the bitmap is the left bitmap (4 pixels wide) and the right half is the right bitmap (4 pixels wide)
						The bitmap must be in 1-bit monochrome format. Limit: L < 200.
						If the built-in default built-in bitmap is to be used, an empty string with L=0 must be used
		size	- [in]  The size of 'file' buffer
 retn : Pointer where the packet buffer is stored
 note : Valid from 3.2.0 and newer
*/
PUINT8 CMachineConfig::GetPktCustomEdgeBlendBitmap(ENG_LPGS flag, PCHAR file, UINT32 size)
{
	UINT32 u32Pkts	= MAX_PATH_LEN*2;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(3, 2, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCMC::en_custom_edge_blend_bitmap,
							  pPktNext, u32Pkts, PUINT8(file), size);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCMC::en_custom_edge_blend_bitmap,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc :Table Settings 정보 설정
 parm : num		- [in]  table number
		parallel- [in]  Adjustment factor, multiplied by 1000. (Eg. factor 0.97 is given as 970, factor 1.025 is given as 1025).
						Factor 1.0 (1000) means no adjustment, i.e. use the default angle for parallelogram motion.
		y_dir	- [in]  0 = Y axis starts in negative direction, 1 = Y axis starts in positive direction
 retn : 패킷이 저장된 버퍼 포인트 반환 (호출한 쪽에서 반드시 메모리 해제)
*/
PUINT8 CMachineConfig::GetPktTableSettings(ENG_LPGS flag, UINT8 num, UINT32 parallel, UINT8 y_dir)
{
	UINT32 u32Pkts	= 512;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Preprocessing : Network Bytes Ordering */
	UINT32 u32Parallel	= htonl(parallel);
	UINT8 u8Adjust[8]	= {num, };
	UINT8 u8Direct[8]	= {num, y_dir};
	memcpy(u8Adjust+1,	&u32Parallel, 4);

	UINT32 u32PktSize	= 0, u32Size[2][2]	= { {5, 2}, {1, 1} }, i = 1 /* important */;
	PUINT8 pData[2][2]	= { {u8Adjust, u8Direct}, {&num, &num} };
	ENG_LTCT enType[2]	= { ENG_LTCT::en_write, ENG_LTCT::en_read };

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Get or Set */
	if (flag == ENG_LPGS::en_set)	i = 0 /* important */;
	for (; i<2; i++)
	{
		/* Table Settings - Parallel Ogram Adjust */
		pPktNext = GetPktBase(enType[i], (UINT8)ENG_LCMC::en_parallelogram_motion_adjust,
							  pPktNext, u32Pkts, pData[i][0], u32Size[i][0]);
		/* Table Settings - Start Print In Pos Y Dir */
		pPktNext = GetPktBase(enType[i], (UINT8)ENG_LCMC::en_table_print_direction,
							  pPktNext, u32Pkts-m_u32PktSize, pData[i][1], u32Size[i][1]);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}


/* --------------------------------------------------------------------------------------------- */
/*                                    Packet Function (Save)                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc :수신받은 데이터 처리 - Machine Config
 parm : uid		- [in]  서브 명령어
		data	- [in]  데이터가 저장된 버퍼 포인터
		size	- [in]  'data' 의 크기
 retn : None
*/
VOID CMachineConfig::SetRecvPacket(UINT8 uid, PUINT8 data, UINT32 size)
{
	switch (uid)
	{
	case (UINT8)ENG_LCMC::en_total_photo_heads				:
	case (UINT8)ENG_LCMC::en_photo_head_pitch				:
	case (UINT8)ENG_LCMC::en_photo_head_rotate				:
	case (UINT8)ENG_LCMC::en_motion_control_type			:
	case (UINT8)ENG_LCMC::en_active_table					:
	case (UINT8)ENG_LCMC::en_emulate_motor_controller		:
	case (UINT8)ENG_LCMC::en_emulate_photo_heads			:
	case (UINT8)ENG_LCMC::en_emulate_triggers				:
	case (UINT8)ENG_LCMC::en_debug_print					:
	case (UINT8)ENG_LCMC::en_z_drive_type					:
	case (UINT8)ENG_LCMC::en_artwork_complexity				:
	case (UINT8)ENG_LCMC::en_over_pressure_mode				:
	case (UINT8)ENG_LCMC::en_mtc_mode						:
	case (UINT8)ENG_LCMC::en_extra_long_stripes				:
	case (UINT8)ENG_LCMC::en_use_ethercat_for_af			:	SetRecv1Bytes(uid, data);			break;
	case (UINT8)ENG_LCMC::en_scroll_rate					:
	case (UINT8)ENG_LCMC::en_xy_drive_id					:
	case (UINT8)ENG_LCMC::en_table_print_direction			:
	case (UINT8)ENG_LCMC::en_depth_of_focus					:
	case (UINT8)ENG_LCMC::en_ocp_error_mask					:
	case (UINT8)ENG_LCMC::en_spx_level						:	SetRecv2Bytes(uid, data);			break;
	case (UINT8)ENG_LCMC::en_motion_control_ipaddr			:
	case (UINT8)ENG_LCMC::en_max_y_motion_speed				:
	case (UINT8)ENG_LCMC::en_x_motion_speed					:
	case (UINT8)ENG_LCMC::en_z_drive_ip_addr				:
	case (UINT8)ENG_LCMC::en_y_acceleration_distance		:
	case (UINT8)ENG_LCMC::en_product_id						:	SetRecv4Bytes(uid, data);			break;
	case (UINT8)ENG_LCMC::en_photo_head_ip_addr				:
	case (UINT8)ENG_LCMC::en_parallelogram_motion_adjust	:	SetRecv5Bytes(uid, data);			break;
	case (UINT8)ENG_LCMC::en_get_table_motion_start_position:
	case (UINT8)ENG_LCMC::en_position_hysteresis_type1		:
	case (UINT8)ENG_LCMC::en_table_exposure_start_pos		:
	case (UINT8)ENG_LCMC::en_photo_head_offset				:	SetRecv9Bytes(uid, data);			break;
	case (UINT8)ENG_LCMC::en_x_correction_table				:
	case (UINT8)ENG_LCMC::en_y_correction_table				:
	case (UINT8)ENG_LCMC::en_table_position_limits			:
	case (UINT8)ENG_LCMC::en_hysteresis_type1				:
	case (UINT8)ENG_LCMC::en_linear_z_drive_settings		:
	case (UINT8)ENG_LCMC::en_custom_edge_blend_bitmap		:
	case (UINT8)ENG_LCMC::en_print_simulation_out_dir		:	SetRecvEtcBytes(uid, data, size);	break;
	default:	return;
	}
}

/*
 desc :1 Bytes 수신 데이터 처리
 parm : uid		- [in]  서브 명령어
		data	- [in]  수신된 데이터
 retn : None
*/
VOID CMachineConfig::SetRecv1Bytes(UINT8 uid, PUINT8 data)
{
	switch (uid)
	{
	case (UINT8)ENG_LCMC::en_total_photo_heads			:	m_pstShMem->total_ph				= data[0];	break;
	case (UINT8)ENG_LCMC::en_photo_head_pitch			:	m_pstShMem->ph_pitch				= data[0];	break;
	case (UINT8)ENG_LCMC::en_photo_head_rotate			:	m_pstShMem->ph_rotate				= data[0];	break;
	case (UINT8)ENG_LCMC::en_motion_control_type		:	m_pstShMem->motion_control_type		= data[0];	break;
	case (UINT8)ENG_LCMC::en_active_table				:	m_pstShMem->active_table			= data[0];	break;
	case (UINT8)ENG_LCMC::en_emulate_motor_controller	:	m_pstShMem->emulate_motor_controller= data[0];	break;
	case (UINT8)ENG_LCMC::en_emulate_photo_heads		:	m_pstShMem->emulate_ph				= data[0];	break;
	case (UINT8)ENG_LCMC::en_emulate_triggers			:	m_pstShMem->emulate_triggers		= data[0];	break;
	case (UINT8)ENG_LCMC::en_debug_print				:	m_pstShMem->debug_print_level		= data[0];	break;
	case (UINT8)ENG_LCMC::en_z_drive_type				:	m_pstShMem->z_drive_type			= data[0];	break;
	case (UINT8)ENG_LCMC::en_artwork_complexity			:	m_pstShMem->artwork_complexity		= data[0];	break;
	case (UINT8)ENG_LCMC::en_over_pressure_mode			:	m_pstShMem->over_pressure_mode		= data[0];	break;
	case (UINT8)ENG_LCMC::en_mtc_mode					:	m_pstShMem->mtc_mode				= data[0];	break;
	case (UINT8)ENG_LCMC::en_use_ethercat_for_af		:	m_pstShMem->use_ethercat_for_af		= data[0];	break;
	case (UINT8)ENG_LCMC::en_extra_long_stripes			:	m_pstShMem->extra_long_stripes		= data[0];	break;
	}
}

/*
 desc :2 Bytes 수신 데이터 처리
 parm : uid		- [in]  서브 명령어
		data	- [in]  수신된 데이터
 retn : None
*/
VOID CMachineConfig::SetRecv2Bytes(UINT8 uid, PUINT8 data)
{
	UINT16 u16Data	= 0;

	switch (uid)
	{
	case (UINT8)ENG_LCMC::en_ocp_error_mask			:
	case (UINT8)ENG_LCMC::en_spx_level				:
	case (UINT8)ENG_LCMC::en_scroll_rate			:
	case (UINT8)ENG_LCMC::en_depth_of_focus			:	memcpy(&u16Data, data, sizeof(UINT16));
														u16Data	= SWAP16(u16Data);
		switch (uid)
		{
		case (UINT8)ENG_LCMC::en_scroll_rate		:	m_pstShMem->scroll_rate		= u16Data;					break;
		case (UINT8)ENG_LCMC::en_depth_of_focus		:	m_pstShMem->depth_of_focus	= u16Data;					break;
		case (UINT8)ENG_LCMC::en_spx_level			:	m_pstShMem->spx_level		= u16Data;					break;
		case (UINT8)ENG_LCMC::en_ocp_error_mask		:	m_pstShMem->ocp_error_mask	= u16Data;					break;
		}
		break;
														
	case (UINT8)ENG_LCMC::en_xy_drive_id			:	m_pstShMem->xy_drive_id[data[0]-1][0]		= data[1];			/* X Motion Drive Id */
														m_pstShMem->xy_drive_id[data[0]-1][1]		= data[2];	break;	/* Y Motion Drive Id */
	case (UINT8)ENG_LCMC::en_table_print_direction	:	m_pstShMem->table_print_direction[data[0]-1]= data[1];	break;
	}
}

/*
 desc :4 Bytes 수신 데이터 처리
 parm : uid		- [in]  서브 명령어
		data	- [in]  수신된 데이터
 retn : None
*/
VOID CMachineConfig::SetRecv4Bytes(UINT8 uid, PUINT8 data)
{
	UINT32 u32Data	= 0;

	/* 4바이트 값 무조건 복사 */
	memcpy(&u32Data, data, 4);
	u32Data	= SWAP32(u32Data);

	switch (uid)
	{
	case (UINT8)ENG_LCMC::en_motion_control_ipaddr		:	memcpy(m_pstShMem->motion_control_ipv4, data, 4);	break;
	case (UINT8)ENG_LCMC::en_x_motion_speed				:	m_pstShMem->x_motion_speed			= u32Data;		break;
	case (UINT8)ENG_LCMC::en_max_y_motion_speed			:	m_pstShMem->max_y_motion_speed		= u32Data;		break;
	case (UINT8)ENG_LCMC::en_y_acceleration_distance	:	m_pstShMem->y_acceleration_distance	= u32Data;		break;
	case (UINT8)ENG_LCMC::en_product_id					:	m_pstShMem->product_id				= u32Data;		break;
	case (UINT8)ENG_LCMC::en_z_drive_ip_addr			:	memcpy(m_pstShMem->z_drive_ipv4, data, 4);			break;
	}
}

/*
 desc :5 Bytes 수신 데이터 처리
 parm : uid		- [in]  서브 명령어
		data	- [in]  수신된 데이터
 retn : None
*/
VOID CMachineConfig::SetRecv5Bytes(UINT8 uid, PUINT8 data)
{
	UINT32 u32Data	= 0;

	switch (uid)
	{
	case (UINT8)ENG_LCMC::en_photo_head_ip_addr				:	
		memcpy(m_pstShMem->ph_ipv4[data[0]-1], data+1, 4);		break;
	case (UINT8)ENG_LCMC::en_parallelogram_motion_adjust	:	
		memcpy(&u32Data, data+1, 4);
		m_pstShMem->parallelogram_adjust[data[0]-1]	= SWAP32(u32Data);	break;
	}
}

/*
 desc :9 Bytes 수신 데이터 처리
 parm : uid		- [in]  서브 명령어
		data	- [in]  수신된 데이터
 retn : None
*/
VOID CMachineConfig::SetRecv9Bytes(UINT8 uid, PUINT8 data)
{
	UINT8 u8Data	= 0;
	UINT32 u32Data	= 0;

	switch (uid)
	{
	case (UINT8)ENG_LCMC::en_get_table_motion_start_position:	/* 노광이 시작하기 위해 이동되는 모션 위치 */
		memcpy(&u32Data, data+1, 4);
		m_pstShMem->get_table_motion_start_xy[data[0]-1].x	= SWAP32(u32Data);
		memcpy(&u32Data, data+5, 4);
		m_pstShMem->get_table_motion_start_xy[data[0]-1].y	= SWAP32(u32Data);	break;
	case (UINT8)ENG_LCMC::en_table_exposure_start_pos		:	/* 노광이 시작되는 모션 위치 */
		memcpy(&u32Data, data+1, 4);
		m_pstShMem->table_expo_start_xy[data[0]-1].x		= SWAP32(u32Data);
		memcpy(&u32Data, data+5, 4);
		m_pstShMem->table_expo_start_xy[data[0]-1].y		= SWAP32(u32Data);	break;
	case (UINT8)ENG_LCMC::en_photo_head_offset				:	/* 광학계 1 번 기준으로 Offset */
		memcpy(&u32Data, data+1, 4);
		m_pstShMem->ph_offset_xy[data[0]-1].pos_offset_x	= SWAP32(u32Data);
		memcpy(&u32Data, data+5, 4);
		m_pstShMem->ph_offset_xy[data[0]-1].pos_offset_y	= SWAP32(u32Data);	break;
	case (UINT8)ENG_LCMC::en_position_hysteresis_type1		:
		u8Data	= data[0];
		memcpy(&u32Data, data+1, 4);
		m_pstShMem->scroll_mode_delay_pos[u8Data-1]	= SWAP32(u32Data);
		memcpy(&u32Data, data+5, 4);
		m_pstShMem->scroll_mode_delay_neg[u8Data-1]	= SWAP32(u32Data);	break;
	}
}

/*
 desc :xx Bytes 수신 데이터 처리
 parm : uid		- [in]  서브 명령어
		data	- [in]  수신된 데이터
		size	- [in]  'data' 버퍼의 크기
 retn : None
*/
VOID CMachineConfig::SetRecvEtcBytes(UINT8 uid, PUINT8 data, UINT32 size)
{
	UINT16 u16Data	= 0;
	UINT32 u32Data	= 0;

	switch (uid)
	{
	case (UINT8)ENG_LCMC::en_table_position_limits		:
		memcpy(&u32Data, data+1, 4);
		m_pstShMem->table_position_limit[data[0]-1].min_xy[0]	= SWAP32(u32Data);
		memcpy(&u32Data, data+5, 4);
		m_pstShMem->table_position_limit[data[0]-1].min_xy[1]	= SWAP32(u32Data);
		memcpy(&u32Data, data+9, 4);
		m_pstShMem->table_position_limit[data[0]-1].max_xy[0]	= SWAP32(u32Data);
		memcpy(&u32Data, data+13, 4);
		m_pstShMem->table_position_limit[data[0]-1].max_xy[1]	= SWAP32(u32Data);	break;
	case (UINT8)ENG_LCMC::en_hysteresis_type1			:
		m_pstShMem->hysteresis_type1.scroll_mode	= data[0];
		memcpy(&u16Data, data+1, 2);
		m_pstShMem->hysteresis_type1.negative_offset			= SWAP16(u16Data);
		memcpy(&u32Data, data+3, 4);
		m_pstShMem->hysteresis_type1.delay_positive				= SWAP32(u32Data);			/* Direction : Positvie */
		memcpy(&u32Data, data+7, 4);
		m_pstShMem->hysteresis_type1.delay_negative				= SWAP32(u32Data);	break;	/* Direction : Negative*/
	case (UINT8)ENG_LCMC::en_print_simulation_out_dir	:
		if (data && size > 0 && size < MAX_PATH_LEN)
		{
#if 0 	/* Because of C# T.T */
			memcpy(m_pstShMem->print_simulate_out_dir, data, size);
			m_pstShMem->print_simulate_out_dir[size]	= 0x00;
#else
			memset(m_pstShMem->print_simulate_out_dir, 0x00, MAX_PATH_LEN);
			memcpy(m_pstShMem->print_simulate_out_dir, data, size);
#endif
		}	break;
	case (UINT8)ENG_LCMC::en_custom_edge_blend_bitmap	:
		if (data && size > 0 && size < MAX_PATH_LEN)
		{
#if 0 	/* Because of C# T.T */
			memcpy(m_pstShMem->custom_edge_blen_bitmap, data, size);
			m_pstShMem->custom_edge_blen_bitmap[size]	= 0x00;
#else
			memset(m_pstShMem->custom_edge_blen_bitmap, 0x00, MAX_PATH_LEN);
			memcpy(m_pstShMem->custom_edge_blen_bitmap, data, size);
#endif
		}	break;
	case (UINT8)ENG_LCMC::en_x_correction_table			:
		if (data && size > 0)
		{
			UINT8 u8TblNo	= data[0], *pData = data+1;
			UINT16 u16Count	= 0, i = 0;
			memcpy(&u16Count, pData, 2);	pData	+= 2;
			m_pstShMem->x_correction_count	= SWAP16(u16Count);
			for (; i<m_pstShMem->x_correction_count; i++)
			{
				memcpy(&m_pstShMem->x_correction_table[u8TblNo-1][i], pData, 8);	pData	+= 8;
				m_pstShMem->x_correction_table[u8TblNo-1][i].SWAP();
			}
		}	break;
	case (UINT8)ENG_LCMC::en_y_correction_table			:
		if (data && size > 0)
		{
			UINT8 u8TblNo	= data[0], *pData = data+1;
			UINT16 u16Count	= 0, i = 0;
			memcpy(&u16Count, pData, 2);	pData	+= 2;
			m_pstShMem->y_correction_count	= SWAP16(u16Count);
			for (; i<m_pstShMem->y_correction_count; i++)
			{
				m_pstShMem->y_correction_table[u8TblNo-1][i].strip_no	= pData[0];	pData	+= 1;
				memcpy(&m_pstShMem->y_correction_table[u8TblNo-1][i], pData, 4);	pData	+= 4;
				m_pstShMem->y_correction_table[u8TblNo-1][i].SWAP();
			}
		}	break;
	case (UINT8)ENG_LCMC::en_linear_z_drive_settings	:
		m_pstShMem->linear_z_drive_settings[data[0]-1]	= data[1];
		break;
	}
}
