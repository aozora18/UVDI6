
/*
 desc : PM100x Information
*/

#pragma once

/* --------------------------------------------------------------------------------------------- */
/*                                            정의값                                             */
/* --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- */
/*                                            상수값                                             */
/* --------------------------------------------------------------------------------------------- */



/* --------------------------------------------------------------------------------------------- */
/*                                            열거형                                             */
/* --------------------------------------------------------------------------------------------- */

/* The sensor type of measurement device */
typedef enum class __en_measurement_device_sensor_type__ : UINT8
{
	en_none					= 0x00,
	en_led_power			= 0x01,		/* Unit : Watt */
	en_led_enery			= 0x02,		/* Unit : J */
	en_led_frequency		= 0x03,		/* Unit : nm */
	en_led_enery_density	= 0x04,		/* Unit : mJ/cm2 */

}	ENG_MDST;


/* --------------------------------------------------------------------------------------------- */
/*                                            구조체                                             */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 1)


#pragma pack (pop)	// 8 bytes


#pragma pack (push, 8)

/* Get device id */
typedef struct __st_pm100d_device_detail_id__
{
	TCHAR				d_name[64];			/* Device Name */
	TCHAR				m_name[64];			/* Manufacturer Name */
	TCHAR				s_num[64];			/* Serial Number */
	TCHAR				s_firm[64];			/* Firmware Ver. - Software */
	TCHAR				i_firm[64];			/* Firmware Ver. - Instrument driver */
	TCHAR				c_date[64];			/* Calibration Date */

}	STG_PDDI,	*LPG_PDDI;

/* Get beam diameter */
typedef struct __st_pm100d_beam_diameter_value__
{
	DOUBLE				set_val;			/* beam diameter set (Unit: mm)*/
	DOUBLE				min_val;			/* beam diameter min (Unit: mm)*/
	DOUBLE				max_val;			/* beam diameter max (Unit: mm)*/
	DOUBLE				def_val;			/* beam diameter default (Unit: mm) */

}	STG_PBDV,	*LPG_PBDV;

/* Get Sensor Information */
typedef struct __st_pm100d_sensor_detail_information__
{
	TCHAR				s_name[64];			/* Sensor Name */
	TCHAR				s_num[64];			/* Serial Number */

	INT16				s_type;				/* Sensor Type */
	INT16				s_subt;				/* Sensor Sub Type */
	INT16				flags;				/* Flags */
	INT16				i16_reserved;

}	STG_PSDI,	*LPG_PSDI;

/* PM100D 장비 설정 정보 */
typedef struct __st_pm100d_set_data_value__
{
	/* Setting Value */
	STG_PDDI			dev_id;				/* device id */
	STG_PBDV			beam;				/* beam diameter */
	STG_PSDI			sensor;				/* sensor information */

	INT16				line_freq;			/* Line Frequency (Line Filter) (단위: 50Hz ro 60Hz) */
	INT16				i16_reserved[3];

}	STG_PSDV,	*LPG_PSDV;

/* PM100D 실시간 발생 값 */
typedef struct __st_pm100d_get_data_value__
{
	/* unit: W or dBm, J (Energy), Hz or J/cm^2) */
	DOUBLE				last_val;			/* The most recently measured value */
	DOUBLE				max_min;			/* Range (Max-Min) */
	DOUBLE				average;			/* Average (Mean) */
	DOUBLE				std_dev;			/* Standard devication */
	DOUBLE				cov_rate;			/* Coefficient of variation (Percent) = standard deviation / Average * 100.0 */

}	STG_PGDV,	*LPG_PGDV;


/* PM100D 실시간 전체 설정 및 발생 값 */
typedef struct __st_pm100d_all_data_value__
{
	STG_PSDV			set;
	STG_PGDV			get;

}	STG_PADV,	*LPG_PADV;

#pragma pack (pop)	/* 8 bytes order */