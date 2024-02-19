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

	// �Ѽ�Ʈ ���� (IO ���Ͷ� ����)
// 	stTargetInterlock.nCommandIndex = 0;
// 	stTargetInterlock.nInterlockType = 1;
// 	stTargetInterlock.strTargetName =_T("CENTERING 1 PULL SENSOR/INPUT");
// 	stTargetInterlock.dTargetValue = FALSE
// 	stTargetInterlock.strTargetSign = _T("=");
//  m_vnterlockList.push_back(stTargetInterlock);
	// �Ѽ�Ʈ ��
	// �Ѽ�Ʈ ���� (���� ��ġ ���Ͷ� ����)
// 	stTargetInterlock.nCommandIndex = 0;
// 	stTargetInterlock.nInterlockType = 0;
// 	stTargetInterlock.strTargetName = _T("STAGE1_Y");
// 	stTargetInterlock.dTargetValue = _T("1");
// 	stTargetInterlock.strTargetSign = _T("<");
//	m_vnterlockList.push_back(stTargetInterlock);
	// �Ѽ�Ʈ ��


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
	// STAGE Y �̵� �� INTERLOCK
	if (StartMotorInterlock(ENG_MMDI::en_stage_y, _T("STAGE Y ���� �̵� �� CLAMP X1 DOWN POSITION�� �ƴϸ� �̵��� �� �����ϴ�.")))
	{
		AddCondition(EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_INPUT, _T("CLAMP X1 DOWN POSITION SENSOR"), _T("=="), FALSE);
	}

	if (StartMotorInterlock(ENG_MMDI::en_stage_y, _T("STAGE Y ���� �̵� �� CLAMP X2 DOWN POSITION�� �ƴϸ� �̵��� �� �����ϴ�.")))
	{
		AddCondition(EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_INPUT, _T("CLAMP X2 DOWN POSITION SENSOR"), _T("=="), FALSE);
	}

	if (StartMotorInterlock(ENG_MMDI::en_stage_y, _T("STAGE Y ���� �̵� �� CLAMP Y1 DOWN POSITION�� �ƴϸ� �̵��� �� �����ϴ�.")))
	{
		AddCondition(EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_INPUT, _T("CLAMP Y1 DOWN POSITION SENSOR"), _T("=="), FALSE);
	}

	if (StartMotorInterlock(ENG_MMDI::en_stage_y, _T("STAGE Y ���� �̵� �� CLAMP Y2 DOWN POSITION�� �ƴϸ� �̵��� �� �����ϴ�.")))
	{
		AddCondition(EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_INPUT, _T("CLAMP Y2 DOWN POSITION SENSOR"), _T("=="), FALSE);
	}

	if (StartMotorInterlock(ENG_MMDI::en_stage_y, _T("STAGE Y ���� �̵� �� LIFT PIN X AXIS DOWN POSITION�� �ƴϸ� �̵��� �� �����ϴ�.")))
	{
		AddCondition(EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_INPUT, _T("LIFT PIN X AXIS DOWN POSITION"), _T("=="), FALSE);
	}

	// ���Ŀ� STR�� �̸� �������ؼ� �ٸ� ����ü�� �ְ� �װɷ� InterLockCheck �ϴ°� ���ƺ���... ���� ����

	return TRUE;
}

BOOL CInterLockManager::StartMotorInterlock(ENG_MMDI enMotorindex, CString strDes, double dPos, CString strSign)
{
	for (auto& motor : m_vMotor)
	{
		if (enMotorindex == motor.DeviceNum)
		{
			ST_INTERLOCK_GROUP stGroup;
			stGroup.strInterlockDes = strSign;// _T("STAGE Y ���� �̵� �� CLAMP X1 DOWN POSITION�� �ƴϸ� �̵��� �� �����ϴ�.");
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
				if (target.eTargetType == EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_INPUT || target.eTargetType == EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_OUTPUT) // üũ ����� IO
				{
					if (!uvEng_Philhmi_IsConnected())
					{
						m_strLastError = _T("PHILHMI ��� ������ ���� �ʾ� ������ �� �ִ� �������� �Ǵ��� �Ұ��մϴ�.");
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
				else if (target.eTargetType == EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_MOTOR) // üũ ����� IO
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
						//���� ���Ӱ� ��ġ�ϴ� ���� �ε��� ��������
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
	ST_MOTION stTemp;		/* ���� �ӽ� ���� */

	m_u8ACamCount = uvEng_GetConfig()->set_cams.acam_count;		/* ī�޶� ������ �����´�. */
	m_u8HeadCount = uvEng_GetConfig()->luria_svc.ph_count;		/* Photo Head ������ �����´�. */
	m_u8StageCount = uvEng_GetConfig()->luria_svc.table_count;	/* Table�� ������ �����´�. */

	// Stage ����(x, y��) + Head ���� + Align Camera ����
	m_u8AllMotorCount = (m_u8StageCount * 2) + m_u8HeadCount + (m_u8ACamCount * 2);

	m_vMotor.clear();

	// Stage X, Y �׸� ����
	for (int i = 0; i < m_u8StageCount; i++)
	{
		stTemp.strMotorName.Format(_T("STAGE%d X"), i + 1);
		stTemp.DeviceNum = ENG_MMDI(i * 2);
		m_vMotor.push_back(stTemp);

		stTemp.strMotorName.Format(_T("STAGE%d Y"), i + 1);
		stTemp.DeviceNum = ENG_MMDI((i * 2) + 1);
		m_vMotor.push_back(stTemp);
	}

	// Align Camera X �׸� ����
	for (int i = 0; i < m_u8ACamCount; i++)
	{
		stTemp.strMotorName.Format(_T("CAM%d X"), i + 1);
		stTemp.DeviceNum = ENG_MMDI((int)ENG_MMDI::en_align_cam1 + i);
		m_vMotor.push_back(stTemp);
	}

	// Photo Head Z �׸� ����
	for (int i = 0; i < m_u8HeadCount; i++)
	{
		stTemp.strMotorName.Format(_T("PH%d Z"), i + 1);
		stTemp.DeviceNum = ENG_MMDI((int)ENG_MMDI::en_axis_ph1 + i);
		m_vMotor.push_back(stTemp);
	}

	// Align Camera Z �׸� ����
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

	if (stCommandInterlock.eCommandType == EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_MOTOR) // üũ ����� IO
	{
		if (CheckCondition(dValue, stCommandInterlock.strCommandSign, stCommandInterlock.dCommandValue))
		{
			return TRUE;
		}
	}
// IO CMD �� INTERLOCK�� �̱���
// 	else if (stCommandInterlock.eCommandType == eINTERLOCK_TYPE_INPUT) // üũ ����� IO
// 	{
// 		if (CheckCondition(dValue, stCommandInterlock.strCommandSign, stCommandInterlock.dCommandValue))
// 		{
// 			return TRUE;
// 		}
// 	}
// 	else if (stCommandInterlock.eCommandType == eINTERLOCK_TYPE_OUTPUT) // üũ ����� IO
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
