
/*
 desc : MC2 Client Library
*/

#include "pch.h"
#include "MainApp.h"

/* Socket Objects */
#include "MC2CThread.h"


/* Align Camera 2D Calibration 사용 버전 */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                           전역 변수                                           */
/* --------------------------------------------------------------------------------------------- */

UINT64						g_u64SendFailTime	= GetTickCount64();	/* 패킷 송신 실패를 부모에게 알린 시간 저장 */
LPG_MDSM					g_pstShMemMC2		= NULL;

/* Socket Objects */
CMC2CThread					*g_pMC2CThread		= NULL;


LPG_MDSM					g_pstShMemMC2b = NULL;
CMC2CThread					*g_pMC2bCThread = NULL;


/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 현재 송신 가능한 상태인지 확인
 parm : None
 retn : TRUE or FALSE
*/
BOOL IsSendData()
{
	if (!g_pMC2CThread)	return FALSE;
	if (!g_pMC2CThread->IsConnected())
	{
		LOG_WARN(ENG_EDIC::en_mc2, L"It was not connected to the remote system");
		return FALSE;
	}

#if (MC2_DRIVE_2SET == 1)
	if (!g_pMC2bCThread)	return FALSE;
	if (!g_pMC2bCThread->IsConnected())
	{
		LOG_WARN(ENG_EDIC::en_mc2, L"It was not connected to the remote system");
		return FALSE;
	}
#endif



	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                           외부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/*
 desc : MC2와의 통신 시작
 parm : config	- [in]  환경 설정 정보
		shmem	- [in]  MC2 관련 공유 메모리
 retn : TRUE or FALSE
*/
#if (MC2_DRIVE_2SET == 0)
API_EXPORT BOOL uvMC2_Init(LPG_CIEA config, LPG_MDSM shmem)
{
	/* Luria Shared Memory */
	g_pstShMemMC2	= shmem;
	/* All Configuration */
	g_pstConfig		= config;

	/* MC2 Object Thread */
	g_pMC2CThread	= new CMC2CThread(&g_pstConfig->mc2_svc, g_pstShMemMC2,
									  g_pstConfig->set_comm.mc2s_port,	/* MC2 Port. 송신 전용 */
									  g_pstConfig->set_comm.mc2c_port,	/* CMPS Port. 수신 전용 */
									  uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.cmps_ipv4),	/* Local IPv4 */
									  uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.mc2_ipv4),		/* MC2 IPv4 */
									  g_pstConfig->mc2_svc.mc2_serial,
									  g_pstConfig->set_comm.port_buff,
									  g_pstConfig->set_comm.recv_buff,
									  g_pstConfig->set_comm.ring_buff,
									  g_pstConfig->set_comm.idle_time,
									  g_pstConfig->set_comm.sock_time);
	ASSERT(g_pMC2CThread);
	/* 스레드 동작 시킴 */
	return g_pMC2CThread->CreateThread(0, NULL, MEDIUM_THREAD_SPEED);
}
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15|| \
DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
API_EXPORT BOOL uvMC2_Init(LPG_CIEA config, LPG_MDSM shmem, LPG_MDSM shmem2)
{
	/* Luria Shared Memory */
	g_pstShMemMC2	= shmem;
	g_pstShMemMC2b	= shmem2;
	/* All Configuration */
	g_pstConfig		= config;

	/* MC2 Object Thread */
	g_pMC2CThread	= new CMC2CThread(&g_pstConfig->mc2_svc, g_pstShMemMC2,
									  g_pstConfig->set_comm.mc2s_port,	/* MC2 Port. 송신 전용 */
									  g_pstConfig->set_comm.mc2c_port,	/* CMPS Port. 수신 전용 */
									  uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.cmps_ipv4),	/* Local IPv4 */
									  uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.mc2_ipv4),		/* MC2 IPv4 */
									  g_pstConfig->mc2_svc.mc2_serial,
									  g_pstConfig->set_comm.port_buff,
									  g_pstConfig->set_comm.recv_buff,
									  g_pstConfig->set_comm.ring_buff,
									  g_pstConfig->set_comm.idle_time,
									  g_pstConfig->set_comm.sock_time);
	ASSERT(g_pMC2CThread);

	/* MC2 Object Thread */
	g_pMC2bCThread	= new CMC2CThread(&g_pstConfig->mc2b_svc, g_pstShMemMC2b,
		g_pstConfig->set_comm.mc2bs_port,	/* MC2 Port. 송신 전용 */
		g_pstConfig->set_comm.mc2bc_port,	/* CMPS Port. 수신 전용 */
									  uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.cmps_ipv4),		/* Local IPv4 */
									  uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.mc2b_ipv4),		/* MC2 IPv4 */
									  g_pstConfig->mc2b_svc.mc2_serial,
									  g_pstConfig->set_comm.port_buff,
									  g_pstConfig->set_comm.recv_buff,
									  g_pstConfig->set_comm.ring_buff,
									  g_pstConfig->set_comm.idle_time,
									  g_pstConfig->set_comm.sock_time);
	ASSERT(g_pMC2bCThread);
	/* 스레드 동작 시킴 */
	return g_pMC2CThread->CreateThread(0, NULL, MEDIUM_THREAD_SPEED)  && g_pMC2bCThread->CreateThread(0, NULL, MEDIUM_THREAD_SPEED);
}
#endif


API_EXPORT BOOL uvMC2a_Init(LPG_CIEA config, LPG_MDSM shmem)
{
	/* Luria Shared Memory */
	g_pstShMemMC2	= shmem;
	/* All Configuration */
	g_pstConfig		= config;

	/* MC2 Object Thread */
	g_pMC2CThread	= new CMC2CThread(&g_pstConfig->mc2_svc, g_pstShMemMC2,
									  g_pstConfig->set_comm.mc2s_port,	/* MC2 Port. 송신 전용 */
									  g_pstConfig->set_comm.mc2c_port,	/* CMPS Port. 수신 전용 */
									  uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.cmps_ipv4),	/* Local IPv4 */
									  uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.mc2_ipv4),		/* MC2 IPv4 */
									  g_pstConfig->mc2_svc.mc2_serial,
									  g_pstConfig->set_comm.port_buff,
									  g_pstConfig->set_comm.recv_buff,
									  g_pstConfig->set_comm.ring_buff,
									  g_pstConfig->set_comm.idle_time,
									  g_pstConfig->set_comm.sock_time);
	ASSERT(g_pMC2CThread);
	/* 스레드 동작 시킴 */
	return g_pMC2CThread->CreateThread(0, NULL, MEDIUM_THREAD_SPEED);
}

API_EXPORT BOOL uvMC2b_Init(LPG_CIEA config, LPG_MDSM shmem)
{
	/* Luria Shared Memory */
	g_pstShMemMC2	= shmem;

	/* All Configuration */
	g_pstConfig		= config;


	/* MC2 Object Thread */
	g_pMC2bCThread	= new CMC2CThread(&g_pstConfig->mc2b_svc, g_pstShMemMC2,
		g_pstConfig->set_comm.mc2bs_port,	/* MC2 Port. 송신 전용 */
		g_pstConfig->set_comm.mc2bc_port,	/* CMPS Port. 수신 전용 */
									  uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.cmps_ipv4),		/* Local IPv4 */
									  uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.mc2b_ipv4),		/* MC2 IPv4 */
									  g_pstConfig->mc2b_svc.mc2_serial,
									  g_pstConfig->set_comm.port_buff,
									  g_pstConfig->set_comm.recv_buff,
									  g_pstConfig->set_comm.ring_buff,
									  g_pstConfig->set_comm.idle_time,
									  g_pstConfig->set_comm.sock_time);
	ASSERT(g_pMC2bCThread);
	/* 스레드 동작 시킴 */
	return g_pMC2bCThread->CreateThread(0, NULL, MEDIUM_THREAD_SPEED);
}


/*
 desc : MC2와의 통신 정지
 parm : None
 retn : None
*/
API_EXPORT VOID uvMC2_Close()
{
	/* Socket Objects */
	if (g_pMC2CThread)
	{
		g_pMC2CThread->Stop();
		while (g_pMC2CThread->IsBusy())	g_pMC2CThread->Sleep(300);
		/* 콜백 리셋 함수 호출 후 메모리 해제 */
		delete g_pMC2CThread;
		g_pMC2CThread	= NULL;
	}
#if (MC2_DRIVE_2SET == 1)
	/* Socket Objects */
	if (g_pMC2bCThread)
	{
		g_pMC2bCThread->Stop();
		while (g_pMC2bCThread->IsBusy())	g_pMC2bCThread->Sleep(300);
		/* 콜백 리셋 함수 호출 후 메모리 해제 */
		delete g_pMC2bCThread;
		g_pMC2bCThread = NULL;
	}
#endif
	/* 반드시 해줘야 됨 */
	g_pstConfig	= NULL;
}

/*
 desc : 기존 시스템 접속 해제 후 재접속 진행
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMC2_Reconnected()
{
	if (g_pMC2CThread)
	{
		g_pMC2CThread->Stop();
		while (g_pMC2CThread->IsBusy())	g_pMC2CThread->Sleep(300);
		/* 콜백 리셋 함수 호출 후 메모리 해제 */
		delete g_pMC2CThread;
		g_pMC2CThread = NULL;
	}



	/* Socket Objects */
	if (g_pMC2CThread)
	{
		g_pMC2CThread->Stop();
		while (g_pMC2CThread->IsBusy())	g_pMC2CThread->Sleep(300);
		/* 콜백 리셋 함수 호출 후 메모리 해제 */
		delete g_pMC2CThread;
		g_pMC2CThread	= NULL;
	}
	/* MC2 Object Thread */
	g_pMC2CThread	= new CMC2CThread(&g_pstConfig->mc2_svc, g_pstShMemMC2,
									  g_pstConfig->set_comm.mc2s_port,	/* MC2 Port. 송신 전용 */
									  g_pstConfig->set_comm.mc2c_port,	/* CMPS Port. 수신 전용 */
									  uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.cmps_ipv4),	/* Local IPv4 */
									  uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.mc2_ipv4),		/* MC2 IPv4 */
									  g_pstConfig->mc2_svc.mc2_serial,
									  g_pstConfig->set_comm.port_buff,
									  g_pstConfig->set_comm.recv_buff,
									  g_pstConfig->set_comm.ring_buff,
									  g_pstConfig->set_comm.idle_time,
									  g_pstConfig->set_comm.sock_time);
	ASSERT(g_pMC2CThread);

#if(MC2_DRIVE_2SET == 0)
	
	/* 스레드 동작 시킴 */
	return g_pMC2CThread->CreateThread(0, NULL, MEDIUM_THREAD_SPEED);

#elif(MC2_DRIVE_2SET == 1)
	/* Socket Objects */
	if (g_pMC2bCThread)
	{
		g_pMC2bCThread->Stop();
		while (g_pMC2bCThread->IsBusy())	g_pMC2bCThread->Sleep(300);
		/* 콜백 리셋 함수 호출 후 메모리 해제 */
		delete g_pMC2bCThread;
		g_pMC2bCThread = NULL;
	}
	/* MC2 Object Thread */
	g_pMC2bCThread	= new CMC2CThread(&g_pstConfig->mc2b_svc, g_pstShMemMC2b,
									  g_pstConfig->set_comm.mc2s_port,	/* MC2 Port. 송신 전용 */
									  g_pstConfig->set_comm.mc2c_port,	/* CMPS Port. 수신 전용 */
									  uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.cmps_ipv4),	/* Local IPv4 */
									  uvCmn_GetIPv4BytesToUint32(g_pstConfig->set_comm.mc2b_ipv4),		/* MC2 IPv4 */
									  g_pstConfig->mc2b_svc.mc2_serial,
									  g_pstConfig->set_comm.port_buff,
									  g_pstConfig->set_comm.recv_buff,
									  g_pstConfig->set_comm.ring_buff,
									  g_pstConfig->set_comm.idle_time,
									  g_pstConfig->set_comm.sock_time);
	ASSERT(g_pMC2bCThread);
	/* 스레드 동작 시킴 */
	return g_pMC2CThread->CreateThread(0, NULL, MEDIUM_THREAD_SPEED)  && g_pMC2bCThread->CreateThread(0, NULL, MEDIUM_THREAD_SPEED);
#endif

}

/*
 desc : MC2 Server에 연결 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMC2_IsConnected()
{
	return IsSendData();
}

/*
 desc : 가장 최근에 발생된 소켓 에러 코드 값 반환
 parm : None
 retn : Socket Error Code (MSDN 참조)
*/
API_EXPORT UINT32 uvMC2_GetSockLastError()
{
	return !g_pMC2CThread ? 0 : g_pMC2CThread->GetSockLastError();
}

/* --------------------------------------------------------------------------------------------- */
/*                                  MC2 Communication Functions                                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 모터 속도 제어
 parm : drv_id	- [in]  Axis 정보 (0, 1, 4, 5)
		velo	- [in]  이동 속도 (단위: mm/sec)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMC2_SendDevGoVelo(ENG_MMDI drv_id, UINT32 velo)
{
	if (velo < 1)	return FALSE;
#if (MC2_DRIVE_2SET == 0)
	return !IsSendData() ? FALSE : g_pMC2CThread->WriteDevGoVelo(drv_id, velo);

#elif(MC2_DRIVE_2SET == 1)
	UINT8 u8drv_id;

	if (drv_id < (ENG_MMDI)DRIVEDIVIDE)
	{
		return !IsSendData() ? FALSE : g_pMC2CThread->WriteDevGoVelo(drv_id, velo);
	}
	else
	{
		u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
		return !IsSendData() ? FALSE : g_pMC2bCThread->WriteDevGoVelo((ENG_MMDI)u8drv_id, velo);
	}

#endif

}

/*
 desc : 모터 기본 속도 및 가속도 제어
 parm : drv_id	- [in]  Axis 정보 (0, 1, 4, 5)
		velo	- [in]  이동 속도 (단위: mm/sec)
		acce	- [in]  이동 가속도 (단위: mm^2)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMC2_SendDevBaseVeloAcce(ENG_MMDI drv_id, UINT32 velo, UINT32 acce)
{
	if (velo < 1 || acce < 1)	return FALSE;

#if (MC2_DRIVE_2SET == 0)
	return !IsSendData() ? FALSE : g_pMC2CThread->WriteDevBaseVeloAcce(drv_id, velo, acce);
#elif(MC2_DRIVE_2SET == 1)
	UINT8 u8drv_id;
	
	if (drv_id < (ENG_MMDI)DRIVEDIVIDE)
	{
		return !IsSendData() ? FALSE : g_pMC2CThread->WriteDevBaseVeloAcce(drv_id, velo, acce);
	}
	else
	{
		u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
		return !IsSendData() ? FALSE : g_pMC2bCThread->WriteDevBaseVeloAcce((ENG_MMDI)u8drv_id, velo, acce);
	}
#endif

}

/*
 desc : Luria Expose Mode로 설정
 parm : drv_id	- [in]  노광 방향의 축 Drive ID
		mode	- [in]  Area (Align) mode or Expose mode (ENG_MTAE)
						Area Mode			: 절대 스테이지가 움직이지 않음
						Expo Mode			: 현재 위치 값보다 Min 값이 작으면, 움직이지 않으며, 나머지는 Min 값으로 이동 함
		lens	- [in]  Lens Magnification (2000, 1000, 500)
		pixel	- [in]  Pixel Size (unit: 100 nm or 0.1 um)
		scroll	- [in]  Pixel Resolution (5.4 or 10.8 or 21.6 um) (unit: pm; picometer = 1/1000 nm)
						<LLS 10> <lens:  500>
							Scroll mode 1	:  5.4310344828	um
							Scroll mode 2	: 10.8620689655	um
							Scroll mode 3	: 16.2931034483	um
							Scroll mode 4	: 21.724137931	um
						<LLS 25> <lens: 1000>
							Scroll mode 1	: 10.8620689655	um
							Scroll mode 2	: 21.724137931	um
							Scroll mode 3	: 32.5862068966	um
							Scroll mode 4	: 43.4482758621	um
		begin	- [in]  노광 시작 위치 (단위: nm)
		end		- [in]  노광 종료 위치 (단위: nm)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMC2_SendDevLuriaMode(ENG_MMDI drv_id, ENG_MTAE mode,
									   UINT32 pixel, INT32 begin, INT32 end)
{
	//Trigger 0.5um
	if (0 == pixel)	pixel = 100000;	/* 100 nm or 0.1 um */
	//if (0 == pixel)	pixel = 500000;	/* 100 nm or 0.1 um */

#if (MC2_DRIVE_2SET == 0)
	return !IsSendData() ? FALSE : g_pMC2CThread->WriteDevLuriaMode(drv_id, mode, pixel, begin, end);
#elif(MC2_DRIVE_2SET == 1)
	UINT8 u8drv_id;

	if (drv_id < (ENG_MMDI)DRIVEDIVIDE)
	{
		return !IsSendData() ? FALSE : g_pMC2CThread->WriteDevLuriaMode(drv_id, mode, pixel, begin, end);
	}
	else
	{
		u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
		return !IsSendData() ? FALSE : g_pMC2bCThread->WriteDevLuriaMode((ENG_MMDI)u8drv_id, mode, pixel, begin, end);
	}
#endif

}

/*
 desc : 스테이지 상대 이동 (Up, Down, Left, Bottom)
 parm : method	- [in]  ENG_MMMM (Jog, Step, Abs:절대 위치)
		drv_id	- [in]  Motor Drive ID (SD3 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
		direct	- [in]  이동 방향 (Up / Down / Left / Right ...)
		dist	- [in]  이동 거리 (단위: 0.1 um or 100 nm) (반드시 0 보다 커야 됨)
		velo	- [in]  이동 속도 (단위: 0.1 um/sec or 100 nm/sec)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMC2_SendDevRefMove(ENG_MMMM method, ENG_MMDI drv_id,
									 ENG_MDMD direct, INT32 dist, UINT32 velo)
{
	INT32 i32DrvPos	= 0;
	INT32 i32MaxDist= (INT32)ROUNDED(g_pstConfig->mc2_svc.max_dist[UINT8(drv_id)] * 10000.0f, 0);
	
	/* 현재 장비가 멈춘 상태인지 확인 */
	if (!g_pstShMemMC2->act_data[UINT8(drv_id)].IsStopped())
	{
		return FALSE;
	}

	/* 값 유효성 확인 */
	if (velo < 1)	return FALSE;
	if (ENG_MMMM::en_move_step == method && dist < 1 )
	{
		return FALSE;
	}
	/* 현재 드라이브 위치 */
	i32DrvPos	= g_pstShMemMC2->act_data[UINT8(drv_id)].real_position;

	/* 원점 기준에 따라 조건 확인 */
	switch ((ENG_MSOP)g_pstConfig->mc2_svc.origin_point)
	{
	case ENG_MSOP::en_left_bottom	:
		switch (direct)
		{
		case ENG_MDMD::en_move_right	:
		case ENG_MDMD::en_move_up	:	if ((i32DrvPos + dist) > i32MaxDist)	dist = i32MaxDist - i32DrvPos;	break;
		case ENG_MDMD::en_move_left	:
		case ENG_MDMD::en_move_down	:	if ((i32DrvPos - dist) < 0)				dist = i32DrvPos;				break;
		}
		break;

	case ENG_MSOP::en_left_top		:
		switch (direct)
		{
		case ENG_MDMD::en_move_left	:
		case ENG_MDMD::en_move_up	:	if ((i32DrvPos - dist) < 0)				dist = i32DrvPos;				break;
		case ENG_MDMD::en_move_right	:
		case ENG_MDMD::en_move_down	:	if ((i32DrvPos + dist) > i32MaxDist)	dist = i32MaxDist - i32DrvPos;	break;
		}
		break;

	case ENG_MSOP::en_right_bottom	:
		switch (direct)
		{
		case ENG_MDMD::en_move_left	:
		case ENG_MDMD::en_move_up	:	if ((i32DrvPos + dist) > i32MaxDist)	dist = i32MaxDist - i32DrvPos;	break;
		case ENG_MDMD::en_move_right	:
		case ENG_MDMD::en_move_down	:	if ((i32DrvPos - dist) < 0)				dist = i32DrvPos;				break;
		}
		break;

	case ENG_MSOP::en_right_top		:
		switch (direct)
		{
		case ENG_MDMD::en_move_right	:
		case ENG_MDMD::en_move_up	:	if ((i32DrvPos - dist) < 0)				dist = i32DrvPos;				break;
		case ENG_MDMD::en_move_left	:
		case ENG_MDMD::en_move_down	:	if ((i32DrvPos + dist) > i32MaxDist)	dist = i32MaxDist - i32DrvPos;	break;
		}
		break;
	}

	/* 이동 명령 송신 */
	return !IsSendData() ? FALSE : g_pMC2CThread->WriteDevMoveRef(method, drv_id, direct,
																  dist, velo);
}

/*
 desc : 스테이지 절대 이동 (Up, Down, Left, Bottom)
 parm : drv_id	- [in]  Motor Drive ID (SD3 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
		move	- [in]  스테이지가 최종 이동될 위치 (단위: 0.1 um or 100 nm)
		velo	- [in]  이동 속도 (단위: 0.1 um or 100 nm /sec)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMC2_SendDevAbsMove(ENG_MMDI drv_id, INT32 move, UINT32 velo)
{
#if (MC2_DRIVE_2SET == 0)
	INT32 i32MinDist = (INT32)ROUNDED(g_pstConfig->mc2_svc.min_dist[UINT8(drv_id)] * 10000.0f, 0);
	INT32 i32MaxDist = (INT32)ROUNDED(g_pstConfig->mc2_svc.max_dist[UINT8(drv_id)] * 10000.0f, 0);

	/* 현재 장비가 멈춘 상태인지 확인 */
	if (!g_pstShMemMC2->act_data[(UINT8)drv_id].IsStopped())
	{
		LOG_WARN(ENG_EDIC::en_mc2, L"Motion is not stopped");
		return FALSE;
	}
#elif(MC2_DRIVE_2SET == 1)
	INT32 i32MinDist, i32MaxDist;
	UINT8 u8drv_id;

	if (drv_id < (ENG_MMDI)DRIVEDIVIDE)
	{
		i32MinDist = (INT32)ROUNDED(g_pstConfig->mc2_svc.min_dist[UINT8(drv_id)] * 10000.0f, 0);
		i32MaxDist = (INT32)ROUNDED(g_pstConfig->mc2_svc.max_dist[UINT8(drv_id)] * 10000.0f, 0);

		/* MC2A 현재 장비가 멈춘 상태인지 확인 */
		if (!g_pstShMemMC2->act_data[(UINT8)drv_id].IsStopped())
		{
			LOG_WARN(ENG_EDIC::en_mc2, L"Motion is not stopped");
			return FALSE;
		}
	}
	else
	{
		u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
		i32MinDist = (INT32)ROUNDED(g_pstConfig->mc2b_svc.min_dist[u8drv_id] * 10000.0f, 0);
		i32MaxDist = (INT32)ROUNDED(g_pstConfig->mc2b_svc.max_dist[u8drv_id] * 10000.0f, 0);

		/* MC2B 현재 장비가 멈춘 상태인지 확인 */
		if (!g_pstShMemMC2b->act_data[u8drv_id].IsStopped())
		{
			LOG_WARN(ENG_EDIC::en_mc2, L"Motion is not stopped");
			return FALSE;
		}
	}
#endif

	move = std::clamp(move, i32MinDist + 5, i32MaxDist - 5);

	/* 이동 위치 값이 조건에 부합되는지 확인 */
	if (i32MinDist > move || i32MaxDist < move)
	{
		LOG_WARN(ENG_EDIC::en_mc2, L"Move position is not in valid range");
		return FALSE;
	}

#if (MC2_DRIVE_2SET == 0)
	return !IsSendData() ? FALSE : g_pMC2CThread->WriteDevMoveAbs(drv_id, move, velo);
#elif(MC2_DRIVE_2SET == 1)

	u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
	if (drv_id < (ENG_MMDI)DRIVEDIVIDE)
	{
		return !IsSendData() ? FALSE : g_pMC2CThread->WriteDevMoveAbs(drv_id, move, velo);
	}
	else
	{
		return !IsSendData() ? FALSE : g_pMC2bCThread->WriteDevMoveAbs((ENG_MMDI)u8drv_id, move, velo);
	}
#endif

}

/*
 desc : 스테이지를 Home 위치 (0, 0)로 이동
 parm : drv_id	- [in]  Home 위치로 이동하기 위한 Drive ID
		velo	- [in]  스테이지 이동 속도 (0.1 um/sec or 100 nm/sec)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMC2_SendDevMovingHome(ENG_MMDI drv_id, INT32 velo)
{
	if (velo < 1)	return FALSE;

#if (MC2_DRIVE_2SET == 0)
	/* 현재 해당 드라이브가 완전히 멈추었는지 여부 */
	if (!g_pstShMemMC2->act_data[UINT8(drv_id)].IsStopped())	return FALSE;
	/* 절대 좌표로 위치 이동 (벡터로 이동) */
	return !IsSendData() ? FALSE : g_pMC2CThread->WriteDevMoveAbs(drv_id, 0, velo);
#elif(MC2_DRIVE_2SET == 1)
	if (drv_id < (ENG_MMDI)DRIVEDIVIDE)
	{
		/* 현재 해당 드라이브가 완전히 멈추었는지 여부 */
		if (!g_pstShMemMC2->act_data[UINT8(drv_id)].IsStopped())	return FALSE;
		/* 절대 좌표로 위치 이동 (벡터로 이동) */
		return !IsSendData() ? FALSE : g_pMC2CThread->WriteDevMoveAbs(drv_id, 0, velo);
	}
	else
	{
		UINT8 u8drv_id;
		u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
		/* 현재 해당 드라이브가 완전히 멈추었는지 여부 */
		if (!g_pstShMemMC2b->act_data[UINT8(u8drv_id)].IsStopped())	return FALSE;
		/* 절대 좌표로 위치 이동 (벡터로 이동) */
		return !IsSendData() ? FALSE : g_pMC2bCThread->WriteDevMoveAbs((ENG_MMDI)u8drv_id, 0, velo);
	}
#endif
}

/*
 desc : 스테이지 X / Y를 Vector 로 이동
 parm : drv_x	- [in]  Vector 이동 대상 축의 Drive ID 1
		drv_y	- [in]  Vector 이동 대상 축의 Drive ID 2
		pos_x	- [in]  스테이지가 최종 이동될 X 축의 위치 (단위: 0.1 um or 100 nm)
		pos_y	- [in]  스테이지가 최종 이동될 Y 축의 위치 (단위: 0.1 um or 100 nm)
		velo	- [in]  스테이지 이동 속도 값
		axis	- [out] Vector 이동할 경우, Master Axis (기준 축) 값 반환
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMC2_SendDevMoveVectorXY(ENG_MMDI drv_x, ENG_MMDI drv_y,
										  INT32 pos_x, INT32 pos_y, INT32 velo, ENG_MMDI &axis)
{
	UINT32 u32DistX, u32DistY;	/* 단위: 0.1 um or 100 nm */
	INT32 i32PosM, i32PosS;		/* Master & Slave의 이동 위치 값 (단위: 0.1 um or 100 nm) */
	INT32 i32MaxDistX	= (INT32)ROUNDED(g_pstConfig->mc2_svc.max_dist[UINT8(drv_x)] * 10000.0f, 0);
	INT32 i32MaxDistY	= (INT32)ROUNDED(g_pstConfig->mc2_svc.max_dist[UINT8(drv_y)] * 10000.0f, 0);
	ENG_MMDI enDrvM, enDrvS;	/* Master & Slave Driver */

	if (velo < 1)	return FALSE;
	/* 현재 해당 드라이브가 완전히 멈추었는지 여부 */
	if (!g_pstShMemMC2->act_data[UINT8(drv_x)].IsStopped())	return FALSE;
	if (!g_pstShMemMC2->act_data[UINT8(drv_y)].IsStopped())	return FALSE;

	/* 이동 위치 값 중 하나라도 초과되었는지 확인 */
	if (i32MaxDistX < pos_x || i32MaxDistY < pos_y)	return FALSE;

	/* 이동 축이 긴 쪽에 기준 축을 잡아야 한다. */
	/* 현재 위치에서 이동하는 1, 2 축의 위치 중 가장 멀리 떨어져 있는 곳 판단 */
	u32DistX= (UINT32)abs(g_pstShMemMC2->act_data[UINT8(drv_x)].real_position - pos_x);
	u32DistY= (UINT32)abs(g_pstShMemMC2->act_data[UINT8(drv_y)].real_position - pos_y);

	/* 1 or 2 축 중 어느 쪽이 멀리 이동하는지 판단 */
	axis	= u32DistX > u32DistY ? drv_x : drv_y;

	/* Master Driver */
	if (u32DistX > u32DistY)
	{
		enDrvM	= drv_x;
		enDrvS	= drv_y;
		i32PosM	= pos_x;
		i32PosS	= pos_y;
	}
	else
	{
		enDrvM	= drv_y;
		enDrvS	= drv_x;
		i32PosM	= pos_y;
		i32PosS	= pos_x;
	}

	return g_pMC2CThread->WriteStageMoveVec(enDrvM, enDrvS, i32PosM, i32PosS, velo);

}

/*
 desc : Device Stopped
 parm : drv_id	- [in]  SD3 Driver ID 즉, 0x00, 0x01, 0x04, 0x05
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMC2_SendDevStopped(ENG_MMDI drv_id)
{
	if (!IsSendData())	return FALSE;

#if (MC2_DRIVE_2SET == 0)
	return g_pMC2CThread->WriteDevCtrl(drv_id, ENG_MCCN::en_cancel_command);
#elif(MC2_DRIVE_2SET == 1)
	UINT8 u8drv_id;

	if (drv_id < (ENG_MMDI)DRIVEDIVIDE)
	{
		return g_pMC2CThread->WriteDevCtrl(drv_id, ENG_MCCN::en_cancel_command);
	}
	else
	{
		u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
		return g_pMC2bCThread->WriteDevCtrl((ENG_MMDI)u8drv_id, ENG_MCCN::en_cancel_command);
	}
#endif
}

/*
 desc : Device Locked
 parm : drv_id	- [in]  SD3 Driver ID
		flag	- [in]  TRUE (Servo On) or FALSE(Servo Off) Command
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMC2_SendDevLocked(ENG_MMDI drv_id, BOOL flag)
{
	if (!IsSendData())	return FALSE;

	UINT8 u8drv_id;

	if (drv_id < (ENG_MMDI)DRIVEDIVIDE)
	{
		return	flag ?  g_pMC2CThread->WriteDevCtrl(drv_id, ENG_MCCN::en_do_power_on) :
						g_pMC2CThread->WriteDevCtrl(drv_id, ENG_MCCN::en_do_power_off);
	}
#if (MC2_DRIVE_2SET == 1)
	else
	{
		u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
		return	flag ?  g_pMC2bCThread->WriteDevCtrl((ENG_MMDI)u8drv_id, ENG_MCCN::en_do_power_on) :
						g_pMC2bCThread->WriteDevCtrl((ENG_MMDI)u8drv_id, ENG_MCCN::en_do_power_off);
	}
#endif
}

/*
 desc : Device Fault Reset (Error Init)
 parm : drv_id	- [in]  Motor Drive ID (SD3 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMC2_SendDevFaultReset(ENG_MMDI drv_id)
{
	if (!IsSendData())	return FALSE;

#if (MC2_DRIVE_2SET == 0)
	/* 모든 모터 드라이브의 에러 초기화 */
	return g_pMC2CThread->WriteDevCtrl(drv_id, ENG_MCCN::en_fault_reset);
#elif(MC2_DRIVE_2SET == 1)
	UINT8 u8drv_id;
	if (drv_id < (ENG_MMDI)DRIVEDIVIDE)
	{
		/* 모든 모터 드라이브의 에러 초기화 */
		return g_pMC2CThread->WriteDevCtrl(drv_id, ENG_MCCN::en_fault_reset);
	}
	else
	{
		/* 모든 모터 드라이브의 에러 초기화 */
		u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
		return g_pMC2bCThread->WriteDevCtrl(ENG_MMDI(u8drv_id), ENG_MCCN::en_fault_reset);
	}
#endif
}

/*
 desc : 모든 드라이브들 (복수의 SD3 Drive) Error에 대해 동시에 Fault Reset 처리
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMC2_SendDevFaultResetAll()
{
	UINT8 i	= 0x00;
	STG_RVCH stHead	= {NULL};
	STG_RVCD stData[MAX_MC2_DRIVE]	= {NULL};
	LPG_RVCD pstData= g_pstShMemMC2->ref_data;

	if (!IsSendData())	return FALSE;

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
	/* 기존 값 가져오기 */
	memcpy(&stHead, &g_pstShMemMC2->ref_head, sizeof(STG_RVCH));
	/* EnStop 값 Heat-Beat 토콜? */
	stHead.mc_en_stop	= 0x00;
	stHead.mc_heart_beat= !stHead.mc_heart_beat;
#endif

	/* 설정 대상의 드라이브 개수 유효성 처리 */
	for (; i<MAX_MC2_DRIVE; i++)
	{
		stData[i].ctrl_cmd			= UINT16(ENG_MCCN::en_fault_reset);
#if 0	/* I don't know what it's for. */
		stData[i].ctrl_cmd_toggle	= !pstData[i].ctrl_cmd_toggle;
#endif
	}

	/* 모든 SD3 드라이브 중 임의 설정된 드라이브에 대한 Fault Reset 처리 */
	return g_pMC2CThread->WriteObjectValueByIndex(&stHead, stData);

}

/*
 desc : Device Homed
 parm : drv_id	- [in]  Motor Drive ID
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMC2_SendDevHoming(ENG_MMDI drv_id)
{
	if (!IsSendData())	return FALSE;
#if (MC2_DRIVE_2SET == 0)
	return g_pMC2CThread->WriteDevCtrl(drv_id, ENG_MCCN::en_do_homing);
#elif(MC2_DRIVE_2SET == 1)
	UINT8 u8drv_id;

	if (drv_id < (ENG_MMDI)DRIVEDIVIDE)
	{
		return g_pMC2CThread->WriteDevCtrl(drv_id, ENG_MCCN::en_do_homing);
	}
	else
	{
		u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
		return g_pMC2bCThread->WriteDevCtrl((ENG_MMDI)u8drv_id, ENG_MCCN::en_do_homing);
	}
#endif

}

/*
 desc : 모든 드라이브들 (복수의 SD3 Drive)에 대해 동시에 Homing 처리
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMC2_SendDevHomingAll()
{
	UINT8 i	= 0x00, j = 0x00, u8DrvCount = GetConfig()->mc2_svc.drive_count;
	STG_RVCH stHead	= {NULL};
	STG_RVCD stData[MAX_MC2_DRIVE]	= {NULL};
	LPG_RVCD pstData= g_pstShMemMC2->ref_data;

	if (!IsSendData())	return FALSE;

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
	/* 기존 값 가져오기 */
	memcpy(&stHead, &g_pstShMemMC2->ref_head, sizeof(STG_RVCH));
	/* EnStop 값 Heat-Beat 토콜? */
	stHead.mc_en_stop	= 0x00;
	stHead.mc_heart_beat= !stHead.mc_heart_beat;
#endif

	/* 설정 대상의 드라이브 개수 유효성 처리 */
	for (j=0x00; i<MAX_MC2_DRIVE; i++)
	{
		if (i == GetConfig()->mc2_svc.axis_id[j])
		{
			stData[i].ctrl_cmd			= UINT16(ENG_MCCN::en_do_homing);
#if 0	/* I don't know what it's for. */
			stData[i].ctrl_cmd_toggle	= !pstData[i].ctrl_cmd_toggle;
#endif
			j++;
		}
	}

	/* 모든 SD3 드라이브 중 임의 설정된 드라이브에 대한 Homing 처리 */
	return g_pMC2CThread->WriteObjectValueByIndex(&stHead, stData);
}

/*
 desc : Luria가 현재 노광 (Printing) 중인지 여부에 따라,
		MC2에게 주기적으로 통신 데이터 요청 여부 설정
 parm : enable	- [in]  TRUE : 요청 진행, FALSE : 요청 중지 (아예 요청 중지가 아님)
 retn : None
*/
API_EXPORT VOID uvMC2_SetSendPeriodPkt(BOOL enable)
{
	if (g_pMC2CThread)	g_pMC2CThread->SetSendPeriodPkt(enable);
}

/*
 desc : MC2에게 주기적으로 패킷 데이터를 요청하는지 여부 반환
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMC2_IsSendPeriodPkt()
{
	if (!g_pMC2CThread)	return TRUE;
	return g_pMC2CThread->IsSendPeriodPkt();
}

/*
 desc : Reference Data 전체 요청
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMC2_ReadReqPktRefAll()
{
	if (!g_pMC2CThread)	return TRUE;
	/* 패킷 데이터 송신 */
	return g_pMC2CThread->ReadReqPktRefAll();
}

/*
 desc : Active Data 전체 요청
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMC2_ReadReqPktActAll()
{
	if (!g_pMC2CThread)	return TRUE;
	/* 패킷 데이터 송신 */
	return g_pMC2CThread->ReadReqPktActAll();
}

/*
 desc : MC2에서 동작 중인 Drive (SD3S) 중 한 개라도 에러가 발생 했는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMC2_IsAnyDriveError()
{
	UINT8 i	= 0x00;

	for (; i < g_pstConfig->mc2_svc.drive_count; i++)
	{
		UINT8 aaa;
		aaa = g_pstConfig->mc2_svc.axis_id[i];
		if (g_pstShMemMC2->IsDriveError(g_pstConfig->mc2_svc.axis_id[i]))	return TRUE;
	}
#if(MC2_DRIVE_2SET == 1)
	for (i=0; i < g_pstConfig->mc2_svc.drive_count; i++)
	{
		if (g_pstShMemMC2b->IsDriveError(g_pstConfig->mc2b_svc.axis_id[i]))	return TRUE;
	}
#endif


	return FALSE;
}

/*
 desc : Device Control - Reference Header - for EN_STOP and HEART_BEAT
 parm : en_stop	- [in]  ENG_MMSM (0x00:Only Homing operation or 0x01: Other operation)
		beat	- [in]  Toggle (0 or 1) (Alive Check 용도)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMC2_SetRefHeadMControl(ENG_MMST en_stop, UINT8 beat)
{
	UINT8 u8EnStop	= UINT8(en_stop) << 6;
	UINT8 u8Beat	= beat << 7;
	return g_pMC2CThread->WritePdoRefHead(ENG_PSOI::en_pdo_ref_head_mcontrol, UINT64(u8EnStop|u8Beat));
}
API_EXPORT BOOL uvMC2_SetRefHeadMControlEx(ENG_MMST en_stop)
{
	UINT8 u8EnStop	= UINT8(en_stop) << 6;
	UINT8 u8Beat	= (!g_pstShMemMC2->ref_head.mc_heart_beat) << 7;
	return g_pMC2CThread->WritePdoRefHead(ENG_PSOI::en_pdo_ref_head_mcontrol, UINT64(u8EnStop|u8Beat));
}


#ifdef __cplusplus
}
#endif
