
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
API_EXPORT BOOL uvEng_Gentec_IsConnected()
{
	return uvGentec_IsConnected();
}

/*
 desc : Gentec Device Setup Information 반환
 parm : None
 retn : 구조체 포인터 반환
*/
API_EXPORT LPG_PSDV uvEng_Gentec_GetSetupInfo()
{
	return uvGentec_GetSetupInfo();
}

/*
 desc : Gentec에서 수집된 데이터가 꽉찬 상황인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Gentec_IsQueFulled()
{
	return uvGentec_IsQueFulled();
}

/*
 desc : 현재 수집된 버퍼 비율이 저장할 수 있는 큐의 버퍼 공간 대비 얼마만큼 저장되어 있는지 확인
 parm : rate	- [in]  Queue에 저장되어 있는 비율 값 (단위: 0.1 ~ 0.99)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Gentec_IsQueFullRate(FLOAT rate)
{
	return uvGentec_IsQueFullRate(rate);
}

/*
 desc : 임의 시간 동안 측정한 평균 값 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvEng_Gentec_ResetValue()
{
	uvGentec_ResetValue();
}

/*
 desc : Set whether to measure the illuminace from PM100D
 parm : flag	- [in]  FALSE - Stop, TRUE - Start
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Gentec_RunMeasure(BOOL flag)
{
	if (!uvGentec_RunMeasure(flag))
	{
		LOG_WARN(ENG_EDIC::en_gentec, L"Failed to start illuminance measurement");
		return FALSE;
	}
	return TRUE;
}

/*
 desc : Returns whether the PM100D is measuring illuminance
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Gentec_IsRunMeasure()
{
	return uvGentec_IsRunMeasure();
}

/*
 desc : 조도 데이터 결과 반환 (메모리에 저장된 값)
 parm : None
 retn : 데이터가 저장된 전역 포인터 반환
*/
API_EXPORT LPG_PGDV uvEng_Gentec_GetValue()
{
	if (GetConfig()->IsRunDemo())
	{
		if (!uvGentec_IsConnected())	return NULL;
	}
	if (!uvGentec_IsRunMeasure())
	{
		return NULL;
	}
	return uvGentec_GetRunPower();
}

/*
 desc : Get the current power (Average)
 parm : None
 retn : 0.0f (FALSE?) or Later.... (Power (Watt or dBm))
 note : It should not be in automatic measurement mode.
*/
API_EXPORT DOUBLE uvEng_Gentec_GetCurPower()
{
	if (GetConfig()->IsRunDemo())
	{
		if (!uvGentec_IsConnected())	return DOUBLE(uvCmn_GetRandNumerI32(0x00, 3)/1000.0f);
	}
	if (!uvGentec_IsRunMeasure())	return 0.0f;
	return uvGentec_GetRunPower()->average;
}

/*
 desc : Set Wave Length
 parm : wave	- [in]  Wave Length (or Line Filter) (단위: nm)
						ex> 365, 385, 395, 405 (nm)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Gentec_SetQueryWaveLength(INT16 wave)
{
	return uvGentec_SetQueryWaveLength(wave);
}

/*
 desc : Get Wave Length
 parm : None
 retn : 양수 값 (음수 값인 경우 실패) (365, 385, 390, 405 (nm))
*/
API_EXPORT INT16 uvEng_Gentec_GetQueryWaveLength()
{
	if (GetConfig()->IsRunDemo())
	{
		if (!uvGentec_IsConnected())	return uvCmn_GetRandNumerI32(0x00, 2);
	}
	return uvGentec_GetQueryWaveLength();
}

#ifdef __cplusplus
}
#endif