
/*
 desc : Align Camera 2D Calibration �⺻ ��ü
*/

#include "pch.h"
#include "../MainApp.h"
#include "Work.h"


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
CWork::CWork()
{
	/* ���� �ʱ�ȭ (��ü ��� ����) */
	wmemset(m_tzFile, 0x00, MAX_PATH_LEN);
	m_fpCali		= NULL;
	/* Grabbed Image ũ�� ��� (����: bytes) */
	m_u32GrabSize	= uvEng_GetConfig()->set_cams.ioi_size[0] *
					  uvEng_GetConfig()->set_cams.ioi_size[1];
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CWork::~CWork()
{
	/* Ȥ�� ������ ������ ������ �ݱ� */
	CloseCaliFile();
}

/*
 desc : ���� ������ ���� �ݱ�
 parm : None
 retn : None
*/
VOID CWork::CloseCaliFile()
{
	if (m_fpCali)	fclose(m_fpCali);
	m_fpCali	= NULL;
}

/*
 desc : �ʱ� �۾� ����
 parm : cam_id	- [in]  ���� ���� ī�޶� �ε��� (1 or 2)
 retn : None
*/
VOID CWork::InitWork(UINT8 cam_id)
{
	m_pstCali		= &uvEng_GetConfig()->acam_cali;
	/* ���� ��� ī�޶� ID */
	m_u8ACamID		= cam_id;
	/* ��ü �۾� �ܰ� */
	m_u8WorkTotal	= 0x0c;
	m_u8StepIt		= 0x01;
	m_u8TrigRetry	= 0x00;				/* ��ũ �ν� Ƚ�� �ʱ�ȭ */
	m_u16MoveRows	= 0;
	m_u16MoveCols	= 0;
	m_u64StartTime	= GetTickCount64();
	m_u64WorkTime	= GetTickCount64();
	m_enWorkState	= ENG_JWNS::en_none;

	/* Align Camera is Calibration Mode */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);
	/* Trigger & Strobe - Enable ó�� */
	uvEng_Trig_ReqTriggerStrobe(TRUE);
}

/*
 desc : ���� �ð����� ��� �ߴ��� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsWorkWaitTime()
{
#ifdef _DEBUG
	return	GetTickCount64() > (m_u64WaitTime + 60000);	/* 60 �� �̻� �����Ǹ� */
#else
	return	GetTickCount64() > (m_u64WaitTime + 20000);	/* 20 �� �̻� �����Ǹ� */
#endif
}

/*
 desc : ���� �ð� ���� �۾��� ����ϱ� ����
 parm : time	- [in]  �ּ����� ��⿡ �ʿ��� �ð� (����: msec)
 retn : None
*/
VOID CWork::SetWorkWaitTime(UINT64 time)
{
	m_u64WaitTime	= GetTickCount64() + time;
}

/*
 desc : Check if the wait time for the work has been exceeded
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsWorkTimeout()
{
#ifdef _DEBUG
	return GetTickCount64() > (m_u64WorkTime + 30 * 1000/*30 sec*/);
#else
	return GetTickCount64() > (m_u64WorkTime + 10 * 1000/*10 sec*/);
#endif
}

/*
 desc : ���� ��ġ�� �̵�
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::SetMovingInit()
{
	UINT8 u8StartLed	= ((LPG_PMRW)uvEng_ShMem_GetPLC()->data)->r_start_button;
	DOUBLE dbPosStage[2], dbPosACamX[2], dbACam1PosX;
	DOUBLE dbVeloStage[2], dbVeloACam[2];

	/* �� Axis ���� �⺻ ���� �ӵ� �� ��� */
	dbVeloStage[0]	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_x];
	dbVeloStage[1]	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_y];
	dbVeloACam[0]	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_align_cam1];
	dbVeloACam[1]	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_align_cam2];

	/* ���� �̵��ϰ��� �ϴ� ��� ��ġ �� ��� */
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
	dbPosStage[0]	= m_pstCali->mark_stage_x;
#else
	dbPosStage[0]	= m_pstCali->mark_stage_x[m_u8ACamID-1];
#endif
	dbPosStage[1]	= m_pstCali->mark_stage_y[m_u8ACamID-1];
	if (0x01 == m_u8ACamID)
	{
		dbPosACamX[0]	= m_pstCali->mark_acam[0];
		dbPosACamX[1]	= uvEng_GetConfig()->mc2_svc.max_dist[(UINT8)ENG_MMDI::en_align_cam2];
	}
	else
	{
		dbPosACamX[0]	= uvEng_GetConfig()->mc2_svc.min_dist[(UINT8)ENG_MMDI::en_align_cam1];
		dbPosACamX[1]	= m_pstCali->mark_acam[1];
	}
#if 0
	/* Vacuum�� Shutter�� ���� ���� ���� */
	if (!uvEng_MCQ_LedVacuumShutter(u8StartLed, 0x01, 0x01))	return ENG_JWNS::en_error;
#endif
	/* ��� Motor Drive�� ��� �� ���� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam2);

	/* ���� ī�޶� 1 ���� ���� ��ġ ���� ���, �̵��ϰ��� �ϴ� ���� ��ġ ���� ū ������ Ȥ�� */
	/* ���� ������ ���ο� ����, Align Camera 1 Ȥ�� 2 �� �� ��� ���� ���� �̵��ؾ� ���� ����*/
	dbACam1PosX	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1);
	/* Align Camera 1 or 2���� ������ �� (���� (1) or (2) ������)���� �̵� */
	if (dbPosACamX[0] > dbACam1PosX)
	{
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, dbPosACamX[1], dbVeloACam[1]))	return ENG_JWNS::en_error;
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, dbPosACamX[0], dbVeloACam[0]))	return ENG_JWNS::en_error;
	}
	else
	{
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, dbPosACamX[0], dbVeloACam[0]))	return ENG_JWNS::en_error;
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, dbPosACamX[1], dbVeloACam[1]))	return ENG_JWNS::en_error;
	}
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dbPosStage[0], dbVeloStage[0]))		return ENG_JWNS::en_error;
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbPosStage[1], dbVeloStage[1]))		return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark�� �����ϴ� ���� ��ġ�� �̵��ߴ��� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsMovedInited()
{
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_x))		return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))		return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1))	return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam2))	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : ���� ����� �� ��ȯ
 parm : None
 retn : ����� ��
*/
DOUBLE CWork::GetProcRate()
{
	DOUBLE dbRate	= 0.0f;

	/* ������� �۾� ����� ���� */
	dbRate	= m_u8WorkTotal * (UINT32(m_u16MoveRows) * UINT32(m_pstCali->set_cols_count) + m_u16MoveCols);
	dbRate	+= m_u8StepIt + 1;

	return (dbRate / m_dbTotalRate) * 100.0f;
}