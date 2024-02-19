/*
 desc : MIL Pattern Image (Buffer) ���� �� ó�� (Source Image : ��ϵ� Pattern Image)
*/

#include "pch.h"
#include "MainApp.h"
#include "MilModel.h"
#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

#define USE_MIXED_MARK_CALC_AVERAGE	0	/* ���� ���� �˻��� ��ũ���� ��� �� ������ ���� */
#define	MAX_EDGE_LINE_FIND_COUNT	25

/*
 desc : ������
 parm : config		- [in]  Config Info.
		shmem		- [in]  Shared Memory
		cam_id		- [in]  Align Camera Index. (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		ml_sys		- [in]  MIL System ID
		ml_dis		- [in]  MIL Display ID
		run_mode	- [in]  0x01 : CMPS, 0x01 : Vision Step
 retn : None
*/
#ifndef _NOT_USE_MIL_
CMilModel::CMilModel(LPG_CIEA config, LPG_VDSM shmem,
	UINT8 cam_id, /*MIL_ID ml_sys, MIL_ID ml_dis, */ENG_ERVM run_mode)
	: CMilImage(config, shmem, cam_id, /*ml_sys, ml_dis, */run_mode)
#else
CMilModel::CMilModel(LPG_CIEA config, LPG_VDSM shmem,
	UINT8 cam_id, ENG_ERVM run_mode)
	: CMilImage(config, shmem, cam_id, run_mode)
#endif
{
#ifndef _NOT_USE_MIL_
	m_mlEdgeID = M_NULL;
	m_mlLineID = M_NULL;	/* Line Detection ID (�ӽ�) */
	m_mlStripID = M_NULL;
	//m_mIMergeMark = M_NULL;
	for (int i = 0; i < 3; i++) {
		m_mlModelID[i] = M_NULL;
		m_mlPATID[i] = M_NULL;
		m_findMode[i] = 0;
	}
	m_mlPATID_D = M_NULL;
	m_mlModelID_D = M_NULL;
#endif
	//m_u8ModelRegist = 0;
	m_enMarkMethod = ENG_MMSM::en_single;
	m_u8MarkFindSet = 0x01;
	m_u8MarkFindGet = 0x00;
	m_i32EdgeFindGet = 0;	/* Edge Detection �˻� �� ����� ��� */
	m_camid = cam_id;
	
	/* ��� ��� �ӽ÷� �����ϱ� ���� ���� */
	CreateMarkResult();
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CMilModel::~CMilModel()
{
#ifndef _NOT_USE_MIL_
	/* ���� �Ҵ�� �޸𸮰� ������ ���� */
	if (m_mlEdgeID)		MbufFree(m_mlEdgeID);
	m_mlEdgeID = M_NULL;
	/* Line Detection MIL ID �޸� ���� */
	if (m_mlLineID)		MbufFree(m_mlLineID);
	m_mlLineID = NULL;
	/* Marker Strip MIL ID �޸� ���� */
	if (m_mlStripID)	MbufFree(m_mlStripID);
	m_mlStripID = NULL;

	if (theApp.clMilMain.MilEdgeContext)	MedgeFree(theApp.clMilMain.MilEdgeContext);
	theApp.clMilMain.MilEdgeContext = M_NULL;
#endif
	for (int i = 2; i >= 0; i--) {
		/* ���� ������ �� �ʱ�ȭ */
		for (int j = 0; j < 2; j++) {
			ReleaseMarkModel(i, j);
		}
	}
	/*  �˻� �� ��� �޸� ���� */
	ReleaseMarkResult();
}

/*
 desc : ���� ������ �� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CMilModel::ReleaseMarkModel(UINT8 i, UINT8 findMode)
{
#ifndef _NOT_USE_MIL_
	if (findMode == 0) {
		if (m_mlModelID[i])	
			MmodFree(m_mlModelID[i]);
		m_mlModelID[i] = M_NULL;
		if (m_mlModelID_D)	
			MmodFree(m_mlModelID_D);
		m_mlModelID_D = M_NULL;

	}
	else {
		if (m_mlPATID[i])		
			MpatFree(m_mlPATID[i]);
		m_mlPATID[i] = M_NULL;
		if (m_mlPATID_D)		
			MpatFree(m_mlPATID_D);
		m_mlPATID_D = M_NULL;
	}
#endif
	//m_u8ModelRegist = 0;
}

/*
 desc : ��ũ �� ���� ����� ������, �� �˻� ����� ������ ������ ��ü ����
 parm : None
 retn : None
*/
VOID CMilModel::CreateMarkResult()
{
	// by sys&j : MIL_INT�� INT64 ���� ������� ����. ���� MIL �̻�� ȣȯ���� ���� INT64�� ��ȯ
	//m_pMilIndex = (PINT64)::Alloc(sizeof(MIL_INT) * m_pstConfig->mark_find.max_mark_find);
	m_pMilIndex = (PINT64)::Alloc(sizeof(INT64) * m_pstConfig->mark_find.max_mark_find);
	m_pFindPosX = (DOUBLE*)::Alloc(sizeof(DOUBLE) * m_pstConfig->mark_find.max_mark_find);
	m_pFindPosY = (DOUBLE*)::Alloc(sizeof(DOUBLE) * m_pstConfig->mark_find.max_mark_find);
	m_pFindAngle = (DOUBLE*)::Alloc(sizeof(DOUBLE) * m_pstConfig->mark_find.max_mark_find);
	m_pFindScore = (DOUBLE*)::Alloc(sizeof(DOUBLE) * m_pstConfig->mark_find.max_mark_find);
	m_pFindScale = (DOUBLE*)::Alloc(sizeof(DOUBLE) * m_pstConfig->mark_find.max_mark_find);
	m_pFindDist = (DOUBLE*)::Alloc(sizeof(DOUBLE) * m_pstConfig->mark_find.max_mark_find);
	m_pFindCovg = (DOUBLE*)::Alloc(sizeof(DOUBLE) * m_pstConfig->mark_find.max_mark_find);
	m_pFindFitErr = (DOUBLE*)::Alloc(sizeof(DOUBLE) * m_pstConfig->mark_find.max_mark_find);
	m_pFindCircleRadius = (DOUBLE*)::Alloc(sizeof(DOUBLE) * m_pstConfig->mark_find.max_mark_find);

	m_pstMarkModel = m_pstShMemVisi->mod_define[m_u8ACamID - 1];
	m_pstModelSize = m_pstShMemVisi->mod_size[m_u8ACamID - 1];
	m_pstModResult = m_pstShMemVisi->mark_result[m_u8ACamID - 1];

	m_pGrabMarkImage = m_pstShMemVisi->mark_image[m_u8ACamID - 1];
	m_pGrabEdgeImage = m_pstShMemVisi->edge_image[m_u8ACamID - 1];
	m_pGrabLineImage = m_pstShMemVisi->line_image[m_u8ACamID - 1];
}

/*
 desc : ��ũ �� �˻� ��� �޸� ����
 parm : None
 retn : None
*/
VOID CMilModel::ReleaseMarkResult()
{
	if (m_pMilIndex)	::Free(m_pMilIndex);
	if (m_pFindPosX)	::Free(m_pFindPosX);
	if (m_pFindPosY)	::Free(m_pFindPosY);
	if (m_pFindAngle)	::Free(m_pFindAngle);
	if (m_pFindScore)	::Free(m_pFindScore);
	if (m_pFindScale)	::Free(m_pFindScale);
	if (m_pFindDist)	::Free(m_pFindDist);
	if (m_pFindCovg)	::Free(m_pFindCovg);
	if (m_pFindFitErr)	::Free(m_pFindFitErr);
	if (m_pFindCircleRadius)	::Free(m_pFindCircleRadius);
	
	m_pMilIndex = NULL;
	m_pFindPosX = NULL;
	m_pFindPosY = NULL;
	m_pFindAngle = NULL;
	m_pFindScore = NULL;
	m_pFindScale = NULL;
	m_pFindDist = NULL;
	m_pFindCovg = NULL;
	m_pFindFitErr = NULL;
	m_pFindCircleRadius = NULL;
}

/*
 desc : ��� ������ �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CMilModel::ResetMarkResult()
{
#if 0	/* �Ʒ� 2�� ������ �ʱ�ȭ �ϸ� �ȵ� (Shared Memory���� ���������� ó�� ��) */
	memset(m_pstMarkModel, 0x00, sizeof(STG_CMPV) * m_pstConfig->mark_find.max_mark_regist);
	memset(m_pstModelSize, 0x00, sizeof(STG_DBXY) * m_pstConfig->mark_find.max_mark_regist);
#endif
	memset(m_pstModResult, 0x00, sizeof(STG_GMFR) * m_pstConfig->mark_find.max_mark_find);

	memset(m_pMilIndex, 0x00, sizeof(INT64) * m_pstConfig->mark_find.max_mark_find);
	memset(m_pFindPosX, 0x00, sizeof(DOUBLE) * m_pstConfig->mark_find.max_mark_find);
	memset(m_pFindPosY, 0x00, sizeof(DOUBLE) * m_pstConfig->mark_find.max_mark_find);
	memset(m_pFindAngle, 0x00, sizeof(DOUBLE) * m_pstConfig->mark_find.max_mark_find);
	memset(m_pFindScore, 0x00, sizeof(DOUBLE) * m_pstConfig->mark_find.max_mark_find);
	memset(m_pFindScale, 0x00, sizeof(DOUBLE) * m_pstConfig->mark_find.max_mark_find);
	memset(m_pFindDist, 0x00, sizeof(DOUBLE) * m_pstConfig->mark_find.max_mark_find);
	memset(m_pFindCovg, 0x00, sizeof(DOUBLE) * m_pstConfig->mark_find.max_mark_find);
	memset(m_pFindFitErr, 0x00, sizeof(DOUBLE) * m_pstConfig->mark_find.max_mark_find);
	memset(m_pFindCircleRadius, 0x00, sizeof(DOUBLE) * m_pstConfig->mark_find.max_mark_find);
}

/*
 desc : ���� ��ϵ� Mark Model ���� ��ȯ
 parm : index	- [in]  ��ϵ� ��ũ ���� �迭 �ε��� (Zero-based. �ִ� m_pstConfig->mark_find.max_mark_regist-1)
 retn : Mark Model �� (���ٸ� 0 �� ��ȯ)
*/
UINT32 CMilModel::GetMarkModelType(UINT8 index/* =0 */)
{
	//if (index >= m_u8ModelRegist/*m_pstConfig->mark_find.max_mark_regist*/)	return 0;
	if (!m_pstMarkModel)									return 0;
	if (ENG_MMDT(m_pstMarkModel[index].type) != ENG_MMDT::en_image)
	{
		if (!m_pstMarkModel[index].IsValid())	return 0;
	}
	return m_pstMarkModel[index].type;
}

/*
 desc : Mark ���� ����
 parm : speed		- [in]  0 - Low, 1 - Medium, 2 - High, 3 - Very High
		level		- [in]  ��ϵ� �� �̹����� Grabbed Image�̹����� Edge ������ ���� �� ���� ��
							���� Ŭ���� ���� ��������, �ӵ��� ���� ��������
							LOW���� HIGH�� ������ �� ���� Edge�� ������ ���ϰ� ��
							0 (M_MEDIUM), 1 (M_HIGH), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - �⺻ ��, 1.0f ~ 100.0f
							�⺻ ���� 50������, 100���� ������ ������ ������ ���͸� ��Ÿ���ϴ�.
		scale_min	- [in]  �˻� ����� ũ�� ���� �� ���� (�ּ��� �� �� �������� Ŀ�� ��. ����: 0.5 ~ 1.0)
		scale_max	- [in]  �˻� ����� ũ�� ���� �� ���� (�ִ��� �� �� �������� �۾ƾ� ��. ����: 1.0 ~ 1.0)
		score_min	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0))
		score_tgt	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0))
 retn : TRUE or FALSE
*/
BOOL CMilModel::SetModelDefine(UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, // lk91 SetModelDefine_tot�� ����
	DOUBLE scale_min, DOUBLE scale_max, DOUBLE score_min, DOUBLE score_tgt)	 
{
	return TRUE;
}

/*
 desc : Mark ���� ����
 parm : speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		detail_level- [in]  ��ϵ� �� �̹����� Grabbed Image�̹����� Edge ������ ���� �� ���� ��
							���� Ŭ���� ���� ��������, �ӵ��� ���� ��������
							LOW���� HIGH�� ������ �� ���� Edge�� ������ ���ϰ� ��
							0 (M_MEDIUM), 1 (M_HIGH), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - �⺻ ��, 1.0f ~ 100.0f
							�⺻ ���� 50������, 100���� ������ ������ ������ ���͸� ��Ÿ���ϴ�.
		value		- [in]  Model Type, Param 1 ~ 5�� �������� ��� ���Ե� ����ü ������
		count		- [in]  ����ϰ��� �ϴ� ���� ����
		scale_min	- [in]  �˻� ����� ũ�� ���� �� ���� (�ּ��� �� �� �������� Ŀ�� ��. ����: 0.5 ~ 1.0)
		scale_max	- [in]  �˻� ����� ũ�� ���� �� ���� (�ִ��� �� �� �������� �۾ƾ� ��. ����: 1.0 ~ 1.0)
		score_min	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0)
		score_tgt	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0))
 retn : TRUE or FALSE
*/
BOOL CMilModel::SetModelDefine(UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, LPG_CMPV value,		// lk91 SetModelDefine_tot�� ����
	UINT8 mark_no, DOUBLE scale_min, DOUBLE scale_max,
	DOUBLE score_min, DOUBLE score_tgt)
{
//	UINT8 i = 0;
//
//	/* ���� �����Ǿ� �ִٸ� �ϴ� ���� */
//	ReleaseMarkModel();
//	/* ���� ��� ���� �ʱ�ȭ */
//	ResetMarkResult();
//#if 0
//	/* ����ϰ��� �ϴ� ���� ���� �ľ� (�ִ� ��� ������ ���� ���ϵ��� �ϱ� ����) */
//	for (i = 0; value[i].IsValid() && i < m_pstConfig->mark_find.max_mark_regist; i++)
//	{
//		m_u8ModelRegist++;
//	}
//#else
//	/* ����ϰ��� �ϴ� �� ���� ���� */
//	m_u8ModelRegist = count;
//	/* ��� ��� ���� ������ �ʰ��ϸ�, �ִ� ������ ���� */
//	if (m_pstConfig->mark_find.max_mark_regist < count)
//	{
//		m_u8ModelRegist = m_pstConfig->mark_find.max_mark_regist;
//		AfxMessageBox(L"The number of registration models was exceeded,\n"
//			L"and replaced with the maximum value defined in the config file",
//			MB_ICONINFORMATION);
//	}
//#endif
//	/* ��ȿ�� �˻� ����� �� �� ���� */
//	for (i = 0; i < m_u8ModelRegist && value[i].IsValid(); i++)
//	{
//		memcpy(&m_pstMarkModel[i], &value[i], sizeof(STG_CMPV));
//	}

	return SetModelDefine(speed, level, count, smooth, scale_min, scale_max, score_min);
}

/*
 desc : Mark ���� ����
 parm : speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		detail_level- [in]  ��ϵ� �� �̹����� Grabbed Image�̹����� Edge ������ ���� �� ���� ��
							���� Ŭ���� ���� ��������, �ӵ��� ���� ��������
							LOW���� HIGH�� ������ �� ���� Edge�� ������ ���ϰ� ��
							0 (M_MEDIUM), 1 (M_HIGH), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - �⺻ ��, 1.0f ~ 100.0f
							�⺻ ���� 50������, 100���� ������ ������ ������ ���͸� ��Ÿ���ϴ�.
		model		- [in]  Model Type ��, circle, square, rectangle, cross, diamond, triangle
		param		- [in]  �� 5���� Parameter Values (unit : um)
		count		- [in]  ����ϰ��� �ϴ� ���� ����
		scale_min	- [in]  �˻� ����� ũ�� ���� �� ���� (�ּ��� �� �� �������� Ŀ�� ��. ����: 0.5 ~ 1.0)
		scale_max	- [in]  �˻� ����� ũ�� ���� �� ���� (�ִ��� �� �� �������� �۾ƾ� ��. ����: 1.0 ~ 1.0)
		score_min	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0)
		score_tgt	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0))
 retn : TRUE or FALSE
*/
BOOL CMilModel::SetModelDefine(UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, PUINT32 model,
	DOUBLE* param1, DOUBLE* param2, DOUBLE* param3,	// lk91 param �迭 �Ѱ��ִ� �κ� üũ
	DOUBLE* param4, DOUBLE* param5, UINT8 mark_no,
	DOUBLE scale_min, DOUBLE scale_max,
	DOUBLE score_min, DOUBLE score_tgt)
{

	/* ���� �����Ǿ� �ִٸ� �ϴ� ���� */
	ReleaseMarkModel(mark_no, 0);
	/* ���� ��� ���� �ʱ�ȭ */
	ResetMarkResult();
	///* ����ϰ��� �ϴ� �� ���� ���� */
	//m_u8ModelRegist = count;
	/* ��� ��� ���� ������ �ʰ��ϸ�, �ִ� ������ ���� */
	//if (m_pstConfig->mark_find.max_mark_regist < count)
	//{
	//	m_u8ModelRegist = m_pstConfig->mark_find.max_mark_regist;
	//	AfxMessageBox(L"The number of registration models was exceeded,\n"
	//		L"and replaced with the maximum value defined in the config file",
	//		MB_ICONINFORMATION);
	//}

	/* ��ȿ�� �˻� ����� �� �� ���� */
	//for (i = 0; i < m_u8ModelRegist/*m_pstConfig->mark_find.max_mark_regist*/; i++)
	//{
	//	m_pstMarkModel[i].type = model[i];
	//	m_pstMarkModel[i].param[0] = param1[i];
	//	m_pstMarkModel[i].find_type = 0;
	//	if (param2)	m_pstMarkModel[i].param[1] = param2[i];
	//	if (param3)	m_pstMarkModel[i].param[2] = param3[i];
	//	if (param4)	m_pstMarkModel[i].param[3] = param4[i];
	//	if (param5)	m_pstMarkModel[i].param[4] = param5[i];
	//}

	m_pstMarkModel[mark_no].type = model[0];
	m_pstMarkModel[mark_no].param[0] = param1[0];
	if (param2)	m_pstMarkModel[mark_no].param[1] = param2[0];
	if (param3)	m_pstMarkModel[mark_no].param[2] = param3[0];
	if (param4)	m_pstMarkModel[mark_no].param[3] = param4[0];
	if (param5)	m_pstMarkModel[mark_no].param[4] = param5[0];
	return SetModelDefine_tot(speed, level, count, smooth, mark_no, NULL,
		scale_min, scale_max, score_min, score_tgt);
}

/*
 desc : Mark ���� ���� - �̹��� ������
 parm : speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		level		- [in]  ��ϵ� �� �̹����� Grabbed Image�̹����� Edge ������ ���� �� ���� ��
							���� Ŭ���� ���� ��������, �ӵ��� ���� ��������
							LOW���� HIGH�� ������ �� ���� Edge�� ������ ���ϰ� ��
							0 (M_MEDIUM), 1 (M_HIGH), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - �⺻ ��, 1.0f ~ 100.0f
		scale_min	- [in]  �˻� ����� ũ�� ���� �� ���� (�ּ��� �� �� �������� Ŀ�� ��. ����: 0.5 ~ 1.0)
		scale_max	- [in]  �˻� ����� ũ�� ���� �� ���� (�ִ��� �� �� �������� �۾ƾ� ��. ����: 1.0 ~ 1.0)
		score_min	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0)
		name		- [in]  Model Name
		file		- [in]  �� �̹����� ����� ���� �̸� (��ü ��� ����. Image File)
 retn : TRUE or FALSE
*/
BOOL CMilModel::SetModelDefineLoad(UINT8 speed, UINT8 level, DOUBLE smooth,	// lk91 �̻��
	DOUBLE scale_min, DOUBLE scale_max,
	DOUBLE score_min, DOUBLE score_tgt,
	PTCHAR name, CStringArray& file)
{
	return TRUE;
}


/*
 desc : �ش� �˻� ���ǿ� ���յǴ� ������ �ִ��� Ȯ��
 parm : find	- [in]  ã���� �ϴ� ����
		score	- [in]  ��Ī ���� ��
		scale	- [in]  ũ�� ���� ��
 retn : TRUE (find ������ �����Ѵ�) or FALSE (find �������� ���ų� ���� ���)
*/
BOOL CMilModel::IsFindModCount(UINT8 find, DOUBLE score, DOUBLE scale)
{
	UINT8 i = 0, u8Find = 0;

	for (; i < m_u8MarkFindGet; i++)
	{
		if (m_pstModResult[i].IsValidMark(score, scale))	u8Find++;
	}
	if (find > u8Find)
	{
		TCHAR tzMesg[128] = { NULL };
		swprintf_s(tzMesg, 128, L"Find count Invalid = total(%d), find :%d, get=%d\n",
			m_u8MarkFindGet, find, u8Find);
		LOG_WARN(ENG_EDIC::en_mil, tzMesg);
		return FALSE;
	}
	return TRUE;
}

/*
 desc : �˻��� ��� �� Scale ���� 1.0�� ���� ����� �͸� ��ȯ
 parm : None
 retn : �˻��� ����� ����� ������ ��ȯ
*/
LPG_GMFR CMilModel::GetFindMarkBestScale()
{
	UINT8 i;
	LPG_GMFR pstResult = NULL;

	/* �˻��� ��찡 ������ ���� */
	if (m_u8MarkFindGet < 1)	return NULL;
	/* �� ó�� ��ġ �� ������ ���� */
	pstResult = &m_pstModResult[0];
	/* ���� �˻��� ����� 1���� ���� �ٷ� ���� */
	if (1 == m_u8MarkFindGet)
	{
		/* �˻� ����� �� �ε��� �������� �� ũ�� ���� */
		//m_pstModResult[0].mark_width = (UINT32)ROUNDED(m_pstModelSize[m_pstModResult[0].model_index].x, 0);
		//m_pstModResult[0].mark_height = (UINT32)ROUNDED(m_pstModelSize[m_pstModResult[0].model_index].y, 0);
		m_pstModResult[0].mark_width = (UINT32)ROUNDED(m_pstMarkModel[0].iSizeP.x, 0);
		m_pstModResult[0].mark_height = (UINT32)ROUNDED(m_pstMarkModel[0].iSizeP.y, 0);
	}
	else
	{
		/* �˻��� ����� 2�� �̻��� ���, Scale ���� 1.0�� ���� ����� ���� ��ȯ */
		for (i = 0x00; i < m_u8MarkFindGet; i++)  
		{
			/* �˻� ����� �� �ε��� �������� �� ũ�� ���� */
			//m_pstModResult[i].mark_width = (UINT32)ROUNDED(m_pstModelSize[m_pstModResult[i].model_index].x, 0);
			//m_pstModResult[i].mark_height = (UINT32)ROUNDED(m_pstModelSize[m_pstModResult[i].model_index].y, 0);
			m_pstModResult[i].mark_width = (UINT32)ROUNDED(m_pstMarkModel[0].iSizeP.x, 0);
			m_pstModResult[i].mark_height = (UINT32)ROUNDED(m_pstMarkModel[0].iSizeP.y, 0);
			/* ���� ������ ���� �˻��� ���� Scale�� �� ��ϵ� ��ũ ũ�⿡ ������� �� */
			if (pstResult->scale_rate < m_pstModResult[i].scale_rate)
			{
				pstResult = &m_pstModResult[i];
			}
		}
	}

	/* ���� ���� ã������, �ᱹ 1���� ã�Ҵٰ� ������ ���� */
	if (pstResult != &m_pstModResult[0])	memcpy(&m_pstModResult[0], pstResult, sizeof(STG_GMFR));
	m_u8MarkFindGet = 0x01;

	/* ���� �˻��� ����� ����� ������ ��ȯ */
	pstResult->valid_multi = 0x01;	/* Fixed */
	pstResult->mark_method = UINT8(m_enMarkMethod);

	return pstResult;
}

/*
 desc : �˻��� ����� �߿��� �߾� 1�� ��ũ �׸��� �ֺ� ��ũ �������� �߽� ���� ���� ��� ��ȯ
 parm : None
 retn : �˻��� ����� ����� ������ ��ȯ
*/
LPG_GMFR CMilModel::GetFindMarkCentSide()
{
	TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
	UINT8 i/*, j, k*/;
	BOOL bSucc = FALSE;
	DOUBLE dbDistCent = 0.0f;		/* ���� �߽ɰ� ���簢���� �߽� ���� �Ÿ� (����: pixel) */
	DOUBLE dbMarkSize = 0.0f, dbSquareRate = 0.0f;
	STG_DBXY stCent = { NULL }, stDist = { NULL };
	LPG_GMFR pstMain = { NULL };
	PUINT8 pValid = NULL;
	LPG_CVMF pstMarkFind = &m_pstConfig->mark_find;

	/* ��� �� �ʱ�ȭ */
	memset(&m_stModResult, 0x00, sizeof(STG_GMFR));
	/* �˻� ��� �� ���� */
	m_stModResult.mark_method = (UINT8)m_enMarkMethod;

	/* �˻��� ��� Mark ���� ���� ��ȿ ���� ���� */
	for (i = 0x00; i < m_u8MarkFindGet; i++)
	{
		/* �⺻ �� ���� */
		m_pstModResult[i].mark_width = UINT32(ROUNDED(m_pstModelSize[m_pstModResult[i].model_index].x, 0));
		m_pstModResult[i].mark_height = UINT32(ROUNDED(m_pstModelSize[m_pstModResult[i].model_index].y, 0));
		m_pstModResult[i].mark_method = (UINT8)m_enMarkMethod;
		/* �̹� ModDefine �Լ����� Scale Rate Min ~ Max ���� Acceptance (Score) Rate �̻� ���� �˻� */
		/* �ϱ� ���� �˻� ������ ���� �߱� ������ �Ʒ�, ���� (Interlock) ������ �ʿ� ����. �ּ�ó�� */
#if 0
		/* ���⼭ �˻� ���� (Only score_rate, scale_rate)�� ���� �ʴ� ���� �ɷ� ���� */
		/* ã���� �ϴ� �� (Mark)�� ũ�Ⱑ ��κ� �۱� ������, scale���� score�� ������ �� */
#if 1
		if (m_pstModResult[i].score_rate >= m_dbRecipeScore &&
			m_pstModResult[i].scale_rate >= m_dbRecipeScale)
#else
		if (m_pstModResult[i].score_rate >= m_dbRecipeScore)
#endif
#endif
		{
			m_pstModResult[i].valid_multi = 0x01;
			/* �߽� ��ũ�� ���� ���� ���, 1���� �����ؾ� �ϹǷ� */
			if (ENG_MMSM::en_cent_side == m_enMarkMethod &&
				m_pstModResult[i].model_index == 0x00)
			{
				if (!pstMain)	pstMain = &m_pstModResult[i];
				else
				{
					/* ���� �˻��� ��ũ�� ��. (Scale�θ� ��. Score�� ����) ���� */
					/* �˻��� ��ũ�� �ѷ��ؾ� �ϱ� ������, scale�� ���� ������ �켱 */
					/* ���� 2�� �� ������ �Ǿ� �ִٺ���, score���� scale�� ���� ������ ������ */
					if (pstMain->scale_rate < m_pstModResult[i].scale_rate)
					{
						pstMain->valid_multi = 0x00;		/* ���� ���� ��ȿ���� �ʴٰ� �÷��� ���� */
						pstMain = &m_pstModResult[i];	/* ���ο� Mark ������ ���� */
					}
				}
			}
		}
	}
#if 0	/* �ϴ� ��ũ �˻� �ӵ� ���� ������, �ϴ� �ҽ����� ���� ó�� */
	/* �˻��� �ܰ� ��ũ���� ������ �ּ����� �ڽ��� ������ �̻� (�߽� ���� �Ÿ�) ������ �ִ��� Ȯ�� */
	if (ENG_MMSM::en_ring_circle != m_enMarkMethod)
	{
		for (i = 0x00; i < m_u8MarkFindGet; i++)
		{
			/* ���� ��ȿ�� ��츸 �˻� */
			if (!m_pstModResult[i].IsValid())	continue;
			/* ��ũ �߽ɿ� �ִ� ���� �˻����� ���� */
			if (ENG_MMSM::en_cent_side == m_enMarkMethod &&
				m_pstModResult[i].model_index == 0x00)	continue;
			/* �߽� ��ũ �̿��� �˻��� ��ũ�� ���� ������ ���� */
			for (j = i + 1; j < m_u8MarkFindGet; j++)
			{
				/* ���� ��ȿ�� ��츸 �˻� */
				if (!m_pstModResult[j].IsValid())	continue;
				/* ��ũ �߽ɿ� �ִ� ���� �˻����� ���� */
				if (ENG_MMSM::en_cent_side == m_enMarkMethod &&
					m_pstModResult[i].model_index == 0x00)	continue;
				/* 2���� ��ũ�� �߽� ���� ������ �Ÿ� (Pixel) ���ϱ� */
				dbDistCent = sqrt(pow(m_pstModResult[j].cent_x - m_pstModResult[i].cent_x, 2) +
					pow(m_pstModResult[j].cent_y - m_pstModResult[i].cent_y, 2));
				/* �ش� ���� ũ�� */
				dbMarkSize = m_pstModResult[j].GetMarkSize() > m_pstModResult[i].GetMarkSize() ?
					m_pstModResult[j].GetMarkSize() : m_pstModResult[i].GetMarkSize();
				/* 2���� �˻��� ��ũ�� �߽� �Ÿ��� ���� ������ ũ�⺸�� �����ٸ� 1���� ������ */
				/* 1���� ���� ����, 2�� �� ���� ���� ���� (score)�� ���ܵΰ�, �������� ������ */
				if (dbMarkSize / 2.0f > dbDistCent)
				{
					/* ���� ���� ���� �˻� */
					k = m_pstModResult[j].score_rate > m_pstModResult[i].score_rate ? i : j;
					m_pstModResult[k].valid_multi = 0x00;
				}
			}
		}
	}
#endif

	/* �� ��ũ �˻� ���ǿ� ����, �˻��� ��ũ�� ��ȿ���� �ٽ� �ѹ� �˻� ���� */
	switch (m_enMarkMethod) 
	{
	case ENG_MMSM::en_cent_side:	bSucc = CalcMarkCentSide(stCent, dbSquareRate, stDist);	break;	/* ����ġ�� ����� �߽� �� ��ȯ */
	case ENG_MMSM::en_multi_only:	bSucc = CalcMarkMultiOnly(stCent, dbSquareRate);		break;
	case ENG_MMSM::en_ring_circle:	bSucc = CalcMarkRingCircle(stCent, stDist);				break;
	}

	/* �簢�� ���� �� ���� */
	m_stModResult.square_rate = dbSquareRate;

	if (bSucc)
	{
		/* �˻��� ��ũ �� ��ȿ�� �� �������� ��� �˻��� ���ϱ� ���� */
		m_stModResult.scale_rate = DBL_MIN;
		m_stModResult.score_rate = DBL_MIN;
		m_stModResult.r_angle = DBL_MAX;
		m_stModResult.diameter = DBL_MAX;
		/* ��� Ȥ�� �ּ� �� ���ϱ� ���� */
		for (i = 0x00/*,j=0x00*/; i < m_u8MarkFindGet; i++)
		{
#if 0	/* ModDefine ���� �̹� �˻� �����Ƿ�, ���⼭ �� �ʿ� ���� */
			if (!m_pstModResult[i].IsValid())	continue;
#endif
#if (USE_MIXED_MARK_CALC_AVERAGE == 0)	/* �˻��� �� �߿��� �ּ� or �ִ� ���� �����ϱ� ���� */
			if (m_stModResult.scale_rate < m_pstModResult[i].scale_rate)
				m_stModResult.scale_rate = m_pstModResult[i].scale_rate;
			if (fabs(m_stModResult.score_rate) < fabs(m_pstModResult[i].score_rate))
				m_stModResult.score_rate = m_pstModResult[i].score_rate;
			if (fabs(m_stModResult.r_angle) > fabs(m_pstModResult[i].r_angle))
				m_stModResult.r_angle = m_pstModResult[i].r_angle;
			if (fabs(m_stModResult.diameter) > fabs(m_pstModResult[i].diameter))
				m_stModResult.diameter = m_pstModResult[i].diameter;
#else	/* ��� ���� ���ϱ� ���� */
			m_stModResult.scale_range += m_pstModResult[i].scale_range;
			m_stModResult.score_rate += m_pstModResult[i].score_rate;
			m_stModResult.r_angle += m_pstModResult[i].r_angle;
			m_stModResult.radius += m_pstModResult[i].radius;
#endif
			/*j++;*/
		}
		if (m_u8MarkFindGet/*j*/)
		{
#if (USE_MIXED_MARK_CALC_AVERAGE == 1)	/* ��� ���� ���ϱ� ���� */
			m_stModResult.radius /= DOUBLE(j);
			m_stModResult.scale_range /= DOUBLE(j);
			m_stModResult.score_rate /= DOUBLE(j);
			m_stModResult.r_angle /= DOUBLE(j);
#endif
			/* Mark Size (�˻��� �߾��� Mark) */
			m_stModResult.mark_width = (UINT32)ROUNDED(m_pstModelSize[0].x, 0);
			m_stModResult.mark_height = (UINT32)ROUNDED(m_pstModelSize[0].y, 0);
			/* �߽� ��ġ �� ����  */
			m_stModResult.cent_x = stCent.x;
			m_stModResult.cent_y = stCent.y;
		}
#if 0	/* RunFindModel ���� �̹� �˻� �� �˻��� ������ ���� �����Ƿ�, ���⼭ �� �ʿ� ���� */
		/* ���� ���������� �˻��� ���� �� ���� */
		m_u8MarkFindGet = j;
#endif
		/* �ܺ� ��ũ�� ���� �߽� ��ũ ���� ���� �߽� ���� �Ÿ� ���� �� ���ϱ� */
		if (m_enMarkMethod != ENG_MMSM::en_multi_only)
		{
			/* �߽� ��ũ ���� �Ÿ� �� ���� */
			m_stModResult.cent_dist_x = (UINT32)ROUNDED((stDist.x * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1)) * 1000.0f, 0);
			m_stModResult.cent_dist_y = (UINT32)ROUNDED((stDist.y * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1)) * 1000.0f, 0);
			/* ȯ�� ���Ͽ� ������ �ּ� ��ȿ �߽� �� ���� �Ÿ� ������ ������ �߽� �� ���� ���� ũ�� ���� ó�� */
			/* �߽� ���� ���� (X) ���� ���� (Y)�� ���̰� �־��� ��ȿ �߽� ���� �Ÿ� �� ���� ũ�� ���� ó�� */
			if (m_stModResult.GetCentDistLen() / 1000.0f > pstMarkFind->mixed_dist_limit)
			{
				swprintf_s(tzMesg, LOG_MESG_SIZE,
					L"After searching for a multi-mark : "
					L"the distance from the center is far away"
					L"set dist (%.3f mm) / get_dist_xy (%.3f mm)",
					pstMarkFind->mixed_dist_limit,
					m_stModResult.GetCentDistLen() / 1000.0f);
				LOG_ERROR(ENG_EDIC::en_mil, tzMesg);
				bSucc = FALSE;
			}
		}
#if 0	/* �̹� RunModelFind �Լ��� �˻� �����Ƿ�, ���⼭ �� �ʿ� ���� */
		/* --------------------------------------------------- */
		/* ã���� �ϴ� �������� ���� ������ŭ ������ ���� ��� */
		/* �ּ��� 2 �� ���� ��� ������, �� �̻� ������ ���� */
		/* --------------------------------------------------- */
		if (bSucc && (m_u8MarkFindGet != m_u8MarkFindSet))
		{
			swprintf_s(tzMesg, LOG_MESG_SIZE,
				L"Not all were found (FindMarkCentSide) : search (%u) / found (%u)",
				m_u8MarkFindSet, m_u8MarkFindGet);
			LOG_WARN(ENG_EDIC::en_mil, tzMesg);
			bSucc = FALSE;
		}
#endif
	}

	/* ----------------------------------------------------------------------------------------- */
	/*                        Grabbed Image�� ����� ��ȿ���� �÷��� ����                        */
	/* ----------------------------------------------------------------------------------------- */
	if (bSucc && (ENG_MMSM::en_ring_circle != m_enMarkMethod))
	{
		/* �⺻ ��ũ�� �ֺ� ��ũ���� ������ �˻� ����� ��츸 �ش�� */
		m_stModResult.valid_multi = UINT32(pstMarkFind->mixed_squre_rate * 100.0f) <
			UINT32(ROUNDED(dbSquareRate * 100.0f, 0));
		if (!m_stModResult.valid_multi)
		{
			swprintf_s(tzMesg, LOG_MESG_SIZE,
				L"Invalid square rate : set_rate (%u %%) / cur_rate (%.2f %%)",
				pstMarkFind->mixed_squre_rate, dbSquareRate);
			LOG_ERROR(ENG_EDIC::en_mil, tzMesg);
			bSucc = FALSE;
		}
	}

	/* ���� �˻��� ����� ����� ������ ��ȯ */
	return bSucc ? &m_stModResult : NULL;
}

/*
 desc : ����ġ ��� �˰���
 parm : hole	- [in]  ���� (HOLE) �߽� ��ǥ (X or Y)
		laser	- [in]  �ֺ� (Laser) �߽� ��ǥ (X or Y)
 retn : ���� ����ġ�� �ο��� �� (��ǥ: x or y) ��ȯ
*/
DOUBLE CMilModel::GetWeightRateToErrLen(DOUBLE hole, DOUBLE laser)
{
	return	((hole - laser) * (m_pstConfig->mark_find.mixed_weight_rate / 10000.0f));
}

/*
 desc : �˻��� ����(����) ����� ��ũ�� ���� �߽� ��ǥ ���ϱ� (��, �簢�� ������ �߽�)
 parm : cent	- [out] �߽� ��ǥ�� ����� (�� X/Y �ּ�, �ִ� ��ǥ ���ϰ�, �� �߽��� ��ȯ)
		rate	- [out] �ֺ� ��ǥ�� �� Min / Max�� �簢���� ���� ���� ��, ����/���� ���� ��
		dist	- [out] �߽� ��ũ�� �ֺ� (Side) ��ũ���� �߽� ���� ������ �Ÿ� ��ȯ
 retn : TRUE or FALSE
 note : ������ �簢���� ����/���� ������ ȯ�� ������ �־��� �� ��� ��ȿ���� ���ε� �Ǵ�
*/
BOOL CMilModel::CalcMarkCentSide(STG_DBXY& cent, DOUBLE& rate, STG_DBXY& dist)
{
	UINT8 i = 0, k = 0, j = 0xff;
	DOUBLE dbArea = 0.0f;
	STG_DBXY stCircle = { NULL }, stRect = { NULL }, stCent = { NULL };
	STG_DBXY stMin = { DBL_MAX, DBL_MAX }, stMax = { DBL_MIN, DBL_MIN };

	/* ��ȯ �� �ʱ�ȭ */
	cent.x = cent.y = 0.0f;
	rate = 0.0f;

	for (; i < m_u8MarkFindGet; i++)
	{
		/* ���� ��ȿ�� �ͺ��� ���͸��ϰ�, Model�� ù��°�� ���� */
		if (m_pstModResult[i].IsValid())
		{
			if (0 != m_pstModResult[i].model_index)
			{
				/* �˻��� Mark ���� �� ��ǥ�� ���� ū ���� ���� �� ���� */
				if (stMin.x > m_pstModResult[i].cent_x)	stMin.x = m_pstModResult[i].cent_x;
				if (stMin.y > m_pstModResult[i].cent_y)	stMin.y = m_pstModResult[i].cent_y;
				if (stMax.x < m_pstModResult[i].cent_x)	stMax.x = m_pstModResult[i].cent_x;
				if (stMax.y < m_pstModResult[i].cent_y)	stMax.y = m_pstModResult[i].cent_y;
				k++;
			}
			else
			{
				stCircle.x = m_pstModResult[i].cent_x;
				stCircle.y = m_pstModResult[i].cent_y;
				j = i;
			}
		}
	}
	/* �˻��� ��� ������ 4�� �����̰ų�, �߽� Mark�� ��ȿ���� ������ ���� ó�� */
	if (k < 4 || 0xff == j /* Failed to search */)	return FALSE;
	/* �簢���� ���� / ���� ���� ���� ȯ�� ���Ͽ� ������ ���� ��ȿ���� ���� */
	/* �簢���� �󸶳� ���簢������ ���ο� ������ ũ�� ���� �� ��� �� ��ȯ */
	if ((stMax.x - stMin.x) > (stMax.y - stMin.y))	rate = (stMax.y - stMin.y) / (stMax.x - stMin.x);
	else											rate = (stMax.x - stMin.x) / (stMax.y - stMin.y);
	rate *= 100.0f;

	/* ���簢���� �߽� ��ǥ ���ϱ� */
	if (0x00 == m_pstConfig->mark_find.mixed_cent_type)
	{
		/* ���簢���� �߽� ��ǥ ���ϱ� */
		stRect.x = stMin.x + (stMax.x - stMin.x) / 2.0f;
		stRect.y = stMin.y + (stMax.y - stMin.y) / 2.0f;
		/* �־��� ����ġ (����)�� �̿��Ͽ� Hole �� Laser�� �󸶳� ���� ������ Ȯ�� */
		dist.x = GetWeightRateToErrLen(stCircle.x, stRect.x);	/* (Hole.x - Laser.x) * weight_rate = Hole���� Laser������ �̵��� �� */
		dist.y = GetWeightRateToErrLen(stCircle.y, stRect.y);	/* (Hole.y - Laser.y) * weight_rate = Hole���� Laser������ �̵��� �� */
	}
	/* �ٰ����� ���� �߽� ��ǥ ���ϱ� */
	else
	{
		CalcPolyWeightCent(stCent.x, stCent.y, dbArea);
		/* �־��� ����ġ (����)�� �̿��Ͽ� Hole �� Laser�� �󸶳� ���� ������ Ȯ�� */
		dist.x = GetWeightRateToErrLen(stCircle.x, stCent.x);	/* (Hole.x - Laser.x) * weight_rate = Hole���� Laser������ �̵��� �� */
		dist.y = GetWeightRateToErrLen(stCircle.y, stCent.y);	/* (Hole.y - Laser.y) * weight_rate = Hole���� Laser������ �̵��� �� */
	}

	/* ------------------------------------------------------- */
	/* �ܰ� Mark�� ���� Mark ���� �߽� ���� ��� (����ġ �ο�) */
	/* Hole ��ġ�� (Hole - Laser) * Weight_Rate = �� ��ŭ ���� */
	/* ------------------------------------------------------- */
	cent.x = stCircle.x - dist.x;
	cent.y = stCircle.y - dist.y;

	/* �ֺ� (Side) ��ũ���� �߽ɰ� �߽� ��ũ�� �߽� ���� �Ÿ� ���ϱ� */
	/* ���簢���� �߽� ��ǥ ���ϱ� */
	if (0x00 == m_pstConfig->mark_find.mixed_cent_type)
	{
		dist.x = (stCircle.x - stRect.x) - dist.x;
		dist.y = (stCircle.y - stRect.y) - dist.y;
	}
	/* �ٰ����� ���� �߽� ��ǥ ���ϱ� */
	else
	{
		dist.x = (stCircle.x - stCent.x) - dist.x;
		dist.y = (stCircle.y - stCent.y) - dist.y;
	}
	return TRUE;
}

/*
 desc : �˻��� ����(����) ����� ��ũ�� ���� ���� �߽� ��ǥ ���ϱ�
		�ܰ��� ��(����) ��ũ�� ���ο� �� (Circle)�� ������ ��ũ �˻�
 parm : cent	- [out] �߽� ��ǥ�� �����
		dist	- [out] �߽� ��ũ�� �ֺ� (Side) ��ũ���� �߽� ���� ������ �Ÿ� ��ȯ
 retn : TRUE or FALSE
*/
BOOL CMilModel::CalcMarkRingCircle(STG_DBXY& cent, STG_DBXY& dist)
{
	UINT8 i = 0x00;
	STG_DBXY stIn, stOut;

	/* ��ȯ �� �ʱ�ȭ */
	cent.x = cent.y = 0.0f;

	/* �˻��� Mark�� 2�� ����, 2���� �� �ε��� ���� ���Ƶ� �ȵǰ�, ���� ��� ��ȿ�ؾ� �� */
	if (m_u8MarkFindGet != 2 ||
		m_pstModResult[0].model_index == m_pstModResult[1].model_index ||
		!m_pstModResult[0].IsValid() || !m_pstModResult[1].IsValid())	return FALSE;

	/* ���� ���� �ܺ� ���� �߽� ��ǥ */
	for (; i < 2; i++)
	{
		/* ���� �� ���� ����  */
		if (0 == m_pstModResult[i].model_index)
		{
			stIn.x = m_pstModResult[i].cent_x;
			stIn.y = m_pstModResult[i].cent_y;
		}
		/* �ܺ� �� ���� ���� */
		else
		{
			stOut.x = m_pstModResult[i].cent_x;
			stOut.y = m_pstModResult[i].cent_y;
		}
	}

	/* Circle�� �߽ɰ� Ring ��ũ�� �߽� ���� �Ÿ� ���ϱ� */
	/* �־��� ����ġ (����)�� �̿��Ͽ� Hole �� Laser�� �󸶳� ���� ������ Ȯ�� */
	dist.x = GetWeightRateToErrLen(stIn.x, stOut.x);	/* (Hole.x - Laser.x) * weight_rate = Hole���� Laser������ �̵��� �� */
	dist.y = GetWeightRateToErrLen(stIn.y, stOut.y);	/* (Hole.y - Laser.y) * weight_rate = Hole���� Laser������ �̵��� �� */
	/* ------------------------------------------------------- */
	/* �ܰ� Mark�� ���� Mark ���� �߽� ���� ��� (����ġ �ο�) */
	/* ------------------------------------------------------- */
	/* �ܰ� Mark �������� ����ġ�� �ο��ϱ� ������ */
	cent.x = stIn.x - dist.x;
	cent.y = stIn.y - dist.y;

	/* �ֺ� (Side) ��ũ���� �߽ɰ� �߽� ��ũ�� �߽� ���� �Ÿ� ���ϱ� */
	dist.x = (stIn.x - stOut.x) - dist.x;
	dist.y = (stIn.y - stOut.y) - dist.y;

	return TRUE;
}

/*
 desc : �˻��� ����(����) ����� ��ũ�� ���� ���� �߽� ��ǥ ���ϱ�
		�ֺ� (2x2 or 3x3 or 4x4)�� ������ ��� (Shape)���� ������ ���� (���� �ƴ�) ����� ��ũ �˻�
 parm : cent	- [out] �߽� ��ǥ�� �����
		rate	- [out] �ֺ� ��ǥ�� �� Min / Max�� �簢���� ���� ���� ��, ����/���� ���� ��
 retn : TRUE or FALSE
 note : ������ �簢���� ����/���� ������ ȯ�� ������ �־��� �� ��� ��ȿ���� ���ε� �Ǵ�
*/
BOOL CMilModel::CalcMarkMultiOnly(STG_DBXY& cent, DOUBLE& rate)
{
	UINT8 i = 0, j = 0;
	DOUBLE dbArea = 0.0f;
	STG_DBXY stMin = { DBL_MAX, DBL_MAX }, stMax = { DBL_MIN, DBL_MIN };

	/* ��ȯ �� �ʱ�ȭ */
	cent.x = cent.y = 0.0f;
	rate = 0.0f;

	/* �簢�� ������ Boundary �� ���ϱ� */
	for (; i < m_u8MarkFindGet; i++)
	{
		/* ���� ���� ��ȿ���� ���� */
		if (m_pstModResult[i].IsValid())
		{
			/* �˻��� Mark ���� �� ��ǥ�� ���� ū ���� ���� �� ���� */
			if (stMin.x > m_pstModResult[i].cent_x)	stMin.x = m_pstModResult[i].cent_x;
			if (stMin.y > m_pstModResult[i].cent_y)	stMin.y = m_pstModResult[i].cent_y;
			if (stMax.x < m_pstModResult[i].cent_x)	stMax.x = m_pstModResult[i].cent_x;
			if (stMax.y < m_pstModResult[i].cent_y)	stMax.y = m_pstModResult[i].cent_y;
			j++;
		}
	}

	/* �˻��� ��� ������ 4�� �����̰ų�, �߽� Mark�� ��ȿ���� ������ ���� ó�� */
	if (j < 4 || 0xff == j /* Failed to search */)
	{
		LOG_ERROR(ENG_EDIC::en_mil,
			L"The number of coordinates required for multi-mark configuration is too small");
		return FALSE;
	}
	/* �簢���� ���� / ���� ���� ���� ȯ�� ���Ͽ� ������ ���� ��ȿ���� ���� */
	/* �簢���� �󸶳� ���簢������ ���ο� ������ ũ�� ���� �� ��� �� ��ȯ */
	if ((stMax.x - stMin.x) > (stMax.y - stMin.y))	rate = (stMax.y - stMin.y) / (stMax.x - stMin.x);
	else											rate = (stMax.x - stMin.x) / (stMax.y - stMin.y);
	rate *= 100.0f;

	/* ���簢���� �߽� ��ǥ ���ϱ� */
	if (0x00 == m_pstConfig->mark_find.mixed_cent_type)
	{
		cent.x = stMin.x + (stMax.x - stMin.x) / 2.0f;
		cent.y = stMin.y + (stMax.y - stMin.y) / 2.0f;
	}
	/* �ٰ����� ���� �߽� ��ǥ ���ϱ� */
	else if (0x01 == m_pstConfig->mark_find.mixed_cent_type)
	{
		CalcPolyWeightCent(cent.x, cent.y, dbArea);
	}
	/* ������ ��ȯ (�밢�� ������?)�� ���� �߽� ��ǥ ���ϱ� */
	else
	{
		DOUBLE* dbX, * dbY;
		dbX = new DOUBLE[m_u8MarkFindGet];
		dbY = new DOUBLE[m_u8MarkFindGet];

		for (i = 0; i < m_u8MarkFindGet; i++)
		{
			dbX[i] = m_pstModResult[i].cent_x;
			dbY[i] = m_pstModResult[i].cent_y;
			uvCalc_GetCoordsToCicleCent(m_u8MarkFindGet, dbX, dbY, 0x00/*0x01, 0x02, 0x03*/, cent.x, cent.y);
		}

		delete[] dbX;
		delete[] dbY;
	}

	return TRUE;
}

/*
 desc : �˻��� ��ũ �������� �ٰ��� (��)���� ������ ��ǥ�鿡 ���� ���� �߽� ��ǥ ���ϱ�
 parm : cent_x	- [out] ���� �߽� ��ǥ �� ��ȯ
		cent_y	- [out] ���� �߽� ��ǥ �� ��ȯ
		area	- [out] �ٰ����� ���� �� ��ȯ
 retn : None
*/
VOID CMilModel::CalcPolyWeightCent(DOUBLE& cent_x, DOUBLE& cent_y, DOUBLE& area)
{
	UINT8 i, j;
	DOUBLE /*dbX1, dbY1, dbX2, dbY2,*/* dbArrX, * dbArrY;

	/* �ӽ� ���� �Ҵ� */
	dbArrX = (DOUBLE*)::Alloc(sizeof(DOUBLE) * m_u8MarkFindGet);
	dbArrY = (DOUBLE*)::Alloc(sizeof(DOUBLE) * m_u8MarkFindGet);
	ASSERT(dbArrX && dbArrY);
	for (i = 0; i < m_u8MarkFindGet; i++)
	{
		dbArrX[i] = m_pstModResult[i].cent_x;
		dbArrY[i] = m_pstModResult[i].cent_y;
	}
	/* ���� �۾� ���� */
	uvCalc_GetSortCoordPoints(dbArrX, dbArrY, UINT32(m_u8MarkFindGet));

	/* ��ȯ �� �ݵ�� �ʱ�ȭ */
	cent_x = cent_y = area = 0.0f;

	for (i = 0; i < m_u8MarkFindGet; i++)
	{
		j = (i + 1) % m_u8MarkFindGet;
#if 0
		dbX1 = dbArrX[i];
		dbY1 = dbArrY[i];
		dbX2 = dbArrX[j];
		dbY2 = dbArrY[j];
#endif
#if 0
		/* ���� ���� ��ȿ���� ���� */
		if (m_pstModResult[i].IsValid())
#endif
		{
			area += dbArrX[i] * dbArrY[j];
			area -= dbArrY[i] * dbArrX[j];

			cent_x += ((dbArrX[i] + dbArrX[j]) * ((dbArrX[i] * dbArrY[j]) - (dbArrX[j] * dbArrY[i])));
			cent_y += ((dbArrY[i] + dbArrY[j]) * ((dbArrX[i] * dbArrY[j]) - (dbArrX[j] * dbArrY[i])));
		}
	}

	/* �޸� ���� */
	::Free(dbArrX);
	::Free(dbArrY);

	/* ���� ���ϱ� */
	area /= 2.0f;
	/* ���� �߽� ��ǥ ���ϱ� */
	cent_x = cent_x / (6.0f * area);	/* �� 6.0�� ���ϴ��� ?*/
	cent_y = cent_y / (6.0f * area);	/* �� 6.0�� ���ϴ��� ?*/
	/* ������ ������ ��� ���̹Ƿ� ... */
	area = fabs(area);
}

/*
 desc : �˻��� ��� ��ȯ
 parm : None
 retn : �˻��� ����� ����� ������ ��ȯ
*/
LPG_GMFR CMilModel::GetFindMark()
{
	switch (m_enMarkMethod)
	{
	case ENG_MMSM::en_single:	/* �˻��� ��ũ �� ���� ����� ���� Mark�� �˻��Ͽ� ��� ��ȯ */
		return GetFindMarkBestScale();
	case ENG_MMSM::en_cent_side:	/* �߽��� Mark 1���� �ֺ��� ��ũ ���� �� ���� ����ġ�� �ο��Ͽ� ���� ����� ��ȯ */
	case ENG_MMSM::en_ring_circle:	/* �ܰ��� ��(����) ��ũ�� ���ο� �� (Circle)�� ������ ��ũ �˻��ϴ� ��� */
	case ENG_MMSM::en_multi_only:	/* �߽ɿ��� Mark�� ���� �ֺ��� ��ũ ���� �� ���� ���� �߽��� ���� ����� ��ȯ */
		return GetFindMarkCentSide();
	}

	return NULL;
}

/*
 desc : �˻��ϱ� ���� �Էµ� ������ ���� �˻��� ������ ���� ���� �� ���
 parm : find	- [in]  �˻��� ���� ��
		input	- [in]  �˻��ϱ� ���� �Էµ� ���� ��
 retn : ���� ��
*/
DOUBLE CMilModel::CalcAngleDist(DOUBLE find, DOUBLE input)
{
	DOUBLE dbDist = fabs(find - input);

	while (dbDist >= 360.0f)	dbDist -= 360.0f;
	if (dbDist > 180.0f)		dbDist = 360.0f - dbDist;

	return dbDist;
}

/*
 desc : �� ���� ��ǥ ���� ������ �Ÿ� ���ϱ�
 parm : x1,y1	- [in]  ù ��° ���� ��ǥ ��ġ
		x2,y2	- [in]  �� ��° ���� ��ǥ ��ġ
 retn : ������ �Ÿ� ��ȯ
*/
DOUBLE CMilModel::CalcLineDist(DOUBLE x1, DOUBLE y1, DOUBLE x2, DOUBLE y2)
{
	return sqrt(pow(x2 - x1, 2.0f) + pow(y2 - y1, 2.0f));
}

#ifndef _NOT_USE_MIL_
/*
 desc : Edge Detection ����
 parm : grab_id	- [in]  Grabbed Image ������ ����� ID (Target Image (Buffer))
		width	- [in]  Grabbed Image - Width (Pixel)
		height	- [in]  Grabbed Image - Width (Pixel)
		saved	- [in]  �м��� edge image ���� ����
 retn : TRUE or FALSE
*/
BOOL CMilModel::RunEdgeDetect(MIL_ID grab_id, UINT32 width, UINT32 height, UINT8 saved)
{
	BOOL bFinded = FALSE;
	INT32 i, u32Find = 0;	/* ���������� ã�� ���� */
	DOUBLE dbMmToPixel = 0.0f;
	MIL_ID mlEdgeContext = M_NULL;
	MIL_ID mlEdgeResult = M_NULL;
	MIL_INT miEdgeResults = 0;
	MIL_INT miEnable[2] = { M_DISABLE, M_ENABLE };
	MIL_INT32 miFilterType[5] = { M_SHEN, M_DERICHE, M_PREWITT, M_SOBEL, M_FREI_CHEN };
	MIL_INT32 miThreshold[5] = { M_VERY_HIGH, M_HIGH, M_MEDIUM, M_LOW, M_DISABLE };
	MIL_DOUBLE* mdEdgeDiameter = NULL;
	MIL_DOUBLE* mdEdgeRadius = NULL;
	MIL_DOUBLE* mdEdgeCircleX = NULL;
	MIL_DOUBLE* mdEdgeCircleY = NULL;
	MIL_DOUBLE* mdEdgeBoxMinX = NULL;
	MIL_DOUBLE* mdEdgeBoxMinY = NULL;
	MIL_DOUBLE* mdEdgeBoxMaxX = NULL;
	MIL_DOUBLE* mdEdgeBoxMaxY = NULL;
	MIL_DOUBLE* mdEdgeStrength = NULL;
	MIL_DOUBLE* mdEdgeLength = NULL;
	/* �˻� ���� ���� ��� */
	LPG_CMED pstEdge = &m_pstConfig->edge_find;

	/* ���� ��� �� �ʱ�ȭ */
	m_i32EdgeFindGet = 0x00;
	/* Allocate a Edge Finder context. */
	mlEdgeContext = MedgeAlloc(theApp.clMilMain.m_mSysID, M_CONTOUR, M_DEFAULT, NULL);
	/* Allocate a result buffer. */
	mlEdgeResult = MedgeAllocResult(theApp.clMilMain.m_mSysID, M_DEFAULT, NULL);
	/* ----------------------------------------------------------------------------------------- */
	/*                                       Set Flag                                            */
	/* ----------------------------------------------------------------------------------------- */
	MedgeControl(mlEdgeContext, M_ACCURACY, pstEdge->m_accuracy);	/*3 (Default:High), 4 (Very High)*/
//	MedgeControl(mlEdgeContext,	M_CHAIN_ALL_NEIGHBORS,				M_ENABLE);				/* ��� �̿� Point�� �����Ͽ� Edge ���� */
	MedgeControl(mlEdgeContext, M_MOMENT_ELONGATION, M_ENABLE);
	MedgeControl(mlEdgeContext, M_FERET_MEAN_DIAMETER + M_SORT1_UP, M_ENABLE);
	MedgeControl(mlEdgeContext, M_BOX_X_MAX, M_ENABLE);	/* ���� ���� ���� */
	MedgeControl(mlEdgeContext, M_BOX_X_MIN, M_ENABLE);	/* ���� ���� ���� */
	MedgeControl(mlEdgeContext, M_BOX_Y_MAX, M_ENABLE);	/* ���� ���� ���� */
	MedgeControl(mlEdgeContext, M_BOX_Y_MIN, M_ENABLE);	/* ���� ���� ���� */
	MedgeControl(mlEdgeContext, M_CIRCLE_FIT_CENTER_X, M_ENABLE);	/* �� ���� ���� �߽� �� */
	MedgeControl(mlEdgeContext, M_CIRCLE_FIT_CENTER_Y, M_ENABLE);	/* �� ���� ���� �߽� �� */
	MedgeControl(mlEdgeContext, M_CIRCLE_FIT_RADIUS, M_ENABLE);
	MedgeControl(mlEdgeContext, M_AVERAGE_STRENGTH, M_ENABLE);
	MedgeControl(mlEdgeContext, M_LENGTH, M_ENABLE);	/* ���� ������ ������ �� ���� */
	MedgeControl(mlEdgeContext, M_FILTER_SMOOTHNESS, DOUBLE(pstEdge->filter_smoothness));	/* 0 ~ 100���� �����ϸ�, 100�� ����� ���� ���� ������ �� �͸� ������ �� ���� */
	/* ----------------------------------------------------------------------------------------- */
	/*                                       Set Value                                           */
	/* ----------------------------------------------------------------------------------------- */
	MedgeControl(mlEdgeContext, M_THRESHOLD_MODE, miThreshold[pstEdge->threshold - 1]);
	/* SHEN		: �⺻ ���ͷμ�, ���������� �̻����̸�, �ſ� ���� ��ġ�� Edge�� �����ϴµ� ȿ����������, */
	/*			  ������ ����� ���� ����ġ �ʰ� �ΰ��ϰų� ������������ �β��� Crest�� �����ϴµ� ����*/
	/*			  ���� ����� ���� �� �ִ�.                                                              */
	/* DERICHE	: Shen���� Edge�� �̿��� �� ������ �ΰ� �ֱ� ������, �̷� ������ ���� Deriche�� ����ؾ� */
	MedgeControl(mlEdgeContext, M_FILTER_TYPE, miFilterType[pstEdge->filter_type - 1]);
	/* Sets whether to force all the processing of edge extraction operations to be perfrometd using floating-point calculations */
	MedgeControl(mlEdgeContext, M_FLOAT_MODE, miEnable[pstEdge->use_float_mode]);
	/* ���� ���� �� ������ �� �� ���� ���� ������ ��(0�� ����� ����) ���� ���ἱ�� ���� */
	/* ���� ��� ������ ���� (100�� ����� ����) ���� ���ἱ�� �������� ���� ���� ���� */
	if (pstEdge->fill_gap_continuity > 0.0f)	MedgeControl(mlEdgeContext, M_FILL_GAP_CONTINUITY, pstEdge->fill_gap_continuity);
	/* The default value is 1.0f */
	if (pstEdge->extraction_scale > 0.0f)		MedgeControl(mlEdgeContext, M_EXTRACTION_SCALE, pstEdge->extraction_scale);
	/* ȯ�� ������ �ɼ� ���� ���� ���� ���� ���� */
	if (pstEdge->use_float_mode)				MedgeControl(mlEdgeContext, M_FLOAT_MODE, M_ENABLE);
	if (pstEdge->use_closed_line)				MedgeControl(mlEdgeContext, M_CLOSURE, M_ENABLE);

	/* Calculate edges and features. */
	MedgeCalculate(mlEdgeContext, grab_id, M_NULL, M_NULL, M_NULL, mlEdgeResult, M_DEFAULT);
	/* Get the number of edges found. */
	MedgeGetResult(mlEdgeResult, M_DEFAULT, M_NUMBER_OF_CHAINS + M_TYPE_MIL_INT, &miEdgeResults, M_NULL);
	/* �˻��� �׸��� ������ ���� ���� */
	if (miEdgeResults < 1)
	{
		/* Free MIL objects. */
		MedgeFree(mlEdgeResult);
		MedgeFree(mlEdgeContext);
		return FALSE;
	}
	/* Exclude elongated edges. moment_elongation �� ���� ���� Edge�� �ɷ����� ��, */
	/* ���� ���� �� ���� ���� ����Ǵ� ������ �־��� �� (Max 1.0)���� ������ ����. */
	if (pstEdge->moment_elongation > 0.0f)	MedgeSelect(mlEdgeResult, M_DELETE, M_MOMENT_ELONGATION, M_LESS, pstEdge->moment_elongation, M_NULL);
	/* Line�� ������ �ʴ� �͸� ���� */
	if (pstEdge->use_closed_line)			MedgeSelect(mlEdgeResult, M_EXCLUDE, M_CLOSURE, M_NOT_EQUAL, M_TRUE, M_NULL);
#if 0
	/* ���� �ѷ� = ������ * 2.0 * 3.14 */
	/* �˻��� ��ü �� ������ ũ�Ⱑ "pstEdge->min_dia_size (mm)" ������ ���� ��� ���� */
	if (pstEdge->min_dia_size > 0)
	{
		dbMmToPixel = (pstEdge->min_dia_size / 2.0f) / m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1);
		MedgeSelect(mlEdgeResult, M_EXCLUDE, M_CIRCLE_FIT_RADIUS, M_LESS, dbMmToPixel, M_NULL);
	}
	/* �˻��� ��ü �� ������ ũ�Ⱑ "pstEdge->max_dia_size (mm)" �̻��� ���� ��� ���� */
	if (pstEdge->max_dia_size > 0)
	{
		dbMmToPixel = (pstEdge->max_dia_size / 2.0f) / m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1);
		MedgeSelect(mlEdgeResult, M_EXCLUDE, M_CIRCLE_FIT_RADIUS, M_GREATER, dbMmToPixel, M_NULL);
	}
#endif
	/* Exclude inner chains. (���Ե� ������ �� ���� �ڽ��� �ִ� �������� ���� */
	if (!pstEdge->inc_internal_edge)		MedgeSelect(mlEdgeResult, M_EXCLUDE, M_INCLUDED_EDGES, M_INSIDE_BOX, M_NULL, M_NULL);
	/* Get the number of edges found. */
	MedgeGetResult(mlEdgeResult, M_DEFAULT, M_NUMBER_OF_CHAINS + M_TYPE_MIL_INT, &miEdgeResults, M_NULL);
	/* �˻��� �׸��� ������ ���� ���� */
	if (miEdgeResults < 1)
	{
		/* Free MIL objects. */
		MedgeFree(mlEdgeResult);
		MedgeFree(mlEdgeContext);
		return FALSE;
	}
	/* �˻��� ����� 255�� �ʰ��� ���, 255���� ó���ϵ��� ���� */
	if (miEdgeResults > UINT16_MAX)	miEdgeResults = UINT16_MAX;
	/* ���� �Ҵ�� �� ���� */
	ResetEdgeObject();
	/* Edge Drawing */
	m_mlEdgeID = MbufAlloc2d(theApp.clMilMain.m_mSysID, width, height, 8 + M_UNSIGNED, M_IMAGE + M_PROC, M_NULL);
	/* �ݵ�� �ʱ�ȭ ����� �� (�� ������ theApp.clMilMain.m_mSysID���� �Ӽ� ������ ����� �����̹Ƿ�)  */
	/* �ʱ�ȭ ������ ������, Edge Detection �̹����� ���� �̹����� ��ø�Ǿ� ��Ÿ���� ��) */
	MbufClear(m_mlEdgeID, M_COLOR_BLACK);
	/* ���� �̹��� ���� */
	MbufCopy(grab_id, m_mlEdgeID);

	if (!m_mlEdgeID || MappGetError(M_GLOBAL, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"MIL_ERR : Failed to allocate <MappAlloc : Edge Buffer>");
		return FALSE;
	}
	MedgeDraw(M_DEFAULT, mlEdgeResult, m_mlEdgeID, M_DRAW_EDGE, M_DEFAULT, M_DEFAULT);
#if 0
	if (miEdgeResults < pstEdge->max_mark_find)
#endif
	{
		MedgeDraw(M_DEFAULT, mlEdgeResult, m_mlEdgeID, M_DRAW_INDEX, M_DEFAULT, M_DEFAULT);
		if (saved)
		{
			SYSTEMTIME stTm = { NULL };
			/* Grabbed Image �̹��� ���� ����*/
			TCHAR tzGrabFile[MAX_PATH_LEN] = { NULL };
			/* ���� ���� ���� �ð� ���� */
			GetLocalTime(&stTm);
			/* �ӽ� ���� ��, �����ִ� ��ũ ������ ������ ���� ���� ���� �� */
			swprintf_s(tzGrabFile, L"%s\\save_img\\edge\\edge_%04d%02d%02d_%02d%02d%02d.bmp",
				g_tzWorkDir, stTm.wYear, stTm.wMonth, stTm.wDay,
				stTm.wHour, stTm.wMinute, stTm.wSecond);
			MbufExport(tzGrabFile, M_BMP, m_mlEdgeID);
		}
	}
	/* Grabbed Image�� ��µ� ��� ������ �̹����� ���� �޸� ������ �ӽ� ���� */
	MbufGet2d(m_mlEdgeID, 0, 0, width, height, m_pGrabEdgeImage);

	/* �˻��� ��� ���� ������ ���� �Ҵ� */
	mdEdgeDiameter = (MIL_DOUBLE*)Alloc(sizeof(MIL_DOUBLE) * miEdgeResults);
	mdEdgeRadius = (MIL_DOUBLE*)Alloc(sizeof(MIL_DOUBLE) * miEdgeResults);
	mdEdgeCircleX = (MIL_DOUBLE*)Alloc(sizeof(MIL_DOUBLE) * miEdgeResults);
	mdEdgeCircleY = (MIL_DOUBLE*)Alloc(sizeof(MIL_DOUBLE) * miEdgeResults);
	mdEdgeBoxMinX = (MIL_DOUBLE*)Alloc(sizeof(MIL_DOUBLE) * miEdgeResults);
	mdEdgeBoxMinY = (MIL_DOUBLE*)Alloc(sizeof(MIL_DOUBLE) * miEdgeResults);
	mdEdgeBoxMaxX = (MIL_DOUBLE*)Alloc(sizeof(MIL_DOUBLE) * miEdgeResults);
	mdEdgeBoxMaxY = (MIL_DOUBLE*)Alloc(sizeof(MIL_DOUBLE) * miEdgeResults);
	mdEdgeStrength = (MIL_DOUBLE*)Alloc(sizeof(MIL_DOUBLE) * miEdgeResults);
	mdEdgeLength = (MIL_DOUBLE*)Alloc(sizeof(MIL_DOUBLE) * miEdgeResults);
	/* Sets whether to calculate the extreme left edgel coordinate of each edge. */
	MedgeGetResult(mlEdgeResult, M_DEFAULT, M_CIRCLE_FIT_RADIUS, mdEdgeRadius, M_NULL);
	MedgeGetResult(mlEdgeResult, M_DEFAULT, M_CIRCLE_FIT_CENTER_X, mdEdgeCircleX, M_NULL);
	MedgeGetResult(mlEdgeResult, M_DEFAULT, M_CIRCLE_FIT_CENTER_Y, mdEdgeCircleY, M_NULL);
	MedgeGetResult(mlEdgeResult, M_DEFAULT, M_BOX_X_MIN, mdEdgeBoxMinX, M_NULL);
	MedgeGetResult(mlEdgeResult, M_DEFAULT, M_BOX_Y_MIN, mdEdgeBoxMinY, M_NULL);
	MedgeGetResult(mlEdgeResult, M_DEFAULT, M_BOX_X_MAX, mdEdgeBoxMaxX, M_NULL);
	MedgeGetResult(mlEdgeResult, M_DEFAULT, M_BOX_Y_MAX, mdEdgeBoxMaxY, M_NULL);
	MedgeGetResult(mlEdgeResult, M_DEFAULT, M_AVERAGE_STRENGTH, mdEdgeStrength, M_NULL);
	MedgeGetResult(mlEdgeResult, M_DEFAULT, M_LENGTH, mdEdgeLength, M_NULL);
	/* Get the mean Feret diameters. */
	MedgeGetResult(mlEdgeResult, M_DEFAULT, M_FERET_MEAN_DIAMETER, mdEdgeDiameter, M_NULL);
	/* If the right number of edges were found. */
	for (i = 0x00; i < INT32(miEdgeResults); i++)
	{
		/* ���� ���̸�, ©�� �κ��� */
		if (mdEdgeCircleX[i] - mdEdgeRadius[i] >= 0 && mdEdgeCircleY[i] - mdEdgeRadius[i] >= 0)
		{
#if 0
			/* ������ �ִ� ũ�⺸�� ū ������� */
			if (pstEdge->arc_max_size > 0.0f &&
				mdEdgeDiameter[i] * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1) * 1000.0f > pstEdge->arc_max_size)	continue;
			/* ������ �ּ� ũ�⺸�� ���� ������� */
			if (pstEdge->arc_min_size > 0.0f &&
				mdEdgeDiameter[i] * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1) * 1000.0f < pstEdge->arc_min_size)	continue;
#endif
			/* ---------------------------------------------------------------------------------- */
			/* Edge ���� �ִ� / �ּ� �� ���� */
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].circle_area.left = (INT32)ROUNDUP(mdEdgeCircleX[i] - mdEdgeRadius[i], 0);// - pstEdgeDetect->area_spare_left;	/* spare pixel */
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].circle_area.top = (INT32)ROUNDUP(mdEdgeCircleY[i] - mdEdgeRadius[i], 0);// - pstEdgeDetect->area_spare_top;		/* spare pixel */
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].circle_area.right = (INT32)ROUNDUP(mdEdgeCircleX[i] + mdEdgeRadius[i], 0);// + pstEdgeDetect->area_spare_right;	/* spare pixel */
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].circle_area.bottom = (INT32)ROUNDUP(mdEdgeCircleY[i] + mdEdgeRadius[i], 0);// + pstEdgeDetect->area_spare_bottom;	/* spare pixel */
			/* Box ���� �ִ� / �ּ� �� ���� */
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].box_area.left = (INT32)ROUNDUP(mdEdgeBoxMinX[i], 0);
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].box_area.right = (INT32)ROUNDUP(mdEdgeBoxMaxX[i], 0);
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].box_area.top = (INT32)ROUNDUP(mdEdgeBoxMinY[i], 0);
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].box_area.bottom = (INT32)ROUNDUP(mdEdgeBoxMaxY[i], 0);
			/* ---------------------------------------------------------------------------------- */
			/* �������� ���� (�е�?) ��� �� */
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].strength = mdEdgeStrength[i];
			/* �������� ����� ���е��� �� ���� */
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].length = mdEdgeLength[i];
			/* ���� Ȥ�� ������ ����/���� ũ�� ���ϱ� (����: um) */
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].dia_meter_px = mdEdgeDiameter[i];
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].dia_meter_um = mdEdgeDiameter[i] * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1) * 1000.0f;
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].width_um = m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].GetBoxWidth() * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1) * 1000.0f;
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].height_um = m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].GetBoxHeight() * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1) * 1000.0f;
			/* Detecting ������ �����Ѵٰ� �÷��� ���� */
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].set_data = 0x01;
#if 0
			/* ã������ ������ �ʰ��Ǵ� ���, �� �̻� ã�� �ʰ� ���� ���� */
			if (++u32Find >= pstEdge->max_mark_find)	break;
#endif
		}
	}

	/* �˻� ���ǿ� �°� ã�� ���� ���� */
	m_i32EdgeFindGet = INT32(miEdgeResults);
	/* ���� ���� �̻� ã���� �÷��� �������� ���� */
	if (m_i32EdgeFindGet > 0)	bFinded = TRUE;

	/* ��� �� �޸� ���� */
	Free(mdEdgeDiameter);
	Free(mdEdgeRadius);
	Free(mdEdgeCircleX);
	Free(mdEdgeCircleY);
	Free(mdEdgeBoxMinX);
	Free(mdEdgeBoxMinY);
	Free(mdEdgeBoxMaxX);
	Free(mdEdgeBoxMaxY);
	Free(mdEdgeStrength);
	Free(mdEdgeLength);
	/* Free MIL objects. */
	MedgeFree(mlEdgeResult);
	MedgeFree(mlEdgeContext);

	return bFinded;
}


/*
 desc : �̹����� ���� Ȥ�� ������ ��輱 �߽� ��ġ ���
 parm : grab_id		- [in]  Grabbed Image ������ ����� ID (Target Image (Buffer))
		mil_result	- [in]  MIL ���� �˻� ��� ��ȭ ���� ��� ����
 retn : TRUE or FALSE
*/
BOOL CMilModel::RunLineCentXY(MIL_ID grab_id, BOOL mil_result)
{
	BOOL bFinded = FALSE;

	INT32 i32ScanWidth, i32ScanHeight, i32CentX, i32CentY, i32OffX, i32OffY;
	MIL_INT mlSizeX, mlSizeY;
	MIL_ID mlContextID, mlMarkerId, mlColorID;
	LPG_CLED pstLine = &m_pstConfig->line_find;

	/* ���� Line Edge Detection �̹����� �ӽ� ���� �޸� ���� */
	if (m_mlLineID)
	{
		MbufFree(m_mlLineID);
		m_mlLineID = M_NULL;
	}

	/* ���� Grabbed Image�� ���� ��� */
	mlSizeX = (INT32)MbufInquire(grab_id, M_SIZE_X, M_NULL);
	mlSizeY = (INT32)MbufInquire(grab_id, M_SIZE_Y, M_NULL);

	/* �̹��� �˻� ���� ���� */
	i32ScanWidth = (INT32)ROUNDED(mlSizeX * DOUBLE(pstLine->box_size_width / 100.0f), 0);
	i32ScanHeight = (INT32)ROUNDED(mlSizeY * DOUBLE(pstLine->box_size_height / 100.0f), 0);
	i32CentX = (INT32)ROUNDED(mlSizeX / 2.0f, 0);
	i32CentY = (INT32)ROUNDED(mlSizeY / 2.0f, 0);

	/* Allocation a measurement context */
	MmeasAllocContext(theApp.clMilMain.m_mSysID, M_DEFAULT, &mlContextID);
	if (!mlContextID || MappGetError(M_CURRENT, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"MIL_Err : Failed to allocate a measurement context (buffer)");
		return FALSE;
	}
	/* Allocate a measurement marker */
	mlMarkerId = MmeasAllocMarker(theApp.clMilMain.m_mSysID, M_EDGE, M_DEFAULT, M_NULL);
	if (!mlMarkerId || MappGetError(M_CURRENT, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"MIL_Err : Failed to allocate a measurement marker (buffer)");
		/* Release to Memory */
		MmeasFree(mlContextID);
		return FALSE;
	}

	/* Edge Line Detection �˻� ���� (�Ķ����) �� ���� */
	SetLineMarkerSetup(mlMarkerId, grab_id, i32ScanWidth, i32ScanHeight, i32CentX, i32CentY);
	/* !!! Find the marker and compute all applicable measurements !!! */
	MmeasFindMarker(mlContextID, grab_id, mlMarkerId, M_DEFAULT);

	/* �ӽ� RGB ���� �Ҵ� */
	mlColorID = MbufAllocColor(theApp.clMilMain.m_mSysID, 3,	/* 1: Monochrome, 3: RGB */
		mlSizeX, mlSizeY, 8 + M_UNSIGNED, M_IMAGE + M_PROC, M_NULL);
	if (mlColorID)
	{
		MbufCopy(grab_id, mlColorID);
		/* �˻� ��� ���� �̹��� ������ ��� (Linex) */
		DrawLineMarkerResult(mlMarkerId, mlColorID);
		/* ��� ���� ��ȯ  */
		bFinded = GetMeasLineResult(mlMarkerId, mlColorID, i32CentX, i32CentY);
		/* MIL ���� ���� ��� ��� ���� */
		if (bFinded && mil_result)	MmeasGetResult(mlMarkerId, M_INTERACTIVE, NULL, NULL);
		/* ���������� �˻��� ������ �̹��� ���ۿ� ���� */
		m_mlLineID = MbufAllocColor(theApp.clMilMain.m_mSysID, 3,	/* 1: Monochrome, 3: RGB */
			i32ScanWidth, i32ScanHeight,
			8 + M_UNSIGNED, M_IMAGE + M_PROC, M_NULL);
		i32OffX = i32CentX - i32ScanWidth / 2 - 2;
		i32OffY = i32CentY - i32ScanHeight / 2 - 2;
		i32OffX = i32OffX < 0 ? 0 : i32OffX;
		i32OffY = i32OffY < 0 ? 0 : i32OffY;
		MbufCopyColor2d(mlColorID, m_mlLineID, M_ALL_BANDS, i32OffX, i32OffY,
			M_ALL_BANDS, 0, 0, i32ScanWidth, i32ScanHeight);
		/* Release to Memory */
		MbufFree(mlColorID);
	}
	/* Release to Memory */
	MmeasFree(mlMarkerId);
	MmeasFree(mlContextID);

	return bFinded;
}

/*
 desc : Set the parameters of the strip maker
 parm : grab_id	- [in]  Grabbed Image ID
		param	- [in]  Structures pointer with information to set up is stored
		result	- [out] Buffer which the resulting value will be returned and stored
 retn : TRUE or FALSE
*/
BOOL CMilModel::RunMarkerStrip(MIL_ID graph_id, MIL_ID grab_id, LPG_MSMP param, STG_MSMR& result)
{
	TCHAR tzFile[MAX_PATH_LEN] = { NULL };
	MIL_ID mlMarkerID = M_NULL/*, mlContextID = M_NULL*/;
	MIL_INT mlSizeX, mlSizeY;
	MIL_DOUBLE mlCentX, mlCentY, mlWidth, mlAngle, mlScore, mlLength;
	SYSTEMTIME stTm = { NULL };

	/* Get the current system time */
	GetLocalTime(&stTm);
	/* Set the file name */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\save_img\\hole\\%04d%02d%02d_%02d%02d%02d.bmp",
		g_tzWorkDir, stTm.wYear, stTm.wMonth, stTm.wDay, stTm.wHour, stTm.wMinute, stTm.wSecond);
	/* ���� Marker Strip �̹����� �ӽ� ���� �޸� ���� */
	if (m_mlStripID)	MbufFree(m_mlStripID);

	/* Get the size of grabbed image */
	mlSizeX = (INT32)MbufInquire(grab_id, M_SIZE_X, M_NULL);
	mlSizeY = (INT32)MbufInquire(grab_id, M_SIZE_Y, M_NULL);

	/* Allocate a measurement marker (Marker�� ���� �Ҵ��� �մϴ�. �� ��° ���ڰ� M_STRIPE �� �Ǿ� �ִٴ°� Ȯ���ϼ���)
	/* stripe �ܿ��� edge, point ������ ���� �����մϴ� */
	mlMarkerID = MmeasAllocMarker(theApp.clMilMain.m_mSysID, M_STRIPE, M_DEFAULT, M_NULL);
	if (!mlMarkerID || MappGetError(M_CURRENT, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"MIL_Err : Failed to allocate a measurement marker (buffer)");
		return FALSE;
	}
#if 0
	/* Allocation a measurement context */
	MmeasAllocContext(theApp.clMilMain.m_mSysID, M_DEFAULT, &mlContextID);
	if (!mlContextID || MappGetError(M_CURRENT, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"MIL_Err : Failed to allocate a measurement context (buffer)");
		if (mlMarkerID)	MmeasFree(mlMarkerID);
		return FALSE;
	}
#endif
	/* Setup the properties of marker to be find */
	if (SetStripMakerSetup(mlMarkerID, param))
	{
#if 0
		/* Draw the marker information */
		MIL_ID mlColorID = MbufAllocColor(theApp.clMilMain.m_mSysID, 3, /* 1: Monochrome, 3: RGB */
			mlSizeX, mlSizeY, 8 + M_UNSIGNED, M_IMAGE + M_PROC, M_NULL);
		MbufCopy(grab_id, mlColorID);
		/* Draw the contour of the measurement region. */
		MgraColor(M_DEFAULT, M_COLOR_RED);
		MmeasDraw(M_DEFAULT, mlMarkerID, mlColorID, M_DRAW_SEARCH_REGION, M_DEFAULT, M_MARKER);
		MbufExport(L"g:\\download\\marker_search_region.bmp", M_BMP, mlColorID);
		if (mlColorID)	MbufFree(mlColorID);
#endif
		/* !!! Find the marker and compute all applicable measurements !!! */
		//MmeasFindMarker(M_DEFAULT, grab_id, mlMarkerID, M_CONTRAST);
		MmeasFindMarker(M_DEFAULT, grab_id, mlMarkerID, M_EDGE_CONTRAST); 
		/* Get the stripe position, width and angle. */
		MmeasGetResult(mlMarkerID, M_POSITION, &mlCentX, &mlCentY);
		MmeasGetResult(mlMarkerID, M_WIDTH, &mlWidth, M_NULL);
		MmeasGetResult(mlMarkerID, M_ANGLE, &mlAngle, M_NULL);
		MmeasGetResult(mlMarkerID, M_SCORE, &mlScore, M_NULL);
		MmeasGetResult(mlMarkerID, M_LENGTH, &mlLength, M_NULL);
		/* Save the result values in the return buffer */
		result.cent_x = mlCentX;
		result.cent_y = mlCentY;
		result.angle = mlAngle;
		result.score = mlScore;
		result.width = mlWidth;
		result.length = mlLength;
		/* Draw the marker information */
		m_mlStripID = MbufAllocColor(theApp.clMilMain.m_mSysID, 3, /* 1: Monochrome, 3: RGB */
			mlSizeX, mlSizeY, 8 + M_UNSIGNED, M_IMAGE + M_PROC, M_NULL);
		MbufCopy(grab_id, m_mlStripID);
		/* Draw the contour of the measurement region. */
#if 0
		MgraColor(M_DEFAULT, M_COLOR_RED);
		MmeasDraw(M_DEFAULT, mlMarkerID, m_mlStripID, M_DRAW_SEARCH_REGION + M_DRAW_POSITION, M_DEFAULT, M_RESULT);
#else
		if (mlWidth > 32.0f)
		{
			MgraColor(M_DEFAULT, M_COLOR_DARK_GREEN);
			MmeasDraw(M_DEFAULT, mlMarkerID, m_mlStripID, M_DRAW_SEARCH_REGION, M_DEFAULT, M_RESULT);
			MmeasDraw(M_DEFAULT, mlMarkerID, m_mlStripID, M_DRAW_SEARCH_REGION_CENTER, M_DEFAULT, M_RESULT);
			MgraColor(M_DEFAULT, M_COLOR_BLUE);
			MmeasDraw(M_DEFAULT, mlMarkerID, m_mlStripID, M_DRAW_WIDTH, M_DEFAULT, M_RESULT);
		}
		MgraColor(M_DEFAULT, M_COLOR_RED);
		MmeasDraw(M_DEFAULT, mlMarkerID, m_mlStripID, M_DRAW_POSITION + M_EDGE_FIRST + M_EDGE_SECOND, M_DEFAULT, M_RESULT);
		// 		MgraColor(M_DEFAULT, M_COLOR_DARK_MAGENTA);
		// 		MmeasDraw(M_DEFAULT, mlMarkerID, m_mlStripID, M_DRAW_ARROW, M_DEFAULT, M_RESULT);
		// 		MgraColor(M_DEFAULT, M_COLOR_DARK_YELLOW);
		// 		MmeasDraw(M_DEFAULT, mlMarkerID, m_mlStripID, M_DRAW_EDGES, M_DEFAULT, M_RESULT);
#endif
		/* Draw the contour of the measurement region. */
		MbufExport(tzFile, M_BMP, m_mlStripID);
	}
	/* Release memory if any existing created objects created object exist */
	if (mlMarkerID)		MmeasFree(mlMarkerID);
	//	if (mlContextID)	MmeasFree(mlContextID);

	return TRUE;
}
#endif

/*
 desc : Align Camera�� ���� �˻��� ��ũ�� �߽� ��ġ�� �ٽ� ī�޶��� ȸ�� ���� ���� �����ؼ�
		���� �˻��� ��ũ�� X, Y �߽� �� ��ȯ
 parm : cent_x	- [in]  Grabbed Image�� �߽� ��ġ
		cent_y	- [in]  Grabbed Image�� �߽� ��ġ
		find_x	- [out] ȸ�� ������ �˻��� �߽� ��ġ�� �Է� �޾� ȸ�� ���� �߽� ��ġ �� ��ȯ
		find_y	- [out] ȸ�� ������ �˻��� �߽� ��ġ�� �Է� �޾� ȸ�� ���� �߽� ��ġ �� ��ȯ
 retn : None
*/
VOID CMilModel::GetFindRotatePosXY(DOUBLE cent_x, DOUBLE cent_y, DOUBLE& find_x, DOUBLE& find_y)
{
	DOUBLE dbRadian = 0.0f, dbFindX = find_x, dbFindY = find_y;
	DOUBLE dbAngle = m_pstConfig->acam_spec.spec_angle[m_u8ACamID - 1];	/* Degree */
	/* ȸ�� �� (Degree)�� �ִ� ��������� ó�� */
	if (dbAngle != 0.0f)
	{
		dbRadian = -uvCmn_GetDeg2Rad(dbAngle);	/* ���� ���̳ʽ��� �ݴ�� ȸ���� ���� ���ϱ� ���� */
		/* �߽� ��ġ �������� ������ŭ ȸ�� ���� ���, �� X, Y ��ǥ �� ��� */
		dbFindX = (find_x - cent_x) * cos(dbRadian) - (find_y - cent_y) * sin(dbRadian) + cent_x;
		dbFindY = (find_x - cent_x) * sin(dbRadian) + (find_y - cent_y) * cos(dbRadian) + cent_y;
	}
	/* ȸ�� Ȥ�� ȸ�� ���� �� ���� */
	find_x = dbFindX;
	find_y = dbFindY;
}

/*
 desc : ���� �˻��� Edge Object ����
 parm : None
 retn : None
*/
VOID CMilModel::ResetEdgeObject()
{
	UINT32 i = 0x00;

#ifndef _NOT_USE_MIL_
	if (m_mlEdgeID)
	{
		MbufFree(m_mlEdgeID);
		m_mlEdgeID = M_NULL;
	}
#endif

	for (; i < m_pstConfig->edge_find.max_find_count; i++)
	{
		m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].set_data = 0x00;
	}
}

/*
 desc : ���� ��ϵ� Mark Model ũ�� ��ȯ
 parm : index	- [in]  ��û�ϰ��� �ϴ� ���� ��ġ (Zero-based)
		flag	- [in]  0x00 : ����  ũ��, 0x01 : ���� ũ��
		unit	- [in]  0x00 : um, 0x01 : pixel
		size	- [in]  0x00 : radius (������), 0x01 : diameter (����)
 retn : ũ�� ��ȯ (����: um or pixel)
*/
DOUBLE CMilModel::GetMarkModelSize(UINT8 index, UINT8 flag, UINT8 unit)
{
	DOUBLE dbSize = 0.0f;

	if (!m_pstMarkModel)	return dbSize;
	if (ENG_MMDT(m_pstMarkModel[index].type) != ENG_MMDT::en_image)
	{
		if (!m_pstMarkModel[index].IsValid())	return dbSize;
		switch (m_pstMarkModel[index].type)
		{
		case ENG_MMDT::en_circle:
		case ENG_MMDT::en_square:
		case ENG_MMDT::en_ring:
			if (!unit)	dbSize = m_pstMarkModel[index].param[1];
			else		dbSize = m_pstMarkModel[index].param[1] / (m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1) * 1000.0f);
#if 0	/* ������ �� ũ��� �������� ����ϹǷ� */
			switch (m_pstMarkModel[index].type)
			{
			case ENG_MMDT::en_circle:
			case ENG_MMDT::en_square:	dbSize *= 2.0f;	break; /* ���̴� 2��� ������ ��� �� */
			}
#endif
			break;
		case ENG_MMDT::en_ellipse:
		case ENG_MMDT::en_rectangle:
		case ENG_MMDT::en_cross:
		case ENG_MMDT::en_diamond:
		case ENG_MMDT::en_triangle:
			if (0x00 == flag)
			{
				if (!unit)	dbSize = m_pstMarkModel[index].param[1];
				else		dbSize = m_pstMarkModel[index].param[1] / (m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1) * 1000.0f);
			}
			else
			{
				if (!unit)	dbSize = m_pstMarkModel[index].param[2];
				else		dbSize = m_pstMarkModel[index].param[2] / (m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1) * 1000.0f);
			}
			break;
		}
	}
	else
	{
		if (0x00 == flag)
		{
			if (unit)	dbSize = m_pstModelSize[index].x;	/* pixel */
			else		dbSize = m_pstModelSize[index].x / (m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1) * 1000.0f);
		}
		else
		{
			if (unit)	dbSize = m_pstModelSize[index].y;	/* pixel */
			else		dbSize = m_pstModelSize[index].y / (m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1) * 1000.0f);
		}
	}

	return dbSize;
}

/*
 desc : Align Mark �˻� ��� ����
 parm : method		- [in]  0x00 : ������ 1�� ��ȯ, 0x01 : ���� ��ũ �������� ����ġ�� �ο��Ͽ� ��� ��ȯ
		count		- [in]  'method' ���� 2 or 3�� ���, ���� �˻��� Mark ���� �� 2 �̻� ���̾�� ��
 retn : None
*/
VOID CMilModel::SetMarkMethod(ENG_MMSM method, UINT8 count)
{
	m_enMarkMethod = method;
	switch (method)
	{
	case ENG_MMSM::en_single:	m_u8MarkFindSet = 0x01;	break;
	case ENG_MMSM::en_ring_circle:	m_u8MarkFindSet = 0x02;	break;
	case ENG_MMSM::en_ph_step:	m_u8MarkFindSet = 0x02;	break;
	case ENG_MMSM::en_cent_side:
	case ENG_MMSM::en_multi_only:	m_u8MarkFindSet = count;	break;
	}
}

#ifndef _NOT_USE_MIL_
/*
 desc : Edge Line Detection �˻� ���� (�Ķ����) �� ����
 parm : mark_id		- [in]  Marker ID
		grab_id		- [in]  �̹��� ���� ���� ID (Drawing Object ID)
		scan_width	- [in]  �̹��� �˻� ���� ũ�� (����: pixel) (�̹��� ũ�� �ƴ�)
		scan_height	- [in]  �̹��� �˻� ���� ũ�� (����: pixel) (�̹��� ũ�� �ƴ�)
		cent_x/y	- [in]  �̹��� �˻� ���� �߽� ��ǥ (����: pixel)
 retn : None
*/
VOID CMilModel::SetLineMarkerSetup(MIL_ID mark_id, MIL_ID grab_id,
	INT32 scan_width, INT32 scan_height,
	INT32 cent_x, INT32 cent_y)
{
	DOUBLE dbFilterType[3] = { M_EULER, M_PREWITT, M_SHEN };
	DOUBLE dbPolarity[4] = { M_NULL, M_ANY, M_POSITIVE, M_NEGATIVE };
	DOUBLE dbOrientation[4] = { M_NULL, M_ANY, M_HORIZONTAL, M_VERTICAL };
	DOUBLE dbEnable[2] = { M_DISABLE, M_ENABLE };
	LPG_CLED pstLine = &m_pstConfig->line_find;

	/* �ڽ� ������ ũ�� ��, �̹��� �˻� ������ ũ�� */
	MmeasSetMarker(mark_id, M_BOX_SIZE, scan_width, scan_height);
	/* �ڽ� ������ �߽� ��, ī�޶� Grabbed Area�� �߽� ��ǥ (unit: Pixel) */
	MmeasSetMarker(mark_id, M_BOX_CENTER, cent_x, cent_y);
	/* �˻� ���� (BOX_SIZE) ���� ���ǿ� �´� ��� Edge Line Detection */
	MmeasSetMarker(mark_id, M_NUMBER, M_ALL, M_NULL);
	MmeasSetMarker(mark_id, M_NUMBER_MIN, M_DEFAULT, M_NULL);
	/* Filter Type : EULER (0) / PREWITT (1) / SHEN (2) */
	MmeasSetMarker(mark_id, M_FILTER_TYPE, dbFilterType[pstLine->filter_type], M_NULL);
	if (pstLine->filter_type == 0x02 && pstLine->filter_smoothness > 0)	/* in case 'SHEN' */
	{
		MmeasSetMarker(mark_id, M_FILTER_SMOOTHNESS, DOUBLE(pstLine->filter_smoothness), M_NULL);
	}
	/* ���� �۾��� ���� ���� ������ �����Ͽ�, ó�� �ӵ��� ����������, ��� (ǰ��)�� ������ */
	if (pstLine->edge_threshold)
	{
		//MmeasSetMarker(mark_id, M_EDGE_THRESHOLD, DOUBLE(pstLine->edge_threshold), M_NULL);
		MmeasSetMarker(mark_id, M_EDGEVALUE_VAR_MIN, DOUBLE(pstLine->edge_threshold), M_NULL); 
	}
	/* 2��°�� stripe markers�� ����. ������ M_NULL */
	MmeasSetMarker(mark_id, M_POLARITY, dbPolarity[pstLine->polarity], M_OPPOSITE);
	/* �Ʒ� 2���� �̹����� ���ݿ� ���� ���� �ٸ��� ��� �� */
	MmeasSetMarker(mark_id, M_SUB_REGIONS_OFFSET, DOUBLE(pstLine->sub_regions_offset), M_NULL);			/* Sub Region Offset �� (-100 % ~ +100 %) */
	MmeasSetMarker(mark_id, M_SUB_REGIONS_SIZE, DOUBLE(pstLine->sub_regions_size), M_NULL);				/* Sub Region ũ�� (0 ~ 100 %) */
	MmeasSetMarker(mark_id, M_SUB_REGIONS_NUMBER, DOUBLE(pstLine->sub_regions_number), M_NULL);			/* �˻� ������ ���� ���� �� (1 or Later) */
	/* �˻��� ��Ŀ�� ������ �������� �������� ���� */
	MmeasSetMarker(mark_id, M_ORIENTATION, dbOrientation[pstLine->orientation], M_NULL);
	/* ��Ŀ�� ��ġ�� ã�� ��, �ܰ躰�� ȸ���ϴ� ������ �� (0.1 ~ 180.0) */
	MmeasSetMarker(mark_id, M_BOX_ANGLE_MODE, dbEnable[pstLine->box_angle_mode], M_NULL);
	/* ���� Ȱ��Ȳ ���ο� ���� */
	if (pstLine->box_angle_mode)
	{
		MmeasSetMarker(mark_id, M_BOX_ANGLE, DOUBLE(pstLine->box_angle), M_NULL);						/* 0 ���� �� */
		/* ȸ�� ������ �˻� �̵� ���� ũ�� �� */
		if (pstLine->box_angle_accuracy == 0.0f)
			MmeasSetMarker(mark_id, M_BOX_ANGLE_ACCURACY, M_DISABLE, M_NULL);
		else
			MmeasSetMarker(mark_id, M_BOX_ANGLE_ACCURACY, pstLine->box_angle_accuracy, M_NULL);			/* ���� ������ ���������� �˻��� ������ ������ �� ������ */
		/* �Ʒ� 2���� ������ �̿��Ͽ� �ᱹ (BOX_ANGLE - BOX_ANGLE_DELTA_NEG) ~ (BOX_ANGLE + BOX_ANGLE_DELTA_POS) ���� �˻� ����	*/
		MmeasSetMarker(mark_id, M_BOX_ANGLE_DELTA_POS, DOUBLE(pstLine->box_angle_delta_neg), M_NULL);	/* �˻� ������ ������ �� ���� �� ������ */
		MmeasSetMarker(mark_id, M_BOX_ANGLE_DELTA_NEG, DOUBLE(pstLine->box_angle_delta_pos), M_NULL);	/* �˻� ������ ������ �� ���� �� ������ */
	}
	/* Enable search region clipping : �ڵ����� ������ Clipping */
	MmeasSetMarker(mark_id, M_SEARCH_REGION_CLIPPING, dbEnable[pstLine->search_region_clip], M_NULL);
}

/*
 desc : Marker �˻� ��� ������ �̹��� ������ ����ϱ�
 parm : mark_id	- [in]  Marker ID
		grab_id	- [in]  �̹��� ���� ���� ID (Drawing Object ID)
 retn : None
*/
VOID CMilModel::DrawLineMarkerResult(MIL_ID mark_id, MIL_ID grab_id)
{
	MgraControl(M_DEFAULT, M_BACKGROUND_MODE, M_TRANSPARENT);
#ifdef _DEBUG
	/* Show the subject text string */
	MgraColor(M_DEFAULT, M_COLOR_BLUE);
	MgraText(M_DEFAULT, grab_id, 10.0f, 10.0f, MIL_TEXT("Measurement Search Region - Blue"));
	MgraColor(M_DEFAULT, M_COLOR_MAGENTA);
	MgraText(M_DEFAULT, grab_id, 10.0f, 25.0f, MIL_TEXT("Marker Position           - Yellow"));
	MgraColor(M_DEFAULT, M_COLOR_RED);
	MgraText(M_DEFAULT, grab_id, 10.0f, 40.0f, MIL_TEXT("Edges                     - Red"));
#endif
	/* Draw the found marker edges */
	MgraColor(M_DEFAULT, M_COLOR_RED);
	MmeasDraw(M_DEFAULT, mark_id, grab_id, M_DRAW_EDGES, M_DEFAULT, M_RESULT);
	/* Draw a cross at the found position */
	MgraColor(M_DEFAULT, M_COLOR_MAGENTA);
	MmeasDraw(M_DEFAULT, mark_id, grab_id, M_DRAW_POSITION, M_DEFAULT, M_RESULT);
	/* Measure and print results */
	MgraColor(M_DEFAULT, M_COLOR_BLUE);
	MmeasDraw(M_DEFAULT, mark_id, grab_id, M_DRAW_SEARCH_REGION, M_DEFAULT, M_RESULT);
	/* Draw the search region orientation */
	
	//MmeasDraw(M_DEFAULT, mark_id, grab_id, M_DRAW_ARROW, M_DEFAULT, M_RESULT);
	MmeasDraw(M_DEFAULT, mark_id, grab_id, M_DRAW_SEARCH_DIRECTION, M_DEFAULT, M_RESULT); 

	/* Draw a cross at the found position */
	MgraColor(M_DEFAULT, M_COLOR_MAGENTA);
	MmeasDraw(M_DEFAULT, mark_id, grab_id, M_DRAW_POSITION, M_DEFAULT, M_RESULT);
}

/*
 desc : �̹����� ��輱 �߽� ��ġ ���
 parm : mark_id		- [in]  Marker ID
		grab_id		- [in]  �̹��� ���� ���� ID (Drawing Object ID)
		cent_x/y	- [in]  �̹����� �߽� ��ġ (����: pixel)
 retn : TRUE or FALSE
*/
BOOL CMilModel::GetMeasLineResult(MIL_ID mark_id, MIL_ID grab_id, INT32 cent_x, INT32 cent_y)
{
	TCHAR tzPosXY[128] = { NULL }, tzFile[MAX_PATH_LEN] = { NULL };
	LONG l, lFindCount = 0, lPos1, lPos2;
	DOUBLE dbValue[2], dbPixelToUm = m_pstConfig->acam_spec.spec_pixel_um[m_u8ACamID - 1];
	DOUBLE dbGrabEdgeAngle = m_pstConfig->line_find.grab_image_edge_angle;
	LPG_ELFR pstValue = m_pstShMemVisi->line_result[m_u8ACamID - 1];
	SYSTEMTIME stTime = { NULL };

	/* �ؽ�Ʈ ���� ����  */
	MgraColor(M_DEFAULT, M_COLOR_RED);
	/* Get the specified type of result(s) for all points, edges, stripes, or circles */
	/* from a measurement marker or result buffer */
	MmeasGetResult(mark_id, M_NUMBER + M_TYPE_LONG, &lFindCount, NULL);		/* �˻��� Marker Counts */
	for (l = 0; l < lFindCount; l++)
	{
		memset(pstValue, 0x00, sizeof(STG_ELFR));

		/* �˻��� ���� ������ �� �Ķ���Ϳ� ���� ��� �� ��� */
		MmeasGetResultSingle(mark_id, M_ANGLE, &dbValue[0], M_NULL, l);
		pstValue->angle = (INT16)ROUNDED((dbValue[0] - dbGrabEdgeAngle) * 100.0f, 0);
		MmeasGetResultSingle(mark_id, M_EDGE_STRENGTH, &dbValue[0], M_NULL, l);
		pstValue->strength = (UINT16)ROUNDED(dbValue[0] * 100.0f, 0);
		//MmeasGetResultSingle(mark_id, M_CONTRAST,		&dbValue[0],	M_NULL,		l);
		MmeasGetResultSingle(mark_id, M_EDGE_CONTRAST, &dbValue[0], M_NULL, l); 
		pstValue->contrast = (UINT16)ROUNDED(dbValue[0] * 10.0f, 0);
		MmeasGetResultSingle(mark_id, M_POSITION, &dbValue[0], &dbValue[1], l);
		pstValue->position[0] = (UINT32)ROUNDED(dbValue[0] * 1000.0f, 0);
		pstValue->position[1] = (UINT32)ROUNDED(dbValue[1] * 1000.0f, 0);
		//MmeasGetResultSingle(mark_id, M_TOTAL_SCORE,	&dbValue[0],	M_NULL,		l);
		MmeasGetResultSingle(mark_id, M_SCORE_TOTAL, &dbValue[0], M_NULL, l); 
		pstValue->total_score = (UINT32)ROUNDED(dbValue[0] * 1000.0f, 0);
		MmeasGetResultSingle(mark_id, M_SCORE, &dbValue[0], M_NULL, l);
		pstValue->percent_score = (UINT32)ROUNDED(dbValue[0] * 1000.0f, 0);
		MmeasGetResultSingle(mark_id, M_WIDTH, &dbValue[0], M_NULL, l);
		pstValue->line_width = dbValue[0];
		/* �� ������ ����� �׸񿡴� SPACING �� ���Ҽ� ���� (���� �߻�) */
		if (!(l + 1 == lFindCount))
		{
			MmeasGetResultSingle(mark_id, M_SPACING, &dbValue[0], M_NULL, l);
			pstValue->spacing_um = dbValue[0] * dbPixelToUm;
		}
		/* ------------------------------------------------------------------- */
		/* �̹����� �߽� (Position X)���� ���� �νĵ� Edge���� �Ÿ� (����: um) */
		/* ------------------------------------------------------------------- */
		pstValue->cent_diff_um = (pstValue->position[0] / 1000.0f - cent_x) * dbPixelToUm;

		/* ��� ������ ��� ��ġ ��� */
		lPos1 = ((l / 2) * 50) * ((l % 2 == 0) ? 1 : -1);
		lPos2 = ((l % 2 == 0) ? 20 : -20);
		/* �˻� ��� ��ȯ */
		swprintf_s(tzPosXY, 128, L"C:%.1f | A:%+.2f | S:%.2f",
			pstValue->contrast / 10.0f, pstValue->angle / 100.0f, pstValue->strength / 100.0f);
		MgraText(M_DEFAULT, grab_id,
			(pstValue->position[0] / 1000.0f) + 5, pstValue->position[1] / 1000.0f + lPos1 + lPos2, tzPosXY);
	}

	/* �����̴� ���д�, �ش� �����۾� ������ �̹��� ���� */
	GetLocalTime(&stTime);
#ifdef _DEBUG
	swprintf_s(tzFile, MAX_PATH_LEN, L"g:\\download\\qcells\\export_line.bmp");
#else
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\line\\%04d%02d%02d_%02d%02d%02d.bmp",
		g_tzWorkDir,
		stTime.wYear, stTime.wMonth, stTime.wDay,
		stTime.wHour, stTime.wMinute, stTime.wSecond);
#endif
	MbufExport(tzFile, M_BMP, grab_id);

	return (lFindCount > 0);
}

/*
 desc : Find an marker in an image and take the specified measurements
 parm : graph_id- [in]
		strip_id- [in]  Stripe Marker ID
		param	- [in]  Structures pointer with information to set up is stored
 retn : TRUE or FALSE
*/
BOOL CMilModel::SetStripMakerSetup(MIL_ID strip_id, LPG_MSMP param)
{
	MIL_INT32 mlOrientation = param->orientation == 0 ? M_HORIZONTAL : M_VERTICAL;
	MIL_INT32 mlPolLeft = param->polarity[0] == 0 ? M_POSITIVE : M_NEGATIVE;
	MIL_INT32 mlPolRight = param->polarity[1] == 0 ? M_POSITIVE : M_NEGATIVE;
	MIL_INT32 mlAngleEnable = param->enable_angle == 1 ? M_ENABLE : M_DISABLE;
	MIL_INT32 mlAngleMethod = M_BILINEAR;

	/* Select the Interpolation Method */
	switch (param->angle_int_mode)
	{
	case 0x00:	mlAngleMethod = M_NEAREST_NEIGHBOR;	break;
	case 0x01:	mlAngleMethod = M_BILINEAR;			break;
	default:	mlAngleMethod = M_BICUBIC;			break;
	}

	/* ��Ŀ�� ���� ������ ���� �մϴ� */
	MmeasSetMarker(strip_id, M_ORIENTATION, mlOrientation, M_NULL);
	/* ��Ŀ�� �ؼ��� ���� (������ positive, ������ negative�� ���, ���� ������ �ʺ� ����) */
	MmeasSetMarker(strip_id, M_POLARITY, mlPolLeft, mlPolRight);
	/* Set the start location of inspection area */
	MmeasSetMarker(strip_id, M_BOX_ORIGIN, param->box_origin.x, param->box_origin.y);
	/* Sets the size of inspection area */
	MmeasSetMarker(strip_id, M_BOX_SIZE, param->box_size.x, param->box_size.y);
	/* Enable for rotation search in the measurement area (box) */
	MmeasSetMarker(strip_id, M_BOX_ANGLE_MODE, mlAngleEnable, M_NULL);
	if (param->enable_angle)
	{
		/* 90���� ���, Angle�� ������ ���� �������� 90������ ���� */
		MmeasSetMarker(strip_id, M_BOX_ANGLE_DELTA_POS, param->angle_delta, M_NULL);
		/* ������ �˰��� ���� */
		//MmeasSetMarker(strip_id, M_BOX_ANGLE_INTERPOLATION_MODE, mlAngleMethod, M_NULL);
		MmeasSetMarker(strip_id, M_SEARCH_REGION_ANGLE_INTERPOLATION_MODE, mlAngleMethod, M_NULL); 
	}
	//if (param->edge_width > 0.0f)
	//{
	//	/* ����ڰ� width�� ���� ���� ����. �˰����� �˻��� �� �����ϰ� ���� ��? ���� �����ϴ� pxiel ũ�� �� ���� */
	//	MmeasSetMarker(strip_id, M_WIDTH , param->edge_width, M_NULL);
	//	/* ���� ��� ���� pixel ���� ���� */
	//	//MmeasSetMarker(strip_id, M_WIDTH_VARIATION, param->edge_offset, M_NULL);
	//	MmeasSetMarker(strip_id, M_WIDTH_VARIATION, param->edge_offset, M_NULL);
	//}
	/* Sets the output unit to a pixel value */
	MmeasSetMarker(strip_id, M_RESULT_OUTPUT_UNITS, M_PIXEL, M_NULL);

	return TRUE;
}
#endif
/*
 desc : ���� ����� �������� ��ũ �˻� ���� �� ����
 parm : score	- [in]  Score Rate
		scale	- [in]  Scale Rate
 retn : None
*/
VOID CMilModel::SetRecipeMarkRate(DOUBLE score, DOUBLE scale)
{
	m_dbRecipeScore = score;
	m_dbRecipeScale = scale;
}

/*
 desc : ���� ����� ���� ũ�� ��ȯ
 parm : index	- [in]  ��ϵ� ���� �ε��� �� (zero-based)
 retn : ���� ũ�� �� (����: pixel)
*/
UINT32 CMilModel::GetModelWidth(UINT8 index)
{
	DOUBLE dbDia = 1.0f;

	switch (m_pstMarkModel[index].type)
	{
		/* �Ʒ� 2���� Type�� �˻� ��� �� ��, ���� (����)�� �ƴ� ������ */
	case ENG_MMDT::en_ring:
	case ENG_MMDT::en_circle:	dbDia = 2.0f;
	}
	//return (UINT32)ROUNDED(m_pstModelSize[index].x * dbDia, 0);
	return (UINT32)ROUNDED(m_pstMarkModel[index].iSizeP.x * dbDia, 0);
	
}

/*
 desc : ���� ����� ���� ũ�� ��ȯ
 parm : index	- [in]  ��ϵ� ���� �ε��� �� (zero-based)
 retn : ���� ũ�� �� (����: pixel)
*/
UINT32 CMilModel::GetModelHeight(UINT8 index)
{
	DOUBLE dbDia = 1.0f;

	switch (m_pstMarkModel[index].type)
	{
		/* �Ʒ� 2���� Type�� �˻� ��� �� ��, ���� (����)�� �ƴ� ������ */
	case ENG_MMDT::en_ring:
	case ENG_MMDT::en_circle:	dbDia = 2.0f;
	}
	//return (UINT32)ROUNDED(m_pstModelSize[index].y * dbDia, 0);
	return (UINT32)ROUNDED(m_pstMarkModel[index].iSizeP.y * dbDia, 0);
}

/*
 desc : Mulit Mark �˻� ����� ���, ��, MMF ��ũ �˻� ����� ���, �˻��� ��ũ ������ �ֺ��� �׸��� ���� �뵵
		Mark �� MMF ������ ���, ��ũ ũ�� ��
 parm : width	- [in]  �簢�� ������ ���� �� (����: um)
		height	- [in]  �簢�� ������ ���� �� (����: um)
 retn : None
*/
VOID CMilModel::SetMultiMarkArea(UINT32 width, UINT32 height)
{
	m_u32MMFMarkWidth = width;
	m_u32MMFMarkHeight = height;
}

/* ----------------------------------------------------------------------------------------- */
/*                                 lk91 VISION �߰� �Լ�                                     */
/* ----------------------------------------------------------------------------------------- */

/* desc : Regist Pattern Matching Mark (pat) */
BOOL CMilModel::RegistPat(PUINT8 img_src, CRect fi_rectArea, CString fi_filename, UINT8 mark_no)
{
	int width = fi_rectArea.right - fi_rectArea.left;
	int height = fi_rectArea.bottom - fi_rectArea.top;

#ifndef _NOT_USE_MIL_
	theApp.clMilMain.m_MarkSize.x = width;	// lk91 ��� ���̴��� m_pstMarkModel[mark_no].iSizeP �� ��ü�� �� �ִ���
	theApp.clMilMain.m_MarkSize.y = height;
#endif
	m_pstMarkModel[mark_no].iSizeP.x = width;
	m_pstMarkModel[mark_no].iSizeP.y = height;
	m_pstMarkModel[mark_no].iOffsetP.x = width / 2;
	m_pstMarkModel[mark_no].iOffsetP.y = height / 2;

#ifndef _NOT_USE_MIL_
	/* Put the image to image buffer */
	if (m_mlPATID[mark_no]) {
		MpatFree(m_mlPATID[mark_no]);
		m_mlPATID[mark_no] = M_NULL;
	}

	MpatAlloc(theApp.clMilMain.m_mSysID, M_NORMALIZED, M_DEFAULT, &m_mlPATID[mark_no]);

	/* Define a unique model*/
	//MpatDefine(m_mlPATID, M_REGULAR_MODEL, theApp.clMilMain.m_mImg[0], fi_rectArea.left, fi_rectArea.top, width, height, M_DEFAULT); 
	MpatDefine(m_mlPATID[mark_no], M_REGULAR_MODEL, theApp.clMilMain.m_mImgProc[m_camid - 1], fi_rectArea.left, fi_rectArea.top, width, height, M_DEFAULT);

	if (m_mlPATID[mark_no]) {
		MpatSave(fi_filename, m_mlPATID[mark_no], M_DEFAULT);
	}
#endif
	return TRUE;
}

/* desc : Regist Model Finder Mark (mmf) */
BOOL CMilModel::RegistMod(PUINT8 img_src, CRect fi_rectArea, CString fi_filename, UINT8 mark_no)
{
	int width = fi_rectArea.right - fi_rectArea.left;
	int height = fi_rectArea.bottom - fi_rectArea.top;

#ifndef _NOT_USE_MIL_
	theApp.clMilMain.m_MarkSize.x = width;// lk91 ��� ���̴��� m_pstMarkModel[mark_no].iSizeP �� ��ü�� �� �ִ���
	theApp.clMilMain.m_MarkSize.y = height;
#endif

	m_pstMarkModel[mark_no].iSizeP.x = width;
	m_pstMarkModel[mark_no].iSizeP.y = height;
	m_pstMarkModel[mark_no].iOffsetP.x = width / 2;
	m_pstMarkModel[mark_no].iOffsetP.y = height / 2;

#ifndef _NOT_USE_MIL_
	if (m_mlModelID[mark_no]) {
		MmodFree(m_mlModelID[mark_no]);
		m_mlModelID[mark_no] = M_NULL;
	}
	MmodAlloc(theApp.clMilMain.m_mSysID, M_GEOMETRIC, M_DEFAULT, &m_mlModelID[mark_no]);
	/* Define a unique model*/
	MmodDefine(m_mlModelID[mark_no], M_IMAGE, theApp.clMilMain.m_mImgProc[m_camid - 1], fi_rectArea.left, fi_rectArea.top, width, height);

	if (m_mlModelID) {
		MmodSave(fi_filename, m_mlModelID[mark_no], M_DEFAULT);
		if (theApp.clMilMain.m_mImg_MarkSet_Ori) {
			MbufFree(theApp.clMilMain.m_mImg_MarkSet_Ori);
			theApp.clMilMain.m_mImg_MarkSet_Ori = M_NULL;
		}
		if(!theApp.clMilMain.m_mImg_MarkSet_Ori)
			MbufAlloc2d(theApp.clMilMain.m_mSysID, width, height, 8L + M_UNSIGNED, M_IMAGE + M_DISP + M_PROC, &theApp.clMilMain.m_mImg_MarkSet_Ori);
		MmodDraw(M_DEFAULT, m_mlModelID[mark_no], theApp.clMilMain.m_mImg_MarkSet_Ori, M_DRAW_IMAGE, M_DEFAULT, M_DEFAULT);
	}
#endif
	return TRUE;
}

/* desc : Get Mark Size */
CPoint CMilModel::GetMarkSize(int mark_no)
{
	return m_pstMarkModel[mark_no].iSizeP;
}

/* desc : Get Mark Offset */
CPoint CMilModel::GetMarkOffset(BOOL bNewOffset, int mark_no)
{
	if (!bNewOffset)
		return m_pstMarkModel[mark_no].iOffsetP;
	else
		return um_iNewOffP[mark_no];
}

/* desc : Get Mark Find Mode */
UINT8 CMilModel::GetMarkFindMode(UINT8 mark_no)
{
	return m_findMode[mark_no];
}

/* desc : Set Mark Find Mode */
BOOL CMilModel::SetMarkFindMode(UINT8 find_mode, UINT8 mark_no)
{
	m_findMode[mark_no] = find_mode;
	return TRUE;
}

/* desc : Set Mark Size */
VOID CMilModel::SetMarkSize(CPoint fi_MarkSize, int setMode, int mark_no)
{
	switch (setMode)
	{
	case 0:
		m_pstMarkModel[mark_no].iSizeP = fi_MarkSize;
		break;
	case 1:
		um_iTmpSizeP[mark_no] = fi_MarkSize;
		break;
	default:
		break;
	}
}

/* desc : Set Mark Offset */
VOID CMilModel::SetMarkOffset(CPoint fi_MarkCenter, int setOffsetMode, int mark_no) // 0(default) : �Ϲ� offset, 1 : NewOffset, 2: (MarkSet)NewOffset -> Offset ��ȯ, 3: Draw �� ����ϴ� �ӽ� Offset
{
	switch (setOffsetMode)
	{
	case 0:
		m_pstMarkModel[mark_no].iOffsetP = fi_MarkCenter;
		break;
	case 1:
		um_iNewOffP[mark_no].x = fi_MarkCenter.x;
		um_iNewOffP[mark_no].y = fi_MarkCenter.y;
		break;
	case 2:
		m_pstMarkModel[mark_no].iOffsetP.x = (LONG)(fi_MarkCenter.x / theApp.clMilMain.MARKSET_DISP_RATE);
		m_pstMarkModel[mark_no].iOffsetP.y = (LONG)(fi_MarkCenter.y / theApp.clMilMain.MARKSET_DISP_RATE);
		break;
	case 3:
		um_iTmpOffP[mark_no].x = fi_MarkCenter.x;
		um_iTmpOffP[mark_no].y = fi_MarkCenter.y;
		break;
	default:
		break;
	}
}

/* desc : Mark Size, Offset �ʱ�ȭ */
VOID CMilModel::InitSetMarkSizeOffset(UINT8 cam_id, TCHAR* file, UINT8 fi_findType, UINT8 mark_no)
{
	DOUBLE tSize_x = 0, tSize_y = 0;
	if (fi_findType == 0) {
#ifndef _NOT_USE_MIL_
		if (m_mlModelID[mark_no]) {
			MmodFree(m_mlModelID[mark_no]);
			m_mlModelID[mark_no] = M_NULL;
		}
		//MmodAlloc(theApp.clMilMain.m_mSysID, M_GEOMETRIC, M_DEFAULT, &m_mlModelID[mark_no]);
		MmodRestore(file, theApp.clMilMain.m_mSysID, M_DEFAULT, &m_mlModelID[mark_no]);

		if (!m_mlModelID[mark_no] || MappGetError(M_CURRENT, M_NULL))
		{
			LOG_ERROR(ENG_EDIC::en_mil, L"MIL_ERR : Failed to restore the mark image (buffer)");
			return;
		}
		if (m_mlModelID[mark_no]) {
			MmodInquire(m_mlModelID[mark_no], M_DEFAULT, M_ALLOC_SIZE_X, &tSize_x);
			MmodInquire(m_mlModelID[mark_no], M_DEFAULT, M_ALLOC_SIZE_Y, &tSize_y);
		}
#endif
	}
	else {
#ifndef _NOT_USE_MIL_
		if (m_mlPATID[mark_no])
		{
			MpatFree(m_mlPATID[mark_no]);
			m_mlPATID[mark_no] = M_NULL;
		}
		MpatAlloc(theApp.clMilMain.m_mSysID, M_DEFAULT, M_DEFAULT, &m_mlPATID[mark_no]);
		MpatRestore(file, theApp.clMilMain.m_mSysID, M_DEFAULT, &m_mlPATID[mark_no]);
		if (!m_mlPATID[mark_no] || MappGetError(M_CURRENT, M_NULL))
		{
			LOG_ERROR(ENG_EDIC::en_mil, L"MIL_ERR : Failed to restore the mark image (buffer)");
			return;
		}
		if (m_mlPATID[mark_no]) {
			MpatInquire(m_mlPATID[mark_no], M_DEFAULT, M_ALLOC_SIZE_X, &tSize_x);
			MpatInquire(m_mlPATID[mark_no], M_DEFAULT, M_ALLOC_SIZE_Y, &tSize_y);
		}
#endif
	}

	m_pstMarkModel[mark_no].iSizeP.x = um_iNewSizeP[mark_no].x = (int)tSize_x;
	m_pstMarkModel[mark_no].iSizeP.y = um_iNewSizeP[mark_no].y = (int)tSize_y;
	m_pstMarkModel[mark_no].iOffsetP.x = um_iNewOffP[mark_no].x = (int)(tSize_x / 2.0);
	m_pstMarkModel[mark_no].iOffsetP.y = um_iNewOffP[mark_no].y = (int)(tSize_y / 2.0);

	//m_pstMarkModel->iOffsetP = fi_MarkCenter;
}

/* desc : Draw Mark Display */
VOID CMilModel::PutMarkDisp(HWND hwnd, int fi_iNo, RECT draw, UINT8 cam_id, TCHAR* file, int fi_findType) // fi_findType : 0-Mod, 1-pat
{
#ifndef _NOT_USE_MIL_
	if(theApp.clMilMain.m_mImg_Mark[fi_iNo])	MbufClear(theApp.clMilMain.m_mImg_Mark[fi_iNo], 192L);
	else {
		AfxMessageBox(_T("m_mImg_Mark Buffer Create Fail"));
		return;
	}
	//--- SetMarkDisp ���ۻ���
	if (theApp.clMilMain.m_mImgDisp_Mark[fi_iNo])	MbufClear(theApp.clMilMain.m_mImgDisp_Mark[fi_iNo], 192L);
	else
	{
		AfxMessageBox(_T("m_mImgDisp_Mark Buffer Create Fail"));
		return;
	}

	//if (theApp.clMilMain.m_mImgDisp_Mark[fi_iNo])
	//{
	//	MdispSelectWindow(theApp.clMilMain.m_mDisID_Mark[fi_iNo], theApp.clMilMain.m_mImgDisp_Mark[fi_iNo], hwnd); 

	//	if (theApp.clMilMain.m_mDisID_Mark[fi_iNo])
	//	{
	//		MdispControl(theApp.clMilMain.m_mDisID_Mark[fi_iNo], M_OVERLAY_SHOW, M_ENABLE);
	//		MdispControl(theApp.clMilMain.m_mDisID_Mark[fi_iNo], M_OVERLAY, M_ENABLE);
	//		MdispInquire(theApp.clMilMain.m_mDisID_Mark[fi_iNo], M_OVERLAY_ID, &theApp.clMilMain.m_mOverlay_Mark[fi_iNo]);
	//		MgraControl(M_DEFAULT, M_BACKGROUND_MODE, M_TRANSPARENT);
	//	}
	//}
#endif
	DOUBLE tSize_x = 0, tSize_y = 0;
	DOUBLE tOffset_x = 0, tOffset_y = 0;
#ifndef _NOT_USE_MIL_
	if (fi_findType == 0) {
		if (m_mlModelID_D)
		{
			MmodFree(m_mlModelID_D);
			m_mlModelID_D = M_NULL;
		}
		//MmodAlloc(theApp.clMilMain.m_mSysID, M_GEOMETRIC, M_DEFAULT, &m_mlModelID_D);
		MmodRestore(file, theApp.clMilMain.m_mSysID, M_DEFAULT, &m_mlModelID_D);

		if (!m_mlModelID_D || MappGetError(M_CURRENT, M_NULL))
		{
			LOG_ERROR(ENG_EDIC::en_mil, L"MIL_ERR : Failed to restore the mark image (buffer)");
			return;
		}
		if (m_mlModelID_D) {
			MmodDraw(M_DEFAULT, m_mlModelID_D, theApp.clMilMain.m_mImg_Mark[fi_iNo], M_DRAW_IMAGE, M_DEFAULT, M_DEFAULT);
			MgraColor(M_DEFAULT, M_COLOR_BLUE);
			MmodDraw(M_DEFAULT, m_mlModelID_D, theApp.clMilMain.m_mImg_Mark[fi_iNo], M_DRAW_DONT_CARE, M_DEFAULT, M_DEFAULT);

			MmodInquire(m_mlModelID_D, M_DEFAULT, M_ALLOC_SIZE_X, &tSize_x);
			MmodInquire(m_mlModelID_D, M_DEFAULT, M_ALLOC_SIZE_Y, &tSize_y);
			//MmodInquire(m_mlModelID_D, M_DEFAULT, M_ORIGINAL_X, &tOffset_x); // lk91 M_REFERENCE_�� �ȵȴ�.. CDPoint �� �ȵ�
			//MmodInquire(m_mlModelID_D, M_DEFAULT, M_ORIGINAL_Y, &tOffset_y);
			tOffset_x = m_pstMarkModel[fi_iNo].iOffsetP.x;
			tOffset_y = m_pstMarkModel[fi_iNo].iOffsetP.y;


		}
	}
	else {
		if (m_mlPATID_D)
		{
			MpatFree(m_mlPATID_D);
			m_mlPATID_D = M_NULL;
		}
		//MpatAlloc(theApp.clMilMain.m_mSysID, M_DEFAULT, M_DEFAULT, &m_mlPATID_D);
		MpatRestore(file, theApp.clMilMain.m_mSysID, M_DEFAULT, &m_mlPATID_D);
		if (!m_mlPATID_D || MappGetError(M_CURRENT, M_NULL))
		{
			LOG_ERROR(ENG_EDIC::en_mil, L"MIL_ERR : Failed to restore the mark image (buffer)");
			return;
		}
		if (m_mlPATID_D) {
			MpatDraw(M_DEFAULT, m_mlPATID_D, theApp.clMilMain.m_mImg_Mark[fi_iNo], M_DRAW_IMAGE, M_DEFAULT, M_DEFAULT);
			MgraColor(M_DEFAULT, M_COLOR_BLUE);
			MpatDraw(M_DEFAULT, m_mlPATID_D, theApp.clMilMain.m_mImg_Mark[fi_iNo], M_DRAW_DONT_CARE, M_DEFAULT, M_DEFAULT);

			MpatInquire(m_mlPATID_D, M_DEFAULT, M_ALLOC_SIZE_X, &tSize_x);
			MpatInquire(m_mlPATID_D, M_DEFAULT, M_ALLOC_SIZE_Y, &tSize_y);
			//MpatInquire(m_mlPATID_D, M_DEFAULT, M_ORIGINAL_X, &tOffset_x); 
			//MpatInquire(m_mlPATID_D, M_DEFAULT, M_ORIGINAL_Y, &tOffset_y);
			tOffset_x = m_pstMarkModel[fi_iNo].iOffsetP.x;
			tOffset_y = m_pstMarkModel[fi_iNo].iOffsetP.y;

		}
	}
#endif
	CPoint iSizeP;

	iSizeP.x = um_iTmpSizeP[2].x = (LONG)tSize_x;
	iSizeP.y = um_iTmpSizeP[2].y = (LONG)tSize_y;
	um_iTmpOffP[2].x = (LONG)tOffset_x;
	um_iTmpOffP[2].y = (LONG)tOffset_y;

	//iSizeP = m_pstMarkModel->iSizeP;
	//iSizeP = um_iTmpSizeP; // lk91 sizeP�� �� �����ϰ� ���� �ҷ����� ��� ����ϱ�...

	double dRate, dRate1, dRate2 = 1.0;

	if (iSizeP.x != 0 && iSizeP.y != 0)
	{
		//if (iSizeP.x <= iSizeP.y)	dRate = (double)(draw.bottom - draw.top) / (double)iSizeP.y;
		//else					dRate = (double)(draw.right - draw.left) / (double)iSizeP.x;
		dRate1 = (double)(draw.bottom - draw.top) / (double)iSizeP.y;
		dRate2 = (double)(draw.right - draw.left) / (double)iSizeP.x;

		if (dRate1 < dRate2)
			dRate = dRate1;
		else
			dRate = dRate2;

#ifndef _NOT_USE_MIL_
		MbufClear(theApp.clMilMain.m_mImgDisp_Mark[fi_iNo], 192);
		//��ũ���°�
		// lk91 �̻��ϰ� dRate�� �����ѵ��� �̹��� ����� ó���� �� ���ķ� ������Ʈ �ɶ� �ٸ��� ���
		//MimResize(theApp.clMilMain.m_mImg_Mark[fi_iNo], theApp.clMilMain.m_mImgDisp_Mark[fi_iNo], dRate, dRate, M_DEFAULT);
		theApp.clMilMain.MARK_DISP_RATE = dRate;
#endif
	}

	//CPoint um_iNewOffP, um_iNewSizeP;
	//um_iNewOffP.x = (int)(m_pstMarkModel->iOffsetP.x);
	//um_iNewOffP.y = (int)(m_pstMarkModel->iOffsetP.y);
	//um_iNewSizeP.x = (int)(m_pstMarkModel->iSizeP.x);
	//um_iNewSizeP.y = (int)(m_pstMarkModel->iSizeP.y);

	// Overlay �׸���, fi_iDispType : 1
	theApp.clMilDisp.DrawOverlayDC(false, DISP_TYPE_MARK, fi_iNo);
	theApp.clMilDisp.AddLineList(DISP_TYPE_MARK, fi_iNo, um_iTmpOffP[2].x, 0, um_iTmpOffP[2].x, um_iTmpSizeP[2].y, PS_DOT, COLOR_GREEN);
	theApp.clMilDisp.AddLineList(DISP_TYPE_MARK, fi_iNo, 0, um_iTmpOffP[2].y, um_iTmpSizeP[2].x, um_iTmpOffP[2].y, PS_DOT, COLOR_GREEN);
	theApp.clMilDisp.DrawOverlayDC(true, DISP_TYPE_MARK, fi_iNo);
	//theApp.clMilDisp.DrawOverlayDC(false, DISP_TYPE_MARK, fi_iNo);
	//theApp.clMilDisp.AddLineList(DISP_TYPE_MARK, fi_iNo, um_iNewOffP.x, 0, um_iNewOffP.x, um_iNewSizeP.y, PS_DOT, COLOR_GREEN);
	//theApp.clMilDisp.AddLineList(DISP_TYPE_MARK, fi_iNo, 0, um_iNewOffP.y, um_iNewSizeP.x, um_iNewOffP.y, PS_DOT, COLOR_GREEN);
	//theApp.clMilDisp.DrawOverlayDC(true, DISP_TYPE_MARK, fi_iNo);

#ifndef _NOT_USE_MIL_
	if (fi_findType == 0) {
		MgraControl(M_DEFAULT, M_DRAW_ZOOM_X, theApp.clMilMain.MARK_DISP_RATE);
		MgraControl(M_DEFAULT, M_DRAW_ZOOM_Y, theApp.clMilMain.MARK_DISP_RATE);
		//int smooth = 70;
		DOUBLE smooth = (DOUBLE)m_pstConfig->mark_find.model_smooth;
		MgraColor(M_DEFAULT, M_COLOR_YELLOW);
		MmodControl(m_mlModelID_D, M_CONTEXT, M_SMOOTHNESS, smooth);
		MmodDraw(M_DEFAULT, m_mlModelID_D, theApp.clMilMain.m_mOverlay_Mark[fi_iNo], M_DRAW_EDGES, M_DEFAULT, M_DEFAULT);
	}

	if (fi_findType == 0) {
		if (m_mlModelID_D)
		{
			MmodFree(m_mlModelID_D);
			m_mlModelID_D = M_NULL;
		}
	}
	else {
		if (m_mlPATID_D)
		{
			MpatFree(m_mlPATID_D);
			m_mlPATID_D = M_NULL;
		}
	}
#endif
}

/* desc : Draw Set Mark Display */
BOOL CMilModel::PutSetMarkDisp(HWND hwnd, RECT draw, UINT8 cam_id, TCHAR* file, int fi_findType, DOUBLE fi_dRate) // Mark Disp�� �Ѹ���
{
	theApp.clMilMain.MARKSET_DISP_RATE = fi_dRate;

	long	BufferAttributes = M_IMAGE + M_DISP + M_PROC;

#ifndef _NOT_USE_MIL_
	if (!theApp.clMilMain.m_mImg_MarkSet) {
		MbufAlloc2d(theApp.clMilMain.m_mSysID, MARK_BUF_SIZE_X, MARK_BUF_SIZE_Y, 8L + M_UNSIGNED, BufferAttributes, &theApp.clMilMain.m_mImg_MarkSet);
		if (theApp.clMilMain.m_mImg_MarkSet)		MbufClear(theApp.clMilMain.m_mImg_MarkSet, 192L);
		else
		{
			AfxMessageBox(_T("Failed to Create m_mImg_MarkSet Buffer!"));
			return FALSE;
		}
	}
	else {
		MbufClear(theApp.clMilMain.m_mImg_MarkSet, 192L);
	}

	//--- SetMarkDisp ���ۻ���
	if (!theApp.clMilMain.m_mImgDisp_MarkSet)
	{
		MbufAlloc2d(theApp.clMilMain.m_mSysID, draw.right - draw.left, draw.bottom - draw.top, 8L + M_UNSIGNED, BufferAttributes, &theApp.clMilMain.m_mImgDisp_MarkSet);
		if (theApp.clMilMain.m_mImgDisp_MarkSet)	MbufClear(theApp.clMilMain.m_mImgDisp_MarkSet, 192L);
		else
		{
			AfxMessageBox(_T("m_mImgDisp_MarkSet Buffer Create Fail"));
			return FALSE;
		}
	}

	if (theApp.clMilMain.m_mImgDisp_MarkSet)
	{
		MdispSelectWindow(theApp.clMilMain.m_mDisID_MarkSet, theApp.clMilMain.m_mImgDisp_MarkSet, hwnd); 

		if (theApp.clMilMain.m_mDisID_MarkSet)
		{
			MdispControl(theApp.clMilMain.m_mDisID_MarkSet, M_OVERLAY_SHOW, M_ENABLE);
			MdispControl(theApp.clMilMain.m_mDisID_MarkSet, M_OVERLAY, M_ENABLE);
			MdispInquire(theApp.clMilMain.m_mDisID_MarkSet, M_OVERLAY_ID, &theApp.clMilMain.m_mOverlay_MarkSet);
			MgraControl(M_DEFAULT, M_BACKGROUND_MODE, M_TRANSPARENT);
		}
	}

	if (fi_findType == 0) {
		if (m_mlModelID_D)
		{
			MmodFree(m_mlModelID_D);
			m_mlModelID_D = M_NULL;
		}
		//MmodAlloc(theApp.clMilMain.m_mSysID, M_GEOMETRIC, M_DEFAULT, &m_mlModelID_D);
		MmodRestore(file, theApp.clMilMain.m_mSysID, M_DEFAULT, &m_mlModelID_D);

		if (!m_mlModelID_D || MappGetError(M_CURRENT, M_NULL))
		{
			LOG_ERROR(ENG_EDIC::en_mil, L"MIL_ERR : Failed to restore the mark image (buffer)");
			return FALSE;
		}

		if (m_mlModelID_D) {
			MmodDraw(M_DEFAULT, m_mlModelID_D, theApp.clMilMain.m_mImg_MarkSet, M_DRAW_IMAGE, M_DEFAULT, M_DEFAULT);
		}
	}
	else {
		if (m_mlPATID_D)
		{
			MpatFree(m_mlPATID_D);
			m_mlPATID_D = M_NULL;
		}
		MpatAlloc(theApp.clMilMain.m_mSysID, M_DEFAULT, M_DEFAULT, &m_mlPATID_D);
		MpatRestore(file, theApp.clMilMain.m_mSysID, M_DEFAULT, &m_mlPATID_D);
		if (!m_mlPATID_D || MappGetError(M_CURRENT, M_NULL))
		{
			LOG_ERROR(ENG_EDIC::en_mil, L"MIL_ERR : Failed to restore the mark image (buffer)");
			return FALSE;
		}

		if (m_mlPATID_D) {
			MpatDraw(M_DEFAULT, m_mlPATID_D, theApp.clMilMain.m_mImg_MarkSet, M_DRAW_IMAGE, M_DEFAULT, M_DEFAULT);
		}
	}

	BOOL bRtnVal = FALSE;
	// �ִ� Max-SizeȮ��
	CPoint	iMaxSizeP;
	iMaxSizeP.x = draw.right - draw.left;
	iMaxSizeP.y = draw.bottom - draw.top;
	// ��ũ����� �ش� ������ �°� ȯ��...
	CPoint	iTmpSizeP;
	iTmpSizeP.x = (int)(m_pstMarkModel[2].iSizeP.x * fi_dRate + 0.5); 
	iTmpSizeP.y = (int)(m_pstMarkModel[2].iSizeP.y * fi_dRate + 0.5);
	// ���÷��̹��� ���� ����� ũ�� �� ��Ŀ���� ���͸�...
	if (iTmpSizeP.x > iTmpSizeP.y)
	{
		if (iTmpSizeP.x > iMaxSizeP.x)
		{
			fi_dRate = (double)iMaxSizeP.x / (double)m_pstMarkModel[2].iSizeP.x;
			theApp.clMilMain.MARKSET_DISP_RATE = fi_dRate;
			bRtnVal = false;
		}
	}
	else
	{
		if (iTmpSizeP.y > iMaxSizeP.y)
		{
			fi_dRate = (double)iMaxSizeP.y / (double)m_pstMarkModel[2].iSizeP.y;
			theApp.clMilMain.MARKSET_DISP_RATE = fi_dRate;
			bRtnVal = false;
		}
	}

	MbufClear(theApp.clMilMain.m_mImgDisp_MarkSet, 192);
	MimResize(theApp.clMilMain.m_mImg_MarkSet, theApp.clMilMain.m_mImgDisp_MarkSet, fi_dRate, fi_dRate, M_DEFAULT);
#endif
	return TRUE;
}

/* desc : PAT ���Ͽ� MASK ���� ����, Mark Set������ ��� */
VOID CMilModel::SaveMask_PAT(UINT8 mark_no)
{
#ifndef _NOT_USE_MIL_
	MbufClear(theApp.clMilMain.MilMask, 0x00);
	MbufClear(theApp.clMilMain.m_mImg_MarkSet, 0x00);
	MbufPut(theApp.clMilMain.MilMask, theApp.clMilMain.pucMaskBuf);

	MpatMask(m_mlPATID[mark_no], M_DEFAULT, theApp.clMilMain.MilMask, M_DONT_CARE, M_DEFAULT);

	MgraColor(M_DEFAULT, M_COLOR_RED);
	MpatDraw(M_DEFAULT, m_mlPATID[mark_no], theApp.clMilMain.m_mImg_MarkSet, M_DRAW_IMAGE + M_DRAW_DONT_CARE, M_DEFAULT, M_DEFAULT);

	MbufClear(theApp.clMilMain.m_mImgDisp_MarkSet, 192);
	MimResize(theApp.clMilMain.m_mImg_MarkSet, theApp.clMilMain.m_mImgDisp_MarkSet, theApp.clMilMain.MARKSET_DISP_RATE, theApp.clMilMain.MARKSET_DISP_RATE, M_DEFAULT); 
#endif
}

/* desc : MMF ���Ͽ� MASK ���� ����, Mark Set������ ��� */
VOID CMilModel::SaveMask_MOD(UINT8 mark_no)
{
#ifndef _NOT_USE_MIL_
	MbufClear(theApp.clMilMain.MilMask, 0x00);
	MbufClear(theApp.clMilMain.m_mImg_MarkSet, 0x00);
	MbufPut(theApp.clMilMain.MilMask, theApp.clMilMain.pucMaskBuf);

	MmodMask(m_mlModelID[mark_no], M_DEFAULT, theApp.clMilMain.MilMask, M_DONT_CARE, M_DEFAULT);

	MgraColor(M_DEFAULT, M_COLOR_RED);
	MmodDraw(M_DEFAULT, m_mlModelID[mark_no], theApp.clMilMain.m_mImg_MarkSet, M_DRAW_IMAGE + M_DRAW_DONT_CARE, M_DEFAULT, M_DEFAULT);

	MbufClear(theApp.clMilMain.m_mImgDisp_MarkSet, 192);
	MimResize(theApp.clMilMain.m_mImg_MarkSet, theApp.clMilMain.m_mImgDisp_MarkSet, theApp.clMilMain.MARKSET_DISP_RATE, theApp.clMilMain.MARKSET_DISP_RATE, M_DEFAULT); 
#endif
}

/* desc : PAT MARK ���� */
VOID CMilModel::PatMarkSave(CString fi_strFileName, UINT8 mark_no)
{
	wchar_t* wchar_str;
	int char_str_len;

	wchar_str = fi_strFileName.GetBuffer(fi_strFileName.GetLength());
	char_str_len = WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, NULL, 0, NULL, NULL);

#ifndef _NOT_USE_MIL_
	MpatSave(wchar_str, m_mlPATID[mark_no]);
#endif
}

/* desc : MMF MARK ���� */
VOID CMilModel::ModMarkSave(CString fi_strFileName, UINT8 mark_no)
{
	wchar_t* wchar_str;
	int char_str_len;

	wchar_str = fi_strFileName.GetBuffer(fi_strFileName.GetLength());
	char_str_len = WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, NULL, 0, NULL, NULL);

#ifndef _NOT_USE_MIL_
	MmodSave(wchar_str, m_mlModelID[mark_no], M_DEFAULT);
#endif
}

/* desc : Clear Mask(MMF) */
VOID CMilModel::MaskClear_MOD(CPoint fi_iSizeP, UINT8 mark_no)
{
#ifndef _NOT_USE_MIL_
	MIL_ID MilMod;
	MilMod = m_mlModelID[mark_no];

	MbufClear(theApp.clMilMain.MilMask, 0x00);
	memset(theApp.clMilMain.pucMaskBuf, 0x00, (fi_iSizeP.x * fi_iSizeP.y));
	MbufPut(theApp.clMilMain.MilMask, theApp.clMilMain.pucMaskBuf);

	MmodMask(MilMod, M_DEFAULT, theApp.clMilMain.MilMask, M_DONT_CARE, M_DEFAULT);
#endif
}

/* desc : Clear Mask(PAT) */
VOID CMilModel::MaskClear_PAT(CPoint fi_iSizeP, UINT8 mark_no)
{
#ifndef _NOT_USE_MIL_
	MIL_ID MilPat;
	MilPat = m_mlPATID[mark_no];

	MbufClear(theApp.clMilMain.MilMask, 0x00);
	memset(theApp.clMilMain.pucMaskBuf, 0x00, (fi_iSizeP.x * fi_iSizeP.y));
	MbufPut(theApp.clMilMain.MilMask, theApp.clMilMain.pucMaskBuf);

	MpatMask(MilPat, M_DEFAULT, theApp.clMilMain.MilMask, M_DONT_CARE, M_DEFAULT);
#endif
}

/* desc : Find Center (Mark Set������ ���) */
VOID CMilModel::MarkSetCenterFind(int fi_length, int fi_curSmoothness, double* fi_NumEdgeMIN_X, double* fi_NumEdgeMAX_X, double* fi_NumEdgeMIN_Y, double* fi_NumEdgeMAX_Y, int* fi_NumEdgeFound)
{
	MIL_ID MilTmpResult = M_NULL;
	MIL_ID MilTmpEdgeContext = M_NULL;
	MIL_INT  NumEdgeFound = 0L;

#ifndef _NOT_USE_MIL_
	MedgeAllocResult(theApp.clMilMain.m_mSysID, M_DEFAULT, &theApp.clMilMain.MilEdgeResult[0]);
	MedgeAlloc(theApp.clMilMain.m_mSysID, M_CONTOUR, M_DEFAULT, &theApp.clMilMain.MilEdgeContext);

	MilTmpResult = theApp.clMilMain.MilEdgeResult[0];
	MilTmpEdgeContext = theApp.clMilMain.MilEdgeContext;

	MedgeControl(MilTmpEdgeContext, M_ACCURACY, M_HIGH);
	MedgeControl(MilTmpEdgeContext, M_FILTER_SMOOTHNESS, fi_curSmoothness);
	MedgeControl(MilTmpEdgeContext, M_FILTER_TYPE, M_SHEN);
	MedgeControl(MilTmpEdgeContext, M_FLOAT_MODE, M_DISABLE);
	MedgeControl(MilTmpEdgeContext, M_ANGLE_ACCURACY, M_HIGH);
	MedgeControl(MilTmpEdgeContext, M_MAGNITUDE_TYPE, M_SQR_NORM);
	MedgeControl(MilTmpEdgeContext, M_CHAIN_ALL_NEIGHBORS, M_ENABLE);
	MedgeControl(MilTmpEdgeContext, M_THRESHOLD_MODE, M_HIGH);
	MedgeControl(MilTmpEdgeContext, M_THRESHOLD_TYPE, M_FULL_HYSTERESIS);
	MedgeControl(MilTmpEdgeContext, M_FILL_GAP_DISTANCE, 20);

	MedgeControl(MilTmpEdgeContext, M_BOX_X_MIN + M_TYPE_MIL_DOUBLE, M_ENABLE);
	MedgeControl(MilTmpEdgeContext, M_BOX_X_MAX + M_TYPE_MIL_DOUBLE, M_ENABLE);
	MedgeControl(MilTmpEdgeContext, M_BOX_Y_MIN + M_TYPE_MIL_DOUBLE, M_ENABLE);
	MedgeControl(MilTmpEdgeContext, M_BOX_Y_MAX + M_TYPE_MIL_DOUBLE, M_ENABLE);

	MedgeControl(MilTmpEdgeContext, M_LENGTH + M_SORT1_DOWN, M_ENABLE);
	MedgeControl(MilTmpEdgeContext, M_CENTER_OF_GRAVITY_X, M_ENABLE);
	MedgeControl(MilTmpEdgeContext, M_CENTER_OF_GRAVITY_Y, M_ENABLE);

	MedgeCalculate(MilTmpEdgeContext, theApp.clMilMain.m_mImg_MarkSet_Ori, M_NULL, M_NULL, M_NULL, MilTmpResult, M_DEFAULT); 

	MedgeSelect(MilTmpResult, M_EXCLUDE, M_LENGTH, M_LESS, fi_length, M_NULL);

	MedgeGetResult(MilTmpResult, M_DEFAULT, M_NUMBER_OF_CHAINS + M_TYPE_MIL_INT, &NumEdgeFound, M_NULL);

	// edge�� 1.0 �̸��� ��� 1.0���� edge�� �׷�����... mgracontrol�� �ȸ���.. �ϴ��� 1.0 ���� ���� 
	if (theApp.clMilMain.MARKSET_DISP_RATE >= 1.0)
	{
		MgraControl(M_DEFAULT, M_DRAW_ZOOM_X, theApp.clMilMain.MARKSET_DISP_RATE);
		MgraControl(M_DEFAULT, M_DRAW_ZOOM_Y, theApp.clMilMain.MARKSET_DISP_RATE);
		MgraColor(M_DEFAULT, M_COLOR_RED);
		MedgeDraw(M_DEFAULT, MilTmpResult, theApp.clMilMain.m_mOverlay_MarkSet, M_DRAW_EDGES, M_DEFAULT, M_DEFAULT);
	}

	if (NumEdgeFound <= 0)	return;			

	if (NumEdgeFound > 100) NumEdgeFound = 100;

	*fi_NumEdgeFound = (int)NumEdgeFound;

	MedgeGetResult(MilTmpResult, M_DEFAULT, M_BOX_X_MIN + M_TYPE_MIL_DOUBLE, fi_NumEdgeMIN_X, NULL);
	MedgeGetResult(MilTmpResult, M_DEFAULT, M_BOX_X_MAX + M_TYPE_MIL_DOUBLE, fi_NumEdgeMAX_X, NULL);
	MedgeGetResult(MilTmpResult, M_DEFAULT, M_BOX_Y_MIN + M_TYPE_MIL_DOUBLE, fi_NumEdgeMIN_Y, NULL);
	MedgeGetResult(MilTmpResult, M_DEFAULT, M_BOX_Y_MAX + M_TYPE_MIL_DOUBLE, fi_NumEdgeMAX_Y, NULL);

	MIL_DOUBLE* NumEdgeLength = new MIL_DOUBLE[100];
	MIL_DOUBLE* NumEdgeCenter_X = new MIL_DOUBLE[100];
	MIL_DOUBLE* NumEdgeCenter_Y = new MIL_DOUBLE[100];

	MedgeGetResult(MilTmpResult, M_DEFAULT, M_LENGTH, NumEdgeLength, NULL);
	MedgeGetResult(MilTmpResult, M_DEFAULT, M_CENTER_OF_GRAVITY_X + M_TYPE_MIL_DOUBLE, NumEdgeCenter_X, NULL);
	MedgeGetResult(MilTmpResult, M_DEFAULT, M_CENTER_OF_GRAVITY_Y + M_TYPE_MIL_DOUBLE, NumEdgeCenter_Y, NULL);

#endif
}

/* desc : Mark ���� �׸� �� ����ϴ� MIL �Լ� */
VOID CMilModel::DrawMarkInfo_UseMIL(UINT8 fi_smooth, UINT8 mark_no)
{
#ifndef _NOT_USE_MIL_
	MgraControl(M_DEFAULT, M_DRAW_ZOOM_X, theApp.clMilMain.MARKSET_DISP_RATE);
	MgraControl(M_DEFAULT, M_DRAW_ZOOM_Y, theApp.clMilMain.MARKSET_DISP_RATE);

	MgraColor(M_DEFAULT, M_COLOR_YELLOW);
	MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_SMOOTHNESS, fi_smooth);
	MmodDraw(M_DEFAULT, m_mlModelID[mark_no], theApp.clMilMain.m_mOverlay_MarkSet, M_DRAW_EDGES, M_DEFAULT, M_DEFAULT);
	if (theApp.clMilMain.MilMask) {
		MpatDraw(M_DEFAULT, m_mlPATID[mark_no], theApp.clMilMain.MilMask, M_DRAW_DONT_CARE, M_DEFAULT, M_DEFAULT);
		//MbufGet(theApp.clMilMain.MilMask, theApp.clMilMain.pucMaskBuf); // lk91 ���� �������� üũ...ó������ ��������..
	}
#endif
}

/* desc : Set Mask */
VOID CMilModel::Mask_MarkSet(CRect rectTmp, CPoint iTmpSizeP, CRect rectFill, int fi_color, bool bMask)
{
#ifndef _NOT_USE_MIL_
	if (bMask) {
		int x, y;
		int ibufSize = iTmpSizeP.x;
		if (rectTmp.left < 0)
			rectTmp.left = 0;
		if (rectTmp.top < 0)
			rectTmp.top = 0;
		if (rectTmp.right > iTmpSizeP.x)
			rectTmp.right = iTmpSizeP.x;
		if (rectTmp.bottom > iTmpSizeP.y)
			rectTmp.bottom = iTmpSizeP.y;

		for (x = rectTmp.left; x < rectTmp.right; x++)
		{
			for (y = rectTmp.top; y < rectTmp.bottom; y++)
			{
				theApp.clMilMain.pucMaskBuf[y * ibufSize + x] = 0xff;	// pModel �� �߰�
			}
		}
	}
	MgraControl(M_DEFAULT, M_DRAW_ZOOM_X, theApp.clMilMain.MARKSET_DISP_RATE);
	MgraControl(M_DEFAULT, M_DRAW_ZOOM_Y, theApp.clMilMain.MARKSET_DISP_RATE);
	MgraColor(M_DEFAULT, theApp.clMilMain.MilColorConvert(fi_color));
	MgraRectFill(M_DEFAULT, theApp.clMilMain.m_mOverlay_MarkSet, rectTmp.left, rectTmp.top, rectTmp.right, rectTmp.bottom);
#endif
}

#ifndef _NOT_USE_MIL_
/* desc : Geometric Model Find */
BOOL CMilModel::RunModelFind(MIL_ID graph_id, MIL_ID grab_id, BOOL angle, UINT8 img_id, UINT8 dlg_id, UINT8 mark_no, BOOL useMilDisp, UINT8 img_proc)
{
	if (!m_mlModelID[mark_no])
		return FALSE;

	if (dlg_id == DISP_TYPE_MARK_LIVE || dlg_id == DISP_TYPE_MARK)
		dlg_id = DISP_TYPE_MARK_LIVE;
	
	// lk91 ProcImage() ���..
	theApp.clMilMain.ProcImage(m_camid-1, 0);
	
	if (img_proc == 1)// lk91 �̹��� ó�� ���� ���� �߰�
	{
		theApp.clMilMain.ProcImage(m_camid-1, 1);
	}
	else {
		theApp.clMilMain.ProcImage(m_camid-1, 0);
	}

	if(m_findMode[mark_no] == 1){
		bool brtn = RunPATFind(graph_id, theApp.clMilMain.m_mImgProc[m_camid - 1], angle, img_id, dlg_id, mark_no, useMilDisp);
		return brtn;
	}

	// lk91 �ӽ�!!!!!!!!!!!!!!!
	//int HIST_NUM_INTENSITIES = 256;
	//MIL_ID HistResult;
	//MIL_INT    HistValues[256];
	//MimAllocResult(theApp.clMilMain.m_mSysID, HIST_NUM_INTENSITIES, M_HIST_LIST, &HistResult);
	//MimHistogram(theApp.clMilMain.m_mImgProc[m_camid - 1], HistResult);
	//MimGetResult(HistResult, M_VALUE, HistValues);

	//int    Contrast[256] = { 0, };
	//int cnt = 0;
	//bool Chk = true;

	//for (int i = 0; i < 256; i++) {
	//	if (Chk && HistValues[i] > 10000) { // lk91 10000�� �����Ѱ� ������ ��...���� Ȯ���� ����... ã�ƺ���
	//		Contrast[cnt] = i;
	//		cnt++;
	//		Chk = false;
	//	}
	//	else if (!Chk && HistValues[i] < 10000) {
	//		Contrast[cnt] = i;
	//		cnt++;
	//		Chk = true;
	//	}
	//}
	//
	//int result_con;
	//if (cnt == 1)
	//	result_con = 255 - Contrast[0];
	//else
	//	result_con = Contrast[cnt - 1] - Contrast[0];

	//MimFree(HistResult);
	// lk91 �ӽ�!!!!!!!!!!!!!!!

	UINT8 u8ScaleSize = 0x00 /* equal */; /* 0x01 : ScaleUp, 0x02 : ScaleDown */
	BOOL bSucc = TRUE;
	TCHAR tzData[128] = { NULL };
	UINT32 u32GrabWidth = 0, u32GrabHeight = 0, i, j, k;
	UINT32 u32MaxCount;
	if(dlg_id != DISP_TYPE_CALB_EXPO)
		u32MaxCount = m_pstConfig->mark_find.max_mark_find;
	else
		u32MaxCount = 2;

	DOUBLE dbGrabCentX = 0.0f, dbGrabCentY = 0.0f/*,*/ /*dbLineDist = 0.0f,*/ /*dbModelSize*/;
	DOUBLE dbPosX, dbPosY, dbScore, dbScale, dbAngle, dbCovg, dbFitErr, dbCircleR;
	MIL_ID mlResult = M_NULL, miModResults = 0, miOperation;

	/* ����ü �� �ʱ�ȭ (!!! �ݵ�� �ʱ�ȭ !!!) */
	m_u8MarkFindGet = 0x00;
	ResetMarkResult();

	/* Get the size of grabbed image (buffer) */
	u32GrabWidth = (UINT32)MbufInquire(grab_id, M_SIZE_X, NULL);
	u32GrabHeight = (UINT32)MbufInquire(grab_id, M_SIZE_Y, NULL);
	dbGrabCentX = (DOUBLE)u32GrabWidth / 2.0f;	/* ��ǥ ���� Zero-based �̹Ƿ� */
	dbGrabCentY = (DOUBLE)u32GrabHeight / 2.0f;
	CRect tmpSearchROI;
	// Search ROI 
	if (dlg_id == DISP_TYPE_CALB_CAMSPEC) {
		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_OFFSET_X, theApp.clMilMain.rectSearhROI_Calb_CamSpec.left);
		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_OFFSET_Y, theApp.clMilMain.rectSearhROI_Calb_CamSpec.top);
		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_SIZE_X, theApp.clMilMain.rectSearhROI_Calb_CamSpec.right - theApp.clMilMain.rectSearhROI_Calb_CamSpec.left);
		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_SIZE_Y, theApp.clMilMain.rectSearhROI_Calb_CamSpec.bottom - theApp.clMilMain.rectSearhROI_Calb_CamSpec.top);
		tmpSearchROI = theApp.clMilMain.rectSearhROI_Calb_CamSpec;
	}
	else if (dlg_id == DISP_TYPE_CALB_EXPO) {
		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_OFFSET_X, theApp.clMilMain.rectSearhROI_Calb_Step.left);
		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_OFFSET_Y, theApp.clMilMain.rectSearhROI_Calb_Step.top);
		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_SIZE_X, theApp.clMilMain.rectSearhROI_Calb_Step.right - theApp.clMilMain.rectSearhROI_Calb_Step.left);
		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_SIZE_Y, theApp.clMilMain.rectSearhROI_Calb_Step.bottom - theApp.clMilMain.rectSearhROI_Calb_Step.top);
		tmpSearchROI = theApp.clMilMain.rectSearhROI_Calb_Step;
	}
	else if (dlg_id == DISP_TYPE_CALB_ACCR) {
		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_OFFSET_X, theApp.clMilMain.rectSearhROI_Calb_Accr.left);
		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_OFFSET_Y, theApp.clMilMain.rectSearhROI_Calb_Accr.top);
		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_SIZE_X, theApp.clMilMain.rectSearhROI_Calb_Accr.right - theApp.clMilMain.rectSearhROI_Calb_Accr.left);
		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_SIZE_Y, theApp.clMilMain.rectSearhROI_Calb_Accr.bottom - theApp.clMilMain.rectSearhROI_Calb_Accr.top);
		tmpSearchROI = theApp.clMilMain.rectSearhROI_Calb_Accr;
	}
	else {
		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_OFFSET_X, theApp.clMilMain.rectSearhROI[m_camid - 1].left);
		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_OFFSET_Y, theApp.clMilMain.rectSearhROI[m_camid - 1].top);
		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_SIZE_X, theApp.clMilMain.rectSearhROI[m_camid - 1].right - theApp.clMilMain.rectSearhROI[m_camid - 1].left);
		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_SIZE_Y, theApp.clMilMain.rectSearhROI[m_camid - 1].bottom - theApp.clMilMain.rectSearhROI[m_camid - 1].top);
	}
	MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_ACTIVE_EDGELS, 100.0);
	MmodPreprocess(m_mlModelID[mark_no], M_DEFAULT); // find �Ҷ� Preprocess �ϵ���...

	MIL_DOUBLE m_ModelType;
	MmodInquire(m_mlModelID[mark_no], M_DEFAULT, M_MODEL_TYPE, &m_ModelType);

	/* Allocate a result buffer. */
	MmodAllocResult(theApp.clMilMain.m_mSysID, M_DEFAULT, &mlResult);

	/* Find the model */
	MmodFind(m_mlModelID[mark_no], theApp.clMilMain.m_mImgProc[m_camid - 1], mlResult); 

	/* Get the number of models found. */
	MmodGetResult(mlResult, M_DEFAULT, M_NUMBER + M_TYPE_MIL_INT, &miModResults);
	if (miModResults < 1)
	{
		swprintf_s(tzData, 128, L"Not all mark were found : search (%u) / found (%u)",
			m_u8MarkFindSet, UINT8(miModResults));
		LOG_WARN(ENG_EDIC::en_mil, tzData);
		/* ��� ���� �޸� ���� */
		MmodFree(mlResult);
		if (useMilDisp) {
			theApp.clMilDisp.DrawOverlayDC(false, dlg_id, m_camid - 1);
			CString sTmp = _T("NG");
			if (dlg_id == DISP_TYPE_CALB_EXPO) {
				theApp.clMilDisp.AddTextList(dlg_id, 0, 30, 90, sTmp, eM_COLOR_RED, 10, 20, VISION_FONT_TEXT, true);
				theApp.clMilDisp.AddTextList(dlg_id, 1, 30, 90, sTmp, eM_COLOR_RED, 10, 20, VISION_FONT_TEXT, true);
			}
			else {
				theApp.clMilDisp.AddTextList(dlg_id, m_camid - 1, 30, 90, sTmp, eM_COLOR_RED, 10, 20, VISION_FONT_TEXT, true);
			}
			theApp.clMilDisp.DrawOverlayDC(true, dlg_id, m_camid - 1);
		}
		return FALSE;
	}
#ifdef _DEBUG
	TRACE(L"CMilModel::RunModelFind::Mark Find Count = %d\n", miModResults);
#endif
	/* If a model was found above the acceptance threshold */
	if (miModResults > u32MaxCount)	miModResults = u32MaxCount;
	/* Read results */
	//MmodGetResult(mlResult, M_DEFAULT, M_INDEX + M_TYPE_MIL_INT, m_pMilIndex);	/* �� ������ ������� ��, �˻��� ���� ��ȣ */, �� ���� ������� ����
	MmodGetResult(mlResult, M_DEFAULT, M_POSITION_X, m_pFindPosX);
	MmodGetResult(mlResult, M_DEFAULT, M_POSITION_Y, m_pFindPosY);
	MmodGetResult(mlResult, M_DEFAULT, M_SCORE, m_pFindScore);
	MmodGetResult(mlResult, M_DEFAULT, M_SCALE, m_pFindScale);
	MmodGetResult(mlResult, M_DEFAULT, M_ANGLE, m_pFindAngle);
	MmodGetResult(mlResult, M_DEFAULT, M_MODEL_COVERAGE, m_pFindCovg);
	MmodGetResult(mlResult, M_DEFAULT, M_FIT_ERROR, m_pFindFitErr);
	if (m_ModelType == M_CIRCLE)
		MmodGetResult(mlResult, M_DEFAULT, M_RADIUS, m_pFindCircleRadius);

	/* ���� ��ũ �˻� ����� ���, �˻� ��� ������ŭ ã�� ���ߴ��� ���� Ȯ�� */
	if (ENG_MMSM::en_cent_side == m_enMarkMethod || ENG_MMSM::en_multi_only == m_enMarkMethod)
	{
		if (miModResults < m_u8MarkFindSet)
		{
			swprintf_s(tzData, 128, L"Not all were found (RunModelFind) : find_set (%u) > find_get (%u)",
				m_u8MarkFindSet, UINT8(miModResults));
			LOG_ERROR(ENG_EDIC::en_mil, tzData);
			bSucc = FALSE;	/* �۾� ���� */
		}
		/* ���� ã���� �ϴ� �������� ���� �˻��� ���, Score or Scale ���� ���� ���� �� (��������)���� ���� */
		else if (miModResults > m_u8MarkFindSet)
		{
			/* ������ ���� ������ ���ϸ� �ǹǷ� n-1 ������ŭ �� */
			for (i = 0; i < miModResults - 1; i++)
			{
				k = i;	/* ���� �ε��� */
				for (j = i + 1; j < miModResults; j++)
				{
#if 0
					/* SCALE ���� 0.000f ���� ���� ����� ���ϼ��� ����  */
					if (abs(1.0f - m_pFindScale[j]) > abs(1.0f - m_pFindScale[k]))	k = j;	/* 0.0 ���� ���� ����� ���� ���� ������ */
#else
					/* SCORE ���� 100.000 ���� ���� ����� ���ϼ��� ���� */
					if (m_pFindScore[j] > m_pFindScore[k])	k = j;	/* ���� ���� ���ϼ��� ���� ������ */
#endif
				}
				/* ���� ��ġ �� �ӽ� ��� */
				dbPosX = m_pFindPosX[i];
				dbPosY = m_pFindPosY[i];
				dbScore = m_pFindScore[i];
				dbScale = m_pFindScale[i];
				dbAngle = m_pFindAngle[i];
				dbCovg = m_pFindCovg[i];
				dbFitErr = m_pFindFitErr[i];
				dbCircleR = m_pFindCircleRadius[i];
				/* ���� �˻� ���� ��ġ�� ���� ���� ������ ���� */
				m_pFindPosX[i] = m_pFindPosX[k];
				m_pFindPosY[i] = m_pFindPosY[k];
				m_pFindScore[i] = m_pFindScore[k];
				m_pFindScale[i] = m_pFindScale[k];
				m_pFindAngle[i] = m_pFindAngle[k];
				m_pFindCovg[i] = m_pFindCovg[k];
				m_pFindFitErr[i] = m_pFindFitErr[k];
				m_pFindCircleRadius[i] = m_pFindCircleRadius[k];
				/* ���� ���� ���� ���� �˻� ���� ������ ���� */
				m_pFindPosX[k] = dbPosX;
				m_pFindPosY[k] = dbPosY;
				m_pFindScore[k] = dbScore;
				m_pFindScale[k] = dbScale;
				m_pFindAngle[k] = dbAngle;
				m_pFindCovg[k] = dbCovg;
				m_pFindFitErr[k] = dbFitErr;
				m_pFindCircleRadius[k] = dbCircleR;
			}
		}
		/* ������ �˻� ������ ���� ã���� �ϴ� ������ ���� */
		miModResults = m_u8MarkFindSet;
	}
	// X ���� sorting 
	//if (ENG_MMSM::en_ph_step == m_enMarkMethod) {
	if (dlg_id == DISP_TYPE_CALB_EXPO) {
		if (miModResults < m_u8MarkFindSet)
		{
			swprintf_s(tzData, 128, L"Not all were found (RunModelFind) : find_set (%u) > find_get (%u)",
				m_u8MarkFindSet, UINT8(miModResults));
			LOG_ERROR(ENG_EDIC::en_mil, tzData);
			bSucc = FALSE;	/* �۾� ���� */
		}
		/* ���� ã���� �ϴ� �������� ���� �˻��� ���, Score or Scale ���� ���� ���� �� (��������)���� ���� */
			/* ������ ���� ������ ���ϸ� �ǹǷ� n-1 ������ŭ �� */
		for (i = 0; i < miModResults - 1; i++)
		{
			k = i;	/* ���� �ε��� */
			for (j = i + 1; j < miModResults; j++)
			{
#if 0
				/* SCALE ���� 0.000f ���� ���� ����� ���ϼ��� ����  */
				if (abs(1.0f - m_pFindScale[j]) > abs(1.0f - m_pFindScale[k]))	k = j;	/* 0.0 ���� ���� ����� ���� ���� ������ */
#else
				/* SCORE ���� 100.000 ���� ���� ����� ���ϼ��� ���� */
				//if (m_pFindScore[j] > m_pFindScore[k])	k = j;	/* ���� ���� ���ϼ��� ���� ������ */
				if (m_pFindPosX[j] < m_pFindPosX[k])	k = j;	/* ���� ���� ���ϼ��� ���� ������ */
#endif
			}
			/* ���� ��ġ �� �ӽ� ��� */
			dbPosX = m_pFindPosX[i];
			dbPosY = m_pFindPosY[i];
			dbScore = m_pFindScore[i];
			dbScale = m_pFindScale[i];
			dbAngle = m_pFindAngle[i];
			dbCovg = m_pFindCovg[i];
			dbFitErr = m_pFindFitErr[i];
			dbCircleR = m_pFindCircleRadius[i];
			/* ���� �˻� ���� ��ġ�� ���� ���� ������ ���� */
			m_pFindPosX[i] = m_pFindPosX[k];
			m_pFindPosY[i] = m_pFindPosY[k];
			m_pFindScore[i] = m_pFindScore[k];
			m_pFindScale[i] = m_pFindScale[k];
			m_pFindAngle[i] = m_pFindAngle[k];
			m_pFindCovg[i] = m_pFindCovg[k];
			m_pFindFitErr[i] = m_pFindFitErr[k];
			m_pFindCircleRadius[i] = m_pFindCircleRadius[k];
			/* ���� ���� ���� ���� �˻� ���� ������ ���� */
			m_pFindPosX[k] = dbPosX;
			m_pFindPosY[k] = dbPosY;
			m_pFindScore[k] = dbScore;
			m_pFindScale[k] = dbScale;
			m_pFindAngle[k] = dbAngle;
			m_pFindCovg[k] = dbCovg;
			m_pFindFitErr[k] = dbFitErr;
			m_pFindCircleRadius[k] = dbCircleR;

		}
		
		/* ������ �˻� ������ ���� ã���� �ϴ� ������ ���� */
		miModResults = m_u8MarkFindSet;
	}

	//if (useMilDisp && (dlg_id == DISP_TYPE_MARK_LIVE || dlg_id == DISP_TYPE_MARK)) {
	if (useMilDisp && (dlg_id != DISP_TYPE_EXPO && dlg_id != DISP_TYPE_CALB_EXPO)) {
		if (bSucc)
		{

			theApp.clMilDisp.DrawOverlayDC(false, dlg_id, m_camid-1); //lk91 �ӽ�, fi_No�ε�.. m_camid ����
			for (i = 0; i < miModResults; i++) {
				CRect	rectArea;
				rectArea.left = (int)(m_pFindPosX[i] - m_pstMarkModel[mark_no].iOffsetP.x);
				rectArea.top = (int)(m_pFindPosY[i] - m_pstMarkModel[mark_no].iOffsetP.y);
				rectArea.right = (int)(rectArea.left + m_pstMarkModel[mark_no].iSizeP.x);
				rectArea.bottom = (int)(rectArea.top + m_pstMarkModel[mark_no].iSizeP.y);

				//if (dlg_id == DISP_TYPE_CALB_CAMSPEC || dlg_id == DISP_TYPE_CALB_EXPO) {
				if (dlg_id == DISP_TYPE_CALB_CAMSPEC || dlg_id == DISP_TYPE_CALB_ACCR) {
					theApp.clMilDisp.AddCrossList(dlg_id, m_camid - 1, (int)m_pFindPosX[i]- tmpSearchROI.left, (int)m_pFindPosY[i]- tmpSearchROI.top, 200, 200, COLOR_GREEN);
					theApp.clMilDisp.AddBoxList(dlg_id, m_camid - 1, rectArea.left- tmpSearchROI.left, rectArea.top- tmpSearchROI.top,
						rectArea.right- tmpSearchROI.left, rectArea.bottom- tmpSearchROI.top, PS_SOLID, COLOR_GREEN);
				}
				else {
					theApp.clMilDisp.AddCrossList(dlg_id, m_camid - 1, (int)m_pFindPosX[i], (int)m_pFindPosY[i], 200, 200, COLOR_GREEN);
					theApp.clMilDisp.AddBoxList(dlg_id, m_camid - 1, rectArea.left, rectArea.top, rectArea.right, rectArea.bottom, PS_SOLID, COLOR_GREEN);
				}

				/* ���� ���� ���α׷����� ȣ���� �����, �Ʒ� ī�޶� ȸ�� ���� ������ ��ǥ �� �������� ���� */
				if (!angle)
				{
					/* ����� ī�޶��� ȸ�� ������ ���� �˻��� ��ũ�� �߽� ��ġ�� �޶��� */
					GetFindRotatePosXY(dbGrabCentX, dbGrabCentY, m_pFindPosX[i], m_pFindPosY[i]);
				}

				/* Grabbed Image�� �߽� ��ǥ���� �˻��� Mark�� �߽� ��ǥ ���� ������ �Ÿ� ���ϱ� */
				m_pFindDist[i] = CalcLineDist(m_pFindPosX[i], m_pFindPosY[i], dbGrabCentX, dbGrabCentY);
				/* Scale Rate ���� ���� ���� 100 percent �������� ȯ���Ͽ� ���� �� ���� */
				m_pFindScale[i] = (1.0f - fabs(1.0f - m_pFindScale[i])) * 100.0f;
				/* �˻��� ��ü�� ũ�Ⱑ ��ϵ� ��ũ ���� ��� ū ������ �ƴ��� �÷��� ���� */
				if (m_pFindScale[i] > 1.0f)			u8ScaleSize = 0x01;
				else if (m_pFindScale[i] < 1.0f)	u8ScaleSize = 0x02;
				/* --------------------------------------------------------------------------------------- */
				/* �˻� ��� ���յǴ��� Ȯ�� */
				m_pstModResult[i].SetValue(UINT32(m_pMilIndex[i]),
					m_pFindScale[i], m_pFindScore[i], m_pFindDist[i],
					m_pFindAngle[i], m_pFindCovg[i], m_pFindFitErr[i],
					m_pFindPosX[i], m_pFindPosY[i], u8ScaleSize, m_pFindCircleRadius[i]);
			}
			// lk91 model ã�� ������ ��ã��....
			//miOperation = M_MODEL + M_DRAW_BOX + M_DRAW_EDGES + M_DRAW_POSITION;
			////MgraColor(graph_id/*M_DEFAULT*/, M_COLOR_RED);
			//MgraColor(M_DEFAULT, M_COLOR_RED);
			////MmodDraw(M_DEFAULT, mlResult, theApp.clMilMain.m_mOverlay_Mark_Live, miOperation, 0, M_DEFAULT);
			//MmodDraw(M_DEFAULT, mlResult, theApp.clMilMain.m_mImgDisp_Mark_Live, miOperation, 0, M_DEFAULT);
			theApp.clMilDisp.DrawOverlayDC(true, dlg_id, m_camid - 1);
		}
		else {
			theApp.clMilDisp.DrawOverlayDC(false, dlg_id, m_camid - 1);
			CString sTmp = _T("NG");
			//AddTextList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, CString fi_sText, int fi_color, int fi_iSizeX, int fi_iSizeY, CString fi_sFont, bool fi_bEgdeFlag)
			theApp.clMilDisp.AddTextList(dlg_id, m_camid - 1, 30, 90, sTmp, eM_COLOR_RED, 10, 20, VISION_FONT_TEXT, true);
			theApp.clMilDisp.DrawOverlayDC(true, dlg_id, m_camid - 1);
		}
	}
	else if (useMilDisp && dlg_id == DISP_TYPE_CALB_EXPO) {
		//for (int j = 0; j < 2; j++) {
			if (bSucc)
			{
				theApp.clMilDisp.DrawOverlayDC(false, dlg_id, 0);
				for (i = 0; i < miModResults; i++) {
					CRect	rectArea;
					rectArea.left = (int)(m_pFindPosX[i] - m_pstMarkModel[mark_no].iOffsetP.x);
					rectArea.top = (int)(m_pFindPosY[i] - m_pstMarkModel[mark_no].iOffsetP.y);
					rectArea.right = (int)(rectArea.left + m_pstMarkModel[mark_no].iSizeP.x);
					rectArea.bottom = (int)(rectArea.top + m_pstMarkModel[mark_no].iSizeP.y);
					theApp.clMilDisp.AddCrossList(dlg_id, i, (int)m_pFindPosX[i] - tmpSearchROI.left, (int)m_pFindPosY[i] - tmpSearchROI.top, 200, 200, COLOR_GREEN);
					theApp.clMilDisp.AddBoxList(dlg_id, i, rectArea.left - tmpSearchROI.left, rectArea.top - tmpSearchROI.top,
						rectArea.right - tmpSearchROI.left, rectArea.bottom - tmpSearchROI.top, PS_SOLID, COLOR_GREEN);

					/* ���� ���� ���α׷����� ȣ���� �����, �Ʒ� ī�޶� ȸ�� ���� ������ ��ǥ �� �������� ���� */
					if (!angle)
					{
						/* ����� ī�޶��� ȸ�� ������ ���� �˻��� ��ũ�� �߽� ��ġ�� �޶��� */
						GetFindRotatePosXY(dbGrabCentX, dbGrabCentY, m_pFindPosX[i], m_pFindPosY[i]);
					}

					/* Grabbed Image�� �߽� ��ǥ���� �˻��� Mark�� �߽� ��ǥ ���� ������ �Ÿ� ���ϱ� */
					m_pFindDist[i] = CalcLineDist(m_pFindPosX[i], m_pFindPosY[i], dbGrabCentX, dbGrabCentY);
					/* Scale Rate ���� ���� ���� 100 percent �������� ȯ���Ͽ� ���� �� ���� */
					m_pFindScale[i] = (1.0f - fabs(1.0f - m_pFindScale[i])) * 100.0f;
					/* �˻��� ��ü�� ũ�Ⱑ ��ϵ� ��ũ ���� ��� ū ������ �ƴ��� �÷��� ���� */
					if (m_pFindScale[i] > 1.0f)			u8ScaleSize = 0x01;
					else if (m_pFindScale[i] < 1.0f)	u8ScaleSize = 0x02;
					/* --------------------------------------------------------------------------------------- */
					/* �˻� ��� ���յǴ��� Ȯ�� */
					m_pstModResult[i].SetValue(UINT32(m_pMilIndex[i]),
						m_pFindScale[i], m_pFindScore[i], m_pFindDist[i],
						m_pFindAngle[i], m_pFindCovg[i], m_pFindFitErr[i],
						m_pFindPosX[i], m_pFindPosY[i], u8ScaleSize, m_pFindCircleRadius[i]);
				}

				theApp.clMilDisp.DrawOverlayDC(true, dlg_id, 0);
			}
			else {
				theApp.clMilDisp.DrawOverlayDC(false, dlg_id, 0);
				CString sTmp = _T("NG");
				//AddTextList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, CString fi_sText, int fi_color, int fi_iSizeX, int fi_iSizeY, CString fi_sFont, bool fi_bEgdeFlag)
				theApp.clMilDisp.AddTextList(dlg_id, 0, 30, 90, sTmp, eM_COLOR_RED, 10, 20, VISION_FONT_TEXT, true);
				theApp.clMilDisp.AddTextList(dlg_id, 1, 30, 90, sTmp, eM_COLOR_RED, 10, 20, VISION_FONT_TEXT, true);
				theApp.clMilDisp.DrawOverlayDC(true, dlg_id, 0);
			}

		//}
	}
	else {
		if (bSucc)
		{
			for (i = 0; i < miModResults; i++)
			{
#if 1
				miOperation = M_MODEL + M_DRAW_BOX + M_DRAW_EDGES + M_DRAW_POSITION;
				//			miOperation	= M_TARGET+M_DRAW_BOX+M_DRAW_EDGES+M_DRAW_POSITION;
				//			miOperation	= M_DRAW_EDGES+M_DRAW_BOX+M_DRAW_POSITION/*+M_DRAW_WEIGHT_REGIONS*/;

				/* �˻��� ���� Target (Matching ���� �ƴ϶�)�� ���� �׸� �׷� �ֱ� */
				MgraColor(graph_id/*M_DEFAULT*/, M_COLOR_RED);
				/* Draw the model */
				MmodDraw(M_DEFAULT, mlResult, grab_id, miOperation, i, M_DEFAULT);
#endif
				/* ���� ���� ���α׷����� ȣ���� �����, �Ʒ� ī�޶� ȸ�� ���� ������ ��ǥ �� �������� ���� */
				if (!angle)
				{
					/* ����� ī�޶��� ȸ�� ������ ���� �˻��� ��ũ�� �߽� ��ġ�� �޶��� */
					GetFindRotatePosXY(dbGrabCentX, dbGrabCentY, m_pFindPosX[i], m_pFindPosY[i]);
				}

				/* Grabbed Image�� �߽� ��ǥ���� �˻��� Mark�� �߽� ��ǥ ���� ������ �Ÿ� ���ϱ� */
				m_pFindDist[i] = CalcLineDist(m_pFindPosX[i], m_pFindPosY[i], dbGrabCentX, dbGrabCentY);
				/* Scale Rate ���� ���� ���� 100 percent �������� ȯ���Ͽ� ���� �� ���� */
				m_pFindScale[i] = (1.0f - fabs(1.0f - m_pFindScale[i])) * 100.0f;
				/* �˻��� ��ü�� ũ�Ⱑ ��ϵ� ��ũ ���� ��� ū ������ �ƴ��� �÷��� ���� */
				if (m_pFindScale[i] > 1.0f)			u8ScaleSize = 0x01;
				else if (m_pFindScale[i] < 1.0f)	u8ScaleSize = 0x02;
				/* --------------------------------------------------------------------------------------- */
				/* �˻� ��� ���յǴ��� Ȯ�� */
				m_pstModResult[i].SetValue(UINT32(m_pMilIndex[i]),
					m_pFindScale[i], m_pFindScore[i], m_pFindDist[i],
					m_pFindAngle[i], m_pFindCovg[i], m_pFindFitErr[i],
					m_pFindPosX[i], m_pFindPosY[i], u8ScaleSize, m_pFindCircleRadius[i]);
			}
		}
	}

	/* ��� ���� �޸� ���� */
	MmodFree(mlResult);

	/* 1 �� �̻� ã�ƾ� �� */
	m_u8MarkFindGet = UINT8(miModResults);
	return miModResults > 0;
}

/* desc : Pattern Matching Mark Find */
BOOL CMilModel::RunPATFind(MIL_ID graph_id, MIL_ID grab_id, BOOL angle, UINT8 img_id, UINT8 dispType, UINT8 mark_no, BOOL useMilDisp)
{
	if (!m_mlPATID[mark_no])
		return FALSE;

	UINT8 u8ScaleSize = 0x00 /* equal */; /* 0x01 : ScaleUp, 0x02 : ScaleDown */
	BOOL bSucc = TRUE;
	TCHAR tzData[128] = { NULL };
	UINT32 u32GrabWidth = 0, u32GrabHeight = 0, i, j, k;
	UINT32 u32MaxCount = m_pstConfig->mark_find.max_mark_find;
	DOUBLE dbGrabCentX = 0.0f, dbGrabCentY = 0.0f/*,*/ /*dbLineDist = 0.0f,*/ /*dbModelSize*/;
	DOUBLE dbPosX, dbPosY, dbScore, dbScale, dbAngle, dbCovg, dbFitErr;
	MIL_ID mlResult = M_NULL, miOperation;
	MIL_INT miPatResults = 0;

	/* ����ü �� �ʱ�ȭ (!!! �ݵ�� �ʱ�ȭ !!!) */
	m_u8MarkFindGet = 0x00;
	ResetMarkResult();

	/* Get the size of grabbed image (buffer) */
	u32GrabWidth = (UINT32)MbufInquire(grab_id, M_SIZE_X, NULL);
	u32GrabHeight = (UINT32)MbufInquire(grab_id, M_SIZE_Y, NULL);
	dbGrabCentX = (DOUBLE)u32GrabWidth / 2.0f;	/* ��ǥ ���� Zero-based �̹Ƿ� */
	dbGrabCentY = (DOUBLE)u32GrabHeight / 2.0f;

	CRect tmpSearchROI;
	// Search ROI 
	if (dispType == DISP_TYPE_CALB_CAMSPEC) {
		MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_SEARCH_OFFSET_X, theApp.clMilMain.rectSearhROI_Calb_CamSpec.left);
		MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_SEARCH_OFFSET_Y, theApp.clMilMain.rectSearhROI_Calb_CamSpec.top);
		MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_SEARCH_SIZE_X, theApp.clMilMain.rectSearhROI_Calb_CamSpec.right - theApp.clMilMain.rectSearhROI_Calb_CamSpec.left);
		MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_SEARCH_SIZE_Y, theApp.clMilMain.rectSearhROI_Calb_CamSpec.bottom - theApp.clMilMain.rectSearhROI_Calb_CamSpec.top);
		tmpSearchROI = theApp.clMilMain.rectSearhROI_Calb_CamSpec;
	}
	else if (dispType == DISP_TYPE_CALB_EXPO) {
		MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_SEARCH_OFFSET_X, theApp.clMilMain.rectSearhROI_Calb_Step.left);
		MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_SEARCH_OFFSET_Y, theApp.clMilMain.rectSearhROI_Calb_Step.top);
		MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_SEARCH_SIZE_X, theApp.clMilMain.rectSearhROI_Calb_Step.right - theApp.clMilMain.rectSearhROI_Calb_Step.left);
		MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_SEARCH_SIZE_Y, theApp.clMilMain.rectSearhROI_Calb_Step.bottom - theApp.clMilMain.rectSearhROI_Calb_Step.top);
		tmpSearchROI = theApp.clMilMain.rectSearhROI_Calb_Step;
	}
	else if (dispType == DISP_TYPE_CALB_ACCR) {
		MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_SEARCH_OFFSET_X, theApp.clMilMain.rectSearhROI_Calb_Accr.left);
		MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_SEARCH_OFFSET_Y, theApp.clMilMain.rectSearhROI_Calb_Accr.top);
		MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_SEARCH_SIZE_X, theApp.clMilMain.rectSearhROI_Calb_Accr.right - theApp.clMilMain.rectSearhROI_Calb_Accr.left);
		MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_SEARCH_SIZE_Y, theApp.clMilMain.rectSearhROI_Calb_Accr.bottom - theApp.clMilMain.rectSearhROI_Calb_Accr.top);
		tmpSearchROI = theApp.clMilMain.rectSearhROI_Calb_Accr;
	}
	else {
		MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_SEARCH_OFFSET_X, theApp.clMilMain.rectSearhROI[m_camid - 1].left);
		MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_SEARCH_OFFSET_Y, theApp.clMilMain.rectSearhROI[m_camid - 1].top);
		MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_SEARCH_SIZE_X, theApp.clMilMain.rectSearhROI[m_camid - 1].right - theApp.clMilMain.rectSearhROI[m_camid - 1].left);
		MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_SEARCH_SIZE_Y, theApp.clMilMain.rectSearhROI[m_camid - 1].bottom - theApp.clMilMain.rectSearhROI[m_camid - 1].top);
	}

	MpatPreprocess(m_mlPATID[mark_no], M_DEFAULT, theApp.clMilMain.m_mImgProc[m_camid - 1]);


	MpatAllocResult(theApp.clMilMain.m_mSysID, M_DEFAULT, &mlResult);

	//MpatFind(m_mlPATID, theApp.clMilMain.m_mImg[0], mlResult); 
	MpatFind(m_mlPATID[mark_no], grab_id, mlResult);

	MpatGetResult(mlResult, M_GENERAL, M_NUMBER + M_TYPE_MIL_INT, &miPatResults);

	/* Get the number of models found. */
	if (miPatResults < 1)
	{
		swprintf_s(tzData, 128, L"Not all PAT mark were found : search (%u) / found (%u)",
			m_u8MarkFindSet, UINT8(miPatResults));
		LOG_WARN(ENG_EDIC::en_mil, tzData);
		/* ��� ���� �޸� ���� */
		MpatFree(mlResult);
		if (useMilDisp) {
			theApp.clMilDisp.DrawOverlayDC(false, dispType, m_camid - 1);
			CString sTmp = _T("NG");
			theApp.clMilDisp.AddTextList(dispType, m_camid - 1, 30, 90, sTmp, eM_COLOR_RED, 10, 20, VISION_FONT_TEXT, true);
			theApp.clMilDisp.DrawOverlayDC(true, dispType, m_camid - 1);
		}

		return FALSE;
	}
#ifdef _DEBUG
	TRACE(L"CMilModel::RunPATFind::Mark Find Count = %d\n", miPatResults);
#endif
	/* If a model was found above the acceptance threshold */
	if (miPatResults > u32MaxCount)	miPatResults = u32MaxCount;
	/* Read results */
	MpatGetResult(mlResult, M_DEFAULT, M_INDEX + M_TYPE_MIL_INT, m_pMilIndex);	/* �� ������ ������� ��, �˻��� ���� ��ȣ */
	MpatGetResult(mlResult, M_DEFAULT, M_POSITION_X, m_pFindPosX);
	MpatGetResult(mlResult, M_DEFAULT, M_POSITION_Y, m_pFindPosY);
	MpatGetResult(mlResult, M_DEFAULT, M_SCORE, m_pFindScore);

	for (int i = 0; i < miPatResults; i++) {
		m_pFindScale[i] = 0.0;
		m_pFindAngle[i] = 0.0;
		m_pFindCovg[i] = 0.0;
		m_pFindFitErr[i] = 0.0;
	}

	/* ���� ��ũ �˻� ����� ���, �˻� ��� ������ŭ ã�� ���ߴ��� ���� Ȯ�� */
	if (ENG_MMSM::en_cent_side == m_enMarkMethod || ENG_MMSM::en_multi_only == m_enMarkMethod)
	{
		if (miPatResults < m_u8MarkFindSet)
		{
			swprintf_s(tzData, 128, L"Not all were found (RunPATFind) : find_set (%u) > find_get (%u)",
				m_u8MarkFindSet, UINT8(miPatResults));
			LOG_ERROR(ENG_EDIC::en_mil, tzData);
			bSucc = FALSE;	/* �۾� ���� */
		}
		/* ���� ã���� �ϴ� �������� ���� �˻��� ���, Score or Scale ���� ���� ���� �� (��������)���� ���� */
		else if (miPatResults > m_u8MarkFindSet)
		{
			/* ������ ���� ������ ���ϸ� �ǹǷ� n-1 ������ŭ �� */
			for (i = 0; i < miPatResults - 1; i++)
			{
				k = i;	/* ���� �ε��� */
				for (j = i + 1; j < miPatResults; j++)
				{
#if 0
					/* SCALE ���� 0.000f ���� ���� ����� ���ϼ��� ����  */
					if (abs(1.0f - m_pFindScale[j]) > abs(1.0f - m_pFindScale[k]))	k = j;	/* 0.0 ���� ���� ����� ���� ���� ������ */
#else
					/* SCORE ���� 100.000 ���� ���� ����� ���ϼ��� ���� */
					if (m_pFindScore[j] > m_pFindScore[k])	k = j;	/* ���� ���� ���ϼ��� ���� ������ */
#endif
				}
				/* ���� ��ġ �� �ӽ� ��� */
				dbPosX = m_pFindPosX[i];
				dbPosY = m_pFindPosY[i];
				dbScore = m_pFindScore[i];
				dbScale = m_pFindScale[i];
				dbAngle = m_pFindAngle[i];
				dbCovg = m_pFindCovg[i];
				dbFitErr = m_pFindFitErr[i];
				/* ���� �˻� ���� ��ġ�� ���� ���� ������ ���� */
				m_pFindPosX[i] = m_pFindPosX[k];
				m_pFindPosY[i] = m_pFindPosY[k];
				m_pFindScore[i] = m_pFindScore[k];
				m_pFindScale[i] = m_pFindScale[k];
				m_pFindAngle[i] = m_pFindAngle[k];
				m_pFindCovg[i] = m_pFindCovg[k];
				m_pFindFitErr[i] = m_pFindFitErr[k];
				/* ���� ���� ���� ���� �˻� ���� ������ ���� */
				m_pFindPosX[k] = dbPosX;
				m_pFindPosY[k] = dbPosY;
				m_pFindScore[k] = dbScore;
				m_pFindScale[k] = dbScale;
				m_pFindAngle[k] = dbAngle;
				m_pFindCovg[k] = dbCovg;
				m_pFindFitErr[k] = dbFitErr;
			}
		}
		/* ������ �˻� ������ ���� ã���� �ϴ� ������ ���� */
		miPatResults = m_u8MarkFindSet;
	}
	// X ���� sorting 
	//if (ENG_MMSM::en_ph_step == m_enMarkMethod) {
	if (dispType == DISP_TYPE_CALB_EXPO) {
		if (miPatResults < m_u8MarkFindSet)
		{
			swprintf_s(tzData, 128, L"Not all were found (RunModelFind) : find_set (%u) > find_get (%u)",
				m_u8MarkFindSet, UINT8(miPatResults));
			LOG_ERROR(ENG_EDIC::en_mil, tzData);
			bSucc = FALSE;	/* �۾� ���� */
		}
		/* ���� ã���� �ϴ� �������� ���� �˻��� ���, Score or Scale ���� ���� ���� �� (��������)���� ���� */
			/* ������ ���� ������ ���ϸ� �ǹǷ� n-1 ������ŭ �� */
		for (i = 0; i < miPatResults - 1; i++)
		{
			k = i;	/* ���� �ε��� */
			for (j = i + 1; j < miPatResults; j++)
			{
#if 0
				/* SCALE ���� 0.000f ���� ���� ����� ���ϼ��� ����  */
				if (abs(1.0f - m_pFindScale[j]) > abs(1.0f - m_pFindScale[k]))	k = j;	/* 0.0 ���� ���� ����� ���� ���� ������ */
#else
				/* SCORE ���� 100.000 ���� ���� ����� ���ϼ��� ���� */
				//if (m_pFindScore[j] > m_pFindScore[k])	k = j;	/* ���� ���� ���ϼ��� ���� ������ */
				if (m_pFindPosX[j] < m_pFindPosX[k])	k = j;	/* ���� ���� ���ϼ��� ���� ������ */
#endif
			}
			/* ���� ��ġ �� �ӽ� ��� */
			dbPosX = m_pFindPosX[i];
			dbPosY = m_pFindPosY[i];
			dbScore = m_pFindScore[i];
			dbScale = m_pFindScale[i];
			dbAngle = m_pFindAngle[i];
			dbCovg = m_pFindCovg[i];
			dbFitErr = m_pFindFitErr[i];
			/* ���� �˻� ���� ��ġ�� ���� ���� ������ ���� */
			m_pFindPosX[i] = m_pFindPosX[k];
			m_pFindPosY[i] = m_pFindPosY[k];
			m_pFindScore[i] = m_pFindScore[k];
			m_pFindScale[i] = m_pFindScale[k];
			m_pFindAngle[i] = m_pFindAngle[k];
			m_pFindCovg[i] = m_pFindCovg[k];
			m_pFindFitErr[i] = m_pFindFitErr[k];
			/* ���� ���� ���� ���� �˻� ���� ������ ���� */
			m_pFindPosX[k] = dbPosX;
			m_pFindPosY[k] = dbPosY;
			m_pFindScore[k] = dbScore;
			m_pFindScale[k] = dbScale;
			m_pFindAngle[k] = dbAngle;
			m_pFindCovg[k] = dbCovg;
			m_pFindFitErr[k] = dbFitErr;
		}

		/* ������ �˻� ������ ���� ã���� �ϴ� ������ ���� */
		miPatResults = m_u8MarkFindSet;
	}

	if (useMilDisp && (dispType != DISP_TYPE_EXPO && dispType != DISP_TYPE_CALB_EXPO)) {
		if (bSucc)
		{
			theApp.clMilDisp.DrawOverlayDC(false, dispType, m_camid - 1);
			for (i = 0; i < miPatResults; i++) {
				CRect	rectArea;
				rectArea.left = (int)(m_pFindPosX[i] - m_pstMarkModel[mark_no].iOffsetP.x);
				rectArea.top = (int)(m_pFindPosY[i] - m_pstMarkModel[mark_no].iOffsetP.y);
				rectArea.right = (int)(rectArea.left + m_pstMarkModel[mark_no].iSizeP.x);
				rectArea.bottom = (int)(rectArea.top + m_pstMarkModel[mark_no].iSizeP.y);

				//if (dispType == DISP_TYPE_CALB_CAMSPEC || dispType == DISP_TYPE_CALB_EXPO) {
				if (dispType == DISP_TYPE_CALB_CAMSPEC || dispType == DISP_TYPE_CALB_ACCR) {
					theApp.clMilDisp.AddCrossList(dispType, m_camid - 1, (int)m_pFindPosX[i] - tmpSearchROI.left, (int)m_pFindPosY[i] - tmpSearchROI.top, 50, 50, COLOR_GREEN);
					theApp.clMilDisp.AddBoxList(dispType, m_camid - 1, rectArea.left - tmpSearchROI.left, rectArea.top - tmpSearchROI.top,
						rectArea.right - tmpSearchROI.left, rectArea.bottom - tmpSearchROI.top, PS_SOLID, COLOR_GREEN);
				}
				else {
					theApp.clMilDisp.AddCrossList(dispType, m_camid - 1, (int)m_pFindPosX[i], (int)m_pFindPosY[i], 50, 50, COLOR_GREEN);
					theApp.clMilDisp.AddBoxList(dispType, m_camid - 1, rectArea.left, rectArea.top, rectArea.right, rectArea.bottom, PS_SOLID, COLOR_GREEN);
				}

				/* ���� ���� ���α׷����� ȣ���� �����, �Ʒ� ī�޶� ȸ�� ���� ������ ��ǥ �� �������� ���� */
				if (!angle)
				{
					/* ����� ī�޶��� ȸ�� ������ ���� �˻��� ��ũ�� �߽� ��ġ�� �޶��� */
					GetFindRotatePosXY(dbGrabCentX, dbGrabCentY, m_pFindPosX[i], m_pFindPosY[i]);
				}

				/* Grabbed Image�� �߽� ��ǥ���� �˻��� Mark�� �߽� ��ǥ ���� ������ �Ÿ� ���ϱ� */
				m_pFindDist[i] = CalcLineDist(m_pFindPosX[i], m_pFindPosY[i], dbGrabCentX, dbGrabCentY);
				/* Scale Rate ���� ���� ���� 100 percent �������� ȯ���Ͽ� ���� �� ���� */
				m_pFindScale[i] = (1.0f - fabs(1.0f - m_pFindScale[i])) * 100.0f;
				/* �˻��� ��ü�� ũ�Ⱑ ��ϵ� ��ũ ���� ��� ū ������ �ƴ��� �÷��� ���� */
				if (m_pFindScale[i] > 1.0f)			u8ScaleSize = 0x01;
				else if (m_pFindScale[i] < 1.0f)	u8ScaleSize = 0x02;
				/* --------------------------------------------------------------------------------------- */
				/* �˻� ��� ���յǴ��� Ȯ�� */
				m_pstModResult[i].SetValue(UINT32(m_pMilIndex[i]),
					m_pFindScale[i], m_pFindScore[i], m_pFindDist[i],
					m_pFindAngle[i], m_pFindCovg[i], m_pFindFitErr[i],
					m_pFindPosX[i], m_pFindPosY[i], u8ScaleSize, 0.0);
			}

			theApp.clMilDisp.DrawOverlayDC(true, dispType, m_camid - 1);
		}
		else {
			theApp.clMilDisp.DrawOverlayDC(false, dispType, m_camid - 1);
			CString sTmp = _T("NG");
			//AddTextList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, CString fi_sText, int fi_color, int fi_iSizeX, int fi_iSizeY, CString fi_sFont, bool fi_bEgdeFlag)
			theApp.clMilDisp.AddTextList(dispType, m_camid - 1, 30, 90, sTmp, eM_COLOR_RED, 10, 20, VISION_FONT_TEXT, true);
			theApp.clMilDisp.DrawOverlayDC(true, dispType, m_camid - 1);
		}
	}
	else if (useMilDisp && dispType == DISP_TYPE_CALB_EXPO) {
		for (int j = 0; j < 2; j++) {
			if (bSucc)
			{
				theApp.clMilDisp.DrawOverlayDC(false, dispType, j);
				for (i = 0; i < miPatResults; i++) {
					CRect	rectArea;
					rectArea.left = (int)(m_pFindPosX[i] - m_pstMarkModel[mark_no].iOffsetP.x);
					rectArea.top = (int)(m_pFindPosY[i] - m_pstMarkModel[mark_no].iOffsetP.y);
					rectArea.right = (int)(rectArea.left + m_pstMarkModel[mark_no].iSizeP.x);
					rectArea.bottom = (int)(rectArea.top + m_pstMarkModel[mark_no].iSizeP.y);
					theApp.clMilDisp.AddCrossList(dispType, m_camid - 1, (int)m_pFindPosX[i] - tmpSearchROI.left, (int)m_pFindPosY[i] - tmpSearchROI.top, 50, 50, COLOR_GREEN);
					theApp.clMilDisp.AddBoxList(dispType, m_camid - 1, rectArea.left - tmpSearchROI.left, rectArea.top - tmpSearchROI.top,
						rectArea.right - tmpSearchROI.left, rectArea.bottom - tmpSearchROI.top, PS_SOLID, COLOR_GREEN);

					/* ���� ���� ���α׷����� ȣ���� �����, �Ʒ� ī�޶� ȸ�� ���� ������ ��ǥ �� �������� ���� */
					if (!angle)
					{
						/* ����� ī�޶��� ȸ�� ������ ���� �˻��� ��ũ�� �߽� ��ġ�� �޶��� */
						GetFindRotatePosXY(dbGrabCentX, dbGrabCentY, m_pFindPosX[i], m_pFindPosY[i]);
					}

					/* Grabbed Image�� �߽� ��ǥ���� �˻��� Mark�� �߽� ��ǥ ���� ������ �Ÿ� ���ϱ� */
					m_pFindDist[i] = CalcLineDist(m_pFindPosX[i], m_pFindPosY[i], dbGrabCentX, dbGrabCentY);
					/* Scale Rate ���� ���� ���� 100 percent �������� ȯ���Ͽ� ���� �� ���� */
					m_pFindScale[i] = (1.0f - fabs(1.0f - m_pFindScale[i])) * 100.0f;
					/* �˻��� ��ü�� ũ�Ⱑ ��ϵ� ��ũ ���� ��� ū ������ �ƴ��� �÷��� ���� */
					if (m_pFindScale[i] > 1.0f)			u8ScaleSize = 0x01;
					else if (m_pFindScale[i] < 1.0f)	u8ScaleSize = 0x02;
					/* --------------------------------------------------------------------------------------- */
					/* �˻� ��� ���յǴ��� Ȯ�� */
					m_pstModResult[i].SetValue(UINT32(m_pMilIndex[i]),
						m_pFindScale[i], m_pFindScore[i], m_pFindDist[i],
						m_pFindAngle[i], m_pFindCovg[i], m_pFindFitErr[i],
						m_pFindPosX[i], m_pFindPosY[i], u8ScaleSize, 0.0);
				}

				theApp.clMilDisp.DrawOverlayDC(true, dispType, j);
			}
			else {
				theApp.clMilDisp.DrawOverlayDC(false, dispType, j);
				CString sTmp = _T("NG");
				//AddTextList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, CString fi_sText, int fi_color, int fi_iSizeX, int fi_iSizeY, CString fi_sFont, bool fi_bEgdeFlag)
				theApp.clMilDisp.AddTextList(dispType, j, 30, 90, sTmp, eM_COLOR_RED, 10, 20, VISION_FONT_TEXT, true);
				theApp.clMilDisp.DrawOverlayDC(true, dispType, j);
			}

		}
	}
	else {
		if (bSucc)
		{
			for (i = 0; i < miPatResults; i++)
			{
#if 1
				miOperation = M_MODEL + M_DRAW_BOX + M_DRAW_EDGES + M_DRAW_POSITION;
				//			miOperation	= M_TARGET+M_DRAW_BOX+M_DRAW_EDGES+M_DRAW_POSITION;
				//			miOperation	= M_DRAW_EDGES+M_DRAW_BOX+M_DRAW_POSITION/*+M_DRAW_WEIGHT_REGIONS*/;

							/* �˻��� ���� Target (Matching ���� �ƴ϶�)�� ���� �׸� �׷� �ֱ� */
				MgraColor(graph_id/*M_DEFAULT*/, M_COLOR_RED);
				/* Draw the model */
				MpatDraw(M_DEFAULT, mlResult, grab_id, miOperation, i, M_DEFAULT);
#endif
				/* ���� ���� ���α׷����� ȣ���� �����, �Ʒ� ī�޶� ȸ�� ���� ������ ��ǥ �� �������� ���� */
				if (!angle)
				{
					/* ����� ī�޶��� ȸ�� ������ ���� �˻��� ��ũ�� �߽� ��ġ�� �޶��� */
					GetFindRotatePosXY(dbGrabCentX, dbGrabCentY, m_pFindPosX[i], m_pFindPosY[i]);
				}

				/* Grabbed Image�� �߽� ��ǥ���� �˻��� Mark�� �߽� ��ǥ ���� ������ �Ÿ� ���ϱ� */
				m_pFindDist[i] = CalcLineDist(m_pFindPosX[i], m_pFindPosY[i], dbGrabCentX, dbGrabCentY);
				/* Scale Rate ���� ���� ���� 100 percent �������� ȯ���Ͽ� ���� �� ���� */
				m_pFindScale[i] = (1.0f - fabs(1.0f - m_pFindScale[i])) * 100.0f;
				/* �˻��� ��ü�� ũ�Ⱑ ��ϵ� ��ũ ���� ��� ū ������ �ƴ��� �÷��� ���� */
				if (m_pFindScale[i] > 1.0f)			u8ScaleSize = 0x01;
				else if (m_pFindScale[i] < 1.0f)	u8ScaleSize = 0x02;
				/* --------------------------------------------------------------------------------------- */
				/* �˻� ��� ���յǴ��� Ȯ�� */
				m_pstModResult[i].SetValue(UINT32(m_pMilIndex[i]),
					m_pFindScale[i], m_pFindScore[i], m_pFindDist[i],
					m_pFindAngle[i], m_pFindCovg[i], m_pFindFitErr[i],
					m_pFindPosX[i], m_pFindPosY[i], u8ScaleSize, 0.0);
			}
		}
	}
	/* ��� ���� �޸� ���� */
	MpatFree(mlResult);

	/* 1 �� �̻� ã�ƾ� �� */
	m_u8MarkFindGet = UINT8(miPatResults);
	return miPatResults > 0;
}
#endif

/* desc : Mark ���� ���� - MMF (MIL Model Find File) */
BOOL CMilModel::SetModelDefineMMF(PTCHAR name, PTCHAR mmf, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no)
{
	CUniToChar csCnv;

	/* ���� �����Ǿ� �ִٸ� �ϴ� ���� */
	ReleaseMarkModel(mark_no, 0);
	/* ���� ��� ���� �ʱ�ȭ */
	ResetMarkResult();
	/* ����ϰ��� �ϴ� ���� ���� �ľ� */
	//m_u8ModelRegist = 0x01;	/* MMF �� ���, ������ 1�� �ۿ� �ȵ� */
#ifndef _NOT_USE_MIL_
	/* Model ���� */
	MmodRestore(mmf, theApp.clMilMain.m_mSysID, M_DEFAULT, &m_mlModelID[mark_no]);
	if (!m_mlModelID[mark_no] || MappGetError(M_CURRENT, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"MIL_ERR : Failed to restore the mark image (buffer, mmf)");
		return FALSE;
	}
	/* Model ���� ���� */
	m_pstMarkModel[mark_no].type = UINT32(ENG_MMDT::en_image);
	m_pstMarkModel[mark_no].iSizeP = m_MarkSizeP;
	m_pstMarkModel[mark_no].iOffsetP = m_MarkCenterP;
	sprintf_s(m_pstMarkModel[mark_no].name, MARK_MODEL_NAME_LENGTH, "%s", csCnv.Str2Ansi(name));
	/* Model Size ��� */
#if 0
	m_pstModelSize[0].x = (DOUBLE)MbufInquire(m_mlModelID, M_SIZE_X, M_NULL);
	m_pstModelSize[0].y = (DOUBLE)MbufInquire(m_mlModelID, M_SIZE_Y, M_NULL);
#else	/* ���� MMF�� ��ϵ� �� ũ�� ������ ����� ����. ������ 100 ������ ���� (?) T.T */
#if 0
	DOUBLE dbPixelToMM = m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1);
	m_pstModelSize[0].x = (m_u32MMFMarkWidth / (dbPixelToMM * 1000.0f)) / 2.0f;
	m_pstModelSize[0].y = (m_u32MMFMarkHeight / (dbPixelToMM * 1000.0f)) / 2.0f;
#else
	m_pstModelSize[mark_no].x = 200.0f;
	m_pstModelSize[mark_no].y = 200.0f;
#endif

#endif
	MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_REFERENCE_X, m_pstMarkModel[mark_no].iOffsetP.x);
	MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_REFERENCE_Y, m_pstMarkModel[mark_no].iOffsetP.y);
	MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_NUMBER, 1); // lk91 �̹����� �ϴ� 1���� ����...
	//double smooth = 80.0;
	//DOUBLE dbSmooth = (DOUBLE)uvEng_GetConfig()->mark_find.model_smooth;
	DOUBLE dbSmooth = (DOUBLE)m_pstConfig->mark_find.model_smooth;
	MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_SMOOTHNESS, dbSmooth);
	MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_ACTIVE_EDGELS, 100.0);
	/* Preprocess the search context */
	//MmodPreprocess(m_mlModelID, M_DEFAULT);
#endif

	return TRUE;
}

/* desc : Mark ���� ���� - PAT (MIL Model Find File) */
BOOL CMilModel::SetModelDefinePAT(PTCHAR name, PTCHAR pat, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no)
{
	CUniToChar csCnv;

	/* ���� �����Ǿ� �ִٸ� �ϴ� ���� */
	ReleaseMarkModel(mark_no, 1);
	/* ���� ��� ���� �ʱ�ȭ */
	ResetMarkResult();
	/* ����ϰ��� �ϴ� ���� ���� �ľ� */
	//m_u8ModelRegist = 0x01;	/* MMF �� ���, ������ 1�� �ۿ� �ȵ� */
#ifndef _NOT_USE_MIL_
	if (m_mlPATID[mark_no]) {
		MpatFree(m_mlPATID[mark_no]);
		m_mlPATID[mark_no] = M_NULL;
	}
	MpatAlloc(theApp.clMilMain.m_mSysID, M_DEFAULT, M_DEFAULT, &m_mlPATID[mark_no]);

	/* Model ���� */
	MpatRestore(pat, theApp.clMilMain.m_mSysID, M_DEFAULT, &m_mlPATID[mark_no]);
	if (!m_mlPATID[mark_no] || MappGetError(M_CURRENT, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"MIL_ERR : Failed to restore the mark image (buffer, pat)");
		return FALSE;
	}
	/* Model ���� ���� */
	m_pstMarkModel[mark_no].type = UINT32(ENG_MMDT::en_none);
	m_pstMarkModel[mark_no].iSizeP = m_MarkSizeP;
	m_pstMarkModel[mark_no].iOffsetP = m_MarkCenterP;
	sprintf_s(m_pstMarkModel[mark_no].name, MARK_MODEL_NAME_LENGTH, "%s", csCnv.Str2Ansi(name));
	/* Model Size ��� */
#if 0
	m_pstModelSize[0].x = (DOUBLE)MbufInquire(m_mlModelID, M_SIZE_X, M_NULL);
	m_pstModelSize[0].y = (DOUBLE)MbufInquire(m_mlModelID, M_SIZE_Y, M_NULL);
#else	/* ���� MMF�� ��ϵ� �� ũ�� ������ ����� ����. ������ 100 ������ ���� (?) T.T */
#if 0
	DOUBLE dbPixelToMM = m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1);
	m_pstModelSize[0].x = (m_u32MMFMarkWidth / (dbPixelToMM * 1000.0f)) / 2.0f;
	m_pstModelSize[0].y = (m_u32MMFMarkHeight / (dbPixelToMM * 1000.0f)) / 2.0f;
#else
	m_pstModelSize[mark_no].x = (double)m_MarkSizeP.x;
	m_pstModelSize[mark_no].y = (double)m_MarkSizeP.y;
	//m_pstModelSize[0].x = 200.0f;
	//m_pstModelSize[0].y = 200.0f;
#endif

#endif
	/* Preprocess the search context */

	MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_REFERENCE_X, m_pstMarkModel[0].iOffsetP.x);
	MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_REFERENCE_Y, m_pstMarkModel[0].iOffsetP.y);

	//MpatPreprocess(m_mlPATID, M_DEFAULT, theApp.clMilMain.m_mImg[0]);


#endif

	return TRUE;
}

BOOL CMilModel::SetModelDefine_tot(UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, UINT8 mark_no, TCHAR* file,
	DOUBLE scale_min, DOUBLE scale_max, DOUBLE score_min, DOUBLE score_tgt)
{
#ifndef _NOT_USE_MIL_
	if (ENG_MMDT(m_pstMarkModel[mark_no].type) != ENG_MMDT::en_image ) {

		/* ���� �����Ǿ� �ִٸ� �ϴ� ���� */
		ReleaseMarkModel(mark_no, 0);

		UINT32 i = 0, j = 0;
		LONG lSpeed[4] = { M_LOW, M_MEDIUM, M_HIGH, M_VERY_HIGH };
		LONG lLevel[3] = { M_MEDIUM, M_HIGH, M_VERY_HIGH };
		LONG lFilter[3] = { M_DEFAULT , M_KERNEL , M_RECURSIVE };	/* Filter Mode */
		DOUBLE dbPixel[4] = { NULL };	/* um --> size */
		DOUBLE dbDivSize = 1.0f;	/* Ring�� Circle�� ���, �� ũ�� ����� ��, ���������� �ؾ��ϱ� ������ */
		DOUBLE dbPixelToMM = m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1);
		DOUBLE dbCertainty = 0.0f;
		/* ���� �ֱٿ� ��ϵ� Mark Model ���� */
		if (m_mlModelID[mark_no]) {
			MmodFree(m_mlModelID[mark_no]);
			m_mlModelID[mark_no] = M_NULL;
		}
		MmodAlloc(theApp.clMilMain.m_mSysID, M_GEOMETRIC, M_DEFAULT, &m_mlModelID[mark_no]);
		if (!m_mlModelID[mark_no] || MappGetError(M_CURRENT, M_NULL))
		{
			LOG_ERROR(ENG_EDIC::en_mil, L"MILL_ERR : Failed to allocate the model image (buffer)");
			return FALSE;
		}
		/* Pixel Size ���ϱ� */
		for (j = 0; j < 4; j++)
		{
			dbPixel[j] = m_pstMarkModel[mark_no].param[j + 1] / (dbPixelToMM * 1000.0f);
		}
		switch (m_pstMarkModel[mark_no].type)
		{
			/* �Ʒ� 2���� Type�� �˻� ��� �� ��, ���� (����)�� �ƴ� ������ */
		case ENG_MMDT::en_ring:
		case ENG_MMDT::en_circle:	dbDivSize = 2.0f;
			m_pstModelSize[mark_no].x = dbPixel[0] / 2.0f;
			m_pstModelSize[mark_no].y = dbPixel[0] / 2.0f;	break;

		case ENG_MMDT::en_square:	m_pstModelSize[mark_no].x = dbPixel[0];
			m_pstModelSize[mark_no].y = dbPixel[0];	break;
		case ENG_MMDT::en_cross:
		case ENG_MMDT::en_diamond:
		case ENG_MMDT::en_triangle:
		case ENG_MMDT::en_rectangle:
		case ENG_MMDT::en_ellipse:	m_pstModelSize[mark_no].x = dbPixel[0];
			m_pstModelSize[mark_no].y = dbPixel[1];	break;
		}
		/* Define the model */
		if (m_pstMarkModel[mark_no].param[0] == 1) {
			MmodDefine(m_mlModelID[mark_no], m_pstMarkModel[mark_no].type, M_FOREGROUND_BLACK,
				dbPixel[0] / dbDivSize, dbPixel[1] / dbDivSize,
				dbPixel[2] / dbDivSize, dbPixel[3] / dbDivSize);
		}
		else if (m_pstMarkModel[mark_no].param[0] == 2) {
			MmodDefine(m_mlModelID[mark_no], m_pstMarkModel[mark_no].type, M_FOREGROUND_WHITE,
				dbPixel[0] / dbDivSize, dbPixel[1] / dbDivSize,
				dbPixel[2] / dbDivSize, dbPixel[3] / dbDivSize);
		}
		else {
			MmodDefine(m_mlModelID[mark_no], m_pstMarkModel[mark_no].type, M_ANY,
				dbPixel[0] / dbDivSize, dbPixel[1] / dbDivSize,
				dbPixel[2] / dbDivSize, dbPixel[3] / dbDivSize);
		}
		
		// lk91 MmodControl �����ϰ� ����...
		/* �̹��� ������ ���� �� ���� */
		if (smooth < 0.0f || smooth > 100.0f)	smooth = 50.0f;
		/* ���� �˻� �ӵ� ���� (�� ũ�Ⱑ �۰ų� ��Ī������ ���� ��Ȯ���� �ʿ�� �ϰų�, ���� ������ ������������ ������ ��쿡�� ����Ʈ ���� (M_MEDIUM) ���) */
		MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_SPEED, lSpeed[speed]);
		MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_SMOOTHNESS, smooth);
		/* ���� ��Ȯ���� �䱸�� �� (Accuracy�� �������� ó�� �ӵ��� �������� ��) */
		MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_ACCURACY, M_MEDIUM);
		MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_ACTIVE_EDGELS, 100.0);
		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_POLARITY, M_ANY);

		//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_ACCEPTANCE, M_DEFAULT);
		//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_ANGLE, M_DEFAULT);
		//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_ANGLE_DELTA_NEG, 45.0);
		//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_ANGLE_DELTA_POS, 45.0);
		//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_FIT_ERROR_WEIGHTING_FACTOR, 25);
		MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_TIMEOUT, 2000);
		/* Target Image���� �˻��ϰ��� �ϴ� �ִ� ���� ���� */
		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_NUMBER, MIL_INT(count));
		//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_SCALE_RANGE, M_ENABLE);
		//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SHARED_EDGES, M_ENABLE);
		//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SAVE_TARGET_EDGES, M_ENABLE);
		//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_TARGET_CACHING, M_ENABLE);

	///* ��ϵ� ��ũ ��������, Scale �ּ� / �ִ� �� ���� */
	//	if (scale_min > 0.0f && scale_max > 0.0f)
	//	{
	//		/* scale_min�� scale_max �� ��ȿ�� Ȯ�� �� �߸��� �� ���� */
	//		if (scale_min < 0.5)	scale_min = 0.5f;
	//		if (scale_min > 1.0)	scale_min = 1.0f;
	//		if (scale_max < 1.0)	scale_min = 1.0f;
	//		if (scale_max > 2.0)	scale_min = 2.0f;

	//		/* �� ũ�� ���� ���� */
	//		MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_SEARCH_SCALE_RANGE, M_ENABLE);
	//		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SCALE, 1.0f);
	//		/* �� �־��� ���� ��������, ��ϵ� ��ũ�� ���缺�� ���� ���� ���� ������ �� ��, Score ���� ���� ���� ���� ������ �� */
	//		/* scale_min : 0.5 ~ 1.0, scale_max : 1.0 ~ 2.0 ������ ���� ���� �����߸� �� */
	//		if (0.0f != scale_min)	MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SCALE_MIN_FACTOR, scale_min);
	//		if (0.0f != scale_max)	MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SCALE_MAX_FACTOR, scale_max);
	//	}
	//	/* Score �ּ� �˻� �� ���� */
	//	if (score_min > 0.0f)
	//	{
	//		/* Target Image���� Model �˻� ������ �����̵Ǵ� ��Ī ������ ���� */
	//		/* ���� �� ���� 100 ���� �����ϸ�, �� ���� Active Edge�� Target�� �˻��� Object�� ������ ��Ī�ؾ� �� */
	//		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_ACCEPTANCE, score_min);
	//		//if (score_min > dbCertainty)	dbCertainty = score_min + score_min / 10;	/* �׻� 10% ���� ���� */
	//		//MmodControl(m_mlModelID, M_DEFAULT, M_CERTAINTY, dbCertainty);
	//	}
	//	/* Score (for target) �ּ� �˻� �� ���� */
	//	if (score_tgt > 0.0f)
	//	{
	//		/* score acceptacne ������ �˻��� �� �� target score ���� �� �� ������ ���, �˻� ��󿡼� ���� */
	//		/* ���� grabbed image �� �߿��� mark model�� ������ �̹����� ���� ���� �� ���� �� �˻��ϴ� ����� */
	//		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_ACCEPTANCE_TARGET, score_tgt);
	//	}

		
		/* Preprocess the search context */

		DOUBLE tSize_x = 0, tSize_y = 0;
		MmodInquire(m_mlModelID[mark_no], M_DEFAULT, M_ALLOC_SIZE_X, &tSize_x);
		MmodInquire(m_mlModelID[mark_no], M_DEFAULT, M_ALLOC_SIZE_Y, &tSize_y);
		m_pstMarkModel[mark_no].iSizeP.x = (LONG)tSize_x;
		m_pstMarkModel[mark_no].iSizeP.y = (LONG)tSize_y;

		// lk91 mark ���� �̹����� �ƴѰŴ� Offset�� 0���̶� ���� ���������
		DOUBLE tOffset_x = 0, tOffset_y = 0;

		m_pstMarkModel[mark_no].iOffsetP.x = (LONG)(tSize_x / 2.0);
		m_pstMarkModel[mark_no].iOffsetP.y = (LONG)(tSize_y / 2.0);

	}
	else if (ENG_MMDT(m_pstMarkModel[mark_no].type) == ENG_MMDT::en_image) {
		LONG lSpeed[4] = { M_LOW, M_MEDIUM, M_HIGH, M_VERY_HIGH };
		int tmpFindMode = 0;
		CString strTmp = file;
		int i = strTmp.ReverseFind('.');
		CString strExt;
		if (i > -1) {
			strExt = strTmp.Right(strTmp.GetLength() - i - 1);
			if (strExt.Compare(_T("mmf")) == 0) {
				tmpFindMode = 0;
			}
			else if (strExt.Compare(_T("pat")) == 0) {
				tmpFindMode = 1;
			}
		}
		else {
			return FALSE;
		}
		if (tmpFindMode == 0) {
			/* ���� �����Ǿ� �ִٸ� �ϴ� ���� */
			ReleaseMarkModel(mark_no, 0);
			if (m_mlModelID[mark_no]) {
				MmodFree(m_mlModelID[mark_no]);
				m_mlModelID[mark_no] = M_NULL;
			}
			MmodRestore(file, theApp.clMilMain.m_mSysID, M_DEFAULT, &m_mlModelID[mark_no]);
			MIL_DOUBLE m_type;
			MmodInquire(m_mlModelID[mark_no], M_DEFAULT, M_CONTEXT_TYPE, &m_type);
			if (!m_mlModelID[mark_no] || MappGetError(M_CURRENT, M_NULL))
			{
				LOG_ERROR(ENG_EDIC::en_mil, L"MIL_ERR : Failed to restore the mark image (buffer, mmf)");
				return FALSE;
			}

			if (m_pstMarkModel[mark_no].iSizeP.x == 0 && m_pstMarkModel[mark_no].iSizeP.y == 0) { // lk91 mark �̹����� ���� �������� ����ó��
				DOUBLE tSize_x = 0, tSize_y = 0;
				MmodInquire(m_mlModelID[mark_no], M_DEFAULT, M_ALLOC_SIZE_X, &tSize_x); 
				MmodInquire(m_mlModelID[mark_no], M_DEFAULT, M_ALLOC_SIZE_Y, &tSize_y);
				m_pstMarkModel[mark_no].iSizeP.x = (LONG)tSize_x;
				m_pstMarkModel[mark_no].iSizeP.y = (LONG)tSize_y;
				DOUBLE tOffset_x = 0, tOffset_y = 0;
				MmodInquire(m_mlModelID[mark_no], M_DEFAULT, M_ORIGINAL_X, &tOffset_x);
				MmodInquire(m_mlModelID[mark_no], M_DEFAULT, M_ORIGINAL_Y, &tOffset_y);

				m_pstMarkModel[mark_no].iOffsetP.x = (LONG)tOffset_x;
				m_pstMarkModel[mark_no].iOffsetP.y = (LONG)tOffset_y;
			}

			if (ENG_MMDT::en_image == ENG_MMDT(m_type)) {
				MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_REFERENCE_X, m_pstMarkModel[mark_no].iOffsetP.x);
				MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_REFERENCE_Y, m_pstMarkModel[mark_no].iOffsetP.y);
			}

			// lk91 MmodControl �����ϰ� ����...
			/* �̹��� ������ ���� �� ���� */
			if (smooth < 0.0f || smooth > 100.0f)	smooth = 50.0f;
			/* ���� �˻� �ӵ� ���� (�� ũ�Ⱑ �۰ų� ��Ī������ ���� ��Ȯ���� �ʿ�� �ϰų�, ���� ������ ������������ ������ ��쿡�� ����Ʈ ���� (M_MEDIUM) ���) */
			MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_SPEED, lSpeed[speed]);
			MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_SMOOTHNESS, smooth);
			/* ���� ��Ȯ���� �䱸�� �� (Accuracy�� �������� ó�� �ӵ��� �������� ��) */
			MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_ACCURACY, M_MEDIUM);
			MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_ACTIVE_EDGELS, 100.0);
			MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_POLARITY, M_ANY);

			//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_ACCEPTANCE, M_DEFAULT);
			//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_ANGLE, M_DEFAULT);
			//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_ANGLE_DELTA_NEG, 45.0);
			//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_ANGLE_DELTA_POS, 45.0);
			//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_FIT_ERROR_WEIGHTING_FACTOR, 25);
			MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_TIMEOUT, 2000);
			/* Target Image���� �˻��ϰ��� �ϴ� �ִ� ���� ���� */
			MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_NUMBER, MIL_INT(count));
			//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_SCALE_RANGE, M_ENABLE);
			//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SHARED_EDGES, M_ENABLE);
			//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SAVE_TARGET_EDGES, M_ENABLE);
			//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_TARGET_CACHING, M_ENABLE);



		}
		else {
			/* ���� �����Ǿ� �ִٸ� �ϴ� ���� */
			ReleaseMarkModel(mark_no, 1);
			if (m_mlPATID[mark_no]) {
				MpatFree(m_mlPATID[mark_no]);
				m_mlPATID[mark_no] = M_NULL;
			}
			MpatAlloc(theApp.clMilMain.m_mSysID, M_DEFAULT, M_DEFAULT, &m_mlPATID[mark_no]);

			/* Model ���� */
			MpatRestore(file, theApp.clMilMain.m_mSysID, M_DEFAULT, &m_mlPATID[mark_no]);
			if (!m_mlPATID[mark_no] || MappGetError(M_CURRENT, M_NULL))
			{
				LOG_ERROR(ENG_EDIC::en_mil, L"MIL_ERR : Failed to restore the mark image (buffer, pat)");
				return FALSE;
			}

			if (m_pstMarkModel[mark_no].iSizeP.x == 0 && m_pstMarkModel[mark_no].iSizeP.y == 0) { 
				DOUBLE tSize_x = 0, tSize_y = 0;
				MpatInquire(m_mlPATID[mark_no], M_DEFAULT, M_ALLOC_SIZE_X, &tSize_x);
				MpatInquire(m_mlPATID[mark_no], M_DEFAULT, M_ALLOC_SIZE_Y, &tSize_y);
				m_pstMarkModel[mark_no].iSizeP.x = (LONG)tSize_x;
				m_pstMarkModel[mark_no].iSizeP.y = (LONG)tSize_y;
				DOUBLE tOffset_x = 0, tOffset_y = 0;
				MpatInquire(m_mlPATID[mark_no], M_DEFAULT, M_ORIGINAL_X, &tOffset_x); 
				MpatInquire(m_mlPATID[mark_no], M_DEFAULT, M_ORIGINAL_Y, &tOffset_y);

				m_pstMarkModel[mark_no].iOffsetP.x = (LONG)tOffset_x;
				m_pstMarkModel[mark_no].iOffsetP.y = (LONG)tOffset_y;
			}

			MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_REFERENCE_X, m_pstMarkModel[mark_no].iOffsetP.x);
			MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_REFERENCE_Y, m_pstMarkModel[mark_no].iOffsetP.y);

			// lk91 �ӽ� �߰�
			MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_ACCEPTANCE, 20.0);
			//MpatSetAcceptance(m_mlPATID[mark_no], 20.0);
			MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_ACCURACY, M_MEDIUM);
			//MpatSetAccuracy(m_mlPATID[mark_no], M_MEDIUM);
			MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_SPEED, lSpeed[speed]);
			//MpatSetSpeed(m_mlPATID[mark_no], M_LOW);
			MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_CERTAINTY, 80);
			//MpatSetCertainty(m_mlPATID[mark_no], 80);
			MpatControl(m_mlPATID[mark_no], M_DEFAULT, M_NUMBER, MIL_INT(count));
			//MpatSetNumber(m_mlPATID[mark_no], 1L);
			//MpatSetSearchParameter(m_mlPATID[mark_no], M_FAST_FIND, M_DISABLE);

		}
	}
#endif
	return TRUE;
}

BOOL CMilModel::SetModelDefine_tot(UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, LPG_CMPV value,
	UINT8 mark_no, TCHAR* file, DOUBLE scale_min, DOUBLE scale_max,
	DOUBLE score_min, DOUBLE score_tgt)
{
	//UINT8 i = 0;

	///* ���� �����Ǿ� �ִٸ� �ϴ� ���� */
	//ReleaseMarkModel(mark_no);
	/* ���� ��� ���� �ʱ�ȭ */
	ResetMarkResult();
	/* ����ϰ��� �ϴ� �� ���� ���� */
	//m_u8ModelRegist = count;
	/* ��� ��� ���� ������ �ʰ��ϸ�, �ִ� ������ ���� */
	//if (m_pstConfig->mark_find.max_mark_regist < count)
	//{
	//	m_u8ModelRegist = m_pstConfig->mark_find.max_mark_regist;
	//	AfxMessageBox(L"The number of registration models was exceeded,\n"
	//		L"and replaced with the maximum value defined in the config file",
	//		MB_ICONINFORMATION);
	//}
	/* ��ȿ�� �˻� ����� �� �� ���� */
	//for (i = 0; i < m_u8ModelRegist && value[i].IsValid(); i++)
	//{
	//	memcpy(&m_pstMarkModel[i], &value[i], sizeof(STG_CMPV));
	//}
	memcpy(&m_pstMarkModel[mark_no], value, sizeof(STG_CMPV)); 

	return SetModelDefine_tot(speed, level, count, smooth, mark_no, file, scale_min, scale_max, score_min);
}

BOOL CMilModel::MergeMark(LPG_CMPV value, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, UINT8 mark_no, TCHAR* file1, TCHAR* file2,
	TCHAR* RecipeName, DOUBLE scale_min, DOUBLE scale_max, DOUBLE score_min, DOUBLE score_tgt) // file1, file2, ������ ��
{
#ifndef _NOT_USE_MIL_
	// ��ư �ϳ��� ����
	LONG lSpeed[4] = { M_LOW, M_MEDIUM, M_HIGH, M_VERY_HIGH };
	const int cntMerge = 3;
	const int cntParam = 4;
	MIL_INT64 m_type[cntMerge];
	DOUBLE modelParam[cntMerge][cntParam];
	DOUBLE dbDivSize = 1.0f;	/* Ring�� Circle�� ���, �� ũ�� ����� ��, ���������� �ؾ��ϱ� ������ */
	DOUBLE dbPixelToMM = m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1);
	MIL_DOUBLE dbPixel[cntMerge][cntParam] = { NULL };	/* um --> size */
	DOUBLE dbSizeX[cntMerge];
	DOUBLE dbSizeY[cntMerge];
	DOUBLE dbMaxSizeX = 0.0; // merge �� ��, margin �� �ֱ� ����
	DOUBLE dbMaxSizeY = 0.0; // merge �� ��, margin �� �ֱ� ����
	MIL_INT LastIndex = 0;

	CString strTmp;
	strTmp = file1;
	CString strTmp1 = RecipeName;

	int baseMargin = 20;
	int margin = 0;

	for (int i = 0; i < cntMerge; i++) {
		dbSizeX[i] = 0.0;
		dbSizeY[i] = 0.0;
		for (int j = 0; j < cntParam; j++) {
			modelParam[i][j] = 0.0;
		}
	}

	// �ӽ� �� ����.. ���� dialog �� �� ���� ����...
	// type
	m_type[0] = (MIL_INT64)ENG_MMDT::en_circle;
	m_type[1] = (MIL_INT64)ENG_MMDT::en_rectangle;
	m_type[2] = (MIL_INT64)ENG_MMDT::en_rectangle;

	// circle
	modelParam[0][0] = 970;

	// rectangle 
	modelParam[1][0] = 1650;
	modelParam[1][1] = 1650;

	// rectangle
	modelParam[2][0] = 1960;
	modelParam[2][1] = 1960;

	/* ���� �ֱٿ� ��ϵ� Mark Model ���� */
	MIL_ID mergeModelID;
	MmodAlloc(theApp.clMilMain.m_mSysID, M_GEOMETRIC, M_DEFAULT, &mergeModelID);

	if (!mergeModelID || MappGetError(M_CURRENT, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"MILL_ERR : Failed to allocate the model image (buffer)");
		return FALSE;
	}

	/* Pixel Size ���ϱ� */
	for (int i = 0; i < cntMerge; i++) {
		for (int j = 0; j < cntParam; j++) {
			dbPixel[i][j] = modelParam[i][j] / (dbPixelToMM * 1000.0f);
		}
	}

	for (int i = 0; i < cntMerge; i++) {
		switch (ENG_MMDT(m_type[i]))
		{
			/* �Ʒ� 2���� Type�� �˻� ��� �� ��, ���� (����)�� �ƴ� ������ */
		case ENG_MMDT::en_ring:
		case ENG_MMDT::en_circle:	
			dbDivSize = 2.0f;
			dbSizeX[i] = dbPixel[i][0];
			dbSizeY[i] = dbPixel[i][0];
			break;
		case ENG_MMDT::en_square:	
			dbDivSize = 1.0f;
			dbSizeX[i] = dbPixel[i][0];
			dbSizeY[i] = dbPixel[i][0];
			break;
		case ENG_MMDT::en_cross:
		case ENG_MMDT::en_diamond:
		case ENG_MMDT::en_triangle:
		case ENG_MMDT::en_rectangle:
		case ENG_MMDT::en_ellipse:	
			dbDivSize = 1.0f;
			dbSizeX[i] = dbPixel[i][0];
			dbSizeY[i] = dbPixel[i][1];
			break;
		}

		/* Define the model */
		MmodDefine(mergeModelID, m_type[i], M_ANY,
			dbPixel[i][0] / dbDivSize, dbPixel[i][1] / dbDivSize,
			dbPixel[i][2] / dbDivSize, dbPixel[i][3] / dbDivSize);

		if (dbMaxSizeX == 0 && dbMaxSizeY == 0) {
			dbMaxSizeX = dbSizeX[i];
			dbMaxSizeY = dbSizeY[i];
		}
		else {
			if (dbMaxSizeX < dbSizeX[i])
				dbMaxSizeX = dbSizeX[i];
			if (dbMaxSizeY < dbSizeY[i])
				dbMaxSizeY = dbSizeY[i];
		}


	}

	for (int i = 0; i < cntMerge; i++) {
		margin = (int)((dbMaxSizeX - (dbSizeX[i])) / 2 + baseMargin);
		MmodControl(mergeModelID, i, M_BOX_MARGIN_LEFT, margin);
		MmodControl(mergeModelID, i, M_BOX_MARGIN_RIGHT, margin);
	}
	for (int i = 0; i < cntMerge; i++) {
		margin = (int)((dbMaxSizeY - (dbSizeY[i])) / 2 + baseMargin);
		MmodControl(mergeModelID, i, M_BOX_MARGIN_BOTTOM, margin);
		MmodControl(mergeModelID, i, M_BOX_MARGIN_TOP, margin);
	}

	int cnt1 = 0;
	int cnt2 = 0;
	for (int i = cntMerge-2; i >= 0; i--) {
		cnt2 = (cnt1 * 2) + 1;
		MmodDefine(mergeModelID, M_MERGE_MODEL, i, i+cnt2, M_NULL, M_NULL, M_NULL);
		cnt1++;
	}

	LastIndex = MmodInquire(mergeModelID, M_CONTEXT, M_NUMBER_MODELS + M_TYPE_MIL_INT, M_NULL);
	while (LastIndex != 1) {
		MmodDefine(mergeModelID, M_DELETE, 0, M_DEFAULT, M_DEFAULT, M_DEFAULT, M_DEFAULT);

		LastIndex = MmodInquire(mergeModelID, M_CONTEXT, M_NUMBER_MODELS + M_TYPE_MIL_INT, M_NULL);
	}

	//int find = strTmp.ReverseFind('\\'); 
	//CString strDirPath = strTmp.Left(find);
	//CString strMergePath;
	//strMergePath.Format(_T("%s\\merge"), strDirPath);
	//CFileFind file;
	//if (!file.FindFile(strMergePath)) {
	//	CreateDirectory(strMergePath+"\\", NULL);
	//}
	//strMergePath.Format(_T("%s\\merge\\merge_result.mmf"), strDirPath);

	UINT8 u8Speed = (UINT8)m_pstConfig->mark_find.model_speed;
	DOUBLE dbSmooth = (DOUBLE)m_pstConfig->mark_find.model_smooth;

	// lk91 MmodControl �����ϰ� ����...
	/* �̹��� ������ ���� �� ���� */
	if (dbSmooth < 0.0f || dbSmooth > 100.0f)	dbSmooth = 50.0f;
	/* ���� �˻� �ӵ� ���� (�� ũ�Ⱑ �۰ų� ��Ī������ ���� ��Ȯ���� �ʿ�� �ϰų�, ���� ������ ������������ ������ ��쿡�� ����Ʈ ���� (M_MEDIUM) ���) */
	MmodControl(mergeModelID, M_CONTEXT, M_SPEED, lSpeed[u8Speed]);
	MmodControl(mergeModelID, M_CONTEXT, M_SMOOTHNESS, dbSmooth);
	MmodControl(mergeModelID, M_CONTEXT, M_ACCURACY, M_MEDIUM);
	MmodControl(mergeModelID, M_CONTEXT, M_ACTIVE_EDGELS, 100.0);
	MmodControl(mergeModelID, M_DEFAULT, M_POLARITY, M_ANY);
	MmodControl(mergeModelID, M_DEFAULT, M_ACCEPTANCE, M_DEFAULT);
	MmodControl(mergeModelID, M_DEFAULT, M_ANGLE, M_DEFAULT);
	MmodControl(mergeModelID, M_DEFAULT, M_ANGLE_DELTA_NEG, 45.0);
	MmodControl(mergeModelID, M_DEFAULT, M_ANGLE_DELTA_POS, 45.0);
	MmodControl(mergeModelID, M_DEFAULT, M_FIT_ERROR_WEIGHTING_FACTOR, 25);
	MmodControl(mergeModelID, M_CONTEXT, M_TIMEOUT, 2000);
	MmodControl(mergeModelID, M_DEFAULT, M_NUMBER, MIL_INT(count));
	MmodControl(mergeModelID, M_DEFAULT, M_SCALE_MIN_FACTOR, 0.5);
	MmodControl(mergeModelID, M_DEFAULT, M_SCALE_MAX_FACTOR, 2.0);
	//MmodControl(mergeModelID, M_DEFAULT, M_SEARCH_SCALE_RANGE, M_ENABLE);


	CString strMergePath;
	CFileFind file;
	if (!file.FindFile(strTmp)) {
		CreateDirectory(strTmp + "\\", NULL);
	}
	// ���Ŀ��� ���ϸ� �޾Ƽ� ���� �� �� �ֵ���...
	strMergePath.Format(_T("%s\\merge_result.mmf"), strTmp);
	MmodSave(strMergePath, mergeModelID, M_DEFAULT);

	MmodFree(mergeModelID);

#endif
	return TRUE;
}

BOOL CMilModel::RunModel_VisionCalib(MIL_ID graph_id, MIL_ID grab_id, UINT8 img_id, UINT8 dlg_id, UINT8 mark_no, int* roi_left, int* roi_right, int* roi_top, int* roi_bottom, int row, int col)
{
	if (!m_mlModelID[mark_no])
		return FALSE;

	dlg_id = DISP_TYPE_MARK_LIVE;

	// lk91 ProcImage() ���..
	theApp.clMilMain.ProcImage(m_camid - 1, 0);

	UINT8 u8ScaleSize = 0x00 /* equal */; /* 0x01 : ScaleUp, 0x02 : ScaleDown */
	BOOL bSucc = TRUE;
	TCHAR tzData[128] = { NULL };
	UINT32 u32GrabWidth = 0, u32GrabHeight = 0, i, j, k;
	UINT32 u32MaxCount;
	int calib_col = col; // lk91 ���߿� �� �޾ƾ���
	int calib_row = row;
	u32MaxCount = calib_col * calib_row; // lk91 �ӽ�!! row * col

	int* Point_x = (int*)malloc(sizeof(int) * (calib_row * calib_col));
	int* Point_y = (int*)malloc(sizeof(int) * (calib_row * calib_col));

	memset(Point_x, 0, sizeof(int) * (calib_row * calib_col));
	memset(Point_y, 0, sizeof(int) * (calib_row * calib_col));


	DOUBLE dbGrabCentX = 0.0f, dbGrabCentY = 0.0f/*,*/ /*dbLineDist = 0.0f,*/ /*dbModelSize*/;
	DOUBLE dbPosX, dbPosY, dbScore, dbScale, dbAngle, dbCovg, dbFitErr, dbCircleR;
	MIL_ID mlResult = M_NULL, miModResults = 0, miOperation;

	/* ����ü �� �ʱ�ȭ (!!! �ݵ�� �ʱ�ȭ !!!) */
	m_u8MarkFindGet = 0x00;
	ResetMarkResult();

	/* Get the size of grabbed image (buffer) */
	u32GrabWidth = (UINT32)MbufInquire(grab_id, M_SIZE_X, NULL);
	u32GrabHeight = (UINT32)MbufInquire(grab_id, M_SIZE_Y, NULL);
	dbGrabCentX = (DOUBLE)u32GrabWidth / 2.0f;	/* ��ǥ ���� Zero-based �̹Ƿ� */
	dbGrabCentY = (DOUBLE)u32GrabHeight / 2.0f;
	CString sTmp;
	
	theApp.clMilDisp.DrawOverlayDC(false, dlg_id, m_camid - 1);
	for (int i = 0; i < calib_col; i++)  // col
	{
		for (int j = 0; j < calib_row; j++) // row
		{
			MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_OFFSET_X, roi_left[i + (j * calib_col)]);
			MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_OFFSET_Y, roi_top[i + (j * calib_col)]);
			MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_SIZE_X, roi_right[i + (j * calib_col)] - roi_left[i + (j * calib_col)]);
			MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_SIZE_Y, roi_bottom[i + (j * calib_col)] - roi_top[i + (j * calib_col)]);
			MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_ACTIVE_EDGELS, 100.0);
			MmodPreprocess(m_mlModelID[mark_no], M_DEFAULT); // find �Ҷ� Preprocess �ϵ���...

			MmodAllocResult(theApp.clMilMain.m_mSysID, M_DEFAULT, &mlResult);
			MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_NUMBER, 1); // �� roi ���� ���� 1��

			MmodFind(m_mlModelID[mark_no], theApp.clMilMain.m_mImgProc[m_camid - 1], mlResult);

			MmodGetResult(mlResult, M_DEFAULT, M_NUMBER + M_TYPE_MIL_INT, &miModResults);

			if (miModResults < 1) 
			{
				//sTmp.Format(_T("%d_NG"), i + (j * calib_col) + 1);
				sTmp = "NG";
				theApp.clMilDisp.AddTextList(dlg_id, m_camid - 1, roi_left[i + (j * calib_col)] + 50, roi_top[i + (j * calib_col)] + 50, sTmp, eM_COLOR_RED, 7, 14, VISION_FONT_TEXT, true);
				theApp.clMilDisp.AddCrossList(dlg_id, m_camid - 1, (roi_left[i + (j * calib_col)] + roi_right[i + (j * calib_col)]) / 2,
					(roi_top[i + (j * calib_col)] + roi_bottom[i + (j * calib_col)]) / 2, 10, 10, eM_COLOR_RED);
			}
			else
			{
				MmodGetResult(mlResult, M_DEFAULT, M_POSITION_X, m_pFindPosX);
				MmodGetResult(mlResult, M_DEFAULT, M_POSITION_Y, m_pFindPosY);
				Point_x[i + (j * calib_col)] = m_pFindPosX[0];
				Point_y[i + (j * calib_col)] = m_pFindPosY[0];

				//sTmp.Format(_T("%d_OK"), i + (j * calib_col) + 1);
				//theApp.clMilDisp.DrawOverlayDC(false, dlg_id, m_camid - 1);
				//theApp.clMilDisp.AddTextList(dlg_id, m_camid - 1, roi_left[i + (j * calib_col)] + 50, roi_top[i + (j * calib_col)] + 50, sTmp, eM_COLOR_GREEN, 7, 14, VISION_FONT_TEXT, true);
				//theApp.clMilDisp.AddCrossList(dlg_id, m_camid - 1, Point_x[i + (j * calib_col)], Point_y[i + (j * calib_col)], 7, 7, eM_COLOR_YELLOW);
			}
			MmodFree(mlResult);
		}
	}

	//theApp.clMilDisp.DrawOverlayDC(false, dlg_id, m_camid - 1);
	sTmp = "���� : [mm]";
	theApp.clMilDisp.AddTextList(dlg_id, m_camid - 1, ACA1920_SIZE_X - 300, ACA1920_SIZE_Y - 100, sTmp, eM_COLOR_MAGENTA, 10, 20, VISION_FONT_TEXT, true);

	// x
	int cnt = 1;
	for (int i = 0; i < calib_row * calib_col; i++) 
	{
		if (i == (calib_col * cnt) - 1)
			cnt++;
		else{
			if ((Point_x[i + 1] == 0 && Point_y[i + 1] == 0) || (Point_x[i] == 0 && Point_y[i] == 0)) // NG ����ó��
				continue;
			sTmp.Format(_T("%.3f"), (Point_x[i + 1] - Point_x[i]) * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1));
			theApp.clMilDisp.AddTextList(dlg_id, m_camid - 1, (Point_x[i + 1] + Point_x[i]) / 2,
				Point_y[i + 1] - 50, sTmp, eM_COLOR_GREEN, 7, 14, VISION_FONT_TEXT, true);
			theApp.clMilDisp.AddLineList(dlg_id, m_camid - 1, Point_x[i], Point_y[i],
				Point_x[i + 1], Point_y[i + 1], PS_SOLID, eM_COLOR_GREEN);
		}
	}

	// y
	for (int i = 0; i < (calib_row - 1) * calib_col; i++)
	{
		if ((Point_x[i + calib_col] == 0 && Point_y[i + calib_col] == 0) || (Point_x[i] == 0 && Point_y[i] == 0)) // NG ����ó��
			continue;
		sTmp.Format(_T("%.3f"), (Point_y[i + calib_col] - Point_y[i]) * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1));
		theApp.clMilDisp.AddTextList(dlg_id, m_camid - 1, Point_x[i],
			(Point_y[i + calib_col] + Point_y[i]) / 2, sTmp, eM_COLOR_BLUE, 7, 14, VISION_FONT_TEXT, true);
		theApp.clMilDisp.AddLineList(dlg_id, m_camid - 1, Point_x[i], Point_y[i],
			Point_x[i + calib_col], Point_y[i + calib_col], PS_SOLID, eM_COLOR_BLUE);
	}
	theApp.clMilDisp.DrawOverlayDC(true, dlg_id, m_camid - 1);

	free(Point_x);
	free(Point_y);

	return TRUE;
}

/* desc : MIL �Ҵ� ���� */
//VOID CMilModel::CloseMilModel()
//{
//#ifndef _NOT_USE_MIL_
//	if (m_mlModelID) {
//		MmodFree(m_mlModelID);
//		m_mlModelID = M_NULL;
//	}
//	if (m_mlPATID) {
//		MpatFree(m_mlPATID);
//		m_mlPATID = M_NULL;
//	}
//#endif
//}


#if 0
// 3. Find Marker
Marker�� �˻��մϴ�.�˻� ��, ����� Draw �Լ��� �̿��Ͽ� width �� ���������� �ܰ��� �׸��ϴ�
MmeasFindMarker(M_DEFAULT, MilTargetImage, MilStripMarker, M_CONTRAST);
MmeasDraw(M_DEFAULT, MilStripMarker, MilOverlay, M_DRAW_WIDTH + M_DRAW_BOX, M_DEFAULT, M_RESULT);

// 4. Result
GetResult�Լ��� �̿��Ͽ� width �� ���ͼ� ����մϴ�
MmeasGetResult(MilStripMarker, M_WIDTH, &vWidth, M_NULL);
strWidth.Format("Width= %. 3f pixels", vWidth);
MgraText(M_DEFAULT, MilOverlay, 0, 0, (char*)(const char*)strWidth);
#endif