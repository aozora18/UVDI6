
/*
 desc : EFEM Communication Libarary
*/

#include "../conf/global.h"
#include "../conf/efem.h"



#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : EFEM (Robot)과 통신 시작
 parm : config	- [in]  환경 설정 정보
		shmem	- [in]  EFEM 관련 공유 메모리
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEFEM_Init(LPG_CIEA config, LPG_EDSM shmem);
/*
 desc : EFEM (Robot)과 통신 시작
 parm : config	- [in]  환경 설정 정보
		shmem	- [in]  EFEM 관련 공유 메모리
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEFEM_Open(LPG_CIEA config, LPG_EDSM shmem);
/*
 desc : 라이브러리 해제
 parm : None
 retn : None
*/
API_IMPORT VOID uvEFEM_Close();
/*
 desc : 현재 PLC가 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEFEM_IsConnected();
/*
 desc : 현재 EFEM의 상태 값 반환
 parm : None
 retn : 0x00 - Ready, 0x01 - Busy, 0x02 - Error
*/
API_IMPORT ENG_ECSV uvEFEM_GetEfemState();
/*
 desc : 현재 EFEM의 리셋 진행률
 parm : None
 retn : 0 ~ 100%
*/
API_IMPORT UINT8 uvEFEM_GetEfemRate();
/*
 desc : 현재 EFEM내 Lot이 놓여진 위치 값 반환
 parm : None
 retn : 0x00 - None, 0x01 - Head, 0x02 - Aligner, 0x04 - Buffer
*/
API_IMPORT UINT8 uvEFEM_GetEfemWafer();
/*
 desc : 현재 EFEM or DI Loader에 Lot (Wafer)이 모두 비어 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEFEM_IsLotOnClearAll();
/*
 desc : 현재 EFEM의 Buffer에 Lot (Wafer)이 존재하는 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEFEM_IsLotOnBuffer();
/*
 desc : 현재 EFEM의 Aligner에 Lot (Wafer)이 존재하는 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEFEM_IsLotOnAligner();
/*
 desc : 현재 EFEM의 상태가 Busy인지 아닌지 확인
 parm : None
 retn : FALSE - Busy or Error, TRUE - Ready
*/
API_IMPORT BOOL uvEFEM_IsEfemReady();
/*
 desc : 현재 EFEM의 상태가 Error 상태인지 아닌지 확인
 parm : None
 retn : FALSE - Busy or Ready, TRUE - Error
*/
API_IMPORT BOOL uvEFEM_IsEfemError();
/*
 desc : 현재 EFEM의 에러 값 반환
 parm : None
 retn : Error Code 값
*/
API_IMPORT UINT16 uvEFEM_GetErrorCode();
/*
 desc : 현재 FOUP에 적재된 Lot (Wafer) 개수 반환
 parm : foup_num	- [in]  FOUP Number (1 or 2)
 retn : Lot Counts
*/
API_IMPORT UINT8 uvEFEM_GetLotCount(UINT8 foup_num);
/*
 desc : 노광 초기화 (기존 노광된 정보 모두 초기화)
 parm : None
 retn : None
*/
API_IMPORT VOID uvEFEM_ResetExpose();
/*
 desc : FOUP이 적재 (Loaded)되어 있는지 여부
 parm : foup	- [in]  FOUP Number (1 or 2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEFEM_IsFoupLoaded(ENG_EFIN foup);
/*
 desc : FOUP에 Lot (Wafer)가 적재 (Loaded)되어 있는지 여부
 parm : foup	- [in]  FOUP Number (1 or 2)
		lot		- [in]  FOUP의 Lot (Wafer) 위치 (1 ~ 25)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEFEM_IsFoupLotLoaded(ENG_EFIN foup, UINT8 lot);
/*
 desc : 현재 Lot (Wafer)이 장비 내부의 임의 위치에 적재되어 있는지 여부
 parm : find	- [in]  해당 위치에 Lot (Wafer)이 적재되어 있는지 확인하기 위한 검색어
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEFEM_IsLotLoaded(ENG_EWLI find);
/*
 desc : Reset Event Sync
 parm : None
 retn : None
*/
API_IMPORT VOID uvEFEM_ResetEventSync();
/*
 desc : 송신과 수신 이벤트가 서로 동기화 (궁합이)가 맞는지 여부 확인
 parm : None
 parm : TRUE - 맞다. FALSE - 틀리다. (이전 CMPS 이벤트와 현재 EFEM 이벤트가 서로 다르다)
*/
API_IMPORT BOOL uvEFEM_IsEventSync();
/*
 desc : EFEM으로부터 수신된 이벤트 코드 값 반환
 parm : None
 parm : ENG_EFEC 값 반환
*/
API_IMPORT UINT8 uvEFEM_GetEFEMEvent();
/*
 desc : 현재 EFEM Reset 상태 여부 반환
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEFEM_IsEFEMReset();
/*
 desc : 0 bytes (16 bits) 데이터 송신
 parm : cmd		- [in]  명령어
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEFEM_SendDataCmd(ENG_ECPC cmd);
/*
 desc : 1 bytes (8 bits) 데이터 송신
 parm : cmd		- [in]  명령어
		data	- [in]  1 bytes 데이터가 저장된 변수
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEFEM_SendData8(ENG_ECPC cmd, UINT8 data);
/*
 desc : 2 bytes (16 bits) 데이터 송신
 parm : cmd		- [in]  명령어
		data	- [in]  2 bytes 데이터가 저장된 변수
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEFEM_SendData16(ENG_ECPC cmd, UINT16 data);
/*
 desc : 4 bytes (32 bits) 데이터 송신
 parm : cmd		- [in]  명령어
		data	- [in]  4 bytes 데이터가 저장된 변수
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEFEM_SendData32(ENG_ECPC cmd, UINT32 data);
/*
 desc : n bytes (n*8 bits) 데이터 송신
 parm : cmd		- [in]  명령어
		data	- [in]  n bytes 데이터가 저장된 변수
		size	- [in]  'data' 버퍼에 저장된 데이터의 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEFEM_SendData(ENG_ECPC cmd, PUINT8 data, UINT16 size);

/*
 desc : Event
 parm : event	- [in]  Event Code (0x01 ~ 0x04)
		get		- [in]  EFEM의 FOUP에서 DI의 Loader에 Lot (Wafer)를 적재할 때, Lot (Wafer) 회전 여부 값 (0x00 : Normal, 0x1 : Invert)
		put		- [in]  Loader에서 EFEM의 FOUP에 Lot (Wafer)를 적재할 때, Lot (Wafer) 회전 여부 값 (0x00 : Normal, 0x01: Invert)
		foup	- [in]  FOUP Number (1 or 2) (FOUP 설치 개수가 2개인 경우)
		lot		- [in]  Lot (Wafer) Number (1 ~ 25)
 retn : 명령 전송 성공 여부 (TRUE or FALSE)
*/
API_IMPORT BOOL uvEFEM_ReqPktEvent(ENG_EFEC event, LPG_PLRD data);
/*
 desc : 노광 동작 여부 설정
 parm : enable	- [in]  TRUE - 노광 동작, FALSE - 노광 중지
 retn : None
*/
API_IMPORT VOID uvEFEM_SetExposeMode(BOOL enable);
/*
 desc : EFEM 상태 초기화
 parm : None
 retn : None
*/
API_IMPORT VOID uvEFEM_ResetState();
/*
 desc : EFEM State 상태가 주어진 시간 안에 갱신되었는지 확인
 parm : time	- [in]  이 시간 이내에 갱신되었는지 확인 시간 (단위: msec)
 retn : TRUE (주어진 시간보다 짧게 갱신되었다)
		FALSE (주어진 시간 이내에 갱신되지 않았다. 즉, 수신된 데이터가 아직 없다.)
*/
API_IMPORT BOOL uvEFEM_IsUpdateState(UINT64 time);
/*
 desc : FOUP Lot List 요청 패킷 송신
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEFEM_ReqFoupLotList();
/*
 desc : 가장 최근에 발생된 EFEM의 에러 코드 및 시간 정보 반환
 parm : None
 retn : 0x0000 or 에러 코드 반환 (0x0001 ~ 0xffff)
*/
API_IMPORT LPG_EATD uvEFEM_GetErrorTime();


#ifdef __cplusplus
}
#endif