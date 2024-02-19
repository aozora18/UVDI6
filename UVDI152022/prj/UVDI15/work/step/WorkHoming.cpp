
/*
 desc : �⺻ �ʼ� ��� Homing
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkHoming.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : ������
 parm : None
 retn : None
*/
CWorkHoming::CWorkHoming()
	: CWorkStep()
{
	m_enWorkJobID	= ENG_BWOK::en_work_home;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CWorkHoming::~CWorkHoming()
{
}

/*
 desc : �ʱ� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkHoming::InitWork()
{
	/* ���� ��� ���� �� �ʱ�ȭ */
	if (!CWork::InitWork())	return FALSE;

	/* ��ü �۾� �ܰ� */
	m_u8StepTotal = 0x11;

	return TRUE;
}

/*
 desc : �ֱ������� �۾� ����
 parm : None
 retn : None
*/
VOID CWorkHoming::DoWork()
{
	/* �۾� �ܰ� ���� ���� ó�� */
	switch (m_u8StepIt)
	{
	case 0x01 : m_enWorkState = InitDriveErrorReset();					break;
	case 0x02 : m_enWorkState = IsDriveErrorReseted();					break;
	/* ��� ����̺갡 Homing �����ϴµ� �ּҷ� �ʿ��� ��� �ð� */
	case 0x03 : m_enWorkState = SetWorkWaitTime(1000);					break;
	case 0x04 : m_enWorkState = IsWorkWaitTime();						break;
	/* Stage X / Y / ACam1/2 and PH1/2 Drive�� ���ؼ� Homing ���� */
	case 0x05 : m_enWorkState = DoDriveHomingAll();						break;
	/* Stage X / Y / ACam1/2 and PH1/2 Drive�� Homing �ϴµ� �ּ� ��� (���� �� �ʿ� ������...) */
	case 0x06 : m_enWorkState = SetWorkWaitTime(500);					break;
	case 0x07 : m_enWorkState = IsWorkWaitTime();						break;
	/* Align Camera 2���� ���� ���� Ȯ���� �ʿ� (���� ���� �ʿ�) */
	case 0x08 : m_enWorkState = WaitACamError(2);						break;
	/* Align Camera 2���� ���� ġ��, �ణ ���� ���� ����ϱ� ���� */
	case 0x09 : m_enWorkState = SetWorkWaitTime(1000);					break;
	case 0x0a : m_enWorkState = IsWorkWaitTime();						break;
	/* Align Camera 2�� ������ ���߰�, 1�� Align Camera Limit ���� Ȯ�� */
	case 0x0b : m_enWorkState = ACam2HomingStop();						break;
	/* Align Camera 1�� Homing �Ϸ�� ������ ��� */
	case 0x0c : m_enWorkState = IsDrivedHomed(ENG_MMDI::en_align_cam1);	break;
	/* Align Camera 1�� ���� Ȯ�� �� Align Camera 2�� Re-homing�� */
	case 0x0d : m_enWorkState = DoDriveHoming(ENG_MMDI::en_align_cam2);	break;
	/* Align Camera 2 Drive�� Homing �����ϴµ� �ּҷ� �ʿ��� ��� �ð� */
	case 0x0e : m_enWorkState = SetWorkWaitTime(2000);					break;
	case 0x0f : m_enWorkState = IsWorkWaitTime();						break;
	/* ��� ����̺갡 Homing �Ǿ������� Ȯ�� (ACam2�� ���� ����) */
	case 0x10 : m_enWorkState = IsDrivedHomedAll();						break;
	/* Luria Service ����� ���Ŀ� H/W �ʱ�ȭ�� �ݵ�� �ؾ��ϰ� */
	/* �׸��� ��� �ʱ�ȭ ���Ŀ��� H/W �ʱ�ȭ�� �ݵ�� �ؾ��ϰ� */
	case 0x11 : m_enWorkState = InitializeHWInit();						break;
	}

	/* ���� �۾� ���� ���� �Ǵ� */
	CWork::SetWorkNext();
	/* ��ð� ���� ���� ��ġ�� �ݺ� �����Ѵٸ� ���� ó�� */
	CWork::CheckWorkTimeout();
}
