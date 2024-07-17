
/*
 desc : ��� ������ ������ ����ó��
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkAbort.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : ������
 parm : forced	- [in]  ������ �뱤 ���� ��� ���� ��, �뱤 ������ �ʴ� ���¿���, Luria�κ���
						�뱤 ���� �Ϸ� ������ ���� �ʰ�, ��� ���� ���Ͱ� ������ �Ǿ� �ִٸ�
						������ �뱤 ���� �۾��� ���� ��Ŵ
 retn : None
*/
CWorkAbort::CWorkAbort(UINT8 forced)
	: CWorkStep()
{
	m_enWorkJobID	= ENG_BWOK::en_work_stop;
	m_u8AbortForced	= forced;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CWorkAbort::~CWorkAbort()
{
}

/*
 desc : �ʱ� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkAbort::InitWork()
{
	/* ���� ��� ���� �� �ʱ�ȭ */
	if (!CWork::InitWork())	return FALSE;

	/* ��ü �۾� �ܰ� */
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	m_u8StepTotal = 0x07;
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	m_u8StepTotal	= 0x0a;
#endif

	//m_u8StepTotal = 0x07;
	/*Philhmi�� ��� ���� ��ȣ ������*/
	//PhilInterrupsStop();
	return TRUE;
}

/*
 desc : �ֱ������� �۾� ����
 parm : None
 retn : None
*/
VOID CWorkAbort::DoWork()
{
	/* �۾� �ܰ� ���� ���� ó�� */
	switch (m_u8StepIt)
	{
		case 0x01 : m_enWorkState = StopPrinting();							break;
		/* GetExposeState ��û�� ���� ������ �� ������ ���� �ð� ��� */
		case 0x02 : m_enWorkState = SetWorkWaitTime(1000);					break;
		case 0x03 : m_enWorkState = IsWorkWaitTime();						break;
		case 0x04 : m_enWorkState = IsStopPrinting();						break;
		/* ���������� ���� ������ ���� �ð� ��� */
		case 0x05 : m_enWorkState = SetWorkWaitTime(2000);					break;
		case 0x06 : m_enWorkState = IsWorkWaitTime();						break;
		case 0x07 : m_enWorkState = SetTrigEnable(FALSE);					break;
		case 0x08 : m_enWorkState = SetMovingUnloader();					break;
		case 0x09: m_enWorkState = IsTrigEnabled(FALSE);					break;
		case 0x0a: m_enWorkState = IsMovedUnloader();						break;
	}

	
	/* ���� �۾� ���� ���� �Ǵ� */
	CWork::SetWorkNext();
	/* ��ð� ���� ���� ��ġ�� �ݺ� �����Ѵٸ� ���� ó�� */
	CWork::CheckWorkTimeout();
}

/*
 desc : ���� ���� �뱤 ���̸�, ������ �뱤 ���� ��Ŵ
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkAbort::StopPrinting()
{
	/* ���� �۾� Step Name ���� */
	SetStepName(L"Stop.Printing");

	/* �뱤 ������ �˸��� ���� (���� 2���� ��ư ����ġ) ���� */
	// by sysandj : MCQ��ü �߰� �ʿ� : uvEng_MCQ_StartLampEnable(0x00);

	if (!uvCmn_Luria_IsExposeIdle())
	{
		if (!uvEng_Luria_ReqSetPrintAbort())
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetPrintAbort)");
			return ENG_JWNS::en_error;
		}
		if (!uvEng_GetConfig()->luria_svc.use_announcement)
		{
			/* ��� �۽� �ð� ���� */
			SetSendCmdTime();

			/* Printing Status ���� ��û */
			if (0x00 == uvEng_Luria_ReqGetExposureState())
			{
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetExposureState)");
				return ENG_JWNS::en_error;
			}
		}
	}

	/* ��ð� ����ϱ⸦ �����ϱ� ���� */
	m_u64TickAbortWait	= GetTickCount64();

	return ENG_JWNS::en_next;
}

/*
 desc : ���� �뱤 ������ ���� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkAbort::IsStopPrinting()
{
	/* ���� �۾� Step Name ���� */
	SetStepName(L"Is.Stop.Printing");

	/* ���� Printing ���̶��, ������ ���� */
	if (0x00 == m_u8AbortForced)
	{
		/* ���� Luria�� �뱤 ��� ��� �������� ���� */
		if (uvCmn_Luria_IsExposeIdle())
		{
			return ENG_JWNS::en_next;
		}

		if (!uvEng_GetConfig()->luria_svc.use_announcement)
		{
			/* �ٽ� ��� ������ �Ǵ� �ð����� �Ǵ� */
			if (IsSendCmdTime(250))
			{
				/* Printing Status ���� ��û */
				if (0x00 == uvEng_Luria_ReqGetExposureState())
				{
					LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetExposureState)");
					return ENG_JWNS::en_error;
				}
			}
		}
		/* �ٽ� ��� ������ �Ǵ� �ð����� �Ǵ� */
		if (IsSendCmdTime(250))
		{
			/* �۽� �ð� ���� */
			SetSendCmdTime();
			/* ������ Printing ���� */
			if (!uvEng_Luria_ReqSetPrintAbort())	return ENG_JWNS::en_next;
		}
		/* 5 �� ���� ������ ������, ������ ���� �۾����� Skip ó�� */
		if (GetTickCount64() > m_u64TickAbortWait + 5000)
		{
			/* ��� ���Ͱ� ���� �������� ���� Ȯ�� */
			if (uvCmn_MC2_IsMotorDriveStopAll())
			{
				m_u8StepIt		= 0x00;
				m_u8AbortForced	= 0x01;
				return ENG_JWNS::en_next;
			}
		}
	}
	/* ��� ���� Motion�� ������ ���¶��, �ٷ� ���� �۾����� Skip ó�� */
	else
	{
		/* ��� ���Ͱ� ���� �������� ���� Ȯ�� */
		if (uvCmn_MC2_IsMotorDriveStopAll())	
			return ENG_JWNS::en_next;
	}

	return ENG_JWNS::en_wait;
}

/*
 desc : Philhmi�� ��� ���� ��ȣ ������
 parm : None
 retn : None
*/
VOID CWorkAbort::PhilInterrupsStop()
{
	STG_PP_P2C_INTERRUPT_STOP		stP2CInterruptStop;
	STG_PP_P2C_INTERRUPT_STOP_ACK	stInterruptStopAck;

	stP2CInterruptStop.Reset();
	stInterruptStopAck.Reset();

	uvEng_Philhmi_Send_P2C_INTERRUPT_STOP(stP2CInterruptStop, stInterruptStopAck);



	if (stInterruptStopAck.usErrorCode)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Philhmi Failure Equipment stop signal");
	}
}