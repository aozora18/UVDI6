#pragma once

#include "stdafx.h"

class CMilImage
{
/* 생성자 & 파괴자 */
public:
#ifndef _NOT_USE_MIL_
	CMilImage(LPG_CIEA config, LPG_VDSM shmem,
			  UINT8 cam_id, /*MIL_ID ml_sys, MIL_ID ml_dis, */ENG_ERVM run_mode);
#else
	CMilImage(LPG_CIEA config, LPG_VDSM shmem,
		UINT8 cam_id, ENG_ERVM run_mode);
#endif
	virtual ~CMilImage();

/* 구조체 */
protected:


/* 가상 함수 */
protected:



/* 로컬 변수 */
protected:

	UINT8				m_u8ACamID;			/* Align Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT - 1) */

	UINT32				m_u32ImgWidth;		/* Grabbed Image Size (Width : Pixel) */
	UINT32				m_u32ImgHeight;		/* Grabbed Image Size (Height: Pixel) */
	UINT32				m_u32ImgSize;		/* Grabbed Image Size (Bytes. 256 gray color) */

#ifndef _NOT_USE_MIL_
	//MIL_ID				m_mlSysID;			/* MIL System (Frame Grabber) */
	//MIL_ID				m_mlDisID;			/* Display */
	MIL_ID				m_mlBufID;			/* Paterrn or Grabbed Image Buffer (영상) */
											/* Source : 검색 대상의 패턴 이미지 */
											/* Target : 검색 (캡처)된 패턴 이미지 */
#endif

	ENG_AOEM			m_enAlignMode;		/* 노광 방식 */

	LPG_CIEA			m_pstConfig;
	LPG_VDSM			m_pstShMemVisi;


/* 로컬 함수 */
protected:

	BOOL				SetBufAlloc2d();
	VOID				SetGrabSize(ENG_ERVM mode);


/* 공용 함수 */
public:
#ifndef _NOT_USE_MIL_
	MIL_ID				GetBufID()			{	return m_mlBufID;		}
#endif
	UINT32				GetImageWidth()		{	return m_u32ImgWidth;	}
	UINT32				GetImageHeight()	{	return m_u32ImgHeight;	}
	UINT32				GetImageSize()		{	return m_u32ImgSize;	}

	UINT32				GetGaussianFilter(UINT8 index);

	/* !!! 노광 모드 설정 : 직접 노광, 얼라인 노광, 얼라인 카메라 보정 값 적용 후 얼라인 노광 !!! */
	VOID				SetAlignMode(ENG_AOEM mode)	{	m_enAlignMode	= mode;	}
};
