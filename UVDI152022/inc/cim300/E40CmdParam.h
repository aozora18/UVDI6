
#pragma once

class CE40CmdParam
{
public:
	/* 생성자 & 파괴자 */
	CE40CmdParam();
	virtual ~CE40CmdParam();
 
/* 로컬 변수 */
protected:

	CStringArray		m_arrParamName;	/* Param Name */

	CAtlList <E30_GPVT>	m_lstValType;	/* Value Data Format (UINT8, ..., INT32, FLOAT, ..., ) */
	CAtlList <UINT16>	m_lstValIndex;	/* Value 값이 저장된 인덱스 버퍼 위치 (Zero-based) */

	CStringArray		m_arrValString;	/* Value buffer for String */
	CAtlList<INT64>		m_lstValI64;	/* Value buffer for signed Integer */
	CAtlList<UINT64>	m_lstValU64;	/* Value buffer for unsigned Integer */
	CAtlList<DOUBLE>	m_lstValDBL;	/* Value buffer for float or double */

/* 로컬 함수 */
protected:

	VOID				RemoveEventAll();
	BOOL				AppendData(PTCHAR cp_name, UINT16 index);

/* 공용 함수 */
public:

	BOOL				AppendData(PTCHAR cp_name, INT64 value);
	BOOL				AppendData(PTCHAR cp_name, UINT64 value);
	BOOL				AppendData(PTCHAR cp_name, DOUBLE value);
	BOOL				AppendData(PTCHAR cp_name, PTCHAR value);

	PTCHAR				GetParamName(UINT16 index);
	E30_GPVT			GetValueType(UINT16 index);
	INT64				GetValueI64(UINT16 index);
	UINT64				GetValueU64(UINT16 index);
	DOUBLE				GetValueDBL(UINT16 index);
};