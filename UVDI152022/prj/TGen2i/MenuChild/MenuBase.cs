using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

/* User-defined Namespace */
using TGen2i.EngineLib;

namespace TGen2i.MenuChild
{
	public class MenuBase : Window
	{
		/* private members */
		private Boolean		bInitCompleted	= false;
		private UInt64		m_u64UpdateTime	= 0;	/* unit: msec */
		private UInt64		m_u64CalledTime	= 0;	/* unit: msec */

		/* public members */

		/*
		 desc : Constructor
		 parm : None
		 retn : None
		*/
		public MenuBase()
		{
		}

		/* ------------------------------------------------------------------------------------- */
		/*                            User-defined Override Function                             */
		/* ------------------------------------------------------------------------------------- */

		/*
		 desc : Updates the control in the window
		 parm : None
		 retn : None
		*/
		public virtual void UpdateControl()
		{
			/* Updates the most recently called time */
			m_u64CalledTime	= EngineComn.GetTimeCount64();
		}

		/*
		 desc : Updates the most recently called time
		 parm : None
		 retn : None
		*/
		protected virtual void UpdateCalledTime()
		{
			/* Updates the most recently called time */
			m_u64UpdateTime	= m_u64CalledTime;
		}

		/* ------------------------------------------------------------------------------------- */
		/*                                  User Defined Function                                */
		/* ------------------------------------------------------------------------------------- */

		/*
		 desc : Set windows' initial operation to be completed
		 parm : None
		 retn : None
		*/
		protected void InitCompleted()		{	bInitCompleted	= true;	}

		/*
		 desc : Whether the initial operation of the window has been completed
		 parm : None
		 retn : true (completed) or false (Not complete)
		*/
		protected Boolean IsInitCompleted()	{	return bInitCompleted;	}

		/*
		 desc : Whether it waited for a given amount of time
		 parm : wait	- [in]  Waiting time (unit: msec)
		 retn : true (waited) or false (yet)
		*/
		protected Boolean IsWaitedTime(UInt64 wait)
		{
			return ((m_u64UpdateTime + wait) < EngineComn.GetTimeCount64());
		}
	}
}
