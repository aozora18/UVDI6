`
/*
 desc : IDS Camera SDK Interface
*/

#include "pch.h"
#include "MainApp.h"
#include "CamMain.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 desc : ������
 parm : config	- [in]  ȯ�� ����
 retn : None
*/
CCamMain::CCamMain(LPG_CIEA config)
{
	m_hCam				= 0;
	m_u32WhiteCount		= 0;
	m_pACamImgAct		= NULL;
	m_pACamImgCnv		= NULL;
	m_i32MemActID		= 0;
	m_i32MemCnvID		= 0;
	m_u64LastBandTime	= 0;
	m_i32LastError		= 0;
	m_pDragBuff			= NULL;

	m_stGrab.grab_data	= NULL;
#if (USE_EQUIPMENT_SIMULATION_MODE)
	m_stFileGrab.grab_data	= NULL;
#endif
	m_pHistLevel		= NULL;
	m_pGrayCount		= NULL;
	m_pGrabBuff			= NULL;

	m_pstConfig			= config;
	m_enCamMode			= ENG_VCCM::en_none;	// Grabbed Mode
	wmemset(m_tzErrMsg, 0x00, 1024);

	m_u8GrabIndex		= 0;

	memset(&m_stGrab, 0x00, sizeof(STG_ACGR));
#if (USE_EQUIPMENT_SIMULATION_MODE)
	memset(&m_stFileGrab, 0x00, sizeof(STG_ACGR));
#endif
	/* �ʴ� �߻��� �� �ִ� Frame ���� �� (�⺻ ��) */
	m_dbCurFPS			= 1.0f;

#if (USE_EQUIPMENT_SIMULATION_MODE)
	UINT32 i	= 0;
	/* ī�޶� ���� grab �Ǵ� �̹����� ����� ��ü ���� �޸� �Ҵ� */
	m_pGrabBuff		= (PUINT8*)Alloc(sizeof(PUINT8) * config->mark_find.max_find_count);
	m_pGrabBuff[0]	= (PUINT8)Alloc(UINT64(config->mark_find.max_find_count) *
									UINT64(config->GetACamGrabBytes()) +
									config->mark_find.max_find_count); 
	for (i=1; i<config->mark_find.max_find_count; i++)
	{
		m_pGrabBuff[i]	= m_pGrabBuff[i-1] + config->GetACamGrabBytes() + 1);
		m_pGrabBuff[i][config->GetACamGrabBytes()] = 0;
	}
#endif
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CCamMain::~CCamMain()
{
	if (m_pDragBuff)	::Free(m_pDragBuff);
#if (USE_EQUIPMENT_SIMULATION_MODE)
	if (m_stFileGrab.grab_data)	::Free(m_stFileGrab.grab_data);
#endif
	DetachDevice();
}

/*
 desc : ��Ʈ��ũ���� ���� PC�� ������ �� �ִ� ī�޶� ���� ���
 parm : None
 retn : ī�޶� �ڵ� ��ȯ
*/
HIDS CCamMain::GetCamHandle()
{
	INT32 i, i32Ret	= 0, i32CamCount = 0;
	HIDS hCam		= 0;
	PUEYE_CAMERA_LIST pstCamList	= NULL;

	/* ���� PC�� ����� ī�޶� ���� ��� */
	if (IS_SUCCESS != is_GetNumberOfCameras(&i32CamCount))	return 0;
	if (i32CamCount < 1)	return 0;
	/* ���� PC�� ����� ī�޶�� ���� ��� */
	pstCamList	= (PUEYE_CAMERA_LIST)::Alloc(sizeof(ULONG) + sizeof(UEYE_CAMERA_INFO) * UINT32(i32CamCount));
	ASSERT(pstCamList);
	pstCamList->dwCount	= i32CamCount;	/* �� �˻��� ī�޶� ���� ���� */
	/* ���� �ý���(���� ��Ʈ��ũ ����)�� ����� ī�޶� ���� ���� ��� */
	i32Ret	= is_GetCameraList(pstCamList);
	if (IS_NO_SUCCESS == i32Ret)	return 0;
	/* �˻��� ī�޶� ���� ��ŭ �о, ã���� �ϴ� ī�޶��� ID (�ڵ�) ��ȯ */
	for (i=0; i<i32CamCount; i++)
	{
		/* ������ ī�޶� �ø��� ��ȣ�� ������ ���� �˻��Ǹ�, �ڵ� �� ���� */
		if (UINT32(atoi(pstCamList->uci[i].SerNo)) == m_pstConfig->set_ids.serial_no)
		{
			/* Camera ID ���� */
			hCam	= HIDS(pstCamList->uci[i].dwDeviceID | IS_USE_DEVICE_ID);	/* or dwDeviceID ���߿� �ϳ��� ���� �� �ʹ� */
			break;	/* �˻� �������� Ȯ�� */
		}
	}

	/* �޸� ���� */
	if (pstCamList)	::Free(pstCamList);

	return hCam;
}

/*
 desc : ī�޶� ���� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamMain::InitACam()
{
	INT32 i32Ret	= 0;

	/* ����� ī�޶��� ���� ���� ��� */
	memset(&m_stSensor, 0x00, sizeof(SENSORINFO));
	memset(&m_stCamera, 0x00, sizeof(CAMINFO));

	if (IS_SUCCESS == i32Ret)	i32Ret	= is_GetSensorInfo(m_hCam, &m_stSensor);
	/* ī�޶� ���� ��� */
	if (IS_SUCCESS == i32Ret)	i32Ret	= is_GetCameraInfo(m_hCam, &m_stCamera);
	/* AOI ���� ũ�� ��� */
	if (IS_SUCCESS == i32Ret)	i32Ret	= GetAOISize();
	/* Parameter ���� */
	if (IS_SUCCESS == i32Ret)	i32Ret	= SetParameter();
	/* AOI ������ ��ŭ, ī�޶� ���ο� Grabbed Image�� �ӽ÷� ������ ���� �Ҵ� */
	if (IS_SUCCESS == i32Ret)	i32Ret	= SetAllocData();
#if 0
	/* �̹� ������ ȯ�� ���� ���� ���� (ī�޶� ���� �Ķ���Ͱ� ����� ȯ�� ���� ����) */
	i32Ret	= is_ParameterSet(m_hCam, IS_PARAMETERSET_CMD_SAVE_FILE, L"f:\\download\\set.ini", NULL);
#endif

	/* ���� �ڵ� �� ���ڿ� ��� */
	if (i32Ret != IS_SUCCESS)	SetLastError();

	return i32Ret == IS_SUCCESS;
}

/*
 desc : ī�޶� ���� ����
 parm : None
 retn : None
*/
VOID CCamMain::CloseACam()
{
	if (m_pACamImgAct)		is_FreeImageMem(m_hCam, (PCHAR)&m_pACamImgAct, m_i32MemActID);
	if (m_pACamImgCnv)		is_FreeImageMem(m_hCam, (PCHAR)&m_pACamImgCnv, m_i32MemCnvID);
	/* Gray Index �޸� ���� */
	if (m_pHistLevel)		Free(m_pHistLevel);
	if (m_pGrayCount)		Free(m_pGrayCount);

	// ť�� ����� Grabbed �̹��� ����
	ResetGrabbedImage();
	/* Grabbed Image Buffer �޸� ���� */
	if (m_pGrabBuff)
	{
		Free(m_pGrabBuff[0]);
		Free(m_pGrabBuff);
		m_pGrabBuff	= NULL;
	}

	m_pACamImgAct		= NULL;
	m_pACamImgCnv		= NULL;
	m_pHistLevel		= NULL;
	m_pGrayCount		= NULL;
}

/*
 desc : ī�޶� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamMain::AttachDevice()
{
	INT32 i32Ret	= 0, i32UploadTime	= (INT32)m_pstConfig->set_ids.upload_time_limit;

	m_hCam			= 0;
	m_i32BandWidth	= 0;

	/* ���� ����Ȱ� ���� */
	DetachDevice();
	/* ���� ������ ī�޶� �ڵ� ��� */
	m_hCam	= GetCamHandle();
	if (!m_hCam)	return FALSE;

	/* ī�޶� ���� ���� */
	i32Ret	= is_InitCamera(&m_hCam, NULL);
	if (IS_NO_SUCCESS == i32Ret)
	{
		SetLastError();
		return FALSE;
	}
	/* ī�޶� �߿��� ���ε� ���ο� ���� */
	if (IS_STARTER_FW_UPLOAD_NEEDED == i32Ret)
	{
		/* Time for the firmware upload = 25 seconds by default*/
		i32Ret	= is_GetDuration(m_hCam, IS_STARTER_FW_UPLOAD_NEEDED, &i32UploadTime);
		if (IS_NO_SUCCESS == i32Ret)
		{
			SetLastError();
			return FALSE;
		}
		/* ���� �ڵ� �̿��Ͽ� ����� ���� �ݱ� */
		is_ExitCamera(m_hCam);
		/* �ٽ� �ڵ� ID �Ҵ� */
		m_hCam	= (HIDS)((INT32)m_hCam | IS_ALLOW_STARTER_FW_UPLOAD);
		i32Ret	= is_InitCamera(&m_hCam, NULL);
		if (IS_NO_SUCCESS == i32Ret)
		{
			SetLastError();
			return FALSE;
		}
	}

	// enable/disable the dialog based error report
	i32Ret = is_SetErrorReport(m_hCam, IS_DISABLE_ERR_REP); // or IS_ENABLE_ERR_REP);
	if (IS_NO_SUCCESS == i32Ret)
	{
		SetLastError();
		return FALSE;
	}
	/* �ʱ�ȭ ���� ���� */
	if (!InitACam())	return FALSE;

	return TRUE;
}

/*
 desc : ī�޶� ���� ����
 parm : None
 retn : None
*/
VOID CCamMain::DetachDevice()
{
	CloseACam();

	/* ������ ����� ī�޶� �ִٸ� */
	if (0 != m_hCam/* && IsConnected()*/)
	{
		if (m_hCam)	is_ExitCamera(m_hCam);
		m_hCam				= 0;
		m_i32BandWidth		= 0;
		m_u64LastBandTime	= 0;
	}
}

/*
 desc : ���� ī�޶� ����Ǿ� �ִ��� ����
 parm : None
 retn : TRUE (����Ǿ� �ִ�) or FALSE
*/
BOOL CCamMain::IsConnected()
{
	INT32 i32Ret, i32Err;
	PCHAR pzErr	= NULL;

	/* ���� ���� �ȵǾ� �ִ� �����̸� */
	if (0 == m_hCam)	return FALSE;
	/* ���� ����Ǿ� �ִ� ī�޶� ���� Ȯ�� */
	i32Ret	= is_GetError(m_hCam, &i32Err, &pzErr);
	if (i32Ret != IS_SUCCESS)
	{
		/* ���� �߻��� ���� �� ���� */
		SetLastError();
		return FALSE;
	}

	/* ���� �ֱٿ� �߻��� �뿪�� ���� */
	if (GetTickCount64() - 1000 > m_u64LastBandTime)
	{
		m_u64LastBandTime	= GetTickCount64();
		m_i32BandWidth		= is_GetUsedBandwidth(m_hCam);
//		TRACE(L"badwidth = %d MBytes\n", m_i32BandWidth);
	}

	return m_i32BandWidth > 0;
}

/*
 desc : AOI ���� ���
 parm : None
 retn : IDS Error Value
*/
INT32 CCamMain::GetAOISize()
{
	INT32 i32SupportedAOI	= 0, i32Ret = IS_SUCCESS;
	IS_SIZE_2D stSizeAOI	= {NULL};

	/* Check if the camera supports an arbitrary AOI / Only the ueye xs does not support an arbitrary AOI */
	i32Ret	= is_ImageFormat(m_hCam, IMGFRMT_CMD_GET_ARBITRARY_AOI_SUPPORTED,
							 (VOID *)&i32SupportedAOI, sizeof(i32SupportedAOI));
	if (IS_NO_SUCCESS == i32Ret)	return i32Ret;
	if (i32SupportedAOI == 0)
	{
		AfxMessageBox(L"AOI function is not supported", MB_ICONSTOP|MB_TOPMOST);
		return -1;
	}

	/* �̹��� ������ �ػ� ũ�� ���� Grabbed Image ũ�Ⱑ ũ��, �̹��� �ػ� ũ��� ���� ���� */
	if (m_stSensor.nMaxWidth < m_pstConfig->GetACamGrabSize(0))	m_pstConfig->SetACamGrabSize(0x00, m_stSensor.nMaxWidth);
	if (m_stSensor.nMaxHeight< m_pstConfig->GetACamGrabSize(1))	m_pstConfig->SetACamGrabSize(0x01, m_stSensor.nMaxHeight);
#if 0
	/* AOI ������ ���� ��� */
	if (IS_SUCCESS == i32Ret)	i32Ret	= is_AOI(m_hCam, IS_AOI_IMAGE_GET_SIZE_MAX,
												 (VOID *)&stSizeAOI, sizeof(stSizeAOI));
	if (i32Ret != IS_SUCCESS)	return i32Ret;
	/* ī�޶� AOI �������� ȯ�� ���Ͽ� ������ AOI ���� ũ��, ������ */
	if (m_u32AoiWidth > UINT32(stSizeAOI.s32Width))		m_u32AoiWidth	= stSizeAOI.s32Width;
	if (m_u32AoiHeight > UINT32(stSizeAOI.s32Height))	m_u32AoiHeight	= stSizeAOI.s32Height;
#endif

	return i32Ret;
}

/*
 desc : ī�޶� ���ο� �ӽ÷� ����� �޸� ���� �Ҵ�
 parm : None
 ��ȯ IDS Error Value
*/
INT32 CCamMain::SetAllocData()
{
	INT32 i, i32Ret, i32ColorMode, i32Pitch;
	IS_SIZE_2D stSize2D		= {NULL};

	/* �÷� ��忡 ���� */
	if (m_stSensor.nColorMode == IS_COLORMODE_MONOCHROME)
	{
		m_i32BitPerPixel= 8;
		i32ColorMode	= IS_CM_MONO8;
	}
	else
	{
		m_i32BitPerPixel= 24;
		i32ColorMode	= IS_CM_BGR8_PACKED;
	}
	/* ī�޶� ���� �Ķ���� ���� - �⺻ ���� */
	i32Ret	= is_SetColorMode(m_hCam, IS_CM_MONO8);
	/* �޸� �Ҵ� */
	if (IS_SUCCESS == i32Ret)	i32Ret	= is_AllocImageMem(m_hCam,
														   m_pstConfig->GetACamGrabSize(0),
														   m_pstConfig->GetACamGrabSize(1),
														   m_i32BitPerPixel, (PCHAR*)&m_pACamImgAct,
														   &m_i32MemActID);
	/* �޸� ���� (Active) */
	if (IS_SUCCESS == i32Ret)	i32Ret	= is_SetImageMem(m_hCam, (PCHAR)m_pACamImgAct, m_i32MemActID);
	/* ���� ī�޶� �Ҵ�� �̹����� ũ�� ��� 1 line�� �� Bytes * Height */
	if (IS_SUCCESS == i32Ret)	i32Ret	= is_GetImageMemPitch(m_hCam, &i32Pitch);
	/* Image Color Mode */
	if (IS_SUCCESS == i32Ret)	i32Ret = is_SetDisplayMode(m_hCam, IS_SET_DM_DIB);

	/* Grabbed Image Size ��, Camera ���ο� �ӽ÷� ����Ǵ� Grabbed Data Size */
	if (i32Ret != IS_SUCCESS)	return i32Ret;
	m_u32GrabBytes	= UINT32(i32Pitch) * m_pstConfig->GetACamGrabSize(1);	/* unit : bytes */
	/* Gray Index ���� ������ ���� �Ҵ� */
	m_pGrayCount	= (PUINT32)Alloc(256 * sizeof(UINT32));
	ASSERT(m_pGrayCount);
	/* Gray Level ���� ������ ���� �Ҵ� */
	m_pHistLevel	= (PUINT64)Alloc(sizeof(UINT64) * m_pstConfig->GetACamGrabSize(0));
	ASSERT(m_pHistLevel);

	// Live Mode �ӽ� �����
	memset(&m_stGrab, 0x00, sizeof(STG_ACGR));
	/* ī�޶� ���� grab �Ǵ� �̹����� ����� ��ü ���� �޸� �Ҵ� */
	m_pGrabBuff		= (PUINT8*)Alloc(sizeof(PUINT8) *
									 UINT64(m_pstConfig->mark_find.max_find_count));
	m_pGrabBuff[0]	= (PUINT8)Alloc(UINT64(m_pstConfig->mark_find.max_find_count) *
									UINT64(m_pstConfig->GetACamGrabBytes()) + m_pstConfig->mark_find.max_find_count);
	for (i=1; i<MAX_GRABBED_IMAGE; i++)
	{
		m_pGrabBuff[i]	= m_pGrabBuff[i-1] + (m_pstConfig->GetACamGrabBytes() + 1);
		m_pGrabBuff[i][m_pstConfig->GetACamGrabBytes()] = 0;
	}

	return i32Ret;
}

/*
 desc : Pixel Clock / Frame Rate / Expose Time ����
 parm : None
 retn : IDS Error Value
*/
INT32 CCamMain::SetParameter()
{
	INT32 i32Ret		= 0, i32New;
	UINT32 u32Range[3]	= {NULL}, u32New;
	DOUBLE dbMin, dbMax, dbSet, dbNew, dbEnable = 0.0f;
	IS_RANGE_S32 stRange= {NULL};
	IS_RECT stRectAOI	= {NULL};

	/* Display Mode */
	if (i32Ret == IS_SUCCESS)	i32Ret	= is_SetDisplayMode(m_hCam, IS_SET_DM_DIB);
	/* Set Auto Parameter : Disabled */
	if (i32Ret == IS_SUCCESS)	i32Ret	= is_SetAutoParameter(m_hCam, IS_SET_ENABLE_AUTO_GAIN,			&dbEnable, NULL);
	if (i32Ret == IS_SUCCESS)	i32Ret	= is_SetAutoParameter(m_hCam, IS_SET_ENABLE_AUTO_SHUTTER,		&dbEnable, NULL);
	if (i32Ret == IS_SUCCESS)	i32Ret	= is_SetAutoParameter(m_hCam, IS_SET_ENABLE_AUTO_FRAMERATE,		&dbEnable, NULL);
	if (i32Ret == IS_SUCCESS)	i32Ret	= is_SetAutoParameter(m_hCam, IS_SET_AUTO_REFERENCE,			&dbEnable, NULL);

	/* AOI Setup�� ī�޶��� �߽����� AOI ���� ���� */
	stRectAOI.s32X		= (INT32)ROUNDED((m_stSensor.nMaxWidth - m_pstConfig->GetACamGrabSize(0)) / 2.0f, 0);
	stRectAOI.s32Y		= (INT32)ROUNDED((m_stSensor.nMaxHeight- m_pstConfig->GetACamGrabSize(1)) / 2.0f, 0);
	stRectAOI.s32Width	= m_pstConfig->GetACamGrabSize(0);
	stRectAOI.s32Height	= m_pstConfig->GetACamGrabSize(1);
	if (IS_SUCCESS == i32Ret)	i32Ret	= is_AOI(m_hCam, IS_AOI_IMAGE_SET_AOI,
													(VOID *)&stRectAOI, sizeof(stRectAOI));
	/* Disable Gain && Gamma */
	if (i32Ret == IS_SUCCESS)	i32Ret	= is_SetHardwareGain(m_hCam, 0, 0, 0, 0);
	if (i32Ret == IS_SUCCESS)	i32Ret	= is_SetHardwareGamma(m_hCam, m_pstConfig->set_ids.use_gamma);
	if (m_pstConfig->set_ids.use_gamma)
	{
		i32New	= 100 + (INT32)ROUNDED(100 * (m_pstConfig->set_ids.gamma_rate / 100.0f), 0);
		is_Gamma(m_hCam, IS_GAMMA_CMD_SET, (VOID*)&i32New, sizeof(i32New));
	}

	/* Binning ó�� */
	if (i32Ret == IS_SUCCESS && m_pstConfig->set_ids.use_binning)
	{
		i32Ret	= is_SetBinning(m_hCam, IS_BINNING_2X_VERTICAL|IS_BINNING_2X_HORIZONTAL);
	}
	/* Scaler ó�� */
	if (i32Ret == IS_SUCCESS && m_pstConfig->set_ids.use_scaler)
	{
		SENSORSCALERINFO stInfo	= {NULL};
		i32Ret	= is_GetSensorScalerInfo(m_hCam, &stInfo, sizeof(stInfo));
		if (i32Ret == IS_SUCCESS)
		{
			dbNew	= stInfo.dblMinFactor + (stInfo.dblMaxFactor - stInfo.dblMinFactor) *
											 DOUBLE(m_pstConfig->set_ids.scale_rate / 100.0f);
			i32Ret	= is_SetSensorScaler(m_hCam, IS_ENABLE_SENSOR_SCALER|IS_ENABLE_ANTI_ALIASING, dbNew);
		}
	}
	else
	{
		dbNew	= 0.0f;
		i32Ret	= is_SetSensorScaler(m_hCam, IS_DISABLE_SENSOR_SCALER, dbNew);
	}

	/* ���� ī�޶��� Pixel Clock �⺻ �� ��� ���� */
	if (i32Ret == IS_SUCCESS)
	{
		i32Ret	= is_PixelClock(m_hCam, IS_PIXELCLOCK_CMD_GET_RANGE, (VOID*)u32Range, sizeof(u32Range));
		if (i32Ret == IS_SUCCESS)
		{
			u32New	= u32Range[0] /* Min */ +
					  (INT32)ROUNDED((u32Range[1] /* Max */ - u32Range[0]) *
									 DOUBLE(m_pstConfig->set_ids.pixel_clock_rate/100.0f), 0);
			i32Ret	= is_PixelClock(m_hCam, IS_PIXELCLOCK_CMD_SET, &u32New, sizeof(u32New));
		}
	}
#if 1	/* ���������� �����ϴ� ����� �𸣰ڴ�. �ϴ� �ּ� ó��. ���������� �ڵ� �����ϵ��� */
	/* Frame Rate ���� */
	if (i32Ret == IS_SUCCESS)
	{
#if 0
		i32Ret	= is_GetFrameTimeRange(m_hCam, &dbMin, &dbMax, &dbSet);
		if (i32Ret == IS_SUCCESS)
		{
			dbSet	= dbMin + (dbMax - dbMin) * (m_pstConfig->set_ids.frame_rate / 100.0f);
			i32Ret	= is_SetFrameRate(m_hCam, dbSet, &dbNew);
			if (i32Ret == IS_SUCCESS)	m_dbCurFPS	= dbNew;
		}
#else
		i32Ret = is_SetFrameRate(m_hCam, IS_GET_DEFAULT_FRAMERATE, &dbSet);
		if (i32Ret == IS_SUCCESS)	i32Ret = is_SetFrameRate(m_hCam, dbSet, &dbNew);
		if (i32Ret == IS_SUCCESS)	m_dbCurFPS	= dbNew;
#endif
	}
#endif
	/* Global Shutter ���� */
	i32New	= IS_DEVICE_FEATURE_CAP_SHUTTER_MODE_GLOBAL;
	if (i32Ret == IS_SUCCESS)	i32Ret	= is_DeviceFeature(m_hCam, IS_DEVICE_FEATURE_CMD_SET_SHUTTER_MODE, (VOID*)&i32New, sizeof(i32New));

	/* ���� ī�޶��� Expose Time Range (Min ~ Max) ��� */
	if (i32Ret == IS_SUCCESS)	i32Ret	= is_Exposure(m_hCam, IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MIN, &dbMin, sizeof(DOUBLE));
	if (i32Ret == IS_SUCCESS)	i32Ret	= is_Exposure(m_hCam, IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MAX, &dbMax, sizeof(DOUBLE));
	/* �����ϰ��� �ϴ� ���� Min ~ Max �� ���̿� �������� ������, �߰� ������ ���� */
	if (i32Ret == IS_SUCCESS)
	{
		/* dbMin ~ dbMax ���̿���, Percentage ����ŭ �� ���� */
		dbSet	= dbMin + (dbMax - dbMin) * (m_pstConfig->set_ids.expose_time_rate / 100.0f);
		/* Expose Time (unit : ms) �� ���� */
		i32Ret	= is_Exposure(m_hCam, IS_EXPOSURE_CMD_SET_EXPOSURE, &dbSet, sizeof(DOUBLE));
	}

	/* Mirror ��/�� or ��/�� ���� ���� */
	if (m_pstConfig->set_ids.mirror_updown && i32Ret == IS_SUCCESS)	i32Ret = is_SetRopEffect(m_hCam, IS_SET_ROP_MIRROR_UPDOWN, 1, 0);
	if (m_pstConfig->set_ids.mirror_leftright && i32Ret == IS_SUCCESS)	i32Ret = is_SetRopEffect(m_hCam, IS_SET_ROP_MIRROR_LEFTRIGHT, 1, 0);

	/* Edge Enhancement ���� */
	if (m_pstConfig->set_ids.edge_act_rate && i32Ret == IS_SUCCESS)
	{
		/* Edge Range ��� */
		i32Ret = is_EdgeEnhancement(m_hCam, IS_EDGE_ENHANCEMENT_CMD_GET_RANGE,
									(VOID*)u32Range, sizeof(u32Range));
		if (i32Ret == IS_SUCCESS)
		{
			u32New	= u32Range[0] +
					  (UINT32)ROUNDED((u32Range[1] - u32Range[0]) * (m_pstConfig->set_ids.edge_act_rate / 100.0f), 0);
			i32Ret = is_EdgeEnhancement(m_hCam, IS_EDGE_ENHANCEMENT_CMD_SET, (VOID*)&u32New, sizeof(u32New));
		}
	}

	/* Black Level (�̹����� ������ Level ���� ����) */
	if (m_pstConfig->set_ids.use_black_level && i32Ret == IS_SUCCESS)
	{
		/* Edge Range ��� */
		i32Ret = is_Blacklevel(m_hCam, IS_BLACKLEVEL_CMD_GET_OFFSET_RANGE,
									(VOID*)&stRange, sizeof(stRange));
		if (i32Ret == IS_SUCCESS)
		{
			i32New	= stRange.s32Min +
					  (INT32)ROUNDED((stRange.s32Max - stRange.s32Min) *
									 (m_pstConfig->set_ids.black_level_rate / 100.0f), 0);
			i32Ret = is_Blacklevel(m_hCam, IS_BLACKLEVEL_CMD_SET_OFFSET, (VOID*)&i32New, sizeof(i32New));
		}
	}
	/* ī�޶� ������ �ҷ� ȭ�ҿ� ���� HotPixel ��� ���� */
	if (m_pstConfig->set_ids.use_hotpixel && i32Ret == IS_SUCCESS)
	{
		i32Ret	= is_HotPixel(m_hCam, IS_HOTPIXEL_GET_SUPPORTED_CORRECTION_MODES,
							  (VOID*)&i32New, sizeof(i32New));
		if (i32Ret == IS_SUCCESS &&
			IS_HOTPIXEL_ENABLE_CAMERA_CORRECTION == (i32New & IS_HOTPIXEL_ENABLE_CAMERA_CORRECTION))
		{
			i32Ret	= is_HotPixel(m_hCam, IS_HOTPIXEL_ENABLE_CAMERA_CORRECTION, NULL, NULL);
		}
	}

#if 0	/* Live View������ ������ �ʰ�, ���α׷� ���ᵵ �ȵ�. �Ƹ�, Single Capture ������ ��, ����Ǵ� �� */
	/* ī�޶� ���� ��� ���� */
	i32New	= 0x00;
	switch (m_pstConfig->set_ids.shutter_mode)
	{
	case 0x00 :	i32New	= IS_DEVICE_FEATURE_CAP_SHUTTER_MODE_ROLLING;					break;
	case 0x01 :	i32New	= IS_DEVICE_FEATURE_CAP_SHUTTER_MODE_ROLLING_GLOBAL_START;		break;
	case 0x02 :	i32New	= IS_DEVICE_FEATURE_CAP_SHUTTER_MODE_GLOBAL;					break;
	case 0x03 :	i32New	= IS_DEVICE_FEATURE_CAP_SHUTTER_MODE_GLOBAL_ALTERNATIVE_TIMING;	break;
	}
	if (i32Ret == IS_SUCCESS)
	{
		i32Ret	= is_DeviceFeature(m_hCam, IS_DEVICE_FEATURE_CMD_SET_SHUTTER_MODE, (VOID*)&i32New, sizeof(i32New));
	}
#endif
	return i32Ret;
}

/*
 desc : Live Grabbed Image ó��
 parm : by_grab	- [in]  ���� Bayer Image Format���ε� Grabbed Imaged Data
 retn : None
*/
VOID CCamMain::SetLiveGrabbedImage(PUINT8 by_grab)
{
	// ���� Grabbed�� �̹��� ���� (���) ó��
	m_stGrab.cam_id			= 0x01;
	m_stGrab.img_id			= 0;
	m_stGrab.grab_size		= m_u32GrabBytes;
	m_stGrab.grab_width		= m_pstConfig->GetACamGrabSize(0);
	m_stGrab.grab_height	= m_pstConfig->GetACamGrabSize(1);
	m_stGrab.grab_data		= by_grab;
}

/*
 desc : File Grabbed Image ó��
 parm : size	- [in]  Grabbed Image Size (Unit : Bytes)
		width	- [in]  Grabbed Image Width
		height	- [in]  Grabbed Image Height
		grab	- [in]  Grabbed Image Data
 retn : None
*/
#if (USE_EQUIPMENT_SIMULATION_MODE)
VOID CCamMain::SetFileGrabbedImage(UINT32 size, UINT32 width, UINT32 height, PUINT8 grab)
{
	m_stFileGrab.cam_id			= 0x01;
	m_stFileGrab.img_id			= 0;
	m_stFileGrab.grab_size		= size;
	m_stFileGrab.grab_width		= width;
	m_stFileGrab.grab_height	= height;
	/* �޸� �ӽ� �Ҵ�  */
	if (m_stFileGrab.grab_data)	::Free(m_stFileGrab.grab_data);
	m_stFileGrab.grab_data		= (PUINT8)::Alloc(size+1);
	m_stFileGrab.grab_data[size]= 0x00;
	memcpy(m_stFileGrab.grab_data, grab, size);
}
#endif
/*
 desc : Grabbed Image ȹ�� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamMain::GetSingleGrabbedImage()
{
	INT32 i32Ret	= 0;
	BOOL bGrabbed	= FALSE;
	UINT64 u64Tick	= GetTickCount64();

	/* Single Image Grabbed ó��*/
#if 0
	i32Ret	= is_FreezeVideo(m_hCam, IS_DONT_WAIT);
#else
	i32Ret	= is_FreezeVideo(m_hCam, IS_WAIT);
#endif
	if (IS_NO_SUCCESS == i32Ret)
	{
		SetLastError();
		return FALSE;
	}
#if 0	/* ����� ������ ����ؾ߸� �� */
	if (uvData_Info_GetData()->common->use_grab_image_save && m_enCamMode != ENG_VCCM::en_live_mode)
	{
		TCHAR tzFile[_MAX_PATH]	= {NULL};
		SYSTEMTIME stNow	= {NULL};
		GetLocalTime(&stNow);
		swprintf_s(tzFile, L"g:\\download\\ids\\grab_file_%02d%02d%02d.bmp", stNow.wHour, stNow.wMinute, stNow.wSecond);
		IMAGE_FILE_PARAMS ImageFileParams;
		ImageFileParams.pwchFileName = tzFile;
		ImageFileParams.pnImageID = NULL;
		ImageFileParams.ppcImageMem = NULL;
		ImageFileParams.nQuality = 0;
		// Save bitmap from active memory to file (with file open dialog)
		ImageFileParams.nFileType = IS_IMG_BMP;
		INT nRet = is_ImageFile(m_hCam, IS_IMAGE_FILE_CMD_SAVE, (void*)&ImageFileParams, sizeof(ImageFileParams));
	}
#endif
#if 0
	/* ���� �ð� ���� Grabbed Image ��ȸ */
	do {

		i32Ret	= is_IsVideoFinish(m_hCam, &bGrabbed);
		if (i32Ret == IS_SUCCESS)	break;
		/* ��ð� ��� ����� ��� */
		if (GetTickCount64()-u64Tick > 1000)
		{
			TRACE(L"Wait Timeout Grabbed\n");
			return FALSE;
		}

	} while (!bGrabbed);
#endif
#if 0
	/* Vision Stop */
	if (i32Ret == IS_SUCCESS)
	{
		i32Ret	= is_StopLiveVideo(m_hCam, IS_WAIT);
		if (IS_NO_SUCCESS == i32Ret)	SetLastError();
	}
#endif
	return TRUE;
}

/*
 desc : ī�޶� ������ �̹��� ���ۿ� Single Grabbed Image ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamMain::RunGrabbedImage()
{
#if (0 == USE_EQUIPMENT_SIMULATION_MODE)
	INT32 i32Ret	= 0;
	PVOID pGrabMem	= NULL;
	if (!m_hCam)	return FALSE;
	/* Single Image Grabbed ó��*/
	if (!GetSingleGrabbedImage())	return FALSE;
	/* ���� �ֱ��� Grabbed Image �޸� ��ġ ��� */
	i32Ret	= is_GetImageMem(m_hCam, &pGrabMem);
	if (IS_NO_SUCCESS == i32Ret)
	{
		SetLastError();
		return FALSE;
	}
#endif
	/* ���� ��Ŀ� ���� */
	switch (m_enCamMode)
	{
	case ENG_VCCM::en_live_mode	:
	case ENG_VCCM::en_edge_mode	:
	case ENG_VCCM::en_line_mode	:	/* Line Edge Detection Mode */
#if (0 == USE_EQUIPMENT_SIMULATION_MODE)
	case ENG_VCCM::en_cali_mode	:	SetLiveGrabbedImage((PUINT8)pGrabMem);	break;
#endif
	case ENG_VCCM::en_grab_mode	:
		if (m_syncGrab.Enter())
		{
			/* Grabbed�� ���������� ����Ǿ����� ���� �� ���� Grabbed �Ǿ� ����� �̹����� ���� �ʰ� ���� */
			if (m_lstMark.GetCount() <= m_pstConfig->mark_find.max_find_count)
			{
				LPG_ACGR pstMark	= NULL;
				pstMark	= (LPG_ACGR)::Alloc(sizeof(STG_ACGR));
				ASSERT(pstMark);
				/* ���� Grabbed�� �̹��� ���� (���) ó�� */
				pstMark->cam_id		= 0x01;
				pstMark->img_id		= m_u8GrabIndex;
				pstMark->grab_width	= m_pstConfig->GetACamGrabSize(0);
				pstMark->grab_height= m_pstConfig->GetACamGrabSize(1);
				pstMark->grab_data	= m_pGrabBuff[m_u8GrabIndex];
#if (0 == USE_EQUIPMENT_SIMULATION_MODE)
				pstMark->grab_size	= m_u32GrabBytes;
				memcpy(pstMark->grab_data, pGrabMem, pstMark->grab_size);
#else
				pstMark->grab_size	= m_stFileGrab.grab_size;
				memcpy(pstMark->grab_data, m_stFileGrab.grab_data, pstMark->grab_size);
#endif
				pstMark->grab_data[pstMark->grab_size]	= 0x00;
				m_lstMark.AddTail(pstMark);
				/* ���� ��ũ �˻��ϱ� ���� ���� ó��*/
				m_u8GrabIndex++;
			}
			// ����ȭ ����
			m_syncGrab.Leave();
		}
		break;
	}
	return TRUE;
}

/*
 desc : Open�� ī�޶��� Event ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamMain::SetParamUpdate()
{
	/* ���� �Ҵ�� �޸� �� ��Ÿ ���� ���� */
	CloseACam();
	/* ���ο� ������ ���� �޸� �Ҵ� */
	return InitACam();
}

/*
 desc : ť�� ����� ������ ����
 parm : None
 retn : None
*/
VOID CCamMain::ResetGrabbedImage()
{
	POSITION pPos	= NULL;
	LPG_ACGR pstGrab= NULL;

	// ����ȭ ����
	if (m_syncGrab.Enter())
	{
		m_u8GrabIndex	= 0x00;
		pPos	= m_lstMark.GetHeadPosition();
		while (pPos)
		{
			pstGrab	= m_lstMark.GetNext(pPos);
			if (pstGrab)	::Free(pstGrab);
		}
		m_lstMark.RemoveAll();
		// ����ȭ ����
		m_syncGrab.Leave();
	}
}

/*
 desc : Grabbed �̹��� ��ȯ
 parm : grab	- [out] Grabbed Image�� ���� ��ȯ
 retn : ����ü ������
*/
LPG_ACGR CCamMain::GetGrabbedImage()
{
	LPG_ACGR pstGrab	= NULL;
	switch (m_enCamMode)
	{
	// Live Mode
	case ENG_VCCM::en_live_mode	:
	case ENG_VCCM::en_cali_mode	:
	case ENG_VCCM::en_edge_mode	:
	case ENG_VCCM::en_line_mode	:
		pstGrab	= &m_stGrab;
		break;
	// Grabbed Mode
	case ENG_VCCM::en_grab_mode	:
		// ����ȭ ����
		if (m_syncGrab.Enter())
		{
			// ���� ����Ǿ� �ִ� Grabbed Image�� �ִ��� Ȯ��
			if (m_lstMark.GetCount())
			{
				// �� ó���� ��ϵǾ� �ִ� �׸� ��ȯ
				pstGrab	= m_lstMark.GetHead();
				// �� ó���� ��ϵǾ� �ִ� �׸� ����
				m_lstMark.RemoveHead();
			}
			// ����ȭ ����
			m_syncGrab.Leave();
		}
		break;
	}
	return pstGrab;
}

/*
 desc : Live Grabbed Mode ���� ����
 parm : mode	- [in]  ENG_VCCM::en_xxx
 retn : None
*/
VOID CCamMain::SetCamMode(ENG_VCCM mode)
{
#if 0
	m_stGrab.grab_size		= 0;
	m_stGrab.grab_width		= 0;
	m_stGrab.grab_height	= 0;
#endif
	m_enCamMode				= mode;

#if 0	/* ���� ������ �ʿ� ����. �뱤 ������ ��, �����Ѵ�. */
	ResetGrabbedImage();	/* ���� Grabbed�� �̹��� ��� ����� */
#endif
}

/*
 desc : ���� Grab Grabbed Image �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CCamMain::ResetGrabbedMark()
{
	/* �⺻ ���� �ʱ�ȭ */
	m_stGrab.cam_id			= 0;
	m_stGrab.grab_size		= 0;
	m_stGrab.grab_width		= 0;
	m_stGrab.grab_height	= 0;
	m_u32WhiteCount			= 0;
}

/*
 desc : ���� Grabbed Image�� ���� �� Bytes ���� ����� Gray �� ���� ���
 parm : mode	- [in]  Grabbed Image�� ĸó�ϴ� ��� (���)
 retn : TRUE or FALSE
*/
BOOL CCamMain::SetGrabGrayRate(ENG_VCCM mode)
{
	UINT32 i	= 0;

	/* Gray Index �޸� �ʱ�ȭ */
	memset(m_pGrayCount, 0x00, 256 * sizeof(UINT32));

	/* ���� Grabbed Image�� Grab Mode�� ��ġ���� �ʴ��� ���� */
	if (m_enCamMode != mode)	return FALSE;
	/* Grabbed Image�� �ִ��� ���� */
	if (m_stGrab.grab_size < 1)	return FALSE;
	/* �̹��� ũ�� ��, ����Ʈ �� ��ŭ ���� �ݺ��ϸ鼭 ���� ���� �ջ� ó�� */
	m_u32WhiteCount	= 0;
	for (; i<m_stGrab.grab_size; i++)
	{
		m_pGrayCount[m_stGrab.grab_data[i]]++;	/* �� Gray Index ���� ���� ���� ó�� */
		if (m_stGrab.grab_data[i])	m_u32WhiteCount++;
	}

	return TRUE;
}

/*
 desc : ���� Grabbed Image�� ���� �� Gray Level �� (����) ��ȯ
 parm : index	- [in]  0 ~ 255�� �ش�Ǵ� Gray Level Index
 retn : 0 ~ 255 ���� Grabbed Image���� ���� �� ��ȯ
*/
DOUBLE CCamMain::GetGrabGrayRate(UINT8 index)
{
	/* Grabbed Image�� �ִ��� ���� */
	if (m_stGrab.grab_size < 1)	return 0.0f;

	return (m_pGrayCount[index] / DOUBLE(m_stGrab.grab_size)) * 100.0f;
}

/*
 desc : ��ü Grabbed Image���� ������ �ִ� ���� ���� ��ȯ
 parm : None
 retn : White ���� �� ��ȯ
*/
DOUBLE CCamMain::GetGrabGrayRateTotal()
{
	if (m_stGrab.grab_size < 1)	return 0.0f;
	return (m_u32WhiteCount / DOUBLE(m_stGrab.grab_size)) * 100.0f;
}

/*
 desc : ���� Grabbed Image�� ���� �� Bytes ���� ����� Gray �� ����
		���̿� ���� ���̿� �ִ� ��� Gray ���� �ջ��Ͽ� ����
 parm : mode	- [in]  Grabbed Image�� ĸó�ϴ� ��� (���)
 retn : TRUE or FALSE
*/
BOOL CCamMain::SetGrabHistLevel(ENG_VCCM mode)
{
	UINT32 i, j;

	/* Gray Index �޸� �ʱ�ȭ */
	memset(m_pGrayCount, 0x00, 256 * sizeof(UINT32));

	/* ���� Grabbed Image�� Grab Mode�� ��ġ���� �ʴ��� ���� */
	if (m_enCamMode != mode)	return FALSE;
	/* Grabbed Image�� �ִ��� ���� */
	if (m_stGrab.grab_size < 1)	return FALSE;

	/* ���� ������ �޸� �޸� �ʱ�ȭ */
	memset(m_pHistLevel, 0x00, sizeof(UINT64) * m_stGrab.grab_width);
	m_u64HistTotal	= 0;
	m_u64HistMax	= 0;
	/* �̹��� ũ�� ��, ����Ʈ �� ��ŭ ���� �ݺ��ϸ鼭 ���� ���� �ջ� ó�� */
	for (i=0; i<m_stGrab.grab_size; i++)
	{
		/* �ش� I ��° ��ġ�� Array�� ���� �ȼ��� Gray �� �ջ� ó�� */
		j	= i % m_stGrab.grab_width;
		m_pHistLevel[j]	+= m_stGrab.grab_data[i];	/* �� Gray ���� �ջ� ó�� */
		m_u64HistTotal	+= m_stGrab.grab_data[i];	/* Grabbed Image�� ��ü Gray Index �� �� �� */
		/* �� Column ���� ���� (Row) �������� ����Ǿ� �ִ� Gray Level ���� �� ���� ū �� ���� */
		if (m_pHistLevel[j] > m_u64HistMax)	m_u64HistMax = m_pHistLevel[j];
	}

	return TRUE;
}

/*
 desc : ���� Grabbed Image�� ���� �� Bytes ���� ����� Gray Level ��ȯ
 parm : None
 retn : ���� Grabbed Image�� ���� (Column)�� 1 �ȼ����� ������ Level ������ ����Ǿ� �ִ� ���� ��ȯ
*/
PUINT64 CCamMain::GetGrabHistLevel()
{
#if 0
	if (m_stGrab.grab_size < 1)	return NULL;
#endif
	return m_pHistLevel;
}

/*
 desc : ���� �ڵ忡 ���� ���� (���ڿ�) ����
 parm : None
 retn : None
*/
VOID CCamMain::SetLastError()
{
	INT32 i32LastErr	= 0, i32Ret;
	CHAR *pszLastErr	= NULL;
	CUniToChar csCnv;

	/* ���� �ֱٿ� �߻��� ī�޶��� ���� �޽��� */
	i32Ret	= is_GetError(m_hCam, &i32LastErr, &pszLastErr);
	/* ������ �߻��� ������ �������� ���ο� ���� */
	if (i32LastErr == 0 || i32LastErr == m_i32LastError)	return;
	m_i32LastError	= i32LastErr;

	/* �ֱٿ� �߻��� ���� �޽��� ���� */
	wmemset(m_tzErrMsg, 0x00, 1024);
	swprintf_s(m_tzErrMsg, 1024, L"Failed to set the IDS Camera : [%d] %s", i32LastErr, csCnv.Ansi2Uni(pszLastErr));
#ifdef _DEBUG
	AfxMessageBox(m_tzErrMsg, MB_ICONSTOP|MB_TOPMOST);
#else
	LOG_SAVED(ENG_LNWE::en_warning, m_tzErrMsg);
#endif
}

/*
 desc : ���� �ֱٿ� ����� Live Image�� ���Ϸ� ����
 parm : file	- [in]  ����� ���� �̸� (��ü ��� ����)
 retn : TRUE or FALSE
*/
BOOL CCamMain::SaveLiveImageToFile(TCHAR *file)
{
	INT32 i32Ret	= IS_SUCCESS;
	PVOID pGrabMem	= NULL;
	IMAGE_FILE_PARAMS stImgParam	= {NULL};

	/* ���� �ֱ��� Grabbed Image �޸� ��ġ ��� */
	i32Ret	= is_GetImageMem(m_hCam, &pGrabMem);
	if (i32Ret != IS_SUCCESS)
	{
		SetLastError();
		return FALSE;
	}

	/* �Ķ���Ϳ� �⺻ �� ���� */
	stImgParam.nFileType	= IS_IMG_BMP;
	stImgParam.pwchFileName	= file;
	stImgParam.ppcImageMem	= (PCHAR*)&pGrabMem;
	/* ���Ϸ� ���� */
	i32Ret	= is_ImageFile(m_hCam, IS_IMAGE_FILE_CMD_SAVE, (VOID *)&stImgParam, sizeof(stImgParam));
	if (i32Ret != IS_SUCCESS)
	{
		SetLastError();
		return FALSE;
	}

	return TRUE;
}

/*
 desc : ���� �ֱٿ� ����� Live Image���� Drag ���� �̹��� ���� ���
 parm : drag	- [in]  Live Image ������ Drag ������ ������ ��ǥ ���� ���� (����: �ȼ�)
 retn : drag �̹��� ������ �����ϰ� �ִ� ���� ������
*/
PUINT8 CCamMain::GetLastLiveDragImage(RECT drag)
{
	UINT32 i, u32BuffSize, u32LiveMem, u32DragLineSize = drag.right - drag.left;
	PUINT8 pLiveBuff	= m_stGrab.grab_data;	/* ���� �ֱ��� Live Image�� ����� ���� ������ ���� */
	PUINT8 pDragBuff	= NULL;

	if (!pLiveBuff)	return NULL;

	/* Drag �̹��� ���� ũ�� */
	u32BuffSize	= (drag.right - drag.left) * (drag.bottom - drag.top);	/* bytes */
	/* ���� Drag �̹����� �����ϰ� �ִ� ���۰� �Ҵ� �Ǿ� �ִٸ� ���� */
	if (m_pDragBuff)	::Free(m_pDragBuff);
	/* Drag Image Buffer �Ҵ� */
	m_pDragBuff	= (PUINT8)::Alloc(u32BuffSize+1);
	m_pDragBuff[u32BuffSize]	= 0x00;
	pDragBuff	= m_pDragBuff;

	/* Drag ������ ���� ���� */
	for (i=UINT32(drag.top); i<UINT32(drag.bottom); i++)
	{
		/* Live ���� ���� ���� ��ġ */
		u32LiveMem	= i * m_pstConfig->GetACamGrabSize(0) + drag.left;
		/* Drag ������ �̹��� ���� */
		memcpy(pDragBuff, pLiveBuff + u32LiveMem, u32DragLineSize);
		/* ������ ����� Drag ���� ��ġ�� �̵� */
		pDragBuff	+= u32DragLineSize;
	}

	return m_pDragBuff;
}