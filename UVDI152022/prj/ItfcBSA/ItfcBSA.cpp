
/*
 desc : Vision Communication Libarary (Vision Protocol)
*/

#include "pch.h"
#include "MainApp.h"
#include "BSAThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                           전역 변수                                           */
/* --------------------------------------------------------------------------------------------- */

CBSAThread					*g_pBSAThread		= NULL;		// 데이터 수집 및 감시 스레드

/* 공유 메모리 */
LPG_RBVC					g_pstShMemVisi		= NULL;
STG_CTCS					g_stCommInfo		= {NULL};


/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 현재 Vision 장비와 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
BOOL IsConnected()
{
	if (!g_pBSAThread)	return FALSE;
	if (!g_pBSAThread->IsBusy())	return FALSE;

	if (!g_pBSAThread->IsConnected())
	{
		LOG_WARN(ENG_EDIC::en_bsa, L"Not connected to BSA S/W");
		return FALSE;
	}
	return TRUE;
}
/* --------------------------------------------------------------------------------------------- */
/*                                           외부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/*
 desc : PLC Server와 통신을 담당하는 Client 스레드 생성 및 실행
 parm : config	- [in]  환경 설정 정보
		shmem	- [in]  Vision 관련 공유 메모리
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_Init(LPG_CIEA config, LPG_RBVC shmem)
{
	/* Luria Shared Memory */
	g_pstShMemVisi	= shmem;
	/* All Configuration */
	g_pstConfig		= config;

	/* 구조체에 통신 관련 정보 설정 */
	g_stCommInfo.tcp_port	= g_pstConfig->set_comm.visi_port;
	g_stCommInfo.source_ipv4= uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.cmps_ipv4);
	g_stCommInfo.target_ipv4= uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.visi_ipv4);
	g_stCommInfo.port_buff	= g_pstConfig->set_comm.port_buff;
	g_stCommInfo.recv_buff	= g_pstConfig->set_comm.recv_buff;
	g_stCommInfo.ring_buff	= g_pstConfig->set_comm.ring_buff;
	g_stCommInfo.link_time	= g_pstConfig->set_comm.link_time;
	g_stCommInfo.idle_time	= g_pstConfig->set_comm.idle_time;
	g_stCommInfo.sock_time	= g_pstConfig->set_comm.sock_time;
	g_stCommInfo.live_time	= g_pstConfig->set_comm.live_time;

	/* Client 스레드 생성 및 동작 시킴 */
	g_pBSAThread = new CBSAThread(&g_stCommInfo, shmem);
	if (!g_pBSAThread)	return FALSE;
	ASSERT(g_pBSAThread);
	/* 스레드 동작 시킴 */
	if (!g_pBSAThread->CreateThread(0, NULL, MEDIUM_THREAD_SPEED))
	{
		delete g_pBSAThread;
		g_pBSAThread = NULL;
		return FALSE;
	}

	return TRUE;
}
/*
 desc : 라이브러리 해제
 parm : None
 retn : None
*/
API_EXPORT VOID uvBSA_Close()
{
	/* 스레드 정지 시키는 명령 전달 후에 정지될 때까지 대기 */
	if (g_pBSAThread)
	{
		g_pBSAThread->Stop();
		while (g_pBSAThread->IsBusy())	g_pBSAThread->Sleep(100);
		delete g_pBSAThread;
		g_pBSAThread = NULL;
	}
	/* 반드시 해줘야 됨 */
	g_pstConfig	= NULL;
}

/* --------------------------------------------------------------------------------------------- */
/*                                    Vision TCP/IP Interface                                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 수신된 데이터가 있는지 여부 반환
 parm : cmd	- [in]  명령어
 retn : TRUE or FALS
*/
API_EXPORT BOOL uvBSA_IsRecvData(ENG_VCPC cmd)
{
	return g_pBSAThread->IsRecvData(cmd);
}

/*
 desc : 현재 Vision 장비가 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_IsConnected()
{
	return IsConnected();
}

/*
 desc : State 수신 받은 시간 강제로 현재 시간으로 갱신
 parm : None
 retn : None
*/
API_EXPORT VOID uvBSA_SetUpdateStateTime()
{
	return g_pBSAThread->SetUpdateStateTime();
}

/*
 desc : State 값이 주어진 시간 내에 한 번이라도 갱신된 적인 있는지 여부
 parm : time	- [in]  이 시간 (msec) 이내에 갱신된적이 있는지 확인하기 위한 시간 (msec)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_IsUpdateStateTime(UINT64 time)
{
	return g_pBSAThread->IsUpdateStateTime(time);
}

/*
 desc : 현재 발생된 이벤트가 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_IsVisionNotiEvent()
{
	return g_pBSAThread->IsVisionNotiEvent();
}

/*
 desc : 현재 발생된 이벤트를 무효화 처리
 parm : None
 retn : None
*/
API_EXPORT VOID uvBSA_ResetVisionNotiEvent()
{
	g_pBSAThread->ResetVisionNotiEvent();
}

/*
 desc : 현재 발생된 이벤트 메시지 반환
 parm : None
 retn : 이벤트 메시지가 저장된 버퍼 포인터 반환
*/
API_EXPORT PTCHAR uvBSA_GetVisionNotiEventMsg()
{
	return g_pBSAThread->GetVisionNotiEventMsg();
}

/*
 desc : 현재 Vision 장비가 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_SendDataCmd(ENG_VCPC cmd, PUINT8 data, UINT16 size)
{
	/* Vision 장비와 통신이 끊겼을 경우 */
	if (!IsConnected())	return FALSE;
		
	return g_pBSAThread->SendData(cmd, data, size);
}

/*
desc : CMPS <-> Vision 간 Ready 상태 확인 요청
parm : None
retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_ReqVisnState()
{
	/* Vision 장비와 통신이 끊겼을 경우 */
	if (!IsConnected())	return FALSE; 

	return uvBSA_SendDataCmd(ENG_VCPC::en_vision_req_state, NULL, 0x0000);
}

/*
 desc : Vision Centering Command Data Send
 parm :	cam_no	- [in] 카메라 설정 (0x01: bsa , 0x02: up)
		unit	- [in] Pattern 정보 1 ~ 100 개 까지 Pattern Mark 정보 210715수정
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_ReqVisionCentering(UINT8 cam_no, UINT8 unit)
{
	if (!IsConnected())	return FALSE;
	return g_pBSAThread->SendCenteringData(ENG_VCPC::en_vision_req_centering, cam_no, unit);
}

/*
 desc : Vision Focus Command Data Send
 parm :	cam_no	- [in] 카메라 설정 (0x01: bsa , 0x02: up)
		unit	- [in] Pattern 정보 1 ~ 100 개 까지 Pattern Mark 정보 210715수정
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_ReqVisionFocus(UINT8 cam_no, UINT8 unit)
{
	if (!IsConnected())	return FALSE;
	return g_pBSAThread->SendFocusData(ENG_VCPC::en_vision_req_focus, cam_no, unit);
}

/*
 desc : Vision Focus Command Data Send
 parm :	point	- [in] 측정 순서 (0x01: 측정1, 0x02: 측정2)
		mark	- [in] Mark 번호 (0x01: mark1, 0x02: mark2)
		pattern	- [in] pattern 번호 
		posx	- [in] 현재 Stage X축 위치
		posy	- [in] 현재 Stage Y축 위치
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_ReqVisionGrab(UINT8 point, UINT8 mark, UINT8 pattern, INT32 posx, INT32 posy)
{
#if _SIMULATION_DEBUG_Start == 0
	if (!IsConnected())	return FALSE;
#endif

	return g_pBSAThread->SendGrabData(ENG_VCPC::en_vision_req_align_grab, point, mark, pattern, posx, posy);
}

/*
 desc : Vision View Command Data Send
 parm :	mode	- [in] 카메라 view Mode (0x01: auto view , 0x02: manual view )
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_ReqVisionView(UINT8 mode)
{
	if (!IsConnected())	return FALSE;
	return g_pBSAThread->SendViewData(ENG_VCPC::en_vision_req_view, mode);
}

/*
 desc : Vision Illuminatino Control Data Send
 parm :	mark	- [in] 측정 mark 번호
		onoff	- [in] 조명 제어 (0x00 : 0ff, 0x01 : on)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_ReqIllumination(UINT8 mark, UINT8 onoff)
{
	if (!IsConnected())	return FALSE;
	return g_pBSAThread->SendIllumination(ENG_VCPC::en_vision_req_illumination, mark, onoff);
}

/*
 desc : Vision Joblist Command Data Send
 parm :	job_mark_1	- [in] Job에 설정된 Mark1 Pattern List
		job_mark_2	- [in] Job에 설정된 Mark2 Pattern List
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvBSA_ReqVisionJobList(UINT64 job_mark_1, UINT64 job_mark_2)
{
	if (!IsConnected())	return FALSE;
	return g_pBSAThread->SendJoblist(ENG_VCPC::en_vision_req_job_pat_list, job_mark_1, job_mark_2);
}


#ifdef __cplusplus
}
#endif