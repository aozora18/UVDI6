#pragma once

class CDirectory
{
/* Constructor and Destructor */
public:

	CDirectory();
	virtual ~CDirectory();

/* Virtual Function */
protected:

public:


/* Member Parameter : Local */
protected:



/* Member Function : Local */
protected:


/* Member Function : Public */
public:

	BOOL				CreateDirectory(LPCTSTR path, LPSECURITY_ATTRIBUTES lpsa);
	BOOL				FindPath(PTCHAR path);
	UINT32				GetChildPathCount(PTCHAR path);
	VOID				GetChildPathList(PTCHAR path, CStringArray &data);
	UINT64				GetDirectorySize(CString path);
	BOOL				GetPathFile(PTCHAR full_file, UINT32 full_size, PTCHAR path_name,
									UINT32 path_size, PTCHAR file_name, UINT32 file_size);
	BOOL				GetSelectFiles(PTCHAR filter, UINT32 size, PTCHAR file, PTCHAR title, PTCHAR init);
	BOOL				GetSelectFile(PTCHAR filter, UINT32 size, PTCHAR file, PTCHAR title, PTCHAR init);
	BOOL				GetSelectPathEx(HWND hwnd, PTCHAR path);
	BOOL				GetSelectPath(HWND hwnd, PTCHAR path);
	BOOL				GetSHPathToPIDL(PTCHAR path, LPITEMIDLIST *ppidl);
	VOID				OpenPath(PTCHAR path);
	BOOL				RemoveDirectory(LPCTSTR path);
	BOOL				RootSelectPath(HWND hwnd, PTCHAR init, PTCHAR path);
	VOID				SearchRecursePath(CString search, CStringArray &path);
	VOID				SearchRecurseFile(CString search, CString filter, CStringArray &file, UINT8 recurse);
	VOID				SearchFile(CString path, CString filter, CStringArray &file);
};
