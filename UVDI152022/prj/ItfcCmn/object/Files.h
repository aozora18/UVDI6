#pragma once

class CFiles
{
/* Constructor and Destructor */
public:

	CFiles();
	virtual ~CFiles();

/* Virtual Function */
protected:

public:


/* Member Parameter : Local */
protected:



/* Member Function : Local */
protected:


/* Member Function : Public */
public:

	BOOL				DeleteForceFile(PTCHAR file);
	UINT32				DeleteAllFiles(PTCHAR path);
	VOID				DeleteHistoryFiles(PTCHAR path, UINT32 days);
	BOOL				FindFile(PTCHAR file);
	BOOL				FindFile(PCHAR file);
	INT32				GetFileLines(PTCHAR file);
	PTCHAR				GetFileName(PTCHAR file, UINT32 size);
	UINT32				GetFileSize(PTCHAR file, PTCHAR mode);
	UINT32				GetFileSizeAuto(PTCHAR file);
	UINT32				GetFileSizeEx(PTCHAR file, UINT8 mode);
	BOOL				GetFileSizeInUnits(PTCHAR file, PTCHAR data, UINT32 size);
	UINT32				LoadFile(PTCHAR file, PUINT8 buff, UINT32 size, UINT8 mode);
	UINT32				LoadFileW(PTCHAR file, PTCHAR buff, UINT32 size, UINT8 mode);
	BOOL				SaveBinFile(PUINT8 buff, UINT32 size, PCHAR file, UINT8 mode);
	VOID				SaveTxtFileA(PCHAR buff, UINT32 size, PCHAR file, UINT8 mode);
	VOID				SaveTxtFileW(PTCHAR buff, UINT32 size, PTCHAR file, UINT8 mode);
};
