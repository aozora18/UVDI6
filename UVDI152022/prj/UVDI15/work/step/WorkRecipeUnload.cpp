
/*
 desc : 노광 레시피 해제 (Unload)
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
 desc : 생성자
 parm : None
 retn : None
*/
CWorkRecipeUnload::CWorkRecipeUnload()
	: CWorkStep()
{
	m_enWorkJobID	= ENG_BWOK::en_gerb_unload;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CWorkRecipeUnload::~CWorkRecipeUnload()
{
}

/*
 desc : 초기 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkRecipeUnload::InitWork()
{
	/* 내부 멤버 변수 값 초기화 */
	if (!CWork::InitWork())	return FALSE;
	/* 기존에 선택된 레시피 해제 */
	// by sysandj : Recipe 해제는 없어야 함.
	//uvEng_Recipe_ResetSelectRecipe();

	/* 전체 작업 단계 */
	m_u8StepTotal	= 0x06;

	return TRUE;
}

/*
 desc : 주기적으로 작업 수행
 parm : None
 retn : None
*/
VOID CWorkRecipeUnload::DoWork()
{
	/* 작업 단계 별로 동작 처리 */
	switch (m_u8StepIt)
	{
		/* 기존 등록된 모든 거버 제거 */
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

	/* 다음 작업 진행 여부 판단 */
	CWork::SetWorkNext();
	/* 장시간 동안 동일 위치를 반복 수행한다면 에러 처리 */
	CWork::CheckWorkTimeout();
}

/*
 desc : Job List 수신 여부 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkRecipeUnload::IsRecvJobLists()
{
	ENG_JWNS enState	= IsWorkWaitTime();

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Recv.Job.Lists");

	/* 일정 시간 대기 */
	if (enState != ENG_JWNS::en_next)	return enState;

	/* 거버가 없는 경우, 더 이상 삭제할 거버가 없기 때문에 */
	if (!uvCmn_Luria_IsJobList())	m_u8StepIt	= 0x04;	/* Completed 위치 */

	return ENG_JWNS::en_next;
}
