
/*
 desc : Align Camera 2D Calibration 검증 (Shutting 후 오차 값 확인)
*/

#include "pch.h"
#include "../MainApp.h"
#include "WorkExam.h"

/*
 desc : 생성자
 parm : None
 retn : None
*/
CWorkExam::CWorkExam()
	: CWork()
{
	/* Grabbed Image 버퍼 할당 */
	m_pstGrabbedResult	= new STG_ACGR[2];
	ASSERT(m_pstGrabbedResult);
	memset(m_pstGrabbedResult, 0x00, (sizeof(STG_ACGR)-sizeof(PUINT8)) * 2);
	m_pstGrabbedResult[0].grab_data	= (PUINT8)::Alloc(m_u32GrabSize + 1);
	m_pstGrabbedResult[1].grab_data	= (PUINT8)::Alloc(m_u32GrabSize + 1);
	m_pstGrabbedResult[0].grab_data[m_u32GrabSize]	= 0x00;
	m_pstGrabbedResult[1].grab_data[m_u32GrabSize]	= 0x00;
}

/*
 desc : 파괴자
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
 desc : 초기 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkExam::InitWork()
{
	CWork::InitWork(0x01);	/* fixed : cam_id = 0x01 */

	/* 전체 작업 단계 */
	m_u8WorkTotal	= 0x08;
	m_dbTotalRate	= m_u8WorkTotal * m_pstCali->GetTotalCount();

	return TRUE;
}

/*
 desc : 주기적으로 호출됨
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

	/* Next Step 호출 */
	SetWorkNext();
	/* 장시간 동안 동일 위치를 반복 수행한다면 에러 처리 */
	IsWorkTimeOut();
}

/*
 desc : 다음 Step 루틴 처리
 parm : None
 retn : None
*/
VOID CWorkExam::SetWorkNext()
{
	/* 작업이 에러이거나 종료되었을 때 */
	if (ENG_JWNS::en_error == m_enWorkState || ENG_JWNS::en_comp == m_enWorkState)
	{
		m_u8WorkStep	= 0x00;
		return;
	}

	if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* 현재 1개 측정이 완료되면 */
		if (m_u8WorkStep == 0x06)	/* 1 개의 항목 (Column)을 측정 완료 했으면 */
		{
			m_u16MoveCols++;		/* 1개 측정이 완료되었으므로 */
			/* 모든 열 (Column)이 측정 완료되었으면, 1 행 (Row) 증가 */
			if (m_u16MoveCols == m_pstCali->set_cols_count)
			{
				m_u16MoveCols	= 0; /* 다음 열 (Col) 측정을 위해 초기화 */
				/* 모든 행 (Row)이 측정 완료되었으면, 종료 */
				if (++m_u16MoveRows == m_pstCali->set_rows_count)
				{
					m_enWorkState	= ENG_JWNS::en_comp;
					m_u64JobTime	= GetTickCount64() - m_u64StartTime;
				}
			}
			else
			{
				m_u8WorkStep	= 0x02;	/* 다음 측정 위치로 이동하기 위함 */
			}
		}
		/* 만약 행의 이동이 완료 되었으면, 다시 다음 측정 위치로 이동 */
		else if (m_u8WorkStep == 0x08)
		{
			m_u8WorkStep	= 0x02;	/* 처음 열 (Column) 측정 위치로 이동하기 위함 */
		}

		/* 무조건 다음 Step으로 이동 */
		m_u8WorkStep++;

		/* 가장 최근에 Waiting 한 시간 저장 */
		m_u64DelayTime	= GetTickCount64();
	}
}

/*
 desc : 트리거 1개 발생 시킴
 parm : None
 retn : wait or error or next or completed
*/
ENG_JWNS CWorkExam::PutOneTrigger()
{
	m_u64WaitTime	= GetTickCount64();

	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	/* 기존 Live & Edge & Calibration 데이터 초기화 */
	uvEng_Camera_ResetGrabbedImage();
	return uvEng_Trig_ReqTrigOutOne(0x01, FALSE) ? ENG_JWNS::en_next : ENG_JWNS::en_error;
}

/*
 desc : Grabbed Image에 대한 분석 결과가 있는지 여부
 parm : None
 retn : wait or error or next or completed
*/
ENG_JWNS CWorkExam::GetGrabResult()
{
	/* 이미지 Grab하고 처리될 때까지 일정 시간 대기 */
	if (GetTickCount64() < m_u64WaitTime + 500)	return ENG_JWNS::en_wait;

	/* 검색된 결과 정보 얻기 */
	if (uvEng_GetConfig()->IsRunDemo())
	{
		/* 가장 최근에 발생된 단차 (OffsetXY; StepXY) 값 임시 저장 */
		m_dbOffsetX	= uvCmn_GetRandNumerI32(0x01, 2) / 10000.0f;
		m_dbOffsetY	= uvCmn_GetRandNumerI32(0x01, 2) / 10000.0f;
	}
	else
	{
		if (!uvEng_Camera_RunModelExam(m_pstGrabbedResult))
		{
			/* 이미지가 grab되지 않았다면, 1초 정도 더 기다려 본다 */
			if ((GetTickCount64() - m_u64DelayTime) > 1000)
			{
				if (++m_u8TrigRetry < 3)	m_u8WorkStep	= 0x02;	/* Move to prev place */
				else						m_u8TrigRetry	= 0x00;	/* Trigger 개수 초기화 */

				/* 아직 정상적으로 측정되지 않았으므로 ... */
				m_dbOffsetX	= 0.9999f;
				m_dbOffsetY	= 0.9999f;
			}
			else	return ENG_JWNS::en_wait;
		}
		else
		{
			/* Trigger 재발생 시도 횟수 초기화 */
			m_u8TrigRetry	= 0x00;
			/* 검색된 2개의 중심 간의 오차 값 비교 */
			m_dbOffsetX	= m_pstGrabbedResult[0].mark_cent_mm_x - m_pstGrabbedResult[1].mark_cent_mm_x;
			m_dbOffsetY	= m_pstGrabbedResult[0].mark_cent_mm_y - m_pstGrabbedResult[1].mark_cent_mm_y;
		}
	}

	return ENG_JWNS::en_next;
}

/*
 desc : 다음 Column으로 이동하기 위한 모션 처리
 parm : None
 retn : wait or error or next or completed
*/
ENG_JWNS CWorkExam::SetMovingCols()
{
	DOUBLE dbVeloCols	= 100.0f;	/* 100 mm/sec : 아무리 속도를 높여도, 내부적으로 가장 가까운 거리를 이동할 때, 50 mm/sec로 제한 */
	DOUBLE dbNextCols, dbMoveCols;

	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	/* 현재 동작되는 얼라인 카메라의 토글 상태 값 저장 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);

	/* Align Camera 1번이 다음에 이동될 위치 계산 */
	dbNextCols	= m_pstCali->mark_acam[0] + DOUBLE(m_u16MoveCols+1) * m_pstCali->period_col_size;
	/* Align Camera가 현재 위치에서 원하는 위치로 이동해야할 거리 계산 */
	dbMoveCols	= abs(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1) - dbNextCols);

	/* 이동 거리에 따라, 카메라 이동 속도 결정 */
	if (abs(dbMoveCols) < 2.5000f)			dbVeloCols	=  10.0000;	/*  2.5 mm 이하 이동할 때,  10 mm/sec */
	else if (abs(dbMoveCols) < 5.0000f)		dbVeloCols	=  20.0000;	/*  5.0 mm 이하 이동할 때,  20 mm/sec */
	else if (abs(dbMoveCols) < 25.0000f)	dbVeloCols	=  50.0000;	/* 25.0 mm 이하 이동할 때,  50 mm/sec */
	else if (abs(dbMoveCols) < 50.0000f)	dbVeloCols	=  75.0000;	/* 50.0 mm 이하 이동할 때,  75 mm/sec */
	else									dbVeloCols	= 100.0000;	/* 50.0 mm 이상 이동할 때, 100 mm/sec */
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, dbNextCols, dbVeloCols))
	{
		return ENG_JWNS::en_error;
	}
	/* !!! 무조건 0 값으로 설정 !!! */
	m_u64MovedTick	= 0;

	return ENG_JWNS::en_next;
}

/*
 desc : 다음 Column으로 이동이 완료되었는지 여부
 parm : None
 retn : wait or error or next or completed
*/
ENG_JWNS CWorkExam::IsMovedCols()
{
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	/* 현재 동작하는 얼라인 카메라의 토글 값이 바뀌었는지 확인 */
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1))
	{
		/* 이 시간 이후로 일정 시간 정도 쉬어야 됨  */
		m_u64MovedTick	= GetTickCount64();
		return ENG_JWNS::en_wait;
	}
	/* 초기 값이면, 현재 시간 설정 */
	if (!m_u64MovedTick)
	{
		m_u64MovedTick	= GetTickCount64();
		return ENG_JWNS::en_wait;
	}
	/* 모터가 안정될 때까지 대기 설정된 일정 시간 대기 후 다음 작업을 진행할지 여부 결정 */
	if (m_u64MovedTick+m_pstCali->period_wait_time > GetTickCount64())
	{
		return ENG_JWNS::en_wait;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : 다음 Row으로 이동하기 위한 모션 처리
 parm : None
 retn : wait or error or next or completed
*/
ENG_JWNS CWorkExam::SetMovingRows()
{
	DOUBLE dbVeloRows	= 100.0f, dbVeloCols = 1000.0f;	/* 100 mm/sec : 아무리 속도를 높여도, 내부적으로 가장 가까운 거리를 이동할 때, 50 mm/sec로 제한 */
	DOUBLE dbNextRows, dbMoveRows, dbMoveCols;
	LPG_CMSI pstMC2	= &uvEng_GetConfig()->mc2_svc;

	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	/* 현재 동작되는 얼라인 카메라의 토글 상태 값 저장 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

	/* 임의 위치로 이동 */
	dbNextRows	= m_pstCali->mark_stage_y[0] + DOUBLE(m_u16MoveRows) * m_pstCali->period_row_size;
	/* 각 축별 이동 거리에 따른 속도 계산 */
	dbMoveRows	= abs(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y) - dbNextRows);
	dbMoveCols	= abs(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1) - m_pstCali->mark_stage_y[0]);

	/* 열 (Camera X 축) 위치 이동 거리에 따라, 카메라 이동 속도 결정 */
	if (abs(dbMoveCols) < 2.5000f)			dbVeloCols	=  10.0000;	/*  2.5 mm 이하 이동할 때,  10 mm/sec */
	else if (abs(dbMoveCols) < 5.0000f)		dbVeloCols	=  20.0000;	/*  5.0 mm 이하 이동할 때,  20 mm/sec */
	else if (abs(dbMoveCols) < 25.0000f)	dbVeloCols	=  50.0000;	/* 25.0 mm 이하 이동할 때,  50 mm/sec */
	else if (abs(dbMoveCols) < 50.0000f)	dbVeloCols	=  75.0000;	/* 50.0 mm 이하 이동할 때,  75 mm/sec */
	else									dbVeloCols	= 100.0000;	/* 50.0 mm 이상 이동할 때, 100 mm/sec */
	/* 행 (Stage Y 축) 위치 이동 거리에 따라, 카메라 이동 속도 결정 */
	if (abs(dbMoveRows) < 2.5000f)			dbVeloRows	=  10.0000;	/*  2.5 mm 이하 이동할 때,  10 mm/sec */
	else if (abs(dbMoveRows) < 5.0000f)		dbVeloRows	=  20.0000;	/*  5.0 mm 이하 이동할 때,  20 mm/sec */
	else if (abs(dbMoveRows) < 25.0000f)	dbVeloRows	=  50.0000;	/* 25.0 mm 이하 이동할 때,  50 mm/sec */
	else if (abs(dbMoveRows) < 50.0000f)	dbVeloRows	=  75.0000;	/* 50.0 mm 이하 이동할 때,  75 mm/sec */
	else									dbVeloRows	= 100.0000;	/* 50.0 mm 이상 이동할 때, 100 mm/sec */

	/* 실제 다음 측정 위치로 행과 열의 모션 위치 이동*/
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, m_pstCali->mark_acam[0], dbVeloCols) ||
		!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbNextRows, dbVeloRows))
	{
		return ENG_JWNS::en_error;
	}
	/* !!! 무조건 0 값으로 설정 !!! */
	m_u64MovedTick	= 0;

	return ENG_JWNS::en_next;
}

/*
 desc : 다음 Row으로 이동이 완료되었는지 여부
 parm : None
 retn : wait or error or next or completed
*/
ENG_JWNS CWorkExam::IsMovedRows()
{
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	/* 현재 동작하는 얼라인 카메라의 토글 값이 바뀌었는지 확인 */
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1) ||
		!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))
	{
		/* 이 시간 이후로 일정 시간 정도 쉬어야 됨  */
		m_u64MovedTick	= GetTickCount64();
		return ENG_JWNS::en_wait;
	}
	/* 초기 값이면, 현재 시간 설정 */
	if (!m_u64MovedTick)
	{
		m_u64MovedTick	= GetTickCount64();
		return ENG_JWNS::en_wait;
	}
	/* 모터가 안정될 때까지 대기 설정된 일정 시간 대기 후 다음 작업을 진행할지 여부 결정 */
	if (m_u64MovedTick+m_pstCali->period_wait_time > GetTickCount64())
	{
		return ENG_JWNS::en_wait;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : 현재 Grabbed Image의 처리 결과 값 반환
 parm : index	- [in]  측정 모드일 경우 무조건 0, 검증 모드일 경우 0 or 1
 retn : 처리 결과 값이 저장된 구조체 포인터 반환
*/
LPG_ACGR CWorkExam::GetGrabbedResult(UINT8 index)
{
	/* 검증 (Inspection) 모드일 경우, 인덱스 값에 따라 반환 */
	if (index > 1)	return NULL;
	return &m_pstGrabbedResult[index];
}