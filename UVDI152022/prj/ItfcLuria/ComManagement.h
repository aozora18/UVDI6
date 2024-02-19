
#pragma once

#include "BaseFamily.h"

class CComManagement : public CBaseFamily
{
/* Constructor & Destructor */
public:

	CComManagement(LPG_LDSM shmem, CCodeToStr *error);
	virtual ~CComManagement();


/* Virtual Function */
protected:

public:

	virtual VOID		SetRecvPacket(UINT8 cmd, PUINT8 data, UINT32 size);


/* Local Member Parameter */
protected:

	UINT8				m_u8Status;		/* Announcement Status 0x01 (Enabled) or 0x00 (Disabled) */

	LPG_LDCM			m_pstCommMgt;	/* Luria Communication Management */


/* Local Member Function */
protected:


/* Public Member Function */
public:

	PUINT8				GetPktAnnounceServerIpAddr(ENG_LPGS flag, PUINT8 ipv4=NULL);	/* uid = 0x01 */
	PUINT8				GetPktGenerateTestAnnouncement();								/* uid = 0x02 */
	PUINT8				GetPktAnnouncementStatus();										/* uid = 0x03 */

	UINT8				GetAnnouncementStatus()		{	return m_u8Status;	}
};
