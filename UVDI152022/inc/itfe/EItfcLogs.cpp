
/*
 desc : The Interface Library for Log
*/

#include "pch.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : Return the most recent log message
 parm : None
 retn : Global buffer pointer where log messages are stored
*/
API_EXPORT PTCHAR uvCmn_Logs_GetErrorMesgLast()
{
	return uvLogs_GetErrorMesgLast();
}

/*
 desc : Whether an error or warning log exists
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_Logs_IsErrorMesg()
{
	return uvLogs_IsErrorMesg();
}

/*
 desc : error message reset
 parm : None
 retn : None
*/
API_EXPORT VOID uvCmn_Logs_ResetErrorMesg()
{
	uvLogs_ResetErrorMesg();
}

/*
 desc : Returns an error log message in an arbitrary location
 parm : index	- [in]  Return the error log at a specific location (0 ~ 7)
						0x00: Request the most recent error log
						[참고] The higher the value, the older the error log request.
 retn : log message (If NULL is returned, no more error logs)
*/
API_EXPORT PTCHAR uvCmn_Logs_GetErrorMesg(UINT8 index)
{
	return uvLogs_GetErrorMesg(index);
}

/*
 desc : Saving Logs - Include debugging information
 parm : apps	- [in]  Application Code (ENG_EDIC)
		type	- [in]  message type (normal, warning, error)
		mesg	- [in]  Buffer in which log generated values are stored
		file	- [in]  The name of the source code file where the message was generated (Full path)
		func	- [in]  Source code function name where the message is generated
		line	- [in]  The source code location where the message originated (Line Number)
 retn : None
*/
API_EXPORT VOID uvEng_Logs_SaveLogs(ENG_EDIC apps, ENG_LNWE type, PTCHAR mesg,
									PTCHAR file, PTCHAR func, UINT32 line)
{
	uvLogs_SaveLogs(apps, type, mesg, file, func, line);
}
API_EXPORT VOID uvEng_Logs_SaveMesgGen2I(PTCHAR mesg, PTCHAR file, PTCHAR func, UINT32 line)
{
	uvLogs_SaveLogs(ENG_EDIC::en_teldi, ENG_LNWE::en_normal, mesg, file, func, line);
}
API_EXPORT VOID uvEng_Logs_SaveWarnGen2I(PTCHAR mesg, PTCHAR file, PTCHAR func, UINT32 line)
{
	uvLogs_SaveLogs(ENG_EDIC::en_teldi, ENG_LNWE::en_warning, mesg, file, func, line);
}
API_EXPORT VOID uvEng_Logs_SaveErrorGen2I(PTCHAR mesg, PTCHAR file, PTCHAR func, UINT32 line)
{
	uvLogs_SaveLogs(ENG_EDIC::en_teldi, ENG_LNWE::en_error, mesg, file, func, line);
}

/*
 desc : Get the log files from local directory (includ full path)
 parm : files	- [out] return the file array buffer included full path
 retn : None
*/
API_EXPORT VOID uvEng_Logs_GetLogFiles(UINT8 type, CStringArray &files)
{
	uvLogs_GetLogFiles(type, files);
}

#ifdef __cplusplus
}
#endif