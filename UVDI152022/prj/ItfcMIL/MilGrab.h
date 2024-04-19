#pragma once

#include "MilImage.h"

class AlignMotion;

class CMilGrab : public CMilImage
{
/* 생성자 & 파괴자 */
public:
#ifndef _NOT_USE_MIL_
	CMilGrab(LPG_CIEA config, LPG_VDSM shmem,
			 UINT8 cam_id, /*, MIL_ID ml_dis, */MIL_ID ml_sys,ENG_ERVM run_mode);
#else
	CMilGrab(LPG_CIEA config, LPG_VDSM shmem,
			UINT8 cam_id, ENG_ERVM run_mode);
#endif
	virtual ~CMilGrab();


/* 가상 함수 */
protected:

	AlignMotion* alignMotionPtr = nullptr;;

/* 로컬 변수 */
protected:

	UINT8				m_u8ResultAll;		/* 검색된 모델 개수 저장 */
	BOOL				m_bMatched;			/* Matching 여부 */
	BOOL				m_bStageMoveDirect;	/* 스테이지 이동 방향 (정방향 / 역방향) */

	/* 거버에 위치한 Mark의 중심 X / Y 좌표 */
	DOUBLE				m_dbGerbMarkX;
	DOUBLE				m_dbGerbMarkY;
//	DOUBLE				m_dbScoreRate;
	DOUBLE				m_dbRadiusAngle;

	/* 현재 선택된 레시피의 Mark 검색하는데 필요한 조건 */
	DOUBLE				m_dbRecipeScore;
	DOUBLE				m_dbRecipeScale;

	LPG_GMSR			m_pstResultAll;		/* 기타 결과 저장 (검색된 Mark 영역을 그리기 위함) */
	LPG_ACGR			m_pstGrabResult;	/* 최종 결과 저장 */


/* 로컬 함수 */
protected:

	VOID				PutGrabReset();
	BOOL				PutGrabImage(PUINT8 image);

/* 공용 함수 */
public:

	BOOL				PutGrabImage(PUINT8 image, UINT8 img_id,
									 DOUBLE mark_x=0.0f, DOUBLE mark_y=0.0f);

	/* 검색 결과 값 반환 */
//	DOUBLE				GetFindScore()			{	return m_dbScoreRate;				}
	DOUBLE				GetFindAngle()			{	return m_dbRadiusAngle;				}

	VOID				SetGrabbedMark(int img_id,
									   LPG_GMFR r_data, LPG_GMFR o_data, UINT8 o_count,
									   UINT32 grab_width, UINT32 grab_height);
#if 0
	VOID				SetGrabResultAngle(LPG_GMFR data,  UINT32 grab_width, UINT32 grab_height);
#endif
	LPG_ACGR			GetGrabbedMark()		{	
		return m_pstGrabResult;				}
	LPG_GMSR			GetGrabbedMarkAll()		{	
		return m_pstResultAll;				}
	UINT8				GetGrabbedMarkCount()	{	return m_u8ResultAll;				}
	VOID				PutGrabbedMark(LPG_ACGR grab, LPG_GMSR gmsr,
									   UINT32 grab_width, UINT32 grab_height);

	BOOL				IsMarkMatched()			{	return m_bMatched;					}
	VOID				SaveGrabbedMarkToFile(UINT8 img_id, LPG_GMFR r_data);
	/* 현재 선택된 레시피 (거버)의 마크 검색 조건 값 설정 */
	VOID				SetRecipeMarkRate(DOUBLE score, DOUBLE scale);
	VOID				ResetRecipeMarkRate()	{	m_dbRecipeScale = m_dbRecipeScore = 0.0f; }

	/* 정방향 / 역방향 측정 설정 */
	VOID				SetMoveStateDirect(BOOL direct)	{	m_bStageMoveDirect	= direct;	};
	VOID				SetAlignMotionptr(AlignMotion* ptr) { alignMotionPtr = ptr; };
};


