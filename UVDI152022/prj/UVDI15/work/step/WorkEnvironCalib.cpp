
/*
 desc : ��� ������ ������ ����ó��
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkEnvironCalib.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


EnvironmentalCalibWork::EnvironmentalCalibWork()
	: CWorkStep()
{
	m_enWorkJobID	= ENG_BWOK::en_env_calib;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
EnvironmentalCalibWork::~EnvironmentalCalibWork()
{
}

/*
 desc : �ʱ� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL EnvironmentalCalibWork::InitWork()
{
	/* ���� ��� ���� �� �ʱ�ȭ */
	if (!CWork::InitWork())	return FALSE;

	return TRUE;
}

/*
 desc : �ֱ������� �۾� ����
 parm : None
 retn : None
*/
VOID EnvironmentalCalibWork::DoWork()
{
	auto& webMonitor = GlobalVariables::GetInstance()->GetWebMonitor();
	auto& enviromental = GlobalVariables::GetInstance()->GetEnvironment();

	vector<function<void()>> stepWork =
	{
		[&]()
		{
			m_enWorkState = SetTrigEnable(FALSE);
		},
		[&]()
		{
			m_enWorkState = IsTrigEnabled(FALSE);
		},
		[&]()
		{
			uvEng_Camera_ResetGrabbedImage();
			m_enWorkState = CameraSetCamMode(ENG_VCCM::en_grab_mode);
		},
		[&]()
		{
			double stageX ,stageY,calibCamZPos, calibCamXPos; int calibCamIdx;
			enviromental.GetCalibPositionData(stageX, stageY, calibCamIdx, calibCamZPos, calibCamXPos);

			map<int, ENG_MMDI> axisMap =
			{
				{1,ENG_MMDI::en_align_cam1},
				{2,ENG_MMDI::en_align_cam2},
				{3,ENG_MMDI::en_axis_none}
			};

			auto res = MoveCamToSafetypos(axisMap[calibCamIdx], calibCamXPos);
			m_enWorkState = res ? ENG_JWNS::en_next : ENG_JWNS::en_error;
		},
		
		[&]()
		{

		},
		[&]()
		{

		},
		[&]()
		{

		},
		[&]()
		{

		},
		[&]()
		{

		},
	};

	m_u8StepTotal = stepWork.size();

	try
	{
		stepWork[m_u8StepIt]();
	}
	catch (const std::exception&)
	{

	}

	CWork::SetWorkNext();
	CWork::CheckWorkTimeout();
}

VOID EnvironmentalCalibWork::SetWorkNext()
{
	UINT64 u64JobTime = GetTickCount64() - m_u64StartTime;

	if (CWork::GetAbort())
	{
		CWork::EndWork();
		return;
	}
	if (m_u8StepTotal == m_u8StepIt || ENG_JWNS::en_error == m_enWorkState)
	{
		m_u8StepIt = 0x00;
		m_enWorkState = ENG_JWNS::en_comp;
		CWork::EndWork();
	}
	else if(m_enWorkState == ENG_JWNS::en_next)
	{
		m_u8StepIt++;
	}

	m_u64DelayTime = GetTickCount64();
	CalcStepRate();
}
