#pragma once

class CNetwork
{
/* Constructor and Destructor */
public:

	CNetwork();
	virtual ~CNetwork();

/* Virtual Function */
protected:

public:


/* Member Parameter : Local */
protected:



/* Member Function : Local */
protected:


/* Member Function : Public */
public:

	UINT32				GetIPv4BytesToUint32(PUINT8 ipv4);
	UINT32				GetIPv4StrToUint32(PTCHAR ipv4);
	VOID				GetIPv4Uint32ToBytes(UINT32 num_ipv4, PUINT8 byte_ipv4);
	VOID				GetIPv4Uint32ToStr(UINT32 num_ipv4, PTCHAR str_ipv4, UINT32 str_size);
	UINT32				GetSocketToIPv4(SOCKET sd);
	UINT16				GetSocketToPort(SOCKET sd);
	BOOL				IsSerialOpen(UINT8 port);
	VOID				ResetPortSendRecvBuff(SOCKET sd);
	UINT32				WaitCommProcTime(UINT32 baud, UINT32 size);
};
