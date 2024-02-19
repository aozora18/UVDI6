#pragma once

class CCodeToStr
{
/* 생성자 & 파괴자 */
public:

	CCodeToStr(TCHAR *work_dir);
	virtual ~CCodeToStr();

/* 가상 함수 */
protected:

public:


/* 로컬 변수 */
protected:

	TCHAR				m_tzWorkPath[MAX_PATH_LEN];

	vector <wstring>	m_vMC2Err;
	vector <wstring>	m_vLuriaReg;
	vector <wstring>	m_vLuriaSys;
	vector <wstring>	m_vLuriaPh;
	vector <wstring>	m_vLuriaSts1;
	vector <wstring>	m_vLuriaSts2;
	vector <wstring>	m_vPlcZErr;


/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	BOOL				LoadFile();

	PTCHAR				GetMesg(ENG_SCTS type, UINT16 err_cd);
};
