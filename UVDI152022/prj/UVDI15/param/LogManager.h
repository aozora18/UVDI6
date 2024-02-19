#pragma once

#include <vector>
#include <string>


#define _UTF_8		FALSE
#define _UTF_16		TRUE


enum EN_LOG_TYPE
{
	eLOG_TYPE_SERVICE = 0,
	eLOG_TYPE_CONTROL,
	eLOG_TYPE_MOTION,
	eLOG_TYPE_LIBRARY,
	eLOG_TYPE_VISION,
	eLOG_TYPE_PHILHMI,
	eLOG_TYPE_PGFS,
	eLOG_TYPE_CALIBRATION,
	eLOG_TYPE_ETC,

	eLOG_TYPE_MAX
};


struct ST_LOG_MSG
{
	UINT				nDate;			//	날짜
	UINT				nTime;			//	시간
	//	UINT				nTimeID;		//	동일한 시간을 구분하기 위한 ID
	CString				strLog;			//	로그
};

typedef std::vector <ST_LOG_MSG> vST_LOG_MSG;



class CLogManager
{
	struct LogMsgHash {
		std::size_t operator()(const ST_LOG_MSG& logMsg) const {
			return std::hash<UINT>{}(logMsg.nDate) ^ std::hash<UINT>{}(logMsg.nTime);//^ std::hash<UINT>{}(logMsg.nTimeID);
		}
	};

	struct LogMsgEqual {
		bool operator()(const ST_LOG_MSG& lhs, const ST_LOG_MSG& rhs) const {
			return lhs.nDate == rhs.nDate && lhs.nTime == rhs.nTime;//&& lhs.nTimeID == rhs.nTimeID;
		}
	};

private:
	TCHAR				m_tzType[MAX_PATH_COUNT][12] = { L"Service", L"Control", L"Motion", L"Library", L"Vision", L"PhilHmi", L"PGFS", L"Calibration", L"Etc" };

	HWND				m_hMainWnd;

	UINT				m_nID;

	vST_LOG_MSG			m_vOriginLogs[eLOG_TYPE_MAX];
	vST_LOG_MSG			m_vSyncLogs[eLOG_TYPE_MAX];

public:



public:
	
	void				InitManager(HWND hWnd);
	BOOL				InitCreatePath();

	void				Destroy();


	void				InitLog(EN_LOG_TYPE type);
	void				SetLog(EN_LOG_TYPE type, UINT nDate, UINT nTime, CString strLog);
	vST_LOG_MSG			GetLog(EN_LOG_TYPE type, BOOL bSyncView);

	void				LoadFileContent(EN_LOG_TYPE type, CString strFileName);
	void				LoadFiles(EN_LOG_TYPE type, COleDateTime timeStart, COleDateTime timeEnd);




protected:

	void				GetArrDates(COleDateTime timeStart, COleDateTime timeEnd, CStringArray& arrDates);

	UINT 				ExtractTime(const CString& str);		//	Parse
	UINT 				ExtractDate(const CString& str);		//	Parse
	CString				FilterString(const CString& input);



protected:
	CLogManager();
	CLogManager(const CLogManager&); // Prohibit Copy Constructor
	CLogManager& operator =(const CLogManager&);

public:
	virtual ~CLogManager(void);

	static CLogManager* GetInstance()
	{
		static CLogManager _inst;
		return &_inst;
	}
};


