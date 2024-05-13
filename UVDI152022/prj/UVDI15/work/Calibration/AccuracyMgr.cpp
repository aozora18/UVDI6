
/*
 desc : �ó����� �⺻ (Base) �Լ� ����
*/

#include "pch.h"
#include "AccuracyMgr.h"
#include "../../MainApp.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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
 desc : ������
 parm : None
 retn : None
*/
CAccuracyMgr::CAccuracyMgr()
{
	m_bStop = FALSE;
	m_bUseCalData = FALSE;
	m_bUseCamDrv = FALSE;
	m_u8ACamID = 1;
	m_nStartIndex = 0;
}

/*
 desc : �Ҹ���
 parm : None
 retn : None
*/
CAccuracyMgr::~CAccuracyMgr()
{
	m_pMeasureThread = NULL;
	delete m_pMeasureThread;
}

/*
 desc : �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CAccuracyMgr::Initialize()
{
}

/*
 desc : ���� �� ȣ��
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

double degreesToRadians(double degrees)
{
	return degrees * (3.14159265358979323846f / 180.0f);
}

CDPoint rotatePointAroundAnotherPoint(CDPoint P, CDPoint anchorPos, double thetaRadians)
{


	double relativeX = P.x - anchorPos.x;
	double relativeY = P.y - anchorPos.y;

	double rotatedX = relativeX * cos(thetaRadians) - relativeY * sin(thetaRadians);
	double rotatedY = relativeX * sin(thetaRadians) + relativeY * cos(thetaRadians);

	return CDPoint(rotatedX + anchorPos.x, rotatedY + anchorPos.y);
}


BOOL CAccuracyMgr::MakeMeasureField(CString strPath, CDPoint dpStartPos, UINT8 u8StartPoint, UINT8 u8Dir, double dAngle, double dPitch, CPoint cpMaxPoint,bool toXDirection,bool turnBack)
{
	CFileException ex;
	CStdioFile sFile;
	CString strWrite;
	CString strLine;
	CDPoint dpPos;
	int nMaxIndex = cpMaxPoint.x   * cpMaxPoint.y ; 

	vector<CDPoint> buffer;


	double dRadian = degreesToRadians(dAngle);

	bool useTurnback = !turnBack;   //<--------------�� �κ��� true�϶� x��ǥ�� �ٽ� ���ҹ������� turnback �ϰ� �� ��, 0���� �����ؼ� 50���� ���ٸ� �ٽ� 45,40,35~ 0 ���� ���ƿ�, false�϶� 50���� ���ٸ� �ٽ� 0���� ���ƿ� 50���� ��. 
	bool turnBackFlag = false;

	buffer.push_back(dpStartPos);

	for (int i = 1; i < nMaxIndex; i++)
	{
		
		CDPoint tempPoint = toXDirection == true ?
				CDPoint((double)(dpStartPos.x + ((i % cpMaxPoint.x) * dPitch)),
					(double)(dpStartPos.y + ((i / cpMaxPoint.x) * dPitch))) :
				CDPoint((double)(dpStartPos.x + ((i / cpMaxPoint.y) * dPitch)),
					(double)(dpStartPos.y + ((i % cpMaxPoint.y) * dPitch)));

		CDPoint tempRotated = rotatePointAroundAnotherPoint(tempPoint, dpStartPos, dRadian);

		buffer.push_back(tempRotated);

		if (buffer.size() == (toXDirection == true ? cpMaxPoint.x : cpMaxPoint.y))
		{
			if (turnBackFlag)
				std::reverse(buffer.begin(), buffer.end());

			for each (CDPoint var in buffer)
			{
				strLine.Format(_T("%.4f,%.4f,\n"), var.x, var.y);
				strWrite += strLine;
			}

			turnBackFlag = useTurnback ? !turnBackFlag : turnBackFlag;
			buffer.clear();
		}
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

	/* ȯ�� ���� */
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
	stGrab.Init();
	BOOL bRunState = TRUE;

	m_bStop = FALSE;

	if (TRUE == m_bUseCamDrv)
	{

		ENG_MMDI enXDrv = (1 == m_u8ACamID) ? ENG_MMDI::en_align_cam2 : ENG_MMDI::en_align_cam1;

		int nIndex = (int)enXDrv - (int)ENG_MMDI::en_align_cam1;

		// ȸ�� ��ġ�� �̵�
		Move(enXDrv, pstCfg->set_cams.safety_pos[nIndex]);

	}

	LOG_MESG(ENG_EDIC::en_2d_cali, _T("[2DCal]\tSTART"));


	std::vector<std::function<void()>> caliTask =
	{ 
		[&]()
		{
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
						// Grab View ����
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
						// Grab View ����
						::SendMessageTimeout(hHwnd, eMSG_ACCR_MEASURE_REPORT, (WPARAM)e2DCAL_REPORT_GRAB_FIAL, NULL, SMTO_NORMAL, 100, NULL);
					}

					m_bRunnigThread = FALSE;
					return FALSE;
				}

				SetPointErrValue(nWorkStep, stGrab.move_mm_x, stGrab.move_mm_y);

				if (NULL != hHwnd)
				{
					// Grab View ����
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
		},
		[&]()
		{
			if (m_stVctTable.size() < 1) return;

			bool moveToX = m_stVctTable[0].dMotorX == m_stVctTable[1].dMotorX;
			double initialVal = moveToX ? m_stVctTable[0].dMotorX : m_stVctTable[0].dMotorY;
			auto it = std::find_if(m_stVctTable.begin(), m_stVctTable.end(), [&](const ST_ACCR_PARAM& v) {return moveToX ? initialVal != v.dMotorX : initialVal != v.dMotorY; });
			int axisCount = std::distance(m_stVctTable.begin(), it);
			int maxCycle = m_stVctTable.size() / axisCount;

			//�ϴ� ��ŸƮ ����Ʈ�� �����ؾ��ϴϱ�. 
			LPG_CIEA pstCfg = uvEng_GetConfig();

			for (int i = 0; i < maxCycle; i++)
			{
				auto initialAxisLoc = moveToX ? m_stVctTable[i * axisCount].dMotorX : m_stVctTable[i * axisCount].dMotorY; //������ǥ.
				pstCfg


			}

			int debug = 0;
			/*
			Ķ������ ù���� �����ͼ� ���Ѵ�.
			ù�ٰ� ������ �������� XY�� ���Ѵ�. 
			X,Y�� ���� ���� ������ �̵����̴�. �� SP(100,10) EP(500,10) �̶�� Y �� �������� �����̴°��̴�. 

			�׸��� ���� �����ͷ� SP�� EP���� �����ȴٸ� �������� �������� - ����Ʈ ���� �����ϰ� �ݴ��� +
			 ����Ʈ���� �����Ѵ�. 

			 SP - EP�� ����Ʈ - ����Ʈ�� �̵����� �����Ѵ�. 

			 �� ���� ROW, COL ó���� Ʈ���� ������ָ�ȴ�. 

			 �����Ѵ�. 
			*/


		}
	};

	caliTask[measureRegion == MeasureRegion::align ? 0 : 1]();

	// ���� �Ϸ�
	LOG_MESG(ENG_EDIC::en_2d_cali, _T("Successful 2D Cal"));

	if (NULL != hHwnd)
	{
		// Grab View ����
		::SendMessageTimeout(hHwnd, eMSG_ACCR_MEASURE_REPORT, (WPARAM)e2DCAL_REPORT_OK, NULL, SMTO_NORMAL, 100, NULL);
	}

	m_bRunnigThread = FALSE;
	return TRUE;
}

/*
 desc : ���ο� �˻��� ���� �˻� ����� �� ���
 parm : None
 retn : Non
*/
BOOL CAccuracyMgr::SetRegistModel()
{
	UINT32 u32Model = (UINT32)ENG_MMDT::en_circle, i = 0;
	DOUBLE dbMSize = 2.0f /* um */, dbMColor = uvEng_GetConfig()->acam_spec.in_ring_color /* 256:Black, 128:White */;
	LPG_CIEA pstCfg = uvEng_GetConfig();

	/* �˻� ��� ���� �� ��� */
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
			uvCmn_Camera_SetMarkFindMode(i + 1, 0, TMP_MARK); // CALIB�� ���� ���� MARK ���(MOD)
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
 desc : �Է��� ����ŭ ��⵿���� �����Ѵ�.
 parm : ��� �ð�(ms)
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

	// X ������ �ƴ϶�� X�� Y�� ���� ���� �����Ѵ�.
	if (eCAL_PARAM_DIR_X != u8Dir)
	{
		cpTempMax.SetPoint(cpMaxGridSize.y, cpMaxGridSize.x);
	}

	// ������� ����� ���� ���
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

	// �Ķ���� �Է� ������ �� -1�� ���� ��ȯ�Ѵ�.
	return cpReturnPoint;
}

// Motor�� ��� ���� �ּ� ������ 0, 0�̴�. (Cal File ��� �� �������� ���� �ʿ�)
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

	/* Ÿ�̸� �ʱ�ȭ �� ���� */
	cTime.Init();
	cTime.Start();

	/* ���� �ð����� ���� */
	while (nTimeOut > (int)cTime.GetTactMs())
	{
		if (TRUE == m_bStop)
		{
			return FALSE;
		}

		/* Ÿ�� ��ġ�� ������ġ�� ������ ���Ѵ�. */
		dDiffPos = dPos - uvCmn_MC2_GetDrvAbsPos(eMotor);

		/* Motor�� Ÿ����ġ�� �����ϰ� ���� ������ ��� ���� */
		if (FALSE == uvCmn_MC2_IsDriveBusy(eMotor) &&
			fabs(dDiffPos) <= dDiffDistance)
		{
			return TRUE;
		}

		cTime.Stop();
		Sleep(1);
	}

	/* �ð� �ʰ� */
	return FALSE;
}

BOOL CAccuracyMgr::MotionCalcMoving(double dMoveX, double dMoveY, int nTimeOut, double dDiffDistance)
{
	CTactTimeCheck cTime;
	UINT8 u8InstAngle = uvEng_GetConfig()->set_cams.acam_inst_angle; // ī�޶� ��ġ�� ��, ȸ�� ���� (0: ȸ�� ����, 1: 180�� ȸ��)
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
		enXDrv = (1 == m_u8ACamID) ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
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

	/* Ÿ�̸� �ʱ�ȭ �� ���� */
	cTime.Init();
	cTime.Start();

	/* ���� �ð����� ���� */
	while (nTimeOut > (int)cTime.GetTactMs())
	{
		if (TRUE == m_bStop)
		{
			return FALSE;
		}

		/* Ÿ�� ��ġ�� ������ġ�� ������ ���Ѵ�. */
		dDiffYPos = dStagePosY - uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);
		dDiffACamPos = dACamPosX - uvCmn_MC2_GetDrvAbsPos(enXDrv);

		/* Motor�� Ÿ����ġ�� �����ϰ� ���� ������ ��� ���� */
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

	/* �ð� �ʰ� */
	return FALSE;
}


/*
 desc : ���� ��ġ���� Mark Image�� Grab !!!
 parm : ��ݺ� Ƚ��
 retn : ���� ���� �� TRUE, ���� �� FALSE
*/
BOOL CAccuracyMgr::GrabData(STG_ACGR& stGrab, BOOL bRunMode, int nRetryCount)
{
	CTactTimeCheck cTime;
	UINT8 u8ChNo = m_u8ACamID;
	UINT8 u8Mode = (TRUE == bRunMode) ? 0xFF : 0xFE;
	LPG_ACGR pstGrab = NULL;

	/* ���� Live & Edge & Calibration ������ �ʱ�ȭ */
	uvEng_Camera_ResetGrabbedImage();

	/* Mark Model�� ��ϵǾ� �ִ��� ���� */
	if (!uvEng_Camera_IsSetMarkModelACam(u8ChNo, 2))
	{
		return FALSE;
	}

	/* ī�޶� Grabbed Mode�� Calibration Mode�� ���� */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);

	for (int i = 0; i < nRetryCount; i++)
	{
		/* Trigger 1�� �߻� */
		if (!uvEng_Mvenc_ReqTrigOutOne(u8ChNo, 0x00, FALSE))
		{
			return FALSE;
		}

		/* Ÿ�̸� �ʱ�ȭ �� ���� */
		cTime.Init();
		cTime.Start();

		while (3000 > (int)cTime.GetTactMs())
		{
			if (TRUE == m_bStop)
			{
				return FALSE;
			}

			auto mode = this->GetSearchMode();

			/* Grabbed Image�� �����ϴ��� Ȯ�� */

			if (mode == SearchMode::single)
			{
				pstGrab = uvEng_Camera_RunModelCali(u8ChNo, u8Mode, (UINT8)DISP_TYPE_CALB_ACCR, TMP_MARK, TRUE, uvEng_GetConfig()->mark_find.image_process);	/* �̹��� �߽����� �̵��ϱ� ������ ������ 0xff �� */
				if (NULL != pstGrab && 0x00 != pstGrab->marked)
				{
					stGrab = *pstGrab;
					/* ���� �ܰ�� �̵� */
					return TRUE;
				}
			}
			else
			{
				double errX, errY;
				if (RunDoublemarkTestExam(u8ChNo, &errX, &errY))
				{
					stGrab.move_mm_x = errX;
					stGrab.move_mm_y = errY;
					return true;
				}


			}
			cTime.Stop();
			Sleep(1);
		}
	}

	/* �ð� �ʰ� */
	return FALSE;
}

/*
 desc : ���� ������ ����
 parm : UI ������ �ڵ�
 retn : None
*/
VOID CAccuracyMgr::MeasureStart(HWND hHwnd/* = NULL*/)
{
	m_bStop = FALSE;
	m_bRunnigThread = TRUE;
	m_pMeasureThread = AfxBeginThread(MeasureThread, (LPVOID)hHwnd);
}

/*
 desc : ���� ������
 parm : UI ������ �ڵ�
 retn : 0
*/
UINT MeasureThread(LPVOID lpParam)
{
	HWND hHwnd = (HWND)lpParam;

	CAccuracyMgr::GetInstance()->Measurement(hHwnd);

	return 0;
}


bool DoubleMarkAccurcytest::RegistMultiMark()
{
	LPG_CIEA pstCfg = uvEng_GetConfig();
	UINT32 u32Model[2] = { (UINT32)ENG_MMDT::en_circle, (UINT32)ENG_MMDT::en_ring }, u32Find = 2;
	DOUBLE dbMSize1[2] = { NULL } /* um */, dbMColor[2] = { NULL } /* 256:Black, 128:White */;
	DOUBLE dbMSize2[2] = { NULL };

	u32Model[0] = (UINT32)ENG_MMDT::en_ring;
	dbMSize1[0] = pstCfg->acam_cali.model_ring_size1 * 1000.0f;
	dbMSize2[0] = pstCfg->acam_cali.model_ring_size2 * 1000.0f;
	dbMColor[0] = pstCfg->acam_cali.model_ring_color;

	u32Model[1] = (UINT32)ENG_MMDT::en_circle;
	dbMSize1[1] = pstCfg->acam_cali.model_shut_size * 1000.0f;
	dbMColor[1] = pstCfg->acam_cali.model_shut_color;

	const int markCnt = 2;

	for (int i = 0; i < pstCfg->set_cams.acam_count; i++)
	{
		if (uvEng_Camera_SetModelDefineEx(i+1,
			pstCfg->mark_find.model_speed,
			pstCfg->mark_find.detail_level,
			markCnt,
			pstCfg->mark_find.model_smooth,
			u32Model,
			dbMColor,
			dbMSize1,
			dbMSize2,
			NULL,
			NULL,
			TMP_MARK,
			pstCfg->acam_spec.in_ring_scale_min,
			pstCfg->acam_spec.in_ring_scale_max) == false)
			return false;
	}
}


bool DoubleMarkAccurcytest::RunDoublemarkTestExam(int camNum, double* pdErrX = nullptr, double* pdErrY = nullptr)
{
	
	UINT32 u32GrabSize = 0;
	DOUBLE dbOffsetXY[2] = { NULL };

	LPG_ACGR pstResult = NULL;
	/* Grabbed Image ũ�� ��� (����: bytes) */
	auto aoiWidth = uvEng_GetConfig()->set_cams.aoi_size[0];
	auto aoiHeight = uvEng_GetConfig()->set_cams.aoi_size[1];
	u32GrabSize = aoiWidth * aoiHeight;
	/* Grabbed Image ���� �Ҵ� */
	pstResult = new STG_ACGR[2];
	ASSERT(pstResult);
	memset(pstResult, 0x00, (sizeof(STG_ACGR) - sizeof(PUINT8)) * 2);
	pstResult[0].grab_data = new UINT8[u32GrabSize + 1];// (PUINT8)::Alloc(u32GrabSize + 1);
	pstResult[1].grab_data = new UINT8[u32GrabSize + 1]; // (PUINT8)::Alloc(u32GrabSize + 1);
	pstResult[0].grab_data[u32GrabSize] = 0x00;
	pstResult[1].grab_data[u32GrabSize] = 0x00;
	pstResult[0].cam_id = camNum;
	pstResult[1].cam_id = camNum;

	double errX = 0, errY = 0;

	BOOL res = FALSE;
	uvEng_Camera_ClearShapes((UINT8)DISP_TYPE_CALB_ACCR);
	uvEng_Camera_OverlayAddLineList((UINT8)DISP_TYPE_CALB_ACCR, 0, 400, 0, 400, 800, PS_SOLID, 4);
	uvEng_Camera_OverlayAddLineList((UINT8)DISP_TYPE_CALB_ACCR, 0, 0, 400, 800, 400, PS_SOLID, 4);

	auto halfW = aoiWidth >> 1;
	auto halfH = aoiHeight >> 1;

	Sleep(300);

	if (res = uvEng_Camera_RunModelExam(pstResult))
	{
		errX = pstResult[0].mark_cent_mm_x - pstResult[1].mark_cent_mm_x;
		errY = pstResult[0].mark_cent_mm_y - pstResult[1].mark_cent_mm_y;

		if (NULL != pdErrX && NULL != pdErrY)
		{
			*pdErrX = errX;
			*pdErrY = errY;
		}
		char temp[255] = { 0, };
		sprintf_s(temp, "offsetX: %.4f", errX);
		uvEng_Camera_OverlayAddTextList((UINT8)DISP_TYPE_CALB_ACCR, 0, 50, 700, CString(temp), 15, 10, 20, VISION_FONT_TEXT, true);
		sprintf_s(temp, "offsetY: %.4f", errY);
		uvEng_Camera_OverlayAddTextList((UINT8)DISP_TYPE_CALB_ACCR, 0, 50, 750, CString(temp), 15, 10, 20, VISION_FONT_TEXT, true);
	}
	else
	{
		uvEng_Camera_OverlayAddTextList((UINT8)DISP_TYPE_CALB_ACCR, 0, 50, 700, (pstResult[0].marked == true ? L"Ring find Success." : L"Ring find failed."), (pstResult[0].marked == true ? 15 : 2), 10, 20, VISION_FONT_TEXT, true);
		uvEng_Camera_OverlayAddTextList((UINT8)DISP_TYPE_CALB_ACCR, 0, 50, 750, (pstResult[1].marked == true ? L"Dot find Success." : L"Dot find failed."), (pstResult[1].marked == true ? 15 : 2), 10, 20, VISION_FONT_TEXT, true);

	}
	//			int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, int fi_iWdt1, int fi_iWdt2, int fi_color)

	uvEng_Camera_DrawImageBitmap((UINT8)DISP_TYPE_CALB_ACCR, 0, camNum, 0, 1);
	
	delete[] pstResult[0].grab_data;
	delete[] pstResult[1].grab_data;
	delete[] pstResult;
	//return pstResult;

	return res;
	//return uvEng_Camera_RunModelExam(res);

}

