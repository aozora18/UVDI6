//
///*
// desc : System Configuration Information
//*/
//
//#pragma once
//
//#include "conf_comn.h"
//
///* --------------------------------------------------------------------------------------------- */
///*                                           상수 값                                             */
///* --------------------------------------------------------------------------------------------- */
//
//
///* --------------------------------------------------------------------------------------------- */
///*                                       전역(Global) 열거형                                     */
///* --------------------------------------------------------------------------------------------- */
//
//
///* --------------------------------------------------------------------------------------------- */
///*                                Config 구조체 - Luria (Service)                                */
///* --------------------------------------------------------------------------------------------- */
//
//#pragma pack (push, 8)
//
///* PreAligner / Diamond Robot */
////typedef struct __st_config_logosole_serial_comm__
////{
////	UINT8				use_lib;			/* Prealigner or Robot Library 사용 여부 (CCF에서 사용할 수도 있기 때문에) */
////	UINT8				port_no;			/* Port Number (2 ~ 19) */
////	UINT8				ext_param_1;		/* 확장 정보. 예를들어, EFU의 경우, 설치된 EFU의 개수 */
////	UINT8				u8_reserved[5];
////	UINT32				baud_rate;			/* 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 1152000 */
////	UINT32				port_buff;			/* 통신 버퍼 크기 즉, 송신/수신 버퍼 크기 (단위: bytes) */
////
////}	STG_CLSC,	*LPG_CLSC;
//
///* 각종 파일 이름 */
////typedef struct __st_config_file_data_name__
////{
////	TCHAR				led_power[MAX_FILE_LEN];
////	TCHAR				gerb_recipe[MAX_FILE_LEN];
////	TCHAR				job_recipe[MAX_FILE_LEN];
////	TCHAR				expo_recipe[MAX_FILE_LEN];
////	TCHAR				ph_step[MAX_FILE_LEN];
////	TCHAR				acam_cali[MAX_FILE_LEN];
////	TCHAR				correct_y[MAX_FILE_LEN];
////	TCHAR				staticAcamCali[3][MAX_FILE_LEN];
////
////}	STG_CFDN,	*LPG_CFDN;
//
///* 소재의 Edge Line Detection 관련 정보 */
////typedef struct __st_config_info_engine_all__
////{
////	STG_CFDN			file_dat;
////	STG_CLSI			luria_svc;
////	STG_CMSI			mc2_svc;
////	STG_CPSI			melsec_q_svc;
////	STG_CSDM			monitor;
////	STG_CCSI			set_comm;
////	STG_CCGS			set_comn;
////	STG_CLSC			pre_aligner;
////	STG_CLSC			diamond_robot;
////	STG_CLSC			efu;
////
////	/*
////	 desc : 데모 (Emulation Mode) 모드로 동작하는지 여부
////	 parm : None
////	 retn : TRUE or FALSE
////	*/
////	BOOL IsRunDemo()
////	{
////		return set_comn.run_emulate_mode ? TRUE : FALSE;
////	}
////
////}	STG_CIEA,	*LPG_CIEA;
//
///* Teaching Position 의 동작 정보 */
//typedef struct __st_config_teaching_position_info__
//{
//	UINT8				e_tower_lamp;
//	UINT8				e_shutter;
//	UINT8				e_align_expo_select;
//	UINT8				e_error_skip;
//	UINT8				e_cal_offset;
//	UINT8				e_auto_focus;
//	UINT8				e_stitch_calib;
//	UINT8				mark_refind_count;				/* Mark Refind Count */
//	UINT8				e_robot_info_mode;				/* Robot Info Mode 0:동기식(WMC로 모션 완료를 확인한다) / 1:비동기식(STA로 동작완료를 확인한다.)*/
//	UINT8				expo_align;						/* Align or Direct */
//	UINT8				expo_rectangle_lock;			/* Rectangle Lock */
//	UINT8				expo_scroll_mode_1;				/* 노광 동작 속도 모드 값 */
//	UINT8				expo_neg_offset_1;				/* HysteresisType1 Negative Offset */
//	UINT8				expo_duty_cycle;				/* */
//	UINT8				u8_reserved[2];
//
//	INT32				expo_start_x_pos;				/* Expose Start X axis Position */
//	INT32				expo_start_y_pos;				/* Expose Start Y axis Position */
//	INT32				gerber_0_x_pos;					/* 거버 (0,0) 의 위치 X AXIS */
//	INT32				gerber_0_y_pos;					/* 거버 (0,0) 의 위치 Y AXIS */
//	INT32				load_x_pos;						/* Wafer Loading Position X (Wafer를 로봇으로 부터 받는 위치)*/
//	INT32				load_y_pos;						/* Wafer Loading Position Y */
//	INT32				unload_x_pos;					/* Wafer UnLoading Position X */
//	INT32				unload_y_pos;					/* Wafer UnLoading Position Y */
//	INT32				orgin_x_pos;					/* Stage Orgin Position X */
//	INT32				orgin_y_pos;					/* Stage Orgin Position Y */
//	INT32				pin_load_x;						/* pin stage가 이동 가능 stage 위치 x */
//	INT32				pin_load_y;						/* pin stage가 이동 가능 stage 위치 y */
//	INT32				pin_unload_x;					/* stage가 이동 가능 pin stage 위치 x */
//	INT32				pin_unload_y;					/* stage가 이동 가능 pin stage 위치 y */
//	INT32				focus_quaz_bsa_z_pos;			/* Quaz Point 1 에 대한 BSA CAM Z Position	*/
//	INT32				focus_mark1_bsa_z_pos;			/* Mark 1 에 대한 BSA CAM Z Position	*/
//	INT32				focus_mark2_bsa_z_pos;			/* Mark 2 에 대한 BSA CAM Z Position	*/
//	INT32				idle_bsa_z_pos;					/* BSA 카메라 Down시 Z Position	*/
//	INT32				focus_quaz_up_z_pos;			/* Quaz Point 1 에 대한 UP CAM Z Position	*/
//	INT32				mark_refind_range;				/* Mark Refind range [mm] */
//	INT32				focus_mark1_up_z_pos;			/* Mark 1 에 대한 UP CAM Z Position	*/
//	INT32				focus_mark2_up_z_pos;			/* Mark 2 에 대한 UP CAM Z Position	*/
//	INT32				pin_stage_z_up;					/* Lift Pin up(focus)시 Z Position	*/
//	INT32				pin_stage_z_down;				/* Lift Pin down시 Z Position	*/
//	INT32				pin_stage_x_input;				/* Lift Pin in시 X Position	*/
//	INT32				pin_stage_x_output;				/* Lift Pin out(focus)시 X Position	*/
//	INT32				expo_offset_x;					/* 노광 START 위치에서 UP카메라 까지의 거리 X */
//	INT32				expo_offset_y;					/* 노광 START 위치에서 UP카메라 까지의 거리 Y */
//	INT32				cam_offset_x;					/* 두개의 카메라(BSA & UP) 간의 차이값 x */
//	INT32				cam_offset_y;					/* 두개의 카메라(BSA & UP) 간의 차이값 y */
//	INT32				bsa_up_down_offset_x;			/* BSA UP/DOWN에 따른 오차값 보정 OFFSET X (mm) */
//	INT32				bsa_up_down_offset_y;			/* BSA UP/DOWN에 따른 오차값 보정 OFFSET Y (mm) */
//	INT32				quartz_center_offset_x;			/* STAGE 위치 정밀도 보정 OFFSET X (mm) */
//	INT32				quartz_center_offset_y;			/* STAGE 위치 정밀도 보정 OFFSET Y (mm) */
//	INT32				cent_quaz_bsa_x_pos;			/* Quaz Point 1 에 대한 BSA CAM Centering X axis Position */
//	INT32				cent_quaz_bsa_y_pos;			/* Quaz Point 1 에 대한 BSA CAM Centering Y axis Position */
//	INT32				cent_mark1_bsa_x_pos;			/* Mark 1 에 대한 BSA CAM CAMCentering X axis Position */
//	INT32				cent_mark1_bsa_y_pos;			/* Mark 1 에 대한 BSA CAMCAMCentering Y axis Position */
//	INT32				cent_mark2_bsa_x_pos;			/* Mark 2 에 대한 BSA CAM Centering X axis Position	*/
//	INT32				cent_mark2_bsa_y_pos;			/* Mark 2 에 대한 BSA CAM Centering Y axis Position	*/
//	INT32				cent_quaz_up_x_pos;				/* Quaz Point 1 에 대한 UP CAM Centering X axis Position */
//	INT32				cent_quaz_up_y_pos;				/* Quaz Point 1 에 대한 UP CAM Centering Y axis Position */
//	INT32				cent_mark1_up_x_pos;			/* Mark 1 에 대한 UP CAM Centering X axis Position */
//	INT32				cent_mark1_up_y_pos;			/* Mark 1 에 대한 UP CAM Centering Y axis Position */
//	INT32				cent_mark2_up_x_pos;			/* Mark 2 에 대한 UP CAM Centering X axis Position */
//	INT32				cent_mark2_up_y_pos;			/* Mark 2 에 대한 UP CAM Centering Y axis Position */
//	INT32				offset_mark1_x;
//	INT32				offset_mark1_y;
//	INT32				offset_mark2_x;
//	INT32				offset_mark2_y;
//	INT32				focus_ph_head_height;
//	INT32				focus_ph_meas_height;
//	INT32				inlock_stage_pos_min_x;			/* STAGE X 축 TABLE의 MIN POSITION	*/
//	INT32				inlock_stage_pos_min_y;			/* STAGE Y 축 TABLE의 MIN POSITION	*/
//	INT32				inlock_stage_pos_max_x;			/* STAGE X 축 TABLE의 MAX POSITION	*/
//	INT32				inlock_stage_pos_max_y;			/* STAGE Y 축 TABLE의 MAX POSITION	*/
//	INT32				inlock_pin_pos_min_x;			/* PIN X 축 TABLE의 MIN POSITION	*/
//	INT32				inlock_pin_pos_max_x;			/* PIN X 축 TABLE의 MAX POSITION	*/
//	INT32				inlock_pin_pos_min_z;			/* PIN Z 축 TABLE의 MIN POSITION	*/
//	INT32				inlock_pin_pos_max_z;			/* PIN Z 축 TABLE의 MAX POSITION	*/
//	INT32				inlock_up_pos_min_z;			/* Optic Z(UP) 축 TABLE의 MIN POSITION	*/
//	INT32				inlock_up_pos_max_z;			/* Optic Z(UP) 축 TABLE의 MAX POSITION	*/
//	INT32				expo_set_rotation;				/* Expose Rotation */
//	INT32				expo_set_scale_x;				/* Expose Scale X [mm] */
//	INT32				expo_set_scale_y;				/* Expose Scale Y [mm] */
//	INT32				expo_set_offset_x;				/* Expose Offset X [mm] */
//	INT32				expo_set_offset_y;				/* Expose Offset Y [mm] */
//	INT32				inlock_bsa_pos_min_z;			/* BSA Z 축 TABLE의 MIN POSITION	*/
//	INT32				inlock_bsa_pos_max_z;			/* BSA Z 축 TABLE의 MAX POSITION	*/
//	INT32				expo_photo1_height;				/* Photohead1 Z Axis [mm] */
//	INT32				expo_photo2_height;				/* Photohead2 Z Axis [mm] */
//	INT32				expo_pos_delay_1;				/* HysterssisType1 Move Positive Delay */
//	INT32				expo_neg_delay_1;				/* HysterssisType1 Move Negative Delay */
//	INT32				expo_ph2_offset_x;				/* ph2 단차 X */
//	INT32				expo_ph2_offset_y;				/* ph2 단차 Y */
//	INT32				illum_ph_x_axis_pos[3];			/* 헤드 1번 조도 센서 측정을 위한 Stage X 축 위치 */
//	INT32				illum_ph_y_axis_pos[3];			/* 헤드 1번 조도 센서 측정을 위한 Stage Y 축 위치 */
//	INT32				inlock_stage_pos_axis_x[4];		/* Bsa 카메라 이동에 관련한 Interlock X Position (Interlock P1 ~ P4)	*/
//	INT32				inlock_stage_pos_axis_y[4];		/* Bsa 카메라 이동에 관련한 Interlock Y Position (Interlock P1 ~ P4)	*/
//	INT32				i32_reserved;
//
//	UINT32				material_thick;					/* Thickness (um) */
//	UINT32				stage_y_acc_dist;				/* Motion controller Y-direction acceleration/deceleration distance. (unit: 100 nm or 0.1 um) */
//	UINT32				velo_align_stage;				/* Align 시 Stage 이동 속도	[mm/s] */
//	UINT32				velo_align_bsa;					/* Align 시 bsa 이동 속도 [mm/s] */
//	UINT32				velo_align_pin_x;				/* Align 시 pin x 이동 속도 [mm/s] */
//	UINT32				velo_align_pin_z;				/* Align 시 pin z 이동 속도 [mm/s] */
//	UINT32				led0_index_ph1;					/* 노광에 사용하는 조도 값에 해당되는 Ph1 인덱스 값 (LED 365) */
//	UINT32				led1_index_ph1;					/* 노광에 사용하는 조도 값에 해당되는 Ph1 인덱스 값 (LED 380) */
//	UINT32				led2_index_ph1;					/* 노광에 사용하는 조도 값에 해당되는 Ph1 인덱스 값 (LED 390) */
//	UINT32				led3_index_ph1;					/* 노광에 사용하는 조도 값에 해당되는 Ph1 인덱스 값 (LED 405) */
//	UINT32				led0_index_ph2;					/* 노광에 사용하는 조도 값에 해당되는 Ph2 인덱스 값 (LED 365) */
//	UINT32				led1_index_ph2;					/* 노광에 사용하는 조도 값에 해당되는 Ph2 인덱스 값 (LED 380) */
//	UINT32				led2_index_ph2;					/* 노광에 사용하는 조도 값에 해당되는 Ph2 인덱스 값 (LED 390) */
//	UINT32				led3_index_ph2;					/* 노광에 사용하는 조도 값에 해당되는 Ph2 인덱스 값 (LED 405) */
//	UINT32				up_cam_light_value;				/* Photo Head 조명 값 */
//	UINT32				ring_light_value;				/* BSA Ring 조명 값	*/
//	UINT32				coaxial_light_value;			/* BSA Coaxial 조명 값 */
//	UINT32				u32_reserved;
//
//	DOUBLE				led0_power_ph;					/* 노광에 사용하는 조도 값 (LED 365) */
// 	DOUBLE				led1_power_ph;					/* 노광에 사용하는 조도 값 (LED 380) */
// 	DOUBLE				led2_power_ph;					/* 노광에 사용하는 조도 값 (LED 390) */
// 	DOUBLE				led3_power_ph;					/* 노광에 사용하는 조도 값 (LED 405) */
//	DOUBLE				focus_ld_sensor_p1;
//	DOUBLE				focus_ld_sensor_p2;
//	DOUBLE				focus_ld_sensor_p3;
//	DOUBLE				focus_ld_sensor_avg;
//	DOUBLE				inlock_stage_max_speed;			/* STAGE MAX SPEED (mm/s)	*/
//	DOUBLE				inlock_stage_min_speed;			/* STAGE MIN SPEED (mm/s)	*/
//	DOUBLE				inlock_bsa_max_speed;			/* BSA Z 축 MAX SPEED (mm/s)	*/
//	DOUBLE				inlock_bsa_min_speed;			/* BSA Z 축 MIN SPEED (mm/s)	*/
//	DOUBLE				inlock_pin_max_speed;			/* PIN Z 축	MAX SPEED (mm/s)	*/
//	DOUBLE				inlock_pin_min_speed;			/* PIN Z 축	MIN SPEED (mm/s)	*/
//
//	/*
//	 desc : 데이터 유효성 검증
//	 parm : None
//	 retn : TRUE or FALSE
//	*/
//	BOOL IsValid()
//	{
//		return expo_start_x_pos > 0 && expo_start_y_pos > 0 &&
//			gerber_0_x_pos > 0 && gerber_0_y_pos > 0 && load_x_pos > 0 && load_y_pos > 0 &&
//			unload_x_pos > 0 && unload_y_pos > 0 && orgin_x_pos > 0 && orgin_y_pos > 0 &&
//			/* 아래는 상당히 중요. 노광 시작 위치 값보다. Y Acceleration Distance 값이 크면 안됨 */
//			expo_start_y_pos >= INT32(stage_y_acc_dist) && velo_align_stage > 0;
//			
//	}
//
//	/*
//	 desc : 정보 유효성 검사 (조도 측정을 이 값으로 동작해도 되는지 여부)
//	 parm : None
//	 retn : TRUE or FALSE
//	*/
//	BOOL IsValidIllum()
//	{
//		return (illum_ph_x_axis_pos[1] > 0.0f && illum_ph_y_axis_pos[1] > 0.0f &&
//				illum_ph_x_axis_pos[2] > 0.0f && illum_ph_y_axis_pos[2] > 0.0f);
//	}
//
//}	STG_CTPI, *LPG_CTPI;
//
///* Teaching Position 의 동작 정보 */
//typedef struct __st_config_exposure_parameter_info__
//{
//	CHAR				expo_matrix_gerber[MAX_GERBER_NAME];		/* Gerber Name (Path included) */
//
//	UINT8				serial_flip_h: 1;					/* Serial String Flip ? Up/Down */
//	UINT8				serial_flip_v: 1;					/* Serial String Flip ? Left/Right */
//	UINT8				scale_flip_h: 1;					/* Scale String Flip ? Up/Down */
//	UINT8				scale_flip_v: 1;					/* Scale String Flip ? Left/Right */
//	UINT8				u8_bits_reserved: 4;
//	UINT8				expo_align;							/* Wafer Loading Position Y */
//	UINT8				expo_rectangle_lock;				/* Wafer UnLoading Position X */
//	UINT8				u8_reserved[5];
//
//	INT32				expo_set_rotation;					/* Expose Start X axis Position */
//	INT32				expo_set_scale_x;					/* Expose Start Y axis Position */
//	INT32				expo_set_scale_y;					/* 거버 (0,0) 의 위치 X AXIS */
//	INT32				expo_set_offset_x;					/* 거버 (0,0) 의 위치 Y AXIS */
//	INT32				expo_set_offset_y;					/* Wafer Loading Position X */
//	INT32				expo_duty_cycle;
//	INT32				expo_photo_height;					/* Wafer UnLoading Position Y */
//	INT32				expo_neg_offset_1;					/* Stage Orgin Position X */
//	INT32				expo_pos_delay_1;					/* Stage Orgin Position Y */
//	INT32				expo_neg_delay_1;					/* bsa camera moving postion x */
//	INT32				expo_neg_offset_2;					/* Stage Orgin Position X */
//	INT32				expo_pos_delay_2;					/* Stage Orgin Position Y */
//	INT32				expo_neg_delay_2;					/* bsa camera moving postion x */
//	INT32				expo_start_pos_x;					/* bsa camera moving postion x */
//	INT32				expo_start_pos_y;					/* bsa camera moving postion x */
//	INT32				expo_matrix_photo1_height;
//	INT32				expo_matrix_photo2_height;
//	INT32				expo_ph_offset_x_2;
//	INT32				expo_ph_offset_y_2;
//	INT32				expo_ph_offset_x_step;
//	INT32				expo_ph_offset_y_step;
//	INT32				expo_neg_offset_step;
//	INT32				expo_pos_delay_step;
//	INT32				expo_neg_delay_step;
//	INT32				expo_matrix_photo_height_step;		/* 노광시 PhotoHead Step 높이 */
//	INT32				expo_matrix_energy;					/* 노광시 Energy [mj] */
//	INT32				expo_matrix_energy_step;			/* 노광시 Energy Step [mj] */
//	INT32				expo_matrix_start_pos_x;			/* matrix 노광 Start Position x */
//	INT32				expo_matrix_start_pos_y;			/* matrix 노광 Start Position y */
//	INT32				i32_reserved;
//
//}	STG_CEPI,	*LPG_CEPI;
//
///* FOUP (Lot)에 노광될 웨이퍼와 거버 설정 정보 */
//typedef struct __st_exposure_foup_wafer_gerber__
//{
//	UINT8				get;				/* Wafer 가져갈 때, 정상 (1) or 반전 (2) */
//	UINT8				put;				/* Wafer 가져올 때, 정상 (1) or 반전 (2) */
//	UINT8				gid;				/* Gerber Index (1 ~ 25) */
//	UINT8				mark1;				/* Mark 설정 1 */
//	UINT8				mark2;				/* Mark 설정 1 */
//	UINT8				u8_reserved[3];
//
//	UINT32				material_thickness; /* Material Thickness (unit: 100 nm or 0.1 um) */
//	UINT32				u32_reserved;
//
//	DOUBLE				energy;				/* 최종 노광될 광량 (단위: mj) */
//	DOUBLE				speed;				/* 최종 노광될 속도 (단위: mm/sec) */
//
//	/*
//	 desc : 초기화
//	 parm : None
//	 retn : None
//	*/
//	VOID Reset()
//	{
//		get = put = gid = mark1 = mark2 = 0x00;
//		material_thickness	= 0;
//		energy = speed = 0.0f;
//	}
//
//}	STG_EFWG,	*LPG_EFWG;
//
///* Exposure Paramter 정보 in Teaching */
//typedef struct __st_config_teaching_exposure_info__
//{
//	UINT32				gerb_expo_cnt, gerb_meas_cnt, teach_expo_cnt;
//
//	TCHAR				gerber_focus[MAX_GERBER_NAME];
//	TCHAR				gerber_offset[MAX_GERBER_NAME];
//	TCHAR				gerber_expo[5][MAX_GERBER_NAME];
//	TCHAR				gerber_meas[10][MAX_GERBER_NAME];
//
//	STG_CEPI			teach_pos;
//	STG_EFWG			teach_expo[2];
//
//	/*
//	 desc : 초기화
//	 parm : None
//	 retn : None
//	*/
//	VOID Reset()
//	{
//		UINT8 i;
//		gerb_expo_cnt	= 0x05;
//		gerb_meas_cnt	= 0x0a;
//		teach_expo_cnt	= 0x02;
//		wmemset(gerber_focus, 0x00, MAX_GERBER_NAME);
//		wmemset(gerber_offset, 0x00, MAX_GERBER_NAME);
//		for (i=0x00; i<gerb_expo_cnt; i++)	wmemset(gerber_expo[i], 0x00, MAX_GERBER_NAME);
//		for (i=0x00; i<gerb_meas_cnt; i++)	wmemset(gerber_meas[i], 0x00, MAX_GERBER_NAME);
//		for (i=0x00; i<teach_expo_cnt; i++)	teach_expo[i].Reset();
//	}
//
//}	STG_CTEI,	*LPG_CTEI;
//
//
//#pragma pack (pop)	/* 8 bytes */
//
///* 반드시 1 bytes 정렬 (Shared Memory 때문에 1 bytes 정렬 불가피) */
//#pragma pack (push, 1)
//
//
//#pragma pack (pop)	// 1 bytes