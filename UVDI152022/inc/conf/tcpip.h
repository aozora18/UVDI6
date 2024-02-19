
#pragma once


/* --------------------------------------------------------------------------------------------- */
/*                                          ������                                               */
/* --------------------------------------------------------------------------------------------- */

/* ���� ��� ���� (�Ʒ� ���� ���� OR ���� ���� ����) */
typedef enum class __en_tcpip_packet_comm_status__ : UINT8
{
	en_closed			= 0x00,	/* ������ ���� */
	
	en_linking			= 0x01,	/* ���� �õ� ���� */
	en_linked			= 0x02,	/* ���� �Ϸ� ���� */
	en_link_failed		= 0x0F,	/* ���� ���� ���� */

	en_accepting		= 0x11,
	en_accepted			= 0x12,
	en_accept_failed	= 0x1F,

	en_sended			= 0x21,	/* �۽� �Ϸ� ���� */
	en_sending			= 0x22,	/* �۽� �õ� ���� */
	en_send_failed		= 0x2F,	/* �۽� ���� ���� */

	en_recved			= 0x31,	/* ���� �Ϸ� ���� */
	en_recving			= 0x32,	/* ���� �õ� ���� */
	en_recv_failed		= 0x3F,	/* ���� ���� ���� */

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
	en_comn_alive_check			= 0xfff0,		// Server or Client ���� Ȯ��
	en_comn_unknown_packet		= 0xffff,		// �˷����� �ʴ� ��ɾ� ����

}	ENG_PCUC;


/* --------------------------------------------------------------------------------------------- */
/*                                          ����ü                                               */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 1)

/* ---------------------------------------------------------------------------------------------- */
/*                                ����(Global) ����ü : Common                                    */
/* ---------------------------------------------------------------------------------------------- */

// UV-DI TCP/IP ��Ŷ ���
typedef struct __stg_uvdi_tcpip_packet_header__
{
	UINT16				head;			// ��� �ڵ�
	UINT16				cmd;			// ��ɾ�
	UINT32				length;			// ��Ŷ ��ü ���� (ntohl ���� ����)

	// desc : �� ��� �������� Bytes Ordering ����
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

// UV-DI TCP/IP ��Ŷ ����
typedef struct __stg_uvdi_tcpip_packet_tailer__
{
	UINT16				crc;						// CRC �� (0)
	UINT16				tail;						// 0xff, 0xaa ���� ��
	/*
	 desc : �� ��� �������� Bytes Ordering ����
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

// TCP/IP Ethernet Client ���
typedef struct __st_config_tcpip_client_setup__
{
	UINT16				tcp_port;
	UINT16				udp_port;
	UINT16				u16_reserved[2];

	UINT32				source_ipv4;		// ���� �ý��۰� ����� ���� �ڽ� IPv4 �ּ�
	UINT32				target_ipv4;		// ���� �ý��۰� ����� ���� ���� IPv4 �ּ�

	UINT32				port_buff;			// TCP/IP ������ �ۼ��� ���� ũ�� (����: KBytes)
	UINT32				recv_buff;			// �ѹ��� ���ŵ� �����Ͱ� ����� �ӽ� ���� ũ�� (����: KBytes)
											// 'PORT_BUFF'���� �׻� 2�� �̻� Ŀ�� ��
	UINT32				ring_buff;			// ���ŵ� ��Ŷ�� ó���Ǳ� ���� �ӽ÷� ����� ���� ũ�� (����: KBytes)
											// 'RECV_BUFF'���� �׻� 2�� �̻� Ŀ�� ��

	UINT32				link_time;			// �� �ð�(����:�и���) �ֱ⸶�� Server���� ��� ���� �۾� ����
											// 0 ������ ������ Ŀ�� �ȴ�.
	UINT32				idle_time;			// �� �ð� ���� ��� ��/���� �۾��� ������ ���� ����
	UINT32				sock_time;			// ���� �̺�Ʈ �����ϴµ� �ּ� ��� �ð� (����: �и���)
	UINT32				live_time;			// �� �ð�(����:�и���) ���� Alive Check Message ��/���� ����
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

// TCP/IP Ethernet Server ���
typedef struct __st_config_tcpip_server_setup__
{
	UINT16				tcp_port;
	UINT16				u16_reserved[3];

	UINT32				source_ipv4;	// ���� �ý��۰� ����� ���� �ڽ� IPv4 �ּ�
	UINT32				target_ipv4;	// ���� �ý��۰� ����� ���� ���� IPv4 �ּ�

	UINT32				port_buff;		// TCP/IP ������ �ۼ��� ���� ũ�� (����: KBytes)
	UINT32				recv_buff;		// �ѹ��� ���ŵ� �����Ͱ� ����� �ӽ� ���� ũ�� (����: KBytes)
										// 'PORT_BUFF'���� �׻� 2�� �̻� Ŀ�� ��
	UINT32				ring_buff;		// ���ŵ� ��Ŷ�� ó���Ǳ� ���� �ӽ÷� ����� ���� ũ�� (����: KBytes)
										// 'RECV_BUFF'���� �׻� 2�� �̻� Ŀ�� ��

	UINT32				idle_time;		// �־��� �ð�(����: �и���) ���� �ƹ��� ��� �۾��� ���� ���, ���� �������� �ϰ�, Listen ��� ����
										// 0 ������ ������ Ŀ�� �ȴ�.
	UINT32				sock_time;		// ���� �̺�Ʈ �����ϴµ� �ּ� ��� �ð� (����: �и���)

	UINT32				u32_reserved;

	/*
	 desc : ȯ�� ���� ��ȿ �˻�
	 parm : name	- [in]  ������
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
	UINT16				u16_reserved[1];	// ���� ���� ũ�Ⱑ ������ ���� ���� ��Ŷ�� ���, ����
	UINT32				len;				// Packet�� Head�� Tail �κ� ���� ����
	PUINT8				data;				// ���� �������� ���� ������

}	STG_PSRB,	*LPG_PSRB;

#pragma pack (pop)	/* 8 bytes order */
