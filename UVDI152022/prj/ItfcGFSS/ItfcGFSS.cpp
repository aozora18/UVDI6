
/*
 desc : Melsec Q PLC Communication Libarary (MC Protocol)
*/

#include "pch.h"
#include "MainApp.h"
#include "GFSSThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                           ���� ����                                           */
/* --------------------------------------------------------------------------------------------- */

UINT64						g_u64LastWriteTime	= 0;		/* ���� �ֱٿ� �۽��� �ð� ���� */

CGFSSThread					*g_pGFSSThread		= NULL;	


/* --------------------------------------------------------------------------------------------- */
/*                                           ���� �Լ�                                           */
/* --------------------------------------------------------------------------------------------- */





/* --------------------------------------------------------------------------------------------- */
/*                                           �ܺ� �Լ�                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/*
 desc : PLC Server�� ����� ����ϴ� Client ������ ���� �� ����
 parm : config	- [in]  ȯ�� ���� ������ ����� ����ü ������
		speed	- [in]  ������ ���� �ӵ� (����: �и���)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSS_Init(LPG_CSID config, UINT32 speed)
{
	if (speed < 10)	speed = 10;
	// Client ������ ���� �� ���� ��Ŵ
	g_pGFSSThread = new CGFSSThread(0, config);
	if (!g_pGFSSThread)	return FALSE;
	if (!g_pGFSSThread->CreateThread(0, 0, speed))
	{
		delete g_pGFSSThread;
		g_pGFSSThread	= NULL;
		return FALSE;
	}

	return TRUE;
}

/*
 desc : ���̺귯�� ����
 parm : None
 retn : None
*/
API_EXPORT VOID uvGFSS_Close()
{
	// ������ ���� ��Ű�� ��� ���� �Ŀ� ������ ������ ���
	if (g_pGFSSThread)
	{
		// �������� ������ ������ ����
		g_pGFSSThread->CloseClient();
		// ������ ���� ȣ��
		g_pGFSSThread->Stop();
		while (g_pGFSSThread->IsBusy())	g_pGFSSThread->Sleep(100);
		/* �ݹ� ���� �Լ� ȣ�� �� �޸� ���� */
		delete g_pGFSSThread;
		g_pGFSSThread	= NULL;
	}
}

/*
 desc : ���� PLC�� ����Ǿ� �ִ��� ���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSS_IsConnected()
{
	if (!g_pGFSSThread)	return FALSE;
	if (!g_pGFSSThread)	return FALSE;
	if (!g_pGFSSThread->IsBusy())	return FALSE;

	return g_pGFSSThread->IsConnected();
}

/*
 desc : ���� Melsec TCP/IP ��� ���� ��� �� ���
 parm : None
 retn : ENG_TCPS
*/
API_EXPORT ENG_TPCS uvGFSS_GetTCPMode()
{
	if (!g_pGFSSThread)	return ENG_TPCS::en_closed;
	if (!g_pGFSSThread->IsConnected())	return ENG_TPCS::en_closed;
	return g_pGFSSThread->GetTCPMode();
}

/*
 desc : ���� ��ɾ ������ �Ǵ��� ���� ��, ���� �ֱٿ� ���� ��ɾ�� ���� ���� ��ɾ� ���� �ð� ������
		�־��� �ð����� �۴ٸ�, FALSE, ũ�ٸ� TRUE ��ȯ
 parm : period	- [in]  ���� (�� �ð����� ũ��, �۽� ����, �۴ٸ� �۽� �Ұ���) (����: msec)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSS_IsSendTimeValid(UINT64 period)
{
	if (!g_pGFSSThread->IsConnected())	return FALSE;
	return (GetTickCount64() > (g_u64LastWriteTime + period));
}

/*
 desc : ���� �ֱٿ� �߻��� ������ �ִ��� ����
 parm : None
 retn : TRUE or FALSE (���� ����)
*/
API_EXPORT BOOL uvGFSS_IsLastError()
{
	if (!g_pGFSSThread)	return FALSE;
	if (!g_pGFSSThread->IsConnected())	return FALSE;

	return g_pGFSSThread->GetLastErrorLevel() == ENG_LNWE::en_error;
}

/*
 desc : ������� �Ź� ������ ���� ����� �� ��ȯ
 parm : None
 retn : ����� ��
*/
API_EXPORT DOUBLE uvGFSS_GetTotalFilesRate()
{
	if (!g_pGFSSThread)	return 0.0f;
	if (!g_pGFSSThread->IsConnected())	0.0f;

	return g_pGFSSThread->GetTotalFilesRate();
}

/*
 desc : Client�κ��� ���ŵ� ��Ŷ ��� ��ɾ� ��ȯ
 parm : None
 retn : ���� ���ŵ� ��ɾ�� �� ���� ���� �����ߴ� ��ɾ� ��ȯ
		���ŵ� ��ɾ �� �̻� ���ٸ� 0x0000 �� ��ȯ
*/
API_EXPORT ENG_PCGG uvGFSS_PopPktHeadCmd()
{
	STG_UTPH stHead	= {NULL};

	if (!g_pGFSSThread)	return ENG_PCGG::en_pcgg_unknown;
	if (!g_pGFSSThread->IsConnected())		return ENG_PCGG::en_pcgg_unknown;
	if (!g_pGFSSThread->PopPktHead(stHead))	return ENG_PCGG::en_pcgg_unknown;

	return ENG_PCGG(stHead.cmd);
}

/*
 desc : �� ���Ź��� ���ϵ��� ���� �� ��ȯ
 parm : None
 retn : �Ź� ���� ����
*/
API_EXPORT UINT32 uvGFSSS_GetTotalFileCount()
{
	if (!g_pGFSSThread)	return 0;
	if (!g_pGFSSThread->IsConnected())	return 0;
	return g_pGFSSThread->GetTotalFileCount();
}

/*
 desc : ���� ���Źް� �ִ� ���� ����
 parm : None
 retn : �Ź� ���� ����
*/
API_EXPORT UINT32 uvGFSSS_GetCurrentFileCount()
{
	if (!g_pGFSSThread)	return 0;
	if (!g_pGFSSThread->IsConnected())	return 0;
	return g_pGFSSThread->GetCurrentFileCount();
}

/*
 desc : ���� �ֱٿ� Client�κ��� ���Ź��� ������ ũ�� ��ȯ (����: bytes)
 parm : None
 retn : ���� ���� �� ũ�� ��
*/
API_EXPORT UINT32 uvGFSS_GetTotalFileSize()
{
	if (!g_pGFSSThread)	return 0;
	if (!g_pGFSSThread->IsConnected())	return 0;
	return g_pGFSSThread->GetTotalFileSize();
}

/*
 desc : ���� �ֱٿ� Client�κ��� ���Ź��� ������ ũ�� ��ȯ (����: bytes)
 parm : None
 retn : ���� ���� �� ũ�� ��
*/
API_EXPORT UINT32 uvGFSS_GetTotalFileZipSize()
{
	if (!g_pGFSSThread)	return 0;
	if (!g_pGFSSThread->IsConnected())	return 0;
	return g_pGFSSThread->GetTotalFileZipSize();
}

/*
 desc : ���� �ֱٿ� ���� / ���� / ���� ���� ��ȯ
 parm : type	- [in]  0x00:����, 0x01:����, 0x02:����
 retn : ���� (��ü ��� ������)
*/
API_EXPORT PTCHAR uvGFSS_GetLastDir(UINT8 type)
{
	if (!g_pGFSSThread)	return L"";
	if (!g_pGFSSThread->IsConnected())	return L"";
	return g_pGFSSThread->GetLastDir(type);
}

/*
 desc : ���� �ֱٿ� ���Ź޴µ� �ҿ�� �ð� ��ȯ
 parm : None
 retn : �ҿ�ð� ��ȯ (����: msec)
*/
API_EXPORT UINT64 uvGFSS_GetLastTotalTime()
{
	return g_pGFSSThread->GetRecvTotalTime();
}

/*
 desc : ��� �Ź� ������ ���ŵǾ����� ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSS_bIsRecvCompleted()
{
	return g_pGFSSThread->IsRecvCompleted();
}

#ifdef __cplusplus
}
#endif