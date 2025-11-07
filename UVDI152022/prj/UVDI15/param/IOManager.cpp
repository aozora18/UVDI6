#include "pch.h"
#include "IOManager.h"
#include <locale.h>
#include <bitset>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

UINT WaitThread(LPVOID lpParam)
{
	CIOManager* pIOMgr = (CIOManager*)lpParam;
	if (pIOMgr->SetQUICKIO(pIOMgr->m_QuickIndex) == FALSE)
	{
		AfxMessageBox(_T("QuickIO Error!"));
	}
	
	pIOMgr->m_QuickIndex = -1;
	return 0;
}

CIOManager::CIOManager()
{
	m_strIOPath = g_tzWorkDir;
	//m_strIOPath.append(_T("\\data\\IO\\"));
	CString strPath;
	strPath.Format(_T("\\%s\\io\\"), CUSTOM_DATA_CONFIG);
	m_strIOPath.append(strPath);
	m_hMainWnd = NULL;
	m_QuickIndex = -1;
	m_pWaitThread = NULL;
}


CIOManager::~CIOManager()
{
	m_pWaitThread = NULL;
	delete m_pWaitThread;
}

void CIOManager::Init(HWND hWnd)
{
	m_hMainWnd = hWnd;

	LoadIOList();
	LoadQUICKIOList();
}

void CIOManager::Destroy()
{
	for (int i = 0; i < eIO_MAX; i++)
	{
		m_vIO[i].clear();
	}

	if (m_pWaitThread != NULL)
	{
		WaitForSingleObject(m_pWaitThread->m_hThread, INFINITE);
	}
}

std::wstring CIOManager::GetIOPath()
{
	return m_strIOPath;
}

BOOL CIOManager::LoadIOList()
{
	CString strValue;
	CString strLine;
	CString strPath;
	CStdioFile clsFile;
	int nCol = 0;
	CString strTitle;
	EN_IO_TYPE eType = eIO_INPUT;
	strPath.Format(_T("%sIO_MAP.csv"), m_strIOPath.c_str());

	setlocale(LC_ALL, "korean");

	if (clsFile.Open(strPath, CStdioFile::modeRead | CStdioFile::shareDenyNone))
	{
		clsFile.ReadString(strTitle);  // Menu;

		while (clsFile.ReadString(strLine))
		{
			nCol = 0;
			// Addr
			if (FALSE == AfxExtractSubString(strValue, strLine, nCol++, ','))
			{
				break;
			}

			ST_IO stIO;
			if (strValue.IsEmpty())
			{
				nCol++;
				nCol++;
				nCol++;
				nCol++;
				nCol++;
				eType = eIO_OUTPUT;
			}
			else
			{
				eType = eIO_INPUT;
			}

			while (eType < eIO_MAX)
			{
				switch (eType)
				{
				case eIO_INPUT:
					stIO.nIndex = (int)m_vIO[eType].size();
					stIO.strAddr = strValue;
					// Name
					if (FALSE == AfxExtractSubString(strValue, strLine, nCol++, ','))
					{
						AfxMessageBox(_T("IO_MAP.csv Check Format!"));
						return FALSE;
					}
					stIO.strName = strValue;
					// On Colar
					if (FALSE == AfxExtractSubString(strValue, strLine, nCol++, ','))
					{
						AfxMessageBox(_T("IO_MAP.csv Check Format!"));
						return FALSE;
					}
					stIO.strClrOn = strValue;
					// On 
					if (FALSE == AfxExtractSubString(strValue, strLine, nCol++, ','))
					{
						AfxMessageBox(_T("IO_MAP.csv Check Format!"));
						return FALSE;
					}
					stIO.strOn = strValue;
					// Off Colar
					if (FALSE == AfxExtractSubString(strValue, strLine, nCol++, ','))
					{
						AfxMessageBox(_T("IO_MAP.csv Check Format!"));
						return FALSE;
					}
					stIO.strClrOff = strValue;
					// Off 
					if (FALSE == AfxExtractSubString(strValue, strLine, nCol++, ','))
					{
						AfxMessageBox(_T("IO_MAP.csv Check Format!"));
						return FALSE;
					}
					stIO.strOff = strValue;
					m_vIO[eType].push_back(stIO);
					eType = eIO_OUTPUT;
					break;
				case eIO_OUTPUT:
					stIO.nIndex = (int)m_vIO[eType].size();
					// Addr
					if (FALSE == AfxExtractSubString(strValue, strLine, nCol++, ','))
					{
						eType = eIO_MAX;
						break;
					}
					if (strValue.IsEmpty())
					{
						eType = eIO_MAX;
						break;
					}
					stIO.strAddr = strValue;
					// Name
					if (FALSE == AfxExtractSubString(strValue, strLine, nCol++, ','))
					{
						AfxMessageBox(_T("IO_MAP.csv Check Format!"));
						return FALSE;
					}
					stIO.strName = strValue;
					// On Colar
					if (FALSE == AfxExtractSubString(strValue, strLine, nCol++, ','))
					{
						AfxMessageBox(_T("IO_MAP.csv Check Format!"));
						return FALSE;
					}
					stIO.strClrOn = strValue;
					// On 
					if (FALSE == AfxExtractSubString(strValue, strLine, nCol++, ','))
					{
						AfxMessageBox(_T("IO_MAP.csv Check Format!"));
						return FALSE;
					}
					stIO.strOn = strValue;
					// Off Colar
					if (FALSE == AfxExtractSubString(strValue, strLine, nCol++, ','))
					{
						AfxMessageBox(_T("IO_MAP.csv Check Format!"));
						return FALSE;
					}
					stIO.strClrOff = strValue;
					// Off 
					if (FALSE == AfxExtractSubString(strValue, strLine, nCol++, ','))
					{
						AfxMessageBox(_T("IO_MAP.csv Check Format!"));
						return FALSE;
					}
					stIO.strOff = strValue;
					// Quick 
					if (FALSE == AfxExtractSubString(strValue, strLine, nCol++, ','))
					{
						AfxMessageBox(_T("IO_MAP.csv Check Format!"));
						return FALSE;
					}
					stIO.bIsQuick = _ttoi(strValue);
					// Match Input 
// 					if (FALSE == AfxExtractSubString(strValue, strLine, nCol++, ','))
// 					{
// 						AfxMessageBox(_T("IO_MAP.csv Check Format!"));
// 						return FALSE;
// 					}
// 					
// 					if (FALSE == strValue.IsEmpty())
// 					{
// 						CString strEachValue;
// 						while (AfxExtractSubString(strEachValue, strValue, (int)stIO.vMatchingInput.size(), _T('/')))
// 						{
// 							if (strEachValue.IsEmpty())
// 							{
// 								break;
// 							}
// 
// 							ST_IO_CMD stCmd;
// 
// 							if (0 == strEachValue.Find('!'))
// 							{
// 								stCmd.bReverse = TRUE;
// 								strEachValue.Delete(0);
// 							}
// 							
// 							stCmd.strAddr = strEachValue;
// 							stIO.vMatchingInput.push_back(stCmd);
// 						}
// 					}
// 
// 					// Mapping Output 
// 					if (FALSE == AfxExtractSubString(strValue, strLine, nCol++, ','))
// 					{
// 						AfxMessageBox(_T("IO_MAP.csv Check Format!"));
// 						return FALSE;
// 					}
// 
// 					if (FALSE == strValue.IsEmpty())
// 					{
// 						CString strEachValue;
// 						while (AfxExtractSubString(strEachValue, strValue, (int)stIO.vMappingOutput.size(), '/'))
// 						{
// 							if (strEachValue.IsEmpty())
// 							{
// 								break;
// 							}
// 
// 							ST_IO_CMD stCmd;
// 
// 							if (0 == strEachValue.Find('!'))
// 							{
// 								stCmd.bReverse = TRUE;
// 								strEachValue.Delete(0);
// 							}
// 
// 							stCmd.strAddr = strEachValue;
// 							stIO.vMappingOutput.push_back(stCmd);
// 						}
// 					}
// 
// 					// Mapping InterLockIO
// 					if (FALSE == AfxExtractSubString(strValue, strLine, nCol++, ','))
// 					{
// 						AfxMessageBox(_T("IO_MAP.csv Check Format!"));
// 						return FALSE;
// 					}
// 					if (FALSE == strValue.IsEmpty())
// 					{
// 						CString strEachValue;
// 						while (AfxExtractSubString(strEachValue, strValue, (int)stIO.vMappingInterLockIO.size(), '/'))
// 						{
// 							if (strEachValue.IsEmpty())
// 							{
// 								break;
// 							}
// 
// 							ST_IO_CMD stCmd;
// 
// 							if (0 == strEachValue.Find('!'))
// 							{
// 								stCmd.bReverse = TRUE;
// 								strEachValue.Delete(0);
// 							}
// 
// 							stCmd.strAddr = strEachValue;
// 							stIO.vMappingInterLockIO.push_back(stCmd);
// 						}
// 					}
					
					m_vIO[eType].push_back(stIO);
					eType = eIO_MAX;
					break;
				default:
					eType = eIO_MAX;
					break;
				}
			}
		}

		clsFile.Close();
	}
	else
	{
		CString strMsg;
		strMsg.Format(_T("[%s] File Open Failed!"), strPath);
		AfxMessageBox(strMsg);
		return FALSE;
	}

	for (auto& io : m_vIO[EN_IO_TYPE::eIO_OUTPUT])
	{
		for (auto& matchinput : io.vMatchingInput)
		{
			for (const auto& input : m_vIO[EN_IO_TYPE::eIO_INPUT])
			{
				if (0 == matchinput.strAddr.compare(input.strAddr))
				{
					matchinput.nIndex = input.nIndex;
					break;
				}
			}
		}

		for (auto& mappingoutput : io.vMappingOutput)
		{
			for (const auto& output : m_vIO[EN_IO_TYPE::eIO_OUTPUT])
			{
				if (0 == mappingoutput.strAddr.compare(output.strAddr))
				{
					mappingoutput.nIndex = output.nIndex;
					break;
				}
			}
		}

		for (auto& mappingInterLock : io.vMappingInterLockIO)
		{
			for (const auto& output : m_vIO[EN_IO_TYPE::eIO_OUTPUT])
			{
				if (0 == mappingInterLock.strAddr.compare(output.strAddr))
				{
					mappingInterLock.nIndex = output.nIndex;
					break;
				}
			}
		}
	}

	return TRUE;
}

BOOL CIOManager::LoadQUICKIOList()
{
	CString strValue;
	CString strONOFFValue;
	CString strLine;
	CString strLine2;
	CString strPath;
	CStdioFile clsFile;
	int nCol = 0;
	int nOnOffCol = 0;
	int nIndex = 0;
	//int nOnOffDivide = 0;
	CString strTitle;
	EN_IO_TYPE eType = eIO_INPUT;
	strPath.Format(_T("%sUVDI_QUICK.csv"), m_strIOPath.c_str());

	setlocale(LC_ALL, "korean");

	if (clsFile.Open(strPath, CStdioFile::modeRead | CStdioFile::shareDenyNone))
	{
		clsFile.ReadString(strTitle);  // Menu;
		
		while (clsFile.ReadString(strLine)) //처음 읽은 줄은 ON으로
		{
			clsFile.ReadString(strLine2); // 한줄 더 읽어서 이건 OFF로
			ST_QUICK_IO_ACT vIO;


			nCol = 0;
			nOnOffCol = 0;
			// Addr
			if (FALSE == AfxExtractSubString(strValue, strLine, nCol++, ','))
			{
				break;
			}
			while (AfxExtractSubString(strONOFFValue, strValue, nOnOffCol++, '/'))
			{
				if (nOnOffCol == 1)
				{
// 					CString strTemp;
// 					AfxExtractSubString(strTemp, strONOFFValue, 0, '.');
					vIO.strName = strONOFFValue;
// 					AfxExtractSubString(strTemp, strONOFFValue, 1, '.');
// 					strTemp.Replace(_T("STEP"), _T(""));
// 					nOnOffDivide = _ttoi(strTemp);
				}
				else
				{
					ST_IO_CMD stIOCMD;
					CString strSubValue;
					if (AfxExtractSubString(strSubValue, strONOFFValue, 0, '.'))
					{
						if (0 == strSubValue.Find('!'))
						{
							stIOCMD.bReverse = TRUE;
							strSubValue.Delete(0);
						}

						if (strSubValue.Compare(_T("INPUT")) == 0)
						{
							stIOCMD.bIOType = TRUE;
						}
						else
						{
							stIOCMD.bIOType = FALSE;
						}

					}

					if (AfxExtractSubString(strSubValue, strONOFFValue, 1, '.'))
					{
						stIOCMD.strAddr = strSubValue;
					}

					vIO.vActOnOffIO.push_back(stIOCMD);
				}
				
			}
			vIO.nIndex = nIndex++;

			while (AfxExtractSubString(strValue, strLine, nCol++, ','))
			{
				if (strValue.Compare(_T("")) != 0)
				{
					ST_IO_CMD stIOCMD;
					CString strSubValue;
					if (AfxExtractSubString(strSubValue, strValue, 0, '.'))
					{

						if (0 == strSubValue.Find('!'))
						{
							stIOCMD.bReverse = TRUE;
							strSubValue.Delete(0);
						}

						if (strSubValue.Compare(_T("INPUT")) == 0)
						{
							stIOCMD.bIOType = TRUE;
						}
						else
						{
							stIOCMD.bIOType = FALSE;
						}

					}

					if (AfxExtractSubString(strSubValue, strValue, 1, '.'))
					{
						stIOCMD.strAddr = strSubValue;
					}

					vIO.vActIO.push_back(stIOCMD);
				}
				
			}

			nCol = 0;
			nOnOffCol = 0;
			// Addr
			if (FALSE == AfxExtractSubString(strValue, strLine2, nCol++, ','))
			{
				break;
			}

			while (AfxExtractSubString(strValue, strLine2, nCol++, ','))
			{
				if (strValue.Compare(_T("")) != 0)
				{
					ST_IO_CMD stIOCMD;
					CString strSubValue;
					if (AfxExtractSubString(strSubValue, strValue, 0, '.'))
					{
						if (0 == strSubValue.Find('!'))
						{
							stIOCMD.bReverse = TRUE;
							strSubValue.Delete(0);
						}

						if (strSubValue.Compare(_T("INPUT")) == 0)
						{
							stIOCMD.bIOType = TRUE;
						}
						else
						{
							stIOCMD.bIOType = FALSE;
						}

					}

					if (AfxExtractSubString(strSubValue, strValue, 1, '.'))
					{
						stIOCMD.strAddr = strSubValue;
					}

					vIO.vActIO2.push_back(stIOCMD);
				}
			}
			m_vQuickIO.push_back(vIO);
		}
		
		clsFile.Close();
	}
	else
	{
		CString strMsg;
		strMsg.Format(_T("[%s] File Open Failed!"), strPath);
		AfxMessageBox(strMsg);
		return FALSE;
	}
	return TRUE;
}

std::wstring CIOManager::GetInputName(int nIndex)
{
	
	return _T("");
}

int CIOManager::GetInputIndex(std::wstring strName)
{

	return -1;
}

std::wstring CIOManager::GetOutputName(int nIndex)
{

	return _T("");
}

int CIOManager::GetOutputIndex(std::wstring strName)
{

	return -1;
}


BOOL CIOManager::GetIO(EN_IO_TYPE eType, int nIndex, ST_IO& stIO)
{
	for (const auto& io : m_vIO[eType])
	{
		if (nIndex == io.nIndex)
		{
			stIO = io;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CIOManager::GetIO(EN_IO_TYPE eType, CString strName, ST_IO& stIO)
{
	for (const auto& io : m_vIO[eType])
	{
		if (strName == io.strName.c_str())
		{
			stIO = io;
			return TRUE;
		}
	}

	stIO.nIndex = -1;

	return FALSE;
}

BOOL CIOManager::GetIOStatus(EN_IO_TYPE eType, CString strName)
{
	for (const auto& io : m_vIO[eType])
	{
		if (strName == io.strName.c_str())
		{
			if (io.bOn)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}

	return FALSE;
}

BOOL CIOManager::SetIO(EN_IO_TYPE eType, ST_IO& stIO)
{
	STG_PP_P2C_OUTPUT_ONOFF		stOutput;
	STG_PP_P2C_OUTPUT_ONOFF_ACK	stOutputAck;
	stOutput.Reset();
	stOutputAck.Reset();
	int	nIOIndex = 0;
	

	for (auto& io : m_vIO[eType])
	{
		if (stIO.nIndex == io.nIndex)
		{
			// send philhmi
			stOutput.usIndex	= stIO.nIndex;
			stOutput.usSignal	= stIO.bOn;
			TRACE(_T("IO NAME : %s - ONOFF : %d\n"), stIO.strName.c_str(), stIO.bOn);
			if (uvEng_Philhmi_Send_P2C_OUTPUT_ONOFF(stOutput, stOutputAck))
			{
				if (0 == stOutputAck.usErrorCode)
				{
					io.bOn = stIO.bOn;
				}
			}
		}
	}

	return FALSE;
}

BOOL CIOManager::SetQUICKIO(int nIndex)
{

	ST_QUICK_IO_ACT vQuickIO;

	vQuickIO = m_vQuickIO.at(nIndex);

	BOOL bOnOff = vQuickIO.bOn;

	if (!bOnOff)
	{
		for (const auto& io : vQuickIO.vActIO)
		{

			if (io.bIOType == TRUE) // INPUT
			{
				BOOL bIoStatus;
				bIoStatus = !io.bReverse;
				UINT64 u64SSTime = GetTickCount64();
				while (bIoStatus != GetIOStatus(eIO_INPUT, io.strAddr.c_str()))
				{
					Sleep(50);
					UpdateIO();
					Sleep(50);
					if ((GetTickCount64() - u64SSTime) > 5000)
					{
						return FALSE;
					}
				}

			}
			else					//OUTPUT
			{
				ST_IO stIO;
				GetIO(eIO_OUTPUT, io.strAddr.c_str(), stIO);
				stIO.bOn = !io.bReverse;
				SetIO(eIO_OUTPUT, stIO);
			}
		}
	}
	else
	{
		for (const auto& io : vQuickIO.vActIO2)
		{

			if (io.bIOType == TRUE) // INPUT
			{
				BOOL bIoStatus;
				bIoStatus = !io.bReverse;
				UINT64 u64SSTime = GetTickCount64();
				while (bIoStatus != GetIOStatus(eIO_INPUT, io.strAddr.c_str()))
				{
					Sleep(50);
					UpdateIO();
					Sleep(50);

					if((GetTickCount64() - u64SSTime) > 5000)
					{
						return FALSE;
					}
				}

			}
			else					//OUTPUT
			{
				ST_IO stIO;
				GetIO(eIO_OUTPUT, io.strAddr.c_str(), stIO);
				stIO.bOn = !io.bReverse;
				SetIO(eIO_OUTPUT, stIO);
			}
		}
	}

	

	return TRUE;
}

int CIOManager::GetIOList(EN_IO_TYPE eType, vST_IO& vIOList)
{
	for (const auto& io : m_vIO[eType])
	{
		vIOList.push_back(io);
	}

	return (int)vIOList.size();
}

int CIOManager::GetQuickIOList(vST_IO& vIOList)
{
	for (const auto& io : m_vIO[EN_IO_TYPE::eIO_OUTPUT])
	{
		if (io.bIsQuick)
		{
			vIOList.push_back(io);
		}
	}

	return (int)vIOList.size();
}

int CIOManager::GetQuickIOList(vST_QUICK_IO_ACT& vIOList)
{

	vIOList = m_vQuickIO;

	return (int)m_vQuickIO.size();
}

BOOL CIOManager::UpdateIO()
{
	STG_PP_P2C_IO_STATUS		stStatus;
	STG_PP_P2C_IO_STATUS_ACK	stStatusAck;
	stStatus.Reset();
	stStatusAck.Reset();
	int	nIOIndex = 0;
	if (uvEng_Philhmi_Send_P2C_IO_STATUS(stStatus, stStatusAck))
	{
		nIOIndex = 0;
		for (int nInputGroup = 0; nInputGroup < stStatusAck.byInputCount; nInputGroup++)
		{
			std::vector <bool>	vInput;
			std::bitset<16>		bits(stStatusAck.usInputData[nInputGroup]);
			for (int nInput = 0; nInput < 16; nInput++)
			{
				if (nIOIndex >= m_vIO[EN_IO_TYPE::eIO_INPUT].size())
				{
					break;
				}

				if (bits[nInput])
				{
					m_vIO[EN_IO_TYPE::eIO_INPUT].at(nIOIndex).bOn = TRUE;
				}
				else
				{
					m_vIO[EN_IO_TYPE::eIO_INPUT].at(nIOIndex).bOn = FALSE;
				}

				nIOIndex++;
			}
		}

		nIOIndex = 0;
		for (int nOutputGroup = 0; nOutputGroup < stStatusAck.byOutputCount; nOutputGroup++)
		{
			std::vector <bool>	vOutput;
			std::bitset<16>		bits(stStatusAck.usOutputData[nOutputGroup]);
			for (int nOutput = 0; nOutput < 16; nOutput++)
			{
				if (nIOIndex >= m_vIO[EN_IO_TYPE::eIO_OUTPUT].size())
				{
					break;
				}

				if (bits[nOutput])
				{
					m_vIO[EN_IO_TYPE::eIO_OUTPUT].at(nIOIndex).bOn = TRUE;
				}
				else
				{
					m_vIO[EN_IO_TYPE::eIO_OUTPUT].at(nIOIndex).bOn = FALSE;
				}

				nIOIndex++;
			}
		}
	}

	return TRUE;
}

void CIOManager::UpdateQUICKIO()
{
	UpdateIO();

	for (int i = 0; i < m_vQuickIO.size(); i++)
	{
		BOOL bOn = FALSE;
		for (int j=0; j < m_vQuickIO.at(i).vActOnOffIO.size(); j++)
		{
			if (m_vQuickIO.at(i).vActOnOffIO.at(j).bIOType == TRUE) // INPUT
			{
				BOOL bIoStatus = !m_vQuickIO.at(i).vActOnOffIO.at(j).bReverse;
				if (bIoStatus == GetIOStatus(eIO_INPUT, m_vQuickIO.at(i).vActOnOffIO.at(j).strAddr.c_str()))
				{
					bOn = TRUE;
				}
				else
				{
					bOn = FALSE;
					break;
				}
			}
			else					//OUTPUT
			{
				BOOL bIoStatus = !m_vQuickIO.at(i).vActOnOffIO.at(j).bReverse;
				if (bIoStatus == GetIOStatus(eIO_OUTPUT, m_vQuickIO.at(i).vActOnOffIO.at(j).strAddr.c_str()))
				{
					bOn = TRUE;
				}
				else
				{
					bOn = FALSE;
					break;
				}
			}
		}
		m_vQuickIO.at(i).bOn = bOn;
	}

}

int CIOManager::GetInputList(vST_IO& vIOList)
{
	for (const auto& io : m_vIO[eIO_INPUT])
	{
		vIOList.push_back(io);
	}

	return (int)vIOList.size();
}


int CIOManager::GetOutputList(vST_IO& vIOList)
{
	for (const auto& io : m_vIO[eIO_OUTPUT])
	{
		vIOList.push_back(io);
	}

	return (int)vIOList.size();
}

void CIOManager::SetQuickIOThread(int nIndex)
{
	m_QuickIndex = nIndex;
	m_pWaitThread = AfxBeginThread(WaitThread, this);
}

