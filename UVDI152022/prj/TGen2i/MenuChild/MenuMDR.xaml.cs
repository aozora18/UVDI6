using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

using System.Diagnostics;
using System.Runtime.InteropServices;
using TGen2i.EngineLib;
using TGen2i.Enum.MDR;

namespace TGen2i.MenuChild
{

	public partial class MenuMDR : MenuBase
	{
		public MenuMDR()
		{
			/* Initialize internal components. */
			InitializeComponent();

			/* Window Event */

			/* Control Event for Button */
			m_btn_mdr_start_teaching.Click					+= btnClick;
			m_btn_mdr_end_teaching.Click					+= btnClick;
			m_btn_mdr_info_mode_set.Click					+= btnClick;
			m_btn_mdr_info_mode_get.Click					+= btnClick;
			m_btn_mdr_slot_pitch_set.Click					+= btnClick;
			m_btn_mdr_slot_pitch_get.Click					+= btnClick;
			m_btn_mdr_station_offset_set.Click				+= btnClick;
			m_btn_mdr_station_offset_get.Click				+= btnClick;
			m_btn_mdr_current_axis_pos_get.Click			+= btnClick;
			m_btn_mdr_end_effector_arm_set.Click			+= btnClick;
			m_btn_mdr_end_effector_arm_get.Click			+= btnClick;
			m_btn_mdr_verical_stroke_set.Click				+= btnClick;
			m_btn_mdr_verical_stroke_get.Click				+= btnClick;
			m_btn_mdr_pick_up_speed_set.Click				+= btnClick;
			m_btn_mdr_pick_up_speed_get.Click				+= btnClick;
			m_btn_mdr_pick_up_acceleration_set.Click		+= btnClick;
			m_btn_mdr_pick_up_acceleration_get.Click		+= btnClick;
			m_btn_mdr_vacuum_time_out_set.Click				+= btnClick;
			m_btn_mdr_vacuum_time_out_get.Click				+= btnClick;
			m_btn_mdr_station_position_set.Click			+= btnClick;
			m_btn_mdr_station_position_get.Click			+= btnClick;
			m_btn_mdr_station_position_retr_set.Click		+= btnClick;
			m_btn_mdr_station_position_retr_get.Click		+= btnClick;
			m_btn_mdr_save_station_param_set.Click			+= btnClick;
			m_btn_mdr_status_get.Click						+= btnClick;
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
		/*                                 User-defined Function                                 */
		/* ------------------------------------------------------------------------------------- */

		private char GetStationName()
		{
			char [] chStation = new char [] {'A', 'a', 'B', 'b', 'C', 'c', 'D', 'd', 'E', 'F', 'G', 'H', 'I'};
			int i32Sel	= 0;
			i32Sel = m_cmb_mdr_station_name.SelectedIndex;

			return chStation[i32Sel];
		}
		private ENG_RANC GetStationAxis()
		{
			return (ENG_RANC)m_cmb_mdr_station_axis.SelectedIndex;
		}
		private void IsRecvAckData()
		{
			Stopwatch sw = new Stopwatch();
			sw.Start();

			while (true)
			{
				if (EngMDR.uvCmn_MDR_IsLastSendAckRecv() &&
					EngMDR.uvCmn_MDR_IsRecvCmdLast())	break;
				Thread.Sleep(100);

				if (sw.ElapsedMilliseconds > 10000)
				{
					MessageBox.Show("wait time-out [10 sec]");
					break;
				}
			}

			sw.Stop();
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
		}

		/*
		 desc : Inited control
		 parm : None
		 retn : None
		*/
		private void InitCtrl()
		{
			m_cmb_mdr_station_name.Items.Add("IN_BUFFER(Primary)");
			m_cmb_mdr_station_name.Items.Add("IN_BUFFER(Secondary)");
			m_cmb_mdr_station_name.Items.Add("ALIGNER(Primary)");
			m_cmb_mdr_station_name.Items.Add("ALIGNER(Secondary)");
			m_cmb_mdr_station_name.Items.Add("STAGE(Primary)");
			m_cmb_mdr_station_name.Items.Add("STAGE(Secondary)");
			m_cmb_mdr_station_name.Items.Add("OUT_BUFFER(Primary)");
			m_cmb_mdr_station_name.Items.Add("OUT_BUFFER(Secondary)");
			m_cmb_mdr_station_name.SelectedIndex	= 0;

			m_cmb_mdr_end_effector_arm.Items.Add("Primary arm");
			m_cmb_mdr_end_effector_arm.Items.Add("Secondary arm");
			m_cmb_mdr_end_effector_arm.SelectedIndex	= 0;

			m_cmb_mdr_station_axis.Items.Add("T");
			m_cmb_mdr_station_axis.Items.Add("R1");
			m_cmb_mdr_station_axis.Items.Add("R2");
			m_cmb_mdr_station_axis.Items.Add("Z");
			m_cmb_mdr_station_axis.Items.Add("Retracted");
			m_cmb_mdr_station_axis.SelectedIndex	= 0;
		}

		/* ------------------------------------------------------------------------------------- */
		/*                                 Windows Event Function                                */
		/* ------------------------------------------------------------------------------------- */

		/* ------------------------------------------------------------------------------------- */
		/*                                 Control Event Function                                */
		/* ------------------------------------------------------------------------------------- */

		/*
		 desc : Button event for Stage Moving Direction
		 parm : s	- [in]  Object
				  e	- [in]  EventArgs
		 retn : None
		*/
		private void btnClick(object s, RoutedEventArgs e)
		{
			FrameworkElement feID = e.Source as FrameworkElement;

			if (feID.Name == "m_btn_mdr_info_mode_set")						btnSetInfoMode();
			else if (feID.Name == "m_btn_mdr_info_mode_get")				btnGetInfoMode();
			else if (feID.Name == "m_btn_mdr_start_teaching")				btnStartOfTeaching();	/* Set the Station Name to Robot */
			else if (feID.Name == "m_btn_mdr_end_teaching")					btnEndOfTeaching();
			else if (feID.Name == "m_btn_mdr_slot_pitch_set")				btnSetSlotPitch();
			else if (feID.Name == "m_btn_mdr_slot_pitch_get")				btnGetSlotPitch();
			else if (feID.Name == "m_btn_mdr_station_offset_set")			btnSetStationOffset();
			else if (feID.Name == "m_btn_mdr_station_offset_get")			btnGetStationOffset();
			else if (feID.Name == "m_btn_mdr_current_axis_pos_get")			btnGetCurrentAxisPos();
			else if (feID.Name == "m_btn_mdr_end_effector_arm_get")			btnGetEndEffectorArm();
			else if (feID.Name == "m_btn_mdr_end_effector_arm_set")			btnSetEndEffectorArm();
			else if (feID.Name == "m_btn_mdr_verical_stroke_set")			btnSetVerticalStroke();
			else if (feID.Name == "m_btn_mdr_verical_stroke_get")			btnGetVerticalStroke();
			else if (feID.Name == "m_btn_mdr_pick_up_speed_set")			btnSetPickUpSpeed();
			else if (feID.Name == "m_btn_mdr_pick_up_speed_get")			btnGetPickUpSpeed();
			else if (feID.Name == "m_btn_mdr_pick_up_acceleration_set")		btnSetPickUpAcceleration();
			else if (feID.Name == "m_btn_mdr_pick_up_acceleration_get")		btnGetPickUpAcceleration();
			else if (feID.Name == "m_btn_mdr_vacuum_time_out_set")			btnSetVacuumTimeout();
			else if (feID.Name == "m_btn_mdr_vacuum_time_out_get")			btnGetVacuumTimeout();
			else if (feID.Name == "m_btn_mdr_station_position_set")			btnSetStationPosition();
			else if (feID.Name == "m_btn_mdr_station_position_get")			btnGetStationPosition();
			else if (feID.Name == "m_btn_mdr_station_position_retr_set")	btnSetStationPositionRetr();
			else if (feID.Name == "m_btn_mdr_station_position_retr_get")	btnGetStationPositionRetr();
			else if (feID.Name == "m_btn_mdr_save_station_param_set")		btnSetSaveStationParam();
			else if (feID.Name == "m_btn_mdr_status_get")					btnGetStatusMesg();
		}

		private void btnGetStatusMesg()
		{
			IsRecvAckData();
		}
		private void btnSetSaveStationParam()
		{
			EngMDR.uvEng_MDR_SaveStationParameters();
		}
		private void btnGetStationPositionRetr()
		{
			EngMDR.uvEng_MDR_GetRetractedStationPositionR(GetStationName());
			IsRecvAckData();
			Double dbVal	= EngMDR.uvCmn_MDR_GetValueStationRetractedPositionR();
			m_edt_mdr_station_position_retr.Text	= Math.Round(dbVal/1000.0, 2).ToString();
		}
		private void btnSetStationPositionRetr()
		{
			ENG_RANC enDrv = (ENG_RANC)m_cmb_mdr_station_axis.SelectedIndex;
			EngMDR.uvEng_MDR_SetStationPositionName(GetStationName(), enDrv, Double.Parse(m_edt_mdr_station_position.Text));
		}
		private void btnGetStationPosition()
		{
			EngMDR.uvEng_MDR_GetStationPositionName(GetStationName());
			IsRecvAckData();
			IntPtr pValue	= EngMDR.uvCmn_MDR_GetValueStationPosition();
			Double [] pPos	= new Double[4];
			Marshal.Copy(pValue, pPos, 0, pPos.Length);
			m_edt_mdr_station_position_t.Text	= Math.Round(pPos[0], 2).ToString();
			m_edt_mdr_station_position_r1.Text	= Math.Round(pPos[1]/1000.0, 2).ToString();
			m_edt_mdr_station_position_r2.Text	= Math.Round(pPos[2]/1000.0, 2).ToString();
			m_edt_mdr_station_position_z.Text	= Math.Round(pPos[3]/1000.0, 2).ToString();
		}
		private void btnSetStationPosition()
		{
			ENG_RANC enDrv = (ENG_RANC)m_cmb_mdr_station_axis.SelectedIndex;
			EngMDR.uvEng_MDR_SetStationPositionName(GetStationName(), enDrv, Double.Parse(m_edt_mdr_station_position.Text));
		}
		private void btnGetVacuumTimeout()
		{
			EngMDR.uvEng_MDR_GetPutGetDelayTime(GetStationName());
			IsRecvAckData();
			UInt32 u32Val	= EngMDR.uvCmn_MDR_GetValuePutGetDelayTime();
			m_edt_mdr_vacuum_time_out.Text	= Math.Round(u32Val / 1000.0, 2).ToString();
		}
		private void btnSetVacuumTimeout()
		{
			UInt32 u32Val = (UInt32)Math.Round(Double.Parse(m_edt_mdr_vacuum_time_out.Text) * 1000.0, 0); /* msec -> sec */
			EngMDR.uvEng_MDR_SetPutGetDelayTime(GetStationName(), u32Val);
		}

		private void btnGetPickUpAcceleration()
		{
			EngMDR.uvEng_MDR_GetStationPickUpAccelerationZ(GetStationName());
			IsRecvAckData();
			Double dbVal	= EngMDR.uvCmn_MDR_GetValueStationPickupAccelerationZ();
			m_edt_mdr_pick_up_acceleration.Text	= Math.Round(dbVal / 1000.0, 2).ToString();
		}

		private void btnSetPickUpAcceleration()
		{
			Double dbVal = Double.Parse(m_edt_mdr_pick_up_acceleration.Text) * 1000.0; /* mm -> um */
			EngMDR.uvEng_MDR_SetStationPickUpAccelerationZ(GetStationName(), dbVal);
		}

		private void btnGetPickUpSpeed()
		{
			EngMDR.uvEng_MDR_GetStationPickUpSpeedZ(GetStationName());
			IsRecvAckData();
			Double dbVal	= EngMDR.uvCmn_MDR_GetValueStationPickupSpeedZ();
			m_edt_mdr_pick_up_speed.Text	= Math.Round(dbVal / 1000.0, 2).ToString();
		}
		private void btnSetPickUpSpeed()
		{
			Double dbVal = Double.Parse(m_edt_mdr_pick_up_speed.Text) * 1000.0; /* mm -> um */
			EngMDR.uvEng_MDR_SetStationPickUpSpeedZ(GetStationName(), dbVal);
		}
		private void btnSetInfoMode()	{	EngMDR.uvEng_MDR_SetInfoMode();	}
		private void btnGetInfoMode()	{	EngMDR.uvEng_MDR_GetInfoMode();	}
		private void btnStartOfTeaching()
		{
			EngMDR.uvEng_MDR_StartTeachingStation(GetStationName());
		}
		private void btnEndOfTeaching() { EngMDR.uvEng_MDR_SetTeachingEnd();}

		private void btnGetSlotPitch()
		{
			EngMDR.uvEng_MDR_GetStationPitch(GetStationName());
			IsRecvAckData();
			Double dbPitchSlot = EngMDR.uvCmn_MDR_GetValueStationPitch();
			m_edt_mdr_slot_pitch.Text	= Math.Round(dbPitchSlot/1000.0, 1).ToString();
		}
		private void btnSetSlotPitch()
		{
			Double dbVal = Double.Parse(m_edt_mdr_slot_pitch.Text) * 1000.0; /* mm -> um */
			EngMDR.uvEng_MDR_SetStationPitch(GetStationName(), dbVal);
		}

		private void btnGetStationOffset()
		{
			EngMDR.uvEng_MDR_GetStationOffsetZ(GetStationName());

			IsRecvAckData();

			Double dbVal = EngMDR.uvCmn_MDR_GetValueStationOffsetZ();
			m_edt_mdr_station_offset.Text	= Math.Round(dbVal/1000.0, 1).ToString();
		}
		private void btnSetStationOffset()
		{
			Double dbVal = Double.Parse(m_edt_mdr_station_offset.Text) * 1000.0; /* mm -> um */
			EngMDR.uvEng_MDR_SetStationOffsetZ(GetStationName(), dbVal);
		}
		private void btnGetCurrentAxisPos()
		{
			EngMDR.uvEng_MDR_GetAxisCurrentPositionAll();
			IsRecvAckData();
			IntPtr pValue	= EngMDR.uvCmn_MDR_GetValueAxisCurrentPosition();
			Double [] pPos	= new Double[4];
			Marshal.Copy(pValue, pPos, 0, pPos.Length);
			m_edt_mdr_axis_pos_t.Text	= (pPos[0]).ToString();
			m_edt_mdr_axis_pos_r1.Text	= (pPos[1]/1000.0).ToString();
			m_edt_mdr_axis_pos_r2.Text	= (pPos[2]/1000.0).ToString();
			m_edt_mdr_axis_pos_z.Text	= (pPos[3]/1000.0).ToString();
		}
		private void btnGetEndEffectorArm()
		{
			EngMDR.uvEng_MDR_GetEndEffectorNumForStation(GetStationName());
			IsRecvAckData();
			Byte u8SelNum	= EngMDR.uvCmn_MDR_GetValueEndEffectorNum();
			m_cmb_mdr_end_effector_arm.SelectedIndex	= (int)u8SelNum;
		}
		private void btnSetEndEffectorArm()
		{
			EngMDR.uvEng_MDR_SetEndEffectorNumForStation(GetStationName(),
															(Byte)m_cmb_mdr_end_effector_arm.SelectedIndex);
		}
		private void btnGetVerticalStroke()
		{
			EngMDR.uvEng_MDR_GetStationStrokeZ(GetStationName());
			IsRecvAckData();
			Byte u8SelNum	= EngMDR.uvCmn_MDR_GetValueEndEffectorNum();
			Double dbVert	= EngMDR.uvCmn_MDR_GetValueStationStrokeZ();
			m_edt_mdr_verical_stroke.Text	= Math.Round(dbVert/1000.0, 2).ToString();
		}
		private void btnSetVerticalStroke()
		{
			EngMDR.uvEng_MDR_SetStationStrokeZ(GetStationName(), Double.Parse(m_edt_mdr_verical_stroke.Text) * 1000.0/* mm -> um */);
		}
	}
}
