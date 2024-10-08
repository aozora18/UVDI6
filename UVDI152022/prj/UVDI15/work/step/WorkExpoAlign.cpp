
/*
 desc : Align �뱤 ���� - ����� ī�޶� X �� �̵� ���� �������� Y �ุ ����
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkExpoAlign.h"
#include "../../mesg/DlgMesg.h"
#include "../../GlobalVariables.h"
//#include <fmt/core.h>
#include <string>
#include "../../stuffs.h"

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
	m_u8StepIt = 0;
	m_stExpoLog.Init();
	//memcpy(&m_stExpoLog, expo, sizeof(STG_CELA));

	/*UI�� ǥ�õǴ� �߱Ⱚ �ʱ�ȭ*/
	LPG_PPTP pstParams = &uvEng_ShMem_GetLuria()->panel.get_transformation_params;
	pstParams->rotation = 0;
	pstParams->scale_xy[0] = 0;
	pstParams->scale_xy[1] = 0;
	uvEng_GetConfig()->measure_flat.MeasurePoolClear();
	LPG_GMLV pstMarkDiff = &uvEng_GetConfig()->mark_diff;
	pstMarkDiff->ResetMarkLen();
	GlobalVariables::GetInstance()->GetAlignMotion().markParams.workErrorType = ENG_WETE::en_none;
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


bool CWorkExpoAlign::SetAlignMode()
{
	auto& motion = GlobalVariables::GetInstance()->GetAlignMotion();

	

	this->alignMotion = motion.markParams.alignMotion;
	this->aligntype = motion.markParams.alignType;

	if (alignMotion == ENG_AMOS::none || aligntype == ENG_ATGL::en_global_0_local_0x0_n_point)
		return FALSE;

	const int INIT_STEP = 0;
	alignCallback[alignMotion][INIT_STEP]();
	
}

/*
 desc : �뱤 �Ϸ� �� ���� ���� ����
 parm : state	- [in]  0x00: �۾� ����, 0x01: �۾� ����
 retn : None
*/
VOID CWorkExpoAlign::SaveExpoResult(UINT8 state)
{
	//UINT8 i;
	TCHAR tzResult[1024] = { NULL }, tzFile[MAX_PATH_LEN] = { NULL }, tzState[2][8] = { L"FAIL", L"SUCC" };
	TCHAR tzDrv[8] = { NULL };
	//UINT64 u64Temp[2]	= {NULL};
	//UINT16 (*pLed)[8]	= uvEng_ShMem_GetLuria()->directph.light_source_driver_temp_led;
	//UINT16 (*pBoard)[8]	= uvEng_ShMem_GetLuria()->directph.light_source_driver_temp_board;
	SYSTEMTIME stTm = { NULL };
	MEMORYSTATUSEX stMem = { NULL };
	LPG_ACGR pstMark = NULL;
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	CUniToChar csCnv1, csCnv2;

	CUniToChar	csCnv;
	LPG_RAAF pstAlignRecipe = uvEng_Mark_GetAlignRecipeName(csCnv.Ansi2Uni(pstRecipe->align_recipe));
	LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstRecipe->expo_recipe));

	/*������ ���� ��������*/
	LPG_RJAF pstJobRecipe = uvEng_JobRecipe_GetSelectRecipe();

	/* ���� ��ǻ�� ��¥�� ���ϸ����� ���� */
	GetLocalTime(&stTm);
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\logs\\expo\\%04d-%02d-%02d AlignExpo.csv",
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
			L"time,tack,succ,"
			L"gerber_name,material_thick(um),expo_eneray(mj),"
			L"horz_dist_13(mm),horz_dist_24(mm),vert_dist_12(mm),vert_dist_34(mm),"
			L"diag_dist_14(mm),diag_dist_23(mm),"
			L"score_1,scale_1,mark_move_x1(mm),mark_move_y1(mm),"
			L"score_2,scale_2,mark_move_x2(mm),mark_move_y2(mm),"
			L"score_3,scale_3,mark_move_x3(mm),mark_move_y3(mm),"
			L"score_4,scale_4,mark_move_x4(mm),mark_move_y4(mm),"
			L"read_thick(mm),\n");
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x00);
	}

	/* �߻� �ð� */
	swprintf_s(tzResult, 1024, L"%02d:%02d:%02d,", stTm.wHour, stTm.wMinute, stTm.wSecond);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	/*ExpoLog ���*/
	memcpy(m_stExpoLog.data, tzResult, 40);


	/* �µ� / �뱤 �ð� / �뱤 ���� / �뱤 ��� / ���� ���� / ���� ȸ�� */
	//swprintf_s(tzResult, 1024, L"\%llu,%s,%.2f,%.2f,",
	//	uvEng_GetJobWorkTime(), tzState[state], pstExpoRecipe->real_scale_range, pstExpoRecipe->real_rotaion_range);
	//uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	/* �뱤 Tack / �뱤 ���� ���� / �Ź� �̸� / ���� �β�/ ���� */
	UINT64 u64JobTime = uvEng_GetJobWorkTime();
	swprintf_s(tzResult, 1024, L"%um%02us,%s,%S,%dum,%.1fmj,",
		uvCmn_GetTimeToType(u64JobTime, 0x01), uvCmn_GetTimeToType(u64JobTime, 0x02),
		tzState[state], pstJobRecipe->gerber_name, pstJobRecipe->material_thick, pstJobRecipe->expo_energy);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);

	/*ExpoLog ���*/
	m_stExpoLog.expo_time = uvEng_GetJobWorkTime();
	m_stExpoLog.expo_succ = state;
	m_stExpoLog.real_scale = pstExpoRecipe->real_scale_range;
	m_stExpoLog.real_rotaion = pstExpoRecipe->real_rotaion_range;


	/* ��ũ ���� 6 �� ���� ���� ���� �� �� ��ü �뱤�ϴµ� �ҿ�� �ð� ���� */
	LPG_GMLV pstMarkDiff = &uvEng_GetConfig()->mark_diff;
	swprintf_s(tzResult, 1024, L"%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,",
		pstMarkDiff->result[0].diff * 100.0f, pstMarkDiff->result[1].diff * 100.0f, pstMarkDiff->result[2].diff * 100.0f,
		pstMarkDiff->result[3].diff * 100.0f, pstMarkDiff->result[4].diff * 100.0f, pstMarkDiff->result[5].diff * 100.0f);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);

	m_stExpoLog.global_dist[0] = pstMarkDiff->result[0].diff * 100.0f;
	m_stExpoLog.global_dist[1] = pstMarkDiff->result[1].diff * 100.0f;
	m_stExpoLog.global_dist[2] = pstMarkDiff->result[2].diff * 100.0f;
	m_stExpoLog.global_dist[3] = pstMarkDiff->result[3].diff * 100.0f;
	m_stExpoLog.global_dist[4] = pstMarkDiff->result[4].diff * 100.0f;
	m_stExpoLog.global_dist[5] = pstMarkDiff->result[5].diff * 100.0f;

	/* ����� ��ũ �˻� ��� �� ���� */
	pstMark = uvEng_Camera_GetGrabbedMark(0x01, 0x00);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%.4f,%.4f,",
			pstMark->score_rate, pstMark->scale_rate,
			pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	pstMark = uvEng_Camera_GetGrabbedMark(0x01, 0x01);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%.4f,%.4f,",
			pstMark->score_rate, pstMark->scale_rate,
			pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	pstMark = uvEng_Camera_GetGrabbedMark(0x02, 0x00);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%.4f,%.4f,",
			pstMark->score_rate, pstMark->scale_rate,
			pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	pstMark = uvEng_Camera_GetGrabbedMark(0x02, 0x01);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%.4f,%.4f,",
			pstMark->score_rate, pstMark->scale_rate,
			pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}

	auto& measureFlat = uvEng_GetConfig()->measure_flat;
	auto mean = measureFlat.GetThickMeasureMean();
	if (measureFlat.u8UseThickCheck)
	{
		DOUBLE RealThick;
		DOUBLE LDSToThickOffset = 0;
		DOUBLE dmater = pstRecipe->material_thick / 1000.0f;
		LDSToThickOffset = uvEng_GetConfig()->measure_flat.dOffsetZPOS;

		RealThick = mean + dmater + LDSToThickOffset;
		swprintf_s(tzResult, 1024, L"%.3f,", RealThick);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	///* ��ũ ���� 6 �� ���� ���� ���� �� �� ��ü �뱤�ϴµ� �ҿ�� �ð� ���� */
	//swprintf_s(tzResult, 1024, L"%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,",
	//	uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_top_horz),
	//	uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_btm_horz),
	//	uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_lft_vert),
	//	uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_rgt_vert),
	//	uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_lft_diag),
	//	uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_rgt_diag));
	//uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	///*ExpoLog ���*/
	//m_stExpoLog.global_dist[0] = uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_top_horz);
	//m_stExpoLog.global_dist[1] = uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_btm_horz);
	//m_stExpoLog.global_dist[2] = uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_lft_vert);
	//m_stExpoLog.global_dist[3] = uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_rgt_vert);
	//m_stExpoLog.global_dist[4] = uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_lft_diag);
	//m_stExpoLog.global_dist[5] = uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_rgt_diag);

	/*ExpoLog ���*/
	//memcpy(m_stExpoLog.gerber_name, pstJobRecipe->gerber_name, MAX_GERBER_NAME);
	strcpy_s(m_stExpoLog.gerber_name, MAX_GERBER_NAME, pstJobRecipe->gerber_name);
	m_stExpoLog.material_thick = pstJobRecipe->material_thick;
	m_stExpoLog.expo_energy = pstJobRecipe->expo_energy;

	m_stExpoLog.align_type = pstAlignRecipe->align_type;
#ifdef USE_ALIGNMOTION
	m_stExpoLog.align_motion = pstAlignRecipe->align_motion;
#endif
	m_stExpoLog.mark_type = pstAlignRecipe->mark_type;
	m_stExpoLog.lamp_type = pstAlignRecipe->lamp_type;
	m_stExpoLog.gain_level[0] = pstAlignRecipe->gain_level[0];
	m_stExpoLog.gain_level[1] = pstAlignRecipe->gain_level[1];

	m_stExpoLog.led_duty_cycle = pstExpoRecipe->led_duty_cycle;
	//sprintf_s(m_stExpoLog.power_name, LED_POWER_NAME_LENGTH, pstExpoRecipe->power_name);
	strcpy_s(m_stExpoLog.power_name, LED_POWER_NAME_LENGTH, pstExpoRecipe->power_name);

	/* �������� ������ ���� �������� �Ѿ���� �ϱ� ���� */
	uvCmn_SaveTxtFileW(L"\n", (UINT32)wcslen(L"\n"), tzFile, 0x01);


}


void CWorkExpoAlign::DoInitOnthefly2cam()
{
	m_u8StepTotal = 0x29;
	m_u8StepIt = 1;
}

void CWorkExpoAlign::DoAlignOnthefly2cam()
{
	auto& motions = GlobalVariables::GetInstance()->GetAlignMotion();
	switch (m_u8StepIt)/* �۾� �ܰ� ���� ���� ó�� */
	{
	case 0x01: 
	{
		m_enWorkState = SetExposeReady(TRUE, TRUE, TRUE, 1);
		offsetPool.clear();
		motions.SetFiducialPool();

		if (motions.IsAlignComplete())
		{
			uvEng_Camera_ResetGrabbedImage();
			motions.SetAlignComplete(false);
		}
	}
	break;
	
	case 0x02:
	{
		m_enWorkState = SetPhZAxisMovingAll();
	}break;

	case 0x03:
	{
		m_enWorkState = IsPhZAxisMovedAll();
	}break;

	case 0x04:
	{
		auto res = SetACamZAxisMovingAll(m_lastUniqueID);
		m_enWorkState = res != ENG_JWNS::en_next ? ENG_JWNS::en_wait : res;
		if (m_enWorkState != ENG_JWNS::en_next)
			this_thread::sleep_for(std::chrono::microseconds(500));
	}break;

	case 0x05:
	{
		m_enWorkState = IsACamZAxisMovedAll(m_lastUniqueID);

	}break;

	case 0x07: 
	{
		m_enWorkState = IsLoadedGerberCheck();


		if (m_enWorkState == ENG_JWNS::en_next && !uvEng_GetConfig()->set_align.manualFixOffsetAtSequence)
		{
			auto res = motions.IsNeedManualFixOffset(nullptr);
			if (res == ENG_MFOR::noNeedToFix)
			{
				//m_u8StepIt = 0x1a;
				m_u8StepIt = 0x18;
				m_enWorkState = ENG_JWNS::en_forceSet;
			}
			else if (res == ENG_MFOR::firstRun) {}
			else
			{
				m_enWorkState = ENG_JWNS::en_error;
			}
		}
	}
	break;	/* �Ź��� ����Ǿ���, Mark�� �����ϴ��� Ȯ�� */

	case 0x08: m_enWorkState = SetTrigEnable(FALSE);						break;	/* Trigger Event - ��Ȱ��ȭ ���� */
	case 0x09: m_enWorkState = IsTrigEnabled(FALSE);						break;	/* Trigger Event - ��Ȱ��ȭ Ȯ��  */
	case 0x0a: 
	{
		m_enWorkState = SetAlignMovingInit();
	}
	break;	/* Stage X/Y, Camera 1/2 - Align (Global) ���� ��ġ�� �̵� */

	case 0x0b: m_enWorkState = SetTrigPosCalcSaved();						break;	/* Trigger �߻� ��ġ ��� �� �ӽ� ���� */
	case 0x0c: m_enWorkState = IsAlignMovedInit();							break;	/* Stage X/Y, Camera 1/2 - Align (Global) ���� ��ġ ���� ���� */
	case 0x0d: m_enWorkState = SetTrigRegistGlobal();						break;	/* Trigger �߻� ��ġ - Ʈ���� ���忡 Global Mark ��ġ ��� */
	case 0x0e: m_enWorkState = IsTrigRegistGlobal();						break;	/* Trigger �߻� ��ġ ��� Ȯ�� */
	case 0x0f: m_enWorkState = SetAlignMeasMode();							break;
	case 0x10:
	{
		m_enWorkState = IsAlignMeasMode();
		SetActionRequest(ENG_RIJA::clearMarkData);
		
	}
	break;
	case 0x11: 
	{
		m_enWorkState = SetAlignMovingGlobal();						
	}
	break;	/* Global Mark 4 ���� ��ġ Ȯ�� */

	case 0x12: m_enWorkState = IsAlignMovedGlobal();						break;	/* Global Mark 4 ���� ���� �Ϸ� ���� */
	case 0x13:
	{
		//������� ������ ���þ������ �ִ°�. ���� �۷ι��� ���������� Ȯ���ؾ���.
		CameraSetCamMode(ENG_VCCM::en_none);
		
		m_enWorkState = SetAlignMovingLocal((UINT8)AlignMotionMode::toInitialMoving, scanCount);	break;	/* Stage X/Y, Camera 1/2 - Align (Local:������) ���� ��ġ�� �̵� */
	}

	case 0x14: m_enWorkState = SetTrigRegistLocal(scanCount);										break;	/* Trigger (������) �߻� ��ġ - Ʈ���� ���忡 Local Mark ��ġ ��� */
	case 0x15: m_enWorkState = IsTrigRegistLocal(scanCount);										break;	/* Trigger (������) �߻� ��ġ ��� Ȯ�� */
	case 0x16:
	{
		m_enWorkState = IsAlignMovedLocal((UINT8)AlignMotionMode::toInitialMoving, scanCount);
		
	}
	break;	/* Stage X/Y, Camera 1/2 - Align (Local) ���� ��ġ ���� ���� */

	case 0x17:
	{
		m_enWorkState = CameraSetCamMode(ENG_VCCM::en_grab_mode);
		
	}
	break;	/* Cam None ���� ���� */

	case 0x18: m_enWorkState = SetAlignMovingLocal((UINT8)AlignMotionMode::toScanMoving, scanCount);		break;	/* Local Mark (������) 16 ���� ��ġ Ȯ�� */
	case 0x19: m_enWorkState = IsAlignMovedLocal((UINT8)AlignMotionMode::toScanMoving, scanCount);		break;	/* Local Mark (������) 16 ���� ���� �Ϸ� ���� */

	case 0x1a:
		this_thread::sleep_for(chrono::milliseconds(200)); // ��� ��ٷ��ִ� ������ �ٽ��� �����忡�� ķ �����͸� �������� ������ �������� Ÿ���� �ֱ⶧��.
		m_u8StepIt = GlobalVariables::GetInstance()->GetAlignMotion().CheckAlignScanFinished(++scanCount) ? 0x16 : 0x0e; //���������� �ٽ� �ö�. 
		m_enWorkState = ENG_JWNS::en_forceSet;
		break;

	case 0x1b:
		m_enWorkState = CameraSetCamMode(ENG_VCCM::en_none);break;	/* Cam None ���� ���� */

	case 0x1c: m_enWorkState = SetTrigEnable(FALSE);						break;
	case 0x1d:
	{
		SetUIRefresh(true);
		m_enWorkState = IsGrabbedImageCount(m_u8MarkCount, 3000);
	}
	break;
	case 0x1e:
	{
		
		m_enWorkState = IsSetMarkValidAll(0x01);
	}
	break;

	case 0x1f:
	{
		m_enWorkState = SetAlignMarkRegist();

	}
	break;
	case 0x20:
	{
		m_enWorkState = IsAlignMarkRegist();
		if(m_enWorkState == ENG_JWNS::en_next)
			motions.SetAlignComplete(true);
		
	}
	break;
	case 0x21: m_enWorkState = IsTrigEnabled(FALSE);						break;

	case 0x22: m_enWorkState = SetPrePrinting();							break;	/* Luria Control - PrePrinting */
	case 0x23: m_enWorkState = IsPrePrinted();								break;	/* Luria Control - PrePrinted Ȯ�� */
	
	case 0x24:
	{
		m_enWorkState = SetPrinting();
	}
	break;	/* Luria Control - Printing */
	case 0x25: m_enWorkState = IsPrinted();								break;	/* Luria Control - Printed Ȯ�� */

	case 0x26: m_enWorkState = SetWorkWaitTime(1000);						break;	/* ���� �ð� ��� */
	case 0x27: m_enWorkState = IsWorkWaitTime();							break;	/* ��� �Ϸ� ���� Ȯ�� */

	case 0x28: m_enWorkState = SetMovingUnloader();						break;	/* Stage Unloader ��ġ�� �̵� */
	case 0x29: m_enWorkState = IsMovedUnloader();							break;	/* Stage Unloader ��ġ�� ���� �ߴ��� ���� Ȯ�� */


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
		m_u8StepIt = 0x00;

		if (g_u8Romote == en_menu_phil_mode_auto)
		{
			/*�뱤 ���ᰡ �Ǹ� Philhmil�� �ϷẸ��*/
			SetPhilProcessCompelet();
		}

		TCHAR tzMesg[128] = { NULL };
		swprintf_s(tzMesg, 128, L"Work Expo Align <Error Step It = 0x%02x>", m_u8StepIt);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
		//m_enWorkState = ENG_JWNS::en_comp;
		m_enWorkState = ENG_JWNS::en_error;
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
		m_enWorkState = ENG_JWNS::en_error;
#endif

	}
	else if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* �۾��� ��� �� �ӽ� ���� */
		CWork::CalcStepRate();

		/* �ʿ信 ���� �б� ó�� */
		switch (m_u8StepIt)
		{
		/* Local Mark ���� ���� Ȯ�� */
		//case 0x0d:	if (!uvEng_Luria_IsMarkLocal())	m_u8StepIt = 0x19;	break;
		case 0x0d:	if (!uvEng_Luria_IsMarkLocal())	m_u8StepIt = 0x17;	break;
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
				m_u8StepIt = 0x00;

#ifdef _DEBUG
				TCHAR szMesg[LOG_MESG_SIZE] = { NULL };

				swprintf_s(szMesg, LOG_MESG_SIZE, L"Expo Only Step :  m_u32ExpoCount= %d / m_u8StepTotal = 10000\n"
					, m_u32ExpoCount);

				/*Log ���*/
				m_strLog.Format(szMesg);
				txtWrite(m_strLog);
#endif


				/*Auto Mdoe�� �뱤 ���ᰡ �Ǹ� Philhmil�� �ϷẸ��*/
				if (g_u8Romote == en_menu_phil_mode_auto)
				//if (g_u16PhilCommand == (int)ENG_PHPC::ePHILHMI_C2P_PROCESS_EXECUTE)
				{
					SetPhilProcessCompelet();
				}
			}
			else
			{
				/*Auto Mdoe�� �뱤 ���ᰡ �Ǹ� Philhmil�� �ϷẸ��*/
				if (g_u8Romote == en_menu_phil_mode_auto)
				//if (g_u16PhilCommand == (int)ENG_PHPC::ePHILHMI_C2P_PROCESS_EXECUTE)
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
	
	m_u8StepIt = 0;

	
}

void CWorkExpoAlign::DoAlignStaticCam()
{
	
	AlignMotion& motions = GlobalVariables::GetInstance()->GetAlignMotion();
	RefindMotion& refindMotion = GlobalVariables::GetInstance()->GetRefindMotion();

	int CENTER_CAM = motions.markParams.centerCamIdx;
	auto& webMonitor = GlobalVariables::GetInstance()->GetWebMonitor();
	const int PAIR = 2;
	const int MARK1 = 0, MARK2 = 1;
	bool refind = refindMotion.IsUseRefind();

	vector<function<void()>> stepWork =
	{
		[&]()
		{
			webMonitor.Clear(nullptr);
			m_enWorkState = SetExposeStartXY();
		},
		[&]()
		{
			m_enWorkState = IsExposeStartXY();
		},

		[&]()
		{
			if (!(motions.GetCalifeature(OffsetType::align).caliCamIdx == CENTER_CAM) ||
				!(motions.GetCalifeature(OffsetType::expo).caliCamIdx == CENTER_CAM)) //���̺��� ���� �ִ������� �˻�.
				{
					m_enWorkState = ENG_JWNS::en_error;
				}
			else
				m_enWorkState = ENG_JWNS::en_next;

		},
		[&]()
		{
			m_enWorkState = SetExposeReady(TRUE, TRUE, TRUE, 1);
		},

		[&]()
		{
			m_enWorkState = SetPhZAxisMovingAll();
		},
		[&]()
		{
			m_enWorkState = IsPhZAxisMovedAll();
		},

		[&]()
		{
			auto res = SetACamZAxisMovingAll(m_lastUniqueID);
			m_enWorkState = res != ENG_JWNS::en_next ? ENG_JWNS::en_wait : res;
			if (m_enWorkState != ENG_JWNS::en_next)
				this_thread::sleep_for(std::chrono::microseconds(500));
		},
		[&]()
		{
			m_enWorkState = IsACamZAxisMovedAll(m_lastUniqueID);
		},

		[&]()
		{
			static map<int, ENG_MMDI> axisMap =
			{
				{1,ENG_MMDI::en_align_cam1},
				{2,ENG_MMDI::en_align_cam2},
				{3,ENG_MMDI::en_axis_none}
			};
			auto res = MoveCamToSafetypos(axisMap[CENTER_CAM], motions.GetCalifeature(OffsetType::expo).caliCamXPos);
			m_enWorkState = res ? ENG_JWNS::en_next : ENG_JWNS::en_error;
		},
		[&]()
		{
			m_enWorkState = SetTrigEnable(FALSE);
		},
		[&]()
		{
			m_enWorkState = IsTrigEnabled(FALSE);
		},
		[&]()
		{
			uvEng_ACamCali_ResetAllCaliData();
			uvEng_Camera_ResetGrabbedImage();
			SetActionRequest(ENG_RIJA::clearMarkData);
			m_enWorkState = CameraSetCamMode(ENG_VCCM::en_grab_mode);
		},
		[&]()
		{
			offsetPool.clear();
			motions.SetFiducialPool();
			grabMarkPath = motions.GetFiducialPool(CENTER_CAM);
			m_enWorkState = grabMarkPath.size() == 0 ? ENG_JWNS::en_error : ENG_JWNS::en_next;
		},
		[&]()
		{
			if (refind)
			{
				SetStepName(L"mark refind step");
				SetActionRequest(ENG_RIJA::invalidateUI);
			}
			m_enWorkState = ENG_JWNS::en_next;
		},
		[&]()
		{

			if (refind == false)
			{
				m_enWorkState = ENG_JWNS::en_next;
			}
			else
			{
				bool errFlag = false;
				try
				{
					//std::vector<STG_XMXY> filteredPath, offsetBuff;
					//std::copy_if(grabMarkPath.begin(), grabMarkPath.end(), std::back_inserter(filteredPath),
					//			[&](const STG_XMXY& v) { return v.tgt_id == MARK1 || v.tgt_id == MARK2; });
					std::vector<STG_XMXY> filteredPath, offsetBuff;
					std::copy_if(grabMarkPath.begin(), grabMarkPath.end(), std::back_inserter(filteredPath),
						[&](const STG_XMXY& v) { return (v.reserve & STG_XMXY_RESERVE_FLAG::GLOBAL) == STG_XMXY_RESERVE_FLAG::GLOBAL && (v.tgt_id == MARK1 || v.tgt_id == MARK2); });


					if (filteredPath.size() != PAIR)
						throw exception();

					if (refindMotion.ProcessEstimateRST(CENTER_CAM, filteredPath, errFlag, offsetBuff) == false)
						throw exception();

					if (errFlag == true)
						throw exception();

					auto match = std::remove_if(grabMarkPath.begin(), grabMarkPath.end(),
						[&](const STG_XMXY& v) {  return (v.reserve & STG_XMXY_RESERVE_FLAG::GLOBAL) == STG_XMXY_RESERVE_FLAG::GLOBAL && (v.tgt_id == MARK1 || v.tgt_id == MARK2); });

					std::transform(offsetBuff.begin(), offsetBuff.end(), std::back_inserter(offsetPool[OffsetType::refind]),
					[&](STG_XMXY v)->CaliPoint
					{
						auto find = std::find_if(filteredPath.begin(), filteredPath.end(), [&](STG_XMXY sv) {return sv.org_id == v.org_id; });
						return CaliPoint(find->mark_x,find->mark_y, v.mark_x,v.mark_y,v.offsetX,v.offsetY, *find);
					}); //�����ε� �ɼ¿� 2�� �߰�.

					grabMarkPath.erase(match, grabMarkPath.end());//2����Ʈ�� �������� ���ָ�ȴ�. 

					//*****************************************************************************//
					CaliPoint align;

					//for (int i = 0; i < PAIR; i++)
					//{
					//	CommonMotionStuffs::GetInstance().GetOffsetsUseMarkPos(CENTER_CAM, filteredPath[i], &align, nullptr, offsetBuff[i].mark_x , offsetBuff[i].mark_y ); //<-�����ɼ� ���������
					//	offsetPool[OffsetType::align].push_back(align);						
					//}

					SetUIRefresh(true);
					m_enWorkState = ENG_JWNS::en_next;
				}
				catch (...)
				{
					m_enWorkState = ENG_JWNS::en_error;
					return;
				}
			}
		},
		[&]()
		{
			const int STABLE_TIME = 1000;
			bool complete = false;

			SetStepName(L"mark find step");
			SetActionRequest(ENG_RIJA::invalidateUI);
			complete = GlobalVariables::GetInstance()->Waiter([&]()->bool
			{
				try
				{
					if (CommonMotionStuffs::GetInstance().NowOnMoving())
					{
						return false;
					}
					else
					{
						auto currPath = grabMarkPath.begin();

						bool arrival = false;
						double grabOffsetX = 0, grabOffsetY = 0;
						double estimatedX = 0, estimatedY = 0;
						STG_XMXY estimatedXMXY = *currPath;//STG_XMXY(currPath->mark_x, currPath->mark_y, currPath->org_id);
						CaliPoint alignOffset;// , expoOffset;// , refindOffset;

						if (refind)
							refindMotion.GetEstimatePos(currPath->mark_x, currPath->mark_y, estimatedXMXY.mark_x, estimatedXMXY.mark_y);

						if (motions.MovetoGerberPos(CENTER_CAM, estimatedXMXY) == false)
							throw exception();

						this_thread::sleep_for(chrono::milliseconds(STABLE_TIME));
						motions.Refresh();

						if (CommonMotionStuffs::GetInstance().SingleGrab(CENTER_CAM) == false || CWork::GetAbort()) //�׷�����. �۾� �ܺ�����
							throw exception();

						auto found = CommonMotionStuffs::GetInstance().IsMarkFindInLastGrab(CENTER_CAM, &grabOffsetX, &grabOffsetY);

						if (found == false)
						{
							if (refind)
							{
								if (motions.MovetoGerberPos(CENTER_CAM, *currPath) == false)//�������� ���������� �̵�.
									throw exception();

								if (CommonMotionStuffs::GetInstance().SingleGrab(CENTER_CAM) == false || CWork::GetAbort()) //�׷�����. �۾� �ܺ�����
									throw exception();

								tuple<double, double> refindOffset,grabOffset;
								auto found = CommonMotionStuffs::GetInstance().IsMarkFindInLastGrab(CENTER_CAM, &grabOffsetX, &grabOffsetY);

								if (found == true) //������ǥ���� ������
								{

									STG_XMXY temp = STG_XMXY(currPath->mark_x + grabOffsetX, currPath->mark_y + grabOffsetY);
									if (CommonMotionStuffs::GetInstance().GetOffsetsCurrPos(CENTER_CAM, *currPath, &alignOffset, nullptr, 0, 0) == false) //<-�����ɼ� ���������
										throw exception();

									offsetPool[OffsetType::refind].push_back(CaliPoint(currPath->mark_x, currPath->mark_y, 0, 0, std::get<0>(grabOffset), std::get<1>(grabOffset) ,*currPath));

									/*webMonitor.AddLog(fmt::format("<{}>refind �����߰� ������ǥ���� ã��!!  ORG_ID{} , TGT_ID{} , ������ũ��ġx = {} , ������ũ��ġy = {}, �׷������ɼ�x = {} , �׷��ɻ����� y={} , ��������οɼ�X = {} , ��������οɼ�y={}\r\n",
													currPath->GetFlag(STG_XMXY_RESERVE_FLAG::GLOBAL) ? "global" : "local", currPath->org_id, currPath->tgt_id, currPath->mark_x,currPath->mark_y, grabOffsetX, grabOffsetY, alignOffset.offsetX, alignOffset.offsetY));*/
								}
								else
								{
									if (refindMotion.ProcessRefind(CENTER_CAM, &refindOffset,&grabOffset) == false) //����ǥ ȸ�� �Ŀ� refind�� ��ã������
										throw exception();

									//ã��.
									STG_XMXY temp = STG_XMXY(currPath->mark_x + std::get<0>(refindOffset) + std::get<0>(grabOffset),
															 currPath->mark_y + std::get<1>(refindOffset) + std::get<1>(grabOffset));

									if (CommonMotionStuffs::GetInstance().GetOffsetsCurrPos(CENTER_CAM, *currPath, &alignOffset, nullptr, std::get<0>(refindOffset), std::get<1>(refindOffset)) == false) //<-�����ɼ� ���������
										throw exception();

									offsetPool[OffsetType::refind].push_back(CaliPoint(currPath->mark_x, currPath->mark_y, std::get<0>(refindOffset), std::get<1>(refindOffset), std::get<0>(grabOffset), std::get<1>(grabOffset) , *currPath));

									/*webMonitor.AddLog(fmt::format("<{}>refind�� ã��!!  ORG_ID{} , TGT_ID{} , ������ũ��ġx = {} , ������ũ��ġy = {},�����ε�ɼ�x = {} , �����ε�ɼ�y = {}, �׷������ɼ�x = {} , �׷��ɻ����� y={} , ��������οɼ�X = {} , ��������οɼ�y={}\r\n",
										currPath->GetFlag(STG_XMXY_RESERVE_FLAG::GLOBAL) ? "global" : "local", currPath->org_id, currPath->tgt_id, currPath->mark_x, currPath->mark_y, std::get<0>(refindOffset), std::get<1>(refindOffset), std::get<0>(grabOffset), std::get<1>(grabOffset), alignOffset.offsetX, alignOffset.offsetY));*/

								}
							}
							else
							{
								offsetPool[OffsetType::refind].push_back(CaliPoint(currPath->mark_x, currPath->mark_y,
									estimatedXMXY.mark_x - currPath->mark_x,
									estimatedXMXY.mark_y - currPath->mark_y, 0, 0, *currPath));
								//!!!!!!!!!!!!!���� �������ؼ��� �̿��ؾ��Ұ�� ���⿡�� ó���ϸ� ��.!!!!!!!!!!!!!!
								//�ϴ� ��ã������ �ٷ� ĵ��. 
								//throw exception();
							}
						}
						else
						{
							STG_XMXY combineAddGrabOffset = STG_XMXY(currPath->mark_x + (estimatedXMXY.mark_x - currPath->mark_x) + grabOffsetX,
														currPath->mark_y + (estimatedXMXY.mark_y - currPath->mark_y) + grabOffsetY,
														currPath->tgt_id);

							//CommonMotionStuffs::GetInstance().GetCurrentOffsets(CENTER_CAM, combineAddGrabOffset, alignOffset, expoOffset);
							offsetPool[OffsetType::refind].push_back(CaliPoint(currPath->mark_x, currPath->mark_y,
																				estimatedXMXY.mark_x - currPath->mark_x,
																				estimatedXMXY.mark_y - currPath->mark_y, grabOffsetX, grabOffsetY,*currPath));
						}

						//offsetPool[OffsetType::align].push_back(alignOffset);

						grabMarkPath.erase(currPath);

						if (grabMarkPath.empty())
							return true;

						return false;
					}
				}
				catch (...)
				{
					m_enWorkState = ENG_JWNS::en_error;
					return true;
				}}, (60 * 1000) * grabMarkPath.size());
				SetUIRefresh(true);
			m_enWorkState = complete == true && m_enWorkState != ENG_JWNS::en_error ? ENG_JWNS::en_next : ENG_JWNS::en_error;
		},
		[&]()
		{
			m_enWorkState = IsGrabbedImageCount(m_u8MarkCount, 3000, &CENTER_CAM);
		},
		[&]()
		{
			try
			{
				bool manualFixed = false;
				m_enWorkState = IsSetMarkValidAll(0x01,&manualFixed, &CENTER_CAM);
				if(m_enWorkState == ENG_JWNS::en_next && manualFixed == true )
				for (auto& v : offsetPool[OffsetType::refind]) //���������� �߻��ߴٸ� �ɼ��� �ٽ� �������ش�. 
				{
					auto grab = uvEng_GetGrabUseMark(CENTER_CAM, v.srcFid);
					if (grab == nullptr)
						throw exception();

					v.suboffsetX = grab->move_mm_x;
					v.suboffsetY = grab->move_mm_y;
				}
			}
			catch (...)
			{
				m_enWorkState = ENG_JWNS::en_error;
			}
		},
		[&]()
		{
			int representCount = offsetPool[OffsetType::refind].size();
			CaliPoint expoCali, grabCali;

			if (representCount >= globalMarkCnt) //�۷ι� ��ũī��Ʈ���� Ŀ���Ѵ�. 
			try
			{
				double mark1RefindX = 0, mark1RefindY = 0, mark2RefindX = 0, mark2RefindY = 0; //POOL �ֱ����� ó���ؾ��Ѵ�. 
				double mark1GrabX, mark1GrabY, mark2GrabX, mark2GrabY;
				double mark1SumX, mark1SumY, mark2SumX, mark2SumY;

				mark1RefindX = offsetPool[OffsetType::refind][MARK1].offsetX;
				mark1RefindY = offsetPool[OffsetType::refind][MARK1].offsetY;
				mark2RefindX = offsetPool[OffsetType::refind][MARK2].offsetX;
				mark2RefindY = offsetPool[OffsetType::refind][MARK2].offsetY;

				mark1GrabX = offsetPool[OffsetType::refind][MARK1].suboffsetX;
				mark1GrabY = offsetPool[OffsetType::refind][MARK1].suboffsetY;
				mark2GrabX = offsetPool[OffsetType::refind][MARK2].suboffsetX;
				mark2GrabY = offsetPool[OffsetType::refind][MARK2].suboffsetY;

				mark1SumX = mark1RefindX - mark1GrabX;
				mark1SumY = mark1RefindY - mark1GrabY;
				mark2SumX = mark2RefindX - mark2GrabX;
				mark2SumY = mark2RefindY - mark2GrabY;

				bool needChangeExpoLocation = (fabs(mark1SumX) > refindMotion.thresholdDist || fabs(mark1SumY) > refindMotion.thresholdDist || fabs(mark2SumX) > refindMotion.thresholdDist || fabs(mark2SumY) > refindMotion.thresholdDist);

				auto xShiftGab = needChangeExpoLocation ? ((mark1SumX + mark2SumX) / 2.0f) : 0;
				auto yShiftGab = needChangeExpoLocation ? ((mark1SumY + mark2SumY) / 2.0f) : 0;

				motions.markParams.SetExpoShiftValue(xShiftGab, yShiftGab);
				m_enWorkState = SetExposeStartXY(&xShiftGab, &yShiftGab);

			}
			catch (...)
			{
				m_enWorkState = ENG_JWNS::en_error;
				return;
			}
			m_enWorkState = ENG_JWNS::en_next;
		},
		[&]()
		{
			CaliPoint expoOffset, grabOffset,alignOffset;
			double expoOffsetX = 0, expoOffsetY = 0;
			motions.markParams.GetExpoShiftValue(expoOffsetX, expoOffsetY);

			for (auto v : offsetPool[OffsetType::refind])
			{
				grabOffset = v;

				grabOffset.offsetX = Stuffs::CutEpsilon(expoOffsetX - (v.offsetX - v.suboffsetX));
				grabOffset.offsetY = Stuffs::CutEpsilon(expoOffsetY - (v.offsetY - v.suboffsetY));

				offsetPool[OffsetType::grab].push_back(grabOffset); //�ϴ� grab�ɼ��� �߰�. 

				uvEng_FixMoveOffsetUseMark(CENTER_CAM, v.srcFid, -v.offsetX + (v.suboffsetX), -v.offsetY + (v.suboffsetY), true);

				if (uvEng_GetConfig()->set_align.use_2d_cali_data == false)
					continue;

				if (CommonMotionStuffs::GetInstance().GetOffsetsUseMarkPos(CENTER_CAM, v.srcFid, &alignOffset, nullptr, v.offsetX, v.offsetY) == false)
				{
					m_enWorkState = ENG_JWNS::en_error;
					return;
				}

				uvEng_FixMoveOffsetUseMark(CENTER_CAM, v.srcFid, alignOffset.offsetX, alignOffset.offsetY);
				offsetPool[OffsetType::align].push_back(alignOffset); //����� �ɼ��� �߰�. 
				uvEng_ACamCali_AddMarkPosForce(CENTER_CAM, v.srcFid.GetFlag(STG_XMXY_RESERVE_FLAG::GLOBAL) ? ENG_AMTF::en_global : ENG_AMTF::en_local, alignOffset.offsetX, alignOffset.offsetY);

				if(CommonMotionStuffs::GetInstance().GetOffsetsUseMarkPos(CENTER_CAM, v.srcFid, nullptr, &expoOffset, v.offsetX, v.offsetY) == false)
				{
					m_enWorkState = ENG_JWNS::en_error;
					return;
				}

				offsetPool[OffsetType::expo].push_back(expoOffset);   //�ͽ��� �ɼ��� �߰�. 

				
			}

			m_enWorkState = ENG_JWNS::en_next;
		},
		[&]()
		{
			motions.SetAlignOffsetPool(offsetPool);
			m_enWorkState = CameraSetCamMode(ENG_VCCM::en_none);
		},
		[&]()
		{
			m_enWorkState = IsExposeStartXY();
		},
		[&]()
		{
			m_enWorkState = SetAlignMarkRegistforStatic();
		},
		[&]()
		{
			m_enWorkState = IsAlignMarkRegist();
		},
		[&]()
		{
			m_enWorkState = SetPrePrinting();
		},
		[&]()
		{
			m_enWorkState = IsPrePrinted();
		},
		[&]()
		{
			m_enWorkState = SetPrinting();
		},
		[&]()
		{
			m_enWorkState = IsPrinted();
		},
		[&]()
		{
			m_enWorkState = SetWorkWaitTime(1000);
		},
		[&]()
		{
			m_enWorkState = IsWorkWaitTime();
		},
		[&]()
		{
			m_enWorkState = SetMovingUnloader();
		},
		[&]()
		{
			m_enWorkState = IsMovedUnloader();
			SetProcessComplete();
		},
	};

	m_u8StepTotal = stepWork.size();

	try
	{
		stepWork[m_u8StepIt]();
		
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
	TCHAR tzMesg[128] = { NULL };

	if (CWork::GetAbort())
	{
		CWork::EndWork();
		return;
	}

	if (ENG_JWNS::en_error == m_enWorkState)
	{	
		swprintf_s(tzMesg, 128, L"Align Test <Error Step It = 0x%02x>", m_u8StepIt);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);

		
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
		SaveExpoResult(0x00);
		m_enWorkState = ENG_JWNS::en_comp;
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
		SaveExpoResult(0x00);
		m_u8StepIt = 0x00;
#endif

		if (g_u8Romote == en_menu_phil_mode_auto)
		{
			/*�뱤 ���ᰡ �Ǹ� Philhmil�� �ϷẸ��*/
			SetPhilProcessCompelet();
		}

	}
	else if (ENG_JWNS::en_next == m_enWorkState)
	{
		CWork::CalcStepRate();
		m_u8StepIt++;
		if (m_u8StepTotal == m_u8StepIt)
		{
			SaveExpoResult(0x01);
			m_u8StepIt = 0x00;
			if (++m_u32ExpoCount != 2)
			{
				m_u8StepIt = 0x00;

#ifdef _DEBUG
				TCHAR szMesg[LOG_MESG_SIZE] = { NULL };

				swprintf_s(szMesg, LOG_MESG_SIZE, L"Expo Only Step :  m_u32ExpoCount= %d / m_u8StepTotal = 10000\n"
					, m_u32ExpoCount);

				/*Log ���*/
				m_strLog.Format(szMesg);
				txtWrite(m_strLog);
#endif


				/*Auto Mdoe�� �뱤 ���ᰡ �Ǹ� Philhmil�� �ϷẸ��*/
				if (g_u8Romote == en_menu_phil_mode_auto)
				//if (g_u16PhilCommand == (int)ENG_PHPC::ePHILHMI_C2P_PROCESS_EXECUTE)
				{
					SetPhilProcessCompelet();
				}

			}
			else
			{
				/*Auto Mdoe�� �뱤 ���ᰡ �Ǹ� Philhmil�� �ϷẸ��*/
				if (g_u8Romote == en_menu_phil_mode_auto)
				//if (g_u16PhilCommand == (int)ENG_PHPC::ePHILHMI_C2P_PROCESS_EXECUTE)
				{
					SetPhilProcessCompelet();
				}


				m_enWorkState = ENG_JWNS::en_comp;

				/* �׻� ȣ��*/
				CWork::EndWork();
			}
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

	stProcessComp.usCount = 15;
	sprintf_s(stProcessComp.stVar[0].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stProcessComp.stVar[0].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Data");
	sprintf_s(stProcessComp.stVar[0].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%S", m_stExpoLog.data);

	sprintf_s(stProcessComp.stVar[1].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stProcessComp.stVar[1].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Time");
	sprintf_s(stProcessComp.stVar[1].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%lld", m_stExpoLog.expo_time);

	sprintf_s(stProcessComp.stVar[2].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "BOOL");
	sprintf_s(stProcessComp.stVar[2].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Mode");
	sprintf_s(stProcessComp.stVar[2].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "0");

	sprintf_s(stProcessComp.stVar[3].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "BOOL");
	sprintf_s(stProcessComp.stVar[3].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Succ");
	sprintf_s(stProcessComp.stVar[3].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.expo_succ);

	sprintf_s(stProcessComp.stVar[4].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stProcessComp.stVar[4].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "GrabDist_top_horz");
	sprintf_s(stProcessComp.stVar[4].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%.4f", m_stExpoLog.global_dist[0]);

	sprintf_s(stProcessComp.stVar[5].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stProcessComp.stVar[5].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "GrabDist_btm_horz");
	sprintf_s(stProcessComp.stVar[5].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%.4f", m_stExpoLog.global_dist[0]);

	sprintf_s(stProcessComp.stVar[6].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stProcessComp.stVar[6].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "GrabDist_lft_vert");
	sprintf_s(stProcessComp.stVar[6].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%.4f", m_stExpoLog.global_dist[0]);

	sprintf_s(stProcessComp.stVar[7].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stProcessComp.stVar[7].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "GrabDist_rgt_vert");
	sprintf_s(stProcessComp.stVar[7].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%.4f", m_stExpoLog.global_dist[0]);

	sprintf_s(stProcessComp.stVar[8].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stProcessComp.stVar[8].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "GrabDist_lft_diag");
	sprintf_s(stProcessComp.stVar[8].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%.4f", m_stExpoLog.global_dist[0]);

	sprintf_s(stProcessComp.stVar[9].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stProcessComp.stVar[9].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "GrabDist_rgt_diag");
	sprintf_s(stProcessComp.stVar[9].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%.4f", m_stExpoLog.global_dist[0]);

	sprintf_s(stProcessComp.stVar[10].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stProcessComp.stVar[10].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Gerber_Name");
	sprintf_s(stProcessComp.stVar[10].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%s", m_stExpoLog.gerber_name);

	sprintf_s(stProcessComp.stVar[11].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "INT");
	sprintf_s(stProcessComp.stVar[11].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Material_Thick");
	sprintf_s(stProcessComp.stVar[11].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.material_thick);

	sprintf_s(stProcessComp.stVar[12].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stProcessComp.stVar[12].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Expo_Energy");
	sprintf_s(stProcessComp.stVar[12].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, " %.1f", m_stExpoLog.expo_energy);

	sprintf_s(stProcessComp.stVar[13].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stProcessComp.stVar[13].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Power_Name");
	sprintf_s(stProcessComp.stVar[13].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%s", m_stExpoLog.power_name);

	sprintf_s(stProcessComp.stVar[14].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "INT");
	sprintf_s(stProcessComp.stVar[14].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Led_Duty_Cycle");
	sprintf_s(stProcessComp.stVar[14].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.led_duty_cycle);

	if (!m_stExpoLog.expo_succ)
	{
		stProcessComp.usErrorCode = ePHILHMI_ERR_STATUS_COMPLETE;
	}

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
