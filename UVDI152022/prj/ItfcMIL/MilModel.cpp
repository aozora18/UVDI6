/*
 desc : MIL Pattern Image (Buffer) 관리 및 처리 (Source Image : 등록된 Pattern Image)
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

#define USE_MIXED_MARK_CALC_AVERAGE	0	/* 여러 개의 검색된 마크들의 평균 값 구할지 여부 */
#define	MAX_EDGE_LINE_FIND_COUNT	25

/*
 desc : 생성자
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
	m_mlLineID = M_NULL;	/* Line Detection ID (임시) */
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
	m_i32EdgeFindGet = 0;	/* Edge Detection 검색 후 저장될 결과 */
	m_camid = cam_id;
	
	/* 계산 결과 임시로 저장하기 위한 버퍼 */
	CreateMarkResult();
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CMilModel::~CMilModel()
{
#ifndef _NOT_USE_MIL_
	/* 기존 할당된 메모리가 있으면 해제 */
	if (m_mlEdgeID)		MbufFree(m_mlEdgeID);
	m_mlEdgeID = M_NULL;
	/* Line Detection MIL ID 메모리 해제 */
	if (m_mlLineID)		MbufFree(m_mlLineID);
	m_mlLineID = NULL;
	/* Marker Strip MIL ID 메모리 해제 */
	if (m_mlStripID)	MbufFree(m_mlStripID);
	m_mlStripID = NULL;

	if (theApp.clMilMain.MilEdgeContext)	MedgeFree(theApp.clMilMain.MilEdgeContext);
	theApp.clMilMain.MilEdgeContext = M_NULL;
#endif
	for (int i = 2; i >= 0; i--) {
		/* 현재 설정된 모델 초기화 */
		for (int j = 0; j < 2; j++) {
			ReleaseMarkModel(i, j);
		}
	}
	/*  검색 및 결과 메모리 해제 */
	ReleaseMarkResult();
}

/*
 desc : 현재 설정된 모델 초기화
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
 desc : 마크 모델 새로 등록할 때마다, 모델 검색 결과를 정보를 저장할 객체 생성
 parm : None
 retn : None
*/
VOID CMilModel::CreateMarkResult()
{
	// by sys&j : MIL_INT와 INT64 같은 사이즈로 사료됨. 따라서 MIL 미사용 호환성에 따라 INT64로 변환
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
 desc : 마크 모델 검색 결과 메모리 해제
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
 desc : 결과 데이터 초기화
 parm : None
 retn : None
*/
VOID CMilModel::ResetMarkResult()
{
#if 0	/* 아래 2개 변수는 초기화 하면 안됨 (Shared Memory에서 내부적으로 처리 함) */
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
 desc : 현재 등록된 Mark Model 정보 반환
 parm : index	- [in]  등록된 마크 모델의 배열 인덱스 (Zero-based. 최대 m_pstConfig->mark_find.max_mark_regist-1)
 retn : Mark Model 값 (없다면 0 값 반환)
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
 desc : Mark 정보 설정
 parm : speed		- [in]  0 - Low, 1 - Medium, 2 - High, 3 - Very High
		level		- [in]  등록된 모델 이미지와 Grabbed Image이미지의 Edge 영역에 대한 비교 정도 값
							값이 클수록 상세히 비교하지만, 속도는 많이 느려지짐
							LOW보다 HIGH로 갈수록 더 많은 Edge를 가지고 비교하게 됨
							0 (M_MEDIUM), 1 (M_HIGH), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - 기본 값, 1.0f ~ 100.0f
							기본 값은 50이지만, 100으로 갈수록 엣지의 강도가 센것만 나타납니다.
		scale_min	- [in]  검색 대상의 크기 범위 값 설정 (최소한 이 값 비율보다 커야 됨. 범위: 0.5 ~ 1.0)
		scale_max	- [in]  검색 대상의 크기 범위 값 설정 (최대한 이 값 비율보다 작아야 됨. 범위: 1.0 ~ 1.0)
		score_min	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0))
		score_tgt	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0))
 retn : TRUE or FALSE
*/
BOOL CMilModel::SetModelDefine(UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, // lk91 SetModelDefine_tot로 변경
	DOUBLE scale_min, DOUBLE scale_max, DOUBLE score_min, DOUBLE score_tgt)	 
{
	return TRUE;
}

/*
 desc : Mark 정보 설정
 parm : speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		detail_level- [in]  등록된 모델 이미지와 Grabbed Image이미지의 Edge 영역에 대한 비교 정도 값
							값이 클수록 상세히 비교하지만, 속도는 많이 느려지짐
							LOW보다 HIGH로 갈수록 더 많은 Edge를 가지고 비교하게 됨
							0 (M_MEDIUM), 1 (M_HIGH), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - 기본 값, 1.0f ~ 100.0f
							기본 값은 50이지만, 100으로 갈수록 엣지의 강도가 센것만 나타납니다.
		value		- [in]  Model Type, Param 1 ~ 5개 정보까지 모두 포함된 구조체 포인터
		count		- [in]  등록하고자 하는 모델의 개수
		scale_min	- [in]  검색 대상의 크기 범위 값 설정 (최소한 이 값 비율보다 커야 됨. 범위: 0.5 ~ 1.0)
		scale_max	- [in]  검색 대상의 크기 범위 값 설정 (최대한 이 값 비율보다 작아야 됨. 범위: 1.0 ~ 1.0)
		score_min	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0)
		score_tgt	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0))
 retn : TRUE or FALSE
*/
BOOL CMilModel::SetModelDefine(UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, LPG_CMPV value,		// lk91 SetModelDefine_tot로 변경
	UINT8 mark_no, DOUBLE scale_min, DOUBLE scale_max,
	DOUBLE score_min, DOUBLE score_tgt)
{
//	UINT8 i = 0;
//
//	/* 기존 설정되어 있다면 일단 해제 */
//	ReleaseMarkModel();
//	/* 기존 결과 정보 초기화 */
//	ResetMarkResult();
//#if 0
//	/* 등록하고자 하는 모델의 개수 파악 (최대 등록 개수를 넘지 못하도록 하기 위함) */
//	for (i = 0; value[i].IsValid() && i < m_pstConfig->mark_find.max_mark_regist; i++)
//	{
//		m_u8ModelRegist++;
//	}
//#else
//	/* 등록하고자 하는 모델 개수 설정 */
//	m_u8ModelRegist = count;
//	/* 등록 대상 모델의 개수가 초과하면, 최대 값으로 설정 */
//	if (m_pstConfig->mark_find.max_mark_regist < count)
//	{
//		m_u8ModelRegist = m_pstConfig->mark_find.max_mark_regist;
//		AfxMessageBox(L"The number of registration models was exceeded,\n"
//			L"and replaced with the maximum value defined in the config file",
//			MB_ICONINFORMATION);
//	}
//#endif
//	/* 유효한 검색 대상의 모델 값 저장 */
//	for (i = 0; i < m_u8ModelRegist && value[i].IsValid(); i++)
//	{
//		memcpy(&m_pstMarkModel[i], &value[i], sizeof(STG_CMPV));
//	}

	return SetModelDefine(speed, level, count, smooth, scale_min, scale_max, score_min);
}

/*
 desc : Mark 정보 설정
 parm : speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		detail_level- [in]  등록된 모델 이미지와 Grabbed Image이미지의 Edge 영역에 대한 비교 정도 값
							값이 클수록 상세히 비교하지만, 속도는 많이 느려지짐
							LOW보다 HIGH로 갈수록 더 많은 Edge를 가지고 비교하게 됨
							0 (M_MEDIUM), 1 (M_HIGH), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - 기본 값, 1.0f ~ 100.0f
							기본 값은 50이지만, 100으로 갈수록 엣지의 강도가 센것만 나타납니다.
		model		- [in]  Model Type 즉, circle, square, rectangle, cross, diamond, triangle
		param		- [in]  총 5개의 Parameter Values (unit : um)
		count		- [in]  등록하고자 하는 모델의 개수
		scale_min	- [in]  검색 대상의 크기 범위 값 설정 (최소한 이 값 비율보다 커야 됨. 범위: 0.5 ~ 1.0)
		scale_max	- [in]  검색 대상의 크기 범위 값 설정 (최대한 이 값 비율보다 작아야 됨. 범위: 1.0 ~ 1.0)
		score_min	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0)
		score_tgt	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0))
 retn : TRUE or FALSE
*/
BOOL CMilModel::SetModelDefine(UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, PUINT32 model,
	DOUBLE* param1, DOUBLE* param2, DOUBLE* param3,	// lk91 param 배열 넘겨주는 부분 체크
	DOUBLE* param4, DOUBLE* param5, UINT8 mark_no,
	DOUBLE scale_min, DOUBLE scale_max,
	DOUBLE score_min, DOUBLE score_tgt)
{

	/* 기존 설정되어 있다면 일단 해제 */
	ReleaseMarkModel(mark_no, 0);
	/* 기존 결과 정보 초기화 */
	ResetMarkResult();
	///* 등록하고자 하는 모델 개수 설정 */
	//m_u8ModelRegist = count;
	/* 등록 대상 모델의 개수가 초과하면, 최대 값으로 설정 */
	//if (m_pstConfig->mark_find.max_mark_regist < count)
	//{
	//	m_u8ModelRegist = m_pstConfig->mark_find.max_mark_regist;
	//	AfxMessageBox(L"The number of registration models was exceeded,\n"
	//		L"and replaced with the maximum value defined in the config file",
	//		MB_ICONINFORMATION);
	//}

	/* 유효한 검색 대상의 모델 값 저장 */
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
 desc : Mark 정보 설정 - 이미지 데이터
 parm : speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		level		- [in]  등록된 모델 이미지와 Grabbed Image이미지의 Edge 영역에 대한 비교 정도 값
							값이 클수록 상세히 비교하지만, 속도는 많이 느려지짐
							LOW보다 HIGH로 갈수록 더 많은 Edge를 가지고 비교하게 됨
							0 (M_MEDIUM), 1 (M_HIGH), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - 기본 값, 1.0f ~ 100.0f
		scale_min	- [in]  검색 대상의 크기 범위 값 설정 (최소한 이 값 비율보다 커야 됨. 범위: 0.5 ~ 1.0)
		scale_max	- [in]  검색 대상의 크기 범위 값 설정 (최대한 이 값 비율보다 작아야 됨. 범위: 1.0 ~ 1.0)
		score_min	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0)
		name		- [in]  Model Name
		file		- [in]  모델 이미지가 저장된 파일 이름 (전체 경로 포함. Image File)
 retn : TRUE or FALSE
*/
BOOL CMilModel::SetModelDefineLoad(UINT8 speed, UINT8 level, DOUBLE smooth,	// lk91 미사용
	DOUBLE scale_min, DOUBLE scale_max,
	DOUBLE score_min, DOUBLE score_tgt,
	PTCHAR name, CStringArray& file)
{
	return TRUE;
}


/*
 desc : 해당 검색 조건에 부합되는 개수가 있는지 확인
 parm : find	- [in]  찾고자 하는 개수
		score	- [in]  매칭 비율 값
		scale	- [in]  크기 비율 값
 retn : TRUE (find 개수가 존재한다) or FALSE (find 개수보다 많거나 적은 경우)
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
 desc : 검색된 결과 중 Scale 값이 1.0에 가장 가까운 것만 반환
 parm : None
 retn : 검색된 결과가 저장된 포인터 반환
*/
LPG_GMFR CMilModel::GetFindMarkBestScale()
{
	UINT8 i;
	LPG_GMFR pstResult = NULL;

	/* 검색된 경우가 없는지 여부 */
	if (m_u8MarkFindGet < 1)	return NULL;
	/* 맨 처음 위치 값 무조건 저장 */
	pstResult = &m_pstModResult[0];
	/* 만약 검색된 결과가 1개인 경우는 바로 리턴 */
	if (1 == m_u8MarkFindGet)
	{
		/* 검색 대상의 모델 인덱스 기준으로 모델 크기 설정 */
		//m_pstModResult[0].mark_width = (UINT32)ROUNDED(m_pstModelSize[m_pstModResult[0].model_index].x, 0);
		//m_pstModResult[0].mark_height = (UINT32)ROUNDED(m_pstModelSize[m_pstModResult[0].model_index].y, 0);
		m_pstModResult[0].mark_width = (UINT32)ROUNDED(m_pstMarkModel[0].iSizeP.x, 0);
		m_pstModResult[0].mark_height = (UINT32)ROUNDED(m_pstMarkModel[0].iSizeP.y, 0);
	}
	else
	{
		/* 검색된 결과가 2개 이상인 경우, Scale 값이 1.0에 가장 가까운 값을 반환 */
		for (i = 0x00; i < m_u8MarkFindGet; i++)  
		{
			/* 검색 대상의 모델 인덱스 기준으로 모델 크기 설정 */
			//m_pstModResult[i].mark_width = (UINT32)ROUNDED(m_pstModelSize[m_pstModResult[i].model_index].x, 0);
			//m_pstModResult[i].mark_height = (UINT32)ROUNDED(m_pstModelSize[m_pstModResult[i].model_index].y, 0);
			m_pstModResult[i].mark_width = (UINT32)ROUNDED(m_pstMarkModel[0].iSizeP.x, 0);
			m_pstModResult[i].mark_height = (UINT32)ROUNDED(m_pstMarkModel[0].iSizeP.y, 0);
			/* 현재 값보다 다음 검색된 값의 Scale이 더 등록된 마크 크기에 가까운지 비교 */
			if (pstResult->scale_rate < m_pstModResult[i].scale_rate)
			{
				pstResult = &m_pstModResult[i];
			}
		}
	}

	/* 최종 많이 찾았지만, 결국 1개만 찾았다고 강제로 설정 */
	if (pstResult != &m_pstModResult[0])	memcpy(&m_pstModResult[0], pstResult, sizeof(STG_GMFR));
	m_u8MarkFindGet = 0x01;

	/* 최종 검색된 결과가 저장된 포인터 반환 */
	pstResult->valid_multi = 0x01;	/* Fixed */
	pstResult->mark_method = UINT8(m_enMarkMethod);

	return pstResult;
}

/*
 desc : 검색된 결과들 중에서 중앙 1번 마크 그리고 주변 마크 기준으로 중심 값이 계산된 결과 반환
 parm : None
 retn : 검색된 결과가 저장된 포인터 반환
*/
LPG_GMFR CMilModel::GetFindMarkCentSide()
{
	TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
	UINT8 i/*, j, k*/;
	BOOL bSucc = FALSE;
	DOUBLE dbDistCent = 0.0f;		/* 무게 중심과 직사각형의 중심 간의 거리 (단위: pixel) */
	DOUBLE dbMarkSize = 0.0f, dbSquareRate = 0.0f;
	STG_DBXY stCent = { NULL }, stDist = { NULL };
	LPG_GMFR pstMain = { NULL };
	PUINT8 pValid = NULL;
	LPG_CVMF pstMarkFind = &m_pstConfig->mark_find;

	/* 결과 값 초기화 */
	memset(&m_stModResult, 0x00, sizeof(STG_GMFR));
	/* 검색 모드 값 설정 */
	m_stModResult.mark_method = (UINT8)m_enMarkMethod;

	/* 검색된 모든 Mark 값에 대한 유효 여부 설정 */
	for (i = 0x00; i < m_u8MarkFindGet; i++)
	{
		/* 기본 값 설정 */
		m_pstModResult[i].mark_width = UINT32(ROUNDED(m_pstModelSize[m_pstModResult[i].model_index].x, 0));
		m_pstModResult[i].mark_height = UINT32(ROUNDED(m_pstModelSize[m_pstModResult[i].model_index].y, 0));
		m_pstModResult[i].mark_method = (UINT8)m_enMarkMethod;
		/* 이미 ModDefine 함수에서 Scale Rate Min ~ Max 값과 Acceptance (Score) Rate 이상 값만 검색 */
		/* 하기 위한 검색 조건을 설정 했기 때문에 아래, 필터 (Interlock) 조건은 필요 없음. 주석처리 */
#if 0
		/* 여기서 검색 조건 (Only score_rate, scale_rate)에 맞지 않는 것은 걸러 낸다 */
		/* 찾고자 하는 모델 (Mark)의 크기가 대부분 작기 때문에, scale보다 score에 비중을 둠 */
#if 1
		if (m_pstModResult[i].score_rate >= m_dbRecipeScore &&
			m_pstModResult[i].scale_rate >= m_dbRecipeScale)
#else
		if (m_pstModResult[i].score_rate >= m_dbRecipeScore)
#endif
#endif
		{
			m_pstModResult[i].valid_multi = 0x01;
			/* 중심 마크가 여러 개인 경우, 1개만 인정해야 하므로 */
			if (ENG_MMSM::en_cent_side == m_enMarkMethod &&
				m_pstModResult[i].model_index == 0x00)
			{
				if (!pstMain)	pstMain = &m_pstModResult[i];
				else
				{
					/* 기존 검색된 마크와 비교. (Scale로만 비교. Score는 배제) 진행 */
					/* 검색된 마크가 뚜렷해야 하기 때문에, scale이 가장 좋은게 우선 */
					/* 보통 2중 링 구조로 되어 있다보니, score보다 scale에 가장 가까운게 맞을듯 */
					if (pstMain->scale_rate < m_pstModResult[i].scale_rate)
					{
						pstMain->valid_multi = 0x00;		/* 기존 값은 유효하지 않다고 플래그 설정 */
						pstMain = &m_pstModResult[i];	/* 새로운 Mark 정보로 연결 */
					}
				}
			}
		}
	}
#if 0	/* 일단 마크 검색 속도 저하 때문에, 일단 소스에서 배제 처리 */
	/* 검색된 외곽 마크들의 간격이 최소한의 자신의 반지름 이상 (중심 간의 거리) 떨어져 있는지 확인 */
	if (ENG_MMSM::en_ring_circle != m_enMarkMethod)
	{
		for (i = 0x00; i < m_u8MarkFindGet; i++)
		{
			/* 값이 유효한 경우만 검색 */
			if (!m_pstModResult[i].IsValid())	continue;
			/* 마크 중심에 있는 경우는 검사하지 않음 */
			if (ENG_MMSM::en_cent_side == m_enMarkMethod &&
				m_pstModResult[i].model_index == 0x00)	continue;
			/* 중심 마크 이외의 검색된 마크들 간의 떨어진 간격 */
			for (j = i + 1; j < m_u8MarkFindGet; j++)
			{
				/* 값이 유효한 경우만 검색 */
				if (!m_pstModResult[j].IsValid())	continue;
				/* 마크 중심에 있는 경우는 검사하지 않음 */
				if (ENG_MMSM::en_cent_side == m_enMarkMethod &&
					m_pstModResult[i].model_index == 0x00)	continue;
				/* 2개의 마크의 중심 간의 떨어진 거리 (Pixel) 구하기 */
				dbDistCent = sqrt(pow(m_pstModResult[j].cent_x - m_pstModResult[i].cent_x, 2) +
					pow(m_pstModResult[j].cent_y - m_pstModResult[i].cent_y, 2));
				/* 해당 모델의 크기 */
				dbMarkSize = m_pstModResult[j].GetMarkSize() > m_pstModResult[i].GetMarkSize() ?
					m_pstModResult[j].GetMarkSize() : m_pstModResult[i].GetMarkSize();
				/* 2개의 검색된 마크의 중심 거리가 각각 반지름 크기보다 가깝다면 1개를 버린다 */
				/* 1개를 버릴 때는, 2개 중 가장 높은 점수 (score)를 남겨두고, 나머지는 버린다 */
				if (dbMarkSize / 2.0f > dbDistCent)
				{
					/* 가장 높은 점수 검색 */
					k = m_pstModResult[j].score_rate > m_pstModResult[i].score_rate ? i : j;
					m_pstModResult[k].valid_multi = 0x00;
				}
			}
		}
	}
#endif

	/* 각 마크 검색 조건에 따라, 검색된 마크가 유효한지 다시 한번 검사 진행 */
	switch (m_enMarkMethod) 
	{
	case ENG_MMSM::en_cent_side:	bSucc = CalcMarkCentSide(stCent, dbSquareRate, stDist);	break;	/* 가중치를 고려한 중심 값 반환 */
	case ENG_MMSM::en_multi_only:	bSucc = CalcMarkMultiOnly(stCent, dbSquareRate);		break;
	case ENG_MMSM::en_ring_circle:	bSucc = CalcMarkRingCircle(stCent, stDist);				break;
	}

	/* 사각형 비율 값 저장 */
	m_stModResult.square_rate = dbSquareRate;

	if (bSucc)
	{
		/* 검색된 마크 중 유효한 값 기준으로 평균 검색률 구하기 위함 */
		m_stModResult.scale_rate = DBL_MIN;
		m_stModResult.score_rate = DBL_MIN;
		m_stModResult.r_angle = DBL_MAX;
		m_stModResult.diameter = DBL_MAX;
		/* 평균 혹은 최소 값 구하기 위함 */
		for (i = 0x00/*,j=0x00*/; i < m_u8MarkFindGet; i++)
		{
#if 0	/* ModDefine 에서 이미 검사 했으므로, 여기서 할 필요 없음 */
			if (!m_pstModResult[i].IsValid())	continue;
#endif
#if (USE_MIXED_MARK_CALC_AVERAGE == 0)	/* 검색된 값 중에서 최소 or 최대 값을 추출하기 위함 */
			if (m_stModResult.scale_rate < m_pstModResult[i].scale_rate)
				m_stModResult.scale_rate = m_pstModResult[i].scale_rate;
			if (fabs(m_stModResult.score_rate) < fabs(m_pstModResult[i].score_rate))
				m_stModResult.score_rate = m_pstModResult[i].score_rate;
			if (fabs(m_stModResult.r_angle) > fabs(m_pstModResult[i].r_angle))
				m_stModResult.r_angle = m_pstModResult[i].r_angle;
			if (fabs(m_stModResult.diameter) > fabs(m_pstModResult[i].diameter))
				m_stModResult.diameter = m_pstModResult[i].diameter;
#else	/* 평균 값을 구하기 위함 */
			m_stModResult.scale_range += m_pstModResult[i].scale_range;
			m_stModResult.score_rate += m_pstModResult[i].score_rate;
			m_stModResult.r_angle += m_pstModResult[i].r_angle;
			m_stModResult.radius += m_pstModResult[i].radius;
#endif
			/*j++;*/
		}
		if (m_u8MarkFindGet/*j*/)
		{
#if (USE_MIXED_MARK_CALC_AVERAGE == 1)	/* 평균 값을 구하기 위함 */
			m_stModResult.radius /= DOUBLE(j);
			m_stModResult.scale_range /= DOUBLE(j);
			m_stModResult.score_rate /= DOUBLE(j);
			m_stModResult.r_angle /= DOUBLE(j);
#endif
			/* Mark Size (검색된 중앙의 Mark) */
			m_stModResult.mark_width = (UINT32)ROUNDED(m_pstModelSize[0].x, 0);
			m_stModResult.mark_height = (UINT32)ROUNDED(m_pstModelSize[0].y, 0);
			/* 중심 위치 값 저장  */
			m_stModResult.cent_x = stCent.x;
			m_stModResult.cent_y = stCent.y;
		}
#if 0	/* RunFindModel 에서 이미 검색 후 검색된 개수를 저장 했으므로, 여기서 할 필요 없음 */
		/* 최종 정상적으로 검색된 개수 값 설정 */
		m_u8MarkFindGet = j;
#endif
		/* 외부 마크와 내부 중심 마크 간의 각각 중심 간의 거리 오차 값 구하기 */
		if (m_enMarkMethod != ENG_MMSM::en_multi_only)
		{
			/* 중심 마크 간의 거리 값 저장 */
			m_stModResult.cent_dist_x = (UINT32)ROUNDED((stDist.x * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1)) * 1000.0f, 0);
			m_stModResult.cent_dist_y = (UINT32)ROUNDED((stDist.y * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1)) * 1000.0f, 0);
			/* 환경 파일에 설정된 최소 유효 중심 간 오차 거리 값보다 측정된 중심 간 오차 값이 크면 에러 처리 */
			/* 중심 간의 가로 (X) 내지 세로 (Y)의 길이가 주어진 유효 중심 간의 거리 값 보다 크면 에러 처리 */
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
#if 0	/* 이미 RunModelFind 함수에 검사 했으므로, 여기서 할 필요 없음 */
		/* --------------------------------------------------- */
		/* 찾고자 하는 개수보다 임의 개수만큼 적으면 에러 출력 */
		/* 최소한 2 개 까지 없어도 되지만, 그 이상 없으면 에러 */
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
	/*                        Grabbed Image의 결과가 유효한지 플래그 설정                        */
	/* ----------------------------------------------------------------------------------------- */
	if (bSucc && (ENG_MMSM::en_ring_circle != m_enMarkMethod))
	{
		/* 기본 마크와 주변 마크들의 구성된 검색 모드인 경우만 해당됨 */
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

	/* 최종 검색된 결과가 저장된 포인터 반환 */
	return bSucc ? &m_stModResult : NULL;
}

/*
 desc : 가중치 계산 알고리즘
 parm : hole	- [in]  기준 (HOLE) 중심 좌표 (X or Y)
		laser	- [in]  주변 (Laser) 중심 좌표 (X or Y)
 retn : 최종 가중치가 부여된 값 (좌표: x or y) 반환
*/
DOUBLE CMilModel::GetWeightRateToErrLen(DOUBLE hole, DOUBLE laser)
{
	return	((hole - laser) * (m_pstConfig->mark_find.mixed_weight_rate / 10000.0f));
}

/*
 desc : 검색된 복합(다점) 얼라인 마크들 간의 중심 좌표 구하기 (즉, 사각형 영역의 중심)
 parm : cent	- [out] 중심 좌표가 저장됨 (각 X/Y 최소, 최대 좌표 구하고, 그 중심점 반환)
		rate	- [out] 주변 좌표들 중 Min / Max로 사각형을 구성 했을 때, 가로/세로 비율 값
		dist	- [out] 중심 마크와 주변 (Side) 마크들의 중심 간의 떨어진 거리 반환
 retn : TRUE or FALSE
 note : 구해진 사각형의 가로/세로 비율이 환경 설정에 주어진 값 대비 유효한지 여부도 판단
*/
BOOL CMilModel::CalcMarkCentSide(STG_DBXY& cent, DOUBLE& rate, STG_DBXY& dist)
{
	UINT8 i = 0, k = 0, j = 0xff;
	DOUBLE dbArea = 0.0f;
	STG_DBXY stCircle = { NULL }, stRect = { NULL }, stCent = { NULL };
	STG_DBXY stMin = { DBL_MAX, DBL_MAX }, stMax = { DBL_MIN, DBL_MIN };

	/* 반환 값 초기화 */
	cent.x = cent.y = 0.0f;
	rate = 0.0f;

	for (; i < m_u8MarkFindGet; i++)
	{
		/* 값이 유효한 것부터 필터링하고, Model이 첫번째는 제외 */
		if (m_pstModResult[i].IsValid())
		{
			if (0 != m_pstModResult[i].model_index)
			{
				/* 검색된 Mark 개수 중 좌표가 가장 큰 값과 작은 값 저장 */
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
	/* 검색된 결과 개수가 4개 이하이거나, 중심 Mark가 유효하지 않으면 에러 처리 */
	if (k < 4 || 0xff == j /* Failed to search */)	return FALSE;
	/* 사각형의 가로 / 세로 비율 값이 환경 파일에 설정된 값에 유효한지 여부 */
	/* 사각형이 얼마나 정사각형인지 가로와 세로의 크기 비율 값 계산 후 반환 */
	if ((stMax.x - stMin.x) > (stMax.y - stMin.y))	rate = (stMax.y - stMin.y) / (stMax.x - stMin.x);
	else											rate = (stMax.x - stMin.x) / (stMax.y - stMin.y);
	rate *= 100.0f;

	/* 직사각형의 중심 좌표 구하기 */
	if (0x00 == m_pstConfig->mark_find.mixed_cent_type)
	{
		/* 직사각형의 중심 좌표 구하기 */
		stRect.x = stMin.x + (stMax.x - stMin.x) / 2.0f;
		stRect.y = stMin.y + (stMax.y - stMin.y) / 2.0f;
		/* 주어진 가중치 (비율)를 이용하여 Hole 과 Laser가 얼마나 떨어 졌는지 확인 */
		dist.x = GetWeightRateToErrLen(stCircle.x, stRect.x);	/* (Hole.x - Laser.x) * weight_rate = Hole에서 Laser쪽으로 이동한 값 */
		dist.y = GetWeightRateToErrLen(stCircle.y, stRect.y);	/* (Hole.y - Laser.y) * weight_rate = Hole에서 Laser쪽으로 이동한 값 */
	}
	/* 다각형의 무제 중심 좌표 구하기 */
	else
	{
		CalcPolyWeightCent(stCent.x, stCent.y, dbArea);
		/* 주어진 가중치 (비율)를 이용하여 Hole 과 Laser가 얼마나 떨어 졌는지 확인 */
		dist.x = GetWeightRateToErrLen(stCircle.x, stCent.x);	/* (Hole.x - Laser.x) * weight_rate = Hole에서 Laser쪽으로 이동한 값 */
		dist.y = GetWeightRateToErrLen(stCircle.y, stCent.y);	/* (Hole.y - Laser.y) * weight_rate = Hole에서 Laser쪽으로 이동한 값 */
	}

	/* ------------------------------------------------------- */
	/* 외곽 Mark와 내부 Mark 간의 중심 오차 계산 (가중치 부여) */
	/* Hole 위치에 (Hole - Laser) * Weight_Rate = 값 만큼 적용 */
	/* ------------------------------------------------------- */
	cent.x = stCircle.x - dist.x;
	cent.y = stCircle.y - dist.y;

	/* 주변 (Side) 마크들의 중심과 중심 마크의 중심 간의 거리 구하기 */
	/* 직사각형의 중심 좌표 구하기 */
	if (0x00 == m_pstConfig->mark_find.mixed_cent_type)
	{
		dist.x = (stCircle.x - stRect.x) - dist.x;
		dist.y = (stCircle.y - stRect.y) - dist.y;
	}
	/* 다각형의 무게 중심 좌표 구하기 */
	else
	{
		dist.x = (stCircle.x - stCent.x) - dist.x;
		dist.y = (stCircle.y - stCent.y) - dist.y;
	}
	return TRUE;
}

/*
 desc : 검색된 복합(다점) 얼라인 마크들 간의 무게 중심 좌표 구하기
		외곽에 링(도넛) 마크와 내부에 원 (Circle)로 구성된 마크 검색
 parm : cent	- [out] 중심 좌표가 저장됨
		dist	- [out] 중심 마크와 주변 (Side) 마크들의 중심 간의 떨어진 거리 반환
 retn : TRUE or FALSE
*/
BOOL CMilModel::CalcMarkRingCircle(STG_DBXY& cent, STG_DBXY& dist)
{
	UINT8 i = 0x00;
	STG_DBXY stIn, stOut;

	/* 반환 값 초기화 */
	cent.x = cent.y = 0.0f;

	/* 검색된 Mark가 2개 인지, 2개의 모델 인덱스 값이 같아도 안되고, 값이 모두 유효해야 함 */
	if (m_u8MarkFindGet != 2 ||
		m_pstModResult[0].model_index == m_pstModResult[1].model_index ||
		!m_pstModResult[0].IsValid() || !m_pstModResult[1].IsValid())	return FALSE;

	/* 내부 원과 외부 링의 중심 좌표 */
	for (; i < 2; i++)
	{
		/* 내부 원 정보 설정  */
		if (0 == m_pstModResult[i].model_index)
		{
			stIn.x = m_pstModResult[i].cent_x;
			stIn.y = m_pstModResult[i].cent_y;
		}
		/* 외부 원 정보 설정 */
		else
		{
			stOut.x = m_pstModResult[i].cent_x;
			stOut.y = m_pstModResult[i].cent_y;
		}
	}

	/* Circle의 중심과 Ring 마크의 중심 간의 거리 구하기 */
	/* 주어진 가중치 (비율)를 이용하여 Hole 과 Laser가 얼마나 떨어 졌는지 확인 */
	dist.x = GetWeightRateToErrLen(stIn.x, stOut.x);	/* (Hole.x - Laser.x) * weight_rate = Hole에서 Laser쪽으로 이동한 값 */
	dist.y = GetWeightRateToErrLen(stIn.y, stOut.y);	/* (Hole.y - Laser.y) * weight_rate = Hole에서 Laser쪽으로 이동한 값 */
	/* ------------------------------------------------------- */
	/* 외곽 Mark와 내부 Mark 간의 중심 오차 계산 (가중치 부여) */
	/* ------------------------------------------------------- */
	/* 외곽 Mark 기준으로 가중치를 부여하기 때문에 */
	cent.x = stIn.x - dist.x;
	cent.y = stIn.y - dist.y;

	/* 주변 (Side) 마크들의 중심과 중심 마크의 중심 간의 거리 구하기 */
	dist.x = (stIn.x - stOut.x) - dist.x;
	dist.y = (stIn.y - stOut.y) - dist.y;

	return TRUE;
}

/*
 desc : 검색된 복합(다점) 얼라인 마크들 간의 무게 중심 좌표 구하기
		주변 (2x2 or 3x3 or 4x4)에 동일한 모양 (Shape)으로 구성된 다점 (복합 아님) 얼라인 마크 검색
 parm : cent	- [out] 중심 좌표가 저장됨
		rate	- [out] 주변 좌표들 중 Min / Max로 사각형을 구성 했을 때, 가로/세로 비율 값
 retn : TRUE or FALSE
 note : 구해진 사각형의 가로/세로 비율이 환경 설정에 주어진 값 대비 유효한지 여부도 판단
*/
BOOL CMilModel::CalcMarkMultiOnly(STG_DBXY& cent, DOUBLE& rate)
{
	UINT8 i = 0, j = 0;
	DOUBLE dbArea = 0.0f;
	STG_DBXY stMin = { DBL_MAX, DBL_MAX }, stMax = { DBL_MIN, DBL_MIN };

	/* 반환 값 초기화 */
	cent.x = cent.y = 0.0f;
	rate = 0.0f;

	/* 사각형 영역의 Boundary 값 구하기 */
	for (; i < m_u8MarkFindGet; i++)
	{
		/* 현재 값이 유효한지 여부 */
		if (m_pstModResult[i].IsValid())
		{
			/* 검색된 Mark 개수 중 좌표가 가장 큰 값과 작은 값 저장 */
			if (stMin.x > m_pstModResult[i].cent_x)	stMin.x = m_pstModResult[i].cent_x;
			if (stMin.y > m_pstModResult[i].cent_y)	stMin.y = m_pstModResult[i].cent_y;
			if (stMax.x < m_pstModResult[i].cent_x)	stMax.x = m_pstModResult[i].cent_x;
			if (stMax.y < m_pstModResult[i].cent_y)	stMax.y = m_pstModResult[i].cent_y;
			j++;
		}
	}

	/* 검색된 결과 개수가 4개 이하이거나, 중심 Mark가 유효하지 않으면 에러 처리 */
	if (j < 4 || 0xff == j /* Failed to search */)
	{
		LOG_ERROR(ENG_EDIC::en_mil,
			L"The number of coordinates required for multi-mark configuration is too small");
		return FALSE;
	}
	/* 사각형의 가로 / 세로 비율 값이 환경 파일에 설정된 값에 유효한지 여부 */
	/* 사각형이 얼마나 정사각형인지 가로와 세로의 크기 비율 값 계산 후 반환 */
	if ((stMax.x - stMin.x) > (stMax.y - stMin.y))	rate = (stMax.y - stMin.y) / (stMax.x - stMin.x);
	else											rate = (stMax.x - stMin.x) / (stMax.y - stMin.y);
	rate *= 100.0f;

	/* 직사각형의 중심 좌표 구하기 */
	if (0x00 == m_pstConfig->mark_find.mixed_cent_type)
	{
		cent.x = stMin.x + (stMax.x - stMin.x) / 2.0f;
		cent.y = stMin.y + (stMax.y - stMin.y) / 2.0f;
	}
	/* 다각형의 무게 중심 좌표 구하기 */
	else if (0x01 == m_pstConfig->mark_find.mixed_cent_type)
	{
		CalcPolyWeightCent(cent.x, cent.y, dbArea);
	}
	/* 허프만 변환 (대각선 교차점?)을 통한 중심 좌표 구하기 */
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
 desc : 검색된 마크 기준으로 다각형 (선)으로 구성된 좌표들에 대한 무게 중심 좌표 구하기
 parm : cent_x	- [out] 무게 중심 좌표 값 반환
		cent_y	- [out] 무게 중심 좌표 값 반환
		area	- [out] 다각형의 면적 값 반환
 retn : None
*/
VOID CMilModel::CalcPolyWeightCent(DOUBLE& cent_x, DOUBLE& cent_y, DOUBLE& area)
{
	UINT8 i, j;
	DOUBLE /*dbX1, dbY1, dbX2, dbY2,*/* dbArrX, * dbArrY;

	/* 임시 버퍼 할당 */
	dbArrX = (DOUBLE*)::Alloc(sizeof(DOUBLE) * m_u8MarkFindGet);
	dbArrY = (DOUBLE*)::Alloc(sizeof(DOUBLE) * m_u8MarkFindGet);
	ASSERT(dbArrX && dbArrY);
	for (i = 0; i < m_u8MarkFindGet; i++)
	{
		dbArrX[i] = m_pstModResult[i].cent_x;
		dbArrY[i] = m_pstModResult[i].cent_y;
	}
	/* 정렬 작업 수행 */
	uvCalc_GetSortCoordPoints(dbArrX, dbArrY, UINT32(m_u8MarkFindGet));

	/* 반환 값 반드시 초기화 */
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
		/* 현재 값이 유효한지 여부 */
		if (m_pstModResult[i].IsValid())
#endif
		{
			area += dbArrX[i] * dbArrY[j];
			area -= dbArrY[i] * dbArrX[j];

			cent_x += ((dbArrX[i] + dbArrX[j]) * ((dbArrX[i] * dbArrY[j]) - (dbArrX[j] * dbArrY[i])));
			cent_y += ((dbArrY[i] + dbArrY[j]) * ((dbArrX[i] * dbArrY[j]) - (dbArrX[j] * dbArrY[i])));
		}
	}

	/* 메모리 해제 */
	::Free(dbArrX);
	::Free(dbArrY);

	/* 면적 구하기 */
	area /= 2.0f;
	/* 무게 중심 좌표 구하기 */
	cent_x = cent_x / (6.0f * area);	/* 왜 6.0을 곱하는지 ?*/
	cent_y = cent_y / (6.0f * area);	/* 왜 6.0을 곱하는지 ?*/
	/* 면적은 무조건 양수 값이므로 ... */
	area = fabs(area);
}

/*
 desc : 검색된 결과 반환
 parm : None
 retn : 검색된 결과가 저장된 포인터 반환
*/
LPG_GMFR CMilModel::GetFindMark()
{
	switch (m_enMarkMethod)
	{
	case ENG_MMSM::en_single:	/* 검색된 마크 중 가장 결과가 좋은 Mark을 검색하여 결과 반환 */
		return GetFindMarkBestScale();
	case ENG_MMSM::en_cent_side:	/* 중심의 Mark 1개와 주변의 마크 여러 개 간의 가중치를 부여하여 계산된 결과를 반환 */
	case ENG_MMSM::en_ring_circle:	/* 외곽에 링(도넛) 마크와 내부에 원 (Circle)로 구성된 마크 검색하는 방법 */
	case ENG_MMSM::en_multi_only:	/* 중심에는 Mark가 없고 주변의 마크 여러 개 간의 무게 중심이 계산된 결과를 반환 */
		return GetFindMarkCentSide();
	}

	return NULL;
}

/*
 desc : 검색하기 위해 입력된 각도와 실제 검색된 각도의 절대 오차 값 계산
 parm : find	- [in]  검색된 각도 값
		input	- [in]  검색하기 위해 입력된 각도 값
 retn : 오차 값
*/
DOUBLE CMilModel::CalcAngleDist(DOUBLE find, DOUBLE input)
{
	DOUBLE dbDist = fabs(find - input);

	while (dbDist >= 360.0f)	dbDist -= 360.0f;
	if (dbDist > 180.0f)		dbDist = 360.0f - dbDist;

	return dbDist;
}

/*
 desc : 두 개의 좌표 간의 직선의 거리 구하기
 parm : x1,y1	- [in]  첫 번째 점의 좌표 위치
		x2,y2	- [in]  두 번째 점의 좌표 위치
 retn : 직선의 거리 반환
*/
DOUBLE CMilModel::CalcLineDist(DOUBLE x1, DOUBLE y1, DOUBLE x2, DOUBLE y2)
{
	return sqrt(pow(x2 - x1, 2.0f) + pow(y2 - y1, 2.0f));
}

#ifndef _NOT_USE_MIL_
/*
 desc : Edge Detection 수행
 parm : grab_id	- [in]  Grabbed Image 정보가 저장된 ID (Target Image (Buffer))
		width	- [in]  Grabbed Image - Width (Pixel)
		height	- [in]  Grabbed Image - Width (Pixel)
		saved	- [in]  분석된 edge image 저장 여부
 retn : TRUE or FALSE
*/
BOOL CMilModel::RunEdgeDetect(MIL_ID grab_id, UINT32 width, UINT32 height, UINT8 saved)
{
	BOOL bFinded = FALSE;
	INT32 i, u32Find = 0;	/* 정상적으로 찾은 개수 */
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
	/* 검색 조건 정보 얻기 */
	LPG_CMED pstEdge = &m_pstConfig->edge_find;

	/* 기존 결과 값 초기화 */
	m_i32EdgeFindGet = 0x00;
	/* Allocate a Edge Finder context. */
	mlEdgeContext = MedgeAlloc(theApp.clMilMain.m_mSysID, M_CONTOUR, M_DEFAULT, NULL);
	/* Allocate a result buffer. */
	mlEdgeResult = MedgeAllocResult(theApp.clMilMain.m_mSysID, M_DEFAULT, NULL);
	/* ----------------------------------------------------------------------------------------- */
	/*                                       Set Flag                                            */
	/* ----------------------------------------------------------------------------------------- */
	MedgeControl(mlEdgeContext, M_ACCURACY, pstEdge->m_accuracy);	/*3 (Default:High), 4 (Very High)*/
//	MedgeControl(mlEdgeContext,	M_CHAIN_ALL_NEIGHBORS,				M_ENABLE);				/* 모든 이웃 Point들 연결하여 Edge 구성 */
	MedgeControl(mlEdgeContext, M_MOMENT_ELONGATION, M_ENABLE);
	MedgeControl(mlEdgeContext, M_FERET_MEAN_DIAMETER + M_SORT1_UP, M_ENABLE);
	MedgeControl(mlEdgeContext, M_BOX_X_MAX, M_ENABLE);	/* 내림 차순 정렬 */
	MedgeControl(mlEdgeContext, M_BOX_X_MIN, M_ENABLE);	/* 내림 차순 정렬 */
	MedgeControl(mlEdgeContext, M_BOX_Y_MAX, M_ENABLE);	/* 내림 차순 정렬 */
	MedgeControl(mlEdgeContext, M_BOX_Y_MIN, M_ENABLE);	/* 내림 차순 정렬 */
	MedgeControl(mlEdgeContext, M_CIRCLE_FIT_CENTER_X, M_ENABLE);	/* 잘 잡힌 원의 중심 점 */
	MedgeControl(mlEdgeContext, M_CIRCLE_FIT_CENTER_Y, M_ENABLE);	/* 잘 잡힌 원의 중심 점 */
	MedgeControl(mlEdgeContext, M_CIRCLE_FIT_RADIUS, M_ENABLE);
	MedgeControl(mlEdgeContext, M_AVERAGE_STRENGTH, M_ENABLE);
	MedgeControl(mlEdgeContext, M_LENGTH, M_ENABLE);	/* 엣지 측정된 선분의 총 길이 */
	MedgeControl(mlEdgeContext, M_FILTER_SMOOTHNESS, DOUBLE(pstEdge->filter_smoothness));	/* 0 ~ 100으로 지정하며, 100에 가까울 수록 엣지 강도가 센 것만 추출할 수 있음 */
	/* ----------------------------------------------------------------------------------------- */
	/*                                       Set Value                                           */
	/* ----------------------------------------------------------------------------------------- */
	MedgeControl(mlEdgeContext, M_THRESHOLD_MODE, miThreshold[pstEdge->threshold - 1]);
	/* SHEN		: 기본 필터로서, 전반적으로 이상적이며, 매우 좋은 위치의 Edge를 추출하는데 효과적이지만, */
	/*			  때때로 노이즈에 대해 예기치 않게 민감하거나 비정상적으로 두꺼운 Crest를 추출하는데 부적*/
	/*			  절한 결과를 얻을 수 있다.                                                              */
	/* DERICHE	: Shen보다 Edge의 이웃에 더 중점을 두고 있기 때문에, 이런 문제에 대해 Deriche를 사용해야 */
	MedgeControl(mlEdgeContext, M_FILTER_TYPE, miFilterType[pstEdge->filter_type - 1]);
	/* Sets whether to force all the processing of edge extraction operations to be perfrometd using floating-point calculations */
	MedgeControl(mlEdgeContext, M_FLOAT_MODE, miEnable[pstEdge->use_float_mode]);
	/* 엣지 검출 후 끊어진 두 점 간의 가장 인접한 값(0에 가까울 수록) 으로 연결선을 구성 */
	/* 가장 곡선에 근접한 엣지 (100에 가까울 수록) 간에 연결선을 구성할지 설정 값을 구성 */
	if (pstEdge->fill_gap_continuity > 0.0f)	MedgeControl(mlEdgeContext, M_FILL_GAP_CONTINUITY, pstEdge->fill_gap_continuity);
	/* The default value is 1.0f */
	if (pstEdge->extraction_scale > 0.0f)		MedgeControl(mlEdgeContext, M_EXTRACTION_SCALE, pstEdge->extraction_scale);
	/* 환경 설정의 옵션 값에 따라 적용 여부 결정 */
	if (pstEdge->use_float_mode)				MedgeControl(mlEdgeContext, M_FLOAT_MODE, M_ENABLE);
	if (pstEdge->use_closed_line)				MedgeControl(mlEdgeContext, M_CLOSURE, M_ENABLE);

	/* Calculate edges and features. */
	MedgeCalculate(mlEdgeContext, grab_id, M_NULL, M_NULL, M_NULL, mlEdgeResult, M_DEFAULT);
	/* Get the number of edges found. */
	MedgeGetResult(mlEdgeResult, M_DEFAULT, M_NUMBER_OF_CHAINS + M_TYPE_MIL_INT, &miEdgeResults, M_NULL);
	/* 검색된 항목이 없으면 에러 리턴 */
	if (miEdgeResults < 1)
	{
		/* Free MIL objects. */
		MedgeFree(mlEdgeResult);
		MedgeFree(mlEdgeContext);
		return FALSE;
	}
	/* Exclude elongated edges. moment_elongation 값 보다 작은 Edge들 걸러내기 즉, */
	/* 시작 점과 끝 점이 서로 연결되는 강도가 주어진 값 (Max 1.0)보다 작으면 배제. */
	if (pstEdge->moment_elongation > 0.0f)	MedgeSelect(mlEdgeResult, M_DELETE, M_MOMENT_ELONGATION, M_LESS, pstEdge->moment_elongation, M_NULL);
	/* Line이 끊이지 않는 것만 추출 */
	if (pstEdge->use_closed_line)			MedgeSelect(mlEdgeResult, M_EXCLUDE, M_CLOSURE, M_NOT_EQUAL, M_TRUE, M_NULL);
#if 0
	/* 원의 둘레 = 반지름 * 2.0 * 3.14 */
	/* 검색된 객체 중 지름의 크기가 "pstEdge->min_dia_size (mm)" 이하인 것을 모두 버림 */
	if (pstEdge->min_dia_size > 0)
	{
		dbMmToPixel = (pstEdge->min_dia_size / 2.0f) / m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1);
		MedgeSelect(mlEdgeResult, M_EXCLUDE, M_CIRCLE_FIT_RADIUS, M_LESS, dbMmToPixel, M_NULL);
	}
	/* 검색된 객체 중 지름의 크기가 "pstEdge->max_dia_size (mm)" 이상인 것을 모두 버림 */
	if (pstEdge->max_dia_size > 0)
	{
		dbMmToPixel = (pstEdge->max_dia_size / 2.0f) / m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1);
		MedgeSelect(mlEdgeResult, M_EXCLUDE, M_CIRCLE_FIT_RADIUS, M_GREATER, dbMmToPixel, M_NULL);
	}
#endif
	/* Exclude inner chains. (포함된 엣지들 중 내부 박스에 있는 엣지들을 제거 */
	if (!pstEdge->inc_internal_edge)		MedgeSelect(mlEdgeResult, M_EXCLUDE, M_INCLUDED_EDGES, M_INSIDE_BOX, M_NULL, M_NULL);
	/* Get the number of edges found. */
	MedgeGetResult(mlEdgeResult, M_DEFAULT, M_NUMBER_OF_CHAINS + M_TYPE_MIL_INT, &miEdgeResults, M_NULL);
	/* 검색된 항목이 없으면 에러 리턴 */
	if (miEdgeResults < 1)
	{
		/* Free MIL objects. */
		MedgeFree(mlEdgeResult);
		MedgeFree(mlEdgeContext);
		return FALSE;
	}
	/* 검색된 결과가 255개 초과인 경우, 255개만 처리하도록 설정 */
	if (miEdgeResults > UINT16_MAX)	miEdgeResults = UINT16_MAX;
	/* 기존 할당된 거 제거 */
	ResetEdgeObject();
	/* Edge Drawing */
	m_mlEdgeID = MbufAlloc2d(theApp.clMilMain.m_mSysID, width, height, 8 + M_UNSIGNED, M_IMAGE + M_PROC, M_NULL);
	/* 반드시 초기화 해줘야 됨 (그 이전에 theApp.clMilMain.m_mSysID에서 속성 정보가 저장된 상태이므로)  */
	/* 초기화 해주지 않으면, Edge Detection 이미지가 이전 이미지와 중첩되어 나타나게 됨) */
	MbufClear(m_mlEdgeID, M_COLOR_BLACK);
	/* 원본 이미지 복사 */
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
			/* Grabbed Image 이미지 저장 수행*/
			TCHAR tzGrabFile[MAX_PATH_LEN] = { NULL };
			/* 현재 파일 생성 시간 설정 */
			GetLocalTime(&stTm);
			/* 임시 저장 후, 문제있는 마크 파일은 별도로 파일 명이 변경 됨 */
			swprintf_s(tzGrabFile, L"%s\\save_img\\edge\\edge_%04d%02d%02d_%02d%02d%02d.bmp",
				g_tzWorkDir, stTm.wYear, stTm.wMonth, stTm.wDay,
				stTm.wHour, stTm.wMinute, stTm.wSecond);
			MbufExport(tzGrabFile, M_BMP, m_mlEdgeID);
		}
	}
	/* Grabbed Image에 출력된 결과 데이터 이미지를 공유 메모리 영역에 임시 저장 */
	MbufGet2d(m_mlEdgeID, 0, 0, width, height, m_pGrabEdgeImage);

	/* 검색된 결과 값을 저장할 버퍼 할당 */
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
		/* 음수 값이면, 짤린 부분임 */
		if (mdEdgeCircleX[i] - mdEdgeRadius[i] >= 0 && mdEdgeCircleY[i] - mdEdgeRadius[i] >= 0)
		{
#if 0
			/* 설정된 최대 크기보다 큰 경우인지 */
			if (pstEdge->arc_max_size > 0.0f &&
				mdEdgeDiameter[i] * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1) * 1000.0f > pstEdge->arc_max_size)	continue;
			/* 설정된 최소 크기보다 작은 경우인지 */
			if (pstEdge->arc_min_size > 0.0f &&
				mdEdgeDiameter[i] * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1) * 1000.0f < pstEdge->arc_min_size)	continue;
#endif
			/* ---------------------------------------------------------------------------------- */
			/* Edge 영역 최대 / 최소 값 설정 */
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].circle_area.left = (INT32)ROUNDUP(mdEdgeCircleX[i] - mdEdgeRadius[i], 0);// - pstEdgeDetect->area_spare_left;	/* spare pixel */
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].circle_area.top = (INT32)ROUNDUP(mdEdgeCircleY[i] - mdEdgeRadius[i], 0);// - pstEdgeDetect->area_spare_top;		/* spare pixel */
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].circle_area.right = (INT32)ROUNDUP(mdEdgeCircleX[i] + mdEdgeRadius[i], 0);// + pstEdgeDetect->area_spare_right;	/* spare pixel */
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].circle_area.bottom = (INT32)ROUNDUP(mdEdgeCircleY[i] + mdEdgeRadius[i], 0);// + pstEdgeDetect->area_spare_bottom;	/* spare pixel */
			/* Box 영역 최대 / 최소 값 설정 */
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].box_area.left = (INT32)ROUNDUP(mdEdgeBoxMinX[i], 0);
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].box_area.right = (INT32)ROUNDUP(mdEdgeBoxMaxX[i], 0);
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].box_area.top = (INT32)ROUNDUP(mdEdgeBoxMinY[i], 0);
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].box_area.bottom = (INT32)ROUNDUP(mdEdgeBoxMaxY[i], 0);
			/* ---------------------------------------------------------------------------------- */
			/* 엣지들의 강도 (밀도?) 평균 값 */
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].strength = mdEdgeStrength[i];
			/* 엣지들이 연결된 선분들의 총 길이 */
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].length = mdEdgeLength[i];
			/* 직경 혹은 영역의 넓이/높이 크기 구하기 (단위: um) */
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].dia_meter_px = mdEdgeDiameter[i];
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].dia_meter_um = mdEdgeDiameter[i] * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1) * 1000.0f;
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].width_um = m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].GetBoxWidth() * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1) * 1000.0f;
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].height_um = m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].GetBoxHeight() * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1) * 1000.0f;
			/* Detecting 정보가 존재한다고 플래그 설정 */
			m_pstShMemVisi->edge_result[m_u8ACamID - 1][i].set_data = 0x01;
#if 0
			/* 찾으려는 개수가 초과되는 경우, 더 이상 찾지 않고 빠져 나감 */
			if (++u32Find >= pstEdge->max_mark_find)	break;
#endif
		}
	}

	/* 검색 조건에 맞게 찾은 개수 저장 */
	m_i32EdgeFindGet = INT32(miEdgeResults);
	/* 일정 개수 이상 찾으면 플래그 성공으로 설정 */
	if (m_i32EdgeFindGet > 0)	bFinded = TRUE;

	/* 결과 값 메모리 해제 */
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
 desc : 이미지의 가로 혹은 세로의 경계선 중심 위치 얻기
 parm : grab_id		- [in]  Grabbed Image 정보가 저장된 ID (Target Image (Buffer))
		mil_result	- [in]  MIL 내부 검색 결과 대화 상자 출력 여부
 retn : TRUE or FALSE
*/
BOOL CMilModel::RunLineCentXY(MIL_ID grab_id, BOOL mil_result)
{
	BOOL bFinded = FALSE;

	INT32 i32ScanWidth, i32ScanHeight, i32CentX, i32CentY, i32OffX, i32OffY;
	MIL_INT mlSizeX, mlSizeY;
	MIL_ID mlContextID, mlMarkerId, mlColorID;
	LPG_CLED pstLine = &m_pstConfig->line_find;

	/* 기존 Line Edge Detection 이미지의 임시 버퍼 메모리 해제 */
	if (m_mlLineID)
	{
		MbufFree(m_mlLineID);
		m_mlLineID = M_NULL;
	}

	/* 현재 Grabbed Image의 정보 얻기 */
	mlSizeX = (INT32)MbufInquire(grab_id, M_SIZE_X, M_NULL);
	mlSizeY = (INT32)MbufInquire(grab_id, M_SIZE_Y, M_NULL);

	/* 이미지 검사 영역 정보 */
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

	/* Edge Line Detection 검색 조건 (파라미터) 값 설정 */
	SetLineMarkerSetup(mlMarkerId, grab_id, i32ScanWidth, i32ScanHeight, i32CentX, i32CentY);
	/* !!! Find the marker and compute all applicable measurements !!! */
	MmeasFindMarker(mlContextID, grab_id, mlMarkerId, M_DEFAULT);

	/* 임시 RGB 버퍼 할당 */
	mlColorID = MbufAllocColor(theApp.clMilMain.m_mSysID, 3,	/* 1: Monochrome, 3: RGB */
		mlSizeX, mlSizeY, 8 + M_UNSIGNED, M_IMAGE + M_PROC, M_NULL);
	if (mlColorID)
	{
		MbufCopy(grab_id, mlColorID);
		/* 검색 결과 정보 이미지 영역에 출력 (Linex) */
		DrawLineMarkerResult(mlMarkerId, mlColorID);
		/* 결과 정보 반환  */
		bFinded = GetMeasLineResult(mlMarkerId, mlColorID, i32CentX, i32CentY);
		/* MIL 내부 측정 결과 출력 여부 */
		if (bFinded && mil_result)	MmeasGetResult(mlMarkerId, M_INTERACTIVE, NULL, NULL);
		/* 정상적으로 검색된 영역만 이미지 버퍼에 저장 */
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
	/* 기존 Marker Strip 이미지의 임시 버퍼 메모리 해제 */
	if (m_mlStripID)	MbufFree(m_mlStripID);

	/* Get the size of grabbed image */
	mlSizeX = (INT32)MbufInquire(grab_id, M_SIZE_X, M_NULL);
	mlSizeY = (INT32)MbufInquire(grab_id, M_SIZE_Y, M_NULL);

	/* Allocate a measurement marker (Marker에 대한 할당을 합니다. 두 번째 인자가 M_STRIPE 로 되어 있다는거 확인하세요)
	/* stripe 외에도 edge, point 등으로 설정 가능합니다 */
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
 desc : Align Camera에 의해 검색된 마크의 중심 위치를 다시 카메라의 회전 기준 값을 적용해서
		최종 검색된 마크의 X, Y 중심 값 반환
 parm : cent_x	- [in]  Grabbed Image의 중심 위치
		cent_y	- [in]  Grabbed Image의 중심 위치
		find_x	- [out] 회전 이전에 검색된 중심 위치를 입력 받아 회전 이후 중심 위치 값 반환
		find_y	- [out] 회전 이전에 검색된 중심 위치를 입력 받아 회전 이후 중심 위치 값 반환
 retn : None
*/
VOID CMilModel::GetFindRotatePosXY(DOUBLE cent_x, DOUBLE cent_y, DOUBLE& find_x, DOUBLE& find_y)
{
	DOUBLE dbRadian = 0.0f, dbFindX = find_x, dbFindY = find_y;
	DOUBLE dbAngle = m_pstConfig->acam_spec.spec_angle[m_u8ACamID - 1];	/* Degree */
	/* 회전 값 (Degree)이 있는 경우인지만 처리 */
	if (dbAngle != 0.0f)
	{
		dbRadian = -uvCmn_GetDeg2Rad(dbAngle);	/* 앞의 마이너스는 반대로 회전된 값을 구하기 위함 */
		/* 중심 위치 기준으로 각도만큼 회전 했을 경우, 각 X, Y 좌표 값 계산 */
		dbFindX = (find_x - cent_x) * cos(dbRadian) - (find_y - cent_y) * sin(dbRadian) + cent_x;
		dbFindY = (find_x - cent_x) * sin(dbRadian) + (find_y - cent_y) * cos(dbRadian) + cent_y;
	}
	/* 회전 혹은 회전 없이 값 갱신 */
	find_x = dbFindX;
	find_y = dbFindY;
}

/*
 desc : 기존 검색된 Edge Object 리셋
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
 desc : 현재 등록된 Mark Model 크기 반환
 parm : index	- [in]  요청하고자 하는 모델의 위치 (Zero-based)
		flag	- [in]  0x00 : 가로  크기, 0x01 : 세로 크기
		unit	- [in]  0x00 : um, 0x01 : pixel
		size	- [in]  0x00 : radius (반지름), 0x01 : diameter (지름)
 retn : 크기 반환 (단위: um or pixel)
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
#if 0	/* 무조건 모델 크기는 지름으로 계산하므로 */
			switch (m_pstMarkModel[index].type)
			{
			case ENG_MMDT::en_circle:
			case ENG_MMDT::en_square:	dbSize *= 2.0f;	break; /* 길이는 2배로 설정해 줘야 됨 */
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
 desc : Align Mark 검색 방식 설정
 parm : method		- [in]  0x00 : 최적의 1개 반환, 0x01 : 다점 마크 기준으로 가중치를 부여하여 결과 반환
		count		- [in]  'method' 값이 2 or 3인 경우, 최종 검색될 Mark 개수 값 2 이상 값이어야 됨
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
 desc : Edge Line Detection 검색 조건 (파라미터) 값 설정
 parm : mark_id		- [in]  Marker ID
		grab_id		- [in]  이미지 저장 버퍼 ID (Drawing Object ID)
		scan_width	- [in]  이미지 검색 영역 크기 (단위: pixel) (이미지 크기 아님)
		scan_height	- [in]  이미지 검색 영역 크기 (단위: pixel) (이미지 크기 아님)
		cent_x/y	- [in]  이미지 검색 영역 중심 좌표 (단위: pixel)
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

	/* 박스 영역의 크기 즉, 이미지 검삭 영역의 크기 */
	MmeasSetMarker(mark_id, M_BOX_SIZE, scan_width, scan_height);
	/* 박스 영역의 중심 즉, 카메라 Grabbed Area의 중심 좌표 (unit: Pixel) */
	MmeasSetMarker(mark_id, M_BOX_CENTER, cent_x, cent_y);
	/* 검사 영역 (BOX_SIZE) 내에 조건에 맞는 모든 Edge Line Detection */
	MmeasSetMarker(mark_id, M_NUMBER, M_ALL, M_NULL);
	MmeasSetMarker(mark_id, M_NUMBER_MIN, M_DEFAULT, M_NULL);
	/* Filter Type : EULER (0) / PREWITT (1) / SHEN (2) */
	MmeasSetMarker(mark_id, M_FILTER_TYPE, dbFilterType[pstLine->filter_type], M_NULL);
	if (pstLine->filter_type == 0x02 && pstLine->filter_smoothness > 0)	/* in case 'SHEN' */
	{
		MmeasSetMarker(mark_id, M_FILTER_SMOOTHNESS, DOUBLE(pstLine->filter_smoothness), M_NULL);
	}
	/* 값이 작아질 수록 엣지 개수가 증가하여, 처리 속도는 느려지지만, 결과 (품질)은 좋아짐 */
	if (pstLine->edge_threshold)
	{
		//MmeasSetMarker(mark_id, M_EDGE_THRESHOLD, DOUBLE(pstLine->edge_threshold), M_NULL);
		MmeasSetMarker(mark_id, M_EDGEVALUE_VAR_MIN, DOUBLE(pstLine->edge_threshold), M_NULL); 
	}
	/* 2번째는 stripe markers를 위함. 무조건 M_NULL */
	MmeasSetMarker(mark_id, M_POLARITY, dbPolarity[pstLine->polarity], M_OPPOSITE);
	/* 아래 2개는 이미지의 성격에 따라 값을 다르게 줘야 됨 */
	MmeasSetMarker(mark_id, M_SUB_REGIONS_OFFSET, DOUBLE(pstLine->sub_regions_offset), M_NULL);			/* Sub Region Offset 값 (-100 % ~ +100 %) */
	MmeasSetMarker(mark_id, M_SUB_REGIONS_SIZE, DOUBLE(pstLine->sub_regions_size), M_NULL);				/* Sub Region 크기 (0 ~ 100 %) */
	MmeasSetMarker(mark_id, M_SUB_REGIONS_NUMBER, DOUBLE(pstLine->sub_regions_number), M_NULL);			/* 검색 영역을 나눌 섹션 수 (1 or Later) */
	/* 검사할 마커의 방향이 수평인지 수직인지 설정 */
	MmeasSetMarker(mark_id, M_ORIENTATION, dbOrientation[pstLine->orientation], M_NULL);
	/* 마커의 위치를 찾을 때, 단계별로 회전하는 각도의 값 (0.1 ~ 180.0) */
	MmeasSetMarker(mark_id, M_BOX_ANGLE_MODE, dbEnable[pstLine->box_angle_mode], M_NULL);
	/* 각도 활성황 여부에 따라 */
	if (pstLine->box_angle_mode)
	{
		MmeasSetMarker(mark_id, M_BOX_ANGLE, DOUBLE(pstLine->box_angle), M_NULL);						/* 0 값일 때 */
		/* 회전 각도의 검사 이동 간격 크기 값 */
		if (pstLine->box_angle_accuracy == 0.0f)
			MmeasSetMarker(mark_id, M_BOX_ANGLE_ACCURACY, M_DISABLE, M_NULL);
		else
			MmeasSetMarker(mark_id, M_BOX_ANGLE_ACCURACY, pstLine->box_angle_accuracy, M_NULL);			/* 공차 내에서 세부적으로 검색할 각도를 몇으로 할 것인지 */
		/* 아래 2개의 변수를 이용하여 결국 (BOX_ANGLE - BOX_ANGLE_DELTA_NEG) ~ (BOX_ANGLE + BOX_ANGLE_DELTA_POS) 까지 검색 수행	*/
		MmeasSetMarker(mark_id, M_BOX_ANGLE_DELTA_POS, DOUBLE(pstLine->box_angle_delta_neg), M_NULL);	/* 검색 각도의 범위는 몇 으로 할 것인지 */
		MmeasSetMarker(mark_id, M_BOX_ANGLE_DELTA_NEG, DOUBLE(pstLine->box_angle_delta_pos), M_NULL);	/* 검색 각도의 범위는 몇 으로 할 것인지 */
	}
	/* Enable search region clipping : 자동으로 영역을 Clipping */
	MmeasSetMarker(mark_id, M_SEARCH_REGION_CLIPPING, dbEnable[pstLine->search_region_clip], M_NULL);
}

/*
 desc : Marker 검색 결과 정보를 이미지 영역에 출력하기
 parm : mark_id	- [in]  Marker ID
		grab_id	- [in]  이미지 저장 버퍼 ID (Drawing Object ID)
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
 desc : 이미지의 경계선 중심 위치 얻기
 parm : mark_id		- [in]  Marker ID
		grab_id		- [in]  이미지 저장 버퍼 ID (Drawing Object ID)
		cent_x/y	- [in]  이미지의 중심 위치 (단위: pixel)
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

	/* 텍스트 색상 설정  */
	MgraColor(M_DEFAULT, M_COLOR_RED);
	/* Get the specified type of result(s) for all points, edges, stripes, or circles */
	/* from a measurement marker or result buffer */
	MmeasGetResult(mark_id, M_NUMBER + M_TYPE_LONG, &lFindCount, NULL);		/* 검색된 Marker Counts */
	for (l = 0; l < lFindCount; l++)
	{
		memset(pstValue, 0x00, sizeof(STG_ELFR));

		/* 검색된 엣지 라인의 각 파라미터에 대한 결과 값 얻기 */
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
		/* 맨 마지막 검출된 항목에는 SPACING 값 구할수 없음 (에러 발생) */
		if (!(l + 1 == lFindCount))
		{
			MmeasGetResultSingle(mark_id, M_SPACING, &dbValue[0], M_NULL, l);
			pstValue->spacing_um = dbValue[0] * dbPixelToUm;
		}
		/* ------------------------------------------------------------------- */
		/* 이미지의 중심 (Position X)에서 부터 인식된 Edge까지 거리 (단위: um) */
		/* ------------------------------------------------------------------- */
		pstValue->cent_diff_um = (pstValue->position[0] / 1000.0f - cent_x) * dbPixelToUm;

		/* 결과 데이터 출력 위치 계산 */
		lPos1 = ((l / 2) * 50) * ((l % 2 == 0) ? 1 : -1);
		lPos2 = ((l % 2 == 0) ? 20 : -20);
		/* 검색 결과 반환 */
		swprintf_s(tzPosXY, 128, L"C:%.1f | A:%+.2f | S:%.2f",
			pstValue->contrast / 10.0f, pstValue->angle / 100.0f, pstValue->strength / 100.0f);
		MgraText(M_DEFAULT, grab_id,
			(pstValue->position[0] / 1000.0f) + 5, pstValue->position[1] / 1000.0f + lPos1 + lPos2, tzPosXY);
	}

	/* 성공이던 실패던, 해당 검출작업 수행한 이미지 저장 */
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

	/* 마커의 측정 방향을 설정 합니다 */
	MmeasSetMarker(strip_id, M_ORIENTATION, mlOrientation, M_NULL);
	/* 마커의 극성을 지정 (좌측은 positive, 우측은 negative일 경우, 내부 안쪽의 너비 측정) */
	MmeasSetMarker(strip_id, M_POLARITY, mlPolLeft, mlPolRight);
	/* Set the start location of inspection area */
	MmeasSetMarker(strip_id, M_BOX_ORIGIN, param->box_origin.x, param->box_origin.y);
	/* Sets the size of inspection area */
	MmeasSetMarker(strip_id, M_BOX_SIZE, param->box_size.x, param->box_size.y);
	/* Enable for rotation search in the measurement area (box) */
	MmeasSetMarker(strip_id, M_BOX_ANGLE_MODE, mlAngleEnable, M_NULL);
	if (param->enable_angle)
	{
		/* 90도인 경우, Angle의 범위를 양의 방향으로 90도까지 지정 */
		MmeasSetMarker(strip_id, M_BOX_ANGLE_DELTA_POS, param->angle_delta, M_NULL);
		/* 보간법 알고리즘 설정 */
		//MmeasSetMarker(strip_id, M_BOX_ANGLE_INTERPOLATION_MODE, mlAngleMethod, M_NULL);
		MmeasSetMarker(strip_id, M_SEARCH_REGION_ANGLE_INTERPOLATION_MODE, mlAngleMethod, M_NULL); 
	}
	//if (param->edge_width > 0.0f)
	//{
	//	/* 사용자가 width의 예상 값을 지정. 알고리즘이 검색을 더 용이하게 도와 줌? 폭을 예상하는 pxiel 크기 값 지정 */
	//	MmeasSetMarker(strip_id, M_WIDTH , param->edge_width, M_NULL);
	//	/* 폭의 허용 오차 pixel 값을 지정 */
	//	//MmeasSetMarker(strip_id, M_WIDTH_VARIATION, param->edge_offset, M_NULL);
	//	MmeasSetMarker(strip_id, M_WIDTH_VARIATION, param->edge_offset, M_NULL);
	//}
	/* Sets the output unit to a pixel value */
	MmeasSetMarker(strip_id, M_RESULT_OUTPUT_UNITS, M_PIXEL, M_NULL);

	return TRUE;
}
#endif
/*
 desc : 현재 적재된 레시피의 마크 검색 조건 값 설정
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
 desc : 현재 적재된 모델의 크기 반환
 parm : index	- [in]  등록된 모델의 인덱스 값 (zero-based)
 retn : 모델의 크기 값 (단위: pixel)
*/
UINT32 CMilModel::GetModelWidth(UINT8 index)
{
	DOUBLE dbDia = 1.0f;

	switch (m_pstMarkModel[index].type)
	{
		/* 아래 2개의 Type은 검색 등록 할 때, 지름 (길이)이 아닌 반지름 */
	case ENG_MMDT::en_ring:
	case ENG_MMDT::en_circle:	dbDia = 2.0f;
	}
	//return (UINT32)ROUNDED(m_pstModelSize[index].x * dbDia, 0);
	return (UINT32)ROUNDED(m_pstMarkModel[index].iSizeP.x * dbDia, 0);
	
}

/*
 desc : 현재 적재된 모델의 크기 반환
 parm : index	- [in]  등록된 모델의 인덱스 값 (zero-based)
 retn : 모델의 크기 값 (단위: pixel)
*/
UINT32 CMilModel::GetModelHeight(UINT8 index)
{
	DOUBLE dbDia = 1.0f;

	switch (m_pstMarkModel[index].type)
	{
		/* 아래 2개의 Type은 검색 등록 할 때, 지름 (길이)이 아닌 반지름 */
	case ENG_MMDT::en_ring:
	case ENG_MMDT::en_circle:	dbDia = 2.0f;
	}
	//return (UINT32)ROUNDED(m_pstModelSize[index].y * dbDia, 0);
	return (UINT32)ROUNDED(m_pstMarkModel[index].iSizeP.y * dbDia, 0);
}

/*
 desc : Mulit Mark 검색 방식일 경우, 즉, MMF 마크 검색 방식일 경우, 검색된 마크 영역의 주변을 그리기 위한 용도
		Mark 가 MMF 파일인 경우, 마크 크기 값
 parm : width	- [in]  사각형 영역의 넓이 값 (단위: um)
		height	- [in]  사각형 영역의 높이 값 (단위: um)
 retn : None
*/
VOID CMilModel::SetMultiMarkArea(UINT32 width, UINT32 height)
{
	m_u32MMFMarkWidth = width;
	m_u32MMFMarkHeight = height;
}

/* ----------------------------------------------------------------------------------------- */
/*                                 lk91 VISION 추가 함수                                     */
/* ----------------------------------------------------------------------------------------- */

/* desc : Regist Pattern Matching Mark (pat) */
BOOL CMilModel::RegistPat(PUINT8 img_src, CRect fi_rectArea, CString fi_filename, UINT8 mark_no)
{
	int width = fi_rectArea.right - fi_rectArea.left;
	int height = fi_rectArea.bottom - fi_rectArea.top;

#ifndef _NOT_USE_MIL_
	theApp.clMilMain.m_MarkSize.x = width;	// lk91 어디에 쓰이는지 m_pstMarkModel[mark_no].iSizeP 로 대체할 수 있는지
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
	theApp.clMilMain.m_MarkSize.x = width;// lk91 어디에 쓰이는지 m_pstMarkModel[mark_no].iSizeP 로 대체할 수 있는지
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
VOID CMilModel::SetMarkOffset(CPoint fi_MarkCenter, int setOffsetMode, int mark_no) // 0(default) : 일반 offset, 1 : NewOffset, 2: (MarkSet)NewOffset -> Offset 변환, 3: Draw 에 사용하는 임시 Offset
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

/* desc : Mark Size, Offset 초기화 */
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
	//--- SetMarkDisp 버퍼생성
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
			//MmodInquire(m_mlModelID_D, M_DEFAULT, M_ORIGINAL_X, &tOffset_x); // lk91 M_REFERENCE_는 안된다.. CDPoint 값 안돼
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
	//iSizeP = um_iTmpSizeP; // lk91 sizeP는 다 삭제하고 직접 불러오는 방식 사용하기...

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
		//마크띄우는곳
		// lk91 이상하게 dRate가 동일한데도 이미지 사이즈가 처음과 그 이후로 업데이트 될때 다르게 출력
		//MimResize(theApp.clMilMain.m_mImg_Mark[fi_iNo], theApp.clMilMain.m_mImgDisp_Mark[fi_iNo], dRate, dRate, M_DEFAULT);
		theApp.clMilMain.MARK_DISP_RATE = dRate;
#endif
	}

	//CPoint um_iNewOffP, um_iNewSizeP;
	//um_iNewOffP.x = (int)(m_pstMarkModel->iOffsetP.x);
	//um_iNewOffP.y = (int)(m_pstMarkModel->iOffsetP.y);
	//um_iNewSizeP.x = (int)(m_pstMarkModel->iSizeP.x);
	//um_iNewSizeP.y = (int)(m_pstMarkModel->iSizeP.y);

	// Overlay 그리기, fi_iDispType : 1
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
BOOL CMilModel::PutSetMarkDisp(HWND hwnd, RECT draw, UINT8 cam_id, TCHAR* file, int fi_findType, DOUBLE fi_dRate) // Mark Disp에 뿌리기
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

	//--- SetMarkDisp 버퍼생성
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
	// 최대 Max-Size확인
	CPoint	iMaxSizeP;
	iMaxSizeP.x = draw.right - draw.left;
	iMaxSizeP.y = draw.bottom - draw.top;
	// 마크사이즈를 해당 비율에 맞게 환산...
	CPoint	iTmpSizeP;
	iTmpSizeP.x = (int)(m_pstMarkModel[2].iSizeP.x * fi_dRate + 0.5); 
	iTmpSizeP.y = (int)(m_pstMarkModel[2].iSizeP.y * fi_dRate + 0.5);
	// 디스플레이버퍼 보다 사이즈가 크면 더 안커지게 필터링...
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

/* desc : PAT 파일에 MASK 정보 저장, Mark Set에서만 사용 */
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

/* desc : MMF 파일에 MASK 정보 저장, Mark Set에서만 사용 */
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

/* desc : PAT MARK 저장 */
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

/* desc : MMF MARK 저장 */
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

/* desc : Find Center (Mark Set에서만 사용) */
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

	// edge가 1.0 미만은 모두 1.0으로 edge가 그려진다... mgracontrol이 안먹혀.. 일단은 1.0 으로 제한 
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

/* desc : Mark 정보 그릴 때 사용하는 MIL 함수 */
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
		//MbufGet(theApp.clMilMain.MilMask, theApp.clMilMain.pucMaskBuf); // lk91 언제 들어오는지 체크...처음에만 들어오는지..
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
				theApp.clMilMain.pucMaskBuf[y * ibufSize + x] = 0xff;	// pModel 에 추가
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
	
	// lk91 ProcImage() 사용..
	theApp.clMilMain.ProcImage(m_camid-1, 0);
	
	if (img_proc == 1)// lk91 이미지 처리 전역 변수 추가
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

	// lk91 임시!!!!!!!!!!!!!!!
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
	//	if (Chk && HistValues[i] > 10000) { // lk91 10000은 자잘한거 제거한 것...좀더 확실한 값을... 찾아보쟈
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
	// lk91 임시!!!!!!!!!!!!!!!

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

	/* 구조체 값 초기화 (!!! 반드시 초기화 !!!) */
	m_u8MarkFindGet = 0x00;
	ResetMarkResult();

	/* Get the size of grabbed image (buffer) */
	u32GrabWidth = (UINT32)MbufInquire(grab_id, M_SIZE_X, NULL);
	u32GrabHeight = (UINT32)MbufInquire(grab_id, M_SIZE_Y, NULL);
	dbGrabCentX = (DOUBLE)u32GrabWidth / 2.0f;	/* 좌표 값은 Zero-based 이므로 */
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
	MmodPreprocess(m_mlModelID[mark_no], M_DEFAULT); // find 할때 Preprocess 하도록...

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
		/* 결과 버퍼 메모리 해제 */
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
	//MmodGetResult(mlResult, M_DEFAULT, M_INDEX + M_TYPE_MIL_INT, m_pMilIndex);	/* 모델 여러개 등록했을 때, 검색된 모델의 번호 */, 모델 각각 등록으로 변경
	MmodGetResult(mlResult, M_DEFAULT, M_POSITION_X, m_pFindPosX);
	MmodGetResult(mlResult, M_DEFAULT, M_POSITION_Y, m_pFindPosY);
	MmodGetResult(mlResult, M_DEFAULT, M_SCORE, m_pFindScore);
	MmodGetResult(mlResult, M_DEFAULT, M_SCALE, m_pFindScale);
	MmodGetResult(mlResult, M_DEFAULT, M_ANGLE, m_pFindAngle);
	MmodGetResult(mlResult, M_DEFAULT, M_MODEL_COVERAGE, m_pFindCovg);
	MmodGetResult(mlResult, M_DEFAULT, M_FIT_ERROR, m_pFindFitErr);
	if (m_ModelType == M_CIRCLE)
		MmodGetResult(mlResult, M_DEFAULT, M_RADIUS, m_pFindCircleRadius);

	/* 복합 마크 검색 방식일 경우, 검색 대상 개수만큼 찾지 못했는지 여부 확인 */
	if (ENG_MMSM::en_cent_side == m_enMarkMethod || ENG_MMSM::en_multi_only == m_enMarkMethod)
	{
		if (miModResults < m_u8MarkFindSet)
		{
			swprintf_s(tzData, 128, L"Not all were found (RunModelFind) : find_set (%u) > find_get (%u)",
				m_u8MarkFindSet, UINT8(miModResults));
			LOG_ERROR(ENG_EDIC::en_mil, tzData);
			bSucc = FALSE;	/* 작업 실패 */
		}
		/* 만약 찾고자 하는 개수보다 많이 검색된 경우, Score or Scale 값이 가장 높은 순 (내림차순)으로 정렬 */
		else if (miModResults > m_u8MarkFindSet)
		{
			/* 마지막 원소 전까지 비교하면 되므로 n-1 개수만큼 비교 */
			for (i = 0; i < miModResults - 1; i++)
			{
				k = i;	/* 기준 인덱스 */
				for (j = i + 1; j < miModResults; j++)
				{
#if 0
					/* SCALE 값이 0.000f 값에 가장 가까운 값일수록 좋음  */
					if (abs(1.0f - m_pFindScale[j]) > abs(1.0f - m_pFindScale[k]))	k = j;	/* 0.0 값에 가장 가까운 값이 높은 점수임 */
#else
					/* SCORE 값이 100.000 값에 가장 가까운 값일수록 좋음 */
					if (m_pFindScore[j] > m_pFindScore[k])	k = j;	/* 가장 높은 값일수록 높음 점수임 */
#endif
				}
				/* 기존 위치 값 임시 백업 */
				dbPosX = m_pFindPosX[i];
				dbPosY = m_pFindPosY[i];
				dbScore = m_pFindScore[i];
				dbScale = m_pFindScale[i];
				dbAngle = m_pFindAngle[i];
				dbCovg = m_pFindCovg[i];
				dbFitErr = m_pFindFitErr[i];
				dbCircleR = m_pFindCircleRadius[i];
				/* 현재 검색 기준 위치에 가장 높은 값으로 변경 */
				m_pFindPosX[i] = m_pFindPosX[k];
				m_pFindPosY[i] = m_pFindPosY[k];
				m_pFindScore[i] = m_pFindScore[k];
				m_pFindScale[i] = m_pFindScale[k];
				m_pFindAngle[i] = m_pFindAngle[k];
				m_pFindCovg[i] = m_pFindCovg[k];
				m_pFindFitErr[i] = m_pFindFitErr[k];
				m_pFindCircleRadius[i] = m_pFindCircleRadius[k];
				/* 기존 가장 높은 값을 검색 기준 값으로 변경 */
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
		/* 강제로 검색 개수를 최종 찾고자 하는 개수로 설정 */
		miModResults = m_u8MarkFindSet;
	}
	// X 기준 sorting 
	//if (ENG_MMSM::en_ph_step == m_enMarkMethod) {
	if (dlg_id == DISP_TYPE_CALB_EXPO) {
		if (miModResults < m_u8MarkFindSet)
		{
			swprintf_s(tzData, 128, L"Not all were found (RunModelFind) : find_set (%u) > find_get (%u)",
				m_u8MarkFindSet, UINT8(miModResults));
			LOG_ERROR(ENG_EDIC::en_mil, tzData);
			bSucc = FALSE;	/* 작업 실패 */
		}
		/* 만약 찾고자 하는 개수보다 많이 검색된 경우, Score or Scale 값이 가장 높은 순 (내림차순)으로 정렬 */
			/* 마지막 원소 전까지 비교하면 되므로 n-1 개수만큼 비교 */
		for (i = 0; i < miModResults - 1; i++)
		{
			k = i;	/* 기준 인덱스 */
			for (j = i + 1; j < miModResults; j++)
			{
#if 0
				/* SCALE 값이 0.000f 값에 가장 가까운 값일수록 좋음  */
				if (abs(1.0f - m_pFindScale[j]) > abs(1.0f - m_pFindScale[k]))	k = j;	/* 0.0 값에 가장 가까운 값이 높은 점수임 */
#else
				/* SCORE 값이 100.000 값에 가장 가까운 값일수록 좋음 */
				//if (m_pFindScore[j] > m_pFindScore[k])	k = j;	/* 가장 높은 값일수록 높음 점수임 */
				if (m_pFindPosX[j] < m_pFindPosX[k])	k = j;	/* 가장 높은 값일수록 높음 점수임 */
#endif
			}
			/* 기존 위치 값 임시 백업 */
			dbPosX = m_pFindPosX[i];
			dbPosY = m_pFindPosY[i];
			dbScore = m_pFindScore[i];
			dbScale = m_pFindScale[i];
			dbAngle = m_pFindAngle[i];
			dbCovg = m_pFindCovg[i];
			dbFitErr = m_pFindFitErr[i];
			dbCircleR = m_pFindCircleRadius[i];
			/* 현재 검색 기준 위치에 가장 높은 값으로 변경 */
			m_pFindPosX[i] = m_pFindPosX[k];
			m_pFindPosY[i] = m_pFindPosY[k];
			m_pFindScore[i] = m_pFindScore[k];
			m_pFindScale[i] = m_pFindScale[k];
			m_pFindAngle[i] = m_pFindAngle[k];
			m_pFindCovg[i] = m_pFindCovg[k];
			m_pFindFitErr[i] = m_pFindFitErr[k];
			m_pFindCircleRadius[i] = m_pFindCircleRadius[k];
			/* 기존 가장 높은 값을 검색 기준 값으로 변경 */
			m_pFindPosX[k] = dbPosX;
			m_pFindPosY[k] = dbPosY;
			m_pFindScore[k] = dbScore;
			m_pFindScale[k] = dbScale;
			m_pFindAngle[k] = dbAngle;
			m_pFindCovg[k] = dbCovg;
			m_pFindFitErr[k] = dbFitErr;
			m_pFindCircleRadius[k] = dbCircleR;

		}
		
		/* 강제로 검색 개수를 최종 찾고자 하는 개수로 설정 */
		miModResults = m_u8MarkFindSet;
	}

	//if (useMilDisp && (dlg_id == DISP_TYPE_MARK_LIVE || dlg_id == DISP_TYPE_MARK)) {
	if (useMilDisp && (dlg_id != DISP_TYPE_EXPO && dlg_id != DISP_TYPE_CALB_EXPO)) {
		if (bSucc)
		{

			theApp.clMilDisp.DrawOverlayDC(false, dlg_id, m_camid-1); //lk91 임시, fi_No인데.. m_camid 넣음
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

				/* 각도 측정 프로그램에서 호출한 경우라면, 아래 카메라 회전 값을 적용한 좌표 값 추출하지 않음 */
				if (!angle)
				{
					/* 얼라인 카메라의 회전 각도에 따라 검색된 마크의 중심 위치가 달라짐 */
					GetFindRotatePosXY(dbGrabCentX, dbGrabCentY, m_pFindPosX[i], m_pFindPosY[i]);
				}

				/* Grabbed Image의 중심 좌표에서 검색된 Mark의 중심 좌표 간의 직선의 거리 구하기 */
				m_pFindDist[i] = CalcLineDist(m_pFindPosX[i], m_pFindPosY[i], dbGrabCentX, dbGrabCentY);
				/* Scale Rate 값의 오차 값을 100 percent 기준으로 환산하여 변경 후 저장 */
				m_pFindScale[i] = (1.0f - fabs(1.0f - m_pFindScale[i])) * 100.0f;
				/* 검색된 객체의 크기가 등록된 마크 기준 대비 큰 것인지 아닌지 플래그 설정 */
				if (m_pFindScale[i] > 1.0f)			u8ScaleSize = 0x01;
				else if (m_pFindScale[i] < 1.0f)	u8ScaleSize = 0x02;
				/* --------------------------------------------------------------------------------------- */
				/* 검색 대상에 부합되는지 확인 */
				m_pstModResult[i].SetValue(UINT32(m_pMilIndex[i]),
					m_pFindScale[i], m_pFindScore[i], m_pFindDist[i],
					m_pFindAngle[i], m_pFindCovg[i], m_pFindFitErr[i],
					m_pFindPosX[i], m_pFindPosY[i], u8ScaleSize, m_pFindCircleRadius[i]);
			}
			// lk91 model 찾고 싶은데 못찾음....
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

					/* 각도 측정 프로그램에서 호출한 경우라면, 아래 카메라 회전 값을 적용한 좌표 값 추출하지 않음 */
					if (!angle)
					{
						/* 얼라인 카메라의 회전 각도에 따라 검색된 마크의 중심 위치가 달라짐 */
						GetFindRotatePosXY(dbGrabCentX, dbGrabCentY, m_pFindPosX[i], m_pFindPosY[i]);
					}

					/* Grabbed Image의 중심 좌표에서 검색된 Mark의 중심 좌표 간의 직선의 거리 구하기 */
					m_pFindDist[i] = CalcLineDist(m_pFindPosX[i], m_pFindPosY[i], dbGrabCentX, dbGrabCentY);
					/* Scale Rate 값의 오차 값을 100 percent 기준으로 환산하여 변경 후 저장 */
					m_pFindScale[i] = (1.0f - fabs(1.0f - m_pFindScale[i])) * 100.0f;
					/* 검색된 객체의 크기가 등록된 마크 기준 대비 큰 것인지 아닌지 플래그 설정 */
					if (m_pFindScale[i] > 1.0f)			u8ScaleSize = 0x01;
					else if (m_pFindScale[i] < 1.0f)	u8ScaleSize = 0x02;
					/* --------------------------------------------------------------------------------------- */
					/* 검색 대상에 부합되는지 확인 */
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

				/* 검색된 모델의 Target (Matching 모델이 아니라)에 대해 그림 그려 주기 */
				MgraColor(graph_id/*M_DEFAULT*/, M_COLOR_RED);
				/* Draw the model */
				MmodDraw(M_DEFAULT, mlResult, grab_id, miOperation, i, M_DEFAULT);
#endif
				/* 각도 측정 프로그램에서 호출한 경우라면, 아래 카메라 회전 값을 적용한 좌표 값 추출하지 않음 */
				if (!angle)
				{
					/* 얼라인 카메라의 회전 각도에 따라 검색된 마크의 중심 위치가 달라짐 */
					GetFindRotatePosXY(dbGrabCentX, dbGrabCentY, m_pFindPosX[i], m_pFindPosY[i]);
				}

				/* Grabbed Image의 중심 좌표에서 검색된 Mark의 중심 좌표 간의 직선의 거리 구하기 */
				m_pFindDist[i] = CalcLineDist(m_pFindPosX[i], m_pFindPosY[i], dbGrabCentX, dbGrabCentY);
				/* Scale Rate 값의 오차 값을 100 percent 기준으로 환산하여 변경 후 저장 */
				m_pFindScale[i] = (1.0f - fabs(1.0f - m_pFindScale[i])) * 100.0f;
				/* 검색된 객체의 크기가 등록된 마크 기준 대비 큰 것인지 아닌지 플래그 설정 */
				if (m_pFindScale[i] > 1.0f)			u8ScaleSize = 0x01;
				else if (m_pFindScale[i] < 1.0f)	u8ScaleSize = 0x02;
				/* --------------------------------------------------------------------------------------- */
				/* 검색 대상에 부합되는지 확인 */
				m_pstModResult[i].SetValue(UINT32(m_pMilIndex[i]),
					m_pFindScale[i], m_pFindScore[i], m_pFindDist[i],
					m_pFindAngle[i], m_pFindCovg[i], m_pFindFitErr[i],
					m_pFindPosX[i], m_pFindPosY[i], u8ScaleSize, m_pFindCircleRadius[i]);
			}
		}
	}

	/* 결과 버퍼 메모리 해제 */
	MmodFree(mlResult);

	/* 1 개 이상 찾아야 됨 */
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

	/* 구조체 값 초기화 (!!! 반드시 초기화 !!!) */
	m_u8MarkFindGet = 0x00;
	ResetMarkResult();

	/* Get the size of grabbed image (buffer) */
	u32GrabWidth = (UINT32)MbufInquire(grab_id, M_SIZE_X, NULL);
	u32GrabHeight = (UINT32)MbufInquire(grab_id, M_SIZE_Y, NULL);
	dbGrabCentX = (DOUBLE)u32GrabWidth / 2.0f;	/* 좌표 값은 Zero-based 이므로 */
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
		/* 결과 버퍼 메모리 해제 */
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
	MpatGetResult(mlResult, M_DEFAULT, M_INDEX + M_TYPE_MIL_INT, m_pMilIndex);	/* 모델 여러개 등록했을 때, 검색된 모델의 번호 */
	MpatGetResult(mlResult, M_DEFAULT, M_POSITION_X, m_pFindPosX);
	MpatGetResult(mlResult, M_DEFAULT, M_POSITION_Y, m_pFindPosY);
	MpatGetResult(mlResult, M_DEFAULT, M_SCORE, m_pFindScore);

	for (int i = 0; i < miPatResults; i++) {
		m_pFindScale[i] = 0.0;
		m_pFindAngle[i] = 0.0;
		m_pFindCovg[i] = 0.0;
		m_pFindFitErr[i] = 0.0;
	}

	/* 복합 마크 검색 방식일 경우, 검색 대상 개수만큼 찾지 못했는지 여부 확인 */
	if (ENG_MMSM::en_cent_side == m_enMarkMethod || ENG_MMSM::en_multi_only == m_enMarkMethod)
	{
		if (miPatResults < m_u8MarkFindSet)
		{
			swprintf_s(tzData, 128, L"Not all were found (RunPATFind) : find_set (%u) > find_get (%u)",
				m_u8MarkFindSet, UINT8(miPatResults));
			LOG_ERROR(ENG_EDIC::en_mil, tzData);
			bSucc = FALSE;	/* 작업 실패 */
		}
		/* 만약 찾고자 하는 개수보다 많이 검색된 경우, Score or Scale 값이 가장 높은 순 (내림차순)으로 정렬 */
		else if (miPatResults > m_u8MarkFindSet)
		{
			/* 마지막 원소 전까지 비교하면 되므로 n-1 개수만큼 비교 */
			for (i = 0; i < miPatResults - 1; i++)
			{
				k = i;	/* 기준 인덱스 */
				for (j = i + 1; j < miPatResults; j++)
				{
#if 0
					/* SCALE 값이 0.000f 값에 가장 가까운 값일수록 좋음  */
					if (abs(1.0f - m_pFindScale[j]) > abs(1.0f - m_pFindScale[k]))	k = j;	/* 0.0 값에 가장 가까운 값이 높은 점수임 */
#else
					/* SCORE 값이 100.000 값에 가장 가까운 값일수록 좋음 */
					if (m_pFindScore[j] > m_pFindScore[k])	k = j;	/* 가장 높은 값일수록 높음 점수임 */
#endif
				}
				/* 기존 위치 값 임시 백업 */
				dbPosX = m_pFindPosX[i];
				dbPosY = m_pFindPosY[i];
				dbScore = m_pFindScore[i];
				dbScale = m_pFindScale[i];
				dbAngle = m_pFindAngle[i];
				dbCovg = m_pFindCovg[i];
				dbFitErr = m_pFindFitErr[i];
				/* 현재 검색 기준 위치에 가장 높은 값으로 변경 */
				m_pFindPosX[i] = m_pFindPosX[k];
				m_pFindPosY[i] = m_pFindPosY[k];
				m_pFindScore[i] = m_pFindScore[k];
				m_pFindScale[i] = m_pFindScale[k];
				m_pFindAngle[i] = m_pFindAngle[k];
				m_pFindCovg[i] = m_pFindCovg[k];
				m_pFindFitErr[i] = m_pFindFitErr[k];
				/* 기존 가장 높은 값을 검색 기준 값으로 변경 */
				m_pFindPosX[k] = dbPosX;
				m_pFindPosY[k] = dbPosY;
				m_pFindScore[k] = dbScore;
				m_pFindScale[k] = dbScale;
				m_pFindAngle[k] = dbAngle;
				m_pFindCovg[k] = dbCovg;
				m_pFindFitErr[k] = dbFitErr;
			}
		}
		/* 강제로 검색 개수를 최종 찾고자 하는 개수로 설정 */
		miPatResults = m_u8MarkFindSet;
	}
	// X 기준 sorting 
	//if (ENG_MMSM::en_ph_step == m_enMarkMethod) {
	if (dispType == DISP_TYPE_CALB_EXPO) {
		if (miPatResults < m_u8MarkFindSet)
		{
			swprintf_s(tzData, 128, L"Not all were found (RunModelFind) : find_set (%u) > find_get (%u)",
				m_u8MarkFindSet, UINT8(miPatResults));
			LOG_ERROR(ENG_EDIC::en_mil, tzData);
			bSucc = FALSE;	/* 작업 실패 */
		}
		/* 만약 찾고자 하는 개수보다 많이 검색된 경우, Score or Scale 값이 가장 높은 순 (내림차순)으로 정렬 */
			/* 마지막 원소 전까지 비교하면 되므로 n-1 개수만큼 비교 */
		for (i = 0; i < miPatResults - 1; i++)
		{
			k = i;	/* 기준 인덱스 */
			for (j = i + 1; j < miPatResults; j++)
			{
#if 0
				/* SCALE 값이 0.000f 값에 가장 가까운 값일수록 좋음  */
				if (abs(1.0f - m_pFindScale[j]) > abs(1.0f - m_pFindScale[k]))	k = j;	/* 0.0 값에 가장 가까운 값이 높은 점수임 */
#else
				/* SCORE 값이 100.000 값에 가장 가까운 값일수록 좋음 */
				//if (m_pFindScore[j] > m_pFindScore[k])	k = j;	/* 가장 높은 값일수록 높음 점수임 */
				if (m_pFindPosX[j] < m_pFindPosX[k])	k = j;	/* 가장 높은 값일수록 높음 점수임 */
#endif
			}
			/* 기존 위치 값 임시 백업 */
			dbPosX = m_pFindPosX[i];
			dbPosY = m_pFindPosY[i];
			dbScore = m_pFindScore[i];
			dbScale = m_pFindScale[i];
			dbAngle = m_pFindAngle[i];
			dbCovg = m_pFindCovg[i];
			dbFitErr = m_pFindFitErr[i];
			/* 현재 검색 기준 위치에 가장 높은 값으로 변경 */
			m_pFindPosX[i] = m_pFindPosX[k];
			m_pFindPosY[i] = m_pFindPosY[k];
			m_pFindScore[i] = m_pFindScore[k];
			m_pFindScale[i] = m_pFindScale[k];
			m_pFindAngle[i] = m_pFindAngle[k];
			m_pFindCovg[i] = m_pFindCovg[k];
			m_pFindFitErr[i] = m_pFindFitErr[k];
			/* 기존 가장 높은 값을 검색 기준 값으로 변경 */
			m_pFindPosX[k] = dbPosX;
			m_pFindPosY[k] = dbPosY;
			m_pFindScore[k] = dbScore;
			m_pFindScale[k] = dbScale;
			m_pFindAngle[k] = dbAngle;
			m_pFindCovg[k] = dbCovg;
			m_pFindFitErr[k] = dbFitErr;
		}

		/* 강제로 검색 개수를 최종 찾고자 하는 개수로 설정 */
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

				/* 각도 측정 프로그램에서 호출한 경우라면, 아래 카메라 회전 값을 적용한 좌표 값 추출하지 않음 */
				if (!angle)
				{
					/* 얼라인 카메라의 회전 각도에 따라 검색된 마크의 중심 위치가 달라짐 */
					GetFindRotatePosXY(dbGrabCentX, dbGrabCentY, m_pFindPosX[i], m_pFindPosY[i]);
				}

				/* Grabbed Image의 중심 좌표에서 검색된 Mark의 중심 좌표 간의 직선의 거리 구하기 */
				m_pFindDist[i] = CalcLineDist(m_pFindPosX[i], m_pFindPosY[i], dbGrabCentX, dbGrabCentY);
				/* Scale Rate 값의 오차 값을 100 percent 기준으로 환산하여 변경 후 저장 */
				m_pFindScale[i] = (1.0f - fabs(1.0f - m_pFindScale[i])) * 100.0f;
				/* 검색된 객체의 크기가 등록된 마크 기준 대비 큰 것인지 아닌지 플래그 설정 */
				if (m_pFindScale[i] > 1.0f)			u8ScaleSize = 0x01;
				else if (m_pFindScale[i] < 1.0f)	u8ScaleSize = 0x02;
				/* --------------------------------------------------------------------------------------- */
				/* 검색 대상에 부합되는지 확인 */
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

					/* 각도 측정 프로그램에서 호출한 경우라면, 아래 카메라 회전 값을 적용한 좌표 값 추출하지 않음 */
					if (!angle)
					{
						/* 얼라인 카메라의 회전 각도에 따라 검색된 마크의 중심 위치가 달라짐 */
						GetFindRotatePosXY(dbGrabCentX, dbGrabCentY, m_pFindPosX[i], m_pFindPosY[i]);
					}

					/* Grabbed Image의 중심 좌표에서 검색된 Mark의 중심 좌표 간의 직선의 거리 구하기 */
					m_pFindDist[i] = CalcLineDist(m_pFindPosX[i], m_pFindPosY[i], dbGrabCentX, dbGrabCentY);
					/* Scale Rate 값의 오차 값을 100 percent 기준으로 환산하여 변경 후 저장 */
					m_pFindScale[i] = (1.0f - fabs(1.0f - m_pFindScale[i])) * 100.0f;
					/* 검색된 객체의 크기가 등록된 마크 기준 대비 큰 것인지 아닌지 플래그 설정 */
					if (m_pFindScale[i] > 1.0f)			u8ScaleSize = 0x01;
					else if (m_pFindScale[i] < 1.0f)	u8ScaleSize = 0x02;
					/* --------------------------------------------------------------------------------------- */
					/* 검색 대상에 부합되는지 확인 */
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

							/* 검색된 모델의 Target (Matching 모델이 아니라)에 대해 그림 그려 주기 */
				MgraColor(graph_id/*M_DEFAULT*/, M_COLOR_RED);
				/* Draw the model */
				MpatDraw(M_DEFAULT, mlResult, grab_id, miOperation, i, M_DEFAULT);
#endif
				/* 각도 측정 프로그램에서 호출한 경우라면, 아래 카메라 회전 값을 적용한 좌표 값 추출하지 않음 */
				if (!angle)
				{
					/* 얼라인 카메라의 회전 각도에 따라 검색된 마크의 중심 위치가 달라짐 */
					GetFindRotatePosXY(dbGrabCentX, dbGrabCentY, m_pFindPosX[i], m_pFindPosY[i]);
				}

				/* Grabbed Image의 중심 좌표에서 검색된 Mark의 중심 좌표 간의 직선의 거리 구하기 */
				m_pFindDist[i] = CalcLineDist(m_pFindPosX[i], m_pFindPosY[i], dbGrabCentX, dbGrabCentY);
				/* Scale Rate 값의 오차 값을 100 percent 기준으로 환산하여 변경 후 저장 */
				m_pFindScale[i] = (1.0f - fabs(1.0f - m_pFindScale[i])) * 100.0f;
				/* 검색된 객체의 크기가 등록된 마크 기준 대비 큰 것인지 아닌지 플래그 설정 */
				if (m_pFindScale[i] > 1.0f)			u8ScaleSize = 0x01;
				else if (m_pFindScale[i] < 1.0f)	u8ScaleSize = 0x02;
				/* --------------------------------------------------------------------------------------- */
				/* 검색 대상에 부합되는지 확인 */
				m_pstModResult[i].SetValue(UINT32(m_pMilIndex[i]),
					m_pFindScale[i], m_pFindScore[i], m_pFindDist[i],
					m_pFindAngle[i], m_pFindCovg[i], m_pFindFitErr[i],
					m_pFindPosX[i], m_pFindPosY[i], u8ScaleSize, 0.0);
			}
		}
	}
	/* 결과 버퍼 메모리 해제 */
	MpatFree(mlResult);

	/* 1 개 이상 찾아야 됨 */
	m_u8MarkFindGet = UINT8(miPatResults);
	return miPatResults > 0;
}
#endif

/* desc : Mark 정보 설정 - MMF (MIL Model Find File) */
BOOL CMilModel::SetModelDefineMMF(PTCHAR name, PTCHAR mmf, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no)
{
	CUniToChar csCnv;

	/* 기존 설정되어 있다면 일단 해제 */
	ReleaseMarkModel(mark_no, 0);
	/* 기존 결과 정보 초기화 */
	ResetMarkResult();
	/* 등록하고자 하는 모델의 개수 파악 */
	//m_u8ModelRegist = 0x01;	/* MMF 인 경우, 무조건 1개 밖에 안됨 */
#ifndef _NOT_USE_MIL_
	/* Model 적재 */
	MmodRestore(mmf, theApp.clMilMain.m_mSysID, M_DEFAULT, &m_mlModelID[mark_no]);
	if (!m_mlModelID[mark_no] || MappGetError(M_CURRENT, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"MIL_ERR : Failed to restore the mark image (buffer, mmf)");
		return FALSE;
	}
	/* Model 정보 설정 */
	m_pstMarkModel[mark_no].type = UINT32(ENG_MMDT::en_image);
	m_pstMarkModel[mark_no].iSizeP = m_MarkSizeP;
	m_pstMarkModel[mark_no].iOffsetP = m_MarkCenterP;
	sprintf_s(m_pstMarkModel[mark_no].name, MARK_MODEL_NAME_LENGTH, "%s", csCnv.Str2Ansi(name));
	/* Model Size 얻기 */
#if 0
	m_pstModelSize[0].x = (DOUBLE)MbufInquire(m_mlModelID, M_SIZE_X, M_NULL);
	m_pstModelSize[0].y = (DOUBLE)MbufInquire(m_mlModelID, M_SIZE_Y, M_NULL);
#else	/* 현재 MMF에 등록된 모델 크기 얻어오는 방법을 몰라서. 무조건 100 값으로 고정 (?) T.T */
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
	MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_NUMBER, 1); // lk91 이미지는 일단 1개로 고정...
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

/* desc : Mark 정보 설정 - PAT (MIL Model Find File) */
BOOL CMilModel::SetModelDefinePAT(PTCHAR name, PTCHAR pat, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no)
{
	CUniToChar csCnv;

	/* 기존 설정되어 있다면 일단 해제 */
	ReleaseMarkModel(mark_no, 1);
	/* 기존 결과 정보 초기화 */
	ResetMarkResult();
	/* 등록하고자 하는 모델의 개수 파악 */
	//m_u8ModelRegist = 0x01;	/* MMF 인 경우, 무조건 1개 밖에 안됨 */
#ifndef _NOT_USE_MIL_
	if (m_mlPATID[mark_no]) {
		MpatFree(m_mlPATID[mark_no]);
		m_mlPATID[mark_no] = M_NULL;
	}
	MpatAlloc(theApp.clMilMain.m_mSysID, M_DEFAULT, M_DEFAULT, &m_mlPATID[mark_no]);

	/* Model 적재 */
	MpatRestore(pat, theApp.clMilMain.m_mSysID, M_DEFAULT, &m_mlPATID[mark_no]);
	if (!m_mlPATID[mark_no] || MappGetError(M_CURRENT, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"MIL_ERR : Failed to restore the mark image (buffer, pat)");
		return FALSE;
	}
	/* Model 정보 설정 */
	m_pstMarkModel[mark_no].type = UINT32(ENG_MMDT::en_none);
	m_pstMarkModel[mark_no].iSizeP = m_MarkSizeP;
	m_pstMarkModel[mark_no].iOffsetP = m_MarkCenterP;
	sprintf_s(m_pstMarkModel[mark_no].name, MARK_MODEL_NAME_LENGTH, "%s", csCnv.Str2Ansi(name));
	/* Model Size 얻기 */
#if 0
	m_pstModelSize[0].x = (DOUBLE)MbufInquire(m_mlModelID, M_SIZE_X, M_NULL);
	m_pstModelSize[0].y = (DOUBLE)MbufInquire(m_mlModelID, M_SIZE_Y, M_NULL);
#else	/* 현재 MMF에 등록된 모델 크기 얻어오는 방법을 몰라서. 무조건 100 값으로 고정 (?) T.T */
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

		/* 기존 설정되어 있다면 일단 해제 */
		ReleaseMarkModel(mark_no, 0);

		UINT32 i = 0, j = 0;
		LONG lSpeed[4] = { M_LOW, M_MEDIUM, M_HIGH, M_VERY_HIGH };
		LONG lLevel[3] = { M_MEDIUM, M_HIGH, M_VERY_HIGH };
		LONG lFilter[3] = { M_DEFAULT , M_KERNEL , M_RECURSIVE };	/* Filter Mode */
		DOUBLE dbPixel[4] = { NULL };	/* um --> size */
		DOUBLE dbDivSize = 1.0f;	/* Ring과 Circle의 경우, 모델 크기 등록할 때, 반지름으로 해야하기 때문에 */
		DOUBLE dbPixelToMM = m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1);
		DOUBLE dbCertainty = 0.0f;
		/* 가장 최근에 등록된 Mark Model 저장 */
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
		/* Pixel Size 구하기 */
		for (j = 0; j < 4; j++)
		{
			dbPixel[j] = m_pstMarkModel[mark_no].param[j + 1] / (dbPixelToMM * 1000.0f);
		}
		switch (m_pstMarkModel[mark_no].type)
		{
			/* 아래 2개의 Type은 검색 등록 할 때, 지름 (길이)이 아닌 반지름 */
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
		
		// lk91 MmodControl 동일하게 유지...
		/* 이미지 노이즈 감소 값 설정 */
		if (smooth < 0.0f || smooth > 100.0f)	smooth = 50.0f;
		/* 모델의 검색 속도 지정 (모델 크기가 작거나 매칭률에서 높은 정확성을 필요로 하거나, 모델의 에지가 기하학적으로 복잡한 경우에는 디폴트 설정 (M_MEDIUM) 사용) */
		MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_SPEED, lSpeed[speed]);
		MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_SMOOTHNESS, smooth);
		/* 모델의 정확성을 요구할 때 (Accuracy가 높을수록 처리 속도는 떨어지게 됨) */
		MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_ACCURACY, M_MEDIUM);
		MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_ACTIVE_EDGELS, 100.0);
		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_POLARITY, M_ANY);

		//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_ACCEPTANCE, M_DEFAULT);
		//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_ANGLE, M_DEFAULT);
		//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_ANGLE_DELTA_NEG, 45.0);
		//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_ANGLE_DELTA_POS, 45.0);
		//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_FIT_ERROR_WEIGHTING_FACTOR, 25);
		MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_TIMEOUT, 2000);
		/* Target Image에서 검색하고자 하는 최대 개수 지정 */
		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_NUMBER, MIL_INT(count));
		//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_SCALE_RANGE, M_ENABLE);
		//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SHARED_EDGES, M_ENABLE);
		//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SAVE_TARGET_EDGES, M_ENABLE);
		//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_TARGET_CACHING, M_ENABLE);

	///* 등록된 마크 기준으로, Scale 최소 / 최대 값 설정 */
	//	if (scale_min > 0.0f && scale_max > 0.0f)
	//	{
	//		/* scale_min과 scale_max 값 유효성 확인 후 잘못된 값 조정 */
	//		if (scale_min < 0.5)	scale_min = 0.5f;
	//		if (scale_min > 1.0)	scale_min = 1.0f;
	//		if (scale_max < 1.0)	scale_min = 1.0f;
	//		if (scale_max > 2.0)	scale_min = 2.0f;

	//		/* 모델 크기 비율 지정 */
	//		MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_SEARCH_SCALE_RANGE, M_ENABLE);
	//		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SCALE, 1.0f);
	//		/* 이 주어진 범위 내에서만, 등록된 마크와 유사성이 가장 높은 것을 추출해 냄 즉, Score 값이 가장 높은 것을 추출해 냄 */
	//		/* scale_min : 0.5 ~ 1.0, scale_max : 1.0 ~ 2.0 사이의 범위 값을 가져야만 됨 */
	//		if (0.0f != scale_min)	MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SCALE_MIN_FACTOR, scale_min);
	//		if (0.0f != scale_max)	MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SCALE_MAX_FACTOR, scale_max);
	//	}
	//	/* Score 최소 검색 값 설정 */
	//	if (score_min > 0.0f)
	//	{
	//		/* Target Image에서 Model 검색 설정에 기준이되는 매칭 점수를 결정 */
	//		/* 만약 이 값을 100 으로 설정하면, 모델 내의 Active Edge가 Target의 검색된 Object와 완전히 매칭해야 함 */
	//		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_ACCEPTANCE, score_min);
	//		//if (score_min > dbCertainty)	dbCertainty = score_min + score_min / 10;	/* 항상 10% 높게 설정 */
	//		//MmodControl(m_mlModelID, M_DEFAULT, M_CERTAINTY, dbCertainty);
	//	}
	//	/* Score (for target) 최소 검색 값 설정 */
	//	if (score_tgt > 0.0f)
	//	{
	//		/* score acceptacne 값으로 검색된 모델 중 target score 값이 이 값 이하인 경우, 검색 대상에서 제외 */
	//		/* 현재 grabbed image 들 중에서 mark model과 유사한 이미지의 분포 정도 값 설정 후 검색하는 방법임 */
	//		MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_ACCEPTANCE_TARGET, score_tgt);
	//	}

		
		/* Preprocess the search context */

		DOUBLE tSize_x = 0, tSize_y = 0;
		MmodInquire(m_mlModelID[mark_no], M_DEFAULT, M_ALLOC_SIZE_X, &tSize_x);
		MmodInquire(m_mlModelID[mark_no], M_DEFAULT, M_ALLOC_SIZE_Y, &tSize_y);
		m_pstMarkModel[mark_no].iSizeP.x = (LONG)tSize_x;
		m_pstMarkModel[mark_no].iSizeP.y = (LONG)tSize_y;

		// lk91 mark 패턴 이미지가 아닌거는 Offset이 0값이라서 직접 나눠줘야함
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
			/* 기존 설정되어 있다면 일단 해제 */
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

			if (m_pstMarkModel[mark_no].iSizeP.x == 0 && m_pstMarkModel[mark_no].iSizeP.y == 0) { // lk91 mark 이미지로 저장 안했을때 예외처리
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

			// lk91 MmodControl 동일하게 유지...
			/* 이미지 노이즈 감소 값 설정 */
			if (smooth < 0.0f || smooth > 100.0f)	smooth = 50.0f;
			/* 모델의 검색 속도 지정 (모델 크기가 작거나 매칭률에서 높은 정확성을 필요로 하거나, 모델의 에지가 기하학적으로 복잡한 경우에는 디폴트 설정 (M_MEDIUM) 사용) */
			MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_SPEED, lSpeed[speed]);
			MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_SMOOTHNESS, smooth);
			/* 모델의 정확성을 요구할 때 (Accuracy가 높을수록 처리 속도는 떨어지게 됨) */
			MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_ACCURACY, M_MEDIUM);
			MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_ACTIVE_EDGELS, 100.0);
			MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_POLARITY, M_ANY);

			//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_ACCEPTANCE, M_DEFAULT);
			//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_ANGLE, M_DEFAULT);
			//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_ANGLE_DELTA_NEG, 45.0);
			//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_ANGLE_DELTA_POS, 45.0);
			//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_FIT_ERROR_WEIGHTING_FACTOR, 25);
			MmodControl(m_mlModelID[mark_no], M_CONTEXT, M_TIMEOUT, 2000);
			/* Target Image에서 검색하고자 하는 최대 개수 지정 */
			MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_NUMBER, MIL_INT(count));
			//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SEARCH_SCALE_RANGE, M_ENABLE);
			//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SHARED_EDGES, M_ENABLE);
			//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_SAVE_TARGET_EDGES, M_ENABLE);
			//MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_TARGET_CACHING, M_ENABLE);



		}
		else {
			/* 기존 설정되어 있다면 일단 해제 */
			ReleaseMarkModel(mark_no, 1);
			if (m_mlPATID[mark_no]) {
				MpatFree(m_mlPATID[mark_no]);
				m_mlPATID[mark_no] = M_NULL;
			}
			MpatAlloc(theApp.clMilMain.m_mSysID, M_DEFAULT, M_DEFAULT, &m_mlPATID[mark_no]);

			/* Model 적재 */
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

			// lk91 임시 추가
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

	///* 기존 설정되어 있다면 일단 해제 */
	//ReleaseMarkModel(mark_no);
	/* 기존 결과 정보 초기화 */
	ResetMarkResult();
	/* 등록하고자 하는 모델 개수 설정 */
	//m_u8ModelRegist = count;
	/* 등록 대상 모델의 개수가 초과하면, 최대 값으로 설정 */
	//if (m_pstConfig->mark_find.max_mark_regist < count)
	//{
	//	m_u8ModelRegist = m_pstConfig->mark_find.max_mark_regist;
	//	AfxMessageBox(L"The number of registration models was exceeded,\n"
	//		L"and replaced with the maximum value defined in the config file",
	//		MB_ICONINFORMATION);
	//}
	/* 유효한 검색 대상의 모델 값 저장 */
	//for (i = 0; i < m_u8ModelRegist && value[i].IsValid(); i++)
	//{
	//	memcpy(&m_pstMarkModel[i], &value[i], sizeof(STG_CMPV));
	//}
	memcpy(&m_pstMarkModel[mark_no], value, sizeof(STG_CMPV)); 

	return SetModelDefine_tot(speed, level, count, smooth, mark_no, file, scale_min, scale_max, score_min);
}

BOOL CMilModel::MergeMark(LPG_CMPV value, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, UINT8 mark_no, TCHAR* file1, TCHAR* file2,
	TCHAR* RecipeName, DOUBLE scale_min, DOUBLE scale_max, DOUBLE score_min, DOUBLE score_tgt) // file1, file2, 레시피 명
{
#ifndef _NOT_USE_MIL_
	// 버튼 하나로 종결
	LONG lSpeed[4] = { M_LOW, M_MEDIUM, M_HIGH, M_VERY_HIGH };
	const int cntMerge = 3;
	const int cntParam = 4;
	MIL_INT64 m_type[cntMerge];
	DOUBLE modelParam[cntMerge][cntParam];
	DOUBLE dbDivSize = 1.0f;	/* Ring과 Circle의 경우, 모델 크기 등록할 때, 반지름으로 해야하기 때문에 */
	DOUBLE dbPixelToMM = m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1);
	MIL_DOUBLE dbPixel[cntMerge][cntParam] = { NULL };	/* um --> size */
	DOUBLE dbSizeX[cntMerge];
	DOUBLE dbSizeY[cntMerge];
	DOUBLE dbMaxSizeX = 0.0; // merge 할 때, margin 값 넣기 위해
	DOUBLE dbMaxSizeY = 0.0; // merge 할 때, margin 값 넣기 위해
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

	// 임시 값 설정.. 추후 dialog 로 값 받을 예정...
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

	/* 가장 최근에 등록된 Mark Model 저장 */
	MIL_ID mergeModelID;
	MmodAlloc(theApp.clMilMain.m_mSysID, M_GEOMETRIC, M_DEFAULT, &mergeModelID);

	if (!mergeModelID || MappGetError(M_CURRENT, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"MILL_ERR : Failed to allocate the model image (buffer)");
		return FALSE;
	}

	/* Pixel Size 구하기 */
	for (int i = 0; i < cntMerge; i++) {
		for (int j = 0; j < cntParam; j++) {
			dbPixel[i][j] = modelParam[i][j] / (dbPixelToMM * 1000.0f);
		}
	}

	for (int i = 0; i < cntMerge; i++) {
		switch (ENG_MMDT(m_type[i]))
		{
			/* 아래 2개의 Type은 검색 등록 할 때, 지름 (길이)이 아닌 반지름 */
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

	// lk91 MmodControl 동일하게 유지...
	/* 이미지 노이즈 감소 값 설정 */
	if (dbSmooth < 0.0f || dbSmooth > 100.0f)	dbSmooth = 50.0f;
	/* 모델의 검색 속도 지정 (모델 크기가 작거나 매칭률에서 높은 정확성을 필요로 하거나, 모델의 에지가 기하학적으로 복잡한 경우에는 디폴트 설정 (M_MEDIUM) 사용) */
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
	// 추후에는 파일명 받아서 저장 할 수 있도록...
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

	// lk91 ProcImage() 사용..
	theApp.clMilMain.ProcImage(m_camid - 1, 0);

	UINT8 u8ScaleSize = 0x00 /* equal */; /* 0x01 : ScaleUp, 0x02 : ScaleDown */
	BOOL bSucc = TRUE;
	TCHAR tzData[128] = { NULL };
	UINT32 u32GrabWidth = 0, u32GrabHeight = 0, i, j, k;
	UINT32 u32MaxCount;
	int calib_col = col; // lk91 나중에 값 받아야해
	int calib_row = row;
	u32MaxCount = calib_col * calib_row; // lk91 임시!! row * col

	int* Point_x = (int*)malloc(sizeof(int) * (calib_row * calib_col));
	int* Point_y = (int*)malloc(sizeof(int) * (calib_row * calib_col));

	memset(Point_x, 0, sizeof(int) * (calib_row * calib_col));
	memset(Point_y, 0, sizeof(int) * (calib_row * calib_col));


	DOUBLE dbGrabCentX = 0.0f, dbGrabCentY = 0.0f/*,*/ /*dbLineDist = 0.0f,*/ /*dbModelSize*/;
	DOUBLE dbPosX, dbPosY, dbScore, dbScale, dbAngle, dbCovg, dbFitErr, dbCircleR;
	MIL_ID mlResult = M_NULL, miModResults = 0, miOperation;

	/* 구조체 값 초기화 (!!! 반드시 초기화 !!!) */
	m_u8MarkFindGet = 0x00;
	ResetMarkResult();

	/* Get the size of grabbed image (buffer) */
	u32GrabWidth = (UINT32)MbufInquire(grab_id, M_SIZE_X, NULL);
	u32GrabHeight = (UINT32)MbufInquire(grab_id, M_SIZE_Y, NULL);
	dbGrabCentX = (DOUBLE)u32GrabWidth / 2.0f;	/* 좌표 값은 Zero-based 이므로 */
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
			MmodPreprocess(m_mlModelID[mark_no], M_DEFAULT); // find 할때 Preprocess 하도록...

			MmodAllocResult(theApp.clMilMain.m_mSysID, M_DEFAULT, &mlResult);
			MmodControl(m_mlModelID[mark_no], M_DEFAULT, M_NUMBER, 1); // 한 roi 내에 값은 1개

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
	sTmp = "단위 : [mm]";
	theApp.clMilDisp.AddTextList(dlg_id, m_camid - 1, ACA1920_SIZE_X - 300, ACA1920_SIZE_Y - 100, sTmp, eM_COLOR_MAGENTA, 10, 20, VISION_FONT_TEXT, true);

	// x
	int cnt = 1;
	for (int i = 0; i < calib_row * calib_col; i++) 
	{
		if (i == (calib_col * cnt) - 1)
			cnt++;
		else{
			if ((Point_x[i + 1] == 0 && Point_y[i + 1] == 0) || (Point_x[i] == 0 && Point_y[i] == 0)) // NG 예외처리
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
		if ((Point_x[i + calib_col] == 0 && Point_y[i + calib_col] == 0) || (Point_x[i] == 0 && Point_y[i] == 0)) // NG 예외처리
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

/* desc : MIL 할당 해제 */
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
Marker를 검색합니다.검색 후, 결과를 Draw 함수를 이용하여 width 와 측정상자의 외곽을 그립니다
MmeasFindMarker(M_DEFAULT, MilTargetImage, MilStripMarker, M_CONTRAST);
MmeasDraw(M_DEFAULT, MilStripMarker, MilOverlay, M_DRAW_WIDTH + M_DRAW_BOX, M_DEFAULT, M_RESULT);

// 4. Result
GetResult함수를 이용하여 width 를 얻어와서 출력합니다
MmeasGetResult(MilStripMarker, M_WIDTH, &vWidth, M_NULL);
strWidth.Format("Width= %. 3f pixels", vWidth);
MgraText(M_DEFAULT, MilOverlay, 0, 0, (char*)(const char*)strWidth);
#endif