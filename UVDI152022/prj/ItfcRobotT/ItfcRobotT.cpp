
/*
 desc : Trigger Board Communication Libarary (Trig Protocol)
*/

#include "pch.h"
#include "MainApp.h"
#include "EFEMThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                           ���� ����                                           */
/* --------------------------------------------------------------------------------------------- */

CEFEMThread		*g_pEFEMThread	= NULL;		// ������ ���� �� ���� ������
LPG_EDSM		g_pstShMemEFEM	= NULL;
LPG_CIEA		g_pstConfig		= NULL;
STG_CTCS		g_stCommInfo	= {NULL};


/* --------------------------------------------------------------------------------------------- */
/*                                           ���� �Լ�                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ���� PLC�� ����Ǿ� �ִ��� ���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL IsConnected()
{
	if (!g_pEFEMThread)				return FALSE;
	if (!g_pEFEMThread->IsBusy())	return FALSE;

	return g_pEFEMThread->IsConnected();
}

/*
 desc : ���� ���� Foup Loaded ���� ��û
 parm : None
 retn : TRUE or FALSE
*/
BOOL ReqGetFoupLoaded()
{
	if (!IsConnected())	return FALSE;

	if (g_pstShMemEFEM->IsFoupLoaded(ENG_EFIN::en_1) || g_pstShMemEFEM->IsFoupLoaded(ENG_EFIN::en_2))
	{
		return g_pEFEMThread->SendData(ENG_ECPC::en_req_foup_loaded);
	}
	return TRUE;
}

/*
 desc : Ư�� FOUP�� �����Ǿ��ٴ� �ñ׳��� �� ���, �ش� FOUP�� Lot list ��û
 parm : foup	- [in]  FOUP ID
		loaded	- [in]  Load ���� (0 or 1)
 retn : TRUE or FALSE
*/
BOOL ReqGetLotList(UINT8 foup, UINT8 loaded)
{
	if (!loaded)	return TRUE;

	return g_pEFEMThread->SendData(ENG_ECPC::en_req_lot_list, &foup, 1);
}

/* --------------------------------------------------------------------------------------------- */
/*                                           �ݹ� �Լ�                                           */
/* --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- */
/*                                           �ܺ� �Լ�                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/*
 desc : EFEM (Robot)�� ��� ����
 parm : config	- [in]  ȯ�� ���� ����
		shmem	- [in]  EFEM ���� ���� �޸�
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_Init(LPG_CIEA config, LPG_EDSM shmem)
{
	/* EFEM Shared Memory */
	g_pstShMemEFEM	= shmem;
	/* All Configuration */
	g_pstConfig		= config;

	/* ����ü�� ��� ���� ���� ���� */
	g_stCommInfo.tcp_port	= g_pstConfig->set_comm.efem_port;
	g_stCommInfo.source_ipv4= uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.cmps_ipv4);
	g_stCommInfo.target_ipv4= uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.efem_ipv4);
	g_stCommInfo.port_buff	= g_pstConfig->set_comm.port_buff;
	g_stCommInfo.recv_buff	= g_pstConfig->set_comm.recv_buff;
	g_stCommInfo.ring_buff	= g_pstConfig->set_comm.ring_buff;
	g_stCommInfo.link_time	= g_pstConfig->set_comm.link_time;
	g_stCommInfo.idle_time	= g_pstConfig->set_comm.idle_time;
	g_stCommInfo.sock_time	= g_pstConfig->set_comm.sock_time;
	g_stCommInfo.live_time	= g_pstConfig->set_comm.live_time;

	/* ------------------------------------------------- */
	/* !!! �߿� !!! �ݵ�� ���⼭ �� ���� ������ ��� �� */
	/* ------------------------------------------------- */
	shmem->foup_lots	= config->efem_svc.foup_lots;
	/* ------------------------------------------------- */

	/* Client ������ ���� �� ���� ��Ŵ */
	g_pEFEMThread	= new CEFEMThread(shmem, 0, &g_stCommInfo);
	if (!g_pEFEMThread)	return FALSE;
	if (!g_pEFEMThread->CreateThread(0, 0, MEDIUM_THREAD_SPEED))
	{
		delete g_pEFEMThread;
		g_pEFEMThread	= NULL;
		return FALSE;
	}

	return TRUE;
}

/*
 desc : EFEM (Robot)�� ��� ����
 parm : config	- [in]  ȯ�� ���� ����
		shmem	- [in]  EFEM ���� ���� �޸�
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_Open(LPG_CIEA config, LPG_EDSM shmem)
{
	/* EFEM Shared Memory */
	g_pstShMemEFEM	= shmem;
	/* All Configuration */
	g_pstConfig		= config;

	return TRUE;
}

/*
 desc : ���̺귯�� ����
 parm : None
 retn : None
*/
API_EXPORT VOID uvEFEM_Close()
{
	/* ������ ���� ��Ű�� ��� ���� �Ŀ� ������ ������ ��� */
	if (g_pEFEMThread)
	{
		/* ������ ���� ȣ�� */
		g_pEFEMThread->Stop();
		while (g_pEFEMThread->IsBusy())	g_pEFEMThread->Sleep(100);
		/* �ݹ� ���� �Լ� ȣ�� �� �޸� ���� */
		delete g_pEFEMThread;
		g_pEFEMThread	= NULL;
	}
}

/*
 desc : ���� PLC�� ����Ǿ� �ִ��� ���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_IsConnected()
{
	return IsConnected();
}

/*
 desc : ���� EFEM�� ���� �� ��ȯ
 parm : None
 retn : 0x00 - Ready, 0x01 - Busy, 0x02 - Error
*/
API_EXPORT ENG_ECSV uvEFEM_GetEfemState()
{
	if (!IsConnected())	return ENG_ECSV::en_error;
	return ENG_ECSV(g_pstShMemEFEM->efem_state);
}

/*
 desc : ���� EFEM�� ���� �����
 parm : None
 retn : 0 ~ 100%
*/
API_EXPORT UINT8 uvEFEM_GetEfemRate()
{
	if (!IsConnected())	return 0x00;
	return g_pstShMemEFEM->efem_rate;
}

/*
 desc : ���� EFEM�� Lot�� ������ ��ġ �� ��ȯ
 parm : None
 retn : 0x00 - None, 0x01 - Head, 0x02 - Aligner, 0x04 - Buffer
*/
API_EXPORT UINT8 uvEFEM_GetEfemWafer()
{
	if (!IsConnected())	return 0x00;
	return g_pstShMemEFEM->efem_wafer;
}

/*
 desc : ���� EFEM or DI Loader�� Lot (Wafer)�� ��� ��� �ִ��� ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_IsLotOnClearAll()
{
	return (g_pstShMemEFEM->efem_wafer & 0x0f) == 0x00 ? TRUE : FALSE;
}

/*
 desc : ���� EFEM�� Buffer�� Lot (Wafer)�� �����ϴ� ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_IsLotOnBuffer()
{
	return (ENG_EWLI::en_wafer_in_buffer == ENG_EWLI(UINT8(ENG_EWLI::en_wafer_in_buffer) & g_pstShMemEFEM->efem_wafer));
}

/*
 desc : ���� EFEM�� Aligner�� Lot (Wafer)�� �����ϴ� ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_IsLotOnAligner()
{
	return (ENG_EWLI::en_wafer_in_aligner == ENG_EWLI(UINT8(ENG_EWLI::en_wafer_in_aligner) & g_pstShMemEFEM->efem_wafer));
}

/*
 desc : ���� EFEM�� ���°� Ready �������� �ƴ��� Ȯ��
 parm : None
 retn : FALSE - Busy or Error, TRUE - Ready
*/
API_EXPORT BOOL uvEFEM_IsEfemReady()
{
	return ENG_ECSV(g_pstShMemEFEM->efem_state) == ENG_ECSV::en_ready ? TRUE : FALSE;
}

/*
 desc : ���� EFEM�� ���°� Error �������� �ƴ��� Ȯ��
 parm : None
 retn : FALSE - Busy or Ready, TRUE - Error
*/
API_EXPORT BOOL uvEFEM_IsEfemError()
{
	if (!IsConnected())	return TRUE;
	return ENG_ECSV(g_pstShMemEFEM->efem_state) == ENG_ECSV::en_error ? TRUE : FALSE;
}

/*
 desc : ���� EFEM�� ���� �� ��ȯ
 parm : None
 retn : Error Code ��
*/
API_EXPORT UINT16 uvEFEM_GetErrorCode()
{
	if (!IsConnected())	return 0x0000;
	return g_pstShMemEFEM->GetError();
}

/*
 desc : ���� FOUP�� ����� Lot (Wafer) ���� ��ȯ
 parm : foup_num	- [in]  FOUP Number (1 or 2)
 retn : Lot Counts
*/
API_EXPORT UINT8 uvEFEM_GetLotCount(UINT8 foup_num)
{
	if (!IsConnected())	return 0x0000;
	return g_pstShMemEFEM->GetLotCount(foup_num);
}

/*
 desc : �뱤 �ʱ�ȭ (���� �뱤�� ���� ��� �ʱ�ȭ)
 parm : None
 retn : None
*/
API_EXPORT VOID uvEFEM_ResetExpose()
{
	if (!IsConnected())	return;
	g_pstShMemEFEM->ResetExpose();
}

/*
 desc : FOUP�� ���� (Loaded)�Ǿ� �ִ��� ����
 parm : foup	- [in]  FOUP Number (1 or 2)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_IsFoupLoaded(ENG_EFIN foup)
{
	return g_pstShMemEFEM->IsFoupLoaded(foup);
}

/*
 desc : FOUP�� Lot (Wafer)�� ���� (Loaded)�Ǿ� �ִ��� ����
 parm : foup	- [in]  FOUP Number (1 or 2)
		lot		- [in]  FOUP�� Lot (Wafer) ��ġ (1 ~ 25)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_IsFoupLotLoaded(ENG_EFIN foup, UINT8 lot)
{
	return g_pstShMemEFEM->IsLotLoaded(foup, lot);
}

/*
 desc : ���� Lot (Wafer)�� ��� ������ ���� ��ġ�� ����Ǿ� �ִ��� ����
 parm : find	- [in]  �ش� ��ġ�� Lot (Wafer)�� ����Ǿ� �ִ��� Ȯ���ϱ� ���� �˻���
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_IsLotLoaded(ENG_EWLI find)
{
	return g_pstShMemEFEM->IsLotLoaded(find);
}

/*
 desc : Reset Event Sync
 parm : None
 retn : None
*/
API_EXPORT VOID uvEFEM_ResetEventSync()
{
	g_pstShMemEFEM->ResetEventSync();
}

/*
 desc : �۽Ű� ���� �̺�Ʈ�� ���� ����ȭ (������)�� �´��� ���� Ȯ��
 parm : None
 parm : TRUE - �´�. FALSE - Ʋ����. (���� CMPS �̺�Ʈ�� ���� EFEM �̺�Ʈ�� ���� �ٸ���)
*/
API_EXPORT BOOL uvEFEM_IsEventSync()
{
	if (!IsConnected())	return FALSE;

	return g_pstShMemEFEM->IsEventSync();
}

/*
 desc : EFEM���κ��� ���ŵ� �̺�Ʈ �ڵ� �� ��ȯ
 parm : None
 parm : ENG_EFEC �� ��ȯ
*/
API_EXPORT UINT8 uvEFEM_GetEFEMEvent()
{
	if (!IsConnected())	return 0x00;

	return g_pstShMemEFEM->efem_event;
}

/*
 desc : ���� EFEM Reset ���� ���� ��ȯ
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_IsEFEMReset()
{
	if (!IsConnected())	return FALSE;
	return g_pstShMemEFEM->IsEFEMReset();
}

/*
 desc : 0 bytes (16 bits) ������ �۽�
 parm : cmd		- [in]  ��ɾ�
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_SendDataCmd(ENG_ECPC cmd)
{
	if (!IsConnected())	return FALSE;

	/* EFEM State�� ��� ���� ���� ����� ������ �۽� */
	if (cmd == ENG_ECPC::en_req_efem_reset)
	{
		/* EFEM Reset �÷��� �ʱ�ȭ */
		g_pstShMemEFEM->SetEFEMReset();
		return g_pEFEMThread->SendData(cmd);
	}

	/* ���� EFEM�� ���°� Busy���� ���� */
	if (ENG_ECSV::en_ready != ENG_ECSV(g_pstShMemEFEM->efem_state))
	{
		return FALSE;
	}
	return g_pEFEMThread->SendData(cmd);
}

/*
 desc : 1 bytes (8 bits) ������ �۽�
 parm : cmd		- [in]  ��ɾ�
		data	- [in]  1 bytes �����Ͱ� ����� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_SendData8(ENG_ECPC cmd, UINT8 data)
{
	if (!IsConnected())	return FALSE;
	/* ���� EFEM�� ���°� Busy���� ���� */
	if (ENG_ECSV::en_ready != ENG_ECSV(g_pstShMemEFEM->efem_state))
	{
		return FALSE;
	}
	return g_pEFEMThread->SendData(cmd, (PUINT8)&data, 1);
}

/*
 desc : 2 bytes (16 bits) ������ �۽�
 parm : cmd		- [in]  ��ɾ�
		data	- [in]  2 bytes �����Ͱ� ����� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_SendData16(ENG_ECPC cmd, UINT16 data)
{
	if (!IsConnected())	return FALSE;
	/* ���� EFEM�� ���°� Busy���� ���� */
	if (ENG_ECSV::en_ready != ENG_ECSV(g_pstShMemEFEM->efem_state))
	{
		return FALSE;
	}
	return g_pEFEMThread->SendData(cmd, (PUINT8)&data, 2);
}

/*
 desc : 4 bytes (32 bits) ������ �۽�
 parm : cmd		- [in]  ��ɾ�
		data	- [in]  4 bytes �����Ͱ� ����� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_SendData32(ENG_ECPC cmd, UINT32 data)
{
	if (!IsConnected())	return FALSE;
	/* ���� EFEM�� ���°� Busy���� ���� */
	if (ENG_ECSV::en_ready != ENG_ECSV(g_pstShMemEFEM->efem_state))
	{
		return FALSE;
	}
	return g_pEFEMThread->SendData(cmd, (PUINT8)&data, 4);
}

/*
 desc : n bytes (n*8 bits) ������ �۽�
 parm : cmd		- [in]  ��ɾ�
		data	- [in]  n bytes �����Ͱ� ����� ����
		size	- [in]  'data' ���ۿ� ����� �������� ũ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_SendData(ENG_ECPC cmd, PUINT8 data, UINT16 size)
{
	if (!IsConnected())	return FALSE;
	/* ���� EFEM�� ���°� Busy���� ���� */
	if (ENG_ECSV::en_ready != ENG_ECSV(g_pstShMemEFEM->efem_state))
	{
		return FALSE;
	}
	return g_pEFEMThread->SendData(cmd, data, size);
}

/*
 desc : Event ��û
 parm : event	- [in]  Event Code (0x01 ~ 0x04)
		data	- [in]  Recipe Data�� ����� ����ü ������
			get		:   EFEM�� FOUP���� DI�� Loader�� Lot (Wafer)�� ������ ��, Lot (Wafer) ȸ�� ���� �� (0x00 : Normal, 0x1 : Invert)
			put		:   Loader���� EFEM�� FOUP�� Lot (Wafer)�� ������ ��, Lot (Wafer) ȸ�� ���� �� (0x00 : Normal, 0x01: Invert)
			foup	:   FOUP Number (1 or 2) (FOUP ��ġ ������ 2���� ���)
			lot		:   Lot (Wafer) Number (1 ~ 25)
 retn : ��� ���� ���� ���� (TRUE or FALSE)
*/
API_EXPORT BOOL uvEFEM_ReqPktEvent(ENG_EFEC event, LPG_PLRD data)
{
	UINT8 u8Data[8]	= { UINT8(event), data->get, data->put, data->foup, data->lot, };

	/* ���� EFEM�� ���°� Busy���� ���� */
	if (ENG_ECSV::en_ready != ENG_ECSV(g_pstShMemEFEM->efem_state))
	{
		return FALSE;
	}
	/* ���� �̺�Ʈ �ڵ� ���� �۽��ϱ� ���� �ʱ�ȭ */
	return !IsConnected() ? FALSE : g_pEFEMThread->SendData(ENG_ECPC::en_req_efem_event, u8Data, 5);
}

/*
 desc : �뱤 ���� ���� ����
 parm : enable	- [in]  TRUE - �뱤 ����, FALSE - �뱤 ����
 retn : None
*/
API_EXPORT VOID uvEFEM_SetExposeMode(BOOL enable)
{
	if (IsConnected())	g_pEFEMThread->SetExposeMode(enable);
}

/*
 desc : EFEM ���� �ʱ�ȭ
 parm : None
 retn : None
*/
API_EXPORT VOID uvEFEM_ResetState()
{
	if (IsConnected())	g_pstShMemEFEM->ResetState();
}

/*
 desc : EFEM State ���°� �־��� �ð� �ȿ� ���ŵǾ����� Ȯ��
 parm : time	- [in]  �� �ð� �̳��� ���ŵǾ����� Ȯ�� �ð� (����: msec)
 retn : TRUE (�־��� �ð����� ª�� ���ŵǾ���)
		FALSE (�־��� �ð� �̳��� ���ŵ��� �ʾҴ�. ��, ���ŵ� �����Ͱ� ���� ����.)
*/
API_EXPORT BOOL uvEFEM_IsUpdateState(UINT64 time)
{
	if (!IsConnected())	return FALSE;

	return g_pstShMemEFEM->IsUpdateState(time);
}

/*
 desc : FOUP Lot List ��û ��Ŷ �۽�
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_ReqFoupLotList()
{
	if (!IsConnected())	return FALSE;
	return g_pEFEMThread->ReqFoupLotList();
}

/*
 desc : ���� �ֱٿ� �߻��� EFEM�� ���� �ڵ� �� �ð� ���� ��ȯ
 parm : None
 retn : 0x0000 or ���� �ڵ� ��ȯ (0x0001 ~ 0xffff)
*/
API_EXPORT LPG_EATD uvEFEM_GetErrorTime()
{
	if (!IsConnected())	return NULL;
	return g_pstShMemEFEM->GetErrorTime();
}


#ifdef __cplusplus
}
#endif