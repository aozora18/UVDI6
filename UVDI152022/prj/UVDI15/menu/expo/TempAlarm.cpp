
/*
 desc : 광학계 LED / Board와 DI 내부의 온도가 임계치 값을 벗어났는지 여부
 */

#include "pch.h"
#include "../../MainApp.h"
#include "TempAlarm.h"

#include "../../../../inc/comn/MyStatic.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 생성자
 parm : h_dist	- [in]  온도 임계치 알람 여부 출력 텍스트 윈도 핸들
 retn : None
*/
CTempAlarm::CTempAlarm(HWND *h_alarm)
{
	/* 윈도 핸들 복사 */
	memcpy(m_hAlarm, h_alarm, sizeof(HWND) * 3);	/* DI / LED / BOARD*/
	memset(m_u8Alarm, 0xff, 3);
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CTempAlarm::~CTempAlarm()
{
}

/*
 desc : 각각 온도 알람이 발생 했는지 여부에 따라, 글자색 변경 후 출력
 parm : None
 retn : None
*/
VOID CTempAlarm::DrawTempAlarm()
{
	UINT8 u8Index, i	= 0x00;
	UINT16 u16MinLed[2]	= {NULL}, u16MaxLed[2] = {NULL}, u16MinBD[2] = {NULL}, u16MaxBD[2] = {NULL};
	DOUBLE dbTempMin	= 0.0f, dbTempMax = 0.0f;
	COLORREF clrText[2]	= { RGB(0, 0, 0), RGB(255, 0, 0) };
	CMyStatic *pTxt		= NULL;
	LPG_DITR pstTemp	= &uvEng_GetConfig()->temp_range;
	LPG_LDDP pstDirectPh= &uvEng_ShMem_GetLuria()->directph;

	/* DI Internal */
	dbTempMin	= pstTemp->GetTempDeg(0x01, 0x01);
	dbTempMax	= pstTemp->GetTempDeg(0x02, 0x01);
	u8Index		= (uvEng_PLC_GetTempDI(0x00) >= dbTempMin && uvEng_PLC_GetTempDI(0x00) <= dbTempMax) ? 0x00 : 0x01;
#if 0	/* 활성화 하면, 화면 갱신 부하는 줄일수 있으나, 갱신안되는 버그가 있음 */
	if (u8Index != m_u8Alarm[0])
#endif
	{
		m_u8Alarm[0]	= u8Index;
		pTxt	= (CMyStatic *)CWnd::FromHandle(m_hAlarm[0]);
		ASSERT(pTxt);
		pTxt->SetRedraw(FALSE);
		pTxt->SetTextColor(clrText[u8Index]);
		pTxt->SetRedraw(TRUE);
		pTxt->Invalidate(TRUE);
	}

	/* PH Led / Board */
	pstDirectPh->GetMinTempLedBD(0x01, u16MinLed[0], u16MinBD[0]);
	pstDirectPh->GetMaxTempLedBD(0x01, u16MaxLed[0], u16MaxBD[0]);
	pstDirectPh->GetMinTempLedBD(0x02, u16MinLed[1], u16MinBD[1]);
	pstDirectPh->GetMaxTempLedBD(0x02, u16MaxLed[1], u16MaxBD[1]);

	/* PH Led */
	u16MinLed[0]= u16MinLed[0] > u16MinLed[1] ? u16MinLed[1] : u16MinLed[0];
	u16MaxLed[0]= u16MaxLed[0] < u16MaxLed[1] ? u16MaxLed[1] : u16MaxLed[0];
	dbTempMin	= pstTemp->GetTempDeg(0x01, 0x02);
	dbTempMax	= pstTemp->GetTempDeg(0x02, 0x02);
	u8Index	= (dbTempMin <= u16MinLed[0]/10.0f && dbTempMax >= u16MaxLed[0]/10.0f) ? 0x00 : 0x01;
#if 0	/* 활성화 하면, 화면 갱신 부하는 줄일수 있으나, 갱신안되는 버그가 있음 */
	if (u8Index != m_u8Alarm[1])
#endif
	{
		m_u8Alarm[1]	= u8Index;
		pTxt	= (CMyStatic *)CWnd::FromHandle(m_hAlarm[1]);
		ASSERT(pTxt);
		pTxt->SetRedraw(FALSE);
		pTxt->SetTextColor(clrText[u8Index]);
		pTxt->SetRedraw(TRUE);
		pTxt->Invalidate(TRUE);
	}

	/* PH Board */
	pTxt	= (CMyStatic *)CWnd::FromHandle(m_hAlarm[2]);
	ASSERT(pTxt);
	pTxt->SetRedraw(FALSE);
	u16MinBD[0]	= u16MinBD[0] > u16MinBD[1] ? u16MinBD[1] : u16MinBD[0];
	u16MaxBD[0]	= u16MaxBD[0] < u16MaxBD[1] ? u16MaxBD[1] : u16MaxBD[0];
	dbTempMin	= pstTemp->GetTempDeg(0x01, 0x03);
	dbTempMax	= pstTemp->GetTempDeg(0x02, 0x03);
	u8Index	= (dbTempMin <= u16MinBD[1]/2.0f && dbTempMax >= u16MaxBD[1]/2.0f) ? 0x00 : 0x01;
#if 0	/* 활성화 하면, 화면 갱신 부하는 줄일수 있으나, 갱신안되는 버그가 있음 */
	if (u8Index != m_u8Alarm[2])
#endif
	{
		m_u8Alarm[2]	= u8Index;
		pTxt	= (CMyStatic *)CWnd::FromHandle(m_hAlarm[2]);
		ASSERT(pTxt);
		pTxt->SetRedraw(FALSE);
		pTxt->SetTextColor(clrText[u8Index]);
		pTxt->SetRedraw(TRUE);
		pTxt->Invalidate(TRUE);
	}
}