
/*
 desc : Align �뱤 ���� - ����� ī�޶� X �� �̵� ���� �������� Y �ุ ����
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkExpoAlign.h"
#include "../../mesg/DlgMesg.h"
#include "../../GlobalVariables.h"

#include <string>

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif

//extern CACamCali* g_pACamCali;

/*
 desc : ������
 parm : None
 retn : None
*/
CWorkExpoAlign::CWorkExpoAlign()
	: CWorkStep()
{
	m_enWorkJobID	= ENG_BWOK::en_expo_align;
	m_u32ExpoCount = 1;

	m_stExpoLog.Init();
	//memcpy(&m_stExpoLog, expo, sizeof(STG_CELA));
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CWorkExpoAlign::~CWorkExpoAlign()
{
}

/*
 desc : �ʱ� �۾� ����
 parm : None
 retn : None
*/
BOOL CWorkExpoAlign::InitWork()
{
	/* ���� ��� ���� �� �ʱ�ȭ */
	if (!CWork::InitWork())	return FALSE;

	 globalMarkCnt = uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	 localMarkCnt = uvEng_Luria_GetMarkCount(ENG_AMTF::en_local);

	 m_u8MarkCount = globalMarkCnt + (IsMarkTypeOnlyGlobal() == true ? 0 : localMarkCnt);


	 SetAlignMode();

	return TRUE;
}

/*
 desc : �ֱ������� �۾� ����
 parm : None
 retn : None
*/
VOID CWorkExpoAlign::DoWork()
{
	const int Initstep = 0, processWork = 1, checkWorkstep = 2;
	try
	{
		alignCallback[alignMotion][processWork]();
		alignCallback[alignMotion][checkWorkstep]();
		CheckWorkTimeout();
	}
	catch (const std::exception&)
	{

	}
}


void CWorkExpoAlign::SetAlignMode()
{
	auto motion = GlobalVariables::GetInstance()->GetAlignMotion();
	this->alignMotion = motion.markParams.alignMotion;
	this->aligntype = motion.markParams.alignType;
	const int INIT_STEP = 0;
	alignCallback[alignMotion][INIT_STEP]();
	//auto alignMotion = GlobalVariables::GetInstance()->GetAlignMotion();

	//GlobalVariables::GetInstance()->GetAlignMotion().SetFiducialPool(ENG_AMOS mode, ENG_ATGL aligntype);
	//GlobalVariables::GetInstance()->GetAlignMotion().SetAlignMode(motion, aligntype);
}


//void CWorkExpoAlign::GeneratePath(ENG_AMOS mode, ENG_ATGL alignType, vector<STG_XMXY>& path)
//{
//	globalMarkCnt = globalMarkCnt;
//	localMarkCnt = localMarkCnt;
//	//m_u8MarkCount = m_u8MarkCount;
//
//	AlignMotion& motions = GlobalVariables::GetInstance()->GetAlignMotion();
//
//	switch (mode)
//	{
//	case ENG_AMOS::en_onthefly_2cam:
//	{
//
//	}
//	break;
//
//	case ENG_AMOS::en_onthefly_3cam:
//	{
//
//	}
//	break;
//
//	case ENG_AMOS::en_static_2cam:
//	{
//
//	}
//	break;
//
//	case ENG_AMOS::en_static_3cam:
//	{
//		STG_XMXY lookat;
//		const int centercam = 3;
//		bool res = true;
//
//		motions.GetGerberPosUseCamPos(centercam, lookat);
//
//		STG_XMXY current = lookat;
//
//		while (res == true)
//			if (res = motions.GetNearFid(current, alignType == ENG_ATGL::en_global_4_local_0_point ? SearchFlag::global : SearchFlag::all, path, current))
//				path.push_back(current);
//
//
//		//motions
//
//	}
//	break;
//	}
//}



/*
 desc : �뱤 �Ϸ� �� ���� ���� ����
 parm : state	- [in]  0x00: �۾� ����, 0x01: �۾� ����
 retn : None
*/
VOID CWorkExpoAlign::SaveExpoResult(UINT8 state)
{
	//UINT8 i;
	TCHAR tzResult[1024]= {NULL}, tzFile[MAX_PATH_LEN] = {NULL}, tzState[2][8] = {L"FAIL", L"SUCC"};
	TCHAR tzDrv[8]		= {NULL};
	//UINT64 u64Temp[2]	= {NULL};
	//UINT16 (*pLed)[8]	= uvEng_ShMem_GetLuria()->directph.light_source_driver_temp_led;
	//UINT16 (*pBoard)[8]	= uvEng_ShMem_GetLuria()->directph.light_source_driver_temp_board;
	SYSTEMTIME stTm		= {NULL};
	MEMORYSTATUSEX stMem= {NULL};
	LPG_ACGR pstMark	= NULL;
	LPG_RJAF pstRecipe	= uvEng_JobRecipe_GetSelectRecipe();
	CUniToChar csCnv1, csCnv2;

	CUniToChar	csCnv;
	LPG_RAAF pstAlignRecipe = uvEng_Mark_GetAlignRecipeName(csCnv.Ansi2Uni(pstRecipe->align_recipe));
	LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstRecipe->expo_recipe));

	/* ���� ��ǻ�� ��¥�� ���ϸ����� ���� */
	GetLocalTime(&stTm);
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\logs\\expo\\%04d-%02d-%02d.csv",
			   g_tzWorkDir, stTm.wYear, stTm.wMonth, stTm.wDay);

	/* ���࿡ ������ �ϴ� ������ ������, �ش� ���� ���� ��, Ÿ��Ʋ (Field) �߰� */
	if (!uvCmn_FindFile(tzFile))
	{
		wcscpy_s(tzResult, 1024,
				 //L"date,recipe,gerber,succ,hot_air,temp_di_1,temp_di_2,temp_di_3,temp_di_4,"
				 //L"hdd_free(GB),mem_free(GB),"
				 //L"led(1:1),led(1:2),led(1:3),led(1:4),board(1:1),board(1:2),board(1:3),board(1:4),"
				 //L"led(2:1),led(2:2),led(2:3),led(2:4),board(2:1),board(2:2),board(2:3),board(2:4),"
				 //L"horz_dist_13(mm),horz_dist_24(mm),vert_dist_12(mm),vert_dist_34(mm),"
				 //L"diag_dist_14(mm),diag_dist_23(mm),expo_time(ms),"
				 //L"score_1,scale_1,scale_size_1,mark_move_x1(mm),mark_move_y1(mm),"
				 //L"score_2,scale_2,scale_size_2,mark_move_x2(mm),mark_move_y2(mm),"
				 //L"score_3,scale_3,scale_size_3,mark_move_x3(mm),mark_move_y3(mm),"
				 //L"score_4,scale_4,scale_size_4,mark_move_x4(mm),mark_move_y4(mm),\n");
			L"date,tack,succ,"
			L"gerber_name,material_thick(um),expo_eneray(mj),"
			L"horz_dist_13(mm),horz_dist_24(mm),vert_dist_12(mm),vert_dist_34(mm),"
			L"diag_dist_14(mm),diag_dist_23(mm),"
			L"score_1,scale_1,scale_size_1,mark_move_x1(mm),mark_move_y1(mm),"
			L"score_2,scale_2,scale_size_2,mark_move_x2(mm),mark_move_y2(mm),"
			L"score_3,scale_3,scale_size_3,mark_move_x3(mm),mark_move_y3(mm),"
			L"score_4,scale_4,scale_size_4,mark_move_x4(mm),mark_move_y4(mm),\n");
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x00);
	}
	/* �߻� �ð� */
	swprintf_s(tzResult, 1024, L"%02d:%02d:%02d,", stTm.wHour, stTm.wMinute, stTm.wSecond);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	/*ExpoLog ���*/
	memcpy(m_stExpoLog.data, tzResult, 40);

	/* �뱤 �ð� / �뱤 ���� / �Ź� �̸� / ���� �β� / ������ */
	UINT64 u64JobTime = uvEng_GetJobWorkTime();
	swprintf_s(tzResult, 1024, L"%u,%s,%S,%d,%.4f,",
		uvCmn_GetTimeToType(u64JobTime, 0x03), tzState[state], pstRecipe->gerber_name, pstRecipe->material_thick, pstRecipe->expo_energy);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);


	/* ��ũ ���� 6 �� ���� ���� ���� �� �� ��ü �뱤�ϴµ� �ҿ�� �ð� ���� */
	LPG_GMLV pstMarkDiff = &uvEng_GetConfig()->mark_diff;
	swprintf_s(tzResult, 1024, L"%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,",
		pstMarkDiff->result[0].diff * 100.0f, pstMarkDiff->result[1].diff * 100.0f, pstMarkDiff->result[2].diff * 100.0f,
		pstMarkDiff->result[3].diff * 100.0f, pstMarkDiff->result[4].diff * 100.0f, pstMarkDiff->result[5].diff * 100.0f);
	//swprintf_s(tzResult, 1024, L"%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,",
	//		   uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_top_horz),
	//		   uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_btm_horz),
	//		   uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_lft_vert),
	//		   uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_rgt_vert),
	//		   uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_lft_diag),
	//		   uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_rgt_diag)
	//		   );
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	/* ����� ��ũ �˻� ��� �� ���� */
	pstMark	= uvEng_Camera_GetGrabbedMark(0x01, 0x00);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%u,%.4f,%.4f,",
				   pstMark->score_rate, pstMark->scale_rate, pstMark->scale_size,
				   pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	pstMark	= uvEng_Camera_GetGrabbedMark(0x01, 0x01);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%u,%.4f,%.4f,",
				   pstMark->score_rate, pstMark->scale_rate, pstMark->scale_size,
				   pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	pstMark	= uvEng_Camera_GetGrabbedMark(0x02, 0x00);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%u,%.4f,%.4f,",
				   pstMark->score_rate, pstMark->scale_rate, pstMark->scale_size,
				   pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	pstMark	= uvEng_Camera_GetGrabbedMark(0x02, 0x01);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%u,%.4f,%.4f,",
				   pstMark->score_rate, pstMark->scale_rate, pstMark->scale_size,
				   pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	/* �������� ������ ���� �������� �Ѿ���� �ϱ� ���� */
	uvCmn_SaveTxtFileW(L"\n", (UINT32)wcslen(L"\n"), tzFile, 0x01);


	/*ExpoLog ���*/
	memcpy(m_stExpoLog.gerber_name, pstRecipe->gerber_name, MAX_GERBER_NAME);
	m_stExpoLog.material_thick = pstRecipe->material_thick;
	m_stExpoLog.expo_energy = pstRecipe->expo_energy;
	m_stExpoLog.align_type = pstAlignRecipe->align_type;
	m_stExpoLog.mark_type = pstAlignRecipe->mark_type;
	m_stExpoLog.lamp_type = pstAlignRecipe->lamp_type;
	m_stExpoLog.gain_level[0] = pstAlignRecipe->gain_level[0];
	m_stExpoLog.gain_level[1] = pstAlignRecipe->gain_level[1];
}


void CWorkExpoAlign::DoInitOnthefly2cam()
{
	m_u8StepTotal = 0x24;
}

void CWorkExpoAlign::DoAlignOnthefly2cam()
{
	auto motions = GlobalVariables::GetInstance()->GetAlignMotion();
	switch (m_u8StepIt)/* �۾� �ܰ� ���� ���� ó�� */
	{
	case 0x01: 
	{
		m_enWorkState = SetExposeReady(TRUE, TRUE, TRUE, 1);

		if (m_enWorkState == ENG_JWNS::en_next)
		{
			alignOffsetPool.clear();
			motions.SetFiducialPool();
		}
	}
	break;
	case 0x02: m_enWorkState = IsLoadedGerberCheck();						break;	/* �Ź��� ����Ǿ���, Mark�� �����ϴ��� Ȯ�� */
	case 0x03: m_enWorkState = SetTrigEnable(FALSE);						break;	/* Trigger Event - ��Ȱ��ȭ ���� */
	case 0x04: m_enWorkState = IsTrigEnabled(FALSE);						break;	/* Trigger Event - ��Ȱ��ȭ Ȯ��  */
	case 0x05: 
	{
		m_enWorkState = SetAlignMovingInit();
	}
	break;	/* Stage X/Y, Camera 1/2 - Align (Global) ���� ��ġ�� �̵� */

	case 0x06: m_enWorkState = SetTrigPosCalcSaved();						break;	/* Trigger �߻� ��ġ ��� �� �ӽ� ���� */
	case 0x07: m_enWorkState = IsAlignMovedInit();							break;	/* Stage X/Y, Camera 1/2 - Align (Global) ���� ��ġ ���� ���� */
	case 0x08: m_enWorkState = SetTrigRegistGlobal();						break;	/* Trigger �߻� ��ġ - Ʈ���� ���忡 Global Mark ��ġ ��� */
	case 0x09: m_enWorkState = IsTrigRegistGlobal();						break;	/* Trigger �߻� ��ġ ��� Ȯ�� */
	case 0x0a: m_enWorkState = SetAlignMeasMode();							break;
	case 0x0b: m_enWorkState = IsAlignMeasMode();							break;
	case 0x0c: m_enWorkState = SetAlignMovingGlobal();						break;	/* Global Mark 4 ���� ��ġ Ȯ�� */
	case 0x0d: m_enWorkState = IsAlignMovedGlobal();						break;	/* Global Mark 4 ���� ���� �Ϸ� ���� */
	case 0x0e:
	{
		//������� ������ ���þ������ �ִ°�. ���� �۷ι��� ���������� Ȯ���ؾ���.
		CameraSetCamMode(ENG_VCCM::en_none);
		
		m_enWorkState = SetAlignMovingLocal((UINT8)AlignMotionMode::toInitialMoving, scanCount);	break;	/* Stage X/Y, Camera 1/2 - Align (Local:������) ���� ��ġ�� �̵� */
	}

	case 0x0f: m_enWorkState = SetTrigRegistLocal(scanCount);										break;	/* Trigger (������) �߻� ��ġ - Ʈ���� ���忡 Local Mark ��ġ ��� */
	case 0x10: m_enWorkState = IsTrigRegistLocal(scanCount);										break;	/* Trigger (������) �߻� ��ġ ��� Ȯ�� */
	case 0x11:
	{
		m_enWorkState = IsAlignMovedLocal((UINT8)AlignMotionMode::toInitialMoving, scanCount);
		
	}
	break;	/* Stage X/Y, Camera 1/2 - Align (Local) ���� ��ġ ���� ���� */

	case 0x12:
	{
		m_enWorkState = CameraSetCamMode(ENG_VCCM::en_grab_mode);
		
	}
	break;	/* Cam None ���� ���� */

	case 0x13: m_enWorkState = SetAlignMovingLocal((UINT8)AlignMotionMode::toScanMoving, scanCount);		break;	/* Local Mark (������) 16 ���� ��ġ Ȯ�� */
	case 0x14: m_enWorkState = IsAlignMovedLocal((UINT8)AlignMotionMode::toScanMoving, scanCount);		break;	/* Local Mark (������) 16 ���� ���� �Ϸ� ���� */

	case 0x15:
		this_thread::sleep_for(chrono::milliseconds(200)); // ��� ��ٷ��ִ� ������ �ٽ��� �����忡�� ķ �����͸� �������� ������ �������� Ÿ���� �ֱ⶧��.
		m_u8StepIt = GlobalVariables::GetInstance()->GetAlignMotion().CheckAlignScanFinished(++scanCount) ? 0x16 : 0x0e; //���������� �ٽ� �ö�. 
		m_enWorkState = ENG_JWNS::en_forceSet;
		break;

	case 0x16:
		m_enWorkState = CameraSetCamMode(ENG_VCCM::en_none);break;	/* Cam None ���� ���� */

	case 0x17: m_enWorkState = SetTrigEnable(FALSE);						break;
	case 0x18:
	{
		SetUIRefresh(true);
		m_enWorkState = IsGrabbedImageCount(m_u8MarkCount, 3000);
	}
	break;
	case 0x19:
	{
		
		m_enWorkState = IsSetMarkValidAll(0x01);
	}
	break;

	case 0x1a:m_enWorkState = SetAlignMarkRegist();break;
	case 0x1b:m_enWorkState = IsAlignMarkRegist();break;
	case 0x1c: m_enWorkState = IsTrigEnabled(FALSE);						break;

	case 0x1d: m_enWorkState = SetPrePrinting();							break;	/* Luria Control - PrePrinting */
	case 0x1e: m_enWorkState = IsPrePrinted();								break;	/* Luria Control - PrePrinted Ȯ�� */
	
	case 0x1f:
	{
		m_enWorkState = SetPrinting();
	}
	break;	/* Luria Control - Printing */
	case 0x20: m_enWorkState = IsPrinted();								break;	/* Luria Control - Printed Ȯ�� */

	case 0x21: m_enWorkState = SetWorkWaitTime(1000);						break;	/* ���� �ð� ��� */
	case 0x22: m_enWorkState = IsWorkWaitTime();							break;	/* ��� �Ϸ� ���� Ȯ�� */

	case 0x23: m_enWorkState = SetMovingUnloader();						break;	/* Stage Unloader ��ġ�� �̵� */
	case 0x24: m_enWorkState = IsMovedUnloader();							break;	/* Stage Unloader ��ġ�� ���� �ߴ��� ���� Ȯ�� */


	}
	
	
}

void CWorkExpoAlign::SetWorkNextOnthefly2cam()
{
	UINT64 u64TickCount = GetTickCount64();
	UINT64 u64JobTime = u64TickCount - m_u64StartTime;

	/* �� �۾� �������� �ð� �� ���� ó�� */
	uvEng_UpdateJobWorkTime(u64JobTime);

	/* ��� �۾��� ���� �Ǿ����� ���� */
	if (ENG_JWNS::en_error == m_enWorkState)
	{
		SaveExpoResult(0x00);
		m_u8StepIt = 0x01;

		TCHAR tzMesg[128] = { NULL };
		swprintf_s(tzMesg, 128, L"Work Expo Align <Error Step It = 0x%02x>", m_u8StepIt);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);

		//m_enWorkState = ENG_JWNS::en_comp;
		m_enWorkState = ENG_JWNS::en_error;
	}
	else if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* �۾��� ��� �� �ӽ� ���� */
		CWork::CalcStepRate();

		/* �ʿ信 ���� �б� ó�� */
		switch (m_u8StepIt)
		{
			/* Local Mark ���� ���� Ȯ�� */
		case 0x0d:	if (!uvEng_Luria_IsMarkLocal())	m_u8StepIt = 0x19;	break;
			/* �뱤 �۾��� �Ϸ�� ����, �ٷ� ���а� �� �µ� �� ��û */
		case 0x24: uvEng_Luria_ReqGetPhLedTempAll();	break;
		}

		/* ��� ������ �Ϸ�Ǿ����� Ȯ�� */
		if (m_u8StepTotal == m_u8StepIt)
		{
			/* �۾� �Ϸ� �� ���� �ʿ��� ���� ���� */
			SaveExpoResult(0x01);

			//if (++m_u32ExpoCount != 10000)
			if (++m_u32ExpoCount != 2)
			{
				m_u8StepIt = 0x01;

#ifdef _DEBUG
				TCHAR szMesg[LOG_MESG_SIZE] = { NULL };

				swprintf_s(szMesg, LOG_MESG_SIZE, L"Expo Only Step :  m_u32ExpoCount= %d / m_u8StepTotal = 10000\n"
					, m_u32ExpoCount);

				/*Log ���*/
				m_strLog.Format(szMesg);
				txtWrite(m_strLog);
#endif


				/*Auto Mdoe�� �뱤 ���ᰡ �Ǹ� Philhmil�� �ϷẸ��*/
				//if (g_u8Romote == en_menu_phil_mode_auto)
				if (g_u16PhilCommand == (int)ENG_PHPC::ePHILHMI_C2P_PROCESS_EXECUTE)
				{
					SetPhilProcessCompelet();
				}
			}
			else
			{
				/*Auto Mdoe�� �뱤 ���ᰡ �Ǹ� Philhmil�� �ϷẸ��*/
				if (g_u16PhilCommand == (int)ENG_PHPC::ePHILHMI_C2P_PROCESS_EXECUTE)
				{
					SetPhilProcessCompelet();
				}


				m_enWorkState = ENG_JWNS::en_comp;

				/* �׻� ȣ��*/
				CWork::EndWork();
			}

		}
		else
		{
			/* ���� �۾� �ܰ�� �̵� */
			m_u8StepIt++;
		}
		/* Align Mark�� ���� �ν� ��, �뱤�� ������ ������ ���� ����� */
		if (m_u8StepIt == 0x1f && uvEng_GetConfig()->set_uvdi15.check_query_expo)
		{
			CDlgMesg dlgMesg;
			if (IDOK != dlgMesg.MyDoModal(L"Do you really want to expose?", 0x02))
			{
				m_u8StepIt = 0x00;
				m_enWorkState = ENG_JWNS::en_comp;
			}
		}
		/* ���� �ֱٿ� Waiting �� �ð� ���� */
		m_u64DelayTime = GetTickCount64();
	}
}
	 
void CWorkExpoAlign::DoInitOnthefly3cam()
{

}

void CWorkExpoAlign::DoAlignOnthefly3cam()
{

}

void CWorkExpoAlign::SetWorkNextOnthefly3cam()
{

}
	 
void CWorkExpoAlign::DoInitStatic2cam()
{

}

void CWorkExpoAlign::DoAlignStatic2cam()
{

}

void CWorkExpoAlign::SetWorkNextStatic2cam()
{

}
	 
void CWorkExpoAlign::DoInitStaticCam()
{

	m_u8StepTotal = 0x14;

	
}

void CWorkExpoAlign::DoAlignStaticCam()
{
	

	AlignMotion& motions = GlobalVariables::GetInstance()->GetAlignMotion();

	int CENTER_CAM = motions.markParams.centerCamIdx;

	try
	{
		switch (m_u8StepIt)/* �۾� �ܰ� ���� ���� ó�� */
		{
		case 0x01: 
		{
			
			m_enWorkState = SetExposeReady(TRUE, TRUE, TRUE, 1);

			if (m_enWorkState == ENG_JWNS::en_next)
			{
				alignOffsetPool.clear();
				motions.SetFiducialPool();
				grabMarkPath = motions.GetFiducialPool(CENTER_CAM);
			}

		}
		break;	    /* �뱤 ������ �������� ���� Ȯ�� */

		case 0x02: 
		{
			
			m_enWorkState = ENG_JWNS::en_next;  //IsLoadedGerberCheck(); ���ʿ�.
		}break;	/* �Ź��� ����Ǿ���, Mark�� �����ϴ��� Ȯ�� */
		case 0x03: m_enWorkState = SetTrigEnable(FALSE);						break;	/* Trigger Event - ��Ȱ��ȭ ���� */
		case 0x04: m_enWorkState = IsTrigEnabled(FALSE);						break;	/* Trigger Event - ��Ȱ��ȭ Ȯ��  */
		case 0x05:
		{		
			uvEng_ACamCali_ResetAllCaliData();

			uvEng_Camera_ResetGrabbedImage();
			m_enWorkState = CameraSetCamMode(ENG_VCCM::en_grab_mode);

			

		}
		break;	//3ķ �̵���ġ ��μ���

		case 0x06:
		{
			m_enWorkState = grabMarkPath.size() == 0 ? ENG_JWNS::en_error : ENG_JWNS::en_next;
			//string temp = "x" + std::to_string(CENTER_CAM);
			//if (m_enWorkState == ENG_JWNS::en_next && uvEng_GetConfig()->set_align.use_2d_cali_data)
				/*for (int i = 0; i < grabMarkPath.size(); i++)
				{
					auto stageAlignOffset = motions.EstimateOffset(CENTER_CAM, grabMarkPath[i].mark_x, grabMarkPath[i].mark_y, CENTER_CAM == 3 ? 0 : motions.GetAxises()["cam"][temp.c_str()].currPos);
					uvEng_ACamCali_AddMarkPosForce(CENTER_CAM, grabMarkPath[i].GetFlag(STG_XMXY_RESERVE_FLAG::GLOBAL) ? ENG_AMTF::en_global : ENG_AMTF::en_local, stageAlignOffset.offsetX, stageAlignOffset.offsetY);
				}*/

			//���⼭ �������. 	
		}
		break;

		case 0x07:
		{
			auto SingleGrab = [&](int camIndex) -> bool {return uvEng_Mvenc_ReqTrigOutOne_(0b1100); };

			bool complete = GlobalVariables::GetInstance()->Waiter([&]()->bool
				{
					if (motions.NowOnMoving() == true)
					{
						this_thread::sleep_for(chrono::milliseconds(100));
					}
					else
					{
						if (grabMarkPath.size() == 0)
							return true;

						auto first = grabMarkPath.begin();
						auto arrival = motions.MovetoGerberPos(CENTER_CAM, *first);

						if (arrival == true)
						{
							const int STABLE_TIME = 1000;
							this_thread::sleep_for(chrono::milliseconds(STABLE_TIME));
							
							string temp = "x" + std::to_string(CENTER_CAM);
							
							
							auto alignOffset = motions.EstimateOffset(CENTER_CAM, 
																	motions.GetAxises()["stage"]["x"].currPos,
																	motions.GetAxises()["stage"]["y"].currPos,
																	CENTER_CAM == 3 ? 0 : motions.GetAxises()["cam"][temp.c_str()].currPos);
							
							
							string tempo;
							alignOffset.srcFid = *first;
							alignOffsetPool.push_back(alignOffset);

							if (SingleGrab(CENTER_CAM))
							{
								if (uvEng_GetConfig()->set_align.use_2d_cali_data)
									uvEng_ACamCali_AddMarkPosForce(CENTER_CAM, first->GetFlag(STG_XMXY_RESERVE_FLAG::GLOBAL) ? ENG_AMTF::en_global : ENG_AMTF::en_local, alignOffset.offsetX, alignOffset.offsetY);

								grabMarkPath.erase(first);

							}
						}
					}
					return false;
				}, 60 * 1000 * 2);
			m_enWorkState = complete == true ? ENG_JWNS::en_next : ENG_JWNS::en_error;
		}
		break;

		case 0x08:
		{
			SetUIRefresh(true);
			m_enWorkState = IsGrabbedImageCount(m_u8MarkCount, 3000, &CENTER_CAM);
		}
		break;


		case 0x09:
		{
			m_enWorkState = IsSetMarkValidAll(0x01, &CENTER_CAM);
		}
		break;
		
		case 0x0a:
		{
			motions.SetAlignOffsetPool(alignOffsetPool);
			m_enWorkState = CameraSetCamMode(ENG_VCCM::en_none); 
		}
		break;

		case 0x0b: 
		{
			m_enWorkState = SetAlignMarkRegistforStatic();
		}
		break;
		
		case 0x0c:m_enWorkState = IsAlignMarkRegist(); 
			break;
		
		
		case 0x0d: 
			m_enWorkState = SetPrePrinting();							
			break;

		case 0x0e:
			m_enWorkState = IsPrePrinted();
			break;

		case 0x0f:
			m_enWorkState = SetPrinting();
			//m_enWorkState = SetPrinting(); 
			break;
		case 0x10:
			m_enWorkState = IsPrinted();								
			break;
		case 0x11:
			m_enWorkState = SetWorkWaitTime(1000);						
			break;	/* ���� �ð� ��� */
		case 0x12:
			m_enWorkState = IsWorkWaitTime();							
			break;	/* ��� �Ϸ� ���� Ȯ�� */
		case 0x13: 
			m_enWorkState = SetMovingUnloader();						
			break;	/* Stage Unloader ��ġ�� �̵� */
		case 0x14: 
			m_enWorkState = IsMovedUnloader();							
			break;	/* Stage Unloader ��ġ�� ���� �ߴ��� ���� Ȯ�� */


		}
	}
	catch (const std::exception&)
	{

	}
}

void CWorkExpoAlign::SetWorkNextStaticCam()
{
	UINT8 u8WorkTotal = m_u8StepTotal;
	UINT64 u64JobTime = GetTickCount64() - m_u64StartTime;

	uvEng_UpdateJobWorkTime(u64JobTime);

	if (ENG_JWNS::en_error == m_enWorkState)
	{
		TCHAR tzMesg[128] = { NULL };
		swprintf_s(tzMesg, 128, L"Align Test <Error Step It = 0x%02x>", m_u8StepIt);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);

		SaveExpoResult(0x00);
		m_u8StepIt = 0x01;
		m_enWorkState = ENG_JWNS::en_error;
	}
	else if (ENG_JWNS::en_next == m_enWorkState)
	{
		CWork::CalcStepRate();
		if (m_u8StepTotal == m_u8StepIt)
		{
			SaveExpoResult(0x01);

			if (++m_u32ExpoCount != 2)
			{
				m_u8StepIt = 0x01;

#ifdef _DEBUG
				TCHAR szMesg[LOG_MESG_SIZE] = { NULL };

				swprintf_s(szMesg, LOG_MESG_SIZE, L"Expo Only Step :  m_u32ExpoCount= %d / m_u8StepTotal = 10000\n"
					, m_u32ExpoCount);

				/*Log ���*/
				m_strLog.Format(szMesg);
				txtWrite(m_strLog);
#endif


				/*Auto Mdoe�� �뱤 ���ᰡ �Ǹ� Philhmil�� �ϷẸ��*/
				//if (g_u8Romote == en_menu_phil_mode_auto)
				if (g_u16PhilCommand == (int)ENG_PHPC::ePHILHMI_C2P_PROCESS_EXECUTE)
				{
					SetPhilProcessCompelet();
				}
			}
			else
			{
				/*Auto Mdoe�� �뱤 ���ᰡ �Ǹ� Philhmil�� �ϷẸ��*/
				if (g_u16PhilCommand == (int)ENG_PHPC::ePHILHMI_C2P_PROCESS_EXECUTE)
				{
					SetPhilProcessCompelet();
				}


				m_enWorkState = ENG_JWNS::en_comp;

				/* �׻� ȣ��*/
				CWork::EndWork();
			}
		}
		else
		{
			m_u8StepIt++;
		}
		m_u64DelayTime = GetTickCount64();
	}
	
}



VOID CWorkExpoAlign::SetPhilProcessCompelet()
{
	STG_PP_P2C_PROCESS_COMP			stProcessComp;
	STG_PP_P2C_PROCESS_COMP_ACK		stProcessCompAck;
	stProcessComp.Reset();
	stProcessCompAck.Reset();

	/*Process Execute���� ���� ����*/
	sprintf_s(stProcessComp.szRecipeName, DEF_MAX_RECIPE_NAME_LENGTH, m_stExpoLog.recipe_name);
	sprintf_s(stProcessComp.szGlassID, DEF_MAX_GLASS_NAME_LENGTH, m_stExpoLog.glassID);

	/*�뱤 ��� �Ķ���Ͱ�*/
	stProcessComp.usCount = 3;
	sprintf_s(stProcessComp.stVar[0].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "CHAR");
	sprintf_s(stProcessComp.stVar[0].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "data");
	sprintf_s(stProcessComp.stVar[0].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, m_stExpoLog.data);

	sprintf_s(stProcessComp.stVar[1].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "UINT64");
	sprintf_s(stProcessComp.stVar[1].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "expo_time");
	sprintf_s(stProcessComp.stVar[1].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.expo_time);

	sprintf_s(stProcessComp.stVar[2].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "UINT8");
	sprintf_s(stProcessComp.stVar[2].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "expo_succ");
	sprintf_s(stProcessComp.stVar[2].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.expo_succ);

	uvEng_Philhmi_Send_P2C_PROCESS_COMP(stProcessComp, stProcessCompAck);
}

VOID CWorkExpoAlign::txtWrite(CString msg)
{
	CStdioFile	imsifile;

	TCHAR tzFile[_MAX_PATH] = { NULL };
	SYSTEMTIME stTm = { NULL };

	GetLocalTime(&stTm);

	TCHAR tzPath[_MAX_PATH];
	swprintf_s(tzPath, _MAX_PATH, L"%s\\logs\\expo\\%04d-%02d-%02d ExpoAlign.dat", g_tzWorkDir, stTm.wYear, stTm.wMonth, stTm.wDay);


	imsifile.Open(tzPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite);

	imsifile.SeekToEnd();

	swprintf_s(tzFile, L"[%04d-%02d-%02d %02d:%02d:%02d.%03d]  ",
		(UINT16)stTm.wYear, (UINT16)stTm.wMonth, (UINT16)stTm.wDay,
		(UINT16)stTm.wHour, (UINT16)stTm.wMinute, (UINT16)stTm.wSecond, (UINT16)stTm.wMilliseconds);

	imsifile.WriteString(tzFile);
	imsifile.WriteString(msg);

	imsifile.Close();

}
