using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

/* User-defined Namespace */
using TGen2i.Enum.Comn;
using TGen2i.Enum.MC2;

namespace TGen2i.EngineLib
{
	/* Constant value for dll library file name */
	static class Constants
	{
#if (DEBUG)
#if (TGen2i_Debug)
		public const string		dllGen2I	= "ItfcGEN2Ix64D.dll";
#else
		public const string		dllGen2I	= "ItfcGEN2ITwcx64D.dll";
#endif
#else
#if (TGen2i_Debug)
		public const string		dllGen2I	= "ItfcGEN2Ix64.dll";
#else
		public const string		dllGen2I	= "ItfcGEN2ITwcx64.dll";
#endif
#endif

		public const Byte		g_MaxLed						= 0x08;		/* Number of LEDs installed in the photohead */
		public const Byte		g_MaxMC2Drive					= 0x08;		/* maximum number of drives */
		public const Byte		g_MaxPH							= 0x08;		/* maximum number of photohead */
		public const Byte		g_MaxRegistJobList				= 8;		/* Maximum number of JOBs that can be registered */
		public const Byte		g_MaxSsdSmartData				= 16;		/* 14 ea. Fixed */
		public const Byte		g_MaxTable						= 0x08;		/* maximum number of stage (table) */

		public const Byte		g_MaxCmdFamilyID				= 0x0B;		/* Family ID 최대 개수 (0xA0 ~ 0xAB) */
		public const Byte		g_MaxCmdUserD					= 0x40;		/* Family ID 내의 최대 User ID 개수 (64 ea) */

		public const Byte		g_MaxAlignerDriveCount			= 0x03;
		public const Byte		g_MaxRobotDriveCount			= 0x04;

		public const Byte		g_MaxEFUCount					= 0x04;		/* 최대 등록 가능한 EFU 개수 */

		public const UInt16		g_MaxCorrectionTableEntry		= 200;
		public const UInt16		g_MaxGerberName					= 200;		/* 최대 Job Path Name 길이 */
		public const UInt16		g_MaxCoordXY					= 400;		/* Maximum number of Coordinations for Location */
		public const UInt16		g_MaxDCodeList					= 400;		/* Maximum number of D-Codes that can be registered in Gerber */
		public const UInt16		g_MaxGlobalMarks				= 4;		/* Maximum number of Global Marks */
		public const UInt16		g_MaxHashValueCount				= 64;		/* Maximum number of hash value characters */
		public const UInt16		g_MaxLocalFixedCount			= 200;		/* Local Fixed Rotaton / Scaling / Offset */
		public const UInt16		g_MaxLocalMarks					= 32;		/* Maximum number of Local Marks (Fiducial) */
		public const UInt16		g_MaxPathLen					= 512;
		public const UInt16		g_MaxRegistrationPoints			= (g_MaxGlobalMarks + g_MaxLocalMarks);	/* Maximum number of mark registrations inspected by the alignment camera */
		public const UInt16		g_MaxPanelSerialString			= 32;		/* Serial string max length */
		public const UInt16		g_MaxPanelTextString			= 32;		/* Text string max length */
		public const UInt16		g_MaxWarpCoordXY				= 400;		/* Maximum number of Warp Coordinates X/Y */

		public const UInt16		g_LastStringDataSize			= 1024;		/* 가장 최근에 수신된 메시지 (문자열) 값이 저장될 임시 버퍼 */
	}

	public class EngineComn
	{
		/* ------------------------------------------------------------------------------------- */
		/*                           DLL Library Interface - for System                          */
		/* ------------------------------------------------------------------------------------- */

		/* DLL Library Interface - for System */
		[DllImport("kernel32", CallingConvention = CallingConvention.Cdecl)]
		public extern static UInt64 GetTickCount64();

		/* ------------------------------------------------------------------------------------- */
		/*                          DLL Library Interface - for Common                           */
		/* ------------------------------------------------------------------------------------- */

		/* DLL Library Interface - for PODIS */
		[DllImport(Constants.dllGen2I, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
		public static extern Boolean uvEng_Init(ENG_ERVM e_mode=ENG_ERVM.en_cmps_sw);
		[DllImport(Constants.dllGen2I, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
		public static extern void uvEng_Close();
		[DllImport(Constants.dllGen2I, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
		public static extern IntPtr uvEng_GetConfig();			/* Marshal.PtrToStructure(ptrConfig, typeof(STG_CIEA)) */
		[DllImport(Constants.dllGen2I, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
		public static extern IntPtr uvEng_GetConfigTeaching();	/* Marshal.PtrToStructure(ptrConfig, typeof(STG_CTPI)) */
		[DllImport(Constants.dllGen2I, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
		public static extern UInt32 uvEng_GetShMemSizeOfStruct(ENG_EDIC dev_id);

		/*
		 desc : 프로그램이 구동 이후 실행된 총 시간 값 가져오기
		 parm : None
		 retn : 실행된 시간 값 반환 (단위: msec)
		*/
		public static UInt64 GetTimeCount64()
		{
			return GetTickCount64();
		}

		/*
		 desc : Storing Byte Array as a Structure
		 parm : source	- [in]  Source Bytes Array Buffer
				target	- [out] Target Structure Buffer
		 retrn : None
		*/
        public void ByteToStruct(byte[] source, ref object target) 
        {
            int len = Marshal.SizeOf(target);

            IntPtr ptrMem = Marshal.AllocHGlobal(len);
            Marshal.Copy(source, 0, ptrMem, len);
            target = Marshal.PtrToStructure(ptrMem, target.GetType());
            Marshal.FreeHGlobal(ptrMem);
        }

		/* 
		 desc : Convert Little Endian To Big Endian or Big Endian To Little Endian
		 parm : endian	- [in]  Swap Target Value
		 retn : None
		*/
		public static UInt16 Swap16(UInt16 endian)
		{
			return (UInt16)(((endian & 0x00ff) << 8) |
							((endian >> 8) & 0x00ff));
		}
		public static UInt32 Swap32(UInt32 endian)
		{
			return (UInt32)(((endian & 0xff000000) >> 24) |
							((endian & 0x00ff0000) >> 8)  |
							((endian & 0x0000ff00) << 8)  |
							((endian & 0x000000ff) << 24));
		}

		/* ------------------------------------------------------------------------------------- */
		/*                           DLL Library Interface - for PODIS                           */
		/* ------------------------------------------------------------------------------------- */

		/*
		 desc : Engine Init
		 parm : None
		 retn : ture or false
		*/
		public Boolean RunEngine()
		{
			var exePath	= Environment.CurrentDirectory;
			return uvEng_Init();
		}

		/*
		 desc : Engine Close
		 parm : None
		 retn : None
		*/
		public void StopEngine()
		{
			uvEng_Close();
		}

	}
}
