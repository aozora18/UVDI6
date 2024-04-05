
/*
 desc : �ó����� �⺻ (Base) �Լ� ����
*/

#include "pch.h"
#include "PowerMeasureMgr.h"
#include "../../MainApp.h"
//#include "../MainApp.h"
//#include "../mesg/DlgMesg.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif

/*
 desc : ������
 parm : None
 retn : None
*/
CPowerMeasureMgr::CPowerMeasureMgr()
{
	m_bStop = FALSE;


	m_u8StartHead = 0;
	m_u8StartLed = 0;

	m_u8Head = 0;
	m_u8Led = 0;
	m_u16PowerIndex = 0;

	m_bUseAllPH = FALSE;
	m_bUseAllWave = FALSE;
	m_bUseMaxIll = FALSE;
	m_u8MaxPoints = 0;
	m_dPitchXY = 0.;
	m_dPitchZ = 0.;

	m_pMeasureThread = NULL;
	m_bRunnigThread = FALSE;

	m_u16Index = 0;

	m_nRecipeNum = -1;
}

/*
 desc : �Ҹ���
 parm : None
 retn : None
*/
CPowerMeasureMgr::~CPowerMeasureMgr()
{	
	m_stVctMeasureTable.clear();
	m_stVctMeasureTable.shrink_to_fit();

	m_stVctPowerTable.clear();
	m_stVctPowerTable.shrink_to_fit();

	m_pMeasureThread = NULL;
	delete m_pMeasureThread;
}

/*
 desc : �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CPowerMeasureMgr::Initialize()
{
	int nHeadCount = uvEng_GetConfig()->luria_svc.ph_count;

	m_stVctPowerTable.clear();
	m_stVctPowerTable.shrink_to_fit();

	for (int i = 1; i <= nHeadCount; i++)
	{
		LoadTableFile(i);
	}
}

/*
 desc : ���� �� ȣ��
 parm : None
 retn : None
*/
VOID CPowerMeasureMgr::Terminate()
{
	m_bStop = TRUE;
	if (m_pMeasureThread != NULL)
	{
		WaitForSingleObject(m_pMeasureThread->m_hThread, INFINITE);
	}
}

/*
 desc : �Է��� ����ŭ ��⵿���� �����Ѵ�.
 parm : ��� �ð�(ms)
 retn : None
*/
VOID CPowerMeasureMgr::Wait(int nTime)
{
	DWORD dwStart = GetTickCount();

	while (GetTickCount() - nTime < dwStart)
	{
		if (TRUE == m_bStop)
		{
			return;
		}

		Sleep(1);
	}
}

/*
 desc : PowerTable ��� �˻�
 parm : �˻��� ��� ��ȣ�� LED ��ȣ
 retn : �Ҵ� ���� ���� ����
*/
BOOL CPowerMeasureMgr::BoundsCheckingPowerTable(UINT8 u8Head, UINT8 u8Led)
{
	if (1 > u8Head || 1 > u8Led)
	{
		return FALSE;
	}

	if (u8Head > (int)m_stVctPowerTable.size())
	{
		/* ȯ�� ���Ͽ� ������ ����ŭ �����ϱ⿡ ����� ����ٸ� ��� ��ȣ�� �߸� ������ �� */
		return FALSE;
	}

	if (u8Led > (int)m_stVctPowerTable[u8Head - 1].size())
	{
		/* ȯ�� ���Ͽ� ������ ����ŭ �����ϱ⿡ ����� ����ٸ� LED ��ȣ�� �߸� ������ �� */
		return FALSE;
	}

	return TRUE;
}

/*
 desc : �������� ����� ������ �о���δ�.
 parm : ���� ������ ��� ��ȣ
 retn : ���� ����
*/
VOID CPowerMeasureMgr::LoadTableFile(UINT8 u8Head)
{
	CStdioFile sFile;		/* �����۾��� */
	CString strPath;		/* ���� ��� */
	CString strLine;		/* ������ �� */
	CString strIndex;		/* ���� �ε��� �� */
	CString strPower;		/* ���� ���� �� */

	/* ���� �Ҵ�� ������ */
	ST_LED_VALUE stTable;
	VCT_LED_TABLE stVctLed;
	VCT_HEAD_TABLE stVctHead;

	int nLedCount = uvEng_GetConfig()->luria_svc.led_count;

	if (0 >= u8Head)
	{
		return;
	}
	
	/* LED ������ŭ ���� �Ҵ� */
	for (int i = 0; i < nLedCount; i++)
	{
		stVctHead.push_back(stVctLed);
	}

	strPath.Format(_T("%s\\%s\\cali\\HEAD%d.csv"), g_tzWorkDir, CUSTOM_DATA_CONFIG, u8Head);

	if (TRUE == sFile.Open(strPath, CStdioFile::shareDenyNone | CStdioFile::modeRead))
	{
		// LED1,365,LED2,385,LED3,395,LED4,405
		sFile.ReadString(strLine);

		// INDEX,POWER,INDEX,POWER,INDEX,POWER,INDEX,POWER
		sFile.ReadString(strLine);

		/* �� ���� �߰ߵ� �� ���� �ݺ� */
		while (sFile.ReadString(strLine))
		{
			// Parsing
			for (int i = 0; i < nLedCount; i++)
			{
				AfxExtractSubString(strIndex, strLine, (i * 2), _T(','));
				AfxExtractSubString(strPower, strLine, (i * 2) + 1, _T(','));

				stTable.u16Index = _ttoi(strIndex);
				stTable.dPower = _ttof(strPower);

				/* Index�� ��� ���� 0�� ��쿡�� ���̺� �������� �ʴ´�. */
				if (0 != stTable.u16Index && 0 != stTable.dPower)
				{
					stVctHead[i].push_back(stTable);
				}
			}
		}

		sFile.Close();
	}

	/* ������ ���� ���Ͽ�� ��� ������ �Ҵ� */
	m_stVctPowerTable.push_back(stVctHead);
}

/*
 desc : ���� ���� ������ ����� ���̺��� ������ �����.
 parm : ������ ��� ��ȣ
 retn : None
*/
BOOL CPowerMeasureMgr::SaveHeadTableFile(UINT8 u8Head)
{
	CStdioFile sFile;	/* �����۾��� */
	CString strPath;	/* ���� ��� */
	CString strLine;	/* ������ �� */
	CString strItem;	/* ���ڿ� �۾��� �ӽ� ���� */

	int nMaxRow = 0;
	int nLedCount = uvEng_GetConfig()->luria_svc.led_count;

	if (0 >= u8Head)
	{
		return FALSE;
	}

	strPath.Format(_T("%s\\%s\\cali\\HEAD%d.csv"), g_tzWorkDir, CUSTOM_DATA_CONFIG, u8Head);

	if (FALSE == sFile.Open(strPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
	{
		return FALSE;
	}

	/* LED ǥ�ö� parsing */
	for (int nLed = 1; nLed <= nLedCount; nLed++)
	{
		nMaxRow = (nMaxRow < m_stVctPowerTable[u8Head - 1][nLed - 1].size()) ? (int)m_stVctPowerTable[u8Head - 1][nLed - 1].size() : nMaxRow;
		strItem.Format(_T("LED%d,%d"), nLed, uvEng_Luria_GetLedNoToFreq(nLed));
		strLine = (1 == nLed) ? strItem : strLine + _T(",") + strItem;
	}

	sFile.WriteString(strLine + _T("\n"));
	sFile.WriteString(_T("INDEX,POWER,INDEX,POWER,INDEX,POWER,INDEX,POWER\n"));

	for (int nRow = 0; nRow < nMaxRow; nRow++)
	{
		strLine = _T("");

		for (int nLed = 1; nLed <= nLedCount; nLed++)
		{
			if (nRow < m_stVctPowerTable[u8Head - 1][nLed - 1].size())
			{
				strItem.Format(_T("%d,%.4f"),
					m_stVctPowerTable[u8Head - 1][nLed - 1][nRow].u16Index,
					m_stVctPowerTable[u8Head - 1][nLed - 1][nRow].dPower);
			}
			else
			{
				strItem = _T(",");
			}

			strLine = (1 == nLed) ? strItem : strLine + _T(",") + strItem;
		}

		sFile.WriteString(strLine + _T("\n"));
	}

	sFile.Close();
	return TRUE;
}

/* ������ �Ϸ�Ǿ��ٴ� �����Ͽ� ������ �Ǿ�� �Ѵ�. */
BOOL CPowerMeasureMgr::MeasureToPowerTable()
{
	if (0 >= (int)m_stVctMeasureTable.size())
	{
		/* ���� ���̺��� �������� �ʾ��� ��� */
		return FALSE;
	}

	if (FALSE == BoundsCheckingPowerTable(m_u8Head, m_u8Led))
	{
		return FALSE;
	}

	// ��� �˻� �Ϸ�

	/* ���� ���� �ʱ�ȭ */
	m_stVctPowerTable[m_u8Head - 1][m_u8Led - 1].clear();
	m_stVctPowerTable[m_u8Head - 1][m_u8Led - 1].shrink_to_fit();

	/* ���� ����� ���� */
	m_stVctPowerTable[m_u8Head - 1][m_u8Led - 1] = m_stVctMeasureTable;

	return TRUE;
}

VOID CPowerMeasureMgr::ClearPowerTable()
{
	for (int i = 0; i < m_stVctMeasureTable.size(); i++)
	{
		m_stVctMeasureTable[i].dPower = 0;
	}
}

/*
 desc : X, Y �̵��� ��ġ ���
 parm : ���� ��ġ, ��ǥ ��ġ, ��ǥ��, �� ����Ʈ �� �Ÿ� ��
 retn : ��ǥ�� ���� �̵��� ��ġ �� ��ȯ
*/
ST_MOVE_POS CPowerMeasureMgr::CalcXYMovePos(double dStandardX, double dStandardY, UINT8 u8Row, UINT8 u8Col, UINT8 u8MaxPoint, double dPitch)
{
	ST_MOVE_POS stResult;	/* ��ȯ�� ��ǥ�� */
	UINT8 u8point;			/* �߽� ��ǥ �� (3x3, 5x5, 7x7 ... ) */

	/* �� ����Ʈ ���� �������� ����, ������ ����Ʈ �� */
	/* �ش� ���� ������ �߽� ��ǥ �� (���� ���� 0�̱⿡ �������� ����ó���� �Ѵ�.) */
	u8point = (UINT8)(sqrt(u8MaxPoint) / 2);

	stResult.dX = dStandardX - ((u8point - u8Row) * dPitch);
	stResult.dY = dStandardY - ((u8point - u8Col) * dPitch);

	return stResult;
}

/*
 desc : X, Y �̵�
 parm : �̵��� ��ġ ��, �ӵ�
 retn : �̵� ���� ����
*/
BOOL CPowerMeasureMgr::StageMoveXY(double dX, double dY, double dSpeed)
{
	double dMin = -9999.;
	double dMax = 9999.;
	double dMaxSpeed = 9999.;

#ifdef MOTOR_SIMUL
	return TRUE;
#endif // MOTOR_SIMUL

	if (FALSE == uvCmn_MC2_IsDevLocked(ENG_MMDI::en_stage_x) ||
		FALSE == uvCmn_MC2_IsDevLocked(ENG_MMDI::en_stage_y))
	{
		LOG_MESG(ENG_EDIC::en_led_power_cali, _T("STAGE OFF"));
		return FALSE;
	}
	// ���� ���� Ȯ��
	if (TRUE == uvCmn_MC2_IsDriveError(ENG_MMDI::en_stage_x) ||
		TRUE == uvCmn_MC2_IsDriveError(ENG_MMDI::en_stage_y))
	{
		LOG_MESG(ENG_EDIC::en_led_power_cali, _T("STAGE ERROR"));
		return FALSE;
	}
	// Busy ��ȣ Ȯ��
	if (TRUE == uvCmn_MC2_IsDriveBusy(ENG_MMDI::en_stage_x) ||
		TRUE == uvCmn_MC2_IsDriveBusy(ENG_MMDI::en_stage_y))
	{
		LOG_MESG(ENG_EDIC::en_led_power_cali, _T("STAGE BUSY"));
		return FALSE;
	}
	
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

	if (0.005 < fabs(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x) - dX))
	{
		/* �̵� ���� �ִ� �ּҸ� �Ѿ�� �ʴ´�. */
		dMin = uvEng_GetConfig()->mc2_svc.min_dist[(UINT8)ENG_MMDI::en_stage_x];
		dMax = uvEng_GetConfig()->mc2_svc.max_dist[(UINT8)ENG_MMDI::en_stage_x];
		dMaxSpeed = uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_x];

		dX = (dX < dMin) ? dMin : dX;
		dX = (dX > dMax) ? dMax : dX;
		dSpeed = (dSpeed > dMaxSpeed) ? dMaxSpeed : dSpeed;

		if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_x, dX))
		{
			return FALSE;
		}
		if (FALSE == uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dX, dSpeed))
		{
			LOG_MESG(ENG_EDIC::en_led_power_cali, _T("STAGE X MOVE FAIL"));
			return FALSE;
		}
	}
	if (0.005 < fabs(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y) - dY))
	{
		/* �̵� ���� �ִ� �ּҸ� �Ѿ�� �ʴ´�. */
		dMin = uvEng_GetConfig()->mc2_svc.min_dist[(UINT8)ENG_MMDI::en_stage_y];
		dMax = uvEng_GetConfig()->mc2_svc.max_dist[(UINT8)ENG_MMDI::en_stage_y];
		dMaxSpeed = uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_y];

		dY = (dY < dMin) ? dMin : dY;
		dY = (dY > dMax) ? dMax : dY;
		dSpeed = (dSpeed > dMaxSpeed) ? dMaxSpeed : dSpeed;

		if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, dY))
		{
			return FALSE;
		}
		if (FALSE == uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dY, dSpeed))
		{
			LOG_MESG(ENG_EDIC::en_led_power_cali, _T("STAGE Y MOVE FAIL"));
			return FALSE;
		}
	}
	return TRUE;
}

/*
 desc : X, Y �̵� Ȯ��
 parm : �̵��ؾ� �� ��ġ ��, ��ٸ� �ð� ��, �̵� ��ġ ���� ��
 retn : �̵��� �Ϸ�Ǿ����� ����
*/
BOOL CPowerMeasureMgr::IsStageMovedXY(double dTargetX, double dTargetY, int nTimeOut/* = 60000*/, double dDiffDistance/* = 0.005*/)
{
	CTactTimeCheck cTime;			/* Ÿ�̸� */
	double dDiffX = 0.;				/* X��ġ ������ */
	double dDiffY = 0.;				/* Y��ġ ������ */

#ifdef MOTOR_SIMUL
	return TRUE;
#endif // MOTOR_SIMUL

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())
	{
		return TRUE;
	}

	/* Ÿ�̸� �ʱ�ȭ �� ���� */
	cTime.Init();
	cTime.Start();

	/* ���� �ð����� ���� */
	while (nTimeOut > (int)cTime.GetTactMs())
	{
		if (TRUE == m_bStop)
		{
			LOG_MESG(ENG_EDIC::en_led_power_cali, _T("STOP"));
			return FALSE;
		}

		/* Ÿ�� ��ġ�� ������ġ�� ������ ���Ѵ�. */
		dDiffX = dTargetX - uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);
		dDiffY = dTargetY - uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);

		/* Motor�� Ÿ����ġ�� �����ϰ� ���� ������ ��� ���� */
		if (FALSE == uvCmn_MC2_IsDriveBusy(ENG_MMDI::en_stage_x) &&
			FALSE == uvCmn_MC2_IsDriveBusy(ENG_MMDI::en_stage_y) &&
			fabs(dDiffX) < dDiffDistance &&
			fabs(dDiffY) < dDiffDistance)
		{
			return TRUE;
		}

		cTime.Stop();
		Sleep(1);
	}

	/* �ð� �ʰ� */
	LOG_MESG(ENG_EDIC::en_led_power_cali, _T("TIME OUT"));
	return FALSE;
}

/*
 desc : ���� ��� Z �̵�
 parm : ��� ��ȣ, �̵��� ��ġ ��, �ӵ�
 retn : �̵� ���� ����
*/
BOOL CPowerMeasureMgr::HeadMoveZ(UINT8 u8Head, double dZ, double dSpeed)
{
	UINT8 u8ConvHeadNo = (UINT8)ENG_MMDI::en_axis_ph1 + (u8Head - 1);	/* Head ��ȣ ��� */
	CString strLog;														/* �α׿� ���ڿ� */
	double dMinZ = -9999.;
	double dMaxZ = 9999.;
	double dMaxSpeed = 9999.;

#ifdef MOTOR_SIMUL
	return TRUE;
#endif // MOTOR_SIMUL

	if (FALSE == uvCmn_MC2_IsDevLocked(ENG_MMDI(u8ConvHeadNo)))
	{
		strLog.Format(_T("HEAD%d OFF"), u8Head);
		LOG_MESG(ENG_EDIC::en_led_power_cali, (TCHAR*)(LPCTSTR)strLog);
		return FALSE;
	}

	// ���� ���� Ȯ��
	if (TRUE == uvCmn_MC2_IsDriveError(ENG_MMDI(u8ConvHeadNo)))
	{
		strLog.Format(_T("HEAD%d ERROR"), u8Head);
		LOG_MESG(ENG_EDIC::en_led_power_cali, (TCHAR*)(LPCTSTR)strLog);
		return FALSE;
	}
	// Busy ��ȣ Ȯ��
	if (TRUE == uvCmn_MC2_IsDriveBusy(ENG_MMDI(u8ConvHeadNo)))
	{
		strLog.Format(_T("HEAD%d BUSY"), u8Head);
		LOG_MESG(ENG_EDIC::en_led_power_cali, (TCHAR*)(LPCTSTR)strLog);
		return FALSE;
	}

	/* �̵� ���� �ִ� �ּҸ� �Ѿ�� �ʴ´�. */
#if (MC2_DRIVE_2SET == 1)
	dMinZ = uvEng_GetConfig()->mc2b_svc.min_dist[u8Head - 1];
	dMaxZ = uvEng_GetConfig()->mc2b_svc.max_dist[u8Head - 1];
	dMaxSpeed = uvEng_GetConfig()->mc2b_svc.max_velo[u8Head - 1];
#else
	dMinZ = uvEng_GetConfig()->mc2_svc.min_dist[u8ConvHeadNo];
	dMaxZ = uvEng_GetConfig()->mc2_svc.max_dist[u8ConvHeadNo];
	dMaxSpeed = uvEng_GetConfig()->mc2_svc.max_velo[u8ConvHeadNo];
#endif

	dZ = (dZ < dMinZ) ? dMinZ : dZ;
	dZ = (dZ > dMaxZ) ? dMaxZ : dZ;
	dSpeed = (dSpeed > dMaxSpeed) ? dMaxSpeed : dSpeed;

	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI(u8ConvHeadNo));

	if (0.005 < fabs(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(u8ConvHeadNo)) - dZ))
	{
		if (FALSE == uvEng_MC2_SendDevAbsMove(ENG_MMDI(u8ConvHeadNo), dZ, dSpeed))
		{
			strLog.Format(_T("HEAD%d MOVE FAIL"), u8Head);
			LOG_MESG(ENG_EDIC::en_led_power_cali, (TCHAR*)(LPCTSTR)strLog);
			return FALSE;
		}
	}

	return TRUE;
}

/*
 desc : ���� ��� Z �̵� Ȯ��
 parm : ��� ��ȣ, �̵��ؾ� �� ��ġ ��, ��ٸ� �ð� ��, �̵� ��ġ ���� ��
 retn : �̵��� �Ϸ�Ǿ����� ����
*/
BOOL CPowerMeasureMgr::IsHeadMovedZ(UINT8 u8Head, double dTargetZ, int nTimeOut/* = 60000*/, double dDiffDistance/* = 0.005*/)
{
	UINT8 u8ConvHeadNo = (UINT8)ENG_MMDI::en_axis_ph1 + (u8Head - 1);	/* Head ��ȣ ��� */
	CTactTimeCheck cTime;												/* Ÿ�̸� */
	double dDiffZ = 0.;													/* Z��ġ ������ */

#ifdef MOTOR_SIMUL
	return TRUE;
#endif // MOTOR_SIMUL

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())
	{
		return TRUE;
	}

	/* Ÿ�̸� �ʱ�ȭ �� ���� */
	cTime.Init();
	cTime.Start();
	
	/* ���� �ð����� ���� */
	while (nTimeOut > (int)cTime.GetTactMs())
	{
		if (TRUE == m_bStop)
		{
			LOG_MESG(ENG_EDIC::en_led_power_cali, _T("STOP"));
			return FALSE;
		}

		/* Ÿ�� ��ġ�� ������ġ�� ������ ���Ѵ�. */
		dDiffZ = dTargetZ - uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(u8ConvHeadNo));

		/* Motor�� Ÿ����ġ�� �����ϰ� ���� ������ ��� ���� */
		if (FALSE == uvCmn_MC2_IsDriveBusy(ENG_MMDI(u8ConvHeadNo)) &&
			fabs(dDiffZ) < dDiffDistance)
		{
			return TRUE;
		}

		cTime.Stop();
		Sleep(1);
	}

	/* �ð� �ʰ� */
	LOG_MESG(ENG_EDIC::en_led_power_cali, _T("TIME OUT"));
	return FALSE;
}

/*
 desc : ���� ��� Z �̵��� Ȯ���ϸ鼭 ���� ����
 parm : ��� ��ȣ, �̵��ؾ� �� ��ġ ��, �ִ� �������� ������ ����, ������ ���� �ð�, ��ٸ� �ð� ��, �̵� ��ġ ���� ��
 retn : �̵��� �Ϸ�Ǿ����� ����
*/
BOOL CPowerMeasureMgr::HeadMovedZWhileMeasure(UINT8 u8Head, double dTargetZ, double& dZ, int nSamplingTime/* = 10*/, int nTimeOut/* = 60000*/, double dDiffDistance/* = 0.005*/)
{
	UINT8 u8ConvHeadNo = (UINT8)ENG_MMDI::en_axis_ph1 + (u8Head - 1);	/* Head ��ȣ ��� */
	CTactTimeCheck cTime;												/* Ÿ�̸� */
	double dCurPos = 0.;												/* ���� ��ġ�� */
	double dReadData = 0.;												/* ���� ������ */
	double dMaxPower = 0.;												/* �ִ� ������ */
	int nOldTime = 0;													/* ���� �ð� */

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())
	{
		return TRUE;
	}

	/* Ÿ�̸� �ʱ�ȭ �� ���� */
	cTime.Init();
	cTime.Start();

	/* �Ŀ����� �ʱ�ȭ �� Read ���� ���� */
	uvEng_Gentec_ResetValue();
	uvEng_Gentec_RunMeasure(TRUE);

	/* �ʱ� ������ �Է� */
	cTime.Stop();
	dMaxPower = dReadData = uvEng_Gentec_GetCurPower();
	nOldTime = (int)cTime.GetTactMs();
	dZ = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(u8ConvHeadNo));

#ifdef LED_SIMUL
	nTimeOut = (int)(nTimeOut * 0.1); // Conv 6 sec
#endif // LED_SIMUL

	/* ���� �ð����� ���� */
	while (nTimeOut > (int)cTime.GetTactMs())
	{
		if (TRUE == m_bStop)
		{
			LOG_MESG(ENG_EDIC::en_led_power_cali, _T("STOP"));
			return FALSE;
		}

		dCurPos = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(u8ConvHeadNo));

		/* Motor�� Ÿ����ġ�� �����ϰ� ���� ������ ��� ���� */
		if (FALSE == uvCmn_MC2_IsDriveBusy(ENG_MMDI(u8ConvHeadNo)) &&
			fabs(dTargetZ - dCurPos) < dDiffDistance)
		{
			uvEng_Gentec_RunMeasure(FALSE);
			return TRUE;
		}

		/* ���� �ð���ŭ ������ ��� */
		if ((int)cTime.GetTactMs() - nOldTime >= nSamplingTime)
		{
			dReadData = uvEng_Gentec_GetCurPower();

			/* ���� ���� �ִ밪���� Ŭ��� �� ���� */
			if (dMaxPower < dReadData)
			{
				dMaxPower = dReadData;
				dZ = dCurPos;
			}
		}

		cTime.Stop();
		Sleep(1);
	}

#ifdef LED_SIMUL
	return TRUE;
#endif // LED_SIMUL

	/* �ð� �ʰ� */
	uvEng_Gentec_RunMeasure(FALSE);
	LOG_MESG(ENG_EDIC::en_led_power_cali, _T("TIME OUT"));
	return FALSE;
}


/*
 desc : ���� �ܰ踦 ������� ��ȯ�Ѵ�.
 parm : ���� ���� �ܰ�
 retn : ���� ���� �ܰ� �����
*/
int CPowerMeasureMgr::GetProgressPercent(UINT16 u16Index)
{
	UINT uMaxIndex = 0;		/* �ִ� ���� ���� */
	int nProgress = 0;		/* �����(�����) */
	UINT nAddProcess = eCOUNT_OF_PROCESS;
	UINT nTableCnt = (UINT)m_stVctMeasureTable.size();

	if (TRUE == m_bUseMaxIll)
	{
		nAddProcess = m_u8MaxPoints + eMAX_PROGRESS;
	}
	if (TRUE == m_bUseAllWave)
	{
		nTableCnt *= (uvEng_GetConfig()->luria_svc.led_count - (m_u8Led - 1));
	}

	if (TRUE == m_bUseAllPH)
	{
		if (TRUE == m_bUseAllWave)
		{
			int nStartPH = nTableCnt;

			nTableCnt = (nAddProcess + (UINT)m_stVctMeasureTable.size()) * (uvEng_GetConfig()->luria_svc.ph_count - m_u8Head);
			uMaxIndex = nAddProcess + nStartPH + nTableCnt;
		}
		else
		{
			uMaxIndex = (nAddProcess + (UINT)m_stVctMeasureTable.size()) * (uvEng_GetConfig()->luria_svc.ph_count - (m_u8Head - 1));
		}
	}
	else
	{
		uMaxIndex = nAddProcess + nTableCnt;
	}

	nProgress = (int)((100.0 / uMaxIndex) * u16Index);

	return nProgress;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 desc : ������ ���̺� ����
 parm : ���� Ƚ��, ���� �ε��� ��, ������ �ε��� ��
 retn : None
*/
VOID CPowerMeasureMgr::SetPowerTable(UINT16 u16Count, UINT16 u16StartIndex, UINT16 u16StepIndex)
{
	ST_LED_VALUE stValue;

	m_stVctMeasureTable.clear();
	m_stVctMeasureTable.shrink_to_fit();

	for (UINT16 u16Step = 0; u16Step < u16Count; u16Step++)
	{
		stValue.u16Index = u16StartIndex + (u16StepIndex * u16Step);
		m_stVctMeasureTable.push_back(stValue);
	}
}

/*
 desc : ���� ��� �ɼ� ����
 parm : ��� ��ȣ, LED ��ȣ
 retn : None
*/
VOID CPowerMeasureMgr::SetPhotoHeadOption(UINT8 u8Head, UINT8 u8Led)
{
	/* ������� �ɼ��� �����ϸ� ������ ���� �ʱ�ȭ�ȴ�. */
	m_stVctMeasureTable.clear();
	m_stVctMeasureTable.shrink_to_fit();

	m_u8Head = u8Head;			/* ���õ� PhotoHead */
	m_u8Led = u8Led;			/* ���õ� LED */
}

/*
 desc : �ִ� ���� ��ġ ���� �� ����
 parm : ���� �ִ� ����Ʈ�� ��, �� ����Ʈ�� ������ �Ÿ� ��
 retn : None
*/
VOID CPowerMeasureMgr::SetFindPosParam(UINT8 u8MaxPoints, double dPitchXY, double dPitchZ)
{
	m_u8MaxPoints = u8MaxPoints;	/* ���� �ִ� ����Ʈ�� �� */
	m_dPitchXY = dPitchXY;			/* �� ����Ʈ�� ������ �Ÿ� �� */
	m_dPitchZ = dPitchZ;			/* �� ����Ʈ�� ������ �Ÿ� �� */
}

/*
 desc : ���� ��� LED ����
 parm : ��� ��ȣ, LED ��ȣ, ���� �ε��� ��, ���� �Ѱ� ����
 retn : ������ �Ϸ�Ǿ����� ����
*/
BOOL CPowerMeasureMgr::PhotoLedOnOff(UINT8 u8Head, UINT8 u8Led, UINT16 u16PwrIndex, BOOL bOn)
{
#ifdef LED_SIMUL
	return TRUE;
#endif // LED_SIMUL

	/* Power Reset */
	if (FALSE == uvEng_Luria_ReqSetLedPowerResetAll())				return FALSE;
	if (FALSE == uvEng_Luria_ReqSetActiveSequence(u8Head, 0x01))	return FALSE;
	if (FALSE == uvEng_Luria_ReqSetLedPowerResetAll())				return FALSE;

	// 20230828 mhbaek Add
	if (FALSE == uvEng_Luria_ReqSetFlatnessMaskOn(u8Head, 0x00))	return FALSE;
	if (FALSE == uvEng_Luria_ReqSetDmdMirrorShake(u8Head, 0x00))	return FALSE;

	if (bOn == TRUE)
	{
		auto res = uvEng_Luria_ReqSetLightIntensity(u8Head, ENG_LLPI(u8Led), u16PwrIndex);
		if (res)
		{
			uvEng_Luria_ReqGetLedPower(u8Head, ENG_LLPI(u8Led));
			res = uvEng_Luria_ReqSetLightPulseDuration(u8Head, LIGHT_PULSE_DURATION);
			if (res == false) return FALSE;
		}
		
	}
	
	//�̹��� ����
	/* ���� ��忡 ���ο� �̹��� ��ȣ ���� ��û */
	//xLuria_ReqSetLedPowerCalibration(HeadNo, 0x02, TRUE);
	if (FALSE == uvEng_Luria_ReqSetLoadInternalTestImage(u8Head, 0x03))	return FALSE;
	/* ���� ��忡 ���� ����Ǿ� �ִ� �̹��� ��ȣ ��û */
	//xLuria_ReqGetLedPowerCalibration(HeadNo);

	/* LED Power �� ���� */
	//xLuria_ReqSetLedPower(HeadNo,	(UINT16)m_edt_led[0].GetLedNo(),(UINT16)m_edt_num[0].GetTextToNum());

	//DMD on/off
	/* ���� ����� Ư�� LED ON or OFF ���� */
	//xLuria_ReqSetLedLightOnOff(HeadNo, UINT16(m_edt_led[0].GetLedNo()), onoff);

	/* ���� ����� Ư�� LED ���� (ON or OFF) ��û */
	if (FALSE == uvEng_Luria_ReqGetLedLightOnOff(u8Head, ENG_LLPI(u8Led)))	return FALSE;
	Wait(100);

	return TRUE;
}

/*
 desc : ��� ���� ��� LED Off
 parm : None
 retn : None
*/
VOID CPowerMeasureMgr::AllPhotoLedOff()
{
#ifdef LED_SIMUL
	return TRUE;
#endif // LED_SIMUL

	for (UINT8 u8Head = 1; u8Head <= uvEng_GetConfig()->luria_svc.ph_count; u8Head++)
	{
		for (UINT8 u8Led = 1; u8Led <= uvEng_GetConfig()->luria_svc.led_count; u8Led++)
		{
			PhotoLedOnOff(u8Head, u8Led, 0, FALSE);
		}
	}
}



/*
 desc : ������ X, Y ��ġ���� �������� ���� ū ��ġ�� �̵�
 parm : UI ������ �ڵ�
 retn : ������ �Ϸ�Ǿ����� ����
�� ��ǥ�� ���� ��ġ�� ������� �����ϱ⿡ ���� ���� �̵� �� ȣ���ؾ� �Ѵ�.
*/
BOOL CPowerMeasureMgr::FindMaxIllumPos(HWND hHwnd/* = NULL*/)
{
	LPG_CLPI pParam = &uvEng_GetConfig()->led_power;
	CString* pstrText = nullptr;				/* ������ �޽��� ���޿� ������ */
	CString strText;							/* UI ���޿� ���ڿ� */
	BOOL bForward = FALSE;						/* ������ */
	BOOL bColMove = FALSE;						/* ���� �̵� */
	UINT8 u8MaxMoveCount = 0;					/* �ִ� ������ �� */
	UINT8 u8CurMoveCount = 0;					/* ���� ������ �� */
	UINT8 u8Row, u8Col;							/* ��(����), ��(����) */
	UINT8 u8MaxPowerIndex = 0;					/* �ִ밪�� ������ ���� */
	ST_MOVE_POS stStartMovePos;					/* ���� ��ġ ����ü (������) */
	ST_MOVE_POS stMovePos;						/* �̵��� ��ġ�� ����ü */
	std::vector<ST_MEASURE_PARAM> stVctTable;	/* ���� ��� ���̺� */
	ST_MEASURE_PARAM stMeasure;					/* ���̺� ����� �ӽ� ���� */
// 	CString strPostMsg;							/* UI ���ÿ� ���ڿ� */
// 	int nPostProgress;							/* UI ���ÿ� ���൵ */

	/* ���� Point ��� */
	u8Col = u8Row = (UINT8)(sqrt(m_u8MaxPoints) / 2);

	/* ����(����) ��ġ ���� */
	stStartMovePos.dX = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);
	stStartMovePos.dY = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);

	if (NULL != hHwnd)
	{
		/* Post Message */
		strText.Format(_T("[HEAD%d] START MAX ILLUM XY PROCESS"), m_u8Head);
		pstrText = &strText;
		::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(++m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
	}

	/* LED On */
	PhotoLedOnOff(m_u8Head, m_u8Led, m_u16PowerIndex, TRUE);

	Wait(1000);

	/* �ִ� ����Ʈ��ŭ �ݺ� */
	for (UINT8 u8Index = 0; u8Index < m_u8MaxPoints; u8Index++)
	{
		if (TRUE == m_bStop)
		{
			strText.Format(_T("[HEAD%d] STOP"), m_u8Head);
			pstrText = &strText;
			::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);

			LOG_MESG(ENG_EDIC::en_led_power_cali, _T("STOP"));
			return FALSE;
		}

		// ù ���� �� ���� ����� ���������� �ʴ´�.
		if (0 != u8Index)
		{
			// �ִ� �̵� �Ÿ� ��ŭ �̵��Ѵٸ� �ʱ�ȭ
			if (u8MaxMoveCount <= u8CurMoveCount)
			{
				u8CurMoveCount = 0;
				bColMove = !bColMove;		// �̵� ���� ���� (����, ����)

				if (TRUE == bColMove)		// ���� �̵����� �����Ǿ��� ��
				{
					bForward = !bForward;	// �̵� ���� ���� (������, ������)
					u8MaxMoveCount++;		// �ִ� �̵� ��ġ ����
				}
			}

			if (TRUE == bColMove)
			{
				// ���� �̵�
				u8Col = (TRUE == bForward) ? (u8Col + 1) : (u8Col - 1);
			}
			else
			{
				// ���� �̵�
				u8Row = (TRUE == bForward) ? (u8Row + 1) : (u8Row - 1);
			}

			u8CurMoveCount++;				// ���� �̵� ��ġ ����
		}

		/* Move */
		stMovePos = CalcXYMovePos(stStartMovePos.dX, stStartMovePos.dY, u8Row, u8Col, m_u8MaxPoints, m_dPitchXY);
		if (FALSE == StageMoveXY(stMovePos.dX, stMovePos.dY, pParam->stage_speed))
		{
			LOG_MESG(ENG_EDIC::en_led_power_cali, _T("MOVE FAIL"));
			return FALSE;
		}

		/* Check InPosition */
		if (FALSE == IsStageMovedXY(stMovePos.dX, stMovePos.dY))
		{
			LOG_MESG(ENG_EDIC::en_led_power_cali, _T("INPOS FAIL"));
			return FALSE;
		}

		/* Data Save */
#ifdef LED_SIMUL
		stMeasure.dPower = DOUBLE(uvCmn_GetRandNumerI32(0x00, 3) / 1000.0f);
#else
		stMeasure.dPower = GetStableValue(m_u8Led);
#endif // LED_SIMUL

		stMeasure.u8Row = u8Row;
		stMeasure.u8Col = u8Col;

		stVctTable.push_back(stMeasure);

		// �ִ� ����
		if (stMeasure.dPower > stVctTable[u8MaxPowerIndex].dPower)
		{
			u8MaxPowerIndex = u8Index;
		}

		if (NULL != hHwnd)
		{
			/* Post Message */
			strText.Format(_T("[HEAD%d][%d/%d] SEARCH MAX ILLUM POINT"), m_u8Head, u8Index + 1, m_u8MaxPoints);
			pstrText = &strText;
			::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(++m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
		}

		Wait(MOTOR_MOVE_DELAY);
	}

	PhotoLedOnOff(m_u8Head, m_u8Led, 0, FALSE);

	if (TRUE == m_bStop)
	{
		strText.Format(_T("[HEAD%d] STOP"), m_u8Head);
		pstrText = &strText;
		::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);

		LOG_MESG(ENG_EDIC::en_led_power_cali, _T("STOP"));
		return FALSE;
	}
	
	/* �̵��ؾ��� ��ġ ��� */
	stMovePos = CalcXYMovePos(stStartMovePos.dX, stStartMovePos.dY,
		stVctTable[u8MaxPowerIndex].u8Row, stVctTable[u8MaxPowerIndex].u8Col,
		m_u8MaxPoints, m_dPitchXY);

	/* Last Move */
	if (FALSE == StageMoveXY(stMovePos.dX, stMovePos.dY, pParam->stage_speed))
	{
		LOG_MESG(ENG_EDIC::en_led_power_cali, _T("MOVE FAIL"));
		return FALSE;
	}

	if (NULL != hHwnd)
	{
		/* Post Message */
		strText.Format(_T("[HEAD%d] MOVE MAX ILLUM POSITION"), m_u8Head);
		pstrText = &strText;
		::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(++m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
	}

	/* Check InPosition */
	if (FALSE == IsStageMovedXY(stMovePos.dX, stMovePos.dY))
	{
		LOG_MESG(ENG_EDIC::en_led_power_cali, _T("INPOS FAIL"));
		return FALSE;
	}

	if (NULL != hHwnd)
	{
		/* Post Message */
		strText.Format(_T("[HEAD%d] MAX ILLUM POSITION MOVE COMPLETE"), m_u8Head);
		pstrText = &strText;
		::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(++m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
	}

	/* �ش� ��ġ ���� ��û */
	::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_XY_POS, (WPARAM)&stMovePos.dX, (LPARAM)&stMovePos.dY, SMTO_NORMAL, 500, NULL);

	return TRUE;
}

/*
 desc : ������ Z ��ġ���� �������� ���� ū ��ġ�� �̵�
 parm : UI ������ �ڵ�
 retn : ������ �Ϸ�Ǿ����� ����
�� ��ǥ�� ���� ��ġ�� ������� �����ϱ⿡ ���� ���� �̵� �� ȣ���ؾ� �Ѵ�.
*/
BOOL CPowerMeasureMgr::FindFocusPos(HWND hHwnd/* = NULL*/)
{
	LPG_CLPI pParam = &uvEng_GetConfig()->led_power;
	CString* pstrText = nullptr;	/* ������ �޽��� ���޿� ������ */
	CString strText;				/* UI ���޿� ���ڿ� */
	double dStandardZ = 0.;			/* ���� ��ġ */
	double dTargetZ = 0.;			/* �̵��� ��ġ */
	UINT8 u8ConvHeadNo = (UINT8)ENG_MMDI::en_axis_ph1 + (m_u8Head - 1);

	dStandardZ = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(u8ConvHeadNo));

	if (NULL != hHwnd)
	{
		/* Post Message */
		strText.Format(_T("[HEAD%d] START MAX ILLUM FOCUS PROCESS"), m_u8Head);
		pstrText = &strText;
		::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(++m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
	}

	/* Move Top Pos */
	// Speed�� �Է��� �޾ƾ� ���� ���
	if (FALSE == HeadMoveZ(m_u8Head, dStandardZ + (m_dPitchZ * 0.5), pParam->head_speed))
	{
		LOG_MESG(ENG_EDIC::en_led_power_cali, _T("MOVE FAIL"));
		return FALSE;
	}

	/* Check InPosition */
	if (FALSE == IsHeadMovedZ(m_u8Head, dStandardZ + (m_dPitchZ * 0.5)))
	{
		LOG_MESG(ENG_EDIC::en_led_power_cali, _T("INPOS FAIL"));
		return FALSE;
	}

	if (NULL != hHwnd)
	{
		/* Post Message */
		strText.Format(_T("[HEAD%d] MOVE UP COMPLETE"), m_u8Head);
		pstrText = &strText;
		::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(++m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
		Wait(200);
	}

	/* LED On */
	PhotoLedOnOff(m_u8Head, m_u8Led, m_u16PowerIndex, TRUE);
	Wait(1000);

	/* ���� ����ȭ �� ������ ��� */
	GetStableValue(m_u8Led);

	/* Move Bottom Pos */
	if (FALSE == HeadMoveZ(m_u8Head, dStandardZ - (m_dPitchZ * 0.5), pParam->head_speed * 0.5))
	{
		LOG_MESG(ENG_EDIC::en_led_power_cali, _T("MOVE FAIL"));
		return FALSE;
	}

	if (NULL != hHwnd)
	{
		/* Post Message */
		strText.Format(_T("[HEAD%d] MOVED Z WHILE MEASURE"), m_u8Head);
		pstrText = &strText;
		::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(++m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
	}

	/* Check InPosition */
	if (FALSE == HeadMovedZWhileMeasure(m_u8Head, dStandardZ - (m_dPitchZ * 0.5), dTargetZ))
	{
		PhotoLedOnOff(m_u8Head, m_u8Led, 0, FALSE);
		LOG_MESG(ENG_EDIC::en_led_power_cali, _T("MEASURE FAIL"));
		return FALSE;
	}

	/* LED Off */
	PhotoLedOnOff(m_u8Head, m_u8Led, 0, FALSE);

	if (TRUE == m_bStop)
	{
		strText.Format(_T("[HEAD%d] STOP"), m_u8Head);
		pstrText = &strText;
		::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);

		LOG_MESG(ENG_EDIC::en_led_power_cali, _T("STOP"));
		return FALSE;
	}

	if (NULL != hHwnd)
	{
		/* Post Message */
		strText.Format(_T("[HEAD%d] MOVE DOWN COMPLETE"), m_u8Head);
		pstrText = &strText;
		::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(++m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
	}

	/* Move Max Ill Pos */
	if (FALSE == HeadMoveZ(m_u8Head, dTargetZ, pParam->head_speed))
	{
		LOG_MESG(ENG_EDIC::en_led_power_cali, _T("MOVE FAIL"));
		return FALSE;
	}

	/* Check InPosition */
	if (FALSE == IsHeadMovedZ(m_u8Head, dTargetZ))
	{
		LOG_MESG(ENG_EDIC::en_led_power_cali, _T("INPOS FAIL"));
		return FALSE;
	}

	if (NULL != hHwnd)
	{
		/* Post Message */
		strText.Format(_T("[HEAD%d] MAX ILLUM FOCUS MOVE COMPLETE"), m_u8Head);
		pstrText = &strText;
		::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(++m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
		Wait(200);
	}

	/* �ش� ��ġ ���� ��û */
	::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_Z_POS, (WPARAM)&dTargetZ, NULL, SMTO_NORMAL, 500, NULL);

	return TRUE;
}

/*
 desc : ������ Z ��ġ���� �������� ���� ū ��ġ�� �̵�
 parm : UI ������ �ڵ�
 retn : ������ �Ϸ�Ǿ����� ����
�� ��ǥ�� ���� ��ġ�� ������� �����ϱ⿡ ���� ���� �̵� �� ȣ���ؾ� �Ѵ�.
*/
BOOL CPowerMeasureMgr::PowerMeasure(HWND hHwnd/* = NULL*/)
{
	CString* pstrPower = nullptr;
	CString* pstrText = nullptr;
	CString strPower;
	CString strText;
	int nMeasureMaxCnt = 1;
	int nStartIdx = 0;

	if (TRUE == m_bUseAllWave)
	{
		// 20230731 mhbaek Modify : ������ LED���� ���� �����ϵ��� ����
		//m_u8Led = 1;
		nMeasureMaxCnt = uvEng_GetConfig()->luria_svc.led_count;
		nStartIdx = (m_u8Led - 1);
	}

	for (int nMeasureCnt = nStartIdx; nMeasureCnt < nMeasureMaxCnt; nMeasureCnt++)
	{
		if (uvEng_Luria_GetLedNoToFreq(m_u8Led) != uvEng_Gentec_GetQueryWaveLength())
		{
			uvEng_Gentec_SetQueryWaveLength(uvEng_Luria_GetLedNoToFreq(m_u8Led));
			Wait(500);
		}

		if (NULL != hHwnd)
		{
			/* Post Message */
			strText.Format(_T("[HEAD%d][%dnm] START MEASURE POWER"), m_u8Head, uvEng_Luria_GetLedNoToFreq(m_u8Led));
			pstrText = &strText;
			::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(++m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
		}

		ClearPowerTable();

		for (int i = 0; i < m_stVctMeasureTable.size(); i++)
		{
			if (TRUE == m_bStop)
			{
				strText.Format(_T("[HEAD%d][%dnm] STOP"), m_u8Head, uvEng_Luria_GetLedNoToFreq(m_u8Led));
				pstrText = &strText;
				::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);

				LOG_MESG(ENG_EDIC::en_led_power_cali, _T("STOP"));
				return FALSE;
			}

			/* LED On */
			if (0 == i)
			{
				PhotoLedOnOff(m_u8Head, m_u8Led, m_stVctMeasureTable[i].u16Index, TRUE);
				Wait(1000);
			}
			
			if (uvEng_Luria_ReqSetLightIntensity(m_u8Head, ENG_LLPI(m_u8Led), m_stVctMeasureTable[i].u16Index))
			{
				uvEng_Luria_ReqGetLedPower(m_u8Head, ENG_LLPI(m_u8Led));

				Wait(1000);
			}



#ifdef LED_SIMUL
			m_stVctMeasureTable[i].dPower = DOUBLE(uvCmn_GetRandNumerI32(0x00, 3) / 1000.0f);
#else
			/* Data Save */
			m_stVctMeasureTable[i].dPower = GetStableValue(m_u8Led);
#endif // _SIMUL

			if (NULL != hHwnd)
			{
				/* Post Message */
				strPower.Format(_T("%.3f"), m_stVctMeasureTable[i].dPower);
				strText.Format(_T("[HEAD%d][%dnm][%d/%d] MEASURE POWER..."),
					m_u8Head,
					uvEng_Luria_GetLedNoToFreq(m_u8Led),
					(i + 1),
					m_stVctMeasureTable.size());

				pstrPower = &strPower;
				pstrText = &strText;

				::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_TABLE, (WPARAM)(i + 1), (LPARAM)pstrPower, SMTO_NORMAL, 500, NULL);
				::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(++m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
			}

			Wait(10);
		}

		/* LED OFF */
		PhotoLedOnOff(m_u8Head, m_u8Led, 0, FALSE);
		Wait(1000);
		
		if (FALSE == MeasureToPowerTable())
		{
			return FALSE;
		}
		else
		{
			SaveHeadTableFile(m_u8Head);
		}

		if (TRUE == m_bUseAllWave)
		{
			m_u8Led++;

			if (m_u8Led > nMeasureMaxCnt)
			{
				m_u8Led = 1;
			}
		}
	}

	return TRUE;
}


/*
 desc : ��������� ������ �����ǿ� �ش� ����� ��� ���� �Ŀ� �� ����
 parm : ���� ���, UI ������ �ڵ�
 retn : ������ �Ϸ�Ǿ����� ����
�� ��ǥ�� ���� ��ġ�� ������� �����ϱ⿡ ���� ���� �̵� �� ȣ���ؾ� �Ѵ�.
*/
BOOL CPowerMeasureMgr::RecipeMeasure(int nHead, HWND hHwnd/* = NULL*/)
{
	LPG_PLPI pstPowerIist = nullptr;
	int nValue = 0;

	if (0 > m_nRecipeNum)
	{
		// �����Ǹ� �������� ���� ä ����
		return FALSE;
	}

	pstPowerIist = uvEng_LedPower_GetLedPowerIndex(m_nRecipeNum);

	if (nullptr == pstPowerIist)
	{
		// ������ �ҷ����� ����
		return FALSE;
	}

	for (int nLed = 1; nLed <= uvEng_GetConfig()->luria_svc.led_count; nLed++)
	{
		if (uvEng_Luria_GetLedNoToFreq(nLed) != uvEng_Gentec_GetQueryWaveLength())
		{
			uvEng_Gentec_SetQueryWaveLength(uvEng_Luria_GetLedNoToFreq(nLed));
			Wait(500);
		}

		if (NULL != hHwnd)
		{
			::SendMessageTimeout(hHwnd, eMSG_UV_POWER_RECIPE_SET_POINT, (WPARAM)nHead, (LPARAM)nLed, SMTO_NORMAL, 100, NULL);
		}

		/* LED On */
		PhotoLedOnOff(nHead, nLed, pstPowerIist->led_index[nHead - 1][nLed - 1], TRUE);

		Wait(3000);

		if (TRUE == m_bStop)
		{
			LOG_MESG(ENG_EDIC::en_led_power_cali, _T("STOP"));
			return FALSE;
		}

#ifdef LED_SIMUL
		nValue = (int)uvCmn_GetRandNumerI32(0x00, 3);
#else
		/* Data Save */
		nValue = (int)(GetStableValue(nLed) * 1000);
#endif // _SIMUL

		if (NULL != hHwnd)
		{
			::SendMessageTimeout(hHwnd, eMSG_UV_POWER_RECIPE_MEASURE_COMP, (WPARAM)nValue, NULL, SMTO_NORMAL, 100, NULL);
		}

		Wait(10);

		/* LED OFF */
		PhotoLedOnOff(nHead, nLed, 0, FALSE);
		Wait(1000);
	}

	return TRUE;
}


/*
 desc : ���� ���μ���
 parm : UI ������ �ڵ�
 retn : ������ �Ϸ�Ǿ����� ����
 cmnt : ���� ��⵿������ ���� �����带 ������ UI���� ȣ���� �����Ѵ�.
*/
BOOL CPowerMeasureMgr::Measurement(HWND hHwnd)
{
	LPG_CLPI pParam = &uvEng_GetConfig()->led_power;
	CString* pstrText = nullptr;
	CString strText;
	int nMeasureMaxCnt = 1;
	int nStartIdx = 0;

	/* ���� �÷��� �ʱ�ȭ */
	m_bStop = FALSE;
	m_u16Index = 0;

	m_u8StartHead = m_u8Head;
	m_u8StartLed = m_u8Led;

	if (TRUE == m_bUseAllPH)
	{
		// 20230731 mhbaek Modify : ������ Head���� ���� �����ϵ��� ����
		//m_u8Head = 1;
		nMeasureMaxCnt = uvEng_GetConfig()->luria_svc.ph_count;
		nStartIdx = (m_u8Head - 1);
	}

	for (int nMeasureCnt = nStartIdx; nMeasureCnt < nMeasureMaxCnt; nMeasureCnt++)
	{

#pragma region Move Measure Position
		if (NULL != hHwnd)
		{
			/* Post Message */
			strText.Format(_T("[HEAD%d] START PROCESS"), m_u8Head);
			pstrText = &strText;
			::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(++m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
		}

		/* Move Measure Pos */
		if (FALSE == StageMoveXY(pParam->measure_pos[m_u8Head - 1][ePOS_X], pParam->measure_pos[m_u8Head - 1][ePOS_Y], pParam->stage_speed))
		{
			if (TRUE == m_bUseAllPH)
			{
				m_u8Head = m_u8StartHead;
				m_u8Led = m_u8StartLed;
			}

			strText.Format(_T("[HEAD%d] MOVE XY FAIL"), m_u8Head);
			pstrText = &strText;
			::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);

			LOG_MESG(ENG_EDIC::en_led_power_cali, _T("MOVE XY FAIL"));
			m_bRunnigThread = FALSE;
			return FALSE;
		}

		if (NULL != hHwnd)
		{
			/* Post Message */
			strText.Format(_T("[HEAD%d] MOVE XY POSITION"), m_u8Head);
			pstrText = &strText;
			::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(++m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
		}

		/* Check InPosition */
		if (FALSE == IsStageMovedXY(pParam->measure_pos[m_u8Head - 1][ePOS_X], pParam->measure_pos[m_u8Head - 1][ePOS_Y]))
		{
			if (TRUE == m_bUseAllPH)
			{
				m_u8Head = m_u8StartHead;
				m_u8Led = m_u8StartLed;
			}

			LOG_MESG(ENG_EDIC::en_led_power_cali, _T("INPOS XY FAIL"));
			m_bRunnigThread = FALSE;
			return FALSE;
		}

		if (NULL != hHwnd)
		{
			/* Post Message */
			strText.Format(_T("[HEAD%d] XY MOVE COMPLETE"), m_u8Head);
			pstrText = &strText;
			::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(++m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
		}

		/* Move Max Ill Pos */
		if (FALSE == HeadMoveZ(m_u8Head, pParam->measure_pos[m_u8Head - 1][ePOS_Z], pParam->head_speed))
		{
			if (TRUE == m_bUseAllPH)
			{
				m_u8Head = m_u8StartHead;
				m_u8Led = m_u8StartLed;
			}

			strText.Format(_T("[HEAD%d] MOVE Z FAIL"), m_u8Head);
			pstrText = &strText;
			::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);

			LOG_MESG(ENG_EDIC::en_led_power_cali, _T("MOVE Z FAIL"));
			m_bRunnigThread = FALSE;
			return FALSE;
		}

		if (NULL != hHwnd)
		{
			/* Post Message */
			strText.Format(_T("[HEAD%d] MOVE Z POSITION"), m_u8Head);
			pstrText = &strText;
			::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(++m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
		}

		/* Check InPosition */
		if (FALSE == IsHeadMovedZ(m_u8Head, pParam->measure_pos[m_u8Head - 1][ePOS_Z]))
		{
			if (TRUE == m_bUseAllPH)
			{
				m_u8Head = m_u8StartHead;
				m_u8Led = m_u8StartLed;
			}

			LOG_MESG(ENG_EDIC::en_led_power_cali, _T("INPOS Z FAIL"));
			m_bRunnigThread = FALSE;
			return FALSE;
		}

		if (NULL != hHwnd)
		{
			/* Post Message */
			strText.Format(_T("[HEAD%d] Z MOVE COMPLETE"), m_u8Head);
			pstrText = &strText;
			::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(++m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
		}

		Wait(MOTOR_MOVE_DELAY);

		/* Search for Max Illuminance Pos */
		if (TRUE == m_bUseMaxIll)
		{
			if (FALSE == FindMaxIllumPos(hHwnd))
			{
				if (TRUE == m_bUseAllPH)
				{
					m_u8Head = m_u8StartHead;
					m_u8Led = m_u8StartLed;
				}

				LOG_MESG(ENG_EDIC::en_led_power_cali, _T("FAIL"));
				m_bRunnigThread = FALSE;
				return FALSE;
			}

			if (FALSE == FindFocusPos(hHwnd))
			{
				if (TRUE == m_bUseAllPH)
				{
					m_u8Head = m_u8StartHead;
					m_u8Led = m_u8StartLed;
				}

				LOG_MESG(ENG_EDIC::en_led_power_cali, _T("FAIL"));
				m_bRunnigThread = FALSE;
				return FALSE;
			}
		}
#pragma endregion

		if (FALSE == PowerMeasure(hHwnd))
		{
			if (TRUE == m_bUseAllPH || TRUE == m_bUseAllWave)
			{
				m_u8Head = m_u8StartHead;
				m_u8Led = m_u8StartLed;
			}

			LOG_MESG(ENG_EDIC::en_led_power_cali, _T("POWER MEASURE FAIL"));
			m_bRunnigThread = FALSE;
			return FALSE;
		}

		if (NULL != hHwnd)
		{
			/* Post Message */
			strText.Format(_T("[HEAD%d] MEASURE PROCESS END"), m_u8Head);
			pstrText = &strText;
			::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_PROGRESS, (WPARAM)GetProgressPercent(++m_u16Index), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
		}

		if (TRUE == m_bUseAllPH)
		{
			m_u8Head++;
		}
	}

	if (TRUE == m_bUseAllPH || TRUE == m_bUseAllWave)
	{
		m_u8Head = m_u8StartHead;
		m_u8Led = m_u8StartLed;
	}

	m_bRunnigThread = FALSE;
	return TRUE;
}


#include <timeapi.h>

BOOL CPowerMeasureMgr::CheckRecipe(HWND hHwnd)
{
	CPowerMeasureMgr::GetInstance()->AllPhotoLedOff();

	LPG_CLPI pParam = &uvEng_GetConfig()->led_power;
	
	/* ���� �÷��� �ʱ�ȭ */
	m_bStop = FALSE;
	const int COOLDOWN_TIME = 5000;
	DWORD tick = timeGetTime();
	for (int nHead = 0; nHead < uvEng_GetConfig()->luria_svc.ph_count; nHead++)
	{
		/* Move Measure Pos */
		if (FALSE == StageMoveXY(pParam->measure_pos[nHead][ePOS_X], pParam->measure_pos[nHead][ePOS_Y], pParam->stage_speed))
		{
			m_bRunnigThread = FALSE;
			return FALSE;
		}

		/* Check InPosition */
		if (FALSE == IsStageMovedXY(pParam->measure_pos[nHead][ePOS_X], pParam->measure_pos[nHead][ePOS_Y]))
		{
			m_bRunnigThread = FALSE;
			return FALSE;
		}

		/* Move Max Ill Pos */
		if (FALSE == HeadMoveZ(nHead + 1, pParam->measure_pos[nHead][ePOS_Z], pParam->head_speed))
		{
			m_bRunnigThread = FALSE;
			return FALSE;
		}

		/* Check InPosition */
		if (FALSE == IsHeadMovedZ(nHead + 1, pParam->measure_pos[nHead][ePOS_Z]))
		{
			m_bRunnigThread = FALSE;
			return FALSE;
		}

		Wait(MOTOR_MOVE_DELAY);

		if(timeGetTime() - tick < COOLDOWN_TIME) //�ּ� 5���� ��ٿ�Ÿ���� �ʿ�.
			Wait(COOLDOWN_TIME - (timeGetTime() - tick));

		if (FALSE == RecipeMeasure(nHead + 1, hHwnd))
		{
			m_bRunnigThread = FALSE;
			return FALSE;
		}
	}

	if (NULL != hHwnd)
	{
		::SendMessageTimeout(hHwnd, eMSG_UV_POWER_RECIPE_ALL_COMP, NULL, NULL, SMTO_NORMAL, 100, NULL);
	}

	m_bRunnigThread = FALSE;
	return TRUE;
}

/*
 desc : ���� �� ���̺� �� ��ü �� ����
 parm : None
 retn : None
*/
VOID CPowerMeasureMgr::MeasureEnd()
{
	if (TRUE == IsProcessWorking())
	{
		return;
	}

	if (TRUE == MeasureToPowerTable())
	{
		SaveHeadTableFile(m_u8Head);
	}
}

/*
 desc : ���� ������ ����
 parm : UI ������ �ڵ�
 retn : ���� ����, ���� ����
*/
BOOL CPowerMeasureMgr::MeasureStart(HWND hHwnd/* = NULL*/)
{
	if (TRUE == m_bRunnigThread)
	{
		return FALSE;
	}

	m_bRunnigThread = TRUE;
	m_pMeasureThread = AfxBeginThread(PowerMeasureThread, (LPVOID)hHwnd);

	return TRUE;
}

/*
 desc : ������ ���� ������ ����
 parm : UI ������ �ڵ�
 retn : ���� ����, ���� ����
*/
BOOL CPowerMeasureMgr::RecipeCheckStart(HWND hHwnd/* = NULL*/)
{
	if (TRUE == m_bRunnigThread)
	{
		return FALSE;
	}

	m_bRunnigThread = TRUE;
	m_pMeasureThread = AfxBeginThread(RecipeCheckThread, (LPVOID)hHwnd);

	return TRUE;
}

/*
 desc : �ε��� �� ����
 parm : ���, LED ��ȣ, ���ϴ� ���� ��
 retn : �ε��� ��
*/
UINT16 CPowerMeasureMgr::GetPowerIndex(UINT8 u8Head, UINT8 u8Led, double dPower)
{
	VCT_LED_TABLE* pVctTable;	/* ���̺� ���� ������ */
	int nApproxIndex = 0;		/* �ٻ�ġ ���� �� */
	int nStartIndex = 0;		/* ��� ������ ���� �� */
	double dMinValue = dPower;	/* �ּ� ������ */

	
	/* ��� �˻� */
	if (FALSE == BoundsCheckingPowerTable(u8Head, u8Led))
	{
		return -1;
	}

	/* ������ ���� */
	pVctTable = &m_stVctPowerTable[u8Head - 1][u8Led - 1];

	// 6um Text
// 	if (pVctTable->begin()->dPower > dPower ||
// 		pVctTable->end()->dPower < dPower) //���� ���ܳ����� ó���Ҽ� ���� ��Ȳ�̴�. 
// 	{
// 		return 0;
// 	}

	if (eLAGRANGE_DATA_COUNT + 1 > (int)pVctTable->size())
	{
		return 0;
	}

	for (int i = 0; i < (int)pVctTable->size(); i++)
	{	
		/* ���� ���̺� �� ������ ���� �����Ѵٸ� ��ȯ */
		if (dPower == pVctTable->at(i).dPower)
		{
			return pVctTable->at(i).u16Index;
		}
		/* ���ϴ� �������� �ٻ�ġ�� ����� �� ���� */
		if (dMinValue > fabs(dPower - pVctTable->at(i).dPower))
		{
			dMinValue = fabs(dPower - pVctTable->at(i).dPower);
			nApproxIndex = i;
		}
	}
	
	/* ���� ������ ���� �ٻ�ġ�� ��� */
	if (nApproxIndex == (int)pVctTable->size() - 1)
	{
		nStartIndex = nApproxIndex - eLAGRANGE_DATA_COUNT;
	}
	/* Normal Case */
	else if(0 != nApproxIndex)
	{
		nStartIndex = nApproxIndex - (int)(eLAGRANGE_DATA_COUNT * 0.5);
	}
	/* ���۰��� ���� �ٻ�ġ�� ��� nStartIndex = 0 */

	///////////////////////////////////////////////////////////

	double dSum = 0;
	int nPolynomial = 0;

	// Lagrange	Interpolate
	for (nPolynomial = nStartIndex; nPolynomial < nStartIndex + eLAGRANGE_DATA_COUNT; nPolynomial++)
	{
		double dValue = 1;

		for (int nIdx = nStartIndex; nIdx < nStartIndex + eLAGRANGE_DATA_COUNT; nIdx++)
		{
			if (nIdx == nPolynomial)
			{
				continue;
			}

			dValue *= ((dPower - pVctTable->at(nIdx).dPower) / (pVctTable->at(nPolynomial).dPower - pVctTable->at(nIdx).dPower));
		}

		dSum += pVctTable->at(nPolynomial).u16Index * dValue;
	}

	/* �׷��� ��� ���� ���� �� */
	if ((UINT16)dSum <= pVctTable->at(nStartIndex + eLAGRANGE_DATA_COUNT - 1).u16Index)
	{
		return (UINT16)dSum;
	}

	nApproxIndex = (0 <= nApproxIndex) ? 1 : nApproxIndex;

	/* ��׶��� ���� �� �׷��� ��� ��簡 ���� ��� ��ʽ����� �̿� */
	double dSlope = (pVctTable->at(nApproxIndex).u16Index - pVctTable->at(nApproxIndex - 1).u16Index) / (pVctTable->at(nApproxIndex).dPower - pVctTable->at(nApproxIndex - 1).dPower);
	double dTempY = pVctTable->at(nApproxIndex - 1).u16Index - dSlope * pVctTable->at(nApproxIndex - 1).dPower;

	return UINT16(dSlope * dPower + dTempY);
}


double CPowerMeasureMgr::GetStableValue(UINT8 u8Led, double dAccuracy/* = 0.005*/, int nTimeOut/* = 60000*/)
{
	CTactTimeCheck cTime;				/* Ÿ�̸� */
	std::vector<double> dVctTable;
	double dAvr = 0.;
	double dMin = 9999.;
	double dMax = -9999.;
	double dSum = 0.;
	LPG_PGDV stReadData;

	/* Ÿ�̸� �ʱ�ȭ �� ���� */
	cTime.Init();
	cTime.Start();

	uvEng_Gentec_ResetValue();
	
	uvEng_Gentec_SetQueryWaveLength(uvEng_Luria_GetLedNoToFreq(u8Led));
	Wait(100);

	uvEng_Gentec_RunMeasure(TRUE);

	while (nTimeOut > cTime.GetTactMs())
	{
		if (TRUE == m_bStop)
		{
			return 0;
		}

		stReadData = uvEng_Gentec_GetValue();

		if (nullptr == stReadData)
		{
			Wait(10);
			continue;
		}

		dAvr = stReadData->average;

		dVctTable.push_back(stReadData->last_val);
		
		/* Vector Size�� �ִ� 10���� �����Ѵ�. */
		if (10 < dVctTable.size())
		{
			/* Front Pop */
			dVctTable.erase(dVctTable.begin());

			dMin = 9999.;
			dMax = -9999.;
			dSum = 0.;

			for (int i = 0; i < dVctTable.size(); i++)
			{
				dSum += dVctTable[i];
				dMin = (dVctTable[i] < dMin) ? dVctTable[i] : dMin;
				dMax = (dVctTable[i] > dMax) ? dVctTable[i] : dMax;
			}

			if (dAccuracy >= fabs(dMax - dMin))
			{
				uvEng_Gentec_RunMeasure(FALSE);
				dAvr = (0 == dSum) ? 0 : (dSum / dVctTable.size());
				return dAvr;
			}
		}

		Wait(100);
	}

	/* Time Out */
	uvEng_Gentec_RunMeasure(FALSE);
	return dAvr;
}


BOOL CPowerMeasureMgr::GetMaxValue(UINT8 u8Head, UINT8 u8Led, UINT16& u16Index, double& dPower)
{
	if (FALSE == BoundsCheckingPowerTable(u8Head, u8Led))
	{
		return FALSE;
	}

	double dMaxPower = 0;

	for (int i = 0; i < m_stVctPowerTable[u8Head - 1][u8Led - 1].size(); i++)
	{
		if (dMaxPower < m_stVctPowerTable[u8Head - 1][u8Led - 1][i].dPower)
		{
			u16Index = m_stVctPowerTable[u8Head - 1][u8Led - 1][i].u16Index;
			dMaxPower = m_stVctPowerTable[u8Head - 1][u8Led - 1][i].dPower;
			dPower = dMaxPower;
		}
	}

	return TRUE;
}


/*
 desc : ���� ������
 parm : UI ������ �ڵ�
 retn : 0
*/
UINT PowerMeasureThread(LPVOID lpParam)
{
	HWND hHwnd = (HWND)lpParam;

	CPowerMeasureMgr::GetInstance()->Measurement(hHwnd);

	return 0;
}

/*
 desc : ������ ���� ������
 parm : UI ������ �ڵ�
 retn : 0
*/
UINT RecipeCheckThread(LPVOID lpParam)
{
	HWND hHwnd = (HWND)lpParam;

	CPowerMeasureMgr::GetInstance()->CheckRecipe(hHwnd);

	return 0;
}