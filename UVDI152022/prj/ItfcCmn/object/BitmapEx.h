#pragma once

class CBitmapEx
{
/* Constructor and Destructor */
public:

	CBitmapEx();
	virtual ~CBitmapEx();

/* Virtual Function */
protected:

public:


/* Member Parameter : Local */
protected:



/* Member Function : Local */
protected:


/* Member Function : Public */
public:

	VOID				BmpDataUpDownChange(UINT32 width, UINT32 height, PUINT8 image);
	VOID				ConvertBit2Byte(UINT32 bit_width, UINT32 bit_height, PUINT8 bit_data,
										PUINT8 byte_data, UINT32 &byte_size);
	VOID				ConvertByte2Bit(UINT32 byte_width, UINT32 byte_height, PUINT8 byte_data,
										PUINT8 bit_data);
	VOID				ImageConvFlip(UINT32 cx, UINT32 cy, PUINT8 source, PUINT8 target);
	VOID				SaveBinaryToBMP(PUINT8 buff, UINT32 width, UINT32 height, PTCHAR file);
	VOID				SaveBitsToBMP(PUINT8 buff, UINT32 width, UINT32 height, PTCHAR file);
	BOOL				SaveDCToBitmap(HDC hdc, HBITMAP hbmp, UINT16 bpps, UINT32 width, UINT32 height, PTCHAR file);
};
