
#pragma once

#include "../WorkStep.h"
#include <map>
#include <array>
#include "../../../../inc/itfe/EItfcLuria.h"

class CWorkMarkTest : public CWorkStep
{
	/* ������ & �ı��� */
public:

	
	CWorkMarkTest(LPG_CELA expo);
	virtual ~CWorkMarkTest();

	/* �����Լ� ������ */
protected:
public:

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();

	ENG_JWNS GeneratePath(ENG_AMOS mode,bool globalMark);

	void SetAlignMode(ENG_AMOS mode, ENG_ATGL type)
	{ 
		alignMode = mode; 
		m_u8StepTotal = mode == ENG_AMOS::en_onthefly_2cam ? 0x21 : 0;
		align_type = type;
	}
	
	static const int endPointPair = 2;

	std::map<ENG_AMOS, array<std::function<void()>, endPointPair>> alignCallback =
	{
		{ENG_AMOS::en_onthefly_2cam,
									{
										[&]() {DoAlignOnthefly2cam(); },
										[&]() {SetWorkNextOnthefly2cam(); }
									}},
		{ENG_AMOS::en_onthefly_3cam,
									{
										[&]() {DoAlignOnthefly3cam(); },
										[&]() {SetWorkNextOnthefly3cam(); }
									}},
		{ENG_AMOS::en_static_2cam,
									{
										[&]() {DoAlignStatic2cam(); },
										[&]() {SetWorkNextStatic2cam(); }
									}},
		{ENG_AMOS::en_static_3cam,
									{
										[&]() {DoAlignStatic3cam(); },
										[&]() {SetWorkNextStatic3cam(); }
									}},
	};

protected:
	void DoAlignOnthefly2cam();
	VOID SetWorkNextOnthefly2cam();

	void DoAlignOnthefly3cam();
	VOID SetWorkNextOnthefly3cam();

	void DoAlignStatic2cam();
	VOID SetWorkNextStatic2cam();

	void DoAlignStatic3cam();
	VOID SetWorkNextStatic3cam();

	enum class AlignMotionMode : UINT8
	{
		toInitialMoving = 0, //���� ��ĵ�� ���� �˻� �ʱ���ġ�� �̵��ϴ� ����
		toScanMoving = 1,    //��ĵ�� ���� �̵��ϴ� ����
	};

	/* ���� ���� */
protected:

	int globalMarkCnt, localMarkCnt = 0;
	UINT8				m_u8MarkCount;	/* �� �˻��� ��ũ ���� */
	UINT32				m_u32ExpoCount;		/* ���� �뱤 �ݺ� Ƚ�� */
	STG_CELA			m_stExpoLog;
	int scanCount = 0;
	ENG_AMOS alignMode = ENG_AMOS::en_onthefly_2cam;
	ENG_ATGL align_type = ENG_ATGL::en_global_4_local_0_point;
	/* ���� �Լ� */
protected:

	

	ENG_JWNS			SetHomingACamSide();
	ENG_JWNS			IsHomedACamSide();

	VOID				SaveExpoResult(UINT8 state);

	/* ���� �Լ� */
public:




};
