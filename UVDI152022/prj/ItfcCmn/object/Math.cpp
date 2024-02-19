
/*
 desc : Math.(Calculation) ���
*/

#include "pch.h"
#include "Math.h"

#include <random>

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
CMath::CMath()
{
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CMath::~CMath()
{
}

/*
 desc : ��Ʈ�� �̹����� ������ ��, ������ ����Ǵ� �������� ũ�� �� ��ȯ (����: Bytes)
 parm : cx, cy	- [in]  Block�� ���� / ���� (����: Pixel)
 retn : ũ�� �� ��ȯ (����: Bytes)
*/
UINT64 CMath::GetBitmapSizeOfBytes(UINT32 cx, UINT32 cy)
{
	return UINT64(WIDTHBYTES(1, UINT64(cx))) * UINT64(cy);
}

/*
 desc : �̹��� (1 bits ��Ʈ��)�� ũ�� (����: Bytes) �� ��ȯ
 parm : width	- [in]  �̹����� ���� (����: �ȼ�)
		height	- [in]  �̹����� ���� (����: �ȼ�)
 retn : ũ�� ��ȯ (����: Bytes)
*/
UINT32 CMath::GetBitmapImageBytes(UINT32 width, UINT32 height)
{
	return (((width + 31) / 32) * 4 * height);
}

/*
 desc : Check Sum �� ��� �� ���ڿ� Check Sum �� ��ȯ
 parm : data	- [in]  �۽ŵ� ��Ŷ�� ����� ����
		size	- [in]  'data' ���ۿ� ����� �������� ũ�� (Check Sum ũ�� �κ��� ���ܵ� ����)
 retn : check sum byte (256 modula)
*/
UINT8 CMath::GetCheckSum256(PUINT8 data, UINT32 size)
{
	UINT32 i	= 0, u32ChkSum = 0;
	/* 256 modula ��� ? */
	for (; i<size; i++)	u32ChkSum	+= data[i];
	/* Check Sum �� ��ȯ */
	return (UINT8)(u32ChkSum % 256);
}

/*
 desc : �� ��ǥ ������ �̿��� ȸ�� ���� ���ϱ�
 parm : x1, y1	- [in]  ù��° ������ X/Y ��ǥ
		x2, y2	- [in]  �ι�° ������ X/Y ��ǥ
 retn : �� ���� ���� �밢���� �����ϴ� ���� �� ���ϱ� (���� ���� �ƴ� ���� ����)
*/
DOUBLE CMath::GetCoordToAngle(INT64 x1, INT64 y1, INT64 x2, INT64 y2)
{
	DOUBLE dbDegree	= 0.0f;

	/* ���� ���ϱ� */
	dbDegree	= atan2f(FLOAT(y2 - y1), FLOAT(x2 - x1)) * 180.f / M_PI;
	/* ������ ���, ��� ���� ��ȯ�ϱ� ���� 360 ������ ���� ���� */
	if (dbDegree < 0)	return DOUBLE(360.0f - dbDegree);
	return dbDegree;
}

/*
 desc : Ư�� �߽� ��ǥ���� ���� ���� ��ŭ ȸ�� ���� ���, ȸ�� �� ��ǥ �� ��ȯ
 parm : cen_x,cen_y	- [in]  ȸ�� �߽� ��ǥ (����: �ȼ�)
		org_x,org_y	- [in]  ȸ�� ��� ��ǥ (����: �ȼ�)
		degree		- [in]  ȸ�� ���� (���������� ���� ������ ���� ��)
		rot_x,rot_y	- [out] ȸ�� ���� ��ǥ (����: �ȼ�)
 retn : None
*/
VOID CMath::RotateCoord(DOUBLE cen_x, DOUBLE cen_y, DOUBLE org_x, DOUBLE org_y,
						DOUBLE degree, DOUBLE &rot_x, DOUBLE &rot_y)
{
	/* ������ ���� ������ ���� */
	DOUBLE dbRad	= degree * M_PI / 180.0f;
	/* ȸ�� ���� ���� �� ��� */
	rot_x	= (org_x - cen_x) * cos(dbRad) - (org_y - cen_y) * sin(dbRad) + cen_x;
	rot_y	= (org_x - cen_x) * sin(dbRad) + (org_y - cen_y) * cos(dbRad) + cen_y;
}

/*
 desc : �簢���� �߽ɿ��� ���� ���� �������� ȸ�� �� ũ�� ��ȯ
 parm : degree	- [in]  ȸ�� ���� (���������� ���� ������ ���� ��)
		size	- [out] �簢���� ���� (�ȼ�) -> ȸ�� �� ����/���� �� ��ȯ
 retn : None
*/
VOID CMath::RotateRectSize(DOUBLE degree, CSize &size)
{
	/* ȸ�� �� �ӽ� ����� ��ǥ �� */
	DOUBLE dbRotX[4], dbRotY[4];
	/* ������ ���� ������ ���� */
	DOUBLE dbRad	= degree * M_PI / 180.0f;
	/* �߽� ��ǥ �� */
	DOUBLE dbCentX	= size.cx / 2.0f, dbCentY = size.cy / 2.0f;

	/* Left/Top ȸ�� �� ��ǥ �� */
	dbRotX[0]	= (0 - dbCentX) * cos(dbRad) - (0 - dbCentY) * sin(dbRad) + dbCentX;
	dbRotY[0]	= (0 - dbCentX) * sin(dbRad) + (0 - dbCentY) * cos(dbRad) + dbCentY;
	/* Right/Top ȸ�� �� ��ǥ �� */
	dbRotX[1]	= (size.cx - dbCentX) * cos(dbRad) - (0 - dbCentY) * sin(dbRad) + dbCentX;
	dbRotY[1]	= (size.cx - dbCentX) * sin(dbRad) + (0 - dbCentY) * cos(dbRad) + dbCentY;
	/* Right/Bottom ȸ�� �� ��ǥ �� */
	dbRotX[2]	= (size.cx - dbCentX) * cos(dbRad) - (size.cy - dbCentY) * sin(dbRad) + dbCentX;
	dbRotY[2]	= (size.cx - dbCentX) * sin(dbRad) + (size.cy - dbCentY) * cos(dbRad) + dbCentY;
	/* Left/Bottom ȸ�� �� ��ǥ �� */
	dbRotX[3]	= (0 - dbCentX) * cos(dbRad) - (size.cy - dbCentY) * sin(dbRad) + dbCentX;
	dbRotY[3]	= (0 - dbCentX) * sin(dbRad) + (size.cy - dbCentY) * cos(dbRad) + dbCentY;

	/* ���� ��ȯ */
	size.cx	= (UINT32)ROUNDUP((abs(dbRotX[2] - dbRotX[0])), 0);
	size.cy	= (UINT32)ROUNDUP((abs(dbRotY[3] - dbRotY[1])), 0);
}

/*
 desc : ���� ���� �������� ��ȯ
 parm : degree	- [in]  ���� ��
 retn : ���� �� ��ȯ
*/
DOUBLE CMath::GetDeg2Rad(DOUBLE degree)
{
	return degree * M_PI / 180.0f;
}


/*
 desc : ���� �ð� (�и���)�� ��/��/�� �� ���� �� ��ȯ
 parm : hour	- [out] �� ��ȯ
		minute	- [out] �� ��ȯ
		second	- [out] �� ��ȯ
 retn : None
*/
VOID CMath::GetTimeToHourMinSec(UINT32 &hour, UINT8 &minute, UINT8 &second)
{
	time_t tNow		= time(NULL);	/* ���� �ð��� �� ������ �����ؼ� t ������ ���� */

	hour	= (UINT8)((tNow / (1000 * 60 * 60)) % 24);
	minute	= (UINT8)((tNow / (1000 * 60)) % 60);
	second	= (UINT8)((tNow / 1000) % 60);
}

/*
 desc : �־��� �Է� �ð� (����: �и���)�� ���ǿ� ���� ��, ��, ��, �и��� �� �����Ͽ� ��ȣ��
 parm : time	- [in]  �Է� �ð� (����: �и���. msec. 1/1000 sec)
		type	- [in]  0x00:hour, 0x01:minute, 0x02:second, 0x03:milli-second
 retn : �ð� �� ��ȯ
*/
UINT8 CMath::GetTimeToType(UINT64 time, UINT8 type)
{
	switch (type)
	{
	case 0x00	:	return (UINT8)((time / (1000 * 60 * 60)) % 24);	/* Hour�� 24�� �Ѿ�� 1�� �Ѿ�Ƿ�... �ٽ� 0 ������ �������� ������ ���� 24�� ���� */
	case 0x01	:	return (UINT8)((time / (1000 * 60)) % 60);
	case 0x02	:	return (UINT8)((time / 1000) % 60);
	case 0x03	:	return (UINT8)(time % 1000);
	}
	return 0;
}

/*
 desc : Ư�� �Ҽ��� �ڸ��� ������ ���� ���� ������ ������
 parm : value	- [in]  ���� �Ǽ��� ��
		digit	- [in]  ���� ��µǴ� �Ҽ��� �ڸ���
 retn : ��� �� ��ȯ
*/
DOUBLE CMath::RoundTrunk(DOUBLE value, UINT32 digit)
{
	if (value == 0.0f)	return 0.0f;

	/* ����� ��� */
	if (value > 0)
	{
		return floor(value * pow(double(10), digit)) / pow(double(10), digit);
	}
	/* ������ ��� */
	else
	{
		return ceil(value * pow(double(10), digit)) / pow(double(10), digit);
	}
}

/*
 desc : �ٰ����� ���� �߽� ���ϱ�
 parm : poly	- [in]  �ٰ����� ��ǥ�� ����Ǿ� �ִ� ���� ����Ʈ
		cent	- [out] �߽� ��ǥ X/Y ��ȯ
 retn : TRUE or FALSE
*/
BOOL CMath::GetPolyCentXY(std::vector <STG_DBXY> poly, STG_DBXY &cent)
{
	UINT32 i = 2, u32Temp;	/* �ʱ� 2���� ���ؼ� �̸� �߽� ��ġ ���� */
	STG_DBXY stVect	= {NULL};
	LPG_DBXY pstTemp= NULL;

	/* �ּ������� 3�� �̻��� ��ǥ�� ����Ǿ� �־�� �� */
	if (poly.size() < 3)	return FALSE;
	/* �߽����� ã�ư��� �������� ������ �߽��� ��ġ�� �����ϱ� ���� ��ǥ ���� �޸� �Ҵ� */
	u32Temp	= UINT32(poly.size()-1);
	pstTemp	= (LPG_DBXY)::Alloc(sizeof(STG_DBXY) * u32Temp);
	memset(pstTemp, 0x00, sizeof(STG_DBXY) * u32Temp);
	/* �ʱ� 2���� �� ���̿��� �߽� ��ġ ���� */
	pstTemp[0].x	= (poly[0].x + poly[1].x) / 2.0f;
	pstTemp[0].y	= (poly[0].y + poly[1].y) / 2.0f;
	/* ������ ã�ư��� ���������� ��ǥ ������ ��� */
	while (i < poly.size())
	{
		/* �� ���� ���͸� ���ؼ� �� ������ 1 / n ���� ���ϱ� */
		stVect.x		= poly[i].x - pstTemp[i-2].x;
		stVect.y		= poly[i].y - pstTemp[i-2].y;
		pstTemp[i-1].x	= pstTemp[i-2].x + stVect.x / (DOUBLE(i) + 1);
		pstTemp[i-1].y	= pstTemp[i-2].y + stVect.y / (DOUBLE(i) + 1);
		i++;
	}
	/* ���� �������� ��ġ�� ���� Polygon�� �߽� ��ǥ �� */
	cent.x	= pstTemp[u32Temp-1].x;
	cent.y	= pstTemp[u32Temp-1].y;
	/* �ӽ� �޸� ���� */
	::Free(pstTemp);

	return TRUE;
}

/*
 desc : �ߺ����� �ʰ� ���� ���� ���
 parm : sign	- [in]  ��� or ���� �� ��� ���� (0x00 - �����, 0x01 - ���� ����)
		digit	- [in]  �����Ǵ� �������� �ڸ� �� (0x00 ~ 0x0a)
 retn : ������ ��ȯ
*/
INT32 CMath::GetRandNumerI32(UINT8 sign, UINT32 digit)
{
	UINT32 i	= 1, u32Digit	= 10;
	INT32 i32Min=0, i32Max = INT32_MAX, i32Val;

	if (digit < 1 || digit > 0x0a)	return 0;
	for (; i<digit; i++)	u32Digit *= 10;
	if (sign)	i32Min	= INT32_MIN;

	/* Random Seed */
	std::random_device csRand;
	/* Initialize Mersenne Twister pseudo - random number generator */
	std::mt19937_64 mtGen(csRand());

	/* Generate pseudo - random numbers - uniformly distributed in range (min, max) */
	uniform_int_distribution <INT32> csDis(i32Min, i32Max);

	/* Generate range_min ~ range_max random number */
#if 1
	i32Val	= csDis(mtGen);
	if (i32Val >= 0)	i32Val	= i32Val % u32Digit;
	else				i32Val	= (i32Val % u32Digit) * -1;
	return i32Val;
#else
	return (csDis(mtGen) % u32Digit);
#endif
}

/*
 desc : �ߺ����� �ʰ� ���� ���� ��� (Ư�� �������� ���� �����ϵ��� ����)
 parm : range_min	- [in]  ���� �߻��� �ּ� ��
		range_max	- [in]  ���� �߻��� �ִ� ��
 retn : ������ ��ȯ
*/
INT32 CMath::GetRandRangeNumerI32(INT32 range_min, INT32 range_max)
{
	/* Random Seed */
	random_device csRand;
	/* Initialize Mersenne Twister pseudo - random number generator */
	mt19937_64 mtGen(csRand());

	/* Generate pseudo - random numbers - uniformly distributed in range (min, max) */
	uniform_int_distribution <> csDis(range_min, range_max);

	/* Generate range_min ~ range_max random number */
	return csDis(mtGen);
}

/*
 desc : �ߺ����� �ʰ� ���� ���� ���
 parm : type	- [in]  0x00 - �빮��, 0x01 - �ҹ���
		buff	- [out] �����ڰ� ����� ����
		size	- [in]  'buff'�� ũ��, �ּ� ũ��� 2 �Դϴ�.
 retn : None
*/
VOID CMath::GetRandString(UINT8 type, PCHAR buff, UINT32 size)
{
	CHAR chStart	= 0x00 == type ? 65 : 97;
	UINT32 i	= 0;

	/* size ���� �ּ� 2 �̻� ���� ������ �˴ϴ�. */
	if (size < 2)	return;

	/* Buffer �ʱ�ȭ */
	memset(buff, 0x00, size);

	/* Random Seed */
	random_device csRand;
	/* Initialize Mersenne Twister pseudo - random number generator */
	mt19937_64 mtGen(csRand());

	/* Generate pseudo - random numbers - uniformly distributed in range (min, max) */
	uniform_int_distribution <> csDis(0, 25);

	for (; i<size-1; i++)
	{
		buff[i]	= (CHAR)csDis(mtGen)%26 + chStart;
	}
}
