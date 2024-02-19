/* EMService Callback 선언 */

#pragma once

#include "resource.h"       // 주 기호입니다.
#include "ItfcCIMCbk_i.h"

/* 외부 공유 객체 연결 */
#include "../../inc/cim300/LogData.h"
#include "../../inc/cim300/SharedData.h"

using namespace ATL;

class CE40CmdParam;

// CEMSvcCbk
class ATL_NO_VTABLE CEMSvcCbk :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CEMSvcCbk, &CLSID_EMSvcCbk>,
	public IDispatchImpl<IEMSvcCbk, &IID_IEMSvcCbk, &LIBID_ItfcCIMCbkLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDispatchImpl<ICxEMAppCallback, &__uuidof(ICxEMAppCallback), &LIBID_EMSERVICELib, /* wMajor = */ 1>,
	public IDispatchImpl<ICxEMAppCallback2, &__uuidof(ICxEMAppCallback2), &LIBID_EMSERVICELib, /* wMajor = */ 1>,
	public IDispatchImpl<ICxEMAppCallback3, &__uuidof(ICxEMAppCallback3), &LIBID_EMSERVICELib, /* wMajor = */ 1>,
	public IDispatchImpl<ICxEMAppCallback4, &__uuidof(ICxEMAppCallback4), &LIBID_EMSERVICELib, /* wMajor = */ 1>,
	public IDispatchImpl<ICxEMAppCallback5, &__uuidof(ICxEMAppCallback5), &LIBID_EMSERVICELib, /* wMajor = */ 1>,
	public IDispatchImpl<ICxEMAppCallback6, &__uuidof(ICxEMAppCallback6), &LIBID_EMSERVICELib, /* wMajor = */ 1>,
	public IDispatchImpl<ICxEMAppDataCallback, &__uuidof(ICxEMAppDataCallback), &LIBID_EMSERVICELib, /* wMajor = */ 1>,
	public IDispatchImpl<ICxEMMsgCallback,&__uuidof(ICxEMMsgCallback),&LIBID_EMSERVICELib, /* wMajor = */ 1>,
	public IDispatchImpl<ICxE87AsyncCarrierTagCallback, &__uuidof(ICxE87AsyncCarrierTagCallback), &LIBID_CMSLib, /* wMajor = */ 1, /* wMinor = */ 1>,
	public IDispatchImpl<ICxE87Callback, &__uuidof(ICxE87Callback), &LIBID_CMSLib, /* wMajor = */ 1, /* wMinor = */ 1>,
	public IDispatchImpl<ICxE87Callback2, &__uuidof(ICxE87Callback2), &LIBID_CMSLib, /* wMajor = */ 1, /* wMinor = */ 1>,
	public IDispatchImpl<ICxE87Callback3, &__uuidof(ICxE87Callback3), &LIBID_CMSLib, /* wMajor = */ 1, /* wMinor = */ 1>,
	public IDispatchImpl<ICxE87Callback4, &__uuidof(ICxE87Callback4), &LIBID_CMSLib, /* wMajor = */ 1, /* wMinor = */ 1>,
	public IDispatchImpl<ICxE87Callback5, &__uuidof(ICxE87Callback5), &LIBID_CMSLib, /* wMajor = */ 1, /* wMinor = */ 1>,
	public IDispatchImpl<ICxE87Callback6, &__uuidof(ICxE87Callback6), &LIBID_CMSLib, /* wMajor = */ 1, /* wMinor = */ 1>,
	public IDispatchImpl<ICxE87Callback7, &__uuidof(ICxE87Callback7), &LIBID_CMSLib, /* wMajor = */ 1, /* wMinor = */ 1>,
	public IDispatchImpl<ICxE87Callback8, &__uuidof(ICxE87Callback8), &LIBID_CMSLib, /* wMajor = */ 1, /* wMinor = */ 1>,
	public IDispatchImpl<ICxE87Callback9, &__uuidof(ICxE87Callback9), &LIBID_CMSLib, /* wMajor = */ 1, /* wMinor = */ 1>,
	public IDispatchImpl<ICxE87Callback10, &__uuidof(ICxE87Callback10), &LIBID_CMSLib, /* wMajor = */ 1, /* wMinor = */ 1>,
	public IDispatchImpl<ICxE87Carrier, &__uuidof(ICxE87Carrier), &LIBID_CMSLib, /* wMajor = */ 1, /* wMinor = */ 1>,
	public IDispatchImpl<ICxE87CMS, &__uuidof(ICxE87CMS), &LIBID_CMSLib, /* wMajor = */ 1, /* wMinor = */ 1>,
	public IDispatchImpl<ICxE39Obj, &__uuidof(ICxE39Obj), &LIBID_E39Lib, /* wMajor = */ 1>,
	
	public IDispatchImpl<ICxE39CB, &__uuidof(ICxE39CB), &LIBID_E39Lib, /* wMajor = */ 1>,
	public IDispatchImpl<ICxE39CB2, &__uuidof(ICxE39CB2), &LIBID_E39Lib, /* wMajor = */ 1>,
	public IDispatchImpl<ICxE39InfoCB, &__uuidof(ICxE39InfoCB), &LIBID_E39Lib, /* wMajor = */ 1>,
	public IDispatchImpl<ICxE39InfoCB2, &__uuidof(ICxE39InfoCB2), &LIBID_E39Lib, /* wMajor = */ 1>,

	public IDispatchImpl<ICxE40PJMCallback, &__uuidof(ICxE40PJMCallback), &LIBID_E40PJMLib, /* wMajor = */ 1>,
	public IDispatchImpl<ICxE40PJMCallback2, &__uuidof(ICxE40PJMCallback2), &LIBID_E40PJMLib, /* wMajor = */ 1>,
	public IDispatchImpl<ICxE94ControlJobCB, &__uuidof(ICxE94ControlJobCB), &LIBID_E94CJMLib, /* wMajor = */ 1>,
	public IDispatchImpl<ICxE94ControlJobCB2, &__uuidof(ICxE94ControlJobCB2), &LIBID_E94CJMLib, /* wMajor = */ 1>,
	public IDispatchImpl<ICxE94ControlJobCB3, &__uuidof(ICxE94ControlJobCB3), &LIBID_E94CJMLib, /* wMajor = */ 1>,
	public IDispatchImpl<ICxE94ControlJobCB4, &__uuidof(ICxE94ControlJobCB4), &LIBID_E94CJMLib, /* wMajor = */ 1>,
	public IDispatchImpl<ICxE90SubstrateCB, &__uuidof(ICxE90SubstrateCB), &LIBID_E90Lib, /* wMajor = */ 1>
#if (USE_SMS_MESSAGE_LIBE)
	,public IDispatchImpl<ICxCommunications, &__uuidof(ICxCommunications), &LIBID_CxSmsLib, /* wMajor = */ 1, /* wMinor = */ 0>,
	public IDispatchImpl<ICxCommunicationsCB, &__uuidof(ICxCommunicationsCB), &LIBID_CxSmsLib, /* wMajor = */ 1, /* wMinor = */ 0>,
	public IDispatchImpl<ICxCommunicationsCB2, &__uuidof(ICxCommunicationsCB2), &LIBID_CxSmsLib, /* wMajor = */ 1, /* wMinor = */ 0>,
	public IDispatchImpl<ICxEMInternalMsg, &__uuidof(ICxEMInternalMsg), &LIBID_MESSAGESLib, /* wMajor = */ 1, /* wMinor = */ 0>,
	public IDispatchImpl<ICxSmsObject, &__uuidof(ICxSmsObject), &LIBID_CxSmsLib, /* wMajor = */ 1, /* wMinor = */ 0>
#endif
{
public:
	/* 생성자 */
	CEMSvcCbk();

	DECLARE_REGISTRY_RESOURCEID(IDR_EMSVCCBK)

	DECLARE_PROTECT_FINAL_CONSTRUCT()
	HRESULT				FinalConstruct();
	VOID				FinalRelease();

/* 로컬 변수 */
protected:

	/* Shared Data Object */
	CLogData			*m_pLogData;
	CSharedData			*m_pSharedData;


/* 로컬 함수 */
protected:

	VOID				PutLogMesg(TCHAR *mesg, ENG_GALC level);
	HRESULT				CheckResult(HRESULT hr, PTCHAR mesg);

	BOOL				DispErrorData(ICxValueDispPtr value_disp, E30_SVET value_type);
	BOOL				E40ParamCmdAnalysis(ICxValueDispPtr parm_list, CE40CmdParam *parm_obj);

	BOOL				E40PRCommandCbk(CComBSTR proc_id,
										E40_PECS cmd_src, E40_PCCC cmd,
										ICxValueDispPtr parm_list,
										ICxProcessJobPtr job_obj,
										ICxValueDispPtr status);
	BOOL				E40PRMtlNameList(CComBSTR proc_id, E40_PCCC cmd, ICxProcessJobPtr job_obj);

	BOOL				CreateCJobID(BSTR job_id, ICxE94ControlJob *object);

	VOID				ParseLogMessage(VARIANT mesg_buff);

	BOOL				ListAtItem(ICxValueDisp *mesg, LONG handle, LONG index, LONG level,
								   CAtlList <STG_CLHC> &item);
	BOOL				GetCxValObjToString(ICxValueDispPtr cx_val, LONG handle, LONG index,
											LPG_TSMV value);


/* 공용 함수  */
public:

/* 메시지 맵 */
protected:

BEGIN_COM_MAP(CEMSvcCbk)
	COM_INTERFACE_ENTRY(IEMSvcCbk)
	COM_INTERFACE_ENTRY2(IDispatch, ICxEMAppCallback)
	COM_INTERFACE_ENTRY(ICxEMAppCallback)
	COM_INTERFACE_ENTRY(ICxEMAppCallback2)
	COM_INTERFACE_ENTRY(ICxEMAppCallback3)
	COM_INTERFACE_ENTRY(ICxEMAppCallback4)
	COM_INTERFACE_ENTRY(ICxEMAppCallback5)
	COM_INTERFACE_ENTRY(ICxEMAppCallback6)
	COM_INTERFACE_ENTRY(ICxEMAppDataCallback)
	COM_INTERFACE_ENTRY(ICxEMMsgCallback)
	COM_INTERFACE_ENTRY(ICxE87AsyncCarrierTagCallback)
	COM_INTERFACE_ENTRY(ICxE87Callback)
	COM_INTERFACE_ENTRY(ICxE87Callback2)
	COM_INTERFACE_ENTRY(ICxE87Callback3)
	COM_INTERFACE_ENTRY(ICxE87Callback4)
	COM_INTERFACE_ENTRY(ICxE87Callback5)
	COM_INTERFACE_ENTRY(ICxE87Callback6)
	COM_INTERFACE_ENTRY(ICxE87Callback7)
	COM_INTERFACE_ENTRY(ICxE87Callback8)
	COM_INTERFACE_ENTRY(ICxE87Callback9)
	COM_INTERFACE_ENTRY(ICxE87Callback10)
	COM_INTERFACE_ENTRY(ICxE87Carrier)
	COM_INTERFACE_ENTRY(ICxE87CMS)
	COM_INTERFACE_ENTRY(ICxE39Obj)
	
	COM_INTERFACE_ENTRY(ICxE39CB)
	COM_INTERFACE_ENTRY(ICxE39CB2)
	COM_INTERFACE_ENTRY(ICxE39InfoCB)
	COM_INTERFACE_ENTRY(ICxE39InfoCB2)
	
	COM_INTERFACE_ENTRY(ICxE40PJMCallback)
	COM_INTERFACE_ENTRY(ICxE40PJMCallback2)
	COM_INTERFACE_ENTRY(ICxE94ControlJobCB)
	COM_INTERFACE_ENTRY(ICxE94ControlJobCB2)
	COM_INTERFACE_ENTRY(ICxE94ControlJobCB3)
	COM_INTERFACE_ENTRY(ICxE94ControlJobCB4)
	COM_INTERFACE_ENTRY(ICxE90SubstrateCB)
#if (USE_SMS_MESSAGE_LIBE)
	COM_INTERFACE_ENTRY(ICxCommunications)
	COM_INTERFACE_ENTRY(ICxCommunicationsCB)
	COM_INTERFACE_ENTRY(ICxCommunicationsCB2)
	COM_INTERFACE_ENTRY(ICxSmsObject)
	COM_INTERFACE_ENTRY(ICxEMInternalMsg)
#endif
END_COM_MAP()

	/* ----------------------------------------------------------------------------------------- */
	/*                                    User Callback Methods                                  */
	/* ----------------------------------------------------------------------------------------- */

public:

	STDMETHOD(SetLogData)(LONG_PTR data);
	STDMETHOD(SetSharedData)(LONG_PTR data);

public:

	/* ----------------------------------------------------------------------------------------- */
	/*                                   ICxEMAppCallback Methods                                */
	/* ----------------------------------------------------------------------------------------- */
	STDMETHOD(AlarmChanged)(long alarm_id, long alarm_code);
	STDMETHOD(AsyncMsgError)(long conn_id, long mesg_id, long trans_id, long error_num, long extra);
	STDMETHOD(CallbackReplaced)(long conn_id, Callbacks callback, long item_id, BSTR item_name)		{	return E_NOTIMPL;	}
	STDMETHOD(CommandCalled)(long conn_id, BSTR command_name, ICxValueDisp *param_names,
							 ICxValueDisp *param_values, CommandResults *cmd_result);
	STDMETHOD(EventTriggered)(long conn_id, long event_id);
	STDMETHOD(GetValue)(long conn_id, long var_id, BSTR var_name, ICxValueDisp *value);
	STDMETHOD(GetValueToByteBuffer)(long conn_id, long var_id, BSTR var_name, VARIANT *value_buff)	{	return E_NOTIMPL;	}
	STDMETHOD(GetValues)(long conn_id, VARIANT var_ids, VARIANT var_names, VARIANT values)			{	return E_NOTIMPL;	}
	STDMETHOD(GetValuesToByteBuffer)(long conn_id, VARIANT var_ids, VARIANT var_names,
									 VARIANT *value_buffs)											{	return E_NOTIMPL;	}
	STDMETHOD(HostPPLoadInqAck)(long conn_id, RecipeGrant result);
	STDMETHOD(HostPPSendAck)(long conn_id, RecipeAck result);
	STDMETHOD(HostTermMsgAck)(long conn_id, TerminalMsgResults result);
	STDMETHOD(MessageReceived)(long conn_id, long mesg_id, ICxValueDisp *mesg,
							   VARIANT_BOOL reply_expected, long trans_id, long *reply_mesg_id,
							   ICxValueDisp *reply_msg, MessageResults *result);
	STDMETHOD(PPData)(long conn_id, ICxValueDisp *recipe, long result)								{	return E_NOTIMPL;	}
	STDMETHOD(PPDataFile)(long conn_id, BSTR recipe_filename, long result)							{	return E_NOTIMPL;	}
	STDMETHOD(PPDelete)(long conn_id, ICxValueDisp *recipe_names, RecipeAck *result)				{	return E_NOTIMPL;	}
	STDMETHOD(PPDRequest)(long conn_id, ICxValueDisp *recipe_names)									{	return E_NOTIMPL;	}
	STDMETHOD(PPLoadInquire)(long conn_id, BSTR recipe_name, long length_bytes,
							 RecipeGrant *result)													{	return E_NOTIMPL;	}
	STDMETHOD(PPRequest)(long conn_id, BSTR recipe_name, ICxValueDisp *recipe, RecipeAck *result);
	STDMETHOD(PPSend)(long conn_id, ICxValueDisp *recipe, RecipeAck *result)						{	return E_NOTIMPL;	}
	STDMETHOD(PPSendFile)(long conn_id, BSTR recipe_filename, RecipeAck *result)					{	return E_NOTIMPL;	}
	STDMETHOD(PPSendFileVerify)(long conn_id, BSTR recipe_filename, RecipeAck *result)				{	return E_NOTIMPL;	}
	STDMETHOD(SetValue)(long conn_id, long var_id, BSTR var_name, ICxValueDisp *value)				{	return E_NOTIMPL;	}
	STDMETHOD(SetValueFromByteBuffer)(long conn_id, long var_id, BSTR var_name, VARIANT value_buff)	{	return E_NOTIMPL;	}
	STDMETHOD(SetValues)(long conn_id, VARIANT var_ids, VARIANT var_names, VARIANT values)			{	return E_NOTIMPL;	}
	STDMETHOD(SetValuesFromByteBuffer)(long conn_id, VARIANT var_ids, VARIANT var_names,
									   VARIANT value_buffs)											{	return E_NOTIMPL;	}
	STDMETHOD(StateChange)(long conn_id, StateMachine machine, long state);
	STDMETHOD(TerminalMsgRcvd)(long conn_id, long term_id, ICxValueDisp *lines,
							   TerminalMsgResults *result);
	STDMETHOD(ValueChanged)(long conn_id, long var_id, BSTR var_name, ICxValueDisp *value);
	STDMETHOD(ValuesChanged)(long conn_id, VARIANT var_ids, VARIANT var_names, VARIANT values)		{	return E_NOTIMPL;	}
	STDMETHOD(VerifyValue)(long conn_id, long var_id, BSTR var_name, ICxValueDisp *value,
						   VerifyValueResults *result)												{	return E_NOTIMPL;	}
	STDMETHOD(VerifyValues)(long conn_id, VARIANT var_ids, VARIANT var_names, VARIANT values,
							VARIANT *results)														{	return E_NOTIMPL;	}

	/* ----------------------------------------------------------------------------------------- */
	/*                                  ICxEMAppCallback2 Methods                                */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(CallbackReplaced2)(long conn_id, Callbacks callback, long item_id, BSTR item_name)	{	return E_NOTIMPL;	}
	STDMETHOD(GetReportValue)(long conn_id, long var_id, BSTR var_name, long rpt_id, long evt_id,
							  ICxValueDisp *value)													{	return E_NOTIMPL;	}

	/* ----------------------------------------------------------------------------------------- */
	/*                                  ICxEMAppCallback3 Methods                                */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(GetValues2)(long conn_id, ICxValueDisp *value)										{	return E_NOTIMPL;	}

	/* ----------------------------------------------------------------------------------------- */
	/*                                  ICxEMAppCallback4 Methods                                */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(ClockChange)(long equip_id, long conn_id);
	STDMETHOD(RequestToChangeClock)(long conn_id, BSTR new_time, VARIANT_BOOL *allow)				{	return E_NOTIMPL;	}

	/* ----------------------------------------------------------------------------------------- */
	/*                                  ICxEMAppCallback5 Methods                                */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(ECChanged)(long conn_id, BSTR var_name, VARIANT new_val);

	/* ----------------------------------------------------------------------------------------- */
	/*                                  ICxEMAppCallback6 Methods                                */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(ValueChanged)(long conn_id, long var_id, BSTR var_name, VARIANT value);

	/* ----------------------------------------------------------------------------------------- */
	/*                                ICxEMAppDataCallback Methods                               */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(EventReport)(__int64 time_stamp, long conn_id, long req_id, long evt_id,
						   VARIANT values)															{	return E_NOTIMPL;	}
	STDMETHOD(ExceptionReport)(__int64 time_stamp, long conn_id, long req_id, long alarm_id,
							   VARIANT_BOOL set, VARIANT values)									{	return E_NOTIMPL;	}
	STDMETHOD(TraceReport)(__int64 time_stamp, long conn_id, long req_id, long sample,
						   VARIANT values)															{	return E_NOTIMPL;	}

	/* ----------------------------------------------------------------------------------------- */
	/*                                ICxEMMsgCallback Methods                                   */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(LogMessage)(VARIANT mesg_buff);
	STDMETHOD(MessageSent)(long conn_id, long mesg_id, long trans_id, VARIANT_BOOL reply,
						   ICxValueDisp *message);

	/* ----------------------------------------------------------------------------------------- */
	/*                                  ICxE87Callback Methods                                   */
	/* ----------------------------------------------------------------------------------------- */
	STDMETHOD(AsyncCarrierTagReadData)(BSTR loc_id, BSTR carr_id, BSTR segment, long length, long trans_id, ICxValueDisp *status);
	STDMETHOD(AsyncCarrierTagWriteData)(BSTR loc_id, BSTR carr_id, BSTR segment, long length, BSTR data, long trans_id, ICxValueDisp *status)	{	return E_NOTIMPL;	}
	STDMETHOD(Bind)(short port_id, BSTR carr_id);
	STDMETHOD(CancelAllCarrierOut)();
	STDMETHOD(CancelBind)(short port_id, BSTR carr_id);
	STDMETHOD(CancelCarrier)(short port_id, BSTR carr_id);
	STDMETHOD(CancelCarrierAtPort)(short port_id);
	STDMETHOD(CancelCarrierNotification)(BSTR carr_id);
	STDMETHOD(CancelCarrierOut)(BSTR carr_id);
	STDMETHOD(CancelReserveAtPort)(short port_id);
	STDMETHOD(CarrierOut)(short port_id, BSTR carr_id);
	STDMETHOD(CarrierNotification)(BSTR carr_id);
	STDMETHOD(CarrierIn)(BSTR carr_id);
	STDMETHOD(CarrierReCreate)(BSTR carr_id, short port_id);
	STDMETHOD(CarrierRelease)(BSTR loc_id, BSTR carr_id, ICxValueDisp *status);
	STDMETHOD(CarrierTagReadData)(BSTR loc_id, BSTR carr_id, BSTR segment, long length, BSTR *data, ICxValueDisp *status);
	STDMETHOD(CarrierTagWriteData)(BSTR loc_id, BSTR carr_id, BSTR segment, long length, BSTR data, ICxValueDisp *status)	{	return E_NOTIMPL;	}
	STDMETHOD(ChangeAccessMode)(short new_mode, short port_id);
	STDMETHOD(ChangeAccessMode)(ICxValueDisp *param_list, ICxValueDisp **status)											{	return E_NOTIMPL;	}
	STDMETHOD(ChangeAccessMode2)(short new_mode, ICxValueDisp *port_list, ICxValueDisp **status)							{	return E_NOTIMPL;	}
	STDMETHOD(ChangeServiceStatus)(short port_id, LoadPortTransferStateEnum state);
	STDMETHOD(ProceedWithCarrier)(short port_id, BSTR carr_id);
	STDMETHOD(ReserveAtPort)(short port_id);
	/* ----------------------------------------------------------------------------------------- */
	/*                                  ICxE87Callback2 Methods                                  */
	/* ----------------------------------------------------------------------------------------- */
	STDMETHOD(AlarmChanged)(BSTR alarm_id, VARIANT_BOOL is_set);
	STDMETHOD(CarrierIDVerified)(short port_id, BSTR carr_id, ICxE87Carrier *pCarrier);
	STDMETHOD(SlotMapVerified)(short port_id, BSTR carr_id, ICxE87Carrier *pCarrier);
	/* ----------------------------------------------------------------------------------------- */
	/*                                  ICxE87Callback3 Methods                                  */
	/* ----------------------------------------------------------------------------------------- */
	STDMETHOD(InServiceTransferState)(short port_id, LoadPortTransferStateEnum *state);
	STDMETHOD(StateChange)(CMSStateMachine machine, long state, short port_id, BSTR carr_id);
	/* ----------------------------------------------------------------------------------------- */
	/*                                  ICxE87Callback4 Methods                                  */
	/* ----------------------------------------------------------------------------------------- */
	STDMETHOD(CarrierRecreateFinalize)(BSTR carr_id, short port_id, ICxValueDisp *pParameterlist);
	/* ----------------------------------------------------------------------------------------- */
	/*                                  ICxE87Callback5 Methods                                  */
	/* ----------------------------------------------------------------------------------------- */
	STDMETHOD(CancelCarrier2)(short port_id, BSTR carr_id, ICxValueDisp **status);
	STDMETHOD(CancelCarrierAtPort2)(short port_id, ICxValueDisp **status);
	/* ----------------------------------------------------------------------------------------- */
	/*                                  ICxE87Callback6 Methods                                  */
	/* ----------------------------------------------------------------------------------------- */
	STDMETHOD(ChangeAccessMode2)(short new_mode, short port_id, int *caack);
	/* ----------------------------------------------------------------------------------------- */
	/*                                  ICxE87Callback7 Methods                                  */
	/* ----------------------------------------------------------------------------------------- */
	STDMETHOD(Bind2)(short port_id, BSTR carr_id, ICxValueDisp *pParameterlist)												{	return E_NOTIMPL;	}
	STDMETHOD(CarrierNotification2)(BSTR carr_id, ICxValueDisp *pParameterlist)												{	return E_NOTIMPL;	}
	STDMETHOD(ProceedWithCarrier2)(short port_id, BSTR carr_id, ICxValueDisp *pParameterlist)								{	return E_NOTIMPL;	}
	/* ----------------------------------------------------------------------------------------- */
	/*                                  ICxE87Callback8 Methods                                  */
	/* ----------------------------------------------------------------------------------------- */
	STDMETHOD(CarrierRemoved)(BSTR carr_id, short port_id);
	STDMETHOD(PortAlarmChanged)(short port_id, BSTR alarm_id, VARIANT_BOOL is_set);
	/* ----------------------------------------------------------------------------------------- */
	/*                                  ICxE87Callback9 Methods                                  */
	/* ----------------------------------------------------------------------------------------- */
	STDMETHOD(ProceedWithCarrier3)(short port_id, BSTR carr_id, ICxValueDisp *pParameterlist, ICxValueDisp *status)			{	return E_NOTIMPL;	}
	/* ----------------------------------------------------------------------------------------- */
	/*                                 ICxE87Callback10 Methods                                  */
	/* ----------------------------------------------------------------------------------------- */
	STDMETHOD(ChangeAccessMode3)(short new_mode, short port_id, int *err_code, BSTR *pErrText, int *caack)					{	return E_NOTIMPL;	}

	/* ----------------------------------------------------------------------------------------- */
	/*                                ICxE87Carrier Methods                                      */
	/* ----------------------------------------------------------------------------------------- */
	STDMETHOD(get_AccessingStatus)(AccessingStatusEnum *pval)																{	return E_NOTIMPL;	}
	STDMETHOD(put_AccessingStatus)(AccessingStatusEnum pval)																{	return E_NOTIMPL;	}
	STDMETHOD(get_AssociatedPortID)(short *pval)																			{	return E_NOTIMPL;	}
	STDMETHOD(put_AssociatedPortID)(short pval)																				{	return E_NOTIMPL;	}
	STDMETHOD(get_Capacity)(long *pval)																						{	return E_NOTIMPL;	}
	STDMETHOD(put_Capacity)(long pval)																						{	return E_NOTIMPL;	}
	STDMETHOD(get_ContentMap)(ICxValueDisp **pval)																			{	return E_NOTIMPL;	}
	STDMETHOD(put_ContentMap)(ICxValueDisp *pval)																			{	return E_NOTIMPL;	}
	STDMETHOD(GetSubstrateId)(int slot, BSTR *substrateId)																	{	return E_NOTIMPL;	}
	STDMETHOD(GetSubstrateIdInSlot)(int slot, BSTR *substrateId)															{	return E_NOTIMPL;	}
	STDMETHOD(SetAttrValues)(ICxValueDisp* pListValue)																		{	return E_NOTIMPL;	}
	STDMETHOD(get_HostProvidedCID)(VARIANT_BOOL *pval)																		{	return E_NOTIMPL;	}
	STDMETHOD(put_HostProvidedCID)(VARIANT_BOOL pval)																		{	return E_NOTIMPL;	}
	STDMETHOD(get_HostProvidedSlotmap)(VARIANT_BOOL *pval)																	{	return E_NOTIMPL;	}
	STDMETHOD(put_HostProvidedSlotmap)(VARIANT_BOOL pval)																	{	return E_NOTIMPL;	}
	STDMETHOD(get_ID)(BSTR *pval)																							{	return E_NOTIMPL;	}
	STDMETHOD(get_IDStatus)(IDStatusEnum *pval)																				{	return E_NOTIMPL;	}
	STDMETHOD(put_IDStatus)(IDStatusEnum pval)																				{	return E_NOTIMPL;	}
	STDMETHOD(Init)(BSTR carr_id, IGemExtendable *pGem)																		{	return E_NOTIMPL;	}
	STDMETHOD(get_Location)(BSTR *pval)																						{	return E_NOTIMPL;	}
	STDMETHOD(put_Location)(BSTR pval)																						{	return E_NOTIMPL;	}
	STDMETHOD(get_ObjectType)(BSTR *pval)																					{	return E_NOTIMPL;	}
	STDMETHOD(get_SlotMap)(ICxValueDisp **pval)																				{	return E_NOTIMPL;	}
	STDMETHOD(put_SlotMap)(ICxValueDisp *pval)																				{	return E_NOTIMPL;	}
	STDMETHOD(get_SlotMapStatus)(SlotMapStatusEnum *pval)																	{	return E_NOTIMPL;	}
	STDMETHOD(put_SlotMapStatus)(SlotMapStatusEnum pval)																	{	return E_NOTIMPL;	}
	STDMETHOD(SlotMapVerified)()																							{	return E_NOTIMPL;	}
	STDMETHOD(get_SubstrateCount)(long *pval)																				{	return E_NOTIMPL;	}
	STDMETHOD(put_SubstrateCount)(long pval)																				{	return E_NOTIMPL;	}
	STDMETHOD(get_Usage)(BSTR *pval)																						{	return E_NOTIMPL;	}
	STDMETHOD(put_Usage)(BSTR pval)																							{	return E_NOTIMPL;	}
	/* ----------------------------------------------------------------------------------------- */
	/*                                  ICxE87CMS Methods                                        */
	/* ----------------------------------------------------------------------------------------- */
	STDMETHOD(AccessCarrier)(BSTR carr_id, AccessingStatusEnum state, VARIANT_BOOL *result)									{	return E_NOTIMPL;	}
	STDMETHOD(Init)(ICxE39Obj *pE39, ICxAbsLayer *abs_layer)																{	return E_NOTIMPL;	}
	STDMETHOD(CreateLoadPort)(short port_id, BSTR port_name, VARIANT_BOOL *result)											{	return E_NOTIMPL;	}
	STDMETHOD(CarrierAtPort)(short port_id, BSTR carr_id, BSTR loc_id, VARIANT_BOOL readerValid, CMSVerification *result)	{	return E_NOTIMPL;	}
	STDMETHOD(CarrierDepartedPort)(short port_id, BSTR carr_id, BSTR loc_id, VARIANT_BOOL keepBlocked, VARIANT_BOOL *result){	return E_NOTIMPL;	}
	STDMETHOD(TransferStart)(short port_id, VARIANT_BOOL *result)															{	return E_NOTIMPL;	}
	STDMETHOD(VerifySlotMap)(BSTR carr_id, ICxValueDisp *slot_map, CMSVerification *result)									{	return E_NOTIMPL;	}
	STDMETHOD(ReadyToUnload)(short port_id, VARIANT_BOOL *result)															{	return E_NOTIMPL;	}
	STDMETHOD(get_TransferStatus)(short port_id, LoadPortTransferStateEnum *pval)											{	return E_NOTIMPL;	}
	STDMETHOD(put_TransferStatus)(short port_id, LoadPortTransferStateEnum pval)											{	return E_NOTIMPL;	}
	STDMETHOD(get_CarrierIDStatus)(BSTR carr_id, IDStatusEnum *pval)														{	return E_NOTIMPL;	}
	STDMETHOD(put_CarrierIDStatus)(BSTR carr_id, IDStatusEnum pval)															{	return E_NOTIMPL;	}
	STDMETHOD(get_SlotMapStatus)(BSTR carr_id, SlotMapStatusEnum *pval)														{	return E_NOTIMPL;	}
	STDMETHOD(put_SlotMapStatus)(BSTR carr_id, SlotMapStatusEnum pval)														{	return E_NOTIMPL;	}
	STDMETHOD(LocateCarrier)(BSTR carr_id, ICxE87Carrier **carrier)															{	return E_NOTIMPL;	}
	STDMETHOD(get_AssociationCarrierID)(short port_id, BSTR *pval)															{	return E_NOTIMPL;	}
	STDMETHOD(put_AssociationCarrierID)(short port_id, BSTR pval)															{	return E_NOTIMPL;	}
	STDMETHOD(RegisterCallback)(ICxE87Callback *pCallback)																	{	return E_NOTIMPL;	}
	STDMETHOD(AddCarrierLocation)(BSTR loc_id)																				{	return E_NOTIMPL;	}
	STDMETHOD(MoveCarrier)(BSTR carr_id, BSTR toLocationID, VARIANT_BOOL keepBlocked)										{	return E_NOTIMPL;	}
	STDMETHOD(CreatePartition)(BSTR partitionID, BSTR partitionType, ICxValueDisp *pLocationIDs)							{	return E_NOTIMPL;	}
	STDMETHOD(get_AccessModeState)(short port_id, LoadPortAccessModeStateEnum *pval)										{	return E_NOTIMPL;	}
	STDMETHOD(put_AccessModeState)(short port_id, LoadPortAccessModeStateEnum pval)											{	return E_NOTIMPL;	}
	STDMETHOD(get_AssociationState)(short port_id, LoadPortCarrierAssociationEnum *pval)									{	return E_NOTIMPL;	}
	STDMETHOD(put_AssociationState)(short port_id, LoadPortCarrierAssociationEnum pval)										{	return E_NOTIMPL;	}
	STDMETHOD(get_ReservationState)(short port_id, LoadPortReservationStateEnum *pval)										{	return E_NOTIMPL;	}
	STDMETHOD(put_ReservationState)(short port_id, LoadPortReservationStateEnum pval)										{	return E_NOTIMPL;	}
	STDMETHOD(get_CarrierAccessingStatus)(BSTR carr_id, AccessingStatusEnum *pval)											{	return E_NOTIMPL;	}
	STDMETHOD(put_CarrierAccessingStatus)(BSTR carr_id, AccessingStatusEnum pval)											{	return E_NOTIMPL;	}
	STDMETHOD(get_AssociationPortID)(BSTR carr_id, short *pval)																{	return E_NOTIMPL;	}
	STDMETHOD(put_AssociationPortID)(BSTR carr_id, short pval)																{	return E_NOTIMPL;	}
	STDMETHOD(SetCIM90Interface)(LPUNKNOWN pCim90)																			{	return E_NOTIMPL;	}
	STDMETHOD(GetCIM90Interface)(LPUNKNOWN *pCim90)																			{	return E_NOTIMPL;	}
	STDMETHOD(CreateCarrier)(BSTR carr_id, BSTR loc_id, ICxE87Carrier **carrier)											{	return E_NOTIMPL;	}
	STDMETHOD(DeleteCarrier)(BSTR loc_id, BSTR carr_id)																		{	return E_NOTIMPL;	}
	STDMETHOD(GetSubstrate)(BSTR carr_id, long slotNumber, LPUNKNOWN *pSubstrate)											{	return E_NOTIMPL;	}
	STDMETHOD(RegisterCallback2)(ICxE87Callback2 *pCallback2)																{	return E_NOTIMPL;	}
	STDMETHOD(Bind)(short port_id, BSTR carr_id, ICxValueDisp *pAttributes, ICxValueDisp **status)							{	return E_NOTIMPL;	}
	STDMETHOD(ChangeServiceStatus)(short port_id, ICxValueDisp *pParameterlist, ICxValueDisp **status)						{	return E_NOTIMPL;	}
	STDMETHOD(CancelBind)(short port_id, BSTR carr_id, ICxValueDisp **status)												{	return E_NOTIMPL;	}
	STDMETHOD(CarrierOut)(short port_id, BSTR carr_id, ICxValueDisp **status)												{	return E_NOTIMPL;	}
	STDMETHOD(CancelCarrier)(short port_id, BSTR carr_id, ICxValueDisp **status)											{	return E_NOTIMPL;	}
	STDMETHOD(ReserveAtPort)(short port_id, ICxValueDisp **status)															{	return E_NOTIMPL;	}
	STDMETHOD(CancelReserveAtPort)(short port_id, ICxValueDisp **status)													{	return E_NOTIMPL;	}
	STDMETHOD(ProceedWithCarrier)(short port_id, BSTR carr_id, ICxValueDisp *pParameterlist, ICxValueDisp **status)			{	return E_NOTIMPL;	}
	STDMETHOD(CancelAllCarrierOut)(ICxValueDisp **status)																	{	return E_NOTIMPL;	}
	STDMETHOD(CancelCarrierAtPort)(short port_id, ICxValueDisp **status)													{	return E_NOTIMPL;	}
	STDMETHOD(CancelCarrierNotification)(BSTR carr_id, ICxValueDisp **status)												{	return E_NOTIMPL;	}
	STDMETHOD(CancelCarrierOut)(BSTR carr_id, ICxValueDisp **status)														{	return E_NOTIMPL;	}
	STDMETHOD(CarrierIn)(BSTR carr_id, ICxValueDisp **status)																{	return E_NOTIMPL;	}
	STDMETHOD(CarrierNotification)(BSTR carr_id, ICxValueDisp *pParameterlist, ICxValueDisp **status)						{	return E_NOTIMPL;	}
	STDMETHOD(CarrierRelease)(BSTR loc_id, BSTR carr_id, ICxValueDisp **status)												{	return E_NOTIMPL;	}
	STDMETHOD(CarrierTagReadData)(BSTR loc_id, BSTR carr_id, BSTR data_seg, long length, BSTR *data, ICxValueDisp **status)	{	return E_NOTIMPL;	}
	STDMETHOD(CarrierTagWriteData)(BSTR loc_id, BSTR carr_id, BSTR data_seg, long length, BSTR data, ICxValueDisp **status)	{	return E_NOTIMPL;	}
	STDMETHOD(Create2LocationLoadPort)(short port_id, BSTR loc_id1, BSTR loc_id2)											{	return E_NOTIMPL;	}
	STDMETHOD(BeginCarrierOut)(short port_id, BSTR carr_id)																	{	return E_NOTIMPL;	}
	STDMETHOD(ChangeLPTransferState)(short port_id, LoadPortTransferStateEnum newState)										{	return E_NOTIMPL;	}
	STDMETHOD(ChangeLPAssociationState)(short port_id, LoadPortCarrierAssociationEnum newState)								{	return E_NOTIMPL;	}
	STDMETHOD(ChangeLPReservationState)(short port_id, LoadPortReservationStateEnum newState)								{	return E_NOTIMPL;	}
	STDMETHOD(Shutdown)()																									{	return E_NOTIMPL;	}
	STDMETHOD(UnregisterCallback)(ICxE87Callback *pCallback)																{	return E_NOTIMPL;	}
	STDMETHOD(UnregisterCallback2)(ICxE87Callback2 *pCallback)																{	return E_NOTIMPL;	}
	STDMETHOD(CarrierReCreate)(BSTR carr_id, short port_id, ICxValueDisp *pParameterlist, ICxValueDisp **status)			{	return E_NOTIMPL;	}
	STDMETHOD(AllocateSlot)(BSTR partitionID)																				{	return E_NOTIMPL;	}
	STDMETHOD(DeallocateSlot)(BSTR partitionID)																				{	return E_NOTIMPL;	}
	STDMETHOD(MoveCarrier2)(BSTR carr_id, BSTR toLocationID, VARIANT_BOOL keepBlocked, VARIANT_BOOL keepAllocated)			{	return E_NOTIMPL;	}
	STDMETHOD(get_AllocatedSlots)(BSTR partitionID, short *pval)															{	return E_NOTIMPL;	}
	STDMETHOD(put_AllocatedSlots)(BSTR partitionID, short pval)																{	return E_NOTIMPL;	}
	STDMETHOD(SubstrateRemoved)(BSTR carr_id, short slot, BSTR *pSubstrateID)												{	return E_NOTIMPL;	}
	STDMETHOD(SubstratePlaced)(BSTR carr_id, short slot, BSTR substrateId)													{	return E_NOTIMPL;	}
	STDMETHOD(get_AllowRemoteControl)(VARIANT_BOOL *pval)																	{	return E_NOTIMPL;	}
	STDMETHOD(put_AllowRemoteControl)(VARIANT_BOOL pval)																	{	return E_NOTIMPL;	}
	STDMETHOD(GetSubstrateFromSource)(BSTR carr_id, long slotNumber, LPUNKNOWN *pSubstrate)									{	return E_NOTIMPL;	}
	STDMETHOD(RegisterCallback3)(ICxE87Callback3 *pCallback)																{	return E_NOTIMPL;	}
	STDMETHOD(UnregisterCallback3)(ICxE87Callback3 *pCallback)																{	return E_NOTIMPL;	}
	STDMETHOD(CarrierPlaced)(short port_id)																					{	return E_NOTIMPL;	}
	STDMETHOD(RegisterCallback4)(ICxE87Callback4 *pCallback)																{	return E_NOTIMPL;	}
	STDMETHOD(UnregisterCallback4)(ICxE87Callback4 *pCallback)																{	return E_NOTIMPL;	}
	STDMETHOD(RegisterCallback5)(ICxE87Callback5 *pCallback)																{	return E_NOTIMPL;	}
	STDMETHOD(UnregisterCallback5)(ICxE87Callback5 *pCallback)																{	return E_NOTIMPL;	}
	STDMETHOD(RegisterCallback6)(ICxE87Callback6 *pCallback)																{	return E_NOTIMPL;	}
	STDMETHOD(UnregisterCallback6)(ICxE87Callback6 *pCallback)																{	return E_NOTIMPL;	}
	STDMETHOD(MoveCarrier3)(BSTR carr_id, BSTR toLocationID)																{	return E_NOTIMPL;	}
	STDMETHOD(RegisterCallback7)(ICxE87Callback7 *pCallback)																{	return E_NOTIMPL;	}
	STDMETHOD(UnregisterCallback7)(ICxE87Callback7 *pCallback)																{	return E_NOTIMPL;	}
	STDMETHOD(RegisterCallback8)(ICxE87Callback8 *pCallback)																{	return E_NOTIMPL;	}
	STDMETHOD(UnregisterCallback8)(ICxE87Callback8 *pCallback)																{	return E_NOTIMPL;	}
	STDMETHOD(get_CIM87LogLevel)(long *pval)																				{	return E_NOTIMPL;	}
	STDMETHOD(put_CIM87LogLevel)(long pval)																					{	return E_NOTIMPL;	}
	STDMETHOD(AddCarrierLocation2)(BSTR loc_id, short subscriptIndex)														{	return E_NOTIMPL;	}
	STDMETHOD(CreateLoadPort2)(short port_id, BSTR port_name, short subscriptIndex, VARIANT_BOOL *result)					{	return E_NOTIMPL;	}
	STDMETHOD(Create2LocationLoadPort2)(short port_id, BSTR loc_id1, short subscriptIndex1, BSTR loc_id2, short subscriptIndex2)	{	return E_NOTIMPL;	}
	STDMETHOD(CreatePartition2)(BSTR partitionID, BSTR partitionType, ICxValueDisp *pLocationIDs, short baseSubscriptIndex)	{	return E_NOTIMPL;	}
	STDMETHOD(ChangeLPSubstrateIndexRange)(short port_id, short baseSubscriptIndex, short SubstrateCount)					{	return E_NOTIMPL;	}
	STDMETHOD(RegisterCallback9)(ICxE87Callback9 *pCallback)																{	return E_NOTIMPL;	}
	STDMETHOD(UnregisterCallback9)(ICxE87Callback9 *pCallback)																{	return E_NOTIMPL;	}
	STDMETHOD(RegisterAsyncCarrierTagCallback)(ICxE87AsyncCarrierTagCallback *pCallback)									{	return E_NOTIMPL;	}
	STDMETHOD(UnregisterAsyncCarrierTagCallback)(ICxE87AsyncCarrierTagCallback *pCallback)									{	return E_NOTIMPL;	}
	STDMETHOD(AsyncCarrierTagReadComplete)(BSTR carr_id, BSTR data, long trans_id, ICxValueDisp *status)					{	return E_NOTIMPL;	}
	STDMETHOD(AsyncCarrierTagWriteComplete)(BSTR carr_id, long trans_id, ICxValueDisp *status)								{	return E_NOTIMPL;	}
	STDMETHOD(RegisterCallback10)(ICxE87Callback10 *pCallback)																{	return E_NOTIMPL;	}
	STDMETHOD(UnregisterCallback10)(ICxE87Callback10 *pCallback)															{	return E_NOTIMPL;	}

	/* ----------------------------------------------------------------------------------------- */
	/*                                  ICxE39OBJ Callback                                       */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(Attach)(ICxValueDisp* attr_sets, ICxValueDisp* status)														{	return E_NOTIMPL;	}
	STDMETHOD(AttachSetAttr)(ICxValueDisp* attr_sets, ICxValueDisp* status)													{	return E_NOTIMPL;	}
	STDMETHOD(AttachSupervisedObject)(BSTR tgt_obj_spec, ICxValueDisp* attr_sets, ICxValueDisp* status, VARIANT reserved)	{	return E_NOTIMPL;	}
	STDMETHOD(Detach)(ICxValueDisp* attr_sets, ICxValueDisp* status)														{	return E_NOTIMPL;	}
	STDMETHOD(DetachSupervisedObject)(BSTR tgt_obj_spec, ICxValueDisp* attr_sets, ICxValueDisp* status, VARIANT reserved)	{	return E_NOTIMPL;	}
	STDMETHOD(GetAttr)(BSTR attr_name, LONG* err_code, BSTR* err_text)														{	return E_NOTIMPL;	}
	STDMETHOD(ObjDelete)(BSTR obj_spec, ICxE39Obj* obj)																		{	return E_NOTIMPL;	}
	STDMETHOD(Reattach)(ICxValueDisp* attr_sets, ICxValueDisp* status)														{	return E_NOTIMPL;	}
	STDMETHOD(SetAttr)(BSTR attr_name, ICxValueDisp* value, LONG* err_code, BSTR* err_text)									{	return E_NOTIMPL;	}

	/* ----------------------------------------------------------------------------------------- */
	/*                                  ICxE39OBJ Callback2                                      */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(SetAttr)(BSTR obj_type, BSTR obj_id, BSTR attr_name, ICxValueDisp* value, ICxValueDisp* err_list)				{	return E_NOTIMPL;	}

	/* ----------------------------------------------------------------------------------------- */
	/*                                ICxE39OBJ Info Callback                                    */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(AttrChange)(ICxE39Obj *obj, BSTR attr_name, ICxValueDisp *value);
	STDMETHOD(ObjAdd)(ICxE39Obj *parent, BSTR obj_spec, ICxE39Obj *obj);
	STDMETHOD(ObjRemove)(ICxE39Obj *parent, BSTR obj_spec, ICxE39Obj *obj);

	/* ----------------------------------------------------------------------------------------- */
	/*                                ICxE39OBJ Info Callback 2                                  */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(AttrChange)(BSTR parent_obj_spec, BSTR obj_id, BSTR obj_type, BSTR attr_name, VARIANT value_buff);
	STDMETHOD(ObjAdd)(BSTR parent_obj_spec, BSTR obj_id, BSTR obj_type, VARIANT attr_name, VARIANT attr_values);
	STDMETHOD(ObjRemove)(BSTR parent_obj_spec, BSTR obj_id, BSTR obj_type, VARIANT attr_name, VARIANT attr_values);

	/* ----------------------------------------------------------------------------------------- */
	/*                                  ICxE39OBJ Methods                                        */
	/* ----------------------------------------------------------------------------------------- */
	STDMETHOD(Init)(BSTR sobj_spec)																					{	return E_NOTIMPL;	}
	STDMETHOD(Add)(ICxE39Obj *child)																				{	return E_NOTIMPL;	}
	STDMETHOD(Remove)(ICxE39Obj *child)																				{	return E_NOTIMPL;	}
	STDMETHOD(LocateObj)(BSTR sobj_spec, ICxE39Obj **objects)														{	return E_NOTIMPL;	}
	STDMETHOD(ListFirstLevelChildObjs)(BSTR sObjType, VARIANT *objects)												{	return E_NOTIMPL;	}
	STDMETHOD(ListAllChildObjs)(BSTR sObjType, VARIANT *objects)													{	return E_NOTIMPL;	}
	STDMETHOD(get_ObjSpec)(BSTR *val)																				{	return E_NOTIMPL;	}
	STDMETHOD(get_ObjType)(BSTR *val)																				{	return E_NOTIMPL;	}
	STDMETHOD(get_ObjID)(BSTR *val)																					{	return E_NOTIMPL;	}
	STDMETHOD(ListAttr)(VARIANT *attribute_names)																	{	return E_NOTIMPL;	}
	STDMETHOD(SetAttr)(BSTR attribute_name, ICxValueDisp *value)													{	return E_NOTIMPL;	}
	STDMETHOD(GetAttr)(BSTR attribute_name, ICxValueDisp *value)													{	return E_NOTIMPL;	}
	STDMETHOD(RemoveAttr)(BSTR attribute_name)																		{	return E_NOTIMPL;	}
	STDMETHOD(RegisterE39Callback)(ICxE39CB *callback)																{	return E_NOTIMPL;	}
	STDMETHOD(UnregisterE39Callback)(ICxE39CB *callback)															{	return E_NOTIMPL;	}
	STDMETHOD(ListE39Callbacks)(VARIANT *callbacks)																	{	return E_NOTIMPL;	}
	STDMETHOD(RegisterE39CreateObjCB)(BSTR sobj_type, ICxE39CreateObjCB *callback)									{	return E_NOTIMPL;	}
	STDMETHOD(UnregisterE39CreateObjCB)(BSTR sobj_type)																{	return E_NOTIMPL;	}
	STDMETHOD(CreateE39Obj)(BSTR sObjType, ICxValueDisp *attribute_settings, ICxValueDisp *status, BSTR *obj_spec)	{	return E_NOTIMPL;	}
	STDMETHOD(RegisterE39DeleteObjCB)(ICxE39DeleteObjCB *callback)													{	return E_NOTIMPL;	}
	STDMETHOD(UnregisterE39DeleteObjCB)()																			{	return E_NOTIMPL;	}
	STDMETHOD(DeleteE39Obj)(ICxValueDisp * pAttributeSettings, ICxValueDisp *status)								{	return E_NOTIMPL;	}
	STDMETHOD(get_ParentSpec)(BSTR *val)																			{	return E_NOTIMPL;	}
	STDMETHOD(put_ParentSpec)(BSTR val)																				{	return E_NOTIMPL;	}
	STDMETHOD(RegisterE39InfoCallback)(ICxE39InfoCB *callback)														{	return E_NOTIMPL;	}
	STDMETHOD(UnregisterE39InfoCallback)(ICxE39InfoCB *callback)													{	return E_NOTIMPL;	}
	STDMETHOD(ObjectAction)(long operation_id, BSTR service, ICxValueDisp *parameter_list, ObjectActionStatus *status, long *link_id, ICxValueDisp *result_list)	{	return E_NOTIMPL;	}
	STDMETHOD(GetServiceNames)(ICxValueDisp *obj_type_list, ICxValueDisp *result_list, ICxValueDisp *status)		{	return E_NOTIMPL;	}
	STDMETHOD(GetServiceParams)(BSTR sobj_type, ICxValueDisp *service_name_list, ICxValueDisp *result_list, ICxValueDisp *status)	{	return E_NOTIMPL;	}
	STDMETHOD(RegisterObjectAction)(BSTR sobj_type, BSTR service, ICxE39ObjActionCB *callback)						{	return E_NOTIMPL;	}
	STDMETHOD(ObjectActionCompleteNotification)(ICxAbsLayer *abs_layer, long operation_id, long link_id, ICxValueDisp *results)	{	return E_NOTIMPL;	}
	STDMETHOD(SaveObjState)(BSTR sobj_spec, VARIANT *save_buffer)													{	return E_NOTIMPL;	}
	STDMETHOD(RestoreObjState)(BSTR sobj_spec, VARIANT restore_buffer)												{	return E_NOTIMPL;	}
	STDMETHOD(RegisterNVSCB)(ICxE39ObjNVSCB *callback)																{	return E_NOTIMPL;	}
	STDMETHOD(RegisterObjectType)(BSTR sobj_type)																	{	return E_NOTIMPL;	}
	STDMETHOD(UnregisterObjectType)(BSTR sobj_type)																	{	return E_NOTIMPL;	}
	STDMETHOD(get_GemExtendable)(IGemExtendable **val)																{	return E_NOTIMPL;	}
	STDMETHOD(put_GemExtendable)(IGemExtendable *val)																{	return E_NOTIMPL;	}
	STDMETHOD(DeleteRequest)(DeleteRequestRC *rc)																	{	return E_NOTIMPL;	}
	STDMETHOD(DeleteRelease)()																						{	return E_NOTIMPL;	}
	STDMETHOD(Init2)(BSTR sobj_spec, ICxAbsLayer *abs_layer)															{	return E_NOTIMPL;	}
	STDMETHOD(RegisterE39InfoCallback2)(ICxE39InfoCB2 *callback)													{	return E_NOTIMPL;	}
	STDMETHOD(UnregisterE39InfoCallback2)(ICxE39InfoCB2 *callback)													{	return E_NOTIMPL;	}
	STDMETHOD(Clear)()																								{	return E_NOTIMPL;	}
	STDMETHOD(get_E39LogLevel)(long *val)																			{	return E_NOTIMPL;	}
	STDMETHOD(put_E39LogLevel)(long val)																			{	return E_NOTIMPL;	}
	STDMETHOD(SetTopObject)(ICxE39 *top_obj)																		{	return E_NOTIMPL;	}
	STDMETHOD(ListAttrValues)(ICxValueDisp *list_value)																{	return E_NOTIMPL;	}
	STDMETHOD(ListAttrValues2)(SAFEARRAY *attr_ids, ICxValueDisp *errors, ICxValueDisp **list_value)				{	return E_NOTIMPL;	}
	STDMETHOD(GetChildData)(BSTR sobj_type, SAFEARRAY *obj_ids, SAFEARRAY *attr_ids, ICxValueDisp *errors, ICxValueDisp **data)	{	return E_NOTIMPL;	}

	/* ----------------------------------------------------------------------------------------- */
	/*                              ICxE40PJMCallback Methods                                    */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(PRCommandCallback)(BSTR proc_id, CommandCallbackEnum cmd, ICxValueDisp *parm_list,
								 ICxProcessJob *job_obj, ICxValueDisp *status);
	STDMETHOD(PRSetMtrlOrder)(MaterialOrderEnum cur_order,
							  MaterialOrderEnum new_order, VARIANT_BOOL *ack);
	STDMETHOD(PRSetRecipeVariable)(BSTR proc_id, ICxProcessJob *job_obj,
								   ICxValueDisp *recipe_variables, ICxValueDisp *status)	{	return E_NOTIMPL;	}
	STDMETHOD(PRStateChange)(BSTR job_id, ProcessJobStateEnum state);
	STDMETHOD(PRCommandCallback)(ExecuteCommandSourceEnum cmd_src,
								 BSTR proc_id, CommandCallbackEnum cmd, ICxValueDisp *parm_list,
								 ICxProcessJob *job_obj, ICxValueDisp *status);

	/* ----------------------------------------------------------------------------------------- */
	/*                               ICxE90SubstrateCB Methods                                   */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(E90CommandCallback)(BSTR subst_id, SubstrateCommandEnum command, ICxValueDisp *parm_list, ICxSubstrate *sub_obj, ICxValueDisp *status);
	STDMETHOD(SubstrateStateChange)(BSTR subst_id, SubstrateStateMachineEnum machine, long new_state, ICxSubstrate *sub_obj);

	/* ----------------------------------------------------------------------------------------- */
	/*                              ICxE94ControlJobCB Methods                                   */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(CJCreate)(BSTR job_id, ICxE94ControlJob *object);
	STDMETHOD(CJHOQ)(BSTR job_id, ICxE94ControlJob *object);
	STDMETHOD(CJDeselect)(BSTR job_id, ICxE94ControlJob *object);
	STDMETHOD(GetNextPrJobID)(ICxE94ControlJob *object, E94CJProcessOrderMgmt order, BSTR *proc_id);
	STDMETHOD(CJStateChange)(BSTR job_id, E94CJState new_state);

	/* ----------------------------------------------------------------------------------------- */
	/*                              ICxE94ControlJobCB2 Methods                                  */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(CJCommandCallback)(BSTR job_id, E94Commands command, ICxValueDisp *parm_list, ICxE94ControlJob *job_obj, ICxValueDisp *status);

	/* ----------------------------------------------------------------------------------------- */
	/*                              ICxE94ControlJobCB3 Methods                                  */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(CJCommandCallback)(E94ExecuteCommandSourceEnum cmd_src, BSTR job_id, E94Commands command, ICxValueDisp *parm_list, ICxE94ControlJob *job_obj, ICxValueDisp *status);

	/* ----------------------------------------------------------------------------------------- */
	/*                              ICxE94ControlJobCB4 Methods                                  */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(SetAttrChangeRequest)(ICxE94ControlJob *object, BSTR attr_name, ICxValueDisp *attr_value, long *err_code, BSTR *err_text)	{	return E_NOTIMPL;	}
	STDMETHOD(AttrChangeNotify)(ICxE94ControlJob *object, BSTR attr_name, ICxValueDisp *attr_value);

	/* ----------------------------------------------------------------------------------------- */
	/*                          ICxE94ControlJobProcessJobCB Methods                             */
	/* ----------------------------------------------------------------------------------------- */


#if (USE_SMS_MESSAGE_LIBE)
	/* ----------------------------------------------------------------------------------------- */
	/*                               ICxCommunications Methods                                   */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(EnableComm)()																			{	return E_NOTIMPL;	}
	STDMETHOD(DisableComm)()																		{	return E_NOTIMPL;	}
	STDMETHOD(SendMsg)(ICxEMInternalMsg *msg)														{	return E_NOTIMPL;	}
	STDMETHOD(GetCommState)(CxCommState *state, long *substate)										{	return E_NOTIMPL;	}
	STDMETHOD(GetMsgFromSendQueue)(ICxEMInternalMsg **msg)											{	return E_NOTIMPL;	}
	STDMETHOD(FreezeSendQueue)()																	{	return E_NOTIMPL;	}
	STDMETHOD(UnFreezeSendQueue)()																	{	return E_NOTIMPL;	}
	STDMETHOD(CancelMessage)(long msgID)															{	return E_NOTIMPL;	}
	STDMETHOD(GetIncomingMsg)(long msgID, ICxEMInternalMsg ** msg)									{	return E_NOTIMPL;	}
	STDMETHOD(GetInitString)(BSTR *str)																{	return E_NOTIMPL;	}
	STDMETHOD(InitFromString)(BSTR str)																{	return E_NOTIMPL;	}
	STDMETHOD(get_LogLevel)(long *LogLevel)															{	return E_NOTIMPL;	}
	STDMETHOD(put_LogLevel)(long pszLogLevel)														{	return E_NOTIMPL;	}
	STDMETHOD(RegisterCallback)(ICxCommunicationsCB *pCB)											{	return E_NOTIMPL;	}
	STDMETHOD(UnregisterCallback)(ICxCommunicationsCB *pCB)											{	return E_NOTIMPL;	}
	STDMETHOD(RegisterCallback2)(ICxCommunicationsCB2 *pCB)											{	return E_NOTIMPL;	}
	STDMETHOD(UnregisterCallback2)(ICxCommunicationsCB2 *pCB)										{	return E_NOTIMPL;	}
	STDMETHOD(get_ConnectionID)(long *ConnectionID)													{	return E_NOTIMPL;	}
	STDMETHOD(put_ConnectionID)(long pszConnectionID)												{	return E_NOTIMPL;	}
	STDMETHOD(get_EquipmentID)(long *EquipmentID)													{	return E_NOTIMPL;	}
	STDMETHOD(put_EquipmentID)(long pszEquipmentID)													{	return E_NOTIMPL;	}

	/* ----------------------------------------------------------------------------------------- */
	/*                               ICxCommunicationsCB Methods                                 */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(LinkStatus)(CxCommState status)														{	return E_NOTIMPL;	}
	STDMETHOD(MessageStatus)(long msgID, long msgType, CxMsgStatus status, long error)				{	return E_NOTIMPL;	}
	STDMETHOD(ProtocolStatus)(long status, BSTR description)										{	return E_NOTIMPL;	}
	STDMETHOD(Log)(long level, BSTR logString)														{	return E_NOTIMPL;	}

	/* ----------------------------------------------------------------------------------------- */
	/*                              ICxCommunicationsCB2 Methods                                 */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(LinkStatus)(long eqID, long connID, CxCommState status)								{	return E_NOTIMPL;	}
	STDMETHOD(MessageStatus)(long eqID, long connID, long msgID, long msgType, CxMsgStatus status, long error)	{	return E_NOTIMPL;	}
	STDMETHOD(ProtocolStatus)(long eqID, long connID, long status, BSTR description)				{	return E_NOTIMPL;	}
	STDMETHOD(Log)(long eqID, long connID, long level, BSTR logString)								{	return E_NOTIMPL;	}

	/* ----------------------------------------------------------------------------------------- */
	/*                                  ICxSmsObject Methods                                     */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(get_AutoS9F1)(VARIANT_BOOL *AutoS9F1)													{	return E_NOTIMPL;	}
	STDMETHOD(put_AutoS9F1)(VARIANT_BOOL pszAutoS9F1)												{	return E_NOTIMPL;	}
	STDMETHOD(get_AutoS9F9)(VARIANT_BOOL *AutoS9F9)													{	return E_NOTIMPL;	}
	STDMETHOD(put_AutoS9F9)(VARIANT_BOOL pszAutoS9F9)												{	return E_NOTIMPL;	}
	STDMETHOD(get_CurrState)(long *CurrState)														{	return E_NOTIMPL;	}
	STDMETHOD(get_TotalMsgSent)(long *TotalMsgSent)													{	return E_NOTIMPL;	}
	STDMETHOD(get_TotalMsgRcvd)(long *TotalMsgRcvd)													{	return E_NOTIMPL;	}
	STDMETHOD(get_TotalCommFailure)(long *TotalCommFailure)											{	return E_NOTIMPL;	}
	STDMETHOD(get_TotalTxTimeout)(long *TotalTxTimeout)												{	return E_NOTIMPL;	}
	STDMETHOD(get_MaximumIncomingSecs2MessageSize)(long *MaximumIncomingSecs2MessageSize)			{	return E_NOTIMPL;	}
	STDMETHOD(put_MaximumIncomingSecs2MessageSize)(long pszMaximumIncomingSecs2MessageSize)			{	return E_NOTIMPL;	}

	/* ----------------------------------------------------------------------------------------- */
	/*                                ICxEMInternalMsg Methods                                   */
	/* ----------------------------------------------------------------------------------------- */

	STDMETHOD(get_ReplyExpected)(VARIANT_BOOL *ReplyExpected)										{	return E_NOTIMPL;	}
	STDMETHOD(put_ReplyExpected)(VARIANT_BOOL pszReplyExpected)										{	return E_NOTIMPL;	}
	STDMETHOD(get_MsgBody)(ICxValueDisp * *MsgBody)													{	return E_NOTIMPL;	}
	STDMETHOD(put_MsgBody)(ICxValueDisp * pszMsgBody)												{	return E_NOTIMPL;	}
	STDMETHOD(get_MsgID)(long *MsgID)																{	return E_NOTIMPL;	}
	STDMETHOD(put_MsgID)(long pszMsgID)																{	return E_NOTIMPL;	}
	STDMETHOD(get_MsgType)(long *MsgType)															{	return E_NOTIMPL;	}
	STDMETHOD(put_MsgType)(long pszMsgType)															{	return E_NOTIMPL;	}
	STDMETHOD(get_Timestamp)(BSTR *Timestamp)														{	return E_NOTIMPL;	}
	STDMETHOD(put_Timestamp)(BSTR pszTimestamp)														{	return E_NOTIMPL;	}
	STDMETHOD(get_Name)(BSTR *Name)																	{	return E_NOTIMPL;	}
	STDMETHOD(put_Name)(BSTR pszName)																{	return E_NOTIMPL;	}
	STDMETHOD(get_Description)(BSTR *Description)													{	return E_NOTIMPL;	}
	STDMETHOD(put_Description)(BSTR pszDescription)													{	return E_NOTIMPL;	}
	STDMETHOD(get_MsgControlType)(long *MsgControlType)												{	return E_NOTIMPL;	}
	STDMETHOD(put_MsgControlType)(long pszMsgControlType)											{	return E_NOTIMPL;	}
	STDMETHOD(PutEmptyMsgBody)()																	{	return E_NOTIMPL;	}
	STDMETHOD(GetHeader)(long maxSize, long * actualSize, unsigned char * data)						{	return E_NOTIMPL;	}
	STDMETHOD(PutHeader)(long dataSize, unsigned char * data)										{	return E_NOTIMPL;	}
	STDMETHOD(get_HeaderSize)(long *HeaderSize)														{	return E_NOTIMPL;	}
	STDMETHOD(put_HeaderSize)(long pszHeaderSize)													{	return E_NOTIMPL;	}
	STDMETHOD(get_ByteStreamSize)(long *ByteStreamSize)												{	return E_NOTIMPL;	}
	STDMETHOD(put_ByteStreamSize)(long pszByteStreamSize)											{	return E_NOTIMPL;	}
	STDMETHOD(CopyToByteStream)(long bufferSize, unsigned char * buffer)							{	return E_NOTIMPL;	}
	STDMETHOD(RestoreFromByteStream)(long bufferSize, unsigned char * buffer)						{	return E_NOTIMPL;	}
	STDMETHOD(get_MsgHeader)(VARIANT *MsgHeader)													{	return E_NOTIMPL;	}
	STDMETHOD(put_MsgHeader)(VARIANT pszMsgHeader)													{	return E_NOTIMPL;	}
	STDMETHOD(VbCopyToByteStream)(VARIANT * buffer)													{	return E_NOTIMPL;	}
	STDMETHOD(VbRestoreFromByteStream)(VARIANT buffer)												{	return E_NOTIMPL;	}
	STDMETHOD(get_OptIU)(LPUNKNOWN *OptIU)															{	return E_NOTIMPL;	}
	STDMETHOD(put_OptIU)(LPUNKNOWN pszOptIU)														{	return E_NOTIMPL;	}
	STDMETHOD(PutEmptyOptIU)()																		{	return E_NOTIMPL;	}
	STDMETHOD(get_OptExtra)(long *OptExtra)															{	return E_NOTIMPL;	}
	STDMETHOD(put_OptExtra)(long pszOptExtra)														{	return E_NOTIMPL;	}
	STDMETHOD(get_Transmit)(VARIANT_BOOL *Transmit)													{	return E_NOTIMPL;	}
	STDMETHOD(put_Transmit)(VARIANT_BOOL pszTransmit)												{	return E_NOTIMPL;	}
	STDMETHOD(CopyToByteArray)(SAFEARRAY * *CopyToByteArray)										{	return E_NOTIMPL;	}
	STDMETHOD(RestoreFromByteArray)(SAFEARRAY * buffer)												{	return E_NOTIMPL;	}
	STDMETHOD(get_Stream)(int *Stream)																{	return E_NOTIMPL;	}
	STDMETHOD(put_Stream)(int pszStream)															{	return E_NOTIMPL;	}
	STDMETHOD(get_Function)(int *Function)															{	return E_NOTIMPL;	}
	STDMETHOD(put_Function)(int pszFunction)														{	return E_NOTIMPL;	}
	STDMETHOD(get_TransactionId)(long *TransactionId)												{	return E_NOTIMPL;	}
	STDMETHOD(put_TransactionId)(long pszTransactionId)												{	return E_NOTIMPL;	}
#endif
};

_COM_SMARTPTR_TYPEDEF(IEMSvcCbk, __uuidof(IEMSvcCbk));
OBJECT_ENTRY_AUTO(__uuidof(EMSvcCbk), CEMSvcCbk)
