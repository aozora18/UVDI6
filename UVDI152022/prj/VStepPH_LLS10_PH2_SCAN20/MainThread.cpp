
/*
 desc : Main Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "MainThread.h"


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
CMainThread::CMainThread(HWND parent, LPG_ACGR grab)
	: CThinThread()
{
	m_hParent		= parent;
	m_bRunMeasure	= FALSE;
	m_pstGrabData	= grab;
	m_u8GridCheck	= 0x00;
	m_u8GridCount	= 0x00;
	m_u64WorkTime	= GetTickCount64();
	m_u64ReqTime	= GetTickCount64();
}

/*
 desc : �ı���
 parm : None
*/
CMainThread::~CMainThread()
{
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
	/* �Ϲ����� ����͸� ȣ�� */
	SendMesgNormal(100);

	/* ���� �۾� ���� */
	if (m_bRunMeasure)
	{
		m_bRunMeasure = DoMeasure();
	}
	else
	{
		/* �Ź� ������ ��ϵǾ� �ִ��� ���θ� Ȯ���ϱ� ���� �ֱ������� ��û ? */
		if (GetTickCount64()+5000)
		{
			/* ���� �ֱ� ��û�� �ð� ���� */
			m_u64ReqTime = GetTickCount64();
			uvEng_Luria_ReqGetJobList();
		}
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
 desc : �θ𿡰� �޽��� ���� (SendMessage)
 parm : timeout	- [in]  SendMessag Lifetime (unit: msec)
 retn : None
*/
VOID CMainThread::SendMesgNormal(UINT32 timeout)
{
	DWORD_PTR dwResult	= 0;
	LRESULT lResult		= 0;

	/* �θ𿡰� �̺�Ʈ �޽��� ���� */
	lResult	= ::SendMessageTimeout(m_hParent, WM_MAIN_THREAD, MSG_ID_NORMAL, 0L,
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
 desc : Grabbed Result ����
 parm : result	- [in]  ���� ����
		timeout	- [in]  SendMessag Lifetime (unit: msec)
 retn : None
*/
VOID CMainThread::SendMesgResult(BOOL result, UINT32 timeout)
{
	DWORD_PTR dwResult	= 0;
	LRESULT lResult		= 0;
	WPARAM wParam		= result ? MSG_ID_RESULT_SUCC : MSG_ID_RESULT_FAIL;

	/* �θ𿡰� �̺�Ʈ �޽��� ���� */
	lResult	= ::SendMessageTimeout(m_hParent, WM_MAIN_THREAD, wParam, 0L,
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
 desc : ���� �۾� ����
 parm : run	- [in]  TRUE or FALSE
 retn : None
*/
VOID CMainThread::RunMeasure(BOOL run)
{
	if (m_syncRun.Enter())
	{
		m_u8Step		= 0x01;
		m_bRunMeasure	= run;
		m_u64StepTime	= GetTickCount64();

		m_syncRun.Leave();
	}
}

/*
 desc : �۾� ��� �ð� ����
 parm : wait	- [in]  ��� �ð� (����: msec)
 retn : None
*/
VOID CMainThread::SetWaitTime(UINT64 wait)
{
	m_u64WorkTime	= (UINT32)GetTickCount();
	m_u64WaitTime	= wait;
}

/*
 desc : ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::DoMeasure()
{
	BOOL bSucc	= FALSE;

	/* ���� �ð� ��� */
	if (!(GetTickCount64() > m_u64StepTime + 100))	return TRUE;
	m_u64StepTime	= GetTickCount64();

	switch (m_u8Step)
	{
	case 0x01	: bSucc = PutOneTrigger();	break;
	case 0x02	: bSucc = GrabbedImage();	break;
	case 0x03	: bSucc = MotionMoving();	break;
	case 0x04	: bSucc = IsMotionMoved();	break;
	default		: return FALSE;
	}

	if (bSucc)
	{
		if (m_u8Step == 0x04)
		{
			if (!m_bResultSucc)	m_u8Step	= 0x00;
			else				SendMesgResult(TRUE);	/* �θ𿡰� �˸� */
		}

		m_u8Step++;
	}
	else
	{
		/* ���� �ð� ���� ������ ���� ���, ���� ó�� */
		bSucc	= TRUE;
		if ((UINT32)GetTickCount() > (m_u64WorkTime+m_u64WaitTime))
		{
			bSucc	= FALSE;
			SendMesgResult(FALSE);
		}
		uvCmn_uSleep(500000);	/* 0.5 �� ���� ��� */
	}

	return bSucc;
}

/*
 desc : Ʈ���� 1�� �߻� ��Ŵ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::PutOneTrigger()
{
	UINT8 u8ChNo= 0x01;

	/* ���� Live & Edge & Calibration ������ �ʱ�ȭ */
	uvEng_Camera_ResetGrabbedImage();
	if (!uvEng_Trig_ReqTrigOutOne(0x01, FALSE))
	{
		AfxMessageBox(L"Failed to send the event for trigger", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* Grabbed Image�� ó���Ǵ� �ð� ��� */
	SetWaitTime(5000);

	return TRUE;
}

/*
 desc : Grabbed Image ó��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::GrabbedImage()
{
	BOOL bIsChanged		= FALSE;
	DOUBLE dbCentOffset = uvEng_GetConfig()->ph_step.center_offset;

	/* �۾� �Ϸ� ���� �������� ���� */
	m_bResultSucc	= FALSE;
	/* Grabbed Image�� �����ϴ��� Ȯ�� */
	m_pstGrabData[0].marked	= 0x00;
	m_pstGrabData[1].marked	= 0x00;
	if (!uvEng_Camera_RunModelStep(0x01, MODEL_FIND_COUNT, FALSE, m_pstGrabData))
	{
		return FALSE;
	}

	/* ����� ī�޶� ȸ���Ǿ� �ִٸ� ... (180�� �̻�) */
	if (uvEng_GetConfig()->set_cams.acam_inst_angle)
	{
		bIsChanged	= m_pstGrabData[0].mark_cent_mm_x < m_pstGrabData[1].mark_cent_mm_x;
	}
	else
	{
		bIsChanged	= m_pstGrabData[0].mark_cent_mm_x > m_pstGrabData[1].mark_cent_mm_x;
	}
	/* X ��ǥ ���� ���� ������ ���ġ */
	if (bIsChanged)
	{
		STG_ACGR stTemp	= {NULL};
		memcpy(&stTemp,				&m_pstGrabData[0],	sizeof(STG_ACGR));
		memcpy(&m_pstGrabData[0],	&m_pstGrabData[1],	sizeof(STG_ACGR));
		memcpy(&m_pstGrabData[1],	&stTemp,			sizeof(STG_ACGR));
	}
	/* ���� ���� �ȵƴٸ� ... */
	if (!(m_pstGrabData[0].marked && m_pstGrabData[1].marked))
	{
		AfxMessageBox(L"Failed to find the mark info", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ���� Grabbed Image 2���� �߽� ���� ������ ����� ���ϰ�, 1��° ī�޶��� ���� �̵� �Ÿ� ���� */
	m_dbCentMove = m_pstGrabData[0].mark_cent_mm_dist - m_pstGrabData[1].mark_cent_mm_dist;
#if 1
	TRACE(L"m_dbCentMove = %.4f, dbCentOffset = %.4f mm\n", m_dbCentMove, dbCentOffset);
#endif
	if (abs(m_dbCentMove) < dbCentOffset)	/* 1.0 mm �̳��� �ִ��� ���� */
	{
		/* �۾� ���� */
		m_bResultSucc	= TRUE;
	}

	return TRUE;
}

/*
 desc : �̹����� �߽ɿ� ������ ��� �̵�
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::MotionMoving()
{
	DOUBLE dbMove	= 0.0f, dbVelo;
#if 0
	/* ���� ��ġ */
	TRACE(L"[start] acam_1 =  %.4f\n", uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1));
#endif
	/* ��� ��ġ ���� ���� */
	if (!m_bResultSucc)
	{
		/* ���� ���õ� ����� ī�޶� �̵��ϱ� ���� �� ���� */
		dbMove	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1) - m_dbCentMove/2.0f;
		/* ���� ����� ī�޶��� Toggle �� ���� */
		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
		/* ���� ���õ� ����� ī�޶��� �̵� �ӵ� �� ��� */
		dbVelo	= uvEng_GetConfig()->mc2_svc.step_velo;

		/* ī�޶� ������ ��ġ�� �̵� */
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, dbMove, dbVelo))
		{
			AfxMessageBox(L"Failed to move the motion drive of alignment camera", MB_ICONSTOP|MB_TOPMOST);
			return FALSE;
		}
	}
	/* ���� ���� ��ġ�� �̵� */
	else
	{
		/* �ڵ����� ���� ��ġ�� �̵�. Set the grid üũ ��ư�� üũ�� ��츸 �ش��. */
		if (m_u8GridCheck)
		{
			dbMove	= uvEng_GetConfig()->ph_step.stripe_width;
#if 0
			if (++m_u8GridCount == 0x06)
			{
				dbMove	+= uvEng_GetConfig()->ph_step.stripe_width / 3;
			}
#else
			m_u8GridCount++;
#endif
			/* ���� ����� ī�޶��� Toggle �� ���� */
			uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
			/* ���� ���� ��ġ�� �̵� */
			uvEng_MC2_SendDevRefMove(ENG_MMMM::en_move_step, ENG_MMDI::en_align_cam1,
									 ENG_MDMD::en_move_right, dbMove);
		}
	}

	/* ��� ���� ���� ��� �ð� */
	SetWaitTime(5000);

	return TRUE;
}

/*
 desc : ���������� �̵��� �Ϸ� �Ǿ����� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::IsMotionMoved()
{
	if (!m_u8GridCheck)	return TRUE;
	/* ���������� �̵��� �Ϸ� �Ǿ��ٸ� ���� ���� ���� */
	return (uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1));
}
