
/*
 desc : Process Job ID 정보 관리
*/

#include "pch.h"
#include "PRJobID.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 desc : 생성자
 parm : None
 retn : None
*/
CPRJobID::CPRJobID(PTCHAR pj_id, E40_PCCC cmd)
{
	m_enCmdCode		= cmd;	/* Process Command Code 설정 */
	m_pstSubstState	= NULL;	/* LPG_CSDI */
	m_u32SubstCount	= 0;
	m_bAutoStart	= FALSE;
	m_enPRJobState	= E40_PPJS::en_reserved5;
	/* Process Job ID 설정 */
	wmemset(m_tzPRJobID, 0x00, CIM_PROCESS_JOB_ID_SIZE);
	wcscpy_s(m_tzPRJobID, CIM_PROCESS_JOB_ID_SIZE, pj_id);
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CPRJobID::~CPRJobID()
{
	if (m_pstSubstState)	::Free(m_pstSubstState);
	m_pstSubstState	= NULL;
}

/*
 desc : Process Job ID에 해당되는 Substrate 정보 등록
 parm : carr_id	- [in]  Carrier ID 리스트가 저장된 배열 포인터
		slot_no	- [in]  Slot Number 리스트가 저장된 배열 포인터
 retn : TRUE or FALSE
*/
BOOL CPRJobID::AddSubstState(CStringArray *carr_id, CByteArray *slot_no)
{
	INT32 i, i32CarrCnt	= 0, i32SlotCnt = 0;
	if (!carr_id || !slot_no)	return FALSE;

	/* 각 항목의 개수 얻기  */
	i32CarrCnt	= (INT32)carr_id->GetCount();
	i32SlotCnt	= (INT32)slot_no->GetCount();
	if (i32CarrCnt < 1 || i32SlotCnt < 1 || i32CarrCnt != i32SlotCnt)	return FALSE;

	/* Substate 상태 정보 저장할 메모리 할당 */
	m_pstSubstState	= (LPG_CSDI)::Alloc(sizeof(STG_CSDI) * i32CarrCnt);
	ASSERT(m_pstSubstState);
	memset(m_pstSubstState, 0x00, sizeof(STG_CSDI) * i32CarrCnt);

	/* 각 항목 값 설정 */
	for (i=0; i<i32CarrCnt; i++)
	{
		wcscpy_s(m_pstSubstState[i].carr_id, CIM_CARRIER_ID_SIZE, carr_id->GetAt(i));
		m_pstSubstState[i].slot_no	= slot_no->GetAt(i);
	}
	/* Substrate ID 개수 저장 */
	m_u32SubstCount	= i;

	return TRUE;
}

/*
 desc : Substrate ID에 매핑된 Carrier ID가 1개 이상 존재하는지 확인
 parm : carr_id	- [in]  검색 대상의 Carrier ID
 retn : TRUE (있다), FALSE (없다)
*/
BOOL CPRJobID::IsFindCarrID(PTCHAR carr_id)
{
	UINT32 i	= 0;

	for (; i<m_u32SubstCount; i++)
	{
		if (0 == wcscmp(m_pstSubstState[i].carr_id, carr_id))	return TRUE;
	}

	return FALSE;
}

/*
 desc : PRJob ID 내에 Substate ID에 대한 상태 값 갱신
 parm : index	- [in]  갱신하려는 Index (Zero-based)
		machine	- [in]  Substrate State Machine Type
		state	- [in]  New State
 retn : TRUE or FALSE
*/
BOOL CPRJobID::UpdateSubstState(UINT32 index, E90_VSMC machine, INT32 state)
{
	if (index >= m_u32SubstCount)	return FALSE;
	m_pstSubstState[index].UpdateState(machine, state);

	return TRUE;
}

/*
 desc : PRJob ID 내에 Substate ID에 대한 상태 값 갱신
 parm : auto_start	- [in]  Process Job ID의 동작 방법 값
		pj_state	- [in]  Process Job ID의 상태 값 갱신
		lst_state	- [in]  상태 값들이 저장된 리스트 버퍼
 retn : TRUE or FALSE
*/
BOOL CPRJobID::UpdateSubstStateAll(BOOL auto_start, E40_PPJS pj_state,
								   CAtlList <STG_CSDI> *lst_state)
{
	INT32 i	= 0;
	POSITION pPos	= NULL;
	STG_CSDI stState;

	/* 입력 파라미터의 정보 유효성 확인 */
	if (!lst_state)	return FALSE;
	if (lst_state->GetCount() < 1)	return FALSE;
	if (m_u32SubstCount != (UINT32)lst_state->GetCount())	return FALSE;

	/* Process Job ID의 시작 방법 값 */
	m_bAutoStart	= auto_start;
	/* Process Job ID의 상태 값 갱신 */
	m_enPRJobState	= pj_state;
	/* 전체 개수 만큼 무조건 복사 진행 */
	for (; i<lst_state->GetCount(); i++)
	{
		stState	= lst_state->GetAt(lst_state->FindIndex(i));
		memcpy(&m_pstSubstState[i], &stState, sizeof(STG_CSDI));
	}
	/* 전체 개수 재조정 (?) */
	if (i >= 0)	m_u32SubstCount	= i;

	return TRUE;
}

/*
 desc : 찾고자 하는 Slot ID가 존재하는지 여부
 parm : st_id	- [in]  검색 대상의 Slot ID (Substrate ID) (carrierid.slot_no)
 retn : 찾은 배열 (인덱스) 위치 값 (Zero-based), 실패의 경우 -1
*/
INT32 CPRJobID::IsFindSlotID(PTCHAR st_id)
{
	UINT32 i	= 0;
	TCHAR tzSubstID[CIM_SUBSTRATE_ID]	= {NULL};

	for (; i<m_u32SubstCount; i++)
	{
		swprintf(tzSubstID, CIM_SUBSTRATE_ID, L"%s.%02d",
				 m_pstSubstState[i].carr_id, m_pstSubstState[i].slot_no);
		if (0 == wcscmp(tzSubstID, st_id))	return INT32(i);
	}

	return INT32(-1);
}

/*
 desc : Process Job ID에 등록된 Substate ID들 중에서 machine type 중 동일한 상태 값이 있는지 확인
 변수 :	machine	- [in]  검색 대상의 Substrate State Machine Type
		state	- [in]  검색 대상의 State
 retn : 찾은 배열 (인덱스) 위치 값 (Zero-based), 실패의 경우 -1
*/
INT32 CPRJobID::IsFindState(E90_VSMC machine, INT32 state)
{
	UINT32 i	= 0;

	for (; i<m_u32SubstCount; i++)
	{
		if (m_pstSubstState[i].IsFindState(machine, state))	return INT32(i);
	}

	return INT32(-1);
}

/*
 desc : Process Job ID에 등록된 Substate ID에 대한 모든 상태 값 반환
 parm : lst_state	- [out] 반환되어 저장될 상태 리스트 버퍼
 retn : TRUE or FALSE
*/
BOOL CPRJobID::GetSubstIDStateAll(CAtlList <STG_CSDI> &lst_state)
{
	UINT32 i	= 0;

	if (m_u32SubstCount < 1)	return FALSE;
	for (; i<m_u32SubstCount; i++)	lst_state.AddTail(m_pstSubstState[i]);
	return lst_state.GetCount() > 0;
}

/*
 desc : Process Job ID에 등록된 Substrate ID의 상태 정보 반환
 parm : subst_id	- [in]  검색 대상의 Substrate ID
		subst_info	- [out] Substrate Information 정보가 저장될 참조 변수
 retn : TRUE or FALSE
*/
BOOL CPRJobID::GetSubstIDState(PTCHAR subst_id, STG_CSDI &subst_info)
{
	BOOL bFind	= FALSE;
	TCHAR tzSubstID[CIM_SUBSTRATE_ID]	= {NULL};
	UINT32 i	= 0;

	if (m_u32SubstCount < 1)	return FALSE;
	for (; i<m_u32SubstCount; i++)
	{
		swprintf_s(tzSubstID, CIM_SUBSTRATE_ID, L"%s.%02d",
				   m_pstSubstState[i].carr_id, m_pstSubstState[i].slot_no);
		if (0 == wcscmp(tzSubstID, subst_id))
		{
			memcpy(&subst_info, &m_pstSubstState[i], sizeof(STG_CSDI));
			bFind	= TRUE;
		}
	}

	return bFind;
}