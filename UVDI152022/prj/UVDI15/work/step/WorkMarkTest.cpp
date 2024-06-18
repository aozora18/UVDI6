
/*
 desc : Align (Both Global and Local Mark) Mark ���� (�ν�)
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
 desc : ������
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
 desc : �ı���
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
 desc : �ʱ� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkMarkTest::InitWork()
{
	/* ���� ��� ���� �� �ʱ�ȭ */
	if (!CWork::InitWork())	return FALSE;

	/* �� �˻� ����� ��ũ ���� ��� */

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
 desc : �ֱ������� �۾� ����
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

//�´��ö��� 3ķ
void CWorkMarkTest::DoAlignOnthefly3cam()
{
	try
	{
		switch (m_u8StepIt)/* �۾� �ܰ� ���� ���� ó�� */
		{
		


		}
	}
	catch (const std::exception& e)
	{
		throw(e);
	}
	
	
}

//����ƽ 2ķ
void CWorkMarkTest::DoAlignStatic2cam()
{
	try
	{
		switch (m_u8StepIt)/* �۾� �ܰ� ���� ���� ó�� */
		{

		}
	}
	catch (const std::exception& e)
	{
		throw(e);
	}
}

//����ƽ 3ķ
void CWorkMarkTest::DoAlignStaticCam()
{
	AlignMotion& motions = GlobalVariables::GetInstance()->GetAlignMotion();

	int CENTER_CAM = motions.markParams.centerCamIdx;

	//���� �߰��� �߰��Ҷ�� ���������� ������ �����ؾ��� �ڵ��� step���� ������ �̷������� �����Ұ�
	static vector<function<void()>> stepWork =
	{
		[&]() 
		{
			if (!(motions.GetCalifeature(CaliTableType::align).caliCamIdx == CENTER_CAM) ||
				!(motions.GetCalifeature(CaliTableType::expo).caliCamIdx == CENTER_CAM)) //���̺��� ���� �ִ������� �˻�.
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

					if (lastGrab == nullptr) return false; //�׷� ��ü�� ����.
					if (lastGrab->IsMarkValid()) return true; //���� ã���ʿ䰡 ����. 
					if (lastGrab->score_rate > 0 || lastGrab->scale_rate > 0) return true; //�̰� �� �׷��� �̰� ã�������� ������ �� ����������, ���⼭ �������� ã�� �������� �� �������� �ִ�. 
				};

			auto ProcessRefind = [&]() -> bool
				{
					float fovStep = 1.0f; // fov ������. mm
					bool sutable = false;
					UINT8 XAXIS = 0b00010000, YAXIS = 0b00100000;
					UINT8 MLEFT = 0b00010001, MRIGHT = 0b00010010 , MUP = 0b00100100, MDOWN = 0b00101000;
					vector<int> searchMap = { MUP,MRIGHT,MDOWN,MDOWN,MLEFT,MLEFT,MUP,MUP };
					
					//�� ã����� ����.
					if (sutable = IsMarkFind() && sutable == true) //���� �׷������ �ùٸ��ٸ� �Ʒ� ������ ������ �ʿ䰡 ����.
						return sutable;

					auto step = searchMap.begin();
					while (sutable == false && step != searchMap.end() && CWork::GetAbort() == false)
					{
						if(sutable = MoveAxis((*step & XAXIS) != 0 ? ENG_MMDI::en_stage_x : ENG_MMDI::en_stage_y,false,
											((*step & MLEFT) != 0 || (*step & MUP) != 0) ? fovStep * -1 : fovStep,true) && sutable == false)
											break;  //�̵�����
						
						if(sutable = SingleGrab(CENTER_CAM) && sutable == false) //�׷�����
							break;

						if(sutable = IsMarkFind() && sutable == true) //��ũã�� ����.
							break;

						if (sutable = uvEng_Camera_RemoveLastGrab(CENTER_CAM) && sutable == false) //���廩����.
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
						return true; //����Ϸ�

					auto first = grabMarkPath.begin();
					auto arrival = motions.MovetoGerberPos(CENTER_CAM, *first);
					string temp = "x" + std::to_string(CENTER_CAM);

					if (arrival == true)
					{
						const int STABLE_TIME = 1000;
						this_thread::sleep_for(chrono::milliseconds(STABLE_TIME));
						motions.Refresh();
						//���⼭ ���� ��ġ��� �������� �������.
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
						if (SingleGrab(CENTER_CAM)) //�� �׷��� �����Ѱ�.
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
							if (USE_REFIND == true && diff.srcFid.tgt_id == MARK2) //�׷��� �����߰�,  use refind�̸� tgt�� mark2�϶�.
							{
								
								ProcessRefind();
								//���⼭ ���� ��ũ2 �׷������� �����ͼ� ��ũ�� �����ϴ��� Ȯ���ϰ� �������� ������ 
							}
							grabMarkPath.erase(first);
						}
						else
						{
							return false; //�׷� ��ü�� ��������.
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


//�´��ö��� 2ķ
void CWorkMarkTest::DoAlignOnthefly2cam()
{
	switch (m_u8StepIt)/* �۾� �ܰ� ���� ���� ó�� */
	{
	case 0x01: 
	{
		m_enWorkState = SetExposeReady(TRUE, TRUE, TRUE, 1);
		if (m_enWorkState == ENG_JWNS::en_next)
			GlobalVariables::GetInstance()->GetAlignMotion().SetFiducialPool();
	}
	break;	    /* �뱤 ������ �������� ���� Ȯ�� */
	case 0x02: 
	{
		
		m_enWorkState = IsLoadedGerberCheck();
	}
	break;	/* �Ź��� ����Ǿ���, Mark�� �����ϴ��� Ȯ�� */
	case 0x03: m_enWorkState = SetTrigEnable(FALSE);						break;	/* Trigger Event - ��Ȱ��ȭ ���� */
	case 0x04: m_enWorkState = IsTrigEnabled(FALSE);						break;	/* Trigger Event - ��Ȱ��ȭ Ȯ��  */
	case 0x05: 
	{
		
		m_enWorkState = SetAlignMovingInit();
	}
	break;	/* Stage X/Y, Camera 1/2 - Align (Global) ���� ��ġ�� �̵� */
	case 0x06: m_enWorkState = SetTrigPosCalcSaved();						break;	/* Trigger �߻� ��ġ ��� �� �ӽ� ���� */
	case 0x07: m_enWorkState = IsAlignMovedInit();							break;	/* Stage X/Y, Camera 1/2 - Align (Global) ���� ��ġ ���� ���� */
	case 0x08: m_enWorkState = SetTrigRegistGlobal();						break;	/* Trigger �߻� ��ġ - Ʈ���� ���忡 Global Mark ��ġ ��� */
	case 0x09: m_enWorkState = IsTrigRegistGlobal();						break;	/* Trigger �߻� ��ġ ��� Ȯ�� */
	case 0x0a: m_enWorkState = SetAlignMeasMode();							break;
	case 0x0b: m_enWorkState = IsAlignMeasMode();							break;
	case 0x0c: m_enWorkState = SetAlignMovingGlobal();						break;	/* Global Mark 4 ���� ��ġ Ȯ�� */
	case 0x0d: m_enWorkState = IsAlignMovedGlobal();						break;	/* Global Mark 4 ���� ���� �Ϸ� ���� */
	case 0x0e:
	{
		
		//������� ������ ���þ������ �ִ°�. ���� �۷ι��� ���������� Ȯ���ؾ���.
		CameraSetCamMode(ENG_VCCM::en_none);
		m_enWorkState = SetAlignMovingLocal((UINT8)AlignMotionMode::toInitialMoving, scanCount);	
	}
	break;

	case 0x0f: m_enWorkState = SetTrigRegistLocal(scanCount);										break;	/* Trigger (������) �߻� ��ġ - Ʈ���� ���忡 Local Mark ��ġ ��� */
	case 0x10: m_enWorkState = IsTrigRegistLocal(scanCount);										break;	/* Trigger (������) �߻� ��ġ ��� Ȯ�� */
	case 0x11: 
	{
		m_enWorkState = IsAlignMovedLocal((UINT8)AlignMotionMode::toInitialMoving, scanCount);
	}
	break;	/* Stage X/Y, Camera 1/2 - Align (Local) ���� ��ġ ���� ���� */

	case 0x12:  
	{
		m_enWorkState = CameraSetCamMode(ENG_VCCM::en_grab_mode);
	}
	break;	/* Cam None ���� ���� */

	case 0x13: m_enWorkState = SetAlignMovingLocal((UINT8)AlignMotionMode::toScanMoving, scanCount);		break;	/* Local Mark (������) 16 ���� ��ġ Ȯ�� */
	case 0x14: m_enWorkState = IsAlignMovedLocal((UINT8)AlignMotionMode::toScanMoving, scanCount);		break;	/* Local Mark (������) 16 ���� ���� �Ϸ� ���� */

	case 0x15:
		this_thread::sleep_for(chrono::milliseconds(200)); // ��� ��ٷ��ִ� ������ �ٽ��� �����忡�� ķ �����͸� �������� ������ �������� Ÿ���� �ֱ⶧��.
		m_u8StepIt = GlobalVariables::GetInstance()->GetAlignMotion().CheckAlignScanFinished(++scanCount) ? 0x16 : 0x0e; //���������� �ٽ� �ö�. 
		m_enWorkState = ENG_JWNS::en_forceSet;
		break;

	case 0x16:
		m_enWorkState = CameraSetCamMode(ENG_VCCM::en_none);
		break;	/* Cam None ���� ���� */

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
 desc : ���� �ܰ�� �̵��ϱ� ���� ó��
 parm : None
 retn : None
*/
VOID CWorkMarkTest::SetWorkNextOnthefly2cam()
{
	
	UINT8 u8WorkTotal = m_u8StepTotal; //IsMarkTypeOnlyGlobal() ? (m_u8StepTotal) : m_u8StepTotal;
	UINT64 u64JobTime = GetTickCount64() - m_u64StartTime;

	/* �� �۾� �������� �ð� �� ���� ó�� */
	uvEng_UpdateJobWorkTime(u64JobTime);

	/* ��� �۾��� ���� �Ǿ����� ���� */
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
		/* �۾��� ��� �� �ӽ� ���� */
		CWork::CalcStepRate();
		/* ���� Global Mark���� �ν� �ߴ��� ���� */
		if (m_u8StepIt == 0x0d)
		{

			/* ���� ���� ��尡 Global ������� Local ���� ������� ���ο� ���� �ٸ� */
			//if (IsMarkTypeOnlyGlobal())	m_u8StepIt	= 0x1a;
			if (IsMarkTypeOnlyGlobal() || uvEng_Luria_GetMarkCount(ENG_AMTF::en_local) == 0)
				m_u8StepIt = 0x16;
		}
		if (m_u8StepTotal == m_u8StepIt)
		{
			/* �۾� �Ϸ� �� ���� �ʿ��� ���� ���� */
			SaveExpoResult(0x01);

			if (++m_u32ExpoCount != m_stExpoLog.expo_count)
			{
				m_u8StepIt = 0x01;
			}
			else
			{
				m_enWorkState = ENG_JWNS::en_comp;
				/* �׻� ȣ��*/
				CWork::EndWork();
			}

		}
		else
		{
			/* ���� �۾� �ܰ�� �̵� */
			m_u8StepIt++;
		}
		/* ���� �ֱٿ� Waiting �� �ð� ���� */
		m_u64DelayTime = GetTickCount64();
	}
}



/*
 desc : ����� ī�޶� 2�븦 ��/�� ������ �̵� ��Ŵ (��� ���� �������� ����)
 parm : None
 retn : TRUE or FALSE
*/
ENG_JWNS CWorkMarkTest::SetHomingACamSide()
{
	LPG_CMSI pstMC2 = &uvEng_GetConfig()->mc2_svc;

	/* ���� ��� ���� �� ��� */
	//uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
	//uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam2);
	/* ¦���� ��, �ּ� �� (���� ��)���� �̵� */
	//if (!m_bMoveACamSide)
	//{
		/* 1�� ī�޶���� home */
	if (!uvEng_MC2_SendDevHoming(ENG_MMDI::en_align_cam1))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to home the align camera (1) to (left)");
		return  ENG_JWNS::en_wait;
	}
	/* 2�� ī�޶���� home */
	if (!uvEng_MC2_SendDevHoming(ENG_MMDI::en_align_cam2))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to home the align camera (2) to (left)");
		return  ENG_JWNS::en_wait;
	}
	//}
	///* Ȧ���� ��, �ִ� �� (���� ��)���� �̵� */
	//else
	//{
	//	/* 2�� ī�޶���� home */
	//	if (!uvEng_MC2_SendDevHoming(ENG_MMDI::en_align_cam2))
	//	{
	//		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to home the align camera (1) to (right)");
	//		return FALSE;
	//	}
	//	/* 1�� ī�޶���� home */
	//	if (!uvEng_MC2_SendDevHoming(ENG_MMDI::en_align_cam1))
	//	{
	//		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to home the align camera (2) to (right)");
	//		return FALSE;
	//	}
	//}

	return	ENG_JWNS::en_next;
}

/*
 desc : 2�븦 ��/�� ������ �̵��� �Ϸ� �Ǿ����� Ȯ�� (��� ���� �������� ����)
 parm : None
 retn : TRUE or FALSE
*/
ENG_JWNS CWorkMarkTest::IsHomedACamSide()
{
	/* �ּ� 1 �и��� �����ϵ��� ���� �� */
//	if (GetTickCount64() < (m_u64TickACamSide + 60000) /* 1 minutue */)	return FALSE;

	/* ��� �̵� �ߴ��� ���� Ȯ�� */
	if (!uvCmn_MC2_IsDriveHomed(ENG_MMDI::en_align_cam1))	return  ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDriveHomed(ENG_MMDI::en_align_cam2))	return  ENG_JWNS::en_wait;

	/* �ݴ������� �̵� */
	//m_bMoveACamSide = !m_bMoveACamSide;

	/* �ֱ� ������ �ð� ���� */
//	m_u64TickACamSide = GetTickCount64();

	return	ENG_JWNS::en_next;
}

/*
 desc : �뱤 �Ϸ� �� ���� ���� ����
 parm : state	- [in]  0x00: �۾� ����, 0x01: �۾� ����
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

	/* ���� ��ǻ�� ��¥�� ���ϸ����� ���� */
	GetLocalTime(&stTm);
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\logs\\expo\\%04d-%02d-%02d MarkText.csv",
		g_tzWorkDir, stTm.wYear, stTm.wMonth, stTm.wDay);

	/* ���࿡ ������ �ϴ� ������ ������, �ش� ���� ���� ��, Ÿ��Ʋ (Field) �߰� */
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

	/* ����� ��ũ �˻� ��� �� ���� */
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

	/* ���а� LED / Board �µ� �� ���� */
	//for (int i = 0x00; i < 0x06; i++)
	//{
	//	swprintf_s(tzResult, 1024, L"%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,",
	//		pLed[i][0] / 10.0f, pLed[i][1] / 10.0f, pLed[i][2] / 10.0f, pLed[i][3] / 10.0f,
	//		pBoard[i][0] / 2.0f, pBoard[i][1] / 2.0f, pBoard[i][2] / 2.0f, pBoard[i][3] / 2.0f);
	//	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	//}
		/* ��ũ ���� 6 �� ���� ���� ���� �� �� ��ü �뱤�ϴµ� �ҿ�� �ð� ���� */
	LPG_GMLV pstMarkDiff = &uvEng_GetConfig()->mark_diff;
	swprintf_s(tzResult, 1024, L"%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,",
		pstMarkDiff->result[0].diff * 100.0f, pstMarkDiff->result[1].diff * 100.0f, pstMarkDiff->result[2].diff * 100.0f,
		pstMarkDiff->result[3].diff * 100.0f, pstMarkDiff->result[4].diff * 100.0f, pstMarkDiff->result[5].diff * 100.0f);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);

	/* �������� ������ ���� �������� �Ѿ���� �ϱ� ���� */
	uvCmn_SaveTxtFileW(L"\n", (UINT32)wcslen(L"\n"), tzFile, 0x01);
}

