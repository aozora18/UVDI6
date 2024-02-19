
#pragma once

#include "../conf/event.h"
#include "MySection.h"

#define MAX_EVENT_SIZE	(1024 * 1024 * 10)	/* Max 10 MBytes */

class CMyEvent
{

// ������ & �ı���
public:

	/*
	 desc : ������
	 parm : count	- [in]  �̺�Ʈ �ִ� ��� ����
	 retn : None
	*/
	CMyEvent()
	{
		m_u32MaxCount	= 128;
		m_bSaveFile		= FALSE;
		m_fpEvent		= NULL;
	}
	CMyEvent(UINT32 count)
	{
		m_u32MaxCount	= count;
		m_bSaveFile		= FALSE;
		m_fpEvent		= NULL;
	}

	/*
	 desc : �ı���
	 parm : None
	 retn : None
	*/
	virtual ~CMyEvent()
	{
		RemoveAllEventData();
		/* �α� ���� �ݱ� */
		if (m_bSaveFile)	CloseFile();
	}


// ���� ����
protected:

	CHAR				m_szLogPath[MAX_PATH_LEN];

	BOOL				m_bSaveFile;	// �α� �����͸� ���Ϸ� ���� ����

	UINT32				m_u32MaxCount;

	FILE				*m_fpEvent;	// Event Data (����ü STG_EDRC)�� ������ ���� ������

	CAtlList <STG_ESTM>	m_lstEvent;

	CMySection			m_syncEvent;


// ���� �Լ�
protected:

	/*
	 desc : ���ο� �α� ���� �̸� (��ü ��� ����) ��ȯ
	 parm : file	- [out] ��ü ��ΰ� ���Ե� ���� �̸� ��ȯ
			size	- [in]  'file' ������ ũ��
	 retn : None
	*/
	VOID GetNewFile(CHAR *file, UINT32 size)
	{
		SYSTEMTIME stTm	= {NULL};
		CUniToChar csCnv;

		// ���� ��ǻ���� ���� �ð� ���
		GetLocalTime(&stTm);
		// �α� or ���� ���ϸ� ����
		sprintf_s(file, size, "%s\\%04d-%02d-%02d %02d.%02d.%02d.log",
							   m_szLogPath, stTm.wYear, stTm.wMonth, stTm.wDay,
							   stTm.wHour, stTm.wMinute, stTm.wSecond);
	}

	/*
	 desc : ���� ������ �ݱ�
	 parm : None
	 retn : None
	*/
	VOID CloseFile()
	{
		if (m_fpEvent)	fclose(m_fpEvent);
		m_fpEvent	= NULL;
	}

	/*
	 desc : ���ο� ������ ����
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL OpenFile()
	{
		CHAR szFile[MAX_PATH_LEN]	= {NULL};

		// ���� ���� �ݰ�
		CloseFile();
		// ���ο� ���ϸ� (��ü ��� ����) ���
		GetNewFile(szFile, MAX_PATH_LEN);
		// ���ο� ���� ����
		if (!(m_fpEvent = _fsopen(szFile, "wb", _SH_DENYWR)))	return FALSE;

		return TRUE;
	}

	/*
	 desc : �̺�Ʈ �α� �����͸� ���Ͽ� ����
	 parm : event	- [in]  �̺�Ʈ ����
	 retn : TRUE or FALSE
	*/
	BOOL SaveEventData(LPG_ESTM event)
	{
		UINT32 u32Size	= sizeof(STG_ESTM);

		// ���� ���Ͽ� ���� �ְų�, ���� ũ�� �̻� (10 MB)���� ����Ǿ� �ִٸ�, ���ο� ������ ������ �Ѵ�.
		if (!m_fpEvent || ftell(m_fpEvent) > MAX_EVENT_SIZE)
		{
			if (!OpenFile())	return FALSE;;
		}
		// ���� ����ǰ� �ִ� ������ ũ�Ⱑ ũ��, ���ο� ������ ����

		// �̺�Ʈ �α׸� ���Ϸ� ����
		if (1 != fwrite(event, u32Size, 1, m_fpEvent))
		{
			// �̺�Ʈ �α� ���� ���� ���� ���� ���
			CloseFile();
			return FALSE;
		}

		return TRUE;
	}

// ���� �Լ�
public:

	/*
	 desc : ���� ��ϵ� ���� ��ȯ
	 parm : None
	 retn : ��ϵ� ���� ��
	*/
	UINT32 GetCount()
	{
		return (UINT32)m_lstEvent.GetCount();
	}

	/*
	 desc : �α� ����
	 parm : path	- [in]  �αװ� ����� ���
	 retn : None
	*/
	VOID SetLogPath(TCHAR *path)
	{
		CUniToChar csCnv;

		sprintf_s(m_szLogPath, MAX_PATH_LEN, "%s", csCnv.Uni2Ansi(path));
		m_bSaveFile	= TRUE;
	}

	/*
	 desc : �ǽð� �߻��Ǵ� �α� �̺�Ʈ ���
	 parm : event	- [in]  Event ����
			mesg	- [in]  ���� �޽���
	 retn : TRUE or FALSE
	*/
	BOOL PushData(LPG_ESCV event, CHAR *mesg)
	{
		size_t s32MsgLen	= 0;
		STG_ESTM stEvent	= {NULL};
		SYSTEMTIME stTm		= {NULL};

		/* ������ ��ϵ� �̺�Ʈ ������ �� ������ �� �̻� ������� ���� */
		if (m_lstEvent.GetCount() > m_u32MaxCount)
		{
#ifdef _DEBUG
			CUniToChar csCnv;
			// �̺�Ʈ �ڵ忡 ���� ���ڿ� ���
			TRACE(L"del_event_data : %02x-%02x-%02x-%s\n",
				  stEvent.event.apps,
				  stEvent.event.indx,
				  stEvent.event.kind,
				  csCnv.Ansi2Uni(stEvent.mesg));
#endif
			return TRUE;
		}
		// ���� ����
		if (m_syncEvent.Enter())
		{
			// �̺�Ʈ ������ ����
			s32MsgLen	= strlen(mesg);	// 128 Bytes ���� �ʱ� ����
			if (s32MsgLen > 127)	s32MsgLen	= 127;
			memcpy(&stEvent.event, event, sizeof(STG_ESCV));
			memcpy(stEvent.mesg, mesg, s32MsgLen);
			// ���� �ý����� ���� �ð� ���
			GetLocalTime(&stTm);
			// �ð� ���� ����
			stEvent.time.yy	= stTm.wYear - 2000;
			stEvent.time.mm	= stTm.wMonth;
			stEvent.time.dd	= stTm.wDay;
			stEvent.time.hh	= stTm.wHour;
			stEvent.time.mn	= stTm.wMinute;
			stEvent.time.ss	= stTm.wSecond;

			/* ------------------------------------------- */
			/* ����� ����� ���, ������ �α� �޽��� ��� */
			/* �޸𸮿� ����ϱ� �� �ܰ迡�� �ݵ�� ȣ���� */
			/* ------------------------------------------- */
#ifdef _DEBUG
			CUniToChar csCnv;
			// �̺�Ʈ �ڵ忡 ���� ���ڿ� ���
			TRACE(L"add_event_data : %02x-%02x-%02x-%s\n",
				  stEvent.event.apps,
				  stEvent.event.indx,
				  stEvent.event.kind,
				  csCnv.Ansi2Uni(stEvent.mesg));
#endif
			// �α� ���� ���� ���� Ȯ��
			if (m_bSaveFile)	SaveEventData(&stEvent);
			/* ------------------------- */
			/* �̺�Ʈ (�α�) ������ ��� */
			/* ------------------------- */
			m_lstEvent.AddTail(stEvent);
		
			// ���� ����
			m_syncEvent.Leave();
		}

		return TRUE;
	}

	/*
	 desc : �ǽð� �߻��Ǵ� �̺�Ʈ ������ ����
	 parm : event	- [out] �̺�Ʈ ���� ��ȯ
	 note : TRUE or FALSE
	*/
	BOOL PopData(STG_ESTM &event)
	{
		BOOL bSucc	= FALSE;

		// ���� ����
		if (m_syncEvent.Enter())
		{
			// Event Data �޸� ����
			if (m_lstEvent.GetCount() > 0)
			{
				event	= m_lstEvent.GetHead();
				m_lstEvent.RemoveHead();
				bSucc	= TRUE;
			}

			// ���� ����
			m_syncEvent.Leave();
		}		

		return bSucc;
	}

	/*
	 desc : ���� ������ �߻��� ���� �� ��ȯ
	 parm : None
	 retn : Error �� ��ȯ
	*/
	UINT32 GetError()
	{
		return GetLastError();
	}

	/*
	 desc : ������� �߻��� ��� �̺�Ʈ �ʱ�ȭ
	 parm : None
	 retn : None
	*/
	VOID RemoveAllEventData()
	{
		POSITION pPos;
		/* ���� ���� */
		if (m_syncEvent.Enter())
		{
			m_lstEvent.RemoveAll();

			/* ���� ���� */
			m_syncEvent.Leave();
		}
	}

};
