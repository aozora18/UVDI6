
/*
 desc : Trigger Board Communication Libarary (Trig Protocol)
*/

#include "pch.h"
#include "MainApp.h"
#include "EFEMThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                           전역 변수                                           */
/* --------------------------------------------------------------------------------------------- */

CEFEMThread		*g_pEFEMThread	= NULL;		// 데이터 수집 및 감시 스레드
LPG_EDSM		g_pstShMemEFEM	= NULL;
LPG_CIEA		g_pstConfig		= NULL;
STG_CTCS		g_stCommInfo	= {NULL};


/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 현재 PLC가 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
BOOL IsConnected()
{
	if (!g_pEFEMThread)				return FALSE;
	if (!g_pEFEMThread->IsBusy())	return FALSE;

	return g_pEFEMThread->IsConnected();
}

/*
 desc : 리셋 이후 Foup Loaded 상태 요청
 parm : None
 retn : TRUE or FALSE
*/
BOOL ReqGetFoupLoaded()
{
	if (!IsConnected())	return FALSE;

	if (g_pstShMemEFEM->IsFoupLoaded(ENG_EFIN::en_1) || g_pstShMemEFEM->IsFoupLoaded(ENG_EFIN::en_2))
	{
		return g_pEFEMThread->SendData(ENG_ECPC::en_req_foup_loaded);
	}
	return TRUE;
}

/*
 desc : 특정 FOUP이 장착되었다는 시그널이 온 경우, 해당 FOUP의 Lot list 요청
 parm : foup	- [in]  FOUP ID
		loaded	- [in]  Load 여부 (0 or 1)
 retn : TRUE or FALSE
*/
BOOL ReqGetLotList(UINT8 foup, UINT8 loaded)
{
	if (!loaded)	return TRUE;

	return g_pEFEMThread->SendData(ENG_ECPC::en_req_lot_list, &foup, 1);
}

/* --------------------------------------------------------------------------------------------- */
/*                                           콜백 함수                                           */
/* --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- */
/*                                           외부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/*
 desc : EFEM (Robot)과 통신 시작
 parm : config	- [in]  환경 설정 정보
		shmem	- [in]  EFEM 관련 공유 메모리
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_Init(LPG_CIEA config, LPG_EDSM shmem)
{
	/* EFEM Shared Memory */
	g_pstShMemEFEM	= shmem;
	/* All Configuration */
	g_pstConfig		= config;

	/* 구조체에 통신 관련 정보 설정 */
	g_stCommInfo.tcp_port	= g_pstConfig->set_comm.efem_port;
	g_stCommInfo.source_ipv4= uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.cmps_ipv4);
	g_stCommInfo.target_ipv4= uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.efem_ipv4);
	g_stCommInfo.port_buff	= g_pstConfig->set_comm.port_buff;
	g_stCommInfo.recv_buff	= g_pstConfig->set_comm.recv_buff;
	g_stCommInfo.ring_buff	= g_pstConfig->set_comm.ring_buff;
	g_stCommInfo.link_time	= g_pstConfig->set_comm.link_time;
	g_stCommInfo.idle_time	= g_pstConfig->set_comm.idle_time;
	g_stCommInfo.sock_time	= g_pstConfig->set_comm.sock_time;
	g_stCommInfo.live_time	= g_pstConfig->set_comm.live_time;

	/* ------------------------------------------------- */
	/* !!! 중요 !!! 반드시 여기서 이 값을 설정해 줘야 됨 */
	/* ------------------------------------------------- */
	shmem->foup_lots	= config->efem_svc.foup_lots;
	/* ------------------------------------------------- */

	/* Client 스레드 생성 및 동작 시킴 */
	g_pEFEMThread	= new CEFEMThread(shmem, 0, &g_stCommInfo);
	if (!g_pEFEMThread)	return FALSE;
	if (!g_pEFEMThread->CreateThread(0, 0, MEDIUM_THREAD_SPEED))
	{
		delete g_pEFEMThread;
		g_pEFEMThread	= NULL;
		return FALSE;
	}

	return TRUE;
}

/*
 desc : EFEM (Robot)과 통신 시작
 parm : config	- [in]  환경 설정 정보
		shmem	- [in]  EFEM 관련 공유 메모리
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_Open(LPG_CIEA config, LPG_EDSM shmem)
{
	/* EFEM Shared Memory */
	g_pstShMemEFEM	= shmem;
	/* All Configuration */
	g_pstConfig		= config;

	return TRUE;
}

/*
 desc : 라이브러리 해제
 parm : None
 retn : None
*/
API_EXPORT VOID uvEFEM_Close()
{
	/* 스레드 정지 시키는 명령 전달 후에 정지될 때까지 대기 */
	if (g_pEFEMThread)
	{
		/* 스레드 중지 호출 */
		g_pEFEMThread->Stop();
		while (g_pEFEMThread->IsBusy())	g_pEFEMThread->Sleep(100);
		/* 콜백 리셋 함수 호출 후 메모리 해제 */
		delete g_pEFEMThread;
		g_pEFEMThread	= NULL;
	}
}

/*
 desc : 현재 PLC가 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_IsConnected()
{
	return IsConnected();
}

/*
 desc : 현재 EFEM의 상태 값 반환
 parm : None
 retn : 0x00 - Ready, 0x01 - Busy, 0x02 - Error
*/
API_EXPORT ENG_ECSV uvEFEM_GetEfemState()
{
	if (!IsConnected())	return ENG_ECSV::en_error;
	return ENG_ECSV(g_pstShMemEFEM->efem_state);
}

/*
 desc : 현재 EFEM의 리셋 진행률
 parm : None
 retn : 0 ~ 100%
*/
API_EXPORT UINT8 uvEFEM_GetEfemRate()
{
	if (!IsConnected())	return 0x00;
	return g_pstShMemEFEM->efem_rate;
}

/*
 desc : 현재 EFEM내 Lot이 놓여진 위치 값 반환
 parm : None
 retn : 0x00 - None, 0x01 - Head, 0x02 - Aligner, 0x04 - Buffer
*/
API_EXPORT UINT8 uvEFEM_GetEfemWafer()
{
	if (!IsConnected())	return 0x00;
	return g_pstShMemEFEM->efem_wafer;
}

/*
 desc : 현재 EFEM or DI Loader에 Lot (Wafer)이 모두 비어 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_IsLotOnClearAll()
{
	return (g_pstShMemEFEM->efem_wafer & 0x0f) == 0x00 ? TRUE : FALSE;
}

/*
 desc : 현재 EFEM의 Buffer에 Lot (Wafer)이 존재하는 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_IsLotOnBuffer()
{
	return (ENG_EWLI::en_wafer_in_buffer == ENG_EWLI(UINT8(ENG_EWLI::en_wafer_in_buffer) & g_pstShMemEFEM->efem_wafer));
}

/*
 desc : 현재 EFEM의 Aligner에 Lot (Wafer)이 존재하는 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_IsLotOnAligner()
{
	return (ENG_EWLI::en_wafer_in_aligner == ENG_EWLI(UINT8(ENG_EWLI::en_wafer_in_aligner) & g_pstShMemEFEM->efem_wafer));
}

/*
 desc : 현재 EFEM의 상태가 Ready 상태인지 아닌지 확인
 parm : None
 retn : FALSE - Busy or Error, TRUE - Ready
*/
API_EXPORT BOOL uvEFEM_IsEfemReady()
{
	return ENG_ECSV(g_pstShMemEFEM->efem_state) == ENG_ECSV::en_ready ? TRUE : FALSE;
}

/*
 desc : 현재 EFEM의 상태가 Error 상태인지 아닌지 확인
 parm : None
 retn : FALSE - Busy or Ready, TRUE - Error
*/
API_EXPORT BOOL uvEFEM_IsEfemError()
{
	if (!IsConnected())	return TRUE;
	return ENG_ECSV(g_pstShMemEFEM->efem_state) == ENG_ECSV::en_error ? TRUE : FALSE;
}

/*
 desc : 현재 EFEM의 에러 값 반환
 parm : None
 retn : Error Code 값
*/
API_EXPORT UINT16 uvEFEM_GetErrorCode()
{
	if (!IsConnected())	return 0x0000;
	return g_pstShMemEFEM->GetError();
}

/*
 desc : 현재 FOUP에 적재된 Lot (Wafer) 개수 반환
 parm : foup_num	- [in]  FOUP Number (1 or 2)
 retn : Lot Counts
*/
API_EXPORT UINT8 uvEFEM_GetLotCount(UINT8 foup_num)
{
	if (!IsConnected())	return 0x0000;
	return g_pstShMemEFEM->GetLotCount(foup_num);
}

/*
 desc : 노광 초기화 (기존 노광된 정보 모두 초기화)
 parm : None
 retn : None
*/
API_EXPORT VOID uvEFEM_ResetExpose()
{
	if (!IsConnected())	return;
	g_pstShMemEFEM->ResetExpose();
}

/*
 desc : FOUP이 적재 (Loaded)되어 있는지 여부
 parm : foup	- [in]  FOUP Number (1 or 2)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_IsFoupLoaded(ENG_EFIN foup)
{
	return g_pstShMemEFEM->IsFoupLoaded(foup);
}

/*
 desc : FOUP에 Lot (Wafer)가 적재 (Loaded)되어 있는지 여부
 parm : foup	- [in]  FOUP Number (1 or 2)
		lot		- [in]  FOUP의 Lot (Wafer) 위치 (1 ~ 25)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_IsFoupLotLoaded(ENG_EFIN foup, UINT8 lot)
{
	return g_pstShMemEFEM->IsLotLoaded(foup, lot);
}

/*
 desc : 현재 Lot (Wafer)이 장비 내부의 임의 위치에 적재되어 있는지 여부
 parm : find	- [in]  해당 위치에 Lot (Wafer)이 적재되어 있는지 확인하기 위한 검색어
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_IsLotLoaded(ENG_EWLI find)
{
	return g_pstShMemEFEM->IsLotLoaded(find);
}

/*
 desc : Reset Event Sync
 parm : None
 retn : None
*/
API_EXPORT VOID uvEFEM_ResetEventSync()
{
	g_pstShMemEFEM->ResetEventSync();
}

/*
 desc : 송신과 수신 이벤트가 서로 동기화 (궁합이)가 맞는지 여부 확인
 parm : None
 parm : TRUE - 맞다. FALSE - 틀리다. (이전 CMPS 이벤트와 현재 EFEM 이벤트가 서로 다르다)
*/
API_EXPORT BOOL uvEFEM_IsEventSync()
{
	if (!IsConnected())	return FALSE;

	return g_pstShMemEFEM->IsEventSync();
}

/*
 desc : EFEM으로부터 수신된 이벤트 코드 값 반환
 parm : None
 parm : ENG_EFEC 값 반환
*/
API_EXPORT UINT8 uvEFEM_GetEFEMEvent()
{
	if (!IsConnected())	return 0x00;

	return g_pstShMemEFEM->efem_event;
}

/*
 desc : 현재 EFEM Reset 상태 여부 반환
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_IsEFEMReset()
{
	if (!IsConnected())	return FALSE;
	return g_pstShMemEFEM->IsEFEMReset();
}

/*
 desc : 0 bytes (16 bits) 데이터 송신
 parm : cmd		- [in]  명령어
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_SendDataCmd(ENG_ECPC cmd)
{
	if (!IsConnected())	return FALSE;

	/* EFEM State와 상관 없이 리셋 명령은 무조건 송신 */
	if (cmd == ENG_ECPC::en_req_efem_reset)
	{
		/* EFEM Reset 플래그 초기화 */
		g_pstShMemEFEM->SetEFEMReset();
		return g_pEFEMThread->SendData(cmd);
	}

	/* 현재 EFEM의 상태가 Busy인지 여부 */
	if (ENG_ECSV::en_ready != ENG_ECSV(g_pstShMemEFEM->efem_state))
	{
		return FALSE;
	}
	return g_pEFEMThread->SendData(cmd);
}

/*
 desc : 1 bytes (8 bits) 데이터 송신
 parm : cmd		- [in]  명령어
		data	- [in]  1 bytes 데이터가 저장된 변수
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_SendData8(ENG_ECPC cmd, UINT8 data)
{
	if (!IsConnected())	return FALSE;
	/* 현재 EFEM의 상태가 Busy인지 여부 */
	if (ENG_ECSV::en_ready != ENG_ECSV(g_pstShMemEFEM->efem_state))
	{
		return FALSE;
	}
	return g_pEFEMThread->SendData(cmd, (PUINT8)&data, 1);
}

/*
 desc : 2 bytes (16 bits) 데이터 송신
 parm : cmd		- [in]  명령어
		data	- [in]  2 bytes 데이터가 저장된 변수
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_SendData16(ENG_ECPC cmd, UINT16 data)
{
	if (!IsConnected())	return FALSE;
	/* 현재 EFEM의 상태가 Busy인지 여부 */
	if (ENG_ECSV::en_ready != ENG_ECSV(g_pstShMemEFEM->efem_state))
	{
		return FALSE;
	}
	return g_pEFEMThread->SendData(cmd, (PUINT8)&data, 2);
}

/*
 desc : 4 bytes (32 bits) 데이터 송신
 parm : cmd		- [in]  명령어
		data	- [in]  4 bytes 데이터가 저장된 변수
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_SendData32(ENG_ECPC cmd, UINT32 data)
{
	if (!IsConnected())	return FALSE;
	/* 현재 EFEM의 상태가 Busy인지 여부 */
	if (ENG_ECSV::en_ready != ENG_ECSV(g_pstShMemEFEM->efem_state))
	{
		return FALSE;
	}
	return g_pEFEMThread->SendData(cmd, (PUINT8)&data, 4);
}

/*
 desc : n bytes (n*8 bits) 데이터 송신
 parm : cmd		- [in]  명령어
		data	- [in]  n bytes 데이터가 저장된 변수
		size	- [in]  'data' 버퍼에 저장된 데이터의 크기
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_SendData(ENG_ECPC cmd, PUINT8 data, UINT16 size)
{
	if (!IsConnected())	return FALSE;
	/* 현재 EFEM의 상태가 Busy인지 여부 */
	if (ENG_ECSV::en_ready != ENG_ECSV(g_pstShMemEFEM->efem_state))
	{
		return FALSE;
	}
	return g_pEFEMThread->SendData(cmd, data, size);
}

/*
 desc : Event 요청
 parm : event	- [in]  Event Code (0x01 ~ 0x04)
		data	- [in]  Recipe Data가 저장된 구조체 포인터
			get		:   EFEM의 FOUP에서 DI의 Loader에 Lot (Wafer)를 적재할 때, Lot (Wafer) 회전 여부 값 (0x00 : Normal, 0x1 : Invert)
			put		:   Loader에서 EFEM의 FOUP에 Lot (Wafer)를 적재할 때, Lot (Wafer) 회전 여부 값 (0x00 : Normal, 0x01: Invert)
			foup	:   FOUP Number (1 or 2) (FOUP 설치 개수가 2개인 경우)
			lot		:   Lot (Wafer) Number (1 ~ 25)
 retn : 명령 전송 성공 여부 (TRUE or FALSE)
*/
API_EXPORT BOOL uvEFEM_ReqPktEvent(ENG_EFEC event, LPG_PLRD data)
{
	UINT8 u8Data[8]	= { UINT8(event), data->get, data->put, data->foup, data->lot, };

	/* 현재 EFEM의 상태가 Busy인지 여부 */
	if (ENG_ECSV::en_ready != ENG_ECSV(g_pstShMemEFEM->efem_state))
	{
		return FALSE;
	}
	/* 기존 이벤트 코드 값을 송신하기 전에 초기화 */
	return !IsConnected() ? FALSE : g_pEFEMThread->SendData(ENG_ECPC::en_req_efem_event, u8Data, 5);
}

/*
 desc : 노광 동작 여부 설정
 parm : enable	- [in]  TRUE - 노광 동작, FALSE - 노광 중지
 retn : None
*/
API_EXPORT VOID uvEFEM_SetExposeMode(BOOL enable)
{
	if (IsConnected())	g_pEFEMThread->SetExposeMode(enable);
}

/*
 desc : EFEM 상태 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvEFEM_ResetState()
{
	if (IsConnected())	g_pstShMemEFEM->ResetState();
}

/*
 desc : EFEM State 상태가 주어진 시간 안에 갱신되었는지 확인
 parm : time	- [in]  이 시간 이내에 갱신되었는지 확인 시간 (단위: msec)
 retn : TRUE (주어진 시간보다 짧게 갱신되었다)
		FALSE (주어진 시간 이내에 갱신되지 않았다. 즉, 수신된 데이터가 아직 없다.)
*/
API_EXPORT BOOL uvEFEM_IsUpdateState(UINT64 time)
{
	if (!IsConnected())	return FALSE;

	return g_pstShMemEFEM->IsUpdateState(time);
}

/*
 desc : FOUP Lot List 요청 패킷 송신
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFEM_ReqFoupLotList()
{
	if (!IsConnected())	return FALSE;
	return g_pEFEMThread->ReqFoupLotList();
}

/*
 desc : 가장 최근에 발생된 EFEM의 에러 코드 및 시간 정보 반환
 parm : None
 retn : 0x0000 or 에러 코드 반환 (0x0001 ~ 0xffff)
*/
API_EXPORT LPG_EATD uvEFEM_GetErrorTime()
{
	if (!IsConnected())	return NULL;
	return g_pstShMemEFEM->GetErrorTime();
}


#ifdef __cplusplus
}
#endif