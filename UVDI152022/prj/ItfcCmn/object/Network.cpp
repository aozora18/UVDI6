
/*
 desc : Network 모듈
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
 desc : 현재 소켓 기술자에 부여된 IPv4 주소 얻기
 parm : sd	- [in]  소켓 기술자
 retn : IPv4 주소 반환, 실패할 경우 0 값
*/
UINT32 CNetwork::GetSocketToIPv4(SOCKET sd)
{
	INT32 i32Ret, i32AddrLen= sizeof(SOCKADDR_IN);
	INT32 i32SockErr		= 0;
	SOCKADDR_IN sdAddrIn	= {NULL};

	// 접속된 원격 시스템의 IPv4 주소 얻기
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
 desc : 현재 소켓 기술자에 부여된 PORT 번호 얻기
 parm :	sd	- [in]  소켓 기술자
 retn : PORT 번호 반환
*/
UINT16 CNetwork::GetSocketToPort(SOCKET sd)
{
	INT32 iAddrLen		= sizeof(SOCKADDR_IN);
	SOCKADDR_IN sdAddrIn= {NULL};

	// 접속된 원격 시스템의 IPv4 주소 얻기
	if (0 != getpeername(sd, (LPSOCKADDR)&sdAddrIn, &iAddrLen))	return 0;

	return sdAddrIn.sin_port;
}

/*
 desc : 시리얼 통신 포트가 유효한지 즉, Open 가능한지 여부 확인
 parm : port	- [in]  시리얼 통신 포트 번호
 반환: TRUE (유효함) or FALSE (열기 불가능함)
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

	// 포트 명을 가지고 포트 열기 시도
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

	// 반드시 핸들 닫기
	CloseHandle(hComm);

	return TRUE;
}

/*
 desc : 해당 소켓 핸들에 대한 송/수신 포트의 버퍼를 초기화 (리셋) 한다.
 parm : sd	- [in]  소켓 기술자 핸들
 retn : None
*/
VOID CNetwork::ResetPortSendRecvBuff(SOCKET sd)
{
	CHAR bBuff;
	u_long i, uiSize;

	// 포트의 송신 버퍼 비우기
// 	uiSize	= 0;
// 	if (ioctlsocket(sd, FIONWRITE, &uiSize) != SOCKET_ERROR)
// 	{
// 		for (i=0; i<uiSize; i++)	recv(sd, &bBuff, sizeof(UINT8), 0);
// 	}

	// 포트의 수신 버퍼 비우기
	uiSize	= 0;
	if (ioctlsocket(sd, FIONREAD, &uiSize) != SOCKET_ERROR)
	{
		for (i=0; i<uiSize; i++)	recv(sd, &bBuff, sizeof(UINT8), 0);
	}
}

/*
 desc : Bytes IP를 4 바이트의 IPv4로  변환 및 반환
 parm : ipv4	- [in]  문자열 IPv4 (Input)
 retn : 반환될 4바이트의 IPv4
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
 desc : 문자열 IP를 4 바이트의 IPv4로  변환 및 반환
 parm : ipv4	- [in]  문자열 IPv4 (Input)
 retn : 반환될 4바이트의 IPv4
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
 desc : 4 바이트의 IPv4 정보를 바이트 버퍼의 IP로 변환 및 반환
 parm : num_ipv4	- [in]  4바이트의 IPv4
		byte_ipv4	- [out] 반환될 문자열 IPv4의 버퍼 포인터
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
 desc : 4 바이트의 IPv4 정보를 문자열 IP로 변환 및 반환
 parm : num_ipv4	- [in]  4바이트의 IPv4
		str_ipv4	- [out] 반환될 문자열 IPv4의 버퍼 포인터
		str_size	- [in]  'str_ipv4' 버퍼 크기
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
 desc : 시리얼 통신 포트의 연결 속도와 데이터의 크기에 따라,
		패킷을 송신 or 수신되는데 걸리는 최소 시간 값 반환 (단위: 밀리초)
		baud	- [in]  시리얼 통신 포트의 연결 속도 (baud rate)
		size	- [in]  시리얼 통신 포트로 송신 or 수신되는 패킷의 크기 (단위: bytes)
 retn : 통신 포트의 속도 (baud rate)와 패킷의 크기에 따라, 송신 or 수신이 완료되는 시간 값 반환
		반환되는 시간 값의 단위는 msec (밀리초)
*/
UINT32 CNetwork::WaitCommProcTime(UINT32 baud, UINT32 size)
{
	/* -------------------------------------------------------------------------- */
	/* Bytes Per Millisecond (1 msec에 전송할 수 있는 데이터의 크기(단위: Bytes)) */
	/* -------------------------------------------------------------------------- */
	DOUBLE dbBPM	= (baud / 8.0f) / 1000.0f;	// bits --> bytes (bits / 8), 1 sec --> 1000 msec
	/* -------------------------------------------------------------------------- */
	// 1 msec에 전송할 수 있는 크기 값에 총 전송되는 크기 (size)를 곱한 후 반올림 */
	/* -------------------------------------------------------------------------- */
	return (UINT32)ROUNDUP(dbBPM * DOUBLE(size), 0);
}
