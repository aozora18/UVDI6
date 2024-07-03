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

enum OffsetType
{
	expo,
	align,
	refind,
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
	void addThread(const std::string& key, std::atomic<bool>&  stopFlag , std::function<void()> callback);

	void removeThread(const std::string& key);

	void waitForAllThreads();

	bool isThreadRunning(const std::string& key);


private:

	std::unordered_map<std::string, tuple<std::unique_ptr<std::thread>, std::atomic<bool>&>> threads_;
	std::mutex mutex_;
};



struct subOffset //참조용.
{
	double suboffsetX;
	double suboffsetY;
};

struct CaliPoint : subOffset
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

	CaliPoint(double x,
		double y,
		double offsetX,
		double offsetY,
		double subOffsetX,
		double subOffsetY,
		STG_XMXY srcFid)
	{
		this->x = x;
		this->y = y;
		this->offsetX = offsetX;
		this->offsetY = offsetY;
		this->suboffsetX = subOffsetX;
		this->suboffsetY = subOffsetY;
		this->srcFid = srcFid;
	}

	CaliPoint(double x,
		double y,
		double offsetX,
		double offsetY,
		STG_XMXY srcFid)
	{
		this->x = x;
		this->y = y;
		this->offsetX = offsetX;
		this->offsetY = offsetY;
		this->srcFid = srcFid;
	}


	CaliPoint()
	{
		this->x = -1;
		this->y = -1;
		this->offsetX = -1;
		this->offsetY = -1;
	}

	bool isNothing()
	{
		return this->x == -1 && this->y == -1 && this->offsetX == -1 && this->offsetY == -1;
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
public :
	struct CaliFeature
	{
		double expoStartX, expoStartY, dummy;
		int caliCamIdx;
		double caliCamXPos;
		int dataRowCnt, dataColCnt;

		CaliFeature() 
		{
			caliCamIdx = -1;
			expoStartX = 0;expoStartY = 0;dummy = 0;caliCamXPos = 0;dataRowCnt = 0;dataColCnt = 0;
		}

		CaliFeature(vector<double> featureVec)
		{
			auto begin = featureVec.begin();

			expoStartX = begin != featureVec.end() ? (double)*begin++ : 0;
			expoStartY = begin != featureVec.end() ? (double)*begin++ : 0;
			dummy = begin != featureVec.end() ? (double)*begin++ : 0;
			dataRowCnt = begin != featureVec.end() ? (int)*begin++ : 0;
			dataColCnt = begin != featureVec.end() ? (int)*begin++ : 0;
			caliCamIdx = begin != featureVec.end() ? (int)*begin++ : -1;
			caliCamXPos = begin != featureVec.end() ? (double)*begin++ : 0;
		}
	};


public:
	bool caliInfoLoadComplete = false;
	CaliFeature features;

protected:
	map<OffsetType, CaliFeature> calidataFeature;
	map<OffsetType, vector<CaliPoint>> caliDataMap;

	double LimittoMicro(double val);

	void SortPos(std::vector<CaliPoint>& dataList);

	CaliPoint Estimate(vector<CaliPoint>& points, double x, double y);
	CaliPoint CalculateAverageOffset(const std::vector<CaliPoint>& nearbyPoints);
public:

	void LoadCaliData(LPG_CIEA cfg);
	CaliPoint EstimateAlignOffset(int camIdx, double stageX, double stageY, double camX);
	CaliPoint EstimateExpoOffset(double gbrX, double gbrY);
	CaliFeature GetCalifeature(OffsetType type);
	
};


struct Params
{
	double currGerbermark2x = 0, currGerbermark2y = 0;
	double caliGerbermark2x = 0, caliGerbermark2y = 0;
	double mark2StageX = 0;

	map<int, tuple<double, double>> mark2CamoffsetXY;

	double convertThreshold = 1.3f;
	
	double distCam2cam[4] = { 0, };
	int centerCamIdx = 3;
	ENG_AMOS alignMotion;
	ENG_ATGL alignType;
	vector<tuple<ENG_MMDI, double, double>> axisLimit;
	
	void SetExpoShiftValue(double x, double y)
	{
		this->expoShiftX = x;
		this->expoShiftY = y;
	}

	void GetExpoShiftValue(double& x, double& y)
	{
		x = this->expoShiftX;
		y = this->expoShiftY;
	}

private:
	double expoShiftX = 0;
	double expoShiftY = 0;
};


class Environment //환경요소.
{
public :
	bool DoCalib();
	void UpdateStateValue();
	void WriteCalibResult();

	tuple<double, double> GetCalibOffset()
	{
		return calibOffset;
	}

	bool NeedCalib()
	{
		return needCalib;
	}


private:
	long reCalibTerm;
	long lastCalibTime;
	double lastTemperature;
	
	bool needCalib;

	int calibCamIdx;
	double calibCamX;
	tuple<double, double> calibStagePos;
	tuple<double, double> calibOffset; //<-가장 최근에 실행한결과값.

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


class Status
{
private:
	bool dataReady;
	//tuple<double, double> refindOffset;

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

	/*void GetRefindOffset(double& x, double& y)
	{
		x = std::get<0>(refindOffset);
		y = std::get<1>(refindOffset);
	}*/

	/*void SetRefindOffset(tuple<double, double> refindOffset)
	{
		this->refindOffset = refindOffset;
	}*/

	void Init()
	{
		dataReady = false;
		SpiltMode spiltMode = SpiltMode::byCamCount;

		gerberRowCnt = 0;
		gerberColCnt = 0;
		localMarkCnt = 0;
		globalMarkCnt = 0;

		BufferClear();
	}

	void BufferClear()
	{
		SetDataReady(false);
		//refindOffset = make_tuple(0, 0);
		offsetPool.clear();
		markPoolForCam.clear();
		markList.clear(); //column별 풀링
		markMapConst.clear(); //원본
	}

	int acamCount = 0;
	int gerberRowCnt = 0;
	int gerberColCnt = 0;
	int localMarkCnt = 0;
	int globalMarkCnt = 0;

	int lastScanCount = 0;


	map< OffsetType, vector<CaliPoint>> offsetPool;
	
	STG_XMXY mark1, mark2; //마크1,2
	
	map<int, vector<STG_XMXY>> markPoolForCam; //카메라별 풀링.
	map<int, vector<STG_XMXY>> markMapConst; //원본인데 스켄라인별로 정렬된것
	std::map<ENG_AMTF, vector<STG_XMXY>> markList; //글로벌, 로컬 원본인데 맵핑만된것.	
};


class CommonMotionStuffs
{
private:
	static  CommonMotionStuffs inst;

public:
	static CommonMotionStuffs& GetInstance()
	{
		return inst;
	}
	
	tuple<double, double> GetCurrStagePos();

	bool SingleGrab(int camIndex);

	bool IsMarkFindInLastGrab();
	int GetGrabCnt(int camIdx);

	bool IsMarkFindInLastGrab(int camIdx, double* grabOffsetX = nullptr, double* grabOffsetY = nullptr);

	bool MoveAxis(ENG_MMDI axis, bool absolute, double pos, bool waiting, int timeout = 30 * 1000);

	void GetOffsetsUseMarkPos(int centerCam, STG_XMXY mark, CaliPoint* alignOffset = nullptr, CaliPoint* expoOffset = nullptr);
	void GetOffsetsCurrPos(int centerCam, STG_XMXY mark, CaliPoint* alignOffset = nullptr, CaliPoint* expoOffset = nullptr, double posOffsetX=0, double posOffsetY=0);
	
	LPG_ACGR GetGrabPtr(int camIdx, int tgtMarkIdx, ENG_AMTF markType);
};

class RefindMotion
{
	struct  RSTValue
	{
		double orgStartX, orgStartY, orgEndX, orgEndY;
		double obsStartX, obsStartY, obsEndX, obsEndY;
		bool rstCalcReady;

		bool GetEstimatePos(double estimatedX, double estimatedY, double& correctedX, double& correctedY);

		RSTValue()
		{
			this->rstCalcReady = false;
		}

		RSTValue(double orgStartX, double orgStartY, double orgEndX, double orgEndY, double obsStartX, double obsStartY, double obsEndX, double obsEndY)
		{
			this->orgStartX = orgStartX;
			this->orgStartY = orgStartY;
			this->orgEndX = orgEndX;
			this->orgEndY = orgEndY;
			this->obsStartX = obsStartX;
			this->obsStartY = obsStartY;
			this->obsEndX = obsEndX;
			this->obsEndY = obsEndY;
			this->rstCalcReady = true;
		}
	};


private:
	bool useRefind;
	double stepSizeX, stepSizeY;
	RSTValue rstValue;
public:
	bool IsUseRefind() { return useRefind; }

	RefindMotion();
	

	bool ProcessRefind(int centerCam, std::tuple<double, double>* refindOffset = nullptr, std::tuple<double, double>* grabOffset = nullptr);
	bool ProcessEstimateRST(int centerCam , std::vector<STG_XMXY> representPoints, bool& errFlag, std::vector<STG_XMXY>& refindOffsetPoints);//ROTATE, SCALE, TRANSFORM
	bool GetEstimatePos(double estimatedX, double estimatedY, double& correctedX, double& correctedY);
	

	

};

class AlignMotion
{
public:
	mutex* motionMutex;
	std::atomic<bool> cancelFlag = false;

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

	CaliCalc::CaliFeature GetCalifeature(OffsetType type);
	CaliPoint EstimateAlignOffset(int camIdx, double stageX = 0, double stageY = 0, double camX = -1);
	CaliPoint EstimateExpoOffset(double gbrX, double gbrY);
	bool GetOffsetFromPool(OffsetType type, int tgtMarkIdx, CaliPoint& temp);
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
	void GetStagePosUseGerberPos(int camNum, STG_XMXY gbrPos, STG_XMXY& stagePos);
	

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
	 

	vector<STG_XMXY> GetFiducialPool(int camNum);
	void UpdateParamValues();
	void SetAlignOffsetPool(map< OffsetType, vector<CaliPoint>> offsetPool);
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
	unique_ptr<RefindMotion> refindMotion;
	unique_ptr<TriggerManager> triggerManager;
	unique_ptr<WebMonitor> webMonitor;
	unique_ptr<Environment> environment;

	
	template <typename MapType>
	bool IsKeyExist(const MapType& map, string key)
	{
		auto it = map.find(key);
		return it != map.end();
	}

public:
	void StartWebMonitor();

public:

	AlignMotion& GetAlignMotion()
	{
		return *alignMotion;
	}

	RefindMotion& GetRefindMotion()
	{
		return *refindMotion;
	}

	TriggerManager& GetTrigger()
	{
		return *triggerManager;
	}

	WebMonitor& GetWebMonitor()
	{
		return *webMonitor;
	}

	Environment& GetEnvironment()
	{
		return *environment;
	}

	void Destroy()
	{
		GetAlignMotion().Destroy();
		alignMotion.reset();
		refindMotion.reset();
		triggerManager.reset();
		webMonitor.reset();
		environment.reset();

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
				{
					if (callback != nullptr)
						callback();
				}
				else
				{
					if (timeoutCallback != nullptr)
						timeoutCallback();
				}

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
		refindMotion = make_unique<RefindMotion>();
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