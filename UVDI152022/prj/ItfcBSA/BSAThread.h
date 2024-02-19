
#pragma once

#include "../../inc/comm/ClientThread.h"

class CSendQue;

class CBSAThread : public CClientThread
{
public:
	CBSAThread(LPG_CTCS vision, LPG_RBVC shmem);
	virtual ~CBSAThread();

/* �����Լ� */
protected:
	virtual BOOL		Connected();
	virtual VOID		DoWork();
	virtual BOOL		RecvData();

/* ���� ����ü
protected:


/* ���� ���� */
protected:

	BOOL				m_bInitUpdate;		/* Ʈ���� ���忡 ����ǰ� �� ���� �ʱ� �� ���� ����  */
	BOOL				m_bNotiEvent;		/* Event �߻� ���� */

	UINT64				m_u64RecvTime;		/* ������ ���ŵ� �ð� ���� */
	UINT64				m_u64ReqTime;		/* �ֱ������� ��û�� �ð� (����: �и���) */
	UINT64				m_u64SendTime;		/* ª�� �ð� ���� ���� �����͸� �۽����� ���ϵ��� �ϱ� ���� */
	UINT64				m_u64PeriodTime;	/* ������ ������ ������ �ð� ���� */

	PUINT8				m_pPktRecvData;		/* ���� ���� ������ �ӽ� ���� */
	PUINT8				m_pPktRecvFlag;		/* Packet ���ŵǾ����� ���� (0x00: None, 0x01 : Sended, 0x02 : Received) */
	PTCHAR				m_pEventMsg;		/* Evnet Noti Message �߻� ���� */
	PTCHAR				m_pLogMsg;

	LPG_RBVC			m_pstShMemVisi;		/* ���� �޸� ���� (��� ���� ������ �� ��Ÿ ������ �ӽ� ����) */

	CMySection			m_syncSend;			/* �۽� ����ȭ ��ü */
	CSendQue			*m_pSendQue;		/* �۽� ť ������ */

/* ���� �Լ� */
protected:

	BOOL				PktAnalysis(LPG_PSRB data);

	BOOL				PopSendData();
	BOOL				IsSendData();
	BOOL				IsPopEmptyData();

	VOID				CloseSocket(SOCKET sd, UINT32 time_out=0);

	VOID				SetSockData(UINT8 flag, LPG_PSRB data);
	VOID				SetSendData(LPG_PSRB data);
	VOID				SetRecvData(LPG_PSRB data);

	VOID				SetNotiMsg(PTCHAR mesg);
	VOID				SetNotiData(LPG_PSRB data);

	VOID				SetAlignGrab(PUINT8 data);


/* ���� �Լ� */
public:

	BOOL				IsInitRecvData()	{ return m_bInitRecv; };
	BOOL				IsRecvData(ENG_VCPC cmd);

	BOOL				SendData(ENG_VCPC cmd, PUINT8 data = NULL, UINT16 size = 0x0000);
	BOOL				SendCenteringData(ENG_VCPC cmd, UINT8 cam_no, UINT8 unit);
	BOOL				SendFocusData(ENG_VCPC cmd, UINT8 cam_no, UINT8 unit);	
	BOOL				SendGrabData(ENG_VCPC cmd, UINT8 point, UINT8 mark, UINT8 pattern, INT32 pos_x, INT32 pos_y);
	BOOL				SendViewData(ENG_VCPC cmd, UINT8 mode);
	BOOL				SendIllumination(ENG_VCPC cmd, UINT8 mark, UINT8 onoff);
	BOOL				SendJoblist(ENG_VCPC cmd, UINT64 job_mark_1, UINT64 job_mark_2);

	BOOL				IsUpdateStateTime(UINT64 time);
	VOID				SetUpdateStateTime()			{	m_pstShMemVisi->update_state_time = GetTickCount64(); }

	BOOL				IsVisionNotiEvent()				{	return m_bNotiEvent;	};
	VOID				ResetVisionNotiEvent()			{	m_bNotiEvent = FALSE;	};
	PTCHAR				GetVisionNotiEventMsg()			{	return m_pEventMsg;		};
};



