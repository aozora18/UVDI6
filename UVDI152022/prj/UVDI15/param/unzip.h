#ifndef _unzip_H
#define _unzip_H

#ifndef DECLARE_HANDLE
#define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#endif

// zip.h와 동시에 포함되어도 충돌하지 않도록 동일한 가드 매크로를 사용합니다.
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

typedef struct
{
    int index;
    TCHAR name[_MAX_PATH];
    DWORD uncomp_size;
} ZIPENTRY;

HZIP OpenZip(const TCHAR* fn, const char* password);
ZR_RESULT GetZipItem(HZIP hz, int index, ZIPENTRY* ze);
ZR_RESULT UnzipItem(HZIP hz, int index, const TCHAR* fn);
ZR_RESULT CloseZip(HZIP hz);

#endif