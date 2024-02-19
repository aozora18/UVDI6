using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Forms;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;
using System.Runtime.InteropServices;
#if (false)
using System.Windows.Threading;
#else
using System.ComponentModel;
#endif
#if (DEBUG)
using System.Diagnostics;
#endif

/* User-defined Namespace */
using TGen2i.EngineLib;
using TGen2i.WorkProc;
using TGen2i.Enum.Comn;
using TGen2i.Enum.MC2;
using TGen2i.Enum.PLC;
using TGen2i.Enum.AppEnum;

using TGen2i.MenuChild;

namespace TGen2i
{
	public partial class MainWindow : Window
	{
		/* private members */
		private EngineComn m_csEngine		= new EngineComn();
		private WorkThread m_csWorkThread	= null;
		private BackgroundWorker m_csWorker	= null;
		private MenuBase m_wndMenuJob		= null;
		private DateTime m_dtNow			= DateTime.Now;

		/*
		 desc : Constructor
		 para : None
		 retn : None
		*/
		public MainWindow()
		{
			/* Initialize internal components. */
			InitializeComponent();

			/* Window Event */
			this.MouseDown						+= wndMouseDown;
			this.SizeChanged					+= new SizeChangedEventHandler(wndSizeChanged);
			this.StateChanged					+= new EventHandler(wndStageChanged);
			this.LocationChanged				+= new EventHandler(wndLocationChanged);
			/* Control Event */
 			this.main_btn_menu_exit.Click		+= btnClickAppExit;
			this.main_btn_menu_work.Click		+= btnClickMenuJob;
			this.main_btn_menu_luria_1.Click	+= btnClickMenuJob;
			this.main_btn_menu_luria_2.Click	+= btnClickMenuJob;
			this.main_btn_menu_mc2.Click		+= btnClickMenuJob;
			this.main_btn_menu_plc.Click		+= btnClickMenuJob;
			this.main_btn_menu_robot.Click		+= btnClickMenuJob;
			this.main_btn_menu_aligner.Click	+= btnClickMenuJob;
			this.main_btn_menu_logs.Click		+= btnClickMenuJob;
			this.main_btn_menu_config.Click		+= btnClickMenuJob;
			this.main_btn_menu_efu.Click		+= btnClickMenuJob;
		}

		/* ------------------------------------------------------------------------------------- */
		/*                                   Override Function                                   */
		/* ------------------------------------------------------------------------------------- */

		/*
		 desc : It is called once for the first time after creating a window.
		 para : e	- [in]  Event Arguments
		 retn : None
		*/
		protected override void OnInitialized(EventArgs e)
		{
			/* Virtual functions must be called */
			base.OnInitialized(e);

			/* PODIS Engine 시작 */
			if (m_csEngine.RunEngine())
			{
				RunMainThread();
			}
		}

		/*
		 desc : When the window closes, it is called last time.
		 para : e	- [in]  Event Arguments
		 retn : None
		*/
		protected override void OnClosed(EventArgs e)
		{
			/* Main Thread 종료 */
			StopMainThread();
			/* PODIS Engine 종료 */
			m_csEngine.StopEngine();

			/* Virtual functions must be called */
			base.OnClosed(e);
		}

		private void onLoaded(object sender, RoutedEventArgs e)
		{
			if (System.Windows.Forms.SystemInformation.MonitorCount > 1)
			{
				System.Drawing.Rectangle secondaryScreenRectangle = System.Windows.Forms.Screen.AllScreens[0].WorkingArea;

				this.WindowStartupLocation = WindowStartupLocation.Manual;
				this.Left		= secondaryScreenRectangle.Right - this.Width;
				this.Top		= secondaryScreenRectangle.Bottom- this.Height;
//				this.Width		= secondaryScreenRectangle.Width;
//				this.Height		= secondaryScreenRectangle.Height;
// 				this.WindowState = WindowState.Maximized;
			}
		}

		/* ------------------------------------------------------------------------------------- */
		/*                                 Windows Event Function                                */
		/* ------------------------------------------------------------------------------------- */

		/*
		 desc : Mouse Click Event
		 para : s	- [in]  Object
				e	- [in]  MouseButtonEventArgs (Event data related to mouse button)
		 retn : None
		*/
		private void wndMouseDown(object s, MouseButtonEventArgs e)
		{
#if (DEBUG)
			/* When the release mode, the main window should not move */
			if (e.LeftButton == MouseButtonState.Pressed)	DragMove();
#endif
		}

		/*
		 desc : Event occurs when the size of window changes.
		 para : s	- [in]  Object
				e	- [in]  SizeChangedEventArgs
		 retn : None
		*/
		private void wndSizeChanged(object s, SizeChangedEventArgs e)
		{
		}

		/*
		 desc : Event occurs when the state of window changes.
		 para : s	- [in]  Object
				e	- [in]  SizeChangedEventArgs
		 retn : None
		*/
		private void wndStageChanged(object s, EventArgs e)
		{
		}

		/*
		 desc : Event occurs when the location of window changes.
		 para : s	- [in]  Object
				e	- [in]  SizeChangedEventArgs
		 retn : None
		*/
		private void wndLocationChanged(object s, EventArgs e)
		{
			/* Change the location of child's window. */
			ChangeLocationChildWindow();
		}

		/* ------------------------------------------------------------------------------------- */
		/*                                 Control Event Function                                */
		/* ------------------------------------------------------------------------------------- */

		/*
		 desc : Exit the program.
		 para : None
		 retn : None
		*/
		private void btnClickAppExit(object s, EventArgs e)
		{
			/* Remove the existing window. */
			RemoveChildWindow();
			/* Close the application. */
			this.Close();
		}

		/*
		 desc : Go to the menu screen for <work>,<recipe>,<mc2>,<plc>,<robot>,<aligner>,<logs>,<config>
		 para : None
		 retn : None
		*/
		private void btnClickMenuJob(object s, EventArgs e)
		{
			CreateChildWindow(((System.Windows.Controls.Button)s).Name);
		}

		/* ------------------------------------------------------------------------------------- */
		/*                           User Defined Function for private                           */
		/* ------------------------------------------------------------------------------------- */

		private static string PtrToStringUtf8(IntPtr ptr) // aPtr is nul-terminated
		{
			if (ptr == IntPtr.Zero)
				return "";
			int len = 0;
			while (System.Runtime.InteropServices.Marshal.ReadByte(ptr, len) != 0)
				len++;
			if (len == 0)
				return "";
			byte[] array = new byte[len];
			System.Runtime.InteropServices.Marshal.Copy(ptr, array, 0, len);
			return System.Text.Encoding.UTF8.GetString(array);
		}
		/*
		 desc : Updates the control in the window
		 parm : None
		 retn : None
		*/
		public void UpdateControl()
		{
			DateTime dtNow	= DateTime.Now;

			if (m_dtNow.Ticks+500 > dtNow.Ticks)	return;
			m_dtNow = dtNow;

			/* 로그 호출 */
			IntPtr ptrErrMesg	= EngLogs.uvCmn_Logs_GetErrorMesgLast();
			/*String strErrMesg	= PtrToStringUtf8(ptrErrMesg);*/
 			String strErrMesg	= Marshal.PtrToStringUni(ptrErrMesg);
 			if (0 == m_edt_main_logs.Text.CompareTo(strErrMesg))	return;
 			m_edt_main_logs.Text= strErrMesg;
		}

		/*
		 desc : Create a new child window
		 para : name	- [in]  The name of button control.
		 retn : None
		*/
		private void CreateChildWindow(string name)
		{
			/* Compare whether it is the same windows. */
			if (IsEqualWindow(name))	return;
			/* Remove the existing window. */
			RemoveChildWindow();
			/* Create an object of the child windows in the parent window */
			switch (name)
			{
			case "main_btn_menu_work"	: m_wndMenuJob = new MenuWork();	break;
			case "main_btn_menu_luria_1": m_wndMenuJob = new MenuLuria1();	break;
			case "main_btn_menu_luria_2": m_wndMenuJob = new MenuLuria2();	break;
			case "main_btn_menu_mc2"	: m_wndMenuJob = new MenuMC2();		break;
			case "main_btn_menu_plc"	: m_wndMenuJob = new MenuPLC();		break;
			case "main_btn_menu_robot"	: m_wndMenuJob = new MenuMDR();		break;
			case "main_btn_menu_aligner": m_wndMenuJob = new MenuMPA();		break;
			case "main_btn_menu_logs"	: m_wndMenuJob = new MenuLogs();	break;
			case "main_btn_menu_config"	: m_wndMenuJob = new MenuConf();	break;
			case "main_btn_menu_efu"	: m_wndMenuJob = new MenuEFU();		break;
			}

			/* Validates the child window object */
			if (null == m_wndMenuJob)	return;
			/* Connect the object of the parent window to the child window */
			m_wndMenuJob.Owner			= this;
			m_wndMenuJob.ShowInTaskbar	= false;
			/* Move the child window to the parent window area */
			ChangeLocationChildWindow();
			/* Move the window and output it on the screen. */
			m_wndMenuJob.Show();
		}

		/*
		 desc : If there is an existing windows, remove it.
		 para : None
		 retn : None
		*/
		private void RemoveChildWindow()
		{
			if (m_wndMenuJob != null)
			{
				m_wndMenuJob.Close();
				m_wndMenuJob = null;
			}
		}

		/*
		 desc : Compare whether it is the same window.
		 para : name	- [in]  The name of button control.
		 retn : true or false
		*/
		private Boolean IsEqualWindow(string name)
		{
			Boolean bFinded	= false;

			if (null == m_wndMenuJob)	return false;

			switch (name)
			{
			case "main_btn_menu_work"	: bFinded = (m_wndMenuJob.Name == "Menu_Work");		break;
			case "main_btn_menu_luria_1": bFinded = (m_wndMenuJob.Name == "Menu_Luria1");	break;
			case "main_btn_menu_luria_2": bFinded = (m_wndMenuJob.Name == "Menu_Luria2");	break;
			case "main_btn_menu_mc2"	: bFinded = (m_wndMenuJob.Name == "Menu_MC2");		break;
			case "main_btn_menu_plc"	: bFinded = (m_wndMenuJob.Name == "Menu_PLC");		break;
			case "main_btn_menu_robot"	: bFinded = (m_wndMenuJob.Name == "Menu_MDR");		break;
			case "main_btn_menu_aligner": bFinded = (m_wndMenuJob.Name == "Menu_MPA");		break;
			case "main_btn_menu_logs"	: bFinded = (m_wndMenuJob.Name == "Menu_Logs");		break;
			case "main_btn_menu_config"	: bFinded = (m_wndMenuJob.Name == "Menu_Conf");		break;
			case "main_btn_menu_efu"	: bFinded = (m_wndMenuJob.Name == "Menu_EFU");		break;
			}

			return bFinded;
		}

		/*
		 desc : Change the child window to a specific location in the parent window.
		 para : None
		 retn : None
		*/
		private void ChangeLocationChildWindow()
		{
			/* Validates the child window object */
			if (null == m_wndMenuJob)	return;

			m_wndMenuJob.Left	= this.Left+ 15;
			m_wndMenuJob.Top	= this.Top + 50;
		}

		/*
		 desc : Regist and stat the thread
		 para : None
		 retn : true or false
		*/
		private bool RunMainThread()
		{
			/* Create thread object */
			m_csWorkThread	= new WorkThread(this);

			/* Reporting progress after thread creation (allocation) and setting cancelable function */
			m_csWorker	= new BackgroundWorker();
//			m_csWorker.WorkerReportsProgress		= true;	/* Progress Report (O) */
			m_csWorker.WorkerSupportsCancellation	= true;	/* Cancelable (O) */
			/* Specify an event handler */
			m_csWorker.DoWork				+= new DoWorkEventHandler(m_csWorkThread.DoWork);
//			m_csWorker.ProgressChanged		+= new ProgressChangedEventHandler(m_csWorkThread.Changed);
			m_csWorker.RunWorkerCompleted	+= new RunWorkerCompletedEventHandler(m_csWorkThread.Completed);
			/* Background Thread 실행 */
			m_csWorker.RunWorkerAsync();

			return true;
		}

		/*
		 desc : Stop and remove the thread
		 para : None
		 retn : None
		*/
		private void StopMainThread()
		{
			/* Stop the Main Thread */
			m_csWorker.CancelAsync();
			/* Check whether a thread is gracefully terminated */
			while (m_csWorker.IsBusy)	WaitProc(100);
		}

		/*
		 desc : UI thread task delay without stopping (meaning different from Sleep)
		 para : delay	- [in]  지연 시간 (단위: msec)
		 retn : None
		*/
		private void WaitProc(Int32 delay)
		{
			DateTime dtNow	= DateTime.Now;
			TimeSpan tsDur	= new TimeSpan(0, 0, 0, 0, delay);
			DateTime dtAdd	= dtNow.Add(tsDur);
			while (dtAdd >= dtNow)
			{
				/* Append the reference event : System.Windows.Forms (C:\Program Files (x86)\Reference Assemblies\Microsoft\Framework\.NETFramework\v4.8) */
				System.Windows.Forms.Application.DoEvents();
				/* Set the current time */
				dtNow = DateTime.Now;
			}
		}

		/*
		 desc : PLC - Green Lamp <ON> or <OFF>
		 para : None
		 retn : None
		*/
		private void btnClickPlcLampGreenOn(object s, EventArgs e)
		{
			EngPLC.uvEng_MCQ_WriteBitsValueEx(ENG_PIOA.en_tower_green_lamp_cmd, 0x01);
		}
		private void btnClickPlcLampGreenOff(object s, EventArgs e)
		{
			EngPLC.uvEng_MCQ_WriteBitsValueEx(ENG_PIOA.en_tower_green_lamp_cmd, 0x00);
		}
		/*
		 desc : PLC - Lamp Buzzer <ON> or <OFF>
		 para : None
		 retn : None
		*/
		private void btnClickPlcLampBuzzerOn(object s, EventArgs e)
		{
			EngPLC.uvEng_MCQ_WriteBitsValueEx(ENG_PIOA.en_tower_buzzer_1_cmd, 0x01);
		}
		private void btnClickPlcLampBuzzerOff(object s, EventArgs e)
		{
			EngPLC.uvEng_MCQ_WriteBitsValueEx(ENG_PIOA.en_tower_buzzer_1_cmd, 0x00);
		}

		/* ------------------------------------------------------------------------------------- */
		/*                           User Defined Function for public                            */
		/* ------------------------------------------------------------------------------------- */

		/*
		 desc : Returns the comon object of child windows
		 para : None
		 retn : MenuBase Object (reference)
		*/
		public MenuBase GetMenuChild()
		{
			return m_wndMenuJob;
		}
	}
}
