#pragma once

#include <vector>
#include <string>

enum EN_IO_TYPE
{
	eIO_INPUT,
	eIO_OUTPUT,
	eIO_MAX
};

typedef struct ST_IO_CMD
{
	ST_IO_CMD()
	{
		nIndex = 0;
		bReverse = FALSE;
	}

	int				nIndex;
	std::wstring	strAddr;
	BOOL			bReverse;
	BOOL			bIOType;   // 0 = INPUT 1= OUTPUT
}ST_IO_CMD;

// X1003	LINEAR Y1 AXIS POWER LINE (CP102)	G	ON	R	OFF
typedef struct ST_IO
{
	ST_IO()
	{
		nIndex	= 0;
		bOn		= FALSE;
	}

	int			nIndex;
	BOOL		bOn;

	std::wstring strAddr;
	std::wstring strName;
	std::wstring strOn;
	std::wstring strOff;
	std::wstring strClrOn;
	std::wstring strClrOff;
	BOOL		bIsQuick;
	std::vector < ST_IO_CMD > vMappingOutput;
	std::vector < ST_IO_CMD > vMatchingInput;
	std::vector < ST_IO_CMD > vMappingInterLockIO;
}ST_IO;

typedef std::vector < ST_IO > vST_IO;

typedef struct ST_QUICK_IO_ACT
{
	ST_QUICK_IO_ACT()
	{
		nIndex = -1;
		bOn = FALSE;
	}
	int			nIndex;
	BOOL		bOn;
	std::wstring strName;

	std::vector < ST_IO_CMD > vActIO;
	std::vector < ST_IO_CMD > vActIO2;
	std::vector < ST_IO_CMD > vActOnOffIO;
}ST_QUICK_IO_ACT;

typedef std::vector < ST_QUICK_IO_ACT > vST_QUICK_IO_ACT;

class CIOManager
{
protected:
	CWinThread* m_pWaitThread;		/* 대기 쓰레드 */
private:
	std::wstring			m_strIOPath;
	HWND					m_hMainWnd;
	vST_IO					m_vIO[eIO_MAX];
	vST_QUICK_IO_ACT		m_vQuickIO;
public:
	//Save & Load 관련 함수
	void				Init(HWND hWnd);
	void				Destroy();
	std::wstring		GetIOPath();
	BOOL				LoadIOList();
	BOOL				LoadQUICKIOList();
	BOOL				UpdateIO();
	void				UpdateQUICKIO();
	std::wstring		GetInputName(int nIndex);
	int					GetInputIndex(std::wstring strName);
	std::wstring		GetOutputName(int nIndex);
	int					GetOutputIndex(std::wstring strName);
	BOOL				GetIO(EN_IO_TYPE eType, int nIndex, ST_IO& stIO);
	BOOL				GetIO(EN_IO_TYPE eType, CString strName, ST_IO& stIO);
	BOOL				GetIOStatus(EN_IO_TYPE eType, CString strName);
	BOOL				SetIO(EN_IO_TYPE eType, ST_IO& stIO);
	BOOL				SetQUICKIO(int nIndex);
	

	//////////////////////////////////////////////////////////////////////////
	//기타함수
	int					GetIOList(EN_IO_TYPE eType,	vST_IO& vIOList);
	int					GetQuickIOList(vST_IO& vIOList);
	int					GetQuickIOList(vST_QUICK_IO_ACT& vIOList);

	int					GetInputList(vST_IO& vIOList);
	int					GetInputCount() { return (int)m_vIO[eIO_INPUT].size(); }

	int					GetOutputList(vST_IO& vIOList);
	int					GetOutputCount() { return (int)m_vIO[eIO_OUTPUT].size(); }

	int					m_QuickIndex;
	void				SetQuickIOThread(int nIndex);

protected:
	CIOManager();
	CIOManager(const CIOManager&); // Prohibit Copy Constructor
	CIOManager& operator =(const CIOManager&);

public:
	virtual ~CIOManager(void);

	static CIOManager* GetInstance()
	{
		static CIOManager _inst;
		return &_inst;
	}
};
UINT WaitThread(LPVOID lpParam);

