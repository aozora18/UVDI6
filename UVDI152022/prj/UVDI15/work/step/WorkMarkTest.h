
#pragma once

#include "../WorkStep.h"
#include <map>
#include <array>
#include "../../../../inc/itfe/EItfcLuria.h"
#include "../../GlobalVariables.h"

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

	//void GeneratePath(ENG_AMOS mode, ENG_ATGL alignType, vector<STG_XMXY>& path);

	void SetAlignMode();
	
	static const int endPointPair = 3;

	std::map<ENG_AMOS, array<std::function<void()>, endPointPair>> alignCallback =
	{
		{ENG_AMOS::en_onthefly_2cam,
									{
										[&]() {DoInitOnthefly2cam(); },
										[&]() {DoAlignOnthefly2cam(); },
										[&]() {SetWorkNextOnthefly2cam(); }
									}},
		{ENG_AMOS::en_onthefly_3cam,
									{
										[&]() {DoInitOnthefly3cam(); },
										[&]() {DoAlignOnthefly3cam(); },
										[&]() {SetWorkNextOnthefly3cam(); }
									}},
		{ENG_AMOS::en_static_2cam,
									{
										[&]() {DoInitStatic2cam(); },
										[&]() {DoAlignStatic2cam(); },
										[&]() {SetWorkNextStatic2cam(); }
									}},
		{ENG_AMOS::en_static_3cam,
									{
										[&]() {DoInitStatic3cam(); },
										[&]() {DoAlignStatic3cam(); },
										[&]() {SetWorkNextStatic3cam(); }
									}},
	};

protected:

	void DoInitOnthefly2cam();
	void DoAlignOnthefly2cam();
	VOID SetWorkNextOnthefly2cam();

	void DoInitOnthefly3cam();
	void DoAlignOnthefly3cam();
	VOID SetWorkNextOnthefly3cam();

	void DoInitStatic2cam();
	void DoAlignStatic2cam();
	VOID SetWorkNextStatic2cam();

	void DoInitStatic3cam();
	void DoAlignStatic3cam();
	VOID SetWorkNextStatic3cam();

	enum class AlignMotionMode : UINT8
	{
		toInitialMoving = 0, //���� ��ĵ�� ���� �˻� �ʱ���ġ�� �̵��ϴ� ����
		toScanMoving = 1,    //��ĵ�� ���� �̵��ϴ� ����
	};

	/* ���� ���� */
protected:

	vector<STG_XMXY>	grabMarkPath;
	int globalMarkCnt, localMarkCnt = 0;
	UINT8				m_u8MarkCount;	/* �� �˻��� ��ũ ���� */
	UINT32				m_u32ExpoCount;		/* ���� �뱤 �ݺ� Ƚ�� */
	STG_CELA			m_stExpoLog;
	int scanCount = 0;
	ENG_AMOS alignMotion = ENG_AMOS::en_onthefly_2cam;
	ENG_ATGL aligntype = ENG_ATGL::en_global_4_local_0_point;
	/* ���� �Լ� */
protected:

	

	ENG_JWNS			SetHomingACamSide();
	ENG_JWNS			IsHomedACamSide();

	VOID				SaveExpoResult(UINT8 state);

	/* ���� �Լ� */
public:




};
