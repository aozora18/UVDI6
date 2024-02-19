#pragma once

#define DATE_TIME_SIZE	32
#define FUNC_NAME_SIZE	96

class CBase
{
	TCHAR m_tzDir[MAX_PATH_COUNT][12] = { L"Service", L"Control", L"Motion", L"Library", L"Vision", L"PhilHmi", L"PGFS", L"Calibration", L"Etc" };


/* 생성자 & 파괴자 */
public:

	CBase(BOOL saved, UINT8 type, ENG_LNWE mode);
	virtual ~CBase();

/* 구조체 */
protected:

#pragma pack (push, 8)

	/* Log Database 내에 저장된 레코드 정보 */
	typedef struct __st_log_database_record_item__
	{
		UINT64				apps:16;					/* 로그 발생처 (ENG_EDIC) */
		UINT64				level:16;					/* 알람 Level (ENG_LNWE) */
		UINT64				line:32;					/* 로그 발생 위치 (코드 파일의 라인 번호) */
		TCHAR				date_time[DATE_TIME_SIZE];	/* ex> 2022-01-20 14:30:23.793 */
		TCHAR				func_name[FUNC_NAME_SIZE];	/* 로그 발생 함수 명 */
		TCHAR				file_name[MAX_FILE_LEN];	/* 로그 발생 파일 명 */
		TCHAR				message[LOG_MESG_SIZE];

	}	STM_LDRI,	*LPM_LDRI;

#pragma pack (pop)	/* 8 bytes alignment */

/* 로컬 변수 */
protected:

	UINT8				m_u8LogType;	/* 0x00 : Text File, 0x01 : Database File (SQLite3) */

	BOOL				m_bLogSaved;

	ENG_LNWE			m_enSavedLevel;
	TCHAR				m_tzDrvName[4];
	TCHAR				m_tzLogFile[MAX_PATH_COUNT][MAX_PATH_LEN];	/* 로그 파일 세분화 */
	TCHAR				m_tzLogDir[MAX_PATH_COUNT][MAX_PATH_LEN];	/* 로그 경로 세분화 */

	UINT64				m_u64HddCheckTime;	/* HDD 용량 확인한 시간 저장 */
	UINT64				m_u64LogCheckTime;	/* 로그 파일 크기 시간 저장 */

	//sqlite3				*m_pSqlDB;			/* 임시 Database */

	CAtlList <STM_LDRI>	m_lstSqlDB;	/* 임시 추출한 Database */


/* 로컬 함수 */
protected:

	BOOL				IsHDDFull(UINT64 limit);
	BOOL				IsLogFull(UINT8 type, UINT64 limit);

	VOID				SetFile(UINT8 type);
	BOOL				SaveTxt(UINT8 type, PTCHAR mesg);
	BOOL				SaveDB(UINT8 type, UINT8 apps, UINT8 level, PTCHAR mesg,
							   PTCHAR file, PTCHAR func, UINT32 line);


/* 공용 함수 */
public:

	BOOL				Init(PTCHAR path);
	VOID				Close();

	PTCHAR				GetLogPath(UINT8 type)		{	return m_tzLogDir[type];	}
	INT32				GetLogData(PTCHAR file, PTCHAR start=NULL, PTCHAR end=NULL);
	VOID				SetLogData(UINT8 apps, UINT8 level, UINT32 line,
								   PTCHAR func, PTCHAR name, PTCHAR time, PTCHAR mesg);
	ENG_LNWE			GetSaveLogLevel()	{	return m_enSavedLevel;	}
};
