#pragma once

class CTempAlarm
{
/* ������ / �ı��� */
public:
	CTempAlarm(HWND *h_alarm);
	virtual ~CTempAlarm();


/* ���� ���� */
protected:

	UINT8				m_u8Alarm[3];	/* DI Internal, LED, Board */
	HWND				m_hAlarm[3];	/* DI Internal, LED, Board */


/* ���� �Լ� */
protected:


/* ���� �Լ� */
public:

	VOID				DrawTempAlarm();
};
