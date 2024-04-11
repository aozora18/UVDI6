
/*
 desc : TCP/IP Client Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "CamThread.h"
#include "CamMain.h"
#include <timeapi.h>

#include "../UVDI15/GlobalVariables.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 desc : ������
 parm : cam_count	- [in]  ī�޶� ����
		max_grab	- [in]  ī�޶� ���� �� ���� ĸó�� �� �̹����� �ִ� ����
		link		- [in]  ī�޶� ���� ���� ����
		cam_main	- [in]  Camera Control Object
 retn : None
*/
CCamThread::CCamThread(UINT8 cam_count, UINT8 max_grab, STG_DLSM **link, CCamMain **cam_main, UINT8 img_proc)
{
	m_u8CamNext			= 0x00;
	m_u8MatchingRotate	= 0x00;
	m_dbMatchingRate	= 80.0f;

	m_u64AttachTime		= 0;
	m_u8CamCount		= cam_count;
	m_u8MaxGrab			= max_grab;
	m_u8SizeMul			= 4;	/* �⺻ ���� 4��� Grabbed Image�� Template Image�� ũ�⸦ Ű�� */
	m_pstLink			= link;
	m_pstGrabbed		= NULL;
	m_u8ImgProc			= img_proc;

	/* ���� �ֱٿ� Grabbed ����� ���� ����� ī�޶� ���� �����ϱ� ���� �ӽ� ���� */
	m_pstGrabACam		= new STG_ACLR[cam_count];
	ASSERT(m_pstGrabACam);
	memset(m_pstGrabACam, 0x00, sizeof(STG_ACLR) * UINT32(cam_count));

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
	if (m_pstGrabACam)	delete [] m_pstGrabACam;
}

/*
 desc : ������ ����� �ʱ� �ѹ� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCamThread::StartWork()
{
	return TRUE;
}

/*
 desc : ������ ����� �ֱ��� ȣ���
 parm : None
 retn : None
*/
VOID CCamThread::RunWork() 
{
	UINT8 u8CamID	= (m_u8CamNext % m_u8CamCount);

	/* ���� ī�޶� ����Ǿ� �ִ��� Ȯ�� */
	if (!m_pCamMain[u8CamID]->IsConnected())
	{
		/* ���� ���� ���� */
		UpdateLinkTime(u8CamID+1, 0x00);

		/* ���� ����Ǿ� �ִ� ī�޶� ���� ������ ���� ��Ŵ */
		m_pCamMain[u8CamID]->DetachDevice();
		/* Ư�� �ð� ���� �ֱ������� Open ���� */
		if (m_u64AttachTime+1000 < GetTickCount64())
		{
			/* ���� �õ� */
			m_pCamMain[u8CamID]->AttachDevice();
			m_u64AttachTime	= GetTickCount64();
		}
	}
	else
	{
		/* ���� �Ϸ� ���� */
		UpdateLinkTime(u8CamID+1, 0x01);

		/* ����Ǿ� �ִٸ�, ���� Grabbed Image�� �����ϴ��� Ȯ�� ��, �˻� ���� */
		if (ENG_VCCM::en_grab_mode == m_pCamMain[u8CamID]->GetCamMode())
		{
			ProcGrabbedImage(u8CamID+1, m_pCamMain[u8CamID]->GetDispType(), m_u8ImgProc);
		}
	}

	/* ��ϵ� ī�޶� ������� ũ��, 0 ������ �ʱ�ȭ ���� */
	if (++m_u8CamNext == m_u8CamCount)	m_u8CamNext	= 0x00;
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
 desc : ����� ī�޶� �κи� ������ ����
 parm : None
 retn : None
*/
VOID CCamThread::Reconnected()
{
	UINT8 i	= 0x00;
	if (m_pCamMain)
	{
		for (; i<m_u8CamCount; i++)	m_pCamMain[i]->DetachDevice();
	}
}

/*
 desc : ī�޶� ���� ���� ���� ��ȯ
 parm :	cam_id	- [in]  Camera Index (1 or 2)
 retn : TRUE or FALSE
*/
BOOL CCamThread::IsConnected(UINT8 cam_id)
{
	if (!m_pCamMain[cam_id-1])	return FALSE;
	return m_pCamMain[cam_id-1]->IsConnected();
}

/*
 desc : ���� Grabbed �̹��� ��� ����
 parm : None
 retn : None
*/
VOID CCamThread::ResetGrabbedImage()
{
	/* ���� ���� */
	if (m_syncGrab.Enter())
	{
		m_lstGrab.RemoveAll();
		/* ���� ���� */
		m_syncGrab.Leave();
	}
}

/*
 desc : Grabbed Image�� �����ϸ�, Align Mark ����
 parm :	cam_id	- [in]  Camera Index (1 or 2)
 retn : None
*/
VOID CCamThread::ProcGrabbedImage(UINT8 cam_id, UINT8 dlg_id, UINT8 img_proc)
{
	BOOL bFinded		= FALSE;
	LPG_ACGR pstGrab	= {NULL};

	/* ����ȭ ���� */
	if (m_syncGrab.Enter())
	{
		// Grabbed Image�� �����ϴ� ��츸 ó��
		pstGrab	= m_pCamMain[cam_id-1]->GetGrabbedImage();
		
		bool globalGrab = true;
		// ���� Grabbed Image�� �����Ѵٸ� ...
		if (pstGrab && pstGrab->grab_data)
		{
			//���⼭ �� �ٲ�����ϳ� ...
			ENG_AMOS alignMotion = alignMotionPtr->markParams.alignMotion;

			switch (alignMotion)
			{
				case ENG_AMOS::en_onthefly_2cam:
				{
					const int SIDE_2CAM_NUM = 2;
					globalGrab = pstGrab->img_id < SIDE_2CAM_NUM ? true : false;
					bFinded = uvMIL_RunModelFind(pstGrab->cam_id, pstGrab->img_id, pstGrab->img_id, pstGrab->grab_data, dlg_id, pstGrab->img_id < SIDE_2CAM_NUM ? GLOBAL_MARK : LOCAL_MARK, FALSE, img_proc); // global mark
				}
				break;

				case ENG_AMOS::en_static_3cam:
				{
					globalGrab = alignMotionPtr->status.globalMarkCnt - 1 > pstGrab->img_id ? true : false;
					bFinded = uvMIL_RunModelFind(pstGrab->cam_id, pstGrab->img_id, pstGrab->img_id, pstGrab->grab_data, dlg_id, globalGrab == true ? GLOBAL_MARK : LOCAL_MARK, FALSE, img_proc); // global mark
				}
				break;

				default:
				{
					throw ACCESS_EXCEPTION("not implement.");
				}
				break;
			}

			/* Get the search result value of mark regardless of success or failure of the search */
			pstGrab	= uvMIL_GetLastGrabbedMark();

			if (!pstGrab)	LOG_ERROR(ENG_EDIC::en_basler, L"The number of images captured has been exceeded");
			else
			{
				pstGrab->grabTime = timeGetTime();
				pstGrab->fiducialMarkIndex = -1818;
				pstGrab->marked = UINT8(bFinded);


				if ((UINT8)m_lstGrab.GetCount() < m_u8MaxGrab)
				{
					m_lstGrab.AddTail(pstGrab);
					pstGrab->fiducialMarkIndex = alignMotionPtr == NULL ? -1818 : alignMotionPtr->GetFiducialIndex(cam_id, globalGrab, &m_lstGrab);
				}
				else
				{
					LOG_ERROR(ENG_EDIC::en_basler, L"The number of images captured has been exceeded");
				}
			}
		}
		/* ����ȭ ���� */
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
	STG_ACGR stGrab;
	stGrab.Init();
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
UINT16 CCamThread::GetGrabbedCount(int* camNum)
{
	UINT16 u16Count	= 0;

	/* ����ȭ ���� */
	if (m_syncGrab.Enter())
	{

		u16Count = camNum == nullptr ? (UINT16)m_lstGrab.GetCount() : [&]()->int
	{
		int grab = 0;

		for (int i = 0; i < m_lstGrab.GetCount(); i++)
		{
			auto val = m_lstGrab.GetAt(m_lstGrab.FindIndex(i));
			grab = val->cam_id == *camNum ? grab + 1: grab;
		}
		return grab;
	}();

		/* ����ȭ ���� */
		m_syncGrab.Leave();
	}

	return u16Count;
}

/*
 desc : Grabbed Images ��� Score ���� ��ȿ���� �� ��ȯ
 parm :	set_score	- [in]  If the score of the searched mark is higher than this set score, it is valid
 							(If 0.0f is unchecked) 
 retn : TRUE : ��ȿ��, FALSE : ��ȿ���� ����
*/
BOOL CCamThread::IsScoreValidAll(DOUBLE set_score)
{
	BOOL bValid			= TRUE;
	POSITION pPos		= NULL;
	LPG_ACGR pstGrab	= {NULL};

	/* ����ȭ ���� */
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.GetHeadPosition();
		if (!pPos)	bValid	= FALSE;
		else
		{
			while (pPos && bValid)
			{
				pstGrab	= m_lstGrab.GetNext(pPos);
				if (pstGrab)
				{
					/* Score ���� ��ȿ���� Ȯ�� */
					if (!pstGrab->valid_score)	bValid	= FALSE;
					else
					{
						if (set_score > pstGrab->score_rate)	bValid = FALSE;
					}
				}
			}
		}
		/* ����ȭ ���� */
		m_syncGrab.Leave();
	}

	return bValid;
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

	/* ����ȭ ���� */
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.GetHeadPosition();
		while (!bFinded && pPos)
		{
			pPrePos	= pPos;
			pstGrab	= m_lstGrab.GetNext(pPos);
			if (pstGrab)
			{
				bFinded	= pstGrab->cam_id == grab->cam_id && pstGrab->img_id == grab->img_id;
				if (bFinded)	memcpy(pstGrab, grab, sizeof(STG_ACGR) - sizeof(PUINT8));
			}
		}
		/* ����ȭ ���� */
		m_syncGrab.Leave();
	}

	return bFinded;
}

/*
 desc : Grabbed Image�� ������ ��ȯ
 parm : index	- [in]  ���������� �ϴ� ��ġ (Zero based)
 retn : TRUE or FALSE
*/
LPG_ACGR CCamThread::GetGrabbedMarkIndex(UINT8 index)
{
	POSITION pPos	= NULL;
	LPG_ACGR pstGrab= NULL;

	/* ���� ���� */
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.FindIndex(index);
		if (pPos)	pstGrab	= m_lstGrab.GetAt(pPos);
#ifdef _DEBUG
		if (!pstGrab)
		{
			INT32 i = 0;
			i++;
		}
#endif
		/* ���� ���� */
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

	/* ���� ���� */
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.FindIndex(index);
		if (pPos)
		{
			pstGrab	= m_lstGrab.GetAt(pPos);
			memcpy(pstGrab, grab, sizeof(STG_ACGR)-sizeof(PUINT8));
			bFinded	= TRUE;
		}
		/* ���� ���� */
		m_syncGrab.Leave();
	}

	return bFinded;
}

/*
 desc : Mark Search�� ��� ���������� �̷�� ������ ����
 parm : multi_mark	- [in]  Multi Mark (���� ��ũ) ���� ����
		set_score	- [in]  If the score of the searched mark is higher than this set score, it is valid
							(If 0.0f is unchecked)
 retn : TRUE or FALSE
*/
BOOL CCamThread::IsGrabbedMarkValidAll(BOOL multi_mark, DOUBLE set_score,int* camNum)
{
	BOOL bIsValid	= TRUE;
	POSITION pPos	= NULL;
	LPG_ACGR pstGrab= {NULL};

	/* ���� ���� */
	if (m_syncGrab.Enter())
	{
		pPos	= m_lstGrab.GetHeadPosition();
		if (!pPos)	bIsValid	= FALSE;
		else
		{
			while (bIsValid && pPos)
			{
				pstGrab	= m_lstGrab.GetNext(pPos);
				if (camNum != nullptr && pstGrab->cam_id != *camNum)
					continue;

				bIsValid= pstGrab->IsMarkValid(multi_mark) && pstGrab->marked;
				if (bIsValid)
				{
					if (set_score > 0.0f && set_score > pstGrab->score_rate)	bIsValid	= FALSE;
				}
			}
		}
		/* ���� ���� */
		m_syncGrab.Leave();
	}

	return bIsValid;
}

/*
 desc : Grabbed Image�� ������ ��ȯ
 parm : cam_id	- [in]  ī�޶� ��ȣ (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
		data	- [out] ��ȯ�� ����ü ���� ����
 retn : TRUE or FALSE
*/
LPG_ACGR CCamThread::GetGrabbedMark(UINT8 cam_id, UINT8 img_id)
{
	POSITION pPos	= NULL;
	LPG_ACGR pstGrab= NULL;

	/* ���� ���� */
	if (m_syncGrab.Enter())
	{
		pPos = m_lstGrab.GetHeadPosition();
		while (pPos)
		{
			pstGrab	= m_lstGrab.GetNext(pPos);
			if (pstGrab->cam_id == cam_id && pstGrab->img_id == img_id)	break;
			/* ã�� �������Ƿ�, �ݵ�� NULL ���Ѿ� �� */
			pstGrab	= NULL;
		}
		/* ���� ���� */
		m_syncGrab.Leave();
	}

	return pstGrab;
}

/*
 desc : Grabbed Image�� ������ ��ȯ
 parm : cam_id	- [in]  ī�޶� ��ȣ (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
		data	- [out] ��ȯ�� ����ü ���� ����
 retn : TRUE or FALSE
*/
CAtlList <LPG_ACGR>* CCamThread::GetGrabbedMarkAll()
{
	return GetGrabImage();
}

/*
 desc : Grabbed Image�� �����ϸ�, Align Mark ����
 parm :	cam_id	- [in]  Camera Index (1 or 2)
		mode	- [in]  0xff : Calibration Mode, 0xfe : Align Camera Angle Measuring Mode
 retn : Calirbated Image�� ������ ����� ����ü ������ ��ȯ
*/
LPG_ACGR CCamThread::RunModelFind(UINT8 cam_id, UINT8 mode, UINT8 dlg_id, UINT8 mark_no, BOOL useMilDisp, UINT8 img_proc , int flipDir)
{
	BOOL bFinded	= FALSE;
#if 0
	/* ���� Camera ���� ��尡 Calibration ������� Ȯ�� */
	if (m_pCamMain[cam_id-1]->GetCamMode() != ENG_VCCM::en_cali_mode)
	{
		LOG_ERROR(L"It is not model find mode.");
		return NULL;
	}
#endif
	// Calibrated Image�� �����ϴ� ��츸 ó��
	m_pstGrabbed	= m_pCamMain[cam_id-1]->GetGrabbedImage();
	if (!m_pstGrabbed)
		return NULL;
	// ���� Grabbed Image�� �����Ѵٸ� ...
	if (m_pstGrabbed->grab_size > 0 && m_pstGrabbed->grab_width> 0 && m_pstGrabbed->grab_height)
	{
		/* ����ȭ ���� */
		if (m_syncGrab.Enter())
		{
			// lk91 �ӽ� �ּ�!!
			//uvMIL_RegistMILImg(m_pstGrabbed->cam_id - 1, m_pstGrabbed->grab_width, m_pstGrabbed->grab_height, m_pstGrabbed->grab_data);
			//int tmpDispType;
			//if (dlg_id == DISP_TYPE_MARK_LIVE || dlg_id == DISP_TYPE_MARK) {
			//	tmpDispType = DISP_TYPE_MARK_LIVE;
			//}
			//else {
			//	tmpDispType = dlg_id;
			//}

			uvMIL_DrawImageBitmapFlip(dlg_id, m_pstGrabbed->cam_id - 1, m_pstGrabbed, 0.0, m_pstGrabbed->cam_id - 1, flipDir); // lk91 img_id �� 0���� �����ϴ��� üũ

			/* Model Find */
			bFinded	= uvMIL_RunModelFind(m_pstGrabbed->cam_id,
										 mode,	/* In case of calibration mode, the value is fixed */
											m_pstGrabbed->img_id, m_pstGrabbed->grab_data, dlg_id, mark_no, useMilDisp, img_proc);
			/* �ϴ�, �ֱ� ������ ����� ī�޶��� Grabbed ������ �߸��Ǿ��ٰ� �÷��� ���� */
			m_pstGrabACam[cam_id-1].marked	= 0x00;
			if (bFinded)
			{
				/* �˻� ������ Mark ���� ��� */
				if (uvMIL_GetLastGrabbedMark())	m_pstGrabbed = uvMIL_GetLastGrabbedMark();
				m_pstGrabbed->marked = bFinded ? 0x01 /* success */ : 0x02 /* failed */;
				/* �ֱ� ����� ī�޶��� Grabbed ��� �� ���� */
				m_pstGrabACam[cam_id - 1].marked = m_pstGrabbed->marked;
				m_pstGrabACam[cam_id - 1].move_mm_x = m_pstGrabbed->move_mm_x;
				m_pstGrabACam[cam_id - 1].move_mm_y = m_pstGrabbed->move_mm_y;
			}

			/* ����ȭ ���� */
			m_syncGrab.Leave();
		}
	}

	return m_pstGrabbed;
}

/*
 desc : Step (Vision ����) Image�� �����ϸ�, Align Mark ����
 parm :	cam_id	- [in]  Camera Index (1 or 2)
		count	- [in]  �˻� ����� Mark ���� (ã�����ϴ� ��ũ ����)
		angle	- [in]  ���� ���� ���� (TRUE : ���� ����, FALSE : ���� ���� ����)
						TRUE : ���� ī�޶��� ȸ���� ���� ���ϱ� ����, FALSE : ���� ���� �����Ͽ� ȸ���� ���� ����
		results	- [out] �˻��� ��� ������ ����� ����ü ���� ������ (grabbed image buffer���� �����ؿ�)
 retn : TRUE or FALSE
*/
BOOL CCamThread::RunModelStep(UINT8 cam_id, UINT16 count, BOOL angle, LPG_ACGR results, UINT8 dlg_id, UINT8 mark_no, UINT8 img_proc)
{
	UINT16 i		= 0;
	BOOL bFinded	= FALSE;
	LPG_ACGR pstGrab= NULL;

	/* �ݵ�� ���� */
	for (i=0; i<count; i++)	results[i].cam_id	= cam_id;
	//results.cam_id = cam_id;

	/* ���� Camera ���� ��尡 Calibration ������� Ȯ�� */
	if (m_pCamMain[cam_id-1]->GetCamMode() != ENG_VCCM::en_cali_mode)
	{
		LOG_ERROR(ENG_EDIC::en_basler, L"It is not model step mode");
		return NULL;
	}
	// Calibrated Image�� �����ϴ� ��츸 ó��
	pstGrab	= m_pCamMain[cam_id-1]->GetGrabbedImage();
	// ���� Grabbed Image�� �����Ѵٸ� ...
	if (pstGrab->grab_size > 0 && pstGrab->grab_width > 0 && pstGrab->grab_height)
	{
		/* ����ȭ ���� */
		if (m_syncGrab.Enter())
		{
			uvMIL_RegistMILImg(pstGrab->cam_id - 1, pstGrab->grab_width, pstGrab->grab_height, pstGrab->grab_data);
			int tmpDispType;
			if (dlg_id == DISP_TYPE_MARK_LIVE || dlg_id == DISP_TYPE_MARK) {
				tmpDispType = DISP_TYPE_MARK_LIVE;
			}
			else {
				tmpDispType = dlg_id;
			}
			uvMIL_DrawImageBitmap(tmpDispType, pstGrab->cam_id - 1, pstGrab, 0.0, pstGrab->cam_id - 1); // lk91 img_id �� 0���� �����ϴ��� üũ


			/* Model Find */
			bFinded = uvMIL_RunModelStep(cam_id, pstGrab->grab_data,
				pstGrab->grab_width, pstGrab->grab_height,
				count, angle, results, pstGrab->img_id, dlg_id, mark_no, img_proc);
			if (bFinded)
			{
				for (i = 0; i < count; i++)
				{
					results[i].marked = bFinded ? 0x01 /* success */ : 0x02 /* failed */;
				}
			}

			/* ����ȭ ���� */
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
	UINT16 i = 0;
	BOOL bFinded = FALSE;
	LPG_ACGR pstGrab = NULL;

	/* �ݵ�� ���� */
	//for (i = 0; i < 2 && results; i++)	results[i].cam_id = 0x01;

	int tgtCamId = results[0].cam_id - 1;
	/* ���� Camera ���� ��尡 Calibration ������� Ȯ�� */
	if (m_pCamMain[tgtCamId]->GetCamMode() != ENG_VCCM::en_cali_mode)
	{
		LOG_ERROR(ENG_EDIC::en_basler, L"It is not model exam mode");
		return NULL;
	}
	// Calibrated Image�� �����ϴ� ��츸 ó��
	int timeAccum = 0;
	while (pstGrab == nullptr)
	{
		pstGrab = m_pCamMain[tgtCamId]->GetGrabbedImage();
		timeAccum += 300;
		Sleep(300);
		if (timeAccum > 3000)break;
	}
	
	// ���� Grabbed Image�� �����Ѵٸ� ...
	if (pstGrab->grab_size > 0 && pstGrab->grab_width > 0 && pstGrab->grab_height > 0)
	{
		/* ����ȭ ���� */
		if (m_syncGrab.Enter())
		{
			/* Model Find */
			bFinded = uvMIL_RunModelExam2(pstGrab->grab_data,
				pstGrab->grab_width, pstGrab->grab_height,
				score, scale, results);
			if (bFinded)
			{
				for (i = 0; i < 2 && results; i++)
				{
					results[i].marked = bFinded ? 0x01 /* success */ : 0x02 /* failed */;
				}
			}

			if (results[0].marked == true && results[0].marked == results[1].marked)
			{
				if (uvMIL_GetLastGrabbedMark())	m_pstGrabbed = uvMIL_GetLastGrabbedMark();
				m_pstGrabbed->marked = bFinded ? 0x01 /* success */ : 0x02 /* failed */;
				/* �ֱ� ����� ī�޶��� Grabbed ��� �� ���� */
				m_pstGrabACam[0].marked = m_pstGrabbed->marked;
				m_pstGrabACam[0].move_mm_x = m_pstGrabbed->move_mm_x;
				m_pstGrabACam[0].move_mm_y = m_pstGrabbed->move_mm_y;
			}

			/* ����ȭ ���� */
			m_syncGrab.Leave();
		}
	}
	else
	{
		int deug = 0;
	}
	return bFinded;
}

/*
 desc : Set the parameters of the strip maker and find in an image and take the specified measurements
 parm : cam_id	- [in]  Align Camera ID (1 or 2 or later)
		param	- [in]  Structures pointer with information to set up is stored
		result	- [out] Buffer in which the resulting value will be returned and stored.
 retn : TRUE or FALSE
*/
BOOL CCamThread::RunMarkerStrip(UINT8 cam_id, LPG_MSMP param, STG_MSMR &results)
{
	UINT16 i		= 0;
	BOOL bFinded	= FALSE;
	LPG_ACGR pstGrab= NULL;

	/* ���� Camera ���� ��尡 Calibration ������� Ȯ�� */
	if (m_pCamMain[0]->GetCamMode() != ENG_VCCM::en_hole_mode)
	{
		LOG_ERROR(ENG_EDIC::en_basler, L"It is not hole measurement mode");
		return NULL;
	}
	// Calibrated Image�� �����ϴ� ��츸 ó��
	pstGrab	= m_pCamMain[0]->GetGrabbedImage();
	// ���� Grabbed Image�� �����Ѵٸ� ...
 	if (pstGrab->grab_size > 0 && pstGrab->grab_width > 0 && pstGrab->grab_height > 0)
	{
		/* Enter sync. */
		if (m_syncGrab.Enter())
		{
			/* Model Find */
			bFinded	= uvMIL_RunMarkerStrip(cam_id, pstGrab->grab_data, param, results);
			/* Leave sync */
			m_syncGrab.Leave();
		}
	}

	return bFinded;
}

/*
 desc : Edge Image�� �����ϸ�, Align Mark ����
 parm :	cam_id	- [in]  Camera Index (1 or 2)
		saved	- [in]  �м��� edge image ���� ����
 retn : TRUE or FALSE
*/
BOOL CCamThread::RunEdgeDetect(UINT8 cam_id, UINT8 saved)
{
	BOOL bFinded		= FALSE;

	/* ���� Camera ���� ��尡 Calibration ������� Ȯ�� */
	if (m_pCamMain[cam_id-1]->GetCamMode() != ENG_VCCM::en_edge_mode)
	{
		LOG_ERROR(ENG_EDIC::en_basler, L"It is not edge detection mode");
		return NULL;
	}
	// Calibrated Image�� �����ϴ� ��츸 ó��
	m_pstGrabbed	= m_pCamMain[cam_id-1]->GetGrabbedImage();
	// ���� Grabbed Image�� �����Ѵٸ� ...
	if (!m_pstGrabbed->IsEmpty())
	{
		/* ����ȭ ���� */
		if (m_syncGrab.Enter())
		{
			bFinded	= uvMIL_RunEdgeDetect(cam_id, m_pstGrabbed, saved);

			/* ����ȭ ���� */
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
 desc : ���� ���� ���� ����
 parm : cam_id	- [in]  Align Camera ID (1 or Later)
		flag	- [in]  0x00: ���� ���� ����, 0x01: ����� ����
 retn : None
*/
VOID CCamThread::UpdateLinkTime(UINT8 cam_id, UINT8 flag)
{
	BOOL bUpate	= FALSE;

	/* ���� ���� ���� �����̰�, ������ ����� ���¶�� */
	if (!flag && m_pstLink[cam_id-1]->is_connected)		bUpate	= TRUE;
	/* ���� ���� �����̰�, ������ ������ ���¶�� */
	else if (flag && !m_pstLink[cam_id-1]->is_connected)	bUpate	= TRUE;

	/* ���� ���¿� �ٸ� �����, ���� ���� */
	if (bUpate)
	{
		SYSTEMTIME stLinkTime;
		GetLocalTime(&stLinkTime);
		m_pstLink[cam_id-1]->is_connected	= flag;
		m_pstLink[cam_id-1]->link_time[0]	= stLinkTime.wYear;
		m_pstLink[cam_id-1]->link_time[1]	= stLinkTime.wMonth;
		m_pstLink[cam_id-1]->link_time[2]	= stLinkTime.wDay;
		m_pstLink[cam_id-1]->link_time[3]	= stLinkTime.wHour;
		m_pstLink[cam_id-1]->link_time[4]	= stLinkTime.wMinute;
		m_pstLink[cam_id-1]->link_time[5]	= stLinkTime.wSecond;
		m_pstLink[cam_id-1]->last_sock_error= 0;
	}
}


CDPoint CCamThread::RunModel_VisionCalib(UINT8 cam_id, UINT8 dlg_id, UINT8 mark_no, int* roi_left, int* roi_right, int* roi_top, int* roi_bottom, int row, int col)
{
	CDPoint MarkP;
	MarkP.x = 0;
	MarkP.y = 0;
	BOOL bFinded = FALSE;
#if 0
	/* ���� Camera ���� ��尡 Calibration ������� Ȯ�� */
	if (m_pCamMain[cam_id - 1]->GetCamMode() != ENG_VCCM::en_cali_mode)
	{
		LOG_ERROR(L"It is not model find mode.");
		return NULL;
	}
#endif
	// Calibrated Image�� �����ϴ� ��츸 ó��
	m_pstGrabbed = m_pCamMain[cam_id - 1]->GetGrabbedImage();
	if (!m_pstGrabbed)
		return MarkP;
	// ���� Grabbed Image�� �����Ѵٸ� ...
	if (m_pstGrabbed->grab_size > 0 && m_pstGrabbed->grab_width > 0 && m_pstGrabbed->grab_height)
	{
		/* ����ȭ ���� */
		if (m_syncGrab.Enter())
		{
			uvMIL_DrawImageBitmap(dlg_id, m_pstGrabbed->cam_id - 1, m_pstGrabbed, 0.0, m_pstGrabbed->cam_id - 1); // lk91 img_id �� 0���� �����ϴ��� üũ

			/* Model Find */
			//bFinded = uvMIL_RunModelFind(m_pstGrabbed->cam_id,
			//	mode,	/* In case of calibration mode, the value is fixed */
			//	m_pstGrabbed->img_id, m_pstGrabbed->grab_data, dlg_id, mark_no, useMilDisp, img_proc);
			uvMIL_RunModel_VisionCalib(m_pstGrabbed->cam_id, m_pstGrabbed->img_id, m_pstGrabbed->grab_data, dlg_id, mark_no, roi_left, roi_right, roi_top, roi_bottom, row, col);

			/* ����ȭ ���� */
			m_syncGrab.Leave();
		}
	}

	return MarkP;
}
