
/*
 desc : User Data Family : Direct Photohead Comm
*/

#include "pch.h"
#include "MainApp.h"
#include "DirectPhComn.h"


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
CDirectPhComn::CDirectPhComn(LPG_LDSM shmem, CCodeToStr *error)
	: CBaseFamily(shmem, error)
{
	/* 기본 Family ID 설정 */
	m_enFamily		= ENG_LUDF::en_direct_photo_comm;
	/* Direct Photohead Structure */
	m_pstShMemDP	= &shmem->directph;
	m_pstShMemPF	= &shmem->focus;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDirectPhComn::~CDirectPhComn()
{
}

/* --------------------------------------------------------------------------------------------- */
/*                                    Packet Function (Make)                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Sets the packet framework
 parm : msg_size- [in]  The lenght of message (Size containing the length of 'data')
		ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		rec_id	- [in]  Record ID Number
		data	- [in]  The area of udp packet
		size	- [in]  The size of 'data'
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CDirectPhComn::GetPktData(UINT16 msg_size, UINT8 ph_no, UINT16 rec_id, PUINT8 data, UINT8 size)
{
	UINT8 u8Body[128]	= { NULL }, *pBody = u8Body;
	UINT16 u16MsgSize	= SWAP16(msg_size);
	UINT16 u16RecID		= SWAP16(rec_id);
	UINT32 u32Pkts		= 256;
	PUINT8 pPktBuff		= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;

	/* Size of Message (= L) */
	memcpy(pBody, &u16MsgSize, 2);	pBody	+= 2;
	/* Control 즉, Photohead Number */
	*pBody	= ph_no;				pBody	+= 1;
	/* Size of Message (= L) */
	memcpy(pBody, &u16MsgSize, 2);	pBody	+= 2;
	/* Record ID */
	memcpy(pBody, &u16RecID, 2);	pBody	+= 2;
	/* 만약와 크기가 궁합이 맞지 않으면 NULL 처리 */
	if ((!data && size > 0) || (data && size < 1))	return NULL;
	if (data)	memcpy(pBody, data, size);	pBody	+= size;
	/* Merge the send packet */
	pPktNext	= GetPktBase(ENG_LTCT::en_read, ENG_LCDP::en_photohead_command,
							 pPktNext, u32Pkts, u8Body, 7+size);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Record that can be used to read out the current status of the focus motor
 parm : None
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CDirectPhComn::GetPktMotorStateAll()
{
	return GetPktData(4, 0xff, (UINT16)ENG_LLRN::en_get_fcs_motor_status);
}

/*
 desc : Focus Motor to Mid-position (Initialize)
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08 or 0xff : All)
		action	- [in]  1 = Start an initialization process. (Any value > 1 will be decoded as 1)
							(ction = 1 will return error = 10014 if motor is busy moving or if tracking-mode is enabled)
						0 = Abort any ongoing initialization process
						    (action = 0 will always return ok (0))
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CDirectPhComn::GetPktMotorPositionInit(UINT8 ph_no, UINT8 action)
{
	return GetPktData(5, ph_no, (UINT16)ENG_LLRN::en_set_fcs_mtr_mid_pos, &action, 1);
}
PUINT8 CDirectPhComn::GetPktMotorPositionInitAll(UINT8 action)
{
	return GetPktData(5, 0xff, (UINT16)ENG_LLRN::en_set_fcs_mtr_mid_pos, &action, 1);
}

/*
 desc : 모든 Photohead의 Focus Motor Move Absolute Position (현재 위치 얻기)
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08 or 0xff : All)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CDirectPhComn::GetPktMotorAbsPositionAll()
{
	return GetPktData(4, 0xff, (UINT16)ENG_LLRN::en_get_fcs_mtr_abs_pos);
}

/*
 desc : 모든 Photohead의 Led On Time 값 얻기
 parm : freq_no	- [in]  전체 Photohead의 주파주 번호 (0 ~ 3)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CDirectPhComn::GetPktPhLedOnTimeAll(UINT16 freq_no)
{
	UINT16 u16LedFreq = (UINT16)htons(freq_no);
	return GetPktData(6, 0xff, (UINT16)ENG_LLRN::en_get_ph_led_ontime, (PUINT8)&u16LedFreq, 2);
}

/*
 desc : 모든 Photohead의 Led Temperature 값 얻기
 parm : freq_no	- [in]  전체 Photohead의 주파주 번호 (0 ~ 3)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CDirectPhComn::GetPktPhLedTempAll(UINT16 freq_no)
{
	UINT16 u16LedFreq = (UINT16)htons(freq_no);
	return GetPktData(6, 0xff, (UINT16)ENG_LLRN::en_get_ph_led_temp, (PUINT8)&u16LedFreq, 2);
}

/*
 desc : Focus Motor Move Absolute Position (이동 위치 설치)
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08 or 0xff : All)
		pos		- [in]  이동하려는 절대 위치 값 (unit: um) (rangle: 63000~85000)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CDirectPhComn::SetPktMotorAbsPosition(UINT8 ph_no, UINT32 pos)
{
	UINT32 u32Pos	= (UINT32)htonl(pos);
	return GetPktData(8, ph_no, (UINT16)ENG_LLRN::en_set_fcs_mtr_abs_pos, (PUINT8)&u32Pos, 4);
}

/*
 desc : Focus Motor Calibration - Measured Distance
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08 or 0xff : All)
		dist	- [in]  이동하고자 하는 거리 값 (단위: um, 범위: 75000 ~ 8000)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CDirectPhComn::SetPktFocusMotorMeasureDist(UINT8 ph_no, UINT32 dist)
{
	return GetPktData(8, ph_no, (UINT16)ENG_LLRN::en_set_fcs_mtr_measure_dist, (PUINT8)&dist, 4);
}
PUINT8 CDirectPhComn::GetPktFocusMotorMeasureDist(UINT8 ph_no)
{
	return GetPktData(4, ph_no, (UINT16)ENG_LLRN::en_get_fcs_mtr_measure_dist);
}

/*
 desc : Photohead에 저장되어 있는 Image Type 정보 요청
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08 or 0xff : All)
 retn : Pointer where the packet buffer is stored
 note : Image Type
		0x0001 - Binary, uncompressed. (1-bit bitmap (.bmp) without bmp-header (62 bytes))
		0x0002 - 1-bit RLE-encoded image data
		0x0003 - 8-bit grayscale bitmap (.bmp). This is used by web-interface only. Will be converted to binary (type. 1)
		0x0004 - 1-bit bitmap (.bmp). This is used by the web-interface only. Will be converted to binary (type 1).
		0x0005 - 1-bit RLE-encoded image data with swapped bits within bytes.
*/
PUINT8 CDirectPhComn::GetPktImageType(UINT8 ph_no)
{
	return GetPktData(4, ph_no, (UINT16)ENG_LLRN::en_get_image_type);
}

/*
 desc : Flatness correction mask Enable or Disable
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		enable	- [in]  Select the usage mode (0x00 - Disable : Spot Mode, 0x01 - Enable : Expo Mode)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CDirectPhComn::SetPktFlatnessMaskOn(UINT8 ph_no, UINT8 enable)
{
	return GetPktData(5, ph_no, (UINT16)ENG_LLRN::en_set_enable_flatness_correction_mask, &enable, 1);
}
PUINT8 CDirectPhComn::GetPktFlatnessMaskOn(UINT8 ph_no)
{
	return GetPktData(4, ph_no, (UINT16)ENG_LLRN::en_get_enable_flatness_correction_mask);
}

/*
 desc : Request to set or get the internal image number to be loaded on the photohead
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		img_no	- [in]  The internal image to be displayed (Loaded) (0x00 ~ 0x04)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CDirectPhComn::SetPktLoadInternalImage(UINT8 ph_no, UINT8 img_no)
{
	return GetPktData(5, ph_no, (UINT16)ENG_LLRN::en_set_load_internal_image, &img_no, 1);
}
PUINT8 CDirectPhComn::GetPktLoadInternalImage(UINT8 ph_no)
{
	return GetPktData(4, ph_no, (UINT16)ENG_LLRN::en_get_load_internal_image);
}

/*
 desc : This record makes it possible to select which sequence file to be used
		It internal sequence is selected, the image that is currently loaded in inum 0, offset 0 will be displayed.
		The build-in sequence file will automatically be fitted to the current DMD-size that is mounted in the system
		(1024x768, 1920x1080 or 1920x1200)
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		type	- [in]  0x00 : Use last loaded (external) sequence file
						0x01 : Use internal sequence file
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CDirectPhComn::SetPktActiveSequence(UINT8 ph_no, UINT8 type)
{
	return GetPktData(5, ph_no, (UINT16)ENG_LLRN::en_set_active_sequence, &type, 1);
}
PUINT8 CDirectPhComn::GetPktActiveSequence(UINT8 ph_no)
{
	return GetPktData(4, ph_no, (UINT16)ENG_LLRN::en_get_active_sequence);
}

/*
 desc : Light Source Driver Light On/Off
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08)
		led_no	- [in]  LED Number (0x0001 ~ 0x0004)
		onoff	- [in]  0x00 : Light Off, 0x01 : Light ON
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CDirectPhComn::SetPktLedLightOnOff(UINT8 ph_no, ENG_LLPI led_no, UINT8 onoff)
{
	UINT8 u8Data[8]	= {NULL};
	UINT16 u16LedNo	= htons(UINT16(led_no) - 1);
	
	/* Payload 패킷 구성 */
	memcpy(u8Data, &u16LedNo, 2);
	u8Data[2]	= onoff;

	return GetPktData(7, ph_no, (UINT16)ENG_LLRN::en_set_led_light_on_off, u8Data, 3);
}
PUINT8 CDirectPhComn::GetPktLedLightOnOff(UINT8 ph_no, ENG_LLPI led_no)
{
	UINT8 u8Data[8]	= {NULL};
	UINT16 u16LedNo	= htons(UINT16(led_no) - 1);

	/* Payload 패킷 구성 */
	memcpy(u8Data, &u16LedNo, 2);

	return GetPktData(6, ph_no, (UINT16)ENG_LLRN::en_get_led_light_on_off, u8Data, 2);
}

/*
 desc : This record is used to manipulate and request the status of the sequencer in Luxibeam (Halted or Running)
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		seq_cmd	- [in]  Command to send/request
						0x01 - Run	: When enabled, the sequencer will start running from its current position
									  When disabled, the sequencer will stop at its current position
						0x02 - Reset: When enabled, the sequencer will enter its <<reset>>-state
									  When disabled, the sequencer will be taken out of its <<reset>>-state
						0x03 - Unused:These values will be regarded as invalid if sent or requested
		enable	- [in]	Enable(0x01) or Disable (0x00)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CDirectPhComn::SetPktSequnceState(UINT8 ph_no, UINT8 seq_cmd, UINT8 enable)
{
	UINT8 u8Data[8]	= { seq_cmd, enable };
	
	return GetPktData(6, ph_no, (UINT16)ENG_LLRN::en_set_sequencer_state, u8Data, 2);
}
PUINT8 CDirectPhComn::GetPktSequnceState(UINT8 ph_no, UINT8 seq_cmd)
{
	UINT8 u8Data[8]	= { seq_cmd };
	
	return GetPktData(5, ph_no, (UINT16)ENG_LLRN::en_get_sequencer_state, u8Data, 1);
}

/*
 desc : LED Driver Amplitude. That is, Records used to manipulate the amplitude in the Light Source drivers
		(It is different from the LedAmplitude function. The purpose of this function is to use th SpotMode)
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		led_no	- [in]  LED Number (0x0001 ~ 0x0004)
		amp_idx	- [in]  Illuminance Power Index
 retn : TRUE or FALSE
*/
PUINT8 CDirectPhComn::SetPktLightAmplitude(UINT8 ph_no, ENG_LLPI led_no, UINT16 amp_idx)
{
	UINT8 u8Data[8]	= {NULL};
	UINT16 u16LedNo	= htons(UINT16(led_no) - 1);

	amp_idx	= htons(amp_idx);
	memcpy(u8Data, &u16LedNo, 2);
	memcpy(u8Data+2, &amp_idx, 2);

	return GetPktData(8, ph_no, (UINT16)ENG_LLRN::en_set_led_amplitude, u8Data, 4);
}
PUINT8 CDirectPhComn::GetPktLightAmplitude(UINT8 ph_no, ENG_LLPI led_no)
{
	UINT8 u8Data[8]	= {NULL};
	UINT16 u16LedNo	= htons(UINT16(led_no) - 1);

	memcpy(u8Data, &u16LedNo, 2);

	return GetPktData(6, ph_no, (UINT16)ENG_LLRN::en_get_led_amplitude, u8Data, 2);
}

/*
 desc : Records that requests status of the Light Source drivers
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		led_no	- [in]  LED Number (0x0001 ~ 0x0004)
 retn : TRUE or FALSE
 note : Note that all errors/warnings will be retained in the status word
		until it is actively cleared, using command <<Clear Error Status>>
*/
PUINT8 CDirectPhComn::GetPktLedSourceDriverStatus(UINT8 ph_no, ENG_LLPI led_no)
{
	UINT8 u8Data[8]	= {NULL};
	UINT16 u16LedNo	= htons(UINT16(led_no) - 1);

	memcpy(u8Data, &u16LedNo, 2);

	return GetPktData(8, ph_no, (UINT16)ENG_LLRN::en_get_led_drive_status, u8Data, 2);
}

/*
 desc : Photohead 번호마다 모든 에러 상태 정보 반환
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08 or 0xff : All)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CDirectPhComn::GetPktOverallErrorStatus(UINT8 ph_no)
{
	return GetPktData(4, ph_no, (UINT16)ENG_LLRN::en_get_overall_error_status);
}

/*
 desc : 모든 Photohead 에 발생된 Error Clear 처리
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08 or 0xff : All)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CDirectPhComn::SetPktClearErrorStatus(UINT8 ph_no)
{
	return GetPktData(4, ph_no, (UINT16)ENG_LLRN::en_set_clear_error_status);
}

/*
 desc : Photohead 마다 설치된 모든 LED 정보 및 연결 개수 요청
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CDirectPhComn::GetPktLightSource(UINT8 ph_no)
{
	return GetPktData(4, ph_no, (UINT16)ENG_LLRN::en_get_light_source_info);
}

/*
 desc : 특정 Photohead에 설치된 카메라의 LED Light Value (0x00 ~ 0xff) 값 설정
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		value	- [in]  Illumination Light Value (0x00 ~ 0xff)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CDirectPhComn::SetPktCamIllumValue(UINT8 ph_no, UINT8 value)
{
	return GetPktData(5, ph_no, (UINT16)ENG_LLRN::en_set_illum_led, &value, 0x01);
}
PUINT8 CDirectPhComn::GetPktCamIllumValue(UINT8 ph_no)
{
	return GetPktData(4, ph_no, (UINT16)ENG_LLRN::en_get_illum_led);
}

/*
 desc : 특정 Photohead에 대한 Line Sensor Data의 Plotting을 Enable or Disable
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		enable	- [in]  enable (0x01) or Disable (0x00)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CDirectPhComn::SetPktLineSensorPlot(UINT8 ph_no, UINT8 enable)
{
	return GetPktData(5, ph_no, (UINT16)ENG_LLRN::en_set_plot_enable, &enable, 0x01);
}
PUINT8 CDirectPhComn::GetPktLineSensorPlot(UINT8 ph_no)
{
	return GetPktData(4, ph_no, (UINT16)ENG_LLRN::en_get_plot_enable);
}

/*
 desc : 현재 광학계의 Autofocus Position 확인
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CDirectPhComn::GetPktCurrentAutofocusPosition(UINT8 ph_no)
{
	return GetPktData(4, ph_no, (UINT16)ENG_LLRN::en_get_autofocus_position);
}

/*
 desc : 현재 광학계의 DMD Flip Output 여부 확인
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		flip	- [in]  0x00: No flipping, 0x01: Flip image in Y direction
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CDirectPhComn::SetPktDmdFlipOutput(UINT8 ph_no, UINT8 flip)
{
	return GetPktData(5, ph_no, (UINT16)ENG_LLRN::en_get_dmd_flip_output, &flip, 0x01);
}
PUINT8 CDirectPhComn::GetPktDmdFlipOutput(UINT8 ph_no)
{
	return GetPktData(4, ph_no, (UINT16)ENG_LLRN::en_get_dmd_flip_output);
}

/*
 desc : This record will turn on/off the shaking function of the DMD mirrors.
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		enable	- [in]  0 : DMD mirror shake OFF, 1 : DMD mirror shake ON
 retn : Pointer where the packet buffer is stored
 note : This function requires that the internal sequencer is running
*/
PUINT8 CDirectPhComn::SetPktDmdMirrorShake(UINT8 ph_no, UINT8 enable)
{
	return GetPktData(5, ph_no, (UINT16)ENG_LLRN::en_set_dmd_mirror_shake, &enable, 0x01);
}
PUINT8 CDirectPhComn::GetPktDmdMirrorShake(UINT8 ph_no)
{
	return GetPktData(4, ph_no, (UINT16)ENG_LLRN::en_get_dmd_mirror_shake);
}

/*
 desc : This record manipulates the duration of the light pulse that is applied when a «LightPulseWord» command 
		is used in the sequence file.
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		value	- [uin16]  The duration the light pulse should be on.
 retn : Pointer where the packet buffer is stored
 note : This function requires that the internal sequencer is running
*/
PUINT8 CDirectPhComn::SetPktLightPulseDuration(UINT8 ph_no, UINT16 value)
{
	UINT8 u8Data[8] = { NULL };
	UINT16 u16Duration = htons(UINT16(value));

	memcpy(u8Data, &u16Duration, 2);

	return GetPktData(6, ph_no, (UINT16)ENG_LLRN::en_set_light_pulse_duration, u8Data, 0x02);
}
PUINT8 CDirectPhComn::GetPktLightPulseDuration(UINT8 ph_no)
{
	return GetPktData(4, ph_no, (UINT16)ENG_LLRN::en_get_light_pulse_duration);
}

/* --------------------------------------------------------------------------------------------- */
/*                                    Packet Function (Save)                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Machine Config
 parm : uid		- [in]  서브 명령어
		data	- [in]  데이터가 저장된 버퍼 포인터
		size	- [in]  'data' 의 크기
 retn : None
*/
VOID CDirectPhComn::SetRecvPacket(UINT8 uid, PUINT8 data, UINT32 size)
{
	UINT8 u8PhNo		= 0x01;
	UINT16 u16TotSize	= 0x0000, u16RecID = 0x0000;
	INT32 i32RetSize	= -1, i32Size = size;
	PUINT8 pData		= data;

	/* 본문 데이터가 없는지 여부 */
	if (!data || i32Size < 2)	return;

	/* 모든 포토 헤드로부터 수신된 메시지 처리 */
	do	{

		/* tot_size */
		memcpy(&u16TotSize, pData, 2);	pData += 2;
		u16TotSize	= ntohs(u16TotSize);
		if (u16TotSize < 1)	break;
		/* rec_id */
		memcpy(&u16RecID, pData, 2);	pData += 2;
		u16RecID	= ntohs(u16RecID);

		/* 전체 Photohead 설정 (or 요청)에 대한 응답 경우 */
		if (0xff == u8PhNo)	break;
		/* 단일 Photohead 설정 (or 요청)에 대한 응답 경우 */
		else
		{
			/* Phtohead Number가 현재 등록된 개수보다 크면 안됨 */
			if (MAX_PH >= u8PhNo && u8PhNo > 0)
			{
				i32RetSize	= (INT32)SetRecvDirectPh(u8PhNo, u16RecID, pData);
			}
		}

		/* 패킷 분석 실패인 경우 */
		if (i32RetSize < 0)	break;

		/* 다음 데이터 위치로 포인터 이동 */
		pData	+= i32RetSize;
		/* 읽어들인 크기 계산 */
		i32Size	-= (4 + i32RetSize);
		/* Photohead Number 증가*/
		u8PhNo++;

		/* 현재 수신된 데이터 크기만큼 모두 읽어들였는지 여부 */
	}	while (i32Size > 0);

	/* 가장 마지막에 수신된 Record ID 저장 */
	if (u16RecID)	m_pstShMemDP->get_last_received_record_id	= u16RecID;
}

/*
 desc : Direct Photohead <single>의 Body Part
 parm : ph_no	- [in]  Photohead Number (1 ~ 8)
		rec_id	- [in]  Record ID
		data	- [in]  This is buffer where the received data is stored (Payload)
 retn : Reeturns the size of the processed data (음수 값이면 실패)
*/
INT32 CDirectPhComn::SetRecvDirectPh(UINT8 ph_no, UINT16 rec_id, PUINT8 data)
{
	TCHAR tzMesg[64]	= {NULL};
	INT32 i32RetSize	= -1;

	/* 포토헤드 번호가 잘못 수신된 경우인지 확인 */
	if (ph_no < 1 || ph_no > MAX_PH)	return -1;

	switch (rec_id)
	{
	case ENG_LLRN::en_res_reply_ack							:	i32RetSize = SetRecvReplyAck(data);					break;	/* General Reply Record */
	case ENG_LLRN::en_res_plot_enable						:	i32RetSize = SetRecvPlotEnable(ph_no, data);		break;
	case ENG_LLRN::en_res_fcs_motor_status					:	i32RetSize = SetRecvMotorStatus(ph_no, data);		break;
	case ENG_LLRN::en_res_fcs_mtr_abs_pos					:	i32RetSize = SetRecvMotorAbsPosition(ph_no, data);	break;
	case ENG_LLRN::en_res_led_amplitude						:	i32RetSize = SetRecvLedPower(ph_no, data);			break;
	case ENG_LLRN::en_res_led_drive_status					:	i32RetSize = SetRecvLedDriveStatus(ph_no, data);	break;
	case ENG_LLRN::en_res_ph_led_temp						:	i32RetSize = SetRecvTemp(ph_no, data);				break;
	case ENG_LLRN::en_res_led_light_on_off					:	i32RetSize = SetRecvLedLightOnOff(ph_no, data);		break;
	case ENG_LLRN::en_res_autofocus_position				:	i32RetSize = SetRecvAutofocusPosition(ph_no, data);	break;
	case ENG_LLRN::en_res_enable_flatness_correction_mask	:	i32RetSize = SetRecvFlatnessMaskOn(ph_no, data);	break;	/* Enable Flatness Correction Mask (PPC) */
	case ENG_LLRN::en_res_ph_led_ontime						:	i32RetSize = SetRecvLedOnTime(ph_no, data);			break;
	case ENG_LLRN::en_res_load_internal_image				:	i32RetSize = SetRecvLoadInternalImage(ph_no, data);	break;
	case ENG_LLRN::en_res_active_sequence					:	i32RetSize = SetRecvActiveSeqFile(ph_no, data);		break;
	case ENG_LLRN::en_res_overall_error_status				:	i32RetSize = SetRecvOverallError(ph_no, data);		break;
	case ENG_LLRN::en_res_illum_led							:	i32RetSize = SetRecvCameraIllumLED(ph_no, data);	break;
	case ENG_LLRN::en_res_light_source_info					:	i32RetSize = SetRecvLightSourceInfo(ph_no, data);	break;
	case ENG_LLRN::en_res_dmd_flip_output					:	i32RetSize = SetRecvDmdFlipOutput(ph_no, data);		break;
	case ENG_LLRN::en_res_dmd_mirror_shake					:	i32RetSize = SetRecvDmdMirrorShake(ph_no, data);	break;
	case ENG_LLRN::en_res_sequencer_state					:	i32RetSize = SetRecvSequencerState(ph_no, data);	break;
	default													:
		swprintf_s(tzMesg, 64, L"Invalid record id received = %d (0x%04x)", rec_id, rec_id);
		LOG_WARN(ENG_EDIC::en_luria, tzMesg);	break;
	}

	return i32RetSize;
}

/*
 desc : Direct Photohead - ReplyAck
 parm : data	- [in]  This is buffer where the received data is stored
 retn : Reeturns the size of the processed data
*/
INT32 CDirectPhComn::SetRecvReplyAck(PUINT8 data)
{
	TCHAR tzMesg[256]	= {NULL};
	UINT16 u16Data		= 0;
	PUINT8 pData		= data;

	memcpy(&u16Data, pData, 2);
	m_pstShMemDP->get_last_received_reply_error = SWAP16(u16Data);

	/* 가장 최근에 발생된 에러 코드 값 반환 */
	if (m_pstShMemDP->get_last_received_reply_error)
	{
		swprintf_s(tzMesg, 256, L"Photohead Reply Error [0x%04x]:%s",
				   m_pstShMemDP->get_last_received_reply_error,
				   m_pCodeToStr->GetMesg(ENG_SCTS::en_luria_ph, m_pstShMemDP->get_last_received_reply_error));
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
	}

	return (0x00000002);
}

/*
 desc : 전체 Photohead의 Motor Status 값
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08)
		data	- [in]  Photohead Number 별로 수신된 데이터 값
 retn : Reeturns the size of the processed data
*/
INT32 CDirectPhComn::SetRecvMotorStatus(UINT8 ph_no, PUINT8 data)
{
	UINT16 u16Status	= 0;

	memcpy(&u16Status, data, 2);
	m_pstShMemDP->focus_motor_status[ph_no-1]	= SWAP16(u16Status);

	return (0x00000002);
}

/*
 desc : 전체 Photohead의 Motor 위치 값
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08)
		data	- [in]  Photohead Number 별로 수신된 데이터 값 (unit: um)
 retn : Reeturns the size of the processed data
*/
INT32 CDirectPhComn::SetRecvMotorAbsPosition(UINT8 ph_no, PUINT8 data)
{
	INT32 i32Pos	= 0;

	/* 현재 절대 좌표 위치 값 복사 */
	memcpy(&i32Pos, data, 4);
	/* 멤버 변수에 값 저장 */
	m_pstShMemDP->focus_motor_move_abs_position[ph_no-1]	= SWAP32(i32Pos);
	m_pstShMemPF->z_pos[ph_no-1]	= SWAP32(i32Pos) * 1000;	/* um -> nm */

	return (0x00000004);
}

/*
 desc : Direct Photohead - Led Temperature
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08) (1 ~ 8)
		data	- [in]  This is buffer where the received data is stored
 retn : Reeturns the size of the processed data
*/
INT32 CDirectPhComn::SetRecvTemp(UINT8 ph_no, PUINT8 data)
{
	UINT16 u16LedNo		= 0;
	UINT16 u16LedTemp	= 0, u16BoardTemp = 0;
	PUINT8 pData		= data;

	/* Led Number */
	memcpy(&u16LedNo,		pData, 2);	pData	+= 2;
	memcpy(&u16LedTemp,		pData, 2);	pData	+= 2;
	memcpy(&u16BoardTemp,	pData, 2);	pData	+= 2;

	/* 정상적으로 값을 읽어온 경우만 갱신  */
	if (*pData != 0x01)	return (0x00000007);
	u16LedNo	= SWAP16(u16LedNo);
	m_pstShMemDP->light_source_driver_temp_led[ph_no-1][u16LedNo]	= SWAP16(u16LedTemp);
	m_pstShMemDP->light_source_driver_temp_board[ph_no-1][u16LedNo]= SWAP16(u16BoardTemp);

	return (0x00000007);
}

/*
 desc : Direct Photohead - Led On Time
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08) (1 ~ 8)
		data	- [in]  This is buffer where the received data is stored
 retn : Reeturns the size of the processed data
*/
INT32 CDirectPhComn::SetRecvLedOnTime(UINT8 ph_no, PUINT8 data)
{
	UINT8 u8ReadOK	= 0;
	UINT16 u16LedNo	= 0;
	UINT32 u32OnTime= 0;
	PUINT8 pData	= data;

	/* Led Number (Luxbeam 4700) */
	memcpy(&u16LedNo,	pData, 2);	pData	+= 2;
	memcpy(&u32OnTime,	pData, 4);	pData	+= 4;
	memcpy(&u8ReadOK,	pData, 1);	pData	+= 1;
	/* 정상적으로 값을 읽어온 경우만 갱신  */
	if (u8ReadOK != 0x01)	return (0x00000007);
	u16LedNo	= SWAP16(u16LedNo);
	m_pstShMemDP->light_source_on_time_counter[ph_no-1][u16LedNo]	= SWAP32(u32OnTime);
	m_pstShMemDP->light_source_on_time_read_ok[ph_no-1][u16LedNo]	= u8ReadOK;

	return (0x00000007);
}

/*
 desc : Direct Photohead - Current Autofocus Position
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08) (1 ~ 8)
		data	- [in]  This is buffer where the received data is stored
 retn : Reeturns the size of the processed data
*/
INT32 CDirectPhComn::SetRecvAutofocusPosition(UINT8 ph_no, PUINT8 data)
{
	UINT16 u16Pos	= 0;
	PUINT8 pData	= data;

	/* 정상적으로 값을 읽어온 경우만 갱신  */
	memcpy(&u16Pos,	pData, 2);	pData	+= 2;
	m_pstShMemDP->auto_focus_position[ph_no]	= SWAP16(u16Pos);

	return (0x00000002);
}

/*
 desc : Direct Photohead - Overall Error Status Word
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08) (1 ~ 8)
		data	- [in]  This is buffer where the received data is stored
 retn : Reeturns the size of the processed data
*/
INT32 CDirectPhComn::SetRecvOverallError(UINT8 ph_no, PUINT8 data)
{
	TCHAR tzMesg[128]	= {NULL};
	UINT32 i, u32Error	= 0, u32Bits[9]	= {1, 2, 4, 8, 16, 32, 64, 128, 256};
	PUINT8 pData		= data;

	/* Error (Only LUT 4800/9500) */
	memcpy(&u32Error, pData, 4);	pData	+= 4;
	u32Error	= SWAP32(u32Error);
	/* 임시로 Swap 값 저장 */
	m_pstShMemDP->overall_error_status_word[ph_no-1]	= (u32Error & 0x000001ff);	/* 0 ~ 8 bit 까지만 값이 유효 하므로 */

	/* 0 ~ 8 번째까지 에러가 있는지 조회하기 위함 */
	for (i=0; i<9; i++)
	{
		wmemset(tzMesg, 0x00, 128);
		switch (i)
		{
		case 0	: if (u32Error && u32Bits[i])	swprintf_s(tzMesg, 128, L"PH (%d) : Temp regulator 1 out of range (+/- 0.1 degrees outside)", ph_no);	break;
		case 1	: if (u32Error && u32Bits[i])	swprintf_s(tzMesg, 128, L"PH (%d) : Temp regulator 2 out of range (+/- 0.1 degrees outside)", ph_no);	break;
		case 2	: if (u32Error && u32Bits[i])	swprintf_s(tzMesg, 128, L"PH (%d) : AF No Z-motor motion (AF must be enabled)", ph_no);					break;
		case 3	: if (u32Error && u32Bits[i])	swprintf_s(tzMesg, 128, L"PH (%d) : AF Maximum Z-motor motion (AF must be enabled)", ph_no);				break;
		case 4	: if (u32Error && u32Bits[i])	swprintf_s(tzMesg, 128, L"PH (%d) : Z-motor init failed (Initialization : timeout", ph_no);				break;
		case 5	: if (u32Error && u32Bits[i])	swprintf_s(tzMesg, 128, L"PH (%d) : Z-motor position at endstop (inside endstop area)", ph_no);			break;
		case 6	: if (u32Error && u32Bits[i])	swprintf_s(tzMesg, 128, L"PH (%d) : Reserved", ph_no);													break;
		case 7	: if (u32Error && u32Bits[i])	swprintf_s(tzMesg, 128, L"PH (%d) : Low Ethernet link quality (Many re-transmission of UDP)", ph_no);	break;
		case 8	: if (u32Error && u32Bits[i])	swprintf_s(tzMesg, 128, L"PH (%d) : Board Temperature High (Turn off system)", ph_no);					break;
		}
		/* 에러 정보가 있는 경우만 저장 */
		if (_tcslen(tzMesg))	LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
	}

	return (0x00000008);
}

/*
 desc : Direct Photohead - Light Source Info
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08) (1 ~ 8)
		data	- [in]  This is buffer where the received data is stored
 retn : Reeturns the size of the processed data
*/
INT32 CDirectPhComn::SetRecvLightSourceInfo(UINT8 ph_no, PUINT8 data)
{
	/* Led Number */
	m_pstShMemDP->light_source_info[ph_no-1][0]	= data[0];	/* LED Type. 0:LED Combinaer v1, 1:Laser, 2:LED Combiner v2, 3: LRS-HY driver */
	m_pstShMemDP->light_source_info[ph_no-1][1]	= data[1];	/* LED Connection Count */

	return (0x00000002);
}

/*
 desc : 광학계 내부에 설치된 카메라의 LED Level (LED Light Value. 0 ~ 255) 값 저장
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08) (1 ~ 8)
		data	- [in]  This is buffer where the received data is stored
 retn : Reeturns the size of the processed data
*/
INT32 CDirectPhComn::SetRecvCameraIllumLED(UINT8 ph_no, PUINT8 data)
{
	m_pstShMemDP->camera_illumination_led[ph_no-1]	= data[0];

	return (0x00000001);
}

/*
 desc : 광학계 내부 이미지의 Line Sensor Plot Enable 값 설정
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08) (1 ~ 8)
		data	- [in]  This is buffer where the received data is stored
 retn : Reeturns the size of the processed data
*/
INT32 CDirectPhComn::SetRecvPlotEnable(UINT8 ph_no, PUINT8 data)
{
	m_pstShMemDP->line_sensor_plot[ph_no-1]	= data[0];

	return (0x00000001);
}

/*
 desc : 광학계의 현재 Focus 위치 값 설정
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08) (1 ~ 8)
		data	- [in]  This is buffer where the received data is stored
 retn : Reeturns the size of the processed data
 note : 수치 1 당 0.174 um (기준: Luxbeam 4800/9500), 범위 (21284 ~ 44252)
*/
INT32 CDirectPhComn::SetRecvFocusPosition(UINT8 ph_no, PUINT8 data)
{
	UINT16 u16Pos	= 0;

	memcpy(&u16Pos, data, 2);
	m_pstShMemDP->focus_position[ph_no-1]	= SWAP16(u16Pos);

	return (0x00000002);
}

/*
 desc : Direct Photohead - LedPower
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08)
		data	- [in]  This is buffer where the received data is stored
 retn : Reeturns the size of the processed data
*/
INT32 CDirectPhComn::SetRecvLedPower(UINT8 ph_no, PUINT8 data)
{
	TCHAR tzMesg[96]= {NULL};
	UINT16 u16LedNo	= 0x0000, u16Power;
	PUINT8 pData	= data;

	/* LED Number */
	memcpy(&u16LedNo, pData, 2);	pData	+= 2;
	u16LedNo	= SWAP16(u16LedNo);
	/* Amplitude */
	memcpy(&u16Power, pData, 2);	pData	+= 2;
	u16Power	= SWAP16(u16Power);
	
	if (*pData != 0x01)
	{
		swprintf_s(tzMesg, 96, L"LED (%d) illumination value not received from PH (%d)",
				   u16LedNo, ph_no);
		LOG_WARN(ENG_EDIC::en_luria, tzMesg);
		return (0x00000005);
	}
	
	/* LED On or Off 값 설정 */
	m_pstShMemDP->light_source_driver_amplitude[ph_no-1][u16LedNo]	= u16Power;

	return (0x00000005);
}

/*
 desc : Direct Photohead - Led Driver Status
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08)
		data	- [in]  This is buffer where the received data is stored
 retn : Reeturns the size of the processed data
*/
INT32 CDirectPhComn::SetRecvLedDriveStatus(UINT8 ph_no, PUINT8 data)
{
	TCHAR tzMesg[96]= {NULL};
	UINT16 u16LedNo	= 0x0000;
	UINT32 u32Status= 0x00000000;
	PUINT8 pData	= data;

	/* LED Number */
	memcpy(&u16LedNo, pData, 2);	pData	+= 2;
	u16LedNo	= SWAP16(u16LedNo);

	if (*pData != 0x01)
	{
		swprintf_s(tzMesg, 96, L"LED (%d) light source drive status value not received from PH (%d)",
				   u16LedNo, ph_no);
		LOG_WARN(ENG_EDIC::en_luria, tzMesg);
		return (0x00000005);
	}

	/* Status */
	memcpy(&u32Status, pData, 4);	pData	+= 4;
	m_pstShMemDP->light_source_driver_status[ph_no-1][u16LedNo]	= SWAP32(u32Status);

	return (0x00000005);
}

/*
 desc : Direct Photohead - LedLightOnOff
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08)
		data	- [in]  This is buffer where the received data is stored
 retn : Reeturns the size of the processed data
*/
INT32 CDirectPhComn::SetRecvLedLightOnOff(UINT8 ph_no, PUINT8 data)
{
	TCHAR tzMesg[96]= {NULL};
	UINT16 u16LedNo	= 0;
	PUINT8 pData	= data;

	/* LED Number */
	memcpy(&u16LedNo, pData, 2);	pData	+= 2;
	u16LedNo	= SWAP16(u16LedNo);

	if (*pData != 0x01)
	{
		swprintf_s(tzMesg, 96, L"LED (%d) illumination value not received from PH (%d)",
				   u16LedNo, ph_no);
		LOG_WARN(ENG_EDIC::en_luria, tzMesg);
		return (0x00000004);
	}
	else
	{
		/* LED On or Off 값 설정 */
		m_pstShMemDP->light_source_driver_light_onoff[ph_no-1][u16LedNo]	= data[2];
	}

	return (0x00000004);
}

/*
 desc : Direct Photohead - FlatnessMaskOn
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08)
		data	- [in]  This is buffer where the received data is stored
 retn : Reeturns the size of the processed data
*/
INT32 CDirectPhComn::SetRecvFlatnessMaskOn(UINT8 ph_no, PUINT8 data)
{
	m_pstShMemDP->enable_flatness_correction_mask[ph_no-1]	= data[0];

	return (0x00000001);
}

/*
 desc : Direct Photohead - LoadInternalImage
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08)
		data	- [in]  This is buffer where the received data is stored
 retn : Reeturns the size of the processed data
*/
INT32 CDirectPhComn::SetRecvLoadInternalImage(UINT8 ph_no, PUINT8 data)
{
	m_pstShMemDP->load_internal_image[ph_no-1]	= data[0];

	return (0x00000001);
}

/*
 desc : Direct Photohead - ActiveSeqFile
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08)
		data	- [in]  This is buffer where the received data is stored
 retn : Reeturns the size of the processed data
*/
INT32 CDirectPhComn::SetRecvActiveSeqFile(UINT8 ph_no, PUINT8 data)
{
	m_pstShMemDP->active_sequence_file[ph_no-1]	= data[0];

	return (0x00000001);
}

/*
 desc : Direct Photohead - DMD Flipe Output
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08)
		data	- [in]  This is buffer where the received data is stored
 retn : Reeturns the size of the processed data
*/
INT32 CDirectPhComn::SetRecvDmdFlipOutput(UINT8 ph_no, PUINT8 data)
{
	m_pstShMemDP->dmd_flip_output[ph_no-1]	= data[0];

	return (0x00000001);
}

/*
 desc : Direct Photohead - DMD Mirror Shake
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08)
		data	- [in]  This is buffer where the received data is stored
 retn : Reeturns the size of the processed data
*/
INT32 CDirectPhComn::SetRecvDmdMirrorShake(UINT8 ph_no, PUINT8 data)
{
	m_pstShMemDP->dmd_mirror_shake[ph_no-1]	= data[0];

	return (0x00000001);
}

/*
 desc : Direct Photohead - Sequencer State
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08)
		data	- [in]  This is buffer where the received data is stored
 retn : Reeturns the size of the processed data
*/
INT32 CDirectPhComn::SetRecvSequencerState(UINT8 ph_no, PUINT8 data)
{
	m_pstShMemDP->sequencer_state[ph_no-1][0]	= data[0];	/* seq_cmd */
	m_pstShMemDP->sequencer_state[ph_no-1][1]	= data[1];	/* enable or disable */
	m_pstShMemDP->sequencer_state[ph_no-1][2]	= data[2];	/* valid or invalid command */

	return (0x00000003);
}

