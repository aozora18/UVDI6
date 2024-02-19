
#pragma once

#include "../../inc/cim300/LogData.h"
#include "../../inc/cim300/SharedData.h"

class CE30GEM;

class CEBase
{
public:
	CEBase(CLogData *logs, CSharedData *share);
	~CEBase();

/* 구조체 */
protected:

/* 가상 함수 */
protected:
public:


/* 로컬 변수 */
protected:

	TCHAR					m_tzGUID[128];

	LONG					m_lEquipID;		/* Equiptment ID 즉, DI 장비 ID */
	LONG					m_lConnID;		/* Connection ID 즉, Client ID */

	DWORD					m_dwCookie;		/* Global Table에 등록된 Cookie Number */

	GUID					m_gdICxE;		/* Structure Object : ICxExxxObject */

	/* ----------------------------------- */
	/*              Interface              */
	/* ----------------------------------- */

	ICxClientApplicationPtr	m_pICxClientApp;
	ICxClientToolPtr		m_pICxClientTool;
	ICxClientDataPtr		m_pICxClientData;

	ICxHostEmulatorPtr		m_pICxHostEmulator;
	ICxVersionPtr			m_pICxVersion;

	ICxDiagnosticsPtr		m_pICxDiagnostics;

	ICxEMServicePtr			m_pICxEMService;
	IEMSvcCbkPtr			m_pEMServiceCbk;

	ICxAbsLayerPtr			m_pICxAbsLayerPtr;
	ICCAlObjectPtr			m_pICCAlObjectPtr;

	ICxE39Ptr				m_pICxE39Ptr;
	ICxE39ObjPtr			m_pICxE39ObjPtr;
	ICxE40PJMPtr			m_pICxE40PJMPtr;
	ICxE87CMSPtr			m_pICxE87CMSPtr;
	ICxE90STPtr				m_pICxE90STPtr;
	ICxE94CJMPtr			m_pICxE94CJMPtr;
	ICxE116ObjectPtr		m_pICxE116ObjPtr;
	ICxE157ObjectPtr		m_pICxE157ObjPtr;

	IGlobalInterfaceTablePtr	m_pGIT;

	/* ----------------------------------- */
	/*              Callback               */
	/* ----------------------------------- */

	ICxEMAppCallbackPtr		m_pICxEMAppCbk;
	ICxEMAppCallback2Ptr	m_pICxEMAppCbk2;
	ICxEMAppCallback3Ptr	m_pICxEMAppCbk3;
	ICxEMAppCallback5Ptr	m_pICxEMAppCbk5;
	ICxEMMsgCallbackPtr		m_pICxEMMsgCbk;

	/* For Common Object Pointer */
	CLogData				*m_pLogData;
	CSharedData				*m_pSharedData;


/* 로컬 함수 */
protected:

	/* ----------------------------------------------------------------------------------------- */
	/*                                     Override Function                                     */
	/* ----------------------------------------------------------------------------------------- */
	BOOL				Init(LONG equip_id, LONG conn_id);	/* 반드시 Init 함수 호출 순서 지켜야 됨. E30GEM -> CIM90 -> CIM87 -> CIM40 -> CIM94 -> CIM116 -> CIM 148 -> CIM157 */
	VOID				Close();
	BOOL				QueryIGIT(VOID *ppv);
	/* ----------------------------------------------------------------------------------------- */

	BOOL				InitGlobalInterface();
	VOID				CloseGlobalInterface();

	LONG				GetSafeArrayCount(CComSafeArray <LONG> &sa, LONG dim);
	LONG				GetSafeArrayCount(CComSafeArray <BSTR> &sa, LONG dim);

	BOOL				CacheVariable(LONG conn_id, LONG var_id, ICxValueDisp* value,
									  LONG handle=0, LONG index=0);

	BOOL				SendCollectionEventWithData(LONG conn_id,
													CComSafeArray<BSTR> &data_names,
													ICxValueDisp* data_values,
													LONG var_id, LONG handle=0);
	BOOL				SendCollectionEventWithData(LONG conn_id,
													CComSafeArray<BSTR> &data_names,
													ICxValueDisp* data_values,
													PTCHAR var_name, LONG handle=0);

	VOID				MsgIDtoSF(INT32 mesg_id, INT32 &mesg_sid, INT32 &mesg_fid);
	INT32				SFtoMsgID(INT32 mesg_sid, INT32 mesg_fid);


/* 공용 함수 */
public:


	ICxClientApplicationPtr	GetICxClientAppPtr()	{	return m_pICxClientApp;		}
	ICxClientToolPtr		GetICxClientToolPtr()	{	return m_pICxClientTool;	}
	ICxClientDataPtr		GetICxClientDataPtr()	{	return m_pICxClientData;	}

	ICxHostEmulatorPtr		GetICxHostEmulatorPtr()	{	return m_pICxHostEmulator;	}
	ICxVersionPtr			GetICxVersionPtr()		{	return m_pICxVersion;		}

	ICxEMServicePtr			GetICxEMServicePtr()	{	return m_pICxEMService;		}
	IEMSvcCbkPtr			GetIEMSvcCbkPtr()		{	return m_pEMServiceCbk;		}

	ICxDiagnosticsPtr		GetICxDiagnosticsPtr()	{	return m_pICxDiagnostics;	}

	ICxEMAppCallbackPtr		GetICxEMAppCbkPtr()		{	return m_pICxEMAppCbk;		}
	ICxEMAppCallback2Ptr	GetICxEMAppCbk2Ptr()	{	return m_pICxEMAppCbk2;		}
	ICxEMAppCallback3Ptr	GetICxEMAppCbk3Ptr()	{	return m_pICxEMAppCbk3;		}
	ICxEMAppCallback5Ptr	GetICxEMAppCbk5Ptr()	{	return m_pICxEMAppCbk5;		}
	ICxEMMsgCallbackPtr		GetICxEMMsgCbkPtr()		{	return m_pICxEMMsgCbk;		}

	ICxAbsLayerPtr			GetICxAbsLayerPtr()		{	return m_pICxAbsLayerPtr;	}
	ICCAlObjectPtr			GetICCAlObjectPtr()		{	return m_pICCAlObjectPtr;	}

	ICxE39Ptr				GetICxE39Ptr()			{	return m_pICxE39Ptr;		}
	ICxE39ObjPtr			GetICxE39ObjPtr()		{	return m_pICxE39ObjPtr;		}
	ICxE40PJMPtr			GetICxE40PJMPtr()		{	return m_pICxE40PJMPtr;		}
	ICxE87CMSPtr			GetICxE87CMSPtr()		{	return m_pICxE87CMSPtr;		}
	ICxE90STPtr				GetICxE90STPtr()		{	return m_pICxE90STPtr;		}
	ICxE94CJMPtr			GetICxE94CJMPtr()		{	return m_pICxE94CJMPtr;		}
	ICxE116ObjectPtr		GetICxE116ObjectPtr()	{	return m_pICxE116ObjPtr;	}
	ICxE157ObjectPtr		GetICxE157ObjectPtr()	{	return m_pICxE157ObjPtr;	}

	VOID				SetICxClientAppPtr(ICxClientApplicationPtr ptr)	{	m_pICxClientApp		= ptr;	}
	VOID				SetICxClientToolPtr(ICxClientToolPtr ptr)		{	m_pICxClientTool	= ptr;	}
	VOID				SetICxClientDataPtr(ICxClientDataPtr ptr)		{	m_pICxClientData	= ptr;	}

	VOID				SetICxHostEmulatorPtr(ICxHostEmulatorPtr ptr)	{	m_pICxHostEmulator	= ptr;	}
	VOID				SetICxVersionPtr(ICxVersionPtr ptr)				{	m_pICxVersion		= ptr;	}

	VOID				SetICxEMServicePtr(ICxEMServicePtr ptr)			{	m_pICxEMService		= ptr;	}
	VOID				SetIEMSvcCbkPtr(IEMSvcCbkPtr ptr)				{	m_pEMServiceCbk		= ptr;	}

	VOID				SetICxDiagnosticsPtr(ICxDiagnosticsPtr ptr)		{	m_pICxDiagnostics	= ptr;	}

	VOID				SetICxEMAppCbkPtr(ICxEMAppCallbackPtr ptr)		{	m_pICxEMAppCbk		= ptr;	}
	VOID				SetICxEMAppCbk2Ptr(ICxEMAppCallback2Ptr ptr)	{	m_pICxEMAppCbk2		= ptr;	}
	VOID				SetICxEMAppCbk3Ptr(ICxEMAppCallback3Ptr ptr)	{	m_pICxEMAppCbk3		= ptr;	}
	VOID				SetICxEMAppCbk5Ptr(ICxEMAppCallback5Ptr ptr)	{	m_pICxEMAppCbk5		= ptr;	}
	VOID				SetICxEMMsgCbkPtr(ICxEMMsgCallbackPtr ptr)		{	m_pICxEMMsgCbk		= ptr;	}

	VOID				SetICxAbsLayerPtr(ICxAbsLayerPtr ptr)			{	m_pICxAbsLayerPtr	= ptr;	}
	VOID				SetICxE39Ptr(ICxE39Ptr ptr)						{	m_pICxE39Ptr		= ptr;	}
	VOID				SetICxE39ObjPtr(ICxE39ObjPtr ptr)				{	m_pICxE39ObjPtr		= ptr;	}
	VOID				SetICxE40PJMPtr(ICxE40PJMPtr ptr)				{	m_pICxE40PJMPtr		= ptr;	}
	VOID				SetICxE87CMSPtr(ICxE87CMSPtr ptr)				{	m_pICxE87CMSPtr		= ptr;	}
	VOID				SetICxE90STPtr(ICxE90STPtr ptr)					{	m_pICxE90STPtr		= ptr;	}
	VOID				SetICxE94CJMPtr(ICxE94CJMPtr ptr)				{	m_pICxE94CJMPtr		= ptr;	}
	VOID				SetICxE116ObjectPtr(ICxE116ObjectPtr ptr)		{	m_pICxE116ObjPtr	= ptr;	}
	VOID				SetICxE157ObjectPtr(ICxE157ObjectPtr ptr)		{	m_pICxE157ObjPtr	= ptr;	}

	BOOL				GetVarNameToID(LONG conn_id, PTCHAR name, LONG &id, BOOL logs=FALSE);
	BOOL				GetVarIDToName(LONG conn_id, LONG id, PTCHAR name, UINT32 size, BOOL logs=FALSE);
	BOOL				GetVarIDToDesc(LONG conn_id, LONG id, PTCHAR desc, UINT32 size, BOOL logs=FALSE);
	BOOL				GetEventNameToID(LONG conn_id, PTCHAR name, LONG &id, BOOL logs=FALSE);
	BOOL				GetAlarmIDToDesc(LONG id, PTCHAR alarm_desc, UINT32 size, BOOL logs=FALSE);
	BOOL				GetAlarmIDToName(LONG id, PTCHAR name, UINT32 size, BOOL logs=FALSE);


	BOOL				DispErrorData(ICxValueDispPtr value_disp, E30_SVET value_type);

	BOOL				SetCacheValue(LONG conn_id, E30_GPVT type, LONG var_id, UNG_CVVT value, LONG handle=0, LONG index=0);
	BOOL				SetCacheAscii(LONG conn_id, LONG var_id, PTCHAR value, LONG handle=0, LONG index=0);
	BOOL				GetVariableAscii(LONG conn_id, LONG var_id, PTCHAR value, UINT32 size, LONG handle=0, LONG index=0);
	BOOL				SetCacheVariables(LONG conn_id, CComSafeArray<BSTR> &var_name, ICxValueDisp* var_val, LONG handle=0);
};

