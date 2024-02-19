
/*
 desc : CIM Library �� ���� ���� ���α׷� ���� GEM300 ���� ������ �� ��Ÿ ���� ���� ���� ���̺귯��
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
/*                                           ���� ����                                           */
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

CAtlList <STG_TVCI>		g_lstValChangeID;	/* EPJ ������Ʈ ���� ���� ���� ����Ǹ� PCIMLib���� �˸��� ���� �̺�Ʈ ����Ʈ */
CAtlList <STG_TSMI>		g_lstSECSMsgID;		/* SECS-II Message ID �̺�Ʈ ����Ʈ */


/* --------------------------------------------------------------------------------------------- */
/*                                           �ݹ� �Լ�                                           */
/* --------------------------------------------------------------------------------------------- */

#if (USE_SIMULATION_CODE)
/* --------------------------------------------------------------------------------------------- */
/*                             Simulation code area for Cimetrix                                 */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
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
/*                                           ���� �Լ�                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ICxSubstrate ����Ʈ ���
 parm : pj_id		- [in]  Process Job ID
		subst_ptr	- [out] ICxSubstratePtr ��ü ����Ʈ�� ����� ����
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
	/* PRJob ID�� �ش�Ǵ� Slot Number�� Carrier ID ����Ʈ ��� */
	if (!g_pE40PJM->GetProcessJobAllSlotNo(pj_id, arrSlotNo, arrCarrID))	return FALSE;
	/* Carrier ID�� Slot No ������ Substate ��� */
	for (; i<arrSlotNo.GetCount(); i++)
	{
		if (!g_pE87CMS->GetSubstratePtrFromSource(arrCarrID[i].GetBuffer(),
												  arrSlotNo[i], pICxSubstPtr))	return FALSE;
		/* ����Ʈ�� �߰� */
		subst_ptr.Add(pICxSubstPtr);
	}

	return TRUE;
}

/*
 desc : ICxSubstrate for E90STS ��ü ������ ���
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		st_ptr	- [out] ICxSubstratePtr ��ü�� ��ȯ�� ����
 retn : TRUE or FALSE
*/
BOOL GetSubstToE90SubstratePtr(PTCHAR subst_id, ICxSubstratePtr &st_ptr)
{
	if (!!g_pE90STS)	return FALSE;
	if (!g_pE30GEM->IsInitedAll(TRUE))	return FALSE;
#if 1
	/* PRJob ID�� �ش�Ǵ� Slot Number�� Carrier ID ����Ʈ ��� */
	if (!g_pE90STS->GetSubstrateIDPtr(subst_id, &st_ptr))	return FALSE;
#endif
	return TRUE;
}

/*
 desc : ICxSubstrate for E90STS ��ü ������ ���
 parm : carr_id	- [in]  Carrier ID
		slot_no	- [in]  Carrier ���� ��ġ�� Slot Number
		st_ptr	- [out] ICxSubstratePtr ��ü�� ��ȯ�� ����
 retn : TRUE or FALSE
*/
BOOL GetSubstToE87SubstratePtr(PTCHAR carr_id, UINT8 slot_no, ICxSubstratePtr &st_ptr)
{
	if (!!g_pE87CMS)	return FALSE;
	if (!g_pE30GEM->IsInitedAll(TRUE))	return FALSE;
	/* PRJob ID�� �ش�Ǵ� Slot Number�� Carrier ID ����Ʈ ��� */
	if (!g_pE87CMS->GetSubstratePtrFromSource(carr_id, slot_no, st_ptr))	return FALSE;

	return TRUE;
}

/*
 desc : Substrate ID ��ȯ
 parm : carr_id	- [in]  Carrier ID
		slot_no	- [in]  Carrier ���� ��ġ�� Slot Number
		subst_id- [out] Substrate ID�� ��ȯ�� ����
		size	- [in]  'subst_id'�� ���� ũ��
 retn : TRUE or FALSE
*/
BOOL GetSubstToE87SubstrateID(PTCHAR carr_id, UINT8 slot_no,
							  PTCHAR subst_id, UINT32 size)
{
	CComBSTR bstrSubstID;
	ICxSubstratePtr pICxSubstPtr;

	if (!!g_pE87CMS)	return FALSE;
	if (!g_pE30GEM->IsInitedAll(TRUE))	return FALSE;
	/* PRJob ID�� �ش�Ǵ� Slot Number�� Carrier ID ����Ʈ ��� */
	if (!g_pE87CMS->GetSubstratePtrFromSource(carr_id, slot_no, pICxSubstPtr))	return FALSE;
	if (!pICxSubstPtr->get_objectID(&bstrSubstID))	return FALSE;

	/* Substrate ID ��ȯ */
	swprintf_s(subst_id, size, L"%s", (LPTSTR)_bstr_t(bstrSubstID));

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                          TELCIM.INF                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : TELCIM.INF ���Ͽ��� [ValueChangedID] ����� ���� �ӽ� ����Ʈ ���� �޸𸮿� ����
 parm : work_dir	- [in]  ���� ������ �����ϴ� ��� (���μ��� �۾� ���)
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

	/* telcim.inf ���� ��ü ��� ���� ��� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\info\\%s", work_dir, TEL_CIM_CONFIG);

	/* ValueChangedID ���� ��� */
	u16Count	= GetPrivateProfileInt(tzSubj, L"COUNT", 0, tzFile);
	if (u16Count < 1)	return FALSE;

	/* SECS-II.Message.ID ���� ��� */
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

		/* ���� �� �ʱ�ȭ */
		memset(&stData, 0x00, sizeof(STG_TVCI));
		wmemset(tzConn, 0x00, 8);
		wmemset(tzVID, 0x00, 8);
		ptzValue	= tzValue;

		/* ------------------------------------------------------- */
		/* Connection ID, Variable ID, Variable Name �и��Ͽ� ���� */
		/* ------------------------------------------------------- */

		/* Connection ID ��� */
		i32Len		= (INT32)wcslen(tzValue);
		i32Find		= uvCmn_GetFindCh(ptzValue, i32Len, L',');
		if (i32Find < 0 && i32Find > 8)
		{
			g_lstValChangeID.RemoveAll();
			return FALSE;
		}
		wmemcpy(tzConn, ptzValue, i32Find);	ptzValue += i32Find + 1;

		/* Value ID ��� */
		i32Len		= (INT32)wcslen(tzValue);
		i32Find		= uvCmn_GetFindCh(ptzValue, i32Len, L',');
		if (i32Find < 0 && i32Find > 8)
		{
			g_lstValChangeID.RemoveAll();
			return FALSE;
		}
		wmemcpy(tzVID, ptzValue, i32Find);	ptzValue += i32Find + 1;

		/* Variable Name ��� */
		wmemcpy(stData.var_name, ptzValue, i32Len - (i32Find + 1));

		/* ����Ʈ �޸� ���ۿ� ��� */
		stData.conn_id	= (UINT32)_wtoi(tzConn);
		stData.var_id	= (UINT32)_wtoi(tzVID);
		g_lstValChangeID.AddTail(stData);
	}

	return TRUE;
}

/*
 desc : TELCIM.INF ���Ͽ��� [ValueChangedID] ����� ���� �ӽ� ����Ʈ ���� �޸𸮿� ����
 parm : work_dir	- [in]  ���� ������ �����ϴ� ��� (���μ��� �۾� ���)
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

	/* telcim.inf ���� ��ü ��� ���� ��� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\info\\%s", work_dir, TEL_CIM_CONFIG);

	/* ValueChangedID ���� ��� */
	u16Count	= GetPrivateProfileInt(tzSubj, L"COUNT", 0, tzFile);
	if (u16Count < 1)	return FALSE;

	/* ValueChangedID ���� ��� */
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

		/* ���� �� �ʱ�ȭ */
		memset(&stData, 0x00, sizeof(STG_TSMI));
		wmemset(tzConn, 0x00, 8);
		wmemset(tzSID, 0x00, 8);
		wmemset(tzFID, 0x00, 8);
		ptzMesg	= tzMesg;

		/* ------------------------------------------------------- */
		/* Connection ID, Variable ID, Variable Name �и��Ͽ� ���� */
		/* ------------------------------------------------------- */

		/* Connection ID ��� */
		i32Len		= (INT32)wcslen(tzMesg);
		i32Find		= uvCmn_GetFindCh(ptzMesg, i32Len, L',');
		if (i32Find < 0 && i32Find > 8)
		{
			g_lstSECSMsgID.RemoveAll();
			return FALSE;
		}
		wmemcpy(tzConn, ptzMesg, i32Find);	ptzMesg += i32Find + 1;

		/* Stream ID ��� */
		i32Len		= (INT32)wcslen(tzMesg);
		i32Find		= uvCmn_GetFindCh(ptzMesg, i32Len, L',');
		if (i32Find < 0 && i32Find > 8)
		{
			g_lstSECSMsgID.RemoveAll();
			return FALSE;
		}
		wmemcpy(tzSID, ptzMesg, i32Find);	ptzMesg += i32Find + 1;

		/* Function ID ��� */
		i32Len		= (INT32)wcslen(tzMesg);
		i32Find		= uvCmn_GetFindCh(ptzMesg, i32Len, L',');
		if (i32Find < 0 && i32Find > 8)
		{
			g_lstSECSMsgID.RemoveAll();
			return FALSE;
		}
		wmemcpy(tzFID, ptzMesg, i32Find);	ptzMesg += i32Find + 1;

		/* Message Description ��� */
		if ((i32Len - (i32Find + 1)) > CIM_CMD_NAME_SIZE + 8)
		{
			g_lstSECSMsgID.RemoveAll();
			return FALSE;
		}
		wmemcpy(stData.mesg_desc, ptzMesg, i32Len - (i32Find + 1));
		/* ����Ʈ �޸� ���ۿ� ��� */
		stData.conn_id	= (UINT8)_wtoi(tzConn);
		stData.s_id		= (UINT8)_wtoi(tzSID);
		stData.f_id		= (UINT8)_wtoi(tzFID);
		g_lstSECSMsgID.AddTail(stData);
	}

	return TRUE;
}


/* --------------------------------------------------------------------------------------------- */
/*                                           �ܺ� �Լ�                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : ���̺귯�� �ʱ�ȭ
 parm : work_dir	- [in]  ���������� �����ϴ� ��� (���μ����� ���۵Ǵ� ���� ���)
		conn_cnt	- [in]  EPJ ������Ʈ ���Ͽ� ��ϵ� CONNECTIONx ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_Init(PTCHAR work_dir, UINT8 conn_cnt)
{
	/* �α� ������ ���� ��ü ���� */
	g_pLogData		= new CLogData;
	/* CIM300 Data ���� ��ü ���� */
	g_pSharedData	= new CSharedData (conn_cnt);
	ASSERT(g_pLogData && g_pSharedData);
	/* CIM300 Class Object ���� (!!! �ݵ�� ���� ��ų �� !!!) */
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
	/* CIM300 Initialization Thread ����  */
	g_pWorkThread	= new CWorkThread(g_pSharedData, g_pLogData,
									  g_pE30GEM, g_pE40PJM, g_pE87CMS, g_pE90STS, g_pE94CJM);
	ASSERT(g_pWorkThread);
	if (!g_pWorkThread->CreateThread(0, 0, SLOW_THREAD_SPEED))
	{
		AfxMessageBox(L"Can't run the cim300 work_thread for CIM300", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
#endif

	/* ValueChangedID �� ���� */
	if (!LoadValueChangedID(work_dir) || !LoadSECSMessageID(work_dir))
	{
		AfxMessageBox(L"Failed to load LoadValueChangedID or LoadSECSMessageID in the telcim.inf",
					  MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : ���̺귯�� ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT VOID uvCIMLib_Close()
{
#if (USE_WORK_THREAD)
	/* Initialization Thraed �޸� ���� */
	if (g_pWorkThread)
	{
		if (g_pWorkThread->IsBusy())	g_pWorkThread->Stop();
		while (g_pWorkThread->IsBusy())	Sleep(100);
		delete g_pWorkThread;
		g_pWorkThread	= NULL;
	}
#endif

	/* ��ü �޸� ����  (!!! �ݵ�� ���� ��ų �� !!!) */
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

	/* ����Ʈ �޸� ���� */
	g_lstValChangeID.RemoveAll();
	g_lstSECSMsgID.RemoveAll();
}

/*
 desc : CIM300 Connect ���� (�ʱ�ȭ) ���� ����
 parm : None
 retn : None
*/
API_EXPORT VOID uvCIMLib_Stop()
{
	/* �ݵ�� ���� �ʱ�ȭ ȣ���� ��� �� */
	if (g_pE30GEM)	g_pE30GEM->ResetInitFlag();

	/* CIM300 Class Object ���� (!!! �ݵ�� ���� ��ų �� !!!) */
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
 desc : CIM300 Connect ���� (�ʱ�ȭ) ����
 parm : equip_id	- [in]  Equipment ID (0 or later. ȯ�� ���Ͽ� ������ ����-1)
		conn_id		- [in] 1 or Later (���� ������ ���ؼ��� 0 �� ���)
		epj_file	- [in]  EPJ ���� (��ü ���)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_Start(LONG equip_id, LONG conn_id, PTCHAR epj_file)
{
	if (!g_pE30GEM)	return FALSE;

	/* ������ �ʱ�ȭ�� ����� ���� ������� ���� */
	if (g_pE30GEM->IsInitConnected())
	{
		uvCIMLib_Stop();
#ifndef _DEBUG
		g_pE30GEM->WaitOnEMServiceIdle();
#endif
	}

	/* �ʱ�ȭ ���� (!!! �ݵ�� ���� ��ų �� !!!) */
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

	/* EPJ (������Ʈ) ���� ���� */
	if (!g_pE30GEM->LoadProject(epj_file))		return FALSE;
	/* ����� ���� ���� �� ��� (���� ����� ��, �̺�Ʈ ����) */
	if (!g_pE30GEM->RegisterValueChangeID(&g_lstValChangeID))	return FALSE;
	/* ����� ���� SECS-II Message �� ���*/
	if (!g_pE30GEM->RegisterMessageID(&g_lstSECSMsgID))			return FALSE;

	return TRUE;
}

/*
 desc : CIM300 Connection (�ʱ�ȭ �Ϸ�) ���� Finalized ȣ��
 parm : equip_id	- [in]  Equipment ID (0 or later. ȯ�� ���Ͽ� ������ ����-1)
		conn_id		- [in] 1 or Later (���� ������ ���ؼ��� 0 �� ���)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_Finalized(LONG equip_id, LONG conn_id)
{
	UINT16 i	= 0x00;

	if (!g_pE30GEM)									return FALSE;
	if (!g_pE30GEM->IsInitConnected())				return FALSE;

	/* �Ϸ� �۾� ���� (!!! �ݵ�� ���� ��ų �� !!!) */
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
 desc : TS (Tracking System)�� ��ϵ� Equipment Module (Location) Name ��ȯ
 parm : index	- [in]  ��� ��� ���ε� �ε��� �� (1 based)
 retn : �̸� ���ڿ� (������ ��� NULL or Empty String)
*/
API_EXPORT PTCHAR uvCIMLib_GetEquipLocationName(ENG_TSSI index)
{
	if (!g_pE30GEM)	return FALSE;
	return g_pE30GEM->GetEquipLocationName(index);
}

/*
 desc : ���� ����Ǿ� �ִ� ������Ʈ ������ ���Ϸ� ����
 parm : epj_file	- [in]  �����ϰ��� �ϴ� ���� �̸� (��ü ��� ����)
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
 desc : EMService�� ���� ������ ����
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
 desc : CIMConnection�� ����Ǿ� �ִ��� ����
 parm : logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsCIMConnected(BOOL logs)
{
	if (!g_pE30GEM)	return FALSE;
	return g_pE30GEM->IsInitConnected(logs);
}

/*
 desc : CIM Lib. �ʱ�ȭ ���� ��ȯ
 parm : logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsInitedAll(BOOL logs)
{
	if (!g_pE30GEM)	return FALSE;
	return g_pE30GEM->IsInitedAll();
}

/*
 desc : �ý��ۿ��� �߻��� �α� ��ȯ
 parm : logs	- [in]  ��ȯ�� �α� ������ ����� ����ü ������
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_PopLogs(STG_CLLM &logs)
{
	if (!g_pE30GEM)		return FALSE;
	if (!g_pLogData)	return FALSE;
	return g_pLogData->PopLogs(logs);
}

/*
 desc : Recipe Path ����
 parm : path	- [in]  Recipe Path (��ü ��� ����. �������� '/' ����)
 		conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		var_id	- [in]  Identification (Variables Name�� ����� ID)
		var_name- [in]  Identification (Variables ID�� ����� Name)
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
 desc : MDLN �� ����
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
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
 desc : SOFTREV �� ����
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
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
 desc : Recip Path ��ȯ
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		path	- [out] Recipe Names (Files)�� ����� ��ΰ� ��ȯ�� ����
		size	- [in]  'path' ���� ũ��
		var_id	- [in]  Identification (Variables Name�� ����� ID)
		var_name- [in]  Identification (Variables ID�� ����� Name)
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
 desc : Recip Name (File)�� Ȯ���� ��ȯ
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		ext		- [out] Recipe Name (File)�� Ȯ���ڰ� ��ȯ�� ����
		size	- [in]  'ext' ���� ũ��
		var_id	- [in]  Identification (Variables Name�� ����� ID)
		var_name- [in]  Identification (Variables ID�� ����� Name)
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
 desc : EPJ ID�� ����� Name ��ȯ
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		name	- [in]  �˻��� ���� Variable Name
		id		- [out] Vairable ID ��ȯ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetNameToID(LONG conn_id, PTCHAR name, LONG &id)
{
	/* ID�� ���εǴ� �̸� ��ȯ */
	if (!g_pE30GEM->GetVarNameToID(conn_id, name, id))	return FALSE;

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                         Shared Data                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ���� CIM300 (From Host)���κ��� �߻��� �̺�Ʈ ��� ����
 parm : None
 retn : None
*/
API_EXPORT VOID uvCIMLib_RemoveEventAll()
{
	if (!g_pSharedData)	return;
	g_pSharedData->RemoveEventAll();
}

/*
 desc : CIM300Cbk�κ��� �߻��� �̺�Ʈ ��ȯ
 parm : data	- [in]  ��ȯ�� �̺�Ʈ ������ ����� ����ü ������
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_PopEvent(STG_CCED &event)
{
	if (!g_pSharedData)	return FALSE;
	return g_pSharedData->PopEvent(event);
}

/*
 desc : Load Port�� Docked or Undocked Location Name ��ȯ
 parm : port_id	- [in]  Load Port ID (1 or 2)
		docked	- [in]  0x00 : Docked, 0x01 : Undocked
 retn : Docked or Undocked Location Name ��ȯ (������ ���, NULL)
*/
API_EXPORT PTCHAR uvCIMLib_GetLoadPortDockedName(UINT16 port_id, UINT8 docked)
{
	if (!g_pSharedData)	return NULL;
	return g_pSharedData->GetLoadPortDockedName(port_id, docked);
}

/*
 desc : CIM300 �α� ���� ����
 parm : level	- [in]  Level Value ��
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
 desc : ������ ���� ��û�� ���� �� �ʱ�ȭ (������ �������� ����)
 parm : None
 retn : None
*/
API_EXPORT VOID uvCIMLib_ResetPPLoadRecipeAck()
{
	if (!g_pSharedData)	return;
	g_pSharedData->ResetPPLoadRecipeAck();
}

/*
 desc : ������ ���� ��û�� ���� ���� �� ��ȯ
 parm : None
 retn : ANY Type
*/
API_EXPORT E30_HPLA uvCIMLib_GetPPLoadRecipeAck()
{
	if (!g_pSharedData)	return E30_HPLA::en_rg_reset;
	return g_pSharedData->GetPPLoadRecipeAck();
}

/*
 desc : ������ ���� ��û�� ���� ���� ���� ���ŵǾ����� ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsPPLoadRecipeAckOk()
{
	return !g_pSharedData ? NULL : g_pSharedData->IsPPLoadRecipeAckOk();
}

/*
 desc : ������ ������ �۽� (����) ��û�� ���� ���� �� �ʱ�ȭ
 parm : None
 retn : None
*/
API_EXPORT VOID uvCIMLib_ResetPPSendRecipeAck()
{
	if (!g_pSharedData)	return;
	g_pSharedData->ResetPPSendRecipeAck();
}

/*
 desc : ������ ������ �۽� (����) ��û�� ���� ���� �� ��ȯ
 parm : None
 retn : ANY Type
*/
API_EXPORT E30_SRHA uvCIMLib_GetPPSendRecipeAck()
{
	if (!g_pSharedData)	return E30_SRHA::en_ra_reset;
	return g_pSharedData->GetPPSendRecipeAck();
}

/*
 desc : ������ ������ �۽� (����) ��û�� ���� ���� ���� �ִ��� ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsPPSendRecipeAckOk()
{
	return !g_pSharedData ? NULL : g_pSharedData->IsPPSendRecipeAckOk();
}

/*
 desc : �͹̳� �޽��� �۽� (����) ��û�� ���� ���� �� �ʱ�ȭ
 parm : None
 retn : None
*/
API_EXPORT VOID uvCIMLib_ResetTermMsgAck()
{
	if (!g_pSharedData)	return;
	g_pSharedData->ResetTermMsgAck();
}

/*
 desc : �͹̳� �޽��� �۽� (����) ��û�� ���� ���� �� ��ȯ
 parm : None
 retn : ANY Type
*/
API_EXPORT E30_STHA uvCIMLib_GetTermMsgAck()
{
	if (!g_pSharedData)	return E30_STHA::en_t_reset;
	return g_pSharedData->GetTermMsgAck();
}

/*
 desc : �͹̳� �޽��� �۽� (����) ��û�� ���� ������ �ִ��� ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsTermMsgAckOk()
{
	return !g_pSharedData ? NULL : g_pSharedData->IsTermMsgAckOk();
}

/*
 desc : Host�κ��� Recipe File ��û (����)�� ���� ���� �ʱ�ȭ
 parm : None
 retn : None
*/
API_EXPORT VOID uvCIMLib_ResetRecipeFileAck()
{
	if (!g_pSharedData)	return;
	g_pSharedData->ResetRecipeFileAck();
}

/*
 desc : Host�κ��� Recipe File ��û (����)�� ���� ���� �� ��ȯ
 parm : None
 retn : ANY Type
*/
API_EXPORT E30_RMJC uvCIMLib_GetRecipeFileAck()
{
	if (!g_pSharedData)	return E30_RMJC::en_reset;
	return g_pSharedData->GetRecipeFileAck();
}

/*
 desc : ������ (���μ��� ���α׷�; Process Program == Recipe) ��� / ���� / ��ü ����
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		file	- [in]  ������ �̸� (��ü ��ΰ� ���Ե� ����)
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
 desc : ���� ��ϵ� Process Job ID ���� Substrate IDs�� ���°� ��� �Էµ� ���¿� �������� ����
 parm : pj_id	- [in]  �˻� ����� Process Job ID
		machine	- [in]  Substrate Machine Type ��
		state	- [in]  �� ����� Substrate Processing State ��
 retn : �Էµ� state ���� ��� �����ϴٸ� TRUE, �� �� �̻� �ٸ� ���� �����Ѵٸ� FALSE
 */
API_EXPORT BOOL uvCIMLib_IsCheckedAllPRJobSubstState(PTCHAR pj_id, E90_VSMC machine, INT32 state)
{
	if (!g_pSharedData)	return FALSE;
	return g_pSharedData->IsCheckedAllPRJobSubstState(pj_id, machine, state);
}

/*
 desc : ���� ��ϵ� Process Job ID ���� Substrate IDs�� ���°� �Էµ� ���¿� ������ ���� 1���� �ִ��� ����
 parm : pj_id	- [in]  �˻� ����� Process Job ID
		machine	- [in]  Substate Machine Type ��
		state	- [in]  �� ����� Substrate Processing State ��
 retn : �Էµ� state ���� ������ ���� ���� �� ���� �����ϸ� TRUE
		�Էµ� state ���� ������ ���� ���� �� ���� ���ٸ� FALSE
 */
API_EXPORT BOOL uvCIMLib_IsCheckedOnePRJobSubstState(PTCHAR pj_id, E90_VSMC machine, INT32 state)
{
	if (!g_pSharedData)	return FALSE;
	return g_pSharedData->IsCheckedOnePRJobSubstState(pj_id, machine, state);
}

/*
 desc : ���� ��ġ�� ��ϵ� PRJob IDs�� ��ϵ� Substrate IDs�� ��� ���� ������ ��ȯ
 parm : pj_id	- [in]  �˻� ����� Process Job ID
		subst	- [out] Substrate ID�� ���� �� ��Ÿ �������� ����� ����Ʈ �޸� (PRCommandCbk �Լ����� �ַ� ����?)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetSubstIDStateAll(PTCHAR pj_id, CAtlList <STG_CSDI> &lst_state)
{
	if (!g_pSharedData)	return FALSE;
	return g_pSharedData->GetSubstIDStateAll(pj_id, lst_state);
}

/*
 desc : Carrier ID ����
 parm : carr_id	- [in]  ���� ����� Carrier ID
 retn : TRUE (���� ����), FALSE (���� ����)
*/
API_EXPORT BOOL uvCIMLib_RemoveSharedCarrID(PTCHAR carr_id)
{
	if (!g_pSharedData)	return FALSE;
	return g_pSharedData->RemoveSharedCarrID(carr_id);
}

/*
 desc : ���� ��ϵ� SECS-II Message ��ȯ
 parm : mesg_id	- [in]  �˻� ����� Message ID �� (Stream/Function���� ������ 16���� Hexa-Decimal ��)
		remove	- [in]  �˻��� ����� ��ȯ �� �޸𸮿��� �������� ����
		item	- [out] �˻��� �����Ͱ� ��ȯ�� ����Ʈ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetSecsMessage(LONG mesg_id, BOOL remove, CAtlList <STG_TSMV> &item)
{
	if (!g_pSharedData)	return FALSE;
	return g_pSharedData->GetSecsMessage(mesg_id, remove, item);
}

/* 
 desc : ���� ��ϵ� ��� SECS-II Message �޸� ����
 parm : None
 retn : None
*/
API_EXPORT VOID uvCIMLib_RemoveAllSecsMessage()
{
	if (g_pSharedData)	g_pSharedData->RemoveAllSecsMessage();
}


#if 0	/* ���� �� �ʿ� ����. ���߿� �Ǵ��� */
/* --------------------------------------------------------------------------------------------- */
/*                         Operation Job Step [Sequencial Working]                               */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ���� �۾��� �Ϸ�� �������� ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsWorkCompleted()
{
	return g_pWorkThread->IsWorkCompleted();
}

/*
 desc : ���� �۾��� ���е� �������� ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsWorkFailed()
{
	return g_pWorkThread->IsWorkFailed();
}

/*
 desc : ���� �۾��� ��� �������� ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsIdledWork()
{
	return g_pWorkThread->IsWorkIdled();
}

/*
 desc : E30GEM �ʱ�ȭ ����
 parm : None
 retn : TRUE or FALSE
 note : EPJ �ε� ����, ȣ��Ǿ�� ��
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

				/* Carrier ������ �۾��� ��� (?) */
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

					/* Process Job Complete ó�� (E94CJM) */
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
 desc : ���� Variable�� Value �� ���
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		var_id	- [in]  Variable ID
		var_name- [in]  Variable Name
		value	- [out] Value ���� ����Ǵ� ���� ������
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID; 1 or 2)
		index	- [in]	�Ϲ������� 0 ��������, ����Ʈ�� ��� Slot ID (Wafer ID; 1 ~ Max)
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
 desc : ���� Variable�� Value ���� ���ڿ��� ��ȯ
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		var_id	- [in]  Variable ID
		var_name- [in]  Variable Name (NULL�� �ƴϸ�, ������ name �������� �˻�)
		value	- [out] Value ���� ����Ǵ� ���� ������
		size	- [in]  'value' ���� ũ��
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID; 1 or 2)
		index	- [in]	�Ϲ������� 0 ��������, ����Ʈ�� ��� Slot ID (Wafer ID; 1 ~ Max)
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
 desc : ���� CIMConnect�� ���� �� �����Ǵ� Variable�� Cache �� ��ȯ
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		var_id	- [in]  Variable ID
		var_name- [in]  Variable name
		value	- [out] ��ȯ�Ǿ� ����� ����
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID; 1 or 2)
		index	- [in]	�Ϲ������� 0 ��������, ����Ʈ�� ��� Slot ID (Wafer ID; 1 ~ Max)
 retn : Name ���ڿ� ������ ��ȯ (�˻� ������ ���, -1L)
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
 desc : ���� CIMConnect�� ���� �� �����Ǵ� Variable�� Cache ���� ���ڿ� ��ȯ
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		var_id	- [in]  Identification (Variables Name�� ����� ID)
		value	- [out] �ӽ� �����Ͱ� ����� ���� ������ (�Ҵ�� �޸𸮰� �ƴ�)
		size	- [in]  'value' ������ ũ��
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID; 1 or 2)
		index	- [in]	�Ϲ������� 0 ��������, ����Ʈ�� ��� Slot ID (Wafer ID; 1 ~ Max)
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
 desc : CIMConnect ������ ĳ�� ���ۿ� �� ����
 parm : conn_id	- [in] 1 or Later (���� ������ ���ؼ��� 0 �� ���)
		type	- [in]  �ԷµǴ� �� (Value)�� Type
		var_id	- [in]  Variable ID
		var_name- [in]  Variable Name
		value	- [in]  �� (type�� ����, �������� ������ ��)
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID; 1 or 2)
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
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		var_id	- [in]  Variable ID
		var_name- [in]  Variable Name
		value	- [in]  variable value
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID;1 or 2)
		index	- [in]	�Ϲ������� 0 ��������, ����Ʈ�� ��� Slot ID (Wafer ID;1 ~ Max)
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
 desc : TS (Tracking System)�� ��ϵ� Processing Name ��ȯ
 parm : index	- [in]  ��� ��� ���ε� �ε��� �� (!!! -1 based !!!)
 retn : �̸� ���ڿ� (������ ��� NULL or Empty String)
*/
API_EXPORT PTCHAR uvCIMLib_GetEquipProcessingName(E90_SSPS index)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->GetEquipProcessingName(index);
}

/*
 desc : CIMConnect ���ο� �α� ���� ����
 parm : value	- [out] TRUE : Saved, FALSE : No save
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetLogToCIMConnect(BOOL &value)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pICCAL->GetLogToCIMConnect(value);
}

/*
 desc : CIMConnect ���ο� �α� ���� ���� ����
 parm : value	- [in]  TRUE : Saved, FALSE : No save
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetLogToCIMConnect(BOOL value)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pICCAL->SetLogToCIMConnect(value);
}

/*
 desc : SECS-II Message ��� ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_ResetSecsMesgList()
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->ResetSecsMesgList();
}

/*
 desc : SECS-II Message ��� - Root Message
 parm : type	- [in]  Message Type
		value	- [in]  Message Type�� ���� ��
		size	- [in]  'value' ���� ũ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_AddSecsMesgRoot(E30_GPVT type, PVOID value, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->AddSecsMesgRoot(type, value, size);
}

/*
 desc : SECS-II Message ��� - Root Message
 parm : handle	- [in]  List�� �߰��� ���� Parent ID
						�� ó�� �߰��� List�� ���, 0 ��
		clear	- [in]  Message ���� ��� ���� ���� (�����̸� TRUE, �߰��̸� FALSE)
 retn : ���� �߰��� Handle �� ��ȯ (���� ���̸�, ���� ó��)
*/
API_EXPORT LONG uvCIMLib_AddSecsMesgList(LONG handle, BOOL clear)
{
	if (!g_pE30GEM->IsInitedAll())	return -1;
	return g_pE30GEM->AddSecsMesgList(handle);
}

/*
 desc : ���� ��ġ (Host?)���� �۽ŵ� SECS-II Message ���
 parm : handle	- [in]  Handle to a list within the object, or 0
		type	- [in]  'value' ���� ����� ���� ����
		value	- [in]  The value to append
		size	- [in]  'value' ���ۿ� ����� �������� ũ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_AddSecsMesgValue(LONG handle, E30_GPVT type, PVOID value, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->AddSecsMesgValue(handle, type, value, size);
}

/*
 desc : ���� ��ġ (Host?)���� SECS-II Message �۽�
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		mesg_sid- [in]  �۽� ����� SECS-II Message ID (Stream ID) (10����)
		mesg_fid- [in]  �۽� ����� SECS-II Message ID (Function ID) (10����)
		trans_id- [in]  �۽� �� ���� ���� ��, ����ȭ�� ���� �ʿ��� ID (���� ���� ������ �������� �˱� ���� �ĺ� ID) (0 �� �ʱ�ȭ)
		is_reply- [in]  ���� (Ack) ��û ����. TRUE - Host�κ��� �޽��� ���ſ� ���� ���� ��û, FALSE - ���� �ʿ� ����
		is_body	- [in]  Message Body�� �����ϴ��� ���� ��, Message ID�� �����Ѵٸ� FALSE, ���� Message�� �����Ѵٸ� (TRUE)
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
 parm : conn_id	- [in] 1 or Later (���� ������ ���ؼ��� 0 �� ���)
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
 parm : conn_id	- [in] 1 or Later (���� ������ ���ؼ��� 0 �� ���)
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
 parm : conn_id	- [in] 1 or Later (���� ������ ���ؼ��� 0 �� ���)
		state	- [in]  if true, go ONLINE. if false, go OFFLINE
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetControlOnline(LONG conn_id, BOOL state)
{	 
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SetControlOnline(conn_id, state);
}

/*
 desc : Spooling Overwrite ����
 parm : conn_id		- [in]  ����� ��� ID
		overwrite	- [in]  Overwrite (TRUE) or Not overwrite (FALSE)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetSpoolOverwrite(LONG conn_id, BOOL overwrite)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SetSpoolOverwrite(conn_id, overwrite);
}

/*
 desc : Spooling State ����
 parm : conn_id	- [in]  ����� ��� ID
		enable	- [in]  Enable (TRUE) or Disable (FALSE)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetSpoolState(LONG conn_id, BOOL enable)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SetSpoolState(conn_id, enable);
}

/*
 desc : Spooling State ����
 parm : conn_id	- [in]  ����� ��� ID
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
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		alarm_id- [in]  Connection Variables or Alarms�� Section�� ������ variables identification number
		state	- [in]  TRUE (ENABLE) or FALSE (DISABLE)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetAlarmsEnabled(LONG conn_id, LONG alarm_id, BOOL state)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SetAlarmsEnabled(conn_id, alarm_id, state);
}

/*
 desc : ���� CIMConnect ���ο� �߻��� �˶� ��� ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_AlarmClearAll()
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->AlarmClearAll();
}

/*
 desc : ���� CIMConnect ���ο� �߻��� �˶� ��� ����
 parm : var_id	- [in]  Alarm ID ��, EPJ Variables ID�� ���� ��
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
 desc : ���� ��� (�߻�)�� �˶� ID ���� ��ȯ
 parm : id		- [out] �˶� ����Ʈ�� ����� CAtlList <LONG> : �˶� ID
		state	- [out] �˶� ����Ʈ�� ����� CAtlList <LONG>: �˶� ���� (0 or 1)
		alarm	- [in]  �˶��� �߻��� ������ ��ȯ ���� (FLALSE:  ���, TRUE: �˶��� �߻��� �͸�)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetAlarmList(CAtlList <LONG> &id, CAtlList <LONG> &state, BOOL alarm)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetAlarmList(id, state, alarm);
}

/*
 desc : ���� �˶� ���� �� ��ȯ
 parm : id		- [in]  �˶� ID
		state	- [out] �˶� ���� ��ȯ (1 : Set, 0 : Clear)
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetAlarmState(LONG id, LONG &state)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetAlarmState(id, state);
}

/*
 desc : CIMConnect ���η� ������ �˶� �߻�
 parm : var_id	- [in]  Alarm ID ��, EPJ Variables ID�� ���� ��
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
 desc : Event �߻� (Method to trigger an event)
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
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
 desc : Trigger Event Data �߻� (�θ𿡰� �˸�) - 1 �� ���� ���� ����
 parm : conn_id	- [in] 1 or Later (���� ������ ���ؼ��� 0 �� ���)
		evt_id	- [in]  Collection Event ID
		evt_name- [in]  Collection Event Name
		var_id	- [in]  Variable ID
		var_name- [in]  Variable Name
		value	- [in]  �� �̸�
		type	- [in]  'value' type (U1, U2, U4, Bo, I1, I3, ...)
 retn : TRUE or FALSE
 note : ��, list, jis_8_string ���� ����
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
 desc : Trigger Event Data �߻� (�θ𿡰� �˸�) - n �� ���� ���� ����
 parm : conn_id	- [in] 1 or Later (���� ������ ���ؼ��� 0 �� ���)
		evt_id	- [in]  Event ID
		var_id	- [in]  Variable ID�� ����� ����Ʈ
		evt_name- [in]  Event Name
		var_name- [in]  Variable Name�� ����� ����Ʈ
		value	- [in]  Variable Value�� ����� ����Ʈ
		type	- [in]  'value' type�� ����� ����Ʈ (U1, U2, U4, Bo, I1, I3, ...)
 retn : TRUE or FALSE
 note : ��, list, jis_8_string ���� ����
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
	/* Trigger Event ����  */
	if (bSucc)	bSucc = g_pE30GEM->SetTrigEventValues(conn_id, lEvtID, lstVarIDs, value, type);
	/* Array �޸� ��ȯ */
	lstVarIDs.RemoveAll();

	return bSucc;
}

/*
 desc : Trigger Event Data �߻� (�θ𿡰� �˸�) - ���� �� ���� ���� ����
 parm : conn_id	- [in] 1 or Later (���� ������ ���ؼ��� 0 �� ���)
		evt_id	- [in]  Event ID
		var_id	- [in]  Variable ID
		evt_name- [in]  Event Name
		var_name- [in]  Variable Name
		value	- [in]  Variable Value�� ����� ����Ʈ
		type	- [in]  'value' type�� ����� ����Ʈ (U1, U2, U4, Bo, I1, I3, ...)
 retn : TRUE or FALSE
 note : ��, jis_8_string ���� ����
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
 parm : state	- [in]  ���� ���� (���� ���� : 0 (Setup), 1 (Idle), 2 (executing))
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetUpdateProcessState(E30_GPSV state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->SetUpdateProcessState(state);
}

/*
 desc : Send a terminal service acknowledge event to the host. Send to host 1 by default.
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
 retn : TRUE or FALSE
 ���� : This method triggers an equipment well-known event
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
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		term_id	- [in]  Terminal ID (0 or Later)
		mesg	- [in]  The text message
 retn : TRUE or FALSE
 ���� : ���� : This method requests that a terminal message to be sent to a connection
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
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_OperatorCommand(LONG conn_id, PTCHAR name, LONG handle)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->OperatorCommand(conn_id, name, handle);
}

/*
 desc : ��ġ�� ���ǵ� ��� ������ �׸���� ���� (�����ؼ� ����ؾ� ��)
 parm : logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetClearProject( BOOL logs)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	if (!g_pE30GEM->SetClearProject(logs))	return FALSE;
	return TRUE;
}

/*
 desc : ��� ���� �����ϴ� �˶� ID ����
 parm : alarm_id- [in]  �����ϰ��� �ϴ� �˶� ID
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetDeleteAlarm(LONG alarm_id)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SetDeleteAlarm(alarm_id);
}

/*
 desc : ��� ��û���� ������ (���?)�� ����
 parm : logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetDeleteAllRequests(BOOL logs)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SetDeleteAllRequests(logs);
}

/*
 desc : �˶� �̸��� �ش�Ǵ� �˶� ID �� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		name	- [in]  �˶� �̸�
		id		- [out] �˶� ID ��ȯ
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetAlarmID(LONG conn_id, PTCHAR name, LONG &id)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetAlarmID(conn_id, name, id);
}

/*
 desc : �˶� ID�� ���� ���� ��ȯ
 parm : alarm_id- [in]  �˶� ID
		desc	- [out] �˶� ������ ��ȯ�� ����
		size	- [in]  'desc' ���� ũ��
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetAlarmIDToDesc(LONG alarm_id, PTCHAR desc, UINT32 size)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetAlarmIDToDesc(alarm_id, desc, size);
}

/*
 desc : �˶� ID�� ���� ��Ī ��ȯ
 parm : alarm_id- [in]  �˶� ID
		name	- [out] �˶� ��Ī�� ��ȯ�� ����
		size	- [in]  'name' ���� ũ��
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetAlarmIDToName(LONG alarm_id, PTCHAR name, UINT32 size)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetAlarmIDToName(alarm_id, name, size);
}

/*
 desc : ��� ���� (Low Level Communicating state) �� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		state	- [out] ��� ����
		substate- [out] HSMS (Ethernet) ���� ����
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetCommState(LONG conn_id, E30_GCSS &state, E30_GCSH &substate)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetCommState(conn_id, state, substate);
}

/*
 desc : Host ���ῡ ���� �̸� ��, connection id�� ���εǴ� �̸� ���
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		name	- [out] ����� Host Name �� ���
		size	- [in]  'name' ���� ũ��
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetConnectionName(LONG conn_id, PTCHAR name, UINT32 size)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetConnectionName(conn_id, name, size);
}

/*
 desc : ���� ����� Project Name ���
 parm : name	- [out] Project Name �� ���
		size	- [in]  'name' ���� ũ��
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetCurrentProjectName(PTCHAR name, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->GetCurrentProjectName(name, size);
}

/*
 desc : �⺻ ������ Default Project Name ���
 parm : name	- [out] Project Name �� ���
		size	- [in]  'name' ���� ũ��
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetDefaultProjectName(PTCHAR name, UINT32 size)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetDefaultProjectName(name, size);
}

/*
 desc : ���� ����� ��� ID �� ��ȯ
 parm : equip_id- [out] ����(����)�Ǵ� ��� ID �� ����
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetHostToEquipID(LONG &equip_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->GetHostToEquipID(equip_id);
}

/*
 desc : Event Name�� �ش�Ǵ� Event ID ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		name	- [in]  Event Name
		id		- [out] Event ID�� ��ȯ�Ǿ� �����
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetEventNameToID(LONG conn_id, PTCHAR name, LONG &id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->GetEventNameToID(conn_id, name, id);
}

/*
 desc : Variable Name�� �ش�Ǵ� Variable ID ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		name	- [in]  Variable Name
		var_id	- [out] Variable ID�� ��ȯ�Ǿ� �����
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetVarNameToID(LONG conn_id, PTCHAR name, LONG &var_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->GetVarNameToID(conn_id, name, var_id);
}

/*
 desc : Variable ID�� �ش�Ǵ� Variable Name ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		var_id	- [in]	Variable ID
		name	- [out] Variable Name�� ��ȯ�Ǵ� ����
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetVarIDToName(LONG conn_id, LONG var_id, PTCHAR name, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->GetVarIDToName(conn_id, var_id, name, size);
}

/*
 desc : Variable ID�� �ش�Ǵ� Variable Description ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		var_id	- [in]	Variable ID
		desc	- [out] Variable Description�� ��ȯ�Ǵ� ����
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetVarIDToDesc(LONG conn_id, LONG var_id, PTCHAR desc, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->GetVarIDToDesc(conn_id, var_id, desc, size);
}

/*
 desc : Variable ID�� �ش�Ǵ� Min, Max �� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		var_id	- [in]  Variable ID
		type	- [out] Value Type (E30_GPVT)�� �����
		min		- [out] Min ���� �����
		max		- [out] Max ���� �����
		size	- [in]  'min, max' ���ڿ� ������ ũ��
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
 desc : ���� ����Ǿ� �ִ� Host ID �������� ������ ���� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		type	- [in]  ���� Ÿ�� (E30_GVTC)
		lst_data- [out] �������� ������ ����� ����Ʈ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetHostVariables(LONG conn_id, E30_GVTC type, CAtlList <STG_CEVI> &lst_data)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->GetHostVariables(conn_id, type, lst_data);
}

/*
 desc : �α� ���� (���Ͽ� �α� ������ ���� ������ ����)
 parm : enable	- [out] �α� ���� ���� ���� �� ��ȯ
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsLogging(BOOL &enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->IsLogging(enable);
}

/*
 desc : �α� ���� ���� ����
 parm : enable	- [in]  �α� ���� ���� ���� ���� �÷���
						enable=TRUE -> Start, enable=FALSE -> Stop
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetLogEnable(BOOL enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE30GEM->SetLogEnable(enable);
}

/*
 desc : ���� ��� (�߻�)�� ��� ID ���� ��ȯ
 parm : id		- [out] ����Ʈ�� ����� CAtlList <LONG> : ID
		name	- [out] ����Ʈ�� ����� String Array    : Name
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetCommandList(CAtlList <LONG> &id, CStringArray &name)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetCommandList(id, name);
}

/*
 desc : ���� ��� (�߻�)�� EPJ ���ϵ� ��ȯ
 parm : files	- [out] ����Ʈ�� ����� String Array    : EPJ Files (Included Path)
		sizes	- [out] ����Ʈ�� ����� CAtlList <LONG> : EPJ File Size
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetEPJList(CStringArray &files, CAtlList <LONG> &sizes)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetEPJList(files, sizes);
}

/*
 desc : ���� ��� (�߻�)�� Recipe Names ��ȯ
 parm : names	- [out] ����Ʈ�� ����� String Array    : Recipe Names
		sizes	- [out] ����Ʈ�� ����� CAtlList <LONG> : Recipe Sizes
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetRecipes(CStringArray &names, CAtlList <LONG> &sizes)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetRecipes(names, sizes);
}

/*
 desc : Host �ʿ� Process Program (Recipe)�� �����ϰڴٰ� ��û
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		name	- [in]  Recipe (Process Program) Name (Ȯ���ڴ� �� �������� �ʾƵ� ��)
		size	- [in]  Recipe (Process Program) Size (unit: bytes)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_QueryRecipeUpload(LONG conn_id, PTCHAR name, UINT32 size)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->QueryRecipeUpload(conn_id, name, size);
}

/*
 desc : Host �ʿ� Process Program (Recipe)�� ��û
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		name	- [in]  Recipe (Process Program) Name (Ȯ���ڴ� �� �������� �ʾƵ� ��)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_QueryRecipeRequest(LONG conn_id, PTCHAR name)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->QueryRecipeRequest(conn_id, name);
}

/*
 desc : �ý��ۿ� ���ǵ� Report ��� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		ids		- [out] ����Ʈ�� ����� CAtlList <LONG> : Report Names
		names	- [out] ����Ʈ�� ����� String Array    : Report IDs
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetReports(LONG conn_id, CAtlList <LONG> &ids, CStringArray &names)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetReports(conn_id, ids, names);
}

/*
 desc : Report �� ���õ� Event�� ����Ʈ ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		r_ids	- [out] ����Ʈ�� ����� CAtlList <LONG> : Report IDs
		e_ids	- [out] ����Ʈ�� ����� CAtlList <LONG> : Event IDs
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetReportsEvents(LONG conn_id, CAtlList <LONG> &r_ids, CAtlList <LONG> &e_ids)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetReportsEvents(conn_id, r_ids, e_ids);
}

/*
 desc : �߻��� Traces�� ����Ʈ ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		ids		- [out] ����Ʈ�� ����� CAtlList <LONG> : Report IDs
		names	- [out] ����Ʈ�� ����� String Array    : Trance Names
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetListTraces(LONG conn_id, CAtlList <LONG> &ids, CStringArray &names)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetListTraces(conn_id, ids, names);
}

/*
 desc : EPJ ���Ͽ� ��ϵ� variables ��� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		type	- [in]  Variable Type (E30_GVTC)
		ids		- [out] Variable IDs ��ȯ
		names	- [out] Variable Names ��ȯ
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetListVariables(LONG conn_id, E30_GVTC type, CAtlList <LONG> &ids,
										  CStringArray &names)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetListVariables(conn_id, type, ids, names);
}

/*
 desc : List all messages (SECS-II ��� �޽��� ��ȯ)
		�� �޼���� ��� �������̽����� ���� ó�� ���� �޽����� �޽��� ID ����� ��ȯ�մϴ�.
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		ids		- [out] message IDs ��ȯ
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetListMessageIDs(LONG conn_id, CAtlList <LONG> &ids)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetListMessageIDs(conn_id, ids);
}

/*
 desc : Variable ID�� �ش�Ǵ� (E30_GPVT) �� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		var_id	- [in]  Variable ID�� ���εǴ� Name
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
 desc : Variable ID�� �ش�Ǵ� Variable Type (E30_GVTC) �� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		var_id	- [in]  Variable ID�� ���εǴ� Name
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
 desc : ����� ��� ���� ���� ���
 parm : conn_id		- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		comm_pgid	- [out] The COM ProgID of the communications object
		size_pgid	- [in]  'comm_pgid'�� ����� ���ڿ� ����
		comm_sets	- [out] The communications settings
		size_sets	- [in]  'comm_sets'�� ����� ���ڿ� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetCommSetupInfo(LONG conn_id, PTCHAR comm_pgid, UINT32 size_pgid,
										  PTCHAR comm_sets, UINT32 size_sets)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetCommSetupInfo(conn_id, comm_pgid, size_pgid, comm_sets, size_sets);
}

/*
 desc : ����� ��� ����
 parm : conn_id		- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
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
 desc : �α� ���� ���� ȯ�� ���� ���� ���� ������Ʈ���� �������� ���� ����
 parm : enable	- [in]  TRUE : ����, FALSE : ���� ����
		logs	- [in]  �α� ���� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetLoggingConfigSave(BOOL enable)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SetLoggingConfigSave(enable);
}

/*
 desc : �α� ���� ���� ȯ�� ���� ���� ���� ���� �� ��ȯ
 parm : enable	- [out] TRUE : �����ϰ� ����, FALSE : ���� ���ϰ� ����
		logs	- [in]  �α� ���� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetLoggingConfigSave(BOOL &enable)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetLoggingConfigSave(enable);
}

/*
 desc : SECS-II Message �۽� (����) ��, Primary Message �۽�? ���
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		msg_sid	- [in]  SECS-II Message ID : Stream ID (0xssff -> 0x00xx ~ 0xffxx ����)
		msg_fid	- [in]  SECS-II Message ID : Function ID (0xssff -> 0xxx00 ~ 0xxxFF ����)
		term_id	- [in]  ���� 0 ���̳�, �ڽ��� Terminal ID�� 1 �̻��� ��쵵 �ִ� (?)
		str_msg	- [in]  �޽��� ���� (���ڿ�), ���� NULL�̸� �޽��� ������ ���� �����
		reply	- [in]  ���� ��� ���� (Host�κ��� ���� �ޱ⸦ ���ϴ��� ����)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SendPrimaryMessage(LONG conn_id, UINT8 msg_sid, UINT8 msg_fid,
											PTCHAR str_msg, UINT8 term_id, BOOL reply)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SendPrimaryMessage(conn_id, msg_sid, msg_fid, str_msg, term_id, reply);
}

/*
 desc : Application Name ��ȯ
 parm : name	- [out] Application Name�� ����� ����
		size	- [in]  'name' ���� ũ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetAppName(PTCHAR name, UINT32 size)
{
	if (!g_pE30GEM)	return FALSE;
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetAppName(name, size);
}

/*
 desc : Application Name ����
 parm : name	- [in]  Application Name�� ����� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_SetAppName(PTCHAR name)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->SetAppName(name);
}

/*
 desc : ���� ������Ʈ�� ��Ű���� ����� ������ ��¥ (�ð� ����) ��ȯ
 parm : date_time	- [out] ����� �ð� (��¥����)�� ����� ����
		size		- [in]  'data_time' ���� ũ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetBuildDateTime(PTCHAR date_time, UINT32 size)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetBuildDateTime(date_time, size);
}

/*
 desc : ���� ������Ʈ�� ��Ű���� ����� ������ Version ��ȯ
 parm : version	- [out] ����� �ð� (��¥����)�� ����� ����
		size	- [in]  'version' ���� ũ��
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
 desc : ���� (Ư��) ��ġ�� ����Ǿ� �ִ� ���� ���α׷��� IDs�� �̸����� ��ȯ
 parm : equip_id- [in]  ��� ID (0, 1 or Later)
		ids		- [out] ���� ���α׷� IDs ��ȯ (ID ���� ����Ʈ�� ��ȯ)
		names	- [out] ���� ���α׷� Names ��ȯ (���ڿ� �迭�� ��ȯ)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetListApplications(LONG equip_id, CAtlList <LONG> &ids, CStringArray &names)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetListApplications(equip_id, ids, names);
}

/*
 desc : ���� (Ư��) ��ġ�� ����Ǿ� �ִ� Host�� IDs�� �̸����� ��ȯ
 parm : conn_id	- [in]  ��� ID (0, 1 or Later)
		ids		- [out] Host IDs ��ȯ (ID ���� ����Ʈ�� ��ȯ)
		names	- [out] Host Names ��ȯ (���ڿ� �迭�� ��ȯ)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetListConnections(LONG conn_id, CAtlList <LONG> &ids, CStringArray &names)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetListConnections(conn_id, ids, names);
}

/*
 desc : EM Service�� ����Ǿ� �ִ� Equipment�� IDs�� �̸����� ��ȯ
 parm : equip_id- [in]  ��� ID (0, 1 or Later)
		ids		- [out] Equipment IDs ��ȯ (ID ���� ����Ʈ�� ��ȯ)
		names	- [out] Equipment Names ��ȯ (���ڿ� �迭�� ��ȯ)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_GetListEquipments(CAtlList <LONG> &ids, CStringArray &names)
{
	if (!g_pE30GEM->IsInitConnected())	return FALSE;
	return g_pE30GEM->GetListEquipments(ids, names);
}

/*
 desc : ���� Control�� Remote ������� ����
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsControlRemote(LONG conn_id)
{
	if (!g_pE30GEM->IsInitCompleted())	return FALSE;
	return g_pE30GEM->IsControlRemote(conn_id, FALSE);
}

/*
 desc : ���� Control�� Online ������� ����
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_IsControlOnline(LONG conn_id)
{
	if (!g_pE30GEM->IsInitCompleted())	return FALSE;
	return g_pE30GEM->IsControlOnline(conn_id, FALSE);
}

/*
 desc : ���� Communication��尡 Enable���� ����
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
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
 desc : Process Job�� SETTING UP ���¿��� WAITING FOR START ���� PROCESSING ���·� �̵�
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
 note : Moves Process Job from SETTING UP state to WAITING FOR START or PROCESSING state
		���簡 �غ�ǰ� Process Job�� ������ �غ� �Ǿ� �ִٰ� �˸�
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetPRJobStartProcess(PTCHAR pj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetPRJobStartProcess(pj_id);
}

/*
 desc : Process Job�� ���� Control Job ID ��ȯ
 parm : pj_id	- [in]  Object ID of the Process Job
		cj_id	- [out] Object ID of the Control Job
		size	- [in]  'cj_id' ���� ũ��
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetControlJobID(PTCHAR pj_id, PTCHAR cj_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetControlJobID(pj_id, cj_id, size);
}

/*
 desc : Process Job�� ���� Control Job ID ����
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
 desc : Process Job�� Processing�� ���� �����
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
 desc : Process Job�� ���� ��� (Job Processed ������ ��츸 �ش��)
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
 desc : Process Job�� Stopping ���¿��� Stopped ���·� �̵� (����)
 parm : pj_id	- [in]  Objet ID of the Process Job
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetProcessJobStoppingToStopped(PTCHAR pj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetProcessJobStoppingToStopped(pj_id);
}

/*
 desc : Processing Resource�� ���� Queue���� ������ �� �ִ� ���� �ִ� Jobs ���� ��ȯ
 parm : count	- [out] ������ �� �ִ� ���� ���� ��ȯ ���� ����
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetProcessJobSpaceCount(UINT32 &count)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetProcessJobSpaceCount(count);
}

/*
 desc : Process Job (ID)�� ���� ���� ��ȯ
 parm : pj_id	- [in]  Process Job ID
		state	- [OUT] state
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetProcessJobState(PTCHAR pj_id, E40_PPJS &state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetProcessJobState(pj_id, state);
}

/*
 desc : ��� Process Job Id ��ȯ (�Ϸ���� ���� �۾��� ���¸� ��ȯ)
 parm : list_job- [out] Job ID�� ��ȯ�Ǿ� ����� �迭 ����
		logs	- [in]  �α� ��� ����
 ��ȯ : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetProcessJobQueue(CStringArray &list_job)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetProcessJobQueue(list_job);
}

/*
 desc : ���� Process Job ID �� �� ó�� SELECTED ������ Process Job ID ��ȯ
 parm : filter	- [in]  Process Job ID �˻� ���� (E40_PPJS)
		pj_id	- [out] ��ȯ�Ǿ� ����� Process Job ID (String)
		size	- [in]  'pj_id' ���� ũ��
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
 note : �˻��� ����� ������ FALSE ��ȯ
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetProcessJobFirstID(E40_PPJS filter, PTCHAR pj_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetProcessJobFirstID(filter, pj_id, size);
}

/*
 desc : ��� Process Job Id�� State ��ȯ
 parm : list_job	- [out] Job ID�� ��ȯ�Ǿ� ����� �迭 ����
		list_state	- [out] ���� ���� ������� ����Ʈ ����
		logs	- [in]  �α� ��� ����
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
 parm : pj_id	- [in]  �����ϰ��� �ϴ� Process Job Id
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetProcessJobRemove(PTCHAR pj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetProcessJobRemove(pj_id);
}

/*
 desc : Process Job�� Queuing ���¿��� Setting Up ���·� �̵�
 parm : pj_id	- [in]  Objet ID of the Process Job
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetProcessJobQueueToSetup(PTCHAR pj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetProcessJobQueueToSetup(pj_id);
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
 desc : Remote Control ��� ���� ����
 parm : enable	- [in]  Enable or Disable
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetAllowRemoteControl(BOOL enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetAllowRemoteControl(enable);
}

/*
 desc : Remote Control ��� ���� �� ��ȯ
 parm : enable	- [out] Enable or Disable
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetAllowRemoteControl(BOOL &enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetAllowRemoteControl(enable);
}

/*
 desc : Process Material Name List (For substrates)�� �Ӽ� �� (Carrier ID, Slot ID) ��ȯ
		!!! TEL�� ���, ������ 2��° ��, CarrierID �� Slot No�� �Ǿ� �ִ� ������� �о�� �� !!!
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		carr_id	- [out] Carrier ID�� ��ȯ�Ǿ� ����� ����
		slot_no	- [out] Slot Number�� ��ȯ�Ǿ� ����� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetPRMtlNameList(PTCHAR pj_id,
												 CStringArray &carr_id, CByteArray &slot_no)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetPRMtlNameList(pj_id, carr_id, slot_no);
}

/*
 desc : ���� Process Job ID ���� ��ϵ� Slot Number List �߿��� ã���� �ϴ� Slot Number�� �ִ��� ����
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		slot_no	- [in]  �˻��ϰ��� �ϴ� Slot Number (1-based)
		is_slot	- [out] Slot Number ���� ���� ��ȯ
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_IsFindSlotNoOfPRMtlNameList(PTCHAR pj_id, UINT8 slot_no, BOOL &is_slot)
{
	UINT8 i	= 0x00;
	CStringArray arrCarID;
	 CByteArray arrSlotNo;

	/* �ϴ�, ������ �� ã�Ҵٰ� ����  */
	is_slot	= FALSE;

	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	/* ���� Process Job ID�� ���� Carrier ID�� Slot Number ����Ʈ ��� */
	if (!g_pE40PJM->GetPRMtlNameList(pj_id, arrCarID, arrSlotNo))	return FALSE;

	/* Process Job ID�� ���� Slot Number �� */
	for (; i<arrCarID.GetCount(); i++)
	{
		if (arrSlotNo[i] == slot_no)
		{
			is_slot	= TRUE;
			return TRUE;
		}
	}

	/* �޸� ��ȯ */
	arrCarID.RemoveAll();
	arrSlotNo.RemoveAll();

	return TRUE;	/* �˻� ����� ��� ����, CIMETRIX Function ȣ�� ���� ������ */
}

/*
 desc : Process Job�� ���� ���� ���� ������ ���� ��Ŵ
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
 desc : Process Job State ����
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
 desc : Process Job�� ABORTING ���¿��� ABORTED ���·� �̵�
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
 note : Moves Process Job from ABORTING state to the ABORTED state.
		�۾� ��Ұ� �Ϸ�Ǿ��� ��, �� �Լ��� ȣ���� �ָ� ��
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
		Queue�� ��ϵ� ��, Process Job ID�� ���� ����� ��ȯ �մϴ�.
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
		Queue���� Process Job ID�� ����Ǵ� ���� ��� ����
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
 desc : Process Job���� ����� ���� ��Ŵ (Executes a command on a Process Job)
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		command	- [in]  ��ɾ� (PCIMLib���� ��ϵ� ��ɾ�. START;STOP;PAUSE;RESUME;ABORT;CANCEL)
						Register for Remote Commands (Remote Command name�� �� ���̰� 20 string�� ���� �ʾƾ� ��)
 retn : TRUE or FALSE
 note : This method is called by the host to perform a PRJobCommand on the specified Process Job
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetPRJobCommand(PTCHAR pj_id, PTCHAR command)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetPRJobCommand(pj_id, command);
}

/*
 desc : Pause Events�� ���� Enable or Disable
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
 desc : Pause Events�� ���� Enable or Disable ���� �� ��ȯ
 parm : enable	- [out] TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetEnablePauseEvent(BOOL &enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetEnablePauseEvent(enable);
}

/*
 desc : ��� �ʿ��� ����ϴ� �˸� (Notification)�� ���� (Type)�� �������� �˷���
 parm : type	- [out] Notification Type (E40_PENT)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetNotificationType(E40_PENT &type)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetNotificationType(type);
}

/*
 desc : ��� �ʿ��� ����� �˸� (Notification)�� ���� (Type)�� � ������ ������ ��
 parm : type	- [in]  Notification Type (E40_PENT)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetNotificationType(E40_PENT type)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetNotificationType(type);
}

/*
 desc : Process Job�� ���°� ����Ǿ��ٰ� Host���� �˸�
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetPRJobAlert(PTCHAR pj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetPRJobAlert(pj_id);
}

/*
 desc : Process Job ID�� Event State�� ����Ǿ��ٰ� Host���� �˸�
 parm : pj_id		- [in]  Process Job Name ID (String; Ascii)
		evt_id		- [in]  PREventID of PRJobEvent service (E40_PSEO)
		list_vid	- [in]  Variables ID�� ����� ����Ʈ
		list_type	- [in]  Variables Value Type�� ����� ����Ʈ
		list_value	- [in]  'Variables Type'�� ����, ���� Ȥ�� ���ڿ� ���� ��ϵǾ� ����
							�����, ������ ���, _wtoi / _wtof ��� ��ȯ�� �ʿ� ��
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
 desc : ���� (Ư��) ���μ��� Job������ �Ӽ��� "PRProcessStart"�� ���� ����
 parm : list_jobs	- [in]  ���� ����� Process Job���� ����� ����Ʈ
		start		- [in]  TRUE or FALSE
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetPRJobSetStartMethod(CStringArray *list_jobs, BOOL start)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetPRJobSetStartMethod(list_jobs, start);
}

/*
 desc : ���� ������ Material Type �� ��ȯ
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		type	- [out] ���� ������ Material Type �� ����
 retn : TRUE or FALSE
 note : Gets the PRMtlType attribute value
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetPRMtlType(PTCHAR pj_id, E40_PPMT &type)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetPRMtlType(pj_id, type);
}

/*
 desc : Material Type �� ���� ����
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		type	- [in]  ������ Maerial Type ��
 retn : TRUE or FALSE
 note : Sets the PRMtlType attribute value
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetPRMtlType(PTCHAR pj_id, E40_PPMT type)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetPRMtlType(pj_id, type);
}

/*
 desc : ó���� �����ϱ� ���� ��� ���� (Wafer? Substrate)���� ������ ������ ����ϴ��� ���� �� ��ȯ
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		value	- [out] TRUE : ��� ���� (����)���� �����ϴµ� ��ٸ��� ����
						FALSE: ��� ���� (����)���� �̹� ������ ������
 retn : TRUE or FALSE
 note : Gets the Process Job's PRWaitForAllMaterial switch value.
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetPRWaitForAllMaterial(PTCHAR pj_id, BOOL &value)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetPRWaitForAllMaterial(pj_id, value);
}

/*
 desc : ó���� �����ϱ� ���� ��� ���� (Wafer? Substrate)���� ������ ������ ����ؾ� ���� ���� ����
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		value	- [in]  TRUE : ��� ���� (����)���� �����ϴµ� ��ٸ��� �ִٶ�� ����
						FALSE: ��� ���� (����)���� �����Ͽ��ٶ�� ���� (��ٸ� �ʿ� ����?)
 retn : TRUE or FALSE
 note : Sets the Process Job's PRWaitForAllMaterial switch value.
*/
API_EXPORT BOOL uvCIMLib_E40PJM_SetPRWaitForAllMaterial(PTCHAR pj_id, BOOL value)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->SetPRWaitForAllMaterial(pj_id, value);
}

/*
 desc : Process Job ID�κ��� Recipe ID (=Name; with string) ���
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		rp_id	- [out] Recipe ID�� ����� ����
		size	- [in]  'rp_id' ���� ũ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetRecipeID(PTCHAR pj_id, PTCHAR rp_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetRecipeID(pj_id, rp_id, size);
}

/*
 desc : ���� Process Job ID�� ���� �۾��� �Ϸ� �Ǿ����� ����
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		state	- [out] 0x00: Busy, 0x01: Completed
		logs	- [in]  �α� ��� ����
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
 desc : ��� Process Job Id ��ȯ (�Ϸ���� ���� �۾��� ���¸� ��ȯ)
 parm : list_job- [out] Job ID�� ��ȯ�Ǿ� ����� �迭 ����
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetProcessAllJobId(CStringArray &list_job)
{
	if (!g_pE40PJM || !g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE40PJM->GetProcessAllJobId(list_job);
}

/*
 desc : Process Job ID�� ���Ե� ��� Slot Number ��ȯ
 parm : pj_id	- [in]  �˻� ����� Process Job ID
		arr_slot- [out] Slot Number�� ����� Byte Array
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E40PJM_GetPRJobToAllSlotNo(PTCHAR pj_id, CByteArray &arr_slot)
{
	CStringArray arrCarrID;

	if (!g_pE40PJM)	return FALSE;
	if (!g_pE30GEM->IsInitedAll(TRUE))	return FALSE;
	/* PRJob ID�� �ش�Ǵ� Slot Number�� Carrier ID ����Ʈ ��� */
	if (!g_pE40PJM->GetProcessJobAllSlotNo(pj_id, arr_slot, arrCarrID))	return FALSE;

	return TRUE;
}

/*
 desc : Process Job ID�� ���Ե� ��� Slot ID ��ȯ
 parm : pj_id	- [in]  �˻� ����� Process Job ID
		arr_slot- [out] Slot ID�� ����� String Array
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
	/* PRJob ID�� �ش�Ǵ� Slot Number�� Carrier ID ����Ʈ ��� */
	if (!g_pE40PJM->GetProcessJobAllSlotNo(pj_id, arrSlotNo, arrCarrID))
		return FALSE;
	for (; i<(UINT8)arrSlotNo.GetCount(); i++)
	{
		/* Slot ID ���� */
		strSlotID.Empty();
		strSlotID.Format(L"%s.%02d", arrCarrID[i].GetBuffer(), arrSlotNo[i]);
		/* ��ȯ ���ۿ� ��� */
		arr_slot.Add(strSlotID);
	}

	return TRUE;
}

/*
 desc : Process Job�� Queue ���¿��� Completed ���·� �̵� (����)�ϸ� Project Job Id�� ����
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
 note : SetProcessJobRemove �Լ��� �ٸ��� ? Queue���� �����ϴ� ����, Job List���� �����ϴ� ���� �ƴ� 
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
 desc : ICxSubstrate for E90STS ��ü ������ ���
 parm : carr_id	- [in]  Carrier ID
		slot_no	- [in]  Carrier ���� ��ġ�� Slot Number
		subst_id- [out] Substrate ID�� ��ȯ�� ����
		size	- [in]  'subst_id'�� ���� ũ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetSubstToE87SubstrateID(PTCHAR carr_id, UINT8 slot_no,
														 PTCHAR subst_id, UINT32 size)
{
	return GetSubstToE87SubstrateID(carr_id, slot_no, subst_id, size);
}

/*
 desc : Carrier�� �ε� ��Ʈ ���� ���� (��������;�����ϰ�)�ϰų� Ȥ�� ���ŵǰ� �ִ� ���̴� (?)
 parm : port_id	- [in]  Load Port Number (1 or 2)
 retn : TRUE or FALSE
 note : Load Port ���¸� "Ready To Load" or "Ready To Unload"���� "Transfer Bloacked"���� ���� ��Ŵ
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetTransferStart(UINT16 port_id)
{
	if (!g_pE30GEM->IsInitedAll())		return FALSE;
	if (port_id < 1 || port_id > 2)	return FALSE;
	return g_pE87CMS->SetTransferStart(port_id);
}

/*
 desc : Carrier�� �ε� ��Ʈ ���� ���� �ߴ�.
 parm : port_id	- [in]  Load Port Number (1 or 2)
 retn : TRUE or FALSE
 note : Load Transfer Complete (Sequnce Name)
		������Ʈ ���� (EPJ) ����
			"UseCarrierPlaced" ������ ���� 1�� �����Ǿ� �ִٸ�
				CarrierLocationChanged collection event�� �ڵ����� Ʈ���� ������
			"UseCarrierPlaced" ������ ���� 0�� �����Ǿ� �ִٸ�
				CarrierLocationChanged collection event�� CarrierAtPort�� ȣ��� �� Ʈ���� �˴ϴ�.
		�� �Լ��� ��� Load Port Reservation State�� �������� �ʰ�, CarrierAtPort�� ���� �մϴ�.
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCarrierPlaced(UINT16 port_id)
{
	if (!g_pE30GEM->IsInitedAll())		return FALSE;
	if (port_id < 1 || port_id > 2)	return FALSE;
	return g_pE87CMS->SetCarrierPlaced(port_id);
}

/*
 desc : Carrier�� Load Port���� �����ϰ��� �� ��, �� �Լ� ȣ�� (���� ����)
 parm : port_id	- [in]  Load Port Number (1 or 2)
 retn : TRUE or FALSE
 note : AccessMode�� ���� ����� ���,
			Carrier�� Unloading �߿� undocked �� unclamped�� �� ȣ���ؾ� �ϰ�
		AccessMode�� �ڵ� ����� ���, CarrierUnclamped equipment constant value ���� 1�� ���,
			Carrier�� Unloading �߿� ���� Clampled������ undocked �� �� ȣ���ؾ� �մϴ�.
		AccessMode�� �ڵ� ����� ���, CarrierUnclamped equipment constant value ���� 0�� ���,
			Carrier�� Unloading �߿� undocked �� unclamped�� �� ȣ���ؾ� �մϴ�.
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetReadyToUnload(UINT16 port_id)
{
	if (!g_pE30GEM->IsInitedAll())		return FALSE;
	if (port_id < 1 || port_id > 2)	return FALSE;
	return g_pE87CMS->SetReadyToUnload(port_id);
}

/*
 desc : Carrier�� �ε� ��Ʈ ���� �÷�����, ������ ���� CarrierID�� ���������� �ǵ��ǰ� ���� ȣ�� ��
 parm : carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		port_id	- [in]  Load Port Number (1 or 2)
		loc_id	- [in]  Carrier Location ID ��, ��κ� Carrier Undocked Name (Carrier Location Name)
 retn : TRUE or FALSE
 note : CarrierID �бⰡ �����ϸ�, carr_id ���� empty ���·� �Էµŵ� ��� ����
		�� ��, CIM87 ���������� ���ο� CarrierID�� �����Ѵ� (?)
		���� ������ ���� ���� �ٶ��ϴ�.
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCarrierAtPort(PTCHAR carr_id, UINT16 port_id, PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())		return FALSE;
	if (port_id < 1 || port_id > 2)	return FALSE;
	return g_pE87CMS->SetCarrierAtPort(carr_id, port_id, loc_id);
}

/*
 desc : Carrier�� �ε� ��Ʈ���� �и� �Ǿ��ٰ� CIM87�� �˸�
 parm : carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		port_id	- [in]  Load Port Number (1 or 2)
		loc_id	- [in]  Carrier Location ID ��, ��κ� Carrier Undocked Name (Carrier Location Name)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCarrierDepartedPort(PTCHAR carr_id, UINT16 port_id, PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	if (port_id < 1 || port_id > 2)	return FALSE;
	return g_pE87CMS->SetCarrierDepartedPort(carr_id, port_id, loc_id);
}

/*
 desc : Carrier�� ���ο� ��ġ�� �̵� �ߴٰ� �˸�
 parm : carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
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
 desc : Carrier ���� ���� (��ġ)�� Slot Map�� ���� Verification ó��
 parm : carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		slots	- [in]  Wafer�� Slot Map�� ����� ���� ����
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetVerifySlotMap(PTCHAR carr_id, CAtlList <E87_CSMS> *slots, LONG handle)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetVerifySlotMap(carr_id, slots, handle);
}

/*
 desc : Carrier�� ���� ���� ����
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		port_id	- [in]  Load Port Number (1 or 2)
		status	- [in]  Carrier accessing status (E87_CCAS)
						0x00 : ���� Carrier�� �������� ���� ����
						0x01 : Carrier�� �����ϰ� �ִ� ����
						0x02 : ��� (Equipment)�� ���������� Carrier�� ������ ���� ����
						0x03 : ��� (Equipment)�� ������������ Carrier�� ������ ���� ����
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetAccessCarrier(LONG conn_id, PTCHAR carr_id, UINT16 port_id,
												 E87_CCAS status, LONG handle)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetAccessCarrier(conn_id, carr_id, port_id, status, handle);
}

/*
 desc : Carrier Status Event ����
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
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
 desc : Carrier�κ��� Substrate ���� (��������)
 parm : carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		slot_no	- [in]  Slot Number (Not sequential number)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCarrierPopSubstrate(PTCHAR carr_id, UINT8 slot_no)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetCarrierPopSubstrate(carr_id, slot_no);
}

/*
 desc : ���� Load Port ID�� Reservation State ���� ��ȯ
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [out] Reservation Status ��ȯ
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetReservationState(UINT16 port_id, E87_LPRS &state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->GetReservationState(port_id, state);
}

/*
 desc : ���� Load Port ID�� Reservation State ���� ����
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
 desc : ���� Load Port-Carrier�� Association State ���� ��ȯ
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [out] Load Port-Carrier Association State ��ȯ
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetAssociationState(UINT16 port_id, E87_LPAS &state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->GetAssociationState(port_id, state);
}

/*
 desc : ���� Load Port-Carrier�� Association State ���� ����
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
 desc : ���� Carrier�� Access Mode State ���� ��ȯ
		Load Port�� �������� �����ϴ��� Ȥ�� �ڵ����� �����ϴ��� ���� �� ��ȯ
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [out]  Manual (0) or Auto (1)
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetAccessModeState(UINT16 port_id, E87_LPAM &state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->GetAccessModeState(port_id, state);
}

/*
 desc : ���� Carrier�� Access Mode State �� ����
		Load Port�� �������� �����ϴ��� Ȥ�� �ڵ����� �����ϴ��� ���� �� ����
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
 desc : Carrier ID Verification State �� ��ȯ
 parm : carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		status	- [out] Carrier ID Verfication ���� �� ��ȯ
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetCarrierIDStatus(PTCHAR carr_id, E87_CIVS &status)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->GetCarrierIDStatus(carr_id, status);
}

/*
 desc : Carrier ID Verification State �� ����
 parm : carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		state	- [in]  Carrier ID Verfication ���� ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCarrierIDStatus(PTCHAR carr_id, E87_CIVS status)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetCarrierIDStatus(carr_id, status);
}

/*
 desc : The slot map status of the specified carrier. <��ȯ>
 parm : carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		status	- [out] Slot map status ��ȯ
		logs	- [in]  �α� ��� ����
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
 desc : The slot map status of the specified carrier. <����>
 parm : carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
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
 desc : ���� ���� ��� ���� ����
 parm : enable	- [in]  TRUE or FALSE
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetAllowRemoteControl(BOOL enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetAllowRemoteControl(enable);
}

/*
 desc : ���� ���� ��� ���� ����
 parm : enable	- [in]  TRUE or FALSE
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetAllowRemoteControl(BOOL &enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->GetAllowRemoteControl(enable);
}

/*
 desc : ProceedWithCarrier service �ʱ�ȭ (�Ʒ� 2���� ������ ��쿡 ȣ�� �˴ϴ�)
		1. Carrier ID�� Read�� �Ŀ� ������ ��, ȣ���
		2. SlotMap�� Read�� �Ŀ� ������ ��, ȣ���
		Equipment (��ġ)���� ���������� ȣ���ϴ� ��찡 ���� �� �ְ�,
		Host���� ȣ���ϴ� ��찡 ���� �� �ֽ��ϴ�.
 parm : port_id	- [in]  Load Port ID (1 or 2)
		carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetProceedWithCarrier(UINT16 port_id, PTCHAR carr_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetProceedWithCarrier(port_id, carr_id);
}

/*
 desc : CancelCarrier service ��û (Carrier ���)
 parm : port_id	- [in]  Load Port ID (1 or 2)
		carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCancelCarrier(UINT16 port_id, PTCHAR carr_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetCancelCarrier(port_id, carr_id);
}

/*
 desc : CancelCarrierNotification service request (Carrier ��ҵǾ��ٰ� �˸�)
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
		carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
 retn : TRUE or FALSE
 note : The pParameterList parameter is the same format as the attribute list from the S3F17 message
		Carrier ���°� READY_TO_UNLOAD���� ���θ� ���ο��� Ȯ�� �� ���� ���� ����
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCarrierReCreate(UINT16 port_id, PTCHAR carr_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetCarrierReCreate(port_id, carr_id);
}

/*
 desc : ReleaseCarrier service ��û
 parm : carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetCarrierRelease(PTCHAR carr_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetCarrierRelease(carr_id);
}

/*
 desc : Carrir Location service ��û
 parm : loc_id	- [in]  Load Port Ȥ�� Buffer�� �ƴ� �ٸ� ���� �߰��ϰ��� �ϴ� ID (�̸�. String)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetAddCarrierLocation(PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetAddCarrierLocation(loc_id);
}

/*
 desc : Load Port�� �ο� (����; ����)�� Carrier ID (Name) ��ȯ
 parm : port_id	- [in]  Load Port ID (1 or 2)
		carr_id	- [out] Carrier ID�� ��ȯ�Ǿ� ����� ���� ������
		size	- [in]  'carr_id' ���� ũ��
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetAssociationCarrierID(UINT16 port_id, PTCHAR carr_id, UINT32 size,
														BOOL logs)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->GetAssociationCarrierID(port_id, carr_id, size);
}

/*
 desc : Load Port�� �ο� (����; ����)�� Carrier ID (Name) ��ȯ
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
 desc : Carrier ID�� �ο� (����; ����)�� Load Port Number ��ȯ
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		port_id	- [out] Load Port ID (1 or 2)
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetAssociationPortID(PTCHAR carr_id, UINT16 &port_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->GetAssociationPortID(carr_id, port_id);
}

/*
 desc : Carrier ID�� Load Port Number �ο� (�� �Լ��� Ư���� ��츦 �����ϰ�, ��� �������� ����)
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		port_id	- [in]  Load Port ID (1 or 2)
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetAssociationPortID(PTCHAR carr_id, UINT16 port_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetAssociationPortID(carr_id, port_id);
}

/*
 desc : Host�κ��� ��û���� Transaction ���� �� ��ȯ
 parm : None
 retn : ��û ���� Transaction ������ ����� ���� ����ü ������ ��ȯ
*/
API_EXPORT LPG_CHCT uvCIMLib_E87CMS_GetTransactionInfo()
{
	return g_pSharedData->GetTransactionInfo();
}

/*
 desc : ��ġ(CMPS)�� Carrier Tag �а� �� ��, Host���� S3F30 �޽����� ȣ���ϱ� ���� ����
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		tag_id	- [in]  The requested tag data, can be null if a tag is empty or fail to read
		trans_id- [in]  The transaction ID of S3F29 (Carrier Tag Read Request)
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
 note : uvCIMLib_E87CMS_GetCarrierTagReadData ȣ��ǰ� �� ��, Host���� �޽����� ���� ��, ȣ���ؾ� ��
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetAsyncCarrierTagReadComplete(PTCHAR carr_id, PTCHAR tag_id,
															   LONG trans_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetAsyncCarrierTagReadComplete(carr_id, tag_id, trans_id);
}

/*
 desc : Carrier�κ��� ���� Tag Data ��ȯ
 parm : loc_id	- [in]  Location identifier (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		carr_id	- [in]  Carrier ID (or Name; String)
		data_seg- [in]  Indicates a specific section of data (Tag ID ?)
		data_tag- [out] Carrier�κ��� �о���� Tag Data�� ����� ����
		size	- [in]  'data_tag' ���� ũ��
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
 note : loc_id or carr_id �� 1���� �����Ǿ� �־�� ��. ��, �� �߿� 1���� NULL�̾ ��� ����
		CarrierTagReadData�� carrier ID tag���� ������ ���� ��û�ϴµ� ��� �˴ϴ�.
		Equipment�� LocationID�� CarrierID�� ��ġ���� ������ �ź��� �� �ֽ��ϴ�.
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetCarrierTagReadData(PTCHAR loc_id, PTCHAR carr_id, PTCHAR data_seg,
													  PTCHAR data_tag, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->GetCarrierTagReadData(loc_id, carr_id, data_seg, data_tag, size);
}

/*
 desc : Carrier�� ���� (Ư��) Load Port���� ���Ű� ���۵ǰ� �ִٰ� �˸�
		Load Port�� Carrier�� �־�߸� ����
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		port_id	- [in]  Load Port ID (1 or 2)
		logs	- [in]  �α� ������ ��� ����
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
 desc : Carrier�� ���� (Ư��) Load Port���� ���� �Ǿ��ٰ� �˸�
		Load Port�� Carrier�� �־�߸� ����
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
 desc : Carrier�� Load Port���� ���� ���� ��ġ�� �̵� �Ǿ��ٰ� ����
		��, Load Port�� Undocked���� Docked ��ġ�� �̵� �ߴٰ� ���� ��
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		logs	- [in]  �α� ������ ��� ����
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
 desc : ��� CarrierOut Service ���
		This service shall cause all CarrierOut services to be removed from the queue.
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		logs	- [in]  �α� ������ ��� ����
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
 desc : Load Port�� Transfer Status �� ��ȯ
 parm : port_id	- [in]  Load Port ID (1 or 2)
		status	- [out] Load Port�� Transfer Status �� ��ȯ
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
 note : Get the current load port transfer state
*/
API_EXPORT BOOL uvCIMLib_E87CMS_GetTransferStatus(UINT16 port_id, E87_LPTS &status)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->GetTransferStatus(port_id, status);
}

/*
 desc : Load Port�� Transfer Status �� ����
 parm : port_id	- [in]  Load Port ID (1 or 2)
		status	- [in]  Load Port�� Transfer Status ��
 retn : TRUE or FALSE
 note : Sets the Load Port Transfer State for the specified load port.
		The new value can be any state and does not depend on the current state
		!!! important. �������� ���� ��ȯ�� �������, ���������� �˾Ƽ� ��ȯ�� ���� ����
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetTransferStatus(UINT16 port_id, E87_LPTS status)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetTransferStatus(port_id, status);
}

/*
 desc : CancelBind service Request
		Carrier ID�� Load Port ���� �κ��� ����ϰ� Load Port�� NOT_RESERVED ���·� ��ȯ
 parm : port_id	- [in]  Load Port ID (1 or 2) (-1 or 0 �̻� ��)
		carr_id	- [in]  Carrier ID (or Name; String) (NULL or String)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelBind service
		port_id Ȥ�� carr_id �� ���� �ϳ��� ���� �ݵ�� �ԷµǾ�� ��
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
		���� Carrier�� ���õ� Action�� ����ϰ�,
		��� (Equipment)�� Carrier�� Load Port�� Unload (Ȥ�� ���� ����) ��ġ�� ���� �մϴ�.
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
		���� Carrier�� ���õ� CarrierOut ���񽺸� Queue���� ����
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
		������ Load Port���� Reservation (����?)�� �����ϰ� visible signal�� ��Ȱ��ȭ ��Ŵ
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
 desc : ��� (Equipment)���� ���� Carrier�� ���� ���� �� ����
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
 desc : ��� (Equipment)���� ���� Carrier�� ���� ���� �� ���
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
 desc : ���� Carrier ID ���°� Closed �������� ����
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
 desc : Carrier (FOUP) ���� Ư�� ��ġ�� Substrate (Wafer)�� ����
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		slot_no	- [in]  Slot Number (Not sequential number)
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetSubstratePlaced(PTCHAR carr_id, PTCHAR subst_id, UINT8 slot_no)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetSubstratePlaced(carr_id, subst_id, slot_no);
}

/*
 desc : Recipe (����, ����, ����) �̺�Ʈ ó��
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		recipe	- [in]  Recipe Name
		type	- [in]  0x01 (Created), 0x02 (Edited), 0x03 (Deleted)
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E87CMS_SetRecipeManagement(LONG conn_id, PTCHAR recipe, UINT8 type,
													LONG handle)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE87CMS->SetRecipeManagement(conn_id, recipe,type, handle);
}

/*
 desc : Recipe (Selected) �̺�Ʈ ó��
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		recipe	- [in]  Recipe Name
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID;1 or 2)
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
 desc : Substrate (Transport & Processing) ���� �� ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		loc_id	- [in]  Location ID (Name)�� ����� ���ڿ� ���� or 1-Based Index
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
 desc : Substrate (Transport & Processing) ���� �� ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		loc_id	- [in]  Location ID (Name)�� ����� ���ڿ� ���� or 1-Based Index (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
						Substrate Location ID_xx (���ڿ�. RobotArm / PreAligner / ProcessChamber / Buffer)
		trans	- [in]  Substrate transport state
		proc	- [in]  Substrate processing state
		status	- [in]  Substrate ID state E90_SISM
		time_in	- [in]  Batch�� Location ID�� �����ϴ� �ð� (NULL�̸� ��� ����)
		time_out- [in]  Batch�� Location ID�� ��Ż�ϴ� �ð� (NULL�̸� ��� ����)
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
 desc : Substrate Processing ���� �� ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		proc	- [in]  Substrate processing state
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetChangeSubstrateProcessingState(PTCHAR subst_id, E90_SSPS proc)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetChangeSubstrateProcessingState(subst_id, proc);
}

/*
 desc : Substrate ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		sub_id	- [in]  Location Group ID�� ���� ����ϴ� Subscript Index (location's index)
						EPJ ���Ͽ��� �ش� ��ġ�� ���Ǵ� ÷��?(Subscripted) ������ ��Ʈ (set)
						-> EPJ ���Ͽ� ���ǵ� ����� �������� ������... (������� ���� ���� 0 ��)
		lot_id	- [in]  Lot ID ��, ����ڿ� ���� ���ǿ� �Ҵ�� LOT�� Index �� (NULL�̸� ��� ����)
 retn : TRUE or FALSE
 note : Substrate ������ ���� CIM87�� ���� ���� (Carrier slotmap�� ������ ��)������,
		��� �ٸ� ������� ������ �ʿ䰡 ���� ��, �� �Լ��� ȣ���Ͽ� ����
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
 desc : Substrate ID ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetRemoveSubstrate(PTCHAR subst_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->RemoveSubstrate(subst_id);
}

/*
 desc : Substrate Location (Wafer�� ������ ��ġ �̸�) ���
 parm :	loc_id	- [in]  Location ID (Name)�� ����� ���ڿ� ����
						Substrate Location ID_xx (���ڿ�. RobotArm / PreAligner / ProcessChamber / Buffer)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetAddSubstrateLocation(PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->AddSubstrateLocation(loc_id);
}

/*
 desc : Substrate Location (Wafer�� ������ ��ġ �̸�) ����
 parm :	loc_id	- [in]  Location ID (Name)�� ����� ���ڿ� ����
						Substrate Location ID_xx
						(���ڿ�. RobotArm / PreAligner / ProcessChamber / Buffer)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetRemoveSubstrateLocation(PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->RemoveSubstrateLocation(loc_id);
}

/*
 desc : Substrate (Wafer)�� ���� �ϵ��� (�޵���) ��� ���� ȣ��
		��� �ʿ��� ���������� E90CommandCallback �Լ��� ȣ���
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
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
 desc : Remote Control ��� ���� �� ��ȯ
 parm : enable	- [out] Enable or Disable
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetAllowRemoteControl(BOOL &enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetAllowRemoteControl(enable);
}

/*
 desc : Remote Control ��� ���� ����
 parm : enable	- [in]  Enable or Disable
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetAllowRemoteControl(BOOL enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetAllowRemoteControl(enable);
}

/*
 desc : Substrate Location ID�� ���� �� ��ȯ
 parm : loc_id	- [in]  Location ID (Name)�� ����� ���ڿ� ���� or 1-Based Index (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
						Substrate Location ID_xx (���ڿ�. RobotArm / PreAligner / ProcessChamber / Buffer)
		state	- [out] Substrate Location State �� ��ȯ
		logs	- [in]  �α� ������ ��� ����
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
 desc : Substrate Location ID�� ���� �� ����
 parm : loc_id	- [in]  Location ID (Name)�� ����� ���ڿ� ���� or 1-Based Index
						Substrate Location ID_xx (���ڿ�. RobotArm / PreAligner / ProcessChamber / Buffer)
		state	- [in]  Substrate Location State �� ��ȯ (E90_SSLS)
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
 desc : Substrate Processing�� ���� ���� �� ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		state	- [out] Substrate Location State �� ��ȯ (E90_SSPS)
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstratePrevProcessingState(PTCHAR subst_id, E90_SSPS &state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstratePrevProcessingState(subst_id, state);
}

/*
 desc : Substrate Processing ���� �� ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		state	- [out] Substrate Location State �� ��ȯ (E90_SSPS)
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateProcessingState(PTCHAR subst_id, E90_SSPS &state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateProcessingState(subst_id, state);
}

/*
 desc : Substrate Processing ���� �� ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		state	- [in]  �����ϰ��� �ϴ� ���� �� (E90_SSPS)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateProcessingState(PTCHAR subst_id, E90_SSPS state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSubstrateProcessingState(subst_id, state);
}

/*
 desc : Substrate Transport ���� ���� �� ��ȯ
 parm : subst_id- [in]  Substrate ID (���ڿ�)
		state	- [out] Substrate Transport State �� ��ȯ (E90_SSTS)
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstratePrevTransportState(PTCHAR subst_id, E90_SSTS &state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstratePrevTransportState(subst_id, state);
}

/*
 desc : Substrate Transport ���� �� ��ȯ
 parm : subst_id- [in]  Location ID (Name)�� ����� ���ڿ� ���� or 1-Based Index (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
						Substrate Location ID_xx (���ڿ�. RobotArm / PreAligner / ProcessChamber / Buffer)
		state	- [out] Substrate Location State �� ��ȯ (E90_SSPS)
		logs	- [in]  �α� ������ ��� ����
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
 desc : Substrate Transport ���� �� ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		state	- [in]  �����ϰ��� �ϴ� ���� �� (E90_SSTS)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateTransportState(PTCHAR subst_id, E90_SSTS state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSubstrateTransportState(subst_id, state);
}

/*
 desc : Substrate ID (Tag ? Serial ? Bar Code)�� ���� ���� �� ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		status	- [out] Enumeration values for the ID Reader state machine
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstratePrevIDStatus(PTCHAR subst_id, E90_SISM &state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstratePrevIDStatus(subst_id, state);
}

/*
 desc : Substrate ID (Tag ? Serial ? Bar Code) �о��� �� ���� ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		status	- [out] Enumeration values for the ID Reader state machine
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateIDStatus(PTCHAR subst_id, E90_SISM &status)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateIDStatus(subst_id, status);
}

/*
 desc : Substrate ID (Tag ? Serial ? Bar Code) �о��� �� ���� ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		status	- [in]  Enumeration values for the ID Reader state machine
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateIDStatus(PTCHAR subst_id, E90_SISM status)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSubstrateIDStatus(subst_id, status);
}

/*
 desc : CIM300 (Host)���� Substrate ID (Tag ? Serial ? Bar Code) �о��� �� ���¸� �Ǵ��� �޶�� ��û
 parm : subst_id	- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
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
 desc : Substrate ID Reader�� �ǵ� ���� Ȥ�� �Ұ��� �� ��, �̺�Ʈ �߻� ��Ŵ
 parm : enable	- [in]  Substrate ID Reader�� ��� �������� ���� ����
						TRUE: ��� ������ ��, �̺�Ʈ �˸�
						FALSE:��� �Ұ����� ��, �̺�Ʈ �˸�
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateReaderAvailable(INT32 enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SubstrateReaderAvailable(enable);
}

/*
 desc : ��񿡰� ���� substrate�� �ǳʶ��, �ҽ� Ȥ�� �ٸ� ��ġ�� �̵��϶�� ��û �� (���� ��ġ�� �̵��϶�� ��)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
 retn : TRUE or FALSE
 note : This function executes the E90CommandCallback() to allow the equipment to reject the command
		��, �� �Լ��� ȣ���ϸ�, ��� ������ �źε� ���, E90CommandCallback �Լ��� ȣ�� ��
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetCancelSubstrate(PTCHAR subst_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->CancelSubstrate(subst_id);
}

/*
 desc : ���� Substrate ID���� �о���� Wafer Reader Code (Tag, Serial, Bar Code, etc) ���� ��ȯ
 parm : subst_id	- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		acquired_id	- [out] Wafer Reader�κ��� �о���� ID �� ��ȯ (���ڿ�)
		size		- [in]  'acquired_id' ���� ũ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateIDToAcquiredID(PTCHAR subst_id, PTCHAR acquired_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateIDToAcquiredID(subst_id, acquired_id, size);
}

/*
 desc : ���� Substrate ID���� �о���� Wafer Reader Code (Tag, Serial, Bar Code, etc) �� ����
 parm : subst_id	- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		acquired_id	- [in]  Wafer Reader�κ��� �о���� ID �� (���ڿ�)
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
 desc : Substrate ID �� ��ȯ (Slot Number; Slot No)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		slot_no	- [out] Slot Number ���� ��ȯ�Ǿ� ���� (1-based or Later. Max. 255)	
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateIDToSlotNo(PTCHAR subst_id, UINT8 &slot_no)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateIDToSlotNo(subst_id, slot_no);
}

/*
 desc : Substrate Location ID �� ��ȯ (Slot Number; Slot No)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		loc_id	- [out] Location ID (Buffer, Robot, PreAligner, Chamber, etc)�� ����� ����
		size	- [in]  'loc_id' ���� ũ��
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateLocationID(PTCHAR subst_id, PTCHAR loc_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateLocationID(subst_id, loc_id, size);
}

/*
 desc : �ش� ��ġ�� �����۰� �����ϴ��� ����
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
 desc : Ư�� wafer (substrate ID; st_id)�� ���� location�� ������ ���ÿ� ���� ���� �� ����
 parm : subst_id	- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		loc_id		- [in]  Location ID (���ڿ�)
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
 desc : ���� Wafer�� ��Ʈ (Lot; FOUP)�� ������ ��ü�� ��ġ ������ ��ȯ �Ѵ�
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		index	- [out] Wafer�� ��Ʈ (Lot; FOUP)�� ������ ��ġ (�ε���) ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetBatchLocIdx(PTCHAR loc_id, INT16 &index)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetBatchLocIdx(loc_id, index);
}

/*
 desc : ���� Wafer�� ��Ʈ (Lot; FOUP)�� ������ ��ü�� ��ġ ������ ��ȯ �Ѵ�
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		index	- [out] Wafer�� ��Ʈ (Lot; FOUP)�� ������ ��ġ (�ε���) ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetBatchLocIdx(PTCHAR loc_id, INT16 index)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetBatchLocIdx(loc_id, index);
}

/*
 desc : ���� Wafer�� ��Ʈ (Lot; FOUP)�� ������ ��ü�� ���� ���� ����
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_IsBatchLocOccupied(PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->IsBatchLocOccupied(loc_id);
}

/*
 desc : ���� Wafer�� ��Ʈ (Lot; FOUP)�� ������ ��ü�� ���� ���� ����
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		state	- [in]  Batch State �� ��, ���� (1), ���� ���� (0)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetBatchLocState(PTCHAR loc_id, E90_SSLS state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetBatchLocState(loc_id, state);
}

/*
 desc : ���� Wafer�� ��Ʈ (Lot; FOUP)�� ����(���)�� Substrate ID ��� ��ȯ
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [out] loc_id�� ��ġ�� Wafer�� ��Ʈ (Lot; FOUP)�� Substrate ID ����� ����� ����Ʈ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetBatchSubstIDMap(PTCHAR loc_id, CStringArray &subst_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetBatchSubstIDMap(loc_id, subst_id);
}

/*
 desc : ���� Wafer�� ��Ʈ (Lot; FOUP)�� ����(���)�� Substrate ID ��� ���� (���)
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [in]  loc_id�� ��ġ�� Wafer�� ��Ʈ (Lot; FOUP)�� Substrate ID ����� ����� ����Ʈ ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetBatchSubstIDMap(PTCHAR loc_id, CStringArray *subst_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetBatchSubstIDMap(loc_id, subst_id);
}

/*
 desc : Location ID�� ���� �̺�Ʈ�� ������ ���ο� ���� ���� ���
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		flag	- [out] FALSE : �̺�Ʈ ���� (�˸�), TRUE : �̺�Ʈ ������ ���� (�˸��� ����)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetDisableEventsLoc(PTCHAR loc_id, BOOL &flag)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetDisableEventsLoc(loc_id, flag);
}

/*
 desc : Location ID�� ���� �̺�Ʈ�� ������ ���ο� ���� ���� ����
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		flag	- [out] FALSE : �̺�Ʈ ���� (�˸�), TRUE : �̺�Ʈ ������ ���� (�˸��� ����)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetDisableEventsLoc(PTCHAR loc_id, BOOL flag)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetDisableEventsLoc(loc_id, flag);
}

/*
 desc : Batch ID�� ���� �̺�Ʈ�� ������ ���ο� ���� ���� ���
 parm : batch_id- [in]  ID of substrate batch
		flag	- [out] FALSE : �̺�Ʈ ���� (�˸�), TRUE : �̺�Ʈ ������ ���� (�˸��� ����)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetDisableEventsBatch(PTCHAR batch_id, BOOL &flag)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetDisableEventsBatch(batch_id, flag);
}

/*
 desc : Batch ID�� ���� �̺�Ʈ�� ������ ���ο� ���� ���� ����
 parm : batch_id- [in]  ID of substrate batch
		flag	- [out] FALSE : �̺�Ʈ ���� (�˸�), TRUE : �̺�Ʈ ������ ���� (�˸��� ����)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetDisableEventsBatch(PTCHAR batch_id, BOOL flag)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetDisableEventsBatch(batch_id, flag);
}

/*
 desc : Batch (Wafer Collections)�� ����� �� �ִ� �ִ� ���� ��ȯ
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		max_pos	- [in]  �ִ� ��� ������ Substrate position ���� ��ȯ�� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetMaxSubstratePosition(PTCHAR loc_id, LONG &max_pos)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetMaxSubstratePosition(loc_id, max_pos);
}

/*
 desc : Batch (Wafer Collections)�� ����� �� �ִ� �ִ� ���� ��ȯ
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		name	- [out] Object Name�� ����� ����
		size	- [in]  'name' ������ ũ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetObjectName(PTCHAR loc_id, PTCHAR name, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetObjectName(loc_id, name, size);
}

/*
 desc : Update <batch location> GEM data (Batch Location (��ġ) GEM Data ������ �˸�?)
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
 note : GEM ���� - SubstID, SubstLocID, SubstLocState, SubstLocState_i, and SubstLocSubstID_i
*/
API_EXPORT BOOL uvCIMLib_E90STS_SendDataNotificationSubst(PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SendDataNotificationSubst(loc_id);
}

/*
 desc : Substrate Batch Location ID ����
 parm : loc_id	- [in]  �����ϰ��� �ϴ� Batch Location ID (���ڿ�)
		loc_num	- [in]  Batch Location���� substrate locations�� ���� (Carrier ���� ���? ������ substrate location ����)
		sub_id	- [in]  Location Group ID�� ���� ����ϴ� Subscript Index (location's index)
						EPJ ���Ͽ��� �ش� ��ġ�� ���Ǵ� ÷��?(Subscripted) ������ ��Ʈ (set) -> EPJ ���Ͽ� ���ǵ� ����� �������� ������...
						������� ���� ���� 0 ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_AddSubstrateBatchLocation(PTCHAR loc_id, INT32 loc_num, INT16 sub_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->AddSubstrateBatchLocation(loc_id, loc_num, sub_id);
}

/*
 desc : Substrate Batch Location ID ����
 parm : loc_id	- [in]  �����ϰ��� �ϴ� Batch Location ID (���ڿ�)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_DelSubstrateBatchLocation(PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->DelSubstrateBatchLocation(loc_id);
}

/*
 desc : Batch�� ĳ������ ������ �߰� ��
 parm : batch_id- [in]  ID of substrate batch
		carr_id	- [in]  ID of carrier
		slot_map- [in]  1 or 0 or -1
						TRUE : �� Slot map item = 0�� ���� ��ġ ���� ����� �߰� ��
							   ��, �� Slot�� ��ġ ���� �ʱ�ȭ �� �߰��ϵ��� ó��
							   (������� ���� ���� -1)
 retn : TRUE or FALSE
 note : Batch	- ���μ��� �ڿ����� ���ÿ� ó���� ������ �׷� (��ġ?)
*/
API_EXPORT BOOL uvCIMLib_E90STS_AddCarrierToBatch(LONG batch_id, PTCHAR carr_id, LONG slot_map)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->AddCarrierToBatch(batch_id, carr_id, slot_map);
}

/*
 desc : Batch�� ĳ������ ������ ���� ��
 parm : batch_id- [in]  ID of substrate batch
		carr_id	- [in]  ID of carrier
 retn : TRUE or FALSE
 note : Batch	- ���μ��� �ڿ����� ���ÿ� ó���� ������ �׷� (��ġ?)
*/
API_EXPORT BOOL uvCIMLib_E90STS_DelCarrierFromBatch(LONG batch_id, PTCHAR carr_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->DelCarrierFromBatch(batch_id, carr_id);
}

/*
 desc : ���ο� Substrate Batch ����
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
 desc : ���� Substrate Batch ����
 parm : batch_id- [in]  ID of substrate batch
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_DestroyBatch(LONG batch_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->DestroyBatch(batch_id);
}

/*
 desc : Batch�� �ִ� ��� ���ǵ��� �̼� �� ó�� ���� �Ӹ� �ƴ϶�, ��ġ�� ���� ��
 parm : batch_id	- [in]  ID of substrate batch
		loc_id		- [in]  ��ġ �����ϰ��� �ϴ� ID (Location ID: Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		trans_state	- [in]  �̼� ���� �� ��, E90_SSTS
		proc_state	- [in]  ó�� ���� �� ��, E90_SSPS
		time_in		- [in]  Batch�� Location ID�� �����ϴ� �ð� (NULL�̸� ��� ����)
		time_out	- [in]  Batch�� Location ID�� ��Ż�ϴ� �ð� (NULL�̸� ��� ����)
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
 desc : Substrate Location ���
 parm : loc_id	- [in]  ��ġ �����ϰ��� �ϴ� ID (Location ID: Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		sub_id	- [in]  Location Group ID�� ���� ����ϴ� Subscript Index (location's index)
						EPJ ���Ͽ��� �ش� ��ġ�� ���Ǵ� ÷��?(Subscripted) ������ ��Ʈ (set)
						-> EPJ ���Ͽ� ���ǵ� ����� �������� ������...
						������� ���� ���� 0 ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_AddSubstrateLocation(PTCHAR loc_id, PTCHAR subst_id, INT16 sub_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->AddSubstrateLocation(loc_id, subst_id, sub_id);
}

/*
 desc : Substrate Location ����
 parm : loc_id	- [in]  ��ġ �����ϰ��� �ϴ� ID (Location ID: Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_DelSubstrateLocation(PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->DelSubstrateLocation(loc_id);
}

/*
 desc : Batch�� Substrate �߰�
 parm : batch_id- [in]  ID of substrate batch
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
 retn : TRUE or FALSE
 note : Batch	- ���μ��� �ڿ����� ���ÿ� ó���� ������ �׷� (��ġ?)
*/
API_EXPORT BOOL uvCIMLib_E90STS_AddSubstrateToBatch(LONG batch_id, PTCHAR subst_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->AddSubstrateToBatch(batch_id, subst_id);
}

/*
 desc : Batch���� Substrate ����
 parm : batch_id- [in]  ID of substrate batch
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
 retn : TRUE or FALSE
 note : Batch	- ���μ��� �ڿ����� ���ÿ� ó���� ������ �׷� (��ġ?)
*/
API_EXPORT BOOL uvCIMLib_E90STS_DelSubstrateFromBatch(LONG batch_id, PTCHAR subst_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->DelSubstrateFromBatch(batch_id, subst_id);
}

/*
 desc : Batch�� ����ִ� substrate ������ �߰� ��, Substrate�� ���� Batch�� �߰�
 parm : batch_id- [in]  ID of substrate batch
 retn : TRUE or FALSE
 note : Batch	- ���μ��� �ڿ����� ���ÿ� ó���� ������ �׷� (��ġ?)
*/
API_EXPORT BOOL uvCIMLib_E90STS_AddEmptySpaceToBatch(LONG batch_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->AddEmptySpaceToBatch(batch_id);
}

/*
 desc : subscript indices�� ������ ���� (?)
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
 desc : Substrate Transport State ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		state	- [in]  E90_SSTS (Substrate Transport State)
		time_in	- [in]  Batch�� Location ID�� �����ϴ� �ð� (NULL�̸� ��� ����)
		time_out- [in]  Batch�� Location ID�� ��Ż�ϴ� �ð� (NULL�̸� ��� ����)
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
 desc : Substrate Material Status �� ��ȯ (������ ó�� ���� �� ��ȯ. ó�� ���� ?)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		status	- [out] Material Status (Material ó�� ����) ���� ��ȯ�� ���� ����
						��ġ���� ��ȯ�Ǵ� ���� �ٸ���? (EPJ ���� ����?)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateMaterialStatus(PTCHAR subst_id, LONG &status)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateMaterialStatus(subst_id, status);
}

/*
 desc : Substrate Material Status �� ���� (������ ó�� ���� �� ����. ó�� ���� ?)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		status	- [in]  Material Status (Material ó�� ����) ��
						��ġ���� �����ϴ´� ���� �ٸ���? (EPJ ���� ����?)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateMaterialStatus(PTCHAR subst_id, LONG status)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSubstrateMaterialStatus(subst_id, status);
}

/*
 desc : ��� �ٷ�� Substrate�� Type ���� (Wafer, Flat Panel, CD, Mask, etc...)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		type	- [in]  E90_EMST ��, wafer, flat_panel, cd, mask �� �ϳ�
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetCurrentSubstrateType(PTCHAR subst_id, E90_EMST type)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetCurrentSubstrateType(subst_id, type);
}

/*
 desc : ��� �ٷ�� Substrate�� Type ���� (Wafer, Flat Panel, CD, Mask, etc...)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		type	- [out] E90_EMST ��, wafer, flat_panel, cd, mask �� �ϳ�
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetCurrentSubstrateType(PTCHAR subst_id, E90_EMST &type)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetCurrentSubstrateType(subst_id, type);
}

/*
 desc : Substrate ID�� Reader��κ��� ���������� �о������� ��, Host�κ��� Confirm�� �޾Ҵ��� ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		read	- [out] ���������� ID�� �о����� ���� �� (TRUE or FALSE)�� ��ȯ�� ���� ����
 note : Equipment������ NotifySubstrateRead �Լ� ȣ���ؼ� Substrate ID�� ���������� Host���� ��û
		�� NotifySubstrateRead �Լ� ȣ�� �Ŀ�, ���� �Լ� ȣ���ϸ� goodRead���� badRead���� �� �� ����
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateGoodRead(PTCHAR subst_id, BOOL &read)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateGoodRead(subst_id, read);
}

/*
 desc : Substrate ID�� Reader��κ��� �о���� ���� �� ��, TRUE (good) or FALSE (bad) �� ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		read	- [in]  ���������� ID�� �о��ٸ� TRUE, �ƴ϶�� FALSE ����
 note : Equipment������ NotifySubstrateRead �Լ� ȣ���ؼ� Substrate ID�� ���������� Host���� ��û
		�� NotifySubstrateRead �Լ� ȣ�� �Ŀ�, ���� �Լ� ȣ���ϸ� goodRead���� badRead���� �� �� ����
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateGoodRead(PTCHAR subst_id, BOOL read)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSubstrateGoodRead(subst_id, read);
}

/*
 desc : Substrate ID�� ���õ� Lot ID ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		lot_id	- [out] Lot ID�� ����� ����
		size	- [in]  'lot_id' ���� ũ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateLotID(PTCHAR subst_id, PTCHAR lot_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateLotID(subst_id, lot_id, size);
}

/*
 desc : Substrate ID�� ���õ� Lot ID ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		lot_id	- [in]  Lot ID�� ����� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateLotID(PTCHAR subst_id, PTCHAR lot_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSubstrateLotID(subst_id, lot_id);
}

/*
 desc : Substrate ID�� ���õ� Object ID ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		obj_id	- [out] Object ID�� ����� ���� (���⼭�� Substrate ID�� ������ �ؽ�Ʈ?)
		size	- [in]  'lot_id' ���� ũ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateObjID(PTCHAR subst_id, PTCHAR obj_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateObjID(subst_id, obj_id, size);
}

/*
 desc : Substrate ID�� ���õ� Object Type ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		obj_type- [out] Object Type�� ����� ���� ("BatchLoc" or "Substrate" or "SubstLoc")
						���⼭�� ���� "Substrate"
		size	- [in]  'obj_type' ���� ũ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateObjType(PTCHAR subst_id, PTCHAR obj_type, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateObjType(subst_id, obj_type, size);
}

/*
 desc : Location ID�� ���õ� Object ID ��ȯ
 parm : loc_id	- [out] Location ID�� ����� ����
		obj_id	- [out] Object ID�� ����� ���� (���⼭�� Location ID�� ������ �ؽ�Ʈ?)
		size	- [in]  'lot_id' ���� ũ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetLocationObjID(PTCHAR loc_id, PTCHAR obj_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetLocationObjID(loc_id, obj_id, size);
}

/*
 desc : Location ID�� ���õ� Object Type ��ȯ
 parm : loc_id	- [out] Location ID�� ����� ����
		obj_type- [out] Object Type�� ����� ���� ("BatchLoc" or "Substrate" or "SubstLoc")
						���⼭�� ���� "Substrate"
		size	- [in]  'obj_type' ���� ũ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetLocationObjType(PTCHAR loc_id, PTCHAR obj_type, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetLocationObjType(loc_id, obj_type, size);
}

/*
 desc : Substrate�� ���� Source CarrierID ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		carr_id	- [out] sourceCarrierID�� ����� ����
		size	- [in]  'carr_id' ���� ũ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSourceCarrierID(PTCHAR subst_id, PTCHAR carr_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSourceCarrierID(subst_id, carr_id, size);
}

/*
 desc : Substrate�� ���� Source CarrierID ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		carr_id	- [in]  sourceCarrierID�� ����� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSourceCarrierID(PTCHAR subst_id, PTCHAR carr_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSourceCarrierID(subst_id, carr_id);
}

/*
 desc : Substrate�� ���� Source LoadPort ID ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		port_id	- [out] Load Port ID�� ����� ���� ���� (1 or Later)
 retn : TRUE or FALSE
 note : Load Port - the interface location on the equipment where carriers are loaded and unloaded
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSourceLoadPortID(PTCHAR subst_id, LONG &port_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSourceLoadPortID(subst_id, port_id);
}

/*
 desc : Substrate�� ���� Source LoadPort ID ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		port_id	- [in]  sourcLoadPortID�� ����� ���� (1 or Later)
 retn : TRUE or FALSE
 note : Load Port - the interface location on the equipment where carriers are loaded and unloaded
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSourceLoadPortID(PTCHAR subst_id, LONG port_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSourceLoadPortID(subst_id, port_id);
}

/*
 desc : Substrate�� ���� Source Slot ID ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		port_id	- [out] Slot ID�� ����� ���� ���� (1 or Later)
 retn : TRUE or FALSE
 note : slot - physical location capable of holding a substrate within cassette type carrier
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSourceSlotID(PTCHAR subst_id, LONG &slot_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSourceSlotID(subst_id, slot_id);
}

/*
 desc : Substrate�� ���� Source Slot ID ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		port_id	- [in]  sourceSlotID�� ����� ���� (1 or Later)
 retn : TRUE or FALSE
 note : slot - physical location capable of holding a substrate within cassette type carrier
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSourceSlotID(PTCHAR subst_id, LONG slot_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSourceSlotID(subst_id, slot_id);
}

/*
 desc : Substrate�� ������ �̺�Ʈ (�۾�)�� ���õ� Substrate�� ���� ��ġ ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		loc_id	- [out] Location ID�� ����� ����
		size	- [in]  'loc_id' ���� ũ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetCurrentSubstrateLocID(PTCHAR subst_id, PTCHAR loc_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetCurrentSubstrateLocID(subst_id, loc_id, size);
}

/*
 desc : Substrate�� ������ �̺�Ʈ (�۾�)�� ���õ� Substrate�� ���� ��ġ ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		loc_id	- [in]  Location ID�� ����� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetCurrentSubstrateLocID(PTCHAR subst_id, PTCHAR loc_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetCurrentSubstrateLocID(subst_id, loc_id);
}

/*
 desc : Substrate�� Source ID ���
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		src_id	- [out] Source ID�� ����� ����
		size	- [in]  'src_id' ���� ũ��
 retn : TRUE or FALSE
 note : Substrate ID ��� ��� - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateSourceID(PTCHAR subst_id, PTCHAR src_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateSourceID(subst_id, src_id, size);
}

/*
 desc : Substrate�� Source ID ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		src_id	- [in]  Source ID�� ����� ����
 retn : TRUE or FALSE
 note : Substrate ID ��� ��� - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateSourceID(PTCHAR subst_id, PTCHAR src_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSubstrateSourceID(subst_id, src_id);
}

/*
 desc : Substrate�� Destination ID ���
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		tgt_id	- [out] Source ID�� ����� ����
		size	- [in]  'src_id' ���� ũ��
 retn : TRUE or FALSE
 note : Substrate ID ��� ��� - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateDestinationID(PTCHAR subst_id, PTCHAR tgt_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateDestinationID(subst_id, tgt_id, size);
}

/*
 desc : Substrate�� Destination ID ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		tgt_id	- [in]  Source ID�� ����� ����
 retn : TRUE or FALSE
 note : Substrate ID ��� ��� - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetSubstrateDestinationID(PTCHAR subst_id, PTCHAR tgt_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetSubstrateDestinationID(subst_id, tgt_id);
}

/*
 desc : ������ ���� (Transition)�� ����Ǵ� ���� ��ü �׷쿡 ���� ���۵Ǵ� ���� �̺�Ʈ�� �����ϴµ� ���
		��ü (Object)�� ���� �̺�Ʈ�� �����ϴ��� ���� Ȯ��
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		flag	- [out] TRUE : �̺�Ʈ ���� (�˸�), FALSE : �̺�Ʈ ������ ���� (�˸��� ����)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SGetUsingGroupEventsSubst(PTCHAR subst_id, BOOL &flag)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetUsingGroupEventsSubst(subst_id, flag);
}

/*
 desc : ������ ���� (Transition)�� ����Ǵ� ���� ��ü �׷쿡 ���� ���۵Ǵ� ���� �̺�Ʈ�� �����ϴµ� ���
		��ü (Object)�� ���� �̺�Ʈ�� ���� Ȥ�� �����ϴ��� ���� Ȯ��
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		flag	- [in]  TRUE : �̺�Ʈ ���� (�˸�), FALSE : �̺�Ʈ ������ ���� (�˸��� ����)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SSetUsingGroupEventsSubst(PTCHAR subst_id, BOOL flag)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetUsingGroupEventsSubst(subst_id, flag);
}

/*
 desc : ������ ���� (Transition)�� ����Ǵ� ���� ��ü �׷쿡 ���� ���۵Ǵ� ���� �̺�Ʈ�� �����ϴµ� ���
		��ü (Object)�� ���� �̺�Ʈ�� �����ϴ��� ���� Ȯ��
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		flag	- [out] TRUE : �̺�Ʈ ���� (�˸�), FALSE : �̺�Ʈ ������ ���� (�˸��� ����)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SGetUsingGroupEventsLoc(PTCHAR loc_id, BOOL &flag)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetUsingGroupEventsLoc(loc_id, flag);
}

/*
 desc : ������ ���� (Transition)�� ����Ǵ� ���� ��ü �׷쿡 ���� ���۵Ǵ� ���� �̺�Ʈ�� �����ϴµ� ���
		��ü (Object)�� ���� �̺�Ʈ�� ���� Ȥ�� �����ϴ��� ���� Ȯ��
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		flag	- [in]  TRUE : �̺�Ʈ ���� (�˸�), FALSE : �̺�Ʈ ������ ���� (�˸��� ����)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E90STS_SetUsingGroupEventsLoc(PTCHAR loc_id, BOOL flag)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetUsingGroupEventsLoc(loc_id, flag);
}

/*
 desc : Location�� ��ġ�� Substrate ID ���
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [out] Substrate ID�� ����� ����
		size	- [in]  'subst_id' ���� ũ��
 retn : TRUE or FALSE
*/															   
API_EXPORT BOOL uvCIMLib_E90STS_GetLocationSubstID(PTCHAR loc_id, PTCHAR subst_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetLocationSubstID(loc_id, subst_id, size);
}

/*
 desc : Location�� ��ġ�� Substrate ID ����
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [in]  Substrate ID�� ����� ����
 retn : TRUE or FALSE
*/															   
API_EXPORT BOOL uvCIMLib_E90STS_SetLocationSubstID(PTCHAR loc_id, PTCHAR subst_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetLocationSubstID(loc_id, subst_id);
}

/*
 desc : Location�� ��ġ�� Location Index ���
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		index	- [out] Location Index�� ��ȯ�� ���� ���� (1 or Later)
 retn : TRUE or FALSE
*/															   
API_EXPORT BOOL uvCIMLib_E90STS_GetLocationIndex(PTCHAR loc_id, INT16 &index)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetLocationIndex(loc_id, index);
}

/*
 desc : Location�� ��ġ�� Substrate ID ����
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		index	- [in]  Location Index �� (1 or Later)
 retn : TRUE or FALSE
*/															   
API_EXPORT BOOL uvCIMLib_E90STS_SetLocationIndex(PTCHAR loc_id, INT16 index)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->SetLocationIndex(loc_id, index);
}

/*
 desc : Substrate History ��ȯ
 parm : subst_id- [in]  Substrate ID�� ����� ����
		lst_hist- [out] { {loc_id, time_in, time_out}, {loc_id, time_in, time_out}, ..., } ���� ��ȯ�� ����Ʈ
						time_in/out = YearMonthDayHourMinuteSecondCentisecond
 retn : TRUE or FALSE
 ��¥ : 2021.03.08 (Modified by Jinsoo.Kang at philoptics)
 note : N/A
*/
API_EXPORT BOOL uvCIMLib_E90STS_GetSubstrateHistory(PTCHAR subst_id, CAtlList <STG_CSHI> &lst_hist)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE90STS->GetSubstrateHistory(subst_id, lst_hist);
}

/*
 desc : Substrate History ��ȯ
 parm : subst_id- [in]  Substrate ID�� ����� ����
		lst_hist- [in]  { {loc_id, time_in, time_out}, {loc_id, time_in, time_out}, ..., } ���� ����� ����Ʈ
						time_in/out = YearMonthDayHourMinuteSecondCentisecond
 retn : TRUE or FALSE
 ��¥ : 2021.03.08 (Modified by Jinsoo.Kang at philoptics)
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
 desc : Process Job�� �߿��� �� ���� �Ϸ�Ǹ� Control Job���� �˸�
 parm : pj_id	- [in]  Object ID of the Process Job
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_SetPRJobProcessComplete(PTCHAR pj_id)
{
	TCHAR tzCtrlID[CIM_CONTROL_JOB_ID_SIZE]	= {NULL};

	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	/* ���� Process Job ID�� ���� Control Job ID ��� */
	if (!g_pE40PJM->GetControlJobID(pj_id, tzCtrlID, CIM_CONTROL_JOB_ID_SIZE))	return FALSE;
	if (wcslen(tzCtrlID) < 1)	return FALSE;
	/* Completed : Control Job */
	return g_pE94CJM->SetProcessJobComplete(tzCtrlID, pj_id);
}

/*
 desc : ���� Queue�� ��ϵ� Control Job ID ��� �� �� ���� ���� ��ȯ
 parm : list_job	- [out] ��ȯ�Ǿ� ����� Control Job ID (String)
		list_state	- [out] ������ Control Job IDs�� ���� ���� ���� ����� ����Ʈ
		logs		- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetControlJobList(CStringArray &list_job,
												  CAtlList<E94_CCJS> &list_state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetControlJobList(list_job, list_state);
}

/*
 desc : Process Jobs List ��� (Control Job ���ο� ����� Project Job List)
 parm : list_job- [out] ������ Control Job IDs (Names) ����� �迭 ����
		filter	- [in]  Control Job ���� ���� (����)
		logs	- [in]  �α� ��� ����
 retn : TRUE ro FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetControlJobListId(CStringArray &list_job, E94_CLCJ filter)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetControlJobList(list_job, filter);
}

/*
 desc : Process Jobs List �� ���� �� ��� (���͸� ����)
 parm : list_job	- [out] ������ Control Job IDs (Names) ����� �迭 ����
		list_state	- [out] ������ Control Job IDs�� ���� ���� ���� ����� ����Ʈ
		filter		- [in]  Control Job ���� ���� (����)
		logs		- [in]  �α� ��� ����
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
 desc : Control Job ID �� ���Ϳ� ���� �˻��� �� �� ù ��° �˻��� Job ID ��ȯ
 parm : filter	- [in]  Process Job ID �˻� ���� (E40_PPJS)
		cj_id	- [out] ��ȯ�Ǿ� ����� Control Job ID (String)
		size	- [in]  'cj_id' ���� ũ��
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetControlJobFirstID(E94_CCJS filter, PTCHAR cj_id, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetControlJobFirstID(filter, cj_id, size);
}

/*
 desc : Control Job�� ���� ���� ��ȯ
 parm : cj_id	- [in]  Control Job ID
		state	- [out] Job ID�� ���� �� ��ȯ
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetControlJobState(PTCHAR cj_id, E94_CCJS &state)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetControlJobState(cj_id, state);
}

/*
 desc : Control Job ����
 parm : cj_id		- [in]  Control Job ID
		carr_id		- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		pj_id		- [in]  Process Job List �� ����� Array �迭
		job_order	- [in]  Job Order (3: List, 1: Arrival)
		start		- [in]  Auto Start ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_CreateJob(PTCHAR cj_id, PTCHAR carr_id, CStringArray *pj_id,
										  E94_CPOM job_order, BOOL start)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->CreateJob(cj_id, carr_id, pj_id, job_order, start);
}

/*
 desc : Process Jobs List ��� (Control Job ���ο� ����� Project Job List)
 parm : cj_id		- [in]  Object ID of the Control Job
		list_job	- [out] ������ Process Job Names ����� �迭 ����
		list_state	- [out] ������ Control Job IDs (Names)�� State�� ����� ���� ����
		filter		- [in]  Control Job ���ο� ����� Process Job ���� ���� (����)
		logs		- [in]  �α� ��� ����
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
 desc : Process Jobs List ��� (Control Job ���ο� ����� Project Job List)
 parm : cj_id	- [in]  Object ID of the Control Job
		list_job- [out] ������ Process Job Names ����� �迭 ����
		filter	- [in]  Control Job ���ο� ����� Process Job ���� ���� (����)
		logs	- [in]  �α� ��� ����
 retn : TRUE ro FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetProcessJobList(PTCHAR cj_id, CStringArray &list_job,
												  E94_CLPJ filter)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetProcessJobList(cj_id, list_job, filter);
}

/*
 desc : Control Job�� �����ϱ�
 parm : cj_id		- [in]  Control Job ID
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_SetSelectedControlJob(PTCHAR cj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->SetSelectedControlJob(cj_id);
}

/*
 desc : Control Job�� ���� �����ϱ�
 parm : cj_id		- [in]  Control Job ID
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_SetDeselectedControlJob(PTCHAR cj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->SetDeselectedControlJob(cj_id);
}

/*
 desc : Remote Control ��� ���� �� ��ȯ
 parm : enable	- [out] Enable or Disable
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetAllowRemoteControl(BOOL &enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetAllowRemoteControl(enable);
}

/*
 desc : Remote Control ��� ���� ����
 parm : enable	- [in]  Enable or Disable
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_SetAllowRemoteControl(BOOL enable)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->SetAllowRemoteControl(enable);
}

/*
desc : Control Job ���ο��� ���� ProcessJob�� ���۽�Ų��.
parm : cj_id	- [in]  Control Job ID
retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_StartNextProcessJob(PTCHAR cj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->StartNextProcessJob(cj_id);
}

/*
desc : ���� Control Job�� ���۽�Ų��.
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
 desc : EPT (Equiptment Performance Tracking) - Busy ����
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
 desc : EPT (Equiptment Performance Tracking) - Idle ����
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
 desc : Module Process Tracking ���� ����
 parm : mod_name- [in]  Name of the EPT module, use an epty string for the Equipemnt ETP state
						ex> ProcessChamber, RobotArm, PreAligner, Buffer, etc
		pj_id	- [in]  Process Job Name ID (String; Ascii)
		rp_id	- [in]  Recipe ID
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E157MPT_MPTExecutionStarted(PTCHAR mod_name, PTCHAR pj_id, PTCHAR rp_id, PTCHAR subst_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE157MPT->MPTExecutionStarted(mod_name, pj_id, rp_id, subst_id);
}

/*
 desc : Module Process Tracking ���� �Ϸ� (������ ���� �Ϸ�)
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
 desc : Module Process Tracking �ܰ� ����
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
 desc : Module Process Tracking �ܰ� �Ϸ�
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
 desc : Control Job ID�� ��ϵǾ� �ִ� Carrier ID List ��ȯ
 parm : cj_id		- [in]  Control Job ID
		list_carr	- [out] Carrier List�� ��ȯ�� ����
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
		list_carr	- [out] Carrier List ��ȯ�� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetControlJobCarrierOutputSpec(PTCHAR cj_id, CStringList &list_carr)
{
	// Anderson 0515
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetControlJobCarrierOutputSpec(cj_id, list_carr);
}

/*
 desc : Control Job ID�� ���Ե� Process Job ID List ��ȯ
 parm : cj_id	- [in]  Control Job ID
		list_pj	- [out] Process Job List ��ȯ�� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetControlJobProcessJobList(PTCHAR cj_id, CStringList &list_pj)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetControlJobProcessJobList(cj_id, list_pj);
}

/*
 desc : Control Job�� �ڵ� ���� ���� Ȯ��
 parm : cj_id		- [in]  Control Job ID
		auto_start	- [out] Auto Start ���� �� ��ȯ
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetControlJobStartMethod(PTCHAR cj_id, BOOL &auto_start)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetControlJobStartMethod(cj_id, auto_start);
}

/*
 desc : Control Job�� �۾� �Ϸ� �ð� ���
 parm : cj_id		- [in]  Control Job ID
		date_time	- [out] �۾� �Ϸ� �ð� ���ڿ��� ��ȯ
		size		- [in]  'data_time' ���� ũ��
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetControlJobCompletedTime(PTCHAR cj_id, PTCHAR date_time, UINT32 size)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetControlJobCompletedTime(cj_id, date_time, size);
}

/*
 desc : Control Job�� Material ���� ���� ���
 parm : cj_id	- [in]  Control Job ID
		arrived	- [out] TRUE : ����, FALSE : �̵���
 retn : TRUE or FALSE
 note : CarrierInputSpec �Ӽ����� ��� Carrier���� ��� ���� ���� �� TRUE ��ȯ
*/
API_EXPORT BOOL uvCIMLib_E94CJM_GetControlJobMaterialArrived(PTCHAR cj_id, BOOL &arrived)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->GetControlJobMaterialArrived(cj_id, arrived);
}

/*
 desc : Control Job�� ���� ����
 parm : cj_id	- [in]  Control Job ID
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_SetStartControlJob(PTCHAR cj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->SetStartControlJob(cj_id);
}

/*
 desc : Control Job�� ���� (�۾��� �Ϸ�Ǿ��ų� ���� �߰ų� �� �� ���, ���� ��Ŵ) �˸�
 parm : cj_id	- [in]  Control Job ID
		action	- [in]  �۾� ���� Ȥ�� ���� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_SetStopControlJob(PTCHAR cj_id, E94_CCJA action)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->SetStopControlJob(cj_id, action);
}

/*
 desc : Control Job�� �ߴ� (�۾� ���� �߿� �۾��� �ƿ� ���� ���ѹ���) �˸�
 parm : cj_id	- [in]  Control Job ID
		action	- [in]  �۾� ���� Ȥ�� ���� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_SetAbortControlJob(PTCHAR cj_id, E94_CCJA action)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->SetAbortControlJob(cj_id, action);
}

/*
 desc : Control Job�� ��� ���� (�Ͻ� ����. ��� / �ߴ� �� �ƴ�) �˸�
 parm : cj_id	- [in]  Control Job ID
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_SetPauseControlJob(PTCHAR cj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->SetPauseControlJob(cj_id);
}

/*
 desc : Control Job�� ����� (���� ��������) �˸�
 parm : cj_id	- [in]  Control Job ID
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_SetResumeControlJob(PTCHAR cj_id)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->SetResumeControlJob(cj_id);
}

/*
 desc : Control Job�� ��� (�������� ���� ���¿���... �ƿ� �۾��� ���� ����) �˸�
 parm : cj_id	- [in]  Control Job ID
		action	- [in]  �۾� ���� Ȥ�� ���� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCIMLib_E94CJM_SetCancelControlJob(PTCHAR cj_id, E94_CCJA action)
{
	if (!g_pE30GEM->IsInitedAll())	return FALSE;
	return g_pE94CJM->SetCancelControlJob(cj_id, action);
}

/*
 desc : Control Job�� Queue�� �� ������ �̵� ��Ŵ�� �˸�
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
 desc : Carrier ���� Slot Number�� ���� Destination Substrate ID ���
 parm : carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		slot_no	- [in]  Slot Number (Not sequential number)
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		size	- [in]  'st_id' ���� ũ��
		logs	- [in]  �α� ��� ����
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
 desc : Carrier ���� ����� Slot Number (No)�� ���� Substrate ID �� ��ȯ
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		slot_no	- [in]  Slot Number (1-based or Later)
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		size	- [in]  'st_id' ���� ũ��
		logs	- [in]  �α� ������ ��� ����
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
 desc : Control Job ID�� ��ϵǾ� �ִ� Ư�� Process Job ID�� �۾��� ��� �������� ����
 parm : pj_id	- [in]  Process Job ID
		logs	- [in]  �α� ������ ��� ����
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

	/* Process Job ID�� ���� Substrate ID �Ӽ� (����) ��, Carrier ID�� Slot Number ���� ���*/
	if (!g_pE40PJM->GetPRMtlNameList(pj_id, arrCarrID, arrSlotNo))	return FALSE;
	/* ��ϵ� Substrate ������ŭ �ݺ��Ͽ� Process State �� ��� */
	for (; i<arrCarrID.GetCount(); i++)
	{
		/* ICxSubstratePtr ������ ��� */
		if (!g_pE87CMS->GetSubstratePtrFromSource(arrCarrID[i].GetBuffer(),
												  arrSlotNo[i],
												  pICxSubstPtr))	break;
		/* Slot Number ��ġ�� �ش�Ǵ� Substrate ID ��� */
		wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);
		if (!g_pE90STS->GetSubstrateID(pICxSubstPtr, tzSubstID, CIM_SUBSTRATE_ID))	break;
		/* Substrate ID�� Process ���� �� ��� */
		if (!g_pE90STS->GetSubstrateProcessingState(tzSubstID, enState))	break;
		/* Ȥ�� 1���� Completed ���°� �ƴϸ� ���� FALSE ��, �Ʒ� 3������ �ƴϸ� Completed �ϴٰ� �Ǵ� */
		if (E90_SSPS::en_no_state == enState || E90_SSPS::en_needs_processing == enState ||
			E90_SSPS::en_in_process == enState)	break;
	}
	
	/* ��� ó�� �ߴ��� ���ο� ���� ���� ���� */
	bSucc	= (arrCarrID.GetCount() == i);
	/* �޸� ����  */
	arrSlotNo.RemoveAll();

	return bSucc;
}

/*
 desc : Carrier ���� ��ϵǾ� �־��� Process Job ID ���� ��� SubstrateID�� Transport ��ġ��
		Destination (FOUP; Carrier)�� �ִ��� ����
 parm : pj_id	- [in]  Process Job ID
		logs	- [in]  �α� ������ ��� ����
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

	/* Process Job ID�� ���� Substrate ID �Ӽ� (����) ��, Carrier ID�� Slot Number ���� ���*/
	if (!g_pE40PJM->GetPRMtlNameList(pj_id, arrCarrID, arrSlotNo))	return FALSE;
	/* ��ϵ� Substrate ������ŭ �ݺ��Ͽ� Process State �� ��� */
	for (; i<arrCarrID.GetCount(); i++)
	{
		/* ICxSubstratePtr ������ ��� */
		if (!g_pE87CMS->GetSubstratePtrFromSource(arrCarrID[i].GetBuffer(),
												  arrSlotNo[i],
												  pICxSubstPtr))	break;
		/* Slot Number ��ġ�� �ش�Ǵ� Substrate ID ��� */
		wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);
		if (!g_pE90STS->GetSubstrateID(pICxSubstPtr, tzSubstID, CIM_SUBSTRATE_ID))	break;
		/* Substrate ID�� Process ���� �� ��� */
		if (!g_pE90STS->GetSubstrateTransportState(tzSubstID, enState))	break;
		/* Ȥ�� 1���� Destination ���°� �ƴϸ� ���� FALSE ��, �Ʒ� 3������ �ƴϸ� Destination �ϴٰ� �Ǵ� */
		if (E90_SSTS::en_no_state == enState || E90_SSTS::en_source == enState || E90_SSTS::en_work == enState)	break;
	}
	
	/* ��� ó�� �ߴ��� ���ο� ���� ���� ���� */
	bSucc	= (arrCarrID.GetCount() == i);
	/* �޸� ����  */
	arrSlotNo.RemoveAll();

	return bSucc;
}

/*
 desc : Process Job ID�� �۾��� ���� ��, ���� Process Job ID���� �ڵ����� Completed ��Ŵ
 parm : pj_id	- [in]  Process Job ID
 retn : TRUE or FALSE
 note : Process Job ID�� 1���� ������ �ִ� Control Job ID�� ���� �� �Լ� ȣ���� ��ȿ �մϴ�.
*/
API_EXPORT BOOL uvCIMLib_Mixed_SetAutoJobCompleted(PTCHAR pj_id)
{
	TCHAR tzCRJobID[CIM_CONTROL_JOB_ID_SIZE]	= {NULL};

	/* Get the Control Job ID */
	if (!g_pE40PJM->GetControlJobID(pj_id, tzCRJobID, CIM_CONTROL_JOB_ID_SIZE))	return FALSE;

	/* Process Job ID�� 1���� ������ �ִ� Control Job ���� ���� Ȯ�� */
	if (g_pE94CJM->GetProcessJobCount(tzCRJobID) != 1)	return FALSE;

	/* Process Job Completed & PRJob Completed */
	if (!g_pE40PJM->SetPRJobProcessComplete(pj_id))				return FALSE;
	if (!g_pE40PJM->SetPRJobCompleteOnly(pj_id))				return FALSE;
	/* Control Job Completed */
	if (!g_pE94CJM->SetProcessJobComplete(pj_id, tzCRJobID))	return FALSE;

	return TRUE;
}

/*
 desc : E40PJM::PRCommandCbk ȣ��� ��, ����ϱ� ���� SubstateIDs�� ���� ������ ������ �α�
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

	/* ���� ��� (�۾��� ���)�� Process Job IDs ��� */
	if (!g_pE40PJM->GetProcessAllJobId(arrPRJobIDs))	return FALSE;
	/* ���� ��ϵ� Process Job ID ���� Substate ID ���� ��� */
	for (; i<arrPRJobIDs.GetCount(); i++)
	{
		/* ���� ��ϵ� ����Ʈ ���� */
		arrSubstIDs.RemoveAll();
		/* �� Process Job ID ���� ��ϵ� Substrate ID ����Ʈ ��� */
		if (g_pE40PJM->GetProcessJobAllSubstID(arrPRJobIDs[i].GetBuffer(),
											   arrSubstIDs, arrCarrIDs))
		{
			/* PRJobCommandCbk ȣ��� ��, �ʿ��� ���� ������ ���� ���� �ʱ�ȭ �� �� ��� */
			for (j=0; j<arrSubstIDs.GetCount(); j++)
			{
				memset(&stPRCmdState, 0x00, sizeof(STG_CSDI));
				if (g_pE90STS->GetPRCommandSubstState(arrSubstIDs[i].GetBuffer(),
													  stPRCmdState))
				{
					/* �ӽ������� ����Ʈ �޸𸮿� ��� */
					lstPRCmdState.AddTail(stPRCmdState);
				}
			}
			if (lstPRCmdState.GetCount())
			{
				/* Process Job ID�� ���� ��� ��� */
				if (!g_pE40PJM->GetProcessStartMethod(arrPRJobIDs[i].GetBuffer(), bAutoStart))
					return FALSE;

				if (g_pE40PJM->GetProcessJobState(arrPRJobIDs[i].GetBuffer(), enPRJobState))
				{
					g_pSharedData->UpdatePRJobToSubstID(arrPRJobIDs[i].GetBuffer(), bAutoStart,
														enPRJobState, &lstPRCmdState);
				}
			}
			/* ����Ʈ ���� �޸� ���� */
			lstPRCmdState.RemoveAll();
		}
	}

	/* �ӽ� Array Buffer �޸� ���� */
	arrPRJobIDs.RemoveAll();
	arrSubstIDs.RemoveAll();
	arrCarrIDs.RemoveAll();

	return TRUE;
}

/*
 desc : CJ Paused�Ǿ��� �� PJ���� Pause��Ų��.
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
 desc : CJ�� Executing�Ǿ��� �� PJ�� ó���� �簳�Ѵ�.
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
 desc : CJ�� queued�Ǿ��� �� Material Out Spec�� �����Ѵ�.
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
 desc : Carrier SlotMap Verified�� ��, Material Out Spec�� �����Ѵ�.
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
 desc : ��� CJ�� Material Out Spec�� �����Ѵ�.
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
 desc : Process Job�� �ش��ϴ� substrate state�� ��� Ȯ���ϰ�
		__en_e90_substrate_processing_state__ �� E90_SSPS::en_needs_processing �� ���
		��� E90_SSPS::en_skipped�� ������
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

	/* Process Job ID�� ���� Substrate ID �Ӽ� (����) ��, Carrier ID�� Slot Number ���� ���*/
	if (!g_pE40PJM->GetPRMtlNameList(pj_id, arrCarrID, arrSlotNo))	return FALSE;
	/* ��ϵ� Substrate ������ŭ �ݺ��Ͽ� Process State �� ��� */
	for (; i<arrCarrID.GetCount(); i++)
	{
		/* ICxSubstratePtr ������ ��� */
		if (!g_pE87CMS->GetSubstratePtrFromSource(arrCarrID[i].GetBuffer(), arrSlotNo[i],
												  pICxSubstPtr))	break;
		/* Slot Number ��ġ�� �ش�Ǵ� Substrate ID ��� */
		wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);
		if (!g_pE90STS->GetSubstrateID(pICxSubstPtr, tzSubstID, CIM_SUBSTRATE_ID))	break;
		/* Substrate ID�� Process ���� �� ��� */
		if (!g_pE90STS->GetSubstrateProcessingState(tzSubstID, enState))	break;
		/* Substrate�� process ���°��� E90_SSPS::en_needs_processing�̶�� E90_SSPS::en_skipped�� ���� */
		if (E90_SSPS::en_needs_processing == enState)
		{
			if (!g_pE90STS->SetSubstrateProcessingState(tzSubstID, E90_SSPS::en_skipped))  break;
		}
	}

	/* ��� ó�� �ߴ��� ���ο� ���� ���� ���� */
	bSucc = (arrCarrID.GetCount() == i);
	/* �޸� ����  */
	arrSlotNo.RemoveAll();

	return bSucc;
}

/*
 desc : Check to see if all of the substrates in the ProcessJob are completed and are no longer expected
		to be moved. If so, then let CIM40/CIM94 know that the ProcessJob is complete so it can be deleted
		when the material is removed.
		���� 1) substrates�� ��� destination�� �����Ѱ͸� üũ�ϸ� �ȵ�, �ֳ��ϸ� source�� ��ġ�ص� aborted/stopped��
				ó���� ��쿡�� ������ destination�� �ƴ� source�� ������ complete�� �� �ֱ� ������.
			 2) substrates�� destination�� �����ϱ� ���� PROCESSED state �󿡼� completeó�� �� �� ����.
				���� �ܼ��� substrates�� �� ���°� PROCESSING COMPLETED �ΰ� ������ ��ü complete�� �����ϸ� �ȵ�.
 parm : pj_id	- [in]  Process Job ID
 retn : TRUE (Completed ���� ��) or FALSE (Completed �������� ����)
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

	/* Process Job ID�� ���� Substrate ID �Ӽ� (����) ��, Carrier ID�� Slot Number ���� ���*/
	if (!g_pE40PJM->GetPRMtlNameList(pj_id, arrCarrID, arrSlotNo))	return FALSE;

	/* ��ϵ� Substrate ������ŭ �ݺ��Ͽ� Process State �� ��� */
	for (; i<arrCarrID.GetCount(); i++)
	{
		/* ICxSubstratePtr ������ ��� */
		if (!g_pE87CMS->GetSubstratePtrFromSource(arrCarrID[i].GetBuffer(),
												  arrSlotNo[i], pICxSubstPtr))	break;

		/* Slot Number ��ġ�� �ش�Ǵ� Substrate ID ��� */
		wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);
		if (!g_pE90STS->GetSubstrateID(pICxSubstPtr, tzSubstID, CIM_SUBSTRATE_ID))	break;

		/* Substrate ID�� Process ���� �� ��� */
		if (!g_pE90STS->GetSubstrateProcessingState(tzSubstID, enState))	break;

		/* Substrate ID�� Transport ���� �� ��� */
		if (!g_pE90STS->GetSubstrateTransportState(tzSubstID, trpState))	break;

		/* Substrate�� transport state���� E90_SSTS::en_work, �� at work �̰ų� */
		/* process state�� needs processing ���� in process �̸� FALSE ��ȯ */
		if (E90_SSTS::en_work == trpState ||
			!((E90_SSPS::en_needs_processing != enState) && (E90_SSPS::en_in_process != enState)))
			return bSucc; // Return false, ������ ��ٷ����ϴ� substrates�� ����	
	}

	/* ��� ó�� �ߴ��� ���ο� ���� ���� ���� */
	bSucc = (arrCarrID.GetCount() == i);
	if (!bSucc)	return FALSE;

	/* ��� üũ�ؼ� �ش� ���� ���� ���ٸ� PRJob���� complete */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetPRJobComplete::get_ParentControlJob process_id [%s]", pj_id);
	g_pLogData->PushLogs(tzMesg);
	hResult = g_pE40PJM->SetPRJobComplete(pj_id);
	if (g_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Parent control job id �� ��������*/
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobID from process_id [%s]", pj_id);
	g_pLogData->PushLogs(tzMesg);
	hResult = g_pE40PJM->GetControlJobID(pj_id, tzCRJobID, CIM_CONTROL_JOB_ID_SIZE);
	if (g_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Process Job Complete ó�� (E94CJM) */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"E94CJM->SetPRJobComplete process_id / control_id : [%s] / [%s]",
			   pj_id, (LPCTSTR)_bstr_t(tzCRJobID));
	g_pLogData->PushLogs(tzMesg);
	hResult = g_pE94CJM->SetProcessJobComplete(tzCRJobID, pj_id);
	if (g_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* �޸� ����  */
	arrSlotNo.RemoveAll();

	return bSucc;
}

/*
 desc : This method is called to start the next ProcessJob/ControlJob before the currently
		running ProcessJobs/ControlJobs run out of material to process. 
		�� �Լ��� ���� ���ư��� ProcessJobs/ControlJobs�� material�� �� �����ϰ� ������ ���� �̸� �ľ��Ͽ� 
		���� ProcessJobs/ControlJobs�� ������ �� �ֵ��� �غ��ϴ� ����. �� cascading ������� �����ص� ����. 
		�� ����� cascade, sequential, concurrent���� �ְ�  Philoptics�� ��� Ư���� �´� ���ÿ� ���� ���� �� �� ����. 
		������ ProcessJobs/ControlJobs���� ������ �� ������ ���� Job�� �� �Ŀ� �����ϴ� ����� sequential �̶�� �θ�.
		-���� ����: 1) ��� CJ�� �����ͼ� SELECTED�� ����� ��, �ִٸ� �� �Լ��� �׳� ����.
					2) ���� �������� PJ�� ������ SlotID�� �����ͼ� SubstrateID�� ��ȯ�ϰ� �� SubstrateID�� LocationID�� ������. 
						Subst.ID: CARRIER1.03 �̷� ����
					3) IF: ���� CJ�� ��� PJ�� Ȯ���Ͽ� Queued�� PJ�� �ִٸ� ControlJob.StartNextProcessJob() ����
					4) ELSE: ��� CJ�� �����ͼ� Queued�� CJ�� �ִٸ� CIM94.StartNextControlJob() ����

		[�߿�] ó�� Job�� ���۵ɶ��� �� �Լ��� üũ�ؾ���. 
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

	/* Parent control job id �� �������� */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobID from process_id [%s]", pj_id);
	g_pLogData->PushLogs(tzMesg);
	hResult = g_pE40PJM->GetControlJobID(pj_id, tzCRJobID, CIM_CONTROL_JOB_ID_SIZE);
	if (g_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	
	/* Process Jobs �߿��� Queued ������ ���� �ִ��� Ȯ���� (CJ�� �ش��ϴ� PJ list ����) */
	uvCIMLib_E94CJM_GetControlJobProcessJobList(tzCRJobID, lstPRJobIDs);  
	/* PJ list �߿� Queued �����ΰ��� �ִ��� Ȯ�� */
	for (int i = 0; i < lstPRJobIDs.GetCount(); i++)
	{
		/* ���� Process Job State �� ��� */
		hResult = g_pE40PJM->GetProcessJobState(pj_id, enState);
		if (g_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
		if (enState == E40_PPJS::en_queued)	bQueuedPRJobID = TRUE;
	}

	if (arrActiveIDs.GetCount() == 0)	/* ���°� SELECTED(ACTIVE)�� CJ�� ���°��, �������� 2)3)4) */
										/*        Ȯ�ο� ���� startNextProcessJob() ȣ����          */
	{
		/* CJ�� �ش��ϴ� ��� PJs ���¸� ���� QUEUED �� ��� ControlJob.StartNextProcessJob() ���� */
		if (bQueuedPRJobID)
		{
			hResult = g_pE94CJM->StartNextProcessJob(tzCRJobID);
			if (g_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
		}
		else
		{
			/* ��� CJs�߿� QUEUED �� �ִٸ� CIM94.StartNextControlJob() ���� */
			if (bQueuedCRJobID)
			{
				hResult = g_pE94CJM->StartNextControlJob();
				if (g_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
			}
		}
	}
	else	/* ACTIVE ������ CJ�� �ִ� ���, �� �ȿ� SELECTED�� �ִ��� Ȯ�� �� �ִٸ� �ƹ��͵� ���� */
			/*   (active �ȿ� selected�ܿ� waiting for start, executing, paused���� �����Ѵٸ�?)    */
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
