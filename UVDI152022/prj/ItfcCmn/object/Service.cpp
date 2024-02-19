
/*
 desc : Windows Service ���
*/

#include "pch.h"
#include "Service.h"

/* Windows Common Header */
#include <lmapibuf.h>
#include <lmerr.h>
#include <lmwksta.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <winsvc.h>	/* Windows service related header file */

#pragma comment (lib, "winmm")
#pragma comment (lib, "version")

#ifdef _DEBUG
#define new	DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : Constructor
 parm : None
 retn : None
*/
CService::CService()
{
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CService::~CService()
{
}

/*
 desc : Ư�� ���� ���� ����
 parm : svc_name- Service names
 retn : TRUE or FALSE
*/
BOOL CService::StopSvc(PTCHAR svc_name)
{
	SERVICE_STATUS stSS	= {NULL};

	/* Service Manager�� ���� ���� */
	SC_HANDLE hScm	= OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hScm)	return FALSE;

	/* Service Name�� ���� ���� */
	SC_HANDLE hSvc	= OpenService(hScm, svc_name, SERVICE_ALL_ACCESS);
	if (!hSvc)	return FALSE;
	{
		CloseServiceHandle(hScm);
		
	}

	/* Service Status ��� */
	/*bSucc	= */ControlService(hSvc, SERVICE_CONTROL_STOP, &stSS);

	/* Service Handler �ݱ� */
	CloseServiceHandle(hSvc);
	CloseServiceHandle(hScm);

	return TRUE;
}

/*
 desc : Ư�� ���� ���� ����
 parm : svc_name- Service names
 retn : TRUE or FALSE
*/
BOOL CService::StartSvc(PTCHAR svc_name)
{
	BOOL bSucc			= FALSE;
	SERVICE_STATUS stSS	= {NULL};

	/* Service Manager�� ���� ���� */
	SC_HANDLE hScm	= OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hScm)
	{
#ifndef _DEBUG
		AfxMessageBox(L"Can't open the service manager", MB_ICONSTOP|MB_TOPMOST);
#endif
		return FALSE;
	}
	/* Service Name�� ���� ���� */
	SC_HANDLE hSvc	= OpenService(hScm, svc_name, SERVICE_ALL_ACCESS);
	if (!hSvc)
	{
#ifndef _DEBUG
		AfxMessageBox(L"Can't open the service", MB_ICONSTOP|MB_TOPMOST);
#endif
		CloseServiceHandle(hScm);
		return FALSE;
	}

	/* Service Status ��� */
	bSucc	= StartService(hSvc, 0, NULL);
	if (!bSucc)
	{
#ifndef _DEBUG
		AfxMessageBox(L"Can't start the service", MB_ICONSTOP|MB_TOPMOST);
#endif
	}

	/* Service Handler �ݱ� */
	CloseServiceHandle(hSvc);
	CloseServiceHandle(hScm);

	return bSucc;
}

/*
 desc : CPU�� ĳ�ö����� ũ�� ��� (1�� ĳ��?)
 parm : None
 retn : ĳ�� ���� ũ�� ��ȯ (����: bytes). ������ ��� 0 �� ��ȯ
*/
UINT32 CService::GetCPUCacheLineSize()
{
	DWORD u32RetLength	= 0;
	SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX stSysInfo;

	// CPU ���� ���
	if (GetLogicalProcessorInformationEx(RelationCache, &stSysInfo, &u32RetLength))
	{
		return stSysInfo.Cache.LineSize;
	}
	return 0;
}

/*
 desc : ���μ��� ID�� ���� �ڵ� ���
 parm : pid	- [in]  ���μ��� ID
 retn : ���� �ڵ� �� ��ȯ or NULL
*/
HWND CService::GetWindowHwnd(UINT64 pid)
{
	HWND hWnd	= FindWindow(NULL,NULL); // �ֻ��� ������ �ڵ� ã��   
	DWORD dwPID	= 0;

	// �ֻ��� �ڵ����� üũ, ��ư � �ڵ��� ���� �� �����Ƿ� �����ϱ� ����
	if (!GetParent(hWnd)) return NULL;
	/* ó������ ������ �˻� ���� */
	while (hWnd)   
	{   
		/* ���� �ڵ�� Process ID ��� */
		GetWindowThreadProcessId(hWnd, &dwPID);
		/* ������ ���� �ڵ����� �� */
		if (pid ==  dwPID)	return hWnd;
		// ���� ������ �ڵ� ã��
		hWnd = GetWindow(hWnd, GW_HWNDNEXT);
	}

	return NULL;
}

/*
 desc : Ư�� ���� ���� ����
 parm : svc_name- Service names
		status	- [in]  0x00 - Start, 0x01 - Stop, 0x02 - Pause
 retn : TRUE (���� ��) or FALSE (���� �� �ƴ�)
*/
BOOL CService::IsServiceStatus(PTCHAR svc_name, UINT8 status)
{
	BOOL bSucc			= FALSE;
	SERVICE_STATUS stSS	= {NULL};

	/* Service Manager�� ���� ���� */
	SC_HANDLE hScm	= OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hScm)
	{
		return FALSE;
	}
	/* Service Name�� ���� ���� */
	SC_HANDLE hSvc	= OpenService(hScm, svc_name, SERVICE_ALL_ACCESS);
	if (!hSvc)
	{
		CloseServiceHandle(hSvc);
		return FALSE;
	}

	/* Service Status ��� */
	bSucc	= QueryServiceStatus(hSvc, &stSS);

	/* Service Status�� ���� ������ Ȯ�� */
	if (bSucc)
	{
		switch (status)
		{
		case 0x00	:	bSucc	= stSS.dwCurrentState == SERVICE_RUNNING;	break;	/* Is Start ? */
		case 0x01	:	bSucc	= stSS.dwCurrentState == SERVICE_STOPPED;	break;	/* Is Stop ? */
		case 0x02	:	bSucc	= stSS.dwCurrentState == SERVICE_PAUSED;	break;	/* Is Paused ? */
		}
	}

	/* Service Handler �ݱ� */
	CloseServiceHandle(hSvc);
	CloseServiceHandle(hScm);

	return bSucc;
}

/*
 desc : ���μ��� �̸����� �˻� �� �ش� ���α׷� ���� ����
 parm : proc_name	- [in]  ��� �� Ȯ���ڰ� ���Ե� ���μ��� �̸�
		wait_time	- [in]  ���α׷� ���� �� ���������� �ε� �� ������ ��� �ð� (����: msec)
 retn : TRUE or FALSE
*/
BOOL CService::RunProcess(PTCHAR proc_name, UINT32 wait_time)
{
	STARTUPINFO stSI		= {NULL};
	PROCESS_INFORMATION stPI= {NULL};

	stSI.cb	= sizeof(STARTUPINFO);
	if (!::CreateProcess(proc_name, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &stSI, &stPI))
		return FALSE;

	/* ���������� ����� ������ ��� (3 �� ���� ���) */
	WaitForSingleObject(stPI.hProcess, wait_time);

	/* �ڵ� ���� */
	CloseHandle(stPI.hProcess);
	CloseHandle(stPI.hThread);

	return TRUE;
}

/*
 desc : ���μ��� �̸����� �˻� �� ���� ����
 parm : proc_name	- [in]  Ȯ���ڰ� ���Ե� ���μ��� �̸�
 retn : TRUE or FALSE
*/
BOOL CService::FindProcess(PTCHAR proc_name)
{
	TCHAR tzProcName[_MAX_FNAME]	= {NULL};
	TCHAR tzFindName[_MAX_FNAME]	= {NULL};
	BOOL bContinue		= FALSE, bFind = FALSE;
	HANDLE hSnapshot	= INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe32	= {NULL};

	/* ������ �ҹ��� */
	swprintf_s(tzProcName, _MAX_FNAME, L"%s", proc_name);
	_wcslwr_s(tzProcName, _tcslen(tzProcName)+1);

	/* ������ �ڵ� ��� */
	hSnapshot = CreateToolhelp32Snapshot (TH32CS_SNAPPROCESS, 0);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		pe32.dwSize	=sizeof(PROCESSENTRY32);
		if (Process32First(hSnapshot, &pe32))
		{
			/* ���μ��� ��� �˻� ���� */
			do {

				// ���μ��� ��� �� ���� ���μ��� �̸�;
				swprintf_s(tzFindName, _MAX_FNAME, L"%s", pe32.szExeFile);
				_wcslwr_s(tzFindName, _tcslen(tzFindName)+1);
				if (0 == wcscmp(tzProcName, tzFindName))
				{
					bFind = TRUE;	/* �˻� ���� */
					break;
				}

				/* Ȥ�� ������ ���μ����� ������ ����Ǿ� �ִ��� Ȯ���ϱ� ���ؼ� */
				bContinue = Process32Next(hSnapshot, &pe32);

			}	while (bContinue);

		}
		CloseHandle(hSnapshot);
	}

	return bFind;
}

/*
 desc : ���μ��� �̸����� �˻� �� �ش� ���α׷� ���� ����
 parm : proc_name	- [in]  Ȯ���ڰ� ���Ե� ���μ��� �̸�
 retn : None
*/
VOID CService::KillProcess(PTCHAR proc_name)
{
	TCHAR tzProcName[_MAX_FNAME]	= {NULL};
	TCHAR tzFindName[_MAX_FNAME]	= {NULL};
	BOOL bContinue		= FALSE;
	DWORD dwExitCode	= 0;
	HANDLE hSnapshot	= INVALID_HANDLE_VALUE;
	HANDLE hFindProc	= INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe32	= {NULL};

	/* ������ �ҹ��� */
	swprintf_s(tzProcName, _MAX_FNAME, L"%s", proc_name);
	_wcslwr_s(tzProcName, _tcslen(tzProcName)+1);

	/* ������ �ڵ� ��� */
	hSnapshot = CreateToolhelp32Snapshot (TH32CS_SNAPPROCESS, 0);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		pe32.dwSize	=sizeof(PROCESSENTRY32);
		if (Process32First(hSnapshot, &pe32))
		{
			/* ���μ��� ��� �˻� ���� */
			do {

				// ���μ��� ��� �� ���� ���μ��� �̸�;
				swprintf_s(tzFindName, _MAX_FNAME, L"%s", pe32.szExeFile);
				_wcslwr_s(tzFindName, _tcslen(tzFindName)+1);
				if (0 == wcscmp(tzProcName, tzFindName))
				{
					// ���μ��� �ڵ� ���
					hFindProc = OpenProcess(PROCESS_ALL_ACCESS, 0, pe32.th32ProcessID);
					if (hFindProc)
					{
						dwExitCode	= 0;
						GetExitCodeProcess(hFindProc, &dwExitCode);
						/* ���μ��� ���� ���� */
						TerminateProcess(hFindProc, dwExitCode);
					}
				}
				/* Ȥ�� ������ ���μ����� ������ ����Ǿ� �ִ��� Ȯ���ϱ� ���ؼ� */
				bContinue = Process32Next(hSnapshot, &pe32);

			}	while (bContinue);

		}
		CloseHandle(hSnapshot);
	}
}

/*
 desc : Service ����� (���� ���� -> ���� Ȯ�� -> �ٽ� ����)
 parm : None
 retn : TRUE or FALSE
 note : �ݵ�� �ý����� Administrator �������� �α��εǾ� �־�� ��
*/
BOOL CService::RestartService(PTCHAR service_name)
{
	UINT64 u64Tick	= GetTickCount64();

	/* Service�� ���� ������ Ȯ�� */
	if (IsServiceStatus(service_name, 0x00) ||	/* ���� ���� �� */
		IsServiceStatus(service_name, 0x02))		/* �Ͻ� ���� �� */
	{
		/* Service�� �������� ���� */
		if (!StopSvc(service_name))
		{
#ifndef _DEBUG
			AfxMessageBox(L"Failed to init the service <Stop>", MB_ICONSTOP|MB_TOPMOST);
#endif
			return TRUE;	/* ���α׷� ���� ���Ḧ ���� ���� TRUE ���� */
		}
		/* Service ���� ���� Ȯ�� */
		do	{
			/* �����ð� ��� */
			Sleep(100);
			/* ���� Service�� ������ �������� Ȯ�� */
			if (IsServiceStatus(service_name, 0x01))	break;

			/* ���� �ð����� ��� �߿��� ���񽺰� �������� ���� ���, ���� ó�� */
			if (GetTickCount64() > u64Tick + 5000)
			{
#ifndef _DEBUG
				AfxMessageBox(L"Service initialization timed out", MB_ICONSTOP|MB_TOPMOST);
#endif
				return TRUE;	/* ���α׷� ���� ���Ḧ ���� ���� TRUE ���� */
			}

		}	while (1);
	}
	/* Service ���� ���·� ���� */
	return StartSvc(service_name);
}

/*
 desc : �ϵ� ��ũ �뷮 ���� ��ȯ (����: Bytes)
 parm : total	- [out] �� �뷮 ��ȯ
		free	- [out] ���� �ִ� �뷮 ��ȯ
		drive	- [in]  ����̺� ��Ī (��Ʈ �̸� ��, "C:\\" or "D:\\", ...)
 retn : TRUE or FALSE
*/
BOOL CService::GetHDDInfo(UINT64 &total, UINT64 &free, PTCHAR drive)
{
	UINT32 u32DiskType	= DRIVE_UNKNOWN;

	ULARGE_INTEGER lFreeBytesAvailableToCaller;    
	ULARGE_INTEGER lTotalNumberOfBytes;
	ULARGE_INTEGER lTotalNumberOfFreeBytes;

	u32DiskType = GetDriveType(drive);
	if (u32DiskType != DRIVE_FIXED)   return FALSE;

	if(!GetDiskFreeSpaceEx(drive,
						   &lFreeBytesAvailableToCaller,
						   &lTotalNumberOfBytes,
						   &lTotalNumberOfFreeBytes ))
		return FALSE;

	/* �뷮 ���� ��ȯ */
	total	= lTotalNumberOfBytes.QuadPart;
	free	= lTotalNumberOfFreeBytes.QuadPart;

	return TRUE;
}

/*
 desc : �ý����� �޸� ��Ȳ �� ��ȯ
 parm : mem	- [out] �޸� ������ ����Ǵ� ����ü ���� ������
 retn : TRUE or FALSE
*/
BOOL CService::GetMemStatus(MEMORYSTATUSEX &mem)
{
	// �Ʒ� ���� ���� �ݵ�� �ʱ�ȭ ���� ��� ��
	mem.dwLength	= sizeof(MEMORYSTATUSEX);

	return GlobalMemoryStatusEx(&mem) == TRUE ? TRUE : FALSE;
}

/*
 desc : CPU�� ���� ��ü ������ ���� ��ȯ
 parm :	None
 retn : ��ü ������ ���� ��
*/
UINT32 CService::GetNumberOfThread()
{
	SYSTEM_INFO stSysInfo	= {NULL};

	// CPU ������ �˾ƿ��� ���ؼ�. ��, Ŭ���̾�Ʈ �����带 ���������� �����ϱ� ����
	GetSystemInfo(&stSysInfo);
	// ���Ŀ� ���� ������ ���� ��ȯ
	return stSysInfo.dwNumberOfProcessors;	/* CPU Core ������ �ƴ϶�, Thread Counting */
}

/*
 desc : ���� OS ���� ������
 parm : major	- [out] Major version ��ȯ ���� ����
		minor	- [out] Minor version ��ȯ ���� ����
 retn : TRUE or FALSE
*/
BOOL CService::GetOSVer(UINT32 &major, UINT32 &minor)
{
	UINT32 u32Major = 0, u32Minor = 0;
	LPWKSTA_INFO_100 pstInfo = NULL;

	if (NERR_Success != NetWkstaGetInfo(NULL, 100, (LPBYTE*)&pstInfo))	return FALSE;

	major = u32Major = pstInfo->wki100_ver_major;
	minor = u32Minor = pstInfo->wki100_ver_minor;
	NetApiBufferFree(pstInfo);

	return TRUE;
}

/*
 desc : ���� ���μ����� �޸� ��뷮 Ȯ��
 parm : None
 retn : ��뷮 ��ȯ (����: Bytes)
*/
UINT64 CService::GetProcessMemoryUsage()
{
	HANDLE hProc;
	PROCESS_MEMORY_COUNTERS_EX stPMC	= {NULL};

	/* Print information about the memory usage of the process */
	hProc	= OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
	if (!hProc)	return 0;
	/* �޸� ũ�� ��ȯ */
	stPMC.cb	= sizeof(stPMC);
	GetProcessMemoryInfo(hProc, (PROCESS_MEMORY_COUNTERS *)&stPMC, sizeof(stPMC));
	/* �޸� �ڵ� �ݱ� */
	CloseHandle(hProc);

	/* Bytes�� ��ȯ */
	return stPMC.PrivateUsage;
}

/*
 desc : ���� ���μ����� �޸� ��뷮 Ȯ��
 parm : type	- [in]  0x00 : Bytes, 0x01 : KBytes, 0x02 : MBytes, 0x03 : GBytes
 retn : ��뷮 ��ȯ
*/
DOUBLE CService::GetProcessMemoryUsageSize(UINT8 type)
{
	HANDLE hProc;
	PROCESS_MEMORY_COUNTERS_EX stPMC	= {NULL};

	/* Print information about the memory usage of the process */
	hProc	= OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
	if (!hProc)	return 0;
	/* �޸� ũ�� ��ȯ */
	stPMC.cb	= sizeof(stPMC);
	GetProcessMemoryInfo(hProc, (PROCESS_MEMORY_COUNTERS *)&stPMC, sizeof(stPMC));
	/* �޸� �ڵ� �ݱ� */
	CloseHandle(hProc);

	/* Bytes�� ��ȯ */
	switch (type)
	{
	case 0x01	:	return stPMC.PrivateUsage / 1024.0f;
	case 0x02	:	return stPMC.PrivateUsage / (1024.0f * 1024.0f);
	case 0x03	:	return stPMC.PrivateUsage / (1024.0f * 1024.0f * 1024.0f);
	}
	/* �⺻ ���� Bytes */
	return DOUBLE(stPMC.PrivateUsage);
}

/*
 desc : ���� ���μ����� ���� ���� ���� ��ȯ
 parm : path_name	- [out] ���� ���� ��ΰ� ����� ��� ��ȯ
		path_size	- [in]  'path_name'�� ���� ũ��
		file_name	- [out] ���� ���� �̸�(Ȯ���� ����) ��ȯ
		file_size	- [in]  'file_name'�� ���� ũ��
 retn : None
*/
VOID CService::GetProcesssName(PTCHAR path_name, UINT32 path_size, PTCHAR file_name,
							   UINT32 file_size)
{
	TCHAR *pFirstIdx, *pSecondIdx, tzBuffer[MAX_PATH_LEN]	= {NULL};
	UINT32 u32PathLen, u32FileLen;

	// ���� ���μ����� ���������� ��ü ���(���ϸ� ����) ���
	GetModuleFileName(NULL, tzBuffer, MAX_PATH_LEN);
	// ���ϸ��� ���۰� ����Ǵ� ��ġ �˻�
	pFirstIdx	= wcsrchr(tzBuffer, '\\');
	pSecondIdx	= wcsrchr(tzBuffer, '\0');
	// �������� ���� ���ϱ�
	u32PathLen	= UINT32(pFirstIdx - tzBuffer);
	// ���ϸ��� ���� ���ϱ� (Ȯ���� ����)
	u32FileLen	= UINT32(pSecondIdx- (pFirstIdx + 1));

	// ��� �� ���ϸ� ����
	if (path_name)
	{
		if (path_size-1 < u32PathLen)	u32PathLen	= path_size - 1;
		wmemcpy_s(path_name, path_size, tzBuffer,	u32PathLen);
	}
	if (file_name)
	{
		if (file_size-1 < u32FileLen)	u32PathLen	= file_size - 1;
		wmemcpy_s(file_name, file_size, pFirstIdx+1,	u32FileLen);
	}
}

/*
 desc : ���� ���μ����� ��� ��ȯ
 parm : path_name	- [out] ���� ���� ��ΰ� ����� ��� ��ȯ
		path_size	- [in]  'path_name'�� ���� ũ��
 retn : None
*/
VOID CService::GetProcesssPath(PTCHAR path_name, UINT32 path_size)
{
	TCHAR *pFirstIdx, *pSecondIdx, tzBuffer[MAX_PATH_LEN]	= {NULL};
	UINT32 u32PathSize;

	// ���� ���μ����� ���������� ��ü ���(���ϸ� ����) ���
	GetModuleFileName(NULL, tzBuffer, MAX_PATH_LEN);
	// ���ϸ��� ���۰� ����Ǵ� ��ġ �˻�
	pFirstIdx	= wcsrchr(tzBuffer, '\\');
	pSecondIdx	= wcsrchr(tzBuffer, '\0');
	// �������� ���� ���ϱ�
	u32PathSize	= UINT32(pFirstIdx - tzBuffer);

	// ��� �� ���ϸ� ����
	if (path_name)
	{
		if (path_size-1 < u32PathSize)	u32PathSize	= path_size - 1;
		wmemcpy_s(path_name, path_size, tzBuffer,	u32PathSize);
	}
}

/*
 desc : ���� ���� ���� �ڽ��� ���μ��� ���� �˾ƿ���
 parm : major1,2- [out] Major Version
		minor1,2- [out] Minor Version
 retn : None
*/
VOID CService::GetRunProcVerion(UINT32 &major1, UINT32 &major2, UINT32 &minor1, UINT32 &minor2)
{
	TCHAR tzFile[1024]		= {NULL};
	UINT8 *pVerInfo			= NULL;
	DWORD u32Len, u32Handle;
	UINT32 u32VerLen;
	VS_FIXEDFILEINFO stVI	= {NULL};
	LPVOID pVerPtr			= NULL;

	// ���� ������ ���� ���� ���
	GetModuleFileName(NULL, tzFile, sizeof(tzFile));
	u32Len	= ::GetFileVersionInfoSize(tzFile, &u32Handle);
	if (u32Len < 1)	return;

	// ���� ������ �� ���� �Ҵ�
	pVerInfo	= new UINT8 [u32Len+1];
	pVerInfo[u32Len]	= 0x00;

	// ���� ���� ���
	if (::GetFileVersionInfo(tzFile, NULL, u32Len, pVerInfo))
	{
		if (::VerQueryValue(pVerInfo, _T("\\"), &pVerPtr, &u32VerLen))
		{
			stVI = *(VS_FIXEDFILEINFO*)pVerPtr;
			// Major1, 2
			major1	= UINT32(stVI.dwFileVersionMS >> 16);
			major2	= UINT32((stVI.dwFileVersionMS << 16) >> 16);
			// Minor1, 2
			minor1	= UINT32(stVI.dwFileVersionLS >> 16);
			minor2	= UINT32((stVI.dwFileVersionLS << 16) >> 16);
		}
	}

	// ���� ���� �޸� ����
	delete [] pVerInfo;
	pVerInfo	= NULL;
}
