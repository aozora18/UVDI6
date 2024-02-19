#pragma once
//////////////////////////////////////////////////////////////////////////
//생성자에 Load가 포함되어있으며, 소멸자에 Save가 들어있다.
//
//*조심할점*	Save관련 펑션을 만들때, 아래와 같은 구조를 만들면 문제가 될수있다
//				SaveA에서 SaveB를 호출하도록 만들고 싶은 경우
//				SaveA()
//				{ 
//					CFileManager fileMgrA(~~~)
//					fileMgrA.Write()
//					SaveB()
//					{
//						CFileManager fileMgrB(~~~)
//						fileMgrB.Write()
//					}
//				}
//				CFileManager 생성자에서 Load를 하는데
//				SaveB 함수 탈출시, fileMgrB는 소멸되며 내부구조체 저장을 하지만, 
//				SaveA 함수 탈출시, fileMgrA의 내부 구조체는 생성자때 이미 호출되어 SaveB호출 이전의 구조를 갖고있어, SaveB의 저장이 무시됨
//*해결방안*	1. SaveA(); SaveB();를 각각호출하도록 만듬(속도가 아래의 2번보다 다소느릴수 있음)
//				2. 바깥쪽 펑션에서 생성자를 1회 만들고, fileMgr를 포인터로 전달
//////////////////////////////////////////////////////////////////////////
#include <vector>

class CFileManager
{	
	typedef struct ST_FILE_MANAGER_PARAM
	{
		ST_FILE_MANAGER_PARAM()
		{
			strParam = _T("");
			strValue = _T("");
		}
		CString strParam;
		CString strValue;
	}ST_FILE_MANAGER_PARAM;

	typedef std::vector <ST_FILE_MANAGER_PARAM> vFILE_PARAM;

	typedef struct ST_FILE_MANAGER_GROUP
	{
		ST_FILE_MANAGER_GROUP()
		{
			strGroup = _T("");
		}
		CString strGroup;
		vFILE_PARAM vParameter;
	}ST_FILE_MANAGER_GROUP;

	typedef std::vector <ST_FILE_MANAGER_GROUP> vFILE_GROUP;

public:	
	CFileManager(CString strFilePath, CString strFileName, CString strFileExt, CString strBackupPath = _T(""));

	~CFileManager(void);

private:
	CString		m_strFilePath;
	CString		m_strFileName;
	CString		m_strFileExt;
	CString		m_strBackupPath;
	vFILE_GROUP	m_vGroup;
	BOOL		m_bOpen;
	BOOL		m_bChanged; //저장시 바뀌었을때만 소멸자에서 저장하도록한다.
public:
	void DeleteGroup();
	void LoadCSV();
	void SaveCSV();
	void Backup();
	void Write(CString strGroup, CString strParam, CString strData);
	void Write(CString strGroup, CString strParam, int nData);
	void Write(CString strGroup, CString strParam, short nData);
	void Write(CString strGroup, CString strParam, double dData);

	void Read(CString strGroup, CString strParam, CString &strData, CString strDefaultData = _T(""));
	void Read(CString strGroup, CString strParam, int &nData, int nDefaultData = 0);
	void Read(CString strGroup, CString strParam, short &nData, int nDefaultData = 0);
	void Read(CString strGroup, CString strParam, double &dData, double dDefaultData = 0.0);

	BOOL CheckFileExists(LPCTSTR szFileName);
	void CreateDir(LPCTSTR Path);
};

