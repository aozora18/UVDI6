#pragma once

class CPRJobID
{
/* 생성자 & 파괴자 */
public:

	CPRJobID(PTCHAR pj_id, E40_PCCC cmd);
	~CPRJobID();

/* 열거형 */
protected:

/* 구조체 */
protected:

/* 로컬 변수 */
protected:

	BOOL				m_bAutoStart;							/* Process Job ID의 시작 방법 */
	UINT32				m_u32SubstCount;						/* Process Job ID에 등록된 Substrate 개수 */

	TCHAR				m_tzPRJobID[CIM_PROCESS_JOB_ID_SIZE];	/* Process Job ID */

	E40_PCCC			m_enCmdCode;							/* Most recent callback command code */
	E40_PPJS			m_enPRJobState;							/* Process Job State */

	LPG_CSDI			m_pstSubstState;						/* Substrate ID에 대한 상세 정보 */

/* 로컬 함수 */
protected:

/* 공용 함수 */
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
