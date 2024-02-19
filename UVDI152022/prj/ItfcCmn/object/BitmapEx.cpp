
/*
 desc : Bitmap (Images) 모듈
*/

#include "pch.h"
#include "BitmapEx.h"


#ifdef _DEBUG
#define new	DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : Constructor
 parm : None
 retn : None
*/
CBitmapEx::CBitmapEx()
{
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CBitmapEx::~CBitmapEx()
{
}

/*
 desc : 비트맵 이미지 상하 반전 시키기
 parm : width	- [in]  이미지 크기 (넓이. Pixel)
		height	- [in]  이미지 크기 (높이. Pixel)
		image	- [out] 이미지 데이터가 들어있는 버퍼 / 반전(상하)된 이미지 데이터 저장
 retn : None
*/
VOID CBitmapEx::BmpDataUpDownChange(UINT32 width, UINT32 height, PUINT8 image)
{
	UINT32 i, j;

	/* 임시 버퍼 할당 */
	PUINT8 pInvert	= (PUINT8)::Alloc(width * height +1);

	for (i=0; i<height; i++)
	{
		for (j=0; j<width; j++)
		{
			pInvert[(height - i - 1) * width + j] = image[j + i * width];
		}
	}

	/* 값 복사 */
	memcpy(image, pInvert, width * height);

	/* 메모리 해제 */
	if (pInvert)	::Free(pInvert);
}

/*
 desc : Bits 데이터를 Bytes 데이터로 변환 즉, 각 8개의 비트 데이터를 1 바이트 값에 병합
 parm : bit_width	- [in]  Bits 이미지의 넓이 (해상도 값. 비트 개수가 아님)
		bit_height	- [in]  Bits 이미지의 높이 (해상도 값. 비트 개수가 이님)
		bit_data	- [in]  Bits 데이터가 저장된 포인터
		byte_data	- [in]  Bytes 데이터가 저장될 버퍼 포인터
		byte_size	- [out] 'byte_data'의 버퍼 크기 및 [out] 반환될 때 'byte_data'에 실제로 저장된 데이터 크기
 retn : None
*/
VOID CBitmapEx::ConvertBit2Byte(UINT32 bit_width, UINT32 bit_height, PUINT8 bit_data,
								PUINT8 byte_data, UINT32 &byte_size)
{
	/*
	 *** 추후 4 Bytes 즉, 32 bit 경계선 정렬 적용 해야 한다.
	 *** 현재는 1024 * 768로서 한 행(1024)마다 32 bit로 나누었을 경우
	 *** 값이 딱 맞게 떨어지지만 다른 해상도의 Bitmap일 경우, 에러가 발생한다
	*/

	UINT32 u32Col, u32Row, u32Cols = WIDTHBYTES(1, bit_width) * 8;
	UINT32 u32Bits	= 0, u32Bytes = 0;

	for (u32Row=0; u32Row<bit_height; u32Row++)
	{
		for (u32Col=0; u32Col<bit_width; u32Col++, u32Bits++)
		{
			if ((u32Row != 0 || u32Col != 0) && 0 == (u32Col & 7))	// (u32Col & 7) == (u32Col % 8)
			{
				u32Bytes++;
				byte_data[u32Bytes] |= bit_data[u32Bits];
			}
			else
			{
				byte_data[u32Bytes]	= (byte_data[u32Bytes] | bit_data[u32Bits]) << 1;
			}
		}
		for (;u32Col<u32Cols; u32Col++)
		{
			if (u32Col != 0 && 0 == (u32Col & 7))	// (u32Col & 7) == (u32Col % 8)
			{
				u32Bytes++;
			}
		}
	}

	// 총 저장된 바이트 크기 반환
	byte_size = (u32Cols * bit_height) / 8;
}

/*
 desc : Bytes 데이터를 Bits 데이터로 변환 즉, 각 1 바이트 내에 저장된 8 개의 비트 데이터를
		각각 1 바이트 버퍼에 나누어서 저장
 parm : byte_width	- [in]  Bytes 이미지의 넓이 (해상도 값. 비트 개수가 아님)
		byte_height	- [in]  Bytes 이미지의 높이 (해상도 값. 비트 개수가 이님)
		byte_data	- [in]  1 Bytes에 8개의 비트 데이터가 저장된 포인터
		bit_data	- [in]  1 Bytes 내에 각 8 개의 Bit 데이터들을 8 바이트 버퍼에 저장하기 위한 버퍼
 retn : None
*/
VOID CBitmapEx::ConvertByte2Bit(UINT32 byte_width, UINT32 byte_height, PUINT8 byte_data,
								PUINT8 bit_data)
{
	register UINT32 w, h, u32Bytes, u32Bits;
	register UINT32 u32WidthBytes = WIDTHBYTES(1, byte_width);	// 32 bit 경계선 정렬 값 찾아내기 위함

	// Block 이미지의 높이와 넓이 값만 루프 내부를 돌되, 32 bit 경계선 정렬까지는 동작할 필요 없음
	for (h=0; h<byte_height; h++)
	{
		for (w=0; w<byte_width; w++)
		{
			u32Bytes	= w / 8;	// 현재 몇 번째 바이트를 처리 했는지 여부
			u32Bits		= w & 7;	// 현재 바이트 내에 몇 번째 비트를 처리 했는지 여부 (w % 8) == (w & 7)

			/* --------------------------------------------------------------------------------- */
			u32Bytes	= u32WidthBytes * h + w / 8;	// 먼저 메모리 상에서 바이트의 위치 파악
			u32Bits		= w & 7;						// 바이트 중 몇 번째 비트 위치인지 파악 (w % 8) == (w & 7)
			bit_data[h * byte_width + w]	= (byte_data[u32Bytes] << u32Bits) && 0x80;
			/* --------------------------------------------------------------------------------- */
		}
	}
}

/*
 desc : 이미지를 상하(위에서 아래)로 뒤집기 (1바이트에 8개의 데이터가 저장된 정보를 다룬다)
 parm : cx, cy	- [in]  이미지의 크기 (가로/세로 길이)
		source	- [in]  회전 시킬 원본 이미지가 저장된 버퍼 포인터
		target	- [out] 회전된 이미지가 저장될 버퍼 포인터
 retn : None
*/
VOID CBitmapEx::ImageConvFlip(UINT32 cx, UINT32 cy, PUINT8 source, PUINT8 target)
{
	UINT8 u8Mask = 0x01;	// 0000 0001
	UINT32 x, y, cx_bytes = WIDTHBYTES(1, cx);

	/* ---------------------------------------------------------------------- */
	/* x 축이 cx가 아니라 cx_bytes*8 임                                       */
	/* 즉, 실제 크기보다 align이 맞추기 위해 추가 생성된 넓이까지 수행해야 함 */
	/* ---------------------------------------------------------------------- */
	for (y=0; y<cy; y++)
	{
		for (x=0; x<cx_bytes*8; x++)
		{
			target[y*cx_bytes+x/8] |= ((source[(cy-y-1)*cx_bytes+x/8] >> (x%8)) & u8Mask) << (x%8);
		}
	}
}

/*
 desc : 바이트 버퍼의 각 8비트(0 or 1)씩 저장된 데이터를 Binary Bitmap Format으로 변환 및 파일로 저장
 parm : buff	- [in]  바이트 버퍼의 1 바이트 마다 비트 값이 저장된 버퍼 포인터
		width	- [in]  저장하고자 하는 Bitmap Data의 넓이
		height	- [in]  저장하고자 하는 Bitmap Data의 높이
		file	- [in]  Bitmap 파일 (전체 경로 포함)
 retn : None
*/
VOID CBitmapEx::SaveBinaryToBMP(PUINT8 buff, UINT32 width, UINT32 height, PTCHAR file)
{
	UINT8 u8Data		= 0x00;
	UINT8 u8BmpHead[128]= {NULL};
	UINT8 u8Reserved	= 0x00;
	UINT16 u16Reserved	= 0x0000;
	UINT16 u16Data		= 0;
	UINT32 u32Width		= UINT32(width);
	UINT32 u32Height	= UINT32(height);
	UINT32 u32HeadSize	= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2;
	UINT32 u32FileSize	= u32HeadSize + WIDTHBYTES(1, u32Width) * u32Height;
	// 실제 비트맵 데이터가 저장된 위치... 흑백 바이너리 데이터 이므로... RQBQUAD * 2 해줌
	UINT32 u32Data		= 0;
	UINT32 u32BuffPos	= 0;
	UINT8 *pWriteBuff	= buff;
	UINT32 u32WriteSize	= 0;
	FILE *fpWrite		= NULL;
	CUniToChar csCnv;

	/* ----------------------------------------------------- */
	/*                   BITMAPFILEHEADER                    */
	/* ----------------------------------------------------- */
	// 아래 두 바이트는 'BM' 이라는 Hex 코드 값
	u8BmpHead[u32BuffPos]	= 0x42;						u32BuffPos += 1;
	u8BmpHead[u32BuffPos]	= 0x4d;						u32BuffPos += 1;
	// 파일 전체 크기 (헤더 포함한 전체 파일)
	memcpy(u8BmpHead+u32BuffPos,	&u32FileSize,	4);	u32BuffPos += 4;
	// 예약어로 두 워드는 초기화
	memcpy(u8BmpHead+u32BuffPos,	&u16Reserved,	2);	u32BuffPos += 2;
	memcpy(u8BmpHead+u32BuffPos,	&u16Reserved,	2);	u32BuffPos += 2;
	// 현재 위치로부터 실제 데이터가 존재하는 곳 까지의 거리
	memcpy(u8BmpHead+u32BuffPos,	&u32HeadSize,	4);	u32BuffPos += 4;
	/* ----------------------------------------------------- */
	/*                   BITMAPINFOHEADER                    */
	/* ----------------------------------------------------- */
	// BITMAPINFO 헤더 크기
	u32Data			= sizeof(BITMAPINFOHEADER);
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4);	u32BuffPos += 4;
	// 이미지 넓이/높이
	memcpy(u8BmpHead+u32BuffPos,	&u32Width,		4);	u32BuffPos += 4;
	memcpy(u8BmpHead+u32BuffPos,	&u32Height,		4);	u32BuffPos += 4;
	// 이미지의 장수(bitmap은 layer가 존재하지 않으므로 항상 1)
	u16Data			= 1;
	memcpy(u8BmpHead+u32BuffPos,	&u16Data,		2);	u32BuffPos += 2;
	// 한 픽셀의 컬러 비트수(256컬러는 2^8 이므로 8이 표기됨. 여기서는 바이너리 컬러이므로, 1)
	u16Data			= 1;
	memcpy(u8BmpHead+u32BuffPos,	&u16Data,		2);	u32BuffPos += 2;
	// 비트맵이 압축이 되었는지 어떠한 방식으로 압축이 되었는지 표시
	u32Data			= 0;	// BI_RGB = 0, BI_RLE8, BI_RLE4, BI_BITFIELDS = 3, BI_JPEG, BI_PNG ...
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4);	u32BuffPos += 4;
	// 비트맵 이미지의 본문 크기
	u32Data			= u32FileSize - u32HeadSize;	// 즉, 62 바이트의 비트맵 헤더 정보를 제외한 나머지 크기
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4);	u32BuffPos += 4;
	// 비트맵의 타켓 장치의 수평/수직 해상도를 1 미터 당의 픽셀 단위로 지정한다.
	u32Data			= 0;
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4); u32BuffPos += 4;	// 수평
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4); u32BuffPos += 4;	// 수직
	// 컬러 테이블 내의 컬러 인덱스 가운데, 비트맵 내에서 실제로 사용하는 인덱스의 수를 지정한다.
	u32Data			= 2;	// 흑색, 백색 이므로 2
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4);	u32BuffPos += 4;
	// 비트맵을 표시하기 위해서 중요라고 보여지는 컬럭 인덱스 수를 지정 한다. 이 값이 제로의 경우는 모든 색이 중요라고 보여진다.
	u32Data			= 2;
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4);	u32BuffPos += 4;
	/* ----------------------------------------------------- */
	/*                RGBQUAD (Black, White)                 */
	/* ----------------------------------------------------- */
	// 흑색 팔레트 - rgbBlue, rgbGreen, rgbRed, rgu8Reserved
	u8Data			= 0;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Reserved,	1);	u32BuffPos += 1;
	// 백색 팔레트 - rgbBlue, rgbGreen, rgbRed, rgu8Reserved
	u8Data			= 0xff;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Reserved,	1);	u32BuffPos += 1;

	// 파일 쓰기 모드로 열기
	if (0 == fopen_s(&fpWrite, csCnv.Uni2Ansi(file), "wb"))
	{
		// Bits Data가 저장될 버퍼 크기 계산
		u32WriteSize	= WIDTHBYTES(1, u32Width) * u32Height;
		// 헤더 저장
		fwrite(u8BmpHead, sizeof(UINT8), u32HeadSize, fpWrite);
		// 본문 저장
		fwrite(pWriteBuff, sizeof(UINT8), u32WriteSize, fpWrite);
		// 파일 닫기
		fclose(fpWrite);
	}
}

/*
 desc : 바이트 버퍼마다에 비트(0 or 1)로 저장된 데이터를 Binary Bitmap Format으로 변환 및 파일로 저장
 parm : buff	- [in]  바이트 버퍼의 1 바이트 마다 비트 값이 저장된 버퍼 포인터
		width	- [in]  저장하고자 하는 Bitmap Data의 넓이
		height	- [in]  저장하고자 하는 Bitmap Data의 높이
		file	- [in]  Bitmap 파일 (전체 경로 포함)
 retn : None
*/
VOID CBitmapEx::SaveBitsToBMP(PUINT8 buff, UINT32 width, UINT32 height, PTCHAR file)
{
	UINT8 u8Data		= 0x00;
	UINT8 u8BmpHead[128]= {NULL};
	UINT8 u8Reserved	= 0x00;
	UINT16 u16Reserved	= 0x0000;
	UINT16 u16Data		= 0;
	UINT32 u32Width		= UINT32(width);
	UINT32 u32Height	= UINT32(height);
	UINT32 u32HeadSize	= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2;
	UINT32 u32FileSize	= u32HeadSize + WIDTHBYTES(1, u32Width) * u32Height;
	// 실제 비트맵 데이터가 저장된 위치... 흑백 바이너리 데이터 이므로... RQBQUAD * 2 해줌
	UINT32 u32Data		= 0;
	UINT32 u32BuffPos	= 0;
	UINT8 *pWriteBuff	= NULL;
	UINT32 u32WriteSize	= 0;
	FILE *fpWrite		= NULL;
	CUniToChar csCnv;

	/* ----------------------------------------------------- */
	/*                   BITMAPFILEHEADER                    */
	/* ----------------------------------------------------- */
	// 아래 두 바이트는 'BM' 이라는 Hex 코드 값
	u8BmpHead[u32BuffPos]	= 0x42;						u32BuffPos += 1;
	u8BmpHead[u32BuffPos]	= 0x4d;						u32BuffPos += 1;
	// 파일 전체 크기 (헤더 포함한 전체 파일)
	memcpy(u8BmpHead+u32BuffPos,	&u32FileSize,	4);	u32BuffPos += 4;
	// 예약어로 두 워드는 초기화
	memcpy(u8BmpHead+u32BuffPos,	&u16Reserved,	2);	u32BuffPos += 2;
	memcpy(u8BmpHead+u32BuffPos,	&u16Reserved,	2);	u32BuffPos += 2;
	// 현재 위치로부터 실제 데이터가 존재하는 곳 까지의 거리
	memcpy(u8BmpHead+u32BuffPos,	&u32HeadSize,	4);	u32BuffPos += 4;
	/* ----------------------------------------------------- */
	/*                   BITMAPINFOHEADER                    */
	/* ----------------------------------------------------- */
	// BITMAPINFO 헤더 크기
	u32Data			= sizeof(BITMAPINFOHEADER);
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4);	u32BuffPos += 4;
	// 이미지 넓이/높이
	memcpy(u8BmpHead+u32BuffPos,	&u32Width,		4);	u32BuffPos += 4;
	memcpy(u8BmpHead+u32BuffPos,	&u32Height,		4);	u32BuffPos += 4;
	// 이미지의 장수(bitmap은 layer가 존재하지 않으므로 항상 1)
	u16Data			= 1;
	memcpy(u8BmpHead+u32BuffPos,	&u16Data,		2);	u32BuffPos += 2;
	// 한 픽셀의 컬러 비트수(256컬러는 2^8 이므로 8이 표기됨. 여기서는 바이너리 컬러이므로, 1)
	u16Data			= 1;
	memcpy(u8BmpHead+u32BuffPos,	&u16Data,		2);	u32BuffPos += 2;
	// 비트맵이 압축이 되었는지 어떠한 방식으로 압축이 되었는지 표시
	u32Data			= 0;	// BI_RGB = 0, BI_RLE8, BI_RLE4, BI_BITFIELDS = 3, BI_JPEG, BI_PNG ...
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4);	u32BuffPos += 4;
	// 비트맵 이미지의 본문 크기
	u32Data			= u32FileSize - u32HeadSize;	// 즉, 62 바이트의 비트맵 헤더 정보를 제외한 나머지 크기
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4);	u32BuffPos += 4;
	// 비트맵의 타켓 장치의 수평/수직 해상도를 1 미터 당의 픽셀 단위로 지정한다.
	u32Data			= 0;
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4); u32BuffPos += 4;	// 수평
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4); u32BuffPos += 4;	// 수직
	// 컬러 테이블 내의 컬러 인덱스 가운데, 비트맵 내에서 실제로 사용하는 인덱스의 수를 지정한다.
	u32Data			= 2;	// 흑색, 백색 이므로 2
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4);	u32BuffPos += 4;
	// 비트맵을 표시하기 위해서 중요라고 보여지는 컬럭 인덱스 수를 지정 한다. 이 값이 제로의 경우는 모든 색이 중요라고 보여진다.
	u32Data			= 2;
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4);	u32BuffPos += 4;
	/* ----------------------------------------------------- */
	/*                RGBQUAD (Black, White)                 */
	/* ----------------------------------------------------- */
	// 흑색 팔레트 - rgbBlue, rgbGreen, rgbRed, rgu8Reserved
	u8Data			= 0;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Reserved,	1);	u32BuffPos += 1;
	// 백색 팔레트 - rgbBlue, rgbGreen, rgbRed, rgu8Reserved
	u8Data			= 0xff;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Reserved,	1);	u32BuffPos += 1;

	// 파일 쓰기 모드로 열기
	if (0 == fopen_s(&fpWrite, csCnv.Uni2Ansi(file), "wb"))
	{
		// Data가 저장될 버퍼 공간 확보 (최적 공간으로 계산 (?))
		u32WriteSize	= WIDTHBYTES(1, width) * height;
		pWriteBuff		= new UINT8 [u32WriteSize+1];
		if (pWriteBuff)
		{
			memset(pWriteBuff, 0x00, u32WriteSize+1);	// 꼭 초기화

			// Bits Data가 저장될 버퍼 공간 확보 (최적 공간으로 계산 (?))
			ConvertBit2Byte(width, height, buff, pWriteBuff, u32WriteSize);
			// 헤더 저장
			fwrite(u8BmpHead, sizeof(UINT8), u32HeadSize, fpWrite);
			// 본문 저장
			fwrite(pWriteBuff, sizeof(UINT8), u32WriteSize, fpWrite);

			// Data 메모리 해제
			delete [] pWriteBuff;
		}
		// 파일로 저장
		fclose(fpWrite);
	}
}

/*
 desc : DC 에 그려진(출력된) 이미지를 비트맵 파일로 저장
 parm : hdc		- [in]  DC Handle
		hbmp	- [in]  Bitmap Handle
		bpps	- [in]  Bit per Pixel
		width	- [in]  DC에 그려진 이미지 크기
		height	- [in]  DC에 그려진 이미지 크기
		file	- [in]  Bitmap 파일 (전체 경로 포함)
 retn : TRUE or FALSE
*/
BOOL CBitmapEx::SaveDCToBitmap(HDC hdc, HBITMAP hbmp, UINT16 bpps, UINT32 width, UINT32 height, PTCHAR file)
{ 
	UINT32 u32SizeBmpInfo;
	DWORD dwWrite, dwWriten;
	HDC hDC			= NULL;
	HGDIOBJ hObjSel	= NULL;
	VOID *pDibData	= NULL;
	HANDLE hFile	= INVALID_HANDLE_VALUE;
	BITMAPFILEHEADER stBmpFileHead	= {NULL};
	LPBITMAPINFO pBmpInfo	= NULL;
  
	hDC	= CreateCompatibleDC(hdc);
	if (!hDC)	return FALSE;
  
	u32SizeBmpInfo	= sizeof(BITMAPINFOHEADER) + (256 * sizeof(RGBQUAD)); 
	pBmpInfo		= (LPBITMAPINFO) new UINT8 [u32SizeBmpInfo+1];
	ZeroMemory(pBmpInfo, u32SizeBmpInfo+1); 
  
	pBmpInfo->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER); 
	pBmpInfo->bmiHeader.biWidth			= width; 
	pBmpInfo->bmiHeader.biHeight		= height; 
	pBmpInfo->bmiHeader.biPlanes		= 1; 
	pBmpInfo->bmiHeader.biBitCount		= bpps; 
	pBmpInfo->bmiHeader.biCompression	= BI_RGB; 
	pBmpInfo->bmiHeader.biSizeImage		= ((((pBmpInfo->bmiHeader.biWidth * pBmpInfo->bmiHeader.biBitCount) + 31) & ~31) >> 3) * pBmpInfo->bmiHeader.biHeight; 
  	pBmpInfo->bmiColors[1].rgbRed		= 255; 
	pBmpInfo->bmiColors[1].rgbGreen		= 255; 
	pBmpInfo->bmiColors[1].rgbBlue		= 255; 
  
	hbmp = CreateDIBSection(hdc, pBmpInfo,DIB_RGB_COLORS,(void**)&pDibData,NULL,NULL);
	if (!hbmp)
	{
		delete pBmpInfo;
		DeleteDC(hDC);
		return FALSE;
	}

	hObjSel	= SelectObject(hDC, hbmp); 
	BitBlt(hDC, 0, 0, width, height, hdc, 0, 0, SRCCOPY); 

	ZeroMemory(&stBmpFileHead, sizeof(BITMAPFILEHEADER));
	stBmpFileHead.bfOffBits	= sizeof(BITMAPFILEHEADER) + u32SizeBmpInfo;
	stBmpFileHead.bfSize	= (pBmpInfo->bmiHeader.biHeight * pBmpInfo->bmiHeader.biWidth) + sizeof(BITMAPFILEHEADER) + u32SizeBmpInfo;
	stBmpFileHead.bfType	= 0x4d42;
  
	hFile=CreateFile(file, GENERIC_READ|GENERIC_WRITE,(DWORD)0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
	if (hFile==INVALID_HANDLE_VALUE)
	{
		delete pBmpInfo;
		DeleteObject(hbmp);
		return FALSE;
	}
	// Write the BITMAPFILEHEADER
	dwWrite	= sizeof(BITMAPFILEHEADER);
	if (!WriteFile(hFile,(void*)&stBmpFileHead, dwWrite, &dwWriten, NULL))
	{
		delete pBmpInfo;
		DeleteObject(hbmp);
		DeleteDC(hDC);
		CloseHandle(hFile);
		return FALSE;
	}
	// Write the BITMAPINFOHEADER
	dwWrite	= u32SizeBmpInfo;
	if (!WriteFile(hFile,(void*)pBmpInfo,dwWrite,&dwWriten,NULL))
	{
		delete pBmpInfo;
		DeleteObject(hbmp);
		DeleteDC(hDC);
		CloseHandle(hFile);
		return FALSE;
	}
	// Write the Color Index Array???
	dwWrite	= pBmpInfo->bmiHeader.biSizeImage;	// 3 * bmih.biHeight * bmih.biWidth;
	if (!WriteFile(hFile,(void*)pDibData, dwWrite, &dwWriten, NULL))
	{
		delete pBmpInfo;
		DeleteObject(hbmp);
		DeleteDC(hDC);
		CloseHandle(hFile);
		return FALSE;
	}

	/* 메모리 해제 */
	delete pBmpInfo;

	CloseHandle(hFile);

	DeleteObject(SelectObject(hDC, hObjSel));
	DeleteObject(hbmp);
	DeleteDC(hDC);
  
	return TRUE;
}
