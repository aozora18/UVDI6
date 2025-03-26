
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
 desc : ������
 parm : config	- [in]  ȯ�� ����
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
	/* ���� ��忡 ���� */
	if (ENG_ERVM::en_cmps_sw == ENG_ERVM(m_pstConfig->set_comn.engine_mode))
		m_u32ExposeTime	= m_pstConfig->set_basler.grab_times[cam_id-1];
	else
		m_u32ExposeTime	= m_pstConfig->set_basler.step_times[cam_id-1];

	/* Gray Index ���� ������ ���� �Ҵ� */
	m_pGrayCount = new UINT32[256];// (PUINT32)Alloc(256 * sizeof(UINT32));
	ASSERT(m_pGrayCount);
	/* Gray Level ���� ������ ���� �Ҵ� */
	m_pHistLevel = new UINT64[sizeof(UINT64) * config->GetACamGrabSize(0x00)];// (PUINT64)Alloc(sizeof(UINT64) * config->GetACamGrabSize(0x00));
	ASSERT(m_pHistLevel);

	// ����� ī�޶��� �ν��Ͻ� �޸� �Ҵ�
	m_pCamera			= new CCamInst(cam_id);
	ASSERT(m_pCamera);

	// Live Mode �ӽ� �����
	//memset(&m_stGrab, 0x00, sizeof(STG_ACGR));
	m_stGrab.Init();
	/* Basler Camera���� Grabbed Image�� Format�� 256 Grayscale �̹Ƿ�*/
	m_stGrab.grab_data = new UINT8[config->GetACamGrabSize(0x00) * config->GetACamGrabSize(0x01) + 1];// (PUINT8)Alloc(config->GetACamGrabSize(0x00) * config->GetACamGrabSize(0x01) + 1);
	m_stGrab.grab_data[config->GetACamGrabSize(0x00) * config->GetACamGrabSize(0x01)]	= 0x00;

	/* 1 �� Scan���� grab �Ǵ� �̹����� ����� ���� �޸� �Ҵ� */
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
 desc : �ı���
 parm : None
 retn : None
*/
CCamMain::~CCamMain()
{
	DetachDevice();
	// �ݵ�� �Ʒ� �޸� ����
	if (m_pCamera)
	{
		delete m_pCamera;
		m_pCamera	= NULL;
	}

	m_stGrab.Release();
	// Live Mode �ӽ� ����� �޸� ����
	//Free(m_stGrab.grab_data);
	/* Gray Index �޸� ���� */
	if (m_pHistLevel)	delete m_pHistLevel;
	if (m_pGrayCount)	delete m_pGrayCount;

	// ť�� ����� Grabbed �̹��� ����
	ResetGrabbedImage();
	/* Grabbed Image Buffer �޸� ���� */
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
 desc : ī�޶� ����
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
	// �˻��� ī�޶��� �������� ũ�� �ȵ�
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
				// �����ϰ��� �ϴ� ī�޶��� IP�� �������� ����
				strCamIPv4	= pDevice->GetDeviceInfo().GetFullName();
				if (strCamIPv4.find(csCnv.Uni2Ansi(m_tzCamIPv4)) == std::string::npos)	/* ���� ���� �ʴٸ� */
				{
					m_csTransportLayerFactory.DestroyDevice(pDevice);
					pDevice	= NULL;
				}
			}
		}
		/* ī�޶� �ִ��� Ȯ�� */
		if (!pDevice)
		{
			LOG_ERROR(ENG_EDIC::en_basler, L"Can't find the align camera");
			return FALSE;
		}
		// ī�޶� ����
		m_pCamera->Attach(pDevice);
		// ī�޶� Open ����
		m_pCamera->Open();
		// Register CameraBasler's Event Handler
		m_pCamera->RegisterImageEventHandler(this, RegistrationMode_Append, Cleanup_None);
		m_pCamera->RegisterConfiguration(this, RegistrationMode_ReplaceAll, Cleanup_None);
		// ī�޶� ����
		if (!SetParamUpdate())	return FALSE;
		// Grab Event ����
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
 desc : ī�޶� ���� ����
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
 desc : ���� ī�޶� ����Ǿ� �ִ��� ����
 parm : None
 retn : TRUE (����Ǿ� �ִ�) or FALSE
*/
BOOL CCamMain::IsConnected()
{
	// ���� ���ŵǾ��ٸ� ���� �ȵǾ� �ִ°ɷ� ����
	if (m_pCamera->IsCameraDeviceRemoved())	return FALSE;

	return (m_pCamera->IsOpen() && m_pCamera->IsPylonDeviceAttached()) ? TRUE : FALSE;
}

/*
 desc : ���� �ֱٿ� �߻��� ���� �� ��ȯ
 parm : None
 retn : ���� ��
*/
UINT16 CCamMain::GetLastError()
{
	/* enum LastErrorEnums */
	/*return UINT8(m_pCamera->LastError.);*/
	return 0;
}

/*
 desc : Grab ���� ó��
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
 desc : Grab ���� ó��
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
		//Ʈ���� ����  (Ʈ���Ÿ�� )
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
 desc : Open�� ī�޶��� Event ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamMain::SetParamUpdate()
{
	BOOL bIsGrab	= FALSE;

	// ���� Grabbing ���� �����, Grab�� �����ϰ� �� ����
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

	// ���� �ֱٿ� Grabbing ���ζ��
	if (bIsGrab)
	{
		if (!StartGrab())	return FALSE;
	}

	return TRUE;
}

/*
 desc : Grab Event ó��
 parm : camera	- [in]  �̺�Ʈ�� �߻��� ī�޶�
		grabbed	- [in]  Grab�� ����� ����� ���� ����
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
		/* �ʴ� 10 �� ������ ���� ĸó�ϵ��� ���� ó�� ���� */
#ifdef _DEBUG
		if (GetTickCount64() < (m_u64TickGrab+250))	return;
#else
		if (GetTickCount64() < (m_u64TickGrab+100))	return;
#endif
		m_u64TickGrab	= GetTickCount64();	/* ���� Grabbed �ð��� ���ؼ� ���� �ð� ���� */
	case ENG_VCCM::en_edge_mode	:
	case ENG_VCCM::en_cali_mode	:
	case ENG_VCCM::en_hole_mode	:
		if (m_syncGrab.Enter())
		{
			if (grabbed->GrabSucceeded())
			{
				// ���� Grabbed�� �̹��� ���� (���) ó��
				m_stGrab.cam_id		= m_u8CamID;
				m_stGrab.img_id		= 0;
				m_stGrab.grab_size	= (UINT32)grabbed->GetImageSize();
				m_stGrab.grab_width	= grabbed->GetWidth();
				m_stGrab.grab_height= grabbed->GetHeight();
 				memcpy(m_stGrab.grab_data, grabbed->GetBuffer(), m_stGrab.grab_size);

				m_stGrab.grab_data[m_stGrab.grab_size]	= 0x00;

				uvMIL_RegistMILImg(m_stGrab.cam_id - 1, m_stGrab.grab_width, m_stGrab.grab_height, m_stGrab.grab_data);

				// lk91 cam �ΰ� ��ȣ ���ö� ���� ����
				//if(m_uDispType == DISP_TYPE_MARK_LIVE || m_uDispType == DISP_TYPE_MARK)
				//	uvMIL_DrawImageBitmap(DISP_TYPE_MARK_LIVE, m_stGrab.cam_id, &m_stGrab, 0.0, m_stGrab.cam_id - 1);
				//else
				//	uvMIL_DrawImageBitmap(m_uDispType, m_stGrab.cam_id, &m_stGrab, 0.0, m_stGrab.cam_id - 1);


				/* Live �����ϰ� ���� ���� */
				if (m_enCamMode != ENG_VCCM::en_live_mode && !m_stGrab.IsEmpty())
				{
					//swprintf_s(tzFile, L"%s\\%s\\cam(%u)_img(0)_%04d%02d%02d_%02d%02d%02d.bmp",
					swprintf_s(tzFile, L"%s\\save_img\\%s\\cam(%u)_img(0)_%04d%02d%02d_%02d%02d%02d.bmp",
							   g_tzWorkDir, tzMode[UINT8(m_enCamMode)], m_u8CamID,
							   stTm.wYear, stTm.wMonth, stTm.wDay, stTm.wHour, stTm.wMinute, stTm.wSecond);
					uvMIL_SaveGrabbedToFile(tzFile, m_stGrab.grab_width, m_stGrab.grab_height, m_stGrab.grab_data);
				}
			}
			// ����ȭ ����
			m_syncGrab.Leave();
		}
		break;

	case ENG_VCCM::en_grab_mode	:
		if (m_syncGrab.Enter())
		{
			// Grabbed�� ���������� ����Ǿ����� ���� �� ���� Grabbed �Ǿ� ����� �̹����� ���� �ʰ� ����
			if (grabbed && grabbed->GrabSucceeded() && m_u8GrabIndex < m_pstConfig->mark_find.max_mark_grab)
			{
				/* ���� ��ϵ� �Ź��� ��ũ �������� ũ�� ���� ó�� (�� �̻� �޸𸮿� �������� ����) */
				/* �ݵ�� ���� �α� ���⵵�� ó�� */

				//LPG_ACGR pstMark	= NULL;
				//pstMark	= NULL;
				//pstMark	= (LPG_ACGR)::Alloc(sizeof(STG_ACGR));
				//ASSERT(pstMark);
				// ���� Grabbed�� �̹��� ���� (���) ó��
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
				/* Grab ���� ���� */
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
			// ����ȭ ����
			m_syncGrab.Leave();
		}

		break;
	}
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

	/* ����ȭ ���� */
	if (m_syncGrab.Enter())
	{
		m_u8GrabIndex	= 0;	
		m_lstMark.RemoveAll();
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
		/* ����ȭ ���� */
		if (m_syncGrab.Enter())
		{
			/* ���� ����Ǿ� �ִ� Grabbed Image�� �ִ��� Ȯ�� */
			if (m_lstMark.GetCount())
			{
				/* �� ó���� ��ϵǾ� �ִ� �׸� ��ȯ */
				pstGrab	= m_lstMark.GetHead();
				/* �� ó���� ��ϵǾ� �ִ� �׸� ���� */
				m_lstMark.RemoveHead();
#if 1
				TRACE(L"CCamMain::GetGrabbedImage() = cam_id=%d, img_id=%d, size=%d\n",
					  pstGrab->cam_id, pstGrab->img_id, pstGrab->grab_size);
#endif
			}
			/* ����ȭ ���� */
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
	m_stGrab.grab_size		= 0;
	m_stGrab.grab_width		= 0;
	m_stGrab.grab_height	= 0;
	m_enCamMode				= mode;

	/* Live Grabbed Mode�� ������� */
	if (m_enCamMode == ENG_VCCM::en_live_mode ||
		m_enCamMode == ENG_VCCM::en_cali_mode)
	{
		m_u64TickGrab	= GetTickCount64();
	}
#if 0	/* ���� ������ �ʿ� ����. �뱤 ������ ��, �����Ѵ�. */
	ResetGrabbedImage();	/* ���� Grabbed�� �̹��� ��� ����� */
#endif
}

/*
 desc : ���� Grabbed Image�� ��ũ �˻� ���� �ʱ�ȭ
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
	if (m_stGrab.grab_size < 1)	return NULL;
	return m_pHistLevel;
}

/*
 desc : ����� ī�޶��� Gain Level �� ����
 parm : level	- [in] 0 ~ 255 (���� Ŭ���� ��� ó��)
 retn : TRUE or FALSE
*/
BOOL CCamMain::SetGainLevel(int level)
{
	BOOL bIsGrab	= FALSE;

	// ���� Grabbing ���� �����, Grab�� �����ϰ� �� ����
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

	// ���� �ֱٿ� Grabbing ���ζ��
	if (bIsGrab)
	{
		if (!StartGrab())	return FALSE;
	}

	return TRUE;
}

/*
 desc : �������� �̹����� �ҷ��ͼ� �����͸� ���� �մϴ�. (���� ��忡�� �ַ� ��� ��)
 parm : file	- [in]  ��ü ��ΰ� ���Ե� 265 grayscale bitmap ����
 retn : TRUE or FALSE
*/
BOOL CCamMain::LoadImageFromFile(PTCHAR file, INT32 cam_id)
{
	UINT32 u32FileSize		= 0;	/* ���� ��ü ũ�� (����: bytes) */
	PUINT8 pImgBuff			= NULL, pImgNext;

	/* ������ ��ü ũ�� */
	u32FileSize	= uvCmn_GetFileSize(file);
	if (u32FileSize < 1)	return FALSE;
	/* ���� ��ü�� ������ ���� ���� */
	pImgBuff = new UINT8[u32FileSize + 1];// (PUINT8)::Alloc(u32FileSize + 1);
	ASSERT(pImgBuff);
	pImgBuff[u32FileSize]	= 0x00;
	/* ���� ��ü ���� ���� */
	if (!uvCmn_LoadFile(file, pImgBuff, u32FileSize))
	{
		delete pImgBuff;
		return FALSE;
	}
	pImgNext	= pImgBuff;
	/* ���� ��� �κ� Skip */
	pImgNext	+= 2/*bfType*/+4/*bfSize*/+4/*bfReserved*/+4/*bfOffBits*/;
	/* �̹��� ���� Skip */
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
	/* �̹��� ������ */
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

	/* �ӽ� ���� ���� */
	delete pImgBuff; 

	return TRUE;
}
