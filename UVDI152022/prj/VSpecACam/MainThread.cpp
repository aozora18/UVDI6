
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
	m_u64TickGrabbed	= 0;
	m_u8WorkStep		= 0x00;
	m_bRunSpec			= FALSE;
	m_enWorkState		= ENG_JWNS::en_none;
	m_u64PeridPktTime	= 0;
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
	if (m_syncSpec.Enter())
	{
		/* ��Ŀ�� �۾� ���� */
		if (m_bRunSpec)	RunSpec();
		else			ReqPeriodPkt();

		/* ���� ���� */
		m_syncSpec.Leave();
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
 parm : flag	- [in]  TRUE : ��� �۾� �Ϸ�, FALSE : 1 ȸ ���� �۾� �Ϸ�
		timeout	- [in]  SendMessag Lifetime (unit: msec)
 retn : None
*/
VOID CMainThread::SendMesgResult(BOOL flag, UINT32 timeout)
{
	WPARAM wParam		= flag ? MSG_ID_CALI_COMPLETE : MSG_ID_CALI_RESULT;
	DWORD_PTR dwResult	= 0;
	LRESULT lResult		= 0;

	/* �θ𿡰� �̺�Ʈ �޽��� ���� */
	lResult	= ::SendMessageTimeout(m_hParent, WM_MAIN_THREAD, wParam, 0L,
								   SMTO_NORMAL, timeout, &dwResult);
	if (0 == lResult)
	{
#if 0
		/* ���� �߻��� ���� �� ���� */
		TRACE("MainThread : SendMessage Time out <MesgResult> = %d \n", GetLastError());
#endif
	}
}

/*
 desc : �θ𿡰� �޽��� ���� (SendMessage) - Step Result ��� ��
 parm : flag	- [in]  TRUE : ��� �۾� �Ϸ�, FALSE : 1 ȸ ���� �۾� �Ϸ�
		timeout	- [in]  SendMessag Lifetime (unit: msec)
 retn : None
*/
VOID CMainThread::SendMesgGrabView(UINT32 timeout)
{
	DWORD_PTR dwResult	= 0;
	LRESULT lResult		= 0;

	/* �θ𿡰� �̺�Ʈ �޽��� ���� */
	lResult	= ::SendMessageTimeout(m_hParent, WM_MAIN_THREAD, MSG_ID_GRAB_VIEW, 0L,
								   SMTO_NORMAL, timeout, &dwResult);
	if (0 == lResult)
	{
#if 0
		/* ���� �߻��� ���� �� ���� */
		TRACE("MainThread : SendMessage Time out <GrabView> = %d \n", GetLastError());
#endif
	}
}


/*
 desc : Camera Specification ���� ���� �Ű� ���� ���� �� ���� ����
 parm : run_mode- [in]  ���� ��� �� (0x00 : ī�޶� ȸ�� ���� �� ����, 0x01 : ī�޶� �ȼ� ũ�� �� ���ϱ�)
		cam_id	- [in]  ī�޶� �ε��� 1 or Later
		repeat	- [in]  �� �ݺ� ���� Ƚ��
 retn : None
*/
VOID CMainThread::StartSpec(UINT8 run_mode, UINT8 cam_id, UINT32 repeat)
{
	/* ���� ���� */
	if (m_syncSpec.Enter())
	{
		m_u8RunMode		= run_mode == 0x00 ? 0xfe /*���� ���� ����*/: 0xff /*���� �������� ����*/;
		m_u8ACamID		= cam_id;
		m_u32RepeatTotal= repeat;
		m_u32RepeatCount= 0;
		m_dbRateLast	= 0.0f;
		m_u8WorkTotal	= 0x10;
		m_u8WorkStep	= 0x01;
		/* ������ Quartz�� Vision Camera�� �߽� ��ġ�� ������ ó�� */
		m_u8GrabbedCount= 0x00;	/* ���� ������ ���� �̹��� (Quartz Mark) ���� Ƚ�� ���� */
		m_u64DelayTime	= GetTickCount64();
		/* ���� ���� */
		m_bRunSpec		= TRUE;

		/* ���� ���� */
		m_syncSpec.Leave();
	}
}

/*
 desc : �۾� ������ ����
 parm : None
 retn : None
*/
VOID CMainThread::StopSpec()
{
	/* ���� ���� */
	if (m_syncSpec.Enter())
	{
		m_bRunSpec	= FALSE;
		/* ���� ���� */
		m_syncSpec.Leave();
	}
}

/*
 desc : Specification ���� �۾� ����
 parm : None
 retn : None
*/
VOID CMainThread::RunSpec()
{
	/* �۾� �ܰ� ���� ���� ó�� */
	switch (m_u8WorkStep)
	{
	/* ������ Align Camera�� �������� �̵��ϱ� */
	case 0x01	:	m_enWorkState	= SetACamMovingSide();			break;
	case 0x02	:	m_enWorkState	= IsACamMovedSide();			break;
	/* Quartz�� �°� Z Axis Focusing */
	case 0x03	:	m_enWorkState	= SetACamMovingZAxisQuartz();	break;
	case 0x04	:	m_enWorkState	= IsACamMovedZAxisQuartz();		break;
	/* Quartz�� �ִ� ������ Align Camera �̵��ϱ� */
	case 0x05	:	m_enWorkState	= SetACamMovingQuartz();		break;
	case 0x06	:	m_enWorkState	= IsACamMovedQuartz();			break;
	/* Trigger Enabled */
	case 0x07	:	m_enWorkState	= SetTrigEnabled();				break;
	case 0x08	:	m_enWorkState	= IsTrigEnabled();				break;
	/* Align Camera�� ���� �̹��� Grabbed & ���� �� ���� */
	case 0x09	:	m_enWorkState	= GetGrabQuartzData();			break;
	case 0x0a	:	m_enWorkState	= IsGrabbQuartzData();			break;
	/* Quartz�� Align Camera�� �߽����� �̵��ϵ��� ���� */
	case 0x0b	:	m_enWorkState	= SetMotionMoving();			break;
	case 0x0c	:	m_enWorkState	= IsMotionMoved();				break;
	/* Quartz�� Mark�� Vision Camera�� ����(or ������)�� ������ Stage X �̵� ó�� */
	case 0x0d	:	m_enWorkState	= SetMotionMovingX();			break;
	case 0x0e	:	m_enWorkState	= IsMotionMovedX();				break;
	/* Quartz�� Mark Image�� Grabbed */
	case 0x0f	:	m_enWorkState	= GetGrabQuartzData();			break;
	case 0x10	:	m_enWorkState	= IsGrabbQuartzMoveX();			break;
	}

	/* ���� �۾� ���� ���� �Ǵ� */
	SetWorkNext();

	/* ��ð� ���� ���� ��ġ�� �ݺ� �����Ѵٸ� ���� ó�� */
	if (IsWorkTimeOut())
	{
		if (m_syncSpec.Enter())
		{
			m_bRunSpec		= FALSE;
			m_enWorkState	= ENG_JWNS::en_error;

			m_syncSpec.Leave();
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
		m_bRunSpec		= FALSE;
		return;
	}

	if (ENG_JWNS::en_next == m_enWorkState)
	{
#if 1
		if (m_u8WorkStep > 0x0a/* && m_u8WorkStep < 0x10*/)	SendMesgGrabView();
#endif
		if (m_u8WorkTotal == m_u8WorkStep)
		{
#if 0	/* �ٸ� �Լ����� ���� (ReqPeriodPkt �Լ����� ȣ���) */
			/* ���� ���а� ���� �� ��û */
			uvEng_Luria_ReqGetMotorAbsPositionAll();
#endif
			if (++m_u32RepeatCount == m_u32RepeatTotal)
			{
				m_u8WorkStep	= 0x00;
				m_enWorkState	= ENG_JWNS::en_comp;
				m_bRunSpec		= FALSE;
				CalcACamSpec(TRUE);		/* ���� ������ Align Camera�� Angle�� Pixel Size ���ϱ� */
			}
			else
			{
				m_u8GrabbedCount= 0x00;
				m_u8WorkStep	= 0x05;	/* QuartZ �߽� ��ġ�� �̵� */
				m_dbRateLast	= 0.0f;
				CalcACamSpec(FALSE);	/* ���� ������ Align Camera�� Angle�� Pixel Size ���ϱ� */
			}
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
 desc : ���� �̵��ϰ��� �ϴ� ī�޶� �ƴ� ���, �� ���� or ������ ������ �̵� ó��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetACamMovingSide()
{
	DOUBLE dbMovePos	= 0.0f, dbMoveVelo = 0.0f;
	ENG_MMDI enACamDrv	= ENG_MMDI::en_align_cam1;
	LPG_CMSI pstCfgMC2	= &uvEng_GetConfig()->mc2_svc;
	LPG_MDSM pstMemMC2	= uvEng_ShMem_GetMC2();

	/* ������ ī�޶� Ÿ���� ��� */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)uvEng_GetConfig()->set_comn.align_camera_kind)
	{
		/* ���� ����� �ƴ� Align Camera Drive ID�� �̵��ϰ��� �ϴ� ��ġ ��� */
		if (m_u8ACamID == 0x01)	enACamDrv = ENG_MMDI::en_align_cam2;
		/* Align Camera ���� ���̵�� �̵��ϰ��� �ϴ� ��ġ ���� �ٸ� */
		if (enACamDrv == ENG_MMDI::en_align_cam2)
		{
			dbMovePos	= pstCfgMC2->max_dist[(UINT8)enACamDrv];
		}
		/* Align Camera 1 */
		else
		{
			dbMovePos	= pstCfgMC2->min_dist[(UINT8)enACamDrv];
		}
		/* Speed */
		dbMoveVelo= pstCfgMC2->max_velo[(UINT8)enACamDrv];

		/* ���� �̵��ϰ��� �ϴ� ����� Toggled �� ��� */
		uvCmn_MC2_GetDrvDoneToggled(enACamDrv);

		/* �� ���� Ȥ�� �� �������� ���� ����� �ƴ� ī�޶� �̵� ��Ű�� */
		if (!uvEng_MC2_SendDevAbsMove(enACamDrv, dbMovePos, dbMoveVelo))
		{
			return ENG_JWNS::en_error;
		}
	}
	/* ������ ī�޶��� ��� */
	else
	{
		/* �ƹ��� �۾��� �������� ���� */
		return ENG_JWNS::en_next;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : �� ������ �̵��ߴ��� ���� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsACamMovedSide()
{
	ENG_MMDI enACamDrv	= m_u8ACamID == 0x01 ? ENG_MMDI::en_align_cam2 : ENG_MMDI::en_align_cam1;
	/* ���Ͱ� ���߾����� Ȯ�� */
	if (!uvCmn_MC2_IsDrvDoneToggled(enACamDrv))	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : Align Camera�� Quartz�� �� ���̵��� Focusing�� ���� Z AXis ��ġ �̵�
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetACamMovingZAxisQuartz()
{
	/* Align Camera Z Axis �̵� */
	uvEng_MCQ_SetACamMovePosZ(m_u8ACamID, 0x00, uvEng_GetConfig()->acam_spec.acam_z_focus[m_u8ACamID-1]);
	return ENG_JWNS::en_next;
}

/*
 desc : Quartz �� Z Axis�� ��� �̵� �ߴ��� ���� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsACamMovedZAxisQuartz()
{
	DOUBLE dbACamZPos	= 0.0f, dbACamZSet = 0.0f;

	/* �̵��ϰ��� �ϴ� ī�޶��� Z �� Focus ��ġ */
	dbACamZSet	= uvEng_GetConfig()->acam_spec.acam_z_focus[m_u8ACamID-1];
	/* ���� ����� ī�޶��� ��ġ �� ��� */
	dbACamZPos	= uvCmn_MCQ_GetACamCurrentPosZ(m_u8ACamID);
	/* �Ѵ� �Ҽ��� ���� 4�ڸ������� ��ȿ�ϰ� ���� */
	dbACamZSet	= ROUNDED(dbACamZSet, 4);
	dbACamZPos	= ROUNDED(dbACamZPos, 4);

	/* �̵��ϰ��� �ϴ� ��ġ�� �̵��� ��ġ�� ���� ���� um ���� ���� �������� ���� */
	if (dbACamZSet != dbACamZPos)	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : Align Camera�� Quartz�� �ִ� ��ġ�� �̵�
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetACamMovingQuartz()
{
	ENG_MMDI enACamDrv	= m_u8ACamID == 0x01 ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
	DOUBLE dbMoveACamPos= 0, dbMoveXPos = 0, dbMoveYPos = 0;
	DOUBLE dbSpeedACam	= 0, dbMoveXSpeed = 0, dbMoveYSpeed;

	/* Quartz Position */
	dbMoveXPos		= uvEng_GetConfig()->acam_spec.quartz_stage_x;
	dbMoveYPos		= uvEng_GetConfig()->acam_spec.quartz_stage_y[m_u8ACamID-1];
	dbMoveACamPos	= uvEng_GetConfig()->acam_spec.quartz_acam[m_u8ACamID-1];

	/* Speed */
	dbSpeedACam		= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)enACamDrv];
	dbMoveXSpeed	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_x];
	dbMoveYSpeed	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_y];

	/* ���� �̵��ϰ��� �ϴ� ����� Toggled �� ��� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	uvCmn_MC2_GetDrvDoneToggled(enACamDrv);

	/* Motion �̵� ��Ű�� */
	if (!uvEng_MC2_SendDevAbsMove(enACamDrv,			dbMoveACamPos,	dbSpeedACam))	return ENG_JWNS::en_error;
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x,	dbMoveXPos,		dbMoveXSpeed))	return ENG_JWNS::en_error;
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y,	dbMoveYPos,		dbMoveYSpeed))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : Quartz ��ġ�� ��� �̵��� �Ϸ� �Ǿ����� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsACamMovedQuartz()
{
	ENG_MMDI enACamDrv	= m_u8ACamID == 0x01 ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
	if (!uvCmn_MC2_IsDrvDoneToggled(enACamDrv))			return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_x))	return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : ���� Trigger�� Strobe ������ Enable ��Ų��.
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetTrigEnabled()
{
	/* ���� Trigger Mode�� Enable �������� Ȯ�� */
	if (uvCmn_Trig_IsStrobEnable())	return ENG_JWNS::en_next;
	/* Trigger Mode (Trigger & Strobe)�� ������ Enable ��Ŵ */
	if (!uvEng_Trig_ReqTriggerStrobe(TRUE))
	{
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ���� Trigger�� Strobe�� Enable �������� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsTrigEnabled()
{
	/* ���� Trigger Mode�� Enable �������� Ȯ�� */
	if (uvCmn_Trig_IsStrobEnable())	return ENG_JWNS::en_next;

	return ENG_JWNS::en_wait;
}

/*
 desc : ���� ��ġ���� Mark Image�� Grab !!!
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::GetGrabQuartzData()
{
	UINT8 u8ChNo		= m_u8ACamID;	/* Camera Number�� Channel Number */
	ENG_MMDI enACamDrv	= m_u8ACamID == 0x01 ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
	UINT32 u32Model		= UINT32(ENG_MMDT::en_circle);
	DOUBLE dbParam[2]	= { 256.0f,
							(DOUBLE)ROUNDED(uvEng_GetConfig()->acam_spec.in_ring_size * 1000.0f, 0) / 2.0 };
#if 0	/* ���α׷� �ʱ� ������ ��, �ڵ����� ��� �� */
	/* Quartz �� Mark�� �ν��� Mark Model ��� */
	if (!uvEng_Camera_SetModelDefineEx(m_u8ACamID,
									   uvEng_GetConfig()->mark_find.model_speed,	/* Medium */
									   uvEng_GetConfig()->mark_find.model_smooth,
									   &u32Model,		/* circle */
									   &dbParam[0],		/* BLACK (256) or WHITE (128) */
									   &dbParam[1],		/* Width or Radius or Diameter, ... */
									   NULL, NULL, NULL, 1,
									   ))
	{
		return ENG_JWNS::en_error;
	}
#endif
	/* ���� Live & Edge & Calibration ������ �ʱ�ȭ */
	uvEng_Camera_ResetGrabbedImage();

	/* Trigger 1�� �߻� */
	if (!uvEng_Trig_ReqTrigOutOneOnly(u8ChNo))
	{
		return ENG_JWNS::en_error;
	}

	/* Image Grabbed ������ �ð� ���� */
	m_u64TickGrabbed	= GetTickCount64();

	return ENG_JWNS::en_next;
}

/*
 desc : Left �� ��ũ �̹����� Grabbed �ߴ��� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsGrabbQuartzData()
{
	ENG_MMDI enACamDrv	= m_u8ACamID == 0x01 ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
	LPG_ACGR pstGrab	= NULL;

	/* Grabbed Image�� �����ϴ��� Ȯ�� */
	pstGrab = uvEng_Camera_RunModelCali(m_u8ACamID, 0xff);	/* �̹��� �߽����� �̵��ϱ� ������ ������ 0xff �� */
	if (pstGrab && 0x00 != pstGrab->marked)
	{
		m_dbDiffCamX	= pstGrab->move_mm_x;	/* Axis X (Align Camera) Error Value */
		m_dbDiffStageY	= pstGrab->move_mm_y;	/* Axis Y (Stage) Error Value */
#if 0
		TRACE(L"m_dbDiffCamX = %9.4f m_dbDiffStageY = %9.4f\n", m_dbDiffCamX, m_dbDiffStageY);
#endif
		/* !!! �߿�. ���� ���� ���� 0.2 um �̳��̸�, �̵� ���� !!! */
		if (abs(m_dbDiffCamX) < MOTION_CALI_CENTERING_MIN &&
			abs(m_dbDiffStageY) < MOTION_CALI_CENTERING_MIN)
		{
			/* Quartz Mark �߽��� Left or Right �̵� �� �����ϱ� ���� ��ƾ���� Skip */
			m_u8WorkStep	= 0x0c;
		}
		/* ���� �ܰ�� �̵� */
		return ENG_JWNS::en_next;
	}

	/* 3 �� ���� Grabbed Image�� ���� ���, �ٽ� Ʈ���� �߻� ��Ű�� ���� �ڷ� �̵� */
	if (m_u64TickGrabbed+3000 < GetTickCount64())
	{
		m_u8WorkStep	-= 2;
		return ENG_JWNS::en_next;
	}

	return ENG_JWNS::en_wait;
}

/*
 desc : ���� ��ġ���� �־��� ���� ���� ��� �̼� ��ġ ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetMotionMoving()
{
	UINT8 u8InstAngle	= uvEng_GetConfig()->set_cams.acam_inst_angle;
	ENG_MDMD enDirect	= ENG_MDMD::en_move_left;
	ENG_MMDI enACamDrv	= m_u8ACamID == 0x01 ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
	DOUBLE dbACamPosX	= 0.0f, dbStagePosY = 0.0f;
	DOUBLE dbStepVelo	= uvEng_GetConfig()->mc2_svc.step_velo;

	/* ���� ��ġ ��� */
	dbACamPosX	= uvCmn_MC2_GetDrvAbsPos(enACamDrv);
	dbStagePosY	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);

	/* Align Camera �̵� */
	if (abs(m_dbDiffCamX) > MOTION_CALI_CENTERING_MIN)
	{
		uvCmn_MC2_GetDrvDoneToggled(enACamDrv);
#if 0
		if (m_dbDiffCamX < 0.0f)	enDirect = ENG_MDMD::en_move_left;	/* Mark ���� �� ���� ���� ��� ���̸� */
		else						enDirect = ENG_MDMD::en_move_right;	/* Mark ���� �� ���� ���� ���� ���̸� */
		if (!uvEng_MC2_SendDevRefMove(ENG_MMMM::en_move_step,
									  enACamDrv, enDirect, abs(m_dbDiffCamX)))
		{
			return ENG_JWNS::en_error;
		}
#else
		/* Align Camera�� X ��ġ�� QuartZ�� �߾� ��ġ�� �̵� */
		if (u8InstAngle)	dbACamPosX	-= m_dbDiffCamX;
		else				dbACamPosX	+= m_dbDiffCamX;
		if (!uvEng_MC2_SendDevAbsMove(enACamDrv, dbACamPosX, dbStepVelo))
			return ENG_JWNS::en_error;
#endif
	}
	/* Stage Y �̵� */
	if (abs(m_dbDiffStageY) > MOTION_CALI_CENTERING_MIN)
	{
		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
#if 0
		/* Stage Y Axis �̵� */
		if (m_dbDiffStageY < 0.0f)	enDirect = ENG_MDMD::en_move_down;	/* Mark ���� �� ���� ���� ��� ���̸� */
		else						enDirect = ENG_MDMD::en_move_up;	/* Mark ���� �� ���� ���� ���� ���̸� */
		/* Ư�� ��ġ�� �̵� */
		if (!uvEng_MC2_SendDevRefMove(ENG_MMMM::en_move_step,
									  ENG_MMDI::en_stage_y, enDirect, abs(m_dbDiffStageY)))
		{
			return ENG_JWNS::en_error;
		}
#else
		/* Align Camera�� X ��ġ�� QuartZ�� �߾� ��ġ�� �̵� */
		if (u8InstAngle)	dbStagePosY	+= m_dbDiffStageY;
		else				dbStagePosY	-= m_dbDiffStageY;
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbStagePosY, dbStepVelo))
			return ENG_JWNS::en_error;
#endif
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ���ϴ� ��ġ���� �̵��� �Ϸ� �Ǿ����� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsMotionMoved()
{
	ENG_MMDI enACamDrv	= m_u8ACamID == 0x01 ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
	/* �̼� �������� �̵��ϰ� �ִ� Align Camera�� Stage Y Axis�� ��� �̵��Ǿ����� Ȯ�� */
	if (abs(m_dbDiffCamX) > MOTION_CALI_CENTERING_MIN && !uvCmn_MC2_IsDrvDoneToggled(enACamDrv))
	{
		return ENG_JWNS::en_wait;
	}
	if (abs(m_dbDiffStageY) > MOTION_CALI_CENTERING_MIN && !uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))
	{
		return ENG_JWNS::en_wait;
	}
	/* Quartz �߽� �̵� �ϱ� ���� �ٽ� ���� ���� */
	m_u8WorkStep	= 0x08;

	return ENG_JWNS::en_next;
}

/*
 desc : ���� ���õ� ī�޶� ��������, Quartz�� Mark �߽� ��ġ�� ���� or ���������� �̵��ϱ� ����
		���� �Ÿ���ŭ Stage X ��ǥ �̵�
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetMotionMovingX()
{
	DOUBLE dbMovePos	= 0.0f, dbDist[2]	= {NULL}, dbMoveSpeed	= 0.0f;
	DOUBLE dbPointDist	= uvEng_GetConfig()->acam_spec.point_dist;

	/* �̵� �Ÿ� �Է� */
	dbDist[0]	= -dbPointDist / 2.0f;
	dbDist[1]	= dbPointDist;

	/* ���� ���õ� Align Camera */
	dbMovePos	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x) + dbDist[m_u8GrabbedCount];
	dbMoveSpeed	= 50;	/* Speed : 50 mm/sec */

	/* ���� �̵��ϰ��� �ϴ� ����� Toggled �� ��� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	/* ���̵�� �̵� ��Ű�� */
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dbMovePos, dbMoveSpeed))
	{
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ���� ���õ� ī�޶� ��������, Quartz�� Mark �߽� ��ġ�� ���� or ���������� �̵��ϱ� ����
		���� �Ÿ���ŭ Stage X ��ǥ �̵� �ߴ��� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsMotionMovedX()
{
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_x))	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : ���ϴ� �̹����� Grabbed �ߴ��� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsGrabbQuartzMoveX()
{
	LPG_ACGR pstGrab= NULL;

	/* Grabbed Image�� �����ϴ��� Ȯ�� */
	pstGrab = uvEng_Camera_RunModelCali(m_u8ACamID, m_u8RunMode);
	if (pstGrab && 0x00 != pstGrab->marked)
	{
		/* Grabbed Image�� Mark �߽� ��ǥ */
		m_stGrabCentXY[m_u8GrabbedCount].x	= pstGrab->mark_cent_px_x;
		m_stGrabCentXY[m_u8GrabbedCount].y	= pstGrab->mark_cent_px_y;
		m_stGrabMoveXY[m_u8GrabbedCount].x	= pstGrab->move_px_x;
		m_stGrabMoveXY[m_u8GrabbedCount].y	= pstGrab->move_px_y;
#if 0
		TRACE(L"cam_id[%u]  id[%u]  cent_px_xy = %.6f / %.5f  move_px_xy = %.6f / %.6f\n",
			  m_u8ACamID, m_u8GrabbedCount,
			  pstGrab->mark_cent_px_x, pstGrab->mark_cent_px_y,
			  pstGrab->move_px_x, pstGrab->move_px_y);
#endif
		/* ��� �۾����� Skip �������� ���� */
		if (0 == m_u8GrabbedCount)
		{
			m_u8GrabbedCount++;
			m_u8WorkStep	= 0x0c;	/* 2 ��° (Right) �����ϱ� ���� �̵� ��ġ */
			/* ���� �ܰ�� �̵� */
			return ENG_JWNS::en_next;
		}
		return ENG_JWNS::en_next;
	}

	/* 3 �� ���� Grabbed Image�� ���� ���, �ٽ� Ʈ���� �߻� ��Ű�� ���� �ڷ� �̵� */
	if (m_u64TickGrabbed+3000 < GetTickCount64())
	{
		m_u8WorkStep	-= 2;
		return ENG_JWNS::en_next;
	}

	return ENG_JWNS::en_wait;
}

/*
 desc : Align Camera�� Angle�� Pixel Resolution ���ϱ�
 parm : flag	- [in]  TRUE : ��� �۾� �Ϸ�, FALSE : ���� 1ȸ �Ϸ�
 retn : None
*/
VOID CMainThread::CalcACamSpec(BOOL flag)
{
	DOUBLE dbRadian		= 0.0f, dbDegree = 0.0f, dbPixelSize = 0.0f, dbCrossDist = 0.0f;
	DOUBLE dbMoveDist	= uvEng_GetConfig()->acam_spec.point_dist * 1000.0f;	/* unit : um */
	ENG_MMDI enACamDrv	= m_u8ACamID == 0x01 ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;

	/* Align Camera : Angle */
	dbRadian	= atan2((m_stGrabCentXY[0].y - m_stGrabCentXY[1].y), (m_stGrabCentXY[0].x - m_stGrabCentXY[1].x));
	dbDegree	= (dbRadian * 180.0f) / M_PI;
	TRACE(L"ACamNo = %d  Degree = %.4f\n", m_u8ACamID, dbDegree);
	if (uvEng_GetConfig()->set_cams.acam_inst_angle)
	{
		uvEng_GetConfig()->acam_spec.spec_angle[m_u8ACamID-1]	= dbDegree - 180.0f;	/* unit : degree */
	}
	else
	{
		uvEng_GetConfig()->acam_spec.spec_angle[m_u8ACamID-1]	= dbDegree;	/* unit : degree */
	}
	/* Align Camera : Pixel Resolution */
	dbCrossDist	= sqrt(pow(m_stGrabCentXY[0].x - m_stGrabCentXY[1].x, 2) + pow(m_stGrabCentXY[0].y - m_stGrabCentXY[1].y, 2));
	dbPixelSize = dbMoveDist / dbCrossDist;
	uvEng_GetConfig()->acam_spec.spec_pixel_um[m_u8ACamID-1]	= dbPixelSize;	/* unit : um */
#if 0
	TRACE(L"Align Camera [Degree / PixelSize] = %.8f (degree:%.8f), %.8f\n", 
		  uvEng_GetConfig()->acam_spec.spec_angle[m_u8ACamID-1], dbDegree, dbPixelSize);
#endif

	/* ��� ���� �θ𿡰� �˸� */
	SendMesgResult(flag);
}

/*
 desc : ���� �۾� ������ Ȯ���ϱ�
 parm : None
 retn : �۾� ����� �� (����: Percent)
*/
DOUBLE CMainThread::GetStepRate()
{
	DOUBLE dbRate	= 0.0f;

	dbRate	= ((m_u8WorkStep * 1.0f) / DOUBLE(m_u8WorkTotal)) * 100.0f;
	if (dbRate > m_dbRateLast)	m_dbRateLast	= dbRate;

	/* ���� �ֱٿ� ����� ���� ū ���� ���� ��ȯ */
	return m_dbRateLast;
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