
/*
 desc : The Interface Library for ALT's StrobeLamp Interface
*/

#pragma once

#include "../conf/global.h"
#include "../conf/define.h"
#include "../../inc/conf/strobe_lamp_uvdi15.h"


#ifdef __cplusplus
extern "C"
{
#endif

	/* --------------------------------------------------------------------------------------------- */
	/*                             외부 함수 - < StrobeLamp >  < for Engine >                           */
	/* --------------------------------------------------------------------------------------------- */

	API_IMPORT BOOL uvEng_StrobeLamp_Reconnected();
	API_IMPORT BOOL uvEng_StrobeLamp_IsConnected();

	API_IMPORT int uvEng_StrobeLamp_Send_ChannelDelayControl(const uint8_t page, const uint8_t channel, const uint16_t delayValue, int nTimeout = 10000);
	API_IMPORT int uvEng_StrobeLamp_Send_ChannelStrobeControl(const uint8_t page, const uint8_t channel, const uint16_t strobeValue, int nTimeout = 10000);
	API_IMPORT int uvEng_StrobeLamp_Send_ChannelWrite(const uint8_t page, const uint8_t channel, const uint16_t delayValue, const uint16_t strobeValue, int nTimeout = 10000);
	API_IMPORT int uvEng_StrobeLamp_Send_PageDataWrite(const uint8_t page, const uint8_t channelCount, const uint16_t* delayValues, const uint16_t* strobeValues, const uint8_t* strobeLowValues, int nTimeout = 10000);
	API_IMPORT int uvEng_StrobeLamp_Send_PageDataReadRequest(const uint8_t page, int nTimeout = 10000);
	API_IMPORT int uvEng_StrobeLamp_Send_PageLoopDataWrite(const uint8_t page, const uint8_t loopDelay3Value, const uint8_t loopDelay2Value, const uint8_t loopDelay1Value, const uint8_t loopDelay0Value, const uint8_t loopCount, int nTimeout = 10000);
	API_IMPORT int uvEng_StrobeLamp_Send_PageLoopDataRead(const uint8_t page, int nTimeout = 10000);
	API_IMPORT int uvEng_StrobeLamp_Send_StrobeMode(const uint8_t mode, int nTimeout = 10000);
	API_IMPORT int uvEng_StrobeLamp_Send_TriggerMode(const uint8_t mode, int nTimeout = 10000);
	API_IMPORT int uvEng_StrobeLamp_Send_DataSave(int nTimeout = 10000);
	API_IMPORT int uvEng_StrobeLamp_Send_DataLoad(int nTimeout = 10000);
	API_IMPORT int uvEng_StrobeLamp_Send_StrobeTriggerModeRead(int nTimeout = 10000);

#ifdef __cplusplus
}
#endif