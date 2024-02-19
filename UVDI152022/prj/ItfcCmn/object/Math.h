#pragma once

class CMath
{
/* Constructor and Destructor */
public:

	CMath();
	virtual ~CMath();

/* Virtual Function */
protected:

public:


/* Member Parameter : Local */
protected:



/* Member Function : Local */
protected:


/* Member Function : Public */
public:

	UINT32				GetBitmapImageBytes(UINT32 width, UINT32 height);;
	UINT64				GetBitmapSizeOfBytes(UINT32 cx, UINT32 cy);
	UINT8				GetCheckSum256(PUINT8 data, UINT32 size);
	DOUBLE				GetCoordToAngle(INT64 x1, INT64 y1, INT64 x2, INT64 y2);
	DOUBLE				GetDeg2Rad(DOUBLE degree);
	UINT64				GetMultiOf8(UINT64 val)	{	return (val%8)==0 ? val : (val/8*8+8);	}
	BOOL				GetPolyCentXY(std::vector <STG_DBXY> poly, STG_DBXY &cent);
	DOUBLE				GetRad2Deg(DOUBLE radian)	{	return radian * 180.0f / M_PI; }
	INT32				GetRandNumerI32(UINT8 sign, UINT32 digit);
	INT32				GetRandRangeNumerI32(INT32 range_min, INT32 range_max);
	VOID				GetRandString(UINT8 type, PCHAR buff, UINT32 size);
	VOID				GetTimeToHourMinSec(UINT32 &hour, UINT8 &minute, UINT8 &second);
	UINT8				GetTimeToType(UINT64 time, UINT8 type);
	VOID				RotateCoord(DOUBLE cen_x, DOUBLE cen_y, DOUBLE org_x, DOUBLE org_y,
									DOUBLE degree, DOUBLE &rot_x, DOUBLE &rot_y);
	VOID				RotateRectSize(DOUBLE degree, CSize &size);
	DOUBLE				RoundTrunk(DOUBLE value, UINT32 digit);
};
