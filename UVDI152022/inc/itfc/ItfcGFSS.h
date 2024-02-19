
/*
 desc : desc : File Sync. API (Server)
*/

#pragma once

/* User Header */
#include "../../common/conf/global.h"
#include "../../common/conf/tcpip.h"
#include "../../common/conf/packet.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : PLC Server와 통신을 담당하는 Client 스레드 생성 및 실행
 parm : config	- [in]  환경 설정 정보가 저장된 구조체 포인터
		speed	- [in]  스레드 동작 속도 (단위: 밀리초)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvGFSS_Init(LPG_CSID config, UINT32 speed=MEDIUM_THREAD_SPEED);
/*
 desc : 라이브러리 해제
 parm : None
 retn : None
*/
API_IMPORT VOID uvGFSS_Close();
/*
 desc : 현재 PLC가 연결되어 있는지 여부 확인
 parm : None
 retn : DI_TRUE or DI_FALSE
*/
API_IMPORT BOOL uvGFSS_IsConnected();
/*
 desc : 현재 Melsec TCP/IP 통신 상태 모드 값 얻기
 parm : None
 retn : ENG_TCPS
*/
API_IMPORT ENG_TPCS uvGFSS_GetTCPMode();
/*
 desc : 현재 명령어를 보내도 되는지 여부 즉, 가장 최근에 보낸 명령어와 현재 보낼 명령어 간의 시간 간격이
		주어진 시간보다 작다면, FALSE, 크다면 TRUE 반환
 parm : period	- [in]  간격 (이 시간보다 크면, 송신 가능, 작다면 송신 불가능) (단위: msec)
 retn : TRUE or FALSE
*/
#ifdef _DEBUG
API_IMPORT BOOL uvGFSS_IsSendTimeValid(UINT64 period=2000);
#else
API_IMPORT BOOL uvGFSS_IsSendTimeValid(UINT64 period=1000);
#endif
/*
 desc : 가장 최근에 발생된 에러가 있는지 여부
 parm : None
 retn : TRUE or FALSE (에러 없음)
*/
API_IMPORT BOOL uvGFSS_IsLastError();
/*
 desc : 현재까지 거버 파일의 수신 진행률 값 반환
 parm : None
 retn : 진행률 값
*/
API_IMPORT DOUBLE uvGFSS_GetTotalFilesRate();
/*
 desc : Client로부터 수신된 패킷 헤더 명령어 반환
 parm : None
 retn : 현재 수신된 명령어들 중 가장 먼저 도착했던 명령어 반환
		수신된 명령어가 더 이상 없다면 0x0000 값 반환
*/
API_IMPORT ENG_PCGG uvGFSS_PopPktHeadCmd();
/*
 desc : 총 송신/수신받을 파일들의 개수 값 반환
 parm : None
 retn : 거버 파일 개수
*/
API_IMPORT UINT32 uvGFSSS_GetTotalFileCount();
/*
 desc : 현재 수신받고 있는 파일 개수
 parm : None
 retn : 거버 파일 개수
*/
API_IMPORT UINT32 uvGFSSS_GetCurrentFileCount();
/*
 desc : 가장 최근에 Client로부터 수신받은 파일의 크기 반환 (단위: bytes)
 parm : None
 retn : 파일 (기타 파일 포함)들의 총 크기 값
*/
API_IMPORT UINT32 uvGFSS_GetTotalFileSize();
/*
 desc : 가장 최근에 Client로부터 수신받은 파일의 크기 반환 (단위: bytes)
 parm : None
 retn : 파일 들의 총 크기 값
*/
API_IMPORT UINT32 uvGFSS_GetTotalFileZipSize();
/*
 desc : 가장 최근에 생성 / 삭제 / 저장 된 폴더 반환
 parm : type	- [in]  0x00:생성, 0x01:삭제, 0x02:저장
 retn : 폴더 (전체 경로 미포함)
*/
API_IMPORT PTCHAR uvGFSS_GetLastDir(UINT8 type);
/*
 desc : 가장 최근에 수신받는데 소요된 시간 반환
 parm : None
 retn : 소요시간 반환 (단위: msec)
*/
API_IMPORT UINT64 uvGFSS_GetLastTotalTime();
/*
 desc : 모든 거버 파일이 수신되었는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvGFSS_bIsRecvCompleted();



#ifdef __cplusplus
}
#endif