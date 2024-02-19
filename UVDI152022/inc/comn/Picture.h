
#ifndef __I_PICTURE_H__
#define	__I_PICTURE_H__

class CPicture
{
public:

	CPicture();
	virtual ~CPicture();

protected:

	TCHAR				m_tzFileName[_MAX_PATH];

	IPicture			*m_IPicture;	// Same As LPPICTURE (typedef IPicture __RPC_FAR *LPPICTURE)

	LONG				m_Height;		// 이미지 높이(픽셀단위)
	LONG				m_Width;		// 이미지 넓이(픽셀단위)
	LONG				m_Weight;		// 이미지 크기(바이트단위)


public:

	BOOL				Load(TCHAR *sFilePathName);
	BOOL				Load(UINT32 ResourceName, LPCWSTR ResourceType);
	BOOL				LoadPictureData(BYTE* pBuffer, int nSize);
	BOOL				SaveAsBitmap(CString sFilePathName);
	BOOL				Show(CDC* pDC, CPoint LeftTop, CPoint WidthHeight, int MagnifyX, int MagnifyY);
	BOOL				Show(CDC* pDC, CRect DrawRect);
	BOOL				Show(HDC  hDC, CRect DrawRect);
	BOOL				ShowFit(HDC hdc, CRect& DrawRect);
	BOOL				ShowFit(HDC hdc, CRect &DrawRect, CRect& ImageRect);
	BOOL				ShowBitmapResource(CDC* pDC, const int BMPResource, CPoint LeftTop);
	BOOL				UpdateSizeOnDC(CDC* pDC);
	BOOL				IsDrawImage()		{	return	m_IPicture != NULL;	}

	VOID				FreePictureData();
	VOID				GetPicRect(RECT &rect);
	VOID				GetPicRect(CRect &rect);

	IPicture			*GetImgHandle()		{	return m_IPicture;		}

	TCHAR				*GetFileName()		{	return m_tzFileName;	};

	LONG				GetWidth()			{	return m_Width;			}
	LONG				GetHeight()			{	return m_Height;		}
};

#endif
