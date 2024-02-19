
#pragma once


/* --------------------------------------------------------------------------------------------- */
/*                                          열거형                                               */
/* --------------------------------------------------------------------------------------------- */

/* 현재 통신 상태 (아래 상태 값은 OR 연산 조합 가능) */
typedef enum class __en_tcpip_packet_comm_status__ : UINT8
{
	en_closed			= 0x00,	/* 해제된 상태 */
	
	en_linking			= 0x01,	/* 연결 시도 상태 */
	en_linked			= 0x02,	/* 연결 완료 상태 */
	en_link_failed		= 0x0F,	/* 연결 설정 실패 */

	en_accepting		= 0x11,
	en_accepted			= 0x12,
	en_accept_failed	= 0x1F,

	en_sended			= 0x21,	/* 송신 완료 상태 */
	en_sending			= 0x22,	/* 송신 시도 상태 */
	en_send_failed		= 0x2F,	/* 송신 실패 상태 */

	en_recved			= 0x31,	/* 수신 완료 상태 */
	en_recving			= 0x32,	/* 수신 시도 상태 */
	en_recv_failed		= 0x3F,	/* 수신 실패 상태 */

}	ENG_TPCS;

/* Packet Header Identification */
typedef enum class __en_packet_header_system_id__ : UINT16
{
	en_head_efem_vs_cmps		= 0xaa99,
	en_head_gfss_vs_gfsc		= 0xaa88,
	en_head_vision_vs_cmps		= 0xaabb,

	en_comn_tail				= 0xffaa,

}	ENG_PHSI;

/* Packet Command : Common */
typedef enum class __en_packet_command_uvdi_common__ : UINT16
{
	en_comn_alive_check			= 0xfff0,		// Server or Client 동작 확인
	en_comn_unknown_packet		= 0xffff,		// 알려지지 않는 명령어 수신

}	ENG_PCUC;


/* --------------------------------------------------------------------------------------------- */
/*                                          구조체                                               */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 1)

/* ---------------------------------------------------------------------------------------------- */
/*                                전역(Global) 구조체 : Common                                    */
/* ---------------------------------------------------------------------------------------------- */

// UV-DI TCP/IP 패킷 헤더
typedef struct __stg_uvdi_tcpip_packet_header__
{
	UINT16				head;			// 헤더 코드
	UINT16				cmd;			// 명령어
	UINT32				length;			// 패킷 전체 길이 (ntohl 변형 수행)

	// desc : 각 멤버 변수들을 Bytes Ordering 수행
	void SwapHostToNetwork()
	{
		head			= htons(head);
		cmd				= htons(cmd);
		length			= htonl(length);
	}
	void SwapNetworkToHost()
	{
		head			= ntohs(head);
		cmd				= ntohs(cmd);
		length			= ntohl(length);
	}
}	STG_UTPH,	*LPG_UTPH;

// UV-DI TCP/IP 패킷 테일
typedef struct __stg_uvdi_tcpip_packet_tailer__
{
	UINT16				crc;						// CRC 값 (0)
	UINT16				tail;						// 0xff, 0xaa 고정 값
	/*
	 desc : 각 멤버 변수들을 Bytes Ordering 수행
	*/
	void SwapHostToNetwork()
	{
		crc				= htons(crc);
		tail			= htons(tail);
	}
	void SwapNetworkToHost()
	{
		crc				= ntohs(crc);
		tail			= ntohs(tail);
	}
}	STG_UTPT,	*LPG_UTPT;

#pragma pack (pop)	// 1 bytes sort

#pragma pack (push, 4)
#pragma pack (pop)

#pragma pack (push, 8)

// TCP/IP Ethernet Client 통신
typedef struct __st_config_tcpip_client_setup__
{
	UINT16				tcp_port;
	UINT16				udp_port;
	UINT16				u16_reserved[2];

	UINT32				source_ipv4;		// 원격 시스템과 통신을 위한 자신 IPv4 주소
	UINT32				target_ipv4;		// 로컬 시스템과 통신을 위한 원격 IPv4 주소

	UINT32				port_buff;			// TCP/IP 소켓의 송수신 버퍼 크기 (단위: KBytes)
	UINT32				recv_buff;			// 한번에 수신된 데이터가 저장될 임시 버퍼 크기 (단위: KBytes)
											// 'PORT_BUFF'보다 항상 2배 이상 커야 됨
	UINT32				ring_buff;			// 수신된 패킷이 처리되기 전에 임시로 저장될 버퍼 크기 (단위: KBytes)
											// 'RECV_BUFF'보다 항상 2배 이상 커야 됨

	UINT32				link_time;			// 이 시간(단위:밀리초) 주기마다 Server와의 통신 연결 작업 수행
											// 0 값보다 무조건 커야 된다.
	UINT32				idle_time;			// 이 시간 동안 통신 송/수신 작업이 없으면 연결 해제
	UINT32				sock_time;			// 소켓 이벤트 감지하는데 최소 대기 시간 (단위: 밀리초)
	UINT32				live_time;			// 이 시간(단위:밀리초) 마다 Alive Check Message 송/수신 수행
	UINT32				u32_reserved;

	BOOL IsValid(PTCHAR name)
	{
		BOOL bSucc	= !(port_buff < 1 || recv_buff < 1 ||
					    (ring_buff != 0 && ring_buff < recv_buff) ||
						idle_time < 1 || source_ipv4 < 1	|| target_ipv4 < 1);
		if (!bSucc)
		{
			TCHAR tzMsg[128]	= {NULL};
			swprintf_s(tzMsg, 128, L"Invalid the tcpip_client_config of (%s)", name);
			AfxMessageBox(tzMsg, MB_ICONSTOP|MB_TOPMOST);
		}

		return bSucc;
	}

}	STG_CTCS,	*LPG_CTCS;

// TCP/IP Ethernet Server 통신
typedef struct __st_config_tcpip_server_setup__
{
	UINT16				tcp_port;
	UINT16				u16_reserved[3];

	UINT32				source_ipv4;	// 원격 시스템과 통신을 위한 자신 IPv4 주소
	UINT32				target_ipv4;	// 로컬 시스템과 통신을 위한 원격 IPv4 주소

	UINT32				port_buff;		// TCP/IP 소켓의 송수신 버퍼 크기 (단위: KBytes)
	UINT32				recv_buff;		// 한번에 수신된 데이터가 저장될 임시 버퍼 크기 (단위: KBytes)
										// 'PORT_BUFF'보다 항상 2배 이상 커야 됨
	UINT32				ring_buff;		// 수신된 패킷이 처리되기 전에 임시로 저장될 버퍼 크기 (단위: KBytes)
										// 'RECV_BUFF'보다 항상 2배 이상 커야 됨

	UINT32				idle_time;		// 주어진 시간(단위: 밀리초) 동안 아무런 통신 작업이 없는 경우, 기존 연결해제 하고, Listen 모드 돌입
										// 0 값보다 무조건 커야 된다.
	UINT32				sock_time;		// 소켓 이벤트 감지하는데 최소 대기 시간 (단위: 밀리초)

	UINT32				u32_reserved;

	/*
	 desc : 환경 정보 유효 검사
	 parm : name	- [in]  구분자
	 retn : TRUE or FALSE
	*/
	BOOL IsValid(TCHAR *name)
	{
		BOOL bSucc	= !(port_buff < 1 || recv_buff < 1 ||
					    (ring_buff != 0 && ring_buff < recv_buff) ||
						idle_time < 1 || source_ipv4 < 1	|| target_ipv4 < 1);
		if (!bSucc)
		{
			TCHAR tzMsg[128]	= {NULL};
			swprintf_s(tzMsg, 128, _T("Invalid the tcpip_server_config of %s"), name);
			AfxMessageBox(tzMsg, MB_ICONSTOP|MB_TOPMOST);
		}
		return bSucc;
			  /* unzip_size < 1); */
	}

}	STG_CTSS,	*LPG_CTSS;

/* ------------------------------------------ */
/* TCP/IP Send & Recv Packet Data. Queue Data */
/* ------------------------------------------ */
typedef struct __st_packet_send_recv_buff__
{
	UINT16				cmd;
	UINT16				u16_reserved[1];	// 수신 받을 크기가 정해져 있지 않은 패킷의 경우, 사용됨
	UINT32				len;				// Packet의 Head와 Tail 부분 제외 길이
	PUINT8				data;				// 본문 데이터의 시작 포인터

}	STG_PSRB,	*LPG_PSRB;

#pragma pack (pop)	/* 8 bytes order */
