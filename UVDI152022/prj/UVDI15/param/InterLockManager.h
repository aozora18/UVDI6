#pragma once

#include <vector>
#include <string>
#include "IOManager.h"
enum class EN_INTERLOCK_TYPE : int
{
	eINTERLOCK_TYPE_MOTOR = 0,
	eINTERLOCK_TYPE_INPUT,
	eINTERLOCK_TYPE_OUTPUT,
	eINTERLOCK_MAX
};

struct ST_MOTION
{
	ENG_MMDI DeviceNum;
	CString strMotorName;
};

typedef struct ST_TARGET_INTERLOCK
{
	ST_TARGET_INTERLOCK()
	{
		nTargetIndex = -1;
		dTargetValue = 0;
		strTargetSign = _T("=");
		eTargetType = EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_INPUT;
	}
	
	EN_INTERLOCK_TYPE			eTargetType;
	CString						strTargetName;
	int							nTargetIndex;
	double						dTargetValue;
	CString						strTargetSign;
}ST_TARGET_INTERLOCK;

typedef std::vector < ST_TARGET_INTERLOCK > vST_TARGET_INTERLOCK;

typedef struct ST_INTERLOCK_GROUP
{
	ST_INTERLOCK_GROUP()
	{
		nCommandIndex = -1;
		eCommandType = EN_INTERLOCK_TYPE::eINTERLOCK_TYPE_MOTOR;
		dCommandValue = 0;
		dInterlockValue = 0;
		strCommandSign = _T("");		// 빈값을때는 모든 상황에서 체크하지 않음
	}
	CString				strInterlockDes;
	int					nCommandIndex;
	CString				strCommandName;
	EN_INTERLOCK_TYPE	eCommandType;
	double				dInterlockValue;
	double				dCommandValue;
	CString				strCommandSign;		// 빈값을때는 CommandValue 상관없이 모든 상황에서 인터락체크하지 않음

	vST_TARGET_INTERLOCK vTargetInterlock;

}ST_INTERLOCK_GROUP;
typedef std::vector < ST_INTERLOCK_GROUP > vST_INTERLOCK_LIST;


class CInterLockManager
{
private:
	HWND				m_hMainWnd;
	CString				m_strLastError;
public:
	void				Init(HWND hWnd);
	void				Destroy();


	vST_INTERLOCK_LIST		m_vInterlockList;
	vector<ST_MOTION>		m_vMotor;				/* 모션 정보 */

	UINT8				m_u8ACamCount;			/* 설정된 카메라 개수 */
	UINT8				m_u8HeadCount;			/* 설정된 헤드 개수 */
	UINT8				m_u8StageCount;			/* 설정된 테이블 개수 */
	UINT8				m_u8AllMotorCount;		/* 전체 모터 개수 */

	BOOL				LoadInterlcok();
	BOOL				StartMotorInterlock(ENG_MMDI enMotorindex, CString strDes, double dPos = 0, CString strSign = _T(""));
	BOOL				AddCondition(EN_INTERLOCK_TYPE eType, CString strName, CString strSign, double Value);
	BOOL				AddCondition(EN_INTERLOCK_TYPE eType, int nTargetIndex, CString strSign, double Value);

	BOOL				CheckMoveInterlock(ENG_MMDI nMotorIndex, double dPos);
	CString				GetLastError() { return m_strLastError; }

	VOID				InitMotionIndex();

	BOOL				CheckCommandCondition(ST_INTERLOCK_GROUP& stCommandInterlock, double dValue);
	BOOL				CheckCondition(double dValue1, CString strSign, double dValue2);

protected:
	CInterLockManager();
	CInterLockManager(const CInterLockManager&); // Prohibit Copy Constructor
	CInterLockManager& operator =(const CInterLockManager&);

public:
	virtual ~CInterLockManager(void);

	static CInterLockManager* GetInstance()
	{
		static CInterLockManager _inst;
		return &_inst;
	}
};


