
/*
 desc : Logs Library
*/

#pragma once

#include "../conf/global.h"
#include "../conf/enum.h"
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
#include "../conf/conf_podis.h"
#elif (CUSTOM_CODE_GEN2I == DELIVERY_PRODUCT_ID)
#include "../conf/conf_gen2i.h"
#endif


#ifdef __cplusplus
extern "C"
{
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                외부 함수 - Only Shared Memory                                 */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 라이브러리 초기화
 parm : path	- [in]  로그 저장 경로
		saved	- [in]  로그 저장 여부
		type	- [in]  Log Type (0 : text, 1 : DB (SQLite3)
		mode	- [in]  로그 저장 기준 (None, All, Warning and Error, Only Error)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLogs_Init(PTCHAR path, BOOL saved, UINT8 type=0x00,
							ENG_LNWE mode=ENG_LNWE::en_all);
/*
 desc : 라이브러리 해제
 parm : None
 retn : None
*/
API_IMPORT VOID uvLogs_Close();
/*
 desc : 로그 저장
 parm : apps	- [in]  Application Code (ENG_EDIC)
		type	- [in]  메시지 성격 (normal, warning, error)
		mesg	- [in]  기타 발생된 값이 저장된 버퍼
		file	- [in]  메시지가 발생된 소스 코드 파일명 (전체 경로가 포함된 파일 ... )
		line	- [in]  메시지가 발생된 소스 코드 위치 (Line Number)
 retn : None
*/
API_IMPORT VOID uvLogs_SaveLogs(ENG_EDIC apps, ENG_LNWE level, PTCHAR mesg, PTCHAR file, PTCHAR func, UINT32 line);
API_IMPORT VOID uvLogs_SaveLogsEx(ENG_LNWE level, PTCHAR mesg, PTCHAR file, PTCHAR func, UINT32 line);
/*
 desc : 가장 마지막에 발생된 로그 메시지 반환
 parm : None
 retn : Error message가 저장된 전역 버퍼 포인터
*/
API_IMPORT PTCHAR uvLogs_GetErrorMesgLast();
/*
 desc : 에러 혹은 경고 로그 존재 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLogs_IsErrorMesg();
/*
 desc : 마지막 에러 메시지 리셋
 parm : None
 retn : None
*/
API_IMPORT VOID uvLogs_ResetErrorMesg();
/*
 desc : 임의의 위치에 있는 에러 로그 메시지 반환
 parm : index	- [in]  특정 위치에 있는 에러 로그 반환 (Zero-based)
						0x00: 가장 최근에 발생된 에러 로그 요청
						[참고] 값이 커질수록 오래된 에러 로그 요청
 retn : 로그 메시지
		NULL이 반환된 경우, 더 이상 에러 로그 없음
*/
API_IMPORT PTCHAR uvLogs_GetErrorMesg(UINT8 index);
/*
 desc : 전체 경로가 포함된 로그 파일 목록 반환
 parm : files	- [out] 로그 파일 목록이 저장될 Array Buffer
 retn : None
*/
API_IMPORT VOID uvLogs_GetLogFiles(UINT8 type, CStringArray &files);


#ifdef __cplusplus
}
#endif