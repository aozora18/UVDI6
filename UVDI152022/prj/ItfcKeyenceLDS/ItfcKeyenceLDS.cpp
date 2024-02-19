
/*
 desc : PM100x Library
*/

#include "pch.h"
#include "MainApp.h"
#include "KeyenceLDS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/* --------------------------------------------------------------------------------------------- */
/*                                           전역 변수                                           */
/* --------------------------------------------------------------------------------------------- */

CKeyenceLDS* g_pKeyenceLDS = NULL;

/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- */
/*                                           외부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

	/*
	 desc : Library Init
	 parm : config	- [in]  환경 설정 정보
	 retn : None
	 note : 공유메모리가 과연 필요할까?
	*/
	API_EXPORT VOID uvKeyenceLDS_Init(LPG_CIEA config)
	{
		char chLocalIP[32] = { 0, };
		char chRemoteIP[32] = { 0, };

		IN_ADDR stLocalAddr = { NULL };
		IN_ADDR stRemoteAddr = { NULL };

		UINT32 u32LocalAddr = uvCmn_GetIPv4BytesToUint32(config->set_comm.cmps_ipv4);
		UINT32 u32RemoteAddr = uvCmn_GetIPv4BytesToUint32(config->set_comm.keyence_lds_ipv4);

		g_pKeyenceLDS = new CKeyenceLDS();

		memcpy(&stLocalAddr.s_addr, &u32LocalAddr, sizeof(UINT32));
		memcpy(&stRemoteAddr.s_addr, &u32RemoteAddr, sizeof(UINT32));

		inet_ntop(AF_INET, (CONST VOID*) & stLocalAddr, chLocalIP, IPv4_LENGTH);
		inet_ntop(AF_INET, (CONST VOID*) & stRemoteAddr, chRemoteIP, IPv4_LENGTH);

		g_pKeyenceLDS->SetConfigLocalIPAddr(chLocalIP);
		g_pKeyenceLDS->SetConfigRemoteIPAddr(chRemoteIP);
		g_pKeyenceLDS->SetConfigPortNumber(config->set_comm.keyence_lds_port);

		g_pKeyenceLDS->Connect();
		return;
	}

	/*
	 desc : Library Close
	 parm : None
	 retn : None
	*/
	API_EXPORT VOID uvKeyenceLDS_Close()
	{
		if (g_pKeyenceLDS)
		{
			delete g_pKeyenceLDS;
			g_pKeyenceLDS = NULL;
		}
	}

	/*
	 desc : 연결 여부 확인
	 parm : None
	 retn : TRUE or FALSE
	*/
	API_EXPORT BOOL uvKeyenceLDS_IsConnected()
	{
		if (!g_pKeyenceLDS)	return FALSE;

		return g_pKeyenceLDS->ChkConnect();
	}

	/*
	 desc : LDS의 Zero 값을 현재 위치로 변경
	 parm : u8ModuleIdx	- [in]  LDS 모듈 번호
	 retn : TRUE or FALSE
	*/
	API_EXPORT BOOL uvKeyenceLDS_ZeroShift(UINT8 u8ModuleIdx)
	{
		if (!g_pKeyenceLDS)	return FALSE;

		return g_pKeyenceLDS->ZeroShift((int)u8ModuleIdx);
	}

	/*
	 desc : LDS의 Zero 값을 초기화
	 parm : u8ModuleIdx	- [in]  LDS 모듈 번호
	 retn : TRUE or FALSE
	*/
	API_EXPORT BOOL uvKeyenceLDS_ResetZeroShift(UINT8 u8ModuleIdx)
	{
		if (!g_pKeyenceLDS)	return FALSE;

		return g_pKeyenceLDS->ResetZeroShift((int)u8ModuleIdx);
	}

	/*
	 desc : LDS의 설정 값을 초기화
	 parm : u8ModuleIdx	- [in]  LDS 모듈 번호
	 retn : TRUE or FALSE
	*/
	API_EXPORT BOOL uvKeyenceLDS_ResetDevice(UINT8 u8ModuleIdx)
	{
		if (!g_pKeyenceLDS)	return FALSE;

		return g_pKeyenceLDS->ResetDevice((int)u8ModuleIdx);
	}

	/*
	 desc : LDS의 출력 값을 입력 값만큼 수정 (S/W Zero Set)(덧셈 연산)
	 parm : dValue	- [in]  입력 값
	 retn : None
	*/
	API_EXPORT VOID uvKeyenceLDS_ZeroSet(double dValue)
	{
		if (!g_pKeyenceLDS)	return;

		g_pKeyenceLDS->ZeroSet(dValue);
	}

	/*
	 desc : LDS의 출력 값의 소숫점 자리 설정
	 parm : dValue	- [in]  입력 값 (1, 0.1, 0.01, 0.001 ... )
	 retn : None
	*/
	API_EXPORT VOID uvKeyenceLDS_SetDecimalPos(double dValue)
	{
		if (!g_pKeyenceLDS)	return;

		g_pKeyenceLDS->SetDecimalPos(dValue);
	}

	/*
	 desc : LDS에 설정된 S/W Zero Set 값을 출력
	 parm : None
	 retn : 1, 0.1, 0.01, 0.001 ... 
	*/
	API_EXPORT DOUBLE uvKeyenceLDS_GetBaseValue()
	{
		if (!g_pKeyenceLDS)	return 0;

		return g_pKeyenceLDS->GetBaseValue();
	}

	/*
	 desc : LDS 측정
	 parm : u8ModuleIdx	- [in]  LDS 모듈 번호
	 retn : LDS Value
	*/
	API_EXPORT DOUBLE uvKeyenceLDS_Measure(UINT8 u8ModuleIdx)
	{
		if (!g_pKeyenceLDS)	return -9999.999;

		double dValue = 9999.999;

		g_pKeyenceLDS->LDSMeasure(&dValue, (int)u8ModuleIdx);
		return dValue;
	}

#ifdef __cplusplus
}
#endif