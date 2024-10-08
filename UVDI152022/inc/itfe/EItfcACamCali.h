﻿
/*
 desc : The Interface Library for Align Camera Calibration
*/

#pragma once

#include "../conf/global.h"
#include "../conf/define.h"


#ifdef __cplusplus
extern "C"
{
#endif

/* --------------------------------------------------------------------------------------------- */
/*                           외부 함수 - Align Camera Calibration File                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Calibration Align Camera File 적재
 parm : cali_thick	- [in]  2D 측정된 노광 소재 두께 값 (단위: um) (100 ~ 99999 um)
 retn : None
*/
API_IMPORT BOOL uvEng_ACamCali_LoadFile(UINT16 cali_thick);
API_IMPORT BOOL uvEng_ACamCali_LoadFileEx(CString strFilePath);
API_IMPORT CString uvEng_ACamCali_GetFilePath();
/*
 desc : 가장 최근에 발생된 에러 메시지 출력
 parm : None
 retn : 에러 메시지가 저장된 버퍼
*/
API_IMPORT PTCHAR uvEng_ACamCali_GetLastErrMsg();
/*
 desc : 기존 작업 때에 등록되어 있던 Mark X/Y 위치 및 Mark 오차 정보 모두 제거
 parm : None
 retn : None
*/
API_IMPORT VOID uvEng_ACamCali_ResetAllCaliData();
/*
 desc : Global Fiducial의 X 좌표 간의 오차 즉, 1 & 2, 3 & 4 Mark 간의 거버 상에서 X 좌표 간의 오차 값
 parm : cam_id	- [in]  Camera Index (1 or 2)
		mark_no	- [in]  저장될 Mark Number (Zero based)
		offset	- [in]  X Offset 값 (0.1 um or 100 nm)
 retn : None
*/
API_IMPORT VOID uvEng_ACamCali_SetAlignACamVertGX(UINT8 cam_id, UINT8 mark_no, INT32 offset);
/*
 desc : Local Fiducial의 X 좌표 간의 오차 즉, 거버 상에서 X 좌표들 간의 오차 값
 parm : cam_id	- [in]  Camera Index (1 or 2)
		mark_no	- [in]  저장될 Mark Number
		offset	- [in]  X Offset 값
 retn : None
*/
API_IMPORT VOID uvEng_ACamCali_SetAlignACamVertLX(UINT8 cam_id, UINT8 mark_no, INT32 offset);
/*
 desc : 각 드라이브 별 즉, 축 (Axis) 별 Align Mark가 존재하는 모션의 실제 위치 값 저장
 parm : cam_id	- [in]  Camera Index (1 or 2)
		mark	- [in]  Global or Local Mark
		axis	- [in]  0x00 : X Axis (Camera X), 0x01 : Y Axis (Stage Y)
		idx		- [in]  저장 위치 값 (Zero based)
		pos		- [in]  모션의 위치 값 (mm)
 retn : None
*/
API_IMPORT VOID uvEng_ACamCali_AddMarkPos(UINT8 cam_id, ENG_AMTF mark, UINT8 axis,
										  UINT8 idx, DOUBLE pos);


API_IMPORT VOID uvEng_ACamCali_AddMarkPosForce(UINT8 cam_id, ENG_AMTF mark, double offsetX, double offsetY);

/*
 desc : 원하는 위치에 근사한 Align Camera의 Calibration 오차 값 반환
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
		a_cam_x	- [in]  이동하고자 하는 Align Camera의 Drive Position (절대 값. 단위: 0.1 um or 100 nm)
		stage_y	- [in]  이동하고자 하는 Stage의 Drive Position (절대 값. 단위: 0.1 um or 100 nm)
		error_x	- [out] Calibration 오차 값 (X Axis 오차 값. 단위: 100 nm or 0.1 um)
		error_y	- [out] Calibration 오차 값 (Y Axis 오차 값. 단위: 100 nm or 0.1 um)
 retn : 0x00 (검색 실패), 0x01 (검색 성공), 0x02 (보정 영역에 없음)
*/
API_IMPORT UINT8 uvEng_ACamCali_GetCaliPos(UINT8 cam_id, DOUBLE a_cam_x, DOUBLE stage_y,
										   DOUBLE &error_x, DOUBLE &error_y);
API_IMPORT UINT8 uvEng_ACamCali_GetCaliPosEx(DOUBLE dPosX, DOUBLE dPosY, DOUBLE& error_x, DOUBLE& error_y);
/*
 desc : 트리거 발생 위치에 해당되는 Stage Y와 Align Camera X 좌표에 대해, 2D Calibration 데이터 적용
 parm : None
 retn : 0x00 (검색 실패), 0x01 (검색 성공), 0x02 (보정 영역에 없음)
*/
API_IMPORT UINT8 uvEng_ACamCali_SetTrigPosCaliApply();
/*
 desc : 2D Calibration Data가 존재하는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_ACamCali_IsCaliExistData();
/*
 desc : Global Mark - 거버 데이터의 Mark 2번 기준으로 Mark 1번의 좌표 오차 값 임시 저장
		Global Mark - 거버 데이터의 Mark 4번 기준으로 Mark 2번의 좌표 오차 값 임시 저장
 parm : diff_x21	- [in]  좌표 X 값 기준으로 Mark 2번 - Mark 1번 오차 값 (단위: mm)
		diff_x43	- [in]  좌표 X 값 기준으로 Mark 4번 - Mark 3번 오차 값 (단위: mm)
 retn : None
*/
API_IMPORT VOID uvEng_ACamCali_SetGlobalMarkDiffX(DOUBLE diff_x21, DOUBLE diff_x43);
/*
 desc : Global Mark - 거버 데이터의 Mark 2번 기준으로 Mark 1번의 좌표 오차 값 반환
		Global Mark - 거버 데이터의 Mark 4번 기준으로 Mark 2번의 좌표 오차 값 반환
 parm : cam_id	- [in]  Align Camera ID (1 or Later)
						0x01 : 좌표 X 값 기준으로 Mark 2번 - Mark 1번 오차 값 (단위: mm)
						0x02 : 좌표 X 값 기준으로 Mark 4번 - Mark 3번 오차 값 (단위: mm)
 retn : Global Mark의 두 점의 X 좌표 오차 값 반환 (단위: mm)
*/
API_IMPORT DOUBLE uvEng_ACamCali_GetGlobalMarkDiffX(UINT8 type);


#ifdef __cplusplus
}
#endif