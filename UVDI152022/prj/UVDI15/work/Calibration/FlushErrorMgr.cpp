
/*
 desc : 시나리오 기본 (Base) 함수 모음
*/

#include "pch.h"
#include "FlushErrorMgr.h"
#include "../../MainApp.h"


#include "../../param/RecipeManager.h"
#include "../../DlgMain.h"
#include "../../MainThread.h"


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
 desc : 소멸자
 parm : None
 retn : None
*/
CFlushErrorMgr::~CFlushErrorMgr()
{
	m_dVctStripeWidth.clear();
	
	m_pMeasureThread = NULL;
	
}

/*
 desc : 초기화
 parm : None
 retn : None
*/
VOID CFlushErrorMgr::Initialize()
{
	UINT32 u32Size = 0;
	UINT8 u8CamCount = uvEng_GetConfig()->set_cams.acam_count;

	/* Grabbed Image 버퍼 할당 */
	m_pstGrabData = new STG_ACGR[(int)u8CamCount];
	ASSERT(m_pstGrabData);
	memset(m_pstGrabData, 0x00, sizeof(STG_ACGR) * 2);
	/* Grabbed Image Buffer 할당*/
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
 desc : 종료 시 호출
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
		m_pstGrabData[i].grab_data = nullptr;
	}

	delete[] m_pstGrabData;
	m_pstGrabData = nullptr;
}

BOOL CFlushErrorMgr::SetRegistModel()
{
#ifndef FLUSH_SIMUL
	UINT32 u32Model = (UINT32)ENG_MMDT::en_circle;
	DOUBLE dbMSize = 0.0f /* um */, dbMColor = 256 /* 256:Black, 128:White */;
	LPG_CIEA pstCfg = uvEng_GetConfig();

	/* 반드시 2개 검색해야 하므로... 검색 개수 설정 */
	uvEng_Camera_SetMarkMethod(ENG_MMSM::en_ph_step);

	/* 검색 대상에 대한 모델 등록 */
	dbMColor = uvEng_GetConfig()->ph_step.model_color;
	dbMSize = uvEng_GetConfig()->ph_step.model_dia_size * 1000.0f;

	if (!uvEng_Camera_SetModelDefineEx(0x01,
		uvEng_GetConfig()->mark_find.model_speed,
		uvEng_GetConfig()->mark_find.detail_level,
		2,
		uvEng_GetConfig()->mark_find.model_smooth,
		&u32Model, &dbMColor, &dbMSize,
		NULL, NULL, NULL, TMP_MARK,	/* 등록하려는 모델의 개수는 반드시 1개 */
		pstCfg->acam_spec.in_ring_scale_min,
		pstCfg->acam_spec.in_ring_scale_max, 0, 0, true))
	{
		return FALSE;
	}
	else {
		uvCmn_Camera_SetMarkFindMode(1, 0, TMP_MARK); // FLUSH는 직접 만든 MARK 사용(MOD)
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
 desc : 입력한 값만큼 대기동작을 수행한다.
 parm : 대기 시간(ms)
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
 desc : Grabbed Image 처리
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

	/* 작업 완료 하지 못함으로 설정 */
	m_bResultSucc = FALSE;

	/* Grabbed Image가 존재하는지 확인 */
	m_pstGrabData[0].marked = 0x00;
	m_pstGrabData[1].marked = 0x00;

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
		if (!uvEng_Mvenc_ReqTrigOutOne(u8ChNo))
		{
			return FALSE;
		}
		Sleep(4000);

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
			uvEng_Camera_RunModelStep(0x01, eMODEL_FIND_COUNT, FALSE, m_pstGrabData, (UINT8)DISP_TYPE_CALB_EXPO, TMP_MARK, uvEng_GetConfig()->mark_find.image_process);

			if (nullptr != m_pstGrabData)
			{
				/* 얼라인 카메라가 회전되어 있다면 ... (180도 이상) */
				if (uvEng_GetConfig()->set_cams.acam_inst_angle)
				{
					bIsChanged = m_pstGrabData[0].mark_cent_mm_x < m_pstGrabData[1].mark_cent_mm_x;
				}
				else
				{
					bIsChanged = m_pstGrabData[0].mark_cent_mm_x > m_pstGrabData[1].mark_cent_mm_x;
				}

				/* X 좌표 값이 작은 순으로 재배치 */
				if (FALSE == bIsChanged)
				{
					STG_ACGR stTemp;
					stTemp.Init();
					memcpy(&stTemp, &m_pstGrabData[0], sizeof(STG_ACGR));
					memcpy(&m_pstGrabData[0], &m_pstGrabData[1], sizeof(STG_ACGR));
					memcpy(&m_pstGrabData[1], &stTemp, sizeof(STG_ACGR));
				}

				/* 정상 검출 안됐다면 ... */
				if (!(m_pstGrabData[0].marked && m_pstGrabData[1].marked))
				{
					AfxMessageBox(L"Failed to find the mark info", MB_ICONSTOP | MB_TOPMOST);
					return FALSE;
				}

				/* 현재 Grabbed Image 2개의 중심 간의 떨어진 평균을 구하고, 1번째 카메라의 최종 이동 거리 구함 */
				dCenterMove = m_pstGrabData[0].mark_cent_mm_dist - m_pstGrabData[1].mark_cent_mm_dist;

				TRACE(L"m_dbCentMove = %.4f, dbCentOffset = %.4f mm\n", dCenterMove, dbCentOffset);

				if (abs(dCenterMove) < dbCentOffset)	/* 1.0 mm 이내에 있는지 여부 */
				{
					/* 작업 성공 */
					m_bResultSucc = TRUE;
				}

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
 desc : 이미지가 중심에 오도록 모션 이동
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

	CTactTimeCheck cTime;			/* 타이머 */
	double dDiff = 0.;				/* 위치 오차값 */
	double dPos = uvCmn_MC2_GetDrvAbsPos(MoveDrve);
	double dOldPos = 0;
	double dSpeed = 0;
	double dNextPos = 0;
	TCHAR tzMesg[1024] = { NULL };

	dOldPos = uvCmn_MC2_GetDrvAbsPos(MoveDrve);
	dNextPos = uvEng_GetConfig()->ph_step.stripe_width;

	/* 현재 선택된 얼라인 카메라의 이동 속도 값 얻기 */
	dSpeed = uvEng_GetConfig()->mc2_svc.step_velo;

	/* 모션 위치 정밀 조정 */
	if (FALSE == m_bResultSucc)
	{
		/* 현재 선택된 얼라인 카메라를 이동하기 위한 값 설정 */
		dPos = dOldPos + (dCenterMove * 0.25f);

		/* 현재 얼라인 카메라의 Toggle 값 저장 */
		uvCmn_MC2_GetDrvDoneToggled(MoveDrve);

		/* 카메라를 정해진 위치로 이동 */
		if (!uvEng_MC2_SendDevAbsMove(MoveDrve, dPos, dSpeed))
		{
			AfxMessageBox(L"Failed to move the motion drive of alignment camera", MB_ICONSTOP | MB_TOPMOST);
			return FALSE;
		}
	}
	/* 다음 측정 위치로 이동 */
	else
	{
		/* 다음 측정할 단계가 존재하면 다음 위치로 이동 */
		if (m_u8CurStep < m_u8MaxStep)
		{
			/* 현재 선택된 얼라인 카메라를 이동하기 위한 값 설정 */
			if (0 < m_dVctStripeWidth.size() && m_u8CurStep <= m_dVctStripeWidth.size())
			{
				dNextPos = m_dVctStripeWidth[m_u8CurStep - 1];
			}

			dPos = dOldPos + dNextPos;

			swprintf_s(tzMesg, 1024, L"[STEP][%d/%d] OLD POS:%.4f, NEXT DIST:%.4f, NEXT POS:%.4f", m_u8CurStep, m_u8MaxStep, dOldPos, dNextPos, dPos);
			LOG_MESG(ENG_EDIC::en_ph_step_cali, tzMesg);

			/* 현재 얼라인 카메라의 Toggle 값 저장 */
			uvCmn_MC2_GetDrvDoneToggled(MoveDrve);

			/* 카메라를 정해진 위치로 이동 */
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

		if (TRUE == uvCmn_MC2_IsDrvDoneToggled(MoveDrve))
		{
			/* 타겟 위치와 현재위치의 오차를 구한다. */
			dDiff = dPos - uvCmn_MC2_GetDrvAbsPos(MoveDrve);

			/* Motor가 타겟위치에 도달하고 정지 상태일 경우 종료 */
			if (FALSE == uvCmn_MC2_IsDriveBusy(MoveDrve) &&
				fabs(dDiff) < dDiffDistance)
			{
				return TRUE;
			}
		}
		/* TimeOut 시간이 반절이나 지났을 때 */
		else if (nTimeOut * 0.5 <= (int)cTime.GetTactMs())
		{
			/* 기동조차 하지 않았다면 */
			if (dDiffDistance >= fabs(dOldPos - uvCmn_MC2_GetDrvAbsPos(MoveDrve)))
			{
				/* 재명령 */
				uvEng_MC2_SendDevAbsMove(MoveDrve, dPos, dSpeed);
			}
		}

		cTime.Stop();
		Sleep(1);
	}

	/* 시간 초과 */
	return FALSE;
}

/*
 desc : 측정 프로세스
 parm : UI 윈도우 핸들
 retn : 동작이 완료되었는지 유무
 cmnt : 내부 대기동작으로 인해 쓰레드를 제외한 UI에서 호출을 금지한다.
*/
BOOL CFlushErrorMgr::Measurement(HWND hHwnd,int count, CDlgMain* dlgMain)
{
	CString* pstrText = nullptr;				/* 윈도우 메시지 전달용 포인터 */
	CString strText;							/* UI 전달용 문자열 */
	int	u8Step = count == -1 && dlgMain == nullptr ? eMEASURE_GRABBED_IMAGE : eMEASURE_LOAD_RECIPE;
	double dCenterMove = 0.;
	BOOL bRun = TRUE;
	bool prepared = false;

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	char* gbrName = "SKC_Step_Ruler";
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	char* gbrName = "6um_Step_Ruler";
#endif

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
			case eMEASURE_LOAD_RECIPE:
			{
				if (prepared)
				{
					u8Step = eMEASURE_SET_EXPO_POS;
					continue;
				}

				if (uvCmn_Luria_IsJobNameLoaded())
				{
					char loadedJob[255] = { 0, };
					bool getLoadedJob = uvEng_Luria_GetLoadedJobName(loadedJob, 255);
					prepared = string(loadedJob) == string(gbrName);
				}

				if (prepared == false)
				{
					if (CRecipeManager::GetInstance()->SelectRecipe(CString(gbrName), EN_RECIPE_SELECT_TYPE::eRECIPE_MODE_SEL_FROM_LOCAL))
					{
						if (dlgMain->IsBusyWorkJob())
							return false;

						UINT8 u8Offset = 0xff;
						//dlgMain->GetMainthreadPtr()->RunWorkJob(ENG_BWOK::en_gerb_load, (PUINT64)(&u8Offset));
						Sleep(500);
					}
					else
						return false;
				}
				u8Step++;
			}
			break;

			case eMEASURE_IS_LOAD_COMPLETE:
			{
				if (dlgMain->IsBusyWorkJob())
					continue;
				else
				{
					u8Step++;
					prepared = true;
				}
			}
			break;

			case eMEASURE_SET_EXPO_POS:
			{
				if (prepared)
				{
					LPG_LDMC pstMachine = &uvEng_ShMem_GetLuria()->machine;
					pstMachine->table_expo_start_xy[0].x = -1;
					pstMachine->table_expo_start_xy[0].y = -1;

					DOUBLE* pStartXY = uvEng_GetConfig()->luria_svc.table_expo_start_xy[0];
					if (!uvEng_Luria_ReqSetTableExposureStartPos(0x01, pStartXY[0], pStartXY[1] + (completedCount * 20.0f)))
						return false;
					Sleep(500);

					if (pstMachine->table_expo_start_xy[0].x == -1 ||
						pstMachine->table_expo_start_xy[0].y == -1)
						return false;
				}
				u8Step++;
			}
			break;
			case eMEASURE_PREPRINT:
			{
				uvEng_ShMem_GetLuria()->exposure.ResetExposeState();

				if (!uvEng_Luria_ReqSetPrintOpt(0x00))
				{
					LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetPrintOpt.PrePrint)");
					return false;
				}


				if (!uvEng_Luria_ReqGetExposureState())
				{
					LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetExposureState.PrePrint)");
					return false;
				}
				u8Step++;
			}
			break;

			case eMEASURE_IS_PREPRINT:
			{
				/* 현재 준비 상태가 실패되었는지 확인 */
				if (uvCmn_Luria_IsExposeStateFailed(ENG_LCEP::en_pre_print))
				{
					LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to run the preprint job");
					return false;
				}
				/* 현재 노광 준비 상태가 성공적인 경우 */
				if (uvCmn_Luria_IsExposeStateSuccess(ENG_LCEP::en_pre_print))
				{
					u8Step++;
					uvEng_Luria_ReqGetExposureState();
				}

			}
			break;

			case eMEASURE_PRINT:
			{
				if (!uvEng_Luria_ReqSetPrintOpt(0x01))
				{
					LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetPrintOpt.Print)");
					return false;
				}
				if (!uvEng_GetConfig()->luria_svc.use_announcement)
				{
					/* Printing Status 정보 요청 (함수 내부에 1초에 4번 밖에 요청하지 못함) */
					if (!uvEng_Luria_ReqGetExposureState())
					{
						LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetExposureState.Printing)");
						return false;
					}
				}
				u8Step++;
			}
			break;

			case eMEASURE_IS_PRINT:
			{
				auto u8State = (UINT8)uvCmn_Luria_GetExposeState();
				if (0x0f == (u8State & 0x0f))
				{
					LOG_ERROR(ENG_EDIC::en_uvdi15, L"There was a problem during the expose operation");
					return false;
				}

				/* 노광 완료 상태이면 */
				else if (uvCmn_Luria_IsExposeStateSuccess(ENG_LCEP::en_print))
				{
					u8Step++;
				}

				/* Printing Status 정보 요청 (함수 내부에 1초에 2번 밖에 요청하지 못함) */
				if (!uvEng_GetConfig()->luria_svc.use_announcement)
				{
					/* 다시 현재 노광 진행 상태 요청 */
					if (!uvEng_Luria_ReqGetExposureState())
					{
						LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetExposureState.IsPrinting)");
						return false;
					}
					
				}
			}
			break;

		case eMEASURE_GRABBED_IMAGE:
		{
			if (NULL != hHwnd)
			{
				strText.Format(_T("[%d/%d] GRAB IMAGE"), m_u8CurStep, m_u8MaxStep);
				pstrText = &strText;
				::SendMessageTimeout(hHwnd, eMSG_EXPO_FLUSH_RESULT_PROGRESS, (WPARAM)(((double)m_u8CurStep / (double)m_u8MaxStep) * 100), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
			}

			bRun = GrabbedImage(dCenterMove);
			if (!bRun)
				return false;
			u8Step++;
		}
		break;

		case eMEASURE_MOVE_MOTION:
		{
			if (NULL != hHwnd)
			{
				strText.Format(_T("[%d/%d] MOVE MOTION"), m_u8CurStep, m_u8MaxStep);
				pstrText = &strText;
				::SendMessageTimeout(hHwnd, eMSG_EXPO_FLUSH_RESULT_PROGRESS, (WPARAM)(((double)m_u8CurStep / (double)m_u8MaxStep) * 100), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
			}

			bRun = MoveMotion(dCenterMove);
			if (!bRun)
				return false;
			
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
				if (count != -1 && completedCount == count)
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
				else
				{
					completedCount++;	
					u8Step = eMEASURE_SET_EXPO_POS;
				}
			}
			else
			{
				u8Step = eMEASURE_GRABBED_IMAGE;
			}
		}
		break;

 		default:
			m_bRunnigThread = FALSE;
			return FALSE;
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
 desc : 측정 쓰레드 실행
 parm : UI 윈도우 핸들
 retn : None
*/
VOID CFlushErrorMgr::MeasureStart(HWND hHwnd/* = NULL*/,int count,CDlgMain* dlgMain)
{
	m_bStop = FALSE;
	m_bRunnigThread = TRUE;
	measureCount = count;

	BYTE* ptr = new BYTE[8+8+4];

	memcpy(ptr, &hHwnd,8);
	memcpy(ptr+8, &dlgMain, 8);
	memcpy(ptr+8+8, &measureCount, 4);

	m_pMeasureThread = AfxBeginThread(FlushMeasureThread, (LPVOID)ptr);
}

/*
 desc : 측정 쓰레드
 parm : UI 윈도우 핸들
 retn : 0
*/
UINT FlushMeasureThread(LPVOID lpParam)
{
	
	BYTE* p = static_cast<BYTE*>(lpParam);

	HWND hHwnd;
	memcpy(&hHwnd, p, sizeof(HWND));

	CDlgMain* mainDlg;
	memcpy(&mainDlg, p+ sizeof(HWND), sizeof(CDlgMain*));

	int measureCount;
	memcpy(&measureCount, p + sizeof(HWND)+ sizeof(CDlgMain*), sizeof(int));


	CFlushErrorMgr::GetInstance()->Measurement(hHwnd, measureCount, mainDlg);
	delete[] p;
	return 0;
}