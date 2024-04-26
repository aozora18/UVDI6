
/*
 desc : Main Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "MainThread.h"

/* �۾� ��ü */
#include "./work/step/WorkAbort.h"
#include "./work/step/WorkHoming.h"
#include "./work/step/WorkInited.h"
#include "./work/step/WorkRecipeLoad.h"
#include "./work/step/WorkRecipeUnload.h"
#include "./work/step/WorkExpoOnly.h"
#include "./work/step/WorkExpoAlign.h"
#include "./work/step/WorkMarkMove.h"
#include "./work/step/WorkMarkTest.h"
#include "./work/step/WorkOnlyFEM.h"
#include "./work/step/WorkFEMExpo.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : parent	- [in]  �ڽ��� ȣ���� �θ� ���� �ڵ�
 retn : None
*/
CMainThread::CMainThread(HWND parent)
	: CThinThread()
{
	m_hParent		= parent;
	m_pWorkJob		= NULL;
	m_bAppExit		= FALSE;
	m_u64TickFile	= 0;
	m_u64TickTemp	= 0;
	m_u64TickExit	= 0;
	m_u64TickHotMon	= GetTickCount64();
	m_u64TickHotCtrl=m_u64TickHotMon;
}

/*
 desc : �ı���
 parm : None
*/
CMainThread::~CMainThread()
{
	/* ���� �۾� �޸� ���� */
	ResetWorkJob();
}

/*
 desc : ������ ���۽ÿ� �ʱ� �ѹ� ȣ���
 parm : None
 retn : DI_TRUE or DI_FALSE
*/
BOOL CMainThread::StartWork()
{
	/* ������ 1 �� ���� ���� �����ϵ��� �� */
	CThinThread::Sleep(1000);

	return TRUE;
}

/*
 desc : ������ ���۽ÿ� �ֱ������� ȣ���
 parm : None
 retn : None
*/
VOID CMainThread::RunWork()
{
	/* ���� ���� */
	if (m_csSyncWork.Enter())
	{
		UINT64 u64Tick	= GetTickCount64();

		/* ���ķ����� ���� ������ ���, ���� ������ �����ϱ� ���� (�޽��� ��� Ȯ�� ����) */
		if (uvEng_GetConfig() && uvEng_GetConfig()->IsRunDemo() &&
			m_pWorkJob && m_pWorkJob->IsWorkStopped())
		{
			uvCmn_uSleep(100);
		}
		/* ���� ���� �۾� ���� ������ ��� */
		/* �ó����� ���� */
		if (m_pWorkJob && !m_pWorkJob->IsWorkStopped())
		{
			/* Luria ���� �۾��� ��� �˸� */
			if (m_pWorkJob->GetWorkJobID() == ENG_BWOK::en_gerb_load ||
				m_pWorkJob->GetWorkJobID() == ENG_BWOK::en_gerb_unload)
			{
				uvEng_Luria_SetWorkBusy(TRUE);
			}
			RunWorkJob();
		}
		else
		{
			/* �۾��� ���� ����� �˸� */
			uvEng_Luria_SetWorkBusy(FALSE);
		}
		/* ���� �۾� ������ ������ �߻� �ߴٸ� ... Abort ���� ���� */
		if (m_pWorkJob && m_pWorkJob->IsWorkError())
		{
			RunWorkJob(ENG_BWOK::en_work_stop);
		}
		/* �α� ������ ���� */
		else if (!m_pWorkJob || m_pWorkJob->IsWorkCompleted())
		{
			/* 1 �и��� ���� �̷� ���� ���� ���� */
			if ((m_u64TickFile + 60 * 1000) < u64Tick)
			{
				m_u64TickFile	= u64Tick;
				DeleteHistFiles();
			}
			/* Ư�� �ֱ⸶�� ��� ���� �µ� ������ ���� */
			if ((m_u64TickTemp + 10 * 1000) < u64Tick)
			{
				m_u64TickTemp	= u64Tick;
				//SaveTempFiles();
			}
			/* ������ �µ� ���� (Ư�� �ֱ⸶��... ���� ����) */
			if (uvEng_GetConfig()->auto_hot_air.use_auto_hot_air &&
				(m_u64TickHotMon + 5 * 1000) < u64Tick)
			{
				m_u64TickHotMon	= u64Tick;
				SetAutoHotAir(u64Tick);
			}
		}

		/* ���α׷� ���� �޽��� ó�� */
		if (m_bAppExit && (u64Tick > m_u64TickExit))
		{
			SendMesgParent(ENG_BWOK::en_app_exit, 100);
		}
		else
		{
			/* �θ𿡰� �ֱ������� �˸��� ��� ���� */
			if (m_pWorkJob && IsSetAlignedMark())	SendMesgParent(ENG_BWOK::en_mesg_mark, 100);
			else									SendMesgParent(ENG_BWOK::en_mesg_norm, 100);
		}
		/* ���� ���� */
		m_csSyncWork.Leave();
	}
}

/*
 desc : ������ ����ÿ� ���������� �ѹ� ȣ���
 parm : None
 retn : None
*/
VOID CMainThread::EndWork()
{
}

/*
 desc : ���� ���� ���� �۾� �޸� ����
 parm : None
 retn : None
*/
VOID CMainThread::ResetWorkJob()
{
	if (!m_pWorkJob)	return;
	delete m_pWorkJob;
	m_pWorkJob		= NULL;
#if 0
	/* ������ �ִ� ������ �ʱ�ȭ */
	m_u8AlignStepIt	= 0xff;
#endif
}

/*
 desc : �۾� ���� (�۾��� ����� ������ ����)
 parm : None
 retn : None
*/
VOID CMainThread::RunWorkJob()
{
#if 0	/* SKC Project���� �ϴ� ��� ����. �� �ΰ� �� */
	if (m_pWorkJob->IsWorkStopped())
	{
		if (CheckStartButton())	return;
	}
#endif
#if 0
	/* ���� ���� ��ư�� ���� ������� ���� */
	if (m_pWorkJob->IsWorkAbort())
	{
		ResetWorkJob();
		return;
	}
	else
#endif
	{
		/* �۾��� �Ҵ�Ǿ� �ְ�, �������� ���� ������ ��츸 ���� */
		if (!m_pWorkJob->IsWorkStopped())
		{
			switch (m_pWorkJob->GetWorkJobID())
			{
			case ENG_BWOK::en_work_stop		:
			case ENG_BWOK::en_work_home		:
			case ENG_BWOK::en_work_init		:

			case ENG_BWOK::en_gerb_load		:
			case ENG_BWOK::en_gerb_unload	:

			case ENG_BWOK::en_gerb_onlyfem:

			case ENG_BWOK::en_mark_move		:
			case ENG_BWOK::en_mark_test		:

			case ENG_BWOK::en_expo_only		:
			case ENG_BWOK::en_expo_align	:	
			case ENG_BWOK::en_gerb_expofem	:	m_pWorkJob->DoWork();	break;	/* ���� �۾� �� �ܰ� ��Ī ���� */
			}
		}
	}
	/* ���� �۾� ��Ī ���� */
	UpdateWorkName();
}

/*
 desc : �۾� ����
 parm : job_id	- [in]  ENG_BWOK ��
		data	- [in]  Job �� ������ ��
 retn : TRUE or FALSE
*/
BOOL CMainThread::RunWorkJob(ENG_BWOK job_id, PUINT64 data)
{
	BOOL bSucc	= FALSE;

	/* ���� ���� */
	if (m_csSyncWork.Enter())
	{
#if 0
		/* ���� ���� ���� �۾��� �ִٸ� ��� ��Ŵ */
		if (m_pWorkJob && !m_pWorkJob->IsWorkStopped() &&
			job_id == ENG_BWOK::en_work_stop)
		{
			/* ������ �۾� ��� ��Ű�� ���� �۾� �õ� */
			m_pWorkJob->SetWorkStop();
			bSucc = TRUE;
		}
		else
#endif
		{
			/* ���� �۾� �޸� ���� */
			ResetWorkJob();

			/* �۾� ���� ��� */
			switch (job_id)
			{
			case ENG_BWOK::en_work_stop		:
				if (!data)	m_pWorkJob = new CWorkAbort();
				else		m_pWorkJob = new CWorkAbort(UINT8(*data));							break;
			case ENG_BWOK::en_work_home		: m_pWorkJob = new CWorkHoming();					break;
			case ENG_BWOK::en_work_init		: m_pWorkJob = new CWorkInited();					break;

			case ENG_BWOK::en_gerb_load		: m_pWorkJob = new CWorkRecipeLoad(UINT8(*data));	break;
			case ENG_BWOK::en_gerb_unload	: m_pWorkJob = new CWorkRecipeUnload();				break;

			case ENG_BWOK::en_gerb_onlyfem	: m_pWorkJob = new CWorkOnlyFEM();					break;

			case ENG_BWOK::en_mark_move		: m_pWorkJob = new CWorkMarkMove(UINT8(*data));		break;
			//case ENG_BWOK::en_mark_test	: m_pWorkJob = new CWorkMarkTest();					break;
			case ENG_BWOK::en_mark_test		: 
			{
				auto workMarkTest = new CWorkMarkTest(LPG_CELA(data));
				m_pWorkJob = static_cast<CWorkStep*>(workMarkTest);
			}
			break;

			//case ENG_BWOK::en_expo_only		: m_pWorkJob = new CWorkExpoOnly(LPG_CPHE(data));	break;
			case ENG_BWOK::en_expo_only		: m_pWorkJob = new CWorkExpoOnly(LPG_CELA(data));	break;
			case ENG_BWOK::en_expo_align	: 
			{
				auto expoAlign = new CWorkExpoAlign();
				m_pWorkJob = static_cast<CWorkStep*>(expoAlign);
			}
			break;
			//case ENG_BWOK::en_expo_align: m_pWorkJob = new CWorkExpoAlign(LPG_CELA(data));

			case ENG_BWOK::en_gerb_expofem	: m_pWorkJob = new CWorkFEMExpo(HWND(data));		break;
			}

			if (!m_pWorkJob)	bSucc = FALSE;
			else
			{
				bSucc = TRUE;
				/* �ʱ�ȭ �۾� ���� */
				if (!m_pWorkJob->InitWork())
				{
					ResetWorkJob();
					bSucc = FALSE;
				}
				else
				{
					/* GUI ù ȭ�� ���� �˸� */
					switch (job_id)
					{
					case ENG_BWOK::en_mark_test		:
					case ENG_BWOK::en_expo_align	: SendMesgParent(ENG_BWOK::en_mesg_init, 100);	break;
					}
				}
			}
		}
		/* ���� ���� */
		m_csSyncWork.Leave();
	}

	return bSucc;
}

/*
 desc : �θ𿡰� �޽��� ���� (SendMessage)
 parm : msg_id	- [in]  �޽��� ID (0, 1, 2, 4)
		timeout	- [in]  SendMessag Lifetime (unit: msec)
 retn : None
*/
VOID CMainThread::SendMesgParent(ENG_BWOK msg_id, UINT32 timeout)
{
	LONG lParam			= IsBusyWorkJob() ? 1 : 0;
	DWORD_PTR dwResult	= 0;
	LRESULT lResult		= 0;

	/* �θ𿡰� �̺�Ʈ �޽��� ���� */
	lResult	= ::SendMessageTimeout(m_hParent, WM_MAIN_THREAD, WPARAM(msg_id), lParam,
								   SMTO_NORMAL, timeout, &dwResult);
	if (0 == lResult)
	{
#if 0
		/* ���� �߻��� ���� �� ���� */
		TRACE("MainThread : SendMessage Time out <Normal> = %d \n", GetLastError());
#endif
	}
}

/*
 desc : ���� �ֱٿ� Work Job�� ID ��� �� ��ȯ
 parm : None
 retn : ENG_BWOK �� ��ȯ
*/
ENG_BWOK CMainThread::GetWorkJobID()
{
	if (!m_pWorkJob)	return ENG_BWOK::en_work_none;
	return m_pWorkJob->GetWorkJobID();
}

/*
 desc : ���� �۾� ���� �̸� (��Ī)�� �޸𸮿� �ӽ� ����
 parm : None
 retn : None
*/
VOID CMainThread::UpdateWorkName()
{
	TCHAR tzName[64]	= {NULL}, *ptzState = NULL;
	TCHAR tzState[5][8]	= { L"N", L"W", L"S", L"T", L"F" };
	TCHAR tzWork[WORK_NAME_LEN]		= {NULL};

	/* Scenario �۾� ���� �� */
	if (m_pWorkJob->IsWorkStopped())
	{
		switch (m_pWorkJob->GetWorkJobID())
		{
		case ENG_BWOK::en_work_stop		: wcscpy_s(tzName, 64, L"All Work Stop");		break;
		case ENG_BWOK::en_work_home		: wcscpy_s(tzName, 64, L"All Motor Homing");	break;
		case ENG_BWOK::en_work_init		: wcscpy_s(tzName, 64, L"All Device Init");		break;

		case ENG_BWOK::en_gerb_load		: wcscpy_s(tzName, 64, L"Recipe Load");			break;
		case ENG_BWOK::en_gerb_unload	: wcscpy_s(tzName, 64, L"Recipe Unload");		break;
		case ENG_BWOK::en_gerb_onlyfem	: wcscpy_s(tzName, 64, L"Recipe FEM");		break;

		case ENG_BWOK::en_mark_move		: wcscpy_s(tzName, 64, L"MARK (Move)");			break;
		case ENG_BWOK::en_mark_test		: wcscpy_s(tzName, 64, L"MARK (Test)");			break;

		case ENG_BWOK::en_expo_only		: wcscpy_s(tzName, 64, L"EXPO (Only)");			break;
		case ENG_BWOK::en_expo_align	: wcscpy_s(tzName, 64, L"EXPO (Align)");		break;

		case ENG_BWOK::en_gerb_expofem	: wcscpy_s(tzName, 64, L"EXPO (FEM)");		break;
		}
	}
	/* ���� �۾� ���¿� ����, ù ���� ���� (����, Ÿ�Ӿƿ�, ����, ..) */
	switch (m_pWorkJob->GetWorkState())
	{
	case ENG_JWNS::en_next	:	ptzState = tzState[0];	break;
	case ENG_JWNS::en_wait	:	ptzState = tzState[1];	break;
	case ENG_JWNS::en_comp	:	ptzState = tzState[2];	break;
	case ENG_JWNS::en_time	:	ptzState = tzState[3];	break;
	case ENG_JWNS::en_error	:	ptzState = tzState[4];	break;
	}

	/* �� �۾� �� ���� */
	if (ptzState)
	{
		if (!m_pWorkJob->IsWorkCompleted())
		{
			swprintf_s(tzWork, WORK_NAME_LEN, L"[%s] %s : %s",
					   ptzState, tzName, m_pWorkJob->GetStepName());
		}
		else
		{
			swprintf_s(tzWork, WORK_NAME_LEN, L"[%s] %s : Completed", ptzState, tzName);
		}
	}
	uvEng_SetWorkStepName(tzWork);
}

/*
 desc : ���� DI ��� ���� Start Button�� ���������� ���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::CheckStartButton()
{
	/* ���� DI ��� ���� �г��� �뱤 ��ư 2���� ������ (Push) �ִ� �������� ���� */
	// by sysandj : MCQ��ü �߰� �ʿ�
// 	if (0x01 == uvEng_ShMem_GetPLCExt()->r_start_button)
// 	{
// 		m_u8StartButton	= 0x01;
// 	}
	/* ���� �г��� �뱤 ��ư 2���� ���� (Push)�� ��, �ٽ� ���������� (Pull) ���� */
	// by sysandj : MCQ��ü �߰� �ʿ�
	//if (m_u8StartButton == 0x01 && uvEng_ShMem_GetPLCExt()->r_start_button == 0x00)
	{
		m_u8StartButton	= 0x00;

		/* �ݵ�� �̺�Ʈ �޽����� ���� */
		SendMesgParent(ENG_BWOK::en_mesg_expo, 100);

		return TRUE;
	}

	return FALSE;
}

/*
 desc : ���� �۾� ������ ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::IsBusyWorkJob()
{
	if (!m_pWorkJob)	return FALSE;

	return m_pWorkJob->IsWorkStopped() ? FALSE : TRUE;
}

/*
 desc : �˻簡 ����� Align Mark�� �ִ��� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::IsSetAlignedMark()
{
	if (m_pWorkJob->GetUIRefresh())
	{
		m_pWorkJob->SetUIRefresh(false);
		return TRUE;
	}
	else
	return FALSE;
}
/*
 desc : �α� �� ��Ÿ ���� ����
 parm : None
 retn : None
*/
VOID CMainThread::DeleteHistFiles()
{
	TCHAR tzPath[MAX_PATH_LEN]	= {NULL};

	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\save_img\\cali", g_tzWorkDir);
	uvCmn_DeleteHistoryFiles(tzPath, 31);
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\save_img\\edge", g_tzWorkDir);
	uvCmn_DeleteHistoryFiles(tzPath, 31);
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\save_img\\grab", g_tzWorkDir);
	uvCmn_DeleteHistoryFiles(tzPath, 31);

	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\save_img\\mark", g_tzWorkDir);
	uvCmn_DeleteHistoryFiles(tzPath, 31);

	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\save_img\\live", g_tzWorkDir);
	uvCmn_DeleteHistoryFiles(tzPath, 31);


	//swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\logs", g_tzWorkDir);
	//uvCmn_DeleteHistoryFiles(tzPath, 100);

	//swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\temp", g_tzWorkDir);
	//uvCmn_DeleteHistoryFiles(tzPath, 100);
}

/*
 desc : Ư�� �ֱ⸶�� ��� ������ �µ� ���� ���Ϸ� ����
 parm : None
 retn : None
*/
VOID CMainThread::SaveTempFiles()
{
	UINT8 i;
	TCHAR tzResult[512]	= {NULL}, tzFile[MAX_PATH_LEN] = {NULL};
	UINT16 (*pLed)[8]	= uvEng_ShMem_GetLuria()->directph.light_source_driver_temp_led;
	UINT16 (*pBoard)[8]	= uvEng_ShMem_GetLuria()->directph.light_source_driver_temp_board;
	SYSTEMTIME stTm		= {NULL};
#if 0
	/* Demo ���� ������ ��� ���� */
	if (uvEng_GetConfig()->IsRunDemo())	return;
#endif
	/* ���� ��ǻ�� ��¥�� ���ϸ����� ���� */
	GetLocalTime(&stTm);
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\logs\\temp\\%04d-%02d-%02d.csv",
			   g_tzWorkDir, stTm.wYear, stTm.wMonth, stTm.wDay);
	/* ���࿡ ������ �ϴ� ������ ������, �ش� ���� ���� ��, Ÿ��Ʋ (Field) �߰� */
	if (!uvCmn_FindFile(tzFile))
	{
		wcscpy_s(tzResult, 512,
				 L"DATE,HOT,TEMP1,TEMP2,TEMP3,TEMP4,"
				 L"PLD(11),PLD(12),PLD(13),PLD(14),PBD(11),PBD(12),PBD(13),PBD(14),"
				 L"PLD(21),PLD(22),PLD(23),PLD(24),PBD(21),PBD(22),PBD(23),PBD(24),\n");
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x00);
	}

	/* �߻� �ð� / ������ �̸� / �Ź� / �۾� ���� ���� / DI ���� �µ� (4 ��) �� ���� ���� */
	
	/* ���а� LED / Board �µ� �� ���� */
	for (i=0x00; i<0x02; i++)
	{
		swprintf_s(tzResult, 512, L"%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,",
				   pLed[i][0]/10.0f,  pLed[i][1]/10.0f,	 pLed[i][2]/10.0f,	pLed[i][3]/10.0f,
				   pBoard[i][0]/2.0f,pBoard[i][1]/2.0f,pBoard[i][2]/2.0f,pBoard[i][3]/2.0f);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	/* �������� ������ ���� �������� �Ѿ���� �ϱ� ���� */
	uvCmn_SaveTxtFileW(L"\n", (UINT32)wcslen(L"\n"), tzFile, 0x01);
}

/*
 desc : ���α׷� ���� ó��
 parm : delay	- [in]  �� �ð� (����: msec) ���� �θ� �������� ���� �޽��� �˸�
 retn : None
*/
VOID CMainThread::SetExitApp(UINT64 delay)
{
	/* ���� ���� */
	if (m_csSyncWork.Enter())
	{
		m_u64TickExit	= GetTickCount64() + delay;
		m_bAppExit	= TRUE;

		/* ���� ���� */
		m_csSyncWork.Leave();
	}
}

/*
 ���� : ������ �µ� �ڵ� ����
 ���� : tick	- [in]  ���� CPU ���� Time
 ��ȯ : None
*/
VOID CMainThread::SetAutoHotAir(UINT64 tick)
{
	/* ������ ���� ���ǿ� ���յǴ� �ð� (��)�� �Ǿ����� Ȯ���ϱ� */
	UINT64 u64TickHold	= UINT64(uvEng_GetConfig()->auto_hot_air.temp_holding_time) * 60 * 1000;	/* min -> msec */
#if 0
	u64TickHold	= 5 * 1000;	/* fixed 5 sec */
#endif
	if (!((tick - m_u64TickHotCtrl) > u64TickHold))	return;

	/* ȯ�� ������ DI ���� ������ ������ ���� ���� ���� �� �������� */
	DOUBLE dbGetHotRange= uvEng_GetConfig()->auto_hot_air.auto_temp_range;	/* unit: �� */
	/* ���� �����Ǵ� �µ� ���� DI ���� ���� �µ� ���� ���� (���: �µ� ��� �ǹ�, ����: �µ� �϶� �ǹ�)*/
	DOUBLE dbTempDiffDI = 0; // by sysandj : MCQ��ü �߰� �ʿ� : uvEng_MCQ_GetTempDI(0) - uvEng_GetConfig()->temp_range.di_internal[0];
	/* ������ ������ �ʿ��� �������� Ȯ�� (���ο� ������ range �� ���� ũ��, ������ ���� �µ� �缳��) */
	if (abs(dbTempDiffDI) > dbGetHotRange)
	{
		/* ������ ������ �ʿ��� ���� ���� �ð� ���� (�� �ð����� u64TickHold �ð� ���� ���� ������� ����) */
		m_u64TickHotCtrl	= tick;
		/* ������ ������ �µ� �� ����ϱ� (DI ���� �µ��� ���� �µ� ���� ���� ����, ������ �µ� ���� �� ���ϱ�) */
		DOUBLE dbDiffHotAir	= uvEng_GetConfig()->auto_hot_air.auto_temp_rate/100.0f * dbTempDiffDI;
		/* ���� �����⿡ ������ ���� ���� ���� ����� �� (!!! ������ �µ� ������ �ݴ�� ����� �� !!!) */
		/* DI ���� �µ��� ����ϸ�, ������ �µ��� ������� �ǰ�, DI ���� �µ��� �϶��ϸ� ������ �µ��� ��� ���Ѿ� �� */
		DOUBLE dbSetHotAir = 0;// by sysandj : MCQ��ü �߰� �ʿ� : uvEng_MCQ_GetHotAirTempSetting() - dbDiffHotAir;
		/* PLC�� ����, ������ ���� �µ� �� �缳�� (�����ϰ� 15 ~ 30 ���� �ȿ����� �����ϵ��� ��) */
		if (dbSetHotAir > 20.0f && dbSetHotAir < 30.0f)
		{
			// by sysandj : MCQ��ü �߰� �ʿ�
			//uvEng_MCQ_SetHotAirTempSetting(dbSetHotAir);
		}
	}
	/* ������ ������ �ʿ��� ������ �ƴϸ�... �µ� ���� �ð��� �ʱ�ȭ */
	else
	{
		/* �ֱ� �ð����� ���� */
		m_u64TickHotCtrl	= tick;
	}
}
