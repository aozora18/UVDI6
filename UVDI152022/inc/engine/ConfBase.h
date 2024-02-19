
#pragma once

#define	MAX_SUBJ_STRING	64
#define	MAX_KEY_STRING	64

class CConfBase
{
public:

	CConfBase(PTCHAR cfg_name);
	virtual ~CConfBase();


/* 가상 함수 */
protected:
public:


/* 로컬 변수 */
protected:

	TCHAR				m_tzFile[MAX_PATH_LEN];
	TCHAR				m_tzSubj[MAX_SUBJ_STRING];


/* 로컬 함수 */
protected:

	INT32				GetConfigInt(PTCHAR key, INT32 def=0);
	UINT32				GetConfigIPv4(PTCHAR key);
	VOID				GetConfigIPv4(PTCHAR key, PUINT8 ipv4);
	DOUBLE				GetConfigDouble(PTCHAR key);
	UINT8				GetConfigUint8(PTCHAR key);
	INT8				GetConfigInt8(PTCHAR key);
	UINT16				GetConfigUint16(PTCHAR key);
	UINT32				GetConfigUint32(PTCHAR key);
	INT32				GetConfigInt32(PTCHAR key);
	UINT64				GetConfigUint64(PTCHAR key);

	UINT8				GetConfigHex8(PTCHAR key);
	UINT16				GetConfigHex16(PTCHAR key);
	UINT32				GetConfigHex32(PTCHAR key);


	VOID				GetConfigStr(PTCHAR key, PTCHAR data, UINT32 size, PTCHAR def=L"");
	VOID				SetConfigStr(PTCHAR key, PTCHAR str);
	VOID				SetConfigInt32(PTCHAR key, INT32 val);
	VOID				SetConfigInt64(PTCHAR key, INT64 val);
	VOID				SetConfigDouble(PTCHAR key, DOUBLE val, UINT8 pts=0);
	VOID				SetConfigUint32(PTCHAR key, UINT32 val);
	VOID				SetConfigUint64(PTCHAR key, UINT64 val);
	VOID				SetConfigIPv4(PTCHAR key, UINT32 ipv4);
	VOID				SetConfigIPv4(PTCHAR key, PUINT8 ipv4);


/* 공용 함수 */
public:

};
