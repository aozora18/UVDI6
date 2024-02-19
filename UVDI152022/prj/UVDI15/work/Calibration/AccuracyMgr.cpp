
/*
 desc : 시나리오 기본 (Base) 함수 모음
*/

#include "pch.h"
#include "AccuracyMgr.h"
#include "../../MainApp.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif

bool _SortX(ST_ACCR_PARAM& tagA, ST_ACCR_PARAM& tagB)
{
	return tagA.dMotorX < tagB.dMotorX;
}

bool _SortY(ST_ACCR_PARAM& tagA, ST_ACCR_PARAM& tagB)
{
	return tagA.dMotorY < tagB.dMotorY;
}

/*
 desc : 생성자
 parm : None
 retn : None
*/
CAccuracyMgr::CAccuracyMgr()
{
	m_bStop = FALSE;
	m_bUseCalData = FALSE;
	m_bUseCamDrv = FALSE;
	m_u8ACamID = 0;
	m_nStartIndex = 0;
}

/*
 desc : 소멸자
 parm : None
 retn : None
*/
CAccuracyMgr::~CAccuracyMgr()
{
	m_pMeasureThread = NULL;
	delete m_pMeasureThread;
}

/*
 desc : 초기화
 parm : None
 retn : None
*/
VOID CAccuracyMgr::Initialize()
{
}

/*
 desc : 종료 시 호출
 parm : None
 retn : None
*/
VOID CAccuracyMgr::Terminate()
{
	m_bStop = TRUE;
	if (m_pMeasureThread != NULL)
	{
		WaitForSingleObject(m_pMeasureThread->m_hThread, INFINITE);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VOID CAccuracyMgr::SortField(VCT_ACCR_TABLE& stVctField)
{
	double dX = 0, dY = 0;

	std::sort(stVctField.begin(), stVctField.end(), _SortY);

	for (int i = 0; i < (int)stVctField.size() - 1; i++)
	{
		for (int j = 0; j < (int)stVctField.size() - 1 - i; j++)
		{
			if (DEF_DIFF_POINTS >= fabs(stVctField[j].dMotorY - stVctField[j + 1].dMotorY))
			{
				if (stVctField[j].dMotorX > stVctField[j + 1].dMotorX)
				{
					dX = stVctField[j].dMotorX;
					dY = stVctField[j].dMotorY;

					stVctField[j].dMotorX = stVctField[j + 1].dMotorX;
					stVctField[j].dMotorY = stVctField[j + 1].dMotorY;

					stVctField[j + 1].dMotorX = dX;
					stVctField[j + 1].dMotorY = dY;
				}
			}
		}
	}
}

ST_FIELD CAccuracyMgr::ClacField(VCT_ACCR_TABLE stVctField)
{
	ST_FIELD stFieldData;
	double dCurValue = 0;
	UINT32	u32Col = 1;

	std::sort(stVctField.begin(), stVctField.end(), _SortY);

	for (int i = 0; i < (int)stVctField.size(); i++)
	{
		if (DEF_DIFF_POINTS <= fabs(dCurValue - stVctField[i].dMotorY))
		{
			stFieldData.u32Row++;

			stFieldData.u32Col = (u32Col > stFieldData.u32Col) ? u32Col : stFieldData.u32Col;
			u32Col = 1;
		}
		else
		{
			u32Col++;
		}

		dCurValue = stVctField[i].dMotorY;
	}

	return stFieldData;
}

BOOL CAccuracyMgr::LoadMeasureField(CString strPath)
{
	CStdioFile sFile;
	CString strLine;
	CString strValue;

	ST_ACCR_PARAM stValue;

	m_stVctTable.clear();
	m_stVctTable.shrink_to_fit();

	if (TRUE == sFile.Open(strPath, CStdioFile::shareDenyNone | CStdioFile::modeRead))
	{
		while (sFile.ReadString(strLine))
		{
			// Parsing
			AfxExtractSubString(strValue, strLine, 0, _T(','));
			stValue.dMotorX = _ttof(strValue);
			AfxExtractSubString(strValue, strLine, 1, _T(','));
			stValue.dMotorY = _ttof(strValue);

			if (TRUE == strValue.IsEmpty())
			{
				break;
			}

			m_stVctTable.push_back(stValue);
		}

		sFile.Close();
		return TRUE;
	}

	return FALSE;
}
 

BOOL CAccuracyMgr::MakeMeasureField(CString strPath, CDPoint dpStartPos, UINT8 u8StartPoint, UINT8 u8Dir, double dAngle, double dPitch, CPoint cpMaxPoint)
{
	CFileException ex;
	CStdioFile sFile;
	CString strWrite;
	CString strLine;
	CDPoint dpPos;
	int nMaxIndex = cpMaxPoint.x * cpMaxPoint.y;

	for (int i = 0; i < nMaxIndex; i++)
	{
		dpPos = CalcPosition(dpStartPos, dAngle, dPitch, CalcPoint(i, u8StartPoint, u8Dir, cpMaxPoint));
		strLine.Format(_T("%.4f,%.4f,\n"), dpPos.x, dpPos.y);

		strWrite += strLine;
	}

	if (TRUE == sFile.Open(strPath, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate, &ex))
	{
		sFile.SeekToBegin();
		sFile.WriteString(strWrite);
		sFile.Close();
		return TRUE;
	}

	return FALSE;
}


BOOL CAccuracyMgr::SaveCaliFile(CString strFileName)
{
	if (0 >= (int)m_stVctTable.size())
	{
		return FALSE;
	}

	VCT_ACCR_TABLE stVctField = m_stVctTable;
	ST_FIELD stFieldData = ClacField(stVctField);
	CFileException ex;
	CStdioFile sFile;
	CString strWrite;
	CString strLine;
	double dACamPosX = 0.0f, dStagePosY = 0.0f;

	SortField(stVctField);

	/* 환경 파일 */
	strWrite.Format(_T("0.0000\n0.0000\n0.0000\n%d\n%d\n"), stFieldData.u32Row, stFieldData.u32Col);

	for (int i = 0; i < (int)stVctField.size(); i++)
	{
		if (TRUE == m_bUseCalData)
		{
			uvEng_ACamCali_GetCaliPosEx(stVctField[i].dMotorX, stVctField[i].dMotorY, dACamPosX, dStagePosY);
		}

		strLine.Format(_T(" %+.4f, %+.4f, %+.4f, %+.4f,\n"),
			stVctField[i].dMotorX, stVctField[i].dMotorY,
			 dACamPosX + stVctField[i].dValueX, dStagePosY + stVctField[i].dValueY);

		strWrite += strLine;
	}

	if (TRUE == sFile.Open(strFileName, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate, &ex))
	{
		sFile.SeekToBegin();
		sFile.WriteString(strWrite);
		sFile.Close();
		return TRUE;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


BOOL CAccuracyMgr::Measurement(HWND hHwnd/* = NULL*/)
{
	LPG_CIEA pstCfg = uvEng_GetConfig();
	TCHAR tzMesg[128] = { NULL };
	STG_ACGR stGrab;
	BOOL bRunState = TRUE;

	m_bStop = FALSE;

	if (TRUE == m_bUseCamDrv)
	{
		ENG_MMDI enXDrv = (0 == m_u8ACamID) ? ENG_MMDI::en_align_cam2 : ENG_MMDI::en_align_cam1;
		int nIndex = (int)enXDrv - (int)ENG_MMDI::en_align_cam1;

		// 회피 위치로 이동
		Move(enXDrv, pstCfg->set_cams.safety_pos[nIndex]);
	}

	LOG_MESG(ENG_EDIC::en_2d_cali, _T("[2DCal]\tSTART"));

	for (int nWorkStep = m_nStartIndex; nWorkStep < (int)m_stVctTable.size(); nWorkStep++)
	{
		if (TRUE == m_bStop)
		{
			return FALSE;
		}

		if (FALSE == MotionCalcMoving(m_stVctTable[nWorkStep].dMotorX, m_stVctTable[nWorkStep].dMotorY))
		{
			LOG_MESG(ENG_EDIC::en_2d_cali, _T("Move Fail"));

			if (NULL != hHwnd)
			{
				// Grab View 갱신
				::SendMessageTimeout(hHwnd, eMSG_ACCR_MEASURE_REPORT, (WPARAM)e2DCAL_REPORT_MOTION_FIAL, NULL, SMTO_NORMAL, 100, NULL);
			}

			m_bRunnigThread = FALSE;
			return FALSE;
		}

		Wait(3000);

		if (FALSE == GrabData(stGrab, FALSE, 5))
		{
			LOG_MESG(ENG_EDIC::en_2d_cali, _T("Grab Fail"));

			if (NULL != hHwnd)
			{
				// Grab View 갱신
				::SendMessageTimeout(hHwnd, eMSG_ACCR_MEASURE_REPORT, (WPARAM)e2DCAL_REPORT_GRAB_FIAL, NULL, SMTO_NORMAL, 100, NULL);
			}

			m_bRunnigThread = FALSE;
			return FALSE;
		}

		SetPointErrValue(nWorkStep, stGrab.move_mm_x, stGrab.move_mm_y);

		if (NULL != hHwnd)
		{
			// Grab View 갱신
			::SendMessageTimeout(hHwnd, eMSG_ACCR_MEASURE_GRAB, (WPARAM)nWorkStep, NULL, SMTO_NORMAL, 100, NULL);
		}

		swprintf_s(tzMesg, 128, L"%.4f,%.4f,\t%.4f,%.4f",
			m_stVctTable[nWorkStep].dMotorX,
			m_stVctTable[nWorkStep].dMotorY,
			stGrab.move_mm_x,
			stGrab.move_mm_y);

		LOG_MESG(ENG_EDIC::en_2d_cali, tzMesg);
		Wait(500);
	}

	// 동작 완료
	LOG_MESG(ENG_EDIC::en_2d_cali, _T("Successful 2D Cal"));

	if (NULL != hHwnd)
	{
		// Grab View 갱신
		::SendMessageTimeout(hHwnd, eMSG_ACCR_MEASURE_REPORT, (WPARAM)e2DCAL_REPORT_OK, NULL, SMTO_NORMAL, 100, NULL);
	}

	m_bRunnigThread = FALSE;
	return TRUE;
}

/*
 desc : 새로운 검색을 위해 검색 대상의 모델 등록
 parm : None
 retn : Non
*/
BOOL CAccuracyMgr::SetRegistModel()
{
	UINT32 u32Model = (UINT32)ENG_MMDT::en_circle, i = 0;
	DOUBLE dbMSize = 2.0f /* um */, dbMColor = uvEng_GetConfig()->acam_spec.in_ring_color /* 256:Black, 128:White */;
	LPG_CIEA pstCfg = uvEng_GetConfig();

	/* 검색 대상에 대한 모델 등록 */
	dbMSize = pstCfg->acam_spec.in_ring_size * 1000.0f;
	dbMSize = 1000;
	for (i = 0; i < pstCfg->set_cams.acam_count; i++)
	{
		if (!uvEng_Camera_SetModelDefineEx(i + 1,
			pstCfg->mark_find.model_speed,
			pstCfg->mark_find.detail_level,
			pstCfg->mark_find.max_mark_find,
			pstCfg->mark_find.model_smooth,
			&u32Model, &dbMColor, &dbMSize,
			NULL, NULL, NULL, TMP_MARK,
			pstCfg->acam_spec.in_ring_scale_min,
			pstCfg->acam_spec.in_ring_scale_max))
		{
			return FALSE;
		}
		else
		{
				uvCmn_Camera_SetMarkFindMode(i + 1, 0, TMP_MARK); // CALIB는 직접 만든 MARK 사용(MOD)
		}
	}

	return TRUE;
}

BOOL CAccuracyMgr::SetPointErrValue(int nIndex, double dX, double dY)
{
	if (nIndex >= (int)m_stVctTable.size() || nIndex < 0)
	{
		return FALSE;
	}

	m_stVctTable[nIndex].dValueX = dX;
	m_stVctTable[nIndex].dValueY = dY;
	return TRUE;
}

BOOL CAccuracyMgr::GetPointErrValue(int nIndex, double& dX, double& dY)
{
	if (nIndex >= (int)m_stVctTable.size() || nIndex < 0)
	{
		return FALSE;
	}

	dX = m_stVctTable[nIndex].dValueX;
	dY = m_stVctTable[nIndex].dValueY;
	return TRUE;
}

BOOL CAccuracyMgr::GetPointPosition(int nIndex, double& dX, double& dY)
{
	if (nIndex >= (int)m_stVctTable.size() || nIndex < 0)
	{
		return FALSE;
	}

	dX = m_stVctTable[nIndex].dMotorX;
	dY = m_stVctTable[nIndex].dMotorY;
	return TRUE;
}

BOOL CAccuracyMgr::GetPointParam(int nIndex, ST_ACCR_PARAM& stParam)
{
	if (nIndex >= (int)m_stVctTable.size() || nIndex < 0)
	{
		return FALSE;
	}

	stParam = m_stVctTable[nIndex];
	return TRUE;
}

/*
 desc : 입력한 값만큼 대기동작을 수행한다.
 parm : 대기 시간(ms)
 retn : None
*/
VOID CAccuracyMgr::Wait(int nTime)
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


CPoint CAccuracyMgr::CalcPoint(UINT32 u32Index, UINT8 u8StartPoint, UINT8 u8Dir, CPoint cpMaxGridSize)
{
	CPoint cpReturnPoint(-1, -1);
	CPoint cpTempMax = cpMaxGridSize;

	// X 방향이 아니라면 X와 Y의 값을 서로 변경한다.
	if (eCAL_PARAM_DIR_X != u8Dir)
	{
		cpTempMax.SetPoint(cpMaxGridSize.y, cpMaxGridSize.x);
	}

	// 지그재그 방식의 공용 계산
	cpReturnPoint.y = (LONG)u32Index / cpTempMax.x;
	cpReturnPoint.x = (0 != cpReturnPoint.y % 2) ? cpTempMax.x - ((LONG)u32Index % cpTempMax.x) - 1 : (LONG)u32Index % cpTempMax.x;

	if (eCAL_PARAM_DIR_X != u8Dir)
	{
		cpReturnPoint.SetPoint(cpReturnPoint.y, cpReturnPoint.x);
	}

	switch (u8StartPoint)
	{
	// Start Point : 0, 0
	case eCAL_PARAM_START_POINT_LB:
		break;
			
	// Start Point : 0, Y Max
	case eCAL_PARAM_START_POINT_LT:
		cpReturnPoint.y = cpMaxGridSize.y - cpReturnPoint.y - 1;
		break;
			
	// Start Point : X Max, 0
	case eCAL_PARAM_START_POINT_RB:
		cpReturnPoint.x = cpMaxGridSize.x - cpReturnPoint.x - 1;
		break;
			
	// Start Point : X Max, Y Max
	case eCAL_PARAM_START_POINT_RT:
		cpReturnPoint.x = cpMaxGridSize.x - cpReturnPoint.x - 1;
		cpReturnPoint.y = cpMaxGridSize.y - cpReturnPoint.y - 1;
		break;

	default:
		break;
	}

	// 파라미터 입력 오류일 시 -1의 값을 반환한다.
	return cpReturnPoint;
}

// Motor의 모든 값이 최소 지점이 0, 0이다. (Cal File 계산 시 오름차순 정렬 필요)
CDPoint CAccuracyMgr::CalcPosition(CDPoint dpStartPos, double dAngle, double dPitch, CPoint cpPoint)
{
	double dRadian = dAngle * (M_PI / 180.0f);
	CDPoint dpXMove;
	CDPoint dpYMove;
	CDPoint dpPos;

	dpXMove.x = cos(dRadian) * dPitch;
	dpXMove.y = sin(dRadian) * dPitch;

	dRadian = (dAngle + 90) * (M_PI / 180.0f);

	dpYMove.x = cos(dRadian) * dPitch;
	dpYMove.y = sin(dRadian) * dPitch;

	dpPos.x = dpStartPos.x + (dpXMove.x * cpPoint.x) + (dpYMove.x * cpPoint.y);
	dpPos.y = dpStartPos.y + (dpXMove.y * cpPoint.x) + (dpYMove.y * cpPoint.y);

	return dpPos;
}


BOOL CAccuracyMgr::Move(ENG_MMDI eMotor, double dPos, int nTimeOut, double dDiffDistance)
{
	CTactTimeCheck cTime;
	double dDiffPos = 0;
	double dVel = uvEng_GetConfig()->mc2_svc.step_velo;

	if (CInterLockManager::GetInstance()->CheckMoveInterlock(eMotor, dPos))
	{
		return FALSE;
	}

	if (FALSE == uvEng_MC2_SendDevAbsMove(eMotor, dPos, dVel)) return FALSE;
	Sleep(100);

#ifdef CAM_SPEC_SIMUL
	return TRUE;
#endif // CAM_SPEC_SIMUL

	/* 타이머 초기화 및 시작 */
	cTime.Init();
	cTime.Start();

	/* 일정 시간동안 동작 */
	while (nTimeOut > (int)cTime.GetTactMs())
	{
		if (TRUE == m_bStop)
		{
			return FALSE;
		}

		/* 타겟 위치와 현재위치의 오차를 구한다. */
		dDiffPos = dPos - uvCmn_MC2_GetDrvAbsPos(eMotor);

		/* Motor가 타겟위치에 도달하고 정지 상태일 경우 종료 */
		if (FALSE == uvCmn_MC2_IsDriveBusy(eMotor) &&
			fabs(dDiffPos) <= dDiffDistance)
		{
			return TRUE;
		}

		cTime.Stop();
		Sleep(1);
	}

	/* 시간 초과 */
	return FALSE;
}

BOOL CAccuracyMgr::MotionCalcMoving(double dMoveX, double dMoveY, int nTimeOut, double dDiffDistance)
{
	CTactTimeCheck cTime;
	UINT8 u8InstAngle = uvEng_GetConfig()->set_cams.acam_inst_angle; // 카메라가 설치될 때, 회전 여부 (0: 회전 없음, 1: 180도 회전)
	double dACamPosX = dMoveX, dStagePosY = dMoveY;
	double dDiffACamPos = 0, dDiffYPos = 0;
	double dVel = uvEng_GetConfig()->mc2_svc.step_velo;
	ENG_MMDI enXDrv;

	if (FALSE == m_bUseCamDrv)
	{
		enXDrv = ENG_MMDI::en_stage_x;
	}
	else
	{
		enXDrv = (0 == m_u8ACamID) ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
	}

	if (TRUE == m_bUseCalData)
	{
		if (!uvEng_ACamCali_GetCaliPosEx(dMoveX, dMoveY, dACamPosX, dStagePosY))
		{
			//swprintf_s(tzMesg, 128, L"The calibration position does not find from calibration file");
			return FALSE;
		}
		dACamPosX = dMoveX + (dACamPosX * -1.0);
		dStagePosY = dMoveY + (dStagePosY * -1.0);
	}

	if (CInterLockManager::GetInstance()->CheckMoveInterlock(enXDrv, dACamPosX))
	{
		return FALSE;
	}

	if (FALSE == uvEng_MC2_SendDevAbsMove(enXDrv, dACamPosX, dVel))	return FALSE;
	Sleep(100);

	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, dStagePosY))
	{
		return FALSE;
	}

	if (FALSE == uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dStagePosY, dVel)) return FALSE;
	Sleep(100);

#ifdef CAM_SPEC_SIMUL
	return TRUE;
#endif // CAM_SPEC_SIMUL

	/* 타이머 초기화 및 시작 */
	cTime.Init();
	cTime.Start();

	/* 일정 시간동안 동작 */
	while (nTimeOut > (int)cTime.GetTactMs())
	{
		if (TRUE == m_bStop)
		{
			return FALSE;
		}

		/* 타겟 위치와 현재위치의 오차를 구한다. */
		dDiffYPos = dStagePosY - uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);
		dDiffACamPos = dACamPosX - uvCmn_MC2_GetDrvAbsPos(enXDrv);

		/* Motor가 타겟위치에 도달하고 정지 상태일 경우 종료 */
		if (FALSE == uvCmn_MC2_IsDriveBusy(ENG_MMDI::en_stage_y) &&
			FALSE == uvCmn_MC2_IsDriveBusy(enXDrv) &&
			fabs(dDiffYPos) <= dDiffDistance &&
			fabs(dDiffACamPos) <= dDiffDistance)
		{
			return TRUE;
		}

		cTime.Stop();
		Sleep(1);
	}

	/* 시간 초과 */
	return FALSE;
}


/*
 desc : 현재 위치에서 Mark Image를 Grab !!!
 parm : 재반복 횟수
 retn : 정상 동작 시 TRUE, 실패 시 FALSE
*/
BOOL CAccuracyMgr::GrabData(STG_ACGR& stGrab, BOOL bRunMode, int nRetryCount)
{
	CTactTimeCheck cTime;
	UINT8 u8ChNo = (0 == m_u8ACamID) ? 0x01 : 0x02;
	UINT8 u8Mode = (TRUE == bRunMode) ? 0xFF : 0xFE;
	LPG_ACGR pstGrab = NULL;

	/* 기존 Live & Edge & Calibration 데이터 초기화 */
	uvEng_Camera_ResetGrabbedImage();

	/* Mark Model이 등록되어 있는지 여부 */
	if (!uvEng_Camera_IsSetMarkModelACam(u8ChNo, 2))
	{
		return FALSE;
	}

	/* 카메라 Grabbed Mode를 Calibration Mode로 동작 */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);

	for (int i = 0; i < nRetryCount; i++)
	{
		/* Trigger 1개 발생 */
		if (!uvEng_Mvenc_ReqTrigOutOne(u8ChNo, 0x00, FALSE))
		{
			return FALSE;
		}

		/* 타이머 초기화 및 시작 */
		cTime.Init();
		cTime.Start();

		while (3000 > (int)cTime.GetTactMs())
		{
			if (TRUE == m_bStop)
			{
				return FALSE;
			}

			/* Grabbed Image가 존재하는지 확인 */
			pstGrab = uvEng_Camera_RunModelCali(u8ChNo, u8Mode, (UINT8)DISP_TYPE_CALB_ACCR, TMP_MARK, TRUE, uvEng_GetConfig()->mark_find.image_process);	/* 이미지 중심으로 이동하기 때문에 무조건 0xff 값 */
			if (NULL != pstGrab && 0x00 != pstGrab->marked)
			{
				stGrab = *pstGrab;
				/* 다음 단계로 이동 */
				return TRUE;
			}

			cTime.Stop();
			Sleep(1);
		}
	}

	/* 시간 초과 */ 
	return FALSE;
}

/*
 desc : 측정 쓰레드 실행
 parm : UI 윈도우 핸들
 retn : None
*/
VOID CAccuracyMgr::MeasureStart(HWND hHwnd/* = NULL*/)
{
	m_bStop = FALSE;
	m_bRunnigThread = TRUE;
	m_pMeasureThread = AfxBeginThread(MeasureThread, (LPVOID)hHwnd);
}

/*
 desc : 측정 쓰레드
 parm : UI 윈도우 핸들
 retn : 0
*/
UINT MeasureThread(LPVOID lpParam)
{
	HWND hHwnd = (HWND)lpParam;

	CAccuracyMgr::GetInstance()->Measurement(hHwnd);

	return 0;
}