
/*
 desc : �ø��� ��� ������
*/

#include "pch.h"
#include "SerialThread.h"


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
CSerialThread::CSerialThread(UINT8 port, UINT32 baud, UINT32 buff, ENG_SHOT shake)
{
	m_u8Port	= port;
	m_bIsReady	= FALSE;

	/* ���� ��ü ���� */
	m_pComm		= new CSerialComm(port, baud, buff, shake);
	ASSERT(m_pComm);
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CSerialThread::~CSerialThread()
{
	if (m_pComm)
	{
		/* �ø��� ��� ��Ʈ ���� */
		m_pComm->Close();
		/* �ø��� ��� ��ü ���� */
		delete m_pComm;
		m_pComm	= NULL;
	}
}

/*
 desc : ������ ������ ��, ���� �ѹ� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CSerialThread::StartWork()
{
	/* ��� ��Ʈ�κ��� �����͸� ��/������ �غ� �Ǿ��ٰ� ���� */
	m_bIsReady	= TRUE;

	return TRUE;
}

/*
 desc : ������ ���� ��, ���������� ȣ���
 parm : None
 retn : None
*/
VOID CSerialThread::RunWork()
{
	if (!m_bIsReady)	return;

	/* ���� ��� ��Ʈ�� ������ �ִ��� ���θ� Ȯ�� */
	/* ������ ���� ������, ���� �� �ٽ� ���� �õ� */
	if (!m_pComm->IsOpenPort())
	{
		/* ��� ��Ʈ �ٽ� ���� ���ؼ�, ���� �ð� ��� */
		Sleep(500);
		if (!m_pComm->Open())	m_pComm->Close();
		/* ��� ��Ʈ �ٽ� ���� ������, ���� �ð� ��� */
		Sleep(500);
		return;
	}

	/* �ֱ������� ������ ������ �ִ��� �б� �õ� */
	if (m_syncComm.Enter())
	{
		m_pComm->ReadData();
		m_syncComm.Leave();
	}
}

/*
 desc : ������ ����(����)�� ��, ���������� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CSerialThread::EndWork()
{
}

/*
 desc : ������ ������ �����ϰ��� �� ��,
		��� �̺�Ʈ ��� ���¸� Ż���ϵ��� �� ��, ���� �۾��� �����ؾ� �Ѵ�.
 parm : None
 retn : None
*/
VOID CSerialThread::Stop()
{
	/* ������ ��/���� �۾� �����ϵ��� �÷��� ������ ����       */
	m_bIsReady	= FALSE;
#if 0
	/* �ݵ�� ��� �̺�Ʈ ���� ����� ������ ��, ���� �۾� ���� */
	if (m_pComm)	m_pComm->WaitCommEventBreak();
#endif
	// ������ ������ ���� ��Ŵ
	CThinThread::Stop();
}

/*
 desc : ���� �ø��� ��� ��Ʈ�� ������ �ִ��� ���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CSerialThread::IsOpenPort()
{
	if (!m_pComm)	return FALSE;
	return m_pComm->IsOpenPort();
}

/*
 desc : ���� �ֱٿ� �߻��� ���� �޽��� ����
 parm : msg	- [in]  ���� ������ ����� ���ڿ� ������
 retn : None
*/
VOID CSerialThread::PutErrMsg(TCHAR *msg)
{
	if (m_pComm)	m_pComm->PutErrMsg(msg);
}

/*
 desc : ���� �ֱٿ� �߻��� ���� �޽��� ��ȯ
 parm : None
 retn : ���� �޽����� ����� �޸��� ��ġ(������) ��ȯ
*/
PTCHAR CSerialThread::GetErrMsg()
{
	if (!m_pComm)	return NULL;
	return m_pComm->GetErrMsg();
}

/*
 desc : ���� ��� ��Ʈ�� ȯ�� ���� �� ��ȯ
 parm : data	- [out] ��ȯ�Ǿ� ����� ����ü ����
 retn : TRUE or FALSE
*/
BOOL CSerialThread::GetConfigPort(STG_SSCI &data)
{
	if (!m_pComm)	return FALSE;
	return m_pComm->GetConfigPort(data);
}

/*
 desc : ���� ��� ��Ʈ�� ȯ�� ���� �� ����
 parm : data	- [in]  �����ϰ��� �ϴ� ���� ����� ����ü ������
 retn : TRUE or FALSE
*/
BOOL CSerialThread::SetConfigPort(LPG_SSCI data)
{
	if (!m_pComm)	return FALSE;
	return m_pComm->SetConfigPort(data);
}

/*
 desc : ���� ������ �ִ� ��� ��Ʈ�� ������ ���� �۾� ����
 parm : data	- [in]  �����Ϸ��� �����Ͱ� ����� ���� ������
		size	- [out] 'data' ���ۿ� ����� �������� ũ�� �� ���� ���۵� ũ�� �� ��ȯ
 retn : TRUE (���� ����) or FALSE (���� ����)
*/
BOOL CSerialThread::WriteData(PUINT8 data, UINT32 &size)
{
	UINT32 u32Write		= 0;

	if (!m_pComm)	return FALSE;

	/* ���� �����尡 ���� ������ / �ƴ��� Ȯ�� */
	if (!IsBusy())
	{
		m_pComm->PutErrMsg(L"The comm thread is stopped");
		return FALSE;
	}

	/* ���� ���� */
	if (m_syncComm.Enter())
	{
		/* ���� ���� �߿� �ִٰ� ��� ���� */
		m_bIsSend	= FALSE;
		/* ��� ��Ʈ�� ������ ���� (size ũ�⸸ŭ) */
		u32Write = m_pComm->WriteData(data, size);
		/* ���� ������ ���, ���� ���� �߿� ���� �ʴٰ� �÷��� �缳�� */
		if (u32Write < 1)
		{
			m_bIsSend = TRUE;	/* ��, ���� ���� �ߴٰ� ó�� */
			m_pComm->Close();	/* �ø��� ��� ��Ʈ �ݱ� */
		}
		/* ���� ���� */
		m_syncComm.Leave();
	}
	return u32Write > 0 ? TRUE : FALSE;
}

/*
 desc : �ֱ������� ���ŵ� �����Ͱ� �ִ��� �� ��û
 parm : port	- [in]  �˻� ����� ��� ��Ʈ ��ȣ
		data	- [in]  ���ŵ� �����Ͱ� ����� ���� ������
		size	- [out] ���ŵ� �����Ͱ� ����� 'data' ������ ũ��
						������ ���ŵ� �������� ũ��
 retn : TRUE or FALSE
*/
BOOL CSerialThread::PopReadData(PUINT8 data, UINT32 &size)
{
	if (!m_pComm)	return FALSE;

	/* ���� �����尡 ���� ������ / �ƴ��� Ȯ�� */
	if (!IsBusy())
	{
		m_pComm->PutErrMsg(L"The comm thread is stopped");
		return FALSE;
	}

	/* ���� ���� */
	if (m_syncComm.Enter())
	{
		/* ���ŵ� ������ ��������, ������ ũ�� ���� */
		size = m_pComm->PopReadData(data, size);
		/* ���� ���� */
		m_syncComm.Leave();
	}

	return TRUE;
}

/*
 desc : ���� ���ŵ� ���ۿ� ����� �������� ũ�� ��ȯ
 parm : port	- [in]  �˻� ����� ��� ��Ʈ ��ȣ
 retn : ���ŵ� ���ۿ� ����� �������� ũ��(����: Bytes) �� ��ȯ
*/
UINT32 CSerialThread::GetReadSize()
{
	UINT32 u32Size	= 0;
	if (!m_pComm)	return u32Size;

	/* ���� ���� */
	if (m_syncComm.Enter())
	{
		u32Size = m_pComm->GetReadSize();
		/* ���� ���� */
		m_syncComm.Leave();
	}

	return u32Size;
}

/*
 desc : ���ŵ� �����Ͱ� �ִ��� �� ��û (���� �����͸� ���縸 �ؼ� ������)
 parm : port	- [in]  �˻� ����� ��� ��Ʈ ��ȣ
		data	- [in]  ���ŵ� �����Ͱ� ����� ���� ������
		size	- [out] ���ŵ� �����Ͱ� ����� 'data' ������ ũ��
						������ ���ŵ� �������� ũ��
 retn : TRUE or FALSE
*/
BOOL CSerialThread::CopyReadData(PUINT8 data, UINT32 &size)
{
	if (!m_pComm)	return FALSE;
	/* ���� �����尡 ���� ������ / �ƴ��� Ȯ�� */
	if (!IsBusy())
	{
		m_pComm->PutErrMsg(L"The comm thread is stopped");
		return FALSE;
	}

	/* ���� ���� */
	if (m_syncComm.Enter())
	{
		/* ���ŵ� ������ ��������, ������ ũ�� ���� */
		size	= m_pComm->CopyReadData(data, size);
		m_syncComm.Leave();
	}

	return TRUE;
}

/*
 desc : ���� ��� ���� �� ��ȯ
 parm : None
 retn : ENG_SCSC
*/
ENG_SCSC CSerialThread::GetCommStatus()
{
	return m_pComm->GetCommStatus();
}

/*
 desc : ���� ��� ���� ���� ó��
 parm : None
 retn : None
*/
VOID CSerialThread::CommClosed()
{
	if (m_pComm)	m_pComm->Close();
}