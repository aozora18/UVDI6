
/*
 desc : Melsec Q PLC Communication Libarary (MC Protocol)
*/

#pragma once

/* User Header */
#include "../conf/global.h"
#include "../conf/tcpip.h"
#include "../conf/mcdrv.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : PLC Server와 통신을 담당하는 Client 스레드 생성 및 실행
 parm : shmem	- [in]  PLC Shared Memory
		config	- [in]  PLC Setup Info.
		speed	- [in]  스레드 동작 속도 (단위: 밀리초)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMCQDrv_Init(LPG_CIEA config, LPG_PDSM shmem, UINT32 speed=NORMAL_THREAD_SPEED);
/*
 desc : PLC Server와 통신을 담당하는 Client 스레드 생성 및 실행
 parm : config	- [in]  환경 설정 정보
		shmem	- [in]  MC2 관련 공유 메모리
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMCQDrv_Open(LPG_CIEA config, LPG_PDSM shmem);
/*
 desc : 라이브러리 해제
 parm : None
 retn : None
*/
API_IMPORT VOID uvMCQDrv_Close();
/*
 desc : 현재 PLC가 연결되어 있는지 여부 확인
 parm : None
 retn : DI_TRUE or DI_FALSE
*/
API_IMPORT BOOL uvMCQDrv_IsConnected();
/*
 desc : Device Code의 숫자 값을 입력하면 문자열 값 반환
 parm : code	- [in]  PLC Device Code 값
 retn : 문자열 값 반환. 검색 실패하면 NULL 값 반환
*/
API_IMPORT CHAR *uvMCQDrv_GetDevCodeValToStr(UINT8 code);
/*
 desc : Device Code의 문자열 값을 입력하면 숫자 값 반환
 parm : code	- [in]  PLC Device Code 값
 retn : 숫자 값 반환. 검색 실패하면 0 값 반환
*/
API_IMPORT UINT8 uvMCQDrv_GetDevCodeStrToVal(CHAR *code);
/*
 desc : MC Protocol - Batch Read (Only Word)
 parm : code	- [in]  디바이스 코드와 매핑되는 Device Name (ENG_DNTC)
		addr	- [in]  디바이스 시작 주소 값 (4 Bytes이지만, 3 Bytes 값만 유효)
		count	- [in]  디바이스 개수 (읽어들일 점수)
 retn : DI_TRUE or DI_FALSE
*/
API_IMPORT BOOL uvMCQDrv_ReadWord(ENG_DNTC code, UINT32 addr, UINT16 count);
/*
 desc : MC Protocol - Word 단위로 데이터 쓰기
 parm : code	- [in]  디바이스 코드와 매핑되는 Device Code (ENG_DNTC)
		addr	- [in]  디바이스 시작 주소 값 (4 Bytes이지만, 3 Bytes 값만 유효)
		count	- [in]  디바이스 개수 (작성될 점수, 변경될 데이터의 개수)
		data	- [in]  워드 데이터가 저장되어 있는 메모리 포인터
 retn : DI_TRUE or DI_FALSE
*/
API_IMPORT BOOL uvMCQDrv_WriteWord(ENG_DNTC code, UINT32 addr, UINT16 count, PUINT16 data);
/*
 desc : MC Protocol - Bits (1 Bytes) 단위로 데이터 쓰기
 parm : code	- [in]  디바이스 코드와 매핑되는 Device Code (ENG_DNTC)
		addr	- [in]  디바이스 시작 주소 값 (4 Bytes이지만, 3 Bytes 값만 유효)
		count	- [in]  디바이스 개수 (작성될 점수, 즉 비트의 개수)
		data	- [in]  디바이스 개수 즉, count가 8이면, 'data' 메모리의 개수는 1 Bytes
						버퍼로서 1 bit의 정보가 8개 존재하는 메모리 포인터임
						count가 8개 이하인 경우는 나머지 비트가 의미없는 0 값으로 채워짐
 retn : DI_TRUE or DI_FALSE
*/
API_IMPORT BOOL uvMCQDrv_WriteBits(ENG_DNTC code, UINT32 addr, UINT16 count, PUINT8 data);
/*
 desc : 현재 Melsec TCP/IP 통신 상태 모드 값 얻기
 parm : None
 retn : ENG_TCPS
*/
API_IMPORT ENG_TPCS uvMCQDrv_GetTCPMode();
/*
 desc : 현재 명령어를 보내도 되는지 여부 즉, 가장 최근에 보낸 명령어와 현재 보낼 명령어 간의 시간 간격이
		주어진 시간보다 작다면, FALSE, 크다면 TRUE 반환
 parm : period	- [in]  간격 (이 시간보다 크면, 송신 가능, 작다면 송신 불가능) (단위: msec)
 retn : TRUE or FALSE
*/
#ifdef _DEBUG
API_IMPORT BOOL uvMCQDrv_IsSendTimeValid(UINT64 period=2000);
#else
API_IMPORT BOOL uvMCQDrv_IsSendTimeValid(UINT64 period=1000);
#endif


#ifdef __cplusplus
}
#endif