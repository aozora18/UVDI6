using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

/* User-defined Namespace */
using TGen2i.Enum.Comn;
using TGen2i.Enum.EFU;

namespace TGen2i.EngineLib
{
	public class EngEFU
	{
		/* ------------------------------------------------------------------------------------- */
		/*                                 Shared memory in EFU                                  */
		/* ------------------------------------------------------------------------------------- */
		/* Return full EFU information */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvEng_ShMem_GetEFU();/* STG_EPAS */

		/* ------------------------------------------------------------------------------------- */
		/*                          DLL Library Interface - for Common                           */
		/* ------------------------------------------------------------------------------------- */

		/* ------------------------------------------------------------------------------------- */
		/*                           DLL Library Interface - Only EFU                            */
		/* ------------------------------------------------------------------------------------- */
		/* Check whether it is currently connected with the EFU */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvEng_EFU_IsConnected();
		/* Set the waiting time value for receiving to the remote system. */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern void uvEng_EFU_SetAckTimeout(UInt64 wait);
		/* Reset the existing connected communication */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern void uvEng_EFU_ResetCommData();
		/* Set the specific (=Speed. RPM) status value of EFU */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvEng_EFU_ReqSetSpeed(Byte bl_id, Byte speed);
		/* Set the specific (=Speed. RPM) status value of EFU */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvEng_EFU_IsSetSpeed();
		/* Request the current status value of EFU. */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvEng_EFU_ReqGetState();
		/* Whether the command can be sent */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvEng_EFU_IsSendIdle();
		/* Returns a value by ID for BL500. */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Byte uvEng_EFU_GetDataValue(Byte bl_id, ENG_SEBT type);
		/* Setting whether to periodically request status information of EFU */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern void uvEng_EFU_SetPeriodState(Boolean enable);
	}
}
