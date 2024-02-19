
/*
 desc : Math.(Calculation) 모듈
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
 desc : 비트맵 이미지를 저장할 때, 실제로 저장되는 데이터의 크기 값 반환 (단위: Bytes)
 parm : cx, cy	- [in]  Block의 넓이 / 높이 (단위: Pixel)
 retn : 크기 값 반환 (단위: Bytes)
*/
UINT64 CMath::GetBitmapSizeOfBytes(UINT32 cx, UINT32 cy)
{
	return UINT64(WIDTHBYTES(1, UINT64(cx))) * UINT64(cy);
}

/*
 desc : 이미지 (1 bits 비트맵)의 크기 (단위: Bytes) 값 반환
 parm : width	- [in]  이미지의 넓이 (단위: 픽셀)
		height	- [in]  이미지의 높이 (단위: 픽셀)
 retn : 크기 반환 (단위: Bytes)
*/
UINT32 CMath::GetBitmapImageBytes(UINT32 width, UINT32 height)
{
	return (((width + 31) / 32) * 4 * height);
}

/*
 desc : Check Sum 값 계산 및 문자열 Check Sum 값 반환
 parm : data	- [in]  송신될 패킷이 저장된 버퍼
		size	- [in]  'data' 버퍼에 저장된 데이터의 크기 (Check Sum 크기 부분이 제외된 길이)
 retn : check sum byte (256 modula)
*/
UINT8 CMath::GetCheckSum256(PUINT8 data, UINT32 size)
{
	UINT32 i	= 0, u32ChkSum = 0;
	/* 256 modula 방식 ? */
	for (; i<size; i++)	u32ChkSum	+= data[i];
	/* Check Sum 값 반환 */
	return (UINT8)(u32ChkSum % 256);
}

/*
 desc : 두 좌표 지점을 이용한 회전 각도 구하기
 parm : x1, y1	- [in]  첫번째 지점의 X/Y 좌표
		x2, y2	- [in]  두번째 지점의 X/Y 좌표
 retn : 두 지점 간의 대각선을 구성하는 각도 값 구하기 (라디안 값이 아닌 실제 각도)
*/
DOUBLE CMath::GetCoordToAngle(INT64 x1, INT64 y1, INT64 x2, INT64 y2)
{
	DOUBLE dbDegree	= 0.0f;

	/* 각도 구하기 */
	dbDegree	= atan2f(FLOAT(y2 - y1), FLOAT(x2 - x1)) * 180.f / M_PI;
	/* 음수인 경우, 양수 값을 반환하기 위해 360 도에서 값을 빼줌 */
	if (dbDegree < 0)	return DOUBLE(360.0f - dbDegree);
	return dbDegree;
}

/*
 desc : 특정 중심 좌표에서 임의 각도 만큼 회전 했을 경우, 회전 후 좌표 값 반환
 parm : cen_x,cen_y	- [in]  회전 중심 좌표 (단위: 픽셀)
		org_x,org_y	- [in]  회전 대상 좌표 (단위: 픽셀)
		degree		- [in]  회전 각도 (내부적으로 라디안 값으로 변경 됨)
		rot_x,rot_y	- [out] 회전 이후 좌표 (단위: 픽셀)
 retn : None
*/
VOID CMath::RotateCoord(DOUBLE cen_x, DOUBLE cen_y, DOUBLE org_x, DOUBLE org_y,
						DOUBLE degree, DOUBLE &rot_x, DOUBLE &rot_y)
{
	/* 각도를 라디안 값으로 변경 */
	DOUBLE dbRad	= degree * M_PI / 180.0f;
	/* 회전 이후 각도 값 계산 */
	rot_x	= (org_x - cen_x) * cos(dbRad) - (org_y - cen_y) * sin(dbRad) + cen_x;
	rot_y	= (org_x - cen_x) * sin(dbRad) + (org_y - cen_y) * cos(dbRad) + cen_y;
}

/*
 desc : 사각형의 중심에서 일정 각도 기준으로 회전 후 크기 반환
 parm : degree	- [in]  회전 각도 (내부적으로 라디안 값으로 변경 됨)
		size	- [out] 사각형의 넓이 (픽셀) -> 회전 후 넓이/높이 값 반환
 retn : None
*/
VOID CMath::RotateRectSize(DOUBLE degree, CSize &size)
{
	/* 회전 후 임시 저장될 좌표 값 */
	DOUBLE dbRotX[4], dbRotY[4];
	/* 각도를 라디안 값으로 변경 */
	DOUBLE dbRad	= degree * M_PI / 180.0f;
	/* 중심 좌표 값 */
	DOUBLE dbCentX	= size.cx / 2.0f, dbCentY = size.cy / 2.0f;

	/* Left/Top 회전 후 좌표 값 */
	dbRotX[0]	= (0 - dbCentX) * cos(dbRad) - (0 - dbCentY) * sin(dbRad) + dbCentX;
	dbRotY[0]	= (0 - dbCentX) * sin(dbRad) + (0 - dbCentY) * cos(dbRad) + dbCentY;
	/* Right/Top 회전 후 좌표 값 */
	dbRotX[1]	= (size.cx - dbCentX) * cos(dbRad) - (0 - dbCentY) * sin(dbRad) + dbCentX;
	dbRotY[1]	= (size.cx - dbCentX) * sin(dbRad) + (0 - dbCentY) * cos(dbRad) + dbCentY;
	/* Right/Bottom 회전 후 좌표 값 */
	dbRotX[2]	= (size.cx - dbCentX) * cos(dbRad) - (size.cy - dbCentY) * sin(dbRad) + dbCentX;
	dbRotY[2]	= (size.cx - dbCentX) * sin(dbRad) + (size.cy - dbCentY) * cos(dbRad) + dbCentY;
	/* Left/Bottom 회전 후 좌표 값 */
	dbRotX[3]	= (0 - dbCentX) * cos(dbRad) - (size.cy - dbCentY) * sin(dbRad) + dbCentX;
	dbRotY[3]	= (0 - dbCentX) * sin(dbRad) + (size.cy - dbCentY) * cos(dbRad) + dbCentY;

	/* 넓이 반환 */
	size.cx	= (UINT32)ROUNDUP((abs(dbRotX[2] - dbRotX[0])), 0);
	size.cy	= (UINT32)ROUNDUP((abs(dbRotY[3] - dbRotY[1])), 0);
}

/*
 desc : 각도 값을 라디안으로 변환
 parm : degree	- [in]  각도 값
 retn : 각도 값 반환
*/
DOUBLE CMath::GetDeg2Rad(DOUBLE degree)
{
	return degree * M_PI / 180.0f;
}


/*
 desc : 현재 시간 (밀리초)을 시/분/초 로 변경 후 반환
 parm : hour	- [out] 시 반환
		minute	- [out] 분 반환
		second	- [out] 초 반환
 retn : None
*/
VOID CMath::GetTimeToHourMinSec(UINT32 &hour, UINT8 &minute, UINT8 &second)
{
	time_t tNow		= time(NULL);	/* 현재 시간을 초 단위로 변경해서 t 변수에 저장 */

	hour	= (UINT8)((tNow / (1000 * 60 * 60)) % 24);
	minute	= (UINT8)((tNow / (1000 * 60)) % 60);
	second	= (UINT8)((tNow / 1000) % 60);
}

/*
 desc : 주어진 입력 시간 (단위: 밀리초)을 조건에 따라 시, 분, 초, 밀리초 로 구분하여 반호나
 parm : time	- [in]  입력 시간 (단위: 밀리초. msec. 1/1000 sec)
		type	- [in]  0x00:hour, 0x01:minute, 0x02:second, 0x03:milli-second
 retn : 시간 값 반환
*/
UINT8 CMath::GetTimeToType(UINT64 time, UINT8 type)
{
	switch (type)
	{
	case 0x00	:	return (UINT8)((time / (1000 * 60 * 60)) % 24);	/* Hour가 24시 넘어가면 1일 넘어가므로... 다시 0 값으로 나오도록 나머지 연산 24로 했음 */
	case 0x01	:	return (UINT8)((time / (1000 * 60)) % 60);
	case 0x02	:	return (UINT8)((time / 1000) % 60);
	case 0x03	:	return (UINT8)(time % 1000);
	}
	return 0;
}

/*
 desc : 특정 소수점 자릿수 까지만 남겨 놓고 무조건 버리기
 parm : value	- [in]  원래 실수형 값
		digit	- [in]  최종 출력되는 소숫점 자릿수
 retn : 결과 값 반환
*/
DOUBLE CMath::RoundTrunk(DOUBLE value, UINT32 digit)
{
	if (value == 0.0f)	return 0.0f;

	/* 양수인 경우 */
	if (value > 0)
	{
		return floor(value * pow(double(10), digit)) / pow(double(10), digit);
	}
	/* 음수인 경우 */
	else
	{
		return ceil(value * pow(double(10), digit)) / pow(double(10), digit);
	}
}

/*
 desc : 다각형의 무게 중심 구하기
 parm : poly	- [in]  다각형의 좌표가 저장되어 있는 벡터 리스트
		cent	- [out] 중심 좌표 X/Y 반환
 retn : TRUE or FALSE
*/
BOOL CMath::GetPolyCentXY(std::vector <STG_DBXY> poly, STG_DBXY &cent)
{
	UINT32 i = 2, u32Temp;	/* 초기 2개에 대해서 미리 중심 위치 구함 */
	STG_DBXY stVect	= {NULL};
	LPG_DBXY pstTemp= NULL;

	/* 최소한으로 3개 이상의 좌표가 저장되어 있어야 됨 */
	if (poly.size() < 3)	return FALSE;
	/* 중심점을 찾아가는 과정마다 마지막 중심점 위치를 저장하기 위한 좌표 정보 메모리 할당 */
	u32Temp	= UINT32(poly.size()-1);
	pstTemp	= (LPG_DBXY)::Alloc(sizeof(STG_DBXY) * u32Temp);
	memset(pstTemp, 0x00, sizeof(STG_DBXY) * u32Temp);
	/* 초기 2개의 점 사이에서 중심 위치 구함 */
	pstTemp[0].x	= (poly[0].x + poly[1].x) / 2.0f;
	pstTemp[0].y	= (poly[0].y + poly[1].y) / 2.0f;
	/* 중점을 찾아가는 과정에서의 좌표 값들을 얻기 */
	while (i < poly.size())
	{
		/* 두 점의 벡터를 구해서 그 벡터의 1 / n 지점 구하기 */
		stVect.x		= poly[i].x - pstTemp[i-2].x;
		stVect.y		= poly[i].y - pstTemp[i-2].y;
		pstTemp[i-1].x	= pstTemp[i-2].x + stVect.x / (DOUBLE(i) + 1);
		pstTemp[i-1].y	= pstTemp[i-2].y + stVect.y / (DOUBLE(i) + 1);
		i++;
	}
	/* 가장 마지막에 위치한 값이 Polygon의 중심 좌표 임 */
	cent.x	= pstTemp[u32Temp-1].x;
	cent.y	= pstTemp[u32Temp-1].y;
	/* 임시 메모리 해제 */
	::Free(pstTemp);

	return TRUE;
}

/*
 desc : 중복되지 않게 랜덤 숫자 출력
 parm : sign	- [in]  양수 or 음수 값 출력 여부 (0x00 - 양수만, 0x01 - 음수 포함)
		digit	- [in]  생성되는 데이터의 자릿 수 (0x00 ~ 0x0a)
 retn : 난수값 반환
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
 desc : 중복되지 않게 랜덤 숫자 출력 (특정 범위까지 생성 가능하도록 설정)
 parm : range_min	- [in]  랜덤 발생될 최소 값
		range_max	- [in]  랜덤 발생될 최대 값
 retn : 난수값 반환
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
 desc : 중복되지 않게 랜덤 문자 출력
 parm : type	- [in]  0x00 - 대문자, 0x01 - 소문자
		buff	- [out] 난문자가 저장될 버퍼
		size	- [in]  'buff'의 크기, 최소 크기는 2 입니다.
 retn : None
*/
VOID CMath::GetRandString(UINT8 type, PCHAR buff, UINT32 size)
{
	CHAR chStart	= 0x00 == type ? 65 : 97;
	UINT32 i	= 0;

	/* size 값은 최소 2 이상 값을 가져야 됩니다. */
	if (size < 2)	return;

	/* Buffer 초기화 */
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
