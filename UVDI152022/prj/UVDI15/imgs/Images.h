#pragma once

/* Image �⺻ ���� */

#include "../../../inc/comn/Picture.h"

class CImages
{
/* ������ / �ı��� */
public:
	CImages(HWND hwnd);
	virtual ~CImages();

/* ���� �Լ� */
protected:
public:

/* ���� ���� */
protected:

	TCHAR				m_tzFile[MAX_PATH_LEN];

	HWND				m_hPicWnd;	/* �̹����� ��µǴ� ���� �ڵ� */
	HDC					m_hPicDC;	/* �̹����� ��µǴ� DC */

	CPicture			m_picLogo;	/* Logo �̹��� ���� �������̽� */
	CRect				m_rDraw;

/* ���� �Լ� */
protected:

/* ���� �Լ� */
public:

	VOID				DrawImage();
	BOOL				LoadFile();

};

/* Logo File for PHILOPTICS */
class CImgPhil : public CImages
{
/* ������ / �ı��� */
public:
	CImgPhil(HWND hwnd);
	virtual ~CImgPhil()	{};
};

/* Logo File for SKC */
class CImgCustomer : public CImages
{
/* ������ / �ı��� */
public:
	CImgCustomer(HWND hwnd);
	virtual ~CImgCustomer()	{};
};

