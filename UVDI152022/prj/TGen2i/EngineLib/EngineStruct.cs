using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Net;

using TGen2i.EngineLib;
using TGen2i.Utility;
using TGen2i.Enum.Luria;
using TGen2i.Enum.MDR;
using TGen2i.Enum.MPA;
using TGen2i.Struct.Comn;

/* --------------------------------------------------------------------------------------------- */
/*                                       Engine -> Common                                        */
/* --------------------------------------------------------------------------------------------- */

namespace TGen2i.Struct.Comn
{
	/* Communication link status information */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_DLSM	/* device_link_shared_memory */
	{
		public Byte		is_connected;		/* Communication connection or not */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 7)]
		public Byte[]	u8_reserved;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]
		public UInt16[]	link_time;			/* Store communication connected time (year-month-day hour-minute-second) */
		public UInt16	u16_error_code;		/* PLC : Complete Code, Luria : Luria Status */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 1)]
		public UInt16[]	u16_reserved;
		public Int32	last_sock_error;	/* The most recent socket error */
		public Int32	i32_reserved;

		/*
		 desc : reset data
		 parm : None
		 retn : None
		*/
		public void ResetData()
		{
			is_connected	= 0x00;
			/*memset(link_time, 0x00, sizeof(UInt16) * 6);*/
			System.Array.Clear(link_time, 0x00, Marshal.SizeOf(typeof(UInt16)) * 6);
			u16_error_code	= 0x0000;
			last_sock_error	= 0x00000000;
		}
	};

	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_LDSP  /* luria_data_shared_memory - Packet Command (User ID) */
	{
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxCmdUserD)]
		public Byte[]	user_id;
	}

	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_I4XY /* common_config_Int32_x_y */
	{
		public Int32	x, y;
	};
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_U4XY /* common_config_UInt32_x_y */
	{
		public UInt32	x, y;
	};

	/* 좌표 구조체 : Int32 X/Y */
	[StructLayout(LayoutKind.Sequential, Pack = 8)]
	public struct STG_I32XY /* coord_integer32_x_y */
	{
		public Int32	x, y;
	};

	/* 좌표 구조체 : UInt32 X/Y */
	[StructLayout(LayoutKind.Sequential, Pack = 8)]
	public struct STG_U32XY /* coord_unsigned32_x_y */
	{
		public UInt32	x, y;
	};

	/* 좌표 구조체 : Double X/Y */
	[StructLayout(LayoutKind.Sequential, Pack = 8)]
	public struct STG_DBXY /* coord_double_x_y */
	{
		public double	x, y;
	};
	/* 좌표 구조체 : Double Width / Height */
	[StructLayout(LayoutKind.Sequential, Pack = 8)]
	public struct STG_DBWH /* coord_double_width_height */
	{
		public double	cx, cy;
	};

	/* IP Address for IPv4 */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_SIAB /* system_ip_addr_byte */
	{
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public Byte[]	ipv4;
	}
	/* JobManagement - Job Item Name */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_JJIN /* job_management_job_item_names */
	{
#if (false)
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxGerberName)]
		public Byte[]	name;	/* Convert Byte to Char Array */
#else
		[MarshalAs(UnmanagedType.ByValTStr, SizeConst = Constants.g_MaxGerberName)]
		public string	name;   /* Convert Byte to Char Array */
#endif
	}

	/* Exposure - Led Amplitude Power */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_ELAP /* exposure_led_amplitude_power */
	{
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxLed)]
		public UInt16[] led_no;
	}

	/* DirectPh - Light Source Type Info */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_DLST
	{
		public Byte		led_type;	/* 0:LED Combinaer v1, 1:Laser, 2:LED Combiner v2 */
		public Byte		led_count;	/* connected count or available count */
	}
	/* DirectPh - light_source_driver_light_onoff */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_DLSD
	{
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxLed)]
		public Byte[]	onoff;
	}
	/* DirectPh - light_source_common_uint16 */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_DL16
	{
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxLed)]
		public UInt16[]	value;
	}
	/* DirectPh - light_source_common_uint32 */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_DL32
	{
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxLed)]
		public UInt32[]	value;
	}
	/* Exposure - exposure_work_light_source */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_ELSS
	{
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxLed)]
		public UInt32[] status;
	}
}

/* --------------------------------------------------------------------------------------------- */
/*                                         Engine -> MC2                                         */
/* --------------------------------------------------------------------------------------------- */

namespace TGen2i.Struct.MC2
{
	/* Reference - Header (32 bytes) */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_RVCH	/* reference_value_collective_header */
	{
		/* MControl */
		public Byte		mc_control;
		public UInt16	mc_reserved							{ get { return Bit.GetU16ToU16(mc_control, 0x00, 0x06); } }	/* reserved : 0 ~ 5 bit */
		public Byte		mc_en_stop							{ get { return Bit.GetU8ToBit(mc_control, 0x06); } }		/* All drives stop after an error has occurred (not yet implemented) */
		public Byte		mc_heart_beat						{ get { return Bit.GetU8ToBit(mc_control, 0x07); } }		/* Heart beat toggle bit */
		/* V_SCALE */
		public Byte		v_scale;																					/* This parameter allows percentage modification of the speed of all drives (1 ~ 100 %) */
		/* Reserved */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 18)]
		public Byte[]	u8_reserved1;
		/* OUTPUTS */
		public UInt64	digital_outputs;																			/* The parameter OUTPUTS allows influencing the possibly existing digital outputs at the MC2 control. The field has a width of 64 bits. */
		/* Reserved */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public Byte[]	u8_reserved2;
	};

	/* Reference - Drive Data (16 bytes) */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_RVCD	/* reference_value_collective_drive */
	{
		/* Control */
		public UInt16	control;
		public Byte		ctrl_cmd							{ get { return Bit.GetU16ToBits(control, 0x00, 0x07); } }	/* Command number (ENG_MCCN) */
		public Byte		ctrl_reserved						{ get { return Bit.GetU16ToBits(control, 0x08, 0x07); } }
		public Byte		ctrl_cmd_toggle						{ get { return Bit.GetU16ToBit(control, 0x0e); } }			/* Command Toggle Bit */

		public Byte		param_0;
		public Byte		u8_reserved1;
		/* Different values are stored depending on the value of the ctrl_cmd variable (See ENG_MCCN) */
		public Int32	param_1;			/* (See ENG_MCCN) */
		public Int32	param_2;			/* (See ENG_MCCN) */
		public Int32	param_3;			/* (See ENG_MCCN) */
	};

	/* Actual - Header (32 bytes) */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_AVCH	/* actual_value_collective_header */
	{
		/* MState */
		public Byte	m_state;
		public UInt16	ms_reserved						{ get { return Bit.GetU16ToU16(m_state, 0x00, 0x06); } }	/* reserved */
		public UInt16	ms_heart_beat1					{ get { return Bit.GetU8ToBit(m_state, 0x06); } }			/*  */
		public UInt16	ms_heart_beat2					{ get { return Bit.GetU8ToBit(m_state, 0x07); } }			/* Heart beat toggle bit */

		public Byte		u8_reserved1;

		/* MError */
		public UInt16	merror;				/*  */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public Byte[]	u8_reserved2;
		/* Inputs & Outputs */
		public UInt64	inputs;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public Byte[]	u8_reserved3;
		public UInt64	outputs;			/*  */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public Byte[]	u8_reserved4;
	};

	/* Actual - Drive Data (16 bytes) */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_AVCD	/* actual_value_collective_drive */
	{
		/* State */
		public UInt16	state;
		public Byte		flag_done						{ get { return Bit.GetU16ToBit(state, 0x00); } }		/* command_done */
		public Byte		flag_cal						{ get { return Bit.GetU16ToBit(state, 0x01); } }		/* drive_calibrated */
		public Byte		flag_on							{ get { return Bit.GetU16ToBit(state, 0x02); } }		/* power_stage_on */
		public Byte		flag_err						{ get { return Bit.GetU16ToBit(state, 0x03); } }		/* error_code_available */
		public Byte		flag_msg						{ get { return Bit.GetU16ToBit(state, 0x04); } }		/* message_code_available */
		public Byte		flag_busy						{ get { return Bit.GetU16ToBit(state, 0x05); } }		/* command_busy */
		public Byte		flag_zero						{ get { return Bit.GetU16ToBit(state, 0x06); } }		/* velocity_zero */
		public Byte		flag_inpos						{ get { return Bit.GetU16ToBit(state, 0x07); } }		/* target_position_reached */
		public Byte		flag_varc						{ get { return Bit.GetU16ToBit(state, 0x08); } }		/* target_velocity_reached */
		public Byte		flag_init						{ get { return Bit.GetU16ToBit(state, 0x09); } }		/* drive_ready */
		public UInt16	flag_reserved					{ get { return Bit.GetU16ToU16(state, 0x0a, 0x05); } }	/* u16_reserved */
		public Byte		flag_done_toggle				{ get { return Bit.GetU16ToBit(state, 0x0f); } }		/* 항상 반전되는 값 */

		public UInt16	error;																					/* Error Code */
		public Int32	real_position;																			/* Actual Position (unit: 100 nm or 0.1 um) */
		public Int32	result1;
		public Int32	result2;

		/*
		 desc : Whether the current driver (Axis) is moving (moving)
		 parm : None
		 retn : true (Stopped) or false (Moving)
		*/
		public Boolean IsStopped()
		{
#if (true)
#if (false)
			return ((0x01 == flag_zero) && (0x00 == flag_busy));
#else
			return (0x00 == flag_busy);
#endif
#else
			return ((0x01 == flag_zero) && (0x00 == flag_busy) && (0x01 == flag_inpos));
#endif
		}
		/*
		 desc : Whether it is currently busy
		 parm : None
		 retn : true or false
		*/
		public Boolean IsBusy()			{	return flag_busy == 0x00 ? true : false;	}

		/*
		 desc : Whether you have reached the desired location
		 parm : None
		 retn : true (Reached) or false (Not yet)
		*/
		public Boolean IsReached()		{	return 0x01 == flag_inpos;	}

		/*
		 desc : Whether the current Servo On / Off, that is, whether it is calibrated
		 parm : None
		 retn : true - Servo Off, false - Servo On
		*/
		public Boolean IsServoOn()		{	return (0x01 == flag_on);	}
		public Boolean IsServoOff()		{	return (0 == flag_on);		}

		/*
		 desc : Whether it is currently Servo On, that is, whether it is calibrated
		 parm : None
		 retn : true - Servo Off, false - Servo On
		*/
		public Boolean IsCalibrated()	{	return (0x01 == flag_cal);	}

		/*
		 desc : Whether it is the current Device Error
		 parm : None
		 retn : true - Servo Off, false - Servo On
		*/
		public Boolean IsServoError()
		{
			return ((0x00 == flag_on) &&
					((0x01 == flag_err) &&
					 (error != 0x0000 && error <= 0x0160 /* If it is greater than the 0x0160 value, it is not an error */) ||
					 (error != 0x0000 && error == 0x1100)));	/* The 0x1100 is hardware error */
		}
	};

	/* Data information received from MC2 */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_MPRD	/* mc2_packet_recv_data */
	{
		public Byte		msg_type;				/* Message Type (E_CMD_NACK:7 or E_GEN3_DNC:10) */
		public Byte		dnc_cmd;				/* ENG_DCG3 즉, Read Request or Write Request */
		public Byte		dnc_err;				/* DNC Command Error (수신된 패킷의 에러 정보) (ENG_MGEC) */
		public Byte		sub_id;					/* SubID of the SD3 device */
		public Byte		undefined;				/* 정상일 때는 0x00, 정의(구현)하지 않은 명령어가 수신된 경우 양수 값 (0x01) */
		public Byte		data_len;				/* data 버퍼에 저장된 길이 (크기) (0일 경우, data = NULL 임) */
		public Byte		object_index;			/* ENG_PSOI (PDO & SDO Object Index) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 1)]
		public Byte[]	u8_reserved;

		public UInt32	obj_index;					/* Object ID 즉, PDO or SDO */
		public UInt32	sub_index;					/* Object의 Offset (송신 쪽의 Sub Index이지, 수신쪽의 Sub Index는 아님) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 168)]
		public Byte[]	data;						/* 수신된 데이터가 저장된 버퍼, 추후 이 메모리는 수동으로 해제 */
	};

	/* MC2 full environment file */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_MDSM	/* mc2_data_shared_memory */
	{
		/* Reference Objects */
		public STG_RVCH		ref_head;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxMC2Drive)]
		public STG_RVCD[]	ref_data;
		/* Actual Objects */
		public STG_AVCH		act_head;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxMC2Drive)]
		public STG_AVCD[]	act_data;
		/* Last Recv Packet */
		public STG_MPRD		last_recv;

		/* 연결 상태 */
		public STG_DLSM		link;

		/*
		 desc : Whether it is a DNC error
		 parm : None
		 retn : 0x00 (No Error), 0x01 or Later (Error)
		*/
		public Byte GetDNCError()
		{
			return last_recv.dnc_err;
		}

		/* ----------------------------------------------------------------------------------------- */
		/*                  Object 0x3801, PDO Reference Value Collective Object                     */
		/* ----------------------------------------------------------------------------------------- */

		/*
		 desc : PDO Reference Value Setting
		 parm : data	- [in]  Buffer pointer where received data is stored
				size	- [in]  size stored in buffer 'data'
		 retn : None
		*/
		public void SetDataRefVal(Byte[] data, Int32 size)
		{
			Int32 i32SizeHead = (Int32)Marshal.SizeOf(typeof(STG_RVCH));
			Int32 i32SizeData = (Int32)Marshal.SizeOf(typeof(STG_RVCD)) * Constants.g_MaxMC2Drive;

			if (size == (i32SizeHead + i32SizeData))
			{
				/* Header */
				ref_head = Converter.BytesToStruct<STG_RVCH>(data);
				/* Body : Drive 8 ea */
				Buffer.BlockCopy(data, i32SizeHead, ref_data, 0, i32SizeData);
			}
		}

		/*
		 desc : return header information
		 parm : None
		 retn : A pointer to a structure that stores header information.
		*/
		public STG_RVCH GetHeadRef()	/* LPG_RVCH */
		{
			return ref_head;
		}

		/*
		 desc : Return drive information
		 parm : drv_id	- [in]  SD2 driver number (0x00 ~ 0x07)
		 retn : A pointer to a structure that stores drive information
		*/
		public STG_RVCD GetDriveRef(Byte drv_id)	/* LPG_RVCD */
		{
			return ref_data[drv_id];
		}

		/*
		 desc : Returns the current command status toggle value
		 parm : drv_id	- [in]  SD2 driver number (0x00 ~ 0x07)
		 retn : return toggle value
		*/
		public UInt16 GetCmdToggled(Byte drv_id)
		{
			return ref_data[drv_id].ctrl_cmd_toggle;
		}

		/* ----------------------------------------------------------------------------------------- */
		/*                    Object 0x3802, PDO Active Value Collective Object                      */
		/* ----------------------------------------------------------------------------------------- */

		/*
		 desc : return header information
		 parm : None
		 retn : A pointer to a structure that stores header information.
		*/
		public STG_AVCH GetHeadAct()	/* LPG_AVCH */
		{
			return act_head;
		}

		/*
		 desc : Return drive information
		 parm : drv_id	- [in]  SD2 driver number (0x00 ~ 0x07)
		 retn : A pointer to a structure that stores drive information
		*/
		public STG_AVCD GetDriveAct(Byte drv_id)	/* LPG_AVCD */
		{
			return act_data[drv_id];
		}

		/*
		 desc : PDO Actual Value Setting
		 parm : data	- [in]  Buffer pointer where received data is stored
				size	- [in]  size stored in buffer 'data'
		 retn : None
		*/
		public void SetDataActVal(Byte[] data, UInt32 size)
		{
			Int32 i32SizeHead = (Int32)Marshal.SizeOf(typeof(STG_AVCH));
			Int32 i32SizeData = (Int32)Marshal.SizeOf(typeof(STG_AVCD)) * Constants.g_MaxMC2Drive;

			if (size == (i32SizeHead + i32SizeData))
			{
				/* Header */
				act_head = Converter.BytesToStruct<STG_AVCH>(data);
				/* Body : Drive 8 ea */
				Buffer.BlockCopy(data, i32SizeHead, act_data, 0, i32SizeData);
			}
		}

		/*
		 desc : Whether a specific drive has an error
		 parm : drv_id	- [in]  Driver ID to check for errors
		 retn : true or false
		*/
		public Boolean IsDriveError(Byte drv_id)
		{
			/* 아래 MERROR는 사용되지 않음 */
#if (false)
			/* 기본 Header 에러 정보 유무 */
			if (act_head.merror > 0)	return true;
#endif
			return ((act_data[drv_id].flag_err > 0) && (act_data[drv_id].error > 0));
		}

		/*
		 desc : Whether a particular drive is Busy
		 parm : drv_id	- [in]  Driver ID to check for errors
		 retn : true or false
		*/
		public Boolean IsDriveBusy(Byte drv_id)
		{
			return act_data[drv_id].flag_busy > 0 ? true : false;
		}

		/*
		 desc : Whether an error occurred in one or more of the drives
		 parm : count	- [in]  number to search
				drv_id	- [in]  Array pointer where Drive ID is stored
		 retn : true or false
		*/
		public Boolean IsAnyDriveError(Byte count, Byte[] drv_id)
		{
			Byte i = 0x00;

			for (; i<count; i++)
			{
				if (IsDriveError(drv_id[i]))	return true;
			}

			return false;
		}

		/*
		 desc : Return the error code value of the first searched drive among motion drives
		 parm : drv_id	- [in]  Array pointer where the driver ID to check for errors is stored
				count	- [in]  Number of Drive IDs stored in 'drv_id'
		 retn : Error code value. A value of 0 means no error.
		*/
		public UInt16 GetErrorCode(Byte count, Byte[] drv_id)
		{
			Byte i = 0x00;

			for (; i<count; i++)
			{
				if (0x00 == act_data[drv_id[i]].flag_err)	continue;
				return act_data[drv_id[i]].error;
			}

			return 0x0000;
		}
		public UInt16 GetErrorCodeOne(Byte drv_id)
		{
			if (0 == act_data[drv_id].flag_err)	return 0x0000;
			return act_data[drv_id].error;
		}

		/*
		 desc : Reset all drive error values
		 parm : None
		 retn : None
		*/
		void ResetDriveAll()
		{
			Byte i	= 0x00;
			for (i=0; i<Constants.g_MaxMC2Drive; i++)	act_data[i].error	= 0x0000;
		}

		/*
		 desc : Whether any one of the drivers is Servo On, that is, whether it is calibrated
		 parm : drv_id	- [in]  Array pointer where the driver ID to check for errors is stored
				count	- [in]  Number of Drive IDs stored in 'drv_id'
		 retn : true or false
		*/
		public Boolean IsServoOn(Byte[] drv_id, Byte count)
		{
			Byte i = 0x00;

			for (; i<count; i++)
			{
				if (act_data[drv_id[i]].IsServoOn())	return true;
			}

			return false;
		}

		/*
		 desc : Whether arbitrary servo drive is off
		 parm : drv_id	- [in]  SD2 Driver ID 즉, 0x00, 0x01, 0x04, 0x05, ..., 0x07
		 retn : true (Servo Off) or false (Servo On)
		*/
		public Boolean IsServoOff(Byte drv_id)
		{
			if (!act_data[drv_id].IsServoOn())	return true;

			return false;
		}

		/*
		 desc : Returns the Done Toggled value of the current drive
		 parm : drv_id	- [in]  Drive ID (0x00 ~ 0x07)
		 retn : Current Done Toggled value
		*/
		public Byte GetDoneToggled(Byte drv_id)
		{
			return (Byte)(act_data[drv_id].flag_done_toggle);
		}

		/*
		 desc : Return current drive location information (unit: mm)
		 parm : drv_id - [in]  Drive ID (ENG_MMDI)
		 retn : Drive position (unit: mm)
		*/
		public Double GetDrivePos(Byte drv_id)
		{
			return act_data[drv_id].real_position / 10000.0f;
		}
	};
}

/* --------------------------------------------------------------------------------------------- */
/*                                       Engine -> Luria                                         */
/* --------------------------------------------------------------------------------------------- */

namespace TGen2i.Struct.Luria
{
	/* ----------------------------------------------------------------------------------------- */
	/*                               Luria (MachineConfig) (0xA0)                                */
	/* ----------------------------------------------------------------------------------------- */

	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_MCTP /* machine_config_table_position_limit */
	{
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public Int32[]		min_xy;										/* (unit: nm) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public Int32[]		max_xy;

		public Boolean IsValid()
		{
			for (Byte i=0x00; i<2; i++)
			{
				if (max_xy[i] < 1)	return false;
			}
			return true;
		}
	};

	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_MCHT /* machine_config_hysteresis_type1 */
	{
		public Byte			scroll_mode;									/* 노광 속도 값 (1 ~ 6) (높을수록 속도는 빠르나 광량은 줄어듬) */
		public UInt16		negative_offset;								/* Given in number of trigger pulses. pixel (LLS25 -> 1 pixel -> 10.8 um)  */
																			/* Scroll Mode: 1 ~ 4, Negative offset. Pixel Count (1 or Later) (unit: pixel) */
		public UInt32		delay_positive;									/* Scroll Mode: 1 ~ 4, 1 Pixel의 이동 지연 시간) (0th: positive, 1th: negative) (unit: nano seconds) */
		public UInt32		delay_negative;

		public Boolean IsSetData()
		{
			return ((scroll_mode != 0xff) && (negative_offset != UInt16.MaxValue) &&
					(delay_negative != UInt32.MaxValue) && (delay_negative != UInt32.MaxValue));
		}
	};

	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_MCPO /* machine_config_photohead_offset */
	{
		public UInt32		pos_offset_x;									/* 1 번 Photo head 기준으로 2 ~ 8 번의 Photo head X Offset 값 (unit: nm) */
		public Int32		pos_offset_y;									/* 1 번 Photo head 기준으로 2 ~ 7 번의 Photo head Y Offset 값 (unit: nm) */

		public Boolean IsSetData()
		{
			return (pos_offset_x != UInt32.MaxValue && pos_offset_y != Int32.MaxValue);
		}
	};

	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_MCXC /* machine_config_x_correction_table */
	{
		public UInt32		table_source_x;									/* Table 1 / 2에 대한 보정 전 위치 값이 저장된 버퍼 (단위: nm) */
																			/* Table 1 보정 전 데이터 : x_correction_table_source[0], Table 2 보정 전 데이터 : x_correction_table_source[1] */
		public UInt32		table_adjust_x;									/* Table 1 / 2에 대한 보정 후 위치 값이 저장된 버퍼 (단위: nm) */
																			/* Table 1 보정 후 데이터 : x_correction_table_adjust[0], Table 2 보정 후 데이터 : x_correction_table_adjust[1] */
	};

	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_MCYC /* machine_config_y_correction_table */
	{
		public Byte			strip_no;										/* Y 축 방향으로 노광한 Strip 간의 상/하 단차 보정 (Strip Number는 1번부터 시작) */
		public Int32		table_adjust_y;									/* Y-adjustment for strip number given in entry 1 ~ N (unit: nm) */
	};

	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_LDMC /* luria_data_machine_config */
	{
#if (false)
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPathLen)]
		public Byte[]		print_simulate_out_dir;						/* Simulate 동작 후 결과 파일이 저장될 경로 설정 (절대 경로) (LEN: MAX_PATH_LEN) */
#else
		[MarshalAs(UnmanagedType.ByValTStr, SizeConst = Constants.g_MaxPathLen)]
		public string		print_simulate_out_dir;						/* Simulate 동작 후 결과 파일이 저장될 경로 설정 (절대 경로) (LEN: MAX_PATH_LEN) */
#endif
		public Byte			total_ph;                                       /* 현재 장비에 운용 중인 포토헤드 총 개수 (1 ~ 8) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public STG_SIAB[]	ph_ip;
		public Byte			ph_pitch;										/* LLS50:2, LLS25:4, LLS10:8 (단, Photo head가 1개인 경우, 1 값) */
		public Byte			ph_rotate;										/* Photohead 설치 방향 (0 = No rotation, 1 = Rotated 180 degrees) */
		public Byte			motion_control_type;							/* SIEB&MEYER = 1, Newport = 2, 3= ACS */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public Byte[]		motion_control_ip;								/* Motion Control IP address */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxTable)]
		public Byte[]		table_print_direction;							/* 0: Negative, 1: Positive 방향으로 노광 시작 (최대 테이블 개수만큼) */
		public Byte			active_table;									/* 초기 활성화 하려는 Table Number (Work Table 1 or 2). Double Gantry Type일 경우 고려. Single Stage일 경우, 무조건 1 */
		public Byte			emulate_motor_controller;						/* Luria 서비스가 모터 컨트롤러 연동 여부에 따라 동작 설정 할 수 있도록 설정 */
																			/* 0 = 실제 모터 컨틀로러가 연동되어 동작, 1 = 모터 컨트롤러 없이 동작 가능 */
		public Byte			emulate_ph;										/* Luria 서비스가 포토헤드 연동 여부에 따라 동작 수행할지 여부 설정 */
																			/* 0 = 실제 포토헤드가 연동되어 동작, 1 = 포토헤드 없이 동작 */
		public Byte			emulate_triggers;								/* Luria 서비스가 트리거 연동 여부에 따라 동작 수행할지 여부 설정 */
																			/* 0 = 실제 포토헤드가 연동되어 동작, 1 = 포토헤드 없이 동작 */
		public Byte			debug_print_level;								/* 0: Print info only, 1: Print info and debug, 2: Print info, debug and trace message */
		public Byte			artwork_complexity;								/* Artwork Complexity (거버 복잡도에 따라 값 지정. 정확한 이해는 문서 참조) */
																			/* 보통 0 값 지정 (1 or 2 값의 경우, 포토헤드에 등록되는 거버 개수 제한 됨) */
																			/* 0 = Normal, 1 = High, 2 = eXtrem */
		public Byte			z_drive_type;									/* ZdriveType (포토헤드에 장착된 Motion Drive Type) */
																			/* 1 = Stepper Motor, 2 = Linear drive (ACS), 3 = Linear drive (Sieb & Meyer) */
		public Byte			over_pressure_mode;								/* 포토헤드 내부에 과부하가 발생한 경우 냉각 팬 동작 유무 설정 */
																			/* 1 = Over pressure mode ON, 0 = Over pressure mode OFF */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxTable * 2)]
		public Byte[]		xy_drive_id /* [2, 2] */;						/* 포토헤드 기준 모션 X / Y 축 드라이브 ID (0 ~ 7). At Normal : X = 0, Y = 1 */
		public Byte			mtc_mode;										/* When this is enabled the exposure mode is set to expose each image three times, resulting in smoother edges. It is required that the job is preprocessed with the triple exposure option */
		public Byte			use_ethercat_for_af;							/* When not using ethercat for AF Luria will not try to communicate over ethercat with the z-motion controller */

		public UInt16		scroll_rate;									/* 0 ~ 18000 Hz (Base default : 17000 Hz */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxTable)]
		public UInt16[]		x_correction_table_entry;						/* Table 1/2 에 대한 X 보정 데이터 개수 (0 ~ 200) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxTable)]
		public UInt16[]		y_correction_table_entry;						/* Table 1/2 에 대한 Y 보정 데이터 개수 (0 ~ 200) */

		public UInt16		depth_of_focus;									/* 스텝 개수 (Depth of focus 지정) 즉, 이동할 수 있는 스텝의 단계 (수) 지정 */
																			/* Z 축이 Linear Motor인 경우 유효. 현재 Stepper Motor일 경우 무시 됨 */
		public UInt16		spx_level;										/* The spx-level from preprocessed files must correspond to the value set here */
		public UInt16		ocp_error_mask;									/* This will mask any OCP errors for the time that is given. This will apply to ALL light sources */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxTable)]
		public UInt32[]		parallelogram_adjust;							/* 노광 중 모션(스테이지)을 상/하 이동할 때, 대각선 이동 크기에 */
																			/* Fixed 1 (1000). ex> 0.97 --> 970 (송신 * 1000, 수신 / 1000) */
		public UInt32		max_y_motion_speed;								/* 테이블 Y 축 방향의 최대 이동 속도 (단위: um/sec) */
		public UInt32		x_motion_speed;									/* 테이블 X 축 이동할 때 속도. 최대 속도 아님 (노광할 때, 노광 시작 위치로 이동하는 속도도 포함) */
		public UInt32		y_acceleration_distance;						/* Y 축 가속도가 정상 궤도에 오르기 위한 취소한의 여유 거리 (unit: um) */
		public UInt32		product_id;										/* ID identifying the LLS-system. ENG_LPPI (LLS2500 : 16702, LLS04=25002, LLS10 = 50001, LLS25=100001, ...,) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxTable)]
		public STG_I4XY[]	table_expo_start_xy;							/* 스테이지 (Table)마다 실제 노광이 시작되는 모션의 위치 값 (unit: um) Only table (stage) number 1 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxTable)]
		public STG_MCTP[]	table_position_limit;							/* Table 별 (1 / 2 / ...)로 노광할 영역의 최소 X/Y 위치 값 (unit: um) */
		public STG_MCHT		hysteresis_type1;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public STG_MCPO[]	ph_offset_xy;									/* 1 번 Photo head 기준으로 2 ~ 8 번의 Photo head X / Y Offset 값 (unit: nm) */
																			/* 1 번 배열부터 2번 광학계의 Offset (Step) XY 값이 저장됨. 0번 배열은 0 값으로 비워 둠 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxTable)]
		public STG_I4XY[]	get_table_motion_start_xy;						/* 노광이 시작하기 위해 이동하는 지점의 모션 X/Y 위치 (unit: um) (가속도 구간 고려) !!! Read Only !!! */

																			/* Table 1 / 2에 대한 보정 전 위치 값이 저장된 버퍼 (단위: nm) */
																			/* Table 1 보정 전 데이터 : x_correction_table_source[0], Table 2 보정 전 데이터 : x_correction_table_source[1] */
																			/* Y 축 방향으로 노광한 Strip 간의 상/하 단차 보정 (Strip Number는 1번부터 시작) */
																			/* 전체 노광할 때, 발생되는 Strip의 개수 (Big Jump가 없다면, 포토헤드 5개이고, PITCH가 4이면 Strip 개수는 총 20개 (1 ~ 20) */
																			/* 1번째 Strip의 보정은 보통 0 값이고, 1번 기준으로 Strip 단차 값 입력 (단위: nm) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxTable * Constants.g_MaxCorrectionTableEntry)]
		public STG_MCXC[,]	x_correction_table;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxTable * Constants.g_MaxCorrectionTableEntry)]
		public STG_MCYC[,]	y_correction_table;

		/*
		 desc : HysteresisType1 정보가 설정되어 있는지 여부
		 parm : ph_no	- [in]  Photohead Number (1-based)
		 retn : true or false
		*/
		public Boolean IsHysteresisType1()
		{
			return hysteresis_type1.IsSetData();
		}

		/*
		 desc : Photohead Offset 정보가 설정되어 있는지 여부
		 parm : ph_no	- [in]  Photohead Number (1-based. !!! Important - Not 0 !!!)
								if (ph_no == 1) then photohead 2 offset
		 retn : true or false
		*/
		public Boolean IsPhOffset(Byte ph_no)
		{
			return ph_offset_xy[ph_no].IsSetData();
		}

		/*
		 desc : System Settings 정보가 설정되어 있는지 여부
		 parm : init	- [in]  비교하려는 값. 0 or 1
		 retn : true or false
		*/
		public Boolean IsSystemSetting(Byte init)
		{
			return (emulate_motor_controller == init &&
					emulate_ph == init &&
					emulate_triggers == init &&
					debug_print_level != 0xff);
		}

		/*
		 desc : Table Settings 정보가 설정되어 있는지 여부
		 parm : tbl_no	- [in]  1 or 2
		 retn : true or false
		*/
		public Boolean IsTableSetting(Byte tbl_no)
		{
			return (parallelogram_adjust[tbl_no-1] > 0 && table_print_direction[tbl_no-1] > 0 &&
					table_position_limit[tbl_no-1].IsValid());
		}

		/*
		 desc : Motion Type1 정보가 설정되어 있는지 여부
		 parm : None
		 retn : true or false
		*/
		public Boolean IsMotionType1()
		{
			return (y_acceleration_distance > 0 && active_table > 0);
		}

		/*
		 desc : 포토헤드 기본 정보 값이 존재하는지 여부
		 parm : None
		 retn : true or false
		*/
		public Boolean IsSetAllPHInfo()
		{
			return ph_pitch > 0 && total_ph > 0 && scroll_rate > 0;
		}

		/*
		 desc : 모든 Photohead IPv4 값이 존재하는지 여부
		 parm : None
		 retn : true or false
		*/
		public Boolean IsSetPhotoheadsIPv4()
		{
			if (total_ph < 1)	return false;
			for (Byte i=0x00; i<total_ph; i++)
			{
#if (false)
				if (ph_ip[i,0] < 1)	return false;
#else
				if (ph_ip[i].ipv4[0] < (Byte)1)	return false;
#endif
			}
			return true;
		}

		/*
		 desc : Motion Control 정보가 있는지 여부
		 parm : None
		 retn : true or false
		*/
		public Boolean IsMSetotionControl()
		{
			return (motion_control_type > 0 && motion_control_ip[0] > 0 &&
					max_y_motion_speed > 0 && x_motion_speed > 0);
		}
	};

	/* ----------------------------------------------------------------------------------------- */
	/*                               Luria (JobManagement) (0xA1)                                */
	/* ----------------------------------------------------------------------------------------- */

	/* JobManagement - GetFiducials Information */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_JMFD /* job_management_fiducials_data */
	{
#if (false)
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxHashValueCount)]
		public SByte[]		hash_value;										/* MAX_HASH_VALUE_COUNT. Hash-value represented as 64 characters */
#else
		[MarshalAs(UnmanagedType.ByValTStr, SizeConst = Constants.g_MaxHashValueCount)]
		public string		hash_value;                                     /* MAX_HASH_VALUE_COUNT. Hash-value represented as 64 characters */
#endif
		/* Global Fiducial의 Coordinate Information */
		public Int32		g_d_code;										/* Fiducial D-Code. -1이면, 사용 안됨 */
		public UInt16		g_coord_count;									/* Fiducial의 X / Y 쌍의 개수 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxGlobalMarks)]
		public STG_U4XY[]	g_coord_xy;										/* Fiducial의 Coord X / Y 좌표. (단위: nm) */
		/* Local Fiducial의 Coordinate Information */
		public Int32		l_d_code;										/* Fiducial D-Code. -1이면, 사용 안됨 */
		public UInt16		l_coord_count;									/* Fiducial의 X / Y 쌍의 개수 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxLocalMarks)]
		public STG_U4XY[]	l_coord_xy;										/* Fiducial의 Coord X / Y 좌표. (단위: nm) */

		/*
		 desc : Get Global or Local Coordinate Values얻기
		 parm : type	- [in]  0x00 : Global, 0x01 : Local
				index	- [in]  Fiducial Index (Zero-based)
				direct	- [in]  0x00 : X Coord., 0x01 : Y Coord.
		 retn : Coordinate-X or Y (unit: nm)
		*/
		public UInt32 GetCoord(Byte type, UInt32 index, Byte direct)
		{
			/* Global Fiducial Information */
			if (0x00 == type)
			{
				if (index >= Constants.g_MaxGlobalMarks || direct > 0x01)	return 0;
				return (direct == 0) ? g_coord_xy[index].x : g_coord_xy[index].y;
			}

			/* Local Fiducial Information */
			if (index >= Constants.g_MaxLocalMarks || direct > 0x01)	return 0;
			return (direct == 0) ? l_coord_xy[index].x : l_coord_xy[index].y;
		}
	};

	/* JobManagement - GetJobParams */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_JMJP /* job_management_job_param */
	{
#if (false)
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxHashValueCount)]
		public SByte[]			hash_value;							/* 해쉬 문자 */
#else
		[MarshalAs(UnmanagedType.ByValTStr, SizeConst = Constants.g_MaxHashValueCount)]
		public string			hash_value;                         /* 해쉬 문자 */
#endif
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public UInt16[]			version;							/* Major - Minor - Sub-minor - Build Version */
		public Byte				ph_count;							/* 거버에 사용되는 포토헤드 개수 */
		public Byte				ph_pitch;							/* 포토헤드 사이에 노광되는 Strip 개수 */
		public UInt16			lens_magnification;					/* 렌즈 배율. 1000 값으로 나누기 */
		public Byte				ph_rotated;							/* 1 = rotated, 0 = Not rotated */
		public UInt16			strips_counts;						/* 거버 전체의 Strip 개수 */
		public UInt16			spx;								/* SPX Level */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public Byte[]			flip;								/* 거버의 X/Y Flip 여부 */
		public UInt16			rotation;							/* 거버의 회전 각도 (단위: degrees) */
		public UInt16			pixel_size;							/* Native pixel size (단위: nm) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public UInt32[]			artwork_size;						/* 거버의 X / Y 크기 (단위: um) */
		public Byte				gerber_inverted;					/* 거버 반전 여부 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public Int32[]			erosion;							/* 거버의 X / Y Erosion 값 (단위: um) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public Int32[]			offset;								/* 거버의 X / Y Offset 값 (단위: um) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public Int32[]			origin_offset;						/* X / Y offset from original gerber orgin (단위: um) */
		public UInt32			stripe_overlap;						/* 한 개의 strip에 저장된 여벽이 얼마만큼 겹치는지 ... (단위: um) */
		public Byte				table_print_direction;				/* Y 축 노광 방향. 1 = Positive, 0 = Negative */
		public UInt32			product_id;							/* Pre-Processing에서 제공 (지정)한 Product ID */
		public UInt32			strip_width;						/* DMD에 의해 한 번 노광된 stripe의 넓이 (폭) (단위: nm) */
		public Byte				edge_blend_enabled;					/* 0 = No edge blending, 1 = Edge blending enabled (Available from version 2.11.0) */
		public Byte				extra_long_strips;					/* 0 = Normal length strips, 1 = Extra long strips (Available from version 2.11.0) */
		public Byte				mtc_mode;							/* 0 = Normal exposure mode, 1 = MTC exposure mode (Available from version 3.00.0) */

		/*
		 desc : 값이 유효한지 여부
		 parm : None
		 retn : true or false
		*/
		public Boolean IsValidData()
		{
			return pixel_size > 0 && product_id > 0 && strip_width > 0 && strips_counts > 0;
		}
	};

	/* JobManagement - GetPanelDataDcodeList */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_JMPL /* job_management_paneldata_decode_list */
	{
		public UInt16			count;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxDCodeList)]
		public UInt32[]			d_code;								/* 'count' 만큼 등록된 D-Code Number */
	};

	/* JobManagement - GetPanelDataInfo */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_JMPD /* job_management_panel_data_info */
	{
		public Byte				type;					/* ENG_PDTC (0x00 ~ 0x02) */
		public Byte				content;				/* ENG_PDCC (0x00 ~ 0x02) */
		public UInt16			rotation;				/* Rotation. In degrees */
		public Byte				inverse;				/* Inverse panel data. 0 = Not inverted, 1 = Inverted */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public UInt32[]			size;					/* Panel size (Width / Height) */
		public UInt16			location_count;			/* Coordinate X/Y 개수. 최대 400 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxCoordXY)]
		public STG_U4XY[]		coord_xy;				/* Coordinate X/Y 가 저장된 버퍼. X/Y 순서 쌍으로 저장 (단위: nm) */

		/*
		 desc : Location 위치에 대한 Coordinate X/Y 값 반환 (단위: nm)
		 parm : index	- [in]  가져오고자 하는 Coordniate 위치 (Zero-base. 최대 399)
				direct	- [in]  0x00: Coord. X, 0x01: Coord. Y
		 retn : Coordinate-X or Y 값 반환 (단위: nm)
		*/
		public UInt32 GetLocationCoord(UInt16 index, Byte direct)
		{
			if (index >= Constants.g_MaxCoordXY || direct > 1)	return 0;

			return (direct == 0) ? coord_xy[index].x : coord_xy[index].y;
		}
	};

	/* Luria TCP/IP Data - Job Management (0xA1) (version 2.3.0 or Later (Newer) 기준) */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_LDJM /* luria_data_job_management */
	{
		public Byte				max_job_regist;						/* 등록 가능한 JOB 개수 */
		public Byte				job_regist_count;					/* 현재 등록된 JOB 개수 */
		public Byte				selected_job_load_state;			/* 선택된 Job Load 상태 즉, ENG_LSLS */
		/* 아래 3개 변수는 위의 selected_job_load_state가 Loading 중일 때만 의미가 있음 */
		public Byte				job_state_ph_loaded;				/* 포토헤드에 Job이 모두 적재되었는지 여부 */
		public Byte				job_state_strip_loaded;				/* 포토헤드에 Job 중 Strip이 적재된 개수 */
		public Byte				job_total_strip_loaded;             /* 포토헤드에 Job 중 총 적재될 Strip rotn */
#if (false)
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPathLen)]
		public Byte[]			root_directory;						/* Preprocessing 결과 이미지 파일이 저장될 절대 경로 (NULL이면, 거버 파일 저장 경로가 사용됨) */
																	/* 최대 등록 가능한 Job Path Name 등록 버퍼 위치 */
#else
		[MarshalAs(UnmanagedType.ByValTStr, SizeConst = Constants.g_MaxPathLen)]
		public string			root_directory;
#endif
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRegistJobList)]
		public STG_JJIN[]		job_name_list;
#if (false)
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxGerberName)]
		public Byte[]			selected_job_name;					/* 현재 선택된 Job Path Name */
#else
		[MarshalAs(UnmanagedType.ByValTStr, SizeConst = Constants.g_MaxGerberName)]
		public string			selected_job_name;					/* 현재 선택된 Job Path Name */
#endif
		public UInt32			selected_d_code;					/* 현재 선택된 Job Path Name의 D Code 값 */

		public STG_JMFD			selected_job_fiducial;				/* 현재 선택된 Job Path Name의 Fiducial Information 저장 됨 */
		public STG_JMJP			get_job_params;						/* 현재 선택된 Job Parameters 정보 저장 됨 */
		public STG_JMPL			selected_job_dcode_list;			/* 현재 선택된 Job Path Name의 D-Code 리스트 정보 (거버 내에 등록된 D-Code 리스트) */
		public STG_JMPD			selected_job_panel_data;			/* 현재 선택된 Job Path Name의 Panel Data 정보 저장 됨 */

		/*
		 desc : Returns the current gerber load factor value
		 parm : None
		 retn : Gerber load factor value (unit: percent)
		*/
		public Double GetLoadRate()
		{
			if (0x03 == selected_job_load_state)	return 100.0;
			if (!(job_total_strip_loaded > 0.0 && job_state_strip_loaded > 0.0))	return 0.0;
			return (job_state_strip_loaded * 100.0) / (job_total_strip_loaded * 1.0);
		}

		/*
		 desc : Returns the Job Name as an index
		 parm : index	- [in]  Array Index (Zero-based)
		 retn : succ - Job Names, fail - null
		*/
		public string GetJobNameIndex(Byte index)
		{
			if (job_regist_count <= index)	return null;
#if (false)
			string strJobName	= Encoding.Default.GetString(job_name_list[index].name);
			return strJobName;
#else
			return job_name_list[index].name;
#endif
		}

		/*
		 desc : If there is a currently received Job Name, the location is returned.
		 parm : None
		 retn : 0x00 to xxx if present, -1 if not
		*/
		public Int32 GetJobRegisteredLastIndex()
		{
			Byte i				= 0x00;
#if (false)
			Byte [] bJobName	= new byte[Constants.g_MaxGerberName];
#endif
			string strJobName	= string.Empty;

			/* 현재 수신된 Job List가 있는지 확인 */
			for (i=Constants.g_MaxRegistJobList-1; i>=0; i--)
			{
#if (false)
				/* 2차원 배열에서 임의 행(Row) 값들을 1차원 배열로 전체 가져오기 */
				Array.Clear(bJobName, 0, Constants.g_MaxGerberName);
				Buffer.BlockCopy(job_name_list[i], i * Constants.g_MaxGerberName, bJobName, 0, Constants.g_MaxGerberName);
				strJobName	= Encoding.Default.GetString(bJobName);
#else
	#if (false)
				strJobName	= Encoding.Default.GetString(job_name_list[i].name);
	#else
				strJobName	= job_name_list[i].name;
	#endif
#endif
				if (strJobName.Length > 0)	return i;
			}
			return -1;
		}

		/*
		 desc : Get the position of the currently selected Job Name
		 parm : None
		 retn : Negative number (-1): Failed to get selected Job Name, 0 or Later
		*/
		public SByte GetJobNameSelectedIndex()
		{
			Byte i	= 0x00;
#if (false)
			Byte [] bJobName	= new byte[Constants.g_MaxGerberName];
#endif
			string strJobName	= string.Empty;
#if (false)
			string strSelJob	= Encoding.Default.GetString(selected_job_name);
#else
			string strSelJob	= selected_job_name;
#endif
			if (selected_job_name.Length < 1 || job_regist_count < 1)	return -1;
			/* 현재 선택된 Job Name의 위치 얻기 */
			for (; i<job_regist_count; i++)
			{
#if (false)
				/* 2차원 배열에서 임의 행(Row) 값들을 1차원 배열로 전체 가져오기 */
				Array.Clear(bJobName, 0, Constants.g_MaxGerberName);
				Buffer.BlockCopy(job_name_list[i], i * Constants.g_MaxGerberName, bJobName, 0, Constants.g_MaxGerberName);
				strJobName	= Encoding.Default.GetString(bJobName);
#else
	#if (false)
				strJobName	= Encoding.Default.GetString(job_name_list[i].name);
	#else
				strJobName	= job_name_list[i].name;
	#endif
#endif
				/* 2 개의 문자열 비교 */
				if (0 == String.Compare(strSelJob, strJobName))	return (SByte)(i);
			}
			return -1;
		}
		/*
		 desc : Get the index position of the Job Name to be selected next (Zero-based)
		 parm : None
		 retn : Negative (-1): No Job Name selected. 0 or Later
		*/
		public SByte GetNextJobNameSelectIndex()
		{
			SByte i8Sel;

			/* 선택된 Job Name이 없거나, 등록된 Job Name이 없는지 확인 */
			if (selected_job_name.Length < 1 || job_regist_count < 1)	return -1;
			/* 등록된 Job Name이 1개 뿐이면 무조건 0 반환 */
			if (job_regist_count == 0x01)	return 0x00;

			/* 현재 선택된 Job Name의 위치 얻기 */
			i8Sel	= GetJobNameSelectedIndex();
			if (i8Sel < 0)	return i8Sel;

			/* 현재 선택된 값 1 증가 시키고, 만약 증가된 값이 등록된 개수가 동일하면 0 으로 설정 */
			if (++i8Sel >= job_regist_count)	i8Sel	= 0x00;

			return i8Sel;
		}

		/*
		 desc : Whether there is a previously registered Job Name
		 parm : name	- [in]  Job Name you want to search
		 retn : true - Job Name that has already been registered. false - Unregistered Job Name.
		*/
		public Boolean IsJobOnlyFinded(string name)
		{
			Byte i				= 0x00;
			Int32 i32FindStr	= -1;
			string strSub		= null;
#if (false)
			Byte [] bJobName	= new byte[Constants.g_MaxGerberName];
#endif
			string strJobName	= string.Empty;

			for (; i<job_regist_count; i++)
			{
#if (false)
				/* 2차원 배열에서 임의 행(Row) 값들을 1차원 배열로 전체 가져오기 */
				Array.Clear(bJobName, 0, Constants.g_MaxGerberName);
				Buffer.BlockCopy(job_name_list[i], i * Constants.g_MaxGerberName, bJobName, 0, Constants.g_MaxGerberName);
				strJobName	= Encoding.Default.GetString(bJobName);
#else
	#if (false)
				strJobName	= Encoding.Default.GetString(job_name_list[i].name);
	#else
				strJobName	= job_name_list[i].name;
	#endif
#endif
				/* 등록된 Job Name이 없는 경우인지 확인 */
				if (strJobName.Length < 1)	continue;

				/* 마지막에 디렉토리 구분 ('/')이 있는 곳 검색 */
				i32FindStr	= strJobName.LastIndexOf('\\', strJobName.Length-1, strJobName.Length);
				if (i32FindStr < 0)	continue;

				/* 검색된 문자열만 저장? */
				strSub	= strJobName.Substring(i32FindStr, strJobName.Length - i32FindStr);
				/* 동일한 Job Name인지 검사 */
				if (0 == String.Compare(name, strSub))	return true;
			}

			return false;
		}

		/*
		 desc : Whether there is a previously registered Job Name
		 parm : name	- [in]  Job Name you want to search for (including full path)
		 retn : true - It is an already registered Job Name. false - It is an unregistered job name.
		*/
		public Boolean IsJobFullFinded(string name)
		{
			Byte i	= 0x00;
#if (false)
			Byte [] bJobName	= new byte[Constants.g_MaxGerberName];
#endif
			string strJobName	= string.Empty;

			for (; i<job_regist_count; i++)
			{
#if (false)
				/* 2차원 배열에서 임의 행(Row) 값들을 1차원 배열로 전체 가져오기 */
				Array.Clear(bJobName, 0, Constants.g_MaxGerberName);
				Buffer.BlockCopy(job_name_list[i], i * Constants.g_MaxGerberName, bJobName, 0, Constants.g_MaxGerberName);
				strJobName	= Encoding.Default.GetString(bJobName);
#else
	#if (false)
				strJobName	= Encoding.Default.GetString(job_name_list[i].name);
	#else
				strJobName	= job_name_list[i].name;
	#endif
#endif

				/* 등록된 Job Name이 없는 경우인지 확인 */
				if (strJobName.Length < 1)	continue;
				/* 동일한 Job Name인지 검사 */
				if (0 == String.Compare(name, strJobName))	return true;
			}

			return false;
		}

		/*
		 desc : Whether there is a registered Job Name in the current location
		 parm : index	- [in]  Registration location (index) (0 to MAX_REGIST_JOB_LIST-1)
		 retn : true or false
		*/
		public Boolean IsRegisteredJobName(Byte index)
		{
#if (false)
			Byte [] bJobName	= new byte[Constants.g_MaxGerberName];
#endif
			string strJobName	= string.Empty;

			if (index >= Constants.g_MaxRegistJobList)	return false;
#if (false)
			/* 2차원 배열에서 임의 행(Row) 값들을 1차원 배열로 전체 가져오기 */
			Array.Clear(bJobName, 0, Constants.g_MaxGerberName);
			Buffer.BlockCopy(job_name_list[int], index * Constants.g_MaxGerberName, bJobName, 0, Constants.g_MaxGerberName);
			strJobName	= Encoding.Default.GetString(bJobName);
#else
	#if (false)
			strJobName	= Encoding.Default.GetString(job_name_list[index].name);
	#else
			strJobName	= job_name_list[index].name;
	#endif
#endif

			return (strJobName.Length > 0) ? true : false;
		}

		/*
		 desc : Whether there is currently selected Job Name
		 parm : None
		 retn : true or false
		*/
		public Boolean IsJobNameSelected()
		{
#if (false)
			string strJobName	= Encoding.Default.GetString(selected_job_name);
			return (strJobName.Length > 0) ? true : false;
#else
			return (selected_job_name.Length > 0) ? true : false;
#endif
		}

		/*
		 desc : Whether the currently selected Job Name is Loaded
		 parm : None
		 retn : true or false
		*/
		public Boolean IsJobNameLoadCompleted()
		{
			return selected_job_load_state == (Byte)ENG_LSLS.en_load_completed;
		}

		/*
		 desc : Whether the current job name is registered, selected, and loaded (IsJobNameLoadCompleted function extended)
		 parm : None
		 retn : true or false
		*/
		public Boolean IsJobNameRegistedSelectedLoaded()
		{
#if (false)
			string strJobName	= Encoding.Default.GetString(selected_job_name);
			if (strJobName.Length < 1)	return false;
#else
			if (selected_job_name.Length < 1)	return false;
#endif

#if (false)
			/* 부하 많이 걸린다... ? */
			if (!IsJobFinded(selected_job_name))	return false;
#endif
			return IsJobNameLoadCompleted();
		}

		/*
		 desc : Whether the currently selected Job Name is Loaded
		 parm : None
		 retn : true or false
		*/
		public Boolean IsJobNameLoadFailed()
		{
			return selected_job_load_state == (Byte)ENG_LSLS.en_load_failed;
		}

		/*
		 desc : Returns the length of the currently selected Job Name
		 parm : None
		 retn : 이름 길이
		*/
		public Int32 GetSelectedJobSize()
		{
#if (false)
			string strJobName	= Encoding.Default.GetString(selected_job_name);
			if (!IsJobNameSelected())	return 0;
			return (Int32)strJobName.Length;
#else
			if (!IsJobNameSelected())	return 0;
			return (Int32)selected_job_name.Length;
#endif
		}

		/*
		 desc : Whether there are registered Job Names
		 parm : None
		 retn : true or false
		*/
		public Boolean IsRegisteredJobs()
		{
			return job_regist_count > 0;
		}
	};

	/* ----------------------------------------------------------------------------------------- */
	/*                              Luria (PanelPreParation) (0xA2)                              */
	/* ----------------------------------------------------------------------------------------- */

	/* Panel Preparation - DynamicFiducials or Registration Points */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_PPDG /* panel_preparation_dynamic_global */
	{
		public UInt16		g_count;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxGlobalMarks)]
		public STG_I4XY[]	g_coord_xy;
	};

	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_PPDL /* panel_preparation_dynamic_local */
	{
		public UInt16		l_count;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxLocalMarks)]
		public STG_I4XY[]	l_coord_xy;
	};

	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_PPRP /* panel_preparation_registration_points */
	{
		public UInt16		r_count;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRegistrationPoints)]
		public STG_I4XY[]	r_coord_xy;
	};

	/* Panel Preparation - GetTransformationParams */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_PPTP /* panel_preparation_transformation_params */
	{
		public UInt16		zone_number;								/* 0 = Global, 1 and abover: local zone numbers */

		public Int32		rotation;									/* (unit: micro-degreses) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public UInt32[]		scale_xy;									/* X / Y scaling factor * 10^6. ex> 1.0002 = 1000200 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public Int32[]		offset_xy;									/* (unit: nm) */
		public UInt16		fiducial_coord_group_count;					/* Number of fiducial coordinate groups (N) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRegistrationPoints)]
		public STG_I4XY[]	fiducial_coord_xy;							/* X / Y Coordniate fiducial point (unit: nm) */
	};

	/* Panel Preparation - PanelDataSerialNumber */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_PPSN /* panel_preparation_serial_number */
	{
		public UInt32		d_code;								/* D-code */
		public Byte			horz_flip;							/* 1: flip, 0: No flip */
		public Byte			vert_flip;							/* 1: flip, 0: No flip */
		public UInt16		horz_font_size;						/* unit: nm, 자동 모드 원하면 0 값 */
		public UInt16		vert_font_size;						/* unit: nm, 자동 모드 원하면 0 값 */
		public Byte			symbol_mark;						/* ex> '#', '@', etc */
		public UInt32		start_num;
		public UInt16		string_len;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPanelSerialString)]
		public char[]		serial_str;
	};

	/* Panel Preparation - PanelDataSerialNumber */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_PPPD /* panel_preparation_panel_data */
	{
		public UInt32		d_code;								/* D-code */
		public Byte			horz_flip;							/* 1: flip, 0: No flip */
		public Byte			vert_flip;							/* 1: flip, 0: No flip */
		public UInt16		horz_font_size;						/* unit: nm, 자동 모드 원하면 0 값 */
		public UInt16		vert_font_size;						/* unit: nm, 자동 모드 원하면 0 값 */
		public UInt16		string_len;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPanelTextString)]
		public char[]		text_str;
	};

	/* Panel Preparation - GetWarpOfLimitsCoordinates */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_PPWL /* panel_preparation_warp_limits */
	{
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public UInt32[]		coord_xy;						/* unit: um */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public UInt32[]		error_xy;						/* unit: um */
	};

	/* Luria TCP/IP Data - Panel Preparation (0xA2) (version 2.3.0 or Later (Newer) 기준) */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_LDPP /* luria_data_panel_preparation */
	{
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
		public Byte[]		global_transformation_recipe;					/* [0] Rotation, [1] Scaling, [2] Offset, 0:Auto, 1:Fixed */
		public Byte			use_shared_local_zones;							/* 1: Use shared zones, 0: Do not use shared zoned */
		public Byte			local_transformation_recipe;					/* Bit 0 => 0: Auto, 1:Fixed, Bit 1 ~ 3 (Rotaion, Scale, Offset) */
		public Byte			global_rectangle_lock;							/* 1: 강제로 Rectangle Lock, 0: 강제로 Rectangle Locl 하지 않음 (기본 값) */
		public Byte			snap_to_zone_mode;								/* When this mode is enabled, the artwork is always warped to the closest zone registration setting */

		public UInt16		registration_pair_xy_count;						/* 얼라인 카메라에 의해 검사된 X/Y의 개수 (최대 400) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public UInt16[]		local_zone_geometry_xy;							/* Number of fiducial points in X/Y direction for one zone (1개의 Zone에 X/Y로 몇 개씩 존재하는지) */
		public UInt16		get_number_of_local_zones;						/* 계산된 Local Zone 개수 */
		public UInt16		get_registration_status;						/* 현재 Registration Status 값 (ENG_PDRS) */
		public UInt16		local_fixed_rotation_cnt;						/* Local Zone 개수 (최대 200) */
		public UInt16		local_fixed_scaling_cnt;						/* Local Zone 개수 (최대 200) */
		public UInt16		local_fixed_offset_cnt;							/* Local Zone 개수 (최대 200) */
		public UInt16		warp_coord_xy_cnt;								/* Warp Coordinate 개수 (최대 400) */

		public Int32		global_fixed_rotation;							/* Global fixed rotation, in micro-degrees */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public Int32[]		global_fixed_offset_xy;							/* Global fixed X/Y-offset (단위: nm) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public UInt32[]		global_fixed_scaling_xy;						/* Global fixed X/Y-scaling factor * 10^6 */

		public STG_PPDG		fiducial_global;								/* 동적으로 등록할 수 있는 Global Fiducial (단위: nm) */
		public STG_PPDL		fiducial_local;									/* 동적으로 등록할 수 있는 Local Fiducial (단위: nm) */
		public STG_PPRP		registration_points;							/* 등록된 마크 좌표 정보 영역 (단위: nm) */

		public STG_PPTP		get_transformation_params;						/* 가장 최근에 요청한 Transformation Params 정보 */
		public STG_PPSN		panel_data_serial_number;						/* 가장 최근에 요청한 Panel Data Serial Number 정보 */
		public STG_PPPD		panel_data;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxWarpCoordXY)]
		public STG_PPWL[]	warp_limits_coord_xy;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxLocalFixedCount)]
		public Int32[]		local_fixed_rotation;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxLocalFixedCount)]
		public STG_U4XY[]	local_fixed_scaling;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxLocalFixedCount)]
		public STG_I4XY[]	local_fixed_offset;

		/*
		 desc : 좌표 정보 얻기
		 parm : type	- [in]  좌표 대상의 구분 (ENG_PDCT)
				index	- [in]  저장된 메모리 위치 (Zero-based)
				direct	- [in]  0x00: Coord. X, 0x01: Coord. Y
		 retn : 좌표 값 (단위: nm)
		*/
		Int32 GetLocationCoord(ENG_PDCT type, UInt16 index, Byte direct)
		{
			switch(type)
			{
			case ENG_PDCT.en_dynamic_fiducial_global:
				if (index >= Constants.g_MaxGlobalMarks)		return 0;
				return (direct == 0x00) ? fiducial_global.g_coord_xy[index].x : fiducial_global.g_coord_xy[index].y;
			case ENG_PDCT.en_dynamic_fiducial_local	:
				if (index >= Constants.g_MaxLocalMarks)			return 0;
				return (direct == 0x00) ? fiducial_local.l_coord_xy[index].x : fiducial_local.l_coord_xy[index].y;
			case ENG_PDCT.en_registration_point		:
				if (index >= Constants.g_MaxRegistrationPoints)	return 0;
				return (direct == 0x00) ? registration_points.r_coord_xy[index].x : registration_points.r_coord_xy[index].y;
			}
			return 0;
		}
	};

	/* ----------------------------------------------------------------------------------------- */
	/*                              Luria (PanelPreParation) (0xA2)                              */
	/* ----------------------------------------------------------------------------------------- */

	/* Exposure - GetNumberOfLightSource */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_EWLI /* exposure_work_led_info */
	{
		public Byte			led_count;							/* 이용 가능한 주파수 개수 (265, 385, 395, 405) */
		public Byte			led_type;							/* 0:LED, 1:Laser, 2:LED combiner */
	};

	/* Exposure - AFSensor */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_EWAS /* exposure_work_af_sensor */
	{
		public Byte			sensor_type;						/* 1: Diffuse (퍼짐), 2:Specular (반사광) */
		public Byte			agc_type;							/* 1:AGC ON (기본), 0,2:AGC OFF. 2일 경우, 측정된 PWM 값이 사용됨 */
		public UInt16		agc_value;							/* Laser에 대한 PWM 값. (AGC Off 상태이어야 하고, Range:2~511. 기본 값은 100) */
	};

	/* Exposure - AFSensorMeasureLaserPWM */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_EWAP /* exposure_work_af_pwm */
	{
		Byte				meas_count;							/* 전체 측정된 개수 (Photohead마다 1개씩 임). 결국 Photohead 개수 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		UInt16[]			pwm_value;					/* 측정된 PWM 값 (2 ~ 511) */
	};

	/* Exposure - GetLightSourceStatusMulti */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_EWLS /* exposure_work_light_source */
	{
		public Byte			ph_count;							/* 전체 Photohead 개수 */
		public Byte			led_count;							/* 각 Photohead마다 설치된 LED 개수 */
		public Byte			led_type;							/* 0:LED, 1:Laser, 2:LED combiner, 4:Knife edge laser */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public STG_ELSS[]	status;
	};

	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_LDEW /* luria_data_exposure_work */
	{
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
		public Byte[]		get_exposure_state;					/* 현재 노광 상태 값 (exposure state (ENG_LPES) / Last completed print motion / Total number of print motions for the job) */
		public Byte			scroll_step_size;					/* Step Size (Scroll Mode) (1 ~ 4) */
		public Byte			led_duty_cycle;						/* LED duty cycle (Rangle: 0 ~ 100%) */
																/* 이 명령은 모든 포토 헤드에 LED의 On-time 시간 주기를 설정 */
																/* LED on-time = (1 / (scroll_rate * scroll_step_size * frame_rate_factor)) * led_duty_cycle/100 */
		public Byte			enable_af_copy_last_strip;			/* Enable AF copy 기능 적용 여부. 2번째 Strip의 Autofocus Profile을 마지막 Strip에 복사할지 말지 결정 */
		public Byte			disable_af_first_strip;				/* 첫 번째 포토헤드의 첫 Strip에 대한 Autofocus 동작 여부 설정. 0:Disable, 1:Not disable (기본 값) (반대 개념이니. 꼭 확인) */
		public Byte			af_sensor_measured_count;			/* AFSensorMeasureLaserPWM - 측정된 개수 즉, 포토헤드 개수 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public Byte[]		available_light_source_cnt;			/* Photohead 마다 이용 가능한 LED 개수 (설치된 개수 아님) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public Byte[]		light_source_type;					/* Photohead 마다 설치된 Light Source Type (0:LED, 1:Laser, 2: LED Combiner) */

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public STG_ELAP[]	led_amp;							/* 4개 파장(365nm, 385nm, 395nm, 405nm) 에 최대 8개 Photohead 지원. Led Power Index 값 */
		public UInt16		frame_rate_factor;					/* Frame Rate 값 설정 (0 < frame_rate_factor / 1000 <= 1 (Rangle: 1 ~ 1000) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public UInt16[]		af_sensor_measured_value;			/* AFSensorMeasureLaserPWM - 측정된 개수 즉, 포토헤드에 적용된 측정된 PWM 값 */
		public UInt32		get_exposure_speed;					/* 노광 중에 사용 중인 모션 속도 (unit: um/sec) */
																/* speed = scroll_rate * pixel_size * scroll_step_size * frame_rate_factor */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public STG_EWLI[]	light_source_info;					/* Light Source Property (LED Count, LED Kind) */
		public STG_EWAS		af_sensor;							/* Autofocus Sensor 관련 정보 */
		public STG_EWAP		af_pwm;								/* Autofocus Sensor (for laser)의 PWM 값 */
		public STG_EWLS		light_source_status;				/* Photohead 내에 설치된 Light Source의 상태 값 */

		/*
		 desc : The number of scan lines exposed so far (not the total number of exposures)
		 parm : None
		 retn : Number of Scan Lines (Number of Exposures)
		*/
		public Byte GetScanLine()
		{
			return get_exposure_state[1];
		}
		/*
		 desc : Total number of scan lines to be exposed (total number of exposures)
		 parm : None
		 retn : Number of Scan Lines (Total Number of Exposures)
		*/
		public Byte GetScanTotal()
		{
			return get_exposure_state[2];
		}
		/*
		 desc : Returns the exposure progress value.
		 parm : None
		 retn : Progress value (Rate. percent)
		*/
		public Double GetScanRate()
		{
			if (get_exposure_state[1] < 1 || get_exposure_state[2] < 1)	return 0.0f;
			return get_exposure_state[1] / (Double)(get_exposure_state[2] * 1.0) * 100.0;
		}
	
		/*
		 desc : Whether Duty Cycle, Step Size, and Frame Rate are all set
		 parm : None
		 retn : true or false
		*/
		public Boolean IsSetDutyStepFrame()
		{
			return led_duty_cycle > 0 && scroll_step_size > 0 && frame_rate_factor > 0;
		}

		/*
		 desc : Whether the LED Power Index and Watt values are set
		 parm : ph_cnt	- [in]  Photohead Count of Inspection Targets
		 retn : true or false
		*/
		public Boolean IsSetPowerIndex(Byte ph_cnt)
		{
			UInt16 i	= 0x00;

			if (ph_cnt > Constants.g_MaxPH)	ph_cnt	= Constants.g_MaxPH;
			for (; i<ph_cnt; i++)
			{
				if (led_amp[i].led_no[0] == 0xffff ||
					led_amp[i].led_no[1] == 0xffff ||
					led_amp[i].led_no[2] == 0xffff ||
					led_amp[i].led_no[3] == 0xffff)	return false;
			}
			return true;
		}

		/*
		 desc : Whether the current exposure state is Idle
		 parm : None
		 retn : true or false
		*/
		public Boolean IsWorkIdle()
		{
			Byte u8State	= get_exposure_state[0];	/* ENG_LPES */

#if (true)
			return ((u8State != 0x01) && (0x01 != (u8State & 0x01))) ? true : false;
#else
			/* If It is idle, then return FASLE */
			if ((Byte(ENG_LPES.en_idle) == (u8State & 0x0f)) ||
				(Byte(ENG_LPES.en_preprint_running)	!= u8State &&
				 Byte(ENG_LPES.en_print_running)		!= u8State &&
				 Byte(ENG_LPES.en_abort_in_progress)	!= u8State))	return true;	/* Idle */
#endif
		}
	};

	/* ----------------------------------------------------------------------------------------- */
	/*                                 Direct Photohead (0xA04)                                  */
	/* ----------------------------------------------------------------------------------------- */

	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_LDDP /* luria_data_direct_photohead */
	{
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public Byte[]			enable_flatness_correction_mask;					/* 1: Enable, 0: Disabled*/
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public Byte[]			load_internal_image;								/* The internal image number to be displayed. Range: 0 ~ 5 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public Byte[]			active_sequence_file;								/* 0: Use last loaded (external) sequence file, 1: Use internal sequence file */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public STG_DLSD[]		light_source_driver_light_onoff;					/* LED Power On/Off (1 ~ 8. PH 마다 LED 4) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public STG_DLST[]		light_source_info;									/* [0:type] 0:LED Combinaer v1, 1:Laser, 2:LED Combiner v2, 3: LRS-HY driver, [1:connect count] (1 (TYPE) + 1 (Connected count)) * 8 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public Byte[]			camera_illumination_led;							/* 각 광학계에 설치된 LED 조명 밝기 값 (Max 8) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public Byte[]			camera_led_power_onoff;								/* 각 광학계에 설치된 LED 전원 On or Off (Max 8) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public Byte[]			line_sensor_plot;									/* 각 광학계의 Line Sensor Plot Enable 여부 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public Byte[]			dmd_flip_output;									/* 각 광학계에 설치된 DMD Flip 설정 여부 (Max 8) */

		public UInt16			get_last_received_record_id;						/* 가장 최근에 수신된 Record ID 값 임시 저장 */
		public UInt16			get_last_received_reply_error;						/* 가장 최근에 수신된 응답 에러 값 */

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public UInt16[]			focus_position;										/* Range: 21284 - 44252, where 32768 is theoretical middle. 1 unit = 0.174um (기준: Luxbeam 4800/9500) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public UInt16[]			auto_focus_position;								/* Range: 21284 - 44252, where 32768 is theoretical middle. 1 unit = 0.174um (기준: Luxbeam 4800/9500) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public UInt16[]			focus_motor_status;									/* Motor Status (1 ~ 8) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public STG_DL16[]		light_source_driver_amplitude;						/* LED Power Value (1 ~ 8. PH 마다 LED 4) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public STG_DL16[]		light_source_driver_temp_led;						/* LED Temperature (1 ~ 8. PH 마다 LED 4). 나누기 10을 하면 실제 온도 값 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public STG_DL16[]		light_source_driver_temp_board;						/* Board Temperature (1 ~ 8. PH 마다 LED 4). 나누기 10을 하면 실제 온도 값 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public STG_DL32[]		light_source_on_time_counter;						/* LED On Time 시간 (단위: seconds) (1 ~ 8. PH 마다 LED 4) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public STG_DL32[]		light_source_on_time_read_ok;						/* LED On Time 시간 (단위: seconds) (1 ~ 8. PH 마다 LED 4) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public UInt32[]			overall_error_status_word;							/* LUT 48000 이상만 (이하는 지원하지 않음) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public UInt32[]			focus_motor_move_abs_position;						/* Motor Position (63000 - 85000) (1 ~ 8) (단위: um)*/
																					/* RLT 4600_4700 모델만 Status2 필드 값 사용 */

		/*
		 desc : 모든 Photohead의 Motor Status가 초기화 되었는지 여부
		 parm : ph_cnt	- [in]  검사 대상인 Photohead Count
		 retn : TRUE (초기화 완료된 상태) or FALSE (아직 초기화 되지 않은 상태)
		 note : 모두 중앙으로 이동이 되었는지만 체크
		*/
		public Boolean IsAllMotorMiddleInited(Byte ph_cnt)
		{
#if (false)
			switch (ph_count)
			{
			case 0x08	:	if ((focus_motor_status[7] & 0x000b) != 0x0000)	return false;
			case 0x07	:	if ((focus_motor_status[6] & 0x000b) != 0x0000)	return false;
			case 0x06	:	if ((focus_motor_status[5] & 0x000b) != 0x0000)	return false;
			case 0x05	:	if ((focus_motor_status[4] & 0x000b) != 0x0000)	return false;
			case 0x04	:	if ((focus_motor_status[3] & 0x000b) != 0x0000)	return false;
			case 0x03	:	if ((focus_motor_status[2] & 0x000b) != 0x0000)	return false;
			case 0x02	:	if ((focus_motor_status[1] & 0x000b) != 0x0000)	return false;
			case 0x01	:	if ((focus_motor_status[0] & 0x000b) != 0x0000)	return false;
			}
			return true;
#else
			if (ph_cnt < 0x01 || ph_cnt >= Constants.g_MaxPH)	return false;
			for (SByte i=(SByte)(ph_cnt-1); i>=0; i--)
			{
				if ((focus_motor_status[i] & 0x000b) != 0x0000)	return false;
			}
			return true;
#endif
		}

		/*
		 desc : 모든 Photohead의 Motor Status가 초기화 되었는지 여부
		 parm : ph_no	- [in]  검사 대상인 Photohead Index (1 or Later)
		 retn : TRUE (초기화 완료된 상태) or FALSE (아직 초기화 되지 않은 상태)
		 note : 모두 중앙으로 이동이 되었는지만 체크
		*/
		public Boolean IsMotorMiddleInited(Byte ph_no)
		{
			if (ph_no < 0x01 || ph_no > Constants.g_MaxPH)	return false;
			return ((focus_motor_status[ph_no-1] & 0x000b) != 0x0000) ? false : true;
		}

		/*
		 desc : PH 내에 온도가 가장 높은 LED와 Board의 온도 값 반환
		 parm : ph_no		- [in]  Photohead Number (1 or Later)
				temp_led	- [out] 가장 높은 LED 온도 (4개 LED 중)
				temp_bd		- [out] 가장 높은 Board 온도 (4개 LED Board 중)
		 retn : None
		*/
		public void GetMaxTempLedBD(Byte ph_no, ref UInt16 temp_led, ref UInt16 temp_bd)
		{
			Byte i	= 0x00;

			/* 값 초기화 */
			temp_led= 0x0000;
			temp_bd	= 0x0000;
			if (ph_no < 0x01 || ph_no > Constants.g_MaxPH)	return;

			for (; i<0x04; i++)
			{
				if (temp_led <= light_source_driver_temp_led[ph_no-1].value[i])
				{
					temp_led = light_source_driver_temp_led[ph_no-1].value[i];
				}
				if (temp_bd <= light_source_driver_temp_board[ph_no-1].value[i])
				{
					temp_bd = light_source_driver_temp_board[ph_no-1].value[i];
				}
			}
		}

		/*
		 desc : PH 내에 온도가 가장 낮은 LED와 Board의 온도 값 반환
		 parm : ph_no		- [in]  Photohead Number (1 or Later)
				temp_led	- [out] 가장 높은 LED 온도 (4개 LED 중)
				temp_bd		- [out] 가장 높은 Board 온도 (4개 LED Board 중)
		 retn : None
		*/
		public void GetMinTempLedBD(Byte ph_no, ref UInt16 temp_led, ref UInt16 temp_bd)
		{
			Byte i	= 0x00;

			/* 값 초기화 */
			temp_led= UInt16.MaxValue;
			temp_bd	= UInt16.MaxValue;
			if (ph_no < 0x01 || ph_no > Constants.g_MaxPH)	return;

			for (; i<0x04; i++)
			{
				if (temp_led > light_source_driver_temp_led[ph_no-1].value[i])
				{
					temp_led = light_source_driver_temp_led[ph_no-1].value[i];
				}
				if (temp_bd > light_source_driver_temp_board[ph_no-1].value[i])
				{
					temp_bd = light_source_driver_temp_board[ph_no-1].value[i];
				}
			}
		}
	};

	/* ----------------------------------------------------------------------------------------- */
	/*                                  Comm.Management (0xA05)                                  */
	/* ----------------------------------------------------------------------------------------- */

	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_LDCM /* luria_data_comm_management */
	{
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public Byte[]		server_ip;			/* Announce Server Ip Addr (The IP address of the UI (announce server) that the Luria (announce client) will send announcements to) */
		public Byte			announce_status;	/* 1:Enabled, 0:Disabled (The announcements will be automatically disabled by Luria if no announcements could be sent three times in a row) */
	};

	/* ----------------------------------------------------------------------------------------- */
	/*                                      System (0xA07)                                       */
	/* ----------------------------------------------------------------------------------------- */

	/* SSDSmartEntry of System (0xA7) */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_LSSE /* luria_system_ssd_entry */
	{
		public Byte			id;
		public Byte			flag;
		public Byte			pad1;
		public Byte			worst_value;
		public UInt32		data;
		public Byte			attrib;
		public Byte			pad2;
		public Byte			threadhold;
	};

	/* SSDSmartData of System (0xA7) */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_LSSD /* luria_system_ssd_data */
	{
		public Byte			ssd_no;
		public Byte			read_ok;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxSsdSmartData)]
		public STG_LSSE[]	data;	/* fixed. 14 ea */
	};

	/* Luria Config - System Status (0xA7) (version 2.3.0 or Later (Newer) 기준) */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_LDSS /* luria_data_system_status */
	{
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public UInt16[]		ph_mainboard_temp;				/* Photohead 8개의 Mainboard Temperature (실제 값은 나누기 10을 해줘야 됨) */
																/* 이 값이 만약 50도 이상 넘으면, Photohead 내에 냉각이 정상적으로 동작되지 않음 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public UInt32[]		get_overall_error_status_multi;	/* Photohead의 전반적인 에러 상태 값 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
		public UInt32[]		get_system_status;					/* 0th: Motor controller status bit (0: Normal) */
																/* 1th: Photo head status bit (0: Normal) */
																/* 2th: Luria PC status bit (0: Normal) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public STG_LSSD[]	get_smart_data;						/* 포토헤드 개수만큼 SSD 정보 */

		/*
		 desc : 에러 발생 여부
		 parm : None
		 retn : TRUE (Error) or FALSE (Normal)
		*/
		public Boolean IsErrorAll()
		{
			return (get_system_status[0]+get_system_status[1]+get_system_status[2]) > 0 ? true : false;
		}
		public Boolean IsErrorPH()
		{
			return get_system_status[1] > 0 ? true : false;
		}
		public Boolean IsErrorPC()
		{
			return get_system_status[2] > 0 ? true : false;
		}
	};

	/* Luria Config - Focus (0xA8) (version 2.3.0 or Later 기준) */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_LDPF /* luria_data_ph_focus */
	{
		/* State */
		public Byte			state;
		public Byte			initialized						{ get { return Bit.GetU8ToBit(state, 0x00); } }		/* command_done */
		public Byte			aaq_actived						{ get { return Bit.GetU8ToBit(state, 0x01); } }		/* drive_calibrated */
		public Byte			af_edge_trig					{ get { return Bit.GetU8ToBit(state, 0x02); } }		/* power_stage_on */
		public Byte			u8_bit_reserved					{ get { return Bit.GetU8ToBits(state, 0x03, 5); } }	/* message_code_available */

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public Byte[]		auto_focus;					/* Photohead 별로 Auto Focus에 대해 Enable 여부 */
		public UInt16		af_gain;					/* Gain/damping factor (Valid range 1 ~ 4095) */
		public UInt16		af_edge_trig_dist;			/* The max distance below or above current AF set-point that AF will be active (um) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public Int32[]		z_pos;						/* Z-position (unit: nm) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public Int32[]		trim;						/* Focus trim (unit: nm) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public Int32[]		abs_work_range_min;			/* The min absolute focus position that the AF will operate within (um) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public Int32[]		abs_work_range_max;			/* The max absolute focus position that the AF will operate within (um) */
		public UInt32		abs_work_range_status;		/* Status word giving status of autofocus absolute work range */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public UInt32[]		aaq_start_end;				/* Distance from exposure start/end where AF is inactive. (um) (index=0 : start, index=1 : end) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxPH)]
		public UInt32[]		out_dof_trig_pulse;			/* Reports the number of trigger pulses that autofocus position has been outside the given DOF (MachineConfig:DepthOfFocus) */

		/*
		 desc : Abs Work Range Status 정보 반환 (최소 혹은 최대 노광 범위를 벗어났는지 여부)
		 parm : ph_no	- [in]  Photohead Number (1-based. Max 8)
				type	- [in]  최소 혹은 최대 값을 벗어났는지 여부 즉, 0 : below_min, 1 : above max
		 retn : TRUE or FALSE
		*/
		public Boolean IsWorkRangeStatusValid(Byte ph_no, Byte type)
		{
		if (ph_no < 0x01 || ph_no > Constants.g_MaxPH)	return false;
			return (abs_work_range_status & (0x00000001 << ((ph_no - 0x01) + Constants.g_MaxPH * type))) > 0;
		}
	};

	/* Luria Shared Memory - All */
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct STG_LDSM /* luria_data_shared_memory */
	{
		public Byte			last_send_fid;
		public Byte			last_send_uid;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxCmdFamilyID)]
		public STG_LDSP[]	pkt_cmd;
		public STG_LDMC		machine;
		public STG_LDJM		jobmgt;
		public STG_LDPP		panel;
		public STG_LDEW		exposure;
		public STG_LDDP		directph;
		public STG_LDCM		commmgt;
		public STG_LDSS		system;
		public STG_LDPF		focus;

		public STG_DLSM		link;	/* 통신 상태 */

		/*
		 desc : Check if there are any recently received packets
		 parm : f_id	- [in]  Family ID (0xA0 ~ 0xA9)
				u_id	- [in]  User ID (0x01 ~ 0xff)
		 retn : TRUE or FALSE
		*/
		public Boolean IsRecvCmd(Byte f_id, Byte u_id)
		{
#if (false)
			return pkt_cmd[f_id - (Byte)ENG_LUDF.en_machine_config,u_id] > 0x00 ? true : false;
#else
			return pkt_cmd[f_id-(Byte)ENG_LUDF.en_machine_config].user_id[u_id] > 0x00 ? true : false;
#endif
		}
	};


	/* Dynamic Panel Data */
	[StructLayout(LayoutKind.Sequential, Pack = 8)]
	public struct STG_XDPD /* xml_dynamic_panel_data */
	{
		public Byte		panel_head;
		public Byte		inverse				{ get { return Bit.GetU8ToBit(panel_head, 0x00); } }			/* 0 : FALSE, 1 : TRUE */
		public Byte		content				{ get { return Bit.GetU8ToBits(panel_head, 0x01, 0x02); } }		/* 0 : Serial Number, 1 : Scale Info, 2 : General Text */
		public Byte		type				{ get { return Bit.GetU8ToBits(panel_head, 0x03, 0x02); } }		/* 0 : normal, 1 : QR Code, 2: DC Code */
		public Byte		u8_bit_reserved		{ get { return Bit.GetU8ToBits(panel_head, 0x05, 0x03); } }
		public Byte		field_cnt;			/* D Code 하위에 존재하는 Field 좌표 정보 개수 (최대 255) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
		public Byte[]	u8_reserved;

		public UInt16	d_code;
		public UInt16	rotation_head;
		public UInt16	rotation			{ get { return Bit.GetU8ToBits(panel_head, 0x00, 0x09); } }		/* Rotation Angle (Degree) */
		public UInt16	u16_bit_reserved	{ get { return Bit.GetU8ToBits(panel_head, 0x09, 0x07); } }
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public UInt16[]	u16_reserved;

		public UInt32	size_x;				/* Field Size. Horizontal*/
		public UInt32	size_y;				/* Field Size. Vertical*/

		/*
		 desc : Whether to set a value
		 parm : None
		 retn : TRUE or FALSE
		*/
		public Boolean IsValid()
		{
			return (d_code > 0 && size_x > 0 && size_y > 0 && field_cnt > 0) ? true : false;
		}
	};

	/* XML에서 읽어들인 마크 구성 정보 */
	[StructLayout(LayoutKind.Sequential, Pack = 8)]
	public struct STG_XMXY /* xml_mark_x_y */
	{
		public UInt32	org_id;		/* Sorting 이전에 저장된 메모리 저장 위치 (XML 파일로부터 읽어들인 순서) */
		public UInt32	tgt_id;		/* Sorting 이후에 저장된 메모리 저장 위치 (얼라인 마크 검사를 위한 순서) */
		public Double	mark_x;		/* 단위: mm */
		public Double	mark_y;		/* 단위: mm */
	};
}

/* --------------------------------------------------------------------------------------------- */
/*                                         Engine -> PLC                                         */
/* --------------------------------------------------------------------------------------------- */
namespace TGen2i.Struct.PLC
{
	/* Digital Input Bit Alarm */
	[StructLayout(LayoutKind.Sequential, Pack = 2)]
	public struct STG_IDBA /* input_digital_bit_alarm */
	{
		/* 1100.0 ~ 1100.F */
		public UInt16	D1100;
		public Byte		spd_fail_alarm						{ get { return Bit.GetU16ToBit(D1100, 0x00); } }	/* SPD는 서지보호기를 뜻함, 고장시 교체 및 점검 필요 */
		public Byte		linear_x_axis_power_line_cp100		{ get { return Bit.GetU16ToBit(D1100, 0x01); } }	/* CP 번호는 차단기와 차단기 번호를 뜻함, 해당 차단기의 ON/OFF 유무 */
		public Byte		linear_y_axis_power_line_cp101		{ get { return Bit.GetU16ToBit(D1100, 0x02); } }
		public Byte		linear_z1_z3_axis_smps_power_fail	{ get { return Bit.GetU16ToBit(D1100, 0x03); } }	/* SMPS FAIL = 파워서플라이 모듈에 문제가 있음 */
		public Byte		linear_z1_z3_power_cp103_cp105		{ get { return Bit.GetU16ToBit(D1100, 0x04); } }
		public Byte		optics_1_smps_power_fail			{ get { return Bit.GetU16ToBit(D1100, 0x05); } }
		public Byte		optics_2_smps_power_fail			{ get { return Bit.GetU16ToBit(D1100, 0x06); } }
		public Byte		optics_1_2_power_cp107_cp109		{ get { return Bit.GetU16ToBit(D1100, 0x07); } }
		public Byte		robot_prealigner_power_cp110_cp112	{ get { return Bit.GetU16ToBit(D1100, 0x08); } }
		public Byte		all_sd2s_control_power_cp116		{ get { return Bit.GetU16ToBit(D1100, 0x09); } }	/* SD2S란 모션 드라이브 */
		public Byte		mc2_power_cp117						{ get { return Bit.GetU16ToBit(D1100, 0x0A); } }	/* MC2란 모션 마스터 컨트롤러 */
		public Byte		liftpin_axis_cp119					{ get { return Bit.GetU16ToBit(D1100, 0x0B); } }
		public Byte		vision_system_power_cp122			{ get { return Bit.GetU16ToBit(D1100, 0x0C); } }
		public Byte		efu_modul_power_cp123				{ get { return Bit.GetU16ToBit(D1100, 0x0D); } }
		public Byte		server_pc_1_2_power_cp124_cp125		{ get { return Bit.GetU16ToBit(D1100, 0x0E); } }
		public Byte		front_safety_cover_1				{ get { return Bit.GetU16ToBit(D1100, 0x0F); } }	/* COVER란 장비 커버를 뜻함 */
		/* 1101.0 ~ 1101.F */
		public UInt16	D1101;
		public Byte		front_safety_cover_2				{ get { return Bit.GetU16ToBit(D1101, 0x00); } }
		public Byte		front_safety_cover_3				{ get { return Bit.GetU16ToBit(D1101, 0x01); } }
		public Byte		front_safety_cover_4				{ get { return Bit.GetU16ToBit(D1101, 0x02); } }
		public Byte		rear_saferty_cover_1				{ get { return Bit.GetU16ToBit(D1101, 0x03); } }
		public Byte		rear_saferty_cover_2				{ get { return Bit.GetU16ToBit(D1101, 0x04); } }
		public Byte		rear_saferty_cover_3				{ get { return Bit.GetU16ToBit(D1101, 0x05); } }
		public Byte		rear_saferty_cover_4				{ get { return Bit.GetU16ToBit(D1101, 0x06); } }
		public Byte		rear_saferty_cover_5				{ get { return Bit.GetU16ToBit(D1101, 0x07); } }
		public Byte		utility_box_water_leak_sensor		{ get { return Bit.GetU16ToBit(D1101, 0x08); } }	/* 해당 장소에서 리크센서에 액체가 감지됨 */
		public Byte		ph_water_leak_sensor_1				{ get { return Bit.GetU16ToBit(D1101, 0x09); } }
		public Byte		ph_water_leak_sensor_2				{ get { return Bit.GetU16ToBit(D1101, 0x0A); } }
		public Byte		water_flow_low_alarm				{ get { return Bit.GetU16ToBit(D1101, 0x0B); } }	/* 물 유량 알람 */
		public Byte		water_flow_high_alarm				{ get { return Bit.GetU16ToBit(D1101, 0x0C); } }	/* 물 유량 알람 */
		public Byte		water_pressure_low_alarm			{ get { return Bit.GetU16ToBit(D1101, 0x0D); } }	/* 물 압력 알람 */
		public Byte		water_pressure_high_alarm			{ get { return Bit.GetU16ToBit(D1101, 0x0E); } }	/* 물 압력 알람 */
		public Byte		air_pressure_low_alarm				{ get { return Bit.GetU16ToBit(D1101, 0x0F); } }	/* 메인 공압 알람 */
		/* 1102.0 ~ 1102.F */
		public UInt16	D1102;
		public Byte		halogen_ring_light_burn_out_alarm	{ get { return Bit.GetU16ToBit(D1102, 0x00); } }	/* 할로겐 램프 고장 알람 */
		public Byte		halogen_coaxial_light_burn_out_alarm{ get { return Bit.GetU16ToBit(D1102, 0x01); } }
		public Byte		lift_pin_x_axis_drive_alarm			{ get { return Bit.GetU16ToBit(D1102, 0x02); } }	/* 리프트핀 X축 드라이브 하드웨어 알람 */
		public Byte		lift_pin_x_axis_operation_alarm		{ get { return Bit.GetU16ToBit(D1102, 0x03); } }	/* 리프트핀 X축 구동 시에 SW알람 */
		public Byte		lift_pin_x_axis_home_request_alarm	{ get { return Bit.GetU16ToBit(D1102, 0x04); } }	/* 리프트핀 X축 원점 복귀 알람 */
		public Byte		lift_pin_z_axis_drive_alarm			{ get { return Bit.GetU16ToBit(D1102, 0x05); } }	
		public Byte		lift_pin_z_axis_operation_alarm		{ get { return Bit.GetU16ToBit(D1102, 0x06); } }	
		public Byte		lift_pin_z_axis_home_request_alarm	{ get { return Bit.GetU16ToBit(D1102, 0x07); } }	
		public Byte		bsa_z_axis_operation_alarm			{ get { return Bit.GetU16ToBit(D1102, 0x08); } }	
		public Byte		bsa_z_axis_home_request_alarm		{ get { return Bit.GetU16ToBit(D1102, 0x09); } }	
		public Byte		led_lamp_controller_power_cp127		{ get { return Bit.GetU16ToBit(D1102, 0x0A); } }	
		public Byte		lift_pin_x_not_move_alarm			{ get { return Bit.GetU16ToBit(D1102, 0x0B); } }	/* 포지션 인터락 알람 (충돌 위치 명령 시): 해제방법 : RESET */
		public Byte		lift_pin_z_not_move_alarm			{ get { return Bit.GetU16ToBit(D1102, 0x0C); } }	/* 포지션 인터락 알람 (충돌 위치 명령 시): 해제방법 : RESET */
		public Byte		lift_pin_x_position_count_alarm		{ get { return Bit.GetU16ToBit(D1102, 0x0D); } }	/* 포지션 이동할 시 스테핑모터의 레졸버(EX.엔코더)카운터 값 비교 후 4pulse(2.4um)초과 시 알람 */
		public Byte		lift_pin_z_position_count_alarm		{ get { return Bit.GetU16ToBit(D1102, 0x0E); } }	/* 허용가능 오차범위 4pulse(2.4um)이하 */
		public Byte		z1_z3_air_pressure_low_alarm		{ get { return Bit.GetU16ToBit(D1102, 0x0F); } }
		/* 1103.0 ~ 1103.F */
		public UInt16	spare_word_d1103;
	};

	/* Digital Input Bit Warn */
	[StructLayout(LayoutKind.Sequential, Pack = 2)]
	public struct STG_IDBW /* input_digital_bit_warn */
	{
		/* 1104.0 ~ 1104.F */
		public UInt16	D1104;
		public Byte		pc_rack_upper_out_fan_alarm_1		{ get { return Bit.GetU16ToBit(D1104, 0x00); } }			/* 해당 장소의 FAN 고장알람 */
		public Byte		pc_rack_upper_out_fan_alarm_2		{ get { return Bit.GetU16ToBit(D1104, 0x01); } }
		public Byte		pc_rack_bottom_out_fan_alarm_1		{ get { return Bit.GetU16ToBit(D1104, 0x02); } }
		public Byte		pc_rack_bottom_out_fan_alarm_2		{ get { return Bit.GetU16ToBit(D1104, 0x03); } }
		public Byte		power_box_in_fan_alarm_1			{ get { return Bit.GetU16ToBit(D1104, 0x04); } }
		public Byte		power_box_in_fan_alarm_2			{ get { return Bit.GetU16ToBit(D1104, 0x05); } }
		public Byte		power_box_in_fan_alarm_3			{ get { return Bit.GetU16ToBit(D1104, 0x06); } }
		public Byte		power_box_in_fan_alarm_4			{ get { return Bit.GetU16ToBit(D1104, 0x07); } }
		public Byte		power_box_out_fan_alarm_1			{ get { return Bit.GetU16ToBit(D1104, 0x08); } }
		public Byte		power_box_out_fan_alarm_2			{ get { return Bit.GetU16ToBit(D1104, 0x09); } }
		public Byte		power_box_out_fan_alarm_3			{ get { return Bit.GetU16ToBit(D1104, 0x0A); } }
		public Byte		power_box_out_fan_alarm_4			{ get { return Bit.GetU16ToBit(D1104, 0x0B); } }
		public Byte		spare_d1104_c						{ get { return Bit.GetU16ToBit(D1104, 0x0C); } }
		public Byte		spare_d1104_d						{ get { return Bit.GetU16ToBit(D1104, 0x0D); } }
		public Byte		spare_d1104_e						{ get { return Bit.GetU16ToBit(D1104, 0x0E); } }
		public Byte		spare_d1104_f						{ get { return Bit.GetU16ToBit(D1104, 0x0F); } }
		/* 1105.0 ~ 1105.F */
		public UInt16	spare_word_1105;
	};

	/* Digital Input Bit Normal */
	[StructLayout(LayoutKind.Sequential, Pack = 2)]
	public  struct STG_IDBN	/* input_digital_bit_normal */
	{
		/* 1106.0 ~ 1106.F */
		public UInt16	D1106;
		public Byte		auto_maint_mode						{ get { return Bit.GetU16ToBit(D1106, 0x00); } }	/* 자동/메인트 키스위치 MODE */
		public Byte		wafer_chuck_vacuum_status			{ get { return Bit.GetU16ToBit(D1106, 0x01); } }	/* 웨이퍼 흡착 완료 */
		public Byte		wafer_chuck_atm_status				{ get { return Bit.GetU16ToBit(D1106, 0x02); } }	/* 웨이퍼 진공 해제로 탈착 가능조건 */
		public Byte		robot_vacuum_status					{ get { return Bit.GetU16ToBit(D1106, 0x03); } }
		public Byte		robot_atm_status					{ get { return Bit.GetU16ToBit(D1106, 0x04); } }
		public Byte		aligner_vacuum_status				{ get { return Bit.GetU16ToBit(D1106, 0x05); } }
		public Byte		aligner_atm_status					{ get { return Bit.GetU16ToBit(D1106, 0x06); } }
		public Byte		safety_reset_possible_status		{ get { return Bit.GetU16ToBit(D1106, 0x07); } }	/* 인터락 해제 후 SAFETY RESET 요구조건이 갖춰 졌을시 켜짐 */
		public Byte		spare_d1106_8						{ get { return Bit.GetU16ToBit(D1106, 0x08); } }
		public Byte		spare_d1106_9						{ get { return Bit.GetU16ToBit(D1106, 0x09); } }
		public Byte		stage_wafer_detact					{ get { return Bit.GetU16ToBit(D1106, 0x0A); } }
		public Byte		buffer_wafer_in_detact				{ get { return Bit.GetU16ToBit(D1106, 0x0B); } }
		public Byte		buffer_wafer_out_detact				{ get { return Bit.GetU16ToBit(D1106, 0x0C); } }
		public Byte		robot_output_signal_1_ex_alarm		{ get { return Bit.GetU16ToBit(D1106, 0x0D); } }
		public Byte		robot_output_signal_2				{ get { return Bit.GetU16ToBit(D1106, 0x0E); } }
		public Byte		robot_output_signal_3				{ get { return Bit.GetU16ToBit(D1106, 0x0F); } }
		/* 1107.0 ~ 1107.F */
		public UInt16	D1107;
		public Byte		robot_output_signal_4				{ get { return Bit.GetU16ToBit(D1107, 0x00); } }
		public Byte		robot_output_signal_5				{ get { return Bit.GetU16ToBit(D1107, 0x01); } }
		public Byte		robot_output_signal_6				{ get { return Bit.GetU16ToBit(D1107, 0x02); } }
		public Byte		robot_output_signal_7				{ get { return Bit.GetU16ToBit(D1107, 0x03); } }
		public Byte		robot_output_signal_8				{ get { return Bit.GetU16ToBit(D1107, 0x04); } }
		public Byte		spare_d1107_5						{ get { return Bit.GetU16ToBit(D1107, 0x05); } }
		public Byte		signal_tower_green_lamp				{ get { return Bit.GetU16ToBit(D1107, 0x06); } }	/* 시그날 타워의 최종 동작값을 PC 에 알려줌 */
		public Byte		signal_tower_yellow_lamp_flashing	{ get { return Bit.GetU16ToBit(D1107, 0x07); } }
		public Byte		signal_tower_red_lamp_flashing		{ get { return Bit.GetU16ToBit(D1107, 0x08); } }
		public Byte		signal_tower_buzzer_1				{ get { return Bit.GetU16ToBit(D1107, 0x09); } }
		public Byte		xy_stage_move_prohibit_status		{ get { return Bit.GetU16ToBit(D1107, 0x0A); } }	/* XY STAGE MOVE PHOHIBIT 상태를 알 수 있음 */
		public Byte		spare_d1107_b						{ get { return Bit.GetU16ToBit(D1107, 0x0B); } }
		public Byte		spare_d1107_c						{ get { return Bit.GetU16ToBit(D1107, 0x0C); } }
		public Byte		spare_d1107_d						{ get { return Bit.GetU16ToBit(D1107, 0x0D); } }
		public Byte		spare_d1107_e						{ get { return Bit.GetU16ToBit(D1107, 0x0E); } }
		public Byte		spare_d1107_f						{ get { return Bit.GetU16ToBit(D1107, 0x0F); } }
		/* 1108.0 ~ 1108.F */
		public UInt16	spare_word_d1108;
	};

	/* TEL Interface Input (Track -> Exposure) */
	[StructLayout(LayoutKind.Sequential, Pack = 2)]
	public struct STG_IDTE	/* input_digital_track_exposure */
	{
		/* 1109.0 ~ 1109.F */
		public UInt16	D1109;
		public Byte		trk_ink_track_inline				{ get { return Bit.GetU16ToBit(D1109, 0x00); } }	/* 트랙에서 장비로 주는 인터페이스 신호 */
		public Byte		trk_rdy_track_ready					{ get { return Bit.GetU16ToBit(D1109, 0x01); } }
		public Byte		trk_snd_track_send					{ get { return Bit.GetU16ToBit(D1109, 0x02); } }
		public Byte		trk_lend_track_end_of_lot			{ get { return Bit.GetU16ToBit(D1109, 0x03); } }
		public Byte		trk_reserve_1						{ get { return Bit.GetU16ToBit(D1109, 0x04); } }
		public Byte		trk_reserve_2						{ get { return Bit.GetU16ToBit(D1109, 0x05); } }
		public Byte		trk_reserve_3						{ get { return Bit.GetU16ToBit(D1109, 0x06); } }
		public Byte		trk_reserve_4						{ get { return Bit.GetU16ToBit(D1109, 0x07); } }
		public Byte		trk_reserve_5						{ get { return Bit.GetU16ToBit(D1109, 0x08); } }
		public Byte		spare_d1109_9						{ get { return Bit.GetU16ToBit(D1109, 0x09); } }
		public Byte		spare_d1109_a						{ get { return Bit.GetU16ToBit(D1109, 0x0A); } }
		public Byte		spare_d1109_b						{ get { return Bit.GetU16ToBit(D1109, 0x0B); } }
		public Byte		spare_d1109_c						{ get { return Bit.GetU16ToBit(D1109, 0x0C); } }
		public Byte		spare_d1109_d						{ get { return Bit.GetU16ToBit(D1109, 0x0D); } }
		public Byte		spare_d1109_e						{ get { return Bit.GetU16ToBit(D1109, 0x0E); } }
		public Byte		spare_d1109_f						{ get { return Bit.GetU16ToBit(D1109, 0x0F); } }
	};
	[StructLayout (LayoutKind.Explicit)]
	public struct UNG_IDTE	/* input_digital_track_exposure */
	{
		[FieldOffset (0)]
		public STG_IDTE	s_scan;								/* 구조체 값 */
		[FieldOffset (0)]
		public UInt16	w_scan;								/* 변수 값 */
	};

	/* Track & Expose State */
	[StructLayout (LayoutKind.Sequential, Pack = 2)]
	public struct STG_STSE /* state_track_state_expo */
	{
		public UNG_IDTE	in_track_expo;
		public UNG_ODET	out_expo_track;
	};

	/* Analog Input Value Normal */
	[StructLayout(LayoutKind.Sequential, Pack = 2)]
	public struct STG_IAVN	/* input_analog_value_normal (D1110 ~ D1119) */
	{
		public Int16										differential_pressure;					/* -500 ~ +500 (-50.0 ~ +50.0 pa. 1 -> 0.1 ) */
		public Int16										monitoring_temperature;					/* -300 ~ +1000 (-30.0 ~ +100.0 degree. 1 -> 0.1) */
		public Int16										z1_z3_axis_cooling_air_pressure;		/* -100 ~ +1000 (1 -> 1) kPa */
		public Int16										z1_z3_axis_cyclinder_air_pressure;		/* -100 ~ +1000 (1 -> 1) kPa */
		public UInt16										power_box_temperature;					/* 0 ~ 12000 (0.0 ~ +1200.0. 1 -> 0.1 ℃)*/
		public UInt16										pc_rack_upper_monitoring_temperature;	/* 0 ~ 1000 (0.0 ~ +1200.0. 1 -> 0.1 ℃)*/
		public UInt16										pc_rack_bottom_monitoring_temperature;	/* 0 ~ 1000 (0.0 ~ +1200.0. 1 -> 0.1 ℃)*/
		public UInt16										spare_word_d1117;
		public UInt16										spare_word_d1118;
		public UInt16										spare_word_d1119;
	};

	/* Read Lift Pin [ X / Z / BSA ] Axis (Lamp) Bit */
	[StructLayout(LayoutKind.Sequential, Pack = 2)]
	public struct STG_ILAB	/* input_lift_axis_bit */
	{
		/* 1120.0 ~ 1120.F */
		public UInt16	D1120_D1122;
		public Byte		axis_status							{ get { return Bit.GetU16ToBit(D1120_D1122, 0x00); } }
		public Byte		axis_p_limit_position				{ get { return Bit.GetU16ToBit(D1120_D1122, 0x01); } }
		public Byte		axis_n_limit_psition				{ get { return Bit.GetU16ToBit(D1120_D1122, 0x02); } }
		public Byte		axis_position						{ get { return Bit.GetU16ToBit(D1120_D1122, 0x03); } }
		public Byte		axis_loading_position				{ get { return Bit.GetU16ToBit(D1120_D1122, 0x04); } }	/* 해당축 로딩포지션 센서에 감지함 */
		public Byte		axis_safety_position				{ get { return Bit.GetU16ToBit(D1120_D1122, 0x05); } }	/* 해당축 안전위치 센서에 감지함 => MC2 MOTION 이동 가능 조건 */
		public Byte		spare_D1120_6						{ get { return Bit.GetU16ToBit(D1120_D1122, 0x06); } }
		public Byte		spare_D1120_7						{ get { return Bit.GetU16ToBit(D1120_D1122, 0x07); } }
		public Byte		spare_D1120_8						{ get { return Bit.GetU16ToBit(D1120_D1122, 0x08); } }
		public Byte		spare_D1120_9						{ get { return Bit.GetU16ToBit(D1120_D1122, 0x09); } }
		public Byte		spare_D1120_a						{ get { return Bit.GetU16ToBit(D1120_D1122, 0x0A); } }
		public Byte		spare_D1120_b						{ get { return Bit.GetU16ToBit(D1120_D1122, 0x0B); } }
		public Byte		spare_D1120_c						{ get { return Bit.GetU16ToBit(D1120_D1122, 0x0C); } }
		public Byte		spare_D1120_d						{ get { return Bit.GetU16ToBit(D1120_D1122, 0x0D); } }
		public Byte		spare_D1120_e						{ get { return Bit.GetU16ToBit(D1120_D1122, 0x0E); } }
		public Byte		spare_D1120_f						{ get { return Bit.GetU16ToBit(D1120_D1122, 0x0F); } }
	};

	/* Read Lift Pin [ X / Z / BSA ] Axis (Lamp) Word (Value) */
	[StructLayout(LayoutKind.Sequential, Pack = 2)]
	public struct STG_ILAV	/* input_lift_axis_value */
	{
		public UInt32	axis_currnt_position_display;
		public UInt16	axis_alarm_code;
	};

	/* Digital Output Bit Normal */
	[StructLayout(LayoutKind.Sequential, Pack = 2)]
	public struct STG_ODBN	/*output_digital_bit_normal */
	{
		/* 1150.0 ~ 1150.F */
		public UInt16	D1150;
		public Byte	safety_reset_cmd						{ get { return Bit.GetU16ToBit(D1150, 0x00); } }	/* 안전 리셋 명령 D1106.7(SAFETY RESET POSSIBLE STATUS)를 OFF 할 수 있음. */
		public Byte	tower_green_lamp_cmd					{ get { return Bit.GetU16ToBit(D1150, 0x01); } }	/* 타워 램프&부저 ON/OFF 명령 */
		public Byte	tower_yellow_lamp_flashing_cmd			{ get { return Bit.GetU16ToBit(D1150, 0x02); } }
		public Byte	tower_red_lamp_flashing_cmd				{ get { return Bit.GetU16ToBit(D1150, 0x03); } }
		public Byte	tower_buzzer_1_cmd						{ get { return Bit.GetU16ToBit(D1150, 0x04); } }
		public Byte	z1_axis_air_cooling_on_off				{ get { return Bit.GetU16ToBit(D1150, 0x05); } }	/* Z축 에어 쿨링 (장비 MAINT상황이 아니라면 항시 ON) */
		public Byte	z2_axis_air_cooling_on_off				{ get { return Bit.GetU16ToBit(D1150, 0x06); } }
		public Byte	z3_axis_air_cooling_on_off				{ get { return Bit.GetU16ToBit(D1150, 0x07); } }
		public Byte	z1_axis_cylinder_on_off					{ get { return Bit.GetU16ToBit(D1150, 0x08); } }	/* Z축 실린더 ON (장비 MAINT상황이 아니라면 항시 ON) */
		public Byte	z2_axis_cylinder_on_off					{ get { return Bit.GetU16ToBit(D1150, 0x09); } }
		public Byte	z3_axis_cylinder_on_off					{ get { return Bit.GetU16ToBit(D1150, 0x0A); } }
		public Byte	wafer_chuck_vacuum_on_off				{ get { return Bit.GetU16ToBit(D1150, 0x0B); } }	/* 웨이퍼 흡착 ON/OFF */
		public Byte	wafer_chuck_exhaust_on_off				{ get { return Bit.GetU16ToBit(D1150, 0x0C); } }	/* 웨이퍼 탈착 ON (흡착도 OFF해야함) */
		public Byte	robot_vacuumuum_on_off					{ get { return Bit.GetU16ToBit(D1150, 0x0D); } }
		public Byte	aligner_vacuum_on_off					{ get { return Bit.GetU16ToBit(D1150, 0x0E); } }
		public Byte	photohead_1_power_off_cmd				{ get { return Bit.GetU16ToBit(D1150, 0x0F); } }	/* 광학계의 전원을 OFF할수 있음 */
		/* 1151.0 ~ 1151.F */
		public UInt16	D1151;
		public Byte	photohead_2_power_off_cmd				{ get { return Bit.GetU16ToBit(D1151, 0x00); } }
		public Byte	illumination_sensor_pwr_on_off			{ get { return Bit.GetU16ToBit(D1151, 0x01); } }	/* 조도계의 전원을 ON/OFF 할수 있음 */
		public Byte	halogen_ring_light_power_on				{ get { return Bit.GetU16ToBit(D1151, 0x02); } }	/* 할로겐 램프의 전원을 킴 */
		public Byte	halogen_coaxial_light_power_on			{ get { return Bit.GetU16ToBit(D1151, 0x03); } }
		public Byte	spare_d1151_3							{ get { return Bit.GetU16ToBit(D1151, 0x04); } }
		public Byte	spare_d1151_4							{ get { return Bit.GetU16ToBit(D1151, 0x05); } }
		public Byte	robot_input_signal_1					{ get { return Bit.GetU16ToBit(D1151, 0x06); } }	/* 로봇에게 받는 신호(현재는 사용하지 않음) */
		public Byte	robot_input_signal_2					{ get { return Bit.GetU16ToBit(D1151, 0x07); } }
		public Byte	robot_input_signal_3					{ get { return Bit.GetU16ToBit(D1151, 0x08); } }
		public Byte	robot_input_signal_4					{ get { return Bit.GetU16ToBit(D1151, 0x09); } }
		public Byte	robot_input_signal_5					{ get { return Bit.GetU16ToBit(D1151, 0x0A); } }
		public Byte	robot_input_signal_6					{ get { return Bit.GetU16ToBit(D1151, 0x0B); } }
		public Byte	robot_input_signal_7					{ get { return Bit.GetU16ToBit(D1151, 0x0C); } }
		public Byte	robot_input_signal_8					{ get { return Bit.GetU16ToBit(D1151, 0x0D); } }
		public Byte	robot_input_signal_9					{ get { return Bit.GetU16ToBit(D1151, 0x0E); } }
		public Byte	robot_input_signal_10					{ get { return Bit.GetU16ToBit(D1151, 0x0F); } }
		/* 1152.0 ~ 1152.F */
		public UInt16	D1152;
		public Byte	xy_stage_move_prohibit					{ get { return Bit.GetU16ToBit(D1152, 0x00); } }	/* 장비에서 트랙으로 주는 인터페이스 신호 */
		public Byte	xy_stage_move_prohibit_release			{ get { return Bit.GetU16ToBit(D1152, 0x01); } }
		public Byte	spare_d1152_2							{ get { return Bit.GetU16ToBit(D1152, 0x02); } }
		public Byte	spare_d1152_3							{ get { return Bit.GetU16ToBit(D1152, 0x03); } }
		public Byte	spare_d1152_4							{ get { return Bit.GetU16ToBit(D1152, 0x04); } }
		public Byte	spare_d1152_5							{ get { return Bit.GetU16ToBit(D1152, 0x05); } }
		public Byte	spare_d1152_6							{ get { return Bit.GetU16ToBit(D1152, 0x06); } }
		public Byte	spare_d1152_7							{ get { return Bit.GetU16ToBit(D1152, 0x07); } }
		public Byte	spare_d1152_8							{ get { return Bit.GetU16ToBit(D1152, 0x08); } }
		public Byte	spare_d1152_9							{ get { return Bit.GetU16ToBit(D1152, 0x09); } }
		public Byte	spare_d1152_a							{ get { return Bit.GetU16ToBit(D1152, 0x0A); } }
		public Byte	spare_d1152_b							{ get { return Bit.GetU16ToBit(D1152, 0x0B); } }
		public Byte	spare_d1152_c							{ get { return Bit.GetU16ToBit(D1152, 0x0C); } }
		public Byte	spare_d1152_d							{ get { return Bit.GetU16ToBit(D1152, 0x0D); } }
		public Byte	spare_d1152_e							{ get { return Bit.GetU16ToBit(D1152, 0x0E); } }
		public Byte	spare_d1152_f							{ get { return Bit.GetU16ToBit(D1152, 0x0F); } }
	};

	/* TEL Interface Input (Track -> Exposure) */
	[StructLayout(LayoutKind.Sequential, Pack = 2)]
	public struct STG_ODET	/* output_digital_exposure_track */
	{
		/* 1153.0 ~ 1153.F */
		public UInt16	D1153;
		public Byte	exp_ink_exposure_inline					{ get { return Bit.GetU16ToBit(D1153, 0x00); } }
		public Byte	exp_rej_exposure_reject					{ get { return Bit.GetU16ToBit(D1153, 0x01); } }
		public Byte	exp_snd_exposure_send_to_ready			{ get { return Bit.GetU16ToBit(D1153, 0x02); } }
		public Byte	exp_rdy_exposure_receive_to_ready		{ get { return Bit.GetU16ToBit(D1153, 0x03); } }
		public Byte	exp_reserve_1							{ get { return Bit.GetU16ToBit(D1153, 0x04); } }
		public Byte	exp_reserve_2							{ get { return Bit.GetU16ToBit(D1153, 0x05); } }
		public Byte	exp_reserve_3							{ get { return Bit.GetU16ToBit(D1153, 0x06); } }
		public Byte	exp_reserve_4							{ get { return Bit.GetU16ToBit(D1153, 0x07); } }
		public Byte	exp_reserve_5							{ get { return Bit.GetU16ToBit(D1153, 0x08); } }
		public Byte	spare_d1153_9							{ get { return Bit.GetU16ToBit(D1153, 0x09); } }
		public Byte	spare_d1153_a							{ get { return Bit.GetU16ToBit(D1153, 0x0A); } }
		public Byte	spare_d1153_b							{ get { return Bit.GetU16ToBit(D1153, 0x0B); } }
		public Byte	spare_d1153_c							{ get { return Bit.GetU16ToBit(D1153, 0x0C); } }
		public Byte	spare_d1153_d							{ get { return Bit.GetU16ToBit(D1153, 0x0D); } }
		public Byte	spare_d1153_e							{ get { return Bit.GetU16ToBit(D1153, 0x0E); } }
		public Byte	spare_d1153_f							{ get { return Bit.GetU16ToBit(D1153, 0x0F); } }

	};
	[StructLayout (LayoutKind.Explicit)]
	public struct UNG_ODET	/* output_digital_exposure_track */
	{
		[FieldOffset (0)]
		public STG_ODET	s_track;			/*_구조체_값_*/
		[FieldOffset (0)]
		public UInt16	w_track;			/*_변수_값_*/
	};

	/* Analog Output Value Normal */
	[StructLayout(LayoutKind.Sequential, Pack = 2)]
	public struct STG_OAVN	/* output_analog_value_normal */
	{
		public UInt16	halogen_ring_light_power;
		public UInt16	halogen_coaxial_light_power;
	};

	/* Analog Output Value Etc */
	[StructLayout(LayoutKind.Sequential, Pack = 2)]
	public struct STG_OAVE	/* output_analog_value_etc */
	{
		public UInt16	pc_live_check;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
		public UInt16[] spare_word;
	};

	/* Write Lift Pin [ X / Z / BSA ] Axis (Lamp) Bit */
	[StructLayout(LayoutKind.Sequential, Pack = 2)]
	public struct STG_OLAB	/* output_lift_axis_bit */
	{
		/* 1160.0 ~ 1162.F */
		public UInt16	D1160_D1162;
		public Byte	axis_enable_disable					{ get { return Bit.GetU16ToBit(D1160_D1162, 0x00); } }	/* PLC가 자동으로 ON 함 */
		public Byte	axis_reset							{ get { return Bit.GetU16ToBit(D1160_D1162, 0x01); } }	/* 해당축 운영 시 감지한 알람을 리셋함 */
		public Byte	axis_stop							{ get { return Bit.GetU16ToBit(D1160_D1162, 0x02); } }
		public Byte	spare_bit_1							{ get { return Bit.GetU16ToBit(D1160_D1162, 0x03); } }
		public Byte	axis_jog_plus_move					{ get { return Bit.GetU16ToBit(D1160_D1162, 0x04); } }	/* 해당 명령은 리프트핀 z축이 SAFETY SENSOR가 감지시 사용 가능= 감지 안된 후 사용시 USER 알람 발생 */
		public Byte	axis_jog_minus_move					{ get { return Bit.GetU16ToBit(D1160_D1162, 0x05); } }
		public Byte	axis_home_position					{ get { return Bit.GetU16ToBit(D1160_D1162, 0x06); } }
		public Byte	axis_position						{ get { return Bit.GetU16ToBit(D1160_D1162, 0x07); } }
		public Byte	axis_dreve_alarm_reset				{ get { return Bit.GetU16ToBit(D1160_D1162, 0x08); } }	/* 해당축 드라이브의 하드웨어 알람을 해제함 */
		public Byte	spare_d1160_9						{ get { return Bit.GetU16ToBit(D1160_D1162, 0x09); } }
		public Byte	spare_d1160_a						{ get { return Bit.GetU16ToBit(D1160_D1162, 0x0A); } }
		public Byte	spare_d1160_b						{ get { return Bit.GetU16ToBit(D1160_D1162, 0x0B); } }
		public Byte	spare_d1160_c						{ get { return Bit.GetU16ToBit(D1160_D1162, 0x0C); } }
		public Byte	spare_d1160_d						{ get { return Bit.GetU16ToBit(D1160_D1162, 0x0D); } }
		public Byte	spare_d1160_e						{ get { return Bit.GetU16ToBit(D1160_D1162, 0x0E); } }
		public Byte	spare_d1160_f						{ get { return Bit.GetU16ToBit(D1160_D1162, 0x0F); } }
	};

	/* Write Lift Pin [ X / Z / BSA ] Axis (Lamp) Word (Value) */
	[StructLayout(LayoutKind.Sequential, Pack = 4)]
	public struct STG_OLAV	/* output_lift_axis_value */
	{
		public UInt32	axis_jog_speed_setting;
		public UInt32	axis_position_speed_setting;
		public UInt32	axis_position_setting;
	};

	/* PLC I/O 구조체 (PIO 포함) */
	[StructLayout(LayoutKind.Sequential, Pack = 2)]
	public struct STG_PMDV	/* plc_memory_data_value */
	{
		public STG_IDBA	in_alarm;
		public STG_IDBW	in_warn;
		public STG_IDBN	in_normal;
		public UNG_IDTE	in_track_expo;	/* PIO : Scanner (or Exposure) Side (Track -> Exposure) */
		public STG_IAVN	in_analog;

		public STG_ILAB	in_pin_x_bit;
		public STG_ILAB	in_pin_z_bit;
		public STG_ILAB	in_cam_z_bit;

		public STG_ILAV	in_pin_x_val;
		public STG_ILAV	in_pin_z_val;
		public STG_ILAV	in_cam_z_val;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 18)]
		public UInt16[]	u16_in_spare;	/* 1132 ~ 1149 */

		/* Output Address (D1150 ~ D1199) : Total 50 Word */
		public STG_ODBN	out_normal;
		public UNG_ODET	out_expo_track;
		public STG_OAVN	out_analog;
		public STG_OAVE	out_live_etc;

		public STG_OLAB	out_pin_x_bit;
		public STG_OLAB	out_pin_z_bit;
		public STG_OLAB	out_cam_z_bit;

		public STG_OLAV	out_pin_x_val;
		public STG_OLAV	out_pin_z_val;
		public STG_OLAV	out_cam_z_val;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 19)]
		public UInt16[]	u16_out_spare;	/* 1181 ~ 1199 */

		/*
		 desc : Comparison of input PIO state and current memory PIO state
		 parm : scan	- [in]  The PIO status of Scanner
				track	- [in]  The PIO status of Track
		 retn : true or false
		*/
		public Boolean IsEqualPIO(UInt16 scan, UInt16 track)
		{
			return (in_track_expo.w_scan == scan && out_expo_track.w_track == track);
		}
	};
}

/* --------------------------------------------------------------------------------------------- */
/*                                      Engine -> Prealigner                                     */
/* --------------------------------------------------------------------------------------------- */

namespace TGen2i.Struct.Aligner
{
	/* PreAligner Data Value */
	[StructLayout(LayoutKind.Sequential, Pack = 8)]
	public struct  STG_MPDV /* milara_prealigner_data_value */
	{
#if (false)
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_LastStringDataSize)]
		public Byte[]				last_string_data;
#else
		[MarshalAs(UnmanagedType.ByValTStr, SizeConst = Constants.g_LastStringDataSize)]
		public string				last_string_data;
#endif
		public SByte				wafer_type;					/* -2: 외부 센서에 의해 그려진 Notch, -1: Notch or Flat이 없는 Wafer, 0 : Notch가 있는 Wafer, 1 : 1개 이상 Flat이 있는 Wafer, 2: 정사각형 기판이 있는 Wafer, */
		public SByte				wafer_size_inch;			/* 현재 이 값 (Wafer Size (inch)) 으로 자동 사용되도록 파라미터들 설정 됨 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]
		public SByte[]				i8_reserved;

		public Byte					info_mode;					/* 0 ~ 3. 0 : 응답 대기, 1 ~ 3 : 1 사이클 끝내고 응답. 1 : Hex Code 값만 반환, 2 : '>' 값만 반환, 3 : 문자열만 반환 */
		public Byte					wafer_loaded;				/* Wafer가 Chuck에 놓여져 있는지 여부 */
		public Byte					wafer_size_detection;		/* 0x00: 자동으로 크기 감지 안함, 0x01: BAL 명령 수행할 때, 자동으로 크기 감지 함 */
		public Byte					wafer_scan_again;			/* 0x00: 정렬 검색 실패한 경우, 재검색 플래그 비활성화, 0x01: 정렬 검색 실패한 경우, 한번더 자동으로 재검색을 플래그 활성화 */
		public Byte					vacuum_state_after_align;	/* Align 동작 완료 후 Chuck 위에 소재 (Wafer)를 Vacuum 상태로 놔둘지 Off로 놔둘지 설정된 값 */
		public Byte					wafer_glass_type;			/* 0x00: 비투명한 재질의 Wafer, 0x01: 투명한 재질의 Wafer */
		public Byte					notch_depth_mode;			/* Notch의 Depth 해상도를 높일 것이지 여부 (0x00: 표준 해상도, 0x01: 해상도 더 높임) */
		public Byte					mode_z_after_move_pins;		/* MTP 명령 이후에 Z (Veritcal)축의 최종 위치 모드 (_LD (0) or _LU (1)) */

		public Byte					rotation_sample_mode;		/* T 축의 회전 Sampling Mode 값 (0:1/3, 1:Half, 2:Default, 3:2배) */
		public Byte					stop_on_pins;				/* Wafer 이송 중에 Chuck 모션을 Pins Position 지점에서 멈출게 할지 여부 제어 값 */
		public Byte					notch_light_source;			/* 0: 첫 번째 Light Source, 1: 두 번째 Light Source */
		public Byte					align_fail_auto_dumps;		/* Wafer에 대해 정렬 (BAL Command) 작업이 실패한 경우, 관련 CCD Data를 포함하여 파일을 자동으로 생성할지 여부 */
		public Byte					auto_samples_dump_file;		/* 생성하고자 하는 CCD Samples 파일에 대한 File Numbering 값 설정 */
		public Byte					is_edge_handling;			/* 0x00: Edge Idle, 0x01: Edge Handling */
		/* Current Limit (전류 한계) (unit: level. 1 (min) ~ 251 (max)) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxAlignerDriveCount)]
		public Byte[]				current_limit;				/* 0 : Rotation Axis (T), 1 : Horizon Axis (R), 2 : Veritical Axis (Z) */
		/* Velocity Profile (T/R/Z 순서) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxAlignerDriveCount)]
		public Byte[]				velocity_profile;			/* 0 : Rotation Axis (T), 1 : Horizon Axis (R), 2 : Veritical Axis (Z) */
		public Byte					wafer_min_size;				/* 현재 Prealinger가 작업 가능한 Wafer 최소/최대 크기 값 (2, 3, 4, 5, 6, 8, 12, 18 중 1개 값) */
		public Byte					wafer_max_size;				/* 현재 Prealinger가 작업 가능한 Wafer 최소/최대 크기 값 (2, 3, 4, 5, 6, 8, 12, 18 중 1개 값) */
		public Byte					wafer_work_size;			/* 현재 Prealinger에서 작업하려는 Target Wafer 크기 값 (2, 3, 4, 5, 6, 8, 12, 18 중 1개 값) */
		public Byte					notch_average_mode;			/* Notch/Flag 검색을 위한 Average Mode 값 (0: 이 기능 사용 안함, 3 or 5: 이 기능 사용 함 */

		public Byte					wafer_loading_type;			/* 0 : load wafer on pins, 1 : load wafer on chuck */
		public Byte					minimum_notch_depth;		/* Currently set minimum notch depth (Range : 1 ~ 100) */
		public Byte					notch_find_mode;			/* Currently set notch find mode (Range : 0 ~ 3) */
		public Byte					notch_max_der_val;			/* Currently set maximum notch/flat slope (Range : ??) */
		public Byte					notch_min_der_val;			/* Currently set minimum notch/flat slope (Range : ??) */
		public Byte					notch_min_der_width;		/* Currently set minimum samples with specified slope (Range : ??) */
		public Byte					notch_num_average;			/* Currently set number of samples outside notch/flat (Range : ??) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 1)]
		public Byte[]				u8_reserved;

		public Int16				last_align_succ;
		public Int16				last_align_fail;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public UInt16[]				u16_reserved;

		public UInt16				align_offset_twice;			/* unit : 0.0001 inches */
		public UInt16				align_offset_accept;		/* unit : 0.0001 inches */
		public UInt16				axis_switch_active;			/* Homing 이후 Axis이 Actvie인지 여부 */
		public UInt16				system_status;				/* 가장 최근의 컨트롤러의 상태 코드 값 */
		/* Motor Position Limit Error (1 ~ 65535) 즉, Motor Tunging Alogrithm에 적용되는 계수 값 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxAlignerDriveCount)]
		public UInt16[]				error_limit;				/* 0 : Rotation Axis (T), 1 : Horizon Axis (R), 2 : Veritical Axis (Z) (Motor Encoder Counts : Max 65535) */
		public UInt16				ccd_pixels;					/* Image Sensor로부터 수집된 Image의 Pixel 구성 개수 (0 ~ 4890) */
		/* 가장 최근의 Alignment 성공 or 실패 개수 */
		public UInt16				previous_cmd_fail;			/* 직전에 송신한 명령어의 실패에 대한 에러 코드 값 */
		public UInt16				min_good_sample_count;		/* Notch 검색하는데 GOOD 판정을 위해 필요한 CCD Sampling 개수 (장수) */
		public UInt16				ccd_chuck_sensing_value;	/* 일반 Wafer를 감지하기 위해, CCD Sensor의 Sensing 최대 크기 값 (for chuck) (unit: pixels) */
		public UInt16				ccd_chuck_sensing_small;	/* 작은 Wafer를 감지하기 위해, CCD Sensor의 Sensing 최대 크기 값 (for chuck) (unit: pixels) */
		public UInt16				ccd_chuck_sensing_great;	/* 대형 Wafer를 감지하기 위해, CCD Sensor의 Sensing 최대 크기 값 (for chuck) (unit: pixels) */
		public UInt16				ccd_pins_sensing_value;		/* 일반 Wafer를 감지하기 위해, CCD Sensor의 Sensing 최대 크기 값 (for pins)  (unit: pixels) */
		public UInt16				ccd_pins_sensing_small;		/* 작은 Wafer를 감지하기 위해, CCD Sensor의 Sensing 최대 크기 값 (for pins)  (unit: pixels) */
		public UInt16				ccd_pins_sensing_great;		/* 대형 Wafer를 감지하기 위해, CCD Sensor의 Sensing 최대 크기 값 (for pins)  (unit: pixels) */

		public UInt32				ccd_sensor_center;			/* CCD Sensor의 중심 위치 값 (Wafer의 Notch 혹은 Flat을 측정할 수 있는 위치) (unit: pixel) */
		public UInt32				last_param_timeout;			/* 가장 최근에 조회한 Parameter Timeout 값 임시 저장 (unit: msec) */
	//	UInt32				u32_reserved;
		/* Notch 관련 Align 수행 후 검색된 Depth와 Width 값 (unit: ?) 저장 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public UInt32[]				find_depth_width;		/* 0x00 : Depth, 0x01 : Width (unit: ???) */
		/* Align 검색 관련 Notch Depth 유효 값 (Min/Max) */
		public UInt32				notch_range_depth_min;
		public UInt32				notch_range_depth_max;
		/* Align 검색 관련 Notch Width 유효 값 (Min/Max) */
		public UInt32				notch_range_width_min;
		public UInt32				notch_range_width_max;
		/* CCD Data 크기 0:Min, 1:Max */
 		public UInt32				ccd_grab_size_min;			/* CCD로부터 Notch 영역을 가져올 크기. 가로/세로 크기 (unit: Pixel) */
 		public UInt32				ccd_grab_size_max;			/* CCD로부터 Notch 영역을 가져올 크기. 가로/세로 크기 (unit: Pixel) */

		/* Current Position */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxAlignerDriveCount)]
		public Double[]				axis_pos;					/* 0 : Rotation Axis (T): degree, 1 : Horizon Axis (R): um, 2 : Veritical Axis (Z): um */
		/* Home Position */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxAlignerDriveCount)]
		public Double[]				homed_pos;					/* 0 : Rotation Axis (T): degree, 1 : Horizon Axis (R): um, 2 : Veritical Axis (Z): um */
		/* Home Offset */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxAlignerDriveCount)]
		public Double[]				home_offset;				/* 0 : Rotation Axis (T): degree, 1 : Horizon Axis (R): um, 2 : Veritical Axis (Z): um */
		/* Direction (Forward or Reverse) Software Limit */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxAlignerDriveCount)]
		public Double[]				direct_f_limit;				/* 0 : Rotation Axis (T): degree, 1 : Horizon Axis (R): um, 2 : Veritical Axis (Z): um */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxAlignerDriveCount)]
		public Double[]				direct_r_limit;				/* 0 : Rotation Axis (T): degree, 1 : Horizon Axis (R): um, 2 : Veritical Axis (Z): um */
		/* Acceleration / Deceleration (sec^2) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxAlignerDriveCount)]
		public Double[]				axis_acl;					/* 0 : Rotation Axis 회전 각도 (Degree/sec^2), 1 : Vertical Motion 위치 (um/sec^2), 2 : Horizontal Motion 위치 (um/sec^2) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxAlignerDriveCount)]
		public Double[]				axis_dcl;					/* 0 : Rotation Axis 회전 각도 (Degree/sec^2), 1 : Vertical Motion 위치 (um/sec^2), 2 : Horizontal Motion 위치 (um/sec^2) */
		/* Acceleration / Deceleration (sec^3) for JERK */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxAlignerDriveCount)]
		public Double[]				axis_ajk;					/* 0 : Rotation Axis 회전 각도 (Degree/sec^3), 1 : Vertical Motion 위치 (um/sec^3), 2 : Horizontal Motion 위치 (um/sec^3) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxAlignerDriveCount)]
		public Double[]				axis_djk;					/* 0 : Rotation Axis 회전 각도 (Degree/sec^3), 1 : Vertical Motion 위치 (um/sec^3), 2 : Horizontal Motion 위치 (um/sec^3) */
		/* Speed (degree or um / sec^2) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxAlignerDriveCount)]
		public Double[]				axis_speed;					/* 0 : Rotation Axis 회전 각도 (Degree/sec^2), 1 : Vertical Motion 위치 (um/sec^2), 2 : Horizontal Motion 위치 (um/sec^2) */
		/* Align 검색 후 Notch 기준 회전할 각도 값 */
		public Double				rotate_after_align;			/* 실제 Notch 검색 성공 후 Wafer를 회전할 각도 값 */
		/* RWI 즉, Wafer Information ([0] Offset:Center (um), [1] Offset:Angle (degree), [2] Angle:R-Axis vs. Notch (degree) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxAlignerDriveCount)]
		public Double[]				rwi_wafer_info;
		/* Wafer Loading Up/Down 시점의 Position 값 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxAlignerDriveCount)]
		public Double[]				wafer_loading_down_pos;		/* Z : Vertical Axis 수직 위치 (um). 0:Up, 1:Down */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxAlignerDriveCount)]
		public Double[]				wafer_loading_up_pos;		/* Z : Vertical Axis 수직 위치 (um). 0:Up, 1:Down */
		/* measurement acceleration */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxAlignerDriveCount)]
		public Double[]				measure_acceleration;		/* 0 : Rotation Axis 회전 각도 (Degree/sec^2), 1 : Vertical Motion 위치 (um/sec^2), 2 : Horizontal Motion 위치 (um/sec^2) */
		/* measurement deceleration */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxAlignerDriveCount)]
		public Double[]				measure_deceleration;		/* 0 : Rotation Axis 회전 각도 (Degree/sec^2), 1 : Vertical Motion 위치 (um/sec^2), 2 : Horizontal Motion 위치 (um/sec^2) */
		/* measurement speed */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxAlignerDriveCount)]
		public Double[]				measure_speed;				/* 0 : Rotation Axis 회전 각도 (Degree/sec), 1 : Vertical Motion 위치 (um/sec), 2 : Horizontal Motion 위치 (um/sec) */
		/* Z Axis Position (Chuck과 Pins가 수평되는 위치) */
		public Double				level_chuck_pins_pos_z;		/* Chuck과 Pins가 수평인 위치의 Prealinger의 Z 축 위치 (unit: um) */
		/* Moving Up or Down 시점의 Offset 값 */
		public Double				offset_chuck_pin_pos_up;	/* Z 축을 Up or Down 시점의 발생할 수 있는 Backlash에 대한 허용 Offset 인정 값 (unit: um) */
		public Double				offset_chuck_pin_pos_down;	/* Z 축을 Up or Down 시점의 발생할 수 있는 Backlash에 대한 허용 Offset 인정 값 (unit: um) */
		/* CCD Sensor와 R (Horizontal) 축 간의 각도 값 */
		public Double				ccd_r_axis_angle;			/* 수평 축과 CCD Sensor 간의 설치(?)된 각도 값 입력 (Wafer Size는 고려 안됨) (unit: degrees) */
		/* Wafer가 Chuck에서 안전하게 이동하기 위한 Chuck의 위치 */
		public Double				wafer_trans_safe_pos_up;	/* Wafer가 Chuck에서 안전하게 이동 (0:가져다 놓을 때 (DOWN) 혹은 1:가지러 갈 때 (UP)하기 위한 Chuck의 위치. unit: um)*/
		public Double				wafer_trans_safe_pos_down;	/* Wafer가 Chuck에서 안전하게 이동 (0:가져다 놓을 때 (DOWN) 혹은 1:가지러 갈 때 (UP)하기 위한 Chuck의 위치. unit: um)*/
		public Double				safe_rotate_z_pos;			/* Wafer가 올려지고, 안전하게 회전하는데  필요한 Z 축의 높이 (unit: um) */

		/*
		 desc : 각 모션 축의 Homing 상태 정보 얻기
		 parm : axis	- [in]  축 명칭 (Veritical, Horizontal, Rotation)
				type	- [in]  스위치 위치 (종류) (Home, Forward, Reverse)
		 retn : TRUE or FALSE
		*/
		public Boolean IsMotionAxisChecked(ENG_PANC axis, ENG_PASP type)
		{
			UInt16 u16Mask	= axis_switch_active;

			/* 축 정보에 해당되는 4 bit 값 얻기 */
			switch (axis)
			{
			case ENG_PANC.pac_z	:	u16Mask	= Convert.ToUInt16((u16Mask & 0x0f00) >> 8);	break;
			case ENG_PANC.pac_r	:	u16Mask	= Convert.ToUInt16((u16Mask & 0x00f0) >> 4);	break;
			case ENG_PANC.pac_t	:	u16Mask	= Convert.ToUInt16((u16Mask & 0x000f));			break;
			default				:	return false;
			}
			/* Home (or Limit)에 해당되는 비트 위치 값 얻기 */
			switch (type)
			{
			case ENG_PASP.asp_h	:	return (u16Mask & 0x0001) > 0;
			case ENG_PASP.asp_f	:	return (u16Mask & 0x0002) > 0;
			case ENG_PASP.asp_r	:	return (u16Mask & 0x0004) > 0;
			}

			/* 무조건 실패*/
			return false;
		}

		/*
		 desc : System Status 값이 유효한 상태인지 여부
		 parm : None
		 retn : TRUE or FALSE
		*/
		public Boolean IsValidSystemStatus()
		{
			return (0xffff != system_status);
		}

		/*
		 desc : Prealigner에 문제가 생겼는지 여부
		 parm : None
		 retn : TRUE or FALSE
		*/
		public Boolean IsProblemPrealigner()
		{
			if (0x0001 == (system_status & 0x0001))	return true;	/* Unable to execute motion commands */
			if (0x0002 == (system_status & 0x0002))	return true;	/* Previous command returned an error. See PER command for details */
			if (0x0010 == (system_status & 0x0010))	return true;	/* Motion Error */
			if (0x0020 == (system_status & 0x0020))	return true;	/* Motion limits violation */
			if (0x0040 == (system_status & 0x0040))	return true;	/* Motion limits violation */
			if (0x0400 == (system_status & 0x0400))	return true;	/* I/O File error */
			if (0x2000 == (system_status & 0x2000))	return true;	/* Servo OFF on one or more axes */

			return false;
		}

		/*
		 desc : Prealigner에 동작 중인지 여부
		 parm : None
		 retn : TRUE or FALSE
		*/
		public Boolean IsBusyPrealigner()
		{
			return (0x0100 == (system_status & 0x0100) ||
					0x0080 == (system_status & 0x0080));
		}

		/*
		 desc : Wafer Scan 동작을 해도 되는지 여부
		 parm : None
		 retn : TRUE or FALSE
		*/
		public Boolean IsRunWaferScan()
		{
			if (IsProblemPrealigner())	return false;
			if (IsBusyPrealigner())		return false;

			/* Chuck Vacuum Sensor & Switch On 여부에 따라 */
			return Convert.ToBoolean(system_status & 0x000C /* 0x0004 | 0x0008 */);
		}
	};
}

/* --------------------------------------------------------------------------------------------- */
/*                                        Engine -> Robot                                        */
/* --------------------------------------------------------------------------------------------- */

namespace TGen2i.Struct.Robot
{
	/* Robot Data Value */
	[StructLayout(LayoutKind.Sequential, Pack = 8)]
	public struct STG_MRDV /* milara_robot_data_value */
	{
		public Byte					station_name;                                           /* 현재 선택된 Station Name 값 임시 저장 */
#if (false)
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 10)]
		public Byte[]				serial_number;											/* Serial number of the robot */
#else
		[MarshalAs(UnmanagedType.ByValTStr, SizeConst = 10)]
		public string				serial_number;											/* Serial number of the robot */
#endif
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
		public Byte[]				c_reserved;
#if (false)
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_LastStringDataSize)]
		public Byte[]				last_string_data;
#else
		[MarshalAs(UnmanagedType.ByValTStr, SizeConst = Constants.g_LastStringDataSize)]
		public string				last_string_data;
#endif
		public SByte				otf_debug_level;										/* Current debug level, or calibration parameters (Default 0, Max : 15) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 7)]
		public SByte[]				i8_reserved;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
		public Byte[]				wafer_map_loaded_state;									/* Cassette or Station에 Wafer가 적재된 상태 즉, 0 ~ 6 (ENG_RWLS) 값 중 한 개 값이 등록됨 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Byte[]				current_limit;											/* 각 축 (T, R1, R2, Z)의 현재 전류의 한계 값 (1 ~ 251) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public Byte[]				controlled_stop;										/* Enable/Disable Controlled Stop Feature. 0:State, 1:Option (0:활성화 출력 안함, 1:활성화 출력 함) */
		public Byte					info_mode;												/* 0 ~ 5. 0 : 응답 대기, 1 ~ 3 : 1 사이클 끝내고 응답. 1 : Hex Code 값만 반환, 2 : '>' 값만 반환, 3 : 문자열만 반환, 4 : 명령어 포함 반환, 5 : 에러 코드 정보 포함 반환 */
		public Byte					station_set_no;											/* current station set number (1 ~ 4) : Station Number들의 그룹 (총 4개까지 지원) */

		public Byte					station_no;												/* Current station number. (1 ~ 130) */
		public Byte					scanner_type;											/* Current scanner type. 0 for reflective type of scanner, 1 for thru-beam type of scanner */
		public Byte					current_slot_no;										/* Current slot of a station */
		public Byte					end_effector_num;										/* 가장 최근에 조회된 Station에 사용되는 End Effector Number (0 or 1) */
		public Byte					inline_station_type;									/* 0: radial in / radial out, 1: radial in / linear out, 2: linear in / linear out */
		public Byte					loaded_speed_enable;									/* Current state of loaded parameters 즉, 사용 여부 */
		public Byte					station_slot_num;										/* 가장 최근의 Station에 설정된 Slot의 개수 */
		public Byte					station_num;											/* 가장 최근의 등록된 Station의 개수 */

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public Byte[]				put_get_failed_option;									/* Current failure options for GET/PUT */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public Byte[]				put_get_option;											/* Current options for GET/PUT */
		public Byte					pusher_type;											/* Pusher Type (0: Not defined, 1: pushing is defined) */
		public Byte					station_service_busy;									/* Numeric value, 0 (Not busy) or 1 (Busy 즉, service 중 (상태)) */
		public Byte					station_locked_state;									/* Current station lock state (가장 최근에 설정한 Station State) */
		public Byte					station_axis_overlap_factor;							/* Currently set motion overlap factor (0 ~ 95%) */

		public Byte					station_trajectory_mode;								/* Currently set trajectory mode */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public Byte[]				station_vacuum_check;									/* [0]:Top Effector, [1]:Bottom Effector, Current wafer (vacuum) check options. 0: PUT, 1: GET */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public Byte[]				end_effector_wafer_check;								/* [0]:Top Effector, [1]:Bottom Effector, 0: Wafer was not detected; 1: Wafer was detected */
		public Byte					motion_wait_finished;									/* 0: 현재 모션이 동작 중 ..., 1: 모든 모션이 동작 완료된 상태  */
		public Byte					otf_set_second_scan_notch;								/* 0 (Notch 검색 무시) or 1 (Notch 검색 강제로 진행) */
		public Byte					otf_ignore_middle_samples;								/* 0 : 중간에 얻어진 샘플 무시하지 않음. 1 : 처음과 끝 제외한 모든 샘플 무시 */

		public Byte					scan_debug_mode;										/* 0 : disable, 1 : Enable (for Scan Debug Mode)*/
		public Byte					arm_used_scan_spec_station;								/* optional arm no, 0 (primary), or 1 (secondary) */
		public Byte					end_effector_up_down_station;							/* End Effector가 Station 정보를 이용하여 Wafer를 GET or PUT 동작할 때, Z 축을 UP (0x00) or DOWN (0x01) 처리 */
		public Byte					station_last_seg_speed_factor;							/* Inline Station Last Segment Speed Factor */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public Byte[]				u8_reserved;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Int16[]				enc_status;												/* Absolute Encoder status (-1 : 이용 불가 (엔코더 아님), 0:Healthy, 1:ErrABS, 2:BatteryLOW), T, R1, R2, Z, K (N/A) 축 순서대로 저장 */

		public UInt16				low_level_enc_status;									/* Low-level absolute encoder status (Last Axis) */
		public UInt16				digital_input_state;									/* Input state of given or all digital inputs (0 ~ 8 pin) */
		public UInt16				digital_output_state;									/* Output state of given or all digital inputs (0 ~ 8 pin) */
		public UInt16				home_switch_state;										/* All robot axes home switches state. (순서대로: K, Z, R, T) */

		public UInt16				extended_error_code;									/* Error Code Returned by EERR */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public UInt16[]				backup_dc_power;										/* Turn Backup DC Power On or Off */
		public UInt16				otf_set_number;											/* Currently set on-the-fly set number */

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public UInt16[]				axis_acceleration_jerk;									/* Currently set axis acceleration jerk */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public UInt16[]				axis_deceleration_jerk;									/* Currently set axis deceleration jerk */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public UInt16[]				axis_loaded_acceleration_jerk;							/* Currently set axis acceleration jerk when loaded with a wafer */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public UInt16[]				axis_loaded_deceleration_jerk;							/* Currently set axis deceleration jerk when loaded with a wafer */

		public UInt16				system_status;											/* System status word */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
		public UInt16[]				u16_reserved;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Int32[]				position_error_limit;									/* Currently set position error limit of one or all axes (unit: Encoder Counts) */
		public Int32				custom_generic_param;									/* Custom generic parameter value */
		public Int32				custom_axis_param;										/* Custom Axis parameter */
		public Int32				custom_station_param;									/* Custom Station parameter */

		public UInt32				external_di_in_state;									/* Input state of digital input */
		public UInt32				external_di_out_state;									/* Last state set to digital output */
		public UInt32				put_get_delay_time;										/* Currently set delay for PUT and GET (unit: mill-seconds) */
		public UInt32				vacuum_search_time_out;									/* Currently set time out (Station에서 Wafer 검색하는데, Vacuum Search Time Out: milli-seconds 값) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public UInt32[]				axis_diff_coeff_hold;									/* Currently Differential Coefficient of one or all axes */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public UInt32[]				axis_prop_coeff_hold;									/* Currently set proportional coefficient during axis hold */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public UInt32[]				axis_inte_coeff;										/* Currently Integral Coefficient of one or all axes */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public UInt32[]				axis_inte_coeff_hold;									/* Currently Integral Coefficient of one or all axes hold */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public UInt32[]				axis_diff_coeff;										/* Currently Differential Coefficient of one or all axes */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public UInt32[]				axis_prop_coeff;										/* Currently set proportional coefficient of one or all axes */

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				homed_position;											/* Currently set customized home position of all the axes (Homing 이후, 이동될 각 축의 Homed Position  */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				enc_zero_offset;										/* Currently set encoder zero offsets of all the axes (Homing 이후) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				sw_positive_limit;										/* Currently set forward direction software limit for all the axes */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				sw_negative_limit;										/* Currently set backward direction software limit for all the axes */
		public Double				maximum_wafer_thick;									/* Currently set maximum wafer thickness */
		public Double				r_axis_push_acc_def;									/* Current value of R axis pushing acceleration (Default) */
		public Double				r_axis_push_acc_sta;									/* Current value of R axis pushing acceleration for last station */
		public Double				r_axis_push_spd_def;									/* Current value of R axis pushing speed (Default) */
		public Double				r_axis_push_spd_sta;									/* Current value of R axis pushing speed for last station */
		public Double				wafer_scan_z_axis_acc;									/* Current wafer scanning Z axis acceleration for given station (for Last Station) */
		public Double				wafer_scan_z_axis_spd;									/* Current wafer scanning Z axis speed for given station (for Last Station) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				coord_compensation_slot;								/* Currently offset compensation with slot for T, R1, R2, Z */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				axis_acceleration;										/* Currently set axis acceleration */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				axis_deceleration;										/* Currently set axis deceleration */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				axis_loaded_acceleration;								/* Currently set axis acceleration when loaded with a wafer */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				axis_loaded_deceleration;								/* Currently set axis deceleration when loaded with a wafer */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				axis_loaded_speed;										/* Currently set loaded speed */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				axis_working_speed;										/* Currently set working speed */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				axis_safe_envelop_forward;								/* Currently Forward Safe Envelop */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				axis_safe_envelop_reverse;								/* Currently Reverse Safe Envelop */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				pos_err_limit_hold;										/* Currently position error limit of one or all axes hold */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				axis_inte_limit_hold;									/* Currently set integral limit of one or all axes hold */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				axis_integral_limit;									/* Currently set integral limit of on or all axes */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				home_flip_position;										/* Currently REE Homing Flip Position */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public Double[]				beam_threshold;											/* Currently set maximum beam thresholds for Global (Upper / Bottom) */
		public Double				station_center_offset;									/* Currently set center offset (for R Axis) */
		public Double				station_center_offset_edge_grip;						/* Currently set center offset (for R Axis) for Edge Gripper */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				axis_current_position;			/* Current position of all axes (sequence : T, R1, R2, Z. Reference to ENG_RANC) */
		public Double				station_retracted_r_position;							/* reports Inline Station Retracted R position for station Station */
		public Double				station_retracted_t_position;							/* reports Inline Station Retracted T position for station Station */
		public Double				station_exit_angle;										/* Report Inline Station Exit Angle (degree) */
		public Double				station_exit_distance;									/* Report Inline Station Exit Distance (um) */
		public Double				station_end_effect_length;								/* Report Inline Station End-Effector Length (um) */
		public Double				station_corner_radius;									/* Report Inline Station Corner Radius (degree) */
		public Double				station_offset_z;										/* Set/Report Station Offset for Z Axis (um) */
		public Double				station_pitch;											/* Currently set pitch (um) */
		public Double				start_pushing_distance;									/* Current value of Start Pushing Distance (um) */
		public Double				station_start_pushing_offset;							/* Current value of Start Pushing Offset (um) */
		public Double				station_pushing_offset_z;								/* Current value of Z push offset */
		public Double				station_pickup_acceleration_z;							/* Currently set pickup acceleration */
		public Double				station_pickup_speed_z;									/* Currently set pick-up speedn (unit: um) */
		public Double				station_retracted_position_r;							/* Currently set position (for R Axis. Last Position) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public Double[]				station_per_beam_threshold;								/* Currently set maximum beam thresholds for Station (Upper / Bottom) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				station_scanning_center_coord;							/* Current Scanning Center Coordinate, for all or given axis */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				station_scanning_left_coord;							/* Current Scanning Left Coordinate, for all or given axis */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				station_scanning_right_coord;							/* Current Scanning Right Coordinate, for all or given axis */
		public Double				station_wafer_max_thickness;							/* Currently set maximum wafer thickness for Station (unit: um) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxRobotDriveCount)]
		public Double[]				station_position;										/* Currently set station position */
		public Double				station_scaning_highest_z_coord;						/* Current Scanning Highest Z Position, for a given or default station */
		public Double				station_scaning_lowest_z_coord;							/* Current Scanning Lowest Z Position, for a given or default station */
		public Double				station_stroke_z;										/* Current Z Axis Stroke Position (um) */
	};
}

/* --------------------------------------------------------------------------------------------- */
/*                                         Engine -> EFU                                         */
/* --------------------------------------------------------------------------------------------- */

namespace TGen2i.Struct.EFU
{
	/* BL500 (EFU) 상태 정보 */
	[StructLayout(LayoutKind.Sequential, Pack = 8)]
	public struct STG_EPAS /* efu_pv_alarm_sv */
	{
		public Byte					bl_id;		/* BL500 ID (1 ~ 32) */
		public Byte					pv;			/* Process Value (ten's digit)*/
		public Byte					alarm;		/* Alarm Value */
		public Byte					sv;			/* Setting Value (ten's digit) */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public Byte[]				reserved;
	};
	/* BL500 (EFU) 전체 상태 정보 */
	[StructLayout(LayoutKind.Sequential, Pack = 8)]
	public struct STG_APAS /* all_pv_alarm_sv */
	{
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.g_MaxEFUCount)]
		public STG_EPAS[]			efu_pas;
	}
}

/* --------------------------------------------------------------------------------------------- */
/*                                      Engine -> Teaching                                       */
/* --------------------------------------------------------------------------------------------- */

namespace TGen2i.Struct.Teaching
{
/* Teaching Position 의 동작 정보 */
	[StructLayout(LayoutKind.Sequential, Pack = 8)]
	public struct STG_CTPI /* config_teaching_position_info */
	{
		public Byte				e_tower_lamp;
		public Byte				e_shutter;
		public Byte				e_align_expo_select;
		public Byte				e_error_skip;
		public Byte				e_cal_offset;
		public Byte				e_auto_focus;
		public Byte				e_stitch_calib;
		public Byte				mark_refind_count;				/* Mark Refind Count */
		public Byte				e_robot_info_mode;				/* Robot Info Mode 0:동기식(WMC로 모션 완료를 확인한다) / 1:비동기식(STA로 동작완료를 확인한다.)*/
		public Byte				expo_align;						/* Align or Direct */
		public Byte				expo_rectangle_lock;			/* Rectangle Lock */
		public Byte				expo_scroll_mode_1;				/* 노광 동작 속도 모드 값 */
		public Byte				expo_neg_offset_1;				/* HysteresisType1 Negative Offset */
		public Byte				expo_duty_cycle;				/* */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public Byte[]			u8_reserved;

		public Int32			expo_start_x_pos;				/* Expose Start X axis Position */
		public Int32			expo_start_y_pos;				/* Expose Start Y axis Position */
		public Int32			gerber_0_x_pos;					/* 거버 (0,0) 의 위치 X AXIS */
		public Int32			gerber_0_y_pos;					/* 거버 (0,0) 의 위치 Y AXIS */
		public Int32			load_x_pos;						/* Wafer Loading Position X (Wafer를 로봇으로 부터 받는 위치)*/
		public Int32			load_y_pos;						/* Wafer Loading Position Y */
		public Int32			unload_x_pos;					/* Wafer UnLoading Position X */
		public Int32			unload_y_pos;					/* Wafer UnLoading Position Y */
		public Int32			orgin_x_pos;					/* Stage Orgin Position X */
		public Int32			orgin_y_pos;					/* Stage Orgin Position Y */
		public Int32			bsa_move_left_pos_x;			/* bsa camera moving postion x */
		public Int32			bsa_move_left_pos_y;			/* bsa camera moving postion y */
		public Int32			bsa_move_right_pos_x;			/* bsa camera moving postion x */
		public Int32			bsa_move_right_pos_y;			/* bsa camera moving postion y */
		public Int32			pin_load_x;						/* pin stage가 이동 가능 stage 위치 x */
		public Int32			pin_load_y;						/* pin stage가 이동 가능 stage 위치 y */
		public Int32			pin_unload_x;					/* stage가 이동 가능 pin stage 위치 x */
		public Int32			pin_unload_y;					/* stage가 이동 가능 pin stage 위치 y */
		public Int32			focus_quaz_bsa_z_pos;			/* Quaz Point 1 에 대한 BSA CAM Z Position	*/
		public Int32			focus_mark1_bsa_z_pos;			/* Mark 1 에 대한 BSA CAM Z Position	*/
		public Int32			focus_mark2_bsa_z_pos;			/* Mark 2 에 대한 BSA CAM Z Position	*/
		public Int32			idle_bsa_z_pos;					/* BSA 카메라 Down시 Z Position	*/
		public Int32			focus_quaz_up_z_pos;			/* Quaz Point 1 에 대한 UP CAM Z Position	*/
		public Int32			mark_refind_range;				/* Mark Refind range [mm] */
		public Int32			focus_mark1_up_z_pos;			/* Mark 1 에 대한 UP CAM Z Position	*/
		public Int32			focus_mark2_up_z_pos;			/* Mark 2 에 대한 UP CAM Z Position	*/
		public Int32			pin_stage_z_up;					/* Lift Pin up(focus)시 Z Position	*/
		public Int32			pin_stage_z_down;				/* Lift Pin down시 Z Position	*/
		public Int32			pin_stage_x_input;				/* Lift Pin in시 X Position	*/
		public Int32			pin_stage_x_output;				/* Lift Pin out(focus)시 X Position	*/
		public Int32			expo_offset_x;					/* 노광 START 위치에서 UP카메라 까지의 거리 X */
		public Int32			expo_offset_y;					/* 노광 START 위치에서 UP카메라 까지의 거리 Y */
		public Int32			cam_offset_x;					/* 두개의 카메라(BSA & UP) 간의 차이값 x */
		public Int32			cam_offset_y;					/* 두개의 카메라(BSA & UP) 간의 차이값 y */
		public Int32			bsa_up_down_offset_x;			/* BSA UP/DOWN에 따른 오차값 보정 OFFSET X (mm) */
		public Int32			bsa_up_down_offset_y;			/* BSA UP/DOWN에 따른 오차값 보정 OFFSET Y (mm) */
		public Int32			quartz_center_offset_x;			/* STAGE 위치 정밀도 보정 OFFSET X (mm) */
		public Int32			quartz_center_offset_y;			/* STAGE 위치 정밀도 보정 OFFSET Y (mm) */
		public Int32			cent_quaz_bsa_x_pos;			/* Quaz Point 1 에 대한 BSA CAM Centering X axis Position */
		public Int32			cent_quaz_bsa_y_pos;			/* Quaz Point 1 에 대한 BSA CAM Centering Y axis Position */
		public Int32			cent_mark1_bsa_x_pos;			/* Mark 1 에 대한 BSA CAM CAMCentering X axis Position */
		public Int32			cent_mark1_bsa_y_pos;			/* Mark 1 에 대한 BSA CAMCAMCentering Y axis Position */
		public Int32			cent_mark2_bsa_x_pos;			/* Mark 2 에 대한 BSA CAM Centering X axis Position	*/
		public Int32			cent_mark2_bsa_y_pos;			/* Mark 2 에 대한 BSA CAM Centering Y axis Position	*/
		public Int32			cent_quaz_up_x_pos;				/* Quaz Point 1 에 대한 UP CAM Centering X axis Position */
		public Int32			cent_quaz_up_y_pos;				/* Quaz Point 1 에 대한 UP CAM Centering Y axis Position */
		public Int32			cent_mark1_up_x_pos;			/* Mark 1 에 대한 UP CAM Centering X axis Position */
		public Int32			cent_mark1_up_y_pos;			/* Mark 1 에 대한 UP CAM Centering Y axis Position */
		public Int32			cent_mark2_up_x_pos;			/* Mark 2 에 대한 UP CAM Centering X axis Position */
		public Int32			cent_mark2_up_y_pos;			/* Mark 2 에 대한 UP CAM Centering Y axis Position */
		public Int32			offset_mark1_x;
		public Int32			offset_mark1_y;
		public Int32			offset_mark2_x;
		public Int32			offset_mark2_y;
		public Int32			focus_ph_head_height;
		public Int32			focus_ph_meas_height;
		public Int32			inlock_stage_pos_min_x;			/* STAGE X 축 TABLE의 MIN POSITION	*/
		public Int32			inlock_stage_pos_min_y;			/* STAGE Y 축 TABLE의 MIN POSITION	*/
		public Int32			inlock_stage_pos_max_x;			/* STAGE X 축 TABLE의 MAX POSITION	*/
		public Int32			inlock_stage_pos_max_y;			/* STAGE Y 축 TABLE의 MAX POSITION	*/
		public Int32			inlock_pin_pos_min_x;			/* PIN X 축 TABLE의 MIN POSITION	*/
		public Int32			inlock_pin_pos_max_x;			/* PIN X 축 TABLE의 MAX POSITION	*/
		public Int32			inlock_pin_pos_min_z;			/* PIN Z 축 TABLE의 MIN POSITION	*/
		public Int32			inlock_pin_pos_max_z;			/* PIN Z 축 TABLE의 MAX POSITION	*/
		public Int32			inlock_up_pos_min_z;			/* Optic Z(UP) 축 TABLE의 MIN POSITION	*/
		public Int32			inlock_up_pos_max_z;			/* Optic Z(UP) 축 TABLE의 MAX POSITION	*/
		public Int32			expo_set_rotation;				/* Expose Rotation */
		public Int32			expo_set_scale_x;				/* Expose Scale X [mm] */
		public Int32			expo_set_scale_y;				/* Expose Scale Y [mm] */
		public Int32			expo_set_offset_x;				/* Expose Offset X [mm] */
		public Int32			expo_set_offset_y;				/* Expose Offset Y [mm] */
		public Int32			inlock_bsa_pos_min_z;			/* BSA Z 축 TABLE의 MIN POSITION	*/
		public Int32			inlock_bsa_pos_max_z;			/* BSA Z 축 TABLE의 MAX POSITION	*/
		public Int32			expo_photo1_height;				/* Photohead1 Z Axis [mm] */
		public Int32			expo_photo2_height;				/* Photohead2 Z Axis [mm] */
		public Int32			expo_pos_delay_1;				/* HysterssisType1 Move Positive Delay */
		public Int32			expo_neg_delay_1;				/* HysterssisType1 Move Negative Delay */
		public Int32			expo_ph2_offset_x;				/* ph2 단차 X */
		public Int32			expo_ph2_offset_y;				/* ph2 단차 Y */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
		public Int32[]			illum_ph_x_axis_pos;			/* 헤드 1번 조도 센서 측정을 위한 Stage X 축 위치 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
		public Int32[]			illum_ph_y_axis_pos;			/* 헤드 1번 조도 센서 측정을 위한 Stage Y 축 위치 */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public Int32[]			inlock_stage_pos_axis_x;		/* Bsa 카메라 이동에 관련한 Interlock X Position (Interlock P1 ~ P4)	*/
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public Int32[]			inlock_stage_pos_axis_y;		/* Bsa 카메라 이동에 관련한 Interlock Y Position (Interlock P1 ~ P4)	*/
		public Int32			i32_reserved;

		public UInt32			material_thick;					/* Thickness (um) */
		public UInt32			stage_y_acc_dist;				/* Motion controller Y-direction acceleration/deceleration distance. (unit: 100 nm or 0.1 um) */
		public UInt32			velo_align_stage;				/* Align 시 Stage 이동 속도	[mm/s] */
		public UInt32			velo_align_bsa;					/* Align 시 bsa 이동 속도 [mm/s] */
		public UInt32			velo_align_pin_x;				/* Align 시 pin x 이동 속도 [mm/s] */
		public UInt32			velo_align_pin_z;				/* Align 시 pin z 이동 속도 [mm/s] */
		public UInt32			led0_index_ph1;					/* 노광에 사용하는 조도 값에 해당되는 Ph1 인덱스 값 (LED 365) */
		public UInt32			led1_index_ph1;					/* 노광에 사용하는 조도 값에 해당되는 Ph1 인덱스 값 (LED 380) */
		public UInt32			led2_index_ph1;					/* 노광에 사용하는 조도 값에 해당되는 Ph1 인덱스 값 (LED 390) */
		public UInt32			led3_index_ph1;					/* 노광에 사용하는 조도 값에 해당되는 Ph1 인덱스 값 (LED 405) */
		public UInt32			led0_index_ph2;					/* 노광에 사용하는 조도 값에 해당되는 Ph2 인덱스 값 (LED 365) */
		public UInt32			led1_index_ph2;					/* 노광에 사용하는 조도 값에 해당되는 Ph2 인덱스 값 (LED 380) */
		public UInt32			led2_index_ph2;					/* 노광에 사용하는 조도 값에 해당되는 Ph2 인덱스 값 (LED 390) */
		public UInt32			led3_index_ph2;					/* 노광에 사용하는 조도 값에 해당되는 Ph2 인덱스 값 (LED 405) */
		public UInt32			up_cam_light_value;				/* Photo Head 조명 값	*/
		public UInt32			ling_light_value;				/* BSA Ling 조명 값	*/
		public UInt32			coaxial_light_value;			/* BSA Coaxial 조명 값	*/
		public UInt32			u32_reserved;

		public Double			led0_power_ph;					/* 노광에 사용하는 조도 값 (LED 365) */
 		public Double			led1_power_ph;					/* 노광에 사용하는 조도 값 (LED 380) */
 		public Double			led2_power_ph;					/* 노광에 사용하는 조도 값 (LED 390) */
 		public Double			led3_power_ph;					/* 노광에 사용하는 조도 값 (LED 405) */
		public Double			focus_ld_sensor_p1;
		public Double			focus_ld_sensor_p2;
		public Double			focus_ld_sensor_p3;
		public Double			inlock_stage_max_speed;			/* STAGE MAX SPEED (mm/s)	*/
		public Double			inlock_stage_min_speed;			/* STAGE MIN SPEED (mm/s)	*/
		public Double			inlock_bsa_max_speed;			/* BSA Z 축 MAX SPEED (mm/s)	*/
		public Double			inlock_bsa_min_speed;			/* BSA Z 축 MIN SPEED (mm/s)	*/
		public Double			inlock_pin_max_speed;			/* PIN Z 축	MAX SPEED (mm/s)	*/
		public Double			inlock_pin_min_speed;			/* PIN Z 축	MIN SPEED (mm/s)	*/

		/*
		 desc : 데이터 유효성 검증
		 parm : None
		 retn : TRUE or FALSE
		*/
		public Boolean IsValid()
		{
			return expo_start_x_pos > 0 && expo_start_y_pos > 0 &&
				gerber_0_x_pos > 0 && gerber_0_y_pos > 0 && load_x_pos > 0 && load_y_pos > 0 &&
				unload_x_pos > 0 && unload_y_pos > 0 && orgin_x_pos > 0 && orgin_y_pos > 0 &&
				bsa_move_left_pos_x > 0 && bsa_move_left_pos_y > 0 && bsa_move_right_pos_x > 0 &&
				bsa_move_right_pos_y > 0 &&			   
				/* 아래는 상당히 중요. 노광 시작 위치 값보다. Y Acceleration Distance 값이 크면 안됨 */
				expo_start_y_pos >= (Int32)stage_y_acc_dist && (Int32)velo_align_stage > 0;			
		}

		/*
		 desc : 정보 유효성 검사 (조도 측정을 이 값으로 동작해도 되는지 여부)
		 parm : None
		 retn : TRUE or FALSE
		*/
		public Boolean IsValidIllum()
		{
			return (illum_ph_x_axis_pos[1] > 0.0f && illum_ph_y_axis_pos[1] > 0.0f &&
					illum_ph_x_axis_pos[2] > 0.0f && illum_ph_y_axis_pos[2] > 0.0f);
		}
	}
}