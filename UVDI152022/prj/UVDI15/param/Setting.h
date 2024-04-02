#pragma once
//#include "SettingDefine.h"
//#include "SettingDefineReserve.h"
#include "enum.h"



JHMI_ENUM(EN_SETTING_TAB, int
	, OPTION
	, MAX
)


#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
JHMI_ENUM(EN_SETTING_OPTION, int
	, USE_AF
	, AF_GAIN
	, AF_RANGE_MIN
	, AF_RANGE_MAX
	, CAM_1_GAIN_LEVEL
	, CAM_2_GAIN_LEVEL
	, TIME_GRAB_1
	, TIME_STEP_1
	, TIME_GRAB_2
	, TIME_STEP_2
	, USE_2D_CALI_DATA
	, MARK2_ORG_GERB_X
	, MARK2_ORG_GERB_Y
	, MARK2_STAGE_X
	, SCORE_RATE
	, SCALE_RATE
	, STROBE_LAMP_1
	, STROBE_LAMP_2
	, STROBE_LAMP_3
	, STROBE_LAMP_4
	, STROBE_LAMP_5
	, STROBE_LAMP_6
	, USE_THICK_CHECK
	, RANG_START_Y
	, RANG_END_Y
	, OFFSET_Z
	, LIMIT_Z
	, MAX
)
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
JHMI_ENUM(EN_SETTING_OPTION, int
	, USE_AF
	, AF_GAIN
	, AF_RANGE_MIN
	, AF_RANGE_MAX
	, CAM_1_GAIN_LEVEL
	, CAM_2_GAIN_LEVEL
	, CAM_3_GAIN_LEVEL
	, TIME_GRAB_1
	, TIME_STEP_1
	, TIME_GRAB_2
	, TIME_STEP_2
	, TIME_GRAB_3
	, TIME_STEP_3
	, USE_2D_CALI_DATA
	, MARK2_ORG_GERB_X
	, MARK2_ORG_GERB_Y
	, MARK2_STAGE_X
	, SCORE_RATE
	, SCALE_RATE
	, STROBE_LAMP_1
	, STROBE_LAMP_2
	, STROBE_LAMP_3
	, STROBE_LAMP_4
	, STROBE_LAMP_5
	, STROBE_LAMP_6
	, USE_THICK_CHECK
	, RANG_START_Y
	, RANG_END_Y
	, OFFSET_Z
	, LIMIT_Z
	, MAX
)
#endif

JHMI_ENUM(EN_SETTING_GROUP, int
	//, COMMON
	, PH_SERVICE
	//, UVDI15_COMN
	, CAMERA_BASLER
	//, TRIG_GRAB
	, SETUP_ALIGN
	//, GLOBAL_TRANS
	, MARK_FIND
	, STROBE_LAMP
	, MAX
)


#define DEF_DATA_TYPE_BOOL					_T("BOOL")
#define DEF_DATA_TYPE_INT					_T("INT")
#define DEF_DATA_TYPE_DOUBLE				_T("DOUBLE")
#define DEF_DATA_TYPE_STRING				_T("STRING")

#define DEF_INVALID_DATA							-32767

//////////////////////////////////////////////////////////////////////////
//1. 레시피 - 탭 - 파라미터 구조
typedef struct ST_SETTING_PARAM
{
	ST_SETTING_PARAM() 
	{
		bShow			= FALSE;
		bUse			= FALSE;
		dScale			= 0;
		dChangeLimit	= 0;
		dMin			= 0;
		dMax			= 10000;
		eUILevel		= eLOGIN_LEVEL_UNKNOWN;
		strDescript		= _T("설정되지 않은 파라메터입니다.");
		nArrCnt			= 0;
		nIndexParam		= -1;
		nIndexGroup		= -1;
		strValue.Format(_T("%d"), DEF_INVALID_DATA);
	}

	ST_SETTING_PARAM(ST_SETTING_PARAM* pStParam)
	{		
		strGroup		= pStParam->strGroup;
		strName			= pStParam->strName;
		strDisplayName	= pStParam->strDisplayName;
		strCIMName		= pStParam->strCIMName;
		strDataType		= pStParam->strDataType;
		strUnit			= pStParam->strUnit;
		bShow			= pStParam->bShow;
		bUse			= pStParam->bUse;
		strDefaultValue = pStParam->strDefaultValue;
		strValue		= pStParam->strValue;
		dScale			= pStParam->dScale;
		dChangeLimit	= pStParam->dChangeLimit;
		dMin			= pStParam->dMin;
		dMax			= pStParam->dMax;
		eUILevel		= pStParam->eUILevel;
		strDescript		= pStParam->strDescript;
		nArrCnt			= pStParam->nArrCnt;
		nIndexParam		= pStParam->nIndexParam;
	}

	void Set(ST_SETTING_PARAM stParam)
	{
		strGroup		= stParam.strGroup;
		strName			= stParam.strName;
		strDisplayName	= stParam.strDisplayName;
		strCIMName		= stParam.strCIMName;
		strDataType		= stParam.strDataType;
		strUnit			= stParam.strUnit;
		bShow			= stParam.bShow;
		bUse			= stParam.bUse;
		strDefaultValue = stParam.strDefaultValue;
		strValue		= stParam.strValue;
		dScale			= stParam.dScale;
		dChangeLimit	= stParam.dChangeLimit;
		dMin			= stParam.dMin;
		dMax			= stParam.dMax;
		eUILevel		= stParam.eUILevel;
		strDescript		= stParam.strDescript;
		nArrCnt			= stParam.nArrCnt;
		nIndexParam		= stParam.nIndexParam;
	}

	CString					GetDefaultValue()				{ return strDefaultValue;					}
	int						GetDefaultInt()					{ return _ttoi(strDefaultValue);			}
	double					GetDefaultDouble()				{ return _ttof(strDefaultValue);			}
	CString					GetValue()						{ return strValue;							}
	int						GetInt()						{ return _ttoi(strValue);					}
	double					GetDouble()						{ return _ttof(strValue);					}
	BOOL					SetValue(CString strValueRcv)
	{
		if (DEF_DATA_TYPE_BOOL == strDataType || DEF_DATA_TYPE_INT == strDataType)
		{
			if (dMin > _ttoi(strValueRcv) || dMax < _ttoi(strValueRcv))
			{
				return FALSE;
			}
// 			else if (0 != dChangeLimit && fabs(_ttoi(strValueRcv) - _ttoi(strValue)) > dChangeLimit) 
// 			{
// 				return FALSE;
// 			}
			else
			{
				strValue.Format(_T("%d"), _ttoi(strValueRcv));
			}
			//Setting은 UI에서 관리함.. 이유는 임시저장을 따로 구현해서 ChangeLimit을 여기서 검사할 필요가없다.
			//또한 여기서 관리하게되면, ChageLimit이하로 여러번 변경하여 ChangeLimit을 이상 변경한뒤 저장을 누를경우 변경된값이 저장되지 않음
		}
		else if (DEF_DATA_TYPE_DOUBLE == strDataType) //Num
		{
			if (dMin > _ttof(strValueRcv) || dMax < _ttof(strValueRcv))
			{
				return FALSE;
			}
// 			else if (0 != dChangeLimit && fabs(_ttof(strValueRcv) - _ttof(strValue)) > dChangeLimit) //Setting은 UI에서 관리함..
// 			{
// 				return FALSE;
// 			}
			else
			{
				strValue.Format(_T("%.6f"), _ttof(strValueRcv));
			}
		}
		else
		{
			strValue = strValueRcv;
		}
		
		return TRUE;
	}
	BOOL					SetValue(int nValueRcv)
	{
		if (dMin > nValueRcv || dMax < nValueRcv)
		{
			return FALSE;
		}
// 		else if (0 != dChangeLimit && fabs(nValueRcv - _ttoi(strValue)) > dChangeLimit) //Setting은 UI에서 관리함..
// 		{
// 			return FALSE;
// 		}
		strValue.Format(_T("%d"), nValueRcv);
		return TRUE;
	}
	BOOL					SetValue(double dValueRcv)
	{
		if (dMin > dValueRcv || dMax < dValueRcv)
		{
			return FALSE;
		}
// 		else if (0 != dChangeLimit && fabs(dValueRcv - _ttof(strValue)) > dChangeLimit)  //Setting은 UI에서 관리함..
// 		{
// 			return FALSE;
// 		}
		strValue.Format(_T("%.3f"), dValueRcv);
		return TRUE;
	}
	int						GetArrCount()					{ return nArrCnt;							}
	BOOL					GetShow()						{ return bShow;								}
	BOOL					GetUse()						{ return bUse;								}
	CString					GetScaledValue()
	{
		CString strScaledValue = strValue;

		if (dScale != 0 && 1 != dScale)
		{
			if (DEF_DATA_TYPE_INT == strDataType)
			{
				strScaledValue.Format(_T("%d"), (int)(_ttoi(strValue) * dScale));
			}
			else if (DEF_DATA_TYPE_DOUBLE == strDataType)
			{
				strScaledValue.Format(_T("%.3f"), _ttof(strValue) * dScale);
			}
		}

		return strScaledValue;
	}
	BOOL					SetScaledValue(CString strValueRcv)
	{
		CString strScaledValue;

		//스케일 변환부터 진행
		if (dScale != 0 && 1 != dScale)
		{
			if (DEF_DATA_TYPE_INT == strDataType)
			{
				strScaledValue.Format(_T("%d"), (int)(_ttoi(strValueRcv) / dScale));
			}
			else if (DEF_DATA_TYPE_DOUBLE == strDataType)
			{
				strScaledValue.Format(_T("%.6f"), _ttof(strValueRcv) / dScale);
			}
			else //BOOL, String은 스케일 없음
			{
				strScaledValue = strValueRcv;
			}
		}
		else
		{
			strScaledValue = strValueRcv;
		}

		//셋벨류를 호출하여 리밋검사 및 입력
		return SetValue(strScaledValue);
	}
	EN_LOGIN_LEVEL			GetUILevel() { return eUILevel; }





	int						nIndexGroup;
	CString					strGroup;
	CString					strName;
	CString					strDisplayName;
	CString					strCIMName;
	CString					strDataType;
	CString					strUnit;
	BOOL					bShow;
	BOOL					bUse;
	CString					strDefaultValue;
	CString					strValue;
	double					dScale;
	double					dChangeLimit;
	double					dMin;
	double					dMax;
	EN_LOGIN_LEVEL			eUILevel;
	CString					strDescript;
	//strName가 연속적으로 같을 경우, 배열처럼 활용할수 있도록한다.
	//[MARK X , Cnt = 4] [MARK X2, Cnt = 1] [MARK X3, Cnt = 1] [MARK X4, Cnt = 1] 과같이 변경하여 저장한다.
	int						nArrCnt;
	int						nIndexParam;
}ST_SETTING_PARAM;

struct ST_FIND_SET_PARAM
{
	ST_FIND_SET_PARAM(CString _strName)
	{
		strName = _strName;
		nIndex = -1;
	};

	ST_FIND_SET_PARAM(int _nIndex)
	{
		nIndex = _nIndex;
	};

	bool operator()(ST_SETTING_PARAM stParam)
	{
		if (stParam.nIndexParam == nIndex || 0 == stParam.strName.Compare(strName))
		{
			return true;
		}

		return false;
	};

	CString strName;
	int		nIndex;;
};

typedef std::vector <ST_SETTING_PARAM> vSettingParam;

typedef struct ST_SETTING_TAB
{
	ST_SETTING_TAB()
	{
		bUse = FALSE;
		strName = _T("INVALID DATA");;
	}
	//인덱스 참조 실패시 NULL 리턴	
	ST_SETTING_PARAM		GetParam(int nIndex)
	{
		ST_SETTING_PARAM stParam;
		if (nIndex < 0 || nIndex >= GetParamCount()) { MsgWrongIndex(nIndex); return stParam; }

		return vParameter.at(nIndex);
	}
	//인덱스 참조 실패시 NULL 리턴	
	ST_SETTING_PARAM		GetParam(CString strName)
	{
		ST_SETTING_PARAM stParam;
		ST_FIND_SET_PARAM stFind(strName);
		auto iparam = std::find_if(vParameter.begin(), vParameter.end(), stFind);
		if (iparam == vParameter.end()) { MsgWrongIndex(strName); return stParam; }

		return (*iparam);
	}
	//인덱스 참조 실패시 _T("INVALID DATA"); 리턴
	CString					GetParamName(int nIndex)
	{
		if (nIndex < 0 || nIndex >= GetParamCount()) { MsgWrongIndex(nIndex); return _T("INVALID DATA");; }
		return vParameter.at(nIndex).strName;
	}
	//
	int						GetParamCount()
	{
		return (int)vParameter.size();
	}
	//인덱스 참조 실패시 _T("INVALID DATA"); 리턴
	CString					GetValue(int nIndex)
	{
		if (nIndex < 0 || nIndex >= GetParamCount()) { MsgWrongIndex(nIndex); return _T("INVALID DATA");; }
		return vParameter.at(nIndex).strValue;
	}
	//인덱스 참조 실패시 _T("INVALID DATA"); 리턴
	CString					GetValue(CString strName)
	{
		ST_FIND_SET_PARAM stFind(strName);
		auto iparam = std::find_if(vParameter.begin(), vParameter.end(), stFind);
		if (iparam == vParameter.end()) { MsgWrongIndex(strName); return _T("INVALID DATA");; }

		return iparam->strValue;
	}
	//인덱스 참조 실패시 0 리턴
	int						GetInt(int nIndex)
	{
		if (nIndex < 0 || nIndex >= GetParamCount()) { MsgWrongIndex(nIndex); return DEF_INVALID_DATA; }
		return _ttoi(vParameter.at(nIndex).strValue);
	}
	//인덱스 참조 실패시 0 리턴
	int						GetInt(CString strName)
	{
		ST_FIND_SET_PARAM stFind(strName);
		auto iparam = std::find_if(vParameter.begin(), vParameter.end(), stFind);
		if (iparam == vParameter.end()) { MsgWrongIndex(strName); return DEF_INVALID_DATA; }

		return _ttoi(iparam->strValue);
	}
	//인덱스 참조 실패시 0 리턴
	double					GetDouble(int nIndex)
	{
		if (nIndex < 0 || nIndex >= GetParamCount()) { MsgWrongIndex(nIndex); return DEF_INVALID_DATA; }
		return _ttof(vParameter.at(nIndex).strValue);
	}
	//인덱스 참조 실패시 0 리턴
	double					GetDouble(CString strName)
	{
		ST_FIND_SET_PARAM stFind(strName);
		auto iparam = std::find_if(vParameter.begin(), vParameter.end(), stFind);
		if (iparam == vParameter.end()) { MsgWrongIndex(strName); return DEF_INVALID_DATA; }

		return _ttof(iparam->strValue);
	}
	//인덱스 참조 실패 : FALSE, 정상 : return TRUE
	BOOL					SetValue(int nIndex, CString strValue)
	{
		if (nIndex < 0 || nIndex >= GetParamCount()) { MsgWrongIndex(nIndex); return FALSE; }
		vParameter.at(nIndex).strValue = strValue;
		return TRUE;
	}
	//인덱스 참조 실패 : FALSE, 정상 : return TRUE
	BOOL					SetValue(int nIndex, int nValue)
	{
		if (nIndex < 0 || nIndex >= GetParamCount()) { MsgWrongIndex(nIndex); return FALSE; }
		vParameter.at(nIndex).strValue.Format(_T("%d"), nValue);
		return TRUE;
	}
	//인덱스 참조 실패 : FALSE, 정상 : return TRUE
	BOOL					SetValue(int nIndex, double dValue)
	{
		if (nIndex < 0 || nIndex >= GetParamCount()) { MsgWrongIndex(nIndex); return FALSE; }
		vParameter.at(nIndex).strValue.Format(_T("%.3f"), dValue);
		return TRUE;
	}
	//인덱스 참조 실패 : FALSE, 정상 : return TRUE
	BOOL					SetValue(CString strName, CString strValue)
	{
		ST_FIND_SET_PARAM stFind(strName);
		auto iparam = std::find_if(vParameter.begin(), vParameter.end(), stFind);
		if (iparam == vParameter.end()) { MsgWrongIndex(strName); return FALSE; }
		iparam->strValue = strValue;

		return TRUE;
	}
	//인덱스 참조 실패 : FALSE, 정상 : return TRUE
	BOOL					SetValue(CString strName, int nValue)
	{
		ST_FIND_SET_PARAM stFind(strName);
		auto iparam = std::find_if(vParameter.begin(), vParameter.end(), stFind);
		if (iparam == vParameter.end()) { MsgWrongIndex(strName); return FALSE; }
		iparam->strValue.Format(_T("%d"), nValue);
		return TRUE;
	}
	//인덱스 참조 실패 : FALSE, 정상 : return TRUE
	BOOL					SetValue(CString strName, double dValue)
	{
		ST_FIND_SET_PARAM stFind(strName);
		auto iparam = std::find_if(vParameter.begin(), vParameter.end(), stFind);
		if (iparam == vParameter.end()) { MsgWrongIndex(strName); return FALSE; }
		iparam->strValue.Format(_T("%.3f"), dValue);
		return TRUE;
	}
	//인덱스 참조 실패시 0 리턴
	int						GetArrCount(int nIndex)
	{
		if (nIndex < 0 || nIndex >= GetParamCount()) { MsgWrongIndex(nIndex); return 0; }
		return vParameter.at(nIndex).nArrCnt;
	}
	//인덱스 참조 실패시 0 리턴
	int						GetArrCount(CString strName)
	{
		ST_FIND_SET_PARAM stFind(strName);
		auto iparam = std::find_if(vParameter.begin(), vParameter.end(), stFind);
		if (iparam == vParameter.end()) { MsgWrongIndex(strName); return 0; }
		return iparam->nArrCnt;
	}

	void					MsgWrongIndex(int nIndex);
	void					MsgWrongIndex(CString strParam);

	BOOL					bUse;
	CString					strName;	//TabName
	int						nIndexTab;
	vSettingParam			vParameter;
}ST_SETTING_TAB;

typedef std::vector <ST_SETTING_TAB> vSettingTab;

struct ST_FIND_SETTING
{
	ST_FIND_SETTING(CString _strName)
	{
		strName = _strName;
		nIndex = -1;
	};

	ST_FIND_SETTING(int _nIndex)
	{
		nIndex = _nIndex;
	};

	bool operator()(ST_SETTING_PARAM stParam)
	{
		if (stParam.nIndexParam == nIndex || 0 == stParam.strName.Compare(strName))
		{
			return true;
		}

		return false;
	};

	bool operator()(ST_SETTING_TAB stTab)
	{
		if (stTab.nIndexTab == nIndex || 0 == stTab.strName.Compare(strName))
		{
			return true;
		}

		return false;
	};

	CString strName;
	int		nIndex;;
};

//////////////////////////////////////////////////////////////////////////
class CSetting
{
public:
	CSetting();
	virtual ~CSetting();
private:
	//Setting
	vSettingTab			m_vTab;
	
public:
	//////////////////////////////////////////////////////////////////////////
	//임시 메세지
	void					MsgWrongIndexTab(int nIndexTab);
	void					MsgWrongIndexTab(CString strTab);
	//
	//Setting
	void					MakeSettingForm();
	void					LoadSettingForm();
	void					DeleteSettingForm();
	int						GetTabCount();
	
	ST_SETTING_TAB			GetTab(int nIndexTab);
	ST_SETTING_TAB			GetTab(CString strTab);

	int						GetTabIndex(CString strTab);
	CString					GetTabName(int nIndexTab);

	BOOL					GetTabUse(CString strTab);
	BOOL					GetTabUse(int nIndexTab);

	int						GetParamCount(int nIndexTab);
	int						GetParamCount(CString strTab);
	ST_SETTING_PARAM		GetParam(int nIndexTab, int nIndexParam);
	ST_SETTING_PARAM		GetParam(int nIndexTab, CString strParam);
	ST_SETTING_PARAM		GetParam(CString strTab, int nIndexParam);
	ST_SETTING_PARAM		GetParam(CString strTab, CString strParam);
	void					SetParam(int nIndexTab, int nIndexParam, ST_SETTING_PARAM stParam);
	void					SetParam(int nIndexTab, CString strParam, ST_SETTING_PARAM stParam);
	void					SetParam(CString strTab, int nIndexParam, ST_SETTING_PARAM stParam);
	void					SetParam(CString strTab, CString strParam, ST_SETTING_PARAM stParam);

	int						GetParamIndex(int nIndexTab, CString strParam);
	int						GetParamIndex(CString strTab, CString strParam);
	CString					GetParamName(int nIndexTab, int nIndexParam);
	CString					GetParamName(CString strTab, int nIndexParam);

	CString					GetParamDataType(int nIndexTab, int nIndexParam);
	CString					GetParamDataType(int nIndexTab, CString strParam);
	CString					GetParamDataType(CString strTab, int nIndexParam);
	CString					GetParamDataType(CString strTab, CString strParam);

	int						GetCount(int nIndexTab, int nIndexParam);
	int						GetCount(int nIndexTab, CString strParam);
	int						GetCount(CString strTab, int nIndexParam);
	int						GetCount(CString strTab, CString strParam);

	CString					GetDefaultValue(int nIndexTab, int nIndexParam);
	CString					GetDefaultValue(int nIndexTab, CString strParam);
	CString					GetDefaultValue(CString strTab, int nIndexParam);
	CString					GetDefaultValue(CString strTab, CString strParam);

	int						GetDefaultInt(int nIndexTab, int nIndexParam);
	int						GetDefaultInt(int nIndexTab, CString strNstrParamame);
	int						GetDefaultInt(CString strTab, int nIndexParam);
	int						GetDefaultInt(CString strTab, CString strParam);

	double					GetDefaultDouble(int nIndexTab, int nIndexParam);
	double					GetDefaultDouble(int nIndexTab, CString strParam);
	double					GetDefaultDouble(CString strTab, int nIndexParam);
	double					GetDefaultDouble(CString strTab, CString strParam);

	CString					GetValue(int nIndexTab, int nIndexParam);
	CString					GetValue(int nIndexTab, CString strParam);
	CString					GetValue(CString strTab, int nIndexParam);
	CString					GetValue(CString strTab, CString strParam);
	CString					GetValue(CString strParam);

	int						GetInt(int nIndexTab, int nIndexParam);
	int						GetInt(int nIndexTab, CString strNstrParamame);
	int						GetInt(CString strTab, int nIndexParam);
	int						GetInt(CString strTab, CString strParam);

	double					GetDouble(int nIndexTab, int nIndexParam);
	double					GetDouble(int nIndexTab, CString strParam);
	double					GetDouble(CString strTab, int nIndexParam);
	double					GetDouble(CString strTab, CString strParam);

	BOOL					SetValue(int nIndexTab, int nIndexParam, CString strValue);
	BOOL					SetValue(int nIndexTab, CString strParam, CString strValue);
	BOOL					SetValue(CString strTab, int nIndexParam, CString strValue);
	BOOL					SetValue(CString strTab, CString strParam, CString strValue);

	BOOL					SetValue(int nIndexTab, int nIndexParam, int nValue);
	BOOL					SetValue(int nIndexTab, CString strParam, int nValue);
	BOOL					SetValue(CString strTab, int nIndexParam, int nValue);
	BOOL					SetValue(CString strTab, CString strParam, int nValue);

	BOOL					SetIncrease(int nIndexTab, int nIndexParam);
	BOOL					SetIncrease(int nIndexTab, CString strParam);
	BOOL					SetIncrease(CString strTab, int nIndexParam);
	BOOL					SetIncrease(CString strTab, CString strParam);

	BOOL					SetDecrease(int nIndexTab, int nIndexParam);
	BOOL					SetDecrease(int nIndexTab, CString strParam);
	BOOL					SetDecrease(CString strTab, int nIndexParam);
	BOOL					SetDecrease(CString strTab, CString strParam);

	BOOL					SetValue(int nIndexTab, int nIndexParam, double dValue);
	BOOL					SetValue(int nIndexTab, CString strParam, double dValue);
	BOOL					SetValue(CString strTab, int nIndexParam, double dValue);
	BOOL					SetValue(CString strTab, CString strParam, double dValue);

	BOOL					GetShow(int nIndexTab, int nIndexParam);
	BOOL					GetShow(int nIndexTab, CString strNstrParamame);
	BOOL					GetShow(CString strTab, int nIndexParam);
	BOOL					GetShow(CString strTab, CString strParam);

	BOOL					GetUse(int nIndexTab, int nIndexParam);
	BOOL					GetUse(int nIndexTab, CString strNstrParamame);
	BOOL					GetUse(CString strTab, int nIndexParam);
	BOOL					GetUse(CString strTab, CString strParam);

	CString					GetCIMValue(CString strTab, CString strCIMName);
	BOOL					SetCIMValue(CString strTab, CString strCIMName, CString strValue);

	BOOL					GetGroupList(int nTab, int nGroup, vSettingParam& stArrGroup);
	BOOL					GetGroupList(CString strTab, CString strGroup, vSettingParam &stArrGroup);

	BOOL					IsInt(CString strText);
	BOOL					IsDouble(CString strText);
};
