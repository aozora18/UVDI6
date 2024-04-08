
#pragma once

#include "../../inc/comn/ThinThread.h"


class CCamMain;
class AlignMotion;

class CCamThread : public CThinThread
{
// 생성자/파괴자
public:

	CCamThread(UINT8 cam_count, UINT8 max_grab, STG_DLSM **link, CCamMain **cam_main, UINT8 img_proc);
	virtual ~CCamThread();

// 가상함수 재정의
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();
	AlignMotion*		alignMotionPtr = nullptr;

// 구조체 변수
protected:


// 로컬 변수
protected:

	UINT8				m_u8CamCount;		/* 총 카메라 개수 */
	UINT8				m_u8CamNext;		/* 현재 작업 중인 카메라 Number (0 ~ 카메라 대수-1) */
	UINT8				m_u8MatchingRotate;
	UINT8				m_u8SizeMul;		/* 이미지 템플릿 매칭에 사용될 크기 배율 값 */
	UINT8				m_u8MaxGrab;		/* 카메라 마다 한 번의 동작으로 캡처할 수 있는 최대 이미지 개수 */
	DOUBLE				m_dbMatchingRate;	/* 검색 매칭률 (단위: 퍼센트) */
	UINT8				m_u8ImgProc;		/* Image Processing 여부 체크 */

	UINT64				m_u64AttachTime;

	LPG_ACGR			m_pstGrabbed;		/* Grabbed Mark Image */ 
	LPG_ACLR			m_pstGrabACam;		/* 가장 최근에 얼라인 카메라 별 Grabbed 결과에 대한 정보 임시 저장 */
	STG_DLSM			**m_pstLink;		/* 카메라 연결 상태 정보 */

	CMySection			m_syncGrab;			/* Grabbed Image의 동기화 객체 */
	CAtlList <LPG_ACGR>	m_lstGrab;			/* Align Mark Image */
	CCamMain			**m_pCamMain;


// 로컬 함수
protected:

	VOID				ProcGrabbedImage(UINT8 cam_id, UINT8 dispType, UINT8 img_proc);
	VOID				UpdateLinkTime(UINT8 cam_id, UINT8 flag);


// 공용 함수
public:

	void SetAlignMotionPtr(AlignMotion& ptr)
	{
		alignMotionPtr = &ptr;
	}

	CAtlList <LPG_ACGR>* GetGrabImage()
	{
		return &m_lstGrab;
	}

	bool TryEnterCS() { return m_syncGrab.Enter(); }
	void ExitCS() { m_syncGrab.Leave(); }



	VOID				Reconnected();
	// 연결 여부
	BOOL				IsConnected(UINT8 cam_id);
	VOID				ResetGrabbedImage();
	BOOL				IsGrabbedMarkValidAll(BOOL multi_mark, DOUBLE set_score, int* camNum=nullptr);

	// Grabbed Image Result
	UINT16				GetGrabbedCount(int* camNum=nullptr);
	BOOL				IsScoreValidAll(DOUBLE set_score);
	LPG_ACGR			GetGrabbedMark(UINT8 cam_id, UINT8 img_id);
	BOOL				SetGrabbedMark(LPG_ACGR grab);
	LPG_ACGR			GetGrabbedMarkIndex(UINT8 index);
	BOOL				SetGrabbedMarkIndex(UINT8 index, LPG_ACGR data);

	VOID				SetMatchParam(UINT8 rotate, DOUBLE match);
	CAtlList <LPG_ACGR>* GetGrabbedMarkAll();
	LPG_ACGR			RunModelFind(UINT8 cam_id, UINT8 mode, UINT8 dlg_id, UINT8 mark_no, BOOL useMilDisp, UINT8 img_proc, int flipDir=-1); // default mode = 0xff
	BOOL				RunModelStep(UINT8 cam_id, UINT16 count, BOOL angle, LPG_ACGR results, UINT8 dlg_id, UINT8 mark_no, UINT8 img_proc);
	BOOL				RunModelExam(DOUBLE score, DOUBLE scale, LPG_ACGR results);
	BOOL				RunEdgeDetect(UINT8 cam_id, UINT8 saved);
	BOOL				RunMarkerStrip(UINT8 cam_id, LPG_MSMP param, STG_MSMR &results);

	VOID				SetMatchSizeMul(UINT8 size_mul)	{	m_u8SizeMul	= size_mul;	};

	/* 가장 최근에 갱신된 매칭된 결과 값 반환 */
	LPG_ACGR			GetLastGrabbedMark()	{	return m_pstGrabbed;	};
	BOOL				SetGrabbedMark(LPG_ACGR grab, LPG_GMFR gmfr, LPG_GMSR gmsr);
	LPG_ACLR			GetLastGrabbedACam()	{	return m_pstGrabACam;	};

	CDPoint RunModel_VisionCalib(UINT8 cam_id, UINT8 dlg_id, UINT8 mark_no, int* roi_left, int* roi_right, int* roi_top, int* roi_bottom, int row, int col);

};
