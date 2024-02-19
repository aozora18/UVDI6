
/*
 desc : PreAligner and Robot for Logosol with serial
*/

#pragma once


/* --------------------------------------------------------------------------------------------- */
/*                                           상수 정의                                           */
/* --------------------------------------------------------------------------------------------- */

#define MAX_ROBOT_DRIVE_COUNT					4		/* Robot Drive의 최대 개수 (0:T/1:R1/2:R2/3:Z) */
#define MAX_ALIGNER_DRIVE_COUNT					3		/* Prealigner Drive의 최대 개수 (0:T/1:R/2:Z) */
#define MAX_CMD_MILARA_SEND						128		/* 파라미터가 포함된 송신 명령어 최대 크기 */
#define MAX_CMD_MILARA_RECV						1024	/* 파라미터가 포함된 수신 명령어 최대 크기 */

#define LAST_STRING_DATA_SIZE					1024	/* 가장 최근에 수신된 메시지 (문자열) 값이 저장될 임시 버퍼 */

/* --------------------------------------------------------------------------------------------- */
/*                                       열거형 (for common)                                     */
/* --------------------------------------------------------------------------------------------- */

/* 송신에 대한 수신 상태 */
typedef enum class __en_milara_send_ack_status__ : UINT8
{
	none				= 0xff,		/* Not defined */
	fail				= 0x00,		/* Failed */
	succ				= 0x01,		/* Success */
	bell				= 0x02,		/* Host가 Prealigner or Robot에 연속으로 데이터 2번 보내면, Prealigner or Robot으로부터 BELL (0x07 문자 받음) */

}	ENG_MSAS;

/* Milara Axis Direction */
typedef enum class __en_milara_axis_move_direction__ : UINT8
{
	amd_forward			= 0x00,	/* 정방향 */
	amd_reverse			= 0x01,	/* 역방향 */

}	ENG_MAMD;

/* Milara Axis Left / Right */
typedef enum class __en_milara_axis_left_right__ : UINT8
{
	alr_left			= 0x00,	/* 왼  쪽 */
	alr_right			= 0x01,	/* 오른쪽 */

}	ENG_MALR;

/* 동작/환경설정 등 관련 플래그 활성화 여부 */
typedef enum class __en_milara_flag_enable_disable__ : UINT8
{
	fed_disable			= 0x00,		/* Not performed the Automatic Wafer Size Detection */
	fed_enable			= 0x01,		/* Performed the Automatic Wafer Size Detection */

}	ENG_MFED;

/* Wafer Size */
typedef enum class __en_milara_wafer_kind_type__ : UINT8
{
	wkt_2				= 0x02,		/* Wafer Size :   2 inch */
	wkt_3				= 0x03,		/* Wafer Size :   3 inch */
	wkt_4				= 0x04,		/* Wafer Size :   4 inch */
	wkt_5				= 0x05,		/* Wafer Size :   5 inch */
	wkt_6				= 0x06,		/* Wafer Size :   6 inch */
	wkt_8				= 0x08,		/* Wafer Size :   8 inch */
	wkt_12				= 0x0C,		/* Wafer Size :  12 inch */
	wkt_18				= 0x12,		/* Wafer Size :  18 inch */

}	ENG_MWKT;

/* Min or Max 구분 값 */
typedef enum class __en_milara_min_max_range__ : UINT8
{
	mmr_min				= 0x00,
	mmr_max				= 0x01,

}	ENG_MMMR;

/* Apply Mode (for INF Command) */
typedef enum class __en_milara_recv_information_mode__ : UINT8
{
	en_rim_0			= 0x00,		/* Synchronous Mode, 응답이 올 때까지 무한? 대기 */
	en_rim_1			= 0x01,		/* Asynchronous Mode, 명령 송신 후, 펌웨어 쪽에서 한 Step 작업 완료 후 응답 송신 (16진수 결과 값 포함해서 전달) */
	en_rim_2			= 0x02,		/* en_rim_1과 유사하지만, 오직 프롬프로만 즉, '>' or '?' */
	en_rim_3			= 0x03,		/* en_rim_1과 유사하지만, 반환되는 값이 문자열로 값을 전달 */
	en_rim_4			= 0x04,		/* en_rim_2와 유사하지만, 상태 값 숫자 (헥사) 앞에 명령어 문자열이 포함 됨 */
	en_rim_5			= 0x05,		/* en_rim_1과 유사하지만, 상태 값 숫자 (헥사) 앞에, 명령 성공 여부 1 or 0 값이 먼저 옴 (문서 참조) */

}	ENG_MRIM;

/* Prealigner (or Robot) Axis Speed Type (Acceleration or Deceleration) */
typedef enum class __en_milara_axis_speed_type__ : UINT8
{
	ast_acl				= 0x00,	/* 가속력: inch / sec^2 or degree / sec^2 */
	ast_dcl				= 0x01,	/* 감속력: inch / sec^2 or degree / sec^2 */

}	ENG_MAST;


/* --------------------------------------------------------------------------------------------- */
/*                                     열거형 (for prealigner)                                   */
/* --------------------------------------------------------------------------------------------- */

/* Axis Name */
typedef enum class __en_prealigner_axis_name_code__ : UINT8
{
	pac_t					= 0x00,		/* T : Rotation */
	pac_r					= 0x01,		/* R : Horizontal */
	pac_z					= 0x02,		/* Z : Vertical */
	pac_a					= 0x03,		/* T|R|Z 즉, All axes */

}	ENG_PANC;

/* Prealinger Serial Command */
typedef enum class __en_prealigner_serial_command_code__ : UINT16
{
	psc_none				= 0xffff,	/* None */

	/* 1. Status and Information Commands */
	psc_mcpo				= 0x1010,	/* Read current position */
	psc_diag				= 0x1020,	/* Display current/last error diagnostics */
	psc_est					= 0x1030,	/* Extended error status */
	psc_per					= 0x1040,	/* Display reason for previous error */
	psc_inf					= 0x1050,	/* Set/Display inform mode */
	psc_rhs					= 0x1060,	/* Read home switches state */
	psc_sta					= 0x1070,	/* Command execution status */
	psc_ver					= 0x1080,	/* Read firmware version number */

	/* 2. Axis Parameters Set and Retrieve */
	psc__ch					= 0x2010,	/* Set/Display customized home position */
	psc__cl					= 0x2020,	/* Set/Display current limit */
	psc__el					= 0x2030,	/* Set/Display position error limit */
	psc__fl					= 0x2040,	/* Set/Display forward direction software limit */
	psc__ho					= 0x2050,	/* Set/Display home offset */
	psc__rl					= 0x20a0,	/* Set/Display reverse direction software limit */
	psc_acl					= 0x20b0,	/* Set/Display axis acceleration */
	psc_ajk					= 0x20c0,	/* Set/Display axis acceleration jerk */
	psc_dcl					= 0x20d0,	/* Set/Display axis acceleration */
	psc_djk					= 0x20e0,	/* Set/Display axis deceleration */
	psc_pro					= 0x20f0,	/* Set/Display axis velocity profile mode */
	psc_spd					= 0x2100,	/* Set/Display speed */

	/* 3.Basic Axis Control */
	psc_hom					= 0x3030,	/* Start homing the prealigner */
	psc_mth					= 0x3040,	/* Move to home position */
	psc_mva					= 0x3050,	/* Move to absolute position */
	psc_mvr					= 0x3060,	/* Move to relative position */

	psc_sof					= 0x3080,	/* Turn axis Servo OFF */
	psc_son					= 0x3090,	/* Turn axis Servo ON */
	psc_stp					= 0x30a0,	/* Stop motion */
	psc_wmc					= 0x30b0,	/* Wait for motion completion */

	/* 4.Alignment Commands */
	psc__ao2				= 0x4010,	/* Set/Display align offset twice */
	psc__aof				= 0x4020,	/* Set/Display acceptable offset */
	psc__asz				= 0x4030,	/* Set/Display automatic wafer Size detection */
	psc__autoscanagain		= 0x4040,	/* Set/Display align again in case of CCD data error */

	psc__finalvacc			= 0x40c0,	/* Set/Display vacuum on chuck after BAL state */

	psc_mglm				= 0x40f0,	/* Set/Display glass mode operation */

	psc__ld					= 0x4140,	/* Set/Display load down position */
	psc__lu					= 0x4150,	/* Set/Display load up position */
	psc__ma					= 0x4160,	/* Set/Display measure acceleration */
	psc__md					= 0x4170,	/* Set/Display measure deceleration */

	psc__mgd				= 0x4190,	/* Set/Display minimum good data samples */
	psc__maxwsz				= 0x41a0,	/* Set/Display maximum wafer size */
	psc__minwsz				= 0x41b0,	/* Set/Display minimum wafer size */
	psc__notchdepthmode		= 0x41c0,	/* Set/Display notch / flat depth calculation mode */
	psc__mnd				= 0x41d0,	/* Set/Display minimum notch depth */
	psc__mnw				= 0x41e0,	/* Set/Display minimum notch width */

	psc_mmtl				= 0x4200,	/* Move Wafer to the load position */
	psc_mmtm				= 0x4210,	/* Set/Display measure position */
	psc__mtpmode			= 0x4220,	/* Set/Display MTP command mode of operation */

	psc__mxd				= 0x4240,	/* Set/Display maximum notch depth */
	psc__mxw				= 0x4250,	/* Set/Display maximum notch width */
	psc__ms					= 0x4260,	/* Set/Display measure speed */

	psc__pp					= 0x4280,	/* Set/Display Z-axis coordinate at pins position */
	psc__ppcd				= 0x4290,	/* Set/Display pins position correction when going down */
	psc__ppcu				= 0x42a0,	/* Set/Display pins position correction when going up */

	psc__sa					= 0x42c0,	/* Set/Display CCD sensor angle */
	psc__sam				= 0x42d0,	/* Set/Display sample averaging mode */
	psc__sc					= 0x42e0,	/* Set/Display CCD sensor center value */

	psc__sm					= 0x4320,	/* Set/Display sampling mode */

	psc__sp					= 0x4350,	/* Set/Display stop on pins mode */
	psc__sqmax				= 0x4360,	/* Set/Display maximum CCD value for square substrates */
	psc__sqmin				= 0x4370,	/* Set/Display minimum CCD value for square substrates */

	psc__td					= 0x4390,	/* Set/Display transfer down position */
	psc__to					= 0x43a0,	/* Set/Display timeouts */
	psc__tu					= 0x43b0,	/* Set/Display transfer up position */
	psc__wszautoset			= 0x43c0,	/* Assign a set of parameters to wafer size */
	psc__wt					= 0x43d0,	/* Set/Display “wafer” type */
	psc_bal					= 0x43e0,	/* Start alignment */
	psc_ccd					= 0x43f0,	/* Read CCD sensor */

	psc_mcvf				= 0x4400,	/* Turn off the chuck vacuum */
	psc_mcvn				= 0x4410,	/* Turn on the chuck vacuum */
	psc_mpvf				= 0x4420,	/* Pins vacuum off */
	psc_mpvn				= 0x4430,	/* Pins vacuum on */
	psc_doc					= 0x4420,	/* Detect an object using CCD sensors (Wafer Loaded) */
	psc_mfwo				= 0x4430,	/* Set/Display final wafer orientation */

	psc_ltc					= 0x4450,	/* Set/Display loading type (on chuck/on pins) */
	psc_nbn					= 0x4460,	/* Display number of bad notches */
	psc_ngn					= 0x4470,	/* Display number of good notches */
	psc_ndw					= 0x4480,	/* Display notch depth and width */

	psc_rma					= 0x44d0,	/* Perform wafer misalignment */
	psc_mrwi				= 0x44e0,	/* Read wafer information */

	psc_wsz					= 0x4580,	/* Set/Display wafer size */
	psc_wsz2				= 0x4590,	/* Set/Display wafer size when two LEDs are available */

	/* 5.Housekeeping Commands */

	psc_lps					= 0x5010,	/* Load performance set parameters */
	psc_rmp					= 0x5020,	/* Restore motion and calibration parameters */
	psc_rps					= 0x5030,	/* Restore parameter sets */
	psc_smp					= 0x5040,	/* Save motion and calibration parameters */
	psc_sps					= 0x5050,	/* Save parameter sets */
	psc_wps					= 0x5060,	/* Save performance set parameters */

	/* 6.Calibration and Service Commands */
	psc__autodumps			= 0x6010,	/* Enables saving the CCD samples from unsuccessful alignment */
	psc__autodumpsf			= 0x6020,	/* Specifies the file name for CCD samples from unsuccessful alignment */
	psc__cal				= 0x6030,	/* Complete calibration procedure of standard (non-edge handling) prealigners */
	psc__cal_cor			= 0x6040,	/* Calibration procedure of CCD compensation factors */
	psc_ddpf				= 0x6050,	/* Saves CCD samples to .BAL file */
	psc_ddumpsamples		= 0x6060,	/* Saves CCD samples to .CSV file */
	psc_drps				= 0x6070,	/* Prepares CCD samples for saving to .BAL file */

	/* 7. Edge Handling Prealigner-Specific Commands */
	psc__eh					= 0x7010,	/* Verifies the prealigner is edge handling */
	psc__ehc				= 0x7020,	/* Maximum chuck CCD value */
	psc__ehc1				= 0x7030,	/* Maximum chuck CCD value */
	psc__ehc2				= 0x7040,	/* Maximum chuck CCD value */
	psc__ehp				= 0x7050,	/* Maximum chuck pins CCD value */
	psc__ehp1				= 0x7060,	/* Maximum chuck pins CCD value */
	psc__ehp2				= 0x7070,	/* Maximum chuck pins CCD value */

	psc__sfz				= 0x70b0,	/* Set/Display safe Z position above which which the chuck is clear of the stationary pins can rotate freely */

	/* 8. Exotic Commands */
	psc__mindepthmain		= 0x8010,	/* Set/Display minimum notch depth */
	psc__notchfindmode		= 0x8020,	/* Set/Display notch angle calculation mode */
	psc__notchmaxderval		= 0x8030,	/* Set/Display maximum notch/flat slope */
	psc__notchminderval		= 0x8040,	/* Set/Display minimum notch/flat slope */
	psc__notchminderwidth	= 0x8050,	/* Set/Display minimum samples with specified slope */
	psc__notchnavg			= 0x8060,	/* Set/Display number of samples outside notch/flat */

	/* 9. Miscellaneous Commands */

	psc__dt					= 0x9010,	/* Set system date and time */

	psc_hsa					= 0x9030,	/* HSA */
	psc_wsa					= 0x9040,	/* WSA */

	psc_res					= 0x9060,	/* Reset the controller */

}	ENG_PSCC;

/* Prealigner Axis Switch */
typedef enum class __en_prealigner_axis_switch_position__ : UINT8
{
	asp_h					= 0x00,		/* H : Home Switch */
	asp_f					= 0x01,		/* F : Forward Switch */
	asp_r					= 0x02,		/* R : Reverse Switch */

}	ENG_PASP;

/* Prealigner Axis position movement method (Absolute or Reference) */
typedef enum class __en_prealigner_axis_move_type__ : UINT8
{
	amt_abolute			= 0x00,		/* 절대 이동 : 좌표 위치로 이동 */
	amt_reference		= 0x01,		/* 상대 이동 : 현재 위치에서 얼마만큼 이동할지 여부 */

}	ENG_PAMT;

/* Prealigner Axis의 Sensor Control (ex> Servo OFF or ON, Vacuum OFF or ON, etc) */
typedef enum class __en_prealigner_sensor_on_off__ : UINT8
{
	soo_off				= 0x00,		/* Servo Control OFF --> Homing을 해야할 수도 있음 */
	soo_on				= 0x01,		/* Servo Control ON */

}	ENG_PSOO;

/* Prealinger의 Wafer가 놓여질 유형 (장소) 즉, Chuck or Pins */
typedef enum class __en_prealigner_wafer_place_type__ : UINT8
{
	wpt_pins			= 0x00,		/* Prealigner 쪽의 Wafer가 놓여지는 곳이 PIN */
	wpt_chuck			= 0x01,		/* Prealigner 쪽의 Wafer가 놓여지는 곳이 Chuck */

}	ENG_PWPT;

/* Prealinger의 Wafer Alignment (정렬) 검색 성공 여부 */
typedef enum class __en_prealigner_alignment_search_result__ : UINT8
{
	asr_fail			= 0x01,		/* Alignment 성공 */
	asr_succ			= 0x00,		/* Alignment 실패 */

}	ENG_PASR;

/* Prealinger의 Wafer Notch 구분 유형 */
typedef enum class __en_prealigner_notch_type_info__ : UINT8
{
	nti_depth			= 0x00,		/* Notch Depth (피자 조각 모양) */
	nti_width			= 0x01,		/* Notch Width (호의 길이) */

}	ENG_PNTI;
#if 0
/* Prealigner의 Wafer Size 구분 유형 */
typedef enum class __en_prealigner_wafer_size_type__ : UINT8
{
	wst_2_50mm			= 0x00,		/*  50 mm ( 2 inch) */
	wst_3_75mm			= 0x01,		/*  75 mm ( 3 inch) */
	wst_4_100mm			= 0x02,		/* 100 mm ( 4 inch) */
	wst_5_125mm			= 0x03,		/* 125 mm ( 5 inch) */
	wst_6_150mm			= 0x04,		/* 150 mm ( 6 inch) */
	wst_8_200mm			= 0x05,		/* 200 mm ( 8 inch) */
	wst_12_300mm		= 0x06,		/* 300 mm (12 inch) */
	wst_18_450mm		= 0x07,		/* 450 mm (18 inch) */

}	ENG_PWST;
#endif
/* Prealigner의 Wafer Size Auto Detection */
typedef enum class __en_prealigner_wafer_size_detection__ : UINT8
{
	wsd_manual			= 0x00,		/* Not performed the Automatic Wafer Size Detection */
	wsd_auto			= 0x01,		/* Performed the Automatic Wafer Size Detection */

}	ENG_PWSD;

/* Wafer의 재질이 투명한 것인지 아닌지 여부 */
typedef enum class __en_prealigner_wafer_galss_object__ : UINT8
{
	wgo_non_transparent	= 0x00,	/* 투명하지 않은 재질의 Wafer */
	wgo_transparent		= 0x01,	/*  투명한 Wafer 재질 */

}	ENG_PWGO;

/* Wafer의 Load Up/Down Position Flag */
typedef enum class __en_prealigner_wafer_load_position__ : UINT8
{
	wlp_load_down		= 0x00,		/* wafer loading position: Down */
	wlp_load_up			= 0x01,		/* wafer loading position: Up */

}	ENG_PWLP;

/* Wafer의 Sampling Mode */
typedef enum class __en_prealigner_rotation_sample_mode__ : UINT8
{
	rsm_one_third_0		= 0x00,		/* Encoder Lines의 1/3배 (아주 덜 정밀하지만, 아주 빠른 정렬 시간) */
	rsm_half_1			= 0x00,		/* Encoder Lines의 1/2배 (Half) (덜 정밀하지만, 정렬 시간 빠름) */
	rsm_default_2		= 0x00,		/* 샘플링 개수는 Chuck (T-Axis) Encoder에서 Encoder Lines의 수와 동일 (기본 값) */
	rsm_twice_3			= 0x00,		/* Encoder Lines의 2배 (더 정밀하지만, 정렬 시간 오래 걸림) */

}	ENG_PRSM;

/* Global Wafer Type */
typedef enum class __en_prealinger_global_wafer_type__ : INT8
{
	gwt_marking_notch_wafer	= -2,
	gwt_none_notch_wafer	= -1,
	gwt_one_notch_wafer		= 0,
	gwt_more_notch_wafer	= 1,
	gwt_square_substrate	= 2,

}	ENG_PGWT;

/* --------------------------------------------------------------------------------------------- */
/*                                       열거형 (for robot)                                      */
/* --------------------------------------------------------------------------------------------- */

/* Axis Name */
typedef enum class __en_robot_axis_name_code__ : UINT8
{
	rac_t				= 0x00,		/* T : Rotation */
	rac_r1				= 0x01,		/* R1: Horizontal */
	rac_r2				= 0x02,		/* R2: Horizontal */
	rac_z				= 0x03,		/* Z : Vertical */
	rac_k				= 0x04,		/* K : K axes (Not used)  */
	rac_a				= 0x05,		/* T|R|Z 즉, All axes */

}	ENG_RANC;

/* Inch To Um */
typedef enum class __en_robot_unit_inch_to_um__
{
	itu_00				= 0x02,		/* 1 값 = 0.01 */
	itu_000				= 0x03,		/* 1 값 = 0.001 */
	itu_0000			= 0x04,		/* 1 값 = 0.0001 */

}	ENG_RITU;

/* Um To Inch */
typedef enum class __en_robot_unit_um_to_inch__
{
	uti_00				= 0x02,		/* 1 값 = 100 */
	uti_000				= 0x03,		/* 1 값 = 1000 */
	uti_0000			= 0x04,		/* 1 값 = 10000 */

}	ENG_RUTI;

/* Wafer Loaded Map State */
typedef enum class __en_robot_wafer_loaded_state__ : UINT8
{
	wls_no_wafer		= 0x00,		/* No wafer present in this slot */
	wls_normal_wafer	= 0x01,		/* Single wafer is present, no other cross-slotted */
	wls_two_wafer		= 0x02,		/* Two wafers found in single slot */
	wls_wafer_cross		= 0x04,		/* Single Cross-slotted wafer */
	wls_wafer_cross_ext	= 0x05,		/* Single wafer with cross-slotted wafer on next slot */
	wls_wafer_cross_dbl	= 0x06,		/* Double-slotted, or cross-slotted wafer with another wafer */

} ENG_RWLS;

/* Diamond Robot Serial Command */
typedef enum class __en_robot_serial_command_code__ : UINT16
{
	rsc_none			= 0xffff,	/* None */

	/* 1. Informational Commands */
	rsc__dt				= 0x1010,	/* Set system date and time */
	rsc__si				= 0x1020,	/* Report System Configuration Information */
	rsc_res				= 0x1030,	/* Reset the robot controller */
	rsc_snr				= 0x1040,	/* Report serial number */
	rsc_ver				= 0x1050,	/* Report firmware version number */

	/* 2. Housekeeping (Parameters save / restore) commands */
	rsc__cld			= 0x2010,	/* Restore Custom Parameters */
	rsc__csv			= 0x2020,	/* Save Custom Parameters */
	rsc_rmp				= 0x2030,	/* Restore robot’s Motion and Calibration Parameters */
	rsc_rps				= 0x2040,	/* Restore Parameters Sets */
	rsc_rsp				= 0x2050,	/* Restore station parameters */
	rsc_smp				= 0x2060,	/* Save robot’s motion and calibration parameters */
	rsc_sps				= 0x2070,	/* Save Parameters Sets */
	rsc_rpp				= 0x2080,	/* Restore Calibration Parameters for single-axis prealigner */
	rsc_spp				= 0x2090,	/* Save Calibration Parameters for single-axis prealigner */
	rsc_ssp				= 0x20a0,	/* Save station parameters */

	/* 3. Status and diagnostic commands */
	rsc___cs			= 0x3010,	/* Report current robot state */
	rsc_cpo				= 0x3020,	/* Report axis current position */
	rsc_diag			= 0x3030,	/* Report last error information */
	rsc_eerr			= 0x3040,	/* Report extended error information */
	rsc_esta			= 0x3050,	/* Report enhanced system status */
	rsc_est				= 0x3060,	/* Report last error technical information */
	rsc_inf				= 0x3070,	/* Set/Report inform-after-motion mode */
	rsc_lop				= 0x3080,	/* Report last position object sensor triggered */
	rsc_per				= 0x3090,	/* Report previous error reason (robot) */
	rsc_rhs				= 0x30a0,	/* Report home switches state */
	rsc_sta				= 0x30b0,	/* Report command execution status */

	/* 4. Support for systems with absolute encoders */
	rsc__aer			= 0x4010,	/* Reset absolute encoder warning or error state */
	rsc__aest			= 0x4020,	/* Report absolute encoder status */
	rsc__eo				= 0x4030,	/* Set/Report encoder offset */
	rsc__eo_t			= 0x4031,	/* Set/Report encoder offset for Angle */
	rsc__eo_r1			= 0x4032,	/* Set/Report encoder offset for Horizontal R1 */
	rsc__eo_r2			= 0x4033,	/* Set/Report encoder offset for Horizontal R2 */
	rsc__eo_z			= 0x4034,	/* Set/Report encoder offset for Vertical Z */
	rsc__eor			= 0x4040,	/* Set/Report Encoder Zero Offset for all axis */
	rsc__ralmc			= 0x4050,	/* Report low-level encoder status for an Axis */

	/* 5. Basic (non-axes, non-station) action commands */
	rsc__bb				= 0x5010,	/* Turn backup DC power on/off */
	rsc_cstp			= 0x5020,	/* Enable/Disable QuickStop feature */
	rsc_din				= 0x5030,	/* Read input value from external I/O module */
	rsc_dout			= 0x5040,	/* Set output value to external I/O module */
	rsc_edg				= 0x5050,	/* Get status or operate Edge Gripper */
	rsc_inp				= 0x5060,	/* Read an input value */
	rsc_lenb			= 0x5070,	/* Enable/Disable "Loaded Speeds" feature */
	rsc_out				= 0x5080,	/* Set an output value */
	rsc_sof				= 0x5090,	/* Turn robot, axis or REE Servo OFF */
	rsc_son				= 0x50a0,	/* Check for wafer presense using vacuum */
	rsc_vof				= 0x50b0,	/* Turn vacuum OFF */
	rsc_von				= 0x50c0,	/* Turn vacuum ON */
	rsc_vst_t			= 0x50d1,	/* Check for wafer presence - Top Effector */
	rsc_vst_b			= 0x50d2,	/* Check for wafer presence - Bottom Effector */

	/* 6. Basic axis motion parameters */
	rsc__ch				= 0x6010,	/* Set/Report customized home position */
	rsc__cl				= 0x6020,	/* Set/Report axis current limit */
	rsc__el				= 0x6030,	/* Set/Report axis position error limit */
	rsc__fl				= 0x6040,	/* Set/Report axis positive software limit */
	rsc__fse			= 0x6050,	/* Set/Report axis forward safe envelope */
	rsc__hd				= 0x6060,	/* Set/Report axis PID loop differential coefficient during axis hold */
	rsc__he				= 0x6070,	/* Set/Report axis position error limit during axis hold */
	rsc__hi				= 0x6080,	/* Set/Report axis PID loop integral coefficient during axis hold */
	rsc__hl				= 0x6090,	/* Set/Report axis PID loop integral limit during axis hold */
	rsc__ho				= 0x60a0,	/* Set/Report axis home offset */
	rsc__ho_t			= 0x60a1,	/* Set/Report Axis home offset for T */
	rsc__ho_r1			= 0x60a2,	/* Set/Report Axis home offset for R1 */
	rsc__ho_r2			= 0x60a3,	/* Set/Report Axis home offset for R2 */
	rsc__ho_z			= 0x60a4,	/* Set/Report Axis home offset for Z */
	rsc__hp				= 0x60b0,	/* Set/Report axis PID loop proportional coefficient during axis hold */
	rsc__il				= 0x60c0,	/* Set/Report axis PID loop integral limit */
	rsc__kd				= 0x60d0,	/* Set/Report axis PID loop differential coefficient */
	rsc__ki				= 0x60e0,	/* Set/Report axis PID loop integral coefficient */
	rsc__kp				= 0x60f0,	/* Set/Report axis PID loop proportional coefficient */
	rsc__rl				= 0x6100,	/* Set/Report axis negative software limit */
	rsc__rse			= 0x6110,	/* Set/Report axis reverse safe envelope */
	rsc_acl				= 0x6120,	/* Set/Report axis acceleration */
	rsc_ajk				= 0x6130,	/* Set/Report axis acceleration jerk */
	rsc_dcl				= 0x6140,	/* Set/Report axis deceleration */
	rsc_djk				= 0x6150,	/* Set/Report axis deceleration jerk */
	rsc_hfp				= 0x6160,	/* Set/Report axis safe flip position */
	rsc_lacl			= 0x6170,	/* Set/Report axis loaded acceleration */
	rsc_lajk			= 0x6180,	/* Set/Report axis loaded acceleration jerk */
	rsc_ldcl			= 0x6190,	/* Set/Report axis loaded deceleration */
	rsc_ldjk			= 0x61a0,	/* Set/Report axis loaded deceleration jerk */
	rsc_lspd			= 0x61b0,	/* Set/Report axis loaded speed */
	rsc_spd				= 0x61c0,	/* Set/Report axis working speed */

	/* 7. Axis parameter sets, and custom parameters commands */
	rsc__cax			= 0x7010,	/* Set/Report custom axis parameter */
	rsc__cpa			= 0x7020,	/* Set/Report custom generic parameter */
	rsc__cst			= 0x7030,	/* Set/Report Custom Station Parameter */
	rsc_css				= 0x7040,	/* Copy One Parameter Set into Another */
	rsc_csw				= 0x7050,	/* Copy Parameter Set into Working Values */
	rsc_cwp				= 0x7060,	/* Copy PID Parameters to all Parameter Set */
	rsc_cws				= 0x7070,	/* Copy Current Working Values as Parameter Set */
	rsc_das				= 0x7080,	/* Report All Parameter Sets for a Single Axis */
	rsc_dps				= 0x7090,	/* Report a Parameter Set */
	rsc_hsa				= 0x70a0,	/* Set homing speed and acceleration */
	rsc_lsa				= 0x70b0,	/* Set loaded speed and acceleration */
	rsc_wsa				= 0x70c0,	/* Set working speed and acceleration */

	/* 8. Basic axis motions commands */
	rsc__afe			= 0x8010,	/* Find limit-switch to absolute encoder zero distance */
	rsc__fe				= 0x8020,	/* Find Edge (distance between home switch and motor index) */
	rsc__fh				= 0x8030,	/* Find Home Switch */
	rsc__hm				= 0x8040,	/* Home an Axis */
	rsc_afe				= 0x8050,	/* Find limit-switch to absolute encoder zero distance */
	rsc_hom				= 0x8060,	/* Start homing the robot */
	rsc_mov				= 0x8070,	/* Move relative over vector */
	rsc_mth				= 0x8080,	/* Move all axes to home position */
	rsc_mtb				= 0x8090,	/* Move an axis until sensor is triggered */
	rsc_mtp				= 0x80a0,	/* Move three axes to position */
	rsc_mva				= 0x80b0,	/* Move an axis to absolute position */
	rsc_mvr				= 0x80c0,	/* Move an axis to relative position */
	rsc_mvt1			= 0x80d0,	/* Move over line */
	rsc_mvt2			= 0x80e0,	/* Move over two lines */
	rsc_mvtp			= 0x80f0,	/* Set/Report End Effector Length for station-less motions */
	rsc_rdw				= 0x8100,	/* Rotate end-effector down */
	rsc_rup				= 0x8110,	/* Rotate end-effector up */
	rsc_stp				= 0x8120,	/* Stop motion */
	rsc_wmc				= 0x8130,	/* Wait for motion completed */

	/* 9. Station parameters commands */
	rsc__css			= 0x9010,	/* Set/Report current station set number */
	rsc__pa				= 0x9020,	/* Set/Report R axis pushing acceleration */
	rsc__pa_def			= 0x9021,	/* Set/Report R axis pushing acceleration (Default) */
	rsc__ps				= 0x9030,	/* Set/Report R axis pushing speed */
	rsc__ps_def			= 0x9031,	/* Set/Report R Axis pushing speed (Default) */
	rsc__zc				= 0x9040,	/* Set/Report station coordinate compensation with slot */
	rsc_app				= 0x9050,	/* Report Inline Station' Approach position */
	rsc_cof				= 0x9060,	/* Set/Report station center Offset */
	rsc_cofp			= 0x9070,	/* Set/Report additional station center Offset when using edge gripper */
	rsc_csl				= 0x9080,	/* Set/Report station current slot */
	rsc_csp				= 0x9090,	/* Clear single or all station parameters */
	rsc_cst				= 0x90a0,	/* Set/Report current station, by name */
	rsc_cstn			= 0x90b0,	/* Set/Report current station, by number */
	rsc_dsp				= 0x90c0,	/* Duplicate Station Parameters, by name */
	rsc_dspn			= 0x90d0,	/* Duplicate Station Parameters, by number */
	rsc_een				= 0x90e0,	/* Set/Report station servicing end effector */
	rsc_eeo				= 0x90f0,	/* Set/Report rotating end effector orientation for station */
	rsc_ens				= 0x9100,	/* Enumerate existing stations */
	rsc_eot				= 0x9110,	/* End of teaching a station */
	rsc_ils				= 0x9120,	/* Set/Report inline station last segment speed factor */
	rsc_irr				= 0x9130,	/* Set/Report inline station retracted R position */
	rsc_irt				= 0x9140,	/* Set/Report inline station retracted T position */
	rsc_isa				= 0x9150,	/* Set/Report inline station exit angle */
	rsc_isd				= 0x9160,	/* Set/Report inline station exit distance */
	rsc_ise				= 0x9170,	/* Set/Report inline station end effector length */
	rsc_isr				= 0x9180,	/* Set/Report inline station corner radius */
	rsc_ist				= 0x9190,	/* Set/Report inline station type */
	rsc_nsl				= 0x91a0,	/* Set/Report station number of slots */
	rsc_nst				= 0x91b0,	/* Report number of stations */
	rsc_ofs				= 0x91c0,	/* Set/Report station offset */
	rsc_pgd				= 0x91d0,	/* Set/Report station Put and Get Delay */
	rsc_pgf				= 0x91e0,	/* Put (place) and Get (pick) failure options */
	rsc_pgo				= 0x91f0,	/* Set/Report Put/Get Option and T-Z axes synchronization mode */
	rsc_pit				= 0x9200,	/* Set/Report station pitch */
	rsc_psd				= 0x9210,	/* Set/Report pushing distance */
	rsc_psh				= 0x9220,	/* Set/Report pusher type to use with a station */
	rsc_pss				= 0x9230,	/* Set/Report start pushing offset */
	rsc_psz				= 0x9240,	/* Set/Report Z push offset */
	rsc_pua				= 0x9250,	/* Set/Report station pickup Z acceleration */
	rsc_pus				= 0x9260,	/* Set/Report station pickup Z speed */
	rsc_rfl				= 0x9270,	/* Turn on or off rotate-on-the-fly permanently */
	rsc_rof				= 0x9280,	/* Turn on or off rotate-on-the-fly for next GET or PUT operation */
	rsc_rpo				= 0x9290,	/* Set/Report radial retracted station position */
	rsc_spo				= 0x92a0,	/* Set/Report station coordinate, by name */
	rsc_spon			= 0x92b0,	/* Set/Report station coordinate, by number */
	rsc_str				= 0x92c0,	/* Set/Report station' stroke distance */
	rsc_tbl				= 0x92d0,	/* Set/Report Trajectory Mode Overlap factor */
	rsc_tch				= 0x92e0,	/* Start teaching a station */
	rsc_tmd				= 0x92f0,	/* Set/Report Trajectory Motion mode */
	rsc_vchk			= 0x9300,	/* Set/Report vacuum check mode before Put and Get */
	rsc_vto				= 0x9310,	/* Set/Report vacuum timeout before Put and Get */

	/* 10.Station motion commands */
	rsc_get				= 0xa010,	/* Get (pick) a wafer from a station (by name), with end option */
	rsc_getn			= 0xa020,	/* Get (pick) a wafer from a station (by number) */
	rsc_geto			= 0xa030,	/* Get (pick) a wafer from a station, with temporary offsets from taught position */
	rsc_getr			= 0xa040,	/* Get (pick) a wafer from a station (by name), with end option */
	rsc_getrt			= 0xa050,	/* Get (pick) a wafer from a station (by name), with retry option */
	rsc_getw			= 0xa060,	/* Get (pick) a wafer from a station, with wait option at pick-up position */
	rsc_getxy			= 0xa070,	/* Get (pick) a wafer from a station with defined X/Y offsets */
	rsc_mos				= 0xa080,	/* Move arm out of station */
	rsc_mss				= 0xa090,	/* Move a wafer from station to station (by name) */
	rsc_mssn			= 0xa0a0,	/* Move a wafer from station to station (by number) */
	rsc_mta				= 0xa0b0,	/* Move to Station' Approach position */
	rsc_mtr				= 0xa0c0,	/* Move to Station' Retracted position */
	rsc_mts				= 0xa0d0,	/* Move to Station Coordinate */
	rsc_pgwa			= 0xa0e0,	/* Set/Report wait-at-station position state */
	rsc_put				= 0xa0f0,	/* Put (place) a wafer into a station (by name) */
	rsc_putn			= 0xa100,	/* Put (place) a wafer into a station (number) */
	rsc_puto			= 0xa110,	/* Put (place) a wafer into a station, with temporary offsets */
	rsc_putw			= 0xa120,	/* Put (place) a wafer into a station, with wait option at drop-down */
	rsc_sbusy			= 0xa130,	/* Check whether a station is being serviced */
	rsc_sgp				= 0xa140,	/* Move a wafer from station to station, with end option */
	rsc_slck			= 0xa150,	/* Set/Report Station Lock State */
	rsc_spg				= 0xa160,	/* Place a wafer, pick a wafer, with end option */

	/* 11.Station scanning parameters and motion commands */
	rsc__bt				= 0xb010,	/* Set/Report Beam threshold above/below a wafer (global) */
	rsc__mt				= 0xb020,	/* Set/Report Maximum Wafer Thickness (global) */
	rsc__sa				= 0xb030,	/* Set/Report Wafer Scanning Z Axis Acceleration */
	rsc__sc				= 0xb040,	/* Set Current Position as Scanning Center Coordinate (Report는 없음) */
	rsc__sl				= 0xb050,	/* Set Current Position as Scanning Left Coordinate (Report는 없음) */
	rsc__sr				= 0xb060,	/* Set Current Position as Scanning Right Coordinate (Report는 없음) */
	rsc__ss				= 0xb070,	/* Set/Report Wafer Scanning Z Axis Speed */
	rsc__st				= 0xb080,	/* Set/Report Scanner Type (Scanner Sensing Type) */
	rsc_dscn			= 0xb090,	/* Report Scanned Positions */
	rsc_map				= 0xb0a0,	/* Prints the result of last scanning operation */
	rsc_msc				= 0xb0b0,	/* Move to Scanning Coordinate */
	rsc_sbt				= 0xb0c0,	/* Set/Report Beam threshold above/below a wafer (per-station) */
	rsc_scn				= 0xb0d0,	/* Start Scanning procedure */
	rsc_scp				= 0xb0e0,	/* Set/Report scanning center position */
	rsc_scp_t			= 0xb0e1,	/* Set/Report scanning center position for Rotation (T) */
	rsc_scp_r1			= 0xb0e2,	/* Set/Report scanning center position for R1 (Horizontal) */
	rsc_scp_r2			= 0xb0e3,	/* Set/Report scanning center position for R2 (Horizontal) */
	rsc_scp_z			= 0xb0e4,	/* Set/Report scanning center position for Z (Vertical) */
	rsc_sdbg			= 0xb0f0,	/* Set/Report scanning debug mode */
	rsc_slp				= 0xb100,	/* Set/Report scanning left position */
	rsc_slp_t			= 0xb101,	/* Set/Report scanning left position for Rotation (T) */
	rsc_slp_r1			= 0xb102,	/* Set/Report scanning left position for R1 (Horizontal) */
	rsc_slp_r2			= 0xb103,	/* Set/Report scanning left position for R2 (Horizontal) */
	rsc_slp_z			= 0xb104,	/* Set/Report scanning left position for Z (Vertical) */
	rsc_smt				= 0xb110,	/* Set/Report Maximum Wafer Thickness (per-station) */
	rsc_srp				= 0xb120,	/* Set/Report scanning right position */
	rsc_srp_t			= 0xb121,	/* Set/Report scanning right position for Rotation (T) */
	rsc_srp_r1			= 0xb122,	/* Set/Report scanning right position for R1 (Horizontal) */
	rsc_srp_r2			= 0xb123,	/* Set/Report scanning right position for R2 (Horizontal) */
	rsc_srp_z			= 0xb124,	/* Set/Report scanning right position for Z (Vertical) */
	rsc_szh				= 0xb130,	/* Set/Report scanning highest Z position */
	rsc_szl				= 0xb140,	/* Set/Report scanning lowest Z position */
	rsc_tcs				= 0xb150,	/* (re)Teach scanning position */
	rsc_tsp				= 0xb160,	/* Test scanning position */

	/* 12.Align-on-Flight (On-The-Flight, OtF) parameters and commands */
	rsc_aofcn2			= 0xc010,	/* Set/Report Check for Notch on Second Scan Option */
	rsc_acs				= 0xc020,	/* Report OtF configuration set number */
	rsc_aofc			= 0xc030,	/* Execute OtF calibration run */
	rsc_aofcs			= 0xc040,	/* Execute OtF calibration run against a station */
	rsc_aofd			= 0xc050,	/* Set/Report OtF debug level */
	rsc_aofim			= 0xc060,	/* Set/Report OtF samples Ignore Mode */
	rsc_aofdump			= 0xc070,	/* Report Samples from Last OtF Operation */
	rsc_aofmd			= 0xc080,	/* Set/Report Default Maximum Allowed center offsets */
	rsc_aofm			= 0xc090,	/* Set/Report Maximum allowed center offsets */
	rsc_aofnl			= 0xc0a0,	/* Set/Report OtF Notch Limit */
	rsc_aofpmi			= 0xc0b0,	/* Report Debug Informatin from last OtF Execution */
	rsc_aofsa			= 0xc0c0,	/* Set/Report Axis Search Speed and Acceleration */
	rsc_asl				= 0xc0d0,	/* Set/Report On-The-Fly wafer radius/end effector length */
	rsc_getc			= 0xc0e0,	/* Get (pick) a wafer from a station with on-the-fly offset calculation */
	rsc_getcw			= 0xc0f0,	/* Get (pick) a wafer from a station with on-the-fly offset calculation, and wait option */
	rsc_mvrc			= 0xc100,	/* Execute relative axis motion with on-the-fly sensor capture */
	rsc_putc			= 0xc110,	/* Put (place) a wafer into a station (letter) with OtF offset calculation and correction */
	rsc_putcw			= 0xc120,	/* Put (place) a wafer into a station (letter) with OtF offset and wait option */
	rsc_putm			= 0xc130,	/* Put (place) a wafer into a station (letter) with OtF Offset Measurement */
	rsc_putxy			= 0xc140,	/* Put (place) a wafer into a station with X/Y offsets from OtF calculation */
	rsc_putxyw			= 0xc150,	/* Put (place) a wafer into a station with X/Y offsets from OtF and wait option */
	rsc_rwic			= 0xc160,	/* Report center offset from last OtF operation */

	/* 13. New Command for DUAL Arm */
	rsc_dmva			= 0xd010,	/* Move Both Arms to Same Absolute Position */
	rsc_dmvr			= 0xd020,	/* Move Both Arms Over Same Relative Distance */
	rsc_dget			= 0xd030,	/* Execute Object Pick-Up Operation with Designated or Both Arms */
	rsc_dput			= 0xd040,	/* Execute Object Place Operation with Designated or Both Arms */
	rsc_see				= 0xd050,	/* Set/Report Arm to Be Used for Wafer Mapping */

}	ENG_RSCC;

/* Absolute Encoder Clear Mode */
typedef enum class __en_robot_absolute_encoder_clear__ : UINT16
{
	aec_mode_warn		= 256,		/* Warning 지우기 (초기화) */
	aec_mode_position	= 1024,		/* 위치 보정 값 지우기 (위험?) */

}	ENG_RAEC;

/* Absolute Encoder Status */
typedef enum class __en_robot_absolute_encoder_status__ : INT16
{
	aes_na				= -1,		/* Not applicable, axis is not with absolute encoder */
	aes_healthy			= 0,		/* Absolute Encoder status OK */
	aes_err_abs			= 1,		/* Battery was disconnected, encoder readings OK */
	aes_battery_low		= 2,		/* Battery low, encoder readings OK */
	aes_battery_error	= 3,		/* Battery low or missing, encoder readings lost */

}	ENG_RAES;

/* --------------------------------------------------------------------------------------------- */
/*                                     구조체 (for prealigner)                                   */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 8)

/* PreAligner Data Value */
typedef struct __st_milara_prealigner_data_value__
{
	CHAR				last_string_data[LAST_STRING_DATA_SIZE];

	INT8				wafer_type;					/* -2: 외부 센서에 의해 그려진 Notch, -1: Notch or Flat이 없는 Wafer, 0 : Notch가 있는 Wafer, 1 : 1개 이상 Flat이 있는 Wafer, 2: 정사각형 기판이 있는 Wafer, */
	INT8				wafer_size_inch;			/* 현재 이 값 (Wafer Size (inch)) 으로 자동 사용되도록 파라미터들 설정 됨 */
	INT8				i8_reserved[6];

	UINT8				info_mode;					/* 0 ~ 3. 0 : 응답 대기, 1 ~ 3 : 1 사이클 끝내고 응답. 1 : Hex Code 값만 반환, 2 : '>' 값만 반환, 3 : 문자열만 반환 */
	UINT8				wafer_loaded;				/* Wafer가 Chuck에 놓여져 있는지 여부 */
	UINT8				wafer_size_detection;		/* 0x00: 자동으로 크기 감지 안함, 0x01: BAL 명령 수행할 때, 자동으로 크기 감지 함 */
	UINT8				wafer_scan_again;			/* 0x00: 정렬 검색 실패한 경우, 재검색 플래그 비활성화, 0x01: 정렬 검색 실패한 경우, 한번더 자동으로 재검색을 플래그 활성화 */
	UINT8				vacuum_state_after_align;	/* Align 동작 완료 후 Chuck 위에 소재 (Wafer)를 Vacuum 상태로 놔둘지 Off로 놔둘지 설정된 값 */
	UINT8				wafer_glass_type;			/* 0x00: 비투명한 재질의 Wafer, 0x01: 투명한 재질의 Wafer */
	UINT8				notch_depth_mode;			/* Notch의 Depth 해상도를 높일 것이지 여부 (0x00: 표준 해상도, 0x01: 해상도 더 높임) */
	UINT8				mode_z_after_move_pins;		/* MTP 명령 이후에 Z (Veritcal)축의 최종 위치 모드 (_LD (0) or _LU (1)) */

	UINT8				rotation_sample_mode;		/* T 축의 회전 Sampling Mode 값 (0:1/3, 1:Half, 2:Default, 3:2배) */
	UINT8				stop_on_pins;				/* Wafer 이송 중에 Chuck 모션을 Pins Position 지점에서 멈출게 할지 여부 제어 값 */
	UINT8				notch_light_source;			/* 0: 첫 번째 Light Source, 1: 두 번째 Light Source */
	UINT8				align_fail_auto_dumps;		/* Wafer에 대해 정렬 (BAL Command) 작업이 실패한 경우, 관련 CCD Data를 포함하여 파일을 자동으로 생성할지 여부 */
	UINT8				auto_samples_dump_file;		/* 생성하고자 하는 CCD Samples 파일에 대한 File Numbering 값 설정 */
	UINT8				is_edge_handling;			/* 0x00: Edge Idle, 0x01: Edge Handling */
	/* Current Limit (전류 한계) (unit: level. 1 (min) ~ 251 (max)) */
	UINT8				current_limit[MAX_ALIGNER_DRIVE_COUNT];		/* 0 : Rotation Axis (T), 1 : Horizon Axis (R), 2 : Veritical Axis (Z) */
	/* Velocity Profile (T/R/Z 순서) */
	UINT8				velocity_profile[MAX_ALIGNER_DRIVE_COUNT];	/* 0 : Rotation Axis (T), 1 : Horizon Axis (R), 2 : Veritical Axis (Z) */
	UINT8				wafer_min_size;				/* 현재 Prealinger가 작업 가능한 Wafer 최소/최대 크기 값 (2, 3, 4, 5, 6, 8, 12, 18 중 1개 값) */
	UINT8				wafer_max_size;				/* 현재 Prealinger가 작업 가능한 Wafer 최소/최대 크기 값 (2, 3, 4, 5, 6, 8, 12, 18 중 1개 값) */
	UINT8				wafer_work_size;			/* 현재 Prealinger에서 작업하려는 Target Wafer 크기 값 (2, 3, 4, 5, 6, 8, 12, 18 중 1개 값) */
	UINT8				notch_average_mode;			/* Notch/Flag 검색을 위한 Average Mode 값 (0: 이 기능 사용 안함, 3 or 5: 이 기능 사용 함 */

	UINT8				wafer_loading_type;			/* 0 : load wafer on pins, 1 : load wafer on chuck */
	UINT8				minimum_notch_depth;		/* Currently set minimum notch depth (Range : 1 ~ 100) */
	UINT8				notch_find_mode;			/* Currently set notch find mode (Range : 0 ~ 3) */
	UINT8				notch_max_der_val;			/* Currently set maximum notch/flat slope (Range : ??) */
	UINT8				notch_min_der_val;			/* Currently set minimum notch/flat slope (Range : ??) */
	UINT8				notch_min_der_width;		/* Currently set minimum samples with specified slope (Range : ??) */
	UINT8				notch_num_average;			/* Currently set number of samples outside notch/flat (Range : ??) */
	UINT8				u8_reserved[1];

	INT16				last_align_succ;
	INT16				last_align_fail;
	UINT16				u16_reserved[2];

	UINT16				align_offset_twice;			/* unit : 0.0001 inches */
	UINT16				align_offset_accept;		/* unit : 0.0001 inches */
	UINT16				axis_switch_active;			/* Homing 이후 Axis이 Actvie인지 여부 */
	UINT16				system_status;				/* 가장 최근의 컨트롤러의 상태 코드 값 */
	/* Motor Position Limit Error (1 ~ 65535) 즉, Motor Tunging Alogrithm에 적용되는 계수 값 */
	UINT16				error_limit[MAX_ALIGNER_DRIVE_COUNT];				/* 0 : Rotation Axis (T), 1 : Horizon Axis (R), 2 : Veritical Axis (Z) (Motor Encoder Counts : Max 65535) */
	UINT16				ccd_pixels;					/* Image Sensor로부터 수집된 Image의 Pixel 구성 개수 (0 ~ 4890) */
	/* 가장 최근의 Alignment 성공 or 실패 개수 */
	UINT16				previous_cmd_fail;			/* 직전에 송신한 명령어의 실패에 대한 에러 코드 값 */
	UINT16				min_good_sample_count;		/* Notch 검색하는데 GOOD 판정을 위해 필요한 CCD Sampling 개수 (장수) */
	UINT16				ccd_chuck_sensing_value;	/* 일반 Wafer를 감지하기 위해, CCD Sensor의 Sensing 최대 크기 값 (for chuck) (unit: pixels) */
	UINT16				ccd_chuck_sensing_small;	/* 작은 Wafer를 감지하기 위해, CCD Sensor의 Sensing 최대 크기 값 (for chuck) (unit: pixels) */
	UINT16				ccd_chuck_sensing_great;	/* 대형 Wafer를 감지하기 위해, CCD Sensor의 Sensing 최대 크기 값 (for chuck) (unit: pixels) */
	UINT16				ccd_pins_sensing_value;		/* 일반 Wafer를 감지하기 위해, CCD Sensor의 Sensing 최대 크기 값 (for pins)  (unit: pixels) */
	UINT16				ccd_pins_sensing_small;		/* 작은 Wafer를 감지하기 위해, CCD Sensor의 Sensing 최대 크기 값 (for pins)  (unit: pixels) */
	UINT16				ccd_pins_sensing_great;		/* 대형 Wafer를 감지하기 위해, CCD Sensor의 Sensing 최대 크기 값 (for pins)  (unit: pixels) */

	UINT32				ccd_sensor_center;			/* CCD Sensor의 중심 위치 값 (Wafer의 Notch 혹은 Flat을 측정할 수 있는 위치) (unit: pixel) */
	UINT32				last_param_timeout;			/* 가장 최근에 조회한 Parameter Timeout 값 임시 저장 (unit: msec) */
//	UINT32				u32_reserved;
	/* Notch 관련 Align 수행 후 검색된 Depth와 Width 값 (unit: ?) 저장 */
	UINT32				find_depth_width[2];		/* 0x00 : Depth, 0x01 : Width (unit: ???) */
	/* Align 검색 관련 Notch Depth 유효 값 (Min/Max) */
	UINT32				notch_range_depth_min;
	UINT32				notch_range_depth_max;
	/* Align 검색 관련 Notch Width 유효 값 (Min/Max) */
	UINT32				notch_range_width_min;
	UINT32				notch_range_width_max;
	/* CCD Data 크기 0:Min, 1:Max */
 	UINT32				ccd_grab_size_min;			/* CCD로부터 Notch 영역을 가져올 크기. 가로/세로 크기 (unit: Pixel) */
 	UINT32				ccd_grab_size_max;			/* CCD로부터 Notch 영역을 가져올 크기. 가로/세로 크기 (unit: Pixel) */

	/* Current Position */
	DOUBLE				axis_pos[MAX_ALIGNER_DRIVE_COUNT];			/* 0 : Rotation Axis (T): degree, 1 : Horizon Axis (R): um, 2 : Veritical Axis (Z): um */
	/* Home Position */
	DOUBLE				homed_pos[MAX_ALIGNER_DRIVE_COUNT];			/* 0 : Rotation Axis (T): degree, 1 : Horizon Axis (R): um, 2 : Veritical Axis (Z): um */
	/* Home Offset */
	DOUBLE				home_offset[MAX_ALIGNER_DRIVE_COUNT];		/* 0 : Rotation Axis (T): degree, 1 : Horizon Axis (R): um, 2 : Veritical Axis (Z): um */
	/* Direction (Forward or Reverse) Software Limit */
	DOUBLE				direct_f_limit[MAX_ALIGNER_DRIVE_COUNT];	/* 0 : Rotation Axis (T): degree, 1 : Horizon Axis (R): um, 2 : Veritical Axis (Z): um */
	DOUBLE				direct_r_limit[MAX_ALIGNER_DRIVE_COUNT];	/* 0 : Rotation Axis (T): degree, 1 : Horizon Axis (R): um, 2 : Veritical Axis (Z): um */
	/* Acceleration / Deceleration (sec^2) */
	DOUBLE				axis_acl[MAX_ALIGNER_DRIVE_COUNT];			/* 0 : Rotation Axis 회전 각도 (Degree/sec^2), 1 : Vertical Motion 위치 (um/sec^2), 2 : Horizontal Motion 위치 (um/sec^2) */
	DOUBLE				axis_dcl[MAX_ALIGNER_DRIVE_COUNT];			/* 0 : Rotation Axis 회전 각도 (Degree/sec^2), 1 : Vertical Motion 위치 (um/sec^2), 2 : Horizontal Motion 위치 (um/sec^2) */
	/* Acceleration / Deceleration (sec^3) for JERK */
	DOUBLE				axis_ajk[MAX_ALIGNER_DRIVE_COUNT];			/* 0 : Rotation Axis 회전 각도 (Degree/sec^3), 1 : Vertical Motion 위치 (um/sec^3), 2 : Horizontal Motion 위치 (um/sec^3) */
	DOUBLE				axis_djk[MAX_ALIGNER_DRIVE_COUNT];			/* 0 : Rotation Axis 회전 각도 (Degree/sec^3), 1 : Vertical Motion 위치 (um/sec^3), 2 : Horizontal Motion 위치 (um/sec^3) */
	/* Speed (degree or um / sec^2) */
	DOUBLE				axis_speed[MAX_ALIGNER_DRIVE_COUNT];		/* 0 : Rotation Axis 회전 각도 (Degree/sec^2), 1 : Vertical Motion 위치 (um/sec^2), 2 : Horizontal Motion 위치 (um/sec^2) */
	/* Align 검색 후 Notch 기준 회전할 각도 값 */
	DOUBLE				rotate_after_align;			/* 실제 Notch 검색 성공 후 Wafer를 회전할 각도 값 */
	/* RWI 즉, Wafer Information ([0] Offset:Center (um), [1] Offset:Angle (degree), [2] Angle:R-Axis vs. Notch (degree) */
	DOUBLE				rwi_wafer_info[MAX_ALIGNER_DRIVE_COUNT];
	/* Wafer Loading Up/Down 시점의 Position 값 */
	DOUBLE				wafer_loading_down_pos[MAX_ALIGNER_DRIVE_COUNT];/* Z : Vertical Axis 수직 위치 (um). 0:Up, 1:Down */
	DOUBLE				wafer_loading_up_pos[MAX_ALIGNER_DRIVE_COUNT];	/* Z : Vertical Axis 수직 위치 (um). 0:Up, 1:Down */
	/* measurement acceleration */
	DOUBLE				measure_acceleration[MAX_ALIGNER_DRIVE_COUNT];	/* 0 : Rotation Axis 회전 각도 (Degree/sec^2), 1 : Vertical Motion 위치 (um/sec^2), 2 : Horizontal Motion 위치 (um/sec^2) */
	/* measurement deceleration */
	DOUBLE				measure_deceleration[MAX_ALIGNER_DRIVE_COUNT];	/* 0 : Rotation Axis 회전 각도 (Degree/sec^2), 1 : Vertical Motion 위치 (um/sec^2), 2 : Horizontal Motion 위치 (um/sec^2) */
	/* measurement speed */
	DOUBLE				measure_speed[MAX_ALIGNER_DRIVE_COUNT];			/* 0 : Rotation Axis 회전 각도 (Degree/sec), 1 : Vertical Motion 위치 (um/sec), 2 : Horizontal Motion 위치 (um/sec) */
	/* Z Axis Position (Chuck과 Pins가 수평되는 위치) */
	DOUBLE				level_chuck_pins_pos_z;		/* Chuck과 Pins가 수평인 위치의 Prealinger의 Z 축 위치 (unit: um) */
	/* Moving Up or Down 시점의 Offset 값 */
	DOUBLE				offset_chuck_pin_pos_up;	/* Z 축을 Up or Down 시점의 발생할 수 있는 Backlash에 대한 허용 Offset 인정 값 (unit: um) */
	DOUBLE				offset_chuck_pin_pos_down;	/* Z 축을 Up or Down 시점의 발생할 수 있는 Backlash에 대한 허용 Offset 인정 값 (unit: um) */
	/* CCD Sensor와 R (Horizontal) 축 간의 각도 값 */
	DOUBLE				ccd_r_axis_angle;			/* 수평 축과 CCD Sensor 간의 설치(?)된 각도 값 입력 (Wafer Size는 고려 안됨) (unit: degrees) */
	/* Wafer가 Chuck에서 안전하게 이동하기 위한 Chuck의 위치 */
	DOUBLE				wafer_trans_safe_pos_up;	/* Wafer가 Chuck에서 안전하게 이동 (0:가져다 놓을 때 (DOWN) 혹은 1:가지러 갈 때 (UP)하기 위한 Chuck의 위치. unit: um)*/
	DOUBLE				wafer_trans_safe_pos_down;	/* Wafer가 Chuck에서 안전하게 이동 (0:가져다 놓을 때 (DOWN) 혹은 1:가지러 갈 때 (UP)하기 위한 Chuck의 위치. unit: um)*/
	DOUBLE				safe_rotate_z_pos;			/* Wafer가 올려지고, 안전하게 회전하는데  필요한 Z 축의 높이 (unit: um) */

	/*
	 desc : 각 모션 축의 Homing 상태 정보 얻기
	 parm : axis	- [in]  축 명칭 (Veritical, Horizontal, Rotation)
			type	- [in]  스위치 위치 (종류) (Home, Forward, Reverse)
	 retn : TRUE or FALSE
	*/
	BOOL IsMotionAxisChecked(ENG_PANC axis, ENG_PASP type)
	{
		UINT16 u16Mask	= axis_switch_active;

		/* 축 정보에 해당되는 4 bit 값 얻기 */
		switch (axis)
		{
		case ENG_PANC::pac_z	:	u16Mask	= (u16Mask & 0x0f00) >> 8;	break;
		case ENG_PANC::pac_r	:	u16Mask	= (u16Mask & 0x00f0) >> 4;	break;
		case ENG_PANC::pac_t	:	u16Mask	= (u16Mask & 0x000f);		break;
		default					:	return FALSE;
		}
		/* Home (or Limit)에 해당되는 비트 위치 값 얻기 */
		switch (type)
		{
		case ENG_PASP::asp_h	:	return (u16Mask & 0x0001) > 0;
		case ENG_PASP::asp_f	:	return (u16Mask & 0x0002) > 0;
		case ENG_PASP::asp_r	:	return (u16Mask & 0x0004) > 0;
		}

		/* 무조건 실패*/
		return FALSE;
	}

	/*
	 desc : System Status 값이 유효한 상태인지 여부
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsValidSystemStatus()
	{
		return (0xffff != system_status);
	}

	/*
	 desc : Prealigner에 문제가 생겼는지 여부
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsProblemPrealigner()
	{
		if (0x0001 == (system_status & 0x0001))	return TRUE;	/* Unable to execute motion commands */
		if (0x0002 == (system_status & 0x0002))	return TRUE;	/* Previous command returned an error. See PER command for details */
		if (0x0010 == (system_status & 0x0010))	return TRUE;	/* Motion Error */
		if (0x0020 == (system_status & 0x0020))	return TRUE;	/* Motion limits violation */
		if (0x0040 == (system_status & 0x0040))	return TRUE;	/* Motion limits violation */
		if (0x0400 == (system_status & 0x0400))	return TRUE;	/* I/O File error */
		if (0x2000 == (system_status & 0x2000))	return TRUE;	/* Servo OFF on one or more axes */

		return FALSE;
	}

	/*
	 desc : Prealigner에 동작 중인지 여부
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsBusyPrealigner()
	{
		return (0x0100 == (system_status & 0x0100) ||
				0x0080 == (system_status & 0x0080));
	}

	/*
	 desc : Wafer Scan 동작을 해도 되는지 여부
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsRunWaferScan()
	{
		if (IsProblemPrealigner())	return FALSE;
		if (IsBusyPrealigner())		return FALSE;

		/* Chuck Vacuum Sensor & Switch On 여부에 따라 */
		return (system_status & 0x000C /* 0x0004 | 0x0008 */);
	}

}	STG_MPDV,	*LPG_MPDV;

#pragma pack (pop)	/* 8 bytes order */


/* --------------------------------------------------------------------------------------------- */
/*                                       구조체 (for robot)                                      */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 8)

/* Diamond Robot Data Value */
typedef struct __st_milara_robot_data_value__
{
	CHAR				station_name;											/* 현재 선택된 Station Name 값 임시 저장 */
	CHAR				serial_number[10];										/* Serial number of the robot */
	CHAR				c_reserved[5];
	CHAR				last_string_data[LAST_STRING_DATA_SIZE];

	INT8				otf_debug_level;										/* Current debug level, or calibration parameters (Default 0, Max : 15) */
	INT8				i8_reserved[7];

	UINT8				wafer_map_loaded_state[32];								/* Cassette or Station에 Wafer가 적재된 상태 즉, 0 ~ 6 (ENG_RWLS) 값 중 한 개 값이 등록됨 */
	UINT8				current_limit[MAX_ROBOT_DRIVE_COUNT];					/* 각 축 (T, R1, R2, Z)의 현재 전류의 한계 값 (1 ~ 251) */
	UINT8				controlled_stop[2];										/* Enable/Disable Controlled Stop Feature. 0:State, 1:Option (0:활성화 출력 안함, 1:활성화 출력 함) */
	UINT8				info_mode;												/* 0 ~ 5. 0 : 응답 대기, 1 ~ 3 : 1 사이클 끝내고 응답. 1 : Hex Code 값만 반환, 2 : '>' 값만 반환, 3 : 문자열만 반환, 4 : 명령어 포함 반환, 5 : 에러 코드 정보 포함 반환 */
	UINT8				station_set_no;											/* current station set number (1 ~ 4) : Station Number들의 그룹 (총 4개까지 지원) */

	UINT8				station_no;												/* Current station number. (1 ~ 130) */
	UINT8				scanner_type;											/* Current scanner type. 0 for reflective type of scanner, 1 for thru-beam type of scanner */
	UINT8				current_slot_no;										/* Current slot of a station */
	UINT8				end_effector_num;										/* 가장 최근에 조회된 Station에 사용되는 End Effector Number (0 or 1) */
	UINT8				inline_station_type;									/* 0: radial in / radial out, 1: radial in / linear out, 2: linear in / linear out */
	UINT8				loaded_speed_enable;									/* Current state of loaded parameters 즉, 사용 여부 */
	UINT8				station_slot_num;										/* 가장 최근의 Station에 설정된 Slot의 개수 */
	UINT8				station_num;											/* 가장 최근의 등록된 Station의 개수 */

	UINT8				put_get_failed_option[2];								/* Current failure options for GET/PUT */
	UINT8				put_get_option[2];										/* Current options for GET/PUT */
	UINT8				pusher_type;											/* Pusher Type (0: Not defined, 1: pushing is defined) */
	UINT8				station_service_busy;									/* Numeric value, 0 (Not busy) or 1 (Busy 즉, service 중 (상태)) */
	UINT8				station_locked_state;									/* Current station lock state (가장 최근에 설정한 Station State) */
	UINT8				station_axis_overlap_factor;							/* Currently set motion overlap factor (0 ~ 95%) */

	UINT8				station_trajectory_mode;								/* Currently set trajectory mode */
	UINT8				station_vacuum_check[2];								/* [0]:Top Effector, [1]:Bottom Effector, Current wafer (vacuum) check options. 0: PUT, 1: GET */
	UINT8				end_effector_wafer_check[2];							/* [0]:Top Effector, [1]:Bottom Effector, 0: Wafer was not detected; 1: Wafer was detected */
	UINT8				motion_wait_finished;									/* 0: 현재 모션이 동작 중 ..., 1: 모든 모션이 동작 완료된 상태  */
	UINT8				otf_set_second_scan_notch;								/* 0 (Notch 검색 무시) or 1 (Notch 검색 강제로 진행) */
	UINT8				otf_ignore_middle_samples;								/* 0 : 중간에 얻어진 샘플 무시하지 않음. 1 : 처음과 끝 제외한 모든 샘플 무시 */

	UINT8				scan_debug_mode;										/* 0 : disable, 1 : Enable (for Scan Debug Mode)*/
	UINT8				arm_used_scan_spec_station;								/* optional arm no, 0 (primary), or 1 (secondary) */
	UINT8				end_effector_up_down_station;							/* End Effector가 Station 정보를 이용하여 Wafer를 GET or PUT 동작할 때, Z 축을 UP (0x00) or DOWN (0x01) 처리 */
	UINT8				station_last_seg_speed_factor;							/* Inline Station Last Segment Speed Factor */
	UINT8				u8_reserved[4];

	INT16				enc_status[MAX_ROBOT_DRIVE_COUNT];						/* Absolute Encoder status (-1 : 이용 불가 (엔코더 아님), 0:Healthy, 1:ErrABS, 2:BatteryLOW), T, R1, R2, Z, K (N/A) 축 순서대로 저장 */

	UINT16				low_level_enc_status;									/* Low-level absolute encoder status (Last Axis) */
	UINT16				digital_input_state;									/* Input state of given or all digital inputs (0 ~ 8 pin) */
	UINT16				digital_output_state;									/* Output state of given or all digital inputs (0 ~ 8 pin) */
	UINT16				home_switch_state;										/* All robot axes home switches state. (순서대로: K, Z, R, T) */

	UINT16				extended_error_code;									/* Error Code Returned by EERR */
	UINT16				backup_dc_power[2];										/* Turn Backup DC Power On or Off */
	UINT16				otf_set_number;											/* Currently set on-the-fly set number */

	UINT16				axis_acceleration_jerk[MAX_ROBOT_DRIVE_COUNT];			/* Currently set axis acceleration jerk */
	UINT16				axis_deceleration_jerk[MAX_ROBOT_DRIVE_COUNT];			/* Currently set axis deceleration jerk */
	UINT16				axis_loaded_acceleration_jerk[MAX_ROBOT_DRIVE_COUNT];	/* Currently set axis acceleration jerk when loaded with a wafer */
	UINT16				axis_loaded_deceleration_jerk[MAX_ROBOT_DRIVE_COUNT];	/* Currently set axis deceleration jerk when loaded with a wafer */

	UINT16				system_status;											/* System status word */
	UINT16				u16_reserved[3];

	INT32				position_error_limit[MAX_ROBOT_DRIVE_COUNT];			/* Currently set position error limit of one or all axes (unit: Encoder Counts) */
	INT32				custom_generic_param;									/* Custom generic parameter value */
	INT32				custom_axis_param;										/* Custom Axis parameter */
	INT32				custom_station_param;									/* Custom Station parameter */

	UINT32				external_di_in_state;									/* Input state of digital input */
	UINT32				external_di_out_state;									/* Last state set to digital output */
	UINT32				put_get_delay_time;										/* Currently set delay for PUT and GET (unit: mill-seconds) */
	UINT32				vacuum_search_time_out;									/* Currently set time out (Station에서 Wafer 검색하는데, Vacuum Search Time Out: milli-seconds 값) */
	UINT32				axis_diff_coeff_hold[MAX_ROBOT_DRIVE_COUNT];			/* Currently Differential Coefficient of one or all axes */
	UINT32				axis_prop_coeff_hold[MAX_ROBOT_DRIVE_COUNT];			/* Currently set proportional coefficient during axis hold */
	UINT32				axis_inte_coeff[MAX_ROBOT_DRIVE_COUNT];					/* Currently Integral Coefficient of one or all axes */
	UINT32				axis_inte_coeff_hold[MAX_ROBOT_DRIVE_COUNT];			/* Currently Integral Coefficient of one or all axes hold */
	UINT32				axis_diff_coeff[MAX_ROBOT_DRIVE_COUNT];					/* Currently Differential Coefficient of one or all axes */
	UINT32				axis_prop_coeff[MAX_ROBOT_DRIVE_COUNT];					/* Currently set proportional coefficient of one or all axes */

	DOUBLE				homed_position[MAX_ROBOT_DRIVE_COUNT];					/* Currently set customized home position of all the axes (Homing 이후, 이동될 각 축의 Homed Position  */
	DOUBLE				enc_zero_offset[MAX_ROBOT_DRIVE_COUNT];					/* Currently set encoder zero offsets of all the axes (Homing 이후) */
	DOUBLE				sw_positive_limit[MAX_ROBOT_DRIVE_COUNT];				/* Currently set forward direction software limit for all the axes */
	DOUBLE				sw_negative_limit[MAX_ROBOT_DRIVE_COUNT];				/* Currently set backward direction software limit for all the axes */
	DOUBLE				maximum_wafer_thick;									/* Currently set maximum wafer thickness */
	DOUBLE				r_axis_push_acc_def;									/* Current value of R axis pushing acceleration (Default) */
	DOUBLE				r_axis_push_acc_sta;									/* Current value of R axis pushing acceleration for last station */
	DOUBLE				r_axis_push_spd_def;									/* Current value of R axis pushing speed (Default) */
	DOUBLE				r_axis_push_spd_sta;									/* Current value of R axis pushing speed for last station */
	DOUBLE				wafer_scan_z_axis_acc;									/* Current wafer scanning Z axis acceleration for given station (for Last Station) */
	DOUBLE				wafer_scan_z_axis_spd;									/* Current wafer scanning Z axis speed for given station (for Last Station) */
	DOUBLE				coord_compensation_slot[MAX_ROBOT_DRIVE_COUNT];			/* Currently offset compensation with slot for T, R1, R2, Z */
	DOUBLE				axis_acceleration[MAX_ROBOT_DRIVE_COUNT];				/* Currently set axis acceleration */
	DOUBLE				axis_deceleration[MAX_ROBOT_DRIVE_COUNT];				/* Currently set axis deceleration */
	DOUBLE				axis_loaded_acceleration[MAX_ROBOT_DRIVE_COUNT];		/* Currently set axis acceleration when loaded with a wafer */
	DOUBLE				axis_loaded_deceleration[MAX_ROBOT_DRIVE_COUNT];		/* Currently set axis deceleration when loaded with a wafer */
	DOUBLE				axis_loaded_speed[MAX_ROBOT_DRIVE_COUNT];				/* Currently set loaded speed */
	DOUBLE				axis_working_speed[MAX_ROBOT_DRIVE_COUNT];				/* Currently set working speed */
	DOUBLE				axis_safe_envelop_forward[MAX_ROBOT_DRIVE_COUNT];		/* Currently Forward Safe Envelop */
	DOUBLE				axis_safe_envelop_reverse[MAX_ROBOT_DRIVE_COUNT];		/* Currently Reverse Safe Envelop */
	DOUBLE				pos_err_limit_hold[MAX_ROBOT_DRIVE_COUNT];				/* Currently position error limit of one or all axes hold */
	DOUBLE				axis_inte_limit_hold[MAX_ROBOT_DRIVE_COUNT];			/* Currently set integral limit of one or all axes hold */
	DOUBLE				axis_integral_limit[MAX_ROBOT_DRIVE_COUNT];				/* Currently set integral limit of on or all axes */
	DOUBLE				home_flip_position[MAX_ROBOT_DRIVE_COUNT];				/* Currently REE Homing Flip Position */
	DOUBLE				beam_threshold[2];										/* Currently set maximum beam thresholds for Global (Upper / Bottom) */
	DOUBLE				station_center_offset;									/* Currently set center offset (for R Axis) */
	DOUBLE				station_center_offset_edge_grip;						/* Currently set center offset (for R Axis) for Edge Gripper */
	DOUBLE				axis_current_position[MAX_ROBOT_DRIVE_COUNT];			/* Current position of all axes (sequence : T, R1, R2, Z. Reference to ENG_RANC) */
	DOUBLE				station_retracted_r_position;							/* reports Inline Station Retracted R position for station Station */
	DOUBLE				station_retracted_t_position;							/* reports Inline Station Retracted T position for station Station */
	DOUBLE				station_exit_angle;										/* Report Inline Station Exit Angle (degree) */
	DOUBLE				station_exit_distance;									/* Report Inline Station Exit Distance (um) */
	DOUBLE				station_end_effect_length;								/* Report Inline Station End-Effector Length (um) */
	DOUBLE				station_corner_radius;									/* Report Inline Station Corner Radius (degree) */
	DOUBLE				station_offset_z;										/* Set/Report Station Offset for Z Axis (um) */
	DOUBLE				station_pitch;											/* Currently set pitch (um) */
	DOUBLE				start_pushing_distance;									/* Current value of Start Pushing Distance (um) */
	DOUBLE				station_start_pushing_offset;							/* Current value of Start Pushing Offset (um) */
	DOUBLE				station_pushing_offset_z;								/* Current value of Z push offset */
	DOUBLE				station_pickup_acceleration_z;							/* Currently set pickup acceleration */
	DOUBLE				station_pickup_speed_z;									/* Currently set pick-up speedn (unit: um) */
	DOUBLE				station_retracted_position_r;							/* Currently set position (for R Axis. Last Position) */
	DOUBLE				station_per_beam_threshold[2];							/* Currently set maximum beam thresholds for Station (Upper / Bottom) */
	DOUBLE				station_scanning_center_coord[MAX_ROBOT_DRIVE_COUNT];	/* Current Scanning Center Coordinate, for all or given axis */
	DOUBLE				station_scanning_left_coord[MAX_ROBOT_DRIVE_COUNT];		/* Current Scanning Left Coordinate, for all or given axis */
	DOUBLE				station_scanning_right_coord[MAX_ROBOT_DRIVE_COUNT];	/* Current Scanning Right Coordinate, for all or given axis */
	DOUBLE				station_wafer_max_thickness;							/* Currently set maximum wafer thickness for Station (unit: um) */
	DOUBLE				station_position[MAX_ROBOT_DRIVE_COUNT];				/* Currently set station position */
	DOUBLE				station_scaning_highest_z_coord;						/* Current Scanning Highest Z Position, for a given or default station */
	DOUBLE				station_scaning_lowest_z_coord;							/* Current Scanning Lowest Z Position, for a given or default station */
	DOUBLE				station_stroke_z;										/* Current Z Axis Stroke Position (um) */

}	STG_MRDV,	*LPG_MRDV;

#pragma pack (pop)	/* 8 bytes order */