
/*
 desc : Global 4 개 Mark 간의 6 구간의 오차 값이 유효한지 여부
 */

#include "pch.h"
#include "../../MainApp.h"
#include "MarkDist.h"

#include "../../../../inc/comn/MyStatic.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 생성자
 parm : h_dist	- [in]  Global 4 개 Mark 간의 6 구간의 오차 정보를 출력할 핸들
 retn : None
*/
CMarkDist::CMarkDist(HWND *h_dist)
{
	/* 윈도 핸들 복사 */
	memcpy(m_hDist, h_dist, sizeof(HWND) * 6);
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CMarkDist::~CMarkDist()
{
}

/*
 desc : 현재 혹은 가장 최근에 검색된 마크 결과 정보 출력 (갱신)
 parm : None
 retn : None
*/
VOID CMarkDist::DrawDist()
{
	UINT8 u8Mode	= uvEng_Camera_GetGrabbedCount() > 0 ? 0x01 : 0x00;
	MarkDist(u8Mode);
}

/*
 desc : 검색된 마크 이미지 출력
 parm : mode	- [in]  0x00 : 검색된 마크 없음, 0x01 : 검색된 마크 있음
 retn : None
*/
VOID CMarkDist::MarkDist(UINT8 mode)
{
	UINT8 u8Index, i	= 0x00;
	DOUBLE dbMarkDiff	= 0.0f;
	COLORREF clrText[3]	= { RGB(128, 128, 128), RGB(0, 0, 255), RGB(255, 0, 0) };
	CMyStatic *pTxt		= NULL;
#if 1	/* 레시피에 등록된 마크 6곳의 오차 값과 비교 후 판단하도록 수정 */
	for (; i<6; i++)
	{
		u8Index	= 0x00;
		pTxt	= (CMyStatic *)CWnd::FromHandle(m_hDist[i]);
		ASSERT(pTxt);

		/* Gerber File (XML) 내에 Global Mark 개수 및 거리 (6 곳) 구하기 */
		if (mode && uvEng_Luria_GetMarkCount(ENG_AMTF::en_global) == 0x04)	/* 검색된 마크가 있는 경우 (Only Global) */
		{
			/* 얼라인 카메라에 의해 측정된 마크 중심들 간의 거리 오차 값과 거버에 존재하는 */
			/* 마크들 간의 거리 값 비교 후 환경 파일에 저장된 비율 값 보다 크면 에러로 판단 */
			u8Index	= uvEng_GetConfig()->mark_diff.result[i].len_valid;	/* 0x00 : Not set or Reset, 0x01 : Set (Succ), 0x02 : Set (Fail) */
		}
		/* 컨트롤 내, 문자 (텍스트) 색상 설정 */
		pTxt->SetTextColor(clrText[u8Index]);
		pTxt->Invalidate(FALSE);
	}
#endif
}