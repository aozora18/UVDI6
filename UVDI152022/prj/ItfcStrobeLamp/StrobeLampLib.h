
#pragma once

#include "../../inc/comm/ClientThread.h"
#include "./Socket/SocketHandle.h"
#include "./Socket/SocketClientImpl.h"
#include "./Mcl/Include/CMcl.h"
#include <queue>
#include <list>



#define DEF_COUNT_OF_MAX_EVENT		12
#define DEF_COUNT_OF_MAX_RECVDATA	40




typedef SocketClientImpl<ISocketClientHandler> CSocketClient;

class CStrobeLampLib : public ISocketClientHandler, CMclThreadHandler
{
	/* 가상함수 */
	friend CSocketClient;
	virtual void		OnDataReceived(CSocketHandle* pSH, const BYTE* pbData, DWORD dwCount, const SockAddrIn& addr);
	virtual void		OnThreadBegin(CSocketHandle* pSH);
	virtual void		OnThreadExit(CSocketHandle* pSH);
	virtual void		OnConnectionDropped(CSocketHandle* pSH);
	virtual void		OnConnectionError(CSocketHandle* pSH, DWORD dwError);

public:
	CStrobeLampLib();
	virtual ~CStrobeLampLib(void);

	/* 로컬 구조체 */
protected:


	/* 로컬 변수 */
protected:
	UINT64				m_u64RecvTime;			/* 데이터 수신된 시간 저장 */
	UINT64				m_u64SendTime;			/* 짧은 시간 내에 많은 데이터를 송신하지 못하도록 하기 위함 */

	CMySection			m_syncSend;				/* 송신 동기화 객체 */
	CMySection			m_syncRecv;
	CMySection			m_syncResult;

	CSocketClient		m_SocketClient;

	CString				m_str_local_ip;
	CString				m_str_svr_ip;
	int					m_nPortNum;

	std::vector<int>				m_vReplyMsg;
	std::vector<BYTE>				m_vRecvData;
	std::queue <STG_SLPR_RECV>		m_qRecvStruct;
	std::list <STG_SLPR_RECV>		m_lResultStruct;
	CMclEventAutoPtr	m_evt_comm_wait[DEF_COUNT_OF_MAX_EVENT];
	HWND				m_hMainWnd;

	unsigned int		m_uiDataPos;
	unsigned int		m_uiDataLength;
	unsigned int		m_nUniqueID;

	CMclThreadAutoPtr	m_apNetMonitorThread;
	volatile BOOL		m_bAutoReconnection;
	volatile BOOL		m_bRun;

	STG_SLPR_RECV		m_stRecvData;

	LPG_CTCS			m_pComm;
	LPG_CCGS			m_pComn;

	BOOL				m_bProve;

	/* 로컬 함수 */
protected:
	unsigned			ThreadHandlerProc();
	int					GetUniqueID();
	BOOL				SendData(STG_SLPR_SEND& stSend, int nSize, int nTimeout = 10000);
	BOOL				WaitReply(int nIndex, int nTimeout);
	BOOL				RecvData(STG_SLPR_RECV& stRecv);
	void				AddPktData(STG_SLPR_RECV& stData);
	void				PopPktData();

	uint8_t				CalculateChecksum(const stStrobeLampMessage& msg, size_t length);		//	범위 내 X-OR 연산하기
	void				finalizeMessage(stStrobeLampMessage& msg);								//	메세지 체크섬 얻기


	ERR_CODE_STROBE		IsValidPage(uint8_t page);
	ERR_CODE_STROBE		IsValidChannel(uint8_t channel);
	ERR_CODE_STROBE		IsValidDelayValue(uint16_t delayValue);
	ERR_CODE_STROBE		IsValidStrobeValue(uint16_t strobeValue);
	ERR_CODE_STROBE		IsValidStrobeMode(uint16_t strobeMode);

	/* 프로토콜 함수 */
public:
	BOOL				Init(HWND hwnd, LPG_CTCS pComm, LPG_CCGS pComn);
	BOOL				Open();
	BOOL				Open(LPCTSTR Ip, int nPort);
	BOOL				Open(LPCTSTR LocalIp, LPCTSTR Ip, int nPort);
	BOOL				IsConnect();
	BOOL				Close();




	int Send_ChannelDelayControl(const uint8_t page, const uint8_t channel, const uint16_t delayValue, int nTimeout = 10000);
	int Send_ChannelStrobeControl(const uint8_t page, const uint8_t channel, const uint16_t strobeValue, int nTimeout = 10000);
	int Send_ChannelWrite(const uint8_t page, const uint8_t channel, const uint16_t delayValue, const uint16_t strobeValue, int nTimeout = 10000);
	int Send_PageDataWrite(const uint8_t page, const uint8_t channelCount, const uint16_t* delayValues, const uint16_t* strobeValues, int nTimeout = 10000);
	int Send_PageDataReadRequest(const uint8_t page, int nTimeout = 10000);
	int Send_PageLoopDataWrite(const uint8_t page, const uint8_t loopDelay3Value, const uint8_t loopDelay2Value, const uint8_t loopDelay1Value, const uint8_t loopDelay0Value, const uint8_t loopCount, int nTimeout = 10000);
	int Send_PageLoopDataRead(const uint8_t page, int nTimeout = 10000);
	int Send_StrobeMode(const uint8_t mode, int nTimeout = 10000);
	int Send_TriggerMode(const uint8_t mode, int nTimeout = 10000);
	int Send_DataSave(int nTimeout = 10000);
	int Send_DataLoad(int nTimeout = 10000);
	int Send_StrobeTriggerModeRead(int nTimeout = 10000);








protected:
	/* 직렬화 */
	std::vector<BYTE> SerializeMassege(const stStrobeLampMessage& msg)
	{
		std::vector<BYTE> data(sizeof(stStrobeLampMessage));

		std::memcpy(data.data(), reinterpret_cast<const BYTE*>(&msg), msg.GetSize());	//sizeof(stStrobeLampMessage);

		return data;
	}

	/* 직렬화 : 동적 메모리 할당 */
	BYTE* SerializeMessage(const stStrobeLampMessage& msg)
	{
		//	동적 메모리 할당(new)을 사용하였으므로, 개발자가 명시적으로 메모리를 해제(delete)해야 합니다.

		size_t size = msg.GetSize();	//sizeof(stStrobeLampMessage);

		BYTE* serializedData = new BYTE[size];

		std::memcpy(serializedData, &msg, size);

		return serializedData;
	}
};
