
/*
 desc : Data Library
*/

#pragma once

/* User Header */
#include "../conf/global.h"
#include "../conf/enum.h"
#include "../conf/pgfs.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : 라이브러리 초기화
 parm : work_dir	- [in]  작업 기본 경로 (Full path)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvGFSP_Init(PTCHAR work_dir);
/*
 desc : 라이브러리 해제
 parm : None
 retn : None
*/
API_IMPORT VOID uvGFSP_Close();
/*
 desc : 환경 파일 저장
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvGFSP_Save();
/*
 desc : 환경 파일 재적재
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvGFSP_ReloadConfig();
/*
 desc : 무한 루프 시간 비교를 위한 시간 값 초기화 (현재 CPU Tick Count로 초기화)
 parm : None
 retn : None
*/
API_IMPORT VOID uvGFSP_ResetLoopTime();
/*
 desc : 주어진 시간 (Time Tick) 기준으로 무한 루프에 빠져 있는지 여부 확인
 parm : file	- [in]  호출된 Source File Name
		line	- [in]  호출된 Source Code의 Line Number
		limit	- [in]  	이 시간 (CPU Time Tick) 이후 만큼 대기 시간을 초과 했는지 검사하기 위한 Time Tick 값
 retn : TRUE or FALSE
*/
#if (USE_CHECK_LIMIT_LOOP_TIME)
API_IMPORT BOOL uvGFSP_IsLoopTimeOut(PTCHAR file, UINT32 line, UINT64 limit=3000);
#else
API_IMPORT BOOL uvGFSP_IsLoopTimeOut();
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                            Config                                             */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Init 설정 정보 반환
 parm : None
 retn : Init 설정 정보가 저장된 구조체 포인터 반환
*/
API_IMPORT LPG_CSID uvGFSP_Info_GetData();

#ifdef __cplusplus
}
#endif