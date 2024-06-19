
#pragma once

#include "../WorkStep.h"
#include <map>
#include <vector>
#include "../../GlobalVariables.h"

using namespace std;
class CWorkExpoAlign : public CWorkStep
{
/* ������ & �ı��� */
public:

	CWorkExpoAlign();
	virtual ~CWorkExpoAlign();

/* �����Լ� ������ */
protected:
	enum class AlignMotionMode : UINT8
	{
		toInitialMoving = 0, //���� ��ĵ�� ���� �˻� �ʱ���ġ�� �̵��ϴ� ����
		toScanMoving = 1,    //��ĵ�� ���� �̵��ϴ� ����
	};
public:

	map< OffsetType, vector<CaliPoint>> offsetPool;

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();
	int scanCount = 0;
	
	ENG_AMOS alignMotion = ENG_AMOS::en_onthefly_2cam;
	ENG_ATGL aligntype = ENG_ATGL::en_global_4_local_0_point;

	vector<STG_XMXY>	grabMarkPath;
	int globalMarkCnt, localMarkCnt = 0;

	void DoInitOnthefly2cam();
	void DoAlignOnthefly2cam();
	void SetWorkNextOnthefly2cam();

	void DoInitOnthefly3cam();
	void DoAlignOnthefly3cam();
	void SetWorkNextOnthefly3cam();

	void DoInitStatic2cam();
	void DoAlignStatic2cam();
	void SetWorkNextStatic2cam();

	void DoInitStaticCam();
	void DoAlignStaticCam();
	void SetWorkNextStaticCam();



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
										[&]() {DoInitStaticCam(); },
										[&]() {DoAlignStaticCam(); },
										[&]() {SetWorkNextStaticCam(); }
									}},
	};



/* ���� ���� */
protected:

	UINT8				m_u8MarkCount;	/* �� �˻��� ��ũ ���� */
	UINT32				m_u32ExpoCount;		/* ���� �뱤 �ݺ� Ƚ�� */
	STG_CELA			m_stExpoLog;

/* ���� �Լ� */
protected:

	VOID				SetWorkNext();
	VOID				SaveExpoResult(UINT8 state);

	CString				m_strLog;
	VOID				txtWrite(CString msg);
	VOID				SetPhilProcessCompelet();
/* ���� �Լ� */
public:


};
