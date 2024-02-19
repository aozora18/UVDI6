
/*
 desc : 시스템 정보 관리
*/

#include "pch.h"
#include "MainApp.h"
#include "PODIS.h"


#ifdef _DEBUG
#define	new DEBUG_NEW
#undef	THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : work	- [in]  작업 기본 경로 (Full path)
 retn : None
*/
CPODIS::CPODIS(PTCHAR work_dir)
	: CConfig()
{
	/* 환경 파일 설정 */
	swprintf_s(m_tzFile, MAX_PATH_LEN, L"%s\\info\\pgfs.inf", work_dir);
	/* 각종 시스템 관련 멤버 구조체 포인터 초기화 */
	m_pstConfig	= NULL;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CPODIS::~CPODIS()
{
	if (m_pstConfig)
	{
		m_pstConfig->Close();
		::Free(m_pstConfig);
		m_pstConfig	= NULL;
	}
}

/*
 desc : 환경 파일에 설정된 데이터 적재
 parm : None
 retn : TRUE or FALSE
*/
BOOL CPODIS::LoadConfig()
{
	/* 환경 파일 메모리 할당 */
	if (!m_pstConfig)
	{
		m_pstConfig	= (LPG_CSID)::Alloc(sizeof(STG_CSID));
		ASSERT(m_pstConfig);
		if (!m_pstConfig->Init())		return FALSE;
	}

	/* 기본 정보 적재 */
	if (!LoadInit())					return FALSE;
	return TRUE;
}

/*
 desc : 환경 파일에 설정된 데이터 저장
 parm : None
 retn : TRUE or FALSE
*/
BOOL CPODIS::SaveConfig()
{
	if (!SaveInit())	return FALSE;

	return TRUE;
}

/*
 desc : 기본 환경 데이터 적재
 parm : None
 retn : TRUE or FALSE
*/
BOOL CPODIS::LoadInit()
{
	CUniToChar csCnv;

	/* 공통 정보 얻기 */
	m_pstConfig->common->use_zip_file	= GetConfigUint8(L"COMMON", L"USE_ZIP_SEND");
	m_pstConfig->common->gfile_timeout	= GetConfigUint32(L"COMMON", L"GFILE_TIMEOUT");

	GetConfigStr(L"COMMON", L"GERBER_PATH_GFSS", m_pstConfig->gerber_path->gfss, MAX_PATH_LEN);
	GetConfigStr(L"COMMON", L"GERBER_PATH_GFSC", m_pstConfig->gerber_path->gfsc, MAX_PATH_LEN);
	GetConfigStr(L"COMMON", L"RECIPE_PATH_GFSS", m_pstConfig->recipe_path->gfss, MAX_PATH_LEN);
	GetConfigStr(L"COMMON", L"RECIPE_PATH_GFSC", m_pstConfig->recipe_path->gfsc, MAX_PATH_LEN);

	/* TCP/IP 정보 적재 - Server Side */
	m_pstConfig->server->idle_time		= GetConfigUint32(L"TCP/IP", L"IDLE_TIME");
 	m_pstConfig->server->sock_time		= GetConfigUint32(L"TCP/IP", L"SOCK_TIME");
 	m_pstConfig->server->port_buff		= GetConfigUint32(L"TCP/IP", L"PORT_BUFF") * 1024;
 	m_pstConfig->server->recv_buff		= GetConfigUint32(L"TCP/IP", L"RECV_BUFF") * 1024;
 	m_pstConfig->server->ring_buff		= GetConfigUint32(L"TCP/IP", L"RING_BUFF") * 1024;
 	m_pstConfig->server->tcp_port		= GetConfigUint16(L"TCP/IP", L"TCP_PORT");
 	m_pstConfig->server->source_ipv4	= GetConfigIPv4(L"TCP/IP", L"SERVER_IPv4");
 	m_pstConfig->server->target_ipv4	= GetConfigIPv4(L"TCP/IP", L"CLIENT_IPv4");
	/* TCP/IP 정보 적재 - Client Side */
	m_pstConfig->client->idle_time		= GetConfigUint32(L"TCP/IP", L"IDLE_TIME");
	m_pstConfig->client->link_time		= GetConfigUint32(L"TCP/IP", L"LINK_TIME");
 	m_pstConfig->client->sock_time		= GetConfigUint32(L"TCP/IP", L"SOCK_TIME");
 	m_pstConfig->client->live_time		= GetConfigUint32(L"TCP/IP", L"LIVE_TIME");
 	m_pstConfig->client->port_buff		= GetConfigUint32(L"TCP/IP", L"PORT_BUFF") * 1024;
 	m_pstConfig->client->recv_buff		= GetConfigUint32(L"TCP/IP", L"RECV_BUFF") * 1024;
 	m_pstConfig->client->ring_buff		= GetConfigUint32(L"TCP/IP", L"RING_BUFF") * 1024;
 	m_pstConfig->client->tcp_port		= GetConfigUint16(L"TCP/IP", L"TCP_PORT");
 	m_pstConfig->client->source_ipv4	= GetConfigIPv4(L"TCP/IP", L"CLIENT_IPv4");
 	m_pstConfig->client->target_ipv4	= GetConfigIPv4(L"TCP/IP", L"SERVER_IPv4");

	/* 전체 환경 정보 값을 정상적으로 이동 되었는지 확인 */
	return IsValidConfig();
}

/*
 desc : 기본 환경 데이터 저장
 parm : None
 retn : TRUE or FALSE
*/
BOOL CPODIS::SaveInit()
{
	CUniToChar csCnv;

	return IsValidConfig();
}

/*
 desc : 환경 설정 값 유효성 확인
 parm : None
 retn : TRUE or FALSE
*/
BOOL CPODIS::IsValidConfig()
{
	return	m_pstConfig->IsValid();
}

