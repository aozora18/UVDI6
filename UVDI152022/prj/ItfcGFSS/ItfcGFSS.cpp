
/*
 desc : Melsec Q PLC Communication Libarary (MC Protocol)
*/

#include "pch.h"
#include "MainApp.h"
#include "GFSSThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                           전역 변수                                           */
/* --------------------------------------------------------------------------------------------- */

UINT64						g_u64LastWriteTime	= 0;		/* 가장 최근에 송신한 시간 저장 */

CGFSSThread					*g_pGFSSThread		= NULL;	


/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */





/* --------------------------------------------------------------------------------------------- */
/*                                           외부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/*
 desc : PLC Server와 통신을 담당하는 Client 스레드 생성 및 실행
 parm : config	- [in]  환경 설정 정보가 저장된 구조체 포인터
		speed	- [in]  스레드 동작 속도 (단위: 밀리초)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSS_Init(LPG_CSID config, UINT32 speed)
{
	if (speed < 10)	speed = 10;
	// Client 스레드 생성 및 동작 시킴
	g_pGFSSThread = new CGFSSThread(0, config);
	if (!g_pGFSSThread)	return FALSE;
	if (!g_pGFSSThread->CreateThread(0, 0, speed))
	{
		delete g_pGFSSThread;
		g_pGFSSThread	= NULL;
		return FALSE;
	}

	return TRUE;
}

/*
 desc : 라이브러리 해제
 parm : None
 retn : None
*/
API_EXPORT VOID uvGFSS_Close()
{
	// 스레드 정지 시키는 명령 전달 후에 정지될 때까지 대기
	if (g_pGFSSThread)
	{
		// 서버와의 연결을 강제로 해제
		g_pGFSSThread->CloseClient();
		// 스레드 중지 호출
		g_pGFSSThread->Stop();
		while (g_pGFSSThread->IsBusy())	g_pGFSSThread->Sleep(100);
		/* 콜백 리셋 함수 호출 후 메모리 해제 */
		delete g_pGFSSThread;
		g_pGFSSThread	= NULL;
	}
}

/*
 desc : 현재 PLC가 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSS_IsConnected()
{
	if (!g_pGFSSThread)	return FALSE;
	if (!g_pGFSSThread)	return FALSE;
	if (!g_pGFSSThread->IsBusy())	return FALSE;

	return g_pGFSSThread->IsConnected();
}

/*
 desc : 현재 Melsec TCP/IP 통신 상태 모드 값 얻기
 parm : None
 retn : ENG_TCPS
*/
API_EXPORT ENG_TPCS uvGFSS_GetTCPMode()
{
	if (!g_pGFSSThread)	return ENG_TPCS::en_closed;
	if (!g_pGFSSThread->IsConnected())	return ENG_TPCS::en_closed;
	return g_pGFSSThread->GetTCPMode();
}

/*
 desc : 현재 명령어를 보내도 되는지 여부 즉, 가장 최근에 보낸 명령어와 현재 보낼 명령어 간의 시간 간격이
		주어진 시간보다 작다면, FALSE, 크다면 TRUE 반환
 parm : period	- [in]  간격 (이 시간보다 크면, 송신 가능, 작다면 송신 불가능) (단위: msec)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSS_IsSendTimeValid(UINT64 period)
{
	if (!g_pGFSSThread->IsConnected())	return FALSE;
	return (GetTickCount64() > (g_u64LastWriteTime + period));
}

/*
 desc : 가장 최근에 발생된 에러가 있는지 여부
 parm : None
 retn : TRUE or FALSE (에러 없음)
*/
API_EXPORT BOOL uvGFSS_IsLastError()
{
	if (!g_pGFSSThread)	return FALSE;
	if (!g_pGFSSThread->IsConnected())	return FALSE;

	return g_pGFSSThread->GetLastErrorLevel() == ENG_LNWE::en_error;
}

/*
 desc : 현재까지 거버 파일의 수신 진행률 값 반환
 parm : None
 retn : 진행률 값
*/
API_EXPORT DOUBLE uvGFSS_GetTotalFilesRate()
{
	if (!g_pGFSSThread)	return 0.0f;
	if (!g_pGFSSThread->IsConnected())	0.0f;

	return g_pGFSSThread->GetTotalFilesRate();
}

/*
 desc : Client로부터 수신된 패킷 헤더 명령어 반환
 parm : None
 retn : 현재 수신된 명령어들 중 가장 먼저 도착했던 명령어 반환
		수신된 명령어가 더 이상 없다면 0x0000 값 반환
*/
API_EXPORT ENG_PCGG uvGFSS_PopPktHeadCmd()
{
	STG_UTPH stHead	= {NULL};

	if (!g_pGFSSThread)	return ENG_PCGG::en_pcgg_unknown;
	if (!g_pGFSSThread->IsConnected())		return ENG_PCGG::en_pcgg_unknown;
	if (!g_pGFSSThread->PopPktHead(stHead))	return ENG_PCGG::en_pcgg_unknown;

	return ENG_PCGG(stHead.cmd);
}

/*
 desc : 총 수신받을 파일들의 개수 값 반환
 parm : None
 retn : 거버 파일 개수
*/
API_EXPORT UINT32 uvGFSSS_GetTotalFileCount()
{
	if (!g_pGFSSThread)	return 0;
	if (!g_pGFSSThread->IsConnected())	return 0;
	return g_pGFSSThread->GetTotalFileCount();
}

/*
 desc : 현재 수신받고 있는 파일 개수
 parm : None
 retn : 거버 파일 개수
*/
API_EXPORT UINT32 uvGFSSS_GetCurrentFileCount()
{
	if (!g_pGFSSThread)	return 0;
	if (!g_pGFSSThread->IsConnected())	return 0;
	return g_pGFSSThread->GetCurrentFileCount();
}

/*
 desc : 가장 최근에 Client로부터 수신받은 파일의 크기 반환 (단위: bytes)
 parm : None
 retn : 파일 들의 총 크기 값
*/
API_EXPORT UINT32 uvGFSS_GetTotalFileSize()
{
	if (!g_pGFSSThread)	return 0;
	if (!g_pGFSSThread->IsConnected())	return 0;
	return g_pGFSSThread->GetTotalFileSize();
}

/*
 desc : 가장 최근에 Client로부터 수신받은 파일의 크기 반환 (단위: bytes)
 parm : None
 retn : 파일 들의 총 크기 값
*/
API_EXPORT UINT32 uvGFSS_GetTotalFileZipSize()
{
	if (!g_pGFSSThread)	return 0;
	if (!g_pGFSSThread->IsConnected())	return 0;
	return g_pGFSSThread->GetTotalFileZipSize();
}

/*
 desc : 가장 최근에 생성 / 삭제 / 저장 폴더 반환
 parm : type	- [in]  0x00:생성, 0x01:삭제, 0x02:저장
 retn : 폴더 (전체 경로 미포함)
*/
API_EXPORT PTCHAR uvGFSS_GetLastDir(UINT8 type)
{
	if (!g_pGFSSThread)	return L"";
	if (!g_pGFSSThread->IsConnected())	return L"";
	return g_pGFSSThread->GetLastDir(type);
}

/*
 desc : 가장 최근에 수신받는데 소요된 시간 반환
 parm : None
 retn : 소요시간 반환 (단위: msec)
*/
API_EXPORT UINT64 uvGFSS_GetLastTotalTime()
{
	return g_pGFSSThread->GetRecvTotalTime();
}

/*
 desc : 모든 거버 파일이 수신되었는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSS_bIsRecvCompleted()
{
	return g_pGFSSThread->IsRecvCompleted();
}

#ifdef __cplusplus
}
#endif