
/*
 desc : The Interface Library for Sieb & Meyer's MC2 Motion Equipment
*/

#include "pch.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Check whether the number of the SD2S drive of MC2 is valid
 parm : drv_no	- [in]  SD2S drive number (0x00 ~ 0x07)
 retn : TRUE or FALSE
*/
BOOL IsMC2DriveValid(ENG_MMDI drv_no)
{
	if (UINT8(drv_no) >= MAX_MC2_DRIVE*2)
	{
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128, L"The SD2S drive of MC2 is not valid (drv_no=0x%02x)", UINT8(drv_no));
		LOG_WARN(ENG_EDIC::en_mc2, L"");
		return FALSE;
	}

	return TRUE;
}

#ifdef __cplusplus
extern "C"
{
#endif

/* --------------------------------------------------------------------------------------------- */
/*                              외부 함수 - < MC2 > < for Engine >                               */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 모든 Device Homing All (Sequential) (0x3808)
 parm : None
 retn : TRUE or FALSE
 note : EN_STOP 모드가 적용되지 않는 곳에 사용
*/
API_EXPORT BOOL uvEng_MC2_SendDevHomingAll()
{
	UINT8 i	= 0x00;


	/* 모든 드라이브에 대한 Homing 작업 수행 */
	for (; i < GetConfig()->mc2_svc.drive_count; i++)
	{
		if (!uvMC2_SendDevHoming(ENG_MMDI(GetConfig()->mc2_svc.axis_id[i])))	return FALSE;
	}

#if(MC2_DRIVE_2SET == 1)
	UINT8 u8drv_id;
	/* 모든 드라이브에 대한 Homing 작업 수행 */
	for (i=0; i < GetConfig()->mc2b_svc.drive_count; i++)
	{
		u8drv_id = i + DRIVEDIVIDE;
		if (!uvMC2_SendDevHoming(ENG_MMDI(u8drv_id)))	return FALSE;
	}

#endif

	
	return TRUE;
}

/*
 desc : 기존 시스템 접속 해제 후 재접속 진행
 parm : None
 retn : TRUE or FALSE
 note : EN_STOP 모드가 적용되지 않는 곳에 사용
*/
API_EXPORT BOOL uvEng_MC2_Reconnected()
{

	uvMC2_Reconnected();

	return TRUE;
}

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
API_EXPORT BOOL uvEng_MC2_SendDevMoveVectorXY(ENG_MMDI drv_x, ENG_MMDI drv_y,
											  DOUBLE pos_x, DOUBLE pos_y, DOUBLE velo, ENG_MMDI &axis)
{
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;
	if (!IsMC2DriveValid(drv_x))	return FALSE;
	if (!IsMC2DriveValid(drv_y))	return FALSE;

	INT32 i32MoveX	= (INT32)ROUNDED(pos_x * 10000.0f, 0);	/* mm -> 0.1 um or 100 nm */
	INT32 i32MoveY	= (INT32)ROUNDED(pos_y * 10000.0f, 0);	/* mm -> 0.1 um or 100 nm */
	UINT32 u32Velo	= (UINT32)ROUNDED(velo * 10000.0f, 0);	/* mm -> 0.1 um or 100 nm */
	return uvMC2_SendDevMoveVectorXY(drv_x, drv_y, i32MoveX, i32MoveY, u32Velo, axis);
}

/*
 desc : Device Stopped
 parm : drv_id	- [in]  SD2 Driver ID 즉, 0x00, 0x01, 0x04, 0x05
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MC2_SendDevStopped(ENG_MMDI drv_id)
{
	if (!IsMC2DriveValid(drv_id))	return FALSE;

	/* 전체 드라이브를 동시에 중지는 시키지 못함. 개별적으로 해야 됨 */
	if (drv_id == ENG_MMDI::en_all || drv_id == ENG_MMDI::en_axis_none)	return FALSE;

	return uvMC2_SendDevStopped(drv_id);
}

/*
 desc : 현재 모든 Drive의 Motor 중지
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MC2_SendDevStoppedAll()
{
	UINT8 i	= 0x00;
	LPG_CMSI pstCfg	= &GetConfig()->mc2_svc;

	for (; i < pstCfg->drive_count; i++)
	{
		if (uvMC2_SendDevStopped(ENG_MMDI(pstCfg->axis_id[i])))	return FALSE;
	}
#if(MC2_DRIVE_2SET == 1)
	LPG_CMSI pstCfg2 = &GetConfig()->mc2b_svc;
	UINT8 u8drv_id;

	for (i=0; i < pstCfg2->drive_count; i++)
	{
		u8drv_id = i + DRIVEDIVIDE;
		if (uvMC2_SendDevStopped(ENG_MMDI(u8drv_id)))	return FALSE;
	}
#endif

	return TRUE;
}

/*
 desc : Device Locked (On or OFF)
 parm : drv_id	- [in]  SD2 Driver ID
		flag	- [in]  TRUE (Servo On) or FALSE(Servo Off) Command
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MC2_SendDevLocked(ENG_MMDI drv_id, BOOL flag)
{
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;
	if (!IsMC2DriveValid(drv_id))	return FALSE;

	return uvMC2_SendDevLocked(drv_id, flag);
}

/*
 desc : All Device Locked (On or OFF)
 parm : flag	- [in]  TRUE (Servo On) or FALSE(Servo Off) Command
 retn : TRUE (All Servo On) or FALSE (All Servo Off)
*/
API_EXPORT BOOL uvCmn_MC2_SendDevLockedAll(BOOL flag)
{
	UINT8 i	= 0x00;

	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;

	/* 모든 드라이브에 대한 Homing 작업 수행 */
	for (; i < GetConfig()->mc2_svc.drive_count; i++)
	{
		if (!uvMC2_SendDevLocked(ENG_MMDI(GetConfig()->mc2_svc.axis_id[i]), flag))	return FALSE;
	}

#if(MC2_DRIVE_2SET == 1)
	UINT8 u8drv_id;
	for (i=0; i < GetConfig()->mc2b_svc.drive_count; i++)
	{
		u8drv_id = i + DRIVEDIVIDE;
		if (!uvMC2_SendDevLocked(ENG_MMDI(ENG_MMDI(u8drv_id)), flag))	return FALSE;
	}

#endif
	return TRUE;
}


/*
 desc : Device Fault Reset (Error Init)
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MC2_SendDevFaultReset(ENG_MMDI drv_id)
{
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;
	if (!IsMC2DriveValid(drv_id))	return FALSE;

	/* 모든 모터 드라이브의 에러 초기화 */
	return uvMC2_SendDevFaultReset(drv_id);
}

/*
 desc : 모든 Device Fault Reset All (Sequential) (0x3808)
 parm : None
 retn : TRUE or FALSE
 note : EN_STOP 모드가 적용되지 않는 곳에 사용
*/
API_EXPORT BOOL uvEng_MC2_SendDevFaultResetAll()
{
	UINT8 i	= 0x00;
	LPG_CMSI pstCfg	= &GetConfig()->mc2_svc;

	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;

	for (; i<pstCfg->drive_count; i++)
	{
		if (uvMC2_SendDevFaultReset(ENG_MMDI(pstCfg->axis_id[i])))	return FALSE;
	}
	 
#if(MC2_DRIVE_2SET == 1)
	LPG_CMSI pstCfg2 = &GetConfig()->mc2b_svc;

	UINT8 u8drv_id;
	for (i=0; i < pstCfg2->drive_count; i++)
	{
		u8drv_id = i + DRIVEDIVIDE;
		if (uvMC2_SendDevFaultReset(ENG_MMDI(u8drv_id)))	return FALSE;
	}
#endif

	return TRUE;
}

/*
 desc : 모든 Device Homing All (Concurrency : Used where EN_STOP mode is enabled.) (0x3808)
 parm : None
 retn : TRUE or FALSE
 note : EN_STOP 모드가 적용된 곳에 사용
*/
API_EXPORT BOOL uvEng_MC2_SendDevHomingAllEx()
{
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;

	return uvMC2_SendDevHomingAll();
}

/*
 desc : Motor Drive에 대해 Homing
 parm : drv_id	- [in]  MC2 (SDS) Drive ID
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MC2_SendDevHoming(ENG_MMDI drv_id)
{
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;
	if (!IsMC2DriveValid(drv_id))	return FALSE;

	return uvMC2_SendDevHoming(drv_id);
}

/*
 desc : 모든 Device Fault Reset All (Concurrency : Used where EN_STOP mode is enabled.) (0x3808)
 parm : None
 retn : TRUE or FALSE
 note : EN_STOP 모드가 적용된 곳에 사용
*/
API_EXPORT BOOL uvEng_MC2_SendDevFaultResetAllEx()
{
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;

	return uvMC2_SendDevFaultResetAll();
}

/*
 desc : All Device Fault Reset (Error Init) - Only drives in which errors occur sequentially are reset.
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MC2_SendDevFaultResetCheckAll()
{
	UINT8 i	= 0x00;
	LPG_CMSI pstCfg	= &GetConfig()->mc2_svc;

	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;

	for (; i<pstCfg->drive_count; i++)
	{
		if (!GetShMemMC2()->IsDriveError(pstCfg->axis_id[i]))		continue;
		if (uvMC2_SendDevFaultReset(ENG_MMDI(pstCfg->axis_id[i])))	return FALSE;
	}

#if(MC2_DRIVE_2SET == 1)
	LPG_CMSI pstCfg2 = &GetConfig()->mc2b_svc;

	UINT8 u8drv_id;
	for (i=0; i < pstCfg2->drive_count; i++)
	{
		u8drv_id = i + DRIVEDIVIDE;
		if (!GetShMemMC2b()->IsDriveError(pstCfg2->axis_id[i]))		continue;
		if (uvMC2_SendDevFaultReset(ENG_MMDI(u8drv_id)))	return FALSE;
	}
#endif


	return TRUE;
}

/*
 desc : Luria가 현재 노광 (Printing) 중인지 여부에 따라,
		MC2에게 주기적으로 통신 데이터 요청 여부 설정
 parm : enable	- [in]  TRUE : 요청 진행, FALSE : 요청 중지 (아예 요청 중지가 아님)
 retn : None
*/
API_EXPORT VOID uvEng_MC2_SetSendPeriodPkt(BOOL enable)
{
	uvMC2_SetSendPeriodPkt(enable);
}

/*
 desc : MC2에게 주기적으로 패킷 데이터를 요청하는지 여부 반환
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MC2_IsSendPeriodPkt()
{
	return uvMC2_IsSendPeriodPkt();
}

/*
 desc : Focus Motor Move Absolute Position (모든 Photohead의 Z Axis을 기본 위치로 이동)
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MC2_ReqSetPhMotorFocusMove(ENG_MMDI drv_id)
{
	BOOL bSucc	= TRUE;

	if (!IsMC2DriveValid(drv_id))	return FALSE;

	UINT8 u8DrvID	= UINT8(drv_id) - UINT8(ENG_MMDI::en_axis_ph1);
	INT32 i32Move	= (INT32)ROUNDED(GetConfig()->luria_svc.ph_z_focus[u8DrvID] * 10000.0f, 0);
	//UINT32 u32Velo = (UINT32)ROUNDED(GetConfig()->mc2b_svc.max_velo[u8DrvID] * 10000.0f, 0);	/* mm -> 0.1 um or 100 nm */
	UINT32 u32Velo = (UINT32)ROUNDED(GetConfig()->mc2b_svc.move_velo * 10000.0f, 0);	/* mm -> 0.1 um or 100 nm */

	/* Photohead Z Axis인지 검사 */
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	if (drv_id != ENG_MMDI::en_axis_ph1 && 
		drv_id != ENG_MMDI::en_axis_ph2 &&
		drv_id != ENG_MMDI::en_axis_ph3 &&
		drv_id != ENG_MMDI::en_axis_ph4 &&
		drv_id != ENG_MMDI::en_axis_ph5 &&
		drv_id != ENG_MMDI::en_axis_ph6)
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	if (drv_id != ENG_MMDI::en_axis_ph1 &&
		drv_id != ENG_MMDI::en_axis_ph2 )
#endif
		
	{
		LOG_WARN(ENG_EDIC::en_mc2, L"Not a Photohead Z Axis drive.");
		return FALSE;
	}
	/* 기준 소재 높이 값에 따른 Photohead Z 축 기본 높이로 이동 */
	return uvMC2_SendDevAbsMove(drv_id, i32Move, u32Velo);
}

/*
 desc : 모든 Photohead Z 축이 Focus 위치로 이동 했는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_MC2_IsAllPhMotorFocusMoved()
{
	UINT8 i			= 0;
	INT32 i32Set	= 0, i32Get = 0;

	/* 현재 DEMO 모드로 동작 중인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;
	/* Photohead 개수만큼 기본 위치 이동 완료 여부 확인 */
	for (; i<GetConfig()->luria_svc.ph_count; i++)
	{
		/* 해당 Photohead의 Z Axis이 Base 위치로 이동이 완료 되었는지 여부 */
		i32Set	= (INT32)ROUNDED(GetConfig()->luria_svc.ph_z_focus[i]*1000.0f, 0);

		i32Get = (INT32)GetShMemMC2b()->act_data[i].real_position;

		if (abs(i32Set - i32Get) > 10 /* 10 -> 1 um */)	return FALSE;
	}

	return TRUE;
}

/*
 desc : Move the motor to absoule position (Up, Down, Left, Bottom)
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
		move	- [in]  position value to move the motor (unit: mm) (It is valid up to 4 decimal places)
		velo	- [in]  Speed value (unit: mm /sec) (It is valid up to 4 decimal places)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MC2_SendDevAbsMove(ENG_MMDI drv_id, DOUBLE move, DOUBLE velo)
{
	INT32 i32Move	= (INT32)ROUNDED(move * 10000.0f, 0);	/* mm -> 0.1 um or 100 nm */
	UINT32 u32Velo	= (UINT32)ROUNDED(velo * 10000.0f, 0);	/* mm -> 0.1 um or 100 nm */

	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;
	if (!IsMC2DriveValid(drv_id))	return FALSE;

// 	/* Check if the motor shift value is out of range */
// 	if (GetConfig()->mc2_svc.max_dist[UINT8(drv_id)] < move ||
// 		GetConfig()->mc2_svc.min_dist[UINT8(drv_id)] > move)
// 	{
// 		LOG_WARN(ENG_EDIC::en_mc2, L"The movement range of the motor is out of range");
// 		return FALSE;
// 	}

	return uvMC2_SendDevAbsMove(drv_id, i32Move, u32Velo);
}
API_EXPORT BOOL uvEng_MC2_SendDevAbsMoveExt(ENG_MMDI drv_id, DOUBLE move)
{
	DOUBLE dbVelo	= GetConfig()->mc2_svc.max_velo[UINT8(drv_id)];
	return uvEng_MC2_SendDevAbsMove(drv_id, move, dbVelo);
}

/*
 desc : Move the motor to a relative position (Up, Down, Left, Bottom)
 parm : method	- [in]  ENG_MMMM (Jog, Step, Abs: Absolute Position)
		drv_id	- [in]  Motor Drive ID (SD2 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
		direct	- [in]  Move direct (Up / Down / Left / Right ...)
		dist	- [in]  Move distance (unit: mm) (It is valid up to 4 decimal places)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MC2_SendDevRefMove(ENG_MMMM method, ENG_MMDI drv_id,
										 ENG_MDMD direct, DOUBLE dist)
{
	INT32 i32Dist	= (INT32)ROUNDED(abs(dist) * 10000.0f, 0);	/* 반드시 양수 값 abs(dis) */

	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;
	if (!IsMC2DriveValid(drv_id))	return FALSE;

	UINT32 u32Velo = (UINT32)ROUNDED(GetConfig()->mc2_svc.move_velo * 10000.0f, 0);	/* mm -> 0.1 um or 100 nm */


	return uvMC2_SendDevRefMove(method, drv_id, direct, i32Dist, u32Velo);
}

/*
 desc : Luria Expose Mode로 설정
 parm : drv_id	- [in]  노광 방향의 축 Drive ID
		mode	- [in]  Area (Align) mode or Expose mode (ENG_MTAE)
						Area Mode			: 절대 스테이지가 움직이지 않음
						Expo Mode			: 현재 위치 값보다 Min 값이 작으면, 움직이지 않으며, 나머지는 Min 값으로 이동 함
		pixel	- [in]  Pixel Size (unit: mm)
		begin	- [in]  노광 시작 위치 (단위: mm)
		end		- [in]  노광 종료 위치 (단위: mm)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MC2_SendDevLuriaMode(ENG_MMDI drv_id, ENG_MTAE mode,
										   DOUBLE pixel, DOUBLE begin, DOUBLE end)
{
	UINT32 u32Pixel	= (UINT32)ROUNDED(pixel *	1000000000.0f, 0);
	INT32 i32Begin	= (UINT32)ROUNDED(begin *	1000000000.0f, 0);
	INT32 i32End	= (UINT32)ROUNDED(end *		1000000000.0f, 0);

	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;
	if (!IsMC2DriveValid(drv_id))	return FALSE;

	return uvMC2_SendDevLuriaMode(drv_id, mode, u32Pixel, i32Begin, i32End);
}

/*
 desc : Reference Data 전체 요청
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MC2_ReadReqPktRefAll()
{
	return uvMC2_ReadReqPktRefAll();
}

/*
 desc : Active Data 전체 요청
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MC2_ReadReqPktActAll()
{
	return uvMC2_ReadReqPktActAll();
}

/*
 desc : Device Control - Reference Header - for EN_STOP and HEART_BEAT
 parm : en_stop	- [in]  ENG_MMSM (0x00:Only Homing operation or 0x01: Other operation)
		beat	- [in]  Toggle (0 or 1) (Enter the opposite value based on the current value)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MC2_SetRefHeadEnStop(ENG_MMST en_stop, UINT8 beat)
{
	return uvMC2_SetRefHeadMControl(en_stop, beat);
}
API_EXPORT BOOL uvEng_MC2_SetRefHeadEnStopEx(ENG_MMST en_stop)
{
	return uvMC2_SetRefHeadMControlEx(en_stop);
}

/* --------------------------------------------------------------------------------------------- */
/*                              외부 함수 - < MC2 > < for Common >                               */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : MC2 Server에 연결 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_MC2_IsConnected()
{
	if (!GetConfig())	return FALSE;
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;

#if (MC2_DRIVE_2SET == 0)
	return GetShMemMC2()->link.is_connected;
#elif(MC2_DRIVE_2SET == 1)
	UINT8 Conn1, Conn2;
	Conn1 = GetShMemMC2()->link.is_connected;
	Conn2 = GetShMemMC2b()->link.is_connected;
	LPG_MDSM MC2 = GetShMemMC2();
	LPG_MDSM MC2b = GetShMemMC2b();

	return GetShMemMC2()->link.is_connected && GetShMemMC2b()->link.is_connected;
#endif

}

/*
 desc : 현재 모든 Drive의 Motor가 중지 상태인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_MC2_IsMotorDriveStopAll()
{
	UINT8 i	= 0x00;
	LPG_MDSM pstAct	= GetShMemMC2();
	LPG_CMSI pstCfg	= &GetConfig()->mc2_svc;

	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;
	for (; i<pstCfg->drive_count; i++)
	{
		if (pstAct->IsDriveBusy(pstCfg->axis_id[i]))	return FALSE;
	}

#if (MC2_DRIVE_2SET == 1)
	UINT8 u8drv_id;
	LPG_MDSM pstAct2 = GetShMemMC2b();
	for (; i < GetConfig()->mc2b_svc.drive_count; i++)
	{
		u8drv_id = i + DRIVEDIVIDE;
		if (pstAct2->IsDriveBusy(u8drv_id))	return FALSE;
	}

#endif
	return TRUE;
}

/*
 desc : MC2에서 동작 중인 Drive (SD2S) 중 한 개라도 에러가 발생 했는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_MC2_IsAnyDriveError()
{
	return uvMC2_IsAnyDriveError();
}

/*
 desc : 현재 Motor Drive의 EN_STOP 값 반환
 parm : None
 retn : TRUE (EN_STOP == yes) or FALSE (EN_STOP == no)
*/
API_EXPORT BOOL uvCmn_MC2_IsDriveEnStop()
{
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;
	return GetShMemMC2()->ref_head.mc_en_stop ? TRUE : FALSE;
}

/*
 desc : 현재 Motor Drive의 Heart Beat 값 반환
 parm : None
 retn : 0 or 1
*/
API_EXPORT UINT8 uvCmn_MC2_GetRefHeartBeat()
{
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;
	return GetShMemMC2()->ref_head.mc_heart_beat;
}

/*
 desc : 현재 Motor Drive의 V_SCALE 값 반환
 parm : None
 retn : scale 값 (0 ~ 100 %)
*/
API_EXPORT UINT8 uvCmn_MC2_GetRefVScale()
{
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;
	return GetShMemMC2()->ref_head.v_scale;
}

/*
 desc : 현재 Motor Drive의 OUTPUT S값 반환
 parm : None
 retn : scale 값 (0 ~ 100 %)
*/
API_EXPORT UINT64 uvCmn_MC2_GetRefOutputs()
{
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;
	return GetShMemMC2()->ref_head.digital_outputs;
}

/*
 desc : Check whether an error has occurred in the any motor drive
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) that is, 0x00, 0x01, 0x04, 0x05
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_MC2_IsDriveError(ENG_MMDI drv_id)
{
	auto cfg = GetConfig();
	if (cfg == nullptr) return FALSE;
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return FALSE;
	if (!IsMC2DriveValid(drv_id))	return FALSE;

#if (MC2_DRIVE_2SET == 0)
	return GetShMemMC2()->IsDriveError(GetConfig()->mc2_svc.axis_id[UINT8(drv_id)]);
#elif(MC2_DRIVE_2SET == 1)
	UINT8 u8drv_id;
	if ((UINT8)drv_id < DRIVEDIVIDE)
	{
		//return GetShMemMC2()->IsDriveError(GetConfig()->mc2_svc.axis_id[UINT8(drv_id)]);
		return GetShMemMC2()->IsDriveError(UINT8(drv_id));
	}
	else
	{
		u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
		//return GetShMemMC2b()->IsDriveError(GetConfig()->mc2b_svc.axis_id[u8drv_id]);
		return GetShMemMC2b()->IsDriveError(u8drv_id);
	}


#endif
}
API_EXPORT BOOL uvCmn_MC2_IsDriveErrorAll()
{
	UINT8 i	= 0x00;

	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return FALSE;
	for (; i<GetConfig()->mc2_svc.drive_count; i++)
	{
		if (GetShMemMC2()->IsDriveError(GetConfig()->mc2_svc.axis_id[i]))	return TRUE;
	}

#if (MC2_DRIVE_2SET == 1)
	for (i=0; i < GetConfig()->mc2b_svc.drive_count; i++)
	{
		if (GetShMemMC2b()->IsDriveError(GetConfig()->mc2b_svc.axis_id[i]))	return TRUE;
	}
#endif

	return FALSE;
}

/*
 desc : MC2에서 DNC Error가 발생 했는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_MC2_IsDNCError()
{
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return FALSE;

#if (MC2_DRIVE_2SET == 0)
	return GetShMemMC2()->GetDNCError() > 0;
#elif(MC2_DRIVE_2SET == 1)
	return (GetShMemMC2()->GetDNCError() > 0) || (GetShMemMC2b()->GetDNCError() > 0);
#endif

}

/*
 desc : MC2에서 동작 중인 Drive (SD2S) 중 한 개라도 BUSY 상태인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_MC2_IsAnyDriveBusy()
{
	UINT8 i	= 0x00;
	LPG_MDSM pstAct	= GetShMemMC2();
	LPG_CMSI pstCfg	= &GetConfig()->mc2_svc;

	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return FALSE;
	for (; i< GetConfig()->mc2_svc.drive_count; i++)
	{
		if (pstAct->IsDriveBusy(pstCfg->axis_id[i]))	return TRUE;
	}

#if(MC2_DRIVE_2SET == 1)
	LPG_MDSM pstAct2 = GetShMemMC2b();
	LPG_CMSI pstCfg2 = &GetConfig()->mc2b_svc;

	for (i=0; i < GetConfig()->mc2b_svc.drive_count; i++)
	{
		if (pstAct2->IsDriveBusy(pstCfg2->axis_id[i]))	return TRUE;
	}
#endif

	return FALSE;
}

/*
 desc : MC2에서 동작 중인 임의 Drive (SD2S)가 Busy 상태인지 여부
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
 retn : TRUE (Busy) or FALSE (Idle)
*/
API_EXPORT BOOL uvCmn_MC2_IsDriveBusy(ENG_MMDI drv_id)
{
	LPG_MDSM pstAct	= GetShMemMC2();

	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return FALSE;
	if (!IsMC2DriveValid(drv_id))	return FALSE;
	
#if (MC2_DRIVE_2SET == 0)
	return pstAct->IsDriveBusy(GetConfig()->mc2_svc.axis_id[UINT8(drv_id)]);
#elif(MC2_DRIVE_2SET == 1)
	LPG_MDSM pstAct2 = GetShMemMC2b();
	UINT8 u8drv_id;

	if ((UINT8)drv_id < DRIVEDIVIDE)
	{
		//return pstAct->IsDriveBusy(GetConfig()->mc2_svc.axis_id[UINT8(drv_id)]);
		return pstAct->IsDriveBusy(UINT8(drv_id));
	}
	else
	{
		u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
		//return pstAct2->IsDriveBusy(GetConfig()->mc2b_svc.axis_id[u8drv_id]);
		return pstAct->IsDriveBusy(u8drv_id);
	}

#endif

}

/*
 desc : 현재 임의 모션 드라이브가 이동 중인지 여부
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_MC2_IsDriveMoving(ENG_MMDI drv_id)
{
	LPG_MDSM pstAct	= GetShMemMC2();

	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return FALSE;
	if (!IsMC2DriveValid(drv_id))	return FALSE;
	
#if (MC2_DRIVE_2SET == 0)
	return !pstAct->IsDriveBusy(GetConfig()->mc2_svc.axis_id[UINT8(drv_id)]);
#elif(MC2_DRIVE_2SET == 1)
	LPG_MDSM pstAct2 = GetShMemMC2b();
	UINT8 u8drv_id;

	if ((UINT8)drv_id < DRIVEDIVIDE)
	{
		//return !pstAct->IsDriveBusy(GetConfig()->mc2_svc.axis_id[UINT8(drv_id)]);
		 return !pstAct->IsDriveBusy(UINT8(drv_id));
	}
	else
	{
		u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
		//return !pstAct->IsDriveBusy(GetConfig()->mc2b_svc.axis_id[u8drv_id]);
		return !pstAct2->IsDriveBusy(u8drv_id);
	}

#endif

}

/*
 desc : 현재 MC2에서 동작 (Busy) 중인 Drive의 번호 반환
 parm : None
 retn : drive number (0 ~ 7), 없다면 0xff
 note : 여러 개가 Busy 상태일지라도, 가장 먼저 Busy인 드라이브 번호만 반환
*/
API_EXPORT UINT8 uvCmn_MC2_GetFirstBusyDriveNo()
{
	UINT8 i	= 0x00;
	LPG_MDSM pstAct	= GetShMemMC2();
	LPG_CMSI pstCfg	= &GetConfig()->mc2_svc;

	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return FALSE;
	for (; i<pstCfg->drive_count; i++)
	{
		if (pstAct->IsDriveBusy(pstCfg->axis_id[i]))	return pstCfg->axis_id[i];
	}

#if(MC2_DRIVE_2SET == 1)
	LPG_MDSM pstAct2 = GetShMemMC2b();
	LPG_CMSI pstCfg2 = &GetConfig()->mc2b_svc;

	for (; i < pstCfg2->drive_count; i++)
	{
		if (pstAct2->IsDriveBusy(pstCfg2->axis_id[i]))	return pstCfg2->axis_id[i];
	}
#endif

	return 0xff;
}

/*
 desc : 현재 임의 Motor Drive의 토글 값 얻기
 parm : drv_id	- [in]  Toggle 값 얻기 위한 드라이브 ID (0x00 ~ 0x07)
 retn : None
*/
API_EXPORT VOID uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI drv_id)
{
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return;
	if (!IsMC2DriveValid(drv_id))	return;

#if (MC2_DRIVE_2SET == 0)
	g_u8DrvDoneToggled[(UINT8)drv_id] = GetShMemMC2()->GetDoneToggled((UINT8)drv_id);
	g_u8DrvCmdToggled[(UINT8)drv_id] = GetShMemMC2()->GetCmdToggled((UINT8)drv_id);
#elif(MC2_DRIVE_2SET == 1)
	UINT8 u8drv_id;

	if ((UINT8)drv_id < DRIVEDIVIDE)
	{
		g_u8DrvDoneToggled[(UINT8)drv_id] = GetShMemMC2()->GetDoneToggled((UINT8)drv_id);
		g_u8DrvCmdToggled[(UINT8)drv_id] = GetShMemMC2()->GetCmdToggled((UINT8)drv_id);
	}
	else
	{
		u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
		g_u8DrvDoneToggled[(UINT8)drv_id] = GetShMemMC2b()->GetDoneToggled(u8drv_id);
		g_u8DrvCmdToggled[(UINT8)drv_id] = GetShMemMC2b()->GetCmdToggled(u8drv_id);
	}

#endif

}

/*
 desc : 임의 Motor Drive의 Toggled 값이 이전 Toggled 값과 다른지 여부 비교
 parm : drv_id	- [in]  Toggle 값 얻기 위한 드라이브 ID (0x00 ~ 0x07)
 retn : TRUE (이전 값과 다르다) or FALSE (이전 값과 동일하다)
*/
API_EXPORT BOOL uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI drv_id)
{
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;
	if (!IsMC2DriveValid(drv_id))	return FALSE;


#if (MC2_DRIVE_2SET == 0)
	if (g_u8DrvDoneToggled[(UINT8)drv_id] == GetShMemMC2()->GetDoneToggled((UINT8)drv_id))	return FALSE;
	if (g_u8DrvCmdToggled[(UINT8)drv_id] == GetShMemMC2()->GetCmdToggled((UINT8)drv_id))	return FALSE;
#elif(MC2_DRIVE_2SET == 1)
	UINT8 u8drv_id;

	if ((UINT8)drv_id < DRIVEDIVIDE)
	{
		if (g_u8DrvDoneToggled[(UINT8)drv_id] == GetShMemMC2()->GetDoneToggled((UINT8)drv_id))	return FALSE;
		if (g_u8DrvCmdToggled[(UINT8)drv_id] == GetShMemMC2()->GetCmdToggled((UINT8)drv_id))	return FALSE;
	}
	else
	{
		u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
		if (g_u8DrvDoneToggled[(UINT8)drv_id] == GetShMemMC2b()->GetDoneToggled(u8drv_id))	return FALSE;
		if (g_u8DrvCmdToggled[(UINT8)drv_id] == GetShMemMC2b()->GetCmdToggled(u8drv_id))	return FALSE;
	}
#endif
	return TRUE;
}

/*
 desc : 모든 Motor Drive의 토글 값 얻기
 parm : None
 retn : None
*/
API_EXPORT VOID uvCmn_MC2_GetDrvDoneToggledAll()
{
	UINT8 i	= 0x00, j = 0x00, u8DrvCount = GetConfig()->mc2_svc.drive_count;

	for (; i<MAX_MC2_DRIVE; i++)
	{
		if (i == GetConfig()->mc2_svc.axis_id[j])
		{
			g_u8DrvDoneToggled[i]	= GetShMemMC2()->GetDoneToggled(i);
			g_u8DrvCmdToggled[i]	= GetShMemMC2()->GetCmdToggled(i);
			j++;
		}
	}
}

/*
 desc : 모든 Motor Drive의 Toggled 값이 이전 Toggled 값과 다른지 여부 비교
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_MC2_IsDrvDoneToggledAll()
{
	UINT8 i	= 0x00, j = 0x00, u8DrvCount = GetConfig()->mc2_svc.drive_count;

	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;

	for (; i<MAX_MC2_DRIVE; i++)
	{
		if (i == GetConfig()->mc2_svc.axis_id[j])
		{
			if (g_u8DrvDoneToggled[j] == GetShMemMC2()->GetDoneToggled(j))	return FALSE;
			if (g_u8DrvCmdToggled[j] == GetShMemMC2()->GetCmdToggled(j))	return FALSE;
			j++;
		}
	}

	return TRUE;
}

/*
 desc : Returns the absolute position of the current motion drive
 parm : drv_id	- [in]  Motion drive ID (0x00 ~ 0x07)
 retn : position (unit: mm)
*/
API_EXPORT DOUBLE uvCmn_MC2_GetDrvAbsPos(ENG_MMDI drv_id)
{
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return 0.0f;
	if (!IsMC2DriveValid(drv_id))	return 0.0f;

#if (MC2_DRIVE_2SET == 0)
	return DOUBLE(g_pMemMC2->GetMemMap()->act_data[(UINT8)drv_id].real_position / 10000.0f);
#elif(MC2_DRIVE_2SET == 1)
	UINT8 u8drv_id;

	if ((UINT8)drv_id < DRIVEDIVIDE)
	{
		return DOUBLE(g_pMemMC2->GetMemMap()->act_data[(UINT8)drv_id].real_position / 10000.0f);
	}
	else
	{
		u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
		return DOUBLE(g_pMemMC2b->GetMemMap()->act_data[u8drv_id].real_position / 10000.0f);
	}

#endif

}

/*
 desc : 모든 드라이브의 상태 값 저장 (에러 포함)
 parm : None
 retn : None
*/
API_EXPORT VOID uvCmn_MC2_SaveAllDriveState()
{
	UINT8 i = 0x00, u8Drv		= 0x00;
	TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL};
	LPG_AVCD pstState	= GetShMemMC2()->act_data;

	for (; i<GetConfig()->mc2_svc.drive_count; i++)
	{
		u8Drv	= GetConfig()->mc2_svc.axis_id[i];
		swprintf_s(tzMesg, LOG_MESG_SIZE, L"<STAGE_X>"
										  L"done:%d, cal:%d, on:%d, err:%d, msg:%d, busy:%d,"
										  L"zero:%d, inpos:%d, varc:%d, init:%d, done_toggle:%d,"
										  L"error_cd:%d",
				   pstState[u8Drv].flag_done, pstState[u8Drv].flag_cal,			pstState[u8Drv].flag_on,
				   pstState[u8Drv].flag_err,  pstState[u8Drv].flag_msg,			pstState[u8Drv].flag_busy,
				   pstState[u8Drv].flag_zero, pstState[u8Drv].flag_inpos,		pstState[u8Drv].flag_varc,
				   pstState[u8Drv].flag_init, pstState[u8Drv].flag_done_toggle, pstState[u8Drv].error);

		/* 로그 파일 저장 */
		LOG_ERROR(ENG_EDIC::en_mc2, tzMesg);
	}

#if (MC2_DRIVE_2SET == 1)
	for (; i < GetConfig()->mc2b_svc.drive_count; i++)
	{
		LPG_AVCD pstState2 = GetShMemMC2b()->act_data;

		u8Drv = GetConfig()->mc2b_svc.axis_id[i];
		swprintf_s(tzMesg, LOG_MESG_SIZE, L"<STAGE_X>"
			L"done:%d, cal:%d, on:%d, err:%d, msg:%d, busy:%d,"
			L"zero:%d, inpos:%d, varc:%d, init:%d, done_toggle:%d,"
			L"error_cd:%d",
			pstState2[u8Drv].flag_done, pstState2[u8Drv].flag_cal, pstState2[u8Drv].flag_on,
			pstState2[u8Drv].flag_err, pstState2[u8Drv].flag_msg, pstState2[u8Drv].flag_busy,
			pstState2[u8Drv].flag_zero, pstState2[u8Drv].flag_inpos, pstState2[u8Drv].flag_varc,
			pstState2[u8Drv].flag_init, pstState2[u8Drv].flag_done_toggle, pstState2[u8Drv].error);

		/* 로그 파일 저장 */
		LOG_ERROR(ENG_EDIC::en_mc2, tzMesg);
	}
#endif

}

/*
 desc : Check whether the motor drive is in homing state
 parm : drv_id	- [in]  motor drive id
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_MC2_IsDriveHomed(ENG_MMDI drv_id)
{
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;
	if (!IsMC2DriveValid(drv_id))	return FALSE;
#if (MC2_DRIVE_2SET == 0)
	return GetShMemMC2()->act_data[UINT8(drv_id)].IsCalibrated() ? TRUE : FALSE;
#elif(MC2_DRIVE_2SET == 1)
	if ((UINT8)drv_id < DRIVEDIVIDE)
	{
		return GetShMemMC2()->act_data[UINT8(drv_id)].IsCalibrated() ? TRUE : FALSE;
	}
	else
	{
		UINT8 u8drv_id;
		u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
		return GetShMemMC2b()->act_data[u8drv_id].IsCalibrated() ? TRUE : FALSE;
	}

#endif

}
API_EXPORT BOOL uvCmn_MC2_IsDriveHomedAll()
{
	UINT8 i	= 0x00;
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;
	
	for (; i<GetConfig()->mc2_svc.drive_count; i++)
	{
		if (!GetShMemMC2()->act_data[GetConfig()->mc2_svc.axis_id[i]].IsCalibrated())	return FALSE;
	}
#if (MC2_DRIVE_2SET == 1)
	for (; i < GetConfig()->mc2_svc.drive_count; i++)
	{
		if (!GetShMemMC2b()->act_data[GetConfig()->mc2b_svc.axis_id[i]].IsCalibrated())	return FALSE;
	}
#endif

	return TRUE;
}

/*
 desc : 특정 모션 드라이브의 축에 해당되는 에러 값 반환
 parm : drv_id	- [in]  모션 드라이브 ID
 retn : 에러 코드 값 반환
*/
API_EXPORT UINT16 uvCmn_MC2_GetDriveError(ENG_MMDI drv_id)
{
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return 0x0000;
	if (!IsMC2DriveValid(drv_id))	return 0x0000;
	
#if (MC2_DRIVE_2SET == 0)
	return GetShMemMC2()->GetErrorCodeOne(UINT8(drv_id));
#elif(MC2_DRIVE_2SET == 1)
	UINT8 u8drv_id;

	if ((UINT8)drv_id < DRIVEDIVIDE)
	{
		return GetShMemMC2()->GetErrorCodeOne(UINT8(drv_id));
	}
	else
	{
		u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
		return GetShMemMC2b()->GetErrorCodeOne(u8drv_id);
	}

#endif

}

/*
 desc : 특정 모션 드라이브의 축에 해당되는 Result 값 반환
 parm : drv_id	- [in]  모션 드라이브 ID
		flag	- [in]  0x01 : Result1, 0x02 : Result2
 retn : Result 1 or 2 값 반환
*/
API_EXPORT INT32 uvCmn_MC2_GetDriveResult(ENG_MMDI drv_id, UINT8 flag)
{
	LPG_AVCD pstActData;

	if ((UINT8)drv_id < DRIVEDIVIDE)
	{
		pstActData = &GetShMemMC2()->act_data[UINT8(drv_id)];
	}
	else
	{
		UINT8 u8drv_id;
		u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
		pstActData = &GetShMemMC2b()->act_data[u8drv_id];
	}


	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return 0x0000;
	if (!IsMC2DriveValid(drv_id))	return 0x0000;
	return flag == 0x01 ? pstActData->result1 : pstActData->result2;
}

/*
 desc : MC2에서 동작 중인 Drive (SD2S)의 상태 값 반환
 parm : drv_id	- [in]  모션 드라이브 ID
		flag	- [in]  요청 플래그 값
 retn : 상태 값 반환
*/
API_EXPORT UINT8 uvCmn_MC2_GetDriveStatus(ENG_MMDI drv_id, ENG_SADS flag)
{
	LPG_AVCD pstActData;
	if ((UINT8)drv_id < DRIVEDIVIDE)
	{
		pstActData = &GetShMemMC2()->act_data[UINT8(drv_id)];
	}
	else
	{
		UINT8 u8drv_id;
		u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
		pstActData = &GetShMemMC2b()->act_data[u8drv_id];
	}
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return 0x00;
	if (!IsMC2DriveValid(drv_id))	return 0x00;

	switch (flag)
	{
	case ENG_SADS::en_flag_done			:	return pstActData->flag_done;
	case ENG_SADS::en_flag_cal			:	return pstActData->flag_cal;
	case ENG_SADS::en_flag_on			:	return pstActData->flag_on;
	case ENG_SADS::en_flag_err			:	return pstActData->flag_err;
	case ENG_SADS::en_flag_msg			:	return pstActData->flag_msg;
	case ENG_SADS::en_flag_busy			:	return pstActData->flag_busy;
	case ENG_SADS::en_flag_zero			:	return pstActData->flag_zero;
	case ENG_SADS::en_flag_inpos		:	return pstActData->flag_inpos;
	case ENG_SADS::en_flag_varc			:	return pstActData->flag_varc;
	case ENG_SADS::en_flag_init			:	return pstActData->flag_init;
	case ENG_SADS::en_flag_done_toggle	:	return pstActData->flag_done_toggle;
	}
	return 0x00;
}

/*
 desc : MC2에서 동작 중인 Drive (SD2S)에서 Actual Header중 MState 값 반환
 parm : heart_no	- [in]  Heart Beat Number (1 0r 2)
 retn : 0 or 1 값 반환
*/
API_EXPORT UINT8 uvCmn_MC2_GetActState(UINT8 heart_no)
{
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return 0x00;

	return heart_no == 1 ? GetShMemMC2()->act_head.ms_heart_beat1 :
						   GetShMemMC2()->act_head.ms_heart_beat2;
}

/*
 desc : MC2에서 동작 중인 Drive (SD2S)에서 Actual Header중 MError 값 반환
 parm : None
 retn : 0 or Error 값 반환
*/
API_EXPORT UINT16 uvCmn_MC2_GetActError()
{
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return 0x0000;

	return GetShMemMC2()->act_head.merror;
}

/*
 desc : MC2에서 동작 중인 Drive (SD2S)에서 Actual Header중 Inputs or Output 값 반환
 parm : type	- [in]  0x00 : Inputs, 0x01 : Outputs
 retn : 값 반환
*/
API_EXPORT UINT64 uvCmn_MC2_GetActInOut(UINT8 type)
{
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return 0x0000000000000000;

	return type == 0x00 ? GetShMemMC2()->act_head.inputs : GetShMemMC2()->act_head.outputs;
}

/*
 desc : 각 드라이브의 서보의 Locked 상태 반환 (On or Off)
 parm : drv_id	- [in]  SD2 Driver ID
 retn : TRUE (Servo On) or FALSE (Servo Off)
*/
API_EXPORT BOOL uvCmn_MC2_IsDevLocked(ENG_MMDI drv_id)
{
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;
	if (!IsMC2DriveValid(drv_id))	return FALSE;

#if (MC2_DRIVE_2SET == 0)
	return GetShMemMC2()->GetErrorCodeOne(UINT8(drv_id));
#elif(MC2_DRIVE_2SET == 1)
	UINT8 u8drv_id;

	if ((UINT8)drv_id < DRIVEDIVIDE)
	{
		return GetShMemMC2()->act_data[UINT8(drv_id)].IsServoOn();
	}
	else
	{
		u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
		return GetShMemMC2b()->act_data[u8drv_id].IsServoOn();
	}
#endif
	
}

/*
 desc : Returns whether the servo status of all motor drives is locked
 parm : None
 retn : TRUE (All Servo On) or FALSE (All Servo Off)
*/
API_EXPORT BOOL uvCmn_MC2_IsAllDevLocked()
{
	UINT8 i	= 0x00;
	/* Check if it is in demo operation mode */
	if (GetConfig()->IsRunDemo())	return TRUE;
	/* 모든 드라이브에 대한 Homing 작업 수행 */
	for (; i<GetConfig()->mc2_svc.drive_count; i++)
	{
		if (!GetShMemMC2()->act_data[GetConfig()->mc2_svc.axis_id[i]].IsServoOn())	return FALSE;
	}
	return TRUE;
}


#ifdef __cplusplus
}
#endif