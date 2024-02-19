
#pragma once

class CBoxCtrl : public CListBox
{
	DECLARE_DYNAMIC (CBoxCtrl)

/* 생성자 & 파괴자 */
public:

	CBoxCtrl();
	virtual ~CBoxCtrl();

/* 로컬 변수 */
protected:

	CFont				m_csFont;

/* 로컬 함수 */
protected:

/* 공용 함수 */
public:

	VOID				SetBoxFont(LOGFONT *lf);

protected:
	
	DECLARE_MESSAGE_MAP()

};