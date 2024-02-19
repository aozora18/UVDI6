
/*
 desc : IP Camera Communication Libarary (Basler pylon sdk ACA1300-30GM)
*/

#include "pch.h"
#include "MainApp.h"

#include "CamThread.h"
#include "CamMain.h"
#include "DrawMark.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GRAB_DRAW_DELAY_TIME	50	/* ����: msec */

/* --------------------------------------------------------------------------------------------- */
/*                                           ���� ����                                           */
/* --------------------------------------------------------------------------------------------- */

UINT32						g_u32LastACamSizeBytes	= 0;	/* ���� �ֱٿ� ������ ����� ī�޶��� �ػ� (�̹���)�� ������ ���� ũ�� */

UINT64						g_u64DrawDelayTime		= 0;	/* ���������� ȭ�� ����ϴµ� �Ѱ谡 �����Ƿ�, ���� �ð� ���� */

LPG_CIEA					g_pstConfig				= NULL;
LPG_VDSM					g_pstShMem				= NULL;

CCamThread					*g_pCamThread			= NULL;
CCamMain					*g_pCamMain				= NULL;
CDrawMark					g_csDrawMark;


/* --------------------------------------------------------------------------------------------- */
/*                                           ���� �Լ�                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ���� Grabbed �̹��� ��� ����
 parm : None
 retn : None
*/
VOID ResetGrabbedImage()
{
	g_pCamMain->ResetGrabbedImage();
	g_pCamThread->ResetGrabbedImage();

	// Match Template Image �޸� ����
	uvMIL_ResetGrabAll();
}

/*
 desc : Global 4���� Mark�� ���ؼ� ��� ��ȿ���� �˻�
 ���� :	img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : TRUE (��ȿ��) or FALSE (4�� �� 1���� ����� �ν� �ȵ�)
*/
BOOL IsValidGrabMarkNum(UINT8 img_id)
{
	LPG_ACGR pstGrab;

	if (!g_pCamThread)	return FALSE;
	pstGrab	= g_pCamThread->GetGrabbedMark(img_id);
	if (!pstGrab->valid_scale || !pstGrab->valid_score)	return FALSE;

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                           �ܺ� �Լ�                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/*
 desc : BIPS Server�� ����� ����ϴ� Client ������ ���� �� ����
 parm : config		- [in]  ȯ�� ����
		shmem		- [in]  Shared Memory
		work_dir	- [in]  �۾� ��� (���� ���� ���)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_Init(LPG_CIEA config, LPG_VDSM shmem, PTCHAR work_dir)
{
	g_pstConfig	= config;
	g_pstShMem	= shmem;

	/* Vision : MIL / Cognex / OpenCV / Halcon */
	if (!config->use_libs.use_vision_kind)	return TRUE;
	switch (config->use_libs.use_vision_kind)
	{
	case ENG_VLPK::en_vision_opencv	:	return FALSE;	break;
	case ENG_VLPK::en_vision_mil		:	if (!uvMIL_Init(config, shmem, work_dir))	return FALSE;	break;
	case ENG_VLPK::en_vision_cognex	:	return FALSE;	break;
	case ENG_VLPK::en_vision_halcon	:	return FALSE;	break;
	}

	/* ȯ�� ���Ͽ� ������ ����� ī�޶� �ػ��� ũ�⸦ ������ ���� (����: Bytes) */
	g_u32LastACamSizeBytes	= g_pstConfig->set_cams.GetCamSizeBytes();
	/* Grabbed Image ���� �� ���� ���� Ŭ���� ���� */
	g_pCamMain	= new CCamMain(config);
	ASSERT(g_pCamMain);
	/* Grabbed Image ó���ϴ� ������ (IDS�� ���, ������ ī�޶� 1�� �̹Ƿ�) */
	g_pCamThread	= new CCamThread(shmem->link[0], g_pCamMain);
	ASSERT(g_pCamThread);
	if (!g_pCamThread->CreateThread(0, 0, NORMAL_THREAD_SPEED))	return FALSE;

	return TRUE;
}

/*
 desc : BIPS Server�� ����� ����ϴ� Client ������ ���� �� ����
 parm : config		- [in]  ȯ�� ����
		shmem		- [in]  Shared Memory
		work_dir	- [in]  �۾� ��� (���� ���� ���)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_Open(LPG_CIEA config, LPG_VDSM shmem)
{
	g_pstConfig	= config;
	g_pstShMem	= shmem;

	return TRUE;
}

/*
 desc : ���̺귯�� ����
 parm : None
 retn : None
*/
API_EXPORT VOID uvIDS_Close()
{
	UINT8 i	= 0;

	// IP Camera�� ����� ����ϴ� ��ü �޸� ����
	if (g_pCamThread)
	{
		/* Grabbed Image ��Ī ���� ������ ���� �� �޸� ���� */
		g_pCamThread->Stop();
		while (g_pCamThread->IsBusy())	g_pCamThread->Sleep(100);
		delete g_pCamThread;
	}
	/* Grabbed Image ���� ��ü �޸� ���� */
	delete g_pCamMain;
	g_pCamMain	= NULL;

	/* Vision : MIL / Cognex / OpenCV / Halcon */
	if (!g_pstConfig->use_libs.use_vision_kind)	return;
	switch (g_pstConfig->use_libs.use_vision_kind)
	{
	case ENG_VLPK::en_vision_opencv	:	break;
	case ENG_VLPK::en_vision_mil	:	uvMIL_Close();
	case ENG_VLPK::en_vision_cognex	:	break;
	case ENG_VLPK::en_vision_halcon	:	break;
	}
}

/*
 desc : ����� ī�޶� �κи� ������ ����
 parm : None
 retn : None
*/
API_EXPORT VOID uvIDS_Reconnected()
{
	g_pCamThread->Reconnected();
	uvMIL_ResetGrabAll();
}

/*
 desc : ���� �ʱ�ȭ
 parm : None
 retn : None
*/
API_EXPORT VOID uvIDS_ResetError()
{
	g_pCamMain->ResetError();
}

/*
 desc : Mark Pattern ���� ���� �ڵ�
 parm : hwnd	- [in]  ��ũ ���� ���� �ڵ�
 retn : None
*/
API_EXPORT VOID uvIDS_SetMarkHwnd(HWND hwnd)
{
	g_csDrawMark.SetDrawHwnd(hwnd);
}

/*
 desc : BIPS Server�� ����� ����ϴ� Client ������ ���� �� ����
 parm : time_out	- [in]  ���� ī�޶���� ���������� ������������ ����Ǿ��� ��,
							ī�޶� ���ο��� ���� ����� Ŭ���̾�Ʈ�� ������ ���� ��, ���� ��� �ð�
							Ŭ���̾�Ʈ�κ��� ���� ���� �ð�(����: �и���)���� ���� ������, ���� ����
		work_dir	- [in]  �۾� ��� (���� ���� ���)
		run_mode	- [in]  0x01 : CMPS, 0x01 : Vision Step
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_UpdateSetParam(TCHAR *work_dir, ENG_ERVM run_mode, UINT64 time_out)
{
	if (g_u32LastACamSizeBytes != g_pstConfig->set_cams.GetCamSizeBytes())
	{
		/* ���� ���� ���̺귯�� ���� �� �ʱ�ȭ ���� */
		uvMIL_Close();
		/* ���� ���̺귯�� �ʱ�ȭ */
		if (!uvMIL_Init(g_pstConfig, g_pstShMem, work_dir))	return FALSE;
		/* ���� �ֱٿ� ����� ����� ī�޶� �ػ��� ũ�� �� �ٽ� ���� */
		g_u32LastACamSizeBytes	= g_pstConfig->set_cams.GetCamSizeBytes();
	}

	return g_pCamMain->SetParamUpdate();
}

/*
 desc : Mark Pattern ���
 parm : inner	- [in]  ���� ũ�� (����: pixel)
		outer	- [in]  �ܺ� ũ�� (����: pixel)
		reverse	- [in]  ��� ���� ���� (0x00 - In:White/Out:Black, 0x01 - In:Black/Out:White)
		pattern	- [in]  ��ũ ���� Ÿ�� (0x00 - Circle, 0x01 - Rectangle, 0x02 - Cross)
 retn : None
*/
API_EXPORT VOID uvIDS_CreateMark(LPRECT inner, LPRECT outer, UINT8 reverse, UINT8 pattern)
{
	g_csDrawMark.Draw(inner, outer, reverse, pattern);
}

/*
 desc : ��ũ �׸���
 parm : file	- [in]  ���� ���� (��ü ��� ����)
		inner	- [in]  ���� ũ�� (����: pixel)
		outer	- [in]  �ܺ� ũ�� (����: pixel)
		reverse	- [in]  ��� ���� ���� (0x00 - In:White/Out:Black, 0x01 - In:Black/Out:White)
		pattern	- [in]  ��ũ ���� Ÿ�� (0x00 - Circle, 0x01 - Rectangle, 0x02 - Cross)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SaveMark(TCHAR *file, LPRECT inner, LPRECT outer, UINT8 reverse, UINT8 pattern)
{
	return g_csDrawMark.Save(file, inner, outer, reverse, pattern);
}

/*
 desc : Vision Camera Mode ����
 parm : mode	- [in]  ENG_VCCM::en_xxx
 retn : None
*/
API_EXPORT VOID uvIDS_SetCamMode(ENG_VCCM mode)
{
	if (g_pCamMain)
	{
		g_pCamMain->SetCamMode(mode);
	}
}

/*
 desc : ���� Camera�� ���� ��� �� ��ȯ
 parm : None
 retn : ENG_VCCM::en_xxx
*/
API_EXPORT ENG_VCCM uvIDS_GetCamMode()
{
	return g_pCamMain == NULL ? ENG_VCCM::en_none : g_pCamMain->GetCamMode();
}

/*
 desc : ���� PLC�� ����Ǿ� �ִ��� ���� Ȯ��
 ���� :	None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_IsConnected()
{
	if (!g_pCamThread || !g_pCamMain)	return FALSE;

	return g_pCamMain->IsConnected();
}

/*
 desc : ���� ��� Align Camera�� ����Ǿ� �ִ��� ���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_IsConnectedAll()
{
	if (!g_pCamMain)				return FALSE;
	if (!g_pCamMain->IsConnected())	return FALSE;

	return TRUE;
}

/*
 desc : ���� ��ϵ� Grabbed �̹��� ���� ��ȯ
 parm : None
 retn : ����� �̹��� ���� ��ȯ
*/
API_EXPORT UINT16 uvIDS_GetGrabbedCount()
{
	return g_pCamThread->GetGrabbedCount();
}

/*
 desc : Grabbed Images ��� Score ���� ��ȿ���� �� ��ȯ
 parm : None
 retn : TRUE : ��ȿ��, FALSE : ��ȿ���� ����
*/
API_EXPORT BOOL uvIDS_IsScoreValidAll()
{
	return !g_pCamThread ? FALSE : g_pCamThread->IsScoreValidAll();
}

/*
 desc : Grabbed Image�� ��� ��ȯ
 parm : img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : grab data ��ȯ
*/
API_EXPORT LPG_ACGR uvIDS_GetGrabbedFind(UINT8 img_id)
{
	if (!g_pCamThread)	return FALSE;
	return g_pCamThread->GetGrabbedMark(img_id);
}

/*
 desc : Grabbed Image�� ��� ����
 parm : grab	- [in]  ����� ����ü ������
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SetGrabbedFind(LPG_ACGR grab)
{
	if (!g_pCamThread || grab->cam_id < 1)	return FALSE;
	return g_pCamThread->SetGrabbedMark(grab);
}

/*
 desc : Grabbed Image�� ������ ��ȯ
 parm : index	- [in]  ���������� �ϴ� ��ġ (Zero based)
		data	- [out] ����� ����ü ���� ����
 retn : grab data ��ȯ
*/
API_EXPORT LPG_ACGR uvIDS_GetGrabbedFindIndex(UINT8 index)
{
	return !g_pCamThread ? NULL : g_pCamThread->GetGrabbedMarkIndex(index);
}

/*
 desc : Grabbed Image�� ������ ����
 parm : index	- [in]  ���������� �ϴ� ��ġ (Zero based)
 parm : grab	- [in]  ����� ����ü ������
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SetGrabbedFindIndex(UINT8 index, LPG_ACGR grab)
{
	return !g_pCamThread ? FALSE : g_pCamThread->SetGrabbedMarkIndex(index, grab);
}

/*
 desc : Global 4���� Mark�� ���ؼ� ��� ��ȿ���� �˻�
 parm : None
 retn : TRUE (��ȿ��) or FALSE (4�� �� 1���� ����� �ν� �ȵ�)
*/
API_EXPORT BOOL uvIDS_IsGrabbedMarkValidAll()
{
	return g_pCamThread->IsGrabbedMarkValidAll();
}

/*
 desc : Global 4���� Mark�� ���ؼ� ��� ��ȿ���� �˻�
 ���� :	img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : TRUE (��ȿ��) or FALSE (4�� �� 1���� ����� �ν� �ȵ�)
*/
API_EXPORT BOOL uvIDS_IsValidGrabMarkNum(UINT8 img_id)
{
	LPG_ACGR pstGrab	= NULL;

	if (!g_pCamThread)	return FALSE;
	pstGrab	= g_pCamThread->GetGrabbedMark(img_id);
	if (!pstGrab)	return FALSE;
#if (USE_MARK_VALID_SCALE)
	if (!pstGrab->valid_scale || !pstGrab->valid_score || !pstGrab->valid_dist)	return FALSE;
#else
	if (!pstGrab->valid_score)	return FALSE;
#endif

	return TRUE;
}

/*
 desc : ���� Grabbed �̹��� ��� ����
 parm : None
 retn : None
*/
API_EXPORT VOID uvIDS_ResetGrabbedImage()
{
	ResetGrabbedImage();
}

/*
 desc : Calibration Image�� �����ϸ�, Align Mark ����
 ���� :	mode	- [in]  0xff : Calibration Mode (����� ī�޶� ���� �������� ����)
						0xfe : Align Camera Angle Measuring Mode (����� ī�޶� ���� �����ϱ� ����)
 retn : Calirbated Image�� ������ ����� ����ü ������ ��ȯ
*/
API_EXPORT LPG_ACGR uvIDS_RunModelCali(UINT8 mode)
{
	/* New Grabbed Image ���� */
	return g_pCamThread->RunModelCali(mode);
}

/*
 desc : ���� �ֱٿ� Calibrated�� ��� ������ ��ȯ
 parm : None
 retn : ��Ī�� ��� �� ��ȯ
*/
API_EXPORT LPG_ACGR uvIDS_GetLastGrabbedMark()
{
	if (!g_pCamThread)	return NULL;
	return g_pCamThread->GetLastGrabbedMark();
}

/*
 desc : ���� �ֱٿ� ī�޶� ���� Calibrated�� ��� �� Error ���� ����� ������ ��ȯ
 parm : None
 retn : ��Ī�� ��� �� ��ȯ
*/
API_EXPORT LPG_ACLR uvIDS_GetLastGrabbedACam()
{
	if (!g_pCamThread)	return NULL;
	return g_pCamThread->GetLastGrabbedACam();
}

/*
 desc : Step (Vision ����) Image�� �����ϸ�, Align Mark ����
 ���� :	counts	- [in]  �˻� ����� Mark ���� (ã�����ϴ� ��ũ ����)
		angle	- [in]  ���� ���� ���� (TRUE : ���� ����, FALSE : ���� ���� ����)
						TRUE : ���� ī�޶��� ȸ���� ���� ���ϱ� ����, FALSE : ���� ���� �����Ͽ� ȸ���� ���� ����
		results	- [out] �˻��� ��� ������ ����� ����ü ���� ������ (grabbed image buffer���� �����ؿ�)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_RunModelStep(UINT16 counts, BOOL angle, LPG_ACGR results)
{
	if (!g_pCamThread)	return FALSE;
	return g_pCamThread->RunModelStep(counts, angle, results);
}

/*
 desc : Examination (Vision Align �˻� (����)) Image�� �����ϸ�, Align Shutting �˻� ����
 ���� :	score		- [in]  ũ�� ���� �� (�� ������ ũ�ų� ������ Okay)
		scale		- [in]  ��Ī ���� �� (1.0 �������� +/- �� �� ���� �ȿ� ������ Okay)
		results		- [out] �˻��� ��� ������ ����� ����ü ���� ������ (grabbed image buffer���� �����ؿ�)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_RunModelExam(DOUBLE score, DOUBLE scale, LPG_ACGR results)
{
	if (!g_pCamThread)	return FALSE;
	return g_pCamThread->RunModelExam(score, scale, results);
}

/*
 desc : Grabbed Image�� �����ϸ�, Edge Detection ����
 ���� :	cam_id	- [in]  Align Camera ID (1 or 2)
		saved	- [in]  �м��� edge image ���� ����
 retn : Edge Detected�� ������ ���� (����) �� ��ȯ (����: Pixel)
*/
API_EXPORT BOOL uvIDS_RunEdgeDetect(UINT8 cam_id, UINT8 saved)
{
	/* ���� Grabbed Image ���� */
	ResetGrabbedImage();
	/* New Grabbed Image ���� */
	return g_pCamThread->RunEdgeDetect(cam_id, saved);
}

/*
 desc : Geometric Model Find
 parm : cam_id	- [in]  Camera Index (1 or 2)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_RunModelFind(UINT8 cam_id)
{
	LPG_ACGR pstGrab	= g_pCamThread->GetLastGrabbedMark();
	if (!pstGrab)	return FALSE;
	/* �˻� �۾� ���� */
	return uvMIL_RunModelFind(cam_id, pstGrab->img_id, pstGrab->grab_data);

}

/*
 desc : ���� Grabbed Image �ʱ�ȭ
 parm : None
 retn : None
*/
API_EXPORT VOID uvIDS_ResetGrabbedMark()
{
	if (!g_pCamMain)	return;
	g_pCamMain->ResetGrabbedMark();
}

/*
 desc : ���� Grabbed Image�� ���� �� Bytes ���� ����� Gray �� ���� ���
 parm : mode	- [in]  Grabbed Image�� ĸó�ϴ� ��� (���)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SetGrabGrayRate(ENG_VCCM mode)
{
	if (!g_pCamMain)	return FALSE;
	return g_pCamMain->SetGrabGrayRate(mode);
}

/*
 desc : ���� Grabbed Image�� ���� �� Gray Level �� (����) ��ȯ
 parm : index	- [in]  0 ~ 255�� �ش�Ǵ� Gray Level Index
 retn : 0 ~ 255 ���� Grabbed Image���� ���� �� ��ȯ
*/
API_EXPORT DOUBLE uvIDS_GetGrabGrayRate(UINT8 index)
{
	if (!g_pCamMain)	return 0.0f;
	return g_pCamMain->GetGrabGrayRate(index);
}

/*
 desc : ��ü Grabbed Image���� ������ �ִ� ���� ���� ��ȯ
 parm : None
 retn : White ���� �� ��ȯ
*/
API_EXPORT DOUBLE uvIDS_GetGrabGrayRateTotal()
{
	if (!g_pCamMain)	return 0.0f;
	return g_pCamMain->GetGrabGrayRateTotal();
}

/*
 desc : ���� Grabbed Image�� ���� �� Bytes ���� ����� Gray �� ����
		���̿� ���� ���̿� �ִ� ��� Gray ���� �ջ��Ͽ� ����
 parm : mode	- [in]  Grabbed Image�� ĸó�ϴ� ��� (���)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SetGrabHistLevel(ENG_VCCM mode)
{
	if (!g_pCamMain)	return FALSE;
	return g_pCamMain->SetGrabHistLevel(mode);
}

/*
 desc : ���� Grabbed Image�� ���� �� Bytes ���� ����� Gray Level ��ȯ
 parm : None
 retn : ���� Grabbed Image�� ���� (Column)�� 1 �ȼ����� ������ Level ������ ����Ǿ� �ִ� ���� ��ȯ
*/
API_EXPORT PUINT64 uvIDS_GetGrabHistLevel()
{
	if (!g_pCamMain)	return NULL;
	return g_pCamMain->GetGrabHistLevel();
}

/*
 desc : ���� Grabbed Image�� ���� �� Bytes ���� ����� Gray Level���� �� �� �� ��ȯ
 parm : None
 retn : Grabbed Image�� ��ü Gray Level ������ �� ��ȯ
*/
API_EXPORT UINT64 uvIDS_GetGrabHistTotal()
{
	if (!g_pCamMain)	return 0;
	return g_pCamMain->GetGrabHistTotal();
}

/*
 desc : ���� Grabbed Image�� ���� Column ���� Gray Level �� �� ���� ū ��
 parm : None
 retn : Grabbed Image�� Column ���� Gray Level �� �� ���� ū �� ��ȯ
*/
API_EXPORT UINT64 uvIDS_GetGrabHistMax()
{
	if (!g_pCamMain)	return 0;
	return g_pCamMain->GetGrabHistMax();
}


/* --------------------------------------------------------------------------------------------- */
/*                             ���� ó�� ���� (Gateway) �Լ� �κ�                                */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ���� �˻��� �̹��� (Edge Detect or Line Detect)�� ���� ������ ��� ����
 parm : hwnd	- [in]  �̹����� ����� ���� �ڵ�
		type	- [in]  0x00: Edge Detection, 0x01: Line Detection
 retn : None
*/
API_EXPORT VOID uvIDS_DrawDetect(HWND hwnd, UINT8 type)
{
	uvMIL_DrawDetect(hwnd, type);
}

/*
 desc : Live ����� ���, ���� Grabbed�� �̹��� ��� ����
 ���� :	hwnd	- [in]  �̹����� ��µ� ���� �ڵ�
 retn : None
*/
API_EXPORT VOID uvIDS_DrawLive(HWND hwnd)
{
	LPG_ACGR pstGrab	= NULL;

	if (ENG_VCCM::en_grab_mode != g_pCamMain->GetCamMode())
	{
		/* Check to Grabbed Image Size */
		pstGrab	= g_pCamMain->GetGrabbedImage();
		if (pstGrab->IsEmpty())	return;
		/* ������ �׷ȴ� �ð��� ���ؼ� ���� �ð� �������� Ȯ�� */
		if (GetTickCount64() > g_u64DrawDelayTime + GRAB_DRAW_DELAY_TIME)
		{
			/* ���� Grabbed Image 1�� ��� */
			uvMIL_DrawLive(hwnd, pstGrab);
			/* ������ �׸� ��, ���� �ð� ������ ���ؼ� */
			g_u64DrawDelayTime	= GetTickCount64();
		}
	}
}

/*
 desc : Live ����� ���, ���� �ֱٿ� Grabbed�� �̹��� ��� ����
 ���� :	hwnd	- [in]  �̹����� ��µ� ���� �ڵ�
 retn : None
*/
API_EXPORT VOID uvIDS_DrawLastLive(HWND hwnd)
{
	LPG_ACGR pstGrab	= NULL;

	pstGrab	= g_pCamMain->GetGrabbedLiveImage();
	if (!pstGrab)	return;
	// ���� Grabbed Image 1�� ���
	if (pstGrab->grab_width && pstGrab->grab_height)
	{
		/* ������ �׷ȴ� �ð��� ���ؼ� ���� �ð� �������� Ȯ�� */
		if (GetTickCount64() > g_u64DrawDelayTime + GRAB_DRAW_DELAY_TIME)
		{
			uvMIL_DrawLive(hwnd, pstGrab);
			/* ������ �׸� ��, ���� �ð� ������ ���ؼ� */
			g_u64DrawDelayTime	= GetTickCount64();
		}
	}
}

/*
 desc : ���� �ֱٿ� Live�� Grabbed�� �̹��� ���� ����
 ���� :	None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_IsGetLastLiveImage()
{
	return g_pCamMain->GetGrabbedLiveImage() ? TRUE : FALSE;
}

/*
 desc : Calibration �̹��� (�˻��� ���)�� ���� ������ ��� ����
 ���� :	hwnd	- [in]  �̹����� ��µ� ���� �ڵ�
 retn : None
*/
API_EXPORT VOID uvIDS_DrawCaliMark(HWND hwnd)
{
	uvMIL_DrawCaliMark(hwnd);
}
/*
 desc : �˻��� ��ũ �̹����� ���Ե� ����ü ������ ��ȯ
 parm : img_id	- [in]  Camera Grabbed Image Index (0 or Later) (if img_id == 0xff then Calibration Method)
 retn : ����ü ������ ��ȯ
*/
API_EXPORT LPG_ACGR uvIDS_GetGrabbedData(UINT8 img_id)
{
#if 0
	return uvMIL_GetGrabbedData(img_id);
#else
	return g_pCamThread->GetGrabbedMark(img_id);
#endif
}

/*
 desc : ���� �˻��� ��ũ �̹��� ���� ������ ��� ����
 ���� :	img_id	- [in]  Camera Grabbed Image Index (0 or Later)
		hwnd	- [in]  �̹����� ��µ� ���� �ڵ�
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_DrawMark(UINT8 img_id, HWND hwnd)
{
	UINT8 u8FindMark	= 0x00;

	/* �˻��� Mark�� ��ȿ���� ���� */
	u8FindMark	= IsValidGrabMarkNum(img_id) ? 0x01 : 0x00;
	/* Grabbed Image�� ���� Mark ���� �׸��� */
	return uvMIL_DrawMark(hwnd, 0x01, img_id, u8FindMark);
}

/*
 desc : Calibration �̹��� ���� ������ ��� ����
 parm : hwnd	- [in]  �̹����� ��µ� ���� �ڵ�
		grab	- [in]  grab �̹����� ����� ����ü ������ (grab_data�� �̹����� ������ �ݵ�� ����Ǿ� �־�� ��)
		find	- [in]  ��ũ �˻� ���� ���� (�˻��ƴ���, �˻� ��� ���� ���� �޶���)
						0x00 - �˻� ��� ����, 0x01 - �˻� ��� ����
 retn : None
*/
API_EXPORT VOID uvIDS_DrawMarkData(HWND hwnd, LPG_ACGR grab, UINT8 find)
{
	uvMIL_DrawMarkData(hwnd, grab, find);
}

/*
 desc : Drawing - Examination Object Image
 parm : hwnd	- [in]  �̹����� ��µ� ���� �ڵ�
		grab_out- [in]  Grabbed Image ������ ����� ����ü ������ (�ٱ� ��)
		grab_in	- [in]  Grabbed Image ������ ����� ����ü ������ (���� ��)
 retn : None
*/
API_EXPORT VOID uvIDS_DrawGrabExamImage(HWND hwnd, LPG_ACGR grab_out, LPG_ACGR grab_in)
{
	uvMIL_DrawGrabExamImage(hwnd, grab_out, grab_in);
}

/*
 desc : Mark ���� ����
 parm : speed	- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		smooth	- [in]  0.0f - �⺻ ��, 1.0f ~ 100.0f
		model	- [in]  Model Type, Param 1 ~ 5�� �������� ��� ���Ե� ����ü ������
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SetModelDefine(UINT8 speed, DOUBLE smooth, LPG_CMPV model)
{
	return uvMIL_SetModelDefine(0x01, speed, smooth, model);
}

/*
 desc : ���� �ý��� (�����)�� ����Ǿ� �ִ� ���� ��Ī ���� �̹��� ����
 parm : speed	- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		smooth	- [in]  0.0f - �⺻ ��, 1.0f ~ 100.0f
		model	- [in]  Model Type ��, circle, square, rectangle, cross, diamond, triangle
		param	- [in]  �� 5���� Parameter Values (unit : um)
		find	- [in]  �˻��ϰ����ϴ� ���� ���� (�ݵ�� ã�����ϴ� ���� ������)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SetModelDefineEx(UINT8 speed, DOUBLE smooth, PUINT32 model,
									   DOUBLE *param1, DOUBLE *param2, DOUBLE *param3,
									   DOUBLE *param4, DOUBLE *param5, UINT32 find)
{
	if (!uvMIL_SetModelDefineEx(0x01, speed, smooth, model,
								param1, param2, param3, param4, param5, find))	return FALSE;
	return TRUE;
}

/*
 desc : Mark ���� ���� - �̹��� ������
 parm : speed	- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		file	- [in]  �� �̹����� ����� ���� �̸� (��ü ��� ����)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SetModelDefineLoad(UINT8 speed, CStringArray &file)
{
	return uvMIL_SetModelDefineLoad(0x01, speed, file);
}

/*
 desc : ���� ��ϵ� Mark Model ���� ��ȯ
 parm : cam_idNone
 retn : ��ϵ� ����ü ������ ��ȯ
*/
API_EXPORT LPG_CMPV uvIDS_GetModelDefine()
{
	return uvMIL_GetModelDefine(0x01);
}

/*
 desc : ���� �˻��� ���� ��ϵ� ���� ���� ��ȯ
 parm : None
 retn : ��ϵ� ����
*/
API_EXPORT UINT32 uvIDS_GetModelRegistCount()
{
	return uvMIL_GetModelRegistCount(0x01);
}

/*
 desc : ������ ���� (���)�Ǿ� �ִ� ��� Mark ���� ����
 parm : None
 retn : None
*/
API_EXPORT VOID uvIDS_ResetMarkModel()
{
	uvMIL_ResetMarkModel();
}

/*
 desc : Mark Template�� ��ϵǾ� �ִ��� Ȯ��
 parm : mode	- [in]  0x00 : Expose, 0x01 : Vstep
 retn : TRUE - ��ϵǾ� �ִ�. FALSE - ��ϵ��� �ʾҴ�.
*/
API_EXPORT BOOL uvIDS_IsSetMarkModel(UINT8 mode)
{
	return uvMIL_IsSetMarkModel(mode, 0x01);
}

/*
 desc : Mark Pattern ��� ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_IsSetMarkModelNumber()
{
	return uvMIL_IsSetMarkModelNumber(0x01);
}

/*
 desc : ��� ī�޶� (ī�޶� ������ ����)�� Mark Model�� �����Ǿ����� ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_IsSetMarkModelAll()
{
	return	uvMIL_IsSetMarkModelNumber(0x01);
}

/*
 desc : ���� �ֱٿ� Edge Detection�� �̹������� Ư�� ������ �����ؼ� Bitmap Image�� ���
		��, Mark Template �̹����� ���
 parm : cam_id	- [in]  Align Camera ID (1 or Later)
		area	- [in]  �����ϰ��� �ϴ� ���� ������ ����� �ȼ� ��ġ ��
		type	- [in]  0x00 : Edge, 0x02 : Line, 0x02 : Match, 0x03 : Grab
		file	- [in]  Mark Template�� �����Ϸ��� ���� �̸� (��ü ��� ����)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SaveGrabbedMarkToFile(UINT8 cam_id, LPRECT area, UINT8 type, TCHAR *file)
{
	return uvMIL_SaveGrabbedMarkToFile(cam_id, area, type, file);
}

/*
 desc : Edge Detection �˻��� ��� ��ȯ
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
 retn : ��� ���� ����� ������ ��ȯ
*/
API_EXPORT LPG_EDFR uvIDS_GetEdgeDetectResults(UINT8 cam_id)
{
	return uvMIL_GetEdgeDetectResults(cam_id);
}

/*
 desc : Edge Detection �˻��� ���� ��ȯ
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
 ��ȯ : ���� ��ȯ. ������ ���, 0 ���� ��
*/
API_EXPORT INT32 uvIDS_GetEdgeDetectCount(UINT8 cam_id)
{
	return uvMIL_GetEdgeDetectCount(cam_id);
}

/*
 desc : ���� ��ϵ� Mark Model Type ��ȯ
 parm : mark_id	- [in]  Align Mark Index (0x00 ~ MAX_REGIST_MODEL-1)
 retn : Model Type ��
*/
API_EXPORT UINT32 uvIDS_GetMarkModelType(UINT8 index)
{
	return uvMIL_GetMarkModelType(0x01, index);
}

/*
 desc : �� ũ�� ��ȯ
 parm : index	- [in]  ��û�ϰ��� �ϴ� ���� ��ġ (Zero-based)
		flag	- [in]  0x00 : ����  ũ��, 0x01 : ���� ũ��
		unit	- [in]  0x00 : um, 0x01 : pixel
 retn : ũ�� ��ȯ (����: um)
*/
API_EXPORT DOUBLE uvIDS_GetMarkModelSize(UINT8 index, UINT8 flag, UINT8 unit)
{
	return uvMIL_GetMarkModelSize(0x01, index, flag, unit);
}

/*
 desc : ����ڿ� ���� �������� �˻��� ���, ��� ���� ó��
 parm : grab	- [in]  ����ڿ� ���� �������� �Էµ� grabbed image ��� ������ ����� ����ü ������
		gmfr	- [in]  GMFR Data
		gmsr	- [in]  GMSR Data
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SetGrabbedMark(LPG_ACGR grab, LPG_GMFR gmfr, LPG_GMSR gmsr)
{
	if (!g_pCamThread)	return FALSE;
	return g_pCamThread->SetGrabbedMark(grab, gmfr, gmsr);
}

/*
 desc : Align Mark �˻� ��� ����
 parm : method	- [in]  0x00 : ������ 1�� ��ȯ, 0x01 : ���� ��ũ �������� ����ġ�� �ο��Ͽ� ��� ��ȯ
		count	- [in]  'method' ���� 1�� ���, ���� �˻��� Mark ���� �� 2 �̻� ���̾�� ��
 retn : None
*/
API_EXPORT VOID uvIDS_SetMarkMethod(ENG_MMSM method, UINT8 count)
{
	if (method == ENG_MMSM::en_single)	count	= 0x00;
	uvMIL_SetMarkMethod(method, count);
}

/*
 desc : ī�޶� ������ �̹��� ���ۿ� Single Grabbed Image ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_RunGrabbedImage()
{
	/* Grabbed Image */
	return g_pCamThread->RunGrabbedImage();
}

/*
 desc : ī�޶�κ��� Live Image 1���� Grabbed �Ǿ����� ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_IsLiveGrabbedImage()
{
	return g_pCamMain->IsLiveGrabbedImage();
}

/*
 desc : ���� Live ���� �̹����� ��µǴ� ���� (Client ȭ�� ��ǥ��) ��ǥ �� ��ȯ
 parm : live	- [out] Client �������� ������ ��µǰ� �ִ� ��ġ ��ȯ
 retn : Live ���� �̹����� ��µ� ���� �� ��ȯ
*/
API_EXPORT VOID uvIDS_GetLiveArea(RECT &live)
{
	uvMIL_GetLiveArea(live);
}

/*
 desc : Iamge ȸ�� (�̹����� �߽��� �������� ȸ��)
 parm : img_src		- [in]  ���� �̹��� ����
		img_dst		- [out] ȸ���� �̹��� ����
		width		- [in]	���� �̹����� ����
		height		- [in]	���� �̹����� ����
		angle		- [in]	ȸ�� �ϰ��� �ϴ� ����
 retn : None
*/
API_EXPORT VOID uvIDS_ImageRotate(PUINT8 img_src, PUINT8 img_dst,
								  UINT32 width, UINT32 height, UINT32 angle)
{
	uvMIL_ImageRotate(img_src, img_dst, width, height, angle);
}

/*
 desc : ���� ���� ī�޶�κ��� Grab�Ǵ� �̹����� ũ�� (Pixel) ��ȯ
 parm : flag	- [in]  0x00 : Width, 0x01 : Height
 retn : �̹��� ũ�� ��ȯ
*/
API_EXPORT UINT32 uvIDS_GetFovSize(UINT8 flag)
{
	if (flag > 2)	return 0;
	return g_pstConfig->GetACamGrabSize(flag);
}

/*
 desc : ���� �ֱٿ� ����� Live Image�� ���Ϸ� ����
 parm : file	- [in]  ����� ���� �̸� (��ü ��� ����)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SaveLiveImageToFile(TCHAR *file)
{
	return g_pCamMain->SaveLiveImageToFile(file);
}

/*
 desc : ���� �ֱٿ� ����� Live Image���� Drag ���� �̹��� ���� ���
 parm : drag	- [in]  Live Image ������ Drag ������ ������ ��ǥ ���� ���� (����: �ȼ�)
 retn : drag �̹��� ������ �����ϰ� �ִ� ���� ������
*/
API_EXPORT PUINT8 uvIDS_GetLastLiveDragImage(RECT drag)
{
	return g_pCamMain->GetLastLiveDragImage(drag);
}

/*
 desc : ���� MIL ���̼����� ��ȿ���� ���� Ȯ��
		Pattern, Model Finder, Edge Finder, Read, OCR, Image Processing �� ��ȿ���� Ȯ��
 parm : None
 retn : TRUE or FALS
*/
API_EXPORT BOOL uvIDS_IsLicenseValid()
{
	return uvMIL_IsLicenseValid();
}

/*
 desc : �뱤 ��� ���� ��, ���� �뱤, ����� �뱤, ���� �� ����� �뱤
 parm : mode	- [in]  ���� �뱤 (0x00), ����� �뱤 (0x01), ����� ī�޶� ���� �� ���� �� ����� �뱤 (0x02)
 retn : None
*/
API_EXPORT VOID uvIDS_SetAlignMode(ENG_AOEM mode)
{
	uvMIL_SetAlignMode(mode);
}

/*
 desc : �̹����� ���� Ȥ�� ������ ��輱 �߽� ��ġ ���
 parm : cam_id		- [in]  Align Camera ID (1 or 2)
		type		- [in]  0x00: Camera Grabbed Image
							0x01: File Grabbed Image
		mil_result	- [in]  MIL ���� �˻� ��� ��ȭ ���� ��� ����
 retn : TRUE or FALSE
*/
#if (USE_EQUIPMENT_SIMULATION_MODE)
API_EXPORT BOOL uvIDS_RunLineDetectCentXY(UINT8 cam_id, UINT8 type, BOOL mil_result)
#else
API_EXPORT BOOL uvIDS_RunLineDetectCentXY(UINT8 cam_id, BOOL mil_result)
#endif
{
	LPG_ACGR pstGrab	= NULL;
	/* Check to Grabbed Image Size */
#if (USE_EQUIPMENT_SIMULATION_MODE)
	if (0x00 == type)	pstGrab	= g_pCamMain->GetGrabbedImage();
	else				pstGrab	= g_pCamMain->GetFileGrabbedImage();
#else
	pstGrab	= g_pCamMain->GetGrabbedImage();
#endif
	if (!pstGrab || pstGrab->IsEmpty())	return FALSE;
	return uvMIL_RunLineDetectCentXY(cam_id, pstGrab, mil_result);
}

#if (USE_EQUIPMENT_SIMULATION_MODE)
/*
 desc : File Grabbed Image ó��
 parm : file	- [in]  ��ü ��ΰ� ���Ե� �̹��� ���� (256 ��Ʈ��. 8 bit gray)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvIDS_SetFileGrabbedImage(PTCHAR file)
{
	UINT32 u32FileSize	= 0, i = 0, j = 0;
	BITMAPFILEHEADER stBmpFile;
	BITMAPINFOHEADER stBmpInfo;
	PUINT8 pImage		= NULL;
	FILE *fpRead		= NULL;
	CUniToChar csCnv;

	/* ���� ���� */
	if (0 != fopen_s(&fpRead, csCnv.Uni2Ansi(file), "rb"))	return FALSE;
	/* Header ���� �б� */
	fread_s(&stBmpFile, sizeof(BITMAPFILEHEADER), sizeof(BITMAPFILEHEADER), 1, fpRead);
	if (stBmpFile.bfType  != 0x4D42)
	{
		fclose(fpRead);
		return FALSE;
	}
	fread_s(&stBmpInfo, sizeof(BITMAPINFOHEADER), sizeof(BITMAPINFOHEADER), 1, fpRead);
	if (stBmpInfo.biBitCount != 8)
	{
		fclose(fpRead);
		return FALSE;
	}
#if 0	/* 256 gray color ����̹Ƿ�, �ȷ�Ʈ�� �ʿ� ���� */
	/* BMP Pallete */
	fread_s(stRGB, sizeof(RGBQUAD)*2, sizeof(RGBQUAD)*2, 1, fpRead);
#endif
	fseek(fpRead, stBmpFile.bfOffBits, SEEK_SET);
	/* ���� �̹��� �б� */
	if (stBmpInfo.biSizeImage < 1)
	{
		stBmpInfo.biSizeImage	= stBmpFile.bfSize - stBmpFile.bfOffBits;
	}
	pImage	= (PUINT8)::Alloc(stBmpInfo.biSizeImage+1);
	pImage[stBmpInfo.biSizeImage]	= 0x00;
	fread_s(pImage, stBmpInfo.biSizeImage+1, 1, stBmpInfo.biSizeImage, fpRead);
	fclose(fpRead);

	/* ��Ʈ�� �̹��� ���� ���� */
#if 0
	pInvert	= (PUINT8)::Alloc(stBmpInfo.biSizeImage+1);
	pInvert[stBmpInfo.biSizeImage]	= 0x00;
	for (i=0; i<stBmpInfo.biHeight; i++)
	{
		for (j=0; j<stBmpInfo.biWidth; j++)
		{
			pInvert[(stBmpInfo.biHeight - i - 1) * stBmpInfo.biWidth + j] = 
				pImage[j + i * stBmpInfo.biWidth];
		}
	}
	::Free(pInvert);
#else
	uvCmn_BmpDataUpDownChange(stBmpInfo.biWidth, stBmpInfo.biHeight, pImage);
#endif
	/* ���� ���ε� ���� */
	g_pCamMain->SetFileGrabbedImage(stBmpInfo.biSizeImage,
									stBmpInfo.biWidth,
									stBmpInfo.biHeight,
									pImage);
	/* �޸� ���� */
	::Free(pImage);

	return TRUE;
}
#endif

#ifdef __cplusplus
}
#endif
