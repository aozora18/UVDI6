
#pragma once

#include "../../inc/comm/ServerThread.h"


class CGFSSThread : public CServerThread
{
// 생성자/파괴자
public:

	CGFSSThread(UINT16 th_id, LPG_CSID config);
	virtual ~CGFSSThread();

// 가상함수 재정의
protected:

	virtual BOOL		Accepted();
	virtual VOID		DoWork();
	/* -------------------------------------------------------------------------- */
	/* 공통 통신 수신 모듈이 아니기 때문에, 별도로 가상함수를 통해 함수 별도 수행 */
	/* -------------------------------------------------------------------------- */
	virtual BOOL		RecvData();

// 로컬 구조체
protected:


// 로컬 변수
protected:

	BOOL				m_bIsRecvCompleted;

	TCHAR				m_tzLastNewDir[MAX_PATH_LEN];	/* 가장 최근에 생성된 폴더 임시 저장 */
	TCHAR				m_tzLastDelDir[MAX_PATH_LEN];	/* 가장 최근에 삭제된 폴더 임시 저장 */
	TCHAR				m_tzLastSaveDir[MAX_PATH_LEN];	/* 가장 최근에 저장된 폴더 임시 저장 (거버 관련 파일) */

	UINT32				m_u32TotalFiles;		/* 가장 최근에 송신 혹은 수신받은(을) 거버 혹은 레시피 파일의 개수 */
	UINT32				m_u32FileCount;			/* 가장 최근에 송신 혹은 수신받은(을) 거버 혹은 레시피 파일의 개수 */
	UINT32				m_u32TotalSizes;		/* 가장 최근에 송신 혹은 수신받은(을) 거버 혹은 레시피 파일의 총 크기 (비압축) */
	UINT32				m_u32TotalZipSize;		/* 가장 최근에 송신 혹은 수신받은(을) 거버 혹은 레시피 파일의 총 크기 (압축) */
	UINT64				m_u64RecvStartTime;		/* 거버 파일 전송 시작 시간 (단위: 밀리초) */
	UINT64				m_u64RecvTotalTime;		/* 거버 파일 모두 받는데 소요된 시간 (단위: 밀리초) */

	UINT64				m_u64PeriodLiveTime;	/* 주기적으로 송신하는 간격 */
	UINT64				m_u64LastLiveTime;		/* 가장 최근에 송신한 시간 */

	ENG_LNWE			m_enLastErrorLevel;		/* 가장 최근의 에러 정보 값 임시 저장 */

	PUINT8				m_pRecvBuff;			/* 패킷 전체 부분 */
	LPG_CSID			m_pstConfig;			/* 전체 환경 파일 */

	CMySection			m_syncReadWrite;		/* 데이터 송수신 */
	CMySection			m_syncPktCmd;			/* 최근에 수신된 명령어 임시 저장 동기화 객체 */

	CAtlList<STG_UTPH>	m_lstPktCmd;			/* 가장 최근에 수신된 명령어 임시 저장 버퍼 */

// 로컬 함수
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

// 공용 함수
public:

	ENG_TPCS			GetTCPMode()		{	return m_enSocket;			}
	ENG_LNWE			GetLastErrorLevel()	{	return m_enLastErrorLevel;	}

	BOOL				PopPktHead(STG_UTPH &head);

	UINT32				GetCurrentFileCount()	{	return m_u32FileCount;		}
	UINT32				GetTotalFileCount()		{	return m_u32TotalFiles;		}
	UINT32				GetTotalFileSize()		{	return m_u32TotalSizes;		}
	UINT32				GetTotalFileZipSize()	{	return m_u32TotalZipSize;	}
	DOUBLE				GetTotalFilesRate();

	/* 가장 최근에 생성 / 삭제 / 저장 (거버 관련 파일들) 된 폴더 반환 */
	PTCHAR				GetLastDir(UINT8 type);

	/* 가장 최근에 수신받는데 소요된 시간 반환 (단위: msec) */
	UINT64				GetRecvTotalTime()	{	return m_u64RecvTotalTime;	};
	BOOL				IsRecvCompleted()	{	return m_bIsRecvCompleted;	};
};