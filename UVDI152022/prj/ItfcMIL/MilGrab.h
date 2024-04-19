#pragma once

#include "MilImage.h"

class AlignMotion;

class CMilGrab : public CMilImage
{
/* ������ & �ı��� */
public:
#ifndef _NOT_USE_MIL_
	CMilGrab(LPG_CIEA config, LPG_VDSM shmem,
			 UINT8 cam_id, /*, MIL_ID ml_dis, */MIL_ID ml_sys,ENG_ERVM run_mode);
#else
	CMilGrab(LPG_CIEA config, LPG_VDSM shmem,
			UINT8 cam_id, ENG_ERVM run_mode);
#endif
	virtual ~CMilGrab();


/* ���� �Լ� */
protected:

	AlignMotion* alignMotionPtr = nullptr;;

/* ���� ���� */
protected:

	UINT8				m_u8ResultAll;		/* �˻��� �� ���� ���� */
	BOOL				m_bMatched;			/* Matching ���� */
	BOOL				m_bStageMoveDirect;	/* �������� �̵� ���� (������ / ������) */

	/* �Ź��� ��ġ�� Mark�� �߽� X / Y ��ǥ */
	DOUBLE				m_dbGerbMarkX;
	DOUBLE				m_dbGerbMarkY;
//	DOUBLE				m_dbScoreRate;
	DOUBLE				m_dbRadiusAngle;

	/* ���� ���õ� �������� Mark �˻��ϴµ� �ʿ��� ���� */
	DOUBLE				m_dbRecipeScore;
	DOUBLE				m_dbRecipeScale;

	LPG_GMSR			m_pstResultAll;		/* ��Ÿ ��� ���� (�˻��� Mark ������ �׸��� ����) */
	LPG_ACGR			m_pstGrabResult;	/* ���� ��� ���� */


/* ���� �Լ� */
protected:

	VOID				PutGrabReset();
	BOOL				PutGrabImage(PUINT8 image);

/* ���� �Լ� */
public:

	BOOL				PutGrabImage(PUINT8 image, UINT8 img_id,
									 DOUBLE mark_x=0.0f, DOUBLE mark_y=0.0f);

	/* �˻� ��� �� ��ȯ */
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
	/* ���� ���õ� ������ (�Ź�)�� ��ũ �˻� ���� �� ���� */
	VOID				SetRecipeMarkRate(DOUBLE score, DOUBLE scale);
	VOID				ResetRecipeMarkRate()	{	m_dbRecipeScale = m_dbRecipeScore = 0.0f; }

	/* ������ / ������ ���� ���� */
	VOID				SetMoveStateDirect(BOOL direct)	{	m_bStageMoveDirect	= direct;	};
	VOID				SetAlignMotionptr(AlignMotion* ptr) { alignMotionPtr = ptr; };
};


