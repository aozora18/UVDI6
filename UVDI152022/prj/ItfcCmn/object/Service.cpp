
/*
 desc : Windows Service 모듈
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
 desc : 특정 서비스 동작 중지
 parm : svc_name- Service names
 retn : TRUE or FALSE
*/
BOOL CService::StopSvc(PTCHAR svc_name)
{
	SERVICE_STATUS stSS	= {NULL};

	/* Service Manager에 대해 열기 */
	SC_HANDLE hScm	= OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hScm)	return FALSE;

	/* Service Name에 대해 열기 */
	SC_HANDLE hSvc	= OpenService(hScm, svc_name, SERVICE_ALL_ACCESS);
	if (!hSvc)	return FALSE;
	{
		CloseServiceHandle(hScm);
		
	}

	/* Service Status 얻기 */
	/*bSucc	= */ControlService(hSvc, SERVICE_CONTROL_STOP, &stSS);

	/* Service Handler 닫기 */
	CloseServiceHandle(hSvc);
	CloseServiceHandle(hScm);

	return TRUE;
}

/*
 desc : 특정 서비스 동작 시작
 parm : svc_name- Service names
 retn : TRUE or FALSE
*/
BOOL CService::StartSvc(PTCHAR svc_name)
{
	BOOL bSucc			= FALSE;
	SERVICE_STATUS stSS	= {NULL};

	/* Service Manager에 대해 열기 */
	SC_HANDLE hScm	= OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hScm)
	{
#ifndef _DEBUG
		AfxMessageBox(L"Can't open the service manager", MB_ICONSTOP|MB_TOPMOST);
#endif
		return FALSE;
	}
	/* Service Name에 대해 열기 */
	SC_HANDLE hSvc	= OpenService(hScm, svc_name, SERVICE_ALL_ACCESS);
	if (!hSvc)
	{
#ifndef _DEBUG
		AfxMessageBox(L"Can't open the service", MB_ICONSTOP|MB_TOPMOST);
#endif
		CloseServiceHandle(hScm);
		return FALSE;
	}

	/* Service Status 얻기 */
	bSucc	= StartService(hSvc, 0, NULL);
	if (!bSucc)
	{
#ifndef _DEBUG
		AfxMessageBox(L"Can't start the service", MB_ICONSTOP|MB_TOPMOST);
#endif
	}

	/* Service Handler 닫기 */
	CloseServiceHandle(hSvc);
	CloseServiceHandle(hScm);

	return bSucc;
}

/*
 desc : CPU의 캐시라인의 크기 얻기 (1차 캐시?)
 parm : None
 retn : 캐시 라인 크기 반환 (단위: bytes). 실패인 경우 0 값 반환
*/
UINT32 CService::GetCPUCacheLineSize()
{
	DWORD u32RetLength	= 0;
	SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX stSysInfo;

	// CPU 정보 얻기
	if (GetLogicalProcessorInformationEx(RelationCache, &stSysInfo, &u32RetLength))
	{
		return stSysInfo.Cache.LineSize;
	}
	return 0;
}

/*
 desc : 프로세스 ID로 윈도 핸들 얻기
 parm : pid	- [in]  프로세스 ID
 retn : 윈도 핸들 값 반환 or NULL
*/
HWND CService::GetWindowHwnd(UINT64 pid)
{
	HWND hWnd	= FindWindow(NULL,NULL); // 최상위 윈도우 핸들 찾기   
	DWORD dwPID	= 0;

	// 최상위 핸들인지 체크, 버튼 등도 핸들을 가질 수 있으므로 무시하기 위해
	if (!GetParent(hWnd)) return NULL;
	/* 처음부터 끝까지 검색 수행 */
	while (hWnd)   
	{   
		/* 윈도 핸들로 Process ID 얻기 */
		GetWindowThreadProcessId(hWnd, &dwPID);
		/* 동일한 윈도 핸들인지 비교 */
		if (pid ==  dwPID)	return hWnd;
		// 다음 윈도우 핸들 찾기
		hWnd = GetWindow(hWnd, GW_HWNDNEXT);
	}

	return NULL;
}

/*
 desc : 특정 서비스 동작 여부
 parm : svc_name- Service names
		status	- [in]  0x00 - Start, 0x01 - Stop, 0x02 - Pause
 retn : TRUE (실행 중) or FALSE (실행 중 아님)
*/
BOOL CService::IsServiceStatus(PTCHAR svc_name, UINT8 status)
{
	BOOL bSucc			= FALSE;
	SERVICE_STATUS stSS	= {NULL};

	/* Service Manager에 대해 열기 */
	SC_HANDLE hScm	= OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hScm)
	{
		return FALSE;
	}
	/* Service Name에 대해 열기 */
	SC_HANDLE hSvc	= OpenService(hScm, svc_name, SERVICE_ALL_ACCESS);
	if (!hSvc)
	{
		CloseServiceHandle(hSvc);
		return FALSE;
	}

	/* Service Status 얻기 */
	bSucc	= QueryServiceStatus(hSvc, &stSS);

	/* Service Status가 동작 중인지 확인 */
	if (bSucc)
	{
		switch (status)
		{
		case 0x00	:	bSucc	= stSS.dwCurrentState == SERVICE_RUNNING;	break;	/* Is Start ? */
		case 0x01	:	bSucc	= stSS.dwCurrentState == SERVICE_STOPPED;	break;	/* Is Stop ? */
		case 0x02	:	bSucc	= stSS.dwCurrentState == SERVICE_PAUSED;	break;	/* Is Paused ? */
		}
	}

	/* Service Handler 닫기 */
	CloseServiceHandle(hSvc);
	CloseServiceHandle(hScm);

	return bSucc;
}

/*
 desc : 프로세스 이름으로 검색 후 해당 프로그램 강제 종료
 parm : proc_name	- [in]  경로 및 확장자가 포함된 프로세스 이름
		wait_time	- [in]  프로그램 실행 후 정상적으로 로드 될 때까지 대기 시간 (단위: msec)
 retn : TRUE or FALSE
*/
BOOL CService::RunProcess(PTCHAR proc_name, UINT32 wait_time)
{
	STARTUPINFO stSI		= {NULL};
	PROCESS_INFORMATION stPI= {NULL};

	stSI.cb	= sizeof(STARTUPINFO);
	if (!::CreateProcess(proc_name, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &stSI, &stPI))
		return FALSE;

	/* 정상적으로 실행될 때까지 대기 (3 초 동안 대기) */
	WaitForSingleObject(stPI.hProcess, wait_time);

	/* 핸들 종료 */
	CloseHandle(stPI.hProcess);
	CloseHandle(stPI.hThread);

	return TRUE;
}

/*
 desc : 프로세스 이름으로 검색 후 존재 여부
 parm : proc_name	- [in]  확장자가 포함된 프로세스 이름
 retn : TRUE or FALSE
*/
BOOL CService::FindProcess(PTCHAR proc_name)
{
	TCHAR tzProcName[_MAX_FNAME]	= {NULL};
	TCHAR tzFindName[_MAX_FNAME]	= {NULL};
	BOOL bContinue		= FALSE, bFind = FALSE;
	HANDLE hSnapshot	= INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe32	= {NULL};

	/* 무조건 소문자 */
	swprintf_s(tzProcName, _MAX_FNAME, L"%s", proc_name);
	_wcslwr_s(tzProcName, _tcslen(tzProcName)+1);

	/* 스냅샷 핸들 얻기 */
	hSnapshot = CreateToolhelp32Snapshot (TH32CS_SNAPPROCESS, 0);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		pe32.dwSize	=sizeof(PROCESSENTRY32);
		if (Process32First(hSnapshot, &pe32))
		{
			/* 프로세스 목록 검색 시작 */
			do {

				// 프로세스 목록 중 비교할 프로세스 이름;
				swprintf_s(tzFindName, _MAX_FNAME, L"%s", pe32.szExeFile);
				_wcslwr_s(tzFindName, _tcslen(tzFindName)+1);
				if (0 == wcscmp(tzProcName, tzFindName))
				{
					bFind = TRUE;	/* 검색 성공 */
					break;
				}

				/* 혹시 동일한 프로세스로 여러개 실행되어 있는지 확인하기 위해서 */
				bContinue = Process32Next(hSnapshot, &pe32);

			}	while (bContinue);

		}
		CloseHandle(hSnapshot);
	}

	return bFind;
}

/*
 desc : 프로세스 이름으로 검색 후 해당 프로그램 강제 종료
 parm : proc_name	- [in]  확장자가 포함된 프로세스 이름
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

	/* 무조건 소문자 */
	swprintf_s(tzProcName, _MAX_FNAME, L"%s", proc_name);
	_wcslwr_s(tzProcName, _tcslen(tzProcName)+1);

	/* 스냅샷 핸들 얻기 */
	hSnapshot = CreateToolhelp32Snapshot (TH32CS_SNAPPROCESS, 0);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		pe32.dwSize	=sizeof(PROCESSENTRY32);
		if (Process32First(hSnapshot, &pe32))
		{
			/* 프로세스 목록 검색 시작 */
			do {

				// 프로세스 목록 중 비교할 프로세스 이름;
				swprintf_s(tzFindName, _MAX_FNAME, L"%s", pe32.szExeFile);
				_wcslwr_s(tzFindName, _tcslen(tzFindName)+1);
				if (0 == wcscmp(tzProcName, tzFindName))
				{
					// 프로세스 핸들 얻기
					hFindProc = OpenProcess(PROCESS_ALL_ACCESS, 0, pe32.th32ProcessID);
					if (hFindProc)
					{
						dwExitCode	= 0;
						GetExitCodeProcess(hFindProc, &dwExitCode);
						/* 프로세스 강제 종료 */
						TerminateProcess(hFindProc, dwExitCode);
					}
				}
				/* 혹시 동일한 프로세스로 여러개 실행되어 있는지 확인하기 위해서 */
				bContinue = Process32Next(hSnapshot, &pe32);

			}	while (bContinue);

		}
		CloseHandle(hSnapshot);
	}
}

/*
 desc : Service 재시작 (멈춤 실행 -> 멈춤 확인 -> 다시 실행)
 parm : None
 retn : TRUE or FALSE
 note : 반드시 시스템이 Administrator 권한으로 로그인되어 있어야 됨
*/
BOOL CService::RestartService(PTCHAR service_name)
{
	UINT64 u64Tick	= GetTickCount64();

	/* Service가 동작 중인지 확인 */
	if (IsServiceStatus(service_name, 0x00) ||	/* 현재 실행 중 */
		IsServiceStatus(service_name, 0x02))		/* 일시 정지 중 */
	{
		/* Service를 멈춤으로 변경 */
		if (!StopSvc(service_name))
		{
#ifndef _DEBUG
			AfxMessageBox(L"Failed to init the service <Stop>", MB_ICONSTOP|MB_TOPMOST);
#endif
			return TRUE;	/* 프로그램 실행 종료를 막기 위해 TRUE 리턴 */
		}
		/* Service 중지 상태 확인 */
		do	{
			/* 일정시간 대기 */
			Sleep(100);
			/* 현재 Service가 중지된 상태인지 확인 */
			if (IsServiceStatus(service_name, 0x01))	break;

			/* 일정 시간동안 대기 중에도 서비스가 중지되지 않은 경우, 에러 처리 */
			if (GetTickCount64() > u64Tick + 5000)
			{
#ifndef _DEBUG
				AfxMessageBox(L"Service initialization timed out", MB_ICONSTOP|MB_TOPMOST);
#endif
				return TRUE;	/* 프로그램 실행 종료를 막기 위해 TRUE 리턴 */
			}

		}	while (1);
	}
	/* Service 시작 상태로 변경 */
	return StartSvc(service_name);
}

/*
 desc : 하드 디스크 용량 정보 반환 (단위: Bytes)
 parm : total	- [out] 총 용량 반환
		free	- [out] 남아 있는 용량 반환
		drive	- [in]  드라이브 명칭 (루트 이름 즉, "C:\\" or "D:\\", ...)
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

	/* 용량 정보 반환 */
	total	= lTotalNumberOfBytes.QuadPart;
	free	= lTotalNumberOfFreeBytes.QuadPart;

	return TRUE;
}

/*
 desc : 시스템의 메모리 현황 값 반환
 parm : mem	- [out] 메모리 정보가 저장되는 구조체 참조 포인터
 retn : TRUE or FALSE
*/
BOOL CService::GetMemStatus(MEMORYSTATUSEX &mem)
{
	// 아래 길이 값은 반드시 초기화 시켜 줘야 됨
	mem.dwLength	= sizeof(MEMORYSTATUSEX);

	return GlobalMemoryStatusEx(&mem) == TRUE ? TRUE : FALSE;
}

/*
 desc : CPU에 내에 전체 스레드 개수 반환
 parm :	None
 retn : 전체 스레드 개수 값
*/
UINT32 CService::GetNumberOfThread()
{
	SYSTEM_INFO stSysInfo	= {NULL};

	// CPU 정보를 알아오기 위해서. 즉, 클라이언트 스레드를 유동적으로 생성하기 위함
	GetSystemInfo(&stSysInfo);
	// 공식에 의한 스레드 개수 반환
	return stSysInfo.dwNumberOfProcessors;	/* CPU Core 개수가 아니라, Thread Counting */
}

/*
 desc : 윈도 OS 버전 얻어오기
 parm : major	- [out] Major version 반환 참조 변수
		minor	- [out] Minor version 반환 참조 변수
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
 desc : 현재 프로세스의 메모리 사용량 확인
 parm : None
 retn : 사용량 반환 (단위: Bytes)
*/
UINT64 CService::GetProcessMemoryUsage()
{
	HANDLE hProc;
	PROCESS_MEMORY_COUNTERS_EX stPMC	= {NULL};

	/* Print information about the memory usage of the process */
	hProc	= OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
	if (!hProc)	return 0;
	/* 메모리 크기 반환 */
	stPMC.cb	= sizeof(stPMC);
	GetProcessMemoryInfo(hProc, (PROCESS_MEMORY_COUNTERS *)&stPMC, sizeof(stPMC));
	/* 메모리 핸들 닫기 */
	CloseHandle(hProc);

	/* Bytes로 반환 */
	return stPMC.PrivateUsage;
}

/*
 desc : 현재 프로세스의 메모리 사용량 확인
 parm : type	- [in]  0x00 : Bytes, 0x01 : KBytes, 0x02 : MBytes, 0x03 : GBytes
 retn : 사용량 반환
*/
DOUBLE CService::GetProcessMemoryUsageSize(UINT8 type)
{
	HANDLE hProc;
	PROCESS_MEMORY_COUNTERS_EX stPMC	= {NULL};

	/* Print information about the memory usage of the process */
	hProc	= OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
	if (!hProc)	return 0;
	/* 메모리 크기 반환 */
	stPMC.cb	= sizeof(stPMC);
	GetProcessMemoryInfo(hProc, (PROCESS_MEMORY_COUNTERS *)&stPMC, sizeof(stPMC));
	/* 메모리 핸들 닫기 */
	CloseHandle(hProc);

	/* Bytes로 반환 */
	switch (type)
	{
	case 0x01	:	return stPMC.PrivateUsage / 1024.0f;
	case 0x02	:	return stPMC.PrivateUsage / (1024.0f * 1024.0f);
	case 0x03	:	return stPMC.PrivateUsage / (1024.0f * 1024.0f * 1024.0f);
	}
	/* 기본 값은 Bytes */
	return DOUBLE(stPMC.PrivateUsage);
}

/*
 desc : 현재 프로세스의 실행 파일 정보 반환
 parm : path_name	- [out] 실행 파일 경로가 저장된 경로 반환
		path_size	- [in]  'path_name'의 버퍼 크기
		file_name	- [out] 실행 파일 이름(확장자 포함) 반환
		file_size	- [in]  'file_name'의 버퍼 크기
 retn : None
*/
VOID CService::GetProcesssName(PTCHAR path_name, UINT32 path_size, PTCHAR file_name,
							   UINT32 file_size)
{
	TCHAR *pFirstIdx, *pSecondIdx, tzBuffer[MAX_PATH_LEN]	= {NULL};
	UINT32 u32PathLen, u32FileLen;

	// 현재 프로세스의 실행파일의 전체 경로(파일명 포함) 얻기
	GetModuleFileName(NULL, tzBuffer, MAX_PATH_LEN);
	// 파일명의 시작과 종료되는 위치 검색
	pFirstIdx	= wcsrchr(tzBuffer, '\\');
	pSecondIdx	= wcsrchr(tzBuffer, '\0');
	// 폴더명의 길이 구하기
	u32PathLen	= UINT32(pFirstIdx - tzBuffer);
	// 파일명의 길이 구하기 (확장자 포함)
	u32FileLen	= UINT32(pSecondIdx- (pFirstIdx + 1));

	// 경로 및 파일명 복사
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
 desc : 현재 프로세스의 경로 반환
 parm : path_name	- [out] 실행 파일 경로가 저장된 경로 반환
		path_size	- [in]  'path_name'의 버퍼 크기
 retn : None
*/
VOID CService::GetProcesssPath(PTCHAR path_name, UINT32 path_size)
{
	TCHAR *pFirstIdx, *pSecondIdx, tzBuffer[MAX_PATH_LEN]	= {NULL};
	UINT32 u32PathSize;

	// 현재 프로세스의 실행파일의 전체 경로(파일명 포함) 얻기
	GetModuleFileName(NULL, tzBuffer, MAX_PATH_LEN);
	// 파일명의 시작과 종료되는 위치 검색
	pFirstIdx	= wcsrchr(tzBuffer, '\\');
	pSecondIdx	= wcsrchr(tzBuffer, '\0');
	// 폴더명의 길이 구하기
	u32PathSize	= UINT32(pFirstIdx - tzBuffer);

	// 경로 및 파일명 복사
	if (path_name)
	{
		if (path_size-1 < u32PathSize)	u32PathSize	= path_size - 1;
		wmemcpy_s(path_name, path_size, tzBuffer,	u32PathSize);
	}
}

/*
 desc : 현재 실행 중인 자신의 프로세스 버전 알아오기
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

	// 모듈과 파일의 버전 길이 얻기
	GetModuleFileName(NULL, tzFile, sizeof(tzFile));
	u32Len	= ::GetFileVersionInfoSize(tzFile, &u32Handle);
	if (u32Len < 1)	return;

	// 버전 정보가 들어갈 버퍼 할당
	pVerInfo	= new UINT8 [u32Len+1];
	pVerInfo[u32Len]	= 0x00;

	// 버전 정보 얻기
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

	// 버전 정보 메모리 해제
	delete [] pVerInfo;
	pVerInfo	= NULL;
}
