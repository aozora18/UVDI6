
/*
 desc : Philhmi Communication Libarary (Philhmi Protocol)
*/

#include "pch.h"
#include "MainApp.h"
#include "PhilhmiLib.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                           전역 변수                                           */
/* --------------------------------------------------------------------------------------------- */

CPhilhmiLib* g_pPhilhmi = NULL;		// 데이터 수집 및 감시 스레드

/* 공유 메모리 */
STG_CTCS					g_stCommInfo = { NULL };
HWND						g_hwnd = NULL;

/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

BOOL IsConnected()
{
	if (!g_pPhilhmi)				return FALSE;

	return (g_pPhilhmi->IsConnect());
}


/* --------------------------------------------------------------------------------------------- */
/*                                           외부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

	API_EXPORT BOOL uvPhilhmi_Init(HWND hWnd, LPG_CIEA config, LPG_PPR shmem, LPG_DLSM link)
	{
		/* All Configuration */
		g_pstConfig = config;
		g_hwnd = hWnd;
		/* 구조체에 통신 관련 정보 설정 */
		g_stCommInfo.tcp_port = g_pstConfig->set_comm.philhmi_port;
		g_stCommInfo.source_ipv4 = uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.cmps_ipv4);
		g_stCommInfo.target_ipv4 = uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.philhmi_ipv4);
		g_stCommInfo.port_buff = g_pstConfig->set_comm.port_buff;
		g_stCommInfo.recv_buff = g_pstConfig->set_comm.recv_buff;
		g_stCommInfo.ring_buff = g_pstConfig->set_comm.ring_buff;
		g_stCommInfo.link_time = g_pstConfig->set_comm.link_time;
		g_stCommInfo.idle_time = g_pstConfig->set_comm.idle_time;
		g_stCommInfo.sock_time = g_pstConfig->set_comm.sock_time;
		g_stCommInfo.live_time = g_pstConfig->set_comm.live_time;

		// Client 스레드 생성 및 동작 시킴
		g_pPhilhmi = new CPhilhmiLib();
		//hWnd, 
		if (!g_pPhilhmi)	return FALSE;
		if (!g_pPhilhmi->Init(hWnd, &g_stCommInfo, &g_pstConfig->set_comn))
		{
			delete g_pPhilhmi;
			g_pPhilhmi = NULL;
			return FALSE;
		}

		return TRUE;
	}

	API_EXPORT BOOL uvPhilhmi_IsSyncResultLocked()
	{
		return g_pPhilhmi->IsSyncResultLocked();
	}
	
	API_EXPORT BOOL uvPhilhmi_Open(LPG_CIEA config, LPG_PPR shmem)
	{
		LPG_CTSP pstPhilhmi = NULL;

		/* All Configuration */
		g_pstConfig = config;

		if (!g_pPhilhmi)	return FALSE;
		if (!g_pPhilhmi->Open())
		{
			delete g_pPhilhmi;
			g_pPhilhmi = NULL;
			return FALSE;
		}

		return TRUE;
	}

	API_EXPORT VOID uvPhilhmi_Close()
	{
		// 스레드 정지 시키는 명령 전달 후에 정지될 때까지 대기
		if (g_pPhilhmi)
		{
			/* 약 데이터 송신될 때까지 대기 */
			Sleep(500);
			// 스레드 중지 호출
			g_pPhilhmi->Close();
			delete g_pPhilhmi;
			g_pPhilhmi = NULL;
		}
	}

	API_EXPORT BOOL uvPhilhmi_Reconnected()
	{
		g_stCommInfo.tcp_port = g_pstConfig->set_comm.philhmi_port;
		g_stCommInfo.source_ipv4 = uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.cmps_ipv4);
		g_stCommInfo.target_ipv4 = uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.philhmi_ipv4);
		g_stCommInfo.port_buff = g_pstConfig->set_comm.port_buff;
		g_stCommInfo.recv_buff = g_pstConfig->set_comm.recv_buff;
		g_stCommInfo.ring_buff = g_pstConfig->set_comm.ring_buff;
		g_stCommInfo.link_time = g_pstConfig->set_comm.link_time;
		g_stCommInfo.idle_time = g_pstConfig->set_comm.idle_time;
		g_stCommInfo.sock_time = g_pstConfig->set_comm.sock_time;
		g_stCommInfo.live_time = g_pstConfig->set_comm.live_time;

		if (!g_pPhilhmi)	return FALSE;
		if (!g_pPhilhmi->Init(g_hwnd, &g_stCommInfo, &g_pstConfig->set_comn))
		{
			delete g_pPhilhmi;
			g_pPhilhmi = NULL;
			return FALSE;
		}

		return TRUE;
	}

	API_EXPORT BOOL uvPhilhmi_IsConnected()
	{
		return IsConnected();
	}

	API_EXPORT BOOL uvPhilhmi_IsSendCmdCountUnder(UINT16 count)
	{
		return TRUE;
	}

	API_EXPORT BOOL uvPhilhmi_GetRecvDataFromCommand(int nCommand, STG_PP_PACKET_RECV* stRecv)
	{
		return g_pPhilhmi->GetRecvDataFromCommand(nCommand, stRecv);
	}

	API_EXPORT BOOL uvPhilhmi_GetRecvDataFromUniqueID(unsigned int nUniqueID, STG_PP_PACKET_RECV* stRecv)
	{
		return g_pPhilhmi->GetRecvDataFromUniqueID(nUniqueID, stRecv);
	}

	API_EXPORT BOOL uvPhilhmi_GetRecvWaitFromUniqueID(unsigned int nUniqueID, STG_PP_PACKET_RECV* stRecv, int nTimeout)
	{
		return g_pPhilhmi->GetRecvWaitFromUniqueID(nUniqueID, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvPhilhmi_DeleteRecvDataFromUniqueID(unsigned int nUniqueID)
	{
		return g_pPhilhmi->DeleteRecvDataFromUniqueID(nUniqueID);
	}

	API_EXPORT BOOL uvPhilhmi_Send_P2C_RCP_CREATE(STG_PP_P2C_RCP_CREATE& stSend, STG_PP_P2C_RCP_CREATE_ACK& stRecv, int nTimeout)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_P2C_RCP_CREATE(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvPhilhmi_Send_P2C_RCP_DELETE(STG_PP_P2C_RCP_DELETE& stSend, STG_PP_P2C_RCP_DELETE_ACK& stRecv, int nTimeout)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_P2C_RCP_DELETE(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvPhilhmi_Send_P2C_RCP_MODIFY(STG_PP_P2C_RCP_MODIFY& stSend, STG_PP_P2C_RCP_MODIFY_ACK& stRecv, int nTimeout)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_P2C_RCP_MODIFY(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvPhilhmi_Send_P2C_RCP_SELECT(STG_PP_P2C_RCP_SELECT& stSend, STG_PP_P2C_RCP_SELECT_ACK& stRecv, int nTimeout)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_P2C_RCP_SELECT(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvPhilhmi_Send_P2C_IO_STATUS(STG_PP_P2C_IO_STATUS& stSend, STG_PP_P2C_IO_STATUS_ACK& stRecv, int nTimeout)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_P2C_IO_STATUS(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvPhilhmi_Send_P2C_OUTPUT_ONOFF(STG_PP_P2C_OUTPUT_ONOFF& stSend, STG_PP_P2C_OUTPUT_ONOFF_ACK& stRecv, int nTimeout)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_P2C_OUTPUT_ONOFF(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvPhilhmi_Send_P2C_ABS_MOVE(STG_PP_P2C_ABS_MOVE& stSend, STG_PP_P2C_ABS_MOVE_ACK& stRecv, int nTimeout)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_P2C_ABS_MOVE(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvPhilhmi_Send_P2C_ABS_MOVE_COMP(STG_PP_P2C_ABS_MOVE_COMP& stSend, STG_PP_P2C_ABS_MOVE_COMP_ACK& stRecv, int nTimeout)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_P2C_ABS_MOVE_COMP(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvPhilhmi_Send_P2C_REL_MOVE(STG_PP_P2C_REL_MOVE& stSend, STG_PP_P2C_REL_MOVE_ACK& stRecv, int nTimeout)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_P2C_REL_MOVE(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvPhilhmi_Send_P2C_REL_MOVE_COMP(STG_PP_P2C_REL_MOVE_COMP& stSend, STG_PP_P2C_REL_MOVE_COMP_ACK& stRecv, int nTimeout)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_P2C_REL_MOVE_COMP(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvPhilhmi_Send_P2C_CHAR_MOVE(STG_PP_P2C_CHAR_MOVE& stSend, STG_PP_P2C_CHAR_MOVE_ACK& stRecv, int nTimeout)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_P2C_CHAR_MOVE(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvPhilhmi_Send_P2C_CHAR_MOVE_COMP(STG_PP_P2C_CHAR_MOVE_COMP& stSend, STG_PP_P2C_CHAR_MOVE_COMP_ACK& stRecv, int nTimeout)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_P2C_CHAR_MOVE_COMP(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvPhilhmi_Send_P2C_PROCESS_STEP(STG_PP_P2C_PROCESS_STEP& stSend, STG_PP_P2C_PROCESS_STEP_ACK& stRecv, int nTimeout)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_P2C_PROCESS_STEP(stSend, nTimeout);
	}

	API_EXPORT BOOL uvPhilhmi_Send_P2C_PROCESS_COMP(STG_PP_P2C_PROCESS_COMP& stSend, STG_PP_P2C_PROCESS_COMP_ACK& stRecv, int nTimeout)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_P2C_PROCESS_COMP(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvPhilhmi_Send_P2C_SUBPROCESS_COMP(STG_PP_P2C_SUBPROCESS_COMP& stSend, STG_PP_P2C_SUBPROCESS_COMP_ACK& stRecv, int nTimeout)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_P2C_SUBPROCESS_COMP(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvPhilhmi_Send_P2C_EC_MODIFY(STG_PP_P2C_EC_MODIFY& stSend, STG_PP_P2C_EC_MODIFY_ACK& stRecv, int nTimeout)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_P2C_EC_MODIFY(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvPhilhmi_Send_P2C_INITIAL_COMPLETE(STG_PP_P2C_INITIAL_COMPLETE& stSend, STG_PP_P2C_INITIAL_COMPLETE_ACK& stRecv, int nTimeout)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_P2C_INITIAL_COMPLETE(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvPhilhmi_Send_P2C_ALARM_OCCUR(STG_PP_P2C_ALARM_OCCUR& stSend, STG_PP_P2C_ALARM_OCCUR_ACK& stRecv, int nTimeout)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_P2C_ALARM_OCCUR(stSend, stRecv, nTimeout);
	}
	API_EXPORT BOOL uvPhilhmi_Send_P2C_EVENT_NOTIFY(STG_PP_P2C_EVENT_NOTIFY& stSend, STG_PP_P2C_EVENT_NOTIFY_ACK& stRecv, int nTimeout)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_P2C_EVENT_NOTIFY(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvPhilhmi_Send_P2C_INTERRUPT_STOP(STG_PP_P2C_INTERRUPT_STOP& stSend, STG_PP_P2C_INTERRUPT_STOP_ACK& stRecv, int nTimeout)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_P2C_INTERRUPT_STOP(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvPhilhmi_Send_C2P_RCP_CREATE_ACK(STG_PP_C2P_RCP_CREATE_ACK& stSend)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_C2P_RCP_CREATE_ACK(stSend);
	}

	API_EXPORT BOOL uvPhilhmi_Send_C2P_RCP_DELETE_ACK(STG_PP_C2P_RCP_DELETE_ACK& stSend)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_C2P_RCP_DELETE_ACK(stSend);
	}

	API_EXPORT BOOL uvPhilhmi_Send_C2P_RCP_MODIFY_ACK(STG_PP_C2P_RCP_MODIFY_ACK& stSend)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_C2P_RCP_MODIFY_ACK(stSend);
	}

	API_EXPORT BOOL uvPhilhmi_Send_C2P_RCP_SELECT_ACK(STG_PP_C2P_RCP_SELECT_ACK& stSend)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_C2P_RCP_SELECT_ACK(stSend);
	}

	API_EXPORT BOOL uvPhilhmi_Send_C2P_RCP_INFO_ACK(STG_PP_C2P_RCP_INFO_ACK& stSend)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_C2P_RCP_INFO_ACK(stSend);
	}

	API_EXPORT BOOL uvPhilhmi_Send_C2P_RCP_LIST_ACK(STG_PP_C2P_RCP_LIST_ACK& stSend)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_C2P_RCP_LIST_ACK(stSend);
	}

	API_EXPORT BOOL uvPhilhmi_Send_C2P_ABS_MOVE_ACK(STG_PP_C2P_ABS_MOVE_ACK& stSend)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_C2P_ABS_MOVE_ACK(stSend);
	}

	API_EXPORT BOOL uvPhilhmi_Send_C2P_ABS_MOVE_COMP_ACK(STG_PP_C2P_ABS_MOVE_COMP_ACK& stSend)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_C2P_ABS_MOVE_COMP_ACK(stSend);
	}

	API_EXPORT BOOL uvPhilhmi_Send_C2P_REL_MOVE_ACK(STG_PP_C2P_REL_MOVE_ACK& stSend)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_C2P_REL_MOVE_ACK(stSend);
	}

	API_EXPORT BOOL uvPhilhmi_Send_C2P_REL_MOVE_COMP_ACK(STG_PP_C2P_REL_MOVE_COMP_ACK& stSend)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_C2P_REL_MOVE_COMP_ACK(stSend);
	}

	API_EXPORT BOOL uvPhilhmi_Send_C2P_CHAR_MOVE_ACK(STG_PP_C2P_CHAR_MOVE_ACK& stSend)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_C2P_CHAR_MOVE_ACK(stSend);
	}

	API_EXPORT BOOL uvPhilhmi_Send_C2P_CHAR_MOVE_COMP_ACK(STG_PP_C2P_CHAR_MOVE_COMP_ACK& stSend)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_C2P_CHAR_MOVE_COMP_ACK(stSend);
	}

	API_EXPORT BOOL uvPhilhmi_Send_C2P_PROCESS_EXECUTE_ACK(STG_PP_C2P_PROCESS_EXECUTE_ACK& stSend)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_C2P_PROCESS_EXECUTE_ACK(stSend);
	}

	API_EXPORT BOOL uvPhilhmi_Send_C2P_SUBPROCESS_EXECUTE_ACK(STG_PP_C2P_SUBPROCESS_EXECUTE_ACK& stSend)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_C2P_SUBPROCESS_EXECUTE_ACK(stSend);
	}

	API_EXPORT BOOL uvPhilhmi_Send_C2P_STATUS_VALUE_ACK(STG_PP_C2P_STATUS_VALUE_ACK& stSend)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_C2P_STATUS_VALUE_ACK(stSend);
	}

	API_EXPORT BOOL uvPhilhmi_Send_C2P_MODE_CHANGE_ACK(STG_PP_C2P_MODE_CHANGE_ACK& stSend)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_C2P_MODE_CHANGE_ACK(stSend);
	}

	API_EXPORT BOOL uvPhilhmi_Send_C2P_EVENT_NOTIFY_ACK(STG_PP_C2P_EVENT_NOTIFY_ACK& stSend)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_C2P_EVENT_NOTIFY_ACK(stSend);
	}

	API_EXPORT BOOL uvPhilhmi_Send_C2P_INITIAL_EXECUTE_ACK(STG_PP_C2P_INITIAL_EXECUTE_ACK& stSend)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_C2P_INITIAL_EXECUTE_ACK(stSend);
	}

	API_EXPORT BOOL uvPhilhmi_Send_C2P_EVENT_STATUS_ACK(STG_PP_C2P_EVENT_STATUS_ACK& stSend)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_C2P_EVENT_STATUS_ACK(stSend);
	}

	API_EXPORT BOOL uvPhilhmi_Send_C2P_TIME_SYNC_ACK(STG_PP_C2P_TIME_SYNC_ACK& stSend)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_C2P_TIME_SYNC_ACK(stSend);
	}

	API_EXPORT BOOL uvPhilhmi_Send_C2P_INTERRUPT_STOP_ACK(STG_PP_C2P_INTERRUPT_STOP_ACK& stSend)
	{
		return ePHILHMI_ERR_OK == g_pPhilhmi->Send_C2P_INTERRUPT_STOP_ACK(stSend);
	}

#ifdef __cplusplus
}
#endif