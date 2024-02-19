
#pragma once

#include "../../inc/comn/ThinThread.h"

class CCamMain;

class CCamThread : public CThinThread
{
// ������/�ı���
public:

	CCamThread(LPG_DLSM link, CCamMain *cam_main);
	virtual ~CCamThread();

// �����Լ� ������
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();

// ����ü ����
protected:


// ���� ����
protected:

	UINT8				m_u8MatchingRotate;

	DOUBLE				m_dbMatchingRate;	/* �˻� ��Ī�� (����: �ۼ�Ʈ) */

	UINT64				m_u64AttachTime;
	UINT64				m_u64LiveTime;		/* Ư�� �ֱ⸶�� 1 Frame �� Capture �ϱ� ���� */

	LPG_ACGR			m_pstGrabbed;		/* Grabbed Mark Image */
	LPG_ACLR			m_pstGrabACam;		/* ���� �ֱٿ� ����� ī�޶� �� Grabbed ����� ���� ���� �ӽ� ���� */
	LPG_DLSM			m_pstLink;			/* ī�޶� ���� ���� */

	CMySection			m_syncGrab;			/* Grabbed Image�� ����ȭ ��ü */
	CAtlList <LPG_ACGR>	m_lstGrab;			/* Align Mark Image */
	CCamMain			*m_pCamMain;


// ���� �Լ�
protected:

	VOID				ProcGrabbedImage();
	VOID				SetError(TCHAR *mesg);
	VOID				UpdateLinkTime(UINT8 flag);


// ���� �Լ�
public:

	VOID				Reconnected();
	// ���� ����
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

	/* ���� �ֱٿ� ���ŵ� ��Ī�� ��� �� ��ȯ */
	LPG_ACGR			GetLastGrabbedMark()	{	return m_pstGrabbed;	};
	BOOL				SetGrabbedMark(LPG_ACGR grab, LPG_GMFR gmfr, LPG_GMSR gmsr);
	LPG_ACLR			GetLastGrabbedACam()	{	return m_pstGrabACam;	};

	/* ī�޶� ���ο� Single Grabbed Image ������ �� �ֵ��� ó�� */
	BOOL				RunGrabbedImage();
};
