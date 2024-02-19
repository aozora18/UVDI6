#pragma once

class CPRJobID
{
/* ������ & �ı��� */
public:

	CPRJobID(PTCHAR pj_id, E40_PCCC cmd);
	~CPRJobID();

/* ������ */
protected:

/* ����ü */
protected:

/* ���� ���� */
protected:

	BOOL				m_bAutoStart;							/* Process Job ID�� ���� ��� */
	UINT32				m_u32SubstCount;						/* Process Job ID�� ��ϵ� Substrate ���� */

	TCHAR				m_tzPRJobID[CIM_PROCESS_JOB_ID_SIZE];	/* Process Job ID */

	E40_PCCC			m_enCmdCode;							/* Most recent callback command code */
	E40_PPJS			m_enPRJobState;							/* Process Job State */

	LPG_CSDI			m_pstSubstState;						/* Substrate ID�� ���� �� ���� */

/* ���� �Լ� */
protected:

/* ���� �Լ� */
public:

	PTCHAR				GetPRJobID()	{	return m_tzPRJobID;		}
	E40_PPJS			GetPRJobState()	{	return m_enPRJobState;	}

	BOOL				IsFindCarrID(PTCHAR carr_id);
	BOOL				AddSubstState(CStringArray *carr_id, CByteArray *slot_no);
	INT32				IsFindSlotID(PTCHAR st_id);
	INT32				IsFindState(E90_VSMC machine, INT32 state);

	UINT32				GetCountSubstID()	{	return m_u32SubstCount;	}

	VOID				UpdateCmdCode(E40_PCCC cmd)	{	m_enCmdCode = cmd;	}
	BOOL				UpdateSubstState(UINT32 index, E90_VSMC machine, INT32 state);
	BOOL				UpdateSubstStateAll(BOOL auto_start, E40_PPJS pj_state,
											CAtlList <STG_CSDI> *lst_state);
	BOOL				GetSubstIDStateAll(CAtlList <STG_CSDI> &lst_state);
	BOOL				GetSubstIDState(PTCHAR subst_id, STG_CSDI &subst_info);
};
