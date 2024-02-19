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
 desc : 생성자
 parm : config	- [in]  환경 정보
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
	/* 초당 발생할 수 있는 Frame 개수 값 (기본 값) */
	m_dbCurFPS			= 1.0f;

#if (USE_EQUIPMENT_SIMULATION_MODE)
	UINT32 i	= 0;
	/* 카메라 별로 grab 되는 이미지가 저장될 전체 버퍼 메모리 할당 */
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
 desc : 파괴자
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
 desc : 네트워크에서 현재 PC에 연결할 수 있는 카메라 정보 얻기
 parm : None
 retn : 카메라 핸들 반환
*/
HIDS CCamMain::GetCamHandle()
{
	INT32 i, i32Ret	= 0, i32CamCount = 0;
	HIDS hCam		= 0;
	PUEYE_CAMERA_LIST pstCamList	= NULL;

	/* 현재 PC에 연결될 카메라 개수 얻기 */
	if (IS_SUCCESS != is_GetNumberOfCameras(&i32CamCount))	return 0;
	if (i32CamCount < 1)	return 0;
	/* 현재 PC에 연결된 카메라들 정보 얻기 */
	pstCamList	= (PUEYE_CAMERA_LIST)::Alloc(sizeof(ULONG) + sizeof(UEYE_CAMERA_INFO) * UINT32(i32CamCount));
	ASSERT(pstCamList);
	pstCamList->dwCount	= i32CamCount;	/* 총 검색된 카메라 개수 설정 */
	/* 현재 시스템(동일 네트워크 영역)에 연결된 카메라 개수 정보 얻기 */
	i32Ret	= is_GetCameraList(pstCamList);
	if (IS_NO_SUCCESS == i32Ret)	return 0;
	/* 검색된 카메라 개수 만큼 읽어서, 찾고자 하는 카메라의 ID (핸들) 반환 */
	for (i=0; i<i32CamCount; i++)
	{
		/* 설정된 카메라 시리얼 번호와 동일한 값이 검색되면, 핸들 값 저장 */
		if (UINT32(atoi(pstCamList->uci[i].SerNo)) == m_pstConfig->set_ids.serial_no)
		{
			/* Camera ID 설정 */
			hCam	= HIDS(pstCamList->uci[i].dwDeviceID | IS_USE_DEVICE_ID);	/* or dwDeviceID 둘중에 하나가 맞을 듯 싶다 */
			break;	/* 검색 성공인지 확인 */
		}
	}

	/* 메모리 해제 */
	if (pstCamList)	::Free(pstCamList);

	return hCam;
}

/*
 desc : 카메라 관련 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamMain::InitACam()
{
	INT32 i32Ret	= 0;

	/* 연결된 카메라의 센서 정보 얻기 */
	memset(&m_stSensor, 0x00, sizeof(SENSORINFO));
	memset(&m_stCamera, 0x00, sizeof(CAMINFO));

	if (IS_SUCCESS == i32Ret)	i32Ret	= is_GetSensorInfo(m_hCam, &m_stSensor);
	/* 카메라 정보 얻기 */
	if (IS_SUCCESS == i32Ret)	i32Ret	= is_GetCameraInfo(m_hCam, &m_stCamera);
	/* AOI 영역 크기 얻기 */
	if (IS_SUCCESS == i32Ret)	i32Ret	= GetAOISize();
	/* Parameter 설정 */
	if (IS_SUCCESS == i32Ret)	i32Ret	= SetParameter();
	/* AOI 영역에 만큼, 카메라 내부에 Grabbed Image를 임시로 저장할 버퍼 할당 */
	if (IS_SUCCESS == i32Ret)	i32Ret	= SetAllocData();
#if 0
	/* 이미 설정된 환경 파일 적재 수행 (카메라 관련 파라미터가 저장된 환경 파일 설정) */
	i32Ret	= is_ParameterSet(m_hCam, IS_PARAMETERSET_CMD_SAVE_FILE, L"f:\\download\\set.ini", NULL);
#endif

	/* 에러 코드 및 문자열 출력 */
	if (i32Ret != IS_SUCCESS)	SetLastError();

	return i32Ret == IS_SUCCESS;
}

/*
 desc : 카메라 관련 해제
 parm : None
 retn : None
*/
VOID CCamMain::CloseACam()
{
	if (m_pACamImgAct)		is_FreeImageMem(m_hCam, (PCHAR)&m_pACamImgAct, m_i32MemActID);
	if (m_pACamImgCnv)		is_FreeImageMem(m_hCam, (PCHAR)&m_pACamImgCnv, m_i32MemCnvID);
	/* Gray Index 메모리 해제 */
	if (m_pHistLevel)		Free(m_pHistLevel);
	if (m_pGrayCount)		Free(m_pGrayCount);

	// 큐에 저장된 Grabbed 이미지 제거
	ResetGrabbedImage();
	/* Grabbed Image Buffer 메모리 해제 */
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
 desc : 카메라 연결
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamMain::AttachDevice()
{
	INT32 i32Ret	= 0, i32UploadTime	= (INT32)m_pstConfig->set_ids.upload_time_limit;

	m_hCam			= 0;
	m_i32BandWidth	= 0;

	/* 기존 연결된거 해제 */
	DetachDevice();
	/* 현재 적용할 카메라 핸들 얻기 */
	m_hCam	= GetCamHandle();
	if (!m_hCam)	return FALSE;

	/* 카메라 연결 수행 */
	i32Ret	= is_InitCamera(&m_hCam, NULL);
	if (IS_NO_SUCCESS == i32Ret)
	{
		SetLastError();
		return FALSE;
	}
	/* 카메라 펌웨어 업로드 여부에 따라 */
	if (IS_STARTER_FW_UPLOAD_NEEDED == i32Ret)
	{
		/* Time for the firmware upload = 25 seconds by default*/
		i32Ret	= is_GetDuration(m_hCam, IS_STARTER_FW_UPLOAD_NEEDED, &i32UploadTime);
		if (IS_NO_SUCCESS == i32Ret)
		{
			SetLastError();
			return FALSE;
		}
		/* 기존 핸들 이용하여 연결된 정보 닫기 */
		is_ExitCamera(m_hCam);
		/* 다시 핸들 ID 할당 */
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
	/* 초기화 과정 진행 */
	if (!InitACam())	return FALSE;

	return TRUE;
}

/*
 desc : 카메라 연결 해제
 parm : None
 retn : None
*/
VOID CCamMain::DetachDevice()
{
	CloseACam();

	/* 기존에 연결된 카메라가 있다면 */
	if (0 != m_hCam/* && IsConnected()*/)
	{
		if (m_hCam)	is_ExitCamera(m_hCam);
		m_hCam				= 0;
		m_i32BandWidth		= 0;
		m_u64LastBandTime	= 0;
	}
}

/*
 desc : 현재 카메라가 연결되어 있는지 여부
 parm : None
 retn : TRUE (연결되어 있다) or FALSE
*/
BOOL CCamMain::IsConnected()
{
	INT32 i32Ret, i32Err;
	PCHAR pzErr	= NULL;

	/* 현재 연결 안되어 있는 상태이면 */
	if (0 == m_hCam)	return FALSE;
	/* 현재 연결되어 있는 카메라 개수 확인 */
	i32Ret	= is_GetError(m_hCam, &i32Err, &pzErr);
	if (i32Ret != IS_SUCCESS)
	{
		/* 현재 발생된 에러 값 저장 */
		SetLastError();
		return FALSE;
	}

	/* 가장 최근에 발생된 대역폭 저장 */
	if (GetTickCount64() - 1000 > m_u64LastBandTime)
	{
		m_u64LastBandTime	= GetTickCount64();
		m_i32BandWidth		= is_GetUsedBandwidth(m_hCam);
//		TRACE(L"badwidth = %d MBytes\n", m_i32BandWidth);
	}

	return m_i32BandWidth > 0;
}

/*
 desc : AOI 영역 얻기
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

	/* 이미지 센서의 해상도 크기 보다 Grabbed Image 크기가 크면, 이미지 해상도 크기로 강제 변경 */
	if (m_stSensor.nMaxWidth < m_pstConfig->GetACamGrabSize(0))	m_pstConfig->SetACamGrabSize(0x00, m_stSensor.nMaxWidth);
	if (m_stSensor.nMaxHeight< m_pstConfig->GetACamGrabSize(1))	m_pstConfig->SetACamGrabSize(0x01, m_stSensor.nMaxHeight);
#if 0
	/* AOI 가능한 영역 얻기 */
	if (IS_SUCCESS == i32Ret)	i32Ret	= is_AOI(m_hCam, IS_AOI_IMAGE_GET_SIZE_MAX,
												 (VOID *)&stSizeAOI, sizeof(stSizeAOI));
	if (i32Ret != IS_SUCCESS)	return i32Ret;
	/* 카메라 AOI 영역보다 환경 파일에 설정된 AOI 값이 크면, 재조정 */
	if (m_u32AoiWidth > UINT32(stSizeAOI.s32Width))		m_u32AoiWidth	= stSizeAOI.s32Width;
	if (m_u32AoiHeight > UINT32(stSizeAOI.s32Height))	m_u32AoiHeight	= stSizeAOI.s32Height;
#endif

	return i32Ret;
}

/*
 desc : 카메라 내부에 임시로 저장될 메모리 영역 할당
 parm : None
 반환 IDS Error Value
*/
INT32 CCamMain::SetAllocData()
{
	INT32 i, i32Ret, i32ColorMode, i32Pitch;
	IS_SIZE_2D stSize2D		= {NULL};

	/* 컬러 모드에 따라 */
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
	/* 카메라 관련 파라미터 설정 - 기본 설정 */
	i32Ret	= is_SetColorMode(m_hCam, IS_CM_MONO8);
	/* 메모리 할당 */
	if (IS_SUCCESS == i32Ret)	i32Ret	= is_AllocImageMem(m_hCam,
														   m_pstConfig->GetACamGrabSize(0),
														   m_pstConfig->GetACamGrabSize(1),
														   m_i32BitPerPixel, (PCHAR*)&m_pACamImgAct,
														   &m_i32MemActID);
	/* 메모리 연결 (Active) */
	if (IS_SUCCESS == i32Ret)	i32Ret	= is_SetImageMem(m_hCam, (PCHAR)m_pACamImgAct, m_i32MemActID);
	/* 현재 카메라에 할당된 이미지의 크기 얻기 1 line에 몇 Bytes * Height */
	if (IS_SUCCESS == i32Ret)	i32Ret	= is_GetImageMemPitch(m_hCam, &i32Pitch);
	/* Image Color Mode */
	if (IS_SUCCESS == i32Ret)	i32Ret = is_SetDisplayMode(m_hCam, IS_SET_DM_DIB);

	/* Grabbed Image Size 즉, Camera 내부에 임시로 저장되는 Grabbed Data Size */
	if (i32Ret != IS_SUCCESS)	return i32Ret;
	m_u32GrabBytes	= UINT32(i32Pitch) * m_pstConfig->GetACamGrabSize(1);	/* unit : bytes */
	/* Gray Index 개수 저장할 버퍼 할당 */
	m_pGrayCount	= (PUINT32)Alloc(256 * sizeof(UINT32));
	ASSERT(m_pGrayCount);
	/* Gray Level 개수 저장할 버퍼 할당 */
	m_pHistLevel	= (PUINT64)Alloc(sizeof(UINT64) * m_pstConfig->GetACamGrabSize(0));
	ASSERT(m_pHistLevel);

	// Live Mode 임시 저장용
	memset(&m_stGrab, 0x00, sizeof(STG_ACGR));
	/* 카메라 별로 grab 되는 이미지가 저장될 전체 버퍼 메모리 할당 */
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
 desc : Pixel Clock / Frame Rate / Expose Time 변경
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

	/* AOI Setup을 카메라의 중심으로 AOI 영역 설정 */
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

	/* Binning 처리 */
	if (i32Ret == IS_SUCCESS && m_pstConfig->set_ids.use_binning)
	{
		i32Ret	= is_SetBinning(m_hCam, IS_BINNING_2X_VERTICAL|IS_BINNING_2X_HORIZONTAL);
	}
	/* Scaler 처리 */
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

	/* 현재 카메라의 Pixel Clock 기본 값 얻고 설정 */
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
#if 1	/* 세부적으로 설정하는 방법을 모르겠다. 일단 주석 처리. 내부적으로 자동 설정하도록 */
	/* Frame Rate 설정 */
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
	/* Global Shutter 설정 */
	i32New	= IS_DEVICE_FEATURE_CAP_SHUTTER_MODE_GLOBAL;
	if (i32Ret == IS_SUCCESS)	i32Ret	= is_DeviceFeature(m_hCam, IS_DEVICE_FEATURE_CMD_SET_SHUTTER_MODE, (VOID*)&i32New, sizeof(i32New));

	/* 현재 카메라의 Expose Time Range (Min ~ Max) 얻기 */
	if (i32Ret == IS_SUCCESS)	i32Ret	= is_Exposure(m_hCam, IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MIN, &dbMin, sizeof(DOUBLE));
	if (i32Ret == IS_SUCCESS)	i32Ret	= is_Exposure(m_hCam, IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MAX, &dbMax, sizeof(DOUBLE));
	/* 설정하고자 하는 값이 Min ~ Max 값 사이에 존재하지 않으면, 중간 값으로 설정 */
	if (i32Ret == IS_SUCCESS)
	{
		/* dbMin ~ dbMax 사이에서, Percentage 값만큼 값 설정 */
		dbSet	= dbMin + (dbMax - dbMin) * (m_pstConfig->set_ids.expose_time_rate / 100.0f);
		/* Expose Time (unit : ms) 값 설정 */
		i32Ret	= is_Exposure(m_hCam, IS_EXPOSURE_CMD_SET_EXPOSURE, &dbSet, sizeof(DOUBLE));
	}

	/* Mirror 상/하 or 좌/우 반전 여부 */
	if (m_pstConfig->set_ids.mirror_updown && i32Ret == IS_SUCCESS)	i32Ret = is_SetRopEffect(m_hCam, IS_SET_ROP_MIRROR_UPDOWN, 1, 0);
	if (m_pstConfig->set_ids.mirror_leftright && i32Ret == IS_SUCCESS)	i32Ret = is_SetRopEffect(m_hCam, IS_SET_ROP_MIRROR_LEFTRIGHT, 1, 0);

	/* Edge Enhancement 여부 */
	if (m_pstConfig->set_ids.edge_act_rate && i32Ret == IS_SUCCESS)
	{
		/* Edge Range 얻기 */
		i32Ret = is_EdgeEnhancement(m_hCam, IS_EDGE_ENHANCEMENT_CMD_GET_RANGE,
									(VOID*)u32Range, sizeof(u32Range));
		if (i32Ret == IS_SUCCESS)
		{
			u32New	= u32Range[0] +
					  (UINT32)ROUNDED((u32Range[1] - u32Range[0]) * (m_pstConfig->set_ids.edge_act_rate / 100.0f), 0);
			i32Ret = is_EdgeEnhancement(m_hCam, IS_EDGE_ENHANCEMENT_CMD_SET, (VOID*)&u32New, sizeof(u32New));
		}
	}

	/* Black Level (이미지에 검정색 Level 강제 설정) */
	if (m_pstConfig->set_ids.use_black_level && i32Ret == IS_SUCCESS)
	{
		/* Edge Range 얻기 */
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
	/* 카메라 영상의 불량 화소에 대한 HotPixel 사용 여부 */
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

#if 0	/* Live View에서는 먹히지 않고, 프로그램 종료도 안됨. 아마, Single Capture 진행할 때, 적용되는 듯 */
	/* 카메라 셔터 모드 설정 */
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
 desc : Live Grabbed Image 처리
 parm : by_grab	- [in]  현재 Bayer Image Format으로된 Grabbed Imaged Data
 retn : None
*/
VOID CCamMain::SetLiveGrabbedImage(PUINT8 by_grab)
{
	// 새로 Grabbed된 이미지 저장 (등록) 처리
	m_stGrab.cam_id			= 0x01;
	m_stGrab.img_id			= 0;
	m_stGrab.grab_size		= m_u32GrabBytes;
	m_stGrab.grab_width		= m_pstConfig->GetACamGrabSize(0);
	m_stGrab.grab_height	= m_pstConfig->GetACamGrabSize(1);
	m_stGrab.grab_data		= by_grab;
}

/*
 desc : File Grabbed Image 처리
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
	/* 메모리 임시 할당  */
	if (m_stFileGrab.grab_data)	::Free(m_stFileGrab.grab_data);
	m_stFileGrab.grab_data		= (PUINT8)::Alloc(size+1);
	m_stFileGrab.grab_data[size]= 0x00;
	memcpy(m_stFileGrab.grab_data, grab, size);
}
#endif
/*
 desc : Grabbed Image 획득 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamMain::GetSingleGrabbedImage()
{
	INT32 i32Ret	= 0;
	BOOL bGrabbed	= FALSE;
	UINT64 u64Tick	= GetTickCount64();

	/* Single Image Grabbed 처리*/
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
#if 0	/* 디버깅 용으로 사용해야만 함 */
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
	/* 일정 시간 동안 Grabbed Image 조회 */
	do {

		i32Ret	= is_IsVideoFinish(m_hCam, &bGrabbed);
		if (i32Ret == IS_SUCCESS)	break;
		/* 장시간 대기 모드일 경우 */
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
 desc : 카메라 내부의 이미지 버퍼에 Single Grabbed Image 저장
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamMain::RunGrabbedImage()
{
#if (0 == USE_EQUIPMENT_SIMULATION_MODE)
	INT32 i32Ret	= 0;
	PVOID pGrabMem	= NULL;
	if (!m_hCam)	return FALSE;
	/* Single Image Grabbed 처리*/
	if (!GetSingleGrabbedImage())	return FALSE;
	/* 가장 최근의 Grabbed Image 메모리 위치 얻기 */
	i32Ret	= is_GetImageMem(m_hCam, &pGrabMem);
	if (IS_NO_SUCCESS == i32Ret)
	{
		SetLastError();
		return FALSE;
	}
#endif
	/* 동작 방식에 따라 */
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
			/* Grabbed이 정상적으로 수행되었는지 여부 및 현재 Grabbed 되어 저장된 이미지의 개수 초과 여부 */
			if (m_lstMark.GetCount() <= m_pstConfig->mark_find.max_find_count)
			{
				LPG_ACGR pstMark	= NULL;
				pstMark	= (LPG_ACGR)::Alloc(sizeof(STG_ACGR));
				ASSERT(pstMark);
				/* 새로 Grabbed된 이미지 저장 (등록) 처리 */
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
				/* 다음 마크 검색하기 위해 증가 처리*/
				m_u8GrabIndex++;
			}
			// 동기화 해제
			m_syncGrab.Leave();
		}
		break;
	}
	return TRUE;
}

/*
 desc : Open된 카메라의 Event 설정
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamMain::SetParamUpdate()
{
	/* 기존 할당된 메모리 및 기타 정보 해제 */
	CloseACam();
	/* 새로운 정보를 통해 메모리 할당 */
	return InitACam();
}

/*
 desc : 큐에 저장된 데이터 리셋
 parm : None
 retn : None
*/
VOID CCamMain::ResetGrabbedImage()
{
	POSITION pPos	= NULL;
	LPG_ACGR pstGrab= NULL;

	// 동기화 진입
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
		// 동기화 해제
		m_syncGrab.Leave();
	}
}

/*
 desc : Grabbed 이미지 반환
 parm : grab	- [out] Grabbed Image의 정보 반환
 retn : 구조체 포인터
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
		// 동기화 진입
		if (m_syncGrab.Enter())
		{
			// 현재 저장되어 있는 Grabbed Image가 있는지 확인
			if (m_lstMark.GetCount())
			{
				// 맨 처음에 등록되어 있는 항목 반환
				pstGrab	= m_lstMark.GetHead();
				// 맨 처음에 등록되어 있는 항목 제거
				m_lstMark.RemoveHead();
			}
			// 동기화 해제
			m_syncGrab.Leave();
		}
		break;
	}
	return pstGrab;
}

/*
 desc : Live Grabbed Mode 설정 유무
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

#if 0	/* 굳이 제거할 필요 없다. 노광 시작할 때, 제거한다. */
	ResetGrabbedImage();	/* 기존 Grabbed된 이미지 모두 지우기 */
#endif
}

/*
 desc : 기존 Grab Grabbed Image 초기화
 parm : None
 retn : None
*/
VOID CCamMain::ResetGrabbedMark()
{
	/* 기본 값만 초기화 */
	m_stGrab.cam_id			= 0;
	m_stGrab.grab_size		= 0;
	m_stGrab.grab_width		= 0;
	m_stGrab.grab_height	= 0;
	m_u32WhiteCount			= 0;
}

/*
 desc : 현재 Grabbed Image에 대한 각 Bytes 별로 저장된 Gray 값 비중 계산
 parm : mode	- [in]  Grabbed Image를 캡처하는 방식 (모드)
 retn : TRUE or FALSE
*/
BOOL CCamMain::SetGrabGrayRate(ENG_VCCM mode)
{
	UINT32 i	= 0;

	/* Gray Index 메모리 초기화 */
	memset(m_pGrayCount, 0x00, 256 * sizeof(UINT32));

	/* 현재 Grabbed Image의 Grab Mode와 일치하지 않는지 여부 */
	if (m_enCamMode != mode)	return FALSE;
	/* Grabbed Image가 있는지 여부 */
	if (m_stGrab.grab_size < 1)	return FALSE;
	/* 이미지 크기 즉, 바이트 수 만큼 루프 반복하면서 동일 값들 합산 처리 */
	m_u32WhiteCount	= 0;
	for (; i<m_stGrab.grab_size; i++)
	{
		m_pGrayCount[m_stGrab.grab_data[i]]++;	/* 각 Gray Index 별로 개수 증가 처리 */
		if (m_stGrab.grab_data[i])	m_u32WhiteCount++;
	}

	return TRUE;
}

/*
 desc : 현재 Grabbed Image에 대한 각 Gray Level 값 (비율) 반환
 parm : index	- [in]  0 ~ 255에 해당되는 Gray Level Index
 retn : 0 ~ 255 들이 Grabbed Image에서 비율 값 반환
*/
DOUBLE CCamMain::GetGrabGrayRate(UINT8 index)
{
	/* Grabbed Image가 있는지 여부 */
	if (m_stGrab.grab_size < 1)	return 0.0f;

	return (m_pGrayCount[index] / DOUBLE(m_stGrab.grab_size)) * 100.0f;
}

/*
 desc : 전체 Grabbed Image에서 색깔이 있는 값의 비율 반환
 parm : None
 retn : White 비율 값 반환
*/
DOUBLE CCamMain::GetGrabGrayRateTotal()
{
	if (m_stGrab.grab_size < 1)	return 0.0f;
	return (m_u32WhiteCount / DOUBLE(m_stGrab.grab_size)) * 100.0f;
}

/*
 desc : 현재 Grabbed Image에 대한 각 Bytes 별로 저장된 Gray 값 저장
		넓이에 대해 높이에 있는 모든 Gray 값이 합산하여 저장
 parm : mode	- [in]  Grabbed Image를 캡처하는 방식 (모드)
 retn : TRUE or FALSE
*/
BOOL CCamMain::SetGrabHistLevel(ENG_VCCM mode)
{
	UINT32 i, j;

	/* Gray Index 메모리 초기화 */
	memset(m_pGrayCount, 0x00, 256 * sizeof(UINT32));

	/* 현재 Grabbed Image의 Grab Mode와 일치하지 않는지 여부 */
	if (m_enCamMode != mode)	return FALSE;
	/* Grabbed Image가 있는지 여부 */
	if (m_stGrab.grab_size < 1)	return FALSE;

	/* 기존 설정된 메모리 메모리 초기화 */
	memset(m_pHistLevel, 0x00, sizeof(UINT64) * m_stGrab.grab_width);
	m_u64HistTotal	= 0;
	m_u64HistMax	= 0;
	/* 이미지 크기 즉, 바이트 수 만큼 루프 반복하면서 동일 값들 합산 처리 */
	for (i=0; i<m_stGrab.grab_size; i++)
	{
		/* 해당 I 번째 위치의 Array에 현재 픽셀의 Gray 값 합산 처리 */
		j	= i % m_stGrab.grab_width;
		m_pHistLevel[j]	+= m_stGrab.grab_data[i];	/* 각 Gray 값을 합산 처리 */
		m_u64HistTotal	+= m_stGrab.grab_data[i];	/* Grabbed Image의 전체 Gray Index 총 합 값 */
		/* 각 Column 마다 높이 (Row) 기준으로 저장되어 있는 Gray Level 값들 중 가장 큰 값 저장 */
		if (m_pHistLevel[j] > m_u64HistMax)	m_u64HistMax = m_pHistLevel[j];
	}

	return TRUE;
}

/*
 desc : 현재 Grabbed Image에 대한 각 Bytes 별로 저장된 Gray Level 반환
 parm : None
 retn : 현재 Grabbed Image의 넓이 (Column)의 1 픽셀마다 높이의 Level 값들이 저장되어 있는 버퍼 반환
*/
PUINT64 CCamMain::GetGrabHistLevel()
{
#if 0
	if (m_stGrab.grab_size < 1)	return NULL;
#endif
	return m_pHistLevel;
}

/*
 desc : 에러 코드에 대한 설명 (문자열) 저장
 parm : None
 retn : None
*/
VOID CCamMain::SetLastError()
{
	INT32 i32LastErr	= 0, i32Ret;
	CHAR *pszLastErr	= NULL;
	CUniToChar csCnv;

	/* 가장 최근에 발생된 카메라의 에러 메시지 */
	i32Ret	= is_GetError(m_hCam, &i32LastErr, &pszLastErr);
	/* 이전에 발생된 에러와 동일한지 여부에 따라 */
	if (i32LastErr == 0 || i32LastErr == m_i32LastError)	return;
	m_i32LastError	= i32LastErr;

	/* 최근에 발생된 에러 메시지 제거 */
	wmemset(m_tzErrMsg, 0x00, 1024);
	swprintf_s(m_tzErrMsg, 1024, L"Failed to set the IDS Camera : [%d] %s", i32LastErr, csCnv.Ansi2Uni(pszLastErr));
#ifdef _DEBUG
	AfxMessageBox(m_tzErrMsg, MB_ICONSTOP|MB_TOPMOST);
#else
	LOG_SAVED(ENG_LNWE::en_warning, m_tzErrMsg);
#endif
}

/*
 desc : 가장 최근에 저장된 Live Image를 파일로 저장
 parm : file	- [in]  저장될 파일 이름 (전체 경로 포함)
 retn : TRUE or FALSE
*/
BOOL CCamMain::SaveLiveImageToFile(TCHAR *file)
{
	INT32 i32Ret	= IS_SUCCESS;
	PVOID pGrabMem	= NULL;
	IMAGE_FILE_PARAMS stImgParam	= {NULL};

	/* 가장 최근의 Grabbed Image 메모리 위치 얻기 */
	i32Ret	= is_GetImageMem(m_hCam, &pGrabMem);
	if (i32Ret != IS_SUCCESS)
	{
		SetLastError();
		return FALSE;
	}

	/* 파라미터에 기본 값 설정 */
	stImgParam.nFileType	= IS_IMG_BMP;
	stImgParam.pwchFileName	= file;
	stImgParam.ppcImageMem	= (PCHAR*)&pGrabMem;
	/* 파일로 저장 */
	i32Ret	= is_ImageFile(m_hCam, IS_IMAGE_FILE_CMD_SAVE, (VOID *)&stImgParam, sizeof(stImgParam));
	if (i32Ret != IS_SUCCESS)
	{
		SetLastError();
		return FALSE;
	}

	return TRUE;
}

/*
 desc : 가장 최근에 저장된 Live Image에서 Drag 영역 이미지 정보 얻기
 parm : drag	- [in]  Live Image 영역에 Drag 영역을 수행한 좌표 정보 저장 (단위: 픽셀)
 retn : drag 이미지 정보를 저장하고 있는 버퍼 포인터
*/
PUINT8 CCamMain::GetLastLiveDragImage(RECT drag)
{
	UINT32 i, u32BuffSize, u32LiveMem, u32DragLineSize = drag.right - drag.left;
	PUINT8 pLiveBuff	= m_stGrab.grab_data;	/* 가장 최근의 Live Image가 저장된 버퍼 포인터 연결 */
	PUINT8 pDragBuff	= NULL;

	if (!pLiveBuff)	return NULL;

	/* Drag 이미지 버퍼 크기 */
	u32BuffSize	= (drag.right - drag.left) * (drag.bottom - drag.top);	/* bytes */
	/* 기존 Drag 이미지를 저장하고 있는 버퍼가 할당 되어 있다면 해제 */
	if (m_pDragBuff)	::Free(m_pDragBuff);
	/* Drag Image Buffer 할당 */
	m_pDragBuff	= (PUINT8)::Alloc(u32BuffSize+1);
	m_pDragBuff[u32BuffSize]	= 0x00;
	pDragBuff	= m_pDragBuff;

	/* Drag 영역만 복사 진행 */
	for (i=UINT32(drag.top); i<UINT32(drag.bottom); i++)
	{
		/* Live 원본 버퍼 시작 위치 */
		u32LiveMem	= i * m_pstConfig->GetACamGrabSize(0) + drag.left;
		/* Drag 영역에 이미지 복사 */
		memcpy(pDragBuff, pLiveBuff + u32LiveMem, u32DragLineSize);
		/* 다음에 저장될 Drag 영역 위치로 이동 */
		pDragBuff	+= u32DragLineSize;
	}

	return m_pDragBuff;
}