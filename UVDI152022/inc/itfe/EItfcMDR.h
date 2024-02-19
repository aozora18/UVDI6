
/*
 desc : LED Power Library
*/

#pragma once

#include "../conf/global.h"
#include "../conf/define.h"
#include "../conf/milara.h"


#ifdef __cplusplus
extern "C"
{
#endif

/* --------------------------------------------------------------------------------------------- */
/*                     외부 함수 - Diamond Robot for Logosol < for Engine >                      */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Check whether the robot is currently connected
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MDR_IsConnected();
/*
 desc : Initialize the previously saved send/recv buffer
 parm : None
 retn : None
*/
API_IMPORT VOID uvEng_MDR_ResetCommData();
/*
 desc : Returns the response status value for the most recently sent command
 parm : None
 retn : 0x00 - FAIL, 0x01 - SUCC, 0xff - Not defined (Not received)
*/
API_IMPORT ENG_MSAS uvEng_MDR_GetLastSendAckStatus();

/* --------------------------------------------------------------------------------------------- */
/*                        Command Reference  (1. Informational commands)                         */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Set/Report System Time and Date (cmd : _dt)
 parm : None
 retn : TRUE or FALSE
 exam : 
*/
API_IMPORT BOOL uvEng_MDR_SetDateTime();
API_IMPORT BOOL uvEng_MDR_GetDateTime();
/*
 desc : Set/Report System Configuration Information
 parm : None
 retn : TRUE or FALSE
 note : uvMDR_SetSystemConfigInfo() => !!! Do not call it except in special cases !!!
 exam : None
*/
API_IMPORT BOOL uvEng_MDR_SetSystemConfigInfo();
API_IMPORT BOOL uvEng_MDR_GetSystemConfigInfo();
/*
 desc : Reset the Controller
 parm : None
 retn : TRUE or FALSE
 exam : None
*/
API_IMPORT BOOL uvEng_MDR_ResetMotionController();
/*
 desc : Report Robot Serial Number
 parm : None
 retn : TRUE or FALSE
 exam : None
*/
API_IMPORT BOOL uvEng_MDR_GetRobotSerialNumber();
/*
 desc : Report the firmware version number
 parm : option	- [in]  0: Normal, 1: Detail
 retn : TRUE or FALSE
 exam : None
*/
API_IMPORT BOOL uvEng_MDR_GetRobotFirmwareVer(UINT8 option);

/* --------------------------------------------------------------------------------------------- */
/*           Command Reference  (2.Housekeeping (parameters save / restore) commands)            */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : This command restores user-defined custom parameters (cmd : _cld)
 parm : None
 retn : TRUE or FALSE
 exam : >_CLD
		>_CPA 0
		123
		>
*/
API_IMPORT BOOL uvEng_MDR_RestoreCustomParameter();
/*
 desc : Restore Custom parameters (cmd : _csv)
 parm : None
 retn : TRUE or FALSE
 note : This command saves user-defined custom parameters
 exam : >_CPA 0 123
		>_CSV
		>
*/
API_IMPORT BOOL uvEng_MDR_SaveCustomParameter();
/*
 desc : Restore Motion and Calibration Parameters (cmd : rmp)
 parm : None
 retn : TRUE or FALSE
 note : This command restores previously saved motion parameters and robot calibration parameters
		(with SMP command) from NV (Non-volatile) memory.
		All motion and calibration parameters currently in effect are lost, and the saved ones take effect.
 exam : None
*/
API_IMPORT BOOL uvEng_MDR_RestoreMotionCalibrationParam();
/*
 desc : Read Parameters Sets from Disk (cmd : rps)
 parm : None
 retn : TRUE or FALSE
 note : This command re-reads all parameter sets from disk file named paramset.ini,
		overwriting existing values. This command is executed automatically during start-up.
 exam : None
*/
API_IMPORT BOOL uvEng_MDR_ReadParametersSetsFromDisk();
API_IMPORT BOOL uvEng_MDR_RestoreStationParameters();
API_IMPORT BOOL uvEng_MDR_SaveParametersSetsToDisk();
/*
 desc : Save Station Parameters (cmd : ssp)
 parm : None
 retn : TRUE or FALSE
 note : This command saves all the station parameters to an NV (Non-volatile) memory.
		This command cannot be used if the robot is busy (e.g. motion is active).
		Following station and wafer parameters are being saved:
		-. station coordinates, stroke, pitch, offset and retract position
		-. number of slots, default slot
		-. pick-up speed and acceleration
		-. vacuum time-outs and pick-up delay
		-. inline station parameters
		-. wafer mapping parameters
 exam : >SSP
		>
*/
API_IMPORT BOOL uvEng_MDR_SaveStationParameters();

/* --------------------------------------------------------------------------------------------- */
/*                     Command Reference  (3.Status and diagnostic commands)                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Report current system state (cmd : _cs)
 parm : None
 retn : TRUE or FALSE
 note : This command reports detailed current system state, and might be useful for diagnosing issues
 exam : >__CS
		Current system state at Date: 2018-1-12 Time: 12:4:2
		STA(axis): T=03000408 R=03000408 Z=03000408
		HWS(devic): T=10429 R=10429 Z=10449 331=00000000/00C08030
		POS(units): T=0 R=-5000 Z=0
		ERR(enc.c): T=0 R=0 Z=0
		ENC(enc.c): T=28780 R=-39505 Z=-1
		>
*/
API_IMPORT BOOL uvEng_MDR_GetCurrentRobotState();
/*
 desc : Report Current Position for an Axis (cmd : cpo)
 parm : None
 retn : TRUE or FALSE
 note : This command requests the current axes positions
 exam : >CPO R
		2000
		>CPO
		5000,2000,1000
		>
*/
API_IMPORT BOOL uvEng_MDR_GetAxisCurrentPositionAll();
/*
 desc : Set/Report Inform-After-Motion Mode (cmd : inf)
 parm : mode	- [in]  information mode. Optional number, range 0 to 5 (fixed. 0x01)
 retn : TRUE or FALSE
 note : This command sets or returns the mode of the firmware response after the end of a macro execution.
		The different modes of operation are intended for synchronizing different host software implementations.
		Mode 0 is designed for synchronous mode of operation.
			   In this mode, the communication protocol is strictly master-slave – the host sends a command,
			   then the firmware returns an answer, and then the host sends the next command.
		Mode 1 is intended for asynchronous mode of operation. After the end of a macro execution,
			   the firmware sends to the host the system status word in hexadecimal format and a prompt.
			   The prompt can be either ‘>’ for successfully executed macro or ‘?’ for non-successfully executed or interrupted macro.
		Mode 2 is similar to mode 1. The difference is that the firmware sends only a prompt.
		Mode 3 is similar to mode 1. Instead of status word, the firmware sends to the host a number of
			   text messages by decoding the status. This is useful for diagnostic when the host is running just a terminal emulator.
		Mode 4 outputs command name before reporting any values.
		Mode 5 outputs command name, error code, and status before reporting any values.
			   Error code is either 0 (no error), or 1 (error in parameters / state), and additional error info is available with EERR command.
 exam : >INF 0
		>GET A 1
		>STA
		1181
		>STA
		100C

		>INF 1
		>PUT A 1
		>1000
		>

		>INF 4
		INF:
		>CPO
		CPO:5000,-5000,660
		>STA
		STA:1008

		>INF 5
		INF:0,1008
		>GET A 1
		>STA
		STA:0,1181
		>GET:1,100C
		>CPO
		CPO:0,100C 5000,-5000,660

		>GET A 1
		>GET:1,100A
		?EERR
		EERR:130,1008
		>
*/
API_IMPORT BOOL uvEng_MDR_SetInfoMode();
API_IMPORT BOOL uvEng_MDR_GetInfoMode();
/*
 desc : Report diagnostic information (cmd : diag)
 parm : None
 retn : TRUE or FALSE
 note : This command reports detailed information about last motion error, and current robot state.
		Its results can be used to determine the reason for the error, and steps to resolve it.
 exam : >PUT A 1
		>1419
		?DIAG
		LAST FAULT Status:
		Stop Reason -> Axis R:PositionERROR,
		0:Axis T	-> Stopped, HomeT ON, MUMS
		1:Axis R	-> PositionERROR, HomeR ON, MUMS
		2:Axis Z	-> Stopped, HomeZ OFF, MUMS, Brake ENGAGED
		MainPOWER: LINE; BackupENABLE: OFF; ScanSOURCE: NONE
		ArmPOWER:OFF; SensorA: OFF; SensorB: OFF; SensorC: ON ; SensorD: ON
		OutputA: ON ; OutputB: OFF; OutputC: OFF; OutputD: OFF; OutputE: OFF
		Current State:
		SystemSTATE -> ACTIVE !!! SupervisorSTOP !!!
		Stop Reason -> Axis R:PositionERROR,
		0:Axis T	-> Stopped, HomeT ON, MUMS
		1:Axis R	-> PositionERROR, HomeR ON, MUMS
		2:Axis Z	-> Stopped, HomeZ OFF, MUMS, Brake ENGAGED
		MainPOWER: LINE; BackupENABLE: OFF; ScanSOURCE: NONE
		ArmPOWER:DIS; SensorA: OFF; SensorB: OFF; SensorC: ON ; SensorD: ON
		OutputA: DIS; OutputB: DIS; OutputC: DIS; OutputD: DIS; OutputE: OFF
		>
*/
API_IMPORT BOOL uvEng_MDR_GetDiagInfoCurrent();
API_IMPORT BOOL uvEng_MDR_GetDiagInfoLast();
/*
 desc : Report Extended Error Number (cmd : eerr)
 parm : None
 retn : TRUE or FALSE
 note : Request Extended Error Number
 exam : >INF 5
		INF:0,044D
		>MVA T 1000
		MVA:1,044D
		?EERR
		EERR:128,044D
		>
*/
API_IMPORT BOOL uvEng_MDR_GetExtendedErrorNumber();
/*
 desc : Extended Error Status (cmd : est)
 parm : None
 retn : TRUE or FALSE
 note : Detailed error description of last exception error
		The returned message is stored in the 'last_string_data' structure variable.
		You can check the result by calling the uvCmn_MDR_GetRecvLastString() function.
 exam : >PUT A 1
		>
		01419
		?EST
		Error: EC=23 EA=0 RC=1 EL=-1 at Date: 2018-1-12 Time: 12:16:15
		STA(axis): T=830884A8 R=830806A8 Z=830884A8
		HWS(devic): T=11951 R=11959 Z=11951 331=00C080B0/00C080B0
		POS(units): T=1170 R=-1510 Z=-1
		ERR(enc.c): T=0 R=-101 Z=-1
		ENC(enc.c): T=28781 R=-11701 Z=-41
		>
*/
API_IMPORT BOOL uvEng_MDR_GetExtendedErrorState();
/*
 desc : Report Extended System Status (cmd : esta)
 parm : None
 retn : TRUE or FALSE
 note : Extended system status - additional info and current axes positions
		This command reports additional system info as a hexa-decimal word, with following bits information
		0001	Current state of "Emergency Stop" input
		0002	Current state of "Controlled Stop" input
		0004	A flag indicating whether "Controlled Stop" has been activated
 exam : >ESTA
		0004,1658,-938,-1
		>CPO
		1658,-938,-1
		>
*/
API_IMPORT BOOL uvEng_MDR_GetExtendedErrorStateCode();
/*
 desc : Report Previous Error Reason (robot) (cmd : per v)
 parm : None
 retn : TRUE or FALSE
 note : Numeric value or text for previous command error
		You can check the result by calling the uvCmn_MDR_GetRecvLastString() function.
 exam : >GET A 1
		>0002
		>PER
		0004
		>PER V
		No wafer detected on paddle/gripper
		>
 code : 0000	No error
		0001	Robot not homed yet
		0002	Motion still in progress
		0004	GET command found no wafer
		0008	Invalid station parameter
		0010	Invalid number parameter
		0020	Wafer found at starting scan position
		0040	PUT command failed to release the wafer
		0080	GET command started with wafer on paddle
		0100	Wafer lost during transport
		0200	Rotating end effector timeout during rotate operation
		0400	Alignment operation failed
		0800	Invalid command, or servo loop OFF
		1000	AC power down, on backup power
		2000	Wafer position sensor did not trigger
		4000	Absolute Encoder pending error
		8000	Init procedure in progress
		10000	Align-on-Fly correction over limits
*/
API_IMPORT BOOL uvEng_MDR_GetPreviousErrorReason();
/*
 desc : Read Home Switches State (cmd : rhs)
 parm : None
 retn : TRUE or FALSE
 note : All robot axes home switches state
		This command returns the state of the home switches and limit switches.
		The result is 4 hexadecimal digits.
		For each axis, there are 4 bits showing the axis switches state.
		Bits 0 to 3 show the state of T,
		bits 4 to 7 show the state of R,
		and bits 8 to 11 show the state of Z. (형식: 0xKZRT)
		To get a message about the error code, call the uvCmn_MDR_GetSystemStatusMesg() function.
 exam : >RHS
		0001
		>_RL
		-24000,-10000,-200
		>MVA Z -210
		>
		?0020
		>RHS
		0501
		>
 code : 0001 : T axis home switch is active
		0002 : T axis forward software limit violated
		0004 : T axis reverse software limit violated
		0010 : R axis home switch is active
		0020 : R axis forward software limit violated
		0040 : R axis reverse software limit violated
		0100 : Z-axis home switch is active
		0200 : Z axis forward software limit violated
		0400 : Z axis reverse software limit violated
		1000 : K-axis home switch is active
		2000 : K axis forward software limit violated
		4000 : K axis reverse software limit violated
*/
API_IMPORT BOOL uvEng_MDR_GetHomeSwitchAll();
/*
 desc : Save Motion and Calibration Parameters (cmd : smp)
 parm : option	- [in]  0 or 1
						If option “1” is used, working speeds and accelerations are not saved.
 retn : TRUE or FALSE
 note : This command saves all axes motion parameters and the robot calibration parameters to the NV (Non-volatile) memory
 exam : None
*/
API_IMPORT BOOL uvEng_MDR_SaveMotionCalibrationParam(UINT8 option);
/*
 desc : Command Execution Status (cmd : sta)
 parm : None
 retn : TRUE or FALSE
 note : This command reports the general status of the controller
 exam : None
 code : 0001	Unable to execute motion commands
		0002	Previous command returned an error
		0004	Vacuum Sensor ON
		0008	Vacuum Switch ON
		0010	Motion or position error
		0020	Software Motion limits violation
		0040	Home procedure not executed
		0080	User macro running
		0100	Motion in progress
		0200	Single-axis prealigner servo off
		0400	Servo OFF on one or more axes
		0800	In teach mode (TCH/EOT)
		1000	In search mode (PGO)
		2000	I/O File error
		4000	In Teach Scan mode (TSC/EOT)
		8000	Prealigner Busy
*/
API_IMPORT BOOL uvEng_MDR_GetSystemStatus();

/* --------------------------------------------------------------------------------------------- */
/*                Command Reference  (4.Support for systems with absolute encoders)              */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : AE (Absolute Encoder) reset warning / position (cmd : _aer)
		This command clears state of AE (Absolute Encoders), and should be used with extreme care.
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		mode	- [in]  256 (Warning) or 1024 (Position) 
						_AER T 256  will clear "Warning" error on the absolute encoder (eg battery low condition),
									allowing for subsequent motion commands to proceed.
						_AER T 1024 should be used by service personel. This command clears AE position,
									thus invalidating robot calibration.
		password- [in]  "Password" to force an action (if password is set inside)
 retn : TRUE or FALSE
 exam : None
*/
API_IMPORT BOOL uvEng_MDR_SetAxisEncoderReset(ENG_RANC axis, ENG_RAEC mode, PCHAR password);
/*
 desc : Absolute Encoder Status (for Dual-Arm) (cmd : _aest)
 parm : None
 retn : TRUE or FALSE
 note : This command reports the status of absolute encoders on single or all axes.
		Possible values of the report are (INF 0, or INF 3):
		-1	_NA				Not applicable, axis is not with absolute encoder
		0	Healthy			Absolute Encoder status OK
		1	ErrABS			Battery was disconnected, encoder readings OK
		2	BatteryLOW		Battery low, encoder readings OK
		256	BatteryERROR	Battery low or missing, encoder readings lost
		-----------------------------------------------------------------------
		!!! Currently operating in INF 1 mode, this command cannot be used. !!!
		-----------------------------------------------------------------------
 exam : >_AEST
		0, 0, 0, _NA
		>INF_ 3
		>_AEST
		T=Healthy(2,4), R=Healthy(0,0), Z=Healthy(0,0), K=N.A.
		>
*/
API_IMPORT BOOL uvEng_MDR_GetEconderStatus();
/*
 desc : Set/Report Encoder Zero Offset for single axis (cmd : _eo)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  sets the encoder zero offset of Axis to Value (um or degree)
 retn : TRUE or FALSE
 note : Currently set encoder zero offsets of all the axes
		The encoder zero offset specifies the distance between the absolute encoder' zero position,
		and the origin of the axis coordinate system
 exam : >_EO
		31204,-28789,1358
		>_EO R
		Home offset is set to -15318
		>_EO
		-15
*/
API_IMPORT BOOL uvEng_MDR_SetEncoderZeroOffsetValue(ENG_RANC axis, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetEncoderZeroOffset();
/*
 desc : Set / report Encoder Zero Offset for all axis (cmd : _eor)
 parm : None
 retn : TRUE or FALSE
 note : Currently set encoder zero offsets of all the axes
 exam : >_EOR
		31204,-28789,1358
		>_EOR #
		>_EOR
		-15318,-28789,1358
		>
*/
API_IMPORT BOOL uvEng_MDR_SetEncoderZeroOffsetAll();
API_IMPORT BOOL uvEng_MDR_GetEncoderZeroOffsetAll();
/*
 desc : Report low-level encoder status for an Axis (cmd : _ralmc)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
 retn : TRUE or FALSE
 note : Low-level absolute encoder status.
		This command must be executed with axis servo loop turned off.
		This command has diagnostic purposes, and its result depends on the type of encoder being used.
 exam : >_RALMZ Z
		ALMC = 0x0000
		>
*/
API_IMPORT BOOL uvEng_MDR_GetLowLevelEncoderStatus(ENG_RANC axis);

/* --------------------------------------------------------------------------------------------- */
/*              Command Reference  (5.Basic (non-axes, non-station) action commands)             */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Set/Report Turn Backup DC Power On or Off (cmd : _bb)
 parm : option	- [in]  0 for OFF, 1 for ON
		value	- [in]  time-out value in seconds
						_BB 0	turns off backup power support.
								If the system had backup power enabled, and AC power is down,
								specifying _BB 0 will turn the system off immediately.
						_BB 1	Value turns on backup power support. If mains power goes down,
								the system will keep controller powered up through backup power supply
								for up to Value seconds, or indefinitely if no Value is specified.
								AC Power down condition is indicated through bit 12 (0x1000) of PER status mask.
								If this command is executed when system operates on backup power,
								timeout is restarted with specified value.
						_BB		reports current state of backup power support, and timeout, as follows:
 retn : TRUE or FALSE
 note : _BB 0 turns off backup power support.
			  If the system had backup power enabled, and AC power is down,
			  specifying _BB 0 will turn the system off immediately.
		_BB 1 Value turns on backup power support.
 exam : >STA
		0041
		>CPO
		1500,-3000,300
		>_BB
		0 <-- backup power operation disabled
		>_BB 1 60 <-- enable 60-seconds operation off backup power
		>_BB
		2,60 <-- mains power OK, will keep running for 60 seconds
		>PER
		0000
		>… … AC power goes down
		0453 <-- servo goes off, error reported
		?PER
		1000 <-- PER indicates there's no power
		>_BB <-- check remaining runtime
		1,57
		>_BB
		1,45
		>_BB 1 1500 <-- extend backup power operation to 1500 sec
		>_BB
		1,1495
		>CPO < position still valid
		1500,-3000,300
		>_BB 0 <-- conserve backup power
*/
API_IMPORT BOOL uvEng_MDR_SetBackupPowerCtrl(UINT8 option, INT32 value);
API_IMPORT BOOL uvEng_MDR_GetBackupPowerCtrl();
/*
 desc : Enable/Disable Controlled Stop Feature (cmd : cstp)
 parm : state	- [in]  optional numeric parameter, 0 for OFF, 1 for ON
		option	- [in]  optional numeric parameter - whether to report activation
						CSTP 1 1	enables the feature, and enables status prompt when the input is activated
						CSTP 0		disables the feature
						CSTP		reports current state of the feature
 retn : TRUE or FALSE
 note : Controlled Stop feature, when enabled, allows for motion command to be gracefully stopped
		if a digital input is activated. This feature must be properly configured in order to opreate.
		If the feature is not enabled, error prompt "?" will be reported.
		CSTP 1 1	enables the feature, and enables status prompt when the input is activated
		CSTP 0		disables the feature
 exam : >CSTP
		1,1
		>INF 1
		>GET A 1
		>1001 <- activation of control input
		?ESTA
		10
*/
API_IMPORT BOOL uvEng_MDR_SetControlledStopFeature(UINT8 state, UINT8 option);
API_IMPORT BOOL uvEng_MDR_GetControlledStopFeature();
/*
 desc : Read State of External Digital Input (cmd : din)
 parm : None
 retn : TRUE or FALSE
 note : This command returns the digital inputs states of an attached external I/O module, or Extended I/O interface, inputs.
 exam : None
*/
API_IMPORT BOOL uvEng_MDR_GetStateExtDI();
/*
 desc : Set/Report State of External Digital Output (cmd : dout)
 parm : output	- [in]  optional output number, range from 0 to 9
		value	- [in]  optional state of specified output
 note : This command sets output state of Extended I/O Interface module.
 exam : None
*/
API_IMPORT BOOL uvEng_MDR_SetStateExtDONo(UINT8 output, UINT8 value);
API_IMPORT BOOL uvEng_MDR_GetStateExtDO();
/*
 desc : Set/Report the state of an Input (cmd : inp)
 parm : input	- [in]  input number
 retn : TRUE or FALSE
 note : This command returns the digital inputs states.
		the states of inputs 0 to 8 in a hexadecimal format.
		If an input is not present (e.g. Vacuum Sensor 2 is not present), the value returned will be 0.
 exam : >INP 1
		1
		>INP
		0002
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStateInput(UINT8 input);
API_IMPORT BOOL uvEng_MDR_GetStateInput();
/*
 desc : Enable/Disable Loaded Speed Parameters (cmd : lenb)
 parm : option	- [in]  0: Disable, 1: Enable
 retn : TRUE or FALSE
 note : This command enables or disables using of loaded speed parameters, defined with LSPD, LACL, LDCL, LAJK, LDJK commands.
 exam : None
*/
API_IMPORT BOOL uvEng_MDR_SetLoadedSpeedParamEnable(UINT8 option);
API_IMPORT BOOL uvEng_MDR_GetLoadedSpeedParamEnable();
/*
 desc : Set/Report Output (cmd : out)
 parm : output	- [in]  Output Number
		value	- [in]  optional output state (0 or 1)
 retn : TRUE or FALSE
 note : This command sets or returns the digital outputs states.
		If an output is not physically present in a particular configuration,
		this command has no effect, and reading such an output’ state returns 0.
 exam : >OUT
		0003
		>OUT 0
		1
		>OUT 0 0
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStateOutput(UINT8 output, UINT8 value);
API_IMPORT BOOL uvEng_MDR_GetStateOutput();
/*
 desc : Turn Axis Servo or REE Power OFF (cmd : sof)
 parm : axis	- [in]  Axis Name (R1/R2 : Horizontal, Z : Vertical, T : Angle, A : All)
 retn : TRUE or FALSE
 note : This command turns off the servo control of the specified axis.
		If the axis is rac_a specified (ENG_RANC::rac_a), the command will turn off the servo of all the axes.
 exam : >STA
		100C
		>SOF
		>STA
		1401
		>SOF E
		>
*/
API_IMPORT BOOL uvEng_MDR_SetAxisServoTurnOff(ENG_RANC axis);
API_IMPORT BOOL uvEng_MDR_SetAxisServoTurnOn(ENG_RANC axis);
/*
 desc : Turn Vacuum OFF (cmd : vof)
 parm : effect_num	- [in]  0 (effector number 0) or 1 (effector number 1) or 2 (All)
 retn : TRUE or FALSE
 note : On systems configured for vacuum-paddle operation,
		this command turns vacuum OFF (closes vacuum valve) to all or specified vacuum paddle
 exam : >STA
		100C
		>VOF
		>STA
		1000
		>
*/
API_IMPORT BOOL uvEng_MDR_SetVacuumPaddleTurnOff(UINT8 effect_num);
API_IMPORT BOOL uvEng_MDR_SetVacuumPaddleTurnOn(UINT8 effect_num);
/*
 desc : Check for wafer presence on a paddle (cmd : vst)
 parm : number	- [in]  End effector number to use, 0 or 1
 retn : TRUE or FALSE
 note : On systems configured for vacuum-paddle operation, this command checks for wafer presence on specified vacuum paddle.
		If vacuum was turned off, it is turned ON in order for wafer presence to be detected by the vacuum sensor.
		If no wafer is detected, vacuum is turned OFF.
 exam : >STA
		1000
		>VST 0
		1
		>STA
		100C
		>
*/
API_IMPORT BOOL uvEng_MDR_GetCheckWaferInVacuumPaddle(UINT8 number);

/* --------------------------------------------------------------------------------------------- */
/*                      Command Reference  (6.Basic axis motions parameters)                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Set/Report Axis Customized Home Position (cmd : _ch)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  customized home position of Axis
 retn : TRUE or FALSE
 note : During homing, after the home position (T=0, R=0, Z=0) is found,
		the robot moves to the customized home position
 exam : >_CH
		0,0,0
		>_CH R -5000
		>_CH
		0,-5000,0
		>CPO
		0,0,500
		>_CH Z
		>_CH
		0,-5000,500
		>
*/
API_IMPORT BOOL uvEng_MDR_SetCustomizedHomePosition(ENG_RANC axis, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetCustomizedHomePosition();
/*
 desc : Set/Report Axis Current Limit (cmd : _cl)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  current limit of Axis
 retn : TRUE or FALSE
 note : This axis parameter limits the current to the axis (motor).
		It is very important for the motor performance.
 exam : >_CL
		201,201,201
		>_CL R 251
		>
*/
API_IMPORT BOOL uvEng_MDR_SetAxisCurrentLimitLevel(ENG_RANC axis, UINT8 value);
API_IMPORT BOOL uvEng_MDR_GetAxisCurrentLimitLevel();
/*
 desc : Set/Report Axis Position Error Limit (cmd : _el)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  Encoder Counts (position error limit of Axis)
 retn : TRUE or FALSE
 note : This axis parameter limits the position error.
		If the position error exceeds the limit, the controller will stop all the axes
 exam : >_EL
		200,100,200
		>_EL R 150
		>_EL
		200,150,200
		>
*/
API_IMPORT BOOL uvEng_MDR_SetAxisPositionErrorLimit(ENG_RANC axis, INT32 value);
API_IMPORT BOOL uvEng_MDR_GetAxisPositionErrorLimit();
/*
 desc : Set/Report Axis Forward or Reverse Safe Envelop (cmd : _fse or _rse)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		direct	- [in]  0x00: Forward, 0x01 : Reverse
		value	- [in]  Forward or Reverse safe envelope position for Axis.
 retn : TRUE or FALSE
 note : The firmware monitors the current position of all the axes.
		If safe envelop position have been defined, and axis position goes outside that limit,
		user-defined output is turned ON. If all axes are within their safe envelope,
		or no safe envelope is defined, that output is turned OFF.
 exam : >_FSE
		0,0,0
		>_FSE R 7200
		>MVA R 8000
		>
		>.OUT _OutputE
		1
		>

		>_RSE
		0,0,0
		>_RSE R -7200
		>MVA R -8000
		>
		>.OUT _OutputE
		1
		>
*/
API_IMPORT BOOL uvEng_MDR_SetAxisSafeEnvelop(ENG_RANC axis, ENG_MAMD direct, INT32 value);
API_IMPORT BOOL uvEng_MDR_GetAxisSafeEnvelop(ENG_MAMD direct);
/*
 desc : Set/Report Forward or Reverse Direction Software Limit (cmd : _fl or _rl)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		direct	- [in]  0x00: Forward, 0x01 : Reverse
		value	- [in]  Forward or Reverse direction software limit for Axis
 retn : TRUE or FALSE
 note : The firmware monitors the current position of all the axes.
		If an axis position exceeds its Forward or Reverse direction software limit,
		the firmware stops the motion of all the axes.
 exam : >_FL
		24000,10100,7500
		>_FL R 14000
		>_FL
		24000,14000,7500
		>CPO
		0,0,8200
		>_FL Z
		>_FL
		200000,10000,8200
		>
		>MVA R 14500
		>
		?CPO
		0,14050,0
		>

		>_RL
		-24000,-10000,-100
		>_RL R -14000
		>_RL
		-24000,-14000,-100
		>CPO
		0,-12500,0
		>_RL R
		>_RL
		-24000,-12500,-100
		>
*/
API_IMPORT BOOL uvEng_MDR_SetAxisSoftwareLimitValue(ENG_RANC axis, ENG_MAMD direct, INT32 value);
API_IMPORT BOOL uvEng_MDR_SetAxisSoftwareLimitCurrent(ENG_RANC axis, ENG_MAMD direct);
API_IMPORT BOOL uvEng_MDR_GetAxisSoftwareLimit(ENG_MAMD direct);
/*
 desc : Set/Report PID Loop Differential Coefficient for Axis Hold (cmd : _hd)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  Differential Coefficient of Axis
 retn : TRUE or FALSE
 note : This command can be used to specify KD (differential coefficient) PID parameter to be set
		to an axis when this axis is not moving. Setting this parameter can help in some special
		circumstances where e.g. heavy loaded axis requires different set of parameters to hold
		its position that when it is moving. If this command is not used, PID parameters are not changed.
		!!! Improper setting of this parameter can lead to robot servoing off during motion,
			or unable to stop properly on hitting an object |||
 exam : None
*/
API_IMPORT BOOL uvEng_MDR_SetDiffCoeffAxisHold(ENG_RANC axis, UINT32 value);
API_IMPORT BOOL uvEng_MDR_GetDiffCoeffAxisHold();
/*
 desc : Set/Report Position Error Limit for Axis Hold (cmd : _he)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  position error limit of axis
 retn : TRUE or FALSE
 note : This command can be used to specify position error to be set to an axis when this axis is not moving.
		Setting this parameter can help in some special circumstances where e.g. heavy loaded axis
		requires different set of parameters to hold its position that when it is moving.
		!!! Improper setting of this parameter can lead to robot servoing off during motion,
			or unable to stop properly on hitting an object !!!
 exam : None
*/
API_IMPORT BOOL uvEng_MDR_SetPosErrorLimitAxisHold(ENG_RANC axis, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetPosErrorLimitAxisHold();
/*
 desc : Set/Report PID Loop Proportional Coefficient for Axis Hold (cmd : _hp)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  proportional coefficient of Axis
 retn : TRUE or FALSE
 note : This command can be used to specify KP (Proportional coefficient) PID parameter to be set
		to an axis when this axis is not moving. Setting this parameter can help in some special
		circumstances where e.g. heavy loaded axis requires different set of parameters to hold its
		position that when it is moving. If this command is not used, PID parameters are not changed.
		!!! Improper setting of this parameter can lead to robot servoing off during motion,
			or unable to stop properly on hitting an object. !!!
 exam : None
*/
API_IMPORT BOOL uvEng_MDR_SetPropCoeffAxisHold(ENG_RANC axis, UINT32 value);
API_IMPORT BOOL uvEng_MDR_GetPropCoeffAxisHold();
/*
 desc : Set/Report Axis Home Offset (cmd : _ho)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  home offset of Axis (unit: angle, um)
 retn : TRUE or FALSE
 note : The home offset specifies the distance between the first index pulse from the home switch,
		or absolute encoder' zero position, and the origin of the axis coordinate system.
		!!! This command should not be used on axes with battery-backed absolute encoders.
			_EO command should be used instead. !!!
 exam : >_HO
		-124,358,208
		>CPO
		0,0,253
		>_HO Z
		Home offset is set to 253
		>_HO
		-124,358,253
		>
*/
API_IMPORT BOOL uvEng_MDR_SetAxisHomeOffsetValue(ENG_RANC axis, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetAxisHomeOffset();
/*
 desc : Set/Report PID Loop Integral Limit (cmd : _il)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  Integral limit limit of axis
 retn : TRUE or FALSE
 note : This parameter is part of the PID servo parameters and is very important for the axis behavior.
		It is factory preset to a tuned value and should be changed carefully
		!!! Improper setting of this parameter can lead to robot servoing off during motion,
			or unable to stop properly on hitting an object !!!
 exam : >_IL
		200,300,260
		>_IL R 280
		>_IL
		200,280,260
		>
*/
API_IMPORT BOOL uvEng_MDR_SetIntegralLimit(ENG_RANC axis, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetIntegralLimit();
/*
 desc : Set/Report PID Loop Differential Coefficient (cmd : _kd)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  Differential Coefficient of Axis
 note : This parameter is part of the PID servo parameters and is very important for the axis behavior.
		It is factory preset to a tuned value and should be changed carefully.
		!!! Improper setting of this parameter can lead to robot servoing off during motion,
			or unable to stop properly on hitting an object. !!!
 exam : >_KD
		1200,1500,2000
		>_KD R 1800
		>_KD
		1200,1800,2000
*/
API_IMPORT BOOL uvEng_MDR_SetDiffCoeffAxis(ENG_RANC axis, UINT32 value);
API_IMPORT BOOL uvEng_MDR_GetDiffCoeffAxis();
/*
 desc : Set/Report PID Loop Integral Coefficient (cmd : _ki)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  Differential Coefficient of Axis (Unit : ?)
 note : This parameter is part of the PID servo parameters and is very important for the axis behavior.
		It is factory preset to a tuned value and should be changed carefully.
		!!! Improper setting of this parameter can lead to robot servoing off during motion,
			or unable to stop properly on hitting an object. !!!
 exam : >_KI
		200,220,210
		>_KI R 230
		>_KI
		200,230,210
		>
*/
API_IMPORT BOOL uvEng_MDR_SetInteCoeffAxis(ENG_RANC axis, UINT32 value);
API_IMPORT BOOL uvEng_MDR_GetInteCoeffAxis();
/*
 desc : Set/Report PID Loop Proportional Coefficient (cmd : _kp)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  Differential Coefficient of Axis
 note : This parameter is part of the PID servo parameters and is very important for the axis behavior.
		It is factory preset to a tuned value and should be changed carefully.
		!!! Improper setting of this parameter can lead to robot servoing off during motion,
			or unable to stop properly on hitting an object. !!!
 exam : >_KP
		180,120,200
		>_KP R 150
		>_KP
		180,150,200
		>
*/
API_IMPORT BOOL uvEng_MDR_SetPropCoeffAxis(ENG_RANC axis, UINT32 value);
API_IMPORT BOOL uvEng_MDR_GetPropCoeffAxis();
/*
 desc : Set/Report Axis Acceleration or Deceleration (cmd : acl or dcl)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		type	- [in]  acceleration (0x00), deceleration (0x01)
		value	- [in]  acceleration/deceleration 값 (unit: um or degree)
 retn : TRUE or FALSE
 note : This command sets or returns the acceleration or deceleration of an axis.
		The values are saved in the motion parameters file by SMP command and may be restored by RMP command.
 exam : 
*/
API_IMPORT BOOL uvEng_MDR_SetAxisAcceleration(ENG_RANC axis, ENG_MAST type, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetAxisAcceleration(ENG_MAST type);
/*
 desc : Set/Report Axis Acceleration or Deceleration Jerk (cmd : ajk or djk)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		type	- [in]  acceleration (0x00), deceleration (0x01)
		value	- [in]  acceleration/deceleration 값
						Non-dimensional unit, ranging from 25 to 950, with 25 being weakest, and 950 strongest jerk setting
 retn : TRUE or FALSE
 note : The values are saved in the motion parameters file by SMP command and may be restored by RMP command
		Applicability of the Acceleration Jerk depends on the internal Velocity Profile Generator (VPG).
		The Acceleration Jerk is accounted if the VPG is set to ‘1’ (Bezier type),
		but is ignored if the VPG is set to zero (default, Spline type).
 exam : >AJK T 600
		 >AJK R 800
		>AJK Z 500
		>AJK
		600,800,500
		>

		>DJK T 600
		>DJK R 800
		>DJK Z 500
		>DJK
		600,800,500
		>
*/
API_IMPORT BOOL uvEng_MDR_SetAxisAccelerationJerk(ENG_RANC axis, ENG_MAST type, UINT16 value);
API_IMPORT BOOL uvEng_MDR_GetAxisAccelerationJerk(ENG_MAST type);
/*
 desc : Set/Report Loaded Acceleration or deceleration (cmd : lacl or ldcl)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		type	- [in]  acceleration (0x00), deceleration (0x01)
		value	- [in]  acceleration/deceleration 값 (unit: um or degree)
 retn : TRUE or FALSE
 note : This command sets or reports the acceleration of an axis when loaded with a wafer.
		When enabled with LENB command, after wafer is picked up on the arm during pick commands (GET, MSS, etc),
		axis' acceleration is set to value specifued with LACL command.
		The value is saved in the robot motion parameters file by SMP command and may be restored by RMP command.
 exam : >ACL
		4500,9000,2500
		>LENB 1
		>LACL R 6000
		>GET A 1
		>STA
		100C
		>ACL
		4500,6000,2500
		>PUT A 1
		>STA
		1000
		>ACL
		4500,9000,2500
		>

		>ACL
		4500,9000,2500
		>LENB 1
		>LACL R 6000
		>LDCL R 4500
		>GET A 1
		>STA
		100C
		>DCL
		4500,4500,2500
		>PUT A 1
		>STA
		1000
*/
API_IMPORT BOOL uvEng_MDR_SetAxisLoadedAcceleration(ENG_RANC axis, ENG_MAST type, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetAxisLoadedAcceleration(ENG_MAST type);
/*
 desc : Set/Report Axis Loaded Acceleration or deceleration Jerk (cmd : lajk or ldjk)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		type	- [in]  acceleration (0x00), deceleration (0x01)
		value	- [in]  acceleration/deceleration 값
						Non-dimensional unit, ranging from 25 to 950, with 25 being weakest, and 950 strongest jerk setting
 retn : TRUE or FALSE
 note : This command sets or reports the acceleration jerk of an axis when loaded with a wafer.
		When enabled with LENB command, after wafer is picked up on the arm during pick commands (GET, MSS, etc),
		axis' acceleration jerk is set to value specifued with LSPD.
		The values are saved in the motion parameters file by SMP command and may be restored by RMP command
 exam : None
*/
API_IMPORT BOOL uvEng_MDR_SetAxisLoadedAccelerationJerk(ENG_RANC axis, ENG_MAST type, UINT16 value);
API_IMPORT BOOL uvEng_MDR_GetAxisLoadedAccelerationJerk(ENG_MAST type);
/*
 desc : Set/Report Loaded Speed (cmd : lspd)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  speed of Axis when loaded with a wafer (unit: um or degree)
 retn : TRUE or FALSE
 note : This command sets or returns the speed of an axis when loaded with a wafer.
		When enabled with LENB command, after wafer is picked up on the arm during pick commands (GET, MSS, etc),
		axis' speed is set to value specifued with LSPD command.
		The value is saved in the robot motion parameters file by SMP command and may be restored by RMP command.
 exam : >SPD
		2000,3000,1000
		>LENB 1
		>LSPD R 1200
		>GET A 1
		>STA
		100C
		>SPD
		2000,1200,1000
		>PUT A 1
		>STA
		1000
		>SPD
		2000,3000,1000
		>
*/
API_IMPORT BOOL uvEng_MDR_SetAxisLoadedSpeed(ENG_RANC axis, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetAxisLoadedSpeed();
/*
 desc : Set/Report Working Speed (cmd : spd)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  speed of Axis (unit: um or degree)
 retn : TRUE or FALSE
 note : This command sets or returns the current speed of an axis.
		The value is saved in the robot motion parameters file by SMP command and may be restored by RMP command.
 exam : >SPD T 2000
		>SPD R 3000
		>SPD Z 1000
		>SPD
		2000,3000,1000
		>
*/
API_IMPORT BOOL uvEng_MDR_SetAxisWorkingSpeed(ENG_RANC axis, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetAxisWorkingSpeed();

/* --------------------------------------------------------------------------------------------- */
/*           Command Reference  (7.Axis parameter sets, and custom parameters commands)          */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Set/Report Custom Axis parameter (cmd : _cax)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		param	- [in]  mandatory parameter number between 0 and 9
		value	- [in]  optional value.
 retn : TRUE or FALSE
 note : This command sets or reports the value of up to ten custom axis parameters.
		These parameters are not used by robot firmware, but can be used by custom user commands
		to store and retrieve parameters necessary for the custom application.
		The values set with custom parameters are saved with _CSV command, and restored with _CLD command,
		into a file named custom.ini. A configuration parameter allows for these values to be
		saved / restored with standard SMP/RMP, SSP/RSP commands, and kept with standard parameter files.
 exam : >_CAX T 0 100
		>_CSV
		>
*/
API_IMPORT BOOL uvEng_MDR_SetCustomAxisParam(ENG_RANC axis, UINT8 param, INT32 value);
API_IMPORT BOOL uvEng_MDR_GetCustomAxisParam(ENG_RANC axis, UINT8 param);
/*
 desc : Set/Report Custom Generic parameter (cmd : _cpa)
 parm : param	- [in]  mandatory parameter number between 0 and 19
		value	- [in]  optional value
 retn : TRUE or FALSE
 note : This command sets or reports the value of up to twenty custom generic parameters.
		These parameters are not used by robot firmware, but can be used by custom user commands
		to store and retrieve parameters necessary for the custom application.
 exam : >_CPA 0 10
		>_CSV
		>
*/
API_IMPORT BOOL uvEng_MDR_SetCustomGenericParam(UINT8 param, INT32 value);
API_IMPORT BOOL uvEng_MDR_GetCustomGenericParam(UINT8 param);
/*
 desc : Set/Report Custom Station Parameter (cmd : _cst)
 parm : station	- [in]  Station Name (문자 1)
		param	- [in]  mandatory parameter number between 0 and 4
		value	- [in]  optional value.
 retn : TRUE or FALSE
 note : This command sets or reports the value of up to five custom statioin parameters.
		These parameters are not used by robot firmware, but can be used by custom user commands
		to store and retrieve parameters necessary for the custom application.
 exam : >_CAX T 0 100
		>_CSV
		>
*/
API_IMPORT BOOL uvEng_MDR_SetCustomStationParam(CHAR station, UINT8 param, INT32 value);
API_IMPORT BOOL uvEng_MDR_GetCustomStationParam(CHAR station, UINT8 param);
/*
 desc : Report All Parameter Sets for a Single Axis (cmd : das)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
 retn : TRUE or FALSE
 note : This command reports a table for all sets and all parameters for a given axis.
		If a particular parameter set is invalid, it is marked as such,
		and a note is displayed at the end of the table. See examples for details.
		!!! The measuring units for the speed, acceleration and deceleration in the DAS commands report are as follow:
			- Speed: 0.01°/sec for rotary axes (T) and 0.001"/sec for linear axes (R, K, Z)
			- Acceleration and deceleration: 0.01°/sec2 for rotary axes (T) and 0.001"/sec2 for linear axes (R, K, Z).
 exam : None
*/
API_IMPORT BOOL uvEng_MDR_GetAllParamSetsSingleAxis(ENG_RANC axis);
/*
 desc : Report a Parameter Set (cmd : dps)
 parm : value	- [in]  parameter set number (0 ~ 9)
 retn : TRUE or FALSE
 note : This command reports a table for all axes and all parameters for a given parameter set.
		If this parameter set is invalid, a note is displayed at the end of the table.
		See examples for details.
		!!! The measuring units for the speed, acceleration and deceleration in the DPS commands report are as follow:
			- Speed: 0.01°/sec for rotary axes (T) and 0.001"/sec for linear axes (R, K, Z);
			- Acceleration and deceleration: 0.01°/sec2 for rotary axes (T) and 0.001"/sec2 for linear axes (R, K, Z).
 exam : None
*/
API_IMPORT BOOL uvEng_MDR_GetParameterSetInfo(UINT8 value);
/*
 desc : Set Homing Speed and Acceleration Parameters (cmd : hsa)
 parm : None
 retn : TRUE or FALSE
 note : This command sets the homing motion parameters
		(speed, acceleration, deceleration, acceleration jerk, and deceleration jerk) to all the axes.
 exam : >SPD
		1800,4000,1300
		>HSA
		>SPD
		100,100,50
		>
*/
API_IMPORT BOOL uvEng_MDR_SetHomingAllAxisMotionParam();
/*
 desc : Set Loaded Speed and Acceleration Parameters (cmd : lsa)
 parm : None
 retn : TRUE or FALSE
 note : This command sets the loaded motion parameters
		(speed, acceleration, deceleration, acceleration jerk, and deceleration jerk),
		as defined with LSPD/LACL/LDCL commands. Only non-zero parameters are loaded
 exam : >SPD
		2000,3000,1000
		>LSP
		>SPD
		2000,1200,1000
		>WSA
		>SPD
		2000,3000,1000
		>
*/
API_IMPORT BOOL uvEng_MDR_SetLoadedAllAxisMotionParam();
/*
 desc : Set Axes Working Speed and Acceleration Parameters (cmd : wsa)
 parm : None
 retn : TRUE or FALSE
 note : This command sets the working trajectory parameters
		(speed, acceleration, deceleration, acceleration jerk, and deceleration jerk) to all the axes.
		This command cannod be executed during homing process
 exam : >SPD
		100,100,40
		>WSA
		>SPD
		2000,3200,1800
		>
*/
API_IMPORT BOOL uvEng_MDR_SetWorkingAllAxisMotionParam();

/* --------------------------------------------------------------------------------------------- */
/*                       Command Reference  (8.Basic axis motions commands)                      */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Absolute Encoder - Report distance from home switch (cmd _afe or afe)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
						Not used ENG_RANC::rac_a
 retn : TRUE or FALSE
 note : Distance between the home switch and the absolute encoder' zero position
		This command starts a procedure, which measures the distance between the home switch
		and the absolute encoder' zero position in the last revolution.
		The measured distance is returned in encoder counts.
 exam : >_AFE T
		Home Switch to AEzero distance: 14572 encoder counts
		>

		>AFE T
		Home Switch to AEzero distance: 14572 encoder counts
		>
*/
API_IMPORT BOOL uvEng_MDR_GetDistanceFromHomeSwitch(ENG_RANC axis);
API_IMPORT BOOL uvEng_MDR_GetDistanceFromHomeSwitchEx(ENG_RANC axis);
/*
 desc : Report Home Switch to Index distance (cmd : _fe)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
 retn : TRUE or FALSE
 note : Distance between the home switch and the first index pulse.
		This command starts a procedure, which measures the distance between the home switch and
		the first index pulse from the encoder.
		The measured distance is returned in both user units and encoder counts.
		Axis units, and Encoder counts 값으로 반환
		ex> >_FE T
			Home Switch to index distance: 546 (4572 ec)
			>
 exam : >_FE T
		Home Switch to index distance: 546 (4572 ec)
		>
*/
API_IMPORT BOOL uvEng_MDR_SetFindEdgeAxis(ENG_RANC axis);
/*
 desc : Find Home Switch (cmd : _fh)
 parm : axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
 retn : TRUE or FALSE
 note : This command starts a procedure which moves specified axis over position where home switch triggers
 exam : >_FH T
		>
*/
API_IMPORT BOOL uvEng_MDR_SetFindHomeSwitch(ENG_RANC axis);
/*
 desc : Start Homing Procedure for the Robot (cmd : hom)
 parm : option	- [in]  1 ~ 3 or -1
 retn : TRUE or FALSE
 note : This command starts the homing procedure for the robot.
		This procedure executed differently, depending on motor' encoder types, and Option parameter.
		HOM 0  is the standard homing procedure for motors with incremental and absolute encoders. Robot paddle(s) are checked for wafer presence, and if clear, homing procedure is executed, followed by moving of all axes to their custom home positions.
		HOM 1  is the standard homing procedure for motors with incremental and absolute encoders. With this option, wafer presence check is not performed.
		HOM 2  can be used on robot with battery-backed absolute encoders. Robot paddle(s) are checked for wafer presence, and if clear, robot is-homed state is set, without additional motions.
		HOM 3  can be used on robot with battery-backed absolute encoders. Wafer presense check is not performed, and robot is-homed state is set, without additional motions.
		HOM -1 can be used on robot with battery-backed absolute encoders to recover zero position when absolute encoders have lost their position.
 exam : >STA
		104D
		>HOM
		?
		>HOM 1
		>
*/
API_IMPORT BOOL uvEng_MDR_SetHomeMethod(INT8 option);
/*
 desc : Home an Axis (Homing Start) (cmd : _hm)
 parm : axis	- [in]  Axis Name (R1/R2 : Horizontal, Z : Vertical, T : Angle, K : N/A, A : All)
 retn : TRUE or FALSE
 note : This command starts the homing procedure for the specified axis or for all axes.
		The procedure stops once the first index after the home switch is reached.
		The current position of the axis is reset to 0.
		Note that this procedure does not apply the home offset, or custom home offset of the axis,
		and system homing state is cleared.

		_HM Axis starts homing of the specified Axis
		_HM starts homing of all axes
 exam : >_HM Z
		>_HM
		>
*/
API_IMPORT BOOL uvEng_MDR_StartHomeAxis(ENG_RANC axis);
/*
 desc : Move to Home Position (cmd : mth)
 parm : axis	- [in]  Axis Name (R1/R2 : Horizontal, Z : Vertical, T : Angle, K : N/A, A : All)
 retn : TRUE or FALSE
 note : This command starts moving the specified axis or all axes to their home positions
 exam : >MTH R
		>MTH
		>
*/
API_IMPORT BOOL uvEng_MDR_MoveHomePosition(ENG_RANC axis=ENG_RANC::rac_a);
/*
 desc : Move Four Axes to Position (cmd : mtp)
 parm : axis_t	- [in]  Rotation Axis (T) (unit: degree)
		axis_r1	- [in]  Primary Axis (R1) (unit: um)
		axis_r2	- [in]  Secondary Axis (R2) (unit: um)
		axis_z	- [in]  Vertical Axis (Z) (unit: um)
		retract	- [in]  optional R1, R2 axes positions. If track is not installed, fifth parameter is taken instead
 retn : TRUE or FALSE
 note : This command starts moving all three axes to the specified positions simultaneously.
		!!! If an RetractR is specified, both arms are first retracted to this position. !!!
 exam : >CPO
		1250,-4500,1275
		>MTP 3000 0 1000
		>CPO
		3000,0,1000
		>
*/
API_IMPORT BOOL uvEng_MDR_MoveAxisAll(DOUBLE axis_t, DOUBLE axis_r1, DOUBLE axis_r2, DOUBLE axis_z);
API_IMPORT BOOL uvEng_MDR_MoveAxisExtAll(DOUBLE axis_t, DOUBLE axis_r1, DOUBLE axis_r2, DOUBLE axis_z,
										 DOUBLE retract);
/*
 desc : Move arm to Station Retracted Position (cmd : mtr)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		slot	- [in]  Slot Number (1 ~ 25?)
		option	- [in]  Vertical Position 지정 값 (0, 1, 2)
						0x00: veritcal position to leave the slot (SPO + OFS), default (if no specified)
						0x01: veritcal position to enter the slot (SPO + OFS - STR3) -> Manual 참고
						0x02: vertical position at slot position for current slot (SPO)
 retn : TRUE or FALSE
 note : moves the robot to retracted position of specified station and slot.
		Option parameter specifies where within the slot to position the arm
 exam : >SPO A
		5000,10950,1350
		>RPO A
		-6500
		>OFS A
		30
		>MTR A 1
		>CPO
		5000,-6500,1380
		>
*/
API_IMPORT BOOL uvEng_MDR_MoveAxisRetractedPosition(CHAR station, UINT8 slot, UINT8 option);
/*
 desc : Move Axis to Absolute Position (cmd : mva)
 parm : axis	- [in]  Axis Name (R1/R2 : Horizontal, Z : Vertical, T : Angle, K : N/A, A : All)
		position- [in]  Target position. (unit: degree or um)
 retn : TRUE or FALSE
 note : This command starts moving the Axis to the specified target Position defined as an absolute coordinate of the axis.
		!!! It is not recommended to use MVA command before successful execution of HOM command !!!
		!!! If target position is beyond software limits, motion will be limited to 50 units beyond the software limit. !!!
 exam : >CPO R
		2000
		>MVA R 6000
		>CPO R
		6000
		>
*/
API_IMPORT BOOL uvEng_MDR_MoveAbsolutePosition(ENG_RANC axis, DOUBLE position);
/*
 desc : Move Axis Relative (cmd : mvr)
 parm : axis	- [in]  Axis Name (R1/R2 : Horizontal, Z : Vertical, T : Angle, K : N/A, A : All)
		distance- [in]  Relative position. (unit: degree or um)
 retn : TRUE or FALSE
 note : This command starts moving the Axis to the specified target position defined as coordinate
		relative to the current position of the axis
 exam : >CPO R
		2000
		>MVR R 3000
		>CPO R
		5000
		>
*/
API_IMPORT BOOL uvEng_MDR_MoveRelativePosition(ENG_RANC axis, DOUBLE distance);
/*
 desc : Move Over Straight Line (cmd : mvt1)
 parm : pos_t	- [in]  Final Theta position (degree)
		pos_r	- [in]  Final Arm position (um)
 retn : TRUE or FALSE
 exam : >CPO
		-4500,4000,350
		>MVT1 4500 4000
		…0000
		>CPO
		45
*/
API_IMPORT BOOL uvEng_MDR_MoveOverStraightLine(DOUBLE pos_t, DOUBLE pos_r);
/*
 desc : Move Over Two Straight Lines (cmd : mvt2)
 parm : pos_t1	- [in]  First Theta position (degree)
		pos_r1	- [in]  First Arm position (um)
		pos_t2	- [in]  Final Theta position (degree)
		pos_r2	- [in]  Final Arm position (um)
 retn : TRUE or FALSE
 exam : >CPO
		-4500,-4000,350
		>MVT2 -4500 4000 -3200 6000 P
		…0000
		>CPO
		-3200,6000,350
		>MVT2 –4500 4000 -4500 -4000 G
		…0000
		>
*/
API_IMPORT BOOL uvEng_MDR_MoveOverTwoStraightLine(DOUBLE pos_t1, DOUBLE pos_r1, DOUBLE pos_t2, DOUBLE pos_r2);
/*
 desc : SSet/Report EE Length and Radius for Station-Less Moves (cmd : mvtp)
 parm : ee_len	- [in]  Length of End Effector (um)
		radius	- [in]  Cornering radius to use with MVT2 command (degree)
 retn : TRUE or FALSE
 note :	This commands sets the length of end effector, and cornering radius,
		to be used with station-less in-line motion commands MOV, MVT1 and MVT2.
 exam : >MVTP
		8000,4000
		>
*/
API_IMPORT BOOL uvEng_MDR_SetEELenRadiusForStationLessMove(DOUBLE ee_len, DOUBLE radius);
/*
 desc : Stop Motions and Tasks (cmd : stp)
 parm : axis	- [in]  Axis Name (R1/R2 : Horizontal, Z : Vertical, T : Angle, K : N/A, A : All)
 retn : TRUE or FALSE
 exam : >HSA
		>GET A 1
		>STP
		>WSA
		>GET A 1
		>
*/
API_IMPORT BOOL uvEng_MDR_StopMotionTask(ENG_RANC axis);
/*
 desc : Wait for Motion Completed (cmd : wmc)
 parm : None
 retn : TRUE or FALSE
 note : waits until all axes stop moving, and all submitted macros finish their execution
 exam : >GET a 1
		>WMC
		………
		>
*/
API_IMPORT BOOL uvEng_MDR_SetWaitForMotionCompleted();

/* --------------------------------------------------------------------------------------------- */
/*                       Command Reference  (9.Station parameters commands)                      */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Set/Report Current Station Set Number (cmd : _css)
 parm : value	- [in]  Station Set Number (1 ~ 4)
 retn : TRUE or FALSE
 note : This command (when enabled through EnableSets=1 line in stations.ini file) allows for defining
		and using of up to 130 stations (instead of 52)
 exam : >_CSS
		1
		>ENS
		ABCPXabcdp
		>SPO a
		6500,7000,350
		>SPO b
		-6500,6500,1250
		>_CSS 2
		>ENS
		ABCPXab
		>SPO a
		12500,4350,120
		>SPO b
		3000,10500,250
		>_CSS 3
		>ENS
		ABCPXac
		>SPO a
		-3500,5600,12350
		>SPO b
		?_CSS 4
		>ens
		ABCPXadp
		>
*/
API_IMPORT BOOL uvEng_MDR_SetCurrentStationSet(UINT8 value);
API_IMPORT BOOL uvEng_MDR_GetCurrentStationSet();
/*
 desc : Set/Report R Axis Pushing Acceleration (cmd : _pa)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  acceleration value (unit: um/sec^2)
 retn : TRUE or FALSE
 note : This command defines the pushing acceleration of R axis when performing pick (GET, GETW, GETC etc)
		operation from a station with pushing enabled
 exam : >PSH a
		0
		>_PA a 100
		?
		>PSH a 1
		>_PA a 1200
		>
*/
API_IMPORT BOOL uvEng_MDR_SetRAxisPusingAcceleration(CHAR station, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetRAxisPusingAcceleration(CHAR station);
API_IMPORT BOOL uvEng_MDR_GetRAxisPusingAccelerationDefault();
/*
 desc : Set/Report R Axis Pushing Speed (cmd : _ps)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  speed value (unit: um/sec)
 retn : TRUE or FALSE
 note : This command defines the pushing speed of R axis when performing GET operation from a station with pushing enabled
 exam : >PSH a
		0
		>_PS a 100
		?
		>PSH a 1
		>_PS a 100
		>
*/
API_IMPORT BOOL uvEng_MDR_SetRAxisPusingSpeed(CHAR station, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetRAxisPusingSpeed(CHAR station);
API_IMPORT BOOL uvEng_MDR_GetRAxisPusingSpeedDefault();
/*
 desc : Set/Report Station Coordinate Compensation with Slot (cmd : _zc)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  rotary axes speed value (unit: degree)
 retn : TRUE or FALSE
 note : This command can be useful in performing pick and place operation on stations
		which are not perfectly aligned vertically with regard to Z (vertical) axis.
		This command specifies the offset of the last slot from the first slot of that station,
		and compensates for T, R, and K coordinates proportionally to that slot.
 exam : >SPO a
		9000,10850,500
		>_ZC a
		0,0
		>MTS a 1
		>CPO
		9000,10850,500
		>MTS a 10
		>CPO
		9000,10850,2750
		>MTS a 25
		>CPO
		9000,10850,6500
		>_ZC a t 1000
		>_ZC a r 500
		>_ZC a
		1000,500
		>MTS a 1
		>CPO
		9000,10850,500
		>MTS a 10
		>CPO
		9375,11037,2750			9375 = 9000 + 1000 * (10 - 1)/(25 - 1)
								11037 = 10850 + 500 * (10 - 1)/(25 - 1)
		>MTS a 25
		>CPO
		10000,11350,6500		10000 = 9000 + 1000 * (25 - 1)/(25 - 1)
								11350 = 10850 + 500 * (25 - 1)/(25 - 1)
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationCoordCompensationSlot(CHAR station, ENG_RANC axis, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetStationCoordCompensationSlot(CHAR station);
/*
 desc : Report Inline Station' Approach position (cmd : app)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
 retn : TRUE or FALSE
 note : This command reports various parameters and calculated positions for stations defined as in-line.
		This information can be useful for setting-up and fine-tuning inline station parameters.
 exam : >IST A
		2
		>APP A
		Pickup T = 5000, R = 10950, IST = 1, EE=7500
		Approach T = 4000, R = 9000
		IRT = 4000 , ISA = 18000, ISD = 8000, ISE=7500, EEN=2
		>
*/
API_IMPORT BOOL uvEng_MDR_GetInlineStationApproachPosition(CHAR station);
/*
 desc : Set/Report Station Center Offset (cmd : cof)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  center offset parameter for Station (unit: degree)
 retn : TRUE or FALSE
 note : This command sets or returns the center offset parameter for a station.
		This offset is used to correct station’ linear position during GET and PUT operations,
		to compensate for e.g. different carrier, For radial station, this offset is added to R station coordinate.
		For in-line stations, this offset is added along station’ exit line.
 exam : >COF A
		30
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationCenterOffset(CHAR station, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetStationCenterOffset(CHAR station);
/*
 desc : Set/Report Station Center Offset for special operations (cmd : cofp)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  center offset parameter for Station (unit: degree)
 retn : TRUE or FALSE
 note : This command sets or returns the center offset parameter for a station when using edge gripper.
		This offset is used to correct station’ linear position during GET and PUT operations,
		to compensate for e.g. different carrier, For radial station, this offset is added to R station coordinate.
		For in-line stations, this offset is added along station’ exit line.
 exam : >COFP A
		-50
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationCenterOffsetEdgeGrip(CHAR station, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetStationCenterOffsetEdgeGrip(CHAR station);
/*
 desc : Set/Report Current Slot for a Station (cmd : csl)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Slot Number
 retn : TRUE or FALSE
 note : This command sets or returns the current slot of a station.
		The current slot is used when no slot is specified. For example GET without parameters will
		get a wafer from the current slot of the current station.
 exam : >CST
		A
		>CSL
		10
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationSlotNo(CHAR station, UINT8 value);
API_IMPORT BOOL uvEng_MDR_GetStationSlotNo(CHAR station);
/*
 desc : Clear Station Parameters (cmd : csp)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
 retn : TRUE or FALSE
 note : This command clears all or specified stations and their parameters from the controller RAM.
		This usually is done when teaching new stations, or to remove definition for obsolete station.
 exam : >NST
		6
		>NSL a
		25
		>CSP a
		>NST
		5
		>NSL a
		?
		>CSP #
		>NST
		0
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationParamClear(CHAR station);
API_IMPORT BOOL uvEng_MDR_SetStationParamClearAll();
/*
 desc : Set/Report Current Station, by name (cmd : cst)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
 retn : TRUE or FALSE
 note : This command sets or returns the current station. The current station is used when no station is specified.
		For example GET without parameters will get a wafer from the current slot of the current station.
 exam : >CST
		A
		>
*/
API_IMPORT BOOL uvEng_MDR_SetCurrentStationName(CHAR station);
API_IMPORT BOOL uvEng_MDR_GetCurrentStationName();
/*
 desc : Set/Report Current Station, by number (cmd : cstn)
 parm : value	- [in]  Station Number (1 ~ 130)
						  1 ~  26 : set_no_1,
						 27 ~  52 : set_no_2,
						 53 ~  78 : set_no_3,
						 79 ~ 104 : set_no_4,
						105 ~ 130 : set_no_5
 retn : TRUE or FALSE
 note : When station sets feature is enabled, this command sets or returns the current station by number.
		The current station is used when no station is specified.
		For example GET without parameters will get a wafer from the current slot of the current station.
 exam : >_CSS
		1
		>CST
		a
		>CSTN
		27
		>CSTN 105
		>_CSS
		4
		>CST
		a
		>
*/
API_IMPORT BOOL uvEng_MDR_SetCurrentStationNo(UINT8 value);
API_IMPORT BOOL uvEng_MDR_GetCurrentStationNo();
/*
 desc : Duplicate Station Parameters, by name (cmd : dsp)
 parm : source	- [in]  원본 Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		target	- [in]  대상 Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
 retn : TRUE or FALSE
 note : This command copies all parameters pertaining to a station
		(e.g. number of slots, pitch, pick-up position, scanning positions, vacuum time-outs etc)
		from station named Station1 to station named Station2.
		Both stations must exist in order for this command to succeed.
		All parameters for Station2 get overwritten.
*/
API_IMPORT BOOL uvEng_MDR_SetDuplicateStationParamByName(CHAR source, CHAR target);
/*
 desc : Duplicate Station Parameters, by number (cmd : dspn)
 parm : source	- [in]  source station number (1 ~ 130)
		target	- [in]  target station number (1 ~ 130)
 retn : TRUE or FALSE
 note : This command copies all parameters pertaining to a station
		(e.g. number of slots, pitch, pick-up position, scanning positions, vacuum time-outs etc)
		from station Number1 to station Number2.
		Both stations must exist in order for this command to succeed.
		All parameters for Station 2 get overwritten.
 exam : >SPON 27
		13500,7200,1100
		>NSL a
		25
		>PIT a
		2500
		>TCH b 1
		>EOT
		>SPON 28
		0,0,0
		>DSPN 27 28
		>SPON 28
		13500,7200,1100
		>
*/
API_IMPORT BOOL uvEng_MDR_SetDuplicateStationParamByNumber(UINT8 source, UINT8 target);
/*
 desc : Set/Report End Effector Number in Use for a Station (cmd : een)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		number	- [in]  0 – use Vacuum Valve 1 and Vacuum Sensor 1 when servicing a station (default)
						1 – use Vacuum Valve 2 and Vacuum Sensor 2 when servicing a station.
							The robot has to be equipped with two vacuum lines in order for this option to be valid.
 retn : TRUE or FALSE
 note : This command sets the end effector in use when servicing a station
 exam : >EEN A 0
		>EEN a 1
		>
*/
API_IMPORT BOOL uvEng_MDR_SetEndEffectorNumForStation(CHAR station, UINT8 number);
API_IMPORT BOOL uvEng_MDR_GetEndEffectorNumForStation(CHAR station);
/*
 desc : Station 동작 옵션에 End Effector가 Wafer를 특정 위치에 GET or PUT 동작 할 때, Z 축을 Up or Down 하기 위해 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		up_down	- [in]  0 – Service a station with End Effector in Upper position (default).
						1 – Servicing a station with End Effector in Down Position
							End Effector is below the wafer when entering (GET) or leaving (PUT) a station during pick and place operations.
 retn : TRUE or FALSE
 note : For robots with rotating end effector option, this command sets the Rotating End effector Orientation when servicing a station.
 exam : >EEO A 0
		>EEO P 1
		>
*/
API_IMPORT BOOL uvEng_MDR_SetEndEffectorUpDownForStation(CHAR station, UINT8 up_down);
API_IMPORT BOOL uvEng_MDR_GetEndEffectorUpDownForStation(CHAR station);
/*
 desc : Enumerate Defined Stations (cmd : ens)
 parm : set	- [in]  Station Set Number (1 ~ 4)
 retn : TRUE or FALSE
 note : List of all defined station names, or their numbers, in specified or all station sets
		This command returns a list of all station names currently defined on the robot.
		If station sets are enabled, this command returns stations available in current or specified station set.
		If letter N is used as a parameter, all stations in all sets are listed as numbers.
		Station numbers can be used in GETN, PUTN and MSSN commands.
 exam : (station sets disabled)
		>NST
		10
		>ENS
		PABMCabpxy
		>
		(station sets enabled)
		>NST
		17
		>_CSS
		1
		>ENS
		ABCPXabcdp
		>ENS 0
		ABCPX,abcdp,ab,ac,adp
		>ENS 2
		ABCPXab
		>ENS N
		0:1,2,3,16,24
		1:27,28,29,30,42
		2:53,54
		3:79,81
		4:105,108,120
		>
*/
API_IMPORT BOOL uvEng_MDR_GetEnumerateDefinedStations();
/*
 desc : Enumerate Defined Stations (cmd : ens 1 or 2 or 3 or 4)
 note : reports names of defined station is specified set 1 thru 4.
 exam : 
*/
API_IMPORT BOOL uvEng_MDR_GetEnumerateDefinedStationsNum(UINT8 set);
/*
 desc : Enumerate Defined Stations (cmd : ens 0)
 note : reports names of defined station is all sets. A comma separates station sets.
 exam : 
*/
API_IMPORT BOOL uvEng_MDR_GetEnumerateDefinedStationsAll();
/*
 desc : Enumerate Defined Stations (cmd : ens N)
 note : report numbers assigned to all stations in all sets.
		Set 0 refers to all upper-case stations, sets 1 though 4 refers to lower-case stations in sets 1, 2, 3 and 4.
*/
API_IMPORT BOOL uvEng_MDR_GetEnumerateDefinedStationsAllNum();
/*
 desc : Set/Report Inline Station Last Segment Speed Factor (cmd : ils)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  speed factor from 0 to 100
 retn : TRUE or FALSE
 note : This command sets the speed factor to be applied to current speed parameter when entering last part of station.
		Setting this parameter to values below 100 might help in smoother motion during placing the wafer into cassette.
		Setting this parameter to 0 has same effect as setting it to 100.
 exam : >IST A
		2
		>ILS A 40
		>
*/
API_IMPORT BOOL uvEng_MDR_SetInlineStationLastSeqmentSpeedFactor(CHAR station, UINT8 value);
API_IMPORT BOOL uvEng_MDR_GetInlineStationLastSeqmentSpeedFactor(CHAR station);
/*
 desc : Set/Report Inline Station Retracted R Position (cmd : irr)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  R coordinate (unit: um)
 retn : TRUE or FALSE
 note : This command sets the retracted position for R axis when exiting Linear-out station during GET operation,
		or starting position for Linear-In station during PUT operation.
		R position does not have to be the same as position defined with RPO command.
		Radial motions starts from the position defined by RPO command.
 exam : >IRR A -4000
		>IRR B -4000
		>
*/
API_IMPORT BOOL uvEng_MDR_SetInlineStationRetractedRPosition(CHAR station, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetInlineStationRetractedRPosition(CHAR station);
/*
 desc : Set/Report Inline Station Retracted T Position (cmd : irt)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  T coordinate (unit: degree)
 retn : TRUE or FALSE
 note : This command sets the retracted position for T axis when exiting Linear-out station during GET operation,
		or starting position for Linear-In station during PUT operation.
		Radial motions starts from the position in front of the station, that is, T coordinate of the station.
 exam : >IRT A 0
		>IRT B 0
		>
*/
API_IMPORT BOOL uvEng_MDR_SetInlineStationRetractedTPosition(CHAR station, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetInlineStationRetractedTPosition(CHAR station);
/*
 desc : Set/Report Inline Station Exit Angle (cmd : isa)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		angle	- [in]  angle (unit: degree)
 retn : TRUE or FALSE
 note : This command sets the angle between the T=0 line and station line.
		This is the direction the end effector has to follow when exiting a linear-our station during GET operation,
		or when entering a linear-in station during PUT operation.
		IST Station reports service type for station Station.
 exam : >ISA A 18000
		>ISA B 18000
		>
*/
API_IMPORT BOOL uvEng_MDR_SetInlineStationExitAngle(CHAR station, DOUBLE angle);
API_IMPORT BOOL uvEng_MDR_GetInlineStationExitAngle(CHAR station);
/*
 desc : Set/Report Inline Station Exit Distance (cmd : isd)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		distance- [in]  distance (unit: um)
 retn : TRUE or FALSE
 note : This command sets how long end effector has to travel on the straight line
		when exiting a linear-our station during GET operation, or when entering a linear-in station during PUT operation.
		This parameter is also known as "Cassette Depth".
 exam : >ISD A 8000
		>ISD B 8000
		>
*/
API_IMPORT BOOL uvEng_MDR_SetInlineStationExitDistance(CHAR station, DOUBLE distance);
API_IMPORT BOOL uvEng_MDR_GetInlineStationExitDistance(CHAR station);
/*
 desc : Set/Report Inline Station End-Effector Length (cmd : ise)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		length	- [in]  length (unit: um)
 retn : TRUE or FALSE
 note : This command sets the length of end effector used for servicing GET and PUT operations to/from inline stations.
		The length is defined as the distance between center of the upper arm link and center of the wafer placed onto end effector.
		This parameter has to be defined for each inline station.
 exam : >ISE A 7500
		>RPO a
		-8000
		>ISE a -7500
		>
*/
API_IMPORT BOOL uvEng_MDR_SetInlineStationEndEffectLength(CHAR station, DOUBLE length);
API_IMPORT BOOL uvEng_MDR_GetInlineStationEndEffectLength(CHAR station);
/*
 desc : Set/Report Inline Station Corner Radius (cmd : isr)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		radius	- [in]  Radius (unit: degree)
 retn : TRUE or FALSE
 note : This command sets the radius of the arc connecting the two straight lines in Linear-In/Linear-Out motions.
 exam : None
*/
API_IMPORT BOOL uvEng_MDR_SetInlineStationCornerRadius(CHAR station, DOUBLE radius);
API_IMPORT BOOL uvEng_MDR_GetInlineStationCornerRadius(CHAR station);
/*
 desc : Set/Report Inline Station Type (cmd : ist)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		type	- [in]  0: radial in / radial out, 1: radial in / linear out, 2: linear in / linear out
 retn : TRUE or FALSE
 note : This command defines how a station will be serviced during PUT or GET operation.
		IST Station Type defines the type of station Station to be type Type,
		where Type=0: Station is radial (that is, only R will be used for getting into station).
		We will call this station "radial in, radial out"
 exam : >IST A 1
		>IST B 1
		>IST B
		1
		>
*/
API_IMPORT BOOL uvEng_MDR_SetInlineStationType(CHAR station, UINT8 type);
API_IMPORT BOOL uvEng_MDR_GetInlineStationType(CHAR station);
/*
 desc : Set/Report Station Put and Get Delay (cmd : pgd)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Put/Get delay parameter for Station (unit: msec)
 retn : TRUE or FALSE
 note : This command sets or returns the Put and Get delay parameter for a station.
		This is the delay, in milliseconds, for which the robot waits for the vacuum to build up
		before lifting the wafer from the station during pick operations,
		and delay after closing the vavle for place operations.
		For edge-gripping end effectors, this value specifies the delay between operating the gripper and reading of sensors.
 exam : >PGD A
		0
		>PGD A 250
		>
*/
API_IMPORT BOOL uvEng_MDR_SetPutGetDelayTime(CHAR station, UINT32 value);
API_IMPORT BOOL uvEng_MDR_GetPutGetDelayTime(CHAR station);
/*
 desc : Set/Report Put/Get Failure Option (cmd : pgf)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		opt_get	- [in]  8 bit (BYTE) 제어 값 (0 ~ 63)
		opt_put	- [in]  8 bit (BYTE) 제어 값 (0 ~ 63)
 retn : TRUE or FALSE
 note : GetOption and PutOption define following behaviour for GET and PUT commands when wafer cannot be sensed/released:
		-. whether the vacuum will be left ON or OFF on failed GET/PUT (bit 1)
		-. whether the arm will stroke down on failed GET/PUT (bit 2)
		-. whether the arm will retract on failed GET/PUT (bit 3)
		-. whether missing wafer during GET will not be reported as an error (bit 4)
		-. whether to check for wafer presence after wafer is released during PUT (bit 5)
 exam : >PGO
		3,1
		>PGF A
		13,13
		>PGF X
		15,15
		>GET A 1
		>1000				--> vacuum turned OFF
		>GET X 1
		>1008				--> vacuum left ON
		>
*/
API_IMPORT BOOL uvEng_MDR_SetPutGetFailureOption(CHAR station, UINT8 opt_get, UINT8 opt_put);
API_IMPORT BOOL uvEng_MDR_GetPutGetFailureOption(CHAR station);
/*
 desc : Set/Report Get Option and Sync Move Option (cmd : pgo)
 parm : option	- [in]  Option Mode 값 (0 ~ 4)
 retn : TRUE or FALSE
 note : This command sets or returns the Get option parameter and T and Z axes synchronization mode.
		PGO Option sets the Get option to Option.
		PGO returns the current Get Option and T and Z axes synchronization mode
		Option has following effect on GET operation:
		0    GET is performed without checking the presence of a wafer.
		1    The robot controller checks the vacuum sensor (or edge gripper sensor, or Bernoulli capacitance sensor)
			 to determine if there is a wafer on the end effector.
			 If there is no wafer, the robot goes to the next slot and repeats the operation, until a wafer is found,
			 or station slots are exhausted.
		3    The robot controller checks the vacuum sensor (edge gripper sensor, or Bernoulli capacitance sensor)
			 to determine if there is a wafer on the end effector. If there is no wafer, the robot retracts and stops.
		2, 4 Reserved.
 exam : >PGO
		0,0
		>PGO 3
		>STA
		1000
		>PUT A 1
		?
*/
API_IMPORT BOOL uvEng_MDR_SetPutGetOption(UINT8 option);
API_IMPORT BOOL uvEng_MDR_GetPutGetOption();
/*
 desc : Set/Report Station Pitch (cmd : pit)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		pitch	- [in]  value (Pitch value. unit: um)
 retn : TRUE or FALSE
 note : This command sets or returns the pitch parameter for a station. Some common values are:
		1875 (3/16”) for 150mm wafers, 2500 (1/4”) for 200mm wafers, 3937 (10mm) for 300mm wafers.
 exam : >PIT A 1875
		>PIT A
		1875
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationPitch(CHAR station, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetStationPitch(CHAR station);
/*
 desc : Set/Report Start Pushing Distance (cmd : psd)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Pushing Distance 값 (unit: um)
 retn : TRUE or FALSE
 note : This command defines the pushing distance when performing GET operation from a station with pushing enabled
 exam : >PSH a
		0
		>PSD a 100
		?
		>PSH b 1
		>PSD b 200
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationPushingDist(CHAR station, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetStationPushingDist(CHAR station);
/*
 desc : Set/Report Pusher Type (cmd : psh)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  0: No pushing is defined, 1: pushing is defined
 retn : TRUE or FALSE
 note : This command defines whether pushing algorithm will be used when performing GET operation from a station
 exam : >PSH a 0
		>PSH b 1
		>PSH a
		0
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationPushingType(CHAR station, UINT8 value);
API_IMPORT BOOL uvEng_MDR_GetStationPushingType(CHAR station);
/*
 desc : Set/Report Start Pushing Offset (cmd : pss)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Start Pushing Offset (unit: um)
 retn : TRUE or FALSE
 note : This command defines where pushing will start from when performing GET operation from a station with pushing enabled
 exam : >PSH a
		0
		>PSS a 0
		?
		>PSH b 1
		>PSS b -100
>
 */
API_IMPORT BOOL uvEng_MDR_SetStartPushingOffsetR(CHAR station, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetStartPushingOffsetR(CHAR station);
/*
 desc : Set/Report Z Push Offset (cmd : psz)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Start Pushing Offset (unit: um)
 retn : TRUE or FALSE
 note : This command defines the Z axis offset where pushing takes place when performing GET operation from a station with pushing enabled.
 exam : >PSH a
		0
		>PSZ a 10
		?
		>PSH b 1
		>PSZ a 10
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStartPushingOffsetZ(CHAR station, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetStartPushingOffsetZ(CHAR station);
/*
 desc : Set/Report Station Pick-Up Acceleration (cmd : pua)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Pick-Up/Down 가속도 (unit: um/sec^2)
 retn : TRUE or FALSE
 note : This command sets or returns the pick-up acceleration for a station.
		This acceleration is used during vertical stroke-up or stroke-down while performing GET or PUT operation.
		If this parameter is not defined, then homing acceleration is used.
 exam : >PUA A 400
		>PUA B
		400
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationPickUpAccelerationZ(CHAR station, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetStationPickUpAccelerationZ(CHAR station);
/*
 desc : Set/Report Station Pick-Up Speed (cmd : pus)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Pick-Up/Down 속도 (unit: um/sec)
 retn : TRUE or FALSE
 note : This command sets or returns the pick-up speed for a station. This speed is used
		during vertical stroke-up or stroke-down while performing GET or PUT operation.
		If this parameter is not defined, then homing speed is used.
 exam : >PUS A 200
		>PUS B
		400
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationPickUpSpeedZ(CHAR station, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetStationPickUpSpeedZ(CHAR station);
/*
 desc : Set/Report Station Retracted Position (cmd : rpo)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Retracted Position (unit: um)
 retn : TRUE or FALSE
 note : This command sets or reports the retracted position parameter for a station.
		This is the position where the arm is being retracted, before executing and after finishing GET or PUT operation.
		To avoid extra motions, it is recommended that retracted position of all stations to be the same,
		and equal to R axis custom home positin (_CH)
 exam : >RPO A
		-6500
		>
*/
API_IMPORT BOOL uvEng_MDR_SetRetractedStationPositionR(CHAR station, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetRetractedStationPositionR(CHAR station);
/*
 desc : Set/Report Station Position (cmd : spo)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  Station Position (unit: um or degree)
 retn : TRUE or FALSE
 note : This command sets or report the station positions
 exam : >SPO A
		2000,4000,1000
		>TCH A
		>SPO A R 4010
		>EOT
		>SPO A
		2000,4010,1000
		>SPO X T 1200
		?STA
		>1000
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationPositionName(CHAR station, ENG_RANC axis, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetStationPositionName(CHAR station);
/*
 desc : Report Station Number Position (cmd : spon)
 parm : station	- [in]  Station Number (1 ~ 130)
 retn : TRUE or FALSE
 note : This command returns the station positions with station given as a number
 exam : >ENS N
		0:1,2,3,16,24
		1:27,28,29,30,42
		2:53,54
		3:79,81
		4:105,108,120
		>SPON A
		2375,12250,1320
		>SPON 1
		2375,12250,1320
		>SPON 105
		7000,7000,700
		>
*/
API_IMPORT BOOL uvEng_MDR_GetStationPositionNumber(UINT8 station);
/*
 desc : Set/Report Station Stroke (cmd : str)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Z Axis Stroke Position (unit: um)
 retn : TRUE or FALSE
 note : This command sets or returns the stroke parameter for a station.
		The stroke is defined as a vertical distance the Z axis is traveling up
		when lifting a wafer off the station during GET operation, or down during PUT operation.
 exam : >STR A
		70
		>STR A 100
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationStrokeZ(CHAR station, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetStationStrokeZ(CHAR station);
/*
 desc : Set/Report Axes Motion Overlap Factor (cmd : tbl)
 parm : value	- [in]  0 ~ 95 (unit: %)
 retn : TRUE or FALSE
 note : When trajectory mode is activated by TMD command,
		complex motions involving more than one axes are overlapped
		when safe, eg during GET command, arm starts extending toward the station while Theta axis is still moving.
		This feature can increase the performance of pick and place operations.
		While allowed values go up to 95, recommended values are up to 30.
		Additional configuration parameter is reserved to limit Theta axis rotation i nfuture firmware releses.
		This parameter is saved and restored with motion parameters.
 exam : >TMD 1
		>TBL 20
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationAxisOverlapFactor(UINT8 value);
API_IMPORT BOOL uvEng_MDR_GetStationAxisOverlapFactor();
/*
 desc : Set Teach Station Position Mode (cmd : tch)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		slots	- [in]  maximum number of slots
 retn : TRUE or FALSE
 note : This command starts a teaching session and enters teaching mode for the selected station.
		If the maximum number of slots is not specified, it defaults to 1.
		Bit 11 (0x0800) of status word is set during Teach Station Position mode, and is cleared with EOT command.
		If the station already exists, Slots parameter can be omitted.
		If the station does not exists, and Slots parameter is omitted, 1 slot is assumed.
 exam : >TCH A 25
		>STA
		1800
		>EOT
		>
*/
API_IMPORT BOOL uvEng_MDR_StartTeachingStation(CHAR station, UINT8 slots=0x01);
/*
 desc : End of Teaching of Scanning or Station Parameters (cmd : eot)
 parm : None
 retn : TRUE or FALSE
 note : This command ends teaching of a station position or scanning parameters.
		If Option is specified, then teaching process is aborted,
		and no changes are made to the station specified by corresponding TCH <Station> <Slots> command.
		Option is valid only for “Teach station position” command, TCH.
 exam : >TCH A 25
		>CPO
		925,10350,1255
		>EOT
		>SPO A
		925,10350,1255
		>
*/
API_IMPORT BOOL uvEng_MDR_SetTeachingEnd();
/*
 desc : Enable/Disable Trajectory Mode (cmd : tmd)
 parm : option	- [in]  0: Disabled, 1: Enabled
 retn : TRUE or FALSE
 exam :
*/
API_IMPORT BOOL uvEng_MDR_SetStationTrajectoryMode(UINT8 option);
API_IMPORT BOOL uvEng_MDR_GetStationTrajectoryMode();
/*
 desc : Enable/Disable wafer check before pick/place operations (cmd : vchk)
 parm : option	- [in]  0: Disabled, 1: Enabled
 retn : TRUE or FALSE
 note : By default, all pick and place operations (GET, PUT, MSS) start with a check whether a wafer is available on the paddle.
		This check ensures that the operation won't damage the wafer on the paddle, or that there's actually a wafer to be put.
		This command allows for these checks to be overwritten in cases where the workflow ensures that these oprations are safe.
 exam : >VCHK
		0,0
		>VCHK 1 1
		>GET A 1
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationVacuumCheck(UINT8 option);
API_IMPORT BOOL uvEng_MDR_GetStationVacuumCheck();
/*
 desc : Set/Report Vacuum Search Time Out (cmd : vto)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Search Time-out (unit: milli-seconds)
 retn : TRUE or FALSE
 note : This command sets or returns the vacuum search time out for a station.
		This timeout is used before pick and place operations to ensure wafer presence.
 exam : >VTO A
		100
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationVacuumSearchTimeout(CHAR station, UINT32 value);
API_IMPORT BOOL uvEng_MDR_GetStationVacuumSearchTimeout(CHAR station);

/* --------------------------------------------------------------------------------------------- */
/*                        Command Reference  (10.Station motion commands)                        */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Get a Wafer From a Station (cmd : get)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		slot	- [in]  Slot Number (1 ~ 25?)
		end_opt	- [in]  number, 0 (default) or 1
 retn : TRUE or FALSE
 note : This command starts a procedure, which gets a wafer from a specified or current station and slot.
		All station parameters and get option (PGO) parameter affect the execution of this command.
		If a station name is specified, this station becomes the default station.
		If a slot number is specified, this slot becomes the default slot for the station.
		Optional EndOpt parameter, if set to 1, causes GETN command to finish execution after stroke-up motion instide the station,
		skipping final motion toward station' retracted position.
		Using this option allows for greater performance in transporting wafers between stations.
 exam : >GET A 1
		>GET B
		>
*/
API_IMPORT BOOL uvEng_MDR_SetGetWaferFromStationName(CHAR station);
API_IMPORT BOOL uvEng_MDR_SetGetWaferFromStationNameSlot(CHAR station, UINT8 slot);
API_IMPORT BOOL uvEng_MDR_SetGetWaferFromStationNameSlotOpt(CHAR station, UINT8 slot, UINT8 end_opt);
/*
 desc : Execute Object Pick-Up Operation with Designated or Both Arms (cmd : dget)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		arm_no	- [in]  Dual Arm의 번호 0 (primary), 1 (secondary arm) or 2 (both arms)
		slot	- [in]  Slot Number (1 ~ 25?)
 retn : TRUE or FALSE
 note : This command starts a procedure, which gets a wafer from a specified or current station and slot,
		using specified arm, or both arms.
		All station parameters and get option (PGO) parameter affect the execution of this command.
		If a station name is specified, this station becomes the default station.
		If a slot number is specified, this slot becomes the default slot for the station.
 exam : >STA
		0000
		>DGET 0 A 1
		……000C
		>DPUT 0 B 1
		……0000
		>DGET 2 A 2
		……000C
		>INP 4
		0
		>INP 5
		1
		>
*/
API_IMPORT BOOL uvEng_MDR_SetGetWaferFromStationNameSlotDual(CHAR station, UINT8 arm, UINT8 slot);
/*
 desc : Get a Wafer from a station number (cmd : getn)
 parm : station	- [in]  Station Number (1 ~ 130)
		slot	- [in]  Slot Number (1 ~ 25?)
		end_opt	- [in]  number, 0 (default) or 1
 retn : TRUE or FALSE
 note : This command starts a procedure, which gets a wafer from a specified station number (as returned by ESN N command) and slot.
		 All station parameters and get option (PGO) parameter affect the execution of this command.
		Using this command allows for direct reference to all defined stations, without switching station sets.
		Optional EndOpt parameter, if set to 1, causes GETN command to finish execution after stroke-up motion instide the station,
		skipping final motion toward station' retracted position.
		Using this option allows for greater performance in transporting wafers between stations.
 exam : >ENS N
		0:1,2,3,16,24
		1:27,28,29,30,42
		2:53,54
		3:79,81
		4:105,108,120
		>GETN 53 1
		>PUTN 1 5
		>
*/
API_IMPORT BOOL uvEng_MDR_SetGetWaferFromStationNum(UINT8 station);
API_IMPORT BOOL uvEng_MDR_SetGetWaferFromStationNumSlot(UINT8 station, UINT8 slot);
API_IMPORT BOOL uvEng_MDR_SetGetWaferFromStationNumSlotOpt(UINT8 station, UINT8 slot, UINT8 end_opt);
/*
 desc : Move Out of Station (cmd : mos)
 parm : z_offset- [in]  Z 축 Up or Down 거리 (unit: um)
 retn : TRUE or FALSE
 note : This command starts a motion by which a robot arm is taken out of a station, by linear retract motion (for radial stations),
		or by in-line motion (for in-line stations).
		Arm is moved to station’ retracted position either radially, or over two-line trajectory.
		If an offset is specified, Z axis is moved relatively by the distance specified before arm retract motion.
 exam : >IST A
		1
		>SPO A
		-3200,10500,350
		>MTS A 1
		>1000
		>MOS
		>1000
		>CPO
		-4500,-6000,350
		>
*/
API_IMPORT BOOL uvEng_MDR_SetMoveOutStationArmOffsetZ(DOUBLE z_offset);
API_IMPORT BOOL uvEng_MDR_SetMoveOutStation();
/*
 desc : Move a Wafer From Station to Station (cmd : mss)
 parm : station_s	- [in]  Source Station (By Name)
		slot_s		- [in]  Source Slot Number
		station_t	- [in]  Target Station (By Name)
		slot_t		- [in]  Target Slot Number
		r_offset	- [in]  arm axis offset to be used during place (PUT) operation (unit: um)
 retn : TRUE or FALSE
 note : This command starts moving a wafer from one station to another, similar to chained GET and PUT commands.
 exam : >MSS A 1 B 1
		>
*/
API_IMPORT BOOL uvEng_MDR_SetMoveStationToStationByNameROffset(CHAR station_s, UINT8 slot_s,
															   CHAR station_t, UINT8 slot_t,
															   DOUBLE r_offset);
API_IMPORT BOOL uvEng_MDR_SetMoveStationToStationByName(CHAR station_s, UINT8 slot_s,
														CHAR station_t, UINT8 slot_t);
/*
 desc : Move a Wafer from Station Number to Station Number (cmd : mssn)
 parm : station_s	- [in]  Source Station (By Number) (1 ~ 130)
		slot_s		- [in]  Source Slot Number
		station_t	- [in]  Target Station (By Number) (1 ~ 130)
		slot_t		- [in]  Target Slot Number
		r_offset	- [in]  arm axis offset to be used during place (PUT) operation (unit: um)
 retn : TRUE or FALSE
 note : This command starts a procedure, which moves a wafer from a specified station number (as returned by ENS N command)
		and slot into a specified station number (as returned by ENS N command) and slot.
		All station parameters and get option (PGO) parameter affect the execution of this command.
		Using this command allows for direct reference to all defined stations, without switching station sets.
 exam : >ENS N
		0:1,2,3,16,24
		1:27,28,29,30,42
		2:53,54
		3:79,81
		4:105,108,120
		>MSSN 27 1 81 1
		>
*/
API_IMPORT BOOL uvEng_MDR_SetMoveStationToStationByNumberROffset(UINT8 station_s, UINT8 slot_s,
																 UINT8 station_t, UINT8 slot_t,
																 DOUBLE r_offset);
API_IMPORT BOOL uvEng_MDR_SetMoveStationToStationByNumber(UINT8 station_s, UINT8 slot_s,
														  UINT8 station_t, UINT8 slot_t);
/*
 desc : Move arm to Station Approach Position (cmd : mta)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		slot	- [in]  Slot Number
		option	- [in]  veritical position (0, 1, 2 중 한 개 선택)
						0 - vertical position to leave the slot (SPO + OFS), default if not specified
						1 - vertical position to enter the slot (SPO + OFS - STR)
						2 - vertical position at slot position for current slot (SPO)
 retn : TRUE or FALSE
 note : 
 exam :
*/
API_IMPORT BOOL uvEng_MDR_SetMoveArmStationApproachPosition(CHAR station, UINT8 slot, UINT8 option);
/*
 desc : Move Arm to Station Position (cmd : mts)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		slot	- [in]  Slot Number
		option	- [in]  veritical position (0, 1, 2 중 한 개 선택)
						0 - vertical position to leave the slot (SPO + OFS)
						1 - vertical position to enter the slot (SPO + OFS - STR)
						2 - vertical position at slot position for current slot (SPO), default if not specified
 retn : TRUE or FALSE
 note : This command starts moving the robot to the specified station' approach position.
		For inline stations, this command works the same as MTR.
 exam : >SPO A
		5000,10950,1350
		>APP A
		Pickup T = 5000, R = 10950, IST = 1, EE=7500
		Approach T = 4000, R = 9000
			IRT = 4000 , ISA = 18000, ISD = 8000, ISE=7500, EEN=2
		>MTA A 1 1
		>CPO
		4000,9000,1350
		>
*/
API_IMPORT BOOL uvEng_MDR_SetMoveArmStationPosition(CHAR station, UINT8 slot, UINT8 option);
/*
 desc : Set/Report Number of Slots for a Station (cmd : nsl)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		slot	- [in]  maximum number of slots (1 ~ 25?)
 retn : TRUE or FALSE
 note : This command sets or reports the number of slots of the specified station
 exam : >NSL A
		25
		>NSL P 1
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationSlotNumber(CHAR station, UINT8 slot);
API_IMPORT BOOL uvEng_MDR_GetStationSlotNumber(CHAR station);
/*
 desc : Report Number of Stations (cmd : nst)
 parm : None
 retn : TRUE or FALSE
 note : This command reports the number of currently defined stations
 exam : >NST
		3
		>
*/
API_IMPORT BOOL uvEng_MDR_GetStationNumber();
/*
 desc : Set/Report Station Offset (cmd : ofs)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Z Axis Offset (unit: um)
 retn : TRUE or FALSE
 note : This command sets or returns the offset parameter for a station.
		This offset specifies Z axis offset where GET command finishes, or PUT command starts.
 exam : >OFS A
		60
		>OFS A 65
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationOffsetZ(CHAR station, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetStationOffsetZ(CHAR station);
/*
 desc : Put a Wafer into a Station (cmd : put)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		slot	- [in]  slot number (1 ~ 25?)
		offset	- [in]  R Axis Offset (unit: um)
		end_opt	- [in]  number, 0 (default) or 1
 retn : TRUE or FALSE
 note : This command starts a procedure, which puts a wafer to the specified station and slot.
		All station parameters affect the execution of this command.
		The specified station becomes the default station.
		If a slot number is specified, this slot becomes the default slot for the station.
		Optional EndOpt parameter, if set to 1, causes PUT command to finish execution
		after stroke-down motion instide the station, skipping final motion toward station' retracted position.
		Using this option allows for greater performance in transporting wafers between stations.
 exam : >GET A 1
		>PUT A
		>
*/
API_IMPORT BOOL uvEng_MDR_PutWaferIntoStationName(CHAR station);
API_IMPORT BOOL uvEng_MDR_PutWaferIntoStationNameFull(CHAR station, UINT8 slot,
													  DOUBLE offset, UINT8 end_opt);
/*
 desc : Put a Wafer into a Station Number (cmd : putn)
 parm : station	- [in]  Station Number (1 ~ 130)
						  1 ~  26 : set_no_1,
						 27 ~  52 : set_no_2,
						 53 ~  78 : set_no_3,
						 79 ~ 104 : set_no_4,
						105 ~ 130 : set_no_5
 		slot	- [in]  slot number (1 ~ 25?)
		offset	- [in]  R Axis Offset (unit: um)
		end_opt	- [in]  number, 0 (default) or 1
 retn : TRUE or FALSE
 note : This command starts a procedure, which puts a wafer to the specified station and slot.
		All station parameters affect the execution of this command.
		The specified station becomes the default station.
		If a slot number is specified, this slot becomes the default slot for the station.
		Optional EndOpt parameter, if set to 1, causes PUTN command to finish execution
		after stroke-down motion instide the station, skipping final motion toward station' retracted position.
		Using this option allows for greater performance in transporting wafers between stations.
 exam : >ENS N
		0:1,2,3,16,24
		1:27,28,29,30,42
		2:53,54
		3:79,81
		4:105,108,120
		>GETN 27 1 1
		>PUTN 28 1
		>
*/
API_IMPORT BOOL uvEng_MDR_PutWaferIntoStationNo(UINT8 station);
API_IMPORT BOOL uvEng_MDR_PutWaferIntoStationNoFull(UINT8 station, UINT8 slot,
													DOUBLE offset, UINT8 end_opt);
/*
 desc : Execute Object Place Operation with Designated or Both Arms (cmd : dput)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		arm_no	- [in]  0 (primary arm), 1 (secondary arm) or 2 (both arms)
		slot	- [in]  slot number (1 ~ 25?)
 retn : TRUE or FALSE
 note : This command starts a procedure, which gets a wafer from a specified or current station and slot,
		using specified arm, or both arms.
		All station parameters and get option (PGO) parameter affect the execution of this command.
		If a station name is specified, this station becomes the default station.
		If a slot number is specified, this slot becomes the default slot for the station.
 exam : >STA
		0000
		>DGET 2 A 1
		……000C
		>DPUT 2 B 1
		……0000
		>
*/
API_IMPORT BOOL uvEng_MDR_PutWaferIntoStationNameDual(CHAR station, UINT8 arm_no, UINT8 slot);
/*
 desc : Check whether a station is being serviced at the moment (cmd : sbusy)
 parm : None
 retn : TRUE or FALSE
 note : This command reports a value reporting whether a robot os busy servicing a particular station.
		Following commands are considered to service a station:
 exam : >STA
		1000
		>HSA
		>GET A 1
		>SBUSY A
		1
		>SBUSY B
		0
		>100C
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationServiceBusy();
/*
 desc : Move a Wafer from Station to Station, with end option (cmd : sgp)
 parm : station_s	- [in]  Source Station (By Name)
		slot_s		- [in]  Source Slot Number
		station_t	- [in]  Target Station (By Name)
		slot_t		- [in]  Target Slot Number
		r_offset	- [in]  arm axis offset to be used during place (PUT) operation (unit: um)
		end_opt		- [in]  0 (default) or 1
 retn : TRUE or FALSE
 note : This command starts moving a wafer from one station to another, similar to chained GET and PUT commands,
		with optional arm offset and end option during place operation
 exam : >STA
		100C
		>SLCK A 1
		>PUT A 1
		?
*/
API_IMPORT BOOL uvEng_MDR_SetMoveStationToStationEndOpt(CHAR station_s, UINT8 slot_s,
														CHAR station_t, UINT8 slot_t,
														DOUBLE r_offset, UINT8 end_opt);
/*
 desc : Set/Report Station Locked State (cmd : slck)
 parm : station	- [in]  Source Station (By Name) or '#' : All station
		option	- [in]  0: Unlocked, 1: Locked
 retn : TRUE or FALSE
 retn : TRUE or FALSE
 note : This command allows for "locking" motion commands against stations.
		See SBUSY for list of commands affected by station lock state.
		When a station is in "locked" state, all motion commands affecting this station are disabled,
		and will return an error. Once station is locked, only SLCK command with unlock option can unlock it.
		Special station name '#' affects all defined stations.
 exam : >RPO A
		-6500
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationLockedState(CHAR station, UINT8 option);
API_IMPORT BOOL uvEng_MDR_GetStationLockedState(CHAR station);

/* --------------------------------------------------------------------------------------------- */
/*            Command Reference  (11.Station scanning parameters and motion commands)            */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Set/Report Maximum Wafer Thickness (cmd : _mt)
 parm : value	- [in]  maximum wafer thickness (unit: um)
 retn : TRUE or FALSE
 note : This command specifies one of the parameters used during scanning and filtering process.
		It specifies the maximum expected difference in scanning readings for top and bottom edges of a wafer.
		Default value is set to 25 (0.025”).
 exam : >_MT
		25
		>_MT 35
		>_MT
		35
		>
*/
API_IMPORT BOOL uvEng_MDR_SetMaximumWaferThick(DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetMaximumWaferThick();
/*
 desc : Set/Report Wafer Scanning Z Axis Acceleration (cmd : _sa)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Z axis acceleration (unit: um / sec^2)
 retn : TRUE or FALSE
 note : This command sets or reports the Z axis acceleration used during wafer scanning.
		If this value is not given (zero or negative), search acceleration will be used.
		The value is saved in the stations parameters file by SSP command and may be restored by RSP command.
 exam : >_SA B
		200
		>_SA B 500
		>_SA A 1000
		>_SA A
		1000
		>
*/
API_IMPORT BOOL uvEng_MDR_SetWaferScanZAxisAcceleration(CHAR station, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetWaferScanZAxisAcceleration(CHAR station);
/*
 desc : Set Current Position as Scanning Center Coordinate (cmd : _sc)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
 retn : TRUE or FALSE
 note : When this command is issued for the first time against a station, it sets default values for
		Left and Right scanning positions as well. If those positions are already defined, _SC command will not change them.
 exam : >_ST
		1
		>MTR a 1
		>MVA R 6570
		>MVR Z -120
		>TCS
		>_SC a
		>EOT
		>SCP a
		3600,6570,1180
		>
*/
API_IMPORT BOOL uvEng_MDR_SetCurrentPositionScanningCenterCoordinate(CHAR station);
/*
 desc : Set Current Position as Scanning Left or Right Coordinate (cmd : _sl or _sr)
 parm : direct	- [in]  0x00: Left, 0x01: Right
		station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
 retn : TRUE or FALSE
 note : This commands sets current position (the one displayed with CPO command) to be the Scanning
		Left or Right Coordinate for first wafer for a station given by Station.
		Z (vertical) scanning coordinate must be the position where scanner would sense the middle of
		a wafer placed in the first station slot.
		This command is valid only during Scan Teaching Mode (entered with TCS command).
 exam : >_ST
		0
		>CPO
		3350,-9550,1180
		>TCS
		>_SL a
		>EOT
		>SLP a
		3350,-9550,1180
		>
*/
API_IMPORT BOOL uvEng_MDR_SetCurrentPositionScanningCoordinate(ENG_MALR direct, CHAR station);
/*
 desc : Set/Report Wafer Scanning Z Axis Speed (cmd : _ss)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Z axis speed (unit: um / sec^2)
 retn : TRUE or FALSE
 note : This command sets or reports the Z axis speed used during wafer scanning.
		The value is saved in the stations parameters file by SSP command and may be restored by RSP command.
 exam : >_SS B
		200
		>_SS B 500
		>_SS A 1000
		>_SS A
		1000
		>
*/
API_IMPORT BOOL uvEng_MDR_SetWaferScanZAxisSpeed(CHAR station, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetWaferScanZAxisSpeed(CHAR station);
/*
 desc : Set/Report Scanner Type (cmd : _st)
 parm : value	- [in]  0 for reflective type of scanner, 1 for thru-beam type of scanner
 retn : TRUE or FALSE
 note : This command sets the scanner type used for wafer scanning.
		For reflective-type scanners, up to three scanning start positions are defined,
		and two-pass scanning is performed on left and right scanning positions.
		For thru-beam scanners, single (center) scanning position is used for both passes.
		The value is saved in the stations parameters file by SSP command and may be restored by RSP command.
 exam : >_ST
		1
		>_ST 0
		>
*/
API_IMPORT BOOL uvEng_MDR_SetScannerSensorType(UINT8 value);
API_IMPORT BOOL uvEng_MDR_GetScannerSensorType();
/*
 desc : Report Scanned Positions (cmd : dscn)
 parm : direction	- [in]  0 : means results from scanning up, (bottom to top),
							1 : means results from scanning down (top to bottom) will be printed
 retn : TRUE or FALSE
 note : DSCN Direction prints out all Z coordinates where laser scanner reported a change
		from on to off or vice versa for last scanning process in the Direction.
		Those coordinates are displayed with precision of 0.0001”. One position per line is printed.
		In Scan Debug Mode (SDBG 1), each pair of samples is reported on a single line.
 exam : >SCN B
		>MAP
		1011101000000010110111100
		>DSCN 0
		15079
		15241
		... ...
		69994
		70179
		>SDBG 1
		Scan debug output is ON:1
		>DSCN 0
		Dumping 22 samples:
		0,1,15079,15241
		... ...
		20,21,69994,70179
		>
*/
API_IMPORT BOOL uvEng_MDR_GetScannedPositions(UINT8 direction);
/*
 desc : Report Results from Last Wafer Scanning (cmd : map)
 parm : direction	- [in]  0 : means results from scanning up, (bottom to top),
							1 : means results from scanning down (top to bottom) will be printed
 retn : TRUE or FALSE
 note : MAP Direction prints out a wafer map from the last station scanning in the required direction.
		If no map is available (e.g. no wafers were present), then an error prompt is printed.
		Otherwise, a single-character per wafer slot is printed with following meaning:
			0	No wafer present in this slot
			1	Single wafer is present, no other cross-slotted
			2	Two wafers found in single slot
			4	Single Cross-slotted wafer
			5	Single wafer with cross-slotted wafer on next slot
			6	Double-slotted, or cross-slotted wafer with another wafer
		Detection of crossed wafers next to normal wafer depends on type of scanner used.
		Thru-beam scanners cannot provide reliable data to distinguish single cross-slotted wafer,
		and single wafer with cross-slotted wafer occupying same slot.
 exam : >SCN a
		>MAP
		5411111111111111111000000
		>SCN b 0
		>MAP 0
		1000100010001000100020000
		>MAP 1
		?
*/
API_IMPORT BOOL uvEng_MDR_GetResultLastWaferScanning(UINT8 direction);
/*
 desc : Set/Report Beam Thresholds (cmd : _bt)
 parm : value_l	- [in]  Bottom Threadhold (unit: um)
		value_h	- [in]  Uppoer Threadhold (unit: um)
 retn : TRUE or FALSE
 note : This command defines additional parameters accounting for variance in vertical positions in a cassette.
		These parameters account for the difference in scanned positions for same set of wafers but placed in different cassettes,
		or not firmly set inside their pockets.
*/
API_IMPORT BOOL uvEng_MDR_SetBeamThreadhold(DOUBLE value_l, DOUBLE value_h);
API_IMPORT BOOL uvEng_MDR_GetBeamThreadhold();
/*
 desc : Set /Report Per-Station Beam Thresholds (cmd : sbt)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value_l	- [in]  Bottom Threadhold (unit: um)
		value_h	- [in]  Uppoer Threadhold (unit: um)
 retn : TRUE or FALSE
 note : This command defines additional, per-station parameters accounting for variance in vertical positions in a cassette.
		These parameters account for the difference in scanned positions for same set of wafers but placed in different cassettes,
		or not firmly set inside their pockets. If these parameters are zero, parameters defined by are _BT used.
 exam : >_BT
		1250, 1250
		>SBT A 700 600
		1000, 800
		>
*/
API_IMPORT BOOL uvEng_MDR_SetPerStationBeamThreadhold(CHAR station, DOUBLE value_l, DOUBLE value_h);
API_IMPORT BOOL uvEng_MDR_GetPerStationBeamThreadhold(CHAR station);
/*
 desc : Start wafer Scanning for a Station (cmd : scn)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		direct	- [in]  0: 아래에서 위로, 1: 위에서 아래로, 2: 양방향으로 (동시에?)
		angle	- [in]  0: 스캔이 중앙 위치, 1: 스캔이 왼쪽 위치, 2: 스캔이 오른쪽 위치
 retn : TRUE or FALSE
 note : SCN Station Direction Angle starts single-pass scanning process for a given station, at given direction and at given position.
									If thru-beam laser scanner is configured with _ST command, Angle must equal to 0 (Center Scanning Position)
		SCN Station Direction starts single-pass scanning process for a given station, at given direction and at Center Scanning Position.
		SCN Station starts dual-pass scanning process for a given station.
					If reflective scanner is configured (_ST 0), pass up starts at Left Scanning Position,
					and pass down starts at Right Scanning Position.
					If thru-beam scanner is configured (_ST 1), both passes up and down are performed at Center Scanning Position.
		Single-pass scanning finishes with arm (R axis) moved to Retracted Position for requested station.
		If two-pass scanning is requested, T axis move between Left and Right Scanning positions is done with arm (R axis) at Left Scanning Position.
		If a wafer is detected before starting vertical motion, or after it finishes,
		scanning is aborted and bit 5 (0x0020) of Previous Error Reason is set.
 exam : >SCN a 0
		>MAP
		1056101000000010640111100
		>
*/
API_IMPORT BOOL uvEng_MDR_StartStationWaferScanning(CHAR station, UINT8 direct, UINT8 angle);
/*
 desc : Set/Report Station’ Scanning Center Coordinate (cmd : scp)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  coordinate or degree value (unit: um or degree)
 retn : TRUE or FALSE
 note : This commands sets given axis coordinate to be that axis’ Scanning coordinate
		for first wafer for a station given by Station parameter.
		This position will be used when scanning with thru-beam scanner (by default),
		or specifically asked by SCN command.
		Z (vertical) scanning coordinate (if specified) must be the position
		where scanner would sense the middle of a wafer placed in the first station slot.
		Setting of coordinates is valid only during Scan Teaching Mode (entered with TCS command).
		If Z axis coordinate is defined, and lowest and/or highest Z scanning positions are zero,
		this command sets lowest and/or highest Z scanning positions (ones defined with SZL and SZH commands)
		to be one-half of station pitch below and above first and last wafers in the station.
		The value is saved in the stations parameters file by SSP command and may be restored by RSP command.
 exam : >_ST
		0
		>SPO b
		12000,9250,40
		>TCS
		>SCP b T -6000
		>SCP b R -9500
		>SCP b Z 160
		>EOT
		>SCP b
		>-6000,-9500,160
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationScanningCenterCoord(CHAR station, ENG_RANC axis, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetStationScanningCenterCoord(CHAR station);
/*
 desc : Set/Report Scan Debug Mode (cmd : sdbg)
 parm : option	- [in]  0: Disabled, 1: Enabled
 retn : TRUE or FALSE
 note : This command sets a debug mode for scan operations.
		When debug mode is set, additional information is reportted during SCN command.
		This includes scan parameters, and positions where wafers are detected.
 exam : >SCN A
		>ScanCassette: T=-2840 R=7950 Z1=1255 Z2=7060
		0:15078; 1:15245; .... 7:25116;
		8:29939; 9:30140; ... 15:62373;
		16:64708; 17:64872; 18:67428; 19:67622; 20:69990; 21:70182;
		ScanCassette: T=-2840 R=7950 Z1=7060 Z2=1255
		0:70149; 1:69968; .... 7:62005;
		8:56426; 9:55502; .... 15:24921;
		16:21299; 17:20493; 18:20087; 19:19909; 20:15221; 21:15049;
		After pass 1: 1056101000000010640111100
		After pass 2: 1054101000000010640111100
		Samples: Up=22, Down=22
		>
*/
API_IMPORT BOOL uvEng_MDR_SetScanDebugMode(UINT8 option);
/*
 desc : Set/Report Station’ Scanning Left Coordinate (cmd : slp)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  coordinate or degree value (unit: um or degree)
 retn : TRUE or FALSE
 note : This commands sets given axis coordinate to be that axis’ Scanning position for first wafer for
		a station given by Station parameter.
		This position will be used during the first pass with a reflective sensor (by default), or specifically asked by SCN command.
		Z (vertical) scanning coordinate (if specified) must be the position
		where scanner would sense the middle of a wafer placed in the first station slot.
		This command is valid only during Scan Teaching Mode (entered with TCS command).
		The value is saved in the stations parameters file by SSP command and may be restored by RSP command.
 exam : >_ST
		0
		>SPO b
		12000,9250,40
		>TCS
		>SLP b T -6250
		>SLP b R -9555
		>SLP b Z 160
		>EOT
		>SLP b
		>-6250,-9555,160
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationScanningLeftCoord(CHAR station, ENG_RANC axis, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetStationScanningLeftCoord(CHAR station);
/*
 desc : Set/Report Station Scanning Right Coordinate (cmd : srp)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		axis	- [in]  Axis Name (ENG_RANC : R1/R2 : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  coordinate or degree value (unit: um or degree)
 retn : TRUE or FALSE
 note : This commands sets given axis coordinate to be that axis’ Right Scanning position for
		first wafer for a station given by Station parameter.
		This position will be used during the second pass with a reflective sensor (by default), or specifically asked by SCN command.
		Z (vertical) scanning coordinate (if specified) must be the position
		where scanner would sense the middle of a wafer placed in the first station slot.
		This command is valid only during Scan Teaching Mode (entered with TCS command).
 exam : >_ST
		0
		>SPO b
		12000,9250,40
		>TCS
		>SRP b T -5750
		>SRP b R -9555
		>SRP b Z 160
		>EOT
		>SRP b
		-5750,-9555,160
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationScanningRightCoord(CHAR station, ENG_RANC axis, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetStationScanningRightCoord(CHAR station);
/*
 desc : Set/Report Per-Station Maximum Wafer Thickness (cmd : smt)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Wafer Thickness (unit: um)
 retn : TRUE or FALSE
 note : This command specifies per-station value for maximum wafer thickness.
		It specifies the maximum expected difference in scanning readings for top and bottom edges of a wafer.
		If this parameter is set to zero, parameter defined by are _MT is used.
 exam : >_MT
		25
		>SMT A 45
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationWaferMaxThickness(CHAR station, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetStationWaferMaxThickness(CHAR station);
/*
 desc : Place a wafer into a station, pick a wafer from a station, with end option (cmd : spg)
 parm : station_s	- [in]  Source Station (By Name)
		slot_s		- [in]  Source Slot Number
		station_t	- [in]  Target Station (By Name)
		slot_t		- [in]  Target Slot Number
		r_offset	- [in]  arm axis offset to be used during place (PUT) operation. (unit: um)
		end_opt		- [in]  0 (default) or 1
 retn : TRUE or FALSE
 note : This command starts moving a wafer from one station to another, similar to chained PUT and GET commands,
		with optional arm offset during place operation, and end option during pick option.
		SPG Station1 Slot1 Station2 Slot2 ROffset EndOpt starts placing a wafer into Station1,
		Slot 1 with specified arm offset, followed by picking a wafer from Station 2, Slot 2 with end-option,
		and is equivalent to the following sequence of commands:
			>PUT <Station1> <Slot1> <ROffset>
			>GET <Station2> <Slot2> <EndOption>
		Specific firmware version may support command SSPG as an alias of SPG, applying the same set of parameters
 exam : >STA
		100C
		>SPG A 1 B 1 -10 1
		>100C
		>
*/
API_IMPORT BOOL uvEng_MDR_SetPlaceStationToStationEndOpt(CHAR station_s, UINT8 slot_s,
														 CHAR station_t, UINT8 slot_t,
														 DOUBLE r_offset, UINT8 end_opt);
/*
 desc : Set/Report Scanning Highest / Lowest Z Coordinate (cmd : szh or szl)
 parm : direct	- [in]  0x00:Highest, 0x01:Lowest
		station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Z Axis Position (unit: um)
 retn : TRUE or FALSE
 note : This commands sets lowest coordinate (staring position when going up,
		and final position when going down) for Z axis during scanning.
		This is position where scanning motion will start (or finish), and where capture process is turned on (or off).
		At this position, wafer mapper should detect a wafer, or scanning command will fail.
		Set command is valid only during Scan Teaching Mode (entered with TCS command).
 exam : >SPO b
		12000,9250,40
		>SCP b
		-6000,-9500,160
		>TCS
		>SZL b -50
		>EOT
		>SZL b
		-50
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStationHighLowestCoordZ(UINT8 direct, CHAR station, DOUBLE value);
API_IMPORT BOOL uvEng_MDR_GetStationHighLowestCoordZ(UINT8 direct, CHAR station);
/*
 desc : Start Teach Scanning Parameters (cmd : tcs)
 parm : None
 retn : TRUE or FALSE
 note : This command enables setting or changing of wafer scanning parameters.
		It has to be issued prior to any command which sets scanning parameter
		(_MT, _SC, _BT, _SC/SCP, _SL/SLP, _SR/SRP and SZL/SZH).
		It cannot be issued while TCH command is in effect (teaching station coordinates),
		or while another TCS command is in effect.
		Bit 14 (0x4000) of status word is set during Teach Scanning mode, and is cleared with EOT command.
 exam : >TCS
		>STA
		4000
		>SZH b 6250
		>TCS
		?EOT
		>
*/
API_IMPORT BOOL uvEng_MDR_SetStartTeachScanningParam();

/* --------------------------------------------------------------------------------------------- */
/*      Command Reference  (12.Align-on-Flight (on-The-Flight, OtF) parameters and commands)     */
/*         (OtF 기능을 사용하면 선택 및/또는 위치 이동 중에 웨이퍼 중심 변위 측정 가능)          */ 
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Report Current On-the-Fly Set Number (cmd : acs)
 parm : None
 retn : TRUE or FALSE
 note : This command reports the current OtF calibration set number,
		and is maintained for compatability with previous versions only.
		Set parameter is ignored, as calibration parameters are stored with every station.
 exam : >ACS
		1
		>ACS 5
		?
*/
API_IMPORT BOOL uvEng_MDR_GetOtFSetNumber();
/*
 desc : Execute On-the-Fly Centering Calibration Run (cmd : aofc)
 parm : dist	- [in]  relative distance to move arm axis during calibration (unit: um)
 retn : TRUE or FALSE
 note : With reference wafer properly placed on the end effector,
		and robot oriented toward target station, AOFC command moves the wafer at specified distance,
		latches sensor positions at both front and back edges of the wafer, and calculates sensor positions.
		These positions, as well as previously defined wafer radius, end effector length, calibration start position,
		and calibration distance, are stored for future reference.
		Wafer radius and end effector length must be specified with ASL command prior to executing of this command.
 exam : >MTR P 1
		>ASL 5906 11500
		>AOFC 15000
		>AOFD -1
		Wafer Radius=5906 EELength=11500 Distance=15000
		Calibration data:
		Left: X=-2.9629, Y= 15.6370
		Right: X= 3.3586, Y= 16.1380
		Start positions: 6691,-5000,1689
		>
*/
API_IMPORT BOOL uvEng_MDR_SetOtFCentCalibRun(DOUBLE dist);
/*
 desc : Set/Report Check for notch on second scan option (cmd : aofcn2)
 parm : option	- [in]  0 will ignore notch detection on second scan.
						1 will force notch detection on second scan.
 retn : TRUE or FALSE
 note : This option controls whether Align-on-flign action commands (GETC, PUTC etc) will look for notch if second scan was necessary.
 exam : None
*/
API_IMPORT BOOL uvEng_MDR_SetOtFNotchSecondScan(UINT8 option);
API_IMPORT BOOL uvEng_MDR_GetOtFNotchSecondScan();
/*
 desc : Execute On-the-Fly Calibration Run against a Station (cmd : aofcs)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		direct	- [in]  direction of arm motion
						0: Retracted Position or Station' Position
						1: Station' Position or Retracted Position
		dist	- [in]  relative distance to move arm axis during calibration (unit: um)
 retn : TRUE or FALSE
 note : With reference wafer properly placed on the end effector, AOFCS first positions the arm at specified Station,
		slot 1 retracted position (Direction = 0), or station' position (Direction = 1),
		followed by moving the wafer either at specified Distance, or toward retracted position (Direction = 1),
		or station' position (Direction = 0). During that motion,
		sensor positions at both front and back edges of the wafer are latched, and sensor positions are calculated.
		These positions, as well as previously defined wafer radius, end effector length, calibration start position,
		and calibration distance, are stored for future reference.
		Wafer radius and end effector length must be specified prior to executing of this command.
		For best results, it is advised that calibration is performed in same direction as future measurement motions.
		In normal circumstances (station is with positive arm coordinate), distance should be negative
		if GETC will be used, and positive if PUTC will be used.
		The resulting calibration values are saved in the motion parameters file by SMP command and may be restored by RMP command.
 exam : >ASL 5906 11500
		>AOFCS A 1
		>AOFD -1
		Wafer Radius=5906 EELength=11500 Distance=15000
		Calibration data:
		Left: X=-2.9629, Y= 15.6370
		Right: X= 3.3586, Y= 16.1380
		Start positions: 6691,-5000,1689
		>
*/
API_IMPORT BOOL uvEng_MDR_SetOtFCaliRunStationDirection(CHAR station, UINT8 direct);
API_IMPORT BOOL uvEng_MDR_SetOtFCaliRunStationDistance(CHAR station, UINT8 direct, DOUBLE dist);
/*
 desc : Set/Report On-the-Fly Debug Level (cmd : aofd)
 parm : value	- [in]  requested level of verbosity, or -1 (range: 0 ~ 15 or -1)
 retn : TRUE or FALSE
 note : This command sets the amount of debug information to be displayed during execution of various align-on-fly commands.
		Default value is 0 (no information at all), with maximum information printed by AOFD 15.
 exam : None
*/
API_IMPORT BOOL uvEng_MDR_SetOtFDebugLevel(INT8 value);
API_IMPORT BOOL uvEng_MDR_GetOtFDebugLevel();
/*
 desc : Report Samples from Last OtF Operation (cmd : aofdump)
 parm : None
 retn : TRUE or FALSE
 note : This command can be used to dump, for troubleshooting purposes, wafer samples captured during OtF operation.
		Format of the output depends on current OtF debug level defined by AOFD.
 exam : >AOFD 0
		>MVRC R 12000
		>AOFDUMP
		; O=1; R=40149:l; O=-1; R=45455:R;; O=1; R=106898:l; O=-1; R=106984:R;
		>AOFD 3
		>AOFDUMP
		Total of 4 positions collected
		Order: 0x0100; R=40149:l
		Order: 0x0101; R=45455:R; **
		Order: 0x0100; R=106898:l
		Order: 0x0101; R=106984:R; **
		>
*/
API_IMPORT BOOL uvEng_MDR_SetOtFDebugDump();
/*
 desc : Set/Report Ignore Middle Samples option (cmd : aofim)
 parm : option	- [in]  0 - will not ignore any samples obtained during the middle of wafer.
							If any such samples are produced, action commands will produce wrong results.
						1 - will ignore all but first and last obtained samples during samples collection.
 retn : TRUE or FALSE
 note : This option controls how Align-on-flign action commands (AOFC, AOFCS, GETC, PUTC etc)
		imterpret extra samples produced from middle of the wafer. For proper functioning,
		these commands need only samples obtained at wafer edge.
 exam : None
*/
API_IMPORT BOOL uvEng_MDR_SetOtFIgnoreMidSamples(UINT8 option);
API_IMPORT BOOL uvEng_MDR_GetOtFIgnoreMidSamples();
/* --------------------------------------------------------------------------------------------- */
/*                  Command Reference  (13.New commands for DUAL Arm Axis Only)                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Move Both Arms to Same Absolute Position (cmd : dmva)
 parm : position	- [in]  target axes position (unit: um)
 retn : TRUE or FALSE
 note : This command starts moving of both arm axes R1 and R2 to same absolute position.
		Each axis will execute the motion using this axis’ motion parameters (speed, acceleration, deceleration etc).
 exam : >CPO
		1000,2000,3000,1500
		>DMVA -6000
		… … 0000
		>CPO
		1000,-6000,-6000,1500
		>
*/
API_IMPORT BOOL uvEng_MDR_SetDualMoveArmAbsolutePositionR(DOUBLE position);
/*
 desc : Move Both Arms to Same Absolute Position (cmd : dmva)
 parm : distance	- [in]  target axes position (unit: um)
 retn : TRUE or FALSE
 note : This command starts moving of both arm axes R1 and R2 to same absolute position.
		Each axis will execute the motion using this axis’ motion parameters (speed, acceleration, deceleration etc).
		DMVA Position moves axes R1 and R2 to specified target position.
		At the end of the motin, both axes R1 and R2 will have same position.
 exam : >CPO
		1000,2000,3000,1500
		>DMVA -6000
		… … 0000
		>CPO
		1000,-6000,-6000,1500
		>
*/
API_IMPORT BOOL uvEng_MDR_SetDualMoveArmRelativeDistanceR(DOUBLE distance);
/*
 desc : Set/Report Arm to Be Used for Wafer Mapping (cmd : see)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		arm_no	- [in]  Arm Number (0: Primary (Right), 1: Secondary (Left))
 retn : TRUE or FALSE
 note : This command setos or report which arm is moved when executing wafer mapping operations with a station.
 exam : >EEN A
		1
		>SEE A
		0
		>
*/
API_IMPORT BOOL uvEng_MDR_SetDualSelectMapperArm(CHAR station, UINT8 arm_no);
/* --------------------------------------------------------------------------------------------- */
/*                                            Common                                             */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Current Station service status (Busy, Idle, Not defined. Only Station !!!)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MDR_IsStationServiceBusy();
API_IMPORT BOOL uvCmn_MDR_IsStationServiceIdle();
API_IMPORT BOOL uvCmn_MDR_IsStationServiceUndefined();
/*
 desc : Whether the state value of system is valid (!!! Important !!!)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MDR_IsValidSystemStatus();
/*
 desc : Whether there are packets received for the most recently sent command
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MDR_IsLastSendAckRecv();
/*
 desc : Whether the receive response was successful for the most recently sent command
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MDR_IsLastSendAckSucc();
/*
 desc : Returns an error message for system status values
 parm : mesg	- [out] A return buffer in which a detailed description of the error code will be stored.
		size	- [in]  The size of 'mesg' buffer
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MDR_GetSystemStatusMesg(TCHAR *mesg, UINT32 size);
/*
 desc : Return an error message about the Home Switch status value
 parm : mesg	- [out] A return buffer in which a detailed description of the error code will be stored.
		size	- [in]  The size of 'mesg' buffer
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MDR_GetHomeSwitchStateMesg(TCHAR *mesg, UINT32 size);
/*
 desc : Returns a string message for the code value of "extended error code (cmd : eerr)"
 parm : mesg	- [out] Buffer where string messages will be stored
		size	- [in]  The size of 'mesg' buffer
 retn : FALSE (failed to return), TRUE (succeeded to return)
*/
API_IMPORT BOOL uvEng_MDR_GetExtErrorMesg(TCHAR *mesg, UINT32 size);
/*
 desc : Returns an error message about the status of home switches (즉, Error codes returned by RHS)
 parm : mesg	- [out] A return buffer in which a detailed description of the error code will be stored.
		size	- [in]  The size of 'mesg' buffer
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MDR_GetHomeSwitchesState(TCHAR *mesg, UINT32 size);
/*
 desc : Whether the motion is in stopped state
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MDR_IsWaitForMotionCompleted();
/*
 desc : Whether the current motion (Robot Arm) is moving, i.e. Busy (asynchronous control)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT UINT8 uvCmn_MDR_IsMotorArmBusy();
/*
 desc : Whether the current motion (Robot Arm) is idle (asynchronous control)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT UINT8 uvCmn_MDR_IsMotorArmIdle();
/*
 desc : Whether the current Vacuum Switch & Sensor are both On (asynchronous control)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT UINT8 uvCmn_MDR_IsVacuumSwitchSensorAllOn();
/*
 desc : Whether there is the most recently received data
 parm : cmd		- [in]  command to be checked
		axis	- [in]  Axis Name (R1/R2 : Horizontal, Z : Vertical, T : Angle, K : N/A)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MDR_IsRecvCmdData(ENG_RSCC cmd=ENG_RSCC::rsc_none, ENG_RANC axis=ENG_RANC::rac_t);
API_IMPORT BOOL uvCmn_MDR_IsRecvCmdLast(ENG_RANC axis=ENG_RANC::rac_t);
/*
 desc : Whether an error has occurred inside the equipment (system)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MDR_IsSystemError();
#if 0
/*
 desc : In case of using with Info mode 1 set, the operation completion status is determined by receiving a return code (asynchronous control)
 parm : None
 retn : 0x00 (Not determined), 0x01 (Not Ready), 0x02 (Completed), 0x03 (error state), 0x04 (Unknown)
*/
API_IMPORT UINT8 uvCmn_MDR_IsWaitMotionDoneReply();
#endif

/* --------------------------------------------------------------------------------------------- */
/*                            Returns a value stored in shared memory                            */
/* --------------------------------------------------------------------------------------------- */

/* String data value returned from the most recently requested command */
API_IMPORT PTCHAR uvCmn_MDR_GetRecvLastString();
/* Returns the Retracted Position value according to the current Effector Number */
API_IMPORT DOUBLE uvCmn_MDR_GetRetractedPositionForEffectorNumber();
/* Currently set custom station parameter (cmd : cst) */
API_IMPORT CHAR uvCmn_MDR_GetValueStationName();
/* Serial number of the robot (cmd : snr) */
API_IMPORT PCHAR uvCmn_MDR_GetValueSerialNumber();
/* Current debug level, or calibration parameters (cmd : aofd) */
API_IMPORT INT8 uvCmn_MDR_GetValueOTFDebugLevel();
/* Currently set information mode, and option (cmd : inf) */
API_IMPORT UINT8 uvCmn_MDR_GetValueInfoMode();
/* Currently set station set number (cmd : _css) */
API_IMPORT UINT8 uvCmn_MDR_GetValueStationSetNo();
/* Current station number (cmd : cstn) */
API_IMPORT UINT8 uvCmn_MDR_GetValueStationNo();
/* Current scanner type (cmd : _st) */
API_IMPORT UINT8 uvCmn_MDR_GetValueScannerType();
/* Current slot of a station (cmd : csl) */
API_IMPORT UINT8 uvCmn_MDR_GetValueCurrentSlotNo();
/* Get end effector number in use for a station  (cmd : een) */
API_IMPORT UINT8 uvCmn_MDR_GetValueEndEffectorNum();
/* Get end effector orientation (up or down) in use for a station  (cmd : eeo) */
API_IMPORT UINT8 uvCmn_MDR_GetValueEndEffectorStationUpDown();
/* Get inline station type (cmd : IST) */
API_IMPORT UINT8 uvCmn_MDR_GetValueLinlineStationType();
/* Current state of loaded parameters (cmd : lenb) */
API_IMPORT UINT8 uvCmn_MDR_GetValueLoadedSpeedEnable();
/* Number of slots of specified or default station (cmd : nsl) */
API_IMPORT UINT8 uvCmn_MDR_GetValueStationSlotNum();
/* Number of stations (cmd : nst) */
API_IMPORT UINT8 uvCmn_MDR_GetValueStationNum();
/* Current value of station pusher type (cmd : psh) */
API_IMPORT UINT8 uvCmn_MDR_GetValuePusherType();
/* Numeric value, 0 or 1 (cmd : sbusy) */
API_IMPORT UINT8 uvCmn_MDR_GetValueStationServiceBusy();
/* Current station lock state (cmd : slck) */
API_IMPORT UINT8 uvCmn_MDR_GetValueStationLockedState();
/* Currently set motion overlap factor (cmd : tbl) */
API_IMPORT UINT8 uvCmn_MDR_GetValueStationAxisOverlapFactor();
/* Current wafer check options (cmd : vchk) */
API_IMPORT UINT8 uvCmn_MDR_GetValueStationTrajectoryMode();
/* 0: Wafer was not detected; 1: Wafer was detected (cmd : vst) num=0 -> Top Effector, num=1 -> Bottom Effector */
API_IMPORT UINT8 uvCmn_MDR_GetValueEndEffectorWaferCheck(UINT8 num);
/* Wait for Motion Completed (cmd : wmc) */
API_IMPORT UINT8 uvCmn_MDR_GetValueMotionWaitFinished();
/* Current state of Check for notch on second scan (cmd : aofcn2) */
API_IMPORT UINT8 uvCmn_MDR_GetValueOTFSetSecondScanNotch();
/* Current state of Ignore Middle Samples option (cmd : aofim) */
API_IMPORT UINT8 uvCmn_MDR_GetValueOTFIgnoreMiddleSamples();
/* Currently set Scan Debug Mode (cmd : sdbg) */
API_IMPORT UINT8 uvCmn_MDR_GetValueScanDebugMode();
/* Current arm used for scanning of specified station (cmd : see) */
API_IMPORT UINT8 uvCmn_MDR_GetValueArmUsedScanSpecStation();
/* Print-out of wafer map from last scanning, if available (cmd : map) */
API_IMPORT PUINT8 uvCmn_MDR_GetValueWaferMapLoadedState();
/* Currently set current limit of all axes (cmd : _cl) */
API_IMPORT PUINT8 uvCmn_MDR_GetValueCurrentLimit();
/* Current state of Controlled Stop Feature. (cmd : cstp) */
API_IMPORT PUINT8 uvCmn_MDR_GetValueControlledStop();
/* Current failure options (cmd : pgf) */
API_IMPORT PUINT8 uvCmn_MDR_GetValuePutGetFailedOption();
/* Currently set Get option and T and Z axes synchronization mode (cmd : pgo) */
API_IMPORT PUINT8 uvCmn_MDR_GetValuePutGetOption();
/* Current wafer check options (cmd : vchk) */
API_IMPORT PUINT8 uvCmn_MDR_GetValueStationVacuumCheck();
/* Absolute Encoder status (cmd : _aest) */
API_IMPORT PINT16 uvCmn_MDR_GetValueEncStatus();
/* Low-level absolute encoder status (cmd : _ralmc) => Not perfect */
API_IMPORT UINT16 uvCmn_MDR_GetValueLowLevelEncStatus();
/* Input state of all digital inputs (cmd : inp) */
API_IMPORT UINT16 uvCmn_MDR_GetValueDigitalInputState();
/* Output state of all digital inputs (cmd : out) */
API_IMPORT UINT16 uvCmn_MDR_GetValueDigitalOutputState();
/* All robot axes home switches state (cmd : rhs) */
API_IMPORT UINT16 uvCmn_MDR_GetValueHomeSwitchState();
/* Error code from last failed command (cmd : eerr) */
API_IMPORT UINT16 uvCmn_MDR_GetValueExtendedErrorCode();
/* Currently set on-the-fly set number (cmd : acs) */
API_IMPORT UINT16 uvCmn_MDR_GetValueOTFSetNumber();
/* System status word (cmd : sta) */
API_IMPORT UINT16 uvCmn_MDR_GetValueSystemStatus();
/* Currently set time-out values (cmd : _bb) */
API_IMPORT PUINT16 uvCmn_MDR_GetValueBackupDCPower();
/* Currently set custom generic parameter (cmd : _cpa) */
API_IMPORT INT32 uvCmn_MDR_GetValueCustomGenericParam();
/* Currently set custom axis parameter (cmd : _cax) */
API_IMPORT INT32 uvCmn_MDR_GetValueCustomAxisParam();
/* Currently set custom station parameter (cmd : _cst) */
API_IMPORT INT32 uvCmn_MDR_GetValueCustomStationParam();
/* Currently set position error limit of all axes (cmd : _el) */
API_IMPORT PINT32 uvCmn_MDR_GetValuePositionErrorLimit();
/* Input state of digital input (cmd : din) */
API_IMPORT UINT32 uvCmn_MDR_GetValueExternalDIInState();
/* Last state set to digital output (cmd : dout) */
API_IMPORT UINT32 uvCmn_MDR_GetValueExternalDIOutState();
/* Currently set delay (cmd : pgd) */
API_IMPORT UINT32 uvCmn_MDR_GetValuePutGetDelayTime();
/* Currently set time out (cmd : vto) */
API_IMPORT UINT32 uvCmn_MDR_GetValueVacuumSearchTimeOut();
/* Currently set maximum wafer thickness (cmd : _mt) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueMaximumWaferThick();
/* Current value of R axis pushing acceleration (cmd : _pa - for default station) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueRAxisPushAccDef();
/* Current value of R axis pushing acceleration (cmd : _pa - for last station) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueRAxisPushAccSta();
/* Current value of R axis pushing speed (cmd : _ps - for default station) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueRAxisPushSpeedDef();
/* Current value of R axis pushing speed (cmd : _ps - for last station) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueRAxisPushSpeedSTA();
/* Current wafer scanning Z axis acceleration for given station (cmd : _sa) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueWaferScanZAxisAcc();
/* Current wafer scanning Z axis speed for given station (cmd : _ss) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueWaferScanZAxisSpeed();
/* Currently set center offset (cmd : cof) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueStationCenterOffset();
/* Currently set center offset (special operation) (cmd : cofp) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueStationCenterOffsetEdgeGrip();
/* Currently Inline Station Retracted R position for the default station (cmd : irr) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueStationRetractedRPosition();
/* Currently Inline Station Retracted T position for the default station (cmd : irt) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueStationRetractedTPosition();
/* Currently Inline Exit Angle for the default station (cmd : isa) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueStationExitAngle();
/* Currently Inline Exit Distance for the default station (cmd : isd) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueStationExitDistance();
/* Currently Inline Station End effector Length for the default station (cmd : ise) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueStationEndEffectLength();
/* Currently set offset (cmd : ofs) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueStationOffsetZ();
/* Currently set pitch (cmd : pit) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueStationPitch();
/* Current value of Start Pushing Distance (cmd : psd) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueStartPushingDistance();
/* Current value of Start Pushing Offset (cmd : pss) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueStationStartPushingOffset();
/* Current value of Z push offset (cmd : psz) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueStationPushingOffsetZ();
/* Currently set pickup acceleration (cmd : pua) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueStationPickupAccelerationZ();
/* Currently set pick-up speed (cmd : pus) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueStationPickupSpeedZ();
/* Currently set position (cmd : rpo) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueStationRetractedPositionR();
/* Currently set maximum wafer thickness for Station (cmd : smt) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueStationWaferMaxThickness();
/*Current Scanning Highest Z Position, for a given or default station  (cmd : szh) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueStationScaningZHighestZCoord();
/* Current Scanning Lowest Z Position, for a given or default station (cmd : szl) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueStationScaningZLowestCoord();
/* Currently set stroke for z axis (cmd : str) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MDR_GetValueStationStrokeZ();
/* Currently set maximum beam thresholds for Global (cmd : _bt) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueBeamThreshold();
/* Currently set maximum beam thresholds for Station (cmd : sbt) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueStationBeamThreshold();
/* Currently set station position (cmd : spon) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueStationPosition();
/* Current Scanning Right Coordinate, for all or given axis (cmd : srp) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueStationScanningRightCoord();
/* Current Scanning Left Coordinate, for all or given axis (cmd : slp) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueStationScanningLeftCoord();
/* Current Scanning Center Coordinate, for all or given axis (cmd : scp) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueStationScanningCenterCoord();
/* Currently set maximum beam thresholds for Station (cmd : sbt) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueStationPerBeamThreshold();
/* Current position of all axes (cmd : cpo) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueAxisCurrentPosition();
/* Currently set integral limit of all axes (cmd : _il) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueAxisIntegralLimit();
/* Currently set proportional coefficient of all axes (cmd : _kp) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueAxisPropCoeff();
/* Currently set integral coefficient of all axes (cmd : _ki) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueAxisInteCoeff();
/* Currently set differential coefficient of all axes (cmd : _kd) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueAxisDiffCoeff();
/* Currently set proportional coefficient of all axes (cmd : _hp) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueAxisPropCoeffHold();
/* Currently set integral limit of all axes (cmd : _hl) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueAxisInteLimitHold();
/* Currently set integral coefficient of all axes (cmd : _hi) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueAxisInteCoeffHold();
/* Currently set position error limit of all axes (cmd : _he) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValuePosErrLimitHold();
/* Currently set differential coefficient of all axes (cmd : _hd) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueAxisDiffCoeffHold();
/* Currently set Reverse safe envelope position for all the axes (cmd : _rse) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueAxisSafeEnvelopReverse();
/* Currently set forward safe envelope position for all the axes (cmd : _fse) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueAxisSafeEnvelopForward();
/* Currently set axis speed (cmd : spd) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueAxisWorkingSpeed();
/* Currently set loaded speed (cmd : lspd) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueAxisLoadedSpeed();
/* Currently set axis deceleration jerk (cmd : ldjk) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueAxisLoadedDecelerationJerk();
/* Currently set axis acceleration jerk (cmd : lajk) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueAxisLoadedAccelerationJerk();
/* Currently set loaded acceleration (cmd : ldcl) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueAxisLoadedDeceleration();
/* Currently set loaded acceleration (cmd : lacl) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueAxisLoadedAcceleration();
/* Currently set axis deceleration jerk (cmd : djk) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueAxisDecelerationJerk();
/* Currently set axis acceleration jerk (cmd : ajk) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueAxisAccelerationJerk();
/* Currently set axis deceleration (cmd : dcl) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueAxisDeceleration();
/* Currently set axis acceleration (cmd : acl) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueAxisAcceleration();
/* Currently offset compensation with slot (cmd : _zc) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueCoordCompensationSlot();
/* Currently set Reverse direction software limits for all the axes (cmd : _rl) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueSWNegativeLimit();
/* Currently set forward direction software limit for all the axes (cmd : _fl) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueSWPositiveLimit();
/* Currently set encoder zero offsets of all the axes (cmd : _eor) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueEncZeroOffset();
/* Currently set customized home position of all the axes (cmd : _ch) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MDR_GetValueHomedPosition();

#ifdef __cplusplus
}
#endif