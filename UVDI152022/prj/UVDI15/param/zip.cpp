#include "pch.h" // 프로젝트 환경에 따라 "pch.h" 또는 "stdafx.h" 유지
#include "zip.h"

HZIP CreateZip(const TCHAR* fn, const char* password)
{
    // 압축 시작 전 핸들 가상 값 반환 (tar 명령어 처리용)
    return (HZIP)1;
}

ZR_RESULT ZipAdd(HZIP hz, const TCHAR* dstzn, const TCHAR* fn)
{
    // Windows 10/11 기본 내장 tar.exe 활용
    // 명령어 구조: tar -a -cf "압축파일경로" "원본파일경로"
    CString strCmd;
    strCmd.Format(L"tar -a -cf \"%s\" \"%s\"", dstzn, fn);

    // 검은색 CMD 창 없이 백그라운드에서 안전하게 실행
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    if (CreateProcess(NULL, (LPWSTR)(LPCWSTR)strCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
    {
        // 압축 처리가 완료될 때까지 최대 5초 대기
        WaitForSingleObject(pi.hProcess, 5000);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return ZR_OK;
    }
    return ZR_NOTFOUND;
}

ZR_RESULT CloseZip(HZIP hz)
{
    return ZR_OK;
}