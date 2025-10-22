#pragma once
//이하는 전부 인라인 클래스임.
#include <iostream>
#include <filesystem>
#include <chrono>
#include <cassert>
#include <list>
#include <map>
#include <string>
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
#include "param/InterLockManager.h"
#include "webViewinterop.h"
#include <iomanip>
#include <sstream>
#include <cmath>
#include <optional>
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
#include "Ajinprotocol.h"
#include "CommWrapper.h"


using namespace std;
using namespace mini;
using namespace mini::wrap;

class CaliCalc;
class AutoFocus;
class AFstate;

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
	enviromental,
	grab,
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
	std::recursive_mutex mutex_;


	
};

struct subOffset //참조용.
{
	double suboffsetX;
	double suboffsetY;
};

struct CaliPoint : subOffset
{
	CaliPoint* SetError()
	{
		this->x = -1;
		this->y = -1;
		this->offsetX = -1;
		this->offsetY = -1;
		return this;
	}

	bool IsError()
	{
		return (this->x == -1 ||  this->y == -1 || this->offsetX == -1 || this->offsetY == -1);
	}

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
	int offset[end] = { 0,0 };

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

	void SetOffset(int channel, int incOffset, int decOffset)
	{
		triggers[channel].offset[increase] = incOffset;
		triggers[channel].offset[decrease] = decOffset;
	}


	void GetDelay(int channel, int& incDelay, int& decDelay)
	{
		incDelay = triggers[channel].delay[increase];
		decDelay = triggers[channel].delay[decrease];
	}

	void GetOffset(int channel, int& incOffset, int& decOffset)
	{
		incOffset = triggers[channel].offset[increase];
		decOffset = triggers[channel].offset[decrease];
	}

	void Reset()
	{
		uvEng_Mvenc_ReqTriggerStrobe(FALSE);
		uvEng_Mvenc_ReqEncoderOutReset();
		uvEng_Mvenc_ResetTrigPosAll();

		for (auto& v : triggers)
			v.second.pos.clear();
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
	~CaliCalc()
	{
		if (loadseqThread.joinable())loadseqThread.join();
	}

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

	map<int, vector<pair<double, pair<double, double>>>> camZMeasure; // cam idx , <z , <offset x, offset y>>

	thread loadseqThread;
	double LimittoMicro(double val);

	void SortPos(std::vector<CaliPoint>& dataList);

	

	CaliPoint Estimate(vector<CaliPoint>& points, double x, double y);
	CaliPoint CalculateAverageOffset(const std::vector<CaliPoint>& nearbyPoints);
public:
	pair<double, double> GetCamZOffset(int camIdx, float currZ);
	void LoadCaliData(LPG_CIEA cfg);
	CaliPoint EstimateAlignOffset(int camIdx, double stageX, double stageY);
	CaliPoint EstimateExpoOffset(int camIdx, double gbrX, double gbrY);
	CaliFeature GetCalifeature(OffsetType type);
	
};

struct Params
{
	double currGerbermark2x = 0, currGerbermark2y = 0;
	double caliGerbermark2x = 0, caliGerbermark2y = 0;
	double mark2StageX = 0;

	map<int, tuple<double, double>> mark2CamoffsetXY;

	double distCam2cam[4] = { 0, };
	int centerCamIdx = 3;
	double centerMarkzeroOffsetX = 0;
	double centerMarkzeroOffsetY = 0;
	ENG_AMOS alignMotion; ENG_AMOS prevAlignMotion; ENG_AMOS processedAlignMotion;
	ENG_ATGL alignType;
	ENG_WETE workErrorType;
	vector<tuple<ENG_MMDI, double, double>> axisLimit;
	
	void Reset()
	{
		alignMotion = ENG_AMOS::none;
		prevAlignMotion = ENG_AMOS::none;
		processedAlignMotion = ENG_AMOS::none;
		alignType = ENG_ATGL::en_not_defined;
	}

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

	bool SingleGrab(int camIndex, bool successIfNotNull);

	bool IsMarkFindInLastGrab();
	int GetGrabCnt(int camIdx);
	bool Inposition(ENG_MMDI axis, double pos, double threashold = 0.002);
	bool IsMarkFindInLastGrab(int camIdx, double* grabOffsetX = nullptr, double* grabOffsetY = nullptr);

	bool MoveAxis(ENG_MMDI axis, bool absolute, double pos, bool waiting, int timeout = 30 * 1000,double speed=50);

	bool GetOffsetsUseMarkPos(int centerCam, STG_XMXY mark, CaliPoint* alignOffset = nullptr, CaliPoint* expoOffset = nullptr, double posOffsetX=0, double posOffsetY=0);
	bool GetOffsetsCurrPos(int centerCam, STG_XMXY mark, CaliPoint* alignOffset = nullptr, CaliPoint* expoOffset = nullptr, double posOffsetX=0, double posOffsetY=0);
	
	LPG_ACGR GetGrabPtr(int camIdx, int tgtMarkIdx, ENG_AMTF markType);

	bool NowOnMoving();


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
	int refindCnt;
	
	RSTValue rstValue;
public:
	double thresholdDist;
	bool IsUseRefind() { return useRefind; }

	RefindMotion();
	

	bool ProcessRefind(int centerCam, std::tuple<double, double>* refindOffset = nullptr, std::tuple<double, double>* grabOffset = nullptr);
	bool ProcessEstimateRST(int centerCam , std::vector<STG_XMXY> representPoints, bool& errFlag, std::vector<STG_XMXY>& refindOffsetPoints);//ROTATE, SCALE, TRANSFORM
	bool GetEstimatePos(double estimatedX, double estimatedY, double& correctedX, double& correctedY);
	

	void UpdateParamValues();
	

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

	void SetAlignComplete(bool res) { alignComplete = res; }
	bool IsAlignComplete() { return alignComplete; }
	bool IsLoadingComplete() { return caliCalcInst.caliInfoLoadComplete;}
	ENG_MFOR IsNeedManualFixOffset(int* camNum = nullptr);
	CaliCalc::CaliFeature GetCalifeature(OffsetType type);
	CaliPoint EstimateAlignOffset(int camIdx, double stageX, double stageY);
	CaliPoint EstimateExpoOffset(int camIdx, double gbrX, double gbrY);
	pair<double, double> GetCamZOffset(int camIdx, float currZ);
	bool GetOffsetFromPool(OffsetType type, int tgtMarkIdx, CaliPoint& temp);
	void Destroy();

protected:
	CaliCalc caliCalcInst;
	map<string, map<string, Axis>> axises;
	bool alignComplete = false;
public:

	map<string, map<string, Axis>> GetAxises() { return axises; }

	void Update();

public:

	void Refresh();

	void LoadCaliData(LPG_CIEA cfg);
	void RevertPrevAlignMotion();
	void ResetprocessedAlignMotion();
	ENG_AMOS GerPrevAlignMotion();

	bool GetNearFid(STG_XMXY currentPos, SearchFlag flag, vector<STG_XMXY> skipList, STG_XMXY& findFid);

	bool isArrive(string drive, string axis, double dest, float threshold = 0.001);

	bool MovetoGerberPos(int camNum, STG_XMXY tgtPos, CaliPoint* offsetPos=nullptr);

	bool GetGerberPosUseCamPos(int camNum, STG_XMXY& point);
	void GetStagePosUseGerberPos(int camNum, STG_XMXY gbrPos, STG_XMXY& stagePos);
	
	void GetMarkoffsetLegacy(ENG_AMOS motionType,bool isGlobal, int markID, double& offsetX, double& offsetY);
	
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

class Environmental //환경요소.
{
public:
	bool DoCalib();
	void ReadCalibValue();
	void WriteCalibValue();

	void GetCalibPositionData(double& stageX, double& stageY, int& calibCamIdx, double& calibCamZPos, double& calibCamXPos);

	void GetCalibOffset(double& offsetX, double& offsetY);

	bool NeedCalib()
	{
		//온도변화가 있거나.
		//일정시간이 지났거나.
		return true;
	}

	bool Update()
	{
		//Stuffs::GetLastLineOfFile(thcLogLocation);
	}

private:
	string thcLogLocation;
	long reCalibTermMinute; //해당시간 경과 후 재 캘리브
	double reCalibGabTemp; //해당온도 차이나면 재 캘리브

	// 이 둘은 외부에서 가져오는값, align파일에서//
	double lastCalibDate;   //마지막 캘리브시간
	double lastCalibTemperature; //

	tuple<int, double, double> calibCamAxisPos; //캘리브레이션에 사용할 카메라와  camidx, x축좌표 , z축좌표
	tuple<double, double> calibStagePos; // (x,y)

	tuple<double, double> latestCalibOffset; //<-가장 최근에 캘리브레이션 옵셋
	tuple<double, double> prevCalibOffset; //<- 기존의 캘리브레이션 옵셋
};

class FEMRunState
{
	std::mutex lockMutex;

	HWND uiHwnd = nullptr;
	bool newInst = false;
public:

	void SetUIhandle(HWND hwnd)
	{
		lock_guard<mutex> locked(lockMutex);
		this->uiHwnd = hwnd;
		newInst = hwnd != nullptr ? true : newInst;
	}

	HWND GetHwnd(bool reset = false)
	{
		lock_guard<mutex> locked(lockMutex);
		if (reset && uiHwnd != nullptr)
			ResetFlag();

		return uiHwnd;
	}
	
	bool IsNew()
	{
		return newInst;
	}

	void ResetFlag()
	{
		newInst = false;
	}

};




class AFstate
{
public:
	enum LDStype
	{
		internal,
		external,
		none,
	};

public:
	AFstate() = default;
	AFstate(int phIdx)
	{
		phIndex = phIdx;
		isAFOn = false;
		isSensorOn = false;
		isPlottingOn = false;
		sensingAFValue = 0;
		storedAFValue = 0;
		AFworkRange[0] = 0;
		AFworkRange[1] = 0;
	}

	int phIndex;
	int sensingAFValue;
	int storedAFValue;
	int AFworkRange[2];
	bool isAFOn;
	bool isPlottingOn;
	bool isSensorOn;
	LDStype ldsType = LDStype::none;

public:
	//set
	bool SetAFSensorOnOff(bool on);
	bool SetStoredAFPosition(int position);
	bool SetAFSensorType(LDStype type);
	bool SetAFOnOff(bool on);
	bool SetAFWorkRange(int below, int above);
	bool SetPlottingOnOff(bool on);
	//get
	
	bool GetPlottingIsOn(bool& on);
	bool GetAFSensorIsOn(bool& on);
	bool GetCurrentAFSensingPosition(int& position);
	bool GetStoredAFPosition(int& position);
	bool GetAFSensorType(LDStype& type);
	bool GetAFisOn(bool& on);
	bool GetAFWorkRange(int& below, int& above);

};

class AutoFocus
{
	friend class AFstate;
	map<int, AFstate> afState;
	
	bool useAF = false;

public:

	AutoFocus() = default;
	AutoFocus(int phNum)
	{
		auto& svc = uvEng_GetConfig()->luria_svc;
		for (int i = 1; i <= phNum; i++)
			afState[i] = AFstate(i);
	}

public:
	AFstate* GetAFState(int phIndex);
	bool InitFocusDrive();
	//set
	bool SetAFSensorOnOff(int phNum, bool on);
	bool SetStoredAFPosition(int phNum, int position);
	bool SetAFOnOff(int phNum, bool on);
	bool SetAFSensorType(int phNum, AFstate::LDStype type);
	bool SetAFWorkRange(int phNum, int below, int above);
	bool SetPlottingOnOff(int phNum, bool on);

	//get
	bool GetPlottingIsOn(int phNum, bool& on);
	bool GetAFSensorIsOn(int phNum, bool& on);
	bool GetCurrentAFSensingPosition(int phNum, int& position);
	bool GetStoredAFPosition(int phNum, int& position);
	bool GetAFSensorType(int phNum, AFstate::LDStype& type);
	bool GetAFisOn(int phNum, bool& on);
	bool GetAFWorkRange(int phNum, int& below, int& above);
	
	//stats

	bool GetUseAF() { return useAF; }
	void SetUseAF(bool use) { useAF = use; }
	
	//command

	bool SetAFActivate(bool activate)
	{
		if (activate)
		{
			AFstate::LDStype _1LDStype, _2LDStype;
			vector<bool> ress;
			int phCnt = uvEng_GetConfig()->luria_svc.ph_count;
			bool res = true;

			if(uvEng_Luria_GetShMem()->focus.initialized != 1)
				res = InitFocusDrive();

			if (res == false) return res;
			
			for (int i = 0; i < phCnt; i++)
				ress.push_back(SetAFSensorType(i + 1, uvEng_GetConfig()->luria_svc.useExternalSensor ? AFstate::external : AFstate::internal));

			res = find(ress.begin(), ress.end(), false) == ress.end();
			ress.clear();

			if (res == false) return res;

			for (int i = 0; i < phCnt; i++)
				ress.push_back(SetAFSensorOnOff(i + 1, true));

			res = find(ress.begin(), ress.end(), false) == ress.end();
			ress.clear();

			if (res == false) return res;
		
			for (int i = 0; i < phCnt; i++)
				ress.push_back(SetAFOnOff(i + 1, true));
	
			res = find(ress.begin(), ress.end(), false) == ress.end();

			return res;

		}
		else
			return ClearAFActivate();
	}


	bool ClearAFActivate()
	{

		if (uvEng_Luria_GetShMem()->focus.initialized != 1)
			InitFocusDrive();

		int phCnt = uvEng_GetConfig()->luria_svc.ph_count;
		
		vector<bool> res;
		bool success = true;

		for (int i = 0; i < phCnt; i++)
			res.push_back(SetAFSensorOnOff(i + 1, false));

		if (find(res.begin(), res.end(), false) == res.end())
			for (int i = 0; i < phCnt; i++)
			{
				res.push_back(SetAFOnOff(i + 1, false));
			}

		return find(res.begin(), res.end(), false) == res.end();
	}

};

////////////////////////////////////////////////////////////
///			아진모터 통신관련
////////////////////////////////////////////////////////////
class AjinMotionNetwork
{
	enum ajinAxis : int
	{
		cam1z,cam2z,cam3z,tableTheta,maxAjinAxis
	};
	private:
		TcpProtoClient client;
		axisInfo info[maxAjinAxis];

	public:
		axisInfo* GetAxisInfo()
		{
			return client.isConnected() ? info : nullptr;
		}

		AjinMotionNetwork()
		{
			Connect();
		}

		~AjinMotionNetwork()
		{
			//Disconnect();
		}

	bool Connect();
	void BindEvent();
	void Disconnect();
	
	bool CanMovePos(int axis, bool positiveDir, string& desc);
	bool ResetMotor(int axis);
	bool MoveAbs(int axis, double pos, double velo, string& desc);
	bool MoveRel(int axis, double pos, double velo, string& desc);
	bool HomeMotor(int axis);
	bool MotorEnable(int axis, bool set);
	bool StopMotor(int axis, bool all);
};


////////////////////////////////////////////////////////////
///			세타테이블 컨트롤 관련
////////////////////////////////////////////////////////////
class ThetaControl
{
public:

	bool Use() 
	{
		return thetaFeature.use;
	}

	class nPoint
	{
		public:
			double x, y;
			nPoint() = default;
			nPoint(double x, double y)
			{
				this->x = x; this->y = y;
			}
	};
	
	using OffsetMap = std::map<double, nPoint>;


	ThetaControl()
	{
		
		SetBasicFeatures(260.7314f, 1019.5877f,
			{ { -0.2f, nPoint(-0.3681,-0.3685)},
			{-0.1f, nPoint(-0.7360,-0.7358)},
			{0, nPoint(0,0)},
			{0.1f, nPoint(0.3659,0.3684)},
			{0.2f, nPoint(0.7320,0.7370)} },
				(462.627 - 260.7314),
				(1011.032 - 1019.5877));
		
	}
	
	bool Judge();

	void SetCam1Pos(double pos);
	void SetStagePosbyFiducial(nPoint xyByFid, int fidIdx);
	void SetOffsetValue(nPoint xyByFid, int fidIdx);

	float GetMotorTheta();
	
	double GetBestTheta();
	

	

	struct feature
	{
		bool use = false;
		double centerOfStageX;
		double centerOfStageY;
		OffsetMap thetaMapping;
		float dist3to1X; 
		float dist3to1Y;

		double cam1Pos;
		nPoint stagexyByFid[2];
		nPoint offsetOfFid[2];

		bool needThetaControl = false;
		double threshold = 0.050; //50um이상
		double correctionTheta = 0; 

		feature() = default;
		feature(double centerOfStageX,
			double centerOfStageY,
			OffsetMap thetaMapping,
			float dist3to1X, float dist3to1Y)
		{
			this->centerOfStageX = centerOfStageX;
			this->centerOfStageY = centerOfStageY;
			this->thetaMapping = thetaMapping;
			this->dist3to1X = dist3to1X;
			this->dist3to1Y = dist3to1Y;
		}
	};

	void SetBasicFeatures(double centerOfStageX,
		double centerOfStageY,
		OffsetMap thetaMapping,
		float dist3to1X, float dist3to1Y);

	void ProcessThetaCorrection();
private:
	bool MoveMotorTheta(float theta, bool absMove);
	bool ResetPosition(float theta = 0.2f);

	

	static double CaclThetaRadianMakeSameX(double cx, double cy, double w1x, double w1y, double w2x, double w2y)
	{
		double u1x = w1x - cx, u1y = w1y - cy;
		double u2x = w2x - cx, u2y = w2y - cy;
		double dx = u1x - u2x;
		double dy = u1y - u2y;
		return std::atan2(dx, dy); // tanθ = dx/dy
	}

	static double rad2deg(double r) { return r * 180.0 / M_PI; }
	static double deg2rad(double d) { return d * M_PI / 180.0; }

	static double NormalizeDegree180(double d) 
	{
		while (d <= -180.0) d += 360.0;
		while (d > 180.0) d -= 360.0;
		return d;
	}
	
	double CaclThetaDegreeMakeSameX(double cx, double cy, double w1x, double w1y, double w2x, double w2y)
	{
		return NormalizeDegree180(rad2deg(CaclThetaRadianMakeSameX(cx, cy, w1x, w1y, w2x, w2y)));
	}

	feature thetaFeature;
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
	unique_ptr<Environmental> environmental;
	unique_ptr<AutoFocus> autoFocus;
	unique_ptr <AjinMotionNetwork> ajinMotion;
	unique_ptr <ThetaControl> thetaControl;
	
	template <typename MapType>
	bool IsKeyExist(const MapType& map, string key)
	{
		auto it = map.find(key);
		return it != map.end();
	}

public:
	void StartWebMonitor();

public:
	FEMRunState femRunState;

	AlignMotion& GetAlignMotion()
	{
		return *alignMotion;
	}

	RefindMotion& GetRefindMotion()
	{
		return *refindMotion;
	}

	AutoFocus& GetAutofocus()
	{
		return *autoFocus;
	}
	TriggerManager& GetTrigger()
	{
		return *triggerManager;
	}

	WebMonitor& GetWebMonitor()
	{
		return *webMonitor;
	}

	Environmental& GetEnvironment()
	{
		return *environmental;
	}

	AjinMotionNetwork& GetAjinMotion()
	{
		return *ajinMotion;
	}

	ThetaControl& GetThetaControl()
	{
		return *thetaControl;
	}

	void Destroy()
	{
		
		waitForAllWaiter();
		ThreadManager::getInstance().waitForAllThreads();

		GetAlignMotion().Destroy();
		alignMotion.reset();
		refindMotion.reset();
		triggerManager.reset();
		autoFocus.reset();
		webMonitor->StopWebServer();
		webMonitor.reset();
		environmental.reset();
		ajinMotion.reset();
		thetaControl.reset();
	}

	void waitForAllWaiter()
	{

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

	}

	bool Waiter(std::function<bool()> func, int timeoutDelay = 3000)
	{
		const int CHECKTERM = 100; int elapsedMS = 0;
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
		autoFocus = make_unique<AutoFocus>(AutoFocus(2));
		ajinMotion = make_unique<AjinMotionNetwork>();
		thetaControl = make_unique<ThetaControl>();
	}

	/*GlobalVariables()
	{
		int debug = 0;
	}*/

	map<int, string> nameTag =
	{
		{(int)ENG_AMOS::en_onthefly_2cam , "ontheFly Sidecam"},
//		{(int)ENG_AMOS::en_onthefly_3cam , "ontheFly Centercam"},
//		{(int)ENG_AMOS::en_static_2cam , "Static Sidecam"},
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

	int ResetCounter(string key, int value = 0)
	{
		/*if (IsKeyExist(counter, key) == false)
			throw new exception("no key.");*/

		counter[key].store(value);
		return GetCount(key);
	}
};