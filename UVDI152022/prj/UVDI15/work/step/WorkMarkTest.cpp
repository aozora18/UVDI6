
/*
 desc : Align (Both Global and Local Mark) Mark 동작 (인식)
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkMarkTest.h"
#include "../../GlobalVariables.h"

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : None
 retn : None
*/
CWorkMarkTest::CWorkMarkTest(LPG_CELA expo)
	: CWorkStep()
{
	m_enWorkJobID = ENG_BWOK::en_mark_test;
	m_u32ExpoCount = 0;

	m_stExpoLog.Init();
	memcpy(&m_stExpoLog, expo, sizeof(STG_CELA));
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CWorkMarkTest::~CWorkMarkTest()
{
}

BOOL CWorkMarkTest::SetAlignMode()
{
	auto& motion = GlobalVariables::GetInstance()->GetAlignMotion();
	this->alignMotion = motion.markParams.alignMotion;
	this->aligntype = motion.markParams.alignType;
	const int INIT_STEP = 0;


	if (alignMotion == ENG_AMOS::none || aligntype == ENG_ATGL::en_global_0_local_0x0_n_point)
		return FALSE;

	alignCallback[alignMotion][INIT_STEP]();
	return TRUE;
}


/*
 desc : 초기 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkMarkTest::InitWork()
{
	/* 내부 멤버 변수 값 초기화 */
	if (!CWork::InitWork())	return FALSE;

	/* 총 검색 대상의 마크 개수 얻기 */

	globalMarkCnt = uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	localMarkCnt = uvEng_Luria_GetMarkCount(ENG_AMTF::en_local);

	m_u8MarkCount = globalMarkCnt + (IsMarkTypeOnlyGlobal() == true ? 0 : localMarkCnt);
	return SetAlignMode();
}

void CWorkMarkTest::DoInitStatic2cam()
{
	int debug = 0;
}

void CWorkMarkTest::DoInitStaticCam()
{
	m_u8StepTotal = 0;
}

void CWorkMarkTest::DoInitOnthefly3cam()
{
	int debug = 0;
}

void CWorkMarkTest::DoInitOnthefly2cam()
{
	m_u8StepTotal = 0x21;
}


/*
 desc : 주기적으로 작업 수행
 parm : None
 retn : None
*/
VOID CWorkMarkTest::DoWork()
{
	const int Initstep = 0 , processWork = 1, checkWorkstep = 2;
	try
	{
		alignCallback[alignMotion][processWork]();
		alignCallback[alignMotion][checkWorkstep]();
		
	}
	catch (const std::exception&)
	{

	}
	
}

//온더플라이 3캠
void CWorkMarkTest::DoAlignOnthefly3cam()
{
	try
	{
		switch (m_u8StepIt)/* 작업 단계 별로 동작 처리 */
		{
		


		}
	}
	catch (const std::exception& e)
	{
		throw(e);
	}
	
	
}

//스테틱 2캠
void CWorkMarkTest::DoAlignStatic2cam()
{
	try
	{
		switch (m_u8StepIt)/* 작업 단계 별로 동작 처리 */
		{

		}
	}
	catch (const std::exception& e)
	{
		throw(e);
	}
}

//스테틱 3캠
void CWorkMarkTest::DoAlignStaticCam()
{
	AlignMotion& motions = GlobalVariables::GetInstance()->GetAlignMotion();

	int CENTER_CAM = motions.markParams.centerCamIdx;

	//스텝 중간에 추가할라면 지랄같으니 앞으로 수정해야할 코드중 step구조 있으면 이런식으로 변경할것
	static vector<function<void()>> stepWork =
	{
		[&]() 
		{
			if (!(motions.GetCalifeature(CaliTableType::align).caliCamIdx == CENTER_CAM) ||
				!(motions.GetCalifeature(CaliTableType::expo).caliCamIdx == CENTER_CAM)) //테이블이 전부 있는지부터 검사.
				{
					m_enWorkState = ENG_JWNS::en_error;
				}
			else
				m_enWorkState = ENG_JWNS::en_next;
				
		},
		[&]()
		{
			m_enWorkState = SetExposeReady(TRUE, TRUE, TRUE, 1);
		},

		[&]()
		{
			static map<int, ENG_MMDI> axisMap = 
			{ 
				{1,ENG_MMDI::en_align_cam1}, 
				{2,ENG_MMDI::en_align_cam2}, 
				{3,ENG_MMDI::en_axis_none} 
			};
			auto res = MoveCamToSafetypos(axisMap[CENTER_CAM], motions.GetCalifeature(CaliTableType::expo).caliCamXPos);
											m_enWorkState = res ? ENG_JWNS::en_next : ENG_JWNS::en_error; 
		},
		[&]()
		{
			m_enWorkState = SetTrigEnable(FALSE);
		},
		[&]()
		{
			m_enWorkState = IsTrigEnabled(FALSE);
		},
		[&]()
		{
			uvEng_ACamCali_ResetAllCaliData();
			uvEng_Camera_ResetGrabbedImage();
			m_enWorkState = CameraSetCamMode(ENG_VCCM::en_grab_mode);
		},
		[&]()
		{
			offsetPool.clear();
			motions.SetFiducialPool();
			grabMarkPath = motions.GetFiducialPool(CENTER_CAM);
			m_enWorkState = grabMarkPath.size() == 0 ? ENG_JWNS::en_error : ENG_JWNS::en_next;
		},
		[&]()
		{
			const int MARK2 = 2;
			const bool USE_REFIND = true;

			auto SingleGrab = [&](int camIndex) -> bool 
			{
				auto triggerMode = uvEng_Camera_GetTriggerMode(CENTER_CAM);
				static vector<function<bool()>> trigAction =
				{
					[&]() {return uvEng_Camera_SWGrab(CENTER_CAM); },
					[&]() {return uvEng_Mvenc_ReqTrigOutOne(CENTER_CAM); },
				};

				auto res = trigAction[triggerMode == ENG_TRGM::en_Sw_mode ? 0 : 1]();
				m_enWorkState = res ? ENG_JWNS::en_next : ENG_JWNS::en_error;
				return res;
			};

			auto IsMarkFind = [&]() -> bool
				{
					auto lastGrab = uvEng_Camera_GetLastGrabbedMark();

					if (lastGrab == nullptr) return false; //그랩 자체가 문제.
					if (lastGrab->IsMarkValid()) return true; //이젠 찾을필요가 없다. 
					if (lastGrab->score_rate > 0 || lastGrab->scale_rate > 0) return true; //이게 좀 그런데 이건 찾긴했으나 조건이 좀 안좋은상태, 여기서 최적조건 찾는 로직으로 또 빠질수가 있다. 
				};

			auto ProcessRefind = [&]() -> bool
				{
					float fovStep = 1.0f; // fov 절반임. mm
					bool sutable = false;
					UINT8 XAXIS = 0b00010000, YAXIS = 0b00100000;
					UINT8 MLEFT = 0b00010001, MRIGHT = 0b00010010 , MUP = 0b00100100, MDOWN = 0b00101000;
					vector<int> searchMap = { MUP,MRIGHT,MDOWN,MDOWN,MLEFT,MLEFT,MUP,MUP };
					
					//자 찾기로직 시작.
					if (sutable = IsMarkFind() && sutable == true) //이전 그랩결과가 올바르다면 아래 로직을 수행할 필요가 없다.
						return sutable;

					auto step = searchMap.begin();
					while (sutable == false && step != searchMap.end() && CWork::GetAbort() == false)
					{
						if(sutable = MoveAxis((*step & XAXIS) != 0 ? ENG_MMDI::en_stage_x : ENG_MMDI::en_stage_y,false,
											((*step & MLEFT) != 0 || (*step & MUP) != 0) ? fovStep * -1 : fovStep,true) && sutable == false)
											break;  //이동실패
						
						if(sutable = SingleGrab(CENTER_CAM) && sutable == false) //그랩실패
							break;

						if(sutable = IsMarkFind() && sutable == true) //마크찾기 성공.
							break;

						if (sutable = uvEng_Camera_RemoveLastGrab(CENTER_CAM) && sutable == false) //막장빼야함.
							break;

						step++;
					}

					return sutable;
				};


			bool complete = GlobalVariables::GetInstance()->Waiter([&]()->bool
			{
				if (motions.NowOnMoving() == true)
				{
					this_thread::sleep_for(chrono::milliseconds(100));
				}
				else
				{
					if (grabMarkPath.size() == 0) 
						return true; //정상완료

					auto first = grabMarkPath.begin();
					auto arrival = motions.MovetoGerberPos(CENTER_CAM, *first);
					string temp = "x" + std::to_string(CENTER_CAM);

					if (arrival == true)
					{
						const int STABLE_TIME = 1000;
						this_thread::sleep_for(chrono::milliseconds(STABLE_TIME));
						motions.Refresh();
						//여기서 현재 위치기반 보정정보 갖고오기.
						auto alignOffset = motions.EstimateAlignOffset(CENTER_CAM, motions.GetAxises()["stage"]["x"].currPos,
																				motions.GetAxises()["stage"]["y"].currPos,
																				CENTER_CAM == 3 ? 0 : motions.GetAxises()["cam"][temp.c_str()].currPos);

						auto markPos = first->GetMarkPos();
						auto expoOffset = motions.EstimateExpoOffset(std::get<0>(markPos), std::get<1>(markPos));

						alignOffset.srcFid = *first;
						offsetPool[CaliTableType::align].push_back(alignOffset);

						auto diff = expoOffset;
						diff.srcFid = *first;
						offsetPool[CaliTableType::expo].push_back(diff);

						TCHAR tzMsg[256] = { NULL };
						if (SingleGrab(CENTER_CAM)) //딱 그랩만 성공한거.
						{
							if (uvEng_GetConfig()->set_align.use_2d_cali_data)
							{
								uvEng_ACamCali_AddMarkPosForce(CENTER_CAM, first->GetFlag(STG_XMXY_RESERVE_FLAG::GLOBAL) ? ENG_AMTF::en_global : ENG_AMTF::en_local, alignOffset.offsetX, alignOffset.offsetY);
								swprintf_s(tzMsg, 256, L"%s", first->GetFlag(STG_XMXY_RESERVE_FLAG::GLOBAL) ? L"global" : L"local");
								LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
								swprintf_s(tzMsg, 256, L"align%d_offset_x = %.4f mark_offset_y =%.4f", first->org_id, alignOffset.offsetX, alignOffset.offsetY);
								LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
								swprintf_s(tzMsg, 256, L"expo%d_offset_x = %.4f mark_offset_y =%.4f", first->org_id, diff.offsetX, diff.offsetY);
								LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
							}
							if (USE_REFIND == true && diff.srcFid.tgt_id == MARK2) //그랩엔 성공했고,  use refind이며 tgt가 mark2일때.
							{
								
								ProcessRefind();
								//여기서 실제 마크2 그랩데이터 가져와서 마크가 존재하는지 확인하고 존재하지 않으면 
							}
							grabMarkPath.erase(first);
						}
						else
						{
							return false; //그랩 자체를 못했을때.
						}
					}
				}
				return false;
			}, 60 * 1000 * 2);
			m_enWorkState = complete == true && CWork::GetAbort() == false ? ENG_JWNS::en_next : ENG_JWNS::en_error;
		},
		[&]()
		{
			SetUIRefresh(true);
			m_enWorkState = IsGrabbedImageCount(m_u8MarkCount, 3000, &CENTER_CAM);
		},
		[&]()
		{
			m_enWorkState = IsSetMarkValidAll(0x01, &CENTER_CAM);
		},
		[&]()
		{
			motions.SetAlignOffsetPool(offsetPool);
			m_enWorkState = CameraSetCamMode(ENG_VCCM::en_none);
		},
		[&]()
		{
			m_enWorkState = SetAlignMarkRegistforStatic();
		},
		[&]()
		{
			m_enWorkState = IsAlignMarkRegist();
		},
		[&]()
		{
			m_enWorkState = SetWorkWaitTime(1000);
		},
		[&]()
		{
			m_enWorkState = IsWorkWaitTime();
		},
		[&]()
		{
			m_enWorkState = SetMovingUnloader();
		},
		[&]()
		{
			m_enWorkState = IsMovedUnloader();
		},
	};

	m_u8StepTotal = stepWork.size();

	try
	{
		stepWork[m_u8StepIt]();
		if (CWork::GetAbort())
			m_enWorkState = ENG_JWNS::en_error;
	}
	catch (const std::exception&)
	{

	}
}


//온더플라이 2캠
void CWorkMarkTest::DoAlignOnthefly2cam()
{
	switch (m_u8StepIt)/* 작업 단계 별로 동작 처리 */
	{
	case 0x01: 
	{
		m_enWorkState = SetExposeReady(TRUE, TRUE, TRUE, 1);
		if (m_enWorkState == ENG_JWNS::en_next)
			GlobalVariables::GetInstance()->GetAlignMotion().SetFiducialPool();
	}
	break;	    /* 노광 가능한 상태인지 여부 확인 */
	case 0x02: 
	{
		
		m_enWorkState = IsLoadedGerberCheck();
	}
	break;	/* 거버가 적재되었고, Mark가 존재하는지 확인 */
	case 0x03: m_enWorkState = SetTrigEnable(FALSE);						break;	/* Trigger Event - 비활성화 설정 */
	case 0x04: m_enWorkState = IsTrigEnabled(FALSE);						break;	/* Trigger Event - 빌활성화 확인  */
	case 0x05: 
	{
		
		m_enWorkState = SetAlignMovingInit();
	}
	break;	/* Stage X/Y, Camera 1/2 - Align (Global) 시작 위치로 이동 */
	case 0x06: m_enWorkState = SetTrigPosCalcSaved();						break;	/* Trigger 발생 위치 계산 및 임시 저장 */
	case 0x07: m_enWorkState = IsAlignMovedInit();							break;	/* Stage X/Y, Camera 1/2 - Align (Global) 시작 위치 도착 여부 */
	case 0x08: m_enWorkState = SetTrigRegistGlobal();						break;	/* Trigger 발생 위치 - 트리거 보드에 Global Mark 위치 등록 */
	case 0x09: m_enWorkState = IsTrigRegistGlobal();						break;	/* Trigger 발생 위치 등록 확인 */
	case 0x0a: m_enWorkState = SetAlignMeasMode();							break;
	case 0x0b: m_enWorkState = IsAlignMeasMode();							break;
	case 0x0c: m_enWorkState = SetAlignMovingGlobal();						break;	/* Global Mark 4 군데 위치 확인 */
	case 0x0d: m_enWorkState = IsAlignMovedGlobal();						break;	/* Global Mark 4 군데 측정 완료 여부 */
	case 0x0e:
	{
		
		//여기까지 왔으면 로컬얼라인이 있는것. 먼저 글로벌이 몇장찍혔나 확인해야함.
		CameraSetCamMode(ENG_VCCM::en_none);
		m_enWorkState = SetAlignMovingLocal((UINT8)AlignMotionMode::toInitialMoving, scanCount);	
	}
	break;

	case 0x0f: m_enWorkState = SetTrigRegistLocal(scanCount);										break;	/* Trigger (역방향) 발생 위치 - 트리거 보드에 Local Mark 위치 등록 */
	case 0x10: m_enWorkState = IsTrigRegistLocal(scanCount);										break;	/* Trigger (역방향) 발생 위치 등록 확인 */
	case 0x11: 
	{
		m_enWorkState = IsAlignMovedLocal((UINT8)AlignMotionMode::toInitialMoving, scanCount);
	}
	break;	/* Stage X/Y, Camera 1/2 - Align (Local) 시작 위치 도착 여부 */

	case 0x12:  
	{
		m_enWorkState = CameraSetCamMode(ENG_VCCM::en_grab_mode);
	}
	break;	/* Cam None 모드로 변경 */

	case 0x13: m_enWorkState = SetAlignMovingLocal((UINT8)AlignMotionMode::toScanMoving, scanCount);		break;	/* Local Mark (역방향) 16 군데 위치 확인 */
	case 0x14: m_enWorkState = IsAlignMovedLocal((UINT8)AlignMotionMode::toScanMoving, scanCount);		break;	/* Local Mark (역방향) 16 군데 측정 완료 여부 */

	case 0x15:
		this_thread::sleep_for(chrono::milliseconds(200)); // 잠깐 기다려주는 이유가 바슬러 스레드에서 캠 데이터를 가져가는 스레드 리프레시 타임이 있기때문.
		m_u8StepIt = GlobalVariables::GetInstance()->GetAlignMotion().CheckAlignScanFinished(++scanCount) ? 0x16 : 0x0e; //남아있으면 다시 올라감. 
		m_enWorkState = ENG_JWNS::en_forceSet;
		break;

	case 0x16:
		m_enWorkState = CameraSetCamMode(ENG_VCCM::en_none);
		break;	/* Cam None 모드로 변경 */

	case 0x17: m_enWorkState = SetTrigEnable(FALSE);						break;
	case 0x18: 
	{
		SetUIRefresh(true);
		m_enWorkState = IsGrabbedImageCount(m_u8MarkCount, 3000);
	}
	break;
	case 0x19: 
	{
		
		m_enWorkState = IsSetMarkValidAll(0x01);
	}
	break;
	case 0x1a: 
	{
		m_enWorkState = SetAlignMarkRegist();
		//m_enWorkState = ENG_JWNS::en_next;
	}
	break;

	case 0x1b: m_enWorkState = IsAlignMarkRegist();							break;
	case 0x1c: m_enWorkState = IsTrigEnabled(FALSE);						break;
	case 0x1d: m_enWorkState = SetWorkWaitTime(2000);						break;
	case 0x1e: m_enWorkState = IsWorkWaitTime();							break;
	case 0x20: 	
	{

		m_enWorkState = SetMovingUnloader();
	}
	break;
	case 0x21: m_enWorkState = IsMovedUnloader();							break;
	/*case 0x22: m_enWorkState = SetHomingACamSide();							break;
	case 0x23: m_enWorkState = IsHomedACamSide();							break;
	case 0x24: m_enWorkState = SetWorkWaitTime(10000);						break;
	case 0x25: m_enWorkState = IsWorkWaitTime();							break;*/
	}
}



VOID CWorkMarkTest::SetWorkNextOnthefly3cam()
{

}


VOID CWorkMarkTest::SetWorkNextStatic2cam()
{

}


VOID CWorkMarkTest::SetWorkNextStaticCam()
{
	UINT8 u8WorkTotal = m_u8StepTotal;
	UINT64 u64JobTime = GetTickCount64() - m_u64StartTime;

	uvEng_UpdateJobWorkTime(u64JobTime);

	if (ENG_JWNS::en_error == m_enWorkState)
	{
		TCHAR tzMesg[128] = { NULL };
		swprintf_s(tzMesg, 128, L"Align Test <Error Step It = 0x%02x>", m_u8StepIt);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);

		SaveExpoResult(0x00);
		m_u8StepIt = 0x01;
		m_enWorkState = ENG_JWNS::en_error;
	}
	else if (ENG_JWNS::en_next == m_enWorkState)
	{
		CWork::CalcStepRate();
		if (m_u8StepTotal == m_u8StepIt)
		{
			SaveExpoResult(0x01);

			if (++m_u32ExpoCount != m_stExpoLog.expo_count)
			{
				m_u8StepIt = 0x01;
			}
			else
			{
				m_enWorkState = ENG_JWNS::en_comp;
				CWork::EndWork();
			}
		}
		else
		{
			m_u8StepIt++;
		}
		m_u64DelayTime = GetTickCount64();
	}
	CheckWorkTimeout();
}


/*
 desc : 다음 단계로 이동하기 위한 처리
 parm : None
 retn : None
*/
VOID CWorkMarkTest::SetWorkNextOnthefly2cam()
{
	
	UINT8 u8WorkTotal = m_u8StepTotal; //IsMarkTypeOnlyGlobal() ? (m_u8StepTotal) : m_u8StepTotal;
	UINT64 u64JobTime = GetTickCount64() - m_u64StartTime;

	/* 매 작업 구간마다 시간 값 증가 처리 */
	uvEng_UpdateJobWorkTime(u64JobTime);

	/* 모든 작업이 종료 되었는지 여부 */
	if (ENG_JWNS::en_error == m_enWorkState)
	{
		TCHAR tzMesg[128] = { NULL };
		swprintf_s(tzMesg, 128, L"Align Test <Error Step It = 0x%02x>", m_u8StepIt);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);

		SaveExpoResult(0x00);
		m_u8StepIt = 0x01;
		m_enWorkState = ENG_JWNS::en_error;
	}
	else if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* 작업률 계산 후 임시 저장 */
		CWork::CalcStepRate();
		/* 현재 Global Mark까지 인식 했는지 여부 */
		if (m_u8StepIt == 0x0d)
		{

			/* 현재 동작 모드가 Global 방식인지 Local 포함 방식인지 여부에 따라 다름 */
			//if (IsMarkTypeOnlyGlobal())	m_u8StepIt	= 0x1a;
			if (IsMarkTypeOnlyGlobal() || uvEng_Luria_GetMarkCount(ENG_AMTF::en_local) == 0)
				m_u8StepIt = 0x16;
		}
		if (m_u8StepTotal == m_u8StepIt)
		{
			/* 작업 완료 후 각종 필요한 정보 저장 */
			SaveExpoResult(0x01);

			if (++m_u32ExpoCount != m_stExpoLog.expo_count)
			{
				m_u8StepIt = 0x01;
			}
			else
			{
				m_enWorkState = ENG_JWNS::en_comp;
				/* 항상 호출*/
				CWork::EndWork();
			}

		}
		else
		{
			/* 다음 작업 단계로 이동 */
			m_u8StepIt++;
		}
		/* 가장 최근에 Waiting 한 시간 저장 */
		m_u64DelayTime = GetTickCount64();
	}
}



/*
 desc : 얼라인 카메라 2대를 좌/우 끝으로 이동 시킴 (장비 예열 목적으로 사용됨)
 parm : None
 retn : TRUE or FALSE
*/
ENG_JWNS CWorkMarkTest::SetHomingACamSide()
{
	LPG_CMSI pstMC2 = &uvEng_GetConfig()->mc2_svc;

	/* 현재 토글 상태 값 얻기 */
	//uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
	//uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam2);
	/* 짝수일 땐, 최소 값 (좌측 끝)으로 이동 */
	//if (!m_bMoveACamSide)
	//{
		/* 1번 카메라부터 home */
	if (!uvEng_MC2_SendDevHoming(ENG_MMDI::en_align_cam1))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to home the align camera (1) to (left)");
		return  ENG_JWNS::en_wait;
	}
	/* 2번 카메라부터 home */
	if (!uvEng_MC2_SendDevHoming(ENG_MMDI::en_align_cam2))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to home the align camera (2) to (left)");
		return  ENG_JWNS::en_wait;
	}
	//}
	///* 홀수일 때, 최대 값 (우측 끝)으로 이동 */
	//else
	//{
	//	/* 2번 카메라부터 home */
	//	if (!uvEng_MC2_SendDevHoming(ENG_MMDI::en_align_cam2))
	//	{
	//		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to home the align camera (1) to (right)");
	//		return FALSE;
	//	}
	//	/* 1번 카메라부터 home */
	//	if (!uvEng_MC2_SendDevHoming(ENG_MMDI::en_align_cam1))
	//	{
	//		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to home the align camera (2) to (right)");
	//		return FALSE;
	//	}
	//}

	return	ENG_JWNS::en_next;
}

/*
 desc : 2대를 좌/우 끝으로 이동이 완료 되었는지 확인 (장비 예열 목적으로 사용됨)
 parm : None
 retn : TRUE or FALSE
*/
ENG_JWNS CWorkMarkTest::IsHomedACamSide()
{
	/* 최소 1 분마다 동작하도록 설정 함 */
//	if (GetTickCount64() < (m_u64TickACamSide + 60000) /* 1 minutue */)	return FALSE;

	/* 모두 이동 했는지 여부 확인 */
	if (!uvCmn_MC2_IsDriveHomed(ENG_MMDI::en_align_cam1))	return  ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDriveHomed(ENG_MMDI::en_align_cam2))	return  ENG_JWNS::en_wait;

	/* 반대편으로 이동 */
	//m_bMoveACamSide = !m_bMoveACamSide;

	/* 최근 동작한 시간 저장 */
//	m_u64TickACamSide = GetTickCount64();

	return	ENG_JWNS::en_next;
}

/*
 desc : 노광 완료 후 각종 정보 저장
 parm : state	- [in]  0x00: 작업 실패, 0x01: 작업 성공
 retn : None
*/
VOID CWorkMarkTest::SaveExpoResult(UINT8 state)
{
	TCHAR tzResult[1024] = { NULL }, tzFile[MAX_PATH_LEN] = { NULL }, tzState[2][8] = { L"FAIL", L"SUCC" };
	LPG_ACGR pstMark = NULL;
	SYSTEMTIME stTm = { NULL };
	MEMORYSTATUSEX stMem = { NULL };

	uvEng_Luria_ReqGetPhLedTempAll();
	UINT16(*pLed)[8] = uvEng_ShMem_GetLuria()->directph.light_source_driver_temp_led;
	UINT16(*pBoard)[8] = uvEng_ShMem_GetLuria()->directph.light_source_driver_temp_board;

	/* 현재 컴퓨터 날짜를 파일명으로 설정 */
	GetLocalTime(&stTm);
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\logs\\expo\\%04d-%02d-%02d MarkText.csv",
		g_tzWorkDir, stTm.wYear, stTm.wMonth, stTm.wDay);

	/* 만약에 열고자 하는 파일이 없으면, 해당 파일 생성 후, 타이틀 (Field) 추가 */
	if (!uvCmn_FindFile(tzFile))
	{
		wcscpy_s(tzResult, 1024,
			L"Count,Time,succ,"
			L"score_1,scale_1,mark_move_x1(mm),mark_move_y1(mm),"
			L"score_2,scale_2,mark_move_x2(mm),mark_move_y2(mm),"
			L"score_3,scale_3,mark_move_x3(mm),mark_move_y3(mm),"
			L"score_4,scale_4,mark_move_x4(mm),mark_move_y4(mm),"
			//L"led(1:1),led(1:2),led(1:3),led(1:4),board(1:1),board(1:2),board(1:3),board(1:4),"
			//L"led(2:1),led(2:2),led(2:3),led(2:4),board(2:1),board(2:2),board(2:3),board(2:4),"
			//L"led(3:1),led(3:2),led(3:3),led(3:4),board(3:1),board(3:2),board(3:3),board(3:4),"
			//L"led(4:1),led(4:2),led(4:3),led(4:4),board(4:1),board(4:2),board(4:3),board(4:4),"
			//L"led(5:1),led(5:2),led(5:3),led(5:4),board(5:1),board(5:2),board(5:3),board(5:4),"
			//L"led(6:1),led(6:2),led(6:3),led(6:4),board(6:1),board(6:2),board(6:3),board(6:4),\n");
			L"horz_dist_13(mm),horz_dist_24(mm),vert_dist_12(mm),vert_dist_34(mm),"
			L"diag_dist_14(mm),diag_dist_23(mm),\n");
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x00);
	}

	swprintf_s(tzResult, 1024, L"Count=%d, %02d:%02d:%02d,%s,",
		m_u32ExpoCount, stTm.wHour, stTm.wMinute, stTm.wSecond, tzState[state]);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);

	/* 얼라인 마크 검색 결과 값 저장 */
	pstMark = uvEng_Camera_GetGrabbedMark(0x01, 0x00);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%.4f,%.4f,",
			pstMark->score_rate, pstMark->scale_rate,
			pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	pstMark = uvEng_Camera_GetGrabbedMark(0x01, 0x01);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%.4f,%.4f,",
			pstMark->score_rate, pstMark->scale_rate,
			pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	pstMark = uvEng_Camera_GetGrabbedMark(0x02, 0x00);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%.4f,%.4f,",
			pstMark->score_rate, pstMark->scale_rate,
			pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	pstMark = uvEng_Camera_GetGrabbedMark(0x02, 0x01);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%.4f,%.4f,",
			pstMark->score_rate, pstMark->scale_rate,
			pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}

	/* 광학계 LED / Board 온도 값 저장 */
	//for (int i = 0x00; i < 0x06; i++)
	//{
	//	swprintf_s(tzResult, 1024, L"%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,",
	//		pLed[i][0] / 10.0f, pLed[i][1] / 10.0f, pLed[i][2] / 10.0f, pLed[i][3] / 10.0f,
	//		pBoard[i][0] / 2.0f, pBoard[i][1] / 2.0f, pBoard[i][2] / 2.0f, pBoard[i][3] / 2.0f);
	//	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	//}
		/* 마크 간의 6 곳 길이 측정 오차 값 과 전체 노광하는데 소요된 시간 저장 */
	LPG_GMLV pstMarkDiff = &uvEng_GetConfig()->mark_diff;
	swprintf_s(tzResult, 1024, L"%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,",
		pstMarkDiff->result[0].diff * 100.0f, pstMarkDiff->result[1].diff * 100.0f, pstMarkDiff->result[2].diff * 100.0f,
		pstMarkDiff->result[3].diff * 100.0f, pstMarkDiff->result[4].diff * 100.0f, pstMarkDiff->result[5].diff * 100.0f);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);

	/* 마지막엔 무조건 다음 라인으로 넘어가도록 하기 위함 */
	uvCmn_SaveTxtFileW(L"\n", (UINT32)wcslen(L"\n"), tzFile, 0x01);
}

