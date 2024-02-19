
/*
 desc : TCP/IP Client Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "CamThread.h"
#include "CamMain.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 desc : ������
 parm : link		- [in]  ī�޶� ���� ���� ����
		cam_main	- [in]  Camera Control Object
 retn : None
*/
CCamThread::CCamThread(LPG_DLSM link, CCamMain *cam_main)
{
	m_u8MatchingRotate	= 0x00;
	m_dbMatchingRate	= 80.0f;

	m_pstLink			= link;

	m_pstGrabbed		= NULL;
	/* ���� �ֱٿ� Grabbed ����� ���� ����� ī�޶� ���� �����ϱ� ���� �ӽ� ���� */
	m_pstGrabACam		= (LPG_ACLR)::Alloc(sizeof(STG_ACLR));
	ASSERT(m_pstGrabACam);
	memset(m_pstGrabACam, 0x00, sizeof(STG_ACLR));

	// ī�޶� �̺�Ʈ ��ü ����
	m_pCamMain		= cam_main;
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CCamThread::~CCamThread()
{
	// ť�� ����� Grabbed �̹��� ����
	ResetGrabbedImage();
	/* Grabbed ī�޶� �� �ӽ� ���� �޸� ���� */
	if (m_pstGrabACam)	::Free(m_pstGrabACam);
}

/*
 desc : ������ ����� �ʱ� �ѹ� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamThread::StartWork()
{
	m_u64LiveTime	= GetTickCount64();
	m_u64AttachTime	= GetTickCount64();

	return TRUE;
}

/*
 desc : ������ ����� �ֱ��� ȣ���
 parm : None
 retn : None
*/
VOID CCamThread::RunWork()
{
#if (0 == USE_EQUIPMENT_SIMULATION_MODE)
	/* ���� ī�޶� ����Ǿ� �ִ��� Ȯ�� */
	if (!m_pCamMain->IsConnected())
	{
		/* ���� ���� ���� */
		UpdateLinkTime(0x00);

		/* Ư�� �ð� ���� �ֱ������� Open ���� (5 �� ���� ����) */
		if (m_u64AttachTime+5000 < GetTickCount64())
		{
			/* ���� �������� �� �ٽ� ���� �õ� */
			m_pCamMain->AttachDevice();	/* AttachDevice ���� �Լ����� DetechDevice �Լ� ȣ���� */
			/* �ֱ� ������ ������ �ð� ���� */
			m_u64AttachTime	= GetTickCount64();
		}
	}
	else
	{
		/* ���� �Ϸ� ���� */
		UpdateLinkTime(0x01);

		// ����Ǿ� �ִٸ�, ���� Grabbed Image�� �����ϴ��� Ȯ�� ��, �˻� ����
		switch (m_pCamMain->GetCamMode())
		{
		case ENG_VCCM::en_grab_mode	:	ProcGrabbedImage();	break;
		case ENG_VCCM::en_live_mode	:
			if (GetTickCount64() > m_u64LiveTime +
				(UINT64)ROUNDUP(1000.0f / m_pCamMain->GetFramePerSecond(), 0))
			{
				m_u64LiveTime	= GetTickCount64();
				m_pCamMain->RunGrabbedImage();
			}
			break;
		}
	}
#else
	/* ���� �Ϸ� ���� (������ ����Ǿ��ٰ� ����) */
	UpdateLinkTime(0x01);
	// ����Ǿ� �ִٸ�, ���� Grabbed Image�� �����ϴ��� Ȯ�� ��, �˻� ����
	switch (m_pCamMain->GetCamMode())
	{
	case ENG_VCCM::en_grab_mode	:	ProcGrabbedImage();	break;
	case ENG_VCCM::en_live_mode	:	break;
	}
#endif
}

/*
 desc : ������ ����� �ѹ� ȣ���
 parm : None
 retn : None
*/
VOID CCamThread::EndWork()
{
}

/*
 desc : ī�޶� ���� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamThread::IsConnected()
{
	return m_pCamMain->IsConnected();
}

/*
 desc : ����� ī�޶� �κи� ������ ����
 parm : None
 retn : None
*/
VOID CCamThread::Reconnected()
{
	m_pCamMain->DetachDevice();
}

/*
 desc : ���� �߻� ó��
 parm : mesg	- [in]  ���� �޽��� (subject)
 retn : None
*/
VOID CCamThread::SetError(TCHAR *mesg)
{
	LOG_SAVED(ENG_LNWE::en_error, mesg);
}

/*
 desc : ���� Grabbed �̹��� ��� ����
 parm : None
 retn : None
*/
VOID CCamThread::ResetGrabbedImage()
{
	// ����ȭ ����
	if (m_syncGrab.Enter())
	{
		m_lstGrab.RemoveAll();
		// ����ȭ ����
		m_syncGrab.Leave();
	}
}

/*
 desc : Grabbed Image�� �����ϸ�, Align Mark ����
 parm :	None
 retn : None
*/
VOID CCamThread::ProcGrabbedImage()
{
	BOOL bFinded		= FALSE;
	LPG_ACGR pstGrab	= {NULL};

	// ����ȭ ����
	if (m_syncGrab.Enter())
	{
		// Grabbed Image�� �����ϴ� ��츸 ó��
		pstGrab	= m_pCamMain->GetGrabbedImage();

		// ���� Grabbed Image�� �����Ѵٸ� ...
		if (pstGrab && pstGrab->grab_data)
		{
			/* Model Find */
			bFinded	= uvMIL_RunModelFind(pstGrab->cam_id, pstGrab->img_id, pstGrab->grab_data);
			/* �˻� ���� Ȥ�� ���п� ��� ���� Mark �˻� ��� ���� ��� */
			pstGrab	= uvMIL_GetLastGrabbedMark();
			if (pstGrab)	pstGrab->marked	= UINT8(bFinded);
			/* �˻� ���ο� ��� ���� ���������, Ư�� ���� �̻��̸� �� �̻� ������� ���� */
			if (m_lstGrab.GetCount() < MAX_GRABBED_IMAGE)
			{
				m_lstGrab.AddTail(pstGrab);
			}
			else
			{
				uvLogs_SaveLogs(ENG_LNWE::en_warning, L"The number of images captured has been exceeded");
			}
		}
		// ����ȭ ����
		m_syncGrab.Leave();
	}
}

/*
 desc : ����ڿ� ���� �������� �˻��� ���, ��� ���� ó��
 parm : grab	- [in]  ����ڿ� ���� �������� �Էµ� grabbed image ��� ������ ����� ����ü ������
		gmfr	- [in]  GMFR Data
		gmsr	- [in]  GMSR Data
 retn : TRUE or FALSE
*/
BOOL CCamThread::SetGrabbedMark(LPG_ACGR grab, LPG_GMFR gmfr, LPG_GMSR gmsr)
{
	STG_ACGR stGrab	= {NULL};

	if (!uvMIL_SetGrabbedMark(grab, gmfr, gmsr))	return FALSE;
	/* �˻� ���� Ȥ�� ���п� ��� ���� Mark �˻� ��� ���� ��� */
	if (!uvMIL_GetLastGrabbedMark())	return FALSE;
	memcpy(&stGrab, uvMIL_GetLastGrabbedMark(), sizeof(STG_ACGR)-sizeof(PUINT8));
	stGrab.marked	= 0x01;
	/* ����� �� �ݿ� */
	SetGrabbedMark(&stGrab);

	return TRUE;
}

/*
 desc : Grabbed Image�� ��� ����
 parm : None
 retn : ����
*/
UINT16 CCamThread::GetGrabbedCount()
{
	UINT16 u16Count	= 0;

	// ����ȭ ����
	if (m_syncGrab.Enter())
	{
		u16Count = (UINT16)m_lstGrab.GetCount();
		// ����ȭ ����
		m_syncGrab.Leave();
	}

	return u16Count;
}

/*
 desc : Grabbed Images ��� Score ���� ��ȿ���� �� ��ȯ
 parm : None
 retn : TRUE : ��ȿ��, FALSE : ��ȿ���� ����
*/
BOOL CCamThread::IsScoreValidAll()
{
	BOOL bValid			= TRUE;
	POSITION pPos		= NULL;
	LPG_ACGR pstGrab	= {NULL};

	// ����ȭ ����
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.GetHeadPosition();
		while (pPos)
		{
			pstGrab	= m_lstGrab.GetNext(pPos);
			/* Score ���� ��ȿ���� Ȯ�� */
			if (!pstGrab->valid_score)
			{
				bValid	= FALSE;
				break;
			}
		}

		// ����ȭ ����
		m_syncGrab.Leave();
	}

	return bValid;
}

/*
 desc : Grabbed Image�� ������ ��ȯ
 parm : img_id	- [in]  Camera Grabbed Image Index (0 or Later)
		data	- [out] ��ȯ�� ����ü ���� ����
 retn : TRUE or FALSE
*/
LPG_ACGR CCamThread::GetGrabbedMark(UINT8 img_id)
{
	POSITION pPos	= NULL;
	LPG_ACGR pstGrab= NULL;

	// ����ȭ ����
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.GetHeadPosition();
		while (pPos)
		{
			pstGrab	= m_lstGrab.GetNext(pPos);
			if (pstGrab->img_id == img_id)	break;
			pstGrab	= NULL;	/* ã�� �������Ƿ�, �ݵ�� NULL ���Ѿ� �� */
		}
		// ����ȭ ����
		m_syncGrab.Leave();
	}

	return pstGrab;;
}

/*
 desc : Grabbed Image�� ������ ����
 parm : cam_id	- [in]  ī�޶� ��ȣ (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
		grab	- [in]  ����� ����ü ����
 retn : TRUE or FALSE
*/
BOOL CCamThread::SetGrabbedMark(LPG_ACGR grab)
{
	BOOL bFinded	= FALSE;
	POSITION pPos	= NULL, pPrePos;
	LPG_ACGR pstGrab= {NULL};

	// ����ȭ ����
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.GetHeadPosition();
		while (!bFinded && pPos)
		{
			pPrePos	= pPos;
			pstGrab	= m_lstGrab.GetNext(pPos);
			bFinded	= pstGrab->cam_id == grab->cam_id && pstGrab->img_id == grab->img_id;
			if (bFinded)
			{
				memcpy(pstGrab, grab, sizeof(STG_ACGR) - sizeof(PUINT8));
				break;
			}
		}
		// ����ȭ ����
		m_syncGrab.Leave();
	}

	return bFinded;
}

/*
 desc : Grabbed Image�� ������ ��ȯ
 parm : index	- [in]  ���������� �ϴ� ��ġ (Zero based)
		data	- [out] ��ȯ�� ����ü ���� ����
 retn : TRUE or FALSE
*/
LPG_ACGR CCamThread::GetGrabbedMarkIndex(UINT8 index)
{
	POSITION pPos	= NULL;
	LPG_ACGR pstGrab= NULL;

	// ����ȭ ����
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.FindIndex(index);
		if (pPos)	pstGrab	= m_lstGrab.GetAt(pPos);
		// ����ȭ ����
		m_syncGrab.Leave();
	}

	return pstGrab;
}

/*
 desc : Grabbed Image�� ������ ����
 parm : index	- [in]  ���������� �ϴ� ��ġ (Zero based)
		grab	- [in]  ����� ����ü ����
 retn : TRUE or FALSE
*/
BOOL CCamThread::SetGrabbedMarkIndex(UINT8 index, LPG_ACGR grab)
{
	BOOL bFinded	= FALSE;
	POSITION pPos	= NULL;
	LPG_ACGR pstGrab= NULL;

	// ����ȭ ����
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.FindIndex(index);
		if (pPos)
		{
			pstGrab	= m_lstGrab.GetAt(pPos);
			memcpy(pstGrab, grab, sizeof(STG_ACGR)-sizeof(PUINT8));
			bFinded	= TRUE;
		}
		// ����ȭ ����
		m_syncGrab.Leave();
	}

	return bFinded;
}

/*
 desc : Mark Search�� ��� ���������� �̷�� ������ ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamThread::IsGrabbedMarkValidAll()
{
	BOOL bSucc		= FALSE;
	POSITION pPos	= NULL;
	LPG_ACGR pstGrab= {NULL};

	// ����ȭ ����
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.GetHeadPosition();
		while (pPos)
		{
			pstGrab	= m_lstGrab.GetNext(pPos);
			bSucc	= pstGrab->IsMarkValid() && pstGrab->marked;
			if (!bSucc)	break;
		}
		// ����ȭ ����
		m_syncGrab.Leave();
	}

	return bSucc;
}

/*
 desc : Calibration Image�� �����ϸ�, Align Mark ����
 parm :	mode	- [in]  0xff : Calibration Mode, 0xfe : Align Camera Angle Measuring Mode
 retn : Calirbated Image�� ������ ����� ����ü ������ ��ȯ
*/
LPG_ACGR CCamThread::RunModelCali(UINT8 mode)
{
	BOOL bFinded	= FALSE;

	/* ���� Camera ���� ��尡 Calibration ������� Ȯ�� */
	if (m_pCamMain->GetCamMode() != ENG_VCCM::en_cali_mode)
	{
		SetError(L"It is not calibration mode.");
		return NULL;
	}
	// Calibrated Image�� �����ϴ� ��츸 ó��
	m_pstGrabbed	= m_pCamMain->GetGrabbedImage();
	// ���� Grabbed Image�� �����Ѵٸ� ...
	if (m_pstGrabbed->grab_size > 0 && m_pstGrabbed->grab_width > 0 && m_pstGrabbed->grab_height)
	{
		// ����ȭ ����
		if (m_syncGrab.Enter())
		{
			/* Model Find */
			bFinded	= uvMIL_RunModelFind(m_pstGrabbed->cam_id,
										 mode,	/* In case of calibration mode, the value is fixed */
										 m_pstGrabbed->grab_data);
			/* �ϴ�, �ֱ� ������ ����� ī�޶��� Grabbed ������ �߸��Ǿ��ٰ� �÷��� ���� */
			m_pstGrabACam->marked	= 0x00;
			if (bFinded)
			{
				/* �˻� ������ Mark ���� ��� */
				if (uvMIL_GetLastGrabbedMark())	m_pstGrabbed	= uvMIL_GetLastGrabbedMark();
				m_pstGrabbed->marked	= bFinded ? 0x01 /* success */ : 0x02 /* failed */;
				/* �ֱ� ����� ī�޶��� Grabbed ��� �� ���� */
				m_pstGrabACam->marked	= m_pstGrabbed->marked;
				m_pstGrabACam->move_mm_x= m_pstGrabbed->move_mm_x;
				m_pstGrabACam->move_mm_y= m_pstGrabbed->move_mm_y;
			}
			// ����ȭ ����
			m_syncGrab.Leave();
		}
	}
	return m_pstGrabbed;
}

/*
 desc : Step (Vision ����) Image�� �����ϸ�, Align Mark ����
 parm :	count	- [in]  �˻� ����� Mark ���� (ã�����ϴ� ��ũ ����)
		angle	- [in]  ���� ���� ���� (TRUE : ���� ����, FALSE : ���� ���� ����)
						TRUE : ���� ī�޶��� ȸ���� ���� ���ϱ� ����, FALSE : ���� ���� �����Ͽ� ȸ���� ���� ����
		results	- [out] �˻��� ��� ������ ����� ����ü ���� ������ (grabbed image buffer���� �����ؿ�)
 retn : TRUE or FALSE
*/
BOOL CCamThread::RunModelStep(UINT16 count, BOOL angle, LPG_ACGR results)
{
	UINT16 i		= 0;
	BOOL bFinded	= FALSE;
	LPG_ACGR pstGrab= NULL;
	/* �ݵ�� ���� */
	for (i=0; i<count; i++)	results[i].cam_id	= 0x01;

	/* ���� Camera ���� ��尡 Calibration ������� Ȯ�� */
	if (m_pCamMain->GetCamMode() != ENG_VCCM::en_cali_mode)
	{
		SetError(L"It is not calibration mode");
		return NULL;
	}
	// Calibrated Image�� �����ϴ� ��츸 ó��
	pstGrab	= m_pCamMain->GetGrabbedImage();
	// ���� Grabbed Image�� �����Ѵٸ� ...
	if (pstGrab->grab_size > 0 && pstGrab->grab_width > 0 && pstGrab->grab_height)
	{
		// ����ȭ ����
		if (m_syncGrab.Enter())
		{
			/* Model Find */
			bFinded	= uvMIL_RunModelStep(0x01, pstGrab->grab_data,
										 pstGrab->grab_width, pstGrab->grab_height,
										 count, angle, results);
			if (bFinded)
			{
				for (i=0; i<count; i++)
				{
					results[i].marked	= bFinded ? 0x01 /* success */ : 0x02 /* failed */;
				}
			}
			// ����ȭ ����
			m_syncGrab.Leave();
		}
	}

	return bFinded;
}

/*
 desc : Examination (Vision Align �˻� (����)) Image�� �����ϸ�, Align Shutting �˻� ����
 parm :	score		- [in]  ũ�� ���� �� (�� ������ ũ�ų� ������ Okay)
		scale		- [in]  ��Ī ���� �� (1.0 �������� +/- �� �� ���� �ȿ� ������ Okay)
		results		- [out] �˻��� ��� ������ ����� ����ü ���� ������ (grabbed image buffer���� �����ؿ�)
 retn : TRUE or FALSE
*/
BOOL CCamThread::RunModelExam(DOUBLE score, DOUBLE scale, LPG_ACGR results)
{
	UINT16 i		= 0;
	BOOL bFinded	= FALSE;
	LPG_ACGR pstGrab= NULL;

	/* �ݵ�� ���� */
	for (i=0; i<2 && results; i++)	results[i].cam_id	= 0x01;

	/* ���� Camera ���� ��尡 Calibration ������� Ȯ�� */
	if (m_pCamMain->GetCamMode() != ENG_VCCM::en_cali_mode)
	{
		SetError(L"It is not calibration mode");
		return NULL;
	}
	// Calibrated Image�� �����ϴ� ��츸 ó��
	pstGrab	= m_pCamMain->GetGrabbedImage();
	// ���� Grabbed Image�� �����Ѵٸ� ...
	if (pstGrab->grab_size > 0 && pstGrab->grab_width > 0 && pstGrab->grab_height)
	{
		// ����ȭ ����
		if (m_syncGrab.Enter())
		{
			/* Model Find */
			bFinded	= uvMIL_RunModelExam(pstGrab->grab_data,
										 pstGrab->grab_width, pstGrab->grab_height,
										 score, scale, results);
			if (bFinded)
			{
				for (i=0; i<2 && results; i++)
				{
					results[i].marked	= bFinded ? 0x01 /* success */ : 0x02 /* failed */;
				}
			}
			// ����ȭ ����
			m_syncGrab.Leave();
		}
	}

	return bFinded;
}

/*
 desc : Edge Image�� �����ϸ�, Align Mark ����
 parm :	cam_id	- [in]  Align Camera ID (1 or 2)
		saved	- [in]  �м��� edge image ���� ����
 retn : TRUE or FALSE
*/
BOOL CCamThread::RunEdgeDetect(UINT8 cam_id, UINT8 saved)
{
	BOOL bFinded		= FALSE;

	/* ���� Camera ���� ��尡 Calibration ������� Ȯ�� */
	if (m_pCamMain->GetCamMode() != ENG_VCCM::en_edge_mode)
	{
		SetError(L"It is not edge detection mode");
		return NULL;
	}
	// Calibrated Image�� �����ϴ� ��츸 ó��
	m_pstGrabbed	= m_pCamMain->GetGrabbedImage();
	// ���� Grabbed Image�� �����Ѵٸ� ...
	if (m_pstGrabbed->grab_size > 0 && m_pstGrabbed->grab_width > 0 && m_pstGrabbed->grab_height)
	{
		// ����ȭ ����
		if (m_syncGrab.Enter())
		{
			bFinded	= uvMIL_RunEdgeDetect(cam_id, m_pstGrabbed, saved);
#ifdef _DEBUG
			if (bFinded)
			{
#if 0
				TRACE(L"Mark Edge <Detection> = %d find (%d) (%.3f%%) \n",
					  GetTickCount64() - u64Tick, bFinded, dbDiaMeter);
#endif
			}
#endif // DEBUG
			// ����ȭ ����
			m_syncGrab.Leave();
		}
	}

	return bFinded;
}

/*
 desc : Matching Parameter ����
 parm : rotate	- [in]  Rotate ��� ����
		cornor	- [in]  Cornor �׸��� ����
		invert	- [in]  ���� ����ȭ ���� ����
		rate	- [in]  Matching Rate (����: %)
 retn : None
*/
VOID CCamThread::SetMatchParam(UINT8 rotate, DOUBLE match)
{
	m_u8MatchingRotate	= rotate;
	m_dbMatchingRate	= match;
}

/*
 desc : ī�޶� ������ �̹��� ���ۿ� Single Grabbed Image ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamThread::RunGrabbedImage()
{
	return m_pCamMain->RunGrabbedImage();
}

/*
 desc : ���� ���� ���� ����
 parm : flag	- [in]  0x00: ���� ���� ����, 0x01: ����� ����
 retn : None
*/
VOID CCamThread::UpdateLinkTime(UINT8 flag)
{
	BOOL bUpate	= FALSE;

	/* ���� ���� ���� �����̰�, ������ ����� ���¶�� */
	if (!flag && m_pstLink->is_connected)		bUpate	= TRUE;
	/* ���� ���� �����̰�, ������ ������ ���¶�� */
	else if (flag && !m_pstLink->is_connected)	bUpate	= TRUE;

	/* ���� ���¿� �ٸ� �����, ���� ���� */
	if (bUpate)
	{
		SYSTEMTIME stLinkTime;
		GetLocalTime(&stLinkTime);
		m_pstLink->is_connected	= flag;
		m_pstLink->link_time[0]	= stLinkTime.wYear;
		m_pstLink->link_time[1]	= stLinkTime.wMonth;
		m_pstLink->link_time[2]	= stLinkTime.wDay;
		m_pstLink->link_time[3]	= stLinkTime.wHour;
		m_pstLink->link_time[4]	= stLinkTime.wMinute;
		m_pstLink->link_time[5]	= stLinkTime.wSecond;
		m_pstLink->last_sock_error	= 0;
	}
}