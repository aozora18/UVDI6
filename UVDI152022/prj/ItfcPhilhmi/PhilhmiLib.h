
#pragma once

#include "../../inc/comm/ClientThread.h"
#include "./Socket/SocketHandle.h"
#include "./Socket/SocketClientImpl.h"
#include "./Mcl/Include/CMcl.h"
#include <queue>
#include <list>
#define DEF_COUNT_OF_MAX_EVENT		24
#define DEF_COUNT_OF_MAX_RECVDATA	20

typedef SocketClientImpl<ISocketClientHandler> CSocketClient;

class CPhilhmiLib : public ISocketClientHandler, CMclThreadHandler
{
	/* 가상함수 */
	friend CSocketClient;
	virtual void		OnDataReceived(CSocketHandle* pSH, const BYTE* pbData, DWORD dwCount, const SockAddrIn& addr);
	virtual void		OnThreadBegin(CSocketHandle* pSH);
	virtual void		OnThreadExit(CSocketHandle* pSH);
	virtual void		OnConnectionDropped(CSocketHandle* pSH);
	virtual void		OnConnectionError(CSocketHandle* pSH, DWORD dwError);

public:
	CPhilhmiLib();
	virtual ~CPhilhmiLib(void);

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

	std::vector<int>					m_vReplyMsg;
	std::vector<BYTE>					m_vRecvData;
	std::queue <STG_PP_PACKET_RECV>		m_qRecvStruct;
	std::list <STG_PP_PACKET_RECV>		m_lResultStruct;
	CMclEventAutoPtr	m_evt_comm_wait[DEF_COUNT_OF_MAX_EVENT];
	HWND				m_hMainWnd;

	unsigned int		m_uiDataPos;
	unsigned int		m_uiDataLength;
	unsigned int		m_nUniqueID;

	CMclThreadAutoPtr	m_apNetMonitorThread;
	volatile BOOL		m_bAutoReconnection;
	volatile BOOL		m_bRun;

	STG_PP_PACKET_RECV	m_stRecvData = { 0, };

	LPG_CTCS			m_pComm;
	LPG_CCGS			m_pComn;

	/* 로컬 함수 */
protected:
	unsigned			ThreadHandlerProc();
	int					GetUniqueID();
	BOOL				SendData(STG_PP_PACKET_SEND &stSend, int nSize, int nTimeout = 10000);
	BOOL				WaitReply(int nIndex, int nTimeout);
	BOOL				RecvData(STG_PP_PACKET_RECV& stRecv);
	void				AddPktData(STG_PP_PACKET_RECV& stData);
	void				PopPktData();
	/* 프로토콜 함수 */
public:
	BOOL				Init(HWND hwnd, LPG_CTCS pComm, LPG_CCGS pComn);
	BOOL				Open();
	BOOL				Open(LPCTSTR Ip, int nPort);
	BOOL				Open(LPCTSTR LocalIp, LPCTSTR Ip, int nPort);
	BOOL				IsConnect();
	BOOL				Close();
	BOOL				GetRecvDataFromCommand(int nCommand, STG_PP_PACKET_RECV* pstRecv);
	BOOL				GetRecvDataFromUniqueID(unsigned int nUniqueID, STG_PP_PACKET_RECV* pstRecv);
	BOOL				GetRecvWaitFromUniqueID(unsigned int nUniqueID, STG_PP_PACKET_RECV* pstRecv, int nTimeout = 10000);
	BOOL				DeleteRecvDataFromUniqueID(unsigned int nUniqueID);
	BOOL				IsSyncResultLocked() {return m_syncResult.IsLockCount() != 0;}
	
	int Send_P2C_RCP_CREATE(STG_PP_P2C_RCP_CREATE& stSend, STG_PP_P2C_RCP_CREATE_ACK& stRecv, int nTimeout = 10000);
	int Send_P2C_RCP_DELETE(STG_PP_P2C_RCP_DELETE& stSend, STG_PP_P2C_RCP_DELETE_ACK& stRecv, int nTimeout = 10000);
	int Send_P2C_RCP_MODIFY(STG_PP_P2C_RCP_MODIFY& stSend, STG_PP_P2C_RCP_MODIFY_ACK& stRecv, int nTimeout = 10000);
	int Send_P2C_RCP_SELECT(STG_PP_P2C_RCP_SELECT& stSend, STG_PP_P2C_RCP_SELECT_ACK& stRecv, int nTimeout = 10000);
	int Send_P2C_IO_STATUS(STG_PP_P2C_IO_STATUS& stSend, STG_PP_P2C_IO_STATUS_ACK& stRecv, int nTimeout = 10000);
	int Send_P2C_OUTPUT_ONOFF(STG_PP_P2C_OUTPUT_ONOFF& stSend, STG_PP_P2C_OUTPUT_ONOFF_ACK& stRecv, int nTimeout = 10000);
	int Send_P2C_ABS_MOVE(STG_PP_P2C_ABS_MOVE& stSend, STG_PP_P2C_ABS_MOVE_ACK& stRecv, int nTimeout = 10000);
	int Send_P2C_ABS_MOVE_COMP(STG_PP_P2C_ABS_MOVE_COMP& stSend, STG_PP_P2C_ABS_MOVE_COMP_ACK& stRecv, int nTimeout = 10000);
	int Send_P2C_REL_MOVE(STG_PP_P2C_REL_MOVE& stSend, STG_PP_P2C_REL_MOVE_ACK& stRecv, int nTimeout = 10000);
	int Send_P2C_REL_MOVE_COMP(STG_PP_P2C_REL_MOVE_COMP& stSend, STG_PP_P2C_REL_MOVE_COMP_ACK& stRecv, int nTimeout = 10000);
	int Send_P2C_CHAR_MOVE(STG_PP_P2C_CHAR_MOVE& stSend, STG_PP_P2C_CHAR_MOVE_ACK& stRecv, int nTimeout = 10000);
	int Send_P2C_CHAR_MOVE_COMP(STG_PP_P2C_CHAR_MOVE_COMP& stSend, STG_PP_P2C_CHAR_MOVE_COMP_ACK& stRecv, int nTimeout = 10000);
	int Send_P2C_PROCESS_STEP(STG_PP_P2C_PROCESS_STEP& stSend, int nTimeout = 10000);
	int Send_P2C_PROCESS_COMP(STG_PP_P2C_PROCESS_COMP& stSend, STG_PP_P2C_PROCESS_COMP_ACK& stRecv, int nTimeout = 10000);
	int Send_P2C_SUBPROCESS_COMP(STG_PP_P2C_SUBPROCESS_COMP& stSend, STG_PP_P2C_SUBPROCESS_COMP_ACK& stRecv, int nTimeout = 10000);
	int Send_P2C_EC_MODIFY(STG_PP_P2C_EC_MODIFY& stSend, STG_PP_P2C_EC_MODIFY_ACK& stRecv, int nTimeout = 10000);
	int Send_P2C_INITIAL_COMPLETE(STG_PP_P2C_INITIAL_COMPLETE& stSend, STG_PP_P2C_INITIAL_COMPLETE_ACK& stRecv, int nTimeout = 10000);
	int Send_P2C_ALARM_OCCUR(STG_PP_P2C_ALARM_OCCUR& stSend, STG_PP_P2C_ALARM_OCCUR_ACK& stRecv, int nTimeout = 10000);
	int Send_P2C_EVENT_NOTIFY(STG_PP_P2C_EVENT_NOTIFY& stSend, STG_PP_P2C_EVENT_NOTIFY_ACK& stRecv, int nTimeout = 10000);
	int Send_P2C_INTERRUPT_STOP(STG_PP_P2C_INTERRUPT_STOP& stSend, STG_PP_P2C_INTERRUPT_STOP_ACK& stRecv, int nTimeout = 10000);
	int Send_C2P_RCP_CREATE_ACK(STG_PP_C2P_RCP_CREATE_ACK& stSend, int nTimeout = 10000);
	int Send_C2P_RCP_DELETE_ACK(STG_PP_C2P_RCP_DELETE_ACK& stSend, int nTimeout = 10000);
	int Send_C2P_RCP_MODIFY_ACK(STG_PP_C2P_RCP_MODIFY_ACK& stSend, int nTimeout = 10000);
	int Send_C2P_RCP_SELECT_ACK(STG_PP_C2P_RCP_SELECT_ACK& stSend, int nTimeout = 10000);
	int Send_C2P_RCP_INFO_ACK(STG_PP_C2P_RCP_INFO_ACK& stSend, int nTimeout = 10000);
	int Send_C2P_RCP_LIST_ACK(STG_PP_C2P_RCP_LIST_ACK& stSend, int nTimeout = 10000);
	int Send_C2P_ABS_MOVE_ACK(STG_PP_C2P_ABS_MOVE_ACK& stSend, int nTimeout = 10000);
	int Send_C2P_ABS_MOVE_COMP_ACK(STG_PP_C2P_ABS_MOVE_COMP_ACK& stSend, int nTimeout = 10000);
	int Send_C2P_REL_MOVE_ACK(STG_PP_C2P_REL_MOVE_ACK& stSend, int nTimeout = 10000);
	int Send_C2P_REL_MOVE_COMP_ACK(STG_PP_C2P_REL_MOVE_COMP_ACK& stSend, int nTimeout = 10000);
	int Send_C2P_CHAR_MOVE_ACK(STG_PP_C2P_CHAR_MOVE_ACK& stSend, int nTimeout = 10000);
	int Send_C2P_CHAR_MOVE_COMP_ACK(STG_PP_C2P_CHAR_MOVE_COMP_ACK& stSend, int nTimeout = 10000);
	int Send_C2P_INITIAL_EXECUTE_ACK(STG_PP_C2P_INITIAL_EXECUTE_ACK& stSend, int nTimeout = 10000);
	int Send_C2P_PROCESS_EXECUTE_ACK(STG_PP_C2P_PROCESS_EXECUTE_ACK& stSend, int nTimeout = 10000);
	int Send_C2P_SUBPROCESS_EXECUTE_ACK(STG_PP_C2P_SUBPROCESS_EXECUTE_ACK& stSend, int nTimeout = 10000);
	int Send_C2P_STATUS_VALUE_ACK(STG_PP_C2P_STATUS_VALUE_ACK& stSend, int nTimeout = 10000);
	int Send_C2P_MODE_CHANGE_ACK(STG_PP_C2P_MODE_CHANGE_ACK& stSend, int nTimeout = 10000);
	int Send_C2P_EVENT_STATUS_ACK(STG_PP_C2P_EVENT_STATUS_ACK& stSend, int nTimeout = 10000);
	int Send_C2P_EVENT_NOTIFY_ACK(STG_PP_C2P_EVENT_NOTIFY_ACK& stSend, int nTimeout = 10000);
	int Send_C2P_TIME_SYNC_ACK(STG_PP_C2P_TIME_SYNC_ACK& stSend, int nTimeout = 10000);
	int Send_C2P_INTERRUPT_STOP_ACK(STG_PP_C2P_INTERRUPT_STOP_ACK& stSend, int nTimeout = 10000);

};