using System;
using System.Collections.Generic;
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

using TGen2i.Enum.MC2;
using TGen2i.EngineLib;

namespace TGen2i.MenuChild
{
	/* MC2 Information (Item, Value) */
	public class MC2Data
	{
		public MC2Data(string item, string value)
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
		public void SetValue(Int32 value)
		{
			m_strValue	= String.Format("{0:0}", value);
		}
		public void SetValue(UInt32 value)
		{
			m_strValue	= String.Format("{0:0}", value);
		}
		public void SetValue(UInt64 value)
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
			case 0x0001	: strFmt = "{0:N1} {1}";	break;
			case 0x0002	: strFmt = "{0:N2} {1}";	break;
			case 0x0003	: strFmt = "{0:N3} {1}";	break;
			case 0x0004	: strFmt = "{0:N4} {1}";	break;
			default		: strFmt = "{0:N5} {1}";	break;
			}
			m_strValue	= String.Format(strFmt, value, unit);
		}
	}

	public partial class MenuMC2 : MenuBase
	{
		/*
		 desc : Constructor
		 parm : None
		 retn : None
		*/
		public MenuMC2()
		{
			/* Initialize internal components. */
			InitializeComponent();

			/* Window Event */

			/* Control Event for Button */
			this.m_btn_mc2_stage_up.Click				+= btnClickStageMoving;
			this.m_btn_mc2_stage_down.Click				+= btnClickStageMoving;
			this.m_btn_mc2_stage_left.Click				+= btnClickStageMoving;
			this.m_btn_mc2_stage_right.Click			+= btnClickStageMoving;
			this.m_btn_mc2_stage_stop.Click				+= btnClickStageMoving;

			this.m_btn_mc2_ph_up.Click					+= btnClickPHMoving;
			this.m_btn_mc2_ph_down.Click				+= btnClickPHMoving;
			this.m_btn_mc2_ph_stop.Click				+= btnClickPHMoving;

			this.m_btn_mc2_homing_all.Click				+= btnClickMC2Homing;
			this.m_btn_mc2_homing_stage_x.Click			+= btnClickMC2Homing;
			this.m_btn_mc2_homing_stage_y.Click			+= btnClickMC2Homing;
			this.m_btn_mc2_homing_ph1.Click				+= btnClickMC2Homing;
			this.m_btn_mc2_homing_ph2.Click				+= btnClickMC2Homing;
			this.m_btn_mc2_homing_tsa_z.Click			+= btnClickMC2Homing;

			this.m_btn_mc2_en_stop_yes.Click			+= btnClickMC2EnStop;
			this.m_btn_mc2_en_stop_no.Click				+= btnClickMC2EnStop;

			this.m_btn_mc2_reset_drive.Click			+= btnClickMC2Etc;
			this.m_btn_mc2_reset_all.Click				+= btnClickMC2Etc;

			this.m_cmb_mc2_drive_no.SelectionChanged	+= SelectionChangedMC2Drive;
			this.m_cmb_mc2_ph_no.SelectionChanged		+= SelectionChangedPHDrive;

			/* Control Event for Checkbox */
			this.m_chk_mc2_servo_axis_x.Click			+= chkClickServo;
			this.m_chk_mc2_servo_axis_y.Click			+= chkClickServo;
			this.m_chk_mc2_servo_axis_ph1.Click			+= chkClickServo;
			this.m_chk_mc2_servo_axis_ph2.Click			+= chkClickServo;
			this.m_chk_mc2_servo_axis_ph3.Click			+= chkClickServo;
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

			/* Update Control's State */
			UpdateControlState();

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
		 desc : Combobox selection event for MC2 Drive
		 parm : s	- [in]  Object
				  e	- [in]  EventArgs
		 retn : None
		*/
		private void SelectionChangedMC2Drive(object s, SelectionChangedEventArgs e)
		{
			/* Get the text and index of the selected item */
			string strDrvName	= (s as ComboBox).SelectedItem as string;
			Int32 i32Index		= (s as ComboBox).SelectedIndex;
		}

		/*
		 desc : Combobox selection event for PH Drive
		 parm : s	- [in]  Object
				  e	- [in]  EventArgs
		 retn : None
		*/
		private void SelectionChangedPHDrive(object s, SelectionChangedEventArgs e)
		{
			/* Get the text and index of the selected item */
			string strDrvName	= (s as ComboBox).SelectedItem as string;
			Int32 i32Index		= (s as ComboBox).SelectedIndex;
		}

		/*
		 desc : Button event for Stage Moving Direction
		 parm : s	- [in]  Object
				  e	- [in]  EventArgs
		 retn : None
		*/
		private void btnClickStageMoving(object s, RoutedEventArgs e)
		{
			Double dbVelo, dbDist, dbDirect = 1.0 /* plus direct */, dbMove = 0.0 /* Current Position */;
			ENG_MMDI enDrvID		= ENG_MMDI.en_axis_none;
			FrameworkElement feID	= e.Source as FrameworkElement;

			/* In case of a command to stop motion */
			if (feID.Name == "m_btn_mc2_stage_stop")
			{
				EngMC2.uvEng_MC2_SendDevStopped(ENG_MMDI.en_stage_x_master);
				EngMC2.uvEng_MC2_SendDevStopped(ENG_MMDI.en_stage_y_master);
				return;
			}

			/* Get the current motion speed and distance (mm) */
			dbVelo	= Convert.ToDouble(m_box_mc2_move_speed.Text);	/* mm */
			dbDist	= Convert.ToDouble(m_box_mc2_move_dist.Text);	/* mm */

			/* Validate input value */
			if (dbVelo < 1 /* 1 mm/sec */ || dbDist < 0.001 /* 1 um */)	return;

			/* Select drive_id */
			switch (feID.Name)
			{
			case "m_btn_mc2_stage_left"	:	enDrvID	= ENG_MMDI.en_stage_y_master;	break;
			case "m_btn_mc2_stage_right":	enDrvID	= ENG_MMDI.en_stage_y_master;	break;
			case "m_btn_mc2_stage_up"	:	enDrvID	= ENG_MMDI.en_stage_x_master;	break;
			case "m_btn_mc2_stage_down"	:	enDrvID	= ENG_MMDI.en_stage_x_master;	break;
			}
			/* Select a direction */
			switch (feID.Name)
			{
			case "m_btn_mc2_stage_left"	:	dbDirect= +1.0;	break;
			case "m_btn_mc2_stage_right":	dbDirect= -1.0;	break;
			case "m_btn_mc2_stage_up"	:	dbDirect= -1.0;	break;
			case "m_btn_mc2_stage_down"	:	dbDirect= +1.0;	break;
			}

			/* Get the current position of motion drive */
			dbMove	= EngMC2.uvCmn_MC2_GetDrvAbsPos(enDrvID);
			/* Set the moving position for motion drive */
			dbMove	+= (dbDirect * dbDist);
			if (false == EngMC2.uvEng_MC2_SendDevAbsMove(enDrvID, dbMove, dbVelo))
			{
				return;
			}
		}

		/*
		 desc : Button event for PH Moving Direction
		 parm : s	- [in]  Object
				  e	- [in]  EventArgs
		 retn : None
		*/
		private void btnClickPHMoving(object s, RoutedEventArgs e)
		{
			Double dbVelo, dbDist, dbDirect = 1.0 /* plus direct */, dbMove = 0.0 /* Current Position */;
			ENG_MMDI enDrvID		= ENG_MMDI.en_axis_ph1;
			FrameworkElement feID	= e.Source as FrameworkElement;

			/* In case of a command to stop motion */
			if (feID.Name == "m_btn_mc2_ph_stop")
			{
				EngMC2.uvEng_MC2_SendDevStopped(ENG_MMDI.en_axis_ph1);
				EngMC2.uvEng_MC2_SendDevStopped(ENG_MMDI.en_axis_ph2);
				return;
			}

			/* Get the current motion speed and distance (mm) */
			dbVelo	= Convert.ToDouble(m_box_mc2_move_speed.Text);	/* mm */
			dbDist	= Convert.ToDouble(m_box_mc2_move_dist.Text);	/* mm */

			/* Get selected drive number */
			switch (m_cmb_mc2_ph_no.SelectedIndex)
			{
			case 0	:	enDrvID	= ENG_MMDI.en_axis_ph1;	break;
			case 1	:	enDrvID	= ENG_MMDI.en_axis_ph2;	break;
			case 2	:	enDrvID	= ENG_MMDI.en_axis_ph3;	break;
			}

			/* Select a direction */
			switch (feID.Name)
			{
			case "m_btn_mc2_ph_up"	:	dbDirect= +1.0;	break;
			case "m_btn_mc2_ph_down":	dbDirect= -1.0;	break;
			}

			/* Get the current position of motion drive */
			dbMove	= EngMC2.uvCmn_MC2_GetDrvAbsPos(enDrvID);
			/* Set the moving position for motion drive */
			dbMove	+= (dbDirect * dbDist);
			if (false == EngMC2.uvEng_MC2_SendDevAbsMove(enDrvID, dbMove, dbVelo))
			{
				return;
			}
		}

		/*
		 desc : Button event for MC2 Homing Operation
		 parm : s	- [in]  Object
				  e	- [in]  EventArgs
		 retn : None
		*/
		private void btnClickMC2Homing(object s, RoutedEventArgs e)
		{
			ENG_MMDI enDrvID		= ENG_MMDI.en_stage_x_master;
			FrameworkElement feID	= e.Source as FrameworkElement;

			/* In case of a command to stop motion */
			if (feID.Name == "m_btn_mc2_homing_all")
				//EngMC2.uvEng_MC2_SendDevHomingAll();
				EngMC2.uvEng_MC2_SendDevHomingAllEx();
			else
			{
				switch (feID.Name)
				{
				case "m_btn_mc2_homing_stage_x"	:	enDrvID = ENG_MMDI.en_stage_x_master;	break;
				case "m_btn_mc2_homing_stage_y"	:	enDrvID = ENG_MMDI.en_stage_y_master;	break;
				case "m_btn_mc2_homing_ph1"		:	enDrvID = ENG_MMDI.en_axis_ph1;			break;
				case "m_btn_mc2_homing_ph2"		:	enDrvID = ENG_MMDI.en_axis_ph2;			break;
				case "m_btn_mc2_homing_tsa_z"	:	enDrvID = ENG_MMDI.en_axis_tsa_z;		break;
				}
				EngMC2.uvEng_MC2_SendDevHoming(enDrvID);
			}
		}

		/*
		 desc : Button event for MC2 EN_STOP Operation
		 parm : s	- [in]  Object
				  e	- [in]  EventArgs
		 retn : None
		*/
		private void btnClickMC2EnStop(object s, RoutedEventArgs e)
		{
			FrameworkElement feID	= e.Source as FrameworkElement;
			Byte u8HeartBeat = EngMC2.uvCmn_MC2_GetRefHeartBeat() == (Byte)0x01 ? (Byte)0x00 : (Byte)0x01;

			/* In case of a command to stop motion */
			if (feID.Name == "m_btn_mc2_en_stop_yes")	EngMC2.uvEng_MC2_SetRefHeadEnStop(ENG_MMST.en_stop_yes, u8HeartBeat);
			else										EngMC2.uvEng_MC2_SetRefHeadEnStop(ENG_MMST.en_stop_no, u8HeartBeat);
		}

		/*
		 desc : Button event for MC2 Etc Operation
		 parm : s	- [in]  Object
				e	- [in]  EventArgs
		 retn : None
		*/
		private void btnClickMC2Etc(object s, RoutedEventArgs e)
		{
			ENG_MMDI enDrv			= ENG_MMDI.en_stage_x_master;
			FrameworkElement feID	= e.Source as FrameworkElement;

			switch (m_cmb_mc2_drive_no.SelectedIndex)
			{
			case 0 : enDrv = ENG_MMDI.en_stage_x_master;	break;
			case 1 : enDrv = ENG_MMDI.en_stage_y_master;	break;
			case 2 : enDrv = ENG_MMDI.en_axis_ph1;			break;
			case 3 : enDrv = ENG_MMDI.en_axis_ph2;			break;
			case 4 : enDrv = ENG_MMDI.en_axis_ph3;			break;
			default:	return;
			}

			if (feID.Name == "m_btn_mc2_reset_drive")
			{
				EngMC2.uvEng_MC2_SendDevFaultReset(enDrv);
			}
			else if (feID.Name == "m_btn_mc2_reset_all")
			{
				EngMC2.uvEng_MC2_SendDevFaultResetAllEx();
			}
		}

		/*
		 desc : Checkbox event for MC2 Servo On/Off Operation
		 parm : s	- [in]  Object
				  e	- [in]  EventArgs
		 retn : None
		*/
		private void chkClickServo(object s, RoutedEventArgs e)
		{
			Boolean bLocked			= false;
			ENG_MMDI enDrv			= ENG_MMDI.en_stage_x_master;
			FrameworkElement feID	= e.Source as FrameworkElement;

			switch (feID.Name)
			{
			case "m_chk_mc2_servo_axis_x"	:	bLocked	= m_chk_mc2_servo_axis_x.IsChecked.Value;
												enDrv	= ENG_MMDI.en_stage_x_master;				break;
			case "m_chk_mc2_servo_axis_y"	:	bLocked	= m_chk_mc2_servo_axis_y.IsChecked.Value;
												enDrv	= ENG_MMDI.en_stage_y_master;				break;
			case "m_chk_mc2_servo_axis_ph1"	:	bLocked	= m_chk_mc2_servo_axis_ph1.IsChecked.Value;
												enDrv	= ENG_MMDI.en_axis_ph1;						break;
			case "m_chk_mc2_servo_axis_ph2"	:	bLocked	= m_chk_mc2_servo_axis_ph2.IsChecked.Value;
												enDrv	= ENG_MMDI.en_axis_ph2;						break;
			case "m_chk_mc2_servo_axis_ph3"	:	bLocked	= m_chk_mc2_servo_axis_ph3.IsChecked.Value;
												enDrv	= ENG_MMDI.en_axis_ph3;						break;
			default:	return;
			}

			/* Get the current servo state for drive */
			bLocked	= EngMC2.uvEng_MC2_SendDevLocked(enDrv, bLocked);
		}

		/* ------------------------------------------------------------------------------------- */
		/*                                  User Defined Function                                */
		/* ------------------------------------------------------------------------------------- */

		/*
		 desc : Inited control
		 parm : None
		 retn : None
		*/
		private void InitCtrl()
		{
			/* Add items to combobox for mc2 drive */
			m_cmb_mc2_drive_no.Items.Add("Stage X Master");
			m_cmb_mc2_drive_no.Items.Add("Stage Y Master");
			m_cmb_mc2_drive_no.Items.Add("PH Z 1");
			m_cmb_mc2_drive_no.Items.Add("PH Z 2");
			m_cmb_mc2_drive_no.Items.Add("PH Z 3");
			/* Set the Index */
			m_cmb_mc2_drive_no.SelectedIndex	= 0;

			/* Add items to combobox for ph number */
			m_cmb_mc2_ph_no.Items.Add("PH Z 1");
			m_cmb_mc2_ph_no.Items.Add("PH Z 2");
			m_cmb_mc2_ph_no.Items.Add("PH Z 3");
			/* Set the Index */
			m_cmb_mc2_ph_no.SelectedIndex	= 0;

			/* Set default values for velocity and distance of motion */
			m_box_mc2_move_speed.Text	= "200.0000";
			m_box_mc2_move_dist.Text	= "100.0000";
		}

		/*
		 desc : Set the data.grid to output the MC2 monitoring value.
		 parm : add	- [in]  whether to add items (true : yes, false : no)
		 retn : None
		*/
		private void SetDataGrid(Boolean add)
		{
			SetGrid_MotionDrivePosition(add);
			SetGrid_MotionDriveStatus(add);
			SetGrid_MotionRefHead(add);
			SetGrid_MotionActHead(add);
		}
		private void SetGrid_MotionDrivePosition(Boolean add)
		{
			UInt16 u16Error	= 0;
			Double dbPos	= 0.0d;
			MC2Data drItem	= null;

			try
			{
				if (add)
				{
					m_grid_mc2_drive_position.Items.Add(new MC2Data(" Stage X Master", ""));
					m_grid_mc2_drive_position.Items.Add(new MC2Data(" Stage Y Master", ""));
					m_grid_mc2_drive_position.Items.Add(new MC2Data(" PH 1", ""));
					m_grid_mc2_drive_position.Items.Add(new MC2Data(" PH 2", ""));
					m_grid_mc2_drive_position.Items.Add(new MC2Data(" PH 3", ""));

					m_grid_mc2_drive_position.Items.Add(new MC2Data(" X Error", ""));
					m_grid_mc2_drive_position.Items.Add(new MC2Data(" Y Error", ""));
					m_grid_mc2_drive_position.Items.Add(new MC2Data(" PH 1 Error", ""));
					m_grid_mc2_drive_position.Items.Add(new MC2Data(" PH 2 Error", ""));
					m_grid_mc2_drive_position.Items.Add(new MC2Data(" PH 3 Error", ""));
				}

				/* Updates the drive position (mm) - Stage X */
				dbPos	= EngMC2.uvCmn_MC2_GetDrvAbsPos(ENG_MMDI.en_stage_x_master);
				drItem	= (MC2Data)m_grid_mc2_drive_position.Items.GetItemAt(0);
				drItem.SetValue(dbPos, 4, "mm");
				/* Updates the drive position (mm) - Stage Y*/
				dbPos	= EngMC2.uvCmn_MC2_GetDrvAbsPos(ENG_MMDI.en_stage_y_master);
				drItem	= (MC2Data)m_grid_mc2_drive_position.Items.GetItemAt(1);
				drItem.SetValue(dbPos, 4, "mm");
				/* Updates the drive position (mm) - PH 1 */
				dbPos	= EngMC2.uvCmn_MC2_GetDrvAbsPos(ENG_MMDI.en_axis_ph1);
				drItem	= (MC2Data)m_grid_mc2_drive_position.Items.GetItemAt(2);
				drItem.SetValue(dbPos, 4, "mm");
				/* Updates the drive position (mm) - PH 2 */
				dbPos	= EngMC2.uvCmn_MC2_GetDrvAbsPos(ENG_MMDI.en_axis_ph2);
				drItem	= (MC2Data)m_grid_mc2_drive_position.Items.GetItemAt(3);
				drItem.SetValue(dbPos, 4, "mm");
				/* Updates the drive position (mm) - PH 3 */
				dbPos	= EngMC2.uvCmn_MC2_GetDrvAbsPos(ENG_MMDI.en_axis_ph3);
				drItem	= (MC2Data)m_grid_mc2_drive_position.Items.GetItemAt(4);
				drItem.SetValue(dbPos, 4, "mm");

				/* Error Code */
				u16Error= EngMC2.uvCmn_MC2_GetDriveError(ENG_MMDI.en_stage_x_master);
				drItem	= (MC2Data)m_grid_mc2_drive_position.Items.GetItemAt(5);
				drItem.SetValue(u16Error, "");
				u16Error= EngMC2.uvCmn_MC2_GetDriveError(ENG_MMDI.en_stage_y_master);
				drItem	= (MC2Data)m_grid_mc2_drive_position.Items.GetItemAt(6);
				drItem.SetValue(u16Error, "");
				u16Error= EngMC2.uvCmn_MC2_GetDriveError(ENG_MMDI.en_axis_ph1);
				drItem	= (MC2Data)m_grid_mc2_drive_position.Items.GetItemAt(7);
				drItem.SetValue(u16Error, "");
				u16Error= EngMC2.uvCmn_MC2_GetDriveError(ENG_MMDI.en_axis_ph2);
				drItem	= (MC2Data)m_grid_mc2_drive_position.Items.GetItemAt(8);
				drItem.SetValue(u16Error, "");
				u16Error= EngMC2.uvCmn_MC2_GetDriveError(ENG_MMDI.en_axis_ph3);
				drItem	= (MC2Data)m_grid_mc2_drive_position.Items.GetItemAt(9);
				drItem.SetValue(u16Error, "");

				/* Refresh the grid control */
				m_grid_mc2_drive_position.Items.Refresh();
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
		private void SetGrid_MotionDriveStatus(Boolean add)
		{
			Byte u8Flag		= 0x00, i;
			UInt16 u16Error	= 0x0000;
			Int32 i32Ret	= 0;
			Double dbPos	= 0.0d;
			ENG_MMDI enDrv	= ENG_MMDI.en_stage_x_master;
			MC2Data drItem	= null;

			switch (m_cmb_mc2_drive_no.SelectedIndex)
			{
			case 0 : enDrv = ENG_MMDI.en_stage_x_master;	break;
			case 1 : enDrv = ENG_MMDI.en_stage_y_master;	break;
			case 2 : enDrv = ENG_MMDI.en_axis_ph1;			break;
			case 3 : enDrv = ENG_MMDI.en_axis_ph2;			break;
			case 4 : enDrv = ENG_MMDI.en_axis_ph3;			break;
			}
			try
			{
				if (add)
				{
					m_grid_mc2_drive_status.Items.Add(new MC2Data(" command done", ""));
					m_grid_mc2_drive_status.Items.Add(new MC2Data(" drive calibrated", ""));
					m_grid_mc2_drive_status.Items.Add(new MC2Data(" power stage on", ""));
					m_grid_mc2_drive_status.Items.Add(new MC2Data(" error code", ""));
					m_grid_mc2_drive_status.Items.Add(new MC2Data(" message code", ""));
					m_grid_mc2_drive_status.Items.Add(new MC2Data(" command busy", ""));
					m_grid_mc2_drive_status.Items.Add(new MC2Data(" velocity zero", ""));
					m_grid_mc2_drive_status.Items.Add(new MC2Data(" target position reached", ""));
					m_grid_mc2_drive_status.Items.Add(new MC2Data(" target velocity reached", ""));
					m_grid_mc2_drive_status.Items.Add(new MC2Data(" drive ready", ""));
					m_grid_mc2_drive_status.Items.Add(new MC2Data(" done toggle", ""));
					m_grid_mc2_drive_status.Items.Add(new MC2Data(" error code", ""));
					m_grid_mc2_drive_status.Items.Add(new MC2Data(" actual position", ""));
					m_grid_mc2_drive_status.Items.Add(new MC2Data(" result1", ""));
					m_grid_mc2_drive_status.Items.Add(new MC2Data(" result2", ""));
				}

				/* Updates the drive status - command done ~ done toggle */
				for (i=0x00; i<0x0b; i++)
				{
					u8Flag	= EngMC2.uvCmn_MC2_GetDriveStatus(enDrv, (ENG_SADS)i);
					drItem	= (MC2Data)m_grid_mc2_drive_status.Items.GetItemAt(i);
					drItem.SetValue(u8Flag);
				}
				/* Updates the drive status - error code */
				u16Error= EngMC2.uvCmn_MC2_GetDriveError(enDrv);
				drItem	= (MC2Data)m_grid_mc2_drive_status.Items.GetItemAt(11);
				drItem.SetValue(u16Error);
				/* Updates the drive status - actual position */
				dbPos	= EngMC2.uvCmn_MC2_GetDrvAbsPos(enDrv);
				drItem	= (MC2Data)m_grid_mc2_drive_status.Items.GetItemAt(12);
				drItem.SetValue(dbPos, 4, "mm");
				/* Updates the drive status - result1 */
				i32Ret	= EngMC2.uvCmn_MC2_GetDriveResult(enDrv, 0x01);
				drItem	= (MC2Data)m_grid_mc2_drive_status.Items.GetItemAt(13);
				drItem.SetValue(i32Ret);
				/* Updates the drive status - result2 */
				i32Ret	= EngMC2.uvCmn_MC2_GetDriveResult(enDrv, 0x02);
				drItem	= (MC2Data)m_grid_mc2_drive_status.Items.GetItemAt(14);
				drItem.SetValue(i32Ret);

				/* Refresh the grid control */
				m_grid_mc2_drive_status.Items.Refresh();
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
		private void SetGrid_MotionRefHead(Boolean add)
		{
			Int32 i32Val	= 0;
			UInt64 u64Out	= 0;
			MC2Data drItem	= null;

			try
			{
				if (add)
				{
					m_grid_mc2_ref_head.Items.Add(new MC2Data(" MControl : EN_STOP", ""));
					m_grid_mc2_ref_head.Items.Add(new MC2Data(" MControl : HEART_BEAT", ""));
					m_grid_mc2_ref_head.Items.Add(new MC2Data(" V_SCALE", ""));
					m_grid_mc2_ref_head.Items.Add(new MC2Data(" OUTPUTS", ""));
				}

				/* Updates the MControl - EN_STOP (0 or 1) */
				i32Val	= EngMC2.uvCmn_MC2_IsDriveEnStop() == true ? 1 : 0;
				drItem	= (MC2Data)m_grid_mc2_ref_head.Items.GetItemAt(0);
				drItem.SetValue(i32Val);
				/* Updates the MControl - HEART_BEAT (0 or 1) */
				i32Val	= EngMC2.uvCmn_MC2_GetRefHeartBeat();
				drItem	= (MC2Data)m_grid_mc2_ref_head.Items.GetItemAt(1);
				drItem.SetValue(i32Val);
				/* Updates the MControl - V_SCALE */
				i32Val	= EngMC2.uvCmn_MC2_GetRefVScale();
				drItem	= (MC2Data)m_grid_mc2_ref_head.Items.GetItemAt(2);
				drItem.SetValue(i32Val, "%");
				/* Updates the MControl - OUTPUTS */
				u64Out	= EngMC2.uvCmn_MC2_GetRefOutputs();
				drItem	= (MC2Data)m_grid_mc2_ref_head.Items.GetItemAt(3);
				drItem.SetValue(u64Out);

				/* Refresh the grid control */
				m_grid_mc2_ref_head.Items.Refresh();
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
		private void SetGrid_MotionActHead(Boolean add)
		{
			Int32 i32Val	= 0;
			UInt64 u64Out	= 0;
			MC2Data drItem	= null;

			try
			{
				if (add)
				{
					m_grid_mc2_act_head.Items.Add(new MC2Data(" MState : HEART-BEAT1", ""));
					m_grid_mc2_act_head.Items.Add(new MC2Data(" MState : HEART-BEAT2", ""));
					m_grid_mc2_act_head.Items.Add(new MC2Data(" MError", ""));
					m_grid_mc2_act_head.Items.Add(new MC2Data(" INPUTS", ""));
					m_grid_mc2_act_head.Items.Add(new MC2Data(" OUTPUTS", ""));
				}
				/* Updates the MState - Heart Beat 1 */
				i32Val	= EngMC2.uvCmn_MC2_GetActState(1);
				drItem	= (MC2Data)m_grid_mc2_act_head.Items.GetItemAt(0);
				drItem.SetValue(i32Val);
				/* Updates the MState - Heart Beat 2 */
				i32Val	= EngMC2.uvCmn_MC2_GetActState(2);
				drItem	= (MC2Data)m_grid_mc2_act_head.Items.GetItemAt(1);
				drItem.SetValue(i32Val);
				/* Updates - MError */
				i32Val	= EngMC2.uvCmn_MC2_GetActError();
				drItem	= (MC2Data)m_grid_mc2_act_head.Items.GetItemAt(2);
				drItem.SetValue(i32Val, "%");
				/* Updates - INPUTS */
				u64Out	= EngMC2.uvCmn_MC2_GetActInOut(0);
				drItem	= (MC2Data)m_grid_mc2_act_head.Items.GetItemAt(3);
				drItem.SetValue(u64Out);
				/* Updates - OUTPUTS */
				u64Out	= EngMC2.uvCmn_MC2_GetActInOut(1);
				drItem	= (MC2Data)m_grid_mc2_act_head.Items.GetItemAt(4);
				drItem.SetValue(u64Out);

				/* Refresh the grid control */
				m_grid_mc2_act_head.Items.Refresh();
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

		public bool IsEnableDrv(ENG_MMDI drv)
		{
			return !EngMC2.uvCmn_MC2_IsDriveBusy(drv)	&&
				   !EngMC2.uvCmn_MC2_IsDriveError(drv)&&
				   EngMC2.uvCmn_MC2_IsDriveHomed(drv);
		}
		/*
		 desc : Updates the control's state
		 parm : None
		 retn : None
		*/
		private void UpdateControlState()
		{
			Boolean bEnable	= true;
			Boolean bEnStop	= EngMC2.uvCmn_MC2_IsDriveEnStop();

			/* Whether motors are busy or error or not calibrated */
			bEnable	= IsEnableDrv(ENG_MMDI.en_stage_x_master);
			m_btn_mc2_stage_left.IsEnabled		= bEnable && bEnStop;
			m_btn_mc2_stage_right.IsEnabled		= bEnable && bEnStop;
			bEnable	= IsEnableDrv(ENG_MMDI.en_stage_y_master);
			m_btn_mc2_stage_up.IsEnabled		= bEnable && bEnStop;
			m_btn_mc2_stage_down.IsEnabled		= bEnable && bEnStop;
			switch (m_cmb_mc2_ph_no.SelectedIndex)
			{
			case 0: bEnable	= IsEnableDrv(ENG_MMDI.en_axis_ph1);	break;
			case 1: bEnable	= IsEnableDrv(ENG_MMDI.en_axis_ph2);	break;
			case 2: bEnable	= IsEnableDrv(ENG_MMDI.en_axis_ph3);	break;
			}	
			m_btn_mc2_ph_up.IsEnabled	= bEnable && bEnStop;
			m_btn_mc2_ph_down.IsEnabled	= bEnable && bEnStop;


			bEnable	= IsEnableDrv(ENG_MMDI.en_stage_x_master);
			m_btn_mc2_homing_stage_x.IsEnabled	= bEnable && !bEnStop;
			bEnable	= IsEnableDrv(ENG_MMDI.en_stage_y_master);
			m_btn_mc2_homing_stage_y.IsEnabled	= bEnable && !bEnStop;
			bEnable	= IsEnableDrv(ENG_MMDI.en_axis_ph1);
			m_btn_mc2_homing_ph1.IsEnabled	= bEnable && !bEnStop;
			bEnable	= IsEnableDrv(ENG_MMDI.en_axis_ph2);
			m_btn_mc2_homing_ph2.IsEnabled	= bEnable && !bEnStop;
			bEnable	= IsEnableDrv(ENG_MMDI.en_axis_ph3);
			m_btn_mc2_homing_tsa_z.IsEnabled	= bEnable && !bEnStop;

			if (m_btn_mc2_homing_tsa_z.IsEnabled && m_btn_mc2_homing_ph2.IsEnabled &&
				m_btn_mc2_homing_ph1.IsEnabled && m_btn_mc2_homing_stage_y.IsEnabled &&
				m_btn_mc2_homing_stage_x.IsEnabled)
			{
				m_btn_mc2_homing_all.IsEnabled	= bEnable && !bEnStop;
			}

			bEnable	= EngMC2.uvCmn_MC2_IsDevLocked(ENG_MMDI.en_stage_x_master);
			m_chk_mc2_servo_axis_x.IsChecked	= bEnable;
			bEnable	= EngMC2.uvCmn_MC2_IsDevLocked(ENG_MMDI.en_stage_y_master);
			m_chk_mc2_servo_axis_y.IsChecked	= bEnable;
			bEnable	= EngMC2.uvCmn_MC2_IsDevLocked(ENG_MMDI.en_axis_ph1);
			m_chk_mc2_servo_axis_ph1.IsChecked	= bEnable;
			bEnable	= EngMC2.uvCmn_MC2_IsDevLocked(ENG_MMDI.en_axis_ph2);
			m_chk_mc2_servo_axis_ph2.IsChecked	= bEnable;
			bEnable	= EngMC2.uvCmn_MC2_IsDevLocked(ENG_MMDI.en_axis_ph3);
			m_chk_mc2_servo_axis_ph3.IsChecked	= bEnable;
		}
	}
}
