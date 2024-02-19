using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Threading;
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
using TGen2i.Enum.Luria;
using TGen2i.Struct.Comn;
using TGen2i.Struct.Luria;

namespace TGen2i.MenuChild
{
	public partial class MenuLuria2 : MenuBase
	{
		public MenuLuria2()
		{
			/* Initialize internal components. */
			InitializeComponent();

			/* Window Event */

			/* Control Event for Button */
			m_btn_luria_2_InitZ.Click						+= btnClick;
			m_btn_luria_2_af_all_on.Click					+= btnClick;
			m_btn_luria_2_af_all_off.Click					+= btnClick;
			m_btn_luria_2_af_all_get.Click					+= btnClick;
			m_btn_luria_2_af_edge_trigger_set.Click			+= btnClick;
			m_btn_luria_2_af_edge_trigger_get.Click			+= btnClick;
			m_btn_luria_2_af_gain_set.Click					+= btnClick;
			m_btn_luria_2_af_gain_get.Click					+= btnClick;
			m_btn_luria_2_aaq_set.Click						+= btnClick;
			m_btn_luria_2_aaq_get.Click						+= btnClick;
			m_btn_luria_2_abs_work_range_set.Click			+= btnClick;
			m_btn_luria_2_abs_work_range_get.Click			+= btnClick;
			m_btn_luria_2_abs_work_range_status.Click		+= btnClick;
			m_btn_luria_2_abs_work_range_all_set.Click		+= btnClick;
			m_btn_luria_2_abs_work_range_all_get.Click		+= btnClick;
			m_btn_luria_2_focus_pos_set.Click				+= btnClick;
			m_btn_luria_2_focus_pos_get.Click				+= btnClick;
			m_btn_luria_2_focus_pos_clear.Click				+= btnClick;
			m_btn_luria_2_outside_dof_status.Click			+= btnClick;
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

		/*
		 desc : Updates the control in the window
		 parm : None
		 retn : None
		*/
		public override void UpdateControl()
		{
			/* Get the shared memory for luria */
			IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
			STG_LDSM stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));
		}

		/*
		 desc : Inited control
		 parm : None
		 retn : None
		*/
		private void InitCtrl()
		{
			m_cmb_luria_2_abs_work_range.Items.Add("1");
			m_cmb_luria_2_abs_work_range.Items.Add("2");
			m_cmb_luria_2_abs_work_range.Items.Add("3");
		}

		/*
		 desc : UI thread task delay without stopping (meaning different from Sleep)
		 para : delay	- [in]  delay time (unit: msec)
				cmd		- [in]  check command
		 retn : true or false
		*/
		private Boolean WaitRecv(Int32 delay, ENG_FDPR cmd=ENG_FDPR.en_luria_none)
		{
			DateTime dtNow	= DateTime.Now;
			TimeSpan tsDur	= new TimeSpan(0, 0, 0, 0, delay);
			DateTime dtAdd	= dtNow.Add(tsDur);
			while (dtAdd >= dtNow)
			{
				Thread.Sleep(100);
				if (cmd != ENG_FDPR.en_luria_none)
				{
					if (EngLuria.uvEng_Luria_IsRecvPktData(cmd))	return true;
				}
				else
				{
					if (EngLuria.uvEng_Luria_IsLastRecvCmd())	return true;
				}
				/* Append the reference event : System.Windows.Forms (C:\Program Files (x86)\Reference Assemblies\Microsoft\Framework\.NETFramework\v4.8) */
				System.Windows.Forms.Application.DoEvents();
				/* Set the current time */
				dtNow = DateTime.Now;
			}

			MessageBox.Show("Failed to send or recv the command");
			return false;
		}

		/* ------------------------------------------------------------------------------------- */
		/*                                 Control Event Function                                */
		/* ------------------------------------------------------------------------------------- */

		/*
		 desc : Button event
		 parm : s	- [in]  Object
				e	- [in]  EventArgs
		 retn : None
		*/
		private void btnClick(object s, RoutedEventArgs e)
		{
			FrameworkElement feID = e.Source as FrameworkElement;

			if (feID.Name == "m_btn_luria_2_InitZ")								btnInitZAxis();
			else if (feID.Name == "m_btn_luria_2_af_all_on")					btnAutoFocusOnOffAll(0);
			else if (feID.Name == "m_btn_luria_2_af_all_off")					btnAutoFocusOnOffAll(1);
			else if (feID.Name == "m_btn_luria_2_af_all_get")					btnAutoFocusOnOffAll(2);
			else if (feID.Name == "m_btn_luria_2_af_edge_trigger_set")			btnEdgeTrigger(0);
			else if (feID.Name == "m_btn_luria_2_af_edge_trigger_get")			btnEdgeTrigger(1);
			else if (feID.Name == "m_btn_luria_2_af_gain_set")					btnAfGain(0);
			else if (feID.Name == "m_btn_luria_2_af_gain_get")					btnAfGain(1);
			else if (feID.Name == "m_btn_luria_2_aaq_set")						btnAAQ(0);
			else if (feID.Name == "m_btn_luria_2_aaq_get")						btnAAQ(1);
			else if (feID.Name == "m_btn_luria_2_abs_work_range_set")			btnAbsWorkRange(0);
			else if (feID.Name == "m_btn_luria_2_abs_work_range_get")			btnAbsWorkRange(1);
			else if (feID.Name == "m_btn_luria_2_abs_work_range_all_set")		btnAbsWorkRange(2);
			else if (feID.Name == "m_btn_luria_2_abs_work_range_all_get")		btnAbsWorkRange(3);
			else if (feID.Name == "m_btn_luria_2_abs_work_range_status")		btnAbsWorkRange(4);
			else if (feID.Name == "m_btn_luria_2_focus_pos_set")				btnFocusPos(0);
			else if (feID.Name == "m_btn_luria_2_focus_pos_get")				btnFocusPos(1);
			else if (feID.Name == "m_btn_luria_2_focus_pos_clear")				btnFocusPos(2);
			else if (feID.Name == "m_btn_luria_2_outside_dof_status")			btnOutsideDofStatus();
		}

		private void btnOutsideDofStatus()
		{
			//m_edt_luria_2_outside_dof_status_1;
			EngLuria.uvEng_Luria_ReqGetOutsideDOFStatusLM();
			if (!WaitRecv(2000))	return;

			IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
			STG_LDSM stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));
			m_edt_luria_2_outside_dof_status_1.Text	= stLuria.focus.out_dof_trig_pulse[0].ToString();
			m_edt_luria_2_outside_dof_status_2.Text	= stLuria.focus.out_dof_trig_pulse[1].ToString();
			m_edt_luria_2_outside_dof_status_3.Text	= stLuria.focus.out_dof_trig_pulse[2].ToString();
		}

		private void btnFocusPos(int v)
		{
			Byte u8SetCnt	= 0;
			Byte[] arrPhNo	= null;
//			Int32[] arr32Pos= null;
			Double[] arrDbPos= null;

			if (m_chk_luria_2_focus_pos_1.IsChecked == true)	u8SetCnt++;
			if (m_chk_luria_2_focus_pos_2.IsChecked == true)	u8SetCnt++;
			if (m_chk_luria_2_focus_pos_3.IsChecked == true)	u8SetCnt++;
			if (u8SetCnt > 0 && 0 == v)
			{
				arrPhNo	= new byte[u8SetCnt];
//				arr32Pos= new Int32[u8SetCnt];
				arrDbPos= new Double[u8SetCnt];
				u8SetCnt= 0;
				if (m_chk_luria_2_focus_pos_1.IsChecked == true)
				{
					arrPhNo[u8SetCnt]	= 1;
					if (m_edt_luria_2_focus_pos_1.Text.Length < 1)	return;
//					arr32Pos[u8SetCnt]	= Convert.ToInt32(Convert.ToDouble(m_edt_luria_2_focus_pos_1.Text)*1000.0);
					arrDbPos[u8SetCnt]	= Convert.ToDouble(m_edt_luria_2_focus_pos_1.Text);
					u8SetCnt++;
				}
				if (m_chk_luria_2_focus_pos_2.IsChecked == true)
				{
					arrPhNo[u8SetCnt]	= 2;
					if (m_edt_luria_2_focus_pos_2.Text.Length < 1)	return;
//					arr32Pos[u8SetCnt]	= Convert.ToInt32(Convert.ToDouble(m_edt_luria_2_focus_pos_2.Text)*1000.0);
					arrDbPos[u8SetCnt]	= Convert.ToDouble(m_edt_luria_2_focus_pos_2.Text);
					u8SetCnt++;
				}
				if (m_chk_luria_2_focus_pos_3.IsChecked == true)
				{
					arrPhNo[u8SetCnt]	= 3;
					if (m_edt_luria_2_focus_pos_3.Text.Length < 1)	return;
//					arr32Pos[u8SetCnt]	= Convert.ToInt32(Convert.ToDouble(m_edt_luria_2_focus_pos_3.Text)*1000.0);
					arrDbPos[u8SetCnt]	= Convert.ToDouble(m_edt_luria_2_focus_pos_3.Text);
					u8SetCnt++;
				}
			}
			if (0 == v)
			{
				if (u8SetCnt < 1)	return;
//				EngLuria.uvEng_Luria_ReqSetPositionMultiLM(u8SetCnt, arrPhNo, arr32Pos);
				EngLuria.uvEng_Luria_ReqSetPositionMultiLM(u8SetCnt, arrPhNo, arrDbPos);
			}
			else if (1 == v)
			{
				EngLuria.uvEng_Luria_ReqGetPositionMultiLM();
				if (!WaitRecv(2000))	return;
				IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
				STG_LDSM stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));
				m_edt_luria_2_focus_pos_1.Text	= string.Format("{0:0.000000}", stLuria.focus.z_pos[0]/1000000.0);
				m_edt_luria_2_focus_pos_2.Text	= string.Format("{0:0.000000}", stLuria.focus.z_pos[1]/1000000.0);
				m_edt_luria_2_focus_pos_3.Text	= string.Format("{0:0.000000}", stLuria.focus.z_pos[2]/1000000.0);
			}
			else
			{
				m_edt_luria_2_focus_pos_1.Text	= "0.000000";
				m_edt_luria_2_focus_pos_2.Text	= "0.000000";
				m_edt_luria_2_focus_pos_3.Text	= "0.000000";
			}
		}

		private void btnAbsWorkRange(int v)
		{
			Int32 i32PhNo			= m_cmb_luria_2_abs_work_range.SelectedIndex;
			Int32 i32MinAbsPos		= 0;
			if (m_edt_luria_2_min_abs_work_range.Text.Length>0)
				i32MinAbsPos	= Convert.ToInt32(Convert.ToDouble(m_edt_luria_2_min_abs_work_range.Text)*1000.0);
			Int32 i32MaxAbsPos		= 0;
			if (m_edt_luria_2_max_abs_work_range.Text.Length>0)
				i32MaxAbsPos	= Convert.ToInt32(Convert.ToDouble(m_edt_luria_2_max_abs_work_range.Text)*1000.0);
			Int32 i32MinAbsPosAll	= 0;
			if (m_edt_luria_2_min_abs_work_range_all.Text.Length>0)
				i32MinAbsPosAll	= Convert.ToInt32(Convert.ToDouble(m_edt_luria_2_min_abs_work_range_all.Text)*1000.0);
			Int32 i32MaxAbsPosAll	= 0;
			if (m_edt_luria_2_max_abs_work_range_all.Text.Length>0)
				i32MaxAbsPosAll	= Convert.ToInt32(Convert.ToDouble(m_edt_luria_2_max_abs_work_range_all.Text)*1000.0);
			if (0 == v)
			{
				if (i32PhNo < 0)	return;
				EngLuria.uvEng_Luria_ReqSetAbsWorkRangeLM(Convert.ToByte(i32PhNo), i32MinAbsPos, i32MaxAbsPos);
			}
			else if (1 == v)
			{
				if (i32PhNo < 0)	return;
				EngLuria.uvEng_Luria_ReqGetAbsWorkRangeLM(Convert.ToByte(i32PhNo+1));
				if (!WaitRecv(2000))	return;
				IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
				STG_LDSM stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));
				m_edt_luria_2_min_abs_work_range.Text	= string.Format("{0:0.000}", stLuria.focus.abs_work_range_min[i32PhNo]/1000.0);
				m_edt_luria_2_max_abs_work_range.Text	= string.Format("{0:0.000}", stLuria.focus.abs_work_range_max[i32PhNo]/1000.0);
			}
			else if (2 == v)
			{
				EngLuria.uvEng_Luria_ReqSetAbsWorkRangeLM(0x00, i32MinAbsPosAll, i32MaxAbsPosAll);
			}
			else if (3 == v)
			{
				EngLuria.uvEng_Luria_ReqGetAbsWorkRangeLM(0x00);
				if (!WaitRecv(2000))	return;
				IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
				STG_LDSM stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));
				m_edt_luria_2_min_abs_work_range_all.Text	= string.Format("{0:0.000}", stLuria.focus.abs_work_range_min[0]/1000.0);
				m_edt_luria_2_max_abs_work_range_all.Text	= string.Format("{0:0.000}", stLuria.focus.abs_work_range_max[0]/1000.0);
			}
			else
			{
				EngLuria.uvEng_Luria_ReqGetAbsWorkRangeStatusLM();
				if (!WaitRecv(2000))	return;
				IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
				STG_LDSM stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));
				m_edt_luria_2_status_below_min.Text	= string.Format("{0} {1} {2}",
					stLuria.focus.IsWorkRangeStatusValid(0, 0) ? 1 : 0,
					stLuria.focus.IsWorkRangeStatusValid(1, 0) ? 1 : 0,
					stLuria.focus.IsWorkRangeStatusValid(2, 0) ? 1 : 0);
				m_edt_luria_2_status_above_max.Text	= string.Format("{0} {1} {2}",
					stLuria.focus.IsWorkRangeStatusValid(0, 1) ? 1 : 0,
					stLuria.focus.IsWorkRangeStatusValid(1, 1) ? 1 : 0,
					stLuria.focus.IsWorkRangeStatusValid(2, 1) ? 1 : 0);
			}
		}

		private void btnAAQ(int v)
		{
			Byte u8Enable	= Convert.ToByte(m_chk_luria_2_aaq.IsChecked);
			UInt32 u32Start	= 0;
			if (m_edt_luria_2_aaq_dist_start.Text.Length>0)
				u32Start= Convert.ToUInt32(Convert.ToDouble(m_edt_luria_2_aaq_dist_start.Text) * 1000.0);
			UInt32 u32End	= 0;
			if (m_edt_luria_2_aaq_dist_end.Text.Length>0)
				u32End	= Convert.ToUInt32(Convert.ToDouble(m_edt_luria_2_aaq_dist_end.Text) * 1000.0);
			if (0 == v)	EngLuria.uvEng_Luria_ReqSetActiveAreaQualifierLM(u8Enable, u32Start, u32End);
			else
			{
				EngLuria.uvEng_Luria_ReqGetActiveAreaQualifierLM();
				if (!WaitRecv(2000))	return;
				IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
				STG_LDSM stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));
				m_edt_luria_2_aaq_dist_start.Text= string.Format("{0:0.000}", stLuria.focus.aaq_start_end[0]/1000.0);
				m_edt_luria_2_aaq_dist_end.Text= string.Format("{0:0.000}", stLuria.focus.aaq_start_end[1]/1000.0);
				m_chk_luria_2_aaq.IsChecked	= stLuria.focus.aaq_actived==1 ? true : false;
			}
		}

		private void btnAfGain(int v)
		{
			UInt16 u16Gain	= 0;
			if (m_edt_luria_2_af_gain_factor.Text.Length > 0)
				u16Gain = Convert.ToUInt16(m_edt_luria_2_af_gain_factor.Text);
			if (0 == v)	EngLuria.uvEng_Luria_ReqSetAfGainLM(u16Gain);
			else
			{
				EngLuria.uvEng_Luria_ReqGetAfGainLM();
				if (!WaitRecv(2000))	return;
				IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
				STG_LDSM stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));
				m_edt_luria_2_af_gain_factor.Text	= stLuria.focus.af_gain.ToString();
			}
		}

		private void btnEdgeTrigger(int v)
		{
			UInt16 u16Dist	= 0;
			if (m_edt_luria_2_max_set_point_diff.Text.Length>0)
				u16Dist = Convert.ToUInt16(m_edt_luria_2_max_set_point_diff.Text);
			Byte u8Enable	= Convert.ToByte(m_chk_luria_2_af_edge_trigger.IsChecked);
			if (0 == v)	EngLuria.uvEng_Luria_ReqSetAfEdgeTriggerLM(u16Dist, u8Enable);
			else
			{
				EngLuria.uvEng_Luria_ReqGetAfEdgeTriggerLM();
				if (!WaitRecv(2000))	return;
				IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
				STG_LDSM stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));

				m_edt_luria_2_max_set_point_diff.Text	= stLuria.focus.af_edge_trig_dist.ToString();
				m_chk_luria_2_af_edge_trigger.IsChecked	= stLuria.focus.af_edge_trig==1 ? true : false;
			}
		}

		private void btnAutoFocusOnOffAll(int v)
		{
			if (0 == v)			EngLuria.uvEng_Luria_ReqSetAutoFocusLM(0x00, 0x01);
			else if (1 == v)	EngLuria.uvEng_Luria_ReqSetAutoFocusLM(0x00, 0x00);
			else
			{
				EngLuria.uvEng_Luria_ReqGetAutoFocusLM(0x00);
				if (!WaitRecv(2000))	return;
				IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
				STG_LDSM stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));
				if (stLuria.focus.auto_focus[0] == 1)	m_edt_luria_2_af_all.Text = "ON";
				else									m_edt_luria_2_af_all.Text = "OFF";
			}
		}

		private void btnInitZAxis()
		{
			EngLuria.uvEng_Luria_ReqSetInitializeFocusLM();
		}
	}
}
