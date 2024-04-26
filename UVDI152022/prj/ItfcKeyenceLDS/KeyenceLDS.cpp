#include "pch.h"
#include "KeyenceLDS.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CKeyenceLDS::CKeyenceLDS() : CSocketCComm()
{
	m_bListen = FALSE;
	//m_bReadStart = FALSE;

	m_dBaseValue = 0;
	m_dDecimalPos = 0.001;

	InitializeCriticalSection(&m_csSend);
}

CKeyenceLDS::~CKeyenceLDS(void)
{
	NetClose();
	DeleteCriticalSection(&m_csSend);
}

bool CKeyenceLDS::Connect()
{
	if (nullptr == this)
	{
		return false;
	}

	char pczLocalIP[32] = { 0, };
	char pczRemoteIP[32] = { 0, };
	unsigned short uczPort;

	GetConfigLocalIPAddr(pczLocalIP);
	GetConfigRemoteIPAddr(pczRemoteIP);
	uczPort = GetConfigPortNumber();

	return NetOpen(pczLocalIP, pczRemoteIP, uczPort);
}

long CKeyenceLDS::RecvProcess(long lSize, BYTE *pbyData)
{
	if (0 >= lSize)
		return -1;

	std::string strBuf = std::string(pbyData, pbyData + lSize);

	if (TryEnterCriticalSection(&m_csRecv))
	{
		m_strBuffer += strBuf;	
		LeaveCriticalSection(&m_csRecv);
	}
	
	return 0;
}

long CKeyenceLDS::WaitReply(char* pszRecv, DWORD dwTimeout)
{
	DWORD dwTime = GetTickCount();

	while ((GetTickCount() - dwTime) < dwTimeout)
	{
		if (TryEnterCriticalSection(&m_csRecv))
		{
		
			
			if (m_strBuffer.find(DEF_CMD_ERROR) != std::string::npos)
			{
				m_strBuffer.replace(0, m_strBuffer.find(m_strCMD) + 3, "");
				m_strBuffer.replace(m_strBuffer.find(DEF_CMD_END), m_strBuffer.size(), "");
				LeaveCriticalSection(&m_csRecv);
				return eCOMM_ERROR_INVALIDDATA;
			}
			else if (m_strBuffer.size() != 0 && m_strBuffer.find(DEF_CMD_END) != std::string::npos)
			{
				while (m_strBuffer.find(m_strCMD) != std::string::npos)
				{
					m_strBuffer.replace(0, m_strBuffer.find(m_strCMD) + 3, "");
				}

				m_strBuffer.replace(m_strBuffer.find(DEF_CMD_END), m_strBuffer.size(), "");

				if (NULL != pszRecv)
				{
					memcpy(pszRecv, m_strBuffer.data(), m_strBuffer.length());
				}

				m_strBuffer.clear();
				LeaveCriticalSection(&m_csRecv);
				return eCOMM_RESULT_OK;
			}
			LeaveCriticalSection(&m_csRecv);
		}
		Sleep(10);
	}

	// timeout
	m_strBuffer.clear();
	//return eERR_RESPON;
	return eCOMM_ERROR_TIMEOUT;
}

long CKeyenceLDS::ReadData(char* pszRecv, DWORD dwTimeout, BOOL bReadStatus)
{
	if (FALSE == ChkConnect())
		return eCOMM_ERROR_DISCONNECTED;/*return eERR_CONNECT;*/

	EnterCriticalSection(&m_csSend);
	m_strCMD = (TRUE == bReadStatus) ? DEF_CMD_STATUS_READ : DEF_CMD_READ;
	
	CString strMsg;

	strMsg = m_strCMD.c_str();
	strMsg += _T(DEF_CMD_END);

	long lSize = strMsg.GetLength();
	char *szMsg = new char[lSize];

	memcpy(szMsg, CStringA(strMsg), lSize);
	Send(lSize, (BYTE*)szMsg);
	delete[] szMsg;

	LeaveCriticalSection(&m_csSend);

	long lRet = WaitReply(pszRecv, dwTimeout);

	return lRet;
}

long CKeyenceLDS::WriteData(int nModuleIdx, int nCode, DWORD dwTimeout)
{

	if (TryEnterCriticalSection(&m_csRecv))
	{
		m_strBuffer.clear();
		LeaveCriticalSection(&m_csRecv);
	}

	if (FALSE == ChkConnect())
		return eCOMM_ERROR_DISCONNECTED;/*return eERR_CONNECT;*/

	EnterCriticalSection(&m_csSend);

	m_strCMD = DEF_CMD_SELECT_WRITE;
	
	CString strMsg;

	strMsg.Format(_T("%s,%02d,%03d,%s%s"), _T(DEF_CMD_SELECT_WRITE), nModuleIdx, nCode, _T(DEF_CMD_EXECUTED), _T(DEF_CMD_END));

	long lSize = strMsg.GetLength();
	char *szMsg = new char[lSize];

	memcpy(szMsg, CStringA(strMsg), lSize);

	m_bListen = TRUE;
	Send(lSize, (BYTE*)szMsg);

	long lRet = WaitReply(NULL, dwTimeout);

	delete[] szMsg;

	LeaveCriticalSection(&m_csSend);

	return lRet;
}

BOOL CKeyenceLDS::ZeroShift(int nModuleIdx, DWORD dwTimeout)
{
	if (/*eERR_NONE*/eCOMM_RESULT_OK == WriteData(nModuleIdx, eIL_ZERO_SHIFT, dwTimeout))
	{
		return TRUE;
	}
	else
	{
		// Write Errer
		return FALSE;
	}
}

BOOL CKeyenceLDS::ResetZeroShift(int nModuleIdx, DWORD dwTimeout)
{
	if (/*eERR_NONE*/eCOMM_RESULT_OK == WriteData(nModuleIdx, eIL_RESET_ZERO_SHIFT, dwTimeout))
	{
		return TRUE;
	}
	else
	{
		// Write Errer
		return FALSE;
	}
}

BOOL CKeyenceLDS::ResetDevice(int nModuleIdx, DWORD dwTimeout)
{
	if (/*eERR_NONE*/eCOMM_RESULT_OK == WriteData(nModuleIdx, eIL_RESET_REQ, dwTimeout))
	{
		return TRUE;
	}
	else
	{
		// Write Errer
		return FALSE;
	}
}

// BOOL CKeyenceLDS::ResetInitDevice(int nModuleIdx, DWORD dwTimeout)
// {
// 	if (/*eERR_NONE*/eCOMM_RESULT_OK == WriteData(nModuleIdx, eIL_RESET_INIT_REQ, dwTimeout))
// 	{
// 		return TRUE;
// 	}
// 	else
// 	{
// 		// Write Errer
// 		return FALSE;
// 	}
// }

long CKeyenceLDS::LDSMeasure(double* pdValue, int nModuleIdx, DWORD dwTimeout)
{
	char pszRecv[255];
	CString strValue;
	double dValue = 0;
	double error = 9999.999;

	int nResult = ReadData(pszRecv, dwTimeout);

	//if (eERR_NONE == nResult)
	if (eCOMM_RESULT_OK == nResult)
	{
		AfxExtractSubString(strValue, (CString)pszRecv, nModuleIdx, _T(','));
		dValue = _ttof(strValue);

		dValue *= m_dDecimalPos;
		dValue += m_dBaseValue;

		*pdValue = dValue;
	}
	else
	{
		// Error
		// AddLog
		*pdValue = error;
		return eCOMM_ERROR_INVALIDDATA;
	}

	return eCOMM_RESULT_OK;
}

long CKeyenceLDS::LDSMeasure(double* pdValue, int nModuleIdx, int* pnStatus, DWORD dwTimeout)
{
	char pszRecv[255];
	CString strValue;
	double dValue = 0;

	int nResult = ReadData(pszRecv, dwTimeout, TRUE);

	//if (eERR_NONE == nResult)
	if (eCOMM_RESULT_OK == nResult)
	{
		AfxExtractSubString(strValue, (CString)pszRecv, (nModuleIdx * 2), _T(','));
		*pnStatus = _ttoi(strValue);

		AfxExtractSubString(strValue, (CString)pszRecv, (nModuleIdx * 2) + 1, _T(','));
		dValue = _ttof(strValue);

		dValue *= m_dDecimalPos;
		dValue += m_dBaseValue;

		*pdValue = dValue;
	}
	else
	{
		// Error
		// AddLog
		*pdValue = 9999.999;
		return eCOMM_ERROR_INVALIDDATA;
	}

	return eCOMM_RESULT_OK;
}

void CKeyenceLDS::LDSMultiMeasure(int nModuleCnt, double* pdValue, DWORD dwTimeout)
{
	char pszRecv[255];
	CString strValue;

	int nResult = ReadData(pszRecv, dwTimeout);

	//if (eERR_NONE == nResult)
	if (eCOMM_RESULT_OK == nResult)
	{
		int nIdx = 0;

		for (nIdx = 0; nIdx < nModuleCnt; nIdx++)
		{
			AfxExtractSubString(strValue, (CString)pszRecv, nIdx, _T(','));

			pdValue[nIdx] = _ttof(strValue);
			pdValue[nIdx] *= m_dDecimalPos;
			pdValue[nIdx] += m_dBaseValue;
		}
	}
	else
	{
		// Error
	}
}