
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
 desc : PLC Server�� ����� ����ϴ� Client ������ ���� �� ����
 parm : config	- [in]  ȯ�� ���� ����
		shmem	- [in]  Vision ���� ���� �޸�
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_Init(LPG_CIEA config, LPG_RBVC shmem);
/*
 desc : ���̺귯�� ����
 parm : None
 retn : None
*/
API_IMPORT VOID uvBSA_Close();

/* --------------------------------------------------------------------------------------------- */
/*                                    Vision TCP/IP Interface                                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ���ŵ� �����Ͱ� �ִ��� ���� ��ȯ
 parm : cmd	- [in]  ��ɾ�
 retn : TRUE or FALS
*/
API_IMPORT BOOL uvBSA_IsRecvData(ENG_VCPC cmd);
/*
desc : ���� PLC�� ����Ǿ� �ִ��� ���� Ȯ��
parm : None
retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_IsConnected();
/*
 desc : State ���� �־��� �ð� ���� �� ���̶� ���ŵ� ���� �ִ��� ����
 parm : time	- [in]  �� �ð� (msec) �̳��� ���ŵ����� �ִ��� Ȯ���ϱ� ���� �ð� (msec)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_IsUpdateStateTime(UINT64 time);
/*
 desc : State ���� ���� �ð� ������ ���� �ð����� ����
 parm : None
 retn : None
*/
API_IMPORT VOID uvBSA_SetUpdateStateTime();
/*
 desc : ���� �߻��� �̺�Ʈ�� �ִ��� ����
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_IsVisionNotiEvent();
/*
 desc : ���� �߻��� �̺�Ʈ�� ��ȿȭ ó��
 parm : None
 retn : None
*/
API_IMPORT VOID uvBSA_ResetVisionNotiEvent();
/*
 desc : ���� �߻��� �̺�Ʈ �޽��� ��ȯ
 parm : None
 retn : �̺�Ʈ �޽����� ����� ���� ������ ��ȯ
*/
API_IMPORT PTCHAR uvBSA_GetVisionNotiEventMsg();
/*
 desc : CMPS <-> VISION Command Data ��û
 parm : data	- [in] ��û�ϰ��� �ϴ� Data ����
		size	- [in] Data ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_SendDataCmd(ENG_VCPC cmd, PUINT8 data=NULL, UINT16 size=0x0000);
/*
 desc : CMPS <-> VISION STATE ���� ��û
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_ReqVisnState();
/*
 desc : Vision Centering Command Data Send
 parm :	cam_no	- [in] ī�޶� ���� (0x01: bsa , 0x02: up)
		unit	- [in] ���� ��ġ����(0x01: quaz, 0x02: mark1, 0x03: mark2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_ReqVisionCentering(UINT8 cam_no, UINT8 unit);
/*
 desc : Vision Focus Command Data Send
 parm :	cam_no	- [in] ī�޶� ���� (0x01: bsa , 0x02: up)
		unit	- [in] ���� ��ġ����(0x01: quaz, 0x02: mark1, 0x03: mark2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_ReqVisionFocus(UINT8 cam_no, UINT8 unit);
/*
 desc : Vision Focus Command Data Send
 parm :	point	- [in] ���� ���� (0x01: ����1, 0x02: ����2)
		mark	- [in] Mark ��ȣ (0x01: mark1, 0x02: mark2)
		pattern	- [in] pattern ��ȣ 
		posx	- [in] ���� Stage X�� ��ġ
		posy	- [in] ���� Stage Y�� ��ġ
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_ReqVisionGrab(UINT8 point, UINT8 mark, UINT8 pattern, INT32 posx, INT32 posy);
/*
 desc : Vision View Command Data Send
 parm :	mode	- [in] ī�޶� view Mode (0x01: auto view , 0x02: manual view )
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_ReqVisionView(UINT8 mode);
/*
 desc : Vision Illuminatino Control Data Send
 parm :	mark	- [in] ���� mark ��ȣ
		onoff	- [in] ���� ���� (0x00 : 0ff, 0x01 : on)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_ReqIllumination(UINT8 mark, UINT8 onoff);
/*
 desc : Vision Joblist Command Data Send
 parm :	job_mark_1	- [in] Job�� ������ Mark1 Pattern List
		job_mark_2	- [in] Job�� ������ Mark2 Pattern List
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBSA_ReqVisionJobList(UINT64 job_mark_1, UINT64 job_mark_2);

/* --------------------------------------------------------------------------------------------- */
/*                                  BSA Camera Teaching nterface                                 */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Teach Centering Data ����
 parm :	cam_no	- [in] ī�޶� ��ȣ (0x01: bsa , 0x02: up)
		point	- [in] Centering Point (Quaz(0x01), Mark1(0x02), Mark2(0x03)
		dx		- [in] Centering Point�� ���� X Axis Position
		dy		- [in] Centering Point�� ���� Y Axis Position
 retn : None
*/
API_IMPORT VOID uvBSA_SetTeachCentPos(UINT8 cam_no, UINT8 point, DOUBLE dx, DOUBLE dy);

#ifdef __cplusplus
}
#endif