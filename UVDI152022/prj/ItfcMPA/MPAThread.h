
#pragma once

#include "../../inc/comn/ThinThread.h"
#include "../../inc/comm/SerialComm.h"
#include "../../inc/comm/MilaraThread.h"

class CMPAThread : public CMilaraThread
{
/* 생성자 & 파괴자 */
public:

	CMPAThread(UINT8 port, UINT32 baud, UINT32 buff, LPG_MPDV shmem);
	virtual ~ CMPAThread();

/* 가상함수 재선언 */
protected:

	virtual VOID		RecvData();
	virtual BOOL		Initialized();


/* 로컬 변수 */
protected:

	ENG_PSCC			m_enLastCmd;		/* 가장 최근에 송신한 명령어 */

	LPG_MPDV			m_pstShMem;			/* 공유 메모리 */


/* 로컬 함수 */
protected:

	BOOL				IsValidTechingSet(ENG_PSCC code);

	VOID				SetAxisZValue(DOUBLE &host_val, vector <string> &vals);
	VOID				SetAxisTValue(DOUBLE &host_val, vector <string> &vals);
	VOID				SetAxisValues(DOUBLE *host_val, vector <string> &vals);
	VOID				SetAxisMRWI(DOUBLE *host_val, vector <string> &vals);

	VOID				SetTempTextInfo(vector <string> &vals);

	VOID				RecvStatusInfoCmd(vector <string> &vals);
	VOID				RecvAxisParamSetRetrieveCmd(vector <string> &vals);
	VOID				RecvBasicControlCmd(vector <string> &vals);
	VOID				RecvAlignmentCmd(vector <string> &vals);
	VOID				RecvHousekeepingCmd(vector <string> &vals);
	VOID				RecvCaliSvcCmd(vector <string> &vals);
	VOID				RecvEdgeHandlingPreAlignerSpecCmd(vector <string> &vals);
	VOID				RecvExoticCmd(vector <string> &vals);
	VOID				RecvMiscCmd(vector <string> &vals);

	VOID				ResetRecvData(ENG_PSCC code);
	VOID				ResetStatusInfoCmd(ENG_PSCC code);
	VOID				ResetAxisParamSetRetrieveCmd(ENG_PSCC code);
	VOID				ResetBasicControlCmd(ENG_PSCC code);
	VOID				ResetAlignmentCmd(ENG_PSCC code);
	VOID				ResetHousekeepingCmd(ENG_PSCC code);
	VOID				ResetCaliSvcCmd(ENG_PSCC code);	
	VOID				ResetEdgeHandlingPreAlignerSpecCmd(ENG_PSCC code);
	VOID				ResetExoticCmd(ENG_PSCC code);
	VOID				ResetMiscCmd(ENG_PSCC code);


/* 공용 함수 */
public:

	DOUBLE				GetAxisPosApp(ENG_PANC axis, INT32 value);
	DOUBLE				GetWaferCentDirect(UINT8 index, INT32 value);

	BOOL				SendData(ENG_PSCC code, PCHAR cmd);
	BOOL				GetSystemStatus(TCHAR *mesg, UINT32 size);
	BOOL				GetPreviousError(TCHAR *mesg, UINT32 size);

	/* 가장 최근에 송신한 명령어 반환 */
	ENG_PSCC			GetLastSendCmd()	{	return m_enLastCmd;	}
};