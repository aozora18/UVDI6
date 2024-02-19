
/*
 desc : Serial (RS-232C) �Լ�
*/

#include "pch.h"
#include "SerialComm.h"
/* ��/���� ���� ��ü */
#include "RingBuff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : port	- [in]  ��� ��Ʈ ��ȣ (1 ~ 255)
		baud	- [in]  ��� ��Ʈ �ӵ� (CBR_100 ~ CBR_CBR_256000 �� �Է�)
		buff	- [in]  ��� ��Ʈ�� ��/���� ���� ũ��
		shake	- [in]  SerialComm - HandShae ��� (0x00 ~ 0x03)
 retn : None
*/
CSerialComm::CSerialComm(UINT8 port, UINT32 baud, UINT32 buff, ENG_SHOT shake)
{
	m_bIsOpened			= FALSE;
	m_hComm				= INVALID_HANDLE_VALUE;
	m_enCommStatus		= ENG_SCSC::en_scsc_closed;

	m_u64OpenTryTime	= 0;	/* �ʱ� ���� �õ��� ���� �ٷ� �����ϵ��� �ݵ�� 0 �� ���� */
	m_pPortBuff			= NULL;	/* �ӽ� ���� ���� �ʱ�ȭ (SetupComm �Լ����� �Ҵ�) */

	/* I/O Pending ���� �ƴ� */
	m_bIsWritePending	= FALSE;
	m_bIsReadPending	= FALSE;

	m_enHandShake		= shake;	/* ��� �帧 ���� ��� (����� ���� ��Ŵ) */
	m_u32BaudRate		= baud;		/* ��� ��Ʈ �ӵ� */
	m_u32PortBuff		= buff;		/* ��� ��/���� ��Ʈ ���� ũ�� */

	/* ��Ʈ�� 10 �̻��̸� �Ʒ��� ���� ���� */
	wmemset(m_tzPort, 0x00, 64);
	if (port < 10)	swprintf_s(m_tzPort, 64, L"COM%d", port);
	else			swprintf_s(m_tzPort, 64, L"\\\\.\\COM%d", port);

	/* Error Message�� ������ ���� ���� */
	m_pErrMsg			= (PTCHAR)::Alloc(1024 * sizeof(TCHAR));
	/* ��Ŷ ������ ��/���� ���� ���� */
	m_pReadBuff			= new CRingBuff;
	ASSERT(m_pReadBuff);
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CSerialComm::~CSerialComm()
{
	m_bIsOpened		= FALSE;
	m_enCommStatus	= ENG_SCSC::en_scsc_closed;

	/* ���� ��� ���� ���� */
	Close();

	if (m_pErrMsg)		::Free(m_pErrMsg);		/* ���� �޽��� ���� ���� */
	if (m_pReadBuff)	delete m_pReadBuff;		/* ���� ���� �޸� ���� */
	if (m_pPortBuff)	::Free(m_pPortBuff);	/* ��� ��Ʈ�� �ӽ� ���� ���� �޸� ���� */
}

/*
 desc : ���� ���� ����
 parm : msg		- [in]  ���� ���ڿ� ������ ����� ����
		flag	- [in]  ���� �ڵ尡 �ִ� ���-0x01 / ���� �ڵ� ���� �׳� ���ڿ����� ���-0x00
		msg_box	- [in]  �޽��� �ڽ� ��� ���� (����� ����� ��츸 ����)
 retn : ����
 ���� : �ۼ���-������ / ����-2007.09.06 / ����-2007.09.06
*/
void CSerialComm::PutErrMsg(PTCHAR msg, UINT8 flag, BOOL msg_box)
{
	wmemset(m_pErrMsg, 0x00, 1024);
	if (flag == 0x01)	swprintf_s(m_pErrMsg, 1024, L"[%s] %s [ERR_CODE : %d]", m_tzPort, msg, GetLastError());
	else				swprintf_s(m_pErrMsg, 1024, L"[%s] %s", msg, m_tzPort);
#ifdef _DEBUG
	if (msg_box)	AfxMessageBox(m_pErrMsg, MB_ICONSTOP|MB_TOPMOST);
	TRACE(L"Serial Error : %s\n", m_pErrMsg);
#endif // _DEBUG
	LOG_ERROR(ENG_EDIC::en_engine, m_pErrMsg);
}

/*
 desc : ��� ��Ʈ ���� (�ڵ� �ʱ�ȭ �� Port opened)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CSerialComm::OpenDefault()
{
	m_bIsOpened		= FALSE;			/* ��Ʈ�� �ʱ�ȭ ���� �ʾҴٰ� ���� */
	m_u64OpenTryTime= GetTickCount64();	/* �ø��� ��� ��Ʈ�� ���� ������ �ð� ���� */

	/* Read / Write Event ���� */
	if (!CreateEvent())	return FALSE;
	/* ������ ������ ������ ���� ���з� ó�� */
	if (m_hComm != INVALID_HANDLE_VALUE)
	{
		PutErrMsg(L"The port number is already opened", TRUE);
		return FALSE;
	}
	/* ��Ʈ ���� ������ ��Ʈ ���� �õ� */
	m_hComm = CreateFile(m_tzPort,									/* Port Name */
						 GENERIC_READ | GENERIC_WRITE,				/* Desired Access */
						 0,											/* Shared Mode */
						 NULL,										/* Security */
						 OPEN_EXISTING,								/* Creation Disposition */
						 FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
						 NULL);										/* Non Overlapped */
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		PutErrMsg(L"Failed to open the com port", TRUE);
		return FALSE;
	}
	/* ��� ��Ʈ �ʱ�ȭ */
	if (!ConfigPort())	return FALSE;

	/* ��Ʈ�� ���ȴٰ� ���� */
	m_bIsOpened			= TRUE;
	/* I/O Pending ���� �ƴ϶�� ���� */
	m_bIsWritePending	= FALSE;
	m_bIsReadPending	= FALSE;
	m_enCommStatus		= ENG_SCSC::en_scsc_opened;

	return TRUE;
}

/*
 desc : ��� ��Ʈ ���� (�ڵ� �ʱ�ȭ �� Port opened)
 parm : block_mode	- [in]  �ø��� ��Ʈ �б� Block Mode ���� (0x00, 0x01)
		is_binary	- [in]  �ۼ��� ������ ���� ���̳ʸ� ������� �ƴ��� (TRUE : binary, FALSE : text)
 retn : TRUE or FALSE
*/
BOOL CSerialComm::OpenExt(ENG_CRTB block_mode, BOOL is_binary)
{
	m_bIsOpened			= FALSE;			/* ��Ʈ�� �ʱ�ȭ ���� �ʾҴٰ� ���� */
	m_u64OpenTryTime	= GetTickCount64();	/* �ø��� ��� ��Ʈ�� ���� ������ �ð� ���� */

	/* Read / Write Event ���� */
	if (!CreateEvent())	return FALSE;
	/* ������ ������ ������ ���� ���з� ó�� */
	if (m_hComm != INVALID_HANDLE_VALUE)
	{
		PutErrMsg(L"The port number is already opened", TRUE);
		return FALSE;
	}
	/* ��Ʈ ���� ������ ��Ʈ ���� �õ� */
	m_hComm = CreateFile(m_tzPort,									/* Port Name */
						 GENERIC_READ | GENERIC_WRITE,				/* Desired Access */
						 0,											/* Shared Mode */
						 NULL,										/* Security */
						 OPEN_EXISTING,								/* Creation Disposition */
						 FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
						 NULL);										/* Non Overlapped */
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		PutErrMsg(L"Failed to open the com port", TRUE);
		return FALSE;
	}

	/* ��� ��Ʈ �۽�/���� ���� ũ�� ���� */
	if (!SetupComm(m_u32PortBuff))	return FALSE;
	/* ��� ��Ʈ ���� ���� �� ���� �ʱ�ȭ */
	if (!PurgeClear())	return FALSE;
	/* ��ż��� �󿡼� �� ����Ʈ�� ���۵ǰ� �� �� �� �� ����Ʈ�� ���۵Ǳ������ �ð� */
	if (!SetTimeout(block_mode))	return FALSE;	/* �ּ� 50 msec �б� �ð� ���� ����!!! */
	/* DCB ���� */
	if (!SetCommState(8, ONESTOPBIT, NOPARITY, is_binary))	return FALSE;
	/* ��� �̺�Ʈ ���� ��� ���� */
	if (!SetCommMask(EV_RXCHAR))	return FALSE;


	/* ��Ʈ�� ���ȴٰ� ���� */
	m_bIsOpened			= TRUE;
	/* I/O Pending ���� �ƴ϶�� ���� */
	m_bIsWritePending	= FALSE;
	m_bIsReadPending	= FALSE;
	m_enCommStatus		= ENG_SCSC::en_scsc_opened;

	/* ��Ʈ Open �����ٰ� �α� ���� ���� */
	PutErrMsg(L"Successfully opened the communication port");

	return TRUE;
}

/*
 desc : �ø��� ��Ʈ �ݱ�
 parm : ����
 retn : TRUE or FALSE
*/
BOOL CSerialComm::Close()
{
	DWORD dwFlag = PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR;

	/* ������ ������ ���� ���� ��� */
	if (m_hComm == INVALID_HANDLE_VALUE)	return FALSE;

	/* ��� Event Mask�� ���ش�. */
	SetCommMask(0);
	/* ���� �ʾƵ� �ǰ�, ���� �ص� �ǰ�? */
	::EscapeCommFunction(m_hComm, CLRDTR);
	::EscapeCommFunction(m_hComm, CLRRTS) ;
	/* ��� Queue���� �ʱ�ȭ */
	::PurgeComm(m_hComm, dwFlag);
	/* Event ��ü ���� */
	if (m_ovRead.hEvent)	CloseHandle(m_ovRead.hEvent);
	if (m_ovWrite.hEvent)	CloseHandle(m_ovWrite.hEvent);
	m_ovRead.hEvent		= NULL;
	m_ovWrite.hEvent	= NULL;
	/* ��� ����̽� �ڵ��� ���� */
	::CloseHandle(m_hComm);
	m_hComm = INVALID_HANDLE_VALUE;

	/* ��Ʈ�� �����ٰ� ���� */
	m_bIsOpened			= FALSE;
	/* I/O Pending ���� �ƴ϶�� ���� */
	m_bIsWritePending	= FALSE;
	m_bIsReadPending	= FALSE;
	m_enCommStatus		= ENG_SCSC::en_scsc_closed;
	
	PutErrMsg(L"The serial communication port is closed");

	return TRUE;
}

/*
 desc : ClearCommError ȣ�� �� ����� ������ ��� ������ ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CSerialComm::SetClearCommError()
{
	DWORD dwError	= 0;
	COMSTAT stStat	= {NULL};

	/* ���� ���� �� ���� ���� �ʱ�ȭ */
	m_u32CommError	= 0;
	if (m_hComm && !::ClearCommError(m_hComm, &dwError, &stStat))
	{
		PutErrMsg(L"Failed to invoke the <ClearCommError> function", 0x01);
		return FALSE;
	}

	/* ���� ���� ���� */
	m_u32CommError	= dwError;
	switch (dwError)
	{
	case CE_BREAK		:	PutErrMsg(L"The hardware detected a break condition",	0x01);	break;	// �ϵ���� �ߴ� ������ ����
	case CE_FRAME		:	PutErrMsg(L"The hardware detected a framing error",		0x01);	break;	// �ϵ���� ������ ���� ����
	case CE_OVERRUN		:	PutErrMsg(L"A character-buffer overrun has occurred",	0x01);	break;	// ���� ���� ������ �߻���, ���� ���� �ս�
	case CE_RXOVER		:	PutErrMsg(L"An input buffer overflow has occurred",		0x01);	break;	// �Է� ���� �����÷ο� �߻��� (�Է� ������ ������ ������, EOF ���ڸ� ���� �Ŀ� ���ڰ� ������ �߻�)
	case CE_RXPARITY	:	PutErrMsg(L"The hardware detected a parity error",		0x01);	break;	// �ϵ���� �и�Ƽ ���� ����
	}

	return TRUE;
}

/*
 desc : Read / Write Event ���� �� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CSerialComm::CreateEvent()
{
	/* Overlapped Event ��ü �ʱ�ȭ */
	memset(&m_ovRead,	0x00, sizeof(OVERLAPPED));
	memset(&m_ovWrite,	0x00, sizeof(OVERLAPPED));

	/* ------------------------------------------------------------------------------------------ */
	/*                               Read & Write �̺�Ʈ ���� �� �ʱ�ȭ                           */
	/* ------------------------------------------------------------------------------------------ */
	/* lpEventAttributes                                                                          */
    /*                                                                                            */
	/*	SECURITY_ATTRIBUTES ����ü�� ����� ������ �ּҸ� ����Ѵ�. �� �Ű������� NULL�� ����ϸ� */ 
	/*	������ �̺�Ʈ �ڵ��� �ڽ� ���μ����� ��ӵ��� �ʴ´�. SECURITY_ATTRIBUTES ����ü�� �׸��� */
	/*	lpSecurityDescriptor���� ���ο� �̺�Ʈ�� ���� �����(security descriptor)�� ���Ͽ� ����� */
	/*	���� �Ǿ��ִ�. ����, �� �Ű������� NULL�� ����Ͽ� ���� ����ڿ� ���� ��ø� ���� ���� �� */
	/*	�쿡��, ���� ������ �̺�Ʈ ��ü�� �Ϲ����� ���� ����ڰ� ��õȰɷ� �����Ѵ�.			  */
	/*	�Ϲ����� ���� ������� ACL(Access Control List)�� ���� �α��εǾ� �ִ� ������� �α��� �� */
	/*	���� �ִ� ACL�� �̿��ϰų� �̺�Ʈ ��ü�� ������ ���μ����� ACL�� ������ ����Ѵ�.		  */
	/*                                                                                            */
	/* bManualReset                                                                               */
	/*                                                                                            */
    /*  �� �Ű������� TRUE �� ����ϸ� �߻��� �̺�Ʈ ������ ��� �����Ǵ� �̺�Ʈ ��ü�� �����Ѵ�. */ 
    /*  ��, �̺�Ʈ ��ü�� SetEvent �Լ��� �̿��Ͽ� �̺�Ʈ�� �����ϸ� ResetEvent �Լ��� ȣ���Ͽ� ��*/
	/*  ���ϱ� ������ ��� �̺�Ʈ�� �߻��� ���·� �����ȴ�.                                       */
    /*  �ݴ�� �� �Ű������� FALSE �� ����ϸ� �ڵ����� �̺�Ʈ�� �����Ǵ� �̺�Ʈ ��ü�� �����Ѵ�. */
    /*  ��, �̺�Ʈ�� ��ٸ��� �����尡 �̺�Ʈ�� �߻��� ��Ȳ�� üũ�ϰ� �Ǹ� �ڵ����� �̺�Ʈ ������*/
	/*	�����ȴ�. ���� ������ ResetEvent �Լ��� ȣ���� �ʿ䰡 ����.                             */
	/*                                                                                            */
	/* bInitialState
	/*                                                                                            */
    /*  �� �Ű������� TRUE��� �̺�Ʈ ��ü�� �̺�Ʈ�� �߻��� ���·� �����Ǹ� FALSE�̸� �̺�Ʈ�� ��*/
	/*  ������ ���� ���·� �����ȴ�.                                                              */
	/*                                                                                            */
	/* lpName                                                                                     */
	/*                                                                                            */
    /*  �� �Ű��������� �̺�Ʈ ��ü�� ����� �̸��� ����Ѵ�. ���⿡ ���Ǵ� �̸��� ���̴� MAX_- */
	/*	PATH������ ���ѵǸ� ��ҹ��ڸ� ����(Tips, tips, tipS�� ���� �ٸ��ɷ� ����)�Ѵ�. ����, ���*/
	/*	�� �̸��� �̹� ������ �ٸ� �̺�Ʈ ��ü�� ���ǰ� �ִٸ� �̹� ������ ��ü�� �̺�Ʈ �߻��� */
	/*	���� �����ϰ� �ȴ�. ���� ���� ������� �̺�Ʈ ��ü�� ������ �켱�õǱ� ������ ���� ��� */
	/*	�� bManualReset, bInitialState�� ����� ������ ���õȴ�. �׸��� lpEventAttributes�� NULL��*/
	/*	�ƴ� ���������� ��õ� ���, ���� ������ �̺�Ʈ ��ü�� ���� ����ڿ� ���ؼ� ���Ǿ�������*/
	/*	�� �����Ǹ� ��밡���ϴٰ� �ǴܵǴ��� �ش� ����ü�� �����׸��� ���ȱ���� �׸� ��õ� */
	/*	������ ���õȴ�. ���� ������ �̺�Ʈ ��ü�� �����Ϸ��� �̺�Ʈ �̸��� ����Ҷ� ���� �Ѵ�. */
	/*                                                                                            */
    /*  ����, �� �Ű������� ����� �̸��� �̹� ������ Semaphore, Mutex, Waitable timer, Job �Ǵ�  */
	/*	File-mapping ��ü�� ���ǰ� �ִٸ� �� �Լ��� �����Ұ��̰� ��Ȯ�� ����üũ�� ���� GetLast-*/
	/*	Error �Լ��� ȣ���ϸ� ERROR_INVALID_HANDLE ���� ��ȯ�� ���̴�. ��, �ϳ��� ��ü�� �ٸ� ����*/
	/*	�� ��ü�� �̸��� �����Ҽ� ���� ������ ������ ��ü������ �������ΰ� �����Ǿ�����.          */
	/*                                                                                            */
    /*  �� �Ű������� NULL�� ����ϸ� �̸����� �̺�Ʈ ��ü�� �����Ѵ�.                            */
	/*                                                                                            */        
    /*  �߰������� ��ü�� �̸��� ����Ҷ�, �ý��� ���������� ��ü�� �̸��� �����ϰ� �ʹٸ� ����� */
	/*	���� �ϴ� �̸��տ� "Global\"�̶�� ���ξ ����ϰ� ���� ���ǰ��������� ����ϰ� ��������*/
	/*	�� "Local\"�̶�� ���ξ ����Ҽ� �ִ�."                                                */
	/*                                                                                            */ 
    /*  "Global" �Ǵ�  "Local" �̶�� ���ξ ����Ҽ� �ִ� Ŀ�� ��ü�� ���ؼ� ���� �ڼ��� ������*/
	/*	���ϽŴٸ� �Ʒ� ������ �����Ͻñ� �ٶ��ϴ�.                                               */
	/* ------------------------------------------------------------------------------------------ */
	m_ovRead.hEvent	= ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_ovWrite.hEvent= ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!m_ovRead.hEvent || !m_ovWrite.hEvent)
	{
		if (m_ovRead.hEvent)	CloseHandle(m_ovRead.hEvent);
		if (m_ovWrite.hEvent)	CloseHandle(m_ovWrite.hEvent);
		m_ovRead.hEvent		= NULL;
		m_ovWrite.hEvent	= NULL;

		return FALSE;
	}


	return TRUE;
}

/*
 desc : ��� ��Ʈ ȯ�� ����
 parm : None
 retn : TRUE-���� ���� / FALSE - ���� ����
*/
BOOL CSerialComm::ConfigPort()
{
	/* ��� ��Ʈ �۽�/���� ���� ũ�� ���� */
	if (!SetupComm(m_u32PortBuff))	return FALSE;
	/* ��� ��Ʈ ���� ���� �� ���� �ʱ�ȭ */
	if (!PurgeClear())	return FALSE;
	/* ��ż��� �󿡼� �� ����Ʈ�� ���۵ǰ� �� �� �� �� ����Ʈ�� ���۵Ǳ������ �ð� */
	if (!SetTimeout(ENG_CRTB::en_crtb_read_timeout_nonblocking))	return FALSE;	/* �ּ� 50 msec �б� �ð� ���� ����!!! */
	/* DCB ���� */
	if (!SetCommState())	return FALSE;
	/* ��� �̺�Ʈ ���� ��� ���� */
	if (!SetCommMask(EV_RXCHAR))	return FALSE;

	return TRUE;
}

/*
 desc : ��� ��Ʈ�� ���� �� ���۸� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CSerialComm::PurgeClear()
{
	DWORD dwError= 0;
	DWORD dwFlag = PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR;

	/* ��� ��Ʈ�� ��� ������ ���� ��Ŵ */
	if (m_hComm && !::ClearCommError(m_hComm, &dwError, NULL))
	{
		PutErrMsg(L"Failed to invoke the <ClearCommError> function", 0x01);
	}
	/* ��� TX/RX ������ �����ϰ�, ���� ������ ������ ���� */
	if (m_hComm && !::PurgeComm(m_hComm, dwFlag))
	{
		PutErrMsg(L"Failed to invoke the <PurgeComm> function", 0x01);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : ��� ��Ʈ �۽�/���� ���� ũ�� ����
 parm : port_buff	- ��� ��Ʈ �� ��/���� ���� ũ�� (MAX : 65534)
 retn : TRUE or FALSE
*/
BOOL CSerialComm::SetupComm(UINT32 port_buff)
{
	/* ��� ��Ʈ �۽�/���� ���� ũ�� ��ȿ�� �˻� */
	if (port_buff >= 65535)
	{
		PutErrMsg(L"The Send/Recv buffer of serial port has been exceeded");
		return FALSE;
	}
	/* ��/���� ���� ũ�� ���� */
	if (m_hComm && !::SetupComm(m_hComm, port_buff, port_buff))
	{
		PutErrMsg(L"Failed to invoke the <SetupComm> function", 0x01);
		return FALSE;
	}

	/* ������ ��/���� ���� ũ�� ���� */
	m_u32PortBuff	= port_buff;
	/* ��� ��Ʈ�κ��� ���ŵǴ� �����͸� ������ �ӽ� ���� (��� ��Ʈ ���� ũ�⸸ŭ �Ҵ�) */
	if (m_pPortBuff)	Free(m_pPortBuff);
	m_pPortBuff	= (PUINT8)::Alloc(m_u32PortBuff+1);
	ASSERT(m_pPortBuff);
	memset(m_pPortBuff, 0x00, m_u32PortBuff+1);

	/* ���� ���� ���� �޸� ���� �� �ٽ� �Ҵ� */
	m_pReadBuff->Destroy();
	if (!m_pReadBuff->Create(m_u32PortBuff))
	{
		PutErrMsg(L"Failed to allocate the read buffer", 0x01);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : ��� ��Ʈ�κ��� �����͸� �а�/���µ� �ִ� ��� �ð� �� ���� (��� ������ msec)
 parm : rit	- [in]  Read Interval Timeout
					byte�� byte ���̿� ���� �ð��� �����ϴ� �����μ� ������ �ð� �ȿ� ���� byte��
					�������� ������ Ÿ�� �ƿ��� �߻��Ѵ�.
					��� MAXDWORD�� ���� �����Ѵ�. ���� �����ϰ� ���� ������ 0�� �Է��Ѵ�.
		rtm	- [in]  Read Total Timeout Multiplier
					�� ĳ���͸� �����ϴµ� �ɸ��� �ð��̴�. ���Ź��۸� 4KB�� ��Ҵٰ� �ϸ� �� ����
					�ð��� (ReadTotalTimeoutMultiplier) x (���Ź��ۻ����� [4096]) �� �ǰڴ�.
		rtc	- [in]  Read Total Timeout Constant
					�� ���� �ð� �̿��� �׿� �ð����μ� �ش� �ð���ŭ �� ��ٷȴµ��� �����Ͱ�
					���ſϷ���� ������ Ÿ�� �ƿ��� �߻��Ѵ�. ����, �б⵿�ۿ��� ������ ���Ź��۸�
					ä��� ���� �� ���� �ð��� (ReadTotalTimeoutMultiplier) x (���Ź��ۻ�����) +
					ReadTotalTimeoutConstant �� �� ���̴�.
		wtm	- [in]  Write Total Timeout Multiplier
					���� �۾��� ���� �� Ÿ�Ӿƿ� �ð��� ����ϱ� ���� ���ϱ� ��� ���� (����: �и���)
					������ ���� �۾��� �� ����Ʈ ����ŭ�� �� ���� ���� ��
		wtc	- [in]  Write Total Timeout Constant
					���� �۾��� ���� ��Ż Ÿ�� �ƿ� �ð��� ����ϱ� ���� �и��� ������ ���� ����
					���� �۾��� �� ����Ʈ ���� WriteTotalTimeoutMultiplier �ɹ��� ���� ���� ���� ��
					WriteTotalTimeoutMultiplier�� WriteTotalTimeoutConstant �ɹ��� ���� 0���� ����
					�ϸ� WriteFile�� Ÿ�� �ƿ��� �������� ����
 retn : TRUE or FALSE
*/
BOOL CSerialComm::SetTimeout(ENG_CRTB mode)
{
	/* ������ ������ ���� ���� ��� */
	if (m_hComm == INVALID_HANDLE_VALUE)	return FALSE;

	/* ��� ��Ʈ�� ���� �б�/�����ϴµ� �ɸ��� ��� �ð� ���� ���� */
	COMMTIMEOUTS stTimeout;
	if (!::GetCommTimeouts(m_hComm, &stTimeout))
	{
		PutErrMsg(L"Failed to invoke the <GetCommTimeouts> function");
		return FALSE;
	}

	/* ---------------------------------------------------------------------------------------- */
	/*                                    Ÿ�̸��� �ð� ����                                    */
	/* ---------------------------------------------------------------------------------------- */
	/* ��ü������ ��� Ÿ�̸��� �ð��� �����ϸ� ������?										*/
	/* COMMTIMEOUTS ����ü�� ������ ���� �� �� �ִ� ��ó�� �����ϴ� ����� 2������ �ִ�.		*/
	/* �ϳ��� ���͹� �ð��̰� �ٸ� �ϳ��� ��Ż �ð��̴�.										*/
	/* ���͹� �ð��� ������ ������ ���� Ÿ�̸� ���ø� �ϱ� ���� ���̱� ������ 				    */
	/* ������� 9600bps�� ������ ��� 1�ɸ��ʹ� 1ms�����̱� ������ ������ ����				    */
	/* 10ms�̸� ����ϴ�. (�� ��밡 �����ؼ� �����͸� �۽��ϴ� ����̴�.)					    */
	/* ���� �ð��� ������ �������� ���ȴ�. �� �ΰ��� ���ýð��� ������ Ÿ�Ӿƿ��� �߻��Ѵ�. */
	/* Ÿ�Ӿƿ��� ��� ���� �����̳� �۽� ������ ���߿� �ߴܵǰ� ������ ���� 					*/
	/* Ÿ�Ӿƿ��� �߻��� ���������� �����͸� �д´�.											*/
	/*																							*/
	/* ���� 																					*/
	/* 	ReadIntervalTimeout�� ������ ������ �� ������ �ִ� ��� �ð��� msec�� �����Ѵ�.			*/
	/* 	ReadTotalTimeoutMuliplier�� ReadTotalTimeoutConstant�� ���� ��Ż �ð���				    */
	/* 	������ ������ ����Ѵ�.																    */
	/* 																							*/
	/* 	���� ���͹� �ð� = ReadIntervalTimeout(msec)											*/
	/* 	���� ��Ż �ð� = 																		*/
	/* 	(ReadTotalTimeoutMuliplier * ������ Byte��) + ReadTotalTimeoutConstant(msec)			*/
	/* 																							*/
	/* �۽� 																					*/
	/* 	�۽ſ��� ������ ���� Ÿ�̸Ӵ� ����.														*/
	/* 	WriteTotalTimeoutMuliplier�� WriteTotalTimeoutConstant�� �۽� ��Ż �ð���				*/
	/* 	������ ������ ����Ѵ�.																	*/
	/* 																							*/
	/* 	�۽� ��Ż �ð� = 																		*/
	/* 	(WriteTotalTimeoutMuliplier * �۽��� Byte��) + WriteTotalTimeoutConstant(msec)			*/
	/* ---------------------------------------------------------------------------------------- */

	/* ----------------------------------------------------------------------------------------------------------------------- */
	/* ReadTotalTimeoutMultiplier�� ReadTotalTimeoutConstant �Լ� �Ѵ� 0�� ��� � �����͵� ���� �ʾҴ��� �����Ѵ�.        */
	/* WriteTotalTimeoutMultiplier �� WriteTotalTimeoutConstant ���� �Ѵ� 0 �̶�� � �����͵� �۽����� �ʾҴ��� �����Ѵ�. */
	/* ----------------------------------------------------------------------------------------------------------------------- */
	switch (mode)
	{
	case ENG_CRTB::en_crtb_read_timeout_blocking		:
		stTimeout.ReadIntervalTimeout		= 0;
		break;

	case ENG_CRTB::en_crtb_read_timeout_nonblocking	:
		stTimeout.ReadIntervalTimeout		= UINT32_MAX;
		stTimeout.ReadTotalTimeoutMultiplier= 0;
		stTimeout.ReadTotalTimeoutConstant	= 0;
		break;
	}
	if (!::SetCommTimeouts(m_hComm, &stTimeout))
	{
		PutErrMsg(L"Failed to invoke the <SetCommTimeouts> function");
		return FALSE;
	}

	/* -------------------------------------------------------------------------------------------- */
	/* COM Port�κ��� 1 ����Ʈ ������ �д� 1 ms �� ���� �ҿ� �ȴ�.                                  */
	/* -------------------------------------------------------------------------------------------- */

	/* -------------------------------------------------------------------------------------------- */
	/* Ÿ�̸��� �ð� ����																			*/
	/* -------------------------------------------------------------------------------------------- */
	/* ReadIntervalTimeout�� ReadTotalTimeoutMultilier�� MAXDWORD�� �����ϰ�						*/
	/* MAXDWORD > ReadTotalTimeoutConstant > 0���� �����ϸ�											*/
	/* ReadFile�Լ��� ���� �� �Ʒ��� ���� �ȴ�.													*/
 	/*																								*/
	/* 	- �Է� ���ۿ� ���� �����Ͱ� ������ ReadFile�� �ٷ� ������ ���ڸ� ��ȯ�Ѵ�.					*/
	/* 	- �Է� ���ۿ� ���� �����Ͱ� ������ ReadFile�� ������ ������ ��ٷȴٰ� ��ȯ�Ѵ�.				*/
	/* 	- ReadTotalTimeoutConstant�� ���� ������ �ð� �̳��� 1����Ʈ�� �������� ������				*/
	/*																								*/
	/* ReadFile�� Ÿ�Ӿƿ��� �ȴ�.																	*/
	/*																								*/
	/* -------------------------------------------------------------------------------------------- */
	/* Ÿ�̸��� �ð� ����																			*/
	/* -------------------------------------------------------------------------------------------- */
	/* 	- ���͹� �ð�																				*/
	/* 	- ��Ż �ð�																					*/
	/* 	- �� �ΰ��� ���ýð��� ������ Ÿ�Ӿƿ��� �߻��Ѵ�.											*/
	/* 	- Ÿ�� �ƿ��� �߻��ϸ� �����̳�, �۽��� ���߿� �����Ѵ�.										*/
	/* 	- ������ ���, Ÿ�� �ƿ� �߻� �������� �����͸� �д´�.										*/
	/* 	- �۽��� ���, ó������ �ٽ� ����. ���͹� �ð��� ����.										*/
	/*																								*/
	/* ���͹� �ð��� ������ ������ ���� Ÿ�̸� ���ø� �ϱ� ���� ���̱� ������ 1byte�� 1ms����		*/
	/* �̹Ƿ� ������ ��� �ָ� �ȴ�.																*/
	/*																								*/
	/* -------------------------------------------------------------------------------------------- */
	/* ����																							*/
	/* -------------------------------------------------------------------------------------------- */
	/*																								*/
	/* 	- ReadIntervalTimeout : ������ ������ �� ������ �ִ� ��� �ð��� msec�� �����Ѵ�.			*/
	/* 	- ReadTotalTimeoutMuliplier�� ReadTotalTimeoutConstant										*/
 	/* 																								*/
	/* 	Read Interval Time = ReadIntervalTimeout													*/
	/* 	Read Total time = (ReadTotalTimeoutMuliplier * ������ Byte��) + ReadTotalTimeoutConstant	*/
 	/* 																								*/
	/* -------------------------------------------------------------------------------------------- */
	/* �۽�																							*/
	/* -------------------------------------------------------------------------------------------- */
	/*																								*/
	/* 	- �۽ſ��� Interval Time�� ����.																*/
	/* 	- WriteTotalTimeoutMuliplier�� WriteTotalTimeoutConstant										*/
	/* 	Write Total time = (WriteTotalTimeoutMuliplier * �۽��� Byte��) + WriteTotalTimeoutConstant	*/
	/* -------------------------------------------------------------------------------------------- */

	/* --------- */
	/* ���� �� 1 */
	/* -------------------------------------------------------------------------------------------- */
	/* ���ο� ������(��Ŷ)�� ������ ���, �д� ������ ���ð� ���� ��� �����ϴ� ����ü ���� ���  */
	/* -------------------------------------------------------------------------------------------- */
	/* timeouts.ReadIntervalTimeout			= MAXDWORD; 											*/
	/* timeouts.ReadTotalTimeoutMultiplier	= 0;													*/
	/* timeouts.ReadTotalTimeoutConstant	= 0;													*/
	/* timeouts.WriteTotalTimeoutMultiplier	= 0;													*/
	/* timeouts.WriteTotalTimeoutConstant	= 0;													*/
	/* -------------------------------------------------------------------------------------------- */

	/* --------- */
	/* ���� �� 2 */
	/* -------------------------------------------------------------------------------------------- */
	/* ReadTotalTimeoutConstant�� ���� Timeout ������ �ǹ̰� ����. 10 msec�� ������ Ÿ�Ӿƿ� �߻�   */
	/* -------------------------------------------------------------------------------------------- */
	/* timeouts.ReadIntervalTimeout			= MAXDWORD; 											*/
	/* timeouts.ReadTotalTimeoutMultiplier	= 0;													*/
	/* timeouts.ReadTotalTimeoutConstant	= 10;													*/
	/* timeouts.WriteTotalTimeoutMultiplier	= 0;													*/
	/* timeouts.WriteTotalTimeoutConstant	= 0;													*/
	/* -------------------------------------------------------------------------------------------- */

	/* --------- */
	/* ���� �� 3 */
	/* -------------------------------------------------------------------------------------------- */
	/* ������ ����Ʈ�� ������ ������ ��ȯ���� �ʴ� ���� (CTRL+Alt+Del Ű�θ� �������� �� ����)      */
	/* -------------------------------------------------------------------------------------------- */
	/* timeouts.ReadIntervalTimeout			= 0; 													*/
	/* timeouts.ReadTotalTimeoutMultiplier	= 0;													*/
	/* timeouts.ReadTotalTimeoutConstant	= 0;													*/
	/* timeouts.WriteTotalTimeoutMultiplier	= 0;													*/
	/* timeouts.WriteTotalTimeoutConstant	= 0;													*/
	/* -------------------------------------------------------------------------------------------- */

	/* --------- */
	/* ���� �� 4 */
	/* -------------------------------------------------------------------------------------------- */
	/* ���� 20ms ���� Serial data�� �����Ͽ� ó���ϴ� �ڵ��� ���, 20ms�� �����µ��� ���ŵ� data��  */
	/* ���� ��쿡 timeout�� �߻��ϵ��� �ϰ� �ʹٸ� (��, data�� ���ŵ� ������ ��� �̻� ��ٸ��� �� */
	/* �� �ڵ��� ���� ������ �����ϰ� �ϰ� �ʹٸ�)                                                  */ 
	/* -------------------------------------------------------------------------------------------- */
	/* timeouts.ReadIntervalTimeout			= MAXDWORD; 											*/
	/* timeouts.ReadTotalTimeoutMultiplier	= 0;													*/
	/* timeouts.ReadTotalTimeoutConstant	= 20; (Ȥ�� Windows�� �ǽð����� �������� �����Ƿ� 100) */
	/* timeouts.WriteTotalTimeoutMultiplier	= 0;													*/
	/* timeouts.WriteTotalTimeoutConstant	= 0;													*/
	/* -------------------------------------------------------------------------------------------- */

	return TRUE;
}

/*
 desc : DCB ����
 parm : byte_size	- �۽�/���� ����Ʈ ũ�� ���� (��, 5 ~ 8 bit)
		stop_bit	- ���� ��Ʈ ��
		parity		- �и�Ƽ ��Ʈ ��
		is_binary	- �ۼ��� ������ ���� ���̳ʸ� ������� �ƴ���
 retn : TRUE or FALSE
*/
BOOL CSerialComm::SetCommState(UINT8 byte_size, UINT8 stop_bit, UINT8 parity, BOOL is_binary)
{
	/* ���� ��� ��Ʈ�� ������ �� �������� */
	DCB stDCB	= {NULL};
	stDCB.DCBlength	= sizeof(DCB);
	if (!::GetCommState(m_hComm, &stDCB))
	{
		PutErrMsg(L"Failed to invoke the <GetCommState> function", 0x01);
		return FALSE;
	}

	/* DCB ȯ�� ���� ���� (��� ��Ʈ�� ���� �ӵ� ����) */
	stDCB.BaudRate	= m_u32BaudRate;
	stDCB.ByteSize	= byte_size;
	stDCB.Parity	= parity;
	stDCB.StopBits	= stop_bit;
	stDCB.fBinary	= is_binary;
	stDCB.fParity	= (parity != 0);
	/* �帧��� ������� �����Ƿ� */
	stDCB.fInX		= FALSE;
	stDCB.fOutX		= FALSE;
#if 0
	stDCB.XonChar	= 0;
	stDCB.XoffChar	= 0;
#endif
	/* �⺻ �� ���� */
	stDCB.fOutxCtsFlow	= FALSE;				/* Disable CTS monitoring */
	stDCB.fOutxDsrFlow	= FALSE;				/* Disable DSR monitoring */
	stDCB.fDtrControl	= DTR_CONTROL_DISABLE;	/* Disable DTR monitoring */
	stDCB.fRtsControl	= RTS_CONTROL_DISABLE;	/* Disable RTS (Ready To Send) */
	stDCB.XonLim		= 2048;
	stDCB.XoffLim		= 1024;

	/* ��� �帧 ���� ���� ���� */
	switch (m_enHandShake)
	{
	case ENG_SHOT::en_shot_led_can		:
	case ENG_SHOT::en_shot_strobe_camera	:
	case ENG_SHOT::en_shot_handshake_off	:
	case ENG_SHOT::en_shot_handshake_jted	:
		break;
	case ENG_SHOT::en_shot_handshake_hw	:
		stDCB.fOutxCtsFlow	= TRUE;			/* Enable CTS monitoring */
		stDCB.fOutxDsrFlow	= TRUE;			/* Enable DSR monitoring */
		break;
	case ENG_SHOT::en_shot_handshake_sw	:
		stDCB.fOutX			= TRUE;			/* Enable XON/XOFF for transmission */
		stDCB.fInX			= TRUE;			/* Enable XON/XOFF for receiving */
		stDCB.XonLim		= 2048;
		stDCB.XoffLim		= 1024;
		break;
	case ENG_SHOT::en_shot_img_opt_lens	:
		stDCB.StopBits		= 2;			/* �̷� ���� �� ����? (���� �������� �Ѵ�.) */
		break;
	}

	/* ��� ��Ʈ ���� �� �缳�� */
	if (!::SetCommState(m_hComm, &stDCB))
	{
		PutErrMsg(L"Failed to invoke the <SetCommState> function", 0x01);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : ��� ��ġ�� � �̺�Ʈ���� ������ ������ ����
 parm : evt_mask	- [in]  Ȱ��ȭ �� �̺�Ʈ ���� ��, ������ ��� �̺�Ʈ ����
							0 ���� ���, ��� �̺�Ʈ�� ��Ȱ��ȭ (��, ���� ����)
							���� ���� ������ OR ����� �� ����

							EV_BREAK	: �Է¿��� �극��ũ �˻�
							EV_CTS		: CTS (clear-to-send)��ȣ ���� ��ȯ
							EV_DSR		: DSR (data-set-ready)��ȣ ���� ��ȯ
							EV_ERR		: ���λ��� ���� �߻�
										  ���λ��� ���� : CE_FRAME, CE_OVERRUN, and CE_RXPARITY
							EV_RING		: ��ȭ�� ��ȣ�� ����
							EV_RLSD		: RLSD(reveive-line-signal-detect ���ż� ��ȣ �˻���)���� �� ����
							EV_RXCHAR	: ���ο� ���ڰ� ���ŵǾ� �Է� ���ۿ� ����
							EV_RXFLAG	: �̺�Ʈ ���ڰ� ���ŵǾ� �Է� ���ۿ� ����
							EV_TXEMPTY	: �۽Ź��ۿ� ������ ���ڰ� ���� ��
										  ��, WriteFile ȣ�� �� ������ ��� �����Ͱ� �������� ������
										  �Ϸ�Ǿ��� ��, �̺�Ʈ�� �߻���Ŵ
 retn : TRUE or FALSE
*/
BOOL CSerialComm::SetCommMask(UINT32 evt_mask)
{
	if (m_hComm && !::SetCommMask(m_hComm, evt_mask))
	{
		PutErrMsg(L"Failed to invoke the <SetCommMask> function", 0x01);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : ���ϴ� �̺�Ʈ�� �߻� �ߴ��� ���� ��� ...
 parm : None
 retn : ������ �̺�Ʈ �ڵ� �� ��ȯ (EV_~ ��, EV_RXCHAR, EV_RXFLAG, EV_TXEMPTY, EV_BREAK, EV_ERR, ...)
		0x0000	- �߻��� �̺�Ʈ ����. (�̺�Ʈ ���� ����� ������ �� ��ȯ��)
*/
UINT16 CSerialComm::WaitCommEvent()
{
	DWORD dwEventMask	= 0;

	/* ��� ��Ʈ�κ��� ���ŵǴ� �̺�Ʈ ��� */
	if (!::WaitCommEvent(m_hComm, &dwEventMask, NULL))
	{
		/* ��Ÿ ������ �߻��� ��� */
		return EV_ERR;
	}

	/* ��Ʈ�κ��� ���ŵ� ������ �ñ׳��� ������ */
	if (EV_RXCHAR == (dwEventMask & EV_RXCHAR))			return EV_RXCHAR;
	/* ��Ʈ�κ��� �۽� �Ϸ� �ñ׳��� ������ */
	else if (EV_TXEMPTY == (dwEventMask & EV_TXEMPTY))	return EV_TXEMPTY;

	return 0x0000;
}

/*
 desc : WaitCommEvent�� ������ �����. ��, While loop ���������� ����
 parm : None
 retn : None
*/
VOID CSerialComm::WaitCommEventBreak()
{
	if (m_hComm && !::SetCommMask(m_hComm, 0))
	{
		PutErrMsg(L"Failed to invoke the <SetCommMask> function", 0x01);
	}
}

/*
 desc : ���� ���ۿ� ����� ������ ũ�� ��ȯ
 parm : None
 retn : ��ȯ ������ �������� ũ�� ��ȯ (����: Bytes)
*/
UINT32 CSerialComm::GetReadSize()
{
	if (m_pReadBuff)	return (UINT32)m_pReadBuff->GetReadSize();
	return 0;
}

/*
 desc : ��� ��Ʈ���� ������ ���� ó��
 parm : time_out	- [in]  ��� ��Ʈ ���۷κ��� �о���̴µ� �� ��� �ð� (����: msec)
							�־��� �ð���ŭ �� �о���̴µ� ��� ��
 retn : �о ���ۿ� ����� ������ ũ��, 0 - �б� ����
*/
UINT32 CSerialComm::ReadData(UINT32 time_out)
{
	BOOL bReaded		= FALSE;
	DWORD dwQueBytes, dwReadBytes, dwReadTotal = 0, dwErrorFlags;
	UINT64 u64ReadTime	= GetTickCount64();
	PUINT8 pPortBuff	= m_pPortBuff;
	COMSTAT stComStat	= {NULL};

	m_enCommStatus	= ENG_SCSC::en_scsc_reading;

	/* ������ ����������, ���ŵ� �����Ͱ� �ִ��� Ȯ�� */
	ClearCommError(m_hComm, &dwErrorFlags, &stComStat);
	/* �ý��� ť���� ������, ���ۿ��� �����Ͱ� ���� ? */
	if (stComStat.cbInQue < 1)	return 0;

	/* ���ۿ� �ִ� ��� �����͸� �о� ������. */
	do {

		/* �ý��� ť�� ���ŵ� �����Ͱ� �ִ��� Ȯ�� (���� ũ�Ⱑ ���� �� ���� �б�) */
		dwQueBytes	= min (m_u32PortBuff, stComStat.cbInQue);

		/* ���ۿ� �ϴ� �о���̴µ�, ���� �о���� ���� ���ٸ� */
		bReaded = ReadFile(m_hComm, pPortBuff, dwQueBytes, &dwReadBytes, &m_ovRead);
		if (!bReaded)
		{
			/* ���� �Ÿ��� ���Ҵٸ� ... (�б� ��Ⱑ �ɸ� ���, �ణ ����) */
			if (GetLastError() == ERROR_IO_PENDING)
			{
				/* time_out ������ �ð���ŭ ��ٷ� �ش� */
				while (!GetOverlappedResult(m_hComm, &m_ovRead, &dwReadBytes, TRUE))
				{
					m_u32CommError = GetLastError();
					if (m_u32CommError != ERROR_IO_INCOMPLETE)
					{
						dwReadBytes	= 0;
						ClearCommError(m_hComm, &dwErrorFlags, &stComStat);
						break;
					}
				}
			}
			else
			{
				dwReadBytes	= 0;
			}
		}

		if (dwReadBytes)
		{
			/* ������� �о���� �������� ũ�� ���� */
			dwReadTotal	+= dwReadBytes;
			/* �׸���, �̾ ���ŵǴ� �����͸� ������ ���� ������ �̵� */
			pPortBuff	+= dwReadBytes;
			/* Ȥ�� �� �о���� �����Ͱ� �ִ��� Ȯ�� */
			ClearCommError(m_hComm, &dwErrorFlags, &stComStat);
		}

		/* �о���̴µ�, ���� ������ �ð����� ���, �ڵ����� ���� �������� */
		if (GetTickCount64() > (u64ReadTime + time_out))
		{
			dwReadTotal	= 0;	/* �б� ���� �ߴٰ� ���� */
			break;
		}

	/* �� �о���� �����Ͱ� �ִ��� Ȯ�� or �� �̻� �о ������ ���� ������ ������ Ȯ�� */
	} while (dwReadBytes && ((dwReadTotal + stComStat.cbInQue) < m_u32PortBuff));

	/* IO Pending �÷��� �ʱ�ȭ */
	m_bIsReadPending	= FALSE;

	/* -------------------------------------------------------------------------------------- */
	/* ��Ʈ�κ��� �о���� �������� ũ�Ⱑ ���� ���� (Ring Buffer) �� �������� ũ�� �ʾƾ� �� */
	/* -------------------------------------------------------------------------------------- */
	if (dwReadTotal && dwReadTotal < m_pReadBuff->GetWriteSize())
	{
		/* ��Ʈ�κ��� ���������� �о���� �����͸� ���� ����(Ring Buffer)�� ���� */
		m_pReadBuff->WriteBinary(m_pPortBuff, dwReadTotal);
	}

	/* ������ �б� ���� */
	m_enCommStatus	= ENG_SCSC::en_scsc_readed;

	return dwReadTotal;
}

/*
 desc : ��� ��Ʈ�� ������ �۽� ó��
 parm : data	- [in]  1 Byte �۽� ������
		time_out- [in]  �۽��ϴµ�, �� �ҿ�Ǵ� �ð� ���� (����: msec)
						�� �ð����� �۽����� ������ ���� ó��
 retn : 0 - �۽� ����, ��� - �۽� ũ��
*/
UINT32 CSerialComm::WriteByte(UINT8 data, UINT32 time_out)
{
	BOOL bSendSucc	= TRUE;
	DWORD dwSentByte	= 0, dwErrorFlags	= 0;
	UINT64 u64WriteTime	= GetTickCount64();
	COMSTAT stComStat	= {NULL};

	/* ������ �۽��� �����Ͱ� ���� �ִ��� Ȯ�� (��� ��Ʈ�� ��� ������ Reset ���� */
	ClearCommError(m_hComm, &dwErrorFlags, &stComStat);
	
	do {

		/* ������ �۽� ������ ���, IO_PENDING���� ���� */
		if (!WriteFile(m_hComm, &data, 1, &dwSentByte, &m_ovWrite))
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				/* 1�� ���� ���� �̺�Ʈ ��� ��, �Ǵ� */
				if (WaitForSingleObject(m_ovWrite.hEvent, 100) != WAIT_OBJECT_0)
				{
					bSendSucc	= FALSE;
					LOG_ERROR(ENG_EDIC::en_engine, L"Enter - WaitForSingleObject - wait_fail (send_fail)");
				}
				else
				{
					/* �۽� �Ϸ� �����Ͱ� �ִ��� Ȯ�� */
					GetOverlappedResult(m_hComm, &m_ovWrite, &dwSentByte, FALSE);
					if (dwSentByte < 1)
					{
						bSendSucc	= FALSE;
						LOG_ERROR(ENG_EDIC::en_engine, L"Enter - WaitForSingleObject - wait_succ (send_fail)");
					}
				}
			}
			else
			{
				bSendSucc	= FALSE;
				TCHAR tzMesg[256] = {NULL};
				swprintf_s(tzMesg, 256, L"Enter - WaitForSingleObject - write_fail. err_cd=%d", GetLastError());
				LOG_ERROR(ENG_EDIC::en_engine, tzMesg);
			}
		}

		/* �۽��ϴµ� �ð��� �ʰ��Ǿ����� ���� */
		if (dwSentByte < 1 && GetTickCount64() > (u64WriteTime + time_out))
		{
			bSendSucc	= FALSE;
			dwSentByte	= 0;
			/* ������ ���� ���� ���� */
			break;
		}

	/* ��� ���۵� ������ ���� �ݺ� (�۽� �����̰ų�, 1 ����Ʈ ���� �Ϸ�� ������) */
	} while (!(bSendSucc && dwSentByte == 1));

	/* �ٽ� �ѹ� ��� ��Ʈ�� ��� ������ Reset �� */
	ClearCommError(m_hComm, &dwErrorFlags, &stComStat);

	/* IO Pending �÷��� �ʱ�ȭ */
	m_bIsWritePending	= FALSE;

	/* �۽� ���� */
	if (bSendSucc)	m_enCommStatus	= ENG_SCSC::en_scsc_writed;
	else			m_enCommStatus	= ENG_SCSC::en_scsc_write_fail;

	/* �۽��� ���������� �̷������, ��� ���� �ߴ��� ���� �� */
	return bSendSucc;
}

/*
 desc : ��� ��Ʈ�� ������ �۽� ó��
 parm : buff		- [in]  �۽� �����Ͱ� ����� ����
		size		- [both]�۽� �������� ũ�� �� �۽ŵ� �������� ũ�� ����
		time_byte	- [in]  ��Ŷ �� 1 Byte �� ���� ������ �ణ�� Term�� �ֱ� ���� �ð� �� (����: �и���)
		time_step	- [in]  ���� ��Ŷ ��ü�� ������ �� ���� �ٷ� �̾ ���� �� ������ ���� �ð� ����ϵ��� ��
 retn : 0 - �۽� ����, ��� - �۽� ũ��
*/
UINT32 CSerialComm::WriteData(PUINT8 buff, UINT32 size, UINT32 time_byte, UINT32 time_step)
{
	UINT32 u32Retry	= 0, i = 0,	u32SendBytes = size, u32SendedBytes = 0;
	PUINT8 pBuff	= buff;

	m_enCommStatus	= ENG_SCSC::en_scsc_writing;

	/* ������ ũ�⸸ŭ ��� ��Ʈ�� ������ �۽� �õ� */
	for (; (i<u32SendBytes) && (u32Retry < 3); i++)
	{
		if (!WriteByte(pBuff[i]))	u32Retry++;
		else						u32SendedBytes++;
		/* ������ ��� �ӵ��̾�� �ϳ�, �������� ��� ������ ������ ������ ����� �Ѵ� */
		/* 1 Bytes �� �۽��� ��, ��� �ð� */
		if (time_byte)	WaitPeekMessageQueue(time_byte);
	}
	/* 1 ���� ��Ŷ ������ ���������� ������ �� ������ ����ϱ� ���� (JusTek Board�� ���� ������... ���� ������.) */
	if (time_step)	WaitPeekMessageQueue(time_step);
	/* ��� ��Ʈ�� ���������� �۽ŵǾ��ٸ�, �۽ŵ� ������ ũ�� ��ȯ */
	return u32SendedBytes;
}

/*
 desc : ���������� Event Message�� ó���ϸ鼭, ���� �ð����� ���
 parm : wait	- [in]  ��� �ð� (����: �и���)
 retn : None
*/
VOID CSerialComm::WaitPeekMessageQueue(UINT64 wait)
{
	if (wait < 1)	return;
	UINT64 u64Tick	= GetTickCount64();
	MSG stMsg		= {NULL};  /* ������ ���� MSG ������ ���� ���� */

	do 	{

		/* PeekMessage �Լ��� �޽��� ť�� �̺�Ʈ ������ �ִٸ�, */
		/* �ش� �̺�Ʈ �޽����� ť���� ������ ���������� ó���� */
		if (PeekMessage(&stMsg, NULL, 0, 0, PM_REMOVE) == TRUE)
		{
			/* PM_REMOVE = ó���Ŀ� ť���� �޽��� ���� */
			TranslateMessage(&stMsg);
			DispatchMessage(&stMsg);
		}

		/* 1 �и��� ���� Wait ��Ŵ */
		uSleep(1);
		/* ���� �ð� (�и���)���� ��⸦ �ߴ��� �˻� */
		if ((GetTickCount64() - u64Tick) > wait)	break;

	} while (1);
}

/*
 desc : micro �������� wait ��Ŵ
 parm : parm : usec	- [in]  ���(Sleep) �ϰ��� �ϴ� �ð� (����: micro-second = 1 / 1000000 ��)
 retn : None
*/
VOID CSerialComm::uSleep(UINT64 usec)
{
	/* Microsecond (1 / 1000000��)���� �����ϱ� ���� ���� Type */
	LARGE_INTEGER perf, start, now;

	/* GetTickCount�Լ��� 10 msec �̻� ���� �߻�, �� ���� �� ��Ȯ�� �Լ��� �Ʒ� �Լ� */
	if (!QueryPerformanceFrequency(&perf))	return Sleep((UINT32)usec);
	if (!QueryPerformanceCounter(&start))	return Sleep((UINT32)usec);

	do {

		QueryPerformanceCounter((LARGE_INTEGER*)&now);

	  /* 1 ����ũ�� �� (1 / 1000000) ���� ���Ͽ�, �ʰ��Ǿ����� ���� ���� ���� */
	} while ((now.QuadPart - start.QuadPart) / (perf.QuadPart / 1000000.0f) <= usec);
}

/*
 desc : ���� ��� ��Ʈ�� ȯ�� ���� �� ��ȯ
 parm : data	- [out] ��ȯ�Ǿ� ����� ����ü ����
 retn : TRUE or FALSE
*/
BOOL CSerialComm::GetConfigPort(STG_SSCI &data)
{
	DCB stDCB			= {NULL};
	COMMTIMEOUTS stCTS	= {NULL};

	/* DCB �� ��� */
	if (!::GetCommState(m_hComm, &stDCB))
	{
		PutErrMsg(L"Failed to invoke the <GetCommState> function", 0x01);
		return FALSE;
	}
	/* ��� ��Ʈ�� ���� �б�/�����ϴµ� �ɸ��� ��� �ð� ���� ��� */
	if (!::GetCommTimeouts(m_hComm, &stCTS))
	{
		PutErrMsg(L"Failed to invoke the <GetCommTimeouts> function");
		return FALSE;
	}

	/* ���� ������ ���� ���� ����ü�� ��ȯ */
	data.byte_size				= stDCB.ByteSize;
	data.stop_bits				= stDCB.StopBits;
	data.is_binary				= stDCB.fBinary;
	data.is_parity				= stDCB.fParity;
	data.parity					= stDCB.Parity;

	data.port_buff				= m_u32PortBuff;
	data.read_Interval_time		= stCTS.ReadIntervalTimeout;
	data.read_total_multiple	= stCTS.ReadTotalTimeoutMultiplier;
	data.read_const_time		= stCTS.ReadTotalTimeoutConstant;
	data.write_total_multiple	= stCTS.WriteTotalTimeoutMultiplier;
	data.write_const_time		= stCTS.WriteTotalTimeoutConstant;

	return TRUE;
}

/*
 desc : ���� ��� ��Ʈ�� ȯ�� ���� �� ����
 parm : data	- [in]  ����� ���� ���� ����� ����ü
 retn : TRUE or FALSE
*/
BOOL CSerialComm::SetConfigPort(LPG_SSCI data)
{
	BOOL bSucc	= TRUE;

	if (!m_bIsOpened)
	{
		PutErrMsg(L"Can't opened the serial port");
		return FALSE;
	}

	/* Setup Send & Recv Buffer size */
	if (bSucc)	bSucc = SetupComm(data->port_buff);
	/* Setup Comm timeouts */
#if 0
	if (bSucc)	bSucc = SetTimeout(data->read_Interval_time,
								   data->read_total_multiple, data->read_const_time,
								   data->write_total_multiple, data->write_const_time);
#else
	if (bSucc)	bSucc = SetTimeout(ENG_CRTB::en_crtb_read_timeout_nonblocking);
#endif
	/* Setup Comm status */
	if (bSucc)	bSucc = SetCommState(data->byte_size, data->stop_bits, data->parity, data->is_binary);

	return bSucc;
}

/*
 desc : ���� ������ ������ ��ȯ (���� �����ؼ� ��ȯ�ϴ� ���� �ƴ϶�, �߶󳻱� �� �� ��ȯ �Ѵ�)
		���� ��ȯ�� �� ���� ���۴� ��ȯ ������ �����͸� ���� �ְ� �ȴ�
 parm : data	- [out] ��ȯ�Ǿ� ����� ����
		size	- [in]  ��ȯ�Ǵ� �������� ũ��
 retn : 0 - ��ȯ�Ǵ� �����Ͱ� ���� (��ȯ ����), ��� - ��ȯ�Ǵ� �������� ũ��
*/
UINT32 CSerialComm::PopReadData(PUINT8 data, UINT32 size)
{
	UINT32 u32PopSize	= (UINT32)m_pReadBuff->GetReadSize();
	if (u32PopSize < 1)	return 0;

	/* �����ִ� �������� ũ�Ⱑ ��ȯ ��û�� �������� ũ�⺸�� ū ���, ��û�� ũ�⸸ŭ�� ��ȯ�ϱ� ���� */
	if (size < u32PopSize)	u32PopSize	= size;

	/* ������ ���� */
	if (!m_pReadBuff->ReadBinary(data, u32PopSize))
	{
		PutErrMsg(L"Failed to get the received data");
		return 0;
	}

	return u32PopSize;
}

/*
 desc : ���� ������ ������ ��ȯ (���� �����ؼ� ��ȯ�ϴ� ����, ���� ���۴� ������ �״�� ���� ����)
		���� ��ȯ�� �� ���� ���۴� ��ȯ ������ �����͸� ���� �ְ� �ȴ�
 parm : data	- [out] ��ȯ�Ǿ� ����� ����
		size	- [in]  ��ȯ�Ǵ� �������� ũ��
 retn : FALSE - ��ȯ ����, TRUE - ��û�� ũ�⸸ŭ ��ȯ ����
*/
BOOL CSerialComm::CopyReadData(PUINT8 data, UINT32 size)
{
	UINT32 u32CopySize	= (UINT32)m_pReadBuff->GetReadSize();

	/* ���� ���� �ִ� ������ ũ�⺸�� ��û�� ������ ũ�Ⱑ ũ�ٸ� ���� ó�� */
	if (u32CopySize < size)	return FALSE;

	/* ������ ���� */
	if (!m_pReadBuff->CopyBinary(data, size))
	{
		PutErrMsg(L"Failed to copy the received data");
		return FALSE;
	}

	return TRUE;
}

/*
 desc : �ø��� ��� ��Ʈ�κ��� ���ŵ� ������ ������ ��� �о ������.
 parm : time_out	- [in]  ���� �ð� ���� ���ŵ� ���� ������ ��� ���� (����: msec)
 retn : None
*/
VOID CSerialComm::ReadAllData(UINT32 time_out)
{
	UINT8 u8Read[2]	= {NULL};
	UINT32 u32Read	= 1;
	UINT64 u64Tick	= GetTickCount64();

	do 	{

		if (!PopReadData(u8Read, u32Read))	break;

		/* ���� ������ ��� ������ ���� Ȯ�� */
		if (GetTickCount64() > (u64Tick + time_out))	break;

	} while (u32Read);
}

/*
 desc : �˻� ����� ���� 1���� ���� ���� �ִ���, �ִٸ� �ش� ��ġ�� ��ȯ (zero-based)
 parm : find	- [in]  �˻��ϰ��� �ϴ� ����
		pos		- [out] �˻�(ã��)�� ��ġ (zero-based)
 retn : 0x00 - �˻� ����, 0x01 - �˻� ����
*/
BOOL CSerialComm::FindChar(UINT8 find, UINT32 &pos)
{
	return m_pReadBuff->FindChar(find, pos);
}

/*
 desc : �˻� ����� ���� 2 ���� ���� ���� �ִ���, �ִٸ� �ش� ��ġ�� ��ȯ (zero-based)
 parm : find1	- [in]  ù ��° �˻��ϰ��� �ϴ� ����
		find2	- [in]  �� ��° �˻��ϰ��� �ϴ� ����
		pos		- [out] �˻�(ã��)�� ��ġ (zero-based) (find2�� ��ġ ��ȯ)
 retn : 0x00 - �˻� ����, 0x01 - �˻� ����
*/
BOOL CSerialComm::FindChar(UINT8 find1, UINT8 find2, UINT32 &pos)
{
	return m_pReadBuff->FindChar(find1, find2, pos);
}

/*
 desc : �� ó�� (�� �κ�)�� �����ϴ� 1 byte �� ��ȯ
 parm : None
 retn : �� ��ȯ (0x00 ~ 0xff)
*/
UINT8 CSerialComm::GetHeadData()
{
	UINT8 u8Data	= 0x00;
	m_pReadBuff->PeekChar(0, u8Data);

	return u8Data;
}

/*
 desc : �ø��� ��� ��Ʈ�� Open �Ǿ����� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CSerialComm::IsOpenPort()
{
	if (!m_bIsOpened)
	{
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128, L"The serial port is not open [%s]", m_tzPort);
		LOG_WARN(ENG_EDIC::en_engine, tzMesg);
	}
	return m_bIsOpened;
}