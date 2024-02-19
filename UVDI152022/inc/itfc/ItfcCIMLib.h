/*
 desc : CIM Library �� ���� ���� ���α׷� ���� GEM300 ���� ������ �� ��Ÿ ���� ���� ���� ���̺귯��
*/

#pragma once

#include "../conf/global.h"
#include "../conf/define.h"
#include "../conf/cim300.h"

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
API_IMPORT BOOL uvCIMLib_Init(PTCHAR work_dir, UINT8 conn_cnt=1);
/*
 desc : ���̺귯�� ����
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT VOID uvCIMLib_Close();
/*
 desc : CIM300 Connect ���� (�ʱ�ȭ) ���� ����
 parm : epj_file	- [in]  EPJ ���� �̸� (��� ����)
 retn : None
*/
API_IMPORT VOID uvCIMLib_Stop();
/*
 desc : TS (Tracking System)�� ��ϵ� Equipment Module (Location) Name ��ȯ
 parm : index	- [in]  ��� ��� ���ε� �ε��� �� (1 based)
 retn : �̸� ���ڿ� (������ ��� NULL or Empty String)
*/
API_IMPORT PTCHAR uvCIMLib_GetEquipLocationName(ENG_TSSI index);
/*
 desc : ���� ����Ǿ� �ִ� ������Ʈ ������ ���Ϸ� ����
 parm : epj_file	- [in]  �����ϰ��� �ϴ� ���� �̸� (��ü ��� ����)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SaveProjetsToEPJ(PTCHAR epj_file);
/*
 desc : EMService�� ���� ������ ����
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsServiceRun();
/*
 desc : CIMConnection�� ����Ǿ� �ִ��� ����
 parm : logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsCIMConnected();
/*
 desc : CIM Lib. �ʱ�ȭ ���� ��ȯ
 parm : logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsInitedAll(BOOL logs);
/*
 desc : CIM300 Connect ���� (�ʱ�ȭ) ����
 parm : equip_id	- [in]  Equipment ID (0 or later. ȯ�� ���Ͽ� ������ ����-1)
		conn_id		- [in] 1 or Later (���� ������ ���ؼ��� 0 �� ���)
		epj_file	- [in]  EPJ ���� (��ü ���)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Start(LONG equip_id, LONG conn_id, PTCHAR epj_file);
/*
 desc : CIM300 Connection (�ʱ�ȭ �Ϸ�) ���� Finalized ȣ��
 parm : equip_id	- [in]  Equipment ID (0 or later. ȯ�� ���Ͽ� ������ ����-1)
		conn_id		- [in] 1 or Later (���� ������ ���ؼ��� 0 �� ���)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Finalized(LONG equip_id, LONG conn_id);
/*
 desc : �ý��ۿ��� �߻��� �α� ��ȯ
 parm : logs	- [in]  ��ȯ�� �α� ������ ����� ����ü ������
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_PopLogs(STG_CLLM &logs);
/*
 desc : Recipe Path ����
 parm : recipe	- [in]  Recipe Path (��ü ��� ����. �������� '/' ����)
 		conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		var_id	- [in]  Identification (Variables Name�� ����� ID)
		var_name- [in]  Identification (Variables ID�� ����� Name)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetRecipePathID(PTCHAR recipe, LONG conn_id, LONG var_id=2042);
API_IMPORT BOOL uvCIMLib_SetRecipePathName(PTCHAR recipe, LONG conn_id, PTCHAR var_name=L"RecipePath");
/*
 desc : MDLN �� ����
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		name	- [in]  Equipment model type up to 20 characters
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetEquipModelType(LONG conn_id, PTCHAR name);
/*
 desc : SOFTREV �� ����
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		name	- [in]  Equipment software revision ID up to 20 characters
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetEquipSoftRev(LONG conn_id, PTCHAR name);
/*
 desc : Recip Path ��ȯ
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		path	- [out] Recipe Names (Files)�� ����� ��ΰ� ��ȯ�� ����
		size	- [in]  'path' ���� ũ��
		var_id	- [in]  Identification (Variables Name�� ����� ID)
		var_name- [in]  Identification (Variables ID�� ����� Name)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetRecipePathID(LONG conn_id, PTCHAR path, UINT32 size, LONG var_id=2042);
API_IMPORT BOOL uvCIMLib_GetRecipePathName(LONG conn_id, PTCHAR path, UINT32 size, PTCHAR var_name=L"RecipePath");
/*
 desc : Recip Name (File)�� Ȯ���� ��ȯ
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		ext		- [out] Recipe Name (File)�� Ȯ���ڰ� ��ȯ�� ����
		size	- [in]  'ext' ���� ũ��
		var_id	- [in]  Identification (Variables Name�� ����� ID)
		var_name- [in]  Identification (Variables ID�� ����� Name)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetRecipeFileExtID(LONG conn_id, PTCHAR ext, UINT32 size, LONG var_id=2048);
API_IMPORT BOOL uvCIMLib_GetRecipFileExtName(LONG conn_id, PTCHAR ext, UINT32 size, PTCHAR var_name=L"RecipeExtension");
/*
 desc : EPJ ID�� ����� Name ��ȯ
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		name	- [in]  �˻��� ���� Variable Name
		id		- [out] Vairable ID ��ȯ ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetNameToID(LONG conn_id, PTCHAR name, LONG &id);


/* --------------------------------------------------------------------------------------------- */
/*                                         Shared Data                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ���� CIM300 (From Host)���κ��� �߻��� �̺�Ʈ ��� ����
 parm : None
 retn : None
*/
API_IMPORT VOID uvCIMLib_RemoveEventAll();
/*
 desc : CIM300Cbk�κ��� �߻��� �̺�Ʈ ��ȯ
 parm : data	- [in]  ��ȯ�� �̺�Ʈ ������ ����� ����ü ������
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_PopEvent(STG_CCED &event);
/*
 desc : Load Port�� Docked or Undocked Location Name ��ȯ
 parm : port_id	- [in]  Load Port ID (1 or 2)
		docked	- [in]  0x00 : Docked, 0x01 : Undocked
 retn : Docked or Undocked Location Name ��ȯ (������ ���, NULL)
*/
API_IMPORT PTCHAR uvCIMLib_GetLoadPortDockedName(UINT16 port_id, UINT8 docked);
/*
 desc : CIM300 �α� ���� ����
 parm : level	- [in]  Level Value ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetLogLevelAll(ENG_ILLV level);
/*
 desc : ������ ���� ��û�� ���� �� �ʱ�ȭ (������ �������� ����)
 parm : None
 retn : None
*/
API_IMPORT VOID uvCIMLib_ResetPPLoadRecipeAck();
/*
 desc : ������ ���� ��û�� ���� ���� �� ��ȯ
 parm : None
 retn : ANY Type
*/
API_IMPORT E30_HPLA uvCIMLib_GetPPLoadRecipeAck();
/*
 desc : ������ ���� ��û�� ���� ���� ���� ���ŵǾ����� ����
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsPPLoadRecipeAckOk();
/*
 desc : ������ ������ �۽� (����) ��û�� ���� ���� �� �ʱ�ȭ
 parm : None
 retn : None
*/
API_IMPORT VOID uvCIMLib_ResetPPSendRecipeAck();
/*
 desc : ������ ������ �۽� (����) ��û�� ���� ���� �� ��ȯ
 parm : None
 retn : ANY Type
*/
API_IMPORT E30_SRHA uvCIMLib_GetPPSendRecipeAck();
/*
 desc : ������ ������ �۽� (����) ��û�� ���� ���� ���� �ִ��� ����
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsPPSendRecipeAckOk();
/*
 desc : �͹̳� �޽��� �۽� (����) ��û�� ���� ���� �� �ʱ�ȭ
 parm : None
 retn : None
*/
API_IMPORT VOID uvCIMLib_ResetTermMsgAck();
/*
 desc : �͹̳� �޽��� �۽� (����) ��û�� ���� ���� �� ��ȯ
 parm : None
 retn : ANY Type
*/
API_IMPORT E30_STHA uvCIMLib_GetTermMsgAck();
/*
 desc : �͹̳� �޽��� �۽� (����) ��û�� ���� ������ �ִ��� ����
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsTermMsgAckOk();
/*
 desc : Host�κ��� Recipe File ��û (����)�� ���� ���� �ʱ�ȭ
 parm : None
 retn : None
*/
API_IMPORT VOID uvCIMLib_ResetRecipeFileAck();
/*
 desc : Host�κ��� Recipe File ��û (����)�� ���� ���� �� ��ȯ
 parm : None
 retn : ANY Type
*/
API_IMPORT E30_RMJC uvCIMLib_GetRecipeFileAck();
/*
 desc : ������ (���μ��� ���α׷�; Process Program == Recipe) ��� / ���� / ��ü ����
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		file	- [in]  ������ �̸� (��ü ��ΰ� ���Ե� ����)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_RecipeAppend(LONG conn_id, PTCHAR file);
API_IMPORT BOOL uvCIMLib_RecipeRemove(LONG conn_id, PTCHAR file);
API_IMPORT VOID uvCIMLib_RecipeRemoveAll();
/*
 desc : ���� ��ϵ� Process Job ID ���� Substrate IDs�� ���°� ��� �Էµ� ���¿� �������� ����
 parm : pj_id	- [in]  �˻� ����� Process Job ID
		machine	- [in]  Substate Machine Type ��
		state	- [in]  �� ����� Substrate Processing State ��
 retn : �Էµ� state ���� ��� �����ϴٸ� TRUE, �� �� �̻� �ٸ� ���� �����Ѵٸ� FALSE
 */
API_IMPORT BOOL uvCIMLib_IsCheckedAllPRJobSubstState(PTCHAR pj_id, E90_VSMC machine, INT32 state);
/*
 desc : ���� ��ϵ� Process Job ID ���� Substrate IDs�� ���°� �Էµ� ���¿� ������ ���� 1���� �ִ��� ����
 parm : pj_id	- [in]  �˻� ����� Process Job ID
		machine	- [in]  Substate Machine Type ��
		state	- [in]  �� ����� Substrate Processing State ��
 retn : �Էµ� state ���� ������ ���� ���� �� ���� �����ϸ� TRUE
		�Էµ� state ���� ������ ���� ���� �� ���� ���ٸ� FALSE
 */
API_IMPORT BOOL uvCIMLib_IsCheckedOnePRJobSubstState(PTCHAR pj_id, E90_VSMC machine, INT32 state);
/*
 desc : ���� ��ġ�� ��ϵ� PRJob IDs�� ��ϵ� Substrate IDs�� ��� ���� ������ ��ȯ
 parm : pj_id	- [in]  �˻� ����� Process Job ID
		subst	- [out] Substrate ID�� ���� �� ��Ÿ �������� ����� ����Ʈ �޸� (PRCommandCbk �Լ����� �ַ� ����?)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetSubstIDStateAll(PTCHAR pj_id, CAtlList <STG_CSDI> &lst_state);
/*
 desc : CJobID ����
 parm : carr_id	- [in]  ���� ����� Carrier ID
 retn : TRUE (���� ����), FALSE (���� ����)
*/
API_IMPORT BOOL uvCIMLib_RemoveSharedCarrID(PTCHAR carr_id);
/*
 desc : ���� ��ϵ� SECS-II Message ��ȯ
 parm : mesg_id	- [in]  �˻� ����� Message ID �� (Stream/Function���� ������ 16���� Hexa-Decimal ��)
		remove	- [in]  �˻��� ����� ��ȯ �� �޸𸮿��� �������� ����
		item	- [out] �˻��� �����Ͱ� ��ȯ�� ����Ʈ ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetSecsMessage(LONG mesg_id, BOOL remove, CAtlList <STG_TSMV> &item);
/* 
 desc : ���� ��ϵ� ��� SECS-II Message �޸� ����
 parm : None
 retn : None
*/
API_IMPORT VOID uvCIMLib_RemoveAllSecsMessage();


#if 0	/* ���� �� �ʿ� ����. ���߿� �Ǵ��� */
/* --------------------------------------------------------------------------------------------- */
/*                         Operation Job Step [Sequencial Working]                               */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ���� �۾��� �Ϸ�� �������� ����
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsWorkCompleted();
/*
 desc : ���� �۾��� ���е� �������� ����
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsWorkFailed();
/*
 desc : ���� �۾��� ��� �������� ����
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsIdledWork();
/*
 desc : E30GEM �ʱ�ȭ ����
 parm : None
 retn : TRUE or FALSE
 note : EPJ �ε� ����, ȣ��Ǿ�� ��
*/
API_IMPORT BOOL uvCIMLib_StartInitE30GEM();
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
API_IMPORT BOOL uvCIMLib_SimulateProcessing(PTCHAR pj_id);
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
API_IMPORT BOOL uvCIMLib_GetNameValue(LONG conn_id, PTCHAR var_name, UNG_CVVT &value,
									  LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetIDValue(LONG conn_id, LONG var_id, UNG_CVVT &value,
									LONG handle=0, LONG index=0);
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
API_IMPORT VOID uvCIMLib_GetValueToStr(LONG conn_id, LONG var_id, PTCHAR var_name, PTCHAR value,
									   UINT32 size, LONG handle=0, LONG index=0);
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
API_IMPORT BOOL uvCIMLib_GetNameValueI1(LONG conn_id, LONG var_id, INT8 &value, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetNameValueI2(LONG conn_id, LONG var_id, INT16 &value, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetNameValueI4(LONG conn_id, LONG var_id, INT32 &value, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetNameValueI8(LONG conn_id, LONG var_id, INT64 &value, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetNameValueU1(LONG conn_id, LONG var_id, UINT8 &value, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetNameValueU2(LONG conn_id, LONG var_id, UINT16 &value, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetNameValueU4(LONG conn_id, LONG var_id, UINT32 &value, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetNameValueU8(LONG conn_id, LONG var_id, UINT64 &value, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetNameValueF4(LONG conn_id, LONG var_id, FLOAT &value, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetNameValueF8(LONG conn_id, LONG var_id, DOUBLE &value, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetNameValueBo(LONG conn_id, LONG var_id, BOOL &value, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetIDValueI1(LONG conn_id, LONG var_id, INT8 &value, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetIDValueI2(LONG conn_id, LONG var_id, INT16 &value, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetIDValueI4(LONG conn_id, LONG var_id, INT32 &value, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetIDValueI8(LONG conn_id, LONG var_id, INT64 &value, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetIDValueU1(LONG conn_id, LONG var_id, UINT8 &value, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetIDValueU2(LONG conn_id, LONG var_id, UINT16 &value, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetIDValueU4(LONG conn_id, LONG var_id, UINT32 &value, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetIDValueU8(LONG conn_id, LONG var_id, UINT64 &value, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetIDValueF4(LONG conn_id, LONG var_id, FLOAT &value, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetIDValueF8(LONG conn_id, LONG var_id, DOUBLE &value, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetIDValueBo(LONG conn_id, LONG var_id, BOOL &value, LONG handle=0, LONG index=0);
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
API_IMPORT BOOL uvCIMLib_GetAsciiVariable(LONG conn_id, LONG var_id, PTCHAR value, UINT32 size, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetAsciiEvent(LONG conn_id, LONG var_id, PTCHAR value, UINT32 size, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetAsciiAlarm(LONG conn_id, LONG var_id, PTCHAR value, UINT32 size, LONG handle=0, LONG index=0);
/*
 desc : CIMConnect ������ ĳ�� ���ۿ� �� ����
 parm : conn_id	- [in] 1 or Later (���� ������ ���ؼ��� 0 �� ���)
		type	- [in]  �ԷµǴ� �� (Value)�� Type
		var_name- [in]  ���� �̸�
		value	- [in]  �� (type�� ����, �������� ������ ��)
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID; 1 or 2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetCacheValueID(LONG conn_id, E30_GPVT type, LONG var_id, UNG_CVVT value, LONG handle=0);
API_IMPORT BOOL uvCIMLib_SetCacheValueName(LONG conn_id, E30_GPVT type, PTCHAR var_name, UNG_CVVT value, LONG handle=0);
API_IMPORT BOOL uvCIMLib_SetCacheStrValueID(LONG conn_id, E30_GPVT type, LONG var_id, PTCHAR value, LONG handle=0);
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
API_IMPORT BOOL uvCIMLib_SetCacheAsciiID(LONG conn_id, LONG var_id, PTCHAR value, LONG handle, LONG index);
API_IMPORT BOOL uvCIMLib_SetCacheAsciiName(LONG conn_id, PTCHAR var_name, PTCHAR value, LONG handle, LONG index);
/*
 desc : TS (Tracking System)�� ��ϵ� Processing Name ��ȯ
 parm : index	- [in]  ��� ��� ���ε� �ε��� �� (!!! -1 based !!!)
 retn : �̸� ���ڿ� (������ ��� NULL or Empty String)
*/
API_IMPORT PTCHAR uvCIMLib_GetEquipProcessingName(E90_SSPS index);
/*
 desc : �α� ���� (���Ͽ� �α� ������ ���� ������ ����)
 parm : enable	- [out] �α� ���� ���� ���� �� ��ȯ
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsLogging(BOOL &enable);
/*
 desc : �α� ���� ���� ����
 parm : enable	- [in]  �α� ���� ���� ���� ���� �÷���
						enable=TRUE -> Start, enable=FALSE -> Stop
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetLogEnable(BOOL enable);
/*
 desc : ���� ��� (�߻�)�� ��� ID ���� ��ȯ
 parm : id		- [out] ����Ʈ�� ����� CAtlList <LONG> : ID
		name	- [out] ����Ʈ�� ����� String Array    : Name
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetCommandList(CAtlList <LONG> &id, CStringArray &name);
/*
 desc : ���� ��� (�߻�)�� EPJ ���ϵ� ��ȯ
 parm : files	- [out] ����Ʈ�� ����� String Array    : EPJ Files (Included Path)
		sizes	- [out] ����Ʈ�� ����� CAtlList <LONG> : EPJ File Size
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetEPJList(CStringArray &files, CAtlList <LONG> &sizes);
/*
 desc : ���� ��� (�߻�)�� Recipe Names ��ȯ
 parm : names	- [out] ����Ʈ�� ����� String Array    : Recipe Names
		sizes	- [out] ����Ʈ�� ����� CAtlList <LONG> : Recipe Sizes
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetRecipes(CStringArray &names, CAtlList <LONG> &sizes);
/*
 desc : Host �ʿ� Process Program (Recipe)�� �����ϰڴٰ� ��û
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		name	- [in]  Recipe (Process Program) Name (Ȯ���ڴ� �� �������� �ʾƵ� ��)
		size	- [in]  Recipe (Process Program) Size (unit: bytes)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_QueryRecipeUpload(LONG conn_id, PTCHAR name, UINT32 size);
/*
 desc : Host �ʿ� Process Program (Recipe)�� ��û
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		name	- [in]  Recipe (Process Program) Name (Ȯ���ڴ� �� �������� �ʾƵ� ��)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_QueryRecipeRequest(LONG conn_id, PTCHAR name);
/*
 desc : �ý��ۿ� ���ǵ� Report ��� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		ids		- [out] ����Ʈ�� ����� CAtlList <LONG> : Report Names
		names	- [out] ����Ʈ�� ����� String Array    : Report IDs
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetReports(LONG conn_id, CAtlList <LONG> &ids, CStringArray &names);
/*
 desc : Report �� ���õ� Event�� ����Ʈ ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		r_ids	- [out] ����Ʈ�� ����� CAtlList <LONG> : Report IDs
		e_ids	- [out] ����Ʈ�� ����� CAtlList <LONG> : Event IDs
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetReportsEvents(LONG conn_id, CAtlList <LONG> &r_ids, CAtlList <LONG> &e_ids);
/*
 desc : �߻��� Traces�� ����Ʈ ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		ids		- [out] ����Ʈ�� ����� CAtlList <LONG> : Report IDs
		names	- [out] ����Ʈ�� ����� String Array    : Trance Names
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetListTraces(LONG conn_id, CAtlList <LONG> &ids, CStringArray &names);
/*
 desc : EPJ ���Ͽ� ��ϵ� variables ��� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		type	- [in]  Variable Type (E30_GVTC)
		ids		- [out] Variable IDs ��ȯ
		names	- [out] Variable Names ��ȯ
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetListVariables(LONG conn_id, E30_GVTC type, CAtlList <LONG> &ids,
										  CStringArray &names);
/*
 desc : List all messages (SECS-II ��� �޽��� ��ȯ)
		�� �޼���� ��� �������̽����� ���� ó�� ���� �޽����� �޽��� ID ����� ��ȯ�մϴ�.
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		ids		- [out] message IDs ��ȯ
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetListMessageIDs(LONG conn_id, CAtlList <LONG> &ids);
/*
 desc : Variable ID�� �ش�Ǵ� Value Type (E30_GVTC) �� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		var_id	- [in]  Variable ID�� ���εǴ� Name
		var_name- [in]  Variable Name
		type	- [out] Value Type
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetValueTypeID(LONG conn_id, LONG var_id, E30_GPVT &type);
API_IMPORT BOOL uvCIMLib_GetValueTypeName(LONG conn_id, PTCHAR var_name, E30_GPVT &type);
/*
 desc : Variable ID�� �ش�Ǵ� Variable Type (E30_GVTC) �� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		var_id	- [in]  Variable ID�� ���εǴ� Name
		var_name- [in]  Variable Name
		type	- [out] Variable Type
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetVarTypeID(LONG conn_id, LONG var_id, E30_GVTC &type);
API_IMPORT BOOL uvCIMLib_GetVarTypeName(LONG conn_id, PTCHAR var_name, E30_GVTC &type);
/*
 desc : ����� ��� ���� ���� ���
 parm : conn_id		- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		comm_pgid	- [out] The COM ProgID of the communications object
		size_pgid	- [in]  'comm_pgid'�� ����� ���ڿ� ����
		comm_sets	- [out] The communications settings
		size_sets	- [in]  'comm_sets'�� ����� ���ڿ� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetCommSetupInfo(LONG conn_id, PTCHAR comm_pgid, UINT32 size_pgid,
										  PTCHAR comm_sets, UINT32 size_sets);
/*
 desc : ����� ��� ����
 parm : conn_id		- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		comm_pgid	- [in]  The COM ProgID of the communications object
		comm_sets	- [in]  The communications settings
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetCommSetupInfo(LONG conn_id, PTCHAR comm_pgid, PTCHAR comm_sets);
/*
 desc : �α� ���� ���� ȯ�� ���� ���� ���� ������Ʈ���� �������� ���� ����
 parm : enable	- [in]  TRUE : ����, FALSE : ���� ����
		logs	- [in]  �α� ���� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetLoggingConfigSave(BOOL enable);
/*
 desc : �α� ���� ���� ȯ�� ���� ���� ���� ���� �� ��ȯ
 parm : enable	- [out] TRUE : �����ϰ� ����, FALSE : ���� ���ϰ� ����
		logs	- [in]  �α� ���� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetLoggingConfigSave(BOOL &enable);
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
API_IMPORT BOOL uvCIMLib_SendPrimaryMessage(LONG conn_id, UINT8 msg_sid, UINT8 msg_fid,
											PTCHAR str_msg=NULL, UINT8 term_id=0, BOOL reply=FALSE);
/*
 desc : Application Name ��ȯ
 parm : name	- [out] Application Name�� ����� ����
		size	- [in]  'name' ���� ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetAppName(PTCHAR name, UINT32 size);
/*
 desc : Application Name ����
 parm : name	- [in]  Application Name�� ����� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetAppName(PTCHAR name);
/*
 desc : ���� ������Ʈ�� ��Ű���� ����� ������ ��¥ (�ð� ����) ��ȯ
 parm : date_time	- [out] ����� �ð� (��¥����)�� ����� ����
		size		- [in]  'data_time' ���� ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetBuildDateTime(PTCHAR date_time, UINT32 size);
/*
 desc : ���� ������Ʈ�� ��Ű���� ����� ������ Version ��ȯ
 parm : version	- [out] ����� �ð� (��¥����)�� ����� ����
		size	- [in]  'version' ���� ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetBuildVersionStr(PTCHAR version, UINT32 size);
/*
 desc : Get Package verion (for number)
 parm : major	- [out] Returned Major version revision number.
		minor	- [out] Returned Major version revision number.
		patch	- [out] Returned Patch number.
		build	- [out] Returned Build number.
 retn : TRUE or FALSE
 note : returns package version information
*/
API_IMPORT BOOL uvCIMLib_GetVersion(LONG &major, LONG &minor, LONG &patch, LONG &build);
/*
 desc : ���� (Ư��) ��ġ�� ����Ǿ� �ִ� ���� ���α׷��� IDs�� �̸����� ��ȯ
 parm : equip_id- [in]  ��� ID (0, 1 or Later)
		ids		- [out] ���� ���α׷� IDs ��ȯ (ID ���� ����Ʈ�� ��ȯ)
		names	- [out] ���� ���α׷� Names ��ȯ (���ڿ� �迭�� ��ȯ)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetListApplications(LONG equip_id, CAtlList <LONG> &ids, CStringArray &names);
/*
 desc : ���� (Ư��) ��ġ�� ����Ǿ� �ִ� Host�� IDs�� �̸����� ��ȯ
 parm : conn_id	- [in]  ��� ID (0, 1 or Later)
		ids		- [out] Host IDs ��ȯ (ID ���� ����Ʈ�� ��ȯ)
		names	- [out] Host Names ��ȯ (���ڿ� �迭�� ��ȯ)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetListConnections(LONG conn_id, CAtlList <LONG> &ids, CStringArray &names);
/*
 desc : EM Service�� ����Ǿ� �ִ� Equipment�� IDs�� �̸����� ��ȯ
 parm : equip_id- [in]  ��� ID (0, 1 or Later)
		ids		- [out] Equipment IDs ��ȯ (ID ���� ����Ʈ�� ��ȯ)
		names	- [out] Equipment Names ��ȯ (���ڿ� �迭�� ��ȯ)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetListEquipments(CAtlList <LONG> &ids, CStringArray &names);
/*
 desc : ���� Control�� Remote ������� ����
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsControlRemote(LONG conn_id);
/*
 desc : ���� Control�� Online ������� ����
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsControlOnline(LONG conn_id);
/*
 desc : ���� Communication��尡 Enable���� ����
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsCommEnabled(LONG conn_id);
/*
 desc : CIMConnect ���ο� �α� ���� ����
 parm : value	- [out] TRUE : Saved, FALSE : No save
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetLogToCIMConnect(BOOL &value);
/*
 desc : CIMConnect ���ο� �α� ���� ���� ����
 parm : value	- [in]  TRUE : Saved, FALSE : No save
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetLogToCIMConnect(BOOL value);
/*
 desc : SECS-II Message ��� ����
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_ResetSecsMesgList();
/*
 desc : SECS-II Message ��� - Root Message
 parm : type	- [in]  Message Type
		value	- [in]  Message Type�� ���� ��
		size	- [in]  'value' ���� ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_AddSecsMesgRoot(E30_GPVT type, PVOID value, UINT32 size);
/*
 desc : SECS-II Message ��� - Root Message
 parm : handle	- [in]  List�� �߰��� ���� Parent ID
						�� ó�� �߰��� List�� ���, 0 ��
		clear	- [in]  Message ���� ��� ���� ���� (�����̸� TRUE, �߰��̸� FALSE)
 retn : ���� �߰��� Handle �� ��ȯ (���� ���̸�, ���� ó��)
*/
API_IMPORT LONG uvCIMLib_AddSecsMesgList(LONG handle, BOOL clear=FALSE);
/*
 desc : ���� ��ġ (Host?)���� �۽ŵ� SECS-II Message ���
 parm : handle	- [in]  Handle to a list within the object, or 0
		type	- [in]  'value' ���� ����� ���� ����
		value	- [in]  The value to append
		size	- [in]  'value' ���ۿ� ����� �������� ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_AddSecsMesgValue(LONG handle, E30_GPVT type, PVOID value, UINT32 size);
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
API_IMPORT BOOL uvCIMLib_SendSecsMessage(UINT8 conn_id, UINT8 mesg_sid, UINT8 mesg_fid,
										 BOOL is_reply, LONG &trans_id, BOOL is_body);


/* --------------------------------------------------------------------------------------------- */
/*                               ICxE30 Communication / Control                                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Communication Enable or Disable
 parm : conn_id	- [in] 1 or Later (���� ������ ���ؼ��� 0 �� ���)
		enable	- [in]  TRUE or FALSE
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetCommunication(LONG conn_id, BOOL enable);
/*
 desc : Change the GEM Control State to REMOTE or LOCAL
 parm : conn_id	- [in] 1 or Later (���� ������ ���ؼ��� 0 �� ���)
		state	- [in]  if true, go REMOTE. if false, go LOCAL
 retn : None
*/
API_IMPORT BOOL uvCIMLib_SetControlRemote(LONG conn_id, BOOL state);
/*
 desc : Change the GEM Control state to ONLINE or OFFLINE
 parm : conn_id	- [in] 1 or Later (���� ������ ���ؼ��� 0 �� ���)
		state	- [in]  if true, go ONLINE. if false, go OFFLINE
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetControlOnline(LONG conn_id, BOOL state);
/*
 desc : Spooling Overwrite ����
 parm : conn_id		- [in]  ����� ��� ID
		overwrite	- [in]  Overwrite (TRUE) or Not overwrite (FALSE)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetSpoolOverwrite(LONG conn_id, BOOL overwrite);
/*
 desc : Spooling State ����
 parm : conn_id	- [in]  ����� ��� ID
		enable	- [in]  Enable (TRUE) or Disable (FALSE)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetSpoolState(LONG conn_id, BOOL enable);
/*
 desc : Spooling State ����
 parm : conn_id	- [in]  ����� ��� ID
		enable	- [in]  Enable (TRUE) or Disable (FALSE)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetSpoolPurge(LONG conn_id);
/*
 desc : Enables or Disables alarm reporting for a host for the specified alarms.
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		alarm_id- [in]  Connection Variables or Alarms�� Section�� ������ variables identification number
		state	- [in]  TRUE (ENABLE) or FALSE (DISABLE)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetAlarmsEnabled(LONG conn_id, LONG alarm_id, BOOL state);
/*
 desc : ���� CIMConnect ���ο� �߻��� �˶� ��� ����
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_AlarmClearAll();
/*
 desc : ���� CIMConnect ���ο� �߻��� �˶� ��� ����
 parm : var_id	- [in]  Alarm ID ��, EPJ Variables ID�� ���� ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_AlarmClearID(LONG var_id);
API_IMPORT BOOL uvCIMLib_AlarmClearName(PTCHAR var_name);
/*
 desc : ���� CIMConnect ���ο� �߻��� �˶� ��� ����
 parm : name	- [in]  Alarm Name
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_AlarmClearName(PTCHAR name);
/*
 desc : ���� ��� (�߻�)�� �˶� ID ���� ��ȯ
 parm : id		- [out] �˶� ����Ʈ�� ����� CAtlList <LONG> : �˶� ID
		state	- [out] �˶� ����Ʈ�� ����� CAtlList <LONG>: �˶� ���� (0 or 1)
		alarm	- [in]  �˶��� �߻��� ������ ��ȯ ���� (FLALSE:  ���, TRUE: �˶��� �߻��� �͸�)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetAlarmList(CAtlList <LONG> &id, CAtlList <LONG> &state, BOOL alarm);
/*
 desc : ���� �˶� ���� �� ��ȯ
 parm : id		- [in]  �˶� ID
		state	- [out] �˶� ���� ��ȯ (1 : Set, 0 : Clear)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetAlarmState(LONG id, LONG &state);
/*
 desc : CIMConnect ���η� ������ �˶� �߻�
 parm : var_id	- [in]  Alarm ID ��, EPJ Variables ID�� ���� ��
		var_name- [in]  Alarm name
		text	- [in]  Text about the alarm. If empty, the default text for this alarm is passed to the host.
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_AlarmSetID(LONG var_id);
API_IMPORT BOOL uvCIMLib_AlarmSetName(PTCHAR var_name, PTCHAR text);
/*
 desc : Change an alarm to the SET (detected) state.
 retn : name	- [in]  Alarm name
		text	- [in]  Text about the alarm. If empty, the default text for this alarm is passed to the host.
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_AlarmSetName(PTCHAR name, PTCHAR text);
/*
 desc : Event �߻� (Method to trigger an event)
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		evt_id	- [in]  Collection Event ID
		evt_name- [in]  Collection Event Name
 retn : TRUE or FALSE
 note : This method triggers an equipment event
*/
API_IMPORT BOOL uvCIMLib_SetTrigEventID(LONG conn_id, LONG evt_id);
API_IMPORT BOOL uvCIMLib_SetTrigEventName(LONG conn_id, PTCHAR evt_name);
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
API_IMPORT BOOL uvCIMLib_SetTrigEventValueID(LONG conn_id, LONG evt_id, LONG var_id,
											 LNG_CVVT value, E30_GPVT type);
API_IMPORT BOOL uvCIMLib_SetTrigEventValueName(LONG conn_id, PTCHAR evt_name, PTCHAR var_name,
											   LNG_CVVT value, E30_GPVT type);
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
API_IMPORT BOOL uvCIMLib_SetTrigEventValuesID(LONG conn_id, LONG evt_id,
											  CAtlList<LONG> &var_id, CAtlList<UNG_CVVT> &value,
											  CAtlList<E30_GPVT> &type);
API_IMPORT BOOL uvCIMLib_SetTrigEventValuesName(LONG conn_id, PTCHAR evt_name,
												CStringArray &var_name, CAtlList<UNG_CVVT> &value,
												CAtlList<E30_GPVT> &type);
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
API_IMPORT BOOL uvCIMLib_SetTrigEventListID(LONG conn_id, LONG evt_id, LONG var_id,
											CAtlList<UNG_CVVT> &value, CAtlList<E30_GPVT> &type);
API_IMPORT BOOL uvCIMLib_SetTrigEventListName(LONG conn_id, PTCHAR evt_name, PTCHAR var_name,
											  CAtlList<UNG_CVVT> &value, CAtlList<E30_GPVT> &type);
/*
 desc : Update the GEM Processing State in CIMConnect. 
		Each equipment supplier should customzie the GEM Processing State Model
		to match the actual tool's processing states. This is just a  
		simple example with three states: IDLE, SETUP and EXECUTING. 
 parm : state	- [in]  ���� ���� (���� ���� : 0 (Setup), 1 (Idle), 2 (executing))
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetUpdateProcessState(E30_GPSV state);
/*
 desc : Send a terminal service acknowledge event to the host. Send to host 1 by default.
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
 retn : TRUE or FALSE
 ���� : This method triggers an equipment well-known event
		Equipment events are momentary, not latched
		There is no method to reset events since they are a one-shot, non-latched event.
*/
API_IMPORT BOOL uvCIMLib_SendTerminalAcknowledge(LONG conn_id);
/*
 desc : Send a terminal service text message to the host.
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		term_id	- [in]  Terminal ID (0 or Later)
		mesg	- [in]  The text message
 retn : TRUE or FALSE
 ���� : ���� : This method requests that a terminal message to be sent to a connection
*/
API_IMPORT BOOL uvCIMLib_SendTerminalMessage(LONG conn_id, LONG term_id, PTCHAR mesg);
/*
 desc : Notify CIMConnect that an operator command has been issued.
 parm : conn_id	- [in]  Connection #
		name	- [in]  Name of the command
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_OperatorCommand(LONG conn_id, PTCHAR name, LONG handle);
/*
 desc : ��ġ�� ���ǵ� ��� ������ �׸���� ���� (�����ؼ� ����ؾ� ��)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetClearProject();
/*
 desc : ��� ���� �����ϴ� �˶� ID ����
 parm : alarm_id- [in]  �����ϰ��� �ϴ� �˶� ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetDeleteAlarm(LONG alarm_id);
/*
 desc : ��� ��û���� ������ (���?)�� ����
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetDeleteAllRequests();
/*
 desc : �˶� �̸��� �ش�Ǵ� �˶� ID �� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		name	- [in]  �˶� �̸�
		id		- [out] �˶� ID ��ȯ
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetAlarmID(LONG conn_id, PTCHAR name, LONG &id);
/*
 desc : �˶� ID�� ���� ���� ��ȯ
 parm : alarm_id- [in]  �˶� ID
		desc	- [out] �˶� ������ ��ȯ�� ����
		size	- [in]  'desc' ���� ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetAlarmIDToDesc(LONG alarm_id, PTCHAR desc, UINT32 size);
/*
 desc : �˶� ID�� ���� ��Ī ��ȯ
 parm : alarm_id- [in]  �˶� ID
		name	- [out] �˶� ��Ī�� ��ȯ�� ����
		size	- [in]  'name' ���� ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetAlarmIDToName(LONG alarm_id, PTCHAR name, UINT32 size);
/*
 desc : ��� ���� (Low Level Communicating state) �� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		state	- [out] ��� ����
		substate- [out] HSMS (Ethernet) ���� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetCommState(LONG conn_id, E30_GCSS &state, E30_GCSH &substate);
/*
 desc : Host ���ῡ ���� �̸� ��, connection id�� ���εǴ� �̸� ���
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		name	- [out] ����� Host Name �� ���
		size	- [in]  'name' ���� ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetConnectionName(LONG conn_id, PTCHAR name, UINT32 size);
/*
 desc : ���� ����� Project Name ���
 parm : name	- [out] Project Name �� ���
		size	- [in]  'name' ���� ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetCurrentProjectName(PTCHAR name, UINT32 size);
/*
 desc : �⺻ ������ Default Project Name ���
 parm : name	- [out] Project Name �� ���
		size	- [in]  'name' ���� ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetDefaultProjectName(PTCHAR name, UINT32 size);
/*
 desc : ���� ����� ��� ID �� ��ȯ
 parm : equip_id- [out] ����(����)�Ǵ� ��� ID �� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetHostToEquipID(LONG &equip_id);
/*
 desc : Event Name�� �ش�Ǵ� Event ID ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		name	- [in]  Event Name
		id		- [out] Event ID�� ��ȯ�Ǿ� �����
��ȯ : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetEventNameToID(LONG conn_id, PTCHAR name, LONG &id);
/*
 desc : Variable Name�� �ش�Ǵ� Variable ID ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		name	- [in]  Variable Name
		var_id	- [out] Variable ID�� ��ȯ�Ǿ� �����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetVarNameToID(LONG conn_id, PTCHAR name, LONG &var_id);
/*
 desc : Variable ID�� �ش�Ǵ� Variable Name ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		var_id	- [in]	Variable ID
		name	- [out] Variable Name�� ��ȯ�Ǵ� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetVarIDToName(LONG conn_id, LONG var_id, PTCHAR name, UINT32 size);
/*
 desc : Variable ID�� �ش�Ǵ� Variable Description ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		var_id	- [in]	Variable ID
		desc	- [out] Variable Description�� ��ȯ�Ǵ� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetVarIDToDesc(LONG conn_id, LONG var_id, PTCHAR desc, UINT32 size);
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
API_IMPORT BOOL uvCIMLib_GetVarMinMax(LONG conn_id, LONG var_id, E30_GPVT &type, PVOID min, PVOID max);
API_IMPORT BOOL uvCIMLib_GetVarMinMaxToStr(LONG conn_id, LONG var_id, PTCHAR min, PTCHAR max, UINT32 size);
/*
 desc : ���� ����Ǿ� �ִ� Host ID �������� ������ ���� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		type	- [in]  ���� Ÿ�� (E30_GVTC)
		lst_data- [out] �������� ������ ����� ����Ʈ ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetHostVariables(LONG conn_id, E30_GVTC type, CAtlList <STG_CEVI> &lst_data);



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
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRJobStartProcess(PTCHAR pj_id);
/*
 desc : Process Job�� ���� Control Job ID ��ȯ
 parm : pj_id	- [in]  Object ID of the Process Job
		cj_id	- [out] Object ID of the Control Job
		size	- [in]  'cj_id' ���� ũ��
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetControlJobID(PTCHAR pj_id, PTCHAR cj_id, UINT32 size);
/*
 desc : Process Job�� ���� Control Job ID ����
 parm : pj_id	- [in]  Object ID of the Process Job
		cj_id	- [in]  Object ID of the Control Job
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetControlJobID(PTCHAR pj_id, PTCHAR cj_id);
/*
 desc : Process Job�� Processing�� ���� �����
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
 note : Moves Process Job from PROCESSING state to PROCESS COMPLETE state.
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRJobProcessComplete(PTCHAR pj_id);
/*
 desc : Process Job �۾� �Ϸ� (PRJobComplete -> get_ParentControlJob -> ProcessJobComplete)
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRJobComplete(PTCHAR pj_id);
/*
 desc : Process Job�� Stopping ���¿��� Stopped ���·� �̵� (����)
 parm : pj_id	- [in]  Objet ID of the Process Job
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetProcessJobStoppingToStopped(PTCHAR pj_id);
/*
 desc : Process Job�� ABORTING ���¿��� ABORTED ���·� �̵�
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
 note : Moves Process Job from ABORTING state to the ABORTED state.
		�۾� ��Ұ� �Ϸ�Ǿ��� ��, �� �Լ��� ȣ���� �ָ� ��
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRJobAbortComplete(PTCHAR pj_id);
/*
 desc : Process Job (ID)�� ���� ���� ��ȯ
 parm : pj_id	- [in]  Process Job ID
		state	- [OUT] state
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetProcessJobState(PTCHAR pj_id, E40_PPJS &state);
/*
 desc : ��� Process Job Id ��ȯ (�Ϸ���� ���� �۾��� ���¸� ��ȯ)
 parm : list_job- [out] Job ID�� ��ȯ�Ǿ� ����� �迭 ����
		logs	- [in]  �α� ��� ����
 ��ȯ : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetProcessJobQueue(CStringArray &list_job);
/*
 desc : Processing Resource�� ���� Queue���� ������ �� �ִ� ���� �ִ� Jobs ���� ��ȯ
 parm : count	- [out] ������ �� �ִ� ���� ���� ��ȯ ���� ����
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetProcessJobSpaceCount(UINT32 &count);
/*
 desc : ���� Process Job ID �� �� ó�� SELECTED ������ Process Job ID ��ȯ
 parm : filter	- [in]  Process Job ID �˻� ���� (E40_PPJS)
		pj_id	- [out] ��ȯ�Ǿ� ����� Process Job ID (String)
		size	- [in]  'pj_id' ���� ũ��
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
 note : �˻��� ����� ������ FALSE ��ȯ
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetProcessJobFirstID(E40_PPJS filter, PTCHAR pj_id, UINT32 size);
/*
 desc : ��� Process Job Id�� State ��ȯ
 parm : list_job	- [out] Job ID�� ��ȯ�Ǿ� ����� �迭 ����
		list_state	- [out] ���� ���� ������� ����Ʈ ����
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetProcessAllJobIdState(CStringArray &list_job,
														CAtlList<E40_PPJS> &list_state);
/*
 desc : Process Job Remove
 parm : pj_id	- [in]  �����ϰ��� �ϴ� Process Job Id
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetProcessJobRemove(PTCHAR pj_id);
/*
 desc : Process Job�� Queuing ���¿��� Setting Up ���·� �̵�
 parm : pj_id	- [in]  Objet ID of the Process Job
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetProcessJobQueueToSetup(PTCHAR pj_id);
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
API_IMPORT BOOL uvCIMLib_E40PJM_SetProcessJobCreate(PTCHAR pj_id, PTCHAR carr_id,
													PTCHAR recipe_name, E40_PPMT mtrl_type,
													BOOL recipe_only, BOOL proc_start,
													CAtlList <UINT8> *slot_no, BOOL en_hanced=TRUE);
/*
 desc : Remote Control ��� ���� ����
 parm : enable	- [in]  Enable or Disable
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetAllowRemoteControl(BOOL enable);
/*
 desc : Remote Control ��� ���� �� ��ȯ
 parm : enable	- [out] Enable or Disable
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetAllowRemoteControl(BOOL &enable);
/*
 desc : Process Material Name List (For substrates)�� �Ӽ� �� ��ȯ
		!!! TEL�� ���, ������ 2��° ��, CarrierID �� Slot No�� �Ǿ� �ִ� ������� �о�� �� !!!
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		carr_id	- [out] Carrier ID�� ��ȯ�Ǿ� ����� ����
		slot_no	- [out] Slot Number�� ��ȯ�Ǿ� ����� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetPRMtlNameList(PTCHAR pj_id,
												 CStringArray &carr_id, CByteArray &slot_no);
/*
 desc : ���� Process Job ID ���� ��ϵ� Slot Number List �߿��� ã���� �ϴ� Slot Number�� �ִ��� ����
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		slot_no	- [in]  �˻��ϰ��� �ϴ� Slot Number (1-based)
		is_slot	- [out] Slot Number ���� ���� ��ȯ
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_IsFindSlotNoOfPRMtlNameList(PTCHAR pj_id, UINT8 slot_no, BOOL &is_slot);
/*
 desc : Process Job�� ���� ���� ���� ������ ���� ��Ŵ
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		control	- [in]  E40_PPJC (Abort, Cancel, Pause, Paused, Resume, Start, Stop)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRJobStateControl(PTCHAR pj_id, E40_PPJC control);
/*
 desc : Process Job State ����
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		state	- [in]  Process Job State (E40_PPJS)
 retn : TRUE or FALSE
 note : Sets the Process Job's state
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetProcessJobState(PTCHAR pj_id, E40_PPJS state);
/*
 desc : Process Job State : Aborting -> Aborted
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
 retn : TRUE or FALSE
 note : This method is called by the CIM40 package when aborting has completed
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetProcessJobAborted(PTCHAR pj_id);
/*
 desc : Returns the PRMtrlOrder value set by the host using the PRSetMtrlOrder service
		Queue�� ��ϵ� ��, Process Job ID�� ���� ����� ��ȯ �մϴ�.
 parm : order	- [out] Material Order (E94_CPOM, PRMtrlOrder value)
 retn : TRUE or FALSE
 note : This function returns the PRMtrlOrder value, the order in which materials are processed
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetPRMaterialOrder(E94_CPOM &order);
/*
 desc : Sets the PRMtrlOrder value by the host using PRSetMtrlOrder service
		Queue���� Process Job ID�� ����Ǵ� ���� ��� ����
 parm : order	- [in]  Material Order (E94_CPOM, PRMtrlOrder value)
 retn : TRUE or FALSE
 note : Sets the PRMtrlOrder value by the host using PRSetMtrlOrder service.
		PRSetMtrlOrder callback is called to let the client application grant or deny this request.
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRMaterialOrder(E94_CPOM order);
/*
 desc : Process Job���� ����� ���� ��Ŵ (Executes a command on a Process Job)
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		command	- [in]  ��ɾ� (PCIMLib���� ��ϵ� ��ɾ�. START;STOP;PAUSE;RESUME;ABORT;CANCEL)
						Register for Remote Commands (Remote Command name�� �� ���̰� 20 string�� ���� �ʾƾ� ��)
 retn : TRUE or FALSE
 note : This method is called by the host to perform a PRJobCommand on the specified Process Job
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRJobCommand(PTCHAR pj_id, PTCHAR command);
/*
 desc : Pause Events�� ���� Enable or Disable
 parm : enable	- [in]  TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE
 note : The client application can disable PauseEvent capability for Process Jobs by setting this value to VARIANT_FALSE.
		Default is VARIANT_TRUE.
		When set to VARIANT_FALSE, all requests to create Process Jobs with the PauseEvent parameter will be rejected.
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetEnablePauseEvent(BOOL enable);
/*
 desc : Pause Events�� ���� Enable or Disable ���� �� ��ȯ
 parm : enable	- [out] TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetEnablePauseEvent(BOOL &enable);
/*
 desc : ��� �ʿ��� ����ϴ� �˸� (Notification)�� ���� (Type)�� �������� �˷���
 parm : type	- [out] Notification Type (E40_PENT)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetNotificationType(E40_PENT &type);
/*
 desc : ��� �ʿ��� ����� �˸� (Notification)�� ���� (Type)�� � ������ ������ ��
 parm : type	- [in]  Notification Type (E40_PENT)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetNotificationType(E40_PENT type);
/*
 desc : Process Job�� ���°� ����Ǿ��ٰ� Host���� �˸�
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRJobAlert(PTCHAR pj_id);
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
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRJobEvent(PTCHAR pj_id, E40_PSEO evt_id,
											  CAtlList <UINT32> *list_vid,
											  CAtlList <E30_GPVT> *list_type,
											  CStringArray *list_value);
/*
 desc : ���� (Ư��) ���μ��� Job������ �Ӽ��� "PRProcessStart"�� ���� ����
 parm : list_jobs	- [in]  ���� ����� Process Job���� ����� ����Ʈ
		start		- [in]  TRUE or FALSE
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRJobSetStartMethod(CStringArray *list_jobs, BOOL start);
/*
 desc : ���� ������ Material Type �� ��ȯ
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		type	- [out] ���� ������ Material Type �� ����
 retn : TRUE or FALSE
 note : Gets the PRMtlType attribute value
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetPRMtlType(PTCHAR pj_id, E40_PPMT &type);
/*
 desc : Material Type �� ���� ����
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		type	- [in]  ������ Maerial Type ��
 retn : TRUE or FALSE
 note : Sets the PRMtlType attribute value
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRMtlType(PTCHAR pj_id, E40_PPMT type);
/*
 desc : ó���� �����ϱ� ���� ��� ���� (Wafer? Substrate)���� ������ ������ ����ϴ��� ���� �� ��ȯ
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		value	- [out] TRUE : ��� ���� (����)���� �����ϴµ� ��ٸ��� ����
						FALSE: ��� ���� (����)���� �̹� ������ ������
 retn : TRUE or FALSE
 note : Gets the Process Job's PRWaitForAllMaterial switch value.
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetPRWaitForAllMaterial(PTCHAR pj_id, BOOL &value);
/*
 desc : ó���� �����ϱ� ���� ��� ���� (Wafer? Substrate)���� ������ ������ ����ؾ� ���� ���� ����
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		value	- [in]  TRUE : ��� ���� (����)���� �����ϴµ� ��ٸ��� �ִٶ�� ����
						FALSE: ��� ���� (����)���� �����Ͽ��ٶ�� ���� (��ٸ� �ʿ� ����?)
 retn : TRUE or FALSE
 note : Sets the Process Job's PRWaitForAllMaterial switch value.
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRWaitForAllMaterial(PTCHAR pj_id, BOOL value);
/*
 desc : Process Job ID�κ��� Recipe ID (=Name; with string) ���
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		rp_id	- [out] Recipe ID�� ����� ����
		size	- [in]  'rp_id' ���� ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetRecipeID(PTCHAR pj_id, PTCHAR rp_id, UINT32 size);
/*
 desc : ���� Process Job ID�� ���� �۾��� �Ϸ� �Ǿ����� ����
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		state	- [out] 0x00: Busy, 0x01: Completed
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_IsProcessJobCompleted(PTCHAR pj_id, UINT8 &state);
/*
 desc : ��� Process Job Id ��ȯ
 parm : list_job- [out] Job ID�� ��ȯ�Ǿ� ����� �迭 ����
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetProcessAllJobId(CStringArray &list_job);
/*
 desc : Process Job ID�� ���Ե� ��� Slot Number ��ȯ
 parm : pj_id	- [in]  �˻� ����� Process Job ID
		arr_slot- [out] Slot Number�� ����� Byte Array
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetPRJobToAllSlotNo(PTCHAR pj_id, CByteArray &arr_slot);
/*
 desc : Process Job ID�� ���Ե� ��� Slot ID ��ȯ
 parm : pj_id	- [in]  �˻� ����� Process Job ID
		arr_slot- [out] Slot ID�� ����� String Array
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetPRJobToAllSlotID(PTCHAR pj_id, CStringArray &arr_slot);
/*
 desc : Process Job�� Queue ���¿��� Completed ���·� �̵� (����)�ϸ� Project Job Id�� ����
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
 note : SetProcessJobRemove �Լ��� �ٸ��� ? Queue���� �����ϴ� ����, Job List���� �����ϴ� ���� �ƴ� 
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetProcessJobDequeue(PTCHAR pj_id);

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
API_IMPORT BOOL uvCIMLib_E87CMS_GetSubstToE87SubstrateID(PTCHAR carr_id, UINT8 slot_no,
														 PTCHAR subst_id, UINT32 size);
/*
 desc : Carrier�� �ε� ��Ʈ ���� ���� (��������;�����ϰ�)�ϰų� Ȥ�� ���ŵǰ� �ִ� ���̴� (?)
 parm : port_id	- [in]  Load Port Number (1 or 2)
 retn : TRUE or FALSE
 note : Load Port ���¸� "Ready To Load" or "Ready To Unload"���� "Transfer Bloacked"���� ���� ��Ŵ
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetTransferStart(UINT16 port_id);
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
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierPlaced(UINT16 port_id);
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
API_IMPORT BOOL uvCIMLib_E87CMS_SetReadyToUnload(UINT16 port_id);
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
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierAtPort(PTCHAR carr_id, UINT16 port_id, PTCHAR loc_id);
/*
 desc : Carrier�� �ε� ��Ʈ���� �и� �Ǿ��ٰ� CIM87�� �˸�
 parm : carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		port_id	- [in]  Load Port Number (1 or 2)
		loc_id	- [in]  Carrier Location ID ��, ��κ� Carrier Undocked Name (Carrier Location Name)		
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierDepartedPort(PTCHAR carr_id, UINT16 port_id, PTCHAR loc_id);
/*
 desc : Carrier�� ���ο� ��ġ�� �̵� �ߴٰ� �˸�
 parm : carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		port_id	- [in]  Load Port Number (1 or 2)
		docked	- [in]  Carrier Docked Name (Location Name)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetMoveCarrier(PTCHAR carr_id, UINT16 port_id, PTCHAR docked);
/*
 desc : Carrier ���� ���� (��ġ)�� Slot Map�� ���� Verification ó��
 parm : carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		slots	- [in]  Wafer�� Slot Map�� ����� ���� ����
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetVerifySlotMap(PTCHAR carr_id, CAtlList <E87_CSMS> *slots,
												 LONG handle=0);
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
API_IMPORT BOOL uvCIMLib_E87CMS_SetAccessCarrier(LONG conn_id, PTCHAR carr_id, UINT16 port_id,
												 E87_CCAS status, LONG handle=0);
/*
 desc : Carrier Status Event ����
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		port_id	- [in]  Load Port Number (1 or 2)
		status	- [in]  Carrier Event Value (E87_CCES)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierEvent(LONG conn_id, PTCHAR carr_id, UINT16 port_id,
												E87_CCES status);
/*
 desc : Carrier�κ��� Substrate ���� (��������)
 parm : carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		slot_no	- [in]  Slot Number (Not sequential number)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierPopSubstrate(PTCHAR carr_id, UINT8 slot_no);
/*
 desc : ���� Load Port ID�� Reservation State ���� ��ȯ
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [out] Reservation State ��ȯ
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetReservationState(UINT16 port_id, E87_LPRS &state);
/*
 desc : ���� Load Port ID�� Reservation State ���� ����
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [in]  Reservation State
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetReservationState(UINT16 port_id, E87_LPRS state);
/*
 desc : ���� Load Port-Carrier�� Association State ���� ��ȯ
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [out] Load Port-Carrier Association State ��ȯ
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetAssociationState(UINT16 port_id, E87_LPAS &state);
/*
 desc : ���� Load Port-Carrier�� Association State ���� ����
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [in]  Load Port-Carrier Association State
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetAssociationState(UINT16 port_id, E87_LPAS state);
/*
 desc : ���� Carrier�� Access Mode State ���� ��ȯ
		Load Port�� �������� �����ϴ��� Ȥ�� �ڵ����� �����ϴ��� ���� �� ��ȯ
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [out]  Manual (0) or Auto (1)
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetAccessModeState(UINT16 port_id, E87_LPAM &state);
/*
 desc : ���� Carrier�� Access Mode Status �� ����
		Load Port�� �������� �����ϴ��� Ȥ�� �ڵ����� �����ϴ��� ���� �� ����
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [in]  Manual (0) or Auto (1)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetAccessModeState(UINT16 port_id, E87_LPAM state);
/*
 desc : Carrier ID Verification State �� ��ȯ
 parm : carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		state	- [out] Carrier ID Verfication ���� �� ��ȯ
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetCarrierIDStatus(PTCHAR carr_id, E87_CIVS &status);
/*
 desc : Carrier ID Verification State �� ����
 parm : carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		state	- [in]  Carrier ID Verfication ���� ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierIDStatus(PTCHAR carr_id, E87_CIVS status);
/*
 desc : The slot map status of the specified carrier.
 parm : carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		status	- [out] Slot map status ��ȯ
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
 note : The slot map status shall be read on all production
		equipment prior to removal of substrates from the carrier
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetCarrierIDSlotMapStatus(PTCHAR carr_id, E87_SMVS &status);
/*
 desc : The slot map status of the specified carrier. <����>
 parm : carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		status	- [in]  Slot map status
 retn : TRUE or FALSE
 note : The slot map status shall be read on all production
		equipment prior to removal of substrates from the carrier
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierIDSlotMapStatus(PTCHAR carr_id, E87_SMVS status);
/*
 desc : ���� ���� ��� ���� ����
 parm : enable	- [in]  TRUE or FALSE
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetAllowRemoteControl(BOOL enable);
/*
 desc : ���� ���� ��� ���� ����
 parm : enable	- [in]  TRUE or FALSE
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetAllowRemoteControl(BOOL &enable);
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
API_IMPORT BOOL uvCIMLib_E87CMS_SetProceedWithCarrier(UINT16 port_id, PTCHAR carr_id);
/*
 desc : CancelCarrier service ��û (Carrier ���)
 parm : port_id	- [in]  Load Port ID (1 or 2)
		carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCancelCarrier(UINT16 port_id, PTCHAR carr_id);
/*
 desc : CancelCarrierNotification service request (Carrier ��ҵǾ��ٰ� �˸�)
 parm : carr_id	- [in]  Carrier ID (or Name; String)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelCarrierNotification service
		E_FAIL will be returned if the call fails. The "status" will contain the text of any error
		and is in the form of the S3F18 reply message. Callbacks will be called.
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCancelCarrierNotification(PTCHAR carr_id);
/*
 desc : Request to re-create the carrier object
 parm : port_id	- [in]  Load Port ID (1 or 2)
		carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
 retn : TRUE or FALSE
 note : The pParameterList parameter is the same format as the attribute list from the S3F17 message
		Carrier ���°� READY_TO_UNLOAD���� ���θ� ���ο��� Ȯ�� �� ���� ���� ����
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierReCreate(UINT16 port_id, PTCHAR carr_id);
/*
 desc : ReleaseCarrier service ��û
 parm : carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierRelease(PTCHAR carr_id);
/*
 desc : Carrir Location service ��û
 parm : loc_id	- [in]  Load Port Ȥ�� Buffer�� �ƴ� �ٸ� ���� �߰��ϰ��� �ϴ� ID (�̸�. String)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetAddCarrierLocation(PTCHAR loc_id);
/*
 desc : Load Port�� �ο� (��ġ; ����)�� Carrier ID (Name) ��ȯ
 parm : port_id	- [in]  Load Port ID (1 or 2)
		carr_id	- [out] Carrier ID�� ��ȯ�Ǿ� ����� ���� ������
		size	- [in]  'carr_id' ���� ũ��
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetAssociationCarrierID(UINT16 port_id, PTCHAR carr_id, UINT32 size);
/*
 desc : Load Port�� �ο� (��ġ; ����)�� Carrier ID (Name) ����
 parm : port_id	- [in]  Load Port ID (1 or 2)
		carr_id	- [in]  Carrier ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetAssociationCarrierID(UINT16 port_id, PTCHAR carr_id);
/*
 desc : Carrier ID�� �ο� (����; ����)�� Load Port Number ��ȯ
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		port_id	- [Out] Load Port ID (1 or 2)
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetAssociationPortID(PTCHAR carr_id, UINT16 &port_id);
/*
 desc : Carrier ID�� Load Port Number �ο� (�� �Լ��� Ư���� ��츦 �����ϰ�, ��� �������� ����)
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		port_id	- [in]  Load Port ID (1 or 2)
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetAssociationPortID(PTCHAR carr_id, UINT16 port_id);
/*
 desc : Host�κ��� ��û���� Transaction ���� �� ��ȯ
 parm : None
 retn : ��û ���� Transaction ������ ����� ���� ����ü ������ ��ȯ
*/
API_IMPORT LPG_CHCT uvCIMLib_E87CMS_GetTransactionInfo();
/*
 desc : ��ġ(CMPS)�� Carrier Tag �а� �� ��, Host���� S3F30 �޽����� ȣ���ϱ� ���� ����
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		tag_id	- [in]  The requested tag data, can be null if a tag is empty or fail to read
		trans_id- [in]  The transaction ID of S3F29 (Carrier Tag Read Request)
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
 note : uvCIMLib_E87CMS_GetCarrierTagReadData ȣ��ǰ� �� ��, Host���� �޽����� ���� ��, ȣ���ؾ� ��
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetAsyncCarrierTagReadComplete(PTCHAR carr_id, PTCHAR tag_id,
															   LONG trans_id);
/*
 desc : Carrier�κ��� ���� Tag Data ��ȯ
 parm : loc_id	- [in]  Location identifier
		carr_id	- [in]  Carrier ID (or Name; String)
		data_seg- [in]  Indicates a specific section of data (Tag ID ?)
		data_tag- [out] Carrier�κ��� �о���� Tag Data�� ����� ����
		size	- [in]  'data_tag' ���� ũ��
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
 note : loc_id or carr_id �� 1���� �����Ǿ� �־�� ��. ��, �� �߿� 1���� NULL�̾ ��� ����
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetCarrierTagReadData(PTCHAR loc_id, PTCHAR carr_id, PTCHAR data_seg,
													  PTCHAR data_tag, UINT32 size);
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
API_IMPORT BOOL uvCIMLib_E87CMS_SetBeginCarrierOut(PTCHAR carr_id, UINT16 port_id);
/*
 desc : Carrier�� Load Port���� ���� ���� ��ġ�� �̵� �Ǿ��ٰ� ����
		��, Load Port�� Undocked���� Docked ��ġ�� �̵� �ߴٰ� ���� ��
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		port_id	- [in]  Load Port ID (1 or 2)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CarrierOut service.
		E_FAIL will be returned if the call fails.
		The "status" will contain the text of any error and
		is in the form of the S3F18 reply message. Callbacks will be called.
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierOut(PTCHAR carr_id, UINT16 port_id);
/*
 desc : Carrier�� ������ (Ư��) Load Port���� �ٽ� ���� �ִٰ� �˸�
		������ CarrierOut �Ǿ� �ִ� ������ �ٽ� ���� �ִٰ� �˸�
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
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierIn(PTCHAR carr_id);
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
API_IMPORT BOOL uvCIMLib_E87CMS_SetCancelAllCarrierOut();
/*
 desc : Load Port�� Transfer Status �� ��ȯ
 parm : port_id	- [in]  Load Port ID (1 or 2)
		status	- [out] Load Port�� Transfer Status �� ��ȯ
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
 note : Get the current load port transfer state
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetTransferStatus(UINT16 port_id, E87_LPTS &status);
/*
 desc : Load Port�� Transfer Status �� ����
 parm : port_id	- [in]  Load Port ID (1 or 2)
		status	- [in]  Load Port�� Transfer Status ��
 retn : TRUE or FALSE
 note : Sets the Load Port Transfer State for the specified load port.
		The new value can be any state and does not depend on the current state
		!!! important. �������� ���� ��ȯ�� �������, ���������� �˾Ƽ� ��ȯ�� ���� ����
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetTransferStatus(UINT16 port_id, E87_LPTS status);
/*
 desc : CancelBind service Request
		Carrier ID�� Load Port ���� �κ��� ����ϰ� Load Port�� NOT_RESERVED ���·� ����
 parm : port_id	- [in]  Load Port ID (1 or 2) (-1 or 0 �̻� ��)
		carr_id	- [in]  Carrier ID (or Name; String) (NULL or String)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelBind service
		port_id Ȥ�� carr_id �� ���� �ϳ��� ���� �ݵ�� �ԷµǾ�� ��
		Either portID or carrierID is required, but not both
		Set portID to -1 if it is not used and carrierID to an empty string if it is not used
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCancelBind(UINT16 port_id, PTCHAR carr_id);
/*
 desc : CancelCarrierOut service Request
		���� Carrier�� ���õ� Action�� ����ϰ�,
		��� (Equipment)�� Carrier�� Load Port�� Unload (Ȥ�� ���� ����) ��ġ�� ���� �մϴ�.
 parm : port_id	- [in]  Load Port ID (1 or 2)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelCarrierAtPort service
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCancelCarrierAtPort(UINT16 port_id);
/*
 desc : CancelCarrierOut service Request
		���� Carrier�� ���õ� CarrierOut ���񽺸� Queue���� ����
 parm : carr_id	- [in]  Carrier ID (or Name; String)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelCarrierOut service
		The "status" will contain the text of any error and is in the form of the S3F18 reply message
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCancelCarrierOut(PTCHAR carr_id);
/*
 desc : CancelReserveAtPort service Request
 parm : port_id	- [in]  Load Port ID (1 or 2)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelReserveAtPort service
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCancelReserveAtPort(UINT16 port_id);
/*
 desc : ��� (Equipment)���� ���� Carrier�� ���� ���� �� ����
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		status	- [in]  Accessing Status (E87_CCAS)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelReserveAtPort service
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierAccessingStatus(PTCHAR carr_id, E87_CCAS status);
/*
 desc : ��� (Equipment)���� ���� Carrier�� ���� ���� �� ���
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		status	- [out] Accessing Status (E87_CCAS)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelReserveAtPort service
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetCarrierAccessingStatus(PTCHAR carr_id, E87_CCAS &status);
/*
 desc : ���� Carrier ID ���°� Closed �������� ����
 parm : carr_id	- [in]  Carrier ID (or Name; String)
 retn : TRUE (Closed), FALSE (Not closed)
*/
API_IMPORT BOOL uvCIMLib_E87CMS_IsCarrierAccessClosed(PTCHAR carr_id);
/*
 desc : Carrier (FOUP) ���� Ư�� ��ġ�� Substrate (Wafer)�� ����
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		slot_no	- [in]  Slot Number (Not sequential number)
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetSubstratePlaced(PTCHAR carr_id, PTCHAR subst_id, UINT8 slot_no);
/*
 desc : Recipe (����, ����, ����) �̺�Ʈ ó��
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		recipe	- [in]  Recipe Name
		type	- [in]  0x01 (Created), 0x02 (Edited), 0x03 (Deleted)
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetRecipeManagement(LONG conn_id, PTCHAR recipe, UINT8 type,
													LONG handle=0);
/*
 desc : Recipe (Selected) �̺�Ʈ ó��
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		recipe	- [in]  Recipe Name
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetRecipeSelected(LONG conn_id, PTCHAR recipe, LONG handle=0);


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
API_IMPORT BOOL uvCIMLib_E90STS_SetChangeSubstrateStateName(PTCHAR subst_id, PTCHAR loc_id,
															E90_SSTS trans, E90_SSPS proc);
API_IMPORT BOOL uvCIMLib_E90STS_SetChangeSubstrateStateIndex(PTCHAR subst_id, ENG_TSSI loc_id,
															 E90_SSTS trans, E90_SSPS proc);
/*
 desc : Substrate (Transport & Processing) ���� �� ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		loc_id	- [in]  Location ID (Name)�� ����� ���ڿ� ���� or 1-Based Index
						Substrate Location ID_xx (���ڿ�. RobotArm / PreAligner / ProcessChamber / Buffer)
		trans	- [in]  Substrate transport state
		proc	- [in]  Substrate processing state
		status	- [in]  Substrate ID state E90_SISM
		time_in	- [in]  Batch�� Location ID�� �����ϴ� �ð� (NULL�̸� ��� ����)
		time_out- [in]  Batch�� Location ID�� ��Ż�ϴ� �ð� (NULL�̸� ��� ����)
						time format : YYYYMMDDhhmmsscc for YearMonthDayHourMinuteSecondCentisecond
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetChangeSubstrateStateName2(PTCHAR subst_id, PTCHAR loc_id,
															 E90_SSTS trans, E90_SSPS proc,
															 E90_SISM status,
															 PTCHAR time_in=NULL, PTCHAR time_out=NULL);
API_IMPORT BOOL uvCIMLib_E90STS_SetChangeSubstrateStateIndex2(PTCHAR subst_id, ENG_TSSI loc_id,
															  E90_SSTS trans, E90_SSPS proc,
															  E90_SISM status,
															  PTCHAR time_in=NULL, PTCHAR time_out=NULL);
/*
 desc : Substrate Processing ���� �� ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		proc	- [in]  Substrate processing state
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetChangeSubstrateProcessingState(PTCHAR subst_id, E90_SSPS proc);
/*
 desc : Substrate Location (��ġ;����) ���
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		loc_id	- [in]  Location ID (Name)�� ����� ���ڿ� ���� or 1-Based Index
						Substrate Location ID_xx (���ڿ�. RobotArm / PreAligner / ProcessChamber / Buffer)
		lot_id	- [in]  Lot ID (If NULL, Not used)
						Lot ID (Wafer ���� �ο��� �ĺ� ��ȣ)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_RegisterSubstrateName(PTCHAR subst_id, PTCHAR loc_id,
													  INT16 sub_id=0, PTCHAR lot_id=NULL);
API_IMPORT BOOL uvCIMLib_E90STS_RegisterSubstrateIndex(PTCHAR subst_id, ENG_TSSI loc_id,
													   INT16 sub_id=0, PTCHAR lot_id=NULL);
/*
 desc : Substrate ID ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetRemoveSubstrate(PTCHAR subst_id);
/*
 desc : Substrate Location (Wafer�� ������ ��ġ �̸�) ���
 ���� :	loc_id	- [in]  Location ID (Name)�� ����� ���ڿ� ����
						Substrate Location ID_xx (���ڿ�. RobotArm / PreAligner / ProcessChamber / Buffer)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetAddSubstrateLocation(PTCHAR loc_id);
/*
 desc : Substrate Location (Wafer�� ������ ��ġ �̸�) ����
 ���� :	loc_id	- [in]  Location ID (Name)�� ����� ���ڿ� ����
							Substrate Location ID_xx (���ڿ�. RobotArm / PreAligner / ProcessChamber / Buffer)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetRemoveSubstrateLocation(PTCHAR loc_id);
/*
 desc : Substrate (Wafer)�� ���� �ϵ��� (�޵���) ��� ���� ȣ��
		��� �ʿ��� ���������� E90CommandCallback �Լ��� ȣ���
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
 retn : TRUE or FALSE
 note : Request the equipment to accept the substrate.
		This function executes the E90CommandCallback() to allow the equipment to reject the command.
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetProceedWithSubstrate(PTCHAR subst_id);
/*
 desc : Remote Control ��� ���� �� ��ȯ
 parm : enable	- [out] Enable or Disable
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetAllowRemoteControl(BOOL &enable);
/*
 desc : Remote Control ��� ���� ����
 parm : enable	- [in]  Enable or Disable
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetAllowRemoteControl(BOOL enable);
/*
 desc : Substrate Location ID�� ���� �� ��ȯ
 parm : loc_id	- [in]  Location ID (Name)�� ����� ���ڿ� ���� or 1-Based Index
						Substrate Location ID_xx (���ڿ�. RobotArm / PreAligner / ProcessChamber / Buffer)
		state	- [out] Substrate Location State �� ��ȯ
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateLocationStateName(PTCHAR loc_id, E90_SSLS &state);
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateLocationStateIndex(ENG_TSSI loc_id, E90_SSLS &state);
/*
 desc : Substrate Location ID�� ���� �� ����
 parm : loc_id	- [in]  Location ID (Name)�� ����� ���ڿ� ���� or 1-Based Index
						Substrate Location ID_xx (���ڿ�. RobotArm / PreAligner / ProcessChamber / Buffer)
		state	- [in]  Substrate Location State �� ��ȯ (E90_SSLS)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateLocationStateName(PTCHAR loc_id, E90_SSLS state);
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI loc_id, E90_SSLS state);
/*
 desc : Substrate Processing�� ���� ���� �� ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		state	- [out] Substrate Location State �� ��ȯ (E90_SSPS)
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstratePrevProcessingState(PTCHAR subst_id, E90_SSPS &state);
/*
 desc : Substrate Processing ���� �� ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		state	- [out] Substrate Location State �� ��ȯ (E90_SSPS)
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateProcessingState(PTCHAR subst_id, E90_SSPS &state);
/*
 desc : Substrate Processing ���� �� ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		state	- [in]  �����ϰ��� �ϴ� ���� �� (E90_SSPS)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateProcessingState(PTCHAR subst_id, E90_SSPS state);
/*
 desc : Substrate Transport ���� ���� �� ��ȯ
 parm : subst_id- [in]  Substrate ID (���ڿ�)
		state	- [out] Substrate Transport State �� ��ȯ (E90_SSTS)
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstratePrevTransportState(PTCHAR subst_id, E90_SSTS &state);
/*
 desc : Substrate Transport ���� �� ��ȯ
 parm : subst_id- [in]  Location ID (Name)�� ����� ���ڿ� ���� or 1-Based Index
						Substrate Location ID_xx (���ڿ�. RobotArm / PreAligner / ProcessChamber / Buffer)
		state	- [out] Substrate Location State �� ��ȯ (E90_SSPS)
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateTransportStateName(PTCHAR subst_id, E90_SSTS &state);
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateTransportStateIndex(ENG_TSSI subst_id, E90_SSTS &state);
/*
 desc : Substrate Transport ���� �� ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		state	- [in]  �����ϰ��� �ϴ� ���� �� (E90_SSTS)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateTransportState(PTCHAR subst_id, E90_SSTS state);
/*
 desc : Substrate ID (Tag ? Serial ? Bar Code)�� ���� ���� �� ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		state	- [out] Enumeration values for the ID Reader state machine
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstratePrevIDStatus(PTCHAR subst_id, E90_SISM &status);
/*
 desc : Substrate ID (Tag ? Serial ? Bar Code) �о��� �� ���� ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		state	- [out]  Enumeration values for the ID Reader state machine
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateIDStatus(PTCHAR subst_id, E90_SISM &status);
/*
 desc : Substrate ID (Tag ? Serial ? Bar Code) �о��� �� ���� ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		state	- [in]  Enumeration values for the ID Reader state machine
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateIDStatus(PTCHAR subst_id, E90_SISM status);
/*
 desc : CIM300 (Host)���� Substrate ID (Tag ? Serial ? Bar Code) �о��� �� ���¸� �Ǵ��� �޶�� ��û
 parm : subst_id	- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		acquired_id	- [in]  Substrate ID acquired from the substrate ID reader (Serial? Tag?)
		read_good	- [in]  TRUE if read was successful. Otherwise FALSE
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetNotifySubstrateRead(PTCHAR subst_id, PTCHAR acquired_id,
													   BOOL read_good);
/*
 desc : Substrate ID Reader�� �ǵ� ���� Ȥ�� �Ұ��� �� ��, �̺�Ʈ �߻� ��Ŵ
 parm : enable	- [in]  Substrate ID Reader�� ��� �������� ���� ����
						TRUE: ��� ������ ��, �̺�Ʈ �˸�
						FALSE:��� �Ұ����� ��, �̺�Ʈ �˸�
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateReaderAvailable(INT32 enable);
/*
 desc : Request to the equipment to skip the substrate and move it to its source or destination location
		��ġ���� substrate�� �ǳʶ��, �ҽ� Ȥ�� �ٸ� ��ġ�� �̵��϶�� ��û �� (���� ��ġ�� �̵��϶�� ��)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
 retn : TRUE or FALSE
 note : This function executes the E90CommandCallback() to allow the equipment to reject the command
		��, �� �Լ��� ȣ���ϸ�, ��� ������ �źε� ���, E90CommandCallback �Լ��� ȣ�� ��
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetCancelSubstrate(PTCHAR subst_id);
/*
 desc : ���� Substrate ID���� �о���� Wafer Reader Code (Tag, Serial, Bar Code, etc) ���� ��ȯ
 parm : subst_id	- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		acquired_id	- [out] Wafer Reader�κ��� �о���� ID �� ��ȯ (���ڿ�)
		size		- [in]  'acquired_id' ���� ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateIDToAcquiredID(PTCHAR subst_id, PTCHAR acquired_id, UINT32 size);
/*
 desc : ���� Substrate ID���� �о���� Wafer Reader Code (Tag, Serial, Bar Code, etc) �� ����
 parm : subst_id	- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		acquired_id	- [in]  Wafer Reader�κ��� �о���� ID �� (���ڿ�)
 retn : TRUE or FALSE
 note : Contains the ID read from the substrate. Empty string before the substrate is read.
		The attribute shall be updated as soon the substrate ID has been successfully read
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateIDToAcquiredID(PTCHAR subst_id, PTCHAR acquired_id);
/*
 desc : Substrate ID �� ��ȯ (Slot Number; Slot No)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		slot_no	- [out] Slot Number ���� ��ȯ�Ǿ� ���� (1-based or Later. Max. 255)	
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateIDToSlotNo(PTCHAR subst_id, UINT8 &slot_no);
/*
 desc : Substrate Location ID �� ��ȯ (Slot Number; Slot No)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		loc_id	- [out] Location ID (Buffer, Robot, PreAligner, Chamber, etc)�� ����� ����
		size	- [in]  'loc_id' ���� ũ��
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateLocationID(PTCHAR subst_id, PTCHAR loc_id, UINT32 size);
/*
���� : �ش� ��ġ�� �����۰� �����ϴ��� ����
���� : loc_id	- [in]  Location ID (Buffer, Robot, PreAligner, Chamber, etc)
��ȯ : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_IsLocationSubstrateID(ENG_TSSI loc_id);
/*
 desc : Ư�� wafer (substrate ID; st_id)�� ���� location�� ������ ���ÿ� ���� ���� �� ����
 parm : subst_id	- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		loc_id		- [in]  Location ID (���ڿ�)
		proc_state	- [in]  Processing State (E90_SSPS)
		trans_state	- [in]  Transfer State (E90_SSTS)
		read_status	- [in]  ID Read Status (E90_SISM)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateAnyLocStates(PTCHAR subst_id, PTCHAR loc_id,
														 E90_SISM read_status, E90_SSPS proc_state,
														 E90_SSTS trans_state);
/*
 desc : ���� Wafer�� ��Ʈ (Lot; FOUP)�� ������ ��ü�� ��ġ ������ ��ȯ �Ѵ�
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		index	- [out] Wafer�� ��Ʈ (Lot; FOUP)�� ������ ��ġ (�ε���) ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetBatchLocIdx(PTCHAR loc_id, INT16 &index);
/*
 desc : ���� Wafer�� ��Ʈ (Lot; FOUP)�� ������ ��ü�� ��ġ ������ ��ȯ �Ѵ�
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		index	- [out] Wafer�� ��Ʈ (Lot; FOUP)�� ������ ��ġ (�ε���) ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetBatchLocIdx(PTCHAR loc_id, INT16 index);
/*
 desc : ���� Wafer�� ��Ʈ (Lot; FOUP)�� ������ ��ü�� ���� ���� ����
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_IsBatchLocOccupied(PTCHAR loc_id);
/*
 desc : ���� Wafer�� ��Ʈ (Lot; FOUP)�� ������ ��ü�� ���� ���� ����
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		state	- [in]  Batch State �� ��, ���� (1), ���� ���� (0)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetBatchLocState(PTCHAR loc_id, E90_SSLS state);
/*
 desc : ���� Wafer�� ��Ʈ (Lot; FOUP)�� ����(���)�� Substrate ID ��� ��ȯ
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [out] loc_id�� ��ġ�� Wafer�� ��Ʈ (Lot; FOUP)�� Substrate ID ����� ����� ����Ʈ ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetBatchSubstIDMap(PTCHAR loc_id, CStringArray &subst_id);
/*
 desc : ���� Wafer�� ��Ʈ (Lot; FOUP)�� ����(���)�� Substrate ID ��� ���� (���)
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [in]  loc_id�� ��ġ�� Wafer�� ��Ʈ (Lot; FOUP)�� Substrate ID ����� ����� ����Ʈ ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetBatchSubstIDMap(PTCHAR loc_id, CStringArray *subst_id);
/*
 desc : Location ID�� ���� �̺�Ʈ�� ������ ���ο� ���� ���� ���
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		flag	- [out] FALSE : �̺�Ʈ ���� (�˸�), TRUE : �̺�Ʈ ������ ���� (�˸��� ����)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetDisableEventsLoc(PTCHAR loc_id, BOOL &flag);
/*
 desc : Location ID�� ���� �̺�Ʈ�� ������ ���ο� ���� ���� ����
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		flag	- [out] FALSE : �̺�Ʈ ���� (�˸�), TRUE : �̺�Ʈ ������ ���� (�˸��� ����)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetDisableEventsLoc(PTCHAR loc_id, BOOL flag);
/*
 desc : Batch ID�� ���� �̺�Ʈ�� ������ ���ο� ���� ���� ���
 parm : batch_id- [in]  ID of substrate batch
		flag	- [out] FALSE : �̺�Ʈ ���� (�˸�), TRUE : �̺�Ʈ ������ ���� (�˸��� ����)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetDisableEventsBatch(PTCHAR batch_id, BOOL &flag);
/*
 desc : Batch ID�� ���� �̺�Ʈ�� ������ ���ο� ���� ���� ����
 parm : batch_id- [in]  ID of substrate batch
		flag	- [out] FALSE : �̺�Ʈ ���� (�˸�), TRUE : �̺�Ʈ ������ ���� (�˸��� ����)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetDisableEventsBatch(PTCHAR batch_id, BOOL flag);
/*
 desc : Batch (Wafer Collections)�� ����� �� �ִ� �ִ� ���� ��ȯ
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		max_pos	- [in]  �ִ� ��� ������ Substrate position ���� ��ȯ�� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetMaxSubstratePosition(PTCHAR loc_id, LONG &max_pos);
/*
 desc : Batch (Wafer Collections)�� ����� �� �ִ� �ִ� ���� ��ȯ
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		name	- [out] Object Name�� ����� ����
		size	- [in]  'name' ������ ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetObjectName(PTCHAR loc_id, PTCHAR name, UINT32 size);
/*
 desc : Update <batch location> GEM data (Batch Location (��ġ) GEM Data ������ �˸�?)
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
 note : Updates the batch location status variables
*/
API_IMPORT BOOL uvCIMLib_E90STS_SendDataNotificationBatch(PTCHAR loc_id);
/*
 desc : Update <substrate location> GEM variables for this object
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
 note : GEM ���� - SubstID, SubstLocID, SubstLocState, SubstLocState_i, and SubstLocSubstID_i
*/
API_IMPORT BOOL uvCIMLib_E90STS_SendDataNotificationSubst(PTCHAR loc_id);
/*
 desc : Substrate Batch Location ID ����
 parm : loc_id	- [in]  �����ϰ��� �ϴ� Batch Location ID (���ڿ�)
		loc_num	- [in]  Batch Location���� substrate locations�� ���� (Carrier ���� ���? ������ substrate location ����)
		sub_id	- [in]  Location Group ID�� ���� ����ϴ� Subscript Index (location's index)
						EPJ ���Ͽ��� �ش� ��ġ�� ���Ǵ� ÷��?(Subscripted) ������ ��Ʈ (set) -> EPJ ���Ͽ� ���ǵ� ����� �������� ������...
						������� ���� ���� 0 ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_AddSubstrateBatchLocation(PTCHAR loc_id, INT32 loc_num, INT16 sub_id=0);
/*
 desc : Substrate Batch Location ID ����
 parm : loc_id	- [in]  �����ϰ��� �ϴ� Batch Location ID (���ڿ�)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_DelSubstrateBatchLocation(PTCHAR loc_id);
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
API_IMPORT BOOL uvCIMLib_E90STS_AddCarrierToBatch(LONG batch_id, PTCHAR carr_id, LONG slot_map=-1);
/*
 desc : Batch�� ĳ������ ������ ���� ��
 parm : batch_id- [in]  ID of substrate batch
		carr_id	- [in]  ID of carrier
 retn : TRUE or FALSE
 note : Batch	- ���μ��� �ڿ����� ���ÿ� ó���� ������ �׷� (��ġ?)
*/
API_IMPORT BOOL uvCIMLib_E90STS_DelCarrierFromBatch(LONG batch_id, PTCHAR carr_id);
/*
 desc : ���ο� Substrate Batch ����
 parm : batch_id- [in]  ID of substrate batch
		loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_CreateBatch(LONG batch_id, PTCHAR loc_id);
/*
 desc : ���� Substrate Batch ����
 parm : batch_id- [in]  ID of substrate batch
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_DestroyBatch(LONG batch_id);
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
API_IMPORT BOOL uvCIMLib_E90STS_ChangeBatchState(LONG batch_id, PTCHAR loc_id,
												 E90_SSTS trans_state, E90_SSPS proc_state,
												 PTCHAR time_in=NULL, PTCHAR time_out=NULL);
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
API_IMPORT BOOL uvCIMLib_E90STS_AddSubstrateLocation(PTCHAR loc_id, PTCHAR subst_id, INT16 sub_id);
/*
 desc : Substrate Location ����
 parm : loc_id	- [in]  ��ġ �����ϰ��� �ϴ� ID (Location ID: Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_DelSubstrateLocation(PTCHAR loc_id);
/*
 desc : Batch�� Substrate �߰�
 parm : batch_id- [in]  ID of substrate batch
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
 retn : TRUE or FALSE
 note : Batch	- ���μ��� �ڿ����� ���ÿ� ó���� ������ �׷� (��ġ?)
*/
API_IMPORT BOOL uvCIMLib_E90STS_AddSubstrateToBatch(LONG batch_id, PTCHAR subst_id);
/*
 desc : Batch���� Substrate ����
 parm : batch_id- [in]  ID of substrate batch
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
 retn : TRUE or FALSE
 note : Batch	- ���μ��� �ڿ����� ���ÿ� ó���� ������ �׷� (��ġ?)
*/
API_IMPORT BOOL uvCIMLib_E90STS_DelSubstrateFromBatch(LONG batch_id, PTCHAR subst_id);
/*
 desc : Batch�� ����ִ� substrate ������ �߰� ��, Substrate�� ���� Batch�� �߰�
 parm : batch_id- [in]  ID of substrate batch
 retn : TRUE or FALSE
 note : Batch	- ���μ��� �ڿ����� ���ÿ� ó���� ������ �׷� (��ġ?)
*/
API_IMPORT BOOL uvCIMLib_E90STS_AddEmptySpaceToBatch(LONG batch_id);
/*
 desc : subscript indices�� ������ ���� (?)
 parm : base_sub_index	- [in]  first subscript index to reserve
		reserve_size	- [in]  number of indices to reserve
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_ReserveSubstrateLocationIndices(INT16 base_sub_index,
																INT16 reserve_size);
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
API_IMPORT BOOL uvCIMLib_E90STS_ChangeSubstrateTransportState(PTCHAR subst_id, PTCHAR loc_id,
															  E90_SSTS state,
															  PTCHAR time_in=NULL, PTCHAR time_out=NULL);
/*
 desc : Substrate Material Status �� ��ȯ (������ ó�� ���� �� ��ȯ. ó�� ���� ?)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		status	- [out] Material Status (Material ó�� ����) ���� ��ȯ�� ���� ����
						��ġ���� ��ȯ�Ǵ� ���� �ٸ���? (EPJ ���� ����?)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateMaterialStatus(PTCHAR subst_id, LONG &status);
/*
 desc : Substrate Material Status �� ���� (������ ó�� ���� �� ����. ó�� ���� ?)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		status	- [in]  Material Status (Material ó�� ����) ��
						��ġ���� �����ϴ´� ���� �ٸ���? (EPJ ���� ����?)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateMaterialStatus(PTCHAR subst_id, LONG status);
/*
 desc : ��� �ٷ�� Substrate�� Type ���� (Wafer, Flat Panel, CD, Mask, etc...)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		type	- [in]  E90_EMST ��, wafer, flat_panel, cd, mask �� �ϳ�
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetCurrentSubstrateType(PTCHAR subst_id, E90_EMST type);
/*
 desc : ��� �ٷ�� Substrate�� Type ���� (Wafer, Flat Panel, CD, Mask, etc...)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		type	- [out] E90_EMST ��, wafer, flat_panel, cd, mask �� �ϳ�
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetCurrentSubstrateType(PTCHAR subst_id, E90_EMST &type);
/*
 desc : Substrate ID�� Reader��κ��� ���������� �о������� ��, Host�κ��� Confirm�� �޾Ҵ��� ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		read	- [out] ���������� ID�� �о����� ���� �� (TRUE or FALSE)�� ��ȯ�� ���� ����
 note : Equipment������ NotifySubstrateRead �Լ� ȣ���ؼ� Substrate ID�� ���������� Host���� ��û
		�� NotifySubstrateRead �Լ� ȣ�� �Ŀ�, ���� �Լ� ȣ���ϸ� goodRead���� badRead���� �� �� ����
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateGoodRead(PTCHAR subst_id, BOOL &read);
/*
 desc : Substrate ID�� Reader��κ��� �о���� ���� �� ��, TRUE (good) or FALSE (bad) �� ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		read	- [in]  ���������� ID�� �о��ٸ� TRUE, �ƴ϶�� FALSE ����
 note : Equipment������ NotifySubstrateRead �Լ� ȣ���ؼ� Substrate ID�� ���������� Host���� ��û
		�� NotifySubstrateRead �Լ� ȣ�� �Ŀ�, ���� �Լ� ȣ���ϸ� goodRead���� badRead���� �� �� ����
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateGoodRead(PTCHAR subst_id, BOOL read);
/*
 desc : Substrate ID�� ���õ� Lot ID ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		lot_id	- [out] Lot ID�� ����� ����
		size	- [in]  'lot_id' ���� ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateLotID(PTCHAR subst_id, PTCHAR lot_id, UINT32 size);
/*
 desc : Substrate ID�� ���õ� Lot ID ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		lot_id	- [in]  Lot ID�� ����� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateLotID(PTCHAR subst_id, PTCHAR lot_id);
/*
 desc : Substrate ID�� ���õ� Lot ID ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		obj_id	- [out] Object ID�� ����� ���� (���⼭�� Substrate ID�� ������ �ؽ�Ʈ?)
		size	- [in]  'lot_id' ���� ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateObjID(PTCHAR subst_id, PTCHAR obj_id, UINT32 size);
/*
 desc : Substrate ID�� ���õ� Object Type ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		obj_type- [out] Object Type�� ����� ���� ("BatchLoc" or "Substrate" or "SubstLoc")
						���⼭�� ���� "Substrate"
		size	- [in]  'obj_type' ���� ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateObjType(PTCHAR subst_id, PTCHAR obj_type, UINT32 size);
/*
 desc : Location ID�� ���õ� Object ID ��ȯ
 parm : loc_id	- [out] Location ID�� ����� ����
		obj_id	- [out] Object ID�� ����� ���� (���⼭�� Location ID�� ������ �ؽ�Ʈ?)
		size	- [in]  'lot_id' ���� ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetLocationObjID(PTCHAR loc_id, PTCHAR obj_id, UINT32 size);
/*
 desc : Location ID�� ���õ� Object Type ��ȯ
 parm : loc_id	- [out] Location ID�� ����� ����
		obj_type- [out] Object Type�� ����� ���� ("BatchLoc" or "Substrate" or "SubstLoc")
						���⼭�� ���� "Substrate"
		size	- [in]  'obj_type' ���� ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetLocationObjType(PTCHAR loc_id, PTCHAR obj_type, UINT32 size);
/*
 desc : Substrate�� ���� Source CarrierID ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		carr_id	- [out] sourceCarrierID�� ����� ����
		size	- [in]  'carr_id' ���� ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSourceCarrierID(PTCHAR subst_id, PTCHAR carr_id, UINT32 size);
/*
 desc : Substrate�� ���� Source CarrierID ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		carr_id	- [in]  sourceCarrierID�� ����� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSourceCarrierID(PTCHAR subst_id, PTCHAR carr_id);
/*
 desc : Substrate�� ���� Source LoadPort ID ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		port_id	- [out] Load Port ID�� ����� ���� ���� (1 or Later)
 retn : TRUE or FALSE
 note : Load Port - the interface location on the equipment where carriers are loaded and unloaded
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSourceLoadPortID(PTCHAR subst_id, LONG &port_id);
/*
 desc : Substrate�� ���� Source LoadPort ID ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		port_id	- [in]  sourceLoadPortID�� ����� ���� (1 or Later)
 retn : TRUE or FALSE
 note : Load Port - the interface location on the equipment where carriers are loaded and unloaded
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSourceLoadPortID(PTCHAR subst_id, LONG port_id);
/*
 desc : Substrate�� ���� Source Slot ID ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		port_id	- [out] Slot ID�� ����� ���� ���� (1 or Later)
 retn : TRUE or FALSE
 note : slot - physical location capable of holding a substrate within cassette type carrier
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSourceSlotID(PTCHAR subst_id, LONG &slot_id);
/*
 desc : Substrate�� ���� Source Slot ID ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		port_id	- [in]  sourceSlotID�� ����� ���� (1 or Later)
 retn : TRUE or FALSE
 note : slot - physical location capable of holding a substrate within cassette type carrier
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSourceSlotID(PTCHAR subst_id, LONG slot_id);
/*
 desc : Substrate�� ������ �̺�Ʈ (�۾�)�� ���õ� Substrate�� ���� ��ġ ��ȯ
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		loc_id	- [out] Location ID�� ����� ����
		size	- [in]  'loc_id' ���� ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetCurrentSubstrateLocID(PTCHAR subst_id, PTCHAR loc_id, UINT32 size);
/*
 desc : Substrate�� ������ �̺�Ʈ (�۾�)�� ���õ� Substrate�� ���� ��ġ ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		loc_id	- [in]  Location ID�� ����� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetCurrentSubstrateLocID(PTCHAR subst_id, PTCHAR loc_id);
/*
 desc : Substrate�� Source ID ���
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		src_id	- [out] Source ID�� ����� ����
		size	- [in]  'src_id' ���� ũ��
 retn : TRUE or FALSE
 note : Substrate ID ��� ��� - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateSourceID(PTCHAR subst_id, PTCHAR src_id, UINT32 size);
/*
 desc : Substrate�� Source ID ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		src_id	- [in]  Source ID�� ����� ����
 retn : TRUE or FALSE
 note : Substrate ID ��� ��� - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateSourceID(PTCHAR subst_id, PTCHAR src_id);
/*
 desc : Substrate�� Destination ID ���
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		tgt_id	- [out] Source ID�� ����� ����
		size	- [in]  'src_id' ���� ũ��
 retn : TRUE or FALSE
 note : Substrate ID ��� ��� - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateDestinationID(PTCHAR subst_id, PTCHAR tgt_id, UINT32 size);
/*
 desc : Substrate�� Destination ID ����
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		tgt_id	- [in]  Source ID�� ����� ����
 retn : TRUE or FALSE
 note : Substrate ID ��� ��� - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateDestinationID(PTCHAR subst_id, PTCHAR tgt_id);
/*
 desc : ������ ���� (Transition)�� ����Ǵ� ���� ��ü �׷쿡 ���� ���۵Ǵ� ���� �̺�Ʈ�� �����ϴµ� ���
		��ü (Object)�� ���� �̺�Ʈ�� �����ϴ��� ���� Ȯ��
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		flag	- [out] TRUE : �̺�Ʈ ���� (�˸�), FALSE : �̺�Ʈ ������ ���� (�˸��� ����)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetUsingGroupEventsSubst(PTCHAR subst_id, BOOL &flag);
/*
 desc : ������ ���� (Transition)�� ����Ǵ� ���� ��ü �׷쿡 ���� ���۵Ǵ� ���� �̺�Ʈ�� �����ϴµ� ���
		��ü (Object)�� ���� �̺�Ʈ�� ���� Ȥ�� �����ϴ��� ���� Ȯ��
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber�� ����)
		flag	- [in]  TRUE : �̺�Ʈ ���� (�˸�), FALSE : �̺�Ʈ ������ ���� (�˸��� ����)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetUsingGroupEventsSubst(PTCHAR subst_id, BOOL flag);
/*
 desc : ������ ���� (Transition)�� ����Ǵ� ���� ��ü �׷쿡 ���� ���۵Ǵ� ���� �̺�Ʈ�� �����ϴµ� ���
		��ü (Object)�� ���� �̺�Ʈ�� �����ϴ��� ���� Ȯ��
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		flag	- [out] TRUE : �̺�Ʈ ���� (�˸�), FALSE : �̺�Ʈ ������ ���� (�˸��� ����)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SGetUsingGroupEventsLoc(PTCHAR loc_id, BOOL &flag);
/*
 desc : ������ ���� (Transition)�� ����Ǵ� ���� ��ü �׷쿡 ���� ���۵Ǵ� ���� �̺�Ʈ�� �����ϴµ� ���
		��ü (Object)�� ���� �̺�Ʈ�� ���� Ȥ�� �����ϴ��� ���� Ȯ��
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		flag	- [in]  TRUE : �̺�Ʈ ���� (�˸�), FALSE : �̺�Ʈ ������ ���� (�˸��� ����)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetUsingGroupEventsLoc(PTCHAR loc_id, BOOL flag);
/*
 desc : Location�� ��ġ�� Substrate ID ���
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [out] Substrate ID�� ����� ����
		size	- [in]  'subst_id' ���� ũ��
 retn : TRUE or FALSE
*/															   
API_IMPORT BOOL uvCIMLib_E90STS_GetLocationSubstID(PTCHAR loc_id, PTCHAR subst_id, UINT32 size);
/*
 desc : Location�� ��ġ�� Substrate ID ����
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [in]  Substrate ID�� ����� ����
 retn : TRUE or FALSE
*/															   
API_IMPORT BOOL uvCIMLib_E90STS_SetLocationSubstID(PTCHAR loc_id, PTCHAR subst_id);
/*
 desc : Location�� ��ġ�� Location Index ���
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		index	- [out] Location Index�� ��ȯ�� ���� ���� (1 or Later)
 retn : TRUE or FALSE
*/															   
API_IMPORT BOOL uvCIMLib_E90STS_GetLocationIndex(PTCHAR loc_id, INT16 &index);
/*
 desc : Location�� ��ġ�� Substrate ID ����
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		index	- [in]  Location Index �� (1 or Later)
 retn : TRUE or FALSE
*/															   
API_IMPORT BOOL uvCIMLib_E90STS_SetLocationIndex(PTCHAR loc_id, INT16 index);
/*
 desc : Substrate History ��ȯ
 parm : subst_id- [in]  Substrate ID�� ����� ����
		lst_hist- [out] { {loc_id, time_in, time_out}, {loc_id, time_in, time_out}, ..., } ���� ��ȯ�� ����Ʈ
						time_in/out = YearMonthDayHourMinuteSecondCentisecond
 retn : TRUE or FALSE
 ��¥ : 2021.03.08 (Modified by Jinsoo.Kang at philoptics)
 note : N/A
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateHistory(PTCHAR subst_id, CAtlList <STG_CSHI> &lst_hist);
/*
 desc : Substrate History ��ȯ
 parm : subst_id- [in]  Substrate ID�� ����� ����
		lst_hist- [in]  { {loc_id, time_in, time_out}, {loc_id, time_in, time_out}, ..., } ���� ����� ����Ʈ
						time_in/out = YearMonthDayHourMinuteSecondCentisecond
 retn : TRUE or FALSE
 ��¥ : 2021.03.08 (Modified by Jinsoo.Kang at philoptics)
 note : N/A
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateHistory(PTCHAR subst_id, CAtlList <STG_CSHI> *lst_hist);


/* --------------------------------------------------------------------------------------------- */
/*                             E94 Control Job Management (CJM)                                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Process Job�� �߿��� �� ���� �Ϸ�Ǹ� Control Job���� �˸�
 parm : pj_id	- [in]  Object ID of the Process Job
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_SetPRJobProcessComplete(PTCHAR pj_id);
/*
 desc : ���� Queue�� ��ϵ� Control Job ID ��� �� �� ���� ���� ��ȯ
 parm : list_job	- [out] ��ȯ�Ǿ� ����� Control Job ID (String)
		list_state	- [out] ������ Control Job IDs�� ���� ���� ���� ����� ����Ʈ
		logs		- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobList(CStringArray &list_job,
												  CAtlList<E94_CCJS> &list_state);
/*
 desc : Process Jobs List ��� (Control Job ���ο� ����� Project Job List)
 parm : list_job- [out] ������ Control Job IDs (Names) ����� �迭 ����
		filter	- [in]  Control Job ���� ���� (����)
 retn : TRUE ro FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobListId(CStringArray &list_job,
													E94_CLCJ filter=E94_CLCJ::en_list_all);
/*
 desc : Process Jobs List ��� (���͸� ����)
 parm : list_job	- [out] ������ Control Job IDs (Names) ����� �迭 ����
		list_state	- [out] ������ Control Job IDs�� ���� ���� ���� ����� ����Ʈ
		filter		- [in]  Control Job ���� ���� (����)
		logs		- [in]  �α� ��� ����
 retn : TRUE ro FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobListIdState(CStringArray &list_job,
														 CAtlList<E94_CCJS> &list_state,
														 E94_CLCJ filter=E94_CLCJ::en_list_all);
/*
 desc : Control Job ID �� ���Ϳ� ���� �˻��� �� �� ù ��° �˻��� Job ID ��ȯ
 parm : filter	- [in]  Process Job ID �˻� ���� (E40_PPJS)
		cj_id	- [out] ��ȯ�Ǿ� ����� Control Job ID (String)
		size	- [in]  'cj_id' ���� ũ��
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobFirstID(E94_CCJS filter, PTCHAR cj_id, UINT32 size);
/*
 desc : Control Job�� ���� ���� ��ȯ
 parm : cj_id	- [in]  Control Job ID
		state	- [out] Job ID�� ���� �� ��ȯ
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobState(PTCHAR cj_id, E94_CCJS &state);
/*
 desc : Control Job ����
 parm : cj_id		- [in]  Control Job ID
		carr_id		- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		pj_id		- [in]  Process Job List �� ����� Array �迭
		job_order	- [in]  Job Order (E94_CPOM ��, 3: List, 1: Arrival)
		start		- [in]  Auto Start ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_CreateJob(PTCHAR cj_id, PTCHAR carr_id, CStringArray *pj_id,
										  E94_CPOM job_order, BOOL start);
/*
 desc : Process Jobs List ��� (Control Job ���ο� ����� Project Job List)
 parm : cj_id		- [in]  Object ID of the Control Job
		list_job	- [out] ������ Process Job Names ����� �迭 ����
		list_state	- [out] ������ Control Job IDs (Names)�� State�� ����� ���� ����
		filter		- [in]  Control Job ���ο� ����� Process Job ���� ���� (����)
		logs		- [in]  �α� ��� ����
 retn : TRUE ro FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetProcessJobListIdState(PTCHAR cj_id,
														 CStringArray &list_job,
														 CAtlList <E40_PPJS> &list_state,
														 E94_CLPJ filter);
/*
 desc : Process Jobs List ��� (Control Job ���ο� ����� Project Job List)
 parm : cj_id	- [in]  Object ID of the Control Job
		list_job- [out] ������ Process Job Names ����� �迭 ����
		filter	- [in]  Control Job ���ο� ����� Process Job ���� ���� (����)
		logs	- [in]  �α� ��� ����
 retn : TRUE ro FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetProcessJobList(PTCHAR cj_id, CStringArray &list_job,
												  E94_CLPJ filter=E94_CLPJ::en_list_all);
/*
 desc : Control Job�� �����ϱ�
 parm : cj_id		- [in]  Control Job ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_SetSelectedControlJob(PTCHAR cj_id);
/*
 desc : Control Job�� ���� �����ϱ�
 parm : cj_id		- [in]  Control Job ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_SetDeselectedControlJob(PTCHAR cj_id);
/*
 desc : Remote Control ��� ���� �� ��ȯ
 parm : enable	- [out] Enable or Disable
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetAllowRemoteControl(BOOL &enable);
/*
 desc : Remote Control ��� ���� ����
 parm : enable	- [in]  Enable or Disable
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_SetAllowRemoteControl(BOOL enable);
/*
���� : Control Job ���ο��� ���� ProcessJob�� ���۽�Ų��.
���� : cj_id	- [in]  Control Job ID
��ȯ : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_StartNextProcessJob(PTCHAR cj_id);
/*
���� : ���� Control Job�� ���۽�Ų��.
���� : cj_id	- [in]  Control Job ID
��ȯ : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_StartNextControlJob();

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
API_IMPORT BOOL uvCIMLib_E116EPT_SetBusy(PTCHAR mod_name, PTCHAR task_name, E116_ETBS task_type);
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
API_IMPORT BOOL uvCIMLib_E116EPT_SetIdle(PTCHAR mod_name);

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
API_IMPORT BOOL uvCIMLib_E157MPT_MPTExecutionStarted(PTCHAR mod_name, PTCHAR pj_id, PTCHAR rp_id, PTCHAR subst_id);
/*
 desc : Module Process Tracking ���� �Ϸ� (������ ���� �Ϸ�)
 parm : mod_name- [in]  Name of the EPT module, use an epty string for the Equipemnt ETP state
						ex> ProcessChamber, RobotArm, PreAligner, Buffer, etc
		exec_ok	- [in]  TRUE : Execution success, FALSE : Execution failed
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E157MPT_MPTExecutionCompleted(PTCHAR mod_name, BOOL exec_ok);
/*
 desc : Module Process Tracking �ܰ� ����
 parm : mod_name- [in]  Name of the EPT module, use an epty string for the Equipemnt ETP state
						ex> ProcessChamber, RobotArm, PreAligner, Buffer, etc
		step_id	- [in]  Step ID (Name)
						ex> LithographStep1 ~ LithographStep??
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E157MPT_MPTStepStarted(PTCHAR mod_name, PTCHAR step_id);
/*
 desc : Module Process Tracking �ܰ� �Ϸ�
 parm : mod_name- [in]  Name of the EPT module, use an epty string for the Equipemnt ETP state
						ex> ProcessChamber, RobotArm, PreAligner, Buffer, etc
		step_ok	- [in]  TRUE : Step success, FALSE : step failed
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E157MPT_MPTStepCompleted(PTCHAR mod_name, BOOL step_ok);


/* --------------------------------------------------------------------------------------------- */
/*                                         Cimetrix Area                                         */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Control Job ID�� ��ϵǾ� �ִ� Carrier ID List ��ȯ
 parm : cj_id	- [in]  Control Job ID
		lst_carr- [out] Carrier List�� ��ȯ�� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobCarrierInputSpec(PTCHAR cj_id, CStringList &lst_carr);
/*
 ���� :
 parm : cj_id	- [in]  Control Job ID
		lst_carr- [out] Carrier List ��ȯ�� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobCarrierOutputSpec(PTCHAR cj_id, CStringList &lst_carr);
/*
 desc : Control Job ID�� ���Ե� Process Job ID List ��ȯ
 parm : cj_id	- [in]  Control Job ID
		lst_pj	- [out] Process Job List ��ȯ�� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobProcessJobList(PTCHAR cj_id, CStringList &lst_pj);
/*
 desc : Control Job�� �ڵ� ���� ���� Ȯ��
 parm : cj_id		- [in]  Control Job ID
		auto_start	- [out] Auto Start ���� �� ��ȯ
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobStartMethod(PTCHAR cj_id, BOOL &auto_start);
/*
 desc : Control Job�� �۾� �Ϸ� �ð� ���
 parm : cj_id		- [in]  Control Job ID
		date_time	- [out] �۾� �Ϸ� �ð� ���ڿ��� ��ȯ
		size		- [in]  'data_time' ���� ũ��
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobCompletedTime(PTCHAR cj_id, PTCHAR date_time, UINT32 size);
/*
 desc : Control Job�� Material ���� ���� ���
 parm : cj_id	- [in]  Control Job ID
		arrived	- [out] TRUE : ����, FALSE : �̵���
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobMaterialArrived(PTCHAR cj_id, BOOL &arrived);
/*
 desc : Control Job�� ���� ����
 parm : cj_id	- [in]  Control Job ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_SetStartControlJob(PTCHAR cj_id);
/*
 desc : Control Job�� ���� (�۾��� �Ϸ�Ǿ��ų� ���� �߰ų� �� �� ���, ���� ��Ŵ) �˸�
 parm : cj_id	- [in]  Control Job ID
		action	- [in]  �۾� ���� Ȥ�� ���� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_SetStopControlJob(PTCHAR cj_id, E94_CCJA action);
/*
 desc : Control Job�� �ߴ� (�۾� ���� �߿� �۾��� �ƿ� ���� ���ѹ���) �˸�
 parm : cj_id	- [in]  Control Job ID
		action	- [in]  �۾� ���� Ȥ�� ���� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_SetAbortControlJob(PTCHAR cj_id, E94_CCJA action);
/*
 desc : Control Job�� ��� ���� (�Ͻ� ����. ��� / �ߴ� �� �ƴ�) �˸�
 parm : cj_id	- [in]  Control Job ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_SetPauseControlJob(PTCHAR cj_id);
/*
 desc : Control Job�� ����� (���� ��������) �˸�
 parm : cj_id	- [in]  Control Job ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_SetResumeControlJob(PTCHAR cj_id);
/*
 desc : Control Job�� ��� (�������� ���� ���¿���... �ƿ� �۾��� ���� ����) �˸�
 parm : cj_id	- [in]  Control Job ID
		action	- [in]  �۾� ���� Ȥ�� ���� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_SetCancelControlJob(PTCHAR cj_id, E94_CCJA action);
/*
 desc : Control Job�� Queue�� �� ������ �̵� ��Ŵ�� �˸�
 parm : cj_id	- [in]  Control Job ID
 retn : TRUE or FALSE
 note : Moves the specified control job to the top of the queue.
		Other control jobs in the queue are pushed back.
		The rest of the queue order remains unchanged.
*/
API_IMPORT BOOL uvCIMLib_E94CJM_SetHOQControlJob(PTCHAR cj_id);

/* --------------------------------------------------------------------------------------------- */
/*                     E30 / E40 / E87 / E90 / E94 / E116 / E157 : Mixed Area                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Carrier ���� Slot Number�� ���� Destination Substrate ID ���
 parm : carr_id	- [in]  ���ڿ� ID (�������� �����Ǵ� ID)
		slot_no	- [in]  Slot Number (Not sequential number)
		subst_id- [out] Substrate ID (SourceCarrierID_SlotNumber�� ����)
		size	- [in]  'st_id' ���� ũ��
		logs	- [in]  �α� ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Mixed_GetSubstrateDestinationID(PTCHAR carr_id, UINT8 slot_no,
														 PTCHAR subst_id, UINT32 size);
/*
 desc : Carrier ���� ����� Slot Number (No)�� ���� Substrate ID �� ��ȯ
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		slot_no	- [in]  Slot Number (1-based or Later)
		subst_id- [out] Substrate ID (SourceCarrierID_SlotNumber�� ����)
		size	- [in]  'st_id' ���� ũ��
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Mixed_GetSubstrateID(PTCHAR carr_id, UINT8 slot_no, PTCHAR subst_id,
											  UINT32 size);
/*
 desc : Control Job ID�� ��ϵǾ� �ִ� Ư�� Process Job ID�� �۾��� ��� �������� ����
 parm : pj_id	- [in]  Process Job ID
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE (Completed) or FALSE (Not complete)
*/
API_IMPORT BOOL uvCIMLib_Mixed_IsSubstrateProcessCompleted(PTCHAR pj_id);
/*
 desc : Carrier ���� ��ϵǾ� �־��� Process Job ID ���� ��� SubstrateID�� Transport ��ġ��
		Destination (FOUP; Carrier)�� �ִ��� ����
 parm : pj_id	- [in]  Process Job ID
		logs	- [in]  �α� ������ ��� ����
 retn : TRUE (Inside the destination) or FALSE (Outside the destination)
*/
API_IMPORT BOOL uvCIMLib_Mixed_IsSubstrateTransportDestination(PTCHAR pj_id);
/*
 desc : Process Job ID�� �۾��� ���� ��, ���� Process Job ID���� �ڵ����� Completed ��Ŵ
 parm : pj_id	- [in]  Process Job ID
 retn : TRUE or FALSE
 note : Process Job ID�� 1���� ������ �ִ� Control Job ID�� ���� �� �Լ� ȣ���� ��ȿ �մϴ�.
*/
API_IMPORT BOOL uvCIMLib_Mixed_SetAutoJobCompleted(PTCHAR pj_id);
/*
 desc : E40PJM::PRCommandCbk ȣ��� ��, ����ϱ� ���� SubstateIDs�� ���� ������ ������ �α�
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Mixed_UpdateSubstrateIDsState();
/*
 desc : CJ Paused�Ǿ��� �� PJ���� Pause��Ų��.
 parm : cj_id	- [in] Control Job ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Mixed_PauseProcessJobs(PTCHAR cj_id);
/*
 desc : CJ�� Executing�Ǿ��� �� PJ�� ó���� �簳�Ѵ�.
 parm : cj_id	- [in] Control Job ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Mixed_ResumePausedProcessJobs(PTCHAR cj_id);
/*
 desc : CJ�� queued�Ǿ��� �� Material Out Spec�� �����Ѵ�.
 parm : cj_id	- [in] Control Job ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Mixed_UpdateMtrlOutSpecSubstrateDestination(PTCHAR cj_id);
/*
 desc : Carrier SlotMap Verified�� ��, Material Out Spec�� �����Ѵ�.
 parm : carr_id	- [in] Carrier ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Mixed_UpdateMtrlOutSpecSubstrateDestinationByE87(PTCHAR carr_id);
/*
 desc : ��� CJ�� Material Out Spec�� �����Ѵ�.
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Mixed_UpdateMtrlOutSpecSubstrateDestinationAll();
/*
 desc : Process Job�� �ش��ϴ� substrate state�� ��� Ȯ���ϰ�
		__en_e90_substrate_processing_state__ �� en_ssps_needs_processing �� ���
		��� en_ssps_skipped�� ������
 parm : pj_id	- [in]  Process Job ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Mixed_SkipSubstratesNeedingProcessing(PTCHAR pj_id);
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
API_IMPORT BOOL uvCIMLib_Mixed_CheckJobComplete(PTCHAR pj_id);
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
API_IMPORT BOOL uvCIMLib_Mixed_CheckStartNextJob(PTCHAR pj_id);
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
API_IMPORT BOOL uvCIMLib_Mixed_CheckProcessJobProcessingComplete(PTCHAR pj_id);
#ifdef __cplusplus
}
#endif
