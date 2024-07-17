
/*
 desc : �뱤�� �ʿ��� ��ü ������ ����
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
 desc : ������
 parm : offset	- [in]  ���� �뱤�� ��, ������ ����� ���� �� (Pixel)
						�ٸ�, 0xff�̸�, ���� ȯ�� ���Ͽ� ������ ���� �� �״�� �뱤
						Hysterisys�� Negative Pixel ��
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
 desc : �ı���
 parm : None
 retn : None
*/
CWorkFEMExpo::~CWorkFEMExpo()
{
}

/*
 desc : �ʱ� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkFEMExpo::InitWork()
{
	/* ���� ��� ���� �� �ʱ�ȭ */
	if (!CWork::InitWork())	return FALSE;

	/* ��ü �۾� �ܰ� */
	m_u8StepTotal = 0x20;

	return TRUE;
}

/*
 desc : �ֱ������� �۾� ����
 parm : None
 retn : None
*/
VOID CWorkFEMExpo::DoWork()
{
	/* �۾� �ܰ� ���� ���� ó�� */
	switch (m_u8StepIt)
	{
	////////////////////
	/*FEM Setting ����*/
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
	/*�뱤 ����*/
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

	/* ���� �۾� ���� ���� �Ǵ� */
	CWorkFEMExpo::SetWorkNext();
	/* ��ð� ���� ���� ��ġ�� �ݺ� �����Ѵٸ� ���� ó�� */
	CWork::CheckWorkTimeout();
}


/*
 desc : ȯ�� ���Ͽ� ����� �Ķ���͸� Work Data�� ����
 parm : None
 retn : None
*/
VOID CWorkFEMExpo::InitParameter()
{
	LPG_CPFI pstPhFocs = &uvEng_GetConfig()->ph_focus;

	m_u8PrintType = pstPhFocs->print_type;
	m_u8TotalX = (UINT8)pstPhFocs->step_x_count;		/*X�� �� �뱤 Ƚ��*/
	m_u8TotalY = (UINT8)pstPhFocs->step_y_count;		/*Y�� �� �뱤 Ƚ��*/
	m_u8PritCntX = 1;									/*X�� ���� �뱤 Ƚ��*/
	m_u8PritCntY = 1;									/*Y�� ���� �뱤 Ƚ��*/
	m_dPerioX = pstPhFocs->step_move_x;					/*X�� �뱤 ����*/
	m_dPerioY = pstPhFocs->step_move_y;					/*Y�� �뱤 ����*/

	for (int i = 0; i < MAX_PH; i++)
	{
		m_dStartFocus[i] = pstPhFocs->start_foucs[i];	/*��Ŀ�� ���� ��ġ*/
	}
	m_dStartEnergy = pstPhFocs->start_energy;			/*���� ���� ��ġ*/
	m_dStepFocus = pstPhFocs->step_foucs;				/*��Ŀ�� ������*/
	m_dStepEnergy = pstPhFocs->step_energy;				/*���� ������*/

	m_dbStartX = uvEng_GetConfig()->luria_svc.table_expo_start_xy[0][0];		/*Stage �뱤 ���� X ��ġ (����: mm)*/
	m_dbStartY = uvEng_GetConfig()->luria_svc.table_expo_start_xy[0][1];		/*Stage �뱤 ���� Y ��ġ (����: mm)*/
}


/*
 desc : ȯ�� ���Ͽ� ����Ǿ� �־��� �Ķ���� ���� ����
 parm : None
 retn : None
*/
VOID CWorkFEMExpo::ResetParameter()
{
	uvEng_GetConfig()->luria_svc.table_expo_start_xy[0][0] = m_dbStartX;		/*Stage �뱤 ���� X ��ġ (����: mm)*/
	uvEng_GetConfig()->luria_svc.table_expo_start_xy[0][1] = m_dbStartY;		/*Stage �뱤 ���� Y ��ġ (����: mm)*/
	uvEng_SaveConfig();
}


/*
 desc : ���� �ܰ�� �̵��ϱ� ���� ó��22
 parm : None
 retn : None
*/
VOID CWorkFEMExpo::SetWorkNext()
{
	UINT64 u64JobTime	= GetTickCount64() - m_u64StartTime;

	/* �� �۾� �������� �ð� �� ���� ó�� */
	uvEng_UpdateJobWorkTime(u64JobTime);

	if (GetAbort())
	{
		CWork::EndWork();
		return;
	}

	if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* �۾��� ��� �� �ӽ� ���� */
		CWorkFEMExpo::CalcStepRate();

		if (m_u8StepIt == m_u8StepTotal)
		{
			m_enWorkState = ENG_JWNS::en_comp;
			/* �ʱ� Luria Error �� �ʱ�ȭ */
			uvCmn_Luria_ResetLastErrorStatus();

			/* �׻� ȣ��*/
			CWork::EndWork();
		}
		else
		{
		}
		/* ���� �۾��� ó���ϱ� ���� */
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

	/*1 ����Ŭ �Ϸ� �� ��ü Ƚ�� Ȯ��*/
	if (m_u8StepIt == 0x20)
	{
		ResetParameter();
		AfxMessageBox(L"FEM Expose Complete", MB_OK);
	}

}

/*
 desc : �۾� ����� ���� (percent; %)
 parm : None
 retn : None
*/
VOID CWorkFEMExpo::CalcStepRate()
{
	UINT8 u8PhCount	= uvEng_GetConfig()->luria_svc.ph_count;
	//DOUBLE dbRate	= DOUBLE(m_u8StepIt + m_u8SetPhNo) / DOUBLE(m_u8StepTotal + u8PhCount) * 100.0f;
	DOUBLE dbRate = DOUBLE(m_u8StepIt) / DOUBLE(m_u8StepTotal) * 100.0f;
	if (dbRate > 100.0f)	dbRate = 100.0f;
	/* ���� �޸𸮿� ���� */
	uvEng_SetWorkStepRate(dbRate);
}


/*
 desc : �뱤 ���� ��ġ ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkFEMExpo::SetExposeStartXY()
{
	DOUBLE dArrStartXY[2];
	LPG_LDMC pstMachine = &uvEng_ShMem_GetLuria()->machine;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Expose.Start.XY");

	if (NULL != m_hHwnd)
	{
		::SendMessageTimeout(m_hHwnd, eMSG_EXPO_FEM_PRINT_SET_POINT, (WPARAM)m_u8PritCntX, (LPARAM)m_u8PritCntY, SMTO_NORMAL, 100, NULL);
	}
	
	/*�뱤 Ƚ���� ���� ������ġ ����*/
	dArrStartXY[0] = m_dbStartX + ((m_u8PritCntX - 1) * m_dPerioX);
	dArrStartXY[1] = m_dbStartY + ((m_u8PritCntY - 1) * m_dPerioY);

	TCHAR tzMesg[1024] = { NULL };
	swprintf_s(tzMesg, 1024, L"[FEM][%d][%d] EXPO START POS %.4f, %.4f", m_u8PritCntX, m_u8PritCntY, dArrStartXY[0], dArrStartXY[1]);
	//LOG_MESG(ENG_EDIC::en_debug, _T("[FEM][%d][%d] EXPO START POS %.4f, %.4f", m_u8PritCntX, m_u8PritCntY, dArrStartXY[0], dArrStartXY[1]));
	LOG_MESG(ENG_EDIC::en_debug, tzMesg);

	/* ���� �뱤 ���� ��ġ ���� �����̸�, ���� ó�� */
	if (dArrStartXY[0] < 0.0f || dArrStartXY[1] < 0.0f)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"The exposure start position is incorrect");
		return ENG_JWNS::en_error;
	}

	/* �뱤 ���� ��ġ ���� �ʱ�ȭ */
	pstMachine->table_expo_start_xy[0].x = -1;
	pstMachine->table_expo_start_xy[0].y = -1;

	/* �۽� ���� */
	/* Led Duty Cycle & Frame Rate ��ġ */
	if (!uvEng_Luria_ReqSetTableExposureStartPos(0x01, dArrStartXY[0], dArrStartXY[1]))	/* table number is fixed */
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetExposeStartXY)");
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : �뱤 ���� ��ġ ���� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkFEMExpo::IsExposeStartXY()
{
	LPG_LDMC pstMachine = &uvEng_ShMem_GetLuria()->machine;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Expose.Start.XY");

	/* �뱤 ���� ��ġ ���� Ȯ�� �Ǿ����� ���� */
	if (pstMachine->table_expo_start_xy[0].x >= 0 && pstMachine->table_expo_start_xy[0].y >= 0)
	{
		return ENG_JWNS::en_next;
	}

	return ENG_JWNS::en_wait;
}


/*
 desc : ��� ���а� Z Axis�� ���� ���� - ���� �β��� ����
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

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Optic.ZAxis.Move");

	/* ���� �ֱ��� ������ ���а� Z �� ���� ���� �� �ʱ�ȭ */
	memset(m_dbPhZAxisSet, 0x00, sizeof(DOUBLE) * MAX_PH);

	for (i = 0x00; i < pstLuria->ph_count; i++)
	{
		/* ��� ���а� ���� ���� */
		/* mm -> um ��ȯ */
		/* ���� Focus ��ġ���� X Cnt ���� �뱤 ��Ŀ�� ���� (����: um)*/
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
		/* Photohead Z Axis�� Min or Max �� ���� �ȿ� �ִ��� ���� */
		if (pstMC2->min_dist[i] > m_dbPhZAxisSet[i] ||
			pstMC2->max_dist[i] < m_dbPhZAxisSet[i])
		{
			/* Set the error message */
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Optical Z Axis movement is out of range");
			return ENG_JWNS::en_error;
		}

		/* �ѹ��� ��� ���а� Z �� ���� ���� */
		u8drv_id = (UINT8)ENG_MMDI::en_axis_ph1 + i;
		dbPhVeloZ[i] = pstMC2->max_velo[UINT8(ENG_MMDI::en_axis_ph2)];
		if (!(uvEng_MC2_SendDevAbsMove(ENG_MMDI(u8drv_id), m_dbPhZAxisSet[i], dbPhVeloZ[i])))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to move the photohead z axis (set position)");
			return ENG_JWNS::en_error;
		}
	}
	
	/* ���� ��ġ ������ ��û���� �ʾ�����, ��û �ߴٰ� �ؾ� ��. (�ֳ�! ���� �̵� �� �ٷ� �������� �����Ƿ�) */
	SetSendCmdTime();

	return ENG_JWNS::en_next;
}

/*
 desc : ���а� Z Axis�� ���� ������ �� �Ǿ����� ���� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkFEMExpo::IsPhZAxisMovedAll()
{
	UINT8 i;
	BOOL bArrived = TRUE;
	DOUBLE dbPhNowZ;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Optic.ZAxis.Moved");

	/* ���� ���� ���� ������ ���ο� ���� */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	/* ��� ���а��� Z ���� ���ϴ� ��ġ�� ���� �ߴ��� ���� */
	for (i = 0x00; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		/*���� ��ġ�� Ȯ��*/
		dbPhNowZ = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(0x06 + i));

		if (abs(dbPhNowZ - m_dbPhZAxisSet[i]) > 0.010 /* .0 um */) /* �� �˳��� ������ ��� �Ѵ�. */
		{
			bArrived = FALSE;
			break;
		}
	}
	/* ���� ��� ���а谡 �������� �ʾҴٸ�, ���� �ð� �Ŀ� ���� ���а� ��ġ ���� ��û */
	if (!bArrived)
	{
		if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
		{
			if (IsSendCmdTime(250))
			{
				/* ���а� Z �� ��ġ�� ������ ���� ���� �� ��û */
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
 desc : Led Duty Cycle and Frame Rate ����
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

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Step.Duty.Frame");
	/* ���� ���� ������ ��� */
	if (uvEng_GetConfig()->IsRunDemo())
	{
		// by sysandj : ��������(����)
		pstJobRecipe->step_size = 4;
		pstExpoRecipe->led_duty_cycle = 10;
		// by sysandj : ��������(����)
		pstJobRecipe->frame_rate = 999;
	}

	/* ���� ��� */
	UINT8 i = 0, j = 0;
	DOUBLE dbTotal = 0.0f, dbPowerWatt[MAX_LED] = { NULL }, dbSpeed = 0.0f;
	UINT16 u16frame_rate;
	DOUBLE dexpo_energy;
	/*�� ������ ���ǿ� ���� �뱤 �ӵ� ���*/
	LPG_PLPI	pstPowerI = uvEng_LedPower_GetLedPowerName(csCnv.Ansi2Uni(pstExpoRecipe->power_name));

	/* ���� Focus ��ġ���� X Cnt ���� �뱤 ��Ŀ�� ���� (����: um)*/
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

	/* Step Size, Duty Cycle �� Frame �� �ʱ�ȭ */
	pstExpo->SetDutyStepFrame();

	/* Step Size, Duty Cycle �� Frame �� ���� ��û */
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
 desc : Led Duty Cycle and Frame Rate ���� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkFEMExpo::IsStepDutyFrame()
{
	LPG_LDEW pstExpo = &uvEng_ShMem_GetLuria()->exposure;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Step.Duty.Frame");
	if (!pstExpo->IsSetDutyStepFrame())
	{
		return ENG_JWNS::en_wait;
	}
	return ENG_JWNS::en_next;
}

/*
���� : �뱤 Ƚ�� Ȯ��
���� : None
��ȯ : Step
*/
ENG_JWNS CWorkFEMExpo::CheckMeasCount()
{
	if (NULL != m_hHwnd)
	{
		::SendMessageTimeout(m_hHwnd, eMSG_EXPO_FEM_PRINT_PRINT_COMP, NULL, NULL, SMTO_NORMAL, 100, NULL);
	}

	/*X�� Ƚ�� Ȯ��*/
	if (m_u8TotalX <= m_u8PritCntX)
	{
		/*Y�� Ƚ�� Ȯ��*/
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
	/*X, Y�� Ƚ�� ��� �Ϸ�*/
	else
	{
		m_u8PritCntX++;
	}

	/*�ٽ� ����*/
	m_u8StepIt = 0x01;
	return ENG_JWNS::en_wait;
}
