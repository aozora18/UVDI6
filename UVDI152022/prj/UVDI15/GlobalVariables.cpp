#pragma once
//이하는 전부 인라인 클래스임.

using namespace std;

#include "pch.h"
#include "GlobalVariables.h"

bool ConditionWrapper::WaitFor(std::chrono::milliseconds timeout)
{
	std::unique_lock<std::mutex> lock(mtx);
	auto result = cv.wait_for(lock, timeout, [this] { return isConditionMet; });
	return result;
}

void ConditionWrapper::Notify(bool res)
{
	std::lock_guard<std::mutex> lock(mtx);
	isConditionMet = res;
	cv.notify_one();
}

void ConditionWrapper::Reset()
{
	std::lock_guard<std::mutex> lock(mtx);
	isConditionMet = false;
}

void ThreadManager::addThread(const std::string& key, std::atomic<bool>& stopFlag, std::function<void()> callback)
{
	if (threads_.find(key) != threads_.end())
		return;

	std::lock_guard<std::mutex> lock(mutex_);
	threads_.emplace(key, 
						make_tuple(std::make_unique<std::thread>(callback),
								   std::ref(stopFlag)));
}

void ThreadManager::removeThread(const std::string& key)
{
	std::lock_guard<std::mutex> lock(mutex_);
	auto it = threads_.find(key);
	if (it != threads_.end()) 
	{
		auto& thread = std::get<0>(it->second);
		auto& stopFlag = std::get<1>(it->second);

		if (thread->joinable())
		{
			stopFlag.store(true);
			thread ->join();
		}
		threads_.erase(it);
	}
}

void ThreadManager::waitForAllThreads()
{
	
	for (auto& pair : threads_) 
	{
		removeThread(pair.first);
	}
	threads_.clear();
}

bool ThreadManager::isThreadRunning(const std::string& key)
{
	std::lock_guard<std::mutex> lock(mutex_);
	auto it = threads_.find(key);
	return it != threads_.end() && std::get<0>(it->second)->joinable();
}


 
double CaliCalc::LimittoMicro(double val)
{
	return round(val * 1000.0) / 1000.0; //마이크로 단위 이하는 필요가 없음. 
}

void CaliCalc::SortPos(std::vector<CaliPoint>& dataList)
{
	std::sort(dataList.begin(), dataList.end(), [](const CaliPoint& a, const CaliPoint& b)
		{
			return a.y == b.y ? a.x < b.x : a.y < b.y;
		});
}


CaliPoint CaliCalc::CalculateAverageOffset(const std::vector<CaliPoint>& nearbyPoints)
{
	double sumOffsetX = 0.0, sumOffsetY = 0.0;

	for (const auto& point : nearbyPoints)
	{
		sumOffsetX += point.offsetX;
		sumOffsetY += point.offsetY;
	}
	return { 0, 0,  LimittoMicro(sumOffsetX / nearbyPoints.size()) ,
					LimittoMicro(sumOffsetY / nearbyPoints.size()) };
}

void CaliCalc::LoadCaliData(LPG_CIEA cfg)
{

	

	auto tokenize = [&](string str, regex re) -> vector<double>
		{
			std::sregex_token_iterator it{ str.begin(), str.end(), re, -1 };
			std::vector<string> tokenized{ it, {} };

			tokenized.erase(std::remove_if(tokenized.begin(),
				tokenized.end(), [](std::string const& s)
				{
					return s.size() == 0;
				}),
				tokenized.end());

			std::vector<double> doubleVector;
			std::transform(tokenized.begin(), tokenized.end(), std::back_inserter(doubleVector),
				[](const std::string& str) { return std::stod(str); });

			return doubleVector;
		};


	auto StackVector = [&](TCHAR* name, vector<CaliPoint>& dataList , vector<double>& featureStored)
		{
			const int XCoord = 0, YCoord = 1, OffsetX = 2, OffsetY = 3;
			const int DATACOUNT = 4;
			const std::regex re(R"([\s|,]+)");
			
			dataList.clear();
			featureStored.clear();

			std::basic_string<TCHAR> str(name);
			if (str.empty()) return;

			try
			{
				std::ifstream file(name);

				for (std::string line; std::getline(file, line);)
				{
					const std::vector<double> tokens = tokenize(line, re);
					if (tokens.empty()) continue;

					if (tokens.size() != DATACOUNT)
						std::copy(tokens.begin(), tokens.end(), std::back_inserter(featureStored));
					else
						dataList.push_back(CaliPoint(tokens[XCoord], tokens[YCoord], tokens[OffsetX], tokens[OffsetY]));
				}
			}
			catch (exception e)
			{
				int debug = 0;
			}

		};


	auto loadSeq = [&](LPG_CIEA cfg)
		{		
			int camCount = cfg->set_cams.acam_count;
			for (int i = 0; i < camCount; i++) 
			{
				try
				{
					TCHAR* name = cfg->file_dat.staticAcamAlignCali[i];
					StackVector(name, caliDataMap[i + 1][CaliTableType::align], calidataFeature[i + 1][CaliTableType::align]);
					if (caliDataMap[i + 1][CaliTableType::align].size() != 0)
						SortPos(caliDataMap[i+1][CaliTableType::align]);

					name = cfg->file_dat.staticAcamExpoCali[i];
					StackVector(name, caliDataMap[i + 1][CaliTableType::expo], calidataFeature[i + 1][CaliTableType::align]);
					if (caliDataMap[i + 1][CaliTableType::expo].size() != 0)
						SortPos(caliDataMap[i+1][CaliTableType::expo]);
				}
				catch (...)
				{
					caliInfoLoadComplete = false;
					return;
				}
			}
			caliInfoLoadComplete = true;
		};

	if (caliInfoLoadComplete == false)
		std::thread([=]() {loadSeq(cfg); }).detach();
}



CaliPoint CaliCalc::Estimate(vector<CaliPoint>& points, double x, double y)
{
#undef max
	double closestDistance = 99999999;
	double secondClosestDistance = 99999999;

	CaliPoint closestPoint{};
	CaliPoint secondClosestPoint{};

	bool fixX = x == -1 ? true : false;
	bool fixY = y == -1 ? true : false;

	for (const auto& point : points)
	{
		x = fixX && x != point.x ? point.x : x; //소팅축 고정
		y = fixY && y != point.y ? point.y : y;

		double currentDistance = std::sqrt(std::pow(point.x - x, 2) + std::pow(point.y - y, 2));

		if (currentDistance < closestDistance)
		{
			secondClosestDistance = closestDistance;
			secondClosestPoint = closestPoint;

			closestDistance = currentDistance;
			closestPoint = point;
		}
		else if (currentDistance < secondClosestDistance)
		{
			secondClosestDistance = currentDistance;
			secondClosestPoint = point;
		}
	}
	CaliPoint weightedAverageOffset;

	const double LIMIT = 0.0001;
	if (closestDistance <= LIMIT)
	{
		weightedAverageOffset.x = x;
		weightedAverageOffset.y = y;
		weightedAverageOffset.offsetX = closestPoint.offsetX;
		weightedAverageOffset.offsetY = closestPoint.offsetY;
	}
	else
	{
		double totalInverseDistance = (1 / closestDistance) + (1 / secondClosestDistance);
		double weightClosest = (1 / closestDistance) / totalInverseDistance;
		double weightSecondClosest = (1 / secondClosestDistance) / totalInverseDistance;

		weightedAverageOffset.x = x;
		weightedAverageOffset.y = y;
		weightedAverageOffset.offsetX = (closestPoint.offsetX * weightClosest) + (secondClosestPoint.offsetX * weightSecondClosest);
		weightedAverageOffset.offsetY = (closestPoint.offsetY * weightClosest) + (secondClosestPoint.offsetY * weightSecondClosest);
	}
	

	return weightedAverageOffset;
}



CaliPoint CaliCalc::EstimateExpoOffset(double gbrX, double gbrY)
{
	TCHAR tzMsg[256] = { NULL };
	

	const int STAGE_CALI_INDEX = 3;
	auto& expoData = caliDataMap[STAGE_CALI_INDEX][CaliTableType::expo];
	CaliPoint weightedAverageOffset = Estimate(expoData, gbrX, gbrY);

	swprintf_s(tzMsg, 256, L"EstimateExpoOffset : X =%.4f Y = %.4f , offset x = %.4f , y = %.4f", gbrX, gbrY, weightedAverageOffset.offsetX, weightedAverageOffset.offsetY);
	LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
	return weightedAverageOffset;
}


CaliPoint CaliCalc::EstimateAlignOffset(int camIdx, double stageX = 0, double stageY = 0, double camX = -1)
{

	const int STAGE_CALI_INDEX = 3;
	auto& stageCaliData = caliDataMap[STAGE_CALI_INDEX][CaliTableType::align];
	auto& camCaliData = caliDataMap[camIdx][CaliTableType::align];

	CaliPoint stageOffset = Estimate(stageCaliData, stageX, stageY);
	CaliPoint camOffset = camIdx != STAGE_CALI_INDEX ? Estimate(camCaliData, camX, -1) : CaliPoint();

	CaliPoint finalv = stageOffset + camOffset;
	TCHAR tzMsg[256] = { NULL };
	swprintf_s(tzMsg, 256, L"EstimateAlignOffset stagePos : X =%.4f Y = %.4f , offset x = %.4f , y = %.4f" , stageX, stageY, finalv.offsetX, finalv.offsetY);
	LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);

	return finalv;
}

 

void AlignMotion::Destroy()
{
	lock_guard<mutex> lock(*motionMutex);
	cancelFlag.store(true);
	axises.clear();
}


CaliPoint AlignMotion::EstimateExpoOffset(double gbrX, double gbrY)
{
	return caliCalcInst.EstimateExpoOffset(gbrX, gbrY);
}

CaliPoint AlignMotion::EstimateAlignOffset(int camIdx, double stageX, double stageY, double camX)
{
	return caliCalcInst.EstimateAlignOffset(camIdx, stageX, stageY, camX);
}

 
void AlignMotion::Update()
{
	const int updateDelay = 1000;
	while (cancelFlag.load() == false)
	{
		Refresh();
		this_thread::sleep_for(chrono::milliseconds(updateDelay));
	}
}
 

void AlignMotion::Refresh() //바로 갱신이 필요하면 요거 다이렉트 호출 보통은 스레드에서 호출
{
	lock_guard<mutex> lock(*motionMutex);
	for (auto i = axises.begin(); i != axises.end(); i++)
		for (auto j = i->second.begin(); j != i->second.end(); j++)
		{
			if (j->second.stateCallback != nullptr)
				j->second.onError = j->second.stateCallback();

			if (j->second.onError == false && j->second.posCallback != nullptr)
				j->second.currPos = j->second.posCallback();
		}
}

void AlignMotion::LoadCaliData(LPG_CIEA cfg)
{
	caliCalcInst.LoadCaliData(cfg);
}

	bool AlignMotion::GetNearFid(STG_XMXY currentPos, SearchFlag flag, vector<STG_XMXY> skipList, STG_XMXY& findFid)
	{
		std::vector<STG_XMXY> temp = (flag == SearchFlag::global ? status.markList[ENG_AMTF::en_global] :
			flag == SearchFlag::local ? status.markList[ENG_AMTF::en_local] : vector<STG_XMXY>());

		if (flag == SearchFlag::all)
		{
			temp.insert(temp.end(), status.markList[ENG_AMTF::en_global].begin(), status.markList[ENG_AMTF::en_global].end());
			temp.insert(temp.end(), status.markList[ENG_AMTF::en_local].begin(), status.markList[ENG_AMTF::en_local].end());
		}

		for (const auto& item : skipList)
		{
			auto it = std::find_if(temp.begin(), temp.end(), [&](const STG_XMXY& c) { return c.org_id == item.org_id && c.reserve == item.reserve; });
			if (it != temp.end())
				temp.erase(it);
		}

		auto FindNear = [&](vector<STG_XMXY> posList, STG_XMXY pos) -> int
			{
				double minDist = std::numeric_limits<double>::max();
				int idx = -1;

				for (int i = 0; i < posList.size(); i++)
				{
					int dx = posList[i].mark_x - pos.mark_x;
					int dy = posList[i].mark_y - pos.mark_y;

					double dist = std::sqrt(dx * dx + dy * dy);
					if (dist < minDist)
					{
						minDist = dist; idx = i;
					}
				}
				return idx;
			};

		auto findIdx = FindNear(temp, currentPos);
		if (findIdx != -1)
		{
			findFid = temp[findIdx];
			return true;
		}
		return false;
	}

	bool AlignMotion::isArrive(string drive, string axis, double dest, float threshold)
	{
		auto res = abs(axises[drive][axis].currPos - dest) < threshold;
		return res;
	}

	bool AlignMotion::NowOnMoving()
	{

		if (pstCfg->IsRunDemo()) return false;

		LPG_MDSM pstShMC2 = uvEng_ShMem_GetMC2();

		if (pstShMC2->IsDriveBusy(UINT8(ENG_MMDI::en_stage_x)) ||
			pstShMC2->IsDriveBusy(UINT8(ENG_MMDI::en_stage_y)) ||
			!pstShMC2->IsDriveCmdDone(UINT8(ENG_MMDI::en_stage_x)) ||
			!pstShMC2->IsDriveCmdDone(UINT8(ENG_MMDI::en_stage_y)) ||
			!pstShMC2->IsDriveReached(UINT8(ENG_MMDI::en_stage_x)) ||
			!pstShMC2->IsDriveReached(UINT8(ENG_MMDI::en_stage_y)))
		{
			return true;
		}
		return false;
	}

	bool AlignMotion::MovetoGerberPos(int camNum, STG_XMXY tgtPos)
	{
		if (NowOnMoving())
			return false;

		Refresh();
		//캠1,2은 캠과 스테이지가 동시에 
		//캠3은 스테이지만 
		//자. 단순하다. 
		//현재 캠이 보고있는 거버내 좌표위치와 가야할 거버위치의 차를 이용해 이동해야할 총량을 구한다. 
		STG_XMXY gbrPos;

		if (GetGerberPosUseCamPos(camNum, gbrPos) == false)return false;

		double dx = tgtPos.mark_x - gbrPos.mark_x;
		double dy = tgtPos.mark_y - gbrPos.mark_y;

		dx = axises["stage"]["x"].currPos + dx;
		dy = axises["stage"]["y"].currPos + dy;


		if (isArrive("stage", "x", dx) == true && isArrive("stage", "y", dy) == true)
			return true;


		if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_x, dx) ||
			CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, dy))
			return false;

		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

		ENG_MMDI vecAxis;

		if (!uvEng_MC2_SendDevMoveVectorXY(ENG_MMDI::en_stage_x, ENG_MMDI::en_stage_y, dx, dy, uvEng_GetConfig()->mc2_svc.max_velo[UINT8(ENG_MMDI::en_stage_x)], vecAxis))
			return false;

		bool arrived = false;

		if (isArrive("stage", "x", dx) == false || isArrive("stage", "y", dy) == false)
		arrived = GlobalVariables::GetInstance()->Waiter([&]()->bool
		{
			bool res = ENG_MMDI::en_axis_none == vecAxis || uvCmn_MC2_IsDrvDoneToggled(vecAxis);
			return res;

		},10000);

		//pstCfg->
		//가급적 스테이지를 먼저 움직이고
		Refresh();
		//스테이지를 움직일수가 없다면 카메라축을 움직인다. 
		return arrived;
	}
	
	
	bool AlignMotion::GetCamPosUseGerberPos(STG_XMXY gerberPos,int baseCamNum, double& camAxis, double& stageX, double& stageY)
	{
		bool res = false;
		camAxis = 0;
		stageX = 0;
		stageY = 0;
		///

		double mark2Xgab = (markParams.caliGerbermark2x - markParams.currGerbermark2x);
		double mark2Ygab = (markParams.caliGerbermark2y - markParams.currGerbermark2y);

		double stageXgab = (axises["stage"]["x"].currPos - markParams.mark2StageX);
		double stageYgab = (axises["stage"]["y"].currPos - std::get<1>(markParams.mark2CamoffsetXY[1]));

		double camPos[] = { axises["cam"]["x1"].currPos ,axises["cam"]["x2"].currPos };

		///

		return res;
	}

	
	bool AlignMotion::GetGerberPosUseCamPos(int camNum, STG_XMXY& point)
	{
		const int _1to3_X_GAB = 0;
		const int _3to2_X_GAB = 1;
		const int _1to3_Y_GAB = 2;
		const int _1to2_Y_GAB = 3;

		const int CAM1 = 0;
		const int CAM2 = 1;

		const int X = 0;
		const int Y = 1;

		double mark2Xgab = (markParams.caliGerbermark2x - markParams.currGerbermark2x);
		double mark2Ygab = (markParams.caliGerbermark2y - markParams.currGerbermark2y);

		double stageXgab = (axises["stage"]["x"].currPos - markParams.mark2StageX);
		double stageYgab = (axises["stage"]["y"].currPos - std::get<1>(markParams.mark2CamoffsetXY[1]));

		
		double camPos[] = { axises["cam"]["x1"].currPos ,axises["cam"]["x2"].currPos };

		double camXOffset = camNum == 1 ? camPos[CAM1] - std::get<0>(markParams.mark2CamoffsetXY[1]) :
							camNum == 2 ? ((markParams.distCam2cam[_1to3_X_GAB] + markParams.distCam2cam[_3to2_X_GAB]) - std::get<0>(markParams.mark2CamoffsetXY[1])) +  camPos[CAM2] :
							camNum == 3 ? (markParams.distCam2cam[_1to3_X_GAB] - std::get<0>(markParams.mark2CamoffsetXY[1])) + std::get<0>(markParams.mark2CamoffsetXY[3]) : 0;

		double camYOffset = camNum == 3 ? markParams.distCam2cam[_1to3_Y_GAB] + std::get<1>(markParams.mark2CamoffsetXY[3]) :
							camNum == 2 ? markParams.distCam2cam[_1to2_Y_GAB] : 0;

		//역산시작
		double tempGerberX = markParams.currGerbermark2x + mark2Xgab + stageXgab + camXOffset;
		double tempGerberY = markParams.currGerbermark2y + mark2Ygab + stageYgab + camYOffset;

		point.mark_x = std::round(tempGerberX * std::pow(10, 3)) / std::pow(10, 3);;
		point.mark_y = std::round(tempGerberY * std::pow(10, 3)) / std::pow(10, 3); ;
		return true;

	}

	void AlignMotion::GetFiducialDimension(ENG_AMTF types, int& x, int& y)
	{
		map<double, double> mx, my;

		for each (STG_XMXY fid in status.markList[types])
		{
			mx[std::floor(fid.mark_x * 100.0f) / 100.0f] = fid.mark_x;
			my[std::floor(fid.mark_y * 100.0f) / 100.0f] = fid.mark_y;
		}

		x = mx.size();
		y = my.size();
	}
 
	/*void AlignMotion::SetAlignMode(ENG_AMOS motion, ENG_ATGL aligntype)
	{
		markParams.alignMotion = motion;
		markParams.alignType = aligntype;
	}*/

	bool AlignMotion::CheckAlignScanFinished(int scanCount)
	{
		return status.lastScanCount <= scanCount ? true : false;
	}

	void AlignMotion::UpdateParamValues()
	{
		

		LPG_RJAF job = uvEng_JobRecipe_GetSelectRecipe();

		LPG_MACP thick = job != nullptr ? uvEng_ThickCali_GetRecipe(job->cali_thick) : nullptr;
		LPG_RAAF alignRecipe = uvEng_Mark_GetSelectAlignRecipe();

		const int _1to3 = 0;
		const int _2to3 = 1;
		const int _1to3_Y_OFFSET = 2;
		const int _1to2_Y_OFFSET = 3;

		int acamCount = 2;

		if(pstCfg == nullptr || thick == nullptr || alignRecipe == nullptr)
			throw exception();
		
		const int X = 0;
		const int Y = 1;
		
		markParams.distCam2cam[_1to3] = pstCfg->set_align.distCam2Cam[_1to3];
		markParams.distCam2cam[_2to3] = pstCfg->set_align.distCam2Cam[_2to3];
		markParams.distCam2cam[_1to3_Y_OFFSET] = pstCfg->set_align.distCam2Cam[_1to3_Y_OFFSET];
		markParams.mark2StageX = pstCfg->set_align.table_unloader_xy[0][0];
		markParams.caliGerbermark2x = pstCfg->set_align.mark2_org_gerb_xy[0];
		markParams.caliGerbermark2y = pstCfg->set_align.mark2_org_gerb_xy[1];

		

		markParams.distCam2cam[_1to2_Y_OFFSET] = thick->mark2_stage_y[1] - thick->mark2_stage_y[0];
		
		markParams.mark2CamoffsetXY[1] = make_tuple(std::round(thick->mark2_acam_x[0] * std::pow(10, 3)) / std::pow(10, 3),
													std::round(thick->mark2_stage_y[0] * std::pow(10, 3)) / std::pow(10, 3));
		
		markParams.mark2CamoffsetXY[2] = make_tuple(std::round(thick->mark2_acam_x[1] * std::pow(10, 3)) / std::pow(10, 3),
													std::round(thick->mark2_stage_y[1] * std::pow(10, 3)) / std::pow(10, 3));


		markParams.mark2CamoffsetXY[3] = make_tuple(std::round(thick->mark2CentercamOffsetXY[0] * std::pow(10, 3)) / std::pow(10, 3),
													std::round(thick->mark2CentercamOffsetXY[1] * std::pow(10, 3)) / std::pow(10, 3));
		
		markParams.alignType = (ENG_ATGL)alignRecipe->align_type;
		markParams.alignMotion = (ENG_AMOS)alignRecipe->align_motion;
	
		STG_XMXY temp;
		auto globalFiducial = uvEng_Luria_GetGlobalFiducial();
		for (int i = 0; i < globalFiducial->GetCount(); i++)
		if (globalFiducial->GetMark(i, temp) && temp.tgt_id == 1)
		{
			markParams.currGerbermark2x = std::round(temp.mark_x * std::pow(10, 3)) / std::pow(10, 3);
			markParams.currGerbermark2y = std::round(temp.mark_y * std::pow(10, 3)) / std::pow(10, 3);
		}

	}

	vector<STG_XMXY> AlignMotion::GetFiducialPool(int camNum)
	{
		return status.markPoolForCam[camNum];
	}

	void AlignMotion::SetAlignOffsetPool(map< CaliTableType, vector<CaliPoint>> offsetPool)
	{
		status.offsetPool = offsetPool;
	}

	void AlignMotion::SetFiducialPool(bool useDefault, ENG_AMOS alignMotion, ENG_ATGL alignType)
	{
		const int sideCamCnt = 2; 
		const int centerCamIdx = markParams.centerCamIdx;

		if (useDefault)
		{
			alignMotion = markParams.alignMotion;
			alignType = markParams.alignType;
		}

		status.BufferClear();

		auto GenOntheFly2camFid = [&]()
	{
			int tempX = 0, tempY = 0;
			GetFiducialDimension(ENG_AMTF::en_local, tempX, tempY);
			
			bool basicUp = true; // 위에서 아래로 올라갈경우이다. 
			bool toUp = basicUp;
			for (int i = 0; i < tempX; i++)
			{
				vector<STG_XMXY> temp;
				std::copy(status.markList[ENG_AMTF::en_local].begin() + (i * tempY),
					status.markList[ENG_AMTF::en_local].begin() + (i * tempY) + tempY,
					std::back_inserter(temp));

				std::copy(temp.begin(), temp.end(), std::back_inserter(status.markMapConst[i]));

				auto tgt = &status.markPoolForCam[i >= (tempX / sideCamCnt) ? 2 : 1];

				if (i == tempX / sideCamCnt)
					toUp = basicUp;

				if (toUp)
					std::reverse(temp.begin(), temp.end());

				toUp = !toUp;

				tgt->insert(tgt->end(), temp.begin(), temp.end());
			}
			status.lastScanCount = tempX / sideCamCnt;
			status.acamCount = sideCamCnt;
		};

		auto GenStatic3camFid = [&]() //<-얘는 작업직전 실시간으로 해야함.
		{
			STG_XMXY lookat;
			const int centercam = centerCamIdx;
			bool res = true;

			GetGerberPosUseCamPos(centercam, lookat);

			STG_XMXY current = lookat;
			auto& pool =  status.markPoolForCam[centercam];

			while (res == true)
				if (res = GetNearFid(current, alignType == ENG_ATGL::en_global_4_local_0_point ? SearchFlag::global : SearchFlag::all, pool, current))
				{
					pool.push_back(current);

					//throw exception();
					//여기서 보정테이블값 넣어주면된다.
				}
		};


		auto GenCommonGlobal = [&]()
		{
			LPG_RJAF job = uvEng_JobRecipe_GetSelectRecipe();
			STG_XMXY temp;
			if (job != nullptr)
			{
				auto globalFiducial = uvEng_Luria_GetGlobalFiducial();
				auto localFiducial = uvEng_Luria_GetLocalFiducial();

				for (int i = 0; i < globalFiducial->GetCount(); i++)
					if (globalFiducial->GetMark(i, temp))
					{
						temp.SetFlag(STG_XMXY_RESERVE_FLAG::GLOBAL);
						status.markList[ENG_AMTF::en_global].push_back(temp);
						status.markList[ENG_AMTF::en_mixed].push_back(temp);
						status.markPoolForCam[(i / sideCamCnt) + 1].push_back(temp);
					}

				for (int i = 0; i < localFiducial->GetCount(); i++)
					if (localFiducial->GetMark(i, temp))
					{
						temp.SetFlag(STG_XMXY_RESERVE_FLAG::LOCAL);
						status.markList[ENG_AMTF::en_local].push_back(temp);
						status.markList[ENG_AMTF::en_mixed].push_back(temp);
					}


				int tempX = 0, tempY = 0;
				GetFiducialDimension(ENG_AMTF::en_local, tempX, tempY);
				status.gerberColCnt = tempX;
				status.gerberRowCnt = tempY;
				status.localMarkCnt = status.markList[ENG_AMTF::en_local].size();
				status.globalMarkCnt = status.markList[ENG_AMTF::en_global].size();
				status.SetDataReady(true);
				assert(tempX * tempY == status.markList[ENG_AMTF::en_local].size());
			}

		};

		GenCommonGlobal();

		switch (alignMotion)
		{
			case ENG_AMOS::en_onthefly_2cam:  GenOntheFly2camFid();	break;
			case ENG_AMOS::en_static_3cam:  GenStatic3camFid();	break;
			default: throw std::exception("not implement"); break;
		}
	}

	void AlignMotion::DoInitial(LPG_CIEA pstCfg)
	{
		this->pstCfg = pstCfg;

		LoadCaliData(this->pstCfg);

		axises["stage"]["x"] = Axis("x", MovingDir::X, (int)Parts::none,
			(double)pstCfg->mc2_svc.min_dist[UINT8(ENG_MMDI::en_stage_x)],
			(double)pstCfg->mc2_svc.max_dist[UINT8(ENG_MMDI::en_stage_x)],
			[&]()->double {return uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x); },
			[&]()->BOOL {return uvCmn_MC2_IsDriveError(ENG_MMDI::en_stage_x); });

		axises["stage"]["y"] = Axis("y", MovingDir::Y, (int)Parts::none,
			(double)pstCfg->mc2_svc.min_dist[UINT8(ENG_MMDI::en_stage_y)],
			(double)pstCfg->mc2_svc.max_dist[UINT8(ENG_MMDI::en_stage_y)],
			[&]()->double {return (double)uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y); },
			[&]()->BOOL {return uvCmn_MC2_IsDriveError(ENG_MMDI::en_stage_y); });

		axises["cam"]["x1"] = Axis("x1", MovingDir::X, (int)Parts::camera,
			(double)pstCfg->mc2_svc.min_dist[UINT8(ENG_MMDI::en_align_cam1)],
			(double)pstCfg->mc2_svc.max_dist[UINT8(ENG_MMDI::en_align_cam1)],
			[&]()->double {return (double)uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1); },
			[&]()->BOOL {return uvCmn_MC2_IsDriveError(ENG_MMDI::en_align_cam1); });

		axises["cam"]["x2"] = Axis("x2", MovingDir::X, (int)Parts::camera,
			(double)pstCfg->mc2_svc.min_dist[UINT8(ENG_MMDI::en_align_cam2)],
			(double)pstCfg->mc2_svc.max_dist[UINT8(ENG_MMDI::en_align_cam2)],
			[&]()->double {return (double)uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam2); },
			[&]()->BOOL {return uvCmn_MC2_IsDriveError(ENG_MMDI::en_align_cam2); });;

		
		ThreadManager::getInstance().addThread("update", cancelFlag,[&]() {Update(); });

	}
 

	void WebMonitor::StartWebMonitor()
	{
		const int SERVER_PORT = 5000;

		auto gv = GlobalVariables::GetInstance();

		if (gv->GetWebMonitor().StartWebServer(SERVER_PORT) == false)
			return;

		if (gv->GetWebMonitor().ConnectClient(SERVER_PORT) == false)
			return;
	
		ThreadManager::getInstance().addThread("webmonitor", cancelFlag,[&]()
			{
				auto gv = GlobalVariables::GetInstance();
				const int updateDelay = 1;
				while (cancelFlag.load() == false)
				{
					gv->GetWebMonitor().RefreshWebPage();
					this_thread::sleep_for(chrono::seconds(updateDelay));
				}
				gv->GetWebMonitor().StopWebServer();
			});		
	}
