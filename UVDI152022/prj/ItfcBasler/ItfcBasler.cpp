
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

UINT64						g_u64DrawDelayTime[MAX_ALIGN_CAMERA]	= {NULL};	/* ���������� ȭ�� ����ϴµ� �Ѱ谡 �����Ƿ�, ���� �ð� ���� */

ENG_VLPK					g_enVisionLib	= ENG_VLPK::en_vision_none;

LPG_VDSM					g_pstShMem		= NULL;

CCamThread					*g_pCamThread	= NULL;
CCamMain					**g_pCamMain	= NULL;
CDrawMark					g_csDrawMark;


/* --------------------------------------------------------------------------------------------- */
/*                                           ���� �Լ�                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ���� Grabbed Image�� ��ũ �˻� ���� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID ResetGrabbedMark()
{
	UINT8 i	= 0x00;

	if (!g_pCamMain)	return;
	for (; i<g_pstConfig->set_cams.acam_count; i++)	g_pCamMain[i]->ResetGrabbedMark();
}

/*
 desc : ���� Grabbed �̹��� ��� ����
 parm : None
 retn : None
*/
VOID ResetGrabbedImage()
{
	UINT8 i	= 0;

	for (; i<g_pstConfig->set_cams.acam_count; i++)
	{
		g_pCamMain[i]->ResetGrabbedImage();
	}
	g_pCamThread->ResetGrabbedImage();

	/* ���� Grabbed Image�� ��ũ �˻� ���� �ʱ�ȭ */
	ResetGrabbedMark();
	/* Match Template Image �޸� ���� */
	uvMIL_ResetGrabAll();
}

/*
 desc : Global 4���� Mark�� ���ؼ� ��� ��ȿ���� �˻�
 ���� :	cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : TRUE (��ȿ��) or FALSE (4�� �� 1���� ����� �ν� �ȵ�)
*/
BOOL IsValidGrabMarkNum(UINT8 cam_id, UINT8 img_id)
{
	LPG_ACGR pstGrab;

	if (!g_pCamThread)	return FALSE;
	pstGrab	= g_pCamThread->GetGrabbedMark(cam_id, img_id);
	if (!pstGrab->valid_scale || !pstGrab->valid_score)	return FALSE;

	return TRUE;
}

/*
 desc : Global Mark �������, 2 ���� �˻��� ��ũ ���� ��ȯ
 parm : direct	- [in]  �������� (0 - 1�� ��ũ�� 3�� ��ũ ���� ���� ��)
								 (1 - 2�� ��ũ�� 4�� ��ũ ���� ���� ��)
								 (2 - 1�� ��ũ�� 2�� ��ũ ���� ���� ��)
								 (3 - 3�� ��ũ�� 4�� ��ũ ���� ���� ��)
								 (4 - 1�� ��ũ�� 4�� ��ũ ���� ���� ��)
								 (5 - 2�� ��ũ�� 3�� ��ũ ���� ���� ��)
		data1	- [out] �˻��� ù ��° ��ũ�� �߽� ��ġ���� ���� �� (����: mm)
		data2	- [out] �˻��� �� ��° ��ũ�� �߽� ��ġ���� ���� �� (����: mm)
 retn : TRUE or FALSE
*/
BOOL GetGrabbedMarkDirect(ENG_GMDD direct, STG_DBXY &data1, STG_DBXY &data2)
{
	LPG_ACGR pstGrab[2]	= {NULL};

	/* ���� �Ź��� ��ǥ ���� ������ nm(����)�̹Ƿ�, 100���� ������� 100 nm or 0.1 um�� �ȴ�.*/
	switch (direct)
	{
	case ENG_GMDD::en_top_horz	:	pstGrab[0]	= g_pCamThread->GetGrabbedMark(1, 0);
									pstGrab[1]	= g_pCamThread->GetGrabbedMark(2, 0);	break;
	case ENG_GMDD::en_btm_horz	:	pstGrab[0]	= g_pCamThread->GetGrabbedMark(1, 1);
									pstGrab[1]	= g_pCamThread->GetGrabbedMark(2, 1);	break;
	case ENG_GMDD::en_lft_vert	:	pstGrab[0]	= g_pCamThread->GetGrabbedMark(1, 0);
									pstGrab[1]	= g_pCamThread->GetGrabbedMark(1, 1);	break;
	case ENG_GMDD::en_rgt_vert	:	pstGrab[0]	= g_pCamThread->GetGrabbedMark(2, 0);
									pstGrab[1]	= g_pCamThread->GetGrabbedMark(2, 1);	break;
	case ENG_GMDD::en_lft_diag	:	pstGrab[0]	= g_pCamThread->GetGrabbedMark(1, 0);
									pstGrab[1]	= g_pCamThread->GetGrabbedMark(2, 1);	break;
	case ENG_GMDD::en_rgt_diag	:	pstGrab[0]	= g_pCamThread->GetGrabbedMark(1, 1);
									pstGrab[1]	= g_pCamThread->GetGrabbedMark(2, 0);	break;
	}
	if (!pstGrab[0] || !pstGrab[1])	return FALSE;

	/* �˻��� ��ũ�� ���� �� ��ȯ */
	//data1.x	= pstGrab[0]->mark_cent_mm_x;
	//data1.y	= pstGrab[0]->mark_cent_mm_y;
	//data2.x	= pstGrab[1]->mark_cent_mm_x;
	//data2.y	= pstGrab[1]->mark_cent_mm_y;
	data1.x = pstGrab[0]->move_mm_x;
	data1.y = pstGrab[0]->move_mm_y;
	data2.x = pstGrab[1]->move_mm_x;
	data2.y = pstGrab[1]->move_mm_y;

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
 parm : config		- [in]  ��ü ȯ�� ����
		shmem		- [in]  Shared Memory
		kind		- [in]  Vision Library ����
		work_dir	- [in]  �۾� ��� (���� ���� ���)
		time_out	- [in]  ���� ī�޶���� ���������� ������������ ����Ǿ��� ��,
							ī�޶� ���ο��� ���� ����� Ŭ���̾�Ʈ�� ������ ���� ��, ���� ��� �ð�
							Ŭ���̾�Ʈ�κ��� ���� ���� �ð�(����: �и���)���� ���� ������, ���� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_Init(LPG_CIEA config, LPG_VDSM shmem, ENG_VLPK kind, PTCHAR work_dir,
							  UINT64 time_out)
{
	UINT8 i	= 0;
	CHAR szPutEnv[128]	= {NULL};

	/* ��ü ȯ�� ���� ���� */
	g_pstConfig		= config;
	g_pstShMem		= shmem;
	g_enVisionLib	= kind;

	/* �۾� ��� ���� */
	wcscpy_s(g_tzWorkDir, MAX_PATH_LEN, work_dir);

	/* Vision : MIL / Cognex / OpenCV / Halcon */
	switch (kind)
	{
	case ENG_VLPK::en_vision_opencv	:	return FALSE;	break;
	case ENG_VLPK::en_vision_mil	:	if (!uvMIL_Init(config, shmem, work_dir))	return FALSE;	break;
	case ENG_VLPK::en_vision_cognex	:	return FALSE;	break;
	case ENG_VLPK::en_vision_halcon	:	return FALSE;	break;
	}

	/* ----------------------------------------------------------------------------------------- */
	/* GigE Type ��ī�޶� ����, Connection �� ���Ƿ� Disconnect �Ǿ��� ��� Disconnect ���Ŀ�  */
	/* ���� �ð��� ���� �� Connection�� �޾Ƶ��� �������� �����Ҽ��ִ�.(Debugging �ÿ� ������) */
	/* ���� ��ü������ ���� �帮�� ī�޶� ���� ���� �Լ� ������ ������ ���ᰡ �� ����� ���α� */
	/* ���� ���ᰡ �Ǵ��� ī�޶� ���� ������ ��� ��� �־ ���ο� ������ �� �� ���� ���°�*/
	/* �ǰ� �˴ϴ�.	�̶����� �Ʒ� �ڵ带 ī�޶� ���� �� �ʱ�ȭ �� ȣ���� �ּż� ���α׷� ������  */
	/* �� �ּž� �մϴ�. ���� �Ʒ� �ڵ带 ���ֽ��� �ʾҴٸ� ī�޶�� ������ ���� �ʰ� �����ڵ尡 */
	/* �� �� �ֽ��ϴ�.                                                                           */
	/* ----------------------------------------------------------------------------------------- */
	sprintf_s(szPutEnv, 128, "PYLON_GIGE_HEARTBEAT=%llu", time_out);
	_putenv(szPutEnv);	/* set to 5000 ms = 5 sec */
    // Before using any pylon methods, the pylon runtime must be initialized. 
    PylonInitialize();
	/* Grabbed Image ���� �� ���� ���� Ŭ���� ���� */
	g_pCamMain	= (CCamMain **) new CCamMain * [g_pstConfig->set_cams.acam_count];
	for (i=0; i<g_pstConfig->set_cams.acam_count; i++)
	{
		g_pCamMain[i]	= new CCamMain(config, i+1 /* Align Camera Index : 1 or Later */);
		ASSERT(g_pCamMain[i]);
	}
	/* Grabbed Image ó���ϴ� ������ */
	g_pCamThread	= new CCamThread(g_pstConfig->set_cams.acam_count,
									 g_pstConfig->mark_find.max_mark_grab,
									 g_pstShMem->link, g_pCamMain, g_pstConfig->mark_find.image_process);
	ASSERT(g_pCamThread);
	if (!g_pCamThread->CreateThread(0, 0, NORMAL_THREAD_SPEED))	return FALSE;

	return TRUE;
}

/*
 desc : BIPS Server�� ����� ����ϴ� Client ������ ���� �� ����
 parm : config		- [in]  ��ü ȯ�� ����
		shmem		- [in]  Shared Memory
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_Open(LPG_CIEA config, LPG_VDSM shmem)
{
	UINT8 i	= 0;
	CHAR szPutEnv[128]	= {NULL};

	/* ��ü ȯ�� ���� ���� */
	g_pstConfig	= config;
	g_pstShMem	= shmem;

	return TRUE;
}

/*
 desc : ���̺귯�� ����
 parm : None
 retn : None
*/
API_EXPORT VOID uvBasler_Close()
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
	if (g_pstConfig && g_pCamMain)
	{
		for (; i<g_pstConfig->set_cams.acam_count; i++)	delete g_pCamMain[i];
		delete [] g_pCamMain;
		g_pCamMain	= NULL;
	}
	// Releases all pylon resources. 
    PylonTerminate();

	/* Vision : MIL / Cognex / OpenCV / Halcon */
	switch (g_enVisionLib)
	{
	case ENG_VLPK::en_vision_opencv	:	break;
	case ENG_VLPK::en_vision_mil	:	uvMIL_Close();
	case ENG_VLPK::en_vision_cognex	:	break;
	case ENG_VLPK::en_vision_halcon	:	break;
	}
	/* �ݵ�� ����� �� */
	g_pstConfig	= NULL;
}

/*
 desc : ����� ī�޶� �κи� ������ ����
 parm : None
 retn : None
*/
API_EXPORT VOID uvBasler_Reconnected()
{
	g_pCamThread->Reconnected();
	uvMIL_ResetGrabAll();
}

/*
 desc : Mark Pattern ���� ���� �ڵ�
 parm : hwnd	- [in]  ��ũ ���� ���� �ڵ�
 retn : None
*/
API_EXPORT VOID uvBasler_SetMarkHwnd(HWND hwnd)
{
	if (!g_pCamThread)	return;
	g_csDrawMark.SetDrawHwnd(hwnd);
}

/*
 desc : BIPS Server�� ����� ����ϴ� Client ������ ���� �� ����
 parm : time_out	- [in]  ���� ī�޶���� ���������� ������������ ����Ǿ��� ��,
							ī�޶� ���ο��� ���� ����� Ŭ���̾�Ʈ�� ������ ���� ��, ���� ��� �ð�
							Ŭ���̾�Ʈ�κ��� ���� ���� �ð�(����: �и���)���� ���� ������, ���� ����
		work_dir	- [in]  �۾� ��� (���� ���� ���)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_UpdateSetParam(PTCHAR work_dir, UINT64 time_out)
{
	if (!g_pCamThread)	return	FALSE;
	uvBasler_Close();
	return uvBasler_Init(g_pstConfig, g_pstShMem, g_enVisionLib, work_dir, time_out);
}

/*
 desc : Mark Pattern ���
 parm : inner	- [in]  ���� ũ�� (����: pixel)
		outer	- [in]  �ܺ� ũ�� (����: pixel)
		reverse	- [in]  ��� ���� ���� (0x00 - In:White/Out:Black, 0x01 - In:Black/Out:White)
		pattern	- [in]  ��ũ ���� Ÿ�� (0x00 - Circle, 0x01 - Rectangle, 0x02 - Cross)
 retn : None
*/
API_EXPORT VOID uvBasler_CreateMark(LPRECT inner, LPRECT outer, UINT8 reverse, UINT8 pattern)
{
	if (!g_pCamThread)	return;
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
API_EXPORT BOOL uvBasler_SaveMark(TCHAR *file, LPRECT inner, LPRECT outer, UINT8 reverse, UINT8 pattern)
{
	if (!g_pCamThread)	return	FALSE;
	return g_csDrawMark.Save(file, inner, outer, reverse, pattern);
}

/*
 desc : Vision Camera Mode ����
 parm : mode	- [in]  ENG_VCCM::en_xxx
 retn : None
*/
API_EXPORT VOID uvBasler_SetCamMode(ENG_VCCM mode)
{
	UINT8 i	= 0;
	if (!g_pCamMain)	return;

#if 1
	for (; i<g_pstConfig->set_cams.acam_count; i++)
	{
		g_pCamMain[i]->SetCamMode(mode);
	}
#else
	g_pCamMain[0]->SetCamMode(mode);
#endif
}

/*
 desc : ���� Camera�� ���� ��� �� ��ȯ
 parm : None
 retn : ENG_VCCM::en_xxx
*/
API_EXPORT ENG_VCCM uvBasler_GetCamMode()
{
	return g_pCamMain == NULL ? ENG_VCCM::en_none : g_pCamMain[0]->GetCamMode();
}

/*
 desc : ���� PLC�� ����Ǿ� �ִ��� ���� Ȯ��
 ���� :	cam_id	- [in]  Camera Index (1 or 2)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_IsConnected(UINT8 cam_id)
{
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return FALSE;
	if (!g_pCamThread || !g_pCamMain)								return FALSE;

	return g_pCamMain[cam_id-1]->IsConnected();
}

/*
 desc : ���� ��� Align Camera�� ����Ǿ� �ִ��� ���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_IsConnectedAll()
{
	UINT8 i	= 0;

	if (!g_pCamMain)	return FALSE;
	for (; i<g_pstConfig->set_cams.acam_count; i++)
	{
		if (!g_pCamMain[i]->IsConnected())	return FALSE;
	}
	return TRUE;
}

/*
 desc : ���� ��ϵ� Grabbed �̹��� ���� ��ȯ
 parm : None
 retn : ����� �̹��� ���� ��ȯ
*/
API_EXPORT UINT16 uvBasler_GetGrabbedCount(int* CamNum = nullptr)
{
	if (!g_pCamThread)	return 0x0000;
	return g_pCamThread->GetGrabbedCount(CamNum);
}

/*
 desc : Grabbed Images ��� Score ���� ��ȿ���� �� ��ȯ
 parm : set_score	- [in]  If the score of the searched mark is higher than this set score, it is valid
  							(If 0.0f is unchecked) 
 retn : TRUE : ��ȿ��, FALSE : ��ȿ���� ����
*/
API_EXPORT BOOL uvBasler_IsScoreValidAll(DOUBLE set_score)
{
	return !g_pCamThread ? FALSE : g_pCamThread->IsScoreValidAll(set_score);
}

/*
 desc : Global Mark �������, 2 ���� �˻��� ��ũ ���� ��ȯ
 parm : direct	- [in]  �������� (0 - 1�� ��ũ�� 3�� ��ũ ���� ���� ��)
								 (1 - 2�� ��ũ�� 4�� ��ũ ���� ���� ��)
								 (2 - 1�� ��ũ�� 2�� ��ũ ���� ���� ��)
								 (3 - 3�� ��ũ�� 4�� ��ũ ���� ���� ��)
								 (4 - 1�� ��ũ�� 4�� ��ũ ���� ���� ��)
								 (5 - 2�� ��ũ�� 3�� ��ũ ���� ���� ��)
		data1	- [out] �˻��� ù ��° ��ũ�� �߽� ��ġ���� ���� �� (����: mm)
		data2	- [out] �˻��� �� ��° ��ũ�� �߽� ��ġ���� ���� �� (����: mm)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_GetGrabbedMarkDirect(ENG_GMDD direct, STG_DBXY &data1, STG_DBXY &data2)
{
	return GetGrabbedMarkDirect(direct, data1, data2);
}

/*
 desc : Global Mark �������, 2 ���� ��ũ�� ������ �Ÿ� �� ��ȯ (���� : mm)
 parm : direct	- [in]  �������� (0 - 1�� ��ũ�� 3�� ��ũ ���� ���� ��)
								 (1 - 2�� ��ũ�� 4�� ��ũ ���� ���� ��)
								 (2 - 1�� ��ũ�� 2�� ��ũ ���� ���� ��)
								 (3 - 3�� ��ũ�� 4�� ��ũ ���� ���� ��)
								 (4 - 1�� ��ũ�� 4�� ��ũ ���� ���� ��)
								 (5 - 2�� ��ũ�� 3�� ��ũ ���� ���� ��)
 retn : �� ��ũ�� ������ �Ÿ� (����: mm)
*/
API_EXPORT DOUBLE uvBasler_GetGrabbedMarkDist(ENG_GMDD direct)
{
	STG_DBXY stData[2]	= {NULL};

	if (!GetGrabbedMarkDirect(direct, stData[0], stData[1]))	return 0.0f;
	/* ���� �� ��ȯ */
	return sqrt(pow(stData[0].x - stData[1].x, 2) + pow(stData[0].y - stData[1].y, 2));
}

/*
 desc : �˻��� ��ũ �̹����� ���Ե� ����ü ������ ��ȯ
 parm : cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later) (if img_id == 0xff then Calibration Method)
 retn : ����ü ������ ��ȯ
*/
API_EXPORT LPG_ACGR uvBasler_GetGrabbedMark(UINT8 cam_id, UINT8 img_id)
{
	if (!g_pCamThread)	return	NULL;
	return g_pCamThread->GetGrabbedMark(cam_id, img_id);
}


/*
 desc : �˻��� ��ũ �̹����� ���Ե� ����ü ������ ��ȯ
 parm : cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later) (if img_id == 0xff then Calibration Method)
 retn : ����ü ������ ��ȯ
*/
API_EXPORT CAtlList <LPG_ACGR>*  uvBasler_GetGrabbedMarkAll()
{
	if (!g_pCamThread)	return	NULL;
	return g_pCamThread->GetGrabbedMarkAll();
}


API_EXPORT BOOL uvBasler_TryEnterCS()
{
	if (!g_pCamThread)	return	FALSE;
	return g_pCamThread->TryEnterCS();
}


API_EXPORT void uvBasler_ExitCS()
{
	if (!g_pCamThread)	return;
	g_pCamThread->ExitCS();
}


/*
 desc : Grabbed Image�� ������ ��ȯ
 parm : index	- [in]  ���������� �ϴ� ��ġ (Zero based)
 retn : grab data ��ȯ
*/
API_EXPORT LPG_ACGR uvBasler_GetGrabbedMarkIndex(UINT8 index)
{
	return !g_pCamThread ? NULL : g_pCamThread->GetGrabbedMarkIndex(index);
}

/*
 desc : Grabbed Image�� ��� ����
 parm : grab	- [in]  ����� ����ü ������
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_SetGrabbedMark(LPG_ACGR grab)
{
	if (!g_pCamThread || grab->cam_id < 1 || g_pstConfig->set_cams.acam_count < grab->cam_id)	return FALSE;
	return g_pCamThread->SetGrabbedMark(grab);
}

/*
 desc : ����ڿ� ���� �������� �˻��� ���, ��� ���� ó��
 parm : grab	- [in]  ����ڿ� ���� �������� �Էµ� grabbed image ��� ������ ����� ����ü ������
		gmfr	- [in]  GMFR Data
		gmsr	- [in]  GMSR Data
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_SetGrabbedMarkEx(LPG_ACGR grab, LPG_GMFR gmfr, LPG_GMSR gmsr)
{
	if (!g_pCamThread)	return FALSE;
	return g_pCamThread->SetGrabbedMark(grab, gmfr, gmsr);
}

/*
 desc : Grabbed Image�� ������ ����
 parm : index	- [in]  ���������� �ϴ� ��ġ (Zero based)
 parm : grab	- [in]  ����� ����ü ������
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_SetGrabbedMarkIndex(UINT8 index, LPG_ACGR grab)
{
	return !g_pCamThread ? FALSE : g_pCamThread->SetGrabbedMarkIndex(index, grab);
}

/*
 desc : Global 4���� Mark�� ���ؼ� ��� ��ȿ���� �˻�
 parm : multi_mark	- [in]  Multi Mark (���� ��ũ) ���� ����
		set_score	- [in]  If the score of the searched mark is higher than this set score, it is valid
		 					(If 0.0f is unchecked) 
 retn : TRUE (��ȿ��) or FALSE (4�� �� 1���� ����� �ν� �ȵ�)
*/
API_EXPORT BOOL uvBasler_IsGrabbedMarkValidAll(BOOL multi_mark, DOUBLE set_score, int* camNum)
{
	return !g_pCamThread ? FALSE : g_pCamThread->IsGrabbedMarkValidAll(multi_mark, set_score,camNum);
}

/*
 desc : Global 4���� Mark�� ���ؼ� ��� ��ȿ���� �˻�
 ���� :	cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : TRUE (��ȿ��) or FALSE (4�� �� 1���� ����� �ν� �ȵ�)
*/
API_EXPORT BOOL uvBasler_IsValidGrabMarkNum(UINT8 cam_id, UINT8 img_id)
{
	LPG_ACGR pstGrab	= NULL;

	if (!g_pCamThread)	return FALSE;
	pstGrab	= g_pCamThread->GetGrabbedMark(cam_id, img_id);
	if (!pstGrab)	return FALSE;
#if (USE_MARK_VALID_SCALE)
	if (!pstGrab->valid_scale || !pstGrab->valid_score || !pstGrab->valid_dist)	return FALSE;
#else
	if (!pstGrab->valid_score)	return FALSE;
#endif

	return TRUE;
}
#if 0
/*
 desc : ���� Grabbed Image�� ��ũ �˻� ���� �ʱ�ȭ
 parm : None
 retn : None
*/
API_EXPORT VOID uvBasler_ResetGrabbedMark()
{
	UINT8 i	= 0x00;

	if (!g_pCamMain)	return;
	for (; i<g_pstConfig->set_cams.acam_count; i++)	g_pCamMain[i]->ResetGrabbedMark();
}
#endif
/*
 desc : ���� Grabbed �̹��� ��� ����
 parm : None
 retn : None
*/
API_EXPORT VOID uvBasler_ResetGrabbedImage()
{
	if (!g_pCamThread)	return;
	ResetGrabbedImage();
}

/*
 desc : Calibration Image�� �����ϸ�, Align Mark ����
 ���� :	cam_id	- [in]  Camera Index (1 or 2)
		mode	- [in]  0xff : Calibration Mode (����� ī�޶� ���� �������� ����)
						0xfe : Align Camera Angle Measuring Mode (����� ī�޶� ���� �����ϱ� ����)
 retn : Calirbated Image�� ������ ����� ����ü ������ ��ȯ
*/
API_EXPORT LPG_ACGR uvBasler_RunModelCali(UINT8 cam_id, UINT8 mode, UINT8 dlg_id, UINT8 mark_no, BOOL useMilDisp, UINT8 img_proc,int flipDir)
{
	/* New Grabbed Image ���� */
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return NULL;
	return g_pCamThread->RunModelFind(cam_id, mode, dlg_id, mark_no, useMilDisp, img_proc,flipDir);
}

/*
 desc : Geometric Model Find
 parm : cam_id	- [in]  Camera Index (1 or 2)
 retn : TRUE or FALSE
*/
//API_EXPORT BOOL uvBasler_RunModelFind(UINT8 cam_id, UINT8 mark_no) // �̻��
//{
//	//LPG_ACGR pstGrab	= NULL;
//	//
//	//if (!g_pCamThread)	return FALSE;
//	//pstGrab	= g_pCamMain[cam_id-1]->GetGrabbedImage();
//	//if (!pstGrab)	return FALSE;
//	/* �˻� �۾� ���� */
//	//return uvMIL_RunModelFind(cam_id, pstGrab->img_id, pstGrab->img_id, pstGrab->grab_data, g_pCamMain[cam_id - 1]->GetDispType(), mark_no);
//	return FALSE;
//}

/*
 desc : ���� �ֱٿ� Calibrated�� ��� ������ ��ȯ
 parm : None
 retn : ��Ī�� ��� �� ��ȯ
*/
API_EXPORT LPG_ACGR uvBasler_GetLastGrabbedMark()
{
	if (!g_pCamThread)	return NULL;
	return g_pCamThread->GetLastGrabbedMark();
}

/*
 desc : ���� �ֱ� Grabbed Image�� ��Ī �˻� ��� ����ü ������ ��ȯ
 parm : None
 retn : ��ü ������ (��Ī�� ������ ��� NULL)
*/
API_EXPORT LPG_ACGR uvBasler_GetLastGrabbedMarkEx()
{
	return uvMIL_GetLastGrabbedMark();
}

/*
 desc : ���� �ֱٿ� ī�޶� ���� Calibrated�� ��� �� Error ���� ����� ������ ��ȯ
 parm : None
 retn : ��Ī�� ��� �� ��ȯ
*/
API_EXPORT LPG_ACLR uvBasler_GetLastGrabbedACam()
{
	if (!g_pCamThread)	return NULL;
	return g_pCamThread->GetLastGrabbedACam();
}

/*
 desc : Step (Vision ����) Image�� �����ϸ�, Align Mark ����
 ���� :	cam_id	- [in]  Camera Index (1 or 2)
		counts	- [in]  �˻� ����� Mark ���� (ã�����ϴ� ��ũ ����)
		angle	- [in]  ���� ���� ���� (TRUE : ���� ����, FALSE : ���� ���� ����)
						TRUE : ���� ī�޶��� ȸ���� ���� ���ϱ� ����, FALSE : ���� ���� �����Ͽ� ȸ���� ���� ����
		results	- [out] �˻��� ��� ������ ����� ����ü ���� ������ (grabbed image buffer���� �����ؿ�)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_RunModelStep(UINT8 cam_id, UINT16 count, BOOL angle, LPG_ACGR results, UINT8 dlg_id, UINT8 mark_no, UINT8 img_proc)
{
	if (!g_pCamThread)	return FALSE;
	return g_pCamThread->RunModelStep(cam_id, count, angle, results, dlg_id, mark_no, img_proc);
}

/*
 desc : Examination (Vision Align �˻� (����)) Image�� �����ϸ�, Align Shutting �˻� ����
 ���� :	score		- [in]  ũ�� ���� �� (�� ������ ũ�ų� ������ Okay)
		scale		- [in]  ��Ī ���� �� (1.0 �������� +/- �� �� ���� �ȿ� ������ Okay)
		results		- [out] �˻��� ��� ������ ����� ����ü ���� ������ (grabbed image buffer���� �����ؿ�)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_RunModelExam(DOUBLE score, DOUBLE scale, LPG_ACGR results)
{
	return g_pCamThread->RunModelExam(score, scale, results);
}

/*
 desc : Grabbed Image�� �����ϸ�, Edge Detection ����
 ���� :	cam_id	- [in]  Camera Index (1 or 2)
		saved	- [in]  �м��� edge image ���� ����
 retn : Edge Detected�� ������ ���� (����) �� ��ȯ (����: Pixel)
*/
API_EXPORT BOOL uvBasler_RunEdgeDetect(UINT8 cam_id, UINT8 saved)
{
	/* ���� Grabbed Image ���� */
	ResetGrabbedImage();
	/* New Grabbed Image ó�� */
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return FALSE;
	return g_pCamThread->RunEdgeDetect(cam_id, saved);
}

/*
 desc : Set the parameters of the strip maker and find in an image and take the specified measurements
 parm : cam_id	- [in]  Align Camera ID (1 or 2 or later)
		param	- [in]  Structures pointer with information to set up is stored
		result	- [out] Buffer in which the resulting value will be returned and stored.
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_RunMarkerStrip(UINT8 cam_id, LPG_MSMP param, STG_MSMR &results)
{
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return FALSE;
	return g_pCamThread->RunMarkerStrip(cam_id, param, results);
}

/*
 desc : ī�޶�� ĸó ���� ��, 1 �ȼ��� ���� ũ�� ��� (Width & Height ��)
 ���� :	cam_id	- [in]  Camera Index (1 or 2)
 retn : ũ�� �� ��ȯ (����: um)
*/
API_EXPORT DOUBLE uvBasler_CalcCamPixelToUM(UINT8 cam_id)
{
	return g_pstConfig->acam_spec.spec_pixel_um[cam_id-1];
}

/*
 desc : ���� Grabbed Image�� ���� �� Bytes ���� ����� Gray �� ���� ���
 parm : cam_id	- [in]  Camera Index (1 or 2)
		mode	- [in]  Grabbed Image�� ĸó�ϴ� ��� (���)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_SetGrabGrayRate(UINT8 cam_id, ENG_VCCM mode)
{
	if (!g_pCamMain)	return FALSE;
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return FALSE;
	return g_pCamMain[cam_id-1]->SetGrabGrayRate(mode);
}

/*
 desc : ���� Grabbed Image�� ���� �� Gray Level �� (����) ��ȯ
 parm : cam_id	- [in]  Camera Index (1 or 2)
		index	- [in]  0 ~ 255�� �ش�Ǵ� Gray Level Index
 retn : 0 ~ 255 ���� Grabbed Image���� ���� �� ��ȯ
*/
API_EXPORT DOUBLE uvBasler_GetGrabGrayRate(UINT8 cam_id, UINT8 index)
{
	if (!g_pCamMain)	return 0.0f;
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return 0.0f;
	return g_pCamMain[cam_id-1]->GetGrabGrayRate(index);
}

/*
 desc : ��ü Grabbed Image���� ������ �ִ� ���� ���� ��ȯ
 parm : cam_id	- [in]  Camera Index (1 or 2)
 retn : White ���� �� ��ȯ
*/
API_EXPORT DOUBLE uvBasler_GetGrabGrayRateTotal(UINT8 cam_id)
{
	if (!g_pCamMain)	return 0.0f;
	return g_pCamMain[cam_id-1]->GetGrabGrayRateTotal();
}

/*
 desc : ���� Grabbed Image�� ���� �� Bytes ���� ����� Gray �� ����
		���̿� ���� ���̿� �ִ� ��� Gray ���� �ջ��Ͽ� ����
 parm : cam_id	- [in]  Camera Index (1 or 2)
		mode	- [in]  Grabbed Image�� ĸó�ϴ� ��� (���)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_SetGrabHistLevel(UINT8 cam_id, ENG_VCCM mode)
{
	if (!g_pCamMain)	return FALSE;
	return g_pCamMain[cam_id-1]->SetGrabHistLevel(mode);
}

/*
 desc : ���� Grabbed Image�� ���� �� Bytes ���� ����� Gray Level ��ȯ
 parm : cam_id	- [in]  Camera Index (1 or 2)
 retn : ���� Grabbed Image�� ���� (Column)�� 1 �ȼ����� ������ Level ������ ����Ǿ� �ִ� ���� ��ȯ
*/
API_EXPORT PUINT64 uvBasler_GetGrabHistLevel(UINT8 cam_id)
{
	if (!g_pCamMain)	return NULL;
	return g_pCamMain[cam_id-1]->GetGrabHistLevel();
}

/*
 desc : ���� Grabbed Image�� ���� �� Bytes ���� ����� Gray Level���� �� �� �� ��ȯ
 parm : cam_id	- [in]  Camera Index (1 or 2)
 retn : Grabbed Image�� ��ü Gray Level ������ �� ��ȯ
*/
API_EXPORT UINT64 uvBasler_GetGrabHistTotal(UINT8 cam_id)
{
	if (!g_pCamMain)	return 0;
	return g_pCamMain[cam_id-1]->GetGrabHistTotal();
}

/*
 desc : ���� Grabbed Image�� ���� Column ���� Gray Level �� �� ���� ū ��
 parm : cam_id	- [in]  Camera Index (1 or 2)
 retn : Grabbed Image�� Column ���� Gray Level �� �� ���� ū �� ��ȯ
*/
API_EXPORT UINT64 uvBasler_GetGrabHistMax(UINT8 cam_id)
{
	if (!g_pCamMain)	return 0;
	return g_pCamMain[cam_id-1]->GetGrabHistMax();
}

/*
 desc : ����� ī�޶��� Gain Level �� ����
 parm : cam_id	- [in]  Camera Index (1 or 2)
		level	- [in] 0 ~ 255 (���� Ŭ���� ��� ó��)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_SetGainLevel(UINT8 cam_id, UINT8 level)
{
	if (!g_pCamMain)	return FALSE;
	if (!g_pCamMain[cam_id-1]->IsConnected())	return FALSE;
	return g_pCamMain[cam_id-1]->SetGainLevel(level);
}

/*
 desc : �������� �̹����� �ҷ��ͼ� �����͸� ���� �մϴ�. (���� ��忡�� �ַ� ��� ��)
 parm : cam_id	- [in]  Camera Index (1 or 2)
		file	- [in]  ��ü ��ΰ� ���Ե� 265 grayscale bitmap ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_LoadImageFromFile(UINT8 cam_id, PTCHAR file)
{
	return g_pCamMain[cam_id-1]->LoadImageFromFile(file, cam_id);
}


/* --------------------------------------------------------------------------------------------- */
/*                             ���� ó�� ���� (Gateway) �Լ� �κ�                                */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Outputs the most recently generated Buffer ID value to the window
 parm : hwnd	- [in]  Handle of window where the image is output
		draw	- [in]  The area of window where the image is output
		cam_id	- [in]  Align Camera ID (1 or 2)
		type	- [in]  0x00: Edge Detection, 0x01: Line Detection, 0x02: Strip Detection (Measurement)
 retn : None
*/
API_EXPORT VOID uvBasler_DrawMBufID(HWND hwnd, RECT draw, UINT8 cam_id, UINT8 type)
{
	if (!g_pCamThread)	return;
	uvMIL_DrawMBufID(hwnd, draw, cam_id, type);
}



/*
 desc : ���� Grabbed�� �̹��� ��� �� ���� (Bitmap�� �̿��Ͽ� ���)
 parm : hdc		- [in]  �̹����� ��� ��� context
		draw	- [in]  �̹����� ��µ� ���� (�θ� ���� ���� ��� ��ǥ)
		cam_id	- [in]  Camera Index (1 or 2)
 retn : None
*/
API_EXPORT VOID uvBasler_DrawGrabBitmap(HDC hdc, RECT draw, UINT8 cam_id)
{
	DOUBLE dbAngle	= g_pstConfig->set_cams.acam_inst_angle == 0 ? 0.0f : 180.0f;
	ENG_VCCM enMode	= g_pCamMain[cam_id-1]->GetCamMode();
	LPG_ACGR pstGrab= NULL;

	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)				return;
	if (ENG_VCCM::en_grab_mode != enMode && ENG_VCCM::en_hole_mode != enMode)	return;

	pstGrab	= g_pCamMain[cam_id-1]->GetGrabbedImage();
	if (!pstGrab)	return;
	// ���� Grabbed Image 1�� ���
	if (!pstGrab->grab_width || !pstGrab->grab_height)	return;
	
		/* ���� Grabbed Image ��� */
		uvMIL_DrawLiveBitmap(hdc, draw, pstGrab, dbAngle);
	
}

/*
 desc : Draw the strip information in the current grabbed image
 parm : hdc		- [in]  The handle of context
		draw	- [in]  The area in which the image is output
		cam_id	- [in]  Camera Index (1 or 2)
		param	- [in]  Strip output information
 retn : None
*/
API_EXPORT VOID uvBasler_DrawStripBitmap(HDC hdc, RECT draw, UINT8 cam_id, LPG_MSMP param)
{
	DOUBLE dbAngle	= g_pstConfig->set_cams.acam_inst_angle == 0 ? 0.0f : 180.0f;
	ENG_VCCM enMode	= g_pCamMain[cam_id-1]->GetCamMode();
	LPG_ACGR pstGrab= NULL;

	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)				return;
	if (ENG_VCCM::en_grab_mode != enMode && ENG_VCCM::en_hole_mode != enMode)	return;

	pstGrab	= g_pCamMain[cam_id-1]->GetGrabbedImage();
	if (!pstGrab)	return;
	// ���� Grabbed Image 1�� ���
	if (!pstGrab->grab_width || !pstGrab->grab_height)	return;

	/* ���� Grabbed Image ��� */
	uvMIL_DrawStripBitmap(hdc, draw, pstGrab, dbAngle, param);
}




/*
 desc : Calibration �̹��� (�˻��� ���)�� ���� ������ ��� ���� (Bitmap�� �̿��Ͽ� ���)
 parm : hdc		- [in]  �̹����� ��� ��� context
		draw	- [in]  �̹����� ��µ� ���� (�θ� ���� ���� ��� ��ǥ)
 retn : None
*/
API_EXPORT VOID uvBasler_DrawCaliMarkBitmap(HDC hdc, RECT draw)
{
	if (!g_pCamThread)	return;
	uvMIL_DrawCaliMarkBitmap(hdc, draw);
}

API_EXPORT VOID uvBasler_Camera_SetAlignMotionPtr(AlignMotion& ptr)
{
	if (!g_pCamThread)	return;

	g_pCamThread->SetAlignMotionPtr(ptr);
	
	uvMIL_SetAlignMotionPtr(ptr);
	
}


/*
 desc : ���� �������� Grabbed Image�� ���
 parm : hdc		- [in]  �̹����� ��� ��� context
		draw	- [in]  �̹����� ��µ� ���� (�θ� ���� ���� ��� ��ǥ)
 retn : None
*/
API_EXPORT VOID uvBasler_DrawLastGrabImage(HDC hdc, RECT draw)
{
	if (!g_pCamThread)	return;
	uvMIL_DrawCaliMarkBitmap(hdc, draw);
}

/*
 desc : �˻��� Mark �̹��� ���� ������ ��� ���� (DC�� �̿��Ͽ� bitmap �̹����� ���)
 parm : hdc		- [in]  �̹����� ��� ��� context
		draw	- [in]  �̹����� ��µ� ���� (�θ� ���� ���� ��� ��ǥ)
		cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_DrawMarkBitmap(HDC hdc, RECT draw, UINT8 cam_id, UINT8 img_id)
{
	UINT8 u8FindMark	= 0x00;

	if (!g_pCamThread)	return FALSE;
	/* ������ Camera ���� ��ȿ�� Ȯ�� */
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return FALSE;
	/* �˻��� Mark�� ��ȿ���� ���� */
	u8FindMark	= IsValidGrabMarkNum(cam_id, img_id) ? 0x01 : 0x00;
	//u8FindMark = 0x01; // lk91!! �̹��� test �� ���� ���!! �ּ�!!!
	/* Grabbed Image�� ���� Mark ���� �׸��� */
	return uvMIL_DrawMarkBitmap(hdc, draw, cam_id, img_id, u8FindMark);
}

/*
 desc : �˻��� Mark �̹��� ���� ������ ��� ���� (MIL Buffer ID�� �̿��� ���)
 parm : hdc		- [in]  �̹����� ��� ��� context
		draw	- [in]  �̹����� ��µ� ���� (�θ� ���� ���� ��� ��ǥ)
		cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_DrawMarkMBufID(HWND hwnd, RECT draw, UINT8 cam_id,UINT8 hwndIdx, UINT8 img_id)
{
	if (!g_pCamThread)	return FALSE;
	/* ������ Camera ���� ��ȿ�� Ȯ�� */
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return FALSE;
	/* Grabbed Image�� ���� Mark ���� �׸��� */
	uvMIL_DrawMarkMBufID(hwnd, draw, cam_id, hwndIdx,img_id);

	return TRUE;
}

/*
 desc : Calibration �̹��� ���� ������ ��� ���� (Bitmap�� �̿��Ͽ� ���)
 parm : hdc		- [in]  �̹����� ��� ��� context
		draw	- [in]  �̹����� ��µ� ���� (�θ� ���� ���� ��� ��ǥ)
		grab	- [in]  grab �̹����� ����� ����ü ������ (grab_data�� �̹����� ������ �ݵ�� ����Ǿ� �־�� ��)
		find	- [in]  ��ũ �˻� ���� ���� (�˻��ƴ���, �˻� ��� ���� ���� �޶���)
						0x00 - �˻� ��� ����, 0x01 - �˻� ��� ����
 retn : None
*/
API_EXPORT VOID uvBasler_DrawMarkDataBitmap(HDC hdc, RECT draw, LPG_ACGR grab, UINT8 find,bool drawForce,UINT8 flipFlag)
{
	if (!g_pCamThread)	return;
	uvMIL_DrawMarkDataBitmap(hdc, draw, grab, find,drawForce, flipFlag);
}

/*
 desc : Drawing - Examination Object Image (Bitmap�� �̿��Ͽ� ���)
 parm : hdc		- [in]  �̹����� ��� ��� context
		draw	- [in]  �̹����� ��µ� ���� (�θ� ���� ���� ��� ��ǥ)
		grab_out- [in]  Grabbed Image ������ ����� ����ü ������ (�ٱ� ��)
		grab_in	- [in]  Grabbed Image ������ ����� ����ü ������ (���� ��)
 retn : None
*/
API_EXPORT VOID uvBasler_DrawGrabExamBitmap(HDC hdc, RECT draw, LPG_ACGR grab_out, LPG_ACGR grab_in)
{
	if (!g_pCamThread)	return;
	uvMIL_DrawGrabExamBitmap(hdc, draw, grab_out, grab_in);
}

/*
 desc : Mark ���� ����
 parm : cam_id		- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		detail_level- [in]  ��ϵ� �� �̹����� Grabbed Image�̹����� Edge ������ ���� �� ���� ��
							���� Ŭ���� ���� ��������, �ӵ��� ���� ��������
							LOW���� HIGH�� ������ �� ���� Edge�� ������ ���ϰ� ��
							0 (M_MEDIUM), 1 (M_HIGH), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - �⺻ ��, 1.0f ~ 100.0f
							�⺻ ���� 50������, 100���� ������ ������ ������ ���͸� ��Ÿ���ϴ�.
		model		- [in]  Model Type, Param 1 ~ 5�� �������� ��� ���Ե� ����ü ������
		count		- [in]  ����ϰ��� �ϴ� ���� ����	
		scale_min	- [in]  �˻� ����� ũ�� ���� �� ���� (�ּ��� �� �� �������� Ŀ�� ��. ����: 0.5 ~ 1.0)
		scale_max	- [in]  �˻� ����� ũ�� ���� �� ���� (�ִ��� �� �� �������� �۾ƾ� ��. ����: 1.0 ~ 1.0)
		score_min	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0)
		score_tgt	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0))
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_SetModelDefine(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
										LPG_CMPV model, UINT8 mark_no,
										DOUBLE scale_min, DOUBLE scale_max,
										DOUBLE score_min, DOUBLE score_tgt)
{
	return uvMIL_SetModelDefine(cam_id, speed, level, count, smooth, model, mark_no,
								scale_min, scale_max, score_min, score_tgt);
}

/*
 desc : ���� �ý��� (�����)�� ����Ǿ� �ִ� ���� ��Ī ���� �̹��� ����
 parm : cam_id		- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		detail_level- [in]  ��ϵ� �� �̹����� Grabbed Image�̹����� Edge ������ ���� �� ���� ��
							���� Ŭ���� ���� ��������, �ӵ��� ���� ��������
							LOW���� HIGH�� ������ �� ���� Edge�� ������ ���ϰ� ��
							0 (M_MEDIUM), 1 (M_HIGH), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - �⺻ ��, 1.0f ~ 100.0f
							�⺻ ���� 50������, 100���� ������ ������ ������ ���͸� ��Ÿ���ϴ�.
		model		- [in]  Model Type ��, circle, square, rectangle, cross, diamond, triangle
		param		- [in]  �� 5���� Parameter Values (unit : um)
		count		- [in]  ����ϰ��� �ϴ� ���� ����	
		scale_min	- [in]  �˻� ����� ũ�� ���� �� ���� (�ּ��� �� �� �������� Ŀ�� ��. ����: 0.5 ~ 1.0)
		scale_max	- [in]  �˻� ����� ũ�� ���� �� ���� (�ִ��� �� �� �������� �۾ƾ� ��. ����: 1.0 ~ 1.0)
		score_min	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0)
		score_tgt	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0))
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_SetModelDefineEx(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
	PUINT32 model, DOUBLE* param1, DOUBLE* param2, DOUBLE* param3,
	DOUBLE* param4, DOUBLE* param5, UINT8 mark_no,
	DOUBLE scale_min, DOUBLE scale_max,
	DOUBLE score_min, DOUBLE score_tgt, bool sameMark)
{
	if (!g_pCamThread)	return FALSE;
	if (!uvMIL_SetModelDefineEx(cam_id, speed, level, count, smooth, model, param1, param2, param3,
		param4, param5, mark_no,
		scale_min, scale_max, score_min, score_tgt, sameMark))	return FALSE;
	return TRUE;
}

/*
 desc : Mark ���� ���� - �̹��� ������
 parm : cam_id		- [in]  Align Camera Index (1 or 2)
		speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		level		- [in]  ��ϵ� �� �̹����� Grabbed Image�̹����� Edge ������ ���� �� ���� ��
							���� Ŭ���� ���� ��������, �ӵ��� ���� ��������
							LOW���� HIGH�� ������ �� ���� Edge�� ������ ���ϰ� ��
							0 (M_MEDIUM), 1 (M_HIGH), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - �⺻ ��, 1.0f ~ 100.0f
							�⺻ ���� 50������, 100���� ������ ������ ������ ���͸� ��Ÿ���ϴ�.
		scale_min	- [in]  �˻� ����� ũ�� ���� �� ���� (�ּ��� �� �� �������� Ŀ�� ��. ����: 0.5 ~ 1.0)
		scale_max	- [in]  �˻� ����� ũ�� ���� �� ���� (�ִ��� �� �� �������� �۾ƾ� ��. ����: 1.0 ~ 1.0)
		score_min	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0)
		score_tgt	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0))
		name		- [in]  Model Name
		file		- [in]  �� �̹����� ����� ���� �̸� (��ü ��� ����. Image File)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_SetModelDefineLoad(UINT8 cam_id, UINT8 speed, UINT8 level, DOUBLE smooth,
											DOUBLE scale_min, DOUBLE scale_max,
											DOUBLE score_min, DOUBLE score_tgt,
											PTCHAR name, CStringArray &file)
{
	if (!g_pCamThread)	return FALSE;
	return uvMIL_SetModelDefineLoad(cam_id, speed, level, smooth,
									scale_min, scale_max, score_min, score_tgt, name, file);
}

/*
 desc : ���� ��ϵ� Mark Model ���� ��ȯ
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : ��ϵ� ����ü ������ ��ȯ
*/
API_EXPORT LPG_CMPV uvBasler_GetModelDefine(UINT8 cam_id)
{
	return uvMIL_GetModelDefine(cam_id);
}

/*
 desc : ���� �˻��� ���� ��ϵ� ���� ���� ��ȯ
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : ��ϵ� ����
*/
API_EXPORT UINT32 uvBasler_GetModelRegistCount(UINT8 cam_id)
{
	if (!g_pCamThread)	return 0;
	return uvMIL_GetModelRegistCount(cam_id);
}

/*
 desc : ������ ���� (���)�Ǿ� �ִ� ��� Mark ���� ����
 parm : None
 retn : None
*/
API_EXPORT VOID uvBasler_ResetMarkModel()
{
	if (!g_pCamThread)	return;
	uvMIL_ResetMarkModel();
}

/*
 desc : Mark Template�� ��ϵǾ� �ִ��� Ȯ��
 parm : mode	- [in]  0x00 : Expose, 0x01 : Vstep
		cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : TRUE - ��ϵǾ� �ִ�. FALSE - ��ϵ��� �ʾҴ�.
*/
API_EXPORT BOOL uvBasler_IsSetMarkModel(UINT8 mode, UINT8 cam_id, UINT8 fi_No = 0x00)
{
	if (!g_pCamThread)	return FALSE;
	return uvMIL_IsSetMarkModel(mode, cam_id, fi_No);
}

/*
 desc : Mark Pattern ��� ����
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_IsSetMarkModelACam(UINT8 cam_id, UINT8 mark_no)
{
	if (!g_pCamThread)	return FALSE;
	return uvMIL_IsSetMarkModelACam(cam_id, mark_no);
}

/*
 desc : ��� ī�޶� (ī�޶� ������ ����)�� Mark Model�� �����Ǿ����� ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_IsSetMarkModelAll(UINT8 mark_no)
{
	UINT8 i	= 0x00;

	if (!g_pCamThread)	return FALSE;
	for (; i<g_pstConfig->set_cams.acam_count; i++)
	{
		if (!uvMIL_IsSetMarkModelACam(i+1, mark_no))	return FALSE;
	}
	return TRUE;
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
API_EXPORT BOOL uvBasler_SaveGrabbedMarkToFile(UINT8 cam_id, LPRECT area, UINT8 type, PTCHAR file)
{
	if (!g_pCamThread)	return FALSE;
	return uvMIL_SaveGrabbedMarkToFile(cam_id, area, type, file);
}

/*
 desc : Edge Detection �˻��� ��� ��ȯ
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
 retn : ��� ���� ����� ������ ��ȯ
*/
API_EXPORT LPG_EDFR uvBasler_GetEdgeDetectResults(UINT8 cam_id)
{
	if (!g_pCamThread)	return NULL;
	return uvMIL_GetEdgeDetectResults(cam_id);
}

/*
 desc : Edge Detection �˻��� ���� ��ȯ
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
 ��ȯ : ���� ��ȯ. ������ ���, 0 ���� ��
*/
API_EXPORT INT32 uvBasler_GetEdgeDetectCount(UINT8 cam_id)
{
	if (!g_pCamThread)	return -1;
	return uvMIL_GetEdgeDetectCount(cam_id);
}

/*
 desc : ���� ��ϵ� Mark Model Type ��ȯ
 parm : cam_id	- [in]  Align Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		mark_id	- [in]  Align Mark Index (0x00 ~ MAX_REGIST_MODEL-1)
 retn : Model Type ��
*/
API_EXPORT UINT32 uvBasler_GetMarkModelType(UINT8 cam_id, UINT8 index)
{
	if (!g_pCamThread)	return 0;
	return uvMIL_GetMarkModelType(cam_id, index);
}

/*
 desc : �� ũ�� ��ȯ
 parm : cam_id	- [in]  Grabbed Image ������ �߻��� Align Camera Index (1 or 2)
		index	- [in]  ��û�ϰ��� �ϴ� ���� ��ġ (Zero-based)
		flag	- [in]  0x00 : ����  ũ��, 0x01 : ���� ũ��
		unit	- [in]  0x00 : um, 0x01 : pixel
 retn : ũ�� ��ȯ (����: um)
*/
API_EXPORT DOUBLE uvBasler_GetMarkModelSize(UINT8 cam_id, UINT8 index, UINT8 flag, UINT8 unit)
{
	if (!g_pCamThread)	return 0.0f;
	return uvMIL_GetMarkModelSize(cam_id, index, flag, unit);
}

/*
 desc : �̹����� ���� Ȥ�� ������ ��輱 �߽� ��ġ ���
 parm : cam_id		- [in]  Align Camera ID (1 or 2)
		grab_data	- [in]  Grabbed Image ������ ����� ����ü ������
		mil_result	- [in]  MIL ���� �˻� ��� ��ȭ ���� ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBasler_RunLineCentXY(UINT8 cam_id, LPG_ACGR grab_data, BOOL mil_result)
{
	return uvMIL_RunLineCentXY(cam_id, grab_data, mil_result);
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
API_EXPORT VOID uvBasler_ImageRotate(PUINT8 img_src, PUINT8 img_dst,
									 UINT32 width, UINT32 height, UINT32 angle)
{
	if (!g_pCamThread)	return;
	uvMIL_ImageRotate(img_src, img_dst, width, height, angle);
}

/*
 desc : ���� MIL ���̼����� ��ȿ���� ���� Ȯ��
		Pattern, Model Finder, Edge Finder, Read, OCR, Image Processing �� ��ȿ���� Ȯ��
 parm : None
 retn : TRUE or FALS
*/
API_EXPORT BOOL uvBasler_IsLicenseValid()
{
	if (!g_pCamThread)	return FALSE;
	return uvMIL_IsLicenseValid();
}

/*
 desc : Align Mark �˻� ��� ����
 parm : method	- [in]  0x00 : ������ 1�� ��ȯ, 0x01 : ���� ��ũ �������� ����ġ�� �ο��Ͽ� ��� ��ȯ
		count	- [in]  'method' ���� 1�� ���, ���� �˻��� Mark ���� �� 2 �̻� ���̾�� ��
 retn : None
*/
API_EXPORT VOID uvBasler_SetMarkMethod(ENG_MMSM method, UINT8 count)
{
	uvMIL_SetMarkMethod(method, count);
}


API_EXPORT UINT8 uvBasler_SetMarkFoundCount(int camNum)
{
	return uvMIL_SetMarkFoundCount(camNum);
}


/*
 desc : �뱤 ��� ���� ��, ���� �뱤, ����� �뱤, ���� �� ����� �뱤
 parm : mode	- [in]  ���� �뱤 (0x00), ����� �뱤 (0x01), ����� ī�޶� ���� �� ���� �� ����� �뱤 (0x02)
 retn : None
*/
API_EXPORT VOID uvBasler_SetAlignMode(ENG_AOEM mode)
{
	if (!g_pCamThread)	return;
	uvMIL_SetAlignMode(mode);
}

/*
 desc : ���� ����� �������� ��ũ �˻� ���� �� ����
 parm : score	- [in]  Score Accpetance (��� �̹������� �˻��� ��ũ�� Score ���� �� ������ ������ ������)
		scale	- [in]  Scale Range (��� �̹������� ��ϵ� ��ũ�� �˻� ����� �߿��� �� ���� ���� ���� )
									(���Ե� ����鸸 ��ȿ�ϰ�, �� �߿��� Score ���� ���� ���� �͸� ����)
									(����. �� ���� 50.0000 �̸�, ���� �˻��� ���Ǵ� ��ũ�� ũ�� ����)
									(�˻����� = 1.0f - (100.0 - 50.0) / 100 �� ��, 0.5 ~ 1.5 ��)
 retn : None
*/
API_EXPORT VOID uvBasler_SetRecipeMarkRate(DOUBLE score, DOUBLE scale)
{
	return uvMIL_SetRecipeMarkRate(score, scale);
}

/*
 desc : ���������� �̵� ���� ���� (������ �̵����� / ������ �̵����� ����)
 parm : direct	- [in]  TRUE (������ �̵� : �տ��� �ڷ� �̵�), FALSE (������ �̵�)
 retn : None
*/
API_EXPORT VOID uvBasler_SetMoveStateDirect(BOOL direct)
{
	uvMIL_SetMoveStateDirect(direct);
}

/*
 desc : Grabbed Image ���� ǥ���� ��ũ�� ������ ���� ���� ���, ��ũ ���� ������ ������ ũ�� �� ����
 parm : width	- [in]  �簢�� ������ ���� �� (����: um)
		height	- [in]  �簢�� ������ ���� �� (����: um)
 retn : None
*/
API_EXPORT VOID uvBasler_SetMultiMarkArea(UINT32 width, UINT32 height)
{
	uvMIL_SetMultiMarkArea(width, height);
}

/* --------------------------------------------------------------------------------------------- */
/*                                 lk91 VISION �߰� �Լ�                                         */
/* --------------------------------------------------------------------------------------------- */

/* desc : Mark ���� ���� - MMF (MIL Model Find File) */
API_EXPORT BOOL uvBasler_SetModelDefineMMF(UINT8 cam_id, PTCHAR name, PTCHAR mmf, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no)
{
	if (!g_pCamThread)	return FALSE;
	return uvMIL_SetModelDefineMMF(cam_id, name, mmf, m_MarkSizeP, m_MarkCenterP, mark_no);
}

/* desc : Mark ���� ���� - PAT (MIL Pattern Matching File) */
API_EXPORT BOOL uvBasler_SetModelDefinePAT(UINT8 cam_id, PTCHAR name, PTCHAR pat, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no)
{
	if (!g_pCamThread)	return FALSE;
	return uvMIL_SetModelDefinePAT(cam_id, name, pat, m_MarkSizeP, m_MarkCenterP, mark_no);
}

/* desc : Set Display Type */
API_EXPORT VOID uvBasler_SetDispType(UINT8 dispType)
{
	UINT8 i = 0;
	if (!g_pCamMain)	return;

#if 1
	for (; i < g_pstConfig->set_cams.acam_count; i++)
	{
		g_pCamMain[i]->SetDispType(dispType);
	}
#else
	g_pCamMain[0]->SetCamMode(mode);
#endif
}

/* desc : MarkROI �ʱ�ȭ */
API_EXPORT VOID uvBasler_initMarkROI(LPG_CRD fi_MarkROI)
{
	uvMIL_InitMarkROI(fi_MarkROI);
}

/* desc : Set Live Disp Size */
API_EXPORT VOID uvBasler_SetMarkLiveDispSize(CSize fi_size)
{
	uvMIL_SetMarkLiveDispSize(fi_size);
}

/* desc : Set CalbCamSpec Disp Size */
API_EXPORT VOID uvBasler_SetCalbCamSpecDispSize(CSize fi_size)
{
	uvMIL_SetCalbCamSpecDispSize(fi_size);
}

API_EXPORT VOID uvBasler_SetAccuracyMeasureDispSize(CSize fi_size)
{
	uvMIL_SetAccuracyMeasureDispSize(fi_size);
}

/* desc : Set CalbStep Disp Size */
API_EXPORT VOID uvBasler_SetCalbStepDispSize(CSize fi_size)
{
	uvMIL_SetCalbStepDispSize(fi_size);
}

/* desc : Set MMPM Disp Size */
API_EXPORT VOID uvBasler_SetMMPMDispSize(CSize fi_size)
{
	uvMIL_SetMMPMDispSize(fi_size);
}

/* desc : ���� �˻��� Live �̹��� ���� ������ ��� ���� (Bitmap�� �̿��Ͽ� ���) */
API_EXPORT VOID uvBasler_DrawLiveBitmap(HDC hdc, RECT draw, UINT8 cam_id, BOOL save)
{
	DOUBLE dbAngle = g_pstConfig->set_cams.acam_inst_angle == 0 ? 0.0f : 180.0f;
	LPG_ACGR pstGrab = NULL;

	if (!g_pCamThread)	return;
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return;
	if (ENG_VCCM::en_grab_mode != g_pCamMain[cam_id - 1]->GetCamMode())
	{
		pstGrab = g_pCamMain[cam_id - 1]->GetGrabbedImage();
		if (!pstGrab)	return;
		// ���� Grabbed Image 1�� ���
		if (pstGrab->grab_width && pstGrab->grab_height)
		{
			/* ������ �׷ȴ� �ð��� ���ؼ� ���� �ð� �������� Ȯ�� */
			if (GetTickCount64() > g_u64DrawDelayTime[cam_id - 1] + GRAB_DRAW_DELAY_TIME)
			{
				uvMIL_DrawLiveBitmap(hdc, draw, pstGrab, dbAngle);
				/* ������ �׸� ��, ���� �ð� ������ ���ؼ� */
				g_u64DrawDelayTime[cam_id - 1] = GetTickCount64();

				/* ���Ϸ� ���� ���� */
				if (save)
				{
					TCHAR tzFile[MAX_PATH_LEN] = { NULL };
					/* ���� �ð� ��� */
					SYSTEMTIME stTime = { NULL };
					GetLocalTime(&stTime);
					/* Live ���� ���� */
					swprintf_s(tzFile, L"%s\\live\\%04d-%02d-%02d %02d%02d%02d.bmp",
						g_tzWorkDir, stTime.wYear, stTime.wMonth, stTime.wDay,
						stTime.wHour, stTime.wMinute, stTime.wSecond);
					uvMIL_SaveGrabbedToFile(tzFile, pstGrab->grab_width, pstGrab->grab_height, pstGrab->grab_data);
				}
			}
		}
	}
}

/* desc : MIL ID �� ����� �̹��� ȭ�� ��� */
API_EXPORT VOID uvBasler_DrawImageBitmap(int dispType, int Num, UINT8 cam_id, BOOL save,int flipDir)
{
	DOUBLE dbAngle = g_pstConfig->set_cams.acam_inst_angle == 0 ? 0.0f : 180.0f;
	LPG_ACGR pstGrab = NULL;

	if (!g_pCamThread)	return;
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return;
	//if (ENG_VCCM::en_image_mode == g_pCamMain[cam_id - 1]->GetCamMode())
	{
		pstGrab = g_pCamMain[cam_id - 1]->GetGrabbedImage();
		if (!pstGrab)	return;
		// ���� Grabbed Image 1�� ���
		if (pstGrab->grab_width && pstGrab->grab_height)
		{
			/* ������ �׷ȴ� �ð��� ���ؼ� ���� �ð� �������� Ȯ�� */
			//if (GetTickCount64() > g_u64DrawDelayTime[cam_id - 1] + GRAB_DRAW_DELAY_TIME)
			//{

			uvMIL_DrawImageBitmapFlip(dispType, 0, pstGrab, dbAngle, cam_id - 1,flipDir);
			
			/* ������ �׸� ��, ���� �ð� ������ ���ؼ� */
			//g_u64DrawDelayTime[cam_id - 1] = GetTickCount64();

			/* ���Ϸ� ���� ���� */
			if (save)
			{
				TCHAR tzFile[MAX_PATH_LEN] = { NULL };
				/* ���� �ð� ��� */
				SYSTEMTIME stTime = { NULL };
				GetLocalTime(&stTime);
				/* Live ���� ���� */
				swprintf_s(tzFile, L"%s\\live\\%04d-%02d-%02d %02d%02d%02d.bmp",
					g_tzWorkDir, stTime.wYear, stTime.wMonth, stTime.wDay,
					stTime.wHour, stTime.wMinute, stTime.wSecond);
				uvMIL_SaveGrabbedToFile(tzFile, pstGrab->grab_width, pstGrab->grab_height, pstGrab->grab_data);
			}
			//}
		}
	}
}

/* desc : Set Mark ROI */
API_EXPORT VOID uvBasler_MilSetMarkROI(UINT8 cam_id, CRect fi_rectSearhROI)
{
	uvMIL_MilSetMarkROI(cam_id, fi_rectSearhROI);
}

/* desc: pat Mark ��� */
API_EXPORT BOOL uvBasler_RegistPat(UINT8 cam_id, CRect fi_rectArea, CString fi_filename, UINT8 mark_no)
{
	LPG_ACGR pstGrab = NULL;
	pstGrab = g_pCamMain[cam_id - 1]->GetGrabbedImage();
	if (!pstGrab)
		return FALSE;
	return uvMIL_RegistPat(cam_id, pstGrab, fi_rectArea, fi_filename, mark_no);
}

/* desc: mmf Mark ��� */
API_EXPORT BOOL uvBasler_RegistMod(UINT8 cam_id, CRect fi_rectArea, CString fi_filename, UINT8 mark_no)
{
	LPG_ACGR pstGrab = NULL;
	pstGrab = g_pCamMain[cam_id - 1]->GetGrabbedImage();
	if (!pstGrab)
		return FALSE;
	return uvMIL_RegistMod(cam_id, pstGrab, fi_rectArea, fi_filename, mark_no);
}

/* desc: MMPM AutoCenter �̹��� ��� */
API_EXPORT BOOL uvBasler_RegistMMPM_AutoCenter(CRect fi_rectArea, UINT8 cam_id, UINT8 img_id)
{
	return uvMIL_RegistMMPM_AutoCenter(fi_rectArea, cam_id, img_id);
}

/* desc: Mark Size, Offset �ʱ�ȭ */
API_EXPORT VOID uvBasler_InitSetMarkSizeOffset(UINT8 cam_id, TCHAR* file, UINT8 fi_findType, UINT8 mark_no)
{
	uvMIL_InitSetMarkSizeOffset(cam_id, file, fi_findType, mark_no);
}

/* desc: Draw Mark Display */
API_EXPORT VOID uvBasler_PutMarkDisp(HWND hwnd, int fi_iNo, RECT draw, UINT8 cam_id, TCHAR* file, int fi_findType)
{
	uvMIL_PutMarkDisp(hwnd, fi_iNo, draw, cam_id, file, fi_findType);
}

/* desc: Draw Set Mark Display */
API_EXPORT BOOL uvBasler_PutSetMarkDisp(HWND hwnd, RECT draw, UINT8 cam_id, TCHAR* file, int fi_findType, DOUBLE fi_dRate)
{
	return uvMIL_PutSetMarkDisp(hwnd, draw, cam_id, file, fi_findType, fi_dRate);
}

/* desc: Get Mark Size */
API_EXPORT CPoint uvBasler_GetMarkSize(UINT8 cam_id, int fi_No)
{
	return uvMIL_GetMarkSize(cam_id, fi_No);
}

/* desc: Get Mark Setting Disp Rate */
API_EXPORT DOUBLE uvBasler_GetMarkSetDispRate()
{
	return uvMIL_GetMarkSetDispRate();
}

/* desc: Set Mark Setting Disp Rate */
API_EXPORT DOUBLE uvBasler_SetMarkSetDispRate(DOUBLE fi_dRate)
{
	return uvMIL_SetMarkSetDispRate(fi_dRate);
}

/* desc: Get Mark Offset */
API_EXPORT CPoint uvBasler_GetMarkOffset(UINT8 cam_id, BOOL bNewOffset, int fi_No)
{
	return uvMIL_GetMarkOffset(cam_id, bNewOffset, fi_No);
}

/* desc: Get Mask Buf */
API_EXPORT BOOL uvBasler_MaskBufGet()
{
	return uvMIL_MaskBufGet();
}

/* desc: Get Mark Find Mode */
API_EXPORT UINT8 uvBasler_GetMarkFindMode(UINT8 cam_id, UINT8 mark_no)
{
	return uvMIL_GetMarkFindMode(cam_id, mark_no);
}

/* desc: Set Mark Find Mode */
API_EXPORT BOOL uvBasler_SetMarkFindMode(UINT8 cam_id, UINT8 find_mode, UINT8 mark_no)
{
	return uvMIL_SetMarkFindMode(cam_id, find_mode, mark_no);
}

/* desc: Clear Mask(MMF) */
API_EXPORT VOID uvBasler_MaskClear_MOD(UINT8 cam_id, CPoint fi_iSizeP, UINT8 mark_no)
{
	return uvMIL_MaskClear_MOD(cam_id, fi_iSizeP, mark_no);
}

/* desc: Clear Mask(PAT) */
API_EXPORT VOID uvBasler_MaskClear_PAT(UINT8 cam_id, CPoint fi_iSizeP, UINT8 mark_no)
{
	return uvMIL_MaskClear_PAT(cam_id, fi_iSizeP, mark_no);
}

/* desc: Find Center (Mark Set������ ���) */
API_EXPORT VOID uvBasler_CenterFind(int cam_id, int fi_length, int fi_curSmoothness, double* fi_NumEdgeMIN_X, double* fi_NumEdgeMAX_X, double* fi_NumEdgeMIN_Y, double* fi_NumEdgeMAX_Y, int* fi_NumEdgeFound, int fi_Mode)
{
	return uvMIL_CenterFind(cam_id, fi_length, fi_curSmoothness, fi_NumEdgeMIN_X, fi_NumEdgeMAX_X, fi_NumEdgeMIN_Y, fi_NumEdgeMAX_Y, fi_NumEdgeFound, fi_Mode);
}

/* desc: Set Mark Size */
API_EXPORT VOID uvBasler_SetMarkSize(UINT8 cam_id, CPoint fi_MarkSize, int setMode, int fi_No)
{
	uvMIL_SetMarkSize(cam_id, fi_MarkSize, setMode, fi_No);
}

/* desc: Set Mark Offset */
API_EXPORT VOID uvBasler_SetMarkOffset(UINT8 cam_id, CPoint fi_MarkCenter, int setOffsetMode, int fi_No)
{
	uvMIL_SetMarkOffset(cam_id, fi_MarkCenter, setOffsetMode, fi_No);
}

/* desc: MMF ���Ͽ� MASK ���� ����, Mark Set������ ��� */
API_EXPORT VOID uvBasler_SaveMask_MOD(UINT8 cam_id, UINT8 mark_no)
{
	uvMIL_SaveMask_MOD(cam_id, mark_no);
}

/* desc: PAT ���Ͽ� MASK ���� ����, Mark Set������ ��� */
API_EXPORT VOID uvBasler_SaveMask_PAT(UINT8 cam_id, UINT8 mark_no)
{
	uvMIL_SaveMask_PAT(cam_id, mark_no);
}

/* desc: PAT MARK ���� */
API_EXPORT VOID uvBasler_PatMarkSave(UINT8 cam_id, CString fi_strFileName, UINT8 mark_no)
{
	uvMIL_PatMarkSave(cam_id, fi_strFileName, mark_no);
}

/* desc: MMF MARK ���� */
API_EXPORT VOID uvBasler_ModMarkSave(UINT8 cam_id, CString fi_strFileName, UINT8 mark_no)
{
	uvMIL_ModMarkSave(cam_id, fi_strFileName, mark_no);
}

/* desc: Mask �ʱ�ȭ */
API_EXPORT VOID uvBasler_InitMask(UINT8 cam_id)
{
	uvMIL_InitMask(cam_id);
}

/* desc: Mil Main �Ҵ� ���� ���� */
API_EXPORT VOID uvBasler_CloseSetMark()
{
	uvMIL_CloseSetMark();
}

/* desc: MARK DISP ID �Ҵ� */
API_EXPORT VOID uvBasler_SetDispMark(CWnd *pWnd)
{
	uvMIL_SetDispMark(pWnd);
}

/* desc: MARK DISP ID �Ҵ� */
API_EXPORT VOID uvBasler_SetDispRecipeMark(CWnd* pWnd[2])
{
	uvMIL_SetDispRecipeMark(pWnd);
}

/* desc: MARK DISP ID �Ҵ� */
API_EXPORT VOID uvBasler_SetDispExpo(CWnd* pWnd[4])
{
	uvMIL_SetDispExpo(pWnd);
}


/* desc: Mark ���� �׸� �� ����ϴ� MIL �Լ� */
API_EXPORT VOID uvBasler_DrawMarkInfo_UseMIL(UINT8 cam_id, UINT8 fi_smooth, UINT8 mark_no)
{
	uvMIL_DrawMarkInfo_UseMIL(cam_id, fi_smooth, mark_no);
}

/* desc: Set Mask */
API_EXPORT VOID uvBasler_Mask_MarkSet(UINT8 cam_id, CRect rectTmp, CPoint iTmpSizeP, CRect rectFill, int fi_color, bool bMask)
{
	uvMIL_Mask_MarkSet(cam_id, rectTmp, iTmpSizeP, rectFill, fi_color, bMask);
}

/*
	desc : Overlay ���� �Լ� - ��ü Overlay Clear or DC ���
	fi_bDrawFlag : FALSE - CLEAR
	fi_iDispType : 0:Expo, 1:mark, 2 : Live, 3 : mark set
	fi_iNo : Cam Num Ȥ�� Grab Mark Num (���� Disp Type �� ���缭 ���)
*/
API_EXPORT VOID uvBasler_DrawOverlayDC(bool fi_bDrawFlag, int fi_iDispType, int fi_iNo)
{
	uvMIL_DrawOverlayDC(fi_bDrawFlag, fi_iDispType, fi_iNo);
}


API_EXPORT VOID uvBasler_Camera_ClearShapes(int fi_iDispType)
{
	uvMIL_Camera_ClearShapes(fi_iDispType);
}





/* desc : Overlay ���� �Լ� - Box List �߰� */
API_EXPORT VOID uvBasler_OverlayAddBoxList(int fi_iDispType, int fi_iNo, int fi_iLeft, int fi_iTop, int fi_iRight, int fi_iBottom, int fi_iStyle, int fi_color)
{
	uvMIL_OverlayAddBoxList(fi_iDispType, fi_iNo, fi_iLeft, fi_iTop, fi_iRight, fi_iBottom, fi_iStyle, fi_color);
}

/* desc : Overlay ���� �Լ� - Cross List �߰� */
API_EXPORT VOID uvBasler_OverlayAddCrossList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, int fi_iWdt1, int fi_iWdt2, int fi_color)
{
	uvMIL_OverlayAddCrossList(fi_iDispType, fi_iNo, fi_iX, fi_iY, fi_iWdt1, fi_iWdt2, fi_color);
}

/* desc : Overlay ���� �Լ� - Text List �߰� */
API_EXPORT VOID uvBasler_OverlayAddTextList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, CString fi_sText, int fi_color, int fi_iSizeX, int fi_iSizeY, CString fi_sFont, bool fi_bEgdeFlag)
{
	uvMIL_OverlayAddTextList(fi_iDispType, fi_iNo, fi_iX, fi_iY, fi_sText, fi_color, fi_iSizeX, fi_iSizeY, fi_sFont, fi_bEgdeFlag);
}

/* desc : Overlay ���� �Լ� - Line List �߰� */
API_EXPORT VOID uvBasler_OverlayAddLineList(int fi_iDispType, int fi_iNo, int fi_iSx, int fi_iSy, int fi_iEx, int fi_iEy, int fi_iStyle, int fi_color)
{
	uvMIL_OverlayAddLineList(fi_iDispType, fi_iNo, fi_iSx, fi_iSy, fi_iEx, fi_iEy, fi_iStyle, fi_color);
}

/* desc: MARK SET DISP ID �Ҵ� */
API_EXPORT VOID uvBasler_SetDispMarkSet(CWnd* pWnd)
{
	uvMIL_SetDispMarkSet(pWnd);
}

/* desc: MMPM AutoCenter DISP ID �Ҵ� */
API_EXPORT VOID uvBasler_SetDispMMPM_AutoCenter(CWnd* pWnd)
{
	uvMIL_SetDispMMPM_AutoCenter(pWnd);
}

/* desc: LIVE DISP ID �Ҵ� */
API_EXPORT VOID uvBasler_SetDisp(CWnd** pWnd, UINT8 fi_Mode)
{
	uvMIL_SetDisp(pWnd, fi_Mode);
}

/* desc: MMPM DISP ID �Ҵ� */
API_EXPORT VOID uvBasler_SetDispMMPM(CWnd* pWnd)
{
	uvMIL_SetDispMMPM(pWnd);
}

/* desc : Zoom ���� �Լ� - Zoom In */
API_EXPORT VOID uvBasler_MilZoomIn(int fi_iDispType, int cam_id, CRect rc)
{
	uvMIL_MilZoomIn(fi_iDispType, cam_id, rc);
}

/* desc : Zoom ���� �Լ� - Zoom Out */
API_EXPORT BOOL uvBasler_MilZoomOut(int fi_iDispType, int cam_id, CRect rc)
{
	return uvMIL_MilZoomOut(fi_iDispType, cam_id, rc);
}

/* desc : Zoom ���� �Լ� - Zoom Fit, �̹��� ���� 1.0 */
API_EXPORT VOID uvBasler_MilAutoScale(int fi_iDispType, int cam_id)
{
	uvMIL_MilAutoScale(fi_iDispType, cam_id);
}

/* desc : Zoom ���� �Լ� - Mouse Point ����, Zoom���� Ȯ��Ǿ��� �� �̹��� �̵��ϱ� ���� */
API_EXPORT VOID uvBasler_SetZoomDownP(int fi_iDispType, int cam_id, CPoint fi_point)
{
	uvMIL_SetZoomDownP(fi_iDispType, cam_id, fi_point);
}

/* desc : Zoom ���� �Լ� - �̹��� �̵� */
API_EXPORT VOID uvBasler_MoveZoomDisp(int fi_iDispType, int cam_id, CPoint fi_point, CRect fi_rect)
{
	uvMIL_MoveZoomDisp(fi_iDispType, cam_id, fi_point, fi_rect);
}

/* desc : Zoom ���� �Լ� - Zoom �� ���󿡼� Ŭ���� ��ǥ�� �̹��� ��ǥ�� ��ȯ�����ִ� �۾� */
API_EXPORT CDPoint uvBasler_trZoomPoint(int fi_iDispType, int cam_id, CPoint fi_point)
{
	return uvMIL_trZoomPoint(fi_iDispType, cam_id, fi_point);
}

API_EXPORT BOOL uvBasler_SetModelDefine_tot(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
	LPG_CMPV model, UINT8 fi_No, TCHAR* file,
	DOUBLE scale_min, DOUBLE scale_max,
	DOUBLE score_min, DOUBLE score_tgt)
{
	return uvMIL_SetModelDefine_tot(cam_id, speed, level, count, smooth, model, fi_No, file,
		scale_min, scale_max, score_min, score_tgt);
}

API_EXPORT BOOL uvBasler_MergeMark(UINT8 cam_id, LPG_CMPV value, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, UINT8 mark_no, TCHAR* file1, TCHAR* file2,
	TCHAR* RecipeName, DOUBLE scale_min, DOUBLE scale_max,
	DOUBLE score_min, DOUBLE score_tgt)
{
	return uvMIL_MergeMark(cam_id, value, speed, level, count, smooth, mark_no, file1, file2,
		RecipeName, scale_min, scale_max, score_min, score_tgt);
}

API_EXPORT BOOL uvBasler_ProcImage(UINT8 cam_id, UINT8 imgProc)
{
	return uvMIL_ProcImage(cam_id, imgProc);
	//return g_pCamMain[cam_id - 1]->LoadImageFromFile(cam_id);
}

API_EXPORT CDPoint uvBasler_RunModel_VisionCalib(UINT8 cam_id, UINT8 dlg_id, UINT8 mark_no, int* roi_left, int* roi_right, int* roi_top, int* roi_bottom, int row, int col)
{
	CDPoint MarkP;
	MarkP.x = 0;
	MarkP.y = 0;
	/* New Grabbed Image ���� */
	if (cam_id < 1 || g_pstConfig->set_cams.acam_count < cam_id)	return MarkP;
	return g_pCamThread->RunModel_VisionCalib(cam_id, dlg_id, mark_no, roi_left, roi_right, roi_top, roi_bottom, row, col);
}
#ifdef __cplusplus
}
#endif
