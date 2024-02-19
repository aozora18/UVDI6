#pragma once

class CService
{
/* Constructor and Destructor */
public:

	CService();
	virtual ~CService();

/* Virtual Function */
protected:

public:


/* Member Parameter : Local */
protected:


/* Member Function : Local */
protected:


/* Member Function : Public */
public:

	UINT32				GetCPUCacheLineSize();
	BOOL				GetHDDInfo(UINT64 &total, UINT64 &free, PTCHAR drive);
	BOOL				GetMemStatus(MEMORYSTATUSEX &mem);
	UINT32				GetNumberOfThread();
	BOOL				GetOSVer(UINT32 &major, UINT32 &minor);
	UINT64				GetProcessMemoryUsage();
	DOUBLE				GetProcessMemoryUsageSize(UINT8 type);
	VOID				GetProcesssName(PTCHAR path_name, UINT32 path_size, PTCHAR file_name,
										UINT32 file_size);
	VOID				GetProcesssPath(PTCHAR path_name, UINT32 path_size);
	VOID				GetRunProcVerion(UINT32 &major1, UINT32 &major2, UINT32 &minor1, UINT32 &minor2);
	HWND				GetWindowHwnd(UINT64 pid);
	BOOL				FindProcess(PTCHAR proc_name);
	BOOL				IsServiceStatus(PTCHAR service_name, UINT8 status);
	VOID				KillProcess(PTCHAR proc_name);
	BOOL				RestartService(PTCHAR service_name);
	BOOL				RunProcess(PTCHAR proc_name, UINT32 wait_time);
	BOOL				StartSvc(PTCHAR service_name);
	BOOL				StopSvc(PTCHAR service_name);
};
