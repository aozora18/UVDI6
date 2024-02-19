
/*
 desc : File Sync. API (Client)
*/

#pragma once

/* User Header */
#include "../conf/global.h"
#include "../conf/tcpip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : 통신을 담당하는 Client 스레드 생성 및 실행
 parm : config	- [in]  환경 설정 정보가 저장된 구조체 포인터
		speed	- [in]  스레드 동작 속도 (단위: 밀리초)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvGFSC_Init(LPG_CSID config, UINT32 speed=MEDIUM_THREAD_SPEED);
/*
 desc : 라이브러리 해제
 parm : None
 retn : None
*/
API_IMPORT VOID uvGFSC_Close();
/*
 desc : 현재 통신이 연결되어 있는지 여부 확인
 parm : None
 retn : DI_TRUE or DI_FALSE
*/
API_IMPORT BOOL uvGFSC_IsConnected();
/*
 desc : 현재 TCP/IP 통신 상태 모드 값 얻기
 parm : None
 retn : ENG_TCPS
*/
API_IMPORT ENG_TPCS uvGFSC_GetTCPMode();
/*
 desc : 현재 명령어를 보내도 되는지 여부 즉, 가장 최근에 보낸 명령어와 현재 보낼 명령어 간의 시간 간격이
		주어진 시간보다 작다면, FALSE, 크다면 TRUE 반환
 parm : period	- [in]  간격 (이 시간보다 크면, 송신 가능, 작다면 송신 불가능) (단위: msec)
 retn : TRUE or FALSE
*/
#ifdef _DEBUG
API_IMPORT BOOL uvGFSC_IsSendTimeValid(UINT64 period=2000);
#else
API_IMPORT BOOL uvGFSC_IsSendTimeValid(UINT64 period=1000);
#endif
/*
 desc : 새로 생성된 거버 폴더가 유효한지 여부 검사
 parm : g_path	- [in]  거버 폴더로서 유효한지 검사하기 위한 Gerber Path (전체 경로 포함)
		sub_path- [out] 하위 경로 존재 여부 값 반환 참조 변수
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvGFSC_IsValidGerberPath(PTCHAR g_path, BOOL &sub_path);
/*
 desc : 가장 최근에 발생된 에러가 있는지 여부
 parm : None
 retn : TRUE or FALSE (에러 없음)
*/
API_IMPORT BOOL uvGFSC_IsLastError();
/*
 desc : 현재까지 거버 파일의 송신 진행률 값 반환
 parm : None
 retn : 진행률 값
*/
API_IMPORT DOUBLE uvGFSC_GetSendGFilesRate();
/*
 desc : Client로부터 수신된 패킷 명령어 부분 반환
 parm : None
 retn : 현재 수신된 명령어들 중 가장 먼저 도착했던 명령어 반환
		수신된 명령어가 더 이상 없다면 0x0000 값 반환
*/
API_IMPORT ENG_PCGG uvGFSC_PopPktHeadCmd();
/*
 desc : 현재 거버 파일 송신 중이면 중지 처리
 parm : None
 retn : None
*/
API_IMPORT VOID uvGFSC_StopGerberSendFile();
/*
 desc : 현재까지 송신된 파일 크기 반환
 parm : None
 retn : 파일 크기 (단위: Bytes)
*/
API_IMPORT UINT32 uvGFSC_GetTotalFileSize();


/* --------------------------------------------------------------------------------------------- */
/*                                 Packet Data Communication Part                                */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 서버에 Alive Check 명령 전달
 parm : logs	- [in]  로그 파일 저장 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvGFSC_PktSendAliveTime();
/*
 desc : 서버에 Alive Check 수신 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvGFSC_IsRecvAliveTime();
/*
 desc : 거버 파일 경로 명 생성 명령 전달
 parm : g_name	- [in]  새로 생성될 거버 명 (전체 경로는 미포함. 기본 경로 하위에 생성될 거버 폴더 명)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvGFSC_PktSendNewGerberPath(PTCHAR g_name);
/*
 desc : 거버 파일 경로 명 삭제 명령 전달
 parm : g_name	- [in]  삭제될 거버 명 (전체 경로는 미포함. 기본 경로 하위의 삭제될 거버 폴더 명)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvGFSC_PktSendDelGerberPath(PTCHAR g_name);
/*
 desc : 거버 파일들을 Server에 전달
 parm : files	- [in]  파일 정보가 저장된 배열 리스트 버퍼 포인터
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvGFSC_PktSendGerberFiles(CStringArray *files);
/*
 desc : 서버에게 Recipe Files 요청
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvGFSC_PktSendGetRecipeFile();
/*
 desc : 서버에게 Recipe Files 전달
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvGFSC_PktSendSetRecipeFile();


#ifdef __cplusplus
}
#endif