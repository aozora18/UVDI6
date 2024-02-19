
/*
 desc : Expose - Only Direct
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkExpoOnly.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : ������
 parm : expo	- [in]  �뱤 ����
 retn : None
*/
CWorkExpoOnly::CWorkExpoOnly(LPG_CELA expo)
	: CWorkStep()
{
	m_enWorkJobID		= ENG_BWOK::en_expo_only;
	m_bMoveACamSide		= FALSE;
	m_bMovePhUpDown		= FALSE;
	m_u32ExpoCount		= 0;
	m_u64TickACamSide	= 0;

	//memcpy(&m_stExpoInfo, expo, sizeof(STG_CPHE));

	m_stExpoLog.Init();
	memcpy(&m_stExpoLog, expo, sizeof(STG_CELA));
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CWorkExpoOnly::~CWorkExpoOnly()
{
}

/*
 desc : �ʱ� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkExpoOnly::InitWork()
{
	/* ���� ��� ���� �� �ʱ�ȭ */
	CWork::InitWork();

	/* ��ü �۾� �ܰ� */
	m_u8StepTotal	= 0x0a;

	/* ���� �뱤 ���� ���� ��û */
	if (!uvEng_Luria_ReqGetExposureFactor())
	{
		AfxMessageBox(L"Failed to request the Exposure Factor", MB_ICONSTOP | MB_TOPMOST);
	}

	/* ������ �̵� */
	if (m_stExpoLog.move_acam)	SetMovingACamSide();

	return TRUE;
}

/*
 desc : �ֱ������� �۾� ����
 parm : None
 retn : None
*/
VOID CWorkExpoOnly::DoWork()
{
	/* �۾� �ܰ� ���� ���� ó�� */
	switch (m_u8StepIt)
	{
	case 0x01 : m_enWorkState = SetExposeReady(FALSE, FALSE, FALSE, m_stExpoLog.expo_count);	break;

	case 0x02 : m_enWorkState = SetPrePrinting();												break;
	case 0x03 : m_enWorkState = IsPrePrinted();													break;

	case 0x04 : m_enWorkState = SetPrinting();													break;
	case 0x05 : m_enWorkState = IsPrinted();													break;

	case 0x06 : m_enWorkState = IsMotorDriveStopAll();											break;

	case 0x07 : m_enWorkState = SetWorkWaitTime(1000);											break;
	case 0x08 : m_enWorkState = IsWorkWaitTime();												break;
	case 0x09 : m_enWorkState = SetMovingUnloader();											break;
	case 0x0a : m_enWorkState = IsMovedUnloader();												break;
	}

	/* �ּ��� 2 ȸ �̻� �ݺ� �뱤�� ��, �����ϵ��� �� */
	if (m_stExpoLog.expo_count > 1)
	{
		/* ����� ī�޶� ��/�� �̵� */
		//if (m_stExpoInfo.move_acam)
		if (m_stExpoLog.move_acam)
		{
			if (IsMovedACamSide())	SetMovingACamSide();
		}
		/* �뱤 �� ������ ��/�� �ѹ����� �̵��� �� */
		//if (m_stExpoInfo.move_ph)
		if (m_stExpoLog.move_ph)
		{
			if (0x01 == m_u8StepIt && m_enWorkState == ENG_JWNS::en_next)
			{
				if (IsMovedPhUpDown())	SetMovingPhUpDown();
			}
		}
	}

	/* ���� �۾� ���� ���� �Ǵ� */
	CWorkExpoOnly::SetWorkNext();
	/* ��ð� ���� ���� ��ġ�� �ݺ� �����Ѵٸ� ���� ó�� */
	CheckWorkTimeout();
}

/*
 desc : ���� �ܰ�� �̵��ϱ� ���� ó��
 parm : None
 retn : None
*/
VOID CWorkExpoOnly::SetWorkNext()
{
	UINT64 u64JobTime	= GetTickCount64() - m_u64StartTime;

	/* �� �۾� �������� �ð� �� ���� ó�� */
	uvEng_UpdateJobWorkTime(u64JobTime);

	/* ��� �۾��� ���� �Ǿ����� ���� */
	if (ENG_JWNS::en_error == m_enWorkState)
	{
		m_u8StepIt	= 0x01;
	}
	else if (ENG_JWNS::en_wait == m_enWorkState)
	{
		/* �۾��� ��� �� �ӽ� ���� */
		if (0x05 == m_u8StepIt)	CalcStepRate();
	}
	else if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* �۾��� ��� �� �ӽ� ���� */
		CWorkExpoOnly::CalcStepRate();
		/* �۾��� �Ϸ� �Ǿ����� ���� */
		if (m_u8StepTotal == m_u8StepIt)
		{
			/*1����Ŭ �뱤 �Ϸ�� �α� ���*/
			//SaveExpoLog(TRUE);
			/*Auto Mdoe�� �뱤 ���ᰡ �Ǹ� Philhmil�� �ϷẸ��*/
			if (g_u16PhilCommand == ePHILHMI_C2P_PROCESS_EXECUTE)
			{
				SetPhilProcessCompelet();
			}

			/* ���� �뱤 Ƚ���� ���� �ߴ��� ���� */
			//if (++m_u32ExpoCount != m_stExpoInfo.expo_count)
			if (++m_u32ExpoCount != m_stExpoLog.expo_count)
			{
				m_u8StepIt	= 0x01;

#ifdef _DEBUG
				TCHAR szMesg[LOG_MESG_SIZE] = { NULL };

				swprintf_s(szMesg, LOG_MESG_SIZE, L"Expo Only Step :  m_u32ExpoCount= %d / m_u8StepTotal = %d\n"
					, m_u32ExpoCount, m_stExpoLog.expo_count);

				/*Log ���*/
				m_strLog.Format(szMesg);
				txtWrite(m_strLog);
#endif
			}
			else
			{
				m_enWorkState	= ENG_JWNS::en_comp;
				CWork::EndWork();	/* �׻� ȣ��*/
			}
		}
		else
		{
			/* ���� �۾� �ܰ�� �̵� */
			m_u8StepIt++;
		}
		/* ���� �ֱٿ� Next ������ �ð� ���� */
		m_u64DelayTime	= GetTickCount64();
	}
	else if (ENG_JWNS::en_error == m_enWorkState)
	{
		m_enWorkState = ENG_JWNS::en_comp;
	}
}

/*
 desc : �۾� ����� ���� (percent; %)
 parm : None
 retn : None
*/
VOID CWorkExpoOnly::CalcStepRate()
{
	UINT8 u8PhCount	= uvEng_GetConfig()->luria_svc.ph_count;
	LPG_LDEW pstExpo= &uvEng_ShMem_GetLuria()->exposure;
	DOUBLE dbStep	= m_u8StepIt + pstExpo->GetScanLine() +
					  (m_u32ExpoCount * (m_u8StepTotal + pstExpo->GetScanTotal()));
	DOUBLE dbTotal	= (m_u8StepTotal + pstExpo->GetScanTotal()) * m_stExpoLog.expo_count;
	DOUBLE dbRate	= dbStep / dbTotal * 100.0f;
#if 0
	DOUBLE dbRate	= DOUBLE(m_u8StepIt + pstExpo->GetScanLine()) /
					  DOUBLE(m_u8StepTotal + pstExpo->GetScanTotal()) * 100.0f;
#endif
	if (dbRate > 100.0f)	dbRate = 100.0f;
	/* ���� �޸𸮿� ���� */
	uvEng_SetWorkStepRate(dbRate);


}

/*
 desc : ����� ī�޶� 2�븦 ��/�� ������ �̵� ��Ŵ (��� ���� �������� ����)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkExpoOnly::SetMovingACamSide()
{
	LPG_CMSI pstMC2	= &uvEng_GetConfig()->mc2_svc;

	/* ���� ��� ���� �� ��� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam2);
	/* ¦���� ��, �ּ� �� (���� ��)���� �̵� */
	if (!m_bMoveACamSide)
	{
		/* 1�� ī�޶���� �̵� */
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1,
									  pstMC2->min_dist[UINT8(ENG_MMDI::en_align_cam1)],
									  pstMC2->max_velo[UINT8(ENG_MMDI::en_align_cam1)]))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to move the align camera (1) to (left)");
			return FALSE;
		}
		/* ���� ����� ��ũ�� �̵��� ��, �浹 �߻� ������, �� 50 mm �����ְ� ������ �ε��� �Ѵ�. */
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2,
									  pstMC2->min_dist[UINT8(ENG_MMDI::en_align_cam2)]+50.0f,	 /* �ʹ� �پ� ������, ���� �̵��� ��, �浹 �߻� ������ ... */
									  pstMC2->max_velo[UINT8(ENG_MMDI::en_align_cam2)]))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to move the align camera (2) to (left)");
			return FALSE;
		}
	}
	/* Ȧ���� ��, �ִ� �� (���� ��)���� �̵� */
	else
	{
		/* 2�� ī�޶���� �̵� */
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2,
									  pstMC2->max_dist[UINT8(ENG_MMDI::en_align_cam2)],
									  pstMC2->max_velo[UINT8(ENG_MMDI::en_align_cam2)]))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to move the align camera (1) to (right)");
			return FALSE;
		}
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1,
									  pstMC2->max_dist[UINT8(ENG_MMDI::en_align_cam1)]-50.0f,	 /* �ʹ� �پ� ������, ���� �̵��� ��, �浹 �߻� ������ ... */
									  pstMC2->max_velo[UINT8(ENG_MMDI::en_align_cam1)]))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to move the align camera (2) to (right)");
			return FALSE;
		}
	}

	return TRUE;
}

/*
 desc : 2�븦 ��/�� ������ �̵��� �Ϸ� �Ǿ����� Ȯ�� (��� ���� �������� ����)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkExpoOnly::IsMovedACamSide()
{
	/* �ּ� 1 �и��� �����ϵ��� ���� �� */
	if (GetTickCount64() < (m_u64TickACamSide + 60000) /* 1 minutue */)	return FALSE;

	/* ��� �̵� �ߴ��� ���� Ȯ�� */
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1))	return FALSE;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam2))	return FALSE;

	/* �ݴ������� �̵� */
	m_bMoveACamSide	= !m_bMoveACamSide;

	/* �ֱ� ������ �ð� ���� */
	m_u64TickACamSide	= GetTickCount64();

	return TRUE;
}

/*
 desc : ���а� 2�븦 �� (Up:Top) / �� (Down:Bottom) ������ �̵� ��Ŵ
		��� ���� �������� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkExpoOnly::SetMovingPhUpDown()
{
		DOUBLE dbPhMoveZ[2]	= {NULL}, dbPhVeloZ[2] = {NULL};
	if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x01)
	{
		LPG_CLSI pstLuria	= &uvEng_GetConfig()->luria_svc;
		/* ���а� 2 ��� �� ���� �̵� ��Ŵ */
		if (!m_bMovePhUpDown)
		{
			/* �ִ� ������ �̵� ��Ŵ */
			dbPhMoveZ[0]	= pstLuria->ph_z_move_max;
			dbPhMoveZ[1]	= pstLuria->ph_z_move_max;
		}
		/* ���а� 2 ��� �� �Ʒ��� �̵� ��Ŵ */
		else
		{
			/* �ּ� ������ �̵� ��Ŵ */
			dbPhMoveZ[0]	= pstLuria->ph_z_move_min;
			dbPhMoveZ[1]	= pstLuria->ph_z_move_min;
		}
		/* ���а� Z �� 2�� ��� �̵� */
		if (!uvEng_Luria_ReqSetMotorAbsPositionAll(pstLuria->ph_count, dbPhMoveZ))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to move the photohead z axis (min/max)");
			return FALSE;
		}
	}
	else if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x03)
	{
		LPG_CMSI pstMC2	= &uvEng_GetConfig()->mc2_svc;
		/* ���а� 2 ��� �� ���� �̵� ��Ŵ */
		if (!m_bMovePhUpDown)
		{
			/* �ִ� ������ �̵� ��Ŵ */
			dbPhMoveZ[0]= pstMC2->max_dist[UINT8(ENG_MMDI::en_axis_ph1)];
			dbPhMoveZ[1]= pstMC2->max_dist[UINT8(ENG_MMDI::en_axis_ph2)];
		}
		/* ���а� 2 ��� �� �Ʒ��� �̵� ��Ŵ */
		else
		{
			/* �ּ� ������ �̵� ��Ŵ */
			dbPhMoveZ[0]= pstMC2->min_dist[UINT8(ENG_MMDI::en_axis_ph1)];
			dbPhMoveZ[1]= pstMC2->min_dist[UINT8(ENG_MMDI::en_axis_ph2)];
		}
		dbPhVeloZ[0] = pstMC2->max_velo[UINT8(ENG_MMDI::en_axis_ph1)];
		dbPhVeloZ[1] = pstMC2->max_velo[UINT8(ENG_MMDI::en_axis_ph2)];
		/* ���� ���� �� ���� */
		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_axis_ph1);
		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_axis_ph2);
		/* ���а� Z �� 2�� ��� �̵� */
		if (!(uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_axis_ph1, dbPhMoveZ[0], dbPhVeloZ[0]) &&
			  uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_axis_ph2, dbPhMoveZ[1], dbPhVeloZ[1])))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to move the photohead z axis (min/max)");
			return FALSE;
		}
	}
	/* �ݴ������� �̵� */
	m_bMovePhUpDown	= !m_bMovePhUpDown;

	return TRUE;
}

/*
 desc : ���а� Z ���� Up/Down ��ġ���� ���� �ִ��� ���� Ȯ�� (��� ���� �������� ����)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkExpoOnly::IsMovedPhUpDown()
{
	if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x01)
	{
		/* ��� �̵� �ߴ��� ���� Ȯ�� */
		if (!uvCmn_Luria_IsAllPhZAxisIdle())	return FALSE;
	}
	else if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x03)
	{
		if (!(uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_axis_ph1) &&
			  uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_axis_ph2)))	return FALSE;
	}
	return TRUE;
}


/*
 desc : �뱤 �Ϸ� �� ���� ���� ����
 parm : state	- [in]  0x00: �۾� ����, 0x01: �۾� ����
 retn : None
*/
VOID CWorkExpoOnly::SaveExpoLog(UINT8 state)
{
	TCHAR tzResult[1024] = { NULL }, tzFile[MAX_PATH_LEN] = { NULL }, tzState[2][8] = { L"FAIL", L"SUCC" };
	SYSTEMTIME stTm = { NULL };
	MEMORYSTATUSEX stMem = { NULL };

	/*������ ���� ��������*/
	CUniToChar	csCnv;
	LPG_RJAF pstJobRecipe = uvEng_JobRecipe_GetSelectRecipe();
	LPG_RAAF pstAlignRecipe = uvEng_Mark_GetAlignRecipeName(csCnv.Ansi2Uni(pstJobRecipe->align_recipe));
	LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstJobRecipe->expo_recipe));
	CUniToChar csCnv1, csCnv2;

	/* ���� ��ǻ�� ��¥�� ���ϸ����� ���� */
	GetLocalTime(&stTm);
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\logs\\expo\\%04d-%02d-%02d.csv",
		g_tzWorkDir, stTm.wYear, stTm.wMonth, stTm.wDay);
	/* ���࿡ ������ �ϴ� ������ ������, �ش� ���� ���� ��, Ÿ��Ʋ (Field) �߰� */
	if (!uvCmn_FindFile(tzFile))
	{
		wcscpy_s(tzResult, 1024,
			L"date,temp,expo_time(ms),mode,succ,real_cale,real_roation,"
			L"horz_dist_13(mm),horz_dist_24(mm),vert_dist_12(mm),vert_dist_34(mm),"
			L"diag_dist_14(mm),diag_dist_23(mm),"
			L"gerber,thick,energy,align_type,mark_type,lamp_type,gain_leven[0],gain_leven[1],"
			L"power_name,duty_cycle,\n");
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x00);
	}

	/* �߻� �ð� */
	swprintf_s(tzResult, 1024, L"%02d:%02d:%02d,", stTm.wHour, stTm.wMinute, stTm.wSecond);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	/*ExpoLog ���*/
	memcpy(m_stExpoLog.data, tzResult, 40);


	/* �µ� / �뱤 �ð� / �뱤 ���� / �뱤 ��� / ���� ���� / ���� ȸ�� */
	swprintf_s(tzResult, 1024, L"\%llu,%s,%.2f,%.2f,",
		uvEng_GetJobWorkTime(), tzState[state], pstExpoRecipe->real_scale_range, pstExpoRecipe->real_rotaion_range);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	/*ExpoLog ���*/
	m_stExpoLog.expo_time = uvEng_GetJobWorkTime();
	m_stExpoLog.expo_succ = state;
	m_stExpoLog.real_scale = pstExpoRecipe->real_scale_range;
	m_stExpoLog.real_rotaion = pstExpoRecipe->real_rotaion_range;
	

	/* ��ũ ���� 6 �� ���� ���� ���� �� �� ��ü �뱤�ϴµ� �ҿ�� �ð� ���� */
	swprintf_s(tzResult, 1024, L"%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,",
		uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_top_horz),
		uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_btm_horz),
		uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_lft_vert),
		uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_rgt_vert),
		uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_lft_diag),
		uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_rgt_diag));
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	/*ExpoLog ���*/
	m_stExpoLog.global_dist[0] = uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_top_horz);
	m_stExpoLog.global_dist[1] = uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_btm_horz);
	m_stExpoLog.global_dist[2] = uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_lft_vert);
	m_stExpoLog.global_dist[3] = uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_rgt_vert);
	m_stExpoLog.global_dist[4] = uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_lft_diag);
	m_stExpoLog.global_dist[5] = uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_rgt_diag);


	/* �Ź� / ����β� / ���� / ����� Ÿ�� / ��ũ Ÿ�� / ���� Ÿ�� / 1�� ī�޶� ���� / 2�� ī�޶� ���� */
	TCHAR tzAlignType[7][40] = { L"Global 0 Local 0_0"	, L"Global 4 Local 0_0" , L"Global 4 Local 2_1" , L"Global 4 Local 2_2"
														, L"Global 4 Local 3_2" , L"Global 4 Local 4_2" , L"Global 4 Local 5_2" };
	TCHAR tzMarkType[2][20] = { L"Geomatrix", L"Pattern Image" };
	TCHAR tzLampType[2][8] = { L"Ring", L"Coaxial" };

 
	swprintf_s(tzResult, 1024, L"%s,%d,%.2f,%s,%s,%s,%d,%d,",
		csCnv1.Ansi2Uni(pstJobRecipe->gerber_name), pstJobRecipe->material_thick, pstJobRecipe->expo_energy, tzAlignType[pstAlignRecipe->align_type],
		tzMarkType[pstAlignRecipe->mark_type], tzLampType[pstAlignRecipe->lamp_type], pstAlignRecipe->gain_level[0], pstAlignRecipe->gain_level[1]);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
 
 
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);

	/*ExpoLog ���*/
	memcpy(m_stExpoLog.gerber_name, pstJobRecipe->gerber_name, MAX_GERBER_NAME);
	m_stExpoLog.material_thick	= pstJobRecipe->material_thick;
	m_stExpoLog.expo_energy		= pstJobRecipe->expo_energy;

	m_stExpoLog.align_type		= pstAlignRecipe->align_type;

	m_stExpoLog.mark_type		= pstAlignRecipe->mark_type;
	m_stExpoLog.lamp_type		= pstAlignRecipe->lamp_type;
	m_stExpoLog.gain_level[0]	= pstAlignRecipe->gain_level[0];
	m_stExpoLog.gain_level[1]	= pstAlignRecipe->gain_level[1];

	m_stExpoLog.led_duty_cycle = pstExpoRecipe->led_duty_cycle;
	sprintf_s(m_stExpoLog.power_name, LED_POWER_NAME_LENGTH, pstExpoRecipe->power_name);

	/* �������� ������ ���� �������� �Ѿ���� �ϱ� ���� */
	uvCmn_SaveTxtFileW(L"\n", (UINT32)wcslen(L"\n"), tzFile, 0x01);
}


VOID CWorkExpoOnly::SetPhilProcessCompelet()
{
	STG_PP_P2C_PROCESS_COMP			stProcessComp;
	STG_PP_P2C_PROCESS_COMP_ACK		stProcessCompAck;
	stProcessComp.Reset();
	stProcessCompAck.Reset();

	/*Process Execute���� ���� ����*/
	sprintf_s(stProcessComp.szRecipeName, DEF_MAX_RECIPE_NAME_LENGTH, m_stExpoLog.recipe_name);
	sprintf_s(stProcessComp.szGlassID, DEF_MAX_GLASS_NAME_LENGTH, m_stExpoLog.glassID);

	/*�뱤 ��� �Ķ���Ͱ�*/
	stProcessComp.usCount = 15;
	sprintf_s(stProcessComp.stVar[0].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "CHAR");
	sprintf_s(stProcessComp.stVar[0].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Data");
	sprintf_s(stProcessComp.stVar[0].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%S", m_stExpoLog.data);

	sprintf_s(stProcessComp.stVar[1].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "UINT64");
	sprintf_s(stProcessComp.stVar[1].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Time");
	sprintf_s(stProcessComp.stVar[1].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%lld", m_stExpoLog.expo_time);

	sprintf_s(stProcessComp.stVar[2].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "UINT8");
	sprintf_s(stProcessComp.stVar[2].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Mode");
	sprintf_s(stProcessComp.stVar[2].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "Direct");

	sprintf_s(stProcessComp.stVar[3].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "UINT8");
	sprintf_s(stProcessComp.stVar[3].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Succ");
	sprintf_s(stProcessComp.stVar[3].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.expo_succ);

	sprintf_s(stProcessComp.stVar[4].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "UINT32");
	sprintf_s(stProcessComp.stVar[4].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Real Scale");
	sprintf_s(stProcessComp.stVar[4].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%.1f", m_stExpoLog.real_scale);

	sprintf_s(stProcessComp.stVar[5].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "INT32");
	sprintf_s(stProcessComp.stVar[5].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Real Rotain");
	sprintf_s(stProcessComp.stVar[5].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%.1f", m_stExpoLog.real_rotaion);

	sprintf_s(stProcessComp.stVar[6].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "PCHAR");
	sprintf_s(stProcessComp.stVar[6].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Gerber Name");
	sprintf_s(stProcessComp.stVar[6].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%S", m_stExpoLog.gerber_name);

	sprintf_s(stProcessComp.stVar[7].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "UINT32");
	sprintf_s(stProcessComp.stVar[7].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Material Thick");
	sprintf_s(stProcessComp.stVar[7].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.material_thick);

	sprintf_s(stProcessComp.stVar[8].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "float");
	sprintf_s(stProcessComp.stVar[8].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Expo Energy");
	sprintf_s(stProcessComp.stVar[8].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, " %.1f", m_stExpoLog.expo_energy);

	sprintf_s(stProcessComp.stVar[9].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "UINT8");
	sprintf_s(stProcessComp.stVar[9].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Align Type");
	sprintf_s(stProcessComp.stVar[9].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.align_type);

	sprintf_s(stProcessComp.stVar[10].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "UINT8");
	sprintf_s(stProcessComp.stVar[10].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Mark Type");
	sprintf_s(stProcessComp.stVar[10].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.mark_type);

	sprintf_s(stProcessComp.stVar[11].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "UINT8");
	sprintf_s(stProcessComp.stVar[11].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Lamp Type");
	sprintf_s(stProcessComp.stVar[11].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.lamp_type);

	sprintf_s(stProcessComp.stVar[12].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "UINT8");
	sprintf_s(stProcessComp.stVar[12].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Gain Level1");
	sprintf_s(stProcessComp.stVar[12].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.gain_level[0]);

	sprintf_s(stProcessComp.stVar[13].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "UINT8");
	sprintf_s(stProcessComp.stVar[13].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Gain Level2");
	sprintf_s(stProcessComp.stVar[13].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.gain_level[1]);

	sprintf_s(stProcessComp.stVar[14].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "PCHAR");
	sprintf_s(stProcessComp.stVar[14].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Power Name");
	sprintf_s(stProcessComp.stVar[14].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%S", m_stExpoLog.power_name);

	sprintf_s(stProcessComp.stVar[15].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "UINT8");
	sprintf_s(stProcessComp.stVar[15].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Led Duty Cycle");
	sprintf_s(stProcessComp.stVar[15].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.led_duty_cycle);

	if (!m_stExpoLog.expo_succ)
	{
		stProcessComp.usErrorCode = ePHILHMI_ERR_STATUS_COMPLETE;
	}

	uvEng_Philhmi_Send_P2C_PROCESS_COMP(stProcessComp, stProcessCompAck);
}

VOID CWorkExpoOnly::txtWrite(CString msg)
{
	CStdioFile	imsifile;

	TCHAR tzFile[_MAX_PATH] = { NULL };
	SYSTEMTIME stTm = { NULL };

	GetLocalTime(&stTm);

	TCHAR tzPath[_MAX_PATH];
	swprintf_s(tzPath, _MAX_PATH, L"%s\\logs\\expo\\%04d-%02d-%02d ExpoOnly.dat", g_tzWorkDir, stTm.wYear, stTm.wMonth, stTm.wDay);


	imsifile.Open(tzPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite);

	imsifile.SeekToEnd();

	swprintf_s(tzFile, L"[%04d-%02d-%02d %02d:%02d:%02d.%03d]  ",
		(UINT16)stTm.wYear, (UINT16)stTm.wMonth, (UINT16)stTm.wDay,
		(UINT16)stTm.wHour, (UINT16)stTm.wMinute, (UINT16)stTm.wSecond, (UINT16)stTm.wMilliseconds);

	imsifile.WriteString(tzFile);
	imsifile.WriteString(msg);

	imsifile.Close();

}
