
#pragma once

class CCodeToStr;

class CBaseFamily
{
/* Constructor / Destructor */
public:

	CBaseFamily(LPG_LDSM shmem, CCodeToStr *error);
	virtual ~CBaseFamily();

/* Virtual Function */
protected:

public:

	virtual VOID		SetRecvPacket(UINT8 uid, PUINT8 data, UINT32 size) = 0;


/* Local Member Parameter */
protected:

	UINT32				m_u32PktSize;		/* 가장 최근에 생성된 패킷 크기 값 저장 */

	ENG_LUDF			m_enFamily;			/* User Data : Familiy ID */
	
	LPG_LDSM			m_pstShMemLuria;	/* Luria 공유 메모리 영역 */

	CCodeToStr			*m_pCodeToStr;		/* 각종 에러 정보를 관리하는 객체 */


/* Local Member Fucntion */
protected:

	BOOL				ValidFamilyID(ENG_LUDF f_id);
	BOOL				ValidVersion(UINT16 major, UINT16 minor, UINT16 build, UINT8 mode, BOOL logs=TRUE);

	PUINT8				GetPktBase(ENG_LTCT type, ENG_LCMC cmd, PUINT8 p_data, UINT32 p_size, PUINT8 u_data=NULL, UINT32 u_size=0);
	PUINT8				GetPktBase(ENG_LTCT type, ENG_LCSS cmd, PUINT8 p_data, UINT32 p_size, PUINT8 u_data=NULL, UINT32 u_size=0);
	PUINT8				GetPktBase(ENG_LTCT type, ENG_LCJM cmd, PUINT8 p_data, UINT32 p_size, PUINT8 u_data=NULL, UINT32 u_size=0);
	PUINT8				GetPktBase(ENG_LTCT type, ENG_LCPP cmd, PUINT8 p_data, UINT32 p_size, PUINT8 u_data=NULL, UINT32 u_size=0);
	PUINT8				GetPktBase(ENG_LTCT type, ENG_LCEP cmd, PUINT8 p_data, UINT32 p_size, PUINT8 u_data=NULL, UINT32 u_size=0);
	PUINT8				GetPktBase(ENG_LTCT type, ENG_LCDP cmd, PUINT8 p_data, UINT32 p_size, PUINT8 u_data=NULL, UINT32 u_size=0);
	PUINT8				GetPktBase(ENG_LTCT type, ENG_LCCM cmd, PUINT8 p_data, UINT32 p_size, PUINT8 u_data=NULL, UINT32 u_size=0);
	PUINT8				GetPktBase(ENG_LTCT type, ENG_LCPF cmd, PUINT8 p_data, UINT32 p_size, PUINT8 u_data=NULL, UINT32 u_size=0);
	PUINT8				GetPktBase(ENG_LTCT type, UINT8 cmd, PUINT8 p_data, UINT32 p_size, PUINT8 u_data, UINT32 u_size);
#if 0
	PUINT8				GetPktCommon(ENG_LCMC cmd, ENG_LPGS flag=ENG_LPGS::en_get, UINT64 data=0, UINT8 size=0);
	PUINT8				GetPktCommon(UINT8 cmd, ENG_LPGS flag, UINT64 data, UINT8 size);
#endif

/* Public Member Function */
public:

	/* Returns the size of the most recently stored packet */
	UINT32				GetPktSize()	{	return m_u32PktSize;	}
};