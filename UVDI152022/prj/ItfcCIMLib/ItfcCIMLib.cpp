
/*
 desc : CIM Library 와 상위 응용 프로그램 간의 GEM300 관련 데이터 및 기타 동작 정보 공유 라이브러리
*/

#include "pch.h"
#include "MainApp.h"
#include <afxmt.h>
#include <time.h>
#if (USE_WORK_THREAD)
/* Uitility Object */
#include "WorkThread.h"
#endif
/* Only CIM300 Object */
#include "E30GEM.h"
#include "E39OBJ.h"
#include "E40PJM.h"
#include "E87CMS.h"
#include "E90STS.h"
#include "E94CJM.h"
#include "E116EPT.h"
#include "E157MPT.h"
#include "ICCAL.h"

/* Data Object */
#include "../../inc/cim300/LogData.h"
#include "../../inc/cim300/SharedData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef	THIS_FILE
static	CHAR THIS_FILE[] = __FILE__;
#endif


/* --------------------------------------------------------------------------------------------- */
/*                                           전역 변수                                           */
/* --------------------------------------------------------------------------------------------- */
#if (USE_WORK_THREAD)
CWorkThread				*g_pWorkThread;
#endif
CLogData				*g_pLogData;
CSharedData				*g_pSharedData;
CE30GEM					*g_pE30GEM;
CE39OBJ					*g_pE39OBJ;
CE40PJM					*g_pE40PJM;
CE87CMS					*g_pE87CMS;
CE90STS					*g_pE90STS;
CE94CJM					*g_pE94CJM;
CE116EPT				*g_pE116EPT;
CE157MPT				*g_pE157MPT;
CICCAL					*g_pICCAL;

CAtlList <STG_TVCI>		g_lstValChangeID;	/* EPJ 프로젝트 파일 내에 값이 변경되면 PCIMLib에게 알리기 위한 이벤트 리스트 */
CAtlList <STG_TSMI>		g_lstSECSMsgID;		/* SECS-II Message ID 이벤트 리스트 */


/* --------------------------------------------------------------------------------------------- */
/*                                           콜백 함수                                           */
/* --------------------------------------------------------------------------------------------- */

#if (USE_SIMULATION_CODE)
/* --------------------------------------------------------------------------------------------- */
/*                             Simulation code area for Cimetrix                                 */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		pj_id	- [in]  Process Job ID
		rp_id	- [in]  
 retn : TRUE or FALSE
 */
BOOL ProcessSubstrate(PTCHAR subst_id, PTCHAR pj_id, PTCHAR rp_id)
{
	HRESULT hResult;
	CComVariant vErrors;

	// EPT is BUSY
	g_pLogData->PushLogs(L"ICxE116Object::Busy ProcessChamber Lithography");
	hResult	= g_pE30GEM->GetE116Ptr()->Busy(CComBSTR("ProcessChamber"), CComBSTR("Lithography"), CxEPTTaskType::eptProcess);
	if (g_pLogData->CheckResult(hResult, L"ICxE116Object::Busy ProcessChamber"))	return FALSE;

	// General Execution Started
	g_pLogData->PushLogs(L"ICxE157Object::MPTExecutionStarted ProcessChamber");
	hResult = g_pE30GEM->GetE157Ptr()->MPTExecutionStarted(CComBSTR("ProcessChamber"),
														   CComBSTR(pj_id),
														   CComBSTR(rp_id),
														   CComBSTR(rp_id),
														   CComBSTR(subst_id), NULL, &vErrors);
	if (g_pLogData->CheckResult(hResult, L"ICxE157Object::MPTExecutionStarted"))	return FALSE;

	// Step #1 in Lithography Started
	g_pLogData->PushLogs(L"ICxE157Object::MPTStepStarted LithographStep1");
	hResult = g_pE30GEM->GetE157Ptr()->MPTStepStarted(CComBSTR("ProcessChamber"),
													  CComBSTR("LithographStep1"),
													  &vErrors);
	if (g_pLogData->CheckResult(hResult, L"ICxE157Object::MPTStepStarted"))	return FALSE;
	Sleep(100);

	// Step #1 in Lithography Completed
	g_pLogData->PushLogs(L"ICxE157Object::MPTStepCompleted");
	hResult = g_pE30GEM->GetE157Ptr()->MPTStepCompleted(CComBSTR("ProcessChamber"),
														VARIANT_TRUE, &vErrors);
	if (g_pLogData->CheckResult(hResult, L"ICxE157Object::MPTStepCompleted"))	return FALSE;

	// Step #2 in Lithography Started
	g_pLogData->PushLogs(L"ICxE157Object::MPTStepStarted LithographStep2");
	hResult = g_pE30GEM->GetE157Ptr()->MPTStepStarted(CComBSTR("ProcessChamber"),
													  CComBSTR("LithographStep2"), &vErrors);
	if (g_pLogData->CheckResult(hResult, L"ICxE157Object::MPTStepStarted"))	return FALSE;
	Sleep(100);

	// Step #2 in Lithography Completed
	g_pLogData->PushLogs(L"ICxE157Object::MPTStepCompleted");
	hResult = g_pE30GEM->GetE157Ptr()->MPTStepCompleted(CComBSTR("ProcessChamber"),
														VARIANT_TRUE, &vErrors);
	if (g_pLogData->CheckResult(hResult, L"ICxE157Object::MPTStepCompleted"))	return FALSE;

	// EPT: change busy task
	g_pLogData->PushLogs(L"ICxE116Object::Busy ProcessChamber Chilling");
	hResult = g_pE30GEM->GetE116Ptr()->Busy(CComBSTR("ProcessChamber"),
											CComBSTR("Chilling"),
											CxEPTTaskType::eptProcess);
	if (g_pLogData->CheckResult(hResult, L"ICxE116Object::Busy ProcessChamber"))	return FALSE;

	// Step #1 in Chilling Started
	g_pLogData->PushLogs(L"ICxE157Object::MPTStepStarted ChillingStep1");
	hResult = g_pE30GEM->GetE157Ptr()->MPTStepStarted(CComBSTR("ProcessChamber"),
													  CComBSTR("ChillingStep1"), &vErrors);
	if (g_pLogData->CheckResult(hResult, L"ICxE157Object::MPTStepStarted"))	return FALSE;
	Sleep(100);

	// Step #1 in Chilling Completed
	g_pLogData->PushLogs(L"ICxE157Object::MPTStepCompleted");
	hResult = g_pE30GEM->GetE157Ptr()->MPTStepCompleted(CComBSTR("ProcessChamber"),
														VARIANT_TRUE, &vErrors);
	if (g_pLogData->CheckResult(hResult, L"ICxE157Object::MPTStepCompleted"))	return FALSE;

	// General Execution Completed
	g_pLogData->PushLogs(L"ICxE157Object::MPTExecutionCompleted");
	hResult = g_pE30GEM->GetE157Ptr()->MPTExecutionCompleted(CComBSTR("ProcessChamber"),
															 VARIANT_TRUE, &vErrors);
	if (g_pLogData->CheckResult(hResult, L"ICxE157Object::MPTExecutionCompleted"))	return FALSE;

	// EPT is IDLE
	g_pLogData->PushLogs(L"ICxE116Object::Idle ProcessChamber");
	hResult = g_pE30GEM->GetE116Ptr()->Idle(CComBSTR("ProcessChamber"));
	if (g_pLogData->CheckResult(hResult, L"ICxE116Object::Idle"))	return FALSE;

	return TRUE;
}

/*
 desc : 
 parm : carr_id		- [in]  
		slot_no		- [in]  
		pj_id		- [in]  Process Job ID
		rp_id		- [in]  
		last_one_pj	- [in]  
		dest_id		- [in]  
 retn : TRUE or FALSE
 */
BOOL ScheduleSubstrate(PTCHAR carr_id, UINT8 slot_no, PTCHAR pj_id, PTCHAR rp_id,
					   BOOL last_one_pj, PTCHAR dest_id)
{
	LONG lStatus;
	HRESULT hResult;
	CComBSTR bstrSubstID;

	/* Remove from carrier */
	g_pLogData->PushLogs(L"ICxE87CMS::SubstrateRemoved");
	hResult = g_pE30GEM->GetE87Ptr()->SubstrateRemoved(CComBSTR(carr_id), slot_no, &bstrSubstID);
	if (g_pLogData->CheckResult(hResult, L"ICxE87CMS::SubstrateRemoved"))	return FALSE;

	// To the robot
	g_pLogData->PushLogs(L"ICxE90ST::ChangeSubstrateState RobotArm", (LPTSTR)_bstr_t(bstrSubstID));
	hResult = g_pE30GEM->GetE90Ptr()->ChangeSubstrateState(bstrSubstID, CComBSTR("RobotArm"),
		SubstrateTransportStateEnum::stsAT_WORK, SubstrateProcessingStateEnum::spsNEEDS_PROCESSING, &lStatus);
	if (g_pLogData->CheckResult(hResult, L"ICxE90ST::ChangeSubstrateState RobotArm"))	return FALSE;

	// To the prealigner
	g_pLogData->PushLogs(L"ICxE90ST::ChangeSubstrateState PreAligner", (LPTSTR)_bstr_t(bstrSubstID));
	hResult = g_pE30GEM->GetE90Ptr()->ChangeSubstrateState(bstrSubstID, CComBSTR("PreAligner"),
		SubstrateTransportStateEnum::stsAT_WORK, SubstrateProcessingStateEnum::spsNEEDS_PROCESSING, &lStatus);
	if (g_pLogData->CheckResult(hResult, L"ICxE90ST::ChangeSubstrateState PreAligner"))	return FALSE;

	// To the robot
	g_pLogData->PushLogs(L"ICxE90ST::ChangeSubstrateState RobotArm", (LPTSTR)_bstr_t(bstrSubstID));
	hResult = g_pE30GEM->GetE90Ptr()->ChangeSubstrateState(bstrSubstID, CComBSTR("RobotArm"),
		SubstrateTransportStateEnum::stsAT_WORK, SubstrateProcessingStateEnum::spsNEEDS_PROCESSING, &lStatus);
	if (g_pLogData->CheckResult(hResult, L"ICxE90ST::ChangeSubstrateState RobotArm"))	return FALSE;

	// To the process chamber
	g_pLogData->PushLogs(L"ICxE90ST::ChangeSubstrateState ProcessChamber", (LPTSTR)_bstr_t(bstrSubstID));
	hResult = g_pE30GEM->GetE90Ptr()->ChangeSubstrateState(bstrSubstID, CComBSTR("ProcessChamber"),
		SubstrateTransportStateEnum::stsAT_WORK, SubstrateProcessingStateEnum::spsIN_PROCESS, &lStatus);
	if (g_pLogData->CheckResult(hResult, L"ICxE90ST::ChangeSubstrateState ProcessChamber"))	return FALSE;

	ProcessSubstrate(bstrSubstID, pj_id, rp_id);

	// Process in the chamber...
	if (last_one_pj)
	{
		ICxValueDispPtr pICxStatus(CLSID_CxValueObject);
		g_pLogData->PushLogs(L"m_ICxE40PJM::PRJobProcessComplete");
		hResult	= g_pE30GEM->GetE40Ptr()->PRJobProcessComplete(CComBSTR(pj_id), pICxStatus);
		if (g_pLogData->CheckResult(hResult, L"m_ICxE40PJM::PRJobProcessComplete"))	return FALSE;
	}

	// To the robot
	g_pLogData->PushLogs(L"ICxE90ST::ChangeSubstrateState RobotArm", (LPTSTR)_bstr_t(bstrSubstID));
	hResult = g_pE30GEM->GetE90Ptr()->ChangeSubstrateState(bstrSubstID, CComBSTR("RobotArm"),
														   SubstrateTransportStateEnum::stsAT_WORK,
														   SubstrateProcessingStateEnum::spsPROCESSED,
														   &lStatus);
	if (g_pLogData->CheckResult(hResult, L"ICxE90ST::ChangeSubstrateState RobotArm"))	return FALSE;


	// To the carrier

	g_pLogData->PushLogs(L"ICxE90ST::ChangeSubstrateState Destination");
	hResult = g_pE30GEM->GetE90Ptr()->ChangeSubstrateState(bstrSubstID, CComBSTR(dest_id),
		SubstrateTransportStateEnum::stsAT_DESTINATION, SubstrateProcessingStateEnum::spsPROCESSED, &lStatus);
	if (g_pLogData->CheckResult(hResult, L"ICxE90ST::ChangeSubstrateState Destination"))	return FALSE;

	g_pLogData->PushLogs(L"ICxE87CMS::SubstratePlaced", (LPTSTR)_bstr_t(bstrSubstID));
	hResult	= g_pE30GEM->GetE87Ptr()->SubstratePlaced(CComBSTR(carr_id), slot_no, bstrSubstID);
	if (g_pLogData->CheckResult(hResult, L"ICxE87CMS::SubstratePlaced"))	return FALSE;

	return true;
}
#endif	/* USE_SIMULATION_CODE */

/* --------------------------------------------------------------------------------------------- */
/*                                           로컬 함수                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ICxSubstrate 리스트 얻기
 parm : pj_id		- [in]  Process Job ID
		subst_ptr	- [out] ICxSubstratePtr 객체 리스트가 저장될 버퍼
 retn : TRUE or FALSE
*/
BOOL GetPRJobToListSubstratePtr(PTCHAR pj_id, CPtrArray &subst_ptr)
{
	INT32 i	= 0;
	CByteArray arrSlotNo;
	CStringArray arrCarrID;
	ICxSubstratePtr pICxSubstPtr;

	if (!g_pE40PJM || !g_pE87CMS)	return FALSE;
	if (!g_pE30GEM->IsInitedAll(TRUE))	return FALSE;
	/* PRJob ID에 해당되는 Slot Number와 Carrier ID 리스트 얻기 */
	if (!g_pE40PJM->GetProcessJobAllSlotNo(pj_id, arrSlotNo, arrCarrID))	return FALSE;
	/* Carrier ID와 Slot No 가지고 Substate 얻기 */
	for (; i<arrSlotNo.GetCount(); i++)
	{
		if (!g_pE87CMS->GetSubstratePtrFromSource(arrCarrID[i].GetBuffer(),
												  arrSlotNo[i], pICxSubstPtr))	return FALSE;
		/* 리스트에 추가 */
		subst_ptr.Add(pICxSubstPtr);
	}

	return TRUE;
}

/*
 desc : ICxSubstrate for E90STS 객체 포인터 얻기
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		st_ptr	- [out] ICxSubstratePtr 객체가 반환될 버퍼
 retn : TRUE or FALSE
*/
BOOL GetSubstToE90SubstratePtr(PTCHAR subst_id, ICxSubstratePtr &st_ptr)
{
	if (!!g_pE90STS)	return FALSE;
	if (!g_pE30GEM->IsInitedAll(TRUE))	return FALSE;
#if 1
	/* PRJob ID에 해당되는 Slot Number와 Carrier ID 리스트 얻기 */
	if (!g_pE90STS->GetSubstrateIDPtr(subst_id, &st_ptr))	return FALSE;
#endif
	return TRUE;
}

/*
 desc : ICxSubstrate for E90STS 객체 포인터 얻기
 parm : carr_id	- [in]  Carrier ID
		slot_no	- [in]  Carrier 내에 위치한 Slot Number
		st_ptr	- [out] ICxSubstratePtr 객체가 반환될 버퍼
 retn : TRUE or FALSE
*/
BOOL GetSubstToE87SubstratePtr(PTCHAR carr_id, UINT8 slot_no, ICxSubstratePtr &st_ptr)
{
	if (!!g_pE87CMS)	return FALSE;
	if (!g_pE30GEM->IsInitedAll(TRUE))	return FALSE;
	/* PRJob ID에 해당되는 Slot Number와 Carrier ID 리스트 얻기 */
	if (!g_pE87CMS->GetSubstratePtrFromSource(carr_id, slot_no, st_ptr))	return FALSE;

	return TRUE;
}

/*
 desc : Substrate ID 반환
 parm : carr_id	- [in]  Carrier ID
		slot_no	- [in]  Carrier 내에 위치한 Slot Number
		subst_id- [out] Substrate ID가 반환될 버퍼
		size	- [in]  'subst_id'의 버퍼 크기
 retn : TRUE or FALSE
*/
BOOL GetSubstToE87SubstrateID(PTCHAR carr_id, UINT8 slot_no,
							  PTCHAR subst_id, UINT32 size)
{
	CComBSTR bstrSubstID;
	ICxSubstratePtr pICxSubstPtr;

	if (!!g_pE87CMS)	return FALSE;
	if (!g_pE30GEM->IsInitedAll(TRUE))	return FALSE;
	/* PRJob ID에 해당되는 Slot Number와 Carrier ID 리스트 얻기 */
	if (!g_pE87CMS->GetSubstratePtrFromSource(carr_id, slot_no, pICxSubstPtr))	return FALSE;
	if (!pICxSubstPtr->get_objectID(&bstrSubstID))	return FALSE;

	/* Substrate ID 반환 */
	swprintf_s(subst_id, size, L"%s", (LPTSTR)_bstr_t(bstrSubstID));

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                          TELCIM.INF                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : TELCIM.INF 파일에서 [ValueChangedID] 목록을 얻어와 임시 리스트 버퍼 메모리에 저장
 parm : work_dir	- [in]  실행 파일이 존재하는 경로 (프로세스 작업 경로)
 retn : TRUE or FALSE
*/
BOOL LoadValueChangedID(PTCHAR work_dir)
{
	UINT16 i = 0, u16Count;
	INT32 i32Find	= 0, i32Len;
	TCHAR tzVID[8]	= {NULL}, tzConn[8]	= {NULL}, tzFile[MAX_PATH_LEN] = {NULL};
	TCHAR tzItem[8]	= {NULL}, tzSubj[]	= {L"Value.Changed.ID"};
	TCHAR tzValue[CIM_CMD_NAME_SIZE+8]	= {NULL};	/* ID + Name */
	PTCHAR ptzValue;
	STG_TVCI stData	= {NULL};

	/* telcim.inf 파일 전체 경로 포함 얻기 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\info\\%s", work_dir, TEL_CIM_CONFIG);

	/* ValueChangedID 개수 얻기 */
	u16Count	= GetPrivateProfileInt(tzSubj, L"COUNT", 0, tzFile);
	if (u16Count < 1)	return FALSE;

	/* SECS-II.Message.ID 정보 얻기 */
	for (; i<u16Count; i++)
	{
		/* Number */
		wmemset(tzValue, 0x00, CIM_CMD_NAME_SIZE+8);
		swprintf_s(tzItem, 8, L"%05d", i+1);
		GetPrivateProfileString(tzSubj, tzItem, L"", tzValue, CIM_CMD_NAME_SIZE, tzFile);
		if (wcslen(tzValue) < 1)
		{
			g_lstValChangeID.RemoveAll();
			return FALSE;
		}

		/* 기존 값 초기화 */
		memset(&stData, 0x00, sizeof(STG_TVCI));
		wmemset(tzConn, 0x00, 8);
		wmemset(tzVID, 0x00, 8);
		ptzValue	= tzValue;

		/* ------------------------------------------------------- */
		/* Connection ID, Variable ID, Variable Name 분리하여 저장 */
		/* ------------------------------------------------------- */

		/* Connection ID 얻기 */
		i32Len		= (INT32)wcslen(tzValue);
		i32Find		= uvCmn_GetFindCh(ptzValue, i32Len, L',');
		if (i32Find < 0 && i32Find > 8)
		{
			g_lstValChangeID.RemoveAll();
			return FALSE;
		}
		wmemcpy(tzConn, ptzValue, i32Find);	ptzValue += i32Find + 1;

		/* Value ID 얻기 */
		i32Len		= (INT32)wcslen(tzValue);
		i32Find		= uvCmn_GetFindCh(ptzValue, i32Len, L',');
		if (i32Find < 0 && i32Find > 8)
		{
			g_lstValChangeID.RemoveAll();
			return FALSE;
		}
		wmemcpy(tzVID, ptzValue, i32Find);	ptzValue += i32Find + 1;

		/* Variable Name 얻기 */
		wmemcpy(stData.var_name, ptzValue, i32Len - (i32Find + 1));

		/* 리스트 메모리 버퍼에 등록 */
		stData.conn_id	= (UINT32)_wtoi(tzConn);
		stData.var_id	= (UINT32)_wtoi(tzVID);
		g_lstValChangeID.AddTail(stData);
	}

	return TRUE;
}

/*
 desc : TELCIM.INF 파일에서 [ValueChangedID] 목록을 얻어와 임시 리스트 버퍼 메모리에 저장
 parm : work_dir	- [in]  실행 파일이 존재하는 경로 (프로세스 작업 경로)
 retn : TRUE or FALSE
*/
BOOL LoadSECSMessageID(PTCHAR work_dir)
{
	UINT16 i = 0, u16Count;
	INT32 i32Find	= 0, i32Len;
	TCHAR tzSID[8]	= {NULL}, tzFID[8] = {NULL}, tzConn[8]	= {NULL}, tzFile[MAX_PATH_LEN] = {NULL};
	TCHAR tzItem[8]	= {NULL}, tzSubj[]	= {L"SECS-II.Message.ID"};
	TCHAR tzMesg[CIM_CMD_NAME_SIZE+8]	= {NULL};	/* ID + Name */
	PTCHAR ptzMesg;
	STG_TSMI stData	= {NULL};

	/* telcim.inf 파일 전체 경로 포함 얻기 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\info\\%s", work_dir, TEL_CIM_CONFIG);

	/* ValueChangedID 개수 얻기 */
	u16Count	= GetPrivateProfileInt(tzSubj, L"COUNT", 0, tzFile);
	if (u16Count < 1)	return FALSE;

	/* ValueChangedID 정보 얻기 */
	for (; i<u16Count; i++)
	{
		/* Number */
		wmemset(tzMesg, 0x00, CIM_CMD_NAME_SIZE+8);
		swprintf_s(tzItem, 8, L"%05d", i+1);
		GetPrivateProfileString(tzSubj, tzItem, L"", tzMesg, CIM_CMD_NAME_SIZE, tzFile);
		if (wcslen(tzMesg) < 1)
		{
			g_lstSECSMsgID.RemoveAll();
			return FALSE;
		}

		/* 기존 값 초기화 */
		memset(&stData, 0x00, sizeof(STG_TSMI));
		wmemset(tzConn, 0x00, 8);
		wmemset(tzSID, 0x00, 8);
		wmemset(tzFID, 0x00, 8);
		ptzMesg	= tzMesg;

		/* ------------------------------------------------------- */
		/* Connection ID, Variable ID, Variable Name 분리하여 저장 */
		/* ------------------------------------------------------- */

		/* Connection ID 얻기 */
		i32Len		= (INT32)wcslen(tzMesg);
		i32Find		= uvCmn_GetFindCh(ptzMesg, i32Len, L',');
		if (i32Find < 0 && i32Find > 8)
		{
			g_lstSECSMsgID.RemoveAll();
			return FALSE;
		}
		wmemcpy(tzConn, ptzMesg, i32Find);	ptzMesg += i32Find + 1;

		/* Stream ID 얻기 */
		i32Len		= (INT32)wcslen(tzMesg);
		i32Find		= uvCmn_GetFindCh(ptzMesg, i32Len, L',');
		if (i32Find < 0 && i32Find > 8)
		{
			g_lstSECSMsgID.RemoveAll();
			return FALSE;
		}
		wmemcpy(tzSID, ptzMesg, i32Find);	ptzMesg += i32Find + 1;

		/* Function ID 얻기 */
		i32Len		= (INT32)wcslen(tzMesg);
		i32Find		= uvCmn_GetFindCh(ptzMesg, i32Len, L',');
		if (i32Find < 0 && i32Find > 8)
		{
			g_lstSECSMsgID.RemoveAll();
			return FALSE;
		}
		wmemcpy(tzFID, ptzMesg, i32Find);	ptzMesg += i32Find + 1;

		/* Message Description 얻기 */
		if ((i32Len - (i32Find + 1)) > CIM_CMD_NAME_SIZE + 8)
		{
			g_lstSECSMsgID.RemoveAll();
			return FALSE;
		}
		wmemcpy(stData.mesg_desc, ptzMesg, i32Len - (i32Find + 1));
		/* 리스트 메모리 버퍼에 등록 */
		stData.conn_id	= (UINT8)_wtoi(tzConn);
		stData.s_id		= (UINT8)_wtoi(tzSID);
		stData.f_id		= (UINT8)_wtoi(tzFID);
		g_lstSECSMsgID.AddTail(stData);
	}

	return TRUE;
}


/* --------------------------------------------------------------------------------------------- */
/*                                           외부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : 라이브러리 초기화
 parm : work_dir	- [in]  실행파일이 존재하는 경로 (프로세스가 동작되는 메인 경로)
		conn_cnt	- [in]  EPJ 프로젝트 파일에 등록된 CONNECTIONx 개수
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_Init(PTCHAR work_dir, UINT8 conn_cnt)
{
	/* 로그 데이터 관리 객체 생성 */
	g_pLogData		= new CLogData;
	/* CIM300 Data 관리 객체 생성 */
	g_pSharedData	= new CSharedData (conn_cnt);
	ASSERT(g_pLogData && g_pSharedData);
	/* CIM300 Class Object 생성 (!!! 반드시 순서 지킬 것 !!!) */
	g_pICCAL	= new CICCAL(g_pLogData,	g_pSharedData);
	g_pE30GEM	= new CE30GEM(g_pLogData,	g_pSharedData, conn_cnt);
	g_pE39OBJ	= new CE39OBJ(g_pLogData,	g_pSharedData);
	g_pE40PJM	= new CE40PJM(g_pLogData,	g_pSharedData);
	g_pE87CMS	= new CE87CMS(g_pLogData,	g_pSharedData);
	g_pE90STS	= new CE90STS(g_pLogData,	g_pSharedData);
	g_pE94CJM	= new CE94CJM(g_pLogData,	g_pSharedData);
	g_pE116EPT	= new CE116EPT(g_pLogData,	g_pSharedData);
	g_pE157MPT	= new CE157MPT(g_pLogData,	g_pSharedData);
	ASSERT(g_pICCAL && g_pE30GEM && g_pE39OBJ && g_pE40PJM && g_pE87CMS &&
		   g_pE90STS && g_pE94CJM && g_pE116EPT && g_pE157MPT);
#if (USE_WORK_THREAD)
	/* CIM300 Initialization Thread 생성  */
	g_pWorkThread	= new CWorkThread(g_pSharedData, g_pLogData,
									  g_pE30GEM, g_pE40PJM, g_pE87CMS, g_pE90STS, g_pE94CJM);
	ASSERT(g_pWorkThread);
	if (!g_pWorkThread->CreateThread(0, 0, SLOW_THREAD_SPEED))
	{
		AfxMessageBox(L"Can't run the cim300 work_thread for CIM300", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
#endif

	/* ValueChangedID 값 적재 */
	if (!LoadValueChangedID(work_dir) || !LoadSECSMessageID(work_dir))
	{
		AfxMessageBox(L"Failed to load LoadValueChangedID or LoadSECSMessageID in the telcim.inf",
					  MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : 라이브러리 해제
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT VOID uvCIMLib_Close()
{
#if (USE_WORK_THREAD)
	/* Initialization Thraed 메모리 해제 */
	if (g_pWorkThread)
	{
		if (g_pWorkThread->IsBusy())	g_pWorkThread->Stop();
		while (g_pWorkThread->IsBusy())	Sleep(100);
		delete g_pWorkThread;
		g_pWorkThread	= NULL;
	}
#endif

	/* 객체 메모리 해제  (!!! 반드시 순서 지킬 것 !!!) */
	if (g_pE157MPT)		delete g_pE157MPT;		g_pE157MPT		= NULL;
	if (g_pE116EPT)		delete g_pE116EPT;		g_pE116EPT		= NULL;
	if (g_pE94CJM)		delete g_pE94CJM;		g_pE94CJM		= NULL;
	if (g_pE40PJM)		delete g_pE40PJM;		g_pE40PJM		= NULL;
	if (g_pE87CMS)		delete g_pE87CMS;		g_pE87CMS		= NULL;
	if (g_pE90STS)		delete g_pE90STS;		g_pE90STS		= NULL;
	if (g_pE39OBJ)		delete g_pE39OBJ;		g_pE39OBJ		= NULL;
	if (g_pICCAL)		delete g_pICCAL;		g_pICCAL		= NULL;
	if (g_pE30GEM)		delete g_pE30GEM;		g_pE30GEM		= NULL;

	if (g_pLogData)		delete g_pLogData;		g_pLogData		= NULL;
	if (g_pSharedData)	delete g_pSharedData;	g_pSharedData	= NULL;

	/* 리스트 메모리 해제 */
	g_lstValChangeID.RemoveAll();
	g_lstSECSMsgID.RemoveAll();
}

/*
 desc : CIM300 Connect 연결 (초기화) 해제 실행
 parm : None
 retn : None
*/
API_EXPORT VOID uvCIMLib_Stop()
{
	/* 반드시 먼저 초기화 호출해 줘야 됨 */
	if (g_pE30GEM)	g_pE30GEM->ResetInitFlag();

	/* CIM300 Class Object 해제 (!!! 반드시 순서 지킬 것 !!!) */
	if (g_pE157MPT)	g_pE157MPT->Close();
	if (g_pE116EPT)	g_pE116EPT->Close();
	if (g_pE94CJM)	g_pE94CJM->Close();
	if (g_pE40PJM)	g_pE40PJM->Close();
	if (g_pE87CMS)	g_pE87CMS->Close();
	if (g_pE90STS)	g_pE90STS->Close();
	if (g_pE39OBJ)	g_pE39OBJ->Close();
	if (g_pICCAL)	g_pICCAL->Close();
	if (g_pE30GEM)	g_pE30GEM->Close();
}

/*
 desc : CIM300 Connect 연결 (초기화) 실행
 parm : equip_id	- [in]  Equipment ID (0 or later. 환경 파일에 설정된 개수-1)
		conn_id		- [in] 1 or Later (공통 적용을 위해서는 0 값 사용)
		epj_file	- [in]  EPJ 파일 (전체 경로)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_Start(LONG equip_id, LONG conn_id, PTCHAR epj_file)
{
	if (!g_pE30GEM)	return FALSE;

	/* 기존에 초기화된 경우라면 해제 진행부터 수행 */
	if (g_pE30GEM->IsInitConnected())
	{
		uvCIMLib_Stop();
#ifndef _DEBUG
		g_pE30GEM->WaitOnEMServiceIdle();
#endif
	}

	/* 초기화 수행 (!!! 반드시 순서 지킬 것 !!!) */
	if (!g_pE30GEM->Init(equip_id, conn_id))	return FALSE;

	g_pICCAL->SetICxClientAppPtr(g_pE30GEM->GetICxClientAppPtr());
	if (!g_pICCAL->Init(equip_id, conn_id))		return FALSE;

	g_pE39OBJ->SetICxAbsLayerPtr(g_pICCAL->GetICxAbsLayerPtr());
	g_pE39OBJ->SetICxEMAppCbkPtr(g_pE30GEM->GetICxEMAppCbkPtr());
	if (!g_pE39OBJ->Init(equip_id, conn_id))	return FALSE;

	g_pE40PJM->SetICxAbsLayerPtr(g_pICCAL->GetICxAbsLayerPtr());
	g_pE40PJM->SetICxEMAppCbkPtr(g_pE30GEM->GetICxEMAppCbkPtr());
	g_pE40PJM->SetICxE39ObjPtr(g_pE39OBJ->GetICxE39ObjPtr());
	if (!g_pE40PJM->Init(equip_id, conn_id))	return FALSE;

	g_pE90STS->SetIEMSvcCbkPtr(g_pE30GEM->GetIEMSvcCbkPtr());
	g_pE90STS->SetICxAbsLayerPtr(g_pICCAL->GetICxAbsLayerPtr());
	g_pE90STS->SetICxE39ObjPtr(g_pE39OBJ->GetICxE39ObjPtr());
	g_pE90STS->SetICxDiagnosticsPtr(g_pE30GEM->GetICxDiagnosticsPtr());
	if (!g_pE90STS->Init(equip_id, conn_id))	return FALSE;

	g_pE87CMS->SetICxClientAppPtr(g_pE30GEM->GetICxClientAppPtr());
	g_pE87CMS->SetICxClientToolPtr(g_pE30GEM->GetICxClientToolPtr());
	g_pE87CMS->SetIEMSvcCbkPtr(g_pE30GEM->GetIEMSvcCbkPtr());
	g_pE87CMS->SetICxAbsLayerPtr(g_pICCAL->GetICxAbsLayerPtr());
	g_pE87CMS->SetICxE39ObjPtr(g_pE39OBJ->GetICxE39ObjPtr());
	g_pE87CMS->SetICxE90STPtr(g_pE90STS->GetICxE90STPtr());
	if (!g_pE87CMS->Init(equip_id, conn_id))	return FALSE;

	g_pE94CJM->SetICxEMAppCbkPtr(g_pE30GEM->GetICxEMAppCbkPtr());
	g_pE94CJM->SetICxAbsLayerPtr(g_pICCAL->GetICxAbsLayerPtr());
	g_pE94CJM->SetICxE39ObjPtr(g_pE39OBJ->GetICxE39ObjPtr());
	g_pE94CJM->SetICxE40PJMPtr(g_pE40PJM->GetICxE40PJMPtr());
	g_pE94CJM->SetICxE87CMSPtr(g_pE87CMS->GetICxE87CMSPtr());
	if (!g_pE94CJM->Init(equip_id, conn_id))	return FALSE;

	g_pE116EPT->SetICxAbsLayerPtr(g_pICCAL->GetICxAbsLayerPtr());
	g_pE116EPT->SetICxE39ObjPtr(g_pE39OBJ->GetICxE39ObjPtr());
	if (!g_pE116EPT->Init(equip_id, conn_id))	return FALSE;

	g_pE157MPT->SetICxAbsLayerPtr(g_pICCAL->GetICxAbsLayerPtr());
	if (!g_pE157MPT->Init(equip_id, conn_id))	return FALSE;

	/* EPJ (프로젝트) 파일 적재 */
	if (!g_pE30GEM->LoadProject(epj_file))		return FALSE;
	/* 사용자 정의 변수 값 등록 (값이 변경될 때, 이벤트 인지) */
	if (!g_pE30GEM->RegisterValueChangeID(&g_lstValChangeID))	return FALSE;
	/* 사용자 정의 SECS-II Message 값 등록*/
	if (!g_pE30GEM->RegisterMessageID(&g_lstSECSMsgID))			return FALSE;

	return TRUE;
}

/*
 desc : CIM300 Connection (초기화 완료) 이후 Finalized 호출
 parm : equip_id	- [in]  Equipment ID (0 or later. 환경 파일에 설정된 개수-1)
		conn_id		- [in] 1 or Later (공통 적용을 위해서는 0 값 사용)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_Finalized(LONG equip_id, LONG conn_id)
{
	UINT16 i	= 0x00;

	if (!g_pE30GEM)									return FALSE;
	if (!g_pE30GEM->IsInitConnected())				return FALSE;

	/* 완료 작업 수행 (!!! 반드시 순서 지킬 것 !!!) */
#if 0
	if (!g_pE157MPT->Finalized(equip_id, conn_id))	return FALSE;
	if (!g_pE116EPT->Finalized(equip_id, conn_id))	return FALSE;
	if (!g_pE94CJM->Finalized(equip_id, conn_id))	return FALSE;
	if (!g_pE40PJM->Finalized(equip_id, conn_id))	return FALSE;
	if (!g_pE90STS->Finalized(equip_id, conn_id))	return FALSE;
	if (!g_pE39OBJ->Finalized(equip_id, conn_id))	return FALSE;
	if (!g_pICCAL->Finalized(equip_id, conn_id))	return FALSE;
#endif
	if (!g_pE87CMS->Finalized(equip_id, conn_id))	return FALSE;
	if (!g_pE30GEM->Finalized(equip_id, conn_id))	return FALSE;

	return TRUE;
}

/*
 desc : TS (Tracking System)에 등록된 Equipment Module (Location) Name 반환
 parm : index	- [in]  장비 명과 매핑된 인덱스 값 (1 based)
 retn : 이름 문자열 (실패의 경우 NULL or Empty String)
*/
API_EXPORT PTCHAR uvCIMLib_GetEquipLocationName(ENG_TSSI index)
{
	if (!g_pE30GEM)	return FALSE;
	return g_pE30GEM->GetEquipLocationName(index);
}

/*
 desc : 현재 적재되어 있는 프로젝트 정보를 파일로 저장
 parm : epj_file	- [in]  저장하고자 하는 파일 이름 (전체 경로 포함)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SaveProjetsToEPJ(PTCHAR epj_file)
{
	if (!g_pE30GEM)	return FALSE;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	if (!g_pE30GEM->IsLoadedProject())	return FALSE;
	return g_pE30GEM->SaveProjetsToEPJ(epj_file);
}

/*
 desc : EMService가 동작 중인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsServiceRun()
{
	if (!g_pE30GEM)	return FALSE;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->IsServiceRun();
}

/*
 desc : CIMConnection에 연결되어 있는지 여부
 parm : logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsCIMConnected(BOOL logs)
{
	if (!g_pE30GEM)	return FALSE;
	return g_pE30GEM->IsInitConnected(logs);
}

/*
 desc : CIM Lib. 초기화 여부 반환
 parm : logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsInitedAll(BOOL logs)
{
	if (!g_pE30GEM)	return FALSE;
	return g_pE30GEM->IsInitedAll();
}

/*
 desc : 시스템에서 발생된 로그 반환
 parm : logs	- [in]  반환된 로그 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_PopLogs(STG_CLLM &logs)
{
	if (!g_pE30GEM)		return FALSE;
	if (!g_pLogData)	return FALSE;
	return g_pLogData->PopLogs(logs);
}

/*
 desc : Recipe Path 설정
 parm : path	- [in]  Recipe Path (전체 경로 포함. 마지막에 '/' 제거)
 		conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		var_id	- [in]  Identification (Variables Name과 연결된 ID)
		var_name- [in]  Identification (Variables ID와 연결된 Name)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetRecipePathID(PTCHAR path, LONG conn_id, LONG var_id)
{
	if (!g_pE30GEM)		return FALSE;
	return g_pE30GEM->SetCacheAscii(conn_id, var_id, path);
}
API_EXPORT BOOL uvCIMLib_SetRecipePathName(PTCHAR path, LONG conn_id, PTCHAR var_name)
{
	LONG lVarID	= 0;
	if (!g_pE30GEM)		return FALSE;
	if (!g_pSharedData)	return FALSE;

	if (!g_pE30GEM->GetVarNameToID(conn_id, var_name, lVarID, TRUE))	return FALSE;
	if (!g_pE30GEM->SetCacheAscii(conn_id, lVarID, path))	return FALSE;
	return g_pSharedData->SetRecipePath(path);
}

/*
 desc : MDLN 값 설정
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		name	- [in]  Equipment model type up to 20 characters
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetEquipModelType(LONG conn_id, PTCHAR name)
{
	LONG lVarID	= 0;
	if (!g_pE30GEM)		return FALSE;
	if (!g_pE30GEM->GetVarNameToID(conn_id, L"MDLN", lVarID, TRUE))	return FALSE;
	return g_pE30GEM->SetCacheAscii(conn_id, lVarID, name);
}

/*
 desc : SOFTREV 값 설정
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		name	- [in]  Equipment software revision ID up to 20 characters
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetEquipSoftRev(LONG conn_id, PTCHAR name)
{
	LONG lVarID	= 0;
	if (!g_pE30GEM)		return FALSE;
	if (!g_pE30GEM->GetVarNameToID(conn_id, L"SOFTREV", lVarID, TRUE))	return FALSE;
	return g_pE30GEM->SetCacheAscii(conn_id, lVarID, name);
}

/*
 desc : Recip Path 반환
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		path	- [out] Recipe Names (Files)이 저장된 경로가 반환될 버퍼
		size	- [in]  'path' 버퍼 크기
		var_id	- [in]  Identification (Variables Name과 연결된 ID)
		var_name- [in]  Identification (Variables ID와 연결된 Name)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetRecipePathID(LONG conn_id, PTCHAR path, UINT32 size, LONG var_id)
{
	return g_pE30GEM->GetVariableAscii(conn_id, var_id, path, size);
}
API_EXPORT BOOL uvCIMLib_GetRecipePathName(LONG conn_id, PTCHAR path, UINT32 size, PTCHAR var_name)
{
	LONG lVarID	= 0;
	if (!g_pE30GEM)		return FALSE;
	if (!g_pE30GEM->GetVarNameToID(conn_id, var_name, lVarID, TRUE))	return FALSE;
	return g_pE30GEM->GetVariableAscii(conn_id, lVarID, path, size);
}

/*
 desc : Recip Name (File)의 확장자 반환
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		ext		- [out] Recipe Name (File)의 확장자가 반환될 버퍼
		size	- [in]  'ext' 버퍼 크기
		var_id	- [in]  Identification (Variables Name과 연결된 ID)
		var_name- [in]  Identification (Variables ID와 연결된 Name)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetRecipeFileExtID(LONG conn_id, PTCHAR ext, UINT32 size, LONG var_id)
{
	return g_pE30GEM->GetVariableAscii(conn_id, var_id, ext, size);
}
API_EXPORT BOOL uvCIMLib_GetRecipFileExtName(LONG conn_id, PTCHAR ext, UINT32 size, PTCHAR var_name)
{
	LONG lVarID	= 0;
	if (!g_pE30GEM)		return FALSE;
	if (!g_pSharedData)	return FALSE;

	if (!g_pE30GEM->GetVarNameToID(conn_id, var_name, lVarID, TRUE))	return FALSE;
	if (!g_pE30GEM->GetVariableAscii(conn_id, lVarID, ext, size))		return FALSE;
	return g_pSharedData->SetRecipeExtension(ext);
}

/*
 desc : EPJ ID에 연결된 Name 반환
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		name	- [in]  검색을 위한 Variable Name
		id		- [out] Vairable ID 반환 버퍼
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetNameToID(LONG conn_id, PTCHAR name, LONG &id)
{
	/* ID에 매핑되는 이름 반환 */
	if (!g_pE30GEM->GetVarNameToID(conn_id, name, id))	return FALSE;

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                         Shared Data                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 현재 CIM300 (From Host)으로부터 발생된 이벤트 모두 해제
 parm : None
 retn : None
*/
API_EXPORT VOID uvCIMLib_RemoveEventAll()
{
	if (!g_pSharedData)	return;
	g_pSharedData->RemoveEventAll();
}

/*
 desc : CIM300Cbk로부터 발생된 이벤트 반환
 parm : data	- [in]  반환된 이벤트 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_PopEvent(STG_CCED &event)
{
	if (!g_pSharedData)	return FALSE;
	return g_pSharedData->PopEvent(event);
}

/*
 desc : Load Port의 Docked or Undocked Location Name 반환
 parm : port_id	- [in]  Load Port ID (1 or 2)
		docked	- [in]  0x00 : Docked, 0x01 : Undocked
 retn : Docked or Undocked Location Name 반환 (실패일 경우, NULL)
*/
API_EXPORT PTCHAR uvCIMLib_GetLoadPortDockedName(UINT16 port_id, UINT8 docked)
{
	if (!g_pSharedData)	return NULL;
	return g_pSharedData->GetLoadPortDockedName(port_id, docked);
}

/*
 desc : CIM300 로그 레벨 설정
 parm : level	- [in]  Level Value 값
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetLogLevelAll(ENG_ILLV level)
{
	if (!g_pSharedData)	return FALSE;
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	if (!g_pE40PJM->SetLogLevel(level))	return FALSE;
	if (!g_pE87CMS->SetLogLevel(level))	return FALSE;
	if (!g_pE90STS->SetLogLevel(level))	return FALSE;
	if (!g_pE94CJM->SetLogLevel(level))	return FALSE;

	return TRUE;
}

/*
 desc : 레시피 적재 요청에 대한 값 초기화 (레시피 적재하지 않음)
 parm : None
 retn : None
*/
API_EXPORT VOID uvCIMLib_ResetPPLoadRecipeAck()
{
	if (!g_pSharedData)	return;
	g_pSharedData->ResetPPLoadRecipeAck();
}

/*
 desc : 레시피 적재 요청에 대한 응답 값 반환
 parm : None
 retn : ANY Type
*/
API_EXPORT E30_HPLA uvCIMLib_GetPPLoadRecipeAck()
{
	if (!g_pSharedData)	return E30_HPLA::en_rg_reset;
	return g_pSharedData->GetPPLoadRecipeAck();
}

/*
 desc : 레시피 적재 요청에 대한 응답 값이 수신되었는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsPPLoadRecipeAckOk()
{
	return !g_pSharedData ? NULL : g_pSharedData->IsPPLoadRecipeAckOk();
}

/*
 desc : 레시피 데이터 송신 (전달) 요청에 대한 응답 값 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvCIMLib_ResetPPSendRecipeAck()
{
	if (!g_pSharedData)	return;
	g_pSharedData->ResetPPSendRecipeAck();
}

/*
 desc : 레시피 데이터 송신 (전달) 요청에 대해 응답 값 반환
 parm : None
 retn : ANY Type
*/
API_EXPORT E30_SRHA uvCIMLib_GetPPSendRecipeAck()
{
	if (!g_pSharedData)	return E30_SRHA::en_ra_reset;
	return g_pSharedData->GetPPSendRecipeAck();
}

/*
 desc : 레시피 데이터 송신 (전달) 요청에 대해 응답 값이 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsPPSendRecipeAckOk()
{
	return !g_pSharedData ? NULL : g_pSharedData->IsPPSendRecipeAckOk();
}

/*
 desc : 터미널 메시지 송신 (전달) 요청에 대한 응답 값 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvCIMLib_ResetTermMsgAck()
{
	if (!g_pSharedData)	return;
	g_pSharedData->ResetTermMsgAck();
}

/*
 desc : 터미널 메시지 송신 (전달) 요청에 대한 응답 값 반환
 parm : None
 retn : ANY Type
*/
API_EXPORT E30_STHA uvCIMLib_GetTermMsgAck()
{
	if (!g_pSharedData)	return E30_STHA::en_t_reset;
	return g_pSharedData->GetTermMsgAck();
}

/*
 desc : 터미널 메시지 송신 (전달) 요청에 대해 응답이 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsTermMsgAckOk()
{
	return !g_pSharedData ? NULL : g_pSharedData->IsTermMsgAckOk();
}

/*
 desc : Host로부터 Recipe File 요청 (관리)에 대한 응답 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvCIMLib_ResetRecipeFileAck()
{
	if (!g_pSharedData)	return;
	g_pSharedData->ResetRecipeFileAck();
}

/*
 desc : Host로부터 Recipe File 요청 (관리)에 대한 응답 값 반환
 parm : None
 retn : ANY Type
*/
API_EXPORT E30_RMJC uvCIMLib_GetRecipeFileAck()
{
	if (!g_pSharedData)	return E30_RMJC::en_reset;
	return g_pSharedData->GetRecipeFileAck();
}

/*
 desc : 레시피 (프로세스 프로그램; Process Program == Recipe) 등록 / 삭제 / 전체 삭제
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		file	- [in]  레시피 이름 (전체 경로가 포함된 파일)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_RecipeAppend(LONG conn_id, PTCHAR file)
{
	if (!g_pSharedData)	return FALSE;
	return g_pSharedData->RecipeAppend(conn_id, file);
}
API_EXPORT BOOL uvCIMLib_RecipeRemove(LONG conn_id, PTCHAR file)
{
	if (!g_pSharedData)	return FALSE;
	return g_pSharedData->RecipeRemove(conn_id, file);
}
API_EXPORT VOID uvCIMLib_RecipeRemoveAll()
{
	if (!g_pSharedData)	return;
	return g_pSharedData->RecipeRemoveAll();
}

/*
 desc : 현재 등록된 Process Job ID 내의 Substrate IDs의 상태가 모두 입력된 상태와 동일한지 여부
 parm : pj_id	- [in]  검색 대상의 Process Job ID
		machine	- [in]  Substrate Machine Type 값
		state	- [in]  비교 대상의 Substrate Processing State 값
 retn : 입력된 state 값과 모두 동일하다면 TRUE, 한 개 이상 다른 값이 존재한다면 FALSE
 */
API_EXPORT BOOL uvCIMLib_IsCheckedAllPRJobSubstState(PTCHAR pj_id, E90_VSMC machine, INT32 state)
{
	if (!g_pSharedData)	return FALSE;
	return g_pSharedData->IsCheckedAllPRJobSubstState(pj_id, machine, state);
}

/*
 desc : 현재 등록된 Process Job ID 내의 Substrate IDs의 상태가 입력된 상태와 동일한 값이 1개라도 있는지 여부
 parm : pj_id	- [in]  검색 대상의 Process Job ID
		machine	- [in]  Substate Machine Type 값
		state	- [in]  비교 대상의 Substrate Processing State 값
 retn : 입력된 state 값과 동일한 상태 값이 한 개라도 존재하면 TRUE
		입력된 state 값과 동일한 상태 값이 한 개라도 없다면 FALSE
 */
API_EXPORT BOOL uvCIMLib_IsCheckedOnePRJobSubstState(PTCHAR pj_id, E90_VSMC machine, INT32 state)
{
	if (!g_pSharedData)	return FALSE;
	return g_pSharedData->IsCheckedOnePRJobSubstState(pj_id, machine, state);
}

/*
 desc : 현재 장치에 등록된 PRJob IDs에 등록된 Substrate IDs의 모두 상태 정보들 반환
 parm : pj_id	- [in]  검색 대상의 Process Job ID
		subst	- [out] Substrate ID의 상태 및 기타 정보들이 저장될 리스트 메모리 (PRCommandCbk 함수에서 주로 사용됨?)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetSubstIDStateAll(PTCHAR pj_id, CAtlList <STG_CSDI> &lst_state)
{
	if (!g_pSharedData)	return FALSE;
	return g_pSharedData->GetSubstIDStateAll(pj_id, lst_state);
}

/*
 desc : Carrier ID 제거
 parm : carr_id	- [in]  제거 대상인 Carrier ID
 retn : TRUE (제거 성공), FALSE (제거 실패)
*/
API_EXPORT BOOL uvCIMLib_RemoveSharedCarrID(PTCHAR carr_id)
{
	if (!g_pSharedData)	return FALSE;
	return g_pSharedData->RemoveSharedCarrID(carr_id);
}

/*
 desc : 기존 등록된 SECS-II Message 반환
 parm : mesg_id	- [in]  검색 대상의 Message ID 값 (Stream/Function으로 구성된 16진수 Hexa-Decimal 값)
		remove	- [in]  검색된 대상을 반환 후 메모리에서 제거할지 여부
		item	- [out] 검색된 데이터가 반환될 리스트 버퍼
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetSecsMessage(LONG mesg_id, BOOL remove, CAtlList <STG_TSMV> &item)
{
	if (!g_pSharedData)	return FALSE;
	return g_pSharedData->GetSecsMessage(mesg_id, remove, item);
}

/* 
 desc : 기존 등록된 모든 SECS-II Message 메모리 해제
 parm : None
 retn : None
*/
API_EXPORT VOID uvCIMLib_RemoveAllSecsMessage()
{
	if (g_pSharedData)	g_pSharedData->RemoveAllSecsMessage();
}


#if 0	/* 현재 할 필요 없음. 나중에 판단함 */
/* --------------------------------------------------------------------------------------------- */
/*                         Operation Job Step [Sequencial Working]                               */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 현재 작업이 완료된 상태인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsWorkCompleted()
{
	return g_pWorkThread->IsWorkCompleted();
}

/*
 desc : 현재 작업이 실패된 상태인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsWorkFailed()
{
	return g_pWorkThread->IsWorkFailed();
}

/*
 desc : 현재 작업이 대기 상태인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsIdledWork()
{
	return g_pWorkThread->IsWorkIdled();
}

/*
 desc : E30GEM 초기화 진행
 parm : None
 retn : TRUE or FALSE
 note : EPJ 로딩 이후, 호출되어야 됨
*/
API_EXPORT BOOL uvCIMLib_StartInitE30GEM()
{
	return g_pWorkThread->StartInitE30GEM();
}
#endif

#if (USE_SIMULATION_CODE)
/* --------------------------------------------------------------------------------------------- */
/*                             Simulation code area for Cimetrix                                 */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 
 parm : pj_id	- [in]  Process Job ID
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SimulateProcessing(PTCHAR pj_id)
{
	UINT8 u8SlotNo;
	BOOL bLastOnePRJob	= FALSE;
	LONG lSlotIndex, lCarrIndex, lSlotCounts, lHandle1, lHandle2;
	HRESULT hResult;
	CComBSTR bstrCarrierID, bstrRecipeID, bstrDestination;
	ICxSubstratePtr pICxSubstrate;
	IUnknownPtr pIUnknown;

	// Get the process job from its ID. 
	ICxProcessJobPtr pICxProcessJob;
	g_pLogData->PushLogs(L"m_ICxE40PJM::PRGetJob");
	hResult	= g_pE40PJM->GetE40PJMPtr()->PRGetJob(CComBSTR(pj_id), &pICxProcessJob);
	if (g_pLogData->CheckResult(hResult, L"m_ICxE40PJM::PRGetJob"))	return FALSE;

	// Get the recipe ID
	g_pLogData->PushLogs(L"ICxProcessJob::get_PRRecipeID");
	hResult	= pICxProcessJob->get_PRRecipeID(&bstrRecipeID);
	if (g_pLogData->CheckResult(hResult, L"ICxProcessJob::get_PRRecipeID"))	return FALSE;

	// Get the material type format (CARRIER or SUBSTRATE)
	MaterialTypeEnum materialType;
	g_pLogData->PushLogs(L"ICxProcessJob::get_PRMtlType");
	hResult	= pICxProcessJob->get_PRMtlType(&materialType);
	if (g_pLogData->CheckResult(hResult, L"ICxProcessJob::get_PRMtlType"))	return FALSE;

	// Get the list of material that should be processed. 
	ICxValueDispPtr materialList(CLSID_CxValueObject);
	g_pLogData->PushLogs(L"ICxProcessJob::get_PRMtlNameList");
	hResult	= pICxProcessJob->get_PRMtlNameList(&materialList);
	if (g_pLogData->CheckResult(hResult, L"ICxProcessJob::get_PRMtlNameList"))	return FALSE;

	// Get the number of material (carriers or substrates, depending on material type format)
	long materialCount;
	g_pLogData->PushLogs(L"ICxValueDisp::ItemCount (materialCount)");
	hResult	= materialList->ItemCount(0, &materialCount);
	if (g_pLogData->CheckResult(hResult, L"ICxValueDisp::ItemCount (materialCount)"))	return FALSE;

	// Get the specific material to be processed. 
	switch (materialType)
	{
	case MaterialTypeEnum::pjCARRIER:
		for (lCarrIndex=1; lCarrIndex<=materialCount; lCarrIndex++)
		{
			// Get a handle to the carrier/slot information
			g_pLogData->PushLogs(L"ICxValueDisp::ListAt (carrier slot list handle)");
			hResult	= materialList->ListAt(0, lCarrIndex, &lHandle1);
			if (g_pLogData->CheckResult(hResult, L"ICxValueDisp::ListAt (carrier slot list handle)"))	return FALSE;

			// Get the Carrier ID
			g_pLogData->PushLogs(L"ICxValueDisp::GetValueAscii (carrier ID)");
			hResult	= materialList->GetValueAscii(lHandle1, 1, &bstrCarrierID);
			if (g_pLogData->CheckResult(hResult, L"ICxValueDisp::ListAt (carrier ID)"))	return FALSE;

			// Get a handle to the list of slot #
			g_pLogData->PushLogs(L"ICxValueDisp::ListAt (slot list handle)");
			hResult	= materialList->ListAt(lHandle1, 2, &lHandle2);
			if (g_pLogData->CheckResult(hResult, L"ICxValueDisp::ListAt (slot list handle)"))	return FALSE;

			// Get the number of slots
			g_pLogData->PushLogs(L"ICxValueDisp::ItemCount (slot count)");
			hResult	= materialList->ItemCount(lHandle2, &lSlotCounts);
			if (g_pLogData->CheckResult(hResult, L"ICxValueDisp::ItemCount (slot count)"))	return FALSE;

			// Get each slot number
			for (lSlotIndex=1; lSlotIndex<=lSlotCounts; lSlotIndex++)
			{
				// Get the carrier's slot number for the substrate to process. 
				g_pLogData->PushLogs(L"ICxValueDisp::GetValueU1 (slot number)");
				hResult	= materialList->GetValueU1(lHandle2, lSlotIndex, &u8SlotNo);
				if (g_pLogData->CheckResult(hResult, L"ICxValueDisp::GetValueU1 (slot number)"))	return FALSE;

				// Get the substrate object
				g_pLogData->PushLogs(L"ICxE87CMS::GetSubstrateFromSource");
				hResult	= g_pE87CMS->GetE87CMSPtr()->GetSubstrateFromSource(bstrCarrierID,
																		 (LONG)u8SlotNo, &pIUnknown);
				if (g_pLogData->CheckResult(hResult, L"ICxE87CMS::GetSubstrateFromSource"))	return FALSE;

				if (pIUnknown == NULL)
				{
					g_pLogData->PushLogs(L"Substrate in the process job not found. Skipping.");
					break;
				}
				pICxSubstrate = pIUnknown;

				// Carrier is IN ACCESS when pulling the first substrate. 
				if (lSlotIndex == 1)
				{
					E87_CCAS nowAccessState;
					PTCHAR carId = PTCHAR(bstrCarrierID);

					if (g_pE87CMS->GetCarrierAccessingStatus(carId, nowAccessState) == FALSE)
					{
						g_pLogData->PushLogs(L"ERROR = ICxE87CMS::GetCarrierAccessingStatus");
						return FALSE;
					}

					if (nowAccessState == E87_CCAS::en_carrier_not_accessed)
					{
						g_pLogData->PushLogs(L"ICxE87CMS::AccessCarrier port #1 IN ACCESS");
						VARIANT_BOOL vbResult;
						hResult = g_pE87CMS->GetE87CMSPtr()->AccessCarrier(bstrCarrierID,
							AccessingStatusEnum::IN_ACCESS, &vbResult);
						if (g_pLogData->CheckResult(hResult, L"ICxE87CMS::AccessCarrier port #1"))
							return FALSE;
					}
				}

				// Get the destination
				g_pLogData->PushLogs(L"ICxSubstrate::get_substrateDestination");
				hResult	= pICxSubstrate->get_substrateDestination(&bstrDestination);
				if (g_pLogData->CheckResult(hResult, L"ICxSubstrate::get_substrateDestination"))	return FALSE;

				// check for last substrate in the process job
				bLastOnePRJob = (lSlotIndex == lSlotCounts) && (lCarrIndex == materialCount);

				// Example processing.
				ScheduleSubstrate(bstrCarrierID, u8SlotNo,
								  pj_id, (LPTSTR)_bstr_t(bstrRecipeID),
								  bLastOnePRJob, (LPTSTR)_bstr_t(bstrDestination));

				/* Carrier 마지막 작업인 경우 (?) */
				if (bLastOnePRJob)
				{
					// PJ: ProcessComplete
					g_pLogData->PushLogs(L"g_pE40PJM->SetPRJobComplete");
					if (g_pE40PJM->SetPRJobComplete(pj_id) == FALSE)
					{
						g_pLogData->PushLogs(L"ERROR: g_pE40PJM->SetPRJobComplete");
						return FALSE;
					}

					// Anderson 0427
					TCHAR cj_id[CIM_CONTROL_JOB_ID_SIZE] = { NULL };
					wmemset(cj_id, 0x00, CIM_CONTROL_JOB_ID_SIZE);
					g_pLogData->PushLogs(L"g_pE40PJM->GetControlJobID");
					if (g_pE40PJM->GetControlJobID(pj_id, cj_id, CIM_CONTROL_JOB_ID_SIZE, TRUE) == FALSE)
					{
						g_pLogData->PushLogs(L"ERROR: g_pE40PJM->GetControlJobID");
						return FALSE;
					}

					/* Process Job Complete 처리 (E94CJM) */
					g_pLogData->PushLogs(L"g_pE94CJM->SetProcessJobComplete");
					if (g_pE94CJM->SetProcessJobComplete(cj_id, pj_id) == FALSE)
					{
						g_pLogData->PushLogs(L"ERROR: g_pE94CJM->SetProcessJobComplete");
						return FALSE;
					}
				}
			}
		}
		break;
	case MaterialTypeEnum::pjSUBSTRATE:
	default:
		g_pLogData->PushLogs(L"Unexpected PRMtlType");
		return FALSE;
	}

	return TRUE;
}
#endif	/* USE_SIMULATION_CODE */
/* --------------------------------------------------------------------------------------------- */
/*                                Client Application Control                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 현재 Variable의 Value 값 얻기
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		var_id	- [in]  Variable ID
		var_name- [in]  Variable Name
		value	- [out] Value 값이 저장되는 버퍼 포인터
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID; 1 or 2)
		index	- [in]	일반적으로 0 값이지만, 리스트의 경우 Slot ID (Wafer ID; 1 ~ Max)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetNameValue(LONG conn_id, PTCHAR var_name, UNG_CVVT &value, LONG handle, LONG index)
{
	LONG lID = -1;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	if (!g_pE30GEM->GetVarNameToID(conn_id, var_name, lID))	return FALSE;
	return g_pE30GEM->GetValue(conn_id, lID, value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetIDValue(LONG conn_id, LONG var_id, UNG_CVVT &value, LONG handle, LONG index)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValue(conn_id, var_id, value, handle, index);
}

/*
 desc : 현재 Variable의 Value 값을 문자열로 반환
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		var_id	- [in]  Variable ID
		var_name- [in]  Variable Name (NULL이 아니면, 무조건 name 기준으로 검색)
		value	- [out] Value 값이 저장되는 버퍼 포인터
		size	- [in]  'value' 버퍼 크기
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID; 1 or 2)
		index	- [in]	일반적으로 0 값이지만, 리스트의 경우 Slot ID (Wafer ID; 1 ~ Max)
 retn : None
*/
API_EXPORT VOID uvCIMLib_GetValueToStr(LONG conn_id, LONG var_id, PTCHAR var_name, PTCHAR value,
									   UINT32 size, LONG handle, LONG index)
{
	E30_GPVT enType;
	UNG_CVVT unValue	= {NULL};

	if (!g_pE30GEM->IsInitConnected())										return;
	if (var_name && !g_pE30GEM->GetVarNameToID(conn_id, var_name, var_id))	return;
	if (!g_pE30GEM->GetValueType(conn_id, var_id, enType))					return;
	if (!g_pE30GEM->GetValue(conn_id, var_id, unValue, handle, index))		return;

	switch (enType)
	{
	case E30_GPVT::en_i1	:	swprintf_s(value, size, L"%d",		unValue.i8_value);	break;
	case E30_GPVT::en_i2	:	swprintf_s(value, size, L"%d",		unValue.i16_value);	break;
	case E30_GPVT::en_i4	:	swprintf_s(value, size, L"%d",		unValue.i32_value);	break;
	case E30_GPVT::en_i8	:	swprintf_s(value, size, L"%I64d",	unValue.i64_value);	break;
	case E30_GPVT::en_b		:	swprintf_s(value, size, L"%d",		unValue.b_value);	break;
	case E30_GPVT::en_bi	:	swprintf_s(value, size, L"%d",		unValue.u8_value);	break;
	case E30_GPVT::en_u1	:	swprintf_s(value, size, L"%u",		unValue.u8_value);	break;
	case E30_GPVT::en_u2	:	swprintf_s(value, size, L"%u",		unValue.u16_value);	break;
	case E30_GPVT::en_u4	:	swprintf_s(value, size, L"%u",		unValue.u32_value);	break;
	case E30_GPVT::en_u8	:	swprintf_s(value, size, L"%I64u",	unValue.u64_value);	break;
	case E30_GPVT::en_f4	:	swprintf_s(value, size, L"%f",		unValue.f_value);	break;
	case E30_GPVT::en_f8	:	swprintf_s(value, size, L"%f",		unValue.d_value);	break;
	case E30_GPVT::en_w		:
	case E30_GPVT::en_a		:	swprintf_s(value, size, L"%s",		unValue.s_value);	break;
	}
}

/*
 desc : 현재 CIMConnect에 저장 및 관리되는 Variable의 Cache 값 반환
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		var_id	- [in]  Variable ID
		var_name- [in]  Variable name
		value	- [out] 반환되어 저장될 버퍼
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID; 1 or 2)
		index	- [in]	일반적으로 0 값이지만, 리스트의 경우 Slot ID (Wafer ID; 1 ~ Max)
 retn : Name 문자열 포인터 반환 (검색 실패의 경우, -1L)
*/
API_EXPORT BOOL uvCIMLib_GetNameValueI1(LONG conn_id, PTCHAR var_name, INT8 &value, LONG handle, LONG index)
{
	LONG lID = -1;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	if (!g_pE30GEM->GetVarNameToID(conn_id, var_name, lID))	return FALSE;
	return g_pE30GEM->GetValueI1(conn_id, lID, &value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetNameValueI2(LONG conn_id, PTCHAR var_name, INT16 &value, LONG handle, LONG index)
{
	LONG lID = -1;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueI2(conn_id, lID, &value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetNameValueI4(LONG conn_id, PTCHAR var_name, INT32 &value, LONG handle, LONG index)
{
	LONG lID = -1;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueI4(conn_id, lID, &value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetNameValueI8(LONG conn_id, PTCHAR var_name, INT64 &value, LONG handle, LONG index)
{
	LONG lID = -1;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueI8(conn_id, lID, &value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetNameValueU1(LONG conn_id, PTCHAR var_name, UINT8 &value, LONG handle, LONG index)
{
	LONG lID = -1;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueU1(conn_id, lID, &value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetNameValueU2(LONG conn_id, PTCHAR var_name, UINT16 &value, LONG handle, LONG index)
{
	LONG lID = -1;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueU2(conn_id, lID, &value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetNameValueU4(LONG conn_id, PTCHAR var_name, UINT32 &value, LONG handle, LONG index)
{
	LONG lID = -1;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueU4(conn_id, lID, &value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetNameValueU8(LONG conn_id, PTCHAR var_name, UINT64 &value, LONG handle, LONG index)
{
	LONG lID = -1;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueU8(conn_id, lID, &value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetNameValueF4(LONG conn_id, PTCHAR var_name, FLOAT &value, LONG handle, LONG index)
{
	LONG lID = -1;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueF4(conn_id, lID, &value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetNameValueF8(LONG conn_id, PTCHAR var_name, DOUBLE &value, LONG handle, LONG index)
{
	LONG lID = -1;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueF8(conn_id, lID, &value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetNameValueBo(LONG conn_id, PTCHAR var_name, BOOL &value, LONG handle, LONG index)
{
	LONG lID = -1;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueBo(conn_id, lID, &value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetIDValueI1(LONG conn_id, LONG var_id, INT8 &value, LONG handle, LONG index)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueI1(conn_id, var_id, &value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetIDValueI2(LONG conn_id, LONG var_id, INT16 &value, LONG handle, LONG index)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueI2(conn_id, var_id, &value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetIDValueI4(LONG conn_id, LONG var_id, INT32 &value, LONG handle, LONG index)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueI4(conn_id, var_id, &value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetIDValueI8(LONG conn_id, LONG var_id, INT64 &value, LONG handle, LONG index)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueI8(conn_id, var_id, &value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetIDValueU1(LONG conn_id, LONG var_id, UINT8 &value, LONG handle, LONG index)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueU1(conn_id, var_id, &value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetIDValueU2(LONG conn_id, LONG var_id, UINT16 &value, LONG handle, LONG index)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueU2(conn_id, var_id, &value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetIDValueU4(LONG conn_id, LONG var_id, UINT32 &value, LONG handle, LONG index)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueU4(conn_id, var_id, &value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetIDValueU8(LONG conn_id, LONG var_id, UINT64 &value, LONG handle, LONG index)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueU8(conn_id, var_id, &value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetIDValueF4(LONG conn_id, LONG var_id, FLOAT &value, LONG handle, LONG index)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueF4(conn_id, var_id, &value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetIDValueF8(LONG conn_id, LONG var_id, DOUBLE &value, LONG handle, LONG index)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueF8(conn_id, var_id, &value, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetIDValueBo(LONG conn_id, LONG var_id, BOOL &value, LONG handle, LONG index)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueBo(conn_id, var_id, &value, handle, index);
}

/*
 desc : 현재 CIMConnect에 저장 및 관리되는 Variable의 Cache 값을 문자열 반환
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		var_id	- [in]  Identification (Variables Name과 연결된 ID)
		value	- [out] 임시 데이터가 저장된 버퍼 포인터 (할당된 메모리가 아님)
		size	- [in]  'value' 버퍼의 크기
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID; 1 or 2)
		index	- [in]	일반적으로 0 값이지만, 리스트의 경우 Slot ID (Wafer ID; 1 ~ Max)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetAsciiVariable(LONG conn_id, LONG var_id, PTCHAR value,
										  UINT32 size, LONG handle, LONG index)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetVariableAscii(conn_id, var_id, value, size, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetAsciiEvent(LONG conn_id, LONG var_id, PTCHAR value,
									   UINT32 size, LONG handle, LONG index)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetVariableAscii(conn_id, var_id, value, size, handle, index);
}
API_EXPORT BOOL uvCIMLib_GetAsciiAlarm(LONG conn_id, LONG var_id, PTCHAR value,
									   UINT32 size, LONG handle, LONG index)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetVariableAscii(conn_id, var_id, value, size, handle, index);
}

/*
 desc : CIMConnect 내부의 캐쉬 버퍼에 값 저장
 parm : conn_id	- [in] 1 or Later (공통 적용을 위해서는 0 값 사용)
		type	- [in]  입력되는 값 (Value)의 Type
		var_id	- [in]  Variable ID
		var_name- [in]  Variable Name
		value	- [in]  값 (type에 따라, 여러가지 포맷의 값)
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID; 1 or 2)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetCacheValueID(LONG conn_id, E30_GPVT type, LONG var_id,
										 UNG_CVVT value, LONG handle)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->SetCacheValue(conn_id, type, var_id, value, handle);
}
API_EXPORT BOOL uvCIMLib_SetCacheValueName(LONG conn_id, E30_GPVT type, PTCHAR var_name,
										   UNG_CVVT value, LONG handle)
{
	LONG lID = -1;
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	if (!g_pE30GEM->GetVarNameToID(conn_id, var_name, lID))	return FALSE;
	return g_pE30GEM->SetCacheValue(conn_id, type, lID, value, handle);
}
API_EXPORT BOOL uvCIMLib_SetCacheStrValueID(LONG conn_id, E30_GPVT type, LONG var_id,
											PTCHAR value, LONG handle)
{
	UNG_CVVT unValue	= {NULL};

	if (!value)	return FALSE;
	if (!g_pE30GEM->IsInitedAll())	return FALSE;

	switch (type)
	{
	case E30_GPVT::en_i1		:	unValue.i8_value	= (INT8)_wtoi(value);				break;
	case E30_GPVT::en_i2		:	unValue.i16_value	= (INT16)_wtoi(value);				break;
	case E30_GPVT::en_i4		:	unValue.i32_value	= (INT32)_wtoi(value);				break;
	case E30_GPVT::en_i8		:	unValue.i64_value	= (INT64)_wtoi64(value);			break;
	case E30_GPVT::en_b		:	unValue.b_value		= (BOOL)_wtoi(value);				break;
	case E30_GPVT::en_bi		:	unValue.u8_value	= (UINT8)_wtoi(value);				break;
	case E30_GPVT::en_u1		:	unValue.u8_value	= (UINT8)_wtoi(value);				break;
	case E30_GPVT::en_u2		:	unValue.u16_value	= (UINT16)_wtoi(value);				break;
	case E30_GPVT::en_u4		:	unValue.u32_value	= (UINT32)_wtoi(value);				break;
	case E30_GPVT::en_u8		:	unValue.u64_value	= (UINT64)_wtoi(value);				break;
	case E30_GPVT::en_f4		:	unValue.f_value		= (FLOAT)_wtof(value);				break;
	case E30_GPVT::en_f8		:	unValue.d_value		= (DOUBLE)_wtof(value);				break;
	case E30_GPVT::en_w		:
	case E30_GPVT::en_a		:	wcscpy_s(unValue.s_value, CIM_VAL_ASCII_SIZE, value);	break;
	default				:	return FALSE;
	}

	return g_pE30GEM->SetCacheValue(conn_id, type, var_id, unValue, handle);
}

/*
 desc : Cache the value of one variable of type ASCII in CIMConnect.
		The variable can be type Status Variable, Data Variable or Equipment Constant.
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		var_id	- [in]  Variable ID
		var_name- [in]  Variable Name
		value	- [in]  variable value
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID;1 or 2)
		index	- [in]	일반적으로 0 값이지만, 리스트의 경우 Slot ID (Wafer ID;1 ~ Max)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetCacheAsciiID(LONG conn_id, LONG var_id, PTCHAR value,
										 LONG handle, LONG index)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->SetCacheAscii(conn_id, var_id, value, handle, index);
}
API_EXPORT BOOL uvCIMLib_SetCacheAsciiName(LONG conn_id, PTCHAR var_name, PTCHAR value,
										   LONG handle, LONG index)
{
	LONG lID = -1;
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	if (!g_pE30GEM->GetVarNameToID(conn_id, var_name, lID))	return FALSE;
	return g_pE30GEM->SetCacheAscii(conn_id, lID, value, handle, index);
}

/*
 desc : TS (Tracking System)에 등록된 Processing Name 반환
 parm : index	- [in]  장비 명과 매핑된 인덱스 값 (!!! -1 based !!!)
 retn : 이름 문자열 (실패의 경우 NULL or Empty String)
*/
API_EXPORT PTCHAR uvCIMLib_GetEquipProcessingName(E90_SSPS index)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->GetEquipProcessingName(index);
}

/*
 desc : CIMConnect 내부에 로그 저장 여부
 parm : value	- [out] TRUE : Saved, FALSE : No save
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetLogToCIMConnect(BOOL &value)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pICCAL->GetLogToCIMConnect(value);
}

/*
 desc : CIMConnect 내부에 로그 저장 여부 설정
 parm : value	- [in]  TRUE : Saved, FALSE : No save
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetLogToCIMConnect(BOOL value)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pICCAL->SetLogToCIMConnect(value);
}

/*
 desc : SECS-II Message 등록 헤재
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_ResetSecsMesgList()
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->ResetSecsMesgList();
}

/*
 desc : SECS-II Message 등록 - Root Message
 parm : type	- [in]  Message Type
		value	- [in]  Message Type에 따른 값
		size	- [in]  'value' 버퍼 크기
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_AddSecsMesgRoot(E30_GPVT type, PVOID value, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->AddSecsMesgRoot(type, value, size);
}

/*
 desc : SECS-II Message 등록 - Root Message
 parm : handle	- [in]  List가 추가될 상위 Parent ID
						맨 처음 추가될 List의 경우, 0 값
		clear	- [in]  Message 버퍼 모두 비우기 여부 (최초이면 TRUE, 중간이면 FALSE)
 retn : 새로 추가된 Handle 값 반환 (음수 값이면, 에러 처리)
*/
API_EXPORT LONG uvCIMLib_AddSecsMesgList(LONG handle, BOOL clear)
{
	if (!g_pE30GEM->IsInitedAll())	return -1;
	return g_pE30GEM->AddSecsMesgList(handle);
}

/*
 desc : 원격 장치 (Host?)에게 송신될 SECS-II Message 등록
 parm : handle	- [in]  Handle to a list within the object, or 0
		type	- [in]  'value' 값이 저장된 변수 형식
		value	- [in]  The value to append
		size	- [in]  'value' 버퍼에 저장된 데이터의 크기
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_AddSecsMesgValue(LONG handle, E30_GPVT type, PVOID value, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->AddSecsMesgValue(handle, type, value, size);
}

/*
 desc : 원격 장치 (Host?)에게 SECS-II Message 송신
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		mesg_sid- [in]  송신 대상의 SECS-II Message ID (Stream ID) (10진수)
		mesg_fid- [in]  송신 대상의 SECS-II Message ID (Function ID) (10진수)
		trans_id- [in]  송신 후 응답 받을 때, 동기화를 위해 필요한 ID (내가 보낸 물음에 응답임을 알기 위한 식별 ID) (0 값 초기화)
		is_reply- [in]  응답 (Ack) 요청 여부. TRUE - Host로부터 메시지 수신에 대한 응답 요청, FALSE - 응답 필요 없음
		is_body	- [in]  Message Body가 존재하는지 유무 즉, Message ID만 존재한다면 FALSE, 보낼 Message가 존재한다면 (TRUE)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SendSecsMessage(UINT8 conn_id, UINT8 mesg_sid, UINT8 mesg_fid,
										 BOOL is_reply, LONG &trans_id, BOOL is_body)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->SendSecsMessage(conn_id, mesg_sid, mesg_fid, is_reply, trans_id, is_body);
}

/* --------------------------------------------------------------------------------------------- */
/*                               ICxE30 Communication / Control                                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Communication Enable or Disable
 parm : conn_id	- [in] 1 or Later (공통 적용을 위해서는 0 값 사용)
		enable	- [in]  TRUE or FALSE
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetCommunication(LONG conn_id, BOOL enable)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SetCommunication(conn_id, enable);
}

/*
 desc : Change the GEM Control State to REMOTE or LOCAL
 parm : conn_id	- [in] 1 or Later (공통 적용을 위해서는 0 값 사용)
		state	- [in]  if true, go REMOTE. if false, go LOCAL
 retn : None
*/
API_EXPORT BOOL uvCIMLib_SetControlRemote(LONG conn_id, BOOL state)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SetControlRemote(conn_id, state);
}

/*
 desc : Change the GEM Control state to ONLINE or OFFLINE
 parm : conn_id	- [in] 1 or Later (공통 적용을 위해서는 0 값 사용)
		state	- [in]  if true, go ONLINE. if false, go OFFLINE
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetControlOnline(LONG conn_id, BOOL state)
{	 
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SetControlOnline(conn_id, state);
}

/*
 desc : Spooling Overwrite 여부
 parm : conn_id		- [in]  연결된 장비 ID
		overwrite	- [in]  Overwrite (TRUE) or Not overwrite (FALSE)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetSpoolOverwrite(LONG conn_id, BOOL overwrite)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SetSpoolOverwrite(conn_id, overwrite);
}

/*
 desc : Spooling State 제어
 parm : conn_id	- [in]  연결된 장비 ID
		enable	- [in]  Enable (TRUE) or Disable (FALSE)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetSpoolState(LONG conn_id, BOOL enable)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SetSpoolState(conn_id, enable);
}

/*
 desc : Spooling State 제어
 parm : conn_id	- [in]  연결된 장비 ID
		enable	- [in]  Enable (TRUE) or Disable (FALSE)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetSpoolPurge(LONG conn_id)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SetSpoolPurge(conn_id);
}

/*
 desc : Enables or Disables alarm reporting for a host for the specified alarms.
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		alarm_id- [in]  Connection Variables or Alarms의 Section에 나열된 variables identification number
		state	- [in]  TRUE (ENABLE) or FALSE (DISABLE)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetAlarmsEnabled(LONG conn_id, LONG alarm_id, BOOL state)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SetAlarmsEnabled(conn_id, alarm_id, state);
}

/*
 desc : 현재 CIMConnect 내부에 발생된 알람 모두 해제
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_AlarmClearAll()
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->AlarmClearAll();
}

/*
 desc : 현재 CIMConnect 내부에 발생된 알람 모두 해제
 parm : var_id	- [in]  Alarm ID 즉, EPJ Variables ID와 동일 함
		var_name- [in]  Variable Name (Alarm Name)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_AlarmClearID(LONG var_id)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->AlarmClear(var_id);
}
API_EXPORT BOOL uvCIMLib_AlarmClearName(PTCHAR var_name)
{
	LONG lID = -1;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	if (!g_pE30GEM->GetVarNameToID(0, var_name, lID))	return FALSE;
	return g_pE30GEM->AlarmClear(lID);
}

/*
 desc : 현재 등록 (발생)된 알람 ID 값들 반환
 parm : id		- [out] 알람 리스트가 저장된 CAtlList <LONG> : 알람 ID
		state	- [out] 알람 리스트가 저장된 CAtlList <LONG>: 알람 상태 (0 or 1)
		alarm	- [in]  알람이 발생된 정보만 반환 여부 (FLALSE:  모두, TRUE: 알람이 발생된 것만)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetAlarmList(CAtlList <LONG> &id, CAtlList <LONG> &state, BOOL alarm)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetAlarmList(id, state, alarm);
}

/*
 desc : 개별 알람 상태 값 반환
 parm : id		- [in]  알람 ID
		state	- [out] 알람 상태 반환 (1 : Set, 0 : Clear)
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetAlarmState(LONG id, LONG &state)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetAlarmState(id, state);
}

/*
 desc : CIMConnect 내부로 임의의 알람 발생
 parm : var_id	- [in]  Alarm ID 즉, EPJ Variables ID와 동일 함
		var_name- [in]  Alarm name
		text	- [in]  Text about the alarm. If empty, the default text for this alarm is passed to the host.
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_AlarmSetID(LONG var_id)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->AlarmSet(var_id);
}
API_EXPORT BOOL uvCIMLib_AlarmSetName(PTCHAR var_name, PTCHAR text)
{
	LONG lID = -1;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	if (!g_pE30GEM->GetVarNameToID(0, var_name, lID))	return FALSE;
	return g_pE30GEM->AlarmSet(lID, text);
}

/*
 desc : Event 발생 (Method to trigger an event)
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		evt_id	- [in]  Collection Event ID
		evt_name- [in]  Collection Event Name
 retn : TRUE or FALSE
 note : This method triggers an equipment event
*/
API_EXPORT BOOL uvCIMLib_SetTrigEventID(LONG conn_id, LONG evt_id)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SetTrigEvent(conn_id, evt_id);
}
API_EXPORT BOOL uvCIMLib_SetTrigEventName(LONG conn_id, PTCHAR evt_name)
{
	LONG lID	= -1;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	if (!g_pE30GEM->GetEventNameToID(conn_id, evt_name, lID, TRUE))	return FALSE;
	return g_pE30GEM->SetTrigEvent(conn_id, lID);
}

/*
 desc : Trigger Event Data 발생 (부모에게 알림) - 1 개 값에 대해 설정
 parm : conn_id	- [in] 1 or Later (공통 적용을 위해서는 0 값 사용)
		evt_id	- [in]  Collection Event ID
		evt_name- [in]  Collection Event Name
		var_id	- [in]  Variable ID
		var_name- [in]  Variable Name
		value	- [in]  값 이름
		type	- [in]  'value' type (U1, U2, U4, Bo, I1, I3, ...)
 retn : TRUE or FALSE
 note : 단, list, jis_8_string 등은 제외
*/
API_EXPORT BOOL uvCIMLib_SetTrigEventValueID(LONG conn_id, LONG evt_id, LONG var_id,
											 LNG_CVVT value, E30_GPVT type)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;

	return g_pE30GEM->SetTrigEventValue(conn_id, evt_id, var_id, value, type);
}
API_EXPORT BOOL uvCIMLib_SetTrigEventValueName(LONG conn_id, PTCHAR evt_name, PTCHAR var_name,
											   LNG_CVVT value, E30_GPVT type)
{
	LONG lVarID	= -1, lEvtID = -1;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	if (!g_pE30GEM->GetEventNameToID(conn_id, evt_name, lEvtID, TRUE))	return FALSE;
	if (!g_pE30GEM->GetVarNameToID(conn_id, var_name, lVarID, TRUE))	return FALSE;
	return g_pE30GEM->SetTrigEventValue(conn_id, lEvtID, lVarID, value, type);
}

/*
 desc : Trigger Event Data 발생 (부모에게 알림) - n 개 값에 대해 설정
 parm : conn_id	- [in] 1 or Later (공통 적용을 위해서는 0 값 사용)
		evt_id	- [in]  Event ID
		var_id	- [in]  Variable ID가 저장된 리스트
		evt_name- [in]  Event Name
		var_name- [in]  Variable Name이 저장된 리스트
		value	- [in]  Variable Value가 저장된 리스트
		type	- [in]  'value' type이 저장된 리스트 (U1, U2, U4, Bo, I1, I3, ...)
 retn : TRUE or FALSE
 note : 단, list, jis_8_string 등은 제외
*/
API_EXPORT BOOL uvCIMLib_SetTrigEventValuesID(LONG conn_id, LONG evt_id,
											  CAtlList<LONG> &var_id, CAtlList<UNG_CVVT> &value,
											  CAtlList<E30_GPVT> &type)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->SetTrigEventValues(conn_id, evt_id, var_id, value, type);
}
API_EXPORT BOOL uvCIMLib_SetTrigEventValuesName(LONG conn_id, PTCHAR evt_name,
												CStringArray &var_name, CAtlList<UNG_CVVT> &value,
												CAtlList<E30_GPVT> &type)
{
	BOOL bSucc	= FALSE;
	LONG lVarID	= -1, lEvtID = -1, i = 0;
	TCHAR tzVarName[CIM_CMD_NAME_SIZE];
	CAtlList <LONG> lstVarIDs;

	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	/* Event Name -> Event ID */
	if (!g_pE30GEM->GetEventNameToID(conn_id, evt_name, lEvtID, TRUE))	return FALSE;
	for (; i<var_name.GetCount(); i++)
	{
		swprintf_s(tzVarName, CIM_CMD_NAME_SIZE, L"%s", var_name[i].GetBuffer());
		/* Variable Name -> Variable ID */
		bSucc	= g_pE30GEM->GetVarNameToID(conn_id, tzVarName, lVarID, TRUE);
		if (!bSucc)	break;
		/* Add to Variable ID */
		lstVarIDs.AddTail(lVarID);
	}
	/* Trigger Event 설정  */
	if (bSucc)	bSucc = g_pE30GEM->SetTrigEventValues(conn_id, lEvtID, lstVarIDs, value, type);
	/* Array 메모리 반환 */
	lstVarIDs.RemoveAll();

	return bSucc;
}

/*
 desc : Trigger Event Data 발생 (부모에게 알림) - 여러 개 값에 대해 설정
 parm : conn_id	- [in] 1 or Later (공통 적용을 위해서는 0 값 사용)
		evt_id	- [in]  Event ID
		var_id	- [in]  Variable ID
		evt_name- [in]  Event Name
		var_name- [in]  Variable Name
		value	- [in]  Variable Value가 저장된 리스트
		type	- [in]  'value' type이 저장된 리스트 (U1, U2, U4, Bo, I1, I3, ...)
 retn : TRUE or FALSE
 note : 단, jis_8_string 등은 제외
*/
API_EXPORT BOOL uvCIMLib_SetTrigEventListID(LONG conn_id, LONG evt_id, LONG var_id,
											CAtlList<UNG_CVVT> &value, CAtlList<E30_GPVT> &type)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->SetTrigEventList(conn_id, evt_id, var_id, value, type);
}
API_EXPORT BOOL uvCIMLib_SetTrigEventListName(LONG conn_id, PTCHAR evt_name, PTCHAR var_name,
											  CAtlList<UNG_CVVT> &value, CAtlList<E30_GPVT> &type)
{
	LONG lVarID	= -1, lEvtID = -1;

	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	/* Event Name -> Event ID */
	if (!g_pE30GEM->GetEventNameToID(conn_id, evt_name, lEvtID, TRUE))	return FALSE;
	/* Variable Name -> Variable ID */
	if (!g_pE30GEM->GetVarNameToID(conn_id, var_name, lVarID, TRUE))	return FALSE;

	return g_pE30GEM->SetTrigEventList(conn_id, lEvtID, lVarID, value, type);
}

/*
 desc : Update the GEM Processing State in CIMConnect. 
		Each equipment supplier should customzie the GEM Processing State Model
		to match the actual tool's processing states. This is just a  
		simple example with three states: IDLE, SETUP and EXECUTING. 
 parm : state	- [in]  동작 상태 (동작 상태 : 0 (Setup), 1 (Idle), 2 (executing))
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetUpdateProcessState(E30_GPSV state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->SetUpdateProcessState(state);
}

/*
 desc : Send a terminal service acknowledge event to the host. Send to host 1 by default.
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
 retn : TRUE or FALSE
 참조 : This method triggers an equipment well-known event
		Equipment events are momentary, not latched
		There is no method to reset events since they are a one-shot, non-latched event.
*/
API_EXPORT BOOL uvCIMLib_SendTerminalAcknowledge(LONG conn_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->SendTerminalAcknowledge(conn_id);
}

/*
 desc : Send a terminal service text message to the host.
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		term_id	- [in]  Terminal ID (0 or Later)
		mesg	- [in]  The text message
 retn : TRUE or FALSE
 참조 : 참조 : This method requests that a terminal message to be sent to a connection
*/
API_EXPORT BOOL uvCIMLib_SendTerminalMessage(LONG conn_id, LONG term_id, PTCHAR mesg)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->SendTerminalMessage(conn_id, term_id, mesg);
}

/*
 desc : Notify CIMConnect that an operator command has been issued.
 parm : conn_id	- [in]  Connection #
		name	- [in]  Name of the command
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_OperatorCommand(LONG conn_id, PTCHAR name, LONG handle)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->OperatorCommand(conn_id, name, handle);
}

/*
 desc : 장치에 정의된 모든 데이터 항목들을 제거 (주의해서 사용해야 됨)
 parm : logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetClearProject( BOOL logs)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	if (!g_pE30GEM->SetClearProject(logs))	return FALSE;
	return TRUE;
}

/*
 desc : 장비 내에 존재하는 알람 ID 제거
 parm : alarm_id- [in]  제거하고자 하는 알람 ID
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetDeleteAlarm(LONG alarm_id)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SetDeleteAlarm(alarm_id);
}

/*
 desc : 모든 요청받은 데이터 (명령?)들 제거
 parm : logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetDeleteAllRequests(BOOL logs)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SetDeleteAllRequests(logs);
}

/*
 desc : 알람 이름에 해당되는 알람 ID 값 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		name	- [in]  알람 이름
		id		- [out] 알람 ID 반환
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetAlarmID(LONG conn_id, PTCHAR name, LONG &id)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetAlarmID(conn_id, name, id);
}

/*
 desc : 알람 ID에 대한 설명 반환
 parm : alarm_id- [in]  알람 ID
		desc	- [out] 알람 설명이 반환될 버퍼
		size	- [in]  'desc' 버퍼 크기
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetAlarmIDToDesc(LONG alarm_id, PTCHAR desc, UINT32 size)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetAlarmIDToDesc(alarm_id, desc, size);
}

/*
 desc : 알람 ID에 대한 명칭 반환
 parm : alarm_id- [in]  알람 ID
		name	- [out] 알람 명칭이 반환될 버퍼
		size	- [in]  'name' 버퍼 크기
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetAlarmIDToName(LONG alarm_id, PTCHAR name, UINT32 size)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetAlarmIDToName(alarm_id, name, size);
}

/*
 desc : 통신 상태 (Low Level Communicating state) 값 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		state	- [out] 통신 여부
		substate- [out] HSMS (Ethernet) 상태 여부
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetCommState(LONG conn_id, E30_GCSS &state, E30_GCSH &substate)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetCommState(conn_id, state, substate);
}

/*
 desc : Host 연결에 대한 이름 즉, connection id에 매핑되는 이름 얻기
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		name	- [out] 연결된 Host Name 값 얻기
		size	- [in]  'name' 버퍼 크기
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetConnectionName(LONG conn_id, PTCHAR name, UINT32 size)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetConnectionName(conn_id, name, size);
}

/*
 desc : 현재 적재된 Project Name 얻기
 parm : name	- [out] Project Name 값 얻기
		size	- [in]  'name' 버퍼 크기
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetCurrentProjectName(PTCHAR name, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->GetCurrentProjectName(name, size);
}

/*
 desc : 기본 설정된 Default Project Name 얻기
 parm : name	- [out] Project Name 값 얻기
		size	- [in]  'name' 버퍼 크기
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetDefaultProjectName(PTCHAR name, UINT32 size)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetDefaultProjectName(name, size);
}

/*
 desc : 현재 연결된 장비 ID 값 반환
 parm : equip_id- [out] 매핑(관리)되는 장비 ID 값 저장
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetHostToEquipID(LONG &equip_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->GetHostToEquipID(equip_id);
}

/*
 desc : Event Name에 해당되는 Event ID 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		name	- [in]  Event Name
		id		- [out] Event ID가 반환되어 저장됨
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetEventNameToID(LONG conn_id, PTCHAR name, LONG &id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->GetEventNameToID(conn_id, name, id);
}

/*
 desc : Variable Name에 해당되는 Variable ID 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		name	- [in]  Variable Name
		var_id	- [out] Variable ID가 반환되어 저장됨
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetVarNameToID(LONG conn_id, PTCHAR name, LONG &var_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->GetVarNameToID(conn_id, name, var_id);
}

/*
 desc : Variable ID에 해당되는 Variable Name 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		var_id	- [in]	Variable ID
		name	- [out] Variable Name이 반환되는 버퍼
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetVarIDToName(LONG conn_id, LONG var_id, PTCHAR name, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->GetVarIDToName(conn_id, var_id, name, size);
}

/*
 desc : Variable ID에 해당되는 Variable Description 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		var_id	- [in]	Variable ID
		desc	- [out] Variable Description이 반환되는 버퍼
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetVarIDToDesc(LONG conn_id, LONG var_id, PTCHAR desc, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->GetVarIDToDesc(conn_id, var_id, desc, size);
}

/*
 desc : Variable ID에 해당되는 Min, Max 값 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		var_id	- [in]  Variable ID
		type	- [out] Value Type (E30_GPVT)이 저장됨
		min		- [out] Min 값이 저장됨
		max		- [out] Max 값이 저장됨
		size	- [in]  'min, max' 문자열 버퍼의 크기
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetVarMinMax(LONG conn_id, LONG var_id, E30_GPVT &type, UNG_CVVT &min, UNG_CVVT &max)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->GetVarMinMax(conn_id, var_id, type, min, max);
}
API_EXPORT BOOL uvCIMLib_GetVarMinMaxToStr(LONG conn_id, LONG var_id, PTCHAR min, PTCHAR max, UINT32 size)
{
	E30_GPVT enType;
	UNG_CVVT unMin	= {NULL}, unMax = {NULL};
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	if (!g_pE30GEM->GetVarMinMax(conn_id, var_id, enType, unMin, unMax))	return FALSE;

	switch (enType)
	{
	case E30_GPVT::en_i1		:	swprintf_s(min, size, L"%d",	unMin.i8_value);
							swprintf_s(max, size, L"%d",	unMax.i8_value);	break;
	case E30_GPVT::en_i2		:	swprintf_s(min, size, L"%d",	unMin.i16_value);
							swprintf_s(max, size, L"%d",	unMax.i16_value);	break;
	case E30_GPVT::en_i4		:	swprintf_s(min, size, L"%ld",	unMin.i32_value);
							swprintf_s(max, size, L"%ld",	unMax.i32_value);	break;
	case E30_GPVT::en_i8		:	swprintf_s(min, size, L"%I64d",	unMin.i64_value);
							swprintf_s(max, size, L"%I64d",	unMax.i64_value);	break;
	case E30_GPVT::en_u1		:	swprintf_s(min, size, L"%u",	unMin.i8_value);
							swprintf_s(max, size, L"%u",	unMax.i8_value);	break;
	case E30_GPVT::en_u2		:	swprintf_s(min, size, L"%u",	unMin.i16_value);
							swprintf_s(max, size, L"%u",	unMax.i16_value);	break;
	case E30_GPVT::en_u4		:	swprintf_s(min, size, L"%lu",	unMin.i32_value);
							swprintf_s(max, size, L"%lu",	unMax.i32_value);	break;
	case E30_GPVT::en_u8		:	swprintf_s(min, size, L"%I64u",	unMin.i64_value);
							swprintf_s(max, size, L"%I64u",	unMax.i64_value);	break;
	case E30_GPVT::en_f4		:	swprintf_s(min, size, L"%f",	unMin.f_value);
							swprintf_s(max, size, L"%f",	unMax.f_value);		break;
	case E30_GPVT::en_f8		:	swprintf_s(min, size, L"%lf",	unMin.d_value);
							swprintf_s(max, size, L"%lf",	unMax.d_value);		break;
	case E30_GPVT::en_b		:	swprintf_s(min, size, L"%d",	unMin.b_value);
							swprintf_s(max, size, L"%d",	unMax.b_value);		break;
	case E30_GPVT::en_bi		:	swprintf_s(min, size, L"%d",	unMin.bi_value);
							swprintf_s(max, size, L"%d",	unMax.bi_value);	break;
	case E30_GPVT::en_w		:
	case E30_GPVT::en_a		:	swprintf_s(min, size, L"%s",	unMin.s_value);
							swprintf_s(max, size, L"%s",	unMax.s_value);		break;
	case E30_GPVT::en_list	:
	case E30_GPVT::en_jis8	:
	default				:	return FALSE;
	}

	return TRUE;
}

/*
 desc : 현재 연결되어 있는 Host ID 기준으로 변수들 정보 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		type	- [in]  변수 타입 (E30_GVTC)
		lst_data- [out] 변수들의 정보가 저장될 리스트 참조
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetHostVariables(LONG conn_id, E30_GVTC type, CAtlList <STG_CEVI> &lst_data)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->GetHostVariables(conn_id, type, lst_data);
}

/*
 desc : 로그 상태 (파일에 로그 파일이 저장 중인지 여부)
 parm : enable	- [out] 로그 파일 저장 여부 값 반환
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsLogging(BOOL &enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->IsLogging(enable);
}

/*
 desc : 로그 동작 유무 설정
 parm : enable	- [in]  로그 파일 동작 유무 설정 플래그
						enable=TRUE -> Start, enable=FALSE -> Stop
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetLogEnable(BOOL enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->SetLogEnable(enable);
}

/*
 desc : 현재 등록 (발생)된 명령 ID 값들 반환
 parm : id		- [out] 리스트가 저장될 CAtlList <LONG> : ID
		name	- [out] 리스트가 저장될 String Array    : Name
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetCommandList(CAtlList <LONG> &id, CStringArray &name)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetCommandList(id, name);
}

/*
 desc : 현재 등록 (발생)된 EPJ 파일들 반환
 parm : files	- [out] 리스트가 저장될 String Array    : EPJ Files (Included Path)
		sizes	- [out] 리스트가 저장될 CAtlList <LONG> : EPJ File Size
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetEPJList(CStringArray &files, CAtlList <LONG> &sizes)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetEPJList(files, sizes);
}

/*
 desc : 현재 등록 (발생)된 Recipe Names 반환
 parm : names	- [out] 리스트가 저장될 String Array    : Recipe Names
		sizes	- [out] 리스트가 저장될 CAtlList <LONG> : Recipe Sizes
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetRecipes(CStringArray &names, CAtlList <LONG> &sizes)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetRecipes(names, sizes);
}

/*
 desc : Host 쪽에 Process Program (Recipe)를 생성하겠다고 요청
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		name	- [in]  Recipe (Process Program) Name (확장자는 꼭 포함하지 않아도 됨)
		size	- [in]  Recipe (Process Program) Size (unit: bytes)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_QueryRecipeUpload(LONG conn_id, PTCHAR name, UINT32 size)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->QueryRecipeUpload(conn_id, name, size);
}

/*
 desc : Host 쪽에 Process Program (Recipe)를 요청
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		name	- [in]  Recipe (Process Program) Name (확장자는 꼭 포함하지 않아도 됨)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_QueryRecipeRequest(LONG conn_id, PTCHAR name)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->QueryRecipeRequest(conn_id, name);
}

/*
 desc : 시스템에 정의된 Report 목록 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		ids		- [out] 리스트가 저장될 CAtlList <LONG> : Report Names
		names	- [out] 리스트가 저장될 String Array    : Report IDs
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetReports(LONG conn_id, CAtlList <LONG> &ids, CStringArray &names)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetReports(conn_id, ids, names);
}

/*
 desc : Report 및 관련된 Event의 리스트 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		r_ids	- [out] 리스트가 저장될 CAtlList <LONG> : Report IDs
		e_ids	- [out] 리스트가 저장될 CAtlList <LONG> : Event IDs
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetReportsEvents(LONG conn_id, CAtlList <LONG> &r_ids, CAtlList <LONG> &e_ids)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetReportsEvents(conn_id, r_ids, e_ids);
}

/*
 desc : 발생된 Traces의 리스트 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		ids		- [out] 리스트가 저장될 CAtlList <LONG> : Report IDs
		names	- [out] 리스트가 저장될 String Array    : Trance Names
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetListTraces(LONG conn_id, CAtlList <LONG> &ids, CStringArray &names)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetListTraces(conn_id, ids, names);
}

/*
 desc : EPJ 파일에 등록된 variables 목록 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		type	- [in]  Variable Type (E30_GVTC)
		ids		- [out] Variable IDs 반환
		names	- [out] Variable Names 반환
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetListVariables(LONG conn_id, E30_GVTC type, CAtlList <LONG> &ids,
										  CStringArray &names)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetListVariables(conn_id, type, ids, names);
}

/*
 desc : List all messages (SECS-II 모든 메시지 반환)
		이 메서드는 장비 인터페이스에서 현재 처리 중인 메시지의 메시지 ID 목록을 반환합니다.
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		ids		- [out] message IDs 반환
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetListMessageIDs(LONG conn_id, CAtlList <LONG> &ids)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetListMessageIDs(conn_id, ids);
}

/*
 desc : Variable ID에 해당되는 (E30_GPVT) 값 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		var_id	- [in]  Variable ID와 매핑되는 Name
		var_name- [in]  Variable Name
		type	- [out] Value Type
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetValueTypeID(LONG conn_id, LONG var_id, E30_GPVT &type)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetValueType(conn_id, var_id, type);
}
API_EXPORT BOOL uvCIMLib_GetValueTypeName(LONG conn_id, PTCHAR var_name, E30_GPVT &type)
{
	LONG lID = -1;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	if (!g_pE30GEM->GetVarNameToID(conn_id, var_name, lID))	return FALSE;
	return g_pE30GEM->GetValueType(conn_id, lID, type);
}

/*
 desc : Variable ID에 해당되는 Variable Type (E30_GVTC) 값 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		var_id	- [in]  Variable ID와 매핑되는 Name
		var_name- [in]  Variable Name
		type	- [out] Variable Type
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetVarTypeID(LONG conn_id, LONG var_id, E30_GVTC &type)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetVarType(conn_id, var_id, type);
}
API_EXPORT BOOL uvCIMLib_GetVarTypeName(LONG conn_id, PTCHAR var_name, E30_GVTC &type)
{
	LONG lID = -1;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	if (!g_pE30GEM->GetVarNameToID(conn_id, var_name, lID))	return FALSE;
	return g_pE30GEM->GetVarType(conn_id, lID, type);
}

/*
 desc : 연결된 통신 설정 정보 얻기
 parm : conn_id		- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		comm_pgid	- [out] The COM ProgID of the communications object
		size_pgid	- [in]  'comm_pgid'에 저장된 문자열 길이
		comm_sets	- [out] The communications settings
		size_sets	- [in]  'comm_sets'에 저장된 문자열 길이
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetCommSetupInfo(LONG conn_id, PTCHAR comm_pgid, UINT32 size_pgid,
										  PTCHAR comm_sets, UINT32 size_sets)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetCommSetupInfo(conn_id, comm_pgid, size_pgid, comm_sets, size_sets);
}

/*
 desc : 연결된 통신 설정
 parm : conn_id		- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		comm_pgid	- [in]  The COM ProgID of the communications object
		comm_sets	- [in]  The communications settings
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetCommSetupInfo(LONG conn_id, PTCHAR comm_pgid, PTCHAR comm_sets)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SetCommSetupInfo(conn_id, comm_pgid, comm_sets);
}

/*
 desc : 로그 저장 관련 환경 설정 값을 윈도 레지스트리에 저장할지 여부 설정
 parm : enable	- [in]  TRUE : 저장, FALSE : 저장 안함
		logs	- [in]  로그 저장 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetLoggingConfigSave(BOOL enable)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SetLoggingConfigSave(enable);
}

/*
 desc : 로그 저장 관련 환경 설정 값의 저장 여부 값 반환
 parm : enable	- [out] TRUE : 저장하고 있음, FALSE : 저장 안하고 있음
		logs	- [in]  로그 저장 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetLoggingConfigSave(BOOL &enable)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetLoggingConfigSave(enable);
}

/*
 desc : SECS-II Message 송신 (전송) 즉, Primary Message 송신? 기능
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		msg_sid	- [in]  SECS-II Message ID : Stream ID (0xssff -> 0x00xx ~ 0xffxx 까지)
		msg_fid	- [in]  SECS-II Message ID : Function ID (0xssff -> 0xxx00 ~ 0xxxFF 까지)
		term_id	- [in]  보통 0 값이나, 자신의 Terminal ID가 1 이상인 경우도 있다 (?)
		str_msg	- [in]  메시지 내용 (문자열), 만약 NULL이면 메시지 본문이 없는 경우임
		reply	- [in]  응답 기대 여부 (Host로부터 응답 받기를 원하는지 여부)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SendPrimaryMessage(LONG conn_id, UINT8 msg_sid, UINT8 msg_fid,
											PTCHAR str_msg, UINT8 term_id, BOOL reply)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SendPrimaryMessage(conn_id, msg_sid, msg_fid, str_msg, term_id, reply);
}

/*
 desc : Application Name 반환
 parm : name	- [out] Application Name이 저장될 버퍼
		size	- [in]  'name' 버퍼 크기
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetAppName(PTCHAR name, UINT32 size)
{
	if (!g_pE30GEM)	return FALSE;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetAppName(name, size);
}

/*
 desc : Application Name 설정
 parm : name	- [in]  Application Name이 저장된 버퍼
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetAppName(PTCHAR name)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SetAppName(name);
}

/*
 desc : 현재 프로젝트의 패키지가 빌드된 시점의 날짜 (시간 포함) 반환
 parm : date_time	- [out] 빌드된 시간 (날짜포함)이 저장될 버퍼
		size		- [in]  'data_time' 버퍼 크기
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetBuildDateTime(PTCHAR date_time, UINT32 size)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetBuildDateTime(date_time, size);
}

/*
 desc : 현재 프로젝트의 패키지가 빌드된 시점의 Version 반환
 parm : version	- [out] 빌드된 시간 (날짜포함)이 저장될 버퍼
		size	- [in]  'version' 버퍼 크기
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetBuildVersionStr(PTCHAR version, UINT32 size)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetBuildVersion(version, size);
}

/*
 desc : Get Package verion (for number)
 parm : major	- [out] Returned Major version revision number.
		minor	- [out] Returned Major version revision number.
		patch	- [out] Returned Patch number.
		build	- [out] Returned Build number.
 retn : TRUE or FALSE
 note : returns package version information
*/
API_EXPORT BOOL uvCIMLib_GetBuildVersion(LONG &major, LONG &minor, LONG &patch, LONG &build)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->GetBuildVersion(major, minor, patch, build);
}

/*
 desc : 임의 (특정) 장치에 연결되어 있는 응용 프로그램의 IDs와 이름들을 반환
 parm : equip_id- [in]  장비 ID (0, 1 or Later)
		ids		- [out] 응용 프로그램 IDs 반환 (ID 숫자 리스트로 반환)
		names	- [out] 응용 프로그램 Names 반환 (문자열 배열로 반환)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetListApplications(LONG equip_id, CAtlList <LONG> &ids, CStringArray &names)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetListApplications(equip_id, ids, names);
}

/*
 desc : 임의 (특정) 장치에 연결되어 있는 Host의 IDs와 이름들을 반환
 parm : conn_id	- [in]  장비 ID (0, 1 or Later)
		ids		- [out] Host IDs 반환 (ID 숫자 리스트로 반환)
		names	- [out] Host Names 반환 (문자열 배열로 반환)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetListConnections(LONG conn_id, CAtlList <LONG> &ids, CStringArray &names)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetListConnections(conn_id, ids, names);
}

/*
 desc : EM Service에 연결되어 있는 Equipment의 IDs와 이름들을 반환
 parm : equip_id- [in]  장비 ID (0, 1 or Later)
		ids		- [out] Equipment IDs 반환 (ID 숫자 리스트로 반환)
		names	- [out] Equipment Names 반환 (문자열 배열로 반환)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetListEquipments(CAtlList <LONG> &ids, CStringArray &names)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetListEquipments(ids, names);
}

/*
 desc : 현재 Control이 Remote 모드인지 여부
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsControlRemote(LONG conn_id)
{
	if (!g_pE30GEM->IsInitCompleted())	return FALSE;
	return g_pE30GEM->IsControlRemote(conn_id, FALSE);
}

/*
 desc : 현재 Control이 Online 모드인지 여부
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsControlOnline(LONG conn_id)
{
	if (!g_pE30GEM->IsInitCompleted())	return FALSE;
	return g_pE30GEM->IsControlOnline(conn_id, FALSE);
}

/*
 desc : 현재 Communication모드가 Enable인지 여부
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsCommEnabled(LONG conn_id)
{
	if (!g_pE30GEM->IsInitCompleted())	return FALSE;
	return g_pE30GEM->IsCommEnabled(conn_id, FALSE);
}

/* --------------------------------------------------------------------------------------------- */
/*                            E40 Processing Job Management (PJM)                                */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Process Job을 SETTING UP 상태에서 WAITING FOR START 내지 PROCESSING 상태로 이동
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
 note : Moves Process Job from SETTING UP state to WAITING FOR START or PROCESSING state
		소재가 준비되고 Process Job이 시작할 준비가 되어 있다고 알림
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetPRJobStartProcess(PTCHAR pj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetPRJobStartProcess(pj_id);
}

/*
 desc : Process Job이 속한 Control Job ID 반환
 parm : pj_id	- [in]  Object ID of the Process Job
		cj_id	- [out] Object ID of the Control Job
		size	- [in]  'cj_id' 버퍼 크기
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetControlJobID(PTCHAR pj_id, PTCHAR cj_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetControlJobID(pj_id, cj_id, size);
}

/*
 desc : Process Job이 속한 Control Job ID 설정
 parm : pj_id	- [in]  Object ID of the Process Job
		cj_id	- [in]  Object ID of the Control Job
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetControlJobID(PTCHAR pj_id, PTCHAR cj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetControlJobID(pj_id, cj_id);
}

/*
 desc : Process Job의 Processing이 끝난 경우임
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
 note : Moves Process Job from PROCESSING state to PROCESS COMPLETE state.
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetPRJobProcessComplete(PTCHAR pj_id)
{
	/* Completed : Process Job */
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetPRJobProcessComplete(pj_id);
}

/*
 desc : Process Job이 끝난 경우 (Job Processed 상태인 경우만 해당됨)
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetPRJobComplete(PTCHAR pj_id)
{
	/* Completed : Process Job */
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetPRJobComplete(pj_id);
}

/*
 desc : Process Job을 Stopping 상태에서 Stopped 상태로 이동 (변경)
 parm : pj_id	- [in]  Objet ID of the Process Job
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetProcessJobStoppingToStopped(PTCHAR pj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetProcessJobStoppingToStopped(pj_id);
}

/*
 desc : Processing Resource를 위해 Queue에서 생성될 수 있는 남아 있는 Jobs 개수 반환
 parm : count	- [out] 생성될 수 있는 여유 개수 반환 값이 저장
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetProcessJobSpaceCount(UINT32 &count)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetProcessJobSpaceCount(count);
}

/*
 desc : Process Job (ID)의 현재 상태 반환
 parm : pj_id	- [in]  Process Job ID
		state	- [OUT] state
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetProcessJobState(PTCHAR pj_id, E40_PPJS &state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetProcessJobState(pj_id, state);
}

/*
 desc : 모든 Process Job Id 반환 (완료되지 않은 작업의 상태만 반환)
 parm : list_job- [out] Job ID가 반환되어 저장될 배열 버퍼
		logs	- [in]  로그 출력 여부
 변환 : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetProcessJobQueue(CStringArray &list_job)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetProcessJobQueue(list_job);
}

/*
 desc : 현재 Process Job ID 中 맨 처음 SELECTED 상태인 Process Job ID 반환
 parm : filter	- [in]  Process Job ID 검색 필터 (E40_PPJS)
		pj_id	- [out] 반환되어 저장될 Process Job ID (String)
		size	- [in]  'pj_id' 버퍼 크기
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
 note : 검색된 결과가 없으면 FALSE 반환
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetProcessJobFirstID(E40_PPJS filter, PTCHAR pj_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetProcessJobFirstID(filter, pj_id, size);
}

/*
 desc : 모든 Process Job Id와 State 반환
 parm : list_job	- [out] Job ID가 반환되어 저장될 배열 버퍼
		list_state	- [out] 상태 값이 저자장될 리스트 버퍼
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetProcessAllJobIdState(CStringArray &list_job,
														CAtlList<E40_PPJS> &list_state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetProcessAllJobIdState(list_job, list_state);
}

/*
 desc : Process Job Remove
 parm : pj_id	- [in]  제거하고자 하는 Process Job Id
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetProcessJobRemove(PTCHAR pj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetProcessJobRemove(pj_id);
}

/*
 desc : Process Job을 Queuing 상태에서 Setting Up 상태로 이동
 parm : pj_id	- [in]  Objet ID of the Process Job
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetProcessJobQueueToSetup(PTCHAR pj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetProcessJobQueueToSetup(pj_id);
}

/*
 desc : Process Job 생성
 parm : pj_id		- [in]  Process Job Name ID (String; Ascii)
		mtrl_type	- [in]  Material Type (E40_PPMT)
		carr_id		- [in]  Carrier Name ID (String; Ascii) List
		recipe_name	- [in]  Recipe Name ID
		slot_no		- [in]  Slot Number (U1 Type) List (1 ~ 25 중 임의 개수만큼 저장됨)
		recipe_only	- [in]  TRUE  : RECIPE_ONLY (Recipe method without recipe variable)
							FALSE : RECIPE_WITH_VARIABLE_TUNING (Recipe method with recipe variable)
		proc_start	- [in]  Process Start 여부
							Indicates that the processing resource starts processing imediately
							when ready (PRProcessStart) (Auto Start 여부)
		en_hanced	- [in]  Enhanced create 함수 적용 여부
 retn : TRUE or FALSE
 note : This method is called by the host to create a new Process Job
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetProcessJobCreate(PTCHAR pj_id, PTCHAR carr_id,
													PTCHAR recipe_name, E40_PPMT mtrl_type,
													BOOL recipe_only, BOOL proc_start,
													CAtlList <UINT8> *slot_no, BOOL en_hanced)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetProcessJobCreate(pj_id, carr_id, recipe_name, mtrl_type, recipe_only,
										  proc_start, slot_no, en_hanced);
}

/*
 desc : Remote Control 허용 여부 설정
 parm : enable	- [in]  Enable or Disable
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetAllowRemoteControl(BOOL enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetAllowRemoteControl(enable);
}

/*
 desc : Remote Control 허용 여부 값 반환
 parm : enable	- [out] Enable or Disable
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetAllowRemoteControl(BOOL &enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetAllowRemoteControl(enable);
}

/*
 desc : Process Material Name List (For substrates)의 속성 값 (Carrier ID, Slot ID) 반환
		!!! TEL의 경우, 무조건 2번째 즉, CarrierID 와 Slot No로 되어 있는 방식으로 읽어야 됨 !!!
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		carr_id	- [out] Carrier ID가 반환되어 저장될 버퍼
		slot_no	- [out] Slot Number가 반환되어 저장될 버퍼
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetPRMtlNameList(PTCHAR pj_id,
												 CStringArray &carr_id, CByteArray &slot_no)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetPRMtlNameList(pj_id, carr_id, slot_no);
}

/*
 desc : 현재 Process Job ID 내에 등록된 Slot Number List 중에서 찾고자 하는 Slot Number가 있는지 여부
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		slot_no	- [in]  검색하고자 하는 Slot Number (1-based)
		is_slot	- [out] Slot Number 존재 여부 반환
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_IsFindSlotNoOfPRMtlNameList(PTCHAR pj_id, UINT8 slot_no, BOOL &is_slot)
{
	UINT8 i	= 0x00;
	CStringArray arrCarID;
	 CByteArray arrSlotNo;

	/* 일단, 무조건 못 찾았다고 설정  */
	is_slot	= FALSE;

	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	/* 현재 Process Job ID에 속한 Carrier ID와 Slot Number 리스트 얻기 */
	if (!g_pE40PJM->GetPRMtlNameList(pj_id, arrCarID, arrSlotNo))	return FALSE;

	/* Process Job ID에 속한 Slot Number 중 */
	for (; i<arrCarID.GetCount(); i++)
	{
		if (arrSlotNo[i] == slot_no)
		{
			is_slot	= TRUE;
			return TRUE;
		}
	}

	/* 메모리 반환 */
	arrCarID.RemoveAll();
	arrSlotNo.RemoveAll();

	return TRUE;	/* 검색 결과와 상관 없이, CIMETRIX Function 호출 성공 여부임 */
}

/*
 desc : Process Job의 제어 동작 값을 강제로 제어 시킴
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		control	- [in]  E40_PPJC (Abort, Cancel, Pause, Paused, Resume, Start, Stop)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetPRJobStateControl(PTCHAR pj_id, E40_PPJC control)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetPRJobStateControl(pj_id, control);
}

/*
 desc : Process Job State 변경
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		state	- [in]  Process Job State (E40_PPJS)
 retn : TRUE or FALSE
 note : Sets the Process Job's state
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetProcessJobState(PTCHAR pj_id, E40_PPJS state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetProcessJobState(pj_id, state);
}

/*
 desc : Process Job을 ABORTING 상태에서 ABORTED 상태로 이동
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
 note : Moves Process Job from ABORTING state to the ABORTED state.
		작업 취소가 완료되었을 때, 이 함수를 호출해 주면 됨
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetPRJobAbortComplete(PTCHAR pj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetPRJobAbortComplete(pj_id);
}

/*
 desc : Process Job State : Aborting -> Aborted
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
 retn : TRUE or FALSE
 note : This method is called by the CIM40 package when aborting has completed
		Moves Process Job from ABORTING state to ABORTED state
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetProcessJobAborted(PTCHAR pj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetProcessJobAborted(pj_id);
}

/*
 desc : Returns the PRMtrlOrder value set by the host using the PRSetMtrlOrder service
		Queue에 등록될 때, Process Job ID의 정렬 방법을 반환 합니다.
 parm : order	- [out] Material Order (E94_CPOM, PRMtrlOrder value)
 retn : TRUE or FALSE
 note : This function returns the PRMtrlOrder value, the order in which materials are processed
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetPRMaterialOrder(E94_CPOM &order)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetPRMaterialOrder(order);
}

/*
 desc : Sets the PRMtrlOrder value by the host using PRSetMtrlOrder service
		Queue에서 Process Job ID가 저장되는 정렬 방법 설정
 parm : order	- [in]  Material Order (E94_CPOM, PRMtrlOrder value)
 retn : TRUE or FALSE
 note : Sets the PRMtrlOrder value by the host using PRSetMtrlOrder service.
		PRSetMtrlOrder callback is called to let the client application grant or deny this request.
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetPRMaterialOrder(E94_CPOM order)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetPRMaterialOrder(order);
}

/*
 desc : Process Job에서 명령을 실행 시킴 (Executes a command on a Process Job)
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		command	- [in]  명령어 (PCIMLib에서 등록된 명령어. START;STOP;PAUSE;RESUME;ABORT;CANCEL)
						Register for Remote Commands (Remote Command name의 총 길이가 20 string을 넘지 않아야 됨)
 retn : TRUE or FALSE
 note : This method is called by the host to perform a PRJobCommand on the specified Process Job
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetPRJobCommand(PTCHAR pj_id, PTCHAR command)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetPRJobCommand(pj_id, command);
}

/*
 desc : Pause Events에 대해 Enable or Disable
 parm : enable	- [in]  TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE
 note : The client application can disable PauseEvent capability for Process Jobs by setting this value to VARIANT_FALSE.
		Default is VARIANT_TRUE.
		When set to VARIANT_FALSE, all requests to create Process Jobs with the PauseEvent parameter will be rejected.
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetEnablePauseEvent(BOOL enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetEnablePauseEvent(enable);
}

/*
 desc : Pause Events에 대해 Enable or Disable 상태 값 반환
 parm : enable	- [out] TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetEnablePauseEvent(BOOL &enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetEnablePauseEvent(enable);
}

/*
 desc : 장비 쪽에서 사용하는 알림 (Notification)의 형식 (Type)이 무엇인지 알려줌
 parm : type	- [out] Notification Type (E40_PENT)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetNotificationType(E40_PENT &type)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetNotificationType(type);
}

/*
 desc : 장비 쪽에서 사용할 알림 (Notification)의 형식 (Type)이 어떤 것인지 설정해 줌
 parm : type	- [in]  Notification Type (E40_PENT)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetNotificationType(E40_PENT type)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetNotificationType(type);
}

/*
 desc : Process Job의 상태가 변경되었다고 Host에게 알림
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetPRJobAlert(PTCHAR pj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetPRJobAlert(pj_id);
}

/*
 desc : Process Job ID의 Event State가 변경되었다고 Host에게 알림
 parm : pj_id		- [in]  Process Job Name ID (String; Ascii)
		evt_id		- [in]  PREventID of PRJobEvent service (E40_PSEO)
		list_vid	- [in]  Variables ID가 저장된 리스트
		list_type	- [in]  Variables Value Type이 저장된 리스트
		list_value	- [in]  'Variables Type'에 따라, 숫자 혹은 문자열 등이 등록되어 있음
							참고로, 숫자일 경우, _wtoi / _wtof 등등 변환이 필요 함
 retn : TRUE or FALSE
 note : This method is called by the client application to notify the host about changes in a Process Job's state
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetPRJobEvent(PTCHAR pj_id, E40_PSEO evt_id,
											  CAtlList <UINT32> *list_vid,
											  CAtlList <E30_GPVT> *list_type,
											  CStringArray *list_value)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetPRJobEvent(pj_id, evt_id, list_vid, list_type, list_value);
}

/*
 desc : 임의 (특정) 프로세스 Job들을의 속성을 "PRProcessStart"로 상태 변경
 parm : list_jobs	- [in]  변경 대상의 Process Job들이 저장된 리스트
		start		- [in]  TRUE or FALSE
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetPRJobSetStartMethod(CStringArray *list_jobs, BOOL start)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetPRJobSetStartMethod(list_jobs, start);
}

/*
 desc : 기존 설정된 Material Type 값 반환
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		type	- [out] 기존 설정된 Material Type 값 저장
 retn : TRUE or FALSE
 note : Gets the PRMtlType attribute value
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetPRMtlType(PTCHAR pj_id, E40_PPMT &type)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetPRMtlType(pj_id, type);
}

/*
 desc : Material Type 값 새로 설정
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		type	- [in]  설정될 Maerial Type 값
 retn : TRUE or FALSE
 note : Sets the PRMtlType attribute value
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetPRMtlType(PTCHAR pj_id, E40_PPMT type)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetPRMtlType(pj_id, type);
}

/*
 desc : 처리를 시작하기 전에 모든 자재 (Wafer? Substrate)들이 도착할 때까지 대기하는지 여부 값 반환
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		value	- [out] TRUE : 모든 자재 (소재)들을 도착하는데 기다리고 있음
						FALSE: 모든 자재 (소재)들이 이미 도착한 상태임
 retn : TRUE or FALSE
 note : Gets the Process Job's PRWaitForAllMaterial switch value.
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetPRWaitForAllMaterial(PTCHAR pj_id, BOOL &value)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetPRWaitForAllMaterial(pj_id, value);
}

/*
 desc : 처리를 시작하기 전에 모든 자재 (Wafer? Substrate)들이 도착할 때까지 대기해야 할지 여부 설정
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		value	- [in]  TRUE : 모든 자재 (소재)들이 도착하는데 기다리고 있다라고 설정
						FALSE: 모든 자재 (소재)들이 도착하였다라고 설정 (기다릴 필요 없다?)
 retn : TRUE or FALSE
 note : Sets the Process Job's PRWaitForAllMaterial switch value.
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetPRWaitForAllMaterial(PTCHAR pj_id, BOOL value)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetPRWaitForAllMaterial(pj_id, value);
}

/*
 desc : Process Job ID로부터 Recipe ID (=Name; with string) 얻기
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		rp_id	- [out] Recipe ID가 저장될 버퍼
		size	- [in]  'rp_id' 버퍼 크기
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetRecipeID(PTCHAR pj_id, PTCHAR rp_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetRecipeID(pj_id, rp_id, size);
}

/*
 desc : 현재 Process Job ID에 대한 작업이 완료 되었는지 여부
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		state	- [out] 0x00: Busy, 0x01: Completed
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_IsProcessJobCompleted(PTCHAR pj_id, UINT8 &state)
{
	E40_PPJS enJobState;

	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	if (!g_pE40PJM->GetProcessJobState(pj_id, enJobState))	return FALSE;

	state	= (enJobState == E40_PPJS::en_processjob_completed) ? 0x01 : 0x00;

	return TRUE;
}

/*
 desc : 모든 Process Job Id 반환 (완료되지 않은 작업의 상태만 반환)
 parm : list_job- [out] Job ID가 반환되어 저장될 배열 버퍼
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetProcessAllJobId(CStringArray &list_job)
{
	if (!g_pE40PJM || !g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetProcessAllJobId(list_job);
}

/*
 desc : Process Job ID에 포함된 모든 Slot Number 반환
 parm : pj_id	- [in]  검색 대상의 Process Job ID
		arr_slot- [out] Slot Number가 저장될 Byte Array
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetPRJobToAllSlotNo(PTCHAR pj_id, CByteArray &arr_slot)
{
	CStringArray arrCarrID;

	if (!g_pE40PJM)	return FALSE;
	if (!g_pE30GEM->IsInitedAll(TRUE))	return FALSE;
	/* PRJob ID에 해당되는 Slot Number와 Carrier ID 리스트 얻기 */
	if (!g_pE40PJM->GetProcessJobAllSlotNo(pj_id, arr_slot, arrCarrID))	return FALSE;

	return TRUE;
}

/*
 desc : Process Job ID에 포함된 모든 Slot ID 반환
 parm : pj_id	- [in]  검색 대상의 Process Job ID
		arr_slot- [out] Slot ID가 저장될 String Array
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetPRJobToAllSlotID(PTCHAR pj_id, CStringArray &arr_slot)
{
	UINT8 i	= 0x00;
	CByteArray arrSlotNo;
	CStringArray arrCarrID;
	CString strSlotID;

	if (!g_pE40PJM)	return FALSE;
	if (!g_pE30GEM->IsInitedAll(TRUE))	return FALSE;
	/* PRJob ID에 해당되는 Slot Number와 Carrier ID 리스트 얻기 */
	if (!g_pE40PJM->GetProcessJobAllSlotNo(pj_id, arrSlotNo, arrCarrID))
		return FALSE;
	for (; i<(UINT8)arrSlotNo.GetCount(); i++)
	{
		/* Slot ID 설정 */
		strSlotID.Empty();
		strSlotID.Format(L"%s.%02d", arrCarrID[i].GetBuffer(), arrSlotNo[i]);
		/* 반환 버퍼에 등록 */
		arr_slot.Add(strSlotID);
	}

	return TRUE;
}

/*
 desc : Process Job이 Queue 상태에서 Completed 상태로 이동 (변경)하면 Project Job Id를 제거
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
 note : SetProcessJobRemove 함수와 다른점 ? Queue에서 제거하는 거지, Job List에서 제거하는 것은 아님 
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetProcessJobDequeue(PTCHAR pj_id)
{
	if (!g_pE40PJM)	return FALSE;
	if (!g_pE30GEM->IsInitedAll(TRUE))	return FALSE;
	return g_pE40PJM->SetProcessJobDequeue(pj_id);
}

/* --------------------------------------------------------------------------------------------- */
/*                         E87 Control Management Specification (CMS)                            */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ICxSubstrate for E90STS 객체 포인터 얻기
 parm : carr_id	- [in]  Carrier ID
		slot_no	- [in]  Carrier 내에 위치한 Slot Number
		subst_id- [out] Substrate ID가 반환될 버퍼
		size	- [in]  'subst_id'의 버퍼 크기
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetSubstToE87SubstrateID(PTCHAR carr_id, UINT8 slot_no,
														 PTCHAR subst_id, UINT32 size)
{
	return GetSubstToE87SubstrateID(carr_id, slot_no, subst_id, size);
}

/*
 desc : Carrier가 로드 포트 위에 도착 (내려오고;안착하고)하거나 혹은 제거되고 있는 중이다 (?)
 parm : port_id	- [in]  Load Port Number (1 or 2)
 retn : TRUE or FALSE
 note : Load Port 상태를 "Ready To Load" or "Ready To Unload"에서 "Transfer Bloacked"으로 변경 시킴
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetTransferStart(UINT16 port_id)
{
	if (!g_pE30GEM->IsInitedAll())		return FALSE;
	if (port_id < 1 || port_id > 2)	return FALSE;
	return g_pE87CMS->SetTransferStart(port_id);
}

/*
 desc : Carrier가 로드 포트 위에 안착 했다.
 parm : port_id	- [in]  Load Port Number (1 or 2)
 retn : TRUE or FALSE
 note : Load Transfer Complete (Sequnce Name)
		프로젝트 파일 (EPJ) 내에
			"UseCarrierPlaced" 변수에 값이 1로 설정되어 있다면
				CarrierLocationChanged collection event가 자동으로 트리거 되지만
			"UseCarrierPlaced" 변수에 값이 0로 설정되어 있다면
				CarrierLocationChanged collection event는 CarrierAtPort가 호출될 때 트리거 됩니다.
		이 함수의 경우 Load Port Reservation State를 변경하지 않고, CarrierAtPort가 변경 합니다.
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCarrierPlaced(UINT16 port_id)
{
	if (!g_pE30GEM->IsInitedAll())		return FALSE;
	if (port_id < 1 || port_id > 2)	return FALSE;
	return g_pE87CMS->SetCarrierPlaced(port_id);
}

/*
 desc : Carrier가 Load Port에서 제거하고자 할 때, 이 함수 호출 (상태 변경)
 parm : port_id	- [in]  Load Port Number (1 or 2)
 retn : TRUE or FALSE
 note : AccessMode가 수동 모드일 경우,
			Carrier가 Unloading 중에 undocked 및 unclamped될 때 호출해야 하고
		AccessMode가 자동 모드일 경우, CarrierUnclamped equipment constant value 값이 1일 경우,
			Carrier가 Unloading 중에 아직 Clampled이지만 undocked 일 때 호출해야 합니다.
		AccessMode가 자동 모드일 경우, CarrierUnclamped equipment constant value 값이 0일 경우,
			Carrier가 Unloading 중에 undocked 및 unclamped될 때 호출해야 합니다.
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetReadyToUnload(UINT16 port_id)
{
	if (!g_pE30GEM->IsInitedAll())		return FALSE;
	if (port_id < 1 || port_id > 2)	return FALSE;
	return g_pE87CMS->SetReadyToUnload(port_id);
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
API_EXPORT BOOL uvCIMLib_E87CMS_SetCarrierAtPort(PTCHAR carr_id, UINT16 port_id, PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())		return FALSE;
	if (port_id < 1 || port_id > 2)	return FALSE;
	return g_pE87CMS->SetCarrierAtPort(carr_id, port_id, loc_id);
}

/*
 desc : Carrier가 로드 포트에서 분리 되었다고 CIM87에 알림
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		port_id	- [in]  Load Port Number (1 or 2)
		loc_id	- [in]  Carrier Location ID 즉, 대부분 Carrier Undocked Name (Carrier Location Name)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCarrierDepartedPort(PTCHAR carr_id, UINT16 port_id, PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	if (port_id < 1 || port_id > 2)	return FALSE;
	return g_pE87CMS->SetCarrierDepartedPort(carr_id, port_id, loc_id);
}

/*
 desc : Carrier가 새로운 위치로 이동 했다고 알림
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		port_id	- [in]  Load Port Number (1 or 2)
		docked	- [in]  Carrier Docked Name (Carrier Location Name)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetMoveCarrier(PTCHAR carr_id, UINT16 port_id, PTCHAR docked)
{
	if (!g_pE30GEM->IsInitedAll())		return FALSE;
	if (port_id < 1 || port_id > 2)	return FALSE;
	return g_pE87CMS->SetMoveCarrier(carr_id, port_id, docked);
}

/*
 desc : Carrier 내에 저장 (설치)된 Slot Map에 대해 Verification 처리
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		slots	- [in]  Wafer가 Slot Map에 적재된 상태 정보
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetVerifySlotMap(PTCHAR carr_id, CAtlList <E87_CSMS> *slots, LONG handle)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetVerifySlotMap(carr_id, slots, handle);
}

/*
 desc : Carrier의 접근 상태 변경
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
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
API_EXPORT BOOL uvCIMLib_E87CMS_SetAccessCarrier(LONG conn_id, PTCHAR carr_id, UINT16 port_id,
												 E87_CCAS status, LONG handle)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetAccessCarrier(conn_id, carr_id, port_id, status, handle);
}

/*
 desc : Carrier Status Event 설정
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		port_id	- [in]  Load Port Number (1 or 2)
		status	- [in]  Carrier Event Value (E87_CCES)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCarrierEvent(LONG conn_id, PTCHAR carr_id, UINT16 port_id,
												E87_CCES status)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetCarrierEvent(conn_id, carr_id, port_id, status);
}

/*
 desc : Carrier로부터 Substrate 제거 (가져오기)
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		slot_no	- [in]  Slot Number (Not sequential number)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCarrierPopSubstrate(PTCHAR carr_id, UINT8 slot_no)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetCarrierPopSubstrate(carr_id, slot_no);
}

/*
 desc : 현재 Load Port ID의 Reservation State 정보 반환
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [out] Reservation Status 반환
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetReservationState(UINT16 port_id, E87_LPRS &state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->GetReservationState(port_id, state);
}

/*
 desc : 현재 Load Port ID의 Reservation State 정보 설정
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [in]  Reservation Status
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetReservationState(UINT16 port_id, E87_LPRS state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetReservationState(port_id, state);
}

/*
 desc : 현재 Load Port-Carrier의 Association State 정보 반환
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [out] Load Port-Carrier Association State 반환
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetAssociationState(UINT16 port_id, E87_LPAS &state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->GetAssociationState(port_id, state);
}

/*
 desc : 현재 Load Port-Carrier의 Association State 정보 설정
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [in]  Load Port-Carrier Association State
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetAssociationState(UINT16 port_id, E87_LPAS state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetAssociationState(port_id, state);
}

/*
 desc : 현재 Carrier의 Access Mode State 정보 반환
		Load Port가 수동으로 접근하는지 혹은 자동으로 접근하는지 상태 값 반환
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [out]  Manual (0) or Auto (1)
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetAccessModeState(UINT16 port_id, E87_LPAM &state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->GetAccessModeState(port_id, state);
}

/*
 desc : 현재 Carrier의 Access Mode State 값 설정
		Load Port가 수동으로 접근하는지 혹은 자동으로 접근하는지 상태 값 설정
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [in]  Manual (0) or Auto (1)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetAccessModeState(UINT16 port_id, E87_LPAM state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetAccessModeState(port_id, state);
}

/*
 desc : Carrier ID Verification State 값 반환
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		status	- [out] Carrier ID Verfication 상태 값 반환
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetCarrierIDStatus(PTCHAR carr_id, E87_CIVS &status)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->GetCarrierIDStatus(carr_id, status);
}

/*
 desc : Carrier ID Verification State 값 설정
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		state	- [in]  Carrier ID Verfication 상태 값
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCarrierIDStatus(PTCHAR carr_id, E87_CIVS status)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetCarrierIDStatus(carr_id, status);
}

/*
 desc : The slot map status of the specified carrier. <반환>
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		status	- [out] Slot map status 반환
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
 note : The slot map status shall be read on all production
		equipment prior to removal of substrates from the carrier
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetCarrierIDSlotMapStatus(PTCHAR carr_id, E87_SMVS &status)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->GetCarrierIDSlotMapStatus(carr_id, status);
}

/*
 desc : The slot map status of the specified carrier. <설정>
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		status	- [in]  Slot map status
 retn : TRUE or FALSE
 note : The slot map status shall be read on all production
		equipment prior to removal of substrates from the carrier
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCarrierIDSlotMapStatus(PTCHAR carr_id, E87_SMVS status)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetCarrierIDSlotMapStatus(carr_id, status);
}

/*
 desc : 원격 제어 허용 여부 설정
 parm : enable	- [in]  TRUE or FALSE
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetAllowRemoteControl(BOOL enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetAllowRemoteControl(enable);
}

/*
 desc : 원격 제어 허용 여부 설정
 parm : enable	- [in]  TRUE or FALSE
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetAllowRemoteControl(BOOL &enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->GetAllowRemoteControl(enable);
}

/*
 desc : ProceedWithCarrier service 초기화 (아래 2가지 목적의 경우에 호출 됩니다)
		1. Carrier ID를 Read한 후에 승인할 때, 호출됨
		2. SlotMap을 Read한 후에 승인할 때, 호출됨
		Equipment (장치)에서 직접적으로 호출하는 경우가 있을 수 있고,
		Host에서 호출하는 경우가 있을 수 있습니다.
 parm : port_id	- [in]  Load Port ID (1 or 2)
		carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetProceedWithCarrier(UINT16 port_id, PTCHAR carr_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetProceedWithCarrier(port_id, carr_id);
}

/*
 desc : CancelCarrier service 요청 (Carrier 취소)
 parm : port_id	- [in]  Load Port ID (1 or 2)
		carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCancelCarrier(UINT16 port_id, PTCHAR carr_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetCancelCarrier(port_id, carr_id);
}

/*
 desc : CancelCarrierNotification service request (Carrier 취소되었다고 알림)
 parm : carr_id	- [in]  Carrier ID (or Name; String)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelCarrierNotification service
		E_FAIL will be returned if the call fails. The "status" will contain the text of any error
		and is in the form of the S3F18 reply message. Callbacks will be called.
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCancelCarrierNotification(PTCHAR carr_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetCancelCarrierNotification(carr_id);
}

/*
 desc : Request to re-create the carrier object
 parm : port_id	- [in]  Load Port ID (1 or 2)
		carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
 retn : TRUE or FALSE
 note : The pParameterList parameter is the same format as the attribute list from the S3F17 message
		Carrier 상태가 READY_TO_UNLOAD인지 여부를 내부에서 확인 후 생성 여부 결정
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCarrierReCreate(UINT16 port_id, PTCHAR carr_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetCarrierReCreate(port_id, carr_id);
}

/*
 desc : ReleaseCarrier service 요청
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCarrierRelease(PTCHAR carr_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetCarrierRelease(carr_id);
}

/*
 desc : Carrir Location service 요청
 parm : loc_id	- [in]  Load Port 혹은 Buffer가 아닌 다른 곳을 추가하고자 하는 ID (이름. String)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetAddCarrierLocation(PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetAddCarrierLocation(loc_id);
}

/*
 desc : Load Port에 부여 (설정; 관련)된 Carrier ID (Name) 반환
 parm : port_id	- [in]  Load Port ID (1 or 2)
		carr_id	- [out] Carrier ID가 반환되어 저장될 버퍼 포인터
		size	- [in]  'carr_id' 버퍼 크기
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetAssociationCarrierID(UINT16 port_id, PTCHAR carr_id, UINT32 size,
														BOOL logs)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->GetAssociationCarrierID(port_id, carr_id, size);
}

/*
 desc : Load Port에 부여 (설정; 관련)된 Carrier ID (Name) 반환
 parm : port_id	- [in]  Load Port ID (1 or 2)
		carr_id	- [in]  Carrier ID
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetAssociationCarrierID(UINT16 port_id, PTCHAR carr_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetAssociationCarrierID(port_id, carr_id);
}

/*
 desc : Carrier ID에 부여 (설정; 관련)된 Load Port Number 반환
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		port_id	- [out] Load Port ID (1 or 2)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetAssociationPortID(PTCHAR carr_id, UINT16 &port_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->GetAssociationPortID(carr_id, port_id);
}

/*
 desc : Carrier ID에 Load Port Number 부여 (이 함수는 특별한 경우를 제외하고, 사용 권장하지 않음)
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		port_id	- [in]  Load Port ID (1 or 2)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetAssociationPortID(PTCHAR carr_id, UINT16 port_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetAssociationPortID(carr_id, port_id);
}

/*
 desc : Host로부터 요청받은 Transaction 정보 값 반환
 parm : None
 retn : 요청 받은 Transaction 정보가 저장된 전역 구조체 포인터 반환
*/
API_EXPORT LPG_CHCT uvCIMLib_E87CMS_GetTransactionInfo()
{
	return g_pSharedData->GetTransactionInfo();
}

/*
 desc : 장치(CMPS)가 Carrier Tag 읽고 난 후, Host에게 S3F30 메시지를 호출하기 위해 설정
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		tag_id	- [in]  The requested tag data, can be null if a tag is empty or fail to read
		trans_id- [in]  The transaction ID of S3F29 (Carrier Tag Read Request)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
 note : uvCIMLib_E87CMS_GetCarrierTagReadData 호출되고 난 후, Host에게 메시지를 보낼 때, 호출해야 함
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetAsyncCarrierTagReadComplete(PTCHAR carr_id, PTCHAR tag_id,
															   LONG trans_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetAsyncCarrierTagReadComplete(carr_id, tag_id, trans_id);
}

/*
 desc : Carrier로부터 읽은 Tag Data 반환
 parm : loc_id	- [in]  Location identifier (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		carr_id	- [in]  Carrier ID (or Name; String)
		data_seg- [in]  Indicates a specific section of data (Tag ID ?)
		data_tag- [out] Carrier로부터 읽어들인 Tag Data가 저장될 버퍼
		size	- [in]  'data_tag' 버퍼 크기
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
 note : loc_id or carr_id 중 1개만 설정되어 있어야 함. 즉, 둘 중에 1개는 NULL이어도 상관 없음
		CarrierTagReadData는 carrier ID tag에서 데이터 블럭을 요청하는데 사용 됩니다.
		Equipment는 LocationID와 CarrierID가 일치하지 않으면 거부할 수 있습니다.
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetCarrierTagReadData(PTCHAR loc_id, PTCHAR carr_id, PTCHAR data_seg,
													  PTCHAR data_tag, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->GetCarrierTagReadData(loc_id, carr_id, data_seg, data_tag, size);
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
API_EXPORT BOOL uvCIMLib_E87CMS_SetBeginCarrierOut(PTCHAR carr_id, UINT16 port_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetBeginCarrierOut(carr_id, port_id);
}

/*
 desc : Carrier가 임의 (특정) Load Port에서 제거 되었다고 알림
		Load Port에 Carrier가 있어야만 가능
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		port_id	- [in]  Load Port ID (1 or 2)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CarrierOut service.
		E_FAIL will be returned if the call fails.
		The "status" will contain the text of any error and
		is in the form of the S3F18 reply message. Callbacks will be called.
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCarrierOut(PTCHAR carr_id, UINT16 port_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetCarrierOut(carr_id, port_id);
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
API_EXPORT BOOL uvCIMLib_E87CMS_SetCarrierIn(PTCHAR carr_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetCarrierIn(carr_id);
}

/*
 desc : 모든 CarrierOut Service 취소
		This service shall cause all CarrierOut services to be removed from the queue.
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelAllCarrierOut service.
		E_FAIL will be returned if the call fails.
		The "status" will contain the text of any error and is in the form of the S3F18 reply message.
		Callbacks will be called.
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCancelAllCarrierOut()
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetCancelAllCarrierOut();
}

/*
 desc : Load Port의 Transfer Status 값 반환
 parm : port_id	- [in]  Load Port ID (1 or 2)
		status	- [out] Load Port의 Transfer Status 값 반환
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
 note : Get the current load port transfer state
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetTransferStatus(UINT16 port_id, E87_LPTS &status)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->GetTransferStatus(port_id, status);
}

/*
 desc : Load Port의 Transfer Status 값 설정
 parm : port_id	- [in]  Load Port ID (1 or 2)
		status	- [in]  Load Port의 Transfer Status 값
 retn : TRUE or FALSE
 note : Sets the Load Port Transfer State for the specified load port.
		The new value can be any state and does not depend on the current state
		!!! important. 수동으로 상태 변환을 해줘야지, 내부적으로 알아서 변환해 주지 않음
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetTransferStatus(UINT16 port_id, E87_LPTS status)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetTransferStatus(port_id, status);
}

/*
 desc : CancelBind service Request
		Carrier ID와 Load Port 연결 부분을 취소하고 Load Port가 NOT_RESERVED 상태로 전환
 parm : port_id	- [in]  Load Port ID (1 or 2) (-1 or 0 이상 값)
		carr_id	- [in]  Carrier ID (or Name; String) (NULL or String)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelBind service
		port_id 혹은 carr_id 둘 중의 하나의 값만 반드시 입력되어야 됨
		Either portID or carrierID is required, but not both
		Set portID to -1 if it is not used and carrierID to an empty string if it is not used
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCancelBind(UINT16 port_id, PTCHAR carr_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetCancelBind(port_id, carr_id);
}

/*
 desc : CancelCarrierOut service Request
		현재 Carrier에 관련된 Action을 취소하고,
		장비 (Equipment)가 Carrier를 Load Port의 Unload (혹은 내부 버퍼) 위치로 복구 합니다.
 parm : port_id	- [in]  Load Port ID (1 or 2)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelCarrierAtPort service
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCancelCarrierAtPort(UINT16 port_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetCancelCarrierAtPort(port_id);
}

/*
 desc : CancelCarrierOut service Request
		현재 Carrier에 관련된 CarrierOut 서비스만 Queue에서 제거
 parm : carr_id	- [in]  Carrier ID (or Name; String)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelCarrierOut service
		The "status" will contain the text of any error and is in the form of the S3F18 reply message
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCancelCarrierOut(PTCHAR carr_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetCancelCarrierOut(carr_id);
}

/*
 desc : CancelReserveAtPort service Request
		지정된 Load Port에서 Reservation (상태?)를 제거하고 visible signal을 비활성화 시킴
 parm : port_id	- [in]  Load Port ID (1 or 2)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelReserveAtPort service
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCancelReserveAtPort(UINT16 port_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetCancelReserveAtPort(port_id);
}

/*
 desc : 장비 (Equipment)에서 현재 Carrier의 접근 상태 값 설정
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		status	- [in]  Accessing Status (E87_CCAS)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelReserveAtPort service
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCarrierAccessingStatus(PTCHAR carr_id, E87_CCAS status)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetCarrierAccessingStatus(carr_id, status);
}

/*
 desc : 장비 (Equipment)에서 현재 Carrier의 접근 상태 값 얻기
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		status	- [out] Accessing Status (E87_CCAS)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelReserveAtPort service
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetCarrierAccessingStatus(PTCHAR carr_id, E87_CCAS &status)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->GetCarrierAccessingStatus(carr_id, status);
}

/*
 desc : 현재 Carrier ID 상태가 Closed 상태인지 여부
 parm : carr_id	- [in]  Carrier ID (or Name; String)
 retn : TRUE (Closed), FALSE (Not closed)
*/
API_EXPORT BOOL uvCIMLib_E87CMS_IsCarrierAccessClosed(PTCHAR carr_id)
{
	E87_CCAS enStatus	= E87_CCAS::en_carrier_not_accessed;

	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	if (!g_pE87CMS->GetCarrierAccessingStatus(carr_id, enStatus))	return FALSE;

	switch (enStatus)
	{
	case E87_CCAS::en_carrier_not_accessed	:
	case E87_CCAS::en_carrier_in_access		:
	case E87_CCAS::en_carrier_stopped		:	return FALSE;
	}
	return TRUE;
}

/*
 desc : Carrier (FOUP) 내의 특정 위치에 Substrate (Wafer)를 놓기
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		slot_no	- [in]  Slot Number (Not sequential number)
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetSubstratePlaced(PTCHAR carr_id, PTCHAR subst_id, UINT8 slot_no)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetSubstratePlaced(carr_id, subst_id, slot_no);
}

/*
 desc : Recipe (생성, 수정, 삭제) 이벤트 처리
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		recipe	- [in]  Recipe Name
		type	- [in]  0x01 (Created), 0x02 (Edited), 0x03 (Deleted)
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetRecipeManagement(LONG conn_id, PTCHAR recipe, UINT8 type,
													LONG handle)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetRecipeManagement(conn_id, recipe,type, handle);
}

/*
 desc : Recipe (Selected) 이벤트 처리
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		recipe	- [in]  Recipe Name
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetRecipeSelected(LONG conn_id, PTCHAR recipe, LONG handle)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetRecipeSelected(conn_id, recipe, handle);
}

/* --------------------------------------------------------------------------------------------- */
/*                        E90 Substrate Tracking Specification (CJM)                             */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Substrate (Transport & Processing) 상태 값 변경
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		loc_id	- [in]  Location ID (Name)가 저장된 문자열 버퍼 or 1-Based Index
		trans	- [in]  Substrate transport state
		proc	- [in]  Substrate processing state
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetChangeSubstrateStateName(PTCHAR subst_id, PTCHAR loc_id,
															E90_SSTS trans, E90_SSPS proc)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetChangeSubstrateState(subst_id, loc_id, trans, proc);
}
API_EXPORT BOOL uvCIMLib_E90STS_SetChangeSubstrateStateIndex(PTCHAR subst_id, ENG_TSSI loc_id,
															 E90_SSTS trans, E90_SSPS proc)
{
	PTCHAR ptzLocName	= NULL;
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	ptzLocName	= g_pE30GEM->GetEquipLocationName(loc_id);
	return g_pE90STS->SetChangeSubstrateState(subst_id, ptzLocName, trans, proc);
}

/*
 desc : Substrate (Transport & Processing) 상태 값 변경
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		loc_id	- [in]  Location ID (Name)가 저장된 문자열 버퍼 or 1-Based Index (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
						Substrate Location ID_xx (문자열. RobotArm / PreAligner / ProcessChamber / Buffer)
		trans	- [in]  Substrate transport state
		proc	- [in]  Substrate processing state
		status	- [in]  Substrate ID state E90_SISM
		time_in	- [in]  Batch가 Location ID에 도착하는 시간 (NULL이면 사용 안함)
		time_out- [in]  Batch가 Location ID에 이탈하는 시간 (NULL이면 사용 안함)
						time format : YYYYMMDDhhmmsscc for YearMonthDayHourMinuteSecondCentisecond
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetChangeSubstrateStateName2(PTCHAR subst_id, PTCHAR loc_id,
															 E90_SSTS trans, E90_SSPS proc,
															 E90_SISM status,
															 PTCHAR time_in, PTCHAR time_out)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetChangeSubstrateState2(subst_id, loc_id,
											   trans, proc, status, time_in, time_out);
}
API_EXPORT BOOL uvCIMLib_E90STS_SetChangeSubstrateStateIndex2(PTCHAR subst_id, ENG_TSSI loc_id,
															  E90_SSTS trans, E90_SSPS proc,
															  E90_SISM status,
															  PTCHAR time_in, PTCHAR time_out)
{
	PTCHAR ptzLocName	= NULL;
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	ptzLocName	= g_pE30GEM->GetEquipLocationName(loc_id);
	return g_pE90STS->SetChangeSubstrateState2(subst_id, ptzLocName,
											   trans, proc, status, time_in, time_out);
}

/*
 desc : Substrate Processing 상태 값 변경
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		proc	- [in]  Substrate processing state
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetChangeSubstrateProcessingState(PTCHAR subst_id, E90_SSPS proc)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetChangeSubstrateProcessingState(subst_id, proc);
}

/*
 desc : Substrate 생성
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		sub_id	- [in]  Location Group ID를 위해 사용하는 Subscript Index (location's index)
						EPJ 파일에서 해당 위치에 사용되는 첨자?(Subscripted) 변수의 세트 (set)
						-> EPJ 파일에 정의된 내용과 연관성이 있을듯... (사용하지 않을 때는 0 값)
		lot_id	- [in]  Lot ID 즉, 사용자에 의해 기판에 할당된 LOT의 Index 값 (NULL이면 사용 안함)
 retn : TRUE or FALSE
 note : Substrate 생성은 보통 CIM87에 의해 생성 (Carrier slotmap이 검증될 때)되지만,
		몇몇 다른 방식으로 생성할 필요가 있을 때, 이 함수를 호출하여 생성
*/
API_EXPORT BOOL uvCIMLib_E90STS_RegisterSubstrateName(PTCHAR subst_id, PTCHAR loc_id, INT16 sub_id, PTCHAR lot_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->RegisterSubstrate(subst_id, loc_id, sub_id, lot_id);
}
API_EXPORT BOOL uvCIMLib_E90STS_RegisterSubstrateIndex(PTCHAR subst_id, ENG_TSSI loc_id, INT16 sub_id, PTCHAR lot_id)
{
	PTCHAR ptzLocID	= NULL;
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	ptzLocID	= g_pE30GEM->GetEquipLocationName(loc_id);
	return g_pE90STS->RegisterSubstrate(subst_id, ptzLocID, sub_id, lot_id);
}

/*
 desc : Substrate ID 제거
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetRemoveSubstrate(PTCHAR subst_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->RemoveSubstrate(subst_id);
}

/*
 desc : Substrate Location (Wafer가 놓여질 위치 이름) 등록
 parm :	loc_id	- [in]  Location ID (Name)가 저장된 문자열 버퍼
						Substrate Location ID_xx (문자열. RobotArm / PreAligner / ProcessChamber / Buffer)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetAddSubstrateLocation(PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->AddSubstrateLocation(loc_id);
}

/*
 desc : Substrate Location (Wafer가 놓여질 위치 이름) 제거
 parm :	loc_id	- [in]  Location ID (Name)가 저장된 문자열 버퍼
						Substrate Location ID_xx
						(문자열. RobotArm / PreAligner / ProcessChamber / Buffer)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetRemoveSubstrateLocation(PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->RemoveSubstrateLocation(loc_id);
}

/*
 desc : Substrate (Wafer)를 승인 하도록 (받도록) 장비 쪽을 호출
		장비 쪽에서 내부적으로 E90CommandCallback 함수가 호출됨
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
 retn : TRUE or FALSE
 note : Request the equipment to accept the substrate.
		This function executes the E90CommandCallback() to allow the equipment to reject the command.
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetProceedWithSubstrate(PTCHAR subst_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetProceedWithSubstrate(subst_id);
}

/*
 desc : Remote Control 허용 여부 값 반환
 parm : enable	- [out] Enable or Disable
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetAllowRemoteControl(BOOL &enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetAllowRemoteControl(enable);
}

/*
 desc : Remote Control 허용 여부 설정
 parm : enable	- [in]  Enable or Disable
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetAllowRemoteControl(BOOL enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetAllowRemoteControl(enable);
}

/*
 desc : Substrate Location ID의 상태 값 반환
 parm : loc_id	- [in]  Location ID (Name)가 저장된 문자열 버퍼 or 1-Based Index (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
						Substrate Location ID_xx (문자열. RobotArm / PreAligner / ProcessChamber / Buffer)
		state	- [out] Substrate Location State 값 반환
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateLocationStateName(PTCHAR loc_id, E90_SSLS &state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateLocationState(loc_id, state);
}
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateLocationStateIndex(ENG_TSSI loc_id, E90_SSLS &state)
{
	PTCHAR ptzLocName	= NULL;
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	ptzLocName	= g_pE30GEM->GetEquipLocationName(loc_id);
	return g_pE90STS->GetSubstrateLocationState(ptzLocName, state);
}

/*
 desc : Substrate Location ID의 상태 값 설정
 parm : loc_id	- [in]  Location ID (Name)가 저장된 문자열 버퍼 or 1-Based Index
						Substrate Location ID_xx (문자열. RobotArm / PreAligner / ProcessChamber / Buffer)
		state	- [in]  Substrate Location State 값 반환 (E90_SSLS)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateLocationStateName(PTCHAR loc_id, E90_SSLS state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSubstrateLocationState(loc_id, state);
}
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI loc_id, E90_SSLS state)
{
	PTCHAR ptzLocName	= NULL;
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	ptzLocName	= g_pE30GEM->GetEquipLocationName(loc_id);
	return g_pE90STS->SetSubstrateLocationState(ptzLocName, state);
}

/*
 desc : Substrate Processing의 이전 상태 값 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		state	- [out] Substrate Location State 값 반환 (E90_SSPS)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstratePrevProcessingState(PTCHAR subst_id, E90_SSPS &state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstratePrevProcessingState(subst_id, state);
}

/*
 desc : Substrate Processing 상태 값 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		state	- [out] Substrate Location State 값 반환 (E90_SSPS)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateProcessingState(PTCHAR subst_id, E90_SSPS &state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateProcessingState(subst_id, state);
}

/*
 desc : Substrate Processing 상태 값 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		state	- [in]  설정하고자 하는 상태 값 (E90_SSPS)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateProcessingState(PTCHAR subst_id, E90_SSPS state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSubstrateProcessingState(subst_id, state);
}

/*
 desc : Substrate Transport 이전 상태 값 반환
 parm : subst_id- [in]  Substrate ID (문자열)
		state	- [out] Substrate Transport State 값 반환 (E90_SSTS)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstratePrevTransportState(PTCHAR subst_id, E90_SSTS &state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstratePrevTransportState(subst_id, state);
}

/*
 desc : Substrate Transport 상태 값 반환
 parm : subst_id- [in]  Location ID (Name)가 저장된 문자열 버퍼 or 1-Based Index (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
						Substrate Location ID_xx (문자열. RobotArm / PreAligner / ProcessChamber / Buffer)
		state	- [out] Substrate Location State 값 반환 (E90_SSPS)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateTransportStateName(PTCHAR subst_id, E90_SSTS &state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateTransportState(subst_id, state);
}
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateTransportStateIndex(ENG_TSSI subst_id, E90_SSTS &state)
{
	PTCHAR ptzLocName	= NULL;
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	ptzLocName	= g_pE30GEM->GetEquipLocationName(subst_id);
	return g_pE90STS->GetSubstrateTransportState(ptzLocName, state);
}

/*
 desc : Substrate Transport 상태 값 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		state	- [in]  설정하고자 하는 상태 값 (E90_SSTS)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateTransportState(PTCHAR subst_id, E90_SSTS state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSubstrateTransportState(subst_id, state);
}

/*
 desc : Substrate ID (Tag ? Serial ? Bar Code)의 이전 상태 값 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		status	- [out] Enumeration values for the ID Reader state machine
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstratePrevIDStatus(PTCHAR subst_id, E90_SISM &state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstratePrevIDStatus(subst_id, state);
}

/*
 desc : Substrate ID (Tag ? Serial ? Bar Code) 읽어진 값 상태 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		status	- [out] Enumeration values for the ID Reader state machine
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateIDStatus(PTCHAR subst_id, E90_SISM &status)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateIDStatus(subst_id, status);
}

/*
 desc : Substrate ID (Tag ? Serial ? Bar Code) 읽어진 값 상태 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		status	- [in]  Enumeration values for the ID Reader state machine
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateIDStatus(PTCHAR subst_id, E90_SISM status)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSubstrateIDStatus(subst_id, status);
}

/*
 desc : CIM300 (Host)에게 Substrate ID (Tag ? Serial ? Bar Code) 읽어진 값 상태를 판단해 달라고 요청
 parm : subst_id	- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		acquired_id	- [in]  Substrate ID acquired from the substrate ID reader (Serial? Tag?)
		read_good	- [in]  TRUE if read was successful. Otherwise FALSE
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetNotifySubstrateRead(PTCHAR subst_id, PTCHAR acquired_id,
													   BOOL read_good)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->NotifySubstrateRead(subst_id, acquired_id, read_good);
}

/*
 desc : Substrate ID Reader가 판독 가능 혹은 불가능 할 때, 이벤트 발생 시킴
 parm : enable	- [in]  Substrate ID Reader가 사용 가능한지 여부 설정
						TRUE: 사용 가능할 때, 이벤트 알림
						FALSE:사용 불가능할 때, 이벤트 알림
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateReaderAvailable(INT32 enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SubstrateReaderAvailable(enable);
}

/*
 desc : 장비에게 현재 substrate를 건너띄고, 소스 혹은 다른 위치로 이동하라고 요청 함 (원래 위치로 이동하라고 함)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
 retn : TRUE or FALSE
 note : This function executes the E90CommandCallback() to allow the equipment to reject the command
		즉, 이 함수를 호출하면, 장비 내에서 거부될 경우, E90CommandCallback 함수가 호출 됨
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetCancelSubstrate(PTCHAR subst_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->CancelSubstrate(subst_id);
}

/*
 desc : 실제 Substrate ID에서 읽어들인 Wafer Reader Code (Tag, Serial, Bar Code, etc) 값을 반환
 parm : subst_id	- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		acquired_id	- [out] Wafer Reader로부터 읽어들인 ID 값 반환 (문자열)
		size		- [in]  'acquired_id' 버퍼 크기
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateIDToAcquiredID(PTCHAR subst_id, PTCHAR acquired_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateIDToAcquiredID(subst_id, acquired_id, size);
}

/*
 desc : 실제 Substrate ID에서 읽어들인 Wafer Reader Code (Tag, Serial, Bar Code, etc) 값 설정
 parm : subst_id	- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		acquired_id	- [in]  Wafer Reader로부터 읽어들인 ID 값 (문자열)
 retn : TRUE or FALSE
 note : Contains the ID read from the substrate. Empty string before the substrate is read.
		The attribute shall be updated as soon the substrate ID has been successfully read
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateIDToAcquiredID(PTCHAR subst_id, PTCHAR acquired_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSubstrateIDToAcquiredID(subst_id, acquired_id);
}

/*
 desc : Substrate ID 값 반환 (Slot Number; Slot No)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		slot_no	- [out] Slot Number 값이 반환되어 저장 (1-based or Later. Max. 255)	
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateIDToSlotNo(PTCHAR subst_id, UINT8 &slot_no)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateIDToSlotNo(subst_id, slot_no);
}

/*
 desc : Substrate Location ID 값 반환 (Slot Number; Slot No)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		loc_id	- [out] Location ID (Buffer, Robot, PreAligner, Chamber, etc)가 저장될 버퍼
		size	- [in]  'loc_id' 버퍼 크기
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateLocationID(PTCHAR subst_id, PTCHAR loc_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateLocationID(subst_id, loc_id, size);
}

/*
 desc : 해당 위치에 웨이퍼가 존재하는지 여부
 parm : loc_id	- [in]  Location ID (Buffer, Robot, PreAligner, Chamber, LoadPort1, LoadPort2)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_IsLocationSubstrateID(ENG_TSSI loc_id)
{
	PTCHAR ptzLocID = NULL;

	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	ptzLocID = g_pE30GEM->GetEquipLocationName(loc_id);

	return g_pE90STS->IsLocationSubstrateID(ptzLocID);
}

/*
 desc : 특정 wafer (substrate ID; st_id)를 임의 location에 생성과 동시에 각종 상태 값 지정
 parm : subst_id	- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		loc_id		- [in]  Location ID (문자열)
		proc_state	- [in]  Processing State (E90_SSPS)
		trans_state	- [in]  Transfer State (E90_SSTS)
		read_status	- [in]  ID Read Status (E90_SISM)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateAnyLocStates(PTCHAR subst_id, PTCHAR loc_id,
														 E90_SISM read_status, E90_SSPS proc_state,
														 E90_SSTS trans_state)
{
	HRESULT hResult				= NULL;
	ICxSubstratePtr pICxSubSt	= NULL;

	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	if (!g_pE90STS->GetSubstratePtr(subst_id, pICxSubSt))	return FALSE;

	/* Change location */
	hResult	= pICxSubSt->put_locationID(CComBSTR(loc_id));
	if (g_pLogData->CheckResult(hResult, L"uvCIMLib_E90STS_SetSubstrateAnyLocStates::put_locationID"))
		return FALSE;

	/* Chagne status */
	if (!g_pE90STS->SetSubstrateIDStatus(subst_id, read_status))		return FALSE;
	if (!g_pE90STS->SetSubstrateProcessingState(subst_id, proc_state))	return FALSE;
	if (!g_pE90STS->SetSubstrateTransportState(subst_id, trans_state))	return FALSE;

	return TRUE;
}

/*
 desc : 현재 Wafer의 세트 (Lot; FOUP)가 놓여진 객체의 위치 정보를 반환 한다
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		index	- [out] Wafer의 세트 (Lot; FOUP)가 놓여진 위치 (인덱스) 값
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetBatchLocIdx(PTCHAR loc_id, INT16 &index)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetBatchLocIdx(loc_id, index);
}

/*
 desc : 현재 Wafer의 세트 (Lot; FOUP)가 놓여진 객체의 위치 정보를 반환 한다
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		index	- [out] Wafer의 세트 (Lot; FOUP)가 놓여진 위치 (인덱스) 값
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetBatchLocIdx(PTCHAR loc_id, INT16 index)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetBatchLocIdx(loc_id, index);
}

/*
 desc : 현재 Wafer의 세트 (Lot; FOUP)가 놓여진 객체의 점유 상태 여부
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_IsBatchLocOccupied(PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->IsBatchLocOccupied(loc_id);
}

/*
 desc : 현재 Wafer의 세트 (Lot; FOUP)가 놓여진 객체의 점유 상태 여부
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		state	- [in]  Batch State 값 즉, 점유 (1), 점유 안함 (0)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetBatchLocState(PTCHAR loc_id, E90_SSLS state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetBatchLocState(loc_id, state);
}

/*
 desc : 현재 Wafer의 세트 (Lot; FOUP)에 저장(등록)된 Substrate ID 목록 반환
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [out] loc_id에 위치한 Wafer의 세트 (Lot; FOUP)의 Substrate ID 목록이 저장될 리스트 버퍼
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetBatchSubstIDMap(PTCHAR loc_id, CStringArray &subst_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetBatchSubstIDMap(loc_id, subst_id);
}

/*
 desc : 현재 Wafer의 세트 (Lot; FOUP)에 저장(등록)된 Substrate ID 목록 저장 (등록)
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [in]  loc_id에 위치한 Wafer의 세트 (Lot; FOUP)의 Substrate ID 목록이 저장된 리스트 버퍼
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetBatchSubstIDMap(PTCHAR loc_id, CStringArray *subst_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetBatchSubstIDMap(loc_id, subst_id);
}

/*
 desc : Location ID에 대한 이벤트를 보낼지 여부에 대한 정보 얻기
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		flag	- [out] FALSE : 이벤트 보냄 (알림), TRUE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetDisableEventsLoc(PTCHAR loc_id, BOOL &flag)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetDisableEventsLoc(loc_id, flag);
}

/*
 desc : Location ID에 대한 이벤트를 보낼지 여부에 대한 정보 설정
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		flag	- [out] FALSE : 이벤트 보냄 (알림), TRUE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetDisableEventsLoc(PTCHAR loc_id, BOOL flag)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetDisableEventsLoc(loc_id, flag);
}

/*
 desc : Batch ID에 대한 이벤트를 보낼지 여부에 대한 정보 얻기
 parm : batch_id- [in]  ID of substrate batch
		flag	- [out] FALSE : 이벤트 보냄 (알림), TRUE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetDisableEventsBatch(PTCHAR batch_id, BOOL &flag)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetDisableEventsBatch(batch_id, flag);
}

/*
 desc : Batch ID에 대한 이벤트를 보낼지 여부에 대한 정보 설정
 parm : batch_id- [in]  ID of substrate batch
		flag	- [out] FALSE : 이벤트 보냄 (알림), TRUE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetDisableEventsBatch(PTCHAR batch_id, BOOL flag)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetDisableEventsBatch(batch_id, flag);
}

/*
 desc : Batch (Wafer Collections)에 등록할 수 있는 최대 개수 반환
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		max_pos	- [in]  최대 등록 가능한 Substrate position 값이 반환될 버퍼
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetMaxSubstratePosition(PTCHAR loc_id, LONG &max_pos)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetMaxSubstratePosition(loc_id, max_pos);
}

/*
 desc : Batch (Wafer Collections)에 등록할 수 있는 최대 개수 반환
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		name	- [out] Object Name이 저장될 버퍼
		size	- [in]  'name' 버퍼의 크기
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetObjectName(PTCHAR loc_id, PTCHAR name, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetObjectName(loc_id, name, size);
}

/*
 desc : Update <batch location> GEM data (Batch Location (위치) GEM Data 갱신을 알림?)
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
 note : Updates the batch location status variables
*/
API_EXPORT BOOL uvCIMLib_E90STS_SendDataNotificationBatch(PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SendDataNotificationBatch(loc_id);
}

/*
 desc : Update <substrate location> GEM variables for this object
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
 note : GEM 변수 - SubstID, SubstLocID, SubstLocState, SubstLocState_i, and SubstLocSubstID_i
*/
API_EXPORT BOOL uvCIMLib_E90STS_SendDataNotificationSubst(PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SendDataNotificationSubst(loc_id);
}

/*
 desc : Substrate Batch Location ID 생성
 parm : loc_id	- [in]  생성하고자 하는 Batch Location ID (문자열)
		loc_num	- [in]  Batch Location에서 substrate locations의 개수 (Carrier 내에 등록? 가능한 substrate location 개수)
		sub_id	- [in]  Location Group ID를 위해 사용하는 Subscript Index (location's index)
						EPJ 파일에서 해당 위치에 사용되는 첨자?(Subscripted) 변수의 세트 (set) -> EPJ 파일에 정의된 내용과 연관성이 있을듯...
						사용하지 않을 때는 0 값
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_AddSubstrateBatchLocation(PTCHAR loc_id, INT32 loc_num, INT16 sub_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->AddSubstrateBatchLocation(loc_id, loc_num, sub_id);
}

/*
 desc : Substrate Batch Location ID 제거
 parm : loc_id	- [in]  생성하고자 하는 Batch Location ID (문자열)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_DelSubstrateBatchLocation(PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->DelSubstrateBatchLocation(loc_id);
}

/*
 desc : Batch에 캐리어의 기판을 추가 함
 parm : batch_id- [in]  ID of substrate batch
		carr_id	- [in]  ID of carrier
		slot_map- [in]  1 or 0 or -1
						TRUE : 각 Slot map item = 0에 대해 위치 값을 비워서 추가 함
							   즉, 각 Slot의 위치 값을 초기화 후 추가하도록 처리
							   (사용하지 않을 때는 -1)
 retn : TRUE or FALSE
 note : Batch	- 프로세스 자원에서 동시에 처리할 기판의 그룹 (뭉치?)
*/
API_EXPORT BOOL uvCIMLib_E90STS_AddCarrierToBatch(LONG batch_id, PTCHAR carr_id, LONG slot_map)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->AddCarrierToBatch(batch_id, carr_id, slot_map);
}

/*
 desc : Batch에 캐리어의 기판을 제거 함
 parm : batch_id- [in]  ID of substrate batch
		carr_id	- [in]  ID of carrier
 retn : TRUE or FALSE
 note : Batch	- 프로세스 자원에서 동시에 처리할 기판의 그룹 (뭉치?)
*/
API_EXPORT BOOL uvCIMLib_E90STS_DelCarrierFromBatch(LONG batch_id, PTCHAR carr_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->DelCarrierFromBatch(batch_id, carr_id);
}

/*
 desc : 새로운 Substrate Batch 생성
 parm : batch_id- [in]  ID of substrate batch
		loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_CreateBatch(LONG batch_id, PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->CreateBatch(batch_id, loc_id);
}

/*
 desc : 기존 Substrate Batch 제거
 parm : batch_id- [in]  ID of substrate batch
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_DestroyBatch(LONG batch_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->DestroyBatch(batch_id);
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
API_EXPORT BOOL uvCIMLib_E90STS_ChangeBatchState(LONG batch_id, PTCHAR loc_id,
												 E90_SSTS trans_state, E90_SSPS proc_state,
												 PTCHAR time_in, PTCHAR time_out)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->ChangeBatchState(batch_id, loc_id, trans_state, proc_state, time_in, time_out);
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
*/
API_EXPORT BOOL uvCIMLib_E90STS_AddSubstrateLocation(PTCHAR loc_id, PTCHAR subst_id, INT16 sub_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->AddSubstrateLocation(loc_id, subst_id, sub_id);
}

/*
 desc : Substrate Location 제거
 parm : loc_id	- [in]  위치 변경하고자 하는 ID (Location ID: Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_DelSubstrateLocation(PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->DelSubstrateLocation(loc_id);
}

/*
 desc : Batch에 Substrate 추가
 parm : batch_id- [in]  ID of substrate batch
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
 retn : TRUE or FALSE
 note : Batch	- 프로세스 자원에서 동시에 처리할 기판의 그룹 (뭉치?)
*/
API_EXPORT BOOL uvCIMLib_E90STS_AddSubstrateToBatch(LONG batch_id, PTCHAR subst_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->AddSubstrateToBatch(batch_id, subst_id);
}

/*
 desc : Batch에서 Substrate 제거
 parm : batch_id- [in]  ID of substrate batch
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
 retn : TRUE or FALSE
 note : Batch	- 프로세스 자원에서 동시에 처리할 기판의 그룹 (뭉치?)
*/
API_EXPORT BOOL uvCIMLib_E90STS_DelSubstrateFromBatch(LONG batch_id, PTCHAR subst_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->DelSubstrateFromBatch(batch_id, subst_id);
}

/*
 desc : Batch에 비어있는 substrate 공간을 추가 즉, Substrate가 없는 Batch만 추가
 parm : batch_id- [in]  ID of substrate batch
 retn : TRUE or FALSE
 note : Batch	- 프로세스 자원에서 동시에 처리할 기판의 그룹 (뭉치?)
*/
API_EXPORT BOOL uvCIMLib_E90STS_AddEmptySpaceToBatch(LONG batch_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->AddEmptySpaceToBatch(batch_id);
}

/*
 desc : subscript indices의 범위를 예약 (?)
 parm : base_sub_index	- [in]  first subscript index to reserve
		reserve_size	- [in]  number of indices to reserve
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_ReserveSubstrateLocationIndices(INT16 base_sub_index,
																INT16 reserve_size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->ReserveSubstrateLocationIndices(base_sub_index, reserve_size);
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
*/
API_EXPORT BOOL uvCIMLib_E90STS_ChangeSubstrateTransportState(PTCHAR subst_id, PTCHAR loc_id,
															  E90_SSTS state,
															  PTCHAR time_in, PTCHAR time_out)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->ChangeSubstrateTransportState(subst_id, loc_id, state, time_in, time_out);
}

/*
 desc : Substrate Material Status 값 반환 (기판의 처리 상태 값 반환. 처리 여부 ?)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		status	- [out] Material Status (Material 처리 여부) 값이 반환될 참조 변수
						장치마다 반환되는 값이 다르다? (EPJ 파일 참조?)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateMaterialStatus(PTCHAR subst_id, LONG &status)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateMaterialStatus(subst_id, status);
}

/*
 desc : Substrate Material Status 값 변경 (기판의 처리 상태 값 변경. 처리 여부 ?)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		status	- [in]  Material Status (Material 처리 여부) 값
						장치마다 설정하는는 값이 다르다? (EPJ 파일 참조?)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateMaterialStatus(PTCHAR subst_id, LONG status)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSubstrateMaterialStatus(subst_id, status);
}

/*
 desc : 장비에 다루는 Substrate의 Type 설정 (Wafer, Flat Panel, CD, Mask, etc...)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		type	- [in]  E90_EMST 즉, wafer, flat_panel, cd, mask 중 하나
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetCurrentSubstrateType(PTCHAR subst_id, E90_EMST type)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetCurrentSubstrateType(subst_id, type);
}

/*
 desc : 장비에 다루는 Substrate의 Type 설정 (Wafer, Flat Panel, CD, Mask, etc...)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		type	- [out] E90_EMST 즉, wafer, flat_panel, cd, mask 중 하나
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetCurrentSubstrateType(PTCHAR subst_id, E90_EMST &type)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetCurrentSubstrateType(subst_id, type);
}

/*
 desc : Substrate ID가 Reader기로부터 성공적으로 읽어졌는지 즉, Host로부터 Confirm을 받았는지 여부
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		read	- [out] 정상적으로 ID를 읽었는지 여부 값 (TRUE or FALSE)이 반환될 참조 변수
 note : Equipment에서는 NotifySubstrateRead 함수 호출해서 Substrate ID가 정상인지를 Host에게 요청
		이 NotifySubstrateRead 함수 호출 후에, 현재 함수 호출하면 goodRead인지 badRead인지 알 수 있음
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateGoodRead(PTCHAR subst_id, BOOL &read)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateGoodRead(subst_id, read);
}

/*
 desc : Substrate ID가 Reader기로부터 읽어들인 상태 값 즉, TRUE (good) or FALSE (bad) 값 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		read	- [in]  정상적으로 ID를 읽었다면 TRUE, 아니라면 FALSE 설정
 note : Equipment에서는 NotifySubstrateRead 함수 호출해서 Substrate ID가 정상인지를 Host에게 요청
		이 NotifySubstrateRead 함수 호출 후에, 현재 함수 호출하면 goodRead인지 badRead인지 알 수 있음
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateGoodRead(PTCHAR subst_id, BOOL read)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSubstrateGoodRead(subst_id, read);
}

/*
 desc : Substrate ID와 관련된 Lot ID 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		lot_id	- [out] Lot ID가 저장될 버퍼
		size	- [in]  'lot_id' 버퍼 크기
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateLotID(PTCHAR subst_id, PTCHAR lot_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateLotID(subst_id, lot_id, size);
}

/*
 desc : Substrate ID와 관련된 Lot ID 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		lot_id	- [in]  Lot ID가 저장된 버퍼
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateLotID(PTCHAR subst_id, PTCHAR lot_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSubstrateLotID(subst_id, lot_id);
}

/*
 desc : Substrate ID와 관련된 Object ID 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		obj_id	- [out] Object ID가 저장될 버퍼 (여기서는 Substrate ID와 동일한 텍스트?)
		size	- [in]  'lot_id' 버퍼 크기
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateObjID(PTCHAR subst_id, PTCHAR obj_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateObjID(subst_id, obj_id, size);
}

/*
 desc : Substrate ID와 관련된 Object Type 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		obj_type- [out] Object Type이 저장될 버퍼 ("BatchLoc" or "Substrate" or "SubstLoc")
						여기서는 보통 "Substrate"
		size	- [in]  'obj_type' 버퍼 크기
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateObjType(PTCHAR subst_id, PTCHAR obj_type, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateObjType(subst_id, obj_type, size);
}

/*
 desc : Location ID와 관련된 Object ID 반환
 parm : loc_id	- [out] Location ID가 저장될 버퍼
		obj_id	- [out] Object ID가 저장될 버퍼 (여기서는 Location ID와 동일한 텍스트?)
		size	- [in]  'lot_id' 버퍼 크기
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetLocationObjID(PTCHAR loc_id, PTCHAR obj_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetLocationObjID(loc_id, obj_id, size);
}

/*
 desc : Location ID와 관련된 Object Type 반환
 parm : loc_id	- [out] Location ID가 저장될 버퍼
		obj_type- [out] Object Type이 저장될 버퍼 ("BatchLoc" or "Substrate" or "SubstLoc")
						여기서는 보통 "Substrate"
		size	- [in]  'obj_type' 버퍼 크기
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetLocationObjType(PTCHAR loc_id, PTCHAR obj_type, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetLocationObjType(loc_id, obj_type, size);
}

/*
 desc : Substrate가 속한 Source CarrierID 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		carr_id	- [out] sourceCarrierID가 저장될 버퍼
		size	- [in]  'carr_id' 버퍼 크기
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSourceCarrierID(PTCHAR subst_id, PTCHAR carr_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSourceCarrierID(subst_id, carr_id, size);
}

/*
 desc : Substrate가 속할 Source CarrierID 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		carr_id	- [in]  sourceCarrierID가 저장될 버퍼
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSourceCarrierID(PTCHAR subst_id, PTCHAR carr_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSourceCarrierID(subst_id, carr_id);
}

/*
 desc : Substrate가 속한 Source LoadPort ID 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		port_id	- [out] Load Port ID가 저장될 참조 변수 (1 or Later)
 retn : TRUE or FALSE
 note : Load Port - the interface location on the equipment where carriers are loaded and unloaded
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSourceLoadPortID(PTCHAR subst_id, LONG &port_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSourceLoadPortID(subst_id, port_id);
}

/*
 desc : Substrate가 속할 Source LoadPort ID 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		port_id	- [in]  sourcLoadPortID가 저장될 버퍼 (1 or Later)
 retn : TRUE or FALSE
 note : Load Port - the interface location on the equipment where carriers are loaded and unloaded
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSourceLoadPortID(PTCHAR subst_id, LONG port_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSourceLoadPortID(subst_id, port_id);
}

/*
 desc : Substrate가 속한 Source Slot ID 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		port_id	- [out] Slot ID가 저장될 참조 변수 (1 or Later)
 retn : TRUE or FALSE
 note : slot - physical location capable of holding a substrate within cassette type carrier
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSourceSlotID(PTCHAR subst_id, LONG &slot_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSourceSlotID(subst_id, slot_id);
}

/*
 desc : Substrate가 속할 Source Slot ID 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		port_id	- [in]  sourceSlotID가 저장될 버퍼 (1 or Later)
 retn : TRUE or FALSE
 note : slot - physical location capable of holding a substrate within cassette type carrier
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSourceSlotID(PTCHAR subst_id, LONG slot_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSourceSlotID(subst_id, slot_id);
}

/*
 desc : Substrate의 마지막 이벤트 (작업)와 관련된 Substrate의 현재 위치 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		loc_id	- [out] Location ID가 저장될 버퍼
		size	- [in]  'loc_id' 버퍼 크기
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetCurrentSubstrateLocID(PTCHAR subst_id, PTCHAR loc_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetCurrentSubstrateLocID(subst_id, loc_id, size);
}

/*
 desc : Substrate의 마지막 이벤트 (작업)와 관련된 Substrate의 현재 위치 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		loc_id	- [in]  Location ID가 저장된 버퍼
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetCurrentSubstrateLocID(PTCHAR subst_id, PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetCurrentSubstrateLocID(subst_id, loc_id);
}

/*
 desc : Substrate의 Source ID 얻기
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		src_id	- [out] Source ID가 저장될 버퍼
		size	- [in]  'src_id' 버퍼 크기
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateSourceID(PTCHAR subst_id, PTCHAR src_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateSourceID(subst_id, src_id, size);
}

/*
 desc : Substrate의 Source ID 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		src_id	- [in]  Source ID가 저장된 버퍼
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateSourceID(PTCHAR subst_id, PTCHAR src_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSubstrateSourceID(subst_id, src_id);
}

/*
 desc : Substrate의 Destination ID 얻기
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		tgt_id	- [out] Source ID가 저장될 버퍼
		size	- [in]  'src_id' 버퍼 크기
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateDestinationID(PTCHAR subst_id, PTCHAR tgt_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateDestinationID(subst_id, tgt_id, size);
}

/*
 desc : Substrate의 Destination ID 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		tgt_id	- [in]  Source ID가 저장된 버퍼
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateDestinationID(PTCHAR subst_id, PTCHAR tgt_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSubstrateDestinationID(subst_id, tgt_id);
}

/*
 desc : 동일한 전이 (Transition)가 진행되는 관련 개체 그룹에 대해 전송되는 개별 이벤트를 억제하는데 사용
		객체 (Object)가 개별 이벤트를 억제하는지 여부 확인
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		flag	- [out] TRUE : 이벤트 보냄 (알림), FALSE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SGetUsingGroupEventsSubst(PTCHAR subst_id, BOOL &flag)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetUsingGroupEventsSubst(subst_id, flag);
}

/*
 desc : 동일한 전이 (Transition)가 진행되는 관련 개체 그룹에 대해 전송되는 개별 이벤트를 억제하는데 사용
		객체 (Object)가 개별 이벤트를 억제 혹은 승인하는지 여부 확인
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		flag	- [in]  TRUE : 이벤트 보냄 (알림), FALSE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SSetUsingGroupEventsSubst(PTCHAR subst_id, BOOL flag)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetUsingGroupEventsSubst(subst_id, flag);
}

/*
 desc : 동일한 전이 (Transition)가 진행되는 관련 개체 그룹에 대해 전송되는 개별 이벤트를 억제하는데 사용
		객체 (Object)가 개별 이벤트를 억제하는지 여부 확인
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		flag	- [out] TRUE : 이벤트 보냄 (알림), FALSE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SGetUsingGroupEventsLoc(PTCHAR loc_id, BOOL &flag)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetUsingGroupEventsLoc(loc_id, flag);
}

/*
 desc : 동일한 전이 (Transition)가 진행되는 관련 개체 그룹에 대해 전송되는 개별 이벤트를 억제하는데 사용
		객체 (Object)가 개별 이벤트를 억제 혹은 승인하는지 여부 확인
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		flag	- [in]  TRUE : 이벤트 보냄 (알림), FALSE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetUsingGroupEventsLoc(PTCHAR loc_id, BOOL flag)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetUsingGroupEventsLoc(loc_id, flag);
}

/*
 desc : Location에 위치한 Substrate ID 얻기
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [out] Substrate ID가 저장될 버퍼
		size	- [in]  'subst_id' 버퍼 크기
 retn : TRUE or FALSE
*/															   
API_EXPORT BOOL uvCIMLib_E90STS_GetLocationSubstID(PTCHAR loc_id, PTCHAR subst_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetLocationSubstID(loc_id, subst_id, size);
}

/*
 desc : Location에 위치한 Substrate ID 설정
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [in]  Substrate ID가 저장된 버퍼
 retn : TRUE or FALSE
*/															   
API_EXPORT BOOL uvCIMLib_E90STS_SetLocationSubstID(PTCHAR loc_id, PTCHAR subst_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetLocationSubstID(loc_id, subst_id);
}

/*
 desc : Location에 위치한 Location Index 얻기
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		index	- [out] Location Index가 반환될 참조 변수 (1 or Later)
 retn : TRUE or FALSE
*/															   
API_EXPORT BOOL uvCIMLib_E90STS_GetLocationIndex(PTCHAR loc_id, INT16 &index)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetLocationIndex(loc_id, index);
}

/*
 desc : Location에 위치한 Substrate ID 설정
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		index	- [in]  Location Index 값 (1 or Later)
 retn : TRUE or FALSE
*/															   
API_EXPORT BOOL uvCIMLib_E90STS_SetLocationIndex(PTCHAR loc_id, INT16 index)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetLocationIndex(loc_id, index);
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
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateHistory(PTCHAR subst_id, CAtlList <STG_CSHI> &lst_hist)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateHistory(subst_id, lst_hist);
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
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateHistory(PTCHAR subst_id, CAtlList <STG_CSHI> *lst_hist)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSubstrateHistory(subst_id, lst_hist);
}


/* --------------------------------------------------------------------------------------------- */
/*                             E94 Control Job Management (CJM)                                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Process Job들 중에서 한 개가 완료되면 Control Job에게 알림
 parm : pj_id	- [in]  Object ID of the Process Job
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_SetPRJobProcessComplete(PTCHAR pj_id)
{
	TCHAR tzCtrlID[CIM_CONTROL_JOB_ID_SIZE]	= {NULL};

	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	/* 현재 Process Job ID에 대한 Control Job ID 얻기 */
	if (!g_pE40PJM->GetControlJobID(pj_id, tzCtrlID, CIM_CONTROL_JOB_ID_SIZE))	return FALSE;
	if (wcslen(tzCtrlID) < 1)	return FALSE;
	/* Completed : Control Job */
	return g_pE94CJM->SetProcessJobComplete(tzCtrlID, pj_id);
}

/*
 desc : 현재 Queue에 등록된 Control Job ID 목록 및 각 상태 값들 반환
 parm : list_job	- [out] 반환되어 저장될 Control Job ID (String)
		list_state	- [out] 수집된 Control Job IDs에 대한 상태 값이 저장될 리스트
		logs		- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetControlJobList(CStringArray &list_job,
												  CAtlList<E94_CCJS> &list_state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetControlJobList(list_job, list_state);
}

/*
 desc : Process Jobs List 얻기 (Control Job 내부에 저장된 Project Job List)
 parm : list_job- [out] 수집된 Control Job IDs (Names) 저장될 배열 버퍼
		filter	- [in]  Control Job 추출 필터 (필터)
		logs	- [in]  로그 출력 여부
 retn : TRUE ro FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetControlJobListId(CStringArray &list_job, E94_CLCJ filter)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetControlJobList(list_job, filter);
}

/*
 desc : Process Jobs List 및 상태 값 얻기 (필터링 적용)
 parm : list_job	- [out] 수집된 Control Job IDs (Names) 저장될 배열 버퍼
		list_state	- [out] 수집된 Control Job IDs에 대한 상태 값이 저장될 리스트
		filter		- [in]  Control Job 추출 필터 (필터)
		logs		- [in]  로그 출력 여부
 retn : TRUE ro FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetControlJobListIdState(CStringArray &list_job,
														 CAtlList<E94_CCJS> &list_state,
														 E94_CLCJ filter)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetControlJobList(list_job, list_state, filter);
}

/*
 desc : Control Job ID 중 필터에 의해 검색된 것 중 첫 번째 검색된 Job ID 반환
 parm : filter	- [in]  Process Job ID 검색 필터 (E40_PPJS)
		cj_id	- [out] 반환되어 저장될 Control Job ID (String)
		size	- [in]  'cj_id' 버퍼 크기
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetControlJobFirstID(E94_CCJS filter, PTCHAR cj_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetControlJobFirstID(filter, cj_id, size);
}

/*
 desc : Control Job의 상태 정보 반환
 parm : cj_id	- [in]  Control Job ID
		state	- [out] Job ID의 상태 값 반환
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetControlJobState(PTCHAR cj_id, E94_CCJS &state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetControlJobState(cj_id, state);
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
API_EXPORT BOOL uvCIMLib_E94CJM_CreateJob(PTCHAR cj_id, PTCHAR carr_id, CStringArray *pj_id,
										  E94_CPOM job_order, BOOL start)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->CreateJob(cj_id, carr_id, pj_id, job_order, start);
}

/*
 desc : Process Jobs List 얻기 (Control Job 내부에 저장된 Project Job List)
 parm : cj_id		- [in]  Object ID of the Control Job
		list_job	- [out] 수집된 Process Job Names 저장될 배열 버퍼
		list_state	- [out] 수집된 Control Job IDs (Names)의 State가 저장될 리스 버퍼
		filter		- [in]  Control Job 내부에 저장된 Process Job 추출 필터 (필터)
		logs		- [in]  로그 출력 여부
 retn : TRUE ro FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetProcessJobListIdState(PTCHAR cj_id,
														 CStringArray &list_job,
														 CAtlList <E40_PPJS> &list_state,
														 E94_CLPJ filter)
{
	INT32 i	= 0;
	E40_PPJS enState;

	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	if (!g_pE94CJM->GetProcessJobList(cj_id, list_job, filter))	return FALSE;

	for (; i<list_job.GetCount(); i++)
	{
		if (!g_pE40PJM->GetProcessJobState(list_job[i].GetBuffer(), enState))	return FALSE;
		list_state.AddTail(enState);
	}

	return (list_job.GetCount() == list_state.GetCount());
}

/*
 desc : Process Jobs List 얻기 (Control Job 내부에 저장된 Project Job List)
 parm : cj_id	- [in]  Object ID of the Control Job
		list_job- [out] 수집된 Process Job Names 저장될 배열 버퍼
		filter	- [in]  Control Job 내부에 저장된 Process Job 추출 필터 (필터)
		logs	- [in]  로그 출력 여부
 retn : TRUE ro FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetProcessJobList(PTCHAR cj_id, CStringArray &list_job,
												  E94_CLPJ filter)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetProcessJobList(cj_id, list_job, filter);
}

/*
 desc : Control Job을 선택하기
 parm : cj_id		- [in]  Control Job ID
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_SetSelectedControlJob(PTCHAR cj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->SetSelectedControlJob(cj_id);
}

/*
 desc : Control Job을 선택 해제하기
 parm : cj_id		- [in]  Control Job ID
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_SetDeselectedControlJob(PTCHAR cj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->SetDeselectedControlJob(cj_id);
}

/*
 desc : Remote Control 허용 여부 값 반환
 parm : enable	- [out] Enable or Disable
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetAllowRemoteControl(BOOL &enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetAllowRemoteControl(enable);
}

/*
 desc : Remote Control 허용 여부 설정
 parm : enable	- [in]  Enable or Disable
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_SetAllowRemoteControl(BOOL enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->SetAllowRemoteControl(enable);
}

/*
desc : Control Job 내부에서 다음 ProcessJob을 시작시킨다.
parm : cj_id	- [in]  Control Job ID
retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_StartNextProcessJob(PTCHAR cj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->StartNextProcessJob(cj_id);
}

/*
desc : 다음 Control Job을 시작시킨다.
parm : cj_id	- [in]  Control Job ID
retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_StartNextControlJob()
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->StartNextControlJob();
}


/* --------------------------------------------------------------------------------------------- */
/*                        E116 Equiptment Performance Tracking (EPT)                             */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : EPT (Equiptment Performance Tracking) - Busy 설정
 parm : mod_name	- [in]  Name of the EPT module, use an epty string for the Equipemnt ETP state
							ex> ProcessChamber, RobotArm, PreAligner, Buffer, etc
		task_name	- [in]  Name of the task
							ex> Lithography, Align, etc
		task_type	- [in]  Type of task (E116_ETBS)
 retn : TRUE or FALSE
 note : This method requests that an EPT module or the EPT equipment state machine transition to the BUSY state
		Note: Using this function to directly set Equipment state to BUSY is not compliant with E116-1104
			  (and later revisions).
*/
API_EXPORT BOOL uvCIMLib_E116EPT_SetBusy(PTCHAR mod_name, PTCHAR task_name, E116_ETBS task_type)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE116EPT->SetBusy(mod_name, task_name, task_type);
}

/*
 desc : EPT (Equiptment Performance Tracking) - Idle 설정
 parm : mod_name	- [in]  Name of the EPT module, use an epty string for the Equipemnt ETP state
							ex> ProcessChamber, RobotArm, PreAligner, Buffer, etc
 retn : TRUE or FALSE
 note : This method requests that an EPT module or the EPT equipment state machine transition to the IDLE state
		Note: Using this function to directly set Equipment state to IDLE is not compliant with E116-1104
			  (and later revisions)
			  Also use at startup for the NoState->Idle transition during initialization
*/
API_EXPORT BOOL uvCIMLib_E116EPT_SetIdle(PTCHAR mod_name)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE116EPT->SetIdle(mod_name);
}

/* --------------------------------------------------------------------------------------------- */
/*                           E157 Module Processing Tracking (MPT)                               */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Module Process Tracking 실행 시작
 parm : mod_name- [in]  Name of the EPT module, use an epty string for the Equipemnt ETP state
						ex> ProcessChamber, RobotArm, PreAligner, Buffer, etc
		pj_id	- [in]  Process Job Name ID (String; Ascii)
		rp_id	- [in]  Recipe ID
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E157MPT_MPTExecutionStarted(PTCHAR mod_name, PTCHAR pj_id, PTCHAR rp_id, PTCHAR subst_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE157MPT->MPTExecutionStarted(mod_name, pj_id, rp_id, subst_id);
}

/*
 desc : Module Process Tracking 실행 완료 (레시피 실행 완료)
 parm : mod_name- [in]  Name of the EPT module, use an epty string for the Equipemnt ETP state
						ex> ProcessChamber, RobotArm, PreAligner, Buffer, etc
		exec_ok	- [in]  TRUE : Execution success, FALSE : Execution failed
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E157MPT_MPTExecutionCompleted(PTCHAR mod_name, BOOL exec_ok)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE157MPT->MPTExecutionCompleted(mod_name, exec_ok);
}

/*
 desc : Module Process Tracking 단계 시작
 parm : mod_name- [in]  Name of the EPT module, use an epty string for the Equipemnt ETP state
						ex> ProcessChamber, RobotArm, PreAligner, Buffer, etc
		step_id	- [in]  Step ID (Name)
						ex> LithographStep1 ~ LithographStep??
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E157MPT_MPTStepStarted(PTCHAR mod_name, PTCHAR step_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE157MPT->MPTStepStarted(mod_name, step_id);
}

/*
 desc : Module Process Tracking 단계 완료
 parm : mod_name- [in]  Name of the EPT module, use an epty string for the Equipemnt ETP state
						ex> ProcessChamber, RobotArm, PreAligner, Buffer, etc
		step_ok	- [in]  TRUE : Step success, FALSE : step failed
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E157MPT_MPTStepCompleted(PTCHAR mod_name, BOOL step_ok)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE157MPT->MPTStepCompleted(mod_name, step_ok);
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
API_EXPORT BOOL uvCIMLib_E94CJM_GetControlJobCarrierInputSpec(PTCHAR cj_id, CStringList &list_carr)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetControlJobCarrierInputSpec(cj_id, list_carr);
}

/*
 desc :
 parm : cj_id		- [in]  Control Job ID
		list_carr	- [out] Carrier List 반환될 버퍼
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetControlJobCarrierOutputSpec(PTCHAR cj_id, CStringList &list_carr)
{
	// Anderson 0515
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetControlJobCarrierOutputSpec(cj_id, list_carr);
}

/*
 desc : Control Job ID에 포함된 Process Job ID List 반환
 parm : cj_id	- [in]  Control Job ID
		list_pj	- [out] Process Job List 반환될 버퍼
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetControlJobProcessJobList(PTCHAR cj_id, CStringList &list_pj)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetControlJobProcessJobList(cj_id, list_pj);
}

/*
 desc : Control Job의 자동 시작 여부 확인
 parm : cj_id		- [in]  Control Job ID
		auto_start	- [out] Auto Start 여부 값 반환
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetControlJobStartMethod(PTCHAR cj_id, BOOL &auto_start)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetControlJobStartMethod(cj_id, auto_start);
}

/*
 desc : Control Job의 작업 완료 시간 얻기
 parm : cj_id		- [in]  Control Job ID
		date_time	- [out] 작업 완료 시간 문자열로 반환
		size		- [in]  'data_time' 버퍼 크기
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetControlJobCompletedTime(PTCHAR cj_id, PTCHAR date_time, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetControlJobCompletedTime(cj_id, date_time, size);
}

/*
 desc : Control Job의 Material 도착 여부 얻기
 parm : cj_id	- [in]  Control Job ID
		arrived	- [out] TRUE : 도착, FALSE : 미도착
 retn : TRUE or FALSE
 note : CarrierInputSpec 속성에서 모든 Carrier들이 장비에 도착 했을 때 TRUE 반환
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetControlJobMaterialArrived(PTCHAR cj_id, BOOL &arrived)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetControlJobMaterialArrived(cj_id, arrived);
}

/*
 desc : Control Job의 시작 설정
 parm : cj_id	- [in]  Control Job ID
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_SetStartControlJob(PTCHAR cj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->SetStartControlJob(cj_id);
}

/*
 desc : Control Job의 중지 (작업이 완료되었거나 실패 했거나 등 일 경우, 중지 시킴) 알림
 parm : cj_id	- [in]  Control Job ID
		action	- [in]  작업 저장 혹은 제거 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_SetStopControlJob(PTCHAR cj_id, E94_CCJA action)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->SetStopControlJob(cj_id, action);
}

/*
 desc : Control Job의 중단 (작업 시작 중에 작업을 아예 중지 시켜버림) 알림
 parm : cj_id	- [in]  Control Job ID
		action	- [in]  작업 저장 혹은 제거 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_SetAbortControlJob(PTCHAR cj_id, E94_CCJA action)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->SetAbortControlJob(cj_id, action);
}

/*
 desc : Control Job의 잠시 중지 (일시 중지. 취소 / 중단 등 아님) 알림
 parm : cj_id	- [in]  Control Job ID
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_SetPauseControlJob(PTCHAR cj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->SetPauseControlJob(cj_id);
}

/*
 desc : Control Job의 재시작 (중지 시점부터) 알림
 parm : cj_id	- [in]  Control Job ID
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_SetResumeControlJob(PTCHAR cj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->SetResumeControlJob(cj_id);
}

/*
 desc : Control Job의 취소 (시작하지 않은 상태에서... 아예 작업을 하지 않음) 알림
 parm : cj_id	- [in]  Control Job ID
		action	- [in]  작업 저장 혹은 제거 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_SetCancelControlJob(PTCHAR cj_id, E94_CCJA action)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->SetCancelControlJob(cj_id, action);
}

/*
 desc : Control Job을 Queue의 맨 상위로 이동 시킴을 알림
 parm : cj_id	- [in]  Control Job ID
 retn : TRUE or FALSE
 note : Moves the specified control job to the top of the queue.
		Other control jobs in the queue are pushed back.
		The rest of the queue order remains unchanged.
*/
API_EXPORT BOOL uvCIMLib_E94CJM_SetHOQControlJob(PTCHAR cj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->SetHOQControlJob(cj_id);
}


/* --------------------------------------------------------------------------------------------- */
/*                     E30 / E40 / E87 / E90 / E94 / E116 / E157 : Mixed Area                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Carrier 내의 Slot Number에 대한 Destination Substrate ID 얻기
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		slot_no	- [in]  Slot Number (Not sequential number)
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		size	- [in]  'st_id' 버퍼 크기
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_Mixed_GetSubstrateDestinationID(PTCHAR carr_id, UINT8 slot_no,
														 PTCHAR subst_id, UINT32 size)
{
	ICxSubstratePtr pICxSubstPtr;
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	if (!g_pE87CMS->GetSubstratePtrFromSource(carr_id, slot_no, pICxSubstPtr))
		return FALSE;
	return g_pE90STS->GetSubstrateDestinationID(pICxSubstPtr, subst_id, size);
}

/*
 desc : Carrier 내에 저장된 Slot Number (No)에 대한 Substrate ID 값 반환
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		slot_no	- [in]  Slot Number (1-based or Later)
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		size	- [in]  'st_id' 버퍼 크기
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_Mixed_GetSubstrateID(PTCHAR carr_id, UINT8 slot_no, PTCHAR subst_id,
											  UINT32 size)
{
	ICxSubstratePtr pICxSubstPtr;
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	if (!g_pE87CMS->GetSubstratePtrFromSource(carr_id, slot_no, pICxSubstPtr))	return FALSE;
	return g_pE90STS->GetSubstrateID(pICxSubstPtr, subst_id, size);
}

/*
 desc : Control Job ID에 등록되어 있던 특정 Process Job ID의 작업이 모두 끝났는지 여부
 parm : pj_id	- [in]  Process Job ID
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE (Completed) or FALSE (Not complete)
*/
API_EXPORT BOOL uvCIMLib_Mixed_IsSubstrateProcessCompleted(PTCHAR pj_id)
{
	BOOL bSucc	= FALSE;
	INT32 i	= 0;
	TCHAR tzSubstID[CIM_SUBSTRATE_ID]	= {NULL};
	E90_SSPS enState;
	CStringArray arrCarrID;
	CByteArray arrSlotNo;
	ICxSubstratePtr pICxSubstPtr;

	if (!g_pE30GEM->IsInitedAll())	return FALSE;

	/* Process Job ID에 속한 Substrate ID 속성 (정보) 즉, Carrier ID와 Slot Number 개수 얻기*/
	if (!g_pE40PJM->GetPRMtlNameList(pj_id, arrCarrID, arrSlotNo))	return FALSE;
	/* 등록된 Substrate 개수만큼 반복하여 Process State 값 얻기 */
	for (; i<arrCarrID.GetCount(); i++)
	{
		/* ICxSubstratePtr 포인터 얻기 */
		if (!g_pE87CMS->GetSubstratePtrFromSource(arrCarrID[i].GetBuffer(),
												  arrSlotNo[i],
												  pICxSubstPtr))	break;
		/* Slot Number 위치에 해당되는 Substrate ID 얻기 */
		wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);
		if (!g_pE90STS->GetSubstrateID(pICxSubstPtr, tzSubstID, CIM_SUBSTRATE_ID))	break;
		/* Substrate ID의 Process 상태 값 얻기 */
		if (!g_pE90STS->GetSubstrateProcessingState(tzSubstID, enState))	break;
		/* 혹시 1개라도 Completed 상태가 아니면 리턴 FALSE 즉, 아래 3조건이 아니면 Completed 하다고 판단 */
		if (E90_SSPS::en_no_state == enState || E90_SSPS::en_needs_processing == enState ||
			E90_SSPS::en_in_process == enState)	break;
	}
	
	/* 모두 처리 했는지 여부에 따라 성공 여부 */
	bSucc	= (arrCarrID.GetCount() == i);
	/* 메모리 해제  */
	arrSlotNo.RemoveAll();

	return bSucc;
}

/*
 desc : Carrier 내에 등록되어 있었던 Process Job ID 관련 모든 SubstrateID의 Transport 위치가
		Destination (FOUP; Carrier)에 있는지 여부
 parm : pj_id	- [in]  Process Job ID
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE (Inside the destination) or FALSE (Outside the destination)
*/
API_EXPORT BOOL uvCIMLib_Mixed_IsSubstrateTransportDestination(PTCHAR pj_id)
{
	BOOL bSucc	= FALSE;
	INT32 i	= 0;
	TCHAR tzSubstID[CIM_SUBSTRATE_ID]	= {NULL};
	E90_SSTS enState;
	CStringArray arrCarrID;
	CByteArray arrSlotNo;
	ICxSubstratePtr pICxSubstPtr;

	if (!g_pE30GEM->IsInitedAll())	return FALSE;

	/* Process Job ID에 속한 Substrate ID 속성 (정보) 즉, Carrier ID와 Slot Number 개수 얻기*/
	if (!g_pE40PJM->GetPRMtlNameList(pj_id, arrCarrID, arrSlotNo))	return FALSE;
	/* 등록된 Substrate 개수만큼 반복하여 Process State 값 얻기 */
	for (; i<arrCarrID.GetCount(); i++)
	{
		/* ICxSubstratePtr 포인터 얻기 */
		if (!g_pE87CMS->GetSubstratePtrFromSource(arrCarrID[i].GetBuffer(),
												  arrSlotNo[i],
												  pICxSubstPtr))	break;
		/* Slot Number 위치에 해당되는 Substrate ID 얻기 */
		wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);
		if (!g_pE90STS->GetSubstrateID(pICxSubstPtr, tzSubstID, CIM_SUBSTRATE_ID))	break;
		/* Substrate ID의 Process 상태 값 얻기 */
		if (!g_pE90STS->GetSubstrateTransportState(tzSubstID, enState))	break;
		/* 혹시 1개라도 Destination 상태가 아니면 리턴 FALSE 즉, 아래 3조건이 아니면 Destination 하다고 판단 */
		if (E90_SSTS::en_no_state == enState || E90_SSTS::en_source == enState || E90_SSTS::en_work == enState)	break;
	}
	
	/* 모두 처리 했는지 여부에 따라 성공 여부 */
	bSucc	= (arrCarrID.GetCount() == i);
	/* 메모리 해제  */
	arrSlotNo.RemoveAll();

	return bSucc;
}

/*
 desc : Process Job ID가 작업이 끝날 때, 상위 Process Job ID까지 자동으로 Completed 시킴
 parm : pj_id	- [in]  Process Job ID
 retn : TRUE or FALSE
 note : Process Job ID를 1개만 가지고 있는 Control Job ID일 때만 이 함수 호출이 유효 합니다.
*/
API_EXPORT BOOL uvCIMLib_Mixed_SetAutoJobCompleted(PTCHAR pj_id)
{
	TCHAR tzCRJobID[CIM_CONTROL_JOB_ID_SIZE]	= {NULL};

	/* Get the Control Job ID */
	if (!g_pE40PJM->GetControlJobID(pj_id, tzCRJobID, CIM_CONTROL_JOB_ID_SIZE))	return FALSE;

	/* Process Job ID가 1개만 가지고 있는 Control Job 인지 여부 확인 */
	if (g_pE94CJM->GetProcessJobCount(tzCRJobID) != 1)	return FALSE;

	/* Process Job Completed & PRJob Completed */
	if (!g_pE40PJM->SetPRJobProcessComplete(pj_id))				return FALSE;
	if (!g_pE40PJM->SetPRJobCompleteOnly(pj_id))				return FALSE;
	/* Control Job Completed */
	if (!g_pE94CJM->SetProcessJobComplete(pj_id, tzCRJobID))	return FALSE;

	return TRUE;
}

/*
 desc : E40PJM::PRCommandCbk 호출될 때, 사용하기 위해 SubstateIDs의 상태 정보를 갱신해 두기
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_Mixed_UpdateSubstrateIDsState()
{
	BOOL bAutoStart;
	INT32 i	= 0, j = 0;
	E40_PPJS enPRJobState;
	STG_CSDI stPRCmdState;
	CAtlList <STG_CSDI> lstPRCmdState;
	CStringArray arrPRJobIDs, arrSubstIDs, arrCarrIDs;

	if (!g_pE30GEM->IsInitedAll())	return FALSE;

	/* 현재 등록 (작업될 대상)된 Process Job IDs 얻기 */
	if (!g_pE40PJM->GetProcessAllJobId(arrPRJobIDs))	return FALSE;
	/* 현재 등록된 Process Job ID 별로 Substate ID 정보 얻기 */
	for (; i<arrPRJobIDs.GetCount(); i++)
	{
		/* 기존 등록된 리스트 제거 */
		arrSubstIDs.RemoveAll();
		/* 각 Process Job ID 별로 등록된 Substrate ID 리스트 얻기 */
		if (g_pE40PJM->GetProcessJobAllSubstID(arrPRJobIDs[i].GetBuffer(),
											   arrSubstIDs, arrCarrIDs))
		{
			/* PRJobCommandCbk 호출될 때, 필요한 정보 갱신을 위한 버퍼 초기화 및 값 얻기 */
			for (j=0; j<arrSubstIDs.GetCount(); j++)
			{
				memset(&stPRCmdState, 0x00, sizeof(STG_CSDI));
				if (g_pE90STS->GetPRCommandSubstState(arrSubstIDs[i].GetBuffer(),
													  stPRCmdState))
				{
					/* 임시적으로 리스트 메모리에 등록 */
					lstPRCmdState.AddTail(stPRCmdState);
				}
			}
			if (lstPRCmdState.GetCount())
			{
				/* Process Job ID의 시작 방법 얻기 */
				if (!g_pE40PJM->GetProcessStartMethod(arrPRJobIDs[i].GetBuffer(), bAutoStart))
					return FALSE;

				if (g_pE40PJM->GetProcessJobState(arrPRJobIDs[i].GetBuffer(), enPRJobState))
				{
					g_pSharedData->UpdatePRJobToSubstID(arrPRJobIDs[i].GetBuffer(), bAutoStart,
														enPRJobState, &lstPRCmdState);
				}
			}
			/* 리스트 버퍼 메모리 해제 */
			lstPRCmdState.RemoveAll();
		}
	}

	/* 임시 Array Buffer 메모리 해제 */
	arrPRJobIDs.RemoveAll();
	arrSubstIDs.RemoveAll();
	arrCarrIDs.RemoveAll();

	return TRUE;
}

/*
 desc : CJ Paused되었을 때 PJ들을 Pause시킨다.
 parm : cj_id	- [in] Control Job ID
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_Mixed_PauseProcessJobs(PTCHAR cj_id)
{
	INT32 i;
	BOOL bResult	= TRUE;

	CStringArray strArrProcID;
	CAtlList<E40_PPJS> lstState;
	if (!uvCIMLib_E94CJM_GetProcessJobListIdState(cj_id, strArrProcID, lstState,
												  E94_CLPJ::en_list_all))	return FALSE;
	for (i=0; i<lstState.GetCount(); i++)
	{
		switch (lstState.GetAt(lstState.FindIndex(i)))
		{
		case E40_PPJS::en_setting_up			:
		case E40_PPJS::en_waiting_for_start	:
		case E40_PPJS::en_processing			:
			if (!uvCIMLib_E40PJM_SetPRJobStateControl(strArrProcID[i].GetBuffer(),
													  E40_PPJC::en_pause))	bResult	= FALSE;
		}
	}

	return bResult;
}

/*
 desc : CJ가 Executing되었을 때 PJ의 처리를 재개한다.
 parm : cj_id	- [in] Control Job ID
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_Mixed_ResumePausedProcessJobs(PTCHAR cj_id)
{
	INT32 i;
	BOOL bResult	= TRUE;
	CStringArray arrProcID;
	CAtlList<E40_PPJS> lstState;

	if (!uvCIMLib_E94CJM_GetProcessJobListIdState(cj_id, arrProcID, lstState,
												  E94_CLPJ::en_list_all))	return FALSE;

	for (i=0; i<lstState.GetCount(); i++)
	{
		switch (lstState.GetAt(lstState.FindIndex(i)))
		{
		case E40_PPJS::en_pausing	:
		case E40_PPJS::en_paused		:
			if (!uvCIMLib_E40PJM_SetPRJobStateControl(arrProcID[i].GetBuffer(), E40_PPJC::en_resume))
			{
				// log write
				bResult = FALSE;
			}
		}
	}

	return bResult;
}

/*
 desc : CJ가 queued되었을 때 Material Out Spec을 갱신한다.
 parm : cj_id	- [in] Control Job ID
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_Mixed_UpdateMtrlOutSpecSubstrateDestination(PTCHAR cj_id)
{
	UINT8 u8SlotNo	= 0;
	BOOL bFoundInSourceMap	= FALSE;
	INT32 i, j, i32SubstSlotNo = 0;
	LONG lHandleDest, lHandleDestSlots, lHandleCountSlots;
	HRESULT hResult = S_OK;
	ICxE94ControlJobPtr pICxE94CRJobPtr;
	ICxE39ObjPtr pICxE39Obj;
	ICxValueDispPtr pICxMtrlOutSpecVal(CLSID_CxValueObject);
	ICxValueDispPtr pICxSubstDestVal(CLSID_CxValueObject);
	ICxSubstratePtr pICxSubstPtr;
	ICxE39ObjPtr pICxE39ObjPtr;
	POSITION pPos	= NULL;
	CComBSTR bstrSubstCarrID, bstrSpecCarrID, bstrDestCarrID, bstrDest;
	CString strIDs;
	CStringList lstPRJobIDs, lstSubstTotal;
	CStringArray arrSubstIDs, arrCarrIDs;

	if (!g_pE94CJM->CJGetJob(cj_id, pICxE94CRJobPtr))	return FALSE;
	if (!pICxE94CRJobPtr)	return FALSE;

	pICxE39Obj	= (ICxE39ObjPtr)pICxE94CRJobPtr;
	hResult		= pICxE39Obj->GetAttr(CComBSTR(L"MtrlOutSpec"), pICxMtrlOutSpecVal);
	if (g_pLogData->CheckResult(hResult, L"ICxE39ObjPtr::GetAttr"))	return FALSE;

	/* get only using substrates. get all pj list from pICxE94CRJobPtr */
	if (!g_pE94CJM->GetControlJobProcessJobList(cj_id, lstPRJobIDs))	return FALSE;

	for (i=0; i<lstPRJobIDs.GetCount(); i++)
	{
		strIDs = lstPRJobIDs.GetAt(lstPRJobIDs.FindIndex(i));
		if (!g_pE40PJM->GetProcessJobAllSubstID(strIDs.GetBuffer(), arrSubstIDs, arrCarrIDs))	return FALSE;
		for (j=0; j<lstPRJobIDs.GetCount(); j++)
		{
			lstSubstTotal.AddTail(arrSubstIDs[j]);
		}
	}

	pPos = lstSubstTotal.GetHeadPosition();
	while(pPos)
	{
		strIDs = lstSubstTotal.GetNext(pPos);
		if (!g_pE90STS->GetSubstratePtr(strIDs.GetBuffer(), pICxSubstPtr))	return FALSE;
		
		// get carrier id from substrate
		hResult = pICxSubstPtr->get_sourceCarrierID(&bstrSubstCarrID);
		if (g_pLogData->CheckResult(hResult, L"ICxSubstratePtr::get_sourceCarrierID"))
			return FALSE;

		// get slot no from substrate
		hResult = pICxSubstPtr->get_sourceSlotID(&i32SubstSlotNo);
		if (g_pLogData->CheckResult(hResult, L"ICxSubstratePtr::get_sourceSlotID"))
			return FALSE;

		LONG count_MapSet = 0;
		pICxMtrlOutSpecVal->ItemCount(0, &count_MapSet);
		for (int mapIndex = 1; mapIndex <= count_MapSet; mapIndex++)
		{
			// map header
			LONG h_root = 0;
			pICxMtrlOutSpecVal->ListAt(0, mapIndex, &h_root);

			// source map
			LONG h_source = 0;
			pICxMtrlOutSpecVal->ListAt(h_root, 1, &h_source);

			// source carrier id
			hResult = pICxMtrlOutSpecVal->GetValueAscii(h_source, 1, &bstrSpecCarrID);
			if (g_pLogData->CheckResult(hResult, L"ICxE39ObjPtr::GetAttr"))
				return FALSE;

			// check carrier id
			if (bstrSubstCarrID == bstrSpecCarrID)
			{
				// source slots
				LONG h_source_slots = 0;
				pICxMtrlOutSpecVal->ListAt(h_source, 2, &h_source_slots);
				// source slot counts
				LONG count_s_slots = 0;
				pICxMtrlOutSpecVal->ItemCount(h_source_slots, &count_s_slots);

				bFoundInSourceMap	= FALSE;
				for (int slotIndex = 1; slotIndex <= count_s_slots; slotIndex++)
				{
					// source slot No
					UINT8 s_slotNo = 0;
					hResult = pICxMtrlOutSpecVal->GetValueU1(h_source_slots, slotIndex, &s_slotNo);
					if (g_pLogData->CheckResult(hResult, L"pICxMtrlOutSpecVal::GetValueU1"))
						return FALSE;

					if (i32SubstSlotNo == s_slotNo)
					{
						bFoundInSourceMap = TRUE;

						// destination map
						lHandleDest = 0;
						pICxMtrlOutSpecVal->ListAt(h_root, 2, &lHandleDest);

						// destination carrier id
						hResult = pICxMtrlOutSpecVal->GetValueAscii(lHandleDest, 1, &bstrDestCarrID);
						if (g_pLogData->CheckResult(hResult, L"pICxMtrlOutSpecVal::GetValueAscii"))
							return FALSE;

						// destination slots
						lHandleDestSlots	= 0;
						pICxMtrlOutSpecVal->ListAt(lHandleDest, 2, &lHandleDestSlots);
						// destination slot counts
						lHandleCountSlots	= 0;
						pICxMtrlOutSpecVal->ItemCount(lHandleDestSlots, &lHandleCountSlots);

						if (lHandleCountSlots == 0)
						{
							pICxSubstDestVal->SetValueAscii(0, 0, L"");
						}
						else
						{
							// destination slot no
							pICxMtrlOutSpecVal->GetValueU1(lHandleDestSlots, slotIndex, &u8SlotNo);
							// desination update
							strIDs.Format(_T("%s.%02s"), bstrDestCarrID, u8SlotNo);
							bstrDest.Append(strIDs);
							pICxSubstDestVal->SetValueAscii(0, 0, bstrDest);
						}
						break;
					}
				}
			}
			if (bFoundInSourceMap)	break;
		}

		// final
		pICxE39ObjPtr = (ICxE39ObjPtr)pICxSubstPtr;
		pICxE39ObjPtr->SetAttr(L"SubstDestination", pICxSubstDestVal);
	}

	return (hResult == S_OK);
}

/*
 desc : Carrier SlotMap Verified일 때, Material Out Spec을 갱신한다.
 parm : carr_id	- [in] Carrier ID
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_Mixed_UpdateMtrlOutSpecSubstrateDestinationByE87(PTCHAR carr_id)
{
	INT32 i	= 0;
	POSITION pPosHead, pPosFound;
	CStringArray arrCRJobIDs;
	CStringList lstCarrInIDs, lstCarrOutIDs;

	// get all CJ list.
	if (!uvCIMLib_E94CJM_GetControlJobListId(arrCRJobIDs, E94_CLCJ::en_list_all))	return FALSE;

	for (i=0; i<arrCRJobIDs.GetCount(); i++)
	{
		// input spec
		uvCIMLib_E94CJM_GetControlJobCarrierInputSpec(arrCRJobIDs[i].GetBuffer(), lstCarrInIDs);
		// update if input spec carriers contain the carrier id
		pPosHead	= lstCarrInIDs.GetHeadPosition();
		pPosFound	= lstCarrInIDs.Find((LPCTSTR)carr_id, pPosHead);
		if (pPosFound >= 0)
		{
			if (!uvCIMLib_Mixed_UpdateMtrlOutSpecSubstrateDestination(arrCRJobIDs[i].GetBuffer()))
				return FALSE;
		}
		else
		{
			// output spec
			uvCIMLib_E94CJM_GetControlJobCarrierOutputSpec(arrCRJobIDs[i].GetBuffer(), lstCarrOutIDs);
			// update if output spec carriers contain the carrier id
			pPosHead	= lstCarrOutIDs.GetHeadPosition();
			pPosFound	= lstCarrOutIDs.Find((LPCTSTR)carr_id, pPosHead);
			if (pPosFound >= 0)
			{
				if (!uvCIMLib_Mixed_UpdateMtrlOutSpecSubstrateDestination(arrCRJobIDs[i].GetBuffer()))
					return FALSE;
			}
		}
	}

	return TRUE;
}

/*
 desc : 모든 CJ의 Material Out Spec을 갱신한다.
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_Mixed_UpdateMtrlOutSpecSubstrateDestinationAll()
{
	INT32 i;
	CStringArray arrCRJobIDs;

	// get all CJ list.
	if (!uvCIMLib_E94CJM_GetControlJobListId(arrCRJobIDs, E94_CLCJ::en_list_all))	return FALSE;

	for (i=0; i<arrCRJobIDs.GetCount(); i++)
	{
		if (!uvCIMLib_Mixed_UpdateMtrlOutSpecSubstrateDestination(arrCRJobIDs[i].GetBuffer()))
			return FALSE;
	}

	return TRUE;
}

/*
 desc : Process Job에 해당하는 substrate state를 모두 확인하고
		__en_e90_substrate_processing_state__ 가 E90_SSPS::en_needs_processing 인 경우
		모두 E90_SSPS::en_skipped로 변경함
 parm : pj_id	- [in]  Process Job ID
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_Mixed_SkipSubstratesNeedingProcessing(PTCHAR pj_id)
{
	INT32 i = 0;
	BOOL logs	= TRUE, bSucc = FALSE;
	TCHAR tzSubstID[CIM_SUBSTRATE_ID] = { NULL };
	E90_SSPS enState;
	CStringArray arrCarrID;
	CByteArray arrSlotNo;
	ICxSubstratePtr pICxSubstPtr;

	if (!g_pE30GEM->IsInitedAll())	return FALSE;

	/* Process Job ID에 속한 Substrate ID 속성 (정보) 즉, Carrier ID와 Slot Number 개수 얻기*/
	if (!g_pE40PJM->GetPRMtlNameList(pj_id, arrCarrID, arrSlotNo))	return FALSE;
	/* 등록된 Substrate 개수만큼 반복하여 Process State 값 얻기 */
	for (; i<arrCarrID.GetCount(); i++)
	{
		/* ICxSubstratePtr 포인터 얻기 */
		if (!g_pE87CMS->GetSubstratePtrFromSource(arrCarrID[i].GetBuffer(), arrSlotNo[i],
												  pICxSubstPtr))	break;
		/* Slot Number 위치에 해당되는 Substrate ID 얻기 */
		wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);
		if (!g_pE90STS->GetSubstrateID(pICxSubstPtr, tzSubstID, CIM_SUBSTRATE_ID))	break;
		/* Substrate ID의 Process 상태 값 얻기 */
		if (!g_pE90STS->GetSubstrateProcessingState(tzSubstID, enState))	break;
		/* Substrate의 process 상태값이 E90_SSPS::en_needs_processing이라면 E90_SSPS::en_skipped로 변경 */
		if (E90_SSPS::en_needs_processing == enState)
		{
			if (!g_pE90STS->SetSubstrateProcessingState(tzSubstID, E90_SSPS::en_skipped))  break;
		}
	}

	/* 모두 처리 했는지 여부에 따라 성공 여부 */
	bSucc = (arrCarrID.GetCount() == i);
	/* 메모리 해제  */
	arrSlotNo.RemoveAll();

	return bSucc;
}

/*
 desc : Check to see if all of the substrates in the ProcessJob are completed and are no longer expected
		to be moved. If so, then let CIM40/CIM94 know that the ProcessJob is complete so it can be deleted
		when the material is removed.
		참고 1) substrates가 모두 destination에 도착한것만 체크하면 안됨, 왜냐하면 source에 위치해도 aborted/stopped로
				처리된 경우에는 여전히 destination이 아닌 source에 있지만 complete일 수 있기 때문임.
			 2) substrates가 destination에 도착하기 전에 PROCESSED state 상에서 complete처리 될 수 있음.
				따라서 단순히 substrates의 각 상태가 PROCESSING COMPLETED 인것 만으로 전체 complete을 선언하면 안됨.
 parm : pj_id	- [in]  Process Job ID
 retn : TRUE (Completed 수행 함) or FALSE (Completed 수행하지 않음)
*/
API_EXPORT BOOL uvCIMLib_Mixed_CheckJobComplete(PTCHAR pj_id)
{
	INT32 i = 0;
	BOOL logs	= TRUE, bSucc = FALSE;
	TCHAR tzCRJobID[CIM_CONTROL_JOB_ID_SIZE]= { NULL };
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]			= { NULL };
	TCHAR tzSubstID[CIM_SUBSTRATE_ID]		= { NULL };
	HRESULT hResult;
	E90_SSPS enState;
	E90_SSTS trpState;
	CStringArray arrCarrID;
	CByteArray arrSlotNo;
	ICxSubstratePtr pICxSubstPtr;

	if (!g_pE30GEM->IsInitedAll())	return FALSE;

	/* Process Job ID에 속한 Substrate ID 속성 (정보) 즉, Carrier ID와 Slot Number 개수 얻기*/
	if (!g_pE40PJM->GetPRMtlNameList(pj_id, arrCarrID, arrSlotNo))	return FALSE;

	/* 등록된 Substrate 개수만큼 반복하여 Process State 값 얻기 */
	for (; i<arrCarrID.GetCount(); i++)
	{
		/* ICxSubstratePtr 포인터 얻기 */
		if (!g_pE87CMS->GetSubstratePtrFromSource(arrCarrID[i].GetBuffer(),
												  arrSlotNo[i], pICxSubstPtr))	break;

		/* Slot Number 위치에 해당되는 Substrate ID 얻기 */
		wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);
		if (!g_pE90STS->GetSubstrateID(pICxSubstPtr, tzSubstID, CIM_SUBSTRATE_ID))	break;

		/* Substrate ID의 Process 상태 값 얻기 */
		if (!g_pE90STS->GetSubstrateProcessingState(tzSubstID, enState))	break;

		/* Substrate ID의 Transport 상태 값 얻기 */
		if (!g_pE90STS->GetSubstrateTransportState(tzSubstID, trpState))	break;

		/* Substrate의 transport state값이 E90_SSTS::en_work, 즉 at work 이거나 */
		/* process state가 needs processing 내지 in process 이면 FALSE 반환 */
		if (E90_SSTS::en_work == trpState ||
			!((E90_SSPS::en_needs_processing != enState) && (E90_SSPS::en_in_process != enState)))
			return bSucc; // Return false, 여전히 기다려야하는 substrates가 있음	
	}

	/* 모두 처리 했는지 여부에 따라 성공 여부 */
	bSucc = (arrCarrID.GetCount() == i);
	if (!bSucc)	return FALSE;

	/* 모두 체크해서 해당 없이 문제 없다면 PRJob부터 complete */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRJobComplete::get_ParentControlJob process_id [%s]", pj_id);
	g_pLogData->PushLogs(tzMesg);
	hResult = g_pE40PJM->SetPRJobComplete(pj_id);
	if (g_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Parent control job id 를 가져오고*/
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobID from process_id [%s]", pj_id);
	g_pLogData->PushLogs(tzMesg);
	hResult = g_pE40PJM->GetControlJobID(pj_id, tzCRJobID, CIM_CONTROL_JOB_ID_SIZE);
	if (g_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Process Job Complete 처리 (E94CJM) */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"E94CJM->SetPRJobComplete process_id / control_id : [%s] / [%s]",
			   pj_id, (LPCTSTR)_bstr_t(tzCRJobID));
	g_pLogData->PushLogs(tzMesg);
	hResult = g_pE94CJM->SetProcessJobComplete(tzCRJobID, pj_id);
	if (g_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 메모리 해제  */
	arrSlotNo.RemoveAll();

	return bSucc;
}

/*
 desc : This method is called to start the next ProcessJob/ControlJob before the currently
		running ProcessJobs/ControlJobs run out of material to process. 
		이 함수는 현재 돌아가는 ProcessJobs/ControlJobs가 material을 다 진행하고 끝내기 전에 미리 파악하여 
		다음 ProcessJobs/ControlJobs를 시작할 수 있도록 준비하는 것임. 즉 cascading 방식으로 구현해둔 것임. 
		이 방식은 cascade, sequential, concurrent등이 있고  Philoptics의 장비 특성에 맞는 선택에 따라 변경 될 수 있음. 
		현재의 ProcessJobs/ControlJobs들이 완전히 다 끝나고 다음 Job이 그 후에 시작하는 방식을 sequential 이라고 부름.
		-참고 조건: 1) 모든 CJ를 가져와서 SELECTED가 없어야 함, 있다면 이 함수는 그냥 리턴.
					2) 현재 실행중인 PJ의 마지막 SlotID를 가져와서 SubstrateID로 변환하고 그 SubstrateID의 LocationID를 가져옴. 
						Subst.ID: CARRIER1.03 이런 형식
					3) IF: 현재 CJ의 모든 PJ를 확인하여 Queued인 PJ가 있다면 ControlJob.StartNextProcessJob() 실행
					4) ELSE: 모든 CJ를 가져와서 Queued인 CJ가 있다면 CIM94.StartNextControlJob() 실행

		[중요] 처음 Job이 시작될때도 이 함수로 체크해야함. 
 parm : pj_id	- [in]  Process Job ID
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_Mixed_CheckStartNextJob(PTCHAR pj_id)
{
	BOOL bQueuedCRJobID = FALSE, bQueuedPRJobID	= FALSE;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]			= { NULL };
	TCHAR tzCRJobID[CIM_CONTROL_JOB_ID_SIZE]= { NULL };
	HRESULT hResult;
	E40_PPJS enState;
	CStringArray arrActiveIDs, arrQueuedIDs;
	CAtlList <E94_CCJS> lstActiveState, lstQueuedState;
	CStringList lstPRJobIDs;

	uvCIMLib_E94CJM_GetControlJobListIdState(arrActiveIDs, lstActiveState, E94_CLCJ::en_list_active);
	uvCIMLib_E94CJM_GetControlJobListIdState(arrQueuedIDs, lstQueuedState, E94_CLCJ::en_list_queued);
	if (lstQueuedState.GetCount() > 0)	bQueuedCRJobID = TRUE;

	/* Parent control job id 를 가져오고 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobID from process_id [%s]", pj_id);
	g_pLogData->PushLogs(tzMesg);
	hResult = g_pE40PJM->GetControlJobID(pj_id, tzCRJobID, CIM_CONTROL_JOB_ID_SIZE);
	if (g_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	
	/* Process Jobs 중에서 Queued 상태인 것이 있는지 확인함 (CJ에 해당하는 PJ list 얻음) */
	uvCIMLib_E94CJM_GetControlJobProcessJobList(tzCRJobID, lstPRJobIDs);  
	/* PJ list 중에 Queued 상태인것이 있는지 확인 */
	for (int i = 0; i < lstPRJobIDs.GetCount(); i++)
	{
		/* 현재 Process Job State 값 얻기 */
		hResult = g_pE40PJM->GetProcessJobState(pj_id, enState);
		if (g_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
		if (enState == E40_PPJS::en_queued)	bQueuedPRJobID = TRUE;
	}

	if (arrActiveIDs.GetCount() == 0)	/* 상태가 SELECTED(ACTIVE)인 CJ가 없는경우, 참고조건 2)3)4) */
										/*        확인에 맞춰 startNextProcessJob() 호출함          */
	{
		/* CJ에 해당하는 모든 PJs 상태를 보고 QUEUED 일 경우 ControlJob.StartNextProcessJob() 실행 */
		if (bQueuedPRJobID)
		{
			hResult = g_pE94CJM->StartNextProcessJob(tzCRJobID);
			if (g_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
		}
		else
		{
			/* 모든 CJs중에 QUEUED 가 있다면 CIM94.StartNextControlJob() 실행 */
			if (bQueuedCRJobID)
			{
				hResult = g_pE94CJM->StartNextControlJob();
				if (g_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
			}
		}
	}
	else	/* ACTIVE 상태인 CJ가 있는 경우, 그 안에 SELECTED가 있는지 확인 후 있다면 아무것도 안함 */
			/*   (active 안에 selected외에 waiting for start, executing, paused등이 존재한다면?)    */
	{
		for (int i = 0; i < lstActiveState.GetCount(); i++)
		{
			if (lstActiveState.GetAt(lstActiveState.FindIndex(i)) == E94_CCJS::en_selected)
			{
				swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"ItfcCIMLib::uvCIMLib_Mixed_CheckStartNextJob(), "
													  L"SELECTED Control Job existing. Do nothing.");
				g_pLogData->PushLogs(tzMesg);
			}
		}
	}

	arrActiveIDs.RemoveAll();
	lstActiveState.RemoveAll();
	arrQueuedIDs.RemoveAll();
	lstQueuedState.RemoveAll();

	return TRUE;
}

/*
 desc : When a substrate completes processing, check to see if all of the other substrates in the ProcessJob
		have completed processing. If they have completed, then transition the ProcessJob state from PROCESSING
		to PROCESS COMPLETE (transition #6).
		Substrates complete processing while they are still in the equipment. CxE40PJM.PRJobComplete() and
		CxE94CJM.ProcessJobComplete() will be called when all of the substrates for the ProcessJob have moved
		to their final carrier locations. See SubstTracking_SubstratePlacedToDestination() method.
 parm : pj_id	- [in]  Process Job ID
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_Mixed_CheckProcessJobProcessingComplete(PTCHAR pj_id)
{
	HRESULT hResult;
	ICxValueDispPtr pICxStatus(CLSID_CxValueObject);

	if (!uvCIMLib_Mixed_IsSubstrateProcessCompleted(pj_id))	return FALSE;
	g_pLogData->PushLogs(L"m_ICxE40PJM::PRJobProcessComplete");
	hResult = g_pE40PJM->GetE40PJMPtr()->PRJobProcessComplete(CComBSTR(pj_id), pICxStatus);
	if (g_pLogData->CheckResult(hResult, L"m_ICxE40PJM::PRJobProcessComplete"))	return FALSE;

	return TRUE;
}

#ifdef __cplusplus
}
#endif
