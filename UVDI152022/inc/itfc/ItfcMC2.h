
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
 desc : MC2���� ��� ����
 parm : config	- [in]  ȯ�� ���� ����
		shmem	- [in]  MC2 ���� ���� �޸�
 retn : TRUE or FALSE
*/
//API_IMPORT BOOL uvMC2_Init(LPG_CIEA config, LPG_MDSM shmem);
API_IMPORT BOOL uvMC2_Init(LPG_CIEA config, LPG_MDSM shmem, LPG_MDSM shmem2);

API_IMPORT BOOL uvMC2a_Init(LPG_CIEA config, LPG_MDSM shmem);
API_IMPORT BOOL uvMC2b_Init(LPG_CIEA config, LPG_MDSM shmem);
/*
 desc : MC2���� ��� ����
 parm : None
 retn : None
*/
API_IMPORT VOID uvMC2_Close();
/*
 desc : ���� �ý��� ���� ���� �� ������ ����
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_Reconnected();
/*
 desc : MC2 Server�� ���� ����
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_IsConnected();
/*
 desc : ���� �ֱٿ� �߻��� ���� ���� �ڵ� �� ��ȯ
 parm : None
 retn : Socket Error Code (MSDN ����)
*/
API_IMPORT UINT32 uvMC2_GetSockLastError();

/* --------------------------------------------------------------------------------------------- */
/*                                  MC2 Communication Functions                                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ���� �ӵ� ����
 parm : drv_id	- [in]  Axis ���� (0, 1, 4, 5)
		velo	- [in]  �̵� �ӵ� (����: mm/sec)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevGoVelo(ENG_MMDI drv_id, UINT32 velo);
/*
 desc : ���� �⺻ �ӵ� �� ���ӵ� ����
 parm : drv_id	- [in]  Axis ���� (0, 1, 4, 5)
		velo	- [in]  �̵� �ӵ� (����: mm/sec)
		acce	- [in]  �̵� ���ӵ� (����: mm^2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevBaseVeloAcce(ENG_MMDI drv_id, UINT32 velo, UINT32 acce);
/*
 desc : Luria Expose Mode�� ����
 parm : drv_id	- [in]  �뱤 ������ �� Drive ID
		mode	- [in]  Area (Align) mode or Expose mode (ENG_MTAE)
						Area Mode			: ���� ���������� �������� ����
						Expo Mode			: ���� ��ġ ������ Min ���� ������, �������� ������, �������� Min ������ �̵� ��
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
		begin	- [in]  �뱤 ���� ��ġ (����: nm)
		end		- [in]  �뱤 ���� ��ġ (����: nm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevLuriaMode(ENG_MMDI drv_id, ENG_MTAE mode,
									   UINT32 pixel=0, INT32 begin=0, INT32 end=0);
/*
 desc : �������� ��� �̵� (Up, Down, Left, Bottom)
 parm : method	- [in]  ENG_MMMM
		drv_id	- [in]  Motor Drive ID (SD2 Device ID) ��, 0x00, 0x01, 0x04, 0x05
		direct	- [in]  �̵� ���� (Up / Down / Left / Right ...)
		dist	- [in]  �̵� �Ÿ� (����: 0.1 um or 100 nm) (�ݵ�� 0 ���� Ŀ�� ��)
		velo	- [in]  �̵� �ӵ� (����: 0.1 um/sec or 100 nm/sec)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevRefMove(ENG_MMMM method, ENG_MMDI drv_id,
									 ENG_MDMD direct, INT32 dist, UINT32 velo);
/*
 desc : �������� ���� �̵� (Up, Down, Left, Bottom)
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) ��, 0x00, 0x01, 0x04, 0x05
		move	- [in]  ���������� ���� �̵��� ��ġ (����: 0.1 um or 100 nm)
		velo	- [in]  �̵� �ӵ�  (����: 0.1 um or 100 nm /sec)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevAbsMove(ENG_MMDI drv_id, INT32 move, UINT32 velo);
/*
 desc : ���������� Home ��ġ (0, 0)�� �̵�
 parm : drv_id	- [in]  Home ��ġ�� �̵��ϱ� ���� Drive ID
		velo	- [in]  �������� �̵� �ӵ� (0.1 um/sec or 100 nm/sec)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevMovingHome(ENG_MMDI drv_id, INT32 velo);
/*
 desc : ���������� Vector �� �̵�
 parm : drv_x	- [in]  Vector �̵� ��� ���� Drive ID 1
		drv_y	- [in]  Vector �̵� ��� ���� Drive ID 2
		pos_x	- [in]  ���������� ���� �̵��� X ���� ��ġ (����: 0.1 um or 100 nm)
		pos_y	- [in]  ���������� ���� �̵��� Y ���� ��ġ (����: 0.1 um or 100 nm)
		velo	- [in]  �������� �̵� �ӵ� ��
		axis	- [out] Vector �̵��� ���, Master Axis (���� ��) �� ��ȯ
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevMoveVectorXY(ENG_MMDI drv_x, ENG_MMDI drv_y,
										  INT32 pos_x, INT32 pos_y, INT32 velo, ENG_MMDI &axis);
/*
 desc : Device Stopped
 parm : drv_id	- [in]  SD2 Driver ID ��, 0x00, 0x01, 0x04, 0x05
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevStopped(ENG_MMDI drv_id);
/*
 desc : Device Locked
 parm : drv_id	- [in]  SD2 Driver ID
		flag	- [in]  TRUE (Servo On) or FALSE(Servo Off) Command
��ȯ : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevLocked(ENG_MMDI drv_id, BOOL flag);
/*
 desc : Device Fault Reset (Error Init)
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) ��, 0x00, 0x01, 0x04, 0x05
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevFaultReset(ENG_MMDI drv_id);
/*
 desc : ���� SD3 ����̺�� (������ SD3 Drive) Error�� ���� ���ÿ� Fault Reset ó��
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevFaultResetAll();
/*
 desc : �������� Homing
 parm : drv_id	- [in]  Motor Drive ID. ���� None�̸� ��ü Homing
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevHoming(ENG_MMDI drv_id);
/*
 desc : ��� ����̺�� (������ SD3 Drive)�� ���� ���ÿ� Homing ó��
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SendDevHomingAll();
/*
 desc : Luria�� ���� �뱤 (Printing) ������ ���ο� ����,
		MC2���� �ֱ������� ��� ������ ��û ���� ����
 parm : enable	- [in]  TRUE : ��û ����, FALSE : ��û ���� (�ƿ� ��û ������ �ƴ�)
 retn : None
*/
API_IMPORT VOID uvMC2_SetSendPeriodPkt(BOOL enable);
/*
 desc : MC2���� �ֱ������� ��Ŷ �����͸� ��û�ϴ��� ���� ��ȯ
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_IsSendPeriodPkt();
/*
 desc : Reference Data ��ü ��û
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_ReadReqPktRefAll();
/*
 desc : Active Data ��ü ��û
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_ReadReqPktActAll();
/*
 desc : MC2���� ���� ���� Drive (SD2S) �� �� ���� ������ �߻� �ߴ��� ����
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_IsAnyDriveError();
/*
 desc : Device Control - Reference Header - for EN_STOP and HEART_BEAT
 parm : en_stop	- [in]  ENG_MMSM (0x00:Only Homing operation or 0x01: Other operation)
		beat	- [in]  Toggle (0 or 1) (Alive Check �뵵)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMC2_SetRefHeadMControl(ENG_MMST en_stop, UINT8 beat);
API_IMPORT BOOL uvMC2_SetRefHeadMControlEx(ENG_MMST en_stop);


#ifdef __cplusplus
}
#endif
