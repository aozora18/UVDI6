
/*
 desc : MC2 Client Library
*/

#pragma once

#include "../conf/mc2.h"
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
#include "../conf/conf_podis.h"
#elif (CUSTOM_CODE_GEN2I == DELIVERY_PRODUCT_ID)
#include "../conf/conf_gen2i.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/*
 desc : MC2와의 통신 시작
 parm : config	- [in]  환경 설정 정보
		shmem	- [in]  MC2 관련 공유 메모리
 retn : TRUE or FALSE
*/
//API_IMPORT BOOL uvMC2_Init(LPG_CIEA config, LPG_MDSM shmem);
API_IMPORT BOOL uvMC2_Init(LPG_CIEA config, LPG_MDSM shmem, LPG_MDSM shmem2);

API_IMPORT BOOL uvMC2a_Init(LPG_CIEA config, LPG_MDSM shmem);
API_IMPORT BOOL uvMC2b_Init(LPG_CIEA config, LPG_MDSM shmem);
/*
 desc : MC2와의 통신 정지
 parm : None
 retn : None
*/
API_IMPORT VOID uvMC2_Close();
/*
 desc : 기존 시스템 접속 해제 후 재접속 진행
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_Reconnected();
/*
 desc : MC2 Server에 연결 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_IsConnected();
/*
 desc : 가장 최근에 발생된 소켓 에러 코드 값 반환
 parm : None
 retn : Socket Error Code (MSDN 참조)
*/
API_IMPORT UINT32 uvMC2_GetSockLastError();

/* --------------------------------------------------------------------------------------------- */
/*                                  MC2 Communication Functions                                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 모터 속도 제어
 parm : drv_id	- [in]  Axis 정보 (0, 1, 4, 5)
		velo	- [in]  이동 속도 (단위: mm/sec)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevGoVelo(ENG_MMDI drv_id, UINT32 velo);
/*
 desc : 모터 기본 속도 및 가속도 제어
 parm : drv_id	- [in]  Axis 정보 (0, 1, 4, 5)
		velo	- [in]  이동 속도 (단위: mm/sec)
		acce	- [in]  이동 가속도 (단위: mm^2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevBaseVeloAcce(ENG_MMDI drv_id, UINT32 velo, UINT32 acce);
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
API_IMPORT BOOL uvMC2_SendDevLuriaMode(ENG_MMDI drv_id, ENG_MTAE mode,
									   UINT32 pixel=0, INT32 begin=0, INT32 end=0);
/*
 desc : 스테이지 상대 이동 (Up, Down, Left, Bottom)
 parm : method	- [in]  ENG_MMMM
		drv_id	- [in]  Motor Drive ID (SD2 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
		direct	- [in]  이동 방향 (Up / Down / Left / Right ...)
		dist	- [in]  이동 거리 (단위: 0.1 um or 100 nm) (반드시 0 보다 커야 됨)
		velo	- [in]  이동 속도 (단위: 0.1 um/sec or 100 nm/sec)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevRefMove(ENG_MMMM method, ENG_MMDI drv_id,
									 ENG_MDMD direct, INT32 dist, UINT32 velo);
/*
 desc : 스테이지 절대 이동 (Up, Down, Left, Bottom)
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
		move	- [in]  스테이지가 최종 이동될 위치 (단위: 0.1 um or 100 nm)
		velo	- [in]  이동 속도  (단위: 0.1 um or 100 nm /sec)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevAbsMove(ENG_MMDI drv_id, INT32 move, UINT32 velo);
/*
 desc : 스테이지를 Home 위치 (0, 0)로 이동
 parm : drv_id	- [in]  Home 위치로 이동하기 위한 Drive ID
		velo	- [in]  스테이지 이동 속도 (0.1 um/sec or 100 nm/sec)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevMovingHome(ENG_MMDI drv_id, INT32 velo);
/*
 desc : 스테이지를 Vector 로 이동
 parm : drv_x	- [in]  Vector 이동 대상 축의 Drive ID 1
		drv_y	- [in]  Vector 이동 대상 축의 Drive ID 2
		pos_x	- [in]  스테이지가 최종 이동될 X 축의 위치 (단위: 0.1 um or 100 nm)
		pos_y	- [in]  스테이지가 최종 이동될 Y 축의 위치 (단위: 0.1 um or 100 nm)
		velo	- [in]  스테이지 이동 속도 값
		axis	- [out] Vector 이동할 경우, Master Axis (기준 축) 값 반환
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevMoveVectorXY(ENG_MMDI drv_x, ENG_MMDI drv_y,
										  INT32 pos_x, INT32 pos_y, INT32 velo, ENG_MMDI &axis);
/*
 desc : Device Stopped
 parm : drv_id	- [in]  SD2 Driver ID 즉, 0x00, 0x01, 0x04, 0x05
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevStopped(ENG_MMDI drv_id);
/*
 desc : Device Locked
 parm : drv_id	- [in]  SD2 Driver ID
		flag	- [in]  TRUE (Servo On) or FALSE(Servo Off) Command
반환 : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevLocked(ENG_MMDI drv_id, BOOL flag);
/*
 desc : Device Fault Reset (Error Init)
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevFaultReset(ENG_MMDI drv_id);
/*
 desc : 임의 SD3 드라이브들 (복수의 SD3 Drive) Error에 대해 동시에 Fault Reset 처리
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevFaultResetAll();
/*
 desc : 스테이지 Homing
 parm : drv_id	- [in]  Motor Drive ID. 만약 None이면 전체 Homing
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevHoming(ENG_MMDI drv_id);
/*
 desc : 모든 드라이브들 (복수의 SD3 Drive)에 대해 동시에 Homing 처리
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevHomingAll();
API_IMPORT VOID	 uvMC2_SetInterlockState(bool isInterlocked);
/*
 desc : Luria가 현재 노광 (Printing) 중인지 여부에 따라,
		MC2에게 주기적으로 통신 데이터 요청 여부 설정
 parm : enable	- [in]  TRUE : 요청 진행, FALSE : 요청 중지 (아예 요청 중지가 아님)
 retn : None
*/
API_IMPORT VOID uvMC2_SetSendPeriodPkt(BOOL enable);
/*
 desc : MC2에게 주기적으로 패킷 데이터를 요청하는지 여부 반환
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_IsSendPeriodPkt();
/*
 desc : Reference Data 전체 요청
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_ReadReqPktRefAll();
/*
 desc : Active Data 전체 요청
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_ReadReqPktActAll();
/*
 desc : MC2에서 동작 중인 Drive (SD2S) 중 한 개라도 에러가 발생 했는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_IsAnyDriveError();
/*
 desc : Device Control - Reference Header - for EN_STOP and HEART_BEAT
 parm : en_stop	- [in]  ENG_MMSM (0x00:Only Homing operation or 0x01: Other operation)
		beat	- [in]  Toggle (0 or 1) (Alive Check 용도)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SetRefHeadMControl(ENG_MMST en_stop, UINT8 beat);
API_IMPORT BOOL uvMC2_SetRefHeadMControlEx(ENG_MMST en_stop);


#ifdef __cplusplus
}
#endif
