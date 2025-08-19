
#include "stdafx.h"

#include "AjinMgr.h"
#include <atlstr.h>
#include <thread>

using namespace std;



void axisStatus::SetState(DWORD state, double position, double velocity)
{
	isInposition = state & QIMECHANICAL_INP_LEVEL ? true : false;
	isFault = state & QIMECHANICAL_ALARM_LEVEL ? true : false;
	Islimit[pos] = state & QIMECHANICAL_PELM_LEVEL ? true : false;
	Islimit[neg] = state & QIMECHANICAL_NELM_LEVEL ? true : false;
	this->position = position / scale;
	this->velocity = velocity;
	isEnable = ajinInst->IsServoOn((int)axisIdx);
	isHommed = ajinInst->IsHomeDone((int)axisIdx);
}


CAjinMgr& CAjinMgr::GetInstance()
{
	static CAjinMgr instance;
	return instance;
}

CAjinMgr::CAjinMgr()
{


}

CAjinMgr::~CAjinMgr()
{
	if(m_bIsConnect)
	Close();
}

bool CAjinMgr::GetAxisStatus(axis axis, axisStatus& v)
{
	auto find = axes.find(axis);
	
	if(find == axes.end())
		return false;

	v = find->second;
	return true;
}


BOOL CAjinMgr::InitAjin()
{
	CString strPath;

	m_bIsConnect = false;
	m_lIrqNo1 = 7;

	axes.insert(make_pair(cam1z, axisStatus(cam1z, &CAjinMgr::GetInstance())));
	axes.insert(make_pair(cam2z, axisStatus(cam2z, &CAjinMgr::GetInstance())));
	axes.insert(make_pair(cam3z, axisStatus(cam3z, &CAjinMgr::GetInstance())));
	axes.insert(make_pair(tableTheta, axisStatus(tableTheta, &CAjinMgr::GetInstance())));

	if (TRUE == AxlIsOpened())
	{
		if (AXT_RT_SUCCESS != AxlOpenNoReset(m_lIrqNo1))
		{
			return FALSE;
		}

		m_bIsConnect = true;
	}
	else
	{

		DWORD res = AxlOpen(m_lIrqNo1);
		if (AXT_RT_SUCCESS != res)
		{

			char desc[255]; long recv;
			AxlGetReturnCodeInfo(res, 255, &recv, desc);

			return FALSE;
		}

		strPath.Format(_T("%sAjinParam.mot"), DEF_ROOT_PATH);
		USES_CONVERSION;
		if (AXT_RT_SUCCESS != AxmMotLoadParaAll(T2A(strPath)))
		{
			return FALSE;
		}

		m_bIsConnect = true;
		Update();
	}

	return TRUE;
}

BOOL CAjinMgr::Close()
{
	
	if (TRUE == AxlClose())
	{
		m_bIsConnect = false;
		Sleep(1000);

		if (updateThread.joinable())
			updateThread.join();

		return TRUE;
	}
	
	


	return FALSE;
}

BOOL CAjinMgr::IsConnected(long lBoard)
{
	if (AXT_RT_SUCCESS != AxlGetBoardStatus(lBoard))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CAjinMgr::GetStatus(int nAxis, DWORD* pdwStatus, double* pdPos, double* pdVel)
{
	double dVel;
	DWORD dwRet = 0;
	MOTION_INFO stState;
	stState.dwMask = 0x1F;
	dwRet |= AxmStatusReadMotionInfo(nAxis, &stState);
	dwRet |= AxmStatusReadVel(nAxis, &dVel);

	if (_OK == dwRet)
	{
		*pdwStatus = stState.dwMechSig;
		*pdPos = stState.dActPos;
		*pdVel = dVel;

		return TRUE;
	}
	
	return FALSE;
}

BOOL CAjinMgr::IsServoOn(int nAxis)
{
	DWORD dwStatus;
	AxmSignalIsServoOn(nAxis, &dwStatus);

	return (BOOL)dwStatus;
}

BOOL CAjinMgr::IsHomeDone(int nAxis)
{
	DWORD dwStatus;
	AxmHomeGetResult(nAxis, &dwStatus);

	return (BOOL)dwStatus;
}

BOOL CAjinMgr::Enable(int nAxis)
{
	if (true != m_bIsConnect)
	{
		return FALSE;
	}
	
	if (AXT_RT_SUCCESS != AxmSignalServoOn(nAxis, _ON))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CAjinMgr::PosZeroSet(int nAxis)
{
	if (true != m_bIsConnect)
	{
		return FALSE;
	}

	if (AXT_RT_SUCCESS != AxmSignalServoOn(nAxis, _ON))
	{
		return FALSE;
	}

	if (AXT_RT_SUCCESS != AxmStatusSetPosMatch(nAxis, 0))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CAjinMgr::Disable(int nAxis)
{
	if (true != m_bIsConnect)
	{
		return FALSE;
	}

	DWORD dwStatus;
	if (AXT_RT_SUCCESS != AxmStatusReadMechanical(nAxis, &dwStatus))
	{
		return FALSE;
	}

	if (dwStatus & QIMECHANICAL_ALARM_LEVEL ? TRUE : FALSE)
	{
		if (AXT_RT_SUCCESS != AxmSignalServoAlarmReset(nAxis, TRUE))
		{
			return FALSE;
		}
	}
	else
	{
		if (AXT_RT_SUCCESS != AxmSignalServoOn(nAxis, _OFF))
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CAjinMgr::MotorReset(int nAxis)
{
	if (true != m_bIsConnect)
	{
		return FALSE;
	}

	if (AXT_RT_SUCCESS != AxmSignalServoAlarmReset(nAxis, TRUE) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CAjinMgr::EStop(int nAxis)
{
	if (true != m_bIsConnect)
	{
		return FALSE;
	}

	if (AXT_RT_SUCCESS != AxmMoveEStop(nAxis))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CAjinMgr::SStop(int nAxis)
{
	if (true != m_bIsConnect)
	{
		return FALSE;
	}

	if (AXT_RT_SUCCESS != AxmMoveSStop(nAxis))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CAjinMgr::Homming(int nAxis)
{
	if (true != m_bIsConnect)
	{
		return FALSE;
	}

	if (AXT_RT_SUCCESS != AxmSignalServoOn(nAxis, _ON))
	{
		return FALSE;
	}

	if (AXT_RT_SUCCESS != AxmHomeSetStart(nAxis))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CAjinMgr::HommingStop(int nAxis)
{
	return SStop(nAxis);
}

BOOL CAjinMgr::MoveRel(int nAxis, double dPos, double dVel, double dAcc)
{
	if (true != m_bIsConnect)
	{
		return FALSE;
	}

	double dAccRate = dVel / (dAcc / 1000.0);

	// Rmove mode setting
	if (AXT_RT_SUCCESS != AxmMotSetAbsRelMode(nAxis, POS_REL_MODE))
	{
		return FALSE;
	}

	if (AXT_RT_SUCCESS != AxmMoveStartPos(nAxis, dPos, dVel, dAccRate, dAccRate))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CAjinMgr::MoveAbs(int nAxis, double dPos, double dVel, double dAcc)
{
	if (true != m_bIsConnect)
	{
		return FALSE;
	}

	double dAccRate = dVel / (dAcc / 1000.0);

	// Amove mode setting
	if (AXT_RT_SUCCESS != AxmMotSetAbsRelMode(nAxis, POS_ABS_MODE))
	{
		return FALSE;
	}

	if (AXT_RT_SUCCESS != AxmMoveStartPos(nAxis, dPos, dVel, dAccRate, dAccRate))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CAjinMgr::MultiMoveAbs(int nCount, int *nArrAxis, double *dArrPos, double *dArrVel, double *dArrAcc)
{
	if (true != m_bIsConnect)
	{
		return FALSE;
	}

	double *dArrAccRate = new double[nCount];

	// Amove mode setting
	for (int i = 0; i < nCount; i++)
	{
		dArrAccRate[i] = dArrVel[i] / (dArrAcc[i] / 1000.0);

		if (AXT_RT_SUCCESS != AxmMotSetAbsRelMode(nArrAxis[i], POS_ABS_MODE))
		{
			delete[] dArrAccRate;
			return FALSE;
		}
	}

	if (AXT_RT_SUCCESS != AxmMoveStartMultiPos(nCount, (long*)nArrAxis, dArrPos, dArrVel, dArrAccRate, dArrAccRate))
	{
		delete[] dArrAccRate;
		return FALSE;
	}

	delete[] dArrAccRate;

	return TRUE;
}

BOOL CAjinMgr::Jogging(int nAxis, double dVel, double dAcc, BOOL bDirection)
{
	if (true != m_bIsConnect)
	{
		return FALSE;
	}

	double dAccRate = dVel / (dAcc / 1000.0);

	if (_CW == bDirection)
	{
		if (AXT_RT_SUCCESS != AxmMoveVel(nAxis, dVel * -1., dAccRate, dAccRate))
		{
			return FALSE;
		}
	}
	else
	{
		if (AXT_RT_SUCCESS != AxmMoveVel(nAxis, dVel, dAccRate, dAccRate))
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CAjinMgr::JoggingStop(int nAxis)
{
	return SStop(nAxis);
}


BOOL CAjinMgr::Update()
{
	
	updateThread = std::thread([&]()
		{
			while (m_bIsConnect)
			{
				double pos = 0.;
				double vel = 0.;
				DWORD status = 0;

				for(int i= cam1z ;i<axisMax;i++)
				if (TRUE == GetStatus(i, &status, &pos, &vel))
				{
					axes[(axis)i].SetState(status, pos, vel);
				}
				Sleep(10);
			}
		});
	return 0;
}
