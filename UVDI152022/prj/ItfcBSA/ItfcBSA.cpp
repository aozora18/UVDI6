
/*
 desc : Vision Communication Libarary (Vision Protocol)
*/

#include "pch.h"
#include "MainApp.h"
#include "BSAThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                           ���� ����                                           */
/* --------------------------------------------------------------------------------------------- */

CBSAThread					*g_pBSAThread		= NULL;		// ������ ���� �� ���� ������

/* ���� �޸� */
LPG_RBVC					g_pstShMemVisi		= NULL;
STG_CTCS					g_stCommInfo		= {NULL};


/* --------------------------------------------------------------------------------------------- */
/*                                           ���� �Լ�                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ���� Vision ���� ����Ǿ� �ִ��� ���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL IsConnected()
{
	if (!g_pBSAThread)	return FALSE;
	if (!g_pBSAThread->IsBusy())	return FALSE;

	if (!g_pBSAThread->IsConnected())
	{
		LOG_WARN(ENG_EDIC::en_bsa, L"Not connected to BSA S/W");
		return FALSE;
	}
	return TRUE;
}
/* --------------------------------------------------------------------------------------------- */
/*                                           �ܺ� �Լ�                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/*
 desc : PLC Server�� ����� ����ϴ� Client ������ ���� �� ����
 parm : config	- [in]  ȯ�� ���� ����
		shmem	- [in]  Vision ���� ���� �޸�
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_Init(LPG_CIEA config, LPG_RBVC shmem)
{
	/* Luria Shared Memory */
	g_pstShMemVisi	= shmem;
	/* All Configuration */
	g_pstConfig		= config;

	/* ����ü�� ��� ���� ���� ���� */
	g_stCommInfo.tcp_port	= g_pstConfig->set_comm.visi_port;
	g_stCommInfo.source_ipv4= uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.cmps_ipv4);
	g_stCommInfo.target_ipv4= uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.visi_ipv4);
	g_stCommInfo.port_buff	= g_pstConfig->set_comm.port_buff;
	g_stCommInfo.recv_buff	= g_pstConfig->set_comm.recv_buff;
	g_stCommInfo.ring_buff	= g_pstConfig->set_comm.ring_buff;
	g_stCommInfo.link_time	= g_pstConfig->set_comm.link_time;
	g_stCommInfo.idle_time	= g_pstConfig->set_comm.idle_time;
	g_stCommInfo.sock_time	= g_pstConfig->set_comm.sock_time;
	g_stCommInfo.live_time	= g_pstConfig->set_comm.live_time;

	/* Client ������ ���� �� ���� ��Ŵ */
	g_pBSAThread = new CBSAThread(&g_stCommInfo, shmem);
	if (!g_pBSAThread)	return FALSE;
	ASSERT(g_pBSAThread);
	/* ������ ���� ��Ŵ */
	if (!g_pBSAThread->CreateThread(0, NULL, MEDIUM_THREAD_SPEED))
	{
		delete g_pBSAThread;
		g_pBSAThread = NULL;
		return FALSE;
	}

	return TRUE;
}
/*
 desc : ���̺귯�� ����
 parm : None
 retn : None
*/
API_EXPORT VOID uvBSA_Close()
{
	/* ������ ���� ��Ű�� ��� ���� �Ŀ� ������ ������ ��� */
	if (g_pBSAThread)
	{
		g_pBSAThread->Stop();
		while (g_pBSAThread->IsBusy())	g_pBSAThread->Sleep(100);
		delete g_pBSAThread;
		g_pBSAThread = NULL;
	}
	/* �ݵ�� ����� �� */
	g_pstConfig	= NULL;
}

/* --------------------------------------------------------------------------------------------- */
/*                                    Vision TCP/IP Interface                                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ���ŵ� �����Ͱ� �ִ��� ���� ��ȯ
 parm : cmd	- [in]  ��ɾ�
 retn : TRUE or FALS
*/
API_EXPORT BOOL uvBSA_IsRecvData(ENG_VCPC cmd)
{
	return g_pBSAThread->IsRecvData(cmd);
}

/*
 desc : ���� Vision ��� ����Ǿ� �ִ��� ���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_IsConnected()
{
	return IsConnected();
}

/*
 desc : State ���� ���� �ð� ������ ���� �ð����� ����
 parm : None
 retn : None
*/
API_EXPORT VOID uvBSA_SetUpdateStateTime()
{
	return g_pBSAThread->SetUpdateStateTime();
}

/*
 desc : State ���� �־��� �ð� ���� �� ���̶� ���ŵ� ���� �ִ��� ����
 parm : time	- [in]  �� �ð� (msec) �̳��� ���ŵ����� �ִ��� Ȯ���ϱ� ���� �ð� (msec)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_IsUpdateStateTime(UINT64 time)
{
	return g_pBSAThread->IsUpdateStateTime(time);
}

/*
 desc : ���� �߻��� �̺�Ʈ�� �ִ��� ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_IsVisionNotiEvent()
{
	return g_pBSAThread->IsVisionNotiEvent();
}

/*
 desc : ���� �߻��� �̺�Ʈ�� ��ȿȭ ó��
 parm : None
 retn : None
*/
API_EXPORT VOID uvBSA_ResetVisionNotiEvent()
{
	g_pBSAThread->ResetVisionNotiEvent();
}

/*
 desc : ���� �߻��� �̺�Ʈ �޽��� ��ȯ
 parm : None
 retn : �̺�Ʈ �޽����� ����� ���� ������ ��ȯ
*/
API_EXPORT PTCHAR uvBSA_GetVisionNotiEventMsg()
{
	return g_pBSAThread->GetVisionNotiEventMsg();
}

/*
 desc : ���� Vision ��� ����Ǿ� �ִ��� ���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_SendDataCmd(ENG_VCPC cmd, PUINT8 data, UINT16 size)
{
	/* Vision ���� ����� ������ ��� */
	if (!IsConnected())	return FALSE;
		
	return g_pBSAThread->SendData(cmd, data, size);
}

/*
desc : CMPS <-> Vision �� Ready ���� Ȯ�� ��û
parm : None
retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_ReqVisnState()
{
	/* Vision ���� ����� ������ ��� */
	if (!IsConnected())	return FALSE; 

	return uvBSA_SendDataCmd(ENG_VCPC::en_vision_req_state, NULL, 0x0000);
}

/*
 desc : Vision Centering Command Data Send
 parm :	cam_no	- [in] ī�޶� ���� (0x01: bsa , 0x02: up)
		unit	- [in] Pattern ���� 1 ~ 100 �� ���� Pattern Mark ���� 210715����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_ReqVisionCentering(UINT8 cam_no, UINT8 unit)
{
	if (!IsConnected())	return FALSE;
	return g_pBSAThread->SendCenteringData(ENG_VCPC::en_vision_req_centering, cam_no, unit);
}

/*
 desc : Vision Focus Command Data Send
 parm :	cam_no	- [in] ī�޶� ���� (0x01: bsa , 0x02: up)
		unit	- [in] Pattern ���� 1 ~ 100 �� ���� Pattern Mark ���� 210715����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_ReqVisionFocus(UINT8 cam_no, UINT8 unit)
{
	if (!IsConnected())	return FALSE;
	return g_pBSAThread->SendFocusData(ENG_VCPC::en_vision_req_focus, cam_no, unit);
}

/*
 desc : Vision Focus Command Data Send
 parm :	point	- [in] ���� ���� (0x01: ����1, 0x02: ����2)
		mark	- [in] Mark ��ȣ (0x01: mark1, 0x02: mark2)
		pattern	- [in] pattern ��ȣ 
		posx	- [in] ���� Stage X�� ��ġ
		posy	- [in] ���� Stage Y�� ��ġ
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_ReqVisionGrab(UINT8 point, UINT8 mark, UINT8 pattern, INT32 posx, INT32 posy)
{
#if _SIMULATION_DEBUG_Start == 0
	if (!IsConnected())	return FALSE;
#endif

	return g_pBSAThread->SendGrabData(ENG_VCPC::en_vision_req_align_grab, point, mark, pattern, posx, posy);
}

/*
 desc : Vision View Command Data Send
 parm :	mode	- [in] ī�޶� view Mode (0x01: auto view , 0x02: manual view )
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_ReqVisionView(UINT8 mode)
{
	if (!IsConnected())	return FALSE;
	return g_pBSAThread->SendViewData(ENG_VCPC::en_vision_req_view, mode);
}

/*
 desc : Vision Illuminatino Control Data Send
 parm :	mark	- [in] ���� mark ��ȣ
		onoff	- [in] ���� ���� (0x00 : 0ff, 0x01 : on)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_ReqIllumination(UINT8 mark, UINT8 onoff)
{
	if (!IsConnected())	return FALSE;
	return g_pBSAThread->SendIllumination(ENG_VCPC::en_vision_req_illumination, mark, onoff);
}

/*
 desc : Vision Joblist Command Data Send
 parm :	job_mark_1	- [in] Job�� ������ Mark1 Pattern List
		job_mark_2	- [in] Job�� ������ Mark2 Pattern List
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_ReqVisionJobList(UINT64 job_mark_1, UINT64 job_mark_2)
{
	if (!IsConnected())	return FALSE;
	return g_pBSAThread->SendJoblist(ENG_VCPC::en_vision_req_job_pat_list, job_mark_1, job_mark_2);
}


#ifdef __cplusplus
}
#endif