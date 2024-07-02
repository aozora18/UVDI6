#include "pch.h"
#include "LogManager.h"
#include <locale.h>
 
#include <tchar.h>
#include <iostream>
#include <regex>

#include <vector>
#include <atlstr.h>
#include <algorithm>

#include <unordered_set>
#include <unordered_map>
#include <algorithm>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CLogManager::CLogManager()
{
	m_nID = 0;
}

CLogManager::~CLogManager(void)
{

}


void CLogManager::InitManager(HWND hWnd)
{
	m_hMainWnd = hWnd;

	InitCreatePath();

}

BOOL CLogManager::InitCreatePath()
{
	TCHAR tzPath[MAX_PATH_LEN] = { NULL };

	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\logs\\DlgFile", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\logs\\DlgFile\\Service", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\logs\\DlgFile\\Control", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\logs\\DlgFile\\Motion", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\logs\\DlgFile\\Library", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\logs\\DlgFile\\Vision", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\logs\\DlgFile\\PhilHmi", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\logs\\DlgFile\\PGFS", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\logs\\DlgFile\\Calibration", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\logs\\DlgFile\\etc", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;

	return TRUE;
}


void CLogManager::Destroy()
{

}

void CLogManager::InitLog(EN_LOG_TYPE type)
{
	m_vOriginLogs[type].clear();
	m_vSyncLogs[type].clear();
}

void CLogManager::SetLog(EN_LOG_TYPE type, UINT nDate, UINT nTime, CString strLog)
{
	ST_LOG_MSG log;
	log.nDate = nDate;
	log.nTime = nTime;
	log.strLog.Format(_T("[%d] %s"),	nDate, strLog);
	m_vOriginLogs[type].push_back(log);
}

vST_LOG_MSG CLogManager::GetLog(EN_LOG_TYPE type, BOOL bSyncView)
{
	if (bSyncView)
	{
		m_vSyncLogs[type] = m_vOriginLogs[type];

		std::unordered_multiset<ST_LOG_MSG, LogMsgHash, LogMsgEqual> existingSyncLogs;
		std::unordered_multiset<ST_LOG_MSG, LogMsgHash, LogMsgEqual> existingOrigLogs;

		for (const auto& syncLog : m_vSyncLogs[type])
		{
			existingSyncLogs.insert({ syncLog.nDate, syncLog.nTime });
		}

		for (const auto& vOriginLogs : m_vOriginLogs)
		{
			existingOrigLogs.clear();

			for (const auto& origLog : vOriginLogs)
			{
				existingOrigLogs.insert({ origLog.nDate, origLog.nTime });			
			}

			for (const auto& origLog : vOriginLogs)
			{
				size_t countS = existingSyncLogs.count({ origLog.nDate, origLog.nTime });
				size_t countO = existingOrigLogs.count({ origLog.nDate, origLog.nTime });

				if (countS < countO)
				{
					ST_LOG_MSG stLog;
					stLog.nDate = origLog.nDate;
					stLog.nTime = origLog.nTime;
					stLog.strLog.Format(_T("[%d] [%02d:%02d:%02d] \n"), origLog.nDate, origLog.nTime / 3600, origLog.nTime % 3600 / 60, origLog.nTime % 60);
					m_vSyncLogs[type].push_back(stLog);
					existingSyncLogs.insert({ origLog.nDate, origLog.nTime });
				}
			}
		}

		//std::unordered_multiset<ST_LOG_MSG, LogMsgHash, LogMsgEqual> existingSyncLogs;
		//
		//for (const auto& log : m_vSyncLogs[type])
		//{
		//	existingSyncLogs.insert({ log.nDate, log.nTime, log.nTimeID });
		//}
		//
		//for (const auto& vOriginLogs : m_vOriginLogs)
		//{
		//	for (const auto& log : vOriginLogs)
		//	{
		//		auto iter = existingSyncLogs.find({ log.nDate, log.nTime, log.nTimeID });
		//		size_t count = existingSyncLogs.count({ log.nDate, log.nTime });
		//		if (iter == existingSyncLogs.end())
		//		{
		//			ST_LOG_MSG stLog;
		//			stLog.nDate = log.nDate;
		//			stLog.nTime = log.nTime;
		//			stLog.nTimeID = log.nTimeID;
		//			stLog.strLog.Format(_T("[%02d:%02d:%02d] \n"), log.nTime / 3600, log.nTime % 3600 / 60, log.nTime % 60/*, log.nTimeID*/);
		//			m_vSyncLogs[type].push_back(stLog);
		//			existingSyncLogs.insert({ log.nDate, log.nTime, log.nTimeID });
		//		}
		//	}
		//}

		std::sort(m_vSyncLogs[type].begin(), m_vSyncLogs[type].end(), [](const ST_LOG_MSG& a, const ST_LOG_MSG& b)
			{
				if (a.nDate == b.nDate)
					return a.nTime < b.nTime;
				else
					return a.nDate < b.nDate;
			});

		return m_vSyncLogs[type];
	}

	return m_vOriginLogs[type];
}

void CLogManager::LoadFileContent(EN_LOG_TYPE type, CString strFileName)
{
	CStdioFile file;
	CString strFile;
	CString strLine;
	CFileException e;

	//strFile.Format(_T("%s\\logs\\%s\\%s"), g_tzWorkDir, m_tzType[type], strFileName);
	strFile.Format(_T("%s\\logs\\DlgFile\\%s\\%s"), g_tzWorkDir, m_tzType[type], strFileName);


	if (file.Open(strFile, CFile::modeRead | CFile::typeUnicode, &e))
	{
		while (file.ReadString(strLine))
		{
			if (strLine.GetLength() > 0)
			{
#if _UTF_8
				// UTF-8 BOM (Byte Order Mark) 제거
				if (strLine.GetLength() >= 3 &&
					(BYTE)strLine[0] == 0xEF &&
					(BYTE)strLine[1] == 0xBB &&
					(BYTE)strLine[2] == 0xBF)
				{
					strLine = strLine.Mid(3);
				}
#endif

#if _UTF_16
				// UTF-16 BOM (Byte Order Mark) 제거
				if (strLine.GetLength() > 0 && strLine[0] == 0xFEFF)
				{
					strLine.Delete(0);
				}
#endif

				// '→' 이후 문자열 제거
				strLine = FilterString(strLine);

				// 로그 추가
				SetLog(type, ExtractDate(strFileName), ExtractTime(strLine), strLine);
			}
		}
		file.Close();
	}
	else
	{
		TCHAR szError[1024];
		e.GetErrorMessage(szError, 1024);
		AfxMessageBox(szError);
	}
}

void CLogManager::LoadFiles(EN_LOG_TYPE type, COleDateTime timeStart, COleDateTime timeEnd)
{
	TCHAR tzDir[MAX_PATH_LEN] = { NULL };
	CString strFile;
	CStringArray arrFiles;
	CStringArray arrDates;
	
	/* 설정 기간 얻기 */
	GetArrDates(timeStart, timeEnd, arrDates);

	/* 경로의 파일 리스트 얻기 */
	//swprintf_s(tzDir, MAX_PATH_LEN, L"%s\\logs\\%s", g_tzWorkDir, m_tzType[type]);
	swprintf_s(tzDir, MAX_PATH_LEN, L"%s\\logs\\DlgFile\\%s", g_tzWorkDir, m_tzType[type]);
	uvCmn_SearchFile(tzDir, L"*.log", arrFiles);

	/* 로그 데이터 초기화 */
	InitLog(type);

	/* 등록된 항목 다시 적재 */
	for (int f = 0; f < arrFiles.GetCount(); f++)
	{
		/* 파일 얻기 */
		strFile = arrFiles.GetAt(f);

		for (int d = 0; d < arrDates.GetCount(); d++)
		{
			/* 설정된 기간의 파일만 검출 */
			if (0 == strFile.Find(arrDates.GetAt(d)))
			{
				LoadFileContent(type, strFile);
			}
		}
	}
}

void CLogManager::GetArrDates(COleDateTime timeStart, COleDateTime timeEnd, CStringArray& arrDates)
{
	COleDateTimeSpan timeSpen(1, 0, 0, 0);
	CString	strDate;

	while (timeStart <= timeEnd)
	{
		strDate = timeStart.Format(_T("%Y%m%d_"));
		arrDates.Add(strDate);

		timeStart += timeSpen;
	}
}

UINT CLogManager::ExtractTime(const CString& str)
{
	int hour, minute, second;

	if (_stscanf_s(str, _T("[%02d:%02d:%02d]"), &hour, &minute, &second) == 3)
	{
		// 시간 정보를 int 단위로 변환
		UINT totalSeconds = (hour * 3600) + (minute * 60) + second;

		return totalSeconds;
	}

	return 0;
}

UINT CLogManager::ExtractDate(const CString& str)
{
	int year, month, day;

	if (_stscanf_s(str, _T("%04d%02d%02d_"), &year, &month, &day) == 3)
	{
		// 날짜 정보를 int 단위로 변환
		UINT dateAsUint = (year * 10000) + (month * 100) + day;

		return dateAsUint;
	}

	return 0;
}

CString  CLogManager::FilterString(const CString& input)
{
	int pos = input.Find(_T("→"));
	if (pos != -1)
	{
		return input.Left(pos) + _T("\n");
	}
	return input;
}