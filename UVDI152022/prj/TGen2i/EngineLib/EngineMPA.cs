using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

/* User-defined Namespace */
using TGen2i.Enum.Comn;
using TGen2i.Enum.Milara;
using TGen2i.Enum.MPA;

namespace TGen2i.EngineLib
{
	public class EngMPA
	{
		/* ------------------------------------------------------------------------------------- */
		/*                               Shared memory in Aligner                                 */
		/* ------------------------------------------------------------------------------------- */

		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvEng_ShMem_GetLSPA();	/* Marshal.PtrToStructure(ptrMPA, typeof(STG_MPDV)) */

		/* ------------------------------------------------------------------------------------- */
		/*             External Interface - PreAligner for Logosol < for Engine >                */
		/* ------------------------------------------------------------------------------------- */

		/* Check whether Logosol Prealigner is currently connected */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_IsConnected();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_IsInitialized();
		/* Initialize existing communication-related send/receive data */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern void uvEng_MPA_ResetCommData();
		/* Returns the response status value for the most recently sent command */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern ENG_MSAS uvEng_MPA_GetLastSendAckStatus();
		/* Last received command size (unit: Bytes) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 uvEng_MPA_GetRecvSize();
		/* This command sets or returns the mode of the firmware response after the end of a macro execution. (cmd : inf) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetInfoMode(/*Internal : 0x01*/);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetInfoMode();
		/* This command displays the current axis position (cmd : mcpo) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetAxisPos();
		/* This command displays the current status of the prealigner (cmd : diag c)
		   or the status at the time of last motion or system error. */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetDiagState();
		/* Detailed error description of last error (cmd : est) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetMotionState();
		/* Description of previous error (cmd : per) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetErrorReason();
		/* All prealigner axes home switch state (cmd : rhs) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetAllSwitchState();
		/* System status word (cmd : sta) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetSystemStatus();
		/* The version number of the firmware. Optional parameter causes version information
		   for different modules to be displayed as well (cmd : ver) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetVerModuleAll();
		/* Set/Display Customized Home Position (cmd : _ch) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetAxisHomePosition();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetAxisHomePosition(ENG_PANC axis, Double value);
		/* sets the customized home position of Axis to this axis’ current position (cmd : _ch) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetAxisCurHomePosition(ENG_PANC axis);
		/* Set/Display Axis Current Limit (cmd : _cl) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetAxisCurrentLimit();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetAxisCurrentLimit(ENG_PANC axis, Byte value);
		/* Set/Display Axis Position Error Limit (cmd : _el) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetAxisPositionLimitError();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetAxisPositionLimitError(ENG_PANC axis, UInt16 value);
		/* Set/Display Axis Reverse Direction Software Limit (cmd : _rl) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetAxisDirectLimit(ENG_MAMD direct);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetAxisDirectLimit(ENG_PANC axis, ENG_MAMD direct, Double value);
		/* Set/Display home offset (cmd : _ho) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetAxisHomeOffset();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetAxisHomeOffset(ENG_PANC axis, Double value);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetAxisCurHomeOffset(ENG_PANC axis);
		/* Set/Display Axis Acceleration (cmd : acl or dcl) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetAxisInstSpeed(ENG_MAST type);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetAxisInstSpeed(ENG_PANC axis, ENG_MAST type, Double value);
		/* Set/Display Axis Acceleration or Deceleration Jerk (cmd : ajk or djk) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetAxisJerkSpeed(ENG_MAST type);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetAxisJerkSpeed(ENG_PANC axis, ENG_MAST type, Double value);
		/* Set/Display Axis Velocity Profile Type (cmd : pro) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetVelocityProfile();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetVelocityProfile(ENG_PANC axis, Byte profile);
		/* Set/Display Axis Speed   (cmd : spd) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetAxisSpeed();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetAxisSpeed(ENG_PANC axis, Double speed);
		/* Start homing the prealigner (cmd : hom) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetAxisAllHoming();
		/* Move to Home position (cmd : mth) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_MoveHomePosition(ENG_PANC axis);
		/* Move Axis to Absolute (or Relative) Position (cmd : mva or mvr) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_MoveToAxis(ENG_PANC axis, ENG_PAMT type, Double value);
		/* Turn axis servo off or on (cmd : son or soff) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_ServoControl(ENG_PANC axis, ENG_PSOO type);
		/* Stop Motion, Terminate Execution (cmd : stp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_StopToAxis(ENG_PANC axis);
		/* Wait for Motion Completion (cmd : wmc) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_WaitPrealigner(ENG_PANC axis);
		/* Turn off (or on) the chuck vacuum (cmd : cvf or cvn) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_ChuckVacuum(ENG_PSOO type);
		/* Start alignment (cmd : bal) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_FindNotchAngle(Double max_offset=0.0, Byte offset_try = 0,
															  Double max_angle = 0.0, Byte angle_try = 0,
															  Byte orient_notch = 0);
		/* Display CCD Sensor Value (cmd : ccd) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetCCDSensorPixels();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetCCDSensorPixels(UInt16 value);
		/* Detect an Object Using CCD Sensors (cmd : doc) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_CheckWaferLoaded();
		/* Set/Display Final Wafer Orientation (cmd : fwo) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetRotationAfterAlign(Double angle);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetRotationAfterAlign();
		/* Set/Display Wafer Loading Type (cmd : ltc) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetWaferPlaceType(ENG_PWPT type);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetWaferPlaceType();
		/* Display Number of Bad (Good) Notches (cmd : nbn or ngn) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetWaferNotchCount(ENG_PASR type);
		/* Display Depth and Width of a Notch (cmd : ndw) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetWaferAlignDepthWidth(Int16 index);
		/* Set/Display minimum (or maximum) notch depth (or width) (cmd : _mnd, _mxd, _mnw, _mxw) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetWaferFindNotchRange(ENG_PNTI type, Byte kind);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetWaferFindNotchRange(ENG_PNTI type, Byte kind, UInt32 range);
		/* desc :Set/Display minimum good data samples. (cmd : _mgd) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetMinCCDSampleCount();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetMinCCDSampleCount(UInt16 samples);
		/* Set/Display Maximum (or Minimum) Wafer Size (cmd : _maxwsz or _minwsz) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetWaferMinMaxSize(ENG_MMMR type, ENG_MWKT kind);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetWaferMinMaxSize(ENG_MMMR type);
		/* Set/Display Wafer Size (cmd : wsz) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetWaferWorkSize(ENG_MWKT kind);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetWaferWorkSize();
		/* Set/Display Wafer Size Light Source Select (cmd : wsz2) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetWaferWorkLight(Byte light);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetWaferWorkLight();
		/* Read wafer information (cmd : rwi) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetWaferCenterOrientation();
		/* Set/Display automatic wafer Size detection. (cmd : _asz) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetAutoDetectWaferSize(ENG_PWSD type);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetAutoDetectWaferSize();
		/* Turn On or Off Second Scan in Case of Alignment Failure (cmd : _autoscanagain) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetAutoScanAgain(ENG_MFED type);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetAutoScanAgain();
		/* Set/Display vacuum on chuck after BAL state. (cmd : _finalvacc) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetVacuumSetAfterBAL(ENG_PSOO type);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetVacuumSetAfterBAL();
		/* Set/Display Glass Mode (cmd : _glm) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetWaferGlassType(ENG_PWGO type);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetWaferGlassType();
		/* Set/Display load down (or up) position (cmd : _ld or _lu) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetWaferLoadPosition(ENG_PWLP type);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetWaferLoadPosition(ENG_PANC axis, ENG_PWLP type, Double pos);
		/* Set/Display measure acceleration (or deceleration) (cmd : _ma or _md) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetMeasureAccDcl(ENG_MAST type);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetMeasureAccDcl(ENG_PANC axis, ENG_MAST type, Double value);
		/* Set/Display measure speed (cmd : _ms) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetMeasureSpeedAxis();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetMeasureSpeedAxis(ENG_PANC axis, Double value);
		/* Set/Display notch / flat depth calculation mode. (cmd : _notchdepthmode) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetNotchDepthMode();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetNotchDepthMode(Boolean enable);
		/* Set/Display measure position (cmd : _mp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetWaferAlignmentPos();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetWaferAlignmentPos(ENG_PANC axis, Double value);
		/* Set/Display MTP command mode of operation. (cmd : _mtpmode) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetModeZAfterMovePins();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetModeZAfterMovePins(ENG_PWLP mode);
		/* Set/Display Z-axis coordinate at pins position. (cmd : _pp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetLevelChcukToPinsPosZ();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetLevelChcukToPinsPosZ(Double value);
		/* Set/Display pins position correction when going down (or up). (cmd : _ppcd or _ppcu) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetOffsetChcukToPinsPosZ(ENG_PWLP type);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetOffsetChcukToPinsPosZ(ENG_PWLP type, Double value);
		/* Set/Display CCD sensor angle. (cmd : _sa) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetAngleRAxisToCCD();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetAngleRAxisToCCD(Double value);
		/* Set/Display sample averaging mode. (cmd : _sam) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetNotchAverageMode();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetNotchAverageMode(Boolean enable);
		/* Set/Display CCD sensor center value. (cmd : _sc) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetCCDSensorCenter();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetCCDSensorCenter(UInt32 pixel);
		/* Set/Display sampling mode. (cmd : _sm) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetRotationSampleMode();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetRotationSampleMode(ENG_PRSM mode);
		/* Set/Display stop on pins mode. (cmd : _sp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetStopOnPins();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetStopOnPins(Byte mode);
		/* Set/Display maximum (or minimum) CCD value for square substrates. (cmd : _sqmax or _sqmin) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetSuqreValueOfCCD(ENG_MMMR mode);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetSuqreValueOfCCD(ENG_MMMR mode, UInt32 area);
		/* Set/Display transfer down (or up) position. (cmd : _td or _tu) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetWaferTranserSafePos(ENG_PWLP mode);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetWaferTranserSafePos(ENG_PWLP mode, Double value);
		/* Set/Display timeouts. (cmd : _to) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetParamTimeOut(Byte param);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetParamTimeOut(Byte param, UInt32 value);
		/* Assign a set of parameters to wafersize. (cmd : _wszautoset) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetAssingAutoLoadWafer(SByte set, ENG_MWKT wafer);
		/* Set/Display “wafer” type. (cmd : _wt) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetWaferType();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetWaferType(ENG_PGWT type);
		/* Save performance set parameters. (cmd : wps) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_RestorePerformSetFile(Byte set_no);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_WritePerformSetFile(Byte set_no);
		/* Read parameter sets from file (done at start-up). (cmd : rps) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_RestoreParamSetFile();
		/* Save parameter sets. (cmd : sps) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SaveParamSetFile();
		/* Restore motion and calibration parameters. (cmd : rmp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_RestoreParamSetMemory();
		/* Save motion and calibration parameters. (cmd : smp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SaveParamSetMemory();
		/* Set system date and time. (cmd : _dt) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetSystemTime();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetSystemTime();
		/* Homing speed and acceleration (or Working speed and acceleration). (cmd : hsa or wsa) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_RestoreTrajectorySpeedAcc(Byte mode);
		/* Reset the controller. (cmd : res) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_ResetPrealigner();
		/* Enables saving the CCD samples from unsuccessful alignment. (cmd : _autodumps) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetAlignFailAutoDumps();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetAlignFailAutoDumps(Byte mode);
		/* Specifies the file name for CCD samples from unsuccessful alignment. (cmd : _autodumpsf) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetAlignFailAutoDumpFile();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetAlignFailAutoDumpFile(Byte file_no);
		/* Complete calibration procedure of standard (non-edge handling) prealigners. (cmd : _cal) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetPhysicalChangeCalibration();
		/* Calibration procedure of CCD compensation factors. (cmd : _cal_cor) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetPhysicalChangeCompensation(Byte mode1, Byte mode2);
		/* Saves CCD samples to .BAL file. (cmd : ddpf) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetDetailTroubleFileByManufacture(Byte file_no);
		/* Save CCD Samples to a File (cmd : ddpf) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetBasicTroubleFileByManufacture(Byte file_no);
		/* Prepares CCD samples for saving to .BAL file. (cmd : drps) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetTroubleDumpToMemory();
		/* Verifies the prealigner is edge handling. (cmd : _eh) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetEdgeHandleType();
		/* Maximum chuck CCD value. (cmd : _ehc or _ehc1 or _ehc2) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetCCDSensorSensingDist(Byte type);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetCCDSensorSensingDist(UInt16 pixel, Byte type);
		/* Minimum chuck pins CCD value. (cmd : cmd : _ehp or _ehp1 or _ehp2) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetCCDPinsSensingDist(Byte type);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetCCDPinsSensingDist(UInt16 pixel, Byte type);
		/* Set/Display safe Z position above which which the chuck is clear of the stationary pins can rotate freely. (cmd : _sfz) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_GetSafeRotateAxisZ();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_MPA_SetSafeRotateAxisZ(Double value);

		/* ------------------------------------------------------------------------------------- */
		/*                 External Interface  - PreAligner for Logosol < for Common >                   */
		/* ------------------------------------------------------------------------------------- */

		/* Whether the state value of system is valid (!!! Important !!!) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MPA_IsValidSystemStatus();
		/* Whether the current motion (Robot Arm) is moving, i.e. Busy (asynchronous control) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MPA_IsMotorArmBusy();
		/* Whether the current motion (Robot Arm) is idle (asynchronous control) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MPA_IsMotorArmIdle();
		/* Whether there are packets received for the most recently sent command */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MPA_IsLastSendAckRecv();
		/* Whether the receive response was successful for the most recently sent command */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MPA_IsLastSendAckSucc();
		/* Returns an error message for system status values */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MPA_GetSystemStatusMesg(String mesg, UInt32 size);
		/* Returns detailed string information about the error value (code) of the previously sent command */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MPA_GetPreviousError(String mesg, UInt32 size);
		/* Whether there is the most recently received data */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MPA_IsRecvCmdData(ENG_PSCC cmd=ENG_PSCC.psc_none);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MPA_IsRecvCmdLast();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MPA_GetRecvCmdLast(ref String r_cmd, UInt32 size);
		/* Whether an error has occurred inside the equipment (system) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_MPA_IsSystemError();

		/* ------------------------------------------------------------------------------------- */
		/*                            Returns a value stored in shared memory                            */
		/* ------------------------------------------------------------------------------------- */

		/* String data value returned from the most recently requested command */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern String uvCmn_MPA_GetValueLastString();
		/* Currently set wafer type ((cmd : _wt) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern SByte uvCmn_MPA_GetValueWaferType();
		/* Currently assigned parameter set to the current wafer size (unit: inch) ((cmd : _wszautoset) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern SByte uvCmn_MPA_GetValueWaferSizeInch();
		/* Currently set information mode ((cmd : inf) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueInfoMode();
		/* Information about whether and object is covering any of the CCD sensors ((cmd : doc) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueWaferLoaded();
		/* Currently set mode (0 or 1 – Automatic wafer size detection mode) ((cmd : asz) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueWaferSizeDetection();
		/* Currently set mode (0 or 1 – “Auto scan again” mode) ((cmd : _autoscanagain) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueWaferScanAgain();
		/* Currently set T-axis fast speed (0 or 1– State of chuck vacuum OFF or ON after BAL) ((cmd : _finalvacc) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueVacuumStateAfterAlign();
		/* Currently setting of _GLM (0 or 1) ((cmd : _glm) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueWaferGlassType();
		/* Currently set notch/flat depth calculation mode (0 or 1) ((cmd : _notchdepthmode) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueNotchDepthMode();
		/* Currently set MTP command mode (0 or 1) ((cmd : _mtpmode) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueModeZAfterMovePins();
		/* Currently set sampling mode (0x00 to 0x03) ((cmd : _sm) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueRotationSampleMode();
		/* Currently set „Stop on pins” mode (0 or 1) ((cmd : _sp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueStopOnPins();
		/* Currently set value (0 or 1) ((cmd : wsz2) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueNotchLightSource();
		/* Currently set mode (0 or 1 – “Auto samples dump” mode) ((cmd : _autodumps) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueAlignFailAutoDumps();
		/* Currently set file number (0 to 9 – file number for “Auto samples dump” mode) ((cmd : _autodumpsf) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueAutoSamplesDumpFile();
		/* 1 if the prealigner is edge handling, 0 otherwise ((cmd : _eh) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueIsEdgeHandling();
		/* Currently set current limit of all axes (1 to 251 level)  ((cmd : _cl) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MPA_GetValueCurrentLimits();				/* return info (type : Byte: , size : 3) */
		/* Currently set axis profile type. (cmd : pro) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MPA_GetValueVelocityProfiles();			/* return info (type : Byte: , size : 3) */
		/* Currently set minimum wafer size (cmd : _minwsz) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueWaferMinSize();
		/* Currently set maximum wafer size (cmd : _maxwsz) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueWaferMaxSize();
		/* Currently set wafer diameter (cmd : wsz) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueWaferWorkSize();
		/* Currently set averaging mode (current sampling mode) (0, 3 or 5) (cmd : _sam) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueNotchAverageMode();
		/* Currently set wafer loading type (0 – load wafer on pins; 1 – load wafer on chuck) (cmd : ltc) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueWaferLoadingType();
		/* Currently set minimum notch depth. (1 to 100) (cmd : _mindepthmain) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueMinimumNotchDepth();
		/* Currently set notch find mode (0 to 3) (cmd : _notchfindmode) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueNotchFindMode();
		/* Currently set maximum notch/flat slope (cmd : _notchmaxderval) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueNotchMaxDerVal();
		/* Currently set minimum notch/flat slope (cmd : _notchminderval) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueNotchMinDerVal();
		/* Currently set number of samples outside notch/flat (cmd : _notchnavg) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_MPA_GetValueNotchNumAverage();
		/* Currently set limit (1 -> 0.0001 inch) (cmd : _ao2) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MPA_GetValueAlignOffsetTwice();
		/* Currently set acceptable offset (1 -> 0.0001 inch) (cmd : _aof) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MPA_GetValueAlignOffsetAccept();
		/* All prealigner axes home switch state (cmd : rhs) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MPA_GetValueAxisSwitchActive();
		/* System status word (cmd : sta) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MPA_GetValueSystemStatus();
		/* Current value of CCD sensor(cmd : ccd) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MPA_GetValueCCDPixels();
		/* Number of notches falling within minimum/maximum depth/width criteria(cmd : ngn) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MPA_GetValueLastAlignSucc();
		/* Number of notches failing out of minimum/maximum depth/width criteria (cmd : nbn) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MPA_GetValueLastAlignFail();
		/* Description of previous error (cmd : per) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MPA_GetValuePreviousCmdFail();
		/* Currently set minimum good data samples (cmd : _mgd) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MPA_GetValueMinGoodSampleCount();
		/* Currently set chuck CCD value (cmd : _ehc) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MPA_GetValueCCDChuckSensingValue();
		/* Currently set chuck CCD value of smaller size (cmd : _ehc1) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MPA_GetValueCCDChuckSensingSmall();
		/* Currently set chuck CCD value of greater size. (cmd : _ehc2) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MPA_GetValueCCDChuckSensingGreat();
		/* Currently set chuck pins CCD value (cmd : _ehp) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MPA_GetValueCCDPinsSensingValue();
		/* Currently set chuck pins CCD value of smaller size. (cmd : _ehp1) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MPA_GetValueCCDPinsSensingSmall();
		/* Currently set chuck pins CCD value of greater size (cmd : _ehp2) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_MPA_GetValueCCDPinsSensingGreat();
		/* Currently set CCD center (cmd : _sc) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 uvCmn_MPA_GetValueCCDSensorCenter();
		/* Currently set timeout value (cmd : _to) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 uvCmn_MPA_GetValueLastParamTimeout();
		/* Currently set minimum notch depth. (cmd : _mnd) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 uvCmn_MPA_GetValueNotchRangeDepthMin();
		/* Currently set maximum notch depth. (cmd : _mxd) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 uvCmn_MPA_GetValueNotchRangeDepthMax();
		/* Currently set minimum notch width. (cmd : _mnw) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 uvCmn_MPA_GetValueNotchRangeWidthMin();
		/* Currently set maximum notch width (cmd : _mxw) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 uvCmn_MPA_GetValueNotchRangeWidthMax();
		/* Currently set minimum CCD value for square substrates (cmd : _sqmin) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 uvCmn_MPA_GetValueCCDGrabSizeMin();
		/* Currently set maximum CCD value for square substrates (cmd : _sqmax) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 uvCmn_MPA_GetValueCCDGrabSizeMax();
		/* Two numbers, denoting depth and width of requested (or first) notch (cmd : ndw) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MPA_GetValueFindDepthWidth();				/* return info (type : UInt32: , size : 2) */
		/* Currently set position error limit of all axes (cmd : _el) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MPA_GetValueErrorLimit();					/* return info (type : UInt16: , size : 3) */
		/* Currently set final wafer orientation angle (cmd : fwo) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MPA_GetValueRotateAfterAlign();
		/* Currently set transfer down position (cmd : _td) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MPA_GetValueWaferTransSafePosUp();
		/* Currently set transfer up position (cmd : _tu) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MPA_GetValueWaferTransSafePosDown();
		/* Currently set safe Z position (cmd : _sfz) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MPA_GetValueSafeRotateZPos();
		/* Currently set angle (cmd : _sa) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MPA_GetValueCCDRAxisAngle();
		/* Currently set pins position offset (Moving Up) (cmd : _ppcu) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MPA_GetValueOffsetChuckPinPosUp();
		/* Currently set pins position offset (Moving Down) (cmd : _ppcd) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MPA_GetValueOffsetChuckPinPosDown();
		/* Currently set pins position (cmd : _pp) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_MPA_GetValueLevelChuckPinsPosZ();
		/* Currently set load up position for all axes. (cmd : _lu) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MPA_GetValueWaferLoadingUpPos();			/* return info (type : Double: , size : 3) */
		/* Currently set load down position for all axes. (cmd : _ld) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MPA_GetValueWaferLoadingDownPos();		/* return info (type : Double: , size : 3) */
		/* Current position of all axes (cmd : cpo) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MPA_GetValueAxisPos();					/* return info (type : Double: , size : 3) */
		/* Currently set customized home position of all the axes (cmd : ) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MPA_GetValueHomePos();					/* return info (type : Double: , size : 3) */
		/* Currently set home offset of all the axes (cmd : _ho) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MPA_GetValueHomeOffset();					/* return info (type : Double: , size : 3) */
		/* Currently set forward direction software limit for all axes. (cmd : _fl) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MPA_GetValueDirectLimitF();				/* return info (type : Double: , size : 3) */
		/* Currently set Reverse direction software limit for all the axes (cmd : _rl) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MPA_GetValueDirectLimitR();				/* return info (type : Double: , size : 3) */
		/* Currently set axis acceleration (cmd : acl) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MPA_GetValueAxisACL();					/* return info (type : Double: , size : 3) */
		/* Currently set axis deceleration (cmd : dcl) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MPA_GetValueAxisDCL();					/* return info (type : Double: , size : 3) */
		/* Currently set axis acceleration jerk (cmd : ajk) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MPA_GetValueAxisAJK();					/* return info (type : Double: , size : 3) */
		/* Currently set axis deceleration jerk (cmd : djk) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MPA_GetValueAxisDJK();					/* return info (type : Double: , size : 3) */
		/* Currently set speed for all or one axis (cmd : spd) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MPA_GetValueAxisSpeed();					/* return info (type : Double: , size : 3) */
		/* Displacement (offset), offset angle, and notch/flat angle (cmd : rwi) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MPA_GetValueRWIWaferInfo();				/* return info (type : Double: , size : 3) */
		/* Currently set measure acceleration (cmd : _ma) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MPA_GetValueMeasureACL();					/* return info (type : Double: , size : 3) */
		/* Currently set measure deceleration (cmd : _md) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MPA_GetValueMeasureDCL();					/* return info (type : Double: , size : 3) */
		/* Currently set measuring speed (cmd : _ms) (unit : um or degree or sec) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_MPA_GetValueMeasureSpeed();				/* return info (type : Double: , size : 3) */
	}
}
