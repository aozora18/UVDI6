#pragma once

class CStringEx
{
/* Constructor and Destructor */
public:

	CStringEx();
	virtual ~CStringEx();

/* Virtual Function */
protected:

public:


/* Member Parameter : Local */
protected:



/* Member Function : Local */
protected:


/* Member Function : Public */
public:

#ifdef _UNICODE
	INT32				GetFindCh(PTCHAR buff, INT32 size, TCHAR ch);
	UINT32				GetFindChCount(PTCHAR buff, UINT32 size, TCHAR ch);
#else
	INT32				GetFindCh(CHAR *buff, INT32 size, CHAR ch);
	UINT32				GetFindChCount(CHAR *buff, UINT32 size, CHAR ch);
#endif
	UINT8				GetHexCharToNum(CHAR ch);
	UINT8				GetHexNumToChar(UINT8 num);
	BOOL				IsSpaceInString(PTCHAR str);
	UINT32				SetMakePktToMesg(PCHAR mesg, UINT32 mlen, PUINT8 data, UINT32 dlen);
	INT32				SortMixedString(const VOID *p1, const VOID *p2);
};
