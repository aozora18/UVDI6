using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

/* User-defined Namespace */
using TGen2i.Enum.Comn;
using TGen2i.Struct.Comn;
using TGen2i.Struct.Luria;
using TGen2i.Enum.Luria;

namespace TGen2i.EngineLib
{
	public class EngLuria
	{

		/* ------------------------------------------------------------------------------------- */
		/*                                Shared memory in Luria                                 */
		/* ------------------------------------------------------------------------------------- */

		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvEng_ShMem_GetLuria();	/* Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM)) */

		/* ------------------------------------------------------------------------------------- */
		/*                                 Luria < for Engine >                                  */
		/* ------------------------------------------------------------------------------------- */

		/* Disconnect the existing connection and make a new connection */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_Reconnected();
		/* After connecting to Luria Service (optical system), whether all initialization commands have been transmitted/received */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_IsServiceInited();
		/* Whether the send buffer is full */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_IsSendBuffFull();
		/* Percentage of the number of data remaining in the transmit buffer */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern float uvEng_Luria_GetSendBuffEmptyRate();
		/* Whether packets are received */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_IsRecvPktData(ENG_FDPR flag);
		/* Whether the last (recently) received command response */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_IsLastRecvCmd();
		/* System (H/W) Init information request */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetHWInit();
		/* Request system status information */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetSystemStatus();
		/* System Shutdown Information Settings */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetSystemShutdown();
		/* Set System Shutdown information (no request) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetUpgradeFirmware(char[] file);
		/* Load DMD images stored inside Photohead (settings) (no request) */
		/* Note : Load only the image file inside, and use the "SetLightIntensity" command to actually print it. */
		/*        After executing this command, you must proceed with Hardware Init for normal exposure. */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetLoadInternalTestImage(Byte ph_no, Byte img_no);
		/* Set LED Power for output of DMD images stored inside the photohead (no request) */
		/* Note: To output the selected image inside (For future exposure work, you need to call Hardware Init) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetLightIntensity(Byte ph_no, Byte led_no, UInt16 ampl);
		/* Mainboard temperature request for Photoheads linked to Luria Service (no setting) */
		/* Note: If the temperature is higher than 55 degrees, it means that the cooling does not work normally. */
		/* If the temperature is 0, it means that the Luria Service is not connected to the Photohead. */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetMainboardTemp();
		/* Request overall (detail?) error status values of Photoheads linked to Luria Service */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetOverallErrorStatusMulti();
		/* Only Get - Exposure State */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetExposureState();
		/* Get or Set - When function is enabled, the photo head printing the last strip will copy the autofocus profile of the second to last strip and use that profile for focusing the last strip */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetEnableAfCopyLastStrip();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetEnableAfCopyLastStrip(Byte enable);
		/* Get or Set - Disable autofocus for the first strip for the first photohead */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetDisableAfFirstStrip();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetDisableAfFirstStrip(Byte disable);
		/* Request Panel Data Info */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetPanelDataInfo(UInt32 dcode);
		/* Get or Set - Registration (Points & Global & Local) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetRegistration();
		/* Assign Job List setting */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqAddJobList(char[] job);
		/* Delete the currently selected Job Name (remove, not delete files) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetDeleteSelectedJob();
		/* Select the current job name and request the result */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSelectedJobName(char[] job_name, Byte mode=0x00);
		/* Request for parameter information related to the currently selected Job Name */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGerberJobParam();
		/* Set Selected Load Job */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetLoadSelectedJob();
		/* Delete a job at the location (Index) where a random job name is saved (remove, not file deletion) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetDeleteJobIndex(Byte index);
		/* Job List Request */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetJobList();
		/* Job Selected Load State Request */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetSelectedJobLoadState();
		/* Request information on the currently set exposure value (Led Duty / Frame Rate / Step Size / Exposure Speed) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetExposureFactor();
		/* Set - Scroll Step Size / Frame Rate Factor / Led Duty Cycle */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetExposureFactor(Byte step, Byte duty, Double frame_rate);
		/* LED Driver Amplitude (different from LedAmplitude function. This function is used for Spot) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetLedPowerSpot(Byte ph_no, UInt16 led_no, UInt16 power);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetLedPowerSpot(Byte ph_no, UInt16 led_no);
		/* Request/Set LED Amplitude */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetLedAmplitude(Byte count, UInt16 [,] amp);
		/* Set - LED Amplitude (In case of setting only 1 photo head data) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetLedAmplitudeOne(Byte ph_no, Byte led_no, UInt16 power);
		/* Get LED Amplitude */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetLedAmplitude();
		/* Focus Motor Move Absolute Position (Move to the set position) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetMotorAbsPosition(Byte ph_no, Double pos);
		/* Focus Motor Move Absolute Position (Move to set position) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetMotorAbsPositionAll(Byte count, UInt32[] pos);
		/* Setting the exposure start position / requesting the previously set position */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetExposeStartXY(Byte tbl_num);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetExposeStartXY(Byte tbl_num, UInt32 start_x, UInt32 start_y, Boolean reply=true);
		/* Get exposure start position (Motion X/Y) - Exposure start position considering acceleration */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetTableMotionStartPos(Byte tbl_num);
		/* Any Photohead's Z-axis motor's position initialization (Homing) (moving to the middle position) or stopping */
		/* However, only the Photo Head whose motor is not initialized is initialized. */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetMotorPositionInit(Byte ph_no, Byte action=0x01);
		/* All Photohead's Z-axis motors are either homing (moved to the middle position) or stopped. */
		/* However, only the Photo Head whose motor is not initialized is initialized. */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetMotorPositionInitAll(Byte action=0x01);
		/* Request the current status of all Photohead's Z-axis motors */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetMotorStateAll();
		/* Focus Motor Move Absolute Position (get current position) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetMotorAbsPositionAll();
		/* Print related options */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetPrintOpt(ENG_LCEP flag);
		/* Only Set - Move to Start the position for printing */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetPrintStartMove();
		/* Only Set - This command should be sent to Luria if the current light amplitude is
		   significantly higher/lower than the amplitude used in the previous print.
		   The command results in a short light-output from the photo heads */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetPrintRampUpDown();
		/* Request/Set AF Sensor Type and Setting value */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetAFSensor();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetAFSensor(Byte type, Byte agc, UInt16 pwm);
		/* Request the optimal laser PWM value */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetAFSensorMeasureLaserPWM();
		/* Number of available LEDs installed per photo head and request/setting of Source Type */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_GetPktNumberOfLightSource(Byte ph_no);
		/* Request LED status values within the entire Photohead */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_GetPktLightSourceStatusMulti();
		/* Request for necessary information regarding the step difference of the photo head */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetPhotoheadOffInfo(Byte scroll);
		/* Photo head specification information setting (Only Offset) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetSpecPhotoHeadOffset(Byte ph_no, UInt32 x_pos, Int32 y_pos);
		/* Return Global Mark Data loaded from XML file (Local & Global) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_GetGlobalMark(UInt16 index, ref STG_XMXY point);
		/* Global Mark Coordinates Get Two Position Values */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_GetGlobalMarkDirect(ENG_GMDD direct, ref STG_XMXY data1, ref STG_XMXY data2);
		/* Return Local Mark Data loaded from XML file (Local & Global) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_GetLocalMark(UInt16 index, ref STG_XMXY point);
		/* Return the number of Marks registered in Gerber */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvEng_Luria_GetMarkCount(ENG_AMTF mark);
		/* Whether Global / Local Mark exists */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvEng_Luria_IsMarkGlobal();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvEng_Luria_IsMarkLocal();
		/* return gerber size */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern void uvEng_Luria_GetGerberSize(ref Double width, ref Double height);
		/* Returns the distance between two marks based on the X axis of the global reference point (unit: mm) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvEng_Luria_GetGlobalMarkDist(ENG_GMDD direct);
		/* The left/right X-axis distance between two marks, that is, the error (distance) value between the X-axis */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvEng_Luria_GetGlobalMarkDiffX(Byte mark_x1, Byte mark_x2);
		/* The upper/lower Y-axis distance between the two marks, that is, the error (height) value between the Y-axis */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvEng_Luria_GetGlobalMarkDiffY(Byte mark_y1, Byte mark_y2);
		/* Returns the X coordinate error value of Mark 1 and 3 and Mark 2 and 4 */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvEng_Luria_GetGlobalMarkDiffVertX(Byte type);
		/* Align Mark X axis distance between Camera 1 and 2 */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvEng_Luria_GetLocalMarkACam12DistX(Byte mode, Byte scan);
		/* The upper/lower Y-axis distance between the two marks, that is, the error (height) value between the Y-axis */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 uvEng_Luria_GetLocalMarkDiffY(Byte mark_y1, Byte mark_y2);
		/* Based on Mark Type, return the height difference between the Y coordinates of Left/Bottom of Camera 1 and Right/Bottom of Camera 2 */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 uvEng_Luria_GetGlobalLeftRightBottomDiffY();
		/* Get Local Fiducial Coordinate Values - Left / Bottom */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_GetLocalBottomMark(Byte scan, Byte cam_id, ref STG_XMXY data);
		/* Get or Set - Global Transformation Recipe */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetTransformationRecipeGlobal();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetTransformationRecipeGlobal(Byte rotation=0x00, Byte scaling=0x00, Byte offset=0x00);
		/* Fixed Rotation, Fixed Scaling, Fixed Offset 요청 / 설정 */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetGlobalFixed();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetGlobalFixed(Double rotation, Double scale_x, Double scale_y,
																   Double offset_x, Double offset_y);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetGlobalFixedRotation();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetGlobalFixedRotation(Int32 rotation);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetGlobalFixedScaling();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetGlobalFixedScaling(UInt32 scale_x, UInt32 scale_y);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetGlobalFixedOffset();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetGlobalFixedOffset(Int32 offset_x, Int32 offset_y);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetRotationScalingOffsetGlobal(Double rotation, Double scale_x, Double scale_y, Double offset_x, Double offset_y);
		/* Get or Set - All Registration (Registration Points -> Run Registration) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetRegistPointsAndRun(UInt16 p_count, ref STG_I32XY p_fidxy);
		/* Get or Set - Only Registration Points */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetRegistrationPoints(UInt16 p_count, ref STG_I32XY p_fidxy);
		/* Get or Set - Only Run Registration */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetRunRegistration();
		/* Get Only - Get Registration Status */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetRegistStatus();
		/* Read XML data of currently selected Gerber */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_LoadSelectJobXML(ENG_ATGL align=ENG_ATGL.en_global_4_local_0x0_n_point);
		/* The total number of exposures set (saved) in the XML file, that is, the number of stripes. */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvEng_Luria_GetGerberStripeCount();
		/* If it consists of a three-point global mark, return the batch information */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvEng_Luria_GetGlobalMark3PType();
		/* Request or set the internal image number currently loaded in the photohead */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetLoadInternalImage(Byte ph_no);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetLoadInternalImage(Byte ph_no, Byte img_no);
		/* Request or set Enable Flatness Correction Mask (PPC) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetFlatnessMaskOn(Byte ph_no);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetFlatnessMaskOn(Byte ph_no, Byte enable);
		/* Select the sequence file you want to use (external or internal sequence file. If internal, one of 0 to 4 is selected) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetActiveSequence(Byte ph_no, Byte type);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetActiveSequence(Byte ph_no);
		/* Light Source Driver Light On/Off */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetLedLightOnOff(Byte ph_no, UInt16 led_no, Byte onoff);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetLedLightOnOff(Byte ph_no, UInt16 led_no);
		/* Sequence State (Halted or Running) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetSequnceState(Byte ph_no, Byte seq_cmd, Byte enable);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetSequnceState(Byte ph_no, Byte seq_cmd);
		/* Enable or Disable Plotting of Line Sensor Data for a specific Photohead */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetLineSensorPlot(Byte ph_no, Byte enable);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetLineSensorPlot(Byte ph_no);
		/* Check the Autofocus Position of the current optical system */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetCurrentAutofocusPosition(Byte ph_no);
		/* Serial Information Request / Setup */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetSerialNumber(UInt32 dcode);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetSerialNumber(UInt32 dcode, Byte symbol, UInt16 s_len, Byte[] serial, UInt32 start, Byte flip_x, Byte flip_y, UInt16 font_h, UInt16 font_v);
		/* Panel Data Request / Setup */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetPanelData(UInt32 dcode);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetPanelData(UInt32 dcode, UInt16 s_len, Byte[] text, Byte flip_x, Byte flip_y, UInt16 font_h, UInt16 font_v);
		/* Only Set - Print Abort */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetPrintAbort();
		/* Motion controller information request/setting */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetMotionControl();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetMotionControl(Byte mc2, Byte[] ipv4, Double max_y_speed/* mm */, Double x_speed/* mm */);
		/* When global auto transformation recipe is selected, */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetGlobalRectangle();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetGlobalRectangle(Byte rect_lock);
		/* Request/Set whether or not Warped with the closest Zone Registration setting */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetSnapToZoneMode();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetSnapToZoneMode(Byte enable);
		/* Whether to use Shared Local Zone */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetSharedLocalZones();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetSharedLocalZones(Byte enable);
		/* Basic information request/setting of photo head */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetAllPhotoheads();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetAllPhotoheads(Byte count, Byte pitch, UInt16 rate, Byte rotate);
		/* Setting the specification information of the photo head (Only IPv4) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetSpecPhotoHeadIPv4(Byte ph_no);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetSpecPhotoHeadIPv4(Byte ph_no, Byte[] ipv4);
		/* Position Limit information request/setting */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetTablePositionLimit(Byte tbl_num);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetTablePositionLimit(Byte tbl_num, Int32 max_x, Int32 max_y, Int32 min_x, Int32 min_y);
		/* Exposure stage activation table setting/request */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetActiveTable();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetActiveTable(Byte tbl_num);
		/* Print Simulation Out Dir information setting / request */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetPrintSimulationOutDir();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetPrintSimulationOutDir(char[] out_dir);
		/* System settings information request / settings */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetRotatePhotoheads();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetRotatePhotoheads(Byte rotate);
		/* System settings information request / settings */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetSystemSettings();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetSystemSettings(Byte emul_moto, Byte emul_head, Byte emul_trig, Byte dbg_print);
		/* Artwork Complexity Settings and Requests */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetArtworkComplexity();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetArtworkComplexity(Byte complexity);
		/* Request Optical Product Id */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetProductId();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetProductId(UInt32 pid);
		/* Photohead Focus Z Drive Type Request/Set */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetFocusZDriveType();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetFocusZDriveType(Byte type);
		/* Request/setting of the operating environment of the FAN to create over-pressure inside the photohead */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetOverPressureMode();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetOverPressureMode(Byte enable);
		/* Request/Set Depth of Focus value (Given in number of steps. Value is given by number of steps)
		   The given DOF will be used by the photo heads to determine if the current focus position during autofocus is within the DOF area or not.
		   To determine if outside DOF, the focus:OutsideDOFStatus can be read. */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetDepthOfFocus();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetDepthOfFocus(UInt16 steps);
		/* valid for LLS2500 only!!! */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetExtraLongStripes();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetExtraLongStripes(Byte enable);
		/* When exposing, request / set whether the number of exposures of the image is to be exposed 3 times (exposure) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetMTCMode();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetMTCMode(Byte enable);
		/* Request / set value whether Luria communicates with Photohead's Z-motion controller to perform AF */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetUseEthercatForAF();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetUseEthercatForAF(Byte enable);
		/* Request/Set Spx-level value */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetSpxLevel();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetSpxLevel(UInt16 level);
		/* Root Directory - Get / Set */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetRootDirectory();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetRootDirectory(char[] root, UInt32 size);		/* Request the currently selected Job Name */
		/* Get Max Jobs - Get */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetGetMaxJobs();
		/* Selected Job - Set */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetSelectedJob();
		/* Table Settings Information Request / Settings */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetTableSettings(Byte tbl_num);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetTableSettings(Byte tbl_num, UInt32 parallel, Byte y_dir);
		/* Setting Type1 information of motion controller */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetMotionType1();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetMotionType1(UInt32 acc_dist, Byte active_table);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetMotionType1Hysteresis(Byte scroll);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetMotionType1Hysteresis(Byte scroll, UInt16 offset, UInt32 pos_dir, UInt32 neg_dir);
		/* Request Photohead LED On-Time value from Luria Server */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetPhLedOnTimeAll();
		/* Request Photohead LED Temperature value from Luria Server */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetPhLedTempAll();
		/* Request Photohead LED Temperature value from Luria Server - Request value by frequency among all photoheads */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetPhLedTempFreqAll(Byte freq_no);
		/* The height difference value corresponding to the Left/Bottom or Right/Bottom Mark of the Global Fiducial and the arbitrary position of the Local Fiducial, that is, the distance value */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvEng_Luria_GetGlobalBaseMarkLocalDiffY(Byte direct, Byte index);
		/* 1 Return the number of saved Local Marks based on Scan */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvEng_Luria_GetLocalMarkCountPerScan();
		/* Whether both Local and Global Marks are registered within Gerber data */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_IsMarkMixedGlobalLocal();
		/* Focus Motor Move Absolute Position (Move the Z Axis of all Photoheads to the default position) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetAllMotorBaseAbsPosition();
		/* Only Get - Announcement Status */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetAnnouncementStatus();

		/* --------------------------------------------------------------------------------------------- */
		/*                                      Photohead Z Axis Focus                                   */
		/* --------------------------------------------------------------------------------------------- */

		/* Initialize Focus (Write only) (This will initialize the z-axis) (== Homing) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetInitializeFocusLM();
		/* 임의 Photohead Z Axis의 높이 값 설정 or 요청 */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetPositionLM(Byte ph_no, Double ph_pos/*mm*/);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetPositionLM(Byte ph_no);
		/* Start / Stop (Enable / Disable) photo head auto focus system */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetAutoFocusLM(Byte ph_no, Byte enable);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetAutoFocusLM(Byte ph_no);
		/* Autofocus set-point trim value */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetTrimLM(Byte ph_no, Double trim/*mm*/);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetTrimLM(Byte ph_no);
		/* Autofocus set-point trim value */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetActiveAreaQualifierLM(Byte enable, Double start/*mm*/, Double end/*mm*/);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetActiveAreaQualifierLM();
		/* Autofocus set-point trim value */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetAbsWorkRangeLM(Byte ph_no, Double abs_min/*mm*/, Double abs_max/*mm*/);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetAbsWorkRangeLM(Byte ph_no);
		/* return the status word giving status of autofocus absolute work range */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetAbsWorkRangeStatusLM();
		/* Autofocus Edge Trigger */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetAfEdgeTriggerLM(Double dist/*mm*/, Byte enable);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetAfEdgeTriggerLM();
		/* 모든 Photohead Z Axis의 높이 값 설정 (단위: um) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetPositionMultiLM(Byte count, Byte[] ph_no, Double[] ph_pos/*mm*/);
		/* 임의 Photohead Z Axis의 정밀 조정 (unit: nm) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetPositionHiResLM(Byte ph_no, Double ph_pos/*mm*/);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetPositionHiResLM(Byte ph_no);
		/* Reports the number of trigger pulses that autofocus position has been outside the given DOF (MachineConfig:DepthOfFocus) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetOutsideDOFStatusLM();
		/* Get focus position for all photo heads simultaneously */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetPositionMultiLM();
		/* Autofocus Gain */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetAfGainLM(UInt16 gain/*range:1 ~ 4095*/);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetAfGainLM();

		/* ------------------------------------------------------------------------------------- */
		/*                                 Luria < for XML File >                                */
		/* ------------------------------------------------------------------------------------- */

		/* Inside Gerber XML File, Get Mark Count */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvEng_Luria_GetGerberMarkCount(char[] job, ENG_AMTF type);
		/* Get Global Fiducial value in Gerber XML file <Fiducials> -> <Global> -> <fid> -> <gbr> */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvEng_Luria_GetGlobalMarkJobNameExt(char[] job, ref Double lst_x, ref Double lst_y, UInt16 count, ENG_ATGL type);
		/* Get Local Fiducial Value in Gerber XML File <Fiducials> -> <Global> -> <fid> -> <gbr> */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_GetLocalMarkJobNameExt(char[] job, ref Double lst_x, ref Double lst_y, UInt16 count, ENG_ATGL type);

		/* ------------------------------------------------------------------------------------- */
		/*                                  Luria < for Group >                                  */
		/* ------------------------------------------------------------------------------------- */

		/* All Photoheads - Get / Set / Check */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetGroupAllPhotoheadsStatic();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetGroupAllPhotoheadsStatic(Byte total_ph, Byte ph_pitch, Byte ph_rotated, Byte ctrl_type, UInt32 pid, Byte zdrv_type, Byte mtc_mode, UInt16 spx_level);
		/* desc : All Photoheads - Get / Set / Check */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetGroupAllPhotoheadsJobIndependent();
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetGroupAllPhotoheadsJobIndependent(UInt16 scroll_rate, Byte ethercat, Byte pressure);
		/* desc : specific photohead - Get / Set */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetGroupSpecificPhotohead(Byte ph_no);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetGroupSpecificPhotohead(Byte ph_no, Byte[] ipv4, UInt32 x_pos, Int32 y_pos);
		/* Motion Controller - Get / Set */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetGroupMotionController(Byte scroll_mode);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetGroupMotionController(Byte[] ipv4, UInt32 max_y_speed, UInt32 x_speed, UInt32 y_acc_dist, Byte t1_x_drv_id, Byte t1_y_drv_id, Byte t2_x_drv_id, Byte t2_y_drv_id, Byte scroll_mode, UInt16 nega_offset, UInt32 delay_posi, UInt32 delay_nega);
		/* Table Settings - Get / Set */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqGetGroupTableSettings(Byte tbl_no);
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvEng_Luria_ReqSetGroupTableSettings(Byte tbl_no, UInt32 parallel, Byte print_y_dir, UInt32 start_x, UInt32 start_y, Int32 tbl_min_x, Int32 tbl_min_y, Int32 tbl_max_x, Int32 tbl_max_y);

		/* ------------------------------------------------------------------------------------- */
		/*                                  Luria < for Common >                                 */
		/* ------------------------------------------------------------------------------------- */

		/* Whether to connect to Luria Server */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsConnected();
		/* Returns the value of the most recently occurred Luria Error Code (Status) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_Luria_GetLastErrorStatus();
		/* Message output for the most recent error code */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern void uvCmn_Luria_GetLuriaLastErrorDesc(char[] mesg, UInt32 size);
		/* Returns an error string (description) that maps to the Luria Status code */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern void uvCmn_Luria_GetLuriaStatusToDesc(UInt16 code, char[] mesg, UInt32 size);
		/* Whether there is an error currently occurring in Luria */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsLastError();
		/* Initialize the most recently occurred Luria Error Code (Status) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern void uvCmn_Luria_ResetLastErrorStatus();
		/* Whether the value of the movement range of the optical Z axis is valid */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsValidPhMoveZRange(Double pos);
		/* Data initialization indicating that all optical Z-axis homing (Mid-Position) operations have not been completed */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern void uvCmn_Luria_ResetAllPhZAxisMidPosition();
		/* Whether at least one state of all optical z-axis is homing */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsAnyPhZAxisHoming();
		/* Whether the Homing (Mid-Position) operation of all optical Z-axis has been completed */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsAllPhZAxisMidPosition();
		/* Whether all optical Z axes are idle */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsAllPhZAxisIdle();
		/* Returns the Z Axis position value of the current Photohead */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_Luria_GetPhZAxisPosition(Byte ph_no);
		/* Returns the Expose state value as a string */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_Luria_GetExposeStateToString();	/* PTCHAR -> Marshal.PtrToStringUni(PTCHAR) */
		/* Returns whether the exposure status value of the current Luria Service was successful */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsExposeStateSuccess(ENG_LCEP flag);
		/* Returns whether the current Luria Service's exposure status value is in progress */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsExposeStateRunning(ENG_LCEP flag);
		/* Returns whether the exposure status value of the current Luria Service is a failure status. */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsExposeStateFailed(ENG_LCEP flag);
		/* Returns whether the current Luria Service's exposure status value is in progress */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern ENG_LPES uvCmn_Luria_GetExposeState();
		/* In the case of exposure, the number of printing in progress */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_Luria_GetExposePrintCount();
		/* Whether there is currently selected Job Name */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsSelectedJobName();
		/* Whether the currently selected job name is loaded */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsLoadedJobName();
		/* Whether there are re-registered Job Names */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsRegisteredJobs();
		/* Initialize the current Align Mark registration status information (set as not registered) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern void uvCmn_Luria_ResetRegistrationStatus();
		/* Return current Align Mark registration status information */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt16 uvCmn_Luria_GetRegistrationStatus();
		/* Whether LED Power is set for all installed optical systems */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsPHSetLedPowerAll();
		/* Returns the number of currently registered jobs */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_Luria_GetJobCount();
		/* Whether there is a registered (received) Job Name List */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsRecvJobList();
		/* Returns the maximum exposure speed value, based on the currently set criteria in Luria. */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_Luria_GetExposeMaxSpeed(Byte step_size);
		/* Finding the exposure energy of an optical star */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_Luria_GetExposeEnergy(Byte step_size, Byte duty_cycle, UInt16 frame_rate, ref Double watt);
		/* Calculation of stage movement speed and light amount value through attribute information of the currently selected recipe */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_Luria_GetExposeSpeed(UInt16 frame_rate, Byte step_size);
		/* Based on the current exposure speed and gerber size, it is possible to obtain the time required to expose 1 stripe. */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 uvCmn_Luria_GetOneStripeExposeTime();
		/* If a path including the Gerber name is entered, the Gerber path and Gerber name are returned separately. */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_GetGerbPathName(char[] full, char[] path, char[] name);
		/* If there is a currently received Job Name, the location is returned. */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 uvCmn_Luria_GetJobRegisteredLastIndex();
		/* Initialize the contents of the last registered Job */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_ResetLastRegisteredJob();
		/* Return the starting position of the table (Stage) for exposure or alignment (return the Y position) */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_Luria_GetStartY(ENG_MDMD type, Byte tbl_no=0);
		/* Returns the exposure progress value */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Double uvCmn_Luria_GetPrintRate();
		/* Returns the current exposure progress count value */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Byte uvCmn_Luria_GetCurrentExposeScan();
		/* Registration Last Status - Returns error information */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr uvCmn_Luria_GetLastRegistrationStatus();	/* PTCHAR -> Marshal.PtrToStringUni(PTCHAR) */
		/* Whether Gerber data is registered, selected, and loaded */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsJobNameLoaded();
		/* Whether the current exposure state is Idle */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsExposeIdle();
		/* There are no errors in Photohead of Luria System */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsErrorPH();
		/* Is there any error in Luria PC of Luria System ? */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsErrorPC();
		/* Whether all values are set in Table Setting */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsTableSetting(Byte tbl_no);
		/* Whether the Motor Status of all Photoheads is initialized */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsAllMotorMiddleInited(Byte ph_count);
		/* Whether all Photohead Z axes have been moved to the Focus position */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsAllPhMotorFocusMoved();
		/* Whether the Photohead Offset information is set */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsPhOffset(Byte ph_no);
		/* Whether the Photohead Offset information is set */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsHysteresisType1();
		/* Whether the current Luria state has error information */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_IsSystemStatusError();
		/* Description: Returns an error message about the current Luria state. */
		[DllImport(Constants.dllGen2I, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
		public static extern Boolean uvCmn_Luria_GetSystemStatusErrorMesg(char[] mesg, UInt32 size);
	}
}
