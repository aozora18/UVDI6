
/*
 desc : Bitmap (Images) ���
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
 desc : ��Ʈ�� �̹��� ���� ���� ��Ű��
 parm : width	- [in]  �̹��� ũ�� (����. Pixel)
		height	- [in]  �̹��� ũ�� (����. Pixel)
		image	- [out] �̹��� �����Ͱ� ����ִ� ���� / ����(����)�� �̹��� ������ ����
 retn : None
*/
VOID CBitmapEx::BmpDataUpDownChange(UINT32 width, UINT32 height, PUINT8 image)
{
	UINT32 i, j;

	/* �ӽ� ���� �Ҵ� */
	PUINT8 pInvert	= (PUINT8)::Alloc(width * height +1);

	for (i=0; i<height; i++)
	{
		for (j=0; j<width; j++)
		{
			pInvert[(height - i - 1) * width + j] = image[j + i * width];
		}
	}

	/* �� ���� */
	memcpy(image, pInvert, width * height);

	/* �޸� ���� */
	if (pInvert)	::Free(pInvert);
}

/*
 desc : Bits �����͸� Bytes �����ͷ� ��ȯ ��, �� 8���� ��Ʈ �����͸� 1 ����Ʈ ���� ����
 parm : bit_width	- [in]  Bits �̹����� ���� (�ػ� ��. ��Ʈ ������ �ƴ�)
		bit_height	- [in]  Bits �̹����� ���� (�ػ� ��. ��Ʈ ������ �̴�)
		bit_data	- [in]  Bits �����Ͱ� ����� ������
		byte_data	- [in]  Bytes �����Ͱ� ����� ���� ������
		byte_size	- [out] 'byte_data'�� ���� ũ�� �� [out] ��ȯ�� �� 'byte_data'�� ������ ����� ������ ũ��
 retn : None
*/
VOID CBitmapEx::ConvertBit2Byte(UINT32 bit_width, UINT32 bit_height, PUINT8 bit_data,
								PUINT8 byte_data, UINT32 &byte_size)
{
	/*
	 *** ���� 4 Bytes ��, 32 bit ��輱 ���� ���� �ؾ� �Ѵ�.
	 *** ����� 1024 * 768�μ� �� ��(1024)���� 32 bit�� �������� ���
	 *** ���� �� �°� ���������� �ٸ� �ػ��� Bitmap�� ���, ������ �߻��Ѵ�
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

	// �� ����� ����Ʈ ũ�� ��ȯ
	byte_size = (u32Cols * bit_height) / 8;
}

/*
 desc : Bytes �����͸� Bits �����ͷ� ��ȯ ��, �� 1 ����Ʈ ���� ����� 8 ���� ��Ʈ �����͸�
		���� 1 ����Ʈ ���ۿ� ����� ����
 parm : byte_width	- [in]  Bytes �̹����� ���� (�ػ� ��. ��Ʈ ������ �ƴ�)
		byte_height	- [in]  Bytes �̹����� ���� (�ػ� ��. ��Ʈ ������ �̴�)
		byte_data	- [in]  1 Bytes�� 8���� ��Ʈ �����Ͱ� ����� ������
		bit_data	- [in]  1 Bytes ���� �� 8 ���� Bit �����͵��� 8 ����Ʈ ���ۿ� �����ϱ� ���� ����
 retn : None
*/
VOID CBitmapEx::ConvertByte2Bit(UINT32 byte_width, UINT32 byte_height, PUINT8 byte_data,
								PUINT8 bit_data)
{
	register UINT32 w, h, u32Bytes, u32Bits;
	register UINT32 u32WidthBytes = WIDTHBYTES(1, byte_width);	// 32 bit ��輱 ���� �� ã�Ƴ��� ����

	// Block �̹����� ���̿� ���� ���� ���� ���θ� ����, 32 bit ��輱 ���ı����� ������ �ʿ� ����
	for (h=0; h<byte_height; h++)
	{
		for (w=0; w<byte_width; w++)
		{
			u32Bytes	= w / 8;	// ���� �� ��° ����Ʈ�� ó�� �ߴ��� ����
			u32Bits		= w & 7;	// ���� ����Ʈ ���� �� ��° ��Ʈ�� ó�� �ߴ��� ���� (w % 8) == (w & 7)

			/* --------------------------------------------------------------------------------- */
			u32Bytes	= u32WidthBytes * h + w / 8;	// ���� �޸� �󿡼� ����Ʈ�� ��ġ �ľ�
			u32Bits		= w & 7;						// ����Ʈ �� �� ��° ��Ʈ ��ġ���� �ľ� (w % 8) == (w & 7)
			bit_data[h * byte_width + w]	= (byte_data[u32Bytes] << u32Bits) && 0x80;
			/* --------------------------------------------------------------------------------- */
		}
	}
}

/*
 desc : �̹����� ����(������ �Ʒ�)�� ������ (1����Ʈ�� 8���� �����Ͱ� ����� ������ �ٷ��)
 parm : cx, cy	- [in]  �̹����� ũ�� (����/���� ����)
		source	- [in]  ȸ�� ��ų ���� �̹����� ����� ���� ������
		target	- [out] ȸ���� �̹����� ����� ���� ������
 retn : None
*/
VOID CBitmapEx::ImageConvFlip(UINT32 cx, UINT32 cy, PUINT8 source, PUINT8 target)
{
	UINT8 u8Mask = 0x01;	// 0000 0001
	UINT32 x, y, cx_bytes = WIDTHBYTES(1, cx);

	/* ---------------------------------------------------------------------- */
	/* x ���� cx�� �ƴ϶� cx_bytes*8 ��                                       */
	/* ��, ���� ũ�⺸�� align�� ���߱� ���� �߰� ������ ���̱��� �����ؾ� �� */
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
 desc : ����Ʈ ������ �� 8��Ʈ(0 or 1)�� ����� �����͸� Binary Bitmap Format���� ��ȯ �� ���Ϸ� ����
 parm : buff	- [in]  ����Ʈ ������ 1 ����Ʈ ���� ��Ʈ ���� ����� ���� ������
		width	- [in]  �����ϰ��� �ϴ� Bitmap Data�� ����
		height	- [in]  �����ϰ��� �ϴ� Bitmap Data�� ����
		file	- [in]  Bitmap ���� (��ü ��� ����)
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
	// ���� ��Ʈ�� �����Ͱ� ����� ��ġ... ��� ���̳ʸ� ������ �̹Ƿ�... RQBQUAD * 2 ����
	UINT32 u32Data		= 0;
	UINT32 u32BuffPos	= 0;
	UINT8 *pWriteBuff	= buff;
	UINT32 u32WriteSize	= 0;
	FILE *fpWrite		= NULL;
	CUniToChar csCnv;

	/* ----------------------------------------------------- */
	/*                   BITMAPFILEHEADER                    */
	/* ----------------------------------------------------- */
	// �Ʒ� �� ����Ʈ�� 'BM' �̶�� Hex �ڵ� ��
	u8BmpHead[u32BuffPos]	= 0x42;						u32BuffPos += 1;
	u8BmpHead[u32BuffPos]	= 0x4d;						u32BuffPos += 1;
	// ���� ��ü ũ�� (��� ������ ��ü ����)
	memcpy(u8BmpHead+u32BuffPos,	&u32FileSize,	4);	u32BuffPos += 4;
	// ������ �� ����� �ʱ�ȭ
	memcpy(u8BmpHead+u32BuffPos,	&u16Reserved,	2);	u32BuffPos += 2;
	memcpy(u8BmpHead+u32BuffPos,	&u16Reserved,	2);	u32BuffPos += 2;
	// ���� ��ġ�κ��� ���� �����Ͱ� �����ϴ� �� ������ �Ÿ�
	memcpy(u8BmpHead+u32BuffPos,	&u32HeadSize,	4);	u32BuffPos += 4;
	/* ----------------------------------------------------- */
	/*                   BITMAPINFOHEADER                    */
	/* ----------------------------------------------------- */
	// BITMAPINFO ��� ũ��
	u32Data			= sizeof(BITMAPINFOHEADER);
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4);	u32BuffPos += 4;
	// �̹��� ����/����
	memcpy(u8BmpHead+u32BuffPos,	&u32Width,		4);	u32BuffPos += 4;
	memcpy(u8BmpHead+u32BuffPos,	&u32Height,		4);	u32BuffPos += 4;
	// �̹����� ���(bitmap�� layer�� �������� �����Ƿ� �׻� 1)
	u16Data			= 1;
	memcpy(u8BmpHead+u32BuffPos,	&u16Data,		2);	u32BuffPos += 2;
	// �� �ȼ��� �÷� ��Ʈ��(256�÷��� 2^8 �̹Ƿ� 8�� ǥ���. ���⼭�� ���̳ʸ� �÷��̹Ƿ�, 1)
	u16Data			= 1;
	memcpy(u8BmpHead+u32BuffPos,	&u16Data,		2);	u32BuffPos += 2;
	// ��Ʈ���� ������ �Ǿ����� ��� ������� ������ �Ǿ����� ǥ��
	u32Data			= 0;	// BI_RGB = 0, BI_RLE8, BI_RLE4, BI_BITFIELDS = 3, BI_JPEG, BI_PNG ...
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4);	u32BuffPos += 4;
	// ��Ʈ�� �̹����� ���� ũ��
	u32Data			= u32FileSize - u32HeadSize;	// ��, 62 ����Ʈ�� ��Ʈ�� ��� ������ ������ ������ ũ��
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4);	u32BuffPos += 4;
	// ��Ʈ���� Ÿ�� ��ġ�� ����/���� �ػ󵵸� 1 ���� ���� �ȼ� ������ �����Ѵ�.
	u32Data			= 0;
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4); u32BuffPos += 4;	// ����
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4); u32BuffPos += 4;	// ����
	// �÷� ���̺� ���� �÷� �ε��� ���, ��Ʈ�� ������ ������ ����ϴ� �ε����� ���� �����Ѵ�.
	u32Data			= 2;	// ���, ��� �̹Ƿ� 2
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4);	u32BuffPos += 4;
	// ��Ʈ���� ǥ���ϱ� ���ؼ� �߿��� �������� �÷� �ε��� ���� ���� �Ѵ�. �� ���� ������ ���� ��� ���� �߿��� ��������.
	u32Data			= 2;
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4);	u32BuffPos += 4;
	/* ----------------------------------------------------- */
	/*                RGBQUAD (Black, White)                 */
	/* ----------------------------------------------------- */
	// ��� �ȷ�Ʈ - rgbBlue, rgbGreen, rgbRed, rgu8Reserved
	u8Data			= 0;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Reserved,	1);	u32BuffPos += 1;
	// ��� �ȷ�Ʈ - rgbBlue, rgbGreen, rgbRed, rgu8Reserved
	u8Data			= 0xff;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Reserved,	1);	u32BuffPos += 1;

	// ���� ���� ���� ����
	if (0 == fopen_s(&fpWrite, csCnv.Uni2Ansi(file), "wb"))
	{
		// Bits Data�� ����� ���� ũ�� ���
		u32WriteSize	= WIDTHBYTES(1, u32Width) * u32Height;
		// ��� ����
		fwrite(u8BmpHead, sizeof(UINT8), u32HeadSize, fpWrite);
		// ���� ����
		fwrite(pWriteBuff, sizeof(UINT8), u32WriteSize, fpWrite);
		// ���� �ݱ�
		fclose(fpWrite);
	}
}

/*
 desc : ����Ʈ ���۸��ٿ� ��Ʈ(0 or 1)�� ����� �����͸� Binary Bitmap Format���� ��ȯ �� ���Ϸ� ����
 parm : buff	- [in]  ����Ʈ ������ 1 ����Ʈ ���� ��Ʈ ���� ����� ���� ������
		width	- [in]  �����ϰ��� �ϴ� Bitmap Data�� ����
		height	- [in]  �����ϰ��� �ϴ� Bitmap Data�� ����
		file	- [in]  Bitmap ���� (��ü ��� ����)
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
	// ���� ��Ʈ�� �����Ͱ� ����� ��ġ... ��� ���̳ʸ� ������ �̹Ƿ�... RQBQUAD * 2 ����
	UINT32 u32Data		= 0;
	UINT32 u32BuffPos	= 0;
	UINT8 *pWriteBuff	= NULL;
	UINT32 u32WriteSize	= 0;
	FILE *fpWrite		= NULL;
	CUniToChar csCnv;

	/* ----------------------------------------------------- */
	/*                   BITMAPFILEHEADER                    */
	/* ----------------------------------------------------- */
	// �Ʒ� �� ����Ʈ�� 'BM' �̶�� Hex �ڵ� ��
	u8BmpHead[u32BuffPos]	= 0x42;						u32BuffPos += 1;
	u8BmpHead[u32BuffPos]	= 0x4d;						u32BuffPos += 1;
	// ���� ��ü ũ�� (��� ������ ��ü ����)
	memcpy(u8BmpHead+u32BuffPos,	&u32FileSize,	4);	u32BuffPos += 4;
	// ������ �� ����� �ʱ�ȭ
	memcpy(u8BmpHead+u32BuffPos,	&u16Reserved,	2);	u32BuffPos += 2;
	memcpy(u8BmpHead+u32BuffPos,	&u16Reserved,	2);	u32BuffPos += 2;
	// ���� ��ġ�κ��� ���� �����Ͱ� �����ϴ� �� ������ �Ÿ�
	memcpy(u8BmpHead+u32BuffPos,	&u32HeadSize,	4);	u32BuffPos += 4;
	/* ----------------------------------------------------- */
	/*                   BITMAPINFOHEADER                    */
	/* ----------------------------------------------------- */
	// BITMAPINFO ��� ũ��
	u32Data			= sizeof(BITMAPINFOHEADER);
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4);	u32BuffPos += 4;
	// �̹��� ����/����
	memcpy(u8BmpHead+u32BuffPos,	&u32Width,		4);	u32BuffPos += 4;
	memcpy(u8BmpHead+u32BuffPos,	&u32Height,		4);	u32BuffPos += 4;
	// �̹����� ���(bitmap�� layer�� �������� �����Ƿ� �׻� 1)
	u16Data			= 1;
	memcpy(u8BmpHead+u32BuffPos,	&u16Data,		2);	u32BuffPos += 2;
	// �� �ȼ��� �÷� ��Ʈ��(256�÷��� 2^8 �̹Ƿ� 8�� ǥ���. ���⼭�� ���̳ʸ� �÷��̹Ƿ�, 1)
	u16Data			= 1;
	memcpy(u8BmpHead+u32BuffPos,	&u16Data,		2);	u32BuffPos += 2;
	// ��Ʈ���� ������ �Ǿ����� ��� ������� ������ �Ǿ����� ǥ��
	u32Data			= 0;	// BI_RGB = 0, BI_RLE8, BI_RLE4, BI_BITFIELDS = 3, BI_JPEG, BI_PNG ...
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4);	u32BuffPos += 4;
	// ��Ʈ�� �̹����� ���� ũ��
	u32Data			= u32FileSize - u32HeadSize;	// ��, 62 ����Ʈ�� ��Ʈ�� ��� ������ ������ ������ ũ��
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4);	u32BuffPos += 4;
	// ��Ʈ���� Ÿ�� ��ġ�� ����/���� �ػ󵵸� 1 ���� ���� �ȼ� ������ �����Ѵ�.
	u32Data			= 0;
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4); u32BuffPos += 4;	// ����
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4); u32BuffPos += 4;	// ����
	// �÷� ���̺� ���� �÷� �ε��� ���, ��Ʈ�� ������ ������ ����ϴ� �ε����� ���� �����Ѵ�.
	u32Data			= 2;	// ���, ��� �̹Ƿ� 2
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4);	u32BuffPos += 4;
	// ��Ʈ���� ǥ���ϱ� ���ؼ� �߿��� �������� �÷� �ε��� ���� ���� �Ѵ�. �� ���� ������ ���� ��� ���� �߿��� ��������.
	u32Data			= 2;
	memcpy(u8BmpHead+u32BuffPos,	&u32Data,		4);	u32BuffPos += 4;
	/* ----------------------------------------------------- */
	/*                RGBQUAD (Black, White)                 */
	/* ----------------------------------------------------- */
	// ��� �ȷ�Ʈ - rgbBlue, rgbGreen, rgbRed, rgu8Reserved
	u8Data			= 0;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Reserved,	1);	u32BuffPos += 1;
	// ��� �ȷ�Ʈ - rgbBlue, rgbGreen, rgbRed, rgu8Reserved
	u8Data			= 0xff;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Data,		1);	u32BuffPos += 1;
	memcpy(u8BmpHead+u32BuffPos,	&u8Reserved,	1);	u32BuffPos += 1;

	// ���� ���� ���� ����
	if (0 == fopen_s(&fpWrite, csCnv.Uni2Ansi(file), "wb"))
	{
		// Data�� ����� ���� ���� Ȯ�� (���� �������� ��� (?))
		u32WriteSize	= WIDTHBYTES(1, width) * height;
		pWriteBuff		= new UINT8 [u32WriteSize+1];
		if (pWriteBuff)
		{
			memset(pWriteBuff, 0x00, u32WriteSize+1);	// �� �ʱ�ȭ

			// Bits Data�� ����� ���� ���� Ȯ�� (���� �������� ��� (?))
			ConvertBit2Byte(width, height, buff, pWriteBuff, u32WriteSize);
			// ��� ����
			fwrite(u8BmpHead, sizeof(UINT8), u32HeadSize, fpWrite);
			// ���� ����
			fwrite(pWriteBuff, sizeof(UINT8), u32WriteSize, fpWrite);

			// Data �޸� ����
			delete [] pWriteBuff;
		}
		// ���Ϸ� ����
		fclose(fpWrite);
	}
}

/*
 desc : DC �� �׷���(��µ�) �̹����� ��Ʈ�� ���Ϸ� ����
 parm : hdc		- [in]  DC Handle
		hbmp	- [in]  Bitmap Handle
		bpps	- [in]  Bit per Pixel
		width	- [in]  DC�� �׷��� �̹��� ũ��
		height	- [in]  DC�� �׷��� �̹��� ũ��
		file	- [in]  Bitmap ���� (��ü ��� ����)
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

	/* �޸� ���� */
	delete pBmpInfo;

	CloseHandle(hFile);

	DeleteObject(SelectObject(hDC, hObjSel));
	DeleteObject(hbmp);
	DeleteDC(hDC);
  
	return TRUE;
}
