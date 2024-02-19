#pragma once

class CMarkDist
{
/* 생성자 / 파괴자 */
public:
	CMarkDist(HWND *h_dist);
	virtual ~CMarkDist();


/* 로컬 변수 */
protected:

	/* Global Mark 4개의 6 구간 유효 정보를 표현하기 위한 핸들 */
	HWND				m_hDist[6];


/* 로컬 함수 */
protected:

	VOID				MarkDist(UINT8 mode);


/* 공용 함수 */
public:

	VOID				DrawDist();
};
