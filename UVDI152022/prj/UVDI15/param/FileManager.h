#pragma once
//////////////////////////////////////////////////////////////////////////
//�����ڿ� Load�� ���ԵǾ�������, �Ҹ��ڿ� Save�� ����ִ�.
//
//*��������*	Save���� ����� ���鶧, �Ʒ��� ���� ������ ����� ������ �ɼ��ִ�
//				SaveA���� SaveB�� ȣ���ϵ��� ����� ���� ���
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
//				CFileManager �����ڿ��� Load�� �ϴµ�
//				SaveB �Լ� Ż���, fileMgrB�� �Ҹ�Ǹ� ���α���ü ������ ������, 
//				SaveA �Լ� Ż���, fileMgrA�� ���� ����ü�� �����ڶ� �̹� ȣ��Ǿ� SaveBȣ�� ������ ������ �����־�, SaveB�� ������ ���õ�
//*�ذ���*	1. SaveA(); SaveB();�� ����ȣ���ϵ��� ����(�ӵ��� �Ʒ��� 2������ �ټҴ����� ����)
//				2. �ٱ��� ��ǿ��� �����ڸ� 1ȸ �����, fileMgr�� �����ͷ� ����
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
	BOOL		m_bChanged; //����� �ٲ�������� �Ҹ��ڿ��� �����ϵ����Ѵ�.
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

