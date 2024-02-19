
/*
 desc : Control Job ID ���� ����
*/

#include "pch.h"
#include "CRJobID.h"

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
CCRJobID::CCRJobID(PTCHAR cj_id)
{
	m_u32PRJobCount	= 0;
	m_enCRJobState	= E94_CCJS::en_unknown;
	/* Process Job ID ���� */
	wmemset(m_tzCRJobID, 0x00, CIM_CONTROL_JOB_ID_SIZE);
	wcscpy_s(m_tzCRJobID, CIM_CONTROL_JOB_ID_SIZE, cj_id);
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CCRJobID::~CCRJobID()
{
}

/*
 desc : E94CJM::Callback ȣ��� ��, ����ϱ� ���� �Ӽ� ���� ����
 ���� :	pj_id	- [in]  Process Job ID�� ����� �迭 ������
		carr_id	- [in]  Control Job ID ���� Carrier ID�� ����� �迭 ������
 retn : None
*/
VOID CCRJobID::AddProperty(CStringArray *pj_id, CStringArray *carr_id)
{
	INT32 i	= 0;

	/* ���� �׸� ���� */
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
 desc : E94CJM::Callback ȣ��� ��, ����ϱ� ���� �Ӽ� ���� ����
 ���� :	cj_state	- [in]  Control Job ID�� ���� ��
		auto_start	- [in]  Control Job ID�� Auto Start ����
		pj_id		- [in]  Process Job ID�� ����� �迭 ������
		carr_id		- [in]  Control Job ID ���� Carrier ID�� ����� �迭 ������
 retn : TRUE or FALSE
*/
BOOL CCRJobID::UpdateProperty(E94_CCJS cj_state, BOOL auto_start,
							  CStringArray *pj_id, CStringList *carr_id)
{
	INT32 i	= 0;

	/* ���� �׸� ���� */
	m_arrPRJobIDs.RemoveAll();
	m_arrCarrIDs.RemoveAll();
	m_arrAutoStart.RemoveAll();

	/* Auto Start ���� */
	m_bAutoStart	= auto_start;

	for (i=0; i<pj_id->GetCount(); i++)		m_arrPRJobIDs.Add(pj_id->GetAt(i));
	for (i=0; i<carr_id->GetCount(); i++)	m_arrCarrIDs.Add(carr_id->GetAt(carr_id->FindIndex(i)));

	return TRUE;
}

/*
 desc : ���� �ε��� (Zero-based) ��ġ�� �ִ� Carrier ID �� ��ȯ
 parm : index	- [in]  Zero-based Index �� (Carrier ID�� ����� �迭 �ε���)
 retn : Carrier ID �� ��ȯ (NULL�̸�, �� �̻� �������� ����)
*/
PTCHAR CCRJobID::GetCarrID(UINT32 index)
{
	if (m_arrCarrIDs.GetCount() <= index)	return NULL;
	return m_arrCarrIDs[index].GetBuffer();
}

/*
 desc : Control Job ID�� ��ϵ� ��� Carrier ID �� ��ȯ
 parm : carr_ids- [out] Carrier IDs�� ��ȯ�Ǿ� ����� �迭 ����Ʈ ����
 retn : None
*/
VOID CCRJobID::GetCarrIDs(CStringArray &carr_ids)
{
	carr_ids.Copy(m_arrCarrIDs);
}

/*
 desc : ���� �ε��� (Zero-based) ��ġ�� �ִ� Process Job ID �� ��ȯ
 parm : index	- [in]  Zero-based Index �� (Process Job ID�� ����� �迭 �ε���)
 retn : Process Job ID �� ��ȯ (NULL�̸�, �� �̻� �������� ����)
*/
PTCHAR CCRJobID::GetPRJobID(UINT32 index)
{
	if (m_arrPRJobIDs.GetCount() <= index)	return NULL;
	return m_arrPRJobIDs[index].GetBuffer();
}

/*
 desc : Control Job ID�� ��ϵ� ��� Process Job ID �� ��ȯ
 parm : pj_ids- [out] Process Job IDs�� ��ȯ�Ǿ� ����� �迭 ����Ʈ ����
 retn : None
*/
VOID CCRJobID::GetPRJobIDs(CStringArray &pj_ids)
{
	pj_ids.Copy(m_arrPRJobIDs);
}

/*
 desc : ���� �ε��� (Zero-based) ��ġ�� �ִ� Process Job ID�� ���� ���� �� ��ȯ
 parm : index	- [in]  Zero-based Index �� (Process Job ID�� ����� �迭 �ε���)
		method	- [out] Process Job ID�� ���� ���� ���� ��ȯ�� ���� ����
 retn : TRUE (��ȯ ����) or FALSE (��ȯ ����. �˻� ����)
*/
BOOL CCRJobID::GetAutoStart(UINT32 index, BOOL &method)
{
	if (m_arrAutoStart.GetCount() <= index)	return FALSE;
	method	= BOOL(m_arrAutoStart[index]);
	return TRUE;
}

/*
 desc : Carrier ID�� 1�� �̻� �����ϴ��� Ȯ��
 parm : carr_id	- [in]  �˻� ����� Carrier ID
 retn : TRUE (�ִ�), FALSE (����)
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
