
/*
 desc : Vision TCP/IP Library
*/

#pragma once

#include "../conf/global.h"
#include "../conf/bsa.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : Whether there is data received (or command)
 parm : cmd	- [in]  command
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_BSA_IsRecvData(ENG_VCPC cmd);
/*
 desc : Whether it is currently connected to BSA server
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_BSA_IsConnected();
/*
 desc : Change the received STATE to the current time.
 parm : None
 retn : None
*/
API_IMPORT VOID uvEng_BSA_SetUpdateStateTime();
/*
 desc : State 값이 주어진 시간 내에 한 번이라도 갱신된 적인 있는지 여부
 parm : time	- [in]  이 시간 (msec) 이내에 갱신된적이 있는지 확인하기 위한 시간 (msec)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_BSA_IsUpdateStateTime(UINT64 time);
/*
 desc : 현재 발생된 이벤트가 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_BSA_IsVisionNotiEvent();
/*
 desc : 현재 발생된 이벤트를 무효화 처리
 parm : None
 retn : None
*/
API_IMPORT VOID uvEng_BSA_ResetVisionNotiEvent();
/*
 desc : 현재 발생된 이벤트 메시지 반환
 parm : None
 retn : 이벤트 메시지가 저장된 버퍼 포인터 반환
*/
API_IMPORT PTCHAR uvEng_BSA_GetVisionNotiEventMsg();
/*
 desc : CMPS <-> VISION Command Data 요청
 parm : data	- [in] 요청하고자 하는 Data 정보
		size	- [in] Data 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_BSA_SendDataCmd(ENG_VCPC cmd, PUINT8 data=NULL, UINT16 size=0x0000);
API_IMPORT BOOL uvEng_BSA_SendDataCmdOnly(ENG_VCPC cmd);
API_IMPORT BOOL uvEng_BSA_SendDataCmdEx(ENG_VCPC cmd, UINT8 **data=NULL, UINT16 size=0x0000);	/* Only C# */
/*
 desc : CMPS <-> VISION STATE 상태 요청
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_BSA_ReqVisnState();
/*
 desc : Vision Centering Command Data Send
 변수 :	cam_no	- [in] 카메라 설정 (0x01: bsa , 0x02: up)
		unit	- [in] 측정 위치정보(0x01: quaz, 0x02: mark1, 0x03: mark2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_BSA_ReqVisionCentering(UINT8 cam_no, UINT8 unit);
/*
 desc : Vision Focus Command Data Send
 변수 :	cam_no	- [in] 카메라 설정 (0x01: bsa , 0x02: up)
		unit	- [in] 측정 위치정보(0x01: quaz, 0x02: mark1, 0x03: mark2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_BSA_ReqVisionFocus(UINT8 cam_no, UINT8 unit);
/*
 desc : Vision Focus Command Data Send
 변수 :	point	- [in] 측정 순서 (0x01: 측정1, 0x02: 측정2)
		mark	- [in] Mark 번호 (0x01: mark1, 0x02: mark2)
		pattern	- [in] pattern 번호 
		posx	- [in] 현재 Stage X축 위치
		posy	- [in] 현재 Stage Y축 위치
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_BSA_ReqVisionGrab(UINT8 point, UINT8 mark, UINT8 pattern, INT32 posx, INT32 posy);
/*
 desc : Vision View Command Data Send
 변수 :	mode	- [in] 카메라 view Mode (0x01: auto view , 0x02: manual view )
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_BSA_ReqVisionView(UINT8 mode);
/*
 desc : Vision Illuminatino Control Data Send
 변수 :	mark	- [in] 측정 mark 번호
		onoff	- [in] 조명 제어 (0x00 : 0ff, 0x01 : on)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_BSA_ReqIllumination(UINT8 mark, UINT8 onoff);
/*
 desc : Vision Joblist Command Data Send
 변수 :	job_mark_1	- [in] Job에 설정된 Mark1 Pattern List
		job_mark_2	- [in] Job에 설정된 Mark2 Pattern List
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_BSA_ReqVisionJobList(UINT64 job_mark_1, UINT64 job_mark_2);

#ifdef __cplusplus
}
#endif