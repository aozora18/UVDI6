
/*
 desc : Log Data 처리 (저장 및 관리)
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
 desc : 생성자
 parm : saved	- [in]  로그 파일 저장 여부
		type	- [in]  Log Type (0 : text, 1 : DB (SQLite3)
		mode	- [in]  로그 저장 기준 (None, All, Warning and Error, Only Error)
 retn : None
*/
CLogData::CLogData(BOOL saved, UINT8 type, ENG_LNWE mode)
	: CBase(saved, type, mode)
{
	UINT8 i	= 0x00;
	PTCHAR ptzMsgErr	= NULL;
	/* 가장 최근에 저장된 로그 발생 시간 */
	m_u64LastTick	= 0;
	m_u8LastError	= 0xff;

	/* 메시지 버퍼 임시 할당 */
	m_ptzMsgLog	= (PTCHAR)::Alloc(LOG_MESG_SIZE * sizeof(TCHAR) + 1);
	m_ptzMsgErr	= (PTCHAR)::Alloc(LOG_MESG_SIZE * sizeof(TCHAR) * LAST_MESG_COUNT + sizeof(TCHAR) * LAST_MESG_COUNT /* 종결 문자가 들어갈 버퍼 개수 */);
	m_ptzMsgPtr	= (PTCHAR*)::Alloc(LAST_MESG_COUNT * sizeof(PTCHAR));
	m_pLogType	= (PUINT8)::Alloc(LAST_MESG_COUNT);
	ASSERT(m_ptzMsgLog && m_ptzMsgErr && m_ptzMsgPtr && m_pLogType);
	ResetErrorMesg();

	/* 1차원 배열을 2차원 배열처럼 사용하기 위함 */
	ptzMsgErr	= m_ptzMsgErr;
	for (; i<LAST_MESG_COUNT; i++)
	{
		m_ptzMsgPtr[i]	= ptzMsgErr;
		ptzMsgErr		+= (LOG_MESG_SIZE + 1);
	}
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CLogData::~CLogData()
{
	/* 메시지 버퍼 임시 해제 */
	if (m_pLogType)		::Free(m_pLogType);
	if (m_ptzMsgLog)	::Free(m_ptzMsgLog);
	if (m_ptzMsgErr)	::Free(m_ptzMsgErr);
	if (m_ptzMsgPtr)	::Free(m_ptzMsgPtr);
}

/*
 desc : 가장 최근에 발생된 로그 데이터들 모두 제거
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
	/* 타입 분류 */
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
 desc : 로그 파일 저장
 parm : apps	- [in]  Application Code (ENG_EDIC)
		level	- [in]  메시지 성격 (normal, warning, error)
		mesg	- [in]  기타 발생된 값이 저장된 버퍼
		file	- [in]  메시지가 발생된 소스 코드 파일명 (전체 경로가 포함된 파일 ... )
		func	- [in]  메시지가 발생된 소스 코드 함수명
		line	- [in]  메시지가 발생된 소스 코드 위치 (Line Number)
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

	/* 동일한 로그 메시지로 짧은 시간 내에 많이 발생하면 저장하지 않음 */
	if (u64Tick < m_u64LastTick + 3000)
	{
		/* 현재와 이전에 발생된 로그 중 본문 메시지 부분이 동일하면 저장하지 않음 */
		ptzMsgBuff	= m_ptzMsgLog + 25;
		if (0 == wcsncmp(ptzMsgBuff, mesg, i32Size))
		{
			m_u64LastTick	= u64Tick;	/* 가장 최근에 발생된 로그 시간 저장 */
			return;	/* 저장하지 않음 */
		}
	}
	m_u64LastTick	= u64Tick;	/* 가장 최근에 발생된 로그 저장 시간 임시 저장 */

	/* 에러 혹은 로그가 발생된 전체 경로가 포함된 파일의 경우, 파일만 추출 */
	ptzFile	= uvCmn_GetFileName(file, (UINT32)wcslen(file));

	/* 현재 시간 얻기 */
	GetLocalTime(&stTm);
	/* 로그 데이터 설정 (type 부분의 코드 앞뒤로 반드시 '<' '>' 로 둘러쌓아야 합니다. (문자열 검색 때문에) */
	swprintf_s(m_ptzMsgLog, LOG_MESG_SIZE, L"[%02d:%02d:%02d] [0x%02x] <0x%02x> %s → [%s:%s:%u]\n",
			   stTm.wHour, stTm.wMinute, stTm.wSecond, apps, level, mesg, ptzFile, func, line);

	/* 로그 파일 저장 */
	if (m_bLogSaved && m_enSavedLevel >= level)
	{
		ENG_EDIT type = GetAppsType(apps);

		if (m_u8LogType)	SaveDB(UINT8(type), UINT8(apps), UINT8(level), mesg, ptzFile, func, line);
		else				SaveTxt(UINT8(type), m_ptzMsgLog);
	}

	/* 맨 마지막 글자 지우기 (Carriage Return) */
	i32Size	= (INT32)wcslen(m_ptzMsgLog);
	m_ptzMsgLog[i32Size-1]	= 0x00;

	/* 알람 (에러 혹은 경고) 메시지인 경우 */
	if (0x10 == (UINT8(level) & 0x10))
	{
		/* 가장 최근에 발생된 로그 레벨 값 저장 */
		m_enErrorLevelLast	= level;

		/* 현재 로그 데이터를 저장할 인덱스 값 설정 */
		if (0xff != m_u8LastError)
		{
			if (++m_u8LastError == LAST_MESG_COUNT)	m_u8LastError = 0x00;
		}
		else	m_u8LastError	= 0x00;

		/* 로그 문자열 뒤에서부터 '→' 문자 검색 */
		ptzReverse = wcsrchr(m_ptzMsgLog, L'→');
		wmemset(m_ptzMsgPtr[m_u8LastError], 0x00, LOG_MESG_SIZE);
		/* 현재 발생된 로그 데이터가 저장될 버퍼 위치 설정 */
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
 desc : 가장 최근에 발생된 로그 메시지 반환
 parm : None
 retn : 로그 메시지
*/
PTCHAR CLogData::GetErrorMesgLast()
{
	if (wcslen(m_ptzMsgLog) < 1)	return NULL;
	return m_ptzMsgLog;
}

/*
 desc : 임의의 위치에 있는 에러 로그 메시지 반환
 parm : index	- [in]  특정 위치에 있는 에러 로그 반환 (Zero-based)
						0x00: 가장 최근에 발생된 에러 로그 요청
						[참고] 값이 커질수록 오래된 에러 로그 요청
 retn : 로그 메시지
		NULL이 반환된 경우, 더 이상 에러 로그 없음
*/
PTCHAR CLogData::GetErrorMesg(UINT8 index)
{
	UINT8 u8Index	= 0x00;
	if (0xff == m_u8LastError)	return NULL;

	/* 만약 요청한 위치 값이 버퍼를 초과 했다면 NULL 반환 */
	if (index >= LAST_MESG_COUNT)	return NULL;
	/* 현재 요청된 위치 기준으로 가장 최근 순으로 발생된 위치 재설정 */
	if ((INT16(m_u8LastError) - INT16(index)) >= 0)
	{
		u8Index	= m_u8LastError - index;
	}
	else
	{
		u8Index	= LAST_MESG_COUNT + (INT16(m_u8LastError) - INT16(index));
	}
	/* 더이상 로그가 없다면 NULL 반환 */
	if (wcslen(m_ptzMsgPtr[u8Index]) < 1)	return NULL;
	/* Index 값이 0인 경우, 가장 최근에 발생된 로그 데이터 */
	return m_ptzMsgPtr[u8Index];
}

/*
 desc : 최근에 발생된 메시지 중에서 에러 로그가 있었는지 여부
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