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
#include "../../inc/itfc/ItfcUVDI15.h"
#include "param/InterLockManager.h"


using namespace std;
class CaliCalc;

enum MovingDir
{
	X,
	Y,
	Z,
	T,
	STATIC,
};

enum SearchFlag
{
	global,
	local,
	all,
};


	template <typename T>
	class Singleton
	{
	public:
		static T& getInstance()
		{
			static T instance;
			return instance;
		}
	};

	// 자주쓰게될것같아 래퍼클래스로 만듬. CV이용
	class ConditionWrapper
	{
	public:
		ConditionWrapper() : isConditionMet(false) {}

		bool WaitFor(std::chrono::milliseconds timeout);
		void Notify(bool res);
		void Reset();
		

	private:
		std::mutex mtx;
		std::condition_variable cv;
		bool isConditionMet;
	};


	class ThreadManager : public Singleton<ThreadManager>
	{
	public:
		void addThread(const std::string& key, std::function<void()> callback);

		void removeThread(const std::string& key);

		void waitForAllThreads();

		bool isThreadRunning(const std::string& key);


	private:

		std::unordered_map<std::string, std::unique_ptr<std::thread>> threads_;
		std::mutex mutex_;
	};

 


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
		STG_XMXY srcFid;
	};



	class CaliCalc
	{
	public:
		bool caliInfoLoadComplete = false;
	
	protected:
		map<int, vector<CaliPoint>> caliDataMap;

		double LimittoMicro(double val);

		void SortPos(std::vector<CaliPoint>& dataList);

	
		CaliPoint CalculateAverageOffset(const std::vector<CaliPoint>& nearbyPoints);
	public:
		void LoadCaliData(LPG_CIEA cfg);
		CaliPoint EstimateOffset(int camIdx, double stageX, double stageY, double camX);

	};


	struct Params
	{
		double currGerbermark2x = 0, currGerbermark2y = 0;
		double caliGerbermark2x = 0, caliGerbermark2y = 0;
		int threshold = 10;
		double mark2StageX = 0;
		double mark2cam1Y = 0;
		double mark2cam2Y = 0;
		double mark2Cam1X = 0;
		double mark2Cam2X = 0;
		double distCam2cam[4] = { 0, };
		ENG_AMOS alignMotion;
		ENG_ATGL alignType;
		vector<tuple<ENG_MMDI, double, double>> axisLimit;

	};

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
			//markPoolForCamLocal.clear(); //카메라별 풀링.
			alignOffsetPool.clear();
			markPoolForCam.clear();
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

		//map<int, vector<STG_XMXY>> markPoolForCamGlobal; //카메라별 풀링.
		vector<CaliPoint> alignOffsetPool;
		map<int, vector<STG_XMXY>> markPoolForCam; //카메라별 풀링.
		map<int, vector<STG_XMXY>> markMapConst; //원본인데 스켄라인별로 정렬된것
		//map<int, vector<STG_XMXY>> markMapProcess; //현재 처리된 상태들 (처리된것들은 1818로 변경됨)
		std::map<ENG_AMTF, vector<STG_XMXY>> markList; //글로벌, 로컬 원본인데 맵핑만된것.	
	};

	class AlignMotion
	{
	public:
		mutex* motionMutex;
		bool onUpdate = true;
		LPG_CIEA pstCfg = nullptr;
		
		

		Status status;
		Params markParams;
		

		enum Parts
		{
			none = 0b00000000,
			camera = 0b00000001,
			etc = 0b00000010,
		};

		CaliPoint EstimateOffset(int camIdx, double stageX = 0, double stageY = 0, double camX = -1);
		void Destroy();

	protected:
		CaliCalc caliCalcInst;
		map<string, map<string, Axis>> axises;

	public:

		map<string, map<string, Axis>> GetAxises() { return axises; }

		void Update();

	public:

		void Refresh();

		void LoadCaliData(LPG_CIEA cfg);

		bool GetNearFid(STG_XMXY currentPos, SearchFlag flag, vector<STG_XMXY> skipList, STG_XMXY& findFid);

		bool isArrive(string drive, string axis, double dest, float threshold);

		bool NowOnMoving();

		bool MovetoGerberPos(int camNum, STG_XMXY tgtPos);

		bool GetGerberPosUseCamPos(int camNum, STG_XMXY& point);
		
		bool GetCamPosUseGerberPos(STG_XMXY gerberPos, int baseCamNum, double& camAxis, double& stageX, double& stageY);

		void GetFiducialDimension(ENG_AMTF types, int& x, int& y);
		bool CheckAlignScanFinished(int scanCount);
		//void SetAlignMode(ENG_AMOS motion, ENG_ATGL aligntype);

		
		bool GetFiducialInfo(int camIndex, CAtlList <LPG_ACGR>* grabPool, int index, STG_XMXY& xmxy)
		{
			auto pool = status.markPoolForCam[camIndex];
			int poolSize = pool.size();
			
			if (poolSize == 0 || index > poolSize)
				return false;

			map<int, vector< LPG_ACGR>> buff;

			for (int i = 0 ; i < grabPool->GetCount(); i++)
			{
				auto at = grabPool->GetAt(grabPool->FindIndex(i));
				buff[at->cam_id].push_back(at);
			}
			
			if (index != -1 && buff[camIndex].size() < index) return false;

			if (index == -1)
				index = buff[camIndex].size() == 0 ? 0 : buff[camIndex].size();
			
			xmxy = pool[index];
			return true;

			// return pool[index]; .tgt_id;// *= isGlobal ? -1 : 1;

		}

		//int GetFiducialIndex(int camIndex,   CAtlList <LPG_ACGR>* grabList)
		//{
		//	//auto grabData = grabList;
		//	//const int GLOBAL_FIDUCIAL_OFFSET = -1;

		//	if ((grabList == NULL || grabList->GetCount() == 0))
		//		return -1818;

		//	/*std::map<int, vector<LPG_ACGR>> imgMap;

		//	for (int i = isGlobal ? 0 : status.globalMarkCnt; i < grabData->GetCount(); i++)
		//	{
		//		auto val = grabData->GetAt(grabData->FindIndex(i));
		//		imgMap[val->cam_id].push_back(val);
		//	}*/

		//	auto pool = status.markPoolForCam[camIndex]; //isGlobal ? status.markPoolForCamGlobal[camIndex] : status.markPoolForCamLocal[camIndex];

		//	int currentCnt = grabList->GetCount(); //imgMap[camIndex].size();
		//	int poolSize = pool.size();

		//	/*if (currentCnt == 0)
		//		int debug = 0;*/

		//	if (poolSize == 0)
		//		return -1818;
		//	

		//	return pool[currentCnt].tgt_id;// *= isGlobal ? -1 : 1;
		//}

		vector<STG_XMXY> GetFiducialPool(int camNum);
		void UpdateParamValues();
		void SetAlignOffsetPool(vector<CaliPoint> offsetPool);
		void SetFiducialPool(bool useDefault = true, ENG_AMOS alignMotion = ENG_AMOS::en_onthefly_2cam, ENG_ATGL alignType = ENG_ATGL::en_global_4_local_0_point);
		void DoInitial(LPG_CIEA pstCfg);
	};

	

	//인라인클래스 
	class GlobalVariables 
	{

	public: 
		
		static GlobalVariables* GetInstance()
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

		

	public:
		
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

		bool Waiter(std::function<bool()> func, int timeoutDelay = 3000)
		{
			const int CHECKTERM = 100; int elapsedMS=0;
			bool res = false;
			while (res == false && elapsedMS < timeoutDelay)
			{
				try
				{
					res = func();
				}
				catch (const std::exception&)
				{
					return false;
				}
				this_thread::sleep_for(chrono::milliseconds(CHECKTERM));
				elapsedMS += 100;
			}
			return res;
		}

		bool Waiter(string key, std::function<bool()> func, std::function<void()> callback=nullptr, std::function<void()> timeoutCallback = nullptr, int timeoutDelay = 3000)
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
						if(callback != nullptr)callback();
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