
#pragma once

#include "../../inc/comn/ThinThread.h"

class CCamMain;

class CCamThread : public CThinThread
{
// 생성자/파괴자
public:

	CCamThread(LPG_DLSM link, CCamMain *cam_main);
	virtual ~CCamThread();

// 가상함수 재정의
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();

// 구조체 변수
protected:


// 로컬 변수
protected:

	UINT8				m_u8MatchingRotate;

	DOUBLE				m_dbMatchingRate;	/* 검색 매칭률 (단위: 퍼센트) */

	UINT64				m_u64AttachTime;
	UINT64				m_u64LiveTime;		/* 특정 주기마다 1 Frame 씩 Capture 하기 위함 */

	LPG_ACGR			m_pstGrabbed;		/* Grabbed Mark Image */
	LPG_ACLR			m_pstGrabACam;		/* 가장 최근에 얼라인 카메라 별 Grabbed 결과에 대한 정보 임시 저장 */
	LPG_DLSM			m_pstLink;			/* 카메라 연결 상태 */

	CMySection			m_syncGrab;			/* Grabbed Image의 동기화 객체 */
	CAtlList <LPG_ACGR>	m_lstGrab;			/* Align Mark Image */
	CCamMain			*m_pCamMain;


// 로컬 함수
protected:

	VOID				ProcGrabbedImage();
	VOID				SetError(TCHAR *mesg);
	VOID				UpdateLinkTime(UINT8 flag);


// 공용 함수
public:

	VOID				Reconnected();
	// 연결 여부
	BOOL				IsConnected();
	VOID				ResetGrabbedImage();
	BOOL				IsGrabbedMarkValidAll();

	// Grabbed Image Result
	UINT16				GetGrabbedCount();
	BOOL				IsScoreValidAll();
	LPG_ACGR			GetGrabbedMark(UINT8 img_id);
	BOOL				SetGrabbedMark(LPG_ACGR grab);
	LPG_ACGR			GetGrabbedMarkIndex(UINT8 index);
	BOOL				SetGrabbedMarkIndex(UINT8 index, LPG_ACGR data);

	VOID				SetMatchParam(UINT8 rotate, DOUBLE match);
	LPG_ACGR			RunModelCali(UINT8 mode=0xff);
	BOOL				RunModelStep(UINT16 counts, BOOL angle, LPG_ACGR results);
	BOOL				RunModelExam(DOUBLE score, DOUBLE scale, LPG_ACGR results);
	BOOL				RunEdgeDetect(UINT8 cam_id, UINT8 saved);

	/* 가장 최근에 갱신된 매칭된 결과 값 반환 */
	LPG_ACGR			GetLastGrabbedMark()	{	return m_pstGrabbed;	};
	BOOL				SetGrabbedMark(LPG_ACGR grab, LPG_GMFR gmfr, LPG_GMSR gmsr);
	LPG_ACLR			GetLastGrabbedACam()	{	return m_pstGrabACam;	};

	/* 카메라 내부에 Single Grabbed Image 저장할 수 있도록 처리 */
	BOOL				RunGrabbedImage();
};
