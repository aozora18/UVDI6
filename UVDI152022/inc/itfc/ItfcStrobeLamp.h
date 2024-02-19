
/*
 desc : Trigger Communication Libarary
*/

#pragma once

#include "../conf/global.h"
#include "../conf/strobe_lamp_uvdi15.h"


#ifdef __cplusplus
extern "C"
{
#endif

	API_IMPORT BOOL uvStrobeLamp_Init(HWND hWnd, LPG_CIEA config, LPG_SLPR shmem, LPG_DLSM link);
	API_IMPORT BOOL uvStrobeLamp_Open(LPG_CIEA config, LPG_SLPR shmem);
	API_IMPORT VOID uvStrobeLamp_Close();
	API_IMPORT BOOL uvStrobeLamp_Reconnected();
	API_IMPORT BOOL uvStrobeLamp_IsConnected();

	API_IMPORT int uvStrobeLamp_Send_ChannelDelayControl(const uint8_t page, const uint8_t channel, const uint16_t delayValue, int nTimeout = 10000);
	API_IMPORT int uvStrobeLamp_Send_ChannelStrobeControl(const uint8_t page, const uint8_t channel, const uint16_t strobeValue, int nTimeout = 10000);
	API_IMPORT int uvStrobeLamp_Send_ChannelWrite(const uint8_t page, const uint8_t channel, const uint16_t delayValue, const uint16_t strobeValue, int nTimeout = 10000);
	API_IMPORT int uvStrobeLamp_Send_PageDataWrite(const uint8_t page, const uint8_t channelCount, const uint16_t* delayValues, const uint16_t* strobeValues, int nTimeout = 10000);
	API_IMPORT int uvStrobeLamp_Send_PageDataReadRequest(const uint8_t page, int nTimeout = 10000);
	API_IMPORT int uvStrobeLamp_Send_PageLoopDataWrite(const uint8_t page, const uint8_t loopDelay3Value, const uint8_t loopDelay2Value, const uint8_t loopDelay1Value, const uint8_t loopDelay0Value, const uint8_t loopCount, int nTimeout = 10000);
	API_IMPORT int uvStrobeLamp_Send_PageLoopDataRead(const uint8_t page, int nTimeout = 10000);
	API_IMPORT int uvStrobeLamp_Send_StrobeMode(const uint8_t mode, int nTimeout = 10000);
	API_IMPORT int uvStrobeLamp_Send_TriggerMode(const uint8_t mode, int nTimeout = 10000);
	API_IMPORT int uvStrobeLamp_Send_DataSave(int nTimeout = 10000);
	API_IMPORT int uvStrobeLamp_Send_DataLoad(int nTimeout = 10000);
	API_IMPORT int uvStrobeLamp_Send_StrobeTriggerModeRead(int nTimeout = 10000);
#ifdef __cplusplus
}
#endif