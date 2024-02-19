
/*
 desc : SEMI 작업 동작 스레드
*/

#include "pch.h"
#include "MainApp.h"
#include "WorkThread.h"

#include "E30GEM.h"
#include "E40PJM.h"
#include "E87CMS.h"
#include "E90STS.h"
#include "E94CJM.h"

#include "../../inc/cim300/SharedData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : shared	- [in]  Shared Object 포인터
		e30gem		- [in]  E30GEM Object Pointer
		e40pjm		- [in]  E40PJM Object Pointer
		e87cms		- [in]  E87CMS Object Pointer
		e90sts		- [in]  E90STS Object Pointer
		e94cjm		- [in]  E94CJM Object Pointer
 retn : None
*/
CWorkThread::CWorkThread(CSharedData *shared, CLogData *logs,
						 CE30GEM *e30gem, CE40PJM *e40pjm, CE87CMS *e87cms,
						 CE90STS *e90sts, CE94CJM *e94cjm)
{
	m_u8UpdateStep	= 0x00;
	m_u64TickState	= 0;
	/* Global Object 연결 */
	m_pSharedData	= shared;
	m_pLogData		= logs;
	m_pE30GEM		= e30gem;
	m_pE40PJM		= e40pjm;
	m_pE87CMS		= e87cms;
	m_pE90STS		= e90sts;
	m_pE94CJM		= e94cjm;
}

/*
 desc : 파괴자
 parm : None
*/
CWorkThread::~CWorkThread()
{
}

/*
 desc : 스레드 실행시 주기적 호출됨
 parm : None
 retn : None
*/
VOID CWorkThread::RunWork()
{
	UINT64 u64Tick	= GetTickCount64();

	if (!IsServiceRun())	return;
	if (u64Tick > (m_u64TickState + 1000))
	{
		m_u64TickState	= u64Tick;
		switch (m_u8UpdateStep)
		{
		case 0x00	:	UpdateListPRJobID();	break;
		case 0x01	:	UpdateListCRJobID();	break;
		case 0x02	:	UpdatePortCarrier();	break;
		}
		if (++m_u8UpdateStep == 0x03)	m_u8UpdateStep	= 0x00;
	}
}

/*
 desc : 현재 서비스가 실행 중인지 여부
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkThread::IsServiceRun()
{
	if (!m_pE30GEM)	return FALSE;
	if (!m_pE30GEM->IsInitConnected())	return FALSE;
	/*return m_pE30GEM->IsServiceRun();*/	/* 서로 다른 스레드에서 호출하면 문제가 됨 */
	return m_pE30GEM->IsInitCompleted();
}

/*
 desc : 현재 등록된 PRJob ID 정보 얻기
 parm : None
 retn : None
*/
VOID CWorkThread::UpdateListPRJobID()
{
	BOOL bIsChecked	= FALSE, bAutoStart;
	INT32 i	= 0, j = 0;
	E87_SMVS enStateSlot;
	E40_PPJS enPRJobState;
	STG_CSDI stPRCmd;
	CStringArray arrSubstID, arrCarrID, arrPRJobID;
	CAtlList <STG_CSDI> lstState;
	CAtlList <E40_PPJS>	lstPRJobState;	/* 현재 등록된 PRJob ID의 상태 리스트가 저장될 버퍼 */

	/* 현재 등록된 PRJob ID 얻기 */
	if (!m_pE40PJM->GetProcessAllJobIdState(arrPRJobID, lstPRJobState))	return;

	/* Process Job 개수에 따라 */
	for (i=0; i<arrPRJobID.GetCount(); i++)
	{
		/* Process Job ID에 속한 Substrate ID 리스트 (목록) 얻기 (Carrier ID도 포함됨) */
		if (!m_pE40PJM->GetProcessJobAllSubstID(arrPRJobID[i].GetBuffer(),
												arrSubstID, arrCarrID))	break;
		/* Substrate ID 등록 개수 유효성 확인 */
		for (j=0; j<arrSubstID.GetCount(); j++)
		{
			/* Carrier ID와 Load Port 간의 관련성을 부여하기 전까지는 이후 함수는 정상 동작 하지 않음 */
#if 0	
			if (!m_pE87CMS->IsVerifiedCarrierID(arrCarrID[i].GetBuffer()))
			{
				arrPRJobID.RemoveAll();
				lstPRJobState.RemoveAll();
				return;
			}
#endif
			/* 해당 Substrate를 가지고 있는 Carrier가 Verification OK 인지 확인 */
			if (!m_pE87CMS->GetCarrierIDSlotMapStatus(arrCarrID[j].GetBuffer(), enStateSlot))
			{
				arrPRJobID.RemoveAll();
				lstPRJobState.RemoveAll();
				return;
			}
			if (enStateSlot == E87_SMVS::en_sm_verification_ok)
			{
				/* Substrate ID에 해당되는 각종 상태 정보 반환 */
				memset(&stPRCmd, 0x00, sizeof(STG_CSDI));
				if (m_pE90STS->GetPRCommandSubstState(arrSubstID[j].GetBuffer(), stPRCmd))
				{
					/* 메모리에 등록 */
					lstState.AddTail(stPRCmd);
#ifdef _DEBUG
					TRACE(L"carr_id	= %s\t slot_no = %d  proc_state=%d, trans_state=%d\n",
						  stPRCmd.carr_id, stPRCmd.slot_no, stPRCmd.proc_state, stPRCmd.trans_state);
#endif
				}
			}
		}

		/* Process Job ID의 시작 방법 얻기 */
		if (!m_pE40PJM->GetProcessStartMethod(arrPRJobID[i].GetBuffer(), bAutoStart))
		{
			lstState.RemoveAll();
			break;
		}
		/* PRJob ID에 속한 전체 Substate ID의 상태 정보 갱신 */
		enPRJobState= lstPRJobState.GetAt(lstPRJobState.FindIndex(i));
		bIsChecked	= m_pSharedData->UpdatePRJobToSubstID(arrPRJobID[i].GetBuffer(), bAutoStart,
														  enPRJobState, &lstState);

		/* 기존 등록된 메모리 초기화 */
		if (lstState.GetCount())	lstState.RemoveAll();
		arrSubstID.RemoveAll();
		arrCarrID.RemoveAll();
	}

	/* 기존 등록된 메모리 초기화 */
	arrPRJobID.RemoveAll();
	lstPRJobState.RemoveAll();
}

/*
 desc : 현재 등록된 CRJob ID 정보 얻기
 parm : None
 retn : None
*/
VOID CWorkThread::UpdateListCRJobID()
{
	BOOL bIsChecked	= FALSE, bAutoStart;
	INT32 i	= 0;
	E94_CCJS enCRJobState;
	CStringArray arrPRJobIDs, arrCRJobIDs;
	CStringList lstCarrIDs;
	CAtlList <E94_CCJS>	lstCRJobState;	/* 현재 등록된 CRJob ID의 상태 리스트가 저장될 버퍼 */

	/* 현재 등록된 CRJob ID 얻기 */
	if (!m_pE94CJM->GetControlJobList(arrCRJobIDs, lstCRJobState))	return;

	/* Process Job 개수에 따라 */
	for (i=0; i<arrCRJobIDs.GetCount(); i++)
	{
		/* Control Job ID에 속한 Process Job ID 리스트 (목록) 얻기 */
		if (!m_pE94CJM->GetProcessJobList(arrCRJobIDs[i].GetBuffer(), arrPRJobIDs))	break;;
		/* Control Job ID에 속한 Carrier ID 리스트 (목록) 얻기 */
		if (!m_pE94CJM->GetControlJobCarrierInputSpec(arrCRJobIDs[i].GetBuffer(), lstCarrIDs))	break;
		/* Auto Start 여부 */
		if (!m_pE94CJM->GetControlJobStartMethod(arrCRJobIDs[i].GetBuffer(), bAutoStart))	break;


		/* CRJob ID에 관련된 속성 정보 갱신 */
		enCRJobState= lstCRJobState.GetAt(lstCRJobState.FindIndex(i));
		bIsChecked	= m_pSharedData->UpdateCRJobToProperty(arrCRJobIDs[i].GetBuffer(),
														   enCRJobState, bAutoStart,
														   &arrPRJobIDs, &lstCarrIDs);
		/* 기존 등록된 메모리 초기화 */
		arrPRJobIDs.RemoveAll();
		lstCarrIDs.RemoveAll();
	}

	/* 기존 등록된 메모리 초기화 */
	arrCRJobIDs.RemoveAll();
	lstCRJobState.RemoveAll();
}

/*
 desc : Load Port와 Carrier ID에 대한 정보 갱신
 parm : None
 retn : None
*/
VOID CWorkThread::UpdatePortCarrier()
{
	UINT8 i	= 0x01;
	STG_CLPI stData;

	for (; i<=MAX_FOUP_COUNT; i++)
	{
		memset(&stData, 0x00, sizeof(STG_CLPI));
		if (m_pE87CMS->GetLoadPortCarrierData(i, stData))
		{
			m_pSharedData->UpdateLoadPortDataAll(i, &stData);
		}
	}
}
