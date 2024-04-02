#pragma once

#include "MilImage.h"
#include "stdafx.h"

class CMilModel : public CMilImage
{
/* ������ & �ı��� */
public:
#ifndef _NOT_USE_MIL_
	CMilModel(LPG_CIEA config, LPG_VDSM sheme,
			  UINT8 cam_id, /*, MIL_ID ml_dis, */MIL_ID ml_sys,ENG_ERVM run_mode);
#else
	CMilModel(LPG_CIEA config, LPG_VDSM sheme,
		UINT8 cam_id, ENG_ERVM run_mode);
#endif
	virtual ~CMilModel();

public:
	UINT8 GetMarkFindSetCount() {return m_u8MarkFindSet;}
	UINT8 GetMarkFindedCount() { return m_u8MarkFindGet; }


/* ���� �Լ� */
protected:



/* ���� ���� */
protected:

	UINT8				m_u8MarkFindSet;	/* �˻��� ��ũ ���� */
	UINT8				m_u8MarkFindGet;	/* �˻��� ��ũ ���� */
	UINT8				m_u8ModelRegist;	/* �˻� ����� �� ��� ���� */

	INT32				m_i32EdgeFindGet;	/* �˻��� Edge Result ���� */

	/* Mark �� MMF ������ ���, ��ũ ũ�� �� (����: um) */
	UINT32				m_u32MMFMarkWidth;
	UINT32				m_u32MMFMarkHeight;

#ifndef _NOT_USE_MIL_
	MIL_ID				m_mlEdgeID;
	MIL_ID				m_mlModelID[3];		/* �˻��ϰ��� �ϴ� �� ID - Global, Local, TMP */
	MIL_ID				m_mlPATID[3];		/* �˻��ϰ��� �ϴ� �� ID - Global, Local, TMP */
	MIL_ID				m_mlModelID_D;		/* Draw�� ����ϴ� MMF ID */ // lk91 �ʿ��Ѱ�?
	MIL_ID				m_mlPATID_D;		/* Draw�� ����ϴ� PAT ID */
	MIL_ID				m_mlLineID;			/* Line Detection ID */
	MIL_ID				m_mlStripID;		/* Strip Marker ID */
	//MIL_ID				m_mIMergeMark;		// Merge Mark�� ���
#endif

	ENG_MMSM			m_enMarkMethod;

	STG_GMFR			m_stModResult;		/* ���� ��ũ ����� �� �˻� ��� �� �ӽ� ���� */
	LPG_DBXY			m_pstModelSize;		/* ��ϵ� �� ũ�� �� �ӽ� ���� */
	LPG_GMFR			m_pstModResult;		/* �˻� ��� ���� */
	LPG_CMPV			m_pstMarkModel;		/* �˻� ����� Mark Model�� ����� ����ü ������ */ 

	/* Grabbed Image �ӽ� ���� ������ */
	UINT8				**m_pGrabMarkImage;
	UINT8				*m_pGrabEdgeImage;
	UINT8				**m_pGrabLineImage;

	DOUBLE				m_dbRecipeScore, m_dbRecipeScale;

	INT64				*m_pMilIndex;
	DOUBLE				*m_pFindPosX;
	DOUBLE				*m_pFindPosY;
	DOUBLE				*m_pFindAngle;
	DOUBLE				*m_pFindScore;
	DOUBLE				*m_pFindScale;		/* MIN:0.0 ~ MAX:2.0 �� */
	DOUBLE				*m_pFindDist;		/* �̹����� �߽ɿ��� �˻��� ��ũ �߽ɱ��� �Ÿ� (����: pixel) */
	DOUBLE				*m_pFindCovg;		/* Target Coverage�μ�, �������� �� ���� ���� �� */
	DOUBLE				*m_pFindFitErr;		/* �� ������ �˻��� ���� ������ 2���� ���� ��. 0.0 ���� ������ ��ġ */
	DOUBLE				*m_pFindCircleRadius;		

	UINT8					m_camid;
	UINT8				m_findMode[3]; // 0 : MOD, 1 : PAT

	CPoint um_iNewOffP[3];
	CPoint um_iNewSizeP[3];
	CPoint um_iTmpOffP[3];
	CPoint um_iTmpSizeP[3];

/* ���� �Լ� */
protected:

	DOUBLE				CalcAngleDist(DOUBLE find, DOUBLE input);
	DOUBLE				CalcLineDist(DOUBLE x1, DOUBLE y1, DOUBLE x2, DOUBLE y2);

	VOID				GetFindRotatePosXY(DOUBLE cent_x, DOUBLE cent_y,
										   DOUBLE &find_x, DOUBLE &find_y);
	LPG_GMFR			GetFindMarkBestScale();
	LPG_GMFR			GetFindMarkCentSide();
	BOOL				CalcMarkCentSide(STG_DBXY &cent, DOUBLE &rate, STG_DBXY &dist);
	BOOL				CalcMarkMultiOnly(STG_DBXY &cent, DOUBLE &rate);
	BOOL				CalcMarkRingCircle(STG_DBXY &cent, STG_DBXY &dist);

	VOID				CreateMarkResult();
	VOID				ResetMarkResult();
	VOID				ReleaseMarkResult();

	BOOL				SetModelDefine(UINT8 speed, UINT8 level, UINT8 count, int markIdx,DOUBLE smooth=0.0f,
									   DOUBLE scale_min=0.0f, DOUBLE scale_max=0.0f,
									   DOUBLE score_min=0.0f,DOUBLE score_tgt=0.0f);

	DOUBLE				GetWeightRateToErrLen(DOUBLE hole, DOUBLE laser);
	/* Line Marker */
	VOID				CalcPolyWeightCent(DOUBLE &cent_x, DOUBLE &cent_y, DOUBLE &area);

#ifndef _NOT_USE_MIL_
	VOID				SetLineMarkerSetup(MIL_ID mark_id, MIL_ID grab_id, INT32 scan_width, INT32 scan_height, INT32 cent_x, INT32 cent_y);
	VOID				DrawLineMarkerResult(MIL_ID marker_id, MIL_ID grab_id);
	BOOL				GetMeasLineResult(MIL_ID mark_id, MIL_ID grab_id, INT32 cent_x, INT32 cent_y);
	BOOL				SetStripMakerSetup(MIL_ID strip_id, LPG_MSMP param);
#endif

/* ���� �Լ� */
public:

	/* Align Mark �˻� ��� ���� */
	VOID				SetMarkMethod(ENG_MMSM method, UINT8 count=0x01);
	ENG_MMSM			GetMarkMethod()					{	return m_enMarkMethod;				};
	/* ���� �� �̹��� ���� */
	BOOL				SetModelDefine(UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, PUINT32 model,
									   DOUBLE *param1, DOUBLE *param2, DOUBLE *param3,
									   DOUBLE *param4, DOUBLE *param5, UINT8 mark_no,
									   DOUBLE scale_min=0.0f, DOUBLE scale_max=0.0f,
									   DOUBLE score_min=0.0f, DOUBLE score_tgt=0.0f);

	BOOL				SetModelDefine(UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, LPG_CMPV model,
									   UINT8 mark_no, DOUBLE scale_min=0.0f, DOUBLE scale_max=0.0f,
									   DOUBLE score_min=0.0f, DOUBLE score_tgt=0.0f);

	BOOL				SetModelDefineLoad(UINT8 speed, UINT8 level, DOUBLE smooth,
										   DOUBLE scale_min, DOUBLE scale_max,
										   DOUBLE score_min, DOUBLE score_tgt,
										   PTCHAR name, CStringArray &file);

	BOOL				SetModelDefineMMF(PTCHAR name, PTCHAR mmf, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no);
	BOOL				SetModelDefinePAT(PTCHAR name, PTCHAR pat, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no);

	BOOL				SetModelDefine_tot(UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, UINT8 mark_no, TCHAR* file,
							DOUBLE scale_min = 0.0f, DOUBLE scale_max = 0.0f,
							DOUBLE score_min = 0.0f, DOUBLE score_tgt = 0.0f);
	BOOL				SetModelDefine_tot(UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, LPG_CMPV value,
							UINT8 fi_No, TCHAR* file, DOUBLE scale_min = 0.0f, DOUBLE scale_max = 0.0f,
							DOUBLE score_min = 0.0f, DOUBLE score_tgt = 0.0f);
	BOOL				MergeMark(LPG_CMPV value, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, UINT8 mark_no, TCHAR* file1, TCHAR* file2,
							TCHAR* RecipeName, DOUBLE scale_min = 0.0f, DOUBLE scale_max = 0.0f,
							DOUBLE score_min = 0.0f, DOUBLE score_tgt = 0.0f);

	/* ��ϵ� Mark Model ���� ��ȯ */
	LPG_CMPV			GetModelDefine()				{	return m_pstMarkModel;				};
	UINT8				GetModelRegistCount()			{	return m_u8ModelRegist;				}
	UINT8				GetFindMarkAllCount()			
	{	
		return m_u8MarkFindGet;				
	}
	/* ȸ���� ���� �̹��� �˻� */

	INT32				GetEdgeDetectCount() { return m_i32EdgeFindGet; };
#ifndef _NOT_USE_MIL_
	BOOL				RunEdgeDetect(MIL_ID grab_id, UINT32 width, UINT32 height, UINT8 saved);
	BOOL				RunModelFind(MIL_ID graph_id, MIL_ID grab_id, BOOL angle, UINT8 img_id, UINT8 dlg_id, UINT8 mark_no, BOOL useMilDisp, UINT8 img_proc);
	BOOL				RunModelFind(MIL_ID graph_id, MIL_ID grab_id, BOOL angle);
	BOOL				RunModel_VisionCalib(MIL_ID graph_id, MIL_ID grab_id, UINT8 img_id, UINT8 dlg_id, UINT8 mark_no, int* roi_left, int* roi_right, int* roi_top, int* roi_bottom, int row, int col);
	
	BOOL				RunPATFind(MIL_ID graph_id, MIL_ID grab_id, BOOL angle, UINT8 img_id, UINT8 dlg_id, UINT8 mark_no, BOOL useMilDisp);
	BOOL				RunLineCentXY(MIL_ID grab_id, BOOL mil_result=FALSE);				/* Edge Detection ���� ��, ����(���)�� ���� �� ��輱�� �߽� X/Y ��ǥ ��ȯ */
	BOOL				RunMarkerStrip(MIL_ID graph_id, MIL_ID grab_id, LPG_MSMP param, STG_MSMR &result);
	
	/* ���� ��Ī ��� �� ID ��ȯ */
	MIL_ID				GetBufEdgeID()					{	return m_mlEdgeID;					};
	MIL_ID				GetBufLineID()					{	return m_mlLineID;					};
	MIL_ID				GetBufStripID()					{	return m_mlStripID;					};
#endif

	/* �˻� ��� ��ȯ */
	LPG_GMFR			GetFindMark(UINT8 index)		{	return &m_pstModResult[index];		};
	LPG_GMFR			GetFindMarkAll()				
	{	
		return m_pstModResult;				
	};

	LPG_GMFR			GetFindMark();
	LPG_GMFR			GetFindMarkResultCentSide()		{	return &m_stModResult;				};
	UINT32				GetModelWidth(UINT8 index);
	UINT32				GetModelHeight(UINT8 index);
	/* Mark Pattern ��� ���� */
#ifndef _NOT_USE_MIL_
	BOOL				IsSetMarkModel(UINT8 fi_No)				{	return (m_mlModelID[fi_No] != NULL || m_mlPATID[fi_No] != NULL);			}
#endif
	BOOL				IsModelResult(UINT32 count)		{	return m_u8MarkFindGet == count;	}
	BOOL				IsFindModCount(UINT8 find, DOUBLE score, DOUBLE scale);
	VOID				ReleaseMarkModel(UINT8 index, UINT8 findMode);
	DOUBLE				GetMarkModelSize(UINT8 index, UINT8 flag, UINT8 unit);
	UINT32				GetMarkModelType(UINT8 index=0);
	VOID				ResetEdgeObject();

	VOID				SetRecipeMarkRate(DOUBLE score, DOUBLE scale);
	VOID				SetMultiMarkArea(UINT32 width, UINT32 height);
	BOOL				RegistPat(PUINT8 img_src, CRect fi_rectArea, CString fi_filename, UINT8 mark_no);
	BOOL				RegistMod(PUINT8 img_src, CRect fi_rectArea, CString fi_filename, UINT8 mark_no);
	VOID				InitSetMarkSizeOffset(UINT8 cam_id, TCHAR* file, UINT8 fi_findType, UINT8 mark_no);
	VOID				PutMarkDisp(HWND hwnd, int fi_iNo, RECT draw, UINT8 cam_id, TCHAR* file, int fi_findType);
	BOOL				PutSetMarkDisp(HWND hwnd, RECT draw, UINT8 cam_id, TCHAR* file, int fi_findType, DOUBLE fi_dRate);
	CPoint GetMarkSize(int fi_No);
	CPoint GetMarkOffset(BOOL bNewOffset, int fi_No);
	UINT8 GetMarkFindMode(UINT8 mark_no);
	BOOL SetMarkFindMode(UINT8 find_mode, UINT8 mark_no);

	VOID SetMarkSize(CPoint fi_MarkSize, int setMode, int fi_No);
	VOID SetMarkOffset(CPoint fi_MarkCenter, int setOffsetMode, int fi_No);
	VOID MaskClear_MOD(CPoint fi_iSizeP, UINT8 mark_no);
	VOID MaskClear_PAT(CPoint fi_iSizeP, UINT8 mark_no);
	VOID CenterFind(int fi_length, int fi_curSmoothness, double* fi_NumEdgeMIN_X, double* fi_NumEdgeMAX_X, double* fi_NumEdgeMIN_Y, double* fi_NumEdgeMAX_Y, int* fi_NumEdgeFound, int fi_Mode);

	VOID SaveMask_MOD(UINT8 mark_no);
	VOID SaveMask_PAT(UINT8 mark_no);

	VOID PatMarkSave(CString fi_strFileName, UINT8 mark_no);
	VOID ModMarkSave(CString fi_strFileName, UINT8 mark_no);

	VOID	DrawMarkInfo_UseMIL(UINT8 fi_smooth, UINT8 mark_no);
	VOID	Mask_MarkSet(CRect rectTmp, CPoint iTmpSizeP, CRect rectFill, int fi_color, bool bMask);
	//VOID	CloseMilModel();
};
