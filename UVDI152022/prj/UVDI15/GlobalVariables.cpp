#pragma once
//이하는 전부 인라인 클래스임.

using namespace std;

#include "pch.h"
#include "GlobalVariables.h"
#include "work\Work.h"

CommonMotionStuffs CommonMotionStuffs::inst;

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
			const int DATACOUNT = 4; //x,y, offsetx,offsety
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
			
			try
			{
				vector<double> temp;
				TCHAR* name = cfg->file_dat.staticAcamAlignCali;
				StackVector(name, caliDataMap[OffsetType::align], temp);
				if (caliDataMap[OffsetType::align].size() != 0)
				{
					SortPos(caliDataMap[OffsetType::align]);
					calidataFeature[OffsetType::align] = CaliFeature(temp);
				}

				name = cfg->file_dat.staticAcamExpoCali;
				StackVector(name, caliDataMap[OffsetType::expo], temp);
				if (caliDataMap[OffsetType::expo].size() != 0)
				{
					SortPos(caliDataMap[OffsetType::expo]);
					calidataFeature[OffsetType::expo] = CaliFeature(temp);
				}
			}
			catch (...)
			{
				caliInfoLoadComplete = false;
				return;
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
	

	
	auto& expoData = caliDataMap[OffsetType::expo];
	CaliPoint weightedAverageOffset = Estimate(expoData, gbrX, gbrY);

	swprintf_s(tzMsg, 256, L"EstimateExpoOffset : X =%.4f Y = %.4f , offset x = %.4f , y = %.4f", gbrX, gbrY, weightedAverageOffset.offsetX, weightedAverageOffset.offsetY);
	LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
	return weightedAverageOffset;
}

CaliCalc::CaliFeature CaliCalc::GetCalifeature(OffsetType type)
{
	return calidataFeature.count(type) == 0 ? CaliCalc::CaliFeature() : calidataFeature[type];
}

CaliPoint CaliCalc::EstimateAlignOffset(int camIdx, double stageX = 0, double stageY = 0, double camX = -1)
{

	const int STAGE_CALI_INDEX = 3;
	auto& stageCaliData = caliDataMap[OffsetType::align];
	auto& camCaliData = caliDataMap[OffsetType::align];

	CaliPoint stageOffset = Estimate(stageCaliData, stageX, stageY);
	//CaliPoint camOffset = camIdx != STAGE_CALI_INDEX ? Estimate(camCaliData, camX, -1) : CaliPoint();

	CaliPoint finalv = stageOffset;// +camOffset;
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

bool AlignMotion::GetOffsetFromPool(OffsetType type, int tgtMarkIdx, CaliPoint& temp)
{
	auto offsetPool = status.offsetPool[type];

	auto find = std::find_if(offsetPool.begin(), offsetPool.end(), [&](const CaliPoint p) {return p.srcFid.tgt_id == tgtMarkIdx; });
	if (find == offsetPool.end())
	{
		temp = CaliPoint();
		return false;
	}
	temp = (*find);
	return true;
}

CaliPoint AlignMotion::EstimateExpoOffset(double gbrX, double gbrY)
{
	return caliCalcInst.EstimateExpoOffset(gbrX, gbrY);
}

CaliCalc::CaliFeature AlignMotion::GetCalifeature(OffsetType type)
{
	return caliCalcInst.GetCalifeature(type);
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
					double dx = posList[i].mark_x - pos.mark_x;
					double dy = posList[i].mark_y - pos.mark_y;

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
		STG_XMXY gbrPos = STG_XMXY();
		STG_XMXY stgPos = STG_XMXY();
		GetStagePosUseGerberPos(camNum, tgtPos, stgPos);

		if (GetGerberPosUseCamPos(camNum, gbrPos) == false)return false;

		double dx = tgtPos.mark_x - gbrPos.mark_x;
		double dy = tgtPos.mark_y - gbrPos.mark_y;

		dx = axises["stage"]["x"].currPos + dx;
		dy = axises["stage"]["y"].currPos + dy;


		if (isArrive("stage", "x", dx) == true && isArrive("stage", "y", dy) == true)
			return true;


		if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_x, dx) ||
			CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, dy))
			throw exception();

		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

		ENG_MMDI vecAxis;

		if (!uvEng_MC2_SendDevMoveVectorXY(ENG_MMDI::en_stage_x, ENG_MMDI::en_stage_y, dx, dy, uvEng_GetConfig()->mc2_svc.max_velo[UINT8(ENG_MMDI::en_stage_x)], vecAxis))
			throw exception();

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

	//거버포지션에 대응하는 "얼라인 영역" 스테이지 위치 가져옴.
	void AlignMotion::GetStagePosUseGerberPos(int camNum, STG_XMXY gbrPos, STG_XMXY& stagePos)
	{
		STG_XMXY currPos = STG_XMXY();
		GetGerberPosUseCamPos(camNum, currPos);

		STG_XMXY gab = gbrPos - currPos;

		stagePos.mark_x = axises["stage"]["x"].currPos + gab.mark_x;
		stagePos.mark_y = axises["stage"]["y"].currPos + gab.mark_y;;

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

		point.mark_x = std::round(tempGerberX * std::pow(10, 3)) / std::pow(10, 3);
		point.mark_y = std::round(tempGerberY * std::pow(10, 3)) / std::pow(10, 3);
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
		markParams.centerCamIdx = pstCfg->set_align.centerCamIdx;
	}

	vector<STG_XMXY> AlignMotion::GetFiducialPool(int camNum)
	{
		return status.markPoolForCam[camNum];
	}

	void AlignMotion::SetAlignOffsetPool(map< OffsetType, vector<CaliPoint>> offsetPool)
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
			
			vector<STG_XMXY> lookatPool;
			
			
			const bool useRefind = GlobalVariables::GetInstance()->GetRefindMotion().IsUseRefind();

			if (useRefind) //순서대로
			{
				auto& pool = status.markPoolForCam[centercam];
				auto flag = alignType == ENG_ATGL::en_global_4_local_0_point ? SearchFlag::global : SearchFlag::all;

				if(flag == SearchFlag::global)
					pool.insert(pool.end(), status.markList[ENG_AMTF::en_global].begin(), status.markList[ENG_AMTF::en_global].end());
				
				if (flag == SearchFlag::all)
					pool.insert(pool.end(), status.markList[ENG_AMTF::en_local].begin(), status.markList[ENG_AMTF::en_local].end());
			}
			else //최단거리
			{
				GetGerberPosUseCamPos(centercam, lookat); //현재상황 기준 최단거리조건.
				STG_XMXY current = lookat;
				auto& pool = status.markPoolForCam[centercam];

				while (res == true)
					if (res = GetNearFid(current, alignType == ENG_ATGL::en_global_4_local_0_point ? SearchFlag::global : SearchFlag::all, pool, current))
					{
						pool.push_back(current);
					}
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
						if (temp.tgt_id == 1) status.mark1 = temp;
						else if (temp.tgt_id == 2) status.mark2 = temp;
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
		
	
		ThreadManager::getInstance().addThread("webmonitor", cancelFlag,[&]()
			{
				const int SERVER_PORT = 5000;

				auto gv = GlobalVariables::GetInstance();

				if (gv->GetWebMonitor().StartWebServer(SERVER_PORT) == false)
					return;

				if (gv->GetWebMonitor().ConnectClient(SERVER_PORT) == false)
					return;

				const int updateDelay = 1;
				while (cancelFlag.load() == false)
				{
					gv->GetWebMonitor().Update();
					gv->GetWebMonitor().RefreshWebPage();
					this_thread::sleep_for(chrono::seconds(updateDelay));
				}
				gv->GetWebMonitor().StopWebServer();
			});		
	}


	void Environment::WriteCalibResult()
	{

	}

	bool Environment::DoCalib()
	{
		/*
		드라이버에 에러가있는지
		현재 모션이 사용중인지
		혹시 메인스레드에 뭔가 작업중인지?
		
		

		카메라는 일단 충돌방지를 위해 각자 safety위치로.

		스테이지 이동

		카메라z축 이동

		스테이블 3초

		그랩

		옵셋갖고 
		
		save하고 
		return 결과.
		
		*/
		return true;
	}


	void Environment::UpdateStateValue()
	{
		//info에서 일단 가져옴.
	}


	
RefindMotion::RefindMotion()
{
	rstValue = RSTValue();
	useRefind = true;
	stepSizeX = 2.5;
	stepSizeY = 1.5;
}

bool RefindMotion::RSTValue::GetEstimatePos(double estimatedX, double estimatedY, double& correctedX, double& correctedY)
{
	if (rstCalcReady == false)
		return false;
	
	// 예상되는 선분의 벡터와 실제 관측된 선분의 벡터 계산
	double originalVecX = orgEndX - orgStartX;
	double originalVecY = orgEndY - orgStartY;
	double observedVecX = obsEndX - obsStartX;
	double observedVecY = obsEndY - obsStartY;

	// 벡터의 크기 계산
	double originalLength = std::sqrt(originalVecX * originalVecX + originalVecY * originalVecY);
	double observedLength = std::sqrt(observedVecX * observedVecX + observedVecY * observedVecY);

	// 스케일링 비율 계산
	double scale = observedLength / originalLength;

	// 벡터의 내적 계산
	double dotProduct = originalVecX * observedVecX + originalVecY * observedVecY;

	// 각도 계산
	double cosTheta = dotProduct / (originalLength * observedLength);
	double thetaRad = std::acos(cosTheta);

	// 방향 결정 (외적을 사용하여 방향을 판단)
	double crossProduct = originalVecX * observedVecY - originalVecY * observedVecX;
	thetaRad = crossProduct < 0 ? -thetaRad : thetaRad;

	// 예상 점을 원점으로 이동한 후 스케일링과 회전 적용
	double translatedX = estimatedX - orgStartX;
	double translatedY = estimatedY - orgStartY;
	double scaledX = translatedX * scale;
	double scaledY = translatedY * scale;
	double rotatedX = scaledX * std::cos(thetaRad) - scaledY * std::sin(thetaRad);
	double rotatedY = scaledX * std::sin(thetaRad) + scaledY * std::cos(thetaRad);

	// 회전한 좌표를 실제 관측된 선분의 시작점으로 평행 이동
	correctedX = rotatedX + obsStartX;
	correctedY = rotatedY + obsStartY;
}
 

bool RefindMotion::GetEstimatePos(double estimatedX, double estimatedY, double& correctedX, double& correctedY)
{
	return rstValue.GetEstimatePos(estimatedX, estimatedY, correctedX, correctedY);
}


//ROTATE, SCALE, TRANSFORM
bool RefindMotion::ProcessEstimateRST(int centerCam, std::vector<STG_XMXY> representPoints,bool& errFlag, std::vector<STG_XMXY>& refindOffsetPoints)
{
	const int STABLE_TIME = 1000;
	const int PAIR = 2;
	const int MARK1 = 0;
	const int MARK2 = 1;
	const double CONVERT_THRESHOLD = 1.3f; //원좌표 대비 이 크기만큼을 벗어나면 보정불가다.

	auto sleep = [&](int msec) {this_thread::sleep_for(chrono::milliseconds(msec)); };
	errFlag = false;
	tuple<double, double> refindOffset, grabErrOffset; //절대좌표 차이값

	AlignMotion& motions = GlobalVariables::GetInstance()->GetAlignMotion();
	vector<tuple<STG_XMXY, double, double >> findOffsets; //원래 좌표 구조 , 절대좌표 차이값 x, 절대좌표 차이값y,  
	vector<bool> findAtFirstTime = { false,false };
	if (representPoints.size() != PAIR)
	{
		errFlag = true;
		return true;
	}

	/*
	주 변경포인트. 
	rst 생성시 두 점중 하나라도 못찾았을경우에만 rst 데이터가 생성되고 그렇지 않다면 
	기존의 방법대로 처리된다. 

	*/

	auto res =  GlobalVariables::GetInstance()->Waiter([&]()->bool
	{
		try
		{
			auto currPosBeforeRefind = CommonMotionStuffs::GetInstance().GetCurrStagePos();
			auto currPath = representPoints.begin();
			motions.MovetoGerberPos(centerCam, *currPath);
			sleep(100);

			motions.Refresh();
			if (motions.NowOnMoving() == true)
				return false;

			if (CommonMotionStuffs::GetInstance().SingleGrab(centerCam) == false || CWork::GetAbort()) //그랩실패. 작업 외부종료
				throw exception();

			
			double grabOffsetX = 0, grabOffsetY = 0;
			
			//!!!
			//그랩옵셋을 0으로 만들도록 refind offset에 grab error offset을 더해버린다. 
			//!!!
			
			refindOffset = make_tuple(0, 0); grabErrOffset = make_tuple(0, 0);

			auto idx = std::distance(representPoints.begin(), std::find(representPoints.begin(), representPoints.end(), *currPath));
			
			if (CommonMotionStuffs::GetInstance().IsMarkFindInLastGrab(centerCam, &grabOffsetX, &grabOffsetY))
			{
				findAtFirstTime[idx] = true;
				refindOffsetPoints.push_back(STG_XMXY(0, 0, grabOffsetX, grabOffsetY, currPath->org_id));
				grabErrOffset = make_tuple(grabOffsetX, grabOffsetY);
			}
			else
			{

				uvEng_Camera_RemoveLastGrab(centerCam); //위에서 이미 한번 실패했으므로.
				auto findRes = ProcessRefind(centerCam, &refindOffset,&grabErrOffset);

				if (findRes == false)
					throw exception();

				refindOffsetPoints.push_back(STG_XMXY(std::get<0>(refindOffset), std::get<1>(refindOffset), 
											std::get<0>(grabErrOffset), std::get<1>(grabErrOffset), currPath->org_id));
			}

			findOffsets.push_back(make_tuple(*currPath, std::get<0>(refindOffset) - std::get<0>(grabErrOffset), std::get<1>(refindOffset) - std::get<1>(grabErrOffset)));
			representPoints.erase(currPath);

			if (representPoints.empty())
			{
				return true; //정상완료
			}
				
			return false;
		}
		catch (...)
		{
			errFlag = true;
			return true;
		}
	}, 60 * 1000 * PAIR);

	if (res == false || errFlag == true)
		return res;

	//여기서 rst 계산.

	const int STG_XMXY_VAL = 0, REFIND_OFFSET_X = 1, REFIND_OFFSET_Y = 2;

	if (findAtFirstTime[MARK1] == findAtFirstTime[MARK2] && findAtFirstTime[MARK2] == true) //전부다 원래 예상위치에서 찾은경우. 즉 벗어남이 없는경우.
	{
		findOffsets[MARK1] = make_tuple(std::get<STG_XMXY_VAL>(findOffsets[MARK1]), 0, 0); //원래 포지션값은 유지하고 옵셋만 초기화 .
		findOffsets[MARK2] = make_tuple(std::get<STG_XMXY_VAL>(findOffsets[MARK1]), 0, 0);
	}


	double diffX = fabs(std::get<REFIND_OFFSET_X>(findOffsets[MARK1]) - std::get<REFIND_OFFSET_X>(findOffsets[MARK2]));
	double diffY = fabs(std::get<REFIND_OFFSET_Y>(findOffsets[MARK1]) - std::get<REFIND_OFFSET_Y>(findOffsets[MARK2]));

	if (diffX > CONVERT_THRESHOLD || diffX > CONVERT_THRESHOLD)
	{
		errFlag = true;
		return true;
	}

	//여기서 중요한게 "보정가능성" 이다. 

	/*
	mark1와 mark2의  refind offset + graberr 의 차이가 1.3이상 나게되면 아에 이건 보정 자체가 불가능하다. 
	한쪽을 맞추면 한쪽은 반드시 화면에서 벗어나거나 1.3이상이하로 좁힐수가 없기때문이다. 
	이 경우엔 반드시 종료시켜준다. 




	*/

    rstValue = RSTValue(std::get<STG_XMXY_VAL>(findOffsets[MARK1]).mark_x, std::get<STG_XMXY_VAL>(findOffsets[MARK1]).mark_y,
						std::get<STG_XMXY_VAL>(findOffsets[MARK2]).mark_x, std::get<STG_XMXY_VAL>(findOffsets[MARK2]).mark_y,
						std::get<STG_XMXY_VAL>(findOffsets[MARK1]).mark_x + std::get<REFIND_OFFSET_X>(findOffsets[MARK1]),
						std::get<STG_XMXY_VAL>(findOffsets[MARK1]).mark_y + std::get<REFIND_OFFSET_Y>(findOffsets[MARK1]),
						std::get<STG_XMXY_VAL>(findOffsets[MARK2]).mark_x + std::get<REFIND_OFFSET_X>(findOffsets[MARK2]),
						std::get<STG_XMXY_VAL>(findOffsets[MARK2]).mark_y + std::get<REFIND_OFFSET_Y>(findOffsets[MARK2]));
						
	return true;

}

bool RefindMotion::ProcessRefind(int centerCam, std::tuple<double, double>* refindOffset, std::tuple<double, double>* grabOffset) //패턴기반.
{
	UINT8 XAXIS  = 0b00010000,
		  YAXIS  = 0b00100000;

	UINT8 MLEFT  = 0b00000001, 
		  MRIGHT = 0b00000010, 
		  MUP    = 0b00000100, 
		  MDOWN  = 0b00001000;
	
	double grabOffsetX = 0, grabOffsetY=0;
	const int STABLE_TIME = 1000;
	
	if(refindOffset != nullptr)
		*refindOffset = make_tuple(0, 0);

	auto prevStagePos = CommonMotionStuffs::GetInstance().GetCurrStagePos();
	bool sutable = false;
	vector<int> searchMap = { MUP | YAXIS ,MRIGHT | XAXIS ,MDOWN | YAXIS,MDOWN | YAXIS,MLEFT | XAXIS,MLEFT | XAXIS,MUP | YAXIS,MUP | YAXIS };

	auto step = searchMap.begin();
	while (sutable == false && step != searchMap.end() && CWork::GetAbort() == false)
	{
		try
		{
			auto targetAxis = (*step & XAXIS) != 0 ? ENG_MMDI::en_stage_x : ENG_MMDI::en_stage_y;
			auto modeDelta = (*step & MLEFT) != 0 || (*step & MRIGHT) != 0 ? stepSizeX : stepSizeY;
			modeDelta *= ((*step & MLEFT) != 0 || (*step & MDOWN) != 0) ? -1 : 1;

			sutable = CommonMotionStuffs::GetInstance().MoveAxis(targetAxis, false, modeDelta, true);

			if (sutable == false) //그랩실패
				break;

			this_thread::sleep_for(chrono::milliseconds(STABLE_TIME));

			sutable = CommonMotionStuffs::GetInstance().SingleGrab(centerCam);

			if (sutable == false) //그랩실패
				break;

			sutable = CommonMotionStuffs::GetInstance().IsMarkFindInLastGrab(centerCam,&grabOffsetX,&grabOffsetY);
			
			if (sutable == true) //마크찾기 성공.
				break;

			uvEng_Camera_RemoveLastGrab(centerCam);

			step++;
		}
		catch (...)
		{

		}
		
	}
	if (sutable == false)
		return sutable;

	if (refindOffset != nullptr)
	{
		auto currStagePos = CommonMotionStuffs::GetInstance().GetCurrStagePos();
		*refindOffset = make_tuple(std::get<0>(currStagePos) - std::get<0>(prevStagePos) ,
								  std::get<1>(currStagePos) - std::get<1>(prevStagePos));
	}

	if (grabOffset != nullptr)
	{
		*grabOffset = make_tuple(grabOffsetX, grabOffsetY);
	}
	return sutable;
}

tuple<double, double> CommonMotionStuffs::GetCurrStagePos()
{
	AlignMotion& motions = GlobalVariables::GetInstance()->GetAlignMotion();
	motions.Refresh();
	return make_tuple(motions.GetAxises()["stage"]["x"].currPos, motions.GetAxises()["stage"]["y"].currPos);
}

bool CommonMotionStuffs::SingleGrab(int camIndex)
{
	auto triggerMode = uvEng_Camera_GetTriggerMode(camIndex);
	vector<function<bool()>> trigAction =
	{
		[&]() {return uvEng_Camera_SWGrab(camIndex); },
		[&]() {return uvEng_Mvenc_ReqTrigOutOne(camIndex); },
	};
	bool finalFailed = false;
retry:
	int cnt = GetGrabCnt(camIndex);
	auto res = trigAction[triggerMode == ENG_TRGM::en_Sw_mode ? 0 : 1]();
	
	if (res)
	{
		this_thread::sleep_for(chrono::microseconds(200));
		res = GlobalVariables::GetInstance()->Waiter([&]()->bool
		{
			return cnt + 1 == GetGrabCnt(camIndex);
		}, 3000);
	}

	if (res == false && finalFailed == false)
	{
		finalFailed = !finalFailed;
		goto retry;
	}
	
	return res;
}

bool CommonMotionStuffs::IsMarkFindInLastGrab()
{
	auto lastGrab = uvEng_Camera_GetLastGrabbedMark();

	if (lastGrab == nullptr) return false; //그랩 자체가 문제.
	if (lastGrab->IsMarkValid()) return true; //이젠 찾을필요가 없다. 
	if (lastGrab->score_rate > 0 || lastGrab->scale_rate > 0) return true; //이게 좀 그런데 이건 찾긴했으나 조건이 좀 안좋은상태, 여기서 최적조건 찾는 로직으로 또 빠질수가 있다. 
}

int CommonMotionStuffs::GetGrabCnt(int camIdx)
{
	CAtlList <LPG_ACGR>* grabs = uvEng_Camera_GetGrabbedMarkAll();

	
	if (grabs->GetCount() == 0) return 0;
	int cnt = 0;

	LPG_ACGR lastGrab = nullptr;
	uvEng_Camera_TryEnterCS();
	for (int i = 0; i < grabs->GetCount(); i++)
	{
		auto grab = grabs->GetAt(grabs->FindIndex(i));
		if (grab->cam_id != camIdx) continue;
		cnt++;
		
	}
	uvEng_Camera_ExitCS();
	return cnt;
}

bool CommonMotionStuffs::IsMarkFindInLastGrab(int camIdx,double* grabOffsetX, double* grabOffsetY)
{
	CAtlList <LPG_ACGR>* grabs = uvEng_Camera_GetGrabbedMarkAll();

	int cnt = grabs->GetCount();
	if (cnt == 0) return false;

	LPG_ACGR lastGrab = nullptr;
	uvEng_Camera_TryEnterCS();
	for (int i = 0; i < grabs->GetCount(); i++)
	{
		auto grab = grabs->GetAt(grabs->FindIndex(i));
		if (grab->cam_id != camIdx) continue;
		
		lastGrab = nullptr;

		if (grab->IsMarkValid() == false) continue;
		if (grab->score_rate <= 0 && grab->scale_rate == 0) continue;

		if(grabOffsetX != nullptr)
			*grabOffsetX = grab->move_mm_x;
		if (grabOffsetY != nullptr)
			*grabOffsetY = grab->move_mm_y;
		lastGrab = grab;
	}
	uvEng_Camera_ExitCS();
	return lastGrab == nullptr ? false : true;
}

LPG_ACGR CommonMotionStuffs::GetGrabPtr(int camIdx, int tgtMarkIdx, ENG_AMTF markType)
{
	uvEng_Camera_TryEnterCS();
	CAtlList <LPG_ACGR>* grabs = uvEng_Camera_GetGrabbedMarkAll();
	for (int i = 0; i < grabs->GetCount(); i++)
	{
		auto grab = grabs->GetAt(grabs->FindIndex(i));
		if (grab == nullptr) continue;

		auto typeCorrect = markType == ENG_AMTF::en_global ? (grab->reserve & STG_XMXY_RESERVE_FLAG::GLOBAL) : (grab->reserve & STG_XMXY_RESERVE_FLAG::LOCAL);

		if (grab->cam_id == camIdx &&
			grab->fiducialMarkIndex == tgtMarkIdx &&
			typeCorrect != 0)
		{
			uvEng_Camera_ExitCS();
			return grab;
		}
	}
	uvEng_Camera_ExitCS();
	return nullptr;

}




void CommonMotionStuffs::GetOffsetsCurrPos(int centerCam, STG_XMXY mark, CaliPoint* alignOffset, CaliPoint* expoOffset,double posOffsetX, double posOffsetY )
{
	AlignMotion& motions = GlobalVariables::GetInstance()->GetAlignMotion();
	string temp = "x" + std::to_string(centerCam);

	auto markPos = mark.GetMarkPos();

	if (alignOffset != nullptr)
	{
		motions.Refresh();
		//실시간 좌표로 따지면 나중에 한번에 계산할수가 없다. 마크좌표 기준으로 스테이지 좌표를 역산하게해야한다. 
		*alignOffset = motions.EstimateAlignOffset(centerCam, motions.GetAxises()["stage"]["x"].currPos + posOffsetX,
			motions.GetAxises()["stage"]["y"].currPos + posOffsetY,
			centerCam == 3 ? 0 : motions.GetAxises()["cam"][temp.c_str()].currPos);
		
		alignOffset->srcFid = mark;
	}

	if (expoOffset != NULL)
	{
		*expoOffset = motions.EstimateExpoOffset(std::get<0>(markPos), std::get<1>(markPos));
		expoOffset->srcFid = mark;
	}
}


void CommonMotionStuffs::GetOffsetsUseMarkPos(int centerCam , STG_XMXY mark, CaliPoint* alignOffset , CaliPoint* expoOffset)
{
	AlignMotion& motions = GlobalVariables::GetInstance()->GetAlignMotion();
	string temp = "x" + std::to_string(centerCam);

	auto markPos = mark.GetMarkPos();

	if (alignOffset != nullptr)
	{

		//실시간 좌표로 따지면 나중에 한번에 계산할수가 없다. 마크좌표 기준으로 스테이지 좌표를 역산하게해야한다. 
		/**alignOffset = motions.EstimateAlignOffset(centerCam, motions.GetAxises()["stage"]["x"].currPos,
			motions.GetAxises()["stage"]["y"].currPos,
			centerCam == 3 ? 0 : motions.GetAxises()["cam"][temp.c_str()].currPos);*/
		STG_XMXY stagePos;
		motions.GetStagePosUseGerberPos(centerCam, mark, stagePos);

			*alignOffset = motions.EstimateAlignOffset(centerCam, stagePos.mark_x,stagePos.mark_y,
				centerCam == 3 ? 0 : motions.GetAxises()["cam"][temp.c_str()].currPos);


		alignOffset->srcFid = mark;
	}

	if (expoOffset != NULL)
	{
		*expoOffset = motions.EstimateExpoOffset(std::get<0>(markPos), std::get<1>(markPos));
		expoOffset->srcFid = mark;
	}
}

bool CommonMotionStuffs::MoveAxis(ENG_MMDI axis, bool absolute, double pos, bool waiting, int timeout)
{
	double curr = uvCmn_MC2_GetDrvAbsPos(axis);

	double dest = absolute ? pos : curr + pos;

	if (uvCmn_MC2_IsDriveError(axis) || uvCmn_MC2_IsMotorDriveStopAll() == false ||
		CInterLockManager::GetInstance()->CheckMoveInterlock(axis, dest))
		return false;

	uvCmn_MC2_GetDrvDoneToggled(axis);

	BOOL res = uvEng_MC2_SendDevAbsMove(axis, dest, uvEng_GetConfig()->mc2_svc.step_velo);

	if (waiting && res)
		res = GlobalVariables::GetInstance()->Waiter([&]()->bool
			{
				return uvCmn_MC2_IsDrvDoneToggled(axis);
			}, timeout);

	return res;
}