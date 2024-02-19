
/*
 desc : SEMI �۾� ���� ������
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
 desc : ������
 parm : shared	- [in]  Shared Object ������
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
	/* Global Object ���� */
	m_pSharedData	= shared;
	m_pLogData		= logs;
	m_pE30GEM		= e30gem;
	m_pE40PJM		= e40pjm;
	m_pE87CMS		= e87cms;
	m_pE90STS		= e90sts;
	m_pE94CJM		= e94cjm;
}

/*
 desc : �ı���
 parm : None
*/
CWorkThread::~CWorkThread()
{
}

/*
 desc : ������ ����� �ֱ��� ȣ���
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
 desc : ���� ���񽺰� ���� ������ ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkThread::IsServiceRun()
{
	if (!m_pE30GEM)	return FALSE;
	if (!m_pE30GEM->IsInitConnected())	return FALSE;
	/*return m_pE30GEM->IsServiceRun();*/	/* ���� �ٸ� �����忡�� ȣ���ϸ� ������ �� */
	return m_pE30GEM->IsInitCompleted();
}

/*
 desc : ���� ��ϵ� PRJob ID ���� ���
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
	CAtlList <E40_PPJS>	lstPRJobState;	/* ���� ��ϵ� PRJob ID�� ���� ����Ʈ�� ����� ���� */

	/* ���� ��ϵ� PRJob ID ��� */
	if (!m_pE40PJM->GetProcessAllJobIdState(arrPRJobID, lstPRJobState))	return;

	/* Process Job ������ ���� */
	for (i=0; i<arrPRJobID.GetCount(); i++)
	{
		/* Process Job ID�� ���� Substrate ID ����Ʈ (���) ��� (Carrier ID�� ���Ե�) */
		if (!m_pE40PJM->GetProcessJobAllSubstID(arrPRJobID[i].GetBuffer(),
												arrSubstID, arrCarrID))	break;
		/* Substrate ID ��� ���� ��ȿ�� Ȯ�� */
		for (j=0; j<arrSubstID.GetCount(); j++)
		{
			/* Carrier ID�� Load Port ���� ���ü��� �ο��ϱ� �������� ���� �Լ��� ���� ���� ���� ���� */
#if 0	
			if (!m_pE87CMS->IsVerifiedCarrierID(arrCarrID[i].GetBuffer()))
			{
				arrPRJobID.RemoveAll();
				lstPRJobState.RemoveAll();
				return;
			}
#endif
			/* �ش� Substrate�� ������ �ִ� Carrier�� Verification OK ���� Ȯ�� */
			if (!m_pE87CMS->GetCarrierIDSlotMapStatus(arrCarrID[j].GetBuffer(), enStateSlot))
			{
				arrPRJobID.RemoveAll();
				lstPRJobState.RemoveAll();
				return;
			}
			if (enStateSlot == E87_SMVS::en_sm_verification_ok)
			{
				/* Substrate ID�� �ش�Ǵ� ���� ���� ���� ��ȯ */
				memset(&stPRCmd, 0x00, sizeof(STG_CSDI));
				if (m_pE90STS->GetPRCommandSubstState(arrSubstID[j].GetBuffer(), stPRCmd))
				{
					/* �޸𸮿� ��� */
					lstState.AddTail(stPRCmd);
#ifdef _DEBUG
					TRACE(L"carr_id	= %s\t slot_no = %d  proc_state=%d, trans_state=%d\n",
						  stPRCmd.carr_id, stPRCmd.slot_no, stPRCmd.proc_state, stPRCmd.trans_state);
#endif
				}
			}
		}

		/* Process Job ID�� ���� ��� ��� */
		if (!m_pE40PJM->GetProcessStartMethod(arrPRJobID[i].GetBuffer(), bAutoStart))
		{
			lstState.RemoveAll();
			break;
		}
		/* PRJob ID�� ���� ��ü Substate ID�� ���� ���� ���� */
		enPRJobState= lstPRJobState.GetAt(lstPRJobState.FindIndex(i));
		bIsChecked	= m_pSharedData->UpdatePRJobToSubstID(arrPRJobID[i].GetBuffer(), bAutoStart,
														  enPRJobState, &lstState);

		/* ���� ��ϵ� �޸� �ʱ�ȭ */
		if (lstState.GetCount())	lstState.RemoveAll();
		arrSubstID.RemoveAll();
		arrCarrID.RemoveAll();
	}

	/* ���� ��ϵ� �޸� �ʱ�ȭ */
	arrPRJobID.RemoveAll();
	lstPRJobState.RemoveAll();
}

/*
 desc : ���� ��ϵ� CRJob ID ���� ���
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
	CAtlList <E94_CCJS>	lstCRJobState;	/* ���� ��ϵ� CRJob ID�� ���� ����Ʈ�� ����� ���� */

	/* ���� ��ϵ� CRJob ID ��� */
	if (!m_pE94CJM->GetControlJobList(arrCRJobIDs, lstCRJobState))	return;

	/* Process Job ������ ���� */
	for (i=0; i<arrCRJobIDs.GetCount(); i++)
	{
		/* Control Job ID�� ���� Process Job ID ����Ʈ (���) ��� */
		if (!m_pE94CJM->GetProcessJobList(arrCRJobIDs[i].GetBuffer(), arrPRJobIDs))	break;;
		/* Control Job ID�� ���� Carrier ID ����Ʈ (���) ��� */
		if (!m_pE94CJM->GetControlJobCarrierInputSpec(arrCRJobIDs[i].GetBuffer(), lstCarrIDs))	break;
		/* Auto Start ���� */
		if (!m_pE94CJM->GetControlJobStartMethod(arrCRJobIDs[i].GetBuffer(), bAutoStart))	break;


		/* CRJob ID�� ���õ� �Ӽ� ���� ���� */
		enCRJobState= lstCRJobState.GetAt(lstCRJobState.FindIndex(i));
		bIsChecked	= m_pSharedData->UpdateCRJobToProperty(arrCRJobIDs[i].GetBuffer(),
														   enCRJobState, bAutoStart,
														   &arrPRJobIDs, &lstCarrIDs);
		/* ���� ��ϵ� �޸� �ʱ�ȭ */
		arrPRJobIDs.RemoveAll();
		lstCarrIDs.RemoveAll();
	}

	/* ���� ��ϵ� �޸� �ʱ�ȭ */
	arrCRJobIDs.RemoveAll();
	lstCRJobState.RemoveAll();
}

/*
 desc : Load Port�� Carrier ID�� ���� ���� ����
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
