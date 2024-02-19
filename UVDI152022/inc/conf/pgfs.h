
/*
 desc : System Configuration Information
*/

#pragma once

#define	GERBER_PATH_NAME						192		/* �Ź� ���ϵ��� ���� �θ� (����) �̸� ���� */
#define	GERBER_FILE_NAME						96		/* �Ź� ���� �̸� ���� */
#define	RECIPE_FILE_NAME						64		/* Recipe File �̸� ���� */
#define	MAX_PKT_SEND_GFILES						256		/* �ѹ��� ������ �۽��� �� �ִ� �ִ� �Ź� ���� ���� */
#define	MAX_PKT_LAST_RECV_SIZE					32		/* ���� �ֱٿ� ���ŵ� ��ɾ� �ӽ÷� �����ϴ� ���� ũ�� */


/* PGFS Packet Command */
typedef enum class __en_packet_command_gfss_gfsc__ : UINT16
{
	en_pcgg_unknown					= 0x0000,	/* �������� �ʴ� ��ɾ� */
	/* �Ź� ������ ����� ����(���) ���� ��û / ���� */
	en_pcgg_new_gerber_path			= 0xd101,	
	en_pcgg_new_gerber_path_ack		= 0xd201,
	/* �Ź� ������ ��� (����) ���� */
	en_pcgg_send_gerber_files		= 0xd102,
//	en_pcgg_send_gerber_files_ack	= 0xd202,	/* Does not exist */
	/* �Ź� ���� ���� ���� / ���� */
	en_pcgg_cnt_gerber_files		= 0xd103,
	en_pcgg_cnt_gerber_files_ack	= 0xd203,
	/* �Ź� ���� (����; ���) ���� ���� / ���� */
	en_pcgg_del_gerber_path			= 0xd104,
	en_pcgg_del_gerber_path_ack		= 0xd204,
	/* ��� �Ź� ���� ���� �Ϸ� */
	en_pcgg_send_gerber_comp		= 0xd105,
	en_pcgg_send_gerber_comp_ack	= 0xd205,
	/* ������ ���� ��û / ���� */
	en_pcgg_get_recipe_files		= 0xd106,
	en_pcgg_get_recipe_files_ack	= 0xd206,
	/* ������ ���� ���� / ���� */
	en_pcgg_set_recipe_files		= 0xd107,
	en_pcgg_set_recipe_files_ack	= 0xd207,

}	ENG_PCGG;


#pragma pack (push, 8)

/* ���� ���� */
typedef struct __st_config_pgfs_common_info__
{
	UINT8				use_zip_file:1;			/* ���� ���� �� ���� ���� */
	UINT8				u8_reserved[7];
	UINT32				gfile_timeout;			/* �Ź� ���� 1�� �۽��ϴµ� �ִ� ����� �� �ִ� �ð� �� (����: �и���) */
												/* �۽��ϴµ� �� �ð� �� �̻� �ɸ���, ������ �Ź� ���� �۽� ���� ó�� �� */
	UINT32				u32_reserved;

	/*
	 desc : ���� (or ����)�� ���� ��ȿ���� ���� Ȯ��
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsValid()
	{
		return gfile_timeout > 0 && (use_zip_file == 0 || use_zip_file == 1);
	}

}	STG_CPCI,	*LPG_CPCI;

/* �⺻ �Ź� ��� */
typedef struct __st_config_system_base_path__
{
	PTCHAR				gfss;
	PTCHAR				gfsc;

	/*
	 desc : ��� ���� ���� �޸� �Ҵ�
	 parm : None
	 retn : None
	*/
	BOOL Init()
	{
		gfss	= (PTCHAR)::Alloc(sizeof(TCHAR) * MAX_PATH_LEN);
		gfsc	= (PTCHAR)::Alloc(sizeof(TCHAR) * MAX_PATH_LEN);
		ASSERT(gfss && gfsc);
		return TRUE;
	}
	/*
	 desc : ��� ���� ���� �޸� ����
	 parm : None
	 retn : None
	*/
	VOID Close()
	{
		if (gfss)	::Free(gfss);
		if (gfsc)	::Free(gfsc);
	}

	/*
	 desc : ���� (or ����)�� ���� ��ȿ���� ���� Ȯ��
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsValid()
	{
		if (!gfsc || !gfss)	return FALSE;
		return (wcslen(gfsc) > 0 && wcslen(gfss) > 0);
	}

}	STG_CSBP,	*LPG_CSBP;

/* ��ü ȯ�� ���� */
typedef struct __st_config_system_info_data__
{
	LPG_CPCI			common;
	LPG_CSBP			gerber_path;
	LPG_CSBP			recipe_path;
	LPG_CTSS			server;
	LPG_CTCS			client;

	/*
	 desc : �޸� �Ҵ�
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL Init()
	{
		common		= (LPG_CPCI)::Alloc(sizeof(STG_CPCI));
		gerber_path	= (LPG_CSBP)::Alloc(sizeof(STG_CSBP));
		recipe_path	= (LPG_CSBP)::Alloc(sizeof(STG_CSBP));
		server		= (LPG_CTSS)::Alloc(sizeof(STG_CTSS));
		client		= (LPG_CTCS)::Alloc(sizeof(STG_CTCS));
		ASSERT(gerber_path && recipe_path && server && client);
		if (!(gerber_path->Init() && recipe_path->Init()))	return FALSE;
		
		return TRUE;
	}

	/*
	 desc : �޸� ����
	 parm : None
	 retn : None
	*/
	VOID Close()
	{
		::Free(common);
		gerber_path->Close();
		recipe_path->Close();
		::Free(gerber_path);
		::Free(recipe_path);
		::Free(server);
		::Free(client);
	}

	/*
	 desc : ���� (or ����)�� ���� ��ȿ���� ���� Ȯ��
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsValid()
	{
		return gerber_path->IsValid() && recipe_path->IsValid() &&
			   server->IsValid(L"PGFS Server") && client->IsValid(L"PGFS Server");
	}

}	STG_CSID,	*LPG_CSID;

/* Packet Data �� Gebrer File �ۼ��� ��Ŷ�� ù ���� ��Ŷ �κ� */
typedef struct __st_packet_gerber_file_property__
{
	CHAR				path[GERBER_PATH_NAME];		/* Gerber File�� ����� ��� (�⺻ ����̺� ��� ����. �Ź� �̸��� ���� ���� ��. ���� ������ �ִ� 2�������� �����ϹǷ� �ִ� �ι� ũ��) */
	CHAR				file[GERBER_FILE_NAME];		/* Gerber File �̸� ���� */
	UINT32				s_size;						/* Gerber File�� ���� ũ�� (����: bytes) */
	UINT32				file_no;					/* Gerber Path�� ����Ǿ� �ִ� ������ ���� (zero-based) */

	/*
	 desc : Swap Data
	 parm : None
	 retn : None
	*/
	VOID SwapData()
	{
		s_size	= SWAP32(s_size);
	}

}	STG_PGFP,	*LPG_PGFP;

#pragma pack (pop)	/* 8 bytes boundary sort */