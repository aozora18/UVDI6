
/*
 desc : �ó����� �⺻ (Base) �Լ� ����
*/

#include "pch.h"
#include "FlushErrorMgr.h"
#include "../../MainApp.h"


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
CFlushErrorMgr::CFlushErrorMgr()
{
	m_bStop = FALSE;
	m_u8CurStep = 0x01;
	m_u8MaxStep = 0x01;
	m_bResultSucc = FALSE;
	m_pstGrabData = NULL;
	m_dVctStripeWidth.clear();
}

/*
 desc : �Ҹ���
 parm : None
 retn : None
*/
CFlushErrorMgr::~CFlushErrorMgr()
{
	m_dVctStripeWidth.clear();

	m_pMeasureThread = NULL;
	delete m_pMeasureThread;
}

/*
 desc : �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CFlushErrorMgr::Initialize()
{
	UINT32 u32Size = 0;
	UINT8 u8CamCount = uvEng_GetConfig()->set_cams.acam_count;

	/* Grabbed Image ���� �Ҵ� */
	m_pstGrabData = new STG_ACGR[(int)u8CamCount];
	ASSERT(m_pstGrabData);
	memset(m_pstGrabData, 0x00, sizeof(STG_ACGR) * 2);
	/* Grabbed Image Buffer �Ҵ�*/
	u32Size = uvEng_GetConfig()->set_cams.soi_size[0] *
		uvEng_GetConfig()->set_cams.soi_size[1];

	for (int nIndex = 0; nIndex < u8CamCount; nIndex++)
	{
		m_pstGrabData[nIndex].grab_data = new UINT8[u32Size + 1]; // = (PUINT8)::Alloc(u32Size + 1);
		m_pstGrabData[nIndex].grab_data[u32Size] = 0x00;
	}

	LoadTable();
}

/*
 desc : ���� �� ȣ��
 parm : None
 retn : None
*/
VOID CFlushErrorMgr::Terminate()
{
	UINT8 u8CamCount = uvEng_GetConfig()->set_cams.acam_count;

	m_bStop = TRUE;
	if (m_pMeasureThread != NULL)
	{
		WaitForSingleObject(m_pMeasureThread->m_hThread, INFINITE);
	}

	if (!m_pstGrabData)
	{
		return;
	}

	//for (int nIndex = 0; nIndex < u8CamCount; nIndex++)
	//{
	//	//delete m_pstGrabData[nIndex];
	//	/*if (m_pstGrabData!= null && m_pstGrabData[nIndex]   NULL != m_pstGrabData[nIndex].grab_data)
	//	{
	//		::Free(m_pstGrabData[nIndex].grab_data);
	//	}*/
	//}

	for (int i = 0; i < u8CamCount; i++)
	{
		delete[] m_pstGrabData[i].grab_data;
	}

	delete[] m_pstGrabData;
}

BOOL CFlushErrorMgr::SetRegistModel()
{
#ifndef FLUSH_SIMUL
	UINT32 u32Model = (UINT32)ENG_MMDT::en_circle;
	DOUBLE dbMSize = 0.0f /* um */, dbMColor = 256 /* 256:Black, 128:White */;
	LPG_CIEA pstCfg = uvEng_GetConfig();

	/* �ݵ�� 2�� �˻��ؾ� �ϹǷ�... �˻� ���� ���� */
	uvEng_Camera_SetMarkMethod(ENG_MMSM::en_ph_step);

	/* �˻� ��� ���� �� ��� */
	dbMColor = uvEng_GetConfig()->ph_step.model_color;
	dbMSize = uvEng_GetConfig()->ph_step.model_dia_size * 1000.0f;

	if (!uvEng_Camera_SetModelDefineEx(0x01,
		uvEng_GetConfig()->mark_find.model_speed,
		uvEng_GetConfig()->mark_find.detail_level,
		2,
		uvEng_GetConfig()->mark_find.model_smooth,
		&u32Model, &dbMColor, &dbMSize,
		NULL, NULL, NULL, TMP_MARK,	/* ����Ϸ��� ���� ������ �ݵ�� 1�� */
		pstCfg->acam_spec.in_ring_scale_min,
		pstCfg->acam_spec.in_ring_scale_max, 0, 0, true))
	{
		return FALSE;
	}
	else {
		uvCmn_Camera_SetMarkFindMode(1, 0, TMP_MARK); // FLUSH�� ���� ���� MARK ���(MOD)
	}
#endif // !FLUSH_SIMUL

	return TRUE;
}


BOOL CFlushErrorMgr::LoadTable()
{
	CStdioFile sFile;
	CString strPath;
	CString strLine;
	CString strValue;
	int nCount = 0;

	m_dVctStripeWidth.clear();
	strPath.Format(_T("%s\\%s\\cali\\STRIPE_WIDTH_TABLE.csv"), g_tzWorkDir, CUSTOM_DATA_CONFIG);

	if (TRUE == sFile.Open(strPath, CStdioFile::shareDenyNone | CStdioFile::modeRead))
	{
		sFile.ReadString(strLine);

		nCount = (int)count((LPCTSTR)strLine, (LPCTSTR)strLine + strLine.GetLength(), _T(','));

		for (int i = 0; i < nCount; i++)
		{
			if (TRUE == AfxExtractSubString(strValue, strLine, i, ','))
			{
				//if (0 != _ttof(strValue))
				{
					m_dVctStripeWidth.push_back(_ttof(strValue));
				}
			}
		}

		sFile.Close();
		return TRUE;
	}

	return FALSE;
}


/*
 desc : �Է��� ����ŭ ��⵿���� �����Ѵ�.
 parm : ��� �ð�(ms)
 retn : None
*/
VOID CFlushErrorMgr::Wait(int nTime)
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
 desc : Grabbed Image ó��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CFlushErrorMgr::GrabbedImage(double& dCenterMove)
{
	CTactTimeCheck cTime;
	BOOL bIsChanged = FALSE;
	DOUBLE dbCentOffset = uvEng_GetConfig()->ph_step.center_offset;
	int nRetryCount = 3;
	UINT8 u8ChNo = 0x01;

	/* �۾� �Ϸ� ���� �������� ���� */
	m_bResultSucc = FALSE;

	/* Grabbed Image�� �����ϴ��� Ȯ�� */
	m_pstGrabData[0].marked = 0x00;
	m_pstGrabData[1].marked = 0x00;

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
		Sleep(4000);

		/* Ÿ�̸� �ʱ�ȭ �� ���� */
		cTime.Init();
		cTime.Start();

		while (3000 > (int)cTime.GetTactMs())
		{
			if (TRUE == m_bStop)
			{
				return FALSE;
			}

			/* Grabbed Image�� �����ϴ��� Ȯ�� */
			uvEng_Camera_RunModelStep(0x01, eMODEL_FIND_COUNT, FALSE, m_pstGrabData, (UINT8)DISP_TYPE_CALB_EXPO, TMP_MARK, uvEng_GetConfig()->mark_find.image_process);

			if (nullptr != m_pstGrabData)
			{
				/* ����� ī�޶� ȸ���Ǿ� �ִٸ� ... (180�� �̻�) */
				if (uvEng_GetConfig()->set_cams.acam_inst_angle)
				{
					bIsChanged = m_pstGrabData[0].mark_cent_mm_x < m_pstGrabData[1].mark_cent_mm_x;
				}
				else
				{
					bIsChanged = m_pstGrabData[0].mark_cent_mm_x > m_pstGrabData[1].mark_cent_mm_x;
				}

				/* X ��ǥ ���� ���� ������ ���ġ */
				if (FALSE == bIsChanged)
				{
					STG_ACGR stTemp;
					stTemp.Init();
					memcpy(&stTemp, &m_pstGrabData[0], sizeof(STG_ACGR));
					memcpy(&m_pstGrabData[0], &m_pstGrabData[1], sizeof(STG_ACGR));
					memcpy(&m_pstGrabData[1], &stTemp, sizeof(STG_ACGR));
				}

				/* ���� ���� �ȵƴٸ� ... */
				if (!(m_pstGrabData[0].marked && m_pstGrabData[1].marked))
				{
					AfxMessageBox(L"Failed to find the mark info", MB_ICONSTOP | MB_TOPMOST);
					return FALSE;
				}

				/* ���� Grabbed Image 2���� �߽� ���� ������ ����� ���ϰ�, 1��° ī�޶��� ���� �̵� �Ÿ� ���� */
				dCenterMove = m_pstGrabData[0].mark_cent_mm_dist - m_pstGrabData[1].mark_cent_mm_dist;

				TRACE(L"m_dbCentMove = %.4f, dbCentOffset = %.4f mm\n", dCenterMove, dbCentOffset);

				if (abs(dCenterMove) < dbCentOffset)	/* 1.0 mm �̳��� �ִ��� ���� */
				{
					/* �۾� ���� */
					m_bResultSucc = TRUE;
				}

				return TRUE;
			}

			cTime.Stop();
			Sleep(1);
		}
	}

	/* �ð� �ʰ� */
	return FALSE;
}

/*
 desc : �̹����� �߽ɿ� ������ ��� �̵�
 parm : None
 retn : TRUE or FALSE
*/
BOOL CFlushErrorMgr::MoveMotion(double dCenterMove, int nTimeOut/* = 30000*/, double dDiffDistance/* = 0.005*/)
{

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	ENG_MMDI MoveDrve = ENG_MMDI::en_align_cam1;
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	ENG_MMDI MoveDrve = ENG_MMDI::en_stage_x;
#endif

	CTactTimeCheck cTime;			/* Ÿ�̸� */
	double dDiff = 0.;				/* ��ġ ������ */
	double dPos = uvCmn_MC2_GetDrvAbsPos(MoveDrve);
	double dOldPos = 0;
	double dSpeed = 0;
	double dNextPos = 0;
	TCHAR tzMesg[1024] = { NULL };

	dOldPos = uvCmn_MC2_GetDrvAbsPos(MoveDrve);
	dNextPos = uvEng_GetConfig()->ph_step.stripe_width;

	/* ���� ���õ� ����� ī�޶��� �̵� �ӵ� �� ��� */
	dSpeed = uvEng_GetConfig()->mc2_svc.step_velo;

	/* ��� ��ġ ���� ���� */
	if (FALSE == m_bResultSucc)
	{
		/* ���� ���õ� ����� ī�޶� �̵��ϱ� ���� �� ���� */
		dPos = dOldPos + (dCenterMove * 0.25f);

		/* ���� ����� ī�޶��� Toggle �� ���� */
		uvCmn_MC2_GetDrvDoneToggled(MoveDrve);

		/* ī�޶� ������ ��ġ�� �̵� */
		if (!uvEng_MC2_SendDevAbsMove(MoveDrve, dPos, dSpeed))
		{
			AfxMessageBox(L"Failed to move the motion drive of alignment camera", MB_ICONSTOP | MB_TOPMOST);
			return FALSE;
		}
	}
	/* ���� ���� ��ġ�� �̵� */
	else
	{
		/* ���� ������ �ܰ谡 �����ϸ� ���� ��ġ�� �̵� */
		if (m_u8CurStep < m_u8MaxStep)
		{
			/* ���� ���õ� ����� ī�޶� �̵��ϱ� ���� �� ���� */
			if (0 < m_dVctStripeWidth.size() && m_u8CurStep <= m_dVctStripeWidth.size())
			{
				dNextPos = m_dVctStripeWidth[m_u8CurStep - 1];
			}

			dPos = dOldPos + dNextPos;

			swprintf_s(tzMesg, 1024, L"[STEP][%d/%d] OLD POS:%.4f, NEXT DIST:%.4f, NEXT POS:%.4f", m_u8CurStep, m_u8MaxStep, dOldPos, dNextPos, dPos);
			LOG_MESG(ENG_EDIC::en_ph_step_cali, tzMesg);

			/* ���� ����� ī�޶��� Toggle �� ���� */
			uvCmn_MC2_GetDrvDoneToggled(MoveDrve);

			/* ī�޶� ������ ��ġ�� �̵� */
			if (!uvEng_MC2_SendDevAbsMove(MoveDrve, dPos, dSpeed))
			{
				AfxMessageBox(L"Failed to move the motion drive of alignment camera", MB_ICONSTOP | MB_TOPMOST);
				return FALSE;
			}
		}
		else
		{
			return TRUE;
		}
	}

#ifdef MOTOR_SIMUL
	return TRUE;
#endif // MOTOR_SIMUL

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

		if (TRUE == uvCmn_MC2_IsDrvDoneToggled(MoveDrve))
		{
			/* Ÿ�� ��ġ�� ������ġ�� ������ ���Ѵ�. */
			dDiff = dPos - uvCmn_MC2_GetDrvAbsPos(MoveDrve);

			/* Motor�� Ÿ����ġ�� �����ϰ� ���� ������ ��� ���� */
			if (FALSE == uvCmn_MC2_IsDriveBusy(MoveDrve) &&
				fabs(dDiff) < dDiffDistance)
			{
				return TRUE;
			}
		}
		/* TimeOut �ð��� �����̳� ������ �� */
		else if (nTimeOut * 0.5 <= (int)cTime.GetTactMs())
		{
			/* �⵿���� ���� �ʾҴٸ� */
			if (dDiffDistance >= fabs(dOldPos - uvCmn_MC2_GetDrvAbsPos(MoveDrve)))
			{
				/* ���� */
				uvEng_MC2_SendDevAbsMove(MoveDrve, dPos, dSpeed);
			}
		}

		cTime.Stop();
		Sleep(1);
	}

	/* �ð� �ʰ� */
	return FALSE;
}

/*
 desc : ���� ���μ���
 parm : UI ������ �ڵ�
 retn : ������ �Ϸ�Ǿ����� ����
 cmnt : ���� ��⵿������ ���� �����带 ������ UI���� ȣ���� �����Ѵ�.
*/
BOOL CFlushErrorMgr::Measurement(HWND hHwnd)
{
	CString* pstrText = nullptr;				/* ������ �޽��� ���޿� ������ */
	CString strText;							/* UI ���޿� ���ڿ� */
	UINT8	u8Step = 0x00;
	double dCenterMove = 0.;
	BOOL bRun = TRUE;

	while (TRUE == bRun)
	{
		if (TRUE == m_bStop)
		{
			if (NULL != hHwnd)
			{
				strText.Format(_T("STEP MEASURE STOP"));
				pstrText = &strText;
				::SendMessageTimeout(hHwnd, eMSG_EXPO_FLUSH_RESULT_PROGRESS, (WPARAM)0, (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
			}

			m_bRunnigThread = FALSE;
			return FALSE;
		}

		switch (u8Step)
		{
		/* �뱤 �� ������ġ�� �޶��� ���ɼ��� ���� �������� ��ġ �̵� �� ���� */
// 		case eMEASURE_TRIGGER_ON:
// 			if (NULL != hHwnd)
// 			{
// 				strText.Format(_T("[%d/%d] TRIGGER ON"), m_u8CurStep, m_u8MaxStep);
// 				pstrText = &strText;
// 				::SendMessageTimeout(hHwnd, eMSG_EXPO_FLUSH_RESULT_PROGRESS, (WPARAM)m_u8CurStep / m_u8MaxStep * 100, (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
// 			}
// 
// 			bRun = TriggerOn();
// 			break;

		case eMEASURE_GRABBED_IMAGE:
			if (NULL != hHwnd)
			{
				strText.Format(_T("[%d/%d] GRAB IMAGE"), m_u8CurStep, m_u8MaxStep);
				pstrText = &strText;
				::SendMessageTimeout(hHwnd, eMSG_EXPO_FLUSH_RESULT_PROGRESS, (WPARAM)(((double)m_u8CurStep / (double)m_u8MaxStep) * 100), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
			}

			bRun = GrabbedImage(dCenterMove);
			break;

		case eMEASURE_MOVE_MOTION:
			if (NULL != hHwnd)
			{
				strText.Format(_T("[%d/%d] MOVE MOTION"), m_u8CurStep, m_u8MaxStep);
				pstrText = &strText;
				::SendMessageTimeout(hHwnd, eMSG_EXPO_FLUSH_RESULT_PROGRESS, (WPARAM)(((double)m_u8CurStep / (double)m_u8MaxStep) * 100), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
			}

			bRun = MoveMotion(dCenterMove);
			break;

 		default:
			m_bRunnigThread = FALSE;
			return FALSE;
		}

		if (eMEASURE_MOVE_MOTION == u8Step)
		{
			if (TRUE == m_bResultSucc)
			{
				if (NULL != hHwnd)
				{
					::SendMessageTimeout(hHwnd, eMSG_EXPO_FLUSH_RESULT, (WPARAM)TRUE, (LPARAM)m_u8CurStep, SMTO_NORMAL, 500, NULL);
				}

				m_u8CurStep++;
			}

			if (m_u8CurStep > m_u8MaxStep)
			{
				m_bRunnigThread = FALSE;

				if (NULL != hHwnd)
				{
					strText.Format(_T("FINISH"));
					pstrText = &strText;
					::SendMessageTimeout(hHwnd, eMSG_EXPO_FLUSH_RESULT_PROGRESS, (WPARAM)100, (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
				}

				return TRUE;
			}

			u8Step = eMEASURE_GRABBED_IMAGE;
		}
		else
		{
			u8Step++;
		}

		Wait(500);
	}

	if (NULL != hHwnd)
	{
		strText.Format(_T("STEP MEASURE FAIL"));
		pstrText = &strText;
		::SendMessageTimeout(hHwnd, eMSG_EXPO_FLUSH_RESULT_PROGRESS, (WPARAM)0, (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
		::SendMessageTimeout(hHwnd, eMSG_EXPO_FLUSH_RESULT, (WPARAM)FALSE, NULL, SMTO_NORMAL, 500, NULL);
	}

	m_bRunnigThread = FALSE;
	return FALSE;
}


/*
 desc : ���� ������ ����
 parm : UI ������ �ڵ�
 retn : None
*/
VOID CFlushErrorMgr::MeasureStart(HWND hHwnd/* = NULL*/)
{
	m_bStop = FALSE;
	m_bRunnigThread = TRUE;
	m_pMeasureThread = AfxBeginThread(FlushMeasureThread, (LPVOID)hHwnd);
}

/*
 desc : ���� ������
 parm : UI ������ �ڵ�
 retn : 0
*/
UINT FlushMeasureThread(LPVOID lpParam)
{
	HWND hHwnd = (HWND)lpParam;

	CFlushErrorMgr::GetInstance()->Measurement(hHwnd);

	return 0;
}