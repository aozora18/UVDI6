#pragma once

/* Image 기본 파일 */

#include "../../../inc/comn/Picture.h"

class CImages
{
/* 생성자 / 파괴자 */
public:
	CImages(HWND hwnd);
	virtual ~CImages();

/* 가상 함수 */
protected:
public:

/* 로컬 변수 */
protected:

	TCHAR				m_tzFile[MAX_PATH_LEN];

	HWND				m_hPicWnd;	/* 이미지가 출력되는 윈도 핸들 */
	HDC					m_hPicDC;	/* 이미지가 출력되는 DC */

	CPicture			m_picLogo;	/* Logo 이미지 파일 인터페이스 */
	CRect				m_rDraw;

/* 로컬 함수 */
protected:

/* 공용 함수 */
public:

	VOID				DrawImage();
	BOOL				LoadFile();

};

/* Logo File for PHILOPTICS */
class CImgPhil : public CImages
{
/* 생성자 / 파괴자 */
public:
	CImgPhil(HWND hwnd);
	virtual ~CImgPhil()	{};
};

/* Logo File for SKC */
class CImgCustomer : public CImages
{
/* 생성자 / 파괴자 */
public:
	CImgCustomer(HWND hwnd);
	virtual ~CImgCustomer()	{};
};

