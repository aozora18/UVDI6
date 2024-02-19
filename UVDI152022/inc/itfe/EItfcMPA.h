
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
/*                 External Interface - PreAligner for Logosol < for Engine >                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Check whether Logosol Prealigner is currently connected
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MPA_IsConnected();
/*
 desc : MPA 가 초기화 완료 되었는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_IsInitialized();
/*
 desc : Initialize existing communication-related send/receive data
 parm : None
 retn : None
*/
API_IMPORT VOID uvEng_MPA_ResetCommData();
/*
 desc : Returns the response status value for the most recently sent command
 parm : None
 retn : 0x00 - FAIL, 0x01 - SUCC, 0xff - Not defined (Not received)
*/
API_IMPORT ENG_MSAS uvEng_MPA_GetLastSendAckStatus();
/*
 desc : Last received command size (unit: Bytes)
 parm : None
 retn : Last received command size (unit: Bytes)
*/
API_IMPORT UINT32 uvEng_MPA_GetRecvSize();
/*
 desc : This command sets or returns the mode of the firmware response after the end of a macro execution. (cmd : inf)
		The different modes of operation are intended for synchronizing different host software implementations
 parm : mode	- [in]  Mode 0 is designed for synchronous mode of operation. In this mode, the communication protocol is
								strictly master-slave – the host sends a command, then the firmware returns an answer, and then the
								host sends the next command.
						Mode 1 is intended for asynchronous mode of operation. After the end of a macro execution, the
								firmware sends the system status word to the host in hexadecimal format and a prompt. The prompt
								can be either „>” for successfully executed macro or „?” for nonsuccessfully executed or interrupted
								macro.
						Mode 2 is similar to mode 1. The difference is that the firmware sends only a prompt.
						Mode 3 is similar to mode 1. Instead of status word, the firmware sends to the host a number of
								text messages by decoding the status. This is useful for diagnostic when the host is running just a
								terminal emulator.
 retn : TRUE or FALSE
 note : 
 exam : >INF 1
		>STA
		000C
		>INF 3
		>STA
		Chuck vacuum switch activated
		Chuck vacuum sensor activated
		>
*/
API_IMPORT BOOL uvEng_MPA_SetInfoMode(/*Internal : 0x01*/);
API_IMPORT BOOL uvEng_MPA_GetInfoMode();
/*
 desc : This command displays the current axis position (cmd : mcpo)
 parm : None
 retn : TRUE or FALSE
 note : Current position of all axes
 exam : >MCPO R
		1000
		>MCPO
		900,1000,2000
*/
API_IMPORT BOOL uvEng_MPA_GetAxisPos();
/*
 desc : This command displays the current status of the prealigner
		or the status at the time of last motion or system error. (cmd : )
 parm : Only "DIAG C" applied
		DIAG S displays the saved status at the time of last error.
		DIAG C displays the current status.
		DIAG displays both the saved status at the time of last error and the current status.
 retn : TRUE or FALSE
 note : It is intended to be used by an operator since it offers a humanfriendly description of the status.
 exam : >DIAG
		LAST FAULT Status:
		Stop Reason -> AxisT: PositionERROR,
		AxisT		-> PositionERROR, MUMS,
		AxisR		-> Stopped, MUMS,
		AxisZ		-> Stopped, MUMS,
		Current State:
		SystemSTATE -> ACTIVE
		AxisT		-> ServoON, MUMS,
		AxisR		-> ServoON, HomeR OFF, MUMS,
		AxisZ		-> ServoON, HomeZ ON, MUMS,
*/
API_IMPORT BOOL uvEng_MPA_GetDiagState();
/*
 desc : Detailed error description of last error (cmd : est)
 parm : None
 retn : TRUE or FALSE
 note : Whenever an error condition occurs during execution of a motion command (e.g. syntax error,
		software motion limit is violated, obstacle is hit, E-Stop/Emergency Stop circuit connected to the
		device Enable/Stop input has been activated, etc.), control is passed to a special part of the handling
		firmware which elaborates the error condition
 exam : >EST
		Error: EC=-91 EA=2 RC=2003191D EL=4666 at 2003-12-10 19:17:51
		STA(axis): T=00080400 T=00080400 Z=000804A0
		HWS(devic): T=1D09 R=1D09 Z=191D IO=0880
		POS(units): T=0 R=0 Z=5
		ERR(enc.c): T=0 R=0 Z=-250
		ENC(enc.c): T=0 R=0 R=0 Z=51
		>
*/
API_IMPORT BOOL uvEng_MPA_GetMotionState();
/*
 desc : Description of previous error (cmd : per)
 parm : None
 retn : TRUE or FALSE
 note : If a command fails with Bit 2 of the status word set, PER command gives more details about the reason.
 code : 0001	Wrong wafer size
		0004	Vacuum not sensed on chuck
		0005	Data Capture error
		0008	Data not available
		0009	Wafer lost during align operation
		0010	Alignment algorithm interrupted by host
		001F	Alignment algorithm failed: Data not received on time
		0020	R-axis home switch position error
		0021	Z-axis home switch position error
		002F	Alignment algorithm failed: Index not received on time
		004F	Alignment algorithm failed: Measurement speed not reached on time
		008F	Alignment algorithm failed: Synchronization error
		010F	Alignment algorithm failed: Notch/Flat error
		020F	Alignment algorithm failed: Notch/Flat not found
		040F	Alignment algorithm failed: Calculated offset out of limits
		080F	Alignment algorithm failed: Invalid CCD sensor data
		100F	Alignment algorithm failed: Invalid parameters
		200F	Alignment algorithm failed: Cannot align within required offset
		400F	Alignment algorithm failed: Cannot align within required notch angle
 exam : >INF
		1
		>BAL
		>0002
		?PER
		0004
		>INF 3
		>PER
		No vacuum on chuck
		>
*/
API_IMPORT BOOL uvEng_MPA_GetErrorReason();
/*
 desc : All prealigner axes home switch state (cmd : rhs)
 parm : None
 retn : TRUE or FALSE
 note : This command returns the state of the home switches and limit switches.
		The result is 2 hexadecimal bytes.
		For each axis, there are 4 bits showing the axis switches state.
		Bits 0 to 3 are always zero, bits 4 to 7 show the state of R, and bits 8 to 11 show the state of Z.
		The format is 0x0ZRT

		0010	means R-axis home switch is active.
		0100	means Z-axis home switch is active.

 exam : >RHS
		0010
		>
*/
API_IMPORT BOOL uvEng_MPA_GetAllSwitchState();
/*
 desc : System status word (cmd : st)
 parm : None
 retn : TRUE or FALSE
 note : This command displays the general status of the controller
 code : 0 0001 Unable to execute motion commands
		0002	Previous command returned an error.
				See PER command for details
		0004	Chuck Vacuum Sensor ON
		0008	Chuck Vacuum Switch ON
		0010	Motion error
		0020	Motion limits violation
		0040	Home procedure not executed
		0080	User macro running
		0100	Motion in progress
		0200	Reserved
		0400	Servo OFF on one or more axes
		0800	Reserved
		1000	Reserved
		2000	I/O File error
		4000	Reserved
		8000	Pins Vacuum Switch ON
 exam : >INF 0
		>STA
		000C
		>INF 3
		>STA
		Chuck vacuum switch activated
		Chuck vacuum sensor activated
		>
*/
API_IMPORT BOOL uvEng_MPA_GetSystemStatus();
/*
 desc :  The version number of the firmware. Optional parameter causes version information
		for different modules to be displayed as well (cmd : ver)
 parm : Only "VER S" applied
 retn : TRUE or FALSE
 note : This command returns the firmware version number, and can return version numbers of different firmware modules
 exam : >VER S
		MCL Code Version 2.0.1.6
		System-1.3.2.9 PreAl-1.1.1.37 Calib-1.1.1.18 PA-B1.2.1.8
		>
*/
API_IMPORT BOOL uvEng_MPA_GetVerModuleAll();
/*
 desc : Set/Display Customized Home Position (cmd : _ch)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		value	- [in]  customized home position or angle of Axis (unit: R/Z: um, T: degree)
 retn : TRUE or FALSE
 note : Currently set customized home position of all the axes.
		This command specifies the position along the T, R, and Z axes the prealigner will go to after command MTH.
 exam : >_CH
		0,0,1000
		>_CH R 1500
		>_CH
		0,1500,1000
		>MCPO
		0,0,500
		>_CH Z
		>_CH
		0,1500,500
		>
*/
API_IMPORT BOOL uvEng_MPA_GetAxisHomePosition();
API_IMPORT BOOL uvEng_MPA_SetAxisHomePosition(ENG_PANC axis, DOUBLE value);
/*
 desc : sets the customized home position of Axis to this axis’ current position (cmd : _ch)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
 retn : TRUE or FALSE
 */
API_IMPORT BOOL uvEng_MPA_SetAxisCurHomePosition(ENG_PANC axis);
/*
 desc : Set/Display Axis Current Limit (cmd : _cl)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		value	- [in]  current limit of Axis (1 ~ 251)
 retn : TRUE or FALSE
 note : Currently set current limit of all axes
		This axis parameter limits the current to the axis (motor). It is very important for the motor performance.
 exam : >_CL
		251,121,121
		>_CL R 101
		>
*/
API_IMPORT BOOL uvEng_MPA_GetAxisCurrentLimit();
API_IMPORT BOOL uvEng_MPA_SetAxisCurrentLimit(ENG_PANC axis, UINT8 value);
/*
 desc : Set/Display Axis Position Error Limit (cmd : _el)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		value	- [in]  Axis에 따른 Position Limit Error 값 (1 ~ 65535)
 retn : TRUE or FALSE
 note : Currently set position error limit of all axes.
		This axis parameter limits the position error. If the position error exceeds the limit, the firmware will stop all the axes
 exam : >_EL
		400,100,100
		>_EL R 200
		>_EL
		400,200,100
		>
*/
API_IMPORT BOOL uvEng_MPA_GetAxisPositionLimitError();
API_IMPORT BOOL uvEng_MPA_SetAxisPositionLimitError(ENG_PANC axis, UINT16 value);
/*
 desc : Set/Display Axis Reverse Direction Software Limit (cmd : _rl)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		direct	- [in]  Axis Direction (forward, backward (reverse))
		value	- [in]  numerical value – Reverse direction software limit for Axis
						(unit: R/Z: um, T: degree)
 retn : TRUE or FALSE
 note : Currently set Reverse direction software limit for all the axes.
		The firmware monitors the current position of all the axes.
		If an axis position exceeds its reverse direction software limit,
		the firmware stops the motion of all the axes, and sets Bit 5 (0x0020) of status word on.
 exam : >_RL
		0,-2410,-10
		>_RL R -2400
		>_RL
		0,-2400,-10
		>MCPO
		0,-2405,0
		>STA
		0020
		>_RL R
		>_RL
		0,-2405,-10
		>
*/
API_IMPORT BOOL uvEng_MPA_GetAxisDirectLimit(ENG_MAMD direct);
API_IMPORT BOOL uvEng_MPA_SetAxisDirectLimit(ENG_PANC axis, ENG_MAMD direct, DOUBLE value);
/*
 desc : Set/Display home offset (cmd : _ho)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		value	- [in]  home offset of Axis (unit: R/Z: um, T: degree)
						_HO Axis Value	sets the home offset of Axis to Value.
						_HO Axis		sets the home offset of Axis to its current position.
						_HO				returns the home offsets of all the axes.
 retn : TRUE or FALSE
 note : Currently set home offset of all the axes.
		This command sets or displays a particular axis, or all axes home offsets.
		The home offset specifies the distance between the first index pulse from the home switch and the
		origin of the axis coordinate system.
 exam : >_HO
		0,130,0
		>_HO R 120
		Home offset is set to 120
		>_HO
		0,120,0
		>MCPO
		0,125,500
		>_HO R
		Home offset is set to 125
		>_HO
		0,125,0
		>
*/
API_IMPORT BOOL uvEng_MPA_GetAxisHomeOffset();
API_IMPORT BOOL uvEng_MPA_SetAxisHomeOffset(ENG_PANC axis, DOUBLE value);
API_IMPORT BOOL uvEng_MPA_SetAxisCurHomeOffset(ENG_PANC axis);
/*
 desc : Set/Display Axis Acceleration (cmd : acl or dcl)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		type	- [in]  0x00: Acceleration, 0x01: Deceleration
		value	- [in]  acceleration of Axis (unit: R/Z: um / sec^2, T: degree / sec^2)
 retn : TRUE or FALSE
 note : Currently assigned parameter set to the current wafer size
		This command sets or returns the acceleration of an axis.
 exam : >ACL T 48000
		>ACL R 10000
		>ACL Z 10000
		>ACL
		48000,10000,10000
		>
*/
API_IMPORT BOOL uvEng_MPA_GetAxisInstSpeed(ENG_MAST type);
API_IMPORT BOOL uvEng_MPA_SetAxisInstSpeed(ENG_PANC axis, ENG_MAST type, DOUBLE value);
/*
 desc : Set/Display Axis Acceleration or Deceleration Jerk (cmd : ajk or djk)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		type	- [in]  0x00: Acceleration, 0x01: Deceleration
		value	- [in]  acceleration of Axis (unit: R/Z: um / sec^2, T: degree / sec^2)
 retn : TRUE or FALSE
 note : Currently set axis acceleration jerk.
		This command sets or returns the acceleration jerk of an axis.
 exam : >AJK T 600
		>AJK R 800
		>AJK Z 500
		>AJK
		600,800,500
		>
*/
API_IMPORT BOOL uvEng_MPA_GetAxisJerkSpeed(ENG_MAST type);
API_IMPORT BOOL uvEng_MPA_SetAxisJerkSpeed(ENG_PANC axis, ENG_MAST type, DOUBLE value);
/*
 desc : Set/Display Axis Velocity Profile Type (cmd : pro)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		profile	- [in]  new velocity profile for Axis (0, 1, 2, 3, 9 => one)
 retn : TRUE or FALSE
 note : Currently set axis profile type.
		This command sets or returns the velocity profile type of all axes.
		Valid velocity profiles are as follows:
		0	Trapezoidal velocity profile
		1	Bezier curve velocity profile
		2	Spline curve velocity profile
		3	S-Curved velocity profile
		9	Device velocity profile
 exam : >PRO T
		9
		>
*/
API_IMPORT BOOL uvEng_MPA_GetVelocityProfile();
API_IMPORT BOOL uvEng_MPA_SetVelocityProfile(ENG_PANC axis, UINT8 profile);
/*
 desc : Set/Display Axis Speed   (cmd : spd)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		speed	- [in]  Speed Value (unit: R/Z: um / sec^2, T: degree / sec^2)
 retn : TRUE or FALSE
 note : Currently set speed for all or one axis.
		This command sets or returns the speed of an axis.
 exam : >SPD T 4800
		>SPD R 500
		>SPD Z 500
		>SPD
		4800,500,500
*/
API_IMPORT BOOL uvEng_MPA_GetAxisSpeed();
API_IMPORT BOOL uvEng_MPA_SetAxisSpeed(ENG_PANC axis, DOUBLE speed);
/*
 desc : Start homing the prealigner (cmd : hom)
 parm : None
 retn : TRUE or FALSE
 note : This command starts initializing (homing, also called “referencing”) all prealigner axes to their
		home positions, as defined by the _CH command
		For radial axis (R), this is usually the zero position.
		Vertical axis position after homimg is complete depends on LTC parameter. Edge-handling units will
		move theta (chuck) axis to the home position as well.
 exam : >_LU 4475
		>LTC 1
		>INF 1
		>SON
		>HOM
		...0000
		>MCPO
		0,0,4475
*/
API_IMPORT BOOL uvEng_MPA_SetAxisAllHoming();
/*
 desc : Move to Home position (cmd : mth)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
 retn : TRUE or FALSE
 note : This command starts moving the specified axis or all axes to their home position
 exam : >MTH R
		>MTH
		…0000
		>MCPO
		0,0,0
		>
*/
API_IMPORT BOOL uvEng_MPA_MoveHomePosition(ENG_PANC axis);
/*
 desc : Move Axis to Absolute (or Relative) Position (cmd : mva or mvr)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		type	- [in]  Absolute move (0x00), Relative move (0x01)
		value	- [in]  Target position or angle (um or degree)
 retn : TRUE or FALSE
 note : This command starts moving the Axis to the specified target Position defined as an absolute
		coordinate of the axis.
 exam : >MCPO R
		1000
		>MVA R 1400
		…0000
		>MCPO R
		1400
		>

		>MCPO R
		1400
		>MVR R 300
		…0000
		>MCPO R
		1700
		>
*/
API_IMPORT BOOL uvEng_MPA_MoveToAxis(ENG_PANC axis, ENG_PAMT type, DOUBLE value);
/*
 desc : Turn axis servo off or on (cmd : son or soff)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		type	- [in]  Servo Control Operation OFF (0x00) or ON (0x01)
 retn : TRUE or FALSE
 note : This command turns servo control off for the specified axis. If the axis is not specified, the command
		will turn off (or on) the servo of all axes
		This command will not be accepted if a macro is running
 exam : >SOF R
		>SOF T
		>SOF
		>

		>SOF R
		>SOF T
		>SOF
		>
*/
API_IMPORT BOOL uvEng_MPA_ServoControl(ENG_PANC axis, ENG_PSOO type);
/*
 desc : Stop Motion, Terminate Execution (cmd : stp)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical)), 3: All
 retn : TRUE or FALSE
 note : This command stops the motion of the specified axis or all axes with the defined deceleration
 exam : >STP R
		>STP
		>
*/
API_IMPORT BOOL uvEng_MPA_StopToAxis(ENG_PANC axis);
/*
 desc : Wait for Motion Completion (cmd : wmc)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
 retn : TRUE or FALSE
 note : This command waits until all submitted macros and procedures complete their execution, and
		specified (or all) axes stop moving.
 exam : >MVR R 500
		>WMC
		………
		>
*/
API_IMPORT BOOL uvEng_MPA_WaitPrealigner(ENG_PANC axis);
/*
 desc : Turn off (or on) the chuck vacuum (cmd : mcvf or mcvn)
 parm : type	- [in]  Control Operation (OFF:0x00, ON:0x01)
 retn : TRUE or FALSE
 note : This command closes the chuck vacuum valve to release the wafer from the chuck
 exam : >MCVF
		>
*/
API_IMPORT BOOL uvEng_MPA_ChuckVacuum(ENG_PSOO type);
/*
 desc : Turn off (or on) the pins vacuum (cmd : mpvf or mpvn)
 parm : type	- [in]  Control Operation 즉, OFF (0x00) or ON (0x01)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MPA_PinsVacuum(ENG_PSOO type);
/*
 desc : Start alignment (cmd : bal)
 parm : max_offset	- [in]  Alignment Offset (unit: um)
		offset_try	- [in]  Offset Retry Count
		max_angle	- [in]  Notch Angle (unit: degree)
		angle_try	- [in]  Angle Retry Count
		orient_notch- [in]  Whether the Orient Notch to apply (0 or 1)
 retn : TRUE or FALSE
 note : This command starts a procedure which executes one cycle of the wafer aligning process.
		If MaxOffset, OffsetRetries, MaxAngle and AngleRetries parameters are specified, then the
		alignment process is repeated up to OffsetRetries times until measured Wafer Offset is less than
		or equal to MaxOffset, and then up to AngleRetries times until measured Notch Angle is within
		MaxAngle range from requested notch angle

		BAL 0	rotates the wafer, measures the angle and notch offset, then returns the wafer back to the
				original position. This command can be used to verify the results of the previous alignment.
		BAL 2	uses an additional sampling of the wafer edge after compensating for the offset. After the
				second sampling the firmware calculates only the angle of the notch. This method of aligning the
				wafer is slower but always has better precision of the notch angle.
		BAL 3, BAL 4, and BAL 5 also use additional sampling of the wafer edge after compensating for
				the offset. Before the second sampling, the firmware rotates the chuck below the pins so the notch is
				always positioned the same way on the chuck. After the second sampling, the firmware calculates
				only the angle of the notch. This method of aligning the wafer is slower but has much better precision
				of the notch angle.
		BAL 3	aligns the notch and the chuck as a separate motion sequence.
		BAL 4	aligns the notch and the chuck during offset compensation with higher speed and acceleration.
		BAL 5	aligns the notch and the chuck during offset compensation with higher speed and acceleration
				and orients the notch to the requested angle before the second sampling.
 exam : >BAL
		………
		>0000
*/
API_IMPORT BOOL uvEng_MPA_FindNotchAngle(DOUBLE max_offset=0.0f/*0.0*/, UINT8 offset_try=0.0f/*0*/,
										 DOUBLE max_angle=0.0f/*0.0*/, UINT8 angle_try=0.0f/*0*/, UINT8 orient_notch=0.0f/*0*/);
API_IMPORT BOOL uvEng_MPA_FindNotchAngleExt(UINT8 mode);
/*
 desc : Display CCD Sensor Value (cmd : ccd)
 parm : rate	- [in]  number of samples to read (default is 10).
 retn : TRUE or FALSE
 note : Current value of CCD sensor
		This command displays the current CCD sensor value for the current wafer size. If the sensor is
		completely uncovered (e.g. there’s no wafer), the returned value will be 0.
		If the sensor is completely covered, the returned value will be close to 4890.
 exam : >CCD
		2450
*/
API_IMPORT BOOL uvEng_MPA_GetCCDSensorPixels();
API_IMPORT BOOL uvEng_MPA_SetCCDSensorPixels(UINT16 value);
/*
 desc : Detect an Object Using CCD Sensors (cmd : doc)
 parm : None
 retn : TRUE or FALSE
 note : Information about whether and object is covering any of the CCD sensors.
		This command checks all possible CCD sensors for an object.
		DOC 0 (mode) 1 (ReturnValue)	returns 0 if any one of the sensors is covered and 1 otherwise.
		DOC 0 (mode) 0 (ReturnValue)	returns 1 if any one of the sensors is covered and 0 otherwise.
		DOC 0 (mode)					returns 1 if any one of the sensors is covered and 0 otherwise.
		DOC 1 (mode)					returns the wafer size and the CCD value of each sensor, which detects an object.
		DOC								returns 1 if any of the sensors are covered and 0 otherwise. (This item currently applies)
 exam : >DOC 0
		1
		>DOC 0 1
		0
		>DOC 1
		8,2430
		>
		No wafer Example:
		>DOC 0
		0
		>DOC 0 1
		1
		>DOC 1
		>
*/
API_IMPORT BOOL uvEng_MPA_CheckWaferLoaded();
/*
 desc : Set/Display Final Wafer Orientation (cmd : fwo)
 parm : angle	- [in]  Rotation Angle (degree)
 retn : TRUE or FALSE
 note : Currently set final wafer orientation angle.
		This command defines the angular orientation of the notch or flat upon completion of successful wafer alignment.
 exam : >FWO
		1800
		>FWO 900
		>FWO
		900
		>
*/
API_IMPORT BOOL uvEng_MPA_SetRotationAfterAlign(DOUBLE angle);
API_IMPORT BOOL uvEng_MPA_GetRotationAfterAlign();
/*
 desc : Set/Display Wafer Loading Type (cmd : ltc)
 parm : type	- [in]  PINS (0x00), CHCUK(0x01)
 retn : TRUE or FALSE
 note : Currently set wafer loading type.
		This command sets or displays the selected type of wafer loading and unloading – on chuck or on pins.
 exam : >LTC
		1
		>LTC 0
		>LTC
		0
		>
*/
API_IMPORT BOOL uvEng_MPA_SetWaferPlaceType(ENG_PWPT type);
API_IMPORT BOOL uvEng_MPA_GetWaferPlaceType();
/*
 desc : Display Number of Bad (Good) Notches (cmd : nbn or ngn)
 parm : type	- [in]  Bad (Fail) (0x00), Good (Succ) (0x01)
 retn : TRUE or FALSE
 note : Number of notches falling within minimum/maximum depth/width criteria.
		When aligning wafers with damaged rim, some of defects might appear as a notch. This command
		returns the number of notches which met the criteria defined with _MND, _MXD, _MNW and _MXW commands.
 exam : >BAL
		…0000
		>NBN
		4
		>

		>BAL
		…0000
		>NGN
		4
		>
*/
API_IMPORT BOOL uvEng_MPA_GetWaferNotchCount(ENG_PASR type);
/*
 desc : Display Depth and Width of a Notch (cmd : ndw)
 parm : index	- [in]  numerical value from 0 to total number of notches (-1 : Total)
 retn : TRUE or FALSE
 note : Two numbers, denoting depth and width of requested (or first) notch.
		When aligning wafers with damaged rim, some of defects might appear as a notch. This
		command returns the depth and width of notches found. Notches are ordered as follows:
		first good notches (up to the number reported by NGN command), than bad notches (up to
		the number reported by NBN command).
 exam : >_MND 35
		>_MXD 50
		>_MNW 10
		>_MXW 20
		>BAL
		...000C
		>NGN
		1
		>NBN
		4
		>NDW 0
		45,13
		>NDW 1
		57,7
		>NDW 2
		58,7
		>NDW 3
		59,7
		>NDW 4
		38,7
		>
*/
API_IMPORT BOOL uvEng_MPA_GetWaferAlignDepthWidth(INT16 index);
/*
 desc : Set/Display minimum (or maximum) notch depth (or width) (cmd : _mnd, _mxd, _mnw, _mxw)
 parm : type	- [in] 0x00: Depth, 0x01: Width
		kind	- [in] 0x00: Min, 0x01: Max
		range	- [in] Range Value (_mnd (Relative), _mxd (Internal units), _mnw (Relative), _mxw (Internal units))
 retn : TRUE or FALSE
 note : Currently set minimum (or maximum) notch depth (or width)
		When aligning wafers with damaged rim, some of defects might appear as a notch.
		This command defines minimum  (or maximum) depth (or width) of notch.
 exam : >_MND
		0
		>_MND 25
		>

		>_MNW
		0
		>_MNW 10
		>

		>_MXD
		0
		>_MXD 40
		>

		>_MXW
		0
		>_MXW 20
		>
*/
API_IMPORT BOOL uvEng_MPA_GetWaferFindNotchRange(ENG_PNTI type, UINT8 kind);
API_IMPORT BOOL uvEng_MPA_SetWaferFindNotchRange(ENG_PNTI type, UINT8 kind, UINT32 range);
/*
 desc :Set/Display minimum good data samples. (cmd : _mgd)
 parm : samples	- [in] Integer number in the range of 10 to 2500
 retn : TRUE or FALSE
 note : Currently set minimum good data samples
		This command adjusts the minimum number of “good” CCD data samples, which the firmware accepts.
		Reducing this value can allow calculating the displacement of a wafer with a relatively big flat.
		If the CCD samples from the flat are eliminated using _MDIFF command, the _MGD value should
		be reduced so the remaining number of samples is greater than the limit.
		Note that using a very small number of samples could affect the prealigner accuracy and repeatability.
 exam : >_MGD
		2500
		>_MGD 200
		>
*/
API_IMPORT BOOL uvEng_MPA_GetMinCCDSampleCount();
API_IMPORT BOOL uvEng_MPA_SetMinCCDSampleCount(UINT16 samples);
/*
 desc : Set/Display Maximum (or Minimum) Wafer Size (cmd : _maxwsz or _minwsz)
 parm : type	- [in]  Min / Max 구분 값
		kind	- [in]  maximum (or minimum) wafer size in inches (2, 3, 4, 5, 6, 8, 12, 18 : select one of values)
						50mm(2″), 75mm(3″), 100mm(4″), 125mm(5″), 150mm(6″),
						200mm(8″), 300mm(12″), 450mm(18″)
 retn : TRUE or FALSE
 note : Currently set/display maximum/minimum wafer size.
 exam : >_MAXWSZ
		8
		>WSZ 6
		>WSZ 12
		?

		>_MINWSZ
		8
		>WSZ 8
		>WSZ 6
		?
*/
API_IMPORT BOOL uvEng_MPA_SetWaferMinMaxSize(ENG_MMMR type, ENG_MWKT kind);
API_IMPORT BOOL uvEng_MPA_GetWaferMinMaxSize(ENG_MMMR type);
/*
 desc : Set/Display Wafer Size (cmd : wsz)
 parm : kind	- [in]  Size of the wafer in inches or millimeters (2, 3, 4, 5, 6, 8, 12, 18 : select one of values)
						50mm(2″), 75mm(3″), 100mm(4″), 125mm(5″), 150mm(6″),
						200mm(8″), 300mm(12″), 450mm(18″)
 retn : TRUE or FALSE
 note : Currently set/display wafer diameter.
		If an invalid wafer size has been specified for the prealigner (e.g. requested wafer size of 300mm for
			unit capable of 200mm max), an error is returned.
 exam : >WSZ
		8
		>WSZ 6
		>WSZ
		6
		>WSZ 200
		>WSZ
		8
		>WSZ 12
		?
*/
API_IMPORT BOOL uvEng_MPA_SetWaferWorkSize(ENG_MWKT kind);
API_IMPORT BOOL uvEng_MPA_GetWaferWorkSize();
/*
 desc : Set/Display Wafer Size Light Source Select (cmd : wsz2)
 parm : light	- [in]  0: Light Source (1st), 1: Light Source (2nd)
 retn : TRUE or FALSE
 note : Currently set/display value.
		Some prealigners have two different light sources per wafer size. For example, LPA1218 has two light
		sources for the 12 inch sensor – for 300 and 310 mm and two light sources for the 18 inch sensor – for
		450 and 460 mm wafers. This command sets or reports the selected light source.
		If an invalid wafer size has been specified for the prealigner (e.g. requested wafer size of 300mm for
		unit capable of 200mm max), an error is returned.
 exam : >WSZ
		12
		>WSZ2
		0
		>
		>WSZ2 1
		>
*/
API_IMPORT BOOL uvEng_MPA_SetWaferWorkLight(UINT8 light);
API_IMPORT BOOL uvEng_MPA_GetWaferWorkLight();
/*
 desc : Read wafer information (cmd : mrwi)
 parm : None
 retn : TRUE or FALSE
 note : Displacement (offset), offset angle, and notch/flat angle.
		This command returns:
		• The distance between the chuck center and wafer center in 0.0001”
		• The angle at which the offset is to be compensated
		• The angle between R-axis and the notch/flat in
		MRWI reports angular values in units of 0.1°;
		MRWI 1 reports angular values in units of 0.01°;
		MRWI 2 reports angular values in encoder counts.
 exam : >MRWI
		2,1240,900
		>
*/
API_IMPORT BOOL uvEng_MPA_GetWaferCenterOrientation();
/*
 desc : Set/Display automatic wafer Size detection. (cmd : _asz)
 parm : type	- [in]  Automatic wafer size detection mode (0x00: Enabled, 0x01: Disabled)
 retn : TRUE or FALSE
 note : Currently set mode.
		This parameter toggles automatic wafer size detection on and off. If this option is set to 1, the
		prealigner checks all CCD sensors before alignment in order to determine the size of the substrate
		being aligned. This mode is useful when the host does not (or cannot) specify the wafer size in advance.
 exam : >_ASZ
		0
		>_ASZ 1
		>BAL
		>
*/
API_IMPORT BOOL uvEng_MPA_SetAutoDetectWaferSize(ENG_PWSD type);
API_IMPORT BOOL uvEng_MPA_GetAutoDetectWaferSize();
/*
 desc : Turn On or Off Second Scan in Case of Alignment Failure (cmd : _autoscanagain)
 parm : type	- [in]  “Auto scan again” mode (0x00:Disabled, 0x01: Enabled)
 retn : TRUE or FALSE
 note : Currently set mode.
		This command instructs the prealigner to make an additional scan of the wafer edge if the alignment fails.
		Set this parameter to 1 to enable.
 exam : >_AUTOSCANAGAIN
		0
		>_AUTOSCANAGAIN 1
		>BAL
		>
*/
API_IMPORT BOOL uvEng_MPA_SetAutoScanAgain(ENG_MFED type);
API_IMPORT BOOL uvEng_MPA_GetAutoScanAgain();
/*
 desc : Set/Display vacuum on chuck after BAL state. (cmd : _finalvacc)
 parm : type	- [in]  Vacuum On (0x01) or Off (0x00)
 retn : TRUE or FALSE
 note : Currently set T-axis fast speed.
		This parameter sets the state of chuck vacuum after BAL when the wafer needs to be on the chuck (LTC 1).
		If set to 1, the firmware will turn ON the chuck vacuum at the end of the alignment.
		If set to 0, the chuck vacuum will be OFF.
 exam : >_FINALVACC
		1
		>BAL
		>000C
		>_FINALVACC 0
		>BAL
		>0000
		>
*/
API_IMPORT BOOL uvEng_MPA_SetVacuumSetAfterBAL(ENG_PSOO type);
API_IMPORT BOOL uvEng_MPA_GetVacuumSetAfterBAL();
/*
 desc : Set/Display Glass Mode (cmd : mglm)
 parm : type	- [in]  Transparent (0x01) or Non-transparent (0x00)
 retn : TRUE or FALSE
 note : Currently setting of MGLM.
		This command adjusts the CCD sensor sensitivity for transparent objects.
		MGLM 0	adjusts the CCD sensor for non-transparent objects.
		MGLM 1	adjusts the CCD sensor for transparent or semi-transparent objects.
		MGLM	returns the current setting.
 exam : >MGLM
		0
		>MGLM 1
		>
*/
API_IMPORT BOOL uvEng_MPA_SetWaferGlassType(ENG_PWGO type);
API_IMPORT BOOL uvEng_MPA_GetWaferGlassType();
/*
 desc : Set/Display load down (or up) position (cmd : _ld or _lu)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		type	- [in]  0x00: Loading Down Position, 0x01: Loading Up Position
		pos		- [in]  Position 값 (unit: um)
 retn : TRUE or FALSE
 note : Currently set load down (or up) position for all axes.
		This command sets/retrieves the position of the specified axis when loading a wafer if loading on
		pins is selected (see LTC command).
		_LD Axis Value	sets the load down position for Axis to Value
		_LD Axis		returns the load down position for specified axis
		_LD				returns the load down position for all axes

		_LU Axis Value	sets the load up position for Axis to Value
		_LU Value		returns the load up position for specifies axis
		_LU				returns the load up position for all axes
 exam : >_LD
		0,0,200
		>_LD Z 180
		>_LD Z
		180
		>

		>_LU
		0,0,3800
		>_LU Z 3900
		>_LU Z
		3900
		>
*/
API_IMPORT BOOL uvEng_MPA_GetWaferLoadPosition(ENG_PWLP type);
API_IMPORT BOOL uvEng_MPA_SetWaferLoadPosition(ENG_PANC axis, ENG_PWLP type, DOUBLE pos);
/*
 desc : Set/Display measure acceleration (or deceleration) (cmd : _ma or _md)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		type	- [in]  0x00: Acceleration, 0x01: Deceleration
		value	- [in]  deceleration of the specified axis during measurement (um/sec^2 or degree/sec^2)
 retn : TRUE or FALSE
 note : Currently set measure acceleration (or deceleration).
		This command sets or displays the acceleration of T-axis during measurement phase and of R, Z axes
		during the wafer transfer between the chuck and the pins.
 exam : >_MA
		48000,20000,20000
		>_MA T 11000
		>_MA T
		11000
		>

		>_MD
		48000,20000,20000
		>_MD T 11000
		>_MD T
		11000
		>
*/
API_IMPORT BOOL uvEng_MPA_GetMeasureAccDcl(ENG_MAST type);
API_IMPORT BOOL uvEng_MPA_SetMeasureAccDcl(ENG_PANC axis, ENG_MAST type, DOUBLE value);
/*
 desc : Set/Display measure speed (cmd : _ms)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		value	- [in]  speed of the specified axis during measurement (um/sec or degree/sec)
 retn : TRUE or FALSE
 note : Currently set measuring speed.
		This command sets or displays the speed of T-axis during measurement phase and of R, Z axes
		during the wafer transfer between the chuck and the pins.
 exam : >_MS
		4800,5000,1000
		>_MS Z 1100
		>_MS Z
		1100
		>
*/
API_IMPORT BOOL uvEng_MPA_GetMeasureSpeedAxis();
API_IMPORT BOOL uvEng_MPA_SetMeasureSpeedAxis(ENG_PANC axis, DOUBLE value);
/*
 desc : Set/Display notch / flat depth calculation mode. (cmd : _notchdepthmode)
 parm : enable	- [in] between 0 and 1. (Whether the mode value is applied to the notch's depth resolution)
 retn : TRUE or FALSE
 note : Currently set notch/flat depth calculation mode.
		Setting _NOTCHDEPTHMODE to 1 gives better than the standard resolution of the depth of the notch and
		is recommended when using notch parameters to distinguish between the notch and possible chips
		on the wafer edge.
 exam : >_NOTCHDEPTHMODE
		0
		>_NOTCHDEPTHMODE 1
		>
*/
API_IMPORT BOOL uvEng_MPA_GetNotchDepthMode();
API_IMPORT BOOL uvEng_MPA_SetNotchDepthMode(bool enable);
/*
 desc : Move the prealigner chuck to measure position. (cmd : mmtm)
 parm : None
 retn : TRUE or FALSE
 note : This command starts moving Z-axis to the Z measuring position
 exam : >_MP
		0,0,4890
		>CPO
		1245,350,4580
		>MTM
		…0000
		>CPO
		1245,350,4890
		>
*/
API_IMPORT BOOL uvEng_MPA_SetWaferAlignmentMeasuringPos();
/*
 desc : Move the prealigner chuck to load position. (cmd : mmtl)
 parm : None
 retn : TRUE or FALSE
 note : This command starts moving the wafer to the loading position
 exam : >CPO
		0,0,4580
		>LTC 0
		>MTL
		…0000
		>CPO
		0,0,0
		>
*/
API_IMPORT BOOL uvEng_MPA_SetWaferLoadingPos();
/*
 desc : Set/Display MTP command mode of operation. (cmd : _mtpmode)
 parm : mode	- [in]  _LD (Load Down Position; 0), _LU (Load Up Position; 1)
 retn : TRUE or FALSE
 note : Currently set MTP command mode.
		This setting specifies the final position of the vertical axis after MTP command.
		In _MTPMODE 0: after MTP command Z-axis is at the Z load down position (_LD).
		In _MTPMODE 1: after MTP command Z-axis is at the transfer down position (_TD).
 exam : >_MTPMODE
		0
		>
*/
API_IMPORT BOOL uvEng_MPA_GetModeZAfterMovePins();
API_IMPORT BOOL uvEng_MPA_SetModeZAfterMovePins(ENG_PWLP mode);
/*
 desc : Set/Display Z-axis coordinate at pins position. (cmd : _pp)
 parm : value	- [in]  Z (Vertical) position of the pins (unit: um)
 retn : TRUE or FALSE
 note : Currently set pins position.
		This command sets or displays the position of the vertical axis at which the chuck is level with the pins.
 exam : >_PP
		1880
		>_PP 1890
		>_PP
		1890
		>
*/
API_IMPORT BOOL uvEng_MPA_GetLevelChcukToPinsPosZ();
API_IMPORT BOOL uvEng_MPA_SetLevelChcukToPinsPosZ(DOUBLE value);
/*
 desc : Set/Display pins position correction when going down (or up). (cmd : _ppcd or _ppcu)
 parm : type	- [in]  0x00: Down Postion Offset, 0x01: Up Position Offset
		value	- [in]  Z (Vertical) pins position offset when moving down (or up) (unit: um)
						Pins Position offset when moving down (or up)
 retn : TRUE or FALSE
 note : Currently set pins position offset.
		This parameter adds an offset to the pins position (_PP command) when transferring the wafer from chuck to pins.
		Specifying this offset improves the reliability of chuck-to-pins transfer of thin bowed wafers.
 exam : >_PPCD
		0
		>_PPCD 55
		>_PPCU 65
		>BAL
		>

		>_PPCU
		0
		>_PPCD 55
		>_PPCU 65
		>BAL
		>
*/
API_IMPORT BOOL uvEng_MPA_GetOffsetChcukToPinsPosZ(ENG_PWLP type);
API_IMPORT BOOL uvEng_MPA_SetOffsetChcukToPinsPosZ(ENG_PWLP type, DOUBLE value);
/*
 desc : Set/Display CCD sensor angle. (cmd : _sa)
 parm : value	- [in]  angle between the CCD and R-axis (degrees)
 retn : TRUE or FALSE
 note : Currently set angle.
		This command sets or displays the angle between the CCD sensor and the R-axis.
 exam : >_SA
		0
		>_SA 400
		>_SA
		400
*/
API_IMPORT BOOL uvEng_MPA_GetAngleRAxisToCCD();
API_IMPORT BOOL uvEng_MPA_SetAngleRAxisToCCD(DOUBLE value);
/*
 desc : Set/Display sample averaging mode. (cmd : _sam)
 parm : enable	- [in]  true : "SAM 3", false : "SAM 0"
 retn : TRUE or FALSE
 note : Currently set averaging mode.
		This command sets or displays the current sample averaging mode. If set to either 3 or 5, scanned
		samples are first averaged before processing. Averaging samples can help in proper alignment of
		wafer with irregular ends. All other numbers are considered as “No average mode”.
 exam : >_SAM
		0
		>_SAM 3
		>
*/
API_IMPORT BOOL uvEng_MPA_GetNotchAverageMode();
API_IMPORT BOOL uvEng_MPA_SetNotchAverageMode(BOOL enable);
/*
 desc : Set/Display CCD sensor center value. (cmd : _sc)
 parm : pixel	- [in]  center value of the CCD sensor (unit: pixel)
 retn : TRUE or FALSE
 note : Currently set CCD center.
		This command sets or displays the CCD sensor center.
 exam : >_SC
		2450
		>_SC 2452
		>_SC
		2452
		>
*/
API_IMPORT BOOL uvEng_MPA_GetCCDSensorCenter();
API_IMPORT BOOL uvEng_MPA_SetCCDSensorCenter(UINT32 pixel);
/*
 desc : Set/Display sampling mode. (cmd : _sm)
 parm : mode	- [in]  sampling mode from 0 to 3. (0, 1, 2, 3)
 retn : TRUE or FALSE
 note : Currently set sampling mode.
		This command sets or displays the current sampling mode. This mode sets number of samples per
		revolution sampled, scanning speed (_MS) and sets internally the value returned by _MS command.
		_SM 2	sets the sampling mode in such a way that the number of samples is equal to the number of
				encoder lines on chuck (T-axis) encoder. This is the default mode of operation.
		_SM 3	sets the number of samples to be twice the number of encoder lines. This mode can be used for
				more precise, but slower alignment.
		_SM 1	sets the number of samples to be half the number of encoder lines. This mode ca be used for
				faster, but less precise alignment.
		_SM 0	sets the number of samples to be one third the number of encoder lines. This mode can be
				used for fastest, but least precise alignment.
		_SM		returns the current sampling mode.
 exam : >_SM 2
		>_NS
		2500
		>_SM 1
		>_NS
		1250
		>
*/
API_IMPORT BOOL uvEng_MPA_GetRotationSampleMode();
API_IMPORT BOOL uvEng_MPA_SetRotationSampleMode(ENG_PRSM mode);
/*
 desc : Set/Display stop on pins mode. (cmd : _sp)
 parm : mode	- [in]  Sampling Mode (0 or 1)
						If “Stop on pins” mode is equal to 1,
							the chuck stops at pins position during wafer transfer from chuck
							to pins and back. At pins position, the pins vacuum is turned on before chuck vacuum is turned off.
						If this mode is set to zero,
							the chuck does not stop at the pins position, and chuck vacuum is turned
							off before pins vacuum is turned on.
 retn : TRUE or FALSE
 note : Currently set „Stop on pins” mode.
		This parameter controls whether chuck motion stops at the pins position during wafer transfer.
 exam : >_SP
		0
		>_SP 1
		>
*/
API_IMPORT BOOL uvEng_MPA_GetStopOnPins();
API_IMPORT BOOL uvEng_MPA_SetStopOnPins(UINT8 mode);
/*
 desc : Set/Display maximum (or minimum) CCD value for square substrates. (cmd : _sqmax or _sqmin)
 parm : mode	- [in]  0x00: Min, 0x01: Max
		area	- [in]  가져올 영역의 크기 (Square 즉, 가로/세로 동일한 크기) (unit: pixels)
 retn : TRUE or FALSE
 note : Currently set maximum (or minimum) CCD value for square substrates.
		This command adjusts the maximum value of CCD data which the firmware accepts as taken from
		the corner of the side of the substrate. Adjusting _SQMAX and _SQMIN values allows calculating the
		offset of squares of different sizes. The general rule is to set these limits so that most of the CCD
		samples are between the limits.
 exam : >_SQMAX
		>4900
		>_SQMAX 3000
		>

		>_SQMIN
		>2500
		>_SQMIN 300
		>
*/
API_IMPORT BOOL uvEng_MPA_GetSuqreValueOfCCD(ENG_MMMR mode);
API_IMPORT BOOL uvEng_MPA_SetSuqreValueOfCCD(ENG_MMMR mode, UINT32 area);
/*
 desc : Set/Display transfer down (or up) position. (cmd : _td or _tu)
 parm : mode	- [in]  0x00: Transfer Down, 0x01: Transfer Up
		value	- [in]  Chuck position (unit: um)
 retn : TRUE or FALSE
 note : Currently set transfer down (or up) position.
		This command sets or displays the position below the pins at which it is safe to move the R-axis so
		the chuck does not interfere with the pins.
 exam : >_TD
		1820
		>_TD 1800
		>_TD
		1800
		>

		>_TU
		1975
		>_TU 2000
		>_TU
		2000
		>
*/
API_IMPORT BOOL uvEng_MPA_GetWaferTranserSafePos(ENG_PWLP mode);
API_IMPORT BOOL uvEng_MPA_SetWaferTranserSafePos(ENG_PWLP mode, DOUBLE value);
/*
 desc : Set/Display timeouts. (cmd : _to)
 parm : param	- [in]  integer number in the range of 0 to 8 - describing the type of the timeout
		value	- [in]  Timeout or Delay (msec)
 retn : TRUE or FALSE
 note : Currently set timeout value.
		This command sets or displays timeouts used during different prealigner operations.

		ParamNo has the following meaning:
			0	Timeout while waiting for vacuum to build on chuck when checking for a wafer.
			1	Timeout while waiting for vacuum to build on pins.
			2	Time to wait after R-axis movement during wafer transfer.
			3	Time to wait after Z-axis movement during wafer transfer.
			4	Time to wait at pins position during “Stop at pins” mode (_SP 1).
			5	Reserved.
			6	Time to wait after MVA, MVR and BAL command have finished before returning prompt.
			7	Delay between switching on pins vacuum and switching off chuck vacuum during transfer from chuck to pins.
			8	Delay between switching on chuck vacuum and switching off pins vacuum during transfer from pins to chuck.
			9	Delay before moving Z-axis to transfer down position during transfer.
			10	Delay before moving Z-axis to transfer down position transfer.
 exam : >_TO 0
		250
		>_TO 0 500
		>
*/
API_IMPORT BOOL uvEng_MPA_GetParamTimeOut(UINT8 param);
API_IMPORT BOOL uvEng_MPA_SetParamTimeOut(UINT8 param, UINT32 value);
/*
 desc : Assign a set of parameters to wafersize. (cmd : _wszautoset)
 parm : set		- [in]  Set Number (-1 or 1 or Later)
		wafer	- [in]  Wafer Size (2, 3, 4, 5, 6, 8, 12, 18)
 retn : TRUE or FALSE
 note : Currently assigned parameter set to the current wafer size.
		This command assigns a set of parameters to be automatically used with the selected wafer size. The
		parameters from the assigned set are loaded into working parameters whenever the wafer size is
		changed. Assigning set -1 to a wafer size deactivates this function for the specified wafer size.
 exam : >_WSZ
		12
		>_WSZAUTOSET 1 12
		>
*/
API_IMPORT BOOL uvEng_MPA_SetAssingAutoLoadWafer(INT8 set, ENG_MWKT wafer);
/*
 desc : Set/Display “wafer” type. (cmd : _wt)
 parm : type	- [in]  Wafer Type (-2, -1, 0, 1, 2)
 retn : TRUE or FALSE
 note : Currently set wafer type.
		This command sets or displays the current wafer type. The possible values are:
		-2	Wafer with a notch detected by an external sensor.
		-1	Wafer without notch or flat (these wafers are not oriented, only centered).
		 0	Wafer with a notch.
		 1	Wafer with one or more flats.
		 2	Square substrate.
 exam : >_WT
		0
		>_WT 1
		>_WT
		1
		>
*/
API_IMPORT BOOL uvEng_MPA_GetWaferType();
API_IMPORT BOOL uvEng_MPA_SetWaferType(ENG_PGWT type);
/*
 desc : Save performance set parameters. (cmd : wps)
 parm : set_no	- [in]  Performance Set Number
 retn : TRUE or FALSE
 note : This command saves all axes motion parameters and the prealigner calibration parameters to the NV
		(Non-volatile) memory into a file named perform.ini.
 exam : >WPS 1
		>
*/
API_IMPORT BOOL uvEng_MPA_RestorePerformSetFile(UINT8 set_no);
API_IMPORT BOOL uvEng_MPA_WritePerformSetFile(UINT8 set_no);
/*
 desc : Read parameter sets from file (done at start-up). (cmd : rps)
 parm : None
 retn : TRUE or FALSE
 note : This command re-reads all parameter sets from file named paramset.ini, overwriting existing values.
		For details see Section 4.6: Working With Parameter Sets. This command is executed automatically during start-up.
exam : None
*/
API_IMPORT BOOL uvEng_MPA_RestoreParamSetFile();
/*
 desc : Save parameter sets. (cmd : sps)
 parm : None
 retn : TRUE or FALSE
 note : This command saves all parameter sets to file named paramset.ini, overwriting any existing file.
		This command should be executed if any changes made to parameter sets are to be preserved. For
		details see section Section 4.6: Working With Parameter Sets.
 exam : None
*/
API_IMPORT BOOL uvEng_MPA_SaveParamSetFile();
/*
 desc : Restore motion and calibration parameters. (cmd : rmp)
 parm : None
 retn : TRUE or FALSE
 note : This commands restores previously saved motion parameters and prealigner calibration parameters
		(with SMP command) from NV (Non-volatile) memory. All motion and calibration parameters
		currently in effect are lost and new ones take effect. See section 7.4.1 for details.
 exam : None
*/
API_IMPORT BOOL uvEng_MPA_RestoreParamSetMemory();
/*
 desc : Save motion and calibration parameters. (cmd : smp)
 parm : None
 retn : TRUE or FALSE
 note : This command saves all axes motion parameters and the prealigner calibration parameters to the NV (Non-volatile) memory.
 exam : None
*/
API_IMPORT BOOL uvEng_MPA_SaveParamSetMemory();
/*
 desc : Set system date and time. (cmd : _dt)
 parm : None
 retn : TRUE or FALSE
 note : Current system date and time.
		Set to the current local computer's time value.
		Since most prealigner controllers do not keep the date and time between restarts, correct date and
		time has to be provided from the host. Date and time stamps are used in event logs kept by the
		controller, and are useful for troubleshooting.
 exam : 
*/
API_IMPORT BOOL uvEng_MPA_GetSystemTime();
API_IMPORT BOOL uvEng_MPA_SetSystemTime();
/*
 desc : Homing speed and acceleration (or Working speed and acceleration). (cmd : hsa or wsa)
 parm : mode	- [in]  0x00:HSA (Homing Mode), 0x01:WSA (Working Mode)
 retn : TRUE or FALSE
 note : This command assigns the homing trajectory parameters (speed, acceleration, and deceleration) to all the axes.
		Although these speeds and accelerations can be seen after HSA command is executed,
		there are no commands to change these speeds or accelerations.

		This command sets the working trajectory parameters (speed, acceleration, and deceleration) to all
		the axes. When tuning trajectory parameters, they can be saved using SMP command.
 exam : >SPD
		48000,20000,20000
		>HSA
		>SPD
		4800,500,500
		>

		>SPD
		4800,500,200
		>WSA
		>SPD
		48000,20000,20000
		>
*/
API_IMPORT BOOL uvEng_MPA_RestoreTrajectorySpeedAcc(UINT8 mode);
/*
 desc : Reset the controller. (cmd : res)
 parm : None
 retn : TRUE or FALSE
 note : This command resets the controller.
 exam : >RES
		……0451
		>
*/
API_IMPORT BOOL uvEng_MPA_ResetPrealigner();
/*
 desc : Enables saving the CCD samples from unsuccessful alignment. (cmd : _autodumps)
 parm : mode	- [in] “Auto samples dump” mode (0x00: Disable, 0x01: Enable)
 retn : TRUE or FALSE
 note : Currently set mode.
		This parameter controls whether the prealigner automatically generates a file containing CCD data
		after alignment failure occurs. To enable this functionality, set parameter to 1. This mode can be
		useful when troubleshooting occasional or intermittent alignment failure.
 exam : >_AUTODUMPSF
		7
		>_AUTODUMPSF 1
		>BAL
		>
*/
API_IMPORT BOOL uvEng_MPA_GetAlignFailAutoDumps();
API_IMPORT BOOL uvEng_MPA_SetAlignFailAutoDumps(UINT8 mode);
/*
 desc : Specifies the file name for CCD samples from unsuccessful alignment. (cmd : _autodumpsf)
 parm : file_no	- [in]  0 to 9 – file number for “Auto samples dump” mode
 retn : TRUE or FALSE
 note : Currently set file number.
		This parameter specifies the file number for the CCD samples file generated if the parameter _
		AUTODUMPS option is set to 1. The actual file name is “AUTOS” and the file number, its extension
		is “CSV”. After saving a file, the number is incremented automatically. This mode can be useful when
		troubleshooting occasional alignment failure.
 exam : >_AUTODUMPSF
		7
		>_AUTODUMPSF 1
		>BAL
		>
*/
API_IMPORT BOOL uvEng_MPA_GetAlignFailAutoDumpFile();
API_IMPORT BOOL uvEng_MPA_SetAlignFailAutoDumpFile(UINT8 file_no);
/*
 desc : Complete calibration procedure of standard (non-edge handling) prealigners. (cmd : _cal)
 parm : None
 retn : TRUE or FALSE
 note : This command initiates the main prealigner calibration procedure. It has to be performed if any
		mechanical changes are made to the prealigner such as replacement of the chuck or pins. It should
		be started with a non-transparent 3”, 4”, 8”, 12”, or 18” inch wafer supported by the prealigner. Upon
		successful completion,
 exam : >SON
		>_CAL
		…
		000C
		>
*/
API_IMPORT BOOL uvEng_MPA_SetPhysicalChangeCalibration();
/*
 desc : Calibration procedure of CCD compensation factors. (cmd : _cal_cor)
 parm : mode1	- [in]  use small iteration steps (0 or 1)
						0x00: Fast Calibration Algorithm
						0x01: Slow Calibration Algorithm (doing a finer check instead)
		mode2	- [in]  Whether CCD-centric search (0 or 1)
 retn : TRUE or FALSE
 note : This command initiates calibration of the CCD compensation factors _SH and _SL and optionally the
		CCD center for the current wafer size. It is recommended to be performed if the light house height is
		changed or after changing the Z-axis measure position (_MP Z). It should be started with a wafer on
		the prealigner. It does not save the calibrated parameters, so this should be done manually using the
		SMP command after performing this command.
 exam : >WSZ 8
		>_CAL_COR 1 0
		…
		000C
		>SMP
		>
*/
API_IMPORT BOOL uvEng_MPA_SetPhysicalChangeCompensation(UINT8 mode1, UINT8 mode2);
/*
 desc : Saves CCD samples to .BAL file. (cmd : ddpf)
 parm : file_no	- [in]  File Numbering (0 ~ 9)
 retn : TRUE or FALSE
 note : This command saves the CCD samples from the last scan to a file with extension “.BAL”.
		The file name is “samples” with appended the Value parameter.
		Such files can be used for detailed troubleshooting of the alignment algorithm by the manufacturer.
		Before sending this command, DRPS command should be executed.
 exam : >BAL
		020F
		?DRPS
		2500 samples read.
		>DDPF 1
		2500 samples dumped.
		>
*/
API_IMPORT BOOL uvEng_MPA_SetDetailTroubleFileByManufacture(UINT8 file_no);
/*
 desc : Save CCD Samples to a File (cmd : ddpf)
 parm : file_no	- [in]  File Numbering (0 ~ 9)
 retn : TRUE or FALSE
 note : This command saves the CCD samples from the last scan to a file with extension “.BAL”. The file name
		is “samples” with appended the Value parameter. Such files can be used for detailed troubleshooting
		of the alignment algorithm by the manufacturer. Before sending this command, DRPS command
		should be executed.
 exam : >BAL
		020F
		?DRPS
		2500 samples read.
		>DDPF 1
		2500 samples dumped.
		>
*/
API_IMPORT BOOL uvEng_MPA_SetBasicTroubleFileByManufacture(UINT8 file_no);
/*
 desc : Prepares CCD samples for saving to .BAL file. (cmd : drps)
 parm : None
 retn : TRUE or FALSE
 note : Number of samples read.
		This command reads samples obtained during the last data collection run into internal memory, and
		prepares them for use with the DDPF command.
 exam : >BAL
		...0002
		>DRPS
		2500 samples read
		>DDPF 1
		2500 samples dumped
		>
 */
API_IMPORT BOOL uvEng_MPA_SetTroubleDumpToMemory();
/*
 desc : Verifies the prealigner is edge handling. (cmd : _eh)
 parm : None
 retn : TRUE or FALSE
 note : 1 if the prealigner is edge handling, 0 otherwise.
		This command returns 1 when sent to an edge handling prealigner.
		_EH returns the prealigner handling type
 exam : >_EH
		1
		>
*/
API_IMPORT BOOL uvEng_MPA_GetEdgeHandleType();
/*
 desc : Maximum chuck CCD value. (cmd : _ehc or _ehc1 or _ehc2)
 parm : type	- [in]  0x00: Normal Wafer Size, 0x01: Smaller Wafer Size, 0x02: Greater Wafer Size
		pixel	- [in]  chuck CCD value (unit: pixels)
 retn : TRUE or FALSE
 note : Currently set chuck CCD value.
		This parameter specifies the maximum CCD value when only the chuck of an edge handling
		prealigner covers the sensor. When scanning the CCD sensor, any CCD values below this parameters
		are considered as no-wafer. This parameter should be less than the minimum possible CCD reading
		from a wafer and greater than any CCD values from the chuck.

		_ehc1 : Some edge handling prealigners support two different wafer sizes.
				This command sets the chuck CCD value of the smaller size.
		_ehc2 : Some edge handling prealigners support two different wafer sizes.
				This command sets the chuck CCD value of the larger size.
 exam : >_EHC
		1800
		>_EHC 1830
		>

		>_EHC1
		1800
		>_EHC1 1830

		>_EHC2
		2100
		>_EHC2 2130
		>
		>
*/
API_IMPORT BOOL uvEng_MPA_GetCCDSensorSensingDist(UINT8 type);
API_IMPORT BOOL uvEng_MPA_SetCCDSensorSensingDist(UINT16 pixel, UINT8 type);
/*
 desc : Minimum chuck pins CCD value. (cmd : cmd : _ehp or _ehp1 or _ehp2)
 parm : type	- [in]  0x00: 일반 Wafer Size, 0x01: Smaller Wafer Size, 0x02: Greater Wafer Size
		pixel	- [in]  chuck pin CCD value. (unit: pixels)
 retn : TRUE or FALSE
 note : This parameter specifies the CCD value when the chuck pins of an edge handling prealigner cover
		the sensor. When scanning the CCD sensor, any CCD values above this parameters are considered as
		values from the pins and are ignored. This parameter should be greater than the maximum possible
		CCD reading from a wafer and (if possible) smaller than any CCD values from the pins.

		_EHP1 : Some edge handling prealigners support two different wafer sizes. This command sets the chuck
				pins CCD value of the smaller size.
		_EHP1 : Some edge handling prealigners support two different wafer sizes. This command sets the chuck
				pins CCD value of the greater size.
 exam : >_EHP
		2700
		>_EHP 2730
		>

		>_EHP1
		2700
		>_EHP1 2730
		>

		>_EHP2
		3100
		>_EHP2 3130
		>
*/
API_IMPORT BOOL uvEng_MPA_GetCCDPinsSensingDist(UINT8 type);
API_IMPORT BOOL uvEng_MPA_SetCCDPinsSensingDist(UINT16 pixel, UINT8 type);
/*
 desc : Set/Display safe Z position above which which the chuck is clear of the stationary pins can rotate freely. (cmd : _sfz)
 parm : value	- [in]  Z-axis position to rotate safely (unit: um)
 retn : TRUE or FALSE
 note : Currently set safe Z position.
		This command sets or displays the position along the vertical axis above which the chuck of an edge
		handling prealigner and can safely rotate above the stationary pins.
 exam : >_SFZ
		2280
		>_SFZ 2290
		>_SFZ
		2290
		>
*/
API_IMPORT BOOL uvEng_MPA_GetSafeRotateAxisZ();
API_IMPORT BOOL uvEng_MPA_SetSafeRotateAxisZ(DOUBLE value);

/* --------------------------------------------------------------------------------------------- */
/*                 External Interface  - PreAligner for Logosol < for Common >                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Whether the state value of system is valid (!!! Important !!!)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MPA_IsValidSystemStatus();
/*
 desc : Whether the current motion (Robot Arm) is moving, i.e. Busy (asynchronous control)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MPA_IsMotorArmBusy();
/*
 desc : Whether the current motion (Robot Arm) is idle (asynchronous control)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MPA_IsMotorArmIdle();
/*
 desc : Whether there are packets received for the most recently sent command
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MPA_IsLastSendAckRecv();
/*
 desc : Whether the receive response was successful for the most recently sent command
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MPA_IsLastSendAckSucc();
/*
 desc : Returns an error message for system status values
 parm : mesg	- [out] A return buffer in which a detailed description of the error code will be stored.
		size	- [in]  'mesg' buffer size
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MPA_GetSystemStatusMesg(TCHAR *mesg, UINT32 size);
/*
 desc : Returns detailed string information about the error value (code) of the previously sent command
 parm : mesg	- [out] A return buffer in which a detailed description of the error code will be stored.
		size	- [in]  'mesg' buffer size
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MPA_GetPreviousError(TCHAR *mesg, UINT32 size);
/*
 desc : Whether there is the most recently received data
 parm : cmd		- [in]  command to be checked
		r_cmd	- [out] 가장 최근에 수신된 명령어가 저장될 버퍼
		size	- [in]  'r_cmd' 버퍼의 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MPA_IsRecvCmdData(ENG_PSCC cmd=ENG_PSCC::psc_none);
API_IMPORT BOOL uvCmn_MPA_IsRecvCmdLast();
API_IMPORT BOOL uvCmn_MPA_GetRecvCmdLast(PTCHAR r_cmd, UINT32 size);
/*
 desc : Whether an error has occurred inside the equipment (system)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MPA_IsSystemError();
#if 0
/*
 desc : In case of using with Info mode 1 set, the operation completion status is determined by receiving a return code (asynchronous control)
 parm : None
 retn : 0x00 : Not valid (상태가 값이 유효하지 않은 상태), 0x01 : Not ready, 0x02 : 동작 완료, 0x03 : Error, 0x04 : Unknown (상태값 읽어서 확인)
 retn : TRUE or FALSE
*/
API_IMPORT UINT8 uvCmn_MPA_IsWaitMotionDoneReply()
#endif
/* --------------------------------------------------------------------------------------------- */
/*                            Returns a value stored in shared memory                            */
/* --------------------------------------------------------------------------------------------- */

/* String data value returned from the most recently requested command */
API_IMPORT PTCHAR uvCmn_MPA_GetValueLastString();
/* Currently set wafer type ((cmd : _wt) */
API_IMPORT INT8 uvCmn_MPA_GetValueWaferType();
/* Currently assigned parameter set to the current wafer size (unit: inch) ((cmd : _wszautoset) */
API_IMPORT INT8 uvCmn_MPA_GetValueWaferSizeInch();
/* Currently set information mode ((cmd : inf) */
API_IMPORT UINT8 uvCmn_MPA_GetValueInfoMode();
/* Information about whether and object is covering any of the CCD sensors ((cmd : doc) */
API_IMPORT UINT8 uvCmn_MPA_GetValueWaferLoaded();
/* Currently set mode (0 or 1 – Automatic wafer size detection mode) ((cmd : asz) */
API_IMPORT UINT8 uvCmn_MPA_GetValueWaferSizeDetection();
/* Currently set mode (0 or 1 – “Auto scan again” mode) ((cmd : _autoscanagain) */
API_IMPORT UINT8 uvCmn_MPA_GetValueWaferScanAgain();
/* Currently set T-axis fast speed (0 or 1– State of chuck vacuum OFF or ON after BAL) ((cmd : _finalvacc) */
API_IMPORT UINT8 uvCmn_MPA_GetValueVacuumStateAfterAlign();
/* Currently setting of MGLM (0 or 1) ((cmd : mglm) */
API_IMPORT UINT8 uvCmn_MPA_GetValueWaferGlassType();
/* Currently set notch/flat depth calculation mode (0 or 1) ((cmd : _notchdepthmode) */
API_IMPORT UINT8 uvCmn_MPA_GetValueNotchDepthMode();
/* Currently set MTP command mode (0 or 1) ((cmd : _mtpmode) */
API_IMPORT UINT8 uvCmn_MPA_GetValueModeZAfterMovePins();
/* Currently set sampling mode (0x00 to 0x03) ((cmd : _sm) */
API_IMPORT UINT8 uvCmn_MPA_GetValueRotationSampleMode();
/* Currently set „Stop on pins” mode (0 or 1) ((cmd : _sp) */
API_IMPORT UINT8 uvCmn_MPA_GetValueStopOnPins();
/* Currently set value (0 or 1) ((cmd : wsz2) */
API_IMPORT UINT8 uvCmn_MPA_GetValueNotchLightSource();
/* Currently set mode (0 or 1 – “Auto samples dump” mode) ((cmd : _autodumps) */
API_IMPORT UINT8 uvCmn_MPA_GetValueAlignFailAutoDumps();
/* Currently set file number (0 to 9 – file number for “Auto samples dump” mode) ((cmd : _autodumpsf) */
API_IMPORT UINT8 uvCmn_MPA_GetValueAutoSamplesDumpFile();
/* 1 if the prealigner is edge handling, 0 otherwise ((cmd : _eh) */
API_IMPORT UINT8 uvCmn_MPA_GetValueIsEdgeHandling();
/* Currently set current limit of all axes (1 to 251 level)  ((cmd : _cl) */
API_IMPORT PUINT8 uvCmn_MPA_GetValueCurrentLimits();
/* Currently set axis profile type. (cmd : pro) */
API_IMPORT PUINT8 uvCmn_MPA_GetValueVelocityProfiles();
/* Currently set minimum wafer size (cmd : _minwsz) */
API_IMPORT UINT8 uvCmn_MPA_GetValueWaferMinSize();
/* Currently set maximum wafer size (cmd : _maxwsz) */
API_IMPORT UINT8 uvCmn_MPA_GetValueWaferMaxSize();
/* Currently set wafer diameter (cmd : wsz) */
API_IMPORT UINT8 uvCmn_MPA_GetValueWaferWorkSize();
/* Currently set averaging mode (current sampling mode) (0, 3 or 5) (cmd : _sam) */
API_IMPORT UINT8 uvCmn_MPA_GetValueNotchAverageMode();
/* Currently set wafer loading type (0 – load wafer on pins; 1 – load wafer on chuck) (cmd : ltc) */
API_IMPORT UINT8 uvCmn_MPA_GetValueWaferLoadingType();
/* Currently set minimum notch depth. (1 to 100) (cmd : _mindepthmain) */
API_IMPORT UINT8 uvCmn_MPA_GetValueMinimumNotchDepth();
/* Currently set notch find mode (0 to 3) (cmd : _notchfindmode) */
API_IMPORT UINT8 uvCmn_MPA_GetValueNotchFindMode();
/* Currently set maximum notch/flat slope (cmd : _notchmaxderval) */
API_IMPORT UINT8 uvCmn_MPA_GetValueNotchMaxDerVal();
/* Currently set minimum notch/flat slope (cmd : _notchminderval) */
API_IMPORT UINT8 uvCmn_MPA_GetValueNotchMinDerVal();
/* Currently set number of samples outside notch/flat (cmd : _notchnavg) */
API_IMPORT UINT8 uvCmn_MPA_GetValueNotchNumAverage();
/* Currently set limit (1 -> 0.0001 inch) (cmd : _ao2) */
API_IMPORT UINT16 uvCmn_MPA_GetValueAlignOffsetTwice();
/* Currently set acceptable offset (1 -> 0.0001 inch) (cmd : _aof) */
API_IMPORT UINT16 uvCmn_MPA_GetValueAlignOffsetAccept();
/* All prealigner axes home switch state (cmd : rhs) */
API_IMPORT UINT16 uvCmn_MPA_GetValueAxisSwitchActive();
/* System status word (cmd : sta) */
API_IMPORT UINT16 uvCmn_MPA_GetValueSystemStatus();
/* Current value of CCD sensor(cmd : ccd) */
API_IMPORT UINT16 uvCmn_MPA_GetValueCCDPixels();
/* Number of notches falling within minimum/maximum depth/width criteria(cmd : ngn) */
API_IMPORT UINT16 uvCmn_MPA_GetValueLastAlignSucc();
/* Number of notches failing out of minimum/maximum depth/width criteria (cmd : nbn) */
API_IMPORT UINT16 uvCmn_MPA_GetValueLastAlignFail();
/* Description of previous error (cmd : per) */
API_IMPORT UINT16 uvCmn_MPA_GetValuePreviousCmdFail();
/* Currently set minimum good data samples (cmd : _mgd) */
API_IMPORT UINT16 uvCmn_MPA_GetValueMinGoodSampleCount();
/* Currently set chuck CCD value (cmd : _ehc) */
API_IMPORT UINT16 uvCmn_MPA_GetValueCCDChuckSensingValue();
/* Currently set chuck CCD value of smaller size (cmd : _ehc1) */
API_IMPORT UINT16 uvCmn_MPA_GetValueCCDChuckSensingSmall();
/* Currently set chuck CCD value of greater size. (cmd : _ehc2) */
API_IMPORT UINT16 uvCmn_MPA_GetValueCCDChuckSensingGreat();
/* Currently set chuck pins CCD value (cmd : _ehp) */
API_IMPORT UINT16 uvCmn_MPA_GetValueCCDPinsSensingValue();
/* Currently set chuck pins CCD value of smaller size. (cmd : _ehp1) */
API_IMPORT UINT16 uvCmn_MPA_GetValueCCDPinsSensingSmall();
/* Currently set chuck pins CCD value of greater size (cmd : _ehp2) */
API_IMPORT UINT16 uvCmn_MPA_GetValueCCDPinsSensingGreat();
/* Currently set CCD center (cmd : _sc) */
API_IMPORT UINT16 uvCmn_MPA_GetValueCCDSensorCenter();
/* Currently set timeout value (cmd : _to) */
API_IMPORT UINT32 uvCmn_MPA_GetValueLastParamTimeout();
/* Currently set minimum notch depth. (cmd : _mnd) */
API_IMPORT UINT32 uvCmn_MPA_GetValueNotchRangeDepthMin();
/* Currently set maximum notch depth. (cmd : _mxd) */
API_IMPORT UINT32 uvCmn_MPA_GetValueNotchRangeDepthMax();
/* Currently set minimum notch width. (cmd : _mnw) */
API_IMPORT UINT32 uvCmn_MPA_GetValueNotchRangeWidthMin();
/* Currently set maximum notch width (cmd : _mxw) */
API_IMPORT UINT32 uvCmn_MPA_GetValueNotchRangeWidthMax();
/* Currently set minimum CCD value for square substrates (cmd : _sqmin) */
API_IMPORT UINT32 uvCmn_MPA_GetValueCCDGrabSizeMin();
/* Currently set maximum CCD value for square substrates (cmd : _sqmax) */
API_IMPORT UINT32 uvCmn_MPA_GetValueCCDGrabSizeMax();
/* Two numbers, denoting depth and width of requested (or first) notch (cmd : ndw) */
API_IMPORT PUINT32 uvCmn_MPA_GetValueFindDepthWidth();
/* Currently set position error limit of all axes (cmd : _el) */
API_IMPORT PUINT16 uvCmn_MPA_GetValueErrorLimit();
/* Currently set final wafer orientation angle (cmd : fwo) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MPA_GetValueRotateAfterAlign();
/* Currently set transfer down position (cmd : _td) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MPA_GetValueWaferTransSafePosUp();
/* Currently set transfer up position (cmd : _tu) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MPA_GetValueWaferTransSafePosDown();
/* Currently set safe Z position (cmd : _sfz) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MPA_GetValueSafeRotateZPos();
/* Currently set angle (cmd : _sa) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MPA_GetValueCCDRAxisAngle();
/* Currently set pins position offset (Moving Up) (cmd : _ppcu) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MPA_GetValueOffsetChuckPinPosUp();
/* Currently set pins position offset (Moving Down) (cmd : _ppcd) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MPA_GetValueOffsetChuckPinPosDown();
/* Currently set pins position (cmd : _pp) (unit : um or degree or sec) */
API_IMPORT DOUBLE uvCmn_MPA_GetValueLevelChuckPinsPosZ();
/* Currently set load up position for all axes. (cmd : _lu) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MPA_GetValueWaferLoadingUpPos();
/* Currently set load down position for all axes. (cmd : _ld) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MPA_GetValueWaferLoadingDownPos();
/* Current position of all axes (cmd : mcpo) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MPA_GetValueAxisPos();
/* Currently set customized home position of all the axes (cmd : ) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MPA_GetValueHomePos()	;
/* Currently set home offset of all the axes (cmd : _ho) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MPA_GetValueHomeOffset();
/* Currently set forward direction software limit for all axes. (cmd : _fl) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MPA_GetValueDirectLimitF();
/* Currently set Reverse direction software limit for all the axes (cmd : _rl) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MPA_GetValueDirectLimitR();
/* Currently set axis acceleration (cmd : acl) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MPA_GetValueAxisACL();
/* Currently set axis deceleration (cmd : dcl) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MPA_GetValueAxisDCL();
/* Currently set axis acceleration jerk (cmd : ajk) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MPA_GetValueAxisAJK();
/* Currently set axis deceleration jerk (cmd : djk) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MPA_GetValueAxisDJK();
/* Currently set speed for all or one axis (cmd : spd) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MPA_GetValueAxisSpeed();
/* Displacement (offset), offset angle, and notch/flat angle (cmd : mrwi) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MPA_GetValueRWIWaferInfo();
/* Currently set measure acceleration (cmd : _ma) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MPA_GetValueMeasureACL();
/* Currently set measure deceleration (cmd : _md) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MPA_GetValueMeasureDCL();
/* Currently set measuring speed (cmd : _ms) (unit : um or degree or sec) */
API_IMPORT PDOUBLE uvCmn_MPA_GetValueMeasureSpeed();

#ifdef __cplusplus
}
#endif