#pragma once

#include "stdafx.h"
class CMilModel;
class CMilGrab;

class CMilMain : public CWnd
{
	DECLARE_DYNAMIC(CMilMain, CWnd)

private:
	bool terminated = false;

/* 생성자 & 파괴자 */
public:
	CMilMain();
	virtual ~CMilMain();

	VOID InitMilMain(LPG_CIEA config, LPG_VDSM shmem); 

/* 가상 함수 */
protected:


/* 로컬 변수 */
//protected: 
public:
	//CMilDisp milDisp;

#ifndef _NOT_USE_MIL_
	MIL_ID				m_mAppID;			/* 어떤 프로그램이든 하나만 존재 */
	MIL_ID				m_mSysID;			/* 프레임 그래버를 지칭하며, 이용하는 갯수에 따라 추가 가능 */
											/* 프레임 그래버가 없는 경우, 내부 로컬 시스템으로 설정 가능 */

	MIL_ID				m_mlDisID;					/* Expo 등 공통 DispID, 이전 버전 변경 없이 사용할 때*/
	MIL_ID				m_mDisID_Mark_Live;			/* Mark Live DispID*/
	MIL_ID				*m_mDisID_Mark;				/* Mark DispID*/
	MIL_ID				m_mDisID_MarkSet;			/* Mark Setting DispID*/	
	MIL_ID				m_mDisID_MMPM;				/* Manual Align DispID*/
	MIL_ID				m_mDisID_CALI_CAMSPEC;		/* Calib CamSpec DispID*/
	MIL_ID				*m_mDisID_CALI_STEP;		/* Calib Step DispID*/
	MIL_ID				m_mDisID_ACCR;		
	MIL_ID				m_mDisID_MPMM_AutoCenter;		/* Manual Align Auto Center DispID*/

	MIL_ID				m_mGraphID;						/* GraphicContID */

	MIL_ID				m_mOverlay_Mark_Live;			/* Mark Live OverlayID*/
	MIL_ID				* m_mOverlay_Mark;				/* Mark OverlayID*/
	MIL_ID				m_mOverlay_MarkSet;				/* Mark Setting OverlayID*/
	MIL_ID				m_mOverlay_MMPM;				/* Manual Align OverlayID*/
	MIL_ID				m_mOverlay_CALI_CAMSPEC;		/* Calib CamSpec OverlayID*/
	MIL_ID				* m_mOverlay_CALI_STEP;			/* Calib Step OverlayID*/
	MIL_ID				m_mOverlay_ACCR;		
	MIL_ID				m_mOverlay_MPMM_AutoCenter;		/* Manual Align Auto Center OverlayID*/

	MIL_ID				m_mImgDisp_Mark_Live;			/* Mark Live DispImgID*/
	MIL_ID				* m_mImgDisp_Mark;				/* Mark DispImgID*/
	MIL_ID				m_mImgDisp_MarkSet;				/* Mark Setting DispImgID*/
	MIL_ID				m_mImgDisp_MMPM;				/* Manual Align DispImgID*/
	MIL_ID				m_mImgDisp_CALI_CAMSPEC;		/* Calib CamSpec DispImgID*/
	MIL_ID				* m_mImgDisp_CALI_STEP;			/* Calib Step DispImgID*/
	MIL_ID				m_mImgDisp_ACCR;				/* ACCURACY MEASURE DispImgID*/
	MIL_ID				m_mImgDisp_MPMM_AutoCenter;		/* Manual Align Auto Center DispImgID*/

	MIL_ID				*m_mImg;						/* Cam Live ImgID 원본*/
	MIL_ID				*m_mImg_Grab;					/* Cam Grab ImgID 원본*/
	MIL_ID				*m_mImg_Mark;					/* Mark ImgID*/
	MIL_ID				m_mImg_MarkSet;					/* Mark Setting ImgID*/
	MIL_ID				m_mImg_MarkSet_Ori;				/* Mark Setting에서 Edge Find에 사용 (Auto Center) */ 
	MIL_ID				*m_mImg_CALI_CAMSPEC;			/* Calib CamSpec ImgID*/
	MIL_ID				*m_mImg_CALI_STEP;				/* Calib Step ImgID*/
	MIL_ID				*m_mImg_ACCR;			
	MIL_ID				*m_mImgProc;					/* Imag 변환 */
	MIL_ID				m_mImg_MPMM_AutoCenter;			/* Manual Align Auto Center ImgID*/
	MIL_ID				m_mImg_MPMM_AutoCenter_Proc;

	MIL_ID				*m_mDisID_EXPO;
	MIL_ID				*m_mImgDisp_EXPO;
	MIL_ID				*m_mOverlay_EXPO;

	CPoint	m_MarkSize;
	//--- Mask 관련
	MIL_ID	MilMask;
	HANDLE	hMaskBuf;
	unsigned char* pucMaskBuf;
	//--- Pat, Mod
	//MIL_ID  MilEdgeResult[GRABMARK_CNT];
	MIL_ID  *MilEdgeResult;
	MIL_ID	MilEdgeContext;
	MIL_ID	pCopy;
	MIL_ID	mCopy;
#endif
	LPG_CIEA			m_pstConfig;		/* 전체 환경 정보 */

	LPG_VDSM			m_pstShMemVisi;		/* Vision 공유 메모리 영역 */

	CMilGrab			*m_pLastGrabResult;	/* 가장 최근에 Grabbed Image의 매칭 및 검색 결과 객체 연결 */
	CMilGrab			***m_pMilGrab;		/* 검색 (Grabbed)된 Mark Image 정보를 저장 및 관리하는 객체 */
	CMilModel			** m_pMilModel;		/* 패턴 (Mark) 이미지 저장 및 검색, CAM 별 */

	CRect				*rectSearhROI; // Search ROI 
	CRect				rectSearhROI_Calb_CamSpec; // CamSpec Search ROI 
	CRect				rectSearhROI_Calb_Step; // Step Search ROI 
	CRect				rectSearhROI_Calb_Accr; // ROI 
	CRect				rectSearhROI_MPMM_AutoCenter; // ROI 

/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	BOOL				InitMilAlloc(ENG_ERVM run_mode, PTCHAR font_name=L"tahoma", UINT8 font_size=20);	/* MIL 관련 객체 초기화 */
	VOID				CloseMilAlloc();					/* MIL 관련 객체 해제 */

	/* Align Mark 검색 방식 설정 */
	VOID				SetMarkMethod(ENG_MMSM method, UINT8 count);
	ENG_MMSM			GetMarkMethod(UINT8 cam_id);

	VOID				ResetGrabAll();

	/* X 좌표 기준으로 정렬 */
	UINT8				GetModelRegistCount(UINT8 cam_id);
	BOOL				RunModelFind(UINT8 cam_id, UINT8 img_id, PUINT8 image, UINT8 dlg_id, UINT8 mark_no, BOOL useMilDisp, UINT8 img_proc);
	BOOL				RunModelFind(UINT8 cam_id, PUINT8 image, BOOL angle, UINT8 img_id, UINT8 dlg_id, UINT8 mark_no, BOOL useMilDisp, UINT8 img_proc);
	BOOL				RunModelStep(UINT8 cam_id, PUINT8 image, UINT32 width, UINT32 height,
									 UINT16 count, BOOL angle, LPG_ACGR results, UINT8 img_id, UINT8 dlg_id, UINT8 mark_no, UINT8 img_proc);
	BOOL				RunModelExam(PUINT8 image, UINT32 width, UINT32 height,
									 DOUBLE score, DOUBLE scale, LPG_ACGR results, UINT8 img_id, UINT8 dlg_id, UINT8 mark_no); // 미사용
	BOOL				RunEdgeDetect(UINT8 cam_id, LPG_ACGR grab, UINT8 saved);
	BOOL				RunMarkerStrip(UINT8 cam_id, PUINT8 image, LPG_MSMP param, STG_MSMR &results);
	/* Edge Detection 검출 즉, 윤곽(경계)선 검출 후 경계선의 중심 X/Y 좌표 반환 */
	BOOL				RunLineCentXY(UINT8 cam_id, LPG_ACGR grab_data, BOOL mil_result=FALSE);

	BOOL				SetModelDefine(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
									   LPG_CMPV model, UINT8 mark_no,
									   DOUBLE scale_min, DOUBLE scale_max,
									   DOUBLE score_min, DOUBLE score_tgt);
	BOOL				SetModelDefine(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
									   PUINT32 model, DOUBLE *param1, DOUBLE *param2, DOUBLE *param3,
									   DOUBLE *param4, DOUBLE *param5, UINT8 mark_no,
									   DOUBLE scale_min, DOUBLE scale_max,
									   DOUBLE score_min, DOUBLE score_tgt);
	BOOL				SetModelDefineLoad(UINT8 cam_id, UINT8 speed, UINT8 level, DOUBLE smooth,
										   DOUBLE scale_min, DOUBLE scale_max,
										   DOUBLE score_min, DOUBLE score_tgt,
										   PTCHAR name, CStringArray &file);
	BOOL				SetModelDefine_tot(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
										LPG_CMPV model, UINT8 fi_No, TCHAR* file,
										DOUBLE scale_min, DOUBLE scale_max,
										DOUBLE score_min, DOUBLE score_tgt);
	BOOL				MergeMark(UINT8 cam_id, LPG_CMPV value, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, UINT8 mark_no, TCHAR* file1, TCHAR* file2,
		TCHAR* RecipeName, DOUBLE scale_min, DOUBLE scale_max, DOUBLE score_min, DOUBLE score_tgt);

	BOOL				SetModelDefineMMF(UINT8 cam_id, PTCHAR name, PTCHAR mmf, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no);
	BOOL				SetModelDefinePAT(UINT8 cam_id, PTCHAR name, PTCHAR pat, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no);
	LPG_CMPV			GetModelDefine(UINT8 cam_id);
	/* 가장 최근에 Align and Calibration Grabbed Image의 패턴 매칭 검색 결과 구조체 포인터 반환 */
	LPG_ACGR			GetLastGrabbedMark();
	LPG_ACGR			GetGrabbedMark(UINT8 cam_id=0x01, UINT8 img_id=0x00);
	LPG_GMSR			GetGrabbedMarkAll(UINT8 cam_id, UINT8 img_id=0x00);
	UINT8				GetGrabbedMarkCount(UINT8 cam_id, UINT8 img_id);
#ifndef _NOT_USE_MIL_
	MIL_ID				GetMilSysID()	{	return m_mSysID;	};
	MIL_ID				GetBufID(UINT8 cam_id, UINT8 type);
	MIL_ID				GetMarkBufID(UINT8 cam_id, UINT8 img_id);
	MIL_ID				GetDispID(UINT8 cam_id);  
#endif
	/* Mark Pattern 등록 여부 */
	BOOL				IsSetMarkModel(UINT8 mode, UINT8 cam_id=0x01, UINT8 fi_No = 0x00);
	BOOL				IsSetMarkModelACam(UINT8 cam_id, UINT8 mark_no);
	VOID				ResetMarkModel();
	UINT32				GetMarkModelType(UINT8 cam_id, UINT8 index=0x00);
	DOUBLE				GetMarkModelSize(UINT8 cam_id, UINT8 index, UINT8 flag, UINT8 unit);
	/* Grabbed Image 저장 */
	BOOL				SaveGrabbedMarkToFile(UINT8 cam_id, LPRECT area, UINT8 type, TCHAR *file);
	/* Edge Detection 검색 결과 반환 */
	LPG_EDFR			GetEdgeDetectResults(UINT8 cam_id)	{	return m_pstShMemVisi->edge_result[cam_id-1];	};
	INT32				GetEdgeDetectCount(UINT8 cam_id);

	/* 강제로 입력된 마크 검색 값 처리 */
	BOOL				SetGrabbedMark(LPG_ACGR grab, LPG_GMFR gmfr, LPG_GMSR gmsr);
	VOID				ImageRotate(PUINT8 img_src, PUINT8 img_dst, UINT32 width, UINT32 height,
									DOUBLE angle, UINT32 rotate_x, UINT32 rotate_y);
	VOID				ImageFlip(PUINT8 img_src, PUINT8 img_dst, UINT32 width, UINT32 height, UINT8 flag);
	/* License Check */
	BOOL				IsLicenseValid();
	/* !!! 노광 모드 설정 : 직접 노광, 얼라인 노광, 보정 후 얼라인 노광 !!!*/
	VOID				SetAlignMode(ENG_AOEM mode);

	/* Live 이미지 혹은 Grabbed Image를 특정 파일에 저장 */
	BOOL				SaveGrabbedToFile(PTCHAR file, INT32 width, INT32 height, PUINT8 image);
	/* 현재 선택된 레시피 (거버)의 마크 검색 조건 값 설정 */
	VOID				SetRecipeMarkRate(DOUBLE score, DOUBLE scale);
	/* 스테이지의 이동 방향 설정 */
	VOID				SetMoveStateDirect(BOOL direct);

	/* MMF 검색 방식일 경우, 얼라인 마크 경계선을 그리기 위함 */
	VOID				SetMultiMarkArea(UINT32 width, UINT32 height);
	VOID				MilSetMarkROI(UINT8 cam_id, CRect fi_rectSearhROI);
	/* Regist Pat */
	BOOL				RegistPat(UINT8 cam_id, PUINT8 img_src, CRect fi_rectArea, CString fi_filename, UINT8 mark_no);
	BOOL				RegistMod(UINT8 cam_id, PUINT8 img_src, CRect fi_rectArea, CString fi_filename, UINT8 mark_no);

	VOID				InitSetMarkSizeOffset(UINT8 cam_id, TCHAR* file, UINT8 fi_findType, UINT8 fi_No);
	VOID				PutMarkDisp(HWND hwnd, int fi_iNo, RECT draw, UINT8 cam_id, TCHAR* file, int fi_findType);
	BOOL				PutSetMarkDisp(HWND hwnd, RECT draw, UINT8 cam_id, TCHAR* file, int fi_findType, DOUBLE fi_dRate);
	DOUBLE SetMarkSetDispRate(DOUBLE fi_dRate);
	DOUBLE GetMarkSetDispRate();
	CPoint GetMarkSize(UINT8 cam_id, int fi_No);
	CPoint GetMarkOffset(UINT8 cam_id, BOOL bNewOffset, int fi_No);
	BOOL CMilMain::MaskBufGet();

	UINT8 GetMarkFindMode(UINT8 cam_id, UINT8 mark_no);
	BOOL SetMarkFindMode(UINT8 cam_id, UINT8 find_mode, UINT8 mark_no);

	VOID SetMarkSize(UINT8 cam_id, CPoint fi_MarkSize, int setMode, int fi_No);
	VOID SetMarkOffset(UINT8 cam_id, CPoint fi_MarkCenter, int setOffsetMode, int fi_No);
	VOID MaskClear_MOD(UINT8 cam_id, CPoint fi_iSizeP, UINT8 mark_no);
	VOID MaskClear_PAT(UINT8 cam_id, CPoint fi_iSizeP, UINT8 mark_no);
	VOID CenterFind(int cam_id, int fi_length, int fi_curSmoothness, double* fi_NumEdgeMIN_X, double* fi_NumEdgeMAX_X, double* fi_NumEdgeMIN_Y, double* fi_NumEdgeMAX_Y, int* fi_NumEdgeFound, int fi_Mode);

	VOID SaveMask_MOD(UINT8 cam_id,UINT8 mark_no);
	VOID SaveMask_PAT(UINT8 cam_id,UINT8 mark_no);

	VOID PatMarkSave(UINT8 cam_id, CString fi_strFileName, UINT8 mark_no);
	VOID ModMarkSave(UINT8 cam_id, CString fi_strFileName, UINT8 mark_no);

	VOID DrawMarkInfo_UseMIL(UINT8 cam_id, UINT8 fi_smooth, UINT8 mark_no);
	VOID Mask_MarkSet(UINT8 cam_id, CRect rectTmp, CPoint iTmpSizeP, CRect rectFill, int fi_color, bool bMask);
	VOID SetDispExpo(CWnd* pWnd[4]);
	VOID SetDisp(CWnd** pWnd, UINT8 fi_Mode);
	VOID SetDispMark(CWnd* pWnd);
	VOID SetDispRecipeMark(CWnd* pWnd[2]);
	VOID SetDispMarkSet(CWnd* pWnd);
	VOID SetDispMMPM_AutoCenter(CWnd* pWnd);
	
	VOID SetDispMMPM(CWnd* pWnd);
	VOID InitMask(UINT8 cam_id);
	VOID CloseSetMark();
	VOID CloseMMPMAutoCenter();

	BOOL RegistMILImg(INT32 cam_id, INT32 width, INT32 height, PUINT8 image);
	BOOL RegistMILGrabImg(INT32 fi_No, INT32 width, INT32 height, PUINT8 image);
	BOOL ProcImage(UINT8 cam_id, UINT8 imgProc);
	BOOL VisionCalib(UINT8 cam_id, UINT8 img_id, PUINT8 image, UINT8 dlg_id, UINT8 mark_no, BOOL useMilDisp, UINT8 img_proc);

	int EXPO_DISP_SIZE_X;
	int EXPO_DISP_SIZE_Y;
	int MARK_DISP_SIZE_X;
	int MARK_DISP_SIZE_Y;
	int MARKSET_DISP_SIZE_X;
	int MARKSET_DISP_SIZE_Y;
	int LIVE_DISP_SIZE_X;
	int LIVE_DISP_SIZE_Y;
	int MMPM_DISP_SIZE_X;
	int MMPM_DISP_SIZE_Y;
	int RECIPE_MARK_DISP_SIZE_X;
	int RECIPE_MARK_DISP_SIZE_Y;
	int CALIB_CAMSPEC_DISP_SIZE_X;
	int CALIB_CAMSPEC_DISP_SIZE_Y;
	int CALIB_EXPO_DISP_SIZE_X;
	int CALIB_EXPO_DISP_SIZE_Y;
	int ACCR_DISP_SIZE_X;
	int ACCR_DISP_SIZE_Y;
	int MMPM_AUTOCENTER_DISP_SIZE_X;
	int MMPM_AUTOCENTER_DISP_SIZE_Y;

	double MARK_DISP_RATE;
	double MARKSET_DISP_RATE;
	double MMPM_DISP_RATE;
	double RECIPE_MARK_DISP_RATE;
	double MMPM_AUTOCENTER_DISP_RATE;

	int CAM_CNT;
	int GRABMARK_CNT;
	int MAX_MARK_GRAB; 
	COLORREF MilColorConvert(int col);
	BOOL RunModel_VisionCalib(UINT8 cam_id, UINT8 img_id, PUINT8 image, UINT8 dlg_id, UINT8 mark_no, int* roi_left, int* roi_right, int* roi_top, int* roi_bottom, int row, int col);

};
