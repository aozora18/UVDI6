using System;
using System.Collections.Generic;
using System.IO;
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

using TGen2i.Struct.Comn;
using TGen2i.EngineLib;

namespace TGen2i.MenuChild
{
    public partial class MenuLogs : MenuBase
    {
        public MenuLogs()
        {
			/* Initialize internal components. */
			InitializeComponent();

			/* Window Event */

			/* Control Event for Button */
			m_btn_logs_files_get.Click				+= btnClick;
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
		}

		/*
		 desc : Inited control
		 parm : None
		 retn : None
		*/
		private void InitCtrl()
		{
		}

		/* ------------------------------------------------------------------------------------- */
		/*                                 Windows Event Function                                */
		/* ------------------------------------------------------------------------------------- */

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

			if (feID.Name == "m_btn_logs_files_get")							btnLogFilesGet();
		}

		private void btnLogFilesGet()
		{
			string strCur = Directory.GetCurrentDirectory();
			strCur	+= "\\logs";
			DirectoryInfo dirInfo	= new DirectoryInfo(strCur);
			m_box_logs_files.Items.Clear();

			foreach (FileInfo fi in dirInfo.GetFiles())
			{
				if (fi.Extension.ToLower().CompareTo(".log") == 0)
				{
					String strFileOnly	= fi.Name.Substring(0, fi.Name.Length - 4 /*extension*/);
					m_box_logs_files.Items.Add(strFileOnly);
				}
			}
		}
	}
}
