
#pragma once

#include "../../inc/comm/ClientThread.h"

class CGFSCThread : public CClientThread
{
// 생성자/파괴자
public:

	CGFSCThread(UINT16 th_id, LPG_CSID config);
	virtual ~CGFSCThread();

// 가상함수 재정의
protected:

	virtual BOOL		Connected();
	virtual VOID		DoWork();
	/* -------------------------------------------------------------------------- */
	/* 공통 통신 수신 모듈이 아니기 때문에, 별도로 가상함수를 통해 함수 별도 수행 */
	/* -------------------------------------------------------------------------- */
	virtual BOOL		RecvData();

// 로컬 구조체
protected:


// 로컬 변수
protected:

	BOOL				m_bValidAliveTime;	/* 정상적으로 Alive Time 받았는지 여부 */
	BOOL				m_bSendPktGFiles;	/* 거버 파일의 패킷 송신 여부 플래그 */

	UINT32				m_u32FileCount;		/* 가장 최근에 송신 혹은 수신받은(을) 거버 혹은 레시피 파일의 개수 */
	UINT32				m_u32TotalFiles;	/* 전체 송신될 파일 개수 */
	UINT32				m_u32TotalSizes;	/* 가장 최근에 송신 혹은 수신받은(을) 거버 혹은 레시피 파일의 총 크기 (비압축) */
	UINT32				m_u32AliveCount;	/* Alive Time 보낸 개수 (최대 값이 되면 1로 변경) */

	ENG_LNWE			m_enLastErrorLevel;	/* 가장 최근의 에러 정보 값 임시 저장 */

	PUINT8				m_pRecvBuff;		/* 패킷 전체 부분 */
	LPG_CSID			m_pstConfig;		/* 전체 환경 파일 */

	CAtlList <STG_PGFP> m_lstPktGFiles;		/* 거버 임시 파일 */

	CMySection			m_syncPktCtrl;		/* 데이터 송수신 제어 */
	CMySection			m_syncPktCmd;		/* 최근에 수신된 명령어 임시 저장 동기화 객체 */

	CAtlList<STG_UTPH>	m_lstPktCmd;		/* 가장 최근에 송/수신된 명령어 임시 저장 버퍼 */


// 로컬 함수
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


// 공용 함수
public:

	ENG_TPCS			GetTCPMode()		{	return m_enSocket;			}
	ENG_LNWE			GetLastErrorLevel()	{	return m_enLastErrorLevel;	}

	VOID				StopGerberSendFile();
	VOID				ResetPktGfiles();
	VOID				AddPktGFiles(STG_PGFP gfile);

	BOOL				IsSendGFileCompleted();
	DOUBLE				GetSendGFilesRate();
	/* 전체 송신 혹은 수신된 파일 크기 값 반환 */
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