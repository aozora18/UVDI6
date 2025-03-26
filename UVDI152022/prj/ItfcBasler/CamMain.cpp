
/*
 desc : Basler Camera SDK Interface
*/

#include "pch.h"
#include "MainApp.h"
#include "CamMain.h"

#include "CamInst.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 desc : 생성자
 parm : config	- [in]  환경 정보
		cam_id	- [in]  Align Camera Index (1 or Later)
 retn : None
*/
CCamMain::CCamMain(LPG_CIEA config, UINT8 cam_id)
	: m_csTransportLayerFactory(CTlFactory::GetInstance())
{
	UINT8 i;
	UINT32 u32IPv4;

	m_pstConfig			= config;
	m_u32WhiteCount		= 0;
	m_enCamMode			= ENG_VCCM::en_none;	// Grabbed Mode
	m_u8CamID			= cam_id;
	m_uDispType			= 99;
	u32IPv4				= uvCmn_GetIPv4BytesToUint32(config->set_basler.cam_ip[cam_id-1]);
	uvCmn_GetIPv4Uint32ToStr(u32IPv4, m_tzCamIPv4, 16);
	wmemset(m_tzErrMsg, 0x00, 1024);

	m_u8GrabIndex		= 0;
	/* 실행 모드에 따라 */
	if (ENG_ERVM::en_cmps_sw == ENG_ERVM(m_pstConfig->set_comn.engine_mode))
		m_u32ExposeTime	= m_pstConfig->set_basler.grab_times[cam_id-1];
	else
		m_u32ExposeTime	= m_pstConfig->set_basler.step_times[cam_id-1];

	/* Gray Index 개수 저장할 버퍼 할당 */
	m_pGrayCount = new UINT32[256];// (PUINT32)Alloc(256 * sizeof(UINT32));
	ASSERT(m_pGrayCount);
	/* Gray Level 개수 저장할 버퍼 할당 */
	m_pHistLevel = new UINT64[sizeof(UINT64) * config->GetACamGrabSize(0x00)];// (PUINT64)Alloc(sizeof(UINT64) * config->GetACamGrabSize(0x00));
	ASSERT(m_pHistLevel);

	// 연결될 카메라의 인스턴스 메모리 할당
	m_pCamera			= new CCamInst(cam_id);
	ASSERT(m_pCamera);

	// Live Mode 임시 저장용
	//memset(&m_stGrab, 0x00, sizeof(STG_ACGR));
	m_stGrab.Init();
	/* Basler Camera에서 Grabbed Image의 Format은 256 Grayscale 이므로*/
	m_stGrab.grab_data = new UINT8[config->GetACamGrabSize(0x00) * config->GetACamGrabSize(0x01) + 1];// (PUINT8)Alloc(config->GetACamGrabSize(0x00) * config->GetACamGrabSize(0x01) + 1);
	m_stGrab.grab_data[config->GetACamGrabSize(0x00) * config->GetACamGrabSize(0x01)]	= 0x00;

	/* 1 번 Scan으로 grab 되는 이미지가 저장될 버퍼 메모리 할당 */
	m_pGrabBuff = new PUINT8[m_pstConfig->mark_find.max_mark_grab];
	m_pGrabBuff[0] = new UINT8[
		UINT64(m_pstConfig->mark_find.max_mark_grab) *
			UINT64(config->GetACamGrabSize(0x00)) *
			UINT64(config->GetACamGrabSize(0x01)) +
			m_pstConfig->mark_find.max_mark_grab];

	for (i=1; i<m_pstConfig->mark_find.max_mark_grab; i++)
	{
		m_pGrabBuff[i]	= m_pGrabBuff[i-1] +
						  (UINT64(config->GetACamGrabSize(0x00)) * UINT64(config->GetACamGrabSize(0x01)) + 1);
		m_pGrabBuff[i][UINT64(config->GetACamGrabSize(0x00)) * UINT64(config->GetACamGrabSize(0x01))] = 0;
	}
	markPool.Initialize();
	
	//pstMark = new STG_ACGR; //(LPG_ACGR)::Alloc(sizeof(STG_ACGR));
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CCamMain::~CCamMain()
{
	DetachDevice();
	// 반드시 아래 메모리 해제
	if (m_pCamera)
	{
		delete m_pCamera;
		m_pCamera	= NULL;
	}

	m_stGrab.Release();
	// Live Mode 임시 저장용 메모리 해제
	//Free(m_stGrab.grab_data);
	/* Gray Index 메모리 해제 */
	if (m_pHistLevel)	delete m_pHistLevel;
	if (m_pGrayCount)	delete m_pGrayCount;

	// 큐에 저장된 Grabbed 이미지 제거
	ResetGrabbedImage();
	/* Grabbed Image Buffer 메모리 해제 */
	if (m_pGrabBuff) 
	{
		delete[] m_pGrabBuff[0];
		delete[] m_pGrabBuff;
		m_pGrabBuff = nullptr;
	}

	//delete pstMark;
	//::Free(pstMark);
	markPool.Destroy();
}

/*
 desc : 카메라 연결
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamMain::AttachDevice()
{
	
	UINT32 i	= 0;
	String_t strCamIPv4;
	CUniToChar csCnv;
	DeviceInfoList_t stDevices;
	IPylonDevice *pDevice;

	if (0 == m_csTransportLayerFactory.EnumerateDevices(stDevices))
	{
		if (!GetConfig()->IsRunDemo())
			LOG_ERROR(ENG_EDIC::en_basler, L"Failed to found the devices of camera");
		return FALSE;
	}
#if 0
	// 검색된 카메라의 개수보다 크면 안됨
	if (stDevices.size() < 1)
	{
		LOG_ERROR(ENG_EDIC::en_basler, L"Can't find the align camera");
		return FALSE;
	}
#endif
	try
	{
		// Attach device to Pylon's camera array
		for (pDevice=NULL; !pDevice && i<stDevices.size(); i++)
		{
			pDevice = m_csTransportLayerFactory.CreateDevice(stDevices[i]);
			if (pDevice)
			{
				// 연결하고자 하는 카메라의 IP와 동일한지 여부
				strCamIPv4	= pDevice->GetDeviceInfo().GetFullName();
				if (strCamIPv4.find(csCnv.Uni2Ansi(m_tzCamIPv4)) == std::string::npos)	/* 동일 하지 않다면 */
				{
					m_csTransportLayerFactory.DestroyDevice(pDevice);
					pDevice	= NULL;
				}
			}
		}
		/* 카메라가 있는지 확인 */
		if (!pDevice)
		{
			LOG_ERROR(ENG_EDIC::en_basler, L"Can't find the align camera");
			return FALSE;
		}
		// 카메라 연결
		m_pCamera->Attach(pDevice);
		// 카메라 Open 수행
		m_pCamera->Open();
		// Register CameraBasler's Event Handler
		m_pCamera->RegisterImageEventHandler(this, RegistrationMode_Append, Cleanup_None);
		m_pCamera->RegisterConfiguration(this, RegistrationMode_ReplaceAll, Cleanup_None);
		// 카메라 설정
		if (!SetParamUpdate())	return FALSE;
		// Grab Event 시작
#if 0
		m_pCamera->StartGrabbing(GrabStrategy_OneByOne, GrabLoop_ProvidedByInstantCamera);
#else
		m_pCamera->StartGrabbing(GrabStrategy_UpcomingImage, GrabLoop_ProvidedByInstantCamera);
#endif
	}
	catch (const GenericException &e)
	{
		swprintf_s(m_tzErrMsg, 1024, L"%hs", e.GetDescription());
		LOG_ERROR(ENG_EDIC::en_basler, m_tzErrMsg);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : 카메라 연결 해제
 parm : None
 retn : None
*/
VOID CCamMain::DetachDevice()
{
#if 0
	if (m_pCamera && m_pCamera->IsGrabbing())
#else
	if (m_pCamera && m_pCamera->IsPylonDeviceAttached())
#endif
	{
		m_pCamera->StopGrabbing();
#if 1
		m_pCamera->DeregisterConfiguration(this);
		m_pCamera->DeregisterImageEventHandler(this);
#endif
		if (m_pCamera->IsOpen())	m_pCamera->Close();
		m_pCamera->DetachDevice();
		m_pCamera->DestroyDevice();
	}
}

/*
 desc : 현재 카메라가 연결되어 있는지 여부
 parm : None
 retn : TRUE (연결되어 있다) or FALSE
*/
BOOL CCamMain::IsConnected()
{
	// 만약 제거되었다면 연결 안되어 있는걸로 설정
	if (m_pCamera->IsCameraDeviceRemoved())	return FALSE;

	return (m_pCamera->IsOpen() && m_pCamera->IsPylonDeviceAttached()) ? TRUE : FALSE;
}

/*
 desc : 가장 최근에 발생된 에러 값 반환
 parm : None
 retn : 에러 값
*/
UINT16 CCamMain::GetLastError()
{
	/* enum LastErrorEnums */
	/*return UINT8(m_pCamera->LastError.);*/
	return 0;
}

/*
 desc : Grab 시작 처리
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamMain::StartGrab()
{
	try
	{
		m_pCamera->StartGrabbing(GrabStrategy_OneByOne, GrabLoop_ProvidedByInstantCamera);
	}
	catch (const GenericException &e)
	{
		swprintf_s(m_tzErrMsg, 1024, L"Failed to start the grabbing = %hs", e.GetDescription());
		LOG_ERROR(ENG_EDIC::en_basler, m_tzErrMsg);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : Grab 종료 처리
 parm : None
 retn : None
*/
VOID CCamMain::StopGrab()
{
	try
	{
		m_pCamera->StopGrabbing();
		ResetGrabbedImage();
	}
	catch (const GenericException &e)
	{
		swprintf_s(m_tzErrMsg, 1024, L"Failed to stop the grab = %hs", e.GetDescription());
		LOG_ERROR(ENG_EDIC::en_basler, m_tzErrMsg);
	}
}

bool CCamMain::GetSwTrigGrabImage()
{
	try
	{
		if (GetTriggerMode() != ENG_TRGM::en_Sw_mode)
			throw exception("not a en_sw_mode");
			
			m_pCamera->TriggerSoftware.Execute();

			return true;
	}
	catch (exception e)
	{
		m_pCamera->StopGrabbing();
		swprintf_s(m_tzErrMsg, 1024, L"Failed to start the grabbing = %hs", e.what());
		LOG_ERROR(ENG_EDIC::en_basler, m_tzErrMsg);
		return false;
	}
	catch (const GenericException& e)
	{
		m_pCamera->StopGrabbing();
		swprintf_s(m_tzErrMsg, 1024, L"Failed to start the grabbing = %hs", e.GetDescription());
		LOG_ERROR(ENG_EDIC::en_basler, m_tzErrMsg);
		return false;
	}
}


ENG_TRGM CCamMain::GetTriggerMode()
{
	return triggerMode;
}

bool CCamMain::ChangeTriggerMode(ENG_TRGM mode)
{
	try
	{
		//트리거 관련  (트리거모드 )
		m_pCamera->TriggerMode.SetValue(TriggerMode_Off);

		switch (mode)
		{
		case ENG_TRGM::en_line_mode:
		{
			m_pCamera->TriggerSelector.SetValue(TriggerSelector_FrameStart);
			m_pCamera->TriggerSource.SetValue(TriggerSource_Line1);
			m_pCamera->TriggerActivation.SetValue(TriggerActivation_RisingEdge);
			m_pCamera->TriggerDelayAbs.SetValue(0.0f);
		}
		break;

		case ENG_TRGM::en_Sw_mode:
		{
			m_pCamera->TriggerSource.SetValue(TriggerSource_Software);
		}
		break;
		}
		triggerMode = mode;
		m_pCamera->TriggerMode.SetValue(TriggerMode_On);
		return true;
	}
	catch (...)
	{
		return false;
	}
	
}

/*
 desc : Open된 카메라의 Event 설정
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamMain::SetParamUpdate()
{
	BOOL bIsGrab	= FALSE;

	// 현재 Grabbing 중인 경우라면, Grab을 중지하고 값 설정
	bIsGrab	= m_pCamera->IsGrabbing();
	if (bIsGrab)	StopGrab();

	try
	{
		/* -------------------------- */
		/* Setup the Camera Parameter */
		/* -------------------------- */
		m_pCamera->GainAuto.SetValue(GainAuto_Off);
		m_pCamera->GainSelector.SetValue(GainSelector_All);
		m_pCamera->GainRaw.SetValue(m_pstConfig->set_basler.cam_gain_level[m_u8CamID-1]);
		// Image Format Controls
		m_pCamera->ReverseX.SetValue(m_pstConfig->set_basler.cam_reverse_x);
		m_pCamera->ReverseY.SetValue(m_pstConfig->set_basler.cam_reverse_y);
		// AOI Controls
		m_pCamera->Width.SetValue(m_pstConfig->GetACamGrabSize(0x00));
		m_pCamera->Height.SetValue(m_pstConfig->GetACamGrabSize(0x01));
		m_pCamera->CenterX.SetValue(true);
		m_pCamera->CenterY.SetValue(true);
		m_pCamera->AcquisitionFrameCount.SetValue(1);
		
		ChangeTriggerMode(ENG_TRGM::en_line_mode);

		bool testview = false;
		m_pCamera->ExposureMode.SetValue(testview == true ? ExposureMode_Off : ExposureMode_Timed);
		m_pCamera->ExposureAuto.SetValue(testview == true ? ExposureAuto_Once : ExposureAuto_Off);

		m_pCamera->AcquisitionFrameRateEnable.SetValue(false);
		m_pCamera->AcquisitionStatusSelector.SetValue(AcquisitionStatusSelector_FrameTriggerWait);
		m_pCamera->SyncFreeRunTimerEnable.SetValue(false);

		m_pCamera->ExposureTimeAbs.SetValue(m_u32ExposeTime);

		/* Transport layer */
//		m_pCamera->GevSCPSPacketSize.SetValue(8192);	/* Fixed */
	}
	catch (const GenericException &e)
	{
		swprintf_s(m_tzErrMsg, 1024, L"Failed to attach the align camera = %hs", e.GetDescription());
		LOG_ERROR(ENG_EDIC::en_basler, m_tzErrMsg);
#ifdef _DEBUG
		AfxMessageBox(m_tzErrMsg, MB_ICONSTOP|MB_TOPMOST);
#endif
		return FALSE;
	}

	// 가장 최근에 Grabbing 중인라면
	if (bIsGrab)
	{
		if (!StartGrab())	return FALSE;
	}

	return TRUE;
}

/*
 desc : Grab Event 처리
 parm : camera	- [in]  이벤트가 발생된 카메라
		grabbed	- [in]  Grab된 결과가 저장된 참조 변수
 retn : None
*/
VOID CCamMain::OnImageGrabbed(CInstantCamera& camera, const CGrabResultPtr& grabbed)
{
	TCHAR tzMode[7][8]	= { L"None", L"live", L"grab", L"cali", L"edge", L"line", L"hole" };
	TCHAR tzFile[MAX_PATH_LEN] = {NULL};
	SYSTEMTIME stTm		= { NULL };

	/* Get the current time */
	GetLocalTime(&stTm);

	switch (m_enCamMode)
	{
	case ENG_VCCM::en_live_mode	:
		/* 초당 10 번 프레임 정도 캡처하도록 강제 처리 수행 */
#ifdef _DEBUG
		if (GetTickCount64() < (m_u64TickGrab+250))	return;
#else
		if (GetTickCount64() < (m_u64TickGrab+100))	return;
#endif
		m_u64TickGrab	= GetTickCount64();	/* 다음 Grabbed 시간을 위해서 현재 시간 저장 */
	case ENG_VCCM::en_edge_mode	:
	case ENG_VCCM::en_cali_mode	:
	case ENG_VCCM::en_hole_mode	:
		if (m_syncGrab.Enter())
		{
			if (grabbed->GrabSucceeded())
			{
				// 새로 Grabbed된 이미지 저장 (등록) 처리
				m_stGrab.cam_id		= m_u8CamID;
				m_stGrab.img_id		= 0;
				m_stGrab.grab_size	= (UINT32)grabbed->GetImageSize();
				m_stGrab.grab_width	= grabbed->GetWidth();
				m_stGrab.grab_height= grabbed->GetHeight();
 				memcpy(m_stGrab.grab_data, grabbed->GetBuffer(), m_stGrab.grab_size);

				m_stGrab.grab_data[m_stGrab.grab_size]	= 0x00;

				uvMIL_RegistMILImg(m_stGrab.cam_id - 1, m_stGrab.grab_width, m_stGrab.grab_height, m_stGrab.grab_data);

				// lk91 cam 두개 신호 들어올때 문제 생김
				//if(m_uDispType == DISP_TYPE_MARK_LIVE || m_uDispType == DISP_TYPE_MARK)
				//	uvMIL_DrawImageBitmap(DISP_TYPE_MARK_LIVE, m_stGrab.cam_id, &m_stGrab, 0.0, m_stGrab.cam_id - 1);
				//else
				//	uvMIL_DrawImageBitmap(m_uDispType, m_stGrab.cam_id, &m_stGrab, 0.0, m_stGrab.cam_id - 1);


				/* Live 제외하고 파일 저장 */
				if (m_enCamMode != ENG_VCCM::en_live_mode && !m_stGrab.IsEmpty())
				{
					//swprintf_s(tzFile, L"%s\\%s\\cam(%u)_img(0)_%04d%02d%02d_%02d%02d%02d.bmp",
					swprintf_s(tzFile, L"%s\\save_img\\%s\\cam(%u)_img(0)_%04d%02d%02d_%02d%02d%02d.bmp",
							   g_tzWorkDir, tzMode[UINT8(m_enCamMode)], m_u8CamID,
							   stTm.wYear, stTm.wMonth, stTm.wDay, stTm.wHour, stTm.wMinute, stTm.wSecond);
					uvMIL_SaveGrabbedToFile(tzFile, m_stGrab.grab_width, m_stGrab.grab_height, m_stGrab.grab_data);
				}
			}
			// 동기화 해제
			m_syncGrab.Leave();
		}
		break;

	case ENG_VCCM::en_grab_mode	:
		if (m_syncGrab.Enter())
		{
			// Grabbed이 정상적으로 수행되었는지 여부 및 현재 Grabbed 되어 저장된 이미지의 개수 초과 여부
			if (grabbed && grabbed->GrabSucceeded() && m_u8GrabIndex < m_pstConfig->mark_find.max_mark_grab)
			{
				/* 현재 등록된 거버의 마크 개수보다 크면 에러 처리 (더 이상 메모리에 저장하지 않음) */
				/* 반드시 에러 로그 남기도록 처리 */

				//LPG_ACGR pstMark	= NULL;
				//pstMark	= NULL;
				//pstMark	= (LPG_ACGR)::Alloc(sizeof(STG_ACGR));
				//ASSERT(pstMark);
				// 새로 Grabbed된 이미지 저장 (등록) 처리
				pstMark = markPool.GetNext();
				pstMark->cam_id		= m_u8CamID;
				pstMark->img_id		= m_u8GrabIndex++;
				pstMark->grab_size	= (UINT32)grabbed->GetImageSize();
				pstMark->grab_width	= grabbed->GetWidth();
				pstMark->grab_height= grabbed->GetHeight();
				pstMark->grab_data	= m_pGrabBuff[pstMark->img_id];

				memcpy(pstMark->grab_data, grabbed->GetBuffer(), pstMark->grab_size);
				pstMark->grab_data[pstMark->grab_size]	= 0x00;


				m_lstMark.AddTail(pstMark);

				//uvMIL_RegistMILImg((m_u8CamID - 1) * 2 + (m_u8GrabIndex - 1), m_stGrab.grab_width, m_stGrab.grab_height, m_stGrab.grab_data);
				uvMIL_RegistMILGrabImg((m_u8CamID - 1) * 2 + (m_u8GrabIndex - 1), pstMark->grab_width, pstMark->grab_height, pstMark->grab_data);
				uvMIL_RegistMILImg(m_u8CamID - 1, pstMark->grab_width, pstMark->grab_height, pstMark->grab_data);
#ifdef _DEBUG
				TRACE(L"CCamMain::OnImageGrabbed::grab_mode cam_id=%d, img_id = %d\n", m_u8CamID, pstMark->img_id);
#endif
				UINT8 u8Ch	= m_u8CamID == 1 ? 0 : 2;
				/* Grab 파일 저장 */
				swprintf_s(tzFile, L"%s\\save_img\\grab\\cam(%u)_img(%u)_t(%u)_s(%u)_d(%u)_%04d%02d%02d_%02d%02d%02d.bmp",
						   g_tzWorkDir, m_u8CamID, m_u8GrabIndex,
						   m_pstConfig->trig_grab.trig_on_time[u8Ch],
						   g_pstConfig->trig_grab.strob_on_time[u8Ch],
						   g_pstConfig->trig_grab.trig_delay_time[u8Ch],
						   stTm.wYear, stTm.wMonth, stTm.wDay, stTm.wHour, stTm.wMinute, stTm.wSecond);
				uvMIL_SaveGrabbedToFile(tzFile, pstMark->grab_width, pstMark->grab_height, pstMark->grab_data);
				//::Free(pstMark);
			}
			else
			{
				TCHAR tzMesg[128]	= {NULL};
				swprintf_s(tzMesg, 128, L"Exceeded number of captured images [Now:%d / Max:%d]",
						   m_u8GrabIndex+1, m_pstConfig->mark_find.max_mark_grab);
				LOG_ERROR(ENG_EDIC::en_basler, tzMesg);
			}
			// 동기화 해제
			m_syncGrab.Leave();
		}

		break;
	}
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

	/* 동기화 진입 */
	if (m_syncGrab.Enter())
	{
		m_u8GrabIndex	= 0;	
		m_lstMark.RemoveAll();
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
	/* Live Mode */
	case ENG_VCCM::en_live_mode	:
	case ENG_VCCM::en_cali_mode	:
	case ENG_VCCM::en_edge_mode	:
	case ENG_VCCM::en_hole_mode	:
	case ENG_VCCM::en_image_mode:
		pstGrab	= &m_stGrab;
		break;

	/* Grabbed Mode */
	case ENG_VCCM::en_grab_mode	:
		/* 동기화 진입 */
		if (m_syncGrab.Enter())
		{
			/* 현재 저장되어 있는 Grabbed Image가 있는지 확인 */
			if (m_lstMark.GetCount())
			{
				/* 맨 처음에 등록되어 있는 항목 반환 */
				pstGrab	= m_lstMark.GetHead();
				/* 맨 처음에 등록되어 있는 항목 제거 */
				m_lstMark.RemoveHead();
#if 1
				TRACE(L"CCamMain::GetGrabbedImage() = cam_id=%d, img_id=%d, size=%d\n",
					  pstGrab->cam_id, pstGrab->img_id, pstGrab->grab_size);
#endif
			}
			/* 동기화 해제 */
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
	m_stGrab.grab_size		= 0;
	m_stGrab.grab_width		= 0;
	m_stGrab.grab_height	= 0;
	m_enCamMode				= mode;

	/* Live Grabbed Mode인 경우인지 */
	if (m_enCamMode == ENG_VCCM::en_live_mode ||
		m_enCamMode == ENG_VCCM::en_cali_mode)
	{
		m_u64TickGrab	= GetTickCount64();
	}
#if 0	/* 굳이 제거할 필요 없다. 노광 시작할 때, 제거한다. */
	ResetGrabbedImage();	/* 기존 Grabbed된 이미지 모두 지우기 */
#endif
}

/*
 desc : 기존 Grabbed Image의 마크 검색 정보 초기화
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
	if (m_stGrab.grab_size < 1)	return NULL;
	return m_pHistLevel;
}

/*
 desc : 얼라인 카메라의 Gain Level 값 설정
 parm : level	- [in] 0 ~ 255 (값이 클수록 밝게 처리)
 retn : TRUE or FALSE
*/
BOOL CCamMain::SetGainLevel(int level)
{
	BOOL bIsGrab	= FALSE;

	// 현재 Grabbing 중인 경우라면, Grab을 중지하고 값 설정
	bIsGrab	= m_pCamera->IsGrabbing();
	if (bIsGrab)	StopGrab();

	try
	{
		m_pCamera->GainAuto.SetValue(GainAuto_Off);
		m_pCamera->GainSelector.SetValue(GainSelector_All);
		m_pCamera->GainRaw.SetValue(level);
	}
	catch (const GenericException &e)
	{
		swprintf_s(m_tzErrMsg, 1024, L"Failed to set the gain for align camera = %hs",
				   e.GetDescription());
		LOG_ERROR(ENG_EDIC::en_basler, m_tzErrMsg);
#ifdef _DEBUG
		AfxMessageBox(m_tzErrMsg, MB_ICONSTOP|MB_TOPMOST);
#endif
		return FALSE;
	}

	// 가장 최근에 Grabbing 중인라면
	if (bIsGrab)
	{
		if (!StartGrab())	return FALSE;
	}

	return TRUE;
}

/*
 desc : 수동으로 이미지를 불러와서 데이터를 적재 합니다. (데모 모드에서 주로 사용 됨)
 parm : file	- [in]  전체 경로가 포함됨 265 grayscale bitmap 파일
 retn : TRUE or FALSE
*/
BOOL CCamMain::LoadImageFromFile(PTCHAR file, INT32 cam_id)
{
	UINT32 u32FileSize		= 0;	/* 파일 전체 크기 (단위: bytes) */
	PUINT8 pImgBuff			= NULL, pImgNext;

	/* 파일의 전체 크기 */
	u32FileSize	= uvCmn_GetFileSize(file);
	if (u32FileSize < 1)	return FALSE;
	/* 파일 전체를 저장할 버퍼 생성 */
	pImgBuff = new UINT8[u32FileSize + 1];// (PUINT8)::Alloc(u32FileSize + 1);
	ASSERT(pImgBuff);
	pImgBuff[u32FileSize]	= 0x00;
	/* 파일 전체 내용 저장 */
	if (!uvCmn_LoadFile(file, pImgBuff, u32FileSize))
	{
		delete pImgBuff;
		return FALSE;
	}
	pImgNext	= pImgBuff;
	/* 파일 헤더 부분 Skip */
	pImgNext	+= 2/*bfType*/+4/*bfSize*/+4/*bfReserved*/+4/*bfOffBits*/;
	/* 이미지 헤저 Skip */
	pImgNext	+= 4/*biSize*/;
	memcpy(&m_stGrab.grab_width, pImgNext, 4);	pImgNext	+= 4/*biWidth*/;
	memcpy(&m_stGrab.grab_height, pImgNext, 4);	pImgNext	+= 4/*biHeight*/;
	pImgNext	+= 2/*biPlanes*/+2/*biBitCount*/+4/*biCompression*/;
	memcpy(&m_stGrab.grab_size, pImgNext, 4);	pImgNext	+= 4/*biSizeImage*/;
	pImgNext	+= 4/*biXPixelPerMeter*/+4/*biYPixelPerMeter*/+4/*biClrUsed*/+4/*biClrImportant*/;
	pImgNext += (4/*rgbBlue+rgbGreen+rgbRed+rgbReserved*/ * 256 /* 8 bits*/);
	//pImgNext += 256;
	/* If you don't have the data size of the image, you have to calculate it arbitraily */
	if (m_stGrab.grab_size < 1)
	{
		m_stGrab.grab_size	= u32FileSize - UINT32(pImgNext - pImgBuff);
	}
	/* 이미지 데이터 */
	if ((m_pstConfig->GetACamGrabSize(0x00) * m_pstConfig->GetACamGrabSize(0x01)) == m_stGrab.grab_size) { 
		memcpy(m_stGrab.grab_data, pImgNext, m_stGrab.grab_size); 
		m_stGrab.grab_data[m_stGrab.grab_size]	= 0x00; 
		m_stGrab.cam_id = m_u8CamID; 
	}
	else {
		delete pImgBuff;
		return FALSE;
	}

	double angle = 90.0;

	memcpy(pImgBuff, m_stGrab.grab_data, m_stGrab.grab_size+1);

	uvMIL_ImageFlip(pImgBuff, m_stGrab.grab_data, m_stGrab.grab_width, m_stGrab.grab_height); 

	if (cam_id != 0) {
		uvMIL_RegistMILImg(cam_id-1, m_stGrab.grab_width, m_stGrab.grab_height, m_stGrab.grab_data);
	}

	/* 임시 버퍼 해제 */
	delete pImgBuff; 

	return TRUE;
}
