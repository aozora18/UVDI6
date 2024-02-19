using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

/* User-defined Namespace */
using TGen2i.Enum.Comn;
using TGen2i.Struct.PLC;

namespace TGen2i.EngineLib
{
	class EngineLitho
	{
		/* ------------------------------------------------------------------------------------- */
		/*                                Shared memory in Litho                                 */
		/* ------------------------------------------------------------------------------------- */

		/* ------------------------------------------------------------------------------------- */
		/*                          DLL Library Interface - for Common                           */
		/* ------------------------------------------------------------------------------------- */

		/* ------------------------------------------------------------------------------------- */
		/*                          DLL Library Interface - Only Litho                           */
		/* ------------------------------------------------------------------------------------- */
		/* Returns the status value within the current Track device */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvEng_Litho_GetStateTrack();		/* Marshal.PtrToStructure(ptrPLC, typeof(STG_ODET)) */
		/* Returns the status value in the current Scanner device */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvEng_Litho_GetStateScanner();		/* Marshal.PtrToStructure(ptrPLC, typeof(STG_IDTE)) */
		/* Initialize PIO History */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern void uvEng_Litho_ResetHistPIO();
		/* Return the PIO state value of a specific location (get the most recent to the previous state value) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Litho_GetHistPIO(Byte count, ref STG_STSE state);
		/* EXP-INL (Scanner Inline) Value Control (ON or OFF) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Litho_SetScanInline(Boolean signal);
		/* EXP-RDY (Scanner Received to Ready) Value Control (ON or OFF) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Litho_SetScanRecvToReady(Boolean signal);
		/* EXP_SND (Scanner Send to Ready) Value Control (ON or OFF) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Litho_SetScanSendToReady(Boolean signal);
		/* EXP-REJ (Scanner Reject) Value Control (ON or OFF) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Litho_SetScanReject(Boolean signal);
		/* Whether the wafer is being transferred from the track to the scanner (Transporting from Track To Scanner) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Litho_IsMovingWaferFromTrackToScanner();
		/* Whether it is possible to transfer the wafer from the current track to the scanner. In other words, whether the Scanner is ready to receive a Wafer */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Litho_IsMoveWaferFromTrackToScanner();
		/* Whether the wafer (in lot) has been moved from the current track to the scanner */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Litho_IsMovedWaferFromTrackToScanner();
		/* Whether it is possible to stop the wafer transfer motion operation on the current track
		   That is, whether a command (control) to stop the wafer transfer operation is possible.
		   This is because it cannot be stopped while the wafer is being transferred in the current track. */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Litho_IsMoveStopWaferOnTrack();
		/* Whether the last wafer movement of the current lot wafer has been canceled from the track to the scanner */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Litho_IsAbortedEndOfLotWaferFromTrackToScanner();
		/* Whether the wafer has been transferred from the scanner to the track */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Litho_IsMovingWaferFromScannerToTrack();
		/* Whether the wafer is being transferred from the scanner to the track */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Litho_IsMovedWaferFromScannerToTrack();
		/* Whether the wafer is refusing to transfer from Scanner to Track */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Litho_IsRejectingWaferFromScannerToTrack();
		/* Whether or not the transfer has been rejected by the wafer from the scanner to the track */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Litho_IsRejectedWaferFromScannerToTrack();
		/* Whether the Wafer is being removed (deleted) by the Scanner */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Litho_IsDeletingWaferInScanner();
		/* Whether the Wafer is being removed (deleted) by the Scanner */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Litho_IsDeletedWaferInScanner();
	}
}
