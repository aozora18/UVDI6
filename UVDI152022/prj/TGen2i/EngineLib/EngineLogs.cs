using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

/* User-defined Namespace */
using TGen2i.Enum.Comn;
using TGen2i.Struct.Comn;

namespace TGen2i.EngineLib
{
	class EngLogs
	{
		/* ------------------------------------------------------------------------------------- */
		/*                                Shared memory in Luria                                 */
		/* ------------------------------------------------------------------------------------- */

		/* ------------------------------------------------------------------------------------- */
		/*                                  Logs < for Engine >                                  */
		/* ------------------------------------------------------------------------------------- */

		/* Return the last error message that occurred */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_Logs_GetErrorMesgLast();		/* PTCHAR -> Marshal.PtrToStringUni(PTCHAR) */
		/* Whether an error or warning log exists */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Logs_IsErrorMesg();
		/* error message reset */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern void uvCmn_Logs_ResetErrorMesg();
		/* Returns an error log message in an arbitrary location */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_Logs_GetErrorMesg(Byte index);   /* PTCHAR -> Marshal.PtrToStringUni(PTCHAR) */
		/* Saving Logs - Include debugging information */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern void uvEng_Logs_SaveMesgGen2I(string mesg, string file, string func, UInt32 line);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern void uvEng_Logs_SaveWarnGen2I(string mesg, string file, string func, UInt32 line);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern void uvEng_Logs_SaveErrorGen2I(string mesg, string file, string func, UInt32 line);
	}
}
