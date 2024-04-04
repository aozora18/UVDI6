#pragma once
//이하는 전부 인라인 클래스임.



#include <iostream>
#include <filesystem>
#include <chrono>


#include <map>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <functional>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <unordered_map>
#include <atomic>
#include <algorithm>
#include <stack>
#include <assert.h>
#include <fstream>
#include <regex>

//아이고.... 뭐가 계속 추가되네...
#include <filesystem>
#include "../../inc/conf/conf_uvdi15.h"
#include "../../inc/conf/conf_comn.h"
#include "../../inc/conf/luria.h"
#include "../../inc/itfe/EItfcMC2.h"
#include "../../inc/itfe/EItfcCamera.h"
#include "../../inc/recipe/ACamCali.h"
#include "../../prj/ItfcLuria/JobSelectXml.h"
#include "../../inc/conf/recipe_uvdi15.h"
#include "../../inc/itfe/EItfcRcpUVDI15.h"
#include "../../inc/itfe/EItfcThickCali.h"

using namespace std;
namespace fs = std::filesystem;

struct CaliPoint;

	template <typename T>
	class Singleton
	{
	public:
		static T& getInstance()
		{
			static T instance;
			return instance;
		}

		//Singleton(const Singleton&) = delete;
		//Singleton& operator=(const Singleton&) = delete;
		//Singleton(Singleton&&) = delete;
		//Singleton& operator=(Singleton&&) = delete;
	};


	// 자주쓰게될것같아 래퍼클래스로 만듬. CV이용
	class ConditionWrapper
	{
	public:
		ConditionWrapper() : isConditionMet(false) {}

		bool WaitFor(std::chrono::milliseconds timeout)
		{
			std::unique_lock<std::mutex> lock(mtx);
			auto result = cv.wait_for(lock, timeout, [this] { return isConditionMet; });
			return result;
		}

		void Notify(bool res)
		{
			std::lock_guard<std::mutex> lock(mtx);
			isConditionMet = res;
			cv.notify_one();
		}

		void Reset()
		{
			std::lock_guard<std::mutex> lock(mtx);
			isConditionMet = false;
		}

	private:
		std::mutex mtx;
		std::condition_variable cv;
		bool isConditionMet;

	};


	class ThreadManager : public Singleton<ThreadManager>
	{
	public:
		void addThread(const std::string& key, std::function<void()> callback)
		{
			if (threads_.find(key) != threads_.end())
				return;

			std::lock_guard<std::mutex> lock(mutex_);
			threads_.emplace(key, std::make_unique<std::thread>(callback));
		}

		void removeThread(const std::string& key)
		{
			std::lock_guard<std::mutex> lock(mutex_);
			auto it = threads_.find(key);
			if (it != threads_.end()) {
				if (it->second->joinable()) {
					it->second->join();
				}
				threads_.erase(it);
			}
		}

		void waitForAllThreads()
		{
			std::lock_guard<std::mutex> lock(mutex_);
			for (auto& pair : threads_) {
				if (pair.second->joinable()) {
					pair.second->join();
				}
			}
			threads_.clear();
		}

		bool isThreadRunning(const std::string& key)
		{
			std::lock_guard<std::mutex> lock(mutex_);
			auto it = threads_.find(key);
			return it != threads_.end() && it->second->joinable();
		}


	private:

		std::unordered_map<std::string, std::unique_ptr<std::thread>> threads_;
		std::mutex mutex_;
	};

 
	class caliCalc
	{
	public:
		bool caliInfoLoadComplete = false;
	
		struct CaliPoint
		{
			CaliPoint(double x,
				double y,
				double offsetX,
				double offsetY)
			{
				this->x = x;
				this->y = y;
				this->offsetX = offsetX;
				this->offsetY = offsetY;
			}
			CaliPoint() {}
			double x;
			double y;
			double offsetX;
			double offsetY;
		} data;

	protected:
		map<int, vector<CaliPoint>> caliDataMap;

		double LimittoMicro(double val)
		{
			return round(val * 1000.0) / 1000.0; //마이크로 단위 이하는 필요가 없음. 
		}

		void SortPos(std::vector<CaliPoint>& dataList)
		{
			std::sort(dataList.begin(), dataList.end(), [](const CaliPoint& a, const CaliPoint& b)
				{
					return a.y == b.y ? a.x < b.x : a.y < b.y;
				});
		}

	
		CaliPoint calculateAverageOffset(const std::vector<CaliPoint>& nearbyPoints)
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
	public:
		void LoadCaliData(LPG_CIEA cfg)
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


			auto loadSeq = [=](LPG_CIEA cfg)
				{
					const int DATACOUNT = 4;
					const std::regex re(R"([\s|,]+)");

					//TCHAR			workDir[MAX_PATH] = { NULL };
					
					
					//auto executablePath = std::filesystem::current_path();
					//auto directoryOnly = executablePath.parent_path();

					const int XCoord = 0, YCoord = 1, OffsetX = 2, OffsetY = 3;
					int camCount = cfg->set_cams.acam_count;
					for (int i = 0; i < camCount; i++)
					{
						try
						{
							auto name = cfg->file_dat.staticAcamCali[i];
						//	swprintf_s(workDir, MAX_PATH, L"%s\\%s", directoryOnly.c_str(), name);
							std::ifstream file(name);
							
							for (std::string line; std::getline(file, line);)
							{
								const std::vector<double> tokens = tokenize(line, re);
								if (tokens.size() != DATACOUNT) continue;

								caliDataMap[i+1].push_back(CaliPoint(tokens[XCoord], tokens[YCoord], tokens[OffsetX], tokens[OffsetY]));
							}
							if(caliDataMap[i + 1].size() != 0)
								SortPos(caliDataMap[i]);
							
						}
						catch (...)
						{
							caliInfoLoadComplete = false;
						}
						caliInfoLoadComplete = true;
					}
				};

			if(caliInfoLoadComplete == false)
				std::thread([=]() {loadSeq(cfg); }).detach();
		}


		CaliPoint EstimateOffset(int camIdx,  double stageX=0, double stageY=0,double camX=-1)
		{
			const int STAGE_CALI_INDEX = 3;
			auto stageCaliData = caliDataMap[STAGE_CALI_INDEX];
			auto camCaliData = caliDataMap[camIdx];

			auto estimate = [=](vector<CaliPoint> points, double x, double y) -> CaliPoint
			{
				#undef max
				double closestDistance = std::numeric_limits<double>::max();
				double secondClosestDistance = std::numeric_limits<double>::max();

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

				double totalInverseDistance = (1 / closestDistance) + (1 / secondClosestDistance);
				double weightClosest = (1 / closestDistance) / totalInverseDistance;
				double weightSecondClosest = (1 / secondClosestDistance) / totalInverseDistance;

				CaliPoint weightedAverageOffset;
				weightedAverageOffset.x = x;
				weightedAverageOffset.y = y;
				weightedAverageOffset.offsetX = (closestPoint.offsetX * weightClosest) + (secondClosestPoint.offsetX * weightSecondClosest);
				weightedAverageOffset.offsetY = (closestPoint.offsetY * weightClosest) + (secondClosestPoint.offsetY * weightSecondClosest);

				return weightedAverageOffset;
			};
			
			CaliPoint stageOffset = estimate(stageCaliData, stageX, stageY);
			CaliPoint camOffset = estimate(camCaliData, camX, -1);

			return CaliPoint(0, 0, stageOffset.offsetX + camOffset.x, stageOffset.offsetY + camOffset.y);
		}

	};

	class AlignMotion
	{
	public:
		mutex* motionMutex;
		bool onUpdate = true;
		LPG_CIEA pstCfg = nullptr;
		
		void Destroy()
		{
			lock_guard<mutex> lock(*motionMutex);
			onUpdate = false;
			motions.clear();
		}

		enum MovingDir
		{
			X,
			Y,
			Z,
			T,
			STATIC,
		};

		enum Parts
		{
			none =	 0b00000000,
			camera = 0b00000001,
			etc =    0b00000010,
		};

		caliCalc::CaliPoint EstimateOffset(int camIdx, double stageX = 0, double stageY = 0, double camX = -1)
		{
			 return caliCalcInst.EstimateOffset(camIdx, stageX, stageY, camX);
		}

		class Axis //이동가능한축만 등록하며, 특정기능을 담당한다면 옵션추가한다. 
		{
		public:
			string name;
			MovingDir direction;
			int parts;
			double min; //mm
			double max; //mm
			double currPos;
			bool onError;
			bool centerCam;

			std::function<double()> posCallback;
			std::function<BOOL()> stateCallback;

			Axis() {}

			Axis(string name, MovingDir dir, int parts, double min, double max,
				std::function<double()> posCallback,
				std::function<BOOL()> stateCallback)
			{
				this->min = min;
				this->max = max;
				this->name = name;
				this->parts = parts;
				this->direction = dir;
				this->posCallback = posCallback;
				this->stateCallback = stateCallback;
				onError = stateCallback != nullptr ? stateCallback() : false; //기본적으로 에러는 없다고 가정.
				currPos = posCallback != nullptr ? posCallback() : min;
			}
		};
	protected:
		caliCalc caliCalcInst;
		map<string, map<string, Axis>> motions;
		
	public:
		
		map<string, map<string, Axis>> GetMotion() {return motions;}
		/// </summary>
		/// <param name="camIndex"></param>
		/// <param name="camXPos"></param>
		/// <param name="stageX"></param>
		/// <param name="stageY"></param>
		/// <param name="X"></param>
		/// <param name="Y"></param>
		/// <returns></returns>
		//STG_XMXY GetGerberPosUsePosition(int camIndex, double camXPos, double stageX, double stageY, double& X, double& Y)
		//{
		//	//역산으로 주어진 값으로 피두셜 얻어오기. 오차값이 좀 있긴한데 대략적인 값을 잘 반환하고있다. 

		//	double tempGerberX = markParams.mark2x + (camXPos - markParams.cam1xLookatMark2) + (stageX - markParams.stageXLookatMark2WithCam1) + (camIndex == 1 ? 0 : markParams.distC2C);
		//	double tempGerberY = markParams.mark2y + (stageY - markParams.stageYLookatMark2WithCam1);

		//	//각 카메라별로 설치오차 회전값 적용필
		//	// ---->  calcParam.Cam1PhyTCorrectAngle, calcParam.Cam2PhyTCorrectAngle;


		//	double thresholdVal = markParams.threshold * pstCfg->acam_spec.GetPixelToMM(camIndex); //대충 2pxl이내에서 검색이 되야한다. 

		//		vector<STG_XMXY>::iterator  it = find_if(status.markList[ENG_AMTF::en_local].begin(), status.markList[ENG_AMTF::en_local].end(),
		//		[&](const STG_XMXY& elem)
		//		{
		//			double distance = std::sqrt(std::pow(tempGerberX - elem.mark_x, 3) + std::pow(tempGerberY - elem.mark_y, 3));
		//			return distance <= thresholdVal; //오차값이내
		//		});

		//	if (it != status.markList[ENG_AMTF::en_local].end())
		//	{
		//		X = tempGerberX;
		//		Y = tempGerberY;
		//	}

		//	return it == status.markList[ENG_AMTF::en_local].end() ?  STG_XMXY() : *it;
		//}


		/// <summary>
		/// 
		/// </summary>
		//bool GetMoveMotionOffset(int camIndex, double gerberPosX, double gerberPosY, double& stageX, double& stageY, double& camX)
		//{
		//	auto AreEqual = [&](double a, double b, int precision = 4)->bool
		//		{
		//			double epsilon = std::pow(10, -precision);
		//			return std::fabs(a - b) < epsilon;
		//		};


		//	double lookatX = 0, lookatY = 0;
		//	GetCamLookatGerberPos(camIndex, lookatX, lookatY); //현재 카메라가 보고있는 거버좌표 얻어옴.

		//	string camStr = "x" + std::to_string(camIndex);

		//	double maxRange = GetCamMaxRangeXInGerber(camIndex);
		//	double minRange = GetCamMinRangeXInGerber(camIndex);

		//	double clamped = std::clamp(gerberPosX, minRange, maxRange);

		//	bool inRange = AreEqual(gerberPosX, clamped); //x가 이동가능한 범위인가? y는 따로 검사하지 않는다. 
		//	if (inRange == false) return false;

		//	//Refresh(); <- 이동중에 계산되야한다면 한번 호출해줘야한다. 왜냐면 1초에 한번씩 갱신이라서.
		//	stageX = motions["stage"]["x"].currPos;
		//	camX = motions["camera"][camStr].currPos + (gerberPosX - lookatX);
		//	stageY = motions["stage"]["y"].currPos + (gerberPosY - lookatY);

		//	return true;

		//	/*stageX
		//	stageY
		//	camX*/


		//}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="camIndex"></param>
		/// <param name="markType"></param>
		/// <param name="markIndex"></param>
		/// <param name="stageX"></param>
		/// <param name="stageY"></param>
		/// <param name="camX"></param>
		/// <returns></returns>
		/*bool GetMoveMotionOffset(int camIndex, ENG_AMTF markType, int markIndex, double& stageX, double& stageY, double& camX)
		{
			
			vector<STG_XMXY>::iterator it = std::find_if(status.markList[markType].begin(),
				status.markList[markType].end(),
														[&](const STG_XMXY& XMXY)
														{
															return XMXY.tgt_id == markIndex;
														});

			if (it == status.markList[markType].end()) return false;

			

			return GetMoveMotionOffset(camIndex, it->mark_x, it->mark_y, stageX, stageY, camX);
		}*/

		/// <summary>
		/// 
		/// </summary>
		/// <param name="camIndex"></param>
		/// <param name="X"></param>
		/// <param name="Y"></param>
		//void GetCamLookatGerberPos(int camIndex, double& X, double& Y)
		//{
		//	double stageX = motions["stage"]["x"].currPos;
		//	double stageY = motions["stage"]["y"].currPos;

		//	string camName = "x" + std::to_string(camIndex);

		//	double camX = motions["camera"][camName].currPos;

		//	double tempX, tempY;
		//	//tempX = calcParam.mark2x + (stageX - calcParam.stageXLookatMark2WithCam1) + (camIndex == 1 ? camX - calcParam.cam1xLookatMark2 : calcParam.distC2C + camX);
		//	tempX = markParams.mark2x + (camIndex == 1 ? 0 : markParams.distC2C) + (stageX - markParams.stageXLookatMark2WithCam1) + (camIndex == 1 ? camX - markParams.cam1xLookatMark2 : camX);
		//	tempY = markParams.mark2y + (stageY - markParams.stageYLookatMark2WithCam1);

		//	//각 카메라별로 설치오차 회전값 적용필
		//	// ---->  calcParam.Cam1PhyTCorrectAngle, calcParam.Cam2PhyTCorrectAngle;

		//	X = tempX;
		//	Y = tempY;

		//}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="camIndex"></param>
		/// <returns></returns>
		//double GetCamMinRangeXInGerber(int camIndex)
		//{

		//	string camName = "x" + std::to_string(camIndex);

		//	double camX = motions["camera"][camName].currPos;
		//	double minXLimCam = motions["camera"][camName].min;
		//	double maxXLimCam = motions["camera"][camName].max;

		//	double stageX = motions["stage"]["x"].currPos;
		//	double minXLimStage = motions["stage"]["x"].min;

		//	//캠이 현재 커버가능한 거버상의 X좌표 최저값
		//	double temp = markParams.mark2x + (camIndex == 1 ? 0 : markParams.distC2C) - (camX - minXLimCam) + (markParams.camMoveOnly ? 0 : (stageX - minXLimStage));
		//	return temp;
		//}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="camIndex"></param>
		/// <returns></returns>
		//double GetCamMaxRangeXInGerber(int camIndex)
		//{
		//	string camName = "x" + std::to_string(camIndex);
		//	double camX = motions["camera"][camName].currPos;
		//	double minXLimCam = motions["camera"][camName].min;
		//	double maxXLimCam = motions["camera"][camName].max;

		//	double stageX = motions["stage"]["x"].currPos;
		//	double minXLimStage = motions["stage"]["x"].min;
		//	double maxXLimStage = motions["stage"]["x"].max;

		//	//캠이 현재 커버가능한 거버상의 X좌표 최대값
		//	double temp = markParams.mark2x + (camIndex == 2 ? markParams.cam1xLookatMark2 + markParams.distC2C : 0) + (maxXLimCam - camX) + (markParams.camMoveOnly ? 0 : (maxXLimStage - stageX));
		//	return temp;
		//}

		//////////////////////////////////////////////////////////


		void Update()
		{
			const int updateDelay = 1000;
			while (onUpdate)
			{
				Refresh();
				this_thread::sleep_for(chrono::milliseconds(updateDelay));
			}
		}

	public:

		void Refresh() //바로 갱신이 필요하면 요거 다이렉트 호출 보통은 스레드에서 호출
		{
			lock_guard<mutex> lock(*motionMutex);
			for (auto i = motions.begin(); i != motions.end(); i++)
				for (auto j = i->second.begin(); j != i->second.end(); j++)
				{
					if (j->second.stateCallback != nullptr)
						j->second.onError = j->second.stateCallback();

					if (j->second.onError == false && j->second.posCallback != nullptr)
						j->second.currPos = j->second.posCallback();
				}
		}

		void LoadCaliData(LPG_CIEA cfg)
		{
			caliCalcInst.LoadCaliData(cfg);
		}
		
		enum SearchFlag
		{
			global,
			local,
			all,
		};

		bool GetNearFid(STG_XMXY currentPos, SearchFlag flag,vector<STG_XMXY> skipList, STG_XMXY& findFid)
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
				auto it = std::find_if(temp.begin(), temp.end(), [&](const STG_XMXY& c) { return c.org_id == item.org_id && c.org_id == item.org_id && c.org_id == item.org_id; });
				if (it != temp.end()) 
					temp.erase(it);
			}

			auto FindNear = [&](vector<STG_XMXY> posList, STG_XMXY pos) -> int
			{
				double minDist = std::numeric_limits<double>::max();
				int idx = -1;

				for (int i=0;i<posList.size();i++) 
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

			if (auto findIdx = FindNear(temp, currentPos) != -1)
			{
				findFid = temp[findIdx];
				return true;
			} 
			return false;
		}


		bool MovetoGerberPos(int camNum, STG_XMXY tgtPos)
		{
			Refresh();
			//캠1,2은 캠과 스테이지가 동시에 
			//캠3은 스테이지만 
			//자. 단순하다. 
			//현재 캠이 보고있는 거버내 좌표위치와 가야할 거버위치의 차를 이용해 이동해야할 총량을 구한다. 
			STG_XMXY gbrPos;
			GetGerberPosUseCamPos(camNum, gbrPos);

			int dx = gbrPos.mark_x - tgtPos.mark_x;
			int dy = gbrPos.mark_y - tgtPos.mark_y;

			//pstCfg->
			//가급적 스테이지를 먼저 움직이고

			//스테이지를 움직일수가 없다면 카메라축을 움직인다. 
			
		}


		bool GetGerberPosUseCamPos(int camNum, STG_XMXY& point)
		{
			const int _1to3 = 0;
			const int _3to2 = 1;

			double mark2Xgab = (markParams.currGerbermark2x - markParams.caliGerbermark2x);
			double mark2Ygab = (markParams.currGerbermark2y - markParams.caliGerbermark2y);

			double stageXgab = (motions["stage"]["x"].currPos - markParams.mark2StageX);
			double stageYgab = (motions["stage"]["y"].currPos - markParams.mark2cam1Y);

			double camPos[] = {motions["camera"]["x1"].currPos ,motions["camera"]["x2"].currPos};

			double camOffset = camNum == 1 ? camPos[0] - markParams.mark2Cam1X :
							camNum == 2 ? (markParams.distCam2cam[_1to3] - camPos[0]) + (markParams.distCam2cam[_3to2] + camPos[0]) :
							camNum == 3 ? markParams.distCam2cam[0] - camPos[0] : 0;

		
			//역산시작
			double tempGerberX = markParams.currGerbermark2x + mark2Xgab + stageXgab + camOffset;
			double tempGerberY = markParams.currGerbermark2y + mark2Ygab + stageYgab;

			point.mark_x = tempGerberX;
			point.mark_y = tempGerberY;
			return true;


			////역산으로 주어진 값으로 피두셜 얻어오기. 오차값이 좀 있긴한데 대략적인 값을 잘 반환하고있다. 

			//double tempGerberX = markParams.mark2x + (camXPos - markParams.cam1xLookatMark2) + (stageX - markParams.stageXLookatMark2WithCam1) + (camIndex == 1 ? 0 : markParams.distC2C);
			//double tempGerberY = markParams.mark2y + (stageY - markParams.stageYLookatMark2WithCam1);

			////각 카메라별로 설치오차 회전값 적용필
			//// ---->  calcParam.Cam1PhyTCorrectAngle, calcParam.Cam2PhyTCorrectAngle;


			//double thresholdVal = markParams.threshold * pstCfg->acam_spec.GetPixelToMM(camIndex); //대충 2pxl이내에서 검색이 되야한다. 

			//	vector<STG_XMXY>::iterator  it = find_if(status.markList[ENG_AMTF::en_local].begin(), status.markList[ENG_AMTF::en_local].end(),
			//	[&](const STG_XMXY& elem)
			//	{
			//		double distance = std::sqrt(std::pow(tempGerberX - elem.mark_x, 3) + std::pow(tempGerberY - elem.mark_y, 3));
			//		return distance <= thresholdVal; //오차값이내
			//	});

			//if (it != status.markList[ENG_AMTF::en_local].end())
			//{
			//	X = tempGerberX;
			//	Y = tempGerberY;
			//}

			//return it == status.markList[ENG_AMTF::en_local].end() ?  STG_XMXY() : *it;
		}

		public:
			struct Params 
			{
				double currGerbermark2x = 0, currGerbermark2y = 0;
				double caliGerbermark2x = 0, caliGerbermark2y = 0;
				int threshold = 10;
				double mark2StageX=0;
				double mark2cam1Y = 0;
				double mark2cam2Y = 0;
				double mark2Cam1X = 0;
				double mark2Cam2X = 0;
				double distCam2cam[2] = { 0, };
				vector<tuple<ENG_MMDI, double, double>> axisLimit;

			};

			struct Status
			{
				enum SpiltMode
				{
					byDynamic, //포지션에 따라 동적으로
					byCamCount, //카메라 갯수에 따라서.
				};

				int GetCenterColumn()
				{
					return gerberColCnt / acamCount;
				}

				void Init()
				{
					SpiltMode spiltMode = SpiltMode::byCamCount;
					
					gerberRowCnt = 0;
					gerberColCnt = 0;
					localMarkCnt = 0;
					globalMarkCnt = 0;

					//scanCount = 0;
					//cam1ProcessingColumn = 0;
					//cam2ProcessingColumn = 0;
					//markMapProcess.clear(); //현재 처리된 상태들 (처리된것들은 1818로 변경됨)
					BufferClear();
				}

				void BufferClear()
				{
					markPoolForCamLocal.clear(); //카메라별 풀링.
					markList.clear(); //column별 풀링
					markMapConst.clear(); //원본
				}

				int acamCount = 0;
				//int scanCount = 0;
				int gerberRowCnt = 0;
				int gerberColCnt = 0;
				int localMarkCnt = 0;
				int globalMarkCnt = 0;

				//int cam1ProcessingColumn = 0;
				//int cam2ProcessingColumn = 0;
				int lastScanCount = 0;

				map<int, vector<STG_XMXY>> markPoolForCamGlobal; //카메라별 풀링.
				map<int, vector<STG_XMXY>> markPoolForCamLocal; //카메라별 풀링.
				map<int, vector<STG_XMXY>> markMapConst; //원본인데 스켄라인별로 정렬된것
				//map<int, vector<STG_XMXY>> markMapProcess; //현재 처리된 상태들 (처리된것들은 1818로 변경됨)
				std::map<ENG_AMTF, vector<STG_XMXY>> markList; //글로벌, 로컬 원본인데 맵핑만된것.	
			};

			Status status;
			Params markParams;
	public:
		 

		/// <summary>
		/// 
		/// </summary>
		/// <param name="fiducials"></param>
		/// <param name="x"></param>
		/// <param name="y"></param>
		void GetFiducialDimension(ENG_AMTF types, int& x, int& y)
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

		/// <summary>
		/// 
		/// </summary>
		/// <param name="scanCount"></param>
		/// <returns></returns>
		bool CheckAlignScanFinished(int scanCount)
		{
			return status.lastScanCount <= scanCount ? true : false;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="camIndex"></param>
		/// <param name="startIndex"></param>
		/// <param name="grabList"></param>
		/// <returns></returns>
		int GetFiducialIndex(int camIndex, bool isGlobal , CAtlList <LPG_ACGR>* grabList)
		{				 
			auto grabData = grabList;
			const int GLOBAL_FIDUCIAL_OFFSET = -1;

			if ((grabData == NULL || grabData->GetCount() == 0))
				return -1818;

			std::map<int, vector<LPG_ACGR>> imgMap;

			for (int i = isGlobal ? 0 : status.globalMarkCnt; i < grabData->GetCount(); i++)
			{
				auto val = grabData->GetAt(grabData->FindIndex(i));
				imgMap[val->cam_id].push_back(val);
			}

			auto pool = isGlobal ? status.markPoolForCamGlobal[camIndex] : status.markPoolForCamLocal[camIndex];

			int currentCnt = imgMap[camIndex].size();
			int poolSize = pool.size();

			if (poolSize <= currentCnt-1 || poolSize == 0)
				return -1818;

			return pool[currentCnt - 1].tgt_id *= isGlobal ? -1 : 1;
		}


		void UpdateParamValues()
		{
			LPG_RJAF job = uvEng_JobRecipe_GetSelectRecipe();
			if (job == nullptr) return;

			LPG_MACP thick = uvEng_ThickCali_GetRecipe(job->cali_thick);
			if (thick == nullptr) return;
			
			//double mark2x = 0, mark2y = 0;
			//int threshold = 10;
			//double mark2StageX = 0;
			//double mark2StaegY = 0;
			//double mark2Cam1X = 0;
			//double mark2Cam2X = 0;
			//double distCam2cam[2] = { 0, };
			
			const int _1to3 = 0;
			const int _2to3 = 1;
			markParams.distCam2cam[_1to3] = pstCfg->set_align.distCam2Cam[_1to3];
			markParams.distCam2cam[_2to3] = pstCfg->set_align.distCam2Cam[_2to3];
			markParams.mark2StageX = pstCfg->set_align.table_unloader_xy[0][0];
			
			markParams.caliGerbermark2x = pstCfg->set_align.mark2_org_gerb_xy[0];
			markParams.caliGerbermark2y = pstCfg->set_align.mark2_org_gerb_xy[1];

			markParams.mark2cam1Y = thick->mark2_stage_y[0];
			markParams.mark2cam2Y = thick->mark2_stage_y[0];
			//markParams.mark2StageY = thick->mark2_stage_y[1];
			markParams.mark2Cam1X = thick->mark2_acam_x[0];
			markParams.mark2Cam2X = thick->mark2_acam_x[1];



			
			auto person = std::make_tuple("John", 30);

			// 요소에 이름 지정
			auto [name, age] = person;

			vector<tuple<ENG_MMDI, double, double>> axisLimit =
			{
				{ENG_MMDI::en_stage_x,pstCfg->mc2_svc.min_dist[(UINT8)ENG_MMDI::en_stage_x],pstCfg->mc2_svc.max_dist[(UINT8)ENG_MMDI::en_stage_x]},
				{ENG_MMDI::en_stage_y,pstCfg->mc2_svc.min_dist[(UINT8)ENG_MMDI::en_stage_y],pstCfg->mc2_svc.max_dist[(UINT8)ENG_MMDI::en_stage_y]},
				{ENG_MMDI::en_align_cam1,pstCfg->mc2_svc.min_dist[(UINT8)ENG_MMDI::en_align_cam1],pstCfg->mc2_svc.max_dist[(UINT8)ENG_MMDI::en_align_cam1]},
				{ENG_MMDI::en_align_cam2,pstCfg->mc2_svc.min_dist[(UINT8)ENG_MMDI::en_align_cam2],pstCfg->mc2_svc.max_dist[(UINT8)ENG_MMDI::en_align_cam2]},
			};

		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="globalFiducial"></param>
		/// <param name="localFiducial"></param>
		/// <param name="acamCount"></param>
		void SetFiducial(CFiducialData* globalFiducial, CFiducialData* localFiducial, int acamCount)
		{
			status.BufferClear();
			UpdateParamValues();

			STG_XMXY temp;

			acamCount = 2;

			const UINT32 GLOBAL_FLAG = 0b00000000000000000000000000000001;
			const UINT32 LOCAL_FLAG  = 0b00000000000000000000000000000010;

			for (int i = 0; i < globalFiducial->GetCount(); i++)
				if (globalFiducial->GetMark(i, temp))
				{
					temp.reserve = GLOBAL_FLAG;
					status.markList[ENG_AMTF::en_global].push_back(temp);
					status.markList[ENG_AMTF::en_mixed].push_back(temp);
					
					if (temp.tgt_id == 1)
					{
						markParams.currGerbermark2x = temp.mark_x; //현재 읽은 거버에서의 mark2
						markParams.currGerbermark2y = temp.mark_y;
					}
					
					status.markPoolForCamGlobal[(i/ acamCount)+1].push_back(temp);
				}

			for (int i = 0; i < localFiducial->GetCount(); i++)
				if (localFiducial->GetMark(i, temp))
				{
					temp.reserve = LOCAL_FLAG;
					status.markList[ENG_AMTF::en_local].push_back(temp);
					status.markList[ENG_AMTF::en_mixed].push_back(temp);
				}

			int tempX = 0, tempY = 0;
			GetFiducialDimension(ENG_AMTF::en_local, tempX, tempY);
			status.gerberColCnt = tempX;
			status.gerberRowCnt = tempY;
			status.localMarkCnt = status.markList[ENG_AMTF::en_local].size();
			status.globalMarkCnt = status.markList[ENG_AMTF::en_global].size();

			assert(tempX * tempY == status.markList[ENG_AMTF::en_local].size());

			/*for (int i = 0; i < acamCount; i++)
			{*/
				//markList[ENG_AMTF::en_local]
				//status.markPoolForCam[i + 1].push_back();

			//}
			
			bool basicUp = true; // 위에서 아래로 올라갈경우이다. 
			bool toUp = basicUp;
			for (int i = 0; i < tempX; i++)
			{
				vector<STG_XMXY> temp;
				std::copy(status.markList[ENG_AMTF::en_local].begin() + (i * tempY),
					status.markList[ENG_AMTF::en_local].begin() + (i * tempY) + tempY,
					std::back_inserter(temp));

				std::copy(temp.begin(), temp.end(), std::back_inserter(status.markMapConst[i]));

				auto tgt = &status.markPoolForCamLocal[i >= (tempX / acamCount) ? 2 : 1];
				
				if (i == tempX / acamCount)
					toUp = basicUp;

				if (toUp)
					std::reverse(temp.begin(), temp.end());
					
				toUp = !toUp;

				tgt->insert(tgt->end(), temp.begin(), temp.end());
			}
			status.lastScanCount = tempX / acamCount;
			status.acamCount = acamCount;
		}

		void DoInitial(LPG_CIEA pstCfg)
		{
			this->pstCfg = pstCfg;

			LoadCaliData(this->pstCfg);

			motions["stage"]["x"] = Axis("x", MovingDir::X, (int)Parts::none,
				(double)pstCfg->mc2_svc.min_dist[UINT8(ENG_MMDI::en_stage_x)],
				(double)pstCfg->mc2_svc.max_dist[UINT8(ENG_MMDI::en_stage_x)],
				[&]() {return uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x); },
				[&]() {return uvCmn_MC2_IsDriveError(ENG_MMDI::en_stage_x); });

			motions["stage"]["y"] = Axis("y", MovingDir::Y, (int)Parts::none,
				(double)pstCfg->mc2_svc.min_dist[UINT8(ENG_MMDI::en_stage_y)],
				(double)pstCfg->mc2_svc.max_dist[UINT8(ENG_MMDI::en_stage_y)],
				[&]()->double {return (double)uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y); },
				[&]()->BOOL {return uvCmn_MC2_IsDriveError(ENG_MMDI::en_stage_y); });

			motions["camera"]["x1"] = Axis("x1", MovingDir::X, (int)Parts::camera,
				(double)pstCfg->mc2_svc.min_dist[UINT8(ENG_MMDI::en_align_cam1)],
				(double)pstCfg->mc2_svc.max_dist[UINT8(ENG_MMDI::en_align_cam1)],
				[&]()->double {return (double)uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1); },
				[&]()->BOOL {return uvCmn_MC2_IsDriveError(ENG_MMDI::en_align_cam1); });

			motions["camera"]["x2"] = Axis("x2", MovingDir::X, (int)Parts::camera,
				(double)pstCfg->mc2_svc.min_dist[UINT8(ENG_MMDI::en_align_cam2)],
				(double)pstCfg->mc2_svc.max_dist[UINT8(ENG_MMDI::en_align_cam2)],
				[&]()->double {return (double)uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam2); },
				[&]()->BOOL {return uvCmn_MC2_IsDriveError(ENG_MMDI::en_align_cam2); });;

			 
			ThreadManager::getInstance().addThread("update", [&]() {Update(); });

		}

		//피두셜마크의 차원을 알아냄.
		
	};


	class Stuffs
	{
	private:
		void RemoveOldfiles(const fs::path& path , int hours) 
		{
			auto now = std::chrono::system_clock::now();
			std::chrono::hours ageLimit = std::chrono::hours(hours);

			try 
			{
				if (!fs::exists(path) || !fs::is_directory(path))
					return;
				
				for (auto& entry : fs::recursive_directory_iterator(path)) 
				{
					if (!fs::is_regular_file(entry))
						continue;
					
					auto ftime = fs::last_write_time(entry);
					auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
						ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());

					if (now - sctp < ageLimit)
						continue;

					fs::remove(entry);
				}
			}
			catch (...) {}
		}
	public: 

		string GetCurrentExePath()
		{
			TCHAR execPath[_MAX_PATH];
			GetModuleFileName(NULL, execPath, _MAX_PATH);
			CString path = execPath;
			auto spiltIndex = path.ReverseFind(L'\\');
			return string(CT2CA(path.Left(spiltIndex)));
		}

		void RemoveOldFiles()
		{
			
			RemoveOldfiles(GetCurrentExePath() + "\\save_img",1*24); //하루지난건 싹 삭제.
		}
			
	};


	//인라인클래스 
	class GlobalVariables 
	{

	public: 
		
		static GlobalVariables* getInstance()
		{
			static GlobalVariables _inst;
			return &_inst;
		}

	private:
		map<string, atomic<int>> counter;
		map<string, thread> waiter;
		mutex motionMutex;
		unique_ptr<AlignMotion> alignMotion;
		//AlignMotion* alignMotion = nullptr;
		template <typename MapType>
		bool IsKeyExist(const MapType& map, string key)
		{
			auto it = map.find(key);
			return it != map.end();
		}

		Stuffs stuffUtils;

	public:
		Stuffs& GetStuffs()
		{
			return stuffUtils;
		}


		AlignMotion& GetAlignMotion() 
		{
			return *alignMotion;
		}

		void Destroy()
		{
			GetAlignMotion().Destroy();

			for (auto it = waiter.begin(); it != waiter.end();) {
				if (it->second.joinable())
				{
					it->second.join();
					it = waiter.erase(it); // 맵에서 삭제
				}
				else
				{
					++it;
				}
			}

			ThreadManager::getInstance().waitForAllThreads();
		}

		bool Waiter(string key, std::function<bool()> func, std::function<void()> callback, std::function<void()> timeoutCallback = nullptr, int timeoutDelay = 3000)
		{
			if (IsKeyExist(waiter, key) == true)
			{
				/*if (waiter[key].joinable()) //만약 join으로 설정할경우.
				{
					waiter[key].join();
				}*/
				return false;
			}

			waiter[key] = std::thread([=]()
				{
					int tick = GetTickCount();
					//this_thread::get_id(); 추후 요 id값을 추적해서 스레드 관리.
					bool timeout = false;
					bool condition = false;
					while (condition == false && timeout == false)
					{
						condition = func();
						this_thread::sleep_for(chrono::milliseconds(100));
						timeout = GetTickCount() - tick > timeoutDelay;
					}

					if (condition)
						callback();
					else
						if (timeoutCallback != nullptr)
							timeoutCallback();

					waiter.erase(key);

				});
			waiter[key].detach(); //시간조건상 빠져나가게되어있긴하다. 하지만 callback이나 func()내에서 팬딩상태가 되면 결국 백그라운드 스레드로 남게된다. (상황에 맞게 처리해야한다)
			return true;
		}

		void Init()
		{
			counter["strobeRecved"] = 0;
			//alignMotion = new AlignMotion();
			alignMotion = make_unique<AlignMotion>();
			alignMotion.get()->motionMutex = &motionMutex;
		}

		/*GlobalVariables()
		{
			int debug = 0;
		}*/

		map<int, string> nameTag =
		{
			{(int)ENG_AMOS::en_onthefly_2cam , "ontheFly Sidecam"},
			{(int)ENG_AMOS::en_onthefly_3cam , "ontheFly Centercam"},
			{(int)ENG_AMOS::en_static_2cam , "Static Sidecam"},
			{(int)ENG_AMOS::en_static_3cam , "Static CenterCam"},
		};

		int IncCount(string key)
		{
			if (IsKeyExist(counter, key) == false)
				throw new exception("no key.");

			counter[key].fetch_add(1);
			return GetCount(key);
		}

		int DecCount(string key)
		{
			if (IsKeyExist(counter, key) == false)
				throw new exception("no key.");

			counter[key].fetch_sub(1);
			return GetCount(key);
		}

		int GetCount(string key)
		{
			if (IsKeyExist(counter, key) == false)
				throw new exception("no key.");

			return counter[key].load();
		}

		int ResetCounter(string key)
		{
			if (IsKeyExist(counter, key) == false)
				throw new exception("no key.");

			counter[key].store(0);
			return GetCount(key);
		}
	};


