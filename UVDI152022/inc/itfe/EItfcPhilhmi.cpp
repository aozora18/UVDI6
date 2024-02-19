
/*
 desc : The Interface Library for Philoptics's Philhmi Interface
*/

#include "pch.h"
#include "../../inc/conf/philhmi_uvdi15.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */


#ifdef __cplusplus
extern "C"
{
#endif

	/* --------------------------------------------------------------------------------------------- */
	/*                             외부 함수 - < Philhmi >  < for Engine >                           */
	/* --------------------------------------------------------------------------------------------- */

	API_EXPORT BOOL uvEng_Philhmi_Reconnected()
	{
		return uvPhilhmi_Reconnected();
	}

	API_EXPORT BOOL uvEng_Philhmi_IsConnected()
	{
		return uvPhilhmi_IsConnected();
	}

	API_EXPORT BOOL uvEng_Philhmi_IsSendCmdCountUnder(UINT16 count)
	{
		return uvPhilhmi_IsSendCmdCountUnder(count);
	}


	API_EXPORT BOOL uvEng_Philhmi_GetRecvDataFromCommand(int nCommand, STG_PP_PACKET_RECV* stRecv)
	{
		return uvPhilhmi_GetRecvDataFromCommand(nCommand, stRecv);
	}

	API_EXPORT BOOL uvEng_Philhmi_GetRecvDataFromUniqueID(unsigned int nUniqueID, STG_PP_PACKET_RECV* stRecv)
	{
		return uvPhilhmi_GetRecvDataFromUniqueID(nUniqueID, stRecv);
	}

	API_EXPORT BOOL uvEng_Philhmi_GetRecvWaitFromUniqueID(unsigned int nUniqueID, STG_PP_PACKET_RECV* stRecv, int nTimeout)
	{
		return uvPhilhmi_GetRecvWaitFromUniqueID(nUniqueID, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvEng_uvPhilhmi_IsSyncResultLocked()
	{
		return uvPhilhmi_IsSyncResultLocked();
	}

	API_EXPORT BOOL uvEng_Philhmi_DeleteRecvDataFromUniqueID(unsigned int nUniqueID)
	{
		return uvPhilhmi_DeleteRecvDataFromUniqueID(nUniqueID);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_P2C_RCP_CREATE(STG_PP_P2C_RCP_CREATE& stSend, STG_PP_P2C_RCP_CREATE_ACK& stRecv, int nTimeout)
	{
		return uvPhilhmi_Send_P2C_RCP_CREATE(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_P2C_RCP_DELETE(STG_PP_P2C_RCP_DELETE& stSend, STG_PP_P2C_RCP_DELETE_ACK& stRecv, int nTimeout)
	{
		return uvPhilhmi_Send_P2C_RCP_DELETE(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_P2C_RCP_MODIFY(STG_PP_P2C_RCP_MODIFY& stSend, STG_PP_P2C_RCP_MODIFY_ACK& stRecv, int nTimeout)
	{
		return uvPhilhmi_Send_P2C_RCP_MODIFY(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_P2C_RCP_SELECT(STG_PP_P2C_RCP_SELECT& stSend, STG_PP_P2C_RCP_SELECT_ACK& stRecv, int nTimeout)
	{
		return uvPhilhmi_Send_P2C_RCP_SELECT(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_P2C_IO_STATUS(STG_PP_P2C_IO_STATUS& stSend, STG_PP_P2C_IO_STATUS_ACK& stRecv, int nTimeout)
	{
		return uvPhilhmi_Send_P2C_IO_STATUS(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_P2C_OUTPUT_ONOFF(STG_PP_P2C_OUTPUT_ONOFF& stSend, STG_PP_P2C_OUTPUT_ONOFF_ACK& stRecv, int nTimeout)
	{
		return uvPhilhmi_Send_P2C_OUTPUT_ONOFF(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_P2C_ABS_MOVE(STG_PP_P2C_ABS_MOVE& stSend, STG_PP_P2C_ABS_MOVE_ACK& stRecv, int nTimeout)
	{
		return uvPhilhmi_Send_P2C_ABS_MOVE(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_P2C_ABS_MOVE_COMP(STG_PP_P2C_ABS_MOVE_COMP& stSend, STG_PP_P2C_ABS_MOVE_COMP_ACK& stRecv, int nTimeout)
	{
		return uvPhilhmi_Send_P2C_ABS_MOVE_COMP(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_P2C_REL_MOVE(STG_PP_P2C_REL_MOVE& stSend, STG_PP_P2C_REL_MOVE_ACK& stRecv, int nTimeout)
	{
		return uvPhilhmi_Send_P2C_REL_MOVE(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_P2C_REL_MOVE_COMP(STG_PP_P2C_REL_MOVE_COMP& stSend, STG_PP_P2C_REL_MOVE_COMP_ACK& stRecv, int nTimeout)
	{
		return uvPhilhmi_Send_P2C_REL_MOVE_COMP(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_P2C_CHAR_MOVE(STG_PP_P2C_CHAR_MOVE& stSend, STG_PP_P2C_CHAR_MOVE_ACK& stRecv, int nTimeout)
	{
		return uvPhilhmi_Send_P2C_CHAR_MOVE(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_P2C_CHAR_MOVE_COMP(STG_PP_P2C_CHAR_MOVE_COMP& stSend, STG_PP_P2C_CHAR_MOVE_COMP_ACK& stRecv, int nTimeout)
	{
		return uvPhilhmi_Send_P2C_CHAR_MOVE_COMP(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_P2C_PROCESS_STEP(STG_PP_P2C_PROCESS_STEP& stSend, int nTimeout)
	{
		return uvPhilhmi_Send_P2C_PROCESS_STEP(stSend, nTimeout);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_P2C_PROCESS_COMP(STG_PP_P2C_PROCESS_COMP& stSend, STG_PP_P2C_PROCESS_COMP_ACK& stRecv, int nTimeout)
	{
		return uvPhilhmi_Send_P2C_PROCESS_COMP(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_P2C_SUBPROCESS_COMP(STG_PP_P2C_SUBPROCESS_COMP& stSend, STG_PP_P2C_SUBPROCESS_COMP_ACK& stRecv, int nTimeout)
	{
		return uvPhilhmi_Send_P2C_SUBPROCESS_COMP(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_P2C_EC_MODIFY(STG_PP_P2C_EC_MODIFY& stSend, STG_PP_P2C_EC_MODIFY_ACK& stRecv, int nTimeout)
	{
		return uvPhilhmi_Send_P2C_EC_MODIFY(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_P2C_INITIAL_COMPLETE(STG_PP_P2C_INITIAL_COMPLETE& stSend, STG_PP_P2C_INITIAL_COMPLETE_ACK& stRecv, int nTimeout)
	{
		return uvPhilhmi_Send_P2C_INITIAL_COMPLETE(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_P2C_ALARM_OCCUR(STG_PP_P2C_ALARM_OCCUR& stSend, STG_PP_P2C_ALARM_OCCUR_ACK& stRecv, int nTimeout)
	{
		return uvPhilhmi_Send_P2C_ALARM_OCCUR(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_P2C_EVENT_NOTIFY(STG_PP_P2C_EVENT_NOTIFY& stSend, STG_PP_P2C_EVENT_NOTIFY_ACK& stRecv, int nTimeout)
	{
		return uvPhilhmi_Send_P2C_EVENT_NOTIFY(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_P2C_INTERRUPT_STOP(STG_PP_P2C_INTERRUPT_STOP& stSend, STG_PP_P2C_INTERRUPT_STOP_ACK& stRecv, int nTimeout)
	{
		return uvPhilhmi_Send_P2C_INTERRUPT_STOP(stSend, stRecv, nTimeout);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_C2P_RCP_CREATE_ACK(STG_PP_C2P_RCP_CREATE_ACK& stSend)
	{
		return uvPhilhmi_Send_C2P_RCP_CREATE_ACK(stSend);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_C2P_RCP_DELETE_ACK(STG_PP_C2P_RCP_DELETE_ACK& stSend)
	{
		return uvPhilhmi_Send_C2P_RCP_DELETE_ACK(stSend);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_C2P_RCP_MODIFY_ACK(STG_PP_C2P_RCP_MODIFY_ACK& stSend)
	{
		return uvPhilhmi_Send_C2P_RCP_MODIFY_ACK(stSend);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_C2P_RCP_SELECT_ACK(STG_PP_C2P_RCP_SELECT_ACK& stSend)
	{
		return uvPhilhmi_Send_C2P_RCP_SELECT_ACK(stSend);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_C2P_RCP_LIST_ACK(STG_PP_C2P_RCP_LIST_ACK& stSend)
	{
		return uvPhilhmi_Send_C2P_RCP_LIST_ACK(stSend);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_C2P_RCP_INFO_ACK(STG_PP_C2P_RCP_INFO_ACK& stSend)
	{
		return uvPhilhmi_Send_C2P_RCP_INFO_ACK(stSend);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_C2P_ABS_MOVE_ACK(STG_PP_C2P_ABS_MOVE_ACK& stSend)
	{
		return uvPhilhmi_Send_C2P_ABS_MOVE_ACK(stSend);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_C2P_ABS_MOVE_COMP_ACK(STG_PP_C2P_ABS_MOVE_COMP_ACK& stSend)
	{
		return uvPhilhmi_Send_C2P_ABS_MOVE_COMP_ACK(stSend);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_C2P_REL_MOVE_ACK(STG_PP_C2P_REL_MOVE_ACK& stSend)
	{
		return uvPhilhmi_Send_C2P_REL_MOVE_ACK(stSend);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_C2P_REL_MOVE_COMP_ACK(STG_PP_C2P_REL_MOVE_COMP_ACK& stSend)
	{
		return uvPhilhmi_Send_C2P_REL_MOVE_COMP_ACK(stSend);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_C2P_CHAR_MOVE_ACK(STG_PP_C2P_CHAR_MOVE_ACK& stSend)
	{
		return uvPhilhmi_Send_C2P_CHAR_MOVE_ACK(stSend);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_C2P_CHAR_MOVE_COMP_ACK(STG_PP_C2P_CHAR_MOVE_COMP_ACK& stSend)
	{
		return uvPhilhmi_Send_C2P_CHAR_MOVE_COMP_ACK(stSend);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_C2P_PROCESS_EXECUTE_ACK(STG_PP_C2P_PROCESS_EXECUTE_ACK& stSend)
	{
		return uvPhilhmi_Send_C2P_PROCESS_EXECUTE_ACK(stSend);
	}
	API_EXPORT BOOL uvEng_Philhmi_Send_C2P_INITIAL_EXECUTE_ACK(STG_PP_C2P_INITIAL_EXECUTE_ACK& stSend)
	{
		return uvPhilhmi_Send_C2P_INITIAL_EXECUTE_ACK(stSend);
	}
	API_EXPORT BOOL uvEng_Philhmi_Send_C2P_SUBPROCESS_EXECUTE_ACK(STG_PP_C2P_SUBPROCESS_EXECUTE_ACK& stSend)
	{
		return uvPhilhmi_Send_C2P_SUBPROCESS_EXECUTE_ACK(stSend);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_C2P_STATUS_VALUE_ACK(STG_PP_C2P_STATUS_VALUE_ACK& stSend)
	{
		return uvPhilhmi_Send_C2P_STATUS_VALUE_ACK(stSend);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_C2P_MODE_CHANGE_ACK(STG_PP_C2P_MODE_CHANGE_ACK& stSend)
	{
		return uvPhilhmi_Send_C2P_MODE_CHANGE_ACK(stSend);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_C2P_EVENT_STATUS_ACK(STG_PP_C2P_EVENT_STATUS_ACK& stSend)
	{
		return uvPhilhmi_Send_C2P_EVENT_STATUS_ACK(stSend);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_C2P_EVENT_NOTIFY_ACK(STG_PP_C2P_EVENT_NOTIFY_ACK& stSend)
	{
		return uvPhilhmi_Send_C2P_EVENT_NOTIFY_ACK(stSend);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_C2P_TIME_SYNC_ACK(STG_PP_C2P_TIME_SYNC_ACK& stSend)
	{
		return uvPhilhmi_Send_C2P_TIME_SYNC_ACK(stSend);
	}

	API_EXPORT BOOL uvEng_Philhmi_Send_C2P_INTERRUPT_STOP_ACK(STG_PP_C2P_INTERRUPT_STOP_ACK& stSend)
	{
		return uvPhilhmi_Send_C2P_INTERRUPT_STOP_ACK(stSend);
	}


#ifdef __cplusplus
}
#endif