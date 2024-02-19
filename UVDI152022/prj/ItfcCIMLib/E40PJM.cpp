
#include "pch.h"
#include "MainApp.h"
#include "E40PJM.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 desc : ������
 parm : logs	- [in]  Log Object Pointer
		share	- [in]  Shared Data Object Pointer
 retn : None
*/
CE40PJM::CE40PJM(CLogData *logs, CSharedData *share)
	: CEBase(logs, share)
{
	/* GUID ���� */
	m_gdICxE	= IID_ICxE40PJM;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CE40PJM::~CE40PJM()
{
	Close();

}

/*
 desc : �ʱ�ȭ (�ݵ�� ȣ��)
 parm : equip_id	- [in]  Equipment ID (0 or later. ȯ�� ���Ͽ� ������ ����-1)
		conn_id		- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
 retn : TRUE or FALSE
*/
BOOL CE40PJM::Init(LONG equip_id, LONG conn_id)
{
	if (!CEBase::Init(equip_id, conn_id))	return FALSE;

	/* �Ʒ� ������ �ݵ�� ��ų �� */
	if (!InitE40PJM())	return FALSE;

	return TRUE;
}

/*
 desc : ����
 parm : None
 retn : None
*/
VOID CE40PJM::Close()
{
	/* �Ʒ� ������ �ݵ�� ��ų �� */
	CloseE40PJM();

	CEBase::Close();
}

/*
 desc : �α� ���� �� ����
 parm : level	- [in]  �α� ���� ��
 retn : TRUE or FALSE
*/
BOOL CE40PJM::SetLogLevel(ENG_ILLV level)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	if (m_pICxE40PJMPtr && m_pICxE40PJMPtr.GetInterfacePtr())
	{
		swprintf_s(tzMesg, 512, L"SetLogLevel::CIM40LogLevel level : [%d]", level);
		m_pLogData->PushLogs(tzMesg);
		hResult	= m_pICxE40PJMPtr->put_CIM40LogLevel(LONG(level));
		m_pLogData->CheckResult(hResult, tzMesg);
	}
	return (hResult == S_OK);
}

/*
 desc : �ʱ�ȭ (�ݵ�� ȣ��)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE40PJM::InitE40PJM()
{
	HRESULT hResult	= S_FALSE;

	IUnknownPtr pIUnknownICxE87CMS;
	ICxE40PJMCallbackPtr pICxE40PJMCallback;
	ICxE40PJMCallback2Ptr pICxE40PJMCallback2;

	pICxE40PJMCallback	= m_pICxEMAppCbk;
	pIUnknownICxE87CMS	= m_pICxE87CMSPtr;

	if (m_pICxE40PJMPtr && m_pICxE40PJMPtr.GetInterfacePtr())		return FALSE;

	/* Create & Initialize */
	m_pLogData->PushLogs(L"ICxE40PJM::CreateInstance");
	hResult = m_pICxE40PJMPtr.CreateInstance(CLSID_CxE40PJM);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE40PJM::CreateInstance CLSID_CxE40PJM")))
		return FALSE;
	m_pLogData->PushLogs(L"ICxE40PJM::Initialize2");
	hResult = m_pICxE40PJMPtr->Initialize2(m_pICxE39ObjPtr, m_pICxAbsLayerPtr, pICxE40PJMCallback,
										   CIM_E40_JOB_QUE_SIZE, pIUnknownICxE87CMS);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE40PJM::Initialize2")))	return FALSE;

	/* Register callabck function */
	pICxE40PJMCallback2 = m_pICxEMAppCbk;
	hResult = m_pICxE40PJMPtr->RegisterCallback2(pICxE40PJMCallback2);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE40PJM::RegisterCallback2")))	return FALSE;

	/* Register the global interface */
	m_pLogData->PushLogs(L"RegisterInterfaceInGlobal", m_tzGUID);
	hResult	= m_pGIT->RegisterInterfaceInGlobal(m_pICxE40PJMPtr, m_gdICxE, &m_dwCookie);
	if (FAILED(m_pLogData->CheckResult(hResult, L"RegisterInterfaceInGlobal")))	return FALSE;
	m_pSharedData->GetGITCookie()->e40pjm_cookie	= m_dwCookie;

	return (hResult == S_OK);
}

/*
 desc : ����
 parm : None
 retn : None
*/
VOID CE40PJM::CloseE40PJM()
{
	HRESULT hResult	= S_FALSE;

	if (m_pICxE40PJMPtr && m_pICxE40PJMPtr.GetInterfacePtr())
	{
		m_pLogData->PushLogs(L"ICxE40PJM::UnregisterCallback2");
		hResult = m_pICxE40PJMPtr->UnregisterCallback2();
		m_pLogData->CheckResult(hResult, L"ICxE40PJM::UnregisterCallback2");
		m_pLogData->PushLogs(L"ICxE40PJM::Shutdown");
		hResult = m_pICxE40PJMPtr->Shutdown();
		m_pLogData->CheckResult(hResult, L"ICxE40PJM::Shutdown");
		m_pLogData->PushLogs(L"ICxE40PJM::Release");
		m_pICxE40PJMPtr.Release();
	}
}

/*
 desc : Process Job�� SETTING UP ���¿��� WAITING FOR START ���� PROCESSING ���·� �̵�
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
 note : Moves Process Job from SETTING UP state to WAITING FOR START or PROCESSING state
		���簡 �غ�ǰ� Process Job�� ������ �غ� �Ǿ� �ִٰ� �˸�
*/
BOOL CE40PJM::SetPRJobStartProcess(PTCHAR pj_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	E40_PPJS enState;
	CComBSTR bstrProcID (pj_id);

	/* Waiting for start ���� ���� Ȯ�� */
	if (!GetProcessJobState(pj_id, enState))	return FALSE;
	if (enState != E40_PPJS::en_setting_up)
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRJobStartProcess:: pj_id = %s, "
											  L"Process Job State Code=%d", pj_id, enState);
		m_pLogData->PushLogs(tzMesg);
		return TRUE;
	}

	/* Start Process */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRJobStartProcess::PRJobStartProcess  pj_id = %s, "
										  L"Process Job State Code=%d", pj_id, enState);
	m_pLogData->PushLogs(tzMesg);
	/* ���� ������ �߻��ϸ� Callback �Լ��� ȣ�� (?) */
	hResult = m_pICxE40PJMPtr->PRJobStartProcess(bstrProcID);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Process Job�� ���� Control Job ID ��ȯ
 parm : pj_id	- [in]  Object ID of the Process Job
		cj_id	- [out] Object ID of the Control Job
		size	- [in]  'cj_id' ���� ũ��
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
BOOL CE40PJM::GetControlJobID(PTCHAR pj_id, PTCHAR cj_id, UINT32 size)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	TCHAR tzCtrlID[CIM_CONTROL_JOB_ID_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrProcID (pj_id), bstrCtrlJobID;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobID::get_ParentControlJob : pj_id=%s", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE40PJMPtr->get_ParentControlJob(bstrProcID, &bstrCtrlJobID);
	m_pLogData->CheckResult(hResult, tzMesg);
	if (hResult == S_OK)
	{
		uvCmn_CComBSTR2TCHAR(bstrCtrlJobID, tzCtrlID, CIM_CONTROL_JOB_ID_SIZE);
		memset(cj_id, 0x00, size);
		wcscpy_s(cj_id, size, tzCtrlID);
	}

	return (hResult == S_OK);
}

/*
 desc : Process Job�� ���� Control Job ID ����
 parm : pj_id	- [in]  Object ID of the Process Job
		cj_id	- [in]  Object ID of the Control Job
 retn : TRUE or FALSE
*/
BOOL CE40PJM::SetControlJobID(PTCHAR pj_id, PTCHAR cj_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrProcID (pj_id), bstrCtrlJobID (cj_id);

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetControlJobID::put_ParentControlJob : pj_id=%s, cj_id=%s", pj_id, cj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE40PJMPtr->put_ParentControlJob(bstrProcID, bstrCtrlJobID);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Process Job �۾� �Ϸ� (Only PRJobComplete)
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
 note : Moves Process Job from the QUEUED, PROCESS COMPLETE, STOPPING/STOPPED or
		ABORTING/ABORTED state to the JOB COMPLETE state and deletes the job
*/
BOOL CE40PJM::SetPRJobCompleteOnly(PTCHAR pj_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrProcID, bstrCtrlID;
	ICxValueDispPtr pICxStatusPtr (CLSID_CxValueObject);

	/* Process Job ID ���� */
	bstrProcID	= CComBSTR(pj_id);

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRJobCompleteOnly::PRJobComplete process_id [%s]", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE40PJMPtr->PRJobComplete(bstrProcID, pICxStatusPtr);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Process Job �۾� �Ϸ� (PRJobComplete -> get_ParentControlJob -> ProcessJobComplete)
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
 note : Moves Process Job from the QUEUED, PROCESS COMPLETE, STOPPING/STOPPED or
		ABORTING/ABORTED state to the JOB COMPLETE state and deletes the job
*/
BOOL CE40PJM::SetPRJobComplete(PTCHAR pj_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrProcID, bstrCtrlID;
	ICxValueDispPtr pICxStatusPtr (CLSID_CxValueObject);

	/* Process Job ID ���� */
	bstrProcID	= CComBSTR(pj_id);

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRJobComplete::PRJobComplete process_id [%s]", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE40PJMPtr->PRJobComplete(bstrProcID, pICxStatusPtr);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Process in the chamber (���� Ȯ��)
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
 note : Moves Process Job from PROCESSING state to PROCESS COMPLETE state.
*/
BOOL CE40PJM::SetPRJobProcessComplete(PTCHAR pj_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	ICxValueDispPtr pICxStatusPtr (CLSID_CxValueObject);

	/* Process ID ���� */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRJobProcessComplete::PRJobProcessComplete process_id [%s]", pj_id);
	m_pLogData->PushLogs(tzMesg);
	/* ���� ������ �߻��ϸ� Callback �Լ��� ȣ�� (?) */
	hResult = m_pICxE40PJMPtr->PRJobProcessComplete(CComBSTR(pj_id), pICxStatusPtr);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Process Job�� Queue ���¿��� Completed ���·� �̵� (����)�ϸ� Project Job Id�� ����
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
 note : SetProcessJobRemove �Լ��� �ٸ��� ? Queue���� �����ϴ� ����, Job List���� �����ϴ� ���� �ƴ� 
*/
BOOL CE40PJM::SetProcessJobDequeue(PTCHAR pj_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	LONG l = 0, lStatus	= 0, lHandle = 0;
	HRESULT hResult		= S_OK;
	VARIANT_BOOL varBool= VARIANT_TRUE;
	ICxValueDispPtr pICxProcIDPtr(CLSID_CxValueObject);
	ICxValueDispPtr pICxStatusPtr(CLSID_CxValueObject);

	/* List of Object IDs of the Process Jobs to cancel. Use an empty list for all QUEUED Process Jobs */
	pICxProcIDPtr->AppendList(0, &lHandle);
	pICxProcIDPtr->AppendValueAscii(lHandle, CComBSTR(pj_id));

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetProcessJobDequeue::PRJobDequeue");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRJobDequeue(pICxProcIDPtr, pICxStatusPtr);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* ���� ���� Ȯ�� */
	/* Returned status (PRStatus), L {Bo ACKA} {L {L {U4 ERRCODE} {A ERRTEXT}}...} */
	hResult	= pICxStatusPtr->ItemCount(0, &lStatus);
	if (m_pLogData->CheckResult(hResult, L"SetProcessJobDequeue::ItemCount"))	return FALSE;
	for (l=1; l<=lStatus; l++)
	{
		/* Boolean : ACKA */
		hResult	= pICxStatusPtr->ListAt(0, l, &lHandle);
		if (m_pLogData->CheckResult(hResult, L"SetProcessJobDequeue::ListAt(0, l, &lHandle)"))	return FALSE;
		hResult	= pICxStatusPtr->GetValueBoolean(lHandle, 1, &varBool);
		if (m_pLogData->CheckResult(hResult, L"SetProcessJobDequeue::GetValueBoolean(lHandle, 1, &varBool)"))	return FALSE;
	}

	return (varBool == VARIANT_TRUE && hResult == S_OK);
}

/*
 desc : Process Job�� Stopping ���¿��� Stopped ���·� �̵� (����)
 parm : pj_id	- [in]  Objet ID of the Process Job
 retn : TRUE or FALSE
*/
BOOL CE40PJM::SetProcessJobStoppingToStopped(PTCHAR pj_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	LONG l, lStatus		= 0, lHandle;
	HRESULT hResult		= S_OK;
	VARIANT_BOOL varACKA= VARIANT_TRUE;
	CComBSTR bstrProcID (pj_id);
	ICxValueDispPtr pICxStatusPtr(CLSID_CxValueObject);

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetProcessJobStoppingToStopped::PRJobStopComplete");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRJobStopComplete(bstrProcID, pICxStatusPtr);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* ���� ���� Ȯ�� */
	/* Returned status (PRStatus), L {Bo ACKA} {L {L {U4 ERRCODE} {A ERRTEXT}}...} */
	hResult	= pICxStatusPtr->ItemCount(0, &lStatus);
	if (m_pLogData->CheckResult(hResult, L"SetProcessJobStoppingToStopped::valStatusPtr->ItemCount"))	return FALSE;
	for (l=1; l<=lStatus; l++)
	{
		/* Boolean : ACKA */
		hResult	= pICxStatusPtr->ListAt(0, l, &lHandle);
		if (m_pLogData->CheckResult(hResult, L"SetProcessJobStoppingToStopped::valStatusPtr->ListAt(0, l, &lHandle)"))	return FALSE;
		hResult	= pICxStatusPtr->GetValueBoolean(lHandle, 1, &varACKA);
		if (m_pLogData->CheckResult(hResult, L"SetProcessJobStoppingToStopped::valStatusPtr->GetValueBoolean(lHandle, 1, &varACKA)"))	return FALSE;
	}

	return (varACKA == VARIANT_TRUE && hResult == S_OK);
}

/*
 desc : Process Job�� ABORTING ���¿��� ABORTED ���·� �̵�
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
 note : Moves Process Job from ABORTING state to the ABORTED state.
		�۾� ��Ұ� �Ϸ�Ǿ��� ��, �� �Լ��� ȣ���� �ָ� ��
*/
BOOL CE40PJM::SetPRJobAbortComplete(PTCHAR pj_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	LONG l, lStatus		= 0, lHandle;
	HRESULT hResult		= S_OK;
	VARIANT_BOOL varBool= VARIANT_TRUE;
	ICxValueDispPtr pICxStatusPtr (CLSID_CxValueObject);

	/* Process ID ���� */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRJobAbortComplete::PRJobAbortComplete process_id [%s]", pj_id);
	m_pLogData->PushLogs(tzMesg);
	/* ���� ������ �߻��ϸ� Callback �Լ��� ȣ�� (?) */
	hResult = m_pICxE40PJMPtr->PRJobAbortComplete(CComBSTR(pj_id), pICxStatusPtr);
	m_pLogData->CheckResult(hResult, tzMesg);

	/* ������ �ƴ� ���, ��ȯ�� ���� �� �α׿� ���� */
	/* Returned status (PRStatus), L {Bo ACKA} {L {L {U4 ERRCODE} {A ERRTEXT}}...} */
	hResult	= pICxStatusPtr->ItemCount(0, &lStatus);
	if (m_pLogData->CheckResult(hResult, L"SetPRJobAbortComplete::valStatusPtr->ItemCount"))	return FALSE;
	for (l=1; l<=lStatus; l++)
	{
		/* Boolean : ACKA */
		hResult	= pICxStatusPtr->ListAt(0, l, &lHandle);
		if (m_pLogData->CheckResult(hResult, L"SetPRJobAbortComplete::valStatusPtr->ListAt(0, l, &lHandle)"))	return FALSE;
		hResult	= pICxStatusPtr->GetValueBoolean(lHandle, 1, &varBool);
		if (m_pLogData->CheckResult(hResult, L"SetPRJobAbortComplete::valStatusPtr->GetValueBoolean(lHandle, 1, &varBool)"))	return FALSE;
	}

	return (varBool == VARIANT_TRUE && hResult == S_OK);
}

/*
 desc : Process ID�� �ش�Ǵ� ������ ���� ����(Carrier ID 1 ~ XXX [slot list])
 parm : pj_id	- [in]  Object ID of the Process Job
		carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		slot_no	- [out] Slot Number�� ����� ����Ʈ ��ȯ
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
BOOL CE40PJM::GetPRJobProcessSlotID(PTCHAR pj_id, PTCHAR carr_id, CAtlList <UINT8> &slot_no)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	TCHAR tzCarrID[CIM_CARRIER_ID_SIZE]	= {NULL};
	UINT8 u8SlotNum	= 0;
	LONG lCarrCount	= 0, lSlotCount, lCarrIndex, lSlotIndex;
	LONG lHandleCarr, lHandleSlot;
	HRESULT hResult	= S_OK;
	ICxProcessJobPtr pICxProcJob;
	MaterialTypeEnum matType;
	PCHAR pszCarrID	= NULL;
	CComBSTR bstrCarrID;
	ICxValueDispPtr pICxMtrlList(CLSID_CxValueObject);

	/* ���� ��ϵ� �� ��� ���� */
	slot_no.RemoveAll();

	/* Get the process job from its ID */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRJobProcessSlotID::PRGetJob process_id : [%s]", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRGetJob(CComBSTR(pj_id), &pICxProcJob);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Get the material type format (CARRIER or SUBSTRATE) */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRJobProcessSlotID::get_PRMtlType process_id : [%s]", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxProcJob->get_PRMtlType(&matType);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* Carrier Type�� �ƴϸ� ���� ó�� */
	if (MaterialTypeEnum::pjCARRIER != matType)
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRJobProcessSlotID::get_PRMtlType (Not carrier type) process_id / type : [%s] / [%d]", pj_id, matType);
		m_pLogData->PushLogs(tzMesg);
		return FALSE;
	}

	/* Get the list of material that should be processed */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRJobProcessSlotID::get_PRMtlNameList process_id : [%s]", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxProcJob->get_PRMtlNameList(&pICxMtrlList);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Get the number of material (carriers or substrates, depending on material type format) */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRJobProcessSlotID::MaterialCount process_id / handle : [%s] / [0]", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxMtrlList->ItemCount(0, &lCarrCount);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Carrier Count ��ŭ ���� ���鼭, ���ϴ� Carrier ID�� �ִ��� Ȯ�� */
	for (lCarrIndex=1; lCarrIndex<=lCarrCount && slot_no.GetCount() < 1; lCarrIndex++)
	{
		/* Get a handle to the carrier/slot information */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRJobProcessSlotID::ListAt (carrier slot list handle) process_id / lHandle0 : [%s] / [0]", pj_id);
		m_pLogData->PushLogs(tzMesg);
		hResult	= pICxMtrlList->ListAt(0, lCarrIndex, &lHandleCarr);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		/* Get the Carrier ID */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRJobProcessSlotID::GetValueAscii (carrier ID) process_id / lHandleCarr : [%s] / [%d]", pj_id, lHandleCarr);
		m_pLogData->PushLogs(tzMesg);
		hResult	= pICxMtrlList->GetValueAscii(lHandleCarr, 1, &bstrCarrID);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		/* �˻� ����� Carrier ID���� Ȯ�� */
		wmemset(tzCarrID, 0x00, CIM_CARRIER_ID_SIZE);
		if (uvCmn_CComBSTR2TCHAR(bstrCarrID, tzCarrID, CIM_CARRIER_ID_SIZE))
		{
			if (0 == wcscmp(tzCarrID, carr_id))
			{
				/* Get a handle to the list of slot # */
				swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRJobProcessSlotID::ListAt (slot list handle) process_id / lHandleCarr : [%s] / [%d]", pj_id, lHandleCarr);
				m_pLogData->PushLogs(tzMesg);
				hResult	= pICxMtrlList->ListAt(lHandleCarr, 2, &lHandleSlot);
				if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

				/* Get the number of slots */
				swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRJobProcessSlotID::ItemCount (slot count) process_id / lHandleSlot : [%s] / [%d]", pj_id, lHandleSlot);
				m_pLogData->PushLogs(tzMesg);
				hResult	= pICxMtrlList->ItemCount(lHandleSlot, &lSlotCount);
				if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

				/* Get each slot number */
				for (lSlotIndex=1; lSlotIndex<=lSlotCount; lSlotIndex++)
				{
					/* Get the carrier's slot number for the substrate to process */
					swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRJobProcessSlotID::GetValueU1 (slot number) process_id / SlotIndex / lHandleSlot : [%s] / [%d] / [%d]", pj_id, lSlotIndex, lHandleSlot);
					m_pLogData->PushLogs(tzMesg);
					hResult	= pICxMtrlList->GetValueU1(lHandleSlot, lSlotIndex, &u8SlotNum);
					if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
					/* �˻��� Slot Number�� ��ȯ ����Ʈ�� ���� */
					slot_no.AddTail(u8SlotNum);
				}
			}
		}
	}

	return (hResult == S_OK && slot_no.GetCount() > 0);
}

/*
 desc : Process Job (ID)�� ���Ե� Slot Number �� ������
 parm : pj_id	- [in]  Process Job ID
		slot_no	- [out] Slot Number�� ����� ����
		carr_id	- [in]  Substrate ID�� ���� Carrier ID�� ����� ����
 retn : TRUE or FALSE
*/
BOOL CE40PJM::GetProcessJobAllSlotNo(PTCHAR pj_id, CByteArray &slot_no, CStringArray &carr_id)
{
	/* ���� ���� ���� */
	slot_no.RemoveAll();
	carr_id.RemoveAll();

	/* Carrier ID�� Slot Number ��� */
	if (!GetPRMtlNameList(pj_id, carr_id, slot_no))	return FALSE;

	return slot_no.GetCount() > 0 ? TRUE : FALSE;
}

/*
 desc : Process Job (ID)�� ���Ե� Substrate ID �� ������
 parm : pj_id	- [in]  Process Job ID
		subst_id- [out] Substrate ID�� ����� ����
		carr_id	- [in]  Substrate ID�� ���� Carrier ID�� ����� ����
 retn : TRUE or FALSE
*/
BOOL CE40PJM::GetProcessJobAllSubstID(PTCHAR pj_id, CStringArray &subst_id, CStringArray &carr_id)
{
	INT32 i 	= 0x00;
	TCHAR tzSubstID[CIM_SUBSTRATE_ID];
	CByteArray arrSlotNo;

	/* ���� ���� ���� */
	subst_id.RemoveAll();
	carr_id.RemoveAll();

	/* Carrier ID�� Slot Number ��� */
	if (!GetPRMtlNameList(pj_id, carr_id, arrSlotNo))	return FALSE;
	/* Subbstrate ID ����� */
	for (; i<arrSlotNo.GetCount(); i++)
	{
#if 0	/* swprintf_s ���������� ���� �ʱ�ȭ? */
		wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);
#endif
		swprintf_s(tzSubstID, CIM_SUBSTRATE_ID, L"%s.%02d", carr_id[i].GetBuffer(), arrSlotNo[i]);
		subst_id.Add(tzSubstID);
	}

	return subst_id.GetCount() > 0 ? TRUE : FALSE;
}

/*
 desc : Process Job (ID)�� ���� ���� ��ȯ
 parm : pj_id	- [in]  Process Job ID
		state	- [OUT] state
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
BOOL CE40PJM::GetProcessJobState(PTCHAR pj_id, E40_PPJS &state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrProcID (pj_id);
	ProcessJobStateEnum enState;

	/* Get a handle to the list of slot # */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessJobState::Process process_id [%s]", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRGetJobState(bstrProcID, &enState);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* ���� �� ��ȯ */
	state	= E40_PPJS(enState);

	return (hResult == S_OK);
}

/*
 desc : Process Job Queue ��ȯ (Queue���� �����Ǵ� ��� Process Job ID)
 parm : list_job- [out] Job ID�� ��ȯ�Ǿ� ����� �迭 ����
		logs	- [in]  �α� ��� ����
 ��ȯ : TRUE or FALSE
*/
BOOL CE40PJM::GetProcessJobQueue(CStringArray &list_job)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	LONG i	= 0;
	HRESULT hResult	= S_OK;
	VARIANT varJobIDs;
	BSTR *bstrList;
	SAFEARRAY *psaJobIDs;
	CString strProcID;

	/* ���� ��ϵ� �� ��� ���� */
	list_job.RemoveAll();

	/* Initializes a varJobIDs */
	VariantInit(&varJobIDs);

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessJobQueue::PRGetPrJobQueue");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRGetPrJobQueue(&varJobIDs);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	psaJobIDs	= varJobIDs.parray;
	hResult		= SafeArrayAccessData(psaJobIDs, (VOID**)&bstrList);	/* Lock it down */
	m_pLogData->CheckResult(hResult, tzMesg);
	for (; i<(LONG)psaJobIDs->rgsabound->cElements; i++)
	{
		strProcID.Format(L"%s", bstrList[i]);
		list_job.Add(strProcID);
	}

	return (hResult == S_OK);
}

/*
 desc : Processing Resource�� ���� Queue���� ������ �� �ִ� ���� �ִ� Jobs ���� ��ȯ
 parm : count	- [out] ������ �� �ִ� ���� ���� ��ȯ ���� ����
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
BOOL CE40PJM::GetProcessJobSpaceCount(UINT32 &count)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	LONG lCount	= 0;
	HRESULT hResult	= S_OK;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessJobCreatingCount::PRGetPrJobQueue");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRGetSpace(&lCount);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* ���� ��ȯ */
	count	= lCount;

	return (hResult == S_OK);
}

/*
 desc : ���� Process Job ID �� �� ó�� SELECTED ������ Process Job ID ��ȯ
 parm : filter	- [in]  Process Job ID �˻� ���� (E40_PPJS)
		pj_id	- [out] ��ȯ�Ǿ� ����� Process Job ID (String)
		size	- [in]  'pj_id' ���� ũ��
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
BOOL CE40PJM::GetProcessJobFirstID(E40_PPJS filter, PTCHAR pj_id, UINT32 size)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	LONG i	= 0, lItemCount, lHandle;
	HRESULT hResult	= S_OK;
	ICxValueDispPtr pJobList (CLSID_CxValueObject);
	CComBSTR bstrProcID;
	UINT8 u8State;
	CString strProcID;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessJobFirstID::PRGetAllJobs");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRGetAllJobs(pJobList);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	hResult	= pJobList->ItemCount(0, &lItemCount);
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessJobFirstID::ItemCount");
	m_pLogData->PushLogs(tzMesg);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	for (i=1; i<=lItemCount; i++)
	{
		hResult	= pJobList->ListAt(0, i, &lHandle);
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessJobFirstID::ListAt (0, %d, handle)", i);
		m_pLogData->PushLogs(tzMesg);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		hResult	= pJobList->GetValueAscii(lHandle, 1, &bstrProcID);
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessJobFirstID::GetValueAscii (handle=%d, index=1)", lHandle);
		m_pLogData->PushLogs(tzMesg);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		hResult	= pJobList->GetValueU1(lHandle, 2, &u8State);
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessJobFirstID::GetValueU1 (handle=%d, index=2)", lHandle);
		m_pLogData->PushLogs(tzMesg);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		/* �˻��ϰ��� �ϴ� ���� (Process Job ID State)�� �������� ���� */
		if (ProcessJobStateEnum(filter) == ProcessJobStateEnum(u8State))
		{
#if 1
			strProcID.Format(L"%s", bstrProcID);
			swprintf_s(pj_id, size, L"%s", strProcID.GetBuffer());
#else
			swprintf_s(pj_id, size, L"%s", (LPCTSTR)_bstr_t(bstrProcID));
#endif
			return TRUE;
		}
	}

	return FALSE;
}

/*
 desc : ��� Process Job Id ��ȯ (�Ϸ���� ���� �۾��� ���¸� ��ȯ)
 parm : list_job- [out] Job ID�� ��ȯ�Ǿ� ����� �迭 ����
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
BOOL CE40PJM::GetProcessAllJobId(CStringArray &list_job)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	LONG i	= 0, lItemCount, lHandle;
	HRESULT hResult	= S_OK;
	ICxValueDispPtr pCxJobList (CLSID_CxValueObject);
	CComBSTR bstrProcID;
	CString strProcID;

	/* ���� ��ϵ� �� ��� ���� */
	list_job.RemoveAll();

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessAllJobIdState::PRGetAllJobs");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRGetAllJobs(pCxJobList);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	hResult	= pCxJobList->ItemCount(0, &lItemCount);
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessAllJobIdState::ItemCount");
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	for (i=1; i<=lItemCount; i++)
	{
		hResult	= pCxJobList->ListAt(0, i, &lHandle);
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessAllJobIdState::ListAt (0, %d, handle)", i);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		hResult	= pCxJobList->GetValueAscii(lHandle, 1, &bstrProcID);
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessAllJobIdState::GetValueAscii (handle=%d, index=1)", lHandle);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		/* Add Project Job ID and State */
		strProcID.Format(L"%s", bstrProcID);
		list_job.Add(strProcID);
	}

	return ((list_job.GetCount() > 0) && hResult == S_OK);
}

/*
 desc : ��� Process Job Id�� State ��ȯ
 parm : list_job	- [out] Job ID�� ��ȯ�Ǿ� ����� �迭 ����
		list_state	- [out] ���� ���� ������� ����Ʈ ����
		logs		- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
#if 1
BOOL CE40PJM::GetProcessAllJobIdState(CStringArray &list_job, CAtlList <E40_PPJS> &list_state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	LONG i	= 0, lItemCount, lHandle;
	HRESULT hResult	= S_OK;
	ICxValueDispPtr pICxJobList (CLSID_CxValueObject);
	CComBSTR bstrProcID;
	UINT8 u8State;
	CString strProcID;

	/* ���� ��ϵ� �� ��� ���� */
	list_job.RemoveAll();
	list_state.RemoveAll();

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessAllJobIdState::PRGetAllJobs");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRGetAllJobs(pICxJobList);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	hResult	= pICxJobList->ItemCount(0, &lItemCount);
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessAllJobIdState::ItemCount");
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	for (i=1; i<=lItemCount; i++)
	{
		hResult	= pICxJobList->ListAt(0, i, &lHandle);
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessAllJobIdState::ListAt (0, %d, handle)", i);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		hResult	= pICxJobList->GetValueAscii(lHandle, 1, &bstrProcID);
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessAllJobIdState::GetValueAscii (handle=%d, index=1)", lHandle);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		hResult	= pICxJobList->GetValueU1(lHandle, 2, &u8State);
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessAllJobIdState::GetValueU1 (handle=%d, index=2)", lHandle);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		/* Add Project Job ID and State */
		strProcID.Format(L"%s", bstrProcID);
		list_job.Add(strProcID);
		list_state.AddTail(E40_PPJS(u8State));
	}

	return ((list_job.GetCount() == list_state.GetCount()) && hResult == S_OK);
}
#else
BOOL CE40PJM::GetProcessAllJobIdState(CStringArray &list_job, CAtlList<E40_PPJS> &list_state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	LONG i	= 0, lCount;
	HRESULT hResult	= S_OK;
	ProcessJobStateEnum enState;
	VARIANT vtIDs;
	CComSafeArray<BSTR> saIDs;
	CComBSTR *bstrIDs;
	CString strProcID;

	/* ���� ��ϵ� �� ��� ���� */
	list_job.RemoveAll();
	list_state.RemoveAll();

	VariantInit(&vtIDs);

	if (logs)
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessAllJobIdState::PRGetAllJobs");
		m_pLogData->PushLogs(tzMesg);
	}

	hResult	= m_pICxE40PJMPtr->PRGetPrJobQueue(&vtIDs);
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessAllJobIdState::PRGetPrJobQueue");
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* Get access to the list of IDs, States, Texts */
	hResult = saIDs.Attach(vtIDs.parray);
	if (logs)
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessAllJobIdState::ItemCount");
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	}
	/* ���� �߻�(?)�� �˶��� ���� ���ϱ� */
	if ((lCount = GetSafeArrayCount(saIDs, 1)) > 0)
	{
		/* Get the ID Array */
		hResult	= SafeArrayLock(saIDs);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetCommandList (IDs)"), logs)
		{
			hResult	= SafeArrayAccessData(saIDs, (PVOID*)&bstrIDs);
			if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetCommandList (IDs)"), logs)
				hResult	= SafeArrayUnaccessData(saIDs);
			hResult	= SafeArrayUnlock(saIDs);
		}
		/* Command ID�� Names ���  */
		for (i=0;SUCCEEDED(hResult) && i<lCount;i++)
		{
			if (logs)
			{
				swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessAllJobOIdState::PRGetJobState : pr_job_id=%s", (LPTSTR)_bstr_t(bstrIDs[i]));
				m_pLogData->PushLogs(tzMesg);
			}
			/* Process Job ID�� ���� �� ���  */
			hResult	= m_pICxE40PJMPtr->PRGetJobState(bstrIDs[i], &enState);
			if (m_pLogData->CheckResult(hResult, tzMesg))	continue;;
			/* �˻��� ��� �� ��� */
			list_job.Add(bstrIDs[i]);
			list_state.AddTail(E40_PPJS(enState));
		}
	}

	/* SafeArray ���� */
	saIDs.Detach();

	return ((list_job.GetCount() == list_state.GetCount()) && hResult == S_OK);
}
#endif
/*
 desc : Process Job Remove
 parm : pj_id	- [in]  Objet ID of the Process Job
 retn : TRUE or FALSE
*/
BOOL CE40PJM::SetProcessJobRemove(PTCHAR pj_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrProcID (pj_id);

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetProcessJobRemove::PRJobRemove");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRJobRemove(bstrProcID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Process Job�� Queuing ���¿��� Setting Up ���·� �̵� (����)
 parm : pj_id	- [in]  Objet ID of the Process Job
 retn : TRUE or FALSE
*/
BOOL CE40PJM::SetProcessJobQueueToSetup(PTCHAR pj_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrProcID (pj_id);
	ProcessJobStateEnum enPRJobState;

	/* ���� Process Job State �� ��� */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetProcessJobQueueToSetup::PRGetJobState : pj_id=%s", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRGetJobState(bstrProcID, &enPRJobState);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* Process Job State�� ���� SETTING_UP �����̸� �ٷ� return TRUE */
	if (enPRJobState == ProcessJobStateEnum::pjSETTING_UP)	return TRUE;
	/* ���� Process Job State�� Create Ȥ�� Queued ���°� �ƴ��� ���� Ȯ�� */
	if (enPRJobState > pjSETTING_UP)
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetProcessJobQueueToSetup : PRJobState > pjSETTING_UP : pj_id=%s", pj_id);
		m_pLogData->PushLogs(tzMesg, ENG_GALC::en_warning);
		return FALSE;
	}

	/* ���� Process Job State �� ���� */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetProcessJobQueueToSetup::PRJobSetup : pj_id=%s", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRJobSetup(CComBSTR(pj_id));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Process Job ����
 parm : pj_id		- [in]  Process Job Name ID (String; Ascii)
		mtrl_type	- [in]  Material Type (E40_PPMT)
		carr_id		- [in]  Carrier Name ID (String; Ascii) List
		recipe_name	- [in]  Recipe Name ID
		slot_no		- [in]  Slot Number (U1 Type) List (1 ~ 25 �� ���� ������ŭ �����)
		recipe_only	- [in]  TRUE  : RECIPE_ONLY (Recipe method without recipe variable)
							FALSE : RECIPE_WITH_VARIABLE_TUNING (Recipe method with recipe variable)
		proc_start	- [in]  Process Start ����
							Indicates that the processing resource starts processing imediately
							when ready (PRProcessStart) (Auto Start ����)
		en_hanced	- [in]  Enhanced create �Լ� ���� ����
 retn : TRUE or FALSE
 note : This method is called by the host to create a new Process Job
*/
BOOL CE40PJM::SetProcessJobCreate(PTCHAR pj_id, PTCHAR carr_id, PTCHAR recipe_name,
								  E40_PPMT mtrl_type, BOOL recipe_only, BOOL proc_start,
								  CAtlList <UINT8> *slot_no, BOOL en_hanced)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	LONG i, lHandle	= 0;
	HRESULT hResult	= S_OK;
	CComBSTR bstrProcID (pj_id);
	VARIANT_BOOL vbStart	= proc_start ? VARIANT_TRUE : VARIANT_FALSE;
	RecipeMethodEnum enRecipeMethod	= recipe_only ? pjRECIPE_ONLY : pjRECIPE_WITH_VARIABLE_TUNING;
	ICxValueDispPtr pICxMtrlList (CLSID_CxValueObject);
	ICxValueDispPtr pICxRecipe (CLSID_CxValueObject);
	ICxValueDispPtr pICxEvents (CLSID_CxValueObject);
	ICxValueDispPtr pICxStatus (CLSID_CxValueObject);

	/* Material to process : Material Name List�� ��� GetPRJobProcessSlotID �Լ� ���� */
	/* L {L {A CarrierID} {L {U1 slot} {U1 slot} ...}} */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetProcessJobCreate::PRJobCreate (List) : pj_id = %s "
										  L"carr_id = %s, material_type=%d, recipe_only=%d, proc_start=%d",
			   pj_id, carr_id, mtrl_type, recipe_only, proc_start);
	m_pLogData->PushLogs(tzMesg);
	if (hResult==S_OK)	hResult	= pICxMtrlList->AppendList(0, &lHandle);
	if (hResult==S_OK)	hResult	= pICxMtrlList->AppendValueAscii(lHandle, CComBSTR(carr_id));
	if (hResult==S_OK)	hResult	= pICxMtrlList->AppendList(lHandle, &lHandle);
	for (i=0; i<slot_no->GetCount(); i++)
	{
		if (hResult==S_OK)	hResult	= pICxMtrlList->AppendValueU1(lHandle,
																  (UINT8)(slot_no->GetAt(slot_no->FindIndex(i))));
	}
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Recipe ���� */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetProcessJobCreate::PRJobCreate (Recipe=%s) : pj_id = %s "
										  L"carr_id = %s, material_type=%d, recipe_only=%d, proc_start=%d",
			   recipe_name, pj_id, carr_id, mtrl_type, recipe_only, proc_start);
	m_pLogData->PushLogs(tzMesg);
	if (hResult==S_OK)	hResult	= pICxRecipe->AppendValueU1(0, UINT8(enRecipeMethod));
	if (hResult==S_OK)	hResult	= pICxRecipe->AppendValueAscii(0, CComBSTR(recipe_name));
	if (hResult==S_OK)	hResult	= pICxRecipe->AppendList(0, &lHandle);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* No pause events */
	pICxEvents->SetDataType(0, 0, ValueType(L));

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetProcessJobCreate::PRJobCreate : pj_id = %s "
										  L"carr_id = %s, material_type=%d, recipe_only=%d, proc_start=%d",
			   pj_id, carr_id, mtrl_type, recipe_only, proc_start);
	m_pLogData->PushLogs(tzMesg);
	if (!en_hanced)
	{
		if (hResult==S_OK)	hResult	= m_pICxE40PJMPtr->PRJobCreate(&bstrProcID,
																  MaterialTypeEnum(mtrl_type),
																  pICxMtrlList, pICxRecipe,
																  vbStart, pICxStatus);
	}
	else
	{
		if (hResult==S_OK)	hResult	= m_pICxE40PJMPtr->PRJobCreateEnh(bstrProcID,
																	 MaterialTypeEnum(mtrl_type),
																	 pICxMtrlList, pICxRecipe,
																	 vbStart, pICxEvents, pICxStatus);
	}
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		DispErrorData(pICxStatus, E30_SVET::en_acka);
		return FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : Remote Control ��� ���� ����
 parm : enable	- [in]  Enable or Disable
 retn : TRUE or FALSE
*/
BOOL CE40PJM::SetAllowRemoteControl(BOOL enable)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult			= S_OK;
	VARIANT_BOOL boEnable	= enable == TRUE ? VARIANT_TRUE : VARIANT_FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetAllowRemoteControl::set_AllowRemoteControl : enable = %d", enable);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->put_AllowRemoteControl(boEnable);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Remote Control ��� ���� �� ��ȯ
 parm : enable	- [out] Enable or Disable
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
BOOL CE40PJM::GetAllowRemoteControl(BOOL &enable)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult			= S_OK;
	VARIANT_BOOL boEnable	= VARIANT_FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetAllowRemoteControl::get_AllowRemoteControl");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->get_AllowRemoteControl(&boEnable);
	m_pLogData->CheckResult(hResult, tzMesg);
	/* �� ��ȯ */
	enable	= VARIANT_BOOL(boEnable) == TRUE;

	return (hResult == S_OK);
}

/*
 desc : Process Material Name List (For substrates)�� �Ӽ� �� (Carrier ID, Slot ID) ��ȯ
		!!! TEL�� ���, ������ 2��° ��, CarrierID �� Slot No�� �Ǿ� �ִ� ������� �о�� �� !!!
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		carr_id	- [out] Carrier ID�� ��ȯ�Ǿ� ����� ����
		slot_no	- [out] Slot Number�� ��ȯ�Ǿ� ����� ����
 retn : TRUE or FALSE
*/
BOOL CE40PJM::GetPRMtlNameList(PTCHAR pj_id, CStringArray &carr_id, CByteArray &slot_no)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	UINT8 u8SlotNo	= 0;
	LONG lHandle[3]	= {NULL}, lItemCarrID = 0, lItemSlotNo=0, i, j;
	HRESULT hResult	= S_OK;
	CComBSTR bstrCarrID;
	ICxProcessJobPtr pPRJob	= NULL;
	ICxValueDispPtr pICxValue (CLSID_CxValueObject);

	/* ���� ��ϵ� �� ��� ���� */
	carr_id.RemoveAll();
	slot_no.RemoveAll();

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRMtlNameList::PRGetJob : Process Job ID=%s", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRGetJob(CComBSTR(pj_id), &pPRJob);
	if (!pPRJob || m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* PRMtl Name List ��� */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRMtlNameList::get_PRMtlNameList : Process Job ID=%s", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pPRJob->get_PRMtlNameList(&pICxValue);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* ��ϵ� Carrier ID ������ ���� (L {L {A CarrierId} {L {U1 slot}...}}...) */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRMtlNameList::ItemCount");
	hResult	= pICxValue->ItemCount(0, &lItemCarrID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Carrier ID �� Slot �׷� �ڵ� ���� ��� */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRMtlNameList::ListAt");
	hResult	= pICxValue->ListAt(0, 0, &lHandle[0]);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* ��ϵ� Carrier ID ������ŭ �ݺ� */
	for (i=1; i<=lItemCarrID; i++)
	{
		/* Carrier ID & Slot Group -> [ i ]th Carrier ID [ Handle ] ���� ��� */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRMtlNameList::ListAt : lHandle[0]=%d, index=%d",
				   lHandle[0], i);
		hResult	= pICxValue->ListAt(lHandle[0], i, &lHandle[1]);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		/* Carrier ID & Slot Group -> [ i ]th Carrier ID [ Carrier ID ] ��� */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRMtlNameList::GetValueAscii : lHandle[1]=%d, index=1",
				   lHandle[1]);
		hResult	= pICxValue->GetValueAscii(lHandle[1], 1, &bstrCarrID);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
		/* Carrier ID & Slot Group -> [ i ]th Slot Number [ Handle ] ��� */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRMtlNameList::ListAt : lHandle[1]=%d, index=2",
				   lHandle[1]);
		hResult	= pICxValue->ListAt(lHandle[1], 2, &lHandle[2]);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
		/* Carrier ID & Slot Group -> [ i ]th Slot Number [ Handle ]�� ���� ���� ��� */
		hResult	= pICxValue->ItemCount(lHandle[2], &lItemSlotNo);
		/* Carrier ID ���� ��ϵ� Slot Number ������ŭ �ݺ� �� Slot Number ��� */
		for (j=1; j<=lItemSlotNo; j++)
		{
			/* Slot Number ����Ʈ ��ȣ ��� */
			swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRMtlNameList::GetValueU1 : lHandle[2]=%d, index=%d",
					   lHandle[2], j);
			hResult	= pICxValue->GetValueU1(lHandle[2], j, &u8SlotNo);
			if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

			/* ��ȯ ���ۿ� ��� */
			carr_id.Add(LPTSTR(_bstr_t(bstrCarrID)));
			slot_no.Add(u8SlotNo);
		}
	}

	return (hResult == S_OK);
}

/*
 desc : Process Job State�� ������ ���� ��Ŵ
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		control	- [in]  E40_PPJC (Abort, Cancel, Pause, Paused, Resume, Start, Stop)
 retn : TRUE or FALSE
*/
BOOL CE40PJM::SetPRJobStateControl(PTCHAR pj_id, E40_PPJC control)
{
	BOOL bCondition	= TRUE;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	E40_PPJS enState	= E40_PPJS::en_created;
	ProcessJobStateEnum enPJState;
	ICxValueDispPtr pICxParam (CLSID_CxValueObject);
	ICxValueDispPtr pICxStatus (CLSID_CxValueObject);

	/* ���� Process Job ID�� ���� ���� �� ��ȯ */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRJobStateControl::PRGetJobState : "
										  L"pj_id=%s, control=%d", pj_id, control);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRGetJobState(CComBSTR(pj_id), &enPJState);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* �� ��ȯ �� ���� */
	enState	= (E40_PPJS)enPJState;
	switch (control)
	{
#if 0
	case E40_PPJC::en_abort	: bCondition = (enState == E40_PPJS::en_processing)			|| (enState == E40_PPJS::en_pausing)	||
										   (enState == E40_PPJS::en_paused)				|| (enState == E40_PPJS::en_stopped);		break;
	case E40_PPJC::en_pause	: bCondition = (enState == E40_PPJS::en_processing)			|| (enState == E40_PPJS::en_paused);		break;
	case E40_PPJC::en_paused: bCondition = (enState == E40_PPJS::en_pausing);														break;
	case E40_PPJC::en_resume: bCondition = (enState == E40_PPJS::en_pausing)			|| (enState == E40_PPJS::en_paused);		break;
	case E40_PPJC::en_start	: bCondition = (enState == E40_PPJS::en_waiting_for_start);												break;
	case E40_PPJC::en_stop	: bCondition = (enState == E40_PPJS::en_processing)			|| (enState == E40_PPJS::en_pausing)	||
										   (enState == E40_PPJS::en_paused);														break;
#else
	case E40_PPJC::en_abort	: bCondition = (enState == E40_PPJS::en_queued)				|| (enState == E40_PPJS::en_setting_up) ||
										   (enState == E40_PPJS::en_waiting_for_start)	|| (enState == E40_PPJS::en_processing) ||
										   (enState == E40_PPJS::en_pausing)			|| (enState == E40_PPJS::en_paused)		||
										   (enState == E40_PPJS::en_stopped);														break; // [Ian] PRAbort: QUEUED(without parent ControlJob only), EXECUTING(setting up, waiting for start, processing), PAUSING/PAUSED, STOPPING state to ABORTING state
	case E40_PPJC::en_pause	: bCondition = (enState == E40_PPJS::en_setting_up)			||
										   (enState == E40_PPJS::en_waiting_for_start)	|| (enState == E40_PPJS::en_processing);	break; // [Ian] PRPause: EXECUTING(setting up, waiting for start, processing) state to PAUSING state
	case E40_PPJC::en_paused: bCondition = (enState == E40_PPJS::en_pausing);														break; // [Ian] PJPaused: PAUSING state to PAUSED state
	case E40_PPJC::en_resume: bCondition = (enState == E40_PPJS::en_pausing)			|| (enState == E40_PPJS::en_paused);		break; // [Ian] PJResume: PAUSING/PAUSED state to EXECUTING state
	case E40_PPJC::en_start	: bCondition = (enState == E40_PPJS::en_waiting_for_start);												break; // [Ian] PRStart: WAITING FOR START state to PROCESSING state
	case E40_PPJC::en_stop	: bCondition = (enState == E40_PPJS::en_queued)				|| (enState == E40_PPJS::en_setting_up) ||
										   (enState == E40_PPJS::en_waiting_for_start)	|| (enState == E40_PPJS::en_processing) ||
										   (enState == E40_PPJS::en_pausing)			|| (enState == E40_PPJS::en_paused);		break; // [Ian] PRStop: QUEUED(without parent ControlJob only), EXECUTING(setting up, waiting for start, processing), PAUSING/PAUSED state to STOPPING state
	case E40_PPJC::en_cancel: bCondition = (enState == E40_PPJS::en_queued);														break; // [Ian] PRCncel: QUEUED(without parent ControlJob only) state to JOB COMPLETED state and remove it 
#endif
	}

	/* ���� Process Job ID ���¿��� ��� ������ �������� ���� */
	if (!bCondition)
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRJobStateControl::Invalid control condition : "
											  L"pj_id=%s, control=%d, condition=%d",
				   pj_id, control, enState);
		m_pLogData->PushLogs(tzMesg, ENG_GALC::en_warning);
		return FALSE;
	}

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetPRJobStateControl::Control Function : pj_id=%s, control=%d", pj_id, control);
	m_pLogData->PushLogs(tzMesg);
	switch (control)
	{
	case E40_PPJC::en_abort		:	hResult	= m_pICxE40PJMPtr->PRAbort(CComBSTR(pj_id),	pICxParam, pICxStatus);		break;
	case E40_PPJC::en_cancel	:	hResult	= m_pICxE40PJMPtr->PRCancel(CComBSTR(pj_id), pICxParam, pICxStatus);	break;
#if 0									  
	case E40_PPJC::en_pause		:	hResult	= m_pICxE40PJMPtr->PRPause(CComBSTR(pj_id),	pICxParam, pICxStatus);		break;
	case E40_PPJC::en_paused	:	hResult	= m_pICxE40PJMPtr->PRPaused(CComBSTR(pj_id));							break;
#else									  
	case E40_PPJC::en_pause		:	hResult	= m_pICxE40PJMPtr->PRPause(CComBSTR(pj_id),	pICxParam, pICxStatus);
//									hResult	= m_pICxE40PJMPtr->PRPaused(CComBSTR(pj_id));							break; /* [Ian] �� ����� ��� PRPause �� ��� ���� üũ �� �� ���� �ٷ� pausing -> paused�� ���� ��ȭ�ϸ� �ȴٰ� ��� */
	case E40_PPJC::en_paused	:	hResult	= m_pICxE40PJMPtr->PRPaused(CComBSTR(pj_id));							break; /* [Ian] �� ����� ��� PRPause �� ��� ���� üũ �� �� ���� �ٷ� pausing -> paused�� ���� ��ȭ�ϸ� �ȴٰ� ��� */
#endif									  
	case E40_PPJC::en_resume	:	hResult	= m_pICxE40PJMPtr->PRResume(CComBSTR(pj_id), pICxParam, pICxStatus);	break;
	case E40_PPJC::en_start		:	hResult	= m_pICxE40PJMPtr->PRStart(CComBSTR(pj_id),	pICxParam, pICxStatus);		break;
	case E40_PPJC::en_stop		:	hResult	= m_pICxE40PJMPtr->PRStop(CComBSTR(pj_id),	pICxParam, pICxStatus);		break;
	case E40_PPJC::en_stopped	:	//hResult = m_pICxE40PJMPtr->PRAborted(pICxStatus);
									SetProcessJobStoppingToStopped(CComBSTR(pj_id));								break;
	case E40_PPJC::en_aborted	:	//hResult = m_pICxE40PJMPtr->PRStopped(CComBSTR(pj_id), pICxParam, pICxStatus);	break;
									SetPRJobAbortComplete(CComBSTR(pj_id));											break;
	default						:	m_pLogData->PushLogs(L"This command is not supported", ENG_GALC::en_warning);	return FALSE;
	}
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Process Job State ����
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		state	- [in]  Process Job State (E40_PPJS)
 retn : TRUE or FALSE
 note : Sets the Process Job's state
*/
BOOL CE40PJM::SetProcessJobState(PTCHAR pj_id, E40_PPJS state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	ICxProcessJobPtr pPRJob	= NULL;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetProcessJobState::PRGetJob : Process Job ID=%s", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRGetJob(CComBSTR(pj_id), &pPRJob);
	if (!pPRJob || m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetProcessJobState::put_PRProcessJobState : Job ID=%s, State=%d",
			   pj_id, state);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pPRJob->put_PRProcessJobState(ProcessJobStateEnum(state));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Process Job State : Aborting -> Aborted
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
 retn : TRUE or FALSE
 note : This method is called by the CIM40 package when aborting has completed
		Moves Process Job from ABORTING state to ABORTED state
*/
BOOL CE40PJM::SetProcessJobAborted(PTCHAR pj_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	ICxProcessJobPtr pPRJob	= NULL;
	ICxValueDispPtr pICxStatus (CLSID_CxValueObject);

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetProcessJobAborted::PRGetJob : Process Job ID=%s", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRGetJob(CComBSTR(pj_id), &pPRJob);
	if (!pPRJob || m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetProcessJobAborted::PRAborted : Job ID=%s", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pPRJob->PRAborted(pICxStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		DispErrorData(pICxStatus, E30_SVET::en_acka);
		return FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : Returns the PRMtrlOrder value set by the host using the PRSetMtrlOrder service
 parm : order	- [out] Material Order (E94_CPOM, PRMtrlOrder value)
 retn : TRUE or FALSE
 note : This function returns the PRMtrlOrder value, the order in which materials are processed
*/
BOOL CE40PJM::GetPRMaterialOrder(E94_CPOM &order)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	MaterialOrderEnum enOrder;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRMaterialOrder::PRGetMtrlOrder");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRGetMtrlOrder(&enOrder);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* �� ��ȯ */
	order	= E94_CPOM (enOrder);

	return (hResult == S_OK);
}

/*
 desc : Sets the PRMtrlOrder value by the host using PRSetMtrlOrder service
 parm : order	- [in]  Material Order (E94_CPOM, PRMtrlOrder value)
 retn : TRUE or FALSE
 note : Sets the PRMtrlOrder value by the host using PRSetMtrlOrder service.
		PRSetMtrlOrder callback is called to let the client application grant or deny this request.
*/
BOOL CE40PJM::SetPRMaterialOrder(E94_CPOM order)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	VARIANT_BOOL vbAck;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRMaterialOrder::PRSetMtrlOrder : order = %d", order);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRSetMtrlOrder(MaterialOrderEnum(order), &vbAck);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return ((vbAck == VARIANT_TRUE) && (hResult == S_OK));
}

/*
 desc : Process Job���� ����� ���� ��Ŵ (Executes a command on a Process Job)
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
 retn : TRUE or FALSE
 note : This method is called by the host to perform a PRJobCommand on the specified Process Job
*/
BOOL CE40PJM::SetPRJobCommand(PTCHAR pj_id, PTCHAR command)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	ICxValueDispPtr pICxParamList (CLSID_CxValueObject);
	ICxValueDispPtr pICxStatus (CLSID_CxValueObject);

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRJobCommand::PRJobCommand : pj_id = %s, command=%s", pj_id, command);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRJobCommand(CComBSTR(pj_id), CComBSTR(command), pICxParamList, pICxStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		DispErrorData(pICxStatus, E30_SVET::en_acka);
		return FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : Pause Events�� ���� Enable or Disable
 parm : enable	- [in]  TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE
 note : The client application can disable PauseEvent capability for Process Jobs by setting this value to VARIANT_FALSE.
		Default is VARIANT_TRUE.
		When set to VARIANT_FALSE, all requests to create Process Jobs with the PauseEvent parameter will be rejected.
*/
BOOL CE40PJM::SetEnablePauseEvent(BOOL enable)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	VARIANT_BOOL vbEnable	= enable ? VARIANT_TRUE : VARIANT_FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetEnablePauseEvent::put_EnablePauseEvent : Pause Event Enable = %d", enable);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->put_EnablePauseEvent(vbEnable);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Pause Events�� ���� Enable or Disable ���� �� ��ȯ
 parm : enable	- [out] TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE
*/
BOOL CE40PJM::GetEnablePauseEvent(BOOL &enable)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	VARIANT_BOOL vbEnable;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetEnablePauseEvent::get_EnablePauseEvent");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->get_EnablePauseEvent(&vbEnable);
	m_pLogData->CheckResult(hResult, tzMesg);

	enable	= TRUE;
	if (vbEnable == VARIANT_FALSE)	enable	= FALSE;

	return (hResult == S_OK);
}

/*
 desc : ��� �ʿ��� ����ϴ� �˸� (Notification)�� ���� (Type)�� �������� �˷���
 parm : type	- [out] Notification Type (E40_PENT)
 retn : TRUE or FALSE
*/
BOOL CE40PJM::GetNotificationType(E40_PENT &type)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	UseEventEnum enType;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetNotificationType::get_UseEvent");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->get_UseEvent(&enType);
	m_pLogData->CheckResult(hResult, tzMesg);

	type	= E40_PENT(enType);

	return (hResult == S_OK);
}

/*
 desc : ��� �ʿ��� ����� �˸� (Notification)�� ���� (Type)�� � ������ ������ ��
 parm : type	- [in]  Notification Type (E40_PENT)
 retn : TRUE or FALSE
*/
BOOL CE40PJM::SetNotificationType(E40_PENT type)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetNotificationType::put_UseEvent = %d", type);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->put_UseEvent(UseEventEnum(type));
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Process Job ID�� Job State�� ����Ǿ��ٰ� Host���� �˸�
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
 retn : TRUE or FALSE
 note : This method is called by the client application to notify the host about changes in a Process Job's state
*/
BOOL CE40PJM::SetPRJobAlert(PTCHAR pj_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	ICxValueDispPtr pICxStatus (CLSID_CxValueObject);

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRJobAlert::PRJobAlert : pj_id=%s", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRJobAlert(CComBSTR(pj_id), pICxStatus);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Process Job ID�� Event State�� ����Ǿ��ٰ� Host���� �˸�
 parm : pj_id		- [in]  Process Job Name ID (String; Ascii)
		evt_id		- [in]  PREventID of PRJobEvent service (E40_PSEO)
		list_vid	- [in]  Variables ID�� ����� ����Ʈ
		list_type	- [in]  Variables Type�� ����� ����Ʈ
		list_value	- [in]  'Variables Type'�� ����, ���� Ȥ�� ���ڿ� ���� ��ϵǾ� ����
							�����, ������ ���, _wtoi / _wtof ��� ��ȯ�� �ʿ� ��
 retn : TRUE or FALSE
 note : This method is called by the client application to notify the host about changes in a Process Job's state
*/
BOOL CE40PJM::SetPRJobEvent(PTCHAR pj_id, E40_PSEO evt_id,
							CAtlList <UINT32> *list_vid,
							CAtlList <E30_GPVT> *list_type,
							CStringArray *list_value)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	LONG i, lHandle1= 0, lHandle2 = 0;
	HRESULT hResult	= S_OK;
	ICxValueDispPtr pICxEventData (CLSID_CxValueObject);

	/* �θ� List �ڵ� ��� */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRJobEvent::AppendList(0, &lHandle1) : pj_id=%s, event_id=%d", pj_id, evt_id);
	hResult	= pICxEventData->AppendList(0, &lHandle1);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	for (i=0; i<list_vid->GetCount(); i++)
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRJobEvent::AppendList(lHandle1, &lHandle2) : pj_id=%s, event_id=%d", pj_id, evt_id);
		hResult	= pICxEventData->AppendList(lHandle1, &lHandle2);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
		/* Variable ID ��� */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRJobEvent::AppendList(lHandle1, &lHandle2) : pj_id=%s, event_id=%d", pj_id, evt_id);
		hResult	= pICxEventData->AppendValueU4(lHandle2, list_vid->GetAt(list_vid->FindIndex(i)));
		/* Variable Type�� ���� �� (���� or ���ڿ�) ��� */
		switch (list_type->GetAt(list_type->FindIndex(i)))
		{
		case E30_GPVT::en_i1	:	hResult	= pICxEventData->AppendValueI1(lHandle2, (small)_wtoi(list_value->GetAt(i)));				break;
		case E30_GPVT::en_i2	:	hResult	= pICxEventData->AppendValueI2(lHandle2, (INT16)_wtoi(list_value->GetAt(i)));				break;
		case E30_GPVT::en_i4	:	hResult	= pICxEventData->AppendValueI4(lHandle2, (INT32)_wtoi(list_value->GetAt(i)));				break;
		case E30_GPVT::en_i8	:	hResult	= pICxEventData->AppendValueI8(lHandle2, (INT64)_wtoi(list_value->GetAt(i)));				break;

		case E30_GPVT::en_u1	:	hResult	= pICxEventData->AppendValueU1(lHandle2, (UINT8)_wtoi(list_value->GetAt(i)));				break;
		case E30_GPVT::en_u2	:	hResult	= pICxEventData->AppendValueU2(lHandle2, (UINT16)_wtoi(list_value->GetAt(i)));				break;
		case E30_GPVT::en_u4	:	hResult	= pICxEventData->AppendValueU4(lHandle2, (UINT32)_wtoi(list_value->GetAt(i)));				break;
		case E30_GPVT::en_u8	:	hResult	= pICxEventData->AppendValueU8(lHandle2, (UINT64)_wtoi(list_value->GetAt(i)));				break;

		case E30_GPVT::en_f4	:	hResult	= pICxEventData->AppendValueF4(lHandle2, (FLOAT)_wtof(list_value->GetAt(i)));				break;
		case E30_GPVT::en_f8	:	hResult	= pICxEventData->AppendValueF8(lHandle2, (DOUBLE)_wtof(list_value->GetAt(i)));				break;

		case E30_GPVT::en_a		:	hResult	= pICxEventData->AppendValueAscii(lHandle2, CComBSTR(list_value->GetAt(i)));

		case E30_GPVT::en_b		:	hResult	= pICxEventData->AppendValueBoolean(lHandle2, VARIANT_BOOL(_wtoi(list_value->GetAt(9))));	break;
		case E30_GPVT::en_bi	:	hResult	= pICxEventData->AppendValueI1(lHandle2, (UINT8)_wtoi(list_value->GetAt(i)));				break;

		default					:	m_pLogData->PushLogs(L"SetPRJobEvent::Data format not supported", ENG_GALC::en_warning);			return FALSE;
		}
	}

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRJobEvent::PRJobEvent : pj_id=%s, event_id=%d", pj_id, evt_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRJobEvent(CComBSTR(pj_id), JobEventDataEnum(evt_id), pICxEventData);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : ���� (Ư��) ���μ��� Job������ �Ӽ��� "PRProcessStart"�� ���� ����
 parm : list_jobs	- [in]  ���� ����� Process Job���� ����� ����Ʈ
		start		- [in]  TRUE or FALSE
 retn : TRUE or FALSE
*/
BOOL CE40PJM::SetPRJobSetStartMethod(CStringArray *list_jobs, BOOL start)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	LONG i, lHandle= 0;
	HRESULT hResult	= S_OK;
	VARIANT_BOOL vbStart	= start ? VARIANT_TRUE : VARIANT_FALSE;
	ICxValueDispPtr pICxJobIDList (CLSID_CxValueObject);
	ICxValueDispPtr pICxStatus (CLSID_CxValueObject);
#if 0
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRJobSetStartMethod::AppendList(0, &lHandle) : start=%d", start);
	hResult	= pICxJobIDList->AppendList(0, &lHandle);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
#endif
	for (i=0; i<list_jobs->GetCount(); i++)
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRJobSetStartMethod::AppendList(0, &lHandle) : start=%d", start);
		hResult	= pICxJobIDList->AppendValueAscii(lHandle, CComBSTR(list_jobs->GetAt(i)));
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	}

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRJobSetStartMethod::PRJobSetStartMethod : start=%d, prjob_count=%d",
			   start, (INT32)list_jobs->GetCount());
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRJobSetStartMethod(pICxJobIDList, vbStart, pICxStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		DispErrorData(pICxStatus, E30_SVET::en_acka);
		return FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : ���� ������ Material Type �� ��ȯ
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		type	- [out] ���� ������ Material Type �� ����
 retn : TRUE or FALSE
 note : Gets the PRMtlType attribute value
*/
BOOL CE40PJM::GetPRMtlType(PTCHAR pj_id, E40_PPMT &type)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	MaterialTypeEnum enType;
	ICxProcessJobPtr pICxProcJob;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRMtlType::PRGetJob process_id : [%s]", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRGetJob(CComBSTR(pj_id), &pICxProcJob);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRMtlType::get_PRMtlType");
	hResult	= pICxProcJob->get_PRMtlType(&enType);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* �� ��ȯ */
	type	= E40_PPMT(enType);

	return (hResult == S_OK);
}

/*
 desc : Material Type �� ���� ����
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		type	- [in]  ������ Maerial Type ��
 retn : TRUE or FALSE
 note : Sets the PRMtlType attribute value
*/
BOOL CE40PJM::SetPRMtlType(PTCHAR pj_id, E40_PPMT type)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	ICxProcessJobPtr pICxProcJob;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRMtlType::PRGetJob process_id : [%s]", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRGetJob(CComBSTR(pj_id), &pICxProcJob);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRMtlType::put_PRMtlType : pj_id=%s, type=%d", pj_id, type);
	hResult	= pICxProcJob->put_PRMtlType(MaterialTypeEnum(type));
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : ó���� �����ϱ� ���� ��� ���� (Wafer? Substrate)���� ������ ������ ����ϴ��� ���� �� ��ȯ
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		value	- [out] TRUE : ��� ���� (����)���� �����ϴµ� ��ٸ��� ����
						FALSE: ��� ���� (����)���� �̹� ������ ������
 retn : TRUE or FALSE
 note : Gets the Process Job's PRWaitForAllMaterial switch value.
*/
BOOL CE40PJM::GetPRWaitForAllMaterial(PTCHAR pj_id, BOOL &value)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	VARIANT_BOOL vbValue;
	ICxProcessJobPtr pICxProcJob;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRWaitForAllMaterial::PRGetJob : process_id=%s", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRGetJob(CComBSTR(pj_id), &pICxProcJob);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRWaitForAllMaterial::get_PRWaitForAllMaterial : pj_id=%s", pj_id);
	hResult	= pICxProcJob->get_PRWaitForAllMaterial(&vbValue);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* ���� �� ��ȯ */
	value	= vbValue == VARIANT_TRUE ? TRUE : FALSE;

	return (hResult == S_OK);
}

/*
 desc : ó���� �����ϱ� ���� ��� ���� (Wafer? Substrate)���� ������ ������ ����ؾ� ���� ���� ����
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		value	- [in]  TRUE : ��� ���� (����)���� �����ϴµ� ��ٸ��� �ִٶ�� ����
						FALSE: ��� ���� (����)���� �����Ͽ��ٶ�� ���� (��ٸ� �ʿ� ����?)
 retn : TRUE or FALSE
 note : Sets the Process Job's PRWaitForAllMaterial switch value.
*/
BOOL CE40PJM::SetPRWaitForAllMaterial(PTCHAR pj_id, BOOL value)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	VARIANT_BOOL vbValue	= value ? VARIANT_TRUE : VARIANT_FALSE;
	ICxProcessJobPtr pICxProcJob;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRWaitForAllMaterial::PRGetJob : process_id=%s", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRGetJob(CComBSTR(pj_id), &pICxProcJob);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRWaitForAllMaterial::put_PRWaitForAllMaterial : pj_id=%s, enable=%d", pj_id, value);
	hResult	= pICxProcJob->put_PRWaitForAllMaterial(vbValue);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Process Job ID�κ��� Recipe ID (=Name; with string) ���
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		rp_id	- [out] Recipe ID�� ����� ����
		size	- [in]  'rp_id' ���� ũ��
 retn : TRUE or FALSE
*/
BOOL CE40PJM::GetRecipeID(PTCHAR pj_id, PTCHAR rp_id, UINT32 size)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrRecipeID;
	ICxSubstratePtr pICxSubstrate;
	ICxProcessJobPtr pICxProcessJob;

	/* Get the process job from its ID */
	swprintf(tzMesg, CIM_LOG_MESG_SIZE, L"GetRecipeID::PRGetJob : pj_id=%s", pj_id);
	hResult	= m_pICxE40PJMPtr->PRGetJob(CComBSTR(pj_id), &pICxProcessJob);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Get the recipe ID */
	swprintf(tzMesg, CIM_LOG_MESG_SIZE, L"GetRecipeID::get_PRRecipeID : pj_id=%s", pj_id);
	hResult	= pICxProcessJob->get_PRRecipeID(&bstrRecipeID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* �� ��ȯ */
	swprintf_s(rp_id, size, L"%s", (LPTSTR)_bstr_t(bstrRecipeID));

	return (hResult == S_OK);
}

/*
 desc : Process Job Start ��� �� ��ȯ (Boolean)
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		method	- [out] Process Start Method (Auto or Manual) ��ȯ ���� ����
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
BOOL CE40PJM::GetProcessStartMethod(PTCHAR pj_id, BOOL &method)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	VARIANT_BOOL vbMethod;
	ICxProcessJobPtr pICxProcessJob;

	/* Get the process job from its ID */
	swprintf(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessStartMethod::PRGetJob : pj_id=%s", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE40PJMPtr->PRGetJob(CComBSTR(pj_id), &pICxProcessJob);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Gets the PRProcessStart attribute value */
	swprintf(tzMesg, CIM_LOG_MESG_SIZE, L"GetProcessStartMethod::get_PRProcessStart : pj_id=%s", pj_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxProcessJob->get_PRProcessStart(&vbMethod);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Process Start Method ��ȯ */
	method	= (vbMethod == VARIANT_TRUE) ? TRUE : FALSE;

	return (hResult == S_OK);
}