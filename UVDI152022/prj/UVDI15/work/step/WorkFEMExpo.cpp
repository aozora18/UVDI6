
/*
 desc : 노광에 필요한 전체 레시피 적재
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkFEMExpo.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 생성자
 parm : offset	- [in]  직접 노광할 때, 강제로 적용될 단차 값 (Pixel)
						다만, 0xff이면, 기존 환경 파일에 설정된 단차 값 그대로 노광
						Hysterisys의 Negative Pixel 값
 retn : None
*/
CWorkFEMExpo::CWorkFEMExpo(HWND hHwnd)
	: CWorkStep()
{
	m_enWorkJobID	= ENG_BWOK::en_gerb_expofem;
	m_hHwnd = hHwnd;
	InitParameter();
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CWorkFEMExpo::~CWorkFEMExpo()
{
}

/*
 desc : 초기 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkFEMExpo::InitWork()
{
	/* 내부 멤버 변수 값 초기화 */
	if (!CWork::InitWork())	return FALSE;

	/* 전체 작업 단계 */
	m_u8StepTotal = 0x20;

	return TRUE;
}

/*
 desc : 주기적으로 작업 수행
 parm : None
 retn : None
*/
VOID CWorkFEMExpo::DoWork()
{
	/* 작업 단계 별로 동작 처리 */
	switch (m_u8StepIt)
	{
	////////////////////
	/*FEM Setting 동작*/
	///////////////////
	case 0x01: m_enWorkState = CheckValidRecipe();				break;
	/* Exposure Start XY */
	case 0x02: m_enWorkState = SetExposeStartXY();				break;
	case 0x03: m_enWorkState = IsExposeStartXY();				break;
	/* Photohead Z Axis Up & Down */
	case 0x04: m_enWorkState = SetPhZAxisMovingAll();			break;
	case 0x05: m_enWorkState = IsPhZAxisMovedAll();				break;
	/* Led Duty Cycle & Frame Rate */
	case 0x06: m_enWorkState = SetStepDutyFrame();				break;
	case 0x07: m_enWorkState = IsStepDutyFrame();				break;

	/////////////
	/*노광 동작*/
	/////////////
	//case 0x08: m_enWorkState = SetExposeReady(FALSE, FALSE, FALSE, m_stExpoInfo.expo_count);	break;

	case 0x08: m_enWorkState = SetPrePrinting();				break;
	case 0x09: m_enWorkState = IsPrePrinted();					break;

	case 0x0a: m_enWorkState = SetPrinting();					break;
	case 0x0b: m_enWorkState = IsPrinted();						break;

	case 0x0c: m_enWorkState = IsMotorDriveStopAll();			break;

	case 0x0d: m_enWorkState = SetWorkWaitTime(1000);			break;
	case 0x0e: m_enWorkState = IsWorkWaitTime();				break;

	case 0x0f: m_enWorkState = CheckMeasCount();				break;

	}

	/* 다음 작업 진행 여부 판단 */
	CWorkFEMExpo::SetWorkNext();
	/* 장시간 동안 동일 위치를 반복 수행한다면 에러 처리 */
	CWork::CheckWorkTimeout();
}


/*
 desc : 환경 파일에 적용된 파라미터를 Work Data에 적용
 parm : None
 retn : None
*/
VOID CWorkFEMExpo::InitParameter()
{
	LPG_CPFI pstPhFocs = &uvEng_GetConfig()->ph_focus;

	m_u8PrintType = pstPhFocs->print_type;
	m_u8TotalX = (UINT8)pstPhFocs->step_x_count;		/*X축 총 노광 횟수*/
	m_u8TotalY = (UINT8)pstPhFocs->step_y_count;		/*Y축 총 노광 횟수*/
	m_u8PritCntX = 1;									/*X축 현재 노광 횟수*/
	m_u8PritCntY = 1;									/*Y축 현재 노광 횟수*/
	m_dPerioX = pstPhFocs->step_move_x;					/*X축 노광 간격*/
	m_dPerioY = pstPhFocs->step_move_y;					/*Y축 노광 간격*/

	for (int i = 0; i < MAX_PH; i++)
	{
		m_dStartFocus[i] = pstPhFocs->start_foucs[i];	/*포커스 시작 위치*/
	}
	m_dStartEnergy = pstPhFocs->start_energy;			/*광량 시작 위치*/
	m_dStepFocus = pstPhFocs->step_foucs;				/*포커스 증가값*/
	m_dStepEnergy = pstPhFocs->step_energy;				/*광량 증가값*/

	m_dbStartX = uvEng_GetConfig()->luria_svc.table_expo_start_xy[0][0];		/*Stage 노광 시작 X 위치 (단위: mm)*/
	m_dbStartY = uvEng_GetConfig()->luria_svc.table_expo_start_xy[0][1];		/*Stage 노광 시작 Y 위치 (단위: mm)*/
}


/*
 desc : 환경 파일에 적용되어 있었던 파라미터 값을 원복
 parm : None
 retn : None
*/
VOID CWorkFEMExpo::ResetParameter()
{
	uvEng_GetConfig()->luria_svc.table_expo_start_xy[0][0] = m_dbStartX;		/*Stage 노광 시작 X 위치 (단위: mm)*/
	uvEng_GetConfig()->luria_svc.table_expo_start_xy[0][1] = m_dbStartY;		/*Stage 노광 시작 Y 위치 (단위: mm)*/
	uvEng_SaveConfig();
}


/*
 desc : 다음 단계로 이동하기 위한 처리22
 parm : None
 retn : None
*/
VOID CWorkFEMExpo::SetWorkNext()
{
	UINT64 u64JobTime	= GetTickCount64() - m_u64StartTime;

	/* 매 작업 구간마다 시간 값 증가 처리 */
	uvEng_UpdateJobWorkTime(u64JobTime);

	if (GetAbort())
	{
		CWork::EndWork();
		return;
	}

	if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* 작업률 계산 후 임시 저장 */
		CWorkFEMExpo::CalcStepRate();

		if (m_u8StepIt == m_u8StepTotal)
		{
			m_enWorkState = ENG_JWNS::en_comp;
			/* 초기 Luria Error 값 초기화 */
			uvCmn_Luria_ResetLastErrorStatus();

			/* 항상 호출*/
			CWork::EndWork();
		}
		else
		{
		}
		/* 다음 작업을 처리하기 위함 */
		m_u8StepIt++;
		m_u64DelayTime	= GetTickCount64();
	}
	else if (ENG_JWNS::en_error == m_enWorkState)
	{
		ResetParameter();

		m_u8StepIt = 0x01;
		m_enWorkState = ENG_JWNS::en_comp;
	}
// 	else if (ENG_JWNS::en_comp == m_enWorkState)
// 	{
// 		m_u8StepIt = 0x00;
// 	}

	/*1 싸이클 완료 후 전체 횟수 확인*/
	if (m_u8StepIt == 0x20)
	{
		ResetParameter();
		AfxMessageBox(L"FEM Expose Complete", MB_OK);
	}

}

/*
 desc : 작업 진행률 설정 (percent; %)
 parm : None
 retn : None
*/
VOID CWorkFEMExpo::CalcStepRate()
{
	UINT8 u8PhCount	= uvEng_GetConfig()->luria_svc.ph_count;
	//DOUBLE dbRate	= DOUBLE(m_u8StepIt + m_u8SetPhNo) / DOUBLE(m_u8StepTotal + u8PhCount) * 100.0f;
	DOUBLE dbRate = DOUBLE(m_u8StepIt) / DOUBLE(m_u8StepTotal) * 100.0f;
	if (dbRate > 100.0f)	dbRate = 100.0f;
	/* 전역 메모리에 설정 */
	uvEng_SetWorkStepRate(dbRate);
}


/*
 desc : 노광 시작 위치 설정
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkFEMExpo::SetExposeStartXY()
{
	DOUBLE dArrStartXY[2];
	LPG_LDMC pstMachine = &uvEng_ShMem_GetLuria()->machine;

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Expose.Start.XY");

	if (NULL != m_hHwnd)
	{
		::SendMessageTimeout(m_hHwnd, eMSG_EXPO_FEM_PRINT_SET_POINT, (WPARAM)m_u8PritCntX, (LPARAM)m_u8PritCntY, SMTO_NORMAL, 100, NULL);
	}
	
	/*노광 횟수에 따라 시작위치 설정*/
	dArrStartXY[0] = m_dbStartX + ((m_u8PritCntX - 1) * m_dPerioX);
	dArrStartXY[1] = m_dbStartY + ((m_u8PritCntY - 1) * m_dPerioY);

	TCHAR tzMesg[1024] = { NULL };
	swprintf_s(tzMesg, 1024, L"[FEM][%d][%d] EXPO START POS %.4f, %.4f", m_u8PritCntX, m_u8PritCntY, dArrStartXY[0], dArrStartXY[1]);
	//LOG_MESG(ENG_EDIC::en_debug, _T("[FEM][%d][%d] EXPO START POS %.4f, %.4f", m_u8PritCntX, m_u8PritCntY, dArrStartXY[0], dArrStartXY[1]));
	LOG_MESG(ENG_EDIC::en_debug, tzMesg);

	/* 만약 노광 시작 위치 값이 음수이면, 에러 처리 */
	if (dArrStartXY[0] < 0.0f || dArrStartXY[1] < 0.0f)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"The exposure start position is incorrect");
		return ENG_JWNS::en_error;
	}

	/* 노광 시작 위치 정보 초기화 */
	pstMachine->table_expo_start_xy[0].x = -1;
	pstMachine->table_expo_start_xy[0].y = -1;

	/* 송신 설정 */
	/* Led Duty Cycle & Frame Rate 설치 */
	if (!uvEng_Luria_ReqSetTableExposureStartPos(0x01, dArrStartXY[0], dArrStartXY[1]))	/* table number is fixed */
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetExposeStartXY)");
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : 노광 시작 위치 설정 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkFEMExpo::IsExposeStartXY()
{
	LPG_LDMC pstMachine = &uvEng_ShMem_GetLuria()->machine;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Expose.Start.XY");

	/* 노광 시작 위치 값이 확인 되었는지 여부 */
	if (pstMachine->table_expo_start_xy[0].x >= 0 && pstMachine->table_expo_start_xy[0].y >= 0)
	{
		return ENG_JWNS::en_next;
	}

	return ENG_JWNS::en_wait;
}


/*
 desc : 모든 광학계 Z Axis의 높이 조절 - 소재 두께에 따라
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkFEMExpo::SetPhZAxisMovingAll()
{
	UINT8 i;
	DOUBLE dbPhDiffZ = 0.0f, dbPhVeloZ[8] = { NULL };

	LPG_CSAI pstAlign = &uvEng_GetConfig()->set_align;
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	LPG_CLSI pstLuria = &uvEng_GetConfig()->luria_svc;
	LPG_CMSI pstMC2 = &uvEng_GetConfig()->mc2b_svc;
	UINT8	 u8drv_id;

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Optic.ZAxis.Move");

	/* 가장 최근의 설정한 광학계 Z 축 높이 조절 값 초기화 */
	memset(m_dbPhZAxisSet, 0x00, sizeof(DOUBLE) * MAX_PH);

	for (i = 0x00; i < pstLuria->ph_count; i++)
	{
		/* 모든 광학계 높이 조절 */
		/* mm -> um 변환 */
		/* 시작 Focus 위치에서 X Cnt 맞쳐 노광 포커스 조정 (단위: um)*/
		/* 0 : X = Focus Y = Energy, 1 : X = Energy Y = Focus */
		if (0 == m_u8PrintType)
		{
			dbPhDiffZ = m_dStartFocus[i] + (m_dStepFocus * (m_u8PritCntX - 1))/* / 1000.0f*/;
		}
		else
		{
			dbPhDiffZ = m_dStartFocus[i] + (m_dStepFocus * (m_u8PritCntY - 1))/* / 1000.0f*/;
		}

		TCHAR tzMesg[1024] = { NULL };
		swprintf_s(tzMesg, 1024, L"[FEM][%d][%d] Z POS %d %.4f", m_u8PritCntX, m_u8PritCntY, i, dbPhDiffZ);
		LOG_MESG(ENG_EDIC::en_debug, tzMesg);

		m_dbPhZAxisSet[i] = /*pstLuria->ph_z_focus[i] + */dbPhDiffZ;
		/* Photohead Z Axis의 Min or Max 값 범위 안에 있는지 여부 */
		if (pstMC2->min_dist[i] > m_dbPhZAxisSet[i] ||
			pstMC2->max_dist[i] < m_dbPhZAxisSet[i])
		{
			/* Set the error message */
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Optical Z Axis movement is out of range");
			return ENG_JWNS::en_error;
		}

		/* 한번에 모든 광학계 Z 축 높이 조정 */
		u8drv_id = (UINT8)ENG_MMDI::en_axis_ph1 + i;
		dbPhVeloZ[i] = pstMC2->max_velo[UINT8(ENG_MMDI::en_axis_ph2)];
		if (!(uvEng_MC2_SendDevAbsMove(ENG_MMDI(u8drv_id), m_dbPhZAxisSet[i], dbPhVeloZ[i])))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to move the photohead z axis (set position)");
			return ENG_JWNS::en_error;
		}
	}
	
	/* 현재 위치 정보를 요청하지 않았지만, 요청 했다고 해야 함. (왜냐! 모터 이동 후 바로 도착하지 않으므로) */
	SetSendCmdTime();

	return ENG_JWNS::en_next;
}

/*
 desc : 광학계 Z Axis의 높이 조절이 다 되었는지 여부 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkFEMExpo::IsPhZAxisMovedAll()
{
	UINT8 i;
	BOOL bArrived = TRUE;
	DOUBLE dbPhNowZ;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Optic.ZAxis.Moved");

	/* 데모 모드로 동작 중인지 여부에 따라 */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	/* 모든 광학계의 Z 축이 원하는 위치에 도달 했는지 여부 */
	for (i = 0x00; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		/*현재 위치값 확인*/
		dbPhNowZ = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(0x06 + i));

		if (abs(dbPhNowZ - m_dbPhZAxisSet[i]) > 0.010 /* .0 um */) /* 좀 넉넉히 설정해 줘야 한다. */
		{
			bArrived = FALSE;
			break;
		}
	}
	/* 만약 모든 광학계가 도달하지 않았다면, 일정 시간 후에 현재 광학계 위치 정보 요청 */
	if (!bArrived)
	{
		if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
		{
			if (IsSendCmdTime(250))
			{
				/* 광학계 Z 축 위치와 모터의 현재 상태 값 요청 */
				if (!uvEng_Luria_ReqGetMotorAbsPositionAll() ||
					!uvEng_Luria_ReqGetMotorStateAll())
				{
					return ENG_JWNS::en_error;
				}
			}
		}
		return ENG_JWNS::en_wait;
	}

	return ENG_JWNS::en_next;
}



/*
 desc : Led Duty Cycle and Frame Rate 설정
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkFEMExpo::SetStepDutyFrame()
{
	LPG_LDEW pstExpo = &uvEng_ShMem_GetLuria()->exposure;
	CUniToChar csCnv;

	LPG_RJAF pstJobRecipe = uvEng_JobRecipe_GetSelectRecipe();
	//LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetSelectRecipe();
	LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstJobRecipe->expo_recipe));

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Step.Duty.Frame");
	/* 만약 데모 버전일 경우 */
	if (uvEng_GetConfig()->IsRunDemo())
	{
		// by sysandj : 변수없음(수정)
		pstJobRecipe->step_size = 4;
		pstExpoRecipe->led_duty_cycle = 10;
		// by sysandj : 변수없음(수정)
		pstJobRecipe->frame_rate = 999;
	}

	/* 광량 계산 */
	UINT8 i = 0, j = 0;
	DOUBLE dbTotal = 0.0f, dbPowerWatt[MAX_LED] = { NULL }, dbSpeed = 0.0f;
	UINT16 u16frame_rate;
	DOUBLE dexpo_energy;
	/*각 레시피 조건에 따른 노광 속도 계산*/
	LPG_PLPI	pstPowerI = uvEng_LedPower_GetLedPowerName(csCnv.Ansi2Uni(pstExpoRecipe->power_name));

	/* 시작 Focus 위치에서 X Cnt 맞쳐 노광 포커스 조정 (단위: um)*/
	/* 0 : X = Focus Y = Energy, 1 : X = Energy Y = Focus */
	if (0 == m_u8PrintType)
	{
		dexpo_energy = (m_dStartEnergy + (m_dStepEnergy * (m_u8PritCntY - 1)));
	}
	else
	{
		dexpo_energy = (m_dStartEnergy + (m_dStepEnergy * (m_u8PritCntX - 1)));
	}

	TCHAR tzMesg[1024] = { NULL };
	swprintf_s(tzMesg, 1024, L"[FEM][%d][%d] EXPO ENERGY %.4f", m_u8PritCntX, m_u8PritCntY, dexpo_energy);
	LOG_MESG(ENG_EDIC::en_debug, tzMesg);

	for (i=0; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		for (j = 0; j < MAX_LED; j++)	dbPowerWatt[j] = pstPowerI->led_watt[i][j];
		dbTotal += uvCmn_Luria_GetEnergyToSpeed(pstJobRecipe->step_size, dexpo_energy,
			pstExpoRecipe->led_duty_cycle, dbPowerWatt);

	}
	u16frame_rate = (UINT16)(dbTotal / DOUBLE(i));

	swprintf_s(tzMesg, 1024, L"[FEM][%d][%d] EXPO ENERGY %.4f FRAME RATE=%d", m_u8PritCntX, m_u8PritCntY, dexpo_energy, u16frame_rate);
	LOG_MESG(ENG_EDIC::en_debug, tzMesg);

	/* Step Size, Duty Cycle 및 Frame 값 초기화 */
	pstExpo->SetDutyStepFrame();

	/* Step Size, Duty Cycle 및 Frame 값 설정 요청 */
	if (!uvEng_Luria_ReqSetExposureFactor(pstJobRecipe->step_size,
		pstExpoRecipe->led_duty_cycle,
		u16frame_rate / 1000.0f))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetExposureFactor)");
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : Led Duty Cycle and Frame Rate 설정 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkFEMExpo::IsStepDutyFrame()
{
	LPG_LDEW pstExpo = &uvEng_ShMem_GetLuria()->exposure;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Step.Duty.Frame");
	if (!pstExpo->IsSetDutyStepFrame())
	{
		return ENG_JWNS::en_wait;
	}
	return ENG_JWNS::en_next;
}

/*
설명 : 노광 횟수 확인
변수 : None
반환 : Step
*/
ENG_JWNS CWorkFEMExpo::CheckMeasCount()
{
	if (NULL != m_hHwnd)
	{
		::SendMessageTimeout(m_hHwnd, eMSG_EXPO_FEM_PRINT_PRINT_COMP, NULL, NULL, SMTO_NORMAL, 100, NULL);
	}

	/*X축 횟수 확인*/
	if (m_u8TotalX <= m_u8PritCntX)
	{
		/*Y축 횟수 확인*/
		if (m_u8TotalY <= m_u8PritCntY)
		{
			return ENG_JWNS::en_next;
		}
		else
		{
			m_u8PritCntY++;
			m_u8PritCntX = 1;
		}

	}
	/*X, Y축 횟수 모두 완료*/
	else
	{
		m_u8PritCntX++;
	}

	/*다시 시작*/
	m_u8StepIt = 0x01;
	return ENG_JWNS::en_wait;
}
