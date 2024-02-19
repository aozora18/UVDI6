
/*
 desc : Process Job ID ���� ����
*/

#include "pch.h"
#include "PRJobID.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 desc : ������
 parm : None
 retn : None
*/
CPRJobID::CPRJobID(PTCHAR pj_id, E40_PCCC cmd)
{
	m_enCmdCode		= cmd;	/* Process Command Code ���� */
	m_pstSubstState	= NULL;	/* LPG_CSDI */
	m_u32SubstCount	= 0;
	m_bAutoStart	= FALSE;
	m_enPRJobState	= E40_PPJS::en_reserved5;
	/* Process Job ID ���� */
	wmemset(m_tzPRJobID, 0x00, CIM_PROCESS_JOB_ID_SIZE);
	wcscpy_s(m_tzPRJobID, CIM_PROCESS_JOB_ID_SIZE, pj_id);
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CPRJobID::~CPRJobID()
{
	if (m_pstSubstState)	::Free(m_pstSubstState);
	m_pstSubstState	= NULL;
}

/*
 desc : Process Job ID�� �ش�Ǵ� Substrate ���� ���
 parm : carr_id	- [in]  Carrier ID ����Ʈ�� ����� �迭 ������
		slot_no	- [in]  Slot Number ����Ʈ�� ����� �迭 ������
 retn : TRUE or FALSE
*/
BOOL CPRJobID::AddSubstState(CStringArray *carr_id, CByteArray *slot_no)
{
	INT32 i, i32CarrCnt	= 0, i32SlotCnt = 0;
	if (!carr_id || !slot_no)	return FALSE;

	/* �� �׸��� ���� ���  */
	i32CarrCnt	= (INT32)carr_id->GetCount();
	i32SlotCnt	= (INT32)slot_no->GetCount();
	if (i32CarrCnt < 1 || i32SlotCnt < 1 || i32CarrCnt != i32SlotCnt)	return FALSE;

	/* Substate ���� ���� ������ �޸� �Ҵ� */
	m_pstSubstState	= (LPG_CSDI)::Alloc(sizeof(STG_CSDI) * i32CarrCnt);
	ASSERT(m_pstSubstState);
	memset(m_pstSubstState, 0x00, sizeof(STG_CSDI) * i32CarrCnt);

	/* �� �׸� �� ���� */
	for (i=0; i<i32CarrCnt; i++)
	{
		wcscpy_s(m_pstSubstState[i].carr_id, CIM_CARRIER_ID_SIZE, carr_id->GetAt(i));
		m_pstSubstState[i].slot_no	= slot_no->GetAt(i);
	}
	/* Substrate ID ���� ���� */
	m_u32SubstCount	= i;

	return TRUE;
}

/*
 desc : Substrate ID�� ���ε� Carrier ID�� 1�� �̻� �����ϴ��� Ȯ��
 parm : carr_id	- [in]  �˻� ����� Carrier ID
 retn : TRUE (�ִ�), FALSE (����)
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
 desc : PRJob ID ���� Substate ID�� ���� ���� �� ����
 parm : index	- [in]  �����Ϸ��� Index (Zero-based)
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
 desc : PRJob ID ���� Substate ID�� ���� ���� �� ����
 parm : auto_start	- [in]  Process Job ID�� ���� ��� ��
		pj_state	- [in]  Process Job ID�� ���� �� ����
		lst_state	- [in]  ���� ������ ����� ����Ʈ ����
 retn : TRUE or FALSE
*/
BOOL CPRJobID::UpdateSubstStateAll(BOOL auto_start, E40_PPJS pj_state,
								   CAtlList <STG_CSDI> *lst_state)
{
	INT32 i	= 0;
	POSITION pPos	= NULL;
	STG_CSDI stState;

	/* �Է� �Ķ������ ���� ��ȿ�� Ȯ�� */
	if (!lst_state)	return FALSE;
	if (lst_state->GetCount() < 1)	return FALSE;
	if (m_u32SubstCount != (UINT32)lst_state->GetCount())	return FALSE;

	/* Process Job ID�� ���� ��� �� */
	m_bAutoStart	= auto_start;
	/* Process Job ID�� ���� �� ���� */
	m_enPRJobState	= pj_state;
	/* ��ü ���� ��ŭ ������ ���� ���� */
	for (; i<lst_state->GetCount(); i++)
	{
		stState	= lst_state->GetAt(lst_state->FindIndex(i));
		memcpy(&m_pstSubstState[i], &stState, sizeof(STG_CSDI));
	}
	/* ��ü ���� ������ (?) */
	if (i >= 0)	m_u32SubstCount	= i;

	return TRUE;
}

/*
 desc : ã���� �ϴ� Slot ID�� �����ϴ��� ����
 parm : st_id	- [in]  �˻� ����� Slot ID (Substrate ID) (carrierid.slot_no)
 retn : ã�� �迭 (�ε���) ��ġ �� (Zero-based), ������ ��� -1
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
 desc : Process Job ID�� ��ϵ� Substate ID�� �߿��� machine type �� ������ ���� ���� �ִ��� Ȯ��
 ���� :	machine	- [in]  �˻� ����� Substrate State Machine Type
		state	- [in]  �˻� ����� State
 retn : ã�� �迭 (�ε���) ��ġ �� (Zero-based), ������ ��� -1
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
 desc : Process Job ID�� ��ϵ� Substate ID�� ���� ��� ���� �� ��ȯ
 parm : lst_state	- [out] ��ȯ�Ǿ� ����� ���� ����Ʈ ����
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
 desc : Process Job ID�� ��ϵ� Substrate ID�� ���� ���� ��ȯ
 parm : subst_id	- [in]  �˻� ����� Substrate ID
		subst_info	- [out] Substrate Information ������ ����� ���� ����
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