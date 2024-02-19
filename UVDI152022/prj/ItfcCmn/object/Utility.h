#pragma once

class CUtility
{
/* Constructor and Destructor */
public:

	CUtility();
	virtual ~CUtility();

/* Virtual Function */
protected:

public:


/* Member Parameter : Local */
protected:



/* Member Function : Local */
protected:


/* Member Function : Public */
public:

	BOOL				CComBSTR2TCHAR(BSTR source, PTCHAR target, UINT32 size);
	BOOL				CopyToClipboard(PTCHAR data, UINT32 size);
	BOOL				DecimalNumToBinaryStr(UINT64 dec_num, PUINT8 bin_num, UINT8 bin_size);
	BOOL				GetLogFont(LOGFONT &lf, PTCHAR name, UINT32 size, UINT8 lang);
	VOID				GetReverseArrayUint8(PUINT8 data, UINT32 size);
	VOID				Logfont2Charformat(LOGFONT lf, CHARFORMAT2 &cf);
	VOID				Memset(PVOID dest, UINT64 count);
	VOID				Memset64(PVOID dest, UINT64 count);
	VOID				MemsetS128(PVOID dest, UINT32 count);
	VOID				uSleep(UINT64 usec);
};
