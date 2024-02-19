
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
	m_hParent		= parent;
	m_u8WorkStep	= 0x00;
	m_bRunFocus		= FALSE;
	m_u8RetryTrig	= 0x00;
	/* ������ ���� ������ �м� �� ���� �����ϴ� �ڽ� ���̾�α� ��ü */
	m_pMeasure		= new CMeasure();
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

	m_u8RetryTrig	= 0x00;

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
 parm : timeout	- [in]  SendMessag Lifetime (unit: msec)
 retn : None
*/
VOID CMainThread::SendMesgResult(UINT32 timeout)
{
	DWORD_PTR dwResult	= 0;
	LRESULT lResult		= 0;

	/* �θ𿡰� �̺�Ʈ �޽��� ���� */
	lResult	= ::SendMessageTimeout(m_hParent, WM_MAIN_THREAD, MSG_ID_STEP_RESULT, 0L,
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
 parm : cam_id	- [in]  ī�޶� �ε��� 1 or Later
		step	- [in]  ���� Ÿ�� (������� Ȥ�� ī�޶� Z ��) �̵� ���� (����: mm)
		skip	- [in]  ���� ������ ���� LED ����ȭ�ϴµ� �ʿ��� Ƚ�� (���� �м����� �ʰ� ����)
		repeat	- [in]  �� Step �� �ݺ� ���� Ƚ�� (���� �� ���� �� ��� ���� ���ϱ� ����)
		total	- [in]  �� Step �̵� Ƚ�� ��, step ũ�� ��ŭ ��/�� �̵� Ƚ�� (�ϴ�, Min/Max ���� ����� �ʵ��� ����)
 retn : None
*/
VOID CMainThread::StartFocus(UINT8 cam_id,
							 DOUBLE step,
							 UINT32 skip,
							 UINT32 repeat,
							 UINT32 total)
{
	/* ���� ���� */
	if (m_syncFocus.Enter())
	{
		m_u8ACamID		= cam_id;
		m_dbInitPosZ	= uvCmn_MCQ_GetACamCurrentPosZ(cam_id);	/* ���� ���� Z �� ��ġ �� ���� */
		m_dbStepSize	= step;		/* UP : ��� ��, DOWN : ���� �� */
		m_u32SkipCount	= skip;
		m_u32RepCount	= repeat;
		m_u32TotCount	= total;

		/* ���� ��� �ʱ�ȭ */
		m_bRunFocus		= TRUE;
		m_u8WorkStep	= 0x01;
		m_u8WorkTotal	= 0x07;
		m_u32MeasRep	= 0;
		m_u32MeasTot	= 0;
		m_u32MeasSkip	= 0;
		m_u64DelayTime	= GetTickCount64();

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
	/* Align Camera Z : Homing */
	case 0x01 : m_enWorkState = ACamZHoming();			break;
	/* Align Camera Z : IsHomed */
	case 0x02 : m_enWorkState = IsACamZHomed();			break;
	/* ���� ��ġ�� �̵����� ���� */
	case 0x03 : m_enWorkState = SetMoveStep();			break;
	/* ���� ��ġ�� �̵��ߴ��� Ȯ�� */
	case 0x04 : m_enWorkState = IsMovedStep();			break;
	/* Trigger 1�� �߻� */
	case 0x05 : m_enWorkState = PutOneTrigger();		break;
	/* Grabbed Image ���� �� Edge Detect ���� */
	case 0x06 : m_enWorkState = ProcDetectImage();		break;
	/* ��� ������ ���� �� �м� ���� */
	case 0x07 : m_enWorkState = ResultDetectImage();	break;
	}

	/* ���� �۾� ���� ���� �Ǵ� */
	SetWorkNext();
	/* ��ð� ���� ���� ��ġ�� �ݺ� �����Ѵٸ� ���� ó�� */
	IsWorkTimeOut();
}

/*
 desc : ���� �ܰ�� �̵��ϱ� ���� ó��
 parm : None
 retn : None
*/
VOID CMainThread::SetWorkNext()
{
	/* �۾��� �����̰ų� ����Ǿ��� �� */
	if (ENG_JWNS::en_error == m_enWorkState || ENG_JWNS::en_comp == m_enWorkState)
	{
		m_u8WorkStep	= 0x00;
		m_bRunFocus		= FALSE;
		return;
	}

	if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* ��� �۾��� ���� ������� */
		if (m_u8WorkTotal != m_u8WorkStep)
		{
			/* ���� Skip ó�� �ܰ����� ���� */
			if (0x06 == m_u8WorkStep && 0 == m_u32MeasRep)
			{
				m_u8WorkStep	= 0x05;	/* ���� ���� ��ġ�� �̵� */
			}
			else	m_u8WorkStep++;	/* ���� �۾� �ܰ�� �̵� */
		}
		/* ��� �۾��� �Ϸ�� ��� */
		else
		{
			/* �ٽ� ���� ��ġ���� �ݺ� �����϶�� Step �� ���� */
			if (m_u32MeasRep != m_u32RepCount)
			{
				m_u8WorkStep	= 0x05;
			}
			/* ���� ��ġ���� �ݺ� ���� Ƚ���� ��� ������ �Ǿ����� */
			else
			{
				m_u32MeasRep	= 0x00;	/* ���� ��ġ���� �ݺ� ���� Ƚ�� �ʱ�ȭ */
				m_u32MeasSkip	= 0x00;	/* ���� ��ġ���� �ݺ� ���� Ƚ�� �ʱ�ȭ */
				m_u32MeasTot++;			/* ��ü ���� Ƚ�� ���� ó�� */
				/* ��ü ���� �۾� Ƚ����ŭ ���� �ߴ��� */
				if (IsCompleted())
				{
					m_enWorkState	= ENG_JWNS::en_comp;
					m_bRunFocus		= FALSE;
				}
				else
				{
					/* �ٽ� ó������ �̵��ϱ� ���� */
					m_u8WorkStep= 0x01;
				}
			}
		}
		/* ���� �ֱٿ� Waiting �� �ð� ���� */
		m_u64DelayTime	= GetTickCount64();
	}
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
	if (!m_bRunFocus || (m_u32TotCount == m_u32MeasTot))	return TRUE;

	return FALSE;
}

/*
 desc : ����� ī�޶� Z �� Homing ��û
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::ACamZHoming()
{
	if (!uvEng_MCQ_SetACamHomingZAxis(m_u8ACamID))	return ENG_JWNS::en_error;
	return ENG_JWNS::en_next;
}

/*
 desc : ����� ī�޶� Z �� Homing �Ϸ� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsACamZHomed()
{
	/* Homing �ϴµ� �ּ� �ʿ��� ���� �ð� ��� �� */
#ifdef _DEBUG
	if (GetTickCount64() < m_u64DelayTime + 2000)	return ENG_JWNS::en_wait;
#else
	if (GetTickCount64() < m_u64DelayTime + 1000)	return ENG_JWNS::en_wait;
#endif
	if (!uvCmn_MCQ_IsACamZReady(m_u8ACamID))	return ENG_JWNS::en_wait;

	/* ���Ͱ� ����ȭ�� ������ �ణ ��� */
	uvCmn_uSleep(500000);	/* �� 0.5 �� */

	return ENG_JWNS::en_next;
}

/*
 desc : ���� ��ġ�� ���� �̵�
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetMoveStep()
{
	/* ���� �̵��ؾ� �� ��ġ ��� (���� ���� ��ġ + ���� �̵��� Ƚ�� * �̵� ũ��) */
	m_dbMeasPosZ	= m_dbInitPosZ + (m_u32MeasTot * m_dbStepSize);
	/* Align Camera Z �� �̵� */
	if (!uvEng_MCQ_SetACamMovePosZ(m_u8ACamID, 0x00, m_dbMeasPosZ))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : �̵��� �Ϸ� �ߴ��� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsMovedStep()
{
	DOUBLE dbPosZ	= 0.0f, dbValidMin, dbValidMax;

	/* ���� Z Axis ��ġ �� ��� */
	dbPosZ	= uvCmn_MCQ_GetACamCurrentPosZ(m_u8ACamID);

	/* Z Axis ��ġ�� �־�� �� ��ȿ ���� �� ���� */
	dbValidMax	= dbPosZ + 0.001 /* 1 um */;
	dbValidMin	= dbPosZ - 0.001 /* 1 um */;

	/* �����ϰ��� �ϴ� ��ġ�� �������� ���� (+/- 10 um ���� ������ ����) */
	if (m_dbMeasPosZ > dbValidMin && m_dbMeasPosZ < dbValidMax)
	{
		return ENG_JWNS::en_next;	/* +/- 1.0 um �̳��� ���� Z �� ��ġ�� �̵��� ������ */
	}

	/* ���Ͱ� ����ȭ�� ������ �ణ ��� */
	uvCmn_uSleep(500000);	/* �� 0.5 �� */

	return ENG_JWNS::en_wait;	/* ���� �̵��� �Ϸ���� �ʾ��� */
}

/*
 desc : Ʈ���� 1�� �߻� ��Ŵ
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::PutOneTrigger()
{
	UINT8 u8ChNo	= m_u8ACamID;	/* MAX 4 */

	/* ���� Ʈ���� ��� ���ۿ� ���� ���� ���ϰ� ��� ������ ���� */
	/* Trigger �۽� ���ۿ� �����ϰ� ���� ��ɾ ����Ǵ� �� ���� */
	if (!uvEng_Trig_IsSendCmdCountUnder(3))	return ENG_JWNS::en_wait;

	/* Align Camera is Edge Detection Mode */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_edge_mode);

	/* ���� Live & Edge & Calibration ������ �ʱ�ȭ */
	uvEng_Camera_ResetGrabbedImage();

	/* Trigger ������ (��������) �߻� */
	if (!uvEng_Trig_ReqTrigOutOne(u8ChNo, TRUE))
	{
		AfxMessageBox(L"Failed to send the event for trigger", MB_ICONSTOP|MB_TOPMOST);
		return ENG_JWNS::en_error;
	}

	/* �̹��� Grabbed Ȯ�� �ð� ���� */
	m_u64GrabTime	= GetTickCount64();

	return ENG_JWNS::en_next;
}

/*
 desc : ���� Grabbed Image ��, Edge Detection�� �̹����� �ִ��� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::ProcDetectImage()
{
	/* Grabbed Image�� ������, Edge ����� �ִ��� ���� */
	if (!uvEng_Camera_RunEdgeDetect(m_u8ACamID))
	{
		/* 3 �� ���� Grabbed Image�� ���� ���, �ٽ� Ʈ���� �߻� ��Ű�� ���� �ڷ� �̵� */
		if (!(m_u64GrabTime+3000 < GetTickCount64()))	return ENG_JWNS::en_wait;
		/* �ٽ� �̹��� Grbbaed �۾� �����϶�� ���� */
		m_u8WorkStep	-= 2;
		return ENG_JWNS::en_next;
	}
	/* Grabbed Image�� ������, Edge ��� �� �˻� �������� ���� */
	if (!uvEng_Camera_GetEdgeDetectResults(m_u8ACamID)[0].set_data)
	{
		/* �ٽ� �̹��� Grbbaed �۾� �����϶�� ���� */
		if (m_u8RetryTrig < 0x03)
		{
			m_u8WorkStep	-= 2;
			return ENG_JWNS::en_next;
		}
		AfxMessageBox(L"Failed to detect the grabbed image", MB_ICONSTOP|MB_TOPMOST);
		return ENG_JWNS::en_error;
	}

	/* Ʈ���� �߻� Ƚ�� �ʱ�ȭ */
	m_u8RetryTrig	= 0x00;
	/* Edge Detect ������ ���������� ������, Skip Count ���� ó�� */
	if (m_u32MeasSkip < m_u32SkipCount)	m_u32MeasSkip++;
	else								m_u32MeasRep++;

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
	if (!m_pMeasure->SetResult(m_u8ACamID))	return ENG_JWNS::en_error;
	/* ���� �θ𿡰� �˸� */
	SendMesgResult();

	TRACE(L"m_u32MeasRep = %d  [%d]\n", m_u32MeasRep, m_pMeasure->GetCount());

	return ENG_JWNS::en_next;
}

/*
 desc : ���� ó�� ��� ��ϵ� ���� ��ȯ
 parm : None
 retn : ���� ��ȯ
*/
UINT32 CMainThread::GetGrabCount()
{
	return m_pMeasure->GetCount();
}

/*
 desc : ������� ������ ������ ��ȯ
 parm : index	- [in]  ���������� �ϴ� �޸� ��ġ (Zero-based)
 retn : index ��ġ�� ����� ���� ��
*/
LPG_ZAAL CMainThread::GetGrabData(UINT32 index)
{
	LPG_ZAAL pstData	= NULL;

	/* ���� ���� */
	if (m_syncFocus.Enter())
	{
		pstData	= m_pMeasure->GetResult(index);

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
 desc : ���� ����� Edge Image�� ���� �� ������ ��ȯ
 parm : None
 retn : ���� �� ����ü ������ ��ȯ
*/
LPG_ZAAL CMainThread::GetGrabSel()
{
	return m_pMeasure->GetDataSel();
}

/*
 desc : ������ Min / Max �� ��ȯ
 parm : mode	- [in]  0x00: Min, 0x01: Max
		type	- [in]  0x00 : strength, 0x01 : length, 0x02 : Feret, 0x03 : Diameter
 retn : �� ��ȯ
*/
DOUBLE CMainThread::GetMinMax(UINT8 mode, UINT8 type)
{
	return m_pMeasure->GetMinMax(mode, type);
}

/*
 desc : ������� �۾� ������ ��ȯ
 parm : None
 retn : �۾� ������ ��ȯ (����: percent)
*/
DOUBLE CMainThread::GetWorkStepRate()
{
	DOUBLE dbTotal	= m_u32SkipCount * m_u32RepCount * m_u32TotCount * 1.0f;
	DOUBLE dbCurrent= (m_u32SkipCount * m_u32RepCount * m_u32MeasTot) * 1.0f +
					  (m_u32MeasRep + m_u32MeasSkip) * 1.0f;

	return dbCurrent / dbTotal * 100.0f;
}