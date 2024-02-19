
/*
 desc : List of CmdParameter for E40PJMLib::ICxE40PJMCallback::PRCommandCallback
*/

#include "pch.h"
#include "E40CmdParam.h"

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
CE40CmdParam::CE40CmdParam()
{
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CE40CmdParam::~CE40CmdParam()
{
	RemoveEventAll();
}

/*
 desc : 모든 데이터 제거
 parm : None
 retn : None
*/
VOID CE40CmdParam::RemoveEventAll()
{
	m_arrParamName.RemoveAll();
	m_arrValString.RemoveAll();

	m_lstValType.RemoveAll();
	m_lstValIndex.RemoveAll();
	m_lstValI64.RemoveAll();
	m_lstValU64.RemoveAll();
	m_lstValDBL.RemoveAll();
}

/*
 desc : Parameter Name과 Value가 저장된 인덱스 값 저장
 parm : cp_name	- [in]  Command Parameter Name
		index	- [in]  Value 값이 저장된 위치 저장 (Zero-based)
 retn : TRUE or FALSE
*/
BOOL CE40CmdParam::AppendData(PTCHAR cp_name, UINT16 index)
{
	/* 값이 유효한지 확인 */
	if (!cp_name)				return FALSE;
	if (wcslen(cp_name) < 1)	return FALSE;

	/* Add the name and index of command parameter */
	m_arrParamName.Add(cp_name);
	m_lstValIndex.AddTail(index);

	return TRUE;
}

/*
 desc : Parameter Name & Value 등록
 parm : cp_name	- [in]  Command Parameter Name
		value	- [in]  Command Parameter Value (for INT8 ~ INT64)
 retn : TRUE or FALSE
*/
BOOL CE40CmdParam::AppendData(PTCHAR cp_name, INT64 value)
{
	/* Add the name and index of command parameter */
	if (!AppendData(cp_name, UINT16(m_lstValI64.GetCount())))	return FALSE;
	/* Add the value of command parameter */
	m_lstValI64.AddTail(value);
	/* Parameter Value Type */
	m_lstValType.AddTail(E30_GPVT::en_i8);
	return TRUE;
}

/*
 desc : Parameter Name & Value 등록
 parm : cp_name	- [in]  Command Parameter Name
		value	- [in]  Command Parameter Value (for UINT8 ~ UINT64)
 retn : TRUE or FALSE
*/
BOOL CE40CmdParam::AppendData(PTCHAR cp_name, UINT64 value)
{
	/* Add the name and index of command parameter */
	if (!AppendData(cp_name, UINT16(m_lstValU64.GetCount())))	return FALSE;
	/* Add the value of command parameter */
	m_lstValU64.AddTail(value);
	/* Parameter Value Type */
	m_lstValType.AddTail(E30_GPVT::en_u8);
	return TRUE;
}

/*
 desc : Parameter Name & Value 등록
 parm : cp_name	- [in]  Command Parameter Name
		value	- [in]  Command Parameter Value (for INT8 ~ INT64)
 retn : TRUE or FALSE
*/
BOOL CE40CmdParam::AppendData(PTCHAR cp_name, DOUBLE value)
{
	/* Add the name and index of command parameter */
	if (!AppendData(cp_name, UINT16(m_lstValDBL.GetCount())))	return FALSE;
	/* Add the value of command parameter */
	m_lstValDBL.AddTail(value);
	/* Parameter Value Type */
	m_lstValType.AddTail(E30_GPVT::en_f8);
	return TRUE;
}

/*
 desc : Parameter Name & Value 등록
 parm : cp_name	- [in]  Command Parameter Name
		value	- [in]  Command Parameter Value (for String)
 retn : TRUE or FALSE
*/
BOOL CE40CmdParam::AppendData(PTCHAR cp_name, PTCHAR value)
{
	/* Add the name and index of command parameter */
	if (!AppendData(cp_name, UINT16(m_arrValString.GetCount())))	return FALSE;
	/* Add the value of command parameter */
	m_arrValString.Add(value);
	/* Parameter Value Type */
	m_lstValType.AddTail(E30_GPVT::en_a);
	return TRUE;
}

/*
 desc : 임의 위치에 저장된 Parameter Name 반환 (가져오기)
 parm : index	- [in]  데이터가 저장된 위치 (Zero-based)
 retn : Parameter Name or NULL
*/
PTCHAR CE40CmdParam::GetParamName(UINT16 index)
{
	if (m_arrParamName.GetCount() >= index)	return NULL;
	return (PTCHAR)m_arrParamName[index].GetBuffer();
}

/*
 desc : 임의 위치에 저장된 Parameter Value Type 반환 (가져오기)
 parm : index	- [in]  데이터가 저장된 위치 (Zero-based)
 retn : Parameter Value Type, 실패인 경우, E30_GPVT::en_any
*/
E30_GPVT CE40CmdParam::GetValueType(UINT16 index)
{
	if (m_lstValType.GetCount() >= index)	return E30_GPVT::en_any;
	return m_lstValType.GetAt(m_lstValType.FindIndex(index));
}

/*
 desc : 임의 위치에 저장된 Parameter Value 반환 (int64)
 parm : index	- [in]  데이터가 저장된 위치 (Zero-based)
 retn : Parameter Value, If failed, then zero
*/
INT64 CE40CmdParam::GetValueI64(UINT16 index)
{
	if (m_lstValI64.GetCount() >= index)	return 0;
	return m_lstValI64.GetAt(m_lstValI64.FindIndex(index));
}

/*
 desc : 임의 위치에 저장된 Parameter Value 반환 (uint64)
 parm : index	- [in]  데이터가 저장된 위치 (Zero-based)
 retn : Parameter Value, If failed, then zero
*/
UINT64 CE40CmdParam::GetValueU64(UINT16 index)
{
	if (m_lstValU64.GetCount() >= index)	return 0;
	return m_lstValU64.GetAt(m_lstValU64.FindIndex(index));
}

/*
 desc : 임의 위치에 저장된 Parameter Value 반환 (double)
 parm : index	- [in]  데이터가 저장된 위치 (Zero-based)
 retn : Parameter Value, If failed, then zero
*/
DOUBLE CE40CmdParam::GetValueDBL(UINT16 index)
{
	if (m_lstValDBL.GetCount() >= index)	return 0;
	return m_lstValDBL.GetAt(m_lstValDBL.FindIndex(index));
}