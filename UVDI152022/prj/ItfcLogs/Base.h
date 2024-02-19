#pragma once

#define DATE_TIME_SIZE	32
#define FUNC_NAME_SIZE	96

class CBase
{
	TCHAR m_tzDir[MAX_PATH_COUNT][12] = { L"Service", L"Control", L"Motion", L"Library", L"Vision", L"PhilHmi", L"PGFS", L"Calibration", L"Etc" };


/* ������ & �ı��� */
public:

	CBase(BOOL saved, UINT8 type, ENG_LNWE mode);
	virtual ~CBase();

/* ����ü */
protected:

#pragma pack (push, 8)

	/* Log Database ���� ����� ���ڵ� ���� */
	typedef struct __st_log_database_record_item__
	{
		UINT64				apps:16;					/* �α� �߻�ó (ENG_EDIC) */
		UINT64				level:16;					/* �˶� Level (ENG_LNWE) */
		UINT64				line:32;					/* �α� �߻� ��ġ (�ڵ� ������ ���� ��ȣ) */
		TCHAR				date_time[DATE_TIME_SIZE];	/* ex> 2022-01-20 14:30:23.793 */
		TCHAR				func_name[FUNC_NAME_SIZE];	/* �α� �߻� �Լ� �� */
		TCHAR				file_name[MAX_FILE_LEN];	/* �α� �߻� ���� �� */
		TCHAR				message[LOG_MESG_SIZE];

	}	STM_LDRI,	*LPM_LDRI;

#pragma pack (pop)	/* 8 bytes alignment */

/* ���� ���� */
protected:

	UINT8				m_u8LogType;	/* 0x00 : Text File, 0x01 : Database File (SQLite3) */

	BOOL				m_bLogSaved;

	ENG_LNWE			m_enSavedLevel;
	TCHAR				m_tzDrvName[4];
	TCHAR				m_tzLogFile[MAX_PATH_COUNT][MAX_PATH_LEN];	/* �α� ���� ����ȭ */
	TCHAR				m_tzLogDir[MAX_PATH_COUNT][MAX_PATH_LEN];	/* �α� ��� ����ȭ */

	UINT64				m_u64HddCheckTime;	/* HDD �뷮 Ȯ���� �ð� ���� */
	UINT64				m_u64LogCheckTime;	/* �α� ���� ũ�� �ð� ���� */

	//sqlite3				*m_pSqlDB;			/* �ӽ� Database */

	CAtlList <STM_LDRI>	m_lstSqlDB;	/* �ӽ� ������ Database */


/* ���� �Լ� */
protected:

	BOOL				IsHDDFull(UINT64 limit);
	BOOL				IsLogFull(UINT8 type, UINT64 limit);

	VOID				SetFile(UINT8 type);
	BOOL				SaveTxt(UINT8 type, PTCHAR mesg);
	BOOL				SaveDB(UINT8 type, UINT8 apps, UINT8 level, PTCHAR mesg,
							   PTCHAR file, PTCHAR func, UINT32 line);


/* ���� �Լ� */
public:

	BOOL				Init(PTCHAR path);
	VOID				Close();

	PTCHAR				GetLogPath(UINT8 type)		{	return m_tzLogDir[type];	}
	INT32				GetLogData(PTCHAR file, PTCHAR start=NULL, PTCHAR end=NULL);
	VOID				SetLogData(UINT8 apps, UINT8 level, UINT32 line,
								   PTCHAR func, PTCHAR name, PTCHAR time, PTCHAR mesg);
	ENG_LNWE			GetSaveLogLevel()	{	return m_enSavedLevel;	}
};
