
#pragma once

#include "../conf/serial.h"
#include "../comn/MySection.h"


class CRingBuff;
class CMySection;

class CSerialComm
{
/* ������ & �ı��� */
public:
	CSerialComm(UINT8 port, UINT32 baud, UINT32 buff=8192, ENG_SHOT shake=ENG_SHOT::en_shot_handshake_off);
	virtual ~CSerialComm();

/* �����Լ� ���� */
protected:

/* ���� ���� */
protected:

	BOOL				m_bIsOpened;		/* ��� Port�� ������ �ִ��� ���� */
	// ���� I/O Pending �������� ����
	BOOL				m_bIsWritePending;
	BOOL				m_bIsReadPending;

	TCHAR				m_tzPort[64];		/* ��� ��Ʈ ��ȣ */

	UINT32				m_u32BaudRate;		/* ��� �ӵ� */
	UINT32				m_u32PortBuff;		/* ��� ��Ʈ�� ���� (��)���� ũ�� */

	/* ------------------------------------------ */
	/* ClearCommError �Լ����� ���� ���� �� ����  */
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
	UINT32				m_u32CommError;			/* ClearCommError���� ����� ���� ���� */

	HANDLE				m_hComm;				/* ��� Port �ڵ� */

	UINT64				m_u64OpenTryTime;		/* �ø��� ��� ��Ʈ�� ���� �õ��� �ð� ���� */

	PUINT8				m_pPortBuff;			/* ��� ��Ʈ�κ��� ���ŵ� �����͸� ������ �ӽ� ���� */
	PTCHAR				m_pErrMsg;				/* Error Message�� ������ ���� */

	ENG_SHOT			m_enHandShake;			/* Flow Control Type */

	// ��� ����
	ENG_SCSC			m_enCommStatus;

	OVERLAPPED			m_ovRead, m_ovWrite;	/* Overlapped Read / Write Event ���� */

	/* -------------------------------------------------------------------------------------------------------- */
	/*                                        COMSTAT ����ü ���� ����                                          */
	/* -------------------------------------------------------------------------------------------------------- */
	/*																											*/
	/* fCtsHold	: CTS ��ȣ�� ������ ������ ��� �� ������ ����												    */
	/* 			  TRUE => ���� ���																				*/
	/* fDsrHold	: DSR ��ȣ�� ������ ������ ��� �� ������ ����												    */
	/* 			  TRUE => ���� ���																				*/
	/* fRlsdHold: RLSD ��ȣ�� ������ ������ ��� �� ������ ����												    */
	/* 			  TRUE => ���� ���																				*/
	/* fXoffHold: XOFF ���ڸ� �޾��� �� ������ ��� �� ������ ����											    */
	/* 			  TRUE => ���� ���																				*/
	/* fXoffSent: XOFF ���ڸ� �������� ������ ��� �� ������ ����												*/
	/* 			  TRUE => ���� ���																				*/
	/* 			  XOFF ���ڰ� ���� ���ڿ� ��� ���� ���� ���ڷ� XON�� ������ �ý������� ������ �� ���� �ߴ�	    */
	/* fEof		: EOF ���ڸ� ���� �� ����																		*/
	/* 			  TRUE => EOF ���ڸ� ����																	    */
	/* fTxim	: TRUE => ���ڴ� TransmitCommChar �Լ��� ���� �����ġ�� �����ϱ� ���� ����� 				    */
	/* 			  ��� ��ġ�� ��ġ�� ��� ���ۿ� �ִ� �ٸ� ���� �տ� �̷��� ���ڸ� ������						*/
	/* fReserved: �����. ������� ����																		    */
	/* cbInQue	: �ø��� ���ۿ� �������� ���� ReadFile�Լ��� ���� �������� ���� ����Ʈ��.						*/
	/* cbOutQue	: ��� �����Լ������� ���۵Ǿ�� �� �����ִ� �������� ����Ʈ��									*/
	/*																											*/
	/* -------------------------------------------------------------------------------------------------------- */
	COMSTAT				m_stComStat;		// ClearCommError

	/* -------------------------------------------------------------------------------------------------------- */
	/*                                           DCB ����ü ���� ����                                           */
	/* -------------------------------------------------------------------------------------------------------- */
	/* DCBlength        DCB����ü�� ����Ʈũ�⸦ �����Ѵ�.													    */
	/* BaudRate         �����ġ�� ���ۼӵ��� �����Ѵ�.													        */
	/* 					���� baud�ӵ��� �Է��ϰų� ������ baud�ӵ� �ε����߿� �ϳ��� ����						*/
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
	/* fBinary          �������(binary mode) ������� ����.													*/
	/* 					WIN32 API '�������'�� �����ϹǷ� �̸���� �ݵ�� TRUE�̾�� �Ѵ�.						*/
	/* 					FALSE�̸� �۾��� �Ҽ� ����.																*/
 	/*																											*/
	/* fParity          �и�Ƽüũ�� ����Ұ��ΰ��� ����. TRUE�̸� �и�Ƽüũ�� ����ǰ�, ������ ����ȴ�.		*/
 	/*																											*/
	/* fOutxCtsFlow     CTS(Clear to Send)��ȣ�� ������� ��Ʈ�Ѻ��� �����Ұ��ΰ��� �����Ѵ�.					*/
	/* 					�̸���� TRUE�̰�, CTS�� 0�̸�, CTS��ȣ�� ������������(CTS 1�϶�����) ����� �����ȴ�.	*/
	/*                  �� �Ǵ� �����Ϳ� ���� DCE ��ġ�� ��ǻ�Ϳ� ���� DTE ��ġ���� �����͸� ���� �غ� ���� */
	/*                  �� ��Ÿ���� ��ȣ���̴�.                                                                 */
	/* fOutxDsrFlow     DSR(Data set Read)��ȣ�� ������� ��Ʈ�Ѻ��� �����Ұ��ΰ��� �����Ѵ�.					*/
	/* 					�̸���� TRUE�̰�, DSR�� 0�̸�, DSR��ȣ�� ������������(DSR 1�϶�����) ����� �����ȴ�.	*/
	/* fDtrControl      DTR(Data terminal Ready)��ȣ�� ����Ұ��ΰ� �����Ѵ�. �������߿� �ϳ��� �����Ҽ� �ִ�.	*/
	/*                  ��ǻ�� �Ǵ� �͹̳��� �𵩿��� �ڽ��� �ۼ��� ������ �������� �˸��� ��ȣ���̸�, �Ϲ����� */
	/*                  �� ��ǻ�� ���� ���� �ΰ��� ��� ��Ʈ�� �ʱ�ȭ�� �� �� ��ȣ�� ��� ��Ų��.               */
 	/*																											*/
	/* 					   Value																				*/
	/* 					   DTR_CONTROL_DISABLE      - ��ġ�� ���ų� DTR�� on�Ƿ��� �Ҷ�, DTR�� OFF�Ѵ�.			*/
	/* 					   DTR_CONTROL_ENABLE       - ��ġ�� ���ų� DTR�� OFF�Ƿ��� �Ҷ�, DTR�� on�Ѵ�.			*/
	/* 					   DTR_CONTROL_HANDSHAKE    - DTR handshaking�� on�Ѵ�.									*/
	/* 												  ������ �߻��ϸ� ���ø����̼ǿ��� EscapeCommFunction �Լ���*/
	/* 												  �̿��ؼ� �����Ѵ�.										*/
	/*																											*/
	/* fDsrSensitivity  ��ġ�� DSR(Data Set Read) ��ȣ������ �ΰ��Ұ��� �����Ѵ�.								*/
	/* 					TRUE�̸�, ���� DSR �Է¼��� ����(1)�϶����� ��ġ�� ������Ʈ�� ������ �����Ѵ�.		*/
	/*                  ���� ��ǻ�� �Ǵ� �͹̳ο��� �ڽ��� �ۼ��� ������ �������� �˷��ִ� ��ȣ���̸�, �Ϲ��� */
	/*                  ���� �𵩿� ���� �ΰ��� ���� �ڽ��� ���¸� �ľ��� �� �̻��� ���� �� �� ��ȣ�� ��½�Ŵ*/
	/* fTXContinueOnXoff���Ź��۰� �������ų� ��ġ�� XoffChar �� ���۵Ǹ� �۽��� �ߴ��Ұ��ΰ��� �����Ѵ�.		*/
	/* fOutX            �۽ŵ��� XON/XOFF �帧��� �Ұ��ΰ��� �����Ѵ�.										*/
	/* 					TRUE�̸�, XoffChar���ڸ� �޾����� �۽��� �����ϰ�, �ٽ� XonChar���ڸ� �޾����� �۽��Ѵ�.*/
	/* fInX             ���ŵ��� XON/XOFF �帧��� �Ұ��ΰ��� �����Ѵ�.										*/
	/* 					TRUE�̸�, XoffLim ����Ʈ�� ���۰� ����á���� XoffChar ���ڰ� �۽ŵǰ�					*/
	/* 					XonLim ����Ʈ ���Ϸ� ������� XonChar ���ڰ� �۽ŵȴ�.									*/
	/* fErrorChar       �и�Ƽ������ ���ŵ� ����Ʈ���� ErrorChar ���ڷ� ��ġ�Ұ��ΰ��� �����Ѵ�.				*/
	/* 					TRUE�̸� fParity ����� TRUE�̸� ��ȯ�� �Ͼ��.										*/
	/* fNull            NULL����Ʈ�� ������ ���ΰ��� �����Ѵ�.													*/
	/* 					TRUE�̸� NULL����Ʈ�� ���ŵǾ����� ��������.											*/
	/* fRtsControl      RTS(Request to Send) �帧��� ����Ұ����� �����Ѵ�.									*/
	/* 					0�̸� ����Ʈ RTS_CONTROL_HANDSHAKE �̴�. �������� �ϳ��� �����Ҽ� �ִ�.					*/
	/*                  ��ǻ�Ϳ� ���� DTE ��ġ�� �� �Ǵ� �����Ϳ� ���� DCE ��ġ���� �����͸� ���� �غ� ���� */
	/*                  �� ��Ÿ���� ��ȣ���̴�.                                                                 */
	/*																											*/
	/* 					   Value																				*/
	/* 					   RTS_CONTROL_DISABLE      - ��ġ�� ���ų� RTS��ȣ�� on�� �Ƿ��Ҷ�, RTS�� OFF�Ѵ�.		*/
	/* 					   RTS_CONTROL_ENABLE       - ��ġ�� ���ų� RTS��ȣ�� OFF�Ƿ� �Ҷ�, RTS�� on�Ѵ�.		*/
	/* 					   RTS_CONTROL_HANDSHAKE    - RTS handshaking�� on�Ѵ�.									*/
	/* 					   RTS_CONTROL_TOGGLE       - �۽��� ����Ʈ �ִٸ� RTS ������ 1�̰� 					*/
	/* 												  �����ۿ� ����Ʈ�� ������, RTS ������ 0�̵ȴ�.			*/
	/*																											*/
	/* fAbortOnError    ������ �߻��ϸ� �۽�,���� �۾��� �����Ұ��ΰ��� �����Ѵ�.								*/
	/* 					TRUE�̸�, ������ �߻��ϸ� ���ۼ��� �۾��� �������¿� �Բ� ����ȴ�.					*/
	/* 					��ġ�� ���ſ�� ClearCommError �Լ��� ȣ���ϱ���� �������� �ʴ´�.				*/
 	/*																											*/
	/* fDummy2          ������.																				*/
	/* wReserved        ������. �׻� 0�̾���Ѵ�./*															*/
	/* XonLim           XON���ڰ� ����������, ���Ź��ۿ� ���Ǵ� �ּҹ���Ʈ.									*/
	/* XoffLim          XOFF���ڰ� ����������, ���Ź��ۿ� ���Ǵ� �ִ����Ʈ.									*/
	/*					���� �ִ����Ʈ = ���Ź���(byte) - XoffLin											*/
	/* ByteSize         �۽�,���� ����Ʈ�� ��Ʈ��. (4-8)														*/
	/* Parity           ���� �и�Ƽ�� �����Ѵ�. �������߿� �ϳ��� �����Ҽ� �ִ�.								*/
	/*																											*/
	/* 						Value																				*/
	/* 						EVENPARITY   -  ¦��																*/
	/* 						MARKPARITY   -  ��ũ																*/
	/* 						NOPARITY     -  ����																*/
	/* 						ODDPARITY    -  Ȧ��																*/
	/* 						SPACEPARITY  -  ����																*/
 	/*																											*/
	/* StopBits          ���� STOP��Ʈ���� �����Ѵ�. �������߿� �ϳ��� �����Ҽ� �ִ�.							*/
 	/*																											*/
	/* 						Value																				*/
	/* 						ONESTOPBIT    - 1   ��Ʈ															*/
	/* 						onE5STOPBITS  - 1.5 ��Ʈ															*/
	/* 						TWOSTOPBITS   - 2   ��Ʈ															*/
 	/*																											*/
	/* XonChar          �۽�,���ſ� ���� XON���ڸ� �����Ѵ�.													*/
	/* XoffChar         �۽�,���ſ� ���� XOFF���ڸ� �����Ѵ�.													*/
	/* ErrorChar        Parity ������ �Բ����ŵ� ����Ʈ�� ġȯ�� ���ڸ� �����Ѵ�.								*/
	/* EofChar          binary��尡 �ƴҰ��, ������ ���� ��Ÿ���� ���ڸ� �����Ѵ�.							*/
	/* EvtChar          ��ȣ �̺�Ʈ�� ���� ���ڸ� �����Ѵ�.													*/
	/* wReserved1       ������.																				*/
	/* -------------------------------------------------------------------------------------------------------- */
	DCB					m_stDCB;

	/* ---------------------------------------------- */
	/* ��� ��Ʈ�κ��� ���ŵ� ������ ������ ���� ���� */
	/* ---------------------------------------------- */
	CRingBuff			*m_pReadBuff;

/* ���� �Լ� */
protected:

	BOOL				ConfigPort();
	BOOL				PurgeClear();
	BOOL				CreateEvent();
	BOOL				SetClearCommError();

	VOID				WaitPeekMessageQueue(UINT64 wait);
	VOID				uSleep(UINT64 usec);


/* ���� �Լ� */
public:

	BOOL				Close();
	BOOL				OpenDefault();
	BOOL				OpenExt(ENG_CRTB block_mode, BOOL is_binary);

	/* �ø��� ��Ʈ�� �б�� ���� �ϴµ� �ҿ�Ǵ� ���� �ð�(Timer) ���� */
	BOOL				SetTimeout(ENG_CRTB mode);
	BOOL				SetupComm(UINT32 port_buff=4096);
	BOOL				SetCommMask(UINT32 evt_mask=EV_RXCHAR|EV_TXEMPTY|EV_ERR);
	BOOL				SetCommState(UINT8 byte_size=8, UINT8 stop_bit=ONESTOPBIT,
									 UINT8 parity=NOPARITY, BOOL is_binary=TRUE);
	PTCHAR				GetErrMsg()			{	return m_pErrMsg;		}
	VOID				PutErrMsg(PTCHAR msg, UINT8 flag=0x00, BOOL msg_box=FALSE);

	UINT16				WaitCommEvent();
	VOID				WaitCommEventBreak();

	/* ��� ��Ʈ�� ���� ���� ��ȯ ó�� */
	BOOL				IsOpenPort();

	/* �̺�Ʈ ������ ���� ��/���� ó�� */
	UINT32				ReadData(UINT32 time_out=100);
	UINT8				GetHeadData();
	VOID				ReadAllData(UINT32 time_out=100);
	UINT32				WriteByte(UINT8 data, UINT32 time_out=100);
	UINT32				WriteData(PUINT8 buff, UINT32 size, UINT32 time=0, UINT32 time_step=0);

	/* ��� ��Ʈ�κ��� ���ŵ� ������ ��ȯ */
	BOOL				IsWritePending()	{	return m_bIsWritePending;	}
	BOOL				IsReadPending()		{	return m_bIsReadPending;	}
	UINT32				GetReadSize();
	UINT32				PopReadData(PUINT8 data, UINT32 size);
	BOOL				CopyReadData(PUINT8 data, UINT32 size);

	/* ���� ��� ��Ʈ�� ȯ�� ���� �� ��ȯ */
	BOOL				GetConfigPort(STG_SSCI &data);
	BOOL				SetConfigPort(LPG_SSCI data);

	UINT32				GetBaudRate()	{	return m_u32BaudRate;	}

	/* ��� ���� �� ��ȯ */
	ENG_SCSC			GetCommStatus()	{	return m_enCommStatus;	}
	UINT32				GetCommError()	{	return m_u32CommError;	}

	/* �ø��� ��� ��Ʈ�� �ٽ� ���� �õ��ص� �Ǵ��� ��, �ø��� �����Ʈ �ٽ� ������� ���� �Ⱓ ����ϱ� ���� */
	BOOL				IsTryCommOpenTime(UINT64 wait)	{	return (GetTickCount64() > (m_u64OpenTryTime + wait));	}

	/* ���� ���� Ư�� ���ڰ� �����ϴ��� �˻� */
	BOOL				FindChar(UINT8 find, UINT32 &pos);
	BOOL				FindChar(UINT8 find1, UINT8 find2, UINT32 &pos);

	/* ���� �����Ͱ� �ӽ÷� ����� ���� ������ ��ȯ */
	PUINT8				GetPortBuff()	{	return m_pPortBuff;		}
};
