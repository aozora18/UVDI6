
/*
 desc : PM100x Library
*/

#include "pch.h"
#include "MainApp.h"

#include "CaliThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/* --------------------------------------------------------------------------------------------- */
/*                                           전역 변수                                           */
/* --------------------------------------------------------------------------------------------- */

CCaliThread					*g_pCaliThread	= NULL;


/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Check if it is connected...
 parm : None
 retn : TRUE or FALSE
*/
BOOL IsConnected()
{
	if (!g_pCaliThread)				return FALSE;
	if (!g_pCaliThread->IsBusy())	return FALSE;

	return g_pCaliThread->IsConnected();
}



/* --------------------------------------------------------------------------------------------- */
/*                                           외부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : Library Init
 parm : byPort	- [in]	COM PORT 번호
		shmem	- [in]  Shared Memory
		type	- [in]  Measurement Type (0x01: Power, 0x02: Energy, 0x03: Frequency, 0x04: Density)
						=> Power만 지원
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGentec_Init(LPG_CIEA config, LPG_PADV shmem)
{
	g_pCaliThread = new CCaliThread(uvCmn_GetIPv4BytesToUint32(config->set_comm.cmps_ipv4),
									uvCmn_GetIPv4BytesToUint32(config->set_comm.gentec_ipv4),
									config->set_comm.gentec_port,
									shmem,
									ENG_MDST(config->luria_svc.illum_meas_type));
	ASSERT(g_pCaliThread);
	if (!g_pCaliThread->CreateThread(0, 0, 50/*fixed*//*MEDIUM_THREAD_SPEED*/))
	{
		delete g_pCaliThread;
		g_pCaliThread	= NULL;
		return FALSE;
	}

	return TRUE;
}

/*
 desc : Library Close
 parm : None
 retn : None
*/
API_EXPORT VOID uvGentec_Close()
{
	/* Gentec 수집 및 감시 스레드 해제 */
	if (g_pCaliThread)
	{
		g_pCaliThread->Stop();
		while (g_pCaliThread->IsBusy())	g_pCaliThread->Sleep(100);
		delete g_pCaliThread;
		g_pCaliThread	= NULL;
	}
}

/*
 desc : 연결 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGentec_IsConnected()
{
	return IsConnected();
}

/*
 desc : Gentec Device Setup Information 반환
 parm : None
 retn : 구조체 포인터 반환
*/
API_EXPORT LPG_PSDV uvGentec_GetSetupInfo()
{
	if (!IsConnected())	return NULL;
	return g_pCaliThread->GetSetupValue();
}

/*
 desc : Gentec에서 수집된 데이터가 꽉찬 상황인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGentec_IsQueFulled()
{
	if (!IsConnected())	return FALSE;
	return g_pCaliThread->IsQueFulled();
}

/*
 desc : 현재 수집된 버퍼 비율이 저장할 수 있는 큐의 버퍼 공간 대비 얼마만큼 저장되어 있는지 확인
 parm : rate	- [in]  Queue에 저장되어 있는 비율 값 (단위: 0.1 ~ 0.99)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGentec_IsQueFullRate(FLOAT rate)
{
	if (!IsConnected())	return FALSE;
	return g_pCaliThread->IsQueFull(rate);
}

/*
 desc : 임의 시간 동안 측정한 평균 값 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvGentec_ResetValue()
{
	if (!IsConnected())	return;
	g_pCaliThread->ResetValue();
}

/*
 desc : Set whether to measure the illuminace from Gentec
 parm : flag	- [in]  FALSE - Stop, TRUE - Start
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGentec_RunMeasure(BOOL flag)
{
	BOOL bEnable	= FALSE;

	if (!IsConnected())	return FALSE;

	/* If it is start, it initializes the exisiting values */
	if (flag)	g_pCaliThread->ResetValue();
	bEnable	= g_pCaliThread->RunMeasure(flag);

	return bEnable;
}

/*
 desc : Returns whether the Gentec is measuring illuminance
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGentec_IsRunMeasure()
{
	if (!IsConnected())	return FALSE;
	return g_pCaliThread->IsRunMeasure();
}

/*
 desc : 조도 데이터 결과 반환 (메모리에 저장된 값)
 parm : None
 retn : 데이터가 저장된 전역 포인터 반환
*/
API_EXPORT LPG_PGDV uvGentec_GetRunPower()
{
	if (!IsConnected())	return NULL;
	return g_pCaliThread->GetDataValue();
}

/*
 desc : Set Wave Length
 parm : wave	- [in]  Wave Length (or Line Filter) (단위: nm)
						ex> 365, 385, 395, 405 (nm)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGentec_SetQueryWaveLength(INT16 wave)
{
	if (!IsConnected())	return FALSE;
	return g_pCaliThread->QueryWaveLength(wave);
}

/*
 desc : Get Wave Length
 parm : None
 retn : 양수 값 (음수 값인 경우 실패) (365, 385, 390, 405 (nm))
*/
API_EXPORT INT16 uvGentec_GetQueryWaveLength()
{
	if (!IsConnected())	return -1;
	return g_pCaliThread->QueryWaveLength();
}


#ifdef __cplusplus
}
#endif