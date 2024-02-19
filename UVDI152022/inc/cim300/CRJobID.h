#pragma once

class CCRJobID
{
/* ������ & �ı��� */
public:

	CCRJobID(PTCHAR cj_id);
	~CCRJobID();

/* ������ */
protected:

/* ����ü */
protected:

/* ���� ���� */
protected:

	UINT32				m_u32PRJobCount;						/* Process Job ID�� ��� ���� */

	TCHAR				m_tzCRJobID[CIM_CONTROL_JOB_ID_SIZE];	/* Control Job ID */

	BOOL				m_bAutoStart;							/* Control Job ID�� ���� ��� */
	E94_CCJS			m_enCRJobState;							/* Control Job State */

	CStringArray		m_arrCarrIDs;
	CStringArray		m_arrPRJobIDs;
	CByteArray			m_arrAutoStart;

/* ���� �Լ� */
protected:

/* ���� �Լ� */
public:

	PTCHAR				GetCRJobID()	{	return m_tzCRJobID;	}
	E94_CCJS			GetCRJobState()	{	return m_enCRJobState;	}

	VOID				AddProperty(CStringArray *pj_id, CStringArray *carr_id);
	BOOL				UpdateProperty(E94_CCJS cj_state, BOOL auto_start,
									   CStringArray *pj_id, CStringList *carr_id);
	
	BOOL				IsFindCarrID(PTCHAR carr_id);
	PTCHAR				GetCarrID(UINT32 index);
	VOID				GetCarrIDs(CStringArray &carr_ids);
	PTCHAR				GetPRJobID(UINT32 index);
	VOID				GetPRJobIDs(CStringArray &pj_ids);
	BOOL				GetAutoStart(UINT32 index, BOOL &method);
};
