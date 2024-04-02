/*
 desc : MIL Based Image (Buffer) 관리 및 처리
*/

#include "pch.h"
#include "MainApp.h"
#include "MilImage.h"
#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : config	- [in]  Config Info.
		shmem	- [in]  Shared Memory
		cam_id	- [in]  Align Camera Index. (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		ml_sys	- [in]  MIL System ID
		ml_dis	- [in]  MIL Display ID
		run_mode- [in]  0x01 : CMPS, 0x01 : Vision Step
 retn : None
*/
#ifndef _NOT_USE_MIL_
CMilImage::CMilImage(LPG_CIEA config, LPG_VDSM shmem,
					 UINT8 cam_id, /* MIL_ID ml_dis, */MIL_ID ml_sys,ENG_ERVM run_mode)
#else
CMilImage::CMilImage(LPG_CIEA config, LPG_VDSM shmem,
					 UINT8 cam_id, ENG_ERVM run_mode)
#endif
{
	/* !!! 기본적으로 얼라인 및 보정 노광 방식으로 동작 */
	m_enAlignMode	= ENG_AOEM::en_calib_expose;

	m_pstConfig		= config;
	m_pstShMemVisi	= shmem;
	m_mlSysID = ml_sys;
	/* MIL System & Display ID 연결 */
#ifndef _NOT_USE_MIL_
	//theApp.clMilMain.m_mSysID		= ml_sys;
	//m_mlDisID		= ml_dis;
#endif
	/* Align Camera Index (0x00 ~ MAX_INSTALL_CAMERA_COUNT - 1) */
	m_u8ACamID		= cam_id;

	/* 패턴 이미지 혹은 Grabbed Image의 크기 설정 값 */
	SetGrabSize(run_mode);
	/* Grabbed Image가 저장될 버퍼 메모리 할당 */
	SetBufAlloc2d();
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CMilImage::~CMilImage()
{
	/* Image Buffer 메모리 해제 */
#ifndef _NOT_USE_MIL_
	if (m_mlBufID)
	{
		MbufFree(m_mlBufID);
		m_mlBufID	= M_NULL;
	}
#endif
}

/*
 desc : Grabbed Mode 값에 따른 Grabbed Image의 크기 설정
 parm : mode	- [in]  grabbed mode 값 (CMPS, STEP, SPEC, EXAM, VDOF, etc)
 retn : None
*/
VOID CMilImage::SetGrabSize(ENG_ERVM mode)
{
	m_u32ImgWidth	= 0;
	m_u32ImgHeight	= 0;

	switch (mode)
	{
	case ENG_ERVM::en_cmps_sw	:	m_u32ImgWidth	= m_pstConfig->set_cams.aoi_size[0];
									m_u32ImgHeight	= m_pstConfig->set_cams.aoi_size[1];	break;
	case ENG_ERVM::en_cali_step	:	m_u32ImgWidth	= m_pstConfig->set_cams.soi_size[0];
									m_u32ImgHeight	= m_pstConfig->set_cams.soi_size[1];	break;
	case ENG_ERVM::en_cali_vdof	:	m_u32ImgWidth	= m_pstConfig->set_cams.doi_size[0];
									m_u32ImgHeight	= m_pstConfig->set_cams.doi_size[1];	break;
	case ENG_ERVM::en_acam_spec	:	m_u32ImgWidth	= m_pstConfig->set_cams.spc_size[0];
									m_u32ImgHeight	= m_pstConfig->set_cams.spc_size[1];	break;
	case ENG_ERVM::en_stage_st	:	m_u32ImgWidth	= m_pstConfig->set_cams.stg_size[0];
									m_u32ImgHeight	= m_pstConfig->set_cams.stg_size[1];	break;
	case ENG_ERVM::en_cali_exam	:	m_u32ImgWidth	= m_pstConfig->set_cams.ioi_size[0];
									m_u32ImgHeight	= m_pstConfig->set_cams.ioi_size[1];	break;
	case ENG_ERVM::en_cali_meas	:	m_u32ImgWidth	= m_pstConfig->set_cams.mes_size[0];
									m_u32ImgHeight	= m_pstConfig->set_cams.mes_size[1];	break;
	case ENG_ERVM::en_hole_size	:	m_u32ImgWidth	= m_pstConfig->set_cams.hol_size[0];
									m_u32ImgHeight	= m_pstConfig->set_cams.hol_size[1];	break;
	}

	/* 환경 파일에 Grabbed Image 크기 정보가 없는 경우 */
	if (m_u32ImgWidth < 1 || m_u32ImgHeight < 1 ||
		m_u32ImgWidth > m_pstConfig->set_cams.acam_size[0] ||
		m_u32ImgHeight > m_pstConfig->set_cams.acam_size[1])
	{
		TCHAR tzMode[128]	= {NULL};
		swprintf_s(tzMode, 128, L"Failed to set size value for Grabbed Image "
								L"(CMilImage::SetGrabSize). Mode = %u", mode);
		AfxMessageBox(tzMode, MB_ICONINFORMATION|MB_TOPMOST);

		m_u32ImgWidth	= m_pstConfig->set_cams.acam_size[0];
		m_u32ImgHeight	= m_pstConfig->set_cams.acam_size[1];
		AfxMessageBox(L"Set to the default size for the camera.", MB_ICONINFORMATION|MB_TOPMOST);
	}
	/* 8 bit 256 color 이므로 가로 x 세로 하면 이미지 크기 나옴 */
	m_u32ImgSize	= m_u32ImgWidth * m_u32ImgHeight;
}

/*
 desc : MIL 관련 객체 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMilImage::SetBufAlloc2d()
{
#ifndef _NOT_USE_MIL_
#if 0
	/* Image Buffer 객체 할당 (2d 즉, 흑백) */
	MbufAlloc2d(theApp.clMilMain.m_mSysID,
				m_u32ImgWidth,
				m_u32ImgHeight,
				8+M_UNSIGNED,	/* == (8+M_UNSIGNED)	: data depth (8bit)이며 Unsigned 형으로 할당 */
				M_IMAGE+M_PROC,	/* == (M_IMAGE+M_PROC)	: 이미지용+영상처리용 으로 사용하겠다는 의미 */
								/* M_IMAGE+M_GRAB+M_DISP: 이미지용+영상획득용+영상출력용 으로 사용하겠다는 의미 */
				&m_mlBufID);
#else
	/* Image Buffer 객체 할당 (2d 즉, 흑백) */
	MbufAllocColor(theApp.clMilMain.m_mSysID, 1,
				m_u32ImgWidth,
				m_u32ImgHeight,
				8+M_UNSIGNED,			/* == (8+M_UNSIGNED)	: data depth (8bit)이며 Unsigned 형으로 할당 */
				M_IMAGE+M_PROC+M_DISP,	/* == (M_IMAGE+M_PROC)	: 이미지용+영상처리용 으로 사용하겠다는 의미 */
										/* M_IMAGE+M_GRAB+M_DISP: 이미지용+영상획득용+영상출력용 으로 사용하겠다는 의미 */
				&m_mlBufID);
#endif
	if (!m_mlBufID || MappGetError(M_CURRENT, M_NULL))
	{
		LOG_ERROR(ENG_EDIC::en_mil, L"MbufAllocColor : Failed to allocate <MappAlloc : Buffer>");
		return FALSE;
	}
	/* Image Buffer Clear */
	MbufClear(m_mlBufID, 0L);
#endif
	return TRUE;
}

/*
 desc : Gaussian Filter 값 반환
 parm : index	- [in]  Filter Index
 retn : 실제 필터 값 반환
*/
UINT32 CMilImage::GetGaussianFilter(UINT8 index)
{
#ifndef _NOT_USE_MIL_
	switch (index)
	{
	case 1	:	return M_EDGE_DETECT;
	case 2	:	return M_EDGE_DETECT2;
	case 3	:	return M_HORIZ_EDGE;
	case 4	:	return M_LAPLACIAN_EDGE;
	case 5	:	return M_LAPLACIAN_EDGE2;
	case 6	:	return M_SHARPEN;
	case 7	:	return M_SHARPEN2;
	case 8	:	return M_SMOOTH;
	case 9	:	return M_VERT_EDGE;
	}

	return M_EDGE_DETECT;
#else
	return 0;
#endif
}
