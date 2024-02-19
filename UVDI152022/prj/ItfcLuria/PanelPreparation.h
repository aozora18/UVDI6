
#pragma once

#include "BaseFamily.h"

class CPanelPreparation  : public CBaseFamily
{
/* 생성자 & 파괴자 */
public:

	CPanelPreparation(LPG_LDSM shmem, CCodeToStr *error);
	virtual ~CPanelPreparation();


/* 가상 함수 */
protected:

public:

	virtual VOID		SetRecvPacket(UINT8 cmd, PUINT8 data, UINT32 size);


/* 로컬 변수 */
protected:

	LPG_LDPP			m_pstPanel;


/* 로컬 함수 */
protected:

	VOID				SetRecvPanel1Bytes(UINT8 uid, PUINT8 data, UINT32 size);
	VOID				SetRecvPanel2Bytes(UINT8 uid, PUINT8 data, UINT32 size);
	VOID				SetRecvPanel4Bytes(UINT8 uid, PUINT8 data, UINT32 size);
	VOID				SetRecvPanel8Bytes(UINT8 uid, PUINT8 data, UINT32 size);
	VOID				SetRecvPanelEtcBytes(UINT8 uid, PUINT8 data, UINT32 size);

	VOID				SetRecvDynamicFiducials(PUINT8 data, UINT32 size);
	VOID				SetRecvRegistrationPoints(PUINT8 data, UINT32 size);
	VOID				SetRecvTransformationParams(PUINT8 data, UINT32 size);
	VOID				SetRecvPanelDataSerialNumber(PUINT8 data, UINT32 size);
	VOID				SetRecvPanelData(PUINT8 data, UINT32 size);
	VOID				SetRecvLocalFixedRotation(PUINT8 data, UINT32 size);
	VOID				SetRecvLocalFixedScaling(PUINT8 data, UINT32 size);
	VOID				SetRecvLocalFixedOffset(PUINT8 data, UINT32 size);
	VOID				SetRecvGetWarpOfLimitsCoordinates(PUINT8 data, UINT32 size);

	VOID				SaveRegistrationStatus(UINT16 status);


/* 공용 함수 */
public:

	PUINT8				GetPktDynamicFiducials(ENG_LPGS flag, UINT8 type,
											   UINT16 count=0, INT32 **fidxy=NULL);						/* uid = 0x01 */
	PUINT8				GetPktRegistrationPoints(ENG_LPGS flag,
												 UINT16 count=0, LPG_I32XY fidxy=NULL);					/* uid = 0x02 */
	PUINT8				GetPktGlobalTransformationRecipe(ENG_LPGS flag,
														 UINT8 rotation=0x00,
														 UINT8 scaling=0x00,
														 UINT8 offset=0x00);									/* uid = 0x03 */
	PUINT8				GetPktGlobalFixedRotation(ENG_LPGS flag, INT32 rotation=0);						/* uid = 0x04 */
	PUINT8				GetPktGlobalFixedScaling(ENG_LPGS flag,
												 UINT32 scale_x = 0, UINT32 scale_y = 0);				/* uid = 0x05 */
	PUINT8				GetPktGlobalFixedOffset(ENG_LPGS flag,
												INT32 offset_x = 0, INT32 offset_y = 0);				/* uid = 0x06 */
	PUINT8				GetPktLocalZoneGeometry(ENG_LPGS flag, UINT16 fid_x=0, UINT16 fid_y=0);			/* uid = 0x07 */
	PUINT8				GetPktGetNumberOfLocalZones();													/* uid = 0x08 */
	PUINT8				GetPktGetTransformationParams(UINT16 zone);										/* uid = 0x09 */
	PUINT8				GetPktRunRegistration();														/* uid = 0x0b */
	PUINT8				GetPktGetRegistrationStatus();													/* uid = 0x0c */
	PUINT8				GetPktSerialNumber(ENG_LPGS flag,
										   UINT32 dcode, UINT8 symbol=0,
										   UINT16 s_len=0, PUINT8 serial=NULL, UINT32 start=0,
										   UINT8 flip_h=0, UINT8 flip_v=0,
										   UINT16 font_h=0, UINT16 font_v=0);							/* uid = 0x0d */
	PUINT8				GetPktPanelData(ENG_LPGS flag,
										UINT32 dcode, UINT16 s_len=0, PUINT8 text=NULL,
										UINT8 flip_x=0, UINT8 flip_y=0,
										UINT16 font_h=0, UINT16 font_v=0);								/* uid = 0x0e */
	PUINT8				GetPktUseSharedLocalZones(ENG_LPGS flag, UINT8 use=0);							/* uid = 0x0f */
	PUINT8				GetPktLocalTransformationRecipe(ENG_LPGS flag, UINT8 info=0);					/* uid = 0x10 */
	PUINT8				GetPktLocalFixedRotation(ENG_LPGS flag, UINT16 count=0, PINT32 rotation=NULL);	/* uid = 0x11 */
	PUINT8				GetPktLocalFixedScaling(ENG_LPGS flag, UINT16 count=0,
												PUINT32 scale_x=NULL, PUINT32 scale_y=NULL);			/* uid = 0x12 */
	PUINT8				GetPktLocalFixedOffset(ENG_LPGS flag, UINT16 count=0,
											   PINT32 offset_x=NULL, PINT32 offset_y=NULL);				/* uid = 0x13 */
	PUINT8				GetPktGlobalRectangleLock(ENG_LPGS flag, UINT8 lock=0);							/* uid = 0x14 */
	PUINT8				GetPktRemovePanelData(UINT32 dcode);											/* uid = 0x15 */
	PUINT8				GetPktGetWarpOfLimitsCoordinates();												/* uid = 0x16 */
	PUINT8				GetPktSnapToZoneMode(ENG_LPGS flag, UINT8 mode=0);								/* uid = 0x17 */
	PUINT8				GetPktLocalZoneFidSearchBw(ENG_LPGS flag, UINT32 bw_x=0, UINT32 bw_y=0);		/* uid = 0x18 */

	VOID				GetRegistrationStatus(UINT16 status, PTCHAR mesg, UINT32 size);
};