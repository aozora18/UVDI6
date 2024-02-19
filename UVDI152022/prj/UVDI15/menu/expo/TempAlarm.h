#pragma once

class CTempAlarm
{
/* 생성자 / 파괴자 */
public:
	CTempAlarm(HWND *h_alarm);
	virtual ~CTempAlarm();


/* 로컬 변수 */
protected:

	UINT8				m_u8Alarm[3];	/* DI Internal, LED, Board */
	HWND				m_hAlarm[3];	/* DI Internal, LED, Board */


/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	VOID				DrawTempAlarm();
};
