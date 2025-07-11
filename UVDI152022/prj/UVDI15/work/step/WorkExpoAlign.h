
#pragma once

#include "../WorkStep.h"
#include <map>
#include <vector>
#include "../../GlobalVariables.h"

using namespace std;
class CWorkExpoAlign : public CWorkStep, BundleAction
{
/* 생성자 & 파괴자 */
public:

	CWorkExpoAlign();
	virtual ~CWorkExpoAlign();

/* 가상함수 재정의 */
protected:
	enum class AlignMotionMode : UINT8
	{
		toInitialMoving = 0, //최초 스캔을 위해 검사 초기위치로 이동하는 스탭
		toScanMoving = 1,    //스캔을 위해 이동하는 스텝
	};
public:

	

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();
	int scanCount = 0;
	
	ENG_AMOS alignMotion = ENG_AMOS::en_onthefly_2cam;
	ENG_ATGL aligntype = ENG_ATGL::en_global_4_local_0_point;
	
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



	bool SetAlignMode();

	
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



/* 로컬 변수 */
protected:

	UINT8				m_u8MarkCount;	/* 총 검색될 마크 개수 */
	UINT32				m_u32ExpoCount;		/* 현재 노광 반복 횟수 */
	STG_CELA			m_stExpoLog;

/* 로컬 함수 */
protected:

	VOID				SetWorkNext();
	VOID				SaveExpoResult(UINT8 state);
	VOID	WriteWebLogForExpoResult(UINT8 state);

	CString				m_strLog;
	VOID				txtWrite(CString msg);
	VOID				SetPhilProcessCompelet();
/* 공용 함수 */
public:

	map< OffsetType, vector<CaliPoint>> offsetPool;
	vector<STG_XMXY>	grabMarkPath;

};

