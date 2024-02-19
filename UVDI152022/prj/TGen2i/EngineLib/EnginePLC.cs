using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

/* User-defined Namespace */
using TGen2i.Enum.Comn;
using TGen2i.Enum.PLC;

namespace TGen2i.EngineLib
{
	class EngPLC
	{
		/* ------------------------------------------------------------------------------------- */
		/*                                 Shared memory in PLC                                  */
		/* ------------------------------------------------------------------------------------- */

		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvEng_ShMem_GetPLCStruct();	/* Marshal.PtrToStructure(ptrPLC, typeof(STG_PMDV)) */

		/* ------------------------------------------------------------------------------------- */
		/*                                  PLC < for Engine >                                   */
		/* ------------------------------------------------------------------------------------- */

		/* Check whether the current PLC is connected */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MCQ_IsConnected();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MCQ_GetShMemStruct();	/* STG_PMDV */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MCQ_IsAlarm();										/* Alarm 존재 여부 */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MCQ_IsWarn();										/* Warn 존재 여부 */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MCQ_GetWordMemory(UInt16 index);						/* 임의 주소의 16 Bit 값 - Read */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 uvCmn_MCQ_GetDWordMemory(UInt16 index);						/* 임의 주소의 32 Bit 값 - Read */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MCQ_GetBitsValueEx(ENG_PIOA addr);						/* 임의 주소의 1 Bit 값 - Read */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MCQ_GetWordValueEx(ENG_PIOA addr);					/* 임의 주소의 Word 값 - Read */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 uvCmn_MCQ_GetDWordValueEx(ENG_PIOA addr);					/* 임의 주소의 DWord - Read 값 */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MCQ_WriteBitsValueEx(ENG_PIOA addr, Byte value);		/* 임의 주소의 Bit 값 - Write */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MCQ_WriteBitsInvertEx(ENG_PIOA addr);				/* 임의 주소의 Bit Invert 값 - Write */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MCQ_WriteWordValueEx(ENG_PIOA addr, UInt16 value);	/* 임의 주소의 Word 값 - Write */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MCQ_WriteDWordValueEx(ENG_PIOA addr, UInt32 value);	/* 임의 주소의 DWord 값 - Write */

		/* --------------------------------------------------------------------------------------------- */
		/*                      외부 Utility - < PLC - Melsec Q > < for C# or GUI >                      */
		/* --------------------------------------------------------------------------------------------- */

		/* MC Protocol - Tower (Lamp On or Off) or (All On or OFF) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MCQ_WriteTowerLampOnOff(ENG_TLCI color, Byte power);
		/* MC Protocol - Photohead Z Axis <Cooling> All On or Off */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MCQ_WritePhZAxisCoolingAll(Byte power);
		/* MC Protocol - Photohead Z Axis <Cylinder> All On or Off */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MCQ_WritePhZAxisCyclinderAll(Byte power);
		/* MC Protocol - Vacuum On or Off */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MCQ_WriteVacuumControl(Byte chuck, Byte robot, Byte aligner);
		/* MC Protocol - Photohead Power On or Off (특수하게 처리해야 됨) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MCQ_WritePhPower(Byte ph1_power, Byte ph2_power);
		/* MC Protocol - Light On or Off for Illumination */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MCQ_WriteLightIllumInvert();
		/* MC Protocol - Light On or Off for Halogen */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MCQ_WriteLightControl(Byte h_ring, Byte h_coax);
		/* MC Protocol - XY Stage Move Prohibit Release or OFF */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MCQ_WriteXYStageMoveProhibitRelease(Byte release);
		/* MC Protocol - Linked.Litho : EXP-INK ~ EXP-RDY (Exposure Inline ~ Exposure Receive To Ready) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MCQ_WriteLithoExposureToTrack(ENG_LLET type, Byte mode);
		/* Halogen Light Power (소숫점 2자리까지 유효) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MCQ_WriteLightPowerVolt(ENG_HLPT type, Double volt);
		/* Lift Pin or BSA Axis 제어 */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MCQ_WriteLiftPinAxisControl(ENG_LPAT type, ENG_LPAC mode, Byte value);
		/* Lift Pin or BSA Axis 동작 처리 */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MCQ_WriteLiftPinAxisAction(ENG_LPAT type, ENG_LPAA mode, Byte value);
		/* Lift Pin or BSA Axis : Jog Speed 설정 */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MCQ_WriteLiftPinAxisJogSpeed(ENG_LPAT type, Double value);
		/* Lift Pin or BSA Axis : Position Speed 설정 */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MCQ_WriteLiftPinAxisPosSpeed(ENG_LPAT type, Double value);
		/* Lift Pin or BSA Axis : Position 설정 */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MCQ_WriteLiftPinAxisPos(ENG_LPAT type, Double value);
	}
}
