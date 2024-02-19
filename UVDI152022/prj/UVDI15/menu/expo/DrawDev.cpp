
/*
 desc : 각 장비의 통신 및 전원 상태 출력
 */

#include "pch.h"
#include "../../MainApp.h"
#include "DrawDev.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 생성자
 parm : h_draw	- [in]  이미지가 출력되는 윈도 핸들
 retn : None
*/
CDrawDev::CDrawDev(HWND *h_draw)
{
	/* 윈도 핸들 복사 */
	memcpy(m_hDraw, h_draw, sizeof(HWND) * 9);
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDrawDev::~CDrawDev()
{
}

/*
 desc : 현재 각 장비의 통신 상태 출력
 parm : index	- [in]  장비 구분 ID (0x00 ~ 0x06)
		comm	- [in]  장비 통신 / 동작 상태 (0x00:fail, 0x01:warn, 0x02:succ)
 retn : None
*/
VOID CDrawDev::DrawDev(UINT8 index, UINT8 comm)
{
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	TCHAR tzName[3][32]	= { L"dev_fail", L"dev_warn", L"dev_succ" };
	HDC hDC		= NULL;
	RECT rDraw	= {NULL};
	CPicture picImg;

	/* 그리고자 하는 영역 얻기 */
	::GetClientRect(m_hDraw[index], &rDraw);

	/* 통신 상태 아이콘 파일 설정 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\ui_img\\status\\%s.jpg", g_tzWorkDir, tzName[comm]);
	/* 이미지 적재 */
	if (!picImg.Load(tzFile))	return;
	/* 그리고자 하는 윈도 핸들 영역의 DC 얻고, 이미지 출력 */
	hDC	= ::GetDC(m_hDraw[index]);
	if (hDC)
	{
		picImg.Show(hDC, rDraw);		/* 이미지 출력 */
		::ReleaseDC(m_hDraw[index], hDC);	/* DC 핸들 해제 */
	}
}

/*
 desc : 현재 각 장비의 통신 상태 출력
 parm : None
 retn : None
*/
VOID CDrawDev::DrawDev()
{
	UINT8 u8Comm= 0x00, i = 0x00;

	for (; i<9; i++)
	{
		switch (i)
		{
		/* 01. Luria Service 연결 여부 */
		case 0x00	:	u8Comm	= uvCmn_Luria_IsConnected() ? 0x01 : 0x00;
						u8Comm	+= uvEng_Luria_IsServiceInited() ? 0x01 : 0x00;		break;

		/* 02. MC2 연결 여부 */
		case 0x01	:	u8Comm	= uvCmn_MC2_IsConnected() ? 0x02 : 0x00;			break;
		/* 04. Aling Camera 1 연결 여부 */
		case 0x03	:	u8Comm	= uvCmn_Camera_IsConnectedACam(0x01) ? 0x02 : 0x00;	break;
		/* 05. Aling Camera 2 연결 여부 */
		case 0x04	:	u8Comm	= uvCmn_Camera_IsConnectedACam(0x02) ? 0x02 : 0x00;	break;
		/* 09. Vacuum 연결 여부 */
		case 0x08	:	u8Comm	= uvCmn_Mvenc_IsConnected() ? 0x02 : 0x00;	break;
		}

		/* 현재 검사 위치가 Hot Air, Vacuum 혹은 Chiller 인 경우, PLC가 연결 안되어 있다면... */
		if ((0x05 == i || 0x06 == i || 0x07 == i) && !m_u8DevStat[2])	u8Comm	= 0x01;	/* Warning */
		/* 기존 통신 상태와 현재 통신 상태가 다르면, 통신 상태 아이콘 갱신 */
		if (m_u8DevStat[i] != u8Comm)
		{
			/* 최신 통신 상태로 변경 */
			m_u8DevStat[i]	= u8Comm;
			/* 통신 상태 이미지 갱신 */
			DrawDev(i, u8Comm);
		}
	}
}

/*
 desc : 전체 장비의 통신 상태 무조건 갱신
 parm : None
 retn : None
*/
VOID CDrawDev::DrawDevAll()
{
	/* 가장 최근의 장비 상태 값 초기화 */
	memset(m_u8DevStat, 0xff, sizeof(UINT8) * 9);
	/* 통신 상태 갱신 */
	DrawDev();
}