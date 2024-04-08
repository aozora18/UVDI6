
#pragma once

#include "../../inc/comn/ThinThread.h"


class CCamMain;
class AlignMotion;

class CCamThread : public CThinThread
{
// ������/�ı���
public:

	CCamThread(UINT8 cam_count, UINT8 max_grab, STG_DLSM **link, CCamMain **cam_main, UINT8 img_proc);
	virtual ~CCamThread();

// �����Լ� ������
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();
	AlignMotion*		alignMotionPtr = nullptr;

// ����ü ����
protected:


// ���� ����
protected:

	UINT8				m_u8CamCount;		/* �� ī�޶� ���� */
	UINT8				m_u8CamNext;		/* ���� �۾� ���� ī�޶� Number (0 ~ ī�޶� ���-1) */
	UINT8				m_u8MatchingRotate;
	UINT8				m_u8SizeMul;		/* �̹��� ���ø� ��Ī�� ���� ũ�� ���� �� */
	UINT8				m_u8MaxGrab;		/* ī�޶� ���� �� ���� �������� ĸó�� �� �ִ� �ִ� �̹��� ���� */
	DOUBLE				m_dbMatchingRate;	/* �˻� ��Ī�� (����: �ۼ�Ʈ) */
	UINT8				m_u8ImgProc;		/* Image Processing ���� üũ */

	UINT64				m_u64AttachTime;

	LPG_ACGR			m_pstGrabbed;		/* Grabbed Mark Image */ 
	LPG_ACLR			m_pstGrabACam;		/* ���� �ֱٿ� ����� ī�޶� �� Grabbed ����� ���� ���� �ӽ� ���� */
	STG_DLSM			**m_pstLink;		/* ī�޶� ���� ���� ���� */

	CMySection			m_syncGrab;			/* Grabbed Image�� ����ȭ ��ü */
	CAtlList <LPG_ACGR>	m_lstGrab;			/* Align Mark Image */
	CCamMain			**m_pCamMain;


// ���� �Լ�
protected:

	VOID				ProcGrabbedImage(UINT8 cam_id, UINT8 dispType, UINT8 img_proc);
	VOID				UpdateLinkTime(UINT8 cam_id, UINT8 flag);


// ���� �Լ�
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
	// ���� ����
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

	/* ���� �ֱٿ� ���ŵ� ��Ī�� ��� �� ��ȯ */
	LPG_ACGR			GetLastGrabbedMark()	{	return m_pstGrabbed;	};
	BOOL				SetGrabbedMark(LPG_ACGR grab, LPG_GMFR gmfr, LPG_GMSR gmsr);
	LPG_ACLR			GetLastGrabbedACam()	{	return m_pstGrabACam;	};

	CDPoint RunModel_VisionCalib(UINT8 cam_id, UINT8 dlg_id, UINT8 mark_no, int* roi_left, int* roi_right, int* roi_top, int* roi_bottom, int row, int col);

};
