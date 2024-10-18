
/*
 desc : User Data Family : Panel Preparation
*/

#include "pch.h"
#include "MainApp.h"
#include "PanelPreparation.h"


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
CPanelPreparation::CPanelPreparation(LPG_LDSM shmem, CCodeToStr *error)
	: CBaseFamily(shmem, error)
{

	/* 기본 Family ID 설정 */
	m_enFamily	= ENG_LUDF::en_panel_preparation;
	/* Panel Preparation Structure */
	m_pstPanel	= &shmem->panel;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CPanelPreparation::~CPanelPreparation()
{
}

/* --------------------------------------------------------------------------------------------- */
/*                                    Packet Function (Make)                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Dynamic fiducial coordinates. When sending dynamic fiducials these will be used instead of global
		and local fiducials from the gerber-file. Up to 8 (N <= 8) global dynamic fiducials,
		while up to 200 (N <= 200) local dynamic fiducials are supported
 parm : flag	- [in]  Function Type : Get or Set
		type	- [in]  0x00: Global, 0x01 : Local
		count	- [in]  Fiducial Count
		fidxy	- [in]  Array pointer containing X/Y coordinates [xx][2] (unit: nm)
 retn : Pointer where the packet buffer is stored
 note : To enable use of global and local fiducials from gerber-file again,
		then a special DynamicFiducials command where type = 0 (global) and N = 0 must be sent
*/
PUINT8 CPanelPreparation::GetPktDynamicFiducials(ENG_LPGS flag, UINT8 type,
												 UINT16 count, INT32 **fidxy)
{
	UINT32 u32Pkts	= 4096;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		UINT16 u16Count	= SWAP16(count), i = 0;
		PUINT8 pBody	= NULL, pNext;
		UINT32 u32Body	= 0;
		pBody = new UINT8[sizeof(INT32) * count * 2 + 8];// (PUINT8)::Alloc(sizeof(INT32) * count * 2 + 8);
		memset(pBody, 0x00, sizeof(INT32)*count*2+8);
		pNext	= pBody;
		/* Set the send packet */
		pNext[0]= type;						pNext	+= 1;
		memcpy(pNext, &u16Count, 2);		pNext	+= 2;
		for (u32Body=3; i<count; i++)
		{
			fidxy[i][0]	= SWAP32(fidxy[i][0]);
			fidxy[i][1]	= SWAP32(fidxy[i][1]);
			memcpy(pNext, &fidxy[i][0], 4);	pNext	+= 4;
			memcpy(pNext, &fidxy[i][1], 4);	pNext	+= 4;
			u32Body	+= 8;
		}
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPP::en_dynamic_fiducials,
							  pPktNext, u32Pkts, pBody, u32Body);
		delete pBody;
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPP::en_dynamic_fiducials,
							  pPktNext, u32Pkts-m_u32PktSize, (PUINT8)&type, 1);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Registration points. Up to 400 registration points are supported, ie. N <= 400
 parm : flag	- [in]  Function Type : Get or Set
		count	- [in]  Global & Local Fiducial Count
		fidxy	- [in]  Structure pointer containing X/Y coordinates (unit: nm)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CPanelPreparation::GetPktRegistrationPoints(ENG_LPGS flag, UINT16 count, LPG_I32XY fidxy)
{
	UINT32 u32Pkts	= 4096;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		UINT16 u16Count	= SWAP16(count), i = 0;
		PUINT8 pBody	= NULL, pNext;
		UINT32 u32Body	= 0;
		pBody = new UINT8[sizeof(INT32) * count * 2 + 8];// (PUINT8)::Alloc(sizeof(INT32) * count * 2 + 8);
		memset(pBody, 0x00, sizeof(INT32)*count*2+8);
		pNext	= pBody;
		/* Set the send packet */
		memcpy(pNext, &u16Count, 2);		pNext	+= 2;
		for (u32Body=2; i<count; i++)
		{
			fidxy[i].SWAP();
			memcpy(pNext, &fidxy[i].x, 4);	pNext	+= 4;
			memcpy(pNext, &fidxy[i].y, 4);	pNext	+= 4;
			u32Body	+= 8;
		}
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPP::en_registration_points,
							  pPktNext, u32Pkts, pBody, u32Body);
		delete pBody;
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPP::en_registration_points,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Transformation recipe that is valid for global registration
		If set to Fixed, the values in GlobalFixedRotation, GlobalFixedScaling and GlobalFixedOffset are used
 parm : flag	- [in]  Function Type : Get or Set
		rotation- [in]  Rotating	0x00 - Auto, 0x01 = Fixed
		scaling	- [in]  Scaling		0x00 - Auto, 0x01 = Fixed
		offset	- [in]  Offset		0x00 - Auto, 0x01 = Fixed
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CPanelPreparation::GetPktGlobalTransformationRecipe(ENG_LPGS flag,
														   UINT8 rotation, UINT8 scaling, UINT8 offset)
{
	UINT8 u8Body[8]	= { rotation, scaling, offset, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPP::en_global_transformation_receipe,
							  pPktNext, u32Pkts, u8Body, 3);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPP::en_global_transformation_receipe,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Has effect in fixed transformation mode only. Fixed rotation to be used. N <= 200
 parm : flag	- [in]  Function Type : Get or Set
		rotation- [in]  Global fixed rotation, in micro-degrees. Ignored for TransformationReceipe 0.
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CPanelPreparation::GetPktGlobalFixedRotation(ENG_LPGS flag, INT32 rotation)
{
	UINT8 u8Body[8]	= { NULL };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

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
		rotation	= SWAP32(rotation);
		memcpy(u8Body, &rotation, 4);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPP::en_global_fixed_rotation,
							  pPktNext, u32Pkts, u8Body, 4);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPP::en_global_fixed_rotation,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Has effect in fixed transformation mode only
		Fixed scaling to be used. N <= 200
		(Eg. Factor 1.0002 is given as 1000200, factor 0.9998 is given as 999800)
 parm : flag	- [in]  Function Type : Get or Set
		scale_x	- [in]  Global fixed x-scaling factor * 10^6. Ignored for TransformationReceipe 0.
		scale_y	- [in]  Global fixed y-scaling factor * 10^6. Ignored for TransformationReceipe 0.
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CPanelPreparation::GetPktGlobalFixedScaling(ENG_LPGS flag, UINT32 scale_x, UINT32 scale_y)
{
	UINT8 u8Body[16]= { NULL }, *pBody = u8Body;
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

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
		scale_x	= SWAP32(scale_x);
		scale_y	= SWAP32(scale_y);
		memcpy(pBody, &scale_x, 4);	pBody	+= 4;
		memcpy(pBody, &scale_y, 4);	pBody	+= 4;
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPP::en_global_fixed_scaling,
							  pPktNext, u32Pkts, u8Body, 8);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPP::en_global_fixed_scaling,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Has effect in fixed transformation mode only. Fixed offset to be used. N <= 200
 parm : flag	- [in]  Function Type : Get or Set
		offset_x- [in]  Global fixed x-offset. factor (unit: nm). Ignored for TransformationReceipe 0.
		offset_x- [in]  Global fixed y-offset. factor (unit: nm). Ignored for TransformationReceipe 0.
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CPanelPreparation::GetPktGlobalFixedOffset(ENG_LPGS flag, INT32 offset_x, INT32 offset_y)
{
	UINT8 u8Body[16]= { NULL }, *pBody = u8Body;
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

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
		offset_x	= SWAP32(offset_x);
		offset_y	= SWAP32(offset_y);
		memcpy(pBody, &offset_x, 4);	pBody	+= 4;
		memcpy(pBody, &offset_y, 4);	pBody	+= 4;
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPP::en_global_fixed_offset,
							  pPktNext, u32Pkts, u8Body, 8);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPP::en_global_fixed_offset,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : This will determine the zone geometry to be used for local zones
 parm : flag	- [in]  Function Type : Get or Set
		fid_x	- [in]  Number of fiducial points in x-direction for one zone
		fid_y	- [in]  Number of fiducial points in y-direction for one zone
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CPanelPreparation::GetPktLocalZoneGeometry(ENG_LPGS flag, UINT16 fid_x, UINT16 fid_y)
{
	UINT8 u8Body[8]	= { NULL }, *pBody = u8Body;
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

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
		fid_x	= SWAP16(fid_x);
		fid_y	= SWAP16(fid_y);
		memcpy(pBody, &fid_x, 2);	pBody	+= 2;
		memcpy(pBody, &fid_y, 2);	pBody	+= 2;
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPP::en_local_zone_geometry,
							  pPktNext, u32Pkts, u8Body, 4);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPP::en_local_zone_geometry,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : The number of calculated local zones
 parm : None
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CPanelPreparation::GetPktGetNumberOfLocalZones()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Read */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPP::en_get_number_of_local_zones,
						  pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Retrieve the transformation parameters based on the current registration points
		and transformation recipe
 parm : zone	- [in]  Zone number ( 0 = Global, 1 and above: local zone numbers)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CPanelPreparation::GetPktGetTransformationParams(UINT16 zone)
{
	UINT8 u8Body[8]	= { NULL }, *pBody = u8Body;
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */
	zone	= SWAP16(zone);
	memcpy(pBody, &zone, 2);	pBody	+= 2;
	/* Read */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPP::en_get_transformation_params,
						  pPktNext, u32Pkts, u8Body, 2);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Will run registration based on the current fiducials and registration points
		If registration error is received, then the detailed error can be found using
		“GetRegistrationStatus” command
 parm : None
 retn : Pointer where the packet buffer is stored
 note : Note that this command is also run automatically during PrePrint if this function
		(RunRegistration) has not been called explicitly after changing registration parameters
*/
PUINT8 CPanelPreparation::GetPktRunRegistration()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Read */
	pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPP::en_run_registration,
						  pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Retrieve the current status of the registration module in Luria
 parm : None
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CPanelPreparation::GetPktGetRegistrationStatus()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Read */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPP::en_get_registration_status,
						  pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Set the serial number string and number to be incremented per board
		If the serial number string is set to ABCD####WXYZ, the symbol marking increment number is '#'
		and starting value is set to 0123, then the first serial number will be printed as ABCD0123WXYZ,
		the next ABCD0124WXYZ and so on for all the D-codes defined as SerialNumber
		If the number reaches max value, e.g. 999, the next value will be 000.
 parm : flag	- [in]  Function Type : Get or Set
		type	- [in]  Command Type : Read or Write
		dcode	- [in]	D-Code
		flip_h	- [in]  Horizontal Flip (1: Flip, 0: No Flip)
		flip_v	- [in]  Vertical Flip (1: Flip, 0: No Flip)
		font_h	- [in]  font size (Horizontal) (unit : um)
		font_v	- [in]  font size (Vertical) (unit : um)
		symbol	- [in]  Symbol marking number to be incremented (e.g. '#').
		start	- [in]  Starting value of number to be incremented.
		s_len	- [in]  Length of string (S). Max = 32.
		serial	- [in]  Serial number string
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CPanelPreparation::GetPktSerialNumber(ENG_LPGS flag,
											 UINT32 dcode, UINT8 symbol,
											 UINT16 s_len, PUINT8 serial, UINT32 start,
											 UINT8 flip_h, UINT8 flip_v,
											 UINT16 font_h, UINT16 font_v)
{
	UINT8 u8Body[96]= { NULL }, *pBody = u8Body;
	UINT16 u16Len	= s_len;
	UINT32 u32Pkts	= 256;
	UINT32 u32Body	= 4/*dcode*/+2/*flip*/+4/*font_xy*/+1/*symbol*/+4/*start*/+2/*s_len*/+s_len;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */
	dcode	= SWAP32(dcode);	/* Get Panel Data Information */
	memcpy(pBody, &dcode, 4);		pBody += 4;	/* D-Code */
	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
										memcpy(pBody, &flip_h, 1);		pBody += 1;	/* flip horizontal */
										memcpy(pBody, &flip_v, 1);		pBody += 1;	/* flip vertical */
		font_h	= htons(font_h);		memcpy(pBody, &font_h, 2);		pBody += 2;	/* Font Size : Horizontal */
		font_v	= htons(font_v);		memcpy(pBody, &font_v, 2);		pBody += 2;	/* Font Size : Vertical */
										memcpy(pBody, &symbol, 1);		pBody += 1;	/* symbol */
		start	= htonl(start);			memcpy(pBody, &start, 4);		pBody += 4;	/* Start No. */
		s_len	= htons(s_len);			memcpy(pBody, &s_len, 2);		pBody += 2;	/* s_len */
										memcpy(pBody, serial, u16Len);				/* serial */
		/* Set Serial Number */
		pPktNext	= GetPktBase(ENG_LTCT::en_write, (UINT8)ENG_LCPP::en_panel_data_serial_number,
								 pPktNext, u32Pkts, u8Body, u32Body);
	}
	/* Read */
	else if(ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag)
	{
		pPktNext	= GetPktBase(ENG_LTCT::en_read, (UINT8)ENG_LCPP::en_panel_data_serial_number,
								 pPktNext, u32Pkts-m_u32PktSize, u8Body, 4);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Specify the panel data to be printed on the panel, where to print it and the size/orientation
		D-code must match what is specified in the job (from PreProcessing),
		where also the PanelData type is pre-defined
		For Scaling information, the Length of string should be set to 0 and Text string (S) should be ignored
		For GeneralText and ECC 200 the text string is what will be printed/shown hence Length of text string
		and text string must be set accordingly
		For Paneldata-BMP the Text string specifies the path of the bitmap to be used
		The paneldata bitmap must have: 1 bit per pixel, max width/height 512 pixels,
		total bitmap size cannot be larger than 32768 pixels and width/height must be divisible by 32
 parm : flag	- [in]  Function Type : Get or Set
		dcode	- [in]  D-Code (Panel Dcode List 값 중 1개)
		flip_x	- [in]  X-Flip. 1 = flip, 0 = no flip
		flip_y	- [in]  Y-Flip. 1 = flip, 0 = no flip
		font_h	- [in]  font size (Horizontal) (unit : um)
		font_v	- [in]  font size (Vertical) (unit : um)
		s_len	- [in]  Length of string (S). (Only valid for GeneralText, should be 0 in other cases). Max = 32.
		text	- [in]  Text string
 retn : 패킷이 저장된 버퍼 포인트 반환 (호출한 쪽에서 반드시 메모리 해제)
*/
PUINT8 CPanelPreparation::GetPktPanelData(ENG_LPGS flag,
										  UINT32 dcode, UINT16 s_len, PUINT8 text, 
										  UINT8 flip_x, UINT8 flip_y, UINT16 font_h, UINT16 font_v)
{
	UINT8 u8Body[96]= { NULL }, *pBody = u8Body;
	UINT16 u16Len	= s_len;
	UINT32 u32Pkts	= 256;
	UINT32 u32Body	= 4/*dcode*/+2/*flip_x/y*/+4/*font_xy*/+2/*s_len*/+s_len;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */
	dcode	= SWAP32(dcode);	/* Get Panel Data Information */
	memcpy(pBody, &dcode, 4);		pBody += 4;	/* D-Code */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{		
									memcpy(pBody, &flip_x, 1);		pBody += 1;	/* flip_x */
									memcpy(pBody, &flip_y, 1);		pBody += 1;	/* flip_y */
		font_h	= htons(font_h);	memcpy(pBody, &font_h, 2);		pBody += 2;	/* Font Size : Horizontal */
		font_v	= htons(font_v);	memcpy(pBody, &font_v, 2);		pBody += 2;	/* Font Size : Vertical */
		s_len	= htons(s_len);		memcpy(pBody, &s_len, 2);		pBody += 2;	/* s_len */
									memcpy(pBody, text, u16Len);				/* text */

		/* Set Panel Data Information */
		pPktNext	= GetPktBase(ENG_LTCT::en_write, (UINT8)ENG_LCPP::en_panel_data,
								 pPktNext, u32Pkts, u8Body, u32Body);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext	= GetPktBase(ENG_LTCT::en_read, (UINT8)ENG_LCPP::en_panel_data,
								 pPktNext, u32Pkts-m_u32PktSize, u8Body, 4);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : If “Use shared zones” is selected, then the system will try to create shared zones where possible
 parm : flag	- [in]  Get (Read) or Set (Write)
		use		- [in]  1 = Use shared zones, 0 = Do not use shared zoned
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CPanelPreparation::GetPktUseSharedLocalZones(ENG_LPGS flag, UINT8 use)
{
	UINT8 u8Body[8] = { use, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPP::en_use_shared_local_zones,
							  pPktNext, u32Pkts, u8Body, 1);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPP::en_use_shared_local_zones,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Transformation recipe that is valid for local zones
		If set to all Fixed, the values in LocalFixedRotation, LocalFixedScaling and LocalFixedOffset are used
 parm : flag	- [in]  Get (Read) or Set (Write)
		info	- [in]  Local transformation recipe for all zones
						Bit 0 All transformation parameters fixed, 0 = Auto, 1 = Fixed
						If any of the other bits 1 to 3 is set, these bits override bit 0, and Rotation, Scaling and Offset may be set on or off individualy.
						Bit 1 Rotation. 0 = Auto, 1 = Fixed. LocalFixedRotation input are used.
						Bit 2 Scaling. 0 = Auto, 1 = Fixed. LocalFixedScaling input are used.
						Bit 3 Offset. 0 = Auto, 1 = Fixed. LocalFixedOffset input are used.
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CPanelPreparation::GetPktLocalTransformationRecipe(ENG_LPGS flag, UINT8 info)
{
	UINT8 u8Body[8] = { info, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPP::en_local_transformation_recipe,
							  pPktNext, u32Pkts, u8Body, 1);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPP::en_local_transformation_recipe,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Has effect in fixed transformation mode only. Fixed rotation to be used. N <= 200
 parm : flag	- [in]  Function Type : Get or Set
		count	- [in]  Number of local zones (N)
		rotation- [in]  Fixed rotation for local zone 1, in micro-degrees
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CPanelPreparation::GetPktLocalFixedRotation(ENG_LPGS flag, UINT16 count, PINT32 rotation)
{
	PUINT8 pBody	= NULL, pNext;
	UINT16 u16Count	= SWAP16(count), i = 0;
	UINT32 u32Pkts	= 8192, u32Body = 0;
	PUINT8 pPktBuff	= NULL, pPktNext;

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
		pBody = new UINT8[sizeof(INT32) * count + 8];//(PUINT8)::Alloc(sizeof(INT32)*count+8);
		memset(pBody, 0x00, sizeof(INT32)*count+8);
		pNext	= pBody;
		memcpy(pNext, &u16Count, 2);	pNext	+= 2;
		for (; i<count; i++)
		{
			rotation[i]	= SWAP32(rotation[i]);
			memcpy(pNext, &rotation[i], 4);	pNext	+= 4;
		}
		u32Body	= UINT32(pNext - pBody);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPP::en_local_fixed_rotation,
							  pPktNext, u32Pkts, pBody, u32Body);
		delete pBody;
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPP::en_local_fixed_rotation,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Has effect in fixed transformation mode only
		Fixed scaling to be used. N <= 200.
		(Eg. Factor 1.0002 is given as 1000200, factor 0.9998 is given as 999800)
 parm : flag	- [in]  Function Type : Get or Set
		count	- [in]  Number of local zones (N)
		scale_x	- [in]  Local zone 1 fixed x-scaling factor * 10^6
		scale_y	- [in]  Local zone 1 fixed y-scaling factor * 10^6
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CPanelPreparation::GetPktLocalFixedScaling(ENG_LPGS flag, UINT16 count,
												  PUINT32 scale_x, PUINT32 scale_y)
{
	PUINT8 pBody	= NULL, pNext;
	UINT16 u16Count	= SWAP16(count), i = 0;
	UINT32 u32Pkts	= 8192, u32Body = 0;
	PUINT8 pPktBuff	= NULL, pPktNext;

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
		pBody = new UINT8[sizeof(INT32) * count + 8];//= (PUINT8)::Alloc(sizeof(INT32)*count+8);
		memset(pBody, 0x00, sizeof(INT32)*count+8);
		pNext	= pBody;
		memcpy(pNext, &u16Count, 2);	pNext	+= 2;
		for (; i<count; i++)
		{
			scale_x[i]	= SWAP32(scale_x[i]);
			scale_y[i]	= SWAP32(scale_y[i]);
			memcpy(pNext, &scale_x[i], 4);	pNext	+= 4;
			memcpy(pNext, &scale_y[i], 4);	pNext	+= 4;
		}
		u32Body	= UINT32(pNext - pBody);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPP::en_local_fixed_scaling,
							  pPktNext, u32Pkts, pBody, u32Body);
		delete pBody;
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPP::en_local_fixed_scaling,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Has effect in fixed transformation mode only. Fixed offset to be used. N <= 200
 parm : flag	- [in]  Function Type : Get or Set
		count	- [in]  Number of local zones (N).
		offset_x- [in]  Local fixed x-offset. factor (unit: nm). Ignored for TransformationReceipe 0.
		offset_x- [in]  Local fixed y-offset. factor (unit: nm). Ignored for TransformationReceipe 0.
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CPanelPreparation::GetPktLocalFixedOffset(ENG_LPGS flag, UINT16 count,
												 PINT32 offset_x, PINT32 offset_y)
{
	PUINT8 pBody	= NULL, pNext;
	UINT16 u16Count	= SWAP16(count), i = 0;
	UINT32 u32Pkts	= 8192, u32Body = 0;
	PUINT8 pPktBuff	= NULL, pPktNext;

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
		pBody = new UINT8[sizeof(INT32) * count + 8];// (PUINT8)::Alloc(sizeof(INT32) * count + 8);
		memset(pBody, 0x00, sizeof(INT32)*count+8);
		pNext	= pBody;
		memcpy(pNext, &u16Count, 2);	pNext	+= 2;
		for (; i<count; i++)
		{
			offset_x[i]	= SWAP32(offset_x[i]);
			offset_y[i]	= SWAP32(offset_y[i]);
			memcpy(pNext, &offset_x[i], 4);	pNext	+= 4;
			memcpy(pNext, &offset_y[i], 4);	pNext	+= 4;
		}
		u32Body	= UINT32(pNext - pBody);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPP::en_local_fixed_offset,
							  pPktNext, u32Pkts, pBody, u32Body);
		delete pBody;
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPP::en_local_fixed_offset,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : When global auto transformation recipe is selected,
		it is possible to force the registration points to make a rectangle
 parm : flag	- [in]  Get (Read) or Set (Write)
		lock	- [in]  1 = Force global rectangle lock, 0 = Do not force global rectangle lock
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CPanelPreparation::GetPktGlobalRectangleLock(ENG_LPGS flag, UINT8 lock)
{
	UINT8 u8Body[8] = { lock, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPP::en_global_rectangle_lock,
							  pPktNext, u32Pkts, u8Body, 1);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPP::en_global_rectangle_lock,
							  pPktNext, u32Pkts-m_u32PktSize, u8Body, 1);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Will remove the panel data that is assigned to the given d-code
 parm : dcode	- [in]  D-Code
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CPanelPreparation::GetPktRemovePanelData(UINT32 dcode)
{
	UINT8 u8Body[8] = { NULL };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */
	dcode	= SWAP32(dcode);
	memcpy(u8Body, &dcode, 4);

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPP::en_remove_panel_data,
						  pPktNext, u32Pkts, u8Body, 4);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : If warping is out of system limits, the command GetRegistrationStatus will return;
		WarpOutsideLimitsInZone or WarpOutsideLimitsOutsideZone
		In this case it is stell possible to performe print, but the printed result will contain errors
		in some areas of the panel
		This command, GetWarpOfLimitsCoordinates may be used to get coordinates and the corresponding error,
		where the panel print is outside limits
		This may be used to decide whether position and severity of the error is acceptable or not
		If acceptable the print may continue
		It is possible to continued print if the error is not severe
		A typical case where WarpOustideLimitsOutsideZone, or when only a small part of the panel is damaged
		Max reported error coordinates is 400
 parm : None
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CPanelPreparation::GetPktGetWarpOfLimitsCoordinates()
{
	UINT32 u32Pkts	= 128, u32Body = 0;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPP::en_get_warp_of_limits_coordinates,
						  pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : When this mode is enabled, the artwork is always warped to the closest zone registration setting
		This mode can only be used in combination with non shared local zones
		And the zones have to be distributed in a grid parallel to x and y axis (not scattered)
		Recommended to use only one zone in y direction.
 parm : flag	- [in]  Get (Read) or Set (Write)
		mode	- [in]  0 = Disable, 1 = Enable
 retn : Pointer where the packet buffer is stored
 note : Valid from Luria version 2.12.0 and 3.0.0, and newer
*/
PUINT8 CPanelPreparation::GetPktSnapToZoneMode(ENG_LPGS flag, UINT8 mode)
{
	UINT8 u8Body[8] = { mode, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!(ValidVersion(2, 12, 0, 0x02) || ValidVersion(3, 0, 0, 0x00)))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCPP::en_snap_to_zone_mode,
							  pPktNext, u32Pkts, u8Body, 1);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCPP::en_snap_to_zone_mode,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Set the band width for searching fiducials in a zone
		Nominal values are typically below 1mm to allow strictly rectangular zones only
		This interface may be used to increase the search limit to allow for non square zones
		Values should be set as low as possible to avoid risk of confusing fiducials between zones
 parm : flag	- [in]  Function Type : Get or Set
		bw_x	- [in]  X-Search band width. (unit: um)
		bw_y	- [in]  Y-Search band width. (unit: um)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CPanelPreparation::GetPktLocalZoneFidSearchBw(ENG_LPGS flag, UINT32 bw_x, UINT32 bw_y)
{
	UINT8 u8Body[32]= { NULL }, *pBody = u8Body;
	UINT32 u32Pkts	= 128, u32Body = 0;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!(ValidVersion(2, 13, 0, 0x02) || ValidVersion(3, 3, 0, 0x00)))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* 기본 패킷 구성 */
	if (ENG_LPGS::en_set == flag)
	{
		/* Set the send packet */
		bw_x	= SWAP32(bw_x);
		bw_y	= SWAP32(bw_y);
		memcpy(pBody, &bw_x, 4);	pBody	+= 4;
		memcpy(pBody, &bw_y, 4);	pBody	+= 4;
		u32Body	= UINT32(pBody - u8Body);

		pPktBuff	= GetPktBase(ENG_LTCT::en_write, ENG_LCPP::en_local_zone_fid_search_bw,
								 pPktNext, u32Pkts, u8Body, u32Body);
	}
	if (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag)
	{
		pPktBuff	= GetPktBase(ENG_LTCT::en_read, ENG_LCPP::en_local_zone_fid_search_bw,
								 pPktNext, u32Pkts-m_u32PktSize);
	}

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
VOID CPanelPreparation::SetRecvPacket(UINT8 uid, PUINT8 data, UINT32 size)
{
	switch (uid)
	{
	case (UINT8)ENG_LCPP::en_use_shared_local_zones			:
	case (UINT8)ENG_LCPP::en_local_transformation_recipe	:
	case (UINT8)ENG_LCPP::en_global_rectangle_lock			:
	case (UINT8)ENG_LCPP::en_snap_to_zone_mode				:	SetRecvPanel1Bytes(uid, data, size);	break;
	case (UINT8)ENG_LCPP::en_get_number_of_local_zones		:
	case (UINT8)ENG_LCPP::en_get_registration_status		:	SetRecvPanel2Bytes(uid, data, size);	break;
	case (UINT8)ENG_LCPP::en_global_fixed_rotation			:
	case (UINT8)ENG_LCPP::en_local_zone_geometry			:	SetRecvPanel4Bytes(uid, data, size);	break;
	case (UINT8)ENG_LCPP::en_global_fixed_scaling			:
	case (UINT8)ENG_LCPP::en_global_fixed_offset			:	SetRecvPanel8Bytes(uid, data, size);	break;
	case (UINT8)ENG_LCPP::en_dynamic_fiducials				:
	case (UINT8)ENG_LCPP::en_registration_points			:
	case (UINT8)ENG_LCPP::en_global_transformation_receipe	:
	case (UINT8)ENG_LCPP::en_get_transformation_params		:
	case (UINT8)ENG_LCPP::en_panel_data_serial_number		:
	case (UINT8)ENG_LCPP::en_panel_data						:
	case (UINT8)ENG_LCPP::en_local_fixed_rotation			:
	case (UINT8)ENG_LCPP::en_local_fixed_scaling			:
	case (UINT8)ENG_LCPP::en_get_warp_of_limits_coordinates	:
	case (UINT8)ENG_LCPP::en_local_fixed_offset				:	SetRecvPanelEtcBytes(uid, data, size);	break;
	default:	return;
	}
}

/*
 desc : 1 Bytes 수신 데이터 처리
 parm : uid		- [in]  서브 명령어
		data	- [in]  수신된 데이터
		size	- [in]  'data' 크기
 retn : None
*/
VOID CPanelPreparation::SetRecvPanel1Bytes(UINT8 uid, PUINT8 data, UINT32 size)
{
	switch (uid)
	{
	case (UINT8)ENG_LCPP::en_use_shared_local_zones			:	m_pstPanel->use_shared_local_zones		= data[0];	break;
	case (UINT8)ENG_LCPP::en_local_transformation_recipe	:	m_pstPanel->local_transformation_recipe	= data[0];	break;
	case (UINT8)ENG_LCPP::en_global_rectangle_lock			:	m_pstPanel->global_rectangle_lock		= data[0];	break;
	case (UINT8)ENG_LCPP::en_snap_to_zone_mode				:	m_pstPanel->snap_to_zone_mode			= data[0];	break;
	}
}

/*
 desc : 2 Bytes 수신 데이터 처리
 parm : uid		- [in]  서브 명령어
		data	- [in]  수신된 데이터
		size	- [in]  'data' 크기
 retn : None
*/
VOID CPanelPreparation::SetRecvPanel2Bytes(UINT8 uid, PUINT8 data, UINT32 size)
{
	UINT16 u16Data	= 0;

	switch (uid)
	{
	case (UINT8)ENG_LCPP::en_get_number_of_local_zones	:
		memcpy(&u16Data, data, 2);
		m_pstPanel->get_number_of_local_zones	= SWAP16(u16Data);	break;
	case (UINT8)ENG_LCPP::en_get_registration_status	:
		memcpy(&u16Data, data, 2);
		m_pstPanel->get_registration_status		= SWAP16(u16Data);
		SaveRegistrationStatus(m_pstPanel->get_registration_status);
		break;
	}
}

/*
 desc : 4 Bytes 수신 데이터 처리
 parm : uid		- [in]  서브 명령어
		data	- [in]  수신된 데이터
		size	- [in]  'data' 크기
 retn : None
*/
VOID CPanelPreparation::SetRecvPanel4Bytes(UINT8 uid, PUINT8 data, UINT32 size)
{
	UINT16 u16Data	= 0;
	UINT32 u32Data	= 0;
	PUINT8 pData	= data;

	switch (uid)
	{
	case (UINT8)ENG_LCPP::en_global_fixed_rotation	:
		memcpy(&u32Data, data, 4);
		m_pstPanel->global_fixed_rotation	= SWAP32(u32Data);		break;
	case (UINT8)ENG_LCPP::en_local_zone_geometry	:
		memcpy(&u16Data, pData, 2);	pData	+= 2;
		m_pstPanel->local_zone_geometry_xy[0]	= SWAP16(u16Data);
		memcpy(&u16Data, pData, 2);	pData	+= 2;
		m_pstPanel->local_zone_geometry_xy[1]	= SWAP16(u16Data);	break;
	}
}

/*
 desc : 8 Bytes 수신 데이터 처리
 parm : uid		- [in]  서브 명령어
		data	- [in]  수신된 데이터
		size	- [in]  'data' 크기
 retn : None
*/
VOID CPanelPreparation::SetRecvPanel8Bytes(UINT8 uid, PUINT8 data, UINT32 size)
{
	UINT32 u32Data	= 0;
	PUINT8 pData	= data;

	switch (uid)
	{
	case (UINT8)ENG_LCPP::en_global_fixed_scaling	:
		memcpy(&u32Data, pData, 4);	pData	+= 4;
		m_pstPanel->global_fixed_scaling_xy[0]	= SWAP32(u32Data);
		memcpy(&u32Data, pData, 4);	pData	+= 4;
		m_pstPanel->global_fixed_scaling_xy[1]	= SWAP32(u32Data);	break;
	case (UINT8)ENG_LCPP::en_global_fixed_offset	:
		memcpy(&u32Data, pData, 4);	pData	+= 4;
		m_pstPanel->global_fixed_offset_xy[0]	= SWAP32(u32Data);
		memcpy(&u32Data, pData, 4);	pData	+= 4;
		m_pstPanel->global_fixed_offset_xy[1]	= SWAP32(u32Data);	break;
	}
}

/*
 desc : xx Bytes 수신 데이터 처리
 parm : uid		- [in]  서브 명령어
		data	- [in]  수신된 데이터
		size	- [in]  'data' 버퍼의 크기
 retn : None
*/
VOID CPanelPreparation::SetRecvPanelEtcBytes(UINT8 uid, PUINT8 data, UINT32 size)
{
	switch (uid)
	{
	case (UINT8)ENG_LCPP::en_dynamic_fiducials				:	SetRecvDynamicFiducials(data, size);						break;
	case (UINT8)ENG_LCPP::en_registration_points			:	SetRecvRegistrationPoints(data, size);						break;
	case (UINT8)ENG_LCPP::en_global_transformation_receipe	:	memcpy(m_pstPanel->global_transformation_recipe, data, 3);	break;
	case (UINT8)ENG_LCPP::en_get_transformation_params		:	SetRecvTransformationParams(data, size);					break;
	case (UINT8)ENG_LCPP::en_panel_data_serial_number		:	SetRecvPanelDataSerialNumber(data, size);					break;
	case (UINT8)ENG_LCPP::en_panel_data						:	SetRecvPanelData(data, size);								break;
	case (UINT8)ENG_LCPP::en_local_fixed_rotation			:	SetRecvLocalFixedRotation(data, size);						break;
	case (UINT8)ENG_LCPP::en_local_fixed_scaling			:	SetRecvLocalFixedScaling(data, size);						break;
	case (UINT8)ENG_LCPP::en_local_fixed_offset				:	SetRecvLocalFixedOffset(data, size);						break;
	case (UINT8)ENG_LCPP::en_get_warp_of_limits_coordinates	:	SetRecvGetWarpOfLimitsCoordinates(data, size);				break;
	}
}

/*
 desc : Dynamic.Fiducials 수신 데이터 처리
 parm : uid		- [in]  서브 명령어
		data	- [in]  수신된 데이터
		size	- [in]  'data' 버퍼의 크기
 retn : None
*/
VOID CPanelPreparation::SetRecvDynamicFiducials(PUINT8 data, UINT32 size)
{
	UINT8 u8Type		= 0x00;
	UINT32 u32Size		= size;
	PUINT8 pData		= data;

	LPG_PPDG pstGlobal	= NULL;
	LPG_PPDL pstLocal	= NULL;

	/* Type (Global:0, Local:1) */
	u8Type	= pData[0];	pData	+= 1;	u32Size	-= 1;

	if (0x00 == u8Type)
	{
		/* 임시 저장 구조체 메모리 할당 */
		pstGlobal = new STG_PPDG();// (LPG_PPDG)::Alloc(sizeof(STG_PPDG));
		ASSERT(pstGlobal);
		memcpy(pstGlobal, pData, u32Size);
		pstGlobal->SWAP();

		/* 공유 메모리 영역에 복사 */
		memcpy(&m_pstPanel->fiducial_global, pstGlobal, u32Size);
	}
	else
	{
		/* 임시 저장 구조체 메모리 할당 */
		pstLocal = new STG_PPDL();//(LPG_PPDL)::Alloc(sizeof(STG_PPDL));
		ASSERT(pstLocal);
		memcpy(pstLocal, pData, u32Size);
		pstLocal->SWAP();

		/* 공유 메모리 영역에 복사 */
		memcpy(&m_pstPanel->fiducial_local, pstLocal, u32Size);
	}

	if (pstGlobal)	delete pstGlobal;
	if (pstLocal)	delete pstLocal;
}

/*
 desc : Registration.Points 수신 데이터 처리
 parm : data	- [in]  수신된 데이터
		size	- [in]  'data' 버퍼의 크기
 retn : None
*/
VOID CPanelPreparation::SetRecvRegistrationPoints(PUINT8 data, UINT32 size)
{
	UINT32 u32Size		= size;
	PUINT8 pData		= data;
	LPG_PPRP pstRegist	= NULL;

	/* 임시 저장 구조체 메모리 할당 */
	pstRegist = new STG_PPRP();// (LPG_PPRP)::Alloc(sizeof(STG_PPRP));
	ASSERT(pstRegist);
	memcpy(pstRegist, pData, u32Size);
	pstRegist->SWAP();

	/* 공유 메모리 영역에 복사 */
	memcpy(&m_pstPanel->registration_points, pstRegist, u32Size);

	if (pstRegist)	delete pstRegist;
}

/*
 desc : Get.Transformation.Params 수신 데이터 처리
 parm : data	- [in]  수신된 데이터
		size	- [in]  'data' 버퍼의 크기
 retn : None
*/
VOID CPanelPreparation::SetRecvTransformationParams(PUINT8 data, UINT32 size)
{
	UINT32 u32Size		= size;
	PUINT8 pData		= data;
	LPG_PPTP pstParam	= NULL;

	/* 임시 저장 구조체 메모리 할당 */
	pstParam = new STG_PPTP();// (LPG_PPTP)::Alloc(sizeof(STG_PPTP));
	ASSERT(pstParam);
	memcpy(pstParam, pData, u32Size);
	pstParam->SWAP();

	/* 공유 메모리 영역에 복사 */
	memcpy(&m_pstPanel->get_transformation_params, pstParam, u32Size);

	/* 임시 메모리 해제 */
	delete pstParam;
}

/*
 desc : Panel.Data.Serial.Number 수신 데이터 처리
 parm : data	- [in]  수신된 데이터
		size	- [in]  'data' 버퍼의 크기
 retn : None
*/
VOID CPanelPreparation::SetRecvPanelDataSerialNumber(PUINT8 data, UINT32 size)
{
	UINT32 u32Size		= size;
	PUINT8 pData		= data;
	LPG_PPSN pstSerial	= NULL;

	/* 임시 저장 구조체 메모리 할당 */
	pstSerial = new STG_PPSN(); // (LPG_PPSN)::Alloc(sizeof(STG_PPSN));
	ASSERT(pstSerial);
	memcpy(pstSerial, pData, u32Size);
	pstSerial->SWAP();

	/* 공유 메모리 영역에 복사 */
	memcpy(&m_pstPanel->panel_data_serial_number, pstSerial, u32Size);

	/* 임시 메모리 해제 */
	delete pstSerial;
}

/*
 desc : Panel.Data 수신 데이터 처리
 parm : data	- [in]  수신된 데이터
		size	- [in]  'data' 버퍼의 크기
 retn : None
*/
VOID CPanelPreparation::SetRecvPanelData(PUINT8 data, UINT32 size)
{
	UINT32 u32Size		= size;
	PUINT8 pData		= data;
	LPG_PPPD pstPanel	= NULL;

	/* 임시 저장 구조체 메모리 할당 */
	pstPanel = new STG_PPPD();// (LPG_PPPD)::Alloc(sizeof(STG_PPPD));
	ASSERT(pstPanel);
	memcpy(pstPanel, pData, u32Size);
	pstPanel->SWAP();

	/* 공유 메모리 영역에 복사 */
	memcpy(&m_pstPanel->panel_data, pstPanel, u32Size);

	/* 임시 메모리 해제 */
	delete pstPanel;
}

/*
 desc : Local.Fixed.Rotation 수신 데이터 처리
 parm : data	- [in]  수신된 데이터
		size	- [in]  'data' 버퍼의 크기
 retn : None
*/
VOID CPanelPreparation::SetRecvLocalFixedRotation(PUINT8 data, UINT32 size)
{
	UINT16 i, u16Data	= 0;
	UINT32 u32Size		= size;
	PUINT8 pData		= data;
	PINT32 pRotation	= NULL;

	memcpy(&u16Data, pData, 2);	pData	+= 2;	u32Size	-= 2;
	m_pstPanel->local_fixed_rotation_cnt	= SWAP16(u16Data);

	/* 임시 메모리 할당 */
	pRotation = new INT32[m_pstPanel->local_fixed_rotation_cnt];//  (PINT32)::Alloc(sizeof(INT32) * m_pstPanel->local_fixed_rotation_cnt);
	ASSERT(pRotation);
	memcpy(pRotation, pData, u32Size);

	/* SWAP */
	for (i=0; i<m_pstPanel->local_fixed_rotation_cnt; i++)
	{
		pRotation[i]	= SWAP32(pRotation[i]);
	}

	/* 공유 메모리에 복사 */
	memcpy(&m_pstPanel->local_fixed_rotation, pRotation, u32Size);

	/* 임시 메모리 해제 */
	delete pRotation;
}

/*
 desc : Local.Fixed.Scaling 수신 데이터 처리
 parm : data	- [in]  수신된 데이터
		size	- [in]  'data' 버퍼의 크기
 retn : None
*/
VOID CPanelPreparation::SetRecvLocalFixedScaling(PUINT8 data, UINT32 size)
{
	UINT16 i, u16Data	= 0;
	UINT32 u32Size		= size;
	PUINT8 pData		= data;
	LPG_U4XY pstScaling	= NULL;

	memcpy(&u16Data, pData, 2);	pData	+= 2;	u32Size	-= 2;
	m_pstPanel->local_fixed_scaling_cnt	= SWAP16(u16Data);

	/* 임시 메모리 할당 */
	pstScaling = new  STG_U4XY[m_pstPanel->local_fixed_scaling_cnt];// (LPG_U4XY)::Alloc(sizeof(STG_U4XY) * m_pstPanel->local_fixed_scaling_cnt);
	ASSERT(pstScaling);
	memcpy(pstScaling, pData, u32Size);

	/* SWAP */
	for (i=0; i<m_pstPanel->local_fixed_scaling_cnt; i++)
	{
		pstScaling[i].SWAP();
	}

	/* 공유 메모리에 복사 */
	memcpy(&m_pstPanel->local_fixed_scaling, pstScaling, u32Size);

	/* 임시 메모리 해제 */
	delete pstScaling;
}

/*
 desc : Processing received data - LocalFixedOffset
 parm : data	- [in]  packet buffer
		size	- [in]  The size of'data'
 retn : None
*/
VOID CPanelPreparation::SetRecvLocalFixedOffset(PUINT8 data, UINT32 size)
{
	UINT16 i, u16Data	= 0;
	UINT32 u32Size		= size;
	PUINT8 pData		= data;
	LPG_I4XY pstOffset	= NULL;

	memcpy(&u16Data, pData, 2);	pData	+= 2;	u32Size	-= 2;
	m_pstPanel->local_fixed_offset_cnt	= SWAP16(u16Data);

	/* Allocate the memory */
	pstOffset = new STG_I4XY[m_pstPanel->local_fixed_offset_cnt];//(LPG_I4XY)::Alloc(sizeof(STG_I4XY) * m_pstPanel->local_fixed_offset_cnt);
	ASSERT(pstOffset);
	memcpy(pstOffset, pData, u32Size);
	/* SWAP */
	for (i=0; i<m_pstPanel->local_fixed_offset_cnt; i++)	pstOffset[i].SWAP();
	/* Copy to shared memory */
	memcpy(&m_pstPanel->local_fixed_offset, pstOffset, u32Size);
	/* Release the memory */
	delete pstOffset;
}

/*
 desc : Processing received data - GetWarpOfLimitsCoordinates
 parm : data	- [in]  packet buffer
		size	- [in]  The size of'data'
 retn : None
*/
VOID CPanelPreparation::SetRecvGetWarpOfLimitsCoordinates(PUINT8 data, UINT32 size)
{
	UINT16 i, u16Data	= 0;
	UINT32 u32Size		= size;
	PUINT8 pData		= data;
	LPG_PPWL pstCoord	= NULL;

	memcpy(&u16Data, pData, 2);	
	pData	+= 2;	
	u32Size	-= 2;
	m_pstPanel->warp_limit_coord_xy_cnt	= SWAP16(u16Data);

	/* Allocate the memory */
	pstCoord = new STG_PPWL[m_pstPanel->warp_limit_coord_xy_cnt];// (LPG_PPWL)::Alloc(sizeof(STG_PPWL) * m_pstPanel->warp_limit_coord_xy_cnt);
	ASSERT(pstCoord);
	memcpy(pstCoord, pData, u32Size);
	/* SWAP */
	for (i=0; i<m_pstPanel->warp_limit_coord_xy_cnt; i++)	pstCoord[i].SWAP();
	/* Copy to shared memory */
	memcpy(&m_pstPanel->warp_limits_coord_xy, pstCoord, u32Size);
	/* Release the memory */
	delete pstCoord;
}

/*
 desc : Save the status of registration as a string
 parm : status	- [in]  status code
 retn : None
*/
VOID CPanelPreparation::SaveRegistrationStatus(UINT16 status)
{
	if (1 == status)	return;

	TCHAR tzMesg[256]	= {NULL};
	GetRegistrationStatus(status, tzMesg, 256);
	if (wcslen(tzMesg) > 0)	LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
}

/*
 desc : Returns the status of registration
 parm : status	- [in]  status code
		mesg	- [out] The buffer in which the string of the code value will be stored
		size	- [in]  The size of 'mesg' buffer
 retn : None
*/
VOID CPanelPreparation::GetRegistrationStatus(UINT16 status, PTCHAR mesg, UINT32 size)
{
	switch (status)
	{
	case 0	:	swprintf_s(mesg, size, L"status[%02u]: Not Ready",									status);	break;
	case 1	:	swprintf_s(mesg, size, L"status[%02u]: Ok",											status);	break;
	case 2	:	swprintf_s(mesg, size, L"status[%02u]: MissingFiducials",							status);	break;
	case 3	:	swprintf_s(mesg, size, L"status[%02u]: MissingRegistration",						status);	break;
	case 4	:	swprintf_s(mesg, size, L"status[%02u]: GenerateNotRun",								status);	break;
	case 5	:	swprintf_s(mesg, size, L"status[%02u]: StripHeightNotSet",							status);	break;
	case 6	:	swprintf_s(mesg, size, L"status[%02u]: StripWidthNotSet",							status);	break;
	case 7	:	swprintf_s(mesg, size, L"status[%02u]: NumberOfStripesNotSet",						status);	break;
	case 8	:	swprintf_s(mesg, size, L"status[%02u]: OverlapNotSet",								status);	break;
	case 9	:	swprintf_s(mesg, size, L"status[%02u]: ScalingPointDistanceNotSet",					status);	break;
	case 10	:	swprintf_s(mesg, size, L"status[%02u]: CornerShiftsOutsideLimits",					status);	break;
	case 11	:	swprintf_s(mesg, size, L"status[%02u]: NumberOfRegAndFidNotEqual",					status);	break;
	case 12	:	swprintf_s(mesg, size, L"status[%02u]: UnableToInterpretFiducials",					status);	break;
	case 13	:	swprintf_s(mesg, size, L"status[%02u]: UnableToInterpretRegistrations",				status);	break;
	case 14	:	swprintf_s(mesg, size, L"status[%02u]: InvalidNumberOfScalingPoints",				status);	break;
	case 15	:	swprintf_s(mesg, size, L"status[%02u]: NumberOfStripesMatrixSizeMismatch",			status);	break;
	case 16	:	swprintf_s(mesg, size, L"status[%02u]: MissingOffsets",								status);	break;
	case 17	:	swprintf_s(mesg, size, L"status[%02u]: MissingStripFlipList",						status);	break;
	case 18	:	swprintf_s(mesg, size, L"status[%02u]: MissingXBaseStepSize",						status);	break;
	case 19	:	swprintf_s(mesg, size, L"status[%02u]: RotationOutsideLimits",						status);	break;
	case 20	:	swprintf_s(mesg, size, L"status[%02u]: ScalingOutsideLimits",						status);	break;
	case 21	:	swprintf_s(mesg, size, L"status[%02u]: OffsetOutsideLimits",						status);	break;
	case 22	:	swprintf_s(mesg, size, L"status[%02u]: NumberOfFixedParamIncorrect",				status);	break;
	case 23	:	swprintf_s(mesg, size, L"status[%02u]: CalcRotationOutOfLimits",					status);	break;
	case 24	:	swprintf_s(mesg, size, L"status[%02u]: CalcScaleOutOfLimits",						status);	break;
	case 25	:	swprintf_s(mesg, size, L"status[%02u]: CalcOffsetOutOfLimits",						status);	break;
	case 26	:	swprintf_s(mesg, size, L"status[%02u]: FixedRotationParamOutOfLimits",				status);	break;
	case 27	:	swprintf_s(mesg, size, L"status[%02u]: FixedScaleParamOutOfLimits",					status);	break;
	case 28	:	swprintf_s(mesg, size, L"status[%02u]: FixedOffsetParamOutOfLimits",				status);	break;
	case 29	:	swprintf_s(mesg, size, L"status[%02u]: WarpOutsideLimitsInZone",					status);	break;
	case 30	:	swprintf_s(mesg, size, L"status[%02u]: WarpOutsideLimitsOutsideZone",				status);	break;
	case 31	:	swprintf_s(mesg, size, L"status[%02u]: WarpUnableToCalculateWarpZone",				status);	break;
	case 32	:	swprintf_s(mesg, size, L"status[%02u]: WarpUndefinedLocalZonesFixedParamNotAllowed",status);	break;
	case 33	:	swprintf_s(mesg, size, L"status[%02u]: WarpFixedParamNotAllowedWithSharedZones",	status);	break;
	case 34	:	swprintf_s(mesg, size, L"status[%02u]: WarpOutisdeFpgaLimits",						status);	break;
	case 99	:	swprintf_s(mesg, size, L"status[%02u]: InternalError",								status);	break;
	default	:	swprintf_s(mesg, size, L"status[%02u]: Unknown Error",								status);	break;
	}
}
