
#pragma once

#include "../../inc/comn/ThinThread.h"
#include "../../inc/comm/SerialComm.h"
#include "../../inc/comm/MilaraThread.h"

class CMDRThread : public CMilaraThread
{
/* ������ & �ı��� */
public:

	CMDRThread(UINT8 port, UINT32 baud, UINT32 buff, LPG_MRDV shmem);
	virtual ~ CMDRThread();

/* �����Լ� �缱�� */
protected:

	virtual VOID		RecvData();
	virtual BOOL		Initialized();


/* ���� ���� */
protected:

	ENG_RSCC			m_enLastCmd;		/* ���� �ֱٿ� �۽��� ��ɾ� */

	LPG_MRDV			m_pstShMem;			/* ���� �޸� */


/* ���� �Լ� */
protected:

	BOOL				IsValidTechingSet(ENG_RSCC code);

	VOID				SetMapValues(UINT8 *host_val, vector <string> &vals);
	VOID				SetSerialNum(vector <string> &vals);

	VOID				SetAxisZValue(DOUBLE &host_val, vector <string> &vals);
	VOID				SetAxisTValue(DOUBLE &host_val, vector <string> &vals);
	VOID				SetAxisValues(DOUBLE *host_val, vector <string> &vals);
	VOID				SetAxisValues(UINT32 *host_val, vector <string> &vals);
	VOID				SetAxisValues(UINT16 *host_val, vector <string> &vals);

	VOID				SetTempTextInfo(vector <string> &vals);

	/* ���ŵ� ������ �ӽ� ���� */
	VOID				RecvDataInfoCmds(vector <string> &vals);
	VOID				RecvDataHouseKeeping(vector <string> &vals);
	VOID				RecvDataStatusDiagCmds(vector <string> &vals);
	VOID				RecvDataAbsEncCmds(vector <string> &vals);
	VOID				RecvDataBasicActCmds(vector <string> &vals);
	VOID				RecvDataBasicMotionParam(vector <string> &vals);
	VOID				RecvDataAxisParmSetCmds(vector <string> &vals);
	VOID				RecvDataBasicMotionCmds(vector <string> &vals);
	VOID				RecvDataStaParamCmds(vector <string> &vals);
	VOID				RecvDataStaMotionCmds(vector <string> &vals);
	VOID				RecvDataStaScanCmds(vector <string> &vals);
	VOID				RecvDataOtFCmds(vector <string> &vals);
	VOID				RecvDataNewDualArm(vector <string> &vals);

	/* ���� ���ŵ� ������ �ʱ�ȭ */
	VOID				ResetRecvData(ENG_RSCC code);
	VOID				ResetDataInfoCmds(ENG_RSCC code);
	VOID				ResetDataHousekeepCmds(ENG_RSCC code);
	VOID				ResetDataStatusDiagCmds(ENG_RSCC code);
	VOID				ResetDataAbsEncCmds(ENG_RSCC code);
	VOID				ResetDataBasicActCmds(ENG_RSCC code);
	VOID				ResetDataBasicMotionParam(ENG_RSCC code);
	VOID				ResetDataAxisParmSetCmds(ENG_RSCC code);
	VOID				ResetDataBasicMotionCmds(ENG_RSCC code);
	VOID				ResetDataStaParamCmds(ENG_RSCC code);
	VOID				ResetDataStaMotionCmds(ENG_RSCC code);
	VOID				ResetDataStaScanCmds(ENG_RSCC code);
	VOID				ResetDataOtFCmds(ENG_RSCC code);
	VOID				ResetDataNewDualArm(ENG_RSCC code);


/* ���� �Լ� */
public:

	BOOL				SendData(ENG_RSCC code, PCHAR cmd, BOOL ack=FALSE);
	BOOL				GetExtErrorMesg(TCHAR *mesg, UINT32 size);
	BOOL				GetSystemStatus(TCHAR *mesg, UINT32 size);
	BOOL				GetHomeSwitchesState(TCHAR *mesg, UINT32 size);

	/* ���� �ֱٿ� �۽��� ��ɾ� ��ȯ */
	ENG_RSCC			GetLastSendCmd()	{	return m_enLastCmd;	}
};