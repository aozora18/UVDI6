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
using System.Threading;

using System.Diagnostics;
using System.Runtime.InteropServices;

/* User-defined Namespace */
using TGen2i.Enum.EFU;
using TGen2i.Struct.EFU;
using TGen2i.EngineLib;

namespace TGen2i.MenuChild
{
	public partial class MenuEFU : MenuBase
	{
		public MenuEFU()
		{
			/* Initialize internal components. */
			InitializeComponent();

			m_chk_efu_set_enable.IsChecked	= true;

			/* Window Event */

			/* Control Event for Button */
			m_btn_efu_rpm_set.Click		+= btnClick;
			m_btn_efu_rpm_get_all.Click	+= btnClick;

			m_chk_efu_set_enable.Click	+= chkClick;
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
			bool bChecked = (bool)m_chk_efu_set_enable.IsChecked;
			if (bChecked == false)	return;

			IntPtr ptrEFU	= EngEFU.uvEng_ShMem_GetEFU();
#if (true)
#if (true)
			STG_APAS stEFU	= (STG_APAS)Marshal.PtrToStructure(ptrEFU, typeof(STG_APAS));
			m_edt_efu_proc_val1.Text	= stEFU.efu_pas[0].pv.ToString();
			m_edt_efu_proc_val2.Text	= stEFU.efu_pas[1].pv.ToString();
			m_edt_efu_proc_val3.Text	= stEFU.efu_pas[2].pv.ToString();

			m_edt_efu_alarm_val1.Text	= stEFU.efu_pas[0].alarm.ToString();
			m_edt_efu_alarm_val2.Text	= stEFU.efu_pas[1].alarm.ToString();
			m_edt_efu_alarm_val3.Text	= stEFU.efu_pas[2].alarm.ToString();

			m_edt_efu_set_val1.Text		= stEFU.efu_pas[0].sv.ToString();
			m_edt_efu_set_val2.Text		= stEFU.efu_pas[1].sv.ToString();
			m_edt_efu_set_val3.Text		= stEFU.efu_pas[2].sv.ToString();
#else
			STG_EPAS strEFU = (STG_EPAS)Marshal.PtrToStructure(ptrEFU, typeof(STG_EPAS));
			m_edt_efu_proc_val1.Text	= stEFU[0].pv.ToString();
			m_edt_efu_proc_val2.Text	= stEFU[1].pv.ToString();
			m_edt_efu_proc_val3.Text	= stEFU[2].pv.ToString();
#endif
#else			
			for (Int32 i=0; i<3; i++)
			{
				IntPtr stShift = IntPtr.Add(ptrEFU, Marshal.SizeOf(typeof(STG_EPAS)) * i);
				STG_EPAS stEFU = (STG_EPAS)Marshal.PtrToStructure(stShift, typeof(STG_EPAS));
				switch (i)
				{
				case 0:
					m_edt_efu_proc_val1.Text	= stEFU.pv.ToString();
					m_edt_efu_alarm_val1.Text	= stEFU.alarm.ToString();
					m_edt_efu_set_val1.Text		= stEFU.sv.ToString();
					break;
				case 1:
					m_edt_efu_proc_val2.Text	= stEFU.pv.ToString();
					m_edt_efu_alarm_val2.Text	= stEFU.alarm.ToString();
					m_edt_efu_set_val2.Text		= stEFU.sv.ToString();
					break;
				case 2:
					m_edt_efu_proc_val3.Text	= stEFU.pv.ToString();
					m_edt_efu_alarm_val3.Text	= stEFU.alarm.ToString();
					m_edt_efu_set_val3.Text		= stEFU.sv.ToString();
					break;
				}
			}
#endif
		}

		/*
		 desc : Inited control
		 parm : None
		 retn : None
		*/
		private void InitCtrl()
		{
			m_cmb_efu_bl500_id.Items.Add("BL500 1");
			m_cmb_efu_bl500_id.Items.Add("BL500 2");
			m_cmb_efu_bl500_id.Items.Add("BL500 3");
			m_cmb_efu_bl500_id.Items.Add("BL500 1 / 2");
			m_cmb_efu_bl500_id.Items.Add("BL500 2 / 3");
			m_cmb_efu_bl500_id.Items.Add("BL500 1 / 3");
			m_cmb_efu_bl500_id.Items.Add("BL500 1 / 2 / 3");
			m_cmb_efu_bl500_id.SelectedIndex	= 0;
		}

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

			if (feID.Name == "m_btn_efu_rpm_set")			btnRpmSet();
			else if (feID.Name == "m_btn_efu_rpm_get_all")	btnRpmGetAll();
		}
		private void btnRpmSet()
		{
			Byte blID	= (Byte)(m_cmb_efu_bl500_id.SelectedIndex + 1);
			Byte bSpeed	= Byte.Parse(m_edt_efu_rpm_set.Text);

			if (m_chk_efu_set_enable.IsChecked == true)
			{
				MessageBox.Show("Currently in monitoring mode.");
				return;
			}
			EngEFU.uvEng_EFU_ReqSetSpeed(blID, bSpeed);
			while (!EngEFU.uvEng_EFU_IsSendIdle())
			{
				Thread.Sleep(100);
			}
		}
		private void btnRpmGetAll()
		{
			Byte u8Speed = 0;
			EngEFU.uvEng_EFU_ReqGetState();
			while (!EngEFU.uvEng_EFU_IsSendIdle())
			{
				Thread.Sleep(100);
			}
			u8Speed = EngEFU.uvEng_EFU_GetDataValue(1, ENG_SEBT.en_pv);
			m_edt_efu_rpm_get_1.Text	= u8Speed.ToString();
			u8Speed = EngEFU.uvEng_EFU_GetDataValue(2, ENG_SEBT.en_pv);
			m_edt_efu_rpm_get_2.Text	= u8Speed.ToString();
			u8Speed = EngEFU.uvEng_EFU_GetDataValue(3, ENG_SEBT.en_pv);
			m_edt_efu_rpm_get_3.Text	= u8Speed.ToString();
		}

		private void chkClick(object s, RoutedEventArgs e)
		{
			FrameworkElement feID = e.Source as FrameworkElement;

			if (feID.Name == "m_chk_efu_set_enable")	chkSetEnableStatus();
		}

		private void chkSetEnableStatus()
		{
			bool bChecked = (bool)m_chk_efu_set_enable.IsChecked;
			EngEFU.uvEng_EFU_SetPeriodState(bChecked);
		}
	}
}
