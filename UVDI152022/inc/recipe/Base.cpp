
/*
 desc : �⺻ ��ü
*/

#include "pch.h"
#include "Base.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : work_dir	- [in]  ���� ������ ����Ǵ� ��� (��ü ���, '\' ����)
		led_power	- [in]  Led Power Recipe ���� ��ü ������
 retn : None
*/
CBase::CBase(PTCHAR work_dir)
{
	wmemset(m_tzWorkDir, 0x00, MAX_PATH_LEN);
	wcscpy_s(m_tzWorkDir, MAX_PATH_LEN, work_dir);
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CBase::~CBase()
{
}
