using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Runtime.InteropServices;

/* User-defined Namespace */
using TGen2i.Enum.PLC;
using TGen2i.Struct.PLC;
using TGen2i.EngineLib;

namespace TGen2i.MenuChild
{
	/* PLC Information (Item, Value) */
	public class PLCData
	{
		public PLCData(string item, string value)
		{
			m_strItem	= item;
			m_strValue	= value;
		}

		public string m_strItem		{	get; set;	}
		public string m_strValue	{	get; set;	}

		/*
		 desc : store word value as string value
		 parm : value	- [in]  Word value
		 retn : None
		*/
		public void SetValue(UInt16 value)
		{
			m_strValue	= String.Format("{0:0}", value);
		}
		public void SetValue(UInt32 value)
		{
			m_strValue	= String.Format("{0:0}", value);
		}
		public void SetValue(UInt16 value, string unit)
		{
			m_strValue	= String.Format("{0:N0} {1}", value, unit);
		}
		public void SetValue(UInt32 value, string unit)
		{
			m_strValue	= String.Format("{0:N0} {1}", value, unit);
		}
		public void SetValue(Int16 value, string unit)
		{
			m_strValue	= String.Format("{0:N0} {1}", value, unit);
		}
		public void SetValue(Int32 value, string unit)
		{
			m_strValue	= String.Format("{0:N0} {1}", value, unit);
		}
		public void SetValue(Double value, UInt16 point, string unit)
		{
			string strFmt = "{0:N} {2}";
			switch (point)
			{
			case 0x0001: strFmt = "{0:N1} {1}";	break;
			case 0x0002: strFmt = "{0:N2} {1}";	break;
			case 0x0003: strFmt = "{0:N3} {1}";	break;
			}
			m_strValue	= String.Format(strFmt, value, unit);
		}
		public void SetPower(Byte value)
		{
			if (0x01 == value)	m_strValue	= "ON";
			else				m_strValue	= "OFF";
		}
		public void SetPowerInvert(Byte value)
		{
			if (0x01 == value)	m_strValue	= "OFF";
			else				m_strValue	= "ON";
		}
		public void SetInvert(Byte value)
		{
			if (0x01 == value)	m_strValue	= "ON/OFF";
			else				m_strValue	= "NONE";
		}
		public void SetLimit(Byte value)
		{
			if (0x01 == value)	m_strValue	= "OFF";
			else				m_strValue	= "ON";
		}
		public void SetAlarm(Byte value)
		{
			if (0x01 == value)	m_strValue	= "NORMAL";
			else				m_strValue	= "ALARM";
		}
		public void SetDoor(Byte value)
		{
			if (0x01 == value)	m_strValue	= "CLOSED";
			else				m_strValue	= "OPENED";
		}
		public void SetLeak(Byte value)
		{
			if (0x01 == value)	m_strValue	= "NORMAL";
			else				m_strValue	= "LEAK";
		}
		public void SetMode(Byte value)
		{
			if (0x01 == value)	m_strValue	= "AUTO";
			else				m_strValue	= "MAINT";
		}
		public void SetVacuum(Byte value)
		{
			if (0x01 == value)	m_strValue	= "VACUUM";
			else				m_strValue	= "NONE";
		}
		public void SetAtm(Byte value)
		{
			if (0x01 == value)	m_strValue	= "ATM";
			else				m_strValue	= "NONE";
		}
		public void SetSafety(Byte value)
		{
			if (0x01 == value)	m_strValue	= "RESET REQUIRE";
			else				m_strValue	= "NONE";
		}
		public void SetStopNone(Byte value)
		{
			if (0x01 == value)	m_strValue	= "STOP";
			else				m_strValue	= "OFF";
		}
		public void SetHome(Byte value)
		{
			if (0x01 == value)	m_strValue	= "HOME";
			else				m_strValue	= "NONE";
		}
		public void SetMove(Byte value, Byte flag)
		{
			if (0x01 == value)
			{
				if (0x00 == flag)	m_strValue	= "+MOVE";
				else				m_strValue	= "-MOVE";
			}
			else					m_strValue	= "STOP";
		}
		public void SetResetCmd(Byte value)
		{
			if (0x01 == value)	m_strValue	= "RESET";
			else				m_strValue	= "OFF";
		}
		public void SetEnable(Byte value)
		{
			if (0x01 == value)	m_strValue	= "ENABLE";
			else				m_strValue	= "DISABLE";
		}
		public void SetDetect(Byte value)
		{
			if (0x01 == value)	m_strValue	= "DETECT";
			else				m_strValue	= "NONE";
		}
		public void SetBusy(Byte value)
		{
			if (0x01 == value)	m_strValue	= "BUSY";
			else				m_strValue	= "READY";
		}
		public void SetPosition(Byte value)
		{
			if (0x01 == value)	m_strValue	= "POS";
			else				m_strValue	= "NONE";
		}
		public void SetStopOff(Byte value)
		{
			if (0x01 == value)	m_strValue	= "STOP";
			else				m_strValue	= "NONE";
		}
		public void SetRelease(Byte value)
		{
			if (0x01 == value)	m_strValue	= "RELEASE";
			else				m_strValue	= "OFF";
		}
		public void SetSpare(Byte value)
		{
			if (0x01 == value)	m_strValue	= "Byte_1";
			else				m_strValue	= "Byte_0";
		}
		public void SetSpare(UInt16 value)
		{
			if (0x01 == value)	m_strValue	= "UInt16_1";
			else				m_strValue	= "UInt16_0";
		}
	}

	public partial class MenuPLC : MenuBase
	{
		private SByte m_i8PageNum	= 0x00;	/* The page number of grid control */

		/*
		 desc : Constructor
		 parm : None
		 retn : None
		*/
		public MenuPLC()
		{
			/* Initialize internal components. */
			InitializeComponent();

			/* Window Event */

			/* Control Event for Button */
			this.m_btn_plc_grid_page_prev.Click								+= btnClickGridPages;
			this.m_btn_plc_grid_page_next.Click								+= btnClickGridPages;
			this.m_btn_plc_halogen_ring_light_power.Click					+= btnClickWritePLC;
			this.m_btn_plc_halogen_coaxial_light_power.Click				+= btnClickWritePLC;
			this.m_btn_plc_pc_live_check.Click								+= btnClickWritePLC;

			this.m_btn_plc_lift_pin_x_axis_jog_speed_setting.Click			+= btnClickWritePLC;
			this.m_btn_plc_lift_pin_x_axis_position_speed_setting.Click		+= btnClickWritePLC;
			this.m_btn_plc_lift_pin_x_axis_position_setting.Click			+= btnClickWritePLC;

			this.m_btn_plc_lift_pin_z_axis_jog_speed_setting.Click			+= btnClickWritePLC;
			this.m_btn_plc_lift_pin_z_axis_position_speed_setting.Click		+= btnClickWritePLC;
			this.m_btn_plc_lift_pin_z_axis_position_setting.Click			+= btnClickWritePLC;

			this.m_btn_plc_bsa_camera_z_axis_jog_speed_setting.Click		+= btnClickWritePLC;
			this.m_btn_plc_bsa_camera_z_axis_position_speed_setting.Click	+= btnClickWritePLC;
			this.m_btn_plc_bsa_camera_z_axis_position_setting.Click			+= btnClickWritePLC;

			/* Control Event for Checkbox */
			this.m_chk_plc_safety_reset_cmd.Click					+= chkClickWritePLC;
			this.m_chk_plc_tower_green_lamp_cmd.Click				+= chkClickWritePLC;
			this.m_chk_plc_tower_yellow_lamp_cmd.Click				+= chkClickWritePLC;
			this.m_chk_plc_tower_red_lamp_cmd.Click					+= chkClickWritePLC;
			this.m_chk_plc_tower_buzzer_1_cmd.Click					+= chkClickWritePLC;

			this.m_chk_plc_z1_axis_air_cooling_on_off.Click			+= chkClickWritePLC;
			this.m_chk_plc_z2_axis_air_cooling_on_off.Click			+= chkClickWritePLC;
			this.m_chk_plc_z3_axis_air_cooling_on_off.Click			+= chkClickWritePLC;
			this.m_chk_plc_z1_axis_cylinder_on_off.Click			+= chkClickWritePLC;
			this.m_chk_plc_z2_axis_cylinder_on_off.Click			+= chkClickWritePLC;
			this.m_chk_plc_z3_axis_cylinder_on_off.Click			+= chkClickWritePLC;
			this.m_chk_plc_wafer_chuck_vacuum_on_off.Click			+= chkClickWritePLC;
			this.m_chk_plc_wafer_chuck_exhaust_on_off.Click			+= chkClickWritePLC;
			this.m_chk_plc_robot_vacuumuum_on_off.Click				+= chkClickWritePLC;
			this.m_chk_plc_aligner_vacuum_on_off.Click				+= chkClickWritePLC;
			this.m_chk_plc_photohead_1_power_off_cmd.Click			+= chkClickWritePLC;

			this.m_chk_plc_photohead_2_power_off_cmd.Click			+= chkClickWritePLC;
			this.m_chk_plc_illumination_sensor_pwr_on_off.Click		+= chkClickWritePLC;
			this.m_chk_plc_halogen_ring_light_power_on.Click		+= chkClickWritePLC;
			this.m_chk_plc_halogen_coaxial_light_power_on.Click		+= chkClickWritePLC;
			this.m_chk_plc_robot_input_signal_01.Click				+= chkClickWritePLC;
			this.m_chk_plc_robot_input_signal_02.Click				+= chkClickWritePLC;
			this.m_chk_plc_robot_input_signal_03.Click				+= chkClickWritePLC;
			this.m_chk_plc_robot_input_signal_04.Click				+= chkClickWritePLC;
			this.m_chk_plc_robot_input_signal_05.Click				+= chkClickWritePLC;
			this.m_chk_plc_robot_input_signal_06.Click				+= chkClickWritePLC;
			this.m_chk_plc_robot_input_signal_07.Click				+= chkClickWritePLC;
			this.m_chk_plc_robot_input_signal_08.Click				+= chkClickWritePLC;
			this.m_chk_plc_robot_input_signal_09.Click				+= chkClickWritePLC;
			this.m_chk_plc_robot_input_signal_10.Click				+= chkClickWritePLC;

			this.m_chk_plc_xy_stage_move_prohibit.Click				+= chkClickWritePLC;

			this.m_chk_plc_exp_ink_exposure_inline.Click			+= chkClickWritePLC;
			this.m_chk_plc_exp_rej_exposure_reject.Click			+= chkClickWritePLC;
			this.m_chk_plc_exp_snd_exposure_send_to_ready.Click		+= chkClickWritePLC;
			this.m_chk_plc_exp_rdy_exposure_receive_to_ready.Click	+= chkClickWritePLC;
			this.m_chk_plc_exp_rserve_1.Click						+= chkClickWritePLC;
			this.m_chk_plc_exp_rserve_2.Click						+= chkClickWritePLC;
			this.m_chk_plc_exp_rserve_3.Click						+= chkClickWritePLC;
			this.m_chk_plc_exp_rserve_4.Click						+= chkClickWritePLC;
			this.m_chk_plc_exp_rserve_5.Click						+= chkClickWritePLC;

			this.m_chk_plc_lift_pin_x_axis_enable_disable.Click		+= chkClickWritePLC;
			this.m_chk_plc_lift_pin_x_axis_reset.Click				+= chkClickWritePLC;
			this.m_chk_plc_lift_pin_x_axis_stop.Click				+= chkClickWritePLC;
			this.m_chk_plc_lift_pin_x_axis_jog_plus_move.Click		+= chkClickWritePLC;
			this.m_chk_plc_lift_pin_x_axis_jog_minus_move.Click		+= chkClickWritePLC;
			this.m_chk_plc_lift_pin_x_axis_home_position.Click		+= chkClickWritePLC;
			this.m_chk_plc_lift_pin_x_axis_position.Click			+= chkClickWritePLC;
			this.m_chk_plc_lift_pin_x_axis_drive_alarm_reset.Click	+= chkClickWritePLC;

			this.m_chk_plc_lift_pin_z_axis_enable_disable.Click		+= chkClickWritePLC;
			this.m_chk_plc_lift_pin_z_axis_reset.Click				+= chkClickWritePLC;
			this.m_chk_plc_lift_pin_z_axis_stop.Click				+= chkClickWritePLC;
			this.m_chk_plc_lift_pin_z_axis_jog_plus_move.Click		+= chkClickWritePLC;
			this.m_chk_plc_lift_pin_z_axis_jog_minus_move.Click		+= chkClickWritePLC;
			this.m_chk_plc_lift_pin_z_axis_home_position.Click		+= chkClickWritePLC;
			this.m_chk_plc_lift_pin_z_axis_position.Click			+= chkClickWritePLC;
			this.m_chk_plc_lift_pin_z_axis_drive_alarm_reset.Click	+= chkClickWritePLC;

			this.m_chk_plc_bsa_camera_z_axis_enable_disable.Click	+= chkClickWritePLC;
			this.m_chk_plc_bsa_camera_z_axis_reset.Click			+= chkClickWritePLC;
			this.m_chk_plc_bsa_camera_z_axis_stop.Click				+= chkClickWritePLC;
			this.m_chk_plc_bsa_camera_z_axis_jog_plus_move.Click	+= chkClickWritePLC;
			this.m_chk_plc_bsa_camera_z_axis_jog_minus_move.Click	+= chkClickWritePLC;
			this.m_chk_plc_bsa_camera_z_axis_home_position.Click	+= chkClickWritePLC;
			this.m_chk_plc_bsa_camera_z_axis_position.Click			+= chkClickWritePLC;
		}

		/* ------------------------------------------------------------------------------------- */
		/*                              Windows  Override Function                               */
		/* ------------------------------------------------------------------------------------- */

		/*
		 desc : It is called once for the first time after creating a window.
		 parm : e	- [in]  Event Arguments
		 retn : None
		*/
		protected override void OnInitialized(EventArgs e)
		{
			/* Virtual functions must be called */
			base.OnInitialized(e);

			/* Init the control */
			InitCtrl();

			/* Set the data.grid to output the PLC monitoring value. */
			SetDataGrid(true);

			/* Must set the flag that the control is initialized */
			base.InitCompleted();
		}

		/*
		 desc : When the window closes, it is called last time.
		 parm : e	- [in]  Event Arguments
		 retn : None
		*/
		protected override void OnClosed(EventArgs e)
		{

			/* Virtual functions must be called */
			base.OnClosed(e);
		}

		/* ------------------------------------------------------------------------------------- */
		/*                            User-defined Override Function                             */
		/* ------------------------------------------------------------------------------------- */

		/*
		 desc : Updates the control in the window
		 parm : None
		 retn : None
		*/
		public override void UpdateControl()
		{
			/* Need to call parent's virtual function */
			base.UpdateControl();	/* Set the time the function was called */

			/* The control is updated every 0.5 seconds */
			if (!base.IsWaitedTime(500))	return;
			if (!base.IsInitCompleted())	return;

			/* Refresh the content of the page */
			SetDataGrid(false);
			/* Checkbox. PLC Bit */
			UpdateCheckIO();

			/* Updates the most recently called time */
			base.UpdateCalledTime();
		}

		/* ------------------------------------------------------------------------------------- */
		/*                                 Windows Event Function                                */
		/* ------------------------------------------------------------------------------------- */

		/* ------------------------------------------------------------------------------------- */
		/*                                 Control Event Function                                */
		/* ------------------------------------------------------------------------------------- */

		/*
		 desc : Button event for updating the grid pages
		 parm : s	- [in]  Object
				e	- [in]  EventArgs
		 retn : None
		*/
		private void btnClickGridPages(object s, RoutedEventArgs e)
		{
			FrameworkElement feID	= e.Source as FrameworkElement;

			switch (feID.Name)
			{
			case "m_btn_plc_grid_page_prev"		:	m_i8PageNum--;	break;
			case "m_btn_plc_grid_page_next"		:	m_i8PageNum++;	break;
			}

			if (0x00 == m_i8PageNum)	m_btn_plc_grid_page_prev.IsEnabled = false;
			else						m_btn_plc_grid_page_prev.IsEnabled = true;
			if (0x0b == m_i8PageNum)	m_btn_plc_grid_page_next.IsEnabled = false;
			else						m_btn_plc_grid_page_next.IsEnabled = true;

			/* Refresh the content of the page */
			SetDataGrid(true);
		}

		/*
		 desc : Button event for PLC Output Refresh
		 parm : s	- [in]  Object
				e	- [in]  EventArgs
		 retn : None
		*/
		private void btnClickWritePLC(object s, RoutedEventArgs e)
		{
			UInt16 u16Val	= 0;
			UInt32 u32Val	= 0;
			ENG_PIOA enAddr	= 0x0000;
			FrameworkElement feID	= e.Source as FrameworkElement;

			switch (feID.Name)
			{
			case "m_btn_plc_halogen_ring_light_power"		:
			case "m_btn_plc_halogen_coaxial_light_power"	:
			case "m_btn_plc_pc_live_check"					:
				switch (feID.Name)
				{
				case "m_btn_plc_halogen_ring_light_power"	:
					enAddr	= ENG_PIOA.en_halogen_ring_light_power;
					u16Val	= Convert.ToUInt16(Math.Round(Convert.ToDouble(m_box_plc_halogen_ring_light_power.Text) / 0.12));
					break;
				case "m_btn_plc_halogen_coaxial_light_power":
					enAddr	= ENG_PIOA.en_halogen_coaxial_light_power;
					u16Val	= Convert.ToUInt16(Math.Round(Convert.ToDouble(m_box_plc_halogen_coaxial_light_power.Text) / 0.12));
					break;
				case "m_btn_plc_pc_live_check"				:
					enAddr	= ENG_PIOA.en_pc_live_check;
					u16Val	= Convert.ToUInt16(m_txt_plc_pc_live_check.Text);
					break;
				}
				OutputWord(enAddr, u16Val);
				break;
			case "m_btn_plc_lift_pin_x_axis_jog_speed_setting"			:
			case "m_btn_plc_lift_pin_x_axis_position_speed_setting"		:
			case "m_btn_plc_lift_pin_x_axis_position_setting"			:
			case "m_btn_plc_lift_pin_z_axis_jog_speed_setting"			:
			case "m_btn_plc_lift_pin_z_axis_position_speed_setting"		:
			case "m_btn_plc_lift_pin_z_axis_position_setting"			:
			case "m_btn_plc_bsa_camera_z_axis_jog_speed_setting"		:
			case "m_btn_plc_bsa_camera_z_axis_position_speed_setting"	:
			case "m_btn_plc_bsa_camera_z_axis_position_setting"			:
				switch (feID.Name)
				{
				case "m_btn_plc_lift_pin_x_axis_jog_speed_setting"			:
					enAddr	= ENG_PIOA.en_lift_pin_xo_axis_jog_speed_setting;
					u32Val	= Convert.ToUInt32(Math.Round(Convert.ToDouble(m_box_plc_lift_pin_x_axis_jog_speed_setting.Text) / 0.01));
					break;
				case "m_btn_plc_lift_pin_x_axis_position_speed_setting"		:
					enAddr	= ENG_PIOA.en_lift_pin_xo_axis_position_speed_setting;
					u32Val	= Convert.ToUInt32(Math.Round(Convert.ToDouble(m_box_plc_lift_pin_x_axis_position_speed_setting.Text) / 0.01));
					break;
				case "m_btn_plc_lift_pin_x_axis_position_setting"			:
					enAddr	= ENG_PIOA.en_lift_pin_xo_axis_position_setting;
					u32Val	= Convert.ToUInt32(Math.Round(Convert.ToDouble(m_box_plc_lift_pin_x_axis_position_setting.Text) / 0.1));
					break;
				case "m_btn_plc_lift_pin_z_axis_jog_speed_setting"			:
					enAddr	= ENG_PIOA.en_lift_pin_zo_axis_jog_speed_setting;
					u32Val	= Convert.ToUInt32(Math.Round(Convert.ToDouble(m_box_plc_lift_pin_z_axis_jog_speed_setting.Text) / 0.01));
					break;
				case "m_btn_plc_lift_pin_z_axis_position_speed_setting"		:
					enAddr	= ENG_PIOA.en_lift_pin_zo_axis_position_speed_setting;
					u32Val	= Convert.ToUInt32(Math.Round(Convert.ToDouble(m_box_plc_lift_pin_z_axis_position_speed_setting.Text) / 0.01));
					break;
				case "m_btn_plc_lift_pin_z_axis_position_setting"			:
					enAddr	= ENG_PIOA.en_lift_pin_zo_axis_position_setting;
					u32Val	= Convert.ToUInt32(Math.Round(Convert.ToDouble(m_box_plc_lift_pin_z_axis_position_setting.Text) / 0.1));
					break;
				case "m_btn_plc_bsa_camera_z_axis_jog_speed_setting"		:
					enAddr	= ENG_PIOA.en_bsa_camera_zo_axis_jog_speed_setting;
					u32Val	= Convert.ToUInt32(Math.Round(Convert.ToDouble(m_box_plc_bsa_camera_z_axis_jog_speed_setting.Text) / 0.01));
					break;
				case "m_btn_plc_bsa_camera_z_axis_position_speed_setting"	:
					enAddr	= ENG_PIOA.en_bsa_camera_zo_axis_position_speed_setting;
					u32Val	= Convert.ToUInt32(Math.Round(Convert.ToDouble(m_box_plc_bsa_camera_z_axis_position_speed_setting.Text) / 0.01));
					break;
				case "m_btn_plc_bsa_camera_z_axis_position_setting"			:
					enAddr	= ENG_PIOA.en_bsa_camera_zo_axis_position_setting;
					u32Val	= Convert.ToUInt32(Math.Round(Convert.ToDouble(m_box_plc_bsa_camera_z_axis_position_setting.Text) / 0.1));
					break;
				default:	return;
				}
				OutputWord(enAddr, u32Val);
				break;
			default:	return;
			}
		}

		/*
		 desc : Check event for PLC Output Value
		 parm : s	- [in]  Object
				e	- [in]  EventArgs
		 retn : None
		*/
		private void chkClickWritePLC(object s, RoutedEventArgs e)
		{
			Boolean bCheck	= false;
			ENG_PIOA enAddr;
			FrameworkElement feID	= e.Source as FrameworkElement;

			/* Set the starting address value */
			enAddr = ENG_PIOA.en_safety_reset_cmd;

			switch (feID.Name)
			{
			case "m_chk_plc_safety_reset_cmd"					: enAddr += 0x00;	break;
			case "m_chk_plc_tower_green_lamp_cmd"				: enAddr += 0x01;	break;
			case "m_chk_plc_tower_yellow_lamp_cmd"				: enAddr += 0x02;	break;
			case "m_chk_plc_tower_red_lamp_cmd"					: enAddr += 0x03;	break;
			case "m_chk_plc_tower_buzzer_1_cmd"					: enAddr += 0x04;	break;
			case "m_chk_plc_z1_axis_air_cooling_on_off"			: enAddr += 0x05;	break;
			case "m_chk_plc_z2_axis_air_cooling_on_off"			: enAddr += 0x06;	break;
			case "m_chk_plc_z3_axis_air_cooling_on_off"			: enAddr += 0x07;	break;
			case "m_chk_plc_z1_axis_cylinder_on_off"			: enAddr += 0x08;	break;
			case "m_chk_plc_z2_axis_cylinder_on_off"			: enAddr += 0x09;	break;
			case "m_chk_plc_z3_axis_cylinder_on_off"			: enAddr += 0x0a;	break;
			case "m_chk_plc_wafer_chuck_vacuum_on_off"			: enAddr += 0x0b;	break;
			case "m_chk_plc_wafer_chuck_exhaust_on_off"			: enAddr += 0x0c;	break;
			case "m_chk_plc_robot_vacuumuum_on_off"				: enAddr += 0x0d;	break;
			case "m_chk_plc_aligner_vacuum_on_off"				: enAddr += 0x0e;	break;
			case "m_chk_plc_photohead_1_power_off_cmd"			: enAddr += 0x0f;	break;
			case "m_chk_plc_photohead_2_power_off_cmd"			: enAddr += 0x10;	break;
			case "m_chk_plc_illumination_sensor_pwr_on_off"		: enAddr += 0x11;	break;
			case "m_chk_plc_halogen_ring_light_power_on"		: enAddr += 0x12;	break;
			case "m_chk_plc_halogen_coaxial_light_power_on"		: enAddr += 0x13;	break;
			case "m_chk_plc_robot_input_signal_01"				: enAddr += 0x16;	break;
			case "m_chk_plc_robot_input_signal_02"				: enAddr += 0x17;	break;
			case "m_chk_plc_robot_input_signal_03"				: enAddr += 0x18;	break;
			case "m_chk_plc_robot_input_signal_04"				: enAddr += 0x19;	break;
			case "m_chk_plc_robot_input_signal_05"				: enAddr += 0x1a;	break;
			case "m_chk_plc_robot_input_signal_06"				: enAddr += 0x1b;	break;
			case "m_chk_plc_robot_input_signal_07"				: enAddr += 0x1c;	break;
			case "m_chk_plc_robot_input_signal_08"				: enAddr += 0x1d;	break;
			case "m_chk_plc_robot_input_signal_09"				: enAddr += 0x1e;	break;
			case "m_chk_plc_robot_input_signal_10"				: enAddr += 0x1f;	break;
			case "m_chk_plc_xy_stage_move_prohibit"				: enAddr += 0x20;	break;
			case "m_chk_plc_xy_stage_move_prohibit_release"		: enAddr += 0x21;	break;
			case "m_chk_plc_exp_ink_exposure_inline"			: enAddr += 0x30;	break;
			case "m_chk_plc_exp_rej_exposure_reject"			: enAddr += 0x31;	break;
			case "m_chk_plc_exp_snd_exposure_send_to_ready"		: enAddr += 0x32;	break;
			case "m_chk_plc_exp_rdy_exposure_receive_to_ready"	: enAddr += 0x33;	break;
			case "m_chk_plc_exp_rserve_1"						: enAddr += 0x34;	break;
			case "m_chk_plc_exp_rserve_2"						: enAddr += 0x35;	break;
			case "m_chk_plc_exp_rserve_3"						: enAddr += 0x36;	break;
			case "m_chk_plc_exp_rserve_4"						: enAddr += 0x37;	break;
			case "m_chk_plc_exp_rserve_5"						: enAddr += 0x38;	break;

			case "m_chk_plc_lift_pin_x_axis_enable_disable"		: enAddr += 0xa0;	break;
			case "m_chk_plc_lift_pin_x_axis_reset"				: enAddr += 0xa1;	break;
			case "m_chk_plc_lift_pin_x_axis_stop"				: enAddr += 0xa2;	break;
			case "m_chk_plc_lift_pin_x_axis_jog_plus_move"		: enAddr += 0xa4;	break;
			case "m_chk_plc_lift_pin_x_axis_jog_minus_move"		: enAddr += 0xa5;	break;
			case "m_chk_plc_lift_pin_x_axis_home_position"		: enAddr += 0xa6;	break;
			case "m_chk_plc_lift_pin_x_axis_position"			: enAddr += 0xa7;	break;
			case "m_chk_plc_lift_pin_x_axis_drive_alarm_reset"	: enAddr += 0xa8;	break;

			case "m_chk_plc_lift_pin_z_axis_enable_disable"		: enAddr += 0xb0;	break;
			case "m_chk_plc_lift_pin_z_axis_reset"				: enAddr += 0xb1;	break;
			case "m_chk_plc_lift_pin_z_axis_stop"				: enAddr += 0xb2;	break;
			case "m_chk_plc_lift_pin_z_axis_jog_plus_move"		: enAddr += 0xb4;	break;
			case "m_chk_plc_lift_pin_z_axis_jog_minus_move"		: enAddr += 0xb5;	break;
			case "m_chk_plc_lift_pin_z_axis_home_position"		: enAddr += 0xb6;	break;
			case "m_chk_plc_lift_pin_z_axis_position"			: enAddr += 0xb7;	break;
			case "m_chk_plc_lift_pin_z_axis_drive_alarm_reset"	: enAddr += 0xb8;	break;

			case "m_chk_plc_bsa_camera_z_axis_enable_disable"	: enAddr += 0xc0;	break;
			case "m_chk_plc_bsa_camera_z_axis_reset"			: enAddr += 0xc1;	break;
			case "m_chk_plc_bsa_camera_z_axis_stop"				: enAddr += 0xc2;	break;
			case "m_chk_plc_bsa_camera_z_axis_jog_plus_move"	: enAddr += 0xc4;	break;
			case "m_chk_plc_bsa_camera_z_axis_jog_minus_move"	: enAddr += 0xc5;	break;
			case "m_chk_plc_bsa_camera_z_axis_home_position"	: enAddr += 0xc6;	break;
			case "m_chk_plc_bsa_camera_z_axis_position"			: enAddr += 0xc7;	break;
			}
			bCheck= ((CheckBox)feID).IsChecked.Value;

			/* Output the bit value */
			OutputBit(enAddr, Convert.ToByte(bCheck));
		}

		/* ------------------------------------------------------------------------------------- */
		/*                                  User Defined Function                                */
		/* ------------------------------------------------------------------------------------- */

		private void InitCtrl()
		{
			m_btn_plc_grid_page_prev.IsEnabled = false;
			m_btn_plc_grid_page_next.IsEnabled = true;
		}

		/*
		 desc : Output the bit, word or dword value
		 parm : addr	- [in]  32 bit address
				val		- [in]  1 bit value (0 or 1)
		 retn : true or false
		*/
		private Boolean OutputBit(ENG_PIOA addr, Byte val)
		{
			return EngPLC.uvEng_MCQ_WriteBitsValueEx(addr, val);
		}
		private Boolean OutputWord(ENG_PIOA addr, UInt16 val)
		{
			return EngPLC.uvEng_MCQ_WriteWordValueEx(addr, val);
		}
		private Boolean OutputWord(ENG_PIOA addr, UInt32 val)
		{
			return EngPLC.uvEng_MCQ_WriteDWordValueEx(addr, val);
		}

		/*
		 desc : Set the data.grid to output the PLC monitoring value.
		 parm : add	- [in]  whether to add items (true : yes, false : no)
		 retn : None
		*/
		private void SetDataGrid(Boolean add)
		{
			/* Remove all existing items */
			if (add)
			{
				m_grid_plc_io.Items.Clear();
				/* Change the Title */
				if (m_i8PageNum <0x06)	m_label_plc_io.Content	= "PLC Input";
				else					m_label_plc_io.Content	= "PLC Output";
			}
#if (false)
			/* Add new data */
			switch (m_i8PageNum)
			{
			case 0x00:	LoadDIAlarm(add);		break;
			case 0x01:	LoadDIWarn(add);		break;
			case 0x02:	LoadDINorm(add);		break;
			case 0x03:	LoadDITrackToExpo(add);	break;
			case 0x04:	LoadAINorm(add);		break;
			case 0x05:	LoadDILiftPin(add);		break;
			case 0x06:	LoadAILiftPin(add);		break;
			case 0x07:	LoadDONorm(add);		break;
			case 0x08:	LoadDOExpoToTrack(add);	break;
			case 0x09:	LoadAONorm(add);		break;
			case 0x0a:	LoadDOLiftPin(add);		break;
			case 0x0b:	LoadAOLiftPin(add);		break;
			}
#else
			IntPtr ptrPLC	= EngPLC.uvEng_ShMem_GetPLCStruct();
			STG_PMDV stPLC	= (STG_PMDV)Marshal.PtrToStructure(ptrPLC, typeof(STG_PMDV));
			if (add)
			{
				switch (m_i8PageNum)
				{
				case 0x00:	LoadDIAlarmItems();			break;
				case 0x01:	LoadDIWarnItems();			break;
				case 0x02:	LoadDINormItems();			break;
				case 0x03:	LoadDITrackToExpoItems();	break;
				case 0x04:	LoadAINormItems();			break;
				case 0x05:	LoadDILiftPinItems();		break;
				case 0x06:	LoadAILiftPinItems();		break;
				case 0x07:	LoadDONormItems();			break;
				case 0x08:	LoadDOExpoToTrackItems();	break;
				case 0x09:	LoadAONormItems();			break;
				case 0x0a:	LoadDOLiftPinItems();		break;
				case 0x0b:	LoadAOLiftPinItems();		break;
				}
			}
			switch (m_i8PageNum)
			{
			case 0x00:	LoadDIAlarmValues(ref stPLC.in_alarm);															break;
			case 0x01:	LoadDIWarnValues(ref stPLC.in_warn);															break;
			case 0x02:	LoadDINormValues(ref stPLC.in_normal);															break;
			case 0x03:	LoadDITrackToExpoValues(ref stPLC.in_track_expo.s_scan);										break;
			case 0x04:	LoadAINormValues(ref stPLC.in_analog);															break;
			case 0x05:	LoadDILiftPinValues(ref stPLC.in_pin_x_bit, ref stPLC.in_pin_z_bit, ref stPLC.in_cam_z_bit);	break;
			case 0x06:	LoadAILiftPinValues(ref stPLC.in_pin_x_val, ref stPLC.in_pin_z_val, ref stPLC.in_cam_z_val);	break;
			case 0x07:	LoadDONormValues(ref stPLC.out_normal);															break;
			case 0x08:	LoadDOExpoToTrackValues(ref stPLC.out_expo_track.s_track);										break;
			case 0x09:	LoadAONormValues(ref stPLC.out_analog, ref stPLC.out_live_etc);									break;
			case 0x0a:	LoadDOLiftPinValues(ref stPLC.out_pin_x_bit, ref stPLC.out_pin_z_bit, ref stPLC.out_cam_z_bit);	break;
			case 0x0b:	LoadAOLiftPinValues(ref stPLC.out_pin_x_val, ref stPLC.out_pin_z_val, ref stPLC.out_cam_z_val);	break;
			}
#endif
			/* Updates all items */
			m_grid_plc_io.Items.Refresh();
		}

		/*
		 desc : Load the alaram value in the 1st word of the DI memory.
		 parm : None
		 retn : None
		*/
#if (true)
		private void LoadDIAlarmItems()
		{
			/* Add new items - D1100.0 ~ D1100.F */
			m_grid_plc_io.Items.Add(new PLCData(" SPD FAIL ALARM", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LINEAR Y AXIS POWER LINE (CP100)", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LINEAR Y AXIS POWER LINE (CP101)", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LINEAR Z1~Z3 AXIS SMPS POWER FAIL", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LINEAR Z1~Z3 POWER (CP103 ~ CP105)", ""));
			m_grid_plc_io.Items.Add(new PLCData(" OPTICS 1 SMPS POWER FAIL", ""));
			m_grid_plc_io.Items.Add(new PLCData(" OPTICS 2 SMPS POWER FAIL", ""));
			m_grid_plc_io.Items.Add(new PLCData(" OPTICS 1,2 POWER (CP107),(CP109)", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT & PREALIGNER POWER (CP110 ~ CP112)", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ALL SD2S CONTROL POWER (CP116)", ""));
			m_grid_plc_io.Items.Add(new PLCData(" MC2 POWER (CP117)", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFTPIN AXIS (CP119)", ""));
			m_grid_plc_io.Items.Add(new PLCData(" VISION SYSTEM POWER (CP122)", ""));
			m_grid_plc_io.Items.Add(new PLCData(" EFU MODUL POWER (CP123)", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SERVER PC 1,2 POWER (CP124 ~ CP125)", ""));
			m_grid_plc_io.Items.Add(new PLCData(" FRONT SAFETY COVER 1", ""));
			/* Add new items - D1101.0 ~ D1101.F */
			m_grid_plc_io.Items.Add(new PLCData(" FRONT SAFETY COVER 2", ""));
			m_grid_plc_io.Items.Add(new PLCData(" FRONT SAFETY COVER 3", ""));
			m_grid_plc_io.Items.Add(new PLCData(" FRONT SAFETY COVER 4", ""));
			m_grid_plc_io.Items.Add(new PLCData(" REAR SAFERTY COVER 1", ""));
			m_grid_plc_io.Items.Add(new PLCData(" REAR SAFERTY COVER 2", ""));
			m_grid_plc_io.Items.Add(new PLCData(" REAR SAFERTY COVER 3", ""));
			m_grid_plc_io.Items.Add(new PLCData(" REAR SAFERTY COVER 4", ""));
			m_grid_plc_io.Items.Add(new PLCData(" REAR SAFERTY COVER 5", ""));
			m_grid_plc_io.Items.Add(new PLCData(" UTILITY BOX WATER LEAK SENSOR", ""));
			m_grid_plc_io.Items.Add(new PLCData(" PH WATER LEAK SENSOR 1", ""));
			m_grid_plc_io.Items.Add(new PLCData(" PH WATER LEAK SENSOR 2", ""));
			m_grid_plc_io.Items.Add(new PLCData(" WATER FLOW LOW ALARM", ""));
			m_grid_plc_io.Items.Add(new PLCData(" WATER FLOW HIGH ALARM", ""));
			m_grid_plc_io.Items.Add(new PLCData(" WATER PRESSURE LOW ALARM", ""));
			m_grid_plc_io.Items.Add(new PLCData(" WATER PRESSURE HIGH ALARM", ""));
			m_grid_plc_io.Items.Add(new PLCData(" AIR PRESSURE LOW ALARM", ""));
			/* Add new items - D1102.0 ~ D1102.F */
			m_grid_plc_io.Items.Add(new PLCData(" HALOGEN LING LIGHT BURN OUT ALARM",""));
			m_grid_plc_io.Items.Add(new PLCData(" HALOGEN COAXIAL LIGHT BURN OUT ALARM",""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS DRIVE ALARM",""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS OPERATION ALARM",""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS HOME REQUEST ALARM",""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS DRIVE ALARM",""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS OPERATION ALARM",""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS HOME REQUEST ALARM",""));
			m_grid_plc_io.Items.Add(new PLCData(" BSA Z AXIS OPERATION ALARM",""));
			m_grid_plc_io.Items.Add(new PLCData(" BSA Z AXIS HOME REQUEST ALARM",""));
			m_grid_plc_io.Items.Add(new PLCData(" LED LAMP CONTROLLER POWER (CP127)",""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X NOT MOVE ALARM",""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z NOT MOVE ALARM",""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X POSITION COUNT ALARM",""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z POSITION COUNT ALARM",""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE",""));
		}
		private void LoadDIAlarmValues(ref STG_IDBA data)
		{
			/* D1100.0 ~ D1100.F */
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x00)).SetAlarm(data.spd_fail_alarm);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x01)).SetPower(data.linear_x_axis_power_line_cp100);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x02)).SetPower(data.linear_y_axis_power_line_cp101);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x03)).SetPower(data.linear_z1_z3_axis_smps_power_fail);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x04)).SetPower(data.linear_z1_z3_power_cp103_cp105);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x05)).SetPower(data.optics_1_smps_power_fail);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x06)).SetPower(data.optics_2_smps_power_fail);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x07)).SetPower(data.optics_1_2_power_cp107_cp109);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x08)).SetPower(data.robot_prealigner_power_cp110_cp112);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x09)).SetPower(data.all_sd2s_control_power_cp116);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0A)).SetPower(data.mc2_power_cp117);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0B)).SetPower(data.liftpin_axis_cp119);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0C)).SetPower(data.vision_system_power_cp122);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0D)).SetPower(data.efu_modul_power_cp123);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0E)).SetPower(data.server_pc_1_2_power_cp124_cp125);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0F)).SetDoor(data.front_safety_cover_1);
			/* D1101.0 ~ D1101.F */
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x10)).SetDoor(data.front_safety_cover_2);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x11)).SetDoor(data.front_safety_cover_3);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x12)).SetDoor(data.front_safety_cover_4);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x13)).SetDoor(data.rear_saferty_cover_1);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x14)).SetDoor(data.rear_saferty_cover_2);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x15)).SetDoor(data.rear_saferty_cover_3);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x16)).SetDoor(data.rear_saferty_cover_4);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x17)).SetDoor(data.rear_saferty_cover_5);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x18)).SetLeak(data.utility_box_water_leak_sensor);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x19)).SetLeak(data.ph_water_leak_sensor_1);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1A)).SetLeak(data.ph_water_leak_sensor_2);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1B)).SetAlarm(data.water_flow_low_alarm);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1C)).SetAlarm(data.water_flow_high_alarm);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1D)).SetAlarm(data.water_pressure_low_alarm);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1E)).SetAlarm(data.water_pressure_high_alarm);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1F)).SetAlarm(data.air_pressure_low_alarm);
			/* D1102.0 ~ D1102.F */
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x20)).SetAlarm(data.halogen_ring_light_burn_out_alarm);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x21)).SetAlarm(data.halogen_coaxial_light_burn_out_alarm);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x22)).SetAlarm(data.lift_pin_x_axis_drive_alarm);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x23)).SetAlarm(data.lift_pin_x_axis_operation_alarm);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x24)).SetAlarm(data.lift_pin_x_axis_home_request_alarm);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x25)).SetAlarm(data.lift_pin_z_axis_drive_alarm);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x26)).SetAlarm(data.lift_pin_z_axis_operation_alarm);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x27)).SetAlarm(data.lift_pin_z_axis_home_request_alarm);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x28)).SetAlarm(data.bsa_z_axis_operation_alarm);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x29)).SetAlarm(data.bsa_z_axis_home_request_alarm);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x2A)).SetAlarm(data.led_lamp_controller_power_cp127);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x2B)).SetAlarm(data.lift_pin_x_not_move_alarm);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x2C)).SetAlarm(data.lift_pin_z_not_move_alarm);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x2D)).SetAlarm(data.lift_pin_x_position_count_alarm);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x2E)).SetAlarm(data.lift_pin_z_position_count_alarm);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x2F)).SetAlarm(data.z1_z3_air_pressure_low_alarm);
		}
		/*
		 desc : Load the warn value in the 1st word of the DI memory.
		 parm : None
		 retn : None
		*/
		private void LoadDIWarnItems()
		{
			/* Add new items - D1104.0 ~ D1104.F */
			m_grid_plc_io.Items.Add(new PLCData(" PC RACK UPPER OUT FAN ALARM 1",""));
			m_grid_plc_io.Items.Add(new PLCData(" PC RACK UPPER OUT FAN ALARM 2",""));
			m_grid_plc_io.Items.Add(new PLCData(" PC RACK BOTTOM OUT FAN ALARM 1",""));
			m_grid_plc_io.Items.Add(new PLCData(" PC RACK BOTTOM OUT FAN ALARM 2",""));
			m_grid_plc_io.Items.Add(new PLCData(" POWER BOX IN FAN ALARM 1",""));
			m_grid_plc_io.Items.Add(new PLCData(" POWER BOX IN FAN ALARM 2",""));
			m_grid_plc_io.Items.Add(new PLCData(" POWER BOX IN FAN ALARM 3",""));
			m_grid_plc_io.Items.Add(new PLCData(" POWER BOX IN FAN ALARM 4",""));
			m_grid_plc_io.Items.Add(new PLCData(" POWER BOX OUT FAN ALARM 1",""));
			m_grid_plc_io.Items.Add(new PLCData(" POWER BOX OUT FAN ALARM 2",""));
			m_grid_plc_io.Items.Add(new PLCData(" POWER BOX OUT FAN ALARM 3",""));
			m_grid_plc_io.Items.Add(new PLCData(" POWER BOX OUT FAN ALARM 4",""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE",""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE",""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE",""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE",""));
		}
		private void LoadDIWarnValues(ref STG_IDBW data)
		{
			/* D1102.0 ~ D1102.F */
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x00)).SetAlarm(data.pc_rack_upper_out_fan_alarm_1);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x01)).SetAlarm(data.pc_rack_upper_out_fan_alarm_2);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x02)).SetAlarm(data.pc_rack_bottom_out_fan_alarm_1);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x03)).SetAlarm(data.pc_rack_bottom_out_fan_alarm_2);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x04)).SetAlarm(data.power_box_in_fan_alarm_1);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x05)).SetAlarm(data.power_box_in_fan_alarm_2);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x06)).SetAlarm(data.power_box_in_fan_alarm_3);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x07)).SetAlarm(data.power_box_in_fan_alarm_4);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x08)).SetAlarm(data.power_box_out_fan_alarm_1);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x09)).SetAlarm(data.power_box_out_fan_alarm_2);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0A)).SetAlarm(data.power_box_out_fan_alarm_3);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0B)).SetAlarm(data.power_box_out_fan_alarm_4);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0C)).SetSpare(data.spare_d1104_c);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0D)).SetSpare(data.spare_d1104_d);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0E)).SetSpare(data.spare_d1104_e);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0F)).SetSpare(data.spare_d1104_f);
		}

		/*
		 desc : Load the normal value in the 1st word of the DI memory.
		 parm : None
		 retn : None
		*/
		private void LoadDINormItems()
		{
			/* Add new items - D1106.0 ~ D1106.F */
			m_grid_plc_io.Items.Add(new PLCData(" AUTO / MAINT MODE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" WAFER CHUCK VACUUM STATUS", ""));
			m_grid_plc_io.Items.Add(new PLCData(" WAFER CHUCK ATM STATUS", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT VACUUM STATUS", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT ATM STATUS", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ALIGNER VACUUM STATUS", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ALIGNER ATM STATUS ", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SAFETY RESET POSSIBLE STATUS", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" STAGE WAFER DETACT", ""));
			m_grid_plc_io.Items.Add(new PLCData(" BUFFER WAFER IN DETACT", ""));
			m_grid_plc_io.Items.Add(new PLCData(" BUFFER WAFER OUT DETACT", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT OUTPUT SIGNAL 1 (EX. ALARM)", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT OUTPUT SIGNAL 2", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT OUTPUT SIGNAL 3", ""));
			/* Add new items - D1107.0 ~ D1107.F */
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT OUTPUT SIGNAL 4", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT OUTPUT SIGNAL 5", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT OUTPUT SIGNAL 6", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT OUTPUT SIGNAL 7", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT OUTPUT SIGNAL 8", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SIGNAL TOWER GREEN LAMP", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SIGNAL TOWER YELLOW LAMP FLASHING", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SIGNAL TOWER RED LAMP FLASHING", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SIGNAL TOWER BUZZER 1", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
		}
		private void LoadDINormValues(ref STG_IDBN data)
		{
			/* 1106.0 ~ 1106.F */
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x00)).SetMode(data.auto_maint_mode);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x01)).SetVacuum(data.wafer_chuck_vacuum_status);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x02)).SetAtm(data.wafer_chuck_atm_status);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x03)).SetVacuum(data.robot_vacuum_status);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x04)).SetAtm(data.robot_atm_status);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x05)).SetVacuum(data.aligner_vacuum_status);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x06)).SetAtm(data.aligner_atm_status);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x07)).SetSafety(data.safety_reset_possible_status);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x08)).SetSpare(data.spare_d1106_8);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x09)).SetSpare(data.spare_d1106_9);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0A)).SetDetect(data.stage_wafer_detact);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0B)).SetDetect(data.buffer_wafer_in_detact);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0C)).SetDetect(data.buffer_wafer_out_detact);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0D)).SetPower(data.robot_output_signal_1_ex_alarm);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0E)).SetPower(data.robot_output_signal_2);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0F)).SetPower(data.robot_output_signal_3);
			/* 1107.0 ~ 1107.F */
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x10)).SetPower(data.robot_output_signal_4);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x11)).SetPower(data.robot_output_signal_5);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x12)).SetPower(data.robot_output_signal_6);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x13)).SetPower(data.robot_output_signal_7);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x14)).SetPower(data.robot_output_signal_8);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x15)).SetSpare(data.spare_d1107_5);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x16)).SetPower(data.signal_tower_green_lamp);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x17)).SetPower(data.signal_tower_yellow_lamp_flashing);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x18)).SetPower(data.signal_tower_red_lamp_flashing);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x19)).SetPower(data.signal_tower_buzzer_1);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1A)).SetPower(data.xy_stage_move_prohibit_status);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1B)).SetSpare(data.spare_d1107_b);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1C)).SetSpare(data.spare_d1107_c);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1D)).SetSpare(data.spare_d1107_d);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1E)).SetSpare(data.spare_d1107_e);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1F)).SetSpare(data.spare_d1107_f);
		}

		/*
		 desc : Load the normal value in the 1st word of the DI memory.
		 parm : None
		 retn : None
		*/
		private void LoadDITrackToExpoItems()
		{
			/* Add new items - D1109.0 ~ D1109.F */
			m_grid_plc_io.Items.Add(new PLCData(" TRK-INK (TRACK INLINE)", ""));
			m_grid_plc_io.Items.Add(new PLCData(" TRK-RDY (TRACK READY)", ""));
			m_grid_plc_io.Items.Add(new PLCData(" TRK-SND (TRACK SEND)", ""));
			m_grid_plc_io.Items.Add(new PLCData(" TRK-LEND (TRACK END OF LOT)", ""));
			m_grid_plc_io.Items.Add(new PLCData(" TRK-RSERVE 1", ""));
			m_grid_plc_io.Items.Add(new PLCData(" TRK-RSERVE 2", ""));
			m_grid_plc_io.Items.Add(new PLCData(" TRK-RSERVE 3", ""));
			m_grid_plc_io.Items.Add(new PLCData(" TRK-RSERVE 4", ""));
			m_grid_plc_io.Items.Add(new PLCData(" TRK-RSERVE 5", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
		}
		private void LoadDITrackToExpoValues(ref STG_IDTE data)
		{
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x00)).SetPower(data.trk_ink_track_inline);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x01)).SetPower(data.trk_rdy_track_ready);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x02)).SetPower(data.trk_snd_track_send);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x03)).SetPower(data.trk_lend_track_end_of_lot);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x04)).SetPower(data.trk_reserve_1);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x05)).SetPower(data.trk_reserve_2);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x06)).SetPower(data.trk_reserve_3);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x07)).SetPower(data.trk_reserve_4);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x08)).SetPower(data.trk_reserve_5);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x09)).SetSpare(data.spare_d1109_9);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0A)).SetSpare(data.spare_d1109_a);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0B)).SetSpare(data.spare_d1109_b);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0C)).SetSpare(data.spare_d1109_c);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0D)).SetSpare(data.spare_d1109_d);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0E)).SetSpare(data.spare_d1109_e);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0F)).SetSpare(data.spare_d1109_f);
		}

		/*
		 desc : Load the normal value in the 1st word of the AI memory.
		 parm : None
		 retn : None
		*/
		private void LoadAINormItems()
		{
			/* Add new items - D1110.0 ~ D1110.F */
			m_grid_plc_io.Items.Add(new PLCData(" DIFFERENTIAL PRESSURE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" MONITORING TEMPERATURE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" AIR PRESSURE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" POWER BOX TEMPERATURE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" PC RACK UPPER MONITORING TEMPERATURE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" PC RACK BOTTOM MONITORING TEMPERATURE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
		}
		private void LoadAINormValues(ref STG_IAVN data)
		{
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x00)).SetValue(data.differential_pressure/10.0, 1, "Pa");		/* D1110 */
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x01)).SetValue(data.monitoring_temperature/10.0, 1, "°");		/* D1111 */
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x02)).SetValue(data.z1_z3_axis_cooling_air_pressure, "kPa");	/* D1112 */
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x03)).SetValue(data.z1_z3_axis_cyclinder_air_pressure, "kPa");	/* D1113 */
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x04)).SetValue(data.power_box_temperature/10.0, 1, "℃");
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x05)).SetValue(data.pc_rack_upper_monitoring_temperature/10.0, 1, "℃");
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x06)).SetValue(data.pc_rack_bottom_monitoring_temperature/10.0, 1, "℃");
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x07)).SetSpare(data.spare_word_d1117);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x08)).SetSpare(data.spare_word_d1118);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x09)).SetSpare(data.spare_word_d1119);
		}

		/*
		 desc : Load the normal value in the 1st word of the DI memory.
		 parm : None
		 retn : None
		*/
		private void LoadDILiftPinItems()
		{
			/* Add new items - D1120.0 ~ D1122.F */
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS STATUS", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS P_LIMIT POSITION", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS N_LIMIT POSITION", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS POSITION", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS LOADING POSITION", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS SAFETY POSITION", ""));

			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS STATUS", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS P_LIMIT POSITION", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS N_LIMIT POSITION", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS POSITION", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS LOADING POSITION", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS SAFETY POSITION", ""));

			m_grid_plc_io.Items.Add(new PLCData(" BSA Z AXIS STATUS", ""));
			m_grid_plc_io.Items.Add(new PLCData(" BSA Z AXIS P_LIMIT POSITION", ""));
			m_grid_plc_io.Items.Add(new PLCData(" BSA Z AXIS N_LIMIT POSITION", ""));
			m_grid_plc_io.Items.Add(new PLCData(" BSA Z AXIS POSITION", ""));
		}
		private void LoadDILiftPinValues(ref STG_ILAB pin_x, ref STG_ILAB pin_z, ref STG_ILAB cam_z)
		{
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x00)).SetBusy(pin_x.axis_status);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x01)).SetLimit(pin_x.axis_p_limit_position);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x02)).SetLimit(pin_x.axis_n_limit_psition);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x03)).SetPosition(pin_x.axis_position);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x04)).SetPosition(pin_x.axis_loading_position);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x05)).SetPosition(pin_x.axis_safety_position);

			((PLCData)m_grid_plc_io.Items.GetItemAt(0x06)).SetBusy(pin_z.axis_status);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x07)).SetLimit(pin_z.axis_p_limit_position);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x08)).SetLimit(pin_z.axis_n_limit_psition);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x09)).SetPosition(pin_z.axis_position);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0a)).SetPosition(pin_z.axis_loading_position);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0b)).SetPosition(pin_z.axis_safety_position);

			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0c)).SetBusy(cam_z.axis_status);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0d)).SetLimit(cam_z.axis_p_limit_position);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0e)).SetLimit(cam_z.axis_n_limit_psition);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0f)).SetPosition(cam_z.axis_position);
		}

		/*
		 desc : Load the normal value in the 1st word of the AI memory.
		 parm : None
		 retn : None
		*/
		private void LoadAILiftPinItems()
		{
			/* Add new items - D1123.0 ~ D1131.F */
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS CURRNT POSITION DISPLAY", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS ALARM CODE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS CURRNT POSITION DISPLAY", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS ALARM CODE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS CURRNT POSITION DISPLAY", ""));
			m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS ALARM CODE", ""));
		}
		private void LoadAILiftPinValues(ref STG_ILAV pin_x, ref STG_ILAV pin_z, ref STG_ILAV cam_z)
		{
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x00)).SetValue(pin_x.axis_currnt_position_display/10.0, 1, "um");
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x01)).SetValue(pin_x.axis_alarm_code);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x00)).SetValue(pin_z.axis_currnt_position_display/10.0, 1, "um");
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x01)).SetValue(pin_z.axis_alarm_code);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x00)).SetValue(cam_z.axis_currnt_position_display/10.0, 1, "um");
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x01)).SetValue(cam_z.axis_alarm_code);
		}

		/*
		 desc : Load the normal value in the 1st word of the DO memory.
		 parm : None
		 retn : None
		*/
		private void LoadDONormItems()
		{
			/* Add new items - D1150.0 ~ D1150.F */
			m_grid_plc_io.Items.Add(new PLCData(" SAFETY RESET CMD", ""));
			m_grid_plc_io.Items.Add(new PLCData(" TOWER GREEN LAMP CMD", ""));
			m_grid_plc_io.Items.Add(new PLCData(" TOWER YELLOW LAMP FLASHING CMD", ""));
			m_grid_plc_io.Items.Add(new PLCData(" TOWER RED LAMP FLASHING CMD", ""));
			m_grid_plc_io.Items.Add(new PLCData(" TOWER BUZZER 1 CMD", ""));
			m_grid_plc_io.Items.Add(new PLCData(" Z1 AXIS AIR COOLING ON/OFF", ""));
			m_grid_plc_io.Items.Add(new PLCData(" Z2 AXIS AIR COOLING ON/OFF", ""));
			m_grid_plc_io.Items.Add(new PLCData(" Z3 AXIS AIR COOLING ON/OFF", ""));
			m_grid_plc_io.Items.Add(new PLCData(" Z1 AXIS CYLINDER ON/OFF", ""));
			m_grid_plc_io.Items.Add(new PLCData(" Z2 AXIS CYLINDER ON/OFF", ""));
			m_grid_plc_io.Items.Add(new PLCData(" Z3 AXIS CYLINDER ON/OFF", ""));
			m_grid_plc_io.Items.Add(new PLCData(" WAFER CHUCK VACUUM ON/OFF", ""));
			m_grid_plc_io.Items.Add(new PLCData(" WAFER CHUCK EXHAUST ON/OFF", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT VACUUMUUM ON/OFF", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ALIGNER VACUUM ON/OFF ", ""));
			m_grid_plc_io.Items.Add(new PLCData(" PHOTOHEAD 1 POWER OFF CMD", ""));
			/* Add new items - D1151.0 ~ D1151.F */
			m_grid_plc_io.Items.Add(new PLCData(" PHOTOHEAD 2 POWER OFF CMD", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ILLUMINATION SENSOR PWR ON/OFF", ""));
			m_grid_plc_io.Items.Add(new PLCData(" HALOGEN LING LIGHT POWER ON", ""));
			m_grid_plc_io.Items.Add(new PLCData(" HALOGEN COAXIAL LIGHT POWER ON", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT INPUT SIGNAL 1", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT INPUT SIGNAL 2", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT INPUT SIGNAL 3", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT INPUT SIGNAL 4", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT INPUT SIGNAL 5", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT INPUT SIGNAL 6", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT INPUT SIGNAL 7", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT INPUT SIGNAL 8", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT INPUT SIGNAL 9", ""));
			m_grid_plc_io.Items.Add(new PLCData(" ROBOT INPUT SIGNAL 10", ""));
			/* Add new items - D1152.0 ~ D1152.F */
			m_grid_plc_io.Items.Add(new PLCData(" XY STAGE MOVE PROHIBIT", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
		}
		private void LoadDONormValues(ref STG_ODBN data)
		{
			/* 1150.0 ~ 1150.F */
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x00)).SetResetCmd(data.safety_reset_cmd);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x01)).SetPower(data.tower_green_lamp_cmd);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x02)).SetPower(data.tower_yellow_lamp_flashing_cmd);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x03)).SetPower(data.tower_red_lamp_flashing_cmd);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x04)).SetPower(data.tower_buzzer_1_cmd);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x05)).SetPower(data.z1_axis_air_cooling_on_off);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x06)).SetPower(data.z2_axis_air_cooling_on_off);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x07)).SetPower(data.z3_axis_air_cooling_on_off);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x08)).SetPower(data.z1_axis_cylinder_on_off);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x09)).SetPower(data.z2_axis_cylinder_on_off);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0A)).SetPower(data.z3_axis_cylinder_on_off);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0B)).SetPower(data.wafer_chuck_vacuum_on_off);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0C)).SetPower(data.wafer_chuck_exhaust_on_off);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0D)).SetPower(data.robot_vacuumuum_on_off);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0E)).SetPower(data.aligner_vacuum_on_off);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0F)).SetPowerInvert(data.photohead_1_power_off_cmd);
			/* 1151.0 ~ 1151.F */
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x10)).SetPowerInvert(data.photohead_2_power_off_cmd);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x11)).SetPower(data.illumination_sensor_pwr_on_off);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x12)).SetPower(data.halogen_ring_light_power_on);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x13)).SetPower(data.halogen_coaxial_light_power_on);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x14)).SetSpare(data.spare_d1151_3);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x15)).SetSpare(data.spare_d1151_4);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x16)).SetPower(data.robot_input_signal_1);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x17)).SetPower(data.robot_input_signal_2);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x18)).SetPower(data.robot_input_signal_3);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x19)).SetPower(data.robot_input_signal_4);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1A)).SetPower(data.robot_input_signal_5);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1B)).SetPower(data.robot_input_signal_6);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1C)).SetPower(data.robot_input_signal_7);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1D)).SetPower(data.robot_input_signal_8);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1E)).SetPower(data.robot_input_signal_9);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1F)).SetInvert(data.robot_input_signal_10);
			/* 1152.0 ~ 1152.F */
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x20)).SetStopNone(data.xy_stage_move_prohibit);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x21)).SetRelease(data.xy_stage_move_prohibit_release);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x22)).SetSpare(data.spare_d1152_2);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x23)).SetSpare(data.spare_d1152_3);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x24)).SetSpare(data.spare_d1152_4);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x25)).SetSpare(data.spare_d1152_5);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x26)).SetSpare(data.spare_d1152_6);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x27)).SetSpare(data.spare_d1152_7);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x28)).SetSpare(data.spare_d1152_8);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x29)).SetSpare(data.spare_d1152_9);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x2A)).SetSpare(data.spare_d1152_a);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x2B)).SetSpare(data.spare_d1152_b);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x2C)).SetSpare(data.spare_d1152_c);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x2D)).SetSpare(data.spare_d1152_d);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x2E)).SetSpare(data.spare_d1152_e);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x2F)).SetSpare(data.spare_d1152_f);
		}

		/*
		 desc : Load the normal value in the 1st word of the D0 memory.
		 parm : None
		 retn : None
		*/
		private void LoadDOExpoToTrackItems()
		{
			/* Add new items - D1153.0 ~ D1153.F */
			m_grid_plc_io.Items.Add(new PLCData(" EXP-INK (EXPOSURE INLINE)", ""));
			m_grid_plc_io.Items.Add(new PLCData(" EXP-REY (EXPOSURE REJECT)", ""));
			m_grid_plc_io.Items.Add(new PLCData(" EXP-SND (EXPOSURE SEND TO READY)", ""));
			m_grid_plc_io.Items.Add(new PLCData(" EXP-RDY (EXPOSURE RECEIVE TO READY)", ""));
			m_grid_plc_io.Items.Add(new PLCData(" EXP-RSERVE 1", ""));
			m_grid_plc_io.Items.Add(new PLCData(" EXP-RSERVE 2", ""));
			m_grid_plc_io.Items.Add(new PLCData(" EXP-RSERVE 3", ""));
			m_grid_plc_io.Items.Add(new PLCData(" EXP-RSERVE 4", ""));
			m_grid_plc_io.Items.Add(new PLCData(" EXP-RSERVE 5", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
		}
		private void LoadDOExpoToTrackValues(ref STG_ODET data)
		{
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x00)).SetPower(data.exp_ink_exposure_inline);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x01)).SetPower(data.exp_rej_exposure_reject);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x02)).SetPower(data.exp_snd_exposure_send_to_ready);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x03)).SetPower(data.exp_rdy_exposure_receive_to_ready);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x04)).SetSpare(data.exp_reserve_1);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x05)).SetSpare(data.exp_reserve_2);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x06)).SetSpare(data.exp_reserve_3);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x07)).SetSpare(data.exp_reserve_4);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x08)).SetSpare(data.exp_reserve_5);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x09)).SetSpare(data.spare_d1153_9);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0A)).SetSpare(data.spare_d1153_a);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0B)).SetSpare(data.spare_d1153_b);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0C)).SetSpare(data.spare_d1153_c);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0D)).SetSpare(data.spare_d1153_d);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0E)).SetSpare(data.spare_d1153_e);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0F)).SetSpare(data.spare_d1153_f);
		}

		/*
		 desc : Load the normal value in the 1st word of the AO memory.
		 parm : None
		 retn : None
		*/
		private void LoadAONormItems()
		{
			/* Add new items - D1154.0 ~ D1156.F */
			m_grid_plc_io.Items.Add(new PLCData(" HALOGEN LING LIGHT POWER", ""));
			m_grid_plc_io.Items.Add(new PLCData(" HALOGEN COAXIAL LIGHT POWER", ""));
			m_grid_plc_io.Items.Add(new PLCData(" PC LIVE CHECK", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
			m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
		}
		private void LoadAONormValues(ref STG_OAVN hal, ref STG_OAVE live)
		{
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x00)).SetValue(hal.halogen_ring_light_power*0.12 /* 1% = 0.12V */, 2, "V");
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x01)).SetValue(hal.halogen_coaxial_light_power*0.12 /* 1% = 0.12V */, 2, "V");
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x02)).SetValue(live.pc_live_check);
		}

		/*
		 desc : Load the normal value in the 1st word of the DO memory.
		 parm : None
		 retn : None
		*/
		private void LoadDOLiftPinItems()
		{
			/* Add new items - D1160.0 ~ D1162.F */
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS ENABLE DISABLE", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS RESET", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS STOP", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS JOG + MOVE", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS JOG - MOVE", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS HOME POSITION", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS POSITION", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS DREVE ALARM RESET", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 

			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS ENABLE DISABLE", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS RESET", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS STOP", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS JOG + MOVE", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS JOG - MOVE", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS HOME POSITION", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS POSITION", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS DRIVE ALARM RESET", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 

			m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS ENABLE DISABLE", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS RESET", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS STOP", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS JOG + MOVE", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS JOG - MOVE", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS HOME POSITION", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS POSITION", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
			m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
		}
		private void LoadDOLiftPinValues(ref STG_OLAB pin_x, ref STG_OLAB pin_z, ref STG_OLAB cam_z)
		{
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x00)).SetEnable(pin_x.axis_enable_disable);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x01)).SetResetCmd(pin_x.axis_reset);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x02)).SetStopOff(pin_x.axis_stop);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x03)).SetSpare(pin_x.spare_bit_1);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x04)).SetMove(pin_x.axis_jog_plus_move, 0x00);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x05)).SetMove(pin_x.axis_jog_minus_move, 0x01);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x06)).SetHome(pin_x.axis_home_position);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x07)).SetPosition(pin_x.axis_position);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x08)).SetResetCmd(pin_x.axis_dreve_alarm_reset);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x09)).SetSpare(pin_x.spare_d1160_9);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0A)).SetSpare(pin_x.spare_d1160_a);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0B)).SetSpare(pin_x.spare_d1160_b);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0C)).SetSpare(pin_x.spare_d1160_c);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0D)).SetSpare(pin_x.spare_d1160_d);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0E)).SetSpare(pin_x.spare_d1160_e);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x0F)).SetSpare(pin_x.spare_d1160_f);

			((PLCData)m_grid_plc_io.Items.GetItemAt(0x10)).SetEnable(pin_z.axis_enable_disable);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x11)).SetResetCmd(pin_z.axis_reset);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x12)).SetStopOff(pin_z.axis_stop);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x13)).SetSpare(pin_z.spare_bit_1);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x14)).SetMove(pin_z.axis_jog_plus_move, 0x00);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x15)).SetMove(pin_z.axis_jog_minus_move, 0x01);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x16)).SetHome(pin_z.axis_home_position);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x17)).SetPosition(pin_z.axis_position);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x18)).SetResetCmd(pin_z.axis_dreve_alarm_reset);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x19)).SetSpare(pin_z.spare_d1160_9);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1A)).SetSpare(pin_z.spare_d1160_a);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1B)).SetSpare(pin_z.spare_d1160_b);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1C)).SetSpare(pin_z.spare_d1160_c);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1D)).SetSpare(pin_z.spare_d1160_d);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1E)).SetSpare(pin_z.spare_d1160_e);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x1F)).SetSpare(pin_z.spare_d1160_f);

			((PLCData)m_grid_plc_io.Items.GetItemAt(0x20)).SetEnable(cam_z.axis_enable_disable);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x21)).SetResetCmd(cam_z.axis_reset);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x22)).SetStopOff(cam_z.axis_stop);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x23)).SetSpare(cam_z.spare_bit_1);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x24)).SetMove(cam_z.axis_jog_plus_move, 0x00);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x25)).SetMove(cam_z.axis_jog_minus_move, 0x01);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x26)).SetHome(cam_z.axis_home_position);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x27)).SetPosition(cam_z.axis_position);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x28)).SetSpare(cam_z.spare_d1160_9);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x29)).SetSpare(cam_z.spare_d1160_9);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x2A)).SetSpare(cam_z.spare_d1160_a);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x2B)).SetSpare(cam_z.spare_d1160_b);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x2C)).SetSpare(cam_z.spare_d1160_c);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x2D)).SetSpare(cam_z.spare_d1160_d);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x2E)).SetSpare(cam_z.spare_d1160_e);
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x2F)).SetSpare(cam_z.spare_d1160_f);
		}

		/*
		 desc : Load the normal value in the 1st word of the AO memory.
		 parm : None
		 retn : None
		*/
		private void LoadAOLiftPinItems()
		{
			/* Add new items - D1109.0 ~ D1109.F */
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS JOG SPEED SETTING", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS POSITION SPEED SETTING", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS POSITION SETTING", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS JOG SPEED SETTING", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS POSITION SPEED SETTING", ""));
			m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS POSITION SETTING", ""));
			m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS JOG SPEED SETTING", ""));
			m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS POSITION SPEED SETTING", ""));
			m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS POSITION SETTING", ""));
		}
		private void LoadAOLiftPinValues(ref STG_OLAV pin_x, ref STG_OLAV pin_z, ref STG_OLAV cam_z)
		{
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x00)).SetValue(pin_x.axis_jog_speed_setting/10.0, 1, "mm/min");
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x01)).SetValue(pin_x.axis_position_speed_setting/10.0, 1, "mm/min");
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x02)).SetValue(pin_x.axis_position_setting/10.0, 1, "um");
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x03)).SetValue(pin_z.axis_jog_speed_setting/10.0, 1, "mm/min");
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x04)).SetValue(pin_z.axis_position_speed_setting/10.0, 1, "mm/min");
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x05)).SetValue(pin_z.axis_position_setting/10.0, 1, "um");
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x06)).SetValue(cam_z.axis_jog_speed_setting/10.0, 1, "mm/min");
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x07)).SetValue(cam_z.axis_position_speed_setting/10.0, 1, "mm/min");
			((PLCData)m_grid_plc_io.Items.GetItemAt(0x08)).SetValue(cam_z.axis_position_setting/10.0, 1, "um");
		}
#else
		private void LoadDIAlarm(Boolean add)
		{
			Byte i, j, byVal= 0x00;
			UInt16 u16Val	= 0x0000;
			PLCData drItem	= null;

			try
			{
				/* replaces the new items */
				if (add)
				{
					/* Add new items - D1100.0 ~ D1100.F */
					m_grid_plc_io.Items.Add(new PLCData(" SPD FAIL ALARM", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LINEAR Y AXIS POWER LINE (CP100)", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LINEAR Y AXIS POWER LINE (CP101)", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LINEAR Z1~Z3 AXIS SMPS POWER FAIL", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LINEAR Z1~Z3 POWER (CP103 ~ CP105)", ""));
					m_grid_plc_io.Items.Add(new PLCData(" OPTICS 1 SMPS POWER FAIL", ""));
					m_grid_plc_io.Items.Add(new PLCData(" OPTICS 2 SMPS POWER FAIL", ""));
					m_grid_plc_io.Items.Add(new PLCData(" OPTICS 1,2 POWER (CP107),(CP109)", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT & PREALIGNER POWER (CP110 ~ CP112)", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ALL SD2S CONTROL POWER (CP116)", ""));
					m_grid_plc_io.Items.Add(new PLCData(" MC2 POWER (CP117)", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFTPIN AXIS (CP119)", ""));
					m_grid_plc_io.Items.Add(new PLCData(" VISION SYSTEM POWER (CP122)", ""));
					m_grid_plc_io.Items.Add(new PLCData(" EFU MODUL POWER (CP123)", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SERVER PC 1,2 POWER (CP124 ~ CP125)", ""));
					m_grid_plc_io.Items.Add(new PLCData(" FRONT SAFETY COVER 1", ""));
					/* Add new items - D1101.0 ~ D1101.F */
					m_grid_plc_io.Items.Add(new PLCData(" FRONT SAFETY COVER 2", ""));
					m_grid_plc_io.Items.Add(new PLCData(" FRONT SAFETY COVER 3", ""));
					m_grid_plc_io.Items.Add(new PLCData(" FRONT SAFETY COVER 4", ""));
					m_grid_plc_io.Items.Add(new PLCData(" REAR SAFERTY COVER 1", ""));
					m_grid_plc_io.Items.Add(new PLCData(" REAR SAFERTY COVER 2", ""));
					m_grid_plc_io.Items.Add(new PLCData(" REAR SAFERTY COVER 3", ""));
					m_grid_plc_io.Items.Add(new PLCData(" REAR SAFERTY COVER 4", ""));
					m_grid_plc_io.Items.Add(new PLCData(" REAR SAFERTY COVER 5", ""));
					m_grid_plc_io.Items.Add(new PLCData(" UTILITY BOX WATER LEAK SENSOR", ""));
					m_grid_plc_io.Items.Add(new PLCData(" PH WATER LEAK SENSOR 1", ""));
					m_grid_plc_io.Items.Add(new PLCData(" PH WATER LEAK SENSOR 2", ""));
					m_grid_plc_io.Items.Add(new PLCData(" WATER FLOW LOW ALARM", ""));
					m_grid_plc_io.Items.Add(new PLCData(" WATER FLOW HIGH ALARM", ""));
					m_grid_plc_io.Items.Add(new PLCData(" WATER PRESSURE LOW ALARM", ""));
					m_grid_plc_io.Items.Add(new PLCData(" WATER PRESSURE HIGH ALARM", ""));
					m_grid_plc_io.Items.Add(new PLCData(" AIR PRESSURE LOW ALARM", ""));
					/* Add new items - D1102.0 ~ D1102.F */
					m_grid_plc_io.Items.Add(new PLCData(" HALOGEN LING LIGHT BURN OUT ALARM",""));
					m_grid_plc_io.Items.Add(new PLCData(" HALOGEN COAXIAL LIGHT BURN OUT ALARM",""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS DRIVE ALARM",""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS OPERATION ALARM",""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS HOME REQUEST ALARM",""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS DRIVE ALARM",""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS OPERATION ALARM",""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS HOME REQUEST ALARM",""));
					m_grid_plc_io.Items.Add(new PLCData(" BSA Z AXIS OPERATION ALARM",""));
					m_grid_plc_io.Items.Add(new PLCData(" BSA Z AXIS HOME REQUEST ALARM",""));
					m_grid_plc_io.Items.Add(new PLCData(" LED LAMP CONTROLLER POWER (CP127)",""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X NOT MOVE ALARM",""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z NOT MOVE ALARM",""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X POSITION COUNT ALARM",""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z POSITION COUNT ALARM",""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE",""));
				}

				/* Update the current value (D1100.0 ~ D1100.F) */
				u16Val	= EngPLC.uvCmn_MCQ_GetWordMemory(0);
				for (i=0,j=0; i<16; i++,j++)
				{
					/* Get 1 bit value */
					byVal	= (Byte)((u16Val & (0x0001 << j)) >> j);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(i);
					switch (i)
					{
					case 0:		drItem.SetAlarm(byVal);	break;	/* normal or alarm */
					case 15:	drItem.SetDoor(byVal);	break;	/* open or close */
					default:	drItem.SetPower(byVal);	break;	/* on or off */
					}
				}
				/* Update the current value (D1101.0 ~ D1101.F) */
				u16Val	= EngPLC.uvCmn_MCQ_GetWordMemory(1);
				for (i=16,j=0; i<32; i++,j++)
				{
					/* Get 1 bit value */
					byVal	= (Byte)((u16Val & (0x0001 << j)) >> j);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(i);
					if (j < 8)			drItem.SetDoor(byVal);	/* open or close */
					else if (j < 11)	drItem.SetLeak(byVal);	/* normal or leak */
					else				drItem.SetAlarm(byVal);	/* normal or alarm */
				}
				/* Update the current value (D1102.0 ~ D1102.F) */
				u16Val	= EngPLC.uvCmn_MCQ_GetWordMemory(2);
				for (i=32,j=0; i<48; i++,j++)
				{
					/* Get 1 bit value */
					byVal	= (Byte)((u16Val & (0x0001 << j)) >> j);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(i);
					drItem.SetAlarm(byVal);	/* normal or alarm */
				}
			}
			catch (Exception ex)
			{
				System.Diagnostics.Debug.WriteLine($"Exception : {ex}");
				throw new Exception();
			}
			finally
			{
			}
		}
		/*
		 desc : Load the warn value in the 1st word of the DI memory.
		 parm : add	- [in]  whether to add items (true : yes, false : no)
		 retn : None
		*/
		private void LoadDIWarn(Boolean add)
		{
			Byte i, j, byVal= 0x00;
			UInt16 u16Val	= 0x0000;
			PLCData drItem	= null;

			try
			{
				/* replaces the new items */
				if (add)
				{
					/* Add new items - D1104.0 ~ D1104.F */
					m_grid_plc_io.Items.Add(new PLCData(" PC RACK UPPER OUT FAN ALARM 1",""));
					m_grid_plc_io.Items.Add(new PLCData(" PC RACK UPPER OUT FAN ALARM 2",""));
					m_grid_plc_io.Items.Add(new PLCData(" PC RACK BOTTOM OUT FAN ALARM 1",""));
					m_grid_plc_io.Items.Add(new PLCData(" PC RACK BOTTOM OUT FAN ALARM 2",""));
					m_grid_plc_io.Items.Add(new PLCData(" POWER BOX IN FAN ALARM 1",""));
					m_grid_plc_io.Items.Add(new PLCData(" POWER BOX IN FAN ALARM 2",""));
					m_grid_plc_io.Items.Add(new PLCData(" POWER BOX IN FAN ALARM 3",""));
					m_grid_plc_io.Items.Add(new PLCData(" POWER BOX IN FAN ALARM 4",""));
					m_grid_plc_io.Items.Add(new PLCData(" POWER BOX OUT FAN ALARM 1",""));
					m_grid_plc_io.Items.Add(new PLCData(" POWER BOX OUT FAN ALARM 2",""));
					m_grid_plc_io.Items.Add(new PLCData(" POWER BOX OUT FAN ALARM 3",""));
					m_grid_plc_io.Items.Add(new PLCData(" POWER BOX OUT FAN ALARM 4",""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE",""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE",""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE",""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE",""));
				}

				/* Update the current value (D1104.0 ~ D1104.F) */
				u16Val	= EngPLC.uvCmn_MCQ_GetWordMemory(4);
				for (i=0,j=0; i<16; i++,j++)
				{
					/* Get 1 bit value */
					byVal	= (Byte)((u16Val & (0x0001 << j)) >> j);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(i);
					drItem.SetAlarm(byVal);	/* normal or alarm */
				}
			}
			catch (Exception ex)
			{
				System.Diagnostics.Debug.WriteLine($"Exception : {ex}");
				throw new Exception();
			}
			finally
			{
			}
		}

		/*
		 desc : Load the normal value in the 1st word of the DI memory.
		 parm : add	- [in]  whether to add items (true : yes, false : no)
		 retn : None
		*/
		private void LoadDINorm(Boolean add)
		{
			Byte i, j, byVal= 0x00;
			UInt16 u16Val	= 0x0000;
			PLCData drItem	= null;

			try
			{
				/* replaces the new items */
				if (add)
				{
					/* Add new items - D1106.0 ~ D1106.F */
					m_grid_plc_io.Items.Add(new PLCData(" AUTO / MAINT MODE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" WAFER CHUCK VACUUM STATUS", ""));
					m_grid_plc_io.Items.Add(new PLCData(" WAFER CHUCK ATM STATUS", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT VACUUM STATUS", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT ATM STATUS", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ALIGNER VACUUM STATUS", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ALIGNER ATM STATUS ", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SAFETY RESET POSSIBLE STATUS", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" STAGE WAFER DETACT", ""));
					m_grid_plc_io.Items.Add(new PLCData(" BUFFER WAFER IN DETACT", ""));
					m_grid_plc_io.Items.Add(new PLCData(" BUFFER WAFER OUT DETACT", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT OUTPUT SIGNAL 1 (EX. ALARM)", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT OUTPUT SIGNAL 2", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT OUTPUT SIGNAL 3", ""));
					/* Add new items - D1107.0 ~ D1107.F */
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT OUTPUT SIGNAL 4", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT OUTPUT SIGNAL 5", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT OUTPUT SIGNAL 6", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT OUTPUT SIGNAL 7", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT OUTPUT SIGNAL 8", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SIGNAL TOWER GREEN LAMP", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SIGNAL TOWER YELLOW LAMP FLASHING", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SIGNAL TOWER RED LAMP FLASHING", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SIGNAL TOWER BUZZER 1", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
				}

				/* Update the current value (D1106.0 ~ D1106.F) */
				u16Val	= EngPLC.uvCmn_MCQ_GetWordMemory(6);
				for (i=0,j=0; i<16; i++,j++)
				{
					/* Get 1 bit value */
					byVal	= (Byte)((u16Val & (0x0001 << j)) >> j);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(i);
					switch (i)
					{
					case 0x00:		drItem.SetMode(byVal);		break;
					case 0x07:		drItem.SetSafety(byVal);	break;
					case 0x01:
					case 0x03:
					case 0x05:		drItem.SetVacuum(byVal);	break;
					case 0x02:
					case 0x04:
					case 0x06:		drItem.SetAtm(byVal);		break;
					case 0x0a:
					case 0x0b:
					case 0x0c:		drItem.SetDetect(byVal);	break;
					default:		drItem.SetPower(byVal);		break;
					}
				}
				/* Update the current value (D1107.0 ~ D1107.F) */
				u16Val	= EngPLC.uvCmn_MCQ_GetWordMemory(7);
				for (i=16,j=0; i<32; i++,j++)
				{
					/* Get 1 bit value */
					byVal	= (Byte)((u16Val & (0x0001 << j)) >> j);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(i);
					if (i < 21 || i > 21 && i < 26)	drItem.SetPower(byVal);
				}
			}
			catch (Exception ex)
			{
				System.Diagnostics.Debug.WriteLine($"Exception : {ex}");
				throw new Exception();
			}
			finally
			{
			}
		}

		/*
		 desc : Load the normal value in the 1st word of the DI memory.
		 parm : add	- [in]  whether to add items (true : yes, false : no)
		 retn : None
		*/
		private void LoadDITrackToExpo(Boolean add)
		{
			Byte i, j, byVal= 0x00;
			UInt16 u16Val	= 0x0000;
			PLCData drItem	= null;

			try
			{
				/* replaces the new items */
				if (add)
				{
					/* Add new items - D1109.0 ~ D1109.F */
					m_grid_plc_io.Items.Add(new PLCData(" TRK-INK (TRACK INLINE)", ""));
					m_grid_plc_io.Items.Add(new PLCData(" TRK-RDY (TRACK READY)", ""));
					m_grid_plc_io.Items.Add(new PLCData(" TRK-SND (TRACK SEND)", ""));
					m_grid_plc_io.Items.Add(new PLCData(" TRK-LEND (TRACK END OF LOT)", ""));
					m_grid_plc_io.Items.Add(new PLCData(" TRK-RSERVE 1", ""));
					m_grid_plc_io.Items.Add(new PLCData(" TRK-RSERVE 2", ""));
					m_grid_plc_io.Items.Add(new PLCData(" TRK-RSERVE 3", ""));
					m_grid_plc_io.Items.Add(new PLCData(" TRK-RSERVE 4", ""));
					m_grid_plc_io.Items.Add(new PLCData(" TRK-RSERVE 5", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
				}

				/* Update the current value (D1109.0 ~ D1109.F) */
				u16Val	= EngPLC.uvCmn_MCQ_GetWordMemory(9);
				for (i=0,j=0; i<16; i++,j++)
				{
					/* Get 1 bit value */
					byVal	= (Byte)((u16Val & (0x0001 << j)) >> j);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(i);
					if (i < 9)	drItem.SetPower(byVal);
				}
			}
			catch (Exception ex)
			{
				System.Diagnostics.Debug.WriteLine($"Exception : {ex}");
				throw new Exception();
			}
			finally
			{
			}
		}

		/*
		 desc : Load the normal value in the 1st word of the AI memory.
		 parm : add	- [in]  whether to add items (true : yes, false : no)
		 retn : None
		*/
		private void LoadAINorm(Boolean add)
		{
			Int32 i32Val	= 0x0000;
			UInt32 u32Val	= 0x0000;
			PLCData drItem	= null;

			try
			{
				/* replaces the new items */
				if (add)
				{
					/* Add new items - D1110.0 ~ D1110.F */
					m_grid_plc_io.Items.Add(new PLCData(" DIFFERENTIAL PRESSURE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" MONITORING TEMPERATURE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" AIR PRESSURE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" POWER BOX TEMPERATURE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" PC RACK UPPER MONITORING TEMPERATURE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" PC RACK BOTTOM MONITORING TEMPERATURE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
				}

				/* Update the current value (D1110) */
				i32Val	= (Int32)EngPLC.uvCmn_MCQ_GetWordMemory(10);
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(0);
				drItem.SetValue((Double)(i32Val/10.0), 1, "Pa");
				/* Update the current value (D1111) */
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(1);
				i32Val	= (Int32)EngPLC.uvCmn_MCQ_GetWordMemory(11);
				drItem.SetValue((Double)(i32Val/10.0), 1, "°");
				/* Update the current value (D1112) */
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(2);
				i32Val	= (Int32)EngPLC.uvCmn_MCQ_GetWordMemory(12);
				drItem.SetValue(i32Val, "kPa");
				/* Update the current value (D1114) */
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(4);
				u32Val	= EngPLC.uvCmn_MCQ_GetWordMemory(14);
				drItem.SetValue((Double)(u32Val/10.0), 1, "℃");
				/* Update the current value (D1115) */
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(5);
				u32Val	= EngPLC.uvCmn_MCQ_GetWordMemory(15);
				drItem.SetValue((Double)(u32Val/10.0), 1, "℃");
				/* Update the current value (D1116) */
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(6);
				u32Val	= EngPLC.uvCmn_MCQ_GetWordMemory(16);
				drItem.SetValue((Double)(u32Val/10.0), 1, "℃");
			}
			catch (Exception ex)
			{
				System.Diagnostics.Debug.WriteLine($"Exception : {ex}");
				throw new Exception();
			}
			finally
			{
			}
		}


		/*
		 desc : Load the normal value in the 1st word of the DI memory.
		 parm : add	- [in]  whether to add items (true : yes, false : no)
		 retn : None
		*/
		private void LoadDILiftPin(Boolean add)
		{
			Byte i, j, byVal= 0x00;
			UInt16 u16Val	= 0x0000;
			PLCData drItem	= null;

			try
			{
				/* replaces the new items */
				if (add)
				{
					/* Add new items - D1120.0 ~ D1122.F */
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS STATUS", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS P_LIMIT POSITION", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS N_LIMIT POSITION", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS POSITION", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS LOADING POSITION", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS SAFETY POSITION", ""));

					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS STATUS", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS P_LIMIT POSITION", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS N_LIMIT POSITION", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS POSITION", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS LOADING POSITION", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS SAFETY POSITION", ""));

					m_grid_plc_io.Items.Add(new PLCData(" BSA Z AXIS STATUS", ""));
					m_grid_plc_io.Items.Add(new PLCData(" BSA Z AXIS P_LIMIT POSITION", ""));
					m_grid_plc_io.Items.Add(new PLCData(" BSA Z AXIS N_LIMIT POSITION", ""));
					m_grid_plc_io.Items.Add(new PLCData(" BSA Z AXIS POSITION", ""));
				}

				/* Update the current value (D1120.0 ~ D1120.F) */
				u16Val	= EngPLC.uvCmn_MCQ_GetWordMemory(20);
				/* Get 1 bit value */
				byVal	= (Byte)((u16Val & (0x0001 << 0)) >> 0);
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(0);
				drItem.SetBusy(byVal);	/* busy or ready */
				/* Get 1 bit value */
				for (i=1,j=1; i<3; i++,j++)
				{
					byVal	= (Byte)((u16Val & (0x0001 << j)) >> j);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(i);
					drItem.SetLimit(byVal);/* off or on */
				}
				for (i=3,j=3; i<6; i++,j++)
				{
					byVal	= (Byte)((u16Val & (0x0001 << j)) >> j);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(i);
					drItem.SetPosition(byVal);	/* pos or none */
				}

				/* Update the current value (D1121.0 ~ D1121.F) */
				u16Val	= EngPLC.uvCmn_MCQ_GetWordMemory(21);
				/* Get 1 bit value */
				byVal	= (Byte)((u16Val & (0x0001 << 0)) >> 0);
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(6);
				drItem.SetBusy(byVal);	/* busy or ready */
				/* Get 1 bit value */
				for (i=7,j=1; i<9; i++,j++)
				{
					byVal	= (Byte)((u16Val & (0x0001 << j)) >> j);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(i);
					drItem.SetLimit(byVal);/* off or on */
				}
				for (i=9,j=3; i<12; i++,j++)
				{
					byVal	= (Byte)((u16Val & (0x0001 << j)) >> j);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(i);
					drItem.SetPosition(byVal);	/* pos or none */
				}

				/* Update the current value (D1122.0 ~ D1122.F) */
				u16Val	= EngPLC.uvCmn_MCQ_GetWordMemory(22);
				/* Get 1 bit value */
				byVal	= (Byte)((u16Val & (0x0001 << 0)) >> 0);
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(12);
				drItem.SetBusy(byVal);	/* busy or ready */
				/* Get 1 bit value */
				for (i=13,j=1; i<15; i++,j++)
				{
					byVal	= (Byte)((u16Val & (0x0001 << j)) >> j);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(i);
					drItem.SetLimit(byVal);/* off or on */
				}
				/* Get 1 bit value */
				byVal	= (Byte)((u16Val & (0x0001 << 3)) >> 3);
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(15);
				drItem.SetPosition(byVal);	/* pos or none */
			}
			catch (Exception ex)
			{
				System.Diagnostics.Debug.WriteLine($"Exception : {ex}");
				throw new Exception();
			}
			finally
			{
			}
		}

		/*
		 desc : Load the normal value in the 1st word of the AI memory.
		 parm : add	- [in]  whether to add items (true : yes, false : no)
		 retn : None
		*/
		private void LoadAILiftPin(Boolean add)
		{
			UInt16 u16Val	= 0x0000;
			UInt32 u32Val	= 0x00000000;
			PLCData drItem	= null;

			try
			{
				/* replaces the new items */
				if (add)
				{
					/* Add new items - D1123.0 ~ D1131.F */
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS CURRNT POSITION DISPLAY", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS ALARM CODE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS CURRNT POSITION DISPLAY", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS ALARM CODE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS CURRNT POSITION DISPLAY", ""));
					m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS ALARM CODE", ""));
				}

				/* Update the current value (D1123 ~ D1124) */
				u32Val	= EngPLC.uvCmn_MCQ_GetDWordMemory(23);
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(0);
				drItem.SetValue((Double)(u32Val/10.0), 1, "um");
				/* Update the current value (D1125) */
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(1);
				u16Val	= EngPLC.uvCmn_MCQ_GetWordMemory(25);
				drItem.SetValue(u16Val);
				/* Update the current value (D1126 ~ D1127) */
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(2);
				u32Val	= EngPLC.uvCmn_MCQ_GetDWordMemory(26);
				drItem.SetValue((Double)(u32Val/10.0), 1, "um");
				/* Update the current value (D1128) */
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(3);
				u16Val	= EngPLC.uvCmn_MCQ_GetWordMemory(28);
				drItem.SetValue(u16Val);
				/* Update the current value (D1129 ~ D1130) */
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(4);
				u32Val	= EngPLC.uvCmn_MCQ_GetDWordMemory(29);
				drItem.SetValue((Double)(u32Val/10.0), 1, "um");
				/* Update the current value (D1131) */
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(5);
				u16Val	= EngPLC.uvCmn_MCQ_GetWordMemory(31);
				drItem.SetValue(u16Val);
			}
			catch (Exception ex)
			{
				System.Diagnostics.Debug.WriteLine($"Exception : {ex}");
				throw new Exception();
			}
			finally
			{
			}
		}

		/*
		 desc : Load the normal value in the 1st word of the DO memory.
		 parm : add	- [in]  whether to add items (true : yes, false : no)
		 retn : None
		*/
		private void LoadDONorm(Boolean add)
		{
			Byte i, j, byVal= 0x00;
			UInt16 u16Val	= 0x0000;
			PLCData drItem	= null;

			try
			{
				/* replaces the new items */
				if (add)
				{
					/* Add new items - D1150.0 ~ D1150.F */
					m_grid_plc_io.Items.Add(new PLCData(" SAFETY RESET CMD", ""));
					m_grid_plc_io.Items.Add(new PLCData(" TOWER GREEN LAMP CMD", ""));
					m_grid_plc_io.Items.Add(new PLCData(" TOWER YELLOW LAMP FLASHING CMD", ""));
					m_grid_plc_io.Items.Add(new PLCData(" TOWER RED LAMP FLASHING CMD", ""));
					m_grid_plc_io.Items.Add(new PLCData(" TOWER BUZZER 1 CMD", ""));
					m_grid_plc_io.Items.Add(new PLCData(" Z1 AXIS AIR COOLING ON/OFF", ""));
					m_grid_plc_io.Items.Add(new PLCData(" Z2 AXIS AIR COOLING ON/OFF", ""));
					m_grid_plc_io.Items.Add(new PLCData(" Z3 AXIS AIR COOLING ON/OFF", ""));
					m_grid_plc_io.Items.Add(new PLCData(" Z1 AXIS CYLINDER ON/OFF", ""));
					m_grid_plc_io.Items.Add(new PLCData(" Z2 AXIS CYLINDER ON/OFF", ""));
					m_grid_plc_io.Items.Add(new PLCData(" Z3 AXIS CYLINDER ON/OFF", ""));
					m_grid_plc_io.Items.Add(new PLCData(" WAFER CHUCK VACUUM ON/OFF", ""));
					m_grid_plc_io.Items.Add(new PLCData(" WAFER CHUCK EXHAUST ON/OFF", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT VACUUMUUM ON/OFF", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ALIGNER VACUUM ON/OFF ", ""));
					m_grid_plc_io.Items.Add(new PLCData(" PHOTOHEAD 1 POWER OFF CMD", ""));
					/* Add new items - D1151.0 ~ D1151.F */
					m_grid_plc_io.Items.Add(new PLCData(" PHOTOHEAD 2 POWER OFF CMD", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ILLUMINATION SENSOR PWR ON/OFF", ""));
					m_grid_plc_io.Items.Add(new PLCData(" HALOGEN LING LIGHT POWER ON", ""));
					m_grid_plc_io.Items.Add(new PLCData(" HALOGEN COAXIAL LIGHT POWER ON", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT INPUT SIGNAL 1", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT INPUT SIGNAL 2", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT INPUT SIGNAL 3", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT INPUT SIGNAL 4", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT INPUT SIGNAL 5", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT INPUT SIGNAL 6", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT INPUT SIGNAL 7", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT INPUT SIGNAL 8", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT INPUT SIGNAL 9", ""));
					m_grid_plc_io.Items.Add(new PLCData(" ROBOT INPUT SIGNAL 10", ""));
					/* Add new items - D1152.0 ~ D1152.F */
					m_grid_plc_io.Items.Add(new PLCData(" XY STAGE MOVE PROHIBIT", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
				}

				/* Update the current value (D1150.0 ~ D1150.F) */
				u16Val	= EngPLC.uvCmn_MCQ_GetWordMemory(50);
				for (i=0,j=0; i<16; i++,j++)
				{
					/* Get 1 bit value */
					byVal	= (Byte)((u16Val & (0x0001 << j)) >> j);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(i);
					switch (i)
					{
					case 0:		drItem.SetResetCmd(byVal);			break;	/* reset or off */
					case 15:	byVal ^= 1;	drItem.SetPower(byVal);	break;	/* off or on */
					default:	drItem.SetPower(byVal);				break;	/* on or off */
					}
				}
				/* Update the current value (D1151.0 ~ D1151.F) */
				u16Val	= EngPLC.uvCmn_MCQ_GetWordMemory(51);
				for (i=16,j=0; i<32; i++,j++)
				{
					if (j == 4 || j == 5)	continue;
					/* Get 1 bit value */
					byVal	= (Byte)((u16Val & (0x0001 << j)) >> j);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(i);
					switch (i)
					{
					case 0:		byVal ^= 1;	drItem.SetPower(byVal);	break;	/* off or on */
					case 15:	drItem.SetInvert(byVal);			break;	/* on/off or none */
					default:	drItem.SetPower(byVal);				break;	/* on or off */
					}
				}
				/* Update the current value (D1152.0 ~ D1152.F) */
				u16Val	= EngPLC.uvCmn_MCQ_GetWordMemory(52);
				/* Get 1 bit value */
				byVal	= (Byte)((u16Val & (0x0001 << 0)) >> 0);
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(32);
				drItem.SetStopNone(byVal);	/* normal or alarm */
			}
			catch (Exception ex)
			{
				System.Diagnostics.Debug.WriteLine($"Exception : {ex}");
				throw new Exception();
			}
			finally
			{
			}
		}

		/*
		 desc : Load the normal value in the 1st word of the D0 memory.
		 parm : add	- [in]  whether to add items (true : yes, false : no)
		 retn : None
		*/
		private void LoadDOExpoToTrack(Boolean add)
		{
			Byte i, j, byVal= 0x00;
			UInt16 u16Val	= 0x0000;
			PLCData drItem	= null;

			try
			{
				/* replaces the new items */
				if (add)
				{
					/* Add new items - D1153.0 ~ D1153.F */
					m_grid_plc_io.Items.Add(new PLCData(" EXP-INK (EXPOSURE INLINE)", ""));
					m_grid_plc_io.Items.Add(new PLCData(" EXP-REY (EXPOSURE REJECT)", ""));
					m_grid_plc_io.Items.Add(new PLCData(" EXP-SND (EXPOSURE SEND TO READY)", ""));
					m_grid_plc_io.Items.Add(new PLCData(" EXP-RDY (EXPOSURE RECEIVE TO READY)", ""));
					m_grid_plc_io.Items.Add(new PLCData(" EXP-RSERVE 1", ""));
					m_grid_plc_io.Items.Add(new PLCData(" EXP-RSERVE 2", ""));
					m_grid_plc_io.Items.Add(new PLCData(" EXP-RSERVE 3", ""));
					m_grid_plc_io.Items.Add(new PLCData(" EXP-RSERVE 4", ""));
					m_grid_plc_io.Items.Add(new PLCData(" EXP-RSERVE 5", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
				}

				/* Update the current value (D1153.0 ~ D1153.F) */
				u16Val	= EngPLC.uvCmn_MCQ_GetWordMemory(53);
				for (i=0,j=0; i<16; i++,j++)
				{
					/* Get 1 bit value */
					byVal	= (Byte)((u16Val & (0x0001 << j)) >> j);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(i);
					if (i < 9)	drItem.SetPower(byVal);
				}
			}
			catch (Exception ex)
			{
				System.Diagnostics.Debug.WriteLine($"Exception : {ex}");
				throw new Exception();
			}
			finally
			{
			}
		}

		/*
		 desc : Load the normal value in the 1st word of the AO memory.
		 parm : add	- [in]  whether to add items (true : yes, false : no)
		 retn : None
		*/
		private void LoadAONorm(Boolean add)
		{
			UInt16 u16Val	= 0x0000;
			PLCData drItem	= null;

			try
			{
				/* replaces the new items */
				if (add)
				{
					/* Add new items - D1154.0 ~ D1156.F */
					m_grid_plc_io.Items.Add(new PLCData(" HALOGEN LING LIGHT POWER", ""));
					m_grid_plc_io.Items.Add(new PLCData(" HALOGEN COAXIAL LIGHT POWER", ""));
					m_grid_plc_io.Items.Add(new PLCData(" PC LIVE CHECK", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
					m_grid_plc_io.Items.Add(new PLCData(" SPARE", ""));
				}

				/* Update the current value (D1154) */
				u16Val	= EngPLC.uvCmn_MCQ_GetWordMemory(54);
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(0);
				drItem.SetValue((Double)(u16Val*0.12 /* 1% = 0.12V*/), 2, "V");
				/* Update the current value (D1155) */
				u16Val	= EngPLC.uvCmn_MCQ_GetWordMemory(55);
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(1);
				drItem.SetValue((Double)(u16Val*0.12 /* 1% = 0.12V*/), 2, "V");
				/* Update the current value (D1156) */
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(2);
				u16Val	= EngPLC.uvCmn_MCQ_GetWordMemory(56);
				drItem.SetValue(u16Val);
			}
			catch (Exception ex)
			{
				System.Diagnostics.Debug.WriteLine($"Exception : {ex}");
				throw new Exception();
			}
			finally
			{
			}
		}

		/*
		 desc : Load the normal value in the 1st word of the DO memory.
		 parm : add	- [in]  whether to add items (true : yes, false : no)
		 retn : None
		*/
		private void LoadDOLiftPin(Boolean add)
		{
			Byte i, byVal	= 0x00;
			UInt16[] u16Val	= new UInt16[3] { 0x0000, 0x0000, 0x0000 };
			PLCData drItem	= null;

			try
			{
				/* replaces the new items */
				if (add)
				{
					/* Add new items - D1160.0 ~ D1162.F */
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS ENABLE DISABLE", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS RESET", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS STOP", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS JOG + MOVE", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS JOG - MOVE", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS HOME POSITION", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS POSITION", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS DREVE ALARM RESET", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 

					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS ENABLE DISABLE", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS RESET", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS STOP", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS JOG + MOVE", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS JOG - MOVE", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS HOME POSITION", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS POSITION", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS DRIVE ALARM RESET", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 

					m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS ENABLE DISABLE", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS RESET", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS STOP", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS JOG + MOVE", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS JOG - MOVE", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS HOME POSITION", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS POSITION", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
					m_grid_plc_io.Items.Add(new PLCData(" Spare", "")); 
				}

				/* Update the current value (D1160.0 ~ D1162.F) */
				u16Val[0]	= EngPLC.uvCmn_MCQ_GetWordMemory(60);
				u16Val[1]	= EngPLC.uvCmn_MCQ_GetWordMemory(61);
				u16Val[2]	= EngPLC.uvCmn_MCQ_GetWordMemory(62);
				for (i=0x00; i<0x03; i++)
				{
					/* Lift Pin X Axis Enable or Disable */
					byVal	= (Byte)((u16Val[i] & (0x0001 << 0)) >> 0);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(0+i*16);
					drItem.SetEnable(byVal);
					/* Lift Pin X Axis Reset */
					byVal	= (Byte)((u16Val[i] & (0x0001 << 1)) >> 1);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(1+i*16);
					drItem.SetResetCmd(byVal);
					/* Lift Pin X Axis Stop */
					byVal	= (Byte)((u16Val[i] & (0x0001 << 2)) >> 2);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(2+i*16);
					drItem.SetStopOff(byVal);

					/* Lift Pin [?] Axis Jog + MOVE */
					byVal	= (Byte)((u16Val[i] & (0x0001 << 4)) >> 4);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(4+i*16);
					drItem.SetMove(byVal, 0x00);
					/* Lift Pin [?] Axis Jog + MOVE */
					byVal	= (Byte)((u16Val[i] & (0x0001 << 5)) >> 5);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(5+i*16);
					drItem.SetMove(byVal, 0x01);
					/* Lift Pin [?] Axis Home Position */
					byVal	= (Byte)((u16Val[i] & (0x0001 << 6)) >> 6);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(6+i*16);
					drItem.SetHome(byVal);
					/* Lift Pin [?] Axis Position */
					byVal	= (Byte)((u16Val[i] & (0x0001 << 7)) >> 7);
					drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(7+i*16);
					drItem.SetPosition(byVal);
					if (i != 3)
					{
						/* Lift Pin [?] Axis Drive Alarm Reset */
						byVal	= (Byte)((u16Val[i] & (0x0001 << 8)) >> 8);
						drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(8+i*16);
						drItem.SetResetCmd(byVal);
					}
				}
			}
			catch (Exception ex)
			{
				System.Diagnostics.Debug.WriteLine($"Exception : {ex}");
				throw new Exception();
			}
			finally
			{
			}
		}

		/*
		 desc : Load the normal value in the 1st word of the AO memory.
		 parm : add	- [in]  whether to add items (true : yes, false : no)
		 retn : None
		*/
		private void LoadAOLiftPin(Boolean add)
		{
			UInt32 u32Val	= 0x00000000;
			PLCData drItem	= null;

			try
			{
				/* replaces the new items */
				if (add)
				{
					/* Add new items - D1109.0 ~ D1109.F */
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS JOG SPEED SETTING", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS POSITION SPEED SETTING", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN X AXIS POSITION SETTING", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS JOG SPEED SETTING", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS POSITION SPEED SETTING", ""));
					m_grid_plc_io.Items.Add(new PLCData(" LIFT PIN Z AXIS POSITION SETTING", ""));
					m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS JOG SPEED SETTING", ""));
					m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS POSITION SPEED SETTING", ""));
					m_grid_plc_io.Items.Add(new PLCData(" BSA CAMERA Z AXIS POSITION SETTING", ""));
				}

				/* Update the current value (D1163 ~ D1164) */
				u32Val	= EngPLC.uvCmn_MCQ_GetDWordMemory(63);
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(0);
				drItem.SetValue((Double)(u32Val/10.0), 1, "mm/min");
				/* Update the current value (D1165 ~ D1166) */
				u32Val	= EngPLC.uvCmn_MCQ_GetDWordMemory(65);
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(1);
				drItem.SetValue((Double)(u32Val/10.0), 1, "mm/min");
				/* Update the current value (D1167 ~ D1168) */
				u32Val	= EngPLC.uvCmn_MCQ_GetDWordMemory(67);
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(2);
				drItem.SetValue((Double)(u32Val/10.0), 1, "um");
				/* Update the current value (D1169 ~ D1170) */
				u32Val	= EngPLC.uvCmn_MCQ_GetDWordMemory(69);
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(3);
				drItem.SetValue((Double)(u32Val/10.0), 1, "mm/min");
				/* Update the current value (D1171 ~ D1172) */
				u32Val	= EngPLC.uvCmn_MCQ_GetDWordMemory(71);
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(4);
				drItem.SetValue((Double)(u32Val/10.0), 1, "mm/min");
				/* Update the current value (D1173 ~ D1174) */
				u32Val	= EngPLC.uvCmn_MCQ_GetDWordMemory(73);
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(5);
				drItem.SetValue((Double)(u32Val/10.0), 1, "um");
				/* Update the current value (D1175 ~ D1166) */
				u32Val	= EngPLC.uvCmn_MCQ_GetDWordMemory(75);
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(6);
				drItem.SetValue((Double)(u32Val/10.0), 1, "mm/min");
				/* Update the current value (D1177 ~ D1178) */
				u32Val	= EngPLC.uvCmn_MCQ_GetDWordMemory(77);
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(7);
				drItem.SetValue((Double)(u32Val/10.0), 1, "mm/min");
				/* Update the current value (D1179 ~ D1180) */
				u32Val	= EngPLC.uvCmn_MCQ_GetDWordMemory(79);
				drItem	= (PLCData)m_grid_plc_io.Items.GetItemAt(8);
				drItem.SetValue((Double)(u32Val/10.0), 1, "um");
			}
			catch (Exception ex)
			{
				System.Diagnostics.Debug.WriteLine($"Exception : {ex}");
				throw new Exception();
			}
			finally
			{
			}
		}
#endif

		/*
		 desc : Update PLC Input
		 parm : None
		 retn : None
		*/
		private void UpdateCheckIO()
		{
			m_chk_plc_safety_reset_cmd.IsChecked					= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_safety_reset_possible_status)==1 ? true : false;
			m_chk_plc_tower_green_lamp_cmd.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_signal_tower_green_lamp) == 1 ? true : false;
			m_chk_plc_tower_yellow_lamp_cmd.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_signal_tower_yellow_lamp_flashing) == 1 ? true : false;
			m_chk_plc_tower_red_lamp_cmd.IsChecked					= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_signal_tower_red_lamp_flashing) == 1 ? true : false;
			m_chk_plc_tower_buzzer_1_cmd.IsChecked					= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_signal_tower_buzzer_1) == 1 ? true : false;
			m_chk_plc_z1_axis_air_cooling_on_off.IsChecked			= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_z1_axis_air_cooling_on_off) == 1 ? true : false;
			m_chk_plc_z2_axis_air_cooling_on_off.IsChecked			= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_z2_axis_air_cooling_on_off) == 1 ? true : false;
			m_chk_plc_z3_axis_air_cooling_on_off.IsChecked			= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_z3_axis_air_cooling_on_off) == 1 ? true : false;
			m_chk_plc_z1_axis_cylinder_on_off.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_z1_axis_cylinder_on_off) == 1 ? true : false;
			m_chk_plc_z2_axis_cylinder_on_off.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_z2_axis_cylinder_on_off) == 1 ? true : false;
			m_chk_plc_z3_axis_cylinder_on_off.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_z3_axis_cylinder_on_off) == 1 ? true : false;
			m_chk_plc_wafer_chuck_vacuum_on_off.IsChecked			= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_wafer_chuck_vacuum_on_off) == 1 ? true : false;
			m_chk_plc_wafer_chuck_exhaust_on_off.IsChecked			= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_wafer_chuck_exhaust_on_off) == 1 ? true : false;
			m_chk_plc_robot_vacuumuum_on_off.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_robot_vacuumuum_on_off) == 1 ? true : false;
			m_chk_plc_aligner_vacuum_on_off.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_aligner_vacuum_on_off) == 1 ? true : false;
			m_chk_plc_photohead_1_power_off_cmd.IsChecked			= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_photohead_1_power_off_cmd) == 1 ? true : false;
			m_chk_plc_photohead_2_power_off_cmd.IsChecked			= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_photohead_2_power_off_cmd) == 1 ? true : false;
			m_chk_plc_illumination_sensor_pwr_on_off.IsChecked		= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_illumination_sensor_pwr_on_off) == 1 ? true : false;
			m_chk_plc_halogen_ring_light_power_on.IsChecked			= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_halogen_ring_light_power_on) == 1 ? true : false;
			m_chk_plc_halogen_coaxial_light_power_on.IsChecked		= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_halogen_coaxial_light_power_on) == 1 ? true : false;
			m_chk_plc_robot_input_signal_01.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_robot_input_signal_01) == 1 ? true : false;
			m_chk_plc_robot_input_signal_02.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_robot_input_signal_02) == 1 ? true : false;
			m_chk_plc_robot_input_signal_03.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_robot_input_signal_03) == 1 ? true : false;
			m_chk_plc_robot_input_signal_04.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_robot_input_signal_04) == 1 ? true : false;
			m_chk_plc_robot_input_signal_05.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_robot_input_signal_05) == 1 ? true : false;
			m_chk_plc_robot_input_signal_06.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_robot_input_signal_06) == 1 ? true : false;
			m_chk_plc_robot_input_signal_07.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_robot_input_signal_07) == 1 ? true : false;
			m_chk_plc_robot_input_signal_08.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_robot_input_signal_08) == 1 ? true : false;
			m_chk_plc_robot_input_signal_09.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_robot_input_signal_09) == 1 ? true : false;
			m_chk_plc_robot_input_signal_10.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_robot_input_signal_10) == 1 ? true : false;
			m_chk_plc_xy_stage_move_prohibit.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_xy_stage_move_prohibit) == 1 ? true : false;
			m_chk_plc_xy_stage_move_prohibit_release.IsChecked		= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_xy_stage_move_prohibit_release) == 1 ? true : false;
			m_chk_plc_exp_ink_exposure_inline.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_exp_ink_exposure_inline) == 1 ? true : false;
			m_chk_plc_exp_rej_exposure_reject.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_exp_rej_exposure_reject) == 1 ? true : false;
			m_chk_plc_exp_snd_exposure_send_to_ready.IsChecked		= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_exp_snd_exposure_send_to_ready) == 1 ? true : false;
			m_chk_plc_exp_rdy_exposure_receive_to_ready.IsChecked	= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_exp_rdy_exposure_receive_to_ready) == 1 ? true : false;
			m_chk_plc_exp_rserve_1.IsChecked						= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_exp_reserve_1) == 1 ? true : false;
			m_chk_plc_exp_rserve_2.IsChecked						= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_exp_reserve_2) == 1 ? true : false;
			m_chk_plc_exp_rserve_3.IsChecked						= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_exp_reserve_3) == 1 ? true : false;
			m_chk_plc_exp_rserve_4.IsChecked						= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_exp_reserve_4) == 1 ? true : false;
			m_chk_plc_exp_rserve_5.IsChecked						= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_exp_reserve_5) == 1 ? true : false;
			m_chk_plc_lift_pin_x_axis_enable_disable.IsChecked		= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_lift_pin_xo_axis_enable_disable) == 1 ? true : false;
			m_chk_plc_lift_pin_x_axis_reset.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_lift_pin_xo_axis_reset) == 1 ? true : false;
			m_chk_plc_lift_pin_x_axis_stop.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_lift_pin_xo_axis_stop) == 1 ? true : false;
			m_chk_plc_lift_pin_x_axis_jog_plus_move.IsChecked		= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_lift_pin_xo_axis_jog_plus_move) == 1 ? true : false;
			m_chk_plc_lift_pin_x_axis_jog_minus_move.IsChecked		= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_lift_pin_xo_axis_jog_minus_move) == 1 ? true : false;
			m_chk_plc_lift_pin_x_axis_home_position.IsChecked		= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_lift_pin_xo_axis_home_position) == 1 ? true : false;
			m_chk_plc_lift_pin_x_axis_position.IsChecked			= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_lift_pin_xo_axis_position) == 1 ? true : false;
			m_chk_plc_lift_pin_x_axis_drive_alarm_reset.IsChecked	= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_lift_pin_xo_axis_drive_alarm_reset) == 1 ? true : false;
			m_chk_plc_lift_pin_z_axis_enable_disable.IsChecked		= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_lift_pin_zo_axis_enable_disable) == 1 ? true : false;
			m_chk_plc_lift_pin_z_axis_reset.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_lift_pin_zo_axis_reset) == 1 ? true : false;
			m_chk_plc_lift_pin_z_axis_stop.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_lift_pin_zo_axis_stop) == 1 ? true : false;
			m_chk_plc_lift_pin_z_axis_jog_plus_move.IsChecked		= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_lift_pin_zo_axis_jog_plus_move) == 1 ? true : false;
			m_chk_plc_lift_pin_z_axis_jog_minus_move.IsChecked		= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_lift_pin_zo_axis_jog_minus_move) == 1 ? true : false;
			m_chk_plc_lift_pin_z_axis_home_position.IsChecked		= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_lift_pin_zo_axis_home_position) == 1 ? true : false;
			m_chk_plc_lift_pin_z_axis_position.IsChecked			= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_lift_pin_zo_axis_position) == 1 ? true : false;
			m_chk_plc_lift_pin_z_axis_drive_alarm_reset.IsChecked	= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_lift_pin_zo_axis_dreve_alarm_reset) == 1 ? true : false;
			m_chk_plc_bsa_camera_z_axis_enable_disable.IsChecked	= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_bsa_camera_zo_axis_enable_disable) == 1 ? true : false;
			m_chk_plc_bsa_camera_z_axis_reset.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_bsa_camera_zo_axis_reset) == 1 ? true : false;
			m_chk_plc_bsa_camera_z_axis_stop.IsChecked				= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_bsa_camera_zo_axis_stop) == 1 ? true : false;
			m_chk_plc_bsa_camera_z_axis_jog_plus_move.IsChecked		= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_bsa_camera_zo_axis_jog_plus_move) == 1 ? true : false;
			m_chk_plc_bsa_camera_z_axis_jog_minus_move.IsChecked	= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_bsa_camera_zo_axis_jog_minus_move) == 1 ? true : false;
			m_chk_plc_bsa_camera_z_axis_home_position.IsChecked		= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_bsa_camera_zo_axis_home_position) == 1 ? true : false;
			m_chk_plc_bsa_camera_z_axis_position.IsChecked			= EngPLC.uvCmn_MCQ_GetBitsValueEx(ENG_PIOA.en_bsa_camera_zo_axis_position) == 1 ? true : false;
		}
	}
}
