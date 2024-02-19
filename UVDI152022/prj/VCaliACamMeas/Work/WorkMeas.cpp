
/*
 desc : Align Camera 2D Calibration 측정
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
 desc : 생성자
 parm : None
 retn : None
*/
CWorkMeas::CWorkMeas()
	: CWork()
{
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CWorkMeas::~CWorkMeas()
{
}


/*
 desc : 초기 작업 수행
 parm : cam_id	- [in]  오차 측정 카메라 인덱스 (1 or 2)
		speed	- [in]  Material Thickness (unit: um)
		meas	- [in]  Measurement Information
 retn : TRUE or FALSE
*/
BOOL CWorkMeas::InitWork(UINT8 cam_id, UINT16 thick, LPG_ACCS meas)
{
	CWork::InitWork(cam_id);

	/* 전체 작업 단계 */
	m_u8WorkTotal	= 0x0e;
	m_dbTotalRate	= m_u8WorkTotal * m_pstCali->GetTotalCount();
	m_u16Thickness	= thick;
	m_pstMeas		= meas;

	return TRUE;
}

/*
 desc : 주기적으로 작업 수행
 parm : None
 retn : None
*/
VOID CWorkMeas::DoWork()
{
	/* 작업 단계 별로 동작 처리 */
	switch (m_u8StepIt)
	{
	case 0x01 : m_enWorkState = SetMovingInit();		break;
	case 0x02 : m_enWorkState = IsMovedInited();		break;
	/* 현재 위치에서 마크 인식 수행 */
	case 0x03 : m_enWorkState = GetGrabMarkData();		break;
	case 0x04 : m_enWorkState = IsGrabbedMarkData();	break;
	case 0x05 : m_enWorkState = SetMotionMoving();		break;
	case 0x06 : m_enWorkState = IsMotionMoved();		break;
	/* 행 (Row)으로 이동 */
	case 0x07 : m_enWorkState = SetMovingRows();		break;
	case 0x08 : m_enWorkState = IsMovedRows();			break;
	/* 열 (Column)로 이동 */
	case 0x09 : m_enWorkState = SetMovingCols();		break;
	case 0x0a : m_enWorkState = IsMovedCols();			break;
	/* Mark Calibration 데이터 수집 */
	case 0x0b : m_enWorkState = RunACamCaliData();		break;
	case 0x0c : m_enWorkState = IsACamCaliData();		break;
	/* 열 (Column) 모두 이동 완료 여부 */
	case 0x0d : m_enWorkState = IsCompletedCols();		break;
	/* 행 (Row) 모두 이동 완료 여부 */
	case 0x0e : m_enWorkState = IsCompletedRows();		break;
	}

	/* 다음 작업 진행 여부 판단 */
	SetWorkNext();
	/* 장시간 동안 동일 위치를 반복 수행한다면 에러 처리 */
	if (IsWorkTimeout())	m_enWorkState	= ENG_JWNS::en_time;
}

/*
 desc : 다음 단계로 이동하기 위한 처리
 parm : None
 retn : None
*/
VOID CWorkMeas::SetWorkNext()
{
	/* 작업이 에러이거나 종료되었을 때 */
	if (ENG_JWNS::en_error == m_enWorkState || ENG_JWNS::en_comp == m_enWorkState)
	{
		m_u8StepIt	= 0x00;
		return;
	}

	if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* 모든 열 (Column)과 행 (Row)의 측정이 완료되었는지 여부 */
		if (m_u16MoveCols == m_pstCali->set_cols_count &&
			m_u16MoveRows == m_pstCali->set_rows_count)
		{
			m_enWorkState	= ENG_JWNS::en_comp;
			m_u64WorkTime	= GetTickCount64() - m_u64StartTime;
			CloseCaliFile();	/* Close the file */
		}
		else
		{
			/* 무조건 다음 Step으로 이동 */
			m_u8StepIt++;
		}
		/* Save the last time you worked */
		m_u64WorkTime	= GetTickCount64();
	}
}

/*
 desc : 현재 위치에서 Mark Image를 Grab !!!
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkMeas::GetGrabMarkData()
{
	UINT8 u8ChNo	= m_u8ACamID == 0x01 ? 0x01 : 0x02;

	/* 기존 Live & Edge & Calibration 데이터 초기화 */
	uvEng_Camera_ResetGrabbedImage();
	/* Trigger 1개 발생 */
	if (!uvEng_Trig_ReqTrigOutOneOnly(u8ChNo))
	{
		return ENG_JWNS::en_error;
	}

	/* Image Grabbed 시작한 시간 저장 */
	m_u64GrabbedTick	= GetTickCount64();

	return ENG_JWNS::en_next;
}

/*
 desc : 원하는 이미지를 Grabbed 했는지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkMeas::IsGrabbedMarkData()
{
	TCHAR tzVal[32]	= {NULL};
	ENG_MMDI enDrvID= ENG_MMDI(INT32(ENG_MMDI::en_align_cam1) + INT32(m_u8ACamID - 1));

	if (uvEng_GetConfig()->IsRunDemo())
	{
		/* 기본 헤드 출력 */
		if (!SaveBaseData())	return ENG_JWNS::en_error;
		m_u8StepIt	= 0x06;	/* goto RunACamCaliData() */
		/* 다음 단계로 이동 */
		return ENG_JWNS::en_next;
	}
	else
	{
		/* Grabbed Image가 존재하는지 확인 */
		m_pstGrabbedResult = uvEng_Camera_RunModelCali(m_u8ACamID, 0xff);
		if (m_pstGrabbedResult && 0x00 != m_pstGrabbedResult->marked)
		{
			/* Axis X (Align Camera or Stage) Difference Value */
			m_dbDiffX	= m_pstGrabbedResult->move_mm_x;
			/* Axis Y (Stage) Difference Value */
			m_dbDiffY	= m_pstGrabbedResult->move_mm_y;
			/* !!! 중요. 만약 오차 값이 0.5 um 이내이면, 바로 Inspection Step으로 Skip !!! */
			if (abs(m_dbDiffX) < MOTION_CALI_CENTERING_MIN &&
				abs(m_dbDiffY) < MOTION_CALI_CENTERING_MIN)
			{
				m_u8StepIt	= 0x06;	/* goto RunACamCaliData() */
				/* 현재 Motion Drive의 절대 위치 값을 얻어와 환경 파일에 갱신 */
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
				m_pstCali->mark_acam[m_u8ACamID-1]		= uvCmn_MC2_GetDrvAbsPos(enDrvID);
#else
				m_pstCali->mark_stage_x[m_u8ACamID-1]	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);
#endif
				m_pstCali->mark_stage_y[m_u8ACamID-1]	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);
				/* 기본 헤드 출력 */
				if (!SaveBaseData())	return ENG_JWNS::en_error;
			}
			/* 다음 단계로 이동 */
			return ENG_JWNS::en_next;
		}
	}

	/* 3 초 동안 Grabbed Image가 없는 경우, 다시 트리거 발생 시키기 위해 뒤로 이동 */
	if (m_u64GrabbedTick+3000 < GetTickCount64())
	{
		m_u8StepIt	-= 2;
		return ENG_JWNS::en_next;
	}

	return ENG_JWNS::en_wait;
}

/*
 desc : 현재 위치에서 주어진 오차 값만 모션 미세 위치 조정
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
		/* 카메라 회전 설치 여부에 따라... 반대로 움직여야 됨 */
		if (uvEng_GetConfig()->set_cams.acam_inst_angle)	m_dbDiffX *= -1.0f;
		uvCmn_MC2_GetDrvDoneToggled(enDrvID);

		/* Move the align camera or the stage x axis */
		if (m_dbDiffX < 0)	enDirect = ENG_MDMD::en_move_left;	/* Mark 측정 후 오차 값이 양수 값이면 */
		else				enDirect = ENG_MDMD::en_move_right;	/* Mark 측정 후 오차 값이 음수 값이면 */
		if (!uvEng_MC2_SendDevRefMove(ENG_MMMM::en_move_step, enDrvID, enDirect, m_dbDiffX))
		{
			return ENG_JWNS::en_error;
		}
	}
	/* Move the stage Y axis */
	if (abs(m_dbDiffY) >= MOTION_CALI_CENTERING_MIN)
	{
		/* 카메라 회전 설치 여부에 따라... 반대로 움직여야 됨 */
		if (uvEng_GetConfig()->set_cams.acam_inst_angle)	m_dbDiffY *= -1.0f;
		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
		/* Stage Y Axis 이동 */
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_PODIS_LLS10 ||\
	DELIVERY_PRODUCT_ID == CUSTOM_CODE_PODIS_LLS06)
		if (m_dbDiffY > 0)	enDirect = ENG_MDMD::en_move_down;	/* Mark 측정 후 오차 값이 양수 값이면 */
		else				enDirect = ENG_MDMD::en_move_up;	/* Mark 측정 후 오차 값이 음수 값이면 */
#else
	AfxMessageBox(L"It is not currently supported (CWorkMeas::SetMotionMoving Function)", MB_ICONSTOP|MB_TOPMOST);
#endif
		/* 특정 위치로 이동 */
		if (!uvEng_MC2_SendDevRefMove(ENG_MMMM::en_move_step, ENG_MMDI::en_stage_y, enDirect, m_dbDiffY))
		{
			return ENG_JWNS::en_error;
		}
	}

	return ENG_JWNS::en_next;
}

/*
 desc : 원하는 위치까지 이동이 완료 되었는지 확인
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

	/* 미세 조정으로 이동하고 있는 Align Camera와 Stage Y Axis이 모두 이동되었는지 확인 */
	if (abs(m_dbDiffX) >= MOTION_CALI_CENTERING_MIN && !uvCmn_MC2_IsDrvDoneToggled(enDrvID))
	{
		return ENG_JWNS::en_wait;
	}
	if (abs(m_dbDiffY) >= MOTION_CALI_CENTERING_MIN && !uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))
	{
		return ENG_JWNS::en_wait;
	}
	/* !!! 중요. 다시 Align Mark 오차 측정을 위해서 Work Step 재설정 !!! */
	m_u8StepIt	= 0x02;	/* goto GetGrabMarkData() */

	return ENG_JWNS::en_next;
}

/*
 desc : Calibration Data 수집하기 위해 이벤트 처리
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkMeas::RunACamCaliData()
{
#if 0
	/* 아직 부모 쪽에서 갱신하지 않았다고 설정 */
	if (m_bGrabbedResult)	return ENG_JWNS::en_wait;
#endif
	/* 기존 Live & Edge & Calibration 데이터 초기화 */
	uvEng_Camera_ResetGrabbedImage();

	/* Trigger 1개 발생 */
	if (!uvEng_Trig_ReqTrigOutOneOnly(m_u8ACamID))
	{
		return ENG_JWNS::en_error;
	}

	/* Image Grabbed 시작한 시간 저장 */
	m_u64GrabbedTick	= GetTickCount64();

	return ENG_JWNS::en_next;
}

/*
 desc : 원하는 이미지를 Grabbed 했는지 확인
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
		/* 가장 최근에 발생된 단차 (OffsetXY; StepXY) 값 임시 저장 */
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
		/* Grabbed Image가 존재하는지 확인 */
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
			/* 마크 인식 시도 횟수 초기화 */
			m_u8TrigRetry	= 0x00;

			/* 가장 최근에 발생된 단차 (OffsetXY; StepXY) 값 임시 저장 */
			m_dbOffsetX	= m_pstGrabbedResult->move_mm_x;
			m_dbOffsetY	= m_pstGrabbedResult->move_mm_y;

			return ENG_JWNS::en_next;
		}
	}

	/* 3 초 동안 Grabbed Image가 없는 경우, 다시 트리거 발생 시키기 위해 뒤로 이동 */
	if (m_u64GrabbedTick+3000 < GetTickCount64())
	{
		m_u8StepIt	-= 2;	/* goto RunACamCaliData() */
		/* 총 3번 시도하는 경우, 에러 처리 */
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
	DOUBLE dbVeloCols	= 100.0f;	/* 100 mm/sec : 아무리 속도를 높여도, 내부적으로 가장 가까운 거리를 이동할 때, 50 mm/sec로 제한 */
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
	/* Align Camera가 현재 위치에서 원하는 위치로 이동해야할 거리 계산 */
	dbMoveCols	= abs(uvCmn_MC2_GetDrvAbsPos(enDrvID) - dbNextCols);

	/* 이동 거리에 따라, 카메라 이동 속도 결정 */
	if (dbMoveCols < 2.5000f)		dbVeloCols	=  10.0000;	/*  2.5 mm 이하 이동할 때,  10 mm/sec */
	else if (dbMoveCols < 5.0000f)	dbVeloCols	=  20.0000;	/*  5.0 mm 이하 이동할 때,  20 mm/sec */
	else if (dbMoveCols < 25.0000f)	dbVeloCols	=  50.0000;	/* 25.0 mm 이하 이동할 때,  50 mm/sec */
	else if (dbMoveCols < 50.0000f)	dbVeloCols	=  75.0000;	/* 50.0 mm 이하 이동할 때,  75 mm/sec */
	else							dbVeloCols	= 100.0000;	/* 50.0 mm 이상 이동할 때, 100 mm/sec */
	if (!uvEng_MC2_SendDevAbsMove(enDrvID, dbNextCols, dbVeloCols))
	{
		return ENG_JWNS::en_error;
	}

	/* !!! 무조건 0 값으로 설정 !!! */
	m_u64MovedTick	= 0;
	/* Save the time the motion moved */
	SetMotionTime();

	return ENG_JWNS::en_next;
}

/*
 desc : 원하는 위치까지 이동이 완료 되었는지 확인
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
	/* 현재 동작하는 얼라인 카메라의 토글 값이 바뀌었는지 확인 */
	if (!uvCmn_MC2_IsDrvDoneToggled(enDrvID))
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
#if 0
	TRACE(L"IsMovedCols() : Moving Pos (ROW:%03u, COL:%03u) = X : %10.4f (wait:%u)\n",
		  m_u16MoveRows, m_u16MoveCols, uvCmn_MC2_GetDrvAbsPos(enDrvID), GetTickCount64() - m_u64MovedTick);
#endif
	return ENG_JWNS::en_next;
}

/*
 desc : 모든 열 (Column)이 이동 완료 되었는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkMeas::IsCompletedCols()
{
	/* 모든 열 (Column)이 측정 완료되었으면, 1 행 (Row) 증가 */
	if (++m_u16MoveCols != m_pstCali->set_cols_count)
	{
		m_u8StepIt	= 0x08;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : 현재 위치에서 주어진 값 간격만큼 절대 위치로 이동
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkMeas::SetMovingRows()
{
	DOUBLE dbVeloRows	= 100.0f, dbVeloCols = 1000.0f;	/* 100 mm/sec : 아무리 속도를 높여도, 내부적으로 가장 가까운 거리를 이동할 때, 50 mm/sec로 제한 */
	DOUBLE dbNextRows, dbMoveRows, dbMoveCols;
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
	ENG_MMDI enDrvID	= ENG_MMDI(INT32(ENG_MMDI::en_align_cam1) + INT32(m_u8ACamID - 1));
#else
	ENG_MMDI enDrvID	= ENG_MMDI::en_stage_x;
#endif

	/* 현재 동작되는 얼라인 카메라의 토글 상태 값 저장 */
	uvCmn_MC2_GetDrvDoneToggled(enDrvID);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

	/* 임의 위치로 이동 */
	dbNextRows	= m_pstCali->mark_stage_y[m_u8ACamID-1] + m_pstMeas->it_rows[m_u16MoveRows];
#if 0
	TRACE(L"SetMovingCols() : Moving Pos (ROW:%03u, COL:%03u) = Y : %10.4f\n",
		  m_u16MoveRows, m_u16MoveCols, dbNextRows);
#endif
	/* Align Camera & Stage가 현재 위치에서 원하는 위치로 이동해야할 거리 계산 */
	dbMoveRows	= abs(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y) - dbNextRows);
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
	dbMoveCols	= abs(uvCmn_MC2_GetDrvAbsPos(enDrvID) - m_pstCali->mark_acam[m_u8ACamID-1]);
#else
	dbMoveCols	= abs(uvCmn_MC2_GetDrvAbsPos(enDrvID) - m_pstCali->mark_stage_x[m_u8ACamID-1]);
#endif

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
	if (!uvEng_MC2_SendDevAbsMove(enDrvID, m_pstCali->mark_acam[m_u8ACamID-1], dbVeloCols) ||
		!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbNextRows, dbVeloRows))
	{
		return ENG_JWNS::en_error;
	}

	/* !!! 무조건 0 값으로 초기화 !!! */
	m_u64MovedTick	= 0;
	/* Save the time the motion moved */
	SetMotionTime();

	return ENG_JWNS::en_next;
}

/*
 desc : 원하는 위치까지 이동이 완료 되었는지 확인
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
	/* 현재 동작하는 얼라인 카메라의 토글 값이 바뀌었는지 확인 */
	if (!uvCmn_MC2_IsDrvDoneToggled(enDrvID) ||
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
	/* 설정된 일정 시간 대기 후 다음 작업을 진행할지 여부 결정 */
	if (m_u64MovedTick+m_pstCali->period_wait_time > GetTickCount64())
	{
		return ENG_JWNS::en_wait;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : 모든 행 (Row)이 이동 완료 되었는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkMeas::IsCompletedRows()
{
	/* 모든 열 (Column)이 측정 완료되었으면, 1 행 (Row) 증가 */
	if (++m_u16MoveRows != m_pstCali->set_rows_count)
	{
		m_u16MoveCols	= 0x0000;
		m_u8StepIt		= 0x06;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : 기본 데이터 생성 및 저장
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkMeas::SaveBaseData()
{
	TCHAR tzMsg[256]	= {NULL};
	TCHAR tzValue[32]	= {NULL};
	errno_t eRet		= NULL;
	CUniToChar csCnv;

	/* 현재 저장하려는 파일명 설정 Local Time */
	swprintf_s(m_tzFile, MAX_PATH_LEN, L"%s\\xy2d\\%s_%04u_%u.dat",
			   g_tzWorkDir, uvEng_GetConfig()->file_dat.acam_cali, m_u16Thickness, m_u8ACamID);

	/* 기존 저장하려는 파일명 삭제 */
	if (uvCmn_FindFile(m_tzFile))
	{
		/* 읽기 전용 파일 무조건 삭제하기 위함 */
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

	/* 측정 시작 위치 정보 저장 */
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
	/* 측정하고자 하는 데이터의 간격 (행과 열) 저장 */
	wmemset(tzValue, 0x00, 32);	swprintf_s(tzValue, 32, L"%.4f\n", m_pstCali->period_row_size);
	fputws(tzValue, m_fpCali);
	wmemset(tzValue, 0x00, 32);	swprintf_s(tzValue, 32, L"%.4f\n", m_pstCali->period_col_size);
	fputws(tzValue, m_fpCali);
#endif
	/* 측정하고자 하는 데이터의 개수 (행과 열) 저장 */
	wmemset(tzValue, 0x00, 32);	swprintf_s(tzValue, 32, L"%u\n", m_pstCali->set_rows_count);
	fputws(tzValue, m_fpCali);
	wmemset(tzValue, 0x00, 32);	swprintf_s(tzValue, 32, L"%u\n", m_pstCali->set_cols_count);
	fputws(tzValue, m_fpCali);

	return TRUE;
}

