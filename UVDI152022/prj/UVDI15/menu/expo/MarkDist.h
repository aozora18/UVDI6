#pragma once

class CMarkDist
{
/* ������ / �ı��� */
public:
	CMarkDist(HWND *h_dist);
	virtual ~CMarkDist();


/* ���� ���� */
protected:

	/* Global Mark 4���� 6 ���� ��ȿ ������ ǥ���ϱ� ���� �ڵ� */
	HWND				m_hDist[6];


/* ���� �Լ� */
protected:

	VOID				MarkDist(UINT8 mode);


/* ���� �Լ� */
public:

	VOID				DrawDist();
};
