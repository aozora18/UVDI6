using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Threading.Tasks;


namespace TGen2i.Utility
{
	public class Converter
	{
		public static byte[] StructToBytes<T>(T data) where T : struct
		{
			var formatter	= new BinaryFormatter();
			var stream		= new MemoryStream();
			formatter.Serialize(stream, data);

			return stream.ToArray();
		}

		public static T BytesToStruct<T>(byte[] array)  where T : struct
		{
			var szSt	= Marshal.SizeOf(typeof(T));
			var ptrMem	= Marshal.AllocHGlobal(szSt);
			Marshal.Copy(array, 0, ptrMem, szSt);
			var stData	= (T)Marshal.PtrToStructure(ptrMem, typeof(T));
			Marshal.FreeHGlobal(ptrMem);

			return stData;
		}
	};

	public struct Bit
	{
		/*
		 desc : Returns 1 bit value at a specific position among word (8 bits) values
		 parm : val	- [in]  Input Value (Byte Size)
				pos	- [in]  Bit Position (Bit position value to get)
		 retn : 1 bit. (ON or OFF)
		*/
		public static Byte GetU8ToBit(Byte val, Byte pos)
		{
			return (Byte)((val & (0x01 << pos)) >> pos);
		}

		/*
		 desc : Returns 1 bit value at a specific position among word (8 bits) values
		 parm : val	- [in]  Input Value (Byte Size)
				pos	- [in]  Bit Position (Bit position value to get)
				size- [in]  The number of bits to be fetched
		 retn : 1 bit. (ON or OFF)
		*/
		public static Byte GetU8ToBits(Byte val, Byte pos, Byte size)
		{
			if (size < 1)	return 0x00;
			return (Byte)((val & ((((Byte)1) << size) -1) << pos) >> pos);
		}

		/*
		 desc : Returns 1 bit value at a specific position among word (16 bits) values
		 parm : val	- [in]  Input Value (Word Size)
				pos	- [in]  Bit Position (Bit position value to get)
		 retn : 1 bit. (ON or OFF)
		*/
		public static Byte GetU16ToBit(UInt16 val, Byte pos)
		{
			return (Byte)((val & (0x0001 << pos)) >> pos);
		}

		/*
		 desc : Returns 1 bit value at a specific position among word (8 bits) values
		 parm : val	- [in]  Input Value (Word Size)
				pos	- [in]  Bit Position (Bit position value to get)
				size- [in]  The number of bits to be fetched
		 retn : 1 bit. (ON or OFF)
		*/
		public static Byte GetU16ToBits(UInt16 val, Byte pos, Byte size)
		{
			if (size < 1)	return 0x00;
			return (Byte)((val & ((((Byte)1) << size) -1) << pos) >> pos);
		}

		/*
		 desc : Returns an arbitrary number of bits from a specific position of the word (16 bits) value.
		 parm : val	- [in]  Input Value (Word Size)
				pos	- [in]  Bit Position (Bit position value to get)
				size- [in]  The number of bits to be fetched
		 retn : 1 bit. (ON or OFF)
		*/
		public static UInt16 GetU16ToU16(UInt16 val, Byte pos, Byte size)
		{
			if (size < 1)	return 0x0000;
			return (UInt16)((val & ((((UInt16)1) << size) -1) << pos) >> pos);
		}
	};
}
