
#pragma once

#include "../../inc/comm/ClientThread.h"

class CGFSCThread : public CClientThread
{
// ������/�ı���
public:

	CGFSCThread(UINT16 th_id, LPG_CSID config);
	virtual ~CGFSCThread();

// �����Լ� ������
protected:

	virtual BOOL		Connected();
	virtual VOID		DoWork();
	/* -------------------------------------------------------------------------- */
	/* ���� ��� ���� ����� �ƴϱ� ������, ������ �����Լ��� ���� �Լ� ���� ���� */
	/* -------------------------------------------------------------------------- */
	virtual BOOL		RecvData();

// ���� ����ü
protected:


// ���� ����
protected:

	BOOL				m_bValidAliveTime;	/* ���������� Alive Time �޾Ҵ��� ���� */
	BOOL				m_bSendPktGFiles;	/* �Ź� ������ ��Ŷ �۽� ���� �÷��� */

	UINT32				m_u32FileCount;		/* ���� �ֱٿ� �۽� Ȥ�� ���Ź���(��) �Ź� Ȥ�� ������ ������ ���� */
	UINT32				m_u32TotalFiles;	/* ��ü �۽ŵ� ���� ���� */
	UINT32				m_u32TotalSizes;	/* ���� �ֱٿ� �۽� Ȥ�� ���Ź���(��) �Ź� Ȥ�� ������ ������ �� ũ�� (�����) */
	UINT32				m_u32AliveCount;	/* Alive Time ���� ���� (�ִ� ���� �Ǹ� 1�� ����) */

	ENG_LNWE			m_enLastErrorLevel;	/* ���� �ֱ��� ���� ���� �� �ӽ� ���� */

	PUINT8				m_pRecvBuff;		/* ��Ŷ ��ü �κ� */
	LPG_CSID			m_pstConfig;		/* ��ü ȯ�� ���� */

	CAtlList <STG_PGFP> m_lstPktGFiles;		/* �Ź� �ӽ� ���� */

	CMySection			m_syncPktCtrl;		/* ������ �ۼ��� ���� */
	CMySection			m_syncPktCmd;		/* �ֱٿ� ���ŵ� ��ɾ� �ӽ� ���� ����ȭ ��ü */

	CAtlList<STG_UTPH>	m_lstPktCmd;		/* ���� �ֱٿ� ��/���ŵ� ��ɾ� �ӽ� ���� ���� */


// ���� �Լ�
protected:

	VOID				CloseSocket(SOCKET sd, UINT32 time_out=0);

	BOOL				SendData(PUINT8 data, UINT32 size, UINT64 time_out=0);
	BOOL				SendData(ENG_PCGG cmd, PUINT8 data, UINT32 size, UINT64 time_out=0);
	BOOL				RecvData(PUINT8 data, UINT32 size);

	BOOL				NewGerberPath(PUINT8 data, UINT32 size);
	BOOL				CntGerberFiles(PUINT8 data, UINT32 size);
	BOOL				DelGerberPath(PUINT8 data, UINT32 size);
	BOOL				GetRecipeFiles(PUINT8 data, UINT32 size);
	BOOL				SetRecipeFiles(PUINT8 data, UINT32 size);

	BOOL				SetRecipeFiles(CStringArray *files);

	VOID				PushPktHead(LPG_UTPH head);


// ���� �Լ�
public:

	ENG_TPCS			GetTCPMode()		{	return m_enSocket;			}
	ENG_LNWE			GetLastErrorLevel()	{	return m_enLastErrorLevel;	}

	VOID				StopGerberSendFile();
	VOID				ResetPktGfiles();
	VOID				AddPktGFiles(STG_PGFP gfile);

	BOOL				IsSendGFileCompleted();
	DOUBLE				GetSendGFilesRate();
	/* ��ü �۽� Ȥ�� ���ŵ� ���� ũ�� �� ��ȯ */
	UINT32				GetTotalFileCount()	{	return m_u32TotalFiles;		}
	UINT32				GetTotalFileSize()	{	return m_u32TotalSizes;		}

	BOOL				PopPktHead(STG_UTPH &head);

	/* ----------------------------------------------------------------------------------------- */
	/*                            Packet Data Communication Part                                 */
	/* ----------------------------------------------------------------------------------------- */
	VOID				SetSetupGFile();
	BOOL				SetNewGerberPath(PTCHAR g_name);
	BOOL				SetDelGerberPath(PTCHAR g_name);
	BOOL				SetGerberFiles();
	BOOL				GetRecipeFiles();
	BOOL				SetRecipeFiles();
	BOOL				SetAliveTime(BOOL logs=FALSE);
	BOOL				IsRecvAliveTime()	{	return m_bValidAliveTime;	}
};