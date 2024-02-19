
#include "pch.h"
#include "MainApp.h"
#include "E90STS.h"


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
CE90STS::CE90STS(CLogData *logs, CSharedData *share)
	: CEBase(logs, share)
{
	/* GUID 설정 */
	m_gdICxE	= IID_ICxE90ST;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CE90STS::~CE90STS()
{
	Close();
}

/*
 desc : 초기화 (반드시 호출)
 parm : equip_id	- [in]  Equipment ID (0 or later. 환경 파일에 설정된 개수-1)
		conn_id		- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
 retn : TRUE or FALSE
*/
BOOL CE90STS::Init(LONG equip_id, LONG conn_id)
{
	if (!CEBase::Init(equip_id, conn_id))	return FALSE;

	/* 아래 순서는 반드시 지킬 것 */
	if (!InitE90STS())	return FALSE;
#if 0	/* 외부 GUI에서 직접 호출해야 됨 */
	if (!InitE90HW())	return FALSE;
#endif
	return TRUE;
}

/*
 desc : 해제
 parm : None
 retn : None
*/
VOID CE90STS::Close()
{
	/* 아래 순서는 반드시 지킬 것 */
#if 0
	CloseE90HW();
#endif
	CloseE90STS();

	CEBase::Close();
}

/*
 desc : Substrate ID에 해당되는 인터페이스 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		ptr		- [out] Substrate ID에 대한 인터페이스 반환 포인터
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSubstrateIDPtr(PTCHAR subst_id, ICxSubstrate **ptr)
{
	HRESULT hResult	= S_OK;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetSubstrateIDtoPtr::GetSubstrate : subst_id = %s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	/* Substrate ID를 통해서 ICxSubstrate 얻기 */
	hResult	= m_pICxE90STPtr->GetSubstrate(CComBSTR(subst_id), ptr);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Substrate Location ID에 해당되는 인터페이스 반환
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		ptr		- [out] Substrate ID에 대한 인터페이스 반환 포인터
 retn : TRUE or FALSE
*/
BOOL CE90STS::GetSubstrateLocIDPtr(PTCHAR loc_id, ICxSubstrateLocation **ptr)
{
	HRESULT hResult	= S_OK;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetSubstrateLocIDPtr::GetSubstrateLocation : loc_id = %s", loc_id);
	m_pLogData->PushLogs(tzMesg);
	/* Substrate ID를 통해서 ICxSubstrate 얻기 */
	hResult	= m_pICxE90STPtr->GetSubstrateLocation(CComBSTR(loc_id), ptr);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : 현재 Wafer의 세트 (Lot; FOUP)의 Location에 대한 인터페이스 반환
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		ptr		- [out] Wafer의 세트 (Lot; FOUP)의 Location에 대한 인터페이스 반환 포인터
 retn : TRUE or FALSE
*/
BOOL CE90STS::GetBatchSubstIDPtr(PTCHAR loc_id, ICxBatchLoc **ptr)
{
	HRESULT hResult	= S_OK;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetBatchSubstIDPtr::GetBatchLocation (loc_id=%s)", loc_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->GetBatchLocation(CComBSTR(loc_id), ptr);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : 로그 레벨 값 설정
 parm : level	- [in]  로그 레벨 값
 retn : TRUE or FALSE
*/
BOOL CE90STS::SetLogLevel(ENG_ILLV level)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_FALSE;
	if (m_pICxE90STPtr)
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetLogLevel::CIM90LogLevel level : [%d]", level);
		m_pLogData->PushLogs(tzMesg);
		hResult	= m_pICxE90STPtr->put_CIM90LogLevel((LONG)level);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	}
	return (hResult == S_OK);
}

/*
 desc : 초기화 (반드시 호출)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE90STS::InitE90STS()
{
	HRESULT hResult	= S_FALSE;
	ICxE90SubstrateCBPtr pICxE90SubstrateCB;

	if (m_pICxE90STPtr && m_pICxE90STPtr.GetInterfacePtr())	return FALSE;

	/* Create & Initialize */
	m_pLogData->PushLogs(L"ICxE90ST::CreateInstance");
	hResult = m_pICxE90STPtr.CreateInstance(CLSID_CxE90ST);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE90ST::CreateInstance CLSID_CxE90ST")))
		return FALSE;
	m_pLogData->PushLogs(L"ICxE90ST::Initialize");
	hResult = m_pICxE90STPtr->Initialize(m_pICxE39ObjPtr, m_pICxAbsLayerPtr);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE90ST::Initialize")))	return FALSE;

	/* Register callback function */
	pICxE90SubstrateCB	= m_pEMServiceCbk;
	hResult = m_pICxE90STPtr->RegisterCallback(pICxE90SubstrateCB);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE90ST::RegisterCallback")))	return FALSE;

	/* Register the global interface */
	m_pLogData->PushLogs(L"RegisterInterfaceInGlobal", m_tzGUID);
	hResult	= m_pGIT->RegisterInterfaceInGlobal(m_pICxE90STPtr, m_gdICxE, &m_dwCookie);
	if (FAILED(m_pLogData->CheckResult(hResult, L"RegisterInterfaceInGlobal")))	return FALSE;
	m_pSharedData->GetGITCookie()->e90sts_cookie	= m_dwCookie;

	return (hResult == S_OK);
}

/*
 desc : 해제
 parm : None
 retn : None
*/
VOID CE90STS::CloseE90STS()
{
	HRESULT hResult;

	/* E90STS CallbackPtr */ 
	if (m_pICxE90STPtr && m_pICxE90STPtr.GetInterfacePtr())
	{
		m_pLogData->PushLogs(L"ICxE90ST::Shutdown");
		hResult = m_pICxE90STPtr->Shutdown();
		m_pLogData->CheckResult(hResult, L"ICxE90ST::Shutdown");
		m_pLogData->PushLogs(L"ICxE90ST::Release");
		m_pICxE90STPtr.Release();
	}
}


/*
 desc : 초기화 (반드시 호출)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE90STS::InitE90HW()
{
	HRESULT hResult	= S_OK;

	/* Added : Substrate Tracking System : static substrate locations */
	m_pLogData->PushLogs(L"ICxE90ST::AddSubstrateLocation2 RobotArm 1");
	hResult = m_pICxE90STPtr->AddSubstrateLocation2(CComBSTR("RobotArm"), CComBSTR(""), (SHORT)ENG_TSSI::en_robot);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE90ST::AddSubstrateLocation2 RobotArm 1")))		return FALSE;
	m_pLogData->PushLogs(L"ICxE90ST::AddSubstrateLocation2 PreAligner 2");
	hResult = m_pICxE90STPtr->AddSubstrateLocation2(CComBSTR("PreAligner"), CComBSTR(""), (SHORT)ENG_TSSI::en_pre_aligner);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE90ST::AddSubstrateLocation2 PreAligner 2")))		return FALSE;
	m_pLogData->PushLogs(L"ICxE90ST::AddSubstrateLocation2 ProcessChamber 3");
	hResult = m_pICxE90STPtr->AddSubstrateLocation2(CComBSTR("ProcessChamber"), CComBSTR(""), (SHORT)ENG_TSSI::en_process_chamber);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE90ST::AddSubstrateLocation2 ProcessChamber 3")))	return FALSE;
	m_pLogData->PushLogs(L"ICxE90ST::AddSubstrateLocation2 Buffer 4");
	hResult = m_pICxE90STPtr->AddSubstrateLocation2(CComBSTR("Buffer"), CComBSTR(""), (SHORT)ENG_TSSI::en_buffer);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE90ST::AddSubstrateLocation2 Buffer 4")))			return FALSE;
	m_pLogData->PushLogs(L"ICxE90ST::AddSubstrateLocation2 LoadPort1 5");
	hResult = m_pICxE90STPtr->AddSubstrateLocation2(CComBSTR("LoadPort1"), CComBSTR(""), (SHORT)ENG_TSSI::en_load_port_1);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE90ST::AddSubstrateLocation2 LoadPort1 5")))		return FALSE;
	m_pLogData->PushLogs(L"ICxE90ST::AddSubstrateLocation2 LoadPort2 6");
	hResult = m_pICxE90STPtr->AddSubstrateLocation2(CComBSTR("LoadPort2"), CComBSTR(""), (SHORT)ENG_TSSI::en_load_port_2);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE90ST::AddSubstrateLocation2 LoadPort2 6")))		return FALSE;
	m_pLogData->PushLogs(L"ICxE90ST::AddSubstrateLocation2 Unused3 7");
	hResult = m_pICxE90STPtr->AddSubstrateLocation2(CComBSTR("Unused3"), CComBSTR(""), 7);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE90ST::AddSubstrateLocation2 Unused3 7")))			return FALSE;
	m_pLogData->PushLogs(L"ICxE90ST::AddSubstrateLocation2 Unused4 8");
	hResult = m_pICxE90STPtr->AddSubstrateLocation2(CComBSTR("Unused4"), CComBSTR(""), 8);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE90ST::AddSubstrateLocation2 Unused4 8")))			return FALSE;
	m_pLogData->PushLogs(L"ICxE90ST::AddSubstrateLocation2 Unused5 9");
	hResult = m_pICxE90STPtr->AddSubstrateLocation2(CComBSTR("Unused5"), CComBSTR(""), 9);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE90ST::AddSubstrateLocation2 Unused5 9")))			return FALSE;
	m_pLogData->PushLogs(L"ICxE90ST::AddSubstrateLocation2 Unused6 10");
	hResult = m_pICxE90STPtr->AddSubstrateLocation2(CComBSTR("Unused6"), CComBSTR(""), 10);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE90ST::AddSubstrateLocation2 Unused6 10")))		return FALSE;

	return TRUE;
}

/*
 desc : 해제
 parm : None
 retn : None
*/
VOID CE90STS::CloseE90HW()
{
	HRESULT hResult	= S_OK;

	if (m_pICxE90STPtr && m_pICxE90STPtr.GetInterfacePtr())
	{
		/* Removed : Substrate Tracking System : static substrate locations */
		m_pLogData->PushLogs(L"ICxE90ST::RemoveSubstrateLocation RobotArm 1");
		hResult = m_pICxE90STPtr->RemoveSubstrateLocation(CComBSTR("RobotArm"));
		m_pLogData->CheckResult(hResult, L"ICxE90ST::RemoveSubstrateLocation RobotArm 1");
		m_pLogData->PushLogs(L"ICxE90ST::RemoveSubstrateLocation PreAligner 2");
		hResult = m_pICxE90STPtr->RemoveSubstrateLocation(CComBSTR("PreAligner"));
		m_pLogData->CheckResult(hResult, L"ICxE90ST::RemoveSubstrateLocation PreAligner 2");
		m_pLogData->PushLogs(L"ICxE90ST::RemoveSubstrateLocation ProcessChamber 3");
		hResult = m_pICxE90STPtr->RemoveSubstrateLocation(CComBSTR("ProcessChamber"));
		m_pLogData->CheckResult(hResult, L"ICxE90ST::RemoveSubstrateLocation ProcessChamber 3");
		m_pLogData->PushLogs(L"ICxE90ST::RemoveSubstrateLocation Buffer 4");
		hResult = m_pICxE90STPtr->RemoveSubstrateLocation(CComBSTR("Buffer"));
		m_pLogData->CheckResult(hResult, L"ICxE90ST::RemoveSubstrateLocation Buffer 4");
		m_pLogData->PushLogs(L"ICxE90ST::RemoveSubstrateLocation LoadPort1 5");
		hResult = m_pICxE90STPtr->RemoveSubstrateLocation(CComBSTR("LoadPort1"));
		m_pLogData->CheckResult(hResult, L"ICxE90ST::RemoveSubstrateLocation LoadPort1 5");
		m_pLogData->PushLogs(L"ICxE90ST::RemoveSubstrateLocation LoadPort2 6");
		hResult = m_pICxE90STPtr->RemoveSubstrateLocation(CComBSTR("LoadPort2"));
		m_pLogData->CheckResult(hResult, L"ICxE90ST::RemoveSubstrateLocation LoadPort2 6");
		m_pLogData->PushLogs(L"ICxE90ST::RemoveSubstrateLocation Unused3 7");
		hResult = m_pICxE90STPtr->RemoveSubstrateLocation(CComBSTR("Unused3"));
		m_pLogData->CheckResult(hResult, L"ICxE90ST::RemoveSubstrateLocation Unused3 7");
		m_pLogData->PushLogs(L"ICxE90ST::RemoveSubstrateLocation Unused4 8");
		hResult = m_pICxE90STPtr->RemoveSubstrateLocation(CComBSTR("Unused4"));
		m_pLogData->CheckResult(hResult, L"ICxE90ST::RemoveSubstrateLocation Unused4 8");
		m_pLogData->PushLogs(L"ICxE90ST::RemoveSubstrateLocation Unused5 9");
		hResult = m_pICxE90STPtr->RemoveSubstrateLocation(CComBSTR("Unused5"));
		m_pLogData->CheckResult(hResult, L"ICxE90ST::RemoveSubstrateLocation Unused5 9");
		m_pLogData->PushLogs(L"ICxE90ST::RemoveSubstrateLocation Unused6 10");
		hResult = m_pICxE90STPtr->RemoveSubstrateLocation(CComBSTR("Unused6"));
		m_pLogData->CheckResult(hResult, L"ICxE90ST::RemoveSubstrateLocation Unused6 10");
	}
}

/*
 desc : Substrate (Wafer)를 승인 하도록 (받도록) 장비 쪽을 호출
		장비 쪽에서 내부적으로 E90CommandCallback 함수가 호출됨
 parm : subst_id	- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
 retn : TRUE or FALSE
 note : Request the equipment to accept the substrate.
		Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::SetProceedWithSubstrate(PTCHAR subst_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrSubstID (subst_id);
	ICxValueDispPtr valStatus(CLSID_CxValueObject);

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetProceedWithSubstrate::ProceedWithSubstrate");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->ProceedWithSubstrate(bstrSubstID, valStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		/* 에러 코드 및 문자열 정보 출력 */
		DispErrorData(valStatus, E30_SVET::en_svcack);
		return FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : Remote Control 허용 여부 값 반환
 parm : enable	- [out] Enable or Disable
 retn : TRUE or FALSE
*/
BOOL CE90STS::GetAllowRemoteControl(BOOL &enable)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	VARIANT_BOOL boEnable = VARIANT_FALSE;

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetAllowRemoteControl::get_AllowRemoteControl");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->get_AllowRemoteControl(&boEnable);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* 값 반환 */
	enable	= (boEnable == VARIANT_TRUE) ? TRUE : FALSE;

	return (hResult == S_OK);
}

/*
 desc : Remote Control 허용 여부 설정
 parm : enable	- [in]  Enable or Disable
 retn : TRUE or FALSE
*/
BOOL CE90STS::SetAllowRemoteControl(BOOL enable)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	VARIANT_BOOL boEnable = enable == TRUE ? VARIANT_TRUE : VARIANT_FALSE;

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetAllowRemoteControl::put_AllowRemoteControl : enable = %d", enable);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->put_AllowRemoteControl(boEnable);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Substrate Location (Wafer가 놓여질 위치 이름) 등록
 parm : loc_id	- [in]  Location ID (Name)가 저장된 문자열 버퍼 (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
						Substrate Location ID_xx (문자열. RobotArm / PreAligner / ProcessChamber / Buffer)
 retn : TRUE or FALSE
 note : substrateID가 등록되어 있지 않으면 실패 처리 (RegisterSubstrate ~ RegisterSubstrate4 중 1개 사용)
*/
BOOL CE90STS::AddSubstrateLocation(PTCHAR loc_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrLocID (loc_id), bstrSubstID;

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetAddSubstrateLocation::AddSubstrateLocation(2) : "
										  L"loc_id = %s", loc_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->AddSubstrateLocation(bstrLocID, bstrSubstID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Substrate Location (Wafer가 놓여질 위치 이름) 제거
		loc_id	- [in]  Location ID (Name)가 저장된 문자열 버퍼 (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
						Substrate Location ID_xx (문자열. RobotArm / PreAligner / ProcessChamber / Buffer)
 retn : TRUE or FALSE
*/
BOOL CE90STS::RemoveSubstrateLocation(PTCHAR loc_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrLocID (loc_id);

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetAddSubstrateLocation::RemoveSubstrateLocation(2) : loc_id = %s", loc_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->RemoveSubstrateLocation(bstrLocID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Substrate (Transport & Processing) 상태 값 변경
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		loc_id	- [in]  Location ID (Name)가 저장된 문자열 버퍼 (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
						Substrate Location ID_xx (문자열. RobotArm / PreAligner / ProcessChamber / Buffer)
		trans	- [in]  Substrate transport state
		proc	- [in]  Substrate processing state
		time_in	- [in]  Batch가 Location ID에 도착하는 시간 (NULL이면 사용 안함)
		time_out- [in]  Batch가 Location ID에 이탈하는 시간 (NULL이면 사용 안함)
						time format : YYYYMMDDhhmmsscc for YearMonthDayHourMinuteSecondCentisecond
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::SetChangeSubstrateState(PTCHAR subst_id, PTCHAR loc_id, E90_SSTS trans, E90_SSPS proc,
									  PTCHAR time_in, PTCHAR time_out)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	LONG lStatus	= 0;
	HRESULT hResult	= S_OK;

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetChangeSubstrate::ChangeSubstrateState: st_id=%s, loc_id=%s "
			   L"state_trans=%d, state_proc=%d, time_in=%s, time_out=%s",
			   subst_id, loc_id, trans, proc, time_in, time_out);
	m_pLogData->PushLogs(tzMesg);
	if (time_in && time_out)
	{
		hResult	= m_pICxE90STPtr->ChangeSubstrateStateAndHistory(CComBSTR(subst_id), CComBSTR(loc_id),
																 SubstrateTransportStateEnum(trans),
																 SubstrateProcessingStateEnum(proc),
																 CComBSTR(time_in), CComBSTR(time_out),
																 &lStatus);
	}
	else
	{
		hResult	= m_pICxE90STPtr->ChangeSubstrateState(CComBSTR(subst_id), CComBSTR(loc_id),
													   SubstrateTransportStateEnum(trans),
													   SubstrateProcessingStateEnum(proc), &lStatus);
	}
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Substrate (Transport & Processing) 상태 값 변경
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		loc_id	- [in]  Location ID (Name)가 저장된 문자열 버퍼 (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
						Substrate Location ID_xx (문자열. RobotArm / PreAligner / ProcessChamber / Buffer)
		trans	- [in]  Substrate transport state
		proc	- [in]  Substrate processing state
		status	- [in]  Substrate ID state E90_SISM
		time_in	- [in]  Batch가 Location ID에 도착하는 시간 (NULL이면 사용 안함)
		time_out- [in]  Batch가 Location ID에 이탈하는 시간 (NULL이면 사용 안함)
						time format : YYYYMMDDhhmmsscc for YearMonthDayHourMinuteSecondCentisecond
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::SetChangeSubstrateState2(PTCHAR subst_id, PTCHAR loc_id, E90_SSTS trans,
									   E90_SSPS proc, E90_SISM status,
									   PTCHAR time_in, PTCHAR time_out)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	LONG lStatus	= 0;
	HRESULT hResult	= S_OK;
	ICxValueDispPtr valStatus(CLSID_CxValueObject);

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetChangeSubstrate::ChangeSubstrateState: st_id=%s, loc_id=%s "
			   L"state_trans=%d, state_proc=%d, state_id=%d, time_in=%s, time_out=%s",
			   subst_id, loc_id, trans, proc, status, time_in, time_out);
	m_pLogData->PushLogs(tzMesg);
	if (time_in && time_out)
	{
		hResult	= m_pICxE90STPtr->ChangeSubstrateStateAndHistory2(CComBSTR(subst_id), CComBSTR(loc_id),
																  SubstrateTransportStateEnum(trans),
																  SubstrateProcessingStateEnum(proc),
																  SubstIDStatusEnum(status),
																  CComBSTR(time_in), CComBSTR(time_out),
																  &valStatus);
	}
	else
	{
		hResult	= m_pICxE90STPtr->ChangeSubstrateState2(CComBSTR(subst_id), CComBSTR(loc_id),
														SubstrateTransportStateEnum(trans),
														SubstrateProcessingStateEnum(proc),
														SubstIDStatusEnum(status), &valStatus);
	}
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		if (valStatus)
		{
			DispErrorData(valStatus, E30_SVET::en_acka);
			return FALSE;
		}
	}

	return (hResult == S_OK);
}

/*
 desc : Substrate Processing 상태 값 변경
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		proc	- [in]  Substrate processing state
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::SetChangeSubstrateProcessingState(PTCHAR subst_id, E90_SSPS proc)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	LONG lStatus	= 0;
	HRESULT hResult	= S_OK;

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetChangeSubstrateProcessingState::ChangeSubstrateProcessingState: st_id=%s, state=%d",
			   subst_id, proc);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->ChangeSubstrateProcessingState(CComBSTR(subst_id),
															 SubstrateProcessingStateEnum(proc)); 
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Substrate 생성
 parm : subst_id- [in]  Substrate ID (문자열)
		loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		sub_id	- [in]  Location Group ID를 위해 사용하는 Subscript Index (location's index)
						EPJ 파일에서 해당 위치에 사용되는 첨자?(Subscripted) 변수의 세트 (set)
						-> EPJ 파일에 정의된 내용과 연관성이 있을듯... (0 값이면 사용 안함)
		lot_id	- [in]  Lot ID 즉, 사용자에 의해 기판에 할당된 LOT의 Index 값 (NULL이면 사용 안함) 
 retn : TRUE or FALSE
 note : Substrate 생성은 보통 CIM87에 의해 생성 (Carrier slotmap이 검증될 때)되지만,
		몇몇 다른 방식으로 생성할 필요가 있을 때, 이 함수를 호출하여 생성
		Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::RegisterSubstrate(PTCHAR subst_id, PTCHAR loc_id, INT16 sub_id, PTCHAR lot_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	LONG lStatus	= 0;
	HRESULT hResult	= S_OK;

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"RegisterSubstrate::RegisterSubstrate (st_id:%s / loc_id:%s / subscript_id:%d / lot_id:%s)",
			   subst_id, loc_id, sub_id, lot_id);
	m_pLogData->PushLogs(tzMesg);
	if (lot_id && sub_id > 0)
	{
		hResult	= m_pICxE90STPtr->RegisterSubstrate4(CComBSTR(subst_id),CComBSTR(loc_id),
													 sub_id, CComBSTR(lot_id), &lStatus);
	}
	else if (!lot_id && sub_id > 0)
	{
		hResult	= m_pICxE90STPtr->RegisterSubstrate3(CComBSTR(subst_id), CComBSTR(loc_id),
													 sub_id, &lStatus);
	}
	else if (lot_id)
	{
		hResult	= m_pICxE90STPtr->RegisterSubstrate2(CComBSTR(subst_id), CComBSTR(loc_id),
													 CComBSTR(lot_id), &lStatus);
	}
	else
	{
		hResult	= m_pICxE90STPtr->RegisterSubstrate(CComBSTR(subst_id), CComBSTR(loc_id), &lStatus);
	}
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Substrate ID 제거
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::RemoveSubstrate(PTCHAR subst_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetRemoveSubstrate::RemoveSubstrate2 : st_id = %s", subst_id);
	m_pLogData->PushLogs(tzMesg);
#if 1
	hResult	= m_pICxE90STPtr->RemoveSubstrate(CComBSTR(subst_id));

#else
	hResult	= m_pICxE90STPtr->RemoveSubstrate2(CComBSTR(subst_id));
#endif
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Substrate Location ID의 상태 값 반환
 parm : loc_id	- [in]  Location ID (Name)가 저장된 문자열 버퍼 (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
						Substrate Location ID_xx (문자열. RobotArm / PreAligner / ProcessChamber / Buffer)
		state	- [out] Substrate Location State 값 반환 (E90_SSLS)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
BOOL CE90STS::GetSubstrateLocationState(PTCHAR loc_id, E90_SSLS &state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	SubstrateLocationStateEnum enState; 
	HRESULT hResult	= S_OK;

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"GetSubstrateLocationState::GetSubstrateLocationState : st_id = %s", loc_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->GetSubstrateLocationState(CComBSTR(loc_id), &enState);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	state	= E90_SSLS(enState);

	return (hResult == S_OK);
}

/*
 desc : Substrate Location ID의 상태 값 설정
 parm : loc_id	- [in]  Location ID (Name)가 저장된 문자열 버퍼 (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
						Substrate Location ID_xx (문자열. RobotArm / PreAligner / ProcessChamber / Buffer)
		state	- [in]  Substrate Location State 값 반환 (E90_SSLS)
 retn : TRUE or FALSE
*/
BOOL CE90STS::SetSubstrateLocationState(PTCHAR loc_id, E90_SSLS state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetSubstrateLocationState::SetSubstrateLocationState : st_id = %s", loc_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->SetSubstrateLocationState(CComBSTR(loc_id),
														SubstrateLocationStateEnum(state));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Substrate Processing의 이전 상태 값 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		state	- [out] Substrate Location State 값 반환 (E90_SSPS)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSubstratePrevProcessingState(PTCHAR subst_id, E90_SSPS &state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	SubstrateProcessingStateEnum enState; 
	HRESULT hResult	= S_OK;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"GetSubstratePrevProcessingState::get_previousProcessingState : st_id = %s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_previousProcessingState(&enState);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	state	= E90_SSPS(enState);

	return (hResult == S_OK);
}

/*
 desc : Substrate Processing 상태 값 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		state		- [out] Substrate Location State 값 반환 (E90_SSPS)
		logs		- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSubstrateProcessingState(PTCHAR subst_id, E90_SSPS &state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	SubstrateProcessingStateEnum enState; 
	HRESULT hResult	= S_OK;

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"GetSubstrateProcessingState::GetSubstrateProcessingState : st_id = %s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->GetSubstrateProcessingState(CComBSTR(subst_id), &enState);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	state	= E90_SSPS(enState);

	return (hResult == S_OK);
}

/*
 desc : Substrate Processing 상태 값 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		state	- [in]  설정하고자 하는 상태 값 (E90_SSPS)
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::SetSubstrateProcessingState(PTCHAR subst_id, E90_SSPS state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetSubstrateProcessingState::SetSubstrateProcessingState : st_id = %s, state = %d",
			   subst_id, state);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->SetSubstrateProcessingState(CComBSTR(subst_id),
														  SubstrateProcessingStateEnum(state));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Substrate Transport 이전 상태 값 반환
 parm : subst_id- [in]  Substrate ID (문자열)
		state	- [out] Substrate Transport State 값 반환 (E90_SSTS)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSubstratePrevTransportState(PTCHAR subst_id, E90_SSTS &state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	SubstrateTransportStateEnum enState; 
	HRESULT hResult	= S_OK;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"GetSubstratePrevTransportState::get_previousTransportState : st_id = %s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_previousTransportState(&enState);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	state	= E90_SSTS(enState);

	return (hResult == S_OK);
}

/*
 desc : Substrate Transport 상태 값 반환
 parm : subst_id- [in]  Substrate ID (문자열)
		state	- [out] Substrate Transport State 값 반환 (E90_SSTS)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSubstrateTransportState(PTCHAR subst_id, E90_SSTS &state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	SubstrateTransportStateEnum enState; 
	HRESULT hResult	= S_OK;

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"GetSubstrateTransportState::GetSubstrateTransportState : st_id = %s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->GetSubstrateTransportState(CComBSTR(subst_id), &enState);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	state	= E90_SSTS(enState);

	return (hResult == S_OK);
}

/*
 desc : Substrate Transport 상태 값 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		state	- [in]  설정하고자 하는 상태 값 (E90_SSTS)
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::SetSubstrateTransportState(PTCHAR subst_id, E90_SSTS state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetSubstrateTransportState::SetSubstrateTransportState : st_id = %s, state = %d",
			   subst_id, state);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->SetSubstrateTransportState(CComBSTR(subst_id),
														SubstrateTransportStateEnum(state));

	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Substrate ID (Tag ? Serial ? Bar Code)의 이전 상태 값 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		status	- [out] Enumeration values for the ID Reader state machine
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSubstratePrevIDStatus(PTCHAR subst_id, E90_SISM &status)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	SubstIDStatusEnum enState;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"GetSubstratePrevIDStatus::get_previousIDStatus : st_id = %s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_previousIDStatus(&enState);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 상태 값 반환 */
	status	= E90_SISM(enState);

	return (hResult == S_OK);
}

/*
 desc : Substrate ID (Tag ? Serial ? Bar Code) 읽어진 상태 값 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		status	- [out] Enumeration values for the ID Reader state machine
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSubstrateIDStatus(PTCHAR subst_id, E90_SISM &status)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	SubstIDStatusEnum enState;
	ICxValueDispPtr valStatus(CLSID_CxValueObject);

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"GetSubstIDStatus::GetSubstIDStatus : st_id = %s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->GetSubstIDStatus(CComBSTR(subst_id), valStatus, &enState);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		DispErrorData(valStatus, E30_SVET::en_acka);
		return S_FALSE;
	}

	/* 상태 값 반환 */
	status	= E90_SISM(enState);

	return (hResult == S_OK);
}

/*
 desc : Substrate ID (Tag ? Serial ? Bar Code) 읽어진 값 상태 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		state	- [in]  Enumeration values for the ID Reader state machine
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::SetSubstrateIDStatus(PTCHAR subst_id, E90_SISM state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	ICxValueDispPtr valStatus(CLSID_CxValueObject);

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetSubstIDStatus::SetSubstIDStatus : st_id = %s, state = %d",
			   subst_id, state);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->SetSubstIDStatus(CComBSTR(subst_id), SubstIDStatusEnum(state), valStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		DispErrorData(valStatus, E30_SVET::en_acka);
		return S_FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : CIM300 (Host)에게 Substrate ID (Tag ? Serial ? Bar Code) 읽어진 값 상태를 판단해 달라고 요청
 parm : subst_id	- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		acquired_id	- [in]  Substrate ID acquired from the substrate ID reader (Serial? Tag?)
		read_good	- [in]  TRUE if read was successful. Otherwise FALSE
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::NotifySubstrateRead(PTCHAR subst_id, PTCHAR acquired_id, BOOL read_good)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	VARIANT_BOOL boRead	= read_good ? VARIANT_TRUE : VARIANT_FALSE;
	ICxValueDispPtr valStatus(CLSID_CxValueObject);

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"NotifySubstrateRead::NotifySubstrateRead : st_id = %s, acquired_id = %s, read_goo=%d",
			   subst_id, acquired_id, read_good);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->NotifySubstrateRead(CComBSTR(subst_id), CComBSTR(acquired_id), boRead, valStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		DispErrorData(valStatus, E30_SVET::en_acka);
		return S_FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : Substrate ID Reader가 판독 가능 혹은 불가능 할 때, 이벤트 발생 시킴
 parm : enable	- [in]  Substrate ID Reader가 사용 가능한지 여부 설정
						TRUE: 사용 가능할 때, 이벤트 알림
						FALSE:사용 불가능할 때, 이벤트 알림
 retn : TRUE or FALSE
*/
BOOL CE90STS::SubstrateReaderAvailable(BOOL enable)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetSubstrateReaderAvailable::SubstrateReaderAvailable : enable = %d", enable);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->SubstrateReaderAvailable(INT32(enable));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : 장비에게 현재 substrate를 건너띄고, 소스 혹은 다른 위치로 이동하라고 요청 함 (원래 위치로 이동하라고 함)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
 retn : TRUE or FALSE
 note : This function executes the E90CommandCallback() to allow the equipment to reject the command
		즉, 이 함수를 호출하면, 장비 내에서 거부될 경우, E90CommandCallback 함수가 호출 됨
		Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::CancelSubstrate(PTCHAR subst_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	ICxValueDispPtr valStatus(CLSID_CxValueObject);

	if (!m_pICxE90STPtr)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetCancelSubstrate::CancelSubstrate : st_id = %s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->CancelSubstrate(CComBSTR(subst_id), valStatus);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		DispErrorData(valStatus, E30_SVET::en_acka);
		return S_FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : 실제 Substrate ID에서 읽어들인 Wafer Reader Code (Tag, Serial, Bar Code, etc) 값을 반환
 parm : subst_id	- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		acquired_id	- [out] Wafer Reader로부터 읽어들인 ID 값 반환 (문자열)
		size		- [in]  'acquired_id' 버퍼 크기
 retn : TRUE or FALSE
 note : Contains the ID read from the substrate. Empty string before the substrate is read.
		The attribute shall be updated as soon the substrate ID has been successfully read
		Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSubstrateIDToAcquiredID(PTCHAR subst_id, PTCHAR acquired_id, UINT32 size)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrAcqID;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 AcquiredID 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"GetSubstIDtoAcquiredID::get_acquiredID : st_id = %s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_acquiredID(&bstrAcqID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	swprintf_s(acquired_id, size, L"%s", (LPTSTR)_bstr_t(bstrAcqID));

	return (hResult == S_OK);
}

/*
 desc : 실제 Substrate ID에서 읽어들인 Wafer Reader Code (Tag, Serial, Bar Code, etc) 값 설정
 parm : subst_id	- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		acquired_id	- [in]  Wafer Reader로부터 읽어들인 ID 값 (문자열)
 retn : TRUE or FALSE
 note : Contains the ID read from the substrate. Empty string before the substrate is read.
		The attribute shall be updated as soon the substrate ID has been successfully read
		Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::SetSubstrateIDToAcquiredID(PTCHAR subst_id, PTCHAR acquired_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 AcquiredID 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"GetSubstIDtoAcquiredID::put_acquiredID : st_id=%s, acq_id=%s", subst_id, acquired_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->put_acquiredID(CComBSTR(acquired_id));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Substrate ID 값 반환 (Slot Number; Slot No)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		slot_no	- [out] Slot Number 값이 반환되어 저장 (1-based or Later. Max. 255)	
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSubstrateIDToSlotNo(PTCHAR subst_id, UINT8 &slot_no)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	INT32 i32SlotNo	= -1;
	HRESULT hResult	= S_OK;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Slot ID (Number) 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"GetSubstrateIDToSlotNo::get_sourceSlotID : st_id = %s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_sourceSlotID(&i32SlotNo);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	slot_no	= UINT8(i32SlotNo);

	return (hResult == S_OK);
}

/*
 desc : Substrate Location ID 값 반환 (Slot Number; Slot No)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		loc_id	- [out] Location ID (Buffer, Robot, PreAligner, Chamber, etc)가 저장될 버퍼
		size	- [in]  'loc_id' 버퍼 크기
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSubstrateLocationID(PTCHAR subst_id, PTCHAR loc_id, UINT32 size)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	INT32 i32SlotNo	= -1;
	HRESULT hResult	= S_OK;
	CComBSTR bstrLocID;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;
	/* ICxSubstratePtr를 통해서 Slot ID (Number) 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"GetSubstrateIDToSlotNo::get_locationID : st_id = %s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_locationID(&bstrLocID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	swprintf_s(loc_id, size, L"%s", (LPTSTR)_bstr_t(bstrLocID));

	return (hResult == S_OK);
}

/*
 desc : Carrier 내의 Slot Number에 대한 Destination Substrate ID 얻기
 parm : st_ptr	- [in]  ICxSubstratePtr
		subst_id- [out] Substrate ID 값 반환 버퍼
		size	- [in]  'st_id' 버퍼 크기
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSubstrateDestinationID(ICxSubstratePtr st_ptr, PTCHAR subst_id, UINT32 size)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrSubstID;

	if (!st_ptr)	return FALSE;
	
	/* Get the destination */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetSubstrateDestinationID::get_substrateDestination");
	m_pLogData->PushLogs(tzMesg);
	hResult	= st_ptr->get_substrateDestination(&bstrSubstID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* 값 저장 */
#if 1
	swprintf_s(subst_id, size, L"%s", (LPCTSTR)_bstr_t(bstrSubstID));
#else
	if (!uvCmn_CComBSTR2TCHAR(bstrSubstID, subst_id, size))	return FALSE;
#endif

	return (hResult == S_OK);
}

/*
 desc : Carrier 내의 Slot Number에 대한 Substrate (Source) ID 얻기
 parm : st_ptr	- [in]  ICxSubstratePtr
		subst_id- [out] Substrate ID 값 반환 버퍼
		size	- [in]  'st_id' 버퍼 크기
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSubstrateID(ICxSubstratePtr st_ptr, PTCHAR subst_id, UINT32 size)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrSubstID;

	if (!st_ptr)	return FALSE;
	
	/* Get the destination */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetSubstrateDestinationID::get_objectID");
	m_pLogData->PushLogs(tzMesg);
	hResult	= st_ptr->get_objectID(&bstrSubstID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 저장 */
#if 1
	swprintf_s(subst_id, size, L"%s", (LPCTSTR)_bstr_t(bstrSubstID));
#else
	if (!uvCmn_CComBSTR2TCHAR(bstrSubstID, subst_id, size))	return FALSE;
#endif

	return (hResult == S_OK);
}

/*
 desc : Substrate ID에 해당되는 sourceCarrierID, sourceSlotID, currentProcessingState 값 얻기
 parm : subst_id	- [in]  Substrate ID_xx (문자열)
		state		- [out]  Substrate ID와 관련된 각종 상태 정보 반환
		logs		- [in]  로그 출력 여부
 retn : TRUE or FALSE
 참고 :	carr_id		- [out] 'sourceCarrierID가 저장될 버퍼
		carr_size	- [in]  'carr_id'의 버퍼 크기
		slot_no		- [out] 'sourceSlotID가 저장될 버퍼
		proc_state	- [out]  currentProcessingState 값 반환 버퍼
		trans_state	- [out]  currentTransportState 값 반환 버퍼
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetPRCommandSubstState(PTCHAR subst_id, STG_CSDI &state)
{
	HRESULT hResult	= S_OK;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	INT32 i32SlotNo	= 0;
	SubstrateProcessingStateEnum enStateProc;
	SubstrateTransportStateEnum enStateTrans;
	SubstIDStatusEnum enStateID;
	CComBSTR bstrCarrID, bstrSlotID;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRCommandInfo::get_sourceCarrierID : st_id=%s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_sourceCarrierID(&bstrCarrID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	swprintf_s(state.carr_id, CIM_CARRIER_ID_SIZE, L"%s", (LPTSTR)_bstr_t(bstrCarrID));

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRCommandInfo::get_sourceSlotID : st_id=%s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_sourceSlotID(&i32SlotNo);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	state.slot_no	= UINT8(i32SlotNo);

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRCommandInfo::get_currentProcessingState : st_id=%s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_currentProcessingState(&enStateProc);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	state.proc_state	= INT32(enStateProc);

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRCommandInfo::get_currentTransportState : st_id=%s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_currentTransportState(&enStateTrans);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	state.trans_state	= INT32(enStateTrans);

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRCommandInfo::get_currentIDStatus : st_id=%s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_currentIDStatus(&enStateID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	state.mtl_status	= INT32(enStateID);

	return (hResult == S_OK);
}

/*
 desc : 실제 Substrate ID에서 ICxSubstratePtr 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		st_ptr	- [out] 반환될 ICxSubstratePtr 객체 포인터
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSubstratePtr(PTCHAR subst_id, ICxSubstratePtr &st_ptr)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetSubstratePtr::GetSubstrate : st_id=%s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->GetSubstrate(CComBSTR(subst_id), &st_ptr);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : 해당 로케이션에 웨이퍼가 있는지 여부 
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/															   
BOOL CE90STS::IsLocationSubstrateID(PTCHAR loc_id)
{
	HRESULT hResult = S_OK;
	CComBSTR bstrSubstID;
	ICxSubstrateLocationPtr pICxSubstLocPtr;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Substrate Location ID 인터페이스 포인터 얻기 */
	if (!GetSubstrateLocIDPtr(loc_id, &pICxSubstLocPtr))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"IsLocationSubstrateID::get_substrateID : location name=%s", loc_id);
	m_pLogData->PushLogs(tzMesg);
	
	hResult = pICxSubstLocPtr->get_substrateID(&bstrSubstID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (bstrSubstID != NULL);
}

/*
 desc : 현재 Wafer의 세트 (Lot; FOUP)가 놓여진 객체의 위치 정보를 반환 한다
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		index	- [out] Wafer의 세트 (Lot; FOUP)가 놓여진 위치 (인덱스) 값
 retn : TRUE or FALSE
*/
BOOL CE90STS::GetBatchLocIdx(PTCHAR loc_id, INT16 &index)
{
	short i16Index	= -1;
	HRESULT hResult	= S_OK;
	ICxBatchLocPtr pICxBatchLoc;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Batch Substrate ID에 대한 인터페이스 포인터 반환 */
	if (!GetBatchSubstIDPtr(loc_id, &pICxBatchLoc))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetBatchLocIdx::get_BatchLocIdx");
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxBatchLoc->get_BatchLocIdx(&i16Index);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	index	= i16Index;

	return TRUE;
}

/*
 desc : 현재 Wafer의 세트 (Lot; FOUP)가 놓여진 객체의 위치 정보 설정
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		index	- [in]	Wafer의 세트 (Lot; FOUP)가 놓여질 위치 (인덱스) 값
 retn : TRUE or FALSE
*/
BOOL CE90STS::SetBatchLocIdx(PTCHAR loc_id, INT16 index)
{
	HRESULT hResult	= S_OK;
	ICxBatchLocPtr pICxBatchLoc;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Batch Substrate ID에 대한 인터페이스 포인터 반환 */
	if (!GetBatchSubstIDPtr(loc_id, &pICxBatchLoc))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetBatchLocIdx::put_BatchLocIdx");
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxBatchLoc->put_BatchLocIdx(index);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : 현재 Wafer의 세트 (Lot; FOUP)가 놓여진 객체의 점유 상태 여부
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
*/
BOOL CE90STS::IsBatchLocOccupied(PTCHAR loc_id)
{
	HRESULT hResult	= S_OK;
	ICxBatchLocPtr pICxBatchLoc;
	BatchLocStateEnum enState;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Batch Substrate ID에 대한 인터페이스 포인터 반환 */
	if (!GetBatchSubstIDPtr(loc_id, &pICxBatchLoc))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"IsBatchLocOccupied::get_BatchLocState");
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxBatchLoc->get_BatchLocState(&enState);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return enState == BatchLocStateEnum::blsOCCUPIED;
}

/*
 desc : 현재 Wafer의 세트 (Lot; FOUP)가 놓여진 객체의 점유 상태 여부
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		state	- [in]  Batch State 값 즉, 점유 (1), 점유 안함 (0)
 retn : TRUE or FALSE
*/
BOOL CE90STS::SetBatchLocState(PTCHAR loc_id, E90_SSLS state)
{
	HRESULT hResult	= S_OK;
	ICxBatchLocPtr pICxBatchLoc;
	BatchLocStateEnum enState = (state == E90_SSLS::en_occupied) ?
								BatchLocStateEnum::blsOCCUPIED : BatchLocStateEnum::blsUNOCCUPIED;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Batch Substrate ID에 대한 인터페이스 포인터 반환 */
	if (!GetBatchSubstIDPtr(loc_id, &pICxBatchLoc))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"IsBatchLocOccupied::put_BatchLocState (%d)", INT32(enState));
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxBatchLoc->put_BatchLocState(enState);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return enState == BatchLocStateEnum::blsOCCUPIED;
}

/*
 desc : 현재 Wafer의 세트 (Lot; FOUP)에 저장(등록)된 Substrate ID 목록 반환
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [out] loc_id에 위치한 Wafer의 세트 (Lot; FOUP)의 Substrate ID 목록이 저장될 리스트 버퍼
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetBatchSubstIDMap(PTCHAR loc_id, CStringArray &subst_id)
{
	LONG lItemCount	= 0, lHandle, i = 1;
	HRESULT hResult	= S_OK;
	ICxBatchLocPtr pICxBatchLoc;
	ICxValueDispPtr valSubstIDs(CLSID_CxValueObject);
	CComBSTR bstrSubstID;

	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Batch Substrate ID에 대한 인터페이스 포인터 반환 */
	if (!GetBatchSubstIDPtr(loc_id, &pICxBatchLoc))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetBatchSubstIDMap::get_BatchSubstIDMap (%s)", loc_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxBatchLoc->get_BatchSubstIDMap(&valSubstIDs);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		if (valSubstIDs)
		{
			DispErrorData(valSubstIDs, E30_SVET::en_acka);
			return FALSE;
		}
	}

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetBatchSubstIDMap::valSubstIDs::lItemCount (%s)", loc_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= valSubstIDs->ItemCount(0, &lItemCount);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 최상위의 부모 핸들 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetBatchSubstIDMap::valSubstIDs::ListAt (%s)", loc_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= valSubstIDs->ListAt(0, 0, &lHandle);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 등록된 개수만큼 가져오기 */
	for (; i<=lItemCount; i++)
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetBatchSubstIDMap::valSubstIDs::GetValueAscii (index=%d)", i);
		m_pLogData->PushLogs(tzMesg);
		hResult= valSubstIDs->GetValueAscii(lHandle, i, &bstrSubstID);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		/* List 반환 버퍼에 등록 */
		subst_id.Add((LPTSTR)_bstr_t(bstrSubstID));
	}

	return TRUE;
}

/*
 desc : 현재 Wafer의 세트 (Lot; FOUP)에 저장(등록)된 Substrate ID 목록 저장 (등록)
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [in]  loc_id에 위치한 Wafer의 세트 (Lot; FOUP)의 Substrate ID 목록이 저장된 리스트 버퍼
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::SetBatchSubstIDMap(PTCHAR loc_id, CStringArray *subst_id)
{
	LONG lItemCount	= 0, lHandle, i = 1;
	HRESULT hResult	= S_OK;
	ICxBatchLocPtr pICxBatchLoc;
	ICxValueDispPtr valSubstIDs(CLSID_CxValueObject);
	CComBSTR bstrSubstID;
	CString strSubstID;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Batch Substrate ID에 대한 인터페이스 포인터 반환 */
	if (!GetBatchSubstIDPtr(loc_id, &pICxBatchLoc))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetBatchSubstIDMap::valSubstIDs::AppendList (%s)", loc_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= valSubstIDs->AppendList(0, &lHandle);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* SubstIDs 등록 */
	for (; i<=subst_id->GetCount(); i++)
	{
		strSubstID = subst_id->GetAt(i);
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetBatchSubstIDMap::AppendValueAscii(%d, %s)",
					lHandle, strSubstID.GetBuffer());
		m_pLogData->PushLogs(tzMesg);
		hResult	= valSubstIDs->AppendValueAscii(lHandle, CComBSTR(strSubstID.GetBuffer()));
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	}

	return TRUE;
}

/*
 desc : Location ID에 대한 이벤트를 보낼지 여부에 대한 정보 얻기
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		flag	- [out] FALSE : 이벤트 보냄 (알림), TRUE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
BOOL CE90STS::GetDisableEventsLoc(PTCHAR loc_id, BOOL &flag)
{
	HRESULT hResult	= S_OK;
	ICxSubstrateLocationPtr pICxSubstLocPtr;
	VARIANT_BOOL varFlag;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Substrate Location ID 인터페이스 포인터 얻기 */
	if (!GetSubstrateLocIDPtr(loc_id, &pICxSubstLocPtr))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetDisableEventsLoc::get_disableEvents loc_id=%s", loc_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstLocPtr->get_disableEvents(&varFlag);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	flag	= varFlag == VARIANT_TRUE ? TRUE : FALSE;

	return TRUE;
}

/*
 desc : Location ID에 대한 이벤트를 보낼지 여부에 대한 정보 설정
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		flag	- [out] FALSE : 이벤트 보냄 (알림), TRUE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
BOOL CE90STS::SetDisableEventsLoc(PTCHAR loc_id, BOOL flag)
{
	HRESULT hResult	= S_OK;
	ICxSubstrateLocationPtr pICxSubstLocPtr;
	VARIANT_BOOL varFlag = flag == TRUE ? VARIANT_TRUE : VARIANT_FALSE;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Substrate Location ID 인터페이스 포인터 얻기 */
	if (!GetSubstrateLocIDPtr(loc_id, &pICxSubstLocPtr))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetDisableEventsLoc::put_DisableEvents loc_id=%s enabled=%d",
			   loc_id, flag);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstLocPtr->put_disableEvents(varFlag);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Location ID에 대한 이벤트를 보낼지 여부에 대한 정보 얻기
 parm : batch_id- [in]  ID of substrate batch
		flag	- [out] FALSE : 이벤트 보냄 (알림), TRUE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
BOOL CE90STS::GetDisableEventsBatch(PTCHAR batch_id, BOOL &flag)
{
	HRESULT hResult	= S_OK;
	ICxBatchLocPtr pICxBatchLoc;
	VARIANT_BOOL varFlag;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Batch Substrate ID에 대한 인터페이스 포인터 반환 */
	if (!GetBatchSubstIDPtr(batch_id, &pICxBatchLoc))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetDisableEventsBatch::get_disableEvents batch_id=%s", batch_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxBatchLoc->get_disableEvents(&varFlag);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	flag	= varFlag == VARIANT_TRUE ? TRUE : FALSE;

	return TRUE;
}

/*
 desc : Location ID에 대한 이벤트를 보낼지 여부에 대한 정보 설정
 parm : batch_id- [in]  ID of substrate batch
		flag	- [out] FALSE : 이벤트 보냄 (알림), TRUE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
BOOL CE90STS::SetDisableEventsBatch(PTCHAR batch_id, BOOL flag)
{
	HRESULT hResult	= S_OK;
	ICxBatchLocPtr pICxBatchLoc;
	VARIANT_BOOL varFlag = flag == TRUE ? VARIANT_TRUE : VARIANT_FALSE;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Batch Substrate ID에 대한 인터페이스 포인터 반환 */
	if (!GetBatchSubstIDPtr(batch_id, &pICxBatchLoc))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetDisableEventsBatch::put_DisableEvents batch_id=%s Enable=%d",
			   batch_id, flag);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxBatchLoc->put_disableEvents(varFlag);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Batch (Wafer Collections)에 등록할 수 있는 최대 개수 반환
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		max_pos	- [in]  최대 등록 가능한 Substrate position 값이 반환될 버퍼
 retn : TRUE or FALSE
*/
BOOL CE90STS::GetMaxSubstratePosition(PTCHAR loc_id, LONG &max_pos)
{
	HRESULT hResult	= S_OK;
	ICxBatchLocPtr pICxBatchLoc;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Batch Substrate ID에 대한 인터페이스 포인터 반환 */
	if (!GetBatchSubstIDPtr(loc_id, &pICxBatchLoc))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetMaxSubstratePosition::get_MaxSubstratePosition (%s)", loc_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxBatchLoc->get_MaxSubstratePosition(&max_pos);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Batch (Wafer Collections)에 등록할 수 있는 최대 개수 반환
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		name	- [out] Object Name이 저장될 버퍼
		size	- [in]  'name' 버퍼의 크기
 retn : TRUE or FALSE
*/
BOOL CE90STS::GetObjectName(PTCHAR loc_id, PTCHAR name, UINT32 size)
{
	HRESULT hResult	= S_OK;
	ICxBatchLocPtr pICxBatchLoc;
	CComBSTR bstrName;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Batch Substrate ID에 대한 인터페이스 포인터 반환 */
	if (!GetBatchSubstIDPtr(loc_id, &pICxBatchLoc))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetObjectName::get_ObjName (%s)", loc_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxBatchLoc->get_ObjName(&bstrName);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Object Name 값 반환 */
	swprintf_s(name, size, L"%s", (LPCTSTR)_bstr_t(bstrName));

	return TRUE;
}

/*
 desc : Update <batch location> GEM data (Batch Location (위치) GEM Data 갱신을 알림?)
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
 note : Updates the batch location status variables
*/
BOOL CE90STS::SendDataNotificationBatch(PTCHAR loc_id)
{
	HRESULT hResult	= S_OK;
	ICxBatchLocPtr pICxBatchLoc;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Batch Substrate ID에 대한 인터페이스 포인터 반환 */
	if (!GetBatchSubstIDPtr(loc_id, &pICxBatchLoc))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SendDataNotificationBatch::SendDataNotification (%s)", loc_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxBatchLoc->SendDataNotification();
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Update <substrate location> GEM variables for this object
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
 note : GEM 변수 - SubstID, SubstLocID, SubstLocState, SubstLocState_i, and SubstLocSubstID_i
*/
BOOL CE90STS::SendDataNotificationSubst(PTCHAR loc_id)
{
	HRESULT hResult	= S_OK;
	ICxSubstrateLocationPtr pICxSubstLocPtr;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Substrate Location ID 인터페이스 포인터 얻기 */
	if (!GetSubstrateLocIDPtr(loc_id, &pICxSubstLocPtr))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SendDataNotificationSubst::SendDataNotification (%s)", loc_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstLocPtr->SendDataNotification();
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Substrate Batch Location ID 생성
 parm : loc_id	- [in]  생성하고자 하는 Batch Location ID (문자열)
		loc_num	- [in]  Batch Location에서 substrate locations의 개수
						(Carrier 내에 등록? 가능한 substrate location 개수)
		sub_id	- [in]  Location Group ID를 위해 사용하는 Subscript Index (location's index)
						EPJ 파일에서 해당 위치에 사용되는 첨자?(Subscripted) 변수의 세트 (set)
						-> EPJ 파일에 정의된 내용과 연관성이 있을듯...
						사용하지 않을 때는 0
 retn : TRUE or FALSE
*/
BOOL CE90STS::AddSubstrateBatchLocation(PTCHAR loc_id, INT32 loc_num, INT16 sub_id)
{
	HRESULT hResult	= S_OK;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"AddSubstrateBatchLocation::AddSubstrateBatchLocation2 "
										  L"loc_id:%s, loc_num:%d, sub_id:%d", loc_id, loc_num, sub_id);
	m_pLogData->PushLogs(tzMesg);
	if (sub_id > 0)
	{
		hResult	= m_pICxE90STPtr->AddSubstrateBatchLocation2(CComBSTR(loc_id), loc_num, sub_id);
	}
	else
	{
		hResult	= m_pICxE90STPtr->AddSubstrateBatchLocation(CComBSTR(loc_id), loc_num);
	}
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Substrate Batch Location ID 제거
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
*/
BOOL CE90STS::DelSubstrateBatchLocation(PTCHAR loc_id)
{
	HRESULT hResult	= S_OK;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DelSubstrateBatchLocation::RemoveSubstrateBatchLocation "
										  L"loc_id:%s", loc_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->RemoveSubstrateBatchLocation(CComBSTR(loc_id));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Batch에 캐리어의 기판을 추가 함
 parm : batch_id	- [in]  ID of substrate batch
		carr_id		- [in]  ID of carrier
		slot_map	- [in]  1 or 0 or -1
							TRUE : 각 Slot map item = 0에 대해 위치 값을 비워서 추가 함
								   즉, 각 Slot의 위치 값을 초기화 후 추가하도록 처리
								   (사용하지 않을 때는 -1)
 retn : TRUE or FALSE
 note : Batch	- 프로세스 자원에서 동시에 처리할 기판의 그룹 (뭉치?)
*/
BOOL CE90STS::AddCarrierToBatch(LONG batch_id, PTCHAR carr_id, LONG slot_map)
{
	HRESULT hResult	= S_OK;
	VARIANT_BOOL varSlotMap	= slot_map > 0 ? VARIANT_TRUE : VARIANT_FALSE;	/* preserved Slot Map */
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"AddCarrierToBatch::AddCarrierToBatch2 "
										  L"batch_id:%d, carr_id:%s, slot_map:%d",
			   batch_id, carr_id, slot_map);
	m_pLogData->PushLogs(tzMesg);
	if (slot_map >= 0)
	{
		hResult	= m_pICxE90STPtr->AddCarrierToBatch2(batch_id, CComBSTR(carr_id), varSlotMap);
	}
	else
	{
		hResult	= m_pICxE90STPtr->AddCarrierToBatch(batch_id, CComBSTR(carr_id));
	}
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Batch에 캐리어의 기판을 제거 함
 parm : batch_id- [in]  ID of substrate batch
		carr_id	- [in]  ID of carrier
 retn : TRUE or FALSE
 note : Batch	- 프로세스 자원에서 동시에 처리할 기판의 그룹 (뭉치?)
*/
BOOL CE90STS::DelCarrierFromBatch(LONG batch_id, PTCHAR carr_id)
{
	HRESULT hResult	= S_OK;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DelCarrierToBatch::RemoveCarrierFromBatch "
										  L"batchID:%d, carr_id:%s", batch_id, carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->RemoveCarrierFromBatch(batch_id, CComBSTR(carr_id));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : 새로운 Substrate Batch 생성
 parm : batch_id- [in]  ID of substrate batch
		loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
*/
BOOL CE90STS::CreateBatch(LONG batch_id, PTCHAR loc_id)
{
	HRESULT hResult	= S_OK;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"CreateBatch::CreateBatch "
										  L"batchID:%d, substBatchLocID:%s", batch_id, loc_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->CreateBatch(batch_id, CComBSTR(loc_id));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : 기존 Substrate Batch 제거
 parm : batch_id- [in]  ID of substrate batch
 retn : TRUE or FALSE
*/
BOOL CE90STS::DestroyBatch(LONG batch_id)
{
	HRESULT hResult	= S_OK;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DestroyBatch::DestroyBatch batchID:%d", batch_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxE90STPtr->DestroyBatch(batch_id);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Batch에 있는 모든 기판들의 이송 및 처리 상태 뿐만 아니라, 위치도 변경 함
 parm : batch_id	- [in]  ID of substrate batch
		loc_id		- [in]  위치 변경하고자 하는 ID (Location ID: Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		trans_state	- [in]  이송 상태 값 즉, E90_SSTS
		proc_state	- [in]  처리 상태 값 즉, E90_SSPS
		time_in		- [in]  Batch가 Location ID에 도착하는 시간 (NULL이면 사용 안함)
		time_out	- [in]  Batch가 Location ID에 이탈하는 시간 (NULL이면 사용 안함)
							time format : YYYYMMDDhhmmsscc for YearMonthDayHourMinuteSecondCentisecond
 retn : TRUE or FALSE
*/
BOOL CE90STS::ChangeBatchState(LONG batch_id, PTCHAR loc_id,
							   E90_SSTS trans_state, E90_SSPS proc_state,
							   PTCHAR time_in, PTCHAR time_out)
{
	HRESULT hResult	= S_OK;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"ChangeBatchState::ChangeBatchState "
										  L"batchID:%d, loc_id=%s, trans_state=%d, proc_state=%d",
			   batch_id, loc_id, trans_state, proc_state);
	m_pLogData->PushLogs(tzMesg);
	if (time_in && time_out)
	{
		hResult	= m_pICxE90STPtr->ChangeBatchStateAndHistory(batch_id, loc_id,
															 SubstrateTransportStateEnum(trans_state),
															 SubstrateProcessingStateEnum(proc_state),
															 CComBSTR(time_in), CComBSTR(time_out));
	}
	else
	{
		hResult	= m_pICxE90STPtr->ChangeBatchState(batch_id, loc_id,
												   SubstrateTransportStateEnum(trans_state),
												   SubstrateProcessingStateEnum(proc_state));
	}
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Substrate Location 등록
 parm : loc_id	- [in]  위치 변경하고자 하는 ID (Location ID: Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		sub_id	- [in]  Location Group ID를 위해 사용하는 Subscript Index (location's index)
						EPJ 파일에서 해당 위치에 사용되는 첨자?(Subscripted) 변수의 세트 (set)
						-> EPJ 파일에 정의된 내용과 연관성이 있을듯...
						사용하지 않을 때는 0 값
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::AddSubstrateLocation(PTCHAR loc_id, PTCHAR subst_id, INT16 sub_id)
{
	HRESULT hResult	= S_OK;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"AddSubstrateLocation::AddSubstrateLocation "
										  L"loc_id=%s, subst_id=%s, sub_id=%d",
			   loc_id, subst_id, sub_id);
	m_pLogData->PushLogs(tzMesg);
	if (sub_id > 0)
	{
		hResult	= m_pICxE90STPtr->AddSubstrateLocation2(CComBSTR(loc_id), CComBSTR(subst_id), sub_id);
	}
	else
	{
		hResult	= m_pICxE90STPtr->AddSubstrateLocation(CComBSTR(loc_id), CComBSTR(subst_id));
	}
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Substrate Location 제거
 parm : loc_id	- [in]  위치 변경하고자 하는 ID (Location ID: Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
*/
BOOL CE90STS::DelSubstrateLocation(PTCHAR loc_id)
{
	HRESULT hResult	= S_OK;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DelSubstrateLocation::RemoveSubstrateLocation "
										  L"loc_id=%s", loc_id);
	hResult	= m_pICxE90STPtr->RemoveSubstrateLocation(CComBSTR(loc_id));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Batch에 Substrate 추가
 parm : batch_id- [in]  ID of substrate batch
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
 retn : TRUE or FALSE
 note : Batch	- 프로세스 자원에서 동시에 처리할 기판의 그룹 (뭉치?)
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::AddSubstrateToBatch(LONG batch_id, PTCHAR subst_id)
{
	HRESULT hResult	= S_OK;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"AddSubstrateToBatch::AddSubstrateToBatch "
										  L"batch_id=%d, subst_id=%s", batch_id, subst_id);
	hResult	= m_pICxE90STPtr->AddSubstrateToBatch(batch_id, CComBSTR(subst_id));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Batch에서 Substrate 제거
 parm : batch_id- [in]  ID of substrate batch
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
 retn : TRUE or FALSE
 note : Batch	- 프로세스 자원에서 동시에 처리할 기판의 그룹 (뭉치?)
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::DelSubstrateFromBatch(LONG batch_id, PTCHAR subst_id)
{
	HRESULT hResult	= S_OK;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DelSubstrateToBatch::RemoveSubstrateToBatch "
										  L"batch_id=%d, subst_id=%s", batch_id, subst_id);
	hResult	= m_pICxE90STPtr->RemoveSubstrateFromBatch(batch_id, CComBSTR(subst_id));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Batch에 비어있는 substrate 공간을 추가 즉, Substrate가 없는 Batch만 추가
 parm : batch_id- [in]  ID of substrate batch
 retn : TRUE or FALSE
 note : Batch	- 프로세스 자원에서 동시에 처리할 기판의 그룹 (뭉치?)
*/
BOOL CE90STS::AddEmptySpaceToBatch(LONG batch_id)
{
	HRESULT hResult	= S_OK;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"AddEmptySpaceToBatch::AddEmptySpaceToBatch "
										  L"batch_id=%d", batch_id);
	hResult	= m_pICxE90STPtr->AddEmptySpaceToBatch(batch_id);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : subscript indices의 범위를 예약 (?)
 parm : base_sub_index	- [in]  first subscript index to reserve
		reserve_size	- [in]  number of indices to reserve
 retn : TRUE or FALSE
*/
BOOL CE90STS::ReserveSubstrateLocationIndices(INT16 base_sub_index, INT16 reserve_size)
{
	HRESULT hResult	= S_OK;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"ReserveSubstrateLocationIndices::ReserveSubstrateLocationIndices "
										  L"base_subscript_index=%d, reserve_size=%d",
			   base_sub_index, reserve_size);
	hResult	= m_pICxE90STPtr->ReserveSubstrateLocationIndices(base_sub_index, reserve_size);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Substrate Transport State 변경
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		state	- [in]  E90_SSTS (Substrate Transport State)
		time_in	- [in]  Batch가 Location ID에 도착하는 시간 (NULL이면 사용 안함)
		time_out- [in]  Batch가 Location ID에 이탈하는 시간 (NULL이면 사용 안함)
						time format : YYYYMMDDhhmmsscc for YearMonthDayHourMinuteSecondCentisecond
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::ChangeSubstrateTransportState(PTCHAR subst_id, PTCHAR loc_id, E90_SSTS state,
											PTCHAR time_in, PTCHAR time_out)
{
	HRESULT hResult	= S_OK;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"ChangeSubstrateTransportState::ChangeSubstrateTransportState "
										  L"subst_id=%s, loc_id=%s, state=%d, time_in=%s, time_out=%s",
			   subst_id, loc_id, state, time_in, time_out);
	hResult	= m_pICxE90STPtr->ChangeSubstrateTransportState(CComBSTR(subst_id), CComBSTR(loc_id),
															SubstrateTransportStateEnum(state),
															CComBSTR(time_in), CComBSTR(time_out));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Substrate Material Status 값 반환 (기판의 처리 상태 값 반환. 처리 여부 ?)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		status	- [out] Material Status (Material 처리 여부) 값이 반환될 참조 변수
						장치마다 반환되는 값이 다르다? (EPJ 파일 참조?)
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSubstrateMaterialStatus(PTCHAR subst_id, LONG &status)
{
	HRESULT hResult	= S_OK;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetSubstrateMaterialStatus::GetSubstrateMaterialStatus "
										  L"subst_id=%s", subst_id);
	hResult	= m_pICxE90STPtr->GetSubstrateMaterialStatus(CComBSTR(subst_id), &status);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Substrate Material Status 값 변경 (기판의 처리 상태 값 변경. 처리 여부 ?)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		status	- [in]  Material Status (Material 처리 여부) 값
						장치마다 설정하는는 값이 다르다? (EPJ 파일 참조?)
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::SetSubstrateMaterialStatus(PTCHAR subst_id, LONG status)
{
	HRESULT hResult	= S_OK;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetSubstrateMaterialStatus::SetSubstrateMaterialStatus "
										  L"subst_id=%s", subst_id);
	hResult	= m_pICxE90STPtr->SetSubstrateMaterialStatus(CComBSTR(subst_id), status);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : 장비에 다루는 Substrate의 Type 설정 (Wafer, Flat Panel, CD, Mask, etc...)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		type	- [in]  E90_EMST 즉, wafer, flat_panel, cd, mask 중 하나
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::SetCurrentSubstrateType(PTCHAR subst_id, E90_EMST type)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Equipment Type 설정 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetCurrentSubstrateType::put_currentEquipmentType : st_id=%s, type=%d",
			   subst_id, type);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->put_currentEquipmentType(SubstrateEquipmentTypeEnum(type));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : 장비에 다루는 Substrate의 Type 설정 (Wafer, Flat Panel, CD, Mask, etc...)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		type	- [out] E90_EMST 즉, wafer, flat_panel, cd, mask 중 하나
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetCurrentSubstrateType(PTCHAR subst_id, E90_EMST &type)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	SubstrateEquipmentTypeEnum enType;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Equipment Type 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"GetCurrentSubstrateType::get_currentEquipmentType : st_id=%s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_currentEquipmentType(&enType);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	type	= E90_EMST(enType);

	return (hResult == S_OK);
}

/*
 desc : Substrate ID가 Reader기로부터 성공적으로 읽어졌는지 즉, Host로부터 Confirm을 받았는지 여부
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		read	- [out] 정상적으로 ID를 읽었는지 여부 값 (TRUE or FALSE)이 반환될 참조 변수
 retn : TRUE or FALSE
 note : Equipment에서는 NotifySubstrateRead 함수 호출해서 Substrate ID가 정상인지를 Host에게 요청
		이 NotifySubstrateRead 함수 호출 후에, 현재 함수 호출하면 goodRead인지 badRead인지 알 수 있음
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSubstrateGoodRead(PTCHAR subst_id, BOOL &read)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	VARIANT_BOOL varRead;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Read 성공 여부 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"GetSubstrateGoodRead::get_goodRead : st_id=%s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_goodRead(&varRead);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	read	= varRead == VARIANT_TRUE ? TRUE : FALSE;

	return (hResult == S_OK);
}

/*
 desc : Substrate ID가 Reader기로부터 읽어들인 상태 값 즉, TRUE (good) or FALSE (bad) 값 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		read	- [in]  정상적으로 ID를 읽었다면 TRUE, 아니라면 FALSE 설정
 retn : TRUE or FALSE
 note : Equipment에서는 NotifySubstrateRead 함수 호출해서 Substrate ID가 정상인지를 Host에게 요청
		이 NotifySubstrateRead 함수 호출 후에, 현재 함수 호출하면 goodRead인지 badRead인지 알 수 있음
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::SetSubstrateGoodRead(PTCHAR subst_id, BOOL read)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	VARIANT_BOOL varRead = read ? VARIANT_TRUE : VARIANT_FALSE;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Read 성공 여부 설정 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetSubstrateGoodRead::put_goodRead : st_id=%s, read=%d", subst_id, read);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->put_goodRead(varRead);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Substrate ID와 관련된 Lot ID 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		lot_id	- [out] Lot ID가 저장될 버퍼
		size	- [in]  'lot_id' 버퍼 크기
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSubstrateLotID(PTCHAR subst_id, PTCHAR lot_id, UINT32 size)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrLotID;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Lot ID 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"GetSubstrateLotID::get_lotID : st_id=%s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_lotID(&bstrLotID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	swprintf_s(lot_id, size, L"%s", (LPTSTR)_bstr_t(bstrLotID));

	return (hResult == S_OK);
}

/*
 desc : Substrate ID와 관련된 Lot ID 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		lot_id	- [in]  Lot ID가 저장된 버퍼
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::SetSubstrateLotID(PTCHAR subst_id, PTCHAR lot_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Lot ID 설정 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"GetSubstrateLotID::put_lotID : st_id=%s, lot_id=%s", subst_id, lot_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->put_lotID(CComBSTR(lot_id));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Substrate ID와 관련된 Object ID 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		obj_id	- [out] Object ID가 저장될 버퍼 (여기서는 Substrate ID와 동일한 텍스트?)
		size	- [in]  'lot_id' 버퍼 크기
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSubstrateObjID(PTCHAR subst_id, PTCHAR obj_id, UINT32 size)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrObjID;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Object ID 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"GetSubstrateObjID::get_objectID : st_id=%s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_objectID(&bstrObjID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	swprintf_s(obj_id, size, L"%s", (LPTSTR)_bstr_t(bstrObjID));

	return (hResult == S_OK);
}

/*
 desc : Substrate ID와 관련된 Object Type 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		obj_type- [out] Object Type이 저장될 버퍼 ("BatchLoc" or "Substrate" or "SubstLoc")
						여기서는 보통 "Substrate"
		size	- [in]  'obj_type' 버퍼 크기
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSubstrateObjType(PTCHAR subst_id, PTCHAR obj_type, UINT32 size)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrObjType;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Object Type 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"GetSubstrateObjID::get_objectType : st_id=%s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_objectType(&bstrObjType);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	swprintf_s(obj_type, size, L"%s", (LPTSTR)_bstr_t(bstrObjType));

	return (hResult == S_OK);
}

/*
 desc : Location ID와 관련된 Object ID 반환
 parm : loc_id	- [out] Location ID가 저장될 버퍼
		obj_id	- [out] Object ID가 저장될 버퍼 (여기서는 Location ID와 동일한 텍스트?)
		size	- [in]  'lot_id' 버퍼 크기
 retn : TRUE or FALSE
*/
BOOL CE90STS::GetLocationObjID(PTCHAR loc_id, PTCHAR obj_id, UINT32 size)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrObjID;
	ICxSubstrateLocationPtr pICxSubstLocPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateLocIDPtr(loc_id, &pICxSubstLocPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Object ID 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"GetLocationObjID::get_objectID : loc_id=%s", loc_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstLocPtr->get_objectID(&bstrObjID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	swprintf_s(obj_id, size, L"%s", (LPTSTR)_bstr_t(bstrObjID));

	return (hResult == S_OK);
}

/*
 desc : Location ID와 관련된 Object Type 반환
 parm : loc_id	- [out] Location ID가 저장될 버퍼
		obj_type- [out] Object Type이 저장될 버퍼 ("BatchLoc" or "Substrate" or "SubstLoc")
						여기서는 보통 "Substrate"
		size	- [in]  'obj_type' 버퍼 크기
 retn : TRUE or FALSE
*/
BOOL CE90STS::GetLocationObjType(PTCHAR loc_id, PTCHAR obj_type, UINT32 size)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrObjType;
	ICxSubstrateLocationPtr pICxSubstLocPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateLocIDPtr(loc_id, &pICxSubstLocPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Object Type 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"GetLocationObjType::get_objectType : loc_id=%s", loc_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstLocPtr->get_objectType(&bstrObjType);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	swprintf_s(obj_type, size, L"%s", (LPTSTR)_bstr_t(bstrObjType));

	return (hResult == S_OK);
}

/*
 desc : Substrate가 속한 Source CarrierID 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		carr_id	- [out] sourceCarrierID가 저장될 버퍼
		size	- [in]  'carr_id' 버퍼 크기
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSourceCarrierID(PTCHAR subst_id, PTCHAR carr_id, UINT32 size)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrCarrID;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Source Carrier ID 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"GetSourceCarrierID::get_sourceCarrierID : st_id=%s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_sourceCarrierID(&bstrCarrID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	swprintf_s(carr_id, size, L"%s", (LPTSTR)_bstr_t(bstrCarrID));

	return (hResult == S_OK);
}

/*
 desc : Substrate가 속할 Source CarrierID 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		carr_id	- [in]  sourceCarrierID가 저장될 버퍼
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::SetSourceCarrierID(PTCHAR subst_id, PTCHAR carr_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrCarrID;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Source Carrier ID 설정 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetSourceCarrierID::put_sourceCarrierID : st_id=%s, carrier_id=%s",
			   subst_id, carr_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->put_sourceCarrierID(bstrCarrID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Substrate가 속한 Source LoadPort ID 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		port_id	- [out] Load Port ID가 저장될 참조 변수 (1 or Later)
 retn : TRUE or FALSE
 note : Load Port - the interface location on the equipment where carriers are loaded and unloaded
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSourceLoadPortID(PTCHAR subst_id, LONG &port_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	INT32 i32PortID	= -1;
	HRESULT hResult	= S_OK;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Source Carrier ID 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetSourceLoadPortID::get_sourcePortID : st_id=%s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_sourcePortID(&i32PortID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	port_id	= i32PortID;

	return (hResult == S_OK);
}

/*
 desc : Substrate가 속할 Source LoadPort ID 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		port_id	- [in]  sourceLoadPortID가 저장될 버퍼 (1 or Later)
 retn : TRUE or FALSE
 note : Load Port - the interface location on the equipment where carriers are loaded and unloaded
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::SetSourceLoadPortID(PTCHAR subst_id, LONG port_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Source Carrier ID 설정 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetSourceLoadPortID::put_sourcePortID : st_id=%s, port_id=%d", subst_id, port_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->put_sourcePortID(port_id);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Substrate가 속한 Source Slot ID 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		port_id	- [out] Slot ID가 저장될 참조 변수 (1 or Later)
 retn : TRUE or FALSE
 note : slot - physical location capable of holding a substrate within cassette type carrier
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSourceSlotID(PTCHAR subst_id, LONG &slot_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	INT32 i32SlotID	= -1;
	HRESULT hResult	= S_OK;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Source Carrier ID 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetSourceSlotID::get_sourceSlotID : st_id=%s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_sourceSlotID(&i32SlotID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	slot_id	= i32SlotID;

	return (hResult == S_OK);
}

/*
 desc : Substrate가 속할 Source Slot ID 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		port_id	- [in]  sourceSlotID가 저장될 버퍼 (1 or Later)
 retn : TRUE or FALSE
 note : slot - physical location capable of holding a substrate within cassette type carrier
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::SetSourceSlotID(PTCHAR subst_id, LONG slot_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Source Carrier ID 설정 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetSourceSlotID::put_sourcePortID : st_id=%s, slot_id=%d", subst_id, slot_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->put_sourceSlotID(slot_id);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Substrate의 마지막 이벤트 (작업)와 관련된 Substrate의 현재 위치 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		loc_id	- [out] Location ID가 저장될 버퍼
		size	- [in]  'loc_id' 버퍼 크기
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetCurrentSubstrateLocID(PTCHAR subst_id, PTCHAR loc_id, UINT32 size)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	INT32 i32SlotID	= -1;
	HRESULT hResult	= S_OK;
	CComBSTR bstrLocID;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Source Carrier ID 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetCurrentSubstrateLocID::get_substSubstLocID : st_id=%s",
			   subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_substSubstLocID(&bstrLocID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	swprintf_s(loc_id, size, L"%s", (LPTSTR)_bstr_t(bstrLocID));

	return (hResult == S_OK);
}

/*
 desc : Substrate의 마지막 이벤트 (작업)와 관련된 Substrate의 현재 위치 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		loc_id	- [in]  Location ID가 저장된 버퍼
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::SetCurrentSubstrateLocID(PTCHAR subst_id, PTCHAR loc_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Source Carrier ID 설정 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetSourceSlotID::put_substSubstLocID : st_id=%s, loc_id=%s", subst_id, loc_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->put_substSubstLocID(CComBSTR(loc_id));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Substrate의 Source ID 얻기
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		src_id	- [out] Source ID가 저장될 버퍼
		size	- [in]  'src_id' 버퍼 크기
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSubstrateSourceID(PTCHAR subst_id, PTCHAR src_id, UINT32 size)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	INT32 i32SlotID	= -1;
	HRESULT hResult	= S_OK;
	CComBSTR bstrSrcID;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Source Carrier ID 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetSubstrateSourceID::get_substrateSource : st_id=%s",
			   subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_substrateSource(&bstrSrcID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	swprintf_s(src_id, size, L"%s", (LPTSTR)_bstr_t(bstrSrcID));

	return (hResult == S_OK);
}

/*
 desc : Substrate의 Source ID 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		src_id	- [in]  Source ID가 저장된 버퍼
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::SetSubstrateSourceID(PTCHAR subst_id, PTCHAR src_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Source Carrier ID 설정 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetSubstrateSourceID::put_substrateSource : st_id=%s, src_id=%s", subst_id, src_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->put_substrateSource(CComBSTR(src_id));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Substrate의 Destination ID 얻기
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		tgt_id	- [out] Source ID가 저장될 버퍼
		size	- [in]  'src_id' 버퍼 크기
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::GetSubstrateDestinationID(PTCHAR subst_id, PTCHAR tgt_id, UINT32 size)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	INT32 i32SlotID	= -1;
	HRESULT hResult	= S_OK;
	CComBSTR bstrTgtID;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Source Carrier ID 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetSubstrateDestinationID::get_substrateDestination : st_id=%s",
			   subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_substrateDestination(&bstrTgtID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	swprintf_s(tgt_id, size, L"%s", (LPTSTR)_bstr_t(bstrTgtID));

	return (hResult == S_OK);
}

/*
 desc : Substrate의 Destination ID 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		tgt_id	- [in]  Source ID가 저장된 버퍼
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
BOOL CE90STS::SetSubstrateDestinationID(PTCHAR subst_id, PTCHAR tgt_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	ICxSubstratePtr pICxSubstPtr;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* ICxSubstratePtr를 통해서 Source Carrier ID 설정 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetSubstrateDestinationID::put_substrateDestination : st_id=%s, tgt_id=%s",
			   subst_id, tgt_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->put_substrateDestination(CComBSTR(tgt_id));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : 동일한 전이 (Transition)가 진행되는 관련 개체 그룹에 대해 전송되는 개별 이벤트를 억제하는데 사용
		객체 (Object)가 개별 이벤트를 억제하는지 여부 확인
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		flag	- [out] TRUE : 이벤트 보냄 (알림), FALSE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
BOOL CE90STS::GetUsingGroupEventsSubst(PTCHAR subst_id, BOOL &flag)
{
	HRESULT hResult	= S_OK;
	ICxSubstratePtr pICxSubstPtr;
	VARIANT_BOOL varFlag;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Substrate ID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetUsingGroupEventsSubst::get_UsingGroupEvents");
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_UsingGroupEvents(&varFlag);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	flag	= varFlag == VARIANT_TRUE ? TRUE : FALSE;

	return TRUE;
}

/*
 desc : 동일한 전이 (Transition)가 진행되는 관련 개체 그룹에 대해 전송되는 개별 이벤트를 억제하는데 사용
		객체 (Object)가 개별 이벤트를 억제 혹은 승인하는지 여부 확인
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		flag	- [in]  TRUE : 이벤트 보냄 (알림), FALSE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
BOOL CE90STS::SetUsingGroupEventsSubst(PTCHAR subst_id, BOOL flag)
{
	HRESULT hResult	= S_OK;
	ICxSubstratePtr pICxSubstPtr;
	VARIANT_BOOL varFlag = flag ? VARIANT_TRUE : VARIANT_FALSE;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Substrate ID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetUsingGroupEventsSubst::put_UsingGroupEvents");
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->put_UsingGroupEvents(varFlag);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : 동일한 전이 (Transition)가 진행되는 관련 개체 그룹에 대해 전송되는 개별 이벤트를 억제하는데 사용
		객체 (Object)가 개별 이벤트를 억제하는지 여부 확인
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		flag	- [out] TRUE : 이벤트 보냄 (알림), FALSE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
BOOL CE90STS::GetUsingGroupEventsLoc(PTCHAR loc_id, BOOL &flag)
{
	HRESULT hResult	= S_OK;
	ICxSubstrateLocationPtr pICxSubstLocPtr;
	VARIANT_BOOL varFlag;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Substrate Location ID 인터페이스 포인터 얻기 */
	if (!GetSubstrateLocIDPtr(loc_id, &pICxSubstLocPtr))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetUsingGroupEventsLoc::get_UsingGroupEvents");
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstLocPtr->get_UsingGroupEvents(&varFlag);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	flag	= varFlag == VARIANT_TRUE ? TRUE : FALSE;

	return TRUE;
}

/*
 desc : 동일한 전이 (Transition)가 진행되는 관련 개체 그룹에 대해 전송되는 개별 이벤트를 억제하는데 사용
		객체 (Object)가 개별 이벤트를 억제 혹은 승인하는지 여부 확인
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		flag	- [in]  TRUE : 이벤트 보냄 (알림), FALSE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
BOOL CE90STS::SetUsingGroupEventsLoc(PTCHAR loc_id, BOOL flag)
{
	HRESULT hResult	= S_OK;
	ICxSubstrateLocationPtr pICxSubstLocPtr;
	VARIANT_BOOL varFlag = flag ? VARIANT_TRUE : VARIANT_FALSE;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Substrate Location ID 인터페이스 포인터 얻기 */
	if (!GetSubstrateLocIDPtr(loc_id, &pICxSubstLocPtr))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetUsingGroupEventsLoc::put_UsingGroupEvents");
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstLocPtr->put_UsingGroupEvents(varFlag);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Location에 위치한 Substrate ID 얻기
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [out] Substrate ID가 저장될 버퍼
		size	- [in]  'subst_id' 버퍼 크기
 retn : TRUE or FALSE
*/															   
BOOL CE90STS::GetLocationSubstID(PTCHAR loc_id, PTCHAR subst_id, UINT32 size)
{
	HRESULT hResult = S_OK;
	CComBSTR bstrSubstID;
	ICxSubstrateLocationPtr pICxSubstLocPtr;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Substrate Location ID 인터페이스 포인터 얻기 */
	if (!GetSubstrateLocIDPtr(loc_id, &pICxSubstLocPtr))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetLocationSubstID::get_substrateID : loc_id=%s", loc_id);
	m_pLogData->PushLogs(tzMesg);
	
	hResult = pICxSubstLocPtr->get_substrateID(&bstrSubstID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Substrate 반환 */
	swprintf_s(subst_id, size, L"%s", (LPTSTR)_bstr_t(bstrSubstID));

	return TRUE;
}

/*
 desc : Location에 위치한 Substrate ID 설정
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [in]  Substrate ID가 저장된 버퍼
 retn : TRUE or FALSE
*/															   
BOOL CE90STS::SetLocationSubstID(PTCHAR loc_id, PTCHAR subst_id)
{
	HRESULT hResult = S_OK;
	ICxSubstrateLocationPtr pICxSubstLocPtr;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Substrate Location ID 인터페이스 포인터 얻기 */
	if (!GetSubstrateLocIDPtr(loc_id, &pICxSubstLocPtr))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"IsLocationSubstrateID::get_substrateID loc_id=%s, subst_id=%s",
			   loc_id, subst_id);
	m_pLogData->PushLogs(tzMesg);
	
	hResult = pICxSubstLocPtr->put_substrateID(CComBSTR(subst_id));
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Location에 위치한 Location Index 얻기
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		index	- [out] Location Index가 반환될 참조 변수 (1 or Later)
 retn : TRUE or FALSE
*/															   
BOOL CE90STS::GetLocationIndex(PTCHAR loc_id, INT16 &index)
{
	HRESULT hResult = S_OK;
	ICxSubstrateLocationPtr pICxSubstLocPtr;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Substrate Location ID 인터페이스 포인터 얻기 */
	if (!GetSubstrateLocIDPtr(loc_id, &pICxSubstLocPtr))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetLocationIndex::get_substrateLocationIndex : loc_id=%s", loc_id);
	m_pLogData->PushLogs(tzMesg);
	
	hResult = pICxSubstLocPtr->get_substrateLocationIndex(&index);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Location에 위치한 Substrate ID 설정
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		index	- [in]  Location Index 값 (1 or Later)
 retn : TRUE or FALSE
*/															   
BOOL CE90STS::SetLocationIndex(PTCHAR loc_id, INT16 index)
{
	HRESULT hResult = S_OK;
	ICxSubstrateLocationPtr pICxSubstLocPtr;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};

	/* Substrate Location ID 인터페이스 포인터 얻기 */
	if (!GetSubstrateLocIDPtr(loc_id, &pICxSubstLocPtr))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetLocationIndex::put_substrateLocationIndex loc_id=%s, index=%d",
			   loc_id, index);
	m_pLogData->PushLogs(tzMesg);
	
	hResult = pICxSubstLocPtr->put_substrateLocationIndex(index);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return TRUE;
}

/*
 desc : Substrate History 반환
 parm : subst_id- [in]  Substrate ID가 저장된 버퍼
		lst_hist- [out] { {loc_id, time_in, time_out}, {loc_id, time_in, time_out}, ..., } 값이 반환될 리스트
						time_in/out = YearMonthDayHourMinuteSecondCentisecond
 retn : TRUE or FALSE
 날짜 : 2021.03.08 (Modified by Jinsoo.Kang at philoptics)
 note : N/A
*/
BOOL CE90STS::GetSubstrateHistory(PTCHAR subst_id, CAtlList <STG_CSHI> &lst_hist)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	LONG lItemCount	= 0, lHandle[2]	= {NULL}, i = 1, lError = 0;
	HRESULT hResult	= S_OK;
	STG_CSHI stHist;
	CComBSTR bstrVal;
	ICxSubstratePtr pICxSubstPtr;
	ICxValueDispPtr valHist(CLSID_CxValueObject);

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
				L"GetSubstrateHistory::get_substrateHistory : st_id = %s", subst_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxSubstPtr->get_substrateHistory(&valHist);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 등록된 History 정보 존재 여부 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetSubstrateHistory::ItemCount");
	hResult	= valHist->ItemCount(0, &lItemCount);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	if (lItemCount < 1)	return TRUE;

	/* 최상위의 부모 핸들 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetSubstrateHistory::ListAt[0]");
	hResult	= valHist->ListAt(0, 0, &lHandle[0]);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 기존 등록된 값 메모리 초기화 */
	lst_hist.RemoveAll();

	/* 각 등록된 개수만큼 정보 가져오기 */
	for (; i<=lItemCount; i++)
	{
		memset(&stHist, 0x00, sizeof(STG_CSHI));

		/* Location ID, Time In, Time Out 정보를 순서대로 가져오기 위한 핸들 얻기 */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetSubstrateHistory::ListAt (handle:0 / index=0)");
		hResult	= valHist->ListAt(lHandle[0], i, &lHandle[1]);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		/* Location ID 얻기 */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetSubstrateHistory::Location ID (handle:%d)", lHandle[1]);
		hResult	= valHist->GetValueAscii(lHandle[1], 1, &bstrVal);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
		swprintf_s(stHist.loc_id, CIM_SUBSTRATE_LOC_ID, L"%s", (LPTSTR)_bstr_t(bstrVal));

		/* Time In 얻기 */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetSubstrateHistory::Time In (handle:%d)", lHandle[1]);
		hResult	= valHist->GetValueAscii(lHandle[1], 2, &bstrVal);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
		swprintf_s(stHist.time_in, CIM_TIME_SIZE, L"%s", (LPTSTR)_bstr_t(bstrVal));

		/* Time Out 얻기 */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetSubstrateHistory::Time Out (handle:%d)", lHandle[1]);
		hResult	= valHist->GetValueAscii(lHandle[1], 3, &bstrVal);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
		swprintf_s(stHist.time_out, CIM_TIME_SIZE, L"%s", (LPTSTR)_bstr_t(bstrVal));

		/* 리스트 버퍼에 등록 */
		lst_hist.AddTail(stHist);
	}

	return TRUE;
}

/*
 desc : Substrate History 반환
 parm : subst_id- [in]  Substrate ID가 저장된 버퍼
		lst_hist- [in]  { {loc_id, time_in, time_out}, {loc_id, time_in, time_out}, ..., } 값이 저장된 리스트
						time_in/out = YearMonthDayHourMinuteSecondCentisecond
 retn : TRUE or FALSE
 날짜 : 2021.03.08 (Modified by Jinsoo.Kang at philoptics)
 note : N/A
*/
BOOL CE90STS::SetSubstrateHistory(PTCHAR subst_id, CAtlList <STG_CSHI> *lst_hist)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	LONG lItemCount	= 0, lHandle[2]	= {NULL}, i = 1, lError = 0;
	HRESULT hResult	= S_OK;
	STG_CSHI stHist;
	CComBSTR bstrVal;
	ICxSubstratePtr pICxSubstPtr;
	ICxValueDispPtr valHist(CLSID_CxValueObject);

	/* List에 등록된 개수 확인 */
	if (!lst_hist)	return FALSE;
	if ((lItemCount	= (LONG)lst_hist->GetCount()) < 1)	return FALSE;

	/* SubstrateID 인터페이스 포인터 얻기 */
	if (!GetSubstrateIDPtr(subst_id, &pICxSubstPtr))	return FALSE;

	/* 최상위 부모 List Handle 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetSubstrateHistory::AppendList:Handle");
	hResult	= valHist->AppendList(0, &lHandle[0]);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 등록된 개수만큼 ValueDisp 인터페이스 등록 */
	for (i=1; i<=lItemCount; i++)
	{
		/* 등록될 List의 핸들 얻기 */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetSubstrateHistory::AppendList:Handle[0]=%d", lHandle[0]);
		hResult	= valHist->AppendList(lHandle[0], &lHandle[1]);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		/* 등록 대상의 항목 얻기 */
		stHist	= lst_hist->GetAt(lst_hist->FindIndex(i-1));

		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetSubstrateHistory::AppendValueAscii:Handle[1]=%d", lHandle[1]);

		/* Location ID / Time In / Time Out 값 등록 */
		hResult	= valHist->AppendValueAscii(lHandle[1], CComBSTR(stHist.loc_id));
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
		hResult	= valHist->AppendValueAscii(lHandle[1], CComBSTR(stHist.time_in));
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
		hResult	= valHist->AppendValueAscii(lHandle[1], CComBSTR(stHist.time_out));
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	}

	return TRUE;
}