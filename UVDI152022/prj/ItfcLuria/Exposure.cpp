

/*
 desc : User Data Family : Exposure
*/

#include "pch.h"
#include "MainApp.h"
#include "Exposure.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : shmem	- [in]  Shared Memory (Luria Service)
		error	- [in]  각종 에러 정보를 관리하는 객체
 retn : None
*/
CExposure::CExposure(LPG_LDSM shmem, CCodeToStr *error)
	: CBaseFamily(shmem, error)
{
	/* 기본 Family ID 설정 */
	m_enFamily		= ENG_LUDF::en_exposure;
	/* Exposure Structure */
	m_pstExposure	= &shmem->exposure;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CExposure::~CExposure()
{
}


/* --------------------------------------------------------------------------------------------- */
/*                                    Packet Function (Make)                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Set the light source amplitude that will be used for the exposure
		Luria will set the given amplitude in the photo head(s) when printing starts
 parm : flag	- [in]  Function Type : Get or Set
		count	- [in]  Photohead count
		amp		- [in]	Amplitude of given light source number (or a one-dimension array pointer)
 		ph_no	- [in]  Photohead Number (1 based)
		led_no	- [in]  LED Number (1:365, 2:385, 3:395, 4:405)
 retn : Pointer where the packet buffer is stored
 note : Note that the number of available light sources is given by the command GetNumberOfLightSources
*/
PUINT8 CExposure::GetPktLightAmplitude(ENG_LPGS flag, UINT8 count, UINT16 (*amp)[4])
{
	UINT8 u8Body[32]= { NULL }, i, j;
	UINT32 u32Pkts	= 512;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		UINT16 u16Ampl	= 0x0000;

		/* Set the send packet */
		for (i=0x00; i<count; i++)
		{
			/* Set the index for photohead */
			u8Body[0]	= i + 1;
			for (j=0x00; j<4/*not MAX_LED*/; j++)
			{
				/* Set the index for led of photohead */
				u8Body[1]	= j + 1;
				/* Set the power of led */
				u16Ampl		= SWAP16(amp[i][j]);
				memcpy(u8Body+2, &u16Ampl, 2);
				pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCEP::en_light_amplitude,
									  pPktNext, u32Pkts-m_u32PktSize, u8Body, 4);
			}
		}
	}

	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		/* Set the send packet */
		for (i=0x00; i<count; i++)
		{
			/* Set the index for photohead */
			u8Body[0]	= i + 1;
			for (j=0x00; j<4/*not MAX_LED*/; j++)
			{
				/* Set the index for led of photohead */
				u8Body[1]	= j + 1;
				pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCEP::en_light_amplitude,
									  pPktNext, u32Pkts-m_u32PktSize, u8Body, 2);
			}
		}
	}
	/* Returns the packet buffer */
	return pPktBuff;
}
PUINT8 CExposure::GetPktLightAmplitude(ENG_LPGS flag, UINT8 ph_no, ENG_LLPI led_no, UINT16 amp)
{
	UINT8 u8Body[8]	= { ph_no, UINT8(led_no), };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		amp	= SWAP16(amp);
		memcpy(u8Body+2, &amp, 2);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCEP::en_light_amplitude,
								pPktNext, u32Pkts, u8Body, 4);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCEP::en_light_amplitude,
								pPktNext, u32Pkts-m_u32PktSize, u8Body, 2);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Set the scroll step size to be used for the exposure
 parm : flag	- [in]  Function Type : Get or Set
		step	- [in]  1 = Step size 1, 2 = Step size 2, etc. Default 1
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CExposure::GetPktScrollStepSize(ENG_LPGS flag, UINT8 step)
{
	UINT8 u8Body[8]	= { step, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCEP::en_scroll_step_size,
							  pPktNext, u32Pkts, u8Body, 1);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCEP::en_scroll_step_size,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Set the frame rate factor to be used for the exposure
		The factor will reduce the exposure speed by the given factor. Eg factor 0,5 (given as 500)
		will reduce the speed to half.
 parm : flag	- [in]  Function Type : Get or Set
		rate	- [in]  Set frame rate factor. 0 < FrameRateFactor/1000 <= 1, ie. range is 1 – 1000. Default 1000
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CExposure::GetPktFrameRateFactor(ENG_LPGS flag, UINT16 rate)
{
	UINT8 u8Body[8]	= { NULL };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		rate	= SWAP16(rate);
		memcpy(u8Body, &rate, 2);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCEP::en_frame_rate_factor,
							  pPktNext, u32Pkts, u8Body, 2);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCEP::en_frame_rate_factor,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Set the desired duty cycle of the LEDs in the photo head
		Luria will set the LED on-time in all LEDs when starting an exposure
		LED ontime = (1 / (ScrollRate * ScrollStepSize * FrameRateFactor)) * LedDutyCycle/100
 parm : flag	- [in]  Function Type : Get or Set
		duty	- [in]  LED duty cycle. Range 0 – 100%. Default 50
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CExposure::GetPktLedDutyCycle(ENG_LPGS flag, UINT8 duty)
{
	UINT8 u8Body[8]	= { duty, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCEP::en_led_duty_cycle,
							  pPktNext, u32Pkts, u8Body, 1);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCEP::en_led_duty_cycle,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Prepare for a new print. This will make “Print” starting immediately when called later
		This command can be run in “shadow” time
		The PrePrint status can be queried using GetExposureStatus
		The current settings (led amplitude, scroll step size, duty cycle, etc) are set in the photo heads during pre-print.
 parm : None
 retn : Pointer where the packet buffer is stored
 note : The time taken before a reply is given to this command depends on the complexity of the artwork
		and how many registration points there are, etc
		Hence, the command may not return immediately.
*/
PUINT8 CExposure::GetPktPrePrint()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCEP::en_pre_print, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Start a print of the currently selected job
		Requires PrePrint to be run in advance. Status during printing can be queried using GetExposureState
		or by reading incoming announcements
 parm : None
 retn : Pointer where the packet buffer is stored
 note : This command always returns as soon as the print is initiated (either with OK or an error)
		Any errors occurring during print process is sent as an asynchronous announcement error at any time
*/
PUINT8 CExposure::GetPktPrint()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCEP::en_print, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Abort the current action
 parm : None
 retn : Pointer where the packet buffer is stored
 note : It may take some time until the abort-operation is completed
		This can be checked using GetExposureState, see Table 1 below
*/
PUINT8 CExposure::GetPktAbort()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCEP::en_abort, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Get the current exposure state. If state is “PrintRunning”,
		then information about the current motion is given
 parm : None
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CExposure::GetPktGetExposureState()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCEP::en_get_exposure_state, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Get the motion speed that will be used (or that is being used if printing)
		This speed is calculated as follows: speed (meter/sec) = ScrollRate * PixelSize * ScrollStepSize * FrameRateFactor
 parm : None
 retn : Pointer where the packet buffer is stored
 note : speed <= MaxMotionSpeed
*/
PUINT8 CExposure::GetPktGetExposureSpeed()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCEP::en_get_exposure_speed, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Reset the exposure state for the currently selected job irrespective of current state
 parm : None
 retn : Pointer where the packet buffer is stored
 note : Not implemented !!!
*/
PUINT8 CExposure::GetPktResetExposureState()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCEP::en_reset_exposure_state, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : When function is enabled, the photo head printing the last strip will copy
		the autofocus profile of the second to last strip and use that profile for focusing the last strip
		If autofocus is disabled in the photo head, then this function is bypassed.
 parm : flag	- [in]  Function Type : Get or Set
		enable	- [in]  Enable AF copy function. 0 = Disable (default), 1 = Enable
 retn : Pointer where the packet buffer is stored
 note : Valid from Luria version 2.1.0 and newer
*/
PUINT8 CExposure::GetPktEnableAfCopyLastStrip(ENG_LPGS flag, UINT8 enable)
{
	UINT8 u8Body[8]	= { enable, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 1, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCEP::en_enable_af_copy_last_strip,
							  pPktNext, u32Pkts, u8Body, 1);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCEP::en_enable_af_copy_last_strip,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Start a simulated print
		The selected job will be printed to a file instead of exposed through the photo head(s)
		The resulting output image will be written to the output directory given in MachineConfig::PrintSimulationOutDir
 parm : None
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CExposure::GetPktSimulatePrint()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCEP::en_simulate_print, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : This will move the active table to its exposure start position
 parm : None
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CExposure::GetPktMoveTableToExposureStartPos()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCEP::en_move_table_to_exposure_start_pos, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Get the number of available light sources for the given photo head
 parm : ph_no	- [in]  Photohead number (1 - 8)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CExposure::GetPktGetNumberOfLightSources(UINT8 ph_no)
{
	UINT8 u8Body[8]	= { ph_no, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCEP::en_get_number_of_light_sources,
						  pPktNext, u32Pkts, u8Body, 1);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Get the status of the requested light source. See tables below for detailed information
 parm : ph_no	- [in]  Photohead number (1 - 8)
		led_no	- [in]  Light source number (1 – 4)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CExposure::GetPktGetLightSourceStatus(UINT8 ph_no, ENG_LLPI led_no)
{
	UINT8 u8Body[8]	= { ph_no, UINT8(led_no), };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCEP::en_get_light_source_status,
						  pPktNext, u32Pkts, u8Body, 2);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : This command makes it possible to turn off autofocus for the first strip of a print,
		for example if this strip is outside the panel
		If autofocus is disabled in the photo head, then this function is bypassed
 parm : flag	- [in]  Function Type : Get or Set
		disable	- [in]  Disable autofocus for the first strip for the first photohead.
						1 = Disable autofocus first strip
						0 = Do not disable autofocus first strip (default).
 retn : Pointer where the packet buffer is stored
 note : Valid from Luria version 2.2.0 and newer
*/
PUINT8 CExposure::GetPktDisableAfFirstStrip(ENG_LPGS flag, UINT8 disable)
{
	UINT8 u8Body[8]	= { disable, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 2, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCEP::en_disable_af_first_strip,
							  pPktNext, u32Pkts, u8Body, 1);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCEP::en_disable_af_first_strip,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : The selected AF sensor type and laser settings are set in the photo head(s)
		when running PrePrint. Note that these settings only apply if autofocus is used
 parm : flag	- [in]  Function Type : Get or Set
		type	- [in]  Type of AF sensor in photo head(s): 1 = Diffuse (default), 2 = Specular
		agc		- [in]  Laser AGC (automatic gain control) setting
						1 = AGC on (default), 0 = AGC off (value in PWM-field is used)
						2 = AGC off use measured PWM value (require run AFSensorMeasureLaserPWM cmd first)
		pwm		- [in]  PWM-value for laser (only used when AGC is off, ignored when AGC is on)
						Range 2 – 511. Default = 100
 retn : Pointer where the packet buffer is stored
 note : AFSensorMeasureLaserPWM 명령어 사용하기 전에 이 명령으로 AF Sensor 값 설정
		이 값 설정 시점은 PrePrint 실행할 때, Photohead 값이 설정 (저장) 됨
		다만, 이 값들은 Autofocus가 사용될 때만 적용됨
		Valid from Luria version 2.3.0 and newer
*/
PUINT8 CExposure::GetPktAFSensor(ENG_LPGS flag, UINT8 type, UINT8 agc, UINT16 pwm)
{
	UINT8 u8Body[8]	= { type, agc, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 3, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pwm	= SWAP16(pwm);
		memcpy(u8Body+2, &pwm, 2);
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCEP::en_af_sensor,
							  pPktNext, u32Pkts, u8Body, 4);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCEP::en_af_sensor,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : This command should be sent to Luria if the current light amplitude is significantly higher/lower
		than the amplitude used in the previous print
		The command results in a short light-output from the photo heads
 parm : None
 retn : Pointer where the packet buffer is stored
 note : This command must be sent after sending LightAmplitude command but before PrePrint
		This command is not valid for RLT systems
		If sending this command when RLT heads are connected, then an error message will be given
		Valid from Luria version 2.4.2 and newer
*/
PUINT8 CExposure::GetPktLightRampUpDown()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 4, 2, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCEP::en_light_ramp_up_down, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Function returns optimal laser PWM by using AGC function
		The measured values are saved in Luria for current job
		When new job is loaded new measurements have to be taken
		AFSensor uses the measured values in case Laser AGC setting = 2
		(see description on command 0x11 AFSensor)
		AF has to be activated and all photo heads have to be in focus inside the panel area
 parm : None
 retn : Pointer where the packet buffer is stored
 note : Valid from Luria version 2.6.0 and newer
*/
PUINT8 CExposure::GetPktAFSensorMeasureLaserPWM()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 6, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCEP::en_af_sensor_measure_laser_pwm, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Get the light source type and status of all light sources in all photo heads
		See table below for detailed information on the status word
		(This command replaces the need of sending multiple GetLightSourceStatus commands)
 parm : None
 retn : Pointer where the packet buffer is stored
 note : Valid from Luria version 2.12.0 and newer
*/
PUINT8 CExposure::GetPktGetLightSourceStatusMulti()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 12, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCEP::en_light_source_status_multi, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/* --------------------------------------------------------------------------------------------- */
/*                                    Packet Function (Save)                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Processing received data
 parm : uid		- [in]  User ID
		data	- [in]  Packet buffer
		size	- [in]  The size of 'data'
 retn : None
*/
VOID CExposure::SetRecvPacket(UINT8 uid, PUINT8 data, UINT32 size)
{
	switch (uid)
	{
	case ENG_LCEP::en_light_amplitude				:
	case ENG_LCEP::en_scroll_step_size				:
	case ENG_LCEP::en_frame_rate_factor				:
	case ENG_LCEP::en_led_duty_cycle				:
	case ENG_LCEP::en_get_exposure_state			:
	case ENG_LCEP::en_get_light_source_status		:
	case ENG_LCEP::en_af_sensor						:
	case ENG_LCEP::en_af_sensor_measure_laser_pwm	:
	case ENG_LCEP::en_light_source_status_multi		:
	case ENG_LCEP::en_get_number_of_light_sources	:
	case ENG_LCEP::en_enable_af_copy_last_strip		:
	case ENG_LCEP::en_disable_af_first_strip		:
	case ENG_LCEP::en_get_exposure_speed			:	SetRecvExposureEtcBytes(uid, data, size);	break;
	}
}

/*
 desc : Processing received data
 parm : uid		- [in]  User ID
		data	- [in]  Packet buffer
		size	- [in]  The size of 'data'
 retn : None
*/
VOID CExposure::SetRecvExposureEtcBytes(UINT8 uid, PUINT8 data, UINT32 size)
{
	UINT8 u8Data[2]	= {NULL};
	UINT16 u16Data	= 0;
	UINT32 u32Data	= 0;
	PUINT8 pData	= data;

	switch (uid)
	{
	case ENG_LCEP::en_light_amplitude				:
		memcpy(u8Data, pData, 2);	pData	+= 2;	/* Photohead Number (1 ~ 8), Light Led Number (1 ~ 4) */
		memcpy(&u16Data, pData, 2);
		m_pstExposure->led_amplitude_power[u8Data[0]-1][u8Data[1]-1] = SWAP16(u16Data);	break;
	case ENG_LCEP::en_scroll_step_size				:
		m_pstExposure->scroll_step_size		= pData[0];									break;
	case ENG_LCEP::en_frame_rate_factor				:
		memcpy(&u16Data, pData, 2);	pData	+= 2;
		m_pstExposure->frame_rate_factor	= SWAP16(u16Data);							break;
	case ENG_LCEP::en_led_duty_cycle				:
		m_pstExposure->led_duty_cycle		= pData[0];									break;
	case ENG_LCEP::en_get_exposure_state			:
		memcpy(m_pstExposure->get_exposure_state, pData, 3);							break;
	case ENG_LCEP::en_get_exposure_speed			:
		memcpy(&u32Data, pData, 4);	pData	+= 4;
		m_pstExposure->get_exposure_speed	= SWAP32(u32Data);							break;;
	case ENG_LCEP::en_get_light_source_status		:
		m_pstExposure->light_source_status.led_connect[pData[0]][pData[1]]	= pData[3];	pData+=3;
		memcpy(&m_pstExposure->light_source_status.status[pData[0]][pData[1]], pData, 4);	break;
	case ENG_LCEP::en_get_number_of_light_sources	:
		//m_pstExposure
		break;
	case ENG_LCEP::en_af_sensor						:
		m_pstExposure->af_sensor.sensor_type= pData[0];	pData++;
		m_pstExposure->af_sensor.agc_type	= pData[0];	pData++;
		memcpy(&u16Data, pData, 2);
		m_pstExposure->af_sensor.agc_value	= SWAP16(u16Data);							break;
	case ENG_LCEP::en_af_sensor_measure_laser_pwm	:
		m_pstExposure->af_pwm.meas_count= pData[0];	pData++;
		memcpy(m_pstExposure->af_pwm.pwm_value,	pData, sizeof(UINT16) * data[0]);
		m_pstExposure->af_pwm.SWAP();													break;
	case ENG_LCEP::en_light_source_status_multi		:
		m_pstExposure->light_source_status.ph_count	= pData[0];	pData++;
		m_pstExposure->light_source_status.led_count= pData[0];	pData++;
		m_pstExposure->light_source_status.led_type	= pData[0];	pData++;
		memcpy(m_pstExposure->light_source_status.status, pData, sizeof(UINT32)*data[0]*data[1]);
		m_pstExposure->light_source_status.SWAP();
		SaveLightSourceStatusMulti();													break;
	case ENG_LCEP::en_enable_af_copy_last_strip		:
		m_pstExposure->enable_af_copy_last_strip	= pData[0];	pData++;				break;
	case ENG_LCEP::en_disable_af_first_strip		:
		m_pstExposure->disable_af_first_strip	= pData[0];	pData++;					break;
	}
}

/*
 desc : Announcement로부터 수신된 노광 상태 값 갱신
 parm : data	- [in]  3개의 상태 값이 저장되어 있음
 retn : None
*/
VOID CExposure::SetExposureState(PUINT8 data)
{
	memcpy(m_pstExposure->get_exposure_state, data, 3);
}

/*
 desc : LightSourceStatusMulti 관련 상태 값 분석 후 로그 파일로 저장
 parm : None
 retn : None
*/
VOID CExposure::SaveLightSourceStatusMulti()
{
	UINT8 i	= 0, j	= 0;
	UINT8 u8PhCount	= m_pstExposure->light_source_status.ph_count;
	UINT8 u8LedCount= m_pstExposure->light_source_status.led_count;
	UINT32 (*pStatus)[MAX_LED]	= m_pstExposure->light_source_status.status;

	for (; i<u8PhCount; i++)
	{
		for (j=0; j<u8LedCount; j++)
		{
			if (0 != pStatus[i][j])
			{
				TCHAR tzMesg[128]	= {NULL};
				if ((pStatus[0][0] & 0x00000001))
				{
					swprintf_s(tzMesg, 128, L"PH[%u][%u] : Light source is connected [%08x]", i, j, pStatus[0][0]);
					LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
				}
				if ((pStatus[0][0] & 0x0000002))
				{
					swprintf_s(tzMesg, 128, L"PH[%u][%u] : Door open [%08x]", i, j, pStatus[0][0]);
					LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
				}
				if ((pStatus[0][0] & 0x0000004))
				{
					swprintf_s(tzMesg, 128, L"PH[%u][%u] : Light source driver board temperature out of range [%08x]", i, j, pStatus[0][0]);
					LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
				}
				if ((pStatus[0][0] & 0x0000008))
				{
					swprintf_s(tzMesg, 128, L"PH[%u][%u] : Light source temperature out of range [%08x]", i, j, pStatus[0][0]);
					LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
				}
				if ((pStatus[0][0] & 0x0000010))
				{
					swprintf_s(tzMesg, 128, L"PH[%u][%u] : Duty-cycle limitation [%08x]", i, j, pStatus[0][0]);
					LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
				}
				if ((pStatus[0][0] & 0x0000020))
				{
					swprintf_s(tzMesg, 128, L"PH[%u][%u] : Over-current protection [%08x]", i, j, pStatus[0][0]);
					LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
				}
				if ((pStatus[0][0] & 0x0000040))
				{
					swprintf_s(tzMesg, 128, L"PH[%u][%u] : Over-power protection [%08x]", i, j, pStatus[0][0]);
					LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
				}
			}
		}
	}
}
