using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

/* User-defined Namespace */
using TGen2i.Enum.Comn;
using TGen2i.Enum.MC2;

namespace TGen2i.EngineLib
{
	public class EngMC2
	{
		/* ------------------------------------------------------------------------------------- */
		/*                                 Shared memory in PLC                                  */
		/* ------------------------------------------------------------------------------------- */

		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvEng_ShMem_GetMC2();	/* Marshal.PtrToStructure(ptrMC2, typeof(STG_MDSM)) */

		/* ------------------------------------------------------------------------------------- */
		/*                          DLL Library Interface - for Common                           */
		/* ------------------------------------------------------------------------------------- */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Double uvCmn_MC2_GetDrvAbsPos(ENG_MMDI drv_id);   /* Returns the absolute position of the current motion drive */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Byte uvCmn_MC2_GetDriveStatus(ENG_MMDI drv_id, ENG_SADS flag);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern UInt16 uvCmn_MC2_GetDriveError(ENG_MMDI drv_id);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Int32 uvCmn_MC2_GetDriveResult(ENG_MMDI drv_id, Byte flag);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvCmn_MC2_IsDriveHomed(ENG_MMDI drv_id);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvCmn_MC2_IsDriveBusy(ENG_MMDI drv_id);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvCmn_MC2_IsDriveMoving(ENG_MMDI drv_id);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvCmn_MC2_IsAnyDriveError();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvCmn_MC2_IsDriveError(ENG_MMDI drv_id);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvCmn_MC2_IsDriveEnStop();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Byte uvCmn_MC2_GetRefHeartBeat();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Byte uvCmn_MC2_GetRefVScale();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern UInt64 uvCmn_MC2_GetRefOutputs();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Byte uvCmn_MC2_GetActState(Byte heart_no);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern UInt16 uvCmn_MC2_GetActError();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern UInt64 uvCmn_MC2_GetActInOut(Byte type);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvCmn_MC2_IsDevLocked(ENG_MMDI drv_id);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvCmn_MC2_IsAllDevLoced();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern void uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI drv_id);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI drv_id);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern void uvCmn_MC2_GetDrvDoneToggledAll();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvCmn_MC2_IsDrvDoneToggledAll();

		/* ------------------------------------------------------------------------------------- */
		/*                           DLL Library Interface - Only MC2                            */
		/* ------------------------------------------------------------------------------------- */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvEng_MC2_SendDevAbsMove(ENG_MMDI drv_id, Double move, Double velo);	/* Move the motion drive to absolute position */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvEng_MC2_SendDevRefMove(ENG_MMMM method, ENG_MMDI drv_id, ENG_MDMD direct, Double dist);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvEng_MC2_SendDevStopped(ENG_MMDI drv_id);	/* Forcibly stops the movement of the motion drive */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvEng_MC2_SendDevMoveVectorXY(ENG_MMDI drv_1, ENG_MMDI drv_2, Double pos_1/* mm */, Double pos_2/* mm */, Double speed/* mm/sec */, ref ENG_MMDI axis);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvEng_MC2_SendDevHoming(ENG_MMDI drv_id);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvEng_MC2_SendDevHomingAll();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvEng_MC2_SendDevHomingAllEx();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvEng_MC2_SendDevLocked(ENG_MMDI drv_id, Boolean flag);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvCmn_MC2_SendDevLockedAll(Boolean flag);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvEng_MC2_SendDevStoppedAll();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvEng_MC2_SendDevFaultReset(ENG_MMDI drv_id);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvEng_MC2_SendDevFaultResetAll();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvEng_MC2_SendDevFaultResetCheckAll();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvEng_MC2_SendDevFaultResetAllEx();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern void uvEng_MC2_SetSendPeriodPkt(Boolean enable);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode)]
		public static extern Boolean uvEng_MC2_SetRefHeadEnStop(ENG_MMST en_stop, Byte beat);
	}
}
