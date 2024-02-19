/* EMService Callback 구현 */

#include "pch.h"
#include "EMSvcCbk.h"

#include "../../inc/cim300/E40CmdParam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 desc : 생성자
 parm : None
 retn : None
*/
CEMSvcCbk::CEMSvcCbk()
{
	m_pLogData		= NULL;
	m_pSharedData	= NULL;
}

/*
 desc : 객체 초기화 위해 이 메소드 재정의
 parm : None
 retn : None
*/
HRESULT CEMSvcCbk::FinalConstruct()
{
	return S_OK;
}

/*
 desc : 객체의 클린업을 수행하기 위해 파생 클래스에서 이 메소드 재정의
 parm : None
 retn : None
*/
VOID CEMSvcCbk::FinalRelease()
{
}

/*
 desc : 로그 메시지 등록
 parm : mesg	- [in]  로그 데이터가 저장된 버퍼 포인터
		level	- [in]  로그 에러 (알람) 레벨 (0x00 - Normal, 0x01 - Warning, 0x02 - Alarm (Error), 0x03 - Unknown
 retn : None
*/
VOID CEMSvcCbk::PutLogMesg(TCHAR *mesg, ENG_GALC level)
{
	/* 호출자에게 콜백함수를 통해 로그 메시지 전달 */
	if (m_pLogData)	m_pLogData->PushLogs(mesg, level);
}

/*
 desc : COM 호출 결과 분석
 parm : hr	- [in]  HRESULT 값
		mesg- [in]  Error Message 문자열 버퍼
 retn : HRESULT
*/
HRESULT CEMSvcCbk::CheckResult(HRESULT hr, PTCHAR mesg)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	if (FAILED(hr))
	{
		/* Error Message 추가 */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"COM ERROR %s HRESULT 0x%x", mesg, hr);
		if (m_pLogData)	m_pLogData->PushLogs(tzMesg, ENG_GALC::en_alarm);
	}

	return hr;
}

/*
 desc : ICxValueDispPtr List 내부에 저장된 에러 코드 정보 반환
 parm : value_disp	- [in]  ICxValueDispPtr
		value_type	- [in]  Value Type
 retn : TRUE or FALSE
 note : 에러 코드 값 의미
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
BOOL CEMSvcCbk::DispErrorData(ICxValueDispPtr value_disp, E30_SVET value_type)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	UINT8 u8IsError	= 0x00;
	LONG lItemCount	= 0, lHandle[3]	= {NULL}, i = 1, lError = 0;
	VARIANT_BOOL vbError	= VARIANT_TRUE;
	HRESULT hResult	= S_OK;
	CComBSTR bstrError;

	if (!value_disp)	return FALSE;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DispErrorData::ItemCount");
	hResult	= value_disp->ItemCount(0, &lItemCount);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* 등록된 알람 정보가 있는지 여부 */
	if (lItemCount < 1)	return TRUE;

	/* 최상위의 부모 핸들 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DispErrorData::ListAt");
//	m_pLogData->PushLogs(tzMesg);
	hResult	= value_disp->ListAt(0, 0, &lHandle[0]);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 부모 핸들에 연결된 리스트의 첫 번째 항목 (에러 코드 값) 가져오기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DispErrorData::GetValueU1 (handle=%d / index=0)",
			   lHandle[0]);
//	m_pLogData->PushLogs(tzMesg);
	switch (value_type)
	{
	case E30_SVET::en_svcack	:	hResult	= value_disp->GetValueBinary(lHandle[0], 1, &u8IsError);	break;
	case E30_SVET::en_objack	:
	case E30_SVET::en_caack	:	hResult	= value_disp->GetValueU1(lHandle[0], 1, &u8IsError);		break;
	case E30_SVET::en_acka	:	hResult	= value_disp->GetValueBoolean(lHandle[0], 1, &vbError);		break;
	default				:	m_pLogData->PushLogs(L"DispErrorData::Unknown Value Type");	return FALSE;	break;
	}
	if (m_pLogData->CheckResult(hResult, tzMesg))		return FALSE;
	/* 에러가 없으면 바로 리턴 */
	if (u8IsError < 0x01 || vbError == VARIANT_TRUE)	return TRUE;

	/* 부모 핸들에 연결된 두 번째 리스트의 핸들 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DispErrorData::ListAt (handle=0 / index=1)");
//	m_pLogData->PushLogs(tzMesg);
	hResult	= value_disp->ListAt(lHandle[0], 2, &lHandle[1]);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 두 번째 리스트의 핸들에 등록된 List 개수 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DispErrorData::ItemCount (handle=%d)", lHandle[0]);
//	m_pLogData->PushLogs(tzMesg);
	hResult	= value_disp->ItemCount(lHandle[1], &lItemCount);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* 에러 메시지가 포함되어 있지 않으면 리턴 */
	if (lItemCount < 1)
	{
		m_pLogData->PushLogs(L"Does not contain error messages");
		return TRUE;
	}

	/* 두 번째 리스트의 하위 항목의 List 정보 값 얻기 */
	for (; i<=lItemCount; i++)
	{
		/* 두 번째 리스트의 하위 항목의 핸들 얻기 */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DispErrorData::ListAt (handle=%d)",
				   lHandle[1]);
//		m_pLogData->PushLogs(tzMesg);
		hResult	= value_disp->ListAt(lHandle[1], i, &lHandle[2]);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		/* 두 번째 리스트의 항위 항목 중 첫 번째 에러 코드 얻기 */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DispErrorData::GetValueU2 (handle=%d / index=%d / 1st)",
				   lHandle[2], i);
//		m_pLogData->PushLogs(tzMesg);
		value_disp->GetValueU2(lHandle[2], 1, &lError);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		/* 두 번째 리스트의 항위 항목 중 두 번째 에러 문자열 얻기 */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DispErrorData::GetValueAscii (handle=%d / index=%d / 2nd)",
				   lHandle[2], i);
//		m_pLogData->PushLogs(tzMesg);
		value_disp->GetValueAscii(lHandle[2], 2, &bstrError);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		/* 에러 정보 출력 */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DispErrorData:Error Message (Code:%d, Mesg:%s)",
				   lError, LPCTSTR(_bstr_t(bstrError)));
		m_pLogData->PushLogs(tzMesg, ENG_GALC::en_warning);
	}

	return (hResult == S_OK);
}

/*
 desc : E40ParamCmd 데이터 분석
 parm : parm_list	- [in]  데이터가 저장된 객체
		parm_obj	- [out] 분석된 데이터가 저장될 객체
 retn : TRUE or FALSE
*/
BOOL CEMSvcCbk::E40ParamCmdAnalysis(ICxValueDispPtr parm_list, CE40CmdParam *parm_obj)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	UINT8 u8Value	= 0;
	INT16 i16Value	= 0;
	UINT16 u16Value	= 0;
	LONG lItemCount	= 0, lHandle[2]	= {NULL}, i = 1, lValue = 0;
	INT64 i64Value	= 0;
	UINT64 u64Value	= 0;
	FLOAT fValue	= 0.0f;
	DOUBLE dbValue	= 0.0f;
	VARIANT_BOOL vbError	= VARIANT_TRUE, vbValue = VARIANT_TRUE;
	HRESULT hResult	= S_OK;
	ValueType valType;
	CComBSTR bstrName, bstrValue;

	if (!parm_list)	return FALSE;
	hResult	= parm_list->ItemCount(0, &lItemCount);
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DispErrorData::ItemCount");
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* ItemCount 값은 무조건 1개 이어야만 함 */
	if (lItemCount < 1)	return FALSE;

	/* 최상위의 부모 핸들 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"E40ParamCmdAnalysis::ListAt");
//	m_pLogData->PushLogs(tzMesg);
	hResult	= parm_list->ListAt(0, 0, &lHandle[0]);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 바로 하위 항목의 개수 구하기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"DispErrorData::ItemCount : lHandle[0] = %d", lHandle[0]);
//	m_pLogData->PushLogs(tzMesg);
	hResult	= parm_list->ItemCount(lHandle[0], &lItemCount);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 하위 항목 개수 만큼 반복 */
	for (; i<=lItemCount; i++)
	{
		/* 자식 핸들 얻기 */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"E40ParamCmdAnalysis::ListAt : handle[0] = %d, index=%d",
				   lHandle[0], i);
//		m_pLogData->PushLogs(tzMesg);
		hResult	= parm_list->ListAt(lHandle[0], i, &lHandle[1]);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		/* CP Name 얻기 */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"E40ParamCmdAnalysis::GetValueAscii : handle[1]=%d, index=1", lHandle[1]);
//		m_pLogData->PushLogs(tzMesg);
		hResult	= parm_list->GetValueAscii(lHandle[1], 1, &bstrName);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		/* CP Value Type 얻기 */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"E40ParamCmdAnalysis::GetDataType : handle[1]=%d, index=1", lHandle[1]);
//		m_pLogData->PushLogs(tzMesg);
		hResult	= parm_list->GetDataType(lHandle[1], 1, &valType);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
		/* CP Value 얻기 */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"E40ParamCmdAnalysis::GetValueAscii : handle[1]=%d, index=1", lHandle[1]);
//		m_pLogData->PushLogs(tzMesg);
		switch (valType)
		{
		case ValueType::I1	:
		case ValueType::I2	:
		case ValueType::I4	:
		case ValueType::I8	:
			switch (valType)
			{
			case ValueType::I1	:	hResult	= parm_list->GetValueI1(lHandle[1], 1, &i16Value);	i64Value = i16Value;	break;
			case ValueType::I2	:	hResult	= parm_list->GetValueI2(lHandle[1], 1, &i16Value);	i64Value = i16Value;	break;
			case ValueType::I4	:	hResult	= parm_list->GetValueI4(lHandle[1], 1, &lValue);	i64Value = lValue;		break;
			case ValueType::I8	:	hResult	= parm_list->GetValueI8(lHandle[1], 1, &lValue);	i64Value = lValue;		break;
			}
			if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
			parm_obj->AppendData(LPTSTR(bstrName), i64Value);
			break;

		case ValueType::U1	:
		case ValueType::U2	:
		case ValueType::U4	:
		case ValueType::U8	:
		case ValueType::Bo	:
		case ValueType::Bi	:
			switch (valType)
			{
			case ValueType::U1	:	hResult	= parm_list->GetValueU1(lHandle[1], 1, &u8Value);		u64Value = u8Value;	break;
			case ValueType::U2	:	hResult	= parm_list->GetValueU2(lHandle[1], 1, &lValue);		u64Value = lValue;	break;
			case ValueType::U4	:	hResult	= parm_list->GetValueU4(lHandle[1], 1, &lValue);		u64Value = lValue;	break;
			case ValueType::U8	:	hResult	= parm_list->GetValueU8(lHandle[1], 1, &lValue);		u64Value = lValue;	break;
			case ValueType::Bo	:	hResult	= parm_list->GetValueBinary(lHandle[1], 1, &u8Value);	u64Value = u8Value;	break;
			case ValueType::Bi	:	hResult	= parm_list->GetValueBoolean(lHandle[1], 1, &vbValue);	u64Value = vbValue == VARIANT_TRUE ? 1 : 0;	break;
			}
			if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
			parm_obj->AppendData(LPTSTR(bstrName), u64Value);
			break;

		case ValueType::F4	:
		case ValueType::F8	:
			switch (valType)
			{
			case ValueType::F4	:	hResult	= parm_list->GetValueF4(lHandle[1], 1, &fValue);		dbValue = fValue;	break;
			case ValueType::F8	:	hResult	= parm_list->GetValueF8(lHandle[1], 1, &dbValue);	break;
			}
			if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
			hResult	= parm_obj->AppendData(LPTSTR(bstrName), dbValue);
			break;

		case ValueType::A	:
			hResult	= parm_list->GetValueAscii(lHandle[1], 1, &bstrValue);
			if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
			parm_obj->AppendData(LPTSTR(bstrName), LPTSTR(bstrValue));
			break;

		/* 기타 이외의 데이터 (변수) 타입은 무시? */
		default				:	return FALSE;
		}
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	}

	/* 부모 핸들에 연결된 리스트의 첫 번째 항목 (에러 코드 값) 가져오기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"E40ParamCmdAnalysis::GetValueU1 (handle=%d / index=0)",
			   lHandle[0]);
	m_pLogData->PushLogs(tzMesg);

	return TRUE;
}

/*
 desc : 수신된 로그 메시지 분석
 parm : mesg_buff	- [in]  수신된 로그 메시지가 저장된 버퍼
 retn : None
*/
VOID CEMSvcCbk::ParseLogMessage(VARIANT mesg_buff)
{
#if 0
	HRESULT hResult		= S_OK;
	ULONG ulCount		= 0;
	CComBSTR bstrAscii;
	ICxValuePtr pCxValue (CLSID_CxValueObject);
	PUINT8 pByteMesg	= NULL;
	SAFEARRAY *pSaBuff	= mesg_buff.parray;
	ValueType valType;

	/* Array Buffer 연결 */
	hResult	= SafeArrayAccessData(pSaBuff, (VOID**)&pByteMesg);
	/* 저장된 값 임시 버퍼에 가져다 놓기 */
	if (hResult == S_OK)	hResult	= pCxValue->RestoreFromByteStream(0, 0,
																	  pSaBuff->rgsabound->cElements,
																	  pByteMesg);
	if (hResult == S_OK)	hResult	= pCxValue->ItemCount(0, &ulCount);
	if (hResult == S_OK)	hResult	= pCxValue->GetDataType(0, 0, &valType);
	if (hResult == S_OK)
	{
		hResult	= pCxValue->GetValueAscii(0, 0, &bstrAscii);
		TRACE(L"%s\n", (LPCTSTR)_bstr_t(bstrAscii));
	}
#endif
}

/*
 desc : ValueDisp에 저장된 List Data 가져오는 재귀 함수
 parm : mesg	- [in]  The message value object
		handle	- [in]  List Handle
		index	- [in]  List Index
		level	- [in]  Sub-Depth (Tab 깊이. XML 문법 연상하면 됨 <XML> ... </XML>)
		items	- [in]  List Data에 해당된 Handle과 Count 정보가 저장될 리스트 참조
 retn : TRUE or FALSE
*/
BOOL CEMSvcCbk::ListAtItem(ICxValueDisp *mesg, LONG handle, LONG index, LONG level,
						   CAtlList <STG_CLHC> &item)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG l, lHandle	= -1L, lCount;
	HRESULT hResult	= S_OK;
	STG_CLHC stItem	= {NULL};

	/* List Handle 얻기 */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"CEMSvcCbk::ListAt (handle=%d)", handle);
	m_pLogData->PushLogs(tzMesg);
	hResult = mesg->ListAt(handle, index, &lHandle);
#if 0
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
#else
	if (lHandle < 0)	return FALSE;
#endif
	/* List Count 얻기 */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"CEMSvcCbk::ItemCount (lHandle=%d)", lHandle);
	m_pLogData->PushLogs(tzMesg);
	hResult = mesg->ItemCount(lHandle, &lCount);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 만약 개수가 없다면 */
	if (lCount < 1)	return FALSE;

	/* 검색된 개수만큼, 다시 재귀함수 호출 (어렵다) */
	for (l=1; l<=lCount; l++)
	{
		/* 더 하위에 등록된 그룹 개수가 있으면, 리턴 해버림 */
		if (!ListAtItem(mesg, lHandle, l, level+1, item))
		{
			/* 더 이상 하위에 등록된 정보가 없다면, List에 현재 정보 등록 */
			stItem.handle	= lHandle;
			stItem.index	= l;
			stItem.level	= level;
			item.AddTail(stItem);
		}
	}

	return TRUE;
}

/*
 desc : CxValueObject에 저장된 Value 값을 무조건 String Buffer에 저장 후 반환
 parm : cx_val	- [in]  CxValueObject Object
		handle	- [in]  List Handle
		index	- [in]  List Index (1-based)
		value	- [out] 값이 반환되어 저장될 구조체 참조
 retn : TRUE or FALSE
*/
BOOL CEMSvcCbk::GetCxValObjToString(ICxValueDispPtr cx_val, LONG handle, LONG index, LPG_TSMV value)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	UINT8 u8Val;
	INT16 i16Val;
	LONG l32Val;
	FLOAT fVal;
	DOUBLE dbVal;
	HRESULT hResult	= S_OK;
	ValueType valType;
	CComBSTR bstrValue;
	CUniToChar csCnv;

	/* 값 복사해 옴 */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE,
			   L"GetCxValObjToString::GetDataType (handle=%d, index=%0d)", handle, index);
	m_pLogData->PushLogs(tzMesg);
	hResult	= cx_val->GetDataType(handle, index, &valType);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Value Type에 따라 모두 문자열로 변환 후 저장 */
	switch (E30_GPVT(valType))
	{
	case E30_GPVT::en_i1	:
	case E30_GPVT::en_i2	:
		switch (E30_GPVT(valType))
		{
		case E30_GPVT::en_i1	:	hResult	= cx_val->GetValueI1(handle, index, &i16Val);		break;
		case E30_GPVT::en_i2	:	hResult	= cx_val->GetValueI2(handle, index, &i16Val);		break;
		}
		if (hResult == S_OK)	swprintf_s(value->value, CIM_SUBSTRATE_ID, L"%d", i16Val);			break;
	case E30_GPVT::en_i4	:
	case E30_GPVT::en_i8	:
	case E30_GPVT::en_u2	:
	case E30_GPVT::en_u4	:
	case E30_GPVT::en_u8	:
		switch (E30_GPVT(valType))
		{
		case E30_GPVT::en_i4	:	hResult	= cx_val->GetValueI4(handle, index, &l32Val);		break;
		case E30_GPVT::en_i8	:	hResult	= cx_val->GetValueI8(handle, index, &l32Val);		break;
		case E30_GPVT::en_u2	:	hResult	= cx_val->GetValueU2(handle, index, &l32Val);		break;
		case E30_GPVT::en_u4	:	hResult	= cx_val->GetValueU4(handle, index, &l32Val);		break;
		case E30_GPVT::en_u8	:	hResult	= cx_val->GetValueU4(handle, index, &l32Val);		break;
		}
		if (hResult == S_OK)	swprintf_s(value->value, CIM_SUBSTRATE_ID, L"%d", l32Val);			break;
	case E30_GPVT::en_u1	:	hResult	= cx_val->GetValueU1(handle, index, &u8Val);
								if (hResult == S_OK)	swprintf_s(value->value, CIM_SUBSTRATE_ID, L"%u", u8Val);		break;
	case E30_GPVT::en_f4	:	hResult	= cx_val->GetValueF4(handle, index, &fVal);
								if (hResult == S_OK)	swprintf_s(value->value, CIM_SUBSTRATE_ID, L"%.4f", fVal);		break;
	case E30_GPVT::en_f8	:	hResult	= cx_val->GetValueF8(handle, index, &dbVal);
								if (hResult == S_OK)	swprintf_s(value->value, CIM_SUBSTRATE_ID, L"%.8f", dbVal);		break;

	case E30_GPVT::en_w		:
	case E30_GPVT::en_a		:	hResult = cx_val->GetValueAscii(handle, index, &bstrValue);
								if (hResult == S_OK)	swprintf_s(value->value, CIM_SUBSTRATE_ID, L"%s", (LPCTSTR)_bstr_t(bstrValue));	break;

	case E30_GPVT::en_b		:	hResult	= cx_val->GetValueBoolean(handle, index, (VARIANT_BOOL*)&i16Val);
								swprintf_s(value->value, CIM_SUBSTRATE_ID, L"%d", i16Val);		break;
	case E30_GPVT::en_bi	:	hResult	= cx_val->GetValueBinary(handle, index, (PUINT8)&i16Val);
								swprintf_s(value->value, CIM_SUBSTRATE_ID, L"%d", i16Val);		break;
	case E30_GPVT::en_any	:	
	case E30_GPVT::en_list	:
	case E30_GPVT::en_jis8	:
	case E30_GPVT::en_array	:	return FALSE;
	}

	/* Data Type 저장 */
	value->type	= (UINT16)valType;

	return (hResult == S_OK);
}

/* --------------------------------------------------------------------------------------------- */
/*                                          User Methods                                         */
/* --------------------------------------------------------------------------------------------- */
/*
 desc : 전역으로 사용될 로그 객체 연결
 parm : data	- [in]  로그 객체 포인터
 retn : 0L (S_OK)
*/
STDMETHODIMP CEMSvcCbk::SetLogData(LONG_PTR data)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pLogData	= (CLogData *)data;
	return S_OK;
}

/*
 desc : 전역으로 사용될 공유 데이터 객체 연결
 parm : data	- [in]  데이터 객체 포인터
 retn : 0L (S_OK)
*/
STDMETHODIMP CEMSvcCbk::SetSharedData(LONG_PTR data)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData	= (CSharedData *)data;
	return S_OK;
}


/* --------------------------------------------------------------------------------------------- */
/*                                     ICxEMAppCallback Methods                                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Handler for value change callback
 parm : alarm_id	- [in]  The alarm identification number
		alarm_code	- [in]  Alarm code byte represents the alarm state and category.
							If bit 8 is 1, then the alarm is set. If bit 8 is 0, then the alarm is cleared.
							Bits 7-1 contain the alarm category. Ignore the high-order word.
							For further information, please see ALCD in E5.
 retn : S_OK or S_FALSE
*/
STDMETHODIMP CEMSvcCbk::AlarmChanged(long alarm_id, long alarm_code)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	/* 이벤트 등록 */
	m_pSharedData->E30_AlarmChanged(alarm_id, alarm_code);

	return S_OK;
}

/*
 desc : Handler for custom messaging asynchronous errors
 parm : conn_id		- [in]  The connection identification number.
		mesg_id		- [in]  The message identification number
		trans_id	- [in]  The terminal identification number
		error_num	- [in]  The error number
							For a list of values see the Developer Guide, Error Codes section
		extra		- [in]  Extra information (not yet used)
 retn : S_OK or S_FALSE
*/
STDMETHODIMP CEMSvcCbk::AsyncMsgError(long conn_id, long mesg_id, long trans_id, long error_num,
									  long extra)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return S_OK;
}

/*
/*
 desc : Handler for remote commands
 parm : conn_id		- [in]  The connection identification number
		command_name- [in]  The command to issue
		param_names	- [in]  List of ASCII parameter names L {A name1} {A name2} ...
		param_values- [in]  List of parameter values corresponding to the parameter names L {value1} {value2} ...
		cmd_result	- [out] The command results
 retn : S_OK or S_FALSE
*/
STDMETHODIMP CEMSvcCbk::CommandCalled(long conn_id, BSTR command_name, ICxValueDisp *param_names,
									  ICxValueDisp *param_values, CommandResults *cmd_result)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL};

	if (NULL == cmd_result)	return E_POINTER;

	LONG lArguCount;
	HRESULT hResult;
	CComBSTR bPPID, bRecipeName;
	TCHAR tzCmdName[CIM_CMD_NAME_SIZE]	= {NULL};
	TCHAR tzPPID[CIM_CMD_NAME_SIZE]		= {NULL};

	/* Converting BSTR to TCHAR (LPCTSTR) */
	swprintf_s(tzCmdName, CIM_CMD_NAME_SIZE, L"%s", (LPCTSTR)_bstr_t(command_name));
	/* In this, only connection PRIMARY_HOST (Only One) is allowed to send Remote Commands. */
	if (conn_id != 1)
	{
		m_pLogData->PushLogs(L"Command rejected because it came from a non-primary host connection",
							 tzCmdName, ENG_GALC::en_warning);
		*cmd_result	= cmdRejected;
		return S_FALSE;
	}

	/* The Control State must be remote (E30_CSMC) */
	if (E30_CSMC::en_online_remote != (E30_CSMC)m_pSharedData->E30_GetState(E30_SSMS::en_control, conn_id))
	{
		m_pLogData->PushLogs(L"Command rejected because the control state is not on-line remote",
							 tzCmdName);
		*cmd_result	= cmdCannotPerform;
		return S_FALSE;
	}

	/* Check if the command is valid for the current process state */
	switch (m_pSharedData->GetProcessState())
	{
	case E30_GPSV::en_idle		:
		if (0 != wcscmp(L"START", tzCmdName) && 0 != wcscmp(L"PP-SELECT", tzCmdName))
		{
			m_pLogData->PushLogs(L"Remote command rejected because process state is IDLE. "
								 L"Only START and PP-SELECT allowed", tzCmdName, ENG_GALC::en_warning);
			*cmd_result = cmdCannotPerform; 
			return S_FALSE;
		}	break;
	case E30_GPSV::en_execute	:
		if (0 != wcscmp(L"STOP", tzCmdName))
		{
			m_pLogData->PushLogs(L"Remote command rejected because process state is EXECUTING, "
								 L"Only STOP is allowed", tzCmdName, ENG_GALC::en_warning);
			*cmd_result = cmdCannotPerform; 
			return S_FALSE;
		}	break;
	case E30_GPSV::en_setup		:
		m_pLogData->PushLogs(L"Remote command rejected because process state is SETUP, "
							 L"No commands are allowed", tzCmdName, ENG_GALC::en_warning);
		*cmd_result = cmdCannotPerform; 
		return S_FALSE;
	case E30_GPSV::en_complete	:
		m_pLogData->PushLogs(L"Remote command rejected because process state is COMPLETE, "
							 L"No commands are allowed", tzCmdName, ENG_GALC::en_warning);
		*cmd_result = cmdCannotPerform; 
		return S_FALSE;
	}

	/* Prepare to check the arguments / parameters. */
	if (!param_names)	lArguCount	= 0;
	else
	{
		swprintf_s(tzMesg, LOG_MESG_SIZE, L"Remote command rejected because it failed to get the "
										  L"number of arguments (cmd_name=%s)", tzCmdName);
		hResult = param_names->ItemCount(0, &lArguCount);
		if (m_pLogData->CheckResult(hResult, tzMesg))
		{
			*cmd_result	= cmdParamInvalid;
			return S_FALSE;
		}
	}
	/* Handle the remote command. */
	if (0 == wcscmp(L"START", tzCmdName))
	{
		if (0 != lArguCount)
		{
			m_pLogData->PushLogs(L"Remote command rejected because it had arguments", tzCmdName);
			*cmd_result	= cmdParamInvalid;
			return S_FALSE;
		}
		*cmd_result = cmdPerformed;
		m_pSharedData->CommandCalled(conn_id, E30_GRCI::en_remote_cmd_start, tzCmdName);
	}
	else if (0 == wcscmp(L"STOP", tzCmdName))
	{
		if (0 != lArguCount)
		{
			m_pLogData->PushLogs(L"Remote command rejected because it had arguments", tzCmdName);
			*cmd_result = cmdParamInvalid;
			return S_OK;
		}
		*cmd_result = cmdPerformed;
		m_pSharedData->CommandCalled(conn_id, E30_GRCI::en_remote_cmd_stop, tzCmdName);
	}
	else if (L"PP-SELECT" == tzCmdName)
	{
		if (1 != lArguCount)
		{
			m_pLogData->PushLogs(L"Remote command rejected because it did not have 1 argument", tzCmdName);
			*cmd_result = cmdParamInvalid;
			return S_FALSE;
		}

		/* validate the argument name. */
		hResult	= param_names->GetValueAscii(0, 1, &bPPID);
		swprintf_s(tzPPID, CIM_CMD_NAME_SIZE, L"%s", (LPCTSTR)_bstr_t(bPPID));
		if (L"PPID" != tzPPID)
		{
			m_pLogData->PushLogs(L"Remote command PP-SELECT rejected because the argumennt names was not PPID", tzPPID);
			*cmd_result = cmdParamInvalid;
			return S_FALSE;
		}

		/*validate the argument value */
		hResult = param_values->GetValueAscii(0, 1, &bRecipeName);
		if (FAILED(hResult))
		{
			m_pLogData->PushLogs(L"Remote command rejected because the argumennt type was not ASCII", tzCmdName); 
			*cmd_result = cmdParamInvalid;
			return S_FALSE;
		}
		*cmd_result = cmdPerformed;
		/* Command Event 등록 */
		m_pSharedData->CommandCalled(conn_id, E30_GRCI::en_remote_cmd_ppselect, tzCmdName);
#if 0
		CString *pRecipeName = new CString();
		*pRecipeName = bRecipeName;
		PostMessage(m_pCSharedItems->m_HWND, ID_PPSELECT, (WPARAM)pRecipeName, 0);
#endif
	}
	else
	{
		m_pLogData->PushLogs(L"Remote command rejected because it is not a supported command", tzCmdName);
		*cmd_result = cmdNotExist;
		return S_FALSE;
	}

	return S_OK;
}

/*
 desc : Called when an equipment event is triggered
 parm : conn_id	- [in]  The connection identification number
		event_id- [in]  The event identification number
 retn : S_OK or S_FALSE
*/
STDMETHODIMP CEMSvcCbk::EventTriggered(long conn_id, long event_id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#ifdef _DEBUG
	TRACE(L"[E30] CEMSvcCbk::EventTriggered - conn_id [%d] event_id [%d]\n", conn_id, event_id);
#endif

	return S_OK;
}

/*
 desc : Host에서 등록된 Event (Variable ID) 정보 중에서 값이 변경된 경우가 있다면, 호출되며
		이 때, 해당 variable id의 값을 장치로부터 읽어와서 갱신 (설정)해 주는 작업을 해야 됨
 parm : conn_id	- [in]  The connection identification number
		var_id	- [in]  Variable ID
		var_name- [in]  Varaible Name
		value	- [in]  Value Object
 retn : S_OK or S_FALSE
*/
STDMETHODIMP CEMSvcCbk::GetValue(long conn_id, long var_id, BSTR var_name, ICxValueDisp *value)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return S_OK;
}

/*
 desc : 장비 (Equipment)에서 PP (Process Program; 레시피) 적재 후 Ack 요청에 대한 응답이 수신된 경우
 parm : conn_id	- [in]  The connection identification number (0 if common)
		result	- [in]  Host로부터 수신된 응답 값
 retn : S_OK or S_FAIL
 note : CIMConnect calls this method when a host acknowledges a recipe load inquiry when the application calls PPLoadInquire
*/
STDMETHODIMP CEMSvcCbk::HostPPLoadInqAck(long conn_id, RecipeGrant result)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	/* Shared memory 영역에 값 갱신 */
	if (m_pSharedData)	m_pSharedData->SetPPLoadRecipeAck(E30_HPLA(result));
	return S_OK;
}

/*
 desc : 장비 (Equipment)에서 PP (Process Program; 레시피) 생성 후 Host에게 전달 (송신) 후 응답이 수신된 경우
 parm : conn_id	- [in]  The connection identification number (0 if common)
		result	- [in]  Host로부터 수신된 응답 값
 retn : S_OK or S_FAIL
 note : CIMConnect calls this method when a host acknowledges reception of a recipe sent using PPSend
*/
STDMETHODIMP CEMSvcCbk::HostPPSendAck(long conn_id, RecipeAck result)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	/* Shared memory 영역에 값 갱신 */
	if (m_pSharedData)	m_pSharedData->SetPPSendRecipeAck(E30_SRHA(result));
	return S_OK;
}

/*
 desc : 장비 (Equipment)에서 터미널 메시지가 송신 (전달)된 후,  Host로부터 수신 (응답)된 경우
 parm : conn_id	- [in]  The connection identification number (0 if common)
		result	- [in]  Host로부터 수신된 응답 값
 retn : S_OK or S_FAIL
 note : This method is called when a host acknowledges a terminal message sent by the application
*/
STDMETHODIMP CEMSvcCbk::HostTermMsgAck(long conn_id, TerminalMsgResults result)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	/* Shared memory 영역에 값 갱신 */
	if (m_pSharedData)	m_pSharedData->SetTermMsgAck(E30_STHA(result));
	return S_OK;
}

/*
 desc : 사용자가 등록한 메시지 ID에 대해, CIMConnect에서 호출해주는 콜백 함수
		내부적으로 등록된 메시지 ID에 대해 Host로부터 응답이 오면, CIMConnect가 응용 프로그램에게 알려줌
 parm : conn_id			- [in]  The connection identification number
		mesg_id			- [in]  The message identification number
		mesg			- [in]  The message value object
		reply_expected	- [in]  true if the message w-bit was set and the host expected a reply
		trans_id		- [in]  The terminal identification number
		reply_mesg_id	- [in]  The reply message identification number. Fill this out if result = mReply
		reply_msg		- [in]  the reply message body. Fill this out if result = mReply
		result			- [in]  The message results
 retn : S_OK or S_FALSE
*/
STDMETHODIMP CEMSvcCbk::MessageReceived(long conn_id, long mesg_id, ICxValueDisp *mesg,
										VARIANT_BOOL reply_expected, long trans_id,
										long *reply_mesg_id, ICxValueDisp *reply_msg,
										MessageResults *result)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG i, lReply	= LONG(*result);
	HRESULT hResult	= S_OK;
	VARIANT varList;
	STG_CLHC stItem	= {NULL};
	STG_TSMV stVals	= {NULL};
	STG_TSMD stMesg	= {NULL};
	CAtlList <STG_CLHC> lstItem;
	CComBSTR bstrValue;

#ifdef _DEBUG
	TRACE(L"[E30] CEMSvcCbk::MessageReceived - conn_id [%d] mesg_id [%4x] trans_id [%d]\n",
		  conn_id, mesg_id, trans_id);
#endif

	/* 값 복사해 옴 */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE,
			   L"CEMSvcCbk::CopyToByteStream (conn_id=%d, mesg_id=%04xx)", conn_id, mesg_id);
	m_pLogData->PushLogs(tzMesg);
	mesg->CopyToByteStream(0, 0, &varList);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 재귀 함수 호출을 통해, 현재 수신된 SECS-II Message의 모든 리스트 정보 얻기 */
	ListAtItem(mesg, 0, 0, 0, lstItem);
	if (lstItem.GetCount() > 0)
	{
		/* 임시 메모리 할당 */
		stMesg.conn_id	= (UINT8)conn_id;
		stMesg.mesg_id	= (UINT16)mesg_id;
		stMesg.trans_id	= trans_id;
		stMesg.count	= (LONG)lstItem.GetCount();
		stMesg.value	= (LPG_TSMV)::Alloc(sizeof(STG_TSMV) * UINT32(stMesg.count));
		ASSERT(stMesg.value);

		/* 등록된 검색 개수만큼 반복하여 값 읽어와 문자열 버퍼에 반환 */
		for (i=0; i<lstItem.GetCount(); i++)
		{
			stItem	= lstItem.GetAt(lstItem.FindIndex(i));
			if (GetCxValObjToString(mesg, stItem.handle, stItem.index, &stVals))
			{
				/* 얻어진 결과 값 임시 메모리에 등록 */
				stVals.handle	= stItem.handle;
				stVals.level	= UINT8(stItem.level);
				/* 임시 메모리에 복사 */
				memcpy(&stMesg.value[i], &stVals, sizeof(STG_TSMV));
			}
		}
		/* 임시 Item 메모리 해제 */
		lstItem.RemoveAll();

		/* 수신된 메시지 등록 */
		m_pSharedData->AddSecsMessage(stMesg);
	}

	/* 일단, 현재 발생된 메시지 등록 */
	m_pSharedData->MessageReceived(conn_id, mesg_id, lReply);

	/* 반환 값 (잘 받았다고 응답 알림) */
	if (reply_expected == VARIANT_TRUE)
	{
		if (*result == MessageResults::mReply)
		{
			*reply_mesg_id	= mesg_id + 1;
			*result			= mReply;
		}
	}

	return S_OK;
}


/*
 desc : Handler for state machine callbacks
 parm : conn_id	- [in]  The connection identification number
		machine	- [in]  The state machine that changed
		state	- [in]  The new state of the state machine
 retn : S_OK or S_FALSE
*/
STDMETHODIMP CEMSvcCbk::StateChange(long conn_id, StateMachine machine, long state)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#ifdef _DEBUG
	TRACE(L"[E30] CEMSvcCbk::StateChange - conn_id [%d] machine [%d] state [%d]\n",
		  conn_id, machine, state);
#endif
	/* 이벤트 등록 */
	m_pSharedData->E30_PushState(E30_SSMS(machine), conn_id, state);

	return S_OK;
}

/*
 desc : Handler for value change callback
 parm : conn_id		- [in]  The connection identification number
		var_id		- [in]  The variable identification number
		var_name	- [in]  The variable name
		value		- [in]  The variable value
 retn : S_OK or S_FALSE
*/
STDMETHODIMP CEMSvcCbk::ValueChanged(long conn_id, long var_id, BSTR var_name, ICxValueDisp *value)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#ifdef _DEBUG
	TRACE(L"[E30] CEMSvcCbk::ValueChanged - conn_id [%d] var_id [%d] var_name [%s]\n",
		  conn_id, var_id, var_name);
#endif
	/* 이벤트 등록 */
	m_pSharedData->E30_ValueChanged(conn_id, var_id, (LPTSTR)_bstr_t(var_name));

	return S_OK;
}

/*
 desc : Handler for terminal message callbacks
		This method is called when a connection sends a terminal message and 
		the application has registered as a terminal message handler.
		This works for both single and multiple line messages.
 parm : conn_id	- [in]  The connection identification number
		term_id	- [in]  The terminal identification number
		lines	- [in]  Either a List of ASCII 'L {A line1} {A line2}, ...,'
						if the host sent S10F5, or a single ASCII 'A text' if the host sent S10F3
		result	- [in]  The result code (TerminalMsgResults)
						tAccepted	: message accepted
						tRejected	: message rejected
						tUnavailable: specified terminal is unavailable
 retn : S_OK or S_FALSE
*/
STDMETHODIMP CEMSvcCbk::TerminalMsgRcvd(long conn_id, long term_id,
										ICxValueDisp *lines, TerminalMsgResults *result)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!result)	return E_POINTER;

	LONG i, lLines	= 0, lSizeOfMesgLen	= 0;
	HRESULT hResult;
	ValueType valType;
	_bstr_t bstrMesg2;
	CComBSTR bstrMesg1;
	CString strMesg;

	/* By default, reject. */
	*result	= tRejected;

	/* There are two possible formats, list of ASCII and ASCII. This handles both. */
	hResult	= lines->GetDataType(0, 0/*term_id*/, &valType); 
	if (FAILED(hResult))
	{
		CheckResult(hResult, L"TerminalMsgRcvd::TerminalMsgRcvd failed on GetDataType"); 
		return S_FALSE; 
	}

	strMesg.Format(L"From host #%ld : ", conn_id); 
	switch (valType)
	{
	case L :	lines->ItemCount(0, &lLines);
			for (i=0; i<lLines; i++)
			{
				if (i > 0)	strMesg	+= L"\r\n";
				hResult	= lines->GetValueAscii(0, i+1, &bstrMesg1);
				if (SUCCEEDED(hResult))
				{
					bstrMesg2		= bstrMesg1;
					lSizeOfMesgLen	+= bstrMesg2.length();
					strMesg.Append(bstrMesg2);
				}
			}
			break;
	case A	:	hResult = lines->GetValueAscii(0, 0, &bstrMesg1); 
				if (SUCCEEDED(hResult))
				{
					bstrMesg2		= bstrMesg1;
					lSizeOfMesgLen	+= bstrMesg2.length();
					strMesg.Append(bstrMesg2);
				}
				break;
	default	:	CheckResult(hResult, L"TerminalMsgRcvd::TerminalMsgRcvd failed due to unexpected value type"); 
				return S_FALSE; 
	}

	/* 메시지 수신 완료했다고 플래그 설정 */
	*result = tAccepted;

	/* 메시지를 공유 객체에 등록 */
	if (strMesg.GetLength())	m_pSharedData->TerminalMsgRcvd(conn_id, term_id, strMesg.GetBuffer());

	return S_OK;
}

#if 0	/* 아래 콜백함수는 EPJ 파일 내에 "RecipeHandling" 값이 2로 설정된 경우만 유효 함 (0=None 1=File-Based 2=Value-Based) */
/*
 desc : 현재 장치에 등록된 Process Program (Recipe) 요청
 parm : conn_id		- [in]  The connection identification number (0 if common)
		recipe_names- [in]  Host 쪽에 반환되어야 할 레시피 등록 정보
 retn : S_OK or S_FAIL
 note : CIMConnect calls this method when a connection requests a list of process programs and
		an application has registered as a process program handler.
		This function is called when the well-known variable RecipeHandling is set to 2 - value based recipe
*/
STDMETHODIMP CEMSvcCbk::PPDRequest(long conn_id, ICxValueDisp *recipe_names)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG lHandle	= 0, i = 0;
	HRESULT hResult		= S_OK;
	CStringArray arrRecipe;

	/* Shared memory 영역에서 현재 등록된 레시피 가져오기 */
	if (!m_pSharedData)	return S_FALSE;

	/* conn_id와 동일한 레시피 추출 */
	if (m_pSharedData->GetRecipeList(conn_id, arrRecipe))
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"PPDRequest::AppendList(0, &lHandle)");
		hResult	= recipe_names->AppendList(0, &lHandle);
		if (m_pLogData->CheckResult(hResult, tzMesg))
		{
			arrRecipe.RemoveAll();
			return S_FALSE;
		}
		for (; i<arrRecipe.GetCount(); i++)
		{
			swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"PPDRequest::AppendValueAscii(%d, %s)",
					   lHandle, arrRecipe[i].GetBuffer());
			hResult	= recipe_names->AppendValueAscii(lHandle, CComBSTR(arrRecipe[i].GetBuffer()));
			if (m_pLogData->CheckResult(hResult, tzMesg))
			{
				arrRecipe.RemoveAll();
				return S_FALSE;
			}
		}
	}

	/* 메모리 반환 */
	arrRecipe.RemoveAll();

	return S_OK;
}
#endif

/*
 desc : Host로부터 Process Program (= Recipe)을 등록하라는 요청이 온 경우
 parm : conn_id		- [in]  The connection identification number (0 if common)
		recipe_name	- [in]  적재 (등록)하려는 레시피 이름 (파일 이름. 경로 포함?)
		recipe		- [out] Recipe ID와 BODY 부분으로 채워질 List Value 반환 객체
		result		- [out] 레시피 적재 정상 여부 값 반환
 retn : S_OK or S_FAIL
 note : CIMConnect calls this method when a connection requests a process program and
		the application has registered as a process program handler.
		This function is called when the well-known variable RecipeHandling value is set to either
		1 (file based) or 2 (value based).
		In the case where RecipeHandling has been set to 1, the application's responsibility is
		to simply determine whether to approve the host's request to upload the process program
		by assigning the proper value in the result argument.
		If the recipe pointer passed into the callback is NULL and the result value is set to raAccepted,
		the CIMConnect will retrieve the contents of the process program from the hard-drive and
		build the S7F6 response to the host using the proper file as the PPBODY data item
*/
STDMETHODIMP CEMSvcCbk::PPRequest(long conn_id, BSTR recipe_name, ICxValueDisp *recipe,
								  RecipeAck *result)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	TCHAR tzRecipeFile[MAX_PATH_LEN]= {NULL};
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	PTCHAR ptzRecipePath	= NULL, ptzRecipeExt = NULL;

	/* TEL 장비의 경우, RecipeHandling 값이 무조건 1로 설정되어 있어야 하므로, 아래 값 (반환 값)은 NULL로 초기화 */
	recipe	= NULL;
	*result	= RecipeAck::raRejected;	/* 일단. 무조건 실패 했다고 값을 설정 */

	/* 레시피 경로 얻기 */
	ptzRecipePath	= m_pSharedData->GetRecipePath();
	if (!ptzRecipePath)
	{
		m_pLogData->PushLogs(L"Failed to get the process program (=recipe) path", ENG_GALC::en_warning);
		return S_FALSE;
	}
	/* 확장자 가져오기 */
	ptzRecipeExt	= m_pSharedData->GetRecipeExtension();

	/* 레시피 경로 내에 "recipe_name"과 일치하는 파일이 존재하는지 확인 */
	if (wcslen(ptzRecipeExt) > 0)
	{
		swprintf_s(tzRecipeFile, MAX_PATH_LEN, L"%s\\%s",
				   ptzRecipePath, (LPTSTR)_bstr_t(recipe_name));
	}
	else
	{
		swprintf_s(tzRecipeFile, MAX_PATH_LEN, L"%s\\%s.%s",
				   ptzRecipePath, (LPTSTR)_bstr_t(recipe_name), ptzRecipeExt);
	}

	/* recipe_name을 HDD 드라이브에서 없거나, 적재 테스트가 실패한 경우, 에러 반환 */
	if (!uvCmn_FindFile(tzRecipeFile))
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"Failed to find the recipe file (%s)", tzRecipeFile);
		m_pLogData->PushLogs(tzMesg, ENG_GALC::en_warning);
		return S_FALSE;
	}

	/* 적재할 것이고, 적재하는데 실패하지 않을 것이다 */
	*result	= RecipeAck::raAccepted;
	/* Event 알림 */
	m_pSharedData->E30_PPData(E30_CEFI::en_pp_request, conn_id, tzRecipeFile);

	return S_OK;
}

/* --------------------------------------------------------------------------------------------- */
/*                                    ICxEMAppCallback4 Methods                                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Host 측에서 EC (Equipment Constant) 값 변경을 했을 경우, 호출되는 콜백 함수 임
 parm : equip_id- [in]  The identification number of equipment requesting the Clock change
		conn_id	- [in]  The identification number of connection requesting the Clock change
 retn : S_OK or S_FAIL
 note : CIMConnect calls this method after the Clock value has been changed
		The change may be in response to GEM Host S2F31 message,
		S2F18 reply from GEM Host or client application changing the Clock variable.
*/
STDMETHODIMP CEMSvcCbk::ClockChange(long equip_id, long conn_id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

/* --------------------------------------------------------------------------------------------- */
/*                                    ICxEMAppCallback5 Methods                                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Host 측에서 EC (Equipment Constant) 값 변경을 했을 경우, 호출되는 콜백 함수 임
 parm : conn_id	- [in]  The connection identification number (0 if common)
		var_name- [in]  Variable Name
		new_val	- [in]  새로 변경된 값
 retn : S_OK or S_FAIL
 note : Variable Name에 해당되는 값 (new_val)을 장비 환경에서도 변경을 해야 함
*/
STDMETHODIMP CEMSvcCbk::ECChanged(long conn_id, BSTR var_name, VARIANT new_val)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	/* 이벤트 등록 */
	m_pSharedData->E30_ECChanged(conn_id, (LPTSTR)_bstr_t(var_name));

	return S_OK;
}

/* --------------------------------------------------------------------------------------------- */
/*                                    ICxEMAppCallback6 Methods                                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Handler for value change callback
 parm : conn_id	- [in]  The connection identification number (0 if common)
		var_id	- [in]  The variable identification number
		var_name- [in]  The variable name
		val_buff- [in]  The variable value in ICxValueDisp::CopyToByteStream/RestoreFromByteStream format
 retn : S_OK or S_FAIL
*/
STDMETHODIMP CEMSvcCbk::ValueChanged(long conn_id, LONG var_id, BSTR var_name, VARIANT val_buff)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return S_OK;
}

/* --------------------------------------------------------------------------------------------- */
/*                                  ICxEMMsgCallback Methods                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Used for logging SECS Message traffic
		CIMConnect calls this for logging message traffic when RegisterMsgLogger has been called
 parm : mesg_buff	- [in]  SAFEARRAY of VT_UI1 byte buffer from ICxEMInternalMsg::CopyToByteStream
 retn : S_OK or S_FALSE
 note : The messages are for one equipment and connection. msg contains a byte buffer generated
		using ICxEMInternalMsg::CopyToByteStream. A new ICxEMInternalMsg should be create and
		ICxEMInternalMsg::RestoreFromByteStream should be used to initialize the message.
		The message properties may then be used to generate the logs.
*/
STDMETHODIMP CEMSvcCbk::LogMessage(VARIANT mesg_buff)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ParseLogMessage(mesg_buff);
	return S_OK;
}

/*
 desc : Used for logging SECS Message traffic
		CIMConnect calls this for logging message traffic when RegisterMsgLogger has been called
 parm : conn_id		- [in]  The connection identification number
		mesg_id		- [in]  The message ID
		trans_id	- [in]  The transaction ID
		reply		- [in]  If a reply is expected
		message		- [in]  The body of the message
 retn : S_OK or S_FALSE
 note : The messages are for one equipment and connection. msg contains a byte buffer generated
		using ICxEMInternalMsg::CopyToByteStream. A new ICxEMInternalMsg should be create and
		ICxEMInternalMsg::RestoreFromByteStream should be used to initialize the message.
		The message properties may then be used to generate the logs.
*/
STDMETHODIMP CEMSvcCbk::MessageSent(long conn_id, long mesg_id, long trans_id,
									VARIANT_BOOL reply, ICxValueDisp *message)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return S_OK;
}

/* --------------------------------------------------------------------------------------------- */
/*                                    ICxE39Callback Methods                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Notification that an E39 object attribute's value has been set
 parm : obj			- [in]  The object whose attribute was set
		attr_name	- [in]  The attribute name
		value		- [in]  The new attribute value
 retn : S_OK or S_FAIL
 note : When an E39 object has registered an ICxE39InfoCB interface using CCxE39Obj::RegisterE39InfoCallback,
		this callback function will get called when an E39 attribute value is set
		An attribute value can be set from the host using the SetAttr service (S14F3 - SetAttr request),
		or locally using the API function CCxE39Obj::SetAttr
*/
STDMETHODIMP CEMSvcCbk::AttrChange(ICxE39Obj *obj, BSTR attr_name, ICxValueDisp *value)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return S_OK;
}

/*
 desc : Notification about the addition of a new E39 child object
 parm : parent	- [in]  The parent object
		obj_spec- [in]  The new object's object specifier
		obj		- [in]  The new child object
 retn : S_OK or S_FAIL
 note : This callback method is called when a new E39 child object is added to the parent object
*/
STDMETHODIMP CEMSvcCbk::ObjAdd(ICxE39Obj *parent, BSTR obj_spec, ICxE39Obj *obj)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return S_OK;
}

/*
 desc : Notification about removal of an E39 child object
 parm : parent	- [in]  The parent object
		obj_spec- [in]  The child object's object specifier
		obj		- [in]  The child object
 retn : S_OK or S_FAIL
 note : This callback method is called when a E39 child object is removed from the parent object
*/
STDMETHODIMP CEMSvcCbk::ObjRemove(ICxE39Obj *parent, BSTR obj_spec, ICxE39Obj *obj)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return S_OK;
}

/*
 desc : Notification about an E39 object attribute's value change
 parm : parent_obj_spec	- [in]  The ObjSpec attribute of the parent E39 object
		obj_id			- [in]  The ObjID attribute of the E39 object whose attribute has changed
		obj_type		- [in]  The ObjType attribute of the E39 object whose attribute has changed
		attr_name		- [in]  The E39 attribute name that has changed
		value_buff		- [in]  The new value of the attribute. Use ICxValueDisp::RestoreFromByteStream() to use this parameter
 retn : S_OK or S_FAIL
 note : When an E39 object is registered using ICxE39Obj::RegisterE39InfoCallback2(),
		this callback method will be called when an attribute's value is changed or
		when a new attribute is added
*/
STDMETHODIMP CEMSvcCbk::AttrChange(BSTR parent_obj_spec, BSTR obj_id, BSTR obj_type,
								   BSTR attr_name, VARIANT value_buff)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return S_OK;
}

/*
 desc : Notification about the addition of a new E39 child object
 parm : parent_obj_spec	- [in]  The ObjSpec attribute of the parent E39 object
		obj_id			- [in]  The ObjID attribute of the E39 object whose attribute has changed
		obj_type		- [in]  The ObjType attribute of the E39 object whose attribute has changed
		attr_name		- [in]  A list of attribute names
								Use ICxValueDisp::RestoreFromByteStream() to use this parameter
								The format is "L {A attributeName1}{A attributeName2}...".
		attr_values		- [in]  A list of attribute values that correspond to the names in attributeNames
								Use ICxValueDisp::RestoreFromByteStream() to use this parameter
 retn : S_OK or S_FAIL
 note : This callback method is called when a new E39 child object is added using ICxE39Obj::Add()
		ICxE39Obj::RegisterE39InfoCallback2() is used to register the object to receive this notification
*/
STDMETHODIMP CEMSvcCbk::ObjAdd(BSTR parent_obj_spec, BSTR obj_id, BSTR obj_type,
							   VARIANT attr_name, VARIANT attr_values)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return S_OK;
}

/*
 desc : Notification about the removal of an E39 child object
 parm : parent_obj_spec	- [in]  The ObjSpec attribute of the parent E39 object
		obj_id			- [in]  The ObjID attribute of the E39 object whose attribute has changed
		obj_type		- [in]  The ObjType attribute of the E39 object whose attribute has changed
		attr_name		- [in]  A list of attribute names
								Use ICxValueDisp::RestoreFromByteStream() to use this parameter
								The format is "L {A attributeName1}{A attributeName2}...".
		attr_values		- [in]  A list of attribute values that correspond to the names in attributeNames
								Use ICxValueDisp::RestoreFromByteStream() to use this parameter
 retn : S_OK or S_FAIL
 note : This callback method is called when an E39 child object has been removed using ICxE39Obj::Remove()
		ICxE39Obj::RegisterE39InfoCallback2() is used to register the object to receive this notification
*/
STDMETHODIMP CEMSvcCbk::ObjRemove(BSTR parent_obj_spec, BSTR obj_id, BSTR obj_type,
								  VARIANT attr_name, VARIANT attr_values)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return S_OK;
}


/* --------------------------------------------------------------------------------------------- */
/*                                  ICxE40Callback Methods                                      */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Process Job이 변경되었을 때, 호출됨
 parm : job_id	- [in]  Process Job ID (변경된 Process Job ID?)
		state	- [in]  현재 변경된 상태 값
 retn : S_OK or S_FAIL
*/
STDMETHODIMP CEMSvcCbk::PRStateChange(BSTR job_id, ProcessJobStateEnum state)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	/* 이벤트 등록 */
	m_pSharedData->E40_PRStateChange(job_id, state);

	return S_OK;
}

/*
 desc : Request to equipment to perform one of the commands
 parm : cmd_src		- [in]  Enumeration identifying the source of the command execution
		cmd			- [in]  Callback command code
		proc_id		- [in]  Object identifier of Process Job for the command
		parm_list	- [in]  List of CmdParameter L {L {A CPNAME1}{CPVAL1}} ... {L {A CPNAMEn}{CPVALn}}.
							If command is pjcbCREATE, pjcbSETUP, or pjcbWAITINGFORSTART,
							this parameter is not used and the value will be set equal to NULL
							If command is pjcbSTART and the ProcessJob state transition is
							from SETTING UP to WAITING FOR START or PROCESSING,
							this parameter is not used and the value will be set equal to NULL
		job_obj		- [in]  Interface pointer of Process Job for the command
		status		- [out] Returned status L {Bo ACKA} {L {L {U4 ERRCODE} {A ERRTEXT}}...}
 retn : S_OK or S_FALSE
 note : Notify equipment that a process job command is requested
		Application can use pJobObject to get Process Job's properties
		By implementing this callback, client application can reject any of these commands
		if necessary by returning a failed HRESULT or by filling in error information
		into pStatus parameter with 'acka' set to false
		If the client application sets ACKA to false but adds no ERRCODE / ERRTEXT entries to pStatus,
		CIM300 will supply a single ERRCODE / ERRTEXT by default.
		If the client application adds one or more ERRCODE / ERRTEXT entries to pStatus,
		CIM300 will not supply any additional entries
*/
STDMETHODIMP CEMSvcCbk::PRCommandCallback(BSTR proc_id, CommandCallbackEnum cmd,
										  ICxValueDisp *parm_list, ICxProcessJob *job_obj,
										  ICxValueDisp *status)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#if 0
	CStringArray arrErrMsg;
	CE40CmdParam *pParamCmd	= NULL;
	MaterialTypeEnum enType;
#if 0
	if (status && !DispErrorData(status, E30_SVET::en_svcack))	return FALSE;
#endif
	/* 만약 Mataerial Type이 Wafer가 아니면 무조건 에러 처리 */
	HRESULT hResult = job_obj->get_PRMtlType(&enType);
	if (hResult != S_OK)
	{
		arrErrMsg.Add(L"Failed to get material type information");
		m_pLogData->PushLogs(L"Failed to get material type information", ENG_GALC::en_alarm);
	}
	else
	{
		switch (enType)
		{
		case pjWAFER		:
		case pjCARRIER		:
		case pjSUBSTRATE	:	break;
		default				:
			arrErrMsg.Add(L"This type of material is not handled by the equipment");
			m_pLogData->PushLogs(L"This type of material is not handled by the equipment", ENG_GALC::en_warning);
			break;
		}
	}
	/* parm_list 분석 */
	if (parm_list)
	{
		pParamCmd	= new CE40CmdParam;
		ASSERT(pParamCmd);
		if (!E40ParamCmdAnalysis(parm_list, pParamCmd))
		{
			delete pParamCmd;
			return S_FALSE;
		}
	}

	switch (E40_PCCC(cmd))
	{
	case E40_PCCC::en_pause				:
	case E40_PCCC::en_resume				:
	case E40_PCCC::en_stop				:
	case E40_PCCC::en_create				:
	case E40_PCCC::en_cancelcancel		:
	case E40_PCCC::en_cancelstop			:
	case E40_PCCC::en_cancelabort		:
	case E40_PCCC::en_canceldequeue		:
	case E40_PCCC::en_setup				:
	case E40_PCCC::en_abort				:
	case E40_PCCC::en_waitingforstart	:
	case E40_PCCC::en_start				:	m_pSharedData->E40_PushCommnad(E40_CEFI::en_pr_command_callback,
																	   E40_PCCC(cmd),
																	   PTCHAR(proc_id), pParamCmd);	break;
	default							:	delete pParamCmd;	return S_FALSE;
	}
#else
	if (!E40PRCommandCbk(proc_id, E40_PECS::en_unknown, E40_PCCC(cmd), parm_list, job_obj, status))
		return S_FALSE;
#endif
	return S_OK;
}
STDMETHODIMP CEMSvcCbk::PRCommandCallback(ExecuteCommandSourceEnum cmd_src,
										  BSTR proc_id, CommandCallbackEnum cmd,
										  ICxValueDisp *parm_list, ICxProcessJob *job_obj,
										  ICxValueDisp *status)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#if 0
	CE40CmdParam *pParamCmd	= NULL;
	MaterialTypeEnum enType;
	if (status && !DispErrorData(status, E30_SVET::en_svcack))	return FALSE;

	/* 만약 Mataerial Type이 Wafer가 아니면 무조건 에러 처리 */
	HRESULT hResult = job_obj->get_PRMtlType(&enType);
	if (hResult != S_OK)
	{
		m_pLogData->PushLogs(L"Failed to get material type information", ENG_GALC::en_alarm);
		return S_FALSE;
	}
	switch (enType)
	{
	case pjWAFER	:
	case pjCARRIER	:
	case pjSUBSTRATE:	break;
	default			:
		m_pLogData->PushLogs(L"This type of material is not handled by the equipment",
							 ENG_GALC::en_warning);
		return S_FALSE;
	}

	/* parm_list 분석 */
	if (parm_list)
	{
		pParamCmd	= new CE40CmdParam;
		ASSERT(pParamCmd);
		if (!E40ParamCmdAnalysis(parm_list, pParamCmd))
		{
			delete pParamCmd;
			return S_FALSE;
		}
	}

	switch (E40_PCCC(cmd))
	{
	case E40_PCCC::en_pause				:
	case E40_PCCC::en_resume				:
	case E40_PCCC::en_stop				:
	case E40_PCCC::en_create				:
	case E40_PCCC::en_cancelcancel		:
	case E40_PCCC::en_cancelstop			:
	case E40_PCCC::en_cancelabort		:
	case E40_PCCC::en_canceldequeue		:
	case E40_PCCC::en_setup				:
	case E40_PCCC::en_abort				:
	case E40_PCCC::en_waitingforstart	:
	case E40_PCCC::en_start				:	m_pSharedData->E40_PushCommnad(E40_CEFI::en_pr_command_callback2,
																	   E40_PECS(cmd_src),
																	   E40_PCCC(cmd),
																	   PTCHAR(proc_id), pParamCmd);	break;
	default							:	delete pParamCmd;	return S_FALSE;
	}
#else
	if (!E40PRCommandCbk(proc_id, E40_PECS(cmd_src), E40_PCCC(cmd), parm_list, job_obj, status))	return S_FALSE;
#endif
	return S_OK;
}

/*
 desc : PRCommandCallback 본문 내용 구현
 parm : cmd_src		- [in]  Enumeration identifying the source of the command execution
		cmd			- [in]  Callback command code
		proc_id		- [in]  Object identifier of Process Job for the command
		parm_list	- [in]  List of CmdParameter L {L {A CPNAME1}{CPVAL1}} ... {L {A CPNAMEn}{CPVALn}}.
							If command is pjcbCREATE, pjcbSETUP, or pjcbWAITINGFORSTART,
							this parameter is not used and the value will be set equal to NULL
							If command is pjcbSTART and the ProcessJob state transition is
							from SETTING UP to WAITING FOR START or PROCESSING,
							this parameter is not used and the value will be set equal to NULL
		job_obj		- [in]  Interface pointer of Process Job for the command
		status		- [out] Returned status L {Bo ACKA} {L {L {U4 ERRCODE} {A ERRTEXT}}...}
 retn : TRUE or FALSE
*/
BOOL CEMSvcCbk::E40PRCommandCbk(CComBSTR proc_id,
								E40_PECS cmd_src, E40_PCCC cmd,
								ICxValueDispPtr parm_list,
								ICxProcessJobPtr job_obj,
								ICxValueDispPtr status)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult;
	MaterialTypeEnum enMtlType;
	CE40CmdParam *pParamCmd	= NULL;
	CStringArray arrErrMsg;

	/* 상태 값 확인 */
	if (status && !DispErrorData(status, E30_SVET::en_svcack))	return FALSE;

	/* 만약 Mataerial Type이 Wafer가 아니면 무조건 에러 처리 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"Failed to get material type information. proc_id = %s", (LPTSTR)_bstr_t(proc_id));
	hResult = job_obj->get_PRMtlType(&enMtlType);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/*  */
	switch (enMtlType)
	{
	case pjWAFER	:
	case pjCARRIER	:
	case pjSUBSTRATE:	break;
	default			:
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"This type of material is not handled by the equipment. proc_id=%s", (LPTSTR)_bstr_t(proc_id));
		m_pLogData->PushLogs(tzMesg, ENG_GALC::en_alarm);
		return FALSE;
	}
	/* parm_list 분석 */
	if (parm_list)
	{
		pParamCmd	= new CE40CmdParam;
		ASSERT(pParamCmd);
		if (!E40ParamCmdAnalysis(parm_list, pParamCmd))
		{
			delete pParamCmd;
			return FALSE;
		}
	}

	/* PRJobID (String)가 NULL인지 여부 확인 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"Failed to get the process job id. proc_id = %s", (LPTSTR)_bstr_t(proc_id));
	if (uvCmn_IsSpaceInString((LPTSTR)_bstr_t(proc_id)))
	{
		arrErrMsg.Add(L"ProcessJob ID is an empty value");
		m_pLogData->PushLogs(L"ProcessJob ID is an empty value", ENG_GALC::en_alarm);
		return FALSE;
	}

	/* Process Job ID 작업 관련 처리 */
	if (!E40PRMtlNameList(proc_id, cmd, job_obj))	return FALSE;

	switch (E40_PCCC(cmd))
	{
	case E40_PCCC::en_pause				:
	case E40_PCCC::en_resume			:
	case E40_PCCC::en_stop				:
	case E40_PCCC::en_create			:
	case E40_PCCC::en_cancelcancel		:
	case E40_PCCC::en_cancelstop		:
	case E40_PCCC::en_cancelabort		:
	case E40_PCCC::en_canceldequeue		:
	case E40_PCCC::en_setup				:
	case E40_PCCC::en_abort				:
	case E40_PCCC::en_waitingforstart	:
	case E40_PCCC::en_start				:	m_pSharedData->E40_PushCommnad(E40_CEFI::en_pr_command_callback2,
																		   E40_PECS(cmd_src),
																		   E40_PCCC(cmd),
																		   PTCHAR(proc_id), pParamCmd);	break;
	default								:	if (pParamCmd)	delete pParamCmd;	return S_FALSE;
	}


	return TRUE;
}

/*
 desc : Process Material Name List (For substrates)의 속성 값 (Carrier ID, Slot ID) 갱신
 parm : cmd		- [in]  Callback command code
		proc_id	- [in]  Object identifier of Process Job for the command
		job_obj	- [in]  Interface pointer of Process Job for the command
 retn : TRUE or FALSE
*/
BOOL CEMSvcCbk::E40PRMtlNameList(CComBSTR proc_id, E40_PCCC cmd, ICxProcessJobPtr job_obj)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	UINT8 u8SlotNo	= 0;
	LONG lHandle[3]	= {NULL}, lItemCarrID = 0, lItemSlotNo=0, i, j;
	HRESULT hResult	= S_OK;
	CComBSTR bstrCarrID;
	CStringArray arrCarrID;
	CByteArray arrSlotID;
	ICxValueDispPtr pICxValue (CLSID_CxValueObject);

	/* PRMtl Name List 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRMtlNameList::get_PRMtlNameList : Process Job ID=%s",
			   (LPTSTR)_bstr_t(proc_id));
	m_pLogData->PushLogs(tzMesg);
	hResult	= job_obj->get_PRMtlNameList(&pICxValue);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 등록된 Carrier ID 개수에 따라 (L {L {A CarrierId} {L {U1 slot}...}}...) */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRMtlNameList::ItemCount");
	hResult	= pICxValue->ItemCount(0, &lItemCarrID);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Carrier ID 및 Slot 그룹 핸들 정보 얻기 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRMtlNameList::ListAt");
	hResult	= pICxValue->ListAt(0, 0, &lHandle[0]);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 등록된 Carrier ID 개수만큼 반복 */
	for (i=1; i<=lItemCarrID; i++)
	{
		/* Carrier ID & Slot Group -> [ i ]th Carrier ID [ Handle ] 정보 얻기 */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRMtlNameList::ListAt : lHandle[0]=%d, index=%d",
				   lHandle[0], i);
		hResult	= pICxValue->ListAt(lHandle[0], i, &lHandle[1]);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

		/* Carrier ID & Slot Group -> [ i ]th Carrier ID [ Carrier ID ] 얻기 */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRMtlNameList::GetValueAscii : lHandle[1]=%d, index=1",
				   lHandle[1]);
		hResult	= pICxValue->GetValueAscii(lHandle[1], 1, &bstrCarrID);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
		/* Carrier ID & Slot Group -> [ i ]th Slot Number [ Handle ] 얻기 */
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRMtlNameList::ListAt : lHandle[1]=%d, index=2",
				   lHandle[1]);
		hResult	= pICxValue->ListAt(lHandle[1], 2, &lHandle[2]);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
		/* Carrier ID & Slot Group -> [ i ]th Slot Number [ Handle ]에 대한 개수 얻기 */
		hResult	= pICxValue->ItemCount(lHandle[2], &lItemSlotNo);
		/* Carrier ID 별로 등록된 Slot Number 개수만큼 반복 후 Slot Number 얻기 */
		for (j=1; j<=lItemSlotNo; j++)
		{
			/* Slot Number 리스트 번호 얻기 */
			swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetPRMtlNameList::GetValueU1 : lHandle[2]=%d, index=%d",
					   lHandle[2], j);
			hResult	= pICxValue->GetValueU1(lHandle[2], j, &u8SlotNo);
			if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

			/* 반환 버퍼에 등록 */
			arrCarrID.Add(LPTSTR(_bstr_t(bstrCarrID)));
			arrSlotID.Add(u8SlotNo);
		}
	}

	/* SharedData 영역에 추가 작업 진행 */
	if (arrCarrID.GetCount())
	{
		m_pSharedData->SetPRJobToSubstID(cmd, proc_id, &arrCarrID, &arrSlotID);
	}

	/* 메모리 반환 */
	arrCarrID.RemoveAll();
	arrSlotID.RemoveAll();

	return (hResult == S_OK);
}

/*
 desc : CJobID 생성 처리
 parm : job_id	- [in]  Control Job ID
		object	- [in]  Control Job Object
 retn : TRUE or FALSE
*/
BOOL CEMSvcCbk::CreateCJobID(BSTR job_id, ICxE94ControlJob *object)
{
	LONG i	= 0, lCount;
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult;
	E94CJState enState;
	VARIANT varIDList;
	BSTR *bstrList;
	SAFEARRAY *pSaList;
	ICxE39ObjPtr pICxE39Obj;
	ICxValueDispPtr pICxCarrier(CLSID_CxValueObject);
	CComBSTR bstrVal;
	CString strProcID;
	CStringArray arrPRJobIDs, arrCarrIDs;

	if (!object)	return S_FALSE;

	/* Control Job ID의 현재 상태 값 */
	hResult	= object->get_CJState(&enState);
	if (hResult != S_OK)
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"CEMSvcCbk::CJCreate (job_id=%s)", LPTSTR(job_id));
		m_pLogData->CheckResult(hResult, tzMesg);
		return S_FALSE;
	}

	/* CRJob ID에 속한 PRJob IDs 얻기 */
	hResult	= object->ListProcessJobs(E94ListAll, &varIDList);
	m_pLogData->CheckResult(hResult, tzMesg);
	if (hResult	== S_OK)
	{
		pSaList	= varIDList.parray;
		hResult	= SafeArrayAccessData(pSaList, (VOID**)&bstrList);	/* Lock it down */
		m_pLogData->CheckResult(hResult, tzMesg);
		for (; i<(LONG)pSaList->rgsabound->cElements; i++)
		{
			strProcID.Format(L"%s", bstrList[i]);
			arrPRJobIDs.Add(strProcID);
//			arrCarrIDs.Add(L"");
		}
	}

	pICxE39Obj	= (ICxE39ObjPtr)object;
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobCarrierInputSpec::GetAttr : CarrierInputSpec");
	hResult	= pICxE39Obj->GetAttr(CComBSTR(L"CarrierInputSpec"), pICxCarrier);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobCarrierInputSpec::ItemCount");
	hResult	= pICxCarrier->ItemCount(0, &lCount);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	for (i=1; i<=lCount; i++)
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"GetControlJobCarrierInputSpec::ListAt : lIndex=%d", i);
		hResult	= pICxCarrier->GetValueAscii(0, i, &bstrVal);
		if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
		arrCarrIDs.Add(CString(bstrVal));
	}

	/* 이벤트 버퍼에 저장 */
	m_pSharedData->E94_PushCJState(E94_CCJS(enState), LPTSTR(job_id));
	/* 현재 수신받은 Control Job에 대해서 기존에 존재하는지 여부에 따라, 응답 값 실패 여부 확인 */
	/* Host 쪽에 S14F10 메시지 응답 해줘야 됨 */

	/* Control Job ID의 기본 정보 등록 */
	m_pSharedData->SetCRJobToProperty(E94_CCJS(enState), LPTSTR(job_id), &arrCarrIDs, &arrPRJobIDs);

	return TRUE;
}

/*
 desc : 소재의 작업 순서 변경
 parm : cur_order	- [in]  현재 작업 순서
		new_order	- [in]  새로운 작업 순서
		ack			- [out] Host에게 작업 순서 변경 여부 반환
 retn : S_OK or S_FALSE
*/
STDMETHODIMP CEMSvcCbk::PRSetMtrlOrder(MaterialOrderEnum cur_order,
									   MaterialOrderEnum new_order, VARIANT_BOOL *ack)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	/* Host에 의해 작업 순서 변경은 무조건 안된다고 설정 (실시간으로 변경할 수 없으므로) */
	*ack	= VARIANT_FALSE;
	return S_OK;
}

/* --------------------------------------------------------------------------------------------- */
/*                                    ICxE87Callback Methods                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Load Port에 Carrier가 Binding 될 때, 호출됨
 parm : port_id	- [in]  The load port identifier (0 if port ID is not known)
		carr_id	- [in]  The carrier ID (may be an empty string)
 retn : S_OK or S_FALSE
*/
STDMETHODIMP CEMSvcCbk::Bind(short port_id, BSTR carr_id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E87_PushEvent(E87_CEFI::en_bind, 0, port_id, carr_id);
	return S_OK;
}

/*
 desc : Notification of a CANCELCARRIER service request
 parm : port_id	- [in]  The load port identifier (0 if port ID is not known)
		carr_id	- [in]  The carrier ID (may be an empty string)
 retn : S_OK or S_FALSE
 note : This callback method is called when a CANCELCARRIER service is requested and is allowed
*/
STDMETHODIMP CEMSvcCbk::CancelCarrier(short port_id, BSTR carr_id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E87_PushEvent(E87_CEFI::en_cancel_carrier, 0, port_id, carr_id);
	return S_OK;
}

/*
 desc : Notification of a CANCELBIND service request
 parm : port_id	- [in]  The load port identifier (0 if port ID is not known)
		carr_id	- [in]  The carrier ID (may be an empty string)
 retn : S_OK or S_FALSE
 note : This callback method is called when a CANCELBIND service is requested and is allowed
*/
STDMETHODIMP CEMSvcCbk::CancelBind(short port_id, BSTR carr_id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E87_PushEvent(E87_CEFI::en_cancel_bind, 0, port_id, carr_id);
	return S_OK;
}

/*
 desc : Notification of a CancelCarrierAtPort service request
 parm : port_id	- [in]  The load port identifier (0 if port ID is not known)
 retn : S_OK or S_FALSE
 note : This callback method is called when a CancelCarrierAtPort service is requested and is allowed
*/
STDMETHODIMP CEMSvcCbk::CancelCarrierAtPort(short port_id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E87_PushEvent(E87_CEFI::en_cancel_carrier_at_port, 0, port_id, NULL);
	return S_OK;
}

/*
 desc : Notification of a CancelCarrierNotification service request
 parm : carr_id	- [in]  The carrier ID (may be an empty string)
 retn : S_OK or S_FALSE
 note : This callback method is called when a CancelCarrierNotification service is requested and is allowed
*/
STDMETHODIMP CEMSvcCbk::CancelCarrierNotification(BSTR carr_id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E87_PushEvent(E87_CEFI::en_cancel_carrier_noti, 0, 0, PTCHAR(_bstr_t(carr_id)));
	return S_OK;
}

/*
 desc : Notification of a CancelAllCarrierOut service request
 parm : carr_id	- [in]  The carrier ID (may be an empty string)
 retn : S_OK or S_FALSE
 note : This callback method is called when a CancelAllCarrierOut service is requested and is allowed
*/
STDMETHODIMP CEMSvcCbk::CancelAllCarrierOut()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E87_PushEvent(E87_CEFI::en_cancel_all_carrier_out, 0, NULL);
	return S_OK;
}

/*
 desc : Notification of a CancelCarrierOut service request
 parm : carr_id	- [in]  The carrier ID (may be an empty string)
 retn : S_OK or S_FALSE
 note : This callback method is called when a CancelCarrierOut service is requested and is allowed
*/
STDMETHODIMP CEMSvcCbk::CancelCarrierOut(BSTR carr_id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E87_PushEvent(E87_CEFI::en_cancel_carrier_out, 0, 0, PTCHAR(_bstr_t(carr_id)));
	return S_OK;
}

/*
 desc : Notification of a CANCELRESERVEATPORT service request
 parm : port_id	- [in]  The load port identifier (0 if port ID is not known)
 retn : S_OK or S_FALSE
 note : This callback method is called when a CancelCarrierOut service is requested and is allowed
*/
STDMETHODIMP CEMSvcCbk::CancelReserveAtPort(short port_id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E87_PushEvent(E87_CEFI::en_cancel_reserve_at_port, 0, port_id);
	return S_OK;
}

/*
 desc : Notification of a CarrierIn service request
 parm : carr_id	- [in]  The carrier ID (may be an empty string)
 retn : S_OK or S_FALSE
 note : This callback method is called when a CarrierIn service is requested and is allowed
*/
STDMETHODIMP CEMSvcCbk::CarrierIn(BSTR carr_id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E87_PushEvent(E87_CEFI::en_carrier_in, 0, 0, PTCHAR(_bstr_t(carr_id)));
	return S_OK;
}

/*
 desc : Notification of a CarrierNotification service request
 parm : carr_id	- [in]  The carrier ID (may be an empty string)
 retn : S_OK or S_FALSE
 note : This callback method is called when a CarrierNotification service is requested and is allowed
*/
STDMETHODIMP CEMSvcCbk::CarrierNotification(BSTR carr_id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E87_PushEvent(E87_CEFI::en_carrier_noti, 0, 0, PTCHAR(_bstr_t(carr_id)));
	return S_OK;
}

/*
 desc : Notification of a CARRIEROUT service request
 parm : carr_id	- [in]  The carrier ID (may be an empty string)
		port_id	- [in]  The load port identifier (0 if port ID is not known)
 retn : S_OK or S_FALSE
 note : This callback method is called when a CARRIEROUT service is requested and is allowed
*/
STDMETHODIMP CEMSvcCbk::CarrierOut(short port_id, BSTR carr_id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E87_PushEvent(E87_CEFI::en_carrier_out, 0, port_id, PTCHAR(_bstr_t(carr_id)));
	return S_OK;
}

/*
 desc : Notification of a CarrierReCreate service request
 parm : carr_id	- [in]  The carrier ID (may be an empty string)
		port_id	- [in]  The load port identifier (0 if port ID is not known)
 retn : S_OK or S_FALSE
 note : This callback is used to notify the equipment application that a CarrierReCreate service
		has been requested and CIM87 has not found any problems with the request
*/
STDMETHODIMP CEMSvcCbk::CarrierReCreate(BSTR carr_id, short port_id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	/* 이 때, Load Port가 반드시 READY TO UNLOAD 상태이어야 됨 (Load Port 상태 확인) (E87CMS.CPP에서 확인) */
	m_pSharedData->E87_PushEvent(E87_CEFI::en_carrier_recreate, 0, port_id, PTCHAR(_bstr_t(carr_id)));
#if 0	/* 제어 SW에서 한다 */
	/* Remove Share ... */
	m_pSharedData->RemoveSharedCarrID(PTCHAR(_bstr_t(carr_id)));
#endif

	return S_OK;
}

/*
 desc : Notification of a CarrierRelease service request
 parm : loc_id	- [in] The ID for the location of the carrier
		carr_id	- [in]  The carrier ID (may be an empty string)
 retn : S_OK or S_FALSE
 note : This callback method is called when a CarrierRelease service is requested and is allowed
		!!! Leave the status argument empty if no errors occur !!!
*/
STDMETHODIMP CEMSvcCbk::CarrierRelease(BSTR loc_id, BSTR carr_id, ICxValueDisp *status)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (status)	DispErrorData(status, E30_SVET::en_caack);
	else		m_pSharedData->E87_PushEvent(E87_CEFI::en_carrier_release, 0, 0, PTCHAR(_bstr_t(carr_id)), PTCHAR(_bstr_t(loc_id)));
	return status ? S_FALSE : S_OK;
}

/*
 desc : Notification of a ChangeAccessMode service request
 parm : new_mode	- [in]  The new access mode, 1 = Auto, 0 = Manual
		port_id		- [in]  The port to change the access mode
 retn : S_OK or S_FALSE
 note : This callback method is called when a ChangeAccessMode service is requested and is allowed
*/
STDMETHODIMP CEMSvcCbk::ChangeAccessMode(short new_mode, short port_id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E87_PushEvent(E87_CEFI::en_change_access_mode, new_mode, port_id);
	return S_OK;
}

/*
 desc : Load Port의 동작 상태가 변경될 때, 호출 됨
 parm : port_id	- [in]  The load port identifier (0 if port ID is not known)
		state	- [in]  The requested service status
 retn : S_OK or S_FALSE
*/
STDMETHODIMP CEMSvcCbk::ChangeServiceStatus(short port_id, LoadPortTransferStateEnum state)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E87_PushEvent(E87_CEFI::en_change_service_status, LONG(state), port_id);
	return S_OK;
}

/*
 desc : Notification of a PROCEEDWITHCARRIER service request
 parm : carr_id	- [in]  The carrier ID (may be an empty string)
		port_id	- [in]  The load port identifier (0 if port ID is not known)
 retn : S_OK or S_FALSE
 note : This callback method is called when a PROCEEDWITHCARRIER service is requested and is allowed
*/
STDMETHODIMP CEMSvcCbk::ProceedWithCarrier(short port_id, BSTR carr_id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E87_PushEvent(E87_CEFI::en_proceed_with_carrier, 0, port_id, PTCHAR(_bstr_t(carr_id)));
	return S_OK;
}

/*
 desc : Notification of a RESERVEATPORT service request
 parm : port_id	- [in]  The load port identifier (0 if port ID is not known)
 retn : S_OK or S_FALSE
 note : This callback method is called when a RESERVEATPORT service is requested and is allowed
*/
STDMETHODIMP CEMSvcCbk::ReserveAtPort(short port_id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E87_PushEvent(E87_CEFI::en_reserve_at_port, 0, port_id);
	return S_OK;
}

/* --------------------------------------------------------------------------------------------- */
/*                                    ICxE87Callback2 Methods                                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Notification of a RESERVEATPORT service request
 parm : port_id	- [in]  The load port identifier (0 if port ID is not known)
 retn : S_OK or S_FALSE
 note : This callback method is called when a RESERVEATPORT service is requested and is allowed
*/
STDMETHODIMP CEMSvcCbk::AlarmChanged(BSTR alarm_id, VARIANT_BOOL is_set)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E87_PushEvent(E87_CEFI::en_alarm_changed, is_set, 0,
								 ENG_GCPI::en_e87_alarm_id, ENG_GCPI::en_empty_param, alarm_id, NULL);
	return S_OK;
}

/*
 desc : Notification of carrier ID verification
 parm : carr_id	- [in]  The carrier ID (may be an empty string)
		port_id	- [in]  The load port identifier (0 if port ID is not known)
 retn : S_OK or S_FALSE
 note : This function is called when the CarrierID status changes to ID_VERIFICATION_OK while loading
*/
STDMETHODIMP CEMSvcCbk::CarrierIDVerified(short port_id, BSTR carr_id, ICxE87Carrier *pCarrier)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT hResult			= S_FALSE;
	IDStatusEnum enStatus	= ID_VERIFICATION_FAILED;

	if (pCarrier)	hResult	= pCarrier->get_IDStatus(&enStatus);
	m_pSharedData->E87_PushEvent(E87_CEFI::en_carrier_id_verified, LONG(enStatus), port_id, PTCHAR(_bstr_t(carr_id)));
	return (hResult == S_OK);
}

/*
 desc : Notification of carrier slot map verification
 parm : carr_id	- [in]  The carrier ID (may be an empty string)
		port_id	- [in]  The load port identifier (0 if port ID is not known)
 retn : S_OK or S_FALSE
 note : This function is called when the SlotMap status of a carrier changes to SM_VERIFICATION_OK while loading
*/
STDMETHODIMP CEMSvcCbk::SlotMapVerified(short port_id, BSTR carr_id, ICxE87Carrier *pCarrier)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT hResult				= S_FALSE;
	SlotMapStatusEnum enStatus	= SM_VERIFICATION_FAILED;
	if (pCarrier)	hResult	= pCarrier->get_SlotMapStatus(&enStatus);
	m_pSharedData->E87_PushEvent(E87_CEFI::en_slot_map_verified, LONG(enStatus), port_id, PTCHAR(_bstr_t(carr_id)));
	return S_OK;
}

/* --------------------------------------------------------------------------------------------- */
/*                                    ICxE87Callback3 Methods                                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : E87 state change가 발생될 때, 호출되는 콜백 함수
 parm : machine	- [in]  state machines
						e87LPTSM	(0) : LPTSM Load Port Transfer State Machine 
						e87CIDSM	(1) : COSM Carrier Object ID Status State Machine 
						e87CSMSM	(2) : COSM Carrier Object Slot Map Status State Machine 
						e87CASM		(3) : CASM Carrier Object Accessing State Machine 
						e87LPRSM	(4) : LPRSM Load Port Reservation State Machine 
						e87LPCASM	(5) : LPCASM Load Port/Carrier Association State Machine 
						e87LPASM	(6) : LPASM Load Port Access Mode State Model 
		state	- [in]  New state. (Appropriate state enumeration value)
		port_id	- [in]  The load port identifier (0 if port ID is not known)
		carr_id	- [in]  The carrier ID (may be an empty string)
 retn : S_OK or S_FALSE
 note : This callback function is used to notify registered clients that an E87 state change has occurred
*/
STDMETHODIMP CEMSvcCbk::StateChange(CMSStateMachine machine, long state, short port_id, BSTR carr_id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	TCHAR tzCarrierID[CIM_CARRIER_ID_SIZE]	= {NULL};
	swprintf_s(tzCarrierID, CIM_CARRIER_ID_SIZE, L"%s", PTCHAR(_bstr_t(carr_id)));
	m_pSharedData->E87_PushState(UINT8(machine), state, port_id, tzCarrierID);

	return S_OK;
}

/*
 desc : E87 state change가 발생될 때, 호출되는 콜백 함수
 parm : port_id	- [in]  The load port id
		state	- [out] The suggested transfer state (In --> Out)
 retn : S_OK or S_FALSE
 note : This callback method is called when CIM87's Load Port Transfer State model has entered
		the IN SERVICE state either at system start up or transitioning from OUT OF SERVICE
*/
STDMETHODIMP CEMSvcCbk::InServiceTransferState(short port_id, LoadPortTransferStateEnum *state)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E87_PushEvent(E87_CEFI::en_in_service_transfer_state, LONG(*state), port_id);
	return S_OK;
}

/* --------------------------------------------------------------------------------------------- */
/*                                    ICxE87Callback4 Methods                                    */
/* --------------------------------------------------------------------------------------------- */

STDMETHODIMP CEMSvcCbk::CarrierRecreateFinalize(BSTR carr_id, short port_id, ICxValueDisp *pParameterlist)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E87_PushEvent(E87_CEFI::en_recreate_finalize, 0, port_id, PTCHAR(_bstr_t(carr_id)));
	return S_OK;
}

/* --------------------------------------------------------------------------------------------- */
/*                                    ICxE87Callback5 Methods                                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Notification of a CANCELCARRIER service request
 parm : port_id	- [in]  The load port where the carrier resides
		carr_id	- [in]  The carrierID to cancel
		status	- [out] Status of the CancelCarrier service. Leave empty when successful
						The format of this value is identical to S3F18 - "Carrier Action Acknowledge" message format:
 retn : S_OK or S_FALSE
*/
STDMETHODIMP CEMSvcCbk::CancelCarrier2(short port_id, BSTR carr_id, ICxValueDisp **status)
{
	BOOL bSucc	= TRUE;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

 	m_pSharedData->E87_PushEvent(E87_CEFI::en_cancel_carrier, 0, port_id, PTCHAR(_bstr_t(carr_id)));
 	if (status)	bSucc	= DispErrorData(*status, E30_SVET::en_caack);

	return status ? S_FALSE : S_OK;
}

/*
 desc : Notification of a CancelCarrierAtPort service request
 parm : port_id	- [in]  The load port identifier (0 if port ID is not known)
 retn : S_OK or S_FALSE
 note : This callback method is called when a CancelCarrierAtPort service is requested and is allowed
*/
STDMETHODIMP CEMSvcCbk::CancelCarrierAtPort2(short port_id, ICxValueDisp **status)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BOOL bSucc	= TRUE;
	m_pSharedData->E87_PushEvent(E87_CEFI::en_cancel_carrier_at_port, 0, port_id, NULL);
 	if (status)	bSucc	= DispErrorData(*status, E30_SVET::en_caack);

	return status ? S_FALSE : S_OK;

}

/* --------------------------------------------------------------------------------------------- */
/*                                    ICxE87Callback6 Methods                                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Notification of a ChangeAccessMode service request
 parm : new_mode	- [in]  The new access mode, 1 = Auto, 0 = Manual
		port_id		- [in]  The port to change the access mode
		caak		- [in]  The return code 
 retn : S_OK or S_FALSE
 note : This callback method is called when a ChangeAccessMode service is requested and is allowed
		Use this callback when the equipment application wants to have control over the value of CAACK
*/
STDMETHODIMP CEMSvcCbk::ChangeAccessMode2(short new_mode, short port_id, int *caack)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E87_PushEvent(E87_CEFI::en_change_access_mode, new_mode, port_id);
	/* 반화 코드 값에는 추후 해당 모드 변경을 실행하겠다라고 플래그 설정 */
	*caack	= int (ENG_CAACK::en_will_be_performed);

	return S_OK;
}

/* --------------------------------------------------------------------------------------------- */
/*                                    ICxE87Callback8 Methods                                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : A carrier has been removed from a port
 parm : carr_id	- [in]  The carrier ID (may be an empty string)
		port_id	- [in]  The load port identifier (0 if port ID is not known)
 retn : S_OK or S_FALSE
 note : This callback method is called when a carrier is removed from a port
*/
STDMETHODIMP CEMSvcCbk::CarrierRemoved(BSTR carr_id, short port_id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E87_PushEvent(E87_CEFI::en_carrier_removed, 0, port_id, PTCHAR(_bstr_t(carr_id)));
	return S_OK;
}

/*
 desc : Notification of alarm state change
 parm : port_id	- [in]  The load port identifier (0 if port ID is not known)
		is_set	- [in]  The alarm is set or cleared (!!! This parameter's value will always be true !!!)
 retn : S_OK or S_FALSE
 note : This callback method is called when a load port-related alarm is set by CIM87
*/
STDMETHODIMP CEMSvcCbk::PortAlarmChanged(short port_id, BSTR alarm_id, VARIANT_BOOL is_set)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E87_PushEvent(E87_CEFI::en_port_alarm_changed, is_set, port_id, 
								 ENG_GCPI::en_e87_alarm_id, ENG_GCPI::en_empty_param, alarm_id, NULL);
	return S_OK;
}

/* --------------------------------------------------------------------------------------------- */
/*                                       ICxE87CMS Methods                                       */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Notification of alarm state change
 parm : loc_id	- [in]  
		carr_id	- [in] 
		segment	- [in]  
		length	- [in]  
		trans_id- [in]  
		status	- [in]  
 retn : S_OK or S_FALSE
 note : This callback method is called when a load port-related alarm is set by CIM87
*/
STDMETHODIMP CEMSvcCbk::AsyncCarrierTagReadData(BSTR loc_id, BSTR carr_id, BSTR segment,
												long length, long trans_id, ICxValueDisp *status)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BOOL bSucc	= TRUE;
	m_pSharedData->SetTransactionInfo(loc_id, carr_id, segment, length, trans_id);
	m_pSharedData->E87_PushEvent(E87_CEFI::en_async_carrier_tag_read, 0, 0, 
								 ENG_GCPI::en_e87_carrier_id, ENG_GCPI::en_e87_location_id, PTCHAR(_bstr_t(carr_id)), PTCHAR(_bstr_t(loc_id)));
	if (status)	bSucc	= DispErrorData(status, E30_SVET::en_caack);

	return status ? S_FALSE : S_OK;
}

/*
 desc : Notification of alarm state change
 parm : loc_id	- [in]  Location identifier (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		carr_id	- [in]  Carrier identifier
		segment	- [in]  The DataSeg parameter
		length	- [in]  The DataLength parameter
		data	- [out] The returned Data parameter
		status	- [in]  Status (leave empty for no errors)
 retn : S_OK or S_FALSE
 note : This callback method is called when a load port-related alarm is set by CIM87
*/
STDMETHODIMP CEMSvcCbk::CarrierTagReadData(BSTR loc_id, BSTR carr_id, BSTR segment, long length,
										   BSTR *data, ICxValueDisp *status)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BOOL bSucc	= TRUE;
	m_pSharedData->E87_PushEvent(E87_CEFI::en_async_carrier_tag_read, 0, 0, 
								 ENG_GCPI::en_e87_carrier_id, ENG_GCPI::en_e87_location_id, PTCHAR(_bstr_t(carr_id)), PTCHAR(_bstr_t(loc_id)));
	if (status)	bSucc	= DispErrorData(status, E30_SVET::en_caack);
	return status ? S_FALSE : S_OK;
}

/* --------------------------------------------------------------------------------------------- */
/*                                 ICxE90SubstrateCB Methods                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Notification of a Command service request
 parm : st_loc		- [in]  The object ID for the substrate.
		command		- [in]  The command to be executed
		parm_list	- [in]  The parameters for the command
		sub_obj		- [in]  The substrate object
		status		- [in]  The container object for any erroneous return data;
 retn : S_OK or S_FAIL
*/
STDMETHODIMP CEMSvcCbk::E90CommandCallback(BSTR st_loc, SubstrateCommandEnum command,
										   ICxValueDisp *parm_list, ICxSubstrate *sub_obj,
										   ICxValueDisp *status)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BOOL bSucc	= TRUE;

	if (status)	bSucc	= DispErrorData(status, E30_SVET::en_caack);
	else
	{
		if (!sub_obj)	return S_FALSE;
		m_pSharedData->E90_PushCommnad(E90_SPSC(command), st_loc);
	}

	return status ? S_FALSE : S_OK;
}

/*
 desc : Notification of a substrate state change
 parm : st_loc		- [in]  The object ID for the substrate
		machine		- [in]  Which state machine is changing
		new_state	- [in]  The new state in the state model
							이 값은 machine (SubstrateStateMachineEnum) 값에 따라 달라짐
							switch (machine)
							{
							case sisNO_SM					:	break;
							case sisSUBSTRATE_TRANSPORT_SM	:	SubstrateTransportStateEnum;	break;
							case sisSUBSTRATE_PROCESSING_SM	:	SubstrateProcessingStateEnum;	break;
							case sisSUBSTRATE_READER_SM		:	SubstIDStatusEnum;				break;
							case sisSUBSTRATE_LOCATION_SM	:	SubstrateLocationStateEnum;		break;
							case sisBATCH_LOCATION_SM		:	BatchLocStateEnum;				break;	// 이 값은 무시
							}
		sub_obj		- [in]  The substrate object
 retn : S_OK or S_FAIL
*/
STDMETHODIMP CEMSvcCbk::SubstrateStateChange(BSTR st_loc, SubstrateStateMachineEnum machine,
											 long new_state, ICxSubstrate *sub_obj)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pSharedData->E90_PushEvent(E90_CEFI::en_substrate_state_change,
								 E90_VSMC(machine), new_state, st_loc);

	return S_OK;
}

/* --------------------------------------------------------------------------------------------- */
/*                                 ICxE94ControlJobCB Methods                                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 새로운 Control Job이 만들어졌으면 알림
 parm : job_id	- [in]  The control job의 Object ID
		object	- [in]  The new control job object
 retn : S_OK or S_FALSE
*/
STDMETHODIMP CEMSvcCbk::CJCreate(BSTR job_id, ICxE94ControlJob *object)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!CreateCJobID(job_id, object))	return S_FALSE;
	return S_OK;
}

/*
 desc : Notification about the moving of a control job object to the top of the queue
		JOB_ID가 맨 위쪽으로 이동한 경우, 작업 순서가 바뀐 경우
 parm : job_id	- [in]  The Control Job's object identifier
		object	- [in]  The interface pointer to the control job object
 retn : S_OK or S_FALSE
 note : This callback method is called when a control job is moved to the top of the queue
		through use of the CJHOQ service. It cannot be rejected using this callback
*/
STDMETHODIMP CEMSvcCbk::CJHOQ(BSTR job_id, ICxE94ControlJob *object)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!object)	return S_FALSE;
	m_pSharedData->E94_PushEvent(E94_CEFI::en_cj_hoq, ENG_GCPI::en_e94_control_job_id, LPTSTR(job_id));
	return S_OK;
}

/*
 desc : Notification about a control job state change.
 parm : job_id		- [in]  The control job that changed state.
		new_state	- [in]  The new control job state
 retn : S_OK or S_FALSE
*/
STDMETHODIMP CEMSvcCbk::CJStateChange(BSTR job_id, E94CJState new_state)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	/* 이벤트 버퍼에 저장 */
	m_pSharedData->E94_PushCJState(E94_CCJS(new_state), LPTSTR(job_id));
	return S_OK;
}

/*
 desc : Notification about de-selection of a control job object
 parm : job_id	- [in]  The control job that changed state.
		object	- [in]  The interface pointer to the control job object
 retn : S_OK or S_FALSE
 note : This callback method is called when a control job is deselected through the use of the CJDeselect service
*/
STDMETHODIMP CEMSvcCbk::CJDeselect(BSTR job_id, ICxE94ControlJob *object)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pSharedData->E94_PushEvent(E94_CEFI::en_cj_deselect, ENG_GCPI::en_e94_control_job_id, LPTSTR(job_id));
	return S_OK;
}

/*
 desc : Request for the ID of the next process job to start
		다음에 처리할 Project Job ID 반환
 parm : object	- [in]  The interface pointer to the control job object
		order	- [in]  The ProcessOrderMgmt attribute value (can be ARRIVAL, OPTIMIZE)
		proc_id	- [out] The process job's ID
 retn : S_OK or S_FALSE
 note : This callback method is called when a control job is selected
*/
STDMETHODIMP CEMSvcCbk::GetNextPrJobID(ICxE94ControlJob *object,
									   E94CJProcessOrderMgmt order, BSTR *proc_id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!object)	return S_FALSE;

	INT32 i, i32Count, i32Last;
	UINT32 u32Count	= 0;
	E40_PPJS enState;
	PTCHAR ptzPRJobID;
	CString strPRJobID;
	CStringArray arrPRJobIDs;

	/* 다음에 처리 (동작)해야 할 Process Job ID 알림 */
	m_pSharedData->E94_PushEvent(E94_CEFI::en_get_next_pj_job_id, E94_CPOM(order), LPTSTR(proc_id));

	/* 모든 PRJob IDs를 로드해서. 끝에서 부터 찾는다. 상태가 아직 큐드이고, Substrate Count가 */
	/* 0 보다 크면, Process Job이 Null이 아니면, Process Job ID를 받아서 리턴                 */
	m_pSharedData->GetAllPRJobIDs(arrPRJobIDs);
	i32Count= (INT32)arrPRJobIDs.GetCount();
	i32Last	= i32Count - 1;
	for (i=i32Last; i>=0; i--)
	{
		strPRJobID	= arrPRJobIDs[i];
		ptzPRJobID	= strPRJobID.GetBuffer();
		m_pSharedData->GetPRJobState(ptzPRJobID, enState);
		if (enState == E40_PPJS::en_queued)
		{
			m_pSharedData->GetCountSubstID(ptzPRJobID, u32Count);
			if (u32Count > 0)
			{
				*proc_id = strPRJobID.AllocSysString();
				break;
			}
		}
	}

	return S_OK;
}

/* --------------------------------------------------------------------------------------------- */
/*                                 ICxE94ControlJobCB3 Methods                                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Request for the ID of the next process job to start
		다음에 처리할 Project Job ID 반환
 parm : cmd_src		- [in]  Enumeration identifying the source of the command execution
		ctrl_job_id	- [in]  Object identifier of Control Job for the command
		command		- [in]  Callback command code
		parm_list	- [in]  List of CmdParameter
		job_obj		- [in]  Interface pointer of Control Job for the command
		status		- [in]  Returned status. The format differs depending on command
							When command is cjCommandCreate:
								L {U1 objack} {L {L {U4 errcode} {A errtext} } ...};
							otherwise:
								L {Bo acka} {L {L {U4 errcode} {A errtext} } ...}
 retn : S_OK or S_FALSE
 note : Notify equipment that a control job command is requested.
		By implementing this callback, client application can reject any of these commands if necessary.
		Use ICxE94CJM::RegisterCallback2 to register for the callbacks. 
*/
STDMETHODIMP CEMSvcCbk::CJCommandCallback(BSTR ctrl_job_id, E94Commands command,
										  ICxValueDisp *parm_list, ICxE94ControlJob *job_obj,
										  ICxValueDisp *status)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	E30_SVET enType	= E30_SVET::en_acka;
	if (E94_CCMD (command) == E94_CCMD::en_create)	enType	= E30_SVET::en_objack;
	if (!DispErrorData(status, enType))	return S_FALSE;

	m_pSharedData->E94_PushCommnad(E94_CEFI::en_cj_command_callback2,
								   E94_CCMD(command), LPTSTR(ctrl_job_id));

	return S_OK;
}

/*
 desc : Request for the ID of the next process job to start
		다음에 처리할 Project Job ID 반환
 parm : cmd_src		- [in]  Enumeration identifying the source of the command execution
		job_id		- [in]  Object identifier of Control Job for the command
		command		- [in]  Callback command code
		parm_list	- [in]  List of CmdParameter
		job_obj		- [in]  Interface pointer of Control Job for the command
		status		- [in]  Returned status. The format differs depending on command
							When command is cjCommandCreate:
								L {U1 objack} {L {L {U4 errcode} {A errtext} } ...};
							otherwise:
								L {Bo acka} {L {L {U4 errcode} {A errtext} } ...}
 retn : S_OK or S_FALSE
 note : Notify equipment that a control job command is requested. By implementing this callback,
		client application can reject any of these commands if necessary.
		Use ICxE94CJM::RegisterCallback3 to register for the callbacks. 
*/
STDMETHODIMP CEMSvcCbk::CJCommandCallback(E94ExecuteCommandSourceEnum cmd_src, BSTR job_id,
										  E94Commands command, ICxValueDisp *parm_list,
										  ICxE94ControlJob *job_obj, ICxValueDisp *status)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	INT32 i32NeedProcessing, i	= 0;
	LONG lHandle[2]		= {NULL};
	BOOL bErrorCreate	= FALSE, bIsUnloading[2], bIsSlotMapVerified[2], bErrorPause=FALSE;
	E30_SVET enType		= E30_SVET::en_acka;
	PTCHAR ptzID		= NULL;
	POSITION pPos		= NULL;
	STG_CLPI stPort[2]	= {NULL};
	STG_CSDI stState	= {NULL};
	VARIANT varFlag;
	CComBSTR bstrError;
	CStringArray arrCarrIDs, arrPRJobIDs;
	CAtlList<STG_CSDI> lstSubstState;

	if (E94_CCMD(command) == E94_CCMD::en_create)	enType	= E30_SVET::en_objack;
	if (!DispErrorData(status, enType))	return S_FALSE;

	/* CJCommandCallback - Create */
	if (command == cjCommandCreate)
	{
		bIsUnloading[0]	= (m_pSharedData->GetLoadPortState(1) == ENG_LPOS::en_unloading);
		bIsUnloading[1]	= (m_pSharedData->GetLoadPortState(2) == ENG_LPOS::en_unloading);
		
		m_pSharedData->GetLoadPortDataAll(1, stPort[0]);
		m_pSharedData->GetLoadPortDataAll(2, stPort[1]);
		bIsSlotMapVerified[0] = (E87_SMVS(stPort[0].slot_map_status) == E87_SMVS::en_sm_verification_ok);
		bIsSlotMapVerified[1] = (E87_SMVS(stPort[1].slot_map_status) == E87_SMVS::en_sm_verification_ok);

		m_pSharedData->GetCRJobToSubIDs(LPTSTR(_bstr_t(job_id)), 1, arrCarrIDs);
		/* 1. 로드포트가 현재 언로딩이고 슬랏맵 상태가 베리파이트 오케이 경우에만.	*/
		/* 2. 현재 로드포트의 캐리어 아이디 가져온다.								*/
		/* 3. 프로세스 잡의 머트리얼 네임 리스트에서 캐리어 리스트를 가져온다.		*/
		/* 4. 캐리어 리스트에 캐리어가 있으면 에러 처리한다.						*/		
		if (bIsUnloading[0] && bIsSlotMapVerified[0])
		{
			for (i=0; i<arrCarrIDs.GetCount(); i++)
			{
				if (0 == wcscmp(arrCarrIDs[i].GetBuffer(), L"CARRIER1"))
				{
					bErrorCreate	= TRUE;
					break;
				}
			}
		}
		if (bIsUnloading[1] && bIsSlotMapVerified[1])
		{
			for (i=0; i<arrCarrIDs.GetCount(); i++)
			{
				if (0 == wcscmp(arrCarrIDs[i].GetBuffer(), L"CARRIER2"))
				{
					bErrorCreate	= TRUE;
					break;
				}
			}
		}
		if (bErrorCreate)
		{
			bstrError.Empty();
			swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"Material for the ControlJob <%s> is being removed",
					   LPTSTR(_bstr_t(job_id)));
			bstrError.Append(tzMesg);

			status->AppendValueU1(0, 1);
			status->AppendList(0, &lHandle[0]);
			status->AppendList(lHandle[0], &lHandle[1]);
			status->AppendValueU4(lHandle[1], 17);
			status->AppendValueAscii(lHandle[1], bstrError);

			return S_FALSE;
		}
		else
		{
			if (!CreateCJobID(job_id, job_obj))	return S_FALSE;
		}
	}

	/* CJCommandCallback - Check Pause */
	if (command == cjCommandPause)
	{
		/* Control Job에 해당하는 모든 Substrates를 가져와서, Process */
		/* States가 NeedProcessing이 하나라도 있는 경우 에러 리턴한다 */

		m_pSharedData->GetCRJobToSubIDs(LPTSTR(_bstr_t(job_id)), 0, arrPRJobIDs);
		for (i=0,i32NeedProcessing=0; i<arrPRJobIDs.GetCount() && i32NeedProcessing < 1; i++)
		{
			m_pSharedData->GetSubstIDStateAll(arrPRJobIDs[i].GetBuffer(), lstSubstState);
			if (lstSubstState.GetCount() > 0)
			{
				pPos = lstSubstState.GetHeadPosition();
				while (pPos)
				{
					stState = lstSubstState.GetNext(pPos);
					if (E90_SSPS(stState.proc_state) == E90_SSPS::en_needs_processing)
					{
						i32NeedProcessing++;
						break;
					}
				}
			}
		}

		/* get all sub list from the pj */
		if (i32NeedProcessing == 0)
		{
			swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
					   L"ControlJobID %s was rejected. All material has been processed or currently being processed",
					   LPTSTR(_bstr_t(job_id)));
			bstrError.Append(tzMesg);

			varFlag.boolVal = VARIANT_FALSE;
			varFlag.pboolVal = VARIANT_FALSE;
			status->AddValueBoolean(0, varFlag);
			status->AppendList(0, &lHandle[0]);
			status->AppendList(lHandle[0], &lHandle[1]);
			status->AppendValueU4(lHandle[1], 20);
			status->AppendValueAscii(lHandle[1], bstrError);

			return S_FALSE;
		}
	}

	m_pSharedData->E94_PushCommnad(E94_CEFI::en_cj_command_callback3, E94_PECS(cmd_src),
								   E94_CCMD(command), LPTSTR(job_id));

	return S_OK;
}

/*
desc : ControlJob의 Attribute가 변경되었을때 들어옴.
parm :
retn : S_OK or S_FALSE
참고 : This callback method notifies the application when a ControlJob attribute value changes.
*/
STDMETHODIMP CEMSvcCbk::AttrChangeNotify(ICxE94ControlJob *object, BSTR attr_name, ICxValueDisp *attr_value)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!object)	return S_FALSE;

	CComBSTR bstrCRJobID = L"NONE";
	m_pSharedData->E94_PushCommnad(E94_CEFI::en_attr_change_noti, E94_PECS::en_unknown,
								   E94_CCMD(cjCommandNone), LPTSTR(bstrCRJobID));

	return S_OK;
}

/* --------------------------------------------------------------------------------------------- */
/*                            ICxE94ControlJobProcessJobCB Methods                               */
/* --------------------------------------------------------------------------------------------- */

