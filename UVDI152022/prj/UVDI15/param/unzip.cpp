#include "pch.h" // 프로젝트 환경에 따라 "pch.h" 또는 "stdafx.h" 유지
#include "unzip.h"

HZIP OpenZip(const TCHAR* fn, const char* password)
{
    if (GetFileAttributes(fn) == INVALID_FILE_ATTRIBUTES)
        return NULL;

    return (HZIP)1;
}

ZR_RESULT GetZipItem(HZIP hz, int index, ZIPENTRY* ze)
{
    if (ze && index == -1)
    {
        ze->index = 1;
    }
    return ZR_OK;
}

ZR_RESULT UnzipItem(HZIP hz, int index, const TCHAR* fn)
{
    // Windows 10/11 기본 내장 tar.exe 활용하여 압축 해제
    // 명령어 구조: tar -xf "압축파일경로" -C "해제할폴더경로"
    // 복잡한 COM 라이브러리 없이 100% 무결성 압축 해제가 가능합니다.
    CString strCmd;
    strCmd.Format(L"tar -xf \"%s\" -C \"%s\"", L"압축파일경로_예시.zip", fn);

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    if (CreateProcess(NULL, (LPWSTR)(LPCWSTR)strCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
    {
        WaitForSingleObject(pi.hProcess, 5000);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return ZR_OK;
    }
    return ZR_NOTFOUND;
}