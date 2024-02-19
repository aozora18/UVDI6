#pragma once

class CCRJobID
{
/* 생성자 & 파괴자 */
public:

	CCRJobID(PTCHAR cj_id);
	~CCRJobID();

/* 열거형 */
protected:

/* 구조체 */
protected:

/* 로컬 변수 */
protected:

	UINT32				m_u32PRJobCount;						/* Process Job ID의 등록 개수 */

	TCHAR				m_tzCRJobID[CIM_CONTROL_JOB_ID_SIZE];	/* Control Job ID */

	BOOL				m_bAutoStart;							/* Control Job ID의 동작 방법 */
	E94_CCJS			m_enCRJobState;							/* Control Job State */

	CStringArray		m_arrCarrIDs;
	CStringArray		m_arrPRJobIDs;
	CByteArray			m_arrAutoStart;

/* 로컬 함수 */
protected:

/* 공용 함수 */
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
