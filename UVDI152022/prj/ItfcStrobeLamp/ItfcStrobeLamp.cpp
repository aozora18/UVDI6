
/*
 desc : StrobeLamp Communication Libarary (StrobeLamp Protocol)
*/

#include "pch.h"
#include "MainApp.h"
#include "StrobeLampLib.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                           전역 변수                                           */
/* --------------------------------------------------------------------------------------------- */

CStrobeLampLib* g_pStrobeLamp = NULL;		// 데이터 수집 및 감시 스레드

/* 공유 메모리 */
STG_CTCS					g_stCommInfo = { NULL };
HWND						g_hwnd = NULL;

/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

BOOL IsConnected()
{
	if (!g_pStrobeLamp)				return FALSE;

	return (g_pStrobeLamp->IsConnect());
}


/* --------------------------------------------------------------------------------------------- */
/*                                           외부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

	API_EXPORT BOOL uvStrobeLamp_Init(HWND hWnd, LPG_CIEA config, LPG_SLPR shmem, LPG_DLSM link)
	{
		/* All Configuration */
		g_pstConfig = config;
		g_hwnd = hWnd;
		/* 구조체에 통신 관련 정보 설정 */
		g_stCommInfo.tcp_port = g_pstConfig->set_comm.strb_lamp_port;
		g_stCommInfo.source_ipv4 = uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.cmps_ipv4);
		g_stCommInfo.target_ipv4 = uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.strb_lamp_ipv4);
		g_stCommInfo.port_buff = g_pstConfig->set_comm.port_buff;
		g_stCommInfo.recv_buff = g_pstConfig->set_comm.recv_buff;
		g_stCommInfo.ring_buff = g_pstConfig->set_comm.ring_buff;
		g_stCommInfo.link_time = g_pstConfig->set_comm.link_time;
		g_stCommInfo.idle_time = g_pstConfig->set_comm.idle_time;
		g_stCommInfo.sock_time = g_pstConfig->set_comm.sock_time;
		g_stCommInfo.live_time = g_pstConfig->set_comm.live_time;

		// Client 스레드 생성 및 동작 시킴
		g_pStrobeLamp = new CStrobeLampLib();
		//hWnd, 
		if (!g_pStrobeLamp)	return FALSE;
		if (!g_pStrobeLamp->Init(hWnd, &g_stCommInfo, &g_pstConfig->set_comn))
		{
			delete g_pStrobeLamp;
			g_pStrobeLamp = NULL;
			return FALSE;
		}

		return TRUE;
	}

	API_EXPORT BOOL uvStrobeLamp_Open(LPG_CIEA config, LPG_PPR shmem)
	{
		LPG_CTSP pstStrobeLamp = NULL;

		/* All Configuration */
		g_pstConfig = config;

		if (!g_pStrobeLamp)	return FALSE;
		if (!g_pStrobeLamp->Open())
		{
			delete g_pStrobeLamp;
			g_pStrobeLamp = NULL;
			return FALSE;
		}

		return TRUE;
	}

	API_EXPORT VOID uvStrobeLamp_Close()
	{
		// 스레드 정지 시키는 명령 전달 후에 정지될 때까지 대기
		if (g_pStrobeLamp)
		{
			/* 약 데이터 송신될 때까지 대기 */
			Sleep(500);
			// 스레드 중지 호출
			g_pStrobeLamp->Close();
			delete g_pStrobeLamp;
			g_pStrobeLamp = NULL;
		}
	}

	API_EXPORT BOOL uvStrobeLamp_Reconnected()
	{
		g_stCommInfo.tcp_port = g_pstConfig->set_comm.strb_lamp_port;
		g_stCommInfo.source_ipv4 = uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.cmps_ipv4);
		g_stCommInfo.target_ipv4 = uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.strb_lamp_ipv4);
		g_stCommInfo.port_buff = g_pstConfig->set_comm.port_buff;
		g_stCommInfo.recv_buff = g_pstConfig->set_comm.recv_buff;
		g_stCommInfo.ring_buff = g_pstConfig->set_comm.ring_buff;
		g_stCommInfo.link_time = g_pstConfig->set_comm.link_time;
		g_stCommInfo.idle_time = g_pstConfig->set_comm.idle_time;
		g_stCommInfo.sock_time = g_pstConfig->set_comm.sock_time;
		g_stCommInfo.live_time = g_pstConfig->set_comm.live_time;

		if (!g_pStrobeLamp)	return FALSE;
		if (!g_pStrobeLamp->Init(g_hwnd, &g_stCommInfo, &g_pstConfig->set_comn))
		{
			delete g_pStrobeLamp;
			g_pStrobeLamp = NULL;
			return FALSE;
		}

		return TRUE;
	}

	API_EXPORT BOOL uvStrobeLamp_IsConnected()
	{
		return IsConnected();
	}

	API_EXPORT int uvStrobeLamp_Send_ChannelDelayControl(const uint8_t page, const uint8_t channel, const uint16_t delayValue, int nTimeout /*= 10000*/)
	{
		return g_pStrobeLamp->Send_ChannelDelayControl(page, channel, delayValue, nTimeout);
	}

	API_EXPORT int uvStrobeLamp_Send_ChannelStrobeControl(const uint8_t page, const uint8_t channel, const uint16_t strobeValue, int nTimeout /*= 10000*/)
	{
		return g_pStrobeLamp->Send_ChannelStrobeControl(page, channel, strobeValue, nTimeout);
	}

	API_EXPORT int uvStrobeLamp_Send_ChannelWrite(const uint8_t page, const uint8_t channel, const uint16_t delayValue, const uint16_t strobeValue, int nTimeout /*= 10000*/)
	{
		return g_pStrobeLamp->Send_ChannelWrite(page, channel, delayValue, strobeValue, nTimeout);
	}

	API_EXPORT int uvStrobeLamp_Send_PageDataWrite(const uint8_t page, const uint8_t channelCount, const uint16_t* delayValues, const uint16_t* strobeValues, int nTimeout /*= 10000*/)
	{
		return g_pStrobeLamp->Send_PageDataWrite(page, channelCount, delayValues, strobeValues, nTimeout);
	}

	API_EXPORT int uvStrobeLamp_Send_PageDataReadRequest(const uint8_t page, int nTimeout /*= 10000*/)
	{
		return g_pStrobeLamp->Send_PageDataReadRequest(page, nTimeout);
	}

	API_EXPORT int uvStrobeLamp_Send_PageLoopDataWrite(const uint8_t page, const uint8_t loopDelay3Value, const uint8_t loopDelay2Value, const uint8_t loopDelay1Value, const uint8_t loopDelay0Value, const uint8_t loopCount, int nTimeout /*= 10000*/)
	{
		return g_pStrobeLamp->Send_PageLoopDataWrite(page, loopDelay3Value, loopDelay2Value, loopDelay1Value, loopDelay0Value, loopCount, nTimeout);
	}

	API_EXPORT int uvStrobeLamp_Send_PageLoopDataRead(const uint8_t page, int nTimeout /*= 10000*/)
	{
		return g_pStrobeLamp->Send_PageLoopDataRead(page, nTimeout);
	}

	API_EXPORT int uvStrobeLamp_Send_StrobeMode(const uint8_t mode, int nTimeout /*= 10000*/)
	{
		return g_pStrobeLamp->Send_StrobeMode(mode, nTimeout);
	}

	API_EXPORT int uvStrobeLamp_Send_TriggerMode(const uint8_t mode, int nTimeout /*= 10000*/)
	{
		return g_pStrobeLamp->Send_TriggerMode(mode, nTimeout);
	}

	API_EXPORT int uvStrobeLamp_Send_DataSave(int nTimeout /*= 10000*/)
	{
		return g_pStrobeLamp->Send_DataSave(nTimeout);
	}

	API_EXPORT int uvStrobeLamp_Send_DataLoad(int nTimeout /*= 10000*/)
	{
		return g_pStrobeLamp->Send_DataLoad(nTimeout);
	}

	API_EXPORT int uvStrobeLamp_Send_StrobeTriggerModeRead(int nTimeout /*= 10000*/)
	{
		return g_pStrobeLamp->Send_StrobeTriggerModeRead(nTimeout);
	}

#ifdef __cplusplus
}
#endif