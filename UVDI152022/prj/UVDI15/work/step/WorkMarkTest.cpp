
/*
 desc : Align �뱤 ���� - ����� ī�޶� X �� �̵� ���� �������� Y �ุ ����
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkMarkTest.h"
#include "../../GlobalVariables.h"
#include "../../stuffs.h"
#include <atlstr.h>  // CString �� ��ȯ ��ũ�θ� ����ϱ� ���� �ʿ�
#include <iostream>
#include <string>
//#include <fmt/core.h>

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

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
	m_u8StepIt = 0;
	m_stExpoLog.Init();
	memcpy(&m_stExpoLog, expo, sizeof(STG_CELA));

	/*UI�� ǥ�õǴ� �߱Ⱚ �ʱ�ȭ*/
	LPG_PPTP pstParams = &uvEng_ShMem_GetLuria()->panel.get_transformation_params;
	pstParams->rotation = 0;
	pstParams->scale_xy[0] = 0;
	pstParams->scale_xy[1] = 0;
	uvEng_GetConfig()->measure_flat.MeasurePoolClear();
	LPG_GMLV pstMarkDiff = &uvEng_GetConfig()->mark_diff;
	pstMarkDiff->ResetMarkLen();
	GlobalVariables::GetInstance()->GetAlignMotion().markParams.workErrorType = ENG_WETE::en_none;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CWorkMarkTest::~CWorkMarkTest()
{
	if (bundleAction[0].joinable())
		bundleAction[0].join();

	if (bundleAction[1].joinable())
		bundleAction[1].join();

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
 desc : �뱤 �Ϸ� �� ���� ���� ����
 parm : state	- [in]  0x00: �۾� ����, 0x01: �۾� ����
 retn : None
*/
BOOL CWorkMarkTest::InitWork()
{
	//UINT8 i;
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
	m_u8StepIt = 0;
	m_u8StepTotal = 0;
}

void CWorkMarkTest::DoInitOnthefly3cam()
	{
	int debug = 0;
		}

void CWorkMarkTest::DoInitOnthefly2cam()
	{
	m_u8StepIt = 1;
	m_u8StepTotal = 0x21;
	}


/*
 desc : �ֱ������� �۾� ����
 parm : None
 retn : None
*/
VOID CWorkMarkTest::DoWork()
	{
	const int Initstep = 0, processWork = 1, checkWorkstep = 2;
	try
	{
		alignCallback[alignMotion][processWork]();
		alignCallback[alignMotion][checkWorkstep]();

	}
	catch (const std::exception&)
	{
		CWork::EndWork();

	}

	}


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
	RefindMotion& refindMotion = GlobalVariables::GetInstance()->GetRefindMotion();

	int CENTER_CAM = motions.markParams.centerCamIdx;
	auto& webMonitor = GlobalVariables::GetInstance()->GetWebMonitor();
	const int PAIR = 2;
	const int MARK1 = 0, MARK2 = 1;
	bool refind = refindMotion.IsUseRefind();

	vector<function<void()>> stepWork =
	{
		[&]()
		{
			
			m_enWorkState = SetExposeStartXY();
		},
		[&]()
		{
			m_enWorkState = IsExposeStartXY();
		},

		[&]()
		{
			if (!(motions.GetCalifeature(OffsetType::align).caliCamIdx == CENTER_CAM) ||
				!(motions.GetCalifeature(OffsetType::expo).caliCamIdx == CENTER_CAM)) //���̺��� ���� �ִ������� �˻�.
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
			auto res = MoveCamToSafetypos(axisMap[CENTER_CAM], motions.GetCalifeature(OffsetType::expo).caliCamXPos);
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
			SetActionRequest(ENG_RIJA::clearMarkData);
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
			if (refind)
			{
				SetStepName(L"mark refind step");
				SetActionRequest(ENG_RIJA::invalidateUI);
			}
			m_enWorkState = ENG_JWNS::en_next;
		},
		[&]()
		{

			if (refind == false)
			{
				m_enWorkState = ENG_JWNS::en_next;
			}
			else
			{
				bool errFlag = false;
				try
				{
					//std::vector<STG_XMXY> filteredPath, offsetBuff;
					//std::copy_if(grabMarkPath.begin(), grabMarkPath.end(), std::back_inserter(filteredPath),
					//			[&](const STG_XMXY& v) { return v.tgt_id == MARK1 || v.tgt_id == MARK2; });
					std::vector<STG_XMXY> filteredPath, offsetBuff;
					std::copy_if(grabMarkPath.begin(), grabMarkPath.end(), std::back_inserter(filteredPath),
						[&](const STG_XMXY& v) { return (v.reserve & STG_XMXY_RESERVE_FLAG::GLOBAL) == STG_XMXY_RESERVE_FLAG::GLOBAL && (v.tgt_id == MARK1 || v.tgt_id == MARK2); });


					if (filteredPath.size() != PAIR)
						throw exception();

					if (refindMotion.ProcessEstimateRST(CENTER_CAM, filteredPath, errFlag, offsetBuff) == false)
						throw exception();

					if (errFlag == true)
						throw exception();

					auto match = std::remove_if(grabMarkPath.begin(), grabMarkPath.end(),
						[&](const STG_XMXY& v) {  return (v.reserve & STG_XMXY_RESERVE_FLAG::GLOBAL) == STG_XMXY_RESERVE_FLAG::GLOBAL && (v.tgt_id == MARK1 || v.tgt_id == MARK2); });

					std::transform(offsetBuff.begin(), offsetBuff.end(), std::back_inserter(offsetPool[OffsetType::refind]),
					[&](STG_XMXY v)->CaliPoint
					{
						auto find = std::find_if(filteredPath.begin(), filteredPath.end(), [&](STG_XMXY sv) {return sv.org_id == v.org_id; });
						return CaliPoint(find->mark_x,find->mark_y, v.mark_x,v.mark_y,v.offsetX,v.offsetY, *find);
					}); //�����ε� �ɼ¿� 2�� �߰�.

					grabMarkPath.erase(match, grabMarkPath.end());//2����Ʈ�� �������� ���ָ�ȴ�. 

					//*****************************************************************************//
					CaliPoint align;

					//for (int i = 0; i < PAIR; i++)
					//{
					//	CommonMotionStuffs::GetInstance().GetOffsetsUseMarkPos(CENTER_CAM, filteredPath[i], &align, nullptr, offsetBuff[i].mark_x , offsetBuff[i].mark_y ); //<-�����ɼ� ���������
					//	offsetPool[OffsetType::align].push_back(align);						
					//}

					SetUIRefresh(true);
					m_enWorkState = ENG_JWNS::en_next;
				}
				catch (...)
				{
					m_enWorkState = ENG_JWNS::en_error;
					return;
				}
			}
		},
		[&]()
		{
			const int STABLE_TIME = 1000;
			bool complete = false;

			SetStepName(L"mark find step");
			SetActionRequest(ENG_RIJA::invalidateUI);
			complete = GlobalVariables::GetInstance()->Waiter([&]()->bool
			{
				try
				{
					if (CommonMotionStuffs::GetInstance().NowOnMoving())
					{
						return false;
					}
					else
					{
						auto currPath = grabMarkPath.begin();

						bool arrival = false;
						double grabOffsetX = 0, grabOffsetY = 0;
						double estimatedX = 0, estimatedY = 0;
						STG_XMXY estimatedXMXY = *currPath;//STG_XMXY(currPath->mark_x, currPath->mark_y, currPath->org_id);
						CaliPoint alignOffset;// , expoOffset;// , refindOffset;

						if (refind)
							refindMotion.GetEstimatePos(currPath->mark_x, currPath->mark_y, estimatedXMXY.mark_x, estimatedXMXY.mark_y);

						if (motions.MovetoGerberPos(CENTER_CAM, estimatedXMXY) == false)
							throw exception();

						this_thread::sleep_for(chrono::milliseconds(STABLE_TIME));
						motions.Refresh();

						if (CommonMotionStuffs::GetInstance().SingleGrab(CENTER_CAM,false) == false || CWork::GetAbort()) //�׷�����. �۾� �ܺ�����
							throw exception();

						auto found = CommonMotionStuffs::GetInstance().IsMarkFindInLastGrab(CENTER_CAM, &grabOffsetX, &grabOffsetY);

						if (found == false)
						{
							if (refind)
							{
								if (motions.MovetoGerberPos(CENTER_CAM, *currPath) == false)//�������� ���������� �̵�.
									throw exception();

								if (CommonMotionStuffs::GetInstance().SingleGrab(CENTER_CAM,false) == false || CWork::GetAbort()) //�׷�����. �۾� �ܺ�����
									throw exception();

								tuple<double, double> refindOffset,grabOffset;
								auto found = CommonMotionStuffs::GetInstance().IsMarkFindInLastGrab(CENTER_CAM, &grabOffsetX, &grabOffsetY);

								if (found == true) //������ǥ���� ������
								{

									STG_XMXY temp = STG_XMXY(currPath->mark_x + grabOffsetX, currPath->mark_y + grabOffsetY);
									if (CommonMotionStuffs::GetInstance().GetOffsetsCurrPos(CENTER_CAM, *currPath, &alignOffset, nullptr, 0, 0) == false) //<-�����ɼ� ���������
										throw exception();

									offsetPool[OffsetType::refind].push_back(CaliPoint(currPath->mark_x, currPath->mark_y, 0, 0, std::get<0>(grabOffset), std::get<1>(grabOffset) ,*currPath));

									/*webMonitor.AddLog(fmt::format("<{}>refind �����߰� ������ǥ���� ã��!!  ORG_ID{} , TGT_ID{} , ������ũ��ġx = {} , ������ũ��ġy = {}, �׷������ɼ�x = {} , �׷��ɻ����� y={} , ��������οɼ�X = {} , ��������οɼ�y={}\r\n",
													currPath->GetFlag(STG_XMXY_RESERVE_FLAG::GLOBAL) ? "global" : "local", currPath->org_id, currPath->tgt_id, currPath->mark_x,currPath->mark_y, grabOffsetX, grabOffsetY, alignOffset.offsetX, alignOffset.offsetY));*/
								}
								else
								{
									if (refindMotion.ProcessRefind(CENTER_CAM, &refindOffset,&grabOffset) == false) //����ǥ ȸ�� �Ŀ� refind�� ��ã������
										throw exception();

									//ã��.
									STG_XMXY temp = STG_XMXY(currPath->mark_x + std::get<0>(refindOffset) + std::get<0>(grabOffset),
															 currPath->mark_y + std::get<1>(refindOffset) + std::get<1>(grabOffset));

									if (CommonMotionStuffs::GetInstance().GetOffsetsCurrPos(CENTER_CAM, *currPath, &alignOffset, nullptr, std::get<0>(refindOffset), std::get<1>(refindOffset)) == false) //<-�����ɼ� ���������
										throw exception();

									offsetPool[OffsetType::refind].push_back(CaliPoint(currPath->mark_x, currPath->mark_y, std::get<0>(refindOffset), std::get<1>(refindOffset), std::get<0>(grabOffset), std::get<1>(grabOffset) , *currPath));

									/*webMonitor.AddLog(fmt::format("<{}>refind�� ã��!!  ORG_ID{} , TGT_ID{} , ������ũ��ġx = {} , ������ũ��ġy = {},�����ε�ɼ�x = {} , �����ε�ɼ�y = {}, �׷������ɼ�x = {} , �׷��ɻ����� y={} , ��������οɼ�X = {} , ��������οɼ�y={}\r\n",
										currPath->GetFlag(STG_XMXY_RESERVE_FLAG::GLOBAL) ? "global" : "local", currPath->org_id, currPath->tgt_id, currPath->mark_x, currPath->mark_y, std::get<0>(refindOffset), std::get<1>(refindOffset), std::get<0>(grabOffset), std::get<1>(grabOffset), alignOffset.offsetX, alignOffset.offsetY));*/

								}
							}
							else
							{
								offsetPool[OffsetType::refind].push_back(CaliPoint(currPath->mark_x, currPath->mark_y,
									estimatedXMXY.mark_x - currPath->mark_x,
									estimatedXMXY.mark_y - currPath->mark_y, 0, 0, *currPath));
								//!!!!!!!!!!!!!���� �������ؼ��� �̿��ؾ��Ұ�� ���⿡�� ó���ϸ� ��.!!!!!!!!!!!!!!
								//�ϴ� ��ã������ �ٷ� ĵ��. 
								//throw exception();
							}
						}
						else
						{
							STG_XMXY combineAddGrabOffset = STG_XMXY(currPath->mark_x + (estimatedXMXY.mark_x - currPath->mark_x) + grabOffsetX,
														currPath->mark_y + (estimatedXMXY.mark_y - currPath->mark_y) + grabOffsetY,
														currPath->tgt_id);

							//CommonMotionStuffs::GetInstance().GetCurrentOffsets(CENTER_CAM, combineAddGrabOffset, alignOffset, expoOffset);
							offsetPool[OffsetType::refind].push_back(CaliPoint(currPath->mark_x, currPath->mark_y,
																				estimatedXMXY.mark_x - currPath->mark_x,
																				estimatedXMXY.mark_y - currPath->mark_y, grabOffsetX, grabOffsetY,*currPath));
						}

						//offsetPool[OffsetType::align].push_back(alignOffset);

						grabMarkPath.erase(currPath);

						if (grabMarkPath.empty())
							return true;

						return false;
					}
				}
				catch (...)
				{
					m_enWorkState = ENG_JWNS::en_error;
					return true;
				}}, (60 * 1000) * grabMarkPath.size());
				SetUIRefresh(true);
			m_enWorkState = complete == true && m_enWorkState != ENG_JWNS::en_error ? ENG_JWNS::en_next : ENG_JWNS::en_error;
		},
		[&]()
		{
			m_enWorkState = IsGrabbedImageCount(m_u8MarkCount, 3000, &CENTER_CAM);
		},
		[&]()
		{
			bool manualFixed = false;
			m_enWorkState = IsSetMarkValidAll(0x00,&manualFixed, &CENTER_CAM);

			try
			{
				if (m_enWorkState == ENG_JWNS::en_next && manualFixed == true)
				for (auto& v : offsetPool[OffsetType::refind]) //���������� �߻��ߴٸ� �ɼ��� �ٽ� �������ش�. 
				{
					auto grab = uvEng_GetGrabUseMark(CENTER_CAM, v.srcFid);
					if (grab == nullptr)
						throw exception();

					v.suboffsetX = grab->move_mm_x;
					v.suboffsetY = grab->move_mm_y;
				}
			}
			catch (...)
			{
				m_enWorkState = ENG_JWNS::en_error;
			}
		},
		[&]()
		{
			int representCount = offsetPool[OffsetType::refind].size();
			CaliPoint expoCali, grabCali;

			if (representCount >= globalMarkCnt) //�۷ι� ��ũī��Ʈ���� Ŀ���Ѵ�. 
			try
			{
				double mark1RefindX = 0, mark1RefindY = 0, mark2RefindX = 0, mark2RefindY = 0; //POOL �ֱ����� ó���ؾ��Ѵ�. 
				double mark1GrabX, mark1GrabY, mark2GrabX, mark2GrabY;
				double mark1SumX, mark1SumY, mark2SumX, mark2SumY;

				mark1RefindX = offsetPool[OffsetType::refind][MARK1].offsetX;
				mark1RefindY = offsetPool[OffsetType::refind][MARK1].offsetY;
				mark2RefindX = offsetPool[OffsetType::refind][MARK2].offsetX;
				mark2RefindY = offsetPool[OffsetType::refind][MARK2].offsetY;

				mark1GrabX = offsetPool[OffsetType::refind][MARK1].suboffsetX;
				mark1GrabY = offsetPool[OffsetType::refind][MARK1].suboffsetY;
				mark2GrabX = offsetPool[OffsetType::refind][MARK2].suboffsetX;
				mark2GrabY = offsetPool[OffsetType::refind][MARK2].suboffsetY;

				mark1SumX = mark1RefindX - mark1GrabX;
				mark1SumY = mark1RefindY - mark1GrabY;
				mark2SumX = mark2RefindX - mark2GrabX;
				mark2SumY = mark2RefindY - mark2GrabY;

				bool needChangeExpoLocation = (fabs(mark1SumX) > refindMotion.thresholdDist || fabs(mark1SumY) > refindMotion.thresholdDist || fabs(mark2SumX) > refindMotion.thresholdDist || fabs(mark2SumY) > refindMotion.thresholdDist);

				auto xShiftGab = needChangeExpoLocation ? ((mark1SumX + mark2SumX) / 2.0f) : 0;
				auto yShiftGab = needChangeExpoLocation ? ((mark1SumY + mark2SumY) / 2.0f) : 0;

				motions.markParams.SetExpoShiftValue(xShiftGab, yShiftGab);
				m_enWorkState = SetExposeStartXY(&xShiftGab, &yShiftGab);

			}
			catch (...)
			{
				m_enWorkState = ENG_JWNS::en_error;
				return;
			}
			m_enWorkState = ENG_JWNS::en_next;
		},
		[&]()
		{
			CaliPoint expoOffset, grabOffset,alignOffset;
			double expoStartOffsetX = 0, expoStartOffsetY = 0;
			motions.markParams.GetExpoShiftValue(expoStartOffsetX, expoStartOffsetY);

			for (auto v : offsetPool[OffsetType::refind])
			{
				grabOffset = v;

				grabOffset.offsetX = Stuffs::CutEpsilon(expoStartOffsetX - (v.offsetX - v.suboffsetX));
				grabOffset.offsetY = Stuffs::CutEpsilon(expoStartOffsetY - (v.offsetY - v.suboffsetY));

				offsetPool[OffsetType::grab].push_back(grabOffset); //�ϴ� grab�ɼ��� �߰�. 

				uvEng_FixMoveOffsetUseMark(CENTER_CAM, v.srcFid, -v.offsetX + (v.suboffsetX), -v.offsetY + (v.suboffsetY), true);

				if (uvEng_GetConfig()->set_align.use_2d_cali_data == false)
					continue;

				if (CommonMotionStuffs::GetInstance().GetOffsetsUseMarkPos(CENTER_CAM, v.srcFid, &alignOffset, nullptr, v.offsetX, v.offsetY) == false)
				{
					m_enWorkState = ENG_JWNS::en_error;
					return;
				}
				offsetPool[OffsetType::align].push_back(alignOffset); //����� �ɼ��� �߰�. 

				if (CommonMotionStuffs::GetInstance().GetOffsetsUseMarkPos(CENTER_CAM, v.srcFid, nullptr, &expoOffset, v.offsetX, v.offsetY) == false)
				{
					m_enWorkState = ENG_JWNS::en_error;
					return;
				}

				offsetPool[OffsetType::expo].push_back(expoOffset);   //�ͽ��� �ɼ��� �߰�. 


				switch (m_stExpoLog.includeAddAlignOffset)
				{
					case 1:
						uvEng_FixMoveOffsetUseMark(CENTER_CAM, v.srcFid, alignOffset.offsetX * -1.0f, alignOffset.offsetY * -1.0f);
					break;

					case 2:
						uvEng_FixMoveOffsetUseMark(CENTER_CAM, v.srcFid, (alignOffset.offsetX + (expoOffset.offsetX - alignOffset.offsetX)) * -1.0f,
																		 (alignOffset.offsetY + (expoOffset.offsetY - alignOffset.offsetY)) * -1.0f);
					break;
				}
			}

			m_enWorkState = ENG_JWNS::en_next;
		},
		[&]()
		{
			motions.SetAlignOffsetPool(offsetPool);
			m_enWorkState = CameraSetCamMode(ENG_VCCM::en_none);
		},
		[&]()
		{
			m_enWorkState = IsExposeStartXY();
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
			SetProcessComplete();
		},
	};

	m_u8StepTotal = stepWork.size();

	try
	{
		stepWork[m_u8StepIt]();

	}
	catch (const std::exception&)
	{

	}
}

//�´��ö��� 2ķ
void CWorkMarkTest::DoAlignOnthefly2cam()
{
	auto& motions = GlobalVariables::GetInstance()->GetAlignMotion();
	switch (m_u8StepIt)/* �۾� �ܰ� ���� ���� ó�� */
	{
	case 0x01:
	{
		m_enWorkState = SetExposeReady(TRUE, TRUE, TRUE, 1);
		motions.SetAlignComplete(false);
		if (m_enWorkState == ENG_JWNS::en_next)
			GlobalVariables::GetInstance()->GetAlignMotion().SetFiducialPool();
	}
	break;	    /* �뱤 ������ �������� ���� Ȯ�� */
	case 0x02:
	{
		m_enWorkState = IsLoadedGerberCheck();
	}
	break;	/* �Ź��� ����Ǿ���, Mark�� �����ϴ��� Ȯ�� */
	
	case 0x03: 
	{
		bundleResult[0] = false; bundleResult[1] = false;doNextJob = false;

		bundleAction[0] = std::thread([&]()
		{
			ENG_JWNS res = ENG_JWNS::en_wait;
			if (SetAlignMovingInit() == ENG_JWNS::en_next)
			{
				doNextJob = true;cv.notify_one();//���⼭ bundleAction[1]�� �����ؾ��� flag 
				res = ENG_JWNS::en_wait;
				while (res != ENG_JWNS::en_error && res != ENG_JWNS::en_next)
				{
					res = IsAlignMovedInit();Sleep(100);
				}
			}
			else
			{	
				doNextJob = true; cv.notify_one();//���⼭ bundleAction[1]�� �����ؾ��� flag 
			}

			bundleResult[0] = res == ENG_JWNS::en_next ? true : false;

		});
	
		bundleAction[1] = std::thread([&]()
			{
				ENG_JWNS res = ENG_JWNS::en_next;
				std::unique_lock<std::mutex> lock(mtx);
				cv.wait(lock, [&] { return doNextJob; });

				if(SetTrigEnable(FALSE) == ENG_JWNS::en_next &&
					IsTrigEnabled(FALSE) == ENG_JWNS::en_next && 
					SetTrigPosCalcSaved() == ENG_JWNS::en_next &&
					SetTrigRegistGlobal() == ENG_JWNS::en_next &&
					IsTrigRegistGlobal() == ENG_JWNS::en_next &&
					SetAlignMeasMode() == ENG_JWNS::en_next)
				{
					res = ENG_JWNS::en_wait;
					while (res != ENG_JWNS::en_error && res != ENG_JWNS::en_next)
					{
						res = IsAlignMeasMode(); Sleep(100);
					}
					if(res == ENG_JWNS::en_next)SetActionRequest(ENG_RIJA::clearMarkData);
					bundleResult[1] = res == ENG_JWNS::en_next ? true : false;
				}
			});

		m_enWorkState = ENG_JWNS::en_next;
	}	
	break;	/* Trigger Event - ��Ȱ��ȭ ���� */

	case 0x04: m_enWorkState = ENG_JWNS::en_next; break;	/* Trigger Event - ��Ȱ��ȭ Ȯ��  */
	case 0x05: m_enWorkState = ENG_JWNS::en_next; break;	/* Stage X/Y, Camera 1/2 - Align (Global) ���� ��ġ�� �̵� */
	case 0x06: m_enWorkState = ENG_JWNS::en_next; break;	/* Trigger �߻� ��ġ ��� �� �ӽ� ���� */
	case 0x07: m_enWorkState = ENG_JWNS::en_next; break;	/* Stage X/Y, Camera 1/2 - Align (Global) ���� ��ġ ���� ���� */
	case 0x08: m_enWorkState = ENG_JWNS::en_next; break;	/* Trigger �߻� ��ġ - Ʈ���� ���忡 Global Mark ��ġ ��� */
	case 0x09: m_enWorkState = ENG_JWNS::en_next; break;	/* Trigger �߻� ��ġ ��� Ȯ�� */
	case 0x0a: m_enWorkState = ENG_JWNS::en_next; break;
	case 0x0b: m_enWorkState = ENG_JWNS::en_next; break;

	case 0x0c:
	{
		bundleAction[1].join();
		bundleAction[0].join();

		if (bundleResult[0] == false || bundleResult[1] == false)
		{
			m_enWorkState = ENG_JWNS::en_error;
			return;
		}
		m_enWorkState = SetAlignMovingGlobal();
	}
	break;	/* Global Mark 4 ���� ��ġ Ȯ�� */
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
		m_enWorkState = IsSetMarkValidAll(0x00);
	}
	break;
	case 0x1a:
	{
		m_enWorkState = SetAlignMarkRegist();
		//m_enWorkState = ENG_JWNS::en_next;
	}
	break;

	case 0x1b: 
	{
		m_enWorkState = IsAlignMarkRegist();
		if (m_enWorkState == ENG_JWNS::en_next)
			motions.SetAlignComplete(true);
	}
	break;
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

	if (CWork::GetAbort())
	{
		CWork::EndWork();
		return;
	}

	uvEng_UpdateJobWorkTime(u64JobTime);

	if (ENG_JWNS::en_error == m_enWorkState)
	{
		TCHAR tzMesg[128] = { NULL };
		swprintf_s(tzMesg, 128, L"Align Test <Error Step It = 0x%02x>", m_u8StepIt);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);

		SaveExpoResult(0x00);
		
		m_u8StepIt = 0x00;
		m_enWorkState = ENG_JWNS::en_error;
	}
	else if (ENG_JWNS::en_next == m_enWorkState)
	{
		CWork::CalcStepRate();
		m_u8StepIt++;
		if (m_u8StepTotal == m_u8StepIt)
		{
			SaveExpoResult(0x01);
			
			m_u8StepIt = 0x00;

			if (++m_u32ExpoCount == m_stExpoLog.expo_count)
			{
				m_u8StepIt = 0x00;
				m_enWorkState = ENG_JWNS::en_comp;
				CWork::EndWork();
			}
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
		

		Sleep(3000);
		//m_enWorkState = ENG_JWNS::en_error;

		if (++m_u32ExpoCount != m_stExpoLog.expo_count)
		{
			m_enWorkState = ENG_JWNS::en_next;
		}
		else
				{
			m_enWorkState = ENG_JWNS::en_error;
				}
			}
	else if (ENG_JWNS::en_next == m_enWorkState)
			{
				/*Auto Mdoe�� �뱤 ���ᰡ �Ǹ� Philhmil�� �ϷẸ��*/
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






VOID CWorkMarkTest::WriteWebLogForExpoResult(UINT8 state)
{
	//UINT8 i;
	TCHAR tempStr[1024] = { NULL }, tzFile[MAX_PATH_LEN] = { NULL }, tzState[2][8] = { L"FAIL", L"SUCC" };
	TCHAR tzDrv[8] = { NULL };

	SYSTEMTIME stTm = { NULL };
	MEMORYSTATUSEX stMem = { NULL };
	LPG_ACGR pstMark = NULL;
	bool isLocalSelRecipe = uvEng_JobRecipe_WhatLastSelectIsLocal();
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe(isLocalSelRecipe);
	CUniToChar csCnv1, csCnv2;

	CUniToChar	csCnv;
	LPG_RAAF pstAlignRecipe = uvEng_Mark_GetAlignRecipeName(csCnv.Ansi2Uni(pstRecipe->align_recipe));
	LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstRecipe->expo_recipe));
	auto& webMonitor = GlobalVariables::GetInstance()->GetWebMonitor();


	TCHAR title[1024] = { NULL };

	vector<wstring> temps;
	GetLocalTime(&stTm);
	/* �߻� �ð� */
	swprintf_s(tempStr, 1024, L"time = %02d:%02d:%02d\n", stTm.wHour, stTm.wMinute, stTm.wSecond);
	temps.push_back(wstring(tempStr));

	UINT64 u64JobTime = uvEng_GetJobWorkTime();
	swprintf_s(tempStr, 1024, L"tact = %um%02us\n", uvCmn_GetTimeToType(u64JobTime, 0x01));
	temps.push_back(wstring(tempStr));
	swprintf_s(tempStr, 1024, L"succ = %s\n", tzState[state]);
	temps.push_back(wstring(tempStr));
	swprintf_s(tempStr, 1024, L"gerber_name = %S\n", pstRecipe->gerber_name);
	temps.push_back(wstring(tempStr));
	swprintf_s(tempStr, 1024, L"material_thick(um) = %d\n", pstRecipe->material_thick);
	temps.push_back(wstring(tempStr));
	swprintf_s(tempStr, 1024, L"expo_eneray(mj) = %.1f\n", pstRecipe->expo_energy);
	temps.push_back(wstring(tempStr));

	LPG_GMLV pstMarkDiff = &uvEng_GetConfig()->mark_diff;
	swprintf_s(tempStr, 1024, L"horz_dist_13(mm) = %.4f\n", pstMarkDiff->result[0].diff * 100.0f);
	temps.push_back(wstring(tempStr));
	swprintf_s(tempStr, 1024, L"horz_dist_24(mm) = %.4f\n", pstMarkDiff->result[1].diff * 100.0f);
	temps.push_back(wstring(tempStr));
	swprintf_s(tempStr, 1024, L"vert_dist_12(mm) = %.4f\n", pstMarkDiff->result[2].diff * 100.0f);
	temps.push_back(wstring(tempStr));
	swprintf_s(tempStr, 1024, L"vert_dist_34(mm) = %.4f\n", pstMarkDiff->result[3].diff * 100.0f);
	temps.push_back(wstring(tempStr));
	swprintf_s(tempStr, 1024, L"diag_dist_14(mm) = %.4f\n", pstMarkDiff->result[4].diff * 100.0f);
	temps.push_back(wstring(tempStr));
	swprintf_s(tempStr, 1024, L"diag_dist_23(mm) = %.4f\n", pstMarkDiff->result[5].diff * 100.0f);
	temps.push_back(wstring(tempStr));

	if ((ENG_AMOS)pstAlignRecipe->align_motion == ENG_AMOS::en_static_3cam)
	{

		int cCam = uvEng_GetConfig()->set_align.centerCamIdx;

		/* ����� ��ũ �˻� ��� �� ���� */

		for (int i = 0; i < 4; i++)
		{
			pstMark = uvEng_Camera_GetGrabbedMark(cCam, i);
			
			swprintf_s(tempStr, 1024, L"score_%d = %.3f\n", i + 1, pstMark ? pstMark->score_rate : 0 );
			temps.push_back(wstring(tempStr));
			swprintf_s(tempStr, 1024, L"scale_%d = %.3f\n", i + 1, pstMark ? pstMark->scale_rate : 0 );
			temps.push_back(wstring(tempStr));
			swprintf_s(tempStr, 1024, L"mark_move_x%d(mm) = %.4f\n", i + 1, pstMark ? pstMark->move_mm_x : 0 );
			temps.push_back(wstring(tempStr));
			swprintf_s(tempStr, 1024, L"mark_move_y%d(mm) = %.4f\n", i + 1, pstMark ? pstMark->move_mm_y : 0);
			temps.push_back(wstring(tempStr));
		}
	}
	else
	{
		for (int i = 0; i < 2; i++)
		{
			/* ����� ��ũ �˻� ��� �� ���� */
			pstMark = uvEng_Camera_GetGrabbedMark(i + 1, 0);
			
			swprintf_s(tempStr, 1024, L"score_1 = %.3f\n", pstMark ? pstMark->score_rate : 0);
			temps.push_back(wstring(tempStr));
			swprintf_s(tempStr, 1024, L"scale_1 = %.3f\n", pstMark ? pstMark->scale_rate : 0);
			temps.push_back(wstring(tempStr));
			swprintf_s(tempStr, 1024, L"mark_move_x1(mm) = %.4f\n", pstMark ?  pstMark->move_mm_x : 0);
			temps.push_back(wstring(tempStr));
			swprintf_s(tempStr, 1024, L"mark_move_y1(mm) = %.4f\n", pstMark ?  pstMark->move_mm_y : 0);
			temps.push_back(wstring(tempStr));
			
			pstMark = uvEng_Camera_GetGrabbedMark(i + 1, 1);
			
			swprintf_s(tempStr, 1024, L"score_2 = %.3f\n", pstMark ?  pstMark->score_rate : 0);
			temps.push_back(wstring(tempStr));
			swprintf_s(tempStr, 1024, L"scale_2 = %.3f\n", pstMark ?  pstMark->scale_rate : 0);
			temps.push_back(wstring(tempStr));
			swprintf_s(tempStr, 1024, L"mark_move_x2(mm) = %.4f\n", pstMark ?  pstMark->move_mm_x : 0);
			temps.push_back(wstring(tempStr));
			swprintf_s(tempStr, 1024, L"mark_move_y2(mm) = %.4f\n", pstMark ?  pstMark->move_mm_y : 0);
			temps.push_back(wstring(tempStr));
			
		}
	}

	USES_CONVERSION;
	swprintf_s(tempStr, 1024, L"led_recipe = %s\n", A2T(pstExpoRecipe->power_name));
	temps.push_back(wstring(tempStr));

	auto& measureFlat = uvEng_GetConfig()->measure_flat;
	if (measureFlat.u8UseThickCheck)
	{
		auto mean = measureFlat.GetThickMeasureMean();

		DOUBLE RealThick;
		DOUBLE LDSToThickOffset = 0;
		DOUBLE dmater = pstRecipe->material_thick / 1000.0f;
		LDSToThickOffset = uvEng_GetConfig()->measure_flat.dOffsetZPOS;

		RealThick = mean + dmater + LDSToThickOffset;

		swprintf_s(tempStr, 1024, L"read_thick(mm) = %.3f\n", RealThick);
		temps.push_back(wstring(tempStr));


		/*swprintf_s(tempStr, 1024, L"LDS_BaseHeight(um) = %d\n", pstRecipe->ldsBaseHeight);
		temps.push_back(wstring(tempStr));*/

		swprintf_s(tempStr, 1024, L"LDS_Threshold(um) = %d\n", pstRecipe->ldsThreshold);
		temps.push_back(wstring(tempStr));
	}
	else
	{
		swprintf_s(tempStr, 1024, L"read_thick(mm) = x\n");
		temps.push_back(wstring(tempStr));

		swprintf_s(tempStr, 1024, L"LDS_Threshold(um) = x\n");
		temps.push_back(wstring(tempStr));
	}

	Headoffset offset;
	bool getOffset = uvEng_GetConfig()->headOffsets.GetOffsets(pstExpoRecipe->headOffset, offset);
	swprintf_s(tempStr, 1024, L"materialType = %s\n", (getOffset ? A2T(offset.offsetName) : L"-"));
	temps.push_back(wstring(tempStr));

	std::wstring result = std::accumulate(temps.begin(), temps.end(), std::wstring(L""));
	webMonitor.UpdateLog(string(result.begin(), result.end()));
}

/*
 desc : �뱤 �Ϸ� �� ���� ���� ����
 parm : state	- [in]  0x00: �۾� ����, 0x01: �۾� ����
 retn : None
*/
VOID CWorkMarkTest::SaveExpoResult(UINT8 state)
{
	//UINT8 i;
	TCHAR tzResult[1024] = { NULL }, tzFile[MAX_PATH_LEN] = { NULL }, tzState[2][8] = { L"FAIL", L"SUCC" };
	TCHAR tzDrv[8] = { NULL };

	SYSTEMTIME stTm = { NULL };
	MEMORYSTATUSEX stMem = { NULL };
	LPG_ACGR pstMark = NULL;
	bool isLocalSelRecipe = uvEng_JobRecipe_WhatLastSelectIsLocal();
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe(isLocalSelRecipe);
	CUniToChar csCnv1, csCnv2;

	CUniToChar	csCnv;
	LPG_RAAF pstAlignRecipe = uvEng_Mark_GetAlignRecipeName(csCnv.Ansi2Uni(pstRecipe->align_recipe));
	LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstRecipe->expo_recipe));

	/*������ ���� ��������*/



	/* ���� ��ǻ�� ��¥�� ���ϸ����� ���� */
	GetLocalTime(&stTm);
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\logs\\expo\\%04d-%02d-%02d MarkTest.csv",
		g_tzWorkDir, stTm.wYear, stTm.wMonth, stTm.wDay);

	/* ���࿡ ������ �ϴ� ������ ������, �ش� ���� ���� ��, Ÿ��Ʋ (Field) �߰� */
	if (!uvCmn_FindFile(tzFile))
	{
		wcscpy_s(tzResult, 1024,
			L"time,tack,succ,"
			L"gerber_name,material_thick(um),expo_eneray(mj),"
			L"horz_dist_13(mm),horz_dist_24(mm),vert_dist_12(mm),vert_dist_34(mm),"
			L"diag_dist_14(mm),diag_dist_23(mm),"
			L"score_1,scale_1,mark_move_x1(mm),mark_move_y1(mm),"
			L"score_2,scale_2,mark_move_x2(mm),mark_move_y2(mm),"
			L"score_3,scale_3,mark_move_x3(mm),mark_move_y3(mm),"
			L"score_4,scale_4,mark_move_x4(mm),mark_move_y4(mm),"
			L"led_recipe,"
			L"read_thick(mm),LDS_Threshold(um),materialType\n");

		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x00);


	}

	/* �߻� �ð� */
	swprintf_s(tzResult, 1024, L"%02d:%02d:%02d,", stTm.wHour, stTm.wMinute, stTm.wSecond);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);

	/*ExpoLog ���*/
	memcpy(m_stExpoLog.data, tzResult, 40);

	UINT64 u64JobTime = uvEng_GetJobWorkTime();
	swprintf_s(tzResult, 1024, L"%um%02us,%s,%S,%dum,%.1fmj,",
		uvCmn_GetTimeToType(u64JobTime, 0x01),
		uvCmn_GetTimeToType(u64JobTime, 0x02),
		tzState[state],
		pstRecipe->gerber_name,
		pstRecipe->material_thick,
		pstRecipe->expo_energy);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);

	/*ExpoLog ���*/
	m_stExpoLog.expo_time = uvEng_GetJobWorkTime();
	m_stExpoLog.expo_succ = state;
	m_stExpoLog.real_scale = pstExpoRecipe->real_scale_range;
	m_stExpoLog.real_rotaion = pstExpoRecipe->real_rotaion_range;


	/* ��ũ ���� 6 �� ���� ���� ���� �� �� ��ü �뱤�ϴµ� �ҿ�� �ð� ���� */
	LPG_GMLV pstMarkDiff = &uvEng_GetConfig()->mark_diff;
	swprintf_s(tzResult, 1024, L"%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,",
		pstMarkDiff->result[0].diff * 100.0f, pstMarkDiff->result[1].diff * 100.0f, pstMarkDiff->result[2].diff * 100.0f,
		pstMarkDiff->result[3].diff * 100.0f, pstMarkDiff->result[4].diff * 100.0f, pstMarkDiff->result[5].diff * 100.0f);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);

	m_stExpoLog.global_dist[0] = pstMarkDiff->result[0].diff * 100.0f;
	m_stExpoLog.global_dist[1] = pstMarkDiff->result[1].diff * 100.0f;
	m_stExpoLog.global_dist[2] = pstMarkDiff->result[2].diff * 100.0f;
	m_stExpoLog.global_dist[3] = pstMarkDiff->result[3].diff * 100.0f;
	m_stExpoLog.global_dist[4] = pstMarkDiff->result[4].diff * 100.0f;
	m_stExpoLog.global_dist[5] = pstMarkDiff->result[5].diff * 100.0f;


	if ((ENG_AMOS)pstAlignRecipe->align_motion == ENG_AMOS::en_static_3cam)
	{

		int cCam = uvEng_GetConfig()->set_align.centerCamIdx;
		/* ����� ��ũ �˻� ��� �� ���� */


		for (int i = 0; i < 4; i++)
		{
			pstMark = uvEng_Camera_GetGrabbedMark(cCam, i);
			if (pstMark)
			{
				swprintf_s(tzResult, 1024, L"%.3f,%.3f,%.4f,%.4f,",
					pstMark->score_rate, pstMark->scale_rate,
					pstMark->move_mm_x, pstMark->move_mm_y);
				uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
			}
			else
			{
				uvCmn_SaveTxtFileW(L"-,-,-,-,", (UINT32)wcslen(L"-,-,-,-,"), tzFile, 0x01);
			}
		}
	}
	else
	{

		for (int i = 0; i < 2; i++)
		{
			/* ����� ��ũ �˻� ��� �� ���� */
			pstMark = uvEng_Camera_GetGrabbedMark(i + 1, 0x00);
			if (pstMark)
			{
				swprintf_s(tzResult, 1024, L"%.3f,%.3f,%.4f,%.4f,",
					pstMark->score_rate, pstMark->scale_rate,
					pstMark->move_mm_x, pstMark->move_mm_y);
				uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);

			}
			else
				uvCmn_SaveTxtFileW(L"-,-,-,-,", (UINT32)wcslen(L"-,-,-,-,"), tzFile, 0x01);

			pstMark = uvEng_Camera_GetGrabbedMark(i + 1, 0x01);
			if (pstMark)
			{
				swprintf_s(tzResult, 1024, L"%.3f,%.3f,%.4f,%.4f,",
					pstMark->score_rate, pstMark->scale_rate,
					pstMark->move_mm_x, pstMark->move_mm_y);
				uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);

			}
			else
				uvCmn_SaveTxtFileW(L"-,-,-,-,", (UINT32)wcslen(L"-,-,-,-,"), tzFile, 0x01);
		}


	}



	swprintf_s(tzResult, 1024, L"%S,", pstExpoRecipe->power_name);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);

	auto& measureFlat = uvEng_GetConfig()->measure_flat;
	auto mean = measureFlat.GetThickMeasureMean();
	if (measureFlat.u8UseThickCheck)
	{
		DOUBLE RealThick;
		DOUBLE LDSToThickOffset = 0;
		DOUBLE dmater = pstRecipe->material_thick / 1000.0f;

		LDSToThickOffset = uvEng_GetConfig()->measure_flat.dOffsetZPOS;

		RealThick = mean + dmater + LDSToThickOffset;

		swprintf_s(tzResult, 1024, L"%.3f,", RealThick);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);


		//����� �߰��Ȱ� 2�� �־�����ϳ�.

		//swprintf_s(tzResult, 1024, L"%d,", pstRecipe->ldsBaseHeight);
		//uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);

		swprintf_s(tzResult, 1024, L"%d,", pstRecipe->ldsThreshold);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	else
	{
		swprintf_s(tzResult, 1024, L"-,-,");
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}




	Headoffset offset;

	//	LPG_REAF pstRecipeExpo = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstRecipe->expo_recipe));
	bool getOffset = uvEng_GetConfig()->headOffsets.GetOffsets(pstExpoRecipe->headOffset, offset);

	USES_CONVERSION;

	swprintf_s(tzResult, 1024, L"%s,", getOffset ? A2T(offset.offsetName) : L"-");
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	
	strcpy_s(m_stExpoLog.gerber_name, MAX_GERBER_NAME, pstRecipe->gerber_name);
	m_stExpoLog.material_thick = pstRecipe->material_thick;
	m_stExpoLog.expo_energy = pstRecipe->expo_energy;

	m_stExpoLog.align_type = pstAlignRecipe->align_type;
#ifdef USE_ALIGNMOTION
	m_stExpoLog.align_motion = pstAlignRecipe->align_motion;
#endif
	m_stExpoLog.headOffset = pstExpoRecipe->headOffset;
	m_stExpoLog.mark_type = pstAlignRecipe->mark_type;
	m_stExpoLog.lamp_type = pstAlignRecipe->lamp_type;
	m_stExpoLog.gain_level[0] = pstAlignRecipe->gain_level[0];
	m_stExpoLog.gain_level[1] = pstAlignRecipe->gain_level[1];

	m_stExpoLog.led_duty_cycle = pstExpoRecipe->led_duty_cycle;
	//sprintf_s(m_stExpoLog.power_name, LED_POWER_NAME_LENGTH, pstExpoRecipe->power_name);
	strcpy_s(m_stExpoLog.power_name, LED_POWER_NAME_LENGTH, pstExpoRecipe->power_name);

	/* �������� ������ ���� �������� �Ѿ���� �ϱ� ���� */
	uvCmn_SaveTxtFileW(L"\n", (UINT32)wcslen(L"\n"), tzFile, 0x01);


	WriteWebLogForExpoResult(state);
}

