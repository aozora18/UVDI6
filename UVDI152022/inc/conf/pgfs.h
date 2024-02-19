
/*
 desc : System Configuration Information
*/

#pragma once

#define	GERBER_PATH_NAME						192		/* 거버 파일들의 상위 부모 (폴더) 이름 길이 */
#define	GERBER_FILE_NAME						96		/* 거버 파일 이름 길이 */
#define	RECIPE_FILE_NAME						64		/* Recipe File 이름 길이 */
#define	MAX_PKT_SEND_GFILES						256		/* 한번에 서버에 송신할 수 있는 최대 거버 파일 개수 */
#define	MAX_PKT_LAST_RECV_SIZE					32		/* 가장 최근에 수신된 명령어 임시로 저장하는 버퍼 크기 */


/* PGFS Packet Command */
typedef enum class __en_packet_command_gfss_gfsc__ : UINT16
{
	en_pcgg_unknown					= 0x0000,	/* 존재하지 않는 명령어 */
	/* 거버 파일이 저장될 폴더(경로) 생성 요청 / 응답 */
	en_pcgg_new_gerber_path			= 0xd101,	
	en_pcgg_new_gerber_path_ack		= 0xd201,
	/* 거버 파일의 목록 (정보) 전달 */
	en_pcgg_send_gerber_files		= 0xd102,
//	en_pcgg_send_gerber_files_ack	= 0xd202,	/* Does not exist */
	/* 거버 파일 개수 전달 / 응답 */
	en_pcgg_cnt_gerber_files		= 0xd103,
	en_pcgg_cnt_gerber_files_ack	= 0xd203,
	/* 거버 파일 (폴더; 경로) 삭제 전달 / 응답 */
	en_pcgg_del_gerber_path			= 0xd104,
	en_pcgg_del_gerber_path_ack		= 0xd204,
	/* 모든 거버 파일 전달 완료 */
	en_pcgg_send_gerber_comp		= 0xd105,
	en_pcgg_send_gerber_comp_ack	= 0xd205,
	/* 레시피 파일 요청 / 응답 */
	en_pcgg_get_recipe_files		= 0xd106,
	en_pcgg_get_recipe_files_ack	= 0xd206,
	/* 레시피 파일 전달 / 응답 */
	en_pcgg_set_recipe_files		= 0xd107,
	en_pcgg_set_recipe_files_ack	= 0xd207,

}	ENG_PCGG;


#pragma pack (push, 8)

/* 공통 정보 */
typedef struct __st_config_pgfs_common_info__
{
	UINT8				use_zip_file:1;			/* 파일 압축 후 전송 여부 */
	UINT8				u8_reserved[7];
	UINT32				gfile_timeout;			/* 거버 파일 1개 송신하는데 최대 대기할 수 있는 시간 값 (단위: 밀리초) */
												/* 송신하는데 이 시간 값 이상 걸리면, 나머지 거버 파일 송신 실패 처리 함 */
	UINT32				u32_reserved;

	/*
	 desc : 설정 (or 적재)된 값이 유효한지 여부 확인
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsValid()
	{
		return gfile_timeout > 0 && (use_zip_file == 0 || use_zip_file == 1);
	}

}	STG_CPCI,	*LPG_CPCI;

/* 기본 거버 경로 */
typedef struct __st_config_system_base_path__
{
	PTCHAR				gfss;
	PTCHAR				gfsc;

	/*
	 desc : 경로 설정 관련 메모리 할당
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
	 desc : 경로 설정 관련 메모리 해제
	 parm : None
	 retn : None
	*/
	VOID Close()
	{
		if (gfss)	::Free(gfss);
		if (gfsc)	::Free(gfsc);
	}

	/*
	 desc : 설정 (or 적재)된 값이 유효한지 여부 확인
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsValid()
	{
		if (!gfsc || !gfss)	return FALSE;
		return (wcslen(gfsc) > 0 && wcslen(gfss) > 0);
	}

}	STG_CSBP,	*LPG_CSBP;

/* 전체 환경 정보 */
typedef struct __st_config_system_info_data__
{
	LPG_CPCI			common;
	LPG_CSBP			gerber_path;
	LPG_CSBP			recipe_path;
	LPG_CTSS			server;
	LPG_CTCS			client;

	/*
	 desc : 메모리 할당
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
	 desc : 메모리 해제
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
	 desc : 설정 (or 적재)된 값이 유효한지 여부 확인
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsValid()
	{
		return gerber_path->IsValid() && recipe_path->IsValid() &&
			   server->IsValid(L"PGFS Server") && client->IsValid(L"PGFS Server");
	}

}	STG_CSID,	*LPG_CSID;

/* Packet Data 중 Gebrer File 송수신 패킷의 첫 번재 패킷 부분 */
typedef struct __st_packet_gerber_file_property__
{
	CHAR				path[GERBER_PATH_NAME];		/* Gerber File이 저장될 경로 (기본 드라이브 경로 제외. 거버 이름만 가진 폴더 명만. 하위 폴더는 최대 2개까지만 가능하므로 최대 두배 크기) */
	CHAR				file[GERBER_FILE_NAME];		/* Gerber File 이름 길이 */
	UINT32				s_size;						/* Gerber File의 원본 크기 (단위: bytes) */
	UINT32				file_no;					/* Gerber Path에 저장되어 있는 파일의 순서 (zero-based) */

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