
#pragma once

#include "../conf/serial.h"
#include "../comn/MySection.h"


class CRingBuff;
class CMySection;

class CSerialComm
{
/* 생성자 & 파괴자 */
public:
	CSerialComm(UINT8 port, UINT32 baud, UINT32 buff=8192, ENG_SHOT shake=ENG_SHOT::en_shot_handshake_off);
	virtual ~CSerialComm();

/* 가상함수 선언 */
protected:

/* 로컬 변수 */
protected:

	BOOL				m_bIsOpened;		/* 통신 Port가 열려져 있는지 여부 */
	// 현재 I/O Pending 상태인지 여부
	BOOL				m_bIsWritePending;
	BOOL				m_bIsReadPending;

	TCHAR				m_tzPort[64];		/* 통신 포트 번호 */

	UINT32				m_u32BaudRate;		/* 통신 속도 */
	UINT32				m_u32PortBuff;		/* 통신 포트의 수신 (링)버퍼 크기 */

	/* ------------------------------------------ */
	/* ClearCommError 함수에서 얻은 에러 값 종류  */
	/* ------------------------------------------ */
	/* CE_RXOVER	// Receive Queue overflow	  */
	/* CE_OVERRUN	// Receive Overrun Error	  */
	/* CE_RXPARITY	// Receive Parity Error		  */
	/* CE_FRAME		// Receive Framing error	  */
	/* CE_BREAK		// Break Detected			  */
	/* CE_TXFULL	// TX Queue is full			  */
	/* CE_PTO		// LPTx Timeout				  */
	/* CE_IOE		// LPTx I/O Error			  */
	/* CE_DNS		// LPTx Device not selected	  */
	/* CE_OOP		// LPTx Out-Of-Paper		  */
	/* CE_MODE		// Requested mode unsupported */
	/* ------------------------------------------ */
	UINT32				m_u32CommError;			/* ClearCommError에서 얻어진 에러 종류 */

	HANDLE				m_hComm;				/* 통신 Port 핸들 */

	UINT64				m_u64OpenTryTime;		/* 시리얼 통신 포트를 열기 시도한 시간 저장 */

	PUINT8				m_pPortBuff;			/* 통신 포트로부터 수신된 데이터를 저장할 임시 버퍼 */
	PTCHAR				m_pErrMsg;				/* Error Message를 저장할 버퍼 */

	ENG_SHOT			m_enHandShake;			/* Flow Control Type */

	// 통신 상태
	ENG_SCSC			m_enCommStatus;

	OVERLAPPED			m_ovRead, m_ovWrite;	/* Overlapped Read / Write Event 감시 */

	/* -------------------------------------------------------------------------------------------------------- */
	/*                                        COMSTAT 구조체 변수 설명                                          */
	/* -------------------------------------------------------------------------------------------------------- */
	/*																											*/
	/* fCtsHold	: CTS 신호를 받으면 전송을 대기 할 것인지 지정												    */
	/* 			  TRUE => 전송 대기																				*/
	/* fDsrHold	: DSR 신호를 받으면 전송을 대기 할 것인지 지정												    */
	/* 			  TRUE => 전송 대기																				*/
	/* fRlsdHold: RLSD 신호를 받으면 전송을 대기 할 것인지 지정												    */
	/* 			  TRUE => 전송 대기																				*/
	/* fXoffHold: XOFF 문자를 받았을 때 전송을 대기 할 것인지 지정											    */
	/* 			  TRUE => 전송 대기																				*/
	/* fXoffSent: XOFF 문자를 보냈을때 전송을 대기 할 것인지 지정												*/
	/* 			  TRUE => 전송 대기																				*/
	/* 			  XOFF 문자가 실제 문자와 상관 없이 다음 문자로 XON을 가지는 시스템으로 전송할 때 전송 중단	    */
	/* fEof		: EOF 문자를 받을 지 지정																		*/
	/* 			  TRUE => EOF 문자를 받음																	    */
	/* fTxim	: TRUE => 문자는 TransmitCommChar 함수를 통해 통신장치로 전송하기 위해 대기함 				    */
	/* 			  통신 장치는 장치의 출력 버퍼에 있는 다른 문자 앞에 이러한 문자를 전송함						*/
	/* fReserved: 예약됨. 사용하지 않음																		    */
	/* cbInQue	: 시리얼 버퍼에 들어왔지만 아직 ReadFile함수에 의해 읽혀지지 않은 바이트수.						*/
	/* cbOutQue	: 모든 쓰기함수에의해 전송되어야 할 남아있는 데이터의 바이트수									*/
	/*																											*/
	/* -------------------------------------------------------------------------------------------------------- */
	COMSTAT				m_stComStat;		// ClearCommError

	/* -------------------------------------------------------------------------------------------------------- */
	/*                                           DCB 구조체 변수 설명                                           */
	/* -------------------------------------------------------------------------------------------------------- */
	/* DCBlength        DCB구조체의 바이트크기를 지정한다.													    */
	/* BaudRate         통신장치의 전송속도를 지정한다.													        */
	/* 					실제 baud속도를 입력하거나 다음의 baud속도 인덱스중에 하나를 선택						*/
	/*																											*/
	/* 					CBR_110		CBR_19200																	*/
	/* 					CBR_300		CBR_38400																	*/
	/* 					CBR_600		CBR_56000																	*/
	/* 					CBR_1200	CBR_57600																	*/
	/* 					CBR_2400	CBR_115200																	*/
	/* 					CBR_4800	CBR_128000																	*/
	/* 					CBR_9600	CBR_256000																	*/
	/* 					CBR_14400																				*/
	/*																											*/
	/* fBinary          이진모드(binary mode) 사용함을 지정.													*/
	/* 					WIN32 API '이진모드'만 지원하므로 이멤버는 반드시 TRUE이어야 한다.						*/
	/* 					FALSE이면 작업을 할수 없다.																*/
 	/*																											*/
	/* fParity          패리티체크를 사용할것인가를 지정. TRUE이면 패리티체크는 실행되고, 에러도 보고된다.		*/
 	/*																											*/
	/* fOutxCtsFlow     CTS(Clear to Send)신호를 출력조절 컨트롤부터 감시할것인가를 지정한다.					*/
	/* 					이멤버가 TRUE이고, CTS가 0이면, CTS신호가 보내질때까지(CTS 1일때까지) 출력은 중지된다.	*/
	/*                  모뎀 또는 프린터와 같은 DCE 장치가 컴퓨터와 같은 DTE 장치에게 데이터를 받을 준비가 됐음 */
	/*                  을 나타내는 신호선이다.                                                                 */
	/* fOutxDsrFlow     DSR(Data set Read)신호를 출력조절 컨트롤부터 감시할것인가를 지정한다.					*/
	/* 					이멤버가 TRUE이고, DSR이 0이면, DSR신호가 보내질때까지(DSR 1일때까지) 출력은 중지된다.	*/
	/* fDtrControl      DTR(Data terminal Ready)신호를 사용할것인가 지정한다. 다음값중에 하나를 지정할수 있다.	*/
	/*                  컴퓨터 또는 터미널이 모뎀에게 자신이 송수신 가능한 상태임을 알리는 신호선이며, 일반적으 */
	/*                  로 컴퓨터 등이 전원 인가후 통신 포트를 초기화한 후 이 신호를 출력 시킨다.               */
 	/*																											*/
	/* 					   Value																				*/
	/* 					   DTR_CONTROL_DISABLE      - 장치를 열거나 DTR이 on되려고 할때, DTR을 OFF한다.			*/
	/* 					   DTR_CONTROL_ENABLE       - 장치를 열거나 DTR이 OFF되려고 할때, DTR을 on한다.			*/
	/* 					   DTR_CONTROL_HANDSHAKE    - DTR handshaking을 on한다.									*/
	/* 												  에러가 발생하면 어플리케이션에서 EscapeCommFunction 함수를*/
	/* 												  이용해서 적용한다.										*/
	/*																											*/
	/* fDsrSensitivity  장치가 DSR(Data Set Read) 신호영역에 민감할것을 지정한다.								*/
	/* 					TRUE이면, 모뎀의 DSR 입력선이 고위(1)일때까지 장치가 모든바이트의 수신을 무시한다.		*/
	/*                  모뎀이 컴퓨터 또는 터미널에게 자신이 송수신 가능한 상태임을 알려주는 신호선이며, 일반적 */
	/*                  으로 모뎀에 전원 인가후 모뎀이 자신의 상태를 파악한 후 이상이 없을 때 이 신호를 출력시킴*/
	/* fTXContinueOnXoff수신버퍼가 가득차거나 장치에 XoffChar 가 전송되면 송신을 중단할것인가를 지정한다.		*/
	/* fOutX            송신동안 XON/XOFF 흐름제어를 할것인가를 지정한다.										*/
	/* 					TRUE이면, XoffChar문자를 받았을때 송신을 중지하고, 다시 XonChar문자를 받았을때 송신한다.*/
	/* fInX             수신동안 XON/XOFF 흐름제어를 할것인가를 지정한다.										*/
	/* 					TRUE이면, XoffLim 바이트로 버퍼가 가득찼을때 XoffChar 문자가 송신되고					*/
	/* 					XonLim 바이트 이하로 비었을때 XonChar 문자가 송신된다.									*/
	/* fErrorChar       패리티에러가 수신된 바이트들은 ErrorChar 문자로 대치할것인가를 지정한다.				*/
	/* 					TRUE이면 fParity 멤버가 TRUE이면 변환이 일어난다.										*/
	/* fNull            NULL바이트를 제외할 것인가를 지정한다.													*/
	/* 					TRUE이면 NULL바이트가 수신되었을때 버려진다.											*/
	/* fRtsControl      RTS(Request to Send) 흐름제어를 사용할것인지 지정한다.									*/
	/* 					0이면 디폴트 RTS_CONTROL_HANDSHAKE 이다. 다음값중 하나를 지정할수 있다.					*/
	/*                  컴퓨터와 같은 DTE 장치가 모뎀 또는 프린터와 같은 DCE 장치에게 데이터를 받을 준비가 됐음 */
	/*                  을 나타내는 신호선이다.                                                                 */
	/*																											*/
	/* 					   Value																				*/
	/* 					   RTS_CONTROL_DISABLE      - 장치를 열거나 RTS신호가 on이 되려할때, RTS를 OFF한다.		*/
	/* 					   RTS_CONTROL_ENABLE       - 장치를 열거나 RTS신호가 OFF되려 할때, RTS를 on한다.		*/
	/* 					   RTS_CONTROL_HANDSHAKE    - RTS handshaking을 on한다.									*/
	/* 					   RTS_CONTROL_TOGGLE       - 송신할 바이트 있다면 RTS 라인은 1이고 					*/
	/* 												  모든버퍼에 바이트를 보내고, RTS 라인은 0이된다.			*/
	/*																											*/
	/* fAbortOnError    에러가 발생하면 송신,수신 작업이 종료할것인가를 지정한다.								*/
	/* 					TRUE이면, 에러가 발생하면 모든송수신 작업은 에러상태와 함께 종료된다.					*/
	/* 					장치는 어떤통신운영도 ClearCommError 함수를 호출하기까지 수락하지 않는다.				*/
 	/*																											*/
	/* fDummy2          사용안함.																				*/
	/* wReserved        사용안함. 항상 0이어야한다./*															*/
	/* XonLim           XON문자가 보내기전에, 수신버퍼에 허용되는 최소바이트.									*/
	/* XoffLim          XOFF문자가 보내기전에, 수신버퍼에 허용되는 최대바이트.									*/
	/*					허용된 최대바이트 = 수신버퍼(byte) - XoffLin											*/
	/* ByteSize         송신,수신 바이트의 비트수. (4-8)														*/
	/* Parity           사용될 패리티를 설정한다. 다음값중에 하나를 지정할수 있다.								*/
	/*																											*/
	/* 						Value																				*/
	/* 						EVENPARITY   -  짝수																*/
	/* 						MARKPARITY   -  마크																*/
	/* 						NOPARITY     -  없음																*/
	/* 						ODDPARITY    -  홀수																*/
	/* 						SPACEPARITY  -  간격																*/
 	/*																											*/
	/* StopBits          사용될 STOP비트수를 설정한다. 다음값중에 하나를 지정할수 있다.							*/
 	/*																											*/
	/* 						Value																				*/
	/* 						ONESTOPBIT    - 1   비트															*/
	/* 						onE5STOPBITS  - 1.5 비트															*/
	/* 						TWOSTOPBITS   - 2   비트															*/
 	/*																											*/
	/* XonChar          송신,수신에 사용될 XON문자를 지정한다.													*/
	/* XoffChar         송신,수신에 사용될 XOFF문자를 지정한다.													*/
	/* ErrorChar        Parity 에러와 함께수신된 바이트에 치환할 문자를 지정한다.								*/
	/* EofChar          binary모드가 아닐경우, 데이터 끝을 나타내는 문자를 지정한다.							*/
	/* EvtChar          신호 이벤트에 사용될 문자를 지정한다.													*/
	/* wReserved1       사용안함.																				*/
	/* -------------------------------------------------------------------------------------------------------- */
	DCB					m_stDCB;

	/* ---------------------------------------------- */
	/* 통신 포트로부터 수신된 데이터 저장을 위한 버퍼 */
	/* ---------------------------------------------- */
	CRingBuff			*m_pReadBuff;

/* 로컬 함수 */
protected:

	BOOL				ConfigPort();
	BOOL				PurgeClear();
	BOOL				CreateEvent();
	BOOL				SetClearCommError();

	VOID				WaitPeekMessageQueue(UINT64 wait);
	VOID				uSleep(UINT64 usec);


/* 공용 함수 */
public:

	BOOL				Close();
	BOOL				OpenDefault();
	BOOL				OpenExt(ENG_CRTB block_mode, BOOL is_binary);

	/* 시리얼 포트에 읽기와 쓰기 하는데 소요되는 제한 시간(Timer) 설정 */
	BOOL				SetTimeout(ENG_CRTB mode);
	BOOL				SetupComm(UINT32 port_buff=4096);
	BOOL				SetCommMask(UINT32 evt_mask=EV_RXCHAR|EV_TXEMPTY|EV_ERR);
	BOOL				SetCommState(UINT8 byte_size=8, UINT8 stop_bit=ONESTOPBIT,
									 UINT8 parity=NOPARITY, BOOL is_binary=TRUE);
	PTCHAR				GetErrMsg()			{	return m_pErrMsg;		}
	VOID				PutErrMsg(PTCHAR msg, UINT8 flag=0x00, BOOL msg_box=FALSE);

	UINT16				WaitCommEvent();
	VOID				WaitCommEventBreak();

	/* 통신 포트의 상태 정보 반환 처리 */
	BOOL				IsOpenPort();

	/* 이벤트 감지에 따른 송/수신 처리 */
	UINT32				ReadData(UINT32 time_out=100);
	UINT8				GetHeadData();
	VOID				ReadAllData(UINT32 time_out=100);
	UINT32				WriteByte(UINT8 data, UINT32 time_out=100);
	UINT32				WriteData(PUINT8 buff, UINT32 size, UINT32 time=0, UINT32 time_step=0);

	/* 통신 포트로부터 수신된 데이터 반환 */
	BOOL				IsWritePending()	{	return m_bIsWritePending;	}
	BOOL				IsReadPending()		{	return m_bIsReadPending;	}
	UINT32				GetReadSize();
	UINT32				PopReadData(PUINT8 data, UINT32 size);
	BOOL				CopyReadData(PUINT8 data, UINT32 size);

	/* 현재 통신 포트의 환경 설정 값 반환 */
	BOOL				GetConfigPort(STG_SSCI &data);
	BOOL				SetConfigPort(LPG_SSCI data);

	UINT32				GetBaudRate()	{	return m_u32BaudRate;	}

	/* 통신 상태 값 반환 */
	ENG_SCSC			GetCommStatus()	{	return m_enCommStatus;	}
	UINT32				GetCommError()	{	return m_u32CommError;	}

	/* 시리얼 통신 포트를 다시 열기 시도해도 되는지 즉, 시리얼 통신포트 다시 열기까지 일정 기간 대기하기 위함 */
	BOOL				IsTryCommOpenTime(UINT64 wait)	{	return (GetTickCount64() > (m_u64OpenTryTime + wait));	}

	/* 버퍼 내에 특정 문자가 존재하는지 검색 */
	BOOL				FindChar(UINT8 find, UINT32 &pos);
	BOOL				FindChar(UINT8 find1, UINT8 find2, UINT32 &pos);

	/* 수신 데이터가 임시로 저장된 버퍼 포인터 반환 */
	PUINT8				GetPortBuff()	{	return m_pPortBuff;		}
};
