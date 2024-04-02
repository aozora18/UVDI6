
/*
 desc : TCP/IP Client Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "MvencThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : trig	- [in]  Trigger Server ��� ����
		conf	- [in]  Trigger ȯ�� ����
		shmem	- [in]  Trigger Shared Memory
		comn	- [in]  ��ü ���� ȯ�� ����
 retn : None
*/
CMvencThread::CMvencThread(BYTE byPort, LPG_CTSP conf, LPG_TPQR shmem, LPG_DLSM link, LPG_CCGS comn)
{
	m_handle		= NULL;
	m_byPort		= byPort;
	/* ------------------------------------------- */
	/* RunWork �Լ� ������ �ٷ� ���� �ϵ��� ó���� */
	/* ------------------------------------------- */
	m_bInitRecv		= FALSE;
	m_u64SendTime	= GetTickCount64();
	
	m_pstConfTrig	= conf;
	m_pstShMemTrig	= shmem;
	m_pstShMemLink	= link;
	m_pstConfComn	= comn;

	/* ���� �ֱ� �� �ʱ�ȭ */
	UINT8 i, j;
	for (i=0; i<MAX_TRIG_CHANNEL; i++)
	{
		for (j=0; j<MAX_TRIG_CHANNEL; j++)
		{
			m_u32SendTrigPos[i][j]	= MAX_TRIG_POS;
		}
	}
	MvsEncSetPositiveRun(m_handle, 15);

#if (TRIG_RECV_OUTPUT)
	m_u64TraceTime	= 0;
#endif
}

/*
 desc : Destructor
 parm : None
*/
CMvencThread::~CMvencThread()
{
	//CloseHandle();
}

/*
 desc : ������ ����� �ʱ� �ѹ� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMvencThread::StartWork()
{
	return TRUE;
}

/*
 desc : ������ ����� �ֱ��� ȣ���
 parm : None
 retn : None
*/
VOID CMvencThread::RunWork()
{
	BOOL bSendData	= FALSE;
	/* ���� �۾� �ð� */
	UINT64 u64Tick	= GetTickCount64();

	if (NULL == m_handle)
	{
		m_handle = MvsEncCreateHandler(); //�ڵ鷯 ����
		char szPort[5] = { NULL };
		sprintf_s(szPort, _countof(szPort), "COM%d", m_byPort);
		MvsEncOpen(m_handle, szPort); //�ø��� ��Ʈ ����
		if (MvsEncIsOpen(m_handle) != true) //���� üũ
		{
			//�������
			m_handle = NULL;
			return;
		}

		// �ʱⰪ ����
		for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
		{

			m_stTrigCtrl[i].EncoderType				= 2;	//0:DIFF, 1:TTL, 2:Virtual Encoder
			m_stTrigCtrl[i].EncoderCh				= i;	//Encoder CH (0~3)
			m_stTrigCtrl[i].Multi					= 4;	//Encoder Multi(1:x1, 2:x2, 4:x4)
			m_stTrigCtrl[i].CondFactor				= 7;	//0:DI0 ~ 6:DI6, 7:High, 8:Low
			m_stTrigCtrl[i].TriggerOutputOperator	= 0;	//0:AND, 1:OR, 2:XOR, 3:NAND
			m_stTrigCtrl[i].EncoderResetFactor		= 0;	//0:DISABLE, 1:DI0 ~ 7:DI6, 8:CW, 9:CCW
			m_stTrigCtrl[i].PositionDirection		= 1;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
			m_stTrigCtrl[i].Temp = 0;
			m_stTrigCtrl[i].Correction				= 0;	//0:DISABLE, 1:ENABLE
			m_stTrigCtrl[i].TriggerBase				= 0;	//0:COUNT, 1:POSITION
			UINT uiStatus = MvsEncSetTriggerControls(m_handle, i, &m_stTrigCtrl[i]);//����ü ����
			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerControls API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			}
		}

		m_u64SendTime = GetTickCount64();
		m_bInitRecv = TRUE;
	}

	/* ------------------------------------------------------------------ */
	/* �ֱ������� Trigger�� ���� ���� �о���� �ֱ�� 1000 msec ���� ���� */
	/* ------------------------------------------------------------------ */
	if (u64Tick > (m_u64SendTime + 250))
	{
		bSendData	= TRUE;	/* Trigger ���� �� ��û ��Ŷ �۽� */
		ReqReadSetup();
	}
}

/*
 desc : ������ ����� �ѹ� ȣ���
 parm : None
 retn : None
*/
VOID CMvencThread::EndWork()
{
	if (NULL != m_handle)
	{
		CloseHandle();
	}
}


BOOL CMvencThread::IsConnected()
{
	if (NULL == m_handle)
	{
		return FALSE;
	}

	if (MvsEncIsOpen(m_handle))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CMvencThread::CloseHandle()
{
	MvsEncClose(m_handle);			//��Ʈ �ݱ�
	MvsEncCloseHandler(m_handle);	//�ڵ鷯 ����
	MvsEncRelease();				//���̺귯�� ����

	m_handle = NULL;
}

/*
 desc : Camera Index (1 or 2)�� ���� ä�� ��ȣ �� ��ȯ
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
 retn : Camera Index�� �ش�Ǵ� Trigger Board�� Channel ID (0x01 ~ 0x04) �� ��ȯ
		0x00 ���̸� ���� ó��
*/
UINT8 CMvencThread::GetTrigChNo(UINT8 cam_id)
{
#if 1
	UINT8 u8Type	= m_pstConfComn->strobe_lamp_type;
	if (0x01 == cam_id)
	{
		if (!u8Type)	return 0x01;
		else			return 0x02;
	}
	else
	{
		if (!u8Type)	return 0x03;
		else			return 0x04;
	}
#else	/* Trigger Event�� ������ 1 or 3 CH������ �߻����Ѿ� �� (���� DI ��� ������ ����) */
	if (0x01 == cam_id)	return 0x01;
	else				return 0x03;
#endif

	return 0x00;
}


/*
 desc : Trigger Board�� ������ �� ��û
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMvencThread::ReqReadSetup()
{
	UINT uiStatus		= 0;
	BOOL bSucc			= TRUE;
	STG_TPQR stPktData	= { NULL };
	/* ���� ���� */
	//if (m_syncSend.Enter())
	//{
	//	/* Command */
	//	//UINT32				command;					/* Read/Write : Low 1 Byte, ENG_TBPC <01 : Write, 02 : Read, 07 : IP Write> */
	//	/* ���� ���ڴ� ���� �о� ���ų� ���� �� ���� */
	//	//INT32				enc_counter;				/* ENG_TERR <Read : 4 Bytes, Write : Low 1 Byte, Encoder Counter Read / Reset> */
	//	/* Trigger, Strobe ��� Enable or Disable */
	//	//UINT32				trig_strob_ctrl;			/* ENG_TSED <Read/Write : Low 1 Bytes, 4ä�� ���� ���� <0x01 : Enable, 0x03 : Disable>> */
	//	/* Trigger Position, OnTime, Delay, Encoder Plus or Minus */
	//	//STG_TPTS			trig_set[MAX_TRIG_CHANNEL];	/* 4 ä�� �� */
	//	/* Trigger Board�� IP Address */
	//	//UINT8				ip_addr[4];					/* IP �ּ� ���� ������� 1 Bytes �� �� */
	//	/* Encoder Output Value */
	//	//UINT32				enc_out_val;				/* Encoder ���� Ʈ���� �߻� ���� */
	//	
	//	memcpy(&stPktData, m_pstShMemTrig, sizeof(STG_TPQR));
	//	for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
	//	{
	//		stPktData.command = UINT32(ENG_TBPC::en_read);
	//		uiStatus = MvsEncGetTriggerControls(m_handle, i, &m_stTrigCtrl[i]);//����ü ����
	//		if (uiStatus != 0x00)
	//		{
	//			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
	//			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncGetTriggerControls API Error (status:%d)", uiStatus);
	//			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
	//			bSucc = FALSE;
	//		}

	//		uiStatus = MvsEncGetEncoderPosition(m_handle, i, &stPktData.enc_counter); //���� ��ġ�� ������
	//		if (uiStatus != 0x00)
	//		{
	//			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
	//			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncGetEncoderPosition API Error (status:%d)", uiStatus);
	//			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
	//			bSucc = FALSE;
	//		}

	//		//	area_trig_pos[MAX_REGIST_TRIG_POS_COUNT];	/* ������ �ʴ� ä���� ���� �ִ� �� ���� MAX_UINT32 (unit : 100 nm or 0.1 um) */
	//		//	area_trig_ontime;							/* High�� �����Ǵ� ���� �Է� (1 �� = 1 ns) */
	//		//	area_strob_ontime;							/* Hight�� �����Ǵ� ���� �Է� (1 �� = 1 ns) */
	//		//	area_strob_trig_delay;						/* ��µǴ� Trigger�� Delay ���� �Է� (1 ns) */
	//		//	area_trig_pos_plus;							/* Area Trigger Position (1 ~ 16)�� ���� �� Register ���� �����Ǿ� Strobe�� Trigger ��ȣ�� ��µ� */
	//		//												/* ex> area_trig_pos = 100�̰�, �� Register ���� 10�̸� Encoder�� 110���� Strobe�� Trigger�� ��µ� */
	//		//	area_trig_pos_minus;						/* Area Trigger Position (1 ~ 16)�� ���� �� Register ���� ���ҵǾ� Strobe�� Trigger ��ȣ�� ��µ� */
	//														/* ex> area_trig_pos = 100�̰�, �� Register ���� 10�̸� Encoder�� 90���� Strobe�� Trigger�� ��µ� */

	//		int		nTrigPosition = 0;
	//		UINT	nTrigCount = 0;
	//		uiStatus = MvsEncGetTriggerCount(m_handle, i, &nTrigCount);
	//		if (uiStatus != 0x00)
	//		{
	//			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
	//			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncGetTriggerCount API Error (status:%d)", uiStatus);
	//			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
	//			bSucc = FALSE;
	//		}

	//		for (int posID = 0; posID < MAX_REGIST_TRIG_POS_COUNT/*Count ��ŭ�� �о�� ��� ���� �ʿ�*/; posID++)
	//		{
	//			uiStatus = MvsEncGetIndexTriggerPosition(m_handle, i, posID, &nTrigPosition);
	//			if (uiStatus != 0x00)
	//			{
	//				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
	//				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncGetIndexTriggerPosition API Error (status:%d)", uiStatus);
	//				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
	//				bSucc = FALSE;
	//			}
	//			stPktData.trig_set[i].area_trig_pos[posID] = nTrigPosition;
	//		}

	//		double	dPulseWidth = 0;
	//		uiStatus = MvsEncGetTriggerPulseWidth(m_handle, i, &dPulseWidth);
	//		if (uiStatus != 0x00)
	//		{
	//			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
	//			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncGetTriggerPulseWidth API Error (status:%d)", uiStatus);
	//			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
	//			bSucc = FALSE;
	//		}
	//		stPktData.trig_set[i].area_trig_ontime = UINT32(dPulseWidth * 1000.0);	// um
	//		stPktData.trig_set[i].area_strob_ontime = UINT32(dPulseWidth * 1000.0);	// um
	//		
	//		double	dDelay = 0;
	//		uiStatus = MvsEncGetTriggerDelay(m_handle, i, &dDelay);
	//		if (uiStatus != 0x00)
	//		{
	//			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
	//			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncGetTriggerDelay API Error (status:%d)", uiStatus);
	//			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
	//			bSucc = FALSE;
	//		}

	//		stPktData.trig_set[i].area_strob_trig_delay = UINT32(dDelay * 1000.0);	// um
	//	}

	//	// by sysandj : �����ʿ� => stPktData.trig_strob_ctrl 
	//	// by sysandj : �����ʿ� => stPktData.trig_set[i]area_trig_pos_plus 
	//	// by sysandj : �����ʿ� => stPktData.trig_set[i]area_trig_pos_minus 
	//	// by sysandj : �����ʿ� => stPktData.enc_out_val 

	//	if (bSucc)
	//	{
	//		memcpy(m_pstShMemTrig, &stPktData, sizeof(stPktData));
	//	}
	//	m_u64SendTime = GetTickCount64();
	//	/* ���� ���� */
	//	m_syncSend.Leave();
	//}

	return bSucc;
}

/*
 desc : Ư���� ��쿡�� ���. (Ʈ���ź����� Ʈ���Ŵ� 2�� �ۿ� �߻��ȵǱ� ������...)
 parm : None
 retn : TRUE or FALSE
 note : Ʈ���Ŵ� ������ 1, 3 �� ī�޶�(����;ä��)�� �߻��ϱ� ������...
		2, 4�� ī�޶� (����;ä��)�� �߻���Ű�� ���� ���
*/
BOOL CMvencThread::ReqInitUpdate()
{
	BOOL	bSucc = TRUE;
	UINT	uiStatus = 0;
	UINT8	u8LampType	= m_pstConfComn->strobe_lamp_type;	/* 0x00: Coaxial, 0x01: Ring */
	UINT32 i = 0, j		= 0;

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		/* ���� ���ŵ� ������ �ʱ�ȭ */
		m_pstShMemTrig->ResetPktData();

		/* 1th : Base Setup */
		uiStatus = MvsEncClearAll(m_handle); // ��� ī��Ʈ �ʱ�ȭ
		m_pstShMemTrig->command			= UINT32(ENG_TBPC::en_write);
		m_pstShMemTrig->enc_counter		= INT32(ENG_TECM::en_clear);
		m_pstShMemTrig->trig_strob_ctrl	= UINT32(ENG_TEED::en_disable);


	
		if (uiStatus != 0x00)
		{
			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncClearAll API Error (status:%d)", uiStatus);
			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			bSucc = FALSE;
		}
		m_u64SendTime = GetTickCount64();
		/* ���� ���� */
		m_syncSend.Leave();
	}
	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : ���� ��ϵ� 4�� ä�ο� ���� Trigger Position �� �ʱ�ȭ (�ִ� ��) ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMvencThread::ResetTrigPosAll()
{
	BOOL bSucc			= TRUE;
	UINT uiStatus		= 0;
	UINT32 i = 0, j		= 0;

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		MvsEncClearAll(m_handle);
		///* �ʱ� �� ���� */
		//for (; i<MAX_TRIG_CHANNEL; i++)
		//{
		//	/* �ʱ� Ʈ���� �߻� ��ġ ���� 0�� �ƴ� MAX ������ ���� */
		//	for (j=0; j<MAX_REGIST_TRIG_POS_COUNT; j++)	m_pstShMemTrig->trig_set[i].area_trig_pos[j]	= MAX_TRIG_POS;
		//}
		///* �ֱ� Ʈ���� ��ġ �� �ʱ�ȭ (�ݵ�� ��Ŷ ���ۿ� �ִ°ͺ��� �տ� �����ؾ� ��) */
		//memset(m_u32SendTrigPos, MAX_TRIG_POS, sizeof(UINT32)*MAX_TRIG_CHANNEL*MAX_REGIST_TRIG_POS_COUNT);

		//uiStatus				= MvsEncClearTriggerAll(m_handle); // Ʈ������� ī��Ʈ �ʱ�ȭ
		//m_pstShMemTrig->command = (UINT32)ENG_TBPC::en_write;
		//if (uiStatus != 0x00)
		//{
		//	TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
		//	swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncClearTriggerAll API Error (status:%d)", uiStatus);
		//	LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
		//	bSucc = FALSE;
		//}
		m_u64SendTime = GetTickCount64();
		/* ���� ���� */
		m_syncSend.Leave();
	}

	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : Trigger Board�� ����ǰ� �� ���� EEPROM ������ �а�/���� ����
 parm : mode	- [in]  0x01 - Read, 0x02 - Write
 retn : TRUE or FALSE
*/
BOOL CMvencThread::ReqEEPROMCtrl(UINT8 mode)
{
	BOOL bSucc			= TRUE;
	UINT uiStatus		= 0;
	UINT32 u32Cmd		= mode == 0x01 ? UINT32(ENG_TBPC::en_eeprom_read) : UINT32(ENG_TBPC::en_eeprom_write);

	/* ���� EEPROM���κ��� �б� ����� ���, ���� �� ��� ����� */
	if (0x01 == mode)	m_pstShMemTrig->ResetPktData();

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		/* �ʱ⿡ EEPROM�� ����� ���� �������� */
		uiStatus = MvsEncLoadFlash(m_handle); // �÷��ø޸𸮿��� ������ ��������
		m_pstShMemTrig->command = u32Cmd;
		if (uiStatus != 0x00)
		{
			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncLoadFlash API Error (status:%d)", uiStatus);
			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			bSucc = FALSE;
		}
		m_u64SendTime = GetTickCount64();
		/* ���� ���� */
		m_syncSend.Leave();
	}

	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : Board Reset
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMvencThread::ReqWriteBoardReset()
{
	BOOL bSucc			= TRUE;
	UINT uiStatus		= 0;

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		/* ��ɾ� ���� */
		uiStatus = MvsEncLoadDefaultParameters(m_handle); // �����ʱ�ȭ
		m_pstShMemTrig->command = UINT32(ENG_TBPC::en_reset);
		if (uiStatus != 0x00)
		{
			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncLoadFlash API Error (status:%d)", uiStatus);
			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			bSucc = FALSE;
		}
		m_u64SendTime = GetTickCount64();
		/* ���� ���� */
		m_syncSend.Leave();
	}

	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : Area Trigger Event ����
 parm : direct	- [in]  TRUE: ������, FALSE: ������
		trig_set- [out] Trigger Event ���� ���� �� ��ȯ
 retn : None
 note : Trigger Event�� �޴� ���� ä�� 1/3 �����̰�, Strobe Event�� �޴� ���� ä�� 1/2/3/4 ��� ����
		ä�� 2/4���� ���� �̹����� �������� ä�� 1/3������ Trigger �̺�Ʈ �߻� ���Ѿ� ��
*/
VOID CMvencThread::SetTrigEvent(BOOL direct, LPG_TPTS trig_set)
{
	UINT8 u8LampType= m_pstConfComn->strobe_lamp_type;	/* 0x00: Coaxial, 0x01: Ring */
	UINT32 i = 0;

	/* �ʱ� �� ���� */
	for (i=0x00; i<0x04; i++)
	{
		trig_set[i].area_trig_ontime		= m_pstConfTrig->trig_on_time[i];
		trig_set[i].area_strob_ontime		= m_pstConfTrig->strob_on_time[i];
		trig_set[i].area_strob_trig_delay	= m_pstConfTrig->trig_delay_time[i];
		trig_set[i].area_trig_pos_plus		= 0/*m_pstConfTrig->trig_plus[i]*/;
		trig_set[i].area_trig_pos_minus		= 0/*m_pstConfTrig->trig_minus[i]*/;
	}
}

/*
 desc : Area Trigger Position ����
 parm : trig_set- [in]  Trigger Position ����
		start	- [in]  Ʈ���� ���� ��ġ (0 ~ 15)
		count	- [in]  Ʈ���� ��� ���� (1 ~ 16)
		pos		- [in]  Ʈ���� ��� ��ġ�� ����Ǿ� �ִ� �迭 ������ (unit : 100 nm or 0.1 um)
 note : Trigger Event�� �޴� ���� ä�� 1/3 �����̰�, Strobe Event�� �޴� ���� ä�� 1/2/3/4 ��� ����
		ä�� 2/4���� ���� �̹����� �������� ä�� 1/3������ Trigger �̺�Ʈ �߻� ���Ѿ� ��
*/
VOID CMvencThread::SetTrigPos(LPG_TPTS trig_set,
							 UINT8 start1, UINT8 count1, PINT32 pos1,
							 UINT8 start2, UINT8 count2, PINT32 pos2)
{

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	UINT8 u8Cam1No[2] = { 1, 2 };
	UINT8 u8Cam2No[2] = { 0, 3 };

	/*Cam1 Cam2 ������ ����*/
	memcpy(&trig_set[u8Cam1No[0]].area_trig_pos + start1, pos1, sizeof(UINT32) * count1);
	memcpy(&trig_set[u8Cam2No[0]].area_trig_pos + start2, pos2, sizeof(UINT32) * count2);
	/*����1 ����2 ������ ����*/
	memcpy(&trig_set[u8Cam1No[1]].area_trig_pos + start1, pos1, sizeof(UINT32) * count1);
	memcpy(&trig_set[u8Cam2No[1]].area_trig_pos + start2, pos2, sizeof(UINT32) * count2);

#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)

	/*Cam1 ������ ����*/
	memcpy(&trig_set[0].area_trig_pos + start1, pos1, sizeof(UINT32) * count1);
	/*Cam2 ������ ����*/
	memcpy(&trig_set[1].area_trig_pos + start2, pos2, sizeof(UINT32) * count2);
	/*���� ���� ����*/

	vector<INT32> sorting;

	for (int i = 0; i < count1; i++)
		sorting.push_back(pos1[i]);
	
	for (int i=0; i < count2; i++)
		sorting.push_back(pos2[i]);
	
	std::sort(sorting.begin(), sorting.end(), [&](INT32& a, INT32& b) {return a < b; });
	
	memcpy(&trig_set[3].area_trig_pos + start1, sorting.data(), sizeof(UINT32) * (count1 + count2));
#endif
}

/*
 desc : ä�� ���� Ʈ���� ��ġ ���
 parm : direct	- [in]  TRUE: ������, FALSE: ������
		start	- [in]  Ʈ���� ���� ��ġ (0 ~ 15)
		count	- [in]  Ʈ���� ��� ���� (1 ~ 16)
		pos		- [in]  Ʈ���� ��� ��ġ�� ����Ǿ� �ִ� �迭 ������ (unit : 100 nm or 0.1 um)
		enable	- [in]  Ʈ���� Enable or Disable
		clear	- [in]  Ʈ���� ��ġ ����ϸ鼭 ���� Trigger Encoder ����
						Clear (TRUE)�Ұ����� Ȥ�� �б� ��� (FALSE)�� ������ ������ ����
 retn : TRUE or FALSE
*/
BOOL CMvencThread::ReqWriteAreaTrigPos(BOOL direct,
	UINT8 start1, UINT8 count1, PINT32 pos1,
	UINT8 start2, UINT8 count2, PINT32 pos2,
	ENG_TEED enable, BOOL clear)
{
	UINT8 i1 = 0, i2 = 0, u3MaxTrig = MAX_REGIST_TRIG_POS_COUNT;
	UINT8 u8ChNo[2] = { NULL };

	UINT8 u8Cam1No[2] = { 1, 2 };
	UINT8 u8Cam2No[2] = { 0, 3 };
	BOOL bSucc = TRUE;
	UINT uiStatus = 0;
	UINT uiStatus2 = 0;
	UINT uiStatus3 = 0;
	STG_TPQR stPktData = { NULL };

	/* �� Align Camera�� ���� ä�� ��ȣ ��� */
	//u8ChNo[0]	= GetTrigChNo(0x01);
//	u8ChNo[1]	= GetTrigChNo(0x02);

	if ((!(start1 >= 0 && start1 < MAX_REGIST_TRIG_POS_COUNT) || !pos1 ||
		(start1 + count1) > MAX_REGIST_TRIG_POS_COUNT) ||
		(!(start2 >= 0 && start2 < MAX_REGIST_TRIG_POS_COUNT) || !pos2 ||
			(start2 + count2) > MAX_REGIST_TRIG_POS_COUNT))
	{
		TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
		swprintf_s(tzMesg, LOG_MESG_SIZE,
			L"Trigger Invalid Parameter (start1:%d count1:%d start2:%d count2:%d)",
			start1, count1, start2, count2);
		LOG_ERROR(ENG_EDIC::en_mvenc, tzMesg);
		return FALSE;
	}
	//u8ChNo[0]--;
	//u8ChNo[1]--;

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		m_pstShMemTrig->command = (UINT32)ENG_TBPC::en_write;/* Write */
		m_pstShMemTrig->trig_strob_ctrl = (UINT32)enable;			 /* Trigger Enable or Disable */

#if 0	
		/*Cam1 Seting*/
		/*Mode ����*/
		m_stTrigCtrl[u8Cam1No[0]].EncoderType = (UINT16)0;	//0:DIFF, 1:TTL, 2:Virtual Encoder
		m_stTrigCtrl[u8Cam1No[1]].EncoderType = (UINT16)0;	//0:DIFF, 1:TTL, 2:Virtual Encoder
		// by sysandj: Position ���� ����
		m_stTrigCtrl[u8Cam1No[0]].PositionDirection = (UINT16)enable;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
		m_stTrigCtrl[u8Cam1No[1]].PositionDirection = (UINT16)enable;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
		/*ä�� ch0���� ����[MC2�� ����� ä���̶� ������ Ʈ���ſ� ���� �ϱ� ����]*/
		m_stTrigCtrl[u8Cam1No[0]].EncoderCh = 0;						//Encoder CH (0~3)
		m_stTrigCtrl[u8Cam1No[1]].EncoderCh = 0;						//Encoder CH (0~3)
		/*Ʈ���� ���� �ʱ� ������ ����*/
		uiStatus = MvsEncSetTriggerControls(m_handle, u8Cam1No[0], &m_stTrigCtrl[u8Cam1No[0]]);	//����ü ����
		uiStatus2 = MvsEncSetTriggerControls(m_handle, u8Cam1No[1], &m_stTrigCtrl[u8Cam1No[1]]);	//����ü ����
		if (uiStatus != 0x00 && uiStatus2 != 0x00)
		{
			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerControls API Error (status:%d)", uiStatus);
			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			bSucc = FALSE;
		}
		
		
		/*Cam2 Seting*/
		/*Mode ����*/
		m_stTrigCtrl[u8Cam2No[0]].EncoderType = (UINT16)0;	//0:DIFF, 1:TTL, 2:Virtual Encoder
		m_stTrigCtrl[u8Cam2No[1]].EncoderType = (UINT16)0;	//0:DIFF, 1:TTL, 2:Virtual Encoder
		// by sysandj: Position ���� ����
		m_stTrigCtrl[u8Cam2No[0]].PositionDirection = (UINT16)enable;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
		m_stTrigCtrl[u8Cam2No[1]].PositionDirection = (UINT16)enable;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
		/*ä�� ch0���� ����[MC2�� ����� ä���̶� ������ Ʈ���ſ� ���� �ϱ� ����]*/
		m_stTrigCtrl[u8Cam2No[0]].EncoderCh = 0;						//Encoder CH (0~3)
		m_stTrigCtrl[u8Cam2No[1]].EncoderCh = 0;						//Encoder CH (0~3)
		/*Ʈ���� ���� �ʱ� ������ ����*/
		uiStatus = MvsEncSetTriggerControls(m_handle, u8Cam2No[0], &m_stTrigCtrl[u8Cam2No[0]]);	//����ü ����
		uiStatus2 = MvsEncSetTriggerControls(m_handle, u8Cam2No[1], &m_stTrigCtrl[u8Cam2No[1]]);//����ü ����
		if (uiStatus != 0x00 && uiStatus2 != 0x00)
		{
			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerControls API Error (status:%d)", uiStatus);
			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			bSucc = FALSE;
		}
#endif
		SetPositionTrigMode(enable);

		/*Cam Seting End*/

		/* ���� Area Trigger �� ��� ���� */
		memcpy(stPktData.trig_set, m_pstShMemTrig->trig_set, sizeof(STG_TPTS) * MAX_TRIG_CHANNEL /* Max 4 channel */);

		/* Ʈ���� �̺�Ʈ ���� */
		SetTrigEvent(direct, stPktData.trig_set);
		/* Ʈ���� ��ġ ���� */
		SetTrigPos(stPktData.trig_set, start1, count1, pos1, start2, count2, pos2);
		/* ������ Ʈ���� ��� ��ġ�� �ִ� �� ���� */
		for (i1 = 0; i1 < start1; i1++)						stPktData.trig_set[u8ChNo[0]].area_trig_pos[i1] = MAX_TRIG_POS;
		for (i1 = start1 + count1; i1 < u3MaxTrig; i1++)	stPktData.trig_set[u8ChNo[0]].area_trig_pos[i1] = MAX_TRIG_POS;
		for (i2 = 0; i2 < start2; i2++)						stPktData.trig_set[u8ChNo[1]].area_trig_pos[i2] = MAX_TRIG_POS;
		for (i2 = start2 + count1; i2 < u3MaxTrig; i2++)	stPktData.trig_set[u8ChNo[1]].area_trig_pos[i2] = MAX_TRIG_POS;


		// Trigger Offset ����
		UINT32				area_trig_pos_offset[MAX_TRIG_CHANNEL] = { 0 };
		if (ENG_TEED::en_positive == enable)
		{
			for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
			{
				area_trig_pos_offset[i] = stPktData.trig_set[i].area_trig_pos_plus;
			}
		}
		else if (ENG_TEED::en_negative == enable)
		{
			for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
			{
				area_trig_pos_offset[i] = stPktData.trig_set[i].area_trig_pos_minus;
			}
		}
		// Trigger Offset ����

		for (int posID = 0; posID < MAX_REGIST_TRIG_POS_COUNT; posID++)
		{
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
			// Trigger Position count�� �ִٸ� �� ȿ�������� ��� ����
			/*Cam1 �ټ��� Trigger Position ����*/
			uiStatus = MvsEncSetIndexTriggerPosition(m_handle, u8Cam1No[0], posID, stPktData.trig_set[u8Cam1No[0]].area_trig_pos[posID] + area_trig_pos_offset[u8Cam1No[0]]);
			uiStatus2 = MvsEncSetIndexTriggerPosition(m_handle, u8Cam1No[1], posID, stPktData.trig_set[u8Cam1No[1]].area_trig_pos[posID] + area_trig_pos_offset[u8Cam1No[1]]);

			if (uiStatus != 0x00 && uiStatus2 != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetIndexTriggerPosition API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}

			/*Cam2 �ټ��� Trigger Position ����*/
			uiStatus = MvsEncSetIndexTriggerPosition(m_handle, u8Cam2No[0], posID, stPktData.trig_set[u8Cam2No[0]].area_trig_pos[posID] + area_trig_pos_offset[u8Cam2No[0]]);
			uiStatus2 = MvsEncSetIndexTriggerPosition(m_handle, u8Cam2No[1], posID, stPktData.trig_set[u8Cam2No[1]].area_trig_pos[posID] + area_trig_pos_offset[u8Cam2No[1]]);

			if (uiStatus != 0x00 && uiStatus2 != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetIndexTriggerPosition API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
			// Trigger Position count�� �ִٸ� �� ȿ�������� ��� ����
			/*Cam1 �ټ��� Trigger Position ����*/
			uiStatus = MvsEncSetIndexTriggerPosition(m_handle, 0, posID, stPktData.trig_set[0].area_trig_pos[posID] + area_trig_pos_offset[0]);
			/*Cam2 �ټ��� Trigger Position ����*/
			uiStatus2 = MvsEncSetIndexTriggerPosition(m_handle, 1, posID, stPktData.trig_set[1].area_trig_pos[posID] + area_trig_pos_offset[1]);
			/*���� ���� �ټ��� Trigger Position ����*/
			uiStatus3 = MvsEncSetIndexTriggerPosition(m_handle, 3, posID, stPktData.trig_set[3].area_trig_pos[posID] + area_trig_pos_offset[3]);

			if (uiStatus != 0x00 && uiStatus2 != 0x00 && uiStatus3 != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetIndexTriggerPosition API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}

#endif

		}
		for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
		{
			/*Ʈ���� ����Ŭ Max��*/
			uiStatus = MvsEncSetTriggerGenerator(m_handle, i, MAX_TRIG_POS, MAX_TRIG_POS);
			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerGenerator API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}
		}
		/*�뱤 ���⿡ ���� �� Ʈ���� �߻� ��� ���� ����*/
		if (ENG_TEED::en_positive == enable)
		{
			MvsEncSetPositiveRun(m_handle, 15);
		}
		else if (ENG_TEED::en_negative == enable)
		{
			MvsEncSetNegativeRun(m_handle, 15);
		}


		/*Clear*/
		if (clear)
		{
			/*Position*/
			uiStatus = MvsEncClearEncoderPositionAll(m_handle);
			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncClearEncoderPositionAll API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}
			/*Trigger MvsEncClearTriggerCount*/
			uiStatus = MvsEncClearTriggerAll(m_handle);
			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncClearTriggerAll API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}
		}


		m_u64SendTime = GetTickCount64();
		/* ���� ���� */
		m_syncSend.Leave();
	}

	/* ���� ���� �� �б� ��û */
	return bSucc ? ReqReadSetup() : FALSE;
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
BOOL CMvencThread::ReqWriteAreaTrigPosCh(UINT8 cam_id, UINT8 start, UINT8 count, PINT32 pos,
										ENG_TEED enable, BOOL clear)
{
	UINT8 i =0 ,j		= 0, u8ChNo;
	BOOL bSucc			= TRUE;
	UINT uiStatus		= 0;
	STG_TPQR stPktData	= {NULL};

	/* �� Align Camera�� ���� ä�� ��ȣ ��� */
	u8ChNo	= GetTrigChNo(cam_id);

	if (u8ChNo < 1 || u8ChNo > 4 || !(start >= 0 && start < MAX_REGIST_TRIG_POS_COUNT) || !pos ||
		(start + count) > MAX_REGIST_TRIG_POS_COUNT)
	{
		TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL};
		swprintf_s(tzMesg, LOG_MESG_SIZE,
				   L"Trigger Invalid Parameter (ch_no:%d start:%d count:%d)",
				   u8ChNo, start, count);
		LOG_ERROR(ENG_EDIC::en_mvenc, tzMesg);
		return FALSE;
	}
	u8ChNo--;

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		m_pstShMemTrig->command			= (UINT32)ENG_TBPC::en_write;									/* Write */
		/* Clear to encoder */
		if (clear)
		{
			uiStatus = MvsEncClearEncoderPositionAll(m_handle);
			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncClearEncoderPositionAll API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}
		}

		m_pstShMemTrig->trig_strob_ctrl	= (UINT32)enable;												/* Trigger Enable or Disable */
		// by sysandj: Position ���� ����
		m_stTrigCtrl[u8ChNo].PositionDirection = (UINT16)enable;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
		UINT uiStatus = MvsEncSetTriggerControls(m_handle, u8ChNo, &m_stTrigCtrl[u8ChNo]);//����ü ����
		// by sysandj: Position ���� ����

		/* Ʈ���� �̺�Ʈ ���� */
		SetTrigEvent(TRUE, stPktData.trig_set);
		/* ���ο� ���� ���� �� */
		memcpy(&stPktData.trig_set[u8ChNo].area_trig_pos+start, pos, sizeof(UINT32) * count);
		/* ���� �ֱ��� Ʈ���� �۽� ��ġ �� ���� */
		memcpy(&m_u32SendTrigPos[u8ChNo]+start, pos, sizeof(UINT32) * count);

		// Trigger Offset ����
		UINT32				area_trig_pos_offset[MAX_TRIG_CHANNEL] = { 0 };
		if (ENG_TEED::en_positive == enable)
		{
			for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
			{
				area_trig_pos_offset[i] = stPktData.trig_set[i].area_trig_pos_plus;
			}
		}
		else if (ENG_TEED::en_negative == enable)
		{
			for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
			{
				area_trig_pos_offset[i] = stPktData.trig_set[i].area_trig_pos_minus;
			}
		}
		// Trigger Offset ����
		
		for (int i = start; i < start+count; i++)
		{
			uiStatus = MvsEncSetIndexTriggerPosition(m_handle, u8ChNo, i, pos[i] + area_trig_pos_offset[u8ChNo]);
			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetIndexTriggerPosition API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}
		}
		m_u64SendTime = GetTickCount64();
		/* ���� ���� */
		m_syncSend.Leave();
	}

	/* ���� ���� �� �б� ��û */
	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : Trigger & Strobe Enable or Disable
 parm : enable	- [in]  TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE
*/
BOOL CMvencThread::ReqWriteTriggerStrobe(BOOL enable)
{

	UINT8 i	= 0x00;
	BOOL bSucc			= TRUE;
	UINT uiStatus		= 0;
	STG_TPQR stPktData	= {NULL};

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
#if 1
		/* Encoder Counter Reset Command */
		//stPktData.command			= (UINT32)ENG_TBPC::en_write;
		//stPktData.trig_strob_ctrl	= enable ? (UINT32)ENG_TEED::en_enable : (UINT32)ENG_TEED::en_disable;

		m_pstShMemTrig->command			= (UINT32)ENG_TBPC::en_write;
		m_pstShMemTrig->trig_strob_ctrl	= enable ? (UINT32)ENG_TSED::en_enable : (UINT32)ENG_TSED::en_disable;


		for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
		{
			m_stTrigCtrl[i].PositionDirection = (UINT16)enable;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
			//ä�κ� Trigger Controls Set
			uiStatus = MvsEncSetTriggerControls(m_handle, i, &m_stTrigCtrl[i]);//����ü ����
			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerControls API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}
		}

		//������ġ �ʱ�ȭ
		uiStatus = MvsEncClearEncoderPositionAll(m_handle);
		if (uiStatus != 0x00)
		{
			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncClearEncoderPositionAll API Error (status:%d)", uiStatus);
			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			bSucc = FALSE;
		}

		/* Ʈ���� �̺�Ʈ ���� */
		//if (enable)	SetTrigEvent(TRUE, stPktData.trig_set);
		if (enable)	SetTrigEvent(TRUE, m_pstShMemTrig->trig_set);

		/* --------------------------------------------------------------------------------- */
		/* Triger & Strobe Enable ���� Disable�� ���� ��ϵ� Trigger Position �� ��� �ʱ�ȭ */
		/* --------------------------------------------------------------------------------- */
		/* �ִ� �� ���� */
		//stPktData.ResetTrigPos(0);
		//stPktData.ResetTrigPos(1);
		//stPktData.ResetTrigPos(2);
		//stPktData.ResetTrigPos(3);
		m_pstShMemTrig->ResetTrigPos(0);
		m_pstShMemTrig->ResetTrigPos(1);
		m_pstShMemTrig->ResetTrigPos(2);
		m_pstShMemTrig->ResetTrigPos(3);

		
		//Ʈ������� ī��Ʈ �ʱ�ȭ
		uiStatus = MvsEncClearTriggerAll(m_handle);
		if (uiStatus != 0x00)
		{
			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncClearTriggerAll API Error (status:%d)", uiStatus);
			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			bSucc = FALSE;
		}
#endif
		//for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
		//{
		//	MvsEncSetTriggerOutputMode(m_handle, i, 2);
		//	//Sleep(500);
		//
		//	//bSucc = MvsEncSetTriggerPosition0(m_handle, i, 0); //StartPosition ����
		//	//Sleep(500);
		//	//bSucc = MvsEncSetTriggerPosition1(m_handle, i, 1000000); //StartPosition ����
		//	//Sleep(500);
		//
		//	/*���� Ʈ����*/
		//	bSucc = MvsEncSetTriggerGenerator(m_handle, i, 0, 500);
		//	//int cycle, high;
		//	//MvsEncGetTriggerGenerator(m_handle, i, &cycle, &high);
		//	//Sleep(500);
		//	
		//	//bSucc = MvsEncSetVirtualEncoderFrequency(m_handle, i, 10000);
		//	//Sleep(500);
		//
		//	//bSucc = MvsEncSetPositiveRun(m_handle, i);
		//	//Sleep(500);
		//	//bSucc = MvsEncSetTrgMode(m_handle, i);
		//	//Sleep(500);
		//}
		//if (enable == TRUE)
		//{
		//	MvsEncSetPositiveRun(m_handle, 15);
		//}
		//else
		//{
		//	MvsEncSetPositiveRun(m_handle, 0);
		//}
		//
		//Sleep(500);
		//MvsEncClearAll(m_handle);
		//MvsEncClearTriggerAll(m_handle);

		MvsEncSetPositiveRun(m_handle, 0);

		m_u64SendTime = GetTickCount64();
		/* ���� ���� */
		m_syncSend.Leave();
	}

	/* ���� ���� �� �б� ��û */
	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : ���� Ʈ���� ����
 parm : onoff	- [in]  1 ����Ʈ �� �ǹ̰� ���� (4 Bytes �̹Ƿ�, �� 4ä��)
						�ʴ� 10 frame ���
 retn : TRUE or FALSE
*/
BOOL CMvencThread::ReqWriteEncoderOut(UINT32 enc_out)
{
	UINT8 i				= 0x00;
	BOOL bSucc			= TRUE;
	UINT uiStatus		= 0;
	STG_TPQR stPktData	= {NULL};

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		/* ��ɾ� ���� */
		stPktData.command		= (UINT32)ENG_TBPC::en_trig_internal;
		stPktData.enc_out_val	= enc_out;
		for (; i < MAX_TRIG_CHANNEL; i++)
		{
#if 0
			stPktData.trig_set[i].area_trig_ontime	= 2500;
			stPktData.trig_set[i].area_strob_ontime	= 2500;
#else	/* ���� DI ��� Ư�� ������ (Trigger�� ������ 1 & 3 CH ������ �߻� ���Ѿ� ��) */
			if (i == 0x00 || i == 0x02)	stPktData.trig_set[i].area_trig_ontime	= 2500;
			else						stPktData.trig_set[i].area_trig_ontime	= 0;
			stPktData.trig_set[0].area_strob_ontime	= 2500;
#endif

			// ���� Ʈ���� �߻� : ���� Ȯ�� �ʿ�
			uiStatus = MvsEncSetTriggerGenerator(m_handle, i
				, stPktData.trig_set[i].area_trig_ontime
				, stPktData.trig_set[i].area_trig_ontime);

			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerGenerator API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}
		}
		m_u64SendTime = GetTickCount64();
		/* ���� ���� */
		m_syncSend.Leave();
	}

	/* ���� ���� �� �б� ��û */
	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : ���� ä�� ���� Ʈ���� 1���� �߻�
 parm : enc_out	- [in]  1 ����Ʈ �� �ǹ̰� ���� (4 Bytes �̹Ƿ�, �� 4ä��)
 retn : TRUE or FALSE
*/
BOOL CMvencThread::ReqWriteTrigOutOne(UINT32 enc_out)
{
	BOOL bSucc			= TRUE;
	UINT uiStatus		= 0;
	STG_TPQR stPktData	= {NULL};

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		/* ��ɾ� ���� */
		stPktData.command		= (UINT32)ENG_TBPC::en_trig_out_one;
		stPktData.enc_out_val	= enc_out;
		
		for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
		{
			// ���� Ʈ���� �߻� : ���� Ȯ�� �ʿ�
			uiStatus = MvsEncSetTriggerGenerator(m_handle, i
				, m_pstShMemTrig->trig_set[i].area_trig_ontime
				, m_pstShMemTrig->trig_set[i].area_trig_ontime);

			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerGenerator API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}
		}

#ifdef RUNTRIGGER
		MvsEncSetPositiveRun(m_handle, 15);
		Sleep(10);
		MvsEncSetPositiveRun(m_handle, 0);
#endif

		m_u64SendTime = GetTickCount64();
		/* ���� ���� */
		m_syncSend.Leave();
	}

	/* ���� ���� �� �б� ��û */
	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : Reset Trigger Count
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMvencThread::ReqResetTrigCount()
{
	BOOL bSucc			= TRUE;
	UINT uiStatus		= 0;
	STG_TPQR stPktData	= {NULL};

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		/* Encoder Counter Reset Command */
		stPktData.command		= (UINT32)ENG_TBPC::en_write;
		stPktData.enc_counter	= (INT32)ENG_TECM::en_clear;	/* 0x03 (Clear),  0x02 (Read) */

		uiStatus = MvsEncClearEncoderPositionAll(m_handle);
		if (uiStatus != 0x00)
		{
			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncClearEncoderPositionAll API Error (status:%d)", uiStatus);
			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			bSucc = FALSE;
		}
		
		uiStatus = MvsEncClearTriggerAll(m_handle);

		if (uiStatus != 0x00)
		{
			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerGenerator API Error (status:%d)", uiStatus);
			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			bSucc = FALSE;
		}
		m_u64SendTime = GetTickCount64();
		/* ���� ���� */
		m_syncSend.Leave();
	}

	/* ���� ���� �� �б� ��û */
	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : ������ �Էµ� Trigger ��ġ ���� Trigger Board�κ��� ���ŵ� �Է� �� ��
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		index	- [in]  Ʈ���� ���� ��ġ (0x000 ~ 0x0f)
		pos		- [in]  Ʈ���� �� (���Ϸ��� �Էµ� Ʈ���� ��ġ ��)
 retn : TRUE or FALSE
*/
BOOL CMvencThread::IsTrigPosEqual(UINT8 cam_id, UINT8 index, UINT32 pos)
{
	/* �� Align Camera�� ���� ä�� ��ȣ ��� */
	UINT8 u8ChNo	= GetTrigChNo(cam_id);
	if (!u8ChNo)	return FALSE;
	u8ChNo--;

	return (m_pstShMemTrig->trig_set[u8ChNo].area_trig_pos[index] == pos);
}

/*
 desc : ī�޶� �� ���� Ʈ���� �߻��Ͽ� Live ���� ����
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		enable	- [in]  TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE
*/
BOOL CMvencThread::ReqWirteTrigLive(UINT8 cam_id, BOOL enable)
{
	UINT8 i = 0x00;
	BOOL bSucc = TRUE;
	UINT uiStatus = 0;
	STG_TPQR stPktData = { NULL };

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		//for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
		//{
		//	m_stTrigCtrl[i].PositionDirection = (UINT16)enable;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
		//	//ä�κ� Trigger Controls Set
		//	uiStatus = MvsEncSetTriggerControls(m_handle, i, &m_stTrigCtrl[i]);//����ü ����
		//	if (uiStatus != 0x00)
		//	{
		//		TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
		//		swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerControls API Error (status:%d)", uiStatus);
		//		LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
		//		bSucc = FALSE;
		//	}
		//}
		//
		//for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
		//{
		//	MvsEncSetTriggerOutputMode(m_handle, i, 2);
		//	//Sleep(500);
		//
		//	//bSucc = MvsEncSetTriggerPosition0(m_handle, i, 0); //StartPosition ����
		//	//Sleep(500);
		//	//bSucc = MvsEncSetTriggerPosition1(m_handle, i, 1000000); //StartPosition ����
		//	//Sleep(500);
		//
		//	/*���� Ʈ����*/
		//	bSucc = MvsEncSetTriggerGenerator(m_handle, i, 0, 50);
		//	//int cycle, high;
		//	//MvsEncGetTriggerGenerator(m_handle, i, &cycle, &high);
		//	//Sleep(500);
		//
		//	//bSucc = MvsEncSetVirtualEncoderFrequency(m_handle, i, 10000);
		//	//Sleep(500);
		//
		//	//bSucc = MvsEncSetTrgMode(m_handle, i);
		//	//Sleep(500);
		//}
		SetLiveMode();

		if (enable == TRUE)
		{
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
			if (cam_id == 0x01)
			{
				//cam1 �� ����(0110) Ʈ���� ����
				MvsEncSetPositiveRun(m_handle, 6);
		}
			if (cam_id == 0x02)
			{
				//cam2 �� ����(1001) Ʈ���� ����
				MvsEncSetPositiveRun(m_handle, 9);
			}
			//MvsEncSetPositiveRun(m_handle, 15);
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
			if (cam_id == 0x01)
			{
				//cam1 �� ����(1001) Ʈ���� ����
				MvsEncSetPositiveRun(m_handle, 9);
			}
			if (cam_id == 0x02)
			{
				//cam2 �� ����(0101) Ʈ���� ���� => (1010) => 10
				MvsEncSetPositiveRun(m_handle, 10);
			}
			if (cam_id == 0x03)
			{
				//cam2 �� ����(0011) Ʈ���� ���� (1100)
				MvsEncSetPositiveRun(m_handle, 12);
			}
#endif

		}
		else
		{
			//���� ����(0000)
			MvsEncSetPositiveRun(m_handle, 0);
		}


		Sleep(500);
		MvsEncClearAll(m_handle);
		MvsEncClearTriggerAll(m_handle);



		m_u64SendTime = GetTickCount64();
		/* ���� ���� */
		m_syncSend.Leave();
	}


	return bSucc;
}



/*
 desc : Ʈ���� ���� Live Mode�� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMvencThread::SetLiveMode()
{
	UINT8 i = 0x00;
	BOOL bSucc = TRUE;
	UINT uiStatus = 0;

	/* ���� ���� */
	if (m_syncSend.Enter())
	{

		// �ʱⰪ ����
		for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
		{

			m_stTrigCtrl[i].EncoderType = 2;	//0:DIFF, 1:TTL, 2:Virtual Encoder
			m_stTrigCtrl[i].EncoderCh = i;		//Encoder CH (0~3)
			m_stTrigCtrl[i].Multi = 4;			//Encoder Multi(1:x1, 2:x2, 4:x4)
			m_stTrigCtrl[i].CondFactor = 7;		//0:DI0 ~ 6:DI6, 7:High, 8:Low
			m_stTrigCtrl[i].TriggerOutputOperator = 0;	//0:AND, 1:OR, 2:XOR, 3:NAND
			m_stTrigCtrl[i].EncoderResetFactor = 0;	//0:DISABLE, 1:DI0 ~ 7:DI6, 8:CW, 9:CCW
			m_stTrigCtrl[i].PositionDirection = 1;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
			m_stTrigCtrl[i].Temp = 0;
			m_stTrigCtrl[i].Correction = 0;		//0:DISABLE, 1:ENABLE
			m_stTrigCtrl[i].TriggerBase = 0;	//0:COUNT, 1:POSITION
			uiStatus = MvsEncSetTriggerControls(m_handle, i, &m_stTrigCtrl[i]);//����ü ����
			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerControls API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			}

			MvsEncSetTriggerPosition0(m_handle, i, 0);
			MvsEncSetTriggerPosition1(m_handle, i, 100000000);
			MvsEncSetTriggerGenerator(m_handle, i, 0, 1000);
			MvsEncSetTriggerDelay(m_handle, i, 0);

		}

		//MvsEncSetPositiveRun(m_handle, 15);
		MvsEncClearAll(m_handle);

		m_u64SendTime = GetTickCount64();
		/* ���� ���� */
		m_syncSend.Leave();
	}
	return bSucc;
}


/*
 desc : Ʈ���� ���� Position Trigger ���� ����
 parm : enable	- [ENG_TEED]  Ʈ���� Positive or Negative
 retn : TRUE or FALSE
*/
BOOL CMvencThread::SetPositionTrigMode(ENG_TEED enable)
{
	UINT8 i = 0x00;
	BOOL bSucc = TRUE;
	UINT uiStatus = 0;

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		/*�ʱ�ȭ*/
		MvsEncSetPositiveRun(m_handle, 0);

		// �ʱⰪ ����
		for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
		{

			m_stTrigCtrl[i].EncoderType = 0;	//0:DIFF, 1:TTL, 2:Virtual Encoder
			m_stTrigCtrl[i].EncoderCh = 0;		//Encoder CH (0~3)
			m_stTrigCtrl[i].Multi = 4;			//Encoder Multi(1:x1, 2:x2, 4:x4)
			m_stTrigCtrl[i].CondFactor = 7;		//0:DI0 ~ 6:DI6, 7:High, 8:Low
			m_stTrigCtrl[i].TriggerOutputOperator = 0;	//0:AND, 1:OR, 2:XOR, 3:NAND
			m_stTrigCtrl[i].EncoderResetFactor = 0;	//0:DISABLE, 1:DI0 ~ 7:DI6, 8:CW, 9:CCW

			m_stTrigCtrl[i].Temp = 0;
			m_stTrigCtrl[i].Correction = 0;		//0:DISABLE, 1:ENABLE
			m_stTrigCtrl[i].TriggerBase = 0;	//0:COUNT, 1:POSITION

			if (ENG_TEED::en_positive == enable)
			{
				m_stTrigCtrl[i].PositionDirection = 1;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
			}
			else if (ENG_TEED::en_negative == enable)
			{
				m_stTrigCtrl[i].PositionDirection = 2;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
			}
			uiStatus = MvsEncSetTriggerControls(m_handle, i, &m_stTrigCtrl[i]);//����ü ����
			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerControls API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			}

			MvsEncSetTriggerGenerator(m_handle, i, MAX_TRIG_POS, MAX_TRIG_POS);

			if (ENG_TEED::en_positive == enable)
			{
				MvsEncSetTriggerPosition0(m_handle, i, 100);
				MvsEncSetTriggerPosition1(m_handle, i, 0);
			}
			else if (ENG_TEED::en_negative == enable)
			{
				MvsEncSetTriggerPosition0(m_handle, i, 0);
				MvsEncSetTriggerPosition1(m_handle, i, -100);

			}

			MvsEncSetTriggerDelay(m_handle, 0, 10);
			MvsEncSetTriggerDelay(m_handle, 1, 10);
			MvsEncSetTriggerDelay(m_handle, 2, 0);
			MvsEncSetTriggerDelay(m_handle, 3, 0);

			MvsEncSetTriggerPulseWidth(m_handle, i, 200);

		}

		//if (ENG_TEED::en_positive == enable)
		//{
		//	//MvsEncSetPositiveRun(m_handle, 15);
		//	MvsEncSetPositiveRun(m_handle, 6);
		//	MvsEncSetPositiveRun(m_handle, 9);
		//}
		//else if (ENG_TEED::en_negative == enable)
		//{
		//	//MvsEncSetNegativeRun(m_handle, 15);
		//	MvsEncSetNegativeRun(m_handle, 6);
		//	MvsEncSetNegativeRun(m_handle, 9);
		//}
		//MvsEncClearAll(m_handle);

		m_u64SendTime = GetTickCount64();
		/* ���� ���� */
		m_syncSend.Leave();
	}
	return bSucc;
}