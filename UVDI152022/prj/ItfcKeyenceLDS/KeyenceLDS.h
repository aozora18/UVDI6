#pragma once
#include "SocketComm.h"

#define DEF_CMD_READ			"M0"
#define DEF_CMD_STATUS_READ		"MS"
#define DEF_CMD_SELECT_READ		"SR"
#define DEF_CMD_SELECT_WRITE	"SW"
#define DEF_CMD_DEC_POS_READ	"FR"
#define DEF_CMD_ERROR			"ER"
#define DEF_CMD_END				"\r\n"
#define DEF_CMD_EXECUTED		"+000000001"

enum COMM_RETURNS
{
	eCOMM_RESULT_OK = 0,

	eCOMM_ERROR_INVALIDDATA,
	eCOMM_ERROR_DISCONNECTED,
	eCOMM_ERROR_TIMEOUT,
}; 

class CKeyenceLDS : public CSocketCComm
{
public:
	CKeyenceLDS();
	virtual ~CKeyenceLDS(void);

	enum EN_ERROR_CODE
	{
		eERR_CONNECT = -1,						// No Connect
		eERR_NONE = 0,							// OK
		eERR_RESPON,							// No Respon
		eERR_WRITTEN_DATA_OVER_VALID_RANGE = 9,
		eERR_COMMAND_NOT_STATE = 12,
		eERR_ADDRESS_NOT_WRITE = 14,
		eERR_DATA_NUMBER_NOT_READ = 16,
		eERR_DATA_NUMBER_OVER_VALID_RANGE = 20,
		eERR_ID_OVER_VALID_RANGE = 22,
		eERR_SUPPORT_READ_WRITE = 31,
		eERR_SYSTEM_ERROR_STATE = 254,
		eERR_FORMAT_NOT_CORRECT = 255,
	};

	enum EN_MODULE_STATUS
	{
		eSTS_ALL_OFF = 0,
		eSTS_HIGH,
		eSTS_LOW,
		eSTS_ERROR,
		eSTS_GO,
		eSTS_HH = 8,
		eSTS_LL = 16,
	};

	enum EN_IL_WR_CODE
	{
		eIL_ZERO_SHIFT = 1,
		eIL_RESET_ZERO_SHIFT,
		eIL_RESET_REQ,
//		eIL_RESET_INIT_REQ = 5,
	};

private:
	long RecvProcess(long lSize, BYTE *pbyData);
	long WaitReply(char* pszRecv, DWORD dwTimeout);

public:
	bool Connect();
	
	long ReadData(char* pszRecv, DWORD dwTimeout, BOOL bReadStatus = FALSE);
	long WriteData(int nModuleIdx, int nCode, DWORD dwTimeout);

	// True : Write Successful, FALSE : Failed to write
	BOOL ZeroShift(int nModuleIdx, DWORD dwTimeout = 5000);
	BOOL ResetZeroShift(int nModuleIdx, DWORD dwTimeout = 5000);
	BOOL ResetDevice(int nModuleIdx, DWORD dwTimeout = 5000);
//	BOOL ResetInitDevice(int nModuleIdx, DWORD dwTimeout = 5000);

	void ZeroSet(double dValue) { m_dBaseValue = dValue; }
	void SetDecimalPos(double dValue) { m_dDecimalPos = dValue; }

	double GetBaseValue() { return m_dBaseValue; }

	long LDSMeasure(double* pdValue, int nModuleIdx, DWORD dwTimeout = 5000);
	long LDSMeasure(double* pdValue, int nModuleIdx, int* pnStatus, DWORD dwTimeout = 5000);
	void LDSMultiMeasure(int nModuleCnt, double* pdValue, DWORD dwTimeout = 5000);

private:
	BOOL m_bListen;
	//BOOL m_bReadStart;

	std::string m_strCMD;
	std::string m_strBuffer;
	CRITICAL_SECTION m_csSend;
	CRITICAL_SECTION m_csRecv;

	double m_dBaseValue;
	double m_dDecimalPos;
};
