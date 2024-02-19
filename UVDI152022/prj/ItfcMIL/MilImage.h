#pragma once

#include "stdafx.h"

class CMilImage
{
/* ������ & �ı��� */
public:
#ifndef _NOT_USE_MIL_
	CMilImage(LPG_CIEA config, LPG_VDSM shmem,
			  UINT8 cam_id, /*MIL_ID ml_sys, MIL_ID ml_dis, */ENG_ERVM run_mode);
#else
	CMilImage(LPG_CIEA config, LPG_VDSM shmem,
		UINT8 cam_id, ENG_ERVM run_mode);
#endif
	virtual ~CMilImage();

/* ����ü */
protected:


/* ���� �Լ� */
protected:



/* ���� ���� */
protected:

	UINT8				m_u8ACamID;			/* Align Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT - 1) */

	UINT32				m_u32ImgWidth;		/* Grabbed Image Size (Width : Pixel) */
	UINT32				m_u32ImgHeight;		/* Grabbed Image Size (Height: Pixel) */
	UINT32				m_u32ImgSize;		/* Grabbed Image Size (Bytes. 256 gray color) */

#ifndef _NOT_USE_MIL_
	//MIL_ID				m_mlSysID;			/* MIL System (Frame Grabber) */
	//MIL_ID				m_mlDisID;			/* Display */
	MIL_ID				m_mlBufID;			/* Paterrn or Grabbed Image Buffer (����) */
											/* Source : �˻� ����� ���� �̹��� */
											/* Target : �˻� (ĸó)�� ���� �̹��� */
#endif

	ENG_AOEM			m_enAlignMode;		/* �뱤 ��� */

	LPG_CIEA			m_pstConfig;
	LPG_VDSM			m_pstShMemVisi;


/* ���� �Լ� */
protected:

	BOOL				SetBufAlloc2d();
	VOID				SetGrabSize(ENG_ERVM mode);


/* ���� �Լ� */
public:
#ifndef _NOT_USE_MIL_
	MIL_ID				GetBufID()			{	return m_mlBufID;		}
#endif
	UINT32				GetImageWidth()		{	return m_u32ImgWidth;	}
	UINT32				GetImageHeight()	{	return m_u32ImgHeight;	}
	UINT32				GetImageSize()		{	return m_u32ImgSize;	}

	UINT32				GetGaussianFilter(UINT8 index);

	/* !!! �뱤 ��� ���� : ���� �뱤, ����� �뱤, ����� ī�޶� ���� �� ���� �� ����� �뱤 !!! */
	VOID				SetAlignMode(ENG_AOEM mode)	{	m_enAlignMode	= mode;	}
};
