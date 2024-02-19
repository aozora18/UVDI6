
/*
 desc : Align Camera Calibration Library
*/

#pragma once

#include "../../inc/conf/global.h"
#include "../../inc/conf/define.h"
#include "../conf/conf_podis.h"

/* --------------------------------------------------------------------------------------------- */
/*                                           외부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : Align Camera Calibration 초기화
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvACamCali_Init(LPG_CIEA config);
/*
 desc : Align Camera Calibration 해제
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT VOID uvACamCali_Close();
/*
 desc : Calibration Table (Position) Data 적재
 parm : frame_rate	- [in]  현재 노광하려는 동작 속도 즉, frame_rate (* 1000) 값
		set_align	- [in]  Camera Align Setup 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvACamCali_LoadCaliPos(UINT16 frame_rate, LPG_CSAI set_align);
/*
 desc : Global Fiducial의 X 좌표 간의 오차 즉, 1 & 2, 3 & 4 Mark 간의 거버 상에서 X 좌표 간의 오차 값
 parm : cam_id	- [in]  Camera Index (1 or 2)
		mark_no	- [in]  저장될 Mark Number (Zero based)
		offset	- [in]  X Offset 값
 retn : None
*/
API_IMPORT VOID uvACamCali_SetAlignACamVertX(UINT8 cam_id, UINT8 mark_no, INT32 offset);
/*
 desc : 현재 4개 (Global인 경우) Mark 위치에 대해, 거버 상의 X 오차 값 (1 & 2, 3 & 4) 반환
 parm : cam_id	- [in]  Camera Index (1 or 2)
		mark_no	- [in]  Zero based (0 ~ 3)
 retn : offset 값 (단위: 0.1 um or 100 nm)
*/
API_IMPORT INT32 uvACamCali_GetAlignACamVertX(UINT8 cam_id, UINT8 mark_no);
/*
 desc : 각 드라이브 별 즉, 축 (Axis) 별 Align Mark가 존재하는 모션의 실제 위치 값 반환
 parm : cam_id	- [in]  대상 Align Camera Index (1 or 2)
		axis	- [in]  0x00 : X Axis (Camera X), 0x01 : Y Axis (Stage Y)
		idx		- [in]  Mark 위치 정보가 저장된 위치 (인덱스) 즉, Camera 별 Grabbed Image와 동일
 retn : 모션의 위치 값 반환 (검색 실패의 경우, 0 값 반환)
*/
API_IMPORT INT32 uvACamCali_GetMarkPosXY(UINT8 cam_id, UINT8 axis, UINT8 idx);
/*
 desc : 원하는 위치에 근사한 Align Camera의 Calibration 오차 값 반환
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
		a_cam_x	- [in]  이동하고자 하는 Align Camera의 Drive Position (절대 값. 단위: 0.1 um or 100 nm)
		stage_y	- [in]  이동하고자 하는 Stage의 Drive Position (절대 값. 단위: 0.1 um or 100 nm)
		error_x	- [out] Calibration 오차 값 (X Axis 오차 값. 단위: 100 nm or 0.1 um)
		error_y	- [out] Calibration 오차 값 (Y Axis 오차 값. 단위: 100 nm or 0.1 um)
 retn : TRUE (검색 성공) or FALSE (검색 실패)
*/
API_IMPORT BOOL uvACamCali_GetCaliOffsetXY(UINT8 cam_id, INT32 a_cam_x, INT32 stage_y,
										   INT32 &cal_off_x, INT32 &cal_off_y);


#ifdef __cplusplus
}
#endif