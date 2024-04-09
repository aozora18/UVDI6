
/*
 desc : Trigger Board Communication Libarary (Trig Protocol)
*/

#include "pch.h"
#include "MainApp.h"
#include "MvencThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                           ���� ����                                           */
/* --------------------------------------------------------------------------------------------- */

CMvencThread				*g_pMvencThread		= NULL;		// ������ ���� �� ���� ������

/* ���� �޸� */
LPG_TPQR					g_pstShMemTrig		= NULL;
LPG_DLSM					g_pstShMemLink		= NULL;
STG_CTCS					g_stCommInfo		= {NULL};

/* --------------------------------------------------------------------------------------------- */
/*                                           ���� �Լ�                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ���� PLC�� ����Ǿ� �ִ��� ���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL IsConnected()
{
	if (!g_pMvencThread)				return FALSE;

	return (g_pMvencThread->IsInitRecvData() && g_pMvencThread->IsConnected());
}

/*
 desc : Trigger
 parm : cam_id		- [in]  Align Camera Index (1 or 2)
		lamp_type	- [in]  ���� ���� Ÿ�� (0:Ring or 1:Coaxial)
 retn : TRUE or FALSE
 note : Camera 1 : 1 CH (Lamp: Coxial Type), 2 CH (Lamp: Ring Type)
		Camera 2 : 3 CH (Lamp: Coxial Type), 4 CH (Lamp: Ring Type)
*/
//UINT32 GetTrigEncOutVal(UINT8 cam_id)
UINT32 GetTrigEncOutVal(UINT8 cam_id, UINT8 lamp_type)
{
	BOOL bSucc		= FALSE;
	UINT32 u32EncOut= 0x00000000;

	if (!IsConnected())	return FALSE;

	//switch (g_pstConfig->set_comn.strobe_lamp_type)
	switch (lamp_type)
	{
	case 0x00	:	/* ���� 1 (Lamp: Coaxial) */
		switch (cam_id)
		{
		case 0x01	:	u32EncOut = 0x01000000;	break;	/* Trigger Board: 1 CH Enable */
		case 0x02	:	u32EncOut = 0x00000100;	break;	/* Trigger Board: 3 CH Enable */
		}
		break;
	case 0x01	:	/* ���� 2 (Lamp: Ring) */
		/* Ring Lamp�� ���, 1 & 3 CH (Camera) �ʿ� ������ Trigger �߻���Ű�� ���� */
		switch (cam_id)
		{
		case 0x01	:	u32EncOut = 0x01010000;	break;	/* Trigger Board: 2 CH Enable */
		case 0x02	:	u32EncOut = 0x00000101;	break;	/* Trigger Board: 4 CH Enable */
		}
		break;
	}

	return u32EncOut;
}


/* --------------------------------------------------------------------------------------------- */
/*                                           �ܺ� �Լ�                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/*
 desc : PLC Server�� ����� ����ϴ� Client ������ ���� �� ����
 parm : config	- [in]  ȯ�� ���� ����
		shmem	- [in]  Trigger Service ���� ���� �޸�
		link	- [in]  ��� ���� ���� �޸�
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_Init(BYTE byPort, LPG_CIEA config, LPG_TPQR shmem, LPG_DLSM link)
{
	LPG_CTSP pstTrig= NULL;

	/* Luria Shared Memory */
	g_pstShMemTrig	= shmem;
	g_pstShMemLink	= link;
	/* All Configuration */
	g_pstConfig		= config;

	// Client ������ ���� �� ���� ��Ŵ
	pstTrig	= &config->trig_grab;
	g_pMvencThread = new CMvencThread(byPort, pstTrig, shmem, link, &config->set_comn);
	if (!g_pMvencThread)	return FALSE;
	if (!g_pMvencThread->CreateThread(0, 0, MEDIUM_THREAD_SPEED))
	{
		delete g_pMvencThread;
		g_pMvencThread	= NULL;
		return FALSE;
	}

	return TRUE;
}

/*
 desc : PLC Server�� ����� ����ϴ� Client ������ ���� �� ����
 parm : config	- [in]  ȯ�� ���� ����
		shmem	- [in]  Trigger Service ���� ���� �޸�
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_Open(LPG_CIEA config, LPG_TPQR shmem)
{
	LPG_CTSP pstTrig= NULL;

	/* Luria Shared Memory */
	g_pstShMemTrig	= shmem;
	/* All Configuration */
	g_pstConfig		= config;

	return TRUE;
}

/*
 desc : ���̺귯�� ����
 parm : None
 retn : None
*/
API_EXPORT VOID uvMvenc_Close()
{
	// ������ ���� ��Ű�� ��� ���� �Ŀ� ������ ������ ���
	if (g_pMvencThread)
	{
		/* Trigger Board - Disabled */
		g_pMvencThread->ReqWriteEncoderOut(0x00000000);
		g_pMvencThread->ReqWriteTriggerStrobe(FALSE);
		/* �� ������ �۽ŵ� ������ ��� */
		Sleep(500);
		// ������ ���� ȣ��
		g_pMvencThread->Stop();
		while (g_pMvencThread->IsBusy())	g_pMvencThread->Sleep(100);
		//g_pMvencThread->CloseHandle();
		delete g_pMvencThread;
		g_pMvencThread	= NULL;
	}
	/* ���ָ� �ȵ� */
//	g_pstConfig	= NULL;
}

/*
 desc : �ý��� ������ ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_Reconnected(BYTE byPort)
{
	LPG_CTSP pstTrig= NULL;

	uvMvenc_Close();
	pstTrig	= &g_pstConfig->trig_grab;

	return uvMvenc_Init(byPort, g_pstConfig, g_pstShMemTrig, g_pstShMemLink);
}

/*
 desc : ���� PLC�� ����Ǿ� �ִ��� ���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_IsConnected()
{
	return IsConnected();
}

/*
 desc : �ʱ� ����ǰ� �� ���� ���ŵ� �����Ͱ� �����ϴ��� ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_IsInitRecv()
{
	return !g_pMvencThread ? FALSE : g_pMvencThread->IsInitRecvData();
}
#if 1
/*
 desc : Trigger Board�� ó�� ����ǰ� �� ����, ȯ�� ���Ͽ� ������ ������ �ʱ�ȭ ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_ReqInitUpdate()
{
	return !g_pMvencThread ? FALSE : g_pMvencThread->ReqInitUpdate();
}
#endif
/*
 desc : ���� ��ϵ� 4�� ä�ο� ���� Trigger Position �� �ʱ�ȭ (�ִ� ��) ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_ResetTrigPosAll()
{
	if (!IsConnected())	return FALSE;
	return g_pMvencThread->ResetTrigPosAll();
}

/*
 desc : ���� Ʈ������ ��ġ ��� ���� �ʱ�ȭ �Ǿ����� ����
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		count	- [in]  ä�� ��ȣ�� �ش�Ǵ� �˻��� Ʈ���� ��� ���� (MAX: 16)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_IsTrigPosReset(UINT8 cam_id, UINT8 count)
{
	UINT8 i	= 0;
	UINT8 u8ChNo	= g_pMvencThread->GetTrigChNo(cam_id);
	if (!u8ChNo)	return FALSE;

	for (; i<count; i++)
	{
		if (g_pstShMemTrig->trig_set[u8ChNo-1].area_trig_pos[i] != MAX_TRIG_POS)	return FALSE;
	}

	return TRUE;
}

/*
 desc : ä�� ���� Ʈ���� ��ġ ���
 parm : direct	- [in]  TRUE: ������ (����), FALSE: ������ (����)
		start	- [in]  Ʈ���� ���� ��ġ (0 ~ 15)
		count	- [in]  Ʈ���� ��� ���� (1 ~ 16)
		pos		- [in]  Ʈ���� ��� ��ġ�� ����Ǿ� �ִ� �迭 ������ (unit : 100 nm or 0.1 um)
		enable	- [in]  Ʈ���� Enable or Disable
		clear	- [in]  Ʈ���� ��ġ ����ϸ鼭 ���� Trigger Encoder ����
						Clear (TRUE)�Ұ����� Ȥ�� �б� ��� (FALSE)�� ������ ������ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_ReqWriteAreaTrigPos(BOOL direct,
										   UINT8 start1, UINT8 count1, PINT32 pos1,
										   UINT8 start2, UINT8 count2, PINT32 pos2,
										   ENG_TEED enable, BOOL clear)
{
	if (!IsConnected())	return FALSE;
	return g_pMvencThread->ReqWriteAreaTrigPos(direct,
											  start1, count1, pos1,
											  start2, count2, pos2,
											  enable, clear);
}

/*
 desc : Ư�� ä�ο� Ʈ���� ��ġ ���
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		start	- [in]  Ʈ���� ���� ��ġ (0 ~ 15)
		count	- [in]  Ʈ���� ��� ���� (1 ~ 16)
		pos		- [in]  Ʈ���� ��� ��ġ�� ����Ǿ� �ִ� �迭 ������ (unit : 100 nm or 0.1 um)
		enable	- [in]  Ʈ���� Enable or Disable
		clear	- [in]  Ʈ���� ��ġ ����ϸ鼭 ���� Trigger Encoder ����
						Clear (TRUE)�Ұ����� Ȥ�� �б� ��� (FALSE)�� ������ ������ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_ReqWriteAreaTrigPosCh(UINT8 cam_id, UINT8 start, UINT8 count, PINT32 pos,
											 ENG_TEED enable, BOOL clear)
{
	if (!IsConnected())	return FALSE;
	return g_pMvencThread->ReqWriteAreaTrigPosCh(cam_id, start, count, pos, enable, clear);
}

/*
 desc : Trigger & Strobe Enable or Disable
 parm : enable	- [in]  TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_ReqTriggerStrobe(BOOL enable)
{
	if (!IsConnected())	return FALSE;
	return g_pMvencThread->ReqWriteTriggerStrobe(enable);
}

/*
 ������� ����
 desc : IP �����ϱ�
 parm : ip_addr	- [in]  ��Ŷ ������ ����� ����ü ������
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_ReqIP4Addr(PUINT8 ip_addr)
{
	if (!IsConnected())	return FALSE;
	return TRUE;
}

/*
 desc : EEPROM Write
 parm : mode	- [in]  0x01 - Read, 0x02 - Write
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_ReqEEPROMCtrl(UINT8 mode)
{
	if (!IsConnected())	return FALSE;
	return g_pMvencThread->ReqEEPROMCtrl(mode);
}

/*
 desc : Board S/W Reset
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_ReqBoardReset()
{
	if (!IsConnected())	return FALSE;
	return g_pMvencThread->ReqWriteBoardReset();
}

/*
 desc : ���� Ʈ���� ����
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
		lamp_type	- [in]  ���� ���� Ÿ�� (0:Ring or 1:Coaxial)
 retn : TRUE or FALSE
 note : Camera 1 : 1 CH (Lamp: Coxial Type), 2 CH (Lamp: Ring Type)
		Camera 2 : 3 CH (Lamp: Coxial Type), 4 CH (Lamp: Ring Type)
*/
API_EXPORT BOOL uvMvenc_ReqEncoderLive(UINT8 cam_id, UINT8 lamp_type)
{
	BOOL bSucc		= FALSE;
	UINT32 u32EncOut= GetTrigEncOutVal(cam_id, lamp_type);

	if (!IsConnected())	return FALSE;
#if 0
	switch (g_pstConfig->set_comn.strobe_lamp_type)
	{
	case 0x00	:	/* ���� 1 (Lamp: Coaxial) */
		switch (cam_id)
		{
		case 0x01	:	u32OnOff = 0x01000000;	break;	/* Trigger Board: 1 CH Enable */
		case 0x02	:	u32OnOff = 0x00000100;	break;	/* Trigger Board: 3 CH Enable */
		}
		break;
	case 0x01	:	/* ���� 2 (Lamp: Ring) */
		switch (cam_id)
		{
		case 0x01	:	u32OnOff = 0x00010000;	break;	/* Trigger Board: 2 CH Enable */
		case 0x02	:	u32OnOff = 0x00000001;	break;	/* Trigger Board: 4 CH Enable */
		}
		break;
	}
#endif
	return g_pMvencThread->ReqWriteEncoderOut(u32EncOut);
}

/*
 desc : ���� Ʈ���� ���� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_ReqEncoderOutReset()
{
	if (!IsConnected())	return FALSE;
	return g_pMvencThread->ReqWriteEncoderOut(0x00000000);
}

/*
 desc : Ʈ���� 1�� �߻� ��Ŵ (Only Trigger Event)
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
		lamp_type	- [in]  ���� ���� Ÿ�� (0:Ring or 1:Coaxial)
 retn : TRUE or FALSE
 note : Camera 1 : 1 CH (Lamp: Coxial Type), 2 CH (Lamp: Ring Type)
		Camera 2 : 3 CH (Lamp: Coxial Type), 4 CH (Lamp: Ring Type)
*/
API_EXPORT BOOL uvMvenc_ReqTrigOutOneOnly(UINT8 cam_id, UINT8 lamp_type)
{
	BOOL bSucc	= FALSE;
	UINT32 u32EncOut= GetTrigEncOutVal(cam_id, lamp_type);

	if (!IsConnected())	return FALSE;
#if 0
	switch (g_pstConfig->set_comn.strobe_lamp_type)
	{
	case 0x00	:	/* ���� 1 (ä�� ��ȣ�� �ƴ�) */
		if (0x01 == ch_no)	bSucc = g_pMvencThread->ReqWriteTrigOutOne(0x01000000);
		else				bSucc = g_pMvencThread->ReqWriteTrigOutOne(0x00000100);	break;
	case 0x01	:	/* ���� 2 (ä�� ��ȣ�� �ƴ�) */
		if (0x01 == ch_no)	bSucc = g_pMvencThread->ReqWriteTrigOutOne(0x00010000);
		else				bSucc = g_pMvencThread->ReqWriteTrigOutOne(0x00000001);	break;
	}
#endif
	//return bSucc = g_pMvencThread->ReqWriteTrigOutOne(u32EncOut);
	return bSucc = g_pMvencThread->ReqWriteTrigOutOne(cam_id);
}

/*
 desc : Ʈ���� 1�� �߻� ��Ŵ (Trigger Enable -> Trigger Event -> Trigger Disable)
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
		lamp_type	- [in]  ���� ���� Ÿ�� (0:Ring or 1:Coaxial)
		enable	- [in]  Ʈ���� ������ ���� Disable ��ų�� ����
						Ʈ���� 1�� �߻� ��, ��ٷ� Ʈ���� Disable ���� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_ReqTrigOutOne(UINT8 cam_id, UINT8 lamp_type, BOOL enabl)
{
	BOOL bSucc	= FALSE;
	UINT32 u32EncOut= GetTrigEncOutVal(cam_id, lamp_type);

	if (!IsConnected())	return FALSE;
	/* Trigger & Strobe : Enabled */
	//bSucc	= uvMvenc_ReqTriggerStrobe(TRUE);
#if 0
	/* Trigger Out : one pulse */
	if (bSucc)
	{
		switch (g_pstConfig->set_comn.strobe_lamp_type)
		{
		case 0x00	:	/* ���� 1 (ä�� ��ȣ�� �ƴ�) */
			if (0x01 == cam_id)	bSucc = g_pMvencThread->ReqWriteTrigOutOne(0x01000000);
			else				bSucc = g_pMvencThread->ReqWriteTrigOutOne(0x00000100);	break;
		case 0x01	:	/* ���� 2 (ä�� ��ȣ�� �ƴ�) */
			if (0x01 == cam_id)	bSucc = g_pMvencThread->ReqWriteTrigOutOne(0x00010000);
			else				bSucc = g_pMvencThread->ReqWriteTrigOutOne(0x00000001);	break;
		}
	}
#else
	//bSucc = g_pMvencThread->ReqWriteTrigOutOne(u32EncOut);
//	bSucc = g_pMvencThread->ReqWriteTrigOutOne(cam_id);
	bSucc = g_pMvencThread->ReqWriteTrigOutOne_(0b1000 | (1 << (cam_id-1)));

#endif
	/* Trigger & Strobe : Disabled */
	//if (bSucc && enabl)	bSucc = uvMvenc_ReqTriggerStrobe(FALSE);

	return bSucc;
}

API_EXPORT BOOL uvMvenc_ReqTrigOutOne_(UINT8 channelBit)
{
	if (!IsConnected())	return FALSE;
	BOOL bSucc = g_pMvencThread->ReqWriteTrigOutOne_(channelBit);
	return bSucc;
}



/*
 desc : ������ �Էµ� Trigger ��ġ ���� Trigger Board�κ��� ���ŵ� �Է� �� ��
 parm : ch_no	- [in]  ä�� ��ȣ (0x01 ~ 0x04)
		index	- [in]  Ʈ���� ���� ��ġ (0x000 ~ 0x0f)
		pos		- [in]  Ʈ���� ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_IsTrigPosEqual(UINT8 ch_no, UINT8 index, UINT32 pos)
{
	if (!IsConnected())	return FALSE;
	return g_pMvencThread->IsTrigPosEqual(ch_no, index, pos);
}

/*
 desc : ī�޶� �� ���� Ʈ���� �߻��Ͽ� Live ���� ����
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		enable	- [in]  TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE

*/
API_EXPORT BOOL uvMvenc_ReqWirteTrigLive(UINT8 cam_id, BOOL enable)
{
	if (!IsConnected())	return FALSE;
	return g_pMvencThread->ReqWirteTrigLive(cam_id, enable);
}

/*
 ������� ����
 desc : ���� Ʈ���ſ� �۽ŵ� ��� ������ ��� ������ ���� ���� �������� ����
 parm : count	- [in]  �� ���� �̸��̸� TRUE, �̻��̸� FALSE
 retn : count ���� �̸� (������)�̸� TRUE, �̻� (����)�̸� FALSE ��ȯ
*/
API_EXPORT BOOL uvMvenc_IsSendCmdCountUnder(UINT16 count)
{
	return TRUE;
}

#ifdef __cplusplus
}
#endif