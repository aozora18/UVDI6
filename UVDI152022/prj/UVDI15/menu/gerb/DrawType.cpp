
/*
 desc : Align Mark Array Image 출력
 */

#include "pch.h"
#include "../../MainApp.h"
#include "DrawType.h"


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
CDrawType::CDrawType(HWND h_draw)
{
	m_enType	= ENG_ATGL::en_global_0_local_0x0_n_point;
	m_hDraw		= h_draw;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDrawType::~CDrawType()
{
}

/*
 desc : 현재 각 장비의 통신 상태 출력
 parm : index	- [in]  장비 구분 ID (0x00 ~ 0x06)
		comm	- [in]  장비 통신 / 동작 상태 (0x00:fail, 0x01:warn, 0x02:succ)
 retn : None
*/
VOID CDrawType::Draw()
{
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	TCHAR tzName[64]= { NULL };
	HDC hDC			= NULL;
	RECT rDraw		= {NULL};
	CPicture picImg;

	/* 현재 선택된 얼라인 마크의 구성 정보 */
	switch (m_enType)
	{
	case ENG_ATGL::en_global_0_local_0x0_n_point : wcscpy_s(tzName, 64, L"global_0_local_0x0_n");	break;
	case ENG_ATGL::en_global_4_local_0x0_n_point : wcscpy_s(tzName, 64, L"global_4_local_0x0_n");	break;
	case ENG_ATGL::en_global_4_local_2x2_n_point : wcscpy_s(tzName, 64, L"global_4_local_2x2_n");	break;
	default	:	return;
	}
	/* 통신 상태 아이콘 파일 설정 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\ui_img\\type\\%s.jpg", g_tzWorkDir, tzName);
	/* 이미지 적재 */
	if (!picImg.Load(tzFile))	return;

	/* 그리고자 하는 영역 얻기 */
	::GetClientRect(m_hDraw, &rDraw);

	rDraw.left	+= 16;
	rDraw.top	+= 25;

	// by sys&j : Group Size에 맞춰서 Image Draw로 변경
	//rDraw.right	= rDraw.left+ picImg.GetWidth();
	rDraw.right	 = rDraw.right - 16;
	rDraw.bottom = rDraw.top + picImg.GetHeight();

	/* 그리고자 하는 윈도 핸들 영역의 DC 얻고, 이미지 출력 */
	hDC	= ::GetDC(m_hDraw);
	if (hDC)
	{
		picImg.Show(hDC, rDraw);	/* 이미지 출력 */
		::ReleaseDC(m_hDraw, hDC);	/* DC 핸들 해제 */
	}
}

/*
 desc : 현재 각 장비의 통신 상태 출력
 parm : None
 retn : None
*/
VOID CDrawType::DrawType(ENG_ATGL type)
{
	/* 화면 갱신을 위한 Mark Align Array Type 임시 저장 */
	m_enType	= type;
	Draw();
}

/*
 desc : 다음에 표현될 Align Mark Array Type 값 반환
 parm : None
 retn : Array Type 값 (ENG_ATGL)
*/
ENG_ATGL CDrawType::GetNextType()
{
	/* 현재 선택된 얼라인 마크의 구성 정보 */
	switch (m_enType)
	{
	case ENG_ATGL::en_global_0_local_0x0_n_point: m_enType = ENG_ATGL::en_global_4_local_0x0_n_point;	break;
	case ENG_ATGL::en_global_4_local_0x0_n_point: m_enType = ENG_ATGL::en_global_4_local_2x2_n_point;	break;
	case ENG_ATGL::en_global_4_local_2x2_n_point: m_enType = ENG_ATGL::en_global_0_local_0x0_n_point;	break;
	}

	return m_enType;
}