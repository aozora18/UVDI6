#pragma once
#include "enum.h"

#define DEF_INVALID_DATA					-32767
#define DEF_DATA_TYPE_BOOL					_T("BOOL")
#define DEF_DATA_TYPE_INT					_T("INT")
#define DEF_DATA_TYPE_DOUBLE				_T("DOUBLE")
#define DEF_DATA_TYPE_STRING				_T("STRING")
#define DEF_DATA_TYPE_LOAD					_T("LOAD")
#define DEF_DATA_TYPE_COMBO					_T("COMBO")

JHMI_ENUM(EN_RECIPE_TAB, int
	, JOB
	, EXPOSE
	, ALIGN
)

JHMI_ENUM(EN_RECIPE_JOB, int
	, JOB_NAME
	, GERBER_PATH
	, GERBER_NAME
	, ALIGN_RECIPE
	, EXPO_RECIPE
	, MATERIAL_THICK
	, EXPO_ENERGY
)

JHMI_ENUM(EN_RECIPE_EXPOSE, int
	, EXPO_NAME
	, POWER_NAME
	, MARK_ERR_DIST_RATE
	, MARK_ERR_DIST_TOP_HORZ
	, MARK_ERR_DIST_BTM_HORZ
	, MARK_ERR_DIST_TOP_VERT
	, MARK_ERR_DIST_BTM_VERT
	, MARK_ERR_DIST_LFT_DIAG
	, MARK_ERR_DIST_RGT_DIAG
	, LED_DUTY_CYCLE
	, SCORE_ACCEPT
	, SCALE_RANGE
	, REAL_SCALE_RANGE
	, REAL_ROTAION_RANGE
	, SERIAL_DECODE
	, SCALE_DECODE
	, TEXT_DECODE
	, TEXT_STRING
	, SERIAL_FLIP_HORZ
	, SERIAL_FLIP_VERT
	, SCALE_FLIP_HORZ
	, SCALE_FLIP_VERT
	, TEXT_FLIP_HORZ
	, TEXT_FLIP_VERT
)

JHMI_ENUM(EN_RECIPE_ALIGN, int
	, ALIGN_NAME
	, MARK_TYPE
	, ALIGN_TYPE
	, ALIGN_MOTION
	, LAMP_TYPE
	, GAIN_LEVEL_CAM1
	, GAIN_LEVEL_CAM2
	, SEARCH_TYPE
	, SEARCH_COUNT
	, MARK_AREA_WIDTH
	, MARK_AREA_HEIGHT
	, ALIGN_CAMERA_NUMBER1
	, ALIGN_CAMERA_NUMBER2
	, GLOBAL_MARK_NAME
	, LOCAL_MARK_NAME
)

//////////////////////////////////////////////////////////////////////////
//1. 레시피 - 탭 - 파라미터 구조
typedef struct ST_RECIPE_PARAM
{
	ST_RECIPE_PARAM() 
	{
		bShow = FALSE;
		bUse = FALSE;
		strValue.Format(_T("%d"), DEF_INVALID_DATA);
		dScale = 0;
		dChangeLimit = 0;
		dMin = 0;
		dMax = 0;
		strDisplayName = _T("NO NAME");
		eUILevel = eLOGIN_LEVEL_UNKNOWN;
		strDescript = _T("등록되지 않은 파라메터입니다.");
		nArrCnt = 0;
		nIndexParam = -1;
	}

	ST_RECIPE_PARAM(ST_RECIPE_PARAM* pStParam)
	{		
		strGroup		= pStParam->strGroup;
		strName			= pStParam->strName;
		strDisplayName	= pStParam->strDisplayName;
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

	void Set(ST_RECIPE_PARAM stParam)
	{
		strGroup		= stParam.strGroup;
		strName			= stParam.strName;
		strDisplayName	= stParam.strDisplayName;
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
	};

	CString					GetDefaultValue()				{ return strDefaultValue;					}
	int						GetDefaultInt()					{ return _ttoi(strDefaultValue);			}
	double					GetDefaultDouble()				{ return _ttof(strDefaultValue);			}
	CString					GetValue()						{ return strValue;							}
	int						GetInt()						{ return _ttoi(strValue);					}
	double					GetDouble()						{ return _ttof(strValue);					}
	BOOL					GetDefaultValueList(CStringArray &strArrValue) 
	{ 
		CString strSplitValue;
		while (AfxExtractSubString(strValue, strDefaultValue, (int)strArrValue.GetCount(), ';'))
		{
			if (FALSE == strSplitValue.IsEmpty())
			{
				strArrValue.Add(strSplitValue);
			}
		}
		return TRUE; 
	}

	BOOL					SetValue(CString strValueRcv)
	{
		if (DEF_DATA_TYPE_BOOL == strDataType || DEF_DATA_TYPE_INT == strDataType)
		{
			if (dMin > _ttoi(strValueRcv) || dMax < _ttoi(strValueRcv))
			{
				return FALSE;
			}
			else if ( 0 != dChangeLimit && fabs(_ttoi(strValueRcv) - _ttoi(strValue)) > dChangeLimit)
			{
				return FALSE;
			}
			else
			{
				strValue.Format(_T("%d"), _ttoi(strValueRcv));
			}				
		}
		else if (DEF_DATA_TYPE_DOUBLE == strDataType) //Num
		{
			if (dMin > _ttof(strValueRcv) || dMax < _ttof(strValueRcv))
			{
				return FALSE;
			}
			else if (0 != dChangeLimit && fabs(_ttof(strValueRcv) - _ttof(strValue)) > dChangeLimit)
			{
				return FALSE;
			}
			else
			{
				strValue.Format(_T("%.4f"), _ttof(strValueRcv));
			}				
		}
		else
		{
			strValue = strValueRcv;
		}
		
		return TRUE;
	}
	BOOL					SetValue(CStringA strValueRcv)
	{
		if (DEF_DATA_TYPE_BOOL == strDataType || DEF_DATA_TYPE_INT == strDataType)
		{
			if (dMin > atoi(strValueRcv) || dMax < atoi(strValueRcv))
			{
				return FALSE;
			}
			else if (0 != dChangeLimit && fabs(atoi(strValueRcv) - _ttoi(strValue)) > dChangeLimit)
			{
				return FALSE;
			}
			else
			{
				strValue.Format(_T("%d"), atoi(strValueRcv));
			}
		}
		else if (DEF_DATA_TYPE_DOUBLE == strDataType) //Num
		{
			if (dMin > atof(strValueRcv) || dMax < atof(strValueRcv))
			{
				return FALSE;
			}
			else if (0 != dChangeLimit && fabs(atof(strValueRcv) - _ttof(strValue)) > dChangeLimit)
			{
				return FALSE;
			}
			else
			{
				strValue.Format(_T("%.3f"), atof(strValueRcv));
			}
		}
		else
		{
			strValue = (CString)strValueRcv;
		}

		return TRUE;
	}
	BOOL					SetValue(short nValueRcv)
	{
		if (dMin > nValueRcv || dMax < nValueRcv)
		{
			return FALSE;
		}
		else if (0 != dChangeLimit && fabs(nValueRcv - _ttoi(strValue)) > dChangeLimit)
		{
			return FALSE;
		}
		strValue.Format(_T("%d"), nValueRcv);
		return TRUE;
	}
	BOOL					SetValue(unsigned short nValueRcv)
	{
		if (dMin > nValueRcv || dMax < nValueRcv)
		{
			return FALSE;
		}
		else if (0 != dChangeLimit && fabs(nValueRcv - _ttoi(strValue)) > dChangeLimit)
		{
			return FALSE;
		}
		strValue.Format(_T("%d"), nValueRcv);
		return TRUE;
	}
	BOOL					SetValue(int nValueRcv)
	{
		if (dMin > nValueRcv || dMax < nValueRcv)
		{
			return FALSE;
		}			
		else if (0 != dChangeLimit && fabs(nValueRcv - _ttoi(strValue)) > dChangeLimit)
		{
			return FALSE;
		}
		strValue.Format(_T("%d"), nValueRcv);
		return TRUE;
	}
	BOOL					SetValue(unsigned int nValueRcv)
	{
		if (dMin > nValueRcv || dMax < nValueRcv)
		{
			return FALSE;
		}
		else if (0 != dChangeLimit && fabs(nValueRcv - _ttoi(strValue)) > dChangeLimit)
		{
			return FALSE;
		}
		strValue.Format(_T("%d"), nValueRcv);
		return TRUE;
	}
	BOOL					SetValue(float dValueRcv)
	{
		if (dMin > dValueRcv || dMax < dValueRcv)
		{
			return FALSE;
		}
		else if (0 != dChangeLimit && fabs(dValueRcv - _ttof(strValue)) > dChangeLimit)
		{
			return FALSE;
		}
		strValue.Format(_T("%.3f"), dValueRcv);
		return TRUE;
	}
	BOOL					SetValue(double dValueRcv)
	{
		if (dMin > dValueRcv || dMax < dValueRcv)
		{
			return FALSE;
		}
		else if (0 != dChangeLimit && fabs(dValueRcv - _ttof(strValue)) > dChangeLimit)
		{
			return FALSE;
		}
		strValue.Format(_T("%.4f"), dValueRcv);
		return TRUE;
	}
	int						GetArrCount()					{ return nArrCnt;							}
	BOOL					GetShow()						{ return bShow;								}
	BOOL					GetUse()						{ return bUse;								}
	void					SetUse(BOOL bUseRcv)			{ bUse = bUseRcv;							}
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
				strScaledValue.Format(_T("%.3f"), _ttof(strValueRcv) / dScale);
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
	EN_LOGIN_LEVEL			GetUILevel()					{ return eUILevel;							}





	CString					strGroup;
	CString					strName;
	CString					strDisplayName;
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
}ST_RECIPE_PARAM;

typedef std::vector <ST_RECIPE_PARAM> vRECIPE_PARAM;

struct ST_FIND_RCP_PARAM
{
	ST_FIND_RCP_PARAM(CString _strName)
	{
		strName = _strName;
		nIndex = -1;
	};

	ST_FIND_RCP_PARAM(int _nIndex)
	{
		nIndex = _nIndex;
	};

	bool operator()(const ST_RECIPE_PARAM stParam)
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

typedef struct ST_RECIPE_TAB
{
	ST_RECIPE_TAB()
	{
		bUse = FALSE;
		strName = _T("");
		nIndexTab = -1;
	}

	//인덱스 참조 실패시 NULL 리턴
	ST_RECIPE_PARAM		GetParam(int nIndex)
	{
		ST_RECIPE_PARAM stParam;
		if (nIndex < 0 || nIndex >= GetParamCount()) { MsgWrongIndex(nIndex); return stParam; }

		return vParameter.at(nIndex);
	}
	//인덱스 참조 실패시 NULL 리턴
	ST_RECIPE_PARAM		GetParam(CString strParam)
	{
		ST_RECIPE_PARAM stParam;
		ST_FIND_RCP_PARAM stFindParam(strParam);
		auto iparam = std::find_if(vParameter.begin(), vParameter.end(), stFindParam);
		if (iparam == vParameter.end()) { MsgWrongIndex(strParam); return stParam; }

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

	int						GetParamIndex(CString strParam)
	{
		ST_FIND_RCP_PARAM stFindParam(strParam);
		auto iparam = std::find_if(vParameter.begin(), vParameter.end(), stFindParam);
		if (vParameter.end() == iparam) { MsgWrongIndex(strParam); return DEF_INVALID_DATA; }
		return iparam->nIndexParam;
	}

	//인덱스 참조 실패시 _T("INVALID DATA"); 리턴
	CString					GetValue(int nIndex)
	{
		if (nIndex < 0 || nIndex >= GetParamCount()) { MsgWrongIndex(nIndex); return _T("INVALID DATA");; }
		return vParameter.at(nIndex).strValue;
	}
	//인덱스 참조 실패시 _T("INVALID DATA"); 리턴
	CString					GetValue(CString strParam)
	{
		ST_FIND_RCP_PARAM stFindParam(strParam);
		auto iparam = std::find_if(vParameter.begin(), vParameter.end(), stFindParam);
		if (vParameter.end() == iparam) { MsgWrongIndex(strParam); return _T("INVALID DATA");; }
		return iparam->strValue;
	}
	//인덱스 참조 실패시 0 리턴
	int						GetInt(int nIndex)
	{
		if (nIndex < 0 || nIndex >= GetParamCount()) { MsgWrongIndex(nIndex); return DEF_INVALID_DATA; }
		return _ttoi(vParameter.at(nIndex).strValue);
	}
	//인덱스 참조 실패시 0 리턴
	int						GetInt(CString strParam)
	{
		ST_FIND_RCP_PARAM stFindParam(strParam);
		auto iparam = std::find_if(vParameter.begin(), vParameter.end(), stFindParam);
		if (vParameter.end() == iparam) { MsgWrongIndex(strParam); return DEF_INVALID_DATA; }
		return _ttoi(iparam->strValue);
	}
	//인덱스 참조 실패시 0 리턴
	double					GetDouble(int nIndex)
	{
		if (nIndex < 0 || nIndex >= GetParamCount()) { MsgWrongIndex(nIndex); return DEF_INVALID_DATA; }
		return _ttof(vParameter.at(nIndex).strValue);
	}
	//인덱스 참조 실패시 0 리턴
	double					GetDouble(CString strParam)
	{
		ST_FIND_RCP_PARAM stFindParam(strParam);
		auto iparam = std::find_if(vParameter.begin(), vParameter.end(), stFindParam);
		if (vParameter.end() == iparam) { MsgWrongIndex(strParam); return DEF_INVALID_DATA; }
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
	BOOL					SetValue(CString strParam, CString strValue)
	{
		ST_FIND_RCP_PARAM stFindParam(strParam);
		auto iparam = std::find_if(vParameter.begin(), vParameter.end(), stFindParam);
		if (vParameter.end() == iparam) { MsgWrongIndex(strParam); return FALSE; }
		iparam->strValue = strValue;
		return TRUE;
	}
	//인덱스 참조 실패 : FALSE, 정상 : return TRUE
	BOOL					SetValue(CString strParam, int nValue)
	{
		ST_FIND_RCP_PARAM stFindParam(strParam);
		auto iparam = std::find_if(vParameter.begin(), vParameter.end(), stFindParam);
		iparam->strValue.Format(_T("%d"), nValue);
		return TRUE;
	}
	//인덱스 참조 실패 : FALSE, 정상 : return TRUE
	BOOL					SetValue(CString strParam, double dValue)
	{
		ST_FIND_RCP_PARAM stFindParam(strParam);
		auto iparam = std::find_if(vParameter.begin(), vParameter.end(), stFindParam);
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
	int						GetArrCount(CString strParam)
	{
		ST_FIND_RCP_PARAM stFindParam(strParam);
		auto iparam = std::find_if(vParameter.begin(), vParameter.end(), stFindParam);
		return iparam->nArrCnt;
	}

	void					MsgWrongIndex(int nIndex);
	void					MsgWrongIndex(CString strParam);

	BOOL					bUse;
	CString					strName;	//TabName
	int						nIndexTab;
	vRECIPE_PARAM			vParameter;
}ST_RECIPE_TAB;

struct ST_FIND_RECIPE
{
	ST_FIND_RECIPE(CString _strName)
	{
		strName = _strName;
		nIndex = -1;
	};

	ST_FIND_RECIPE(int _nIndex)
	{
		nIndex = _nIndex;
	};

	bool operator()(const ST_RECIPE_TAB stParam)
	{
		if (stParam.nIndexTab == nIndex || 0 == stParam.strName.Compare(strName))
		{
			return true;
		}

		return false;
	};

	bool operator()(const ST_RECIPE_PARAM stParam)
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

typedef std::vector <ST_RECIPE_TAB> vRecipeTab;
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
class CRecipe
{
public:
	CRecipe();
	virtual ~CRecipe();
private:
	//Recipe
	vRecipeTab			m_vTab;
	
public:
	//////////////////////////////////////////////////////////////////////////
	//임시 메세지
	BOOL					IsInt(CString strText);
	BOOL					IsDouble(CString strText);
	void					MsgWrongIndexTab(int nIndexTab);
	void					MsgWrongIndexTab(CString strTab);
	void					MsgWrongIndexScan(int nIndexScan);
	void					MsgWrongIndexScan(CString strScan);

	//
	//Recipe
	void					MakeRecipeForm();
	void					LoadRecipeForm();
	void					DeleteRecipeForm();
	int						GetTabCount();
	
	ST_RECIPE_TAB			GetTab(int nIndexTab);
	ST_RECIPE_TAB			GetTab(CString strTab);

	int						GetTabIndex(CString strTab);
	CString					GetTabName(int nIndexTab);

	BOOL					GetTabUse(CString strTab);
	BOOL					GetTabUse(int nIndexTab);

	int						GetParamArrCount(int nIndexTab, int nIndexParam);
	int						GetParamArrCount(int nIndexTab, CString strParam);
	int						GetParamArrCount(CString strTab, int nIndexParam);
	int						GetParamArrCount(CString strTab, CString strParam);

	int						GetParamCount(int nIndexTab);
	int						GetParamCount(CString strTab);

	ST_RECIPE_PARAM			GetParam(int nIndexTab, int nIndexParam);
	ST_RECIPE_PARAM			GetParam(int nIndexTab, CString strParam);
	ST_RECIPE_PARAM			GetParam(CString strTab, int nIndexParam);
	ST_RECIPE_PARAM			GetParam(CString strTab, CString strParam);

	void					SetParam(int nIndexTab, int nIndexParam, ST_RECIPE_PARAM stParam);
	void					SetParam(int nIndexTab, ST_RECIPE_PARAM stParam);
	void					SetParam(int nIndexTab, CString strParam, ST_RECIPE_PARAM stParam);
	void					SetParam(CString strTab, int nIndexParam, ST_RECIPE_PARAM stParam);
	void					SetParam(CString strTab, CString strParam, ST_RECIPE_PARAM stParam);

	int						GetParamIndex(int nIndexTab, CString strParam);
	int						GetParamIndex(CString strTab, CString strParam);
	BOOL					GetParamIndex(CString strTab, CString strParam, int &nIndex);	// 20210623 by jyoon : 새로 만듬.
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

	BOOL					SetUse(int nIndexTab, int nIndexParam, BOOL bUse);
	BOOL					SetUse(int nIndexTab, CString strNstrParamame, BOOL bUse);
	BOOL					SetUse(CString strTab, int nIndexParam, BOOL bUse);
	BOOL					SetUse(CString strTab, CString strParam, BOOL bUse);

	CString					GetCIMValue(CString strTab, CString strCIMName);
	BOOL					SetCIMValue(CString strTab, CString strCIMName, CString strValue);
};
