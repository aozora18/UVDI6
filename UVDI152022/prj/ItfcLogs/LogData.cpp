
/*
 desc : Log Data ó�� (���� �� ����)
*/

#include "pch.h"
#include "MainApp.h"
#include "LogData.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

/*
 desc : ������
 parm : saved	- [in]  �α� ���� ���� ����
		type	- [in]  Log Type (0 : text, 1 : DB (SQLite3)
		mode	- [in]  �α� ���� ���� (None, All, Warning and Error, Only Error)
 retn : None
*/
CLogData::CLogData(BOOL saved, UINT8 type, ENG_LNWE mode)
	: CBase(saved, type, mode)
{
	UINT8 i	= 0x00;
	PTCHAR ptzMsgErr	= NULL;
	/* ���� �ֱٿ� ����� �α� �߻� �ð� */
	m_u64LastTick	= 0;
	m_u8LastError	= 0xff;

	/* �޽��� ���� �ӽ� �Ҵ� */
	m_ptzMsgLog	= (PTCHAR)::Alloc(LOG_MESG_SIZE * sizeof(TCHAR) + 1);
	m_ptzMsgErr	= (PTCHAR)::Alloc(LOG_MESG_SIZE * sizeof(TCHAR) * LAST_MESG_COUNT + sizeof(TCHAR) * LAST_MESG_COUNT /* ���� ���ڰ� �� ���� ���� */);
	m_ptzMsgPtr	= (PTCHAR*)::Alloc(LAST_MESG_COUNT * sizeof(PTCHAR));
	m_pLogType	= (PUINT8)::Alloc(LAST_MESG_COUNT);
	ASSERT(m_ptzMsgLog && m_ptzMsgErr && m_ptzMsgPtr && m_pLogType);
	ResetErrorMesg();

	/* 1���� �迭�� 2���� �迭ó�� ����ϱ� ���� */
	ptzMsgErr	= m_ptzMsgErr;
	for (; i<LAST_MESG_COUNT; i++)
	{
		m_ptzMsgPtr[i]	= ptzMsgErr;
		ptzMsgErr		+= (LOG_MESG_SIZE + 1);
	}
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CLogData::~CLogData()
{
	/* �޽��� ���� �ӽ� ���� */
	if (m_pLogType)		::Free(m_pLogType);
	if (m_ptzMsgLog)	::Free(m_ptzMsgLog);
	if (m_ptzMsgErr)	::Free(m_ptzMsgErr);
	if (m_ptzMsgPtr)	::Free(m_ptzMsgPtr);
}

/*
 desc : ���� �ֱٿ� �߻��� �α� �����͵� ��� ����
 parm : None
 retn : None
*/
VOID CLogData::ResetErrorMesg()
{
	m_u8LastError	= 0xff;
	memset(m_pLogType, 0x00, LAST_MESG_COUNT);
	wmemset(m_ptzMsgLog, 0x00, LOG_MESG_SIZE + 1);
	wmemset(m_ptzMsgErr, 0x00, LOG_MESG_SIZE * LAST_MESG_COUNT + LAST_MESG_COUNT);
}

ENG_EDIT CLogData::GetAppsType(ENG_EDIC apps)
{
	/* Ÿ�� �з� */
	switch (apps)
	{
		/* Service S/W (0x01 ~ 0x0f) */
	case ENG_EDIC::en_engine:			/* Engine App */
		return ENG_EDIT::en_log_type_service;
		break;
	
		/* Control S/W (0x10 ~ 0x2f) (16 ea) */
	case ENG_EDIC::en_podis:
	case ENG_EDIC::en_teldi:
	case ENG_EDIC::en_telbsa:
	case ENG_EDIC::en_uvdi15:
		return ENG_EDIT::en_log_type_control;
		break;
	
		// motion
	case ENG_EDIC::en_luria:
	case ENG_EDIC::en_mc2:
		return ENG_EDIT::en_log_type_motion;
		break;
	
		/* Library S/W (0x50 ~ 0x8f) - DI Engine */
	case ENG_EDIC::en_melsec:
	case ENG_EDIC::en_efem:
	case ENG_EDIC::en_pm100d:
	case ENG_EDIC::en_litho:
	case ENG_EDIC::en_efu_ss:
	case ENG_EDIC::en_bsa:
	case ENG_EDIC::en_gentec:
	case ENG_EDIC::en_mvenc:
	
	case ENG_EDIC::en_milara:
	case ENG_EDIC::en_lspa:
	case ENG_EDIC::en_lsdr:
		return ENG_EDIT::en_log_type_library;
		break;
	
		// vision
	case ENG_EDIC::en_ids_cam:
	case ENG_EDIC::en_trig:
	case ENG_EDIC::en_mil:
	case ENG_EDIC::en_ids:
	case ENG_EDIC::en_basler:
		return ENG_EDIT::en_log_type_vision;
		break;
	
	case ENG_EDIC::en_philhmi:
		return ENG_EDIT::en_log_type_philhmi;
		break;
	
	
	
		/* Library S/W (0x90 ~ 0x9f) - Not DI Engine */
	case ENG_EDIC::en_pgfs:
	case ENG_EDIC::en_gfsc:
	case ENG_EDIC::en_gfss:
		return ENG_EDIT::en_log_type_pgfs;
		break;
	
	
		/* Calibration Tool S/W (0xa0 ~ 0xaf) */
	case ENG_EDIC::en_recipe:
	case ENG_EDIC::en_acam_cali:
	case ENG_EDIC::en_led_power_cali:
	case ENG_EDIC::en_ph_step_cali:
	case ENG_EDIC::en_trig_cali:
	case ENG_EDIC::en_acam_focus:
	case ENG_EDIC::en_pcam_focus:
	case ENG_EDIC::en_icam_focus:
	case ENG_EDIC::en_pled_illum:
	case ENG_EDIC::en_stage_st:
	case ENG_EDIC::en_measure_flatness:
	case ENG_EDIC::en_2d_cali:
		return ENG_EDIT::en_log_type_calibration;
		break;
	
		/* Testing (0xf0 ~ 0xff) */
	case ENG_EDIC::en_debug:
	case ENG_EDIC::en_demo:
		return ENG_EDIT::en_log_type_etc;
		break;
	
	}

	return ENG_EDIT::en_log_type_etc;
}

/*
 desc : �α� ���� ����
 parm : apps	- [in]  Application Code (ENG_EDIC)
		level	- [in]  �޽��� ���� (normal, warning, error)
		mesg	- [in]  ��Ÿ �߻��� ���� ����� ����
		file	- [in]  �޽����� �߻��� �ҽ� �ڵ� ���ϸ� (��ü ��ΰ� ���Ե� ���� ... )
		func	- [in]  �޽����� �߻��� �ҽ� �ڵ� �Լ���
		line	- [in]  �޽����� �߻��� �ҽ� �ڵ� ��ġ (Line Number)
 retn : None
*/
VOID CLogData::SaveLogs(ENG_EDIC apps, ENG_LNWE level, PTCHAR mesg,
						PTCHAR file, PTCHAR func, UINT32 line)
{
	INT32 i32Size		= (INT32)wcslen(mesg);
	UINT64 u64Tick		= GetTickCount64();
	PTCHAR ptzFile		= NULL;
	PTCHAR ptzMsgBuff	= NULL, ptzReverse = NULL;
	SYSTEMTIME stTm		= {NULL};

	/* ������ �α� �޽����� ª�� �ð� ���� ���� �߻��ϸ� �������� ���� */
	if (u64Tick < m_u64LastTick + 3000)
	{
		/* ����� ������ �߻��� �α� �� ���� �޽��� �κ��� �����ϸ� �������� ���� */
		ptzMsgBuff	= m_ptzMsgLog + 25;
		if (0 == wcsncmp(ptzMsgBuff, mesg, i32Size))
		{
			m_u64LastTick	= u64Tick;	/* ���� �ֱٿ� �߻��� �α� �ð� ���� */
			return;	/* �������� ���� */
		}
	}
	m_u64LastTick	= u64Tick;	/* ���� �ֱٿ� �߻��� �α� ���� �ð� �ӽ� ���� */

	/* ���� Ȥ�� �αװ� �߻��� ��ü ��ΰ� ���Ե� ������ ���, ���ϸ� ���� */
	ptzFile	= uvCmn_GetFileName(file, (UINT32)wcslen(file));

	/* ���� �ð� ��� */
	GetLocalTime(&stTm);
	/* �α� ������ ���� (type �κ��� �ڵ� �յڷ� �ݵ�� '<' '>' �� �ѷ��׾ƾ� �մϴ�. (���ڿ� �˻� ������) */
	swprintf_s(m_ptzMsgLog, LOG_MESG_SIZE, L"[%02d:%02d:%02d] [0x%02x] <0x%02x> %s �� [%s:%s:%u]\n",
			   stTm.wHour, stTm.wMinute, stTm.wSecond, apps, level, mesg, ptzFile, func, line);

	/* �α� ���� ���� */
	if (m_bLogSaved && m_enSavedLevel >= level)
	{
		ENG_EDIT type = GetAppsType(apps);

		if (m_u8LogType)	SaveDB(UINT8(type), UINT8(apps), UINT8(level), mesg, ptzFile, func, line);
		else				SaveTxt(UINT8(type), m_ptzMsgLog);
	}

	/* �� ������ ���� ����� (Carriage Return) */
	i32Size	= (INT32)wcslen(m_ptzMsgLog);
	m_ptzMsgLog[i32Size-1]	= 0x00;

	/* �˶� (���� Ȥ�� ���) �޽����� ��� */
	if (0x10 == (UINT8(level) & 0x10))
	{
		/* ���� �ֱٿ� �߻��� �α� ���� �� ���� */
		m_enErrorLevelLast	= level;

		/* ���� �α� �����͸� ������ �ε��� �� ���� */
		if (0xff != m_u8LastError)
		{
			if (++m_u8LastError == LAST_MESG_COUNT)	m_u8LastError = 0x00;
		}
		else	m_u8LastError	= 0x00;

		/* �α� ���ڿ� �ڿ������� '��' ���� �˻� */
		ptzReverse = wcsrchr(m_ptzMsgLog, L'��');
		wmemset(m_ptzMsgPtr[m_u8LastError], 0x00, LOG_MESG_SIZE);
		/* ���� �߻��� �α� �����Ͱ� ����� ���� ��ġ ���� */
		if (ptzReverse)
		{
			i32Size = (INT32)(ptzReverse - m_ptzMsgLog) - 1;
			if (i32Size >= LOG_MESG_SIZE)	i32Size	= LOG_MESG_SIZE - 1;
			wmemcpy(m_ptzMsgPtr[m_u8LastError], m_ptzMsgLog+11, INT32(i32Size-11));
		}
		else
		{
			i32Size	= (INT32)wcslen(m_ptzMsgLog);
			wcscpy_s(m_ptzMsgPtr[m_u8LastError], LOG_MESG_SIZE, m_ptzMsgLog);
		}
		m_ptzMsgPtr[m_u8LastError][i32Size]	= 0x00;
		m_pLogType[m_u8LastError]	= (UINT8)level;
	}
}

/*
 desc : ���� �ֱٿ� �߻��� �α� �޽��� ��ȯ
 parm : None
 retn : �α� �޽���
*/
PTCHAR CLogData::GetErrorMesgLast()
{
	if (wcslen(m_ptzMsgLog) < 1)	return NULL;
	return m_ptzMsgLog;
}

/*
 desc : ������ ��ġ�� �ִ� ���� �α� �޽��� ��ȯ
 parm : index	- [in]  Ư�� ��ġ�� �ִ� ���� �α� ��ȯ (Zero-based)
						0x00: ���� �ֱٿ� �߻��� ���� �α� ��û
						[����] ���� Ŀ������ ������ ���� �α� ��û
 retn : �α� �޽���
		NULL�� ��ȯ�� ���, �� �̻� ���� �α� ����
*/
PTCHAR CLogData::GetErrorMesg(UINT8 index)
{
	UINT8 u8Index	= 0x00;
	if (0xff == m_u8LastError)	return NULL;

	/* ���� ��û�� ��ġ ���� ���۸� �ʰ� �ߴٸ� NULL ��ȯ */
	if (index >= LAST_MESG_COUNT)	return NULL;
	/* ���� ��û�� ��ġ �������� ���� �ֱ� ������ �߻��� ��ġ �缳�� */
	if ((INT16(m_u8LastError) - INT16(index)) >= 0)
	{
		u8Index	= m_u8LastError - index;
	}
	else
	{
		u8Index	= LAST_MESG_COUNT + (INT16(m_u8LastError) - INT16(index));
	}
	/* ���̻� �αװ� ���ٸ� NULL ��ȯ */
	if (wcslen(m_ptzMsgPtr[u8Index]) < 1)	return NULL;
	/* Index ���� 0�� ���, ���� �ֱٿ� �߻��� �α� ������ */
	return m_ptzMsgPtr[u8Index];
}

/*
 desc : �ֱٿ� �߻��� �޽��� �߿��� ���� �αװ� �־����� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLogData::IsErrorMesg()
{
	for (UINT8 i=0x00; i<LAST_MESG_COUNT && m_u8LastError != 0xff; i++)
	{
		if (m_pLogType[i] != 0x00 &&
			m_pLogType[i] != UINT8(ENG_LNWE::en_normal))	return TRUE;
	}
	return FALSE;
}