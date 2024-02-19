
/*
 desc : The Interface Library for Log
*/

#pragma once

#include "../conf/global.h"
#include "../conf/define.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : Return the most recent log message
 parm : None
 retn : Global buffer pointer where log messages are stored
*/
API_IMPORT PTCHAR uvCmn_Logs_GetErrorMesgLast();
/*
 desc : Whether an error or warning log exists
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Logs_IsErrorMesg();
/*
 desc : error message reset
 parm : None
 retn : None
*/
API_IMPORT VOID uvCmn_Logs_ResetErrorMesg();
/*
 desc : Returns an error log message in an arbitrary location
 parm : index	- [in]  Return the error log at a specific location (0 ~ 7)
 						0x00: Request the most recent error log
 						[참고] The higher the value, the older the error log request.
 retn : log message (If NULL is returned, no more error logs)
*/
API_IMPORT PTCHAR uvCmn_Logs_GetErrorMesg(UINT8 index);
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
API_IMPORT VOID uvEng_Logs_SaveLogs(ENG_EDIC apps, ENG_LNWE type, PTCHAR mesg,
									PTCHAR file, PTCHAR func, UINT32 line);
API_IMPORT VOID uvEng_Logs_SaveMesgGen2I(PTCHAR mesg, PTCHAR file, PTCHAR func, UINT32 line);
API_IMPORT VOID uvEng_Logs_SaveWarnGen2I(PTCHAR mesg, PTCHAR file, PTCHAR func, UINT32 line);
API_IMPORT VOID uvEng_Logs_SaveErrorGen2I(PTCHAR mesg, PTCHAR file, PTCHAR func, UINT32 line);
/*
 desc : Get the log files from local directory (includ full path)
 parm : files	- [out] return the file array buffer included full path
 retn : None
*/
API_IMPORT VOID uvEng_Logs_GetLogFiles(UINT8 type, CStringArray &files);


#ifdef __cplusplus
}
#endif