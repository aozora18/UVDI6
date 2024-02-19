
/*
 desc : Enum
*/

#pragma once

/* ---------------------------------------------------------------------------- */
/* PLC Address 정의 (장비 마다 주소 값이 다를 수 있음. 16 진수로 구성 : ____ ) */
/* 즉, 앞의 3 자리는 WORD 주소 (0000 ~ FFFF) / 마지막 1 자리는 BIT 주소 (0 ~ F) */
/* ---------------------------------------------------------------------------- */
typedef enum class __en_plc_input_output_address__ : UINT16
{
	/* PLC Address - Read for Bits */
	en_main_mc_power_on_off							= 0x44C0,
	en_ems1_front_push								= 0x44C1,
	en_ems2_3_right_inside_push						= 0x44C2,
	en_safety_line_power_cp24s1_cp24s2				= 0x44C3,
	en_chiller_power_elcb101						= 0x44C4,
	en_temperature_controller_power_elcb102			= 0x44C5,
	en_vacuum_controller_power_elcb103				= 0x44C6,
	en_linear_x_axis_power_cp101					= 0x44C7,
	en_linear_y_axis_power_cp102					= 0x44C8,
	en_linear_ca1_ca2_axis_power_cp103				= 0x44C9,
	en_all_sd2s_control_power_cp123					= 0x44CA,
	en_mc2_power_cp124								= 0x44CB,
	en_photohead_power_line_cp104_cp113				= 0x44CC,
	en_trigger_board_power_line_cp114_cp115			= 0x44CD,
	en_ups_power_cp116								= 0x44CE,
	en_stepping_motor_drive_power_cp125				= 0x44CF,

	en_area_camera_power_cp126						= 0x44D0,
	en_bottom_left_side_cover_1						= 0x44D1,
	en_bottom_left_side_cover_2						= 0x44D2,
	en_bottom_left_side_cover_3						= 0x44D3,
	en_bottom_front_cover							= 0x44D4,
	en_bottom_right_side_cover_1					= 0x44D5,
	en_bottom_right_side_cover_2					= 0x44D6,
	en_bottom_right_side_cover_3					= 0x44D7,
	en_bottom_rear_side_cover						= 0x44D8,
	en_top_left_side_cover							= 0x44D9,
	en_top_right_side_cover							= 0x44DA,
	en_top_rear_side_cover							= 0x44DB,
	en_photohead_water								= 0x44DC,
	en_utility_box_water							= 0x44DD,
	en_chiller_alarm								= 0x44DE,
	en_upper_fan_123_alarm_only_4th					= 0x44DF,

	en_vacuum_controller_emo_push					= 0x44E0,
	en_vacuum_controller_fan_power					= 0x44E1,
	en_vacuum_controller_cc_link_status				= 0x44E2,
	en_vacuum_controller_inverter_alarm				= 0x44E3,
	en_camera1_z_axis_alarm							= 0x44E4,
	en_camera2_z_axis_alarm							= 0x44E5,
	en_temp_controller_box_over_temp_1				= 0x44E6,
	en_temp_controller_box_over_temp_2				= 0x44E7,
	en_temp_controller_compressure_over_current		= 0x44E8,
	en_temp_controller_compressure_over_pressure	= 0x44E9,
	en_temp_controller_condenser_motor_over_current	= 0x44EA,
	en_temp_controller_fan_alarm					= 0x44EB,
	en_temp_controller_power_reverse_phase			= 0x44EC,
	en_camera1_z_axis_home_request_alarm			= 0x44ED,
	en_camera2_z_axis_home_request_alarm			= 0x44EE,
	en_shutter_vacuum_timeout_alarm					= 0x44EF,
	en_auto__maint_mode								= 0x44F0,
	en_start_button									= 0x44F1,
	en_shutter_status								= 0x44F2,
	en_glass_detecting_sensor						= 0x44F3,
	en_vacuum_status								= 0x44F4,
	en_chiller_status								= 0x44F5,
	en_temperature_status							= 0x44F6,
	en_vacuum_controller_running					= 0x44F7,
	en_vacuum_controller_mode						= 0x44F8,
	en_temp_controller_running						= 0x44F9,
	en_shutter_open_close_waiting					= 0x44FA,
	en_vacuum_vac_atm_waiting						= 0x44FB,
	en_signal_tower_green_lamp						= 0x44FC,
	en_signal_tower_yellow_lamp_flashing			= 0x44FD,
	en_signal_tower_red_lamp_flashing				= 0x44FE,
	en_signal_tower_buzzer_1						= 0x44FF,

	/* PLC Address - Read for Words */
	en_internal_monitoring_temp						= 0x4500,
	en_vacuum_controller_current_pressure			= 0x4520,
	en_vacuum_controller_current_frequnency			= 0x4530,
	en_temp_controller_current_temp					= 0x4540,
	en_temp_controller_current_setting_temp			= 0x4550,

	/* PLC Address - Read for Bits */
	en_camera1_z_axis_status						= 0x4560,
	en_camera1_z_axis_p_limit_position				= 0x4561,
	en_camera1_z_axis_n_limit_psition				= 0x4562,
	en_camera1_z_axis_position_set					= 0x4563,
	en_camera1_z_axis_homing_request				= 0x4564,

	en_camera2_z_axis_status						= 0x4568,
	en_camera2_z_axis_p_limit_position				= 0x4569,
	en_camera2_z_axis_n_limit_psition				= 0x456A,
	en_camera2_z_axis_position_set					= 0x456B,
	en_camera2_z_axis_homing_request				= 0x456C,

	/* PLC Address - Read for Words */
	en_camera1_z_axis_currnt_position_display		= 0x4570,
	en_camera1_z_axis_alarm_code					= 0x4590,
	en_camera2_z_axis_currnt_position_display		= 0x45A0,
	en_camera2_z_axis_alarm_code					= 0x45C0,

	en_internal_monitoring_temp_2_only_4th			= 0x4600,
	en_internal_monitoring_temp_3_only_4th			= 0x4620,
	en_internal_monitoring_temp_4_only_4th			= 0x4640,

	/* PLC Address - Write for Bits */
	en_plc_enable									= 0x4B00,
	en_safety_reset_cmd								= 0x4B01,
	en_start_lamp									= 0x4B02,
	en_vacuum_on_off								= 0x4B03,
	en_shutter_open_close							= 0x4B04,
	en_tower_green_lamp_cmd							= 0x4B05,
	en_tower_yellow_lamp_flashing_cmd				= 0x4B06,
	en_tower_red_lamp_flashing_cmd					= 0x4B07,
	en_tower_buzzer_1_cmd							= 0x4B08,
	en_chiller_run_cmd								= 0x4B09,
	en_trigger_board_power_reset					= 0x4B0A,
	en_vacuum_controller_run_cmd					= 0x4B0B,
	en_temp_controller_run_cmd						= 0x4B0C,

	en_upper_exhaust_left_fan_power					= 0x4B10,
	en_upper_exhaust_center_fan_power				= 0x4B11,
	en_upper_exhaust_right_fan_power				= 0x4B12,

	/* PLC Address - Write for Words */
	en_vacuum_controller_frequnency_setting			= 0x4B20,
	en_temp_controller_temp_setting					= 0x4B30,
	en_pc_live_check								= 0x4B40,

	/* PLC Address - Write for Bits */
	en_camera1_z_axis_enable_disable				= 0x4BA0,
	en_camera1_z_axis_reset							= 0x4BA1,
	en_camera1_z_axis_stop							= 0x4BA2,

	en_camera1_z_axis_jog_plus_move					= 0x4BA4,
	en_camera1_z_axis_jog_minus_move				= 0x4BA5,
	en_camera1_z_axis_home_position					= 0x4BA6,
	en_camera1_z_axis_position						= 0x4BA7,
	en_camera2_z_axis_enable_disable				= 0x4BA8,
	en_camera2_z_axis_reset							= 0x4BA9,
	en_camera2_z_axis_stop							= 0x4BAA,

	en_camera2_z_axis_jog_plus_move					= 0x4BAC,
	en_camera2_z_axis_jog_minus_move				= 0x4BAD,
	en_camera2_z_axis_home_position					= 0x4BAE,
	en_camera2_z_axis_position						= 0x4BAF,

	/* PLC Address - Write for DWords */
	en_camera1_z_axis_jog_speed_setting				= 0x4BB0,
	en_camera1_z_axis_position_speed_setting		= 0x4BD0,
	en_camera1_z_axis_position_setting				= 0x4BF0,
	en_camera2_z_axis_jog_speed_setting				= 0x4C10,
	en_camera2_z_axis_position_speed_setting		= 0x4C30,
	en_camera2_z_axis_position_setting				= 0x4C50,

}	ENG_PIOA;

/* --------------------------------------------------------------------------------------------- */
/*                                       전역(Global) 구조체                                     */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 1)

/* Read - PLC Memory Read & Write */
typedef struct __st_plc_memory_read_write__
{
	/*------------------------------------------------------------------------------- */
	/*                                  Read Area                                     */
	/*------------------------------------------------------------------------------- */

	/* D1100.0 ~ D1101.F : Critical Alarm 1 */
	UINT16				r_main_mc_power_onoff:1;							/* 1:ON,	0:OFF	*/
	UINT16				r_ems1_left_side_push:1;							/* 1:PULL,	0:PUSH	*/
	UINT16				r_ems2_right_side_push:1;							/* 1:PULL,	0:PUSH	*/
	UINT16				r_safety_line_power:1;								/* 1:PULL,	0:PUSH	*/
	UINT16				r_chiller_power:1;									/* 1:ON,	0:OFF	*/
	UINT16				r_temperature_controller_power:1;					/* 1:ON,	0:OFF	*/
	UINT16				r_vacuum_controller_power:1;						/* 1:ON,	0:OFF	*/
	UINT16				r_linear_x_axis_power:1;							/* 1:ON,	0:OFF	*/
	UINT16				r_linear_y_axis_power:1;							/* 1:ON,	0:OFF	*/
	UINT16				r_linear_ca1_ca2_axis_power:1;						/* 1:ON,	0:OFF	*/
	UINT16				r_all_sd2s_control_power:1;							/* 1:ON,	0:OFF	*/
	UINT16				r_mc2_power:1;										/* 1:ON,	0:OFF	*/
	UINT16				r_photohead_power_line:1;							/* 1:ON,	0:OFF	*/
	UINT16				r_trigger_board_power_line:1;						/* 1:ON,	0:OFF	*/
	UINT16				r_ups_power:1;										/* 1:ON,	0:OFF	*/
	UINT16				r_stepping_motor_drive_power:1;						/* 1:ON,	0:OFF	*/
	UINT16				r_area_camera_power:1;								/* 1:ON,	0:OFF	*/
	UINT16				r_bottom_left_side_cover_1:1;						/* 1:CLOSE,	0:OPEN	*/
	UINT16				r_bottom_left_side_cover_2:1;						/* 1:CLOSE,	0:OPEN	*/
	UINT16				r_bottom_left_side_cover_3:1;						/* 1:CLOSE,	0:OPEN	*/
	UINT16				r_bottom_front_cover:1;								/* 1:CLOSE,	0:OPEN	*/
	UINT16				r_bottom_right_side_cover_1:1;						/* 1:CLOSE,	0:OPEN	*/
	UINT16				r_bottom_right_side_cover_2:1;						/* 1:CLOSE,	0:OPEN	*/
	UINT16				r_bottom_right_side_cover_3:1;						/* 1:CLOSE,	0:OPEN	*/
	UINT16				r_bottom_rear_side_cover:1;							/* 1:CLOSE,	0:OPEN	*/
	UINT16				r_top_left_side_cover:1;							/* 1:CLOSE,	0:OPEN	*/
	UINT16				r_top_right_side_cover:1;							/* 1:CLOSE,	0:OPEN	*/
	UINT16				r_top_rear_side_cover:1;							/* 1:CLOSE,	0:OPEN	*/
	UINT16				r_photohead_water_leak:1;							/* 1:NORMAL,0:LEAK	*/
	UINT16				r_utility_box_water_leak:1;							/* 1:NORMAL,0:LEAK	*/
	UINT16				r_chiller_alarm:1;									/* 1:NORMAL,0:ALARM	*/
	UINT16				r_top_fan_alarm:1;									/* 1:NORMAL,0:ALARM	*/

	/* D1102.0 ~ D1102.F : Critical Alarm 2 */
	UINT16				r_vacuum_controller_emo_push:1;						/* 1:PULLED,0:PUSHED*/
	UINT16				r_vacuum_controller_fan_power:1;					/* 1:ON,	0:OFF	*/
	UINT16				r_vacuum_controller_cc_link_status:1;				/* 1:RUN,	0:ALARM	*/
	UINT16				r_vacuum_controller_inverter_alarm:1;				/* 1:NORMAL,0:ALARM	*/
	UINT16				r_camera1_z_axis_alarm:1;							/* 1:NORMAL,0:ALARM	*/
	UINT16				r_camera2_z_axis_alarm:1;							/* 1:NORMAL,0:ALARM	*/
	UINT16				r_temp_controller_box_over_temp_1:1;				/* 1:NORMAL,0:ALARM	*/
	UINT16				r_temp_controller_box_over_temp_2:1;				/* 1:NORMAL,0:ALARM	*/
	UINT16				r_temp_controller_compressure_over_current:1;		/* 1:NORMAL,0:ALARM	*/
	UINT16				r_temp_controller_compressure_over_pressure:1;		/* 1:NORMAL,0:ALARM	*/
	UINT16				r_temp_controller_condenser_motor_over_current:1;	/* 1:NORMAL,0:ALARM	*/
	UINT16				r_temp_controller_fan_alarm:1;						/* 1:NORMAL,0:ALARM	*/
	UINT16				r_temp_controller_power_reverse_phase:1;			/* 1:NORMAL,0:ALARM	*/
	UINT16				r_camera1_z_axis_home_request_alarm:1;				/* 1:NORMAL,0:ALARM	*/
	UINT16				r_camera2_z_axis_home_request_alarm:1;				/* 1:NORMAL,0:ALARM	*/
	UINT16				r_shutter_vacuum_action_timeout_alarm:1;			/* 1:NORMAL,0:ALARM	*/

	/* D1103.0 ~ D1103.F : Normal Signal */
	UINT16				r_auto_maint_mode:1;								/* 1:AUTO,  0:MAINT	*/
	UINT16				r_start_button:1;									/* 1:START, 0:NONE	*/
	UINT16				r_shutter_status:1;									/* 1:OPEN,  0:CLOSE	*/
	UINT16				r_glass_detecting_sensor:1;							/* 1:DETECT,0:NONE	*/
	UINT16				r_vacuum_status:1;									/* 1:VAC,   0:ATM	*/
	UINT16				r_chiller_controller_running:1;						/* 1:RUN,   0:STOP	*/
	UINT16				r_temperature_controller_running:1;					/* 1:RUN,   0:STOP	*/
	UINT16				r_vacuum_controller_running:1;						/* 1:RUN,   0:STOP	*/
	UINT16				r_vacuum_controller_mode:1;							/* 1:REMOTE,0:MANUAL*/
	UINT16				r_temp_controller_running:1;						/* 1:RUN,	0:STOP	*/
	UINT16				r_shutter_open_close_waiting:1;						/* 1:WAITING0:NORMAL*/
	UINT16				r_vacuum_vac_atm_waiting:1;							/* 1:WAITING0:NORMAL*/
	UINT16				r_signal_tower_green_lamp:1;						/* 1:ON,    0:OFF	*/
	UINT16				r_signal_tower_yellow_lamp_flashing:1;				/* 1:ON,    0:OFF	*/
	UINT16				r_signal_tower_red_lamp_flashing:1;					/* 1:ON,    0:OFF	*/
	UINT16				r_signal_tower_buzzer_1:1;							/* 1:ON,    0:OFF	*/

	/* D1104.0 ~ D1105.F : Analog Temp */
	UINT32				r_internal_monitoring_temp;							/* 데이터 값: -200000 ~ +850000 (온도 값 : -200.000 ~ +850.000. 데이터 1 값 = 단위 0.001 도씨) */
	/* D1106.0 ~ D1106.F : Vacuum Controller Current Pressure */			
	INT16				r_vacuum_controller_current_pressure;				/* 데이터 값: 0 ~ -101 (Display : 0 ~ -101 kPa) */
	/* D1107.0 ~ D1107.F : Vacuum Controller Current Frequency */
	UINT16				r_vacuum_controller_current_frequency;				/* 데이터 값: 0 ~ 6000 (Display : 0.00 ~ 60.00 Hz */

	/* D1108.0 ~ D1109.F : Reserved */
	UINT16				r_temp_controller_current_temp;						/* 데이터 값: 0 ~ 32767 (Display : 0.00 ~ 327.67. Divided by 100) */
	UINT16				r_temp_controller_current_setting_temp;

	/* D1110.0 ~ D1110.F : CAMERA (1 & 2) Z Axis */
	UINT16				r_camera1_z_axis_status:1;							/* 1:BUSY,  0:READY */
	UINT16				r_camera1_z_axis_p_limit_position:1;				/* 1:OFF,   0:ON    */
	UINT16				r_camera1_z_axis_n_limit_position:1;				/* 1:OFF,   0:ON    */
	UINT16				r_camera1_z_axis_position:1;						/* 1:ON,    0:OFF   */
	UINT16				r_camera1_z_axis_homing_request:1;					/* 1:ON,    0:OFF   */
	UINT16				r_spare_21:1;
	UINT16				r_spare_22:1;
	UINT16				r_spare_23:1;
	UINT16				r_camera2_z_axis_status:1;							/* 1:BUSY,  0:READY */
	UINT16				r_camera2_z_axis_p_limit_position:1;				/* 1:OFF,   0:ON    */
	UINT16				r_camera2_z_axis_n_limit_position:1;				/* 1:OFF,   0:ON    */
	UINT16				r_camera2_z_axis_position:1;						/* 1:ON,    0:OFF   */
	UINT16				r_camera2_z_axis_homing_request:1;					/* 1:ON,    0:OFF   */
	UINT16				r_spare_31:1;
	UINT16				r_spare_32:1;
	UINT16				r_spare_33:1;

	/* D1111.0 ~ D1112.F : Camera1 Z Axis */
	INT32				r_camera1_z_axis_position_display;					/* unit: 0.1 um or 100 nm */
	/* D1113.0 ~ D1113.F : Reserved */
	UINT16				r_camera1_z_axis_alarm_code;
	/* D1114.0 ~ D1115.F : Camera1 Z Axis */
	INT32				r_camera2_z_axis_position_display;					/* unit: 0.1 um or 100 nm */
	/* D1116.0 ~ D1116.F : Reserved */
	UINT16				r_camera2_z_axis_alarm_code;
	/* D1117.0 ~ D1119.F : Reserved to Read Area for others */
	UINT16				r_u16_reserved[3];

	/* D1120 ~ D1121 */
	UINT32				r_internal_monitoring_temp_2;						/* 데이터 값 : -200000 ~ +850000 (온도 값 : -200.000 ~ +850.000. 데이터 1 값 = 단위 0.001 도씨) */
	/* D1122 ~ D1123 */
	UINT32				r_internal_monitoring_temp_3;						/* 데이터 값 : -200000 ~ +850000 (온도 값 : -200.000 ~ +850.000. 데이터 1 값 = 단위 0.001 도씨) */
	/* D1124 ~ D1125 */
	UINT32				r_internal_monitoring_temp_4;						/* 데이터 값 : -200000 ~ +850000 (온도 값 : -200.000 ~ +850.000. 데이터 1 값 = 단위 0.001 도씨) */

	/*------------------------------------------------------------------------------- */
	/*                                  Write Area                                    */
	/*------------------------------------------------------------------------------- */

	/* D1200.0 ~ D1200.F : Digital Output Bit */
	UINT16				w_plc_enable:1;										/* PLC 출력 사용허가, 금지 명령어 */
	UINT16				w_safety_reset_cmd:1;								/* 안전 RESET 명령  // 안전 커버 및 EMS가 눌린경우, 장비 원복 후 RESET 누를 경우 사용 */
	UINT16				w_start_lamp:1;										/* START 램프 신호명령으로 사용자에게 노광 진행중을 알림 */
	UINT16				w_vacuum_on_off:1;									/* VACUUM ON, OFF 명령 *인터락 추가 : VACUUM CONTROLLER 가 RUN일경우 ON 가능 */
	UINT16				w_shutter_open_close:1;								/* 셔터 OPEN, CLOSE 명령 */
	UINT16				w_tower_green_lamp_cmd:1;							/* 시그널 타워 그린 램프 점등 신호 명령	*/
	UINT16				w_tower_yellow_lamp_flashing_cmd:1;					/* 시그널 타워 노란 램프 점멸 신호 명령	*/
	UINT16				w_tower_red_lamp_flashing_cmd:1;					/* 시그널 타워 빨간 램프 점멸 신호 명령	*/
	UINT16				w_tower_buzzer_1_cmd:1;								/* 시그널 타워 부저 신호 명령 */
	UINT16				w_chiller_run_cmd:1;								/* 칠러 RUN 신호 */
	UINT16				w_trigger_board_power_reset:1;						/* 트리거 BORAD 전원 리셋 (RESET후 PLC가 일정시간 후 다시 ON 함) */
	UINT16				w_vacuum_controller_run_cmd:1;						/* 공정시 항시 1로 RUN 해야 함. 불필요시 0으로 셋팅 시 끌수 있음, VACUUM CONTROLLER RUN 시 내부 모터가 회전하여 동작함, STOP시 멈춤 */
	UINT16				w_temp_controller_run_cmd:1;						/* 온조기 RUN / STOP 신호 */
	UINT16				w_spare_41:1;										/*  */
	UINT16				w_spare_42:1;										/*  */
	UINT16				w_spare_43:1;										/*  */
	/* D1201.0 ~ D1201.F : Digitial Output Bit */
	UINT16				w_upper_exhaust_left_fan_power:1;					/*  */
	UINT16				w_upper_exhaust_center_fan_power:1;					/*  */
	UINT16				w_upper_exhaust_right_fan_power:1;					/*  */
	UINT16				w_spare_51:1;										/*  */
	UINT16				w_spare_52:1;										/*  */
	UINT16				w_spare_53:1;										/*  */
	UINT16				w_spare_54:1;										/*  */
	UINT16				w_spare_55:1;										/*  */
	UINT16				w_spare_56:1;										/*  */
	UINT16				w_spare_57:1;										/*  */
	UINT16				w_spare_58:1;										/*  */
	UINT16				w_spare_59:1;										/*  */
	UINT16				w_spare_5a:1;										/*  */
	UINT16				w_spare_5b:1;										/*  */
	UINT16				w_spare_5c:1;										/*  */
	UINT16				w_spare_5d:1;										/*  */
	/* D1202 ~ D1202 */
	UINT16				w_vacuum_controller_frequency_setting;				/* 데이터 값 : 1000 ~ 6000 (주파수 값 : 10.00Hz ~ 60.00 Hz, 데이터 1 값 = 단위 0.01 Hz) */
	/* D1203 ~ D1203 */
	UINT16				w_temp_controller_temp_setting;						/* 데이터 값 : 1500 ~ 2500 (온도 값 : 15.00 ~ 25.00도씨. 데이터 1 값 = 단위 0.01 도씨) */
	/* D1204 ~ D1204 */
	UINT16				w_pc_live_check;									/* 0 ~ 65536. 500ms안에 PLC에게 0 ~ 65536 중 다른 값으로 전송해줘야 함 */

	/* Spare. D1205.0 ~ D1209.F */
	UINT16				w_write_spare[9];

	/* Camera 1/2 Z Axis (SW) Bit (Write) */
	/* D1210.0 ~ D1210.F */
	UINT16				w_camera1_z_axis_enable_disable:1;					/* 사용, 비사용 명령 */
	UINT16				w_camera1_z_axis_reset:1;							/* 에러 클리어 리셋	*/
	UINT16				w_camera1_z_axis_stop:1;							/* Z축 이동중 정지 */
	UINT16				w_spare_81:1;										/* */
	UINT16				w_camera1_z_axis_jog_posi_move:1;					/* JOG 정방향 */
	UINT16				w_camera1_z_axis_jog_nega_move:1;					/* JOG 역방향 */
	UINT16				w_camera1_z_axis_home_position:1;					/* 원점 복귀 요청 */
	UINT16				w_camera1_z_axis_position:1;						/* 위치이동 명령] */
	UINT16				w_camera2_z_axis_enable_disable:1;					/* 사용, 비사용 명령 */
	UINT16				w_camera2_z_axis_reset:1;							/* 에러 클리어 리셋 */
	UINT16				w_camera2_z_axis_stop:1;							/* Z축 이동중 정지 */
	UINT16				w_spare_91:1;										/* */
	UINT16				w_camera2_z_axis_jog_posi_move:1;					/* JOG 정방향 */
	UINT16				w_camera2_z_axis_jog_nega_move:1;					/* JOG 역방향 */
	UINT16				w_camera2_z_axis_home_position:1;					/* 원점 복귀 요청 */
	UINT16				w_camera2_z_axis_position:1;						/* 위치이동 명령 */									

	/* D1211.0 ~ D1216.F Camera 1 Axis (PC -> PLC) */
	UINT32				w_camera1_z_axis_jog_speed_setting;					/* 조그 속도 설정값 */
	UINT32				w_camera1_z_axis_position_speed_setting;			/* 포지션 속도 값 */
	UINT32				w_camera1_z_axis_position_setting;					/* 포지션 셋팅 값 */

	/* D1217.0 ~ D1222.F Camera 1 Axis (PC -> PLC) */
	UINT32				w_camera2_z_axis_jog_speed_setting;					/* 조그 속도 설정값 */
	UINT32				w_camera2_z_axis_position_speed_setting;			/* 포지션 속도 값 */
	UINT32				w_camera2_z_axis_position_setting;					/* 포지션 셋팅 값 */

}	STG_PMRW,	*LPG_PMRW;

#pragma pack (pop)	// 1 bytes


#pragma pack (push, 8)

#pragma pack (pop)	// 8 bytes

	
	
