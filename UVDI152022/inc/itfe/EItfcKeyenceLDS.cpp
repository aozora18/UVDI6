
/*
 desc : Engine Library (Shared Memory & Support Library)
*/

#include "pch.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


#ifdef __cplusplus
extern "C"
{
#endif
	/*
	 desc : 연결 여부 확인
	 parm : None
	 retn : TRUE or FALSE
	*/
	API_EXPORT BOOL uvEng_KeyenceLDS_IsConnected()
	{
		return uvKeyenceLDS_IsConnected();
	}

	/*
	 desc : LDS의 Zero 값을 현재 위치로 변경
	 parm : u8ModuleIdx	- [in]  LDS 모듈 번호
	 retn : TRUE or FALSE
	*/
	API_EXPORT BOOL uvEng_KeyenceLDS_ZeroShift(UINT8 u8ModuleIdx)
	{
		return uvKeyenceLDS_ZeroShift(u8ModuleIdx);
	}

	/*
	 desc : LDS의 Zero 값을 초기화
	 parm : u8ModuleIdx	- [in]  LDS 모듈 번호
	 retn : TRUE or FALSE
	*/
	API_EXPORT BOOL uvEng_KeyenceLDS_ResetZeroShift(UINT8 u8ModuleIdx)
	{
		return uvKeyenceLDS_ResetZeroShift(u8ModuleIdx);
	}

	/*
	 desc : LDS의 설정 값을 초기화
	 parm : u8ModuleIdx	- [in]  LDS 모듈 번호
	 retn : TRUE or FALSE
	*/
	API_EXPORT BOOL uvEng_KeyenceLDS_ResetDevice(UINT8 u8ModuleIdx)
	{
		return uvKeyenceLDS_ResetDevice(u8ModuleIdx);
	}

	/*
	 desc : LDS의 출력 값을 입력 값만큼 수정 (S/W Zero Set)(덧셈 연산)
	 parm : dValue	- [in]  입력 값
	 retn : None
	*/
	API_EXPORT VOID uvEng_KeyenceLDS_ZeroSet(double dValue)
	{
		uvKeyenceLDS_ZeroSet(dValue);
	}

	/*
	 desc : LDS의 출력 값의 소숫점 자리 설정
	 parm : dValue	- [in]  입력 값 (1, 0.1, 0.01, 0.001 ... )
	 retn : None
	*/
	API_EXPORT VOID uvEng_KeyenceLDS_SetDecimalPos(double dValue)
	{
		uvKeyenceLDS_SetDecimalPos(dValue);
	}

	/*
	 desc : LDS에 설정된 S/W Zero Set 값을 출력
	 parm : None
	 retn : 1, 0.1, 0.01, 0.001 ...
	*/
	API_EXPORT DOUBLE uvEng_KeyenceLDS_GetBaseValue()
	{
		return uvKeyenceLDS_GetBaseValue();
	}

	/*
	 desc : LDS 측정
	 parm : u8ModuleIdx	- [in]  LDS 모듈 번호
	 retn : LDS Value
	*/
	API_EXPORT DOUBLE uvEng_KeyenceLDS_Measure(UINT8 u8ModuleIdx)
	{
		return uvKeyenceLDS_Measure(u8ModuleIdx);
	}

#ifdef __cplusplus
}
#endif