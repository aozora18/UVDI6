
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
CMainThread::CMainThread(HWND parent)
	: CThinThread()
{
	m_hParent			= parent;
	m_u8WorkStep		= 0x00;
	m_bRunPrint			= FALSE;
	m_enWorkState		= ENG_JWNS::en_none;
	m_u64PeridPktTime	= GetTickCount64();
	m_bReqPeriodMC2		= FALSE;
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
	/* ���� ���� */
	if (m_syncPrint.Enter())
	{
		/* ��Ŀ�� �۾� ���� */
		if (m_bRunPrint)	RunPrint();
		else				ReqPeriodPkt();

		/* ���� ���� */
		m_syncPrint.Leave();
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
 desc : �θ𿡰� �޽��� ���� (SendMessage)
 parm : timeout	- [in]  SendMessag Lifetime (unit: msec)
		flag	- [in]  FALSE : �뱤 1ȸ �Ϸ�, TRUE : �뱤 ��ü �Ϸ�
 retn : None
*/
VOID CMainThread::SendMesgPrint(BOOL flag, UINT32 timeout)
{
	WPARAM wParam		= flag ? MSG_ID_PRINT_COMPLETE : MSG_ID_PRINT_ONE;
	DWORD_PTR dwResult	= 0;
	LRESULT lResult		= 0;

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
 desc : �뱤 ����
 parm : expo_x	- [in]  �뱤 ���� ��ġ (����: um)
		expo_y	- [in]  �뱤 ���� ��ġ (����: um)
		period_z- [in]  ���а� Z �� �̵� ���� (����: um)
		period_y- [in]  �������� Y �� �̵� ���� (����: 0.1 um or 100 nm)
		repeat	- [in]  �� �ݺ� �뱤 Ƚ��
 retn : None
*/
VOID CMainThread::StartPrint(DOUBLE expo_x, DOUBLE expo_y,
							 DOUBLE period_z, DOUBLE period_y, UINT16 repeat)
{
	UINT8 i	= 0x00;

	/* ���� ���� */
	if (m_syncPrint.Enter())
	{
		m_u16PrintTotal	= repeat;
		m_u16PrintCount	= 0;
		m_dbPeriodZ		= period_z;
		m_dbPeriodY		= period_y;
		m_u8WorkStep	= 0x01;
		m_u8WorkTotal	= 0x0e;
		/* �뱤 ���� ��ġ ��� */
		m_dbExpoX		= expo_x/*uvEng_ShMem_GetLuria()->machine.table_expo_start_xy[0].x*/;
		m_dbExpoY		= expo_y/*uvEng_ShMem_GetLuria()->machine.table_expo_start_xy[0].y*/;
		/* ���� ��� Photohead�� Z Axis ��ġ (����) �� ���� (����: um) */
		for (i=0x00; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
		{
			m_dbPhBaseZ[i]	= uvEng_ShMem_GetLuria()->directph.focus_motor_move_abs_position[i] / 1000.0f;
		}
		/* ���� �۾� ������ ���� �ð� ���� */
		m_u64DelayTime		= GetTickCount64();
		/* ���� ���� */
		m_bRunPrint			= TRUE;

		/* ���� ���� */
		m_syncPrint.Leave();
	}
}

/*
 desc : �۾� ������ ����
 parm : None
 retn : None
*/
VOID CMainThread::StopPrint()
{
	/* ���� ���� */
	if (m_syncPrint.Enter())
	{
		m_bRunPrint	= FALSE;
		/* ���� ���� */
		m_syncPrint.Leave();
	}
}

/*
 desc : Specification ���� �۾� ����
 parm : None
 retn : None
*/
VOID CMainThread::RunPrint()
{
	/* �۾� �ܰ� ���� ���� ó�� */
	switch (m_u8WorkStep)
	{
	/* Luria ���񽺿� �뱤 ���� ��ġ ���� */
	case 0x01	:	m_enWorkState	= SetExpoStartXY();		break;
	/* �뱤 ���� ��ġ�� ���� ���� ����Ǵµ�, �ּ� ��� �ð� */
	case 0x02	:	m_enWorkState	= IsWorkWaitTime();		break;
	/* Luria ���񽺿� �뱤 ���� ��ġ ���� �ߴ��� Ȯ�� */
	case 0x03	:	m_enWorkState	= IsExpoStartXY();		break;
	/* ��� Photohead Z Axis Homing (Reset) (Move to Mid-Position) */
	case 0x04	:	m_enWorkState	= SetPhZAxisHoming();	break;
	/* ��� Photohead Z Axis�� ���� Homing ���� ������ Ȯ�� */
	case 0x05	:	m_enWorkState	= IsPhZAxisHoming();	break;
	/* ��� Photohead Z Axis Homing (Reset) �Ϸ� �ߴ��� Ȯ�� */
	case 0x06	:	m_enWorkState	= IsPhZAxisHomed();		break;
	/* ��� Photohead Z Axis �뱤 ��ġ�� �̵� */
	case 0x07	:	m_enWorkState	= SetPhZAxisMoving();	break;
	/* ��� Photohead Z Axis�� �̵��ϴµ� �ּ� ��� �ð���ŭ ��ٸ� */
	case 0x08	:	m_enWorkState	= IsWorkWaitTime();		break;
	/* ��� Photohead Z Axis�� �뱤 ��ġ�� �̵� �ߴ��� Ȯ�� */
	case 0x09	:	m_enWorkState	= IsPhZAxisMoved();		break;
	/* �뱤 �غ� */
	case 0x0a	:	m_enWorkState	= SetExpoReady();		break;
	/* �뱤 �غ� �Ǿ����� Ȯ�� */
	case 0x0b	:	m_enWorkState	= IsExpoReady();		break;
	/* �뱤 ���� */
	case 0x0c	:	m_enWorkState	= SetExpoPrinting();	break;
	/* �뱤 ������ ����Ǳ� ����, ��� ��� */
	case 0x0d	:	m_enWorkState	= IsWorkWaitTime();		break;
	/* �뱤 �Ϸ� �ߴ��� ���� Ȯ�� */
	case 0x0e	:	m_enWorkState	= IsExpoPrinted();		break;
	}

	/* ���� �۾� ���� ���� �Ǵ� */
	SetWorkNext();

	/* ��ð� ���� ���� ��ġ�� �ݺ� �����Ѵٸ� ���� ó�� */
	if (IsWorkTimeOut())
	{
		if (m_syncPrint.Enter())
		{
			m_bRunPrint		= FALSE;
			m_enWorkState	= ENG_JWNS::en_error;

			m_syncPrint.Leave();
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
	if (ENG_JWNS::en_error == m_enWorkState || ENG_JWNS::en_comp == m_enWorkState)
	{
		m_u8WorkStep	= 0x00;
		m_bRunPrint		= FALSE;
		return;
	}

	if (ENG_JWNS::en_next == m_enWorkState)
	{
		if (m_u8WorkTotal == m_u8WorkStep)
		{
			if (++m_u16PrintCount == m_u16PrintTotal)
			{
				m_u8WorkStep	= 0x00;
				m_enWorkState	= ENG_JWNS::en_comp;
				m_bRunPrint		= FALSE;
				SendMesgPrint(TRUE);
			}
			else
			{
				m_u8WorkStep	= 0x01;
				SendMesgPrint(FALSE);
			}
#if 0	/* ReqPeriodPkt() �Լ����� �ֱ������� ȣ�� */
			/* ���� ���а� ���� �� ��û */
			uvEng_Luria_ReqGetMotorAbsPositionAll();
#endif
		}
		else
		{
			/* ���� �۾� �ܰ�� �̵� */
			m_u8WorkStep++;
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
	return	GetTickCount64() > (m_u64DelayTime + 60000);	/* 30 �� �̻� �����Ǹ� */
#else
	return	GetTickCount64() > (m_u64DelayTime + 10000);	/* 10 �� �̻� �����Ǹ� */
#endif
}

/*
 desc : �۾��� �Ϸ� �Ǿ����� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::IsCompleted()
{
	return m_enWorkState == ENG_JWNS::en_comp;
}

/*
 desc : ���� Photohead Z Axis�� ���ϴ� ��ġ�� �����ߴ��� ����
 parm : ph_no	- [in]  Photohead Number (1 ~ 8)
		pos		- [in]  ���� �̵��� ��ġ �� (����: mm)
 retn : TRUE or FALSE
*/
BOOL CMainThread::IsPhZAxisMoved(UINT8 ph_no, DOUBLE pos)
{
	UINT8 u8Drv		= UINT8(ENG_MMDI::en_axis_ph1);
	DOUBLE dbZAxis	= 0.0f;

	/* ���� �̵��� ��ġ�� ���� �̵��� ��ġ�� ������ +/- 0.001 mm ��, 3 um �̸��̸� ���� */
	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		dbZAxis	= uvCmn_Luria_GetPhZAxisPosition(ph_no);
	}
	else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		dbZAxis	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(u8Drv+ph_no-1));
	}

	return (abs(pos - dbZAxis) < 0.003 /* less than 3 um */);
}

/*
 desc : ���� �۾� ����� ��ȯ
 parm : None
 retn : ����� (����: %; Percent)
*/
UINT16 CMainThread::GetStepCount()
{
	return  m_u8WorkStep + (m_u8WorkTotal * m_u16PrintCount);
}

/*
 desc : �뱤 ���� ��ġ ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetExpoStartXY()
{
	BOOL bSucc	= FALSE;
	DOUBLE dbStartX, dbStartY;
#if 0
	TRACE(L"Repeat Count = %d\n", m_u16PrintCount);
#endif
	/* ����: um (Luria Service�� ����� ��� �ǹǷ�) */
	dbStartX	= m_dbExpoX;									/* X ���� ������ ���� ���� �� */
	dbStartY	= m_dbExpoY + (m_dbPeriodY * m_u16PrintCount);	/* Y ���� �뱤 Ƚ���� ���� ��/�� �ϴ� ���� �� */
	/* �뱤 ���� ��ġ�� Luria (Photohead) Service�� ���� */
	bSucc		= uvEng_Luria_ReqSetTableExposureStartPos(0x01 /* fixed */,
														  (UINT32)ROUNDED(dbStartX * 1000.0f, 0),
														  (UINT32)ROUNDED(dbStartY * 1000.0f, 0), TRUE);
	/* ���� ���信 ���� �ּ� ��� �ð� */
	SetWorkWaitTime(1000);

	return bSucc ? ENG_JWNS::en_next : ENG_JWNS::en_error;
}

/*
 desc : �뱤 ���� ��ġ ���� �ߴ��� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsExpoStartXY()
{
	DOUBLE dbStartX, dbStartY;
	LPG_I4XY pstStart	= &uvEng_ShMem_GetLuria()->machine.table_expo_start_xy[0];

	/* ����: um (Luria Service�� ����� ��� �ǹǷ�) */
	dbStartX	= m_dbExpoX;									/* X ���� ������ ���� ���� �� */
	dbStartY	= m_dbExpoY + (m_dbPeriodY * m_u16PrintCount);	/* Y ���� �뱤 Ƚ���� ���� ��/�� �ϴ� ���� �� */

	/* Luria�� �뱤 ���� ��ġ ���� ���� �Ǿ����� ���� Ȯ�� */
	if (!(pstStart->x == (INT32)ROUNDED(dbStartX * 1000.0f, 0) &&
		  pstStart->y == (INT32)ROUNDED(dbStartY * 1000.0f, 0)))	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : ��� Photohead Z Axis Homing (Reset) (Move to Mid-Position)
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetPhZAxisHoming()
{
	BOOL bSucc	= FALSE;
	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* ��� ���а� Mid-pos ���� �ʱ�ȭ �ȵǾ��ٰ� �� �缳�� */
		uvCmn_Luria_ResetAllPhZAxisMidPosition();
		/* ��� ���а� Mid-Pos ��, Homing �۾� �����϶�� ���� */
		bSucc	= uvEng_Luria_ReqSetMotorPositionInitAll(0x01);
	}
	else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* ���� Motion�� Toggle �� ���� */
		uvCmn_MC2_GetDrvDoneToggledAll();
		/* Photohead 2�븸 Homing ���� */
		uvEng_MC2_SendDevHoming(ENG_MMDI::en_axis_ph1);
		uvEng_MC2_SendDevHoming(ENG_MMDI::en_axis_ph2);
	}
	/* ���� �ֱ� ��� ���� �ð� �ʱ�ȭ */
	m_u64SendTime	= 0;
	m_u64StepTime	= GetTickCount64();	/* �ش� ����� ������ �ð� ���� */

	return bSucc ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : ��� Photohead Z Axis �� 1�� �̻��� Homing �������� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsPhZAxisHoming()
{
	/* !!! �߿� !!! �۾� ��� �ð� �� ���� */
	m_u64DelayTime	= GetTickCount64();

	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* Photohead Z Axis�� ���� ���� ��û */
		if (m_u64SendTime+500 < GetTickCount64())
		{
#if 0
			TRACE(L"Homing Check........\n");
#endif
			m_u64SendTime = GetTickCount64();
			if (!uvEng_Luria_ReqGetMotorStateAll())	return ENG_JWNS::en_error;
		}
		/* �� 10�� ���� Homing �۾��� �������� ������, �ٽ� Homing ��� ���� */
		if (m_u64StepTime+10000 < m_u64DelayTime)
		{
			m_u8WorkStep	-= 2;
			return ENG_JWNS::en_next;
		}
		/* ���� Photohead�� Z Axis�� Homing �������� ���� Ȯ�� */
		if (!uvCmn_Luria_IsAnyPhZAxisHoming())	return ENG_JWNS::en_wait;
	}
	else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* �� 10�� ���� Homing �۾��� �������� ������, �ٽ� Homing ��� ���� */
		if (m_u64StepTime+10000 < m_u64DelayTime)
		{
			m_u8WorkStep	-= 2;
			return ENG_JWNS::en_next;
		}
		/* Photohead 2�� Homing �۾� �Ϸ� �ߴ��� ���� */
		if (!(uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_axis_ph1) &&
			  uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_axis_ph2)))	return ENG_JWNS::en_wait;
	}

	m_u64StepTime	= GetTickCount64();	/* �ش� ����� ������ �ð� ���� */

	return ENG_JWNS::en_next;	/* Homing ������ ��� */
}

/*
 desc : ��� Photohead Z Axis Homing (Reset) �Ǿ����� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsPhZAxisHomed()
{
	/* !!! �߿� !!! �۾� ��� �ð� �� ���� */
	m_u64DelayTime	= GetTickCount64();

	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* Photohead Z Axis�� ���� ���� ��û */
		if (m_u64SendTime+500 < GetTickCount64())
		{
			m_u64SendTime = GetTickCount64();
			if (!uvEng_Luria_ReqGetMotorStateAll())	return ENG_JWNS::en_error;
			return ENG_JWNS::en_wait;
		}
		/* �� 15�� ���� Homing �۾��� �������� ������, �ٽ� Homing ��� ���� */
		if (m_u64StepTime+15000 < m_u64DelayTime)
		{
			m_u8WorkStep	-= 3;
			return ENG_JWNS::en_next;
		}
		/* ���� ��� Photohead�� Mid ���°� �Ϸ� �Ǿ����� Ȯ�� */
		if (!uvCmn_Luria_IsAllPhZAxisMidPosition())	return ENG_JWNS::en_wait;
		/* ���� ��� Phothead�� Z Axis ���°� Idle �������� Ȯ�� */
		if (!uvCmn_Luria_IsAllPhZAxisIdle())		return ENG_JWNS::en_wait;
	}
	else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* �� 3�� ���� Homing �۾��� �������� ������, �ٽ� Homing ��� ���� */
		if (m_u64StepTime+3000 < m_u64DelayTime)
		{
			m_u8WorkStep	-= 3;
			return ENG_JWNS::en_next;
		}
		/* Photohead 2�� Homing �۾� �Ϸ� �ߴ��� ���� */
		if (!(uvCmn_MC2_IsDriveHomed(ENG_MMDI::en_axis_ph1) &&
			  uvCmn_MC2_IsDriveHomed(ENG_MMDI::en_axis_ph2)))	return ENG_JWNS::en_wait;
	}
	return ENG_JWNS::en_next;
}

/*
 desc : ��� Photohead Z Axis�� ���ϴ� �뱤 ��ġ�� �̵�
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetPhZAxisMoving()
{
	UINT8 i			= 0x00, u8Drv = UINT8(ENG_MMDI::en_axis_ph1);
	BOOL bSucc		= FALSE;
	DOUBLE dbZAxis	= 0, *pVelo = uvEng_GetConfig()->mc2_svc.max_velo;
	
	/* Photohead ���� ��ŭ �뱤 ��ġ�� �̵� ��Ŵ */
	for (; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		dbZAxis	= m_dbPhBaseZ[i] + (m_dbPeriodZ * m_u16PrintCount);
#if 0
		TRACE(L"1st. Set PH Z Axis Pos [%d] = %.3f mm\n", i+1, dbZAxis);
#endif
		/* ���� Photohead�� Z �� ��ġ �̵� */
		if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
		{
			if (!uvEng_Luria_ReqSetMotorAbsPosition(i+1, dbZAxis))	return ENG_JWNS::en_error;
		}
		else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
		{
			uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI(u8Drv+i));
			if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI(u8Drv+i), dbZAxis, pVelo[u8Drv+i]))
				return ENG_JWNS::en_error;
		}
	}

	/* Luria���� ��� Photohead�� �뱤 ��ġ�� �̵��ϴµ� �ּ����� ��� �ð� */
	SetWorkWaitTime(1000);

	/* ���� �ֱ� ��� ���� �ð� �ʱ�ȭ */
	m_u64SendTime	= 0;
	m_u64StepTime	= GetTickCount64();	/* �ش� ����� ������ �ð� ���� */

	return ENG_JWNS::en_next;
}

/*
 desc : ��� Photohead Z Axis Homing (Reset) �Ǿ����� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsPhZAxisMoved()
{
	UINT8 i, u8Moved= 0x00, u8Drv = UINT8(ENG_MMDI::en_axis_ph1);
	BOOL bIsMoved	= FALSE;
	DOUBLE dbZAxis	= 0;

	/* !!! �߿� !!! �۾� ��� �ð� �� ���� */
	m_u64DelayTime	= GetTickCount64();

	/* �� 10�� ���� ���ϴ� ��ġ�� �̵����� �ʾҴٸ�, �ٽ� �̵� ��� ���� */
	if (m_u64StepTime+10000 < m_u64DelayTime)
	{
		m_u8WorkStep	-= 2;
		return ENG_JWNS::en_next;
	}
	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* ��� Photohead�� Z Axis�� ���������� �۾��� �غ� �Ǿ� �ִ��� Ȯ�� */
		if (!uvCmn_Luria_IsAllPhZAxisIdle())	return	ENG_JWNS::en_wait;
	}
	else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* Photohead ���� ��ŭ �뱤 ��ġ�� �̵� ��Ŵ */
		for (i=0x00; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
		{
			if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI(u8Drv+i)))	return ENG_JWNS::en_wait;
		}
	}

	/* ��� Photohead�� Z Axis ��ġ ���� ���ϴ� ��ġ�� ���� �ߴ��� ���� */
	for (i=0x00; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		dbZAxis	= m_dbPhBaseZ[i] + (m_dbPeriodZ * m_u16PrintCount);
#if 0
		TRACE(L"Current PH Z Axis Pos [%d] = [set] %.3f mm  == [cur] %.3f mm\n", i+1,
			  dbZAxis, uvCmn_Luria_GetPhZAxisPosition(i+1));
#endif
		/* ���� �̵��� ��ġ�� ���� �̵��� ��ġ�� ������ +/- 0.001 mm ��, 1 um �̳��̸� ���� */
		if (IsPhZAxisMoved(i+1, dbZAxis))	u8Moved++;
	}

	/* ��� �̵��� �Ϸ� �Ǿ����� ���� �� ��� ���а��� Z Axis�� ���°� Idle �������� Ȯ�� */
	if (u8Moved == uvEng_GetConfig()->luria_svc.ph_count)	return ENG_JWNS::en_next;

	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* ���� ��� Photohead�� Z Axis ��ġ �� ��û */
		if (m_u64SendTime+500 < GetTickCount64())
		{
			/* Photohead ���� ��ŭ �뱤 ��ġ�� �̵� ��Ŵ */
			for (i=0x00; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
			{
				dbZAxis	= m_dbPhBaseZ[i] + (m_dbPeriodZ * m_u16PrintCount);
				TRACE(L"2nd. Set PH Z Axis Pos [%d] = %.3f mm\n", i+1, dbZAxis);
				/* ���� ��ġ�� ���� �̵��� ��ġ�� +/- ���� ������ ������, �� �̻� �̵����� ����� �� */
				if (!IsPhZAxisMoved(i+1, dbZAxis))
				{
					/* ���� Photohead�� Z �� ��ġ �̵� */
					if (!uvEng_Luria_ReqSetMotorAbsPosition(i+1, dbZAxis))	return ENG_JWNS::en_error;
				}
			}

			m_u64SendTime = GetTickCount64();
			if (!uvEng_Luria_ReqGetMotorAbsPositionAll())	return ENG_JWNS::en_error;
		}
	}
	return ENG_JWNS::en_wait;
}

/*
 desc : Luria Service���� �뱤�� �غ� �Ǿ� �ִٰ� �˸�
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetExpoReady()
{
	/* ���� �ֱ� ��� ���� �ð� �ʱ�ȭ */
	m_u64SendTime	= 0;
	m_u64StepTime	= GetTickCount64();	/* �ش� ����� ������ �ð� ���� */

	if (!uvEng_Luria_ReqSetPrintOpt(0x00))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : ��� Photohead Z Axis Homing (Reset) �Ǿ����� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsExpoReady()
{
	/* �� 5�� ���� ���ϴ� ��ġ�� �̵����� �ʾҴٸ�, �ٽ� �̵� ��� ���� */
	if (m_u64StepTime+5000 < GetTickCount64())
	{
		m_u8WorkStep	-= 2;
		return ENG_JWNS::en_next;
	}

	/* ���� �뱤�� �غ�� �������� ���� Ȯ�� */
	if (uvCmn_Luria_IsExposeStateSuccess(ENG_LCEP::en_pre_print))	return ENG_JWNS::en_next;

	/* ���� Luria Service�� �뱤 ���� (����) ��û */
	if (m_u64SendTime+500 < GetTickCount64())
	{
		m_u64SendTime = GetTickCount64();
		if (!uvEng_Luria_ReqGetExposureState())	return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_wait;
}

/*
 desc : Luria Service���� �뱤 �����϶�� �˸�
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetExpoPrinting()
{
	/* ���� �ֱ� ��� ���� �ð� �ʱ�ȭ */
	m_u64SendTime	= 0;

	if (!uvEng_Luria_ReqSetPrintOpt(0x01))	return ENG_JWNS::en_error;

	/* �뱤 �����ϱ� ���� �غ��ϴ� ��� �ð� �� ���� */
	SetWorkWaitTime(1000);

	return ENG_JWNS::en_next;
}

/*
 desc : ��� Photohead Z Axis Homing (Reset) �Ǿ����� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsExpoPrinted()
{
	/* ���� �뱤�� �Ϸ�� �������� ���� Ȯ�� */
	if (uvCmn_Luria_IsExposeStateSuccess(ENG_LCEP::en_print))	return ENG_JWNS::en_next;

	/* ���� �뱤�� ���� ���� ��� */
	if (uvCmn_Luria_IsExposeStateRunning(ENG_LCEP::en_print))
	{
		/* !!! �߿� !!! �۾� ��� �ð� �� ���� */
		m_u64DelayTime	= GetTickCount64();
	}

	/* ���� Luria Service�� �뱤 ���� (����) ��û */
	if (m_u64SendTime+500 < GetTickCount64())
	{
		m_u64SendTime = GetTickCount64();
		if (!uvEng_Luria_ReqGetExposureState())	return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_wait;
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