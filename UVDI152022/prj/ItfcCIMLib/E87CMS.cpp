
#include "pch.h"
#include "MainApp.h"
#include "E87CMS.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 desc : 생성자
 parm : logs	- [in]  Log Object Pointer
		share	- [in]  Shared Data Object Pointer
 retn : None
*/
CE87CMS::CE87CMS(CLogData *logs, CSharedData *share)
	: CEBase(logs, share)
{
	/* GUID 설정 */
	m_gdICxE	= IID_ICxE87CMS;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CE87CMS::~CE87CMS()
{
	Close();
}

/*
 desc : 초기화 (반드시 호출)
 parm : equip_id	- [in]  Equipment ID (0 or later. 환경 파일에 설정된 개수-1)
		conn_id		- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
 retn : TRUE or FALSE
*/
BOOL CE87CMS::Init(LONG equip_id, LONG conn_id)
{
	if (!CEBase::Init(equip_id, conn_id))	return FALSE;

	/* 아래 순서는 반드시 지킬 것 */
	if (!InitE87CMS())	return FALSE;
	if (!InitE87HW())	return FALSE;

	return TRUE;
}

/*
 desc : 해제
 parm : None
 retn : None
*/
VOID CE87CMS::Close()
{
	HRESULT hResult	= S_OK;

	ICxE87CallbackPtr pICxE87Callback	= m_pEMServiceCbk;
	ICxE87Callback2Ptr pICxE87Callback2	= m_pEMServiceCbk;
	ICxE87Callback3Ptr pICxE87Callback3	= m_pEMServiceCbk;
	ICxE87Callback4Ptr pICxE87Callback4	= m_pEMServiceCbk;
	ICxE87Callback5Ptr pICxE87Callback5	= m_pEMServiceCbk;
	ICxE87Callback6Ptr pICxE87Callback6	= m_pEMServiceCbk;
	ICxE87Callback7Ptr pICxE87Callback7	= m_pEMServiceCbk;
	ICxE87Callback8Ptr pICxE87Callback8	= m_pEMServiceCbk;
	ICxE87Callback9Ptr pICxE87Callback9	= m_pEMServiceCbk;
	ICxE87AsyncCarrierTagCallbackPtr pICxE87AsyncCarrierTagCallback = m_pEMServiceCbk;

	if (!m_pEMServiceCbk)					return;
	if (!m_pEMServiceCbk.GetInterfacePtr())	return;

	if (m_pICxE87CMSPtr && m_pICxE87CMSPtr.GetInterfacePtr())
	{
		m_pLogData->PushLogs(L"ICxE87CMS::UnregisterCallback9");
		hResult = m_pICxE87CMSPtr->UnregisterCallback9(pICxE87Callback9);
		m_pLogData->CheckResult(hResult, L"ICxE87CMS::UnregisterCallback9");
		m_pLogData->PushLogs(L"ICxE87Callback9::Release");
		pICxE87Callback9.Release();
	
		m_pLogData->PushLogs(L"ICxE87CMS::UnregisterCallback8");
		hResult = m_pICxE87CMSPtr->UnregisterCallback8(pICxE87Callback8);
		m_pLogData->CheckResult(hResult, L"ICxE87CMS::UnregisterCallback8");
		m_pLogData->PushLogs(L"ICxE87Callback8::Release");
		pICxE87Callback8.Release();

		m_pLogData->PushLogs(L"ICxE87CMS::UnregisterCallback7");
		hResult = m_pICxE87CMSPtr->UnregisterCallback7(pICxE87Callback7);
		m_pLogData->CheckResult(hResult, L"ICxE87CMS::UnregisterCallback7");
		m_pLogData->PushLogs(L"ICxE87Callback7::Release");
		pICxE87Callback7.Release();

		m_pLogData->PushLogs(L"ICxE87CMS::UnregisterCallback6");
		hResult = m_pICxE87CMSPtr->UnregisterCallback6(pICxE87Callback6);
		m_pLogData->CheckResult(hResult, L"ICxE87CMS::UnregisterCallback6");
		m_pLogData->PushLogs(L"ICxE87Callback6::Release");
		pICxE87Callback6.Release();

		m_pLogData->PushLogs(L"ICxE87CMS::UnregisterCallback5");
		hResult = m_pICxE87CMSPtr->UnregisterCallback5(pICxE87Callback5);
		m_pLogData->CheckResult(hResult, L"ICxE87CMS::UnregisterCallback5");
		m_pLogData->PushLogs(L"ICxE87Callback5::Release");
		pICxE87Callback5.Release();

		m_pLogData->PushLogs(L"ICxE87CMS::UnregisterCallback4");
		hResult = m_pICxE87CMSPtr->UnregisterCallback4(pICxE87Callback4);
		m_pLogData->CheckResult(hResult, L"ICxE87CMS::UnregisterCallback4");
		m_pLogData->PushLogs(L"ICxE87Callback4::Release");
		pICxE87Callback4.Release();
	
		m_pLogData->PushLogs(L"ICxE87CMS::UnregisterCallback3");
		hResult = m_pICxE87CMSPtr->UnregisterCallback3(pICxE87Callback3);
		m_pLogData->CheckResult(hResult, L"ICxE87CMS::UnregisterCallback3");
		m_pLogData->PushLogs(L"ICxE87Callback3::Release");
		pICxE87Callback3.Release();
	
		m_pLogData->PushLogs(L"ICxE87CMS::UnregisterCallback2");
		hResult = m_pICxE87CMSPtr->UnregisterCallback2(pICxE87Callback2);
		m_pLogData->CheckResult(hResult, L"ICxE87CMS::UnregisterCallback2");
		m_pLogData->PushLogs(L"ICxE87Callback2::Release");
		pICxE87Callback2.Release();
	
		m_pLogData->PushLogs(L"ICxE87CMS::UnregisterCallback");
		hResult = m_pICxE87CMSPtr->UnregisterCallback(pICxE87Callback);
		m_pLogData->CheckResult(hResult, L"ICxE87CMS::UnregisterCallback");
		m_pLogData->PushLogs(L"ICxE87Callback::Release");
		pICxE87Callback.Release();

		m_pLogData->PushLogs(L"ICxE87CMS::UnregisterAsyncCarrierTagCallback");
		hResult = m_pICxE87CMSPtr->UnregisterAsyncCarrierTagCallback(pICxE87AsyncCarrierTagCallback);
		m_pLogData->CheckResult(hResult, L"ICxE87CMS::UnregisterAsyncCarrierTagCallback");
		pICxE87AsyncCarrierTagCallback.Release();

		m_pLogData->PushLogs(L"ICxE87CMS::Shutdown");
		hResult = m_pICxE87CMSPtr->Shutdown();
		m_pLogData->CheckResult(hResult, L"ICxE87CMS::Shutdown");

		m_pLogData->PushLogs(L"ICxE87CMS::Release");
		m_pICxE87CMSPtr.Release();
	}

	CEBase::Close();
}

/*
 desc : 로그 레벨 값 설정
 parm : level	- [in]  로그 레벨 값
 retn : TRUE or FALSE
*/
BOOL CE87CMS::SetLogLevel(ENG_ILLV level)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetLogLevel::CIM87LogLevel level : [%d]", level);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->put_CIM87LogLevel(LONG(level));
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : CIMConnect 초기화 수행 후 완료 작업 진행
 parm : equip_id	- [in]  Equipment ID (0 or later. 환경 파일에 설정된 개수-1)
		conn_id		- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
 retn : TRUE or FALSE
 note : Final initialization of the GEM interface. Until this is called, communication
		with the GEM Host is not permitted. This allows for equipment initialization to
		be completed as well as initialization of CIMConnect and this application. 
*/
BOOL CE87CMS::Finalized(LONG equip_id, LONG conn_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	LONG i;

	LoadPortTransferStateEnum enLPTS;
	LoadPortReservationStateEnum enLPRS;
	LoadPortCarrierAssociationEnum enCIAS;

	for (i=1; i<CIM_LOAD_PORT_COUNT; i++)
	{
		/* Update to load port transfer state machine */
		swprintf_s(tzMesg, CIM_CMD_DESC_SIZE,
				   L"UpdateSharedE87State::get_TransferStatus (port_id=%d)", i);
		m_pLogData->PushLogs(tzMesg);
		hResult	= m_pICxE87CMSPtr->get_TransferStatus(UINT16(i), &enLPTS);
		if (enLPTS != LP_READY_TO_UNLOAD)
		{
			enLPTS	= LP_READY_TO_UNLOAD;
			hResult	= m_pICxE87CMSPtr->put_TransferStatus(UINT16(i), enLPTS);
		}
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
		m_pSharedData->E87_PushState((UINT8)E87_CDSM::en_e87lptsm, (LONG)enLPTS, UINT16(i));

		/* Update to Reservation State */
		swprintf_s(tzMesg, CIM_CMD_DESC_SIZE,
				   L"UpdateSharedE87State::get_ReservationState (port_id=%d)", i);
		m_pLogData->PushLogs(tzMesg);
		hResult	= m_pICxE87CMSPtr->get_ReservationState(UINT16(i), &enLPRS);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
		m_pSharedData->E87_PushState((UINT8)E87_CDSM::en_e87lprsm, (LONG)enLPRS, UINT16(i));

		/* Update to Accessing State */
		swprintf_s(tzMesg, CIM_CMD_DESC_SIZE,
				   L"UpdateSharedE87State::get_AssociationState (carr_id=%d)", i);
		m_pLogData->PushLogs(tzMesg);
		hResult	= m_pICxE87CMSPtr->get_AssociationState(UINT16(i), &enCIAS);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
		m_pSharedData->E87_PushState((UINT8)E87_CDSM::en_e87lpcasm, (LONG)enCIAS, UINT16(i));
	}
	
	return TRUE;
}

/*
 desc : 초기화 (반드시 호출)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE87CMS::InitE87CMS()
{
	HRESULT hResult	= S_FALSE;

	/* E87CMS Callback */
	ICxE87CallbackPtr pICxE87Callback	= m_pEMServiceCbk;
	ICxE87Callback2Ptr pICxE87Callback2	= m_pEMServiceCbk;
	ICxE87Callback3Ptr pICxE87Callback3	= m_pEMServiceCbk;
	ICxE87Callback4Ptr pICxE87Callback4	= m_pEMServiceCbk;
	ICxE87Callback5Ptr pICxE87Callback5	= m_pEMServiceCbk;
	ICxE87Callback6Ptr pICxE87Callback6	= m_pEMServiceCbk;
	ICxE87Callback7Ptr pICxE87Callback7	= m_pEMServiceCbk;
	ICxE87Callback8Ptr pICxE87Callback8	= m_pEMServiceCbk;
	ICxE87Callback9Ptr pICxE87Callback9	= m_pEMServiceCbk;
	ICxE87AsyncCarrierTagCallbackPtr pICxE87AsyncCarrierTagCallback	= m_pEMServiceCbk;
	IUnknownPtr pIUnknownE90;

	if (m_pICxE87CMSPtr && m_pICxE87CMSPtr.GetInterfacePtr())	return FALSE;

	/* Create & Initialize */
	m_pLogData->PushLogs(L"ICxE87CMS::CreateInstance");
	hResult = m_pICxE87CMSPtr.CreateInstance(CLSID_CxE87CMS);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE87CMS::CreateInstance CLSID_CxE87CMS")))	return FALSE;
	m_pLogData->PushLogs(L"ICxE87CMS::Init");
	hResult = m_pICxE87CMSPtr->Init(m_pICxE39ObjPtr, m_pICxAbsLayerPtr);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE87CMS::Init")))	return FALSE;

	/* Link E87 to E90 (to create substrate locations, substrates) */
	pIUnknownE90	= m_pICxE90STPtr;
	m_pLogData->PushLogs(L"ICxE87CMS.SetCIM90Interface");
	hResult = m_pICxE87CMSPtr->SetCIM90Interface(pIUnknownE90);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE87CMS::SetCIM90Interface")))	return FALSE;

	/* Register callback function */
	hResult = m_pICxE87CMSPtr->RegisterAsyncCarrierTagCallback(pICxE87AsyncCarrierTagCallback);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE87CMS::RegisterAsyncCarrierTagCallback")))	return FALSE;
	hResult = m_pICxE87CMSPtr->RegisterCallback(pICxE87Callback);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE87CMS::RegisterCallback")))	return FALSE;
	hResult = m_pICxE87CMSPtr->RegisterCallback2(pICxE87Callback2);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE87CMS::RegisterCallback2")))	return FALSE;
	hResult = m_pICxE87CMSPtr->RegisterCallback3(pICxE87Callback3);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE87CMS::RegisterCallback3")))	return FALSE;
	hResult = m_pICxE87CMSPtr->RegisterCallback4(pICxE87Callback4);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE87CMS::RegisterCallback4")))	return FALSE;
	hResult = m_pICxE87CMSPtr->RegisterCallback5(pICxE87Callback5);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE87CMS::RegisterCallback5")))	return FALSE;
	hResult = m_pICxE87CMSPtr->RegisterCallback6(pICxE87Callback6);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE87CMS::RegisterCallback6")))	return FALSE;
	hResult = m_pICxE87CMSPtr->RegisterCallback7(pICxE87Callback7);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE87CMS::RegisterCallback7")))	return FALSE;
	hResult = m_pICxE87CMSPtr->RegisterCallback8(pICxE87Callback8);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE87CMS::RegisterCallback8")))	return FALSE;
	hResult = m_pICxE87CMSPtr->RegisterCallback9(pICxE87Callback9);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE87CMS::RegisterCallback9")))	return FALSE;

	/* Register the global interface */
	m_pLogData->PushLogs(L"RegisterInterfaceInGlobal", m_tzGUID);
	hResult	= m_pGIT->RegisterInterfaceInGlobal(m_pICxE90STPtr, m_gdICxE, &m_dwCookie);
	if (FAILED(m_pLogData->CheckResult(hResult, L"RegisterInterfaceInGlobal")))	return FALSE;
	m_pSharedData->GetGITCookie()->e87cms_cookie	= m_dwCookie;

	/* GEM Object 포인터 연결 */
	return (hResult == S_OK);
}

/*
 desc : 초기화 (반드시 호출)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE87CMS::InitE87HW()
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	UINT16 i;
	UINT16 u16DockSubID				= 1 /* Exactly 1. Subscript Index */;
	UINT16 u16UndockSubID			= 2 /* Exactly 2. Subscript Index */;
	HRESULT hResult					= S_OK;
	CComBSTR bstrDocked, bstrUndocked;

	/* Initialize the load ports */
	for (i=1;i<=CIM_LOAD_PORT_COUNT;i++,u16DockSubID+=2,u16UndockSubID+=2)
	{
		bstrDocked	= CComBSTR(m_pSharedData->GetLoadPortDockedName(i, 0x00));
		bstrUndocked= CComBSTR(m_pSharedData->GetLoadPortDockedName(i, 0x01));
		swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"ICxE87CMS::Create2LocationLoadPort2 port #%d, sub_id[%d/%d]",
				  i, u16DockSubID, u16UndockSubID);
		m_pLogData->PushLogs(tzMesg);
		hResult	= m_pICxE87CMSPtr->Create2LocationLoadPort2(i, 
														   bstrDocked, u16DockSubID,
														   bstrUndocked, u16UndockSubID);
		if (FAILED(m_pLogData->CheckResult(hResult, tzMesg)))	return FALSE;
	}
	/* 아래 부분은 별도의 외부 함수로 정의. 제어SW에서 현재 Load Port 상태 값 설정하도록 유도 */
	/* For SIMULATION, force the load port transfer state to be READY TO LOAD */
	/* For PRODUCTION, check for a carrier, and set the load port transfer state appropriately */
	for (short port=1;port<=CIM_LOAD_PORT_COUNT;port++)
	{
		LoadPortTransferStateEnum lptsEnum;
		hResult = m_pICxE87CMSPtr->get_TransferStatus(port, &lptsEnum);
		if (LoadPortTransferStateEnum::LP_READY_TO_LOAD != lptsEnum)
		{
			m_pICxE87CMSPtr->put_TransferStatus(port, LoadPortTransferStateEnum::LP_READY_TO_LOAD);
		}
	}

	return (hResult == S_OK);
}

/*
 desc : Carrier로부터 Substrate 제거 (가져오기)
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		slot_no	- [in]  Slot Number (Not sequential number)
 retn : TRUE or FALSE
*/
BOOL CE87CMS::SetCarrierPopSubstrate(PTCHAR carr_id, UINT8 slot_no)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	_bstr_t bstSubstID;
	CComBSTR bstrCarrID(carr_id), bstrSubstID;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetCarrierPopSlot::SubstrateRemoved carrier_id / "
			   L"slot_number : [%s] / [%d]", carr_id, slot_no);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->SubstrateRemoved(bstrCarrID, slot_no, &bstrSubstID); 
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Carrier가 로드 포트 위에 도착 (내려오고;안착하고)하거나 혹은 제거되고 있는 중이다 (?)
 parm : port_id	- [in]  Load Port Number (1 or 2)
 retn : TRUE or FALSE
 note : Load Port 상태를 "Ready To Load" or "Ready To Unload"에서 "Transfer Bloacked"으로 변경 시킴
*/
BOOL CE87CMS::SetTransferStart(UINT16 port_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;
	VARIANT_BOOL varRet	= VARIANT_TRUE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetTransferStart::TransferStart port #%d", port_id);
	m_pLogData->PushLogs(tzMesg);
	/* change the load port state from Ready To Load to Transfer Blocked */
	hResult	= m_pICxE87CMSPtr->TransferStart(port_id, &varRet);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Carrier가 로드 포트 위에 도착이 완료되면 호출 해야 함
 parm : port_id	- [in]  Load Port Number (1 or 2)
 retn : TRUE or FALSE
 note : Load Transfer Complete (Sequnce Name)
		프로젝트 파일 (EPJ) 내에
			"UseCarrierPlaced" 변수에 값이 1로 설정되어 있다면
				CarrierLocationChanged collection event가 자동으로 트리거 되지만
			"UseCarrierPlaced" 변수에 값이 0로 설정되어 있다면
				CarrierLocationChanged collection event는 CarrierAtPort가 호출될 때 트리거 됩니다.
		참고로, 이 함수는 Load Port Reservation State를 변경하지 않고, CarrierAtPort가 변경해 줍니다.
*/
BOOL CE87CMS::SetCarrierPlaced(UINT16 port_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetCarrierPlaced::CarrierPlaced port #%d", port_id);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE87CMSPtr->CarrierPlaced(port_id);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Carrier가 Load Port에서 제거 준비가 되었을 때, Load Port Transfer State를
		LP_READY_TO_UNLOAD로 변경 합니다.
 parm : port_id	- [in]  Load Port Number (1 or 2)
 retn : TRUE or FALSE
 note : AccessMode가 수동 모드일 경우,
			Carrier가 Unloading 중에 undocked 및 unclamped될 때 호출해야 하고
		AccessMode가 자동 모드일 경우, CarrierUnclamped equipment constant value 값이 1일 경우,
			Carrier가 Unloading 중에 아직 Clampled이지만 undocked 일 때 호출해야 합니다.
		AccessMode가 자동 모드일 경우, CarrierUnclamped equipment constant value 값이 0일 경우,
			Carrier가 Unloading 중에 undocked 및 unclamped될 때 호출해야 합니다.
*/
BOOL CE87CMS::SetReadyToUnload(UINT16 port_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;
	VARIANT_BOOL bResult= VARIANT_TRUE;
#ifdef DEBUG
	E87_LPTS enStatus;
	/* LoadPortTransferStateEnum 값이 "LP_READY_TO_UNLOAD" 상태일 때만 호출 유효함 */
	if (!GetTransferStatus(port_id, enStatus))	return FALSE;
	if (enStatus == E87_LPTS::en_lp_ready_to_unload)
	{
		m_pLogData->PushLogs(L"Load port status is LP_READY_TO_UNLOAD", ENG_GALC::en_warning);
		return FALSE;
	}
#endif
	/* Load Port를 Unload 상태로 변경 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetReadyToUnload::ReadyToUnload (port #%d)", port_id);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE87CMSPtr->ReadyToUnload(port_id, &bResult);
	m_pLogData->CheckResult(hResult,  tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Carrier가 로드 포트 위에 올려지고, 센서에 의해 CarrierID가 정상적으로 판독되고 나면 호출 됨
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		port_id	- [in]  Load Port Number (1 or 2)
		loc_id	- [in]  Carrier Location ID 즉, 대부분 Carrier Undocked Name (Carrier Location Name)
 retn : TRUE or FALSE
 note : CarrierID 읽기가 실패하면, carr_id 값이 empty 상태로 입력돼도 상관 없음
		이 때, CIM87 내부적으로 새로운 CarrierID를 생성한다 (?)
		상세한 설명은 도움말 참조 바랍니다.
*/
BOOL CE87CMS::SetCarrierAtPort(PTCHAR carr_id, UINT16 port_id, PTCHAR loc_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CMSVerification cmsVerify;
	/* Carrier ID & Load Port DockedName 얻기 */
	CComBSTR bstrCarrierID (carr_id), bstrUndockedName (loc_id);
	CUniToChar csCnv;
#if 0
	/* 현재 Carrier 상태가 Placed 상태인지 확인 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetCarrierAtPort::Carrier Placed Status : (carr_id=%s / port_id=%d / undocked=%s)",
			   carr_id, port_id, undocked);
#endif
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetCarrierAtPort::CarrierAtPort carrier : (carr_id=%s / port_id=%d / undocked=%s)",
			   carr_id, port_id, loc_id);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE87CMSPtr->CarrierAtPort(port_id, bstrCarrierID, bstrUndockedName,
											VARIANT_TRUE, &cmsVerify);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Carrier가 로드 포트에서 분리 되었다고 CIM87에 알림
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		port_id	- [in]  Load Port Number (1 or 2)
		undocked- [in]  Carrier Undocked Name (Carrier Location Name)
 retn : TRUE or FALSE
*/
BOOL CE87CMS::SetCarrierDepartedPort(PTCHAR carr_id, UINT16 port_id, PTCHAR undocked)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;
	VARIANT_BOOL bResult= VARIANT_TRUE;
	CComBSTR bstrCarrierID (carr_id), bstrUndockedName (undocked);
	CUniToChar csCnv;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetCarrierDepartedPort::CarrierDepartedPort carrier : (carr_id=%s / port_id=%d / undocked=%s)",
			   carr_id, port_id, undocked);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE87CMSPtr->CarrierDepartedPort(port_id, bstrCarrierID, bstrUndockedName,
												  VARIANT_FALSE, &bResult);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Carrier가 새로운 위치로 이동 했다고 알림
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		port_id	- [in]  Load Port Number (1 or 2)
		docked	- [in]  Carrier Docked Name (Carrier Location Name)
 retn : TRUE or FALSE
*/
BOOL CE87CMS::SetMoveCarrier(PTCHAR carr_id, UINT16 port_id, PTCHAR docked)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrCarrierID (carr_id), bstrDockedName (docked);
	CUniToChar csCnv;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetMoveCarrier::CarrierPlaced carrier : (carr_id=%s / port_id=%d / docked=%s)",
			   carr_id, port_id, docked);
	m_pLogData->PushLogs(tzMesg);
#if 0
	hResult = m_pICxE87CMSPtr->MoveCarrier(bstrCarrierID, bstrDockedName, VARIANT_TRUE);
#else
	hResult = m_pICxE87CMSPtr->MoveCarrier3(bstrCarrierID, bstrDockedName);
#endif
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Carrier 내에 저장 (설치)된 Slot Map에 대해 Verification 처리
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		slots	- [in]  Wafer가 Slot Map에 적재된 상태 정보
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
BOOL CE87CMS::SetVerifySlotMap(PTCHAR carr_id, CAtlList <E87_CSMS> *slots, LONG handle)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	UINT8 u8SlotMap	= 0x00;	/* Undefined */
	HRESULT hResult	= S_OK;
	POSITION pPos	= NULL;
	CComBSTR bstrCarrierID (carr_id);
	ICxValueDispPtr vSlotMap(CLSID_CxValueObject);
	CMSVerification cmsVerif;

	/* 등록된 개수만큼 읽어와서 Slot Map 정보 설정 */
	pPos	= slots->GetHeadPosition();
	while (pPos)
	{
		u8SlotMap	= (UINT8)slots->GetNext(pPos);
		vSlotMap->AppendValueU1(handle, u8SlotMap/*SlotStatusEnum*/);
	}

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetVerifySlotMap::VerifySlotMap : carr_id=%s, handle=%d", carr_id, handle);
	m_pLogData->PushLogs(tzMesg);
	/* 내부 에러가 발생하면 Callback 함수가 호출 (?) */
	hResult = m_pICxE87CMSPtr->VerifySlotMap(bstrCarrierID, vSlotMap, &cmsVerif);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Carrier의 접근 상태 변경
 parm : conn_id	- [in]  연결된 장비 ID (Host ID), Common의 경우, 무조건 0 값
		carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		port_id	- [in]  Load Port Number (1 or 2)
		status	- [in]  Carrier accessing status (E87_CCAS)
						0x00 : 아직 Carrier에 접근하지 못한 상태
						0x01 : Carrier에 접근하고 있는 상태
						0x02 : 장비 (Equipment)가 정상적으로 Carrier에 접근이 끝난 상태
						0x03 : 장비 (Equipment)가 비정상적으로 Carrier에 접근이 끝난 상태
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
BOOL CE87CMS::SetAccessCarrier(LONG conn_id, PTCHAR carr_id, UINT16 port_id,
							   E87_CCAS status, LONG handle)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrCarrID(carr_id);
	VARIANT_BOOL vbResult; 

	/* When the last substrate is replaced in the carrier... */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetAccessCarrier::AccessCarrier port Carrier Accessing Status : conn_id=%d, "
			   L"carrier_id=%s, port_id=%d, access=%d, handle=%d",
			   conn_id, carr_id, port_id, status, handle);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->AccessCarrier(bstrCarrID, AccessingStatusEnum(status), &vbResult); 
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (vbResult == VARIANT_TRUE && hResult == S_OK);
}

/*
 desc : Carrier Status Event 설정
 parm : conn_id	- [in]  연결된 장비 ID (Host ID), Common의 경우, 무조건 0 값
		carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		port_id	- [in]  Load Port Number (1 or 2)
		status	- [in]  Carrier Event Value (E87_CCES)
 retn : TRUE or FALSE
*/
BOOL CE87CMS::SetCarrierEvent(LONG conn_id, PTCHAR carr_id, UINT16 port_id, E87_CCES status)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	TCHAR tzStatus[64]	= {NULL};
	HRESULT hResult		= S_OK;
	CComBSTR bstrCarrID(carr_id);
	CComSafeArray<BSTR> dataNames;
	ICxValueDispPtr dataValues(CLSID_CxValueObject);

	switch (status)
	{
	case E87_CCES::en_carrier_closed			:	wcscpy_s(tzStatus, 64, L"CarrierClosed");					break;
	case E87_CCES::en_carrier_opened			:	wcscpy_s(tzStatus, 64, L"CarrierOpened");					break;
	case E87_CCES::en_carrier_clamped		:	wcscpy_s(tzStatus, 64, L"CarrierClamped");					break;
	case E87_CCES::en_carrier_unclamped		:	wcscpy_s(tzStatus, 64, L"CarrierUnclamped");				break;
	case E87_CCES::en_carrier_approached		:	wcscpy_s(tzStatus, 64, L"CarrierApproachingComplete");		break;
	case E87_CCES::en_carrier_process_changed:	wcscpy_s(tzStatus, 64, L"CarrierProcessingStatusChanged");	break;
	case E87_CCES::en_carrier_id_read_enable	:	wcscpy_s(tzStatus, 64, L"IDReaderAvailable");				break;
	case E87_CCES::en_carrier_id_read_disable:	wcscpy_s(tzStatus, 64, L"IDReaderUnavailable");				break;
	case E87_CCES::en_carrier_id_unknown		:	wcscpy_s(tzStatus, 64, L"UnknownCarrierID");				break;
	case E87_CCES::en_carrier_id_duplicate	:	wcscpy_s(tzStatus, 64, L"DuplicateCarrierIDInProcess");		break;
	default									:	wcscpy_s(tzStatus, 64, L"Undefined");						break;
	}

	/* Close the carrier */
	dataNames.Add(CComBSTR(L"PortID"));
	dataNames.Add(CComBSTR(L"CarrierID"));
	dataValues->AppendValueU1(0, UINT8(port_id));	/* PortID */
	dataValues->AppendValueAscii(0, bstrCarrID );	/* CarrierID */
	hResult	= SendCollectionEventWithData(conn_id, dataNames, dataValues, tzStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Substrate를 통해서 ICxSubstratePtr 포인터 얻기
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		slot_no	- [in]  Slot Number (Not sequential number)
		unknown	- [out] IUnknownPtr 반환될 인터페이스 포인터
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
BOOL CE87CMS::GetSubstratePtr(PTCHAR carr_id, UINT8 slot_no,
							  ICxSubstratePtr &st_ptr)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrSubstID;
	IUnknownPtr pIUnknownPtr;

	/* Get the substrate object */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"GetSubstratePtr::GetSubstrate : carr_id=%s, slot_no=%d",
				carr_id, slot_no);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->GetSubstrate(CComBSTR(carr_id), (LONG)slot_no, &pIUnknownPtr);  
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		return FALSE;
	}
	if (!pIUnknownPtr)
	{
		m_pLogData->PushLogs(L"Substrate in the process job not found. Skipping");
		return FALSE;
	}
	/* 값 반환 */
	st_ptr	= pIUnknownPtr;

	return TRUE;
}

/*
 desc : SubstrateFromSource를 통해서 ICxSubstratePtr 포인터 얻기
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		slot_no	- [in]  Slot Number (Not sequential number)
		unknown	- [out] IUnknownPtr 반환될 인터페이스 포인터
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
BOOL CE87CMS::GetSubstratePtrFromSource(PTCHAR carr_id, UINT8 slot_no,
										ICxSubstratePtr &st_ptr)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrSubstID;
	IUnknownPtr pIUnknownPtr;

	/* Get the substrate object */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"GetSubstratePtrFromSource::GetSubstrateFromSource : carr_id=%s, slot_no=%d",
				carr_id, slot_no);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->GetSubstrateFromSource(CComBSTR(carr_id), (LONG)slot_no, &pIUnknownPtr);  
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		return FALSE;
	}
	if (!pIUnknownPtr)
	{
		m_pLogData->PushLogs(L"Substrate in the process job not found. Skipping");
		return FALSE;
	}
	/* 값 반환 */
	st_ptr	= pIUnknownPtr;

	return TRUE;
}

/*
 desc : 현재 Load Port ID의 Reservation State 정보 반환
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [out] Reservation Status 반환
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
BOOL CE87CMS::GetReservationState(UINT16 port_id, E87_LPRS &state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	LoadPortReservationStateEnum pVal;

	if (!m_pICxE87CMSPtr)	return FALSE;
	/* Get the substrate object */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"GetReservationStatus::get_ReservationState : port_id=%d", port_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->get_ReservationState(port_id, &pVal);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* 값 반환 */
	state	= E87_LPRS(pVal);

	return (hResult == S_OK);
}

/*
 desc : 현재 Load Port ID의 Reservation State 정보 설정
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [in]  Reservation Status 반환
 retn : TRUE or FALSE
*/
BOOL CE87CMS::SetReservationState(UINT16 port_id, E87_LPRS state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;

	if (!m_pICxE87CMSPtr)	return FALSE;
	/* Get the substrate object */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetReservationStatus::put_ReservationState : port_id=%d", port_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->put_ReservationState(port_id, LoadPortReservationStateEnum(state));
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : 현재 Load Port-Carrier의 Association State 정보 반환
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [out] Load Port-Carrier Association State 반환
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
BOOL CE87CMS::GetAssociationState(UINT16 port_id, E87_LPAS &state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	LoadPortCarrierAssociationEnum pVal;

	if (!m_pICxE87CMSPtr)	return FALSE;
	/* Load port carrier association state */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"GetAssociationState::get_AssociationState : port_id=%d", port_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->get_AssociationState(port_id, &pVal);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* 값 반환 */
	state	= E87_LPAS(pVal);

	return (hResult == S_OK);
}

/*
 desc : 현재 Load Port-Carrier의 Association State 정보 설정
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [in]  Load Port-Carrier Association State
 retn : TRUE or FALSE
*/
BOOL CE87CMS::SetAssociationState(UINT16 port_id, E87_LPAS state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;

	if (!m_pICxE87CMSPtr)	return FALSE;
	/* Load port carrier association state */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetAssociationState::put_AssociationState : port_id=%d", port_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->put_AssociationState(port_id, LoadPortCarrierAssociationEnum(state));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : 현재 Carrier의 Access Mode Status 정보 반환
		Load Port가 수동으로 접근하는지 혹은 자동으로 접근하는지 상태 값 반환
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [out] Manual (0) or Auto (1)
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
BOOL CE87CMS::GetAccessModeState(UINT16 port_id, E87_LPAM &state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	LoadPortAccessModeStateEnum pVal;

	if (!m_pICxE87CMSPtr)	return FALSE;
	/* Load port carrier association state */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"GetAccessModeState::get_AccessModeState : port_id=%d", port_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->get_AccessModeState(port_id, &pVal);
	m_pLogData->CheckResult(hResult, tzMesg);

	/* 값 반환 */
	state	= E87_LPAM(pVal);

	return (hResult == S_OK);
}

/*
 desc : 현재 Carrier의 Access Mode Status 값 설정
		Load Port가 수동으로 접근하는지 혹은 자동으로 접근하는지 상태 값 설정
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [in]  Manual (0) or Auto (1)
 retn : TRUE or FALSE
*/
BOOL CE87CMS::SetAccessModeState(UINT16 port_id, E87_LPAM state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;

	if (!m_pICxE87CMSPtr)	return FALSE;
	/* Load port carrier association state */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetAccessModeState::put_AccessModeState : port_id=%d, state=%d", port_id, state);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->put_AccessModeState(port_id, LoadPortAccessModeStateEnum(state));
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Carrier ID Verification State 값 반환
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		status	- [out] Carrier ID Verfication 상태 값 반환
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
BOOL CE87CMS::GetCarrierIDStatus(PTCHAR carr_id, E87_CIVS &status)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	IDStatusEnum pVal;
	CComBSTR bstrCarrID (carr_id);

	if (!m_pICxE87CMSPtr)		return FALSE;
	/* Carrier ID Check */
	if (!carr_id)				return FALSE;
	if (wcslen(carr_id) < 1)	return FALSE;
	/* Get the substrate object */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"GetCarrierIDStatus::get_CarrierIDStatus : carr_id=%s", carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->get_CarrierIDStatus(bstrCarrID, &pVal);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* 값 반환 */
	status	= E87_CIVS(pVal);

	return (hResult == S_OK);
}

/*
 desc : Carrier ID Verification State 값 설정
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		state	- [in]  Carrier ID Verfication 상태 값
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
BOOL CE87CMS::SetCarrierIDStatus(PTCHAR carr_id, E87_CIVS status)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrCarrID (carr_id);

	if (!m_pICxE87CMSPtr)		return FALSE;
	/* Carrier ID Check */
	if (!carr_id)				return FALSE;
	if (wcslen(carr_id) < 1)	return FALSE;
	/* Get the substrate object */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetCarrierIDStatus::put_CarrierIDStatus : carr_id=%s", carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->put_CarrierIDStatus(bstrCarrID, IDStatusEnum(status));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : The slot map status of the specified carrier.
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		status	- [out] Slot map status 반환
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
 note : The slot map status shall be read on all production
		equipment prior to removal of substrates from the carrier
*/
BOOL CE87CMS::GetCarrierIDSlotMapStatus(PTCHAR carr_id, E87_SMVS &status)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	IDStatusEnum enIDStatus;
	SlotMapStatusEnum enSlotMapStatus;

	if (!m_pICxE87CMSPtr)		return FALSE;
	/* Carrier ID Check */
	if (!carr_id)				return FALSE;
	if (wcslen(carr_id) < 1)	return FALSE;
	/* Carrier ID Verification OK인지 확인 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"GetCarrierIDSlotMapStatus::get_CarrierIDStatus carrier_id : [%s]", carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->get_CarrierIDStatus(CComBSTR(carr_id), &enIDStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* Carrier ID Status가 Verification OK인지 확인 */
	if (enIDStatus != ID_VERIFICATION_OK)
	{
		m_pLogData->PushLogs(L"GetCarrierIDSlotMapStatus::Carrier ID Status != ID_VERIFICATION_OK");
		return FALSE;
	}
	/* Get the substrate object */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"GetCarrierIDSlotMapStatus::get_SlotMapStatus carrier_id : [%s]", carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->get_SlotMapStatus(CComBSTR(carr_id), &enSlotMapStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* 값 반환 */
	status	= E87_SMVS(enSlotMapStatus);

	return (hResult == S_OK);
}

/*
 desc : The slot map status of the specified carrier.
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		status	- [in]  Slot map status
 retn : TRUE or FALSE
 note : The slot map status shall be read on all production
		equipment prior to removal of substrates from the carrier
*/
BOOL CE87CMS::SetCarrierIDSlotMapStatus(PTCHAR carr_id, E87_SMVS status)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	IDStatusEnum enIDStatus;

	if (!m_pICxE87CMSPtr)		return FALSE;
	/* Carrier ID Check */
	if (!carr_id)				return FALSE;
	if (wcslen(carr_id) < 1)	return FALSE;
	/* Carrier ID Verification OK인지 확인 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"SetCarrierIDSlotMapStatus::put_CarrierIDStatus carrier_id : [%s]", carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->get_CarrierIDStatus(CComBSTR(carr_id), &enIDStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* Carrier ID Status가 Verification OK인지 확인 */
	if (enIDStatus != ID_VERIFICATION_OK)
	{
		m_pLogData->PushLogs(L"GetCarrierIDSlotMapStatus::Carrier ID Status != ID_VERIFICATION_OK");
		return FALSE;
	}
	/* Get the substrate object */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"SetCarrierIDSlotMapStatus::put_SlotMapStatus carrier_id : [%s]", carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->put_SlotMapStatus(CComBSTR(carr_id), SlotMapStatusEnum(status));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : 원격 제어 허용 여부 설정
 parm : enable	- [in]  TRUE or FALSE
 retn : TRUE or FALSE
 note : The following messages will be rejected with ERRCODE = 17 = "Command not valid in current state"
		and ERRTEXT = "Invalid in local mode"
		S3F17 — Carrier Action Request
		S3F19 — Cancel All Carrier Out Requests
		S3F23 — PortGroup Action Acknowledge
		S3F25 — Port Action Request
		S3F27 — Change Access
		S3F29 — Carrier Tag Read Request
		S3F31 — Carrier Tag Write Data Request
*/
BOOL CE87CMS::SetAllowRemoteControl(BOOL enable)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	VARIANT_BOOL boEnable = enable == TRUE ? VARIANT_TRUE : VARIANT_FALSE;

	if (!m_pICxE87CMSPtr)	return FALSE;
	/* Get the substrate object */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetAllowRemoteControl::put_AllowRemoteControl enable : [%d]", enable);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->put_AllowRemoteControl(boEnable);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : 원격 제어 허용 여부 반환
 parm : enable	- [out] TRUE or FALSE
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
BOOL CE87CMS::GetAllowRemoteControl(BOOL &enable)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult			= S_OK;
	VARIANT_BOOL boEnable	= VARIANT_FALSE;

	if (!m_pICxE87CMSPtr)	return FALSE;
	/* Get the substrate object */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"GetAllowRemoteControl::get_AllowRemoteControl");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->get_AllowRemoteControl(&boEnable);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	enable	= VARIANT_BOOL(boEnable) == VARIANT_TRUE;

	return (hResult == S_OK);
}

/*
 desc : ProceedWithCarrier service 초기화 (아래 2가지 목적의 경우에 호출 됩니다)
		1. Carrier ID를 Read한 후에 승인할 때, 호출됨
		2. SlotMap을 Read한 후에 승인할 때, 호출됨
 parm : port_id	- [in]  Load Port ID (1 or 2)
		carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
 retn : TRUE or FALSE
 note : 장비 (Equipment)가 ProceedWithCarrier service를 초기화하도록 허락 합니다.
*/
BOOL CE87CMS::SetProceedWithCarrier(UINT16 port_id, PTCHAR carr_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrCarrID (carr_id);
	ICxValueDispPtr valParamList(CLSID_CxValueObject);
	ICxValueDispPtr valStatus(CLSID_CxValueObject);

	if (!m_pICxE87CMSPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetProceedWithCarrier::ProceedWithCarrier: port_id=%d, carr_id=%s", port_id, carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->ProceedWithCarrier(port_id, bstrCarrID, valParamList, &valStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		/* 에러 코드 및 문자열 정보 출력 */
		DispErrorData(valStatus, E30_SVET::en_caack);
		return FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : CancelCarrier service 요청
 parm : port_id	- [in]  Load Port ID (1 or 2)
		carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
 retn : TRUE or FALSE
*/
BOOL CE87CMS::SetCancelCarrier(UINT16 port_id, PTCHAR carr_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrCarrID (carr_id);
	ICxValueDispPtr valStatus(CLSID_CxValueObject);

	if (!m_pICxE87CMSPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetCancelCarrier::CancelCarrier: port_id=%d, carr_id=%s", port_id, carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->CancelCarrier(port_id, bstrCarrID, &valStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		/* 에러 코드 및 문자열 정보 출력 */
		DispErrorData(valStatus, E30_SVET::en_caack);
		return FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : CancelCarrierNotification service request
 parm : carr_id	- [in]  Carrier ID (or Name; String)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelCarrierNotification service
		E_FAIL will be returned if the call fails. The "status" will contain the text of any error
		and is in the form of the S3F18 reply message. Callbacks will be called.
*/
BOOL CE87CMS::SetCancelCarrierNotification(PTCHAR carr_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	ICxValueDispPtr valStatus(CLSID_CxValueObject);

	if (!m_pICxE87CMSPtr)	return FALSE;
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetCancelCarrierNotification::CancelReserveAtPort: carr_id=%s", carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->CancelCarrierNotification(CComBSTR(carr_id), &valStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		DispErrorData(valStatus, E30_SVET::en_caack);
		return FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : CarrierReCreate service 요청
 parm : port_id	- [in]  Load Port ID (1 or 2)
		carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
 retn : TRUE or FALSE
*/
BOOL CE87CMS::SetCarrierReCreate(UINT16 port_id, PTCHAR carr_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	E87_LPTS enStatus;
	CComBSTR bstrCarrID (carr_id);
	ICxValueDispPtr valParam(CLSID_CxValueObject);
	ICxValueDispPtr valStatus(CLSID_CxValueObject);

	if (!m_pICxE87CMSPtr)	return FALSE;

	/* 현재 Load Port 상태가 READY_TO_UNLOAD인지 여부 확인 */
	if (!GetTransferStatus(port_id, enStatus))	return FALSE;
	/* READY_TO_UNLOAD 상태인지 꼭 확인 */
	if (enStatus != E87_LPTS::en_lp_ready_to_unload)
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				   L"SetCarrierReCreate::TransferStatus=%d (Not in READY_TO_UNLOAD state): port_id=%d, carr_id=%s",
				   enStatus, port_id, carr_id);
		m_pLogData->PushLogs(tzMesg);
		return FALSE;
	}

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetCarrierReCreate::CarrierReCreate: port_id=%d, carr_id=%s", port_id, carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->CarrierReCreate(bstrCarrID, port_id, valParam, &valStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		/* 에러 코드 및 문자열 정보 출력 */
		DispErrorData(valStatus, E30_SVET::en_caack);
		return FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : ReleaseCarrier service 요청
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
 retn : TRUE or FALSE
*/
BOOL CE87CMS::SetCarrierRelease(PTCHAR carr_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrCarrID (carr_id), bstrLocID;
	ICxValueDispPtr valStatus(CLSID_CxValueObject);
	ICxE87CarrierPtr pICxCarrier;

	if (!m_pICxE87CMSPtr)	return FALSE;

	/* ICxE87Carrier 인터페이스 얻기  */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetCarrierRelease::LocateCarrier: carr_id=%s", carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->LocateCarrier(bstrCarrID, &pICxCarrier);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Location ID 구하기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetCarrierRelease::get_Location: carr_id=%s", carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxCarrier->get_Location(&bstrLocID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetCarrierRelease::CarrierRelease: carr_id=%s", carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->CarrierRelease(bstrCarrID, bstrLocID, &valStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		/* 에러 코드 및 문자열 정보 출력 */
		DispErrorData(valStatus, E30_SVET::en_caack);
		return FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : Get the slot map
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		status	- [out] Slot status가 반환되어 저장될 리스트 참조 변수
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
 note : Slot map is an ordered list of n SlotStatusEnum values, one for each slot in the Carrier,
		where n equals the Carrier's Capacity value.
		Format is L {U1 SlotStatusEnum1} ... {U1 SlotStatusEnumn}. 
*/
BOOL CE87CMS::GetCarrierIDSlotStatus(PTCHAR carr_id, CAtlList <E87_CSMS> &status)
{
	UINT8 u8Status	= 0x00;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	LONG i, lCount	= 0/*, lHandle*/;
	HRESULT hResult	= S_OK;
	CComBSTR bstrCarrID (carr_id), bstrLocID;
	ICxValueDispPtr valStatus(CLSID_CxValueObject);
	ICxE87CarrierPtr pICxCarrier;

	if (!m_pICxE87CMSPtr)	return FALSE;

	/* ICxE87Carrier 인터페이스 얻기  */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetCarrierIDSlotStatus::LocateCarrier: carr_id=%s", carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->LocateCarrier(bstrCarrID, &pICxCarrier);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Location ID 구하기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetCarrierIDSlotStatus::get_Location: carr_id=%s", carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxCarrier->get_Location(&bstrLocID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	if (!m_pICxE87CMSPtr)	return FALSE;
	/* Get the slot map */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"GetCarrierIDSlotStatus::get_SlotMap carrier_id : [%s]", carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxCarrier->get_SlotMap(&valStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* slotStatus 개수 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"GetCarrierIDSlotStatus::get_SlotMap carrier_id : [%s]", carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= valStatus->ItemCount(0, &lCount);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	for (i=1; i<=lCount; i++)
	{
#if 0
		hResult	= valStatus->ListAt(0, i, &lHandle);
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetCarrierIDSlotStatus::ListAt (0, %d, handle)", i);
		m_pLogData->PushLogs(tzMesg);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		hResult	= valStatus->GetValueU1(lHandle, 1, &u8Status);
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetCarrierIDSlotStatus::ListAt (0, %d, handle)", i);
		m_pLogData->PushLogs(tzMesg);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
#else
		hResult	= valStatus->GetValueU1(0, i, &u8Status);
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetCarrierIDSlotStatus::ListAt (0, %d)", i);
		m_pLogData->PushLogs(tzMesg);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		/* 값 등록 */
		status.AddTail(E87_CSMS(u8Status));
#endif
	}

	return (hResult == S_OK);
}

/*
 desc : Add Carrier Location service 요청
 parm : loc_id	- [in]  Load Port 혹은 Buffer가 아닌 다른 곳을 추가하고자 하는 ID (이름. String)
 retn : TRUE or FALSE
*/
BOOL CE87CMS::SetAddCarrierLocation(PTCHAR loc_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrLocID (loc_id);

	if (!m_pICxE87CMSPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetAddCarrierLocation::AddCarrierLocation: loc_id=%s", loc_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->AddCarrierLocation(bstrLocID);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Load Port에 부여 (설정; 관련)된 Carrier ID (Name) 반환
 parm : port_id	- [in]  Load Port ID (1 or 2)
		carr_id	- [out] Carrier ID가 반환되어 저장될 버퍼 포인터
		size	- [in]  'carr_id' 버퍼 크기
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
BOOL CE87CMS::GetAssociationCarrierID(UINT16 port_id, PTCHAR carr_id, UINT32 size)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrCarrID;

	if (!m_pICxE87CMSPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"GetAssociationCarrierID::get_AssociationCarrierID: port_id=%d, carr_id=%s", port_id, carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->get_AssociationCarrierID(port_id, &bstrCarrID);
	m_pLogData->CheckResult(hResult, tzMesg);

	/* Carrier ID 값 취득 했는지 여부 */
	if (bstrCarrID.Length() < 1)	return FALSE;
	/* 값 반환 */
	swprintf_s(carr_id, size, L"%s", (LPTSTR)_bstr_t(bstrCarrID));

	return (hResult == S_OK);
}

/*
 desc : Load Port에 부여 (설정; 관련)된 Carrier ID (Name) 반환
 parm : port_id	- [in]  Load Port ID (1 or 2)
		carr_id	- [out] Carrier ID가 반환되어 저장될 버퍼 포인터
 retn : TRUE or FALSE
*/
BOOL CE87CMS::SetAssociationCarrierID(UINT16 port_id, PTCHAR carr_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;

	if (!m_pICxE87CMSPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"SetAssociationCarrierID::put_AssociationCarrierID: port_id=%d, carr_id=%s", port_id, carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->put_AssociationCarrierID(port_id, CComBSTR(carr_id));
	m_pLogData->CheckResult(hResult, tzMesg);


	return (hResult == S_OK);
}

/*
 desc : Carrier ID에 부여 (설정; 관련)된 Load Port Number 반환
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		port_id	- [Out] Load Port ID (1 or 2)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
BOOL CE87CMS::GetAssociationPortID(PTCHAR carr_id, UINT16 &port_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	INT16 i16PortID	= 0;
	HRESULT hResult	= S_OK;
	CComBSTR bstrCarrID (carr_id);

	if (!m_pICxE87CMSPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"GetAssociationPortID::get_AssociationPortID: carr_id=%s", carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->get_AssociationPortID(bstrCarrID, &i16PortID);
	m_pLogData->CheckResult(hResult, tzMesg);

	/* 값 반환 */
	port_id	= i16PortID < 1 ? 0 : i16PortID;

	return (hResult == S_OK);
}

/*
 desc : Carrier ID에 Load Port Number 부여
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		port_id	- [in]  Load Port ID (1 or 2)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
BOOL CE87CMS::SetAssociationPortID(PTCHAR carr_id, UINT16 port_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrCarrID (carr_id);

	if (!m_pICxE87CMSPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"SetAssociationPortID::put_AssociationPortID: carr_id=%s, port_id=%d", carr_id, port_id);
	m_pLogData->PushLogs(tzMesg);

	hResult	= m_pICxE87CMSPtr->put_AssociationPortID(bstrCarrID, port_id);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : 장치(CMPS)가 Carrier Tag 읽고 난 후, Host에게 S3F30 메시지를 호출하기 위해 설정
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		tag_id	- [in]  The requested tag data, can be null if a tag is empty or fail to read
		trans_id- [in]  The transaction ID of S3F29 (Carrier Tag Read Request)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
 note : uvCIMLib_E87CMS_SetCarrierTagReadData 호출되고 난 후, Host에게 메시지를 보낼 때, 호출해야 함
*/
BOOL CE87CMS::SetAsyncCarrierTagReadComplete(PTCHAR carr_id, PTCHAR tag_id, LONG trans_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	INT16 i16PortID	= 0;
	HRESULT hResult	= S_OK;
	CComBSTR bstrCarrID (carr_id), bstrTagID (tag_id);
	ICxValueDispPtr valStatus(CLSID_CxValueObject);

	if (!m_pICxE87CMSPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"SetAsyncCarrierTagReadComplete::AssociationCarrierID: carr_id=%s, tag_id=%s, trans_id=%d",
				carr_id, tag_id, trans_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->AsyncCarrierTagReadComplete(bstrCarrID, bstrTagID, trans_id, valStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		/* 에러 코드 및 문자열 정보 출력 */
		DispErrorData(valStatus, E30_SVET::en_caack);
		return FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : Carrier로부터 읽은 Tag Data 반환
 parm : loc_id	- [in]  Location identifier (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		carr_id	- [in]  Carrier ID (or Name; String)
		data_seg- [in]  DataSeg 파리미터 값이 반환될 버퍼
		data_tag- [out] Carrier로부터 읽어들인 Tag Data가 저장될 버퍼
		size	- [in]  'data_tag' 버퍼 크기
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
 note : loc_id or carr_id 중 1개만 설정되어 있어야 함. 즉, 둘 중에 1개는 NULL이어도 상관 없음
*/
BOOL CE87CMS::GetCarrierTagReadData(PTCHAR loc_id, PTCHAR carr_id, PTCHAR data_seg,
									PTCHAR data_tag, UINT32 size)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	INT16 i16PortID	= 0;
	LONG lDataLen	= (LONG)wcslen(data_seg);
	HRESULT hResult	= S_OK;
	CComBSTR bstrCarrID, bstrLocID;
	CComBSTR bstrDataSeg (data_seg), bstrDataTag;
	ICxValueDispPtr valStatus(CLSID_CxValueObject);

	if (!loc_id || !carr_id)	return FALSE;
	if (!m_pICxE87CMSPtr)		return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"GetCarrierTagReadData::CarrierTagReadData: loc_id=%s, carr_id=%s", loc_id, carr_id);
	m_pLogData->PushLogs(tzMesg);

	bstrCarrID	= CComBSTR(carr_id);
	bstrLocID	= CComBSTR(loc_id);
	hResult	= m_pICxE87CMSPtr->CarrierTagReadData(bstrLocID, bstrCarrID, bstrDataSeg, lDataLen,
												 &bstrDataTag, &valStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	swprintf_s(data_tag, size, L"%s", (LPTSTR)_bstr_t(bstrDataTag));

	return (hResult == S_OK);
}

/*
 desc : Carrier가 임의 (특정) Load Port에서 제거가 시작되고 있다고 알림
		Load Port에 Carrier가 있어야만 가능
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		port_id	- [in]  Load Port ID (1 or 2)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
 note : This method allows the equipment to begin processing a queued carrier out service.
		If the load port ID and carrier ID are verified to exist, and the load port is available,
		then the carrier is associated to the load port and the load port is reserved.
		The load port may be in LP_READY_TO_LOAD or LP_TRANSFER_BLOCKED as long as
		it is NOT_ASSOCIATED. This function sets the load port to ASSOCIATED and RESERVED.
		The function will return E_FAIL if the load port does not exist or is ASSOCIATED or
		LP_READY_TO_UNLOAD, or the carrier does not exist.
*/
BOOL CE87CMS::SetBeginCarrierOut(PTCHAR carr_id, UINT16 port_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrCarrID (carr_id);

	if (!m_pICxE87CMSPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetBeginCarrierOut::BeginCarrierOut: carr_id=%s, port_id=%d",
			   carr_id, port_id);
	m_pLogData->PushLogs(tzMesg);

	hResult	= m_pICxE87CMSPtr->BeginCarrierOut(port_id, bstrCarrID);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Carrier가 임의 (특정) Load Port에서 제거 되었다고 알림
		Load Port에 Carrier가 있어야만 가능
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		port_id	- [in]  Load Port ID (1 or 2)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CarrierOut service.
		E_FAIL will be returned if the call fails.
		The "status" will contain the text of any error and
		is in the form of the S3F18 reply message. Callbacks will be called.
*/
BOOL CE87CMS::SetCarrierOut(PTCHAR carr_id, UINT16 port_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrCarrID (carr_id);
	ICxValueDispPtr valStatus(CLSID_CxValueObject);

	if (!m_pICxE87CMSPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetCarrierOut::CarrierOut: carr_id=%s, port_id=%d",
			   carr_id, port_id);
	m_pLogData->PushLogs(tzMesg);

	hResult	= m_pICxE87CMSPtr->CarrierOut(port_id, bstrCarrID, &valStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		/* 에러 코드 및 문자열 정보 출력 */
		DispErrorData(valStatus, E30_SVET::en_caack);
		return FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : Carrier가 Load Port에서 내부 버퍼 위치로 이동 되었다고 설정
		즉, Load Port의 Undocked에서 Docked 위치로 이동 했다고 보면 됨
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
 note : This service shall cause a carrier to be moved from a load port to an internal buffer location.
		Used in anomaly situations.
		This API allows the equipment application to start a CarrierIn service 
		without actually receiving a service request from the host.
		The CarrierIn service is only used by the host to request the internal buffer equipment to
		internalize a carrier that has been moved to the load port via a previous CarrierOut service
*/
BOOL CE87CMS::SetCarrierIn(PTCHAR carr_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrCarrID (carr_id);
	ICxValueDispPtr valStatus(CLSID_CxValueObject);

	if (!m_pICxE87CMSPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetCarrierIn::CarrierIn: carr_id=%s", carr_id);
	m_pLogData->PushLogs(tzMesg);

	hResult	= m_pICxE87CMSPtr->CarrierIn(bstrCarrID, &valStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		/* 에러 코드 및 문자열 정보 출력 */
		DispErrorData(valStatus, E30_SVET::en_caack);
		return FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : 모든 CarrierOut Service를 Queue에서 제거
		This service shall cause all CarrierOut services to be removed from the queue.
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelAllCarrierOut service.
		E_FAIL will be returned if the call fails.
		The "status" will contain the text of any error and is in the form of the S3F18 reply message.
		Callbacks will be called.
*/
BOOL CE87CMS::SetCancelAllCarrierOut()
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	ICxValueDispPtr valStatus(CLSID_CxValueObject);

	if (!m_pICxE87CMSPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetCancelAllCarrierOut::CancelAllCarrierOut");
	m_pLogData->PushLogs(tzMesg);

	hResult	= m_pICxE87CMSPtr->CancelAllCarrierOut(&valStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		/* 에러 코드 및 문자열 정보 출력 */
		DispErrorData(valStatus, E30_SVET::en_caack);
		return FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : Load Port의 Transfer Status 값 반환
 parm : port_id	- [in]  Load Port ID (1 or 2)
		status	- [out] Load Port의 Transfer Status 값 반환
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
 note : Get the current load port transfer state
*/
BOOL CE87CMS::GetTransferStatus(UINT16 port_id, E87_LPTS &status)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	LoadPortTransferStateEnum enStatus;

	if (!m_pICxE87CMSPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetTransferStatus::get_TransferStatus: port_id=%d", port_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->get_TransferStatus(port_id, &enStatus);
	m_pLogData->CheckResult(hResult, tzMesg);

	/* 값 반환 */
	status	= E87_LPTS(enStatus);

	return (hResult == S_OK);
}

/*
 desc : Load Port의 Transfer Status 값 설정
 parm : port_id	- [in]  Load Port ID (1 or 2)
		status	- [in]  Load Port의 Transfer Status 값
 retn : TRUE or FALSE
 note : Sets the Load Port Transfer State for the specified load port.
		The new value can be any state and does not depend on the current state
		!!! important. 수동으로 상태 변경을 해줘야지, 내부적으로 알아서 변경해 주지 않음
		!!! 만약, Load Port에 Carrier가 없다면, 반드시 READY_TO_LOAD로 설정해 주어야 됨
*/
BOOL CE87CMS::SetTransferStatus(UINT16 port_id, E87_LPTS status)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;

	if (!m_pICxE87CMSPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetTransferStatus::put_TransferStatus: port_id=%d, status=%d",
			   port_id, status);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->put_TransferStatus(port_id, LoadPortTransferStateEnum(status));
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : CancelBind service Request
		Carrier ID와 Load Port 연결 부분을 취소하고 Load Port가 NOT_RESERVED 상태로 변경
 parm : port_id	- [in]  Load Port ID (1 or 2) (-1 or 0 이상 값)
		carr_id	- [in]  Carrier ID (or Name; String) (NULL or String)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelBind service
		port_id 혹은 carr_id 둘 중의 하나의 값만 반드시 입력되어야 됨
		Either portID or carrierID is required, but not both
		Set portID to -1 if it is not used and carrierID to an empty string if it is not used
*/
BOOL CE87CMS::SetCancelBind(UINT16 port_id, PTCHAR carr_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrCarrID;
	ICxValueDispPtr valStatus(CLSID_CxValueObject);

	if (!m_pICxE87CMSPtr)	return FALSE;
	/* 둘다 값이 정상적으로 설정되어 있으면 실패 처리 */
	if (port_id >= 0 && carr_id)
	{
		m_pLogData->PushLogs(L"SetCancelBind::Only one of the two variables must be set");
		return FALSE;
	}
	if (port_id >= 0)
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetCancelBind::CancelBind: port_id=%d", port_id);
	}
	else
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetCancelBind::CancelBind: carr_id=%s", carr_id);
		bstrCarrID	= CComBSTR(carr_id);
	}
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->CancelBind(port_id, bstrCarrID, &valStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		DispErrorData(valStatus, E30_SVET::en_caack);
		return FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : CancelCarrierOut service Request
		현재 Carrier에 관련된 Action을 취소하고,
		장비 (Equipment)가 Carrier를 Load Port의 Unload (혹은 내부 버퍼) 위치로 복구 합니다.
 parm : port_id	- [in]  Load Port ID (1 or 2)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelCarrierAtPort service
*/
BOOL CE87CMS::SetCancelCarrierAtPort(UINT16 port_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	ICxValueDispPtr valStatus(CLSID_CxValueObject);

	if (!m_pICxE87CMSPtr)	return FALSE;
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetCancelCarrierAtPort::CancelCarrierAtPort: port_id=%d", port_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->CancelCarrierAtPort(port_id, &valStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		DispErrorData(valStatus, E30_SVET::en_caack);
		return FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : CancelCarrierOut service Request
		현재 Carrier에 관련된 CarrierOut 서비스만 Queue에서 제거
 parm : carr_id	- [in]  Carrier ID (or Name; String)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelCarrierOut service
		The "status" will contain the text of any error and is in the form of the S3F18 reply message
*/
BOOL CE87CMS::SetCancelCarrierOut(PTCHAR carr_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	ICxValueDispPtr valStatus(CLSID_CxValueObject);

	if (!m_pICxE87CMSPtr)	return FALSE;
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetCancelCarrierOut::CancelCarrierOut: carr_id=%s", carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->CancelCarrierOut(CComBSTR(carr_id), &valStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		DispErrorData(valStatus, E30_SVET::en_caack);
		return FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : CancelReserveAtPort service Request
 parm : port_id	- [in]  Load Port ID (1 or 2)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelReserveAtPort service
*/
BOOL CE87CMS::SetCancelReserveAtPort(UINT16 port_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	ICxValueDispPtr valStatus(CLSID_CxValueObject);

	if (!m_pICxE87CMSPtr)	return FALSE;
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetCancelReserveAtPort::CancelReserveAtPort: port_id=%d", port_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->CancelReserveAtPort(port_id, &valStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		DispErrorData(valStatus, E30_SVET::en_caack);
		return FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : 장비 (Equipment)에서 현재 Carrier의 접근 상태 값 설정
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		status	- [in]  Accessing Status (E87_CCAS)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelReserveAtPort service
*/
BOOL CE87CMS::SetCarrierAccessingStatus(PTCHAR carr_id, E87_CCAS status)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;

	if (!m_pICxE87CMSPtr)	return FALSE;
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetCarrierAccessingStatus::put_CarrierAccessingStatus: carr_id=%s", carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->put_CarrierAccessingStatus(CComBSTR(carr_id), AccessingStatusEnum(status));
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : 장비 (Equipment)에서 현재 Carrier의 접근 상태 값 얻기
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		status	- [out] Accessing Status (E87_CCAS)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelReserveAtPort service
*/
BOOL CE87CMS::GetCarrierAccessingStatus(PTCHAR carr_id, E87_CCAS &status)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	AccessingStatusEnum enStatus;

	if (!m_pICxE87CMSPtr)	return FALSE;
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetCarrierAccessingStatus::get_CarrierAccessingStatus: carr_id=%s", carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->get_CarrierAccessingStatus(CComBSTR(carr_id), &enStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* 값 반환 */
	status	= E87_CCAS(enStatus);

	return (hResult == S_OK);
}

/*
 desc : Carrier (FOUP) 내의 특정 위치에 Substrate (Wafer)를 놓기
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		slot_no	- [in]  Slot Number (Not sequential number)
		st_id	- [in]  Substrate ID (Normally, Destination ID)
 retn : TRUE or FALSE
*/
BOOL CE87CMS::SetSubstratePlaced(PTCHAR carr_id, PTCHAR st_id, UINT8 slot_no)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;

	if (!m_pICxE87CMSPtr)	return FALSE;
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetSubstratePlaced::get_CarrierAccessingStatus: carr_id=%s", carr_id);
	m_pLogData->PushLogs(tzMesg);

	hResult	= m_pICxE87CMSPtr->SubstratePlaced(CComBSTR(carr_id), INT16(slot_no), CComBSTR(st_id));
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Load Port & Carrier ID 관련 상태 및 관련된 정보 모두 얻기
 parm : port_id	- [in]  Load Port Number (1-based)
		data	- [out] 상태 및 관련 정보가 저장될 참조 구조체 변수
 retn : TRUE or FALSE
*/
BOOL CE87CMS::GetLoadPortCarrierData(UINT16 port_id, STG_CLPI &data)
{
	E87_LPAS enPortCarrAssociate;
	E87_LPAM enPortAccessState;
	E87_LPRS enPortReservationState;
	E87_LPTS enPortTransferState;
	E87_CIVS enCarrierIDState;
	E87_SMVS enCarrierIDSlotMapState;

	/* Port Number의 유효성 확인 */
	if (port_id < 1 || port_id > MAX_FOUP_COUNT)	return FALSE;

	/* Load port-carrier association status */
	if (!GetAssociationState(port_id, enPortCarrAssociate))	return FALSE;
	data.port_carr_associate	= UINT8(enPortCarrAssociate);
	/* Load Port Access Mode State */
	if (!GetAccessModeState(port_id, enPortAccessState))	return FALSE;
	data.port_access_mode_state	= UINT8(enPortAccessState);
	/* Load Port Access Mode State */
	if (!GetReservationState(port_id, enPortReservationState))	return FALSE;
	data.port_reservation_state	= UINT8(enPortReservationState);
	/* Load port transfer state */
	if (!GetTransferStatus(port_id, enPortTransferState))	return FALSE;
	data.port_transfer_state	= UINT8(enPortTransferState);

	/* Load Port에 부여된 Carrier ID 얻기 */
	if (!GetAssociationCarrierID(port_id, data.carr_id, CIM_CARRIER_ID_SIZE))	return FALSE;
	/* Carrier ID verification status */
	if (!GetCarrierIDStatus(data.carr_id, enCarrierIDState))	return FALSE;
	data.carrier_id_state		= UINT8(enCarrierIDState);
	/* Carrier slot status */
	if (!GetCarrierIDSlotMapStatus(data.carr_id, enCarrierIDSlotMapState))	return FALSE;
	data.slot_map_status		= UINT8(enCarrierIDSlotMapState);

	return TRUE;
}

/*
 desc : Carrier ID가 유효 (Verification)한지 여부
 parm : carr_id	- [in]  검증 대상의 Carrier ID
		logs	- [in]  로그 출력 여부
 retn : TRUE (유효함) or FALSE (유효하지 않음)
*/
BOOL CE87CMS::IsVerifiedCarrierID(PTCHAR carr_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	UINT16 u16PortID;	/* 1 or 2 */
	HRESULT hResult;
	AccessingStatusEnum enStatus;
	CComBSTR bstrLocID;
	ICxE87CarrierPtr pICxCarrier;

	/* ICxE87Carrier 인터페이스 얻기  */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"IsVerifiedCarrierID::LocateCarrier: carr_id=%s", carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE87CMSPtr->LocateCarrier(CComBSTR(carr_id), &pICxCarrier);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Location ID 구하기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"IsVerifiedCarrierID::get_Location: carr_id=%s", carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxCarrier->get_Location(&bstrLocID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Access Status 구하기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"IsVerifiedCarrierID::get_AccessingStatus: carr_id=%s", carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxCarrier->get_AccessingStatus(&enStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* 현재 상태가 Not Accessed인지 여부 확인 */
	if (AccessingStatusEnum::NOT_ACCESSED == enStatus)	return FALSE;

	/* Carrier ID와 관련된 Load Port ID */
	if (!GetAssociationPortID(carr_id, u16PortID))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Recipe (생성, 수정, 삭제) 이벤트 처리
 parm : conn_id	- [in]  연결된 장비 ID (Host ID), Common의 경우, 무조건 0 값
		recipe	- [in]  Recipe Name
		type	- [in]  0x01 (Created), 0x02 (Edited), 0x03 (Deleted)
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
BOOL CE87CMS::SetRecipeManagement(LONG conn_id, PTCHAR recipe, UINT8 type, LONG handle)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	LONG lVarID		= 0;
	HRESULT hResult	= S_OK;
	UNG_CVVT enValue;
	CComBSTR bstrRecipe(recipe);

	CComSafeArray<BSTR> dataNames;
	ICxValueDispPtr dataValues(CLSID_CxValueObject);

	/* VID 값 설정 : Recipe Name & Status 값 변경 */
	enValue.u8_value	= UINT8(type);
	if (!GetVarNameToID(conn_id, L"PPChangeName", lVarID, TRUE))		return FALSE;
	if (!SetCacheAscii(conn_id, lVarID, recipe, handle))				return FALSE;
	if (!GetVarNameToID(conn_id, L"PPChangeStatus", lVarID, TRUE))		return FALSE;
	if (!SetCacheValue(conn_id, E30_GPVT::en_u1, lVarID, enValue, handle))	return FALSE;

	if (hResult == S_OK)	hResult	= dataNames.Add(CComBSTR("PPChangeName"));
	if (hResult == S_OK)	hResult	= dataNames.Add(CComBSTR("PPChangeStatus"));
	if (hResult == S_OK)	hResult	= dataValues->AppendValueAscii(handle, bstrRecipe);		/* Recipe Name */
	if (hResult == S_OK)	hResult	= dataValues->AppendValueU1(handle, UINT8(type));		/* 0x01 ~ 0x03 */
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	if (!SendCollectionEventWithData(conn_id, dataNames, dataValues, L"PPChange"))
		return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Recipe (Selected) 이벤트 처리
 parm : conn_id	- [in]  연결된 장비 ID (Host ID), Common의 경우, 무조건 0 값
		recipe	- [in]  Recipe Name
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
BOOL CE87CMS::SetRecipeSelected(LONG conn_id, PTCHAR recipe, LONG handle)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrRecipe(recipe);

	CComSafeArray<BSTR> dataNames;
	ICxValueDispPtr dataValues(CLSID_CxValueObject);

	if (hResult == S_OK)	hResult	= dataNames.Add(CComBSTR("OperatorCommand"));
	if (hResult == S_OK)	hResult	= dataValues->AppendValueAscii(handle, L"PPSELECT");
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	if (!SendCollectionEventWithData(conn_id, dataNames, dataValues,
									 L"OperatorCommandIssued"))	return FALSE;

	/* 기존 값 초기화 */
	dataNames.Destroy();
	dataValues->Clear();

	if (hResult == S_OK)	hResult	= dataNames.Add(CComBSTR("PPExecName"));
	if (hResult == S_OK)	hResult	= dataValues->AppendValueAscii(handle, bstrRecipe);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	if (!SendCollectionEventWithData(conn_id, dataNames, dataValues, L"PPSelected"))	return FALSE;

	return (hResult == S_OK);
}