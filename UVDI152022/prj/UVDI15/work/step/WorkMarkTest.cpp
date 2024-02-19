
/*
 desc : Align (Both Global and Local Mark) Mark ���� (�ν�)
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkMarkTest.h"


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
CWorkMarkTest::CWorkMarkTest(LPG_CELA expo)
	: CWorkStep()
{
	m_enWorkJobID	= ENG_BWOK::en_mark_test;
	m_u32ExpoCount = 0;

	m_stExpoLog.Init();
	memcpy(&m_stExpoLog, expo, sizeof(STG_CELA));
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CWorkMarkTest::~CWorkMarkTest()
{
}

/*
 desc : �ʱ� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkMarkTest::InitWork()
{
	/* ���� ��� ���� �� �ʱ�ȭ */
	if (!CWork::InitWork())	return FALSE;

	/* ��ü �۾� �ܰ� */
#if 1
	//m_u8StepTotal	= 0x23;
	m_u8StepTotal = 0x27;
#else
	m_u8StepTotal	= 0x0e;
#endif
	/* �� �˻� ����� ��ũ ���� ��� */
	m_u8MarkCount	= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global) +
					  uvEng_Luria_GetMarkCount(ENG_AMTF::en_local);
	return TRUE;
}

/*
 desc : �ֱ������� �۾� ����
 parm : None
 retn : None
*/
VOID CWorkMarkTest::DoWork()
{
	/* �۾� �ܰ� ���� ���� ó�� */
	switch (m_u8StepIt)
	{
	case 0x01 : m_enWorkState = SetExposeReady(TRUE, TRUE, TRUE, 1);		break;	/* �뱤 ������ �������� ���� Ȯ�� */
	case 0x02 : m_enWorkState = IsLoadedGerberCheck();						break;	/* �Ź��� ����Ǿ���, Mark�� �����ϴ��� Ȯ�� */
	case 0x03 : m_enWorkState = SetTrigEnable(FALSE);						break;	/* Trigger Event - ��Ȱ��ȭ ���� */
	case 0x04 : m_enWorkState = IsTrigEnabled(FALSE);						break;	/* Trigger Event - ��Ȱ��ȭ Ȯ��  */
	case 0x05 : m_enWorkState = SetAlignMovingInit();						break;	/* Stage X/Y, Camera 1/2 - Align (Global) ���� ��ġ�� �̵� */
	case 0x06 : m_enWorkState = SetTrigPosCalcSaved();						break;	/* Trigger �߻� ��ġ ��� �� �ӽ� ���� */

	case 0x07: m_enWorkState = IsAlignMovedInit();							break;	/* Stage X/Y, Camera 1/2 - Align (Global) ���� ��ġ ���� ���� */

	case 0x08 : m_enWorkState = SetTrigRegistGlobal();						break;	/* Trigger �߻� ��ġ - Ʈ���� ���忡 Global Mark ��ġ ��� */
	case 0x09 : m_enWorkState = IsTrigRegistGlobal();						break;	/* Trigger �߻� ��ġ ��� Ȯ�� */

	case 0x0a : m_enWorkState = SetAlignMeasMode();							break;
	case 0x0b : m_enWorkState = IsAlignMeasMode();							break;

	case 0x0c : m_enWorkState = SetAlignMovingGlobal();						break;	/* Global Mark 4 ���� ��ġ Ȯ�� */
	case 0x0d : m_enWorkState = IsAlignMovedGlobal();						break;	/* Global Mark 4 ���� ���� �Ϸ� ���� */

	case 0x0e: m_enWorkState = CameraSetCamMode(ENG_VCCM::en_none);			break;	/* Cam None ���� ���� */
	case 0x0f : m_enWorkState = SetAlignMovingLocal(0x00, 0x00);			break;	/* Stage X/Y, Camera 1/2 - Align (Local:������) ���� ��ġ�� �̵� */
	case 0x10 : m_enWorkState = SetTrigRegistLocal(0x00);					break;	/* Trigger (������) �߻� ��ġ - Ʈ���� ���忡 Local Mark ��ġ ��� */
	case 0x11 : m_enWorkState = IsTrigRegistLocal(0x00);					break;	/* Trigger (������) �߻� ��ġ ��� Ȯ�� */
	case 0x12 : m_enWorkState = IsAlignMovedLocal(0x00, 0x00);				break;	/* Stage X/Y, Camera 1/2 - Align (Local) ���� ��ġ ���� ���� */
	case 0x13: m_enWorkState = CameraSetCamMode(ENG_VCCM::en_grab_mode);	break;	/* Cam None ���� ���� */
	case 0x14 : m_enWorkState = SetAlignMovingLocal(0x01, 0x00);			break;	/* Local Mark (������) 16 ���� ��ġ Ȯ�� */
	case 0x15 : m_enWorkState = IsAlignMovedLocal(0x01, 0x00);				break;	/* Local Mark (������) 16 ���� ���� �Ϸ� ���� */

	case 0x16: m_enWorkState = CameraSetCamMode(ENG_VCCM::en_none);			break;	/* Cam None ���� ���� */
	case 0x17 : m_enWorkState = SetAlignMovingLocal(0x00, 0x01);			break;	/* Stage X/Y, Camera 1/2 - Align (Local:������) ���� ��ġ�� �̵� */
	case 0x18 : m_enWorkState = SetTrigRegistLocal(0x01);					break;	/* Trigger (������) �߻� ��ġ - Ʈ���� ���忡 Local Mark ��ġ ��� */
	case 0x19 : m_enWorkState = IsTrigRegistLocal(0x01);					break;	/* Trigger (������) �߻� ��ġ ��� Ȯ�� */
	case 0x1a : m_enWorkState = IsAlignMovedLocal(0x00, 0x01);				break;	/* Trigger (������) �߻� ��ġ ��� Ȯ�� */
	case 0x1b: m_enWorkState = CameraSetCamMode(ENG_VCCM::en_grab_mode);	break;	/* Cam None ���� ���� */
	case 0x1c : m_enWorkState = SetAlignMovingLocal(0x01, 0x01);			break;	/* Local Mark (������) 16 ���� ��ġ Ȯ�� */
	case 0x1d : m_enWorkState = IsAlignMovedLocal(0x01, 0x01);				break;	/* Local Mark (������) 16 ���� ���� �Ϸ� ���� */

	case 0x1e : m_enWorkState = SetTrigEnable(FALSE);						break;
	case 0x1f : m_enWorkState = IsGrabbedImageCount(m_u8MarkCount,3000);	break;
	case 0x20 : m_enWorkState = IsSetMarkValidAll(0x01);					break;
	case 0x21 : m_enWorkState = SetAlignMarkRegist();						break;
	case 0x22 : m_enWorkState = IsAlignMarkRegist();						break;
	case 0x23 : m_enWorkState = IsTrigEnabled(FALSE);						break;
	case 0x24 : m_enWorkState = SetWorkWaitTime(2000);						break;
	case 0x25 : m_enWorkState = IsWorkWaitTime();							break;
	case 0x26 : m_enWorkState = SetMovingUnloader();						break;
	case 0x27 : m_enWorkState = IsMovedUnloader();							break;
		
	case 0x28: m_enWorkState = SetHomingACamSide();							break;
	case 0x29: m_enWorkState = IsHomedACamSide();							break;
		//case 0x26: m_enWorkState = DoDriveHoming(ENG_MMDI::en_stage_y);			break;
	case 0x2a: m_enWorkState = SetWorkWaitTime(10000);						break;
	case 0x2b: m_enWorkState = IsWorkWaitTime();							break;
		//case 0x29: m_enWorkState = IsDrivedHomed(ENG_MMDI::en_stage_y);			break;
	}

	/* ���� �۾� ���� ���� �Ǵ� */
	CWorkMarkTest::SetWorkNext();
	/* ��ð� ���� ���� ��ġ�� �ݺ� �����Ѵٸ� ���� ó�� */
	CheckWorkTimeout();
}

/*
 desc : ���� �ܰ�� �̵��ϱ� ���� ó��
 parm : None
 retn : None
*/
VOID CWorkMarkTest::SetWorkNext()
{
	UINT8 u8WorkTotal	= IsMarkTypeOnlyGlobal() ? (m_u8StepTotal - 12) : m_u8StepTotal;
	UINT64 u64JobTime	= GetTickCount64() - m_u64StartTime;

	/* �� �۾� �������� �ð� �� ���� ó�� */
	uvEng_UpdateJobWorkTime(u64JobTime);

	/* ��� �۾��� ���� �Ǿ����� ���� */
	if (ENG_JWNS::en_error == m_enWorkState)
	{
		TCHAR tzMesg[128] = {NULL};
		swprintf_s(tzMesg, 128, L"Align Test <Error Step It = 0x%02x>", m_u8StepIt);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
		
		SaveExpoResult(0x00);
		//m_u8StepIt = 0x26;
		m_u8StepIt = 0x01;

		//m_enWorkState = ENG_JWNS::en_comp;
		m_enWorkState = ENG_JWNS::en_error;
		//m_enWorkState = ENG_JWNS::en_next;
	}
	else if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* �۾��� ��� �� �ӽ� ���� */
		CWork::CalcStepRate();
		/* ���� Global Mark���� �ν� �ߴ��� ���� */
		//if (m_u8StepIt == 0x0d)
		if (m_u8StepIt == 0x0d)
		{
#if 1
			/* ���� ���� ��尡 Global ������� Local ���� ������� ���ο� ���� �ٸ� */
			//if (IsMarkTypeOnlyGlobal())	m_u8StepIt	= 0x1a;
			if (IsMarkTypeOnlyGlobal())	m_u8StepIt	= 0x1e;

#else
			/* ���� ���� ��尡 Global ������� Local ���� ������� ���ο� ���� �ٸ� */
			m_u8WorkStep	= 0x1f;
#endif
#if 0	/* CWork::SetAlignMarkRegist() ���⼭ ó���ϰ� ���� */
			uvData_Mark_SetMarkLenActive();
#endif
		}
		if (m_u8StepTotal == m_u8StepIt)
		{
			/* �۾� �Ϸ� �� ���� �ʿ��� ���� ���� */
			SaveExpoResult(0x01);

			//if (++m_u32ExpoCount < 11)
			//if (++m_u32ExpoCount < 2)
			if (++m_u32ExpoCount != m_stExpoLog.expo_count)
			{
				m_u8StepIt = 0x01;
			}
			else
			{
				m_enWorkState = ENG_JWNS::en_comp;
				/* �׻� ȣ��*/
				CWork::EndWork();
			}

		}
		else
		{
			/* ���� �۾� �ܰ�� �̵� */
			m_u8StepIt++;
		}
		/* ���� �ֱٿ� Waiting �� �ð� ���� */
		m_u64DelayTime	= GetTickCount64();
	}
}



/*
 desc : ����� ī�޶� 2�븦 ��/�� ������ �̵� ��Ŵ (��� ���� �������� ����)
 parm : None
 retn : TRUE or FALSE
*/
ENG_JWNS CWorkMarkTest::SetHomingACamSide()
{
	LPG_CMSI pstMC2 = &uvEng_GetConfig()->mc2_svc;

	/* ���� ��� ���� �� ��� */
	//uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
	//uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam2);
	/* ¦���� ��, �ּ� �� (���� ��)���� �̵� */
	//if (!m_bMoveACamSide)
	//{
		/* 1�� ī�޶���� home */
		if(!uvEng_MC2_SendDevHoming(ENG_MMDI::en_align_cam1))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to home the align camera (1) to (left)");
			return  ENG_JWNS::en_wait;
		}
		/* 2�� ī�޶���� home */
		if (!uvEng_MC2_SendDevHoming(ENG_MMDI::en_align_cam2))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to home the align camera (2) to (left)");
			return  ENG_JWNS::en_wait;
		}
	//}
	///* Ȧ���� ��, �ִ� �� (���� ��)���� �̵� */
	//else
	//{
	//	/* 2�� ī�޶���� home */
	//	if (!uvEng_MC2_SendDevHoming(ENG_MMDI::en_align_cam2))
	//	{
	//		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to home the align camera (1) to (right)");
	//		return FALSE;
	//	}
	//	/* 1�� ī�޶���� home */
	//	if (!uvEng_MC2_SendDevHoming(ENG_MMDI::en_align_cam1))
	//	{
	//		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to home the align camera (2) to (right)");
	//		return FALSE;
	//	}
	//}

	return	ENG_JWNS::en_next;
}

/*
 desc : 2�븦 ��/�� ������ �̵��� �Ϸ� �Ǿ����� Ȯ�� (��� ���� �������� ����)
 parm : None
 retn : TRUE or FALSE
*/
ENG_JWNS CWorkMarkTest::IsHomedACamSide()
{
	/* �ּ� 1 �и��� �����ϵ��� ���� �� */
//	if (GetTickCount64() < (m_u64TickACamSide + 60000) /* 1 minutue */)	return FALSE;

	/* ��� �̵� �ߴ��� ���� Ȯ�� */
	if (!uvCmn_MC2_IsDriveHomed(ENG_MMDI::en_align_cam1))	return  ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDriveHomed(ENG_MMDI::en_align_cam2))	return  ENG_JWNS::en_wait;

	/* �ݴ������� �̵� */
	//m_bMoveACamSide = !m_bMoveACamSide;

	/* �ֱ� ������ �ð� ���� */
//	m_u64TickACamSide = GetTickCount64();

	return	ENG_JWNS::en_next;
}

/*
 desc : �뱤 �Ϸ� �� ���� ���� ����
 parm : state	- [in]  0x00: �۾� ����, 0x01: �۾� ����
 retn : None
*/
VOID CWorkMarkTest::SaveExpoResult(UINT8 state)
{
	TCHAR tzResult[1024] = { NULL }, tzFile[MAX_PATH_LEN] = { NULL }, tzState[2][8] = { L"FAIL", L"SUCC" };
	LPG_ACGR pstMark = NULL;
	SYSTEMTIME stTm = { NULL };
	MEMORYSTATUSEX stMem = { NULL };

	uvEng_Luria_ReqGetPhLedTempAll();
	UINT16(*pLed)[8] = uvEng_ShMem_GetLuria()->directph.light_source_driver_temp_led;
	UINT16(*pBoard)[8] = uvEng_ShMem_GetLuria()->directph.light_source_driver_temp_board;

	/* ���� ��ǻ�� ��¥�� ���ϸ����� ���� */
	GetLocalTime(&stTm);
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\logs\\expo\\%04d-%02d-%02d MarkText.csv",
		g_tzWorkDir, stTm.wYear, stTm.wMonth, stTm.wDay);

	/* ���࿡ ������ �ϴ� ������ ������, �ش� ���� ���� ��, Ÿ��Ʋ (Field) �߰� */
	if (!uvCmn_FindFile(tzFile))
	{
		wcscpy_s(tzResult, 1024,
			L"Count,Time,succ,"
			L"score_1,scale_1,mark_move_x1(mm),mark_move_y1(mm),"
			L"score_2,scale_2,mark_move_x2(mm),mark_move_y2(mm),"
			L"score_3,scale_3,mark_move_x3(mm),mark_move_y3(mm),"
			L"score_4,scale_4,mark_move_x4(mm),mark_move_y4(mm),"
			//L"led(1:1),led(1:2),led(1:3),led(1:4),board(1:1),board(1:2),board(1:3),board(1:4),"
			//L"led(2:1),led(2:2),led(2:3),led(2:4),board(2:1),board(2:2),board(2:3),board(2:4),"
			//L"led(3:1),led(3:2),led(3:3),led(3:4),board(3:1),board(3:2),board(3:3),board(3:4),"
			//L"led(4:1),led(4:2),led(4:3),led(4:4),board(4:1),board(4:2),board(4:3),board(4:4),"
			//L"led(5:1),led(5:2),led(5:3),led(5:4),board(5:1),board(5:2),board(5:3),board(5:4),"
			//L"led(6:1),led(6:2),led(6:3),led(6:4),board(6:1),board(6:2),board(6:3),board(6:4),\n");
			L"horz_dist_13(mm),horz_dist_24(mm),vert_dist_12(mm),vert_dist_34(mm),"
			L"diag_dist_14(mm),diag_dist_23(mm),\n");
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x00);
	}

	swprintf_s(tzResult, 1024, L"Count=%d, %02d:%02d:%02d,%s,",
		m_u32ExpoCount, stTm.wHour, stTm.wMinute, stTm.wSecond, tzState[state]);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);

	/* ����� ��ũ �˻� ��� �� ���� */
	pstMark = uvEng_Camera_GetGrabbedMark(0x01, 0x00);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%.4f,%.4f,",
			pstMark->score_rate, pstMark->scale_rate,
			pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	pstMark = uvEng_Camera_GetGrabbedMark(0x01, 0x01);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%.4f,%.4f,",
			pstMark->score_rate, pstMark->scale_rate,
			pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	pstMark = uvEng_Camera_GetGrabbedMark(0x02, 0x00);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%.4f,%.4f,",
			pstMark->score_rate, pstMark->scale_rate,
			pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	pstMark = uvEng_Camera_GetGrabbedMark(0x02, 0x01);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%.4f,%.4f,",
			pstMark->score_rate, pstMark->scale_rate,
			pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}

	/* ���а� LED / Board �µ� �� ���� */
	//for (int i = 0x00; i < 0x06; i++)
	//{
	//	swprintf_s(tzResult, 1024, L"%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,",
	//		pLed[i][0] / 10.0f, pLed[i][1] / 10.0f, pLed[i][2] / 10.0f, pLed[i][3] / 10.0f,
	//		pBoard[i][0] / 2.0f, pBoard[i][1] / 2.0f, pBoard[i][2] / 2.0f, pBoard[i][3] / 2.0f);
	//	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	//}
		/* ��ũ ���� 6 �� ���� ���� ���� �� �� ��ü �뱤�ϴµ� �ҿ�� �ð� ���� */
	LPG_GMLV pstMarkDiff = &uvEng_GetConfig()->mark_diff;
	swprintf_s(tzResult, 1024, L"%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,",
		pstMarkDiff->result[0].diff * 100.0f, pstMarkDiff->result[1].diff * 100.0f, pstMarkDiff->result[2].diff * 100.0f,
		pstMarkDiff->result[3].diff * 100.0f, pstMarkDiff->result[4].diff * 100.0f, pstMarkDiff->result[5].diff * 100.0f);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);

	/* �������� ������ ���� �������� �Ѿ���� �ϱ� ���� */
	uvCmn_SaveTxtFileW(L"\n", (UINT32)wcslen(L"\n"), tzFile, 0x01);
}