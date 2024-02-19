
#pragma once

class CUniToChar
{
public:
	CUniToChar();
	~CUniToChar();

private:
	CHAR			*m_pcData;
	TCHAR			*m_ptData;
	CString			m_strData;

protected:
	void			SetDisplayErrorMsg(CString paraStrErrorMsg);

public:
	CHAR			*Str2Ansi(CString data);			// String -> Ansi Char
	CString			Ansi2Str(CHAR *data);				// Ansi Char -> String

	CHAR			*Uni2Ansi(TCHAR *data);				// TCHAR -> Ansi Char
	TCHAR			*ConstAnsi2Uni(const CHAR *data);
	TCHAR			*Ansi2Uni(CHAR *data);				// Ansi Char -> TCHAR

	CHAR			*Ansi2UTF(CHAR *data);				// Ansi Char -> UTF8
	CHAR			*UTF2Ansi(CHAR *data);				// UTF8 -> Ansi	Char
};

