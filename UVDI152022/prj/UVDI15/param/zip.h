#ifndef _zip_H
#define _zip_H

#ifndef DECLARE_HANDLE
#define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#endif

// HZIP이 정의되지 않았을 때만 딱 한 번만 선언되도록 가드를 정교하게 맞춥니다.
#ifndef _HZIP_DEFINED
#define _HZIP_DEFINED
DECLARE_HANDLE(HZIP);
#endif

typedef DWORD ZR_RESULT;
#define ZR_OK                  0x00000000
#define ZR_RECENT              0x00000001
#define ZR_ARREARS             0x00000002
#define ZR_NOTFOUND            0x00000003
#define ZR_NODUP               0x00000004
#define ZR_DIRENTRY            0x00000005

HZIP CreateZip(const TCHAR* fn, const char* password);
ZR_RESULT ZipAdd(HZIP hz, const TCHAR* dstzn, const TCHAR* fn);
ZR_RESULT CloseZip(HZIP hz);

#endif