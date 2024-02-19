
#pragma once

#include "../../inc/comm/ServerThread.h"


class CGFSSThread : public CServerThread
{
// ������/�ı���
public:

	CGFSSThread(UINT16 th_id, LPG_CSID config);
	virtual ~CGFSSThread();

// �����Լ� ������
protected:

	virtual BOOL		Accepted();
	virtual VOID		DoWork();
	/* -------------------------------------------------------------------------- */
	/* ���� ��� ���� ����� �ƴϱ� ������, ������ �����Լ��� ���� �Լ� ���� ���� */
	/* -------------------------------------------------------------------------- */
	virtual BOOL		RecvData();

// ���� ����ü
protected:


// ���� ����
protected:

	BOOL				m_bIsRecvCompleted;

	TCHAR				m_tzLastNewDir[MAX_PATH_LEN];	/* ���� �ֱٿ� ������ ���� �ӽ� ���� */
	TCHAR				m_tzLastDelDir[MAX_PATH_LEN];	/* ���� �ֱٿ� ������ ���� �ӽ� ���� */
	TCHAR				m_tzLastSaveDir[MAX_PATH_LEN];	/* ���� �ֱٿ� ����� ���� �ӽ� ���� (�Ź� ���� ����) */

	UINT32				m_u32TotalFiles;		/* ���� �ֱٿ� �۽� Ȥ�� ���Ź���(��) �Ź� Ȥ�� ������ ������ ���� */
	UINT32				m_u32FileCount;			/* ���� �ֱٿ� �۽� Ȥ�� ���Ź���(��) �Ź� Ȥ�� ������ ������ ���� */
	UINT32				m_u32TotalSizes;		/* ���� �ֱٿ� �۽� Ȥ�� ���Ź���(��) �Ź� Ȥ�� ������ ������ �� ũ�� (�����) */
	UINT32				m_u32TotalZipSize;		/* ���� �ֱٿ� �۽� Ȥ�� ���Ź���(��) �Ź� Ȥ�� ������ ������ �� ũ�� (����) */
	UINT64				m_u64RecvStartTime;		/* �Ź� ���� ���� ���� �ð� (����: �и���) */
	UINT64				m_u64RecvTotalTime;		/* �Ź� ���� ��� �޴µ� �ҿ�� �ð� (����: �и���) */

	UINT64				m_u64PeriodLiveTime;	/* �ֱ������� �۽��ϴ� ���� */
	UINT64				m_u64LastLiveTime;		/* ���� �ֱٿ� �۽��� �ð� */

	ENG_LNWE			m_enLastErrorLevel;		/* ���� �ֱ��� ���� ���� �� �ӽ� ���� */

	PUINT8				m_pRecvBuff;			/* ��Ŷ ��ü �κ� */
	LPG_CSID			m_pstConfig;			/* ��ü ȯ�� ���� */

	CMySection			m_syncReadWrite;		/* ������ �ۼ��� */
	CMySection			m_syncPktCmd;			/* �ֱٿ� ���ŵ� ��ɾ� �ӽ� ���� ����ȭ ��ü */

	CAtlList<STG_UTPH>	m_lstPktCmd;			/* ���� �ֱٿ� ���ŵ� ��ɾ� �ӽ� ���� ���� */

// ���� �Լ�
protected:

	VOID				CloseSocket(SOCKET sd, UINT32 time_out=0);

	BOOL				SendData(PUINT8 data, UINT32 size, UINT64 time_out=0);
	BOOL				SendData(ENG_PCGG cmd, PUINT8 data, UINT32 size, UINT64 time_out=0);
	BOOL				RecvData(PUINT8 data, UINT32 size);
	BOOL				NewGerberPath(PUINT8 data, UINT32 size);
	BOOL				DelGerberPath(PUINT8 data, UINT32 size);
	BOOL				SetGerberFiles(PUINT8 data);
	BOOL				SetGerberCount(PUINT8 data, UINT32 size);
	BOOL				SetGerberComp();
	BOOL				GetRecipeFiles();
	BOOL				SetRecipeFiles(PUINT8 data, UINT32 size);
	BOOL				SetRecipeFiles(CStringArray *files);
	BOOL				SetAliveTime();

	UINT8				NewGerberPath(PTCHAR path);
	BOOL				SaveGerberFile(PTCHAR path, PTCHAR file, PUINT8 data, UINT32 size, UINT32 f_no);

	VOID				PushPktHead(LPG_UTPH head);

// ���� �Լ�
public:

	ENG_TPCS			GetTCPMode()		{	return m_enSocket;			}
	ENG_LNWE			GetLastErrorLevel()	{	return m_enLastErrorLevel;	}

	BOOL				PopPktHead(STG_UTPH &head);

	UINT32				GetCurrentFileCount()	{	return m_u32FileCount;		}
	UINT32				GetTotalFileCount()		{	return m_u32TotalFiles;		}
	UINT32				GetTotalFileSize()		{	return m_u32TotalSizes;		}
	UINT32				GetTotalFileZipSize()	{	return m_u32TotalZipSize;	}
	DOUBLE				GetTotalFilesRate();

	/* ���� �ֱٿ� ���� / ���� / ���� (�Ź� ���� ���ϵ�) �� ���� ��ȯ */
	PTCHAR				GetLastDir(UINT8 type);

	/* ���� �ֱٿ� ���Ź޴µ� �ҿ�� �ð� ��ȯ (����: msec) */
	UINT64				GetRecvTotalTime()	{	return m_u64RecvTotalTime;	};
	BOOL				IsRecvCompleted()	{	return m_bIsRecvCompleted;	};
};