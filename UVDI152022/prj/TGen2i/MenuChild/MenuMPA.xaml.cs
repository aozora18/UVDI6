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
using TGen2i.Enum.MPA;

namespace TGen2i.MenuChild
{

	public partial class MenuMPA : MenuBase
	{
		String m_strLastCmd = string.Empty;

		public MenuMPA()
		{
			/* Initialize internal components. */
			InitializeComponent();

			/* Window Event */

			/* Control Event for Button */
			m_btn_mpa_alignment.Click += btnClick;
			m_btn_mpa_sta_get.Click += btnClick;
			m_btn_mpa_mcpo_get.Click += btnClick;
			m_btn_mpa_last_cmd_clear.Click += btnClick;
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

		private void IsRecvAckData(UInt32 wait_sec=10)
		{
			UInt32 u32WaitTime = wait_sec * 1000;
			Stopwatch sw = new Stopwatch();
			sw.Start();

			while (true)
			{
				if (EngMPA.uvCmn_MPA_IsLastSendAckRecv() &&
					EngMPA.uvCmn_MPA_IsRecvCmdLast())
					break;
				Thread.Sleep(100);

				if (sw.ElapsedMilliseconds > u32WaitTime)
				{
					MessageBox.Show("wait time-out");
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
			String rCmd = String.Empty;
			if (!EngMPA.uvCmn_MPA_GetRecvCmdLast(ref rCmd, 1024))	return;

			/* 이전에 수신된 명령어와 동일한지 여부 */
			if (m_strLastCmd.CompareTo(rCmd) != 0)
			{
				/* 가장 최근 수신된 명령어 저장 */
				m_strLastCmd = rCmd;

				/* 텍스트 박스 갱신 */
				m_edt_mpa_last_cmd.Text	+= rCmd + "\r\n";
			}
		}

		/*
		 desc : Inited control
		 parm : None
		 retn : None
		*/
		private void InitCtrl()
		{
			m_cmb_mpa_orient_notch.Items.Add("0 : Use");
			m_cmb_mpa_orient_notch.Items.Add("1 : Not use");
			m_cmb_mpa_orient_notch.SelectedIndex = 0;
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

			if (feID.Name == "m_btn_mpa_alignment")		btnSetAlignment();
			else if (feID.Name == "m_btn_mpa_sta_get")	btnGetStatus();
			else if (feID.Name == "m_btn_mpa_mcpo_get")	btnGetAxisPos();
			else if (feID.Name == "m_btn_mpa_last_cmd_clear")	btnLastCmdClear();
		}

		private void btnLastCmdClear()
		{
			m_edt_mpa_last_cmd.Clear();
		}

		private void btnGetAxisPos()
		{
			EngMPA.uvEng_MPA_GetAxisPos();
			IsRecvAckData(5);
		}

		private void btnGetStatus()
		{
			EngMPA.uvEng_MPA_GetSystemStatus();
			IsRecvAckData(5);
		}

		private void btnSetAlignment()
		{
			EngMPA.uvEng_MPA_FindNotchAngle();
			IsRecvAckData(20);
		}

		private void m_edt_mpa_last_cmd_TextChanged(object sender, TextChangedEventArgs e)
		{
			m_edt_mpa_last_cmd.SelectionStart	= m_edt_mpa_last_cmd.Text.Length;
			m_edt_mpa_last_cmd.ScrollToEnd();
		}
	}
}
