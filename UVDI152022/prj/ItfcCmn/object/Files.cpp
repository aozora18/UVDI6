/*
 desc : File ���
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
 desc : ������ ũ�� ��� (���� ��ȯ) - Ansi/Unicode ���� ũ��
 parm : file	- [in]  ��ü ��ΰ� ���Ե� ����
 retn : ������ ũ�� / 0 ���̸� ���� (�б� ����)
*/
UINT32 CFiles::GetFileSizeAuto(PTCHAR file)
{
	struct _stat stFileInfo;
	_wstat(file, &stFileInfo);

	return stFileInfo.st_size;
}

/*
 desc : ������ ���� ���� (�б� ���� ��� ���� ��)
 parm : file	- [in]  �����ϰ��� �ϴ� ���� (��ü ��� ����)
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
 desc : Ư�� ���� ���� ��� ���� ����
 parm : path	- [in]  �����ϰ��� �ϴ� ���� (��ü ��� ����)
 retn : ������ ���� ���� ��ȯ
*/
UINT32 CFiles::DeleteAllFiles(PTCHAR path)
{
	TCHAR tzPath[_MAX_PATH]	= {NULL};
	BOOL bFind		= FALSE;
	UINT32 u32Count	= 0;
	CFileFind csFF;

	/* �˻� ���� ���� */
	swprintf_s(tzPath, _MAX_PATH, L"%s\\*.*", path);
	/* ���� �˻� */
	bFind	= csFF.FindFile(tzPath);
	while (bFind)
	{
		/* ���� ���� �˻� */
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
 desc : ���� ��¥ �������� �˻� �� ��¥ ���ǿ� �´� ���� ����
 parm : path	- [in]  �����Ϸ��� ���ϵ��� ����Ǿ� �ִ� ���� (���)
		days	- [in]  ���� (����) �������� days ��(��)�� ���� ���ϸ� ����
						���� ���� 0 ���̸�, ������ �� �����
 retn : None
*/
VOID CFiles::DeleteHistoryFiles(PTCHAR path, UINT32 days)
{
	BOOL bResult	= FALSE, bDelete = FALSE;
	HANDLE hFile	= INVALID_HANDLE_VALUE;
	TCHAR tzFind[MAX_PATH_LEN]	= {NULL}, tzFile[MAX_PATH_LEN] = {NULL};
	CTime csFileTime;
	CTime csDelTime	= CTime::GetCurrentTime() - CTimeSpan(days, 0, 0, 0); // ��, ��, ��, ��
	FILETIME stTmFile;		/* ������ �ð��� �ʿ� */
	SYSTEMTIME stTmSystem;	/* �ý��� �ð� */
	CFileFind csFileFind;
	CUniToChar csCnv;

	/* ������ �ִ��� Ȯ�� (��ΰ� �߸��Ǿ� �ִٸ�, FALSE ��ȯ */
	swprintf_s(tzFind, MAX_PATH_LEN, L"%s\\*.*", path);
	bResult = csFileFind.FindFile(tzFind);
	while(bResult)
	{
		/* ó������ ���� ã�� ���� */
		bResult = csFileFind.FindNextFile();
		/* ������ �ƴϰ� ������ ��� */
		if(!csFileFind.IsDirectory())
		{
			/* �ϴ�, �������� ����� ���� */
			bDelete	= FALSE;

			/* ���� �̸� �����ϰ�, Open ���� */
			swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s", path, (PTCHAR )(LPCTSTR)csFileFind.GetFileName());
			hFile	= CreateFile(tzFile, GENERIC_READ, FILE_SHARE_READ, NULL,
								 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				/* ���� �ð� ��� (������ ��¥ �������� �ؾ� ��) */
				if (GetFileTime(hFile, NULL, NULL, &stTmFile))
				{
					/* �ý��� �ð����� ���� */
					FileTimeToSystemTime(&stTmFile, &stTmSystem);
					/* �ð� �� ���� */
					csFileTime	= CTime (stTmSystem.wYear, stTmSystem.wMonth, stTmSystem.wDay, 0, 0, 0, 0);
					if (csFileTime < csDelTime || days == 0)
					{
						bDelete	= TRUE;	/* ���� �����϶�� ���� */
					}
				}
				/* ���� �ݱ�*/
				CloseHandle(hFile);
			}
			/* ���� ���� ���� */
			if(bDelete)	DeleteFile(tzFile);	/* Ư�� (����) ���� ���� �α������� ���� */
		}
	}
}

/*
 desc : �ش� ������ �����ϴ��� ����
 parm : file		- [in]  ���丮(���)�� ���Ե� ���� ��
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
	return _waccess(file, 0/* ���� ���� ���� */) == 0;
#if 0
	return _waccess(file, 2/* ���� ���� ���� ��� ���� */) == 0;
	return _waccess(file, 4/* ���� �б� ���� ��� ���� */) == 0;
	return _waccess(file, 6/* ���� �б�/���� ��� ���� */) == 0;
#endif
#endif
}
BOOL CFiles::FindFile(PCHAR file)
{
	return _access(file, 0/* ���� ���� ���� */) == 0;
#if 0
	return _access(file, 2/* ���� ���� ���� ��� ���� */) == 0;
	return _access(file, 4/* ���� �б� ���� ��� ���� */) == 0;
	return _access(file, 6/* ���� �б�/���� ��� ���� */) == 0;
#endif
}

/*
 desc : ������ ���μ� ���ϱ� (���� ��ȯ) (Text ���ϸ� �ش��)
 parm : file	- [in]  ��ü ��ΰ� ���Ե� ����
 retn : ������ ��ü ���μ� ���ϱ� (-1�� ���, ���� ���ϱ� ����)
*/
INT32 CFiles::GetFileLines(PTCHAR file)
{
	INT32 i32Ch;
	INT32 i32Lines	= 1;
	errno_t eRet	= 0;
	FILE *fp;
	CUniToChar csCnv;

	// ���� ����
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(file), "rt");
	if (0 != eRet)	return -1;

	// ���� �� ���ϱ�
	while (INT32(i32Ch = fgetc(fp)) != EOF)
		if (i32Ch == '\n')	i32Lines++;
	/* ���� �ݱ� */
	if (fp)	fclose(fp);
	
	return i32Lines;
}

/*
 desc : ��ü ��θ� ���� ���� �� ���� �̸� (Ȯ���� ����)�� �����Ͽ� ��ȯ
 parm : file	- [in]  ��ü ��ΰ� ���Ե� ���ϸ�
		size	- [in]  'path_file' ���ۿ� ����� �������� ũ��
 retn : ������ �̸��� ����Ǿ� �ִ� ����Ʈ ��ȯ
*/
PTCHAR CFiles::GetFileName(PTCHAR file, UINT32 size)
{
	UINT32 i;

	// ��� ������ �߸��� �����
	if (size < 3)	return NULL;

	/* ----------------------------------------------------------------------------- */
	/* i ���� ���� 0 �� ���� �۾ƾ� ������, ���丮 ������ �ּ� 4�� ��, ����̺��, */
	/* �ݷ�, ������ �� ���ϸ� ���� �ʿ��ϹǷ�, -1 ������ �˻��ؾ��� �ʿ伺�� ����.   */
	/* ----------------------------------------------------------------------------- */
	for (i=size-1; i; i--)
	{
		if (file[i] == '\\')	break;
	}

	return (file + i + 1);
}

/*
 desc : ������ ũ�� ��� (���� ��ȯ) - Ansi ���� ũ��
 parm : file	- [in]  ��ü ��ΰ� ���Ե� ����
		mode	- [in]  "rb", "rt" �� 1��
 retn : ������ ũ�� / 0 ���̸� ���� (�б� ����) (����: bytes)
*/
UINT32 CFiles::GetFileSize(PTCHAR file, PTCHAR mode)
{
	UINT32 u32Size	= 0;
	errno_t eRet	= 0;
	FILE *fp		= NULL;
	CUniToChar csCnv1, csCnv2;

	/* ���� ���� */
	eRet = fopen_s(&fp, csCnv1.Uni2Ansi(file), csCnv2.Uni2Ansi(mode));
	if (0 != eRet)	return 0;
	/* ���� ũ�� ��� */
	fseek(fp, 0, SEEK_END);
	u32Size	= ftell(fp);
	/* ���� �ݱ� */
	fclose(fp);

	return u32Size;
}

/*
 desc : ������ ũ�� ��� (���� ��ȯ) - UINCODE ���� ũ��
 parm : file	- [in]  ��ü ��ΰ� ���Ե� ����
		mode	- [in]  0x00: "rb, ccs=UTF-16LE"
						0x01: "rt, ccs=UTF-16LE"
 retn : ������ ũ�� / 0 ���̸� ���� (�б� ����)
*/
UINT32 CFiles::GetFileSizeEx(PTCHAR file, UINT8 mode)
{
	TCHAR tzMode[2][32]	= { L"rb, ccs=UTF-16LE", L"rt, ccs=UTF-16LE" };
	UINT32 u32Size		= 0;
	errno_t eRet		= 0;
	FILE *fp			= NULL;
	CUniToChar csCnv1, csCnv2;

	// ���� ����
	eRet = _wfopen_s(&fp, file, tzMode[mode]);
	if (0 != eRet)	return 0;
	// ���� ũ�� ���
	fseek(fp, 0, SEEK_END);
	u32Size	= ftell(fp);
	// ���� �ݱ�
	fclose(fp);

	return u32Size;
}

/*
 desc : ������ ������ ���ۿ� ����
 parm : file	- [in]  ���� ��� (��ü ��� ���Ե�)
		buff	- [in]  ������ ������ ����� ����
		size	- [in]  ���� ũ�� ũ��
		mode	- [in]  0x00: Text (Ascii), 0x01: Binary
 retn : ����� ���� ũ�� / ���� ������ ��� 0 �� ��ȯ
*/
UINT32 CFiles::LoadFile(PTCHAR file, PUINT8 buff, UINT32 size, UINT8 mode)
{
	UINT32 u32Size	= 0;
	errno_t eRet	= 0;
	FILE *fp		= NULL;
	CUniToChar csCnv;

	/* ���� �ʱ�ȭ */
	memset(buff, 0x00, size);
	// ���� ũ�Ⱑ ���� ũ�⺸�� ũ�ų� 0�� ��� ���� ó��
	if (mode == 0x01)	u32Size = GetFileSize(file, _T("rb"));
	else				u32Size = GetFileSize(file, _T("rt"));
	if (u32Size < 1 || u32Size > size)	return 0;
	// ���� ����
	if (mode == 0x01)	eRet = fopen_s(&fp, csCnv.Uni2Ansi(file), "rb");
	else				eRet = fopen_s(&fp, csCnv.Uni2Ansi(file), "rt");
	if (0 != eRet)	return 0;
	// ���� ���� �б�
#if 1
	u32Size	= (UINT32)fread_s(buff, (size_t)size, sizeof(UINT8), (size_t)u32Size, fp);
#else
	u32Size	= (UINT32)fread(buff, 1, (size_t)u32Size, fp);
#endif
	// ���� �ݱ�
	fclose(fp);

	return u32Size;
}

/*
 desc : ������ ������ ���ۿ� ���� for UNICODE
 parm : file	- [in]  ���� ��� (��ü ��� ���Ե�)
		buff	- [in]  ������ ������ ����� ����
		size	- [in]  ���� ũ�� ũ��
		mode	- [in]  0x00: Text (Ascii), 0x01: Binary
 retn : ����� ���� ũ�� / ���� ������ ��� 0 �� ��ȯ
*/
UINT32 CFiles::LoadFileW(PTCHAR file, PTCHAR buff, UINT32 size, UINT8 mode)
{
	UINT32 u32Size	= 0;
	errno_t eRet	= 0;
	FILE *fp		= NULL;

	/* ���� �ʱ�ȭ */
	memset(buff, 0x00, size);
	// ���� ũ�Ⱑ ���� ũ�⺸�� ũ�ų� 0�� ��� ���� ó��
	if (mode == 0x01)	u32Size = GetFileSize(file, L"rb");
	else				u32Size = GetFileSize(file, L"rt");
	if (u32Size < 1 || u32Size > size)	return 0;
	// ���� ����
	if (mode == 0x01)	eRet = _wfopen_s(&fp, file, L"rb");
	else				eRet = _wfopen_s(&fp, file, L"rt");
	if (0 != eRet)	return 0;
	/* ���� ���� �б� */
#if 1
	u32Size	= (UINT32)fread_s(buff, (size_t)size, sizeof(UINT8), (size_t)u32Size, fp);
#else
	u32Size	= (UINT32)fread(buff, sizeof(UINT8), (size_t)size, fp);
#endif
	// ���� �ݱ�
	fclose(fp);

	return u32Size;
}

/*
 desc : �Ϲ� �����͸� ���̳ʸ� ���Ϸ� ����
 parm : buff	- [in]  �����Ͱ� ����� ���� ������
		size	- [in]  �����ϰ��� �ϴ� ������ ũ��
		file	- [in]  ���ϸ� (��ü ��� ����)
		mode	- [in]  0x00 - New, 0x01 - Append
 retn : TRUE or FALSE
*/
BOOL CFiles::SaveBinFile(PUINT8 buff, UINT32 size, PCHAR file, UINT8 mode)
{
	CHAR szMode[2][8]	= { "wb", "ab" };
	FILE *fpWrite		= NULL;

	// ���� ���� ���� ����
	fpWrite	= _fsopen(file, szMode[mode], _SH_DENYNO);
	if (fpWrite)
	{
		fwrite(buff, sizeof(UINT8), size, fpWrite);
		// ���� �ݱ�
		fclose(fpWrite);
		return TRUE;
	}
	return FALSE;
}

/*
 desc : ������ ���Ե� ���� ũ�� ��� (���ڿ� ��ȯ)
 parm : file	- [in]  ��ü ��ΰ� ���Ե� ����
		data	- [out] ����(Bytes, KBytes, MBytes, GBytes)�� ���Ե� ũ�Ⱑ ����� ����
		size	- [in]  'data' ������ ũ��
 retn : TRUE or FALSE
*/
BOOL CFiles::GetFileSizeInUnits(PTCHAR file, PTCHAR data, UINT32 size)
{
	UINT32 u32Size	= 0;

	// ���� ũ�� ������
	if (0 == (u32Size = GetFileSizeAuto(file)))	return FALSE;

	// �޸� �ʱ�ȭ
	wmemset(data, 0x00, size);

	// GBytes ���� ū �������
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
 desc : �Ϲ� �����͸� �ؽ�Ʈ ���Ϸ� ���� (Ansi Code)
 parm : buff	- [in]  �����Ͱ� ����� ���� ������
		size	- [in]  �����ϰ��� �ϴ� ������ ũ��
		file	- [in]  ���ϸ� (��ü ��� ����)
		mode	- [in]  ���� ���� ���
						0x00 - ���ο� ���� ����
						0x01 - �߰��ؼ� ���� ����
 retn : None
*/
VOID CFiles::SaveTxtFileA(PCHAR buff, UINT32 size, PCHAR file, UINT8 mode)
{
	FILE *fpWrite	= NULL;

	// ���� ���� ���� ����
	if (mode)	fpWrite	= _fsopen(file, "a+t", _SH_DENYNO);
	else		fpWrite	= _fsopen(file, "w+t", _SH_DENYNO);
	if (fpWrite)
	{
		if (buff && size > 0)
		{
			fwrite(buff, sizeof(CHAR), size, fpWrite);
		}
		// ���� �ݱ�
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
	static std::mutex s_mtxFileWrite;              // �Լ� ���� ���� ��
	std::lock_guard<std::mutex> lock(s_mtxFileWrite);  // �ڵ� ��/���

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
		TRACE("ANSI ���忡���� UTF-16LE ���� �Ұ�\n");
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