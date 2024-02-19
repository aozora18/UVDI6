
/*
 desc : The Interface Library for Philoptics's Trigger Device
*/

#include "pch.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */


#ifdef __cplusplus
extern "C"
{
#endif

/* --------------------------------------------------------------------------------------------- */
/*                             외부 함수 - < Trigger >  < for Engine >                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Trigger & Strobe Enable or Disable
 parm : enable	- [in]  TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Mvenc_ReqTriggerStrobe(BOOL enable)
{
	if (!uvMvenc_IsConnected() && GetConfig()->IsRunDemo())	return TRUE;
	return uvMvenc_ReqTriggerStrobe(enable);
}

/*
 desc : 내부 트리거 설정
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
		lamp_type	- [in]  램프 조명 타입 (0:Ring or 1:Coaxial)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Mvenc_ReqEncoderLive(UINT8 cam_id, UINT8 lamp_type)
{
	if (!uvMvenc_IsConnected() && GetConfig()->IsRunDemo())	return TRUE;
	return uvMvenc_ReqEncoderLive(cam_id, lamp_type);
}

/*
 desc : 내부 트리거 설정 초기화
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Mvenc_ReqEncoderOutReset()
{
	if (!uvMvenc_IsConnected() || GetConfig()->IsRunDemo())	return TRUE;
	return uvMvenc_ReqEncoderOutReset();
}

/*
 desc : 트리거 1개 발생 시킴 (Only Trigger Event)
 parm : ch_no	- [in]  채널 번호 (1 or 2)
		lamp_type	- [in]  램프 조명 타입 (0:Ring or 1:Coaxial)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Mvenc_ReqTrigOutOneOnly(UINT8 ch_no, UINT8 lamp_type)
{
	/* Check if it is in demo operation mode */
	if (!uvMvenc_IsConnected() && GetConfig()->IsRunDemo())	return TRUE;
	return uvMvenc_ReqTrigOutOneOnly(ch_no, lamp_type);
}

/*
 desc : 트리거 1개 발생 시킴 (Trigger Enable -> Trigger Event -> Trigger Disable)
 parm : ch_no	- [in]  채널 번호 (1 or 2)
		lamp_type	- [in]  램프 조명 타입 (0:Ring or 1:Coaxial)
		enable	- [in]  트리거 내보낸 이후 Disable 시킬지 여부
						트리거 1개 발생 후, 곧바로 트리거 Disable 할지 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Mvenc_ReqTrigOutOne(UINT8 ch_no, UINT8 lamp_type, BOOL enable)
{
	if (!uvMvenc_IsConnected() && GetConfig()->IsRunDemo())	return TRUE;
	return uvMvenc_ReqTrigOutOne(ch_no, lamp_type, enable);
}

#if 1
/*
 desc : Trigger Board에 처음 연결되고 난 이후, 환경 파일에 설정된 값으로 초기화 진행
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Mvenc_ReqInitUpdate()
{
	if (!uvMvenc_IsConnected() && GetConfig()->IsRunDemo())	return TRUE;
	return uvMvenc_ReqInitUpdate();
}
#endif
/*
 desc : 기존 등록된 4개 채널에 대한 Trigger Position 값 초기화 (최대 값) 수행
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Mvenc_ResetTrigPosAll()
{
	if (!uvMvenc_IsConnected() && GetConfig()->IsRunDemo())	return TRUE;
	return uvMvenc_ResetTrigPosAll();
}

/*
 desc : 현재 트리거의 위치 등록 값이 초기화 되었는지 여부
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		count	- [in]  채널 번호에 해당되는 검사할 트리거 등록 개수 (MAX: 16)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Mvenc_IsTrigPosReset(UINT8 cam_id, UINT8 count)
{
	if (!uvMvenc_IsConnected() && GetConfig()->IsRunDemo())	return TRUE;
	return uvMvenc_IsTrigPosReset(cam_id, count);
}

/*
 desc : 현재 트리거에 송신될 명령 버퍼의 대기 개수가 임의 개수 이하인지 여부
 parm : count	- [in]  이 개수 미만이면 TRUE, 이상이면 FALSE
 retn : count 개수 미만 (미포함)이면 TRUE, 이상 (포함)이면 FALSE 반환
*/
API_EXPORT BOOL uvEng_Mvenc_IsSendCmdCountUnder(UINT16 count)
{
	if (!uvMvenc_IsConnected() && GetConfig()->IsRunDemo())	return TRUE;
	return uvMvenc_IsSendCmdCountUnder(count);
}

/*
 desc : 채널 별로 트리거 위치 등록
 parm : direct	- [in]  TRUE: 정방향 (전진), FALSE: 역방향 (후진)
		start	- [in]  트리거 저장 시작 위치 (0 ~ 15)
		count	- [in]  트리거 등록 개수 (1 ~ 16)
		pos		- [in]  트리거 등록 위치가 저장되어 있는 배열 포인터 (unit : 100 nm or 0.1 um)
		enable	- [in]  트리거 Enable or Disable
		clear	- [in]  트리거 위치 등록하면서 현재 Trigger Encoder 값을
						Clear (TRUE)할것인지 혹은 읽기 모드 (FALSE)를 유지할 것인지 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Mvenc_ReqWriteAreaTrigPos(BOOL direct,
												UINT8 start1, UINT8 count1, PINT32 pos1,
											   UINT8 start2, UINT8 count2, PINT32 pos2,
											   ENG_TEED enable, BOOL clear)
{
	UINT8 i	= 0x00;
	INT32 i32Trig[MAX_ALIGN_CAMERA][MAX_SCAN_MARK_COUNT]	= {NULL};

	if (!uvMvenc_IsConnected() && GetConfig()->IsRunDemo())	return TRUE;

	//for (; i<count1; i++)
	//{
	//	i32Trig[0][i]	= pos1[i];	/* Align Camera 1 */
	//	i32Trig[1][i]	= pos2[i];	/* Align Camera 2 */
	//}

	//vector <INT32> vNum1;
	//vector <INT32> vNum2;
	//for (; i < count1; i++)
	//{
	//	vNum1.push_back(pos1[i]);
	//	vNum2.push_back(pos2[i]);
	//}
	///* Vector에 등록된 값 기준으로 오름차순 / 내림차순 정렬 */
	//if (enable == ENG_TEED::en_positive)
	//{
	//	sort(vNum1.begin(), vNum1.end());
	//	sort(vNum2.begin(), vNum2.end());
	//}
	//else
	//{
	//	sort(vNum1.begin(), vNum1.end(), greater<INT32>());
	//	sort(vNum2.begin(), vNum2.end(), greater<INT32>());
	//}

	//for (; i < count1; i++)
	//{
	//	pos1[i] == vNum1[i];
	//	pos2[i] == vNum2[i];
	//}

	return uvMvenc_ReqWriteAreaTrigPos(direct,
									  start1, count1, i32Trig[0],
									  start2, count2, i32Trig[1], enable, clear);
}

/*
 desc : 특정 채널에 트리거 위치 등록
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		start	- [in]  트리거 저장 시작 위치 (0 ~ 15)
		count	- [in]  트리거 등록 개수 (1 ~ 16)
		pos		- [in]  트리거 등록 위치가 저장되어 있는 배열 포인터 (unit : 100 nm or 0.1 um)
		enable	- [in]  트리거 Enable or Disable
		clear	- [in]  트리거 위치 등록하면서 현재 Trigger Encoder 값을
						Clear (TRUE)할것인지 혹은 읽기 모드 (FALSE)를 유지할 것인지 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Mvenc_ReqWriteAreaTrigPosCh(UINT8 cam_id, UINT8 start, UINT8 count, PINT32 pos,
												 ENG_TEED enable, BOOL clear)
{
	if (!uvMvenc_IsConnected() && GetConfig()->IsRunDemo())	return TRUE;
	return uvMvenc_ReqWriteAreaTrigPosCh(cam_id, start, count, pos, enable, clear);
}

/*
 desc : 기존에 입력된 Trigger 위치 값과 Trigger Board로부터 수신된 입력 값 비교
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		index	- [in]  트리거 저장 위치 (0x000 ~ 0x0f)
		pos		- [in]  트리거 값 (단위: 100 nm or 0.1 um)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Mvenc_IsTrigPosEqual(UINT8 cam_id, UINT8 index, UINT32 pos)
{
	if (!uvMvenc_IsConnected() && GetConfig()->IsRunDemo())	return TRUE;
	return uvMvenc_IsTrigPosEqual(cam_id, index, pos);
}


/*
 desc : 카메라 별 가상 트리거 발생하여 Live 모드로 진행
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		enable	- [in]  TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Mvenc_ReqWirteTrigLive(UINT8 cam_id, BOOL enable)
{
	if (!uvMvenc_IsConnected() && GetConfig()->IsRunDemo())	return TRUE;
	return uvMvenc_ReqWirteTrigLive(cam_id, enable);
}


/* --------------------------------------------------------------------------------------------- */
/*                             외부 함수 - < Trigger >  < for Common >                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 현재 PLC가 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_Mvenc_IsConnected()
{
	return uvMvenc_IsConnected();
}

/*
 desc : Trigger 내의 Strobe이 Enable or Disable 상태 값 반환
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_Mvenc_IsStrobEnable()
{
	if (GetConfig()->IsRunDemo())	return TRUE;
	return g_pMemMvenc->GetMemMap()->IsTrigStrobEnabled();
}

/*
 desc : Trigger 내의 Strobe이 Enable or Disable 상태 값 반환
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_Mvenc_IsStrobDisable()
{
	if (GetConfig()->IsRunDemo())	return TRUE;
	return g_pMemMvenc->GetMemMap()->IsTrigStrobDisabled();
}


#ifdef __cplusplus
}
#endif