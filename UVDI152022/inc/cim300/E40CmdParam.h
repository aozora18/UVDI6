
#pragma once

class CE40CmdParam
{
public:
	/* ������ & �ı��� */
	CE40CmdParam();
	virtual ~CE40CmdParam();
 
/* ���� ���� */
protected:

	CStringArray		m_arrParamName;	/* Param Name */

	CAtlList <E30_GPVT>	m_lstValType;	/* Value Data Format (UINT8, ..., INT32, FLOAT, ..., ) */
	CAtlList <UINT16>	m_lstValIndex;	/* Value ���� ����� �ε��� ���� ��ġ (Zero-based) */

	CStringArray		m_arrValString;	/* Value buffer for String */
	CAtlList<INT64>		m_lstValI64;	/* Value buffer for signed Integer */
	CAtlList<UINT64>	m_lstValU64;	/* Value buffer for unsigned Integer */
	CAtlList<DOUBLE>	m_lstValDBL;	/* Value buffer for float or double */

/* ���� �Լ� */
protected:

	VOID				RemoveEventAll();
	BOOL				AppendData(PTCHAR cp_name, UINT16 index);

/* ���� �Լ� */
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