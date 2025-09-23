
/*
 desc : MIL ���� �Լ� ��� ���� ��ü
*/

#include "pch.h"
#include "MainApp.h"
#include "MilMain.h"

#include "MilGrab.h"
#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CMilMain, CWnd)

/*
 desc : ������
 parm : config	- [in]  ��ü ȯ�� ����
		shmem	- [in]  ���� �޸� ����
 retn : None
*/
CMilMain::CMilMain()
{
	m_pstConfig = NULL;
	m_pstShMemVisi = NULL;

	/* ��� ���� �ʱ�ȭ */
#ifndef _NOT_USE_MIL_
	m_mSysID = M_NULL;
	m_mAppID = M_NULL;
	m_mGraphID = M_NULL;
#endif

	m_pLastGrabResult = NULL;
	m_pMilModel = NULL;
	m_pMilGrab = NULL;
	MARK_DISP_RATE = 1.0;
	MARKSET_DISP_RATE = 1.0;
	MMPM_DISP_RATE = 1.0;

	EXPO_DISP_SIZE_X			= 0;
	EXPO_DISP_SIZE_Y			= 0;
	MARK_DISP_SIZE_X			= 0;
	MARK_DISP_SIZE_Y			= 0;
	MARKSET_DISP_SIZE_X			= 0;
	MARKSET_DISP_SIZE_Y			= 0;
	LIVE_DISP_SIZE_X			= 0;
	LIVE_DISP_SIZE_Y			= 0;
	MMPM_DISP_SIZE_X			= 0;
	MMPM_DISP_SIZE_Y			= 0;
	RECIPE_MARK_DISP_SIZE_X		= 0;
	RECIPE_MARK_DISP_SIZE_Y		= 0;
	CALIB_CAMSPEC_DISP_SIZE_X	= 0;
	CALIB_CAMSPEC_DISP_SIZE_Y	= 0;
	CALIB_EXPO_DISP_SIZE_X	= 0;
	CALIB_EXPO_DISP_SIZE_Y	= 0;
	ACCR_DISP_SIZE_X		= 0;
	ACCR_DISP_SIZE_Y		= 0;

}



/*
 desc : �ı���
 parm : None
 retn : None
*/
CMilMain::~CMilMain()
{
	if (terminated)return;
		CloseMilAlloc();
	
}

/*
 desc : MIL ���� ��ü �ʱ�ȭ
 parm : run_mode	- [in]  0x01 : CMPS, 0x01 : Vision Step
		font_name	- [in]  �˻��� �̹����� ��� ������ ��µǴ� ��Ʈ �̸� (�ִ� 31 char)
		font_size	- [in]  �˻��� �̹����� ��� ������ ��µǴ� ��Ʈ ũ�� (�ִ� 63)
retn : TRUE or FALSE
*/
BOOL CMilMain::InitMilAlloc(ENG_ERVM run_mode, PTCHAR font_name, UINT8 font_size)
{
#ifndef _NOT_USE_MIL_
	UINT8 i	= 0x00, j = 0x00;
	MIL_UINT64	iSizeX, iSizeY;
	long		iBufAttri_Grab, iBufAttri_Disp, iBufAttri_Proc;
	int iCamCnt;

	/* Application ��ü �Ҵ� */
	m_mAppID	= MappAlloc(M_DEFAULT, M_NULL); 
	if (!m_mAppID || MappGetError(M_CURRENT, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"MIL_ERR : Failed to allocate <MappAlloc>");
		return FALSE;
	}
	/* System ��ü �Ҵ� */
	m_mSysID = MsysAlloc(M_SYSTEM_HOST,	/* ���� �ϵ������ VGA ī�� �ڿ� ��� */
							M_DEFAULT,		/* �Ҵ� �� Board�� ���� �Է�. 1 ���� ��� M_DEFAULT */
							M_DEFAULT,		/* M_DEFAULT == M_COMPLETE, ������ �ΰ��� �� �ѹ��� M_COMPLETE �ʱ�ȭ ���� */
							M_NULL);
	if (!m_mSysID || MappGetError(M_CURRENT, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"MIL_ERR : Failed to allocate <MsysAlloc>");
		return FALSE;
	}
	/* MIL Dongle Key ���� ���� */
	if (!IsLicenseValid())
	{
		AfxMessageBox(L"MIL Dongle Key does not exist.", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	iSizeX = ACA1920_SIZE_X;
	iSizeY = ACA1920_SIZE_Y;

	iBufAttri_Grab = M_IMAGE;
	iBufAttri_Disp = M_IMAGE + M_DISP + M_PROC;
	iBufAttri_Proc = M_IMAGE + M_PROC;

	for (iCamCnt = 0; iCamCnt < CAM_CNT; iCamCnt++)
	{
		// MilImg ����...
		if (!m_mImg[iCamCnt])
			MbufAlloc2d(m_mSysID, iSizeX, iSizeY, 8L + M_UNSIGNED, iBufAttri_Disp, &m_mImg[iCamCnt]);
		if (m_mImg[iCamCnt])		MbufClear(m_mImg[iCamCnt], 0L);
		else
		{
			AfxMessageBox(_T("Failed to Create m_mImg Buffer!"));
			return FALSE;
		}
	}
	for (iCamCnt = 0; iCamCnt < CAM_CNT; iCamCnt++)
	{
		// MilImg ����...
		if (!m_mImgProc[iCamCnt])
			MbufAlloc2d(m_mSysID, iSizeX, iSizeY, 8L + M_UNSIGNED, iBufAttri_Disp, &m_mImgProc[iCamCnt]);
		if (m_mImgProc[iCamCnt])		MbufClear(m_mImgProc[iCamCnt], 0L);
		else
		{
			AfxMessageBox(_T("Failed to Create m_mImgProc Buffer!"));
			return FALSE;
		}
	}

	if (!m_mImg_MPMM_AutoCenter_Proc)
		MbufAlloc2d(m_mSysID, iSizeX, iSizeY, 8L + M_UNSIGNED, iBufAttri_Disp, &m_mImg_MPMM_AutoCenter_Proc);
	if (m_mImg_MPMM_AutoCenter_Proc)		MbufClear(m_mImg_MPMM_AutoCenter_Proc, 0L);
	else
	{
		AfxMessageBox(_T("Failed to Create m_mImg_MPMM_AutoCenter_Proc Buffer!"));
		return FALSE;
	}

	for (iCamCnt = 0; iCamCnt < CAM_CNT; iCamCnt++)
	{
		if (!m_mImg_CALI_CAMSPEC[iCamCnt])
			MbufChild2d(m_mImgProc[iCamCnt], rectSearhROI_Calb_CamSpec.left, rectSearhROI_Calb_CamSpec.top,
				m_pstConfig->set_cams.spc_size[0], m_pstConfig->set_cams.spc_size[1], &m_mImg_CALI_CAMSPEC[iCamCnt]);
			//MbufAlloc2d(m_mSysID, m_pstConfig->set_cams.spc_size[0], m_pstConfig->set_cams.spc_size[1], 8L + M_UNSIGNED, iBufAttri_Disp, &m_mImg_CALI_CAMSPEC[iCamCnt]);
		if (m_mImg_CALI_CAMSPEC[iCamCnt])		MbufClear(m_mImg_CALI_CAMSPEC[iCamCnt], 0L);
		else
		{
			AfxMessageBox(_T("Failed to Create m_mImg_CALI_CAMSPEC Buffer!"));
			return FALSE;
		}
	}

	for (iCamCnt = 0; iCamCnt < CAM_CNT; iCamCnt++)
	{
		if (!m_mImg_CALI_STEP[iCamCnt])
			MbufChild2d(m_mImgProc[iCamCnt], rectSearhROI_Calb_Step.left, rectSearhROI_Calb_Step.top,
				m_pstConfig->set_cams.soi_size[0], m_pstConfig->set_cams.soi_size[1], &m_mImg_CALI_STEP[iCamCnt]);
			//MbufAlloc2d(m_mSysID, m_pstConfig->set_cams.soi_size[0], m_pstConfig->set_cams.soi_size[1], 8L + M_UNSIGNED, iBufAttri_Disp, &m_mImg_CALI_STEP[iCamCnt]);
		if (m_mImg_CALI_STEP[iCamCnt])		MbufClear(m_mImg_CALI_STEP[iCamCnt], 0L);
		else
		{
			AfxMessageBox(_T("Failed to Create m_mImg_CALI_STEP Buffer!"));
			return FALSE;
		}
	}
	
	for (iCamCnt = 0; iCamCnt < CAM_CNT; iCamCnt++)
	{
		if (!m_mImg_ACCR[iCamCnt])
			MbufChild2d(m_mImgProc[iCamCnt], rectSearhROI_Calb_Accr.left, rectSearhROI_Calb_Accr.top,
				m_pstConfig->set_cams.mes_size[0], m_pstConfig->set_cams.mes_size[1], &m_mImg_ACCR[iCamCnt]);
			//MbufAlloc2d(m_mSysID, m_pstConfig->set_cams.spc_size[0], m_pstConfig->set_cams.spc_size[1], 8L + M_UNSIGNED, iBufAttri_Disp, &m_mImg_ACCR[iCamCnt]);
		if (m_mImg_ACCR[iCamCnt])		MbufClear(m_mImg_ACCR[iCamCnt], 0L);
		else
		{
			AfxMessageBox(_T("Failed to Create m_mImg_ACCR Buffer!"));
			return FALSE;
		}
	}

	for (int i = 0; i < GRABMARK_CNT; i++) {
		if (!m_mImg_Mark[i])
			MbufAllocColor(m_mSysID, 1L, MARK_BUF_SIZE_X, MARK_BUF_SIZE_Y, 8L + M_UNSIGNED, iBufAttri_Disp, &m_mImg_Mark[i]);
		if (m_mImg_Mark[i])		MbufClear(m_mImg_Mark[i], 0L);
		else
		{
			AfxMessageBox(_T("Failed to Create m_mImg_Mark Buffer!"));
			return FALSE;
		}
	}

	for (int i = 0; i < GRABMARK_CNT; i++) {
		if (!m_mImg_Grab[i])
			MbufAlloc2d(m_mSysID, iSizeX, iSizeY, 8L + M_UNSIGNED, iBufAttri_Disp, &m_mImg_Grab[i]);
		if (m_mImg_Grab[i])		MbufClear(m_mImg_Grab[i], 0L);
		else
		{
			AfxMessageBox(_T("Failed to Create m_mImg_Grab Buffer!"));
			return FALSE;
		}
	}

	/* Font Size ���� */
	if (font_name)
	{
		MgraFont(M_DEFAULT, M_FONT_DEFAULT_MEDIUM );
		if (font_size)	MgraControl(M_DEFAULT, M_FONT_SIZE, font_size);
	}
	/* Graphic Control ID */
	MgraAlloc(m_mSysID, &m_mGraphID);
	if (!m_mGraphID || MappGetError(M_CURRENT, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"MIL_ERR : Failed to allocate <MgraAlloc>");
		return FALSE;
	}

	/* MIL License Check */
	if (!IsLicenseValid())
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"MIL_ERR : MIL license is invalid");
		AfxMessageBox(L"MIL license is invalid\nCheck the usb dongle key", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* Mark Model Search Object */
	//m_pMilModel	= new CMilModel * [m_pstConfig->set_cams.acam_count];
	//m_pMilModel = new CMilModel * [m_pstConfig->set_cams.acam_count + 1];
	m_pMilModel = new CMilModel * [m_pstConfig->set_cams.acam_count + 1];
	ASSERT(m_pMilModel);
	//for (i=0; i<m_pstConfig->set_cams.acam_count; i++)
	for (i = 0; i < m_pstConfig->set_cams.acam_count + 1; i++)
	{
		m_pMilModel[i]	= new CMilModel(m_pstConfig, m_pstShMemVisi,
			i + 1, m_mSysID, run_mode);
		ASSERT(m_pMilModel[i]);
	}
	/* 1�� Scan �� ������ ������ Grabbed Image�� ���� �� �����ϴ� ��ü �޸� �Ҵ� */
	m_pMilGrab	= (CMilGrab ***) new CMilGrab ** [m_pstConfig->set_cams.acam_count];
	ASSERT(m_pMilGrab);
	for (i=0; i<m_pstConfig->set_cams.acam_count; i++)
	{
		m_pMilGrab[i]	= (CMilGrab **) new CMilGrab * [m_pstConfig->mark_find.max_mark_grab];
		for (j=0; j<m_pstConfig->mark_find.max_mark_grab; j++)
		{
			m_pMilGrab[i][j]	= new CMilGrab(m_pstConfig, m_pstShMemVisi,
				i + 1, m_mSysID,run_mode);
		}
	}
#endif

	return TRUE;
}

/*
 desc : MIL ���� ��ü ����
 parm : None
 retn : None
*/
VOID CMilMain::CloseMilAlloc()
{
	//UINT8 i	= 0x00, j = 0x00;

	if (terminated)return;

	/* ���� Grabbed Image Object ��� ���� */
	ResetGrabAll();
#ifndef _NOT_USE_MIL_
	
	theApp.clMilDisp.CloseMilDisp();

	/* ���� ��Ȯ�� */
	/* BufFree */
	for (int i = GRABMARK_CNT - 1; i >= 0; i--)
	{
		if (m_mImgDisp_EXPO[i])
		{
			MbufFree(m_mImgDisp_EXPO[i]);
			m_mImgDisp_EXPO[i] = M_NULL;
		}
	}

	if (m_mImgDisp_Mark_Live)
	{
		MbufFree(m_mImgDisp_Mark_Live);
		m_mImgDisp_Mark_Live = M_NULL;
	}
	if (m_mImgDisp_MMPM)
	{
		MbufFree(m_mImgDisp_MMPM);
		m_mImgDisp_MMPM = M_NULL;
	}

	if (m_mImgDisp_CALI_CAMSPEC)
	{
		MbufFree(m_mImgDisp_CALI_CAMSPEC);
		m_mImgDisp_CALI_CAMSPEC = M_NULL;
	}
	for (int i = CAM_CNT - 1; i >= 0; i--) {
		if (m_mImgDisp_CALI_STEP[i])
		{
			MbufFree(m_mImgDisp_CALI_STEP[i]);
			m_mImgDisp_CALI_STEP[i] = M_NULL;
		}
	}
	if (m_mImgDisp_ACCR)
	{
		MbufFree(m_mImgDisp_ACCR);
		m_mImgDisp_ACCR = M_NULL;
	}
	for (int i = GRABMARK_CNT - 1; i >= 0; i--)
	{
		if (m_mImgDisp_Mark[i])
		{
			MbufFree(m_mImgDisp_Mark[i]);
			m_mImgDisp_Mark[i] = M_NULL;
		}
	}

	/* DispFree */
	for (int i = GRABMARK_CNT - 1; i >= 0; i--)
	{
		if (m_mDisID_EXPO[i])
		{
			MdispDeselect(m_mDisID_EXPO[i], m_mImgDisp_EXPO[i]);
			MdispFree(m_mDisID_EXPO[i]);
			m_mDisID_EXPO[i] = M_NULL;
		}
	}
	for (int i = GRABMARK_CNT - 1; i >= 0; i--)
	{
		if (m_mDisID_Mark[i])
		{
			MdispDeselect(m_mDisID_Mark[i], m_mImgDisp_Mark[i]);
			MdispFree(m_mDisID_Mark[i]);
			m_mDisID_Mark[i] = M_NULL;
		}
	}
	if (m_mDisID_Mark_Live)
	{
		MdispDeselect(m_mDisID_Mark_Live, m_mImgDisp_Mark_Live);
		MdispFree(m_mDisID_Mark_Live);
		m_mDisID_Mark_Live = M_NULL;
	}

	if (m_mDisID_MMPM)
	{
		MdispDeselect(m_mDisID_MMPM, m_mImgDisp_MMPM);
		MdispFree(m_mDisID_MMPM);
		m_mDisID_MMPM = M_NULL;
	}

	if (m_mDisID_CALI_CAMSPEC)
	{
		MdispDeselect(m_mDisID_CALI_CAMSPEC, m_mImgDisp_CALI_CAMSPEC);
		MdispFree(m_mDisID_CALI_CAMSPEC);
		m_mDisID_CALI_CAMSPEC = M_NULL;
	}

	for (int i = CAM_CNT - 1; i >= 0; i--) {
		if (m_mDisID_CALI_STEP[i])
		{
			MdispDeselect(m_mDisID_CALI_STEP[i], m_mImgDisp_CALI_STEP[i]);
			MdispFree(m_mDisID_CALI_STEP[i]);
			m_mDisID_CALI_STEP[i] = M_NULL;
		}
	}

	if (m_mDisID_ACCR)
	{
		MdispDeselect(m_mDisID_ACCR, m_mImgDisp_ACCR);
		MdispFree(m_mDisID_ACCR);
		m_mDisID_ACCR = M_NULL;
	}

/* �Ź� Align Mark�� Grabbed Image �����ϴ� ��ü �޸� ���� */
	if (m_pMilGrab)
	{
		for (int i = CAM_CNT - 1; i >= 0; i--)
		{
			//for (j = 0; j < MAX_MARK_GRAB; j++)	delete m_pMilGrab[i][j];
			for (int j = MAX_MARK_GRAB - 1; j >= 0; j--)
				delete m_pMilGrab[i][j];
			delete[] m_pMilGrab[i];
		}
		delete[] m_pMilGrab;
	}
	/* Pattern (Model) Search Object ���� */
	if (m_pMilModel)
	{
		//for (int i = 0; i < CAM_CNT + 1; i++)
		for (int i = CAM_CNT; i >= 0; i--) // 0, 1, 2 (3�� �Ҵ�)
		{
			if (m_pMilModel[i]) {
				delete m_pMilModel[i];
			}
		}
		delete[] m_pMilModel;
	}

	/* ���� MIL ��ü �޸� ���� */
	if (m_mGraphID) {
		MgraFree(m_mGraphID);
		m_mGraphID = M_NULL;
	}

	if (m_mImg_MarkSet)
	{
		MbufFree(m_mImg_MarkSet);
		m_mImg_MarkSet = M_NULL;
	}

	if (m_mImg_MarkSet_Ori)
	{
		MbufFree(m_mImg_MarkSet_Ori);
		m_mImg_MarkSet_Ori = M_NULL;
	}
	if (m_mImg_MPMM_AutoCenter)
	{
		MbufFree(m_mImg_MPMM_AutoCenter);
		m_mImg_MPMM_AutoCenter = M_NULL;
	}

	for (int i = GRABMARK_CNT - 1; i >= 0; i--)
	{
		if (m_mImg_Grab[i]) {
			MbufFree(m_mImg_Grab[i]);
			m_mImg_Grab[i] = M_NULL;
		}
	}
	for (int i = GRABMARK_CNT - 1; i >= 0; i--)
	{
		if (m_mImg_Mark[i]) {
			MbufFree(m_mImg_Mark[i]);
			m_mImg_Mark[i] = M_NULL;
		}
	}

	for (int i = CAM_CNT - 1; i >= 0; i--)
	{
		if (m_mImg_ACCR[i]) {
			MbufFree(m_mImg_ACCR[i]);
			m_mImg_ACCR[i] = M_NULL;
		}
	}

	for (int i = CAM_CNT - 1; i >= 0; i--)
	{
		if (m_mImg_CALI_STEP[i]) {
			MbufFree(m_mImg_CALI_STEP[i]);
			m_mImg_CALI_STEP[i] = M_NULL;
		}
	}
	for (int i = CAM_CNT - 1; i >= 0; i--)
	{
		if (m_mImg_CALI_CAMSPEC[i]) {
			MbufFree(m_mImg_CALI_CAMSPEC[i]);
			m_mImg_CALI_CAMSPEC[i] = M_NULL;
		}
	}

	if (m_mImg_MPMM_AutoCenter_Proc) {
		MbufFree(m_mImg_MPMM_AutoCenter_Proc);
		m_mImg_MPMM_AutoCenter_Proc = M_NULL;
	}

	for (int i = CAM_CNT - 1; i >= 0; i--)
	{
		if (m_mImgProc[i]) {
			MbufFree(m_mImgProc[i]);
			m_mImgProc[i] = M_NULL;
		}
	}

	for (int i = CAM_CNT - 1; i >= 0; i--)
	{
		if (m_mImg[i]) {
			MbufFree(m_mImg[i]);
			m_mImg[i] = M_NULL;
		}
	}

	/* SysFree */
	/* AppFree */
	//	/* ���� MIL ��ü �޸� ���� */
//if (m_mGraphID)	MgraFree(m_mGraphID);
//if (m_mlDisID)		MdispFree(m_mlDisID);
	if (m_mSysID) {
		MsysFree(m_mSysID);
		m_mSysID = M_NULL;
	}
	if (m_mAppID) {
		MappFree(m_mAppID);
		m_mAppID = M_NULL;
	}

	delete rectSearhROI;
	delete m_mImg;
	delete m_mOverlay_CALI_STEP;
	delete m_mDisID_Mark;
	delete m_mOverlay_Mark;
	delete m_mImgDisp_Mark;
	delete m_mImg_Mark;
	delete m_mImg_Grab;
	delete MilEdgeResult;
	delete m_mDisID_CALI_STEP;
	delete m_mImgDisp_CALI_STEP;
	delete m_mImg_ACCR;
	delete m_mImg_CALI_CAMSPEC;
	delete m_mImg_CALI_STEP;
	delete m_mImgProc;				// 231027 mhbaek Add �޸� ������ ���� �������� �߰�

	delete m_mDisID_EXPO;
	delete m_mImgDisp_EXPO;
	delete m_mOverlay_EXPO;

#endif
	terminated = true;
}

/*
 desc : ���� �ֱ��� Grabbed Image ��� �ʱ�ȭ
 		���ο� Align �۾��� �����Ѵٸ�, Align �����ϱ� ���� ������ �ѹ��� ȣ��Ǿ�� ��
parm : None
 retn : None
*/
VOID CMilMain::ResetGrabAll()
{
	/* ���� �������� ���� ��Ī�� Align - Grabbed Image �˻� ��� ������ ��ü �ʱ�ȭ */
	m_pLastGrabResult	= NULL;
}

/*
 desc : Grabbed Image �˻�
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : grabbed image result ��ȯ
*/
LPG_ACGR CMilMain::GetGrabbedMark(UINT8 cam_id, UINT8 img_id)
{
	if (!m_pLastGrabResult)	return NULL;
	/* Camera Index ��ȿ�� �˻� */
	if (cam_id > m_pstConfig->set_cams.acam_count)	return NULL;
	return m_pMilGrab[cam_id-1][img_id]->GetGrabbedMark();
}

/*
 desc : Grabbed Image ���� �˻��� ��ũ ���� ��ü ��ȯ
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : grabbed image result ��ȯ
*/
LPG_GMSR CMilMain::GetGrabbedMarkAll(UINT8 cam_id, UINT8 img_id)
{
	/* Camera Index ��ȿ�� �˻� */
	if (cam_id > m_pstConfig->set_cams.acam_count)	return NULL;
	return m_pMilGrab[cam_id-1][img_id]->GetGrabbedMarkAll();
}

/*
 desc : Grabbed Image ���� �˻��� ��ũ ���� ��ȯ
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : grabbed image result ���� ��ȯ
*/
UINT8 CMilMain::GetGrabbedMarkCount(UINT8 cam_id, UINT8 img_id)
{
	/* Camera Index ��ȿ�� �˻� */
	if (cam_id > m_pstConfig->set_cams.acam_count)	return 0x00;
	return m_pMilGrab[cam_id-1][img_id]->GetGrabbedMarkCount();
}

/*
 desc : ���� �ֱ� Align Grabbed Image�� ��Ī �˻� ��� ����ü ������ ��ȯ
 parm : None
 retn : ��ü ������ (���� ��Ī�� ������ ��� NULL)
*/
LPG_ACGR CMilMain::GetLastGrabbedMark()
{
	if (!m_pLastGrabResult)	return NULL;
	return m_pLastGrabResult->GetGrabbedMark();
}

/*
 desc : ���� �˻��� ���� ��ϵ� ���� ���� ��ȯ
 parm : cam_id	- [in]  Align Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : ��ϵ� ����
*/
UINT8 CMilMain::GetModelRegistCount(UINT8 cam_id)
{
	if (cam_id >= MAX_ALIGN_CAMERA)	return 0;
	return m_pMilModel[cam_id-1]->GetModelRegistCount();
	return 2; //lk91 ��� ����.
}

/*
 desc : Geometric Model Find
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later) (if img_id == 0xff then Calibration Method)
		image	- [in]  Grabbed Image
 retn : TRUE or FALSE
*/
BOOL CMilMain::RunModelFind(UINT8 cam_id, UINT8 img_id, PUINT8 image, UINT8 dlg_id, UINT8 mark_no, BOOL useMilDisp, UINT8 img_proc)
{
	DOUBLE dbMarkX		= 0.0f, dbMarkY = 0.0f;
	LPG_GMFR pstResultMain = NULL, pstResultSide = NULL;

	/* ���� �ش� ��ü�� ���� ��ϵǾ� �ִ��� ���� */
	if (!m_pMilModel)	return FALSE;

#ifndef _NOT_USE_MIL_
	if (!m_pMilModel[cam_id-1]->IsSetMarkModel(mark_no))	return FALSE;
#endif

#if 0	/* �̹� Baser �ʿ��� Interlock ó�� �ص��� */
	/* ���� ī�޶� ���� ���� �����ϴ� �̹��� ������ ������� Ȯ��, ��, Ʈ���Ű� ���ڱ� ���� �߻��ϱ� ������ */
	if (img_id >= m_pstConfig->mark_find.max_mark_grab)
	{
		swprintf_s(tzMesg, 128, L"The number of images captured is too large (cur=%u / max=%u)",
				   img_id, m_pstConfig->mark_find.max_mark_grab);
		LOG_ERROR(ENG_EDIC::en_mil, tzMesg);
		return FALSE;
	}
#endif
	/* Grabbed Image ó���� ���� ��ü ���� */
	m_pLastGrabResult	= m_pMilGrab[cam_id-1][img_id];
	if (!m_pLastGrabResult)	return FALSE;

	/* ���� ���� �Ź��� ��ϵ� Mark�� �߽� X, Y ��ǥ */
	if (!m_pLastGrabResult->PutGrabImage(image, img_id, dbMarkX, dbMarkY))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"Failed to put the grabbed image");
	}
	else
	{
#ifndef _NOT_USE_MIL_
		/* Mark �˻� ���� */
		if (m_pMilModel[cam_id-1]->RunModelFind(m_mGraphID, m_pLastGrabResult->GetBufID(), FALSE, img_id, dlg_id, mark_no, useMilDisp, img_proc))
		{
			pstResultMain	= m_pMilModel[cam_id-1]->GetFindMark();
			pstResultSide	= m_pMilModel[cam_id-1]->GetFindMarkAll();
			/* Geometric Find ��� �� ���� (������ ��쿡�� ����) */
			if (!pstResultMain || !pstResultSide)
			{
				if (m_pstConfig->set_comn.grab_mark_saved)
				{
					m_pLastGrabResult->SaveGrabbedMarkToFile(img_id, NULL);
				}
				return FALSE;
			}
			else
			{
				//pstResultMain->mark_width	= m_pMilModel[cam_id-1]->GetModelWidth(UINT8(pstResultMain->model_index));
				//pstResultMain->mark_height	= m_pMilModel[cam_id-1]->GetModelHeight(UINT8(pstResultMain->model_index));

				//pstResultSide->mark_width	= m_pMilModel[cam_id-1]->GetModelWidth(UINT8(pstResultSide->model_index));
				//pstResultSide->mark_height	= m_pMilModel[cam_id-1]->GetModelHeight(UINT8(pstResultSide->model_index));
				
				pstResultMain->mark_width = m_pMilModel[cam_id - 1]->GetModelWidth(mark_no);
				pstResultMain->mark_height = m_pMilModel[cam_id - 1]->GetModelHeight(mark_no);

				pstResultSide->mark_width = m_pMilModel[cam_id - 1]->GetModelWidth(mark_no);
				pstResultSide->mark_height = m_pMilModel[cam_id - 1]->GetModelHeight(mark_no);

				/* ��� �� ��� �� ���� */
				m_pLastGrabResult->SetGrabbedMark(img_id, pstResultMain, pstResultSide,
												  m_pMilModel[cam_id-1]->GetFindMarkAllCount(),
												  m_pMilModel[cam_id-1]->GetImageWidth(),
												  m_pMilModel[cam_id-1]->GetImageHeight());
				/* �˻� ��� ���� */
				if (m_pstConfig->set_comn.grab_mark_saved)
				{
					m_pLastGrabResult->SaveGrabbedMarkToFile(img_id,
															 m_pMilModel[cam_id-1]->GetFindMark());
				}
				/* ã�Ҵٰ� ������ ������ �ƴ϶�, ���Ѵ� ��� ���� ���Դ��� ���ο� ���� */
				if (m_pLastGrabResult->IsMarkMatched())	return TRUE;
			}
		}
		/* �˻� ������ ��� */
		else
		{
			/* �˻� ��� ���� */
			if (m_pstConfig->set_comn.grab_mark_saved)
			{
				m_pLastGrabResult->SaveGrabbedMarkToFile(img_id, NULL);
			}
		}
#endif
	}

	return FALSE;
}

/*
 desc : ����ڿ� ���� �������� �˻��� ���, ��� ���� ó��
 parm : grab	- [in]  ����ڿ� ���� �������� �Էµ� grabbed image ��� ������ ����� ����ü ������
		gmfr	- [in]  GMFR Data
		gmsr	- [in]  GMSR Data
 retn : TRUE or FALSE
*/
BOOL CMilMain::SetGrabbedMark(LPG_ACGR grab, LPG_GMFR gmfr, LPG_GMSR gmsr)
{
	TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL};

	/* Grabbed Image ó���� ���� ��ü ���� */
	m_pLastGrabResult	= m_pMilGrab[grab->cam_id-1][grab->img_id];
	if (!m_pLastGrabResult)
	{
		swprintf_s(tzMesg, LOG_MESG_SIZE,
				   L"There is no grabbed image (cam_id:%d img_id:%d)",
				   grab->cam_id, grab->img_id);
		LOG_ERROR(ENG_EDIC::en_mil, tzMesg);
		return FALSE;
	}
	/* ���� �ش� ��ü�� ���� ��ϵǾ� �ִ��� ���� */
#ifndef _NOT_USE_MIL_
	int mark_num;
	if (grab->img_id < 2)
		mark_num = 0; // global mark
	else
		mark_num = 1; // local mark
	if (!m_pMilModel[grab->cam_id-1]->IsSetMarkModel(mark_num))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"There is no registered mark model");
		return FALSE;
	}
#endif
	/* ������ �Է� ó�� */

#if 1
	m_pLastGrabResult->PutGrabbedMark(grab, gmsr,
									  m_pMilModel[grab->cam_id-1]->GetImageWidth(),
									  m_pMilModel[grab->cam_id-1]->GetImageHeight());
#else
	/* Geometric Find ��� �� ���� (������ ��쿡�� ����) */
	m_pLastGrabResult->SetGrabResult(img_id, gmfr,
									 m_pMilModel[grab->cam_id-1]->GetFindMarkAll(),
									 m_pMilModel[grab->cam_id-1]->GetFindMarkAllCount(),
									 m_pMilModel[grab->cam_id-1]->GetImageWidth(),
									 m_pMilModel[grab->cam_id-1]->GetImageHeight());
#endif
	/* �˻� ��� ���� */
	//m_pLastGrabResult->SaveGrabbedMarkToFile(grab->img_id, gmfr);
	/* ã�Ҵٰ� ������ ������ �ƴ϶�, ���Ѵ� ��� ���� ���Դ��� ���ο� ���� */
	return m_pLastGrabResult->IsMarkMatched();
}

/*
 desc : Geometric Model Find
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		image	- [in]  Grabbed Image
		angle	- [in]  ���� ���� ���� (TRUE : ���� ������, FALSE : ���� �������� ����)
 retn : TRUE or FALSE
*/
BOOL CMilMain::RunModelFind(UINT8 cam_id, PUINT8 image, BOOL angle, UINT8 img_id, UINT8 dlg_id, UINT8 mark_no, BOOL useMilDisp, UINT8 img_proc)
{
	LPG_GMFR pstResultMain = NULL, pstResultSide = NULL;

	/* Grabbed Image ó���� ���� ��ü ���� */
	m_pLastGrabResult	= m_pMilGrab[cam_id-1][0];
	if (!m_pLastGrabResult)	return FALSE;
	if (!m_pLastGrabResult->PutGrabImage(image, 0x00 /* fixed */,
										 0.0f /* fixed */, 0.0f /* fixed */))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"Failed to put the grabbed image");
	}
	else
	{
		if (m_pMilModel)
		{
#ifndef _NOT_USE_MIL_
			/* ���� �ش� ��ü�� ���� ��ϵǾ� �ִ��� ���� */
			if (!m_pMilModel[cam_id-1]->IsSetMarkModel(mark_no))
			{
				LOG_ERROR(ENG_EDIC::en_mil, L"There is no registered mark");
				return FALSE;
			}
			/* Find to model */
			if (m_pMilModel[cam_id-1]->RunModelFind(m_mGraphID, m_pLastGrabResult->GetBufID(), angle, img_id, dlg_id, mark_no, useMilDisp, img_proc))
			{
				pstResultMain	= m_pMilModel[cam_id-1]->GetFindMark();
				if (pstResultMain)
				{
					pstResultMain->mark_width	= m_pMilModel[cam_id-1]->GetModelWidth(mark_no);
					pstResultMain->mark_height	= m_pMilModel[cam_id-1]->GetModelHeight(mark_no);

	#ifdef _DEBUG
					TRACE(L"RunModelFind = mark_width : %u, mark_height = %u\n",
						  pstResultMain->mark_width, pstResultMain->mark_height);
	#endif
					pstResultSide	= m_pMilModel[cam_id-1]->GetFindMarkAll();
					pstResultSide->mark_width	= m_pMilModel[cam_id-1]->GetModelWidth(mark_no);
					pstResultSide->mark_height	= m_pMilModel[cam_id-1]->GetModelHeight(mark_no);
					/* Geometric Find ��� �� ���� */
					m_pLastGrabResult->SetGrabbedMark(0xff,
													  pstResultMain, pstResultSide,
													  m_pMilModel[cam_id-1]->GetFindMarkAllCount(),
													  m_pMilModel[cam_id-1]->GetImageWidth(),
													  m_pMilModel[cam_id-1]->GetImageHeight()); // lk91 ����� mark pixel ���� ���� �Ÿ��� ��ȯ
					/* �÷��� ���� */
					return TRUE;
				}
			}
#else
			return TRUE;
#endif
		}
	}

	return FALSE;
}

/*
 desc : Geometric Model Find (Step; ���� �˻���)
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		width	- [in]  Grabbed Image - Width (Pixel)
		height	- [in]  Grabbed Image - Width (Pixel)
		count	- [in]  �˻��ϰ��� �ϴ� �� ����
		angle	- [in]  ���� ���� ���� (TRUE : ���� ������, FALSE : ���� �������� ����)
						TRUE : ���� ī�޶��� ȸ���� ���� ���ϱ� ����, FALSE : ���� ���� �����Ͽ� ȸ���� ���� ����
		results	- [out] �˻��� ��� ������ ����� ����ü ���� ������ (grabbed image buffer���� �����ؿ�)
 retn : TRUE or FALSE
*/
BOOL CMilMain::RunModelStep(UINT8 cam_id, PUINT8 image, UINT32 width, UINT32 height,
							UINT16 count, BOOL angle, LPG_ACGR results, UINT8 img_id, UINT8 dlg_id, UINT8 mark_no, UINT8 img_proc)
{
	UINT8 i = 0, j = 0;
	LPG_GMFR pstGet = NULL;

	/* �˻� ��� ������ 1 ������ ��� ���� ó�� */
	if (count < 1)	return FALSE;
	/* Grabbed Image ó���� ���� ��ü ���� */
	m_pLastGrabResult	= m_pMilGrab[cam_id-1][0];
	if (!m_pLastGrabResult)	return FALSE;
	if (!m_pLastGrabResult->PutGrabImage(image, 0x00 /* fixed */,
										 0.0f /* fixed */, 0.0f /* fixed */))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"Failed to put the grabbed image");
	}
	else
	{
		/* ���� �ش� ��ü�� ���� ��ϵǾ� �ִ��� ���� */
		if (!m_pMilModel)	return FALSE;
#ifndef _NOT_USE_MIL_
		if (!m_pMilModel[cam_id-1]->IsSetMarkModel(mark_no))
		{
			LOG_ERROR(ENG_EDIC::en_mil, L"There is no registered mark");
			return FALSE;
		}
#endif

#ifndef _NOT_USE_MIL_
		/* Edge Detecting ���� */
		if (!m_pMilModel[cam_id-1]->RunModelFind(m_mGraphID, m_pLastGrabResult->GetBufID(), angle, img_id, dlg_id, mark_no, TRUE, img_proc))
		{
			return FALSE;
		}
		if (m_pMilModel[cam_id-1]->GetFindMarkAllCount() < count)
		{
			TCHAR tzMesg[128] = {NULL};
			swprintf_s(tzMesg, 128, L"Failed to get the model_count (find:%u < total:%u)",
					   m_pMilModel[cam_id-1]->GetFindMarkAllCount(), count);
			LOG_ERROR(ENG_EDIC::en_mil, tzMesg);
			return FALSE;
		}
		for (; i<count && j<2; i++)
		{
			/* �ֱ� Grabbed Image ���� �� ��� */
			pstGet	= m_pMilModel[cam_id-1]->GetFindMark(i);
			pstGet->mark_width	= m_pMilModel[cam_id-1]->GetModelWidth(UINT8(pstGet->model_index));
			pstGet->mark_height	= m_pMilModel[cam_id-1]->GetModelHeight(UINT8(pstGet->model_index));
			/* �˻����� ũ�� ���� ��� ���� �ž� ��ȿ ������ �Ǵ� */
			if (pstGet->scale_rate >= m_pstConfig->mark_find.scale_rate &&
				pstGet->score_rate >= m_pstConfig->mark_find.score_rate)
			{
				/* Geometric Find ��� �� �ӽ� ���� */
				m_pLastGrabResult->SetGrabbedMark(0xff, pstGet,
												  m_pMilModel[cam_id-1]->GetFindMarkAll(),
												  m_pMilModel[cam_id-1]->GetFindMarkAllCount(),
												  m_pMilModel[cam_id-1]->GetImageWidth(),
												  m_pMilModel[cam_id-1]->GetImageHeight());
				/* ��ȯ ����ü�� ���� ���� (����Ʈ �ּұ��� �����ع����� �ȵǹǷ� ...) */
				memcpy(&results[j], m_pLastGrabResult->GetGrabbedMark(), sizeof(STG_ACGR)-sizeof(PUINT8));
				/* Grabbed Image���� ���Ե� */
				memcpy(results[j].grab_data, image, width * height);
				/* ���� �ε��� �� ���� */
				j++;
			}
		}
#endif
	}
	/* �˻� ��ȿ ������ �´��� Ȯ�� */
	return (j == count);
}


/*
 desc : Geometric Model Find (Examination; �˻� (����) �˻���)
 parm : image	- [in]  Bitmap Image Buffer
		width	- [in]  Grabbed Image - Width (Pixel)
		height	- [in]  Grabbed Image - Width (Pixel)
		score	- [in]  ũ�� ���� �� (�� ������ ũ�ų� ������ Okay)
		scale	- [in]  ��Ī ���� �� (1.0 �������� +/- �� �� ���� �ȿ� ������ Okay)
		results	- [out] �˻��� ��� ������ ����� ����ü ���� ������ (grabbed image buffer���� �����ؿ�)
 retn : TRUE or FALSE
*/
BOOL CMilMain::RunModelExam2(PUINT8 image, UINT32 width, UINT32 height,
	DOUBLE score, DOUBLE scale, LPG_ACGR results)
{
	UINT8 i = 0, j = 0;
	BOOL bIsFindAll = FALSE;
	LPG_GMFR pstGet = NULL;
	bool valid[2] = { false,false };
	/* Grabbed Image ó���� ���� ��ü ���� */
	m_pLastGrabResult = m_pMilGrab[0][0];	/* Align Camera 1 - Fixed */
	if (!m_pLastGrabResult)	return FALSE;
	if (!m_pLastGrabResult->PutGrabImage(image, 0x00 /* fixed */, 0.0f /* fixed */, 0.0f /* fixed */))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"Failed to put the grabbed image");
	}
	else
	{
		/* ���� �ش� ��ü�� ���� ��ϵǾ� �ִ��� ���� */
		if (!m_pMilModel)						return FALSE;
		if (!m_pMilModel[0]->IsSetMarkModel(TMP_MARK))	return FALSE;
		/* Edge Detecting ���� */
		if (!m_pMilModel[0]->RunModelFind(m_mGraphID, m_pLastGrabResult->GetBufID(), FALSE))
		{
			return FALSE;
		}
		//if (!m_pMilModel[0]->IsFindModCount(2, score, scale))
		//{
		//	return FALSE;
		//}
		//theApp.clMilDisp.DrawOverlayDC(false, DISP_TYPE_CALB_ACCR, 0);
		
		for (i = 0; i < 2; i++)
		{
			pstGet = m_pMilModel[0]->GetFindMark(i);
			pstGet->mark_width = m_pMilModel[0]->GetModelWidth(i);
			pstGet->mark_height = m_pMilModel[0]->GetModelHeight(i);
			
			if (pstGet->IsValidMark(score, scale))
			{
				int modelIdx = pstGet->model_index;

				valid[modelIdx] = true;
				m_pLastGrabResult->SetGrabbedMark(0xff, pstGet,
					m_pMilModel[0]->GetFindMarkAll(),
					m_pMilModel[0]->GetFindMarkAllCount(),
					pstGet->mark_width, pstGet->mark_height);
				if (results)
				{
					
					
					/* ��ȯ ����ü�� ���� ���� (����Ʈ �ּұ��� �����ع����� �ȵǹǷ� ...) */
					memcpy(&results[modelIdx], m_pLastGrabResult->GetGrabbedMark(),
						sizeof(STG_ACGR) - sizeof(PUINT8));
					/* Grabbed Image���� ���Ե� */
					//memcpy(results[i].grab_data, image, width * height);
					/* �˻��� Mark ũ�� ���� ������ �ٽ� �����ؾ� �� */
					results[modelIdx].mark_width_px = m_pMilModel[0]->GetModelWidth(j);
					results[modelIdx].mark_height_px = m_pMilModel[0]->GetModelHeight(j);
					results[modelIdx].marked = valid[modelIdx];
					//theApp.clMilDisp.AddCrossList(DISP_TYPE_CALB_ACCR, 0, results[i].mark_width_px - results[i].mark_cent_px_x, results[i].mark_height_px - results[i].mark_cent_px_y,  30, 30, COLOR_BLUE);
					//uvEng_Camera_DrawImageBitmap((UINT8)DISP_TYPE_CALB_ACCR, 0, 1, 0, 1);

					/* ���� Array�� �����ϱ� ���԰� ���ÿ�, 2�� ��� �˻��Ǿ����� ���� */
					
						
				}
			}
			else
			{
				
			}
		}
		//theApp.clMilDisp.DrawOverlayDC(true, DISP_TYPE_CALB_ACCR, 0);
	}
	auto res = (valid[0] & valid[1]) == true;

	return res;
}


/*
 desc : Geometric Model Find (Examination; �˻� (����) �˻���)
 parm : image	- [in]  Bitmap Image Buffer	
		width	- [in]  Grabbed Image - Width (Pixel)
		height	- [in]  Grabbed Image - Width (Pixel)
		score	- [in]  ũ�� ���� �� (�� ������ ũ�ų� ������ Okay)
		scale	- [in]  ��Ī ���� �� (1.0 �������� +/- �� �� ���� �ȿ� ������ Okay)
		results	- [out] �˻��� ��� ������ ����� ����ü ���� ������ (grabbed image buffer���� �����ؿ�)
 retn : TRUE or FALSE
*/
BOOL CMilMain::RunModelExam(PUINT8 image, UINT32 width, UINT32 height,
							DOUBLE score, DOUBLE scale, LPG_ACGR results, UINT8 img_id, UINT8 dlg_id, UINT8 mark_no) // �̻��
{
	UINT8 i	= 0, j = 0;
	BOOL bIsFindAll	= FALSE;
	LPG_GMFR pstGet	= NULL;

	/* Grabbed Image ó���� ���� ��ü ���� */
	m_pLastGrabResult	= m_pMilGrab[0][0];	/* Align Camera 1 - Fixed */
	if (!m_pLastGrabResult)	return FALSE;
	if (!m_pLastGrabResult->PutGrabImage(image, 0x00 /* fixed */, 0.0f /* fixed */, 0.0f /* fixed */))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"Failed to put the grabbed image");
	}
	else
	{
		/* ���� �ش� ��ü�� ���� ��ϵǾ� �ִ��� ���� */
		if (!m_pMilModel)						return FALSE;
#ifndef _NOT_USE_MIL_
		if (!m_pMilModel[0]->IsSetMarkModel(mark_no))	return FALSE;
#endif

#ifndef _NOT_USE_MIL_
		/* Edge Detecting ���� */
		if (!m_pMilModel[0]->RunModelFind(m_mGraphID, m_pLastGrabResult->GetBufID(), FALSE, img_id, dlg_id, mark_no, FALSE, 0)) // lk91 �̻������ �ӽ� ������
		{
			return FALSE;
		}
		if (!m_pMilModel[0]->IsFindModCount(2, score, scale))
		{
			return FALSE;
		}
		for (i=0; i<2; i++)
		{
			pstGet	= m_pMilModel[0]->GetFindMark(i);
			pstGet->mark_width	= m_pMilModel[0]->GetModelWidth(i);
			pstGet->mark_height	= m_pMilModel[0]->GetModelHeight(i);
			if (pstGet->IsValidMark(score, scale))
			{
				m_pLastGrabResult->SetGrabbedMark(0xff, pstGet,
												  m_pMilModel[0]->GetFindMarkAll(),
												  m_pMilModel[0]->GetFindMarkAllCount(),
												  pstGet->mark_width, pstGet->mark_height);
				if (results)
				{
					/* ��ȯ ����ü�� ���� ���� (����Ʈ �ּұ��� �����ع����� �ȵǹǷ� ...) */
					memcpy(&results[j], m_pLastGrabResult->GetGrabbedMark(),
						   sizeof(STG_ACGR)-sizeof(PUINT8));
					/* Grabbed Image���� ���Ե� */
					memcpy(results[j].grab_data, image, width * height);
					/* �˻��� Mark ũ�� ���� ������ �ٽ� �����ؾ� �� */
					results[j].mark_width_px	= m_pMilModel[0]->GetModelWidth(j);
					results[j].mark_height_px	= m_pMilModel[0]->GetModelHeight(j);

					/* ���� Array�� �����ϱ� ���԰� ���ÿ�, 2�� ��� �˻��Ǿ����� ���� */
					if (++j == 0x02)	bIsFindAll = TRUE;
				}
			}
		}
#endif
	}

	return bIsFindAll;
}

/*
 desc : Camera�κ��� ������ Grabbed Image (Binary Image Data)�� Edge Detection
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
		grab	- [in]  Grabbed Image ������ ����� ����ü ������
		saved	- [in]  �м��� edge image ���� ����
 retn : TRUE or FALSE
*/
BOOL CMilMain::RunEdgeDetect(UINT8 cam_id, LPG_ACGR grab, UINT8 saved)
{
	UINT8 u8GFilter		= m_pstConfig->edge_find.gaussian_filer;
	BOOL bSucc			= FALSE;
	UINT32 u32GFilter	= 0;
#ifndef _NOT_USE_MIL_
	MIL_ID mlGFilter	= NULL, mlResult;

	/* Grabbed Image ó���� ���� ��ü ���� */
	m_pLastGrabResult	= m_pMilGrab[grab->cam_id-1][0];
	if (!m_pLastGrabResult->PutGrabImage(grab->grab_data, 0 /* fixed */,
										 0.0f /* fixed */, 0.0f /* fixed */))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"Failed to put the grabbed image");
	}
	else
	{
		/* Edge Detecting ���� */
		if (!m_pMilModel)			return FALSE;
		if (!m_pMilModel[cam_id-1])	return FALSE;
		if (MappGetError(M_CURRENT, M_NULL))	return FALSE;

		/* ���� Grabbed Image�� ID */
		mlResult	= m_pLastGrabResult->GetBufID();
		/* ����þ� ���� ���� ���ο� ���� */
		if (0x00 == u8GFilter)
		{
			/* Edge Detect ���� */
			if (m_pMilModel[cam_id-1]->RunEdgeDetect(mlResult, grab->grab_width, grab->grab_height, saved))
			{
				/* �÷��� ���� */
				bSucc	= TRUE;
			}
		}
		else
		{
			/* Gaussian ���� ID �Ҵ� */
			mlGFilter	= MbufAllocColor(m_mSysID, 1,
										 grab->grab_width,
										 grab->grab_height,
										 8+M_UNSIGNED,		/* == (8+M_UNSIGNED)	: data depth (8bit)�̸� Unsigned ������ �Ҵ� */
										 M_IMAGE+M_PROC,	/* == (M_IMAGE+M_PROC)	: �̹�����+����ó���� ���� ����ϰڴٴ� �ǹ� */
															/* M_IMAGE+M_GRAB+M_DISP: �̹�����+����ȹ���+������¿� ���� ����ϰڴٴ� �ǹ� */
										 M_NULL);
			if (M_NULL == mlGFilter)	return FALSE;
			/* Gaussian ó�� */
			u32GFilter	= m_pLastGrabResult->GetGaussianFilter(u8GFilter);
			MimConvolve(mlResult, mlGFilter, u32GFilter);
			/* Edge Detect ���� */
			if (m_pMilModel[cam_id-1]->RunEdgeDetect(mlGFilter, grab->grab_width, grab->grab_height, saved))
			{
				/* �÷��� ���� */
				bSucc	= TRUE;
			}

			/* ���� �޸� ���� */
			MbufFree(mlGFilter);
		}
	}
#endif

	return bSucc;
}

/*
 desc : Find a marker in an image and take the specified measurements
 parm : cam_id	- [in]  Align Camera ID (1 or 2 or later)
		image	- [in]  Bitmap Image Buffer
		param	- [in]  Set the marker information
		results	- [out] �˻��� ��� ������ ����� ����ü ���� ������
 retn : TRUE or FALSE
*/
BOOL CMilMain::RunMarkerStrip(UINT8 cam_id, PUINT8 image, LPG_MSMP param, STG_MSMR &results)
{
	if (m_pstConfig->set_cams.acam_count < cam_id)	return FALSE;

	/* Grabbed Image ó���� ���� ��ü ���� */
	m_pLastGrabResult	= m_pMilGrab[0][0];	/* Align Camera 1 - Fixed */
	if (!m_pLastGrabResult)	return FALSE;
	if (!m_pLastGrabResult->PutGrabImage(image, 0x00))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"Failed to put the grabbed image");
	}
	else
	{
#ifndef _NOT_USE_MIL_
		/* Edge Detecting ���� */
		if (!m_pMilModel[cam_id-1]->RunMarkerStrip(m_mGraphID, m_pLastGrabResult->GetBufID(),
												   param, results))
		{
			return FALSE;
		}
#endif
	}

	return TRUE;
}

/*
 desc : ���� �ֱٿ� �м� (Grabbed, Edge, Line, Find)�� �̹������� Ư�� ������ �����ؼ� Bitmap Image�� ���
 parm : cam_id	- [in]  Align Camera ID (1 or Later)
		area	- [in]  �����ϰ��� �ϴ� ���� ������ ����� �ȼ� ��ġ ��
		type	- [in]  0x00 : Edge, 0x02 : Line, 0x02 : Match, 0x03 : Grab
		file	- [in]  Mark Template�� �����Ϸ��� ���� �̸� (��ü ��� ����)
 retn : TRUE or FALSE
*/
BOOL CMilMain::SaveGrabbedMarkToFile(UINT8 cam_id, LPRECT area, UINT8 type, TCHAR *file)
{
	BOOL bSucc		= TRUE;
#ifndef _NOT_USE_MIL_
	UINT32 i32Width	= 0, i32Height = 0, i32TotalSize = 0;
	PUINT8 pBufMark	= NULL;
	MIL_ID mBufID	= M_NULL;

	if (!m_pLastGrabResult || cam_id < 0x01)	return FALSE;

	/* AOI ���� ũ�� */
	i32Width	= area->right - area->left;
	i32Height	= area->bottom - area->top;
	/* AOI ���� ���� �Ҵ� */
	i32TotalSize= /*sizeof(UINT8) * */i32Width * i32Height;
	pBufMark = new UINT8[i32TotalSize + 1];// (PUINT8)::Alloc(i32TotalSize + 1);
	ASSERT(pBufMark);
	pBufMark[i32TotalSize]	= 0x00;

	switch (type)
	{
	case 0x00	:	/* Edge Detection */
		MbufGet2d(m_pMilModel[cam_id-1]->GetBufEdgeID(), area->left, area->top, i32Width, i32Height, pBufMark);
		break;
	case 0x01	:	/* Line Detection */
		break;
	case 0x02	:	/* Match or Grabbed */
	case 0x03	:	/* Grabbed */
		MbufGet2d(m_pLastGrabResult->GetBufID(), area->left, area->top, i32Width, i32Height, pBufMark);
		break;
	}
	if (MappGetError(M_GLOBAL, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"Failed to assign the aoi area to image buffer");
		bSucc	= FALSE;
	}

	/* Image Buffer ��ü �Ҵ� (2d ��, ���) */
	MbufAlloc2d(m_mSysID,
				i32Width,
				i32Height,
				8+M_UNSIGNED,	/* == (8+M_UNSIGNED)	: data depth (8bit)�̸� Unsigned ������ �Ҵ� */
				M_IMAGE,		/* == (M_IMAGE+M_PROC)	: �̹�����+����ó���� ���� ����ϰڴٴ� �ǹ� */
								/* M_IMAGE+M_GRAB+M_DISP: �̹�����+����ȹ���+������¿� ���� ����ϰڴٴ� �ǹ� */
				&mBufID);
	if (!mBufID || MappGetError(M_CURRENT, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"Failed to allocate <MappAlloc : Mark Template>");
		return FALSE;
	}
	/* Image Buffer Clear */
	MbufClear(mBufID, 1L);
	/* Put the image to image buffer */
	MbufPut2d(mBufID, 0, 0, i32Width, i32Height, pBufMark);
	if (MappGetError(M_GLOBAL, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"Failed to put the aoi area to image buffer");
		bSucc	= FALSE;
	}
	/* Save to file */
	MbufExport(file, M_BMP, mBufID);
	/* Image Buffer Release */
	MbufFree(mBufID);

	/* ���� �޸� ���� */
	if (pBufMark)	delete pBufMark;
#endif
	return bSucc;
}

/*
 desc : Mark Model ���� ���� (���)
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
BOOL CMilMain::SetModelDefine(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
							  LPG_CMPV model, UINT8 mark_no,
							  DOUBLE scale_min, DOUBLE scale_max,
							  DOUBLE score_min, DOUBLE score_tgt)
{
	if (!m_pMilModel)			return FALSE;
	if (!m_pMilModel[cam_id-1])	return FALSE;
	/* Model ��� */
	if (!m_pMilModel[cam_id-1]->SetModelDefine(speed, level, count, smooth, model, mark_no,
											   scale_min, scale_max, score_min, score_tgt))
	{
		return FALSE;
	}
	return TRUE;
}

/*
 desc : Mark Model ���� ���� (���)
 parm : cam_id		- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		detail_level- [in]  ��ϵ� �� �̹����� Grabbed Image�̹����� Edge ������ ���� �� ���� ��
							���� Ŭ���� ���� ��������, �ӵ��� ���� ��������
							LOW���� HIGH�� ������ �� ���� Edge�� ������ ���ϰ� ��
							0 (M_MEDIUM), 1 (M_HIGH), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - �⺻ ��, 1.0f ~ 100.0f
							�⺻ ���� 50������, 100���� ������ ������ ������ ���͸� ��Ÿ���ϴ�.
		model		- [in]  Model Type ��, circle, square, rectangle, cross, diamond, triangle
		param1~5	- [in]  �� 5���� Parameter Values (unit : um)
		count		- [in]  ����ϰ��� �ϴ� ���� ����	
		scale_min	- [in]  �˻� ����� ũ�� ���� �� ���� (�ּ��� �� �� �������� Ŀ�� ��. ����: 0.5 ~ 1.0)
		scale_max	- [in]  �˻� ����� ũ�� ���� �� ���� (�ִ��� �� �� �������� �۾ƾ� ��. ����: 1.0 ~ 1.0)
		score_min	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0)
		score_tgt	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0))
 retn : TRUE or FALSE
*/
BOOL CMilMain::SetModelDefine(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
							  PUINT32 model, DOUBLE *param1, DOUBLE *param2, DOUBLE *param3,
							  DOUBLE *param4, DOUBLE *param5, UINT8 mark_no,
							  DOUBLE scale_min, DOUBLE scale_max, DOUBLE score_min, DOUBLE score_tgt, bool sameMark)
{
	UINT8 i	= 0x00, j = 0x00;

	if (!m_pMilModel)			return FALSE;
	if (!m_pMilModel[cam_id-1])	return FALSE;
	/* Model ��� */
	if (!m_pMilModel[cam_id-1]->SetModelDefine(speed, level, count, smooth, model, param1, param2,
											   param3, param4, param5, mark_no,
												scale_min, scale_max, score_min, score_tgt, sameMark))	return FALSE;
	/* �������� ��ũ �˻� ���� �� �ʱ�ȭ */
	for (i=0; i<m_pstConfig->set_cams.acam_count; i++)
	{
		for (j=0; j<m_pstConfig->mark_find.max_mark_grab; j++)
		{
			m_pMilGrab[i][j]->ResetRecipeMarkRate();
		}
	}

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
		scale_min	- [in]  �˻� ����� ũ�� ���� �� ���� (�ּ��� �� �� �������� Ŀ�� ��. ����: 0.5 ~ 1.0)
		scale_max	- [in]  �˻� ����� ũ�� ���� �� ���� (�ִ��� �� �� �������� �۾ƾ� ��. ����: 1.0 ~ 1.0)
		score_min	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0)
		name		- [in]  Model Name
		file		- [in]  �� �̹����� ����� ���� �̸� (��ü ��� ����. Image File)
		score_tgt	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0))
 retn : TRUE or FALSE
*/
BOOL CMilMain::SetModelDefineLoad(UINT8 cam_id, UINT8 speed, UINT8 level, DOUBLE smooth,
								  DOUBLE scale_min, DOUBLE scale_max,
								  DOUBLE score_min, DOUBLE score_tgt,
								  PTCHAR name, CStringArray &file)
{
	return m_pMilModel[cam_id-1]->SetModelDefineLoad(speed, level, smooth,
													 scale_min, scale_max, score_min, score_tgt,
													 name, file);
}

/*
 desc : ���� ��ϵ� Mark Model ���� ��ȯ
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : ��ϵ� ����ü ������ ��ȯ
*/
LPG_CMPV CMilMain::GetModelDefine(UINT8 cam_id)
{
	if (cam_id >= MAX_ALIGN_CAMERA)	return NULL;
	if (!m_pMilModel)				return NULL;
	if (!m_pMilModel[cam_id-1])		return NULL;

	return m_pMilModel[cam_id-1]->GetModelDefine();
}

/*
 desc : Mark Pattern ��� ����
 parm : mode	- [in]  0x00 : Expose, 0x01 : Vstep
		cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : TRUE or FALSE
*/
BOOL CMilMain::IsSetMarkModel(UINT8 mode, UINT8 cam_id, UINT8 mark_no)
{
	UINT8 i	= 0x00;

	if (!m_pMilModel)	return FALSE;
	/* Align Expose Mode�� ��� */
	if (0x00 == mode)
	{
		/* �ּ������� Global Guide�� 4���� �����Ǿ� �ִ��� Ȯ�� */
		for (; i<m_pstConfig->set_cams.acam_count; i++)
		{
#ifndef _NOT_USE_MIL_
			if (!m_pMilModel[i]->IsSetMarkModel(mark_no))		return FALSE;
#endif
		}
	}
	/* VStep Mode�� ��� */
	else
	{
#ifndef _NOT_USE_MIL_
		if (!m_pMilModel[cam_id-1]->IsSetMarkModel(mark_no))	return FALSE;
#endif
	}
	
	return TRUE;
}

/*
 desc : Mark Pattern ��� ����
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : TRUE or FALSE
*/
BOOL CMilMain::IsSetMarkModelACam(UINT8 cam_id, UINT8 mark_no)
{
	if (!m_pMilModel)	return FALSE;

#ifndef _NOT_USE_MIL_
	return m_pMilModel[cam_id-1]->IsSetMarkModel(mark_no);
#else
	return TRUE;
#endif
}

/*
 desc : ������ ���� (���)�Ǿ� �ִ� ��� Mark ���� ����
 parm : None
 retn : None
*/
VOID CMilMain::ResetMarkModel()
{
	UINT8 i	= 0x00;
	for (; i<m_pstConfig->set_cams.acam_count; i++)
	{
		for (int j = 0; j < 2; j++) { // Global, Local 2��
			for (int k = 0; k < 2; k++) { // PAT, MOD 2��
				if (m_pMilModel && m_pMilModel[i])	m_pMilModel[i]->ReleaseMarkModel(j, k);
			}
		}
	}
}

/*
 desc : ���� ��ϵ� Mark Model Type ��ȯ
 parm : cam_id	- [in]  Align Camera Index (0x01 ~ MAX_INSTALL_CAMERA)
		mark_id	- [in]  Align Mark Index (0x00 ~ MAX_REGIST_MODEL-1)
 retn : Model Type ��
*/
UINT32 CMilMain::GetMarkModelType(UINT8 cam_id, UINT8 index)
{
	if (!m_pMilModel)			return 0;
	if (!m_pMilModel[cam_id-1])	return 0;
	return m_pMilModel[cam_id-1]->GetMarkModelType(index);
}

/*
 desc : ���� ��ϵ� Mark Model ũ�� ��ȯ
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA)
		index	- [in]  ��û�ϰ��� �ϴ� ���� ��ġ (Zero-based)
		flag	- [in]  0x00 : ����  ũ��, 0x01 : ���� ũ��
		unit	- [in]  0x00 : um, 0x01 : pixel
 retn : ũ�� ��ȯ (����: um)
*/
DOUBLE CMilMain::GetMarkModelSize(UINT8 cam_id, UINT8 index, UINT8 flag, UINT8 unit)
{
	if (!m_pMilModel[cam_id-1])	return 0.0f;

	return m_pMilModel[cam_id-1]->GetMarkModelSize(index, flag, unit);
}

#ifndef _NOT_USE_MIL_
/*
 desc : Get the most recently stored buffer ID value
 parm : cam_id	- [in]  align camera id (1 or 2)
		type	- [in]  0x00: Edge, 0x01: Line, 0x02: Strip (Measurement)
 retn : MIL_ID
*/
MIL_ID CMilMain::GetBufID(UINT8 cam_id, UINT8 type)
{
	if (m_pstConfig->set_cams.acam_count < cam_id || cam_id < 1)	return M_NULL;

	switch (type)
	{
	case 0x00 :	return m_pMilModel[cam_id-1]->GetBufEdgeID();
	case 0x01 :	return m_pMilModel[cam_id-1]->GetBufEdgeID();
	case 0x02 :	return m_pMilModel[cam_id-1]->GetBufStripID();
	}

	return M_NULL;
}

/*
 desc : Grabbed Image�� ������ ���� ID ��ȯ
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : grabbed image result ��ȯ
*/
MIL_ID CMilMain::GetMarkBufID(UINT8 cam_id, UINT8 img_id)
{
	CMilGrab *pGrab	= m_pMilGrab[cam_id-1][img_id];
	if (!pGrab)	return NULL;
#if 0
	if (pGrab->GetGrabbedMarkCount() < 0x01)	return NULL;
#endif
	/* Get the last buffer id */
	return pGrab->GetBufID();
}
#endif

/*
 desc : Edge Detection ���� �� �˻��� ��� ���� ��ȯ
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
 ��ȯ : ���� ��ȯ. ������ ���, 0 ���� ��
*/
INT32 CMilMain::GetEdgeDetectCount(UINT8 cam_id)
{
	if (!m_pMilModel)	return 0;
	return m_pMilModel[cam_id-1]->GetEdgeDetectCount();
}

/*
 desc : Align Mark �˻� ��� ����
 parm : method	- [in]  0x00 : ������ 1�� ��ȯ, 0x01 : ���� ��ũ �������� ����ġ�� �ο��Ͽ� ��� ��ȯ
		count	- [in]  'method' ���� 1�� ���, ���� �˻��� Mark ���� �� 2 �̻� ���̾�� ��
 retn : None
*/
VOID CMilMain::SetMarkMethod(ENG_MMSM method, UINT8 count)
{
	if (!m_pMilModel)	return;

	UINT8 i	= 0x00;
	for (; i < m_pstConfig->set_cams.acam_count; i++)
	{
		if (m_pMilModel[i])
		{
			m_pMilModel[i]->SetMarkMethod(method, count);
		}
	}
}

/*
 desc : �̹����� ���� Ȥ�� ������ ��輱 �߽� ��ġ ���
 parm : cam_id		- [in]  Align Camera ID (1 or 2)
		grab_data	- [in]  Grabbed Image ������ ����� ����ü ������
		mil_result	- [in]  MIL ���� �˻� ��� ��ȭ ���� ��� ����
 retn : TRUE or FALSE
*/
BOOL CMilMain::RunLineCentXY(UINT8 cam_id, LPG_ACGR grab_data, BOOL mil_result)
{
	BOOL bSucc		= TRUE, bCreate	= FALSE;
#ifndef _NOT_USE_MIL_
	INT32 i32Size	= 0;
	MIL_INT64 mlAttr= M_IMAGE+M_PROC+M_DISP;
	MIL_ID mlBufID	= M_NULL;

	/* ���� �̹��� ���� ���� */
	mlBufID	= MbufAlloc2d(m_mSysID, grab_data->grab_width, grab_data->grab_height,
						  8+M_UNSIGNED, mlAttr, M_NULL);
	if (!mlBufID)	return FALSE;
	/* �̹��� ���ۿ� ���� Grabbed Image ������ ���� */
	MbufPut2d(mlBufID, 0, 0, grab_data->grab_width, grab_data->grab_height, grab_data->grab_data);
	/* Line Edge Detection ���� */
	bSucc	= m_pMilModel[cam_id-1]->RunLineCentXY(mlBufID, mil_result);
	/* �޸� ����  */
	MbufFree(mlBufID);
#endif
	return bSucc;
}


UINT8 CMilMain::GetMarkFindSetCount(int camNum)
{ return m_pMilModel[camNum-1]->GetMarkFindSetCount(); }
UINT8 CMilMain::GetMarkFindedCount(int camNum) 
{ return m_pMilModel[camNum-1]->GetMarkFindedCount(); }

/*
 desc : ��ũ ��� ��� (Type)
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
 retn : ��ũ ��� ��� �� ��ȯ
*/
ENG_MMSM CMilMain::GetMarkMethod(UINT8 cam_id)
{
	return m_pMilModel[cam_id-1]->GetMarkMethod();
}

/*
 desc : Iamge ȸ��
 parm : img_src		- [in]  ���� �̹��� ����
		img_dst		- [out] ȸ���� �̹��� ����
		width		- [in]	���� �̹����� ����
		height		- [in]	���� �̹����� ����
		angle		- [in]	ȸ�� �ϰ��� �ϴ� ���� (��� ��. 0 ~ 359.99999)
		rotate_x	- [in]	ȸ���ϰ��� �ϴ� Point ��ġ X
		rotate_y	- [in]	ȸ���ϰ��� �ϴ� Point ��ġ Y
 retn : None
*/
VOID CMilMain::ImageRotate(PUINT8 img_src, PUINT8 img_dst, UINT32 width, UINT32 height,		
						   DOUBLE angle, UINT32 rotate_x, UINT32 rotate_y)
{
#ifndef _NOT_USE_MIL_
	TCHAR tzPath[MAX_PATH_LEN] = { NULL }, tzFile[MAX_PATH_LEN] = { NULL };

	MIL_ID mlSrcBufID = M_NULL;
	MIL_ID mlDesBufID = M_NULL;

	/* Image Src Buffer ��ü �Ҵ� (2d ��, ���) */
	MbufAlloc2d(m_mSysID, width, height, 8+M_UNSIGNED, M_IMAGE+M_PROC, &mlSrcBufID);
	MbufAlloc2d(m_mSysID, width, height, 8+M_UNSIGNED, M_IMAGE+M_PROC, &mlDesBufID);
	if (!mlSrcBufID || !mlDesBufID ||  MappGetError(M_CURRENT, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"Failed to allocate <MbufAlloc2d : ImageRotate>");
		return;
	}
	
	/* Image Buffer Clear */
	MbufClear(mlSrcBufID, 1L);
 	MbufClear(mlDesBufID, 1L);

	/* Put the image to image buffer */
	MbufPut2d(mlSrcBufID, 0, 0, width, height, img_src);
	/* Image Rotate */
#if 0
	MimRotate(mlSrcBufID, mlDesBufID, angle,
			  (MIL_DOUBLE)rotate_x, (MIL_DOUBLE)rotate_y, 
			  (MIL_DOUBLE)rotate_x, (MIL_DOUBLE)rotate_y, M_NEAREST_NEIGHBOR);
#else
	MimRotate(mlSrcBufID, mlDesBufID, angle,
			  M_DEFAULT, M_DEFAULT,
			  M_DEFAULT, M_DEFAULT, M_NEAREST_NEIGHBOR);
#endif

	if (MappGetError(M_GLOBAL, M_NULL))
		LOG_ERROR(ENG_EDIC::en_mil, L"Failed to put the aoi area to image buffer");

	/* Get the image  to image buffer */
	MbufGet2d(mlDesBufID, 0, 0, width, height, img_dst);
	/* Image Buffer Release */
	MbufFree(mlSrcBufID);
	MbufFree(mlDesBufID);
#endif
}

/*
 desc : Perform a horizontal or vertical image-flipping operation
 parm : img_src		- [in]  Buffer of source image
		img_dst		- [out] Buffer of flipping image
		width		- [in]	Size of source image - width (pixel)
		height		- [in]	Size of source image - height (pixel)
		flag		- [in]	0x00 : Horizontal reference, 0x01 : Vertical reference
 retn : None
*/
VOID CMilMain::ImageFlip(PUINT8 img_src, PUINT8 img_dst, UINT32 width, UINT32 height, UINT8 flag)
{
#ifndef _NOT_USE_MIL_
	TCHAR tzPath[MAX_PATH_LEN] = { NULL }, tzFile[MAX_PATH_LEN] = { NULL };
	MIL_INT mlOperation	= flag == 0x00 ? M_FLIP_HORIZONTAL : M_FLIP_VERTICAL;
	MIL_ID mlSrcBufID	= M_NULL;
	MIL_ID mlDesBufID	= M_NULL;

	/* Image Src Buffer ��ü �Ҵ� (2d ��, ���) */
	MbufAlloc2d(m_mSysID, width, height, 8+M_UNSIGNED, M_IMAGE+M_PROC, &mlSrcBufID);
	MbufAlloc2d(m_mSysID, width, height, 8+M_UNSIGNED, M_IMAGE+M_PROC, &mlDesBufID);
	if (!mlSrcBufID || !mlDesBufID ||  MappGetError(M_CURRENT, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"Failed to allocate <MbufAlloc2d : ImageFlip>");
		return;
	}
	
	/* Image Buffer Clear */
	MbufClear(mlSrcBufID, 1L);
 	MbufClear(mlDesBufID, 1L);

	/* Put the image to image buffer */
	MbufPut2d(mlSrcBufID, 0, 0, width, height, img_src);
	/* Image Flipping */
	MimFlip(mlSrcBufID, mlDesBufID, mlOperation, M_DEFAULT);

	if (MappGetError(M_GLOBAL, M_NULL))
		LOG_ERROR(ENG_EDIC::en_mil, L"Failed to put the aoi area to image buffer");


	/* Get the image  to image buffer */
	MbufGet2d(mlDesBufID, 0, 0, width, height, img_dst);

	/* Image Buffer Release */
	MbufFree(mlSrcBufID);
	MbufFree(mlDesBufID);
#endif
}

/*
 desc : ���� MIL ���̼����� ��ȿ���� ���� Ȯ��
		Pattern, Model Finder, Edge Finder, Read, OCR, Image Processing �� ��ȿ���� Ȯ��
 parm : None
 retn : TRUE or FALS
*/
BOOL CMilMain::IsLicenseValid()
{
#ifndef _NOT_USE_MIL_
	MIL_INT mlValid	= M_NULL;
#ifdef _DEBUG
	MIL_INT mlType	= M_LICENSE_IM|M_LICENSE_MEAS|M_LICENSE_PAT|M_LICENSE_MOD|M_LICENSE_OCR|M_LICENSE_EDGE;
#else
	MIL_INT mlType	= M_LICENSE_IM|M_LICENSE_MOD|M_LICENSE_EDGE;
#endif
	if (!m_mSysID)	return FALSE;

	/* Lincese Check */
	mlValid	= MsysInquire(m_mSysID, M_LICENSE_MODULES, M_NULL);

	/* ���� ��ȿ���� Ȯ�� */
	return (mlType == (mlValid & mlType));
#else 
	return TRUE;
#endif
}

/*
 desc : �뱤 ��� ���� ��, ���� �뱤, ����� �뱤, ���� �� ����� �뱤
 parm : mode	- [in]  ���� �뱤 (0x00), ����� �뱤 (0x01), ����� ī�޶� ���� �� ���� �� ����� �뱤 (0x02)
 retn : None
*/
VOID CMilMain::SetAlignMode(ENG_AOEM mode)
{
	UINT8 i, j;

	if (!m_pstConfig)	return;

	for (i=0; i<m_pstConfig->set_cams.acam_count; i++)
	{
		for (j=0; j<m_pstConfig->mark_find.max_mark_grab; j++)
		{
			m_pMilGrab[i][j]->SetAlignMode(mode);
		}
	}
}

/*
 desc : ī�޶�κ��� ������ �̹��� ������ ������ ���Ϸ� ����
 parm : file	- [in]  �����Ϸ��� ���� (��ü ��� ����)
		width	- [in]  �̹����� ũ�� (����: pixel)
		height	- [in]  �̹����� ũ�� (����: pixel)
		image	- [in]  �̹����� ����� ����
 retn : TRUE or FALSE
*/
BOOL CMilMain::SaveGrabbedToFile(PTCHAR file, INT32 width, INT32 height, PUINT8 image)
{
	BOOL bSucc		= TRUE;
#ifndef _NOT_USE_MIL_
	UINT32 i32Size	= 0;
	MIL_ID mBufID	= M_NULL;

	/* Image Buffer ��ü �Ҵ� (2d ��, ���) */
	MbufAlloc2d(m_mSysID,
				width,
				height,
				8+M_UNSIGNED,	/* == (8+M_UNSIGNED)	: data depth (8bit)�̸� Unsigned ������ �Ҵ� */
				M_IMAGE,		/* == (M_IMAGE+M_PROC)	: �̹�����+����ó���� ���� ����ϰڴٴ� �ǹ� */
								/* M_IMAGE+M_GRAB+M_DISP: �̹�����+����ȹ���+������¿� ���� ����ϰڴٴ� �ǹ� */
				&mBufID);
	if (!mBufID || MappGetError(M_CURRENT, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"Failed to allocate <MappAlloc : Mark Template>");
		return FALSE;
	}
	/* Image Buffer Clear */
	MbufClear(mBufID, 1L);
	/* Put the image to image buffer */
	MbufPut2d(mBufID, 0, 0, width, height, image);
	if (MappGetError(M_GLOBAL, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"Failed to put the aoi area to image buffer");
		bSucc	= FALSE;
	}
	/* Save to file */
	MbufExport(file, M_BMP, mBufID);
	/* Image Buffer Release */
	MbufFree(mBufID);
#endif
	return TRUE;
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
VOID CMilMain::SetRecipeMarkRate(DOUBLE score, DOUBLE scale)
{
	UINT8 i	= 0x00, j = 0x00;

	if (!m_pMilGrab)	return;
	if (!m_pMilModel)	return;
	if (!m_pstConfig)	return;

	for (; i<m_pstConfig->set_cams.acam_count; i++)
	{
		if (m_pMilGrab[i])
		{
			for (j = 0; j < m_pstConfig->mark_find.max_mark_grab; j++)
			{
				m_pMilGrab[i][j]->SetRecipeMarkRate(score, scale);
			}
		}
		
		m_pMilModel[i]->SetRecipeMarkRate(score, scale);
	}
}

/*
 desc : ���������� �̵� ���� ���� (������ �̵����� / ������ �̵����� ����)
 parm : direct	- [in]  TRUE (������ �̵� : �տ��� �ڷ� �̵�), FALSE (������ �̵�)
 retn : None
*/
VOID CMilMain::SetMoveStateDirect(BOOL direct)
{
	UINT8 i, j;

	if (!m_pMilGrab)	return;

	if (!m_pstConfig)	return;

	for (i=0; i<m_pstConfig->set_cams.acam_count; i++)
	{
		for (j=0; j<m_pstConfig->mark_find.max_mark_grab; j++)
		{
			if (m_pMilGrab[i][j])
			{
				m_pMilGrab[i][j]->SetMoveStateDirect(direct);
			}
		}
	}
}

/*
 desc : Mulit Mark �˻� ����� ���, ��, MMF ��ũ �˻� ����� ���, �˻��� ��ũ ������ �ֺ��� �׸��� ���� �뵵
		Mark �� MMF ������ ���, ��ũ ũ�� ��
 parm : width	- [in]  �簢�� ������ ���� �� (����: um)
		height	- [in]  �簢�� ������ ���� �� (����: um)
 retn : None
*/
VOID CMilMain::SetMultiMarkArea(UINT32 width, UINT32 height)
{
	UINT8 i	= 0x00;

	if (!m_pMilModel)	return;

	for (i=0; i<m_pstConfig->set_cams.acam_count; i++)
	{
		if (m_pMilModel[i])
		{
			m_pMilModel[i]->SetMultiMarkArea(width, height);
		}
	}
}

/* ----------------------------------------------------------------------------------------- */
/*                                 lk91 VISION �߰� �Լ�                                     */
/* ----------------------------------------------------------------------------------------- */
/* desc: MIL Main �ռ� ���� �ʱ�ȭ */
VOID CMilMain::InitMilMain(LPG_CIEA config,  LPG_VDSM shmem)
{
	m_pstConfig = config;
	m_pstShMemVisi = shmem;

	CAM_CNT = m_pstConfig->set_cams.acam_count;
	GRABMARK_CNT = m_pstConfig->set_cams.acam_count * m_pstConfig->mark_find.max_mark_grab; 
	MAX_MARK_GRAB = m_pstConfig->mark_find.max_mark_grab;

#ifndef _NOT_USE_MIL_
	m_mImg = new MIL_ID[CAM_CNT];
	m_mImgProc = new MIL_ID[CAM_CNT];
	m_mOverlay_CALI_STEP = new MIL_ID[CAM_CNT];

	m_mDisID_Mark = new MIL_ID[GRABMARK_CNT];
	m_mOverlay_Mark = new MIL_ID[GRABMARK_CNT];
	m_mImgDisp_Mark = new MIL_ID[GRABMARK_CNT];
	m_mImg_Mark = new MIL_ID[GRABMARK_CNT];
	m_mImg_Grab = new MIL_ID[GRABMARK_CNT];
	MilEdgeResult = new MIL_ID[GRABMARK_CNT];
	m_mDisID_CALI_STEP = new MIL_ID[CAM_CNT];
	m_mImgDisp_CALI_STEP = new MIL_ID[CAM_CNT];
	m_mImg_CALI_CAMSPEC = new MIL_ID[CAM_CNT];
	m_mImg_CALI_STEP = new MIL_ID[CAM_CNT];
	m_mImg_ACCR = new MIL_ID[CAM_CNT];

	m_mImgDisp_EXPO = new MIL_ID[GRABMARK_CNT];
	m_mDisID_EXPO = new MIL_ID[GRABMARK_CNT];
	m_mOverlay_EXPO = new MIL_ID[GRABMARK_CNT];

	for (int i = 0; i < CAM_CNT; i++) {
		m_mImg[i] = M_NULL;
		m_mImgProc[i] = M_NULL;
		m_mOverlay_CALI_STEP[i] = M_NULL;
		m_mDisID_CALI_STEP[i] = M_NULL;
		m_mImgDisp_CALI_STEP[i] = M_NULL;
		m_mImg_CALI_CAMSPEC[i] = M_NULL;
		m_mImg_CALI_STEP[i] = M_NULL;
		m_mImg_ACCR[i] = M_NULL;
	}
	for (int i = 0; i < GRABMARK_CNT; i++) {
		m_mDisID_Mark[i] = M_NULL;
		m_mOverlay_Mark[i] = M_NULL;
		m_mImgDisp_Mark[i] = M_NULL;
		m_mImg_Mark[i] = M_NULL;
		m_mImg_Grab[i] = M_NULL;
		MilEdgeResult[i] = M_NULL;

		m_mDisID_EXPO[i] = M_NULL;
		m_mImgDisp_EXPO[i] = M_NULL;
		m_mOverlay_EXPO[i] = M_NULL;
	}
	
	m_mlDisID				= M_NULL;
	m_mDisID_Mark_Live		= M_NULL;
	m_mDisID_MarkSet		= M_NULL;
	m_mDisID_MMPM			= M_NULL;
	m_mDisID_CALI_CAMSPEC	= M_NULL;
	m_mDisID_ACCR			= M_NULL;
	m_mDisID_MPMM_AutoCenter	= M_NULL;
	//m_mGraphID				= M_NULL;
	m_mOverlay_Mark_Live	= M_NULL;
	m_mOverlay_MarkSet		= M_NULL;
	m_mOverlay_MMPM			= M_NULL;
	m_mOverlay_CALI_CAMSPEC	= M_NULL;
	m_mOverlay_ACCR			= M_NULL;
	m_mOverlay_MPMM_AutoCenter = M_NULL;
	m_mImgDisp_Mark_Live	= M_NULL;
	m_mImgDisp_MarkSet		= M_NULL;
	m_mImgDisp_MMPM			= M_NULL;
	m_mImgDisp_CALI_CAMSPEC	= M_NULL;
	m_mImgDisp_ACCR			= M_NULL;
	m_mImgDisp_MPMM_AutoCenter = M_NULL;
	m_mImg_MarkSet			= M_NULL;
	m_mImg_MarkSet_Ori		= M_NULL;
	m_mImg_MPMM_AutoCenter		= M_NULL;
	m_mImg_MPMM_AutoCenter_Proc = M_NULL;

	MilMask = M_NULL;

	hMaskBuf = NULL;

#endif
	rectSearhROI = new CRect[CAM_CNT];

	theApp.clMilDisp.InitMilDisp();
}

/* desc: Mark ���� ���� - MMF (MIL Model Find File) */
BOOL CMilMain::SetModelDefineMMF(UINT8 cam_id, PTCHAR name, PTCHAR mmf, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no)
{
	if (!m_pMilModel)	return FALSE;
	return m_pMilModel[cam_id - 1]->SetModelDefineMMF(name, mmf, m_MarkSizeP, m_MarkCenterP, mark_no);
}

/* desc: Mark ���� ���� - PAT (MIL Pattern Matching Mark File) */
BOOL CMilMain::SetModelDefinePAT(UINT8 cam_id, PTCHAR name, PTCHAR pat, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no)
{
	if (!m_pMilModel)	return FALSE;
	return m_pMilModel[cam_id - 1]->SetModelDefinePAT(name, pat, m_MarkSizeP, m_MarkCenterP, mark_no);
}

VOID CMilMain::SetAlignMotionPtr(AlignMotion& ptr)
{
	alignMotionPtr = &ptr;	

	for (int i = 0; i < m_pstConfig->set_cams.acam_count; i++)
	{
		for (int j = 0; j < m_pstConfig->mark_find.max_mark_grab; j++)
		{
			m_pMilGrab[i][j]->SetAlignMotionptr(alignMotionPtr);
		}
	}
}

/* desc: Set Search ROI */
VOID CMilMain::MilSetMarkROI(UINT8 cam_id, CRect fi_rectSearhROI)
{
	if (!m_pMilModel)	return;
	rectSearhROI[cam_id] = fi_rectSearhROI;
}

/* desc: pat Mark ��� */
BOOL CMilMain::RegistPat(UINT8 cam_id, PUINT8 img_src, CRect fi_rectArea, CString fi_filename, UINT8 mark_no)
{
	if (!m_pMilModel)	return FALSE;
	return m_pMilModel[cam_id - 1]->RegistPat(img_src, fi_rectArea, fi_filename, mark_no);
}

/* desc: mmf Mark ��� */
BOOL CMilMain::RegistMod(UINT8 cam_id, PUINT8 img_src, CRect fi_rectArea, CString fi_filename, UINT8 mark_no)
{
	if (!m_pMilModel)	return FALSE;
	return m_pMilModel[cam_id - 1]->RegistMod(img_src, fi_rectArea, fi_filename, mark_no);
}

/* desc: Mil Main �Ҵ� ���� ���� */
VOID CMilMain::CloseSetMark()
{
#ifndef _NOT_USE_MIL_
	//if (m_mDisID_MMPM)
	//{
	//	MdispDeselect(m_mDisID_MarkSet, m_mImgDisp_MMPM);
	//	MdispFree(m_mDisID_MMPM);
	//	m_mDisID_MMPM = M_NULL;
	//}

	//if (m_mImgDisp_MMPM)
	//{
	//	MbufFree(m_mImgDisp_MMPM);
	//	m_mImgDisp_MMPM = M_NULL;
	//}

	if (m_mDisID_MarkSet)
	{
		MdispDeselect(m_mDisID_MarkSet, m_mImgDisp_MarkSet);
		MdispFree(m_mDisID_MarkSet);
		m_mDisID_MarkSet = M_NULL;
	}

	if (m_mImgDisp_MarkSet)
	{
		MbufFree(m_mImgDisp_MarkSet);
		m_mImgDisp_MarkSet = M_NULL;
	}

	if (m_mImg_MarkSet)
	{
		MbufFree(m_mImg_MarkSet);
		m_mImg_MarkSet = M_NULL;
	}

	// Mask close
	if (pucMaskBuf)
	{
		GlobalUnlock(pucMaskBuf);
		GlobalFree(hMaskBuf);
		hMaskBuf = NULL;
	}

	if (MilMask)
	{
		MbufFree(MilMask);
		MilMask = M_NULL;
	}
#endif
}

/* desc: Mil Main �Ҵ� ���� ���� */
VOID CMilMain::CloseMMPMAutoCenter()
{
#ifndef _NOT_USE_MIL_
	if (m_mDisID_MPMM_AutoCenter)
	{
		MdispDeselect(m_mDisID_MPMM_AutoCenter, m_mImgDisp_MPMM_AutoCenter);
		MdispFree(m_mDisID_MPMM_AutoCenter);
		m_mDisID_MPMM_AutoCenter = M_NULL;
	}

	if (m_mImgDisp_MPMM_AutoCenter)
	{
		MbufFree(m_mImgDisp_MPMM_AutoCenter);
		m_mImgDisp_MPMM_AutoCenter = M_NULL;
	}

	if (m_mImg_MPMM_AutoCenter)
	{
		MbufFree(m_mImg_MPMM_AutoCenter);
		m_mImg_MPMM_AutoCenter = M_NULL;
	}
#endif
}

/* desc: Mask �ʱ�ȭ */
VOID CMilMain::InitMask(UINT8 cam_id)
{
#ifndef _NOT_USE_MIL_

	if (m_pMilModel)	// lk91 ó�� ��ũ ����Ҷ� �ִ��� üũ
	{
		theApp.clMilMain.m_MarkSize = m_pMilModel[cam_id - 1]->GetMarkSize(TMP_MARK); // MASKING�� ��ũ ��Ͽ��� ����ϴϱ�.. CAM 0���� ����
		//MbufAlloc2d(m_mSysID, theApp.clMilMain.m_MarkSize.x, theApp.clMilMain.m_MarkSize.y, 8L + M_UNSIGNED, M_IMAGE + M_DISP + M_PROC, &MilMask);
	}

	if (!MilMask) {
		MbufAlloc2d(m_mSysID, theApp.clMilMain.m_MarkSize.x, theApp.clMilMain.m_MarkSize.y, 8L + M_UNSIGNED, M_IMAGE + M_DISP + M_PROC, &MilMask);
		MbufClear(MilMask, 0x00);
	}


	if(!hMaskBuf)
		hMaskBuf = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, theApp.clMilMain.m_MarkSize.x * theApp.clMilMain.m_MarkSize.y);

	if (hMaskBuf)
	{
		pucMaskBuf = (unsigned char*)GlobalLock(hMaskBuf);
		memset(pucMaskBuf, 0x00, (theApp.clMilMain.m_MarkSize.x * theApp.clMilMain.m_MarkSize.y));
	}
	else	AfxMessageBox(_T("Memory Allocation Error...[Mask Buffer Error]"));
#endif
}

/* desc: Mark Size, Offset �ʱ�ȭ */
VOID CMilMain::InitSetMarkSizeOffset(UINT8 cam_id, TCHAR* file, UINT8 fi_findType, UINT8 fi_No)
{
	if (!m_pMilModel)	return;
	return m_pMilModel[cam_id - 1]->InitSetMarkSizeOffset(cam_id, file, fi_findType, fi_No);
}

/* desc: Draw Mark Display */
VOID CMilMain::PutMarkDisp(HWND hwnd, int fi_iNo, RECT draw, UINT8 cam_id, TCHAR* file, int fi_findType)
{
	if (!m_pMilModel)	return;
	return m_pMilModel[cam_id - 1]->PutMarkDisp(hwnd, fi_iNo, draw, cam_id, file, fi_findType);
}

/* desc: Draw Set Mark Display */
BOOL CMilMain::PutSetMarkDisp(HWND hwnd, RECT draw, UINT8 cam_id, TCHAR* file, int fi_findType, DOUBLE fi_dRate)
{
	if (!m_pMilModel)	return FALSE;
	return m_pMilModel[cam_id - 1]->PutSetMarkDisp(hwnd, draw, cam_id, file, fi_findType, fi_dRate);
}

/* desc: Get Mark Setting Disp Rate */
DOUBLE CMilMain::GetMarkSetDispRate()
{
	return MARKSET_DISP_RATE;
}

/* desc: Set Mark Setting Disp Rate */
DOUBLE CMilMain::SetMarkSetDispRate(DOUBLE fi_dRate)
{
	if (fi_dRate == 1.0)
		MARKSET_DISP_RATE = 1.0;
	else {
		MARKSET_DISP_RATE += fi_dRate;
		if (MARKSET_DISP_RATE <= 0.0)
			MARKSET_DISP_RATE = 0.1; // �ּڰ�, 0.0 �̻� ����������... ������ 1.0 �̸��� mask, edge�� ������� �ʾ�
	}

	return MARKSET_DISP_RATE;
}

/* desc: Get Mark Size */
CPoint CMilMain::GetMarkSize(UINT8 cam_id, int fi_No)
{
	if (!m_pMilModel)	return CPoint(0,0);
	return m_pMilModel[cam_id - 1]->GetMarkSize(fi_No);
}

/* desc: Get Mark Offset */
CPoint CMilMain::GetMarkOffset(UINT8 cam_id, BOOL bNewOffset, int fi_No)
{
	if (!m_pMilModel)	return CPoint(0, 0);
	return m_pMilModel[cam_id - 1]->GetMarkOffset(bNewOffset, fi_No);
}

/* desc: Get Mask Buf */
BOOL CMilMain::MaskBufGet()
{
#ifndef _NOT_USE_MIL_
	if (MilMask) {
		MbufGet(MilMask, pucMaskBuf);
		return TRUE;
	}
	else {
		return FALSE;
	}
#else
	return TRUE;
#endif
}

/* desc: Get Mark Find Mode */
UINT8 CMilMain::GetMarkFindMode(UINT8 cam_id, UINT8 mark_no)
{
	if (!m_pMilModel)	return 0;
	return m_pMilModel[cam_id - 1]->GetMarkFindMode(mark_no);
}

/* desc: Set Mark Find Mode */
BOOL CMilMain::SetMarkFindMode(UINT8 cam_id, UINT8 find_mode, UINT8 mark_no)
{
	if (!m_pMilModel)	return FALSE;
	return m_pMilModel[cam_id - 1]->SetMarkFindMode(find_mode, mark_no);
}

/* desc: Set Mark Size */
VOID CMilMain::SetMarkSize(UINT8 cam_id, CPoint fi_MarkSize, int setMode, int fi_No)
{
	if (!m_pMilModel)	return;
	return m_pMilModel[cam_id - 1]->SetMarkSize(fi_MarkSize, setMode, fi_No);
}

/* desc: Set Mark Offset */
VOID CMilMain::SetMarkOffset(UINT8 cam_id, CPoint fi_MarkCenter, int setOffsetMode, int fi_No)
{
	if (!m_pMilModel)	return;
	return m_pMilModel[cam_id - 1]->SetMarkOffset(fi_MarkCenter, setOffsetMode, fi_No);
}

/* desc: MMF ���Ͽ� MASK ���� ����, Mark Set������ ��� */
VOID CMilMain::SaveMask_MOD(UINT8 cam_id, UINT8 mark_no)
{
	if (!m_pMilModel)	return;
	return m_pMilModel[cam_id - 1]->SaveMask_MOD(mark_no);
}

/* desc: PAT ���Ͽ� MASK ���� ����, Mark Set������ ��� */
VOID CMilMain::SaveMask_PAT(UINT8 cam_id, UINT8 mark_no)
{
	if (!m_pMilModel)	return;
	return m_pMilModel[cam_id - 1]->SaveMask_PAT(mark_no);
}

/* desc: PAT MARK ���� */
VOID CMilMain::PatMarkSave(UINT8 cam_id, CString fi_strFileName, UINT8 mark_no)
{
	if (!m_pMilModel)	return;
	return m_pMilModel[cam_id - 1]->PatMarkSave(fi_strFileName, mark_no);
}

/* desc: MMF MARK ���� */
VOID CMilMain::ModMarkSave(UINT8 cam_id, CString fi_strFileName, UINT8 mark_no)
{
	if (!m_pMilModel)	return;
	return m_pMilModel[cam_id - 1]->ModMarkSave(fi_strFileName, mark_no);
}

/* desc: Clear Mask(MMF) */
VOID CMilMain::MaskClear_MOD(UINT8 cam_id, CPoint fi_iSizeP, UINT8 mark_no)
{
	if (!m_pMilModel)	return;
	return m_pMilModel[cam_id - 1]->MaskClear_MOD(fi_iSizeP, mark_no);
}

/* desc: Clear Mask(PAT) */
VOID CMilMain::MaskClear_PAT(UINT8 cam_id, CPoint fi_iSizeP, UINT8 mark_no)
{
	if (!m_pMilModel)	return;
	return m_pMilModel[cam_id - 1]->MaskClear_PAT(fi_iSizeP, mark_no);
}

/* desc: Find Center (Mark Set������ ���) */
VOID CMilMain::CenterFind(int cam_id, int fi_length, int fi_curSmoothness, double* fi_NumEdgeMIN_X, double* fi_NumEdgeMAX_X, double* fi_NumEdgeMIN_Y, double* fi_NumEdgeMAX_Y, int* fi_NumEdgeFound, int fi_Mode)
{
	if (!m_pMilModel)	return;
	return m_pMilModel[cam_id - 1]->CenterFind(fi_length, fi_curSmoothness, fi_NumEdgeMIN_X, fi_NumEdgeMAX_X, fi_NumEdgeMIN_Y, fi_NumEdgeMAX_Y, fi_NumEdgeFound, fi_Mode);
}

/* desc: Mark ���� �׸� �� ����ϴ� MIL �Լ� */
VOID CMilMain::DrawMarkInfo_UseMIL(UINT8 cam_id, UINT8 fi_smooth, UINT8 mark_no)
{
	if (!m_pMilModel)	return;
	return m_pMilModel[cam_id - 1]->DrawMarkInfo_UseMIL(fi_smooth, mark_no);
}

/* desc: Set Mask */
VOID CMilMain::Mask_MarkSet(UINT8 cam_id, CRect rectTmp, CPoint iTmpSizeP, CRect rectFill, int fi_color, bool bMask)
{
	if (!m_pMilModel)	return;
	return m_pMilModel[cam_id - 1]->Mask_MarkSet(rectTmp, iTmpSizeP, rectFill, fi_color, bMask);
}

/* desc: EXPO DISP ID �Ҵ� */
VOID CMilMain::SetDispExpo(CWnd* pWnd[4])
{
	//WINDOWPLACEMENT wndpl;
	//CPoint	iSizeP;
	long lBufAttBuf = M_IMAGE + M_DISP + M_PROC;
	CString sTmp;

	for (int i = 0; i < 4; i++) {
		//pWnd[i]->GetWindowPlacement(&wndpl);

		//iSizeP.x = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
		//iSizeP.y = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;

#ifndef _NOT_USE_MIL_
		if(!m_mDisID_EXPO[i])
			m_mDisID_EXPO[i] = MdispAlloc(m_mSysID, M_DEFAULT, _T("M_DEFAULT"), M_DEFAULT, M_NULL);

		if (m_mDisID_EXPO[i])
		{
			MIL_INT DisplayType = MdispInquire(m_mDisID_EXPO[i], M_DISPLAY_TYPE, M_NULL);

			if ((DisplayType & (M_WINDOWED | M_EXCLUSIVE)) != M_WINDOWED)
			{
				AfxMessageBox(_T("Failed to create m_mDisID_EXPO."));
				MdispFree(m_mDisID_EXPO[i]);
				m_mDisID_EXPO[i] = (MIL_ID)NULL;

				//MdispControl(m_mDisID_EXPO[i], M_OVERLAY_SHOW, M_ENABLE);
				//MdispControl(m_mDisID_EXPO[i], M_OVERLAY, M_ENABLE);
				//MgraControl(M_DEFAULT, M_BACKGROUND_MODE, M_TRANSPARENT);
			}

			//MdispSelectWindow(m_mDisID_EXPO[i], m_mImgDisp_EXPO[i], pWnd[i]->m_hWnd);

			//MdispControl(m_mDisID_EXPO[i], M_OVERLAY_SHOW, M_ENABLE);
			//MdispControl(m_mDisID_EXPO[i], M_OVERLAY, M_ENABLE);
			//MdispInquire(m_mDisID_EXPO[i], M_OVERLAY_ID, &m_mOverlay_EXPO[i]);
			//MdispInquire(m_mDisID_EXPO[i], M_TRANSPARENT_COLOR, &m_mTransColor_EXPO[i]); 

			//MgraControl(M_DEFAULT, M_BACKGROUND_MODE, M_TRANSPARENT);
		}
#endif
	}
	//EXPO_DISP_SIZE_X = iSizeP.x;
	//EXPO_DISP_SIZE_Y = iSizeP.y;
}

/* desc: LIVE DISP ID �Ҵ� */
VOID CMilMain::SetDisp(CWnd** pWnd, UINT8 fi_Mode)
{
	WINDOWPLACEMENT wndpl;
	CPoint	iSizeP;
	long lBufAttBuf = M_IMAGE + M_DISP + M_PROC;
	CString sTmp;

#ifndef _NOT_USE_MIL_

	if (fi_Mode == 0x00) { // Mark Live
		pWnd[0]->GetWindowPlacement(&wndpl);

		iSizeP.x = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
		iSizeP.y = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;
		if (!m_mImgDisp_Mark_Live)
			MbufAllocColor(m_mSysID, 1L, iSizeP.x, iSizeP.y, 8L + M_UNSIGNED, lBufAttBuf, &m_mImgDisp_Mark_Live);
		if (m_mImgDisp_Mark_Live)	MbufClear(m_mImgDisp_Mark_Live, 0L);
		else
		{
			sTmp.Format(_T("Failed to create m_mImgDisp_Mark_Live Buffer"));
			AfxMessageBox(sTmp);
			return;
		}

		if (m_mImgDisp_Mark_Live)
		{
			if (!m_mDisID_Mark_Live)
				m_mDisID_Mark_Live = MdispAlloc(m_mSysID, M_DEFAULT, _T("M_DEFAULT"), M_DEFAULT, M_NULL);
			if (m_mDisID_Mark_Live)
			{
				MIL_INT DisplayType = MdispInquire(m_mDisID_Mark_Live, M_DISPLAY_TYPE, M_NULL);

				if ((DisplayType & (M_WINDOWED | M_EXCLUSIVE)) != M_WINDOWED)
				{
					//str.Format(_T("Failed to create MilDisplay."));
					AfxMessageBox(_T("Failed to create m_mDisID_Mark_Live."));
					MdispFree(m_mDisID_Mark_Live);
					m_mDisID_Mark_Live = (MIL_ID)NULL;
				}
			}

			if (m_mDisID_Mark_Live)
			{
				MdispSelectWindow(m_mDisID_Mark_Live, m_mImgDisp_Mark_Live, pWnd[0]->m_hWnd);

				MdispControl(m_mDisID_Mark_Live, M_OVERLAY_SHOW, M_ENABLE);
				MdispControl(m_mDisID_Mark_Live, M_OVERLAY, M_ENABLE);
				//for (int i = 0; i < CAM_CNT; i++) {
					MdispInquire(m_mDisID_Mark_Live, M_OVERLAY_ID, &m_mOverlay_Mark_Live);
					//MdispInquire(m_mDisID_Mark_Live, M_TRANSPARENT_COLOR, &m_mTransColor_Mark_Live);
				//}
				MgraControl(M_DEFAULT, M_BACKGROUND_MODE, M_TRANSPARENT);
			}
		}

		LIVE_DISP_SIZE_X = iSizeP.x;
		LIVE_DISP_SIZE_Y = iSizeP.y;
	}
	else if (fi_Mode == 0x01) { // Calib Camera Spec
		pWnd[0]->GetWindowPlacement(&wndpl);

		iSizeP.x = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
		iSizeP.y = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;
		if (!m_mImgDisp_CALI_CAMSPEC)
			MbufAllocColor(m_mSysID, 1L, iSizeP.x, iSizeP.y, 8L + M_UNSIGNED, lBufAttBuf, &m_mImgDisp_CALI_CAMSPEC);
		if (m_mImgDisp_CALI_CAMSPEC)	MbufClear(m_mImgDisp_CALI_CAMSPEC, 0L);
		else
		{
			sTmp.Format(_T("Failed to create m_mImgDisp_CALI_CAMSPEC Buffer"));
			AfxMessageBox(sTmp);
			return;
		}

		if (m_mImgDisp_CALI_CAMSPEC)
		{
			if (!m_mDisID_CALI_CAMSPEC)
				m_mDisID_CALI_CAMSPEC = MdispAlloc(m_mSysID, M_DEFAULT, _T("M_DEFAULT"), M_DEFAULT, M_NULL);
			if (m_mDisID_CALI_CAMSPEC)
			{
				MIL_INT DisplayType = MdispInquire(m_mDisID_CALI_CAMSPEC, M_DISPLAY_TYPE, M_NULL);

				if ((DisplayType & (M_WINDOWED | M_EXCLUSIVE)) != M_WINDOWED)
				{
					//str.Format(_T("Failed to create MilDisplay."));
					AfxMessageBox(_T("Failed to create m_mDisID_CALI_CAMSPEC."));
					MdispFree(m_mDisID_CALI_CAMSPEC);
					m_mDisID_CALI_CAMSPEC = (MIL_ID)NULL;
				}
			}

			if (m_mDisID_CALI_CAMSPEC)
			{
				MdispSelectWindow(m_mDisID_CALI_CAMSPEC, m_mImgDisp_CALI_CAMSPEC, pWnd[0]->m_hWnd);

				MdispControl(m_mDisID_CALI_CAMSPEC, M_OVERLAY_SHOW, M_ENABLE);
				MdispControl(m_mDisID_CALI_CAMSPEC, M_OVERLAY, M_ENABLE);
				//for (int i = 0; i < CAM_CNT; i++) {
					MdispInquire(m_mDisID_CALI_CAMSPEC, M_OVERLAY_ID, &m_mOverlay_CALI_CAMSPEC);
					//MdispInquire(m_mDisID_CALI_CAMSPEC, M_TRANSPARENT_COLOR, &m_mTransColor_CALI_CAMSPEC);
				//}
				MgraControl(M_DEFAULT, M_BACKGROUND_MODE, M_TRANSPARENT);
			}
		}

		CALIB_CAMSPEC_DISP_SIZE_X = iSizeP.x;
		CALIB_CAMSPEC_DISP_SIZE_Y = iSizeP.y;
	}
	else if (fi_Mode == 0x02) { // Calib Exposure(cali step)
		//for (int i = 0; i < g_pstConfig->set_cams.acam_count; i++) {
		for (int i = 0; i < 2; i++) {
			pWnd[i]->GetWindowPlacement(&wndpl);

			iSizeP.x = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
			iSizeP.y = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;
			if (!m_mImgDisp_CALI_STEP[i])
				MbufAllocColor(m_mSysID, 1L, iSizeP.x, iSizeP.y, 8L + M_UNSIGNED, lBufAttBuf, &m_mImgDisp_CALI_STEP[i]);
			if (m_mImgDisp_CALI_STEP[i])	MbufClear(m_mImgDisp_CALI_STEP[i], 0L);
			else
			{
				sTmp.Format(_T("Failed to create m_mImgDisp_CALI_STEP Buffer"));
				AfxMessageBox(sTmp);
				return;
			}

			if (m_mImgDisp_CALI_STEP[i])
			{
				if (!m_mDisID_CALI_STEP[i])
					m_mDisID_CALI_STEP[i] = MdispAlloc(m_mSysID, M_DEFAULT, _T("M_DEFAULT"), M_DEFAULT, M_NULL);
				if (m_mDisID_CALI_STEP[i])
				{
					MIL_INT DisplayType = MdispInquire(m_mDisID_CALI_STEP[i], M_DISPLAY_TYPE, M_NULL);

					if ((DisplayType & (M_WINDOWED | M_EXCLUSIVE)) != M_WINDOWED)
					{
						//str.Format(_T("Failed to create MilDisplay."));
						AfxMessageBox(_T("Failed to create m_mDisID_CALI_STEP."));
						MdispFree(m_mDisID_CALI_STEP[i]);
						m_mDisID_CALI_STEP[i] = (MIL_ID)NULL;
					}
				}

				if (m_mDisID_CALI_STEP[i])
				{
					MdispSelectWindow(m_mDisID_CALI_STEP[i], m_mImgDisp_CALI_STEP[i], pWnd[i]->m_hWnd);

					MdispControl(m_mDisID_CALI_STEP[i], M_OVERLAY_SHOW, M_ENABLE);
					MdispControl(m_mDisID_CALI_STEP[i], M_OVERLAY, M_ENABLE);
					//for (int i = 0; i < CAM_CNT; i++) {
						MdispInquire(m_mDisID_CALI_STEP[i], M_OVERLAY_ID, &m_mOverlay_CALI_STEP[i]);
						//MdispInquire(m_mDisID_CALI_STEP[i], M_TRANSPARENT_COLOR, &m_mTransColor_CALI_STEP[i]);
					//}
					MgraControl(M_DEFAULT, M_BACKGROUND_MODE, M_TRANSPARENT);
				}
			}

			CALIB_EXPO_DISP_SIZE_X = iSizeP.x;
			CALIB_EXPO_DISP_SIZE_Y = iSizeP.y;
		}
	}
	else if (fi_Mode == 0x03) { // AccuracyMeasure
		pWnd[0]->GetWindowPlacement(&wndpl);

		iSizeP.x = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
		iSizeP.y = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;
		if (!m_mImgDisp_ACCR)
			MbufAllocColor(m_mSysID, 1L, iSizeP.x, iSizeP.y, 8L + M_UNSIGNED, lBufAttBuf, &m_mImgDisp_ACCR);
		if (m_mImgDisp_ACCR)	MbufClear(m_mImgDisp_ACCR, 0L);
		else
		{
			sTmp.Format(_T("Failed to create m_mImgDisp_ACCR Buffer"));
			AfxMessageBox(sTmp);
			return;
		}

		if (m_mImgDisp_ACCR)
		{
			if (!m_mDisID_ACCR)
				m_mDisID_ACCR = MdispAlloc(m_mSysID, M_DEFAULT, _T("M_DEFAULT"), M_DEFAULT, M_NULL);
			if (m_mDisID_ACCR)
			{
				MIL_INT DisplayType = MdispInquire(m_mDisID_ACCR, M_DISPLAY_TYPE, M_NULL);

				if ((DisplayType & (M_WINDOWED | M_EXCLUSIVE)) != M_WINDOWED)
				{
					//str.Format(_T("Failed to create MilDisplay."));
					AfxMessageBox(_T("Failed to create m_mDisID_ACCR."));
					MdispFree(m_mDisID_ACCR);
					m_mDisID_ACCR = (MIL_ID)NULL;
				}
			}

			if (m_mDisID_ACCR)
			{
				MdispSelectWindow(m_mDisID_ACCR, m_mImgDisp_ACCR, pWnd[0]->m_hWnd);

				MdispControl(m_mDisID_ACCR, M_OVERLAY_SHOW, M_ENABLE);
				MdispControl(m_mDisID_ACCR, M_OVERLAY, M_ENABLE);
				//for (int i = 0; i < CAM_CNT; i++) {
				MdispInquire(m_mDisID_ACCR, M_OVERLAY_ID, &m_mOverlay_ACCR);
				//MdispInquire(m_mDisID_CALI_STEP[i], M_TRANSPARENT_COLOR, &m_mTransColor_CALI_STEP[i]);
			//}
				MgraControl(M_DEFAULT, M_BACKGROUND_MODE, M_TRANSPARENT);
			}
		}

		ACCR_DISP_SIZE_X = iSizeP.x;
		ACCR_DISP_SIZE_Y = iSizeP.y;
	}
	else {
		sTmp.Format(_T("SeDisp Error"));
		AfxMessageBox(sTmp);
		return;
	}

#endif

}

/* desc: MARK DISP ID �Ҵ� */
VOID CMilMain::SetDispMark(CWnd* pWnd) // MIL_ID 2�� ���
{
	WINDOWPLACEMENT wndpl;
	CPoint	iSizeP;
	long lBufAttBuf = M_IMAGE + M_DISP + M_PROC;
	CString sTmp;
	int i = 2;

	pWnd->GetWindowPlacement(&wndpl);

	iSizeP.x = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
	iSizeP.y = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;

#ifndef _NOT_USE_MIL_
	if(!m_mImgDisp_Mark[i])
		MbufAllocColor(m_mSysID, 1L, iSizeP.x, iSizeP.y, 8L + M_UNSIGNED, lBufAttBuf, &m_mImgDisp_Mark[i]);
	if (m_mImgDisp_Mark[i])	MbufClear(m_mImgDisp_Mark[i], 192);
	else
	{
		sTmp.Format(_T("Failed to create m_mImgDisp_Mark[%d] Buffer"), i);
		AfxMessageBox(sTmp);
		return;
	}

	if (m_mImgDisp_Mark[i])
	{
		if(!m_mDisID_Mark[i])
			m_mDisID_Mark[i] = MdispAlloc(m_mSysID, M_DEFAULT, _T("M_DEFAULT"), M_DEFAULT, M_NULL);

		if (m_mDisID_Mark[i])
		{
			MIL_INT DisplayType = MdispInquire(m_mDisID_Mark[i], M_DISPLAY_TYPE, M_NULL);

			if ((DisplayType & (M_WINDOWED | M_EXCLUSIVE)) != M_WINDOWED)
			{
				AfxMessageBox(_T("Failed to create m_mDisID_Mark."));
				MdispFree(m_mDisID_Mark[i]);
				m_mDisID_Mark[i] = (MIL_ID)NULL;

				MdispControl(m_mDisID_Mark[i], M_OVERLAY_SHOW, M_ENABLE);
				MdispControl(m_mDisID_Mark[i], M_OVERLAY, M_ENABLE);
				MgraControl(M_DEFAULT, M_BACKGROUND_MODE, M_TRANSPARENT);
			}

			MdispSelectWindow(m_mDisID_Mark[i], m_mImgDisp_Mark[i], pWnd->m_hWnd);

			MdispControl(m_mDisID_Mark[i], M_OVERLAY_SHOW, M_ENABLE);
			MdispControl(m_mDisID_Mark[i], M_OVERLAY, M_ENABLE);
			MdispInquire(m_mDisID_Mark[i], M_OVERLAY_ID, &m_mOverlay_Mark[i]);
			//MdispInquire(m_mDisID_Mark[i], M_TRANSPARENT_COLOR, &m_mTransColor_Mark[i]); 

			MgraControl(M_DEFAULT, M_BACKGROUND_MODE, M_TRANSPARENT);
		}
	}
#endif

	MARK_DISP_SIZE_X = iSizeP.x;
	MARK_DISP_SIZE_Y = iSizeP.y;
}

/* desc: MARK DISP ID �Ҵ� */
VOID CMilMain::SetDispRecipeMark(CWnd* pWnd[2]) // MIL_ID �̹� ����� ������ �������... 0, 1�� ����..(2���� Mark �ǿ��� ����ϱ�)
{
	WINDOWPLACEMENT wndpl;
	CPoint	iSizeP;
	long lBufAttBuf = M_IMAGE + M_DISP + M_PROC;
	CString sTmp;

	//for (int i = 0; i < g_pstConfig->set_cams.acam_count; i++) { // lk91 UI ����Ǹ� CAM 3�� ���� ����
	for (int i = 0; i < 2; i++) {
		pWnd[i]->GetWindowPlacement(&wndpl);

		iSizeP.x = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
		iSizeP.y = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;

#ifndef _NOT_USE_MIL_
		if(!m_mImgDisp_Mark[i])
			MbufAllocColor(m_mSysID, 1L, iSizeP.x, iSizeP.y, 8L + M_UNSIGNED, lBufAttBuf, &m_mImgDisp_Mark[i]);
		if (m_mImgDisp_Mark[i])	MbufClear(m_mImgDisp_Mark[i], 192);
		else
		{
			sTmp.Format(_T("Failed to create m_mImgDisp_Mark[%d] Buffer"), i);
			AfxMessageBox(sTmp);
			return;
		}

		if (m_mImgDisp_Mark[i])
		{
			if(!m_mDisID_Mark[i])
				m_mDisID_Mark[i] = MdispAlloc(m_mSysID, M_DEFAULT, _T("M_DEFAULT"), M_DEFAULT, M_NULL);

			if (m_mDisID_Mark[i])
			{
				MIL_INT DisplayType = MdispInquire(m_mDisID_Mark[i], M_DISPLAY_TYPE, M_NULL);

				if ((DisplayType & (M_WINDOWED | M_EXCLUSIVE)) != M_WINDOWED)
				{
					AfxMessageBox(_T("Failed to create m_mDisID_Mark."));
					MdispFree(m_mDisID_Mark[i]);
					m_mDisID_Mark[i] = (MIL_ID)NULL;

					MdispControl(m_mDisID_Mark[i], M_OVERLAY_SHOW, M_ENABLE);
					MdispControl(m_mDisID_Mark[i], M_OVERLAY, M_ENABLE);
					MgraControl(M_DEFAULT, M_BACKGROUND_MODE, M_TRANSPARENT);
				}

				MdispSelectWindow(m_mDisID_Mark[i], m_mImgDisp_Mark[i], pWnd[i]->m_hWnd);

				MdispControl(m_mDisID_Mark[i], M_OVERLAY_SHOW, M_ENABLE);
				MdispControl(m_mDisID_Mark[i], M_OVERLAY, M_ENABLE);
				MdispInquire(m_mDisID_Mark[i], M_OVERLAY_ID, &m_mOverlay_Mark[i]);
				//MdispInquire(m_mDisID_Mark[i], M_TRANSPARENT_COLOR, &m_mTransColor_Mark[i]);

				MgraControl(M_DEFAULT, M_BACKGROUND_MODE, M_TRANSPARENT);
			}
		}
#endif
	}

	RECIPE_MARK_DISP_SIZE_X = iSizeP.x;
	RECIPE_MARK_DISP_SIZE_Y = iSizeP.y;
}

/* desc: MARK SET DISP ID �Ҵ� */
VOID CMilMain::SetDispMarkSet(CWnd* pWnd)
{
	//WINDOWPLACEMENT wndpl;
	CPoint	iSizeP;
	long lBufAttBuf = M_IMAGE + M_DISP + M_PROC;
	CString sTmp;

#ifndef _NOT_USE_MIL_
	if(!m_mDisID_MarkSet)
		m_mDisID_MarkSet = MdispAlloc(m_mSysID, M_DEFAULT, _T("M_DEFAULT"), M_DEFAULT, M_NULL);

	if (m_mDisID_MarkSet)
	{
		MIL_INT DisplayType = MdispInquire(m_mDisID_MarkSet, M_DISPLAY_TYPE, M_NULL);

		if ((DisplayType & (M_WINDOWED | M_EXCLUSIVE)) != M_WINDOWED)
		{
			AfxMessageBox(_T("Failed to create m_mlDisID_MarkSet."));
			MdispFree(m_mDisID_MarkSet);
			m_mDisID_MarkSet = (MIL_ID)NULL;
		}

	}
#endif
}

/* desc: MMPM AutoCenter DISP ID �Ҵ� */
VOID CMilMain::SetDispMMPM_AutoCenter(CWnd* pWnd)
{
	//WINDOWPLACEMENT wndpl;
	CPoint	iSizeP;
	long lBufAttBuf = M_IMAGE + M_DISP + M_PROC;
	CString sTmp;

#ifndef _NOT_USE_MIL_
	if (!m_mDisID_MPMM_AutoCenter)
		m_mDisID_MPMM_AutoCenter = MdispAlloc(m_mSysID, M_DEFAULT, _T("M_DEFAULT"), M_DEFAULT, M_NULL);

	if (m_mDisID_MPMM_AutoCenter)
	{
		MIL_INT DisplayType = MdispInquire(m_mDisID_MPMM_AutoCenter, M_DISPLAY_TYPE, M_NULL);

		if ((DisplayType & (M_WINDOWED | M_EXCLUSIVE)) != M_WINDOWED)
		{
			AfxMessageBox(_T("Failed to create m_mDisID_MPMM_AutoCenter."));
			MdispFree(m_mDisID_MPMM_AutoCenter);
			m_mDisID_MPMM_AutoCenter = (MIL_ID)NULL;
		}
	}
#endif
}

/* desc: MMPM DISP ID �Ҵ� */
VOID CMilMain::SetDispMMPM(CWnd* pWnd)
{
	WINDOWPLACEMENT wndpl;
	CPoint	iSizeP;
	long lBufAttBuf = M_IMAGE + M_DISP + M_PROC;
	CString sTmp;

	pWnd->GetWindowPlacement(&wndpl);

	iSizeP.x = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
	iSizeP.y = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;

#ifndef _NOT_USE_MIL_
	if(!m_mImgDisp_MMPM)
		MbufAllocColor(m_mSysID, 1L, iSizeP.x, iSizeP.y, 8L + M_UNSIGNED, lBufAttBuf, &m_mImgDisp_MMPM);
	if (m_mImgDisp_MMPM)	MbufClear(m_mImgDisp_MMPM, 192);
	else
	{
		sTmp.Format(_T("Failed to create m_mImgDisp_MMPM Buffer"));
		AfxMessageBox(sTmp);
		return;
	}

	if (m_mImgDisp_MMPM)
	{
		if(!m_mDisID_MMPM)
			m_mDisID_MMPM = MdispAlloc(m_mSysID, M_DEFAULT, _T("M_DEFAULT"), M_DEFAULT, M_NULL);
		if (m_mDisID_MMPM)
		{
			MIL_INT DisplayType = MdispInquire(m_mDisID_MMPM, M_DISPLAY_TYPE, M_NULL);

			if ((DisplayType & (M_WINDOWED | M_EXCLUSIVE)) != M_WINDOWED)
			{
				//str.Format(_T("Failed to create MilDisplay."));
				AfxMessageBox(_T("Failed to create m_mDisID_MMPM."));
				MdispFree(m_mDisID_MMPM);
				m_mDisID_MMPM = (MIL_ID)NULL;
			}
		}

		if (m_mDisID_MMPM)
		{
			MdispSelectWindow(m_mDisID_MMPM, m_mImgDisp_MMPM, pWnd->m_hWnd);

			MdispControl(m_mDisID_MMPM, M_OVERLAY_SHOW, M_ENABLE);
			MdispControl(m_mDisID_MMPM, M_OVERLAY, M_ENABLE);
			for (int i = 0; i < CAM_CNT; i++) {
				MdispInquire(m_mDisID_MMPM, M_OVERLAY_ID, &m_mOverlay_MMPM);
			}
			MgraControl(M_DEFAULT, M_BACKGROUND_MODE, M_TRANSPARENT);
		}
	}
#endif

	MMPM_DISP_SIZE_X = iSizeP.x;
	MMPM_DISP_SIZE_Y = iSizeP.y;

}

/* desc: MIL Img ID ��� */
BOOL CMilMain::RegistMILImg(INT32 cam_id, INT32 width, INT32 height, PUINT8 image)
{
	BOOL bSucc = TRUE;
	//#ifndef _NOT_USE_MIL_
	UINT32 i32Size = 0;
#ifndef _NOT_USE_MIL_
	//MIL_ID mBufID = M_NULL;
	MIL_ID MilImgTmp = M_NULL;
	if (m_mImg[cam_id]) {
		/* Image Buffer Clear */
		MbufClear(m_mImg[cam_id], 0L);
	}
	else {
		return FALSE;
	}
	/* Put the image to image buffer */
	MbufPut2d(m_mImg[cam_id], 0, 0, width, height, image);

	if (MappGetError(M_GLOBAL, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"Failed to put the aoi area to image buffer");
		bSucc = FALSE;
	}

	// lk91 initMilAlloc �̵�
	//if (m_mImg_CALI_CAMSPEC[cam_id]) {
	//	/* Image Buffer Clear */
	//	MbufClear(m_mImg_CALI_CAMSPEC[cam_id], 0L);
	//}
	//else {
	//	return FALSE;
	//}
	///* Put the image to image buffer */
	////MbufPut2d(m_mImg_CALI_CAMSPEC[cam_id], rectSearhROI_Calb_CamSpec.left, rectSearhROI_Calb_CamSpec.top, 
	////	m_pstConfig->set_cams.spc_size[0], m_pstConfig->set_cams.spc_size[1], image);
	//if(m_mImgProc[cam_id])
	//	MbufClear(m_mImgProc[cam_id], 0L);
	//MbufCopy(m_mImg[cam_id], m_mImgProc[cam_id]);
	////MbufChild2d(m_mImgProc[cam_id], rectSearhROI_Calb_CamSpec.left, rectSearhROI_Calb_CamSpec.top,
	////	m_pstConfig->set_cams.spc_size[0], m_pstConfig->set_cams.spc_size[1], &m_mImg_CALI_CAMSPEC[cam_id]);
	//if (MappGetError(M_GLOBAL, M_NULL))
	//{
	//	LOG_ERROR(ENG_EDIC::en_mil, L"Failed to put the aoi area to m_mImg_CALI_CAMSPEC buffer");
	//	bSucc = FALSE;
	//}

	//if (m_mImg_CALI_STEP[0]) {
	//	/* Image Buffer Clear */
	//	MbufClear(m_mImg_CALI_STEP[0], 0L);
	//}
	//else {
	//	return FALSE;
	//}
	//if (m_mImg_CALI_STEP[1]) {
	//	/* Image Buffer Clear */
	//	MbufClear(m_mImg_CALI_STEP[1], 0L);
	//}
	//else {
	//	return FALSE;
	//}
	///* Put the image to image buffer */
	//MbufPut2d(m_mImg_CALI_STEP[cam_id], rectSearhROI_Calb_Step.left, rectSearhROI_Calb_Step.top,
	//	m_pstConfig->set_cams.soi_size[0], m_pstConfig->set_cams.soi_size[1], image);
	//if (m_mImgProc)
	//	MbufClear(m_mImgProc[cam_id], 0L);
	//MbufCopy(m_mImg[cam_id], m_mImgProc[cam_id]);
	////MbufChild2d(m_mImgProc[cam_id], rectSearhROI_Calb_Step.left, rectSearhROI_Calb_Step.top,
	////	m_pstConfig->set_cams.soi_size[0], m_pstConfig->set_cams.soi_size[1], &m_mImg_CALI_STEP[0]);

	//if (m_mImgProc[cam_id])
	//	MbufClear(m_mImgProc[cam_id], 0L);
	//MbufCopy(m_mImg[cam_id], m_mImgProc[cam_id]);
	////MbufChild2d(m_mImgProc[cam_id], rectSearhROI_Calb_Step.left, rectSearhROI_Calb_Step.top,
	////	m_pstConfig->set_cams.soi_size[0], m_pstConfig->set_cams.soi_size[1], &m_mImg_CALI_STEP[1]);
	//if (MappGetError(M_GLOBAL, M_NULL))
	//{
	//	LOG_ERROR(ENG_EDIC::en_mil, L"Failed to put the aoi area to m_mImg_CALI_STEP buffer");
	//	bSucc = FALSE;
	//}

	//if (m_mImg_ACCR[cam_id]) {
	//	/* Image Buffer Clear */
	//	MbufClear(m_mImg_ACCR[cam_id], 0L);
	//}
	//else {
	//	return FALSE;
	//}
	//if (m_mImgProc[cam_id])
	//	MbufClear(m_mImgProc[cam_id], 0L);
	//MbufCopy(m_mImg[cam_id], m_mImgProc[cam_id]);
	////MbufChild2d(m_mImgProc[cam_id], rectSearhROI_Calb_Accr.left, rectSearhROI_Calb_Accr.top,
	////	m_pstConfig->set_cams.spc_size[0], m_pstConfig->set_cams.spc_size[1], &m_mImg_ACCR[cam_id]);
	//if (MappGetError(M_GLOBAL, M_NULL))
	//{
	//	LOG_ERROR(ENG_EDIC::en_mil, L"Failed to put the aoi area to m_mImg_CALI_ACCR buffer");
	//	bSucc = FALSE;
	//}


	if (m_mImgProc[cam_id])
		MbufClear(m_mImgProc[cam_id], 0L);
	MbufCopy(m_mImg[cam_id], m_mImgProc[cam_id]);
	//if(cam_id == 0)
	//	MbufSave(_T("D:\\klk\\cam1.bmp"), m_mImgProc[0]); //lk91 tmp
	//else if(cam_id == 1)
	//	MbufSave(_T("D:\\klk\\cam2.bmp"), m_mImgProc[1]); //lk91 tmp
	//else if (cam_id == 2)
	//	MbufSave(_T("D:\\klk\\cam3.bmp"), m_mImgProc[2]); //lk91 tmp
#endif
	return TRUE;
}

/* desc: MIL Img ID ��� */
BOOL CMilMain::RegistMILGrabImg(INT32 fi_No, INT32 width, INT32 height, PUINT8 image)
{
	BOOL bSucc = TRUE;
	//#ifndef _NOT_USE_MIL_
	UINT32 i32Size = 0;
	MIL_ID mBufID = M_NULL;

#ifndef _NOT_USE_MIL_
	if (m_mImg_Grab[fi_No]) {
		/* Image Buffer Clear */
		MbufClear(m_mImg_Grab[fi_No], 0L);
	}
	else {
		return FALSE;
	}
	/* Put the image to image buffer */
	MbufPut2d(m_mImg_Grab[fi_No], 0, 0, width, height, image);
	if (MappGetError(M_GLOBAL, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"Failed to put the aoi area to grab image buffer");
		bSucc = FALSE;
	}
#endif
	return TRUE;
}

BOOL CMilMain::SetModelDefine_tot(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
	LPG_CMPV model, UINT8 fi_No, TCHAR* file,
	DOUBLE scale_min, DOUBLE scale_max,
	DOUBLE score_min, DOUBLE score_tgt)
{
	if (!m_pMilModel)			return FALSE;
	if (!m_pMilModel[cam_id - 1])	return FALSE;
	/* Model ��� */
	if (!m_pMilModel[cam_id - 1]->SetModelDefine_tot(speed, level, count, smooth, model, fi_No, file,
		scale_min, scale_max, score_min, score_tgt))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CMilMain::MergeMark(UINT8 cam_id, LPG_CMPV value, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, UINT8 mark_no, TCHAR* file1, TCHAR* file2,
	TCHAR* RecipeName, DOUBLE scale_min, DOUBLE scale_max, DOUBLE score_min, DOUBLE score_tgt)
{
	if (!m_pMilModel)			return FALSE;
	if (!m_pMilModel[cam_id - 1])	return FALSE;
	/* Model ��� */
	if (!m_pMilModel[cam_id - 1]->MergeMark(value, speed, level, count, smooth, mark_no, file1, file2,
		RecipeName, scale_min, scale_max, score_min, score_tgt))
	{
		return FALSE;
	}
	return TRUE;
}


#ifndef _NOT_USE_MIL_
MIL_ID	CMilMain::GetDispID(UINT8 dispType)
{	
	if(dispType == DISP_TYPE_MARK_LIVE)
		return m_mDisID_Mark_Live;	
	else if(dispType == DISP_TYPE_EXPO)
		return m_mlDisID;
		//return m_mDisID_EXPO;
}
#endif

BOOL CMilMain::ProcImage(UINT8 cam_id, UINT8 imgProc)
{
#ifndef _NOT_USE_MIL_
	if (m_mImgProc[cam_id]) {
		if (imgProc == 0) {
			MbufCopy(m_mImg[cam_id], m_mImgProc[cam_id]);
		}
		else {
			// lk91 //////////////////////////////////////////////
			// case 1
			//MbufSave(_T("D:\\test\\0.bmp"), m_mImg[cam_id]);
			MIL_ID MilHistogramEqualizeAdaptiveContext, ImageSizeBand, ImageWidth, ImageHeight, ImageType, MilDisplayImage, MilDispChildImage1;
			/* Allocate display buffer. */
			MbufInquire(m_mImgProc[cam_id], M_SIZE_BAND, &ImageSizeBand);
			MbufInquire(m_mImgProc[cam_id], M_SIZE_X, &ImageWidth);
			MbufInquire(m_mImgProc[cam_id], M_SIZE_Y, &ImageHeight);
			MbufInquire(m_mImgProc[cam_id], M_TYPE, &ImageType);
			//MbufAllocColor(theApp.clMilMain.m_mSysID,
			//	ImageSizeBand,
			//	ImageWidth * 2,
			//	ImageHeight,
			//	ImageType,
			//	M_IMAGE + M_PROC + M_DISP,
			//	&MilDisplayImage);
			//MbufClear(MilDisplayImage, 0);
			//MbufChild2d(MilDisplayImage, ImageWidth, 0, ImageWidth, ImageHeight, &m_mImgProc[cam_id]);
			MimAlloc(theApp.clMilMain.m_mSysID, M_HISTOGRAM_EQUALIZE_ADAPTIVE_CONTEXT, M_DEFAULT, &MilHistogramEqualizeAdaptiveContext);
			MimHistogramEqualizeAdaptive(MilHistogramEqualizeAdaptiveContext, m_mImg[cam_id], m_mImgProc[cam_id], M_DEFAULT);

			//MIL_ID m_mImgSmooth;
			////MbufControl(m_mImgSmooth
			//MimConvolve(m_mImgProc, m_mImgProc, M_SMOOTH);
			//MbufSave(_T("D:\\test\\Smooth.bmp"), m_mImgProc);

			//MimDilate(m_mImgProc, m_mImgProc, 1, M_GRAYSCALE);
			//MbufSave(_T("D:\\test\\Dilate.bmp"), m_mImgProc);
			//MimErode(m_mImgProc, m_mImgProc, 1, M_GRAYSCALE);
			//MbufSave(_T("D:\\test\\Erode.bmp"), m_mImgProc);

			//MimArith(m_mImgProc, 2, m_mImgProc, M_DIV_CONST);
			//MimArith(m_mImgProc, 2, m_mImgProc, M_MULT_CONST + M_SATURATION);
			//MbufSave(_T("D:\\test\\Arith.bmp"), m_mImgProc);

			//MbufCopy(m_mImgProc[cam_id], m_mImg[cam_id]);
			//MbufSave(_T("D:\\test\\4.bmp"), m_mImgProc);
			//MbufSave(_T("D:\\test\\5.bmp"), m_mImg[cam_id]);

			MimFree(MilHistogramEqualizeAdaptiveContext);
			//MbufFree(MilDisplayImage);

			// lk91 /////////////////////////////////////////////
		}
	}
	else {
		return FALSE;
	}
#else
	return TRUE;
#endif
}

/* desc:  Convert Mil Color */
COLORREF CMilMain::MilColorConvert(int col)
{
	COLORREF mcol;
	switch (col) {
	case eM_COLOR_BLACK:
		mcol = M_COLOR_BLACK;
		break;
	case eM_COLOR_RED:
		mcol = M_COLOR_RED;
		break;
	case eM_COLOR_GREEN:
		mcol = M_COLOR_GREEN;
		break;
	case eM_COLOR_BLUE:
		mcol = M_COLOR_BLUE;
		break;
	case eM_COLOR_YELLOW:
		mcol = M_COLOR_YELLOW;
		break;
	case eM_COLOR_MAGENTA:
		mcol = M_COLOR_MAGENTA;
		break;
	case eM_COLOR_CYAN:
		mcol = M_COLOR_CYAN;
		break;
	case eM_COLOR_WHITE:
		mcol = M_COLOR_WHITE;
		break;
	case eM_COLOR_GRAY:
		mcol = M_COLOR_GRAY;
		break;
	case eM_COLOR_BRIGHT_GRAY:
		mcol = M_COLOR_BRIGHT_GRAY;
		break;
	case eM_COLOR_LIGHT_GRAY:
		mcol = M_COLOR_LIGHT_GRAY;
		break;
	case eM_COLOR_LIGHT_GREEN:
		mcol = M_COLOR_LIGHT_GREEN;
		break;
	case eM_COLOR_LIGHT_BLUE:
		mcol = M_COLOR_LIGHT_BLUE;
		break;
	case eM_COLOR_LIGHT_WHITE:
		mcol = M_COLOR_LIGHT_WHITE;
		break;
	case eM_COLOR_DARK_RED:
		mcol = M_COLOR_DARK_RED;
		break;
	case eM_COLOR_DARK_GREEN:
		mcol = M_COLOR_DARK_GREEN;
		break;
	case eM_COLOR_DARK_BLUE:
		mcol = M_COLOR_DARK_BLUE;
		break;
	case eM_COLOR_DARK_YELLOW:
		mcol = M_COLOR_DARK_YELLOW;
		break;
	case eM_COLOR_DARK_MAGENTA:
		mcol = M_COLOR_DARK_MAGENTA;
		break;
	case eM_COLOR_DARK_CYAN:
		mcol = M_COLOR_DARK_CYAN;
		break;
	default:
		mcol = M_COLOR_GREEN;
	}

	return mcol;
}

BOOL CMilMain::RunModel_VisionCalib(UINT8 cam_id, UINT8 img_id, PUINT8 image, UINT8 dlg_id, UINT8 mark_no, int* roi_left, int* roi_right, int* roi_top, int* roi_bottom, int row, int col)
{
	DOUBLE dbMarkX = 0.0f, dbMarkY = 0.0f;
	LPG_GMFR pstResultMain = NULL, pstResultSide = NULL;

	/* ���� �ش� ��ü�� ���� ��ϵǾ� �ִ��� ���� */
	if (!m_pMilModel)	return FALSE;

#ifndef _NOT_USE_MIL_
	if (!m_pMilModel[cam_id - 1]->IsSetMarkModel(mark_no))	return FALSE;
#endif

	/* Grabbed Image ó���� ���� ��ü ���� */
	m_pLastGrabResult = m_pMilGrab[cam_id - 1][img_id];
	if (!m_pLastGrabResult)	return FALSE;

	/* ���� ���� �Ź��� ��ϵ� Mark�� �߽� X, Y ��ǥ */
	if (!m_pLastGrabResult->PutGrabImage(image, img_id, dbMarkX, dbMarkY))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"Failed to put the grabbed image");
	}
	else
	{
#ifndef _NOT_USE_MIL_
		/* Mark �˻� ���� */
		m_pMilModel[cam_id - 1]->RunModel_VisionCalib(m_mGraphID, m_pLastGrabResult->GetBufID(), img_id, dlg_id, mark_no, roi_left, roi_right, roi_top, roi_bottom, row, col);
#endif
	}

	return FALSE;
}