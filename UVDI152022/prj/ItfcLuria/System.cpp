
/*
 desc : User Data Family : System Status
*/

#include "pch.h"
#include "MainApp.h"
#include "System.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : shmem	- [in]  Shared Memory (Luria Service)
 retn : None
*/
CSystem::CSystem(LPG_LDSM shmem, CCodeToStr *error)
	: CBaseFamily(shmem, error)
{
	/* �⺻ Family ID ���� */
	m_enFamily	= ENG_LUDF::en_system;
	/* System Structure */
	m_pstSystem	= &shmem->system;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CSystem::~CSystem()
{
}

/* --------------------------------------------------------------------------------------------- */
/*                                    Packet Function (Make)                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : This command will try to connect to all external devices (photo heads, motion controller, etc)
		and initialize these. This command MUST be called before being able to print
		The job list must be empty when calling this command. The command will return ok if sync with
		external devices was successful
		If init failed, then status code InitHardwareFailed will be returned
 parm : None
 retn : Pointer where the packet buffer is stored
 note : In case of HW init failure, then it may take up to 4 seconds until a reply is given
*/
PUINT8 CSystem::GetPktInitializeHardware()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCSS::en_initialize_hardware, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Query the status of the complete system
		If all 0's, then there are no errors
		If any part report a failure (i.e. one ore more bits set),
		this can only be cleared by running a successful InitializeHardware command again
 parm : None
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CSystem::GetPktSystemStatus()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Read */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCSS::en_get_system_status, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : This command will safely shut down all components of the system and leave the photo heads mirrors in a parked state
		(the LEDs will flash red and yellow alternately to indicate shut-down mode)
		This command should be run before turning off power in the photo heads
 parm : None
 retn : Pointer where the packet buffer is stored
 note : Note that the photo heads MUST be powered off and on again after running SystemShutdown command
*/
PUINT8 CSystem::GetPktSystemShutdown()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCSS::en_set_system_shutdown, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : This command will upgrade all photo heads with the firmware in the given bundle path or firmware file
		The path name can be an absolute path to a LLS software bundle or a path to a tar.gz file to be used for upgrading
		When the upgrade starts, a new InitializeHardware will be required
		If trying to do InitializeHardware before upgrade process is complete,
		BusyUpgradingFirmware status code is given
		The upgrade process takes about 7 minutes
		When upgrade is completed and BusyUpgradingFirmware is no longer given in the reply, all photo heads must be manually re-powered
 parm : length	- [in]  Length of path name text string (S). Max = 200
		file	- [in]  Path name text string (without null-termination)of bundle or full path and file name of tar.gz file
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CSystem::GetPktUpgradeFirmware(UINT16 length, PCHAR file)
{
	UINT8 u8Body[512]	= { NULL }, *pBody = u8Body;
	UINT16 u16Length= length;
	UINT32 u32Pkts	= 1024;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */
	length	= SWAP16(length);
	memcpy(pBody, &length, 2);	pBody	+= 2;
	memcpy(pBody, file, u16Length);

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCSS::en_upgrade_firmware,
						  pPktNext, u32Pkts, pBody, u16Length+2);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : This will load the selected test image that is built-in the photo head and display it on the DMD
 parm : ph_no	- [in]  0x00: All Photoheads, 1: PH1, 2: PH2, ...
		img_no	- [in]  Internal test image number (1 ~ 5)
 retn : Created Buffer Pointer
 note : Note that it is required to turn on light using SetLightIntensity in order to actually display the image
		Running this command requires InitializeHardware to be run in order to do an exposure again
		Valid from Luria version 2.3.0 and newer.
*/
PUINT8 CSystem::GetPktLoadInternalTestImage(UINT8 ph_no, UINT8 img_no)
{
	UINT8 u8Body[8]	= { ph_no, img_no, };
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 3, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCSS::en_load_internal_test_image,
						  pPktNext, u32Pkts, u8Body, 2);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Use this command to turn on the light in the photo head(s) in order to display the internal test image selected using command
		LoadInternalTestImage. The lights will be shut off again when running InitializeHardware
 parm : ph_no	- [in]  0: All Photoheads, 1: PH1, 2: PH2, ...
		led_no	- [in]  LED Number (0x00: All Leds, 1:LED1, 2:LED2, 3:LED3, ...)
		ampl	- [in]  Amplitude of selected light source(s)
 retn : Created Buffer Pointer
 note : Valid from Luria version 2.3.0 and newer
*/
PUINT8 CSystem::GetPktSetLightIntensity(UINT8 ph_no, ENG_LLPI led_no, UINT16 ampl)
{
	UINT8 u8Body[8]	= { ph_no, UINT8(led_no), }, *pBody = u8Body+2;
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 3, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */
	ampl	= SWAP16(ampl);
	memcpy(pBody, &ampl, 2);
	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCSS::en_set_light_intensity,
						  pPktNext, u32Pkts, u8Body, 4);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : This will give the temperature of the mainboard for each photo head
 parm : None
 retn : Pointer where the packet buffer is stored
 note : Note that the number must be divided by 10 to get the correct temperature
		If the temperature raises above approx 55 deg. Celsius,
		then this may indicate that the cooling is not working properly
		In such cases the photo head should be switched off and troubleshooting to find
		the cause of the temperature raise should be performed
		If photo head not connected, then 0 will be returned
		Valid from Luria version 2.11.0 and newer
*/
PUINT8 CSystem::GetPktMainboardTemperature()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 11, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Read */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCSS::en_mainboard_temperature, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Get the error status for all photo heads. See table below for bit definitions of the error status word
 parm : None
 retn : Pointer where the packet buffer is stored
 note : Valid from Luria version 2.12.0 and newer
*/
PUINT8 CSystem::GetPktOverallErrorStatusMulti()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* Determines whether it is available in the current version */
	if (!ValidVersion(2, 12, 0, 0x00))	return NULL;
	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= (PUINT8)::Alloc(sizeof(UINT8) * u32Pkts);
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Read */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCSS::en_overall_error_status_multi, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}


/* --------------------------------------------------------------------------------------------- */
/*                                    Packet Function (Save)                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ���Ź��� ������ ó�� - Machine Config
 parm : uid		- [in]  ���� ��ɾ�
		data	- [in]  �����Ͱ� ����� ���� ������
		size	- [in]  'data' �� ũ��
 retn : None
*/
VOID CSystem::SetRecvPacket(UINT8 uid, PUINT8 data, UINT32 size)
{
	switch (uid)
	{
	case (UINT8)ENG_LCSS::en_get_system_status			:	SetRecvSystemSystemStatus(data, size);		break;
	case (UINT8)ENG_LCSS::en_get_smart_data				:	SetRecvSmartData(data, size);				break;
	case (UINT8)ENG_LCSS::en_load_internal_test_image	:	SetRecvLoadInternalTestImage(data, size);	break;
	case (UINT8)ENG_LCSS::en_mainboard_temperature		:	SetRecvMainboardTemp(data, size);			break;
	case (UINT8)ENG_LCSS::en_set_light_intensity		:	SetRecvSetLightIntensity(data, size);		break;
	case (UINT8)ENG_LCSS::en_overall_error_status_multi	:	SetRecvOverallErrorStatusMulti(data, size);	break;
	}
}

/*
 desc : ���Ź��� ������ ó�� - System - System Status
 parm : data	- [in]  ���ŵ� ������
		size	- [in]  'data' ������ ũ��
 retn : None
*/
VOID CSystem::SetRecvSystemSystemStatus(PUINT8 data, UINT32 size)
{
	UINT32 u32Status[3]	= {NULL};
	PUINT8 pData		= data;

	memcpy(u32Status, pData, 12);
	/* ���� ���� �� ó���� ��Ŷ �������� �� ���� */
	u32Status[0] = SWAP32(u32Status[0]);
	u32Status[1] = SWAP32(u32Status[1]);
	u32Status[2] = SWAP32(u32Status[2]);
#if 0
	/* ���� �ý��۰� �ٸ� ���̸� ���� */
	if (u32Status[0] != m_pstSystem->get_system_status[0] ||
		u32Status[1] != m_pstSystem->get_system_status[1] ||
		u32Status[2] != m_pstSystem->get_system_status[2])
#endif
	{
		if (u32Status[0] != m_pstSystem->get_system_status[0])	SaveMotorStatus(u32Status[0]);
		if (u32Status[1] != m_pstSystem->get_system_status[1])	SavePotoHeadStatus(u32Status[1]);
		if (u32Status[2] != m_pstSystem->get_system_status[2])	SaveLuriaPCStatus(u32Status[2]);
		/* ���� �ֱ� �� ���� */
		memcpy(m_pstSystem->get_system_status, u32Status, 12);
	}
}

/*
 desc : ���Ź��� ������ ó�� - System - Smart Data
 parm : data	- [in]  ���ŵ� ������
		size	- [in]  'data' ������ ũ��
 retn : None
*/
VOID CSystem::SetRecvSmartData(PUINT8 data, UINT32 size)
{
	LPG_LSSD pstSmart	= {NULL};

	/* ���ŵ� ������ �ӽ� ������ �޸� �Ҵ� */
	pstSmart	= (LPG_LSSD)::Alloc(sizeof(STG_LSSD));
	ASSERT(pstSmart);

	/* �ӽ� ���� �� SWAP ���� */
	memcpy(pstSmart, data, size);
	pstSmart->SWAP();

	/* ���� �޸� ������ ���� */
	memcpy(&m_pstSystem->get_smart_data, pstSmart, sizeof(STG_LSSD));

	::Free(pstSmart);
}

/*
 desc : ���Ź��� ������ ó�� - System - Mainboard Temperature
 parm : data	- [in]  ���ŵ� ������
		size	- [in]  'data' ������ ũ��
 retn : None
*/
VOID CSystem::SetRecvMainboardTemp(PUINT8 data, UINT32 size)
{
	UINT16 i,u16Temp[MAX_PH]= {NULL};	/* �ִ� 8 ���� Photohead�� Mainboard �µ� �� */
	PUINT8 pData			= data;

	/* ��Ŷ�� ũ�Ⱑ �ʹ� ū�� Ȯ�� */
	if (size > MAX_PH * sizeof(UINT16))	return;
	/* �ϴ�, 4����Ʈ ���ۿ� �µ� �� ���� */
	memcpy(u16Temp, pData, size);
	/* ���� ���� �� ó���� ��Ŷ �������� �� ���� */
	for (i=0; i<MAX_PH; i++)
	{
		m_pstSystem->ph_mainboard_temp[i]	= SWAP16(u16Temp[i]);
		/* ���Ź��� Photohead�� Mainboard �µ� ���� 50�� ������ ������ */
		if (m_pstSystem->ph_mainboard_temp[i] > 500 /* ���� ���� ������ 10�� ����� �µ� ���� */)
		{
			TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL};
			swprintf_s(tzMesg, LOG_MESG_SIZE, L"Photohead [%u] : Temperature High Alarm (%.1f ��)",
					   i+1, m_pstSystem->ph_mainboard_temp[i]/10.0f);
		}
	}
}

/*
 desc : ���Ź��� ������ ó�� - System - Photoheads �������� ���� ���� ��
 parm : data	- [in]  ���ŵ� ������
		size	- [in]  'data' ������ ũ��
 retn : None
*/
VOID CSystem::SetRecvOverallErrorStatusMulti(PUINT8 data, UINT32 size)
{
	UINT8 i, u8PhCount		= 0;
	UINT32 u32Temp[MAX_PH]	= {NULL};	/* �ִ� 8 ���� Photohead�� Mainboard �µ� �� */
	PUINT8 pData			= data;

	/* ��Ŷ�� ũ�Ⱑ �ʹ� ū�� Ȯ�� */
	if (size > 5)	return;
	u8PhCount	= pData[0];
	/* �ϴ�, 4����Ʈ ���ۿ� ���� �� ���� */
	if (UINT32(u8PhCount) * sizeof(UINT32) > sizeof(UINT32) * MAX_PH)	return;
	memcpy(u32Temp, pData+1, UINT32(u8PhCount) * sizeof(UINT32));
	/* ���� ���� �� ó���� ��Ŷ �������� �� ���� */
	for (i=0; i<MAX_PH; i++)
	{
		m_pstSystem->get_overall_error_status_multi[i]	= SWAP32(u32Temp[i]);
		/* ���� ���� �� ������� ���� ���� ������ ������ ��� */
		SaveOverallErrorStatusMulti(i+1, m_pstSystem->get_overall_error_status_multi[i]);
	}
}

/*
 desc : ���Ź��� ������ ó�� - System - Photoheads�� Light Intensity ��
 parm : data	- [in]  ���ŵ� ������
		size	- [in]  'data' ������ ũ��
 retn : None
*/
VOID CSystem::SetRecvSetLightIntensity(PUINT8 data, UINT32 size)
{
}

/*
 desc : ���Ź��� ������ ó�� - System - LoadInternalTestImage ��
 parm : data	- [in]  ���ŵ� ������
		size	- [in]  'data' ������ ũ��
 retn : None
*/
VOID CSystem::SetRecvLoadInternalTestImage(PUINT8 data, UINT32 size)
{
	if (data[0] == 0x00)
	{
		memset(m_pstSystem->ph_load_internal_test_img_num, data[1], GetConfig()->luria_svc.ph_count);
	}
	else
	{
		m_pstSystem->ph_load_internal_test_img_num[data[0]-0x00]	= data[1];
	}
}

/*
 desc : ���� ��Ʈ�ѷ� ���� ���� ���� ���� ������ �α� ���Ͽ� ����
 parm : status	- [in]  ���� ���� ����� 32bit ��
 retn : None
*/
VOID CSystem::SaveMotorStatus(UINT32 status)
{
	if (0 == (0x00000007 & status))	return;

	TCHAR tzMesg[128]	= {NULL};
	if ((status & 0x00000001))
	{
		swprintf_s(tzMesg, 128, L"Motor : Could not communicate with motor [%08x]", status);
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
	}
	if ((status & 0x00000002))
	{
		swprintf_s(tzMesg, 128, L"Motor : Incorrect reply to command [%08x]", status);
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
	}
	if ((status & 0x00000004))
	{
		swprintf_s(tzMesg, 128, L"Motor : Error is reported [%08x]", status);
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
	}
}

/*
 desc : ���а� ���� ���� ���� ���� ������ �α� ���Ͽ� ����
 parm : status	- [in]  ���� ���� ����� 32bit ��
 retn : None
*/
VOID CSystem::SavePotoHeadStatus(UINT32 status)
{
	if (0 == (0xffff0000 & status))	return;

	TCHAR tzMesg[128]	= {NULL};
	if ((0x000000ff & status))
	{
		swprintf_s(tzMesg, 128, L"PH : Head must be initialized [%08x]", status);
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
	}
	if ((0x0000ff00 & status))
	{
		swprintf_s(tzMesg, 128, L"PH : Communication with head failed [%08x]", status);
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
	}
}

/*
 desc : Luria PC ���� ���� ���� ���� ������ �α� ���Ͽ� ����
 parm : status	- [in]  ���� ���� ����� 32bit ��
 retn : None
*/
VOID CSystem::SaveLuriaPCStatus(UINT32 status)
{
	if (0 == (0x00000007 & status))	return;

	TCHAR tzMesg[128]	= {NULL};
	if ((status & 0x00000001))
	{
		swprintf_s(tzMesg, 128, L"PC : License dongle not found [%08x]", status);
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
	}
	if ((status & 0x0000002))
	{
		swprintf_s(tzMesg, 128, L"PC : Disk(s) failed (must be replaced) [%08x]", status);
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
	}
	if ((status & 0x0000004))
	{
		swprintf_s(tzMesg, 128, L"PC : Disk is 90%% of more full [%08x]", status);
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
	}
}

/*
 desc : Luria�� ����� Photoheads�� �������� ���� ���� �� ����
 parm : ph_no	- [in]  Photohead Number (1 ~ 8)
		status	- [in]  ���� ���� ����� 32bit ��
 retn : None
*/
VOID CSystem::SaveOverallErrorStatusMulti(UINT8 ph_no, UINT32 status)
{
	if (0 == (0x000001ff & status))	return;

	TCHAR tzMesg[128]	= {NULL};
	if ((status & 0x00000001))
	{
		swprintf_s(tzMesg, 128, L"PH[%d] : Temp regulator 1 out of range [%08x]", ph_no, status);
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
	}
	if ((status & 0x0000002))
	{
		swprintf_s(tzMesg, 128, L"PH[%d] : Temp regulator 2 out of range [%08x]", ph_no, status);
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
	}
	if ((status & 0x0000004))
	{
		swprintf_s(tzMesg, 128, L"PH[%d] : Low ethernet link quality [%08x]", ph_no, status);
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
	}
	if ((status & 0x0000008))
	{
		swprintf_s(tzMesg, 128, L"PH[%d] : Board temp (FPGA temperature) too high [%08x]", ph_no, status);
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
	}
	if ((status & 0x0000010))
	{
		swprintf_s(tzMesg, 128, L"PH[%d] : Board fan stopped [%08x]", ph_no, status);
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
	}
	if ((status & 0x0000020))
	{
		swprintf_s(tzMesg, 128, L"PH[%d] : AF sensor position error [%08x]", ph_no, status);
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
	}
	if ((status & 0x0000040))
	{
		swprintf_s(tzMesg, 128, L"PH[%d] : Laser light-source speckle wheel error [%08x]", ph_no, status);
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
	}
	if ((status & 0x0000080))
	{
		swprintf_s(tzMesg, 128, L"PH[%d] : DMD temperature too high [%08x]", ph_no, status);
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
	}
	if ((status & 0x0000100))
	{
		swprintf_s(tzMesg, 128, L"PH[%d] : Humidity sensor reports too high humidity [%08x]", ph_no, status);
		LOG_ERROR(ENG_EDIC::en_luria, tzMesg);
	}
}