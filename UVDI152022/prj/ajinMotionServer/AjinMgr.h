#pragma once

using namespace std;

#include <vector>
#include <thread>
#include <map>

#include "Ajin/Include/AXHS_SHIM.h"
#include "Ajin/Include/AXL.h"
#include "Ajin/Include/AXM.h"
#include "Ajin/Include/AXDev.h"

#pragma comment(lib, "AXL.lib")

#define _OK	(0)
#define _ERR (-1)

#define _ON	(1)
#define _OFF (0)

#define _CW (0)
#define _CCW (1)

#define DEF_ROOT_PATH L"C:\\Users\\Admin\\source\\repos\\MFCApplication1\\x64\\Debug\\"

#pragma once

enum axis : int
{
	cam1z = 0,
	cam2z = 1,
	cam3z = 2,
	tableTheta = 3,
	axisMax

};


class CAjinMgr;

class axisStatus
{
private:
	CAjinMgr* ajinInst = nullptr;

public:
	axisStatus()
	{

	}

	axisStatus(axis axisIdx, CAjinMgr* ajinInst)
	{
		this->axisIdx = axisIdx;
		this->ajinInst = ajinInst;
	}

	enum limit:int
	{
		pos,neg,limitMax

	};
	double scale=1;
	
	double position;
	double velocity;

	bool isFault;
	bool isEnable;
	bool isInposition;
	bool isHommed;
	bool Islimit[limitMax];
	axis axisIdx;
	
	void SetState(DWORD state, double position, double velocity);



};



class CAjinMgr
{
public:
	

	static CAjinMgr& GetInstance();  // 싱글톤 접근 함수

	bool GetAxisStatus(axis axis, axisStatus& v);

private:
	CAjinMgr();                      // 생성자 private
	~CAjinMgr();                     // 소멸자 private
	std::map<axis, axisStatus> axes;
	// 복사 및 이동 금지
	CAjinMgr(const CAjinMgr&) = delete;
	CAjinMgr& operator=(const CAjinMgr&) = delete;

private:
	bool m_bIsConnect;
	long m_lIrqNo1;
	long m_lIrqNo2;
	long m_lIrqNo3;

public:
	BOOL InitAjin();
	BOOL Close();
	BOOL IsConnected(long lBoard);

	BOOL GetStatus(int nAxis, DWORD* pdwStatus, double* pdPos, double* pdVel);
	BOOL IsServoOn(int nAxis);
	BOOL IsHomeDone(int nAxis);
	BOOL PosZeroSet(int nAxis);
	BOOL Enable(int nAxis);
	BOOL Disable(int nAxis);
	BOOL MotorReset(int nAxis);
	BOOL EStop(int nAxis);
	BOOL SStop(int nAxis);

	BOOL Homming(int nAxis);
	BOOL HommingStop(int nAxis);

	BOOL MoveRel(int nAxis, double dPos, double dVel, double dAcc = 300);
	BOOL MoveAbs(int nAxis, double dPos, double dVel, double dAcc = 300);
	BOOL MultiMoveAbs(int nCount, int* nArrAxis, double* dArrPos, double* dArrVel, double* dArrAcc);
	BOOL Jogging(int nAxis, double dVel, double dAcc, BOOL nDirection);
	BOOL JoggingStop(int nAxis);
	std::thread updateThread;
	BOOL Update();
};

