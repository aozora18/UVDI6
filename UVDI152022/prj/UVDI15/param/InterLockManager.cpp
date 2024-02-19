#include "pch.h"
#include "InterLockManager.h"
#include <locale.h>
#include <bitset>

#define DEF_INTERLOCK_ALL_CHECK _T("")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CInterLockManager::CInterLockManager()
{
	m_hMainWnd = NULL;
	m_vMotor.clear();
}


CInterLockManager::~CInterLockManager()
{
	for (auto& group : m_vInterlockList)
	{
		group.vTargetInterlock.clear();
	}
	m_vInterlockList.clear();
	m_vInterlockList.shrink_to_fit();
	m_vMotor.clear();
	m_vMotor.shrink_to_fit();
}

void CInterLockManager::Init(HWND hWnd)
{
	m_hMainWnd = hWnd;
}

void CInterLockManager::Destroy()
{
	m_vMotor.clear();
	m_vMotor.shrink_to_fit();
}

BOOL CInterLockManager::LoadInterlcok()
{
	InitMotionIndex();

	// 한셋트 시작 (IO 인터락 예제)
// 	stTargetInterlock.nCommandIndex = 0;
// 	stTargetInterlock.nInterlockType = 1;
// 	stTargetInterlock.strTargetName =_T("CENTERING 1 PULL SENSOR/INPUT");
// 	stTargetInterlock.dTargetValue = FALSE
// 	stTargetInterlock.strTargetSign = _T("=");
//  m_vnterlockList.push_back(stTargetInterlock);
	// 한세트 끝
	// 한셋트 시작 (모터 위치 인터락 예제)
// 	stTargetInterlock.nCommandIndex = 0;
// 	stTargetInterlock.nInterlockType = 0;
// 	stTargetInterlock.strTargetName = _T("STAGE1_Y");
// 	stTargetInterlock.dTargetValue = _T("1");
// 	stTargetInterlock.strTargetSign = _T("<");
//	m_vnterlockList.push_back(stTargetInterlock);
	// 한세트 끝


// 	stInterlock.nCommandIndex = (int)ENG_MMDI::en_stage_y;
// 	stInterlock.nInterlockType = eINTERLOCK_TYPE_INPUT;
// 	stInterlock.strTargetName = _T("CLAMP X1 DOWN POSITION SENSOR");
// 	stInterlock.dTargetValue = FALSE;
// 	stInterlock.strTargetSign = _T("=");
// 	m_vnterlockList.push_back(stInterlock);
// 
// 	stInterlock.nCommandIndex = (int)ENG_MMDI::en_stage_y;
// 	stInterlock.nInterlockType = eINTERLOCK_TYPE_INPUT;
// 	stInterlock.strTargetName = _T("CLAMP X2 DOWN POSITION SENSOR");
// 	stInterlock.dTargetValue = FALSE;
// 	stInterlock.strTargetSign = _T("=");
// 	m_vnterlockList.push_back(stInterlock);
// 
// 	stInterlock.nCommandIndex = (int)ENG_MMDI::en_stage_y;
// 	stInterlock.nInterlockType = eINTERLOCK_TYPE_INPUT;
// 	stInterlock.strTargetName = _T("CLAMP Y1 DOWN POSITION SENSOR");
// 	stInterlock.dTargetValue = FALSE;
// 	stInterlock.strTargetSign = _T("=");
// 	m_vnterlockList.push_back(stInterlock);
// 
// 	stInterlock.nCommandIndex = (int)ENG_MMDI::en_stage_y;
// 	stInterlock.nInterlockType = eINTERLOCK_TYPE_INPUT;
// 	stInterlock.strTargetName = _T("CLAMP Y2 DOWN POSITION SENSOR");
// 	stInterlock.dTargetValue = FALSE;
// 	stInterlock.strTargetSign = _T("=");
// 	m_vnterlockList.push_back(stInterlock);
// 
// 	stInterlock.nCommandIndex = (int)ENG_MMDI::en_stage_y;
// 	stInterlock.nInterlockType = eINTERLOCK_TYPE_INPUT;
// 	stInterlock.strTargetName = _T("LIFT PIN X AXIS DOWN POSITION");
// 	stInterlock.dTargetValue = FALSE;
// 	stInterlock.strTargetSign = _T("=");
// 	m_vnterlockList.push_back(stInterlock);

	//////////////////////////////////////////////////////////////////////////
	// STAGE Y 이동 시 INTERLOCK
	if (StartMotorInterlock(ENG_MMDI::en_stage_y, _T("STAGE Y 모터 이동 시 CLAMP X1 DOWN POSITION이 아니면 이동할 수 없습니다.")))
	{
		AddCondition(EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_INPUT, _T("CLAMP X1 DOWN POSITION SENSOR"), _T("=="), FALSE);
	}

	if (StartMotorInterlock(ENG_MMDI::en_stage_y, _T("STAGE Y 모터 이동 시 CLAMP X2 DOWN POSITION이 아니면 이동할 수 없습니다.")))
	{
		AddCondition(EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_INPUT, _T("CLAMP X2 DOWN POSITION SENSOR"), _T("=="), FALSE);
	}

	if (StartMotorInterlock(ENG_MMDI::en_stage_y, _T("STAGE Y 모터 이동 시 CLAMP Y1 DOWN POSITION이 아니면 이동할 수 없습니다.")))
	{
		AddCondition(EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_INPUT, _T("CLAMP Y1 DOWN POSITION SENSOR"), _T("=="), FALSE);
	}

	if (StartMotorInterlock(ENG_MMDI::en_stage_y, _T("STAGE Y 모터 이동 시 CLAMP Y2 DOWN POSITION이 아니면 이동할 수 없습니다.")))
	{
		AddCondition(EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_INPUT, _T("CLAMP Y2 DOWN POSITION SENSOR"), _T("=="), FALSE);
	}

	if (StartMotorInterlock(ENG_MMDI::en_stage_y, _T("STAGE Y 모터 이동 시 LIFT PIN X AXIS DOWN POSITION이 아니면 이동할 수 없습니다.")))
	{
		AddCondition(EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_INPUT, _T("LIFT PIN X AXIS DOWN POSITION"), _T("=="), FALSE);
	}

	// 이후에 STR을 미리 컨버팅해서 다른 구조체에 넣고 그걸로 InterLockCheck 하는게 나아보임... 추후 수정

	return TRUE;
}

BOOL CInterLockManager::StartMotorInterlock(ENG_MMDI enMotorindex, CString strDes, double dPos, CString strSign)
{
	for (auto& motor : m_vMotor)
	{
		if (enMotorindex == motor.DeviceNum)
		{
			ST_INTERLOCK_GROUP stGroup;
			stGroup.strInterlockDes = strSign;// _T("STAGE Y 모터 이동 시 CLAMP X1 DOWN POSITION이 아니면 이동할 수 없습니다.");
			stGroup.nCommandIndex = (int)enMotorindex;
			stGroup.eCommandType = EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_MOTOR;
			stGroup.dCommandValue = dPos;
			stGroup.strCommandSign = strSign;
			stGroup.strInterlockDes = strDes;

			m_vInterlockList.push_back(stGroup);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CInterLockManager::AddCondition(EN_INTERLOCK_TYPE eType, CString strName, CString strSign, double Value)
{
	if (0 == m_vInterlockList.size())
	{
		return FALSE;
	}

	ST_TARGET_INTERLOCK stTargetInterlock;
	stTargetInterlock.eTargetType = eType;
	stTargetInterlock.strTargetName = strName;
	stTargetInterlock.dTargetValue = Value;
	stTargetInterlock.strTargetSign = strSign;
	m_vInterlockList.at(m_vInterlockList.size() - 1).vTargetInterlock.push_back(stTargetInterlock);

	return TRUE;
}

BOOL CInterLockManager::AddCondition(EN_INTERLOCK_TYPE eType, int nTargetIndex, CString strSign, double Value)
{
	if (0 == m_vInterlockList.size())
	{
		return FALSE;
	}

	ST_TARGET_INTERLOCK stTargetInterlock;
	stTargetInterlock.eTargetType = eType;
	stTargetInterlock.nTargetIndex = nTargetIndex;
	stTargetInterlock.dTargetValue = Value;
	stTargetInterlock.strTargetSign = strSign;
	m_vInterlockList.at(m_vInterlockList.size() - 1).vTargetInterlock.push_back(stTargetInterlock);

	return TRUE;
}


BOOL CInterLockManager::CheckMoveInterlock(ENG_MMDI enMotorindex, double dPos)
{
	CIOManager::GetInstance()->UpdateIO();

	for (auto& list : m_vInterlockList)
	{
		if (int(enMotorindex) == list.nCommandIndex)
		{
			if (CheckCommandCondition(list, dPos))
			{
				continue;
			}

			for (auto& target : list.vTargetInterlock)
			{
				if (target.eTargetType == EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_INPUT || target.eTargetType == EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_OUTPUT) // 체크 대상이 IO
				{
					if (!uvEng_Philhmi_IsConnected())
					{
						m_strLastError = _T("PHILHMI 통신 연결이 되지 않아 움직일 수 있는 상태인지 판단이 불가합니다.");
						return TRUE;
					}

					ST_IO IO;
					EN_IO_TYPE eType = EN_IO_TYPE((int)target.eTargetType - (int)EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_INPUT);

					if (target.strTargetName.IsEmpty())
					{
						CIOManager::GetInstance()->GetIO(eType, target.nTargetIndex, IO);
					}
					else
					{
						CIOManager::GetInstance()->GetIO(eType, target.strTargetName, IO);
					}

					if (0 > IO.nIndex)
					{
						continue;
					}

					if (CheckCondition(IO.bOn, target.strTargetSign, target.dTargetValue))
					{
						m_strLastError = list.strInterlockDes;
						return TRUE;
					}
				}
				else if (target.eTargetType == EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_MOTOR) // 체크 대상이 IO
				{
					if (target.strTargetName.IsEmpty())
					{
						double dValue1 = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(target.nTargetIndex));

						if (CheckCondition(dValue1, target.strTargetSign, target.dTargetValue))
						{
							m_strLastError = list.strInterlockDes;
							return TRUE;
						}
					}
					else
					{
						//모터 네임과 일치하는 모터 인덱스 가져오기
						for (auto& motor : m_vMotor)
						{
							if (motor.strMotorName.Compare(target.strTargetName) == 0)
							{
								double dValue1 = uvCmn_MC2_GetDrvAbsPos(motor.DeviceNum);

								if (CheckCondition(dValue1, target.strTargetSign, target.dTargetValue))
								{
									m_strLastError = list.strInterlockDes;
									return TRUE;
								}
							}
						}
					}
				}
			}
		}
	}

	m_strLastError = _T("NO ERROR");

	return FALSE;
}

VOID CInterLockManager::InitMotionIndex()
{
	ST_MOTION stTemp;		/* 모터 임시 변수 */

	m_u8ACamCount = uvEng_GetConfig()->set_cams.acam_count;		/* 카메라 개수를 가져온다. */
	m_u8HeadCount = uvEng_GetConfig()->luria_svc.ph_count;		/* Photo Head 개수를 가져온다. */
	m_u8StageCount = uvEng_GetConfig()->luria_svc.table_count;	/* Table의 개수를 가져온다. */

	// Stage 개수(x, y축) + Head 개수 + Align Camera 개수
	m_u8AllMotorCount = (m_u8StageCount * 2) + m_u8HeadCount + (m_u8ACamCount * 2);

	m_vMotor.clear();

	// Stage X, Y 항목 저장
	for (int i = 0; i < m_u8StageCount; i++)
	{
		stTemp.strMotorName.Format(_T("STAGE%d X"), i + 1);
		stTemp.DeviceNum = ENG_MMDI(i * 2);
		m_vMotor.push_back(stTemp);

		stTemp.strMotorName.Format(_T("STAGE%d Y"), i + 1);
		stTemp.DeviceNum = ENG_MMDI((i * 2) + 1);
		m_vMotor.push_back(stTemp);
	}

	// Align Camera X 항목 저장
	for (int i = 0; i < m_u8ACamCount; i++)
	{
		stTemp.strMotorName.Format(_T("CAM%d X"), i + 1);
		stTemp.DeviceNum = ENG_MMDI((int)ENG_MMDI::en_align_cam1 + i);
		m_vMotor.push_back(stTemp);
	}

	// Photo Head Z 항목 저장
	for (int i = 0; i < m_u8HeadCount; i++)
	{
		stTemp.strMotorName.Format(_T("PH%d Z"), i + 1);
		stTemp.DeviceNum = ENG_MMDI((int)ENG_MMDI::en_axis_ph1 + i);
		m_vMotor.push_back(stTemp);
	}

	// Align Camera Z 항목 저장
	for (int i = 0; i < m_u8ACamCount; i++)
	{
		stTemp.strMotorName.Format(_T("CAM%d Z"), i + 1);
		stTemp.DeviceNum = ENG_MMDI((int)ENG_MMDI::en_axis_acam1 + i);
		m_vMotor.push_back(stTemp);
	}
}

BOOL CInterLockManager::CheckCommandCondition(ST_INTERLOCK_GROUP& stCommandInterlock, double dValue)
{
	if (stCommandInterlock.strCommandSign.IsEmpty())
	{
		return FALSE;
	}

	if (stCommandInterlock.eCommandType == EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_MOTOR) // 체크 대상이 IO
	{
		if (CheckCondition(dValue, stCommandInterlock.strCommandSign, stCommandInterlock.dCommandValue))
		{
			return TRUE;
		}
	}
// IO CMD 시 INTERLOCK은 미구현
// 	else if (stCommandInterlock.eCommandType == eINTERLOCK_TYPE_INPUT) // 체크 대상이 IO
// 	{
// 		if (CheckCondition(dValue, stCommandInterlock.strCommandSign, stCommandInterlock.dCommandValue))
// 		{
// 			return TRUE;
// 		}
// 	}
// 	else if (stCommandInterlock.eCommandType == eINTERLOCK_TYPE_OUTPUT) // 체크 대상이 IO
// 	{
// 		if (CheckCondition(dValue, stCommandInterlock.strCommandSign, stCommandInterlock.dCommandValue))
// 		{
// 			return TRUE;
// 		}
// 	}

	return FALSE;
}

BOOL CInterLockManager::CheckCondition(double dValue1, CString strTargetSign, double dValue2)
{
	if (0 == strTargetSign.Compare(_T("=")) || 0 == strTargetSign.Compare(_T("==")))
	{
		if (0.01 > fabs(dValue1 - dValue2))
		{
			return TRUE;
		}
	}
	else if (0 == strTargetSign.Compare(_T("!=")))
	{
		if (0.01 < fabs(dValue1 - dValue2))
		{
			return TRUE;
		}
	}
	else if (0 == strTargetSign.Compare(_T("<")))
	{
		if (dValue1 < dValue2 - 0.01)
		{
			return TRUE;
		}
	}
	else if (0 == strTargetSign.Compare(_T(">")))
	{
		if (dValue1 > dValue2 + 0.01)
		{
			return TRUE;
		}
	}

	return FALSE;
}
