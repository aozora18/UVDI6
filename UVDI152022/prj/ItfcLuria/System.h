
#pragma once

#include "BaseFamily.h"

class CSystem : public CBaseFamily
{
/* ������ & �ı��� */
public:

	CSystem(LPG_LDSM shmem, CCodeToStr *error);
	virtual ~CSystem();


/* ���� �Լ� */
protected:

public:

	virtual VOID		SetRecvPacket(UINT8 cmd, PUINT8 data, UINT32 size);


/* ���� ���� */
protected:

	LPG_LDSS			m_pstSystem;


/* ���� �Լ� */
protected:

	VOID				SetRecvSystemSystemStatus(PUINT8 data, UINT32 size);
	VOID				SetRecvSmartData(PUINT8 data, UINT32 size);
	VOID				SetRecvMainboardTemp(PUINT8 data, UINT32 size);
	VOID				SetRecvOverallErrorStatusMulti(PUINT8 data, UINT32 size);
	VOID				SetRecvSetLightIntensity(PUINT8 data, UINT32 size);
	VOID				SetRecvLoadInternalTestImage(PUINT8 data, UINT32 size);

	VOID				SaveMotorStatus(UINT32 status);
	VOID				SavePotoHeadStatus(UINT32 status);
	VOID				SaveLuriaPCStatus(UINT32 status);
	VOID				SaveOverallErrorStatusMulti(UINT8 ph_no, UINT32 status);


/* ���� �Լ� */
public:

	PUINT8				GetPktInitializeHardware();												/* uid : 0x01 */
	PUINT8				GetPktSystemStatus();													/* uid : 0x02 */
	PUINT8				GetPktSystemShutdown();													/* uid : 0x03 */
	PUINT8				GetPktUpgradeFirmware(UINT16 length, PCHAR file);						/* uid : 0x05 */
	PUINT8				GetPktLoadInternalTestImage(UINT8 ph_no, UINT8 img_no);					/* uid : 0x07 */
	PUINT8				GetPktSetLightIntensity(UINT8 ph_no, ENG_LLPI led_no, UINT16 ampl);		/* uid : 0x08 */
	PUINT8				GetPktMainboardTemperature();											/* uid : 0x09 */
	PUINT8				GetPktOverallErrorStatusMulti();										/* uid : 0x0a */
};