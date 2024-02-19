
/*
 desc : PLC Information for GEN2 i
*/

#pragma once


/* ---------------------------------------------------------------------------- */
/* PLC Address 정의 (장비 마다 주소 값이 다를 수 있음. 16 진수로 구성 : _____)  */
/* 즉, 앞의 3 자리는 WORD 주소 (0000 ~ FFFF) / 마지막 1 자리는 BIT 주소 (0 ~ F) */
/* ---------------------------------------------------------------------------- */
typedef enum class __en_plc_input_output_address__ : UINT16
{
	/* ----------------------------------------------- */
	/*                    Read Area                    */
	/* ----------------------------------------------- */

	/* 치명적인 알람 영역 (0x44C ~ 0x44E) */
	en_spd_fail_alarm								= 0x44C0,
	en_linear_x_axis_power_line_cp100				= 0x44C1,
	en_linear_y_axis_power_line_cp101				= 0x44C2,
	en_linear_z1_z3_axis_smps_power_fail			= 0x44C3,
	en_linear_z1_z3_power_cp103_cp105				= 0x44C4,
	en_optics_1_smps_power_fail						= 0x44C5,
	en_optics_2_smps_power_fail						= 0x44C6,
	en_optics_1_2_power_cp107_cp109					= 0x44C7,
	en_robot_prealigner_power_cp110_cp112			= 0x44C8,
	en_all_sd2s_control_power_cp116					= 0x44C9,
	en_mc2_power_cp117								= 0x44CA,
	en_liftpin_axis_cp119							= 0x44CB,
	en_vision_system_power_cp122					= 0x44CC,
	en_efu_modul_power_cp123						= 0x44CD,
	en_server_pc_1_2_power_cp124_cp125				= 0x44CE,
	en_front_safety_cover_1							= 0x44CF,

	en_front_safety_cover_2							= 0x44D0,
	en_front_safety_cover_3							= 0x44D1,
	en_front_safety_cover_4							= 0x44D2,
	en_rear_saferty_cover_1							= 0x44D3,
	en_rear_saferty_cover_2							= 0x44D4,
	en_rear_saferty_cover_3							= 0x44D5,
	en_rear_saferty_cover_4							= 0x44D6,
	en_rear_saferty_cover_5							= 0x44D7,
	en_utility_box_water_leak_sensor				= 0x44D8,
	en_ph_water_leak_sensor_1						= 0x44D9,
	en_ph_water_leak_sensor_2						= 0x44DA,
	en_water_flow_low_alarm							= 0x44DB,
	en_water_flow_high_alarm						= 0x44DC,
	en_water_pressure_low_alarm						= 0x44DD,
	en_water_pressure_high_alarm					= 0x44DE,
	en_air_pressure_low_alarm						= 0x44DF,

	en_halogen_ring_light_burn_out_alarm			= 0x44E0,
	en_halogen_coaxial_light_burn_out_alarm			= 0x44E1,
	en_lift_pin_x_axis_drive_alarm					= 0x44E2,
	en_lift_pin_x_axis_operation_alarm				= 0x44E3,
	en_lift_pin_x_axis_home_request_alarm			= 0x44E4,
	en_lift_pin_z_axis_drive_alarm					= 0x44E5,
	en_lift_pin_z_axis_operation_alarm				= 0x44E6,
	en_lift_pin_z_axis_home_request_alarm			= 0x44E7,
	en_bsa_z_axis_operation_alarm					= 0x44E8,
	en_bsa_z_axis_home_request_alarm				= 0x44E9,
	en_led_lamp_controller_power_cp127				= 0x44EA,
	en_lift_pin_x_not_move_alarm					= 0x44EB,
	en_lift_pin_z_not_move_alarm					= 0x44EC,
	en_lift_pin_x_position_count_alarm				= 0x44ED,
	en_lift_pin_z_position_count_alarm				= 0x44EE,
	en_z1_z3_air_pressure_low_alarm					= 0x44EF,

	en_spare_word_44F0								= 0x44F0,

	/* 경고 알람 영역 (0x450 ~ 0x451) */

	en_pc_rack_upper_out_fan_alarm_1				= 0x4500,
	en_pc_rack_upper_out_fan_alarm_2				= 0x4501,
	en_pc_rack_bottom_out_fan_alarm_1				= 0x4502,
	en_pc_rack_bottom_out_fan_alarm_2				= 0x4503,
	en_power_box_in_fan_alarm_1						= 0x4504,
	en_power_box_in_fan_alarm_2						= 0x4505,
	en_power_box_in_fan_alarm_3						= 0x4506,
	en_power_box_in_fan_alarm_4						= 0x4507,
	en_power_box_out_fan_alarm_1					= 0x4508,
	en_power_box_out_fan_alarm_2					= 0x4509,
	en_power_box_out_fan_alarm_3					= 0x450A,
	en_power_box_out_fan_alarm_4					= 0x450B,
	en_spare_bit_450C								= 0x450C,
	en_spare_bit_450D								= 0x450D,
	en_spare_bit_450E								= 0x450E,
	en_spare_bit_450F								= 0x450F,

	en_spare_word_4510								= 0x4510,

	/* 일반적인 데이터 영역 (0x452 ~ 0x45F) */

	en_auto_maint_mode								= 0x4520,
	en_wafer_chuck_vacuum_status					= 0x4521,
	en_wafer_chuck_atm_status						= 0x4522,
	en_robot_vacuum_status							= 0x4523,
	en_robot_atm_status								= 0x4524,
	en_aligner_vacuum_status						= 0x4525,
	en_aligner_atm_status							= 0x4526,
	en_safety_reset_possible_status					= 0x4527,
	en_spare_bit_4528								= 0x4528,
	en_spare_bit_4529								= 0x4529,
	en_stage_wafer_detact							= 0x452A,
	en_buffer_wafer_in_detact						= 0x452B,
	en_buffer_wafer_out_detact						= 0x452C,
	en_robot_output_signal_1_ex_alarm				= 0x452D,
	en_robot_output_signal_2						= 0x452E,
	en_robot_output_signal_3						= 0x452F,

	en_robot_output_signal_4						= 0x4530,
	en_robot_output_signal_5						= 0x4531,
	en_robot_output_signal_6						= 0x4532,
	en_robot_output_signal_7						= 0x4533,
	en_robot_output_signal_8						= 0x4534,
	en_spare_bit_4535								= 0x4535,
	en_signal_tower_green_lamp						= 0x4536,
	en_signal_tower_yellow_lamp_flashing			= 0x4537,
	en_signal_tower_red_lamp_flashing				= 0x4538,
	en_signal_tower_buzzer_1						= 0x4539,
	en_xy_stage_move_prohibit_status				= 0x453A,
	en_spare_bit_453B								= 0x453B,
	en_spare_bit_453C								= 0x453C,
	en_spare_bit_453D								= 0x453D,
	en_spare_bit_453E								= 0x453E,
	en_spare_bit_453F								= 0x453F,

	en_spare_word_4540								= 0x4540,

	/* TEL Interface Input (Track -> Exposure) */

	en_trk_ink_track_inline							= 0x4550,
	en_trk_rdy_track_ready							= 0x4551,
	en_trk_snd_track_send							= 0x4552,
	en_trk_lend_track_end_of_lot					= 0x4553,
	en_trk_reserve_1								= 0x4554,
	en_trk_reserve_2								= 0x4555,
	en_trk_reserve_3								= 0x4556,
	en_trk_reserve_4								= 0x4557,
	en_trk_reserve_5								= 0x4558,
	en_spare_bit_4559								= 0x4559,
	en_spare_bit_455A								= 0x455A,
	en_spare_bit_455B								= 0x455B,
	en_spare_bit_455C								= 0x455C,
	en_spare_bit_455D								= 0x455D,
	en_spare_bit_455E								= 0x455E,
	en_spare_bit_455F								= 0x455F,

	/* Analog Input Area */
	en_differential_pressure						= 0x4560,
	en_monitoring_temperature						= 0x4570,
	en_z1_z3_axis_cooling_air_pressure				= 0x4580,
	en_z1_z3_axis_cylinder_air_pressure				= 0x4590,
	en_power_box_temperature						= 0x45A0,
	en_pc_rack_upper_monitoring_temperature			= 0x45B0,
	en_pc_rack_bottom_monitoring_temperature		= 0x45C0,
	en_spare_word_45D0								= 0x45D0,
	en_spare_word_45E0								= 0x45E0,
	en_spare_word_45F0								= 0x45F0,

	/* Lift Pin [ X ] Axis (Lamp) Bit */
	en_lift_pin_xi_axis_status						= 0x4600,
	en_lift_pin_xi_axis_p_limit_position			= 0x4601,
	en_lift_pin_xi_axis_n_limit_psition				= 0x4602,
	en_lift_pin_xi_axis_position					= 0x4603,
	en_lift_pin_xi_axis_loading_position			= 0x4604,
	en_lift_pin_xi_axis_safety_position				= 0x4605,
	en_spare_bit_4606								= 0x4606,
	en_spare_bit_4607								= 0x4607,
	en_spare_bit_4608								= 0x4608,
	en_spare_bit_4609								= 0x4609,
	en_spare_bit_460A								= 0x460A,
	en_spare_bit_460B								= 0x460B,
	en_spare_bit_460C								= 0x460C,
	en_spare_bit_460D								= 0x460D,
	en_spare_bit_460E								= 0x460E,
	en_spare_bit_460F								= 0x460F,

	/* Lift Pin [ Z ] Axis (Lamp) Bit */
	en_lift_pin_zi_axis_status						= 0x4610,
	en_lift_pin_zi_axis_p_limit_position			= 0x4611,
	en_lift_pin_zi_axis_n_limit_psition				= 0x4612,
	en_lift_pin_zi_axis_position					= 0x4613,
	en_lift_pin_zi_axis_loading_position			= 0x4614,
	en_lift_pin_zi_axis_safety_position				= 0x4615,
	en_spare_bit_4616								= 0x4616,
	en_spare_bit_4617								= 0x4617,
	en_spare_bit_4618								= 0x4618,
	en_spare_bit_4619								= 0x4619,
	en_spare_bit_461A								= 0x461A,
	en_spare_bit_461B								= 0x461B,
	en_spare_bit_461C								= 0x461C,
	en_spare_bit_461D								= 0x461D,
	en_spare_bit_461E								= 0x461E,
	en_spare_bit_461F								= 0x461F,

	/* BSA Camera [ Z ] Axis (Lamp) Bit */
	en_bsa_zi_axis_status							= 0x4620,
	en_bsa_zi_axis_p_limit_position					= 0x4621,
	en_bsa_zi_axis_n_limit_psition					= 0x4622,
	en_bsa_zi_axis_position							= 0x4623,
	en_spare_bit_4624								= 0x4624,
	en_spare_bit_4625								= 0x4625,
	en_spare_bit_4626								= 0x4626,
	en_spare_bit_4627								= 0x4627,
	en_spare_bit_4628								= 0x4628,
	en_spare_bit_4629								= 0x4629,
	en_spare_bit_462A								= 0x462A,
	en_spare_bit_462B								= 0x462B,
	en_spare_bit_462C								= 0x462C,
	en_spare_bit_462D								= 0x462D,
	en_spare_bit_462E								= 0x462E,
	en_spare_bit_462F								= 0x462F,

	/* Lift Pin [ X ] Axis Word */
	en_lift_pin_xi_axis_currnt_position_display		= 0x4630,	/* DWord */
	en_lift_pin_xi_axis_alarm_code					= 0x4650,
	/* Lift_Pin [ Z ] Axis_Word */
	en_lift_pin_zi_axis_currnt_position_display		= 0x4660,	/* DWord */
	en_lift_pin_zi_axis_alarm_code					= 0x4680,
	/* BSA Camera [ Z ] Axis Word */
	en_bsa_camera_zi_axis_currnt_position_display	= 0x4690,	/* DWord */
	en_bsa_camera_zi_axis_alarm_code				= 0x46B0,

	/* ----------------------------------------------- */
	/*                    Write Area                   */
	/* ----------------------------------------------- */

	/* Digitial Output Bit */
	en_safety_reset_cmd								= 0x47E0,
	en_tower_green_lamp_cmd							= 0x47E1,
	en_tower_yellow_lamp_flashing_cmd				= 0x47E2,
	en_tower_red_lamp_flashing_cmd					= 0x47E3,
	en_tower_buzzer_1_cmd							= 0x47E4,
	en_z1_axis_air_cooling_on_off					= 0x47E5,
	en_z2_axis_air_cooling_on_off					= 0x47E6,
	en_z3_axis_air_cooling_on_off					= 0x47E7,
	en_z1_axis_cylinder_on_off						= 0x47E8,
	en_z2_axis_cylinder_on_off						= 0x47E9,
	en_z3_axis_cylinder_on_off						= 0x47EA,
	en_wafer_chuck_vacuum_on_off					= 0x47EB,
	en_wafer_chuck_exhaust_on_off					= 0x47EC,
	en_robot_vacuumuum_on_off						= 0x47ED,
	en_aligner_vacuum_on_off						= 0x47EE,
	en_photohead_1_power_off_cmd					= 0x47EF,

	en_photohead_2_power_off_cmd					= 0x47F0,
	en_illumination_sensor_pwr_on_off				= 0x47F1,
	en_halogen_ring_light_power_on					= 0x47F2,
	en_halogen_coaxial_light_power_on				= 0x47F3,
	en_spare_bit_47F4								= 0x47F4,
	en_spare_bit_47F5								= 0x47F5,
	en_robot_input_signal_1							= 0x47F6,
	en_robot_input_signal_2							= 0x47F7,
	en_robot_input_signal_3							= 0x47F8,
	en_robot_input_signal_4							= 0x47F9,
	en_robot_input_signal_5							= 0x47FA,
	en_robot_input_signal_6							= 0x47FB,
	en_robot_input_signal_7							= 0x47FC,
	en_robot_input_signal_8							= 0x47FD,
	en_robot_input_signal_9							= 0x47FE,
	en_robot_input_signal_10						= 0x47FF,

	en_xy_stage_move_prohibit						= 0x4800,	/* 1 : ON, 0 : OFF */
	en_xy_stage_move_prohibit_release				= 0x4801,	/* 1 : RELEASE, 0 : OFF */
	en_mc2_power_off_cmd							= 0x4802,
	en_spare_bit_4803								= 0x4803,
	en_spare_bit_4804								= 0x4804,
	en_spare_bit_4805								= 0x4805,
	en_spare_bit_4806								= 0x4806,
	en_spare_bit_4807								= 0x4807,
	en_spare_bit_4808								= 0x4808,
	en_spare_bit_4809								= 0x4809,
	en_spare_bit_480A								= 0x480A,
	en_spare_bit_480B								= 0x480B,
	en_spare_bit_480C								= 0x480C,
	en_spare_bit_480D								= 0x480D,
	en_spare_bit_480E								= 0x480E,
	en_spare_bit_480F								= 0x480F,

	/* TEL Interface Output (Exposre -> Track) */
	en_exp_ink_exposure_inline						= 0x4810,
	en_exp_rej_exposure_reject						= 0x4811,
	en_exp_snd_exposure_send_to_ready				= 0x4812,
	en_exp_rdy_exposure_receive_to_ready			= 0x4813,
	en_exp_reserve_1								= 0x4814,
	en_exp_reserve_2								= 0x4815,
	en_exp_reserve_3								= 0x4816,
	en_exp_reserve_4								= 0x4817,
	en_exp_reserve_5								= 0x4818,
	en_spare_B39									= 0x4819,
	en_spare_B3A									= 0x481A,
	en_spare_B3B									= 0x481B,
	en_spare_B3C									= 0x481C,
	en_spare_B3D									= 0x481D,
	en_spare_B3E									= 0x481E,
	en_spare_B3F									= 0x481F,

	/* Analog Write Area */
	en_halogen_ring_light_power						= 0x4820,
	en_halogen_coaxial_light_power					= 0x4830,

	/* PC Live_Word */
	en_pc_live_check								= 0x4840,
	en_spare_word_4850								= 0x4850,
	en_spare_word_4860								= 0x4860,
	en_spare_word_4870								= 0x4870,
	/* Lift Pin [ X ] Axis (SW) Bit */
	en_lift_pin_xo_axis_enable_disable				= 0x4880,
	en_lift_pin_xo_axis_reset						= 0x4881,
	en_lift_pin_xo_axis_stop						= 0x4882,
	en_spare										= 0x4883,
	en_lift_pin_xo_axis_jog_plus_move				= 0x4884,
	en_lift_pin_xo_axis_jog_minus_move				= 0x4885,
	en_lift_pin_xo_axis_home_position				= 0x4886,
	en_lift_pin_xo_axis_position					= 0x4887,
	en_lift_pin_xo_axis_drive_alarm_reset			= 0x4888,
	en_spare_4889									= 0x4889,
	en_spare_488A									= 0x488A,
	en_spare_488B									= 0x488B,
	en_spare_488C									= 0x488C,
	en_spare_488D									= 0x488D,
	en_spare_488E									= 0x488E,
	en_spare_488F									= 0x488F,
	/* Lift Pin [ Z ] Axis (SW) Bit */
	en_lift_pin_zo_axis_enable_disable				= 0x4890,
	en_lift_pin_zo_axis_reset						= 0x4891,
	en_lift_pin_zo_axis_stop						= 0x4892,
	en_spare_4893									= 0x4893,
	en_lift_pin_zo_axis_jog_plus_move				= 0x4894,
	en_lift_pin_zo_axis_jog_minus_move				= 0x4895,
	en_lift_pin_zo_axis_home_position				= 0x4896,
	en_lift_pin_zo_axis_position					= 0x4897,
	en_lift_pin_zo_axis_dreve_alarm_reset			= 0x4898,
	en_spare_4899									= 0x4899,
	en_spare_489A									= 0x489A,
	en_spare_489B									= 0x489B,
	en_spare_489C									= 0x489C,
	en_spare_489D									= 0x489D,
	en_spare_489E									= 0x489E,
	en_spare_489F									= 0x489F,
	/* BSA Camera [ Z ] Axis (SW) Bit */
	en_bsa_camera_zo_axis_enable_disable			= 0x48A0,
	en_bsa_camera_zo_axis_reset						= 0x48A1,
	en_bsa_camera_zo_axis_stop						= 0x48A2,
	en_spare_48A3									= 0x48A3,
	en_bsa_camera_zo_axis_jog_plus_move				= 0x48A4,
	en_bsa_camera_zo_axis_jog_minus_move			= 0x48A5,
	en_bsa_camera_zo_axis_home_position				= 0x48A6,
	en_bsa_camera_zo_axis_position					= 0x48A7,
	en_spare_48A8									= 0x48A8,
	en_spare_48A9									= 0x48A9,
	en_spare_48AA									= 0x48AA,
	en_spare_48AB									= 0x48AB,
	en_spare_48AC									= 0x48AC,
	en_spare_48AD									= 0x48AD,
	en_spare_48AE									= 0x48AE,
	en_spare_48AF									= 0x48AF,
	/* Lift Pin [ X ] Axis (PC->PLC) Word */
	en_lift_pin_xo_axis_jog_speed_setting			= 0x48B0,	/* DWord */
	en_lift_pin_xo_axis_position_speed_setting		= 0x48D0,	/* DWord */
	en_lift_pin_xo_axis_position_setting			= 0x48F0,	/* DWord */
	/* Lift Pin [ Z ] Axis (PC->PLC) Word */							 
	en_lift_pin_zo_axis_jog_speed_setting			= 0x4910,	/* DWord */
	en_lift_pin_zo_axis_position_speed_setting		= 0x4930,	/* DWord */
	en_lift_pin_zo_axis_position_setting			= 0x4950,	/* DWord */
	/* BSA Camera [ Z ] Axis (PC->PLC) Word */							 
	en_bsa_camera_zo_axis_jog_speed_setting			= 0x4970,	/* DWord */
	en_bsa_camera_zo_axis_position_speed_setting	= 0x4990,	/* DWord */
	en_bsa_camera_zo_axis_position_setting			= 0x49B0,	/* DWord */

}	ENG_PIOA;

/*_PIO_구분_값_*/
typedef enum class __en_pin_input_output_type__ : UINT8
{
	en_scanner	= 0x00,
	en_track	= 0x01,

}	ENG_PIOT;

/* Halogen Light Type */
typedef enum class __en_halogen_light_power_type__
{
	en_ring		= 0x00,
	en_coaxial	= 0x01,

}	ENG_HLPT;

/* Tower Lamp Color Index */
typedef enum class __en_tower_lamp_color_index__ : UINT8
{
	en_green	= 0x01,
	en_yellow	= 0x02,
	en_red		= 0x03,
	en_all		= 0x04,

}	ENG_TLCI;

/* Linked Litho. Exposure -> Track (Reference to PLC Document) */
typedef enum class __en_linked_litho_exposure_track__ : UINT8
{
	en_exp_ink	= 0x01,
	en_exp_rej	= 0x02,
	en_exp_snd	= 0x03,
	en_exp_rdy	= 0x04,

}	ENG_LLET;

/* Lift Pin Axis Type */
typedef enum class __en_lift_pin_axis_type__ : UINT8
{
	en_pin_x_axis	= 0x00,
	en_pin_z_axis	= 0x01,
	en_bsa_z_axis	= 0x02,

}	ENG_LPAT;

/* Lift Pin Axis Control */
typedef enum class __en_lift_pin_axis_control__ : UINT8
{
	en_mode		= 0x01,	/* Enable or Disable */
	en_reset	= 0x02,
	en_stop		= 0x03,

}	ENG_LPAC;

/* Lift Pin Axis Action */
typedef enum class __en_lift_pin_axis_action__ : UINT8
{
	en_jog_move_plus	= 0x01,	/* Jog + Move */
	en_jog_move_minus	= 0x02,	/* Jog - Move */
	en_home_position	= 0x03,	/* Home Position */
	en_move_position	= 0x04,	/* Axis Position */
	en_alarm_reset		= 0x05,	/* Axis Drive Alarm Reset */

}	ENG_LPAA;

#pragma pack (push, 2)

/* Digital Input Bit Alarm */
typedef struct __st_input_digital_bit_alarm__
{
	/* D1100.0 ~ D1100.F */
	UINT8				spd_fail_alarm:1;								/* SPD는 서지보호기를 뜻함, 고장시 교체 및 점검 필요 */
	UINT8				linear_x_axis_power_line_cp100:1;				/* CP 번호는 차단기와 차단기 번호를 뜻함, 해당 차단기의 ON/OFF 유무 */
	UINT8				linear_y_axis_power_line_cp101:1;
	UINT8				linear_z1_z3_axis_smps_power_fail:1;			/* SMPS FAIL = 파워서플라이 모듈에 문제가 있음 */
	UINT8				linear_z1_z3_power_cp103_cp105:1;
	UINT8				optics_1_smps_power_fail:1;
	UINT8				optics_2_smps_power_fail:1;
	UINT8				optics_1_2_power_cp107_cp109:1;
	UINT8				robot_prealigner_power_cp110_cp112:1;
	UINT8				all_sd2s_control_power_cp116:1;					/* SD2S란 모션 드라이브 */
	UINT8				mc2_power_cp117:1;								/* MC2란 모션 마스터 컨트롤러 */
	UINT8				liftpin_axis_cp119:1;
	UINT8				vision_system_power_cp122:1;
	UINT8				efu_modul_power_cp123:1;
	UINT8				server_pc_1_2_power_cp124_cp125:1;
	UINT8				front_safety_cover_1:1;							/* COVER란 장비 커버를 뜻함 */
	/* D1101.0 ~ D1101.F */
	UINT8				front_safety_cover_2:1;
	UINT8				front_safety_cover_3:1;
	UINT8				front_safety_cover_4:1;
	UINT8				rear_saferty_cover_1:1;
	UINT8				rear_saferty_cover_2:1;
	UINT8				rear_saferty_cover_3:1;
	UINT8				rear_saferty_cover_4:1;
	UINT8				rear_saferty_cover_5:1;
	UINT8				utility_box_water_leak_sensor:1;				/* 해당 장소에서 리크센서에 액체가 감지됨 */
	UINT8				ph_water_leak_sensor_1:1;
	UINT8				ph_water_leak_sensor_2:1;
	UINT8				water_flow_low_alarm:1;							/* 물 유량 알람 */
	UINT8				water_flow_high_alarm:1;						/* 물 유량 알람 */
	UINT8				water_pressure_low_alarm:1;						/* 물 압력 알람 */
	UINT8				water_pressure_high_alarm:1;					/* 물 압력 알람 */
	UINT8				air_pressure_low_alarm:1;						/* 메인 공압 알람 */
	/* D1102.0 ~ D1102.F */
	UINT8				halogen_ring_light_burn_out_alarm:1;			/* 할로겐 램프 고장 알람 */
	UINT8				halogen_coaxial_light_burn_out_alarm:1;
	UINT8				lift_pin_x_axis_drive_alarm:1;					/* 리프트핀 X축 드라이브 하드웨어 알람 */
	UINT8				lift_pin_x_axis_operation_alarm:1;				/* 리프트핀 X축 구동 시에 SW알람 */
	UINT8				lift_pin_x_axis_home_request_alarm:1;			/* 리프트핀 X축 원점 복귀 알람 */
	UINT8				lift_pin_z_axis_drive_alarm:1;
	UINT8				lift_pin_z_axis_operation_alarm:1;
	UINT8				lift_pin_z_axis_home_request_alarm:1;
	UINT8				bsa_z_axis_operation_alarm:1;
	UINT8				bsa_z_axis_home_request_alarm:1;
	UINT8				led_lamp_controller_power_cp127:1;
	UINT8				lift_pin_x_not_move_alarm:1;				/* 포지션 인터락 알람 (충돌 위치 명령 시): 해제방법 : RESET */
	UINT8				lift_pin_z_not_move_alarm:1;				/* 포지션 인터락 알람 (충돌 위치 명령 시): 해제방법 : RESET */
	UINT8				lift_pin_x_position_count_alarm:1;			/* 포지션 이동할 시 스테핑모터의 레졸버(EX.엔코더)카운터 값 비교 후 4pulse(2.4um)초과 시 알람 */
	UINT8				lift_pin_z_position_count_alarm:1;			/* 허용가능 오차범위 4pulse(2.4um)이하 */
	UINT8				z1_z3_air_pressure_low_alarm:1;				/* 4 pulse = 3 pluse (센서오차) + 1Pulse (0.6um 분해능으로 나눠떨어지지 않는 위치 경우) */

	UINT16				spare_word_d1113;

}	STG_IDBA,	*LPG_IDBA;

/* Digital Input Bit Warn */
typedef struct __st_input_digital_bit_warn__
{
	/* 1104.0 ~ 1104.F */
	UINT8				pc_rack_upper_out_fan_alarm_1:1;			/* 해당 장소의 FAN 고장알람 */
	UINT8				pc_rack_upper_out_fan_alarm_2:1;
	UINT8				pc_rack_bottom_out_fan_alarm_1:1;
	UINT8				pc_rack_bottom_out_fan_alarm_2:1;
	UINT8				power_box_in_fan_alarm_1:1;
	UINT8				power_box_in_fan_alarm_2:1;
	UINT8				power_box_in_fan_alarm_3:1;
	UINT8				power_box_in_fan_alarm_4:1;
	UINT8				power_box_out_fan_alarm_1:1;
	UINT8				power_box_out_fan_alarm_2:1;
	UINT8				power_box_out_fan_alarm_3:1;
	UINT8				power_box_out_fan_alarm_4:1;
	UINT8				spare_450C:1;
	UINT8				spare_450D:1;
	UINT8				spare_450E:1;
	UINT8				spare_450F:1;
	/* 1105.0 ~ 1105.F */
	UINT16				spare_word_d1105;

}	STG_IDBW,	*LPG_IDBW;

/* Digital Input Bit Normal */
typedef struct __st_input_digital_bit_normal__
{
	/* 1106.0 ~ 1106.F */
	UINT16				auto_maint_mode:1;						/* 자동/메인트 키스위치 MODE */
	UINT16				wafer_chuck_vacuum_status:1;			/* 웨이퍼 흡착 완료 */
	UINT16				wafer_chuck_atm_status:1;				/* 웨이퍼 진공 해제로 탈착 가능조건 */
	UINT16				robot_vacuum_status:1;
	UINT16				robot_atm_status:1;
	UINT16				aligner_vacuum_status:1;
	UINT16				aligner_atm_status_:1;
	UINT16				safety_reset_possible_status:1;			/* 인터락 해제 후 SAFETY RESET 요구조건이 갖춰 졌을시 켜짐 */
	UINT16				spare_d1106_8:1;
	UINT16				spare_d1106_9:1;
	UINT16				stage_wafer_detact:1;
	UINT16				buffer_wafer_in_detact:1;
	UINT16				buffer_wafer_out_detact:1;
	UINT16				robot_output_signal_1_ex_alarm:1;
	UINT16				robot_output_signal_2:1;
	UINT16				robot_output_signal_3:1;
	/* 1107.0 ~ 1107.F */
	UINT16				robot_output_signal_4:1;
	UINT16				robot_output_signal_5:1;
	UINT16				robot_output_signal_6:1;
	UINT16				robot_output_signal_7:1;
	UINT16				robot_output_signal_8:1;
	UINT16				spare_4535:1;
	UINT16				signal_tower_green_lamp:1;				/* 시그날 타워의 최종 동작값을 PC 에 알려줌 */
	UINT16				signal_tower_yellow_lamp_flashing:1;
	UINT16				signal_tower_red_lamp_flashing:1;
	UINT16				signal_tower_buzzer_1:1;
	UINT16				xy_stage_move_prohibit_status:1;		/* XY STAGE MOVE PHOHIBIT 상태를 알 수 있음 */
	UINT16				spare_d1107_b:1;
	UINT16				spare_d1107_c:1;
	UINT16				spare_d1107_d:1;
	UINT16				spare_d1107_e:1;
	UINT16				spare_d1107_f:1;
	/* 1108.0 ~ 1108.F */
	UINT16				spare_word_d1108;

}	STG_IDBN,	*LPG_IDBN;

/* TEL Interface Input (Track -> Exposure) */
typedef struct __st_input_digital_track_exposure__
{
	/* 1109.0 ~ 1109.F */
	UINT16				trk_ink_track_inline:1;					/* 트랙에서 장비로 주는 인터페이스 신호 */
	UINT16				trk_rdy_track_ready:1;
	UINT16				trk_snd_track_send:1;
	UINT16				trk_lend_track_end_of_lot:1;
	UINT16				trk_reserve_1:1;
	UINT16				trk_reserve_2:1;
	UINT16				trk_reserve_3:1;
	UINT16				trk_reserve_4:1;
	UINT16				trk_reserve_5:1;
	UINT16				spare_d1109_9:1;
	UINT16				spare_d1109_a:1;
	UINT16				spare_d1109_b:1;
	UINT16				spare_d1109_c:1;
	UINT16				spare_d1109_d:1;
	UINT16				spare_d1109_e:1;
	UINT16				spare_d1109_f:1;

}	STG_IDTE,	*LPG_IDTE;
typedef union __un_input_digital_track_exposure__
{
	STG_IDTE			s_scan;				/* 구조체 값 */
	UINT16				w_scan;				/* 변수 값 */

}	UNG_IDTE,	*LUG_IDTE;

/* Analog Input Value Normal */
typedef struct __st_input_analog_value_normal__
{
	INT16				differential_pressure;					/* -500 ~ +500 (-50.0 ~ +50.0 pa. 1 -> 0.1 ) */
	INT16				monitoring_temperature;					/* -300 ~ +1000 (-30.0 ~ +100.0 degree. 1 -> 0.1) */
	INT16				z1_z3_axis_cooling_air_pressure;		/* -100 ~ +1000 (1 -> 1) kPa */
	INT16				z1_z3_axis_cyclinder_air_pressure;		/* -100 ~ +1000 (1 -> 1) kPa */
	UINT16				power_box_temperature;					/* 0 ~ 12000 (0.0 ~ +1200.0. 1 -> 0.1 ℃)*/
	UINT16				pc_rack_upper_monitoring_temperature;	/* 0 ~ 1000 (0.0 ~ +1200.0. 1 -> 0.1 ℃)*/
	UINT16				pc_rack_bottom_monitoring_temperature;	/* 0 ~ 1000 (0.0 ~ +1200.0. 1 -> 0.1 ℃)*/
	UINT16				spare_word_45D0;
	UINT16				spare_word_45E0;
	UINT16				spare_word_45F0;

}	STG_IAVN,	*LPG_IAVN;

/* Read Lift Pin [ X / Z / BSA ] Axis (Lamp) Bit */
typedef struct __st_input_lift_axis_bit__
{
	UINT16				axis_status:1;
	UINT16				axis_p_limit_position:1;
	UINT16				axis_n_limit_psition:1;
	UINT16				axis_position:1;
	UINT16				axis_loading_position:1;	/* 해당축 로딩포지션 센서에 감지함 */
	UINT16				axis_safety_position:1;		/* 해당축 안전위치 센서에 감지함 => MC2 MOTION 이동 가능 조건 */
	UINT16				spare_06:1;
	UINT16				spare_07:1;
	UINT16				spare_08:1;
	UINT16				spare_09:1;
	UINT16				spare_0A:1;
	UINT16				spare_0B:1;
	UINT16				spare_0C:1;
	UINT16				spare_0D:1;
	UINT16				spare_0E:1;
	UINT16				spare_0F:1;

}	STG_ILAB,	*LPG_ILAB;

/* Read Lift Pin [ X / Z / BSA ] Axis (Lamp) Word (Value) */
typedef struct __st_input_lift_axis_value__
{
	UINT32				axis_currnt_position_display;
	UINT16				axis_alarm_code;

}	STG_ILAV,	*LPG_ILAV;

/* Digital Output Bit Normal */
typedef struct __st_output_digital_bit_normal__
{
	UINT16				safety_reset_cmd:1;						/* 안전 리셋 명령 D1106.7(SAFETY RESET POSSIBLE STATUS)를 OFF 할 수 있음. */
	UINT16				tower_green_lamp_cmd:1;					/* 타워 램프&부저 ON/OFF 명령 */
	UINT16				tower_yellow_lamp_flashing_cmd:1;
	UINT16				tower_red_lamp_flashing_cmd:1;
	UINT16				tower_buzzer_1_cmd:1;
	UINT16				z1_axis_air_cooling_on_off:1;			/* Z축 에어 쿨링 (장비 MAINT상황이 아니라면 항시 ON) */
	UINT16				z2_axis_air_cooling_on_off:1;
	UINT16				z3_axis_air_cooling_on_off:1;
	UINT16				z1_axis_cylinder_on_off:1;				/* Z축 실린더 ON (장비 MAINT상황이 아니라면 항시 ON) */
	UINT16				z2_axis_cylinder_on_off:1;
	UINT16				z3_axis_cylinder_on_off:1;
	UINT16				wafer_chuck_vacuum_on_off:1;			/* 웨이퍼 흡착 ON/OFF */
	UINT16				wafer_chuck_exhaust_on_off:1;			/* 웨이퍼 탈착 ON (흡착도 OFF해야함) */
	UINT16				robot_vacuumuum_on_off:1;
	UINT16				aligner_vacuum_on_off:1;
	UINT16				photohead_1_power_off_cmd:1;			/* 광학계의 전원을 OFF할수 있음 */

	UINT16				photohead_2_power_off_cmd:1;
	UINT16				illumination_sensor_pwr_on_off:1;		/* 조도계의 전원을 ON/OFF 할수 있음 */
	UINT16				halogen_ring_light_power_on:1;			/* 할로겐 램프의 전원을 킴 */
	UINT16				halogen_coaxial_light_power_on:1;
	UINT16				spare_bit_1:1;
	UINT16				spare_bit_2:1;
	UINT16				robot_input_signal_1:1;					/* 로봇에게 받는 신호(현재는 사용하지 않음) */
	UINT16				robot_input_signal_2:1;
	UINT16				robot_input_signal_3:1;
	UINT16				robot_input_signal_4:1;
	UINT16				robot_input_signal_5:1;
	UINT16				robot_input_signal_6:1;
	UINT16				robot_input_signal_7:1;
	UINT16				robot_input_signal_8:1;
	UINT16				robot_input_signal_9:1;
	UINT16				robot_input_signal_10:1;

	UINT16				xy_stage_move_prohibit:1;				/* 장비에서 트랙으로 주는 인터페이스 신호 */
	UINT16				xy_stage_move_prohibit_release:1;
	UINT16				spare_bits:14;

}	STG_ODBN,	*LPG_ODBN;

/* TEL Interface Input (Track -> Exposure) */
typedef struct __st_output_digital_exposure_track__
{
	UINT16				exp_ink_exposure_inline:1;
	UINT16				exp_rej_exposure_reject:1;
	UINT16				exp_snd_exposure_send_to_ready:1;
	UINT16				exp_rdy_exposure_receive_to_ready:1;
	UINT16				exp_reserve_1:1;
	UINT16				exp_reserve_2:1;
	UINT16				exp_reserve_3:1;
	UINT16				exp_reserve_4:1;
	UINT16				exp_reserve_5:1;
	UINT16				spare_bits:7;

}	STG_ODET,	*LPG_ODET;
typedef union __un_output_digital_exposure_track__
{
	STG_ODET			s_track;			/*_구조체_값_*/
	UINT16				w_track;			/*_변수_값_*/

}	UNG_ODET,	*LUG_ODET;

/* Analog Output Value Normal */
typedef struct __st_output_analog_value_normal__
{
	UINT16				halogen_ring_light_power;
	UINT16				halogen_coaxial_light_power;

}	STG_OAVN,	*LPG_OAVN;

/* Analog Output Value Etc */
typedef struct __st_output_analog_value_etc__
{
	UINT16				pc_live_check;
	UINT16				spare_word[3];

}	STG_OAVE,	*LPG_OAVE;

/* Write Lift Pin [ X / Z / BSA ] Axis (Lamp) Bit */
typedef struct __st_output_lift_axis_bit__
{
	UINT16				axis_enable_disable:1;		/* PLC가 자동으로 ON 함 */
	UINT16				axis_reset:1;				/* 해당축 운영 시 감지한 알람을 리셋함 */
	UINT16				axis_stop:1;
	UINT16				spare_bit_1:1;
	UINT16				axis_jog_plus_move:1;		/* 해당 명령은 리프트핀 z축이 SAFETY SENSOR가 감지시 사용 가능= 감지 안된 후 사용시 USER 알람 발생 */
	UINT16				axis_jog_minus_move:1;
	UINT16				axis_home_position:1;
	UINT16				axis_position:1;
	UINT16				axis_dreve_alarm_reset:1;	/* 해당축 드라이브의 하드웨어 알람을 해제함 */
	UINT16				spare_bits:7;

}	STG_OLAB,	*LPG_OLAB;

/* Write Lift Pin [ X / Z / BSA ] Axis (Lamp) Word (Value) */
typedef struct __st_output_lift_axis_value__
{
	UINT32				axis_jog_speed_setting;
	UINT32				axis_position_speed_setting;
	UINT32				axis_position_setting;

}	STG_OLAV,	*LPG_OLAV;

/* PLC I/O 구조체 (PIO 포함) */
typedef struct __st_plc_memory_data_value__
{
	/* Input Address (D1100 ~ D1149) : Total 50 Word */
	STG_IDBA			in_alarm;
	STG_IDBW			in_warn;
	STG_IDBN			in_normal;
	UNG_IDTE			in_track_expo;	/* PIO : Scanner (or Exposure) Side (Track -> Exposure) */
	STG_IAVN			in_analog;

	STG_ILAB			in_lift_x_bit;
	STG_ILAB			in_lift_z_bit;
	STG_ILAB			in_lift_bsa_bit;

	STG_ILAV			in_lift_x_value;
	STG_ILAV			in_lift_z_value;
	STG_ILAV			in_lift_bsa_value;

	UINT16				u16_in_spare[18];	/* 1132 ~ 1149 */

	/* Output Address (D1150 ~ D1199) : Total 50 Word */
	STG_ODBN			out_normal;
	UNG_ODET			out_expo_track;
	STG_OAVN			out_analog;
	STG_OAVE			out_live_etc;

	STG_OLAB			out_lift_x_bit;
	STG_OLAB			out_lift_z_bit;
	STG_OLAB			out_lift_bsa_bit;

	STG_OLAV			out_lift_x_value;
	STG_OLAV			out_lift_z_value;
	STG_OLAV			out_lift_bsa_value;

	UINT16				u16_out_spare[19];	/* 1181 ~ 1199 */

	/*
	 desc : 현재 Lift Pin이 Safe Position 영역에 있는지 여부
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsLiftPinSafePos()
	{
		return (in_lift_x_bit.axis_safety_position == 0x01 &&
				in_lift_z_bit.axis_safety_position == 0x01);
	}

	/*
	 desc : 입력된 PIO 상태와 현재 메모리의 PIO 상태 비교
	 parm : scan	- [in]  Scanner의 PIO 상태 값
			track	- [in]  Track의 PIO 상태 값
	 retn : TRUE or FALSE
	*/
	BOOL IsEqualPIO(UINT16 scan, UINT16 track)
	{
		return (in_track_expo.w_scan == scan && out_expo_track.w_track == track);
	}

}	STG_PMDV,	*LPG_PMDV;

/* Track & Expose State */
typedef struct __st_state_track_state_expo__
{
	UNG_IDTE			in_track_expo;
	UNG_ODET			out_expo_track;

}	STG_STSE,	*LPG_STSE;


#pragma pack (pop)	/* 2 bytes */