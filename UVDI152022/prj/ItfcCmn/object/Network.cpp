
/*
 desc : Network ���
*/

#include "pch.h"
#include "Network.h"


#ifdef _DEBUG
#define new	DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : Constructor
 parm : None
 retn : None
*/
CNetwork::CNetwork()
{
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CNetwork::~CNetwork()
{
}

/*
 desc : ���� ���� ����ڿ� �ο��� IPv4 �ּ� ���
 parm : sd	- [in]  ���� �����
 retn : IPv4 �ּ� ��ȯ, ������ ��� 0 ��
*/
UINT32 CNetwork::GetSocketToIPv4(SOCKET sd)
{
	INT32 i32Ret, i32AddrLen= sizeof(SOCKADDR_IN);
	INT32 i32SockErr		= 0;
	SOCKADDR_IN sdAddrIn	= {NULL};

	// ���ӵ� ���� �ý����� IPv4 �ּ� ���
	i32Ret = getpeername(sd, (LPSOCKADDR)&sdAddrIn, &i32AddrLen);
	if (SOCKET_ERROR == i32Ret)
	{
#ifdef _DEBUG
		i32SockErr	= WSAGetLastError();
		switch (i32SockErr)
		{
		case WSANOTINITIALISED	: TRACE(L"A successful WSAStartup call must occur before using this function [err_cd = %d]\n", i32SockErr);															break;
		case WSAENETDOWN		: TRACE(L"The network subsystem has failed [err_cd = %d]\n", i32SockErr);																							break;
		case WSAEFAULT			: TRACE(L"The name or the namelen parameter is not in a valid part of the user address space, or the namelen parameter is too small [err_cd = %d]\n", i32SockErr);	break;
		case WSAEINPROGRESS		: TRACE(L"A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function [err_cd = %d]\n", i32SockErr);			break;
		case WSAENOTCONN		: TRACE(L"The socket is not connected [err_cd = %d]\n", i32SockErr);																									break;
		case WSAENOTSOCK		: TRACE(L"The descriptor is not a socket [err_cd = %d]\n", i32SockErr);																								break;
		}
#endif // _DEBUG
		return 0;
	}

	return sdAddrIn.sin_addr.S_un.S_addr;
}

/*
 desc : ���� ���� ����ڿ� �ο��� PORT ��ȣ ���
 parm :	sd	- [in]  ���� �����
 retn : PORT ��ȣ ��ȯ
*/
UINT16 CNetwork::GetSocketToPort(SOCKET sd)
{
	INT32 iAddrLen		= sizeof(SOCKADDR_IN);
	SOCKADDR_IN sdAddrIn= {NULL};

	// ���ӵ� ���� �ý����� IPv4 �ּ� ���
	if (0 != getpeername(sd, (LPSOCKADDR)&sdAddrIn, &iAddrLen))	return 0;

	return sdAddrIn.sin_port;
}

/*
 desc : �ø��� ��� ��Ʈ�� ��ȿ���� ��, Open �������� ���� Ȯ��
 parm : port	- [in]  �ø��� ��� ��Ʈ ��ȣ
 ��ȯ: TRUE (��ȿ��) or FALSE (���� �Ұ�����)
*/
BOOL CNetwork::IsSerialOpen(UINT8 port)
{
	TCHAR tzPort[64]= {NULL};
	HANDLE hComm	= INVALID_HANDLE_VALUE;

#if 1
	if (port < 10)	swprintf_s(tzPort, 64, _T("COM%d"), port);
	else			swprintf_s(tzPort, 64, _T("\\\\.\\COM%d"), port);
#else
	swprintf_s(tzPort, 64, _T("\\\\.\\COM%d"), port);
#endif

	// ��Ʈ ���� ������ ��Ʈ ���� �õ�
	hComm = CreateFile(tzPort,							// Port Name
					   GENERIC_READ | GENERIC_WRITE,	// Desired Access
					   0,								// Shared Mode
					   NULL,							// Security
					   OPEN_EXISTING,					// Creation Disposition
#if 1				   
					   FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
#else				   
					   FILE_FLAG_OVERLAPPED,
#endif				   
					   NULL);								// Non Overlapped
	if (hComm == INVALID_HANDLE_VALUE)
	{
#ifndef _DEBUG
		TCHAR tzMesg[256] = {NULL};
		swprintf_s(tzMesg, 256, _T("Can't open comm_port [%d]"), port);
		AfxMessageBox(tzMesg, MB_ICONSTOP|MB_TOPMOST);
#endif
		return FALSE;
	}

	// �ݵ�� �ڵ� �ݱ�
	CloseHandle(hComm);

	return TRUE;
}

/*
 desc : �ش� ���� �ڵ鿡 ���� ��/���� ��Ʈ�� ���۸� �ʱ�ȭ (����) �Ѵ�.
 parm : sd	- [in]  ���� ����� �ڵ�
 retn : None
*/
VOID CNetwork::ResetPortSendRecvBuff(SOCKET sd)
{
	CHAR bBuff;
	u_long i, uiSize;

	// ��Ʈ�� �۽� ���� ����
// 	uiSize	= 0;
// 	if (ioctlsocket(sd, FIONWRITE, &uiSize) != SOCKET_ERROR)
// 	{
// 		for (i=0; i<uiSize; i++)	recv(sd, &bBuff, sizeof(UINT8), 0);
// 	}

	// ��Ʈ�� ���� ���� ����
	uiSize	= 0;
	if (ioctlsocket(sd, FIONREAD, &uiSize) != SOCKET_ERROR)
	{
		for (i=0; i<uiSize; i++)	recv(sd, &bBuff, sizeof(UINT8), 0);
	}
}

/*
 desc : Bytes IP�� 4 ����Ʈ�� IPv4��  ��ȯ �� ��ȯ
 parm : ipv4	- [in]  ���ڿ� IPv4 (Input)
 retn : ��ȯ�� 4����Ʈ�� IPv4
*/
UINT32 CNetwork::GetIPv4BytesToUint32(PUINT8 ipv4)
{
#if 0
	CHAR szIPv4[32] = { NULL };
	sprintf_s(szIPv4, 32, "%d.%d.%d.%d", ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
	return inet_addr(szIPv4);
#else
	TCHAR tzIPv4[32]	= {NULL};
	SOCKADDR_IN stAddrIn= {NULL};
	swprintf_s(tzIPv4, 32, L"%d.%d.%d.%d", ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
	InetPtonW(AF_INET, tzIPv4, &stAddrIn.sin_addr.s_addr);
	return stAddrIn.sin_addr.s_addr;
#endif
}

/*
 desc : ���ڿ� IP�� 4 ����Ʈ�� IPv4��  ��ȯ �� ��ȯ
 parm : ipv4	- [in]  ���ڿ� IPv4 (Input)
 retn : ��ȯ�� 4����Ʈ�� IPv4
*/
UINT32 CNetwork::GetIPv4StrToUint32(PTCHAR ipv4)
{
#if 0
	CUniToChar csCnv;
	return inet_addr(csCnv.Uni2Ansi(ipv4));
#else
	SOCKADDR_IN stAddrIn= {NULL};
	InetPtonW(AF_INET, ipv4, &stAddrIn.sin_addr.s_addr);
	return stAddrIn.sin_addr.s_addr;
#endif
}

/*
 desc : 4 ����Ʈ�� IPv4 ������ ����Ʈ ������ IP�� ��ȯ �� ��ȯ
 parm : num_ipv4	- [in]  4����Ʈ�� IPv4
		byte_ipv4	- [out] ��ȯ�� ���ڿ� IPv4�� ���� ������
 retn : None
*/
VOID CNetwork::GetIPv4Uint32ToBytes(UINT32 num_ipv4, PUINT8 byte_ipv4)
{
	IN_ADDR stInAddr		= {NULL};
	CUniToChar csCnv;

	memcpy(&stInAddr.s_addr, &num_ipv4, sizeof(UINT32));

	byte_ipv4[0]	= stInAddr.S_un.S_un_b.s_b1;
	byte_ipv4[1]	= stInAddr.S_un.S_un_b.s_b2;
	byte_ipv4[2]	= stInAddr.S_un.S_un_b.s_b3;
	byte_ipv4[3]	= stInAddr.S_un.S_un_b.s_b4;
}

/*
 desc : 4 ����Ʈ�� IPv4 ������ ���ڿ� IP�� ��ȯ �� ��ȯ
 parm : num_ipv4	- [in]  4����Ʈ�� IPv4
		str_ipv4	- [out] ��ȯ�� ���ڿ� IPv4�� ���� ������
		str_size	- [in]  'str_ipv4' ���� ũ��
 retn : None
*/
VOID CNetwork::GetIPv4Uint32ToStr(UINT32 num_ipv4, PTCHAR str_ipv4, UINT32 str_size)
{
	IN_ADDR stInAddr		= {NULL};
	memcpy(&stInAddr.s_addr, &num_ipv4, sizeof(UINT32));
#if 0
	CUniToChar csCnv;
	swprintf_s(str_ipv4, IPv4_LENGTH, _T("%s"), csCnv.Ansi2Uni(inet_ntoa(stInAddr)));
#else
	InetNtopW(AF_INET, (CONST VOID*)&stInAddr, str_ipv4, str_size);
#endif
}

/*
 desc : �ø��� ��� ��Ʈ�� ���� �ӵ��� �������� ũ�⿡ ����,
		��Ŷ�� �۽� or ���ŵǴµ� �ɸ��� �ּ� �ð� �� ��ȯ (����: �и���)
		baud	- [in]  �ø��� ��� ��Ʈ�� ���� �ӵ� (baud rate)
		size	- [in]  �ø��� ��� ��Ʈ�� �۽� or ���ŵǴ� ��Ŷ�� ũ�� (����: bytes)
 retn : ��� ��Ʈ�� �ӵ� (baud rate)�� ��Ŷ�� ũ�⿡ ����, �۽� or ������ �Ϸ�Ǵ� �ð� �� ��ȯ
		��ȯ�Ǵ� �ð� ���� ������ msec (�и���)
*/
UINT32 CNetwork::WaitCommProcTime(UINT32 baud, UINT32 size)
{
	/* -------------------------------------------------------------------------- */
	/* Bytes Per Millisecond (1 msec�� ������ �� �ִ� �������� ũ��(����: Bytes)) */
	/* -------------------------------------------------------------------------- */
	DOUBLE dbBPM	= (baud / 8.0f) / 1000.0f;	// bits --> bytes (bits / 8), 1 sec --> 1000 msec
	/* -------------------------------------------------------------------------- */
	// 1 msec�� ������ �� �ִ� ũ�� ���� �� ���۵Ǵ� ũ�� (size)�� ���� �� �ݿø� */
	/* -------------------------------------------------------------------------- */
	return (UINT32)ROUNDUP(dbBPM * DOUBLE(size), 0);
}
