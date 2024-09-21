
/*
 desc : �뱤 ������ ���� (Unload)
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkRecipeUnload.h"
#include "../../GlobalVariables.h"

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
CWorkRecipeUnload::CWorkRecipeUnload()
	: CWorkStep()
{
	m_enWorkJobID	= ENG_BWOK::en_gerb_unload;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CWorkRecipeUnload::~CWorkRecipeUnload()
{
}

/*
 desc : �ʱ� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkRecipeUnload::InitWork()
{
	/* ���� ��� ���� �� �ʱ�ȭ */
	if (!CWork::InitWork())	return FALSE;
	/* ������ ���õ� ������ ���� */
	// by sysandj : Recipe ������ ����� ��.
	//uvEng_Recipe_ResetSelectRecipe();

	/* ��ü �۾� �ܰ� */
	m_u8StepTotal	= 0x06;

	return TRUE;
}

/*
 desc : �ֱ������� �۾� ����
 parm : None
 retn : None
*/
VOID CWorkRecipeUnload::DoWork()
{
	/* �۾� �ܰ� ���� ���� ó�� */
	switch (m_u8StepIt)
	{
		/* ���� ��ϵ� ��� �Ź� ���� */
		case 0x01 : m_enWorkState = GetJobLists(2000);				break;
		case 0x02 : m_enWorkState = IsRecvJobLists();				break;
		case 0x03 : m_enWorkState = SetSelectedJobIndex();			break;
		case 0x04 : m_enWorkState = IsSelectedJobIndex();			break;
		case 0x05 : m_enWorkState = SetDeleteSelectedJobName(1000);	break;
		case 0x06 : 
		{
			GlobalVariables::GetInstance()->GetAlignMotion().markParams.Reset();
			m_enWorkState = IsDeleteSelectedJobName(0x00);
		}
		break;

	}

	/* ���� �۾� ���� ���� �Ǵ� */
	CWork::SetWorkNext();
	/* ��ð� ���� ���� ��ġ�� �ݺ� �����Ѵٸ� ���� ó�� */
	CWork::CheckWorkTimeout();
}

/*
 desc : Job List ���� ���� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkRecipeUnload::IsRecvJobLists()
{
	ENG_JWNS enState	= IsWorkWaitTime();

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Recv.Job.Lists");

	/* ���� �ð� ��� */
	if (enState != ENG_JWNS::en_next)	return enState;

	/* �Ź��� ���� ���, �� �̻� ������ �Ź��� ���� ������ */
	if (!uvCmn_Luria_IsJobList())	m_u8StepIt	= 0x04;	/* Completed ��ġ */

	return ENG_JWNS::en_next;
}
