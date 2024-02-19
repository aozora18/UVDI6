/*
 desc : Directory ���
*/

#include "pch.h"
#include "Directory.h"


#ifdef _DEBUG
#define new	DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : SHBrowserForFolder �Լ����� Ư�� ������ ���� ��ġ�ϱ� ���� �ݹ� �Լ�
 parm : 
 retn : 
*/
static int CALLBACK BrowseCallback(HWND hwnd, UINT msg, LPARAM param, LPARAM data)
{
	/* Warning�� ���ֱ� ���� ��ġ */
	param	= NULL;

	if (msg == BFFM_INITIALIZED)
	{
		/* Shell */
		HWND hShell	= GetDlgItem(hwnd, 0x00000000);
		/* Tree (Shell ���) */
		HWND hTree	= GetDlgItem(hShell, 0x00000064);
		/* Static - Title */
		if (GetDlgItem(hwnd, 0x00003742))
		{
			EnableWindow(GetDlgItem(hwnd, 0x00003742), FALSE);
			ShowWindow(GetDlgItem(hwnd, 0x00003742), SW_HIDE);
		}
		/* Button - New */
		if (GetDlgItem(hwnd, 0x00003746))
		{
			EnableWindow(GetDlgItem(hwnd, 0x00003746), FALSE);
			ShowWindow(GetDlgItem(hwnd, 0x00003746), SW_HIDE);
		}

		/* ���̾�α� ũ�� ���� (Ÿ��Ʋ (Static)���� ����ø� */
		if (hShell && hTree)
		{
			CRect rWnd;
			GetClientRect(hwnd, rWnd);
			/* Tree ũ�� ���� - Tree�� ������ Shell�� ũ�� ���� */
			MoveWindow(hShell, rWnd.left + 10, rWnd.top + 10, rWnd.Width()-22, rWnd.Height()-52, TRUE);
		}

		/* ���� ���� ���� */
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, data);
	}

	return 0;
}

/*
 desc : Constructor
 parm : None
 retn : None
*/
CDirectory::CDirectory()
{
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CDirectory::~CDirectory()
{
}

/*
 desc : Ư�� ���丮 �뷮 ��� (���� ���丮 ����)
 parm : path	- [in]  ���丮 ������ ����� CString Object (���� ��������  '\\' ����)
 retn : ũ�� ��ȯ (����: Bytes)
*/
UINT64 CDirectory::GetDirectorySize(CString path)
{
    BOOL bExsit;
	UINT64 u64Size	= 0;
    CFileFind csFileFind;
 
    bExsit = csFileFind.FindFile(path);
    while (bExsit)
    {
        bExsit = csFileFind.FindNextFileW();
 
        if (!csFileFind.IsDots())
		{
			if (csFileFind.IsDirectory())
			{
				CString strNextDir = _T("");
				strNextDir =  csFileFind.GetFilePath() + _T("\\*.*");
				GetDirectorySize(strNextDir);
			}
			else
			{
				u64Size += csFileFind.GetLength();
			}
		}
    }
    csFileFind.Close();

	/* ��ü ũ�� ��ȯ */
	return u64Size;
}

/*
 desc : ���� ���� �������� ������ �����ϴ� ���� (��ü ��� ����)��� ��ȯ
 parm : search	- [in]  �˻��ϰ��� �ϴ� ����
		paths	- [out] �˻��� ��ΰ� ����� �迭 ����Ʈ ���� ����
 retn : Zero-based ��
*/
VOID CDirectory::SearchRecursePath(CString search, CStringArray &path)
{
	BOOL bWorking		= FALSE;
	CString strFind, strSub, strResult;
	CFileFind csFind;

	// �˻� ��� �� ���� ����
	strFind.Format(_T("%s\\*.*"), search.GetBuffer());
	bWorking = csFind.FindFile(strFind);

	while (bWorking)
	{
		bWorking = (UINT8)csFind.FindNextFile();
		// . (Dot) or .. (Double dots)�� �ǳ� �ڴ�.
		if (!csFind.IsDots())
		{
			// ���� ���� ������ �����ٸ�... ���ȣ��
			if (csFind.IsDirectory())
			{
				strSub = csFind.GetFilePath();

				/* �ϴ� ���� �˻��� ��� �ӽ� ���� */
				path.Add(strSub);
				/* �ٽ� �˻��� ��� �������� ���� ���� �˻� ���� */
				SearchRecursePath(strSub, path);
			}
		}
	}

	// ��ü �ڵ� �ݱ�
	csFind.Close();
}

/*
 desc : ���� ���� ���� ����Ǿ� �ִ� ���� ���(���� ���� ����) ���ϱ�
 parm : search	- [in]  �˻��ϰ��� �ϴ� ����
		filter	- [in]  �˻��ϰ��� �ϴ� ���� ����(���� Ȥ�� Ư�� ���� ���)
						��� ������ �˻��Ϸ��� L"" (Empty) ���̸� ��
		file	- [out] �˻��� ���� ������ ����� ��Ʈ�� �迭 ���� ����
		recurse	- [in]  ���� ���� �˻� ���� (0x01, 0x00)
 retn : None
*/
VOID CDirectory::SearchRecurseFile(CString search, CString filter, CStringArray &file, UINT8 recurse)
{
	BOOL bWorking	= FALSE;
	CString strFind, strSub, strResult;
	CFileFind csFind;

	// �˻� ��� �� ���� ����
	strFind.Format(_T("%s\\*.*"), search.GetBuffer());
	bWorking = csFind.FindFile(strFind);

	while (bWorking)
	{
		bWorking = (UINT8)csFind.FindNextFile();
		// . (Dot) or .. (Double dots)�� �ǳ� �ڴ�.
		if (!csFind.IsDots())
		{
			// ���� ���� ������ �����ٸ�... ���ȣ��
			if (csFind.IsDirectory())
			{
				if (recurse)
				{
					strSub = csFind.GetFilePath();
					SearchRecurseFile(strSub, filter, file, recurse);
				}
			}
			// ������ �ƴϰ� �����̸� ����۾� ����
			strResult	= csFind.GetFilePath().MakeLower();
			if (filter.IsEmpty())
			{
				file.Add(strResult);
			}
			else
			{
				if (-1 != strResult.Find(filter.MakeLower()))	file.Add(strResult);
			}
		}
	}

	// ��ü �ڵ� �ݱ�
	csFind.Close();
}

/*
 desc : Ư�� Recipe ���� ���� ����Ǿ� �ִ� ���� ��� ���ϱ�
 parm : path	- [in]  �˻��ϰ��� �ϴ� ��Ʈ ����
		filter	- [in]  �˻��ϰ��� �ϴ� ����(���� Ȥ�� Ư�� ���� ���)
		file	- [out] �˻��� ���� ������ ����� ��Ʈ�� �迭 ���� ����
 retn : None
*/
VOID CDirectory::SearchFile(CString path, CString filter, CStringArray &file)
{
	BOOL bWorking	= FALSE;
	CString strExt, strFile, strFilter;
	CFileFind csFind;

	// �˻� ��� �� ���� ����
	strFilter.Format(_T("%s/%s"), path.GetBuffer(), filter.GetBuffer());
	bWorking = csFind.FindFile(strFilter);

	while (bWorking)
	{
		bWorking = (UINT8)csFind.FindNextFile();
		// ������ �ƴϰ� �����̸� ����۾� ����
		strFile	= csFind.GetFileName().MakeLower();
		file.Add(strFile);
	}

	// ��ü �ڵ� �ݱ�
	csFind.Close();
}

/*
 desc : ���� ������ ��� ���� ��ȯ
 parm : path	- [in]  �����Ϸ��� ��� ����
		ppidl	- [out] ������ ��� ������ ����� ITEMIDLIST ����ü ���� ������
 retn : TRUE or FALSE
*/
BOOL CDirectory::GetSHPathToPIDL(PTCHAR path, LPITEMIDLIST *ppidl)
{
	LPSHELLFOLDER pstShellFolder	= NULL;
	ULONG ulCharsParsed				= 0;
	HRESULT hRet					= S_OK;

	hRet	= SHGetDesktopFolder(&pstShellFolder);
	if (FAILED(hRet))	return FALSE;
	hRet	= pstShellFolder->ParseDisplayName(NULL, NULL, path, &ulCharsParsed, ppidl, NULL);
	if (FAILED(hRet))	return FALSE;

	return TRUE;
}

/*
 desc : ���丮(����) ����
 parm : path	- [in]  ������ ���丮 (���� ���丮 ����)
		lpsa	- [in]  ���� �Ӽ� ������ ����� ����ü ������
 retn : TRUE or FALSE
*/
BOOL CDirectory::CreateDirectory(LPCTSTR path, LPSECURITY_ATTRIBUTES lpsa)
{
	TCHAR tzMesg[1024]	= {NULL};

//	ASSERT(path);
	INT32 i32Ret = SHCreateDirectoryEx(NULL, path, lpsa);
	if (i32Ret== ERROR_SUCCESS || i32Ret == ERROR_ALREADY_EXISTS)	return TRUE;

	swprintf_s(tzMesg, 1024, _T("Failed to create the [ %s ] directory"), path);
	AfxMessageBox(tzMesg, MB_ICONSTOP|MB_TOPMOST);

	return FALSE;
}

/*
 desc : ���� ���� ���� �۾� ��� �������� ��� ��, ����ڿ� ���� ���õ� ����(���)�� ��ȯ
 parm : hwnd	- [in]  �ڽ��� ���� �ڵ�
		path	- [out] ���õ� ����(���)�� ����� ����
 retn : TRUE or FALSE (���� �������� ����. ��� ��ư ���� ���)
*/
BOOL CDirectory::GetSelectPathEx(HWND hwnd, PTCHAR path)
{
	TCHAR tzCurDir[MAX_PATH_LEN]	= _T("");
	TCHAR tzPathName[MAX_PATH_LEN]	= _T("");
	ULONG ulCharsParsed	= 0;
	LPITEMIDLIST pidlRoot, pDirList;
	BROWSEINFO stInfo;
	LPSHELLFOLDER pShellFolder	= NULL;

	/* ���� ��� ���ͼ� �����ϱ� ���� �۾� */
	HRESULT hRet	= SHGetDesktopFolder(&pShellFolder);
	if (FAILED(hRet))	return FALSE;
	GetCurrentDirectory(MAX_PATH_LEN, tzCurDir);
	pShellFolder->ParseDisplayName(NULL, NULL, tzCurDir, &ulCharsParsed, &pidlRoot, NULL);

	/* Browse �⺻ �� ���� �� ���� ���� */
	stInfo.hwndOwner		= hwnd;
	stInfo.pidlRoot			= pidlRoot;
	stInfo.lpszTitle		= _T("Select Directory");
	stInfo.pszDisplayName	= tzPathName;
	stInfo.ulFlags			= BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_RETURNONLYFSDIRS;	/*BIF_RETURNONLYFSDIRS;*/
	stInfo.lpfn				= NULL;
	stInfo.lParam			= 0;

	pDirList = SHBrowseForFolder(&stInfo);
	if(pDirList == NULL) return FALSE;

	::SHGetPathFromIDList(pDirList, path);

	/* �ٽ� ���� ���丮 ���� */
	SetCurrentDirectory(tzCurDir);

	return TRUE;
}

/*
 desc : ���� �������� ���̾�α� ��ȭ���� ��� ��, ����ڿ� ���� ���õ� ����(���)�� ��ȯ
 parm : hwnd	- [in]  �ڽ��� ���� �ڵ�
		path	- [out] ���õ� ����(���)�� ����� ����
 retn : TRUE or FALSE (���� �������� ����. ��� ��ư ���� ���)
*/
BOOL CDirectory::GetSelectPath(HWND hwnd, PTCHAR path)
{
	TCHAR tzCurDir[MAX_PATH_LEN]	= _T("");
	TCHAR tzPathName[MAX_PATH_LEN]	= _T("");
	BROWSEINFO stInfo				= {NULL};;
	LPSHELLFOLDER pShellFolder		= NULL;
	LPITEMIDLIST pDirList;

	/* ���� ��� ���ͼ� �����ϱ� ���� �۾� */
	HRESULT hRet	= SHGetDesktopFolder(&pShellFolder);
	if (FAILED(hRet))	return FALSE;
	GetCurrentDirectory(MAX_PATH_LEN, tzCurDir);

	/* Browse �⺻ �� ���� �� ���� ���� */
	stInfo.hwndOwner		= hwnd;
	stInfo.lpszTitle		= _T("Select Directory");
	stInfo.ulFlags			= BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_RETURNONLYFSDIRS;

	pDirList = SHBrowseForFolder(&stInfo);
	if(pDirList == NULL) return FALSE;

	::SHGetPathFromIDList(pDirList, path);

	/* �ٽ� ���� ���丮 ���� */
	SetCurrentDirectory(tzCurDir);

	return TRUE;
}

/*
 desc : �ش� ���丮(���)�� �����ϴ��� ����
 parm : path	- [in]  ���丮(���) ��
 retn : TRUE or FALSE
*/
BOOL CDirectory::FindPath(PTCHAR path)
{
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	BOOL u8Find			= FALSE;
	CFileFind csFind;

	// �˻� ���� ����
	swprintf_s(tzFile, MAX_PATH_LEN, _T("%s\\*.*"), path);
	// ����(���丮) �˻�
	u8Find	= (BOOL)csFind.FindFile(tzFile);
	// �ݱ�
	csFind.Close();

	return u8Find;
}

/*
 desc : �ش� ���丮(���) ���� �����ϴ� ���� �������� ���� ��ȯ
 parm : path	- [in]  ���丮(���) ��
 retn : 0 - ����, ��� - ����
*/
UINT32 CDirectory::GetChildPathCount(PTCHAR path)
{
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	UINT32 u32Count = 0, u32Find= 0;
	CFileFind csFind;

	// �˻� ���� ����
	swprintf_s(tzFile, MAX_PATH_LEN, _T("%s\\*.*"), path);

	// ���� ���� �˻�
	u32Find	= csFind.FindFile(tzFile);
	while (u32Find)
	{
		u32Find	= csFind.FindNextFile();

		// ���� ���� ����
		if (!csFind.IsDots() && csFind.IsDirectory())
		{
			u32Count++;
		}
	}

	// �ݱ�
	csFind.Close();

	return u32Count;
}

/*
 desc : �ش� ���丮(���) ���� �����ϴ� ���� �������� ���� ��ȯ
 parm : path	- [in]  ���丮(���) ��
		data	- [out] ���丮 ����Ʈ�� �����
 retn : None
*/
VOID CDirectory::GetChildPathList(PTCHAR path, CStringArray &data)
{
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	BOOL u8Find			= 0;
	CFileFind csFind;

	// �˻� ���� ����
	swprintf_s(tzFile, MAX_PATH_LEN, _T("%s\\*.*"), path);

	// ���� ���� �˻�
	u8Find	= (BOOL)csFind.FindFile(tzFile);
	while (u8Find)
	{
		u8Find	= (BOOL)csFind.FindNextFile();
		if (!csFind.IsDots() && csFind.IsDirectory())	data.Add(csFind.GetFilePath().MakeLower());
	}
	// �ݱ�
	csFind.Close();
}

/*
 desc : ������ ����� ���� ����
 parm : path	- [in]  ������ �ϴ� ��� �� (��ü ��� ����)
 retn : None
*/
VOID CDirectory::OpenPath(PTCHAR path)
{
	::ShellExecute(NULL, L"open", NULL, NULL, path, SW_SHOW);
}

/*
 desc : ���丮 ���� (���� ���丮���� �����Ͽ� ������)
 parm : path	- [in]  ���ŵ� ���丮 (��ü ��θ� ���� ���丮)
 retn : TRUE or FALSE
*/
BOOL CDirectory::RemoveDirectory(LPCTSTR path)
{
//	ASSERT(path);

	TCHAR szDirectory[MAX_PATH_LEN]	= {NULL};
	SHFILEOPSTRUCT fos				= {NULL};

	_tcscpy_s(szDirectory, MAX_PATH_LEN, path);
	szDirectory[_tcslen(szDirectory) + 1] = 0;

	fos.wFunc	= FO_DELETE;
	fos.fFlags	= FOF_NO_UI;
	fos.pFrom	= szDirectory;

	return (ERROR_SUCCESS == SHFileOperation(&fos));
}

/*
 desc : ����� ���� ���� (����̺�) �������� �ʱ� ��ȭ ���� ����ϰ�, ����ڿ� ���� ���õ� ����(���)�� ��ȯ
 parm : hwnd	- [in]  �ڽ��� ���� �ڵ�
		init	- [in]  �ʱ� ���õ� ��ġ�� �̵��� ���丮 �� (��ü ��� ����)
		path	- [out] ���õ� ����(���)�� ����� ����
 retn : TRUE or FALSE (���� �������� ����. ��� ��ư ���� ���)
*/
BOOL CDirectory::RootSelectPath(HWND hwnd, PTCHAR init, PTCHAR path)
{
	TCHAR tzPathName[MAX_PATH_LEN]	= _T("");
	LPITEMIDLIST pDirList;
	BROWSEINFO stInfo	= {NULL};

	stInfo.hwndOwner		= hwnd;
	stInfo.pidlRoot			= NULL;
	stInfo.lpfn				= BrowseCallback;
	stInfo.lParam			= (LPARAM)init;
	stInfo.pszDisplayName	= tzPathName;
	stInfo.lpszTitle		= _T("Select Directory");
	stInfo.ulFlags			= BIF_NEWDIALOGSTYLE |
							  BIF_RETURNONLYFSDIRS |
#if 0
							  BIF_EDITBOX |
#else
							  BIF_DONTGOBELOWDOMAIN |
#endif
							  BIF_VALIDATE;

	// ���̾�α׸� ����
	pDirList = ::SHBrowseForFolder(&stInfo);
	if( pDirList != NULL)
	{
		// �н��� ����
		::SHGetPathFromIDList(pDirList, path);
		return TRUE;
	}

	return FALSE;
}

/*
 desc : ��ü ��θ� ���� ������ ��ο� ���Ϸ� �и��Ͽ� ��ȯ
 parm : full_file	- [in]  ��ü ��ΰ� ���Ե� ���ϸ�
		full_size	- [in]  'path_file' ���ۿ� ����� �������� ũ��
		path_name	- [out] ��ü ��� ����� ���� ������
		path_size	- [in]  'path_name' ������ ũ��
		file_name	- [out] ���ϸ�(Ȯ���� ����) ����� ���� ������
		file_size	- [in]  'file_name' ������ ũ��
 retn : TRUE or FALSE
*/
BOOL CDirectory::GetPathFile(PTCHAR full_file, UINT32 full_size, PTCHAR path_name,
							 UINT32 path_size, PTCHAR file_name, UINT32 file_size)
{
	BOOL bSucc	= FALSE;
	UINT32 i;

	// ��ȯ�� ���� �ʱ�ȭ
	if (path_name)	wmemset(path_name, 0x00, path_size);
	if (file_name)	wmemset(file_name, 0x00, file_size);

	// ��� ������ �߸��� �����
	if (full_size < 3)	return FALSE;

	/* ----------------------------------------------------------------------------- */
	/* i ���� ���� 0 �� ���� �۾ƾ� ������, ���丮 ������ �ּ� 4�� ��, ����̺��, */
	/* �ݷ�, ������ �� ���ϸ� ���� �ʿ��ϹǷ�, -1 ������ �˻��ؾ��� �ʿ伺�� ����.   */
	/* ----------------------------------------------------------------------------- */
	for (i=full_size-1; i; i--)
	{
		if (full_file[i] == '\\')	break;
	}

	// �ּҷ� ���丮�� ���Ϸ� ������ �Ǿ� �ִ��� Ȯ��
	if (i > 3)
	{
		// ���丮�� ������ �и��Ͽ� ����
		if (path_name)	wmemcpy_s(path_name, path_size, full_file, i);
		if (file_name)	wmemcpy_s(file_name, file_size, full_file+i+1, full_size - i - 1);
		// ��� ���������� ����Ǿ� �ִ��� Ȯ��
		if ((path_name && (_tcslen(path_name) > 0)) || (file_name && (_tcslen(file_name) > 0)))	bSucc = TRUE;
	}

	return bSucc;
}

/*
 desc : Returns the selected file or files
 parm : filter	- [in]  Filter string to search for. (Must keep this format)
						ex> Image Files (*.bmp, *.gif, *.jpg) | *.bmp;*.gif;*.jpg
						    Image File (*.bmp) | *.bmp
		size	- [in]  The size of 'file'
						Size of allocated memory when multiple files are stored
		file	- [out] Buffer which the selected file(or files) will be stored
		title	- [in]  The title name of dialog box
		init	- [in]  The initial directory that you want to open
 retn : TRUE or FALSE (���� �������� ����. ��� ��ư ���� ���)
*/
BOOL CDirectory::GetSelectFiles(PTCHAR filter, UINT32 size, PTCHAR file, PTCHAR title, PTCHAR init)
{
	BOOL bSucc	= FALSE;
	TCHAR tzCurPath[_MAX_PATH]	= {NULL}, tzFilter[_MAX_PATH] = {NULL};

	/* ���� ���丮 �ӽ� ���� */
	GetCurrentDirectory(_MAX_PATH, tzCurPath);

	swprintf_s(tzFilter, _MAX_PATH, L"%s|All Files (*.*)|*.*||", filter);
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT, tzFilter);
	dlgFile.m_ofn.lpstrFile	= file;
	dlgFile.m_ofn.nMaxFile	= size;
	if (title)	dlgFile.m_ofn.lpstrTitle		= title;
	if (init)	dlgFile.m_ofn.lpstrInitialDir	= init;
	if (IDOK == dlgFile.DoModal())	bSucc= TRUE;

	/* ���� ���丮 ���� */
	SetCurrentDirectory(tzCurPath);

	return bSucc;
}
BOOL CDirectory::GetSelectFile(PTCHAR filter, UINT32 size, PTCHAR file, PTCHAR title, PTCHAR init)
{
	BOOL bSucc	= FALSE;
	TCHAR tzCurPath[_MAX_PATH]	= {NULL}, tzFilter[_MAX_PATH] = {NULL};

	/* ���� ���丮 �ӽ� ���� */
	GetCurrentDirectory(_MAX_PATH, tzCurPath);

	swprintf_s(tzFilter, _MAX_PATH, L"%s|All Files (*.*)|*.*||", filter);
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_EXPLORER|OFN_LONGNAMES, tzFilter);
	if (title)	dlgFile.m_ofn.lpstrTitle		= title;
	if (init)	dlgFile.m_ofn.lpstrInitialDir	= init;
	if (IDOK == dlgFile.DoModal())
	{
		/* File name containing the full path */
		swprintf_s(file, size, L"%s", dlgFile.GetPathName().GetBuffer());
		bSucc= TRUE;
	}
	/* ���� ���丮 ���� */
	SetCurrentDirectory(tzCurPath);

	return bSucc;
}
