
#pragma once

class CBase
{
/* 생성자 & 파괴자 */
public:

	CBase(PTCHAR work_dir);
	virtual ~CBase();


/* 로컬 변수 */
protected:

	TCHAR				m_tzWorkDir[MAX_PATH_LEN];



/* 로컬 함수 */
protected:



/* 공용 함수 */
public:

};
