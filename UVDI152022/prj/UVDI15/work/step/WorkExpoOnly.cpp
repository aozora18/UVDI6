
/*
 desc : Expose - Only Direct
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkExpoOnly.h"
#include "../../GlobalVariables.h"

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 생성자
 parm : expo	- [in]  노광 정보
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
	useAFtemp = GlobalVariables::GetInstance()->GetAutofocus().GetUseAF();
	//memcpy(&m_stExpoInfo, expo, sizeof(STG_CPHE));
	
	m_stExpoLog.Init();

	if(expo != nullptr)
		memcpy(&m_stExpoLog, expo, sizeof(STG_CELA));

}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CWorkExpoOnly::~CWorkExpoOnly()
{
}

/*
 desc : 초기 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkExpoOnly::InitWork()
{
	/* 내부 멤버 변수 값 초기화 */
	CWork::InitWork();

	/* 전체 작업 단계 */
	m_u8StepTotal	= 0x0a;

	/* 현재 노광 조건 정보 요청 */
	if (!uvEng_Luria_ReqGetExposureFactor())
	{
		AfxMessageBox(L"Failed to request the Exposure Factor", MB_ICONSTOP | MB_TOPMOST);
	}

	/* 무조건 이동 */
	if (m_stExpoLog.move_acam)	SetMovingACamSide();

	return TRUE;
}

/*
 desc : 주기적으로 작업 수행
 parm : None
 retn : None
*/
VOID CWorkExpoOnly::DoWork()
{
	/* 작업 단계 별로 동작 처리 */
	switch (m_u8StepIt)
	{
	case 0x01:
	{
		DOUBLE* pStartXY = uvEng_GetConfig()->luria_svc.table_expo_start_xy[0];

		if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_x, pStartXY[0]) ||
			CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, pStartXY[1]))
		{
			m_enWorkState = ENG_JWNS::en_error;
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Interlock error. Failed to move the expose position");
		}
		else
			m_enWorkState = SetExposeReady(FALSE, FALSE, FALSE, m_stExpoLog.expo_count);
	}
	break;

	case 0x02: m_enWorkState = SetPrePrinting();												break;
	case 0x03: m_enWorkState = IsPrePrinted();													break;

	case 0x04: m_enWorkState = SetPrinting();													break;
	case 0x05: m_enWorkState = IsPrinted();													break;

	case 0x06: m_enWorkState = IsMotorDriveStopAll();											break;

	case 0x07: m_enWorkState = SetWorkWaitTime(1000);											break;
	case 0x08: m_enWorkState = IsWorkWaitTime();												break;
	case 0x09:
	{
		if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_x, uvEng_GetConfig()->set_align.table_unloader_xy[0][0]) ||
			CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, uvEng_GetConfig()->set_align.table_unloader_xy[0][1]))
		{
			m_enWorkState = ENG_JWNS::en_error;
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Interlock error. Failed to move the expose position");
		}
		else

			m_enWorkState = SetMovingUnloader();

	}
	break;
	case 0x0a: m_enWorkState = IsMovedUnloader();												break;
	}

	/* 최소한 2 회 이상 반복 노광할 때, 동작하도록 함 */
	if (m_stExpoLog.expo_count > 1)
	{
		/* 얼라인 카메라 좌/우 이동 */
		//if (m_stExpoInfo.move_acam)
		if (m_stExpoLog.move_acam)
		{
			if (IsMovedACamSide())	SetMovingACamSide();
		}
		/* 노광 할 때마다 상/하 한번씩만 이동해 줌 */
		//if (m_stExpoInfo.move_ph)
		if (m_stExpoLog.move_ph)
		{
			if (0x01 == m_u8StepIt && m_enWorkState == ENG_JWNS::en_next)
			{
				if (IsMovedPhUpDown())	SetMovingPhUpDown();
			}
		}
	}

	/* 다음 작업 진행 여부 판단 */
	CWorkExpoOnly::SetWorkNext();
	/* 장시간 동안 동일 위치를 반복 수행한다면 에러 처리 */
	CheckWorkTimeout();
}



/*
 desc : 다음 단계로 이동하기 위한 처리
 parm : None
 retn : None
*/
VOID CWorkExpoOnly::SetWorkNext()
{
	UINT64 u64JobTime	= GetTickCount64() - m_u64StartTime;

	/* 매 작업 구간마다 시간 값 증가 처리 */
	uvEng_UpdateJobWorkTime(u64JobTime);


	if (GetAbort())
	{
		GlobalVariables::GetInstance()->GetAutofocus().ClearAFActivate();
		CWork::EndWork();
		return;
	}

	/* 모든 작업이 종료 되었는지 여부 */
	if (ENG_JWNS::en_error == m_enWorkState)
	{
		m_u8StepIt	= 0x01;
	}
	else if (ENG_JWNS::en_wait == m_enWorkState)
	{
		/* 작업률 계산 후 임시 저장 */
		if (0x05 == m_u8StepIt)	CalcStepRate();
	}
	else if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* 작업률 계산 후 임시 저장 */
		CWorkExpoOnly::CalcStepRate();
		/* 작업이 완료 되었는지 여부 */
		if (m_u8StepTotal == m_u8StepIt)
		{
			/*1싸이클 노광 완료시 로그 기록*/
			SaveExpoLog(TRUE);

			/* 현재 노광 횟수에 도달 했는지 여부 */
			//if (++m_u32ExpoCount != m_stExpoInfo.expo_count)
			if (++m_u32ExpoCount != m_stExpoLog.expo_count)
			{
				m_u8StepIt	= 0x01;

#ifdef _DEBUG
				TCHAR szMesg[LOG_MESG_SIZE] = { NULL };

				swprintf_s(szMesg, LOG_MESG_SIZE, L"Expo Only Step :  m_u32ExpoCount= %d / m_u8StepTotal = %d\n"
					, m_u32ExpoCount, m_stExpoLog.expo_count);

				/*Log 기록*/
				m_strLog.Format(szMesg);
				txtWrite(m_strLog);
#endif
			}
			else
			{
				/*노광 종료가 되면 Philhmil에 완료보고*/
				SetPhilProcessCompelet();
				GlobalVariables::GetInstance()->GetAutofocus().ClearAFActivate();
				m_enWorkState	= ENG_JWNS::en_comp;
				CWork::EndWork();	/* 항상 호출*/
			}
		}
		else
		{
			/* 다음 작업 단계로 이동 */
			m_u8StepIt++;
		}
		/* 가장 최근에 Next 수행한 시간 저장 */
		m_u64DelayTime	= GetTickCount64();
	}
	else if (ENG_JWNS::en_error == m_enWorkState)
	{
		SaveExpoLog(TRUE);
		GlobalVariables::GetInstance()->GetAutofocus().ClearAFActivate();
		/*노광 종료가 되면 Philhmil에 완료보고*/
		SetPhilProcessCompelet();

		m_enWorkState = ENG_JWNS::en_comp;
	}
}

/*
 desc : 작업 진행률 설정 (percent; %)
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
	/* 전역 메모리에 설정 */
	uvEng_SetWorkStepRate(dbRate);


}

/*
 desc : 얼라인 카메라 2대를 좌/우 끝으로 이동 시킴 (장비 예열 목적으로 사용됨)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkExpoOnly::SetMovingACamSide()
{
	LPG_CMSI pstMC2	= &uvEng_GetConfig()->mc2_svc;

	/* 현재 토글 상태 값 얻기 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam2);
	/* 짝수일 땐, 최소 값 (좌측 끝)으로 이동 */
	if (!m_bMoveACamSide)
	{
		/* 1번 카메라부터 이동 */
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1,
									  pstMC2->min_dist[UINT8(ENG_MMDI::en_align_cam1)],
									  pstMC2->max_velo[UINT8(ENG_MMDI::en_align_cam1)]))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to move the align camera (1) to (left)");
			return FALSE;
		}
		/* 추후 얼라인 마크로 이동할 때, 충돌 발생 때문에, 약 50 mm 여유있게 간격을 두도록 한다. */
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2,
									  pstMC2->min_dist[UINT8(ENG_MMDI::en_align_cam2)]+50.0f,	 /* 너무 붙어 있으면, 추후 이동할 때, 충돌 발생 때문에 ... */
									  pstMC2->max_velo[UINT8(ENG_MMDI::en_align_cam2)]))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to move the align camera (2) to (left)");
			return FALSE;
		}
	}
	/* 홀수일 때, 최대 값 (우측 끝)으로 이동 */
	else
	{
		/* 2번 카메라부터 이동 */
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2,
									  pstMC2->max_dist[UINT8(ENG_MMDI::en_align_cam2)],
									  pstMC2->max_velo[UINT8(ENG_MMDI::en_align_cam2)]))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to move the align camera (1) to (right)");
			return FALSE;
		}
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1,
									  pstMC2->max_dist[UINT8(ENG_MMDI::en_align_cam1)]-50.0f,	 /* 너무 붙어 있으면, 추후 이동할 때, 충돌 발생 때문에 ... */
									  pstMC2->max_velo[UINT8(ENG_MMDI::en_align_cam1)]))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to move the align camera (2) to (right)");
			return FALSE;
		}
	}

	return TRUE;
}

/*
 desc : 2대를 좌/우 끝으로 이동이 완료 되었는지 확인 (장비 예열 목적으로 사용됨)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkExpoOnly::IsMovedACamSide()
{
	/* 최소 1 분마다 동작하도록 설정 함 */
	if (GetTickCount64() < (m_u64TickACamSide + 60000) /* 1 minutue */)	return FALSE;

	/* 모두 이동 했는지 여부 확인 */
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1))	return FALSE;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam2))	return FALSE;

	/* 반대편으로 이동 */
	m_bMoveACamSide	= !m_bMoveACamSide;

	/* 최근 동작한 시간 저장 */
	m_u64TickACamSide	= GetTickCount64();

	return TRUE;
}

/*
 desc : 광학계 2대를 상 (Up:Top) / 하 (Down:Bottom) 끝으로 이동 시킴
		장비 예열 목적으로 사용됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkExpoOnly::SetMovingPhUpDown()
{
		DOUBLE dbPhMoveZ[2]	= {NULL}, dbPhVeloZ[2] = {NULL};
	if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x01)
	{
		LPG_CLSI pstLuria	= &uvEng_GetConfig()->luria_svc;
		/* 광학계 2 대로 맨 위로 이동 시킴 */
		if (!m_bMovePhUpDown)
		{
			/* 최대 값으로 이동 시킴 */
			dbPhMoveZ[0]	= pstLuria->ph_z_move_max;
			dbPhMoveZ[1]	= pstLuria->ph_z_move_max;
		}
		/* 광학계 2 대로 맨 아래로 이동 시킴 */
		else
		{
			/* 최소 값으로 이동 시킴 */
			dbPhMoveZ[0]	= pstLuria->ph_z_move_min;
			dbPhMoveZ[1]	= pstLuria->ph_z_move_min;
		}
		/* 광학계 Z 축 2대 모두 이동 */
		if (!uvEng_Luria_ReqSetMotorAbsPositionAll(pstLuria->ph_count, dbPhMoveZ))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to move the photohead z axis (min/max)");
			return FALSE;
		}
	}
	else if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x03)
	{
		LPG_CMSI pstMC2	= &uvEng_GetConfig()->mc2_svc;
		/* 광학계 2 대로 맨 위로 이동 시킴 */
		if (!m_bMovePhUpDown)
		{
			/* 최대 값으로 이동 시킴 */
			dbPhMoveZ[0]= pstMC2->max_dist[UINT8(ENG_MMDI::en_axis_ph1)];
			dbPhMoveZ[1]= pstMC2->max_dist[UINT8(ENG_MMDI::en_axis_ph2)];
		}
		/* 광학계 2 대로 맨 아래로 이동 시킴 */
		else
		{
			/* 최소 값으로 이동 시킴 */
			dbPhMoveZ[0]= pstMC2->min_dist[UINT8(ENG_MMDI::en_axis_ph1)];
			dbPhMoveZ[1]= pstMC2->min_dist[UINT8(ENG_MMDI::en_axis_ph2)];
		}
		dbPhVeloZ[0] = pstMC2->max_velo[UINT8(ENG_MMDI::en_axis_ph1)];
		dbPhVeloZ[1] = pstMC2->max_velo[UINT8(ENG_MMDI::en_axis_ph2)];
		/* 이전 상태 값 저장 */
		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_axis_ph1);
		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_axis_ph2);
		/* 광학계 Z 축 2대 모두 이동 */
		if (!(uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_axis_ph1, dbPhMoveZ[0], dbPhVeloZ[0]) &&
			  uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_axis_ph2, dbPhMoveZ[1], dbPhVeloZ[1])))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to move the photohead z axis (min/max)");
			return FALSE;
		}
	}
	/* 반대편으로 이동 */
	m_bMovePhUpDown	= !m_bMovePhUpDown;

	return TRUE;
}

/*
 desc : 광학계 Z 축이 Up/Down 위치에서 멈춰 있는지 여부 확인 (장비 예열 목적으로 사용됨)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkExpoOnly::IsMovedPhUpDown()
{
	if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x01)
	{
		/* 모두 이동 했는지 여부 확인 */
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
 desc : 노광 완료 후 각종 정보 저장
 parm : state	- [in]  0x00: 작업 실패, 0x01: 작업 성공
 retn : None
*/
VOID CWorkExpoOnly::SaveExpoLog(UINT8 state)
{
	//UINT8 i;
	TCHAR tzResult[1024] = { NULL }, tzFile[MAX_PATH_LEN] = { NULL }, tzState[2][8] = { L"FAIL", L"SUCC" };
	TCHAR tzDrv[8] = { NULL };

	SYSTEMTIME stTm = { NULL };
	MEMORYSTATUSEX stMem = { NULL };
	LPG_ACGR pstMark = NULL;
	bool isLocalSelRecipe = uvEng_JobRecipe_WhatLastSelectIsLocal();
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe(isLocalSelRecipe);
	CUniToChar csCnv1, csCnv2;

	CUniToChar	csCnv;
	LPG_RAAF pstAlignRecipe = uvEng_Mark_GetAlignRecipeName(csCnv.Ansi2Uni(pstRecipe->align_recipe));
	LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstRecipe->expo_recipe));

	/* 현재 컴퓨터 날짜를 파일명으로 설정 */
	GetLocalTime(&stTm);
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\logs\\expo\\%04d-%02d-%02d ExpoOnly.csv",
		g_tzWorkDir, stTm.wYear, stTm.wMonth, stTm.wDay);

	/* 만약에 열고자 하는 파일이 없으면, 해당 파일 생성 후, 타이틀 (Field) 추가 */
	if (!uvCmn_FindFile(tzFile))
	{
		wcscpy_s(tzResult, 1024,
			L"time,tack,succ,"
			L"gerber_name,material_thick(um),expo_eneray(mj),"
			L"horz_dist_13(mm),horz_dist_24(mm),vert_dist_12(mm),vert_dist_34(mm),"
			L"diag_dist_14(mm),diag_dist_23(mm),"
			L"led_recipe,materialType\n");

		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x00);


	}

	/* 발생 시간 */
	swprintf_s(tzResult, 1024, L"%02d:%02d:%02d,", stTm.wHour, stTm.wMinute, stTm.wSecond);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);

	/*ExpoLog 기록*/
	memcpy(m_stExpoLog.data, tzResult, 40);

	UINT64 u64JobTime = uvEng_GetJobWorkTime();
	swprintf_s(tzResult, 1024, L"%um%02us,%s,%S,%dum,%.1fmj,",
		uvCmn_GetTimeToType(u64JobTime, 0x01),
		uvCmn_GetTimeToType(u64JobTime, 0x02),
		tzState[state],
		pstRecipe->gerber_name,
		pstRecipe->material_thick,
		pstRecipe->expo_energy);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);

	/*ExpoLog 기록*/
	m_stExpoLog.expo_time = uvEng_GetJobWorkTime();
	m_stExpoLog.expo_succ = state;
	m_stExpoLog.real_scale = pstExpoRecipe->real_scale_range;
	m_stExpoLog.real_rotaion = pstExpoRecipe->real_rotaion_range;


	/* 마크 간의 6 곳 길이 측정 오차 값 과 전체 노광하는데 소요된 시간 저장 */
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



	swprintf_s(tzResult, 1024, L"%S,", pstExpoRecipe->power_name);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);


	Headoffset offset;

	//	LPG_REAF pstRecipeExpo = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstRecipe->expo_recipe));
	bool getOffset = uvEng_GetConfig()->headOffsets.GetOffsets(pstExpoRecipe->headOffset, offset);
	USES_CONVERSION;

	swprintf_s(tzResult, 1024, L"%s,\n", getOffset ? A2T(offset.offsetName) : L"-");
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	

	strcpy_s(m_stExpoLog.gerber_name, MAX_GERBER_NAME, pstRecipe->gerber_name);
	m_stExpoLog.material_thick = pstRecipe->material_thick;
	m_stExpoLog.expo_energy = pstRecipe->expo_energy;

	m_stExpoLog.align_type = pstAlignRecipe->align_type;
#ifdef USE_ALIGNMOTION
	m_stExpoLog.align_motion = pstAlignRecipe->align_motion;
#endif
	m_stExpoLog.headOffset = pstExpoRecipe->headOffset;
	m_stExpoLog.mark_type = pstAlignRecipe->mark_type;
	m_stExpoLog.lamp_type = pstAlignRecipe->lamp_type;
	m_stExpoLog.gain_level[0] = pstAlignRecipe->gain_level[0];
	m_stExpoLog.gain_level[1] = pstAlignRecipe->gain_level[1];

	m_stExpoLog.led_duty_cycle = pstExpoRecipe->led_duty_cycle;
	//sprintf_s(m_stExpoLog.power_name, LED_POWER_NAME_LENGTH, pstExpoRecipe->power_name);
	strcpy_s(m_stExpoLog.power_name, LED_POWER_NAME_LENGTH, pstExpoRecipe->power_name);

	/* 마지막엔 무조건 다음 라인으로 넘어가도록 하기 위함 */
	

}


VOID CWorkExpoOnly::SetPhilProcessCompelet()
{
	STG_PP_P2C_PROCESS_COMP			stProcessComp;
	STG_PP_P2C_PROCESS_COMP_ACK		stProcessCompAck;
	stProcessComp.Reset();
	stProcessCompAck.Reset();

	/*Process Execute으로 받은 정보*/
	sprintf_s(stProcessComp.szRecipeName, DEF_MAX_RECIPE_NAME_LENGTH, m_stExpoLog.recipe_name);
	sprintf_s(stProcessComp.szGlassID, DEF_MAX_GLASS_NAME_LENGTH, m_stExpoLog.glassID);

	stProcessComp.usCount = 15;
	stProcessComp.usProgress = 1; // DONE
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
	sprintf_s(stProcessComp.stVar[4].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.global_dist[0]);

	sprintf_s(stProcessComp.stVar[5].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stProcessComp.stVar[5].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "GrabDist_btm_horz");
	sprintf_s(stProcessComp.stVar[5].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.global_dist[0]);

	sprintf_s(stProcessComp.stVar[6].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stProcessComp.stVar[6].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "GrabDist_lft_vert");
	sprintf_s(stProcessComp.stVar[6].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.global_dist[0]);

	sprintf_s(stProcessComp.stVar[7].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stProcessComp.stVar[7].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "GrabDist_rgt_vert");
	sprintf_s(stProcessComp.stVar[7].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.global_dist[0]);

	sprintf_s(stProcessComp.stVar[8].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stProcessComp.stVar[8].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "GrabDist_lft_diag");
	sprintf_s(stProcessComp.stVar[8].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.global_dist[0]);

	sprintf_s(stProcessComp.stVar[9].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stProcessComp.stVar[9].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "GrabDist_rgt_diag");
	sprintf_s(stProcessComp.stVar[9].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.global_dist[0]);

	sprintf_s(stProcessComp.stVar[10].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stProcessComp.stVar[10].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Gerber_Name");
	sprintf_s(stProcessComp.stVar[10].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%s", m_stExpoLog.gerber_name);

	sprintf_s(stProcessComp.stVar[11].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
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
		stProcessComp.usProgress = 2;
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
