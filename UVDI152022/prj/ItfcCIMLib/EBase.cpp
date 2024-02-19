
#include "pch.h"
#include "MainApp.h"
#include "EBase.h"


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
CEBase::CEBase(CLogData *logs, CSharedData *share)
{
	m_pGIT				= NULL;

	m_pICxClientApp		= NULL;
	m_pICxClientTool	= NULL;
	m_pICxClientData	= NULL;

	m_pICxHostEmulator	= NULL;
	m_pICxVersion		= NULL;
	m_pICxDiagnostics	= NULL;

	m_pICxEMService		= NULL;
	m_pEMServiceCbk		= NULL;

	m_pICxEMAppCbk		= NULL;
	m_pICxEMAppCbk2		= NULL;
	m_pICxEMAppCbk3		= NULL;
	m_pICxEMAppCbk5		= NULL;
	m_pICxEMMsgCbk		= NULL;

	m_pICxAbsLayerPtr	= NULL;
	m_pICxE39Ptr		= NULL;
	m_pICxE39ObjPtr		= NULL;
	m_pICxE40PJMPtr		= NULL;
	m_pICxE87CMSPtr		= NULL;
	m_pICxE90STPtr		= NULL;
	m_pICxE94CJMPtr		= NULL;
	m_pICxE116ObjPtr	= NULL;
	m_pICxE157ObjPtr	= NULL;

	m_dwCookie			= 0;
	m_gdICxE			= GUID_NULL;
	m_lEquipID			= 0;
	m_lConnID			= 0;

	m_pLogData			= logs;
	m_pSharedData		= share;

	wmemset(m_tzGUID, 0x00, 128);
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CEBase::~CEBase()
{
	/* Global Interface ���� */
	CloseGlobalInterface();
}

/*
 desc : �ʱ�ȭ (�ݵ�� ȣ��)
 parm : equip_id	- [in]  Equipment ID (0 or later. ȯ�� ���Ͽ� ������ ����-1)
		conn_id		- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
 retn : TRUE or FALSE
*/
BOOL CEBase::Init(LONG equip_id, LONG conn_id)
{
	m_lEquipID	= equip_id;
	m_lConnID	= conn_id;

	/* GUID ��ȿ�� �˻� */
	if (m_gdICxE != GUID_NULL)
	{
		/* GUID (for structure) -> String && ��ȿ�� �˻� */
		if (StringFromGUID2(m_gdICxE, m_tzGUID, 128) < 1)	return FALSE;
	}

	/* Init the Global Interface */
	if (!InitGlobalInterface())	return FALSE;

	return TRUE;
}

/*
 desc : ����
 parm : None
 retn : None
*/
VOID CEBase::Close()
{
	CloseGlobalInterface();

	m_lEquipID	= 0;
	m_lConnID	= 0;
}

/*
 desc : Global Interface�� ������ ���
 parm : ppv	- [out] Globalo Interface Pointer
 retn : TRUE or FALSE
*/
BOOL CEBase::QueryIGIT(VOID *ppv)
{
	HRESULT hResult	= S_OK;

	m_pLogData->PushLogs(L"GetInterfaceFromGlobal", m_tzGUID);
	hResult = m_pGIT->GetInterfaceFromGlobal(m_dwCookie, m_gdICxE, &ppv);
	if (FAILED(m_pLogData->CheckResult(hResult, L"GetInterfaceFromGlobal CheckResult")))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : Global Interface ���̺� �ʱ�ȭ
 parm : None
 retn : TRUE or FALE
*/
BOOL CEBase::InitGlobalInterface()
{
	HRESULT hResult	= S_OK;

	// Access to the Global Interface Table
	m_pLogData->PushLogs(L"pIGlobalInterfaceTable.CreateInstance", L"CLSID_StdGlobalInterfaceTable");
	hResult	= m_pGIT.CreateInstance(CLSID_StdGlobalInterfaceTable);
	if (FAILED(m_pLogData->CheckResult(hResult, L"IGlobalInterfaceTable::CreateInstance")))	return FALSE;

	return TRUE;
}

/*
 desc : Global Interface ���̺� ����
 parm : None
 retn : None
*/
VOID CEBase::CloseGlobalInterface()
{
	HRESULT hResult	= S_OK;

	if (m_pGIT && m_pGIT.GetInterfacePtr() && m_dwCookie)
	{
		m_pLogData->PushLogs(L"RevokeInterfaceFromGlobal", L"IID_ICxExxx");
		if (m_dwCookie)	m_pGIT->RevokeInterfaceFromGlobal(m_dwCookie);
		m_pLogData->CheckResult(hResult, L"RevokeInterfaceFromGlobal IID_ICxExxx");
		m_pGIT->Release();

		/* Cookie �� �ʱ�ȭ */
		m_dwCookie	= 0;
	}
}

/*
 desc : ICxValueDispPtr List ���ο� ����� ���� �ڵ� ���� ��ȯ
 parm : value_disp	- [in]  ICxValueDispPtr
		value_type	- [in]  Value Type
 retn : TRUE or FALSE
 note : ���� �ڵ� �� �ǹ�
		case 'caack'
			0x00 : ok done
			0x01 : invalid command
			0x02 : cannot perform now
			0x03 : invalid data or argument
			0x04 : initiated for asynchronous completion
			0x05 : rejected - invalid state
			0x06 : command performed with errors
		case 'svcack'
			0x00 : ok done
			0x01 : unknown service
			0x02 : cannot do now
			0x03 : 1 or more parameters invalid
			0x04 : initiated for asynchronous completion
			0x05 : not completed or prohibited
			0x06 : no such object
*/
BOOL CEBase::DispErrorData(ICxValueDispPtr value_disp, E30_SVET value_type)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	UINT8 u8IsError	= 0x00;
	LONG lItemCount	= 0, lHandle[3]	= {NULL}, i = 1, lError = 0;
	HRESULT hResult	= S_OK;
	CComBSTR bstrError;

	if (!value_disp)	return FALSE;

	/* �ֻ����� �θ� �ڵ� ��� */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DispErrorData::ListAt");
	/*m_pLogData->PushLogs(tzMesg);*/
	hResult	= value_disp->ListAt(0, 0, &lHandle[0]);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* �θ� �ڵ鿡 ����� ����Ʈ�� ù ��° �׸� (���� �ڵ� ��) �������� */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DispErrorData::GetValueU1 (handle=%d / index=0)",
			   lHandle[0]);
	/*m_pLogData->PushLogs(tzMesg);*/
	switch (value_type)
	{
	case E30_SVET::en_svcack:	hResult	= value_disp->GetValueBinary(lHandle[0], 1, &u8IsError);			break;
	case E30_SVET::en_caack	:	hResult	= value_disp->GetValueU1(lHandle[0], 1, &u8IsError);				break;
	default					:	m_pLogData->PushLogs(L"DispErrorData::Unknown Value Type");	return FALSE;	break;
	}
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* ������ ������ �ٷ� ���� */
	if (u8IsError < 0x01)	return TRUE;

	/* �θ� �ڵ鿡 ����� �� ��° ����Ʈ�� �ڵ� ��� */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DispErrorData::ListAt (handle=0 / index=1)");
 	/*m_pLogData->PushLogs(tzMesg);*/
	hResult	= value_disp->ListAt(lHandle[0], 2, &lHandle[1]);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* �� ��° ����Ʈ�� �ڵ鿡 ��ϵ� List ���� ��� */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DispErrorData::ItemCount (handle=%d)", lHandle[0]);
	/*m_pLogData->PushLogs(tzMesg);*/
	hResult	= value_disp->ItemCount(lHandle[1], &lItemCount);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* ���� �޽����� ���ԵǾ� ���� ������ ���� */
	if (lItemCount < 1)
	{
		m_pLogData->PushLogs(L"Does not contain error messages");
		return TRUE;
	}

	/* �� ��° ����Ʈ�� ���� �׸��� List ���� �� ��� */
	for (; i<=lItemCount; i++)
	{
		/* �� ��° ����Ʈ�� ���� �׸��� �ڵ� ��� */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DispErrorData::ListAt (handle=%d)", lHandle[1]);
		/*m_pLogData->PushLogs(tzMesg);*/
		hResult	= value_disp->ListAt(lHandle[1], i, &lHandle[2]);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		/* �� ��° ����Ʈ�� ���� �׸� �� ù ��° ���� �ڵ� ��� */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DispErrorData::GetValueU2 (handle=%d / index=%d / 1st)",
				   lHandle[2], i);
		/*m_pLogData->PushLogs(tzMesg);*/
		value_disp->GetValueU2(lHandle[2], 1, &lError);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		/* �� ��° ����Ʈ�� ���� �׸� �� �� ��° ���� ���ڿ� ��� */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DispErrorData::GetValueAscii (handle=%d / index=%d / 2nd)",
				   lHandle[2], i);
		/*m_pLogData->PushLogs(tzMesg);*/
		hResult	= value_disp->GetValueAscii(lHandle[2], 2, &bstrError);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		/* ���� ���� ��� */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DispErrorData:Error Message (Code:%d, Mesg:%s)",
				   lError, LPCTSTR(_bstr_t(bstrError)));
		m_pLogData->PushLogs(tzMesg, ENG_GALC::en_warning);
	}

	return (hResult == S_OK);
}

/*
 desc : Variable Name�� �ش�Ǵ� Variable ID ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		name	- [in]  Variable Name
		id		- [out] Variable ID�� ��ȯ�Ǿ� �����
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
BOOL CEBase::GetVarNameToID(LONG conn_id, PTCHAR name, LONG &id, BOOL logs)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;
	CComBSTR bstrName (name);

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetVarNameToID::GetVarID (%s)", name);
	if (logs)	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientTool->GetVarID(conn_id, bstrName, &id);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Event Name�� �ش�Ǵ� Event ID ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		name	- [in]  Event Name
		id		- [out] Event ID�� ��ȯ�Ǿ� �����
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
BOOL CEBase::GetEventNameToID(LONG conn_id, PTCHAR name, LONG &id, BOOL logs)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;
	CComBSTR bstrName (name);

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetEventNameToID::GetEventID (%s)", name);
	if (logs)	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientTool->GetEventID(conn_id, bstrName, &id);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Variable ID�� �ش�Ǵ� Variable Name ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		id		- [in]	Variable ID
		name	- [out] Variable Name�� ��ȯ�Ǵ� ����
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
BOOL CEBase::GetVarIDToName(LONG conn_id, LONG id, PTCHAR name, UINT32 size, BOOL logs)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG lCount = 0, lEventID;
	HRESULT hResult		= S_OK;
	VARIANT_BOOL varBoPrivate, varBoPersistent;
	VarType varType;
	ValueType valType;
	CComBSTR bstrName, bstrDesc, bstrUnit;
	ICxValueDispPtr pICxValueCur (CLSID_CxValueObject);
	ICxValueDispPtr pICxValueMin (CLSID_CxValueObject);
	ICxValueDispPtr pICxValueMax (CLSID_CxValueObject);
	ICxValueDispPtr pICxValueDef (CLSID_CxValueObject);

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetVarIDToName::GetVarInfo");
	if (logs)	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientTool->GetVarInfo(conn_id, &id, &bstrName, pICxValueCur, &varType,
										   &valType, &lEventID, &bstrDesc, &bstrUnit,
										   pICxValueMin, pICxValueMax, pICxValueDef,
										   &varBoPrivate, &varBoPersistent);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Variable Name ��ȯ */
	swprintf_s(name, size, L"%s", (LPCTSTR)_bstr_t(bstrName));

	return (hResult == S_OK);
}

/*
 desc : Variable ID�� �ش�Ǵ� Variable Description ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		id		- [in]	Variable ID
		desc	- [out] Variable Description�� ��ȯ�Ǵ� ����
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
BOOL CEBase::GetVarIDToDesc(LONG conn_id, LONG id, PTCHAR desc, UINT32 size, BOOL logs)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG lCount = 0, lEventID;
	HRESULT hResult		= S_OK;
	VARIANT_BOOL varBoPrivate, varBoPersistent;
	VarType varType;
	ValueType valType;
	CComBSTR bstrName, bstrDesc, bstrUnit;
	ICxValueDispPtr pICxValueCur (CLSID_CxValueObject);
	ICxValueDispPtr pICxValueMin (CLSID_CxValueObject);
	ICxValueDispPtr pICxValueMax (CLSID_CxValueObject);
	ICxValueDispPtr pICxValueDef (CLSID_CxValueObject);

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetVarIDToDesc::GetVarInfo");
	if (logs)	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientTool->GetVarInfo(conn_id, &id, &bstrName, pICxValueCur, &varType,
										   &valType, &lEventID, &bstrDesc, &bstrUnit,
										   pICxValueMin, pICxValueMax, pICxValueDef,
										   &varBoPrivate, &varBoPersistent);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Variable Description ��ȯ */
	swprintf_s(desc, size, L"%s", (LPCTSTR)_bstr_t(bstrDesc));

	return (hResult == S_OK);
}

/*
 desc : �˶� ID�� ���� ���� ��ȯ
 parm : id		- [in]  �˶� ID
		desc	- [out] �˶� ������ ��ȯ�� ����
		size	- [in]  'desc' ���� ũ��
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
BOOL CEBase::GetAlarmIDToDesc(LONG id, PTCHAR desc, UINT32 size, BOOL logs)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG lSetEvent, lClearEvent;
	HRESULT hResult		= S_OK;
	_bstr_t bstDesc;
	CComBSTR bstrName, bstrText, bstrDesc;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetAlarmIDToDesc::GetAlarmInfo (alarm_id=%d)", id);
	if (logs)	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientTool->GetAlarmInfo(&id, &bstrName, &bstrText, &lSetEvent, &lClearEvent, &bstrDesc);
	m_pLogData->CheckResult(hResult, tzMesg);

	/* �� ��ȯ */
	bstDesc	= _bstr_t(bstrDesc);

	/* ��ȯ ���� �ʱ�ȭ �� �� ���� */
	wmemset(desc, 0x00, size);
	wmemcpy_s(desc, size, (LPCTSTR)bstDesc, bstDesc.length());

	return (hResult == S_OK);
}

/*
 desc : �˶� ID�� ���� Name ��ȯ
 parm : id		- [in]  �˶� ID
		name	- [out] �˶� ��Ī�� ��ȯ�� ����
		size	- [in]  'name' ���� ũ��
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
 retn : TRUE or FALSE
*/
BOOL CEBase::GetAlarmIDToName(LONG id, PTCHAR name, UINT32 size, BOOL logs)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG lSetEvent, lClearEvent;
	HRESULT hResult		= S_OK;
	_bstr_t bstName;
	CComBSTR bstrName, bstrText, bstrDesc;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetAlarmIDToName::GetAlarmInfo (alarm_id=%d)", id);
	if (logs)	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientTool->GetAlarmInfo(&id, &bstrName, &bstrText,
											 &lSetEvent, &lClearEvent, &bstrDesc);
	m_pLogData->CheckResult(hResult, tzMesg);

	/* �� ��ȯ */
	bstName	= _bstr_t(bstrName);

	/* ��ȯ ���� �ʱ�ȭ �� �� ���� */
	wmemset(name, 0x00, size);
	wmemcpy_s(name, size, (LPCTSTR)bstName, bstName.length());

	return (hResult == S_OK);
}

/*
 desc : SafeArray�� ���� ���� ���ϱ�
 parm : sa	- [in]  SafeArray ��ü ������
		dim	- [in]  SafeArray�� ������ �迭 ���� (1����:1, 2����:2, 3����:3, ...)
 retn : ��ϵ� ���� (����. ��� ����)
*/
LONG CEBase::GetSafeArrayCount(CComSafeArray <LONG> &sa, LONG dim)
{
	LONG lBoundL, lBoundU;	/* �� ó���� �� ������ ������ �ε��� */
	HRESULT hResult	= S_OK;

	/* ���� �߻�(?)�� �˶��� ���� ���ϱ� */
	hResult	= SafeArrayGetLBound(sa, 1/* Dimension */, &lBoundL);
	hResult	= SafeArrayGetUBound(sa, 1/* Dimension */, &lBoundU);
	if (FAILED(hResult))
	{
		m_pLogData->CheckResult(hResult, L"GetSafeArrayCount::SafeArrayGetL(U)Bound");
		return -1L;
	}
	/* ���� ���� �� �����Ǵ� �˶��� ���� ��ȯ */
	return (lBoundU - lBoundL + 1);
}
LONG CEBase::GetSafeArrayCount(CComSafeArray <BSTR> &sa, LONG dim)
{
	LONG lBoundL, lBoundU;	/* �� ó���� �� ������ ������ �ε��� */
	HRESULT hResult	= S_OK;

	/* ���� �߻�(?)�� �˶��� ���� ���ϱ� */
	hResult	= SafeArrayGetLBound(sa, 1/* Dimension */, &lBoundL);
	hResult	= SafeArrayGetUBound(sa, 1/* Dimension */, &lBoundU);
	if (FAILED(hResult))
	{
		m_pLogData->CheckResult(hResult, L"GetSafeArrayCount::SafeArrayGetL(U)Bound");
		return -1L;
	}
	/* ���� ���� �� �����Ǵ� �˶��� ���� ��ȯ */
	return (lBoundU - lBoundL + 1);
}

/*
 desc : Cache the value of a variable, any type, in CIMConnect.
		The variable can be type Status Variable, Data Variable or Equipment Constant.
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		name	- [in]  variable name
		value	- [in]  The variable's new value
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID;1 or 2)
		index	- [in]	�Ϲ������� 0 ��������, ����Ʈ�� ��� Slot ID (Wafer ID;1 ~ Max)
 retn : None
*/
BOOL CEBase::CacheVariable(LONG conn_id, LONG var_id, ICxValueDisp* value, LONG handle, LONG index)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult;
	CComBSTR varName;
    VariableResults varResults;
    CComVariant varBuffer;

 	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"CacheVariable::CopyToByteStream (conn_id=%d, var_id=%d)",
			   conn_id, var_id);
	m_pLogData->PushLogs(tzMesg);
    hResult	= value->CopyToByteStream(handle, index, &varBuffer);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

  	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"CacheVariable::SetValueFromByteBuffer (conn_id=%d, var_id=%d)",
			   conn_id, var_id);
	m_pLogData->PushLogs(tzMesg);
    hResult	= m_pICxClientApp->SetValueFromByteBuffer(conn_id, varANY,
													  &var_id, &varName, varBuffer, &varResults);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Cache the value of one variable of type ASCII in CIMConnect.
		The variable can be type Status Variable, Data Variable or Equipment Constant.
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		var_id	- [in]  variable id
		value	- [in]  variable value
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID;1 or 2)
		index	- [in]	�Ϲ������� 0 ��������, ����Ʈ�� ��� Slot ID (Wafer ID;1 ~ Max)
 retn : TRUE or FALSE
*/
BOOL CEBase::SetCacheAscii(LONG conn_id, LONG var_id, PTCHAR value, LONG handle, LONG index)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult;
	CComBSTR bstrValue = value;
	ICxValueDispPtr pICxValueDisp (CLSID_CxValueObject);

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"CacheVariable::SetValueAscii (var_id=%d, value=%s)",
			   var_id, value);
	m_pLogData->PushLogs(tzMesg);
	hResult	= pICxValueDisp->SetValueAscii(handle, index, bstrValue);
	m_pLogData->CheckResult(hResult, tzMesg);
	if (hResult == S_OK)	CacheVariable(conn_id, var_id, pICxValueDisp, handle, index);

	return (hResult == S_OK);
}

/*
 desc : Cache the value of multiple variables (Status Variable, Data Variable and/or Equipment Constant) in CIMConnect. 
		The variable values are all passed to CIMConnect at once. The value numberOfVariables must match the length of 
		array variableNames and the list length of variableValues.
 parm : host_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		conn_id	- [in]  Number of variables updated
		var_name- [in]  variable name
		var_val	- [in]  variable value
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
BOOL CEBase::SetCacheVariables(LONG conn_id, CComSafeArray<BSTR> &var_name,
							   ICxValueDisp* var_val, LONG handle)
{
	LONG i = 0, lNumberOfVariables;
	HRESULT hResult;
    CComVariant varNames = var_name, varTemp, varValues, varResults /* left as VT_EMPTY on purpose, to allow return of results. */, varIDs /* left as VT_EMPTY on purpose. Names are used. */;
	CComSafeArray<VARIANT> saVarValues;

    /* Create values safe array. Put the names and values into the BSTR and VARIANT safe arrays. */
	var_val->ItemCount(handle, &lNumberOfVariables);
	for (;i<lNumberOfVariables;i++)
	{
		// Convert the list element from ICxValueDisp object into a byte array 
		var_val->CopyToByteStream(handle, i+1, &varTemp);
		saVarValues.Add(varTemp);
	}
	varValues = saVarValues;

	// Pass the values to CIMConnect (SetValuesFromByteBuffer is faster than SetValues) 
	m_pLogData->PushLogs(L"CacheVariables::SetValuesFromByteBuffer");
	hResult = m_pICxClientApp->SetValuesFromByteBuffer(conn_id, varANY,
													   &varIDs, &varNames, varValues, &varResults);
	m_pLogData->CheckResult(hResult, L"CacheVariables::SetValuesFromByteBuffer");

	// This does not, but could check vVariableResults to check for errors. 

	return (hResult == S_OK);
}

/*
 desc : CIMConnect ������ ĳ�� ���ۿ� �� ���� (ASCII / String / Wide / List / Array ����)
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		type	- [in]  �ԷµǴ� �� (Value)�� Type
		var_id	- [in]  variable id
		value	- [in]  �� (type�� ����, �������� ������ ��)
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID;1 or 2)
		index	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� 1 or Later
 retn : TRUE or FALSE
*/
BOOL CEBase::SetCacheValue(LONG conn_id, E30_GPVT type, LONG var_id, UNG_CVVT value,
						   LONG handle, LONG index)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult	= S_FALSE;
	ICxValueDispPtr pICxValueDisp (CLSID_CxValueObject);

	/* ������ ������ �ʴ� ���� ������ ���, ���� ó�� */
	switch (type)
	{
	case E30_GPVT::en_i8	:
	case E30_GPVT::en_u8	:
	case E30_GPVT::en_list	:
	case E30_GPVT::en_w		:
	case E30_GPVT::en_jis8	:
	case E30_GPVT::en_array	:
		m_pLogData->PushLogs(L"SetVariableValue::Unused variable type", ENG_GALC::en_warning);		return FALSE;
	}

	switch (type)
	{
	case E30_GPVT::en_i1	:	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetCacheValue (var_id=%d, value=%d, handle=%d, index=%d)",	var_id, value.i8_value,	handle, index);	break;
	case E30_GPVT::en_i2	:	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetCacheValue (var_id=%d, value=%d, handle=%d, index=%d)",	var_id, value.i16_value,handle, index);	break;
	case E30_GPVT::en_i4	:	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetCacheValue (var_id=%d, value=%I32d, handle=%d, index=%d)",var_id, value.i32_value,handle, index);break;
	case E30_GPVT::en_i8	:	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetCacheValue (var_id=%d, value=%I64d, handle=%d, index=%d)",var_id, value.i64_value,handle, index);break;
	case E30_GPVT::en_u1	:	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetCacheValue (var_id=%d, value=%u, handle=%d, index=%d)",	var_id, value.u8_value,	handle, index);	break;
	case E30_GPVT::en_u2	:	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetCacheValue (var_id=%d, value=%u, handle=%d, index=%d)",	var_id, value.u16_value,handle, index);	break;
	case E30_GPVT::en_u4	:	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetCacheValue (var_id=%d, value=%I32u, handle=%d, index=%d)",var_id, value.u32_value,handle, index);break;
	case E30_GPVT::en_u8	:	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetCacheValue (var_id=%d, value=%I64u, handle=%d, index=%d)",var_id, value.u64_value,handle, index);break;
	case E30_GPVT::en_f4	:	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetCacheValue (var_id=%d, value=%.3f, handle=%d, index=%d)",var_id, value.f_value,	handle, index);	break;
	case E30_GPVT::en_f8	:	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetCacheValue (var_id=%d, value=%.6f, handle=%d, index=%d)",var_id, value.d_value,	handle, index);	break;
	case E30_GPVT::en_b		:	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetCacheValue (var_id=%d, value=%d, handle=%d, index=%d)",	var_id, value.b_value,	handle, index);	break;
	case E30_GPVT::en_bi	:	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetCacheValue (var_id=%d, value=%u, handle=%d, index=%d)",	var_id, value.bi_value,	handle, index);	break;
	case E30_GPVT::en_w		:
	case E30_GPVT::en_a		:	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetCacheValue (var_id=%d, value=%s, handle=%d, index=%d)",	var_id, value.s_value,	handle, index);	break;
	}

	switch (type)
	{
	case E30_GPVT::en_i1	:	hResult	= pICxValueDisp->SetValueI1(handle, index, value.i8_value);				break;
	case E30_GPVT::en_i2	:	hResult	= pICxValueDisp->SetValueI2(handle, index, value.i16_value);			break;
	case E30_GPVT::en_i4	:	hResult	= pICxValueDisp->SetValueI4(handle, index, long(value.i32_value));		break;
	case E30_GPVT::en_i8	:	hResult	= pICxValueDisp->SetValueI8(handle, index, long(value.i64_value));		break;
	case E30_GPVT::en_u1	:	hResult	= pICxValueDisp->SetValueU1(handle, index, value.u8_value);				break;
	case E30_GPVT::en_u2	:	hResult	= pICxValueDisp->SetValueU2(handle, index, long(value.u16_value));		break;
	case E30_GPVT::en_u4	:	hResult	= pICxValueDisp->SetValueU4(handle, index, long(value.u32_value));		break;
	case E30_GPVT::en_u8	:	hResult	= pICxValueDisp->SetValueU8(handle, index, long(value.u64_value));		break;
	case E30_GPVT::en_f4	:	hResult	= pICxValueDisp->SetValueF4(handle, index, value.f_value);				break;
	case E30_GPVT::en_f8	:	hResult	= pICxValueDisp->SetValueF8(handle, index, value.d_value);				break;
	case E30_GPVT::en_b		:	hResult	= pICxValueDisp->SetValueBoolean(handle, index, value.b_value);			break;
	case E30_GPVT::en_bi	:	hResult	= pICxValueDisp->SetValueBinary(handle, index, value.bi_value);			break;
	case E30_GPVT::en_w		:
	case E30_GPVT::en_a		:	hResult	= pICxValueDisp->SetValueAscii(handle, index, CComBSTR(value.s_value));	break;
	}

	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	if (!CacheVariable(conn_id, var_id, pICxValueDisp, handle, index))	return FALSE;

	return TRUE;
}

/*
 desc : Get the current value of a Status Variable, Data Variable or Equipment Constant from CIMConnect.
		The variable must be type integer (I1, I2, I4, U1, U2, U4, Binary), and not be of type array for this function to work.
		Returns FALSE if there is an error.
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		var_id	- [in]  ID of the Status Variable, Data Variable or Equipment Constant
		value	- [out] �ӽ� �����Ͱ� ����� ���� ������ (�Ҵ�� �޸𸮰� �ƴ�)
		size	- [in]  'value' ������ ũ��
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID;1 or 2)
		index	- [in]	�Ϲ������� 0 ��������, ����Ʈ�� ��� Slot ID (Wafer ID;1 ~ Max)
 retn : TRUE or FALSE
*/
BOOL CEBase::GetVariableAscii(LONG conn_id, LONG var_id, PTCHAR value, UINT32 size, LONG handle, LONG index)
{
	HRESULT hResult;
	_bstr_t bstrAscii;
	VariableResults vResult;
	CComBSTR varName, varAscii;
	CComVariant varValue;
	ICxValueDispPtr pICxValueDisp (CLSID_CxValueObject);

	/* Query the current value, which is returned as a byte buffer (faster than ::GetValue) */
	hResult = m_pICxClientApp->GetValueToByteBuffer(conn_id, &var_id, &varName, &varValue, &vResult);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxClientApplication::GetValueToByteBuffer")))	return FALSE;

	/* Convert the byte buffer into an ICxValueDisp object */
	hResult = pICxValueDisp->RestoreFromByteStream(handle, index, varValue);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxValueDispPtr::RestoreFromByteStream")))	return FALSE;

	/* Query the variable value's type. */
	ValueType valType;
	hResult = pICxValueDisp->GetDataType(handle, index, &valType);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxValueDispPtr::GetDataType")))	return FALSE;

	/* Get the value from the ICxValueDisp object, and put the value into the 'value' integer. 
	   This is the code that restricts this function to handle values of type integer. */
	hResult = pICxValueDisp->GetValueAscii(handle, index, &varAscii);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxValueDispPtr::GetValueAscii")))	return FALSE;

	/* ���� ���� ���ϸ� ... */
	if (!varAscii)	return FALSE;

	/* �� ��ȯ */
	bstrAscii	= _bstr_t(varAscii);

	/* ��ȯ ���� �ʱ�ȭ */
	swprintf_s(value, size, L"%s", (LPCTSTR)bstrAscii);

	return TRUE;
}

/*
 desc : Cache the value of multiple variables (Status Variable, Data Variable and/or Equipment Constant) in CIMConnect
		while triggering an event. This ensure that any event reports have the correct data.
		This is ideal for updating data variables associated with the event. 
		The variable values are all passed to CIMConnect at once.
		The value numberOfVariables must match the length of array variableNames and the list length of variableValues.
 parm : conn_id		- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		data_names	- [in]  CCOMSafeArray of variable names
		data_values	- [in]  List of variable values
		var_id		- [in]  Collection Event ID
		var_name	- [in]  Collection Event Name
		handle		- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� 1 or Later
 retn : TRUE or FALSE
*/
BOOL CEBase::SendCollectionEventWithData(LONG conn_id,
										 CComSafeArray<BSTR> &data_names,
										 ICxValueDisp* data_values,
										 PTCHAR var_name, LONG handle)
{
	LONG lVarID	= -1;
#if 0
	if (!GetVarNameToID(conn_id, var_name, lVarID, TRUE))	return FALSE;
#else
	if (!GetEventNameToID(conn_id, var_name, lVarID, TRUE))	return FALSE;
#endif
	return SendCollectionEventWithData(conn_id, data_names, data_values, lVarID, handle);
}
BOOL CEBase::SendCollectionEventWithData(LONG conn_id,
										 CComSafeArray<BSTR> &data_names,
										 ICxValueDisp* data_values,
										 LONG var_id, LONG handle)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult;
	LONG i = 0, lNumberOfVariables;
	CComBSTR bstrName;
    CComVariant varNames= data_names, varTemp, varValues, varResults /* left as VT_EMPTY on purpose */, varIDs /* left as VT_EMPTY on purpose, since names are used */;
	CComSafeArray<VARIANT> saVarValues;

   /* Create values safe array. Put the variable values into the BSTR and VARIANT arrays. */
	data_values->ItemCount(handle, &lNumberOfVariables);
	for (;i<lNumberOfVariables;i++)
	{
		/* Each value is put into a separate byte buffer object. */		
		data_values->CopyToByteStream(handle, i+1, &varTemp);
		saVarValues.Add(varTemp);
	}
    varValues = data_values;

	/* Pass the variables values and event to CIMConnect. */
 	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SendCollectionEventWithData::SetValuesTriggerEvent (var_id=%d)", var_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxClientApp->SetValuesTriggerEvent(conn_id, varANY, &varIDs, &varNames,
													 varValues, &var_id, &bstrName, &varResults);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Convert a SECS-II message ID into the stream number and function number. 
 parm : mesg_id	- [in]  SECS-II Message ID
		mesg_sid- [out] Stream Number
		mesg_fid- [out] Function Number
 retn : None
*/
VOID CEBase::MsgIDtoSF(INT32 mesg_id, INT32 &mesg_sid, INT32 &mesg_fid)
{
	mesg_sid	= (mesg_id >> 8) & 0xff;
	mesg_fid	= mesg_id & 0xff;
}

/*
 desc : Convert a stream number and function number into a SECS-II message ID
 parm : str_num	- [out] Stream Number
		func_num- [out] Function Number
 retn : SECS-II Message ID
*/
INT32 CEBase::SFtoMsgID(INT32 mesg_sid, INT32 mesg_fid)
{
    return (mesg_sid << 8) | mesg_fid;
}
