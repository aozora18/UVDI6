﻿
/*
 desc : PM100x Library
*/

// User Header
#include "../conf/global.h"
#include "../conf/pm100.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : Library Init
 parm : shmem	- [in]  Shared Memory
		type	- [in]  Measurement Type (0x01: Power, 0x02: Energy, 0x03: Frequency, 0x04: Density)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvPM100D_Init(LPG_PADV shmem, ENG_MDST type);
/*
 desc : Library Close
 parm : None
 retn : None
*/
API_IMPORT VOID uvPM100D_Close();
/*
 desc : 연결 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvPM100D_IsConnected();
/*
 desc : PM100D Device Setup Information 반환
 parm : None
 retn : 구조체 포인터 반환
*/
API_IMPORT LPG_PSDV uvPM100D_GetSetupInfo();
/*
 desc : PM100D에서 수집된 데이터가 꽉찬 상황인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvPM100D_IsQueFulled();
/*
 desc : 현재 수집된 버퍼 비율이 저장할 수 있는 큐의 버퍼 공간 대비 얼마만큼 저장되어 있는지 확인
 parm : rate	- [in]  Queue에 저장되어 있는 비율 값 (단위: 0.1 ~ 0.99)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvPM100D_IsQueFullRate(FLOAT rate);
/*
 desc : 임의 시간 동안 측정한 평균 값 초기화
 parm : None
 retn : None
*/
API_IMPORT VOID uvPM100D_ResetValue();
/*
 desc : PM100D 로부터 조도 데이터 측정 여부 설정
 parm : flag	- [in]  FALSE - Stop, TRUE - Start
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvPM100D_RunMeasure(BOOL flag);
/*
 desc : Returns whether the PM100D is measuring illuminance
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvPM100D_IsRunMeasure();
/*
 desc : 조도 데이터 결과 반환 (메모리에 저장된 값)
 parm : None
 retn : 데이터가 저장된 전역 포인터 반환
*/
API_IMPORT LPG_PGDV uvPM100D_GetRunPower();
/*
 desc : Set Wave Length
 parm : wave	- [in]  Wave Length (or Line Filter) (단위: nm)
						ex> 365, 385, 395, 405 (nm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvPM100D_SetQueryWaveLength(INT16 wave);
/*
 desc : Get Wave Length
 parm : None
 retn : 양수 값 (음수 값인 경우 실패) (365, 385, 390, 405 (nm))
*/
API_IMPORT INT16 uvPM100D_GetQueryWaveLength();


#ifdef __cplusplus
}
#endif