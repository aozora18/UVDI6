#pragma once
//이하는 전부 인라인 클래스임.

using namespace std;
//#include "framework.h"
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

#include "../../inc/conf/conf_comn.h"
#include "../../inc/conf/luria.h"
#include "../../inc/itfe/EItfcMC2.h"
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
 

	void ThreadManager::addThread(const std::string& key, std::function<void()> callback)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		threads_.emplace(key, std::make_unique<std::thread>(callback));
	}

	void ThreadManager::removeThread(const std::string& key)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		auto it = threads_.find(key);
		if (it != threads_.end()) {
			if (it->second->joinable()) {
				it->second->join();
			}
			threads_.erase(it);
		}
	}

	void ThreadManager::waitForAllThreads()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		for (auto& pair : threads_) {
			if (pair.second->joinable()) {
				pair.second->join();
			}
		}
		threads_.clear();
	}

	bool ThreadManager::isThreadRunning(const std::string& key)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		auto it = threads_.find(key);
		return it != threads_.end() && it->second->joinable();
	}

	 
	 
 
		void AlignCalc::GetFiducialDimension(ENG_AMTF types, int& x, int& y)
		{
			map<double, double> mx, my;

			for each (STG_XMXY fid in markList[types])
			{
				mx[std::floor(fid.mark_x * 100.0f) / 100.0f] = fid.mark_x;
				my[std::floor(fid.mark_y * 100.0f) / 100.0f] = fid.mark_y;
			}

			x = mx.size();
			y = my.size();
		}
 
		bool AlignCalc::CheckAlignScanFinished()
		{
			for each (auto var in status.markMapProcess)
			{
				auto find = std::find_if(var.second.begin(), var.second.end(), [&](STG_XMXY& val) { return val.org_id != 1818; });
				if (find != var.second.end())
					return false;
			}
			return true;
		}

		bool AlignCalc::CheckAlignScanFinished(int scanCount)
		{
			return status.lastScanCount <= scanCount ? true : false;
		}

 
		vector<STG_XMXY> AlignCalc::GetMarkOrder(int columnIndex, bool reverse, bool removeAfterPooling)
		{
			if (status.markMapConst.find(columnIndex) == status.markMapConst.end())
				return vector<STG_XMXY>();

			auto clone = status.markMapConst[columnIndex];

			if (reverse)
				std::reverse(clone.begin(), clone.end());

			if (removeAfterPooling)
				for each (auto val in status.markMapProcess[columnIndex])
					val.org_id = 1818; //다쓴건 1818처리.. nullable이 안되니 이런 등신같은걸 한다. 

			return clone;
		}

		//요건 한줄 빼온거.
		bool AlignCalc::DoPoolingMarks(int camIndex, int columnIndex, bool reverse)
		{
			status.markPoolForCam[camIndex].clear();
			status.markPoolForCam[camIndex] = GetMarkOrder(columnIndex, reverse, true);

			if (status.markPoolForCam[camIndex].size() == 0) return false; //종료.

			return true; //아직할게남음
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="globalFiducial"></param>
		/// <param name="localFiducial"></param>
		/// <param name="fiducialCol"></param>
		/// <param name="fiducialRow"></param>
		/// <param name="leadIndex"></param>
		void AlignCalc::SetFiducial(CFiducialData* globalFiducial, CFiducialData* localFiducial, int acamCount)
		{
			markList.clear();
			
			STG_XMXY temp;

			for (int i = 0; i < globalFiducial->GetCount(); i++)
				if (globalFiducial->GetMark(i, temp))
				{
					markList[ENG_AMTF::en_global].push_back(temp);

					if (temp.tgt_id != 1) continue;

					markParams.mark2x = temp.mark_x;
					markParams.mark2y = temp.mark_y;
				}

			for (int i = 0; i < localFiducial->GetCount(); i++)
				if (localFiducial->GetMark(i, temp))
				{
					markList[ENG_AMTF::en_local].push_back(temp);
				}

			int tempX = 0, tempY = 0;
			GetFiducialDimension(ENG_AMTF::en_local, tempX, tempY);
			status.gerberColCnt = tempX;
			status.gerberRowCnt = tempY;

			
			for (int i = 0; i < tempX; i++)
			{
				std::copy(markList[ENG_AMTF::en_local].begin() + (i * tempY), 
						  markList[ENG_AMTF::en_local].begin() + (i * tempY) + tempY, 
						  std::back_inserter(status.markMapConst[i]));
			}
			status.lastScanCount = tempX / acamCount;
			status.acamCount = acamCount;
		}
 
 
		
		STG_XMXY AlignMotion::GetGerberPosUsePosition(int camIndex, double camXPos, double stageX, double stageY, double& X, double& Y)
		{
			//역산으로 주어진 값으로 피두셜 얻어오기. 오차값이 좀 있긴한데 대략적인 값을 잘 반환하고있다. 

			double tempGerberX = markParams.mark2x + (camXPos - markParams.cam1xLookatMark2) + (stageX - markParams.stageXLookatMark2WithCam1) + (camIndex == 1 ? 0 : markParams.distC2C);
			double tempGerberY = markParams.mark2y + (stageY - markParams.stageYLookatMark2WithCam1);

			//각 카메라별로 설치오차 회전값 적용필
			// ---->  calcParam.Cam1PhyTCorrectAngle, calcParam.Cam2PhyTCorrectAngle;


			double thresholdVal = markParams.thresholdPixel * pstCfg->acam_spec.GetPixelToMM(camIndex); //대충 2pxl이내에서 검색이 되야한다. 

			
		
				vector<STG_XMXY>::iterator  it = find_if(markList[ENG_AMTF::en_local].begin(), markList[ENG_AMTF::en_local].end(),
				[&](const STG_XMXY& elem)
				{
					double distance = std::sqrt(std::pow(tempGerberX - elem.mark_x, 3) + std::pow(tempGerberY - elem.mark_y, 3));
					return distance <= thresholdVal; //오차값이내
				});

			if (it != markList[ENG_AMTF::en_local].end())
			{
				X = tempGerberX;
				Y = tempGerberY;
			}

			return it == markList[ENG_AMTF::en_local].end() ?  STG_XMXY() : *it;
		}


		/// <summary>
		/// 
		/// </summary>
		bool AlignMotion::GetMoveMotionOffset(int camIndex, double gerberPosX, double gerberPosY, double& stageX, double& stageY, double& camX)
		{
			auto AreEqual = [&](double a, double b, int precision = 4)->bool
				{
					double epsilon = std::pow(10, -precision);
					return std::fabs(a - b) < epsilon;
				};


			double lookatX = 0, lookatY = 0;
			GetCamLookatGerberPos(camIndex, lookatX, lookatY); //현재 카메라가 보고있는 거버좌표 얻어옴.

			string camStr = "x" + std::to_string(camIndex);

			double maxRange = GetCamMaxRangeXInGerber(camIndex);
			double minRange = GetCamMinRangeXInGerber(camIndex);

			double clamped = std::clamp(gerberPosX, minRange, maxRange);

			bool inRange = AreEqual(gerberPosX, clamped); //x가 이동가능한 범위인가? y는 따로 검사하지 않는다. 
			if (inRange == false) return false;

			//Refresh(); <- 이동중에 계산되야한다면 한번 호출해줘야한다. 왜냐면 1초에 한번씩 갱신이라서.
			stageX = motions["stage"]["x"].currPos;
			camX = motions["camera"][camStr].currPos + (gerberPosX - lookatX);
			stageY = motions["stage"]["y"].currPos + (gerberPosY - lookatY);

			return true;

			/*stageX
			stageY
			camX*/


		}

		bool AlignMotion::GetMoveMotionOffset(int camIndex, ENG_AMTF markType, int markIndex, double& stageX, double& stageY, double& camX)
		{
			
			vector<STG_XMXY>::iterator it = std::find_if(markList[markType].begin(), 
														 markList[markType].end(),
														[&](const STG_XMXY& XMXY)
														{
															return XMXY.tgt_id == markIndex;
														});

			if (it == markList[markType].end()) return false;

			

			return GetMoveMotionOffset(camIndex, it->mark_x, it->mark_y, stageX, stageY, camX);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="camIndex"></param>
		/// <param name="X"></param>
		/// <param name="Y"></param>
		void AlignMotion::GetCamLookatGerberPos(int camIndex, double& X, double& Y)
		{
			double stageX = motions["stage"]["x"].currPos;
			double stageY = motions["stage"]["y"].currPos;

			string camName = "x" + std::to_string(camIndex);

			double camX = motions["camera"][camName].currPos;

			double tempX, tempY;
			//tempX = calcParam.mark2x + (stageX - calcParam.stageXLookatMark2WithCam1) + (camIndex == 1 ? camX - calcParam.cam1xLookatMark2 : calcParam.distC2C + camX);
			tempX = markParams.mark2x + (camIndex == 1 ? 0 : markParams.distC2C) + (stageX - markParams.stageXLookatMark2WithCam1) + (camIndex == 1 ? camX - markParams.cam1xLookatMark2 : camX);
			tempY = markParams.mark2y + (stageY - markParams.stageYLookatMark2WithCam1);

			//각 카메라별로 설치오차 회전값 적용필
			// ---->  calcParam.Cam1PhyTCorrectAngle, calcParam.Cam2PhyTCorrectAngle;

			X = tempX;
			Y = tempY;

		}

		double AlignMotion::GetCamMinRangeXInGerber(int camIndex)
		{

			string camName = "x" + std::to_string(camIndex);

			double camX = motions["camera"][camName].currPos;
			double minXLimCam = motions["camera"][camName].min;
			double maxXLimCam = motions["camera"][camName].max;

			double stageX = motions["stage"]["x"].currPos;
			double minXLimStage = motions["stage"]["x"].min;

			//캠이 현재 커버가능한 거버상의 X좌표 최저값
			double temp = markParams.mark2x + (camIndex == 1 ? 0 : markParams.distC2C) - (camX - minXLimCam) + (markParams.camMoveOnly ? 0 : (stageX - minXLimStage));
			return temp;
		}

		double AlignMotion::GetCamMaxRangeXInGerber(int camIndex)
		{
			string camName = "x" + std::to_string(camIndex);
			double camX = motions["camera"][camName].currPos;
			double minXLimCam = motions["camera"][camName].min;
			double maxXLimCam = motions["camera"][camName].max;

			double stageX = motions["stage"]["x"].currPos;
			double minXLimStage = motions["stage"]["x"].min;
			double maxXLimStage = motions["stage"]["x"].max;

			//캠이 현재 커버가능한 거버상의 X좌표 최대값
			double temp = markParams.mark2x + (camIndex == 2 ? markParams.cam1xLookatMark2 + markParams.distC2C : 0) + (maxXLimCam - camX) + (markParams.camMoveOnly ? 0 : (maxXLimStage - stageX));
			return temp;
		}

		//////////////////////////////////////////////////////////


		void AlignMotion::Update()
		{
			const int updateDelay = 1000;
			while (onUpdate)
			{
				Refresh();
				this_thread::sleep_for(chrono::milliseconds(updateDelay));
			}
		}
 
		void AlignMotion::Refresh() //바로 갱신이 필요하면 요거 다이렉트 호출 보통은 스레드에서 호출
		{
			for (auto i = motions.begin(); i != motions.end(); i++)
				for (auto j = i->second.begin(); j != i->second.end(); j++)
				{
					if (j->second.stateCallback != nullptr)
						j->second.onError = j->second.stateCallback();

					if (j->second.onError == false && j->second.posCallback != nullptr)
						j->second.currPos = j->second.posCallback();
				}

			////이하 테스트메서드
			//auto _1 = GetCamMinRangeXInGerber(1);
			//auto _2 = GetCamMinRangeXInGerber(2);

			//auto _11 = GetCamMaxRangeXInGerber(1);
			//auto _22 = GetCamMaxRangeXInGerber(2);


			//double x = 0, x2 = 0, y = 0, y2 = 0;

			//auto _33 = GetGerberPosUsePosition(1, motions["camera"]["x1"].currPos,
			//	motions["stage"]["x"].currPos,
			//	motions["stage"]["y"].currPos, x, y);

			//
			//GetCamLookatGerberPos(1, x, y);
			//GetCamLookatGerberPos(2, x2, y2);

			//double c1=0, c2=0;
			//bool canMove1 = GetMoveMotionOffset(1, ENG_AMTF::en_global, 4 -1, x, y, c1); //캠1이 4번마크를 볼수있는가? 없어야한다.
			//	 canMove1 = GetMoveMotionOffset(1, ENG_AMTF::en_global, 2 -1, x, y, c1); //캠1이 2번마크를 볼수있는가?있어야한다.
			//
			//bool canMove2 = GetMoveMotionOffset(2, ENG_AMTF::en_global, 2 -1, x, y, c2); //캠2이 2번마크를 볼수있는가? 없어야한다.
			//	 canMove2 = GetMoveMotionOffset(2, ENG_AMTF::en_global, 4 -1, x, y, c2); //캠2이 2번마크를 볼수있는가? 있어야한다.

		}

		void AlignMotion::DoInitial(LPG_CIEA pstCfg)
		{
			this->pstCfg = pstCfg;

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

	 
		bool GlobalVariables::Waiter(string key, std::function<bool()> func, std::function<void()> callback, std::function<void()> timeoutCallback = nullptr, int timeoutDelay = 3000)
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

		 
