
/*
 desc : Main Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "MainThread.h"
#include "./Meas/Measure.h"


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
	m_hParent			= parent;
	m_u8WorkStep		= 0x00;
	m_bRunFocus			= FALSE;
	m_u64PeridPktTime	= 0;
	/* ������ ���� ������ �м� �� ���� �����ϴ� �ڽ� ���̾�α� ��ü */
	m_pMeasure			= new CMeasure();
	ASSERT(m_pMeasure);
}

/*
 desc : �ı���
 parm : None
*/
CMainThread::~CMainThread()
{
	/* ���� ������ ������ �޸� ���� */
	if (m_pMeasure)	delete m_pMeasure;
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
	/* ���� ���� */
	if (m_syncFocus.Enter())
	{
		/* ��Ŀ�� �۾� ���� */
		if (m_bRunFocus)	RunFocus();
		else				ReqPeriodPkt();

		/* ���� ���� */
		m_syncFocus.Leave();
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
 desc : �θ𿡰� �޽��� ���� (SendMessage) - Step Result ��� ��
 parm : result	- [in]  ��� �ڵ� ��
		timeout	- [in]  SendMessag Lifetime (unit: msec)
 retn : None
*/
VOID CMainThread::SendMesgResult(WPARAM result, UINT32 timeout)
{
	DWORD_PTR dwResult	= 0;
	LRESULT lResult		= 0;

	/* �θ𿡰� �̺�Ʈ �޽��� ���� */
	lResult	= ::SendMessageTimeout(m_hParent, WM_MAIN_THREAD, result, 0L,
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
 desc : ��Ŀ�� ���� ���� �Ű� ���� ���� �� ���� ����
 parm : step_s	- [in]  Y �� �̵� Ƚ��
		repeat	- [in]  ���� �������� �ݺ� �����Ǵ� Ƚ��
		step_y	- [in]  Y �� ��� �̵� ���� (UP) (����: mm)
 retn : None
*/
VOID CMainThread::StartFocus(UINT16 step_s, UINT16 repeat, DOUBLE step_y)
{
	/* ���� ���� */
	if (m_syncFocus.Enter())
	{
		m_u16RetryGrab	= 0;

		m_u8PhNext		= 0x01;
		m_u16StepYNext	= 0x0001;
		m_u16RepeatNext	= 0x0001;

		m_u16StepYTotal	= step_s;
		m_u16RepeatTotal= repeat;

		m_u8WorkStep	= 0x01;
		m_u8WorkTotal	= 0x05;

		m_bRunFocus		= TRUE;

		/* ���� ���� */
		m_syncFocus.Leave();
	}

	/* ���� ������ ������ �޸� ���� */
	m_pMeasure->ResetResult();
}

/*
 desc : �۾� ������ ����
 parm : None
 retn : None
*/
VOID CMainThread::StopFocus()
{
	/* ���� ���� */
	if (m_syncFocus.Enter())
	{
		m_bRunFocus	= FALSE;
		/* ���� ���� */
		m_syncFocus.Leave();
	}
}

/*
 desc : ��Ŀ�� �۾� ����
 parm : None
 retn : None
*/
VOID CMainThread::RunFocus()
{
	switch (m_u8WorkStep)
	{
	/* ���� ��ġ�� �̵� */
	case 0x01	:	m_enWorkState = SetMeasureMoving();		break;
	case 0x02	:	m_enWorkState = IsMeasureMoved();		break;
	/* Trigger 1�� �߻� */
	case 0x03	:	m_enWorkState = PutOneTrigger();		break;
	/* Grabbed Image ���� �� Edge Detect ���� */
	case 0x04	:	m_enWorkState = ProcFindModel();		break;
	/* ��� ������ ���� �� �м� ���� */
	case 0x05	:	m_enWorkState = ResultDetectImage();	break;
	}

	/* ���� �۾� ���� ���� �Ǵ� */
	SetWorkNext();
	/* ��ð� ���� ���� ��ġ�� �ݺ� �����Ѵٸ� ���� ó�� */
	if (IsWorkTimeOut())
	{
		if (m_syncFocus.Enter())
		{
			m_bRunFocus		= FALSE;
			m_enWorkState	= ENG_JWNS::en_error;
			/* ���� ���� */
			m_syncFocus.Leave();
		}
	}
}

/*
 desc : ���� �ܰ�� �̵��ϱ� ���� ó��
 parm : None
 retn : None
*/
VOID CMainThread::SetWorkNext()
{
	/* �۾��� �����̰ų� ����Ǿ��� �� */
	if (ENG_JWNS::en_error == m_enWorkState)
	{
		m_u8WorkStep	= 0x00;
		m_bRunFocus		= FALSE;
		return;
	}

	if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* ��� �۾��� ������ ���� ��� */
		if (m_u8WorkTotal != m_u8WorkStep)
		{
			m_u8WorkStep++;	/* ���� �۾� �ܰ�� �̵� */
		}
		/* ��� �۾��� �Ϸ�� ��� */
		else
		{
#if 0	/* ReqPeriodPkt �Լ����� �ֱ������� ȣ���� */
			if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
			{
				/* ���� ���а� ���� �� ��û */
				uvEng_Luria_ReqGetMotorAbsPositionAll();
			}
#endif
			/* ���� Step �������� �ݺ� ������ ��� �Ͽ����� ���� */
			if (m_u16RepeatNext != m_u16RepeatTotal)
			{
				m_u16RepeatNext++;
				m_u8WorkStep	= 0x03;	/* Trigger �߻� ��ġ�� �̵� */
			}
			/* ���� ���а��� Y �� ���� (UP) �̵��� ��� �Ϸ� �Ǿ����� ���� */
			else if (m_u16StepYNext != m_u16StepYTotal)
			{
				m_u16StepYNext++;
				m_u16RepeatNext	= 0x01;
				m_u8WorkStep	= 0x01;
			}
			/* ���а� (Photohead) ������ŭ ������ �� �Ǿ����� ���� */
			else if (m_u8PhNext != uvEng_GetConfig()->luria_svc.ph_count)
			{
				m_u8PhNext++;
				m_u16RepeatNext	= 0x01;
				m_u16StepYNext	= 0x01;
				m_u8WorkStep	= 0x01;
			}
			else
			{
				m_enWorkState	= ENG_JWNS::en_comp;
				m_u8WorkStep	= 0x00;
				m_bRunFocus		= FALSE;
			}
		}
		/* ���� �ֱٿ� Waiting �� �ð� ���� */
		m_u64DelayTime	= GetTickCount64();
	}
}

/*
 desc : ���� �ð� ���� �۾��� ����ϱ� ����
 parm : time	- [in]  �ּ����� ��⿡ �ʿ��� �ð� (����: msec)
 retn : None
*/
VOID CMainThread::SetWorkWaitTime(UINT64 time)
{
	m_u64WaitTime	= GetTickCount64() + time;
}

/*
 desc : ���� �ð� ���� �۾��� ��� �Ϸ� �ߴ��� ����
 parm : title	- [in]  �۾� (�ܰ�) ���� ��� ����
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsWorkWaitTime()
{
	return GetTickCount64() > m_u64WaitTime ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : ��ð� ������ ��ġ���� ��� ������ ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::IsWorkTimeOut()
{
#ifdef _DEBUG
	return	GetTickCount64() > (m_u64DelayTime + 60000);	/* 60 �� �̻� �����Ǹ� */
#else
	return	GetTickCount64() > (m_u64DelayTime + 20000);	/* 20 �� �̻� �����Ǹ� */
#endif
}

/*
 desc : Focus �۾��� �Ϸ� �Ǿ����� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::IsCompleted()
{
	/* ��°�� ����Ŵϱ�... �Ϸ�� ������ �Ǵ� */
	if (!m_bRunFocus && (m_u8WorkTotal == m_u8WorkStep) &&
		(m_u8PhNext == uvEng_GetConfig()->luria_svc.ph_count))	return TRUE;

	return FALSE;
}

/*
 desc : ������� ������ ������ ��ȯ
 parm : ph_no	- [in]  Photohead Number (1 ~ 8)
		index	- [in]  ���������� �ϴ� �޸� ��ġ (Zero-based)
 retn : index ��ġ�� ����� ���� ��
*/
LPG_ZAAL CMainThread::GetGrabData(UINT8 ph_no, UINT32 index)
{
	LPG_ZAAL pstData	= NULL;

	/* ���� ���� */
	if (m_syncFocus.Enter())
	{
		pstData	= m_pMeasure->GetResult(ph_no, index);

		/* ���� ���� */
		m_syncFocus.Leave();
	}

	return pstData;
}

/*
 desc : ���� �ֱٿ� ������ ������ ��ȯ
 parm : None
 retn : ��� ������ ��ȯ
*/
LPG_ZAAL CMainThread::GetLastGrabData()
{
	LPG_ZAAL pstData	= NULL;

	/* ���� ���� */
	if (m_syncFocus.Enter())
	{
		pstData	= m_pMeasure->GetLastResult();

		/* ���� ���� */
		m_syncFocus.Leave();
	}

	return pstData;
}

/*
 desc : ���� ����� Edge Image�� CImage ��ü ������ ��ȯ
 parm : None
 retn : CImage ��ü ������
*/
CMeasure *CMainThread::GetMeasure()
{
	return m_pMeasure;
}

/*
 desc : ������� �۾� ������ ��ȯ
 parm : None
 retn : �۾� ������ ��ȯ (����: percent)
*/
DOUBLE CMainThread::GetWorkStepRate()
{
#if 1
	DOUBLE dbBase	= (m_u16RepeatTotal + m_u8WorkTotal * 1.0f) * m_u16StepYTotal;
	
	DOUBLE dbTotal	= dbBase * uvEng_GetConfig()->luria_svc.ph_count;
	DOUBLE dbCurrent= (dbBase * (m_u8PhNext - 1)) +
					  (m_u16RepeatTotal + m_u8WorkTotal * 1.0f) * (m_u16StepYNext - 1) +
					  (m_u16RepeatNext + m_u8WorkStep * 1.0f);
#else
	DOUBLE dbTotal	= (m_u16RepeatTotal + m_u8WorkTotal * 1.0f) * m_u16StepYTotal;
	DOUBLE dbCurrent= (m_u16RepeatTotal + m_u8WorkTotal * 1.0f) * (m_u16StepYNext - 1) +
					  (m_u16RepeatNext + m_u8WorkStep * 1.0f);
#endif
	return dbCurrent / dbTotal * 100.0f;
}

/*
 desc : ��ũ ���� ��ġ�� �̵�
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetMeasureMoving()
{
	DOUBLE dbStageX, dbStageY, dbACamX;
	DOUBLE dbVelo		= uvEng_GetConfig()->mc2_svc.move_velo;
	LPG_CPFI pstPhFocus	= &uvEng_GetConfig()->ph_focus;

	/* ���� Motion X/Y, Align Camera X 1 �� �� Toggle �� �ӽ� ���� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
	/* ����� ī�޶� 1�� ��ũ ���� ��ġ�� �̵� */
	dbACamX		= pstPhFocus->mark2_acam_x[0];
	/* ���� ���� �������� ������ ��� X / Y ��ġ ���� */
	dbStageX	= pstPhFocus->mark2_stage[0] +
				  ((m_u8PhNext - 1) * pstPhFocus->mark_period);
	dbStageY	= pstPhFocus->mark2_stage[1] +
				  ((m_u16StepYNext - 1) * pstPhFocus->step_move_y);

	/* ��� �̵� ���� */
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x,	dbStageX,	dbVelo);
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y,	dbStageY,	dbVelo);
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1,dbACamX,	dbVelo);

	return ENG_JWNS::en_next;
}

/*
 desc : ��ũ ���� ��ġ�� �̵��ߴ��� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsMeasureMoved()
{
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_x))		return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))		return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1))	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : Ʈ���� 1�� �߻� ��Ŵ
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::PutOneTrigger()
{
	/* ���� Ʈ���� ��� ���ۿ� ���� ���� ���ϰ� ��� ������ ���� */
	/* Trigger �۽� ���ۿ� �����ϰ� ���� ��ɾ ����Ǵ� �� ���� */
	if (!uvEng_Trig_IsSendCmdCountUnder(3))	return ENG_JWNS::en_wait;

	/* Align Camera is Edge Detection Mode */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);

	/* ���� Live & Edge & Calibration ������ �ʱ�ȭ */
	uvEng_Camera_ResetGrabbedImage();

	/* Trigger ������ (��������) �߻� */
	if (!uvEng_Trig_ReqTrigOutOne(0x01/*fixed*/, TRUE))
	{
		AfxMessageBox(L"Failed to send the event for trigger", MB_ICONSTOP|MB_TOPMOST);
//		return ENG_JWNS::en_error;
		/* ���� �ð����� ���� (Timeout �߻����� �ʵ��� �ϱ� ����) */
		m_u64DelayTime	= GetTickCount64();
		/* �ٽ� Ʈ���� �߻���Ű�� ���� */
		m_u8WorkStep	-= 1;
		return ENG_JWNS::en_next;
	}

	/* �̹��� Grabbed Ȯ�� �ð� ���� */
	m_u64GrabTime	= GetTickCount64();

	return ENG_JWNS::en_next;
}

/*
 desc : ���� Grabbed Image ��, Model Find�� �̹����� �ִ��� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::ProcFindModel()
{
	/* Grabbed Image�� ������, ��Ī�� ����� �ִ��� ���� */
	if (!uvEng_Camera_RunModelFind(0x01))
	{
		/* 3 �� ���� Grabbed Image�� ���� ���, �ٽ� Ʈ���� �߻� ��Ű�� ���� �ڷ� �̵� */
		if (!(m_u64GrabTime+1000 < GetTickCount64()))	return ENG_JWNS::en_wait;
		/* ���� ���� ���� Ƚ�� ���� ó�� */
		m_u16RetryGrab++;
		/* �ٽ� �̹��� Grbbaed �۾� �����϶�� ���� */
		m_u8WorkStep	-= 2;
		/* ���� Ƚ�� �̻� �ν� �������� ���� ó�� */
		if (m_u16RetryGrab > 10)
		{
			SendMesgResult(MSG_ID_STEP_FIND_FAIL);
			return ENG_JWNS::en_error;
		}
		return ENG_JWNS::en_next;
	}

	/* Grabbed Image�� ������, Edge ��� �� �˻� �������� ���� */
	if (!uvEng_Camera_GetLastGrabbedMark())
	{
		SendMesgResult(MSG_ID_STEP_GRAB_FAIL);
		return ENG_JWNS::en_error;
	}

	/* ���������� ó�� ������ �ʱ�ȭ */
	m_u16RetryGrab	= 0x00;

	return ENG_JWNS::en_next;
}

/*
 desc : ���� �� Edge �νĵ� ��� ���� �м�
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::ResultDetectImage()
{
	/* ���� �˻� ��� ó�� */
	if (!m_pMeasure->SetResult(0x01, m_u8PhNext, m_u16StepYNext))	return ENG_JWNS::en_error;
	/* ���� �θ𿡰� �˸� */
	SendMesgResult(MSG_ID_STEP_RESULT);
#if 0
	TRACE(L"PH [%d], Step Y [%d], Count [%d]\n", m_u8PhNext, m_u16StepYNext, m_pMeasure->GetCount(m_u8PhNext));
#endif
	return ENG_JWNS::en_next;
}

/*
 desc : �뱤 �۾��� ���� ��, �ֱ������� ��û (�ַ� Luria Side)
 parm : None
 retn : None
*/
VOID CMainThread::ReqPeriodPkt()
{
	UINT64 u64Tick	= GetTickCount64();

	/* ���� �ֱ⸶�� ȣ���ϵ��� �ϱ� ���� (1�ʿ� n�� Ƚ������ �۽��ϱ� ����) */
	if (u64Tick < m_u64PeridPktTime + 1000/4)	return;

	/* ���� �ֱٿ� ȣ���� �ð� ���� */
	m_u64PeridPktTime	= u64Tick;
#if 0
	/* Motion Position ��û */
	if (m_bReqPeriodMC2)	uvEng_MC2_ReadReqPktActAll();
	else					uvEng_MC2_ReadReqPktRefAll();
	m_bReqPeriodMC2	= !m_bReqPeriodMC2;
#endif
	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* Luria State ��û */
		uvEng_Luria_ReqGetMotorAbsPositionAll();
	}
}