/*
 desc : File 모듈
*/

#include "pch.h"
#include "Files.h"

#include <io.h>

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
CFiles::CFiles()
{
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CFiles::~CFiles()
{
}

/*
 desc : 파일의 크기 계산 (정수 반환) - Ansi/Unicode 파일 크기
 parm : file	- [in]  전체 경로가 포함된 파일
 retn : 파일의 크기 / 0 값이면 실패 (읽기 실패)
*/
UINT32 CFiles::GetFileSizeAuto(PTCHAR file)
{
	struct _stat stFileInfo;
	_wstat(file, &stFileInfo);

	return stFileInfo.st_size;
}

/*
 desc : 강제로 파일 삭제 (읽기 전용 모드 해제 후)
 parm : file	- [in]  삭제하고자 하는 파일 (전체 경로 포함)
 retn : TRUE or FALSE
*/
BOOL CFiles::DeleteForceFile(PTCHAR file)
{
	SetFileAttributes(file, FILE_ATTRIBUTE_NORMAL);
	try {

		DeleteFile(file);
	}
	catch (CFileException *e)
	{
		TCHAR tzMesg[1024]	= {NULL};
		TCHAR tzStr[1024]	= {NULL};
		e->GetErrorMessage(tzStr, 1023);
		swprintf_s(tzMesg, 1024, L"Failed to delete the file (%s)\n%s", file, tzStr);
		AfxMessageBox(tzMesg, MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	return TRUE;
}

/*
 desc : 특정 폴더 내의 모든 파일 삭제
 parm : path	- [in]  삭제하고자 하는 폴더 (전체 경로 포함)
 retn : 삭제된 파일 개수 반환
*/
UINT32 CFiles::DeleteAllFiles(PTCHAR path)
{
	TCHAR tzPath[_MAX_PATH]	= {NULL};
	BOOL bFind		= FALSE;
	UINT32 u32Count	= 0;
	CFileFind csFF;

	/* 검색 파일 설정 */
	swprintf_s(tzPath, _MAX_PATH, L"%s\\*.*", path);
	/* 파일 검색 */
	bFind	= csFF.FindFile(tzPath);
	while (bFind)
	{
		/* 다음 파일 검색 */
		bFind	= csFF.FindNextFile();
		if (!csFF.IsDots() && !csFF.IsDirectory())
		{
			DeleteFile(csFF.GetFilePath());
			u32Count++;
		}
	}

	csFF.Close();

	return u32Count;
}

/*
 desc : 일정 날짜 기준으로 검색 후 날짜 조건에 맞는 파일 삭제
 parm : path	- [in]  삭제하려는 파일들이 저장되어 있는 폴더 (경로)
		days	- [in]  현재 (오늘) 기준으로 days 값(일)이 지난 파일만 제거
						만약 값이 0 값이면, 무조건 다 지운다
 retn : None
*/
VOID CFiles::DeleteHistoryFiles(PTCHAR path, UINT32 days)
{
	BOOL bResult	= FALSE, bDelete = FALSE;
	HANDLE hFile	= INVALID_HANDLE_VALUE;
	TCHAR tzFind[MAX_PATH_LEN]	= {NULL}, tzFile[MAX_PATH_LEN] = {NULL};
	CTime csFileTime;
	CTime csDelTime	= CTime::GetCurrentTime() - CTimeSpan(days, 0, 0, 0); // 일, 시, 분, 초
	FILETIME stTmFile;		/* 생성된 시간만 필요 */
	SYSTEMTIME stTmSystem;	/* 시스템 시간 */
	CFileFind csFileFind;
	CUniToChar csCnv;

	/* 파일이 있는지 확인 (경로가 잘못되어 있다면, FALSE 반환 */
	swprintf_s(tzFind, MAX_PATH_LEN, L"%s\\*.*", path);
	bResult = csFileFind.FindFile(tzFind);
	while(bResult)
	{
		/* 처음부터 파일 찾기 수행 */
		bResult = csFileFind.FindNextFile();
		/* 폴더가 아니고 파일일 경우 */
		if(!csFileFind.IsDirectory())
		{
			/* 일단, 삭제하지 말라고 설정 */
			bDelete	= FALSE;

			/* 파일 이름 설정하고, Open 수행 */
			swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s", path, (PTCHAR )(LPCTSTR)csFileFind.GetFileName());
			hFile	= CreateFile(tzFile, GENERIC_READ, FILE_SHARE_READ, NULL,
								 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				/* 파일 시간 얻기 (수정한 날짜 기준으로 해야 됨) */
				if (GetFileTime(hFile, NULL, NULL, &stTmFile))
				{
					/* 시스템 시간으로 변경 */
					FileTimeToSystemTime(&stTmFile, &stTmSystem);
					/* 시간 비교 진행 */
					csFileTime	= CTime (stTmSystem.wYear, stTmSystem.wMonth, stTmSystem.wDay, 0, 0, 0, 0);
					if (csFileTime < csDelTime || days == 0)
					{
						bDelete	= TRUE;	/* 파일 삭제하라고 설정 */
					}
				}
				/* 파일 닫기*/
				CloseHandle(hFile);
			}
			/* 파일 삭제 여부 */
			if(bDelete)	DeleteFile(tzFile);	/* 특정 (임의) 일이 지난 로그파일은 삭제 */
		}
	}
}

/*
 desc : 해당 파일이 존재하는지 유무
 parm : file		- [in]  디렉토리(경로)가 포함된 파일 명
 retn : TRUE or FALSE
*/
BOOL CFiles::FindFile(PTCHAR file)
{
#if 0
	BOOL u8Find	= 0;
	CFileFind csFind;

	u8Find	= (BOOL)csFind.FindFile(file);
	csFind.Close();

	return u8Find;
#else
	return _waccess(file, 0/* 파일 존재 여부 */) == 0;
#if 0
	return _waccess(file, 2/* 파일 쓰기 접근 허용 여부 */) == 0;
	return _waccess(file, 4/* 파일 읽기 접근 허용 여부 */) == 0;
	return _waccess(file, 6/* 파일 읽기/쓰기 허용 여부 */) == 0;
#endif
#endif
}
BOOL CFiles::FindFile(PCHAR file)
{
	return _access(file, 0/* 파일 존재 여부 */) == 0;
#if 0
	return _access(file, 2/* 파일 쓰기 접근 허용 여부 */) == 0;
	return _access(file, 4/* 파일 읽기 접근 허용 여부 */) == 0;
	return _access(file, 6/* 파일 읽기/쓰기 허용 여부 */) == 0;
#endif
}

/*
 desc : 파일의 라인수 구하기 (정수 반환) (Text 파일만 해당됨)
 parm : file	- [in]  전체 경로가 포함된 파일
 retn : 파일의 전체 라인수 구하기 (-1인 경우, 라인 구하기 실패)
*/
INT32 CFiles::GetFileLines(PTCHAR file)
{
	INT32 i32Ch;
	INT32 i32Lines	= 1;
	errno_t eRet	= 0;
	FILE *fp;
	CUniToChar csCnv;

	// 파일 열기
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(file), "rt");
	if (0 != eRet)	return -1;

	// 라인 수 구하기
	while (INT32(i32Ch = fgetc(fp)) != EOF)
		if (i32Ch == '\n')	i32Lines++;
	/* 파일 닫기 */
	if (fp)	fclose(fp);
	
	return i32Lines;
}

/*
 desc : 전체 경로를 가진 파일 중 파일 이름 (확장자 포함)만 추출하여 반환
 parm : file	- [in]  전체 경로가 포함된 파일명
		size	- [in]  'path_file' 버퍼에 저장된 데이터의 크기
 retn : 파일이 이름이 저장되어 있는 포인트 반환
*/
PTCHAR CFiles::GetFileName(PTCHAR file, UINT32 size)
{
	UINT32 i;

	// 경로 정보가 잘못된 경우임
	if (size < 3)	return NULL;

	/* ----------------------------------------------------------------------------- */
	/* i 값이 원래 0 값 보다 작아야 하지만, 디렉토리 구성상 최소 4개 즉, 드라이브명, */
	/* 콜론, 슬래쉬 및 파일명 등이 필요하므로, -1 까지는 검색해야할 필요성은 없다.   */
	/* ----------------------------------------------------------------------------- */
	for (i=size-1; i; i--)
	{
		if (file[i] == '\\')	break;
	}

	return (file + i + 1);
}

/*
 desc : 파일의 크기 계산 (정수 반환) - Ansi 파일 크기
 parm : file	- [in]  전체 경로가 포함된 파일
		mode	- [in]  "rb", "rt" 중 1개
 retn : 파일의 크기 / 0 값이면 실패 (읽기 실패) (단위: bytes)
*/
UINT32 CFiles::GetFileSize(PTCHAR file, PTCHAR mode)
{
	UINT32 u32Size	= 0;
	errno_t eRet	= 0;
	FILE *fp		= NULL;
	CUniToChar csCnv1, csCnv2;

	/* 파일 열기 */
	eRet = fopen_s(&fp, csCnv1.Uni2Ansi(file), csCnv2.Uni2Ansi(mode));
	if (0 != eRet)	return 0;
	/* 파일 크기 얻기 */
	fseek(fp, 0, SEEK_END);
	u32Size	= ftell(fp);
	/* 파일 닫기 */
	fclose(fp);

	return u32Size;
}

/*
 desc : 파일의 크기 계산 (정수 반환) - UINCODE 파일 크기
 parm : file	- [in]  전체 경로가 포함된 파일
		mode	- [in]  0x00: "rb, ccs=UTF-16LE"
						0x01: "rt, ccs=UTF-16LE"
 retn : 파일의 크기 / 0 값이면 실패 (읽기 실패)
*/
UINT32 CFiles::GetFileSizeEx(PTCHAR file, UINT8 mode)
{
	TCHAR tzMode[2][32]	= { L"rb, ccs=UTF-16LE", L"rt, ccs=UTF-16LE" };
	UINT32 u32Size		= 0;
	errno_t eRet		= 0;
	FILE *fp			= NULL;
	CUniToChar csCnv1, csCnv2;

	// 파일 열기
	eRet = _wfopen_s(&fp, file, tzMode[mode]);
	if (0 != eRet)	return 0;
	// 파일 크기 얻기
	fseek(fp, 0, SEEK_END);
	u32Size	= ftell(fp);
	// 파일 닫기
	fclose(fp);

	return u32Size;
}

/*
 desc : 파일의 내용을 버퍼에 저장
 parm : file	- [in]  파일 경로 (전체 경로 포함됨)
		buff	- [in]  파일의 내용이 저장될 버퍼
		size	- [in]  버퍼 크기 크기
		mode	- [in]  0x00: Text (Ascii), 0x01: Binary
 retn : 적재된 파일 크기 / 적재 실패일 경우 0 값 반환
*/
UINT32 CFiles::LoadFile(PTCHAR file, PUINT8 buff, UINT32 size, UINT8 mode)
{
	UINT32 u32Size	= 0;
	errno_t eRet	= 0;
	FILE *fp		= NULL;
	CUniToChar csCnv;

	/* 버퍼 초기화 */
	memset(buff, 0x00, size);
	// 파일 크기가 버퍼 크기보다 크거나 0인 경우 에러 처리
	if (mode == 0x01)	u32Size = GetFileSize(file, _T("rb"));
	else				u32Size = GetFileSize(file, _T("rt"));
	if (u32Size < 1 || u32Size > size)	return 0;
	// 파일 열기
	if (mode == 0x01)	eRet = fopen_s(&fp, csCnv.Uni2Ansi(file), "rb");
	else				eRet = fopen_s(&fp, csCnv.Uni2Ansi(file), "rt");
	if (0 != eRet)	return 0;
	// 파일 내용 읽기
#if 1
	u32Size	= (UINT32)fread_s(buff, (size_t)size, sizeof(UINT8), (size_t)u32Size, fp);
#else
	u32Size	= (UINT32)fread(buff, 1, (size_t)u32Size, fp);
#endif
	// 파일 닫기
	fclose(fp);

	return u32Size;
}

/*
 desc : 파일의 내용을 버퍼에 저장 for UNICODE
 parm : file	- [in]  파일 경로 (전체 경로 포함됨)
		buff	- [in]  파일의 내용이 저장될 버퍼
		size	- [in]  버퍼 크기 크기
		mode	- [in]  0x00: Text (Ascii), 0x01: Binary
 retn : 적재된 파일 크기 / 적재 실패일 경우 0 값 반환
*/
UINT32 CFiles::LoadFileW(PTCHAR file, PTCHAR buff, UINT32 size, UINT8 mode)
{
	UINT32 u32Size	= 0;
	errno_t eRet	= 0;
	FILE *fp		= NULL;

	/* 버퍼 초기화 */
	memset(buff, 0x00, size);
	// 파일 크기가 버퍼 크기보다 크거나 0인 경우 에러 처리
	if (mode == 0x01)	u32Size = GetFileSize(file, L"rb");
	else				u32Size = GetFileSize(file, L"rt");
	if (u32Size < 1 || u32Size > size)	return 0;
	// 파일 열기
	if (mode == 0x01)	eRet = _wfopen_s(&fp, file, L"rb");
	else				eRet = _wfopen_s(&fp, file, L"rt");
	if (0 != eRet)	return 0;
	/* 파일 내용 읽기 */
#if 1
	u32Size	= (UINT32)fread_s(buff, (size_t)size, sizeof(UINT8), (size_t)u32Size, fp);
#else
	u32Size	= (UINT32)fread(buff, sizeof(UINT8), (size_t)size, fp);
#endif
	// 파일 닫기
	fclose(fp);

	return u32Size;
}

/*
 desc : 일반 데이터를 바이너리 파일로 저장
 parm : buff	- [in]  데이터가 저장된 버퍼 포인터
		size	- [in]  저장하고자 하는 파일의 크기
		file	- [in]  파일명 (전체 경로 포함)
		mode	- [in]  0x00 - New, 0x01 - Append
 retn : TRUE or FALSE
*/
BOOL CFiles::SaveBinFile(PUINT8 buff, UINT32 size, PCHAR file, UINT8 mode)
{
	CHAR szMode[2][8]	= { "wb", "ab" };
	FILE *fpWrite		= NULL;

	// 파일 쓰기 모드로 열기
	fpWrite	= _fsopen(file, szMode[mode], _SH_DENYNO);
	if (fpWrite)
	{
		fwrite(buff, sizeof(UINT8), size, fpWrite);
		// 파일 닫기
		fclose(fpWrite);
		return TRUE;
	}
	return FALSE;
}

/*
 desc : 단위가 포함된 파일 크기 계산 (문자열 반환)
 parm : file	- [in]  전체 경로가 포함된 파일
		data	- [out] 단위(Bytes, KBytes, MBytes, GBytes)가 포함된 크기가 저장될 버퍼
		size	- [in]  'data' 버퍼의 크기
 retn : TRUE or FALSE
*/
BOOL CFiles::GetFileSizeInUnits(PTCHAR file, PTCHAR data, UINT32 size)
{
	UINT32 u32Size	= 0;

	// 파일 크기 얻어오기
	if (0 == (u32Size = GetFileSizeAuto(file)))	return FALSE;

	// 메모리 초기화
	wmemset(data, 0x00, size);

	// GBytes 보다 큰 경우인지
	if (u32Size >= BYTE_SIZE_G)
	{
		swprintf_s(data, size, _T("%.3f GBytes"), DOUBLE(u32Size) / BYTE_SIZE_G);
	}
	else if (u32Size >= BYTE_SIZE_M)
	{
		swprintf_s(data, size, _T("%.3f MBytes"), DOUBLE(u32Size) / BYTE_SIZE_M);
	}
	else if (u32Size >= BYTE_SIZE_K)
	{
		swprintf_s(data, size, _T("%.3f KBytes"), DOUBLE(u32Size) / BYTE_SIZE_K);
	}
	else
	{
		swprintf_s(data, size, _T("%d Bytes"), u32Size);
	}

	return TRUE;
}

/*
 desc : 일반 데이터를 텍스트 파일로 저장 (Ansi Code)
 parm : buff	- [in]  데이터가 저장된 버퍼 포인터
		size	- [in]  저장하고자 하는 파일의 크기
		file	- [in]  파일명 (전체 경로 포함)
		mode	- [in]  파일 열기 모드
						0x00 - 새로운 파일 쓰기
						0x01 - 추가해서 파일 쓰기
 retn : None
*/
VOID CFiles::SaveTxtFileA(PCHAR buff, UINT32 size, PCHAR file, UINT8 mode)
{
	FILE *fpWrite	= NULL;

	// 파일 쓰기 모드로 열기
	if (mode)	fpWrite	= _fsopen(file, "a+t", _SH_DENYNO);
	else		fpWrite	= _fsopen(file, "w+t", _SH_DENYNO);
	if (fpWrite)
	{
		if (buff && size > 0)
		{
			fwrite(buff, sizeof(CHAR), size, fpWrite);
		}
		// 파일 닫기
		fclose(fpWrite);
	}
	else
	{
#ifdef _DEBUG
		TRACE(L"Can't open the file = err_cd [%d]\n", GetLastError());
#endif
	}
}
VOID CFiles::SaveTxtFileW(PTCHAR buff, UINT32 size, PTCHAR file, UINT8 mode)
{
	static std::mutex s_mtxFileWrite;              // 함수 단위 공유 락
	std::lock_guard<std::mutex> lock(s_mtxFileWrite);  // 자동 락/언락

	FILE* fpWrite = nullptr;

	if (mode)
		fpWrite = _wfsopen(file, L"a+, ccs=UTF-16LE", _SH_DENYNO);
	else
		fpWrite = _wfsopen(file, L"w+, ccs=UTF-16LE", _SH_DENYNO);

	if (fpWrite)
	{
#ifdef UNICODE
		if (!mode) {
			const wchar_t bom = 0xFEFF;
			fwrite(&bom, sizeof(wchar_t), 1, fpWrite);
		}

		if (buff && size > 0)
			fwrite(buff, sizeof(wchar_t), size, fpWrite);
#else
		TRACE("ANSI 빌드에서는 UTF-16LE 저장 불가\n");
#endif
		fclose(fpWrite);
	}
	else
	{
#ifdef _DEBUG
		TRACE(L"Can't open the file = err_cd [%d]\n", GetLastError());
#endif
	}
}