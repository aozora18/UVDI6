
/*
 desc : 기본 객체
*/

#include "pch.h"
#include "Base.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : work_dir	- [in]  실행 파일이 실행되는 경로 (전체 경로, '\' 제외)
		led_power	- [in]  Led Power Recipe 저장 객체 포인터
 retn : None
*/
CBase::CBase(PTCHAR work_dir)
{
	wmemset(m_tzWorkDir, 0x00, MAX_PATH_LEN);
	wcscpy_s(m_tzWorkDir, MAX_PATH_LEN, work_dir);
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CBase::~CBase()
{
}
