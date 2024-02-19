
/*
 desc : Align Camera 2D Calibration ���� (Shutting �� ���� �� Ȯ��)
*/

#include "pch.h"
#include "../MainApp.h"
#include "WorkExam.h"

/*
 desc : ������
 parm : None
 retn : None
*/
CWorkExam::CWorkExam()
	: CWork()
{
	/* Grabbed Image ���� �Ҵ� */
	m_pstGrabbedResult	= new STG_ACGR[2];
	ASSERT(m_pstGrabbedResult);
	memset(m_pstGrabbedResult, 0x00, (sizeof(STG_ACGR)-sizeof(PUINT8)) * 2);
	m_pstGrabbedResult[0].grab_data	= (PUINT8)::Alloc(m_u32GrabSize + 1);
	m_pstGrabbedResult[1].grab_data	= (PUINT8)::Alloc(m_u32GrabSize + 1);
	m_pstGrabbedResult[0].grab_data[m_u32GrabSize]	= 0x00;
	m_pstGrabbedResult[1].grab_data[m_u32GrabSize]	= 0x00;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CWorkExam::~CWorkExam()
{
	if (m_pstGrabbedResult)
	{
		if (m_pstGrabbedResult[0].grab_data)	::Free(m_pstGrabbedResult[0].grab_data);
		if (m_pstGrabbedResult[1].grab_data)	::Free(m_pstGrabbedResult[1].grab_data);
		delete [] m_pstGrabbedResult;
	}
}

/*
 desc : �ʱ� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkExam::InitWork()
{
	CWork::InitWork(0x01);	/* fixed : cam_id = 0x01 */

	/* ��ü �۾� �ܰ� */
	m_u8WorkTotal	= 0x08;
	m_dbTotalRate	= m_u8WorkTotal * m_pstCali->GetTotalCount();

	return TRUE;
}

/*
 desc : �ֱ������� ȣ���
 parm : None
 retn : None
*/
VOID CWorkExam::DoWork()
{
	switch (m_u8WorkStep)
	{
	case 0x01	:	m_enWorkState = SetMovingInit();	break;
	case 0x02	:	m_enWorkState = IsMovedInited();	break;
	case 0x03	:	m_enWorkState = PutOneTrigger();	break;
	case 0x04	:	m_enWorkState = GetGrabResult();	break;
	case 0x05	:	m_enWorkState = SetMovingCols();	break;
	case 0x06	:	m_enWorkState = IsMovedCols();		break;
	case 0x07	:	m_enWorkState = SetMovingRows();	break;
	case 0x08	:	m_enWorkState = IsMovedRows();		break;
	}

	/* Next Step ȣ�� */
	SetWorkNext();
	/* ��ð� ���� ���� ��ġ�� �ݺ� �����Ѵٸ� ���� ó�� */
	IsWorkTimeOut();
}

/*
 desc : ���� Step ��ƾ ó��
 parm : None
 retn : None
*/
VOID CWorkExam::SetWorkNext()
{
	/* �۾��� �����̰ų� ����Ǿ��� �� */
	if (ENG_JWNS::en_error == m_enWorkState || ENG_JWNS::en_comp == m_enWorkState)
	{
		m_u8WorkStep	= 0x00;
		return;
	}

	if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* ���� 1�� ������ �Ϸ�Ǹ� */
		if (m_u8WorkStep == 0x06)	/* 1 ���� �׸� (Column)�� ���� �Ϸ� ������ */
		{
			m_u16MoveCols++;		/* 1�� ������ �Ϸ�Ǿ����Ƿ� */
			/* ��� �� (Column)�� ���� �Ϸ�Ǿ�����, 1 �� (Row) ���� */
			if (m_u16MoveCols == m_pstCali->set_cols_count)
			{
				m_u16MoveCols	= 0; /* ���� �� (Col) ������ ���� �ʱ�ȭ */
				/* ��� �� (Row)�� ���� �Ϸ�Ǿ�����, ���� */
				if (++m_u16MoveRows == m_pstCali->set_rows_count)
				{
					m_enWorkState	= ENG_JWNS::en_comp;
					m_u64JobTime	= GetTickCount64() - m_u64StartTime;
				}
			}
			else
			{
				m_u8WorkStep	= 0x02;	/* ���� ���� ��ġ�� �̵��ϱ� ���� */
			}
		}
		/* ���� ���� �̵��� �Ϸ� �Ǿ�����, �ٽ� ���� ���� ��ġ�� �̵� */
		else if (m_u8WorkStep == 0x08)
		{
			m_u8WorkStep	= 0x02;	/* ó�� �� (Column) ���� ��ġ�� �̵��ϱ� ���� */
		}

		/* ������ ���� Step���� �̵� */
		m_u8WorkStep++;

		/* ���� �ֱٿ� Waiting �� �ð� ���� */
		m_u64DelayTime	= GetTickCount64();
	}
}

/*
 desc : Ʈ���� 1�� �߻� ��Ŵ
 parm : None
 retn : wait or error or next or completed
*/
ENG_JWNS CWorkExam::PutOneTrigger()
{
	m_u64WaitTime	= GetTickCount64();

	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	/* ���� Live & Edge & Calibration ������ �ʱ�ȭ */
	uvEng_Camera_ResetGrabbedImage();
	return uvEng_Trig_ReqTrigOutOne(0x01, FALSE) ? ENG_JWNS::en_next : ENG_JWNS::en_error;
}

/*
 desc : Grabbed Image�� ���� �м� ����� �ִ��� ����
 parm : None
 retn : wait or error or next or completed
*/
ENG_JWNS CWorkExam::GetGrabResult()
{
	/* �̹��� Grab�ϰ� ó���� ������ ���� �ð� ��� */
	if (GetTickCount64() < m_u64WaitTime + 500)	return ENG_JWNS::en_wait;

	/* �˻��� ��� ���� ��� */
	if (uvEng_GetConfig()->IsRunDemo())
	{
		/* ���� �ֱٿ� �߻��� ���� (OffsetXY; StepXY) �� �ӽ� ���� */
		m_dbOffsetX	= uvCmn_GetRandNumerI32(0x01, 2) / 10000.0f;
		m_dbOffsetY	= uvCmn_GetRandNumerI32(0x01, 2) / 10000.0f;
	}
	else
	{
		if (!uvEng_Camera_RunModelExam(m_pstGrabbedResult))
		{
			/* �̹����� grab���� �ʾҴٸ�, 1�� ���� �� ��ٷ� ���� */
			if ((GetTickCount64() - m_u64DelayTime) > 1000)
			{
				if (++m_u8TrigRetry < 3)	m_u8WorkStep	= 0x02;	/* Move to prev place */
				else						m_u8TrigRetry	= 0x00;	/* Trigger ���� �ʱ�ȭ */

				/* ���� ���������� �������� �ʾ����Ƿ� ... */
				m_dbOffsetX	= 0.9999f;
				m_dbOffsetY	= 0.9999f;
			}
			else	return ENG_JWNS::en_wait;
		}
		else
		{
			/* Trigger ��߻� �õ� Ƚ�� �ʱ�ȭ */
			m_u8TrigRetry	= 0x00;
			/* �˻��� 2���� �߽� ���� ���� �� �� */
			m_dbOffsetX	= m_pstGrabbedResult[0].mark_cent_mm_x - m_pstGrabbedResult[1].mark_cent_mm_x;
			m_dbOffsetY	= m_pstGrabbedResult[0].mark_cent_mm_y - m_pstGrabbedResult[1].mark_cent_mm_y;
		}
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ���� Column���� �̵��ϱ� ���� ��� ó��
 parm : None
 retn : wait or error or next or completed
*/
ENG_JWNS CWorkExam::SetMovingCols()
{
	DOUBLE dbVeloCols	= 100.0f;	/* 100 mm/sec : �ƹ��� �ӵ��� ������, ���������� ���� ����� �Ÿ��� �̵��� ��, 50 mm/sec�� ���� */
	DOUBLE dbNextCols, dbMoveCols;

	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	/* ���� ���۵Ǵ� ����� ī�޶��� ��� ���� �� ���� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);

	/* Align Camera 1���� ������ �̵��� ��ġ ��� */
	dbNextCols	= m_pstCali->mark_acam[0] + DOUBLE(m_u16MoveCols+1) * m_pstCali->period_col_size;
	/* Align Camera�� ���� ��ġ���� ���ϴ� ��ġ�� �̵��ؾ��� �Ÿ� ��� */
	dbMoveCols	= abs(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1) - dbNextCols);

	/* �̵� �Ÿ��� ����, ī�޶� �̵� �ӵ� ���� */
	if (abs(dbMoveCols) < 2.5000f)			dbVeloCols	=  10.0000;	/*  2.5 mm ���� �̵��� ��,  10 mm/sec */
	else if (abs(dbMoveCols) < 5.0000f)		dbVeloCols	=  20.0000;	/*  5.0 mm ���� �̵��� ��,  20 mm/sec */
	else if (abs(dbMoveCols) < 25.0000f)	dbVeloCols	=  50.0000;	/* 25.0 mm ���� �̵��� ��,  50 mm/sec */
	else if (abs(dbMoveCols) < 50.0000f)	dbVeloCols	=  75.0000;	/* 50.0 mm ���� �̵��� ��,  75 mm/sec */
	else									dbVeloCols	= 100.0000;	/* 50.0 mm �̻� �̵��� ��, 100 mm/sec */
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, dbNextCols, dbVeloCols))
	{
		return ENG_JWNS::en_error;
	}
	/* !!! ������ 0 ������ ���� !!! */
	m_u64MovedTick	= 0;

	return ENG_JWNS::en_next;
}

/*
 desc : ���� Column���� �̵��� �Ϸ�Ǿ����� ����
 parm : None
 retn : wait or error or next or completed
*/
ENG_JWNS CWorkExam::IsMovedCols()
{
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	/* ���� �����ϴ� ����� ī�޶��� ��� ���� �ٲ������ Ȯ�� */
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1))
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

	return ENG_JWNS::en_next;
}

/*
 desc : ���� Row���� �̵��ϱ� ���� ��� ó��
 parm : None
 retn : wait or error or next or completed
*/
ENG_JWNS CWorkExam::SetMovingRows()
{
	DOUBLE dbVeloRows	= 100.0f, dbVeloCols = 1000.0f;	/* 100 mm/sec : �ƹ��� �ӵ��� ������, ���������� ���� ����� �Ÿ��� �̵��� ��, 50 mm/sec�� ���� */
	DOUBLE dbNextRows, dbMoveRows, dbMoveCols;
	LPG_CMSI pstMC2	= &uvEng_GetConfig()->mc2_svc;

	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	/* ���� ���۵Ǵ� ����� ī�޶��� ��� ���� �� ���� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

	/* ���� ��ġ�� �̵� */
	dbNextRows	= m_pstCali->mark_stage_y[0] + DOUBLE(m_u16MoveRows) * m_pstCali->period_row_size;
	/* �� �ະ �̵� �Ÿ��� ���� �ӵ� ��� */
	dbMoveRows	= abs(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y) - dbNextRows);
	dbMoveCols	= abs(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1) - m_pstCali->mark_stage_y[0]);

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
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, m_pstCali->mark_acam[0], dbVeloCols) ||
		!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbNextRows, dbVeloRows))
	{
		return ENG_JWNS::en_error;
	}
	/* !!! ������ 0 ������ ���� !!! */
	m_u64MovedTick	= 0;

	return ENG_JWNS::en_next;
}

/*
 desc : ���� Row���� �̵��� �Ϸ�Ǿ����� ����
 parm : None
 retn : wait or error or next or completed
*/
ENG_JWNS CWorkExam::IsMovedRows()
{
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	/* ���� �����ϴ� ����� ī�޶��� ��� ���� �ٲ������ Ȯ�� */
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1) ||
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
	/* ���Ͱ� ������ ������ ��� ������ ���� �ð� ��� �� ���� �۾��� �������� ���� ���� */
	if (m_u64MovedTick+m_pstCali->period_wait_time > GetTickCount64())
	{
		return ENG_JWNS::en_wait;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ���� Grabbed Image�� ó�� ��� �� ��ȯ
 parm : index	- [in]  ���� ����� ��� ������ 0, ���� ����� ��� 0 or 1
 retn : ó�� ��� ���� ����� ����ü ������ ��ȯ
*/
LPG_ACGR CWorkExam::GetGrabbedResult(UINT8 index)
{
	/* ���� (Inspection) ����� ���, �ε��� ���� ���� ��ȯ */
	if (index > 1)	return NULL;
	return &m_pstGrabbedResult[index];
}