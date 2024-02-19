using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

/* User-defined Namespace */
using TGen2i.Enum.Comn;
using TGen2i.Enum.Milara;
using TGen2i.Enum.MDR;

namespace TGen2i.EngineLib
{
	public class EngMDR
	{
		/* ------------------------------------------------------------------------------------- */
		/*                                Shared memory in Robot                                 */
		/* ------------------------------------------------------------------------------------- */

		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvEng_ShMem_GetLSDR();	/* Marshal.PtrToStructure(ptrMDR, typeof(STG_MRDV)) */

		/* ------------------------------------------------------------------------------------- */
		/*                        Diamond Robot for Logosol < for Engine >                       */
		/* ------------------------------------------------------------------------------------- */

		/* Check whether the robot is currently connected */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_IsConnected();
		/* Initialize the previously saved send/recv buffer */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern void uvEng_MDR_ResetCommData();
		/* Returns the response status value for the most recently sent command */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern ENG_MSAS uvEng_MDR_GetLastSendAckStatus();

		/* ------------------------------------------------------------------------------------- */
		/*                    Command Reference  (1. Informational commands)                     */
		/* ------------------------------------------------------------------------------------- */

		/* Set/Report System Time and Date (cmd : _dt) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetDateTime();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetDateTime();
		/* Set/Report System Configuration Information */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetSystemConfigInfo();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetSystemConfigInfo();
		/* Reset the Controller */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_ResetMotionController();
		/* Report Robot Serial Number */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetRobotSerialNumber();
		/* Report the firmware version number */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetRobotFirmwareVer(Byte option);

		/* ------------------------------------------------------------------------------------- */
		/*       Command Reference  (2.Housekeeping (parameters save / restore) commands)        */
		/* ------------------------------------------------------------------------------------- */

		/* This command restores user-defined custom parameters (cmd : _cld) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_RestoreCustomParameter();
		/* Restore Custom parameters (cmd : _csv) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SaveCustomParameter();
		/* Restore Motion and Calibration Parameters (cmd : rmp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_RestoreMotionCalibrationParam();
		/* Read Parameters Sets from Disk (cmd : rps) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_ReadParametersSetsFromDisk();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_RestoreStationParameters();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SaveParametersSetsToDisk();
		/* Save Station Parameters (cmd : ssp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SaveStationParameters();

		/* ------------------------------------------------------------------------------------- */
		/*                 Command Reference  (3.Status and diagnostic commands)                 */
		/* ------------------------------------------------------------------------------------- */

		/* Report current system state (cmd : _cs) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetCurrentRobotState();
		/* Report Current Position for an Axis (cmd : cpo) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetAxisCurrentPositionAll();
		/* Set/Report Inform-After-Motion Mode (cmd : inf) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetInfoMode();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetInfoMode();
		/* Report diagnostic information (cmd : diag) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetDiagInfoCurrent();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetDiagInfoLast();
		/* Report Extended Error Number (cmd : eerr) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetExtendedErrorNumber();
		/* Extended Error Status (cmd : est) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetExtendedErrorState();
		/* Report Extended System Status (cmd : esta) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetExtendedErrorStateCode();
		/* Report Previous Error Reason (robot) (cmd : per v) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetPreviousErrorReason();
		/* Read Home Switches State (cmd : rhs) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetHomeSwitchAll();
		/* Save Motion and Calibration Parameters (cmd : smp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SaveMotionCalibrationParam(Byte option);
		/* Command Execution Status (cmd : sta) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetSystemStatus();

		/* ------------------------------------------------------------------------------------- */
		/*            Command Reference  (4.Support for systems with absolute encoders)          */
		/* ------------------------------------------------------------------------------------- */

		/* AE (Absolute Encoder) reset warning / position (cmd : _aer) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetAxisEncoderReset(ENG_RANC axis, ENG_RAEC mode, string password);
		/* Absolute Encoder Status (for Dual-Arm) (cmd : _aest) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetEconderStatus();
		/* Set/Report Encoder Zero Offset for single axis (cmd : _eo) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetEncoderZeroOffsetValue(ENG_RANC axis, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetEncoderZeroOffset();
		/* Set / report Encoder Zero Offset for all axis (cmd : _eor) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetEncoderZeroOffsetAll();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetEncoderZeroOffsetAll();
		/* Report low-level encoder status for an Axis (cmd : _ralmc) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetLowLevelEncoderStatus(ENG_RANC axis);

		/* ------------------------------------------------------------------------------------- */
		/*              Command Reference  (5.Basic (non-axes, non-station) action commands)             */
		/* ------------------------------------------------------------------------------------- */

		/* Set/Report Turn Backup DC Power On or Off (cmd : _bb) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetBackupPowerCtrl(Byte option, Int32 value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetBackupPowerCtrl();
		/* Enable/Disable Controlled Stop Feature (cmd : cstp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetControlledStopFeature(Byte state, Byte option);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetControlledStopFeature();
		/* Read State of External Digital Input (cmd : din) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStateExtDI();
		/* Set/Report State of External Digital Output (cmd : dout) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStateExtDONo(Byte output, Byte value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStateExtDO();
		/* Set/Report the state of an Input (cmd : inp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStateInput(Byte input);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStateInput();
		/* Enable/Disable Loaded Speed Parameters (cmd : lenb) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetLoadedSpeedParamEnable(Byte option);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetLoadedSpeedParamEnable();
		/* Set/Report Output (cmd : out) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStateOutput(Byte output, Byte value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStateOutput();
		/* Turn Axis Servo or REE Power OFF (cmd : sof) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetAxisServoTurnOff(ENG_RANC axis);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetAxisServoTurnOn(ENG_RANC axis);
		/* Turn Vacuum OFF (cmd : vof) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetVacuumPaddleTurnOff(Byte effect_num);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetVacuumPaddleTurnOn(Byte effect_num);
		/* Check for wafer presence on a paddle (cmd : vst) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetCheckWaferInVacuumPaddle(Byte number);

		/* ------------------------------------------------------------------------------------- */
		/*                  Command Reference  (6.Basic axis motions parameters)                 */
		/* ------------------------------------------------------------------------------------- */

		/* Set/Report Axis Customized Home Position (cmd : _ch) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetCustomizedHomePosition(ENG_RANC axis, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetCustomizedHomePosition();
		/* Set/Report Axis Current Limit (cmd : _cl) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetAxisCurrentLimitLevel(ENG_RANC axis, Byte value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetAxisCurrentLimitLevel();
		/* Set/Report Axis Position Error Limit (cmd : _el) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetAxisPositionErrorLimit(ENG_RANC axis, Int32 value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetAxisPositionErrorLimit();
		/* Set/Report Axis Forward or Reverse Safe Envelop (cmd : _fse or _rse) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetAxisSafeEnvelop(ENG_RANC axis, ENG_MAMD direct, Int32 value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetAxisSafeEnvelop(ENG_MAMD direct);
		/* Set/Report Forward or Reverse Direction Software Limit (cmd : _fl or _rl) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetAxisSoftwareLimitValue(ENG_RANC axis, ENG_MAMD direct, Int32 value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetAxisSoftwareLimitCurrent(ENG_RANC axis, ENG_MAMD direct);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetAxisSoftwareLimit(ENG_MAMD direct);
		/* Set/Report PID Loop Differential Coefficient for Axis Hold (cmd : _hd) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetDiffCoeffAxisHold(ENG_RANC axis, UInt32 value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetDiffCoeffAxisHold();
		/* Set/Report Position Error Limit for Axis Hold (cmd : _he) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetPosErrorLimitAxisHold(ENG_RANC axis, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetPosErrorLimitAxisHold();
		/* Set/Report PID Loop Proportional Coefficient for Axis Hold (cmd : _hp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetPropCoeffAxisHold(ENG_RANC axis, UInt32 value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetPropCoeffAxisHold();
		/* Set/Report Axis Home Offset (cmd : _ho) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetAxisHomeOffsetValue(ENG_RANC axis, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetAxisHomeOffset();
		/* Set/Report PID Loop Integral Limit (cmd : _il) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetIntegralLimit(ENG_RANC axis, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetIntegralLimit();
		/* Set/Report PID Loop Differential Coefficient (cmd : _kd) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetDiffCoeffAxis(ENG_RANC axis, UInt32 value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetDiffCoeffAxis();
		/* Set/Report PID Loop Integral Coefficient (cmd : _ki) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetInteCoeffAxis(ENG_RANC axis, UInt32 value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetInteCoeffAxis();
		/* Set/Report PID Loop Proportional Coefficient (cmd : _kp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetPropCoeffAxis(ENG_RANC axis, UInt32 value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetPropCoeffAxis();
		/* Set/Report Axis Acceleration or Deceleration (cmd : acl or dcl) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetAxisAcceleration(ENG_RANC axis, ENG_MAST type, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetAxisAcceleration(ENG_MAST type);
		/* Set/Report Axis Acceleration or Deceleration Jerk (cmd : ajk or djk) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetAxisAccelerationJerk(ENG_RANC axis, ENG_MAST type, UInt16 value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetAxisAccelerationJerk(ENG_MAST type);
		/* Set/Report Loaded Acceleration or deceleration (cmd : lacl or ldcl) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetAxisLoadedAcceleration(ENG_RANC axis, ENG_MAST type, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetAxisLoadedAcceleration(ENG_MAST type);
		/* Set/Report Axis Loaded Acceleration or deceleration Jerk (cmd : lajk or ldjk) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetAxisLoadedAccelerationJerk(ENG_RANC axis, ENG_MAST type, UInt16 value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetAxisLoadedAccelerationJerk(ENG_MAST type);
		/* Set/Report Loaded Speed (cmd : lspd) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetAxisLoadedSpeed(ENG_RANC axis, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetAxisLoadedSpeed();
		/* Set/Report Working Speed (cmd : spd) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetAxisWorkingSpeed(ENG_RANC axis, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetAxisWorkingSpeed();

		/* ------------------------------------------------------------------------------------- */
		/*           Command Reference  (7.Axis parameter sets, and custom parameters commands)          */
		/* ------------------------------------------------------------------------------------- */

		/* Set/Report Custom Axis parameter (cmd : _cax) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetCustomAxisParam(ENG_RANC axis, Byte param, Int32 value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetCustomAxisParam(ENG_RANC axis, Byte param);
		/* Set/Report Custom Generic parameter (cmd : _cpa) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetCustomGenericParam(Byte param, Int32 value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetCustomGenericParam(Byte param);
		/* Set/Report Custom Station Parameter (cmd : _cst) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetCustomStationParam(Char station, Byte param, Int32 value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetCustomStationParam(Char station, Byte param);
		/* Report All Parameter Sets for a Single Axis (cmd : das) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetAllParamSetsSingleAxis(ENG_RANC axis);
		/* Report a Parameter Set (cmd : dps) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetParameterSetInfo(Byte value);
		/* Set Homing Speed and Acceleration Parameters (cmd : hsa) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetHomingAllAxisMotionParam();
		/* Set Loaded Speed and Acceleration Parameters (cmd : lsa) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetLoadedAllAxisMotionParam();
		/* Set Axes Working Speed and Acceleration Parameters (cmd : wsa) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetWorkingAllAxisMotionParam();

		/* ------------------------------------------------------------------------------------- */
		/*                       Command Reference  (8.Basic axis motions commands)                      */
		/* ------------------------------------------------------------------------------------- */

		/* Absolute Encoder - Report distance from home switch (cmd _afe or afe) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetDistanceFromHomeSwitch(ENG_RANC axis);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetDistanceFromHomeSwitchEx(ENG_RANC axis);
		/* Report Home Switch to Index distance (cmd : _fe) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetFindEdgeAxis(ENG_RANC axis);
		/* Find Home Switch (cmd : _fh) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetFindHomeSwitch(ENG_RANC axis);
		/* Start Homing Procedure for the Robot (cmd : hom) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetHomeMethod(SByte option);
		/* Home an Axis (Homing Start) (cmd : _hm) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_StartHomeAxis(ENG_RANC axis);
		/* Move to Home Position (cmd : mth) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_MoveHomePosition(ENG_RANC axis);
		/* Move Four Axes to Position (cmd : mtp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_MoveAxisAll(Double axis_t, Double axis_r1, Double axis_r2, Double axis_z);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_MoveAxisExtAll(Double axis_t, Double axis_r1, Double axis_r2, Double axis_z,
												 Double retract);
		/* Move arm to Station Retracted Position (cmd : mtr) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_MoveAxisRetractedPosition(Char station, Byte slot, Byte option);
		/* Move Axis to Absolute Position (cmd : mva) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_MoveAbsolutePosition(ENG_RANC axis, Double position);
		/* Move Axis Relative (cmd : mvr) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_MoveRelativePosition(ENG_RANC axis, Double distance);
		/* Move Over Straight Line (cmd : mvt1) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_MoveOverStraightLine(Double pos_t, Double pos_r);
		/* Move Over Two Straight Lines (cmd : mvt2) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_MoveOverTwoStraightLine(Double pos_t1, Double pos_r1, Double pos_t2, Double pos_r2);
		/* SSet/Report EE Length and Radius for Station-Less Moves (cmd : mvtp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetEELenRadiusForStationLessMove(Double ee_len, Double radius);
		/* Stop Motions and Tasks (cmd : stp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_StopMotionTask(ENG_RANC axis);
		/* Wait for Motion Completed (cmd : wmc) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetWaitForMotionCompleted();

		/* ------------------------------------------------------------------------------------- */
		/*                   Command Reference  (9.Station parameters commands)                  */
		/* ------------------------------------------------------------------------------------- */

		/* Set/Report Current Station Set Number (cmd : _css) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetCurrentStationSet(Byte value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetCurrentStationSet();
		/* Set/Report R Axis Pushing Acceleration (cmd : _pa) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetRAxisPusingAcceleration(Char station, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetRAxisPusingAcceleration(Char station);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetRAxisPusingAccelerationDefault();
		/* Set/Report R Axis Pushing Speed (cmd : _ps) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetRAxisPusingSpeed(Char station, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetRAxisPusingSpeed(Char station);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetRAxisPusingSpeedDefault();
		/* Set/Report Station Coordinate Compensation with Slot (cmd : _zc) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationCoordCompensationSlot(Char station, ENG_RANC axis, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationCoordCompensationSlot(Char station);
		/* Report Inline Station' Approach position (cmd : app) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetInlineStationApproachPosition(Char station);
		/* Set/Report Station Center Offset (cmd : cof) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationCenterOffset(Char station, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationCenterOffset(Char station);
		/* Set/Report Station Center Offset for special operations (cmd : cofp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationCenterOffsetEdgeGrip(Char station, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationCenterOffsetEdgeGrip(Char station);
		/* Set/Report Current Slot for a Station (cmd : csl) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationSlotNo(Char station, Byte value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationSlotNo(Char station);
		/* Clear Station Parameters (cmd : csp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationParamClear(Char station);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationParamClearAll();
		/* Set/Report Current Station, by name (cmd : cst) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetCurrentStationName(Char station);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetCurrentStationName();
		/* Set/Report Current Station, by number (cmd : cstn) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetCurrentStationNo(Byte value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetCurrentStationNo();
		/* Duplicate Station Parameters, by name (cmd : dsp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetDuplicateStationParamByName(Char source, Char target);
		/* Duplicate Station Parameters, by number (cmd : dspn) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetDuplicateStationParamByNumber(Byte source, Byte target);
		/* Set/Report End Effector Number in Use for a Station (cmd : een) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetEndEffectorNumForStation(Char station, Byte number);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetEndEffectorNumForStation(Char station);
		/* When the End Effector in the Station operation option GET or PUT the wafer to a specific position, set/return to move the Z axis up or down (cmd : eeo) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetEndEffectorUpDownForStation(Char station, Byte up_down);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetEndEffectorUpDownForStation(Char station);
		/* Enumerate Defined Stations (cmd : ens) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetEnumerateDefinedStations();
		/* Enumerate Defined Stations (cmd : ens 1 or 2 or 3 or 4) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetEnumerateDefinedStationsNum(Byte set);
		/* Enumerate Defined Stations (cmd : ens 0) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetEnumerateDefinedStationsAll();
		/* Enumerate Defined Stations (cmd : ens N) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetEnumerateDefinedStationsAllNum();
		/* Set/Report Inline Station Last Segment Speed Factor (cmd : ils) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetInlineStationLastSeqmentSpeedFactor(Char station, Byte value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetInlineStationLastSeqmentSpeedFactor(Char station);
		/* Set/Report Inline Station Retracted R Position (cmd : irr) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetInlineStationRetractedRPosition(Char station, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetInlineStationRetractedRPosition(Char station);
		/* Set/Report Inline Station Retracted T Position (cmd : irt) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetInlineStationRetractedTPosition(Char station, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetInlineStationRetractedTPosition(Char station);
		/* Set/Report Inline Station Exit Angle (cmd : isa) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetInlineStationExitAngle(Char station, Double angle);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetInlineStationExitAngle(Char station);
		/* Set/Report Inline Station Exit Distance (cmd : isd) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetInlineStationExitDistance(Char station, Double distance);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetInlineStationExitDistance(Char station);
		/* Set/Report Inline Station End-Effector Length (cmd : ise) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetInlineStationEndEffectLength(Char station, Double length);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetInlineStationEndEffectLength(Char station);
		/* Set/Report Inline Station Corner Radius (cmd : isr) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetInlineStationCornerRadius(Char station, Double radius);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetInlineStationCornerRadius(Char station);
		/* Set/Report Inline Station Type (cmd : ist) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetInlineStationType(Char station, Byte type);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetInlineStationType(Char station);
		/* Set/Report Station Put and Get Delay (cmd : pgd) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetPutGetDelayTime(Char station, UInt32 value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetPutGetDelayTime(Char station);
		/* Set/Report Put/Get Failure Option (cmd : pgf) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetPutGetFailureOption(Char station, Byte opt_get, Byte opt_put);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetPutGetFailureOption(Char station);
		/* Set/Report Get Option and Sync Move Option (cmd : pgo) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetPutGetOption(Byte option);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetPutGetOption();
		/* Set/Report Station Pitch (cmd : pit) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationPitch(Char station, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationPitch(Char station);
		/* Set/Report Start Pushing Distance (cmd : psd) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationPushingDist(Char station, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationPushingDist(Char station);
		/* Set/Report Pusher Type (cmd : psh) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationPushingType(Char station, Byte value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationPushingType(Char station);
		/* Set/Report Start Pushing Offset (cmd : pss) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStartPushingOffsetR(Char station, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStartPushingOffsetR(Char station);
		/* Set/Report Z Push Offset (cmd : psz) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStartPushingOffsetZ(Char station, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStartPushingOffsetZ(Char station);
		/* Set/Report Station Pick-Up Acceleration (cmd : pua) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationPickUpAccelerationZ(Char station, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationPickUpAccelerationZ(Char station);
		/* Set/Report Station Pick-Up Speed (cmd : pus) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationPickUpSpeedZ(Char station, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationPickUpSpeedZ(Char station);
		/* Set/Report Station Retracted Position (cmd : rpo) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetRetractedStationPositionR(Char station, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetRetractedStationPositionR(Char station);
		/* Set/Report Station Position (cmd : spo) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationPositionName(Char station, ENG_RANC axis, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationPositionName(Char station);
		/* Report Station Number Position (cmd : spon) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationPositionNumber(Byte station);
		/* Set/Report Station Stroke (cmd : str) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationStrokeZ(Char station, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationStrokeZ(Char station);
		/* Set/Report Axes Motion Overlap Factor (cmd : tbl) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationAxisOverlapFactor(Byte value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationAxisOverlapFactor();
		/* Set Teach Station Position Mode (cmd : tch) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_StartTeachingStation(Char station, Byte slots=0x01);
		/* End of Teaching of Scanning or Station Parameters (cmd : eot) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetTeachingEnd();
		/* Enable/Disable Trajectory Mode (cmd : tmd) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationTrajectoryMode(Byte option);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationTrajectoryMode();
		/* Enable/Disable wafer check before pick/place operations (cmd : vchk) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationVacuumCheck(Byte option);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationVacuumCheck();
		/* Set/Report Vacuum Search Time Out (cmd : vto) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationVacuumSearchTimeout(Char station, UInt32 value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationVacuumSearchTimeout(Char station);

		/* ------------------------------------------------------------------------------------- */
		/*                        Command Reference  (10.Station motion commands)                        */
		/* ------------------------------------------------------------------------------------- */

		/* Get a Wafer From a Station (cmd : get) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetGetWaferFromStationName(Char station);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetGetWaferFromStationNameSlot(Char station, Byte slot);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetGetWaferFromStationNameSlotOpt(Char station, Byte slot, Byte end_opt);
		/* Execute Object Pick-Up Operation with Designated or Both Arms (cmd : dget) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetGetWaferFromStationNameSlotDual(Char station, Byte arm, Byte slot);
		/* Get a Wafer from a station number (cmd : getn) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetGetWaferFromStationNum(Byte station);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetGetWaferFromStationNumSlot(Byte station, Byte slot);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetGetWaferFromStationNumSlotOpt(Byte station, Byte slot, Byte end_opt);
		/* Move Out of Station (cmd : mos) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetMoveOutStationArmOffsetZ(Double z_offset);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetMoveOutStation();
		/* Move a Wafer From Station to Station (cmd : mss) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetMoveStationToStationByNameROffset(Char station_s, Byte slot_s,
																					Char station_t, Byte slot_t,
																					Double r_offset);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetMoveStationToStationByName(Char station_s, Byte slot_s,
																Char station_t, Byte slot_t);
		/* Move a Wafer from Station Number to Station Number (cmd : mssn) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetMoveStationToStationByNumberROffset(Byte station_s, Byte slot_s,
																					  Byte station_t, Byte slot_t,
																					  Double r_offset);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetMoveStationToStationByNumber(Byte station_s, Byte slot_s,
																			   Byte station_t, Byte slot_t);
		/* Move arm to Station Approach Position (cmd : mta) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetMoveArmStationApproachPosition(Char station, Byte slot, Byte option);
		/* Move Arm to Station Position (cmd : mts) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetMoveArmStationPosition(Char station, Byte slot, Byte option);
		/* Set/Report Number of Slots for a Station (cmd : nsl) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationSlotNumber(Char station, Byte slot);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationSlotNumber(Char station);
		/* Report Number of Stations (cmd : nst) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationNumber();
		/* Set/Report Station Offset (cmd : ofs) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationOffsetZ(Char station, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationOffsetZ(Char station);
		/* Put a Wafer into a Station (cmd : put) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_PutWaferIntoStationName(Char station);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_PutWaferIntoStationNameFull(Char station, Byte slot,
															  Double offset, Byte end_opt);
		/* Put a Wafer into a Station Number (cmd : putn) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_PutWaferIntoStationNo(Byte station);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_PutWaferIntoStationNoFull(Byte station, Byte slot,
																		 Double offset, Byte end_opt);
		/* Execute Object Place Operation with Designated or Both Arms (cmd : dput) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_PutWaferIntoStationNameDual(Char station, Byte arm_no, Byte slot);
		/* Check whether a station is being serviced at the moment (cmd : sbusy) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationServiceBusy();
		/* Move a Wafer from Station to Station, with end option (cmd : sgp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetMoveStationToStationEndOpt(Char station_s, Byte slot_s,
																			 Char station_t, Byte slot_t,
																			 Double r_offset, Byte end_opt);
		/* Set/Report Station Locked State (cmd : slck) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationLockedState(Char station, Byte option);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationLockedState(Char station);

		/* ------------------------------------------------------------------------------------- */
		/*            Command Reference  (11.Station scanning parameters and motion commands)            */
		/* ------------------------------------------------------------------------------------- */

		/* Set/Report Maximum Wafer Thickness (cmd : _mt) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetMaximumWaferThick(Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetMaximumWaferThick();
		/* Set/Report Wafer Scanning Z Axis Acceleration (cmd : _sa) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetWaferScanZAxisAcceleration(Char station, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetWaferScanZAxisAcceleration(Char station);
		/* Set Current Position as Scanning Center Coordinate (cmd : _sc) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetCurrentPositionScanningCenterCoordinate(Char station);
		/* Set Current Position as Scanning Left or Right Coordinate (cmd : _sl or _sr) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetCurrentPositionScanningCoordinate(ENG_MALR direct, Char station);
		/* Set/Report Wafer Scanning Z Axis Speed (cmd : _ss) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetWaferScanZAxisSpeed(Char station, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetWaferScanZAxisSpeed(Char station);
		/* Set/Report Scanner Type (cmd : _st) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetScannerSensorType(Byte value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetScannerSensorType();
		/* Report Scanned Positions (cmd : dscn) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetScannedPositions(Byte direction);
		/* Report Results from Last Wafer Scanning (cmd : map) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetResultLastWaferScanning(Byte direction);
		/* Set/Report Beam Thresholds (cmd : _bt) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetBeamThreadhold(Double value_l, Double value_h);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetBeamThreadhold();
		/* Set /Report Per-Station Beam Thresholds (cmd : sbt) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetPerStationBeamThreadhold(Char station, Double value_l, Double value_h);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetPerStationBeamThreadhold(Char station);
		/* Start wafer Scanning for a Station (cmd : scn) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_StartStationWaferScanning(Char station, Byte direct, Byte angle);
		/* Set/Report Station’ Scanning Center Coordinate (cmd : scp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationScanningCenterCoord(Char station, ENG_RANC axis, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationScanningCenterCoord(Char station);
		/* Set/Report Scan Debug Mode (cmd : sdbg) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetScanDebugMode(Byte option);
		/* Set/Report Station’ Scanning Left Coordinate (cmd : slp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationScanningLeftCoord(Char station, ENG_RANC axis, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationScanningLeftCoord(Char station);
		/* Set/Report Station Scanning Right Coordinate (cmd : srp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationScanningRightCoord(Char station, ENG_RANC axis, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationScanningRightCoord(Char station);
		/* Set/Report Per-Station Maximum Wafer Thickness (cmd : smt) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationWaferMaxThickness(Char station, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationWaferMaxThickness(Char station);
		/* Place a wafer into a station, pick a wafer from a station, with end option (cmd : spg) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetPlaceStationToStationEndOpt(Char station_s, Byte slot_s,
																 Char station_t, Byte slot_t,
																 Double r_offset, Byte end_opt);
		/* Set/Report Scanning Highest or Lowest Z Coordinate (cmd : szh or szl) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStationHighLowestCoordZ(Byte direct, Char station, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetStationHighLowestCoordZ(Byte direct, Char station);
		/* Start Teach Scanning Parameters (cmd : tcs) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetStartTeachScanningParam();

		/* ------------------------------------------------------------------------------------- */
		/*  Command Reference  (12.Align-on-Flight (on-The-Flight, OtF) parameters and commands) */
		/*     (OtF 기능을 사용하면 선택 및/또는 위치 이동 중에 웨이퍼 중심 변위 측정 가능)      */ 
		/* ------------------------------------------------------------------------------------- */

		/* Report Current On-the-Fly Set Number (cmd : acs) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetOtFSetNumber();
		/* Execute On-the-Fly Centering Calibration Run (cmd : aofc) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetOtFCentCalibRun(Double dist);
		/* Set/Report Check for notch on second scan option (cmd : aofcn2) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetOtFNotchSecondScan(Byte option);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetOtFNotchSecondScan();
		/* Execute On-the-Fly Calibration Run against a Station (cmd : aofcs) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetOtFCaliRunStationDirection(Char station, Byte direct);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetOtFCaliRunStationDistance(Char station, Byte direct, Double dist);
		/* Set/Report On-the-Fly Debug Level (cmd : aofd) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetOtFDebugLevel(SByte value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetOtFDebugLevel();
		/* Report Samples from Last OtF Operation (cmd : aofdump) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetOtFDebugDump();
		/* Set/Report Ignore Middle Samples option (cmd : aofim) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetOtFIgnoreMidSamples(Byte option);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetOtFIgnoreMidSamples();

		/* ------------------------------------------------------------------------------------- */
		/*             Command Reference  (13.New commands for DUAL Arm Axis Only)               */
		/* ------------------------------------------------------------------------------------- */

		/* Move Both Arms to Same Absolute Position (cmd : dmva) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetDualMoveArmAbsolutePositionR(Double position);
		/* Move Both Arms to Same Absolute Position (cmd : dmva) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetDualMoveArmRelativeDistanceR(Double distance);
		/* Set/Report Arm to Be Used for Wafer Mapping (cmd : see) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_SetDualSelectMapperArm(Char station, Byte arm_no);

		/* ------------------------------------------------------------------------------------- */
		/*                                        Common                                         */
		/* ------------------------------------------------------------------------------------- */

		/* Current Station service status (Busy, Idle, Not defined. Only Station !!!) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MDR_IsStationServiceBusy();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MDR_IsStationServiceIdle();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MDR_IsStationServiceUndefined();
		/* Whether the state value of system is valid (!!! Important !!!) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MDR_IsValidSystemStatus();
		/*  Whether there are packets received for the most recently sent command */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MDR_IsLastSendAckRecv();
		/* Whether the receive response was successful for the most recently sent command */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MDR_IsLastSendAckSucc();
		/* Returns an error message for system status values */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MDR_GetSystemStatusMesg(String mesg, UInt32 size);
		/* Return an error message about the Home Switch status value */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MDR_GetHomeSwitchStateMesg(String mesg, UInt32 size);
		/* Returns a string message for the code value of "extended error code (cmd : eerr)" */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MDR_GetExtErrorMesg(String mesg, UInt32 size);
		/* Returns an error message about the status of home switches (즉, Error codes returned by RHS) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MDR_GetHomeSwitchesState(String mesg, UInt32 size);
		/* Whether the motion is in stopped state */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MDR_IsWaitForMotionCompleted();
		/* Whether the current motion (Robot Arm) is moving, i.e. Busy (asynchronous control) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_IsMotorArmBusy();
		/* Whether the current motion (Robot Arm) is idle (asynchronous control) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_IsMotorArmIdle();
		/* Whether the current Vacuum Switch & Sensor are both On (asynchronous control) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_IsVacuumSwitchSensorAllOn();
		/* Whether there is the most recently received data */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MDR_IsRecvCmdData(ENG_RSCC cmd=ENG_RSCC.rsc_none, ENG_RANC axis=ENG_RANC.rac_t);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MDR_IsRecvCmdLast(ENG_RANC axis=ENG_RANC.rac_t);
		/* Whether an error has occurred inside the equipment (system) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MDR_IsSystemError();

		/* ------------------------------------------------------------------------------------- */
		/*                        Returns a value stored in shared memory                        */
		/* ------------------------------------------------------------------------------------- */

		/* String data value returned from the most recently requested command */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern string uvCmn_MDR_GetRecvLastString();
		/* Returns the Retracted Position value according to the current Effector Number */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern double uvCmn_MDR_GetRetractedPositionForEffectorNumber();

		/* Currently set custom station parameter (cmd : cst) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern  Char uvCmn_MDR_GetValueStationName();
		/* Serial number of the robot (cmd : snr) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern string uvCmn_MDR_GetValueSerialNumber();
		/* Current debug level, or calibration parameters (cmd : aofd) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern SByte uvCmn_MDR_GetValueOTFDebugLevel();
		/* Currently set information mode, and option (cmd : inf) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValueInfoMode();
		/* Currently set station set number (cmd : _css) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValueStationSetNo();
		/* Current station number (cmd : cstn) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValueStationNo();
		/* Current scanner type (cmd : _st) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValueScannerType();
		/* Current slot of a station (cmd : csl) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValueCurrentSlotNo();
		/* Get end effector number in use for a station  (cmd : een) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValueEndEffectorNum();
		/* Get end effector orientation (up or down) in use for a station  (cmd : eeo) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValueEndEffectorStationUpDown();
		/* Get inline station type (cmd : IST) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValueLinlineStationType();
		/* Current state of loaded parameters (cmd : lenb) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValueLoadedSpeedEnable();
		/* Number of slots of specified or default station (cmd : nsl) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValueStationSlotNum();
		/* Number of stations (cmd : nst) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValueStationNum();
		/* Current value of station pusher type (cmd : psh) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValuePusherType();
		/* Numeric value, 0 or 1 (cmd : sbusy) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValueStationServiceBusy();
		/* Current station lock state (cmd : slck) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValueStationLockedState();
		/* Currently set motion overlap factor (cmd : tbl) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValueStationAxisOverlapFactor();
		/* Current wafer check options (cmd : vchk) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValueStationTrajectoryMode();
		/* 0: Wafer was not detected; 1: Wafer was detected (cmd : vst) num=0 -> Top Effector, num=1 -> Bottom Effector */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValueEndEffectorWaferCheck(Byte num);
		/* Wait for Motion Completed (cmd : wmc) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValueMotionWaitFinished();
		/* Current state of Check for notch on second scan (cmd : aofcn2) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValueOTFSetSecondScanNotch();
		/* Current state of Ignore Middle Samples option (cmd : aofim) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValueOTFIgnoreMiddleSamples();
		/* Currently set Scan Debug Mode (cmd : sdbg) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValueScanDebugMode();
		/* Current arm used for scanning of specified station (cmd : see) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MDR_GetValueArmUsedScanSpecStation();
		/* Print-out of wafer map from last scanning, if available (cmd : map) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueWaferMapLoadedState();		/* return info (type : Byte, size : 32) */
		/* Currently set current limit of all axes (cmd : _cl) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueCurrentLimit();				/* return info (type : Byte: , size : 4) */
		/* Current state of Controlled Stop Feature. (cmd : cstp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueControlledStop();				/* return info (type : Byte: , size : 2) */
		/* Current failure options (cmd : pgf) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValuePutGetFailedOption();			/* return info (type : Byte: , size : 2) */
		/* Currently set Get option and T and Z axes synchronization mode (cmd : pgo) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValuePutGetOption();				/* return info (type : Byte: , size : 2) */
		/* Current wafer check options (cmd : vchk) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueStationVacuumCheck();			/* return info (type : Byte: , size : 2) */
		/* Absolute Encoder status (cmd : _aest) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueEncStatus();					/* return info (type : Int16: , size : 4) */
		/* Low-level absolute encoder status (cmd : _ralmc) => Not perfect */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MDR_GetValueLowLevelEncStatus();
		/* Input state of all digital inputs (cmd : inp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MDR_GetValueDigitalInputState();
		/* Output state of all digital inputs (cmd : out) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MDR_GetValueDigitalOutputState();
		/* All robot axes home switches state (cmd : rhs) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MDR_GetValueHomeSwitchState();
		/* Error code from last failed command (cmd : eerr) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MDR_GetValueExtendedErrorCode();
		/* Currently set on-the-fly set number (cmd : acs) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MDR_GetValueOTFSetNumber();
		/* System status word (cmd : sta) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MDR_GetValueSystemStatus();
		/* Currently set time-out values (cmd : _bb) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueBackupDCPower();				/* return info (type : UInt16: , size : 2) */
		/* Currently set custom generic parameter (cmd : _cpa) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 uvCmn_MDR_GetValueCustomGenericParam();
		/* Currently set custom axis parameter (cmd : _cax) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 uvCmn_MDR_GetValueCustomAxisParam();
		/* Currently set custom station parameter (cmd : _cst) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 uvCmn_MDR_GetValueCustomStationParam();
		/* Currently set position error limit of all axes (cmd : _el) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValuePositionErrorLimit();			/* return info (type : Int32: , size : 4) */
		/* Input state of digital input (cmd : din) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 uvCmn_MDR_GetValueExternalDIInState();
		/* Last state set to digital output (cmd : dout) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 uvCmn_MDR_GetValueExternalDIOutState();
		/* Currently set delay (cmd : pgd) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 uvCmn_MDR_GetValuePutGetDelayTime();
		/* Currently set time out (cmd : vto) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 uvCmn_MDR_GetValueVacuumSearchTimeOut();
		/* Currently set maximum wafer thickness (cmd : _mt) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueMaximumWaferThick();
		/* Current value of R axis pushing acceleration (cmd : _pa - for default station) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueRAxisPushAccDef();
		/* Current value of R axis pushing acceleration (cmd : _pa - for last station) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueRAxisPushAccSta();
		/* Current value of R axis pushing speed (cmd : _ps - for default station) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueRAxisPushSpeedDef();
		/* Current value of R axis pushing speed (cmd : _ps - for last station) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueRAxisPushSpeedSTA();
		/* Current wafer scanning Z axis acceleration for given station (cmd : _sa) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueWaferScanZAxisAcc();
		/* Current wafer scanning Z axis speed for given station (cmd : _ss) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueWaferScanZAxisSpeed();
		/* Currently set center offset (cmd : cof) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueStationCenterOffset();
		/* Currently set center offset (special operation) (cmd : cofp) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueStationCenterOffsetEdgeGrip();
		/* Currently Inline Station Retracted R position for the default station (cmd : irr) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueStationRetractedRPosition();
		/* Currently Inline Station Retracted T position for the default station (cmd : irt) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueStationRetractedTPosition();
		/* Currently Inline Exit Angle for the default station (cmd : isa) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueStationExitAngle();
		/* Currently Inline Exit Distance for the default station (cmd : isd) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueStationExitDistance();
		/* Currently Inline Station End effector Length for the default station (cmd : ise) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueStationEndEffectLength();
		/* Currently set offset (cmd : ofs) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueStationOffsetZ();
		/* Currently set pitch (cmd : pit) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueStationPitch();
		/* Current value of Start Pushing Distance (cmd : psd) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueStartPushingDistance();
		/* Current value of Start Pushing Offset (cmd : pss) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueStationStartPushingOffset();
		/* Current value of Z push offset (cmd : psz) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueStationPushingOffsetZ();
		/* Currently set pickup acceleration (cmd : pua) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueStationPickupAccelerationZ();
		/* Currently set pick-up speed (cmd : pus) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueStationPickupSpeedZ();
		/* Currently set position (cmd : rpo) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueStationRetractedPositionR();
		/* Currently set maximum wafer thickness for Station (cmd : smt) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueStationWaferMaxThickness();
		/*Current Scanning Highest Z Position, for a given or default station  (cmd : szh) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueStationScaningZHighestZCoord();
		/* Current Scanning Lowest Z Position, for a given or default station (cmd : szl) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueStationScaningZLowestCoord();
		/* Currently set stroke for z axis (cmd : str) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MDR_GetValueStationStrokeZ();
		/* Currently set maximum beam thresholds for Global (cmd : _bt) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueBeamThreshold();				/* return info (type : Double: , size : 2) */
		/* Currently set maximum beam thresholds for Station (cmd : sbt) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueStationBeamThreshold();		/* return info (type : Double: , size : 2) */
		/* Currently set station position (cmd : spon) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueStationPosition();			/* return info (type : Double: , size : 4) */
		/* Current Scanning Right Coordinate, for all or given axis (cmd : srp) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueStationScanningRightCoord();	/* return info (type : Double: , size : 4) */
		/* Current Scanning Left Coordinate, for all or given axis (cmd : slp) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueStationScanningLeftCoord();	/* return info (type : Double: , size : 4) */
		/* Current Scanning Center Coordinate, for all or given axis (cmd : scp) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueStationScanningCenterCoord();	/* return info (type : Double: , size : 4) */
		/* Currently set maximum beam thresholds for Station (cmd : sbt) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueStationPerBeamThreshold();	/* return info (type : Double: , size : 4) */
		/* Current position of all axes (cmd : cpo) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueAxisCurrentPosition();		/* return info (type : Double: , size : 4) */
		/* Currently set integral limit of all axes (cmd : _il) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueAxisIntegralLimit();			/* return info (type : Double: , size : 4) */
		/* Currently set proportional coefficient of all axes (cmd : _kp) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueAxisPropCoeff();				/* return info (type : Double: , size : 4) */
		/* Currently set integral coefficient of all axes (cmd : _ki) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueAxisInteCoeff();				/* return info (type : Double: , size : 4) */
		/* Currently set differential coefficient of all axes (cmd : _kd) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueAxisDiffCoeff();				/* return info (type : Double: , size : 4) */
		/* Currently set proportional coefficient of all axes (cmd : _hp) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueAxisPropCoeffHold();			/* return info (type : Double: , size : 4) */
		/* Currently set integral limit of all axes (cmd : _hl) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueAxisInteLimitHold();			/* return info (type : Double: , size : 4) */
		/* Currently set integral coefficient of all axes (cmd : _hi) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueAxisInteCoeffHold();			/* return info (type : Double: , size : 4) */
		/* Currently set position error limit of all axes (cmd : _he) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValuePosErrLimitHold();			/* return info (type : Double: , size : 4) */
		/* Currently set differential coefficient of all axes (cmd : _hd) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueAxisDiffCoeffHold();			/* return info (type : Double: , size : 4) */
		/* Currently set Reverse safe envelope position for all the axes (cmd : _rse) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueAxisSafeEnvelopReverse();		/* return info (type : Double: , size : 4) */
		/* Currently set forward safe envelope position for all the axes (cmd : _fse) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueAxisSafeEnvelopForward();		/* return info (type : Double: , size : 4) */
		/* Currently set axis speed (cmd : spd) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueAxisWorkingSpeed();			/* return info (type : Double: , size : 4) */
		/* Currently set loaded speed (cmd : lspd) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueAxisLoadedSpeed();			/* return info (type : Double: , size : 4) */
		/* Currently set axis deceleration jerk (cmd : ldjk) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueAxisLoadedDecelerationJerk();	/* return info (type : Double: , size : 4) */
		/* Currently set axis acceleration jerk (cmd : lajk) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueAxisLoadedAccelerationJerk();	/* return info (type : Double: , size : 4) */
		/* Currently set loaded acceleration (cmd : ldcl) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueAxisLoadedDeceleration();		/* return info (type : Double: , size : 4) */
		/* Currently set loaded acceleration (cmd : lacl) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueAxisLoadedAcceleration();		/* return info (type : Double: , size : 4) */
		/* Currently set axis deceleration jerk (cmd : djk) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueAxisDecelerationJerk();		/* return info (type : Double: , size : 4) */
		/* Currently set axis acceleration jerk (cmd : ajk) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueAxisAccelerationJerk();		/* return info (type : Double: , size : 4) */
		/* Currently set axis deceleration (cmd : dcl) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueAxisDeceleration();			/* return info (type : Double: , size : 4) */
		/* Currently set axis acceleration (cmd : acl) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueAxisAcceleration();			/* return info (type : Double: , size : 4) */
		/* Currently offset compensation with slot (cmd : _zc) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueCoordCompensationSlot();		/* return info (type : Double: , size : 4) */
		/* Currently set Reverse direction software limits for all the axes (cmd : _rl) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueSWNegativeLimit();			/* return info (type : Double: , size : 4) */
		/* Currently set forward direction software limit for all the axes (cmd : _fl) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueSWPositiveLimit();			/* return info (type : Double: , size : 4) */
		/* Currently set encoder zero offsets of all the axes (cmd : _eor) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueEncZeroOffset();				/* return info (type : Double: , size : 4) */
		/* Currently set customized home position of all the axes (cmd : _ch) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MDR_GetValueHomedPosition();				/* return info (type : Double: , size : 4) */
	}
}
