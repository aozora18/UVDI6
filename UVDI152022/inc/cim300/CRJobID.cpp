
/*
 desc : Control Job ID 정보 관리
*/

#include "pch.h"
#include "CRJobID.h"

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
CCRJobID::CCRJobID(PTCHAR cj_id)
{
	m_u32PRJobCount	= 0;
	m_enCRJobState	= E94_CCJS::en_unknown;
	/* Process Job ID 설정 */
	wmemset(m_tzCRJobID, 0x00, CIM_CONTROL_JOB_ID_SIZE);
	wcscpy_s(m_tzCRJobID, CIM_CONTROL_JOB_ID_SIZE, cj_id);
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CCRJobID::~CCRJobID()
{
}

/*
 desc : E94CJM::Callback 호출될 때, 사용하기 위한 속성 정보 갱신
 변수 :	pj_id	- [in]  Process Job ID가 저장된 배열 포인터
		carr_id	- [in]  Control Job ID 내에 Carrier ID가 저장된 배열 포인터
 retn : None
*/
VOID CCRJobID::AddProperty(CStringArray *pj_id, CStringArray *carr_id)
{
	INT32 i	= 0;

	/* 기존 항목 제거 */
	m_arrPRJobIDs.RemoveAll();
	m_arrCarrIDs.RemoveAll();
	m_arrAutoStart.RemoveAll();

	for (i=0; i<pj_id->GetCount(); i++)
	{
		m_arrPRJobIDs.Add(pj_id->GetAt(i));
	}
	for (i=0; i<carr_id->GetCount(); i++)
	{
		m_arrCarrIDs.Add(carr_id->GetAt(i));
	}
}

/*
 desc : E94CJM::Callback 호출될 때, 사용하기 위한 속성 정보 갱신
 변수 :	cj_state	- [in]  Control Job ID의 상태 값
		auto_start	- [in]  Control Job ID의 Auto Start 여부
		pj_id		- [in]  Process Job ID가 저장된 배열 포인터
		carr_id		- [in]  Control Job ID 내에 Carrier ID가 저장된 배열 포인터
 retn : TRUE or FALSE
*/
BOOL CCRJobID::UpdateProperty(E94_CCJS cj_state, BOOL auto_start,
							  CStringArray *pj_id, CStringList *carr_id)
{
	INT32 i	= 0;

	/* 기존 항목 제거 */
	m_arrPRJobIDs.RemoveAll();
	m_arrCarrIDs.RemoveAll();
	m_arrAutoStart.RemoveAll();

	/* Auto Start 갱신 */
	m_bAutoStart	= auto_start;

	for (i=0; i<pj_id->GetCount(); i++)		m_arrPRJobIDs.Add(pj_id->GetAt(i));
	for (i=0; i<carr_id->GetCount(); i++)	m_arrCarrIDs.Add(carr_id->GetAt(carr_id->FindIndex(i)));

	return TRUE;
}

/*
 desc : 임의 인덱스 (Zero-based) 위치에 있는 Carrier ID 값 반환
 parm : index	- [in]  Zero-based Index 값 (Carrier ID가 저장된 배열 인덱스)
 retn : Carrier ID 값 반환 (NULL이면, 더 이상 존재하지 않음)
*/
PTCHAR CCRJobID::GetCarrID(UINT32 index)
{
	if (m_arrCarrIDs.GetCount() <= index)	return NULL;
	return m_arrCarrIDs[index].GetBuffer();
}

/*
 desc : Control Job ID에 등록된 모든 Carrier ID 값 반환
 parm : carr_ids- [out] Carrier IDs가 반환되어 저장될 배열 리스트 버퍼
 retn : None
*/
VOID CCRJobID::GetCarrIDs(CStringArray &carr_ids)
{
	carr_ids.Copy(m_arrCarrIDs);
}

/*
 desc : 임의 인덱스 (Zero-based) 위치에 있는 Process Job ID 값 반환
 parm : index	- [in]  Zero-based Index 값 (Process Job ID가 저장된 배열 인덱스)
 retn : Process Job ID 값 반환 (NULL이면, 더 이상 존재하지 않음)
*/
PTCHAR CCRJobID::GetPRJobID(UINT32 index)
{
	if (m_arrPRJobIDs.GetCount() <= index)	return NULL;
	return m_arrPRJobIDs[index].GetBuffer();
}

/*
 desc : Control Job ID에 등록된 모든 Process Job ID 값 반환
 parm : pj_ids- [out] Process Job IDs가 반환되어 저장될 배열 리스트 버퍼
 retn : None
*/
VOID CCRJobID::GetPRJobIDs(CStringArray &pj_ids)
{
	pj_ids.Copy(m_arrPRJobIDs);
}

/*
 desc : 임의 인덱스 (Zero-based) 위치에 있는 Process Job ID의 동작 상태 값 반환
 parm : index	- [in]  Zero-based Index 값 (Process Job ID가 저장된 배열 인덱스)
		method	- [out] Process Job ID의 동작 상태 값이 반환될 참조 변수
 retn : TRUE (반환 성공) or FALSE (반환 실패. 검색 실패)
*/
BOOL CCRJobID::GetAutoStart(UINT32 index, BOOL &method)
{
	if (m_arrAutoStart.GetCount() <= index)	return FALSE;
	method	= BOOL(m_arrAutoStart[index]);
	return TRUE;
}

/*
 desc : Carrier ID가 1개 이상 존재하는지 확인
 parm : carr_id	- [in]  검색 대상의 Carrier ID
 retn : TRUE (있다), FALSE (없다)
*/
BOOL CCRJobID::IsFindCarrID(PTCHAR carr_id)
{
	UINT32 i	= 0;

	for (; i<m_arrCarrIDs.GetCount(); i++)
	{
		if (0 == wcscmp(m_arrCarrIDs[i].GetBuffer(), carr_id))	return TRUE;
	}

	return FALSE;
}
