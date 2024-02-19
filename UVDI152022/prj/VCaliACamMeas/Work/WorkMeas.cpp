
/*
 desc : Align Camera 2D Calibration ����
*/

#include "pch.h"
#include "../MainApp.h"
#include "WorkMeas.h"


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
CWorkMeas::CWorkMeas()
	: CWork()
{
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CWorkMeas::~CWorkMeas()
{
}


/*
 desc : �ʱ� �۾� ����
 parm : cam_id	- [in]  ���� ���� ī�޶� �ε��� (1 or 2)
		speed	- [in]  Material Thickness (unit: um)
		meas	- [in]  Measurement Information
 retn : TRUE or FALSE
*/
BOOL CWorkMeas::InitWork(UINT8 cam_id, UINT16 thick, LPG_ACCS meas)
{
	CWork::InitWork(cam_id);

	/* ��ü �۾� �ܰ� */
	m_u8WorkTotal	= 0x0e;
	m_dbTotalRate	= m_u8WorkTotal * m_pstCali->GetTotalCount();
	m_u16Thickness	= thick;
	m_pstMeas		= meas;

	return TRUE;
}

/*
 desc : �ֱ������� �۾� ����
 parm : None
 retn : None
*/
VOID CWorkMeas::DoWork()
{
	/* �۾� �ܰ� ���� ���� ó�� */
	switch (m_u8StepIt)
	{
	case 0x01 : m_enWorkState = SetMovingInit();		break;
	case 0x02 : m_enWorkState = IsMovedInited();		break;
	/* ���� ��ġ���� ��ũ �ν� ���� */
	case 0x03 : m_enWorkState = GetGrabMarkData();		break;
	case 0x04 : m_enWorkState = IsGrabbedMarkData();	break;
	case 0x05 : m_enWorkState = SetMotionMoving();		break;
	case 0x06 : m_enWorkState = IsMotionMoved();		break;
	/* �� (Row)���� �̵� */
	case 0x07 : m_enWorkState = SetMovingRows();		break;
	case 0x08 : m_enWorkState = IsMovedRows();			break;
	/* �� (Column)�� �̵� */
	case 0x09 : m_enWorkState = SetMovingCols();		break;
	case 0x0a : m_enWorkState = IsMovedCols();			break;
	/* Mark Calibration ������ ���� */
	case 0x0b : m_enWorkState = RunACamCaliData();		break;
	case 0x0c : m_enWorkState = IsACamCaliData();		break;
	/* �� (Column) ��� �̵� �Ϸ� ���� */
	case 0x0d : m_enWorkState = IsCompletedCols();		break;
	/* �� (Row) ��� �̵� �Ϸ� ���� */
	case 0x0e : m_enWorkState = IsCompletedRows();		break;
	}

	/* ���� �۾� ���� ���� �Ǵ� */
	SetWorkNext();
	/* ��ð� ���� ���� ��ġ�� �ݺ� �����Ѵٸ� ���� ó�� */
	if (IsWorkTimeout())	m_enWorkState	= ENG_JWNS::en_time;
}

/*
 desc : ���� �ܰ�� �̵��ϱ� ���� ó��
 parm : None
 retn : None
*/
VOID CWorkMeas::SetWorkNext()
{
	/* �۾��� �����̰ų� ����Ǿ��� �� */
	if (ENG_JWNS::en_error == m_enWorkState || ENG_JWNS::en_comp == m_enWorkState)
	{
		m_u8StepIt	= 0x00;
		return;
	}

	if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* ��� �� (Column)�� �� (Row)�� ������ �Ϸ�Ǿ����� ���� */
		if (m_u16MoveCols == m_pstCali->set_cols_count &&
			m_u16MoveRows == m_pstCali->set_rows_count)
		{
			m_enWorkState	= ENG_JWNS::en_comp;
			m_u64WorkTime	= GetTickCount64() - m_u64StartTime;
			CloseCaliFile();	/* Close the file */
		}
		else
		{
			/* ������ ���� Step���� �̵� */
			m_u8StepIt++;
		}
		/* Save the last time you worked */
		m_u64WorkTime	= GetTickCount64();
	}
}

/*
 desc : ���� ��ġ���� Mark Image�� Grab !!!
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkMeas::GetGrabMarkData()
{
	UINT8 u8ChNo	= m_u8ACamID == 0x01 ? 0x01 : 0x02;

	/* ���� Live & Edge & Calibration ������ �ʱ�ȭ */
	uvEng_Camera_ResetGrabbedImage();
	/* Trigger 1�� �߻� */
	if (!uvEng_Trig_ReqTrigOutOneOnly(u8ChNo))
	{
		return ENG_JWNS::en_error;
	}

	/* Image Grabbed ������ �ð� ���� */
	m_u64GrabbedTick	= GetTickCount64();

	return ENG_JWNS::en_next;
}

/*
 desc : ���ϴ� �̹����� Grabbed �ߴ��� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkMeas::IsGrabbedMarkData()
{
	TCHAR tzVal[32]	= {NULL};
	ENG_MMDI enDrvID= ENG_MMDI(INT32(ENG_MMDI::en_align_cam1) + INT32(m_u8ACamID - 1));

	if (uvEng_GetConfig()->IsRunDemo())
	{
		/* �⺻ ��� ��� */
		if (!SaveBaseData())	return ENG_JWNS::en_error;
		m_u8StepIt	= 0x06;	/* goto RunACamCaliData() */
		/* ���� �ܰ�� �̵� */
		return ENG_JWNS::en_next;
	}
	else
	{
		/* Grabbed Image�� �����ϴ��� Ȯ�� */
		m_pstGrabbedResult = uvEng_Camera_RunModelCali(m_u8ACamID, 0xff);
		if (m_pstGrabbedResult && 0x00 != m_pstGrabbedResult->marked)
		{
			/* Axis X (Align Camera or Stage) Difference Value */
			m_dbDiffX	= m_pstGrabbedResult->move_mm_x;
			/* Axis Y (Stage) Difference Value */
			m_dbDiffY	= m_pstGrabbedResult->move_mm_y;
			/* !!! �߿�. ���� ���� ���� 0.5 um �̳��̸�, �ٷ� Inspection Step���� Skip !!! */
			if (abs(m_dbDiffX) < MOTION_CALI_CENTERING_MIN &&
				abs(m_dbDiffY) < MOTION_CALI_CENTERING_MIN)
			{
				m_u8StepIt	= 0x06;	/* goto RunACamCaliData() */
				/* ���� Motion Drive�� ���� ��ġ ���� ���� ȯ�� ���Ͽ� ���� */
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
				m_pstCali->mark_acam[m_u8ACamID-1]		= uvCmn_MC2_GetDrvAbsPos(enDrvID);
#else
				m_pstCali->mark_stage_x[m_u8ACamID-1]	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);
#endif
				m_pstCali->mark_stage_y[m_u8ACamID-1]	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);
				/* �⺻ ��� ��� */
				if (!SaveBaseData())	return ENG_JWNS::en_error;
			}
			/* ���� �ܰ�� �̵� */
			return ENG_JWNS::en_next;
		}
	}

	/* 3 �� ���� Grabbed Image�� ���� ���, �ٽ� Ʈ���� �߻� ��Ű�� ���� �ڷ� �̵� */
	if (m_u64GrabbedTick+3000 < GetTickCount64())
	{
		m_u8StepIt	-= 2;
		return ENG_JWNS::en_next;
	}

	return ENG_JWNS::en_wait;
}

/*
 desc : ���� ��ġ���� �־��� ���� ���� ��� �̼� ��ġ ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkMeas::SetMotionMoving()
{
	ENG_MDMD enDirect	= ENG_MDMD::en_move_left;
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
	ENG_MMDI enDrvID	= ENG_MMDI(INT32(ENG_MMDI::en_align_cam1) + INT32(m_u8ACamID - 1));
#else
	ENG_MMDI enDrvID	= ENG_MMDI::en_stage_x;
#endif

	/* Move the alignment camera or the stage X axis */
	if (abs(m_dbDiffX) >= MOTION_CALI_CENTERING_MIN)
	{
		/* ī�޶� ȸ�� ��ġ ���ο� ����... �ݴ�� �������� �� */
		if (uvEng_GetConfig()->set_cams.acam_inst_angle)	m_dbDiffX *= -1.0f;
		uvCmn_MC2_GetDrvDoneToggled(enDrvID);

		/* Move the align camera or the stage x axis */
		if (m_dbDiffX < 0)	enDirect = ENG_MDMD::en_move_left;	/* Mark ���� �� ���� ���� ��� ���̸� */
		else				enDirect = ENG_MDMD::en_move_right;	/* Mark ���� �� ���� ���� ���� ���̸� */
		if (!uvEng_MC2_SendDevRefMove(ENG_MMMM::en_move_step, enDrvID, enDirect, m_dbDiffX))
		{
			return ENG_JWNS::en_error;
		}
	}
	/* Move the stage Y axis */
	if (abs(m_dbDiffY) >= MOTION_CALI_CENTERING_MIN)
	{
		/* ī�޶� ȸ�� ��ġ ���ο� ����... �ݴ�� �������� �� */
		if (uvEng_GetConfig()->set_cams.acam_inst_angle)	m_dbDiffY *= -1.0f;
		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
		/* Stage Y Axis �̵� */
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_PODIS_LLS10 ||\
	DELIVERY_PRODUCT_ID == CUSTOM_CODE_PODIS_LLS06)
		if (m_dbDiffY > 0)	enDirect = ENG_MDMD::en_move_down;	/* Mark ���� �� ���� ���� ��� ���̸� */
		else				enDirect = ENG_MDMD::en_move_up;	/* Mark ���� �� ���� ���� ���� ���̸� */
#else
	AfxMessageBox(L"It is not currently supported (CWorkMeas::SetMotionMoving Function)", MB_ICONSTOP|MB_TOPMOST);
#endif
		/* Ư�� ��ġ�� �̵� */
		if (!uvEng_MC2_SendDevRefMove(ENG_MMMM::en_move_step, ENG_MMDI::en_stage_y, enDirect, m_dbDiffY))
		{
			return ENG_JWNS::en_error;
		}
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ���ϴ� ��ġ���� �̵��� �Ϸ� �Ǿ����� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkMeas::IsMotionMoved()
{
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
	ENG_MMDI enDrvID	= ENG_MMDI(INT32(ENG_MMDI::en_align_cam1) + INT32(m_u8ACamID - 1));
#else
	ENG_MMDI enDrvID	= ENG_MMDI::en_stage_x;
#endif

	/* �̼� �������� �̵��ϰ� �ִ� Align Camera�� Stage Y Axis�� ��� �̵��Ǿ����� Ȯ�� */
	if (abs(m_dbDiffX) >= MOTION_CALI_CENTERING_MIN && !uvCmn_MC2_IsDrvDoneToggled(enDrvID))
	{
		return ENG_JWNS::en_wait;
	}
	if (abs(m_dbDiffY) >= MOTION_CALI_CENTERING_MIN && !uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))
	{
		return ENG_JWNS::en_wait;
	}
	/* !!! �߿�. �ٽ� Align Mark ���� ������ ���ؼ� Work Step �缳�� !!! */
	m_u8StepIt	= 0x02;	/* goto GetGrabMarkData() */

	return ENG_JWNS::en_next;
}

/*
 desc : Calibration Data �����ϱ� ���� �̺�Ʈ ó��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkMeas::RunACamCaliData()
{
#if 0
	/* ���� �θ� �ʿ��� �������� �ʾҴٰ� ���� */
	if (m_bGrabbedResult)	return ENG_JWNS::en_wait;
#endif
	/* ���� Live & Edge & Calibration ������ �ʱ�ȭ */
	uvEng_Camera_ResetGrabbedImage();

	/* Trigger 1�� �߻� */
	if (!uvEng_Trig_ReqTrigOutOneOnly(m_u8ACamID))
	{
		return ENG_JWNS::en_error;
	}

	/* Image Grabbed ������ �ð� ���� */
	m_u64GrabbedTick	= GetTickCount64();

	return ENG_JWNS::en_next;
}

/*
 desc : ���ϴ� �̹����� Grabbed �ߴ��� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkMeas::IsACamCaliData()
{
	TCHAR tzVal[128] = {NULL};
	DOUBLE dbMoveX, dbMoveY;
	ENG_MMDI enACamID= ENG_MMDI(INT32(ENG_MMDI::en_align_cam1) + INT32(m_u8ACamID - 1));

	if (uvEng_GetConfig()->IsRunDemo())
	{
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
		dbMoveX	= m_pstCali->mark_acam[m_u8ACamID-1] + m_pstMeas->it_cols[m_u16MoveCols];
#else
		dbMoveX	= m_pstCali->mark_stage_x[m_u8ACamID-1] + m_pstMeas->it_cols[m_u16MoveCols];
#endif
		dbMoveY	= m_pstCali->mark_stage_y[m_u8ACamID-1] + m_pstMeas->it_rows[m_u16MoveRows];
		/* ���� �ֱٿ� �߻��� ���� (OffsetXY; StepXY) �� �ӽ� ���� */
		m_dbOffsetX	= uvCmn_GetRandNumerI32(0x01, 2) / 10000.0f;
		m_dbOffsetY	= uvCmn_GetRandNumerI32(0x01, 2) / 10000.0f;
		/* Align Camera X, Stage Y, Axis X / Y Error Value */
		swprintf_s(tzVal, 128, L"%+10.4f,%+10.4f,%+8.4f,%+8.4f,\n",
				   dbMoveX, dbMoveY, m_dbOffsetX, m_dbOffsetY);
		if (EOF == fputws(tzVal, m_fpCali))
		{
			AfxMessageBox(L"Failed to save to the calibration file", MB_ICONSTOP|MB_TOPMOST);
			return ENG_JWNS::en_error;
		}
		return ENG_JWNS::en_next;
	}
	else
	{
		/* Grabbed Image�� �����ϴ��� Ȯ�� */
		m_pstGrabbedResult = uvEng_Camera_RunModelCali(m_u8ACamID, 0xff);
		if (m_pstGrabbedResult && 0x00 != m_pstGrabbedResult->marked)
		{
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
			dbMoveX	= uvCmn_MC2_GetDrvAbsPos(enACamID);
#else
			dbMoveX	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);
#endif
			dbMoveY	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);
			/* Align Camera X, Stage Y, Axis X / Y Error Value */
			swprintf_s(tzVal, 128, L"%+10.4f,%+10.4f,%+8.4f,%+8.4f,\n",
					   dbMoveX, dbMoveY, m_pstGrabbedResult->move_mm_x, m_pstGrabbedResult->move_mm_y);
#if 0
			TRACE(L"IsACamCaliData() [%d][%d] = %s", m_u16MoveRows, m_u16MoveCols, tzVal);
#endif
			if (EOF == fputws(tzVal, m_fpCali))
			{
				AfxMessageBox(L"Failed to save to the calibration file", MB_ICONSTOP|MB_TOPMOST);
				return ENG_JWNS::en_error;
			}
			/* ��ũ �ν� �õ� Ƚ�� �ʱ�ȭ */
			m_u8TrigRetry	= 0x00;

			/* ���� �ֱٿ� �߻��� ���� (OffsetXY; StepXY) �� �ӽ� ���� */
			m_dbOffsetX	= m_pstGrabbedResult->move_mm_x;
			m_dbOffsetY	= m_pstGrabbedResult->move_mm_y;

			return ENG_JWNS::en_next;
		}
	}

	/* 3 �� ���� Grabbed Image�� ���� ���, �ٽ� Ʈ���� �߻� ��Ű�� ���� �ڷ� �̵� */
	if (m_u64GrabbedTick+3000 < GetTickCount64())
	{
		m_u8StepIt	-= 2;	/* goto RunACamCaliData() */
		/* �� 3�� �õ��ϴ� ���, ���� ó�� */
		if (++m_u8TrigRetry > 2)
		{
			return ENG_JWNS::en_error;
		}
		return ENG_JWNS::en_next;
	}

	return ENG_JWNS::en_wait;
}

/*
 desc : Moves from the current position to the absolute position by a given value interval.
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkMeas::SetMovingCols()
{
	DOUBLE dbVeloCols	= 100.0f;	/* 100 mm/sec : �ƹ��� �ӵ��� ������, ���������� ���� ����� �Ÿ��� �̵��� ��, 50 mm/sec�� ���� */
	DOUBLE dbNextCols, dbMoveCols;
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
	ENG_MMDI enDrvID	= ENG_MMDI(INT32(ENG_MMDI::en_align_cam1) + INT32(m_u8ACamID - 1));
#else
	ENG_MMDI enDrvID	= ENG_MMDI::en_stage_x;
#endif

	/* Get the toggle state value of the current drive */
	uvCmn_MC2_GetDrvDoneToggled(enDrvID);
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
	/* Calculates the location value of the drive to be moved next */
	dbNextCols	= m_pstCali->mark_acam[m_u8ACamID-1] + m_pstMeas->it_cols[m_u16MoveCols];
#else
	/* Calculates the location value of the drive to be moved next */
	dbNextCols	= m_pstCali->mark_stage_x[m_u8ACamID-1] + m_pstMeas->it_cols[m_u16MoveCols];
#endif

#if 0
	TRACE(L"SetMovingCols() : Moving Pos (ROW:%03u, COL:%03u) = X : %10.4f\n",
		  m_u16MoveRows, m_u16MoveCols, dbNextCols);
#endif
	/* Align Camera�� ���� ��ġ���� ���ϴ� ��ġ�� �̵��ؾ��� �Ÿ� ��� */
	dbMoveCols	= abs(uvCmn_MC2_GetDrvAbsPos(enDrvID) - dbNextCols);

	/* �̵� �Ÿ��� ����, ī�޶� �̵� �ӵ� ���� */
	if (dbMoveCols < 2.5000f)		dbVeloCols	=  10.0000;	/*  2.5 mm ���� �̵��� ��,  10 mm/sec */
	else if (dbMoveCols < 5.0000f)	dbVeloCols	=  20.0000;	/*  5.0 mm ���� �̵��� ��,  20 mm/sec */
	else if (dbMoveCols < 25.0000f)	dbVeloCols	=  50.0000;	/* 25.0 mm ���� �̵��� ��,  50 mm/sec */
	else if (dbMoveCols < 50.0000f)	dbVeloCols	=  75.0000;	/* 50.0 mm ���� �̵��� ��,  75 mm/sec */
	else							dbVeloCols	= 100.0000;	/* 50.0 mm �̻� �̵��� ��, 100 mm/sec */
	if (!uvEng_MC2_SendDevAbsMove(enDrvID, dbNextCols, dbVeloCols))
	{
		return ENG_JWNS::en_error;
	}

	/* !!! ������ 0 ������ ���� !!! */
	m_u64MovedTick	= 0;
	/* Save the time the motion moved */
	SetMotionTime();

	return ENG_JWNS::en_next;
}

/*
 desc : ���ϴ� ��ġ���� �̵��� �Ϸ� �Ǿ����� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkMeas::IsMovedCols()
{
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
	ENG_MMDI enDrvID	= ENG_MMDI(INT32(ENG_MMDI::en_align_cam1) + INT32(m_u8ACamID - 1));
#else
	ENG_MMDI enDrvID	= ENG_MMDI::en_stage_x;
#endif
#if 0
	/* Check if the timeout for moving the motion has occurred */
	if (IsMotionTimeout(20000))
	{
		m_u8StepIt	= 0x08;	/* Skip back to the motion movement section */
		return ENG_JWNS::en_next;
	}
#endif
	/* ���� �����ϴ� ����� ī�޶��� ��� ���� �ٲ������ Ȯ�� */
	if (!uvCmn_MC2_IsDrvDoneToggled(enDrvID))
	{
		/* �� �ð� ���ķ� ���� �ð� ���� ����� ��  */
		m_u64MovedTick	= GetTickCount64();
		return ENG_JWNS::en_wait;
	}
	/* �ʱ� ���̸�, ���� �ð� ���� */
	if (!m_u64MovedTick)
	{
		m_u64MovedTick	= GetTickCount64();
		return ENG_JWNS::en_wait;
	}
	/* ���Ͱ� ������ ������ ��� ������ ���� �ð� ��� �� ���� �۾��� �������� ���� ���� */
	if (m_u64MovedTick+m_pstCali->period_wait_time > GetTickCount64())
	{
		return ENG_JWNS::en_wait;
	}
#if 0
	TRACE(L"IsMovedCols() : Moving Pos (ROW:%03u, COL:%03u) = X : %10.4f (wait:%u)\n",
		  m_u16MoveRows, m_u16MoveCols, uvCmn_MC2_GetDrvAbsPos(enDrvID), GetTickCount64() - m_u64MovedTick);
#endif
	return ENG_JWNS::en_next;
}

/*
 desc : ��� �� (Column)�� �̵� �Ϸ� �Ǿ����� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkMeas::IsCompletedCols()
{
	/* ��� �� (Column)�� ���� �Ϸ�Ǿ�����, 1 �� (Row) ���� */
	if (++m_u16MoveCols != m_pstCali->set_cols_count)
	{
		m_u8StepIt	= 0x08;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ���� ��ġ���� �־��� �� ���ݸ�ŭ ���� ��ġ�� �̵�
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkMeas::SetMovingRows()
{
	DOUBLE dbVeloRows	= 100.0f, dbVeloCols = 1000.0f;	/* 100 mm/sec : �ƹ��� �ӵ��� ������, ���������� ���� ����� �Ÿ��� �̵��� ��, 50 mm/sec�� ���� */
	DOUBLE dbNextRows, dbMoveRows, dbMoveCols;
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
	ENG_MMDI enDrvID	= ENG_MMDI(INT32(ENG_MMDI::en_align_cam1) + INT32(m_u8ACamID - 1));
#else
	ENG_MMDI enDrvID	= ENG_MMDI::en_stage_x;
#endif

	/* ���� ���۵Ǵ� ����� ī�޶��� ��� ���� �� ���� */
	uvCmn_MC2_GetDrvDoneToggled(enDrvID);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

	/* ���� ��ġ�� �̵� */
	dbNextRows	= m_pstCali->mark_stage_y[m_u8ACamID-1] + m_pstMeas->it_rows[m_u16MoveRows];
#if 0
	TRACE(L"SetMovingCols() : Moving Pos (ROW:%03u, COL:%03u) = Y : %10.4f\n",
		  m_u16MoveRows, m_u16MoveCols, dbNextRows);
#endif
	/* Align Camera & Stage�� ���� ��ġ���� ���ϴ� ��ġ�� �̵��ؾ��� �Ÿ� ��� */
	dbMoveRows	= abs(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y) - dbNextRows);
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
	dbMoveCols	= abs(uvCmn_MC2_GetDrvAbsPos(enDrvID) - m_pstCali->mark_acam[m_u8ACamID-1]);
#else
	dbMoveCols	= abs(uvCmn_MC2_GetDrvAbsPos(enDrvID) - m_pstCali->mark_stage_x[m_u8ACamID-1]);
#endif

	/* �� (Camera X ��) ��ġ �̵� �Ÿ��� ����, ī�޶� �̵� �ӵ� ���� */
	if (abs(dbMoveCols) < 2.5000f)			dbVeloCols	=  10.0000;	/*  2.5 mm ���� �̵��� ��,  10 mm/sec */
	else if (abs(dbMoveCols) < 5.0000f)		dbVeloCols	=  20.0000;	/*  5.0 mm ���� �̵��� ��,  20 mm/sec */
	else if (abs(dbMoveCols) < 25.0000f)	dbVeloCols	=  50.0000;	/* 25.0 mm ���� �̵��� ��,  50 mm/sec */
	else if (abs(dbMoveCols) < 50.0000f)	dbVeloCols	=  75.0000;	/* 50.0 mm ���� �̵��� ��,  75 mm/sec */
	else									dbVeloCols	= 100.0000;	/* 50.0 mm �̻� �̵��� ��, 100 mm/sec */
	/* �� (Stage Y ��) ��ġ �̵� �Ÿ��� ����, ī�޶� �̵� �ӵ� ���� */
	if (abs(dbMoveRows) < 2.5000f)			dbVeloRows	=  10.0000;	/*  2.5 mm ���� �̵��� ��,  10 mm/sec */
	else if (abs(dbMoveRows) < 5.0000f)		dbVeloRows	=  20.0000;	/*  5.0 mm ���� �̵��� ��,  20 mm/sec */
	else if (abs(dbMoveRows) < 25.0000f)	dbVeloRows	=  50.0000;	/* 25.0 mm ���� �̵��� ��,  50 mm/sec */
	else if (abs(dbMoveRows) < 50.0000f)	dbVeloRows	=  75.0000;	/* 50.0 mm ���� �̵��� ��,  75 mm/sec */
	else									dbVeloRows	= 100.0000;	/* 50.0 mm �̻� �̵��� ��, 100 mm/sec */

	/* ���� ���� ���� ��ġ�� ��� ���� ��� ��ġ �̵�*/
	if (!uvEng_MC2_SendDevAbsMove(enDrvID, m_pstCali->mark_acam[m_u8ACamID-1], dbVeloCols) ||
		!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbNextRows, dbVeloRows))
	{
		return ENG_JWNS::en_error;
	}

	/* !!! ������ 0 ������ �ʱ�ȭ !!! */
	m_u64MovedTick	= 0;
	/* Save the time the motion moved */
	SetMotionTime();

	return ENG_JWNS::en_next;
}

/*
 desc : ���ϴ� ��ġ���� �̵��� �Ϸ� �Ǿ����� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkMeas::IsMovedRows()
{
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
	ENG_MMDI enDrvID	= ENG_MMDI(INT32(ENG_MMDI::en_align_cam1) + INT32(m_u8ACamID - 1));
#else
	ENG_MMDI enDrvID	= ENG_MMDI::en_stage_x;
#endif
#if 0
	/* Check if the timeout for moving the motion has occurred */
	if (IsMotionTimeout(30000))
	{
		m_u8StepIt	= 0x0a;	/* Skip back to the motion movement section */
		return ENG_JWNS::en_next;
	}
#endif
	/* ���� �����ϴ� ����� ī�޶��� ��� ���� �ٲ������ Ȯ�� */
	if (!uvCmn_MC2_IsDrvDoneToggled(enDrvID) ||
		!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))
	{
		/* �� �ð� ���ķ� ���� �ð� ���� ����� ��  */
		m_u64MovedTick	= GetTickCount64();
		return ENG_JWNS::en_wait;
	}
	/* �ʱ� ���̸�, ���� �ð� ���� */
	if (!m_u64MovedTick)
	{
		m_u64MovedTick	= GetTickCount64();
		return ENG_JWNS::en_wait;
	}
	/* ������ ���� �ð� ��� �� ���� �۾��� �������� ���� ���� */
	if (m_u64MovedTick+m_pstCali->period_wait_time > GetTickCount64())
	{
		return ENG_JWNS::en_wait;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ��� �� (Row)�� �̵� �Ϸ� �Ǿ����� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkMeas::IsCompletedRows()
{
	/* ��� �� (Column)�� ���� �Ϸ�Ǿ�����, 1 �� (Row) ���� */
	if (++m_u16MoveRows != m_pstCali->set_rows_count)
	{
		m_u16MoveCols	= 0x0000;
		m_u8StepIt		= 0x06;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : �⺻ ������ ���� �� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkMeas::SaveBaseData()
{
	TCHAR tzMsg[256]	= {NULL};
	TCHAR tzValue[32]	= {NULL};
	errno_t eRet		= NULL;
	CUniToChar csCnv;

	/* ���� �����Ϸ��� ���ϸ� ���� Local Time */
	swprintf_s(m_tzFile, MAX_PATH_LEN, L"%s\\xy2d\\%s_%04u_%u.dat",
			   g_tzWorkDir, uvEng_GetConfig()->file_dat.acam_cali, m_u16Thickness, m_u8ACamID);

	/* ���� �����Ϸ��� ���ϸ� ���� */
	if (uvCmn_FindFile(m_tzFile))
	{
		/* �б� ���� ���� ������ �����ϱ� ���� */
		if (!uvCmn_DeleteForceFile(m_tzFile))	return FALSE;
	}
	/* Close the file */
	CloseCaliFile();
	/* Open the file */
	eRet = fopen_s(&m_fpCali, csCnv.Uni2Ansi(m_tzFile), "wt");
	if (0 != eRet)
	{
		swprintf_s(tzMsg, 256, L"Failed to open the file (%s) (err:%d)", m_tzFile, eRet);
		AfxMessageBox(tzMsg, MB_ICONSTOP|MB_TOPMOST);
		if (0 == eRet)	fclose(m_fpCali);
		m_fpCali	= NULL;
		return FALSE;
	}

	/* ���� ���� ��ġ ���� ���� */
	wmemset(tzValue, 0x00, 32);	swprintf_s(tzValue, 32, L"%.4f\n", m_pstCali->mark_acam[m_u8ACamID-1]);
	fputws(tzValue, m_fpCali);
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
	wmemset(tzValue, 0x00, 32);	swprintf_s(tzValue, 32, L"%.4f\n", m_pstCali->mark_stage_x);
#else
	wmemset(tzValue, 0x00, 32);	swprintf_s(tzValue, 32, L"%.4f\n", m_pstCali->mark_stage_x[m_u8ACamID-1]);
#endif
	fputws(tzValue, m_fpCali);
	wmemset(tzValue, 0x00, 32);	swprintf_s(tzValue, 32, L"%.4f\n", m_pstCali->mark_stage_y[m_u8ACamID-1]);
	fputws(tzValue, m_fpCali);
#if (0x10 == (0x10 & USE_ALIGN_CAMERA_2D_CALI_VER))
	/* �����ϰ��� �ϴ� �������� ���� (��� ��) ���� */
	wmemset(tzValue, 0x00, 32);	swprintf_s(tzValue, 32, L"%.4f\n", m_pstCali->period_row_size);
	fputws(tzValue, m_fpCali);
	wmemset(tzValue, 0x00, 32);	swprintf_s(tzValue, 32, L"%.4f\n", m_pstCali->period_col_size);
	fputws(tzValue, m_fpCali);
#endif
	/* �����ϰ��� �ϴ� �������� ���� (��� ��) ���� */
	wmemset(tzValue, 0x00, 32);	swprintf_s(tzValue, 32, L"%u\n", m_pstCali->set_rows_count);
	fputws(tzValue, m_fpCali);
	wmemset(tzValue, 0x00, 32);	swprintf_s(tzValue, 32, L"%u\n", m_pstCali->set_cols_count);
	fputws(tzValue, m_fpCali);

	return TRUE;
}

