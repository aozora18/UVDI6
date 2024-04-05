
/*
 desc : 시나리오 기본 (Base) 함수 모음
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
 desc : 생성자
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
 desc : 소멸자
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
 desc : 초기화
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
 desc : 종료 시 호출
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
 desc : 입력한 값만큼 대기동작을 수행한다.
 parm : 대기 시간(ms)
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
 desc : PowerTable 경계 검사
 parm : 검사할 헤드 번호와 LED 번호
 retn : 할당 영역 존재 유무
*/
BOOL CPowerMeasureMgr::BoundsCheckingPowerTable(UINT8 u8Head, UINT8 u8Led)
{
	if (1 > u8Head || 1 > u8Led)
	{
		return FALSE;
	}

	if (u8Head > (int)m_stVctPowerTable.size())
	{
		/* 환경 파일에 설정된 값만큼 생성하기에 사이즈를 벗어난다면 헤드 번호를 잘못 지정한 것 */
		return FALSE;
	}

	if (u8Led > (int)m_stVctPowerTable[u8Head - 1].size())
	{
		/* 환경 파일에 설정된 값만큼 생성하기에 사이즈를 벗어난다면 LED 번호를 잘못 지정한 것 */
		return FALSE;
	}

	return TRUE;
}

/*
 desc : 측정값이 저장된 문서를 읽어들인다.
 parm : 읽을 문서의 헤드 번호
 retn : 읽은 유무
*/
VOID CPowerMeasureMgr::LoadTableFile(UINT8 u8Head)
{
	CStdioFile sFile;		/* 문서작업용 */
	CString strPath;		/* 문서 경로 */
	CString strLine;		/* 문서의 줄 */
	CString strIndex;		/* 읽은 인덱스 값 */
	CString strPower;		/* 읽은 조도 값 */

	/* 영역 할당용 변수들 */
	ST_LED_VALUE stTable;
	VCT_LED_TABLE stVctLed;
	VCT_HEAD_TABLE stVctHead;

	int nLedCount = uvEng_GetConfig()->luria_svc.led_count;

	if (0 >= u8Head)
	{
		return;
	}
	
	/* LED 개수만큼 사전 할당 */
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

		/* 빈 줄이 발견될 때 까지 반복 */
		while (sFile.ReadString(strLine))
		{
			// Parsing
			for (int i = 0; i < nLedCount; i++)
			{
				AfxExtractSubString(strIndex, strLine, (i * 2), _T(','));
				AfxExtractSubString(strPower, strLine, (i * 2) + 1, _T(','));

				stTable.u16Index = _ttoi(strIndex);
				stTable.dPower = _ttof(strPower);

				/* Index나 결과 값이 0인 경우에는 테이블에 저장하지 않는다. */
				if (0 != stTable.u16Index && 0 != stTable.dPower)
				{
					stVctHead[i].push_back(stTable);
				}
			}
		}

		sFile.Close();
	}

	/* 문서를 읽지 못하였어도 헤드 영역은 할당 */
	m_stVctPowerTable.push_back(stVctHead);
}

/*
 desc : 현재 내부 변수에 저장된 테이블을 문서로 남긴다.
 parm : 저장할 헤드 번호
 retn : None
*/
BOOL CPowerMeasureMgr::SaveHeadTableFile(UINT8 u8Head)
{
	CStdioFile sFile;	/* 문서작업용 */
	CString strPath;	/* 문서 경로 */
	CString strLine;	/* 문서의 줄 */
	CString strItem;	/* 문자열 작업용 임시 변수 */

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

	/* LED 표시란 parsing */
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

/* 측정이 완료되었다는 전재하에 진행이 되어야 한다. */
BOOL CPowerMeasureMgr::MeasureToPowerTable()
{
	if (0 >= (int)m_stVctMeasureTable.size())
	{
		/* 측정 테이블을 생성하지 않았을 경우 */
		return FALSE;
	}

	if (FALSE == BoundsCheckingPowerTable(m_u8Head, m_u8Led))
	{
		return FALSE;
	}

	// 경계 검사 완료

	/* 넣을 공간 초기화 */
	m_stVctPowerTable[m_u8Head - 1][m_u8Led - 1].clear();
	m_stVctPowerTable[m_u8Head - 1][m_u8Led - 1].shrink_to_fit();

	/* 측정 결과값 대입 */
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
 desc : X, Y 이동할 위치 계산
 parm : 기준 위치, 좌표 위치, 좌표계, 각 포인트 별 거리 값
 retn : 좌표값 기준 이동할 위치 값 반환
*/
ST_MOVE_POS CPowerMeasureMgr::CalcXYMovePos(double dStandardX, double dStandardY, UINT8 u8Row, UINT8 u8Col, UINT8 u8MaxPoint, double dPitch)
{
	ST_MOVE_POS stResult;	/* 반환할 좌표값 */
	UINT8 u8point;			/* 중심 좌표 값 (3x3, 5x5, 7x7 ... ) */

	/* 총 포인트 값의 제곱근은 가로, 세로의 포인트 값 */
	/* 해당 값의 절반은 중심 좌표 값 (시작 값이 0이기에 나머지는 버림처리를 한다.) */
	u8point = (UINT8)(sqrt(u8MaxPoint) / 2);

	stResult.dX = dStandardX - ((u8point - u8Row) * dPitch);
	stResult.dY = dStandardY - ((u8point - u8Col) * dPitch);

	return stResult;
}

/*
 desc : X, Y 이동
 parm : 이동할 위치 값, 속도
 retn : 이동 성공 유무
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
	// 에러 상태 확인
	if (TRUE == uvCmn_MC2_IsDriveError(ENG_MMDI::en_stage_x) ||
		TRUE == uvCmn_MC2_IsDriveError(ENG_MMDI::en_stage_y))
	{
		LOG_MESG(ENG_EDIC::en_led_power_cali, _T("STAGE ERROR"));
		return FALSE;
	}
	// Busy 신호 확인
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
		/* 이동 값의 최대 최소를 넘어가지 않는다. */
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
		/* 이동 값의 최대 최소를 넘어가지 않는다. */
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
 desc : X, Y 이동 확인
 parm : 이동해야 할 위치 값, 기다릴 시간 값, 이동 위치 오차 값
 retn : 이동이 완료되었는지 유무
*/
BOOL CPowerMeasureMgr::IsStageMovedXY(double dTargetX, double dTargetY, int nTimeOut/* = 60000*/, double dDiffDistance/* = 0.005*/)
{
	CTactTimeCheck cTime;			/* 타이머 */
	double dDiffX = 0.;				/* X위치 오차값 */
	double dDiffY = 0.;				/* Y위치 오차값 */

#ifdef MOTOR_SIMUL
	return TRUE;
#endif // MOTOR_SIMUL

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())
	{
		return TRUE;
	}

	/* 타이머 초기화 및 시작 */
	cTime.Init();
	cTime.Start();

	/* 일정 시간동안 동작 */
	while (nTimeOut > (int)cTime.GetTactMs())
	{
		if (TRUE == m_bStop)
		{
			LOG_MESG(ENG_EDIC::en_led_power_cali, _T("STOP"));
			return FALSE;
		}

		/* 타겟 위치와 현재위치의 오차를 구한다. */
		dDiffX = dTargetX - uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);
		dDiffY = dTargetY - uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);

		/* Motor가 타겟위치에 도달하고 정지 상태일 경우 종료 */
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

	/* 시간 초과 */
	LOG_MESG(ENG_EDIC::en_led_power_cali, _T("TIME OUT"));
	return FALSE;
}

/*
 desc : 포토 헤드 Z 이동
 parm : 헤드 번호, 이동할 위치 값, 속도
 retn : 이동 성공 유무
*/
BOOL CPowerMeasureMgr::HeadMoveZ(UINT8 u8Head, double dZ, double dSpeed)
{
	UINT8 u8ConvHeadNo = (UINT8)ENG_MMDI::en_axis_ph1 + (u8Head - 1);	/* Head 번호 계산 */
	CString strLog;														/* 로그용 문자열 */
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

	// 에러 상태 확인
	if (TRUE == uvCmn_MC2_IsDriveError(ENG_MMDI(u8ConvHeadNo)))
	{
		strLog.Format(_T("HEAD%d ERROR"), u8Head);
		LOG_MESG(ENG_EDIC::en_led_power_cali, (TCHAR*)(LPCTSTR)strLog);
		return FALSE;
	}
	// Busy 신호 확인
	if (TRUE == uvCmn_MC2_IsDriveBusy(ENG_MMDI(u8ConvHeadNo)))
	{
		strLog.Format(_T("HEAD%d BUSY"), u8Head);
		LOG_MESG(ENG_EDIC::en_led_power_cali, (TCHAR*)(LPCTSTR)strLog);
		return FALSE;
	}

	/* 이동 값의 최대 최소를 넘어가지 않는다. */
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
 desc : 포토 헤드 Z 이동 확인
 parm : 헤드 번호, 이동해야 할 위치 값, 기다릴 시간 값, 이동 위치 오차 값
 retn : 이동이 완료되었는지 유무
*/
BOOL CPowerMeasureMgr::IsHeadMovedZ(UINT8 u8Head, double dTargetZ, int nTimeOut/* = 60000*/, double dDiffDistance/* = 0.005*/)
{
	UINT8 u8ConvHeadNo = (UINT8)ENG_MMDI::en_axis_ph1 + (u8Head - 1);	/* Head 번호 계산 */
	CTactTimeCheck cTime;												/* 타이머 */
	double dDiffZ = 0.;													/* Z위치 오차값 */

#ifdef MOTOR_SIMUL
	return TRUE;
#endif // MOTOR_SIMUL

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())
	{
		return TRUE;
	}

	/* 타이머 초기화 및 시작 */
	cTime.Init();
	cTime.Start();
	
	/* 일정 시간동안 동작 */
	while (nTimeOut > (int)cTime.GetTactMs())
	{
		if (TRUE == m_bStop)
		{
			LOG_MESG(ENG_EDIC::en_led_power_cali, _T("STOP"));
			return FALSE;
		}

		/* 타겟 위치와 현재위치의 오차를 구한다. */
		dDiffZ = dTargetZ - uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(u8ConvHeadNo));

		/* Motor가 타겟위치에 도달하고 정지 상태일 경우 종료 */
		if (FALSE == uvCmn_MC2_IsDriveBusy(ENG_MMDI(u8ConvHeadNo)) &&
			fabs(dDiffZ) < dDiffDistance)
		{
			return TRUE;
		}

		cTime.Stop();
		Sleep(1);
	}

	/* 시간 초과 */
	LOG_MESG(ENG_EDIC::en_led_power_cali, _T("TIME OUT"));
	return FALSE;
}

/*
 desc : 포토 헤드 Z 이동을 확인하면서 조도 측정
 parm : 헤드 번호, 이동해야 할 위치 값, 최대 조도값을 저장할 변수, 데이터 추출 시간, 기다릴 시간 값, 이동 위치 오차 값
 retn : 이동이 완료되었는지 유무
*/
BOOL CPowerMeasureMgr::HeadMovedZWhileMeasure(UINT8 u8Head, double dTargetZ, double& dZ, int nSamplingTime/* = 10*/, int nTimeOut/* = 60000*/, double dDiffDistance/* = 0.005*/)
{
	UINT8 u8ConvHeadNo = (UINT8)ENG_MMDI::en_axis_ph1 + (u8Head - 1);	/* Head 번호 계산 */
	CTactTimeCheck cTime;												/* 타이머 */
	double dCurPos = 0.;												/* 현재 위치값 */
	double dReadData = 0.;												/* 읽은 조도값 */
	double dMaxPower = 0.;												/* 최대 조도값 */
	int nOldTime = 0;													/* 과거 시간 */

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())
	{
		return TRUE;
	}

	/* 타이머 초기화 및 시작 */
	cTime.Init();
	cTime.Start();

	/* 파워미터 초기화 후 Read 동작 시작 */
	uvEng_Gentec_ResetValue();
	uvEng_Gentec_RunMeasure(TRUE);

	/* 초기 데이터 입력 */
	cTime.Stop();
	dMaxPower = dReadData = uvEng_Gentec_GetCurPower();
	nOldTime = (int)cTime.GetTactMs();
	dZ = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(u8ConvHeadNo));

#ifdef LED_SIMUL
	nTimeOut = (int)(nTimeOut * 0.1); // Conv 6 sec
#endif // LED_SIMUL

	/* 일정 시간동안 동작 */
	while (nTimeOut > (int)cTime.GetTactMs())
	{
		if (TRUE == m_bStop)
		{
			LOG_MESG(ENG_EDIC::en_led_power_cali, _T("STOP"));
			return FALSE;
		}

		dCurPos = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(u8ConvHeadNo));

		/* Motor가 타겟위치에 도달하고 정지 상태일 경우 종료 */
		if (FALSE == uvCmn_MC2_IsDriveBusy(ENG_MMDI(u8ConvHeadNo)) &&
			fabs(dTargetZ - dCurPos) < dDiffDistance)
		{
			uvEng_Gentec_RunMeasure(FALSE);
			return TRUE;
		}

		/* 추출 시간만큼 지났을 경우 */
		if ((int)cTime.GetTactMs() - nOldTime >= nSamplingTime)
		{
			dReadData = uvEng_Gentec_GetCurPower();

			/* 읽은 값이 최대값보다 클경우 값 갱신 */
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

	/* 시간 초과 */
	uvEng_Gentec_RunMeasure(FALSE);
	LOG_MESG(ENG_EDIC::en_led_power_cali, _T("TIME OUT"));
	return FALSE;
}


/*
 desc : 진행 단계를 백분율로 변환한다.
 parm : 현재 진행 단계
 retn : 현재 진행 단계 백분율
*/
int CPowerMeasureMgr::GetProgressPercent(UINT16 u16Index)
{
	UINT uMaxIndex = 0;		/* 최대 진행 순서 */
	int nProgress = 0;		/* 결과값(백분율) */
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
 desc : 조도값 테이블 설정
 parm : 측정 횟수, 시작 인덱스 값, 증가할 인덱스 값
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
 desc : 포토 헤드 옵션 설정
 parm : 헤드 번호, LED 번호
 retn : None
*/
VOID CPowerMeasureMgr::SetPhotoHeadOption(UINT8 u8Head, UINT8 u8Led)
{
	/* 포토헤드 옵션을 변경하면 측정값 또한 초기화된다. */
	m_stVctMeasureTable.clear();
	m_stVctMeasureTable.shrink_to_fit();

	m_u8Head = u8Head;			/* 선택된 PhotoHead */
	m_u8Led = u8Led;			/* 선택된 LED */
}

/*
 desc : 최대 조도 위치 설정 값 변경
 parm : 측정 최대 포인트의 수, 각 포인트들 사이의 거리 값
 retn : None
*/
VOID CPowerMeasureMgr::SetFindPosParam(UINT8 u8MaxPoints, double dPitchXY, double dPitchZ)
{
	m_u8MaxPoints = u8MaxPoints;	/* 측정 최대 포인트의 수 */
	m_dPitchXY = dPitchXY;			/* 각 포인트들 사이의 거리 값 */
	m_dPitchZ = dPitchZ;			/* 각 포인트들 사이의 거리 값 */
}

/*
 desc : 포토 헤드 LED 동작
 parm : 헤드 번호, LED 번호, 조도 인덱스 값, 끄고 켜고 유무
 retn : 동작이 완료되었는지 유무
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
	
	//이미지 적재
	/* 포토 헤드에 새로운 이미지 번호 적재 요청 */
	//xLuria_ReqSetLedPowerCalibration(HeadNo, 0x02, TRUE);
	if (FALSE == uvEng_Luria_ReqSetLoadInternalTestImage(u8Head, 0x03))	return FALSE;
	/* 포토 헤드에 기존 적재되어 있는 이미지 번호 요청 */
	//xLuria_ReqGetLedPowerCalibration(HeadNo);

	/* LED Power 값 설정 */
	//xLuria_ReqSetLedPower(HeadNo,	(UINT16)m_edt_led[0].GetLedNo(),(UINT16)m_edt_num[0].GetTextToNum());

	//DMD on/off
	/* 포토 헤드의 특정 LED ON or OFF 설정 */
	//xLuria_ReqSetLedLightOnOff(HeadNo, UINT16(m_edt_led[0].GetLedNo()), onoff);

	/* 포토 헤드의 특정 LED 상태 (ON or OFF) 요청 */
	if (FALSE == uvEng_Luria_ReqGetLedLightOnOff(u8Head, ENG_LLPI(u8Led)))	return FALSE;
	Wait(100);

	return TRUE;
}

/*
 desc : 모든 포토 헤드 LED Off
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
 desc : 설정된 X, Y 위치에서 조도값이 가장 큰 위치로 이동
 parm : UI 윈도우 핸들
 retn : 동작이 완료되었는지 유무
※ 좌표는 현재 위치를 기반으로 생성하기에 필히 먼저 이동 후 호출해야 한다.
*/
BOOL CPowerMeasureMgr::FindMaxIllumPos(HWND hHwnd/* = NULL*/)
{
	LPG_CLPI pParam = &uvEng_GetConfig()->led_power;
	CString* pstrText = nullptr;				/* 윈도우 메시지 전달용 포인터 */
	CString strText;							/* UI 전달용 문자열 */
	BOOL bForward = FALSE;						/* 정방향 */
	BOOL bColMove = FALSE;						/* 세로 이동 */
	UINT8 u8MaxMoveCount = 0;					/* 최대 움직을 수 */
	UINT8 u8CurMoveCount = 0;					/* 현재 움직인 수 */
	UINT8 u8Row, u8Col;							/* 행(가로), 열(세로) */
	UINT8 u8MaxPowerIndex = 0;					/* 최대값이 측정된 순번 */
	ST_MOVE_POS stStartMovePos;					/* 시작 위치 구조체 (기준점) */
	ST_MOVE_POS stMovePos;						/* 이동할 위치용 구조체 */
	std::vector<ST_MEASURE_PARAM> stVctTable;	/* 측정 결과 테이블 */
	ST_MEASURE_PARAM stMeasure;					/* 테이블 저장용 임시 변수 */
// 	CString strPostMsg;							/* UI 전시용 문자열 */
// 	int nPostProgress;							/* UI 전시용 진행도 */

	/* 시작 Point 계산 */
	u8Col = u8Row = (UINT8)(sqrt(m_u8MaxPoints) / 2);

	/* 시작(기준) 위치 저장 */
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

	/* 최대 포인트만큼 반복 */
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

		// 첫 시작 시 별도 계산을 수행하지는 않는다.
		if (0 != u8Index)
		{
			// 최대 이동 거리 만큼 이동한다면 초기화
			if (u8MaxMoveCount <= u8CurMoveCount)
			{
				u8CurMoveCount = 0;
				bColMove = !bColMove;		// 이동 방향 변경 (가로, 세로)

				if (TRUE == bColMove)		// 세로 이동으로 결정되었을 시
				{
					bForward = !bForward;	// 이동 방향 변경 (정방향, 역방향)
					u8MaxMoveCount++;		// 최대 이동 위치 증가
				}
			}

			if (TRUE == bColMove)
			{
				// 세로 이동
				u8Col = (TRUE == bForward) ? (u8Col + 1) : (u8Col - 1);
			}
			else
			{
				// 가로 이동
				u8Row = (TRUE == bForward) ? (u8Row + 1) : (u8Row - 1);
			}

			u8CurMoveCount++;				// 현재 이동 위치 증가
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

		// 최댓값 갱신
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
	
	/* 이동해야할 위치 계산 */
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

	/* 해당 위치 저장 요청 */
	::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_XY_POS, (WPARAM)&stMovePos.dX, (LPARAM)&stMovePos.dY, SMTO_NORMAL, 500, NULL);

	return TRUE;
}

/*
 desc : 설정된 Z 위치에서 조도값이 가장 큰 위치로 이동
 parm : UI 윈도우 핸들
 retn : 동작이 완료되었는지 유무
※ 좌표는 현재 위치를 기반으로 생성하기에 필히 먼저 이동 후 호출해야 한다.
*/
BOOL CPowerMeasureMgr::FindFocusPos(HWND hHwnd/* = NULL*/)
{
	LPG_CLPI pParam = &uvEng_GetConfig()->led_power;
	CString* pstrText = nullptr;	/* 윈도우 메시지 전달용 포인터 */
	CString strText;				/* UI 전달용 문자열 */
	double dStandardZ = 0.;			/* 기준 위치 */
	double dTargetZ = 0.;			/* 이동할 위치 */
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
	// Speed는 입력을 받아야 할지 고민
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

	/* 값이 안정화 될 때까지 대기 */
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

	/* 해당 위치 저장 요청 */
	::SendMessageTimeout(hHwnd, eMSG_UV_POWER_INPUT_Z_POS, (WPARAM)&dTargetZ, NULL, SMTO_NORMAL, 500, NULL);

	return TRUE;
}

/*
 desc : 설정된 Z 위치에서 조도값이 가장 큰 위치로 이동
 parm : UI 윈도우 핸들
 retn : 동작이 완료되었는지 유무
※ 좌표는 현재 위치를 기반으로 생성하기에 필히 먼저 이동 후 호출해야 한다.
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
		// 20230731 mhbaek Modify : 선택한 LED에서 부터 시작하도록 수정
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
 desc : 멤버변수로 설정된 레시피에 해당 헤드의 모든 파장 파워 값 측정
 parm : 측정 헤드, UI 윈도우 핸들
 retn : 동작이 완료되었는지 유무
※ 좌표는 현재 위치를 기반으로 생성하기에 필히 먼저 이동 후 호출해야 한다.
*/
BOOL CPowerMeasureMgr::RecipeMeasure(int nHead, HWND hHwnd/* = NULL*/)
{
	LPG_PLPI pstPowerIist = nullptr;
	int nValue = 0;

	if (0 > m_nRecipeNum)
	{
		// 레시피를 지정하지 않은 채 실행
		return FALSE;
	}

	pstPowerIist = uvEng_LedPower_GetLedPowerIndex(m_nRecipeNum);

	if (nullptr == pstPowerIist)
	{
		// 레시피 불러오기 실패
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
 desc : 측정 프로세스
 parm : UI 윈도우 핸들
 retn : 동작이 완료되었는지 유무
 cmnt : 내부 대기동작으로 인해 쓰레드를 제외한 UI에서 호출을 금지한다.
*/
BOOL CPowerMeasureMgr::Measurement(HWND hHwnd)
{
	LPG_CLPI pParam = &uvEng_GetConfig()->led_power;
	CString* pstrText = nullptr;
	CString strText;
	int nMeasureMaxCnt = 1;
	int nStartIdx = 0;

	/* 정지 플래그 초기화 */
	m_bStop = FALSE;
	m_u16Index = 0;

	m_u8StartHead = m_u8Head;
	m_u8StartLed = m_u8Led;

	if (TRUE == m_bUseAllPH)
	{
		// 20230731 mhbaek Modify : 선택한 Head에서 부터 시작하도록 수정
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
	
	/* 정지 플래그 초기화 */
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

		if(timeGetTime() - tick < COOLDOWN_TIME) //최소 5초의 쿨다운타임이 필요.
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
 desc : 측정 후 테이블 값 교체 후 저장
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
 desc : 측정 쓰레드 실행
 parm : UI 윈도우 핸들
 retn : 시작 성공, 시작 실패
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
 desc : 레시피 점검 쓰레드 실행
 parm : UI 윈도우 핸들
 retn : 시작 성공, 시작 실패
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
 desc : 인덱스 값 추출
 parm : 헤드, LED 번호, 원하는 조도 값
 retn : 인덱스 값
*/
UINT16 CPowerMeasureMgr::GetPowerIndex(UINT8 u8Head, UINT8 u8Led, double dPower)
{
	VCT_LED_TABLE* pVctTable;	/* 테이블 관련 포인터 */
	int nApproxIndex = 0;		/* 근사치 순서 값 */
	int nStartIndex = 0;		/* 계산 시작할 순서 값 */
	double dMinValue = dPower;	/* 최소 오차값 */

	
	/* 경계 검사 */
	if (FALSE == BoundsCheckingPowerTable(u8Head, u8Led))
	{
		return -1;
	}

	/* 포인터 지정 */
	pVctTable = &m_stVctPowerTable[u8Head - 1][u8Led - 1];

	// 6um Text
// 	if (pVctTable->begin()->dPower > dPower ||
// 		pVctTable->end()->dPower < dPower) //현재 스텝내에서 처리할수 없는 상황이다. 
// 	{
// 		return 0;
// 	}

	if (eLAGRANGE_DATA_COUNT + 1 > (int)pVctTable->size())
	{
		return 0;
	}

	for (int i = 0; i < (int)pVctTable->size(); i++)
	{	
		/* 만약 테이블 내 동일한 값이 존재한다면 반환 */
		if (dPower == pVctTable->at(i).dPower)
		{
			return pVctTable->at(i).u16Index;
		}
		/* 원하는 조도값과 근사치에 가까울 때 갱신 */
		if (dMinValue > fabs(dPower - pVctTable->at(i).dPower))
		{
			dMinValue = fabs(dPower - pVctTable->at(i).dPower);
			nApproxIndex = i;
		}
	}
	
	/* 제일 마지막 값이 근사치일 경우 */
	if (nApproxIndex == (int)pVctTable->size() - 1)
	{
		nStartIndex = nApproxIndex - eLAGRANGE_DATA_COUNT;
	}
	/* Normal Case */
	else if(0 != nApproxIndex)
	{
		nStartIndex = nApproxIndex - (int)(eLAGRANGE_DATA_COUNT * 0.5);
	}
	/* 시작값이 제일 근사치일 경우 nStartIndex = 0 */

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

	/* 그려진 곡선이 정상 범주 안 */
	if ((UINT16)dSum <= pVctTable->at(nStartIndex + eLAGRANGE_DATA_COUNT - 1).u16Index)
	{
		return (UINT16)dSum;
	}

	nApproxIndex = (0 <= nApproxIndex) ? 1 : nApproxIndex;

	/* 라그랑주 적용 시 그러진 곡선이 경사가 급할 경우 비례식으로 이용 */
	double dSlope = (pVctTable->at(nApproxIndex).u16Index - pVctTable->at(nApproxIndex - 1).u16Index) / (pVctTable->at(nApproxIndex).dPower - pVctTable->at(nApproxIndex - 1).dPower);
	double dTempY = pVctTable->at(nApproxIndex - 1).u16Index - dSlope * pVctTable->at(nApproxIndex - 1).dPower;

	return UINT16(dSlope * dPower + dTempY);
}


double CPowerMeasureMgr::GetStableValue(UINT8 u8Led, double dAccuracy/* = 0.005*/, int nTimeOut/* = 60000*/)
{
	CTactTimeCheck cTime;				/* 타이머 */
	std::vector<double> dVctTable;
	double dAvr = 0.;
	double dMin = 9999.;
	double dMax = -9999.;
	double dSum = 0.;
	LPG_PGDV stReadData;

	/* 타이머 초기화 및 시작 */
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
		
		/* Vector Size는 최대 10개로 제한한다. */
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
 desc : 측정 쓰레드
 parm : UI 윈도우 핸들
 retn : 0
*/
UINT PowerMeasureThread(LPVOID lpParam)
{
	HWND hHwnd = (HWND)lpParam;

	CPowerMeasureMgr::GetInstance()->Measurement(hHwnd);

	return 0;
}

/*
 desc : 레시피 점검 쓰레드
 parm : UI 윈도우 핸들
 retn : 0
*/
UINT RecipeCheckThread(LPVOID lpParam)
{
	HWND hHwnd = (HWND)lpParam;

	CPowerMeasureMgr::GetInstance()->CheckRecipe(hHwnd);

	return 0;
}