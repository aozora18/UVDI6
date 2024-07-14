
#pragma once

#include "../WorkStep.h"
#include <map>
#include "../../GlobalVariables.h"

class CWorkMarkMove : public CWorkStep
{
/* ������ & �ı��� */
public:

	CWorkMarkMove(UINT16 mark_no);
	virtual ~CWorkMarkMove();

/* �����Լ� ������ */
protected:
public:

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();

/* ���� ���� */
protected:
	UINT8				m_u8MarkNo;		/* �̵��Ϸ��� Global Mark Number ��ġ (Zero-based) */
	UINT8				m_u8ACamID;		/* 1 or 2 */

/* ���� �Լ� */
protected:

	/* 1��° Align Mark�� ��ġ�� �̵� */
	ENG_JWNS			SetMovingAlignMark();
	ENG_JWNS			IsMovedAlignMark();

	ENG_AMOS alignMotion = ENG_AMOS::en_onthefly_2cam;
	ENG_ATGL aligntype = ENG_ATGL::en_global_4_local_0_point;

	void SetAlignMode(ENG_AMOS motion, ENG_ATGL aligntype)
	{
		alignMotion = motion;
		this->aligntype = aligntype;
		const int INIT_STEP = 0;
		markMoveCallback[motion][INIT_STEP]();
		
	}

	void DoInitStatic2cam();
	void DoInitStatic3cam();
	void DoInitOnthefly3cam();
	void DoInitOnthefly2cam();
	
	void DoMovingOnthefly2cam();
	void DoMovingOnthefly3cam();
	void DoMovingStatic2cam();
	void DoMovingStatic3cam();

	void SetWorkNextOnthefly2cam();
	void SetWorkNextOnthefly3cam();
	void SetWorkNextStatic3cam();
	void SetWorkNextStatic2cam();


	static const int endPointPair = 3;
	int includeAlignOffset = 0;
	std::map<ENG_AMOS, array<std::function<void()>, endPointPair>> markMoveCallback =
	{
		{ENG_AMOS::en_onthefly_2cam,
									{
										[&]() {DoInitOnthefly2cam(); },
										[&]() {DoMovingOnthefly2cam(); },
										[&]() {SetWorkNextOnthefly2cam(); }
									}},
		{ENG_AMOS::en_onthefly_3cam,
									{
										[&]() {DoInitOnthefly3cam(); },
										[&]() {DoMovingOnthefly3cam(); },
										[&]() {SetWorkNextOnthefly3cam(); }
									}},
		{ENG_AMOS::en_static_2cam,
									{
										[&]() {DoInitStatic2cam(); },
										[&]() {DoMovingStatic2cam(); },
										[&]() {SetWorkNextStatic2cam(); }
									}},
		{ENG_AMOS::en_static_3cam,
									{
										[&]() {DoInitStatic3cam(); },
										[&]() {DoMovingStatic3cam(); },
										[&]() {SetWorkNextStatic3cam(); }
									}},
	};



/* ���� �Լ� */
public:

};
