
/*
 desc : Main Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "MainThread.h"

#include "./Work/WorkMeas.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : parent	- [in]  �ڽ��� ȣ���� �θ� ���� �ڵ�
 retn : None
*/
CMainThread::CMainThread(HWND parent)
	: CThinThread()
{
	m_bRunCali	= FALSE;
	m_bWorking	= FALSE;
	m_hParent	= parent;
	/* ��ü ���� */
	m_pWorkMeas	= new CWorkMeas();
	ASSERT(m_pWorkMeas);
}

/*
 desc : �ı���
 parm : None
*/
CMainThread::~CMainThread()
{
	delete m_pWorkMeas;
}

/*
 desc : ������ ���۽ÿ� �ʱ� �ѹ� ȣ���
 parm : None
 retn : DI_TRUE or DI_FALSE
*/
BOOL CMainThread::StartWork()
{
	/* ������ 1 �� ���� ���� �����ϵ��� �� */
	CThinThread::Sleep(1000);

	return TRUE;
}

/*
 desc : ������ ���۽ÿ� �ֱ������� ȣ���
 parm : None
 retn : None
*/
VOID CMainThread::RunWork()
{
	/* ���� ���� */
	if (m_syncCali.Enter())
	{
		/* �Ϲ����� ����͸� ȣ�� */
		SendMesgParent(MSG_ID_NORMAL, 100);

		/* �۾� ���� ���ο� ���� */
		if (m_bRunCali)
		{
			m_bWorking	= TRUE;	/* ���� �۾� �� ... */
			DoWorkMeas();
		}
		else
		{
			m_bWorking	= FALSE;	/* ���� �۾� ������ ���� */
		}

		/* ���� ���� */
		m_syncCali.Leave();
	}
}

/*
 desc : ������ ����ÿ� ���������� �ѹ� ȣ���
 parm : None
 retn : None
*/
VOID CMainThread::EndWork()
{
}

/*
 desc : �θ𿡰� �޽��� ���� (SendMessage)
 parm : msg_id	- [in]  �޽��� ID (0, 1, 2, 4)
		timeout	- [in]  SendMessag Lifetime (unit: msec)
 retn : None
*/
VOID CMainThread::SendMesgParent(UINT16 msg_id, UINT32 timeout)
{
	DWORD_PTR dwResult	= 0;
	LRESULT lResult		= 0;

	/* �θ𿡰� �̺�Ʈ �޽��� ���� */
#if 1
	lResult	= ::SendMessageTimeout(m_hParent, WM_MAIN_THREAD, msg_id, 0L,
								   SMTO_NORMAL, timeout, &dwResult);
	if (0 == lResult)
	{
#if 1
		/* ���� �߻��� ���� �� ���� */
		TRACE("MainThread : SendMessage Time out <Normal> = %d \n", GetLastError());
#endif
	}
#else
	::PostMessage(m_hParent, WM_MAIN_THREAD, msg_id, 0L);
#endif
}

/*
 desc : �۾� ����
 parm : mode	- [in]  0x00 : ���� ����, 0x01 : ���� ����
		cam_id	- [in]  Align Camera Index (1 or 2)
		thick	- [in]  Material Thickness (unit: um)
		meas	- [in]  Measurement Information
 retn : None
*/
VOID CMainThread::StartCali(UINT8 cam_id, UINT16 thick, LPG_ACCS meas)
{
	/* ���� ���� */
	if (m_syncCali.Enter())
	{
		/* Align Camera is Calirbated Mode */
		uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);

		m_bRunCali	= TRUE;
		m_pWorkMeas->InitWork(cam_id, thick, meas);

		/* ���� ���� */
		m_syncCali.Leave();
	}
}

/*
 desc : �۾� ����
 parm : None
 retn : None
*/
VOID CMainThread::StopCali()
{
	/* ���� ���� */
	if (m_syncCali.Enter())
	{
		/* Align Camera is Calirbated Mode */
		uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
		m_bRunCali	= FALSE;

		/* ���� ���� */
		m_syncCali.Leave();
	}
}

/*
 desc : �۾� ���� (Measurement)
 parm : None
 retn : None
*/
VOID CMainThread::DoWorkMeas()
{
	UINT8 u8Step	= 0x00;
	UINT16 u16MsgID	= 0xffff;
	ENG_JWNS enState;

	/* ���� ��ġ�� �۾� ���� */
	m_pWorkMeas->DoWork();

	/* ���� Ȥ�� ���� �Ϸ� �Ǿ����� Ȯ�� */
	enState	= m_pWorkMeas->GetWorkState();
	switch (enState)
	{
	case ENG_JWNS::en_comp	:
	case ENG_JWNS::en_error	:	m_bRunCali	= FALSE;
								uvEng_Camera_SetCamMode(ENG_VCCM::en_none);	/* Align Camera is Calirbated Mode */
		return;																/* �۾��� ���̻� ���� ���ϵ��� ���� */
	case ENG_JWNS::en_wait	:	return;										/* �� �̻� �ٸ� �۾� ���� ��� */
	}

	/* ���� �۾� ��ġ Ȯ�� �̺�Ʈ �޽��� ���� ������ Ȯ�� */
	u8Step	= m_pWorkMeas->GetWorkStep();
	switch (u8Step)
	{
	case 0x05	:	u16MsgID = MSG_ID_MEAS_MARK;	break;
	case 0x0d	:	u16MsgID = MSG_ID_MEAS_CALI;	break;
	}
	/* ���� �޽����� �ִ��� ���ο� ���� */
	if (0xffff != u16MsgID)	SendMesgParent(u16MsgID);
	/* ��� �۾��� �Ϸ�Ǿ����� */
	if (m_pWorkMeas->IsCompleted())	SendMesgParent(MSG_ID_WORK_COMP);
}

/*
 desc : ���� Grabbed Image�� ó�� ��� �� ��ȯ
 parm : index	- [in]  ���� ����� ��� ������ 0, ���� ����� ��� 0 or 1
 retn : ó�� ��� ���� ����� ����ü ������ ��ȯ
*/
LPG_ACGR CMainThread::GetGrabbedResult(UINT8 index)
{
	return !m_bRunCali ? NULL : m_pWorkMeas->GetGrabbedResult();
}

/*
 desc : ���� ���� ���� ��� ���� ��ȣ ��ȯ
 parm : row	- [out] ��ȯ�Ǿ� ����� ���� ��ȣ (1 based)
		col	- [out] ��ȯ�Ǿ� ����� ���� ��ȣ (1 based)
 retn : None
*/
VOID CMainThread::GetCurRowCol(INT32 &row, INT32 &col)
{
	if (!m_bRunCali)	return;
	/* ��� ���� ��ġ �� ��ȯ */
	row	= m_pWorkMeas->GetCurRows();
	col	= m_pWorkMeas->GetCurCols();
}

/*
 desc : ���� ���� ���� ���� �� ��ȯ
 parm : offset_x	- [out] ��ȯ�Ǿ� ����� X �� ���� �� (����: mm)
		offset_y	- [out] ��ȯ�Ǿ� ����� Y �� ���� �� (����: mm)
 retn : None
*/
VOID CMainThread::GetCurOffset(DOUBLE &offset_x, DOUBLE &offset_y)
{
	if (!m_bRunCali)	return;
	/* ��� ���� ��ġ �� ��ȯ */
	offset_x	= m_pWorkMeas->GetOffsetX();
	offset_y	= m_pWorkMeas->GetOffsetY();
}

/*
 desc : ���� �۾� ����� ��ȯ
 parm : None
 retn : �۾� ����� (���� : Percentage, %)
*/
DOUBLE CMainThread::GetProcRate()
{
	return !m_bRunCali ? 0.0f : m_pWorkMeas->GetProcRate();
}

/*
 desc : �۾��ϴµ� �ҿ�� �ð� ��ȯ
 parm : None
 retn : �۾� �ҿ� �ð� (����: msec)
*/
UINT64 CMainThread::GetWorkTime()
{
	if (!m_bRunCali)	return 0;
	return m_pWorkMeas->GetWorkTime();
}
