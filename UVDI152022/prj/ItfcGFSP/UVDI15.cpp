
/*
 desc : �ý��� ���� ����
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
 desc : ������
 parm : work	- [in]  �۾� �⺻ ��� (Full path)
 retn : None
*/
CPODIS::CPODIS(PTCHAR work_dir)
	: CConfig()
{
	/* ȯ�� ���� ���� */
	swprintf_s(m_tzFile, MAX_PATH_LEN, L"%s\\info\\pgfs.inf", work_dir);
	/* ���� �ý��� ���� ��� ����ü ������ �ʱ�ȭ */
	m_pstConfig	= NULL;
}

/*
 desc : �ı���
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
 desc : ȯ�� ���Ͽ� ������ ������ ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CPODIS::LoadConfig()
{
	/* ȯ�� ���� �޸� �Ҵ� */
	if (!m_pstConfig)
	{
		m_pstConfig	= (LPG_CSID)::Alloc(sizeof(STG_CSID));
		ASSERT(m_pstConfig);
		if (!m_pstConfig->Init())		return FALSE;
	}

	/* �⺻ ���� ���� */
	if (!LoadInit())					return FALSE;
	return TRUE;
}

/*
 desc : ȯ�� ���Ͽ� ������ ������ ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CPODIS::SaveConfig()
{
	if (!SaveInit())	return FALSE;

	return TRUE;
}

/*
 desc : �⺻ ȯ�� ������ ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CPODIS::LoadInit()
{
	CUniToChar csCnv;

	/* ���� ���� ��� */
	m_pstConfig->common->use_zip_file	= GetConfigUint8(L"COMMON", L"USE_ZIP_SEND");
	m_pstConfig->common->gfile_timeout	= GetConfigUint32(L"COMMON", L"GFILE_TIMEOUT");

	GetConfigStr(L"COMMON", L"GERBER_PATH_GFSS", m_pstConfig->gerber_path->gfss, MAX_PATH_LEN);
	GetConfigStr(L"COMMON", L"GERBER_PATH_GFSC", m_pstConfig->gerber_path->gfsc, MAX_PATH_LEN);
	GetConfigStr(L"COMMON", L"RECIPE_PATH_GFSS", m_pstConfig->recipe_path->gfss, MAX_PATH_LEN);
	GetConfigStr(L"COMMON", L"RECIPE_PATH_GFSC", m_pstConfig->recipe_path->gfsc, MAX_PATH_LEN);

	/* TCP/IP ���� ���� - Server Side */
	m_pstConfig->server->idle_time		= GetConfigUint32(L"TCP/IP", L"IDLE_TIME");
 	m_pstConfig->server->sock_time		= GetConfigUint32(L"TCP/IP", L"SOCK_TIME");
 	m_pstConfig->server->port_buff		= GetConfigUint32(L"TCP/IP", L"PORT_BUFF") * 1024;
 	m_pstConfig->server->recv_buff		= GetConfigUint32(L"TCP/IP", L"RECV_BUFF") * 1024;
 	m_pstConfig->server->ring_buff		= GetConfigUint32(L"TCP/IP", L"RING_BUFF") * 1024;
 	m_pstConfig->server->tcp_port		= GetConfigUint16(L"TCP/IP", L"TCP_PORT");
 	m_pstConfig->server->source_ipv4	= GetConfigIPv4(L"TCP/IP", L"SERVER_IPv4");
 	m_pstConfig->server->target_ipv4	= GetConfigIPv4(L"TCP/IP", L"CLIENT_IPv4");
	/* TCP/IP ���� ���� - Client Side */
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

	/* ��ü ȯ�� ���� ���� ���������� �̵� �Ǿ����� Ȯ�� */
	return IsValidConfig();
}

/*
 desc : �⺻ ȯ�� ������ ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CPODIS::SaveInit()
{
	CUniToChar csCnv;

	return IsValidConfig();
}

/*
 desc : ȯ�� ���� �� ��ȿ�� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CPODIS::IsValidConfig()
{
	return	m_pstConfig->IsValid();
}

