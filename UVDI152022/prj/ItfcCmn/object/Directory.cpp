/*
 desc : Directory 모듈
*/

#include "pch.h"
#include "Directory.h"


#ifdef _DEBUG
#define new	DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : SHBrowserForFolder 함수에서 특정 폴더를 시작 위치하기 위한 콜백 함수
 parm : 
 retn : 
*/
static int CALLBACK BrowseCallback(HWND hwnd, UINT msg, LPARAM param, LPARAM data)
{
	/* Warning을 없애기 위해 조치 */
	param	= NULL;

	if (msg == BFFM_INITIALIZED)
	{
		/* Shell */
		HWND hShell	= GetDlgItem(hwnd, 0x00000000);
		/* Tree (Shell 기반) */
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

		/* 다이얼로그 크기 조정 (타이틀 (Static)까지 끌어올림 */
		if (hShell && hTree)
		{
			CRect rWnd;
			GetClientRect(hwnd, rWnd);
			/* Tree 크기 조정 - Tree를 가지는 Shell의 크기 조정 */
			MoveWindow(hShell, rWnd.left + 10, rWnd.top + 10, rWnd.Width()-22, rWnd.Height()-52, TRUE);
		}

		/* 시작 폴더 지정 */
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
 desc : 특정 디렉토리 용량 얻기 (하위 디렉토리 포함)
 parm : path	- [in]  디렉토리 정보가 저장된 CString Object (폴더 마지막에  '\\' 제외)
 retn : 크기 반환 (단위: Bytes)
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

	/* 전체 크기 반환 */
	return u64Size;
}

/*
 desc : 임의 폴더 기준으로 하위에 존재하는 폴더 (전체 경로 포함)명들 반환
 parm : search	- [in]  검색하고자 하는 폴더
		paths	- [out] 검색된 경로가 저장될 배열 리스트 참조 변수
 retn : Zero-based 값
*/
VOID CDirectory::SearchRecursePath(CString search, CStringArray &path)
{
	BOOL bWorking		= FALSE;
	CString strFind, strSub, strResult;
	CFileFind csFind;

	// 검색 경로 및 필터 설정
	strFind.Format(_T("%s\\*.*"), search.GetBuffer());
	bWorking = csFind.FindFile(strFind);

	while (bWorking)
	{
		bWorking = (UINT8)csFind.FindNextFile();
		// . (Dot) or .. (Double dots)는 건너 뛴다.
		if (!csFind.IsDots())
		{
			// 만약 하위 폴더를 만났다면... 재귀호출
			if (csFind.IsDirectory())
			{
				strSub = csFind.GetFilePath();

				/* 일단 현재 검색된 경로 임시 저장 */
				path.Add(strSub);
				/* 다시 검색된 경로 기준으로 하위 폴더 검색 진행 */
				SearchRecursePath(strSub, path);
			}
		}
	}

	// 객체 핸들 닫기
	csFind.Close();
}

/*
 desc : 임의 폴더 내에 저장되어 있는 파일 목록(하위 폴더 포함) 구하기
 parm : search	- [in]  검색하고자 하는 폴더
		filter	- [in]  검색하고자 하는 파일 정보(파일 혹은 특수 문자 등등)
						모든 파일을 검색하려면 L"" (Empty) 값이면 됨
		file	- [out] 검색된 파일 정보가 저장될 스트링 배열 참조 변수
		recurse	- [in]  하위 디렉토 검사 여부 (0x01, 0x00)
 retn : None
*/
VOID CDirectory::SearchRecurseFile(CString search, CString filter, CStringArray &file, UINT8 recurse)
{
	BOOL bWorking	= FALSE;
	CString strFind, strSub, strResult;
	CFileFind csFind;

	// 검색 경로 및 필터 설정
	strFind.Format(_T("%s\\*.*"), search.GetBuffer());
	bWorking = csFind.FindFile(strFind);

	while (bWorking)
	{
		bWorking = (UINT8)csFind.FindNextFile();
		// . (Dot) or .. (Double dots)는 건너 뛴다.
		if (!csFind.IsDots())
		{
			// 만약 하위 폴더를 만났다면... 재귀호출
			if (csFind.IsDirectory())
			{
				if (recurse)
				{
					strSub = csFind.GetFilePath();
					SearchRecurseFile(strSub, filter, file, recurse);
				}
			}
			// 폴더가 아니고 파일이면 등록작업 진행
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

	// 객체 핸들 닫기
	csFind.Close();
}

/*
 desc : 특정 Recipe 폴더 내에 저장되어 있는 파일 목록 구하기
 parm : path	- [in]  검색하고자 하는 루트 폴더
		filter	- [in]  검색하고자 하는 정보(파일 혹은 특수 문자 등등)
		file	- [out] 검색된 파일 정보가 저장될 스트링 배열 참조 변수
 retn : None
*/
VOID CDirectory::SearchFile(CString path, CString filter, CStringArray &file)
{
	BOOL bWorking	= FALSE;
	CString strExt, strFile, strFilter;
	CFileFind csFind;

	// 검색 경로 및 필터 설정
	strFilter.Format(_T("%s/%s"), path.GetBuffer(), filter.GetBuffer());
	bWorking = csFind.FindFile(strFilter);

	while (bWorking)
	{
		bWorking = (UINT8)csFind.FindNextFile();
		// 폴더가 아니고 파일이면 등록작업 진행
		strFile	= csFind.GetFileName().MakeLower();
		file.Add(strFile);
	}

	// 객체 핸들 닫기
	csFind.Close();
}

/*
 desc : 현재 선택한 경로 정보 반환
 parm : path	- [in]  선택하려는 경로 정보
		ppidl	- [out] 선택한 경로 정보가 저장된 ITEMIDLIST 구조체 참조 포인터
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
 desc : 디렉토리(폴더) 생성
 parm : path	- [in]  생성될 디렉토리 (상위 디렉토리 포함)
		lpsa	- [in]  보안 속성 정보가 저장된 구조체 포인터
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
 desc : 현재 실행 파일 작업 경로 기준으로 출력 후, 사용자에 의해 선택된 폴더(경로)명 반환
 parm : hwnd	- [in]  자신의 윈도 핸들
		path	- [out] 선택된 폴더(경로)가 저장될 버퍼
 retn : TRUE or FALSE (폴더 선택하지 않음. 취소 버튼 누른 경우)
*/
BOOL CDirectory::GetSelectPathEx(HWND hwnd, PTCHAR path)
{
	TCHAR tzCurDir[MAX_PATH_LEN]	= _T("");
	TCHAR tzPathName[MAX_PATH_LEN]	= _T("");
	ULONG ulCharsParsed	= 0;
	LPITEMIDLIST pidlRoot, pDirList;
	BROWSEINFO stInfo;
	LPSHELLFOLDER pShellFolder	= NULL;

	/* 현재 경로 얻어와서 설정하기 위한 작업 */
	HRESULT hRet	= SHGetDesktopFolder(&pShellFolder);
	if (FAILED(hRet))	return FALSE;
	GetCurrentDirectory(MAX_PATH_LEN, tzCurDir);
	pShellFolder->ParseDisplayName(NULL, NULL, tzCurDir, &ulCharsParsed, &pidlRoot, NULL);

	/* Browse 기본 값 설정 및 폴더 선택 */
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

	/* 다시 원래 디렉토리 복귀 */
	SetCurrentDirectory(tzCurDir);

	return TRUE;
}

/*
 desc : 윈도 기준으로 다이얼로그 대화상자 출력 후, 사용자에 의해 선택된 폴더(경로)명 반환
 parm : hwnd	- [in]  자신의 윈도 핸들
		path	- [out] 선택된 폴더(경로)가 저장될 버퍼
 retn : TRUE or FALSE (폴더 선택하지 않음. 취소 버튼 누른 경우)
*/
BOOL CDirectory::GetSelectPath(HWND hwnd, PTCHAR path)
{
	TCHAR tzCurDir[MAX_PATH_LEN]	= _T("");
	TCHAR tzPathName[MAX_PATH_LEN]	= _T("");
	BROWSEINFO stInfo				= {NULL};;
	LPSHELLFOLDER pShellFolder		= NULL;
	LPITEMIDLIST pDirList;

	/* 현재 경로 얻어와서 설정하기 위한 작업 */
	HRESULT hRet	= SHGetDesktopFolder(&pShellFolder);
	if (FAILED(hRet))	return FALSE;
	GetCurrentDirectory(MAX_PATH_LEN, tzCurDir);

	/* Browse 기본 값 설정 및 폴더 선택 */
	stInfo.hwndOwner		= hwnd;
	stInfo.lpszTitle		= _T("Select Directory");
	stInfo.ulFlags			= BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_RETURNONLYFSDIRS;

	pDirList = SHBrowseForFolder(&stInfo);
	if(pDirList == NULL) return FALSE;

	::SHGetPathFromIDList(pDirList, path);

	/* 다시 원래 디렉토리 복귀 */
	SetCurrentDirectory(tzCurDir);

	return TRUE;
}

/*
 desc : 해당 디렉토리(경로)가 존재하는지 유무
 parm : path	- [in]  디렉토리(경로) 명
 retn : TRUE or FALSE
*/
BOOL CDirectory::FindPath(PTCHAR path)
{
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	BOOL u8Find			= FALSE;
	CFileFind csFind;

	// 검색 파일 설정
	swprintf_s(tzFile, MAX_PATH_LEN, _T("%s\\*.*"), path);
	// 폴더(디렉토리) 검색
	u8Find	= (BOOL)csFind.FindFile(tzFile);
	// 닫기
	csFind.Close();

	return u8Find;
}

/*
 desc : 해당 디렉토리(경로) 내에 존재하는 하위 폴더들의 개수 반환
 parm : path	- [in]  디렉토리(경로) 명
 retn : 0 - 없음, 양수 - 개수
*/
UINT32 CDirectory::GetChildPathCount(PTCHAR path)
{
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	UINT32 u32Count = 0, u32Find= 0;
	CFileFind csFind;

	// 검색 파일 설정
	swprintf_s(tzFile, MAX_PATH_LEN, _T("%s\\*.*"), path);

	// 하위 폴더 검색
	u32Find	= csFind.FindFile(tzFile);
	while (u32Find)
	{
		u32Find	= csFind.FindNextFile();

		// 폴더 개수 증가
		if (!csFind.IsDots() && csFind.IsDirectory())
		{
			u32Count++;
		}
	}

	// 닫기
	csFind.Close();

	return u32Count;
}

/*
 desc : 해당 디렉토리(경로) 내에 존재하는 하위 폴더들의 정보 반환
 parm : path	- [in]  디렉토리(경로) 명
		data	- [out] 디렉토리 리스트가 저장됨
 retn : None
*/
VOID CDirectory::GetChildPathList(PTCHAR path, CStringArray &data)
{
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	BOOL u8Find			= 0;
	CFileFind csFind;

	// 검색 파일 설정
	swprintf_s(tzFile, MAX_PATH_LEN, _T("%s\\*.*"), path);

	// 하위 폴더 검색
	u8Find	= (BOOL)csFind.FindFile(tzFile);
	while (u8Find)
	{
		u8Find	= (BOOL)csFind.FindNextFile();
		if (!csFind.IsDots() && csFind.IsDirectory())	data.Add(csFind.GetFilePath().MakeLower());
	}
	// 닫기
	csFind.Close();
}

/*
 desc : 지정한 경로의 폴더 열기
 parm : path	- [in]  열고자 하는 경로 명 (전체 경로 포함)
 retn : None
*/
VOID CDirectory::OpenPath(PTCHAR path)
{
	::ShellExecute(NULL, L"open", NULL, NULL, path, SW_SHOW);
}

/*
 desc : 디렉토리 삭제 (하위 디렉토리까지 포함하여 삭제됨)
 parm : path	- [in]  제거될 디렉토리 (전체 경로를 가진 디렉토리)
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
 desc : 사용자 지정 폴더 (드라이브) 기준으로 초기 대화 상자 출력하고, 사용자에 의해 선택된 폴더(경로)명 반환
 parm : hwnd	- [in]  자신의 윈도 핸들
		init	- [in]  초기 선택된 위치로 이동될 디렉토리 명 (전체 경로 포함)
		path	- [out] 선택된 폴더(경로)가 저장될 버퍼
 retn : TRUE or FALSE (폴더 선택하지 않음. 취소 버튼 누른 경우)
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

	// 다이얼로그를 띄우기
	pDirList = ::SHBrowseForFolder(&stInfo);
	if( pDirList != NULL)
	{
		// 패스를 얻어옴
		::SHGetPathFromIDList(pDirList, path);
		return TRUE;
	}

	return FALSE;
}

/*
 desc : 전체 경로를 가진 파일을 경로와 파일로 분리하여 반환
 parm : full_file	- [in]  전체 경로가 포함된 파일명
		full_size	- [in]  'path_file' 버퍼에 저장된 데이터의 크기
		path_name	- [out] 전체 경로 저장될 버퍼 포인터
		path_size	- [in]  'path_name' 버퍼의 크기
		file_name	- [out] 파일명(확장자 포함) 저장될 버퍼 포인터
		file_size	- [in]  'file_name' 버퍼의 크기
 retn : TRUE or FALSE
*/
BOOL CDirectory::GetPathFile(PTCHAR full_file, UINT32 full_size, PTCHAR path_name,
							 UINT32 path_size, PTCHAR file_name, UINT32 file_size)
{
	BOOL bSucc	= FALSE;
	UINT32 i;

	// 반환할 버퍼 초기화
	if (path_name)	wmemset(path_name, 0x00, path_size);
	if (file_name)	wmemset(file_name, 0x00, file_size);

	// 경로 정보가 잘못된 경우임
	if (full_size < 3)	return FALSE;

	/* ----------------------------------------------------------------------------- */
	/* i 값이 원래 0 값 보다 작아야 하지만, 디렉토리 구성상 최소 4개 즉, 드라이브명, */
	/* 콜론, 슬래쉬 및 파일명 등이 필요하므로, -1 까지는 검색해야할 필요성은 없다.   */
	/* ----------------------------------------------------------------------------- */
	for (i=full_size-1; i; i--)
	{
		if (full_file[i] == '\\')	break;
	}

	// 최소로 디렉토리와 파일로 구성이 되어 있는지 확인
	if (i > 3)
	{
		// 디렉토리와 파일을 분리하여 저장
		if (path_name)	wmemcpy_s(path_name, path_size, full_file, i);
		if (file_name)	wmemcpy_s(file_name, file_size, full_file+i+1, full_size - i - 1);
		// 모두 정상적으로 저장되어 있는지 확인
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
 retn : TRUE or FALSE (파일 선택하지 않음. 취소 버튼 누른 경우)
*/
BOOL CDirectory::GetSelectFiles(PTCHAR filter, UINT32 size, PTCHAR file, PTCHAR title, PTCHAR init)
{
	BOOL bSucc	= FALSE;
	TCHAR tzCurPath[_MAX_PATH]	= {NULL}, tzFilter[_MAX_PATH] = {NULL};

	/* 현재 디렉토리 임시 저장 */
	GetCurrentDirectory(_MAX_PATH, tzCurPath);

	swprintf_s(tzFilter, _MAX_PATH, L"%s|All Files (*.*)|*.*||", filter);
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT, tzFilter);
	dlgFile.m_ofn.lpstrFile	= file;
	dlgFile.m_ofn.nMaxFile	= size;
	if (title)	dlgFile.m_ofn.lpstrTitle		= title;
	if (init)	dlgFile.m_ofn.lpstrInitialDir	= init;
	if (IDOK == dlgFile.DoModal())	bSucc= TRUE;

	/* 기존 디렉토리 복원 */
	SetCurrentDirectory(tzCurPath);

	return bSucc;
}
BOOL CDirectory::GetSelectFile(PTCHAR filter, UINT32 size, PTCHAR file, PTCHAR title, PTCHAR init)
{
	BOOL bSucc	= FALSE;
	TCHAR tzCurPath[_MAX_PATH]	= {NULL}, tzFilter[_MAX_PATH] = {NULL};

	/* 현재 디렉토리 임시 저장 */
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
	/* 기존 디렉토리 복원 */
	SetCurrentDirectory(tzCurPath);

	return bSucc;
}
