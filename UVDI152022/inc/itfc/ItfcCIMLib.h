/*
 desc : CIM Library 와 상위 응용 프로그램 간의 GEM300 관련 데이터 및 기타 동작 정보 공유 라이브러리
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
 desc : 라이브러리 초기화
 parm : work_dir	- [in]  실행파일이 존재하는 경로 (프로세스가 동작되는 메인 경로)
		conn_cnt	- [in]  EPJ 프로젝트 파일에 등록된 CONNECTIONx 개수
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Init(PTCHAR work_dir, UINT8 conn_cnt=1);
/*
 desc : 라이브러리 해제
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT VOID uvCIMLib_Close();
/*
 desc : CIM300 Connect 연결 (초기화) 해제 실행
 parm : epj_file	- [in]  EPJ 파일 이름 (경로 제외)
 retn : None
*/
API_IMPORT VOID uvCIMLib_Stop();
/*
 desc : TS (Tracking System)에 등록된 Equipment Module (Location) Name 반환
 parm : index	- [in]  장비 명과 매핑된 인덱스 값 (1 based)
 retn : 이름 문자열 (실패의 경우 NULL or Empty String)
*/
API_IMPORT PTCHAR uvCIMLib_GetEquipLocationName(ENG_TSSI index);
/*
 desc : 현재 적재되어 있는 프로젝트 정보를 파일로 저장
 parm : epj_file	- [in]  저장하고자 하는 파일 이름 (전체 경로 포함)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SaveProjetsToEPJ(PTCHAR epj_file);
/*
 desc : EMService가 동작 중인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsServiceRun();
/*
 desc : CIMConnection에 연결되어 있는지 여부
 parm : logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsCIMConnected();
/*
 desc : CIM Lib. 초기화 여부 반환
 parm : logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsInitedAll(BOOL logs);
/*
 desc : CIM300 Connect 연결 (초기화) 실행
 parm : equip_id	- [in]  Equipment ID (0 or later. 환경 파일에 설정된 개수-1)
		conn_id		- [in] 1 or Later (공통 적용을 위해서는 0 값 사용)
		epj_file	- [in]  EPJ 파일 (전체 경로)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Start(LONG equip_id, LONG conn_id, PTCHAR epj_file);
/*
 desc : CIM300 Connection (초기화 완료) 이후 Finalized 호출
 parm : equip_id	- [in]  Equipment ID (0 or later. 환경 파일에 설정된 개수-1)
		conn_id		- [in] 1 or Later (공통 적용을 위해서는 0 값 사용)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Finalized(LONG equip_id, LONG conn_id);
/*
 desc : 시스템에서 발생된 로그 반환
 parm : logs	- [in]  반환된 로그 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_PopLogs(STG_CLLM &logs);
/*
 desc : Recipe Path 설정
 parm : recipe	- [in]  Recipe Path (전체 경로 포함. 마지막에 '/' 제거)
 		conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		var_id	- [in]  Identification (Variables Name과 연결된 ID)
		var_name- [in]  Identification (Variables ID와 연결된 Name)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetRecipePathID(PTCHAR recipe, LONG conn_id, LONG var_id=2042);
API_IMPORT BOOL uvCIMLib_SetRecipePathName(PTCHAR recipe, LONG conn_id, PTCHAR var_name=L"RecipePath");
/*
 desc : MDLN 값 설정
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		name	- [in]  Equipment model type up to 20 characters
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetEquipModelType(LONG conn_id, PTCHAR name);
/*
 desc : SOFTREV 값 설정
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		name	- [in]  Equipment software revision ID up to 20 characters
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetEquipSoftRev(LONG conn_id, PTCHAR name);
/*
 desc : Recip Path 반환
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		path	- [out] Recipe Names (Files)이 저장된 경로가 반환될 버퍼
		size	- [in]  'path' 버퍼 크기
		var_id	- [in]  Identification (Variables Name과 연결된 ID)
		var_name- [in]  Identification (Variables ID와 연결된 Name)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetRecipePathID(LONG conn_id, PTCHAR path, UINT32 size, LONG var_id=2042);
API_IMPORT BOOL uvCIMLib_GetRecipePathName(LONG conn_id, PTCHAR path, UINT32 size, PTCHAR var_name=L"RecipePath");
/*
 desc : Recip Name (File)의 확장자 반환
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		ext		- [out] Recipe Name (File)의 확장자가 반환될 버퍼
		size	- [in]  'ext' 버퍼 크기
		var_id	- [in]  Identification (Variables Name과 연결된 ID)
		var_name- [in]  Identification (Variables ID와 연결된 Name)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetRecipeFileExtID(LONG conn_id, PTCHAR ext, UINT32 size, LONG var_id=2048);
API_IMPORT BOOL uvCIMLib_GetRecipFileExtName(LONG conn_id, PTCHAR ext, UINT32 size, PTCHAR var_name=L"RecipeExtension");
/*
 desc : EPJ ID에 연결된 Name 반환
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		name	- [in]  검색을 위한 Variable Name
		id		- [out] Vairable ID 반환 버퍼
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetNameToID(LONG conn_id, PTCHAR name, LONG &id);


/* --------------------------------------------------------------------------------------------- */
/*                                         Shared Data                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 현재 CIM300 (From Host)으로부터 발생된 이벤트 모두 해제
 parm : None
 retn : None
*/
API_IMPORT VOID uvCIMLib_RemoveEventAll();
/*
 desc : CIM300Cbk로부터 발생된 이벤트 반환
 parm : data	- [in]  반환된 이벤트 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_PopEvent(STG_CCED &event);
/*
 desc : Load Port의 Docked or Undocked Location Name 반환
 parm : port_id	- [in]  Load Port ID (1 or 2)
		docked	- [in]  0x00 : Docked, 0x01 : Undocked
 retn : Docked or Undocked Location Name 반환 (실패일 경우, NULL)
*/
API_IMPORT PTCHAR uvCIMLib_GetLoadPortDockedName(UINT16 port_id, UINT8 docked);
/*
 desc : CIM300 로그 레벨 설정
 parm : level	- [in]  Level Value 값
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetLogLevelAll(ENG_ILLV level);
/*
 desc : 레시피 적재 요청에 대한 값 초기화 (레시피 적재하지 않음)
 parm : None
 retn : None
*/
API_IMPORT VOID uvCIMLib_ResetPPLoadRecipeAck();
/*
 desc : 레시피 적재 요청에 대한 응답 값 반환
 parm : None
 retn : ANY Type
*/
API_IMPORT E30_HPLA uvCIMLib_GetPPLoadRecipeAck();
/*
 desc : 레시피 적재 요청에 대한 응답 값이 수신되었는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsPPLoadRecipeAckOk();
/*
 desc : 레시피 데이터 송신 (전달) 요청에 대한 응답 값 초기화
 parm : None
 retn : None
*/
API_IMPORT VOID uvCIMLib_ResetPPSendRecipeAck();
/*
 desc : 레시피 데이터 송신 (전달) 요청에 대해 응답 값 반환
 parm : None
 retn : ANY Type
*/
API_IMPORT E30_SRHA uvCIMLib_GetPPSendRecipeAck();
/*
 desc : 레시피 데이터 송신 (전달) 요청에 대해 응답 값이 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsPPSendRecipeAckOk();
/*
 desc : 터미널 메시지 송신 (전달) 요청에 대한 응답 값 초기화
 parm : None
 retn : None
*/
API_IMPORT VOID uvCIMLib_ResetTermMsgAck();
/*
 desc : 터미널 메시지 송신 (전달) 요청에 대한 응답 값 반환
 parm : None
 retn : ANY Type
*/
API_IMPORT E30_STHA uvCIMLib_GetTermMsgAck();
/*
 desc : 터미널 메시지 송신 (전달) 요청에 대해 응답이 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsTermMsgAckOk();
/*
 desc : Host로부터 Recipe File 요청 (관리)에 대한 응답 초기화
 parm : None
 retn : None
*/
API_IMPORT VOID uvCIMLib_ResetRecipeFileAck();
/*
 desc : Host로부터 Recipe File 요청 (관리)에 대한 응답 값 반환
 parm : None
 retn : ANY Type
*/
API_IMPORT E30_RMJC uvCIMLib_GetRecipeFileAck();
/*
 desc : 레시피 (프로세스 프로그램; Process Program == Recipe) 등록 / 삭제 / 전체 삭제
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		file	- [in]  레시피 이름 (전체 경로가 포함된 파일)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_RecipeAppend(LONG conn_id, PTCHAR file);
API_IMPORT BOOL uvCIMLib_RecipeRemove(LONG conn_id, PTCHAR file);
API_IMPORT VOID uvCIMLib_RecipeRemoveAll();
/*
 desc : 현재 등록된 Process Job ID 내의 Substrate IDs의 상태가 모두 입력된 상태와 동일한지 여부
 parm : pj_id	- [in]  검색 대상의 Process Job ID
		machine	- [in]  Substate Machine Type 값
		state	- [in]  비교 대상의 Substrate Processing State 값
 retn : 입력된 state 값과 모두 동일하다면 TRUE, 한 개 이상 다른 값이 존재한다면 FALSE
 */
API_IMPORT BOOL uvCIMLib_IsCheckedAllPRJobSubstState(PTCHAR pj_id, E90_VSMC machine, INT32 state);
/*
 desc : 현재 등록된 Process Job ID 내의 Substrate IDs의 상태가 입력된 상태와 동일한 값이 1개라도 있는지 여부
 parm : pj_id	- [in]  검색 대상의 Process Job ID
		machine	- [in]  Substate Machine Type 값
		state	- [in]  비교 대상의 Substrate Processing State 값
 retn : 입력된 state 값과 동일한 상태 값이 한 개라도 존재하면 TRUE
		입력된 state 값과 동일한 상태 값이 한 개라도 없다면 FALSE
 */
API_IMPORT BOOL uvCIMLib_IsCheckedOnePRJobSubstState(PTCHAR pj_id, E90_VSMC machine, INT32 state);
/*
 desc : 현재 장치에 등록된 PRJob IDs에 등록된 Substrate IDs의 모두 상태 정보들 반환
 parm : pj_id	- [in]  검색 대상의 Process Job ID
		subst	- [out] Substrate ID의 상태 및 기타 정보들이 저장될 리스트 메모리 (PRCommandCbk 함수에서 주로 사용됨?)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetSubstIDStateAll(PTCHAR pj_id, CAtlList <STG_CSDI> &lst_state);
/*
 desc : CJobID 제거
 parm : carr_id	- [in]  제거 대상인 Carrier ID
 retn : TRUE (제거 성공), FALSE (제거 실패)
*/
API_IMPORT BOOL uvCIMLib_RemoveSharedCarrID(PTCHAR carr_id);
/*
 desc : 기존 등록된 SECS-II Message 반환
 parm : mesg_id	- [in]  검색 대상의 Message ID 값 (Stream/Function으로 구성된 16진수 Hexa-Decimal 값)
		remove	- [in]  검색된 대상을 반환 후 메모리에서 제거할지 여부
		item	- [out] 검색된 데이터가 반환될 리스트 버퍼
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetSecsMessage(LONG mesg_id, BOOL remove, CAtlList <STG_TSMV> &item);
/* 
 desc : 기존 등록된 모든 SECS-II Message 메모리 해제
 parm : None
 retn : None
*/
API_IMPORT VOID uvCIMLib_RemoveAllSecsMessage();


#if 0	/* 현재 할 필요 없음. 나중에 판단함 */
/* --------------------------------------------------------------------------------------------- */
/*                         Operation Job Step [Sequencial Working]                               */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 현재 작업이 완료된 상태인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsWorkCompleted();
/*
 desc : 현재 작업이 실패된 상태인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsWorkFailed();
/*
 desc : 현재 작업이 대기 상태인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsIdledWork();
/*
 desc : E30GEM 초기화 진행
 parm : None
 retn : TRUE or FALSE
 note : EPJ 로딩 이후, 호출되어야 됨
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
 desc : 현재 Variable의 Value 값 얻기
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		var_id	- [in]  Variable ID
		var_name- [in]  Variable Name
		value	- [out] Value 값이 저장되는 버퍼 포인터
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID; 1 or 2)
		index	- [in]	일반적으로 0 값이지만, 리스트의 경우 Slot ID (Wafer ID; 1 ~ Max)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetNameValue(LONG conn_id, PTCHAR var_name, UNG_CVVT &value,
									  LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetIDValue(LONG conn_id, LONG var_id, UNG_CVVT &value,
									LONG handle=0, LONG index=0);
/*
 desc : 현재 Variable의 Value 값을 문자열로 반환
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		var_id	- [in]  Variable ID
		var_name- [in]  Variable Name (NULL이 아니면, 무조건 name 기준으로 검색)
		value	- [out] Value 값이 저장되는 버퍼 포인터
		size	- [in]  'value' 버퍼 크기
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID; 1 or 2)
		index	- [in]	일반적으로 0 값이지만, 리스트의 경우 Slot ID (Wafer ID; 1 ~ Max)
 retn : None
*/
API_IMPORT VOID uvCIMLib_GetValueToStr(LONG conn_id, LONG var_id, PTCHAR var_name, PTCHAR value,
									   UINT32 size, LONG handle=0, LONG index=0);
/*
 desc : 현재 CIMConnect에 저장 및 관리되는 Variable의 Cache 값 반환
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		var_id	- [in]  Variable ID
		var_name- [in]  Variable name
		value	- [out] 반환되어 저장될 버퍼
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID; 1 or 2)
		index	- [in]	일반적으로 0 값이지만, 리스트의 경우 Slot ID (Wafer ID; 1 ~ Max)
 retn : Name 문자열 포인터 반환 (검색 실패의 경우, -1L)
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
 desc : 현재 CIMConnect에 저장 및 관리되는 Variable의 Cache 값을 문자열 반환
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		var_id	- [in]  Identification (Variables Name과 연결된 ID)
		value	- [out] 임시 데이터가 저장된 버퍼 포인터 (할당된 메모리가 아님)
		size	- [in]  'value' 버퍼의 크기
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID; 1 or 2)
		index	- [in]	일반적으로 0 값이지만, 리스트의 경우 Slot ID (Wafer ID; 1 ~ Max)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetAsciiVariable(LONG conn_id, LONG var_id, PTCHAR value, UINT32 size, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetAsciiEvent(LONG conn_id, LONG var_id, PTCHAR value, UINT32 size, LONG handle=0, LONG index=0);
API_IMPORT BOOL uvCIMLib_GetAsciiAlarm(LONG conn_id, LONG var_id, PTCHAR value, UINT32 size, LONG handle=0, LONG index=0);
/*
 desc : CIMConnect 내부의 캐쉬 버퍼에 값 저장
 parm : conn_id	- [in] 1 or Later (공통 적용을 위해서는 0 값 사용)
		type	- [in]  입력되는 값 (Value)의 Type
		var_name- [in]  변수 이름
		value	- [in]  값 (type에 따라, 여러가지 포맷의 값)
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID; 1 or 2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetCacheValueID(LONG conn_id, E30_GPVT type, LONG var_id, UNG_CVVT value, LONG handle=0);
API_IMPORT BOOL uvCIMLib_SetCacheValueName(LONG conn_id, E30_GPVT type, PTCHAR var_name, UNG_CVVT value, LONG handle=0);
API_IMPORT BOOL uvCIMLib_SetCacheStrValueID(LONG conn_id, E30_GPVT type, LONG var_id, PTCHAR value, LONG handle=0);
/*
 desc : Cache the value of one variable of type ASCII in CIMConnect.
		The variable can be type Status Variable, Data Variable or Equipment Constant.
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		var_id	- [in]  Variable ID
		var_name- [in]  Variable Name
		value	- [in]  variable value
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID;1 or 2)
		index	- [in]	일반적으로 0 값이지만, 리스트의 경우 Slot ID (Wafer ID;1 ~ Max)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetCacheAsciiID(LONG conn_id, LONG var_id, PTCHAR value, LONG handle, LONG index);
API_IMPORT BOOL uvCIMLib_SetCacheAsciiName(LONG conn_id, PTCHAR var_name, PTCHAR value, LONG handle, LONG index);
/*
 desc : TS (Tracking System)에 등록된 Processing Name 반환
 parm : index	- [in]  장비 명과 매핑된 인덱스 값 (!!! -1 based !!!)
 retn : 이름 문자열 (실패의 경우 NULL or Empty String)
*/
API_IMPORT PTCHAR uvCIMLib_GetEquipProcessingName(E90_SSPS index);
/*
 desc : 로그 상태 (파일에 로그 파일이 저장 중인지 여부)
 parm : enable	- [out] 로그 파일 저장 여부 값 반환
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsLogging(BOOL &enable);
/*
 desc : 로그 동작 유무 설정
 parm : enable	- [in]  로그 파일 동작 유무 설정 플래그
						enable=TRUE -> Start, enable=FALSE -> Stop
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetLogEnable(BOOL enable);
/*
 desc : 현재 등록 (발생)된 명령 ID 값들 반환
 parm : id		- [out] 리스트가 저장될 CAtlList <LONG> : ID
		name	- [out] 리스트가 저장될 String Array    : Name
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetCommandList(CAtlList <LONG> &id, CStringArray &name);
/*
 desc : 현재 등록 (발생)된 EPJ 파일들 반환
 parm : files	- [out] 리스트가 저장될 String Array    : EPJ Files (Included Path)
		sizes	- [out] 리스트가 저장될 CAtlList <LONG> : EPJ File Size
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetEPJList(CStringArray &files, CAtlList <LONG> &sizes);
/*
 desc : 현재 등록 (발생)된 Recipe Names 반환
 parm : names	- [out] 리스트가 저장될 String Array    : Recipe Names
		sizes	- [out] 리스트가 저장될 CAtlList <LONG> : Recipe Sizes
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetRecipes(CStringArray &names, CAtlList <LONG> &sizes);
/*
 desc : Host 쪽에 Process Program (Recipe)를 생성하겠다고 요청
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		name	- [in]  Recipe (Process Program) Name (확장자는 꼭 포함하지 않아도 됨)
		size	- [in]  Recipe (Process Program) Size (unit: bytes)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_QueryRecipeUpload(LONG conn_id, PTCHAR name, UINT32 size);
/*
 desc : Host 쪽에 Process Program (Recipe)를 요청
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		name	- [in]  Recipe (Process Program) Name (확장자는 꼭 포함하지 않아도 됨)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_QueryRecipeRequest(LONG conn_id, PTCHAR name);
/*
 desc : 시스템에 정의된 Report 목록 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		ids		- [out] 리스트가 저장될 CAtlList <LONG> : Report Names
		names	- [out] 리스트가 저장될 String Array    : Report IDs
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetReports(LONG conn_id, CAtlList <LONG> &ids, CStringArray &names);
/*
 desc : Report 및 관련된 Event의 리스트 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		r_ids	- [out] 리스트가 저장될 CAtlList <LONG> : Report IDs
		e_ids	- [out] 리스트가 저장될 CAtlList <LONG> : Event IDs
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetReportsEvents(LONG conn_id, CAtlList <LONG> &r_ids, CAtlList <LONG> &e_ids);
/*
 desc : 발생된 Traces의 리스트 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		ids		- [out] 리스트가 저장될 CAtlList <LONG> : Report IDs
		names	- [out] 리스트가 저장될 String Array    : Trance Names
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetListTraces(LONG conn_id, CAtlList <LONG> &ids, CStringArray &names);
/*
 desc : EPJ 파일에 등록된 variables 목록 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		type	- [in]  Variable Type (E30_GVTC)
		ids		- [out] Variable IDs 반환
		names	- [out] Variable Names 반환
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetListVariables(LONG conn_id, E30_GVTC type, CAtlList <LONG> &ids,
										  CStringArray &names);
/*
 desc : List all messages (SECS-II 모든 메시지 반환)
		이 메서드는 장비 인터페이스에서 현재 처리 중인 메시지의 메시지 ID 목록을 반환합니다.
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		ids		- [out] message IDs 반환
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetListMessageIDs(LONG conn_id, CAtlList <LONG> &ids);
/*
 desc : Variable ID에 해당되는 Value Type (E30_GVTC) 값 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		var_id	- [in]  Variable ID와 매핑되는 Name
		var_name- [in]  Variable Name
		type	- [out] Value Type
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetValueTypeID(LONG conn_id, LONG var_id, E30_GPVT &type);
API_IMPORT BOOL uvCIMLib_GetValueTypeName(LONG conn_id, PTCHAR var_name, E30_GPVT &type);
/*
 desc : Variable ID에 해당되는 Variable Type (E30_GVTC) 값 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		var_id	- [in]  Variable ID와 매핑되는 Name
		var_name- [in]  Variable Name
		type	- [out] Variable Type
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetVarTypeID(LONG conn_id, LONG var_id, E30_GVTC &type);
API_IMPORT BOOL uvCIMLib_GetVarTypeName(LONG conn_id, PTCHAR var_name, E30_GVTC &type);
/*
 desc : 연결된 통신 설정 정보 얻기
 parm : conn_id		- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		comm_pgid	- [out] The COM ProgID of the communications object
		size_pgid	- [in]  'comm_pgid'에 저장된 문자열 길이
		comm_sets	- [out] The communications settings
		size_sets	- [in]  'comm_sets'에 저장된 문자열 길이
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetCommSetupInfo(LONG conn_id, PTCHAR comm_pgid, UINT32 size_pgid,
										  PTCHAR comm_sets, UINT32 size_sets);
/*
 desc : 연결될 통신 설정
 parm : conn_id		- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		comm_pgid	- [in]  The COM ProgID of the communications object
		comm_sets	- [in]  The communications settings
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetCommSetupInfo(LONG conn_id, PTCHAR comm_pgid, PTCHAR comm_sets);
/*
 desc : 로그 저장 관련 환경 설정 값을 윈도 레지스트리에 저장할지 여부 설정
 parm : enable	- [in]  TRUE : 저장, FALSE : 저장 안함
		logs	- [in]  로그 저장 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetLoggingConfigSave(BOOL enable);
/*
 desc : 로그 저장 관련 환경 설정 값의 저장 여부 값 반환
 parm : enable	- [out] TRUE : 저장하고 있음, FALSE : 저장 안하고 있음
		logs	- [in]  로그 저장 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetLoggingConfigSave(BOOL &enable);
/*
 desc : SECS-II Message 송신 (전송) 즉, Primary Message 송신? 기능
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		msg_sid	- [in]  SECS-II Message ID : Stream ID (0xssff -> 0x00xx ~ 0xffxx 까지)
		msg_fid	- [in]  SECS-II Message ID : Function ID (0xssff -> 0xxx00 ~ 0xxxFF 까지)
		term_id	- [in]  보통 0 값이나, 자신의 Terminal ID가 1 이상인 경우도 있다 (?)
		str_msg	- [in]  메시지 내용 (문자열), 만약 NULL이면 메시지 본문이 없는 경우임
		reply	- [in]  응답 기대 여부 (Host로부터 응답 받기를 원하는지 여부)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SendPrimaryMessage(LONG conn_id, UINT8 msg_sid, UINT8 msg_fid,
											PTCHAR str_msg=NULL, UINT8 term_id=0, BOOL reply=FALSE);
/*
 desc : Application Name 반환
 parm : name	- [out] Application Name이 저장될 버퍼
		size	- [in]  'name' 버퍼 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetAppName(PTCHAR name, UINT32 size);
/*
 desc : Application Name 설정
 parm : name	- [in]  Application Name이 저장된 버퍼
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetAppName(PTCHAR name);
/*
 desc : 현재 프로젝트의 패키지가 빌드된 시점의 날짜 (시간 포함) 반환
 parm : date_time	- [out] 빌드된 시간 (날짜포함)이 저장될 버퍼
		size		- [in]  'data_time' 버퍼 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetBuildDateTime(PTCHAR date_time, UINT32 size);
/*
 desc : 현재 프로젝트의 패키지가 빌드된 시점의 Version 반환
 parm : version	- [out] 빌드된 시간 (날짜포함)이 저장될 버퍼
		size	- [in]  'version' 버퍼 크기
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
 desc : 임의 (특정) 장치에 연결되어 있는 응용 프로그램의 IDs와 이름들을 반환
 parm : equip_id- [in]  장비 ID (0, 1 or Later)
		ids		- [out] 응용 프로그램 IDs 반환 (ID 숫자 리스트로 반환)
		names	- [out] 응용 프로그램 Names 반환 (문자열 배열로 반환)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetListApplications(LONG equip_id, CAtlList <LONG> &ids, CStringArray &names);
/*
 desc : 임의 (특정) 장치에 연결되어 있는 Host의 IDs와 이름들을 반환
 parm : conn_id	- [in]  장비 ID (0, 1 or Later)
		ids		- [out] Host IDs 반환 (ID 숫자 리스트로 반환)
		names	- [out] Host Names 반환 (문자열 배열로 반환)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetListConnections(LONG conn_id, CAtlList <LONG> &ids, CStringArray &names);
/*
 desc : EM Service에 연결되어 있는 Equipment의 IDs와 이름들을 반환
 parm : equip_id- [in]  장비 ID (0, 1 or Later)
		ids		- [out] Equipment IDs 반환 (ID 숫자 리스트로 반환)
		names	- [out] Equipment Names 반환 (문자열 배열로 반환)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetListEquipments(CAtlList <LONG> &ids, CStringArray &names);
/*
 desc : 현재 Control이 Remote 모드인지 여부
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsControlRemote(LONG conn_id);
/*
 desc : 현재 Control이 Online 모드인지 여부
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsControlOnline(LONG conn_id);
/*
 desc : 현재 Communication모드가 Enable인지 여부
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_IsCommEnabled(LONG conn_id);
/*
 desc : CIMConnect 내부에 로그 저장 여부
 parm : value	- [out] TRUE : Saved, FALSE : No save
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetLogToCIMConnect(BOOL &value);
/*
 desc : CIMConnect 내부에 로그 저장 여부 설정
 parm : value	- [in]  TRUE : Saved, FALSE : No save
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetLogToCIMConnect(BOOL value);
/*
 desc : SECS-II Message 등록 헤재
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_ResetSecsMesgList();
/*
 desc : SECS-II Message 등록 - Root Message
 parm : type	- [in]  Message Type
		value	- [in]  Message Type에 따른 값
		size	- [in]  'value' 버퍼 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_AddSecsMesgRoot(E30_GPVT type, PVOID value, UINT32 size);
/*
 desc : SECS-II Message 등록 - Root Message
 parm : handle	- [in]  List가 추가될 상위 Parent ID
						맨 처음 추가될 List의 경우, 0 값
		clear	- [in]  Message 버퍼 모두 비우기 여부 (최초이면 TRUE, 중간이면 FALSE)
 retn : 새로 추가된 Handle 값 반환 (음수 값이면, 에러 처리)
*/
API_IMPORT LONG uvCIMLib_AddSecsMesgList(LONG handle, BOOL clear=FALSE);
/*
 desc : 원격 장치 (Host?)에게 송신될 SECS-II Message 등록
 parm : handle	- [in]  Handle to a list within the object, or 0
		type	- [in]  'value' 값이 저장된 변수 형식
		value	- [in]  The value to append
		size	- [in]  'value' 버퍼에 저장된 데이터의 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_AddSecsMesgValue(LONG handle, E30_GPVT type, PVOID value, UINT32 size);
/*
 desc : 원격 장치 (Host?)에게 SECS-II Message 송신
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		mesg_sid- [in]  송신 대상의 SECS-II Message ID (Stream ID) (10진수)
		mesg_fid- [in]  송신 대상의 SECS-II Message ID (Function ID) (10진수)
		trans_id- [in]  송신 후 응답 받을 때, 동기화를 위해 필요한 ID (내가 보낸 물음에 응답임을 알기 위한 식별 ID) (0 값 초기화)
		is_reply- [in]  응답 (Ack) 요청 여부. TRUE - Host로부터 메시지 수신에 대한 응답 요청, FALSE - 응답 필요 없음
		is_body	- [in]  Message Body가 존재하는지 유무 즉, Message ID만 존재한다면 FALSE, 보낼 Message가 존재한다면 (TRUE)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SendSecsMessage(UINT8 conn_id, UINT8 mesg_sid, UINT8 mesg_fid,
										 BOOL is_reply, LONG &trans_id, BOOL is_body);


/* --------------------------------------------------------------------------------------------- */
/*                               ICxE30 Communication / Control                                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Communication Enable or Disable
 parm : conn_id	- [in] 1 or Later (공통 적용을 위해서는 0 값 사용)
		enable	- [in]  TRUE or FALSE
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetCommunication(LONG conn_id, BOOL enable);
/*
 desc : Change the GEM Control State to REMOTE or LOCAL
 parm : conn_id	- [in] 1 or Later (공통 적용을 위해서는 0 값 사용)
		state	- [in]  if true, go REMOTE. if false, go LOCAL
 retn : None
*/
API_IMPORT BOOL uvCIMLib_SetControlRemote(LONG conn_id, BOOL state);
/*
 desc : Change the GEM Control state to ONLINE or OFFLINE
 parm : conn_id	- [in] 1 or Later (공통 적용을 위해서는 0 값 사용)
		state	- [in]  if true, go ONLINE. if false, go OFFLINE
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetControlOnline(LONG conn_id, BOOL state);
/*
 desc : Spooling Overwrite 여부
 parm : conn_id		- [in]  연결된 장비 ID
		overwrite	- [in]  Overwrite (TRUE) or Not overwrite (FALSE)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetSpoolOverwrite(LONG conn_id, BOOL overwrite);
/*
 desc : Spooling State 제어
 parm : conn_id	- [in]  연결된 장비 ID
		enable	- [in]  Enable (TRUE) or Disable (FALSE)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetSpoolState(LONG conn_id, BOOL enable);
/*
 desc : Spooling State 제어
 parm : conn_id	- [in]  연결된 장비 ID
		enable	- [in]  Enable (TRUE) or Disable (FALSE)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetSpoolPurge(LONG conn_id);
/*
 desc : Enables or Disables alarm reporting for a host for the specified alarms.
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		alarm_id- [in]  Connection Variables or Alarms의 Section에 나열된 variables identification number
		state	- [in]  TRUE (ENABLE) or FALSE (DISABLE)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetAlarmsEnabled(LONG conn_id, LONG alarm_id, BOOL state);
/*
 desc : 현재 CIMConnect 내부에 발생된 알람 모두 해제
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_AlarmClearAll();
/*
 desc : 현재 CIMConnect 내부에 발생된 알람 모두 해제
 parm : var_id	- [in]  Alarm ID 즉, EPJ Variables ID와 동일 함
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_AlarmClearID(LONG var_id);
API_IMPORT BOOL uvCIMLib_AlarmClearName(PTCHAR var_name);
/*
 desc : 현재 CIMConnect 내부에 발생된 알람 모두 해제
 parm : name	- [in]  Alarm Name
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_AlarmClearName(PTCHAR name);
/*
 desc : 현재 등록 (발생)된 알람 ID 값들 반환
 parm : id		- [out] 알람 리스트가 저장된 CAtlList <LONG> : 알람 ID
		state	- [out] 알람 리스트가 저장된 CAtlList <LONG>: 알람 상태 (0 or 1)
		alarm	- [in]  알람이 발생된 정보만 반환 여부 (FLALSE:  모두, TRUE: 알람이 발생된 것만)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetAlarmList(CAtlList <LONG> &id, CAtlList <LONG> &state, BOOL alarm);
/*
 desc : 개별 알람 상태 값 반환
 parm : id		- [in]  알람 ID
		state	- [out] 알람 상태 반환 (1 : Set, 0 : Clear)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetAlarmState(LONG id, LONG &state);
/*
 desc : CIMConnect 내부로 임의의 알람 발생
 parm : var_id	- [in]  Alarm ID 즉, EPJ Variables ID와 동일 함
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
 desc : Event 발생 (Method to trigger an event)
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		evt_id	- [in]  Collection Event ID
		evt_name- [in]  Collection Event Name
 retn : TRUE or FALSE
 note : This method triggers an equipment event
*/
API_IMPORT BOOL uvCIMLib_SetTrigEventID(LONG conn_id, LONG evt_id);
API_IMPORT BOOL uvCIMLib_SetTrigEventName(LONG conn_id, PTCHAR evt_name);
/*
 desc : Trigger Event Data 발생 (부모에게 알림) - 1 개 값에 대해 설정
 parm : conn_id	- [in] 1 or Later (공통 적용을 위해서는 0 값 사용)
		evt_id	- [in]  Collection Event ID
		evt_name- [in]  Collection Event Name
		var_id	- [in]  Variable ID
		var_name- [in]  Variable Name
		value	- [in]  값 이름
		type	- [in]  'value' type (U1, U2, U4, Bo, I1, I3, ...)
 retn : TRUE or FALSE
 note : 단, list, jis_8_string 등은 제외
*/
API_IMPORT BOOL uvCIMLib_SetTrigEventValueID(LONG conn_id, LONG evt_id, LONG var_id,
											 LNG_CVVT value, E30_GPVT type);
API_IMPORT BOOL uvCIMLib_SetTrigEventValueName(LONG conn_id, PTCHAR evt_name, PTCHAR var_name,
											   LNG_CVVT value, E30_GPVT type);
/*
 desc : Trigger Event Data 발생 (부모에게 알림) - n 개 값에 대해 설정
 parm : conn_id	- [in] 1 or Later (공통 적용을 위해서는 0 값 사용)
		evt_id	- [in]  Event ID
		var_id	- [in]  Variable ID가 저장된 리스트
		evt_name- [in]  Event Name
		var_name- [in]  Variable Name이 저장된 리스트
		value	- [in]  Variable Value가 저장된 리스트
		type	- [in]  'value' type이 저장된 리스트 (U1, U2, U4, Bo, I1, I3, ...)
 retn : TRUE or FALSE
 note : 단, list, jis_8_string 등은 제외
*/
API_IMPORT BOOL uvCIMLib_SetTrigEventValuesID(LONG conn_id, LONG evt_id,
											  CAtlList<LONG> &var_id, CAtlList<UNG_CVVT> &value,
											  CAtlList<E30_GPVT> &type);
API_IMPORT BOOL uvCIMLib_SetTrigEventValuesName(LONG conn_id, PTCHAR evt_name,
												CStringArray &var_name, CAtlList<UNG_CVVT> &value,
												CAtlList<E30_GPVT> &type);
/*
 desc : Trigger Event Data 발생 (부모에게 알림) - 여러 개 값에 대해 설정
 parm : conn_id	- [in] 1 or Later (공통 적용을 위해서는 0 값 사용)
		evt_id	- [in]  Event ID
		var_id	- [in]  Variable ID
		evt_name- [in]  Event Name
		var_name- [in]  Variable Name
		value	- [in]  Variable Value가 저장된 리스트
		type	- [in]  'value' type이 저장된 리스트 (U1, U2, U4, Bo, I1, I3, ...)
 retn : TRUE or FALSE
 note : 단, jis_8_string 등은 제외
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
 parm : state	- [in]  동작 상태 (동작 상태 : 0 (Setup), 1 (Idle), 2 (executing))
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetUpdateProcessState(E30_GPSV state);
/*
 desc : Send a terminal service acknowledge event to the host. Send to host 1 by default.
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
 retn : TRUE or FALSE
 참조 : This method triggers an equipment well-known event
		Equipment events are momentary, not latched
		There is no method to reset events since they are a one-shot, non-latched event.
*/
API_IMPORT BOOL uvCIMLib_SendTerminalAcknowledge(LONG conn_id);
/*
 desc : Send a terminal service text message to the host.
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		term_id	- [in]  Terminal ID (0 or Later)
		mesg	- [in]  The text message
 retn : TRUE or FALSE
 참조 : 참조 : This method requests that a terminal message to be sent to a connection
*/
API_IMPORT BOOL uvCIMLib_SendTerminalMessage(LONG conn_id, LONG term_id, PTCHAR mesg);
/*
 desc : Notify CIMConnect that an operator command has been issued.
 parm : conn_id	- [in]  Connection #
		name	- [in]  Name of the command
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_OperatorCommand(LONG conn_id, PTCHAR name, LONG handle);
/*
 desc : 장치에 정의된 모든 데이터 항목들을 제거 (주의해서 사용해야 됨)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetClearProject();
/*
 desc : 장비 내에 존재하는 알람 ID 제거
 parm : alarm_id- [in]  제거하고자 하는 알람 ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetDeleteAlarm(LONG alarm_id);
/*
 desc : 모든 요청받은 데이터 (명령?)들 제거
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_SetDeleteAllRequests();
/*
 desc : 알람 이름에 해당되는 알람 ID 값 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		name	- [in]  알람 이름
		id		- [out] 알람 ID 반환
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetAlarmID(LONG conn_id, PTCHAR name, LONG &id);
/*
 desc : 알람 ID에 대한 설명 반환
 parm : alarm_id- [in]  알람 ID
		desc	- [out] 알람 설명이 반환될 버퍼
		size	- [in]  'desc' 버퍼 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetAlarmIDToDesc(LONG alarm_id, PTCHAR desc, UINT32 size);
/*
 desc : 알람 ID에 대한 명칭 반환
 parm : alarm_id- [in]  알람 ID
		name	- [out] 알람 명칭이 반환될 버퍼
		size	- [in]  'name' 버퍼 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetAlarmIDToName(LONG alarm_id, PTCHAR name, UINT32 size);
/*
 desc : 통신 상태 (Low Level Communicating state) 값 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		state	- [out] 통신 여부
		substate- [out] HSMS (Ethernet) 상태 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetCommState(LONG conn_id, E30_GCSS &state, E30_GCSH &substate);
/*
 desc : Host 연결에 대한 이름 즉, connection id에 매핑되는 이름 얻기
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		name	- [out] 연결된 Host Name 값 얻기
		size	- [in]  'name' 버퍼 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetConnectionName(LONG conn_id, PTCHAR name, UINT32 size);
/*
 desc : 현재 적재된 Project Name 얻기
 parm : name	- [out] Project Name 값 얻기
		size	- [in]  'name' 버퍼 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetCurrentProjectName(PTCHAR name, UINT32 size);
/*
 desc : 기본 설정된 Default Project Name 얻기
 parm : name	- [out] Project Name 값 얻기
		size	- [in]  'name' 버퍼 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetDefaultProjectName(PTCHAR name, UINT32 size);
/*
 desc : 현재 연결된 장비 ID 값 반환
 parm : equip_id- [out] 매핑(관리)되는 장비 ID 값 저장
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetHostToEquipID(LONG &equip_id);
/*
 desc : Event Name에 해당되는 Event ID 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		name	- [in]  Event Name
		id		- [out] Event ID가 반환되어 저장됨
반환 : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetEventNameToID(LONG conn_id, PTCHAR name, LONG &id);
/*
 desc : Variable Name에 해당되는 Variable ID 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		name	- [in]  Variable Name
		var_id	- [out] Variable ID가 반환되어 저장됨
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetVarNameToID(LONG conn_id, PTCHAR name, LONG &var_id);
/*
 desc : Variable ID에 해당되는 Variable Name 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		var_id	- [in]	Variable ID
		name	- [out] Variable Name이 반환되는 버퍼
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetVarIDToName(LONG conn_id, LONG var_id, PTCHAR name, UINT32 size);
/*
 desc : Variable ID에 해당되는 Variable Description 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		var_id	- [in]	Variable ID
		desc	- [out] Variable Description이 반환되는 버퍼
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetVarIDToDesc(LONG conn_id, LONG var_id, PTCHAR desc, UINT32 size);
/*
 desc : Variable ID에 해당되는 Min, Max 값 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		var_id	- [in]  Variable ID
		type	- [out] Value Type (E30_GPVT)이 저장됨
		min		- [out] Min 값이 저장됨
		max		- [out] Max 값이 저장됨
		size	- [in]  'min, max' 문자열 버퍼의 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetVarMinMax(LONG conn_id, LONG var_id, E30_GPVT &type, PVOID min, PVOID max);
API_IMPORT BOOL uvCIMLib_GetVarMinMaxToStr(LONG conn_id, LONG var_id, PTCHAR min, PTCHAR max, UINT32 size);
/*
 desc : 현재 연결되어 있는 Host ID 기준으로 변수들 정보 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		type	- [in]  변수 타입 (E30_GVTC)
		lst_data- [out] 변수들의 정보가 저장될 리스트 참조
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_GetHostVariables(LONG conn_id, E30_GVTC type, CAtlList <STG_CEVI> &lst_data);



/* --------------------------------------------------------------------------------------------- */
/*                            E40 Processing Job Management (PJM)                                */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Process Job을 SETTING UP 상태에서 WAITING FOR START 내지 PROCESSING 상태로 이동
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
 note : Moves Process Job from SETTING UP state to WAITING FOR START or PROCESSING state
		소재가 준비되고 Process Job이 시작할 준비가 되어 있다고 알림
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRJobStartProcess(PTCHAR pj_id);
/*
 desc : Process Job이 속한 Control Job ID 반환
 parm : pj_id	- [in]  Object ID of the Process Job
		cj_id	- [out] Object ID of the Control Job
		size	- [in]  'cj_id' 버퍼 크기
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetControlJobID(PTCHAR pj_id, PTCHAR cj_id, UINT32 size);
/*
 desc : Process Job이 속한 Control Job ID 설정
 parm : pj_id	- [in]  Object ID of the Process Job
		cj_id	- [in]  Object ID of the Control Job
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetControlJobID(PTCHAR pj_id, PTCHAR cj_id);
/*
 desc : Process Job의 Processing이 끝난 경우임
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
 note : Moves Process Job from PROCESSING state to PROCESS COMPLETE state.
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRJobProcessComplete(PTCHAR pj_id);
/*
 desc : Process Job 작업 완료 (PRJobComplete -> get_ParentControlJob -> ProcessJobComplete)
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRJobComplete(PTCHAR pj_id);
/*
 desc : Process Job을 Stopping 상태에서 Stopped 상태로 이동 (변경)
 parm : pj_id	- [in]  Objet ID of the Process Job
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetProcessJobStoppingToStopped(PTCHAR pj_id);
/*
 desc : Process Job을 ABORTING 상태에서 ABORTED 상태로 이동
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
 note : Moves Process Job from ABORTING state to the ABORTED state.
		작업 취소가 완료되었을 때, 이 함수를 호출해 주면 됨
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRJobAbortComplete(PTCHAR pj_id);
/*
 desc : Process Job (ID)의 현재 상태 반환
 parm : pj_id	- [in]  Process Job ID
		state	- [OUT] state
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetProcessJobState(PTCHAR pj_id, E40_PPJS &state);
/*
 desc : 모든 Process Job Id 반환 (완료되지 않은 작업의 상태만 반환)
 parm : list_job- [out] Job ID가 반환되어 저장될 배열 버퍼
		logs	- [in]  로그 출력 여부
 변환 : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetProcessJobQueue(CStringArray &list_job);
/*
 desc : Processing Resource를 위해 Queue에서 생성될 수 있는 남아 있는 Jobs 개수 반환
 parm : count	- [out] 생성될 수 있는 여유 개수 반환 값이 저장
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetProcessJobSpaceCount(UINT32 &count);
/*
 desc : 현재 Process Job ID 中 맨 처음 SELECTED 상태인 Process Job ID 반환
 parm : filter	- [in]  Process Job ID 검색 필터 (E40_PPJS)
		pj_id	- [out] 반환되어 저장될 Process Job ID (String)
		size	- [in]  'pj_id' 버퍼 크기
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
 note : 검색된 결과가 없으면 FALSE 반환
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetProcessJobFirstID(E40_PPJS filter, PTCHAR pj_id, UINT32 size);
/*
 desc : 모든 Process Job Id와 State 반환
 parm : list_job	- [out] Job ID가 반환되어 저장될 배열 버퍼
		list_state	- [out] 상태 값이 저자장될 리스트 버퍼
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetProcessAllJobIdState(CStringArray &list_job,
														CAtlList<E40_PPJS> &list_state);
/*
 desc : Process Job Remove
 parm : pj_id	- [in]  제거하고자 하는 Process Job Id
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetProcessJobRemove(PTCHAR pj_id);
/*
 desc : Process Job을 Queuing 상태에서 Setting Up 상태로 이동
 parm : pj_id	- [in]  Objet ID of the Process Job
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetProcessJobQueueToSetup(PTCHAR pj_id);
/*
 desc : Process Job 생성
 parm : pj_id		- [in]  Process Job Name ID (String; Ascii)
		mtrl_type	- [in]  Material Type (E40_PPMT)
		carr_id		- [in]  Carrier Name ID (String; Ascii) List
		recipe_name	- [in]  Recipe Name ID
		slot_no		- [in]  Slot Number (U1 Type) List (1 ~ 25 중 임의 개수만큼 저장됨)
		recipe_only	- [in]  TRUE  : RECIPE_ONLY (Recipe method without recipe variable)
							FALSE : RECIPE_WITH_VARIABLE_TUNING (Recipe method with recipe variable)
		proc_start	- [in]  Process Start 여부
							Indicates that the processing resource starts processing imediately
							when ready (PRProcessStart) (Auto Start 여부)
		en_hanced	- [in]  Enhanced create 함수 적용 여부
 retn : TRUE or FALSE
 note : This method is called by the host to create a new Process Job
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetProcessJobCreate(PTCHAR pj_id, PTCHAR carr_id,
													PTCHAR recipe_name, E40_PPMT mtrl_type,
													BOOL recipe_only, BOOL proc_start,
													CAtlList <UINT8> *slot_no, BOOL en_hanced=TRUE);
/*
 desc : Remote Control 허용 여부 설정
 parm : enable	- [in]  Enable or Disable
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetAllowRemoteControl(BOOL enable);
/*
 desc : Remote Control 허용 여부 값 반환
 parm : enable	- [out] Enable or Disable
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetAllowRemoteControl(BOOL &enable);
/*
 desc : Process Material Name List (For substrates)의 속성 값 반환
		!!! TEL의 경우, 무조건 2번째 즉, CarrierID 와 Slot No로 되어 있는 방식으로 읽어야 됨 !!!
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		carr_id	- [out] Carrier ID가 반환되어 저장될 버퍼
		slot_no	- [out] Slot Number가 반환되어 저장될 버퍼
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetPRMtlNameList(PTCHAR pj_id,
												 CStringArray &carr_id, CByteArray &slot_no);
/*
 desc : 현재 Process Job ID 내에 등록된 Slot Number List 중에서 찾고자 하는 Slot Number가 있는지 여부
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		slot_no	- [in]  검색하고자 하는 Slot Number (1-based)
		is_slot	- [out] Slot Number 존재 여부 반환
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_IsFindSlotNoOfPRMtlNameList(PTCHAR pj_id, UINT8 slot_no, BOOL &is_slot);
/*
 desc : Process Job의 제어 동작 값을 강제로 제어 시킴
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		control	- [in]  E40_PPJC (Abort, Cancel, Pause, Paused, Resume, Start, Stop)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRJobStateControl(PTCHAR pj_id, E40_PPJC control);
/*
 desc : Process Job State 변경
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
		Queue에 등록될 때, Process Job ID의 정렬 방법을 반환 합니다.
 parm : order	- [out] Material Order (E94_CPOM, PRMtrlOrder value)
 retn : TRUE or FALSE
 note : This function returns the PRMtrlOrder value, the order in which materials are processed
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetPRMaterialOrder(E94_CPOM &order);
/*
 desc : Sets the PRMtrlOrder value by the host using PRSetMtrlOrder service
		Queue에서 Process Job ID가 저장되는 정렬 방법 설정
 parm : order	- [in]  Material Order (E94_CPOM, PRMtrlOrder value)
 retn : TRUE or FALSE
 note : Sets the PRMtrlOrder value by the host using PRSetMtrlOrder service.
		PRSetMtrlOrder callback is called to let the client application grant or deny this request.
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRMaterialOrder(E94_CPOM order);
/*
 desc : Process Job에서 명령을 실행 시킴 (Executes a command on a Process Job)
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		command	- [in]  명령어 (PCIMLib에서 등록된 명령어. START;STOP;PAUSE;RESUME;ABORT;CANCEL)
						Register for Remote Commands (Remote Command name의 총 길이가 20 string을 넘지 않아야 됨)
 retn : TRUE or FALSE
 note : This method is called by the host to perform a PRJobCommand on the specified Process Job
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRJobCommand(PTCHAR pj_id, PTCHAR command);
/*
 desc : Pause Events에 대해 Enable or Disable
 parm : enable	- [in]  TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE
 note : The client application can disable PauseEvent capability for Process Jobs by setting this value to VARIANT_FALSE.
		Default is VARIANT_TRUE.
		When set to VARIANT_FALSE, all requests to create Process Jobs with the PauseEvent parameter will be rejected.
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetEnablePauseEvent(BOOL enable);
/*
 desc : Pause Events에 대해 Enable or Disable 상태 값 반환
 parm : enable	- [out] TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetEnablePauseEvent(BOOL &enable);
/*
 desc : 장비 쪽에서 사용하는 알림 (Notification)의 형식 (Type)이 무엇인지 알려줌
 parm : type	- [out] Notification Type (E40_PENT)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetNotificationType(E40_PENT &type);
/*
 desc : 장비 쪽에서 사용할 알림 (Notification)의 형식 (Type)이 어떤 것인지 설정해 줌
 parm : type	- [in]  Notification Type (E40_PENT)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetNotificationType(E40_PENT type);
/*
 desc : Process Job의 상태가 변경되었다고 Host에게 알림
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRJobAlert(PTCHAR pj_id);
/*
 desc : Process Job ID의 Event State가 변경되었다고 Host에게 알림
 parm : pj_id		- [in]  Process Job Name ID (String; Ascii)
		evt_id		- [in]  PREventID of PRJobEvent service (E40_PSEO)
		list_vid	- [in]  Variables ID가 저장된 리스트
		list_type	- [in]  Variables Value Type이 저장된 리스트
		list_value	- [in]  'Variables Type'에 따라, 숫자 혹은 문자열 등이 등록되어 있음
							참고로, 숫자일 경우, _wtoi / _wtof 등등 변환이 필요 함
 retn : TRUE or FALSE
 note : This method is called by the client application to notify the host about changes in a Process Job's state
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRJobEvent(PTCHAR pj_id, E40_PSEO evt_id,
											  CAtlList <UINT32> *list_vid,
											  CAtlList <E30_GPVT> *list_type,
											  CStringArray *list_value);
/*
 desc : 임의 (특정) 프로세스 Job들을의 속성을 "PRProcessStart"로 상태 변경
 parm : list_jobs	- [in]  변경 대상의 Process Job들이 저장된 리스트
		start		- [in]  TRUE or FALSE
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRJobSetStartMethod(CStringArray *list_jobs, BOOL start);
/*
 desc : 기존 설정된 Material Type 값 반환
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		type	- [out] 기존 설정된 Material Type 값 저장
 retn : TRUE or FALSE
 note : Gets the PRMtlType attribute value
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetPRMtlType(PTCHAR pj_id, E40_PPMT &type);
/*
 desc : Material Type 값 새로 설정
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		type	- [in]  설정될 Maerial Type 값
 retn : TRUE or FALSE
 note : Sets the PRMtlType attribute value
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRMtlType(PTCHAR pj_id, E40_PPMT type);
/*
 desc : 처리를 시작하기 전에 모든 자재 (Wafer? Substrate)들이 도착할 때까지 대기하는지 여부 값 반환
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		value	- [out] TRUE : 모든 자재 (소재)들을 도착하는데 기다리고 있음
						FALSE: 모든 자재 (소재)들이 이미 도착한 상태임
 retn : TRUE or FALSE
 note : Gets the Process Job's PRWaitForAllMaterial switch value.
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetPRWaitForAllMaterial(PTCHAR pj_id, BOOL &value);
/*
 desc : 처리를 시작하기 전에 모든 자재 (Wafer? Substrate)들이 도착할 때까지 대기해야 할지 여부 설정
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		value	- [in]  TRUE : 모든 자재 (소재)들이 도착하는데 기다리고 있다라고 설정
						FALSE: 모든 자재 (소재)들이 도착하였다라고 설정 (기다릴 필요 없다?)
 retn : TRUE or FALSE
 note : Sets the Process Job's PRWaitForAllMaterial switch value.
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetPRWaitForAllMaterial(PTCHAR pj_id, BOOL value);
/*
 desc : Process Job ID로부터 Recipe ID (=Name; with string) 얻기
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		rp_id	- [out] Recipe ID가 저장될 버퍼
		size	- [in]  'rp_id' 버퍼 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetRecipeID(PTCHAR pj_id, PTCHAR rp_id, UINT32 size);
/*
 desc : 현재 Process Job ID에 대한 작업이 완료 되었는지 여부
 parm : pj_id	- [in]  Process Job Name ID (String; Ascii)
		state	- [out] 0x00: Busy, 0x01: Completed
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_IsProcessJobCompleted(PTCHAR pj_id, UINT8 &state);
/*
 desc : 모든 Process Job Id 반환
 parm : list_job- [out] Job ID가 반환되어 저장될 배열 버퍼
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetProcessAllJobId(CStringArray &list_job);
/*
 desc : Process Job ID에 포함된 모든 Slot Number 반환
 parm : pj_id	- [in]  검색 대상의 Process Job ID
		arr_slot- [out] Slot Number가 저장될 Byte Array
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetPRJobToAllSlotNo(PTCHAR pj_id, CByteArray &arr_slot);
/*
 desc : Process Job ID에 포함된 모든 Slot ID 반환
 parm : pj_id	- [in]  검색 대상의 Process Job ID
		arr_slot- [out] Slot ID가 저장될 String Array
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E40PJM_GetPRJobToAllSlotID(PTCHAR pj_id, CStringArray &arr_slot);
/*
 desc : Process Job이 Queue 상태에서 Completed 상태로 이동 (변경)하면 Project Job Id를 제거
 parm : pj_id	- [in]  Object ID of the Process Job
 retn : TRUE or FALSE
 note : SetProcessJobRemove 함수와 다른점 ? Queue에서 제거하는 거지, Job List에서 제거하는 것은 아님 
*/
API_IMPORT BOOL uvCIMLib_E40PJM_SetProcessJobDequeue(PTCHAR pj_id);

/* --------------------------------------------------------------------------------------------- */
/*                         E87 Control Management Specification (CMS)                            */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ICxSubstrate for E90STS 객체 포인터 얻기
 parm : carr_id	- [in]  Carrier ID
		slot_no	- [in]  Carrier 내에 위치한 Slot Number
		subst_id- [out] Substrate ID가 반환될 버퍼
		size	- [in]  'subst_id'의 버퍼 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetSubstToE87SubstrateID(PTCHAR carr_id, UINT8 slot_no,
														 PTCHAR subst_id, UINT32 size);
/*
 desc : Carrier가 로드 포트 위에 도착 (내려오고;안착하고)하거나 혹은 제거되고 있는 중이다 (?)
 parm : port_id	- [in]  Load Port Number (1 or 2)
 retn : TRUE or FALSE
 note : Load Port 상태를 "Ready To Load" or "Ready To Unload"에서 "Transfer Bloacked"으로 변경 시킴
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetTransferStart(UINT16 port_id);
/*
 desc : Carrier가 로드 포트 위에 안착 했다.
 parm : port_id	- [in]  Load Port Number (1 or 2)
 retn : TRUE or FALSE
 note : Load Transfer Complete (Sequnce Name)
		프로젝트 파일 (EPJ) 내에
			"UseCarrierPlaced" 변수에 값이 1로 설정되어 있다면
				CarrierLocationChanged collection event가 자동으로 트리거 되지만
			"UseCarrierPlaced" 변수에 값이 0로 설정되어 있다면
				CarrierLocationChanged collection event는 CarrierAtPort가 호출될 때 트리거 됩니다.
		이 함수의 경우 Load Port Reservation State를 변경하지 않고, CarrierAtPort가 변경 합니다.
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierPlaced(UINT16 port_id);
/*
 desc : Carrier가 Load Port에서 제거하고자 할 때, 이 함수 호출 (상태 변경)
 parm : port_id	- [in]  Load Port Number (1 or 2)
 retn : TRUE or FALSE
 note : AccessMode가 수동 모드일 경우,
			Carrier가 Unloading 중에 undocked 및 unclamped될 때 호출해야 하고
		AccessMode가 자동 모드일 경우, CarrierUnclamped equipment constant value 값이 1일 경우,
			Carrier가 Unloading 중에 아직 Clampled이지만 undocked 일 때 호출해야 합니다.
		AccessMode가 자동 모드일 경우, CarrierUnclamped equipment constant value 값이 0일 경우,
			Carrier가 Unloading 중에 undocked 및 unclamped될 때 호출해야 합니다.
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetReadyToUnload(UINT16 port_id);
/*
 desc : Carrier가 로드 포트 위에 올려지고, 센서에 의해 CarrierID가 정상적으로 판독되고 나면 호출 됨
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		port_id	- [in]  Load Port Number (1 or 2)
		loc_id	- [in]  Carrier Location ID 즉, 대부분 Carrier Undocked Name (Carrier Location Name)
 retn : TRUE or FALSE
 note : CarrierID 읽기가 실패하면, carr_id 값이 empty 상태로 입력돼도 상관 없음
		이 때, CIM87 내부적으로 새로운 CarrierID를 생성한다 (?)
		상세한 설명은 도움말 참조 바랍니다.
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierAtPort(PTCHAR carr_id, UINT16 port_id, PTCHAR loc_id);
/*
 desc : Carrier가 로드 포트에서 분리 되었다고 CIM87에 알림
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		port_id	- [in]  Load Port Number (1 or 2)
		loc_id	- [in]  Carrier Location ID 즉, 대부분 Carrier Undocked Name (Carrier Location Name)		
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierDepartedPort(PTCHAR carr_id, UINT16 port_id, PTCHAR loc_id);
/*
 desc : Carrier가 새로운 위치로 이동 했다고 알림
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		port_id	- [in]  Load Port Number (1 or 2)
		docked	- [in]  Carrier Docked Name (Location Name)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetMoveCarrier(PTCHAR carr_id, UINT16 port_id, PTCHAR docked);
/*
 desc : Carrier 내에 저장 (설치)된 Slot Map에 대해 Verification 처리
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		slots	- [in]  Wafer가 Slot Map에 적재된 상태 정보
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetVerifySlotMap(PTCHAR carr_id, CAtlList <E87_CSMS> *slots,
												 LONG handle=0);
/*
 desc : Carrier의 접근 상태 변경
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		port_id	- [in]  Load Port Number (1 or 2)
		status	- [in]  Carrier accessing status (E87_CCAS)
						0x00 : 아직 Carrier에 접근하지 못한 상태
						0x01 : Carrier에 접근하고 있는 상태
						0x02 : 장비 (Equipment)가 정상적으로 Carrier에 접근이 끝난 상태
						0x03 : 장비 (Equipment)가 비정상적으로 Carrier에 접근이 끝난 상태
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetAccessCarrier(LONG conn_id, PTCHAR carr_id, UINT16 port_id,
												 E87_CCAS status, LONG handle=0);
/*
 desc : Carrier Status Event 설정
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		port_id	- [in]  Load Port Number (1 or 2)
		status	- [in]  Carrier Event Value (E87_CCES)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierEvent(LONG conn_id, PTCHAR carr_id, UINT16 port_id,
												E87_CCES status);
/*
 desc : Carrier로부터 Substrate 제거 (가져오기)
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		slot_no	- [in]  Slot Number (Not sequential number)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierPopSubstrate(PTCHAR carr_id, UINT8 slot_no);
/*
 desc : 현재 Load Port ID의 Reservation State 정보 반환
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [out] Reservation State 반환
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetReservationState(UINT16 port_id, E87_LPRS &state);
/*
 desc : 현재 Load Port ID의 Reservation State 정보 설정
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [in]  Reservation State
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetReservationState(UINT16 port_id, E87_LPRS state);
/*
 desc : 현재 Load Port-Carrier의 Association State 정보 반환
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [out] Load Port-Carrier Association State 반환
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetAssociationState(UINT16 port_id, E87_LPAS &state);
/*
 desc : 현재 Load Port-Carrier의 Association State 정보 설정
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [in]  Load Port-Carrier Association State
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetAssociationState(UINT16 port_id, E87_LPAS state);
/*
 desc : 현재 Carrier의 Access Mode State 정보 반환
		Load Port가 수동으로 접근하는지 혹은 자동으로 접근하는지 상태 값 반환
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [out]  Manual (0) or Auto (1)
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetAccessModeState(UINT16 port_id, E87_LPAM &state);
/*
 desc : 현재 Carrier의 Access Mode Status 값 설정
		Load Port가 수동으로 접근하는지 혹은 자동으로 접근하는지 상태 값 설정
 parm : port_id	- [in]  Load Port Number (1 or 2)
		state	- [in]  Manual (0) or Auto (1)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetAccessModeState(UINT16 port_id, E87_LPAM state);
/*
 desc : Carrier ID Verification State 값 반환
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		state	- [out] Carrier ID Verfication 상태 값 반환
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetCarrierIDStatus(PTCHAR carr_id, E87_CIVS &status);
/*
 desc : Carrier ID Verification State 값 설정
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		state	- [in]  Carrier ID Verfication 상태 값
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierIDStatus(PTCHAR carr_id, E87_CIVS status);
/*
 desc : The slot map status of the specified carrier.
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		status	- [out] Slot map status 반환
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
 note : The slot map status shall be read on all production
		equipment prior to removal of substrates from the carrier
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetCarrierIDSlotMapStatus(PTCHAR carr_id, E87_SMVS &status);
/*
 desc : The slot map status of the specified carrier. <설정>
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		status	- [in]  Slot map status
 retn : TRUE or FALSE
 note : The slot map status shall be read on all production
		equipment prior to removal of substrates from the carrier
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierIDSlotMapStatus(PTCHAR carr_id, E87_SMVS status);
/*
 desc : 원격 제어 허용 여부 설정
 parm : enable	- [in]  TRUE or FALSE
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetAllowRemoteControl(BOOL enable);
/*
 desc : 원격 제어 허용 여부 설정
 parm : enable	- [in]  TRUE or FALSE
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetAllowRemoteControl(BOOL &enable);
/*
 desc : ProceedWithCarrier service 초기화 (아래 2가지 목적의 경우에 호출 됩니다)
		1. Carrier ID를 Read한 후에 승인할 때, 호출됨
		2. SlotMap을 Read한 후에 승인할 때, 호출됨
		Equipment (장치)에서 직접적으로 호출하는 경우가 있을 수 있고,
		Host에서 호출하는 경우가 있을 수 있습니다.
 parm : port_id	- [in]  Load Port ID (1 or 2)
		carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetProceedWithCarrier(UINT16 port_id, PTCHAR carr_id);
/*
 desc : CancelCarrier service 요청 (Carrier 취소)
 parm : port_id	- [in]  Load Port ID (1 or 2)
		carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCancelCarrier(UINT16 port_id, PTCHAR carr_id);
/*
 desc : CancelCarrierNotification service request (Carrier 취소되었다고 알림)
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
		carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
 retn : TRUE or FALSE
 note : The pParameterList parameter is the same format as the attribute list from the S3F17 message
		Carrier 상태가 READY_TO_UNLOAD인지 여부를 내부에서 확인 후 생성 여부 결정
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierReCreate(UINT16 port_id, PTCHAR carr_id);
/*
 desc : ReleaseCarrier service 요청
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierRelease(PTCHAR carr_id);
/*
 desc : Carrir Location service 요청
 parm : loc_id	- [in]  Load Port 혹은 Buffer가 아닌 다른 곳을 추가하고자 하는 ID (이름. String)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetAddCarrierLocation(PTCHAR loc_id);
/*
 desc : Load Port에 부여 (설치; 관련)된 Carrier ID (Name) 반환
 parm : port_id	- [in]  Load Port ID (1 or 2)
		carr_id	- [out] Carrier ID가 반환되어 저장될 버퍼 포인터
		size	- [in]  'carr_id' 버퍼 크기
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetAssociationCarrierID(UINT16 port_id, PTCHAR carr_id, UINT32 size);
/*
 desc : Load Port에 부여 (설치; 관련)된 Carrier ID (Name) 설정
 parm : port_id	- [in]  Load Port ID (1 or 2)
		carr_id	- [in]  Carrier ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetAssociationCarrierID(UINT16 port_id, PTCHAR carr_id);
/*
 desc : Carrier ID에 부여 (설정; 관련)된 Load Port Number 반환
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		port_id	- [Out] Load Port ID (1 or 2)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetAssociationPortID(PTCHAR carr_id, UINT16 &port_id);
/*
 desc : Carrier ID에 Load Port Number 부여 (이 함수는 특별한 경우를 제외하고, 사용 권장하지 않음)
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		port_id	- [in]  Load Port ID (1 or 2)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetAssociationPortID(PTCHAR carr_id, UINT16 port_id);
/*
 desc : Host로부터 요청받은 Transaction 정보 값 반환
 parm : None
 retn : 요청 받은 Transaction 정보가 저장된 전역 구조체 포인터 반환
*/
API_IMPORT LPG_CHCT uvCIMLib_E87CMS_GetTransactionInfo();
/*
 desc : 장치(CMPS)가 Carrier Tag 읽고 난 후, Host에게 S3F30 메시지를 호출하기 위해 설정
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		tag_id	- [in]  The requested tag data, can be null if a tag is empty or fail to read
		trans_id- [in]  The transaction ID of S3F29 (Carrier Tag Read Request)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
 note : uvCIMLib_E87CMS_GetCarrierTagReadData 호출되고 난 후, Host에게 메시지를 보낼 때, 호출해야 함
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetAsyncCarrierTagReadComplete(PTCHAR carr_id, PTCHAR tag_id,
															   LONG trans_id);
/*
 desc : Carrier로부터 읽은 Tag Data 반환
 parm : loc_id	- [in]  Location identifier
		carr_id	- [in]  Carrier ID (or Name; String)
		data_seg- [in]  Indicates a specific section of data (Tag ID ?)
		data_tag- [out] Carrier로부터 읽어들인 Tag Data가 저장될 버퍼
		size	- [in]  'data_tag' 버퍼 크기
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
 note : loc_id or carr_id 중 1개만 설정되어 있어야 함. 즉, 둘 중에 1개는 NULL이어도 상관 없음
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetCarrierTagReadData(PTCHAR loc_id, PTCHAR carr_id, PTCHAR data_seg,
													  PTCHAR data_tag, UINT32 size);
/*
 desc : Carrier가 임의 (특정) Load Port에서 제거가 시작되고 있다고 알림
		Load Port에 Carrier가 있어야만 가능
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		port_id	- [in]  Load Port ID (1 or 2)
		logs	- [in]  로그 데이터 출력 여부
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
 desc : Carrier가 Load Port에서 내부 버퍼 위치로 이동 되었다고 설정
		즉, Load Port의 Undocked에서 Docked 위치로 이동 했다고 보면 됨
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
 desc : Carrier가 이전에 (특정) Load Port에서 다시 놓여 있다고 알림
		이전에 CarrierOut 되어 있는 곳으로 다시 놓여 있다고 알림
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		logs	- [in]  로그 데이터 출력 여부
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
 desc : 모든 CarrierOut Service 취소
		This service shall cause all CarrierOut services to be removed from the queue.
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelAllCarrierOut service.
		E_FAIL will be returned if the call fails.
		The "status" will contain the text of any error and is in the form of the S3F18 reply message.
		Callbacks will be called.
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCancelAllCarrierOut();
/*
 desc : Load Port의 Transfer Status 값 반환
 parm : port_id	- [in]  Load Port ID (1 or 2)
		status	- [out] Load Port의 Transfer Status 값 반환
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
 note : Get the current load port transfer state
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetTransferStatus(UINT16 port_id, E87_LPTS &status);
/*
 desc : Load Port의 Transfer Status 값 설정
 parm : port_id	- [in]  Load Port ID (1 or 2)
		status	- [in]  Load Port의 Transfer Status 값
 retn : TRUE or FALSE
 note : Sets the Load Port Transfer State for the specified load port.
		The new value can be any state and does not depend on the current state
		!!! important. 수동으로 상태 변환을 해줘야지, 내부적으로 알아서 변환해 주지 않음
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetTransferStatus(UINT16 port_id, E87_LPTS status);
/*
 desc : CancelBind service Request
		Carrier ID와 Load Port 연결 부분을 취소하고 Load Port가 NOT_RESERVED 상태로 변경
 parm : port_id	- [in]  Load Port ID (1 or 2) (-1 or 0 이상 값)
		carr_id	- [in]  Carrier ID (or Name; String) (NULL or String)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelBind service
		port_id 혹은 carr_id 둘 중의 하나의 값만 반드시 입력되어야 됨
		Either portID or carrierID is required, but not both
		Set portID to -1 if it is not used and carrierID to an empty string if it is not used
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCancelBind(UINT16 port_id, PTCHAR carr_id);
/*
 desc : CancelCarrierOut service Request
		현재 Carrier에 관련된 Action을 취소하고,
		장비 (Equipment)가 Carrier를 Load Port의 Unload (혹은 내부 버퍼) 위치로 복구 합니다.
 parm : port_id	- [in]  Load Port ID (1 or 2)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelCarrierAtPort service
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCancelCarrierAtPort(UINT16 port_id);
/*
 desc : CancelCarrierOut service Request
		현재 Carrier에 관련된 CarrierOut 서비스만 Queue에서 제거
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
 desc : 장비 (Equipment)에서 현재 Carrier의 접근 상태 값 설정
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		status	- [in]  Accessing Status (E87_CCAS)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelReserveAtPort service
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetCarrierAccessingStatus(PTCHAR carr_id, E87_CCAS status);
/*
 desc : 장비 (Equipment)에서 현재 Carrier의 접근 상태 값 얻기
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		status	- [out] Accessing Status (E87_CCAS)
 retn : TRUE or FALSE
 note : This method allows the equipment to initiate a CancelReserveAtPort service
*/
API_IMPORT BOOL uvCIMLib_E87CMS_GetCarrierAccessingStatus(PTCHAR carr_id, E87_CCAS &status);
/*
 desc : 현재 Carrier ID 상태가 Closed 상태인지 여부
 parm : carr_id	- [in]  Carrier ID (or Name; String)
 retn : TRUE (Closed), FALSE (Not closed)
*/
API_IMPORT BOOL uvCIMLib_E87CMS_IsCarrierAccessClosed(PTCHAR carr_id);
/*
 desc : Carrier (FOUP) 내의 특정 위치에 Substrate (Wafer)를 놓기
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		slot_no	- [in]  Slot Number (Not sequential number)
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetSubstratePlaced(PTCHAR carr_id, PTCHAR subst_id, UINT8 slot_no);
/*
 desc : Recipe (생성, 수정, 삭제) 이벤트 처리
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		recipe	- [in]  Recipe Name
		type	- [in]  0x01 (Created), 0x02 (Edited), 0x03 (Deleted)
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetRecipeManagement(LONG conn_id, PTCHAR recipe, UINT8 type,
													LONG handle=0);
/*
 desc : Recipe (Selected) 이벤트 처리
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		recipe	- [in]  Recipe Name
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E87CMS_SetRecipeSelected(LONG conn_id, PTCHAR recipe, LONG handle=0);


/* --------------------------------------------------------------------------------------------- */
/*                        E90 Substrate Tracking Specification (CJM)                             */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Substrate (Transport & Processing) 상태 값 변경
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		loc_id	- [in]  Location ID (Name)가 저장된 문자열 버퍼 or 1-Based Index
		trans	- [in]  Substrate transport state
		proc	- [in]  Substrate processing state
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetChangeSubstrateStateName(PTCHAR subst_id, PTCHAR loc_id,
															E90_SSTS trans, E90_SSPS proc);
API_IMPORT BOOL uvCIMLib_E90STS_SetChangeSubstrateStateIndex(PTCHAR subst_id, ENG_TSSI loc_id,
															 E90_SSTS trans, E90_SSPS proc);
/*
 desc : Substrate (Transport & Processing) 상태 값 변경
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		loc_id	- [in]  Location ID (Name)가 저장된 문자열 버퍼 or 1-Based Index
						Substrate Location ID_xx (문자열. RobotArm / PreAligner / ProcessChamber / Buffer)
		trans	- [in]  Substrate transport state
		proc	- [in]  Substrate processing state
		status	- [in]  Substrate ID state E90_SISM
		time_in	- [in]  Batch가 Location ID에 도착하는 시간 (NULL이면 사용 안함)
		time_out- [in]  Batch가 Location ID에 이탈하는 시간 (NULL이면 사용 안함)
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
 desc : Substrate Processing 상태 값 변경
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		proc	- [in]  Substrate processing state
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetChangeSubstrateProcessingState(PTCHAR subst_id, E90_SSPS proc);
/*
 desc : Substrate Location (위치;지점) 등록
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		loc_id	- [in]  Location ID (Name)가 저장된 문자열 버퍼 or 1-Based Index
						Substrate Location ID_xx (문자열. RobotArm / PreAligner / ProcessChamber / Buffer)
		lot_id	- [in]  Lot ID (If NULL, Not used)
						Lot ID (Wafer 마다 부여된 식별 번호)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_RegisterSubstrateName(PTCHAR subst_id, PTCHAR loc_id,
													  INT16 sub_id=0, PTCHAR lot_id=NULL);
API_IMPORT BOOL uvCIMLib_E90STS_RegisterSubstrateIndex(PTCHAR subst_id, ENG_TSSI loc_id,
													   INT16 sub_id=0, PTCHAR lot_id=NULL);
/*
 desc : Substrate ID 제거
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetRemoveSubstrate(PTCHAR subst_id);
/*
 desc : Substrate Location (Wafer가 놓여질 위치 이름) 등록
 변수 :	loc_id	- [in]  Location ID (Name)가 저장된 문자열 버퍼
						Substrate Location ID_xx (문자열. RobotArm / PreAligner / ProcessChamber / Buffer)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetAddSubstrateLocation(PTCHAR loc_id);
/*
 desc : Substrate Location (Wafer가 놓여질 위치 이름) 제거
 변수 :	loc_id	- [in]  Location ID (Name)가 저장된 문자열 버퍼
							Substrate Location ID_xx (문자열. RobotArm / PreAligner / ProcessChamber / Buffer)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetRemoveSubstrateLocation(PTCHAR loc_id);
/*
 desc : Substrate (Wafer)를 승인 하도록 (받도록) 장비 쪽을 호출
		장비 쪽에서 내부적으로 E90CommandCallback 함수가 호출됨
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
 retn : TRUE or FALSE
 note : Request the equipment to accept the substrate.
		This function executes the E90CommandCallback() to allow the equipment to reject the command.
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetProceedWithSubstrate(PTCHAR subst_id);
/*
 desc : Remote Control 허용 여부 값 반환
 parm : enable	- [out] Enable or Disable
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetAllowRemoteControl(BOOL &enable);
/*
 desc : Remote Control 허용 여부 설정
 parm : enable	- [in]  Enable or Disable
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetAllowRemoteControl(BOOL enable);
/*
 desc : Substrate Location ID의 상태 값 반환
 parm : loc_id	- [in]  Location ID (Name)가 저장된 문자열 버퍼 or 1-Based Index
						Substrate Location ID_xx (문자열. RobotArm / PreAligner / ProcessChamber / Buffer)
		state	- [out] Substrate Location State 값 반환
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateLocationStateName(PTCHAR loc_id, E90_SSLS &state);
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateLocationStateIndex(ENG_TSSI loc_id, E90_SSLS &state);
/*
 desc : Substrate Location ID의 상태 값 설정
 parm : loc_id	- [in]  Location ID (Name)가 저장된 문자열 버퍼 or 1-Based Index
						Substrate Location ID_xx (문자열. RobotArm / PreAligner / ProcessChamber / Buffer)
		state	- [in]  Substrate Location State 값 반환 (E90_SSLS)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateLocationStateName(PTCHAR loc_id, E90_SSLS state);
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI loc_id, E90_SSLS state);
/*
 desc : Substrate Processing의 이전 상태 값 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		state	- [out] Substrate Location State 값 반환 (E90_SSPS)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstratePrevProcessingState(PTCHAR subst_id, E90_SSPS &state);
/*
 desc : Substrate Processing 상태 값 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		state	- [out] Substrate Location State 값 반환 (E90_SSPS)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateProcessingState(PTCHAR subst_id, E90_SSPS &state);
/*
 desc : Substrate Processing 상태 값 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		state	- [in]  설정하고자 하는 상태 값 (E90_SSPS)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateProcessingState(PTCHAR subst_id, E90_SSPS state);
/*
 desc : Substrate Transport 이전 상태 값 반환
 parm : subst_id- [in]  Substrate ID (문자열)
		state	- [out] Substrate Transport State 값 반환 (E90_SSTS)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstratePrevTransportState(PTCHAR subst_id, E90_SSTS &state);
/*
 desc : Substrate Transport 상태 값 반환
 parm : subst_id- [in]  Location ID (Name)가 저장된 문자열 버퍼 or 1-Based Index
						Substrate Location ID_xx (문자열. RobotArm / PreAligner / ProcessChamber / Buffer)
		state	- [out] Substrate Location State 값 반환 (E90_SSPS)
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateTransportStateName(PTCHAR subst_id, E90_SSTS &state);
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateTransportStateIndex(ENG_TSSI subst_id, E90_SSTS &state);
/*
 desc : Substrate Transport 상태 값 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		state	- [in]  설정하고자 하는 상태 값 (E90_SSTS)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateTransportState(PTCHAR subst_id, E90_SSTS state);
/*
 desc : Substrate ID (Tag ? Serial ? Bar Code)의 이전 상태 값 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		state	- [out] Enumeration values for the ID Reader state machine
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstratePrevIDStatus(PTCHAR subst_id, E90_SISM &status);
/*
 desc : Substrate ID (Tag ? Serial ? Bar Code) 읽어진 값 상태 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		state	- [out]  Enumeration values for the ID Reader state machine
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateIDStatus(PTCHAR subst_id, E90_SISM &status);
/*
 desc : Substrate ID (Tag ? Serial ? Bar Code) 읽어진 값 상태 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		state	- [in]  Enumeration values for the ID Reader state machine
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateIDStatus(PTCHAR subst_id, E90_SISM status);
/*
 desc : CIM300 (Host)에게 Substrate ID (Tag ? Serial ? Bar Code) 읽어진 값 상태를 판단해 달라고 요청
 parm : subst_id	- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		acquired_id	- [in]  Substrate ID acquired from the substrate ID reader (Serial? Tag?)
		read_good	- [in]  TRUE if read was successful. Otherwise FALSE
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetNotifySubstrateRead(PTCHAR subst_id, PTCHAR acquired_id,
													   BOOL read_good);
/*
 desc : Substrate ID Reader가 판독 가능 혹은 불가능 할 때, 이벤트 발생 시킴
 parm : enable	- [in]  Substrate ID Reader가 사용 가능한지 여부 설정
						TRUE: 사용 가능할 때, 이벤트 알림
						FALSE:사용 불가능할 때, 이벤트 알림
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateReaderAvailable(INT32 enable);
/*
 desc : Request to the equipment to skip the substrate and move it to its source or destination location
		장치에게 substrate를 건너띄고, 소스 혹은 다른 위치로 이동하라고 요청 함 (원래 위치로 이동하라고 함)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
 retn : TRUE or FALSE
 note : This function executes the E90CommandCallback() to allow the equipment to reject the command
		즉, 이 함수를 호출하면, 장비 내에서 거부될 경우, E90CommandCallback 함수가 호출 됨
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetCancelSubstrate(PTCHAR subst_id);
/*
 desc : 실제 Substrate ID에서 읽어들인 Wafer Reader Code (Tag, Serial, Bar Code, etc) 값을 반환
 parm : subst_id	- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		acquired_id	- [out] Wafer Reader로부터 읽어들인 ID 값 반환 (문자열)
		size		- [in]  'acquired_id' 버퍼 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateIDToAcquiredID(PTCHAR subst_id, PTCHAR acquired_id, UINT32 size);
/*
 desc : 실제 Substrate ID에서 읽어들인 Wafer Reader Code (Tag, Serial, Bar Code, etc) 값 설정
 parm : subst_id	- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		acquired_id	- [in]  Wafer Reader로부터 읽어들인 ID 값 (문자열)
 retn : TRUE or FALSE
 note : Contains the ID read from the substrate. Empty string before the substrate is read.
		The attribute shall be updated as soon the substrate ID has been successfully read
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateIDToAcquiredID(PTCHAR subst_id, PTCHAR acquired_id);
/*
 desc : Substrate ID 값 반환 (Slot Number; Slot No)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		slot_no	- [out] Slot Number 값이 반환되어 저장 (1-based or Later. Max. 255)	
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateIDToSlotNo(PTCHAR subst_id, UINT8 &slot_no);
/*
 desc : Substrate Location ID 값 반환 (Slot Number; Slot No)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		loc_id	- [out] Location ID (Buffer, Robot, PreAligner, Chamber, etc)가 저장될 버퍼
		size	- [in]  'loc_id' 버퍼 크기
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateLocationID(PTCHAR subst_id, PTCHAR loc_id, UINT32 size);
/*
설명 : 해당 위치에 웨이퍼가 존재하는지 여부
변수 : loc_id	- [in]  Location ID (Buffer, Robot, PreAligner, Chamber, etc)
반환 : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_IsLocationSubstrateID(ENG_TSSI loc_id);
/*
 desc : 특정 wafer (substrate ID; st_id)를 임의 location에 생성과 동시에 각종 상태 값 지정
 parm : subst_id	- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		loc_id		- [in]  Location ID (문자열)
		proc_state	- [in]  Processing State (E90_SSPS)
		trans_state	- [in]  Transfer State (E90_SSTS)
		read_status	- [in]  ID Read Status (E90_SISM)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateAnyLocStates(PTCHAR subst_id, PTCHAR loc_id,
														 E90_SISM read_status, E90_SSPS proc_state,
														 E90_SSTS trans_state);
/*
 desc : 현재 Wafer의 세트 (Lot; FOUP)가 놓여진 객체의 위치 정보를 반환 한다
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		index	- [out] Wafer의 세트 (Lot; FOUP)가 놓여진 위치 (인덱스) 값
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetBatchLocIdx(PTCHAR loc_id, INT16 &index);
/*
 desc : 현재 Wafer의 세트 (Lot; FOUP)가 놓여진 객체의 위치 정보를 반환 한다
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		index	- [out] Wafer의 세트 (Lot; FOUP)가 놓여진 위치 (인덱스) 값
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetBatchLocIdx(PTCHAR loc_id, INT16 index);
/*
 desc : 현재 Wafer의 세트 (Lot; FOUP)가 놓여진 객체의 점유 상태 여부
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_IsBatchLocOccupied(PTCHAR loc_id);
/*
 desc : 현재 Wafer의 세트 (Lot; FOUP)가 놓여진 객체의 점유 상태 여부
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		state	- [in]  Batch State 값 즉, 점유 (1), 점유 안함 (0)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetBatchLocState(PTCHAR loc_id, E90_SSLS state);
/*
 desc : 현재 Wafer의 세트 (Lot; FOUP)에 저장(등록)된 Substrate ID 목록 반환
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [out] loc_id에 위치한 Wafer의 세트 (Lot; FOUP)의 Substrate ID 목록이 저장될 리스트 버퍼
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetBatchSubstIDMap(PTCHAR loc_id, CStringArray &subst_id);
/*
 desc : 현재 Wafer의 세트 (Lot; FOUP)에 저장(등록)된 Substrate ID 목록 저장 (등록)
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [in]  loc_id에 위치한 Wafer의 세트 (Lot; FOUP)의 Substrate ID 목록이 저장된 리스트 버퍼
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetBatchSubstIDMap(PTCHAR loc_id, CStringArray *subst_id);
/*
 desc : Location ID에 대한 이벤트를 보낼지 여부에 대한 정보 얻기
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		flag	- [out] FALSE : 이벤트 보냄 (알림), TRUE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetDisableEventsLoc(PTCHAR loc_id, BOOL &flag);
/*
 desc : Location ID에 대한 이벤트를 보낼지 여부에 대한 정보 설정
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		flag	- [out] FALSE : 이벤트 보냄 (알림), TRUE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetDisableEventsLoc(PTCHAR loc_id, BOOL flag);
/*
 desc : Batch ID에 대한 이벤트를 보낼지 여부에 대한 정보 얻기
 parm : batch_id- [in]  ID of substrate batch
		flag	- [out] FALSE : 이벤트 보냄 (알림), TRUE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetDisableEventsBatch(PTCHAR batch_id, BOOL &flag);
/*
 desc : Batch ID에 대한 이벤트를 보낼지 여부에 대한 정보 설정
 parm : batch_id- [in]  ID of substrate batch
		flag	- [out] FALSE : 이벤트 보냄 (알림), TRUE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetDisableEventsBatch(PTCHAR batch_id, BOOL flag);
/*
 desc : Batch (Wafer Collections)에 등록할 수 있는 최대 개수 반환
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		max_pos	- [in]  최대 등록 가능한 Substrate position 값이 반환될 버퍼
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetMaxSubstratePosition(PTCHAR loc_id, LONG &max_pos);
/*
 desc : Batch (Wafer Collections)에 등록할 수 있는 최대 개수 반환
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		name	- [out] Object Name이 저장될 버퍼
		size	- [in]  'name' 버퍼의 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetObjectName(PTCHAR loc_id, PTCHAR name, UINT32 size);
/*
 desc : Update <batch location> GEM data (Batch Location (위치) GEM Data 갱신을 알림?)
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
 note : Updates the batch location status variables
*/
API_IMPORT BOOL uvCIMLib_E90STS_SendDataNotificationBatch(PTCHAR loc_id);
/*
 desc : Update <substrate location> GEM variables for this object
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
 note : GEM 변수 - SubstID, SubstLocID, SubstLocState, SubstLocState_i, and SubstLocSubstID_i
*/
API_IMPORT BOOL uvCIMLib_E90STS_SendDataNotificationSubst(PTCHAR loc_id);
/*
 desc : Substrate Batch Location ID 생성
 parm : loc_id	- [in]  생성하고자 하는 Batch Location ID (문자열)
		loc_num	- [in]  Batch Location에서 substrate locations의 개수 (Carrier 내에 등록? 가능한 substrate location 개수)
		sub_id	- [in]  Location Group ID를 위해 사용하는 Subscript Index (location's index)
						EPJ 파일에서 해당 위치에 사용되는 첨자?(Subscripted) 변수의 세트 (set) -> EPJ 파일에 정의된 내용과 연관성이 있을듯...
						사용하지 않을 때는 0 값
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_AddSubstrateBatchLocation(PTCHAR loc_id, INT32 loc_num, INT16 sub_id=0);
/*
 desc : Substrate Batch Location ID 제거
 parm : loc_id	- [in]  생성하고자 하는 Batch Location ID (문자열)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_DelSubstrateBatchLocation(PTCHAR loc_id);
/*
 desc : Batch에 캐리어의 기판을 추가 함
 parm : batch_id- [in]  ID of substrate batch
		carr_id	- [in]  ID of carrier
		slot_map- [in]  1 or 0 or -1
						TRUE : 각 Slot map item = 0에 대해 위치 값을 비워서 추가 함
							   즉, 각 Slot의 위치 값을 초기화 후 추가하도록 처리
							   (사용하지 않을 때는 -1)
 retn : TRUE or FALSE
 note : Batch	- 프로세스 자원에서 동시에 처리할 기판의 그룹 (뭉치?)
*/
API_IMPORT BOOL uvCIMLib_E90STS_AddCarrierToBatch(LONG batch_id, PTCHAR carr_id, LONG slot_map=-1);
/*
 desc : Batch에 캐리어의 기판을 제거 함
 parm : batch_id- [in]  ID of substrate batch
		carr_id	- [in]  ID of carrier
 retn : TRUE or FALSE
 note : Batch	- 프로세스 자원에서 동시에 처리할 기판의 그룹 (뭉치?)
*/
API_IMPORT BOOL uvCIMLib_E90STS_DelCarrierFromBatch(LONG batch_id, PTCHAR carr_id);
/*
 desc : 새로운 Substrate Batch 생성
 parm : batch_id- [in]  ID of substrate batch
		loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_CreateBatch(LONG batch_id, PTCHAR loc_id);
/*
 desc : 기존 Substrate Batch 제거
 parm : batch_id- [in]  ID of substrate batch
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_DestroyBatch(LONG batch_id);
/*
 desc : Batch에 있는 모든 기판들의 이송 및 처리 상태 뿐만 아니라, 위치도 변경 함
 parm : batch_id	- [in]  ID of substrate batch
		loc_id		- [in]  위치 변경하고자 하는 ID (Location ID: Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		trans_state	- [in]  이송 상태 값 즉, E90_SSTS
		proc_state	- [in]  처리 상태 값 즉, E90_SSPS
		time_in		- [in]  Batch가 Location ID에 도착하는 시간 (NULL이면 사용 안함)
		time_out	- [in]  Batch가 Location ID에 이탈하는 시간 (NULL이면 사용 안함)
							time format : YYYYMMDDhhmmsscc for YearMonthDayHourMinuteSecondCentisecond
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_ChangeBatchState(LONG batch_id, PTCHAR loc_id,
												 E90_SSTS trans_state, E90_SSPS proc_state,
												 PTCHAR time_in=NULL, PTCHAR time_out=NULL);
/*
 desc : Substrate Location 등록
 parm : loc_id	- [in]  위치 변경하고자 하는 ID (Location ID: Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		sub_id	- [in]  Location Group ID를 위해 사용하는 Subscript Index (location's index)
						EPJ 파일에서 해당 위치에 사용되는 첨자?(Subscripted) 변수의 세트 (set)
						-> EPJ 파일에 정의된 내용과 연관성이 있을듯...
						사용하지 않을 때는 0 값
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_AddSubstrateLocation(PTCHAR loc_id, PTCHAR subst_id, INT16 sub_id);
/*
 desc : Substrate Location 제거
 parm : loc_id	- [in]  위치 변경하고자 하는 ID (Location ID: Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_DelSubstrateLocation(PTCHAR loc_id);
/*
 desc : Batch에 Substrate 추가
 parm : batch_id- [in]  ID of substrate batch
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
 retn : TRUE or FALSE
 note : Batch	- 프로세스 자원에서 동시에 처리할 기판의 그룹 (뭉치?)
*/
API_IMPORT BOOL uvCIMLib_E90STS_AddSubstrateToBatch(LONG batch_id, PTCHAR subst_id);
/*
 desc : Batch에서 Substrate 제거
 parm : batch_id- [in]  ID of substrate batch
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
 retn : TRUE or FALSE
 note : Batch	- 프로세스 자원에서 동시에 처리할 기판의 그룹 (뭉치?)
*/
API_IMPORT BOOL uvCIMLib_E90STS_DelSubstrateFromBatch(LONG batch_id, PTCHAR subst_id);
/*
 desc : Batch에 비어있는 substrate 공간을 추가 즉, Substrate가 없는 Batch만 추가
 parm : batch_id- [in]  ID of substrate batch
 retn : TRUE or FALSE
 note : Batch	- 프로세스 자원에서 동시에 처리할 기판의 그룹 (뭉치?)
*/
API_IMPORT BOOL uvCIMLib_E90STS_AddEmptySpaceToBatch(LONG batch_id);
/*
 desc : subscript indices의 범위를 예약 (?)
 parm : base_sub_index	- [in]  first subscript index to reserve
		reserve_size	- [in]  number of indices to reserve
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_ReserveSubstrateLocationIndices(INT16 base_sub_index,
																INT16 reserve_size);
/*
 desc : Substrate Transport State 변경
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		state	- [in]  E90_SSTS (Substrate Transport State)
		time_in	- [in]  Batch가 Location ID에 도착하는 시간 (NULL이면 사용 안함)
		time_out- [in]  Batch가 Location ID에 이탈하는 시간 (NULL이면 사용 안함)
						time format : YYYYMMDDhhmmsscc for YearMonthDayHourMinuteSecondCentisecond
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_ChangeSubstrateTransportState(PTCHAR subst_id, PTCHAR loc_id,
															  E90_SSTS state,
															  PTCHAR time_in=NULL, PTCHAR time_out=NULL);
/*
 desc : Substrate Material Status 값 반환 (기판의 처리 상태 값 반환. 처리 여부 ?)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		status	- [out] Material Status (Material 처리 여부) 값이 반환될 참조 변수
						장치마다 반환되는 값이 다르다? (EPJ 파일 참조?)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateMaterialStatus(PTCHAR subst_id, LONG &status);
/*
 desc : Substrate Material Status 값 변경 (기판의 처리 상태 값 변경. 처리 여부 ?)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		status	- [in]  Material Status (Material 처리 여부) 값
						장치마다 설정하는는 값이 다르다? (EPJ 파일 참조?)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateMaterialStatus(PTCHAR subst_id, LONG status);
/*
 desc : 장비에 다루는 Substrate의 Type 설정 (Wafer, Flat Panel, CD, Mask, etc...)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		type	- [in]  E90_EMST 즉, wafer, flat_panel, cd, mask 중 하나
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetCurrentSubstrateType(PTCHAR subst_id, E90_EMST type);
/*
 desc : 장비에 다루는 Substrate의 Type 설정 (Wafer, Flat Panel, CD, Mask, etc...)
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		type	- [out] E90_EMST 즉, wafer, flat_panel, cd, mask 중 하나
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetCurrentSubstrateType(PTCHAR subst_id, E90_EMST &type);
/*
 desc : Substrate ID가 Reader기로부터 성공적으로 읽어졌는지 즉, Host로부터 Confirm을 받았는지 여부
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		read	- [out] 정상적으로 ID를 읽었는지 여부 값 (TRUE or FALSE)이 반환될 참조 변수
 note : Equipment에서는 NotifySubstrateRead 함수 호출해서 Substrate ID가 정상인지를 Host에게 요청
		이 NotifySubstrateRead 함수 호출 후에, 현재 함수 호출하면 goodRead인지 badRead인지 알 수 있음
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateGoodRead(PTCHAR subst_id, BOOL &read);
/*
 desc : Substrate ID가 Reader기로부터 읽어들인 상태 값 즉, TRUE (good) or FALSE (bad) 값 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		read	- [in]  정상적으로 ID를 읽었다면 TRUE, 아니라면 FALSE 설정
 note : Equipment에서는 NotifySubstrateRead 함수 호출해서 Substrate ID가 정상인지를 Host에게 요청
		이 NotifySubstrateRead 함수 호출 후에, 현재 함수 호출하면 goodRead인지 badRead인지 알 수 있음
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateGoodRead(PTCHAR subst_id, BOOL read);
/*
 desc : Substrate ID와 관련된 Lot ID 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		lot_id	- [out] Lot ID가 저장될 버퍼
		size	- [in]  'lot_id' 버퍼 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateLotID(PTCHAR subst_id, PTCHAR lot_id, UINT32 size);
/*
 desc : Substrate ID와 관련된 Lot ID 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		lot_id	- [in]  Lot ID가 저장된 버퍼
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateLotID(PTCHAR subst_id, PTCHAR lot_id);
/*
 desc : Substrate ID와 관련된 Lot ID 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		obj_id	- [out] Object ID가 저장될 버퍼 (여기서는 Substrate ID와 동일한 텍스트?)
		size	- [in]  'lot_id' 버퍼 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateObjID(PTCHAR subst_id, PTCHAR obj_id, UINT32 size);
/*
 desc : Substrate ID와 관련된 Object Type 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		obj_type- [out] Object Type이 저장될 버퍼 ("BatchLoc" or "Substrate" or "SubstLoc")
						여기서는 보통 "Substrate"
		size	- [in]  'obj_type' 버퍼 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateObjType(PTCHAR subst_id, PTCHAR obj_type, UINT32 size);
/*
 desc : Location ID와 관련된 Object ID 반환
 parm : loc_id	- [out] Location ID가 저장될 버퍼
		obj_id	- [out] Object ID가 저장될 버퍼 (여기서는 Location ID와 동일한 텍스트?)
		size	- [in]  'lot_id' 버퍼 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetLocationObjID(PTCHAR loc_id, PTCHAR obj_id, UINT32 size);
/*
 desc : Location ID와 관련된 Object Type 반환
 parm : loc_id	- [out] Location ID가 저장될 버퍼
		obj_type- [out] Object Type이 저장될 버퍼 ("BatchLoc" or "Substrate" or "SubstLoc")
						여기서는 보통 "Substrate"
		size	- [in]  'obj_type' 버퍼 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetLocationObjType(PTCHAR loc_id, PTCHAR obj_type, UINT32 size);
/*
 desc : Substrate가 속한 Source CarrierID 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		carr_id	- [out] sourceCarrierID가 저장될 버퍼
		size	- [in]  'carr_id' 버퍼 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSourceCarrierID(PTCHAR subst_id, PTCHAR carr_id, UINT32 size);
/*
 desc : Substrate가 속할 Source CarrierID 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		carr_id	- [in]  sourceCarrierID가 저장될 버퍼
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSourceCarrierID(PTCHAR subst_id, PTCHAR carr_id);
/*
 desc : Substrate가 속한 Source LoadPort ID 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		port_id	- [out] Load Port ID가 저장될 참조 변수 (1 or Later)
 retn : TRUE or FALSE
 note : Load Port - the interface location on the equipment where carriers are loaded and unloaded
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSourceLoadPortID(PTCHAR subst_id, LONG &port_id);
/*
 desc : Substrate가 속할 Source LoadPort ID 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		port_id	- [in]  sourceLoadPortID가 저장될 버퍼 (1 or Later)
 retn : TRUE or FALSE
 note : Load Port - the interface location on the equipment where carriers are loaded and unloaded
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSourceLoadPortID(PTCHAR subst_id, LONG port_id);
/*
 desc : Substrate가 속한 Source Slot ID 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		port_id	- [out] Slot ID가 저장될 참조 변수 (1 or Later)
 retn : TRUE or FALSE
 note : slot - physical location capable of holding a substrate within cassette type carrier
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSourceSlotID(PTCHAR subst_id, LONG &slot_id);
/*
 desc : Substrate가 속할 Source Slot ID 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		port_id	- [in]  sourceSlotID가 저장될 버퍼 (1 or Later)
 retn : TRUE or FALSE
 note : slot - physical location capable of holding a substrate within cassette type carrier
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSourceSlotID(PTCHAR subst_id, LONG slot_id);
/*
 desc : Substrate의 마지막 이벤트 (작업)와 관련된 Substrate의 현재 위치 반환
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		loc_id	- [out] Location ID가 저장될 버퍼
		size	- [in]  'loc_id' 버퍼 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetCurrentSubstrateLocID(PTCHAR subst_id, PTCHAR loc_id, UINT32 size);
/*
 desc : Substrate의 마지막 이벤트 (작업)와 관련된 Substrate의 현재 위치 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		loc_id	- [in]  Location ID가 저장된 버퍼
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetCurrentSubstrateLocID(PTCHAR subst_id, PTCHAR loc_id);
/*
 desc : Substrate의 Source ID 얻기
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		src_id	- [out] Source ID가 저장될 버퍼
		size	- [in]  'src_id' 버퍼 크기
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateSourceID(PTCHAR subst_id, PTCHAR src_id, UINT32 size);
/*
 desc : Substrate의 Source ID 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		src_id	- [in]  Source ID가 저장된 버퍼
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateSourceID(PTCHAR subst_id, PTCHAR src_id);
/*
 desc : Substrate의 Destination ID 얻기
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		tgt_id	- [out] Source ID가 저장될 버퍼
		size	- [in]  'src_id' 버퍼 크기
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateDestinationID(PTCHAR subst_id, PTCHAR tgt_id, UINT32 size);
/*
 desc : Substrate의 Destination ID 설정
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		tgt_id	- [in]  Source ID가 저장된 버퍼
 retn : TRUE or FALSE
 note : Substrate ID 얻는 방법 - ICxE87CMS::GetSubstrateFromSource - ICxSubstrate from the source carrier and slot number
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateDestinationID(PTCHAR subst_id, PTCHAR tgt_id);
/*
 desc : 동일한 전이 (Transition)가 진행되는 관련 개체 그룹에 대해 전송되는 개별 이벤트를 억제하는데 사용
		객체 (Object)가 개별 이벤트를 억제하는지 여부 확인
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		flag	- [out] TRUE : 이벤트 보냄 (알림), FALSE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetUsingGroupEventsSubst(PTCHAR subst_id, BOOL &flag);
/*
 desc : 동일한 전이 (Transition)가 진행되는 관련 개체 그룹에 대해 전송되는 개별 이벤트를 억제하는데 사용
		객체 (Object)가 개별 이벤트를 억제 혹은 승인하는지 여부 확인
 parm : subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
		flag	- [in]  TRUE : 이벤트 보냄 (알림), FALSE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetUsingGroupEventsSubst(PTCHAR subst_id, BOOL flag);
/*
 desc : 동일한 전이 (Transition)가 진행되는 관련 개체 그룹에 대해 전송되는 개별 이벤트를 억제하는데 사용
		객체 (Object)가 개별 이벤트를 억제하는지 여부 확인
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		flag	- [out] TRUE : 이벤트 보냄 (알림), FALSE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SGetUsingGroupEventsLoc(PTCHAR loc_id, BOOL &flag);
/*
 desc : 동일한 전이 (Transition)가 진행되는 관련 개체 그룹에 대해 전송되는 개별 이벤트를 억제하는데 사용
		객체 (Object)가 개별 이벤트를 억제 혹은 승인하는지 여부 확인
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		flag	- [in]  TRUE : 이벤트 보냄 (알림), FALSE : 이벤트 보내지 않음 (알리지 않음)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetUsingGroupEventsLoc(PTCHAR loc_id, BOOL flag);
/*
 desc : Location에 위치한 Substrate ID 얻기
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [out] Substrate ID가 저장될 버퍼
		size	- [in]  'subst_id' 버퍼 크기
 retn : TRUE or FALSE
*/															   
API_IMPORT BOOL uvCIMLib_E90STS_GetLocationSubstID(PTCHAR loc_id, PTCHAR subst_id, UINT32 size);
/*
 desc : Location에 위치한 Substrate ID 설정
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		subst_id- [in]  Substrate ID가 저장된 버퍼
 retn : TRUE or FALSE
*/															   
API_IMPORT BOOL uvCIMLib_E90STS_SetLocationSubstID(PTCHAR loc_id, PTCHAR subst_id);
/*
 desc : Location에 위치한 Location Index 얻기
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		index	- [out] Location Index가 반환될 참조 변수 (1 or Later)
 retn : TRUE or FALSE
*/															   
API_IMPORT BOOL uvCIMLib_E90STS_GetLocationIndex(PTCHAR loc_id, INT16 &index);
/*
 desc : Location에 위치한 Substrate ID 설정
 parm : loc_id	- [in]  Location ID (Port1, Port2, ProcessChamber1, ProcessChamber2, PreAligner, Robot, Buffer, etc)
		index	- [in]  Location Index 값 (1 or Later)
 retn : TRUE or FALSE
*/															   
API_IMPORT BOOL uvCIMLib_E90STS_SetLocationIndex(PTCHAR loc_id, INT16 index);
/*
 desc : Substrate History 반환
 parm : subst_id- [in]  Substrate ID가 저장된 버퍼
		lst_hist- [out] { {loc_id, time_in, time_out}, {loc_id, time_in, time_out}, ..., } 값이 반환될 리스트
						time_in/out = YearMonthDayHourMinuteSecondCentisecond
 retn : TRUE or FALSE
 날짜 : 2021.03.08 (Modified by Jinsoo.Kang at philoptics)
 note : N/A
*/
API_IMPORT BOOL uvCIMLib_E90STS_GetSubstrateHistory(PTCHAR subst_id, CAtlList <STG_CSHI> &lst_hist);
/*
 desc : Substrate History 반환
 parm : subst_id- [in]  Substrate ID가 저장된 버퍼
		lst_hist- [in]  { {loc_id, time_in, time_out}, {loc_id, time_in, time_out}, ..., } 값이 저장된 리스트
						time_in/out = YearMonthDayHourMinuteSecondCentisecond
 retn : TRUE or FALSE
 날짜 : 2021.03.08 (Modified by Jinsoo.Kang at philoptics)
 note : N/A
*/
API_IMPORT BOOL uvCIMLib_E90STS_SetSubstrateHistory(PTCHAR subst_id, CAtlList <STG_CSHI> *lst_hist);


/* --------------------------------------------------------------------------------------------- */
/*                             E94 Control Job Management (CJM)                                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Process Job들 중에서 한 개가 완료되면 Control Job에게 알림
 parm : pj_id	- [in]  Object ID of the Process Job
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_SetPRJobProcessComplete(PTCHAR pj_id);
/*
 desc : 현재 Queue에 등록된 Control Job ID 목록 및 각 상태 값들 반환
 parm : list_job	- [out] 반환되어 저장될 Control Job ID (String)
		list_state	- [out] 수집된 Control Job IDs에 대한 상태 값이 저장될 리스트
		logs		- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobList(CStringArray &list_job,
												  CAtlList<E94_CCJS> &list_state);
/*
 desc : Process Jobs List 얻기 (Control Job 내부에 저장된 Project Job List)
 parm : list_job- [out] 수집된 Control Job IDs (Names) 저장될 배열 버퍼
		filter	- [in]  Control Job 추출 필터 (필터)
 retn : TRUE ro FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobListId(CStringArray &list_job,
													E94_CLCJ filter=E94_CLCJ::en_list_all);
/*
 desc : Process Jobs List 얻기 (필터링 적용)
 parm : list_job	- [out] 수집된 Control Job IDs (Names) 저장될 배열 버퍼
		list_state	- [out] 수집된 Control Job IDs에 대한 상태 값이 저장될 리스트
		filter		- [in]  Control Job 추출 필터 (필터)
		logs		- [in]  로그 출력 여부
 retn : TRUE ro FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobListIdState(CStringArray &list_job,
														 CAtlList<E94_CCJS> &list_state,
														 E94_CLCJ filter=E94_CLCJ::en_list_all);
/*
 desc : Control Job ID 중 필터에 의해 검색된 것 중 첫 번째 검색된 Job ID 반환
 parm : filter	- [in]  Process Job ID 검색 필터 (E40_PPJS)
		cj_id	- [out] 반환되어 저장될 Control Job ID (String)
		size	- [in]  'cj_id' 버퍼 크기
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobFirstID(E94_CCJS filter, PTCHAR cj_id, UINT32 size);
/*
 desc : Control Job의 상태 정보 반환
 parm : cj_id	- [in]  Control Job ID
		state	- [out] Job ID의 상태 값 반환
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobState(PTCHAR cj_id, E94_CCJS &state);
/*
 desc : Control Job 생성
 parm : cj_id		- [in]  Control Job ID
		carr_id		- [in]  문자열 ID (전역으로 관리되는 ID)
		pj_id		- [in]  Process Job List 가 저장된 Array 배열
		job_order	- [in]  Job Order (E94_CPOM 즉, 3: List, 1: Arrival)
		start		- [in]  Auto Start 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_CreateJob(PTCHAR cj_id, PTCHAR carr_id, CStringArray *pj_id,
										  E94_CPOM job_order, BOOL start);
/*
 desc : Process Jobs List 얻기 (Control Job 내부에 저장된 Project Job List)
 parm : cj_id		- [in]  Object ID of the Control Job
		list_job	- [out] 수집된 Process Job Names 저장될 배열 버퍼
		list_state	- [out] 수집된 Control Job IDs (Names)의 State가 저장될 리스 버퍼
		filter		- [in]  Control Job 내부에 저장된 Process Job 추출 필터 (필터)
		logs		- [in]  로그 출력 여부
 retn : TRUE ro FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetProcessJobListIdState(PTCHAR cj_id,
														 CStringArray &list_job,
														 CAtlList <E40_PPJS> &list_state,
														 E94_CLPJ filter);
/*
 desc : Process Jobs List 얻기 (Control Job 내부에 저장된 Project Job List)
 parm : cj_id	- [in]  Object ID of the Control Job
		list_job- [out] 수집된 Process Job Names 저장될 배열 버퍼
		filter	- [in]  Control Job 내부에 저장된 Process Job 추출 필터 (필터)
		logs	- [in]  로그 출력 여부
 retn : TRUE ro FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetProcessJobList(PTCHAR cj_id, CStringArray &list_job,
												  E94_CLPJ filter=E94_CLPJ::en_list_all);
/*
 desc : Control Job을 선택하기
 parm : cj_id		- [in]  Control Job ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_SetSelectedControlJob(PTCHAR cj_id);
/*
 desc : Control Job을 선택 해제하기
 parm : cj_id		- [in]  Control Job ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_SetDeselectedControlJob(PTCHAR cj_id);
/*
 desc : Remote Control 허용 여부 값 반환
 parm : enable	- [out] Enable or Disable
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetAllowRemoteControl(BOOL &enable);
/*
 desc : Remote Control 허용 여부 설정
 parm : enable	- [in]  Enable or Disable
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_SetAllowRemoteControl(BOOL enable);
/*
설명 : Control Job 내부에서 다음 ProcessJob을 시작시킨다.
변수 : cj_id	- [in]  Control Job ID
반환 : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_StartNextProcessJob(PTCHAR cj_id);
/*
설명 : 다음 Control Job을 시작시킨다.
변수 : cj_id	- [in]  Control Job ID
반환 : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_StartNextControlJob();

/* --------------------------------------------------------------------------------------------- */
/*                        E116 Equiptment Performance Tracking (EPT)                             */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : EPT (Equiptment Performance Tracking) - Busy 설정
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
 desc : EPT (Equiptment Performance Tracking) - Idle 설정
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
 desc : Module Process Tracking 실행 시작
 parm : mod_name- [in]  Name of the EPT module, use an epty string for the Equipemnt ETP state
						ex> ProcessChamber, RobotArm, PreAligner, Buffer, etc
		pj_id	- [in]  Process Job Name ID (String; Ascii)
		rp_id	- [in]  Recipe ID
		subst_id- [in]  Substrate ID (SourceCarrierID_SlotNumber의 조합)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E157MPT_MPTExecutionStarted(PTCHAR mod_name, PTCHAR pj_id, PTCHAR rp_id, PTCHAR subst_id);
/*
 desc : Module Process Tracking 실행 완료 (레시피 실행 완료)
 parm : mod_name- [in]  Name of the EPT module, use an epty string for the Equipemnt ETP state
						ex> ProcessChamber, RobotArm, PreAligner, Buffer, etc
		exec_ok	- [in]  TRUE : Execution success, FALSE : Execution failed
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E157MPT_MPTExecutionCompleted(PTCHAR mod_name, BOOL exec_ok);
/*
 desc : Module Process Tracking 단계 시작
 parm : mod_name- [in]  Name of the EPT module, use an epty string for the Equipemnt ETP state
						ex> ProcessChamber, RobotArm, PreAligner, Buffer, etc
		step_id	- [in]  Step ID (Name)
						ex> LithographStep1 ~ LithographStep??
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E157MPT_MPTStepStarted(PTCHAR mod_name, PTCHAR step_id);
/*
 desc : Module Process Tracking 단계 완료
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
 desc : Control Job ID에 등록되어 있는 Carrier ID List 반환
 parm : cj_id	- [in]  Control Job ID
		lst_carr- [out] Carrier List가 반환될 버퍼
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobCarrierInputSpec(PTCHAR cj_id, CStringList &lst_carr);
/*
 설명 :
 parm : cj_id	- [in]  Control Job ID
		lst_carr- [out] Carrier List 반환될 버퍼
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobCarrierOutputSpec(PTCHAR cj_id, CStringList &lst_carr);
/*
 desc : Control Job ID에 포함된 Process Job ID List 반환
 parm : cj_id	- [in]  Control Job ID
		lst_pj	- [out] Process Job List 반환될 버퍼
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobProcessJobList(PTCHAR cj_id, CStringList &lst_pj);
/*
 desc : Control Job의 자동 시작 여부 확인
 parm : cj_id		- [in]  Control Job ID
		auto_start	- [out] Auto Start 여부 값 반환
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobStartMethod(PTCHAR cj_id, BOOL &auto_start);
/*
 desc : Control Job의 작업 완료 시간 얻기
 parm : cj_id		- [in]  Control Job ID
		date_time	- [out] 작업 완료 시간 문자열로 반환
		size		- [in]  'data_time' 버퍼 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobCompletedTime(PTCHAR cj_id, PTCHAR date_time, UINT32 size);
/*
 desc : Control Job의 Material 도착 여부 얻기
 parm : cj_id	- [in]  Control Job ID
		arrived	- [out] TRUE : 도착, FALSE : 미도착
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_GetControlJobMaterialArrived(PTCHAR cj_id, BOOL &arrived);
/*
 desc : Control Job의 시작 설정
 parm : cj_id	- [in]  Control Job ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_SetStartControlJob(PTCHAR cj_id);
/*
 desc : Control Job의 중지 (작업이 완료되었거나 실패 했거나 등 일 경우, 중지 시킴) 알림
 parm : cj_id	- [in]  Control Job ID
		action	- [in]  작업 저장 혹은 제거 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_SetStopControlJob(PTCHAR cj_id, E94_CCJA action);
/*
 desc : Control Job의 중단 (작업 시작 중에 작업을 아예 중지 시켜버림) 알림
 parm : cj_id	- [in]  Control Job ID
		action	- [in]  작업 저장 혹은 제거 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_SetAbortControlJob(PTCHAR cj_id, E94_CCJA action);
/*
 desc : Control Job의 잠시 중지 (일시 중지. 취소 / 중단 등 아님) 알림
 parm : cj_id	- [in]  Control Job ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_SetPauseControlJob(PTCHAR cj_id);
/*
 desc : Control Job의 재시작 (중지 시점부터) 알림
 parm : cj_id	- [in]  Control Job ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_SetResumeControlJob(PTCHAR cj_id);
/*
 desc : Control Job의 취소 (시작하지 않은 상태에서... 아예 작업을 하지 않음) 알림
 parm : cj_id	- [in]  Control Job ID
		action	- [in]  작업 저장 혹은 제거 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_E94CJM_SetCancelControlJob(PTCHAR cj_id, E94_CCJA action);
/*
 desc : Control Job을 Queue의 맨 상위로 이동 시킴을 알림
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
 desc : Carrier 내의 Slot Number에 대한 Destination Substrate ID 얻기
 parm : carr_id	- [in]  문자열 ID (전역으로 관리되는 ID)
		slot_no	- [in]  Slot Number (Not sequential number)
		subst_id- [out] Substrate ID (SourceCarrierID_SlotNumber의 조합)
		size	- [in]  'st_id' 버퍼 크기
		logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Mixed_GetSubstrateDestinationID(PTCHAR carr_id, UINT8 slot_no,
														 PTCHAR subst_id, UINT32 size);
/*
 desc : Carrier 내에 저장된 Slot Number (No)에 대한 Substrate ID 값 반환
 parm : carr_id	- [in]  Carrier ID (or Name; String)
		slot_no	- [in]  Slot Number (1-based or Later)
		subst_id- [out] Substrate ID (SourceCarrierID_SlotNumber의 조합)
		size	- [in]  'st_id' 버퍼 크기
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Mixed_GetSubstrateID(PTCHAR carr_id, UINT8 slot_no, PTCHAR subst_id,
											  UINT32 size);
/*
 desc : Control Job ID에 등록되어 있던 특정 Process Job ID의 작업이 모두 끝났는지 여부
 parm : pj_id	- [in]  Process Job ID
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE (Completed) or FALSE (Not complete)
*/
API_IMPORT BOOL uvCIMLib_Mixed_IsSubstrateProcessCompleted(PTCHAR pj_id);
/*
 desc : Carrier 내에 등록되어 있었던 Process Job ID 관련 모든 SubstrateID의 Transport 위치가
		Destination (FOUP; Carrier)에 있는지 여부
 parm : pj_id	- [in]  Process Job ID
		logs	- [in]  로그 데이터 출력 여부
 retn : TRUE (Inside the destination) or FALSE (Outside the destination)
*/
API_IMPORT BOOL uvCIMLib_Mixed_IsSubstrateTransportDestination(PTCHAR pj_id);
/*
 desc : Process Job ID가 작업이 끝날 때, 상위 Process Job ID까지 자동으로 Completed 시킴
 parm : pj_id	- [in]  Process Job ID
 retn : TRUE or FALSE
 note : Process Job ID를 1개만 가지고 있는 Control Job ID일 때만 이 함수 호출이 유효 합니다.
*/
API_IMPORT BOOL uvCIMLib_Mixed_SetAutoJobCompleted(PTCHAR pj_id);
/*
 desc : E40PJM::PRCommandCbk 호출될 때, 사용하기 위해 SubstateIDs의 상태 정보를 갱신해 두기
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Mixed_UpdateSubstrateIDsState();
/*
 desc : CJ Paused되었을 때 PJ들을 Pause시킨다.
 parm : cj_id	- [in] Control Job ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Mixed_PauseProcessJobs(PTCHAR cj_id);
/*
 desc : CJ가 Executing되었을 때 PJ의 처리를 재개한다.
 parm : cj_id	- [in] Control Job ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Mixed_ResumePausedProcessJobs(PTCHAR cj_id);
/*
 desc : CJ가 queued되었을 때 Material Out Spec을 갱신한다.
 parm : cj_id	- [in] Control Job ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Mixed_UpdateMtrlOutSpecSubstrateDestination(PTCHAR cj_id);
/*
 desc : Carrier SlotMap Verified일 때, Material Out Spec을 갱신한다.
 parm : carr_id	- [in] Carrier ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Mixed_UpdateMtrlOutSpecSubstrateDestinationByE87(PTCHAR carr_id);
/*
 desc : 모든 CJ의 Material Out Spec을 갱신한다.
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Mixed_UpdateMtrlOutSpecSubstrateDestinationAll();
/*
 desc : Process Job에 해당하는 substrate state를 모두 확인하고
		__en_e90_substrate_processing_state__ 가 en_ssps_needs_processing 인 경우
		모두 en_ssps_skipped로 변경함
 parm : pj_id	- [in]  Process Job ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCIMLib_Mixed_SkipSubstratesNeedingProcessing(PTCHAR pj_id);
/*
 desc : Check to see if all of the substrates in the ProcessJob are completed and are no longer expected
		to be moved. If so, then let CIM40/CIM94 know that the ProcessJob is complete so it can be deleted
		when the material is removed.
		참고 1) substrates가 모두 destination에 도착한것만 체크하면 안됨, 왜냐하면 source에 위치해도 aborted/stopped로
				처리된 경우에는 여전히 destination이 아닌 source에 있지만 complete일 수 있기 때문임.
			 2) substrates가 destination에 도착하기 전에 PROCESSED state 상에서 complete처리 될 수 있음.
				따라서 단순히 substrates의 각 상태가 PROCESSING COMPLETED 인것 만으로 전체 complete을 선언하면 안됨.
 parm : pj_id	- [in]  Process Job ID
 retn : TRUE (Completed 수행 함) or FALSE (Completed 수행하지 않음)
*/
API_IMPORT BOOL uvCIMLib_Mixed_CheckJobComplete(PTCHAR pj_id);
/*
 desc : This method is called to start the next ProcessJob/ControlJob before the currently
		running ProcessJobs/ControlJobs run out of material to process. 
		이 함수는 현재 돌아가는 ProcessJobs/ControlJobs가 material을 다 진행하고 끝내기 전에 미리 파악하여 
		다음 ProcessJobs/ControlJobs를 시작할 수 있도록 준비하는 것임. 즉 cascading 방식으로 구현해둔 것임. 
		이 방식은 cascade, sequential, concurrent등이 있고  Philoptics의 장비 특성에 맞는 선택에 따라 변경 될 수 있음. 
		현재의 ProcessJobs/ControlJobs들이 완전히 다 끝나고 다음 Job이 그 후에 시작하는 방식을 sequential 이라고 부름.
		-참고 조건: 1) 모든 CJ를 가져와서 SELECTED가 없어야 함, 있다면 이 함수는 그냥 리턴.
					2) 현재 실행중인 PJ의 마지막 SlotID를 가져와서 SubstrateID로 변환하고 그 SubstrateID의 LocationID를 가져옴. 
						Subst.ID: CARRIER1.03 이런 형식
					3) IF: 현재 CJ의 모든 PJ를 확인하여 Queued인 PJ가 있다면 ControlJob.StartNextProcessJob() 실행
					4) ELSE: 모든 CJ를 가져와서 Queued인 CJ가 있다면 CIM94.StartNextControlJob() 실행

		[중요] 처음 Job이 시작될때도 이 함수로 체크해야함. 
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
