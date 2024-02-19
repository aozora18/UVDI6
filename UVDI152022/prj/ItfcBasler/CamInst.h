#pragma once

class CCamInst : public Camera_t
{
// 생성자 및 파괴자
public:

	CCamInst(UINT8 cam_id);
	virtual ~CCamInst();

// 가상함수 재정의
protected:

public:


// 로컬 변수
protected:

	UINT8				m_u8CamIndex;	/* Align Camera Index (1 or Later) */

// 로컬 함수
protected:


// 공용 함수
public:

	UINT8				GetCamIndex()	{	return m_u8CamIndex;	}
};
