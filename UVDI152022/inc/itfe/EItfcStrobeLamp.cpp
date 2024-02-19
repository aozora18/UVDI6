
/*
 desc : The Interface Library for ALT's StrobeLamp Interface
*/

#include "pch.h"
#include "../../inc/conf/strobe_lamp_uvdi15.h"

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
	/*                             외부 함수 - < StrobeLamp >  < for Engine >                        */
	/* --------------------------------------------------------------------------------------------- */

	API_EXPORT BOOL uvEng_StrobeLamp_Reconnected()
	{
		return uvStrobeLamp_Reconnected();
	}

	API_EXPORT BOOL uvEng_StrobeLamp_IsConnected()
	{
		return uvStrobeLamp_IsConnected();
	}

	API_EXPORT int uvEng_StrobeLamp_Send_ChannelDelayControl(const uint8_t page, const uint8_t channel, const uint8_t delayValue, int nTimeout/* = 10000*/)
	{
		return uvStrobeLamp_Send_ChannelDelayControl(page, channel, delayValue, nTimeout);
	}

	API_EXPORT int uvEng_StrobeLamp_Send_ChannelStrobeControl(const uint8_t page, const uint8_t channel, const uint8_t strobeValue, int nTimeout /*= 10000*/)
	{
		return uvStrobeLamp_Send_ChannelStrobeControl(page, channel, strobeValue, nTimeout);
	}

	API_EXPORT int uvEng_StrobeLamp_Send_ChannelWrite(const uint8_t page, const uint8_t channel, const uint16_t delayValue, const uint16_t strobeValue, int nTimeout /*= 10000*/)
	{
		return uvStrobeLamp_Send_ChannelWrite(page, channel, delayValue, strobeValue, nTimeout);
	}

	API_EXPORT int uvEng_StrobeLamp_Send_PageDataWrite(const uint8_t page, const uint8_t channelCount, const uint16_t* delayValues, const uint16_t* strobeValues, int nTimeout /*= 10000*/)
	{
		return uvStrobeLamp_Send_PageDataWrite(page, channelCount, delayValues, strobeValues, nTimeout);
	}

	API_EXPORT int uvEng_StrobeLamp_Send_PageDataReadRequest(const uint8_t page, int nTimeout /*= 10000*/)
	{
		return uvStrobeLamp_Send_PageDataReadRequest(page, nTimeout);
	}

	API_EXPORT int uvEng_StrobeLamp_Send_PageLoopDataWrite(const uint8_t page, const uint8_t loopDelay3Value, const uint8_t loopDelay2Value, const uint8_t loopDelay1Value, const uint8_t loopDelay0Value, const uint8_t loopCount, int nTimeout /*= 10000*/)
	{
		return uvStrobeLamp_Send_PageLoopDataWrite(page, loopDelay3Value, loopDelay2Value, loopDelay1Value, loopDelay0Value, loopCount, nTimeout);
	}

	API_EXPORT int uvEng_StrobeLamp_Send_PageLoopDataRead(const uint8_t page, int nTimeout /*= 10000*/)
	{
		return uvStrobeLamp_Send_PageLoopDataRead(page, nTimeout);
	}

	API_EXPORT int uvEng_StrobeLamp_Send_StrobeMode(const uint8_t mode, int nTimeout /*= 10000*/)
	{
		return uvStrobeLamp_Send_StrobeMode(mode, nTimeout);
	}

	API_EXPORT int uvEng_StrobeLamp_Send_TriggerMode(const uint8_t mode, int nTimeout /*= 10000*/)
	{
		return uvStrobeLamp_Send_TriggerMode(mode, nTimeout);
	}

	API_EXPORT int uvEng_StrobeLamp_Send_DataSave(int nTimeout /*= 10000*/)
	{
		return uvStrobeLamp_Send_DataSave(nTimeout);
	}

	API_EXPORT int uvEng_StrobeLamp_Send_DataLoad(int nTimeout /*= 10000*/)
	{
		return uvStrobeLamp_Send_DataLoad(nTimeout);
	}

	API_EXPORT int uvEng_StrobeLamp_Send_StrobeTriggerModeRead(int nTimeout /*= 10000*/)
	{
		return uvStrobeLamp_Send_StrobeTriggerModeRead(nTimeout);
	}


#ifdef __cplusplus
}
#endif