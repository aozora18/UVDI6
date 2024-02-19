
/*
 desc : Vision TCP/IP Library
*/

#pragma once

#include "../conf/global.h"
#include "../conf/luria.h"
#include "../conf/conf_gen2i.h"
#include "../conf/bsa.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : PLC Server와 통신을 담당하는 Client 스레드 생성 및 실행
 parm : config	- [in]  환경 설정 정보
		shmem	- [in]  Vision 관련 공유 메모리
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_Init(LPG_CIEA config, LPG_RBVC shmem);
/*
 desc : 라이브러리 해제
 parm : None
 retn : None
*/
API_IMPORT VOID uvBSA_Close();

/* --------------------------------------------------------------------------------------------- */
/*                                    Vision TCP/IP Interface                                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 수신된 데이터가 있는지 여부 반환
 parm : cmd	- [in]  명령어
 retn : TRUE or FALS
*/
API_IMPORT BOOL uvBSA_IsRecvData(ENG_VCPC cmd);
/*
desc : 현재 PLC가 연결되어 있는지 여부 확인
parm : None
retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_IsConnected();
/*
 desc : State 값이 주어진 시간 내에 한 번이라도 갱신된 적인 있는지 여부
 parm : time	- [in]  이 시간 (msec) 이내에 갱신된적이 있는지 확인하기 위한 시간 (msec)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_IsUpdateStateTime(UINT64 time);
/*
 desc : State 수신 받은 시간 강제로 현재 시간으로 갱신
 parm : None
 retn : None
*/
API_IMPORT VOID uvBSA_SetUpdateStateTime();
/*
 desc : 현재 발생된 이벤트가 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_IsVisionNotiEvent();
/*
 desc : 현재 발생된 이벤트를 무효화 처리
 parm : None
 retn : None
*/
API_IMPORT VOID uvBSA_ResetVisionNotiEvent();
/*
 desc : 현재 발생된 이벤트 메시지 반환
 parm : None
 retn : 이벤트 메시지가 저장된 버퍼 포인터 반환
*/
API_IMPORT PTCHAR uvBSA_GetVisionNotiEventMsg();
/*
 desc : CMPS <-> VISION Command Data 요청
 parm : data	- [in] 요청하고자 하는 Data 정보
		size	- [in] Data 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_SendDataCmd(ENG_VCPC cmd, PUINT8 data=NULL, UINT16 size=0x0000);
/*
 desc : CMPS <-> VISION STATE 상태 요청
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_ReqVisnState();
/*
 desc : Vision Centering Command Data Send
 parm :	cam_no	- [in] 카메라 설정 (0x01: bsa , 0x02: up)
		unit	- [in] 측정 위치정보(0x01: quaz, 0x02: mark1, 0x03: mark2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_ReqVisionCentering(UINT8 cam_no, UINT8 unit);
/*
 desc : Vision Focus Command Data Send
 parm :	cam_no	- [in] 카메라 설정 (0x01: bsa , 0x02: up)
		unit	- [in] 측정 위치정보(0x01: quaz, 0x02: mark1, 0x03: mark2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_ReqVisionFocus(UINT8 cam_no, UINT8 unit);
/*
 desc : Vision Focus Command Data Send
 parm :	point	- [in] 측정 순서 (0x01: 측정1, 0x02: 측정2)
		mark	- [in] Mark 번호 (0x01: mark1, 0x02: mark2)
		pattern	- [in] pattern 번호 
		posx	- [in] 현재 Stage X축 위치
		posy	- [in] 현재 Stage Y축 위치
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_ReqVisionGrab(UINT8 point, UINT8 mark, UINT8 pattern, INT32 posx, INT32 posy);
/*
 desc : Vision View Command Data Send
 parm :	mode	- [in] 카메라 view Mode (0x01: auto view , 0x02: manual view )
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_ReqVisionView(UINT8 mode);
/*
 desc : Vision Illuminatino Control Data Send
 parm :	mark	- [in] 측정 mark 번호
		onoff	- [in] 조명 제어 (0x00 : 0ff, 0x01 : on)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_ReqIllumination(UINT8 mark, UINT8 onoff);
/*
 desc : Vision Joblist Command Data Send
 parm :	job_mark_1	- [in] Job에 설정된 Mark1 Pattern List
		job_mark_2	- [in] Job에 설정된 Mark2 Pattern List
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_ReqVisionJobList(UINT64 job_mark_1, UINT64 job_mark_2);

/* --------------------------------------------------------------------------------------------- */
/*                                  BSA Camera Teaching nterface                                 */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Teach Centering Data 저장
 parm :	cam_no	- [in] 카메라 번호 (0x01: bsa , 0x02: up)
		point	- [in] Centering Point (Quaz(0x01), Mark1(0x02), Mark2(0x03)
		dx		- [in] Centering Point에 대한 X Axis Position
		dy		- [in] Centering Point에 대한 Y Axis Position
 retn : None
*/
API_IMPORT VOID uvBSA_SetTeachCentPos(UINT8 cam_no, UINT8 point, DOUBLE dx, DOUBLE dy);

#ifdef __cplusplus
}
#endif