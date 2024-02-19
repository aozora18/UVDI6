
#pragma once

#include "../conf/event.h"
#include "MySection.h"

#define MAX_EVENT_SIZE	(1024 * 1024 * 10)	/* Max 10 MBytes */

class CMyEvent
{

// 생성자 & 파괴자
public:

	/*
	 desc : 생성자
	 parm : count	- [in]  이벤트 최대 등록 개수
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
	 desc : 파괴자
	 parm : None
	 retn : None
	*/
	virtual ~CMyEvent()
	{
		RemoveAllEventData();
		/* 로그 파일 닫기 */
		if (m_bSaveFile)	CloseFile();
	}


// 로컬 변수
protected:

	CHAR				m_szLogPath[MAX_PATH_LEN];

	BOOL				m_bSaveFile;	// 로그 데이터를 파일로 저장 유무

	UINT32				m_u32MaxCount;

	FILE				*m_fpEvent;	// Event Data (구조체 STG_EDRC)를 저장할 파일 포인터

	CAtlList <STG_ESTM>	m_lstEvent;

	CMySection			m_syncEvent;


// 로컬 함수
protected:

	/*
	 desc : 새로운 로그 파일 이름 (전체 경로 포함) 반환
	 parm : file	- [out] 전체 경로가 포함된 파일 이름 반환
			size	- [in]  'file' 버퍼의 크기
	 retn : None
	*/
	VOID GetNewFile(CHAR *file, UINT32 size)
	{
		SYSTEMTIME stTm	= {NULL};
		CUniToChar csCnv;

		// 현재 컴퓨터의 로컬 시간 얻기
		GetLocalTime(&stTm);
		// 로그 or 성능 파일명 설정
		sprintf_s(file, size, "%s\\%04d-%02d-%02d %02d.%02d.%02d.log",
							   m_szLogPath, stTm.wYear, stTm.wMonth, stTm.wDay,
							   stTm.wHour, stTm.wMinute, stTm.wSecond);
	}

	/*
	 desc : 기존 파일을 닫기
	 parm : None
	 retn : None
	*/
	VOID CloseFile()
	{
		if (m_fpEvent)	fclose(m_fpEvent);
		m_fpEvent	= NULL;
	}

	/*
	 desc : 새로운 파일을 열기
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL OpenFile()
	{
		CHAR szFile[MAX_PATH_LEN]	= {NULL};

		// 기존 파일 닫고
		CloseFile();
		// 새로운 파일명 (전체 경로 포함) 얻기
		GetNewFile(szFile, MAX_PATH_LEN);
		// 새로운 파일 열기
		if (!(m_fpEvent = _fsopen(szFile, "wb", _SH_DENYWR)))	return FALSE;

		return TRUE;
	}

	/*
	 desc : 이벤트 로그 데이터를 파일에 저장
	 parm : event	- [in]  이벤트 정보
	 retn : TRUE or FALSE
	*/
	BOOL SaveEventData(LPG_ESTM event)
	{
		UINT32 u32Size	= sizeof(STG_ESTM);

		// 기존 파일에 닫혀 있거나, 일정 크기 이상 (10 MB)으로 저장되어 있다면, 새로운 파일을 열도록 한다.
		if (!m_fpEvent || ftell(m_fpEvent) > MAX_EVENT_SIZE)
		{
			if (!OpenFile())	return FALSE;;
		}
		// 현재 저장되고 있는 파일의 크기가 크면, 새로운 파일을 생성

		// 이벤트 로그를 파일로 저장
		if (1 != fwrite(event, u32Size, 1, m_fpEvent))
		{
			// 이벤트 로그 파일 저장 실패 했을 경우
			CloseFile();
			return FALSE;
		}

		return TRUE;
	}

// 공용 함수
public:

	/*
	 desc : 현재 등록된 개수 반환
	 parm : None
	 retn : 등록된 개수 값
	*/
	UINT32 GetCount()
	{
		return (UINT32)m_lstEvent.GetCount();
	}

	/*
	 desc : 로그 저장
	 parm : path	- [in]  로그가 저장될 경로
	 retn : None
	*/
	VOID SetLogPath(TCHAR *path)
	{
		CUniToChar csCnv;

		sprintf_s(m_szLogPath, MAX_PATH_LEN, "%s", csCnv.Uni2Ansi(path));
		m_bSaveFile	= TRUE;
	}

	/*
	 desc : 실시간 발생되는 로그 이벤트 등록
	 parm : event	- [in]  Event 정보
			mesg	- [in]  정보 메시지
	 retn : TRUE or FALSE
	*/
	BOOL PushData(LPG_ESCV event, CHAR *mesg)
	{
		size_t s32MsgLen	= 0;
		STG_ESTM stEvent	= {NULL};
		SYSTEMTIME stTm		= {NULL};

		/* 기존에 등록된 이벤트 개수가 넘 많으면 더 이상 등록하지 않음 */
		if (m_lstEvent.GetCount() > m_u32MaxCount)
		{
#ifdef _DEBUG
			CUniToChar csCnv;
			// 이벤트 코드에 대한 문자열 출력
			TRACE(L"del_event_data : %02x-%02x-%02x-%s\n",
				  stEvent.event.apps,
				  stEvent.event.indx,
				  stEvent.event.kind,
				  csCnv.Ansi2Uni(stEvent.mesg));
#endif
			return TRUE;
		}
		// 동기 진행
		if (m_syncEvent.Enter())
		{
			// 이벤트 데이터 복사
			s32MsgLen	= strlen(mesg);	// 128 Bytes 넘지 않기 위함
			if (s32MsgLen > 127)	s32MsgLen	= 127;
			memcpy(&stEvent.event, event, sizeof(STG_ESCV));
			memcpy(stEvent.mesg, mesg, s32MsgLen);
			// 현재 시스템의 로컬 시간 얻기
			GetLocalTime(&stTm);
			// 시간 정보 설정
			stEvent.time.yy	= stTm.wYear - 2000;
			stEvent.time.mm	= stTm.wMonth;
			stEvent.time.dd	= stTm.wDay;
			stEvent.time.hh	= stTm.wHour;
			stEvent.time.mn	= stTm.wMinute;
			stEvent.time.ss	= stTm.wSecond;

			/* ------------------------------------------- */
			/* 디버그 모드일 경우, 무조건 로그 메시지 출력 */
			/* 메모리에 등록하기 전 단계에서 반드시 호출함 */
			/* ------------------------------------------- */
#ifdef _DEBUG
			CUniToChar csCnv;
			// 이벤트 코드에 대한 문자열 출력
			TRACE(L"add_event_data : %02x-%02x-%02x-%s\n",
				  stEvent.event.apps,
				  stEvent.event.indx,
				  stEvent.event.kind,
				  csCnv.Ansi2Uni(stEvent.mesg));
#endif
			// 로그 파일 저장 여부 확인
			if (m_bSaveFile)	SaveEventData(&stEvent);
			/* ------------------------- */
			/* 이벤트 (로그) 데이터 등록 */
			/* ------------------------- */
			m_lstEvent.AddTail(stEvent);
		
			// 동기 해제
			m_syncEvent.Leave();
		}

		return TRUE;
	}

	/*
	 desc : 실시간 발생되는 이벤트 데이터 추출
	 parm : event	- [out] 이벤트 정보 반환
	 note : TRUE or FALSE
	*/
	BOOL PopData(STG_ESTM &event)
	{
		BOOL bSucc	= FALSE;

		// 동기 진입
		if (m_syncEvent.Enter())
		{
			// Event Data 메모리 추출
			if (m_lstEvent.GetCount() > 0)
			{
				event	= m_lstEvent.GetHead();
				m_lstEvent.RemoveHead();
				bSucc	= TRUE;
			}

			// 동기 해제
			m_syncEvent.Leave();
		}		

		return bSucc;
	}

	/*
	 desc : 가장 마지막 발생된 에러 값 반환
	 parm : None
	 retn : Error 값 반환
	*/
	UINT32 GetError()
	{
		return GetLastError();
	}

	/*
	 desc : 현재까지 발생된 모든 이벤트 초기화
	 parm : None
	 retn : None
	*/
	VOID RemoveAllEventData()
	{
		POSITION pPos;
		/* 동기 진입 */
		if (m_syncEvent.Enter())
		{
			m_lstEvent.RemoveAll();

			/* 동기 해제 */
			m_syncEvent.Leave();
		}
	}

};
