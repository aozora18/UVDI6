#pragma once

class CCodeToStr
{
/* ������ & �ı��� */
public:

	CCodeToStr(TCHAR *work_dir);
	virtual ~CCodeToStr();

/* ���� �Լ� */
protected:

public:


/* ���� ���� */
protected:

	TCHAR				m_tzWorkPath[MAX_PATH_LEN];

	vector <wstring>	m_vMC2Err;
	vector <wstring>	m_vLuriaReg;
	vector <wstring>	m_vLuriaSys;
	vector <wstring>	m_vLuriaPh;
	vector <wstring>	m_vLuriaSts1;
	vector <wstring>	m_vLuriaSts2;
	vector <wstring>	m_vPlcZErr;


/* ���� �Լ� */
protected:


/* ���� �Լ� */
public:

	BOOL				LoadFile();

	PTCHAR				GetMesg(ENG_SCTS type, UINT16 err_cd);
};
