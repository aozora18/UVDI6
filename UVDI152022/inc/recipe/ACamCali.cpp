
/*
 desc : Align Camera�� Calibration Data ���� �� ����
*/

#include "pch.h"
#include "ACamCali.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : work_dir	- [in]  ���� ������ ����Ǵ� ��� (��ü ���, '\' ����)
		shmem		- [in]  Vision ���� �޸�
 retn : None
*/
CACamCali::CACamCali(PTCHAR work_dir, LPG_VDSM shmem)
	: CBase(work_dir)
{
	m_pstShMem		= shmem;
	/* �޸� �ʱ�ȭ */
	m_pstCali		= NULL;
	m_u8MarkCount	= 0x00;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CACamCali::~CACamCali()
{
	UINT8 i	= 0;
	RemoveAlignCaliData();
}

/*
 desc : Align Cali Data �޸� ����
 parm : None
 retn : None
*/
VOID CACamCali::RemoveAlignCaliData()
{
	UINT8 i	= 0x00;

	if (m_pstCali)
	{
		for (; i<GetConfig()->set_cams.acam_count; i++)
		{
			if (m_pstCali[i].pos_xy)	::Free(m_pstCali[i].pos_xy);
		}
		::Free(m_pstCali);
		m_pstCali	= NULL;
	}
}

/*
 desc : ���� �۾� ���� ��ϵǾ� �ִ� Mark X/Y ��ġ �� Mark ���� ���� ��� ����
 parm : None
 retn : None
*/
VOID CACamCali::ResetAllCaliData()
{
	UINT32 u32Size, u32Temp;
	UINT32 u32ACamCount	= GetConfig()->set_cams.acam_count;

	/* ��ϵ� Mark Position�� ������ ���� */
	m_u8MarkCount	= 0x00;
	/* Global Mark ���� �ʱ�ȭ */
	u32Temp	= MAX_GLOBAL_MARKS / u32ACamCount;
	u32Temp	+= (MAX_GLOBAL_MARKS % u32ACamCount) > 0 ? 0x01 : 0x00;
	u32Size	= sizeof(STG_ACCE) * u32Temp * u32ACamCount;
#if 0	/* �Ʒ��� ���� �� ���, �޸� ������ ��ü�� ����� */
	memset(m_pstShMem->cali_global, 0x00, u32Size);
#else
	memset(m_pstShMem->cali_global[0][0], 0x00, u32Size);
#endif
	/* Local Mark ���� �ʱ�ȭ */
	u32Temp	= MAX_LOCAL_MARKS / u32ACamCount;
	u32Temp	+= (MAX_LOCAL_MARKS % u32ACamCount) > 0 ? 0x01 : 0x00;
	u32Size	= sizeof(STG_ACCE) * u32Temp * u32ACamCount;
#if 0	/* �Ʒ��� ���� �� ���, �޸� ������ ��ü�� ����� */
	memset(m_pstShMem->cali_local, 0x00, u32Size);
#else
	memset(m_pstShMem->cali_local[0][0], 0x00, u32Size);
#endif
}

/*
 desc : �뱤 �ӵ��� ���� Calibration Data�� �����ϴ��� Ȯ��
 parm : cali_thick	- [in]  ������ �뱤 ���� �β� �� (����: um) (100 ~ 99999 um)
 retn : TRUE or FALSE
*/
BOOL CACamCali::IsCalibrationData(UINT16 cali_thick)
{
	UINT8 i	= 0;
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	CUniToChar csCnv;

	for (; i<GetConfig()->set_cams.acam_count; i++)
	{
		/* ȯ�� ���� */
		swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\xy2d\\%s_%04u_%d.dat",
				   m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.acam_cali, cali_thick, i+1);
		if (0 != _access(csCnv.Uni2Ansi(tzFile), 0))	return FALSE;
	}
	return TRUE;
}

/*
 desc : Calibration Align Camera File ����
 parm : cali_thick	- [in]  2D ������ �뱤 ���� �β� �� (����: um) (100 ~ 99999 um)
 retn : None
*/
BOOL CACamCali::LoadFile(UINT16 cali_thick)
{
	TCHAR *ptzVal, tzVal[64], tzFile[MAX_PATH_LEN] = {NULL}, *ptzContext;
	INT32 i, j, i32Total, i32Len;
	errno_t eRet		= {NULL};
	LPM_ACCP pstPosXY	= NULL;
	FILE *fpCali		= {NULL};
	CUniToChar csCnv;

	/* ���� ����� ������ ���� */
	RemoveAlignCaliData();

	/* �뱤 �ӵ��� ���� Calibration Data�� �����ϴ��� Ȯ�� */
	if (!IsCalibrationData(cali_thick))
	{
		swprintf_s(m_tzErrMsg, LOG_MESG_SIZE,
				   L"The calibration file corresponding to calibration_material_thick (%u) does not exist",
				   cali_thick);
		return FALSE;
	}

	/* Align Calibration XY ������ ���� ���� */
	m_pstCali	= (LPM_ACCI)::Alloc(UINT32(GetConfig()->set_cams.acam_count) * sizeof(STM_ACCI));
	ASSERT(m_pstCali);
	memset(m_pstCali, 0x00, UINT32(GetConfig()->set_cams.acam_count) * sizeof(STM_ACCI));
	m_pstCali->pos_xy	= NULL;	/* �ݵ�� �ʱ�ȭ */

	/* Align Camera 1 & 2�� ��߰� ���� ��ġ ��� */
	for (i=0; i<GetConfig()->set_cams.acam_count; i++)
	{
		/* ȯ�� ���� */
		swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\xy2d\\%s_%04u_%d.dat",
				   m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.acam_cali, cali_thick, i+1);
		/* Open the file */
		eRet = fopen_s(&fpCali, csCnv.Uni2Ansi(tzFile), "rt");
		if (0 != eRet)
		{
			swprintf_s(m_tzErrMsg, LOG_MESG_SIZE,
					   L"Failed to open the file (calibration_material_thick:%u)", cali_thick);
			if (0 == eRet)	fclose(fpCali);
			Free(m_pstCali);
			m_pstCali	= NULL;
			RemoveAlignCaliData();
			return FALSE;
		}

		/* Align Camera X : Motion Position */
		wmemset(tzVal, 0x00, 64);	fgetws(tzVal, 32, fpCali);
		m_pstCali[i].acam_x	= (INT32)ROUNDED(_wtof(tzVal) * 10000.0f, 0);
		/* Stage X : Motion Position */
		wmemset(tzVal, 0x00, 64);	fgetws(tzVal, 32, fpCali);
		m_pstCali[i].stage_x= (INT32)ROUNDED(_wtof(tzVal) * 10000.0f, 0);
		/* Stage Y : Motion Position */
		wmemset(tzVal, 0x00, 64);	fgetws(tzVal, 32, fpCali);
		m_pstCali[i].stage_y= (INT32)ROUNDED(_wtof(tzVal) * 10000.0f, 0);
#if (0x10 == (0x10 & USE_ALIGN_CAMERA_2D_CALI_VER))
		/* ��� ���� ���� ���� */
		wmemset(tzVal, 0x00, 64);	fgetws(tzVal, 32, fpCali);
		m_pstCali[i].row_gap= (INT32)ROUNDED(_wtof(tzVal) * 10000.0f, 0);
		wmemset(tzVal, 0x00, 64);	fgetws(tzVal, 32, fpCali);
		m_pstCali[i].col_gap= (INT32)ROUNDED(_wtof(tzVal) * 10000.0f, 0);
#endif
		/* ��� ���� ���� ���� */
		wmemset(tzVal, 0x00, 64);	fgetws(tzVal, 32, fpCali);
		m_pstCali[i].row_cnt= (UINT32)_wtoi(tzVal);
		wmemset(tzVal, 0x00, 64);	fgetws(tzVal, 32, fpCali);
		m_pstCali[i].col_cnt= (UINT32)_wtof(tzVal);

		/* �޸� �Ҵ� */
		i32Total			= UINT32(m_pstCali->col_cnt) * UINT32(m_pstCali->row_cnt);
		m_pstCali[i].pos_xy	= (LPM_ACCP)::Alloc(i32Total * sizeof(STM_ACCP));
		ASSERT(m_pstCali[i].pos_xy);
		memset(m_pstCali[i].pos_xy, 0x00, sizeof(STM_ACCP) * i32Total);
		/* ���� ���� ����� ����ü ������ ���� */
		pstPosXY	= m_pstCali[i].pos_xy;
		/* �� ���� ī�޶��� Calibration ��ġ �� ��� */
		for (j=0; j<i32Total; j++)
		{
			wmemset(tzVal, 0x00, 64);
			/* ���پ� �б� (�о���̰��� �ϴ� ������ �� + 1 (/n; ���๮�� ������) */
			if (!fgetws(tzVal, 64, fpCali))	break;
			/* �ƹ��� �����Ͱ� �������� �ʴ´��� Ȯ�� */
			i32Len	= (INT32)wcslen(tzVal);
#if (0x10 == (0x10 & USE_ALIGN_CAMERA_2D_CALI_VER))
			/*if (i32Len != 15)	break;*/
			/* Align Camera X, Stage Y ���� �� ���� */
			ptzVal	= wcstok_s(tzVal, L",", &ptzContext);
			pstPosXY[j].error_x	= (INT32)ROUNDED(_wtof(ptzVal) * 10000.0f, 0);
			ptzVal	= wcstok_s(NULL, L",", &ptzContext);
			pstPosXY[j].error_y	= (INT32)ROUNDED(_wtof(ptzVal) * 10000.0f, 0);
			/* ���������� ���ۿ� �� �����ϱ� */
			if (0 != (j % m_pstCali[i].col_cnt))
			{
				pstPosXY[j].a_cam_x	= pstPosXY[j-1].a_cam_x + m_pstCali[i].col_gap;
				pstPosXY[j].stage_y	= pstPosXY[j-1].stage_y;
			}
			else
			{
				pstPosXY[j].a_cam_x	= m_pstCali[i].acam_x;
				pstPosXY[j].stage_y	= m_pstCali[i].stage_y + m_pstCali[i].row_gap * (j / m_pstCali[i].col_cnt);
			}
#elif (0x20 == (0x20 & USE_ALIGN_CAMERA_2D_CALI_VER))
			if (i32Len != 41/*included the '\r\n'*/)	break;
			/* Get the measurement location of align_camera_x and stage_y */
			ptzVal	= wcstok_s(tzVal, L",", &ptzContext);
			pstPosXY[j].a_cam_x	= (INT32)ROUNDED(_wtof(ptzVal) * 10000.0f, 0);	/* 0.1 um or 100 nm */
			ptzVal	= wcstok_s(NULL, L",", &ptzContext);
			pstPosXY[j].stage_y	= (INT32)ROUNDED(_wtof(ptzVal) * 10000.0f, 0);	/* 0.1 um or 100 nm */
			/* Get the step_xy of align_camera */
			ptzVal	= wcstok_s(NULL, L",", &ptzContext);
			pstPosXY[j].error_x	= (INT32)ROUNDED(_wtof(ptzVal) * 10000.0f, 0);	/* 0.1 um or 100 nm */
			ptzVal	= wcstok_s(NULL, L",", &ptzContext);
			pstPosXY[j].error_y	= (INT32)ROUNDED(_wtof(ptzVal) * 10000.0f, 0);	/* 0.1 um or 100 nm */
#else
#endif
			/* ������ ������ �о�鿴���� */
			if (feof(fpCali))	break;
		}
		/* ���� �ݱ� */
		fclose(fpCali);
	}

	/* �˻��� ������ ��� ��ġ�ϴ��� Ȯ�� */
	if (j != i32Total)
	{
		RemoveAlignCaliData();
		swprintf_s(m_tzErrMsg, LOG_MESG_SIZE,
				   L"The measured number of data is not stored normally (calibration_material_thick:%u)",
				   cali_thick);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : Ʈ���� �߻� ��ġ�� �ش�Ǵ� Stage Y�� Align Camera X ��ǥ�� ����, 2D Calibration ������ ����
 parm : None
 retn : 0x00 (�˻� ����), 0x01 (�˻� ����), 0x02 (���� ������ ����)
*/
UINT8 CACamCali::SetTrigPosCaliApply()
{
	UINT8 i = 0x00, u8MarkSet = 0xff, u8MarkCnt = 0x00;
	UINT8 u8ACamCount	= GetConfig()->set_cams.acam_count;
	LPG_ACCE pstCaliData= NULL;

	/* ���� ��ϵ� 2D Calibration Data�� ���� ������� Ȯ�� */
	if (!m_pstCali || m_u8MarkCount < 0x01)	return 0x00;
	/* ���� ī�޶� ������ 1 Ȥ�� 2�� �ƴϸ� ���� ó�� T.T */
	if (u8ACamCount != 0x01 && u8ACamCount != 0x02)	return 0x00;

	/* ���� Align Camera�� 1���� ��� */
	if (0x03 == u8ACamCount)
	{
	 
	 	
	}
	else if (0x02 == u8ACamCount)
	{
		TCHAR tzMsg[256] = { NULL };
		/* ����� ī�޶� 2���� ��� */
		for (; i<m_u8MarkCount/ u8ACamCount; i++)
		{
			/* ����� ī�޶� 1 or 2�� */
			if (i < 0x02)	pstCaliData	= m_pstShMem->cali_global[0x00][i];
			else			pstCaliData	= m_pstShMem->cali_local[0x00][i-0x02];
			/* Ʈ���� �߻� ��ġ�� �ش�Ǵ� Calibration XY ���� �� ��� */
			u8MarkSet	= GetCaliPos(0x01,
									 pstCaliData->acam_motion_x, pstCaliData->stage_motion_y,
									 pstCaliData->acam_cali_x, pstCaliData->stage_cali_y);
			if (0x00 == u8MarkSet)	return FALSE;
			/* If it does not exist the calibration area, increase the number */
			if (0x02 == u8MarkSet)	u8MarkCnt++;
			swprintf_s(tzMsg, 256, L"2D Cail Cam1 %d�� acam_motion_x : %d stage_motion_y : %d acam_cali_x : %d stage_cali_y : %d",
				i + 1, pstCaliData->acam_motion_x, pstCaliData->stage_motion_y, pstCaliData->acam_cali_x, pstCaliData->stage_cali_y);
			LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);

			/* ����� ī�޶� 2�� */
			if (i < 0x02)	pstCaliData	= m_pstShMem->cali_global[0x01][i];
			else			pstCaliData	= m_pstShMem->cali_local[0x01][i-0x02];
			/* Ʈ���� �߻� ��ġ�� �ش�Ǵ� Calibration XY ���� �� ��� */
			u8MarkSet	= GetCaliPos(0x02,
									 pstCaliData->acam_motion_x, pstCaliData->stage_motion_y,
									 pstCaliData->acam_cali_x, pstCaliData->stage_cali_y);
			if (0x00 == u8MarkSet)	return FALSE;
			/* If it does not exist the calibration area, increase the number */
			if (0x02 == u8MarkSet)	u8MarkCnt++;
			swprintf_s(tzMsg, 256, L"2D Cail Cam2 %d�� acam_motion_x : %d stage_motion_y : %d acam_cali_x : %d stage_cali_y : %d",
				i + 1, pstCaliData->acam_motion_x, pstCaliData->stage_motion_y, pstCaliData->acam_cali_x, pstCaliData->stage_cali_y);
			LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
		}
	}

	return u8MarkCnt > 0x00 ? 0x02 : 0x01/*They all exist inside the calibration area*/;
}

/*
 desc : ���ϴ� ��ġ�� �ٻ��� Align Camera�� Calibration ���� �� ��ȯ
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
		a_cam_x	- [in]  �̵��ϰ��� �ϴ� Align Camera�� Drive Position (���� ��. ����: 0.1 um or 100 nm)
		stage_y	- [in]  �̵��ϰ��� �ϴ� Stage�� Drive Position (���� ��. ����: 0.1 um or 100 nm)
		error_x	- [out] Calibration ���� �� (X Axis ���� ��. ����: 100 nm or 0.1 um)
		error_y	- [out] Calibration ���� �� (Y Axis ���� ��. ����: 100 nm or 0.1 um)
 retn : 0x00 (�˻� ����), 0x01 (�˻� ����), 0x02 (���� ������ ����)
*/
UINT8 CACamCali::GetCaliPos(UINT8 cam_id, INT32 a_cam_x, INT32 stage_y,
							INT32 &error_x, INT32 &error_y)
{
	UINT8 u8MarkSet		= 0x01;	/* Check if there is an align mark in the calibration area */
	INT32 i32NearX		= -1, i32NearY = -1, i, j;
	INT32 i32CentX, i32CentY, i32LB, i32RB, i32RT, i32LT;
	DOUBLE dbRate		= 0.0f;
	LPM_ACCP pstPosXY	= NULL, pstPrev = NULL, pstNext = NULL;

	/* Calibration Data�� �����ϴ��� ���� */
	pstPosXY	= m_pstCali[cam_id-1].pos_xy;
	if (m_pstCali[cam_id-1].col_cnt < 1 || m_pstCali[cam_id-1].row_cnt < 1)
	{
		swprintf_s(m_tzErrMsg, LOG_MESG_SIZE,
				   L"Invalid number of rows or columns set [rows:%d][cols:%d]",
				   m_pstCali[cam_id-1].col_cnt, m_pstCali[cam_id-1].row_cnt);
		LOG_ERROR(ENG_EDIC::en_acam_cali, m_tzErrMsg);
		return 0x00;
	}

	/* �˻� ������ ���, Offset ���� ������ 0 ��ȯ */
	error_x	= 0;
	error_y	= 0;
	/* ---------------------------------------------------------------- */
	/* a_cam_x �� ������ Align Calibration ������ ���� �ȿ� �ִ��� Ȯ�� */
	/* ---------------------------------------------------------------- */
	if (pstPosXY[0].a_cam_x > a_cam_x)
	{
		swprintf_s(m_tzErrMsg, LOG_MESG_SIZE,
				   L"Invalid Align Camera X Position. set.a_cam_x (%d) > a_acam_x(%d)",
				   pstPosXY[0].a_cam_x, a_cam_x);
		LOG_WARN(ENG_EDIC::en_acam_cali, m_tzErrMsg);
		
		i32NearX	= 0;
		u8MarkSet	= 0x02;	/* Set the mark 1 does not exist in the calibration area */
	}
	else
	{
		/* ���� Align Camera�� �̵��� ��ġ �������� ���� �ٻ�ġ �� ���� */
		for (i=0; i<(INT32)m_pstCali[cam_id-1].col_cnt; i++)
		{
			if (pstPosXY[i].a_cam_x >= a_cam_x)
			{
				i32NearX	= i - 1;
				break;
			}
		}
	}
	/* ���� �˻��� Align Camera X ���� ��ġ�� ���� ���� ���� ���� ��� */
	if (i32NearX < 0)
	{
		swprintf_s(m_tzErrMsg, LOG_MESG_SIZE,
				   L"Align Calibration Out of range Last X. i32NearX (%d) < 0", i32NearX);
		LOG_WARN(ENG_EDIC::en_acam_cali, m_tzErrMsg);

		if (m_pstCali[cam_id-1].col_cnt > 1)	i32NearX	= m_pstCali[cam_id-1].col_cnt - 2;
		else									i32NearX	= m_pstCali[cam_id-1].col_cnt - 1;
		u8MarkSet	= 0x02;	/* Set the mark 1 does not exist in the calibration area */
	}
	/* ---------------------------------------------------------------- */
	/* stage_y �� ������ Align Calibration ������ ���� �ȿ� �ִ��� Ȯ�� */
	/* ---------------------------------------------------------------- */
	if (pstPosXY[0].stage_y > stage_y)
	{
		swprintf_s(m_tzErrMsg, LOG_MESG_SIZE,
				   L"Align Calibration Out of range First Y. pstPosXY[0].stage_y (%d) > stage_y(%d)",
				   pstPosXY[0].stage_y, stage_y);
		LOG_WARN(ENG_EDIC::en_acam_cali, m_tzErrMsg);
		/* Y ���� ���, X �� �˻� ��ġ���� ó�� Line�� Y �� */
		i32NearY	= 0;
		u8MarkSet	= 0x02;	/* Set the mark 1 does not exist in the calibration area */  
	}
	else
	{
		/* ���� Align Camera�� �̵��� ��ġ �������� ���� �ٻ�ġ �� ���� */
		for (i=0; i<(INT32)m_pstCali[cam_id-1].row_cnt; i++)
		{
			j	= i * INT32(m_pstCali[cam_id-1].col_cnt);
			if ((pstPosXY[j].stage_y) >= stage_y)
			{
				i32NearY	= i - 1;
				break;
			}
		}
	}
	/* �� (Top) �� Y ���� ��ġ�� ���� ���� ���� ���� ��� */
	if (i32NearY < 0)
	{
		swprintf_s(m_tzErrMsg, LOG_MESG_SIZE,
				   L"Align Calibration Out of range Last Y. i32NearY (%d) < 0", i32NearY);
		LOG_WARN(ENG_EDIC::en_acam_cali, m_tzErrMsg);
		/* Y ���� ���, X �� �˻� ��ġ���� ���� ������ Line�� Y �� */
		if (m_pstCali[cam_id-1].row_cnt > 1)	i32NearY = m_pstCali[cam_id-1].row_cnt - 2;
		else									i32NearY = m_pstCali[cam_id-1].row_cnt - 1;
  		u8MarkSet	= 0x02;	/* Set the mark 1 does not exist in the calibration area */
	}

	/* �˻��� ��ġ�� �ֺ� �簢�� ���� ��ǥ ��� */
	i32LT	= (i32NearY + 1) * m_pstCali[cam_id-1].col_cnt + (i32NearX + 0);	/* Left		- Top */
	i32RT	= (i32NearY + 1) * m_pstCali[cam_id-1].col_cnt + (i32NearX + 1);	/* Right	- Top */
	i32LB	= (i32NearY + 0) * m_pstCali[cam_id-1].col_cnt + (i32NearX + 0);	/* Left		- Bottom */
	i32RB	= (i32NearY + 0) * m_pstCali[cam_id-1].col_cnt + (i32NearX + 1);	/* Right	- Bottom */
	/* ���� �߽� ��ġ ���ϱ� (Y �� ��, ��/�� �߽�) */
	i32CentX= m_pstCali[cam_id-1].pos_xy[i32LB].a_cam_x +
			  (INT32)ROUNDED((m_pstCali[cam_id-1].pos_xy[i32RB].a_cam_x -
							  m_pstCali[cam_id-1].pos_xy[i32LB].a_cam_x) / 2.0f, 0);
	/* ���� �߽� ��ġ ���ϱ� (X �� ��, ��/�� �߽�) */
	i32CentY= m_pstCali[cam_id-1].pos_xy[i32LT].stage_y +
			  (INT32)ROUNDED((m_pstCali[cam_id-1].pos_xy[i32LT].stage_y -
							  m_pstCali[cam_id-1].pos_xy[i32LB].stage_y) / 2.0f, 0);
	/* ���� �˻��� ��ġ�� <��> ���� ������� Ȥ�� <��> ���� ������� �Ǵ� (X ���� �� ���ϱ� ����) */
	if (i32CentY > stage_y)	/* Bottom */
	{
		/* RB - LB */
		pstPrev	= &pstPosXY[i32LB];
		pstNext	= &pstPosXY[i32RB];
	}
	else					/* Top */
	{
		/* RT - LT */
		pstPrev	= &pstPosXY[i32LT];
		pstNext	= &pstPosXY[i32RT];
	}
	/* ���� ī�޶��� X ��ǥ�� �� ���� ������ ��� ���� ��ġ ���� (����)�� �ִ��� �� ���ϱ� */
	dbRate	= DOUBLE(pstNext->error_x - pstPrev->error_x) / DOUBLE(pstNext->a_cam_x - pstPrev->a_cam_x);
	/* �� ���� ������ ���� ���� ���� �� (������ ���� ��)��, ������ ���� ��ġ ���� (����) ���� */
	/* ���� ��, ���� �� ���� �� �� ��ǥ�� X ������ ���ָ�, ���� ���� X ��ǥ�� ���� ���� �� �� */
	error_x	= pstPrev->error_x + (INT32)ROUNDED(dbRate * (a_cam_x - pstPrev->a_cam_x), 0);
	/* ���� �˻��� ��ġ�� <��> ���� ������� Ȥ�� <��> ���� ������� �Ǵ� (Y ���� �� ���ϱ� ����) */
	if (i32CentX > a_cam_x)	/* Right */
	{
		/* RT - RB */
		pstPrev	= &pstPosXY[i32LB];
		pstNext	= &pstPosXY[i32LT];
	}
	else					/* Left */
	{
		/* LT - LB */
		pstPrev	= &pstPosXY[i32RB];
		pstNext	= &pstPosXY[i32RT];
	}
	/* ���� ī�޶��� X ��ǥ�� �� ���� ������ ��� ���� ��ġ ���� (����)�� �ִ��� �� ���ϱ� */
	dbRate	= DOUBLE(pstNext->error_y - pstPrev->error_y) / DOUBLE(pstNext->stage_y - pstPrev->stage_y);
	/* �� ���� ������ ���� ���� ���� �� (������ ���� ��)��, ������ ���� ��ġ ���� (����) ���� */
	/* ���� ��, ���� �� ���� �� �� ��ǥ�� Y ������ ���ָ�, ���� ���� Y ��ǥ�� ���� ���� �� �� */
	error_y	= pstPrev->error_y + (INT32)ROUNDED(dbRate * ((stage_y - pstPrev->stage_y)), 0);

	return u8MarkSet;
}

/*
 desc : Align Camera�� Calibration ������ ���, �߻��Ǵ� Offset �� ����
 parm : cam_id	- [in]  Camera Index (1 or 2)
		mark_no	- [in]  ����� Mark Number (Zero based)
		offset	- [in]  X Offset �� (0.1 um or 100 nm)
 retn : None
*/
VOID CACamCali::SetAlignACamVertGX(UINT8 cam_id, UINT8 mark_no, INT32 offset)
{
	m_pstShMem->cali_global[cam_id-1][mark_no]->gerb_mark_x_diff = offset;
}

/*
 desc : Align Camera�� Calibration ������ ���, �߻��Ǵ� Offset �� ����
 parm : cam_id	- [in]  Camera Index (1 or 2)
		mark_no	- [in]  ����� Mark Number
		offset	- [in]  X Offset ��
 retn : None
*/
VOID CACamCali::SetAlignACamVertLX(UINT8 cam_id, UINT8 mark_no, INT32 offset)
{
	m_pstShMem->cali_local[cam_id-1][mark_no]->gerb_mark_x_diff = offset;
}

/*
 desc : �� ����̺� �� ��, �� (Axis) �� Align Mark�� �����ϴ� ����� ���� ��ġ �� ����
 parm : cam_id	- [in]  Camera Index (1 or 2)
		mark	- [in]  Global or Local Mark
		axis	- [in]  0x00 : X Axis (Camera X), 0x01 : Y Axis (Stage Y)
		idx		- [in]  ���� ��ġ �� (Zero based)
		pos		- [in]  ����� ��ġ �� (mm)
 retn : None
*/
VOID CACamCali::AddMarkPos(UINT8 cam_id, ENG_AMTF mark, UINT8 axis, UINT8 idx, DOUBLE pos)
{
	LPG_ACCE pstCali	= NULL;
	if (cam_id < 1 || cam_id > GetConfig()->set_cams.acam_count)	return;

	/* in case : Global Mark */
	if (ENG_AMTF::en_global == mark)	pstCali	= m_pstShMem->cali_global[cam_id-1][idx];
	else								pstCali	= m_pstShMem->cali_local[cam_id-1][idx];

	/* X Axis ��, Align Camera X �� Motion ��ġ ���� ��� */
	if (0x00 == axis)	pstCali->acam_motion_x	= (INT32)ROUNDED(pos * 10000.0f, 0);
	/* Y Axis ��, Stage Y �� Motion ��ġ ���� ��� */
	else				pstCali->stage_motion_y	= (INT32)ROUNDED(pos * 10000.0f, 0);

	/* ������� ��ϵ� Mark Position�� ���� ���� */
	if (0x00 == axis)	
		m_u8MarkCount++;
}


/*
 desc : Calibration Align Camera File ����
 parm : cali_thick	- [in]  2D ������ �뱤 ���� �β� �� (����: um) (100 ~ 99999 um)
 retn : None
*/
BOOL CACamCali::LoadFileEx(CString strFilePath)
{
	CStdioFile sFile;
	CString strLine;
	CString strValue;
	STM_TDCD stIndex;

	m_stCaliEx.Clear();

	if (TRUE == sFile.Open(strFilePath, CStdioFile::shareDenyNone | CStdioFile::modeRead))
	{
		m_strFilePath = strFilePath;

		// ������ 4��° �ٱ��� Pass 
		for (int i = 0; i < 4; i++)
		{
			sFile.ReadString(strLine);
		}

		m_stCaliEx.u32RowCnt = _ttoi(strLine);
		
		sFile.ReadString(strLine);
		m_stCaliEx.u32ColCnt = _ttoi(strLine);

		while (sFile.ReadString(strLine))
		{
			// Parsing
			AfxExtractSubString(strValue, strLine, 0, _T(','));
			stIndex.dPosX = _ttof(strValue);
			AfxExtractSubString(strValue, strLine, 1, _T(','));
			stIndex.dPosY = _ttof(strValue);
			AfxExtractSubString(strValue, strLine, 2, _T(','));
			stIndex.dErrorX = _ttof(strValue);
			AfxExtractSubString(strValue, strLine, 3, _T(','));
			stIndex.dErrorY = _ttof(strValue);

			if (TRUE == strValue.IsEmpty())
			{
				break;
			}

			m_stCaliEx.stVctIndex.push_back(stIndex);
		}

		sFile.Close();
		return TRUE;
	}

 	return FALSE;
}


UINT8 CACamCali::GetCaliPosEx(double dPosX, double dPosY, double& dErrX, double& dErrY)
{
	UINT8 u8MarkSet = 0x01;	/* Check if there is an align mark in the calibration area */
	INT32 i32NearX = -1, i32NearY = -1;
	double dCentX, dCentY;
	INT32 i32LB, i32RB, i32RT, i32LT;
	DOUBLE dbRate = 0.0f;
	LPM_TDCD pstPrev = NULL, pstNext = NULL;

	/* Calibration Data�� �����ϴ��� ���� */
	if (m_stCaliEx.u32RowCnt < 1 || m_stCaliEx.u32ColCnt < 1)
	{
		swprintf_s(m_tzErrMsg, LOG_MESG_SIZE,
			L"Invalid number of rows or columns set [rows:%d][cols:%d]",
			m_stCaliEx.u32RowCnt, m_stCaliEx.u32ColCnt);
		LOG_ERROR(ENG_EDIC::en_acam_cali, m_tzErrMsg);
		return 0x00;
	}

	/* �˻� ������ ���, Offset ���� ������ 0 ��ȯ */
	dErrX = 0;
	dErrY = 0;

	/* ---------------------------------------------------------------- */
	/* dPosX �� ������ Align Calibration ������ ���� �ȿ� �ִ��� Ȯ�� */
	/* ---------------------------------------------------------------- */
	if (m_stCaliEx.stVctIndex[0].dPosX > dPosX)
	{
		swprintf_s(m_tzErrMsg, LOG_MESG_SIZE,
			L"Invalid X Position. m_stCaliEx.stVctIndex[0].dPosX (%.4f) > dPosX (%.4f)",
			m_stCaliEx.stVctIndex[0].dPosX, dPosX);
		LOG_WARN(ENG_EDIC::en_acam_cali, m_tzErrMsg);

		i32NearX = 0;
		u8MarkSet = 0x02;	/* Set the mark 1 does not exist in the calibration area */
	}
	else
	{
		/* ���� Align Camera�� �̵��� ��ġ �������� ���� �ٻ�ġ �� ���� */
		for (int i = 0; i < (int)m_stCaliEx.u32ColCnt; i++)
		{
			if (m_stCaliEx.stVctIndex[i].dPosX >= dPosX)
			{
				i32NearX = i - 1;
				break;
			}
		}
	}

	/* ���� �˻��� Align Camera X ���� ��ġ�� ���� ���� ���� ���� ��� */
	if (i32NearX < 0)
	{
		swprintf_s(m_tzErrMsg, LOG_MESG_SIZE,
			L"Align Calibration Out of range Last X. i32NearX (%d) < 0", i32NearX);
		LOG_WARN(ENG_EDIC::en_acam_cali, m_tzErrMsg);

		if (m_stCaliEx.u32ColCnt > 1)			i32NearX = m_stCaliEx.u32ColCnt - 2;
		else									i32NearX = m_stCaliEx.u32ColCnt - 1;

		u8MarkSet = 0x02;	/* Set the mark 1 does not exist in the calibration area */
	}

	/* ---------------------------------------------------------------- */
	/* dPosY �� ������ Align Calibration ������ ���� �ȿ� �ִ��� Ȯ�� */
	/* ---------------------------------------------------------------- */
	if (m_stCaliEx.stVctIndex[0].dPosY > dPosY)
	{
		swprintf_s(m_tzErrMsg, LOG_MESG_SIZE,
			L"Align Calibration Out of range First Y. m_stCaliEx.stVctIndex[0].dPosY (%.4f) > dPosY(%.4f)",
			m_stCaliEx.stVctIndex[0].dPosY, dPosY);
		LOG_WARN(ENG_EDIC::en_acam_cali, m_tzErrMsg);

		/* Y ���� ���, X �� �˻� ��ġ���� ó�� Line�� Y �� */
		i32NearY = 0;
		u8MarkSet = 0x02;	/* Set the mark 1 does not exist in the calibration area */
	}
	else
	{
		/* ���� Align Camera�� �̵��� ��ġ �������� ���� �ٻ�ġ �� ���� */
		for (int i = 0; i < (int)m_stCaliEx.u32RowCnt; i++)
		{
			int j = i * (int)m_stCaliEx.u32RowCnt;

			if (m_stCaliEx.stVctIndex[j].dPosY >= dPosY)
			{
				i32NearY = i - 1;
				break;
			}
		}
	}

	/* �� (Top) �� Y ���� ��ġ�� ���� ���� ���� ���� ��� */
	if (i32NearY < 0)
	{
		swprintf_s(m_tzErrMsg, LOG_MESG_SIZE,
			L"Align Calibration Out of range Last Y. i32NearY (%d) < 0", i32NearY);
		LOG_WARN(ENG_EDIC::en_acam_cali, m_tzErrMsg);

		/* Y ���� ���, X �� �˻� ��ġ���� ���� ������ Line�� Y �� */
		if (m_stCaliEx.u32RowCnt > 1)	i32NearY = m_stCaliEx.u32RowCnt - 2;
		else							i32NearY = m_stCaliEx.u32RowCnt - 1;

		u8MarkSet = 0x02;	/* Set the mark 1 does not exist in the calibration area */
	}

	/* �˻��� ��ġ�� �ֺ� �簢�� ���� ��ǥ ��� */
	i32LT = (i32NearY + 1) * m_stCaliEx.u32ColCnt + (i32NearX + 0);	/* Left		- Top */
	i32RT = (i32NearY + 1) * m_stCaliEx.u32ColCnt + (i32NearX + 1);	/* Right	- Top */
	i32LB = (i32NearY + 0) * m_stCaliEx.u32ColCnt + (i32NearX + 0);	/* Left		- Bottom */
	i32RB = (i32NearY + 0) * m_stCaliEx.u32ColCnt + (i32NearX + 1);	/* Right	- Bottom */

	/* ���� �߽� ��ġ ���ϱ� (Y �� ��, ��/�� �߽�) */
	dCentX = m_stCaliEx.stVctIndex[i32LB].dPosX +
		(INT32)ROUNDED((m_stCaliEx.stVctIndex[i32RB].dPosX -
			m_stCaliEx.stVctIndex[i32LB].dPosX) / 2.0f, 0);

	/* ���� �߽� ��ġ ���ϱ� (X �� ��, ��/�� �߽�) */
	dCentY = m_stCaliEx.stVctIndex[i32LT].dPosY +
		(INT32)ROUNDED((m_stCaliEx.stVctIndex[i32LT].dPosY -
			m_stCaliEx.stVctIndex[i32LB].dPosY) / 2.0f, 0);

	/* ���� �˻��� ��ġ�� <��> ���� ������� Ȥ�� <��> ���� ������� �Ǵ� (X ���� �� ���ϱ� ����) */
	if (dCentY > dPosY)	/* Bottom */
	{
		/* RB - LB */
		pstPrev = &m_stCaliEx.stVctIndex[i32LB];
		pstNext = &m_stCaliEx.stVctIndex[i32RB];
	}
	else					/* Top */
	{
		/* RT - LT */
		pstPrev = &m_stCaliEx.stVctIndex[i32LT];
		pstNext = &m_stCaliEx.stVctIndex[i32RT];
	}

	/* ���� ī�޶��� X ��ǥ�� �� ���� ������ ��� ���� ��ġ ���� (����)�� �ִ��� �� ���ϱ� */
	dbRate = DOUBLE(pstNext->dErrorX - pstPrev->dErrorX) / DOUBLE(pstNext->dPosX - pstPrev->dPosX);
	/* �� ���� ������ ���� ���� ���� �� (������ ���� ��)��, ������ ���� ��ġ ���� (����) ���� */
	/* ���� ��, ���� �� ���� �� �� ��ǥ�� X ������ ���ָ�, ���� ���� X ��ǥ�� ���� ���� �� �� */
	dErrX = pstPrev->dErrorX + (INT32)ROUNDED(dbRate * (dPosX - pstPrev->dPosX), 0);
	/* ���� �˻��� ��ġ�� <��> ���� ������� Ȥ�� <��> ���� ������� �Ǵ� (Y ���� �� ���ϱ� ����) */
	if (dCentX > dPosX)	/* Right */
	{
		/* RT - RB */
		pstPrev = &m_stCaliEx.stVctIndex[i32LB];
		pstNext = &m_stCaliEx.stVctIndex[i32LT];
	}
	else					/* Left */
	{
		/* LT - LB */
		pstPrev = &m_stCaliEx.stVctIndex[i32RB];
		pstNext = &m_stCaliEx.stVctIndex[i32RT];
	}

	/* ���� ī�޶��� X ��ǥ�� �� ���� ������ ��� ���� ��ġ ���� (����)�� �ִ��� �� ���ϱ� */
	dbRate = DOUBLE(pstNext->dErrorY - pstPrev->dErrorY) / DOUBLE(pstNext->dPosY - pstPrev->dPosY);
	/* �� ���� ������ ���� ���� ���� �� (������ ���� ��)��, ������ ���� ��ġ ���� (����) ���� */
	/* ���� ��, ���� �� ���� �� �� ��ǥ�� Y ������ ���ָ�, ���� ���� Y ��ǥ�� ���� ���� �� �� */
	dErrY = pstPrev->dErrorY + (INT32)ROUNDED(dbRate * ((dPosY - pstPrev->dPosY)), 0);

	return u8MarkSet;
}