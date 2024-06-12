#pragma once
//이하는 전부 인라인 클래스임.

#include <iostream>
#include <filesystem>
#include <chrono>

#include <cassert>

#include <list>
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

#include "webViewinterop.h"

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

enum CaliTableType
{
	expo,
	align
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


	CaliPoint()
	{
		this->x = 0;
		this->y = 0;
		this->offsetX = 0;
		this->offsetY = 0;
	}



	CaliPoint operator+(const CaliPoint& other) const
	{
		CaliPoint result;
		result.x = this->x;
		result.y = this->y;
		result.offsetX = this->offsetX + other.offsetX;
		result.offsetY = this->offsetY + other.offsetY;

		result.srcFid = result.srcFid;
		return result;
	}


	CaliPoint operator-(const CaliPoint& other) const
	{
		CaliPoint result;
		result.x = this->x;
		result.y = this->y;
		result.offsetX = this->offsetX - other.offsetX;
		result.offsetY = this->offsetY - other.offsetY;
		result.srcFid = this->srcFid;
		return result;
	}

	double x;
	double y;
	double offsetX;
	double offsetY;
	STG_XMXY srcFid;
};


class TriggerBase
{
public:
	enum
	{
		increase = 0,
		decrease,
		end,
	};
};

class TriggerData : TriggerBase
{
public:
	int channel;
	int delay[end] = { 0,0 };

	vector<INT32> pos;

	void Regist(int direction)
	{
		//여기서 소팅하고 부호 바꾸고, 등록하고 클리어.


		std::sort(pos.begin(), pos.end(), less<INT32>()); //오름차순
		if (direction == decrease)
			std::sort(pos.begin(), pos.end(), greater<INT32>()); //내림차순으로 변경.

		for (int i = 0; i < pos.size(); i++)
		{
			pos[i] *= direction == decrease ? -1 : 1;
		}

		uvEng_Mvenc_ReqWriteAreaTrigPosCh(channel, 0, pos.size(), pos.data(), direction == decrease ? ENG_TEED::en_negative : ENG_TEED::en_positive, TRUE);
		pos.clear();
	}

};

class TriggerManager : TriggerBase
{
public:
	void AddTrigPos(int channel, INT32 pos)
	{
		triggers[channel].pos.push_back(pos);
	}

	void SetDelay(int channel, int incDelay, int decDelay)
	{
		triggers[channel].delay[increase] = incDelay;
		triggers[channel].delay[decrease] = decDelay;
	}

	void Reset()
	{
		uvEng_Mvenc_ReqTriggerStrobe(FALSE);
		uvEng_Mvenc_ReqEncoderOutReset();
		uvEng_Mvenc_ResetTrigPosAll();

		triggers.clear();
	}

	void Regist(int direction, int channel = -1)
	{
		if (channel == -1)
		{
			for each (auto var in triggers)
				var.second.Regist(direction);
		}
		else
		{
			triggers[channel].channel = channel;
			triggers[channel].Regist(direction);
		}


		//uvEng_Mvenc_ReqTriggerStrobe(TRUE);
	}

private:
	map<int, TriggerData> triggers;

};

class CaliCalc
{

public:
	bool caliInfoLoadComplete = false;

protected:
	map<int, map<CaliTableType, vector<double>>> calidataFeature;
	map<int, map<CaliTableType, vector<CaliPoint>>> caliDataMap;

	double LimittoMicro(double val);

	void SortPos(std::vector<CaliPoint>& dataList);

	CaliPoint Estimate(vector<CaliPoint>& points, double x, double y);
	CaliPoint CalculateAverageOffset(const std::vector<CaliPoint>& nearbyPoints);
public:

	void LoadCaliData(LPG_CIEA cfg);
	CaliPoint EstimateAlignOffset(int camIdx, double stageX, double stageY, double camX);
	CaliPoint EstimateExpoOffset(double gbrX, double gbrY);


};


struct Params
{
	double currGerbermark2x = 0, currGerbermark2y = 0;
	double caliGerbermark2x = 0, caliGerbermark2y = 0;
	int threshold = 10;
	//double markZeroOffset[2] = { 0, };
	double mark2StageX = 0;
	//		double mark2cam1Y = 0;
	//		double mark2cam2Y = 0;
			//double mark2Cam1X = 0;
			//double mark2Cam2X = 0;

	map<int, tuple<double, double>> mark2CamoffsetXY;

	double distCam2cam[4] = { 0, };
	int centerCamIdx = 3;
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


		this->min = min;//(INT32)ROUNDED(min * 10000.0f, 0);
		this->max = max;//(INT32)ROUNDED(max * 10000.0f, 0);
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
private:
	bool dataReady;

public:
	bool IsDataReady() { return dataReady; }
	void SetDataReady(bool set) { dataReady = set; }

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
		dataReady = false;
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
		SetDataReady(false);
		//markPoolForCamLocal.clear(); //카메라별 풀링.
		offsetPool.clear();
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


	map< CaliTableType, vector<CaliPoint>> offsetPool;


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

	bool IsDataReady() { return status.IsDataReady(); }

	Status status;
	Params markParams;

	void SetCentercamIdx(int idx)
	{
		markParams.centerCamIdx = idx;
	}

	enum Parts
	{
		none = 0b00000000,
		camera = 0b00000001,
		etc = 0b00000010,
	};

	CaliPoint EstimateAlignOffset(int camIdx, double stageX = 0, double stageY = 0, double camX = -1);
	CaliPoint EstimateExpoOffset(double gbrX, double gbrY);
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

	bool isArrive(string drive, string axis, double dest, float threshold = 0.001);

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

		if (poolSize == 0 || index >= poolSize)
			return false;

		map<int, vector< LPG_ACGR>> buff;

		for (int i = 0; i < grabPool->GetCount(); i++)
		{
			auto at = grabPool->GetAt(grabPool->FindIndex(i));
			buff[at->cam_id].push_back(at);
		}

		if (index != -1 && buff[camIndex].size() < index)
			return false;

		if (index == -1)
			index = buff[camIndex].size() == 0 ? 0 : buff[camIndex].size();

		if (index >= pool.size()) return false;
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
	void SetAlignOffsetPool(map< CaliTableType, vector<CaliPoint>> offsetPool);
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
	unique_ptr<TriggerManager> triggerManager;
	unique_ptr<WebMonitor> webMonitor;
	//AlignMotion* alignMotion = nullptr;
	template <typename MapType>
	bool IsKeyExist(const MapType& map, string key)
	{
		auto it = map.find(key);
		return it != map.end();
	}

public:
	void StartWebMonitor()
	{
		if (GetWebMonitor().StartWebServer())
		{
			if (GetWebMonitor().ConnectClient())
			{
				GetWebMonitor().Update(string("new1"), string("b1"), string("c"));
				GetWebMonitor().Update(string("new2"), string("b2"), string("c"));


				GetWebMonitor().RefreshWebPage();
			}
		}
	}

public:

	AlignMotion& GetAlignMotion()
	{
		return *alignMotion;
	}

	TriggerManager& GetTrigger()
	{
		return *triggerManager;
	}

	WebMonitor& GetWebMonitor()
	{
		return *webMonitor;
	}


	void Destroy()
	{
		GetAlignMotion().Destroy();
		alignMotion.reset();
		triggerManager.reset();
		webMonitor.reset();

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
		const int CHECKTERM = 100; int elapsedMS = 0;
		bool res = false;
		this_thread::sleep_for(chrono::milliseconds(CHECKTERM));
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

	bool Waiter(string key, std::function<bool()> func, std::function<void()> callback = nullptr, std::function<void()> timeoutCallback = nullptr, int timeoutDelay = 3000)
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
					if (callback != nullptr)callback();
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
		ResetCounter("strobeRecved");
		ResetCounter("mainUpdate");
		alignMotion = make_unique<AlignMotion>();
		alignMotion.get()->motionMutex = &motionMutex;
		webMonitor = make_unique<WebMonitor>();
		triggerManager = make_unique<TriggerManager>();
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
		/*if (IsKeyExist(counter, key) == false)
			throw new exception("no key.");*/

		counter[key].store(0);
		return GetCount(key);
	}
};