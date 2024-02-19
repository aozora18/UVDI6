
#include "pch.h"
#include "MainApp.h"
#include "E94CJM.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                        Philoptics Area                                        */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : logs	- [in]  Log Object Pointer
		share	- [in]  Shared Data Object Pointer
 retn : None
*/
CE94CJM::CE94CJM(CLogData *logs, CSharedData *share)
	: CEBase(logs, share)
{
	/* GUID 설정 */
	m_gdICxE	= IID_ICxE94CJM;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CE94CJM::~CE94CJM()
{
	Close();
}

/*
 desc : 초기화 (반드시 호출)
 parm : equip_id	- [in]  Equipment ID (0 or later. 환경 파일에 설정된 개수-1)
		conn_id		- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
 retn : TRUE or FALSE
*/
BOOL CE94CJM::Init(LONG equip_id, LONG conn_id)
{
	if (!CEBase::Init(equip_id, conn_id))	return FALSE;

	HRESULT hResult	= S_OK;
	IUnknownPtr pIUnknownICxE87CMS, pIUnknownICxE40PJM;
	ICxE94ControlJobCBPtr pICxE94ControlJobCB;
	ICxE94ControlJobCB2Ptr pICxE94ControlJobCB2;
	ICxE94ControlJobCB3Ptr pICxE94ControlJobCB3;
	ICxE94ControlJobCB4Ptr pICxE94ControlJobCB4;
	ICxE94ControlProcessJobCBPtr pICxE94ControlProcessJobCB;

	pIUnknownICxE87CMS	= m_pICxE87CMSPtr;
	pIUnknownICxE40PJM	= m_pICxE40PJMPtr;

	if (m_pICxE94CJMPtr && m_pICxE94CJMPtr.GetInterfacePtr())	return FALSE;

	m_pLogData->PushLogs(L"ICxE94CJM::CreateInstance");
	hResult = m_pICxE94CJMPtr.CreateInstance(CLSID_CxE94CJM);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE94CJM::CreateInstance CLSID_CxE94CJM")))
		return FALSE;
	m_pLogData->PushLogs(L"ICxE94CJM::Initialize2");
	hResult = m_pICxE94CJMPtr->Initialize2(m_pICxAbsLayerPtr, m_pICxE39ObjPtr,
										   CIM_E94_JOB_QUE_SIZE, pIUnknownICxE40PJM, pIUnknownICxE87CMS);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE94CJM::Initialize2")))	return FALSE;
	/* Register callback(s) function */
	pICxE94ControlJobCB		= m_pICxEMAppCbk;
	pICxE94ControlJobCB2	= m_pICxEMAppCbk;
	pICxE94ControlJobCB3	= m_pICxEMAppCbk;
	pICxE94ControlJobCB4	= m_pICxEMAppCbk;
	hResult = m_pICxE94CJMPtr->RegisterCallback(pICxE94ControlJobCB);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE94CJM::RegisterCallback")))	return FALSE;
	hResult = m_pICxE94CJMPtr->RegisterCallback2(pICxE94ControlJobCB2);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE94CJM::RegisterCallback2")))	return FALSE;
	hResult = m_pICxE94CJMPtr->RegisterCallback3(pICxE94ControlJobCB3);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE94CJM::RegisterCallback3")))	return FALSE;
	hResult = m_pICxE94CJMPtr->RegisterCallback4(pICxE94ControlJobCB4);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE94CJM::RegisterCallback4")))	return FALSE;

	/* Register the global interface */
	m_pLogData->PushLogs(L"RegisterInterfaceInGlobal", m_tzGUID);
	hResult	= m_pGIT->RegisterInterfaceInGlobal(m_pICxE94CJMPtr, m_gdICxE, &m_dwCookie);
	if (FAILED(m_pLogData->CheckResult(hResult, L"RegisterInterfaceInGlobal")))	return FALSE;
	m_pSharedData->GetGITCookie()->e94cjm_cookie	= m_dwCookie;

	return TRUE;
}

/*
 desc : 해제
 parm : None
 retn : None
*/
VOID CE94CJM::Close()
{
	HRESULT hResult	= S_OK;

	if (m_pICxE94CJMPtr.GetInterfacePtr())
	{
		m_pLogData->PushLogs(L"ICxE94CJM::Shutdown");
		hResult = m_pICxE94CJMPtr->Shutdown();
		m_pLogData->CheckResult(hResult, L"ICxE94CJM::Shutdown");
		m_pLogData->PushLogs(L"ICxE94CJM::Release");
		m_pICxE94CJMPtr.Release();
	}

	CEBase::Close();
}

/*
 desc : 로그 레벨 값 설정
 parm : level	- [in]  로그 레벨 값
 retn : TRUE or FALSE
*/
BOOL CE94CJM::SetLogLevel(ENG_ILLV level)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetLogLevel::CIM94LogLevel level : [%d]", level);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE94CJMPtr->put_CIM94LogLevel((LONG)level);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Process Job들 중에서 한 개가 완료되면 Control Job에게 알림
 parm : cj_id	- [in]  Object ID of the Control Job
		pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
 note : Notifies the control job that one of its process jobs has been completed.
*/
BOOL CE94CJM::SetProcessJobComplete(PTCHAR cj_id, PTCHAR pj_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	HRESULT hResult		= S_OK;
	CComBSTR bstrCtrlID, bstrProcID;

	/* Control & Process Job ID 설정 */
	bstrCtrlID	= CComBSTR(cj_id);
	bstrProcID	= CComBSTR(pj_id);

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetProcessJobComplete::ProcessJobComplete control_jo_id / "
										  L"process_job_id : [%s] / [%s]", cj_id, pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE94CJMPtr->ProcessJobComplete(bstrCtrlID, bstrProcID);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : 현재 Queue에 등록된 Control Job ID 목록 및 각 상태 값들 반환
 parm : list_id		- [out] 반환되어 저장될 Control Job ID (String)
		list_state	- [out] 수집된 Control Job IDs에 대한 상태 값이 저장될 리스트
		logs		- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
BOOL CE94CJM::GetControlJobList(CStringArray &list_cj, CAtlList<E94_CCJS> &list_state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	LONG i	= 0;
	HRESULT hResult	= S_OK;
	VARIANT varIDList;
	CComBSTR bstrProcID;
	BSTR *bstrList;
	SAFEARRAY *psaList;
	E94CJState valState;

	/* 기존 등록된 것 모두 삭제 */
	list_cj.RemoveAll();
	list_state.RemoveAll();

	/* Initializes a variant */
	VariantInit(&varIDList);

	/* Control Job List 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobList::ListControlJobs");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE94CJMPtr->ListControlJobs(E94CJListAll, &varIDList);
	m_pLogData->CheckResult(hResult, tzMesg);

	if (hResult	== S_OK)
	{
		psaList	= varIDList.parray;
		hResult	= SafeArrayAccessData(psaList, (VOID**)&bstrList);	/* Lock it down */
		m_pLogData->CheckResult(hResult, tzMesg);
		for (; i<(LONG)psaList->rgsabound->cElements; i++)
		{
			/* 동일한 상태인지 비교 */
			swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
					   L"GetControlJobList::get_CJState Control ID : [%s]",
					   (LPTSTR)_bstr_t(bstrList[i]));
			m_pLogData->PushLogs(tzMesg);
			hResult	= m_pICxE94CJMPtr->get_CJState(bstrList[i], &valState);
			if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

			/* 검색된 값들 등록 */
			list_cj.Add((LPTSTR)_bstr_t(bstrList[i]));
			list_state.AddTail(E94_CCJS(valState));
		}
	}
	return TRUE;
}

/*
 desc : Control Job ID 중 필터에 의해 검색된 것 중 첫 번째 검색된 Job ID 반환
 parm : filter	- [in]  Process Job ID 검색 필터 (E40_PPJS)
		cj_id	- [out] 반환되어 저장될 Control Job ID (String)
		size	- [in]  'cj_id' 버퍼 크기
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
BOOL CE94CJM::GetControlJobFirstID(E94_CCJS filter, PTCHAR cj_id, UINT32 size)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	LONG i	= 0;
	HRESULT hResult	= S_OK;
	VARIANT varIDList;
	CComBSTR bstrProcID;
	BSTR *bstrList;
	SAFEARRAY *psaList;
	E94CJState valState;
	CString strCtrlID;

	/* Initializes a variant */
	VariantInit(&varIDList);

	/* Control Job List 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobFirstID::ListControlJobs filter : [%d]", filter);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE94CJMPtr->ListControlJobs(E94ListControlJobs(filter), &varIDList);
	m_pLogData->CheckResult(hResult, tzMesg);

	if (hResult	== S_OK)
	{
		psaList	= varIDList.parray;
		hResult	= SafeArrayAccessData(psaList, (VOID**)&bstrList);	/* Lock it down */
		m_pLogData->CheckResult(hResult, tzMesg);
		for (; i<(LONG)psaList->rgsabound->cElements; i++)
		{
			/* ID 추출 */
			strCtrlID.Format(L"%s", bstrList[i]);

			/* 동일한 상태인지 비교 */
			swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
					   L"GetControlJobFirstID::get_CJState Control ID : [%s]", strCtrlID.GetBuffer());
			m_pLogData->PushLogs(tzMesg);
			hResult	= m_pICxE94CJMPtr->get_CJState(bstrList[i], &valState);
			m_pLogData->CheckResult(hResult, tzMesg);
			if (valState == E94CJState(filter))
			{
				swprintf_s(cj_id, size, L"%s", strCtrlID.GetBuffer());
				return TRUE;
			}
		}
	}
	return TRUE;
}

/*
 desc : 전체 Control Job List 얻기 (Control Job 내부에 저장된 Project Job List)
 parm : list_id		- [out] 수집된 Control Job IDs (Names) 저장될 배열 버퍼
		list_state	- [out] 수집된 Control Job IDs에 대한 상태 값이 저장될 리스트
		filter		- [in]  Control Job 추출 필터 (필터)
		logs		- [in]  로그 출력 여부
 retn : TRUE ro FALSE
*/
BOOL CE94CJM::GetControlJobList(CStringArray &list_id, CAtlList<E94_CCJS> &list_state,
								E94_CLCJ filter)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	LONG i	= 0;
	HRESULT hResult	= S_OK;
	E94_CCJS enState;

	/* 기존 등록된 것 모두 삭제 */
	list_id.RemoveAll();
	list_state.RemoveAll();

	if (!GetControlJobList(list_id, filter))	return FALSE;

	for (i=0; i<list_id.GetCount(); i++)
	{
		if (!GetControlJobState((LPTSTR)list_id[i].GetBuffer(), enState))	return FALSE;
		list_state.AddTail(enState);
	}

	/* 만약 IDs와 States 개수 간의 차이가 있으면 에러 */
	if (list_id.GetCount() != list_state.GetCount())
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"IDs (%d) and States (%d) counts are not the same",
				   (INT32)list_id.GetCount(), (INT32)list_state.GetCount());
		m_pLogData->PushLogs(tzMesg, ENG_GALC::en_warning);
		list_id.RemoveAll();
		list_state.RemoveAll();
		return FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : Control Job List 얻기 (Control Job 내부에 저장된 Project Job List)
 parm : list	- [out] 수집된 Control Job IDs (Names) 저장될 배열 버퍼
		filter	- [in]  Control Job 추출 필터 (필터)
		logs	- [in]  로그 출력 여부
 retn : TRUE ro FALSE
*/
BOOL CE94CJM::GetControlJobList(CStringArray &list, E94_CLCJ filter)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	LONG i	= 0;
	HRESULT hResult	= S_OK;
	VARIANT varIDList;
	CComBSTR bstrProcID;
	BSTR *bstrList;
	SAFEARRAY *psaList;
	CString strCtrlID;

	if (!m_pICxE94CJMPtr)	return FALSE;

	/* 기존 등록된 것 모두 삭제 */
	list.RemoveAll();

	/* Initializes a variant */
	VariantInit(&varIDList);

	/* Control Job List 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobsList::ListControlJobs filter : [%d]", filter);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE94CJMPtr->ListControlJobs(E94ListControlJobs(filter), &varIDList);
	m_pLogData->CheckResult(hResult, tzMesg);

	if (hResult	== S_OK)
	{
		psaList	= varIDList.parray;
		hResult	= SafeArrayAccessData(psaList, (VOID**)&bstrList);	/* Lock it down */
		m_pLogData->CheckResult(hResult, tzMesg);
		for (; i<(LONG)psaList->rgsabound->cElements; i++)
		{
			strCtrlID.Format(L"%s", bstrList[i]);
			list.Add(strCtrlID);
		}
	}

	return (hResult == S_OK);
}

/*
 desc : Process Jobs Count 얻기
 parm : cj_id	- [in]  Object ID of the Control Job
		logs	- [in]  로그 출력 여부
 retn : 음수 - 값 얻기 실패, 0 or Later
*/
INT32 CE94CJM::GetProcessJobCount(PTCHAR cj_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	HRESULT hResult	= S_OK;
	VARIANT varIDList;
	SAFEARRAY *pSaList;

	/* Initializes a variant */
	VariantInit(&varIDList);

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessJobCount::ListProcessJobs [%s]", cj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE94CJMPtr->ListProcessJobs(CComBSTR(cj_id), E94ListAll, &varIDList);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return -1;
	pSaList	= varIDList.parray;

	/* 현재 Process Job ID 개수 */
	return (INT32)pSaList->rgsabound->cElements;
}

/*
 desc : Process Jobs List 얻기 (Control Job 내부에 저장된 Project Job List)
 parm : cj_id	- [in]  Object ID of the Control Job
		list	- [out] 수집된 Process Job IDs (Names) 저장될 배열 버퍼
		filter	- [in]  Control Job 내부에 저장된 Process Job 추출 필터 (필터)
		logs	- [in]  로그 출력 여부
 retn : TRUE ro FALSE
*/
BOOL CE94CJM::GetProcessJobList(PTCHAR cj_id, CStringArray &list, E94_CLPJ filter)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	LONG i	= 0;
	HRESULT hResult	= S_OK;
	VARIANT varIDList;
	CComBSTR bstrProcID;
	BSTR *bstrList;
	SAFEARRAY *pSaList;
	CString strProcID;

	/* 기존 등록된 것 모두 삭제 */
	list.RemoveAll();

	/* Initializes a variant */
	VariantInit(&varIDList);

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessJobList::ListProcessJobs cj_id=%s, filter : [%d]",
			   cj_id, filter);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE94CJMPtr->ListProcessJobs(CComBSTR(cj_id), E94ListProcessJobs(filter), &varIDList);
	m_pLogData->CheckResult(hResult, tzMesg);

	if (hResult	== S_OK)
	{
		pSaList	= varIDList.parray;
		hResult	= SafeArrayAccessData(pSaList, (VOID**)&bstrList);	/* Lock it down */
		m_pLogData->CheckResult(hResult, tzMesg);
		for (; i<(LONG)pSaList->rgsabound->cElements; i++)
		{
			strProcID.Format(L"%s", bstrList[i]);
			list.Add(strProcID);
		}
	}

	return (list.GetCount() && hResult == S_OK);
}

/*
 desc : Control Job의 상태 정보 반환
 parm : cj_id	- [in]  Control Job ID
		state	- [out] Job ID의 상태 값 반환
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
BOOL CE94CJM::GetControlJobState(PTCHAR cj_id, E94_CCJS &state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	HRESULT hResult		= S_OK;
	CComBSTR bstrCtrlID(cj_id);
	E94CJState enState;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobState::get_CJState Control ID : [%s]", cj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE94CJMPtr->get_CJState(bstrCtrlID, &enState);
	m_pLogData->CheckResult(hResult, tzMesg);
	/* 값 반환 */
	state	= E94_CCJS(enState);

	return (hResult == S_OK);
}

/*
 desc : Control Job 생성
 parm : cj_id		- [in]  Control Job ID
		carr_id		- [in]  문자열 ID (전역으로 관리되는 ID)
		pj_id		- [in]  Process Job List 가 저장된 Array 배열
		job_order	- [in]  Job Order (3: List, 1: Arrival)
		start		- [in]  Auto Start 여부
 retn : TRUE or FALSE
*/
BOOL CE94CJM::CreateJob(PTCHAR cj_id, PTCHAR carr_id, CStringArray *pj_id,
						E94_CPOM job_order, BOOL start)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	LONG i, lHandle[4]	= {NULL}/*, lStatus*/;
	HRESULT hResult		= S_OK;
	VARIANT_BOOL varACKA= VARIANT_TRUE;
	ICxValueDispPtr valAttrPtr (CLSID_CxValueObject);
	ICxValueDispPtr valStatusPtr (CLSID_CxValueObject);

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"CreateJob::Control Job ID : [%s]", cj_id);
	m_pLogData->PushLogs(tzMesg);

	/* Set : Object ID : Control Job ID */
	/*   Host defined identifier of the control job */
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendList(0, &lHandle[0]);
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendValueAscii(lHandle[0], CComBSTR(L"ObjID"));
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendValueAscii(lHandle[0], CComBSTR(cj_id));

	/* Carrier Input Spec */
	/*   A list of carrierID for material that will be used by the ControlJob */
	/*   An empty list is allowed */
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendList(0, &lHandle[0]);
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendValueAscii(lHandle[0], CComBSTR(L"CarrierInputSpec"));
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendList(lHandle[0], &lHandle[1]);
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendValueAscii(lHandle[1], CComBSTR(carr_id));

	/* Material Out Spec */
	/*   Maps material from source to destination after processing */
	/*   In Slot Integrity Mode the list shall be empty */
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendList(0, &lHandle[0]);
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendValueAscii(lHandle[0], CComBSTR(L"MtrlOutSpec"));
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendList(lHandle[0], &lHandle[1]);

	/* Processing Control Spec */
	/*   A list of structures that defines the process jobs and rules for running each that will be run */
	/*   within this ControlJob */
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendList(0, &lHandle[0]);
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendValueAscii(lHandle[0], CComBSTR(L"ProcessingCtrlSpec"));
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendList(lHandle[0], &lHandle[1]);
	for (i=0; (S_OK == hResult) && (i<pj_id->GetCount()); i++)
	{
		BSTR bstrProcID	= pj_id->GetAt(i).AllocSysString();
		if (S_OK == hResult)	hResult	= valAttrPtr->AppendList(lHandle[1], &lHandle[2]);
		if (S_OK == hResult)	hResult	= valAttrPtr->AppendValueAscii(lHandle[2], bstrProcID);
		if (S_OK == hResult)	hResult	= valAttrPtr->AppendList(lHandle[2], &lHandle[3]);	/* Control Rule */
		if (S_OK == hResult)	hResult	= valAttrPtr->AppendList(lHandle[2], &lHandle[3]);	/* Output Rule */
		SysFreeString(bstrProcID);
	}

	/* Process Order Magement (ProcessOrderMgmt) */
	/*   Define the method for the order in which process jobs are initiated */
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendList(0, &lHandle[0]);
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendValueAscii(lHandle[0],CComBSTR( L"ProcessOrderMgmt"));
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendValueU1(lHandle[0], (UINT8)job_order);	/* Job Order : List, Arrival or Optimize */

	/* StartMethod */
	/*   A logical flag that determines if the ControlJob can start automatically */
	/*   A user start may come through either the host connection or the operator console */
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendList(0, &lHandle[0]);
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendValueAscii(lHandle[0], CComBSTR(L"StartMethod"));
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendValueBoolean(lHandle[0], VARIANT_BOOL(start));
#if 0
	/* Data Collection Plan */
	/*   Identifier for a data collection plan to be used during execution of the control job */
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendList(0, &lHandle[0]);
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendValueAscii(lHandle[0], CComBSTR(L"DataCollectionPlan"));
#if 0
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendValueAscii(lHandle[0], CComBSTR(L"None"));
#else
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendValueAscii(lHandle[0], CComBSTR(L""));
#endif
	/* Material Out By Status */
	/*   List structure which maps locations or Carriers where processed material will be placed */
	/*   based on material status */
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendList(0, &lHandle[0]);
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendValueAscii(lHandle[0], CComBSTR(L"MtrlOutByStatus"));
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendList(lHandle[0], &lHandle[1]);
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendList(lHandle[1], &lHandle[2]);
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendValueAscii(lHandle[2], CComBSTR(carr_id));
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendValueU1(lHandle[2], 0);

	/* Pause Event */
	/*   Identifier of a list of events on which the Control Job shall PAUSE */
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendList(0, &lHandle[0]);
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendValueAscii(lHandle[0], CComBSTR(L"PauseEvent"));
	if (S_OK == hResult)	hResult	= valAttrPtr->AppendList(lHandle[0], &lHandle[1]);
#endif
	/* Create */
	if (S_OK == hResult)	hResult	= m_pICxE94CJMPtr->CreateControlJob(valAttrPtr, valStatusPtr);

	/* 에러 여부 확인 */
	/* Returned status (PRStatus), L {Bo ACKA} {L {L {U4 ERRCODE} {A ERRTEXT}}...} */
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		DispErrorData(valStatusPtr, E30_SVET::en_acka);
		return FALSE;
	}

	return (varACKA == VARIANT_TRUE && hResult == S_OK);
}

/*
 desc : Control Job을 선택하기
 parm : cj_id		- [in]  Control Job ID
 retn : TRUE or FALSE
*/
BOOL CE94CJM::SetSelectedControlJob(PTCHAR cj_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	HRESULT hResult		= S_OK;
	CComBSTR bstrCtrlID (cj_id);
	ICxE94ControlJobPtr ptrCtrlJob;
	ICxValueDispPtr ptrErrInfo (CLSID_CxValueObject);

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetSelectedControlJob::Control Job ID : [%s]", cj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE94CJMPtr->CJGetJob(bstrCtrlID, &ptrCtrlJob);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	if (!ptrCtrlJob)	return FALSE;

	/* Control Job Class */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetSelectedControlJob::Control Job ID : [%s]", cj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= ptrCtrlJob->Select(ptrErrInfo);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Control Job을 선택 해제하기
 parm : cj_id		- [in]  Control Job ID
 retn : TRUE or FALSE
*/
BOOL CE94CJM::SetDeselectedControlJob(PTCHAR cj_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	HRESULT hResult		= S_OK;
	CComBSTR bstrCtrlID (cj_id);
	ICxValueDispPtr ptrErrInfo (CLSID_CxValueObject);

	/* Control Job Class */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetDeselectedControlJob::Control Job ID : [%s]", cj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE94CJMPtr->CJDeselect(&bstrCtrlID, ptrErrInfo);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Remote Control 허용 여부 값 반환
 parm : enable	- [out] Enable or Disable
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
BOOL CE94CJM::GetAllowRemoteControl(BOOL &enable)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult			= S_OK;
	VARIANT_BOOL boEnable	= VARIANT_FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetAllowRemoteControl::get_AllowRemoteControl");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE94CJMPtr->get_AllowRemoteControl(&boEnable);
	m_pLogData->CheckResult(hResult, tzMesg);
	/* 값 반환 */
	enable	= (boEnable == VARIANT_TRUE) ? TRUE : FALSE;

	return (hResult == S_OK);
}

/*
 desc : Remote Control 허용 여부 설정
 parm : enable	- [in]  Enable or Disable
 retn : TRUE or FALSE
*/
BOOL CE94CJM::SetAllowRemoteControl(BOOL enable)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	VARIANT_BOOL boEnable = enable == TRUE ? VARIANT_TRUE : VARIANT_FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetAllowRemoteControl::put_AllowRemoteControl : enable = %d", enable);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE94CJMPtr->put_AllowRemoteControl(boEnable);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/* --------------------------------------------------------------------------------------------- */
/*                                         Cimetrix Area                                         */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Control Job ID에 등록되어 있는 Carrier ID List 반환
 parm : cj_id		- [in]  Control Job ID
		list_carr	- [out] Carrier List가 반환될 버퍼
 retn : TRUE or FALSE
*/
BOOL CE94CJM::GetControlJobCarrierInputSpec(PTCHAR cj_id, CStringList &list_carr)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	LONG lCount		= 0, lIndex = 1;
	HRESULT hResult	= S_OK;
	CComBSTR bstrVal;
	ICxE94ControlJobPtr pICxE94CtrlJob;
	ICxE39ObjPtr pICxE39Obj;
	ICxValueDispPtr pICxCarrier(CLSID_CxValueObject);

	/* 기존 등록된 것 모두 삭제 */
	list_carr.RemoveAll();

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobCarrierInputSpec::CJGetJob : cj_id=%s", cj_id);
	hResult	= m_pICxE94CJMPtr->CJGetJob(CComBSTR(cj_id), &pICxE94CtrlJob);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	pICxE39Obj	= (ICxE39ObjPtr)pICxE94CtrlJob;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobCarrierInputSpec::GetAttr : CarrierInputSpec");
	hResult	= pICxE39Obj->GetAttr(CComBSTR(L"CarrierInputSpec"), pICxCarrier);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobCarrierInputSpec::ItemCount");
	hResult	= pICxCarrier->ItemCount(0, &lCount);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	for (; lIndex<=lCount; lIndex++)
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobCarrierInputSpec::ListAt : lIndex=%d", lIndex);
		hResult	= pICxCarrier->GetValueAscii(0, lIndex, &bstrVal);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
		list_carr.AddTail(CString(bstrVal));
	}

	return TRUE;
}

/*
desc :
parm : cj_id		- [in]  Control Job ID
		list_carr	- [out] Carrier List 반환될 버퍼
retn : TRUE or FALSE
*/
BOOL CE94CJM::GetControlJobCarrierOutputSpec(PTCHAR cj_id, CStringList &list_carr)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = { NULL };
	LONG lCount = 0, lIndex = 1;
	HRESULT hResult = S_OK;
	ICxE94ControlJobPtr pICxE94CtrlJob;
	ICxE39ObjPtr pICxE39Obj;

	/* 기존 등록된 것 모두 삭제 */
	list_carr.RemoveAll();

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobCarrierOutputSpec::CJGetJob : cj_id=%s", cj_id);
	hResult = m_pICxE94CJMPtr->CJGetJob(CComBSTR(cj_id), &pICxE94CtrlJob);
	if (m_pLogData->CheckResult(hResult, tzMesg))	
		return FALSE;

	pICxE39Obj = (ICxE39ObjPtr)pICxE94CtrlJob;

	ICxValueDispPtr mtrlOutSpecVal(CLSID_CxValueObject);
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobCarrierOutputSpec::GetAttr : MtrlOutSpec");
	hResult = pICxE39Obj->GetAttr(CComBSTR(L"MtrlOutSpec"), mtrlOutSpecVal);
	if (m_pLogData->CheckResult(hResult, tzMesg))	
		return FALSE;

	LONG count_MapSet = 0;
	mtrlOutSpecVal->ItemCount(0, &count_MapSet);
	for (int mapIndex = 1; mapIndex <= count_MapSet; mapIndex++)
	{
		// map header
		LONG h_root = 0;
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobCarrierOutputSpec::ListAt : MapRoot");
		hResult = mtrlOutSpecVal->ListAt(0, mapIndex, &h_root);
		if (m_pLogData->CheckResult(hResult, tzMesg))	
			return FALSE;

		// destination map
		LONG h_destination = 0;
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobCarrierOutputSpec::ListAt : DestinationMap");
		hResult = mtrlOutSpecVal->ListAt(h_root, 2, &h_destination);
		if (m_pLogData->CheckResult(hResult, tzMesg))	
			return FALSE;

		// destination carrier id
		CComBSTR d_carrierId;
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobCarrierOutputSpec::GetValueAscii : 1");
		hResult = mtrlOutSpecVal->GetValueAscii(h_destination, 1, &d_carrierId);
		if (m_pLogData->CheckResult(hResult, tzMesg))	
			return FALSE;

		if (d_carrierId)	list_carr.AddTail(d_carrierId);
	}

	return TRUE;
}

/*
 desc : Control Job ID에 포함된 Process Job ID List 반환
 parm : cj_id	- [in]  Control Job ID
		list_pj	- [out] Process Job List 반환될 버퍼
 retn : TRUE or FALSE
*/
BOOL CE94CJM::GetControlJobProcessJobList(PTCHAR cj_id, CStringList &list_pj)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	LONG lCount		= 0, lIndex = 1, lHandle;
	HRESULT hResult	= S_OK;
	CComBSTR bstrVal;
	ICxE94ControlJobPtr pICxE94CtrlJob;
	ICxValueDispPtr pICxPJList(CLSID_CxValueObject);
	ICxE39ObjPtr pICxE390CJM;

	/* 기존 등록된 것 모두 삭제 */
	list_pj.RemoveAll();

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobProcessJobList::CJGetJob : cj_id=%s", cj_id);
	hResult	= m_pICxE94CJMPtr->CJGetJob(CComBSTR(cj_id), &pICxE94CtrlJob);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	pICxE390CJM = (ICxE39ObjPtr)pICxE94CtrlJob;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobProcessJobList::GetAttr : cj_id=%s", cj_id);
	hResult	= pICxE390CJM->GetAttr(CComBSTR(L"PRJobStatusList"), pICxPJList);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobProcessJobList::ItemCount");
	hResult	= pICxPJList->ItemCount(0, &lCount);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	for (; lIndex<=lCount; lIndex++)
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobProcessJobList::ListAt : lIndex=%d", lIndex);
		hResult	= pICxPJList->ListAt(0, lIndex, &lHandle);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobProcessJobList::GetValueAscii : lHandle=%d", lHandle);
		hResult	= pICxPJList->GetValueAscii(lHandle, 1, &bstrVal);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		list_pj.AddTail(CString(bstrVal));
	}

	return TRUE;
}

/*
 desc : Control Job의 자동 시작 여부 확인
 parm : cj_id		- [in]  Control Job ID
		auto_start	- [out] Auto Start 여부 값 반환
 retn : TRUE or FALSE
*/
BOOL CE94CJM::GetControlJobStartMethod(PTCHAR cj_id, BOOL &auto_start)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	LONG lCount		= 0;
	HRESULT hResult	= S_OK;
	VARIANT_BOOL vbAutoStart;
	ICxE94ControlJobPtr pICxE94CtrlJob;
	ICxE39ObjPtr pICxE39Obj;
	ICxValueDispPtr pICxValue(CLSID_CxValueObject);

	/* Control Job Object 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobStartMethod::CJGetJob : cj_id=%s", cj_id);
	hResult	= m_pICxE94CJMPtr->CJGetJob(CComBSTR(cj_id), &pICxE94CtrlJob);
	pICxE39Obj	= (ICxE39ObjPtr)pICxE94CtrlJob;
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* StartMethod 객체 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobStartMethod::GetAttr : cj_id=%s", cj_id);
	hResult	= pICxE39Obj->GetAttr(CComBSTR(L"StartMethod"), pICxValue);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobStartMethod::ItemCount : cj_id=%s", cj_id);
	hResult	= pICxValue->ItemCount(0, &lCount);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	if (lCount < 1)
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobStartMethod::ItemCount : lCount=%d", lCount);
		m_pLogData->PushLogs(tzMesg, ENG_GALC::en_warning);
		return FALSE;
	}

	/* 0 == FALSE, -1 == TRUE */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobStartMethod::GetValueBoolean : cj_id=%s", cj_id);
	hResult	= pICxValue->GetValueBoolean(0, 0, &vbAutoStart);
	/* 값 반환  */
	auto_start = vbAutoStart == VARIANT_TRUE ? TRUE : FALSE;

	return TRUE;
}

/*
 desc : Control Job의 작업 완료 시간 얻기
 parm : cj_id		- [in]  Control Job ID
		date_time	- [out] 작업 완료 시간 문자열로 반환
		size		- [in]  'data_time' 버퍼 크기
 retn : TRUE or FALSE
*/
BOOL CE94CJM::GetControlJobCompletedTime(PTCHAR cj_id, PTCHAR date_time, UINT32 size)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult = S_OK;
	DATE dtDateTime	= 0;
	COleDateTime oleDateTime;
	ICxE94ControlJobPtr ptrCtrlJob;

	/* Control Job Object 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobCompletedTime::CJGetJob : cj_id=%s", cj_id);
	hResult = m_pICxE94CJMPtr->CJGetJob(CComBSTR(cj_id), &ptrCtrlJob);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetControlJobCompletedTime::get_CompletedTime");
	hResult = ptrCtrlJob->get_CompletedTime(&dtDateTime);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* MSDN DATE Type (https://docs.microsoft.com/en-us/cpp/atl-mfc-shared/date-type?view=vs-2019) */
	oleDateTime	= COleDateTime(dtDateTime);
	/* 시간 포맷 문자열로 반환 */
	swprintf_s(date_time, size, L"%4d-%02d-%02d %02d:%02d:%02d",
			   oleDateTime.GetYear(), oleDateTime.GetMonth(), oleDateTime.GetDay(),
			   oleDateTime.GetHour(), oleDateTime.GetMinute(), oleDateTime.GetSecond());

	return (hResult == S_OK);
}

/*
 desc : Control Job의 Material 도착 여부 얻기
 parm : cj_id	- [in]  Control Job ID
		arrived	- [out] TRUE : 도착, FALSE : 미도착
 retn : TRUE or FALSE
 note : CarrierInputSpec 속성에서 모든 Carrier들이 장비에 도착 했을 때 TRUE 반환
*/
BOOL CE94CJM::GetControlJobMaterialArrived(PTCHAR cj_id, BOOL &arrived)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult = S_OK;
	VARIANT_BOOL vbArrived;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobMaterialArrived::get_MaterialArrived : cj_id=%s", cj_id);
	hResult = m_pICxE94CJMPtr->get_MaterialArrived(CComBSTR(cj_id), &vbArrived);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	arrived	= (vbArrived == VARIANT_TRUE) ? TRUE : FALSE;

	return (hResult == S_OK);
}

/*
 desc : Control Job의 시작 설정
 parm : cj_id	- [in]  Control Job ID
 retn : TRUE or FALSE
*/
BOOL CE94CJM::SetStartControlJob(PTCHAR cj_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult = S_OK;
	CComBSTR bstrCtrlID(cj_id);
	ICxValueDispPtr pICxErrInfo(CLSID_CxValueObject);

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetStartControlJob::CJStart : cj_id=%s", cj_id);
	m_pLogData->PushLogs(tzMesg);
	/* Control Job Class */
	hResult = m_pICxE94CJMPtr->CJStart(&bstrCtrlID, pICxErrInfo);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Control Job의 중지 (작업이 완료되었거나 실패 했거나 등 일 경우, 중지 시킴) 알림
 parm : cj_id	- [in]  Control Job ID
		action	- [in]  작업 저장 혹은 제거 여부
 retn : TRUE or FALSE
*/
BOOL CE94CJM::SetStopControlJob(PTCHAR cj_id, E94_CCJA action)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;
	CComBSTR bstrCtrlID(cj_id);
	ICxValueDispPtr pICxErrInfo(CLSID_CxValueObject);

	/* Control Job Class */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetStopControlJob::CJStop : cj_id=%s", cj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE94CJMPtr->CJStop(&bstrCtrlID, E94CJAction(action), pICxErrInfo);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Control Job의 중단 (작업 시작 중에 작업을 아예 중지 시켜버림) 알림
 parm : cj_id	- [in]  Control Job ID
		action	- [in]  작업 저장 혹은 제거 여부
 retn : TRUE or FALSE
*/
BOOL CE94CJM::SetAbortControlJob(PTCHAR cj_id, E94_CCJA action)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult = S_OK;
	CComBSTR bstrCtrlID(cj_id);
	ICxValueDispPtr pICxErrInfo(CLSID_CxValueObject);

	/* Control Job Class */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetAbortControlJob::CJAbort : cj_id=%s", cj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE94CJMPtr->CJAbort(&bstrCtrlID, E94CJAction(action), pICxErrInfo);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Control Job의 잠시 중지 (일시 중지. 취소 / 중단 등 아님) 알림
 parm : cj_id	- [in]  Control Job ID
 retn : TRUE or FALSE
*/
BOOL CE94CJM::SetPauseControlJob(PTCHAR cj_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult = S_OK;
	CComBSTR bstrCtrlID(cj_id);
	ICxValueDispPtr pICxErrInfo(CLSID_CxValueObject);

	/* Control Job Class */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPauseControlJob::CJPause : cj_id=%s", cj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE94CJMPtr->CJPause(&bstrCtrlID, pICxErrInfo);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Control Job의 재시작 (중지 시점부터) 알림
 parm : cj_id	- [in]  Control Job ID
 retn : TRUE or FALSE
*/
BOOL CE94CJM::SetResumeControlJob(PTCHAR cj_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult = S_OK;
	CComBSTR bstrCtrlID(cj_id);
	ICxValueDispPtr pICxErrInfo(CLSID_CxValueObject);

	/* Control Job Class */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetResumeControlJob::CJResume : cj_id=%s", cj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE94CJMPtr->CJResume(&bstrCtrlID, pICxErrInfo);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Control Job의 취소 (시작하지 않은 상태에서... 아예 작업을 하지 않음) 알림
 parm : cj_id	- [in]  Control Job ID
		action	- [in]  작업 저장 혹은 제거 여부
 retn : TRUE or FALSE
*/
BOOL CE94CJM::SetCancelControlJob(PTCHAR cj_id, E94_CCJA action)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult = S_OK;
	CComBSTR bstrCtrlID(cj_id);
	ICxValueDispPtr pICxErrInfo(CLSID_CxValueObject);

	/* Control Job Class */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetCancelControlJob::CJCancel : cj_id=%s", cj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE94CJMPtr->CJCancel(&bstrCtrlID, (E94CJAction)action, pICxErrInfo);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Control Job을 Queue의 맨 상위에 이동 시킴을 알림
 parm : cj_id	- [in]  Control Job ID
 retn : TRUE or FALSE
 note : Moves the specified control job to the top of the queue.
		Other control jobs in the queue are pushed back.
		The rest of the queue order remains unchanged.
*/
BOOL CE94CJM::SetHOQControlJob(PTCHAR cj_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult = S_OK;
	CComBSTR bstrCtrlID(cj_id);
	ICxValueDispPtr pICxErrInfo(CLSID_CxValueObject);

	/* Control Job Class */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetHOQControlJob::CJHOQ : cj_id=%s", cj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE94CJMPtr->CJHOQ(&bstrCtrlID, pICxErrInfo);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Control Job을 Queue의 맨 상위에 이동 시킴을 알림
 parm : cj_id	- [in]  Control Job ID
 retn : TRUE or FALSE
 note : Moves the specified control job to the top of the queue.
		Other control jobs in the queue are pushed back.
		The rest of the queue order remains unchanged.
*/
BOOL CE94CJM::CJGetJob(PTCHAR ctrl_id, ICxE94ControlJobPtr cj)
{
	TCHAR tzMesg[1024] = { NULL };
	HRESULT hResult = S_OK;
	CComBSTR bstrCtrlID(ctrl_id);

	ICxE94ControlJobPtr ptrCtrlJob;

	/* Control Job Class */
	swprintf_s(tzMesg, 1024, L"CJGetJob::Control Job ID : [%s]", ctrl_id);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE94CJMPtr->CJGetJob(bstrCtrlID, &ptrCtrlJob);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	if (!ptrCtrlJob)	return FALSE;

	cj = ptrCtrlJob;

	return (hResult == S_OK);
}

/*
desc : Control Job 내부에서 다음 ProcessJob을 시작시킨다.
parm : cj_id	- [in]  Control Job ID
retn : TRUE or FALSE
*/
BOOL CE94CJM::StartNextProcessJob(PTCHAR cj_id)
{
	HRESULT hResult = S_OK;
	CComBSTR id(cj_id);
	TCHAR tzMesg[1024] = { NULL };
	swprintf_s(tzMesg, 1024, L"m_pICxE94CJMPtr::StartNextProcessJob : [%s]", cj_id);
	hResult = m_pICxE94CJMPtr->StartNextProcessJob(id);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
desc : 다음 Control Job을 시작시킨다.
parm : cj_id	- [in]  Control Job ID
retn : TRUE or FALSE
*/
BOOL CE94CJM::StartNextControlJob()
{
	HRESULT hResult = S_OK;
	TCHAR tzMesg[1024] = { NULL };
	swprintf_s(tzMesg, 1024, L"m_pICxE94CJMPtr::StartNextControlJob");
	hResult = m_pICxE94CJMPtr->StartNextControlJob();
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}
