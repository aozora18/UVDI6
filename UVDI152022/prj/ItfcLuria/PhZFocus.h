
#pragma once

#include "BaseFamily.h"

class CPhZFocus : public CBaseFamily
{
/* Constructor & Destructor */
public:

	CPhZFocus(LPG_LDSM shmem, CCodeToStr *error);
	virtual ~CPhZFocus();


/* Virtual Function */
protected:

public:

	virtual VOID		SetRecvPacket(UINT8 cmd, PUINT8 data, UINT32 size);


/* Local Member Parameter */
protected:

	LPG_LDPF			m_pstShMemPF;
	LPG_LDDP			m_pstShMemDP;

/* Local Member Function */
protected:

	VOID				SetRecv2Bytes(UINT8 uid, PUINT8 data);
	VOID				SetRecv4Bytes(UINT8 uid, PUINT8 data);
	VOID				SetRecv5Bytes(UINT8 uid, PUINT8 data);
	VOID				SetRecv9Bytes(UINT8 uid, PUINT8 data);
	VOID				SetRecvEtcBytes(UINT8 uid, PUINT8 data);


/* Public Member Function */
public:
	PUINT8				GetPktInitializeFocus();																/* uid = 0x01 */
	PUINT8				GetPktPosition(ENG_LPGS flag, UINT8 ph_no, INT32 ph_pos=0);								/* uid = 0x02 */
	PUINT8				GetPktAutofocus(ENG_LPGS flag, UINT8 ph_no, UINT8 enable=0);							/* uid = 0x03 */
	PUINT8				GetPktTrim(ENG_LPGS flag, UINT8 ph_no, INT32 trim=0);									/* uid = 0x04 */
	PUINT8				GetPktActiveAreaQualifier(ENG_LPGS flag, UINT8 enable=0, UINT32 start=0, UINT32 end=0);	/* uid = 0x05 */
	PUINT8				GetPktAbsWorkRange(ENG_LPGS flag, UINT8 ph_no, INT32 abs_min=0, INT32 abs_max=0);		/* uid = 0x06 */
	PUINT8				GetPktGetAbsWorkRangeStatus();															/* uid = 0x07 */
	PUINT8				GetPktAfEdgeTrigger(ENG_LPGS flag, UINT16 dist=0, UINT8 enable=0);						/* uid = 0x08 */
	PUINT8				GetPktSetPositionMulti(UINT8 count, PUINT8 ph_no, PINT32 ph_pos);						/* uid = 0x09 */
	PUINT8				GetPktPositionHiRes(ENG_LPGS flag, UINT8 ph_no, INT32 ph_pos=0);						/* uid = 0x0a */
	PUINT8				GetPktOutsideDOFStatus();																/* uid = 0x0b */
	PUINT8				GetPktGetPositionMulti();																/* uid = 0x0c */
	PUINT8				GetPktAfGain(ENG_LPGS flag, UINT16 gain=0);												/* uid = 0x0d */
	PUINT8				GetPktScanEthercat();																	/* uid = 0x0e */
};
