using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
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
using TGen2i.Enum.Luria;
using TGen2i.Struct.Comn;
using TGen2i.Struct.Luria;

namespace TGen2i.MenuChild
{
	public partial class MenuLuria1 : MenuBase
	{
		public MenuLuria1()
		{
			/* Initialize internal components. */
			InitializeComponent();

			/* Window Event */

			/* Control Event for Button */
			m_btn_luria_1_init_hw.Click								+= btnClick;
			m_btn_luria_1_emulate_get.Click							+= btnClick;
			m_btn_luria_1_emulate_set.Click							+= btnClick;
			m_btn_luria_1_all_photoheads_static_get.Click			+= btnClick;
			m_btn_luria_1_all_photoheads_static_set.Click			+= btnClick;
			m_btn_luria_1_all_photoheads_job_independent_get.Click	+= btnClick;
			m_btn_luria_1_all_photoheads_job_independent_set.Click	+= btnClick;
			m_btn_luria_1_spec_ph_get.Click							+= btnClick;
			m_btn_luria_1_spec_ph_set.Click							+= btnClick;
			m_btn_luria_1_motion_controller_get.Click				+= btnClick;
			m_btn_luria_1_motion_controller_set.Click				+= btnClick;
			m_btn_luria_1_table_settings_get.Click					+= btnClick;
			m_btn_luria_1_table_settings_set.Click					+= btnClick;

			m_btn_luria_1_root_directory_browser.Click				+= btnClick;
			m_btn_luria_1_root_directory_browser_get.Click			+= btnClick;
			m_btn_luria_1_root_directory_browser_set.Click			+= btnClick;
			m_btn_luria_1_artwork_complexity_get.Click				+= btnClick;
			m_btn_luria_1_artwork_complexity_set.Click				+= btnClick;
			m_btn_luria_1_gerber_file_add.Click						+= btnClick;
			m_btn_luria_1_gerber_file_del.Click						+= btnClick;
			m_btn_luria_1_gerber_file_load.Click					+= btnClick;
			m_btn_luria_1_get_jobs.Click							+= btnClick;

			m_btn_print_active_table_get.Click						+= btnClick;
			m_btn_print_active_table_set.Click						+= btnClick;
			m_btn_print_sim_dir_get.Click							+= btnClick;
			m_btn_print_sim_dir_set.Click							+= btnClick;
			m_btn_panel_copy_gbr.Click								+= btnClick;

			m_btn_print_run_registration.Click						+= btnClick;
			m_btn_etc_set.Click										+= btnClick;
			m_btn_etc_get.Click										+= btnClick;
			m_btn_print_trans_get.Click								+= btnClick;
			m_btn_print_trans_set.Click								+= btnClick;
			m_btn_expo_param_get.Click								+= btnClick;
			m_btn_expo_param_set.Click								+= btnClick;
			m_btn_ampl_get.Click									+= btnClick;
			m_btn_ampl_set.Click									+= btnClick;

			m_btn_print_pre_print.Click								+= btnClick;
			m_btn_print_print.Click									+= btnClick;
			m_btn_print_abort.Click									+= btnClick;
			m_btn_print_ramp_up.Click								+= btnClick;

			m_box_luria_1_gerber_files.SelectionChanged				+= boxSelectionChanged;

			/* Control Event for Checkbox */
//			m_chk_luria_1_emulate_motor_controller.Click			+= chkClick;

			m_pgr_luria_1_load_rate.Minimum= 0.0;
			m_pgr_luria_1_load_rate.Maximum= 100.0;
			m_pgr_print_rate.Minimum	= 0.0;
			m_pgr_print_rate.Maximum	= 100.0;
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

			/* Progress - Load Rate */
			m_pgr_luria_1_load_rate.Value	= stLuria.jobmgt.GetLoadRate();
			switch ((ENG_LSLS)stLuria.jobmgt.selected_job_load_state)
			{
			case ENG_LSLS.en_load_none:			m_edt_luria_1_job_load_state.Text	= "None";				break;
			case ENG_LSLS.en_load_not_started:	m_edt_luria_1_job_load_state.Text	= "Load not started";	break;
			case ENG_LSLS.en_loading:			m_edt_luria_1_job_load_state.Text	= "Loading";			break;
			case ENG_LSLS.en_load_completed:	m_edt_luria_1_job_load_state.Text	= "Load completed";		break;
			case ENG_LSLS.en_load_failed:		m_edt_luria_1_job_load_state.Text	= "Load failed";		break;
			}
		}

		/*
		 desc : Inited control
		 parm : None
		 retn : None
		*/
		private void InitCtrl()
		{
			/* Add items to combobox for mc2 drive */
			m_cmb_luria_1_pid.Items.Add("LLS50");
			m_cmb_luria_1_pid.Items.Add("LLS30");
			m_cmb_luria_1_pid.Items.Add("LLS25");
			m_cmb_luria_1_pid.Items.Add("LLS15");
			m_cmb_luria_1_pid.Items.Add("LLS10");
			m_cmb_luria_1_pid.Items.Add("LLS06");
			m_cmb_luria_1_pid.Items.Add("LLS04");
			m_cmb_luria_1_pid.Items.Add("LLS2500");
//			m_cmb_luria_1_pid.SelectedIndex = 0;

			m_cmb_luria_1_motion_controller.Items.Add("Sieb&Meyer");
			m_cmb_luria_1_motion_controller.Items.Add("Newport");
//			m_cmb_luria_1_motion_controller.SelectedIndex = 0;

			m_cmb_luria_1_zdrive_type.Items.Add("Stepper");
			m_cmb_luria_1_zdrive_type.Items.Add("Linear_ACS");
			m_cmb_luria_1_zdrive_type.Items.Add("Linear_SM");
//			m_cmb_luria_1_zdrive_type.SelectedIndex = 0;

			m_cmb_luria_1_spx_level.Items.Add("36");
			m_cmb_luria_1_spx_level.Items.Add("1");
//			m_cmb_luria_1_spx_level.SelectedIndex = 0;

			m_cmb_luria_1_spec_ph_num.Items.Add("1");
			m_cmb_luria_1_spec_ph_num.SelectedIndex = 0;

			m_cmb_luria_1_scroll_mode.Items.Add("1");
			m_cmb_luria_1_scroll_mode.Items.Add("2");
			m_cmb_luria_1_scroll_mode.Items.Add("3");
			m_cmb_luria_1_scroll_mode.Items.Add("4");
			m_cmb_luria_1_scroll_mode.Items.Add("5");
			m_cmb_luria_1_scroll_mode.Items.Add("6");
			m_cmb_luria_1_scroll_mode.SelectedIndex = 0;

			m_cmb_luria_1_table_set_tbl_no.Items.Add("1");
			m_cmb_luria_1_table_set_tbl_no.Items.Add("2");
			m_cmb_luria_1_table_set_tbl_no.SelectedIndex = 0;

			m_cmb_luria_1_artwork_complexity.Items.Add("Normal");
			m_cmb_luria_1_artwork_complexity.Items.Add("High");
			m_cmb_luria_1_artwork_complexity.Items.Add("Extreme");
			m_cmb_luria_1_artwork_complexity.SelectedIndex = 0;

			m_cmb_print_active_table.Items.Add("1");
			m_cmb_print_active_table.Items.Add("2");
			m_cmb_print_active_table.SelectedIndex = 0;
		}

		private Boolean WaitTimeout(Int32 delay, DateTime last)
		{
			TimeSpan tsDur	= new TimeSpan(0, 0, 0, 0, delay);
			return DateTime.Now > last.Add(tsDur);
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

		/* desc : Returns product_id value as an index (zero-based) */
		private Int32 GetProductIdToIndex(ENG_LPPI pid)
		{
			Int32 i32PID	= 0;

			switch (pid)
			{
			case ENG_LPPI.en_lls_50		:	i32PID = 0;	break;
			case ENG_LPPI.en_lls_30		:	i32PID = 1;	break;
			case ENG_LPPI.en_lls_25		:	i32PID = 2;	break;
			case ENG_LPPI.en_lls_15		:	i32PID = 3;	break;
			case ENG_LPPI.en_lls_10		:	i32PID = 4;	break;
			case ENG_LPPI.en_lls_04		:	i32PID = 5;	break;
			case ENG_LPPI.en_lls_2500	:	i32PID = 6;	break;
			}
			return i32PID;
		}
		private UInt32 GetProductIndexToId(Int32 index)
		{
			ENG_LPPI enPID = ENG_LPPI.en_lls_15;

			switch (index)
			{
			case 0:	enPID = ENG_LPPI.en_lls_50;		break;
			case 1:	enPID = ENG_LPPI.en_lls_30;		break;
			case 2:	enPID = ENG_LPPI.en_lls_25;		break;
			case 3:	enPID = ENG_LPPI.en_lls_15;		break;
			case 4:	enPID = ENG_LPPI.en_lls_10;		break;
			case 5:	enPID = ENG_LPPI.en_lls_04;		break;
			case 6:	enPID = ENG_LPPI.en_lls_2500;	break;
			}
			return Convert.ToUInt32(enPID);
		}

		/* ------------------------------------------------------------------------------------- */
		/*                                 Windows Event Function                                */
		/* ------------------------------------------------------------------------------------- */

		/* ------------------------------------------------------------------------------------- */
		/*                                 Control Event Function                                */
		/* ------------------------------------------------------------------------------------- */

		/*
		 desc : Box selection event
		 parm : s	- [in]  Event Handler
				e	- [in]  Event Specification
		 retrn : None
		*/
		private void boxSelectionChanged(object s, SelectionChangedEventArgs e)
		{
			UInt16 i		= 0x0000;
			if (m_box_luria_1_gerber_files.SelectedItem == null)	return;

			string strSel	= m_box_luria_1_gerber_files.SelectedItem.ToString();
			if (strSel.Length < 1)	return;
			/* 선택된 거버 선택 */
			EngLuria.uvEng_Luria_ReqSelectedJobName(strSel.ToCharArray());
			if (!WaitRecv(2000))	return;
			/* 선택된 거버의 적재 상태 요청 */
			EngLuria.uvEng_Luria_ReqGetSelectedJobLoadState();
			if (!WaitRecv(2000))	return;
			/* 선택된 거버의 Job Info & DCode Info */
 			EngLuria.uvEng_Luria_ReqGerberJobParam();
 			if (!WaitRecv(2000))	return;

			/* Get the shared memory for luria */
			IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
			STG_LDSM stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));

			m_edt_luria_1_pre_pro_sw_ver.Text	= string.Format("{0}.{1}.{2}.{3}",
															stLuria.jobmgt.get_job_params.version[0],
															stLuria.jobmgt.get_job_params.version[1],
															stLuria.jobmgt.get_job_params.version[2],
															stLuria.jobmgt.get_job_params.version[3]);
			m_edt_luria_1_lens_magnification.Text		= string.Format("{0:0.000}x", stLuria.jobmgt.get_job_params.lens_magnification/1000.0);
			m_edt_luria_1_number_of_photoheads.Text		= string.Format("{0}", stLuria.jobmgt.get_job_params.ph_count);
			m_edt_luria_1_photohead_pitch.Text			= string.Format("{0}", stLuria.jobmgt.get_job_params.ph_pitch);
			m_edt_luria_1_number_of_stripes.Text		= string.Format("{0}", stLuria.jobmgt.get_job_params.strips_counts);
			m_edt_luria_1_spx.Text						= string.Format("{0}", stLuria.jobmgt.get_job_params.spx);
			m_edt_luria_1_stripe_overlap.Text			= string.Format("{0:0.000} mm", stLuria.jobmgt.get_job_params.stripe_overlap/1000.0);
			m_edt_luria_1_pixel_size.Text				= string.Format("{0:0.000} um", stLuria.jobmgt.get_job_params.pixel_size/1000.0);
			m_edt_luria_1_photohead_rotated.Text		= string.Format("{0}", stLuria.jobmgt.get_job_params.ph_rotated);
			m_edt_luria_1_gerber_inverted.Text			= string.Format("{0}", stLuria.jobmgt.get_job_params.gerber_inverted);
			m_edt_luria_1_orientation_is_flipped_x.Text= string.Format("{0}", stLuria.jobmgt.get_job_params.flip[0]);
			m_edt_luria_1_orientation_is_flipped_y.Text= string.Format("{0}", stLuria.jobmgt.get_job_params.flip[1]);
			m_edt_luria_1_artwork_size_x.Text			= string.Format("{0:0.000} mm", stLuria.jobmgt.get_job_params.artwork_size[0]/1000.0);
			m_edt_luria_1_artwork_size_y.Text			= string.Format("{0:0.000} mm", stLuria.jobmgt.get_job_params.artwork_size[1]/1000.0);
			m_edt_luria_1_erosion_x.Text				= string.Format("{0:0.000} mm", stLuria.jobmgt.get_job_params.erosion[0]/1000.0);
			m_edt_luria_1_erosion_y.Text				= string.Format("{0:0.000} mm", stLuria.jobmgt.get_job_params.erosion[1]/1000.0);
			m_edt_luria_1_offset_x.Text					= string.Format("{0:0.000} mm", stLuria.jobmgt.get_job_params.offset[0]/1000.0);
			m_edt_luria_1_offset_y.Text					= string.Format("{0:0.000} mm", stLuria.jobmgt.get_job_params.offset[1]/1000.0);
			m_edt_luria_1_gerber_origin_offset_x.Text	= string.Format("{0:0.000} mm", stLuria.jobmgt.get_job_params.origin_offset[0]/1000.0);
			m_edt_luria_1_gerber_origin_offset_y.Text	= string.Format("{0:0.000} mm", stLuria.jobmgt.get_job_params.origin_offset[1]/1000.0);
			m_edt_luria_1_rotation_deg.Text				= string.Format("{0}", stLuria.jobmgt.get_job_params.rotation);
			m_edt_luria_1_product_id.Text				= string.Format("{0}", stLuria.jobmgt.get_job_params.product_id);
			m_edt_luria_1_stripe_width.Text				= string.Format("{0:0.000} um", stLuria.jobmgt.get_job_params.strip_width/1000000.0);
			m_edt_luria_1_edge_blending.Text			= string.Format("{0}", stLuria.jobmgt.get_job_params.edge_blend_enabled);
			m_edt_luria_1_extra_long_strips.Text		= string.Format("{0}", stLuria.jobmgt.get_job_params.extra_long_strips);
			m_edt_luria_1_mtc.Text						= string.Format("{0}", stLuria.jobmgt.get_job_params.mtc_mode);

			m_edt_luria_1_global_decode.Text			= string.Format("{0}", stLuria.jobmgt.selected_job_fiducial.g_d_code);
			m_edt_luria_1_local_decode.Text			= string.Format("{0}", stLuria.jobmgt.selected_job_fiducial.l_d_code);

			string strFiducial	= string.Empty;
			m_box_luria_1_global_decode.Items.Clear();
			for (i=0x0000; i<stLuria.jobmgt.selected_job_fiducial.g_coord_count; i++)
			{
				strFiducial	= string.Format("X:{0, 10:N6} Y:{1, 10:N6} mm",
											stLuria.jobmgt.selected_job_fiducial.g_coord_xy[i].x/1000000.0,
											stLuria.jobmgt.selected_job_fiducial.g_coord_xy[i].y/1000000.0);
				m_box_luria_1_global_decode.Items.Add(strFiducial);
			}
			m_box_luria_1_local_decode.Items.Clear();
			for (i=0x0000; i<stLuria.jobmgt.selected_job_fiducial.l_coord_count; i++)
			{
				strFiducial	= string.Format("X:{0, 10:N6} Y:{1, 10:N6} mm",
											stLuria.jobmgt.selected_job_fiducial.l_coord_xy[i].x/1000000.0,
											stLuria.jobmgt.selected_job_fiducial.l_coord_xy[i].y/1000000.0);
				m_box_luria_1_local_decode.Items.Add(strFiducial);
			}
		}

		/*
		 desc : Button event
		 parm : s	- [in]  Object
				e	- [in]  EventArgs
		 retn : None
		*/
		private void btnClick(object s, RoutedEventArgs e)
		{
			FrameworkElement feID = e.Source as FrameworkElement;

			if (feID.Name == "m_btn_luria_1_init_hw")									btnInitHW();
			else if (feID.Name == "m_btn_luria_1_emulate_get")							btnEmulateGet();
			else if (feID.Name == "m_btn_luria_1_emulate_set")							btnEmulateSet();
			else if (feID.Name == "m_btn_luria_1_all_photoheads_static_get")			btnAllPhotoheadsStaticGet();
			else if (feID.Name == "m_btn_luria_1_all_photoheads_static_set")			btnAllPhotoheadsStaticSet();
			else if (feID.Name == "m_btn_luria_1_all_photoheads_job_independent_get")	btnAllPhotoheadsIndependentGet();
			else if (feID.Name == "m_btn_luria_1_all_photoheads_job_independent_set")	btnAllPhotoheadsIndependentSet();
			else if (feID.Name == "m_btn_luria_1_spec_ph_get")							btnGerbSpecPhGet();
			else if (feID.Name == "m_btn_luria_1_spec_ph_set")							btnGerbSpecPhSet();
			else if (feID.Name == "m_btn_luria_1_motion_controller_get")				btnGerbMotionControllerGet();
			else if (feID.Name == "m_btn_luria_1_motion_controller_set")				btnGerbMotionControllerSet();
			else if (feID.Name == "m_btn_luria_1_table_settings_get")					btnGerbTableSettingsGet();
			else if (feID.Name == "m_btn_luria_1_table_settings_set")					btnGerbTableSettingsSet();
			else if (feID.Name == "m_btn_luria_1_root_directory_browser")				btnGerbRootDirectoryBrowser();
			else if (feID.Name == "m_btn_luria_1_root_directory_browser_get")			btnGerbRootDirectoryBrowserGet();
			else if (feID.Name == "m_btn_luria_1_root_directory_browser_set")			btnGerbRootDirectoryBrowserSet();
			else if (feID.Name == "m_btn_luria_1_artwork_complexity_get")				btnGerbArtWorkComplexityGet();
			else if (feID.Name == "m_btn_luria_1_artwork_complexity_set")				btnGerbArtWorkComplexitySet();
			else if (feID.Name == "m_btn_luria_1_gerber_file_add")						btnGerbGerberFileAdd();
			else if (feID.Name == "m_btn_luria_1_gerber_file_del")						btnGerbGerberFileDel();
			else if (feID.Name == "m_btn_luria_1_gerber_file_load")					btnGerbGerberFileLoad();
			else if (feID.Name == "m_btn_luria_1_get_jobs")							btnGerbGetJobs();
			else if (feID.Name == "m_btn_print_active_table_get")					btnPrintActiveTableGet();
			else if (feID.Name == "m_btn_print_active_table_set")					btnPrintActiveTableSet();
			else if (feID.Name == "m_btn_print_sim_dir_get")						btnPrintSimulDirGet();
			else if (feID.Name == "m_btn_print_sim_dir_set")						btnPrintSimulDirSet();
			else if (feID.Name == "m_btn_panel_copy_gbr")							btnPanelCopyFiducial();
			else if (feID.Name == "m_btn_print_run_registration")					btnPanelRunRegistration();
			else if (feID.Name == "m_btn_etc_set")									btnPanelEtcSet();
			else if (feID.Name == "m_btn_etc_get")									btnPanelEtcGet();
			else if (feID.Name == "m_btn_print_trans_get")							btnPanelTransGet();
			else if (feID.Name == "m_btn_print_trans_set")							btnPanelTransSet();
			else if (feID.Name == "m_btn_expo_param_get")							btnExpoParamGet();
			else if (feID.Name == "m_btn_expo_param_set")							btnExpoParamSet();
			else if (feID.Name == "m_btn_ampl_get")									btnOpticAmplitudeGet();
			else if (feID.Name == "m_btn_ampl_set")									btnOpticAmplitudeSet();
			else if (feID.Name == "m_btn_print_pre_print")							btnPrePrint();
			else if (feID.Name == "m_btn_print_print")								btnPrint();
			else if (feID.Name == "m_btn_print_abort")								btnPrintAbort();
			else if (feID.Name == "m_btn_print_ramp_up")							btnPrintRampUp();
		}

		private void btnPrintRampUp()
		{
		}
		private void btnPrintAbort()
		{
			STG_LDSM stLuria;
			DateTime dtNow	= DateTime.Now;

			EngLuria.uvEng_Luria_ReqSetPrintOpt(ENG_LCEP.en_abort);
			
			do {
#if (false)
				/* 장비와 실제로 연동될 때는 빼야 됨 */
				EngLuria.uvEng_Luria_ReqGetExposureState();
				if (!WaitRecv(2000))	break;
#endif
				IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
				stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));

				Thread.Sleep(100);
				if (WaitTimeout(2000, dtNow))
				{
					stLuria.exposure.get_exposure_state[0]	= (Byte)ENG_LPES.en_abort_failed;
					break;
				}

				if (stLuria.exposure.get_exposure_state[0] == (Byte)ENG_LPES.en_idle)				continue;
				if (0xA0 != (stLuria.exposure.get_exposure_state[0] & 0xA0))						continue;
				if (stLuria.exposure.get_exposure_state[0] != (Byte)ENG_LPES.en_abort_in_progress)	break;

			} while (true);

			if (stLuria.exposure.get_exposure_state[0] == (Byte)ENG_LPES.en_abort_failed)
			{
				char[] szStatus = new char[128];
				Array.Clear(szStatus, 0, 128);
				EngLuria.uvCmn_Luria_GetLuriaLastErrorDesc(szStatus, 128);
				string strStatus = new string (szStatus);
				m_edt_print_status.Text = "Abort failed: "+szStatus;
			}
			else m_edt_print_status.Text = "Abort success";
		}
		private void btnPrint()
		{
			STG_LDSM stLuria;
			DateTime dtNow	= DateTime.Now;

			EngLuria.uvEng_Luria_ReqSetPrintOpt(ENG_LCEP.en_print);
			
			m_pgr_print_rate.Value = 0.0;

			do {
#if (false)
				/* 장비와 실제로 연동될 때는 빼야 됨 */
				EngLuria.uvEng_Luria_ReqGetExposureState();
				if (!WaitRecv(2000))	break;
#endif
				IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
				stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));

				Thread.Sleep(100);
				if (WaitTimeout(2000, dtNow))
				{
					stLuria.exposure.get_exposure_state[0]	= (Byte)ENG_LPES.en_print_failed;
					break;
				}

				m_pgr_print_rate.Value	= stLuria.exposure.GetScanRate();
				if (stLuria.exposure.get_exposure_state[0] == (Byte)ENG_LPES.en_idle)			continue;
				if (0x20 != (stLuria.exposure.get_exposure_state[0] & 0x20))					continue;
				if (stLuria.exposure.get_exposure_state[0] != (Byte)ENG_LPES.en_print_running)	break;

			} while (true);

			if (stLuria.exposure.get_exposure_state[0] == (Byte)ENG_LPES.en_print_failed)
			{
				char[] szStatus = new char[128];
				Array.Clear(szStatus, 0, 128);
				EngLuria.uvCmn_Luria_GetLuriaLastErrorDesc(szStatus, 128);
				m_edt_print_status.Text = "Print failed: "+szStatus;
			}
			else m_edt_print_status.Text = "Print success";
		}
		private void btnPrePrint()
		{
			STG_LDSM stLuria;
			DateTime dtNow	= DateTime.Now;

			EngLuria.uvEng_Luria_ReqSetPrintOpt(ENG_LCEP.en_pre_print);
			
			do {
#if (false)
				/* 장비와 실제로 연동될 때는 빼야 됨 */
				EngLuria.uvEng_Luria_ReqGetExposureState();
				if (!WaitRecv(2000))	break;
#endif
				IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
				stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));

				Thread.Sleep(100);
				if (WaitTimeout(2000, dtNow))
				{
					stLuria.exposure.get_exposure_state[0]	= (Byte)ENG_LPES.en_preprint_failed;
					break;
				}

				if (stLuria.exposure.get_exposure_state[0] == (Byte)ENG_LPES.en_idle)				continue;
				if (0x10 != (stLuria.exposure.get_exposure_state[0] & 0x10))						continue;
				if (stLuria.exposure.get_exposure_state[0] != (Byte)ENG_LPES.en_preprint_running)	break;

			} while (true);

			if (stLuria.exposure.get_exposure_state[0] == (Byte)ENG_LPES.en_preprint_failed)
			{
				char[] szStatus = new char[128];
				Array.Clear(szStatus, 0, 128);
				EngLuria.uvCmn_Luria_GetLuriaLastErrorDesc(szStatus, 128);
				m_edt_print_status.Text = "Pre-print failed: "+szStatus;
			}
			else m_edt_print_status.Text = "Pre-print success";
		}

		private void btnOpticAmplitudeSet()
		{
			UInt16[,] ledAmp = new UInt16[3, 4];
			/* Get the shared memory for luria */
			IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
			STG_LDSM stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));

			if (stLuria.machine.total_ph > 0)
			{
				ledAmp[0,0] = Convert.ToUInt16(m_edt_ampl_index_1_365.Text);
				ledAmp[0,1] = Convert.ToUInt16(m_edt_ampl_index_1_385.Text);
				ledAmp[0,2] = Convert.ToUInt16(m_edt_ampl_index_1_395.Text);
				ledAmp[0,3] = Convert.ToUInt16(m_edt_ampl_index_1_405.Text);
			}
			if (stLuria.machine.total_ph > 1)
			{
				ledAmp[1,0] = Convert.ToUInt16(m_edt_ampl_index_2_365.Text);
				ledAmp[1,1] = Convert.ToUInt16(m_edt_ampl_index_2_385.Text);
				ledAmp[1,2] = Convert.ToUInt16(m_edt_ampl_index_2_395.Text);
				ledAmp[1,3] = Convert.ToUInt16(m_edt_ampl_index_2_405.Text);
			}
			if (stLuria.machine.total_ph > 2)
			{
				ledAmp[2,0] = Convert.ToUInt16(m_edt_ampl_index_3_365.Text);
				ledAmp[2,1] = Convert.ToUInt16(m_edt_ampl_index_3_385.Text);
				ledAmp[2,2] = Convert.ToUInt16(m_edt_ampl_index_3_395.Text);
				ledAmp[2,3] = Convert.ToUInt16(m_edt_ampl_index_3_405.Text);
			}

			EngLuria.uvEng_Luria_ReqSetLedAmplitude(stLuria.machine.total_ph, ledAmp);
			if (!WaitRecv(2000))	return;
		}
		private void btnOpticAmplitudeGet()
		{
			EngLuria.uvEng_Luria_ReqGetLedAmplitude();
			if (!WaitRecv(2000))	return;

			/* Get the shared memory for luria */
			IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
			STG_LDSM stLuria	= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));

			if (stLuria.machine.total_ph < 1)	return;
			m_edt_ampl_index_1_365.Text	= stLuria.exposure.led_amp[0].led_no[0].ToString();
			m_edt_ampl_index_1_385.Text	= stLuria.exposure.led_amp[0].led_no[1].ToString();
			m_edt_ampl_index_1_395.Text	= stLuria.exposure.led_amp[0].led_no[2].ToString();
			m_edt_ampl_index_1_405.Text	= stLuria.exposure.led_amp[0].led_no[3].ToString();
			if (stLuria.machine.total_ph < 2)	return;
			m_edt_ampl_index_2_365.Text	= stLuria.exposure.led_amp[1].led_no[0].ToString();
			m_edt_ampl_index_2_385.Text	= stLuria.exposure.led_amp[1].led_no[1].ToString();
			m_edt_ampl_index_2_395.Text	= stLuria.exposure.led_amp[1].led_no[2].ToString();
			m_edt_ampl_index_2_405.Text	= stLuria.exposure.led_amp[1].led_no[3].ToString();
			if (stLuria.machine.total_ph < 3)	return;
			m_edt_ampl_index_3_365.Text	= stLuria.exposure.led_amp[2].led_no[0].ToString();
			m_edt_ampl_index_3_385.Text	= stLuria.exposure.led_amp[2].led_no[1].ToString();
			m_edt_ampl_index_3_395.Text	= stLuria.exposure.led_amp[2].led_no[2].ToString();
			m_edt_ampl_index_3_405.Text	= stLuria.exposure.led_amp[2].led_no[3].ToString();
		}

		private void btnExpoParamSet()
		{
			Byte bStep	= Convert.ToByte(m_edt_expo_scroll_step_size.Text);
			if (bStep < 1)	return;
			Byte bDuty	= Convert.ToByte(m_edt_expo_led_duty_cycle.Text);
			Double dbRate=Convert.ToDouble(m_edt_expo_frame_rate.Text);
			EngLuria.uvEng_Luria_ReqSetExposureFactor(bStep, bDuty, dbRate);
			if (!WaitRecv(2000))	return;

			Byte bEnable	= m_chk_expo_enable_af_copy_last_stripe.IsChecked==true ? (Byte)0x01 : (Byte)0x00;
			Byte bDisable	= m_chk_expo_disable_af_first_stripe.IsChecked==true ? (Byte)0x01 : (Byte)0x00;
			EngLuria.uvEng_Luria_ReqSetEnableAfCopyLastStrip(bEnable);
			EngLuria.uvEng_Luria_ReqSetDisableAfFirstStrip(bDisable);
		}
		private void btnExpoParamGet()
		{
			EngLuria.uvEng_Luria_ReqGetEnableAfCopyLastStrip();
			if (!WaitRecv(2000))	return;
			EngLuria.uvEng_Luria_ReqGetDisableAfFirstStrip();
			if (!WaitRecv(2000))	return;
			EngLuria.uvEng_Luria_ReqGetExposureFactor();
			if (!WaitRecv(2000))	return;

			/* Get the shared memory for luria */
			IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
			STG_LDSM stLuria;
			stLuria	= default(STG_LDSM);
			stLuria	= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));

			m_chk_expo_enable_af_copy_last_stripe.IsChecked = stLuria.exposure.enable_af_copy_last_strip==0x01;
			m_chk_expo_disable_af_first_stripe.IsChecked = stLuria.exposure.disable_af_first_strip==0x01;
			m_edt_expo_scroll_step_size.Text = stLuria.exposure.scroll_step_size.ToString();
			m_edt_expo_led_duty_cycle.Text = stLuria.exposure.led_duty_cycle.ToString();
			m_edt_expo_frame_rate.Text = (stLuria.exposure.frame_rate_factor/1000.0).ToString();
			m_edt_expo_print_speed.Text = (stLuria.exposure.get_exposure_speed/1000.0).ToString();
		}

		private void btnPanelTransGet()
		{
			/* Get the shared memory for luria */
			IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
			STG_LDSM stLuria;

			EngLuria.uvEng_Luria_ReqGetTransformationRecipeGlobal();
			if (!WaitRecv(2000))	return;

			/* Get the shared memory for luria */
			stLuria	= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));

			if (stLuria.panel.global_transformation_recipe[0]==0x01)
			{
				EngLuria.uvEng_Luria_ReqGetGlobalFixedRotation();
				if (!WaitRecv(2000))	return;
			}
			if (stLuria.panel.global_transformation_recipe[1]==0x01)
			{
				EngLuria.uvEng_Luria_ReqGetGlobalFixedScaling();
				if (!WaitRecv(2000))	return;
			}
			if (stLuria.panel.global_transformation_recipe[2]==0x01)
			{
				EngLuria.uvEng_Luria_ReqGetGlobalFixedOffset();
				if (!WaitRecv(2000))	return;
			}

			/* Get the shared memory for luria */
			stLuria	= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));

			m_chk_print_global_fixed_rotation.IsChecked = stLuria.panel.global_transformation_recipe[0]==0x01 ? true : false;
			m_chk_print_global_fixed_scaling.IsChecked = stLuria.panel.global_transformation_recipe[1]==0x01 ? true : false;
			m_chk_print_global_fixed_offset.IsChecked = stLuria.panel.global_transformation_recipe[2]==0x01 ? true : false;

			m_edt_print_global_rotation_deg.Text= "";
			if (stLuria.panel.global_transformation_recipe[0]==(Byte)0x01)
			{
				m_edt_print_global_rotation_deg.Text	= (stLuria.panel.global_fixed_rotation/1000.0).ToString();
			}
			m_edt_print_global_scaling_x.Text	= "";
			m_edt_print_global_scaling_y.Text	= "";
			if (stLuria.panel.global_transformation_recipe[1]==(Byte)0x01)
			{
				m_edt_print_global_scaling_x.Text	= (stLuria.panel.global_fixed_scaling_xy[0]/1000000.0).ToString();
				m_edt_print_global_scaling_y.Text	= (stLuria.panel.global_fixed_scaling_xy[1]/1000000.0).ToString();
			}
			m_edt_print_global_offset_x.Text	= "";
			m_edt_print_global_offset_y.Text	= "";
			if (stLuria.panel.global_transformation_recipe[2]==(Byte)0x01)
			{
				m_edt_print_global_offset_x.Text	= (stLuria.panel.global_fixed_offset_xy[0]/1000000.0).ToString();
				m_edt_print_global_offset_y.Text	= (stLuria.panel.global_fixed_offset_xy[1]/1000000.0).ToString();
			}
		}
		private void btnPanelTransSet()
		{
			Byte bRotationG	= m_chk_print_global_fixed_rotation.IsChecked == true ? (Byte)0x01 : (Byte)0x00;
			/*Boolean bRotationL	= m_chk_print_local_fixed_rotation.IsChecked == true;*/
			Byte bScalingG	= m_chk_print_global_fixed_scaling.IsChecked == true ? (Byte)0x01 : (Byte)0x00;
			/*Boolean bScalingL	= m_chk_print_local_fixed_scaling.IsChecked == true;*/
			Byte bOffsetG	= m_chk_print_global_fixed_offset.IsChecked == true ? (Byte)0x01 : (Byte)0x00;
			/*Boolean bOffsetL	= m_chk_print_local_fixed_offset.IsChecked == true;*/

			Double dbRotationG	= 0.0;
			Double dbScalingGX	= 0.0, dbScalingGY = 0.0;
			Double dbOffsetGX	= 0.0, dbOffsetGY = 0.0;
			if (bRotationG==(Byte)0x01)
			{
				dbRotationG= Convert.ToDouble(m_edt_print_global_rotation_deg.Text);
			}
			if (bScalingG==(Byte)0x01)
			{
				dbScalingGX= Convert.ToDouble(m_edt_print_global_scaling_x.Text);
				dbScalingGY= Convert.ToDouble(m_edt_print_global_scaling_y.Text);
			}
			if (bOffsetG==(Byte)0x01)
			{
				dbOffsetGX	= Convert.ToDouble(m_edt_print_global_offset_x.Text);
				dbOffsetGY	= Convert.ToDouble(m_edt_print_global_offset_y.Text);
			}

			if (!EngLuria.uvEng_Luria_ReqSetTransformationRecipeGlobal((Byte)bRotationG, (Byte)bScalingG, (Byte)bOffsetG))	return;
			if (!WaitRecv(2000))	return;
			if (!EngLuria.uvEng_Luria_ReqSetRotationScalingOffsetGlobal(dbRotationG, dbScalingGX, dbScalingGY, dbOffsetGX, dbOffsetGY))	return;
			if (!WaitRecv(2000))	return;
		}

		private void btnPanelEtcSet()
		{
			Byte bSharedLocal	= (m_chk_print_zone_shared_local.IsChecked==true) ? (Byte)0x01 : (Byte)0x00;
			Byte bSnapLocal		= (m_chk_print_zone_snap_local.IsChecked==true) ? (Byte)0x01 : (Byte)0x00;
			Byte bGlobalLock	= (m_chk_print_global_rectangle_lock.IsChecked==true) ? (Byte)0x01 : (Byte)0x00;

			if (!EngLuria.uvEng_Luria_ReqSetSharedLocalZones(bSharedLocal))	return;
			if (!EngLuria.uvEng_Luria_ReqSetSnapToZoneMode(bSnapLocal))		return;
			if (!EngLuria.uvEng_Luria_ReqSetGlobalRectangle(bGlobalLock))	return;
		}
		private void btnPanelEtcGet()
		{
			EngLuria.uvEng_Luria_ReqGetSharedLocalZones();
			if (!WaitRecv(2000))	return;
			EngLuria.uvEng_Luria_ReqGetSnapToZoneMode();
			if (!WaitRecv(2000))	return;
			EngLuria.uvEng_Luria_ReqGetGlobalRectangle();
			if (!WaitRecv(2000))	return;
			/* Get the shared memory for luria */
			IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
			STG_LDSM stLuria;
			stLuria	= default(STG_LDSM);
			stLuria	= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));

			m_chk_print_zone_shared_local.IsChecked		= stLuria.panel.use_shared_local_zones==0x01 ? true : false;
			m_chk_print_zone_snap_local.IsChecked		= stLuria.panel.snap_to_zone_mode==0x01 ? true : false;
			m_chk_print_global_rectangle_lock.IsChecked	= stLuria.panel.global_rectangle_lock==0x01 ? true : false;
		}

		private void btnPanelRunRegistration()
		{
			StringCollection strLines = new StringCollection();
			Int32 i=0, i32Lines = m_edt_panel_registration_points.LineCount;
			if (i32Lines < 1)	return;
			for (; i<i32Lines; i++)
			{
				strLines.Add(m_edt_panel_registration_points.GetLineText(i));
			}

			Int32 i32Index;
			STG_I32XY[] stCoord = new STG_I32XY[i32Lines];
			i=0;
			foreach (string strCoord in strLines)
			{
				i32Index = strCoord.IndexOf(';');
				if (i32Index < 0)	return;

				stCoord[i].x = (Int32)Math.Round(Convert.ToDouble(strCoord.Substring(0, i32Index-1)) * 1000000.0, 0);
				stCoord[i].y = (Int32)Math.Round(Convert.ToDouble(strCoord.Substring(i32Index+1, strCoord.Length-(i32Index+2))) * 1000000.0, 0);
				i++;
			}
			/* Fixed Global */

			/* Luria 쪽에 마크 정보 등록 및 실행 후 상태 값까지 요청 */
			if (!EngLuria.uvEng_Luria_ReqSetRegistPointsAndRun((UInt16)i32Lines, ref stCoord[0]))	return;
			if (!WaitRecv(2000))	return;
			if (!EngLuria.uvEng_Luria_ReqGetRegistStatus())	return;
			if (!WaitRecv(2000))	return;
			IntPtr ptrMesg = EngLuria.uvCmn_Luria_GetLastRegistrationStatus();
			string strStatus = Marshal.PtrToStringUni(ptrMesg);
			MessageBox.Show(strStatus);
		}

		private void btnPanelCopyFiducial()
		{
			UInt16 i	= 0;
			string strFiducial	= string.Empty;
			m_edt_panel_registration_points.Text = string.Empty;
			/* Get the shared memory for luria */
			IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
			STG_LDSM stLuria;
			stLuria	= default(STG_LDSM);
			stLuria	= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));

			/* Global Fiducial */
			for (i=0x0000; i<stLuria.jobmgt.selected_job_fiducial.g_coord_count; i++)
			{
				strFiducial	= string.Format("{0, 10:N6} ; {1, 10:N6}",
											stLuria.jobmgt.selected_job_fiducial.g_coord_xy[i].x/1000000.0,
											stLuria.jobmgt.selected_job_fiducial.g_coord_xy[i].y/1000000.0);
				m_edt_panel_registration_points.AppendText(strFiducial);
				m_edt_panel_registration_points.Select(m_edt_panel_registration_points.Text.Length, 0);
				if (i+1 != stLuria.jobmgt.selected_job_fiducial.g_coord_count)	m_edt_panel_registration_points.AppendText("\n");
			}
			/* Local Fiducial */
			for (i=0x0000; i<stLuria.jobmgt.selected_job_fiducial.l_coord_count; i++)
			{
				strFiducial	= string.Format("{0, 10:N6} ; {0, 10:N6}",
											stLuria.jobmgt.selected_job_fiducial.l_coord_xy[i].x/1000000.0,
											stLuria.jobmgt.selected_job_fiducial.l_coord_xy[i].y/1000000.0);
				m_edt_panel_registration_points.AppendText(strFiducial);
				m_edt_panel_registration_points.Select(m_edt_panel_registration_points.Text.Length, 0);
				if (i+1 != stLuria.jobmgt.selected_job_fiducial.l_coord_count)	m_edt_panel_registration_points.AppendText("\n");
			}
		}

		private void btnPrintSimulDirSet()
		{
			if (!EngLuria.uvEng_Luria_ReqSetPrintSimulationOutDir(m_edt_print_sim_output_dir.Text.ToCharArray()))	return;
			if (!WaitRecv(2000))	return;
		}
		private void btnPrintSimulDirGet()
		{
			m_edt_print_sim_output_dir.Text	= string.Empty;
			if (!EngLuria.uvEng_Luria_ReqGetPrintSimulationOutDir())	return;
			if (!WaitRecv(2000))	return;

			/* Get the shared memory for luria */
			IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
			STG_LDSM stLuria;
			stLuria	= default(STG_LDSM);
			stLuria	= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));
#if (false)
			m_edt_print_sim_output_dir.Text = Encoding.Default.GetString(stLuria.machine.print_simulate_out_dir);
#else
			m_edt_print_sim_output_dir.Text = stLuria.machine.print_simulate_out_dir;
#endif
		}

		private void btnPrintActiveTableSet()
		{
			if (!EngLuria.uvEng_Luria_ReqSetActiveTable((Byte)(m_cmb_print_active_table.SelectedIndex+1)))	return;
			if (!WaitRecv(2000))	return;
		}
		private void btnPrintActiveTableGet()
		{
			m_cmb_print_active_table.SelectedIndex	= -1;
			if (!EngLuria.uvEng_Luria_ReqGetActiveTable())	return;
			if (!WaitRecv(2000))	return;

			/* Get the shared memory for luria */
			IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
			STG_LDSM stLuria;
			stLuria	= default(STG_LDSM);
			stLuria	= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));
			m_cmb_print_active_table.SelectedIndex = stLuria.machine.active_table-1;
		}

		private void btnGerbGetJobs()
		{
			m_box_luria_1_gerber_files.Items.Clear();
			EngLuria.uvEng_Luria_ReqGetJobList();
			if (!WaitRecv(2000))	return;
			
			/* Get the shared memory for luria */
			IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
			STG_LDSM stLuria;
			stLuria	= default(STG_LDSM);
			stLuria	= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));

			for (Byte i=0x00; i<stLuria.jobmgt.job_regist_count; i++)
			{
				m_box_luria_1_gerber_files.Items.Add(stLuria.jobmgt.GetJobNameIndex(i));
			}
		}
		private void btnGerbGerberFileLoad()
		{
			/* 선택된 거버 적재 */
			EngLuria.uvEng_Luria_ReqSetLoadSelectedJob();
			if (!WaitRecv(2000))	return;
		}
		private void btnGerbGerberFileDel()
		{
			/* 선택된 거버 (대상) 삭제 */
			EngLuria.uvEng_Luria_ReqSetDeleteSelectedJob();
			if (!WaitRecv(2000))	return;
			btnGerbGetJobs();
		}
		private void btnGerbGerberFileAdd()
		{
			/* 기존 발생된 에러 값 초기화 */
			EngLuria.uvCmn_Luria_ResetLastErrorStatus();

			System.Windows.Forms.FolderBrowserDialog dlgFolder = new System.Windows.Forms.FolderBrowserDialog();
			dlgFolder.Description	= "Select Gerber Path";
			dlgFolder.ShowNewFolderButton	= false;
#if (false)
			dlgFolder.SelectedPath = "c:\\gerber\\3.0.0\\tel\\gen2i"/*AppDomain.CurrentDomain.BaseDirectory*/;
#else
			dlgFolder.SelectedPath = "C:\\Gerber\\Calibration";
#endif
			if (dlgFolder.ShowDialog() != System.Windows.Forms.DialogResult.OK)	return;

			EngLuria.uvEng_Luria_ReqAddJobList(dlgFolder.SelectedPath.ToCharArray());
			if (!WaitRecv(2000))	return;
			if (EngLuria.uvCmn_Luria_IsLastError())
			{
				char[] szStatus = new char[128];
				Array.Clear(szStatus, 0, 128);
				EngLuria.uvCmn_Luria_GetLuriaLastErrorDesc(szStatus, 128);
				string strStatus = new string (szStatus);
				MessageBox.Show(strStatus);
			}
			else
			{
				btnGerbGetJobs();
			}
		}
		private void btnGerbArtWorkComplexitySet()
		{
			Byte u8Mode = (Byte)(m_cmb_luria_1_artwork_complexity.SelectedIndex);
			EngLuria.uvEng_Luria_ReqSetArtworkComplexity(u8Mode);
			if (!WaitRecv(2000))	return;
		}
		private void btnGerbArtWorkComplexityGet()
		{
			EngLuria.uvEng_Luria_ReqGetArtworkComplexity();
			EngLuria.uvEng_Luria_ReqGetGetMaxJobs();
			if (!WaitRecv(2000))	return;

			/* Get the shared memory for luria */
			IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
			STG_LDSM stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));

			m_cmb_luria_1_artwork_complexity.SelectedIndex = (Int32)stLuria.machine.artwork_complexity;
			m_edt_luria_1_max_jobs.Text = stLuria.jobmgt.max_job_regist.ToString();
		}
		private void btnGerbRootDirectoryBrowserSet()
		{
			string strDir = string.Empty;
			strDir = m_edt_luria_1_root_directory.Text;
			Int32 i32Len	= m_edt_luria_1_root_directory.Text.Length;
			char [] szDir	= strDir.ToCharArray();
			EngLuria.uvEng_Luria_ReqSetRootDirectory(m_edt_luria_1_root_directory.Text.ToCharArray(),
														(UInt32)m_edt_luria_1_root_directory.Text.Length);
			if (!WaitRecv(2000))	return;
		}
		private void btnGerbRootDirectoryBrowserGet()
		{
			m_edt_luria_1_root_directory.Text = "";
			EngLuria.uvEng_Luria_ReqGetRootDirectory();
			if (!WaitRecv(2000))	return;

			/* Get the shared memory for luria */
			IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
			STG_LDSM stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));
			//m_edt_luria_1_root_directory.Text	= Encoding.Default.GetString(stLuria.jobmgt.root_directory);
			m_edt_luria_1_root_directory.Text	= stLuria.jobmgt.root_directory;
		}
		private void btnGerbRootDirectoryBrowser()
		{
			System.Windows.Forms.FolderBrowserDialog dlgFolder = new System.Windows.Forms.FolderBrowserDialog();
			dlgFolder.Description	= "Select Gerber Root Path";
			dlgFolder.ShowNewFolderButton	= false;
			dlgFolder.SelectedPath = "c:\\gerber\\3.0.0\\tel\\gen2i"/*AppDomain.CurrentDomain.BaseDirectory*/;
//			dlgFolder.RootFolder  = Environment.SpecialFolder.DesktopDirectory;
			if (dlgFolder.ShowDialog() != System.Windows.Forms.DialogResult.OK)	return;
			m_edt_luria_1_root_directory.Text	= dlgFolder.SelectedPath;
		}
		private void btnGerbTableSettingsSet()
		{
			String strTblNo	= m_cmb_luria_1_table_set_tbl_no.SelectedItem as String;
			if (!EngLuria.uvEng_Luria_ReqSetGroupTableSettings(Convert.ToByte(strTblNo),
															   (UInt32)Math.Round(Convert.ToDouble(m_edt_luria_1_parallelogram_adjust.Text) * 1000.0, 0),
															   Convert.ToByte(m_chk_luria_1_start_print_in_pos_y_dir.IsChecked),
															   (UInt32)Math.Round(Convert.ToDouble(m_edt_luria_1_table_exposure_start_pos_x.Text) * 1000.0, 0),
															   (UInt32)Math.Round(Convert.ToDouble(m_edt_luria_1_table_exposure_start_pos_y.Text) * 1000.0, 0),
															   (Int32)Math.Round(Convert.ToDouble(m_edt_luria_1_table_position_limits_min_x.Text) * 1000.0, 0),
															   (Int32)Math.Round(Convert.ToDouble(m_edt_luria_1_table_position_limits_min_y.Text) * 1000.0, 0),
															   (Int32)Math.Round(Convert.ToDouble(m_edt_luria_1_table_position_limits_max_x.Text) * 1000.0, 0),
															   (Int32)Math.Round(Convert.ToDouble(m_edt_luria_1_table_position_limits_max_y.Text) * 1000.0, 0)))	return;
			if (!WaitRecv(2000))	return;
		}
		private void btnGerbTableSettingsGet()
		{
			String strTblNo	= m_cmb_luria_1_table_set_tbl_no.SelectedItem as String;
			Byte u8TblIndex	= (Byte)(Convert.ToByte(strTblNo) - 1);
			if (!EngLuria.uvEng_Luria_ReqGetGroupTableSettings(Convert.ToByte(strTblNo)))	return;
			if (!WaitRecv(2000))	return;

			/* Get the shared memory for luria */
			IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
			STG_LDSM stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));
			m_chk_luria_1_start_print_in_pos_y_dir.IsChecked	= Convert.ToBoolean(stLuria.machine.table_print_direction[u8TblIndex]);
			m_edt_luria_1_parallelogram_adjust.Text			= string.Format("{0:0.000}", stLuria.machine.parallelogram_adjust[u8TblIndex]/1000.0);
			m_edt_luria_1_table_exposure_start_pos_x.Text		= string.Format("{0:0.000}", stLuria.machine.table_expo_start_xy[u8TblIndex].x/1000.0);
			m_edt_luria_1_table_exposure_start_pos_y.Text		= string.Format("{0:0.000}", stLuria.machine.table_expo_start_xy[u8TblIndex].y/1000.0);
			m_edt_luria_1_table_position_limits_min_x.Text		= string.Format("{0:0.000}", stLuria.machine.table_position_limit[u8TblIndex].min_xy[0]/1000.0);
			m_edt_luria_1_table_position_limits_min_y.Text		= string.Format("{0:0.000}", stLuria.machine.table_position_limit[u8TblIndex].min_xy[1]/1000.0);
			m_edt_luria_1_table_position_limits_max_x.Text		= string.Format("{0:0.000}", stLuria.machine.table_position_limit[u8TblIndex].max_xy[0]/1000.0);
			m_edt_luria_1_table_position_limits_max_y.Text		= string.Format("{0:0.000}", stLuria.machine.table_position_limit[u8TblIndex].max_xy[1]/1000.0);
			m_edt_luria_1_table_motion_start_position_x.Text	= string.Format("{0:0.000}", stLuria.machine.get_table_motion_start_xy[u8TblIndex].x/1000.0);
			m_edt_luria_1_table_motion_start_position_y.Text	= string.Format("{0:0.000}", stLuria.machine.get_table_motion_start_xy[u8TblIndex].y/1000.0);
		}

		private void btnGerbMotionControllerSet()
		{
			String strScroll= m_cmb_luria_1_scroll_mode.SelectedItem as String;
			IPAddress IPAddr= IPAddress.Parse(m_edt_luria_1_mc2_ip_addr.Text);
			Byte[] u8IPAddr	= IPAddr.GetAddressBytes();

			if (!EngLuria.uvEng_Luria_ReqSetGroupMotionController(u8IPAddr,
																  Convert.ToUInt32(m_edt_luria_1_mc2_max_y_speed.Text)*1000,
																  Convert.ToUInt32(m_edt_luria_1_mc2_max_x_speed.Text)*1000,
																  Convert.ToUInt32(m_edt_luria_1_mc2_y_acc_dist.Text)*1000,
																  Convert.ToByte(m_edt_luria_1_mc2_x_drv_tbl_1.Text),
																  Convert.ToByte(m_edt_luria_1_mc2_y_drv_tbl_1.Text),
																  Convert.ToByte(m_edt_luria_1_mc2_x_drv_tbl_2.Text),
																  Convert.ToByte(m_edt_luria_1_mc2_y_drv_tbl_2.Text),
																  Convert.ToByte(strScroll),
																  Convert.ToUInt16(m_edt_luria_1_negative_offset.Text),
																  Convert.ToUInt32(m_edt_luria_1_delay_pos_move_dir.Text),
																  Convert.ToUInt32(m_edt_luria_1_delay_neg_move_dir.Text)))	return;
			if (!WaitRecv(2000))	return;
		}
		private void btnGerbMotionControllerGet()
		{
			String strScroll	= m_cmb_luria_1_scroll_mode.SelectedItem as String;
			if (!EngLuria.uvEng_Luria_ReqGetGroupMotionController(Convert.ToByte(strScroll)))	return;
			if (!WaitRecv(2000))	return;

			/* Get the shared memory for luria */
			IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
			STG_LDSM stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));

			m_edt_luria_1_mc2_ip_addr.Text	= string.Format("{0}.{1}.{2}.{3}", stLuria.machine.motion_control_ip[0],
																		   stLuria.machine.motion_control_ip[1],
																		   stLuria.machine.motion_control_ip[2],
																		   stLuria.machine.motion_control_ip[3]);
			m_edt_luria_1_mc2_max_y_speed.Text		= (stLuria.machine.max_y_motion_speed/1000.0).ToString();
			m_edt_luria_1_mc2_max_x_speed.Text		= (stLuria.machine.x_motion_speed/1000.0).ToString();
			m_edt_luria_1_mc2_y_acc_dist.Text		= (stLuria.machine.y_acceleration_distance/1000.0).ToString();
			m_edt_luria_1_mc2_x_drv_tbl_1.Text		= (stLuria.machine.xy_drive_id[0]/* [0, 0] */).ToString();
			m_edt_luria_1_mc2_y_drv_tbl_1.Text		= (stLuria.machine.xy_drive_id[1]/* [0, 1] */).ToString();
			m_edt_luria_1_mc2_x_drv_tbl_2.Text		= (stLuria.machine.xy_drive_id[2]/* [1, 0] */).ToString();
			m_edt_luria_1_mc2_y_drv_tbl_2.Text		= (stLuria.machine.xy_drive_id[3]/* [1, 1] */).ToString();

			m_edt_luria_1_negative_offset.Text		= (stLuria.machine.hysteresis_type1.negative_offset).ToString();
			m_edt_luria_1_delay_pos_move_dir.Text	= (stLuria.machine.hysteresis_type1.delay_positive).ToString();
			m_edt_luria_1_delay_neg_move_dir.Text	= (stLuria.machine.hysteresis_type1.delay_negative).ToString();
		}

		private void btnGerbSpecPhGet()
		{
			String strPhNum	= m_cmb_luria_1_spec_ph_num.SelectedItem as String;
			if (!EngLuria.uvEng_Luria_ReqGetGroupSpecificPhotohead(Convert.ToByte(strPhNum)))	return;
			if (!WaitRecv(2000))	return;

			/* Get the shared memory for luria */
			IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
			STG_LDSM stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));

			Byte u8PhNo	= (Byte)Convert.ToByte(strPhNum);
			m_edt_luria_1_spec_ip_addr.Text	= string.Format("{0}.{1}.{2}.{3}", stLuria.machine.ph_ip[u8PhNo-1].ipv4[0],
																			   stLuria.machine.ph_ip[u8PhNo-1].ipv4[1],
																			   stLuria.machine.ph_ip[u8PhNo-1].ipv4[2],
																			   stLuria.machine.ph_ip[u8PhNo-1].ipv4[3]);
			if (0x00 == (u8PhNo-1))
			{
				m_edt_luria_1_spec_x_pos_relative_to_ph1.Text	= "";
				m_edt_luria_1_spec_y_pos_relative_to_ph1.Text	= ""; 
			}
			else
			{
				m_edt_luria_1_spec_x_pos_relative_to_ph1.Text	= Convert.ToString(stLuria.machine.ph_offset_xy[u8PhNo-1].pos_offset_x/1000000.0);
				m_edt_luria_1_spec_y_pos_relative_to_ph1.Text	= Convert.ToString(stLuria.machine.ph_offset_xy[u8PhNo-1].pos_offset_y/1000000.0);
			}
		}

		private void btnGerbSpecPhSet()
		{
			String strPhNum	= m_cmb_luria_1_spec_ph_num.SelectedItem as String;
			IPAddress IPAddr= IPAddress.Parse(m_edt_luria_1_spec_ip_addr.Text);
			Byte[] u8IPAddr	= IPAddr.GetAddressBytes();
			UInt32 OffsetX	= (UInt32)Math.Round(Convert.ToDouble(m_edt_luria_1_spec_x_pos_relative_to_ph1.Text)*1000000.0, 0);
			Int32 OffsetY	= (Int32)Math.Round(Convert.ToDouble(m_edt_luria_1_spec_y_pos_relative_to_ph1.Text)*1000000.0, 0);

			EngLuria.uvEng_Luria_ReqSetGroupSpecificPhotohead(Convert.ToByte(strPhNum), u8IPAddr, OffsetX, OffsetY);
			if (!WaitRecv(2000))	return;
		}

		private void btnAllPhotoheadsIndependentSet()
		{
			EngLuria.uvEng_Luria_ReqSetGroupAllPhotoheadsJobIndependent(Convert.ToUInt16(m_edt_luria_1_max_scroll_rate.Text),
																		Convert.ToByte(m_chk_luria_1_enable_ethercat.IsChecked),
																		Convert.ToByte(m_chk_luria_1_enable_over_pressure_mode.IsChecked));
			if (!WaitRecv(2000))	return;
		}

		private void btnAllPhotoheadsIndependentGet()
		{
			if (!EngLuria.uvEng_Luria_ReqGetGroupAllPhotoheadsJobIndependent())	return;

			if (!WaitRecv(2000))	return;

			/* Get the shared memory for luria */
			IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
			STG_LDSM stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));

			m_edt_luria_1_max_scroll_rate.Text					= stLuria.machine.scroll_rate.ToString();
			m_chk_luria_1_enable_ethercat.IsChecked			= Convert.ToBoolean(stLuria.machine.use_ethercat_for_af);
			m_chk_luria_1_enable_over_pressure_mode.IsChecked	= Convert.ToBoolean(stLuria.machine.over_pressure_mode);
		}

		private void btnAllPhotoheadsStaticSet()
		{
			String strSpxLevel	= m_cmb_luria_1_spx_level.SelectedItem as String;
			EngLuria.uvEng_Luria_ReqSetGroupAllPhotoheadsStatic(Convert.ToByte(m_edt_luria_1_total_ph_heads.Text),
																Convert.ToByte(m_edt_luria_1_ph_pitch.Text),
																Convert.ToByte(m_chk_luria_1_rotated_photo_head.IsChecked),
																Convert.ToByte(m_cmb_luria_1_motion_controller.SelectedIndex+1),
																GetProductIndexToId(m_cmb_luria_1_pid.SelectedIndex),
																Convert.ToByte(m_cmb_luria_1_zdrive_type.SelectedIndex+1),
																Convert.ToByte(m_chk_luria_1_mtc.IsChecked),
																Convert.ToUInt16(strSpxLevel));

			if (!WaitRecv(2000))	return;
		}
		private void btnAllPhotoheadsStaticGet()
		{
			if (!EngLuria.uvEng_Luria_ReqGetGroupAllPhotoheadsStatic())	return;
			if (!WaitRecv(2000))	return;

			/* Get the shared memory for luria */
			IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
			STG_LDSM stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));

			m_edt_luria_1_total_ph_heads.Text				= stLuria.machine.total_ph.ToString();
			m_edt_luria_1_ph_pitch.Text					= stLuria.machine.ph_pitch.ToString();
			m_chk_luria_1_mtc.IsChecked					= Convert.ToBoolean(stLuria.machine.mtc_mode);
			m_chk_luria_1_rotated_photo_head.IsChecked		= Convert.ToBoolean(stLuria.machine.ph_rotate);
			m_cmb_luria_1_pid.SelectedIndex				= Convert.ToInt32(GetProductIdToIndex((ENG_LPPI)stLuria.machine.product_id));
			m_cmb_luria_1_zdrive_type.SelectedIndex		= Convert.ToInt32(stLuria.machine.z_drive_type-1);
			m_cmb_luria_1_motion_controller.SelectedIndex	= Convert.ToInt32(stLuria.machine.motion_control_type-1);
			m_cmb_luria_1_spx_level.SelectedIndex			= stLuria.machine.spx_level == 36 ? 0 : 1;

			Int32 i = 0;
			m_cmb_luria_1_spec_ph_num.Items.Clear();
			for (; i<Convert.ToInt32(m_edt_luria_1_total_ph_heads.Text); i++)
			{
				m_cmb_luria_1_spec_ph_num.Items.Add((i+1).ToString());
			}
			if (i > 0)	m_cmb_luria_1_spec_ph_num.SelectedIndex = 0;
		}

		private void btnEmulateSet()
		{
			Byte [] bSet	= new byte [4];
			bSet[0] = Convert.ToByte(m_chk_luria_1_emulate_motor_controller.IsChecked.Value);
			bSet[1] = Convert.ToByte(m_chk_luria_1_emulate_photoheads.IsChecked.Value);
			bSet[2] = Convert.ToByte(m_chk_luria_1_emulate_triggers.IsChecked.Value);
			bSet[3] = Convert.ToByte(m_chk_luria_1_debug_print.IsChecked.Value);
			EngLuria.uvEng_Luria_ReqSetSystemSettings(bSet[0], bSet[1], bSet[2], bSet[3]);
		}
		private void btnEmulateGet()
		{
			EngLuria.uvEng_Luria_ReqGetSystemSettings();
			if (!WaitRecv(2000))	return;

			/* Get the shared memory for luria */
			IntPtr ptrLuria	= EngLuria.uvEng_ShMem_GetLuria();
			STG_LDSM stLuria= (STG_LDSM)Marshal.PtrToStructure(ptrLuria, typeof(STG_LDSM));

			m_chk_luria_1_emulate_motor_controller.IsChecked	= Convert.ToBoolean(stLuria.machine.emulate_motor_controller);
			m_chk_luria_1_emulate_photoheads.IsChecked			= Convert.ToBoolean(stLuria.machine.emulate_ph);
			m_chk_luria_1_emulate_triggers.IsChecked			= Convert.ToBoolean(stLuria.machine.emulate_triggers);
			m_chk_luria_1_debug_print.IsChecked				= Convert.ToBoolean(stLuria.machine.debug_print_level);
		}

		/*
		 desc : Checkbox event
		 parm : s	- [in]  Object
				e	- [in]  EventArgs
		 retn : None
		*/
		private void chkClick(object s, RoutedEventArgs e)
		{
			FrameworkElement feID = e.Source as FrameworkElement;
		}

		private void chkEmulate(Byte type)
		{
		}

		private void btnInitHW()
		{
			EngLuria.uvEng_Luria_ReqSetHWInit();
			if (!WaitRecv(2000))	return;

			if (!EngLuria.uvCmn_Luria_IsSystemStatusError())
			{
				MessageBox.Show("Succeeded to initialized hardware");
			}
			else
			{
				char [] szMesg = new char[128];
//				szMesg[0]	= '\0';	/* 배열 내용 지움 */
				Array.Clear(szMesg, 0, 128);
				/* 상태 값 가져오기 */
				UInt16 u16Status	= EngLuria.uvCmn_Luria_GetLastErrorStatus();
				EngLuria.uvCmn_Luria_GetSystemStatusErrorMesg(szMesg, 128);
				string strMesg	= new string(szMesg);
				MessageBox.Show(strMesg);
			}
		}
	}
}
