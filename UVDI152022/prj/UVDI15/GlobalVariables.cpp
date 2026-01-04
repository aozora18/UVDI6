#pragma once
//이하는 전부 인라인 클래스임.

using namespace std;

#include "pch.h"
#include "GlobalVariables.h"
#include "work\Work.h"
#include "stuffs.h"
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

CommonMotionStuffs CommonMotionStuffs::inst;

void Callback(const char* cmd);


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

	std::lock_guard<std::recursive_mutex> lock(mutex_);
	threads_.emplace(key, 
						make_tuple(std::make_unique<std::thread>(callback),
								   std::ref(stopFlag)));
}

void ThreadManager::removeThread(const std::string& key)
{
	std::lock_guard<std::recursive_mutex> lock(mutex_);
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
	std::lock_guard<std::recursive_mutex> lock(mutex_);

	while (threads_.empty() == false)
	{
		auto val = threads_.begin();
		removeThread(val->first);

	}
	threads_.clear();
}

bool ThreadManager::isThreadRunning(const std::string& key)
{
	std::lock_guard<std::recursive_mutex> lock(mutex_);
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




std::vector<double> tokenize(string str)
{
		const std::regex re(R"([\s|,]+)");
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
			[](const std::string& str) { return std::round(std::stod(str) * 1000.0) / 1000.0; });

		return doubleVector;
}

//camZMeasure용
void StackVector(TCHAR* name, map<int, vector<pair<double, pair<double, double>>>>& camZMeasurePtr)
{
	std::basic_string<TCHAR> str(name);
	if (str.empty()) return;

	try
	{
		int camIdx = 0;
		float bestZ = 0;

		std::string line = "";
		std::ifstream file(name);

		if (!file.is_open())
			return;

		std::getline(file, line);  //cam idx
		camIdx = atoi(line.c_str());

		std::getline(file, line); //best z
		bestZ = atof(line.c_str());

		for (std::string line; std::getline(file, line);)
		{
			std::string lineCopy = line;
			const std::vector<double> tokens = tokenize(lineCopy);
			if (tokens.empty()) continue;
			camZMeasurePtr[camIdx].push_back(make_pair(tokens[0], make_pair(tokens[1], tokens[2])));
		}
	}
	catch (exception e)
	{

	}
}

//static offset맵용
void StackVector(TCHAR* name, vector<CaliPoint>& dataList, vector<double>& featureStored)
{
	const int XCoord = 0, YCoord = 1, OffsetX = 2, OffsetY = 3;
	const int DATACOUNT = 4; //x,y, offsetx,offsety


	dataList.clear();
	featureStored.clear();

	std::basic_string<TCHAR> str(name);
	if (str.empty()) return;

	try
	{
		std::ifstream file(name);

		for (std::string line; std::getline(file, line);)
		{
			std::string lineCopy = line;
			const std::vector<double> tokens = tokenize(lineCopy);
			if (tokens.empty()) continue;

			if (tokens.size() != DATACOUNT)
				std::copy(tokens.begin(), tokens.end(), std::back_inserter(featureStored));
			else
				dataList.push_back(CaliPoint(tokens[XCoord], tokens[YCoord], tokens[OffsetX], tokens[OffsetY]));
		}
	}
	catch (exception e)
	{

	}
}

void CaliCalc::LoadCaliData(LPG_CIEA cfg)
{

	auto loadSeq = [&](LPG_CIEA cfg)
		{		
			
			TCHAR fileName[5][256];
			_tcscpy_s(fileName[0], cfg->file_dat.staticAcamAlignCali);
			_tcscpy_s(fileName[1], cfg->file_dat.staticAcamExpoCali);

			_tcscpy_s(fileName[2], cfg->file_dat.camZmeasure[0]);
			_tcscpy_s(fileName[3], cfg->file_dat.camZmeasure[1]);
			_tcscpy_s(fileName[4], cfg->file_dat.camZmeasure[2]);

			try
			{
				vector<double> temp;

				StackVector(fileName[0], caliDataMap[OffsetType::align], temp);
				if (caliDataMap[OffsetType::align].size() != 0)
				{
					SortPos(caliDataMap[OffsetType::align]);
					calidataFeature[OffsetType::align] = CaliFeature(temp);
				}

				StackVector(fileName[1], caliDataMap[OffsetType::expo], temp);
				if (caliDataMap[OffsetType::expo].size() != 0)
				{
					SortPos(caliDataMap[OffsetType::expo]);
					calidataFeature[OffsetType::expo] = CaliFeature(temp);
				}

				StackVector(fileName[2], camZMeasure);
				StackVector(fileName[3], camZMeasure);
				StackVector(fileName[4], camZMeasure);
				
			}
			catch (...)
			{
				caliInfoLoadComplete = false;
				return;
			}
			
			caliInfoLoadComplete = true;
		};

	
	if (caliInfoLoadComplete == false)
		loadseqThread = std::thread([=]() {loadSeq(cfg); });
}

pair<double,double> CaliCalc::GetCamZOffset(int camIdx, float currZ)
{
	auto it = camZMeasure.find(camIdx);
	if (it == camZMeasure.end() || it->second.empty()) return { 0,0 };

	auto v = it->second;
	sort(v.begin(), v.end(), [](auto& a, auto& b) { return a.first < b.first; });

	
	vector<pair<double, pair<double, double>>> u;
	const double eps = 1e-6;
	for (size_t i = 0; i < v.size();) {
		double z = v[i].first, sx = 0, sy = 0; int c = 0;
		do {
			sx += v[i].second.first;
			sy += v[i].second.second;
			++c; ++i;
		} while (i < v.size() && fabs(v[i].first - z) < eps);
		u.push_back({ z,{sx / c, sy / c} });
	}

	auto lb = lower_bound(u.begin(), u.end(), (double)currZ,
		[](auto& e, double z) { return e.first < z; });

	if (lb == u.begin()) return lb->second;
	if (lb == u.end())  return u.back().second;

	auto& a = *prev(lb), & b = *lb;
	double dz = b.first - a.first;
	if (fabs(dz) < eps) return a.second;

	double t = (currZ - a.first) / dz;
	return 
	{
		a.second.first + t * (b.second.first - a.second.first),
		a.second.second + t * (b.second.second - a.second.second)
	};
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

	if (points.front().x > x || points.front().y > y  ||
		points.back().x <  x || points.back().y < y)
		return *CaliPoint().SetError();

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



CaliPoint CaliCalc::EstimateExpoOffset(int camIdx, double gbrX, double gbrY)
{
	TCHAR tzMsg[256] = { NULL };
	
	if (calidataFeature[OffsetType::expo].caliCamIdx != camIdx)
		throw exception();

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

CaliPoint CaliCalc::EstimateAlignOffset(int camIdx, double stageX = 0, double stageY = 0)
{

	if (calidataFeature[OffsetType::align].caliCamIdx != camIdx)
		throw exception();

	const int STAGE_CALI_INDEX = 3;
	auto& stageCaliData = caliDataMap[OffsetType::align];
	//auto& camCaliData = caliDataMap[OffsetType::align];
	
	CaliPoint stageOffset = Estimate(stageCaliData, stageX, stageY);
	//CaliPoint camOffset = camIdx != STAGE_CALI_INDEX ? Estimate(camCaliData, camX, -1) : CaliPoint();

	if (stageOffset.IsError())
		throw exception();

	stageOffset.offsetX = Stuffs::CutEpsilon(stageOffset.offsetX);
	stageOffset.offsetY = Stuffs::CutEpsilon(stageOffset.offsetY);

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

pair<double, double> AlignMotion::GetCamZOffset(int camIdx, float currZ)
{
	return caliCalcInst.GetCamZOffset(camIdx, currZ);
}

CaliPoint AlignMotion::EstimateExpoOffset(int camIdx,  double gbrX, double gbrY)
{
	return caliCalcInst.EstimateExpoOffset(camIdx, gbrX, gbrY);
}

CaliCalc::CaliFeature AlignMotion::GetCalifeature(OffsetType type)
{
	return caliCalcInst.GetCalifeature(type);
}

CaliPoint AlignMotion::EstimateAlignOffset(int camIdx, double stageX, double stageY)
{
	return caliCalcInst.EstimateAlignOffset(camIdx, stageX, stageY);
}

 
void AlignMotion::Update()
{
	const int updateDelay = 1000;
	while (cancelFlag.load() == false)
	{
		Refresh();
		
		bool interlocked = CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_x, axises["stage"]["x"].currPos) || 
						   CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, axises["stage"]["y"].currPos);
		
		uvEng_MC2_SetInterlockState(interlocked);

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

	void AlignMotion::RevertPrevAlignMotion()
	{
		markParams.alignMotion = markParams.prevAlignMotion;
	}

	void AlignMotion::ResetprocessedAlignMotion()
	{
		markParams.processedAlignMotion = markParams.alignMotion;
	}

	ENG_AMOS AlignMotion::GerPrevAlignMotion()
	{
		return markParams.prevAlignMotion;
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

	bool AlignMotion::MovetoGerberPos(int camNum, STG_XMXY tgtPos, CaliPoint* offsetPos)
	{
		if (CommonMotionStuffs::GetInstance().NowOnMoving())
			return false;

		Refresh();
		//캠1,2은 캠과 스테이지가 동시에 
		//캠3은 스테이지만 
		//자. 단순하다. 
		//현재 캠이 보고있는 거버내 좌표위치와 가야할 거버위치의 차를 이용해 이동해야할 총량을 구한다. 
		STG_XMXY gbrPos = STG_XMXY();
		STG_XMXY stgPos = STG_XMXY();
		GetStagePosUseGerberPos(camNum, tgtPos, stgPos);
		float tempCorrectionX = 0.040 + 0.0041;
		float tempCorrectionY = 0.005 + - 0.0025;
		if (GetGerberPosUseCamPos(camNum, gbrPos) == false)return false;

		double dx = tgtPos.mark_x - gbrPos.mark_x;
		double dy = tgtPos.mark_y - gbrPos.mark_y;

		dx = axises["stage"]["x"].currPos + dx - (offsetPos != nullptr ? offsetPos->offsetX : 0) + tempCorrectionX;
		dy = axises["stage"]["y"].currPos + dy - (offsetPos != nullptr ? offsetPos->offsetY : 0) + tempCorrectionY;

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

	void AlignMotion::GetMarkoffsetLegacy(ENG_AMOS motionType,bool isGlobal, int tgtID, double& offsetX, double& offsetY)
	{
		offsetX = .0f; offsetY = .0f;

		if (pstCfg == nullptr)
			return; 

		std::tuple<double, double> val = {0,0};
		pstCfg->set_align.markOffsetPtr->Get(motionType,isGlobal, tgtID, val);
		offsetX = std::get<0>(val);
		offsetY = std::get<1>(val);
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
		double tempGerberX = markParams.currGerbermark2x + mark2Xgab + stageXgab + camXOffset ;
		double tempGerberY = markParams.currGerbermark2y + mark2Ygab + stageYgab + camYOffset;

		tempGerberX += markParams.centerMarkzeroOffsetX;
		tempGerberY += markParams.centerMarkzeroOffsetY;

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
		CUniToChar csCnv;
		LPG_RJAF job = uvEng_JobRecipe_GetSelectRecipe(uvEng_JobRecipe_WhatLastSelectIsLocal());

		LPG_MACP thick = job != nullptr ? uvEng_ThickCali_GetRecipe(job->cali_thick) : nullptr;
		LPG_RAAF alignRecipe = uvEng_Mark_GetAlignRecipeName(csCnv.Ansi2Uni(job->align_recipe)); //uvEng_Mark_GetSelectAlignRecipe();

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
		markParams.prevAlignMotion = markParams.alignMotion;
		markParams.processedAlignMotion = markParams.alignMotion;
		STG_XMXY temp;
		auto globalFiducial = uvEng_Luria_GetGlobalFiducial();
		for (int i = 0; i < globalFiducial->GetCount(); i++)
		if (globalFiducial->GetMark(i, temp) && temp.tgt_id == 1)
		{
			markParams.currGerbermark2x = std::round(temp.mark_x * std::pow(10, 3)) / std::pow(10, 3);
			markParams.currGerbermark2y = std::round(temp.mark_y * std::pow(10, 3)) / std::pow(10, 3);
		}
		markParams.centerCamIdx = pstCfg->set_align.centerCamIdx;



		markParams.centerMarkzeroOffsetX = pstCfg->set_align.centerMarkzeroOffsetX;
		markParams.centerMarkzeroOffsetY = pstCfg->set_align.centerMarkzeroOffsetY;

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

		if (alignType == ENG_ATGL::en_global_0_local_0x0_n_point || alignType == ENG_ATGL::en_not_defined)
			return;

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

			//if (useRefind) //순서대로
			//{
				auto& pool = status.markPoolForCam[centercam];
				auto flag = alignType == ENG_ATGL::en_global_4_local_0_point ? SearchFlag::global : SearchFlag::all;

				pool.insert(pool.end(), status.markList[ENG_AMTF::en_global].begin(), status.markList[ENG_AMTF::en_global].end());
				
				if (flag == SearchFlag::all)
					pool.insert(pool.end(), status.markList[ENG_AMTF::en_local].begin(), status.markList[ENG_AMTF::en_local].end());
			//}
			//else //최단거리
			//{
			//	GetGerberPosUseCamPos(centercam, lookat); //현재상황 기준 최단거리조건.
			//	STG_XMXY current = lookat;
			//	auto& pool = status.markPoolForCam[centercam];

			//	while (res == true)
			//		if (res = GetNearFid(current, alignType == ENG_ATGL::en_global_4_local_0_point ? SearchFlag::global : SearchFlag::all, pool, current))
			//		{
			//			pool.push_back(current);
			//		}
			//}
		};


		auto GenCommonGlobal = [&](ENG_AMOS motion)
		{
			bool isLocalSelectJob = uvEng_JobRecipe_WhatLastSelectIsLocal();
			LPG_RJAF job = uvEng_JobRecipe_GetSelectRecipe(isLocalSelectJob);
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

						if (motion == ENG_AMOS::en_onthefly_2cam)
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

		GenCommonGlobal(alignMotion);

		switch (alignMotion)
		{
			case ENG_AMOS::en_onthefly_2cam:  GenOntheFly2camFid();	break;
			case ENG_AMOS::en_static_3cam:  GenStatic3camFid();	break;
			default: throw std::exception("not implement"); break;
		}
	}

	ENG_MFOR AlignMotion::IsNeedManualFixOffset(int* camNum)
	{
		CUniToChar csCnv;
		bool isLocalSelectJob = uvEng_JobRecipe_WhatLastSelectIsLocal();
		LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe(isLocalSelectJob);
		LPG_REAF pstRecipeExpo = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstRecipe->expo_recipe));
		LPG_RAAF pstRecipeAlign = uvEng_Mark_GetAlignRecipeName(csCnv.Ansi2Uni(pstRecipe->align_recipe));
		auto* config = uvEng_GetConfig();

		AlignMotion& motions = GlobalVariables::GetInstance()->GetAlignMotion();
		
		if(motions.markParams.alignMotion == ENG_AMOS::none)
			return ENG_MFOR::noRecipeLoaded;

		auto grabCount = uvEng_Camera_GetGrabbedCount(camNum);

		if (grabCount == 0) 
			return ENG_MFOR::firstRun;

		auto u8Global = motions.status.globalMarkCnt;
		auto u8Local = motions.markParams.alignType == ENG_ATGL::en_global_4_local_0_point ? 0 : motions.status.localMarkCnt;
		
		bool allGrabbed = grabCount == (u8Global + u8Local);

		if (allGrabbed == false || (u8Global + u8Local == 0))
			return ENG_MFOR::grabcountMiss;



		bool allValid = uvEng_Camera_IsGrabbedMarkValidAll(false, pstRecipeExpo->mark_score_accept, camNum);
		if (allValid)
			return ENG_MFOR::noNeedToFix;

		return ENG_MFOR::canFix;
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

		ThreadManager::getInstance().removeThread("update");
		ThreadManager::getInstance().addThread("update", cancelFlag,[&]() {Update(); });

	}

	std::vector<std::string> SplitCmd(const char* cmd)
	{
		std::vector<std::string> tokens;
		std::string s(cmd);
		size_t pos = 0;
		while ((pos = s.find('/')) != std::string::npos) {
			tokens.push_back(s.substr(0, pos));
			s.erase(0, pos + 1);
		}
		if (!s.empty()) tokens.push_back(s); // 마지막 토큰
		return tokens;
	}

 
	void Callback(const char* cmd)
	{
		
		auto gv = GlobalVariables::GetInstance();

		auto spilt = SplitCmd(cmd);

		if (spilt[0] == "cmd")
		{
			if(spilt[1] == "_readLDS") // 센서에서 현재값 읽어오는거다. (OK)
			{
				int _1=0, _2=0;
				if (gv->GetAutofocus().GetCurrentAFSensingPosition(1, _1) == false ||
					gv->GetAutofocus().GetCurrentAFSensingPosition(2, _2) == false)
				{
					MessageBox(nullptr, L"failed", L"failed", MB_OK);
				}
				int debug = 0;
			}
			
			else if (spilt[1] == "_initAF") //오토포커스 초기화 
			{
				if (gv->GetAutofocus().InitFocusDrive() == false)
				{
				//	MessageBox(nullptr, L"failed", L"failed", MB_OK);
				}
			}
			
			else if (spilt[1] == "_readSV")
			{
				int _1 = 0, _2 = 0;
				if (gv->GetAutofocus().GetStoredAFPosition(1, _1) == false ||
					gv->GetAutofocus().GetStoredAFPosition(2, _2) == false)
				{
					MessageBox(nullptr, L"failed", L"failed", MB_OK);
				}
				int debug = 0;
			}
			else if(spilt[1] == "_writeSV")
			{
				int value = atoi(spilt[2].c_str());

				if (gv->GetAutofocus().SetStoredAFPosition(1, value) == false ||
					gv->GetAutofocus().SetStoredAFPosition(2, value) == false)
				{
					MessageBox(nullptr, L"failed", L"failed", MB_OK);
				}
			}

			else if (spilt[1] == "_IsldsOn")
			{
				bool _1, _2;
				if (gv->GetAutofocus().GetAFSensorIsOn(1, _1) == false ||
					gv->GetAutofocus().GetAFSensorIsOn(2, _2) == false)
				{
					MessageBox(nullptr, L"failed", L"failed", MB_OK);
				}
				int debug = 0;
			}


			else if (spilt[1] == "_ldsOn")
			{
				if (gv->GetAutofocus().SetAFSensorOnOff(1, true) == false ||
					gv->GetAutofocus().SetAFSensorOnOff(2, true) == false)
				{
					MessageBox(nullptr, L"failed", L"failed", MB_OK);
				}
			}

			else if (spilt[1] == "_ldsOff")
			{
				if (gv->GetAutofocus().SetAFSensorOnOff(1, false) == false ||
					gv->GetAutofocus().SetAFSensorOnOff(2, false) == false)
				{
					MessageBox(nullptr, L"failed", L"failed", MB_OK);
				}
			}

			else if(spilt[1] == "_internalLDS")
			{
				if (gv->GetAutofocus().SetAFSensorType(1,  AFstate::internal) == false ||
					gv->GetAutofocus().SetAFSensorType(2, AFstate::internal) == false)
				{
					MessageBox(nullptr, L"failed", L"failed", MB_OK);
				}

			}
			else if (spilt[1] == "_externalLDS")
			{
				if (gv->GetAutofocus().SetAFSensorType(1, AFstate::external) == false ||
					gv->GetAutofocus().SetAFSensorType(2, AFstate::external) == false)
				{
					MessageBox(nullptr, L"failed", L"failed", MB_OK);
				}

			}
			else if (spilt[1] == "_getLDStype")
			{
				AFstate::LDStype _1, _2;

				if (gv->GetAutofocus().GetAFSensorType(1, _1) == false ||
					gv->GetAutofocus().GetAFSensorType(2, _2) == false)
				{
					MessageBox(nullptr, L"failed", L"failed", MB_OK);
				}

				int debug = 0;

			}
			
			else if (spilt[1] == "_afOn")
			{
				if (gv->GetAutofocus().SetAFOnOff(1, true) == false ||
					gv->GetAutofocus().SetAFOnOff(2, true) == false)
				{
					MessageBox(nullptr, L"failed", L"failed", MB_OK);
				}

				

			}
			else if (spilt[1] == "_afOff")
			{
				if (gv->GetAutofocus().SetAFOnOff(1, false) == false ||
					gv->GetAutofocus().SetAFOnOff(2, false) == false)
				{
					MessageBox(nullptr, L"failed", L"failed", MB_OK);
				}



			}

			else if (spilt[1] == "_IsafOn")
			{
				bool _1, _2;
				if (gv->GetAutofocus().GetAFisOn(1, _1) == false ||
					gv->GetAutofocus().GetAFisOn(2, _2) == false)
				{
					MessageBox(nullptr, L"failed", L"failed", MB_OK);
				}
				int debug = 0;
			}
			else if (spilt[1] == "_readAFworkRange")
			{
				int _1, _2;

				if (gv->GetAutofocus().GetAFWorkRange(1, _1,_2) == false ||
					gv->GetAutofocus().GetAFWorkRange(2, _1,_2) == false)
				{
					MessageBox(nullptr, L"failed", L"failed", MB_OK);
				}
				int debug = 0;

			}
			else if (spilt[1] == "_writeAFworkRange")
			{
				int _1, _2;
				_1 = atoi(spilt[2].c_str());
				_2 = atoi(spilt[3].c_str());

				if (gv->GetAutofocus().SetAFWorkRange(1, _1,_2) == false ||
					gv->GetAutofocus().SetAFWorkRange(2, _1,_2) == false)
				{
					MessageBox(nullptr, L"failed", L"failed", MB_OK);
				}
			}
		}
		else if (spilt[0] == "config")
		{

		}
	}
#ifdef USEBT
	vector<string> BTController::GetPoolMessage(BtSppApi::MsgType msg)
	{
		if (msgPool[msg].empty())
			return vector<string>{};

		lock_guard<mutex> lg(mtx);

		auto& q = msgPool[msg];
		if (q.empty()) return {};
		auto val = q.front();
		q.pop();
		return val;
	}

	bool BTController::Stop()
	{
		cancelFlag = true;
		Sleep(2000);
		return true;
	}

	static std::atomic<bool> updateStopFlag = false;
	void Connected(string clientID)
	{
		
	}

#pragma pack(push, 1)
	struct BMPFileHeader {
		uint16_t bfType = 0x4D42; // 'BM'
		uint32_t bfSize;
		uint16_t bfReserved1 = 0;
		uint16_t bfReserved2 = 0;
		uint32_t bfOffBits;       // 헤더 + 팔레트 크기
	};

	struct BMPInfoHeader {
		uint32_t biSize = 40;
		int32_t  biWidth;
		int32_t  biHeight;
		uint16_t biPlanes = 1;
		uint16_t biBitCount = 8;   // 8bpp
		uint32_t biCompression = 0;
		uint32_t biSizeImage;
		int32_t  biXPelsPerMeter = 0;
		int32_t  biYPelsPerMeter = 0;
		uint32_t biClrUsed = 256;
		uint32_t biClrImportant = 256;
	};
#pragma pack(pop)

	std::vector<unsigned char> MakeBmpFromRaw8bpp(const unsigned char* raw, int width, int height)
	{
		int rowSize = ((width + 3) / 4) * 4; // 4바이트 패딩
		int dataSize = rowSize * height;

		BMPFileHeader fh;
		BMPInfoHeader ih;
		ih.biWidth = width;
		ih.biHeight = -height; // top-down (뒤집힘 방지)
		ih.biSizeImage = dataSize;

		// 팔레트: 256 엔트리 (BGRA)
		std::vector<unsigned char> palette(256 * 4);
		for (int i = 0; i < 256; i++) {
			palette[i * 4 + 0] = (unsigned char)i; // B
			palette[i * 4 + 1] = (unsigned char)i; // G
			palette[i * 4 + 2] = (unsigned char)i; // R
			palette[i * 4 + 3] = 0;                // Reserved
		}

		fh.bfOffBits = sizeof(fh) + sizeof(ih) + (uint32_t)palette.size();
		fh.bfSize = fh.bfOffBits + dataSize;

		std::vector<unsigned char> out;
		out.resize(fh.bfSize);

		unsigned char* ptr = out.data();
		memcpy(ptr, &fh, sizeof(fh)); ptr += sizeof(fh);
		memcpy(ptr, &ih, sizeof(ih)); ptr += sizeof(ih);
		memcpy(ptr, palette.data(), palette.size()); ptr += palette.size();

		// 픽셀 붙이기 (row padding 필요)
		for (int y = 0; y < height; y++) {
			memcpy(ptr, raw + y * width, width);
			ptr += rowSize;
		}

		return out;
	}

	bool BTController::Start()
	{
		if (Load() == false)
			return false;
		ThreadManager::getInstance().removeThread("btmonitor");
		ThreadManager::getInstance().addThread("btmonitor", cancelFlag, [&]()
		{
				char key[128], msg[1024];
				auto& ajin = GlobalVariables::GetInstance()->GetAjinMotion();
				auto& align = GlobalVariables::GetInstance()->GetAlignMotion();
				
				auto to_str3 = [](double v) {
					std::ostringstream oss;
					oss << std::fixed << std::setprecision(3) << v;
					return oss.str();
					};

				while (cancelFlag == false)
				{
					if (btSpp.WaitForListen() == false)
						continue;
					if (btSpp.PollMessage(key, sizeof(key), msg, sizeof(msg)))
					{
						int cmd = BtSppApi::MsgType::END;
						vector<string> sliced;
						if (btSpp.ParseMessage(msg, cmd, sliced))
						{
							lock_guard<mutex> lg(mtx);
							msgPool[(BtSppApi::MsgType)cmd].push(sliced);
						}
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(100));

					double x = align.GetAxises()["stage"]["x"].currPos;
					double y = align.GetAxises()["stage"]["y"].currPos;

					auto ajinAxis = ajin.GetAxisInfo();

					if (ajinAxis)
					{
						double cz1 = ajin.GetAxisInfo()[0].position;
						double cz2 = ajin.GetAxisInfo()[1].position;
						double cz3 = ajin.GetAxisInfo()[2].position;

						btSpp.AddOrUpdateMonitoringValue("stageX", to_str3(x).c_str(), "stage X(mm)");
						btSpp.AddOrUpdateMonitoringValue("stageY", to_str3(y).c_str(), "stage Y(mm)");
						btSpp.AddOrUpdateMonitoringValue("cam1Z", to_str3(cz1).c_str(), "cam1 Z(mm)");
						btSpp.AddOrUpdateMonitoringValue("cam2Z", to_str3(cz2).c_str(), "cam2 Z(mm)");
						btSpp.AddOrUpdateMonitoringValue("cam3Z", to_str3(cz3).c_str(), "cam3 Z(mm)");
					}
					btSpp.RefreshMonitoringValue();
				}
				btSpp.Unload();
				btSpp.ServerStop();
			});

		ThreadManager::getInstance().removeThread("btmsgPool");
			ThreadManager::getInstance().addThread("btmsgPool", cancelFlag, [&]()
			{
					

					while (cancelFlag == false)
					{
						lock_guard<mutex> lg(mtx);
						if (!msgPool[BtSppApi::MsgType::CMD].empty())
						{
							char buffer[125] = { 0, };
							btSpp.GetClients(buffer, 125);

							vector<string> msg = msgPool[BtSppApi::MsgType::CMD].front();
							msgPool[BtSppApi::MsgType::CMD].pop();




							if(msg[0] == "grab")
							{
								int camidx = stoi(msg[1]) != 0 ? 1 :
										     stoi(msg[2]) != 0 ? 2 :
									         stoi(msg[3]) != 0 ? 3 : 4;


								//////////////////////

								UINT8 u8ACamID = camidx;
								TCHAR tzVal[64] = { NULL };
								UINT64 u64Tick = GetTickCount64();
								LPG_ACGR pstGrab = NULL;

								if (uvEng_Camera_GetCamMode() != ENG_VCCM::en_image_mode)
								{
									if (!uvEng_Camera_IsSetMarkModelACam(u8ACamID, 2))
									{
										btSpp.ServerSend(buffer, (int)BtSppApi::MsgType::CMD, "error - set model first");
										continue;
									}
									uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);

									if (CommonMotionStuffs::GetInstance().SingleGrab(u8ACamID, true) == false)
									{
										btSpp.ServerSend(buffer, (int)BtSppApi::MsgType::CMD, "error - grab failed.");
										continue;
									}
								}


								do {
									UINT8 mode = 0xff;
									pstGrab = uvEng_Camera_RunModelCali(u8ACamID, mode, (UINT8)DISP_TYPE_MARK_LIVE, TMP_MARK, TRUE, 0, 0);
									if (pstGrab && 0x00 != pstGrab->marked)	break;
									
									if (u64Tick + 100 < GetTickCount64())	break;

									Sleep(100);

								} while (1);

								

								if (pstGrab && 0x00 != pstGrab->marked)
								{
									char buf[128];
									sprintf_s(buf, 128, "move_mm_x : %f, move_mm_y : %f, scale : %f , score : %f", pstGrab->move_mm_x, pstGrab->move_mm_y, pstGrab->scale_rate, pstGrab->score_rate);
									btSpp.ServerSend(buffer, (int)BtSppApi::MsgType::CMD, buf);
								}
								else
								{
									btSpp.ServerSend(buffer, (int)BtSppApi::MsgType::CMD, "GRAB NG - no mark find.");
								}
								if (pstGrab)
								{
									auto bmpBytes = MakeBmpFromRaw8bpp(pstGrab->grab_data, pstGrab->grab_width, pstGrab->grab_height);
									btSpp.UpdatePicture(buffer,bmpBytes );
								}
								//////////////////////
							}
							else if (msg[0] == "setImage" )
							{
								int ph = msg[1] == "1" ? 1 : 2;
								int img = stoi(msg[2]);

								
								uvEng_Luria_ReqSetLoadInternalTestImage(ph, img);
								//btSpp.Bind("setImage", vector<string>{ "selH", "imgIdx"});
								int debug = 0;

							}
							
							else if (msg[0] == "zeroPIndex")
							{
								int ph = msg[1] == "1" ? 1 : 2;
								int led = msg[3] == "1" ? 1 :
									led = msg[4] == "1" ? 2 :
									led = msg[5] == "1" ? 3 : 4;
								uvEng_Luria_ReqSetLedLightOnOff(ph, ENG_LLPI(led), 0);
							}
							else if (msg[0] == "setPIndex")
							{
								int ph = msg[1] == "1" ? 1 : 2;
								int led = msg[3] == "1" ? 1 :
									led = msg[4] == "1" ? 2 :
									led = msg[5] == "1" ? 3 : 4;
								int idx = std::stoi(msg[7]);

								uvEng_Luria_ReqSetLightIntensity(ph, ENG_LLPI(led), idx);
								uvEng_Luria_ReqSetLedLightOnOff(ph, ENG_LLPI(led), idx == 0 ? 0 : 1);
							}
							else if (msg[0] == "up" || msg[0] == "down" || msg[0] == "left" || msg[0] == "right")
							{
								bool abs = stoi(msg[1]) != 0 ? true :
									stoi(msg[2]) != 0 ? false : true;

								double value = stod(msg[3]);


								ENG_MMDI drv = msg[0] == "up" || msg[0] == "down" ? ENG_MMDI::en_stage_y : ENG_MMDI::en_stage_x;
								if (!abs && (msg[0] == "up" || msg[0] == "right"))
									value *= -1.0f;

								bool res = CommonMotionStuffs::GetInstance().MoveAxis(drv, abs, value, true);
								btSpp.ServerSend(buffer, (int)BtSppApi::MsgType::CMD, res ? "move done" : "move failed");
							}
							else if (msg[0] == "upz" || msg[0] == "downz")
							{
								double pos = stod(msg[6]);
								
								bool inc = msg[0] == "upz";

								int camidx = stoi(msg[1]) != 0 ? 0 :
											 stoi(msg[2]) != 0 ? 1 :
										     stoi(msg[3]) != 0 ? 2 : 3;

								bool abs = stoi(msg[4]) != 0 ? true :
										   stoi(msg[5]) != 0 ? false : true;

								

								ENG_MMDI drv = (ENG_MMDI)((int)ENG_MMDI::en_axis_acam1 + camidx);

								bool res = CommonMotionStuffs::GetInstance().MoveAxis(drv, abs, 
									abs ? pos : 
									inc ? pos : pos * -1.0f ,true);

								btSpp.ServerSend(buffer, (int)BtSppApi::MsgType::CMD, res ? "move done" : "move failed");
							}
						}
					}
					
			});


		btSpp.RegisterOnConnected([](const char* key, int len)
			{
				char buffer[125] = { 0, };
				strncat_s(buffer, 125, (char*)key, len);
				Connected(buffer);
			});

		//UI 스크립트 //
		btSpp.AddGroupbox("group1", "스테이지", 1, 1, 300, 400);
		btSpp.AddButton("up",   "↑", 50-5, 15, 30, 30);
		btSpp.AddButton("down", "↓", 50 - 5, 80, 30, 30);
		btSpp.AddButton("left", "←", 17 - 5, 48, 30, 30);
		btSpp.AddButton("right", "→", 85 - 5, 48, 30, 30);
		btSpp.AddMonitor(30, 150);

		

		btSpp.AddLabel("lbl", "스테이지 이동", 120, 5, 160, 30);
		btSpp.AddRadioButton("AbsMoveRad", "abs.", "moveMethod", true, 120, 40);
		btSpp.AddRadioButton("relMoveRad", "rel.", "moveMethod", false, 180, 40);

		btSpp.AddLabel("lbl", "Pos", 120, 80, 50, 35);
		btSpp.AddInputbox("value", "0.000", 180, 80, -1, 40);

		

		int camzGroupShiftx = -250;
		
		btSpp.AddGroupbox("group2", "카메라z", 560+ camzGroupShiftx, 1,580, 580);
		btSpp.AddButton("upz", "↑", 570+5 + camzGroupShiftx, 15, 30, 30);
		btSpp.AddButton("downz", "↓", 570+5 + camzGroupShiftx, 80, 30, 30);
		
		btSpp.AddLabel("lbl", "카메라z축 이동", 620 + camzGroupShiftx, 5, 140, 30);
		btSpp.AddRadioButton("AbsMoveRadZ", "abs.", "moveMethodz", true, 620 + camzGroupShiftx, 40);
		btSpp.AddRadioButton("relMoveRadZ", "rel.", "moveMethodz", false, 680 + camzGroupShiftx, 40);
		
		btSpp.AddRadioButton("selZ1", "cam1", "zaxis", true, 620-60 + camzGroupShiftx, 120);
		btSpp.AddRadioButton("selZ2", "cam2", "zaxis", false, 700 - 60 + camzGroupShiftx, 120);
		btSpp.AddRadioButton("selZ3", "cam3", "zaxis", false, 780 - 60 + camzGroupShiftx, 120);
		btSpp.AddButton("grab", "그랩", 650 + camzGroupShiftx, 155, 60, 30);

		btSpp.AddRadioButton("selH1", "Head1", "selH", true, 560 + camzGroupShiftx, 230, 120);
		btSpp.AddRadioButton("selH2", "Head2", "selH", false, 560 + camzGroupShiftx + 80, 230, 120);

		btSpp.AddRadioButton("selL1", "360", "selL", true, 560 + camzGroupShiftx, 270, 120);
		btSpp.AddRadioButton("selL1", "380", "selL", false, 560 + camzGroupShiftx + 70, 270, 120);
		btSpp.AddRadioButton("selL3", "395", "selL", false, 560 + camzGroupShiftx + 130, 270, 120);
		btSpp.AddRadioButton("selL4", "405", "selL", false, 560 + camzGroupShiftx + 190, 270, 120);

		btSpp.AddLabel("lbl", "P.Index", 560 + camzGroupShiftx, 320, 80, 35);
		btSpp.AddInputbox("pIndex", "100", 600 + camzGroupShiftx + 45, 320, 50, 35);
		btSpp.AddButton("setPIndex", "설정", 640 + camzGroupShiftx + 65, 320, 60, 30);
		btSpp.AddButton("zeroPIndex", "끄기", 640 + camzGroupShiftx + 125, 320, 60, 30);

		btSpp.AddLabel("lbl", "이미지로드", 560 + camzGroupShiftx, 360, 80, 35);
		btSpp.AddInputbox("imgIdx", "0", 600 + camzGroupShiftx + 45, 360, 30, 35);

		btSpp.AddButton("setImage", "로드", 640 + camzGroupShiftx + 40, 360, 60, 30);


		btSpp.AddLabel("lbl", "Pos", 620 + camzGroupShiftx, 80, 50, 35);
		btSpp.AddInputbox("valuez", "0.000", 675 + camzGroupShiftx, 80, -1, 40);
		btSpp.AddPicturebox(550, 10);

		btSpp.Bind("grab", vector<string>{ "selZ1", "selZ2","selZ3" });
		btSpp.Bind("up", vector<string>{ "AbsMoveRad", "relMoveRad", "value" });
		btSpp.Bind("down", vector<string>{ "AbsMoveRad", "relMoveRad", "value" });
		btSpp.Bind("left", vector<string>{ "AbsMoveRad", "relMoveRad", "value" });
		btSpp.Bind("right", vector<string>{ "AbsMoveRad", "relMoveRad", "value" });

		btSpp.Bind("upz", vector<string>{ "selZ1", "selZ2", "selZ3","AbsMoveRadZ","relMoveRadZ","valuez" });
		btSpp.Bind("downz", vector<string>{ "selZ1", "selZ2", "selZ3", "AbsMoveRadZ", "relMoveRadZ", "valuez" });

		btSpp.Bind("setImage", vector<string>{ "selH", "imgIdx"});
		btSpp.Bind("setPIndex", vector<string>{ "selH", "selL", "pIndex"});
		btSpp.Bind("zeroPIndex", vector<string>{ "selH", "selL"});

		btSpp.AddOrUpdateMonitoringValue("stageX", "0", "stage X(mm)");
		btSpp.AddOrUpdateMonitoringValue("stageY", "0", "stage Y(mm)");
		btSpp.AddOrUpdateMonitoringValue("cam1Z", "0", "cam1 Z(mm)");
		btSpp.AddOrUpdateMonitoringValue("cam2Z", "0", "cam2 Z(mm)");
		btSpp.AddOrUpdateMonitoringValue("cam3Z", "0", "cam3 Z(mm)");

		//
		return btSpp.ServerStart() == 1;
	}
#endif
	void WebMonitor::StartWebMonitor()
	{
		ThreadManager::getInstance().removeThread("webmonitor");
		ThreadManager::getInstance().addThread("webmonitor", cancelFlag,[&]()
			{
				
				this_thread::sleep_for(chrono::seconds(10));

				const int SERVER_PORT = 5000;

				auto gv = GlobalVariables::GetInstance();

				gv->GetWebMonitor().AddWebBtn("af초기화", "_initAF");

				gv->GetWebMonitor().AddWebBtn("set internal", "_internalLDS");
				gv->GetWebMonitor().AddWebBtn("set external", "_externalLDS");

				gv->GetWebMonitor().AddWebBtn("센서타입 읽기", "_getLDStype");
				
				gv->GetWebMonitor().AddWebBtn("센서 on/off상태", "_IsldsOn");

				gv->GetWebMonitor().AddWebBtn("센서 켜기", "_ldsOn");
				gv->GetWebMonitor().AddWebBtn("센서 끄기", "_ldsOff");
				
				gv->GetWebMonitor().AddWebBtn("현재 LDS값 읽기", "_readLDS");

				gv->GetWebMonitor().AddWebBtn("AF STORED VALUE 읽기", "_readSV");
				gv->GetWebMonitor().AddWebBtn("AF STORED VALUE 쓰기",  "_writeSV");

				gv->GetWebMonitor().AddWebBtn("AF on/off상태", "_IsafOn");
				gv->GetWebMonitor().AddWebBtn("AF 켜기", "_afOn");
				gv->GetWebMonitor().AddWebBtn("AF 끄기", "_afOff");

				gv->GetWebMonitor().AddWebBtn("AF 작동범위 읽기", "_readAFworkRange");
				gv->GetWebMonitor().AddWebBtn("AF 작동범위 쓰기", "_writeAFworkRange");


				if (gv->GetWebMonitor().StartWebServer(SERVER_PORT) == false)
					return;

				if (gv->GetWebMonitor().ConnectClient(SERVER_PORT) == false)
					return;

				gv->GetWebMonitor().RegisterCommandCallback(Callback);

				const int updateDelay = 1;
				while (cancelFlag.load() == false)
				{
					
					gv->GetWebMonitor().RefreshWebPage();
					this_thread::sleep_for(chrono::seconds(updateDelay));
				
				}
				gv->GetWebMonitor().StopWebServer();
			});		
	}


	void Environmental::GetCalibPositionData(double& stageX, double& stageY, int& calibCamIdx, double& calibCamZPos, double& calibCamXPos)
	{
		stageX = std::get<0>(calibStagePos);
		stageY = std::get<1>(calibStagePos);
		calibCamIdx = std::get<0>(calibCamAxisPos);
		calibCamXPos = std::get<1>(calibCamAxisPos);
		calibCamZPos = std::get<2>(calibCamAxisPos);
	}

	void Environmental::GetCalibOffset(double& offsetX, double& offsetY)
	{
		offsetX = std::get<0>(prevCalibOffset) - std::get<0>(latestCalibOffset);
		offsetY = std::get<1>(prevCalibOffset) - std::get<1>(latestCalibOffset);
	}

	bool Environmental::DoCalib()
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

	void Environmental::WriteCalibValue()
	{
		auto config = uvEng_GetConfig();
		auto& environmental = config->environmental;

		environmental = STG_ENVI();
	}

	void Environmental::ReadCalibValue()
	{
		//info에서 일단 가져옴.
		auto config = uvEng_GetConfig();
		auto& environmental = config->environmental;


		this->reCalibTermMinute = environmental.caliTimeMinuteTerm;
		this->reCalibGabTemp = environmental.caliTempGab;

		
		this->lastCalibDate = environmental.lastCalibDate;
		this->lastCalibTemperature = environmental.lastCalibTemperature;

		this->calibCamAxisPos = make_tuple(environmental.calibCamIdx, environmental.calibCamDriveAxisPos, environmental.calibCamZAxisPos);
		this->calibStagePos = make_tuple(environmental.calibStageX, environmental.calibStageY);


		latestCalibOffset = make_tuple(environmental.indacatorOffsetX, environmental.indacatorOffsetY);
		//prevCalibOffset
		//tuple<int, double, double> calibCamAxisPos; //캘리브레이션에 사용할 카메라와  camidx, x축좌표 , z축좌표
		//tuple<double, double> calibStagePos; // (x,y)

		//tuple<double, double> latestCalibOffset; //<-가장 최근에 캘리브레이션 옵셋
		//tuple<double, double> prevCalibOffset; //<- 기존의 캘리브레이션 옵셋


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
	int idx = 0;
	tuple<double, double> refindOffset, grabErrOffset;
	vector<tuple<double, double>> grabErrOffsetBuffer;

	auto sleep = [&](int msec) {this_thread::sleep_for(chrono::milliseconds(msec)); };
	errFlag = false;
	
	
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
			/*현재 위치 확인*/
			auto currPosBeforeRefind = CommonMotionStuffs::GetInstance().GetCurrStagePos();
			auto currPath = representPoints.begin();
			/*현재 위치기반으로 마크 위치로 이동*/
			motions.MovetoGerberPos(centerCam, *currPath);
			sleep(100);

			motions.Refresh();
			if (CommonMotionStuffs::GetInstance().NowOnMoving() == true)
				return false;
			/*Align Camera Grab 동작 실행 및 실패시 종료*/
			if (CommonMotionStuffs::GetInstance().SingleGrab(centerCam,false) == false || CWork::GetAbort()) //그랩실패. 작업 외부종료
				throw exception();

			
			double grabOffsetX = 0, grabOffsetY = 0;
			
			
			refindOffset = make_tuple(0, 0);

			if (CommonMotionStuffs::GetInstance().IsMarkFindInLastGrab(centerCam, &grabOffsetX, &grabOffsetY))
			{
				findAtFirstTime[idx++] = true;
				refindOffsetPoints.push_back(STG_XMXY(0, 0, grabOffsetX, grabOffsetY, currPath->org_id));
				grabErrOffsetBuffer.push_back(make_tuple(grabOffsetX, grabOffsetY));

				findOffsets.push_back(make_tuple(*currPath, grabOffsetX * -1.0f, grabOffsetY * -1.0f));
			}
			else
			{

				uvEng_Camera_RemoveLastGrab(centerCam); //위에서 이미 한번 실패했으므로.
				auto findRes = ProcessRefind(centerCam, &refindOffset,&grabErrOffset);

				if (findRes == false)
					throw exception();

				refindOffsetPoints.push_back(STG_XMXY(std::get<0>(refindOffset), std::get<1>(refindOffset), 
											std::get<0>(grabErrOffset), std::get<1>(grabErrOffset), currPath->org_id));

				grabErrOffsetBuffer.push_back(grabErrOffset);
				findOffsets.push_back(make_tuple(*currPath, std::get<0>(refindOffset) - std::get<0>(grabErrOffset), 
															std::get<1>(refindOffset) - std::get<1>(grabErrOffset)));
			}

			
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
		return false;

	//여기서 rst 계산.

	const int STG_XMXY_VAL = 0, REFIND_OFFSET_X = 1, REFIND_OFFSET_Y = 2;

	double diffX = std::get<REFIND_OFFSET_X>(findOffsets[1]) - std::get<REFIND_OFFSET_X>(findOffsets[0]);
	double diffY = std::get<REFIND_OFFSET_Y>(findOffsets[1]) - std::get<REFIND_OFFSET_Y>(findOffsets[0]);

	if (fabs(diffX) > thresholdDist || 
		fabs(diffY) > thresholdDist)
	{

		TCHAR tzMesg[128] = { NULL };
		swprintf_s(tzMesg, 128, L"Offset correction failed! diff_X = %f, diff_Y = %f (exceeds max capacity = %f)", diffX , diffY, thresholdDist);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);

		errFlag = true;
		return true;
	}


//여기서 중요한게 "보정가능성" 이다. 
/*
mark1와 mark2의  refind offset + graberr 의 차이가 1.3이상 나게되면 아에 이건 보정 자체가 불가능하다.
한쪽을 맞추면 한쪽은 반드시 화면에서 벗어나거나 1.3이상이하로 좁힐수가 없기때문이다.
이 경우엔 반드시 종료시켜준다.
*/

	if (findAtFirstTime[MARK1] == findAtFirstTime[MARK2] && findAtFirstTime[MARK2] == true) //전부다 원래 예상위치에서 찾은경우. 즉 벗어남이 없는경우.
	{
		findOffsets[MARK1] = make_tuple(std::get<STG_XMXY_VAL>(findOffsets[MARK1]), 0, 0); //원래 포지션값은 유지하고 옵셋만 초기화 .
		findOffsets[MARK2] = make_tuple(std::get<STG_XMXY_VAL>(findOffsets[MARK2]), 0, 0);
	}



    rstValue = RSTValue(std::get<STG_XMXY_VAL>(findOffsets[MARK1]).mark_x, std::get<STG_XMXY_VAL>(findOffsets[MARK1]).mark_y,
						std::get<STG_XMXY_VAL>(findOffsets[MARK2]).mark_x, std::get<STG_XMXY_VAL>(findOffsets[MARK2]).mark_y,
						std::get<STG_XMXY_VAL>(findOffsets[MARK1]).mark_x + std::get<REFIND_OFFSET_X>(findOffsets[MARK1]),
						std::get<STG_XMXY_VAL>(findOffsets[MARK1]).mark_y + std::get<REFIND_OFFSET_Y>(findOffsets[MARK1]),
						std::get<STG_XMXY_VAL>(findOffsets[MARK2]).mark_x + std::get<REFIND_OFFSET_X>(findOffsets[MARK2]),
						std::get<STG_XMXY_VAL>(findOffsets[MARK2]).mark_y + std::get<REFIND_OFFSET_Y>(findOffsets[MARK2]));
						
	return true;

}

void RefindMotion::UpdateParamValues()
{
	auto& align = uvEng_GetConfig()->set_align;

	this->useRefind = align.useRefind;
	this->stepSizeX = align.refindOffsetX;
	this->stepSizeY = align.refindOffsetY;
	this->refindCnt = align.refindCnt;
	this->thresholdDist = align.thresholdDist;
}



//최초 마크 인식 실패시 Refind 동작 
//refindOffset	= Mark Grab 성공한 위치 - 최초 Mark Grab 위치
//grabOffset	= move_mm_x, move_mm_y
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
	/*							    Y축 위,		X축 오른쪽,		Y축 아래,	Y축 아래,		X축 왼쪽,	X축 왼쪽,		Y축 위,		Y축 위*/
	vector<int> searchMap = { MUP | YAXIS ,MRIGHT | XAXIS ,MDOWN | YAXIS,MDOWN | YAXIS,MLEFT | XAXIS,MLEFT | XAXIS,MUP | YAXIS,MUP | YAXIS , //1회전
							  MUP | YAXIS,MRIGHT | XAXIS,MRIGHT | XAXIS,MRIGHT | XAXIS,MRIGHT | XAXIS,YAXIS,MDOWN,YAXIS,MDOWN,YAXIS,MDOWN,YAXIS,MLEFT,YAXIS,MLEFT,YAXIS,MLEFT,YAXIS,MLEFT,YAXIS,MUP | YAXIS,YAXIS,MUP | YAXIS,YAXIS,MUP | YAXIS,YAXIS,MUP | YAXIS }; //2회전

	auto step = searchMap.begin();
	while (sutable == false && step != searchMap.end())
	{
		try
		{
			if (CWork::GetAbort())
				throw exception();

			auto targetAxis = (*step & XAXIS) != 0 ? ENG_MMDI::en_stage_x : ENG_MMDI::en_stage_y;
			auto modeDelta = (*step & MLEFT) != 0 || (*step & MRIGHT) != 0 ? stepSizeX : stepSizeY;
			modeDelta *= ((*step & MLEFT) != 0 || (*step & MDOWN) != 0) ? -1 : 1;
			/*현재 위치에서 상대값 이동*/
			sutable = CommonMotionStuffs::GetInstance().MoveAxis(targetAxis, false, modeDelta, true);

			if (sutable == false) //이동실패
				break;

			this_thread::sleep_for(chrono::milliseconds(STABLE_TIME));

			sutable = CommonMotionStuffs::GetInstance().SingleGrab(centerCam,false);

			if (sutable == false) //그랩실패
				break;

			/*Grab Image에서 Mark Mvoe 산출*/
			sutable = CommonMotionStuffs::GetInstance().IsMarkFindInLastGrab(centerCam,&grabOffsetX,&grabOffsetY);
			
			if (sutable == true) //마크찾기 성공.
				break;

			uvEng_Camera_RemoveLastGrab(centerCam);

			step++;
		}
		catch (...)
		{
			return false;
		}
		
	}
	if (sutable == false)
		return sutable;

	if (refindOffset != nullptr)
	{
		auto currStagePos = CommonMotionStuffs::GetInstance().GetCurrStagePos();
		/*Refind에서 Mark Grab 성공한 위치 - 최초 Mark Grab 위치*/
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

bool CommonMotionStuffs::SingleGrab(int camIndex, bool successIfNotNull)
{
	ENG_TRGM triggerMode = uvEng_GetConfig()->set_cams.trigMode == 0 ? ENG_TRGM::en_line_mode : ENG_TRGM::en_Sw_mode;

	uvEng_Camera_TriggerMode((int)camIndex, triggerMode);

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
		LPG_ACGR grabPtr = nullptr;
		res = GlobalVariables::GetInstance()->Waiter([&]()->bool
		{
			return successIfNotNull ? uvEng_Camera_GetLastGrab(camIndex, grabPtr) : cnt + 1 == GetGrabCnt(camIndex);
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

bool CommonMotionStuffs::Inposition(ENG_MMDI axis, double pos, double threashold)
{
	auto inPos = [=](double curr, double tgt, double thr)->bool
		{
			return std::abs(curr - tgt) <= thr;
		};

	double currPos =  uvCmn_MC2_GetDrvAbsPos(axis);

	return inPos(currPos, pos, threashold);
	
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

bool CommonMotionStuffs::NowOnMoving()
{
	
	auto config = uvEng_GetConfig();

	if (config->IsRunDemo()) return false;

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




bool CommonMotionStuffs::GetOffsetsCurrPos(int centerCam, STG_XMXY mark, CaliPoint* alignOffset, CaliPoint* expoOffset,double posOffsetX, double posOffsetY )
{
	try
	{
		AlignMotion& motions = GlobalVariables::GetInstance()->GetAlignMotion();
		string temp = "x" + std::to_string(centerCam);

		auto markPos = mark.GetMarkPos();

		if (alignOffset != nullptr)
		{
			motions.Refresh();
			//실시간 좌표로 따지면 나중에 한번에 계산할수가 없다. 마크좌표 기준으로 스테이지 좌표를 역산하게해야한다. 
			*alignOffset = motions.EstimateAlignOffset(centerCam, motions.GetAxises()["stage"]["x"].currPos + posOffsetX, motions.GetAxises()["stage"]["y"].currPos + posOffsetY);
			//motions.GetAxises()["stage"]["y"].currPos + posOffsetY); //,
			//centerCam == 3 ? 0 : motions.GetAxises()["cam"][temp.c_str()].currPos);

			alignOffset->srcFid = mark;
		}

		if (expoOffset != NULL)
		{
			*expoOffset = motions.EstimateExpoOffset(centerCam, std::get<0>(markPos), std::get<1>(markPos));
			expoOffset->srcFid = mark;
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}


bool CommonMotionStuffs::GetOffsetsUseMarkPos(int centerCam , STG_XMXY mark, CaliPoint* alignOffset , CaliPoint* expoOffset, double posOffsetX, double posOffsetY)
{
	try
	{
		AlignMotion& motions = GlobalVariables::GetInstance()->GetAlignMotion();

		if (alignOffset != nullptr)
		{
			STG_XMXY stagePos;
			mark.mark_x += posOffsetX; mark.mark_y += posOffsetY;
			motions.GetStagePosUseGerberPos(centerCam, mark, stagePos);

			*alignOffset = motions.EstimateAlignOffset(centerCam, stagePos.mark_x, stagePos.mark_y);
			alignOffset->srcFid = mark;
		}

		if (expoOffset != NULL)
		{
			auto markPos = mark.GetMarkPos();
			markPos = make_tuple(std::get<0>(markPos) + posOffsetX, std::get<1>(markPos) + posOffsetY);

			*expoOffset = motions.EstimateExpoOffset(centerCam, std::get<0>(markPos), std::get<1>(markPos));
			expoOffset->srcFid = mark;
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}

bool CommonMotionStuffs::MoveAxis(ENG_MMDI axis, bool absolute, double pos, bool waiting, int timeout, double speed)
{

	auto& ajinMotion =  GlobalVariables::GetInstance()->GetAjinMotion();
	BOOL res = false;

	if (axis >= ENG_MMDI::en_axis_acam1 && axis <= ENG_MMDI::en_axis_theta)
	{
		int ajinAxis = (int)axis - (int)ENG_MMDI::en_axis_acam1;
		auto* axises = ajinMotion.GetAxisInfo();
		
		string resStr = "";
		res = absolute ? ajinMotion.MoveAbs(ajinAxis, pos, 10, resStr) : ajinMotion.MoveRel(ajinAxis, pos, 10, resStr);
	}
	else
	{
		double curr = uvCmn_MC2_GetDrvAbsPos(axis);

		double dest = absolute ? pos : curr + pos;

		if (uvCmn_MC2_IsDriveError(axis) || uvCmn_MC2_IsMotorDriveStopAll() == false ||
			CInterLockManager::GetInstance()->CheckMoveInterlock(axis, dest))
			return false;

		uvCmn_MC2_GetDrvDoneToggled(axis);

		BOOL res = uvEng_MC2_SendDevAbsMove(axis, dest, speed);

		if (waiting && res)
			res = GlobalVariables::GetInstance()->Waiter([&]()->bool
				{
					return uvCmn_MC2_IsDrvDoneToggled(axis);
				}, timeout);
	}
	

	return res;
}
/////////////////////////////////////////////////////////
//아진모션 네트워크
/////////////////////////////////////////////////////////
bool AjinMotionNetwork::Connect()
{
	client.SetEndpoint("100.100.100.43", 7777);
	BindEvent();
	client.start();

	Sleep(1000);
	return client.isConnected();
}

void AjinMotionNetwork::BindEvent()
{
	client.on<MsgSrvBroadcast>(broadcast, Info, [&](const MsgSrvBroadcast& b)
	{
		memcpy(&info, &b.axis, sizeof(axisInfo) * maxAjinAxis);
	});

	client.on<MsgResult>(etc, result, [&](const MsgResult& b)
	{
		
	});
}

void AjinMotionNetwork::Disconnect()
{
	client.stop();
	Sleep(1000);
}

bool AjinMotionNetwork::CanMovePos(int axis, bool positiveDir, string& desc)
{
	
	
	if (info[axis].isFault)
		desc =  string("axis") + std::to_string(axis) + string("is faulted");
	if (!info[axis].isHommed)
		desc = string("axis") + std::to_string(axis) + string("is not homed");
	if (info[axis].isMaxLimit && positiveDir)
		desc = string("axis") + std::to_string(axis) + string("is P Limited");
	if (info[axis].IsMinlimit && !positiveDir)
		desc = string("axis") + std::to_string(axis) + string("is N Limited");
	if (!info[axis].isInposition)
		desc = string("axis") + std::to_string(axis) + string("is busy");
	
	return desc.length() == 0 ? true : false;
}

bool AjinMotionNetwork::ResetMotor(int axis)
{
	MsgReset t;
	t.axisIndex = axis;
	if (client.send(t))
	{
		bool res = GlobalVariables::GetInstance()->Waiter([&]()->bool
			{
				return !info[axis].isFault;
			}, 10000);

		return res;
	}
	return false;
}

bool AjinMotionNetwork::StopMotor(int axis,bool all)
{
	MsgStop t;
	t.axisIndex = axis;
	t.all = all;

	if (client.send(t))
	{
		bool res = GlobalVariables::GetInstance()->Waiter([&]()->bool
			{
				return info[axis].isInposition;
			}, 10000);

		return res;
	}
	return false;
}


bool AjinMotionNetwork::MotorEnable(int axis, bool set)
{
	MsgMotorEnable t;
	t.axisIndex = axis;
	t.enable = set;

	if (client.send(t))
	{
		bool res = GlobalVariables::GetInstance()->Waiter([&]()->bool
			{
				return info[axis].isEnable == set;
			}, 10000);

		return res;
	}
	return false;
}

bool AjinMotionNetwork::MoveAbs(int axis, double pos, double velo, string& desc)
{
	if (CanMovePos(axis, pos > 0 ? true : false, desc) == false)
		return false;

	MsgMoveAbs t;
	t.axisIndex = axis;
	t.pos = pos;
	t.velocity = velo;

	if (client.send(t))
	{
		bool res = GlobalVariables::GetInstance()->Waiter([&]()->bool
			{
				return info[axis].isInposition;
			}, 10000);

		return res;
	}
	return false;

}

bool AjinMotionNetwork::MoveRel(int axis, double pos, double velo, string& desc)
{
	
	if (CanMovePos(axis, pos > 0 ? true : false, desc) == false)
		return false;

	MsgMoveRel t;
	t.axisIndex = axis;
	t.pos = pos;
	t.velocity = velo;

	if (client.send(t))
	{
		bool res = GlobalVariables::GetInstance()->Waiter([&]()->bool
			{
				return info[axis].isInposition;
			}, 10000);

		return res;
	}
	return false;
}

bool AjinMotionNetwork::HomeMotor(int axis)
{
	MsgHome t;
	t.axisIndex = axis;
	if (client.send(t))
	{
		bool res = GlobalVariables::GetInstance()->Waiter([&]()->bool
			{
				return info[axis].isHommed && info[axis].isInposition;
			}, 10000);

		return res;
	}
	return false;
}


float ThetaControl::GetMotorTheta()
{
	const int thetaAxis = 3;
	auto axisInfo =  GlobalVariables::GetInstance()->GetAjinMotion().GetAxisInfo();
	return axisInfo == nullptr ? 0 : axisInfo[thetaAxis].position;
}


void ThetaControl::ProcessThetaCorrection()
{
	if (thetaFeature.use == false)
		return;

	if (thetaFeature.needThetaControl)
	{
		MoveMotorTheta(thetaFeature.correctionTheta, false);
	}
	else
	{
		ResetPosition();
	}

	thetaFeature.needThetaControl = false;
}

bool ThetaControl::MoveMotorTheta(float theta,bool absMove)
{
	auto cut3 = [=](double v)->double
		{
			return std::round(v * 1000.0) / 1000.0;
		};
	
	theta = cut3(theta);

	const int thetaAxis = 3; const int defaultVelo = 5; string desc;
	
	return absMove ? GlobalVariables::GetInstance()->GetAjinMotion().MoveAbs(thetaAxis, theta, defaultVelo, desc) : 
					 GlobalVariables::GetInstance()->GetAjinMotion().MoveRel(thetaAxis, theta, defaultVelo, desc);
}


void ThetaControl::SetCam1Pos(double pos)
{
	thetaFeature.cam1Pos = pos;
}

void ThetaControl::SetStagePosbyFiducial(nPoint xyByFid,int fidIdx)
{	
	thetaFeature.stagexyByFid[fidIdx] = xyByFid;	
}

void ThetaControl::SetOffsetValue(nPoint xyByFid, int fidIdx)
{
	thetaFeature.offsetOfFid[fidIdx] = xyByFid;
}

bool ThetaControl::Judge()
{
	 return GetBestTheta() == -1 ? true : false;
}


double ThetaControl::GetBestTheta() //<-양 2점 스테이지 ,xy포지션
{
	double distFromCentercamToCam1x = (uvEng_GetConfig()->mc2_svc.max_dist[(int)ENG_MMDI::en_axis_acam1] - thetaFeature.cam1Pos) + thetaFeature.dist3to1X;
	double distFromCentercamToCam1y = thetaFeature.dist3to1Y;



	nPoint nP1 = nPoint(thetaFeature.stagexyByFid[0].x + thetaFeature.offsetOfFid[0].x + distFromCentercamToCam1x,
						thetaFeature.stagexyByFid[0].y + thetaFeature.offsetOfFid[0].y + distFromCentercamToCam1y);

	nPoint nP2 = nPoint(thetaFeature.stagexyByFid[1].x + thetaFeature.offsetOfFid[1].x + distFromCentercamToCam1x,
						thetaFeature.stagexyByFid[1].y + thetaFeature.offsetOfFid[1].y + distFromCentercamToCam1y);

	thetaFeature.needThetaControl =  fabs(thetaFeature.offsetOfFid[0].x - thetaFeature.offsetOfFid[1].x) > thetaFeature.threshold;

	thetaFeature.correctionTheta = thetaFeature.needThetaControl ? CaclThetaDegreeMakeSameX(thetaFeature.centerOfStageX, thetaFeature.centerOfStageY,
		nP1.x, nP1.y,
		nP2.x, nP2.y) : -1;

	float magicFeature = 0.168f;
	thetaFeature.correctionTheta = (thetaFeature.needThetaControl ? thetaFeature.correctionTheta + (thetaFeature.correctionTheta * magicFeature) : thetaFeature.correctionTheta) * -1;

	return thetaFeature.correctionTheta;
}

bool ThetaControl::ResetPosition(float theta)
{
	thetaFeature.needThetaControl = false;
	thetaFeature.correctionTheta = 0;
	return MoveMotorTheta(0.2f,true);
}

void ThetaControl::SetBasicFeatures(double centerOfStageX,
									double centerOfStageY,
									OffsetMap thetaMapping,
									float dist3to1X, float dist3to1Y)
{
	thetaFeature = feature(centerOfStageX, centerOfStageY, thetaMapping, dist3to1X, dist3to1Y);
}
////////////////////////////////////////////////////////////
///			IDS 카메라 
////////////////////////////////////////////////////////////
IDScamManager::IDScamManager() = default;

IDScamManager::~IDScamManager()
{
	Disconnect();
}

bool IDScamManager::Connect(int cameraId,  Aoi aoi)
{
	if (connected)
		return true;

	cam = (HIDS)cameraId;

	IS_RECT r{};
	r.s32X = aoi.x;
	r.s32Y = aoi.y;
	r.s32Width = aoi.w;
	r.s32Height = aoi.h;

	if (is_InitCamera(&cam, nullptr) != IS_SUCCESS)
		return false;

	
	if (is_SetColorMode(cam, IS_CM_MONO8) != IS_SUCCESS) //흑백고정.
		goto FAIL;

		

		if (is_AOI(cam, IS_AOI_IMAGE_SET_AOI, &r, sizeof(r)) != IS_SUCCESS)
			goto FAIL;
	
	if (!AllocateImageMem())
		goto FAIL;

	if (is_CaptureVideo(cam, IS_DONT_WAIT) != IS_SUCCESS)
		goto FAIL;

	if (isavi_InitAVI(&aviId, cam) != IS_AVI_NO_ERR)
		goto FAIL;

	connected = true;
	return true;

FAIL:
	Disconnect();
	return false;
}

void IDScamManager::Disconnect()
{
	StopRecording();

	if (cam)
		is_StopLiveVideo(cam, IS_FORCE_VIDEO_STOP);

	FreeImageMem();

	if (aviId)
	{
		isavi_ExitAVI(aviId);
		aviId = 0;
	}

	if (cam)
	{
		is_ExitCamera(cam);
		cam = 0;
	}

	connected = false;
	gainCache = -1;
}

bool IDScamManager::IsConnected() const
{
	return connected;
}

bool IDScamManager::GrabOnce()
{
	if (!connected)
		return false;

	if (is_FreezeVideo(cam, IS_WAIT) != IS_SUCCESS)
		return false;

	if (recording)
		(void)isavi_AddFrame(aviId, imageMem);

	return true;
}

void IDScamManager::GetFramePtr(const uint8_t*& ptr, int& w, int& h, int& bits, int& linePitch) const
{
	ptr = reinterpret_cast<const uint8_t*>(imageMem);
	w = width;
	h = height;
	bits = bpp;
	linePitch = pitch;
}

bool IDScamManager::SetExposureUs(double us)
{
	if (!connected)
		return false;

	double v = us;
	return is_Exposure(cam, IS_EXPOSURE_CMD_SET_EXPOSURE, &v, sizeof(v)) == IS_SUCCESS;
}


bool IDScamManager::SetPixelClockMHz(int mhz)
{
	if (!connected) return false;

	INT value = mhz;
	return is_PixelClock(cam, IS_PIXELCLOCK_CMD_SET, (void*)&value, sizeof(value)) == IS_SUCCESS;
}

bool IDScamManager::GetPixelClockMHz(int& outMhz) const
{
	if (!connected) return false;

	INT value = 0;
	if (is_PixelClock(cam, IS_PIXELCLOCK_CMD_GET, (void*)&value, sizeof(value)) != IS_SUCCESS)
		return false;

	outMhz = (int)value;
	return true;
}


bool IDScamManager::GetPixelClockRangeMHz(int& minMHz, int& maxMHz, int& incMHz)
{
	if (!connected)
		return false;

	INT r[3] = { 0, 0, 0 }; // [min, max, inc]

	if (is_PixelClock(
		cam,
		IS_PIXELCLOCK_CMD_GET_RANGE,
		r,
		sizeof(r)) != IS_SUCCESS)
		return false;

	minMHz = (int)r[0];
	maxMHz = (int)r[1];
	incMHz = (int)r[2];
	return true;
}



bool IDScamManager::SetFrameRate(double fps)
{
	if (!connected) return false;

	double newFps = fps;

	double actual = 0.0;
	INT ret = is_SetFrameRate(cam, newFps, &actual);
	return ret == IS_SUCCESS;
}

bool IDScamManager::GetFrameRate(double& outFps) const
{
	if (!connected) return false;


	double fps = 0.0;
#ifdef IS_GET_FRAMERATE
	double actual = 0.0;
	INT ret = is_SetFrameRate(cam, IS_GET_FRAMERATE, &actual);
	if (ret == IS_SUCCESS)
	{
		outFps = actual;
		return true;
	}
#endif


	return false;
}


bool IDScamManager::GetExposureRange(double& minUs, double& maxUs, double& incUs)
{
	if (!connected) return false;

	double range[3] = {}; // [min, max, increment]

	if (is_Exposure(
		cam,
		IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE,
		range,
		sizeof(range)) != IS_SUCCESS)
		return false;

	minUs = range[0];
	maxUs = range[1];
	incUs = range[2];
	return true;
}


bool IDScamManager::GetFrameRateRange(double& minFps, double& maxFps)
{
	if (!connected) return false;

	double minFt = 0, maxFt = 0, incFt = 0;

	if (is_GetFrameTimeRange(cam, &minFt, &maxFt, &incFt) != IS_SUCCESS)
		return false;

	maxFps = (minFt > 0) ? (1.0 / minFt) : 0.0;
	minFps = (maxFt > 0) ? (1.0 / maxFt) : 0.0;

	return true;
}

bool IDScamManager::GetExposureUs(double& outUs) const
{
	if (!connected)
		return false;

	double v = 0;
	if (is_Exposure(cam, IS_EXPOSURE_CMD_GET_EXPOSURE, &v, sizeof(v)) != IS_SUCCESS)
		return false;

	outUs = v;
	return true;
}

bool IDScamManager::SetGain(int value)
{
	if (!connected)
		return false;

	INT ret = is_SetHardwareGain(
		cam,
		value,
		IS_IGNORE_PARAMETER,
		IS_IGNORE_PARAMETER,
		IS_IGNORE_PARAMETER
	);

	if (ret != IS_SUCCESS)
		return false;

	gainCache = value;
	return true;
}

bool IDScamManager::GetGain(int& outValue) const
{
	if (!connected)
		return false;

	if (gainCache < 0)
		return false;

	outValue = gainCache;
	return true;
}

bool IDScamManager::SetAoi(int x, int y, int w, int h)
{
	if (!connected)
		return false;

	StopRecording();
	is_StopLiveVideo(cam, IS_FORCE_VIDEO_STOP);

	IS_RECT r{};
	r.s32X = x;
	r.s32Y = y;
	r.s32Width = w;
	r.s32Height = h;

	if (is_AOI(cam, IS_AOI_IMAGE_SET_AOI, &r, sizeof(r)) != IS_SUCCESS)
	{
		is_CaptureVideo(cam, IS_DONT_WAIT);
		return false;
	}

	if (!AllocateImageMem())
	{
		is_CaptureVideo(cam, IS_DONT_WAIT);
		return false;
	}

	return is_CaptureVideo(cam, IS_DONT_WAIT) == IS_SUCCESS;
}

bool IDScamManager::GetAoi(Aoi& out) const
{
	if (!connected)
		return false;

	IS_RECT r{};
	if (is_AOI(cam, IS_AOI_IMAGE_GET_AOI, &r, sizeof(r)) != IS_SUCCESS)
		return false;

	out.x = r.s32X;
	out.y = r.s32Y;
	out.w = r.s32Width;
	out.h = r.s32Height;
	return true;
}

bool IDScamManager::SnapshotFile(const wchar_t* path, int quality)
{
	if (!connected)
		return false;

	IMAGE_FILE_PARAMS p{};
	p.pwchFileName = const_cast<wchar_t*>(path);
	p.nFileType = IS_IMG_JPG;
	p.nQuality = (UINT)quality;

	char* mem = imageMem;
	UINT id = (UINT)memId;

	p.ppcImageMem = &mem;
	p.pnImageID = &id;

	return is_ImageFile(cam, IS_IMAGE_FILE_CMD_SAVE, &p, sizeof(p)) == IS_SUCCESS;
}

bool IDScamManager::SnapshotCopyTo(std::vector<uint8_t>& out)
{
	if (!connected)
		return false;

	int bytesPerPixel = (bpp + 7) / 8;
	int rowBytes = width * bytesPerPixel;

	out.resize((size_t)height * (size_t)rowBytes);

	const uint8_t* src = reinterpret_cast<const uint8_t*>(imageMem);
	if (!src)
		return false;

	for (int y = 0; y < height; ++y)
	{
		const uint8_t* s = src + (size_t)y * (size_t)pitch;
		uint8_t* d = out.data() + (size_t)y * (size_t)rowBytes;
		std::memcpy(d, s, (size_t)rowBytes);
	}
	return true;
}

bool IDScamManager::StartRecording(const wchar_t* path, double fps, int quality)
{
	if (!connected || recording)
		return false;

	int bytesPerPixel = (bpp + 7) / 8;
	int bytesPerLine = width * bytesPerPixel;
	int lineOffset = pitch - bytesPerLine;
	if (lineOffset < 0) lineOffset = 0;

	
	CheckAvi(isavi_SetImageSize(aviId, IS_AVI_CM_Y8, width, height, 0, 0, lineOffset));
	CheckAvi(isavi_OpenAVIW(aviId, path));
	CheckAvi(isavi_SetFrameRate(aviId, fps));
	if (quality >= 1 && quality <= 100)
		CheckAvi(isavi_SetImageQuality(aviId, quality));
	CheckAvi(isavi_StartAVI(aviId));

	recording = true;
	return true;
}

void IDScamManager::StopRecording()
{
	if (!recording)
		return;

	isavi_StopAVI(aviId);
	isavi_CloseAVI(aviId);
	recording = false;
}

bool IDScamManager::AllocateImageMem()
{
	FreeImageMem();

	IS_RECT aoi{};
	if (is_AOI(cam, IS_AOI_IMAGE_GET_AOI, &aoi, sizeof(aoi)) != IS_SUCCESS)
		return false;

	width = aoi.s32Width;
	height = aoi.s32Height;
	bpp = 8; // MONO8

	if (is_AllocImageMem(cam, width, height, bpp, &imageMem, &memId) != IS_SUCCESS)
		return false;

	if (is_SetImageMem(cam, imageMem, memId) != IS_SUCCESS)
		return false;

	int iw = 0, ih = 0, ibpp = 0, p = 0;
	if (is_InquireImageMem(cam, imageMem, memId, &iw, &ih, &ibpp, &p) != IS_SUCCESS)
		return false;

	pitch = p;
	return true;
}

void IDScamManager::FreeImageMem()
{
	if (!imageMem)
		return;

	is_FreeImageMem(cam, imageMem, memId);
	imageMem = nullptr;
	memId = 0;
}

void IDScamManager::CheckAvi(int ret)
{
	if (ret == IS_AVI_NO_ERR)
		return;

	std::cerr << "[AVI ERROR] ret=" << ret << "\n";
	std::exit(1);
}


////////////////////////////////////////////////////////////
///			세타테이블 컨트롤 관련
////////////////////////////////////////////////////////////
AFstate* AutoFocus::GetAFState(int phIndex)
{
	auto value = afState.find(phIndex);
	return value == afState.end() ? nullptr : &value->second;
}

bool AutoFocus::InitFocusDrive()
{
	uvEng_Luria_ReqSetMotorPositionInitAll();
	Sleep(3000);
	bool res = GlobalVariables::GetInstance()->Waiter([&]()->bool
		{
			return uvCmn_MC2_IsMotorDriveStopAll() && uvCmn_Luria_GetLastErrorStatus() != 10079;
		},10000);

	uvEng_Luria_GetShMem()->focus.initialized = res ? 1 : 0;
	return res;
}

//set
bool AutoFocus::SetAFSensorOnOff(int phNum, bool on)
{
	auto* value = GetAFState(phNum);
	return value == nullptr ? false : value->SetAFSensorOnOff(on);
}

bool AutoFocus::SetStoredAFPosition(int phNum, int position)
{
	auto* value = GetAFState(phNum);
	return value == nullptr ? false : value->SetStoredAFPosition(position);
}

bool AutoFocus::SetAFSensorType(int phNum, AFstate::LDStype type)
{
	auto* value = GetAFState(phNum);
	return value == nullptr ? false : value->SetAFSensorType(type);
}


bool AutoFocus::SetAFOnOff(int phNum, bool on)
{
	auto* value = GetAFState(phNum);
	return value == nullptr ? false : value->SetAFOnOff(on);
}

bool AutoFocus::SetPlottingOnOff(int phNum, bool on)
{
	auto* value = GetAFState(phNum);
	return value == nullptr ? false : value->SetPlottingOnOff(on);
}


//get

bool AutoFocus::GetPlottingIsOn(int phNum, bool& on)
{
	auto* value = GetAFState(phNum);
	return value == nullptr ? false : value->GetPlottingIsOn(on);
}

bool AutoFocus::GetAFSensorIsOn(int phNum, bool& on)
{
	auto* value = GetAFState(phNum);
	return value == nullptr ? false : value->GetAFSensorIsOn(on);
}

bool AutoFocus::GetCurrentAFSensingPosition(int phNum, int& position)
{
	auto* value = GetAFState(phNum);
	return value == nullptr ? false : value->GetCurrentAFSensingPosition(position);
}

bool AutoFocus::GetStoredAFPosition(int phNum, int& position)
{
	auto* value = GetAFState(phNum);
	return value == nullptr ? false : value->GetStoredAFPosition(position);
}

bool AutoFocus::GetAFSensorType(int phNum, AFstate::LDStype& type)
{
	auto* value = GetAFState(phNum);
	return value == nullptr ? false : value->GetAFSensorType(type);
}

bool AutoFocus::GetAFisOn(int phNum, bool& on)
{
	auto* value = GetAFState(phNum);
	return value == nullptr ? false : value->GetAFisOn(on);
}

bool AutoFocus::GetAFWorkRange(int phNum, int& below, int& above)
{
	auto* value = GetAFState(phNum);
	return value == nullptr ? false : value->GetAFWorkRange(below,above);
}


//set

bool AutoFocus::SetAFWorkRange(int phNum, int below, int above)
{
	auto* value = GetAFState(phNum);
	return value == nullptr ? false : value->SetAFWorkRange(below,above);
}


bool AFstate::SetAFSensorOnOff(bool on)
{
	uvEng_Luria_GetShMem()->ResetLastRecvCmd();
	bool res = uvEng_Luria_ReqSetAFSensorOnOff(phIndex, on);
	
	if(res)
	res = GlobalVariables::GetInstance()->Waiter([&]()->bool
		{
			return uvEng_Luria_GetShMem()->IsRecvCmd((UINT8)ENG_LUDF::en_direct_photo_comm,1);
		});

	this->isSensorOn = res ? on : this->isSensorOn;
	return res;
}

bool AFstate::SetAFSensorType(LDStype type)
{
	uvEng_Luria_GetShMem()->ResetLastRecvCmd();
	uvEng_Luria_ReqSetAFSensorType(phIndex, (UINT8)type);

	bool res = GlobalVariables::GetInstance()->Waiter([&]()->bool
		{
			return uvEng_Luria_GetShMem()->IsRecvCmd((UINT8)ENG_LUDF::en_direct_photo_comm, 1);
		});
	this->ldsType = res ? type : this->ldsType;
	return res;
}

bool AFstate::SetStoredAFPosition(int position)
{
	uvEng_Luria_GetShMem()->ResetLastRecvCmd();
	uvEng_Luria_ReqSetStoredAutofocusPosition(phIndex, position);

	bool res = GlobalVariables::GetInstance()->Waiter([&]()->bool
		{
			return uvEng_Luria_GetShMem()->IsRecvCmd((UINT8)ENG_LUDF::en_direct_photo_comm, 1);
		});
	this->storedAFValue = res ? position : this->storedAFValue;
	return res;
}

bool AFstate::SetAFOnOff(bool on)
{
	uvEng_Luria_GetShMem()->ResetLastRecvCmd();
	uvEng_Luria_ReqSetAutoFocusLM(phIndex, on ? 1 : 0);

	bool res = GlobalVariables::GetInstance()->Waiter([&]()->bool
		{
			return uvEng_Luria_GetShMem()->IsRecvCmd((UINT8)ENG_LUDF::en_focus,
													 (UINT8)ENG_LCPF::en_auto_focus);
			
		});

	this->isAFOn = res ? on : this->isAFOn;
	return res;
}

bool AFstate::SetPlottingOnOff(bool on)
{
	
	uvEng_Luria_GetShMem()->ResetLastRecvCmd();
	uvEng_Luria_ReqSetLineSensorPlot(phIndex, on);

	bool res = GlobalVariables::GetInstance()->Waiter([&]()->bool
		{
			return uvEng_Luria_GetShMem()->IsRecvCmd((UINT8)ENG_LUDF::en_direct_photo_comm, 1);
		});
	this->isPlottingOn = res ? on : this->isPlottingOn;
	return res;
}

//get
bool AFstate::GetPlottingIsOn(bool& on)
{
	uvEng_Luria_GetShMem()->ResetLastRecvCmd();
	uvEng_Luria_ReqGetLineSensorPlot(phIndex);

	bool res = GlobalVariables::GetInstance()->Waiter([&]()->bool
		{
			return uvEng_Luria_GetShMem()->IsRecvCmd((UINT8)ENG_LUDF::en_direct_photo_comm,
			(UINT8)ENG_LLRN::en_res_plot_enable);
		});
	
	this->isPlottingOn = res ? uvEng_Luria_GetShMem()->directph.line_sensor_plot[phIndex-1] == 0 ? false : true : this->isPlottingOn;
	on = this->isPlottingOn;
	return res;
}

bool AFstate::GetAFSensorIsOn(bool& on)
{
	uvEng_Luria_GetShMem()->ResetLastRecvCmd();
	uvEng_Luria_ReqGetAFSensorOnOff(phIndex);

	bool res = GlobalVariables::GetInstance()->Waiter([&]()->bool
		{
			/*if (uvEng_Luria_IsRecvPktData(ENG_FDPR::en_luria_direct_ph));
			return uvEng_Luria_GetShMem()->directph.get_last_received_record_id == (UINT16)ENG_LLRN::ReplyLaserMode;
			return false;*/

			return uvEng_Luria_GetShMem()->IsRecvCmd((UINT8)ENG_LUDF::en_direct_photo_comm, 1);
		});

	this->isSensorOn = res ? uvEng_Luria_GetShMem()->directph.AFSensorState[0] == 0 ? false : true : this->isSensorOn;
	on = this->isSensorOn;
	return res;
}

bool AFstate::GetCurrentAFSensingPosition(int& position)
{
	uvEng_Luria_GetShMem()->ResetLastRecvCmd();
	uvEng_Luria_ReqGetCurrentAutofocusPosition(phIndex);

	bool res = GlobalVariables::GetInstance()->Waiter([&]()->bool
		{
			return uvEng_Luria_GetShMem()->IsRecvCmd((UINT8)ENG_LUDF::en_direct_photo_comm, 1);
		},5000);

	this->sensingAFValue = res ? uvEng_Luria_GetShMem()->directph.auto_focus_position[0] : this->sensingAFValue;
	position = this->sensingAFValue;
	return res;
}

bool AFstate::GetStoredAFPosition(int& position)
{
	uvEng_Luria_GetShMem()->ResetLastRecvCmd();
	uvEng_Luria_ReqGetStoredAutofocusPosition(phIndex);

	bool res = GlobalVariables::GetInstance()->Waiter([&]()->bool
		{
			return uvEng_Luria_GetShMem()->IsRecvCmd((UINT8)ENG_LUDF::en_direct_photo_comm, 1);
		});

	this->storedAFValue = res ? uvEng_Luria_GetShMem()->directph.focus_position[0] : this->storedAFValue;
	position = this->storedAFValue;
	return res;
}

bool AFstate::GetAFSensorType(LDStype& type)
{
	if (this->ldsType != LDStype::none)
	{
		type = this->ldsType;
		return true;
	}

	uvEng_Luria_GetShMem()->ResetLastRecvCmd();
	uvEng_Luria_ReqGetAFSensorType(phIndex);

	bool res = GlobalVariables::GetInstance()->Waiter([&]()->bool
		{
			return uvEng_Luria_GetShMem()->IsRecvCmd((UINT8)ENG_LUDF::en_direct_photo_comm,1);

		/*	if (uvEng_Luria_IsRecvPktData(ENG_FDPR::en_luria_direct_ph));
			return uvEng_Luria_GetShMem()->directph.get_last_received_record_id == (UINT16)ENG_LLRN::ReplyAfLineSensor;
			return false;*/
		});

	this->ldsType =  res ? (LDStype)uvEng_Luria_GetShMem()->directph.AFSensorType[0] : this->ldsType;
	type = this->ldsType;
	return res;
}

bool AFstate::GetAFWorkRange(int& below, int& above)
{
	uvEng_Luria_GetShMem()->ResetLastRecvCmd();
	uvEng_Luria_ReqGetAbsWorkRange(phIndex);

	bool res = GlobalVariables::GetInstance()->Waiter([&]()->bool
		{
			return uvEng_Luria_GetShMem()->IsRecvCmd((UINT8)ENG_LUDF::en_focus,
			(UINT8)ENG_LCPF::en_abs_work_range);
			
		});

	this->AFworkRange[0] = res ? uvEng_Luria_GetShMem()->focus.abs_work_range_min[phIndex - 1] : this->AFworkRange[0];
	this->AFworkRange[1] = res ? uvEng_Luria_GetShMem()->focus.abs_work_range_max[phIndex - 1] : this->AFworkRange[1];
	below = this->AFworkRange[0];
	above = this->AFworkRange[1];
	return res;

}

bool AFstate::SetAFWorkRange( int below, int above)
{
	uvEng_Luria_GetShMem()->ResetLastRecvCmd();
	uvEng_Luria_ReqSetAbsWorkRangeLM(phIndex,below,above);

	bool res = GlobalVariables::GetInstance()->Waiter([&]()->bool
		{
			return uvEng_Luria_GetShMem()->IsRecvCmd((UINT8)ENG_LUDF::en_focus,
			(UINT8)ENG_LCPF::en_abs_work_range);
			
		});

	this->AFworkRange[0] = res ? below : this->AFworkRange[0];
	this->AFworkRange[1] = res ? above : this->AFworkRange[1];

	return res;
}

bool AFstate::GetAFisOn(bool& on)
{

	uvEng_Luria_GetShMem()->ResetLastRecvCmd();
	uvEng_Luria_ReqGetAutoFocus(phIndex);

	bool res = GlobalVariables::GetInstance()->Waiter([&]()->bool
		{
			return uvEng_Luria_GetShMem()->IsRecvCmd((UINT8)ENG_LUDF::en_focus, 
													 (UINT8)ENG_LCPF::en_auto_focus);
		});
	
	this->isAFOn = res ? (bool)uvEng_Luria_GetShMem()->focus.auto_focus[phIndex] : this->isAFOn;
	on = this->isAFOn;
	return res;
}

