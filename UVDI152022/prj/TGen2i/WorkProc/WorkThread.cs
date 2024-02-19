using System;
using System.ComponentModel;
using System.Threading;
using System.Windows.Threading;
#if (DEBUG)
using System.Diagnostics;
#endif

/* User-defined Namespace */
using TGen2i.EngineLib;
using TGen2i.Enum.MC2;
using TGen2i.Enum.PLC;
using TGen2i.MenuChild;

namespace TGen2i.WorkProc
{
	class WorkThread
	{
		private MainWindow m_csMainWnd	= null;
		private EngMC2 m_csMC2		= new EngMC2();
		private EngPLC m_csPLC		= new EngPLC();

		/* Thread Update Function (thread_update_function_id) */
		public enum EN_TUFI : uint
		{
			en_motor_pos	= 0x01,		/* Motor Position 갱신 함수 호출 */
			en_plc_data		= 0x02,		/* PLC Value 갱신 함수 호출 */
		}

		/*
		 desc : Constructor
		 parm : parent	- [in]  Parent Class Object
		 retn : None
		*/
		public WorkThread(MainWindow parent)
		{
			m_csMainWnd = parent;
		}

		/*
		 desc : A function that is called repeatedly in a thread
		 parm : s	- [in]  Sender Object
				e	- [in]  Event Arguments
		 retn : None
		*/
		public void DoWork(object s, DoWorkEventArgs e)
		{
//			Int32 i32Period	= 0;
			UInt64 u64Time	= 0, u64Prev = EngineComn.GetTimeCount64();
			BackgroundWorker bgWorker = (BackgroundWorker)s;

			do	{

				/* Check if there is a job cancellation request */
				if (bgWorker.CancellationPending)
				{
					e.Cancel = true;
					break;
				}

				/* Get the running time of the current function */
				u64Time = EngineComn.GetTimeCount64();
				/* Update Period (period : 0.200 seconds) */
				if (u64Time > (u64Prev + 200))
				{
//					i32Period	= (Int32)(u64Time - u64Prev);
					/* Set the last time */
					u64Prev	= u64Time;
#if (false)
					/* Called to ProgressChangedEvent Function */
					bgWorker.ReportProgress(i32Period);
#endif
					m_csMainWnd.Dispatcher.BeginInvoke(DispatcherPriority.Normal,
						(ThreadStart)delegate()
						{
							m_csMainWnd.UpdateControl();
							if (null != m_csMainWnd.GetMenuChild())
							{
								m_csMainWnd.GetMenuChild().UpdateControl();
							}
						});
				}


			} while (true);
		}
#if (false)
		/*
		 desc : Updates the work progress 
		 parm : s	- [in]  Object
				e	- [in]  Event Arguments
		 retn : None
		*/
		public void Changed(object s, ProgressChangedEventArgs e)
		{
			MenuBase menuChild = m_csMainWnd.GetMenuChild();
			/* Updates the window's controls */
			if (null != menuChild)	menuChild.UpdateControl();
		}
#endif
		/*
		 desc : Update Job completion
		 parm : s	- [in]  Object
				e	- [in]  Event Arguments
		 retn : None
		*/
		public void Completed(object s, RunWorkerCompletedEventArgs e)
		{
			try
			{
				if (e.Error != null)    /* Exception raised */
				{
				}
				else
				{
					if (e.Cancelled == true)    /* When a job is forcibly canceled */
					{
					}
					else    /* If the operation is completed normally */
					{
					}
				}
			}
			catch (Exception ex)
			{
				throw new Exception ("An error occurred in thread termination", ex);
			}
			finally
			{
			}
		}

		/* ------------------------------------------------------------------------------------- */
		/*                           User Defined Function for public                            */
		/* ------------------------------------------------------------------------------------- */

	}
}
