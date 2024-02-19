
/*
 desc : PM100D Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "CaliThread.h"

#include <numeric>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/* --------------------------------------------------------------------------------------------- */
/*                                    Array Queue <eXtended>                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Constructor
 parm : count	- [in]  버퍼 개수 (Max : 32)
 retn : None
*/
CArrQueEx::CArrQueEx()
{
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CArrQueEx::~CArrQueEx()
{
	m_vecValue.clear();
}

/*
 desc : Put the current value and then calculates the average and standard deviation
 parm : value	- [in]  Current value
 retn : None
*/
VOID CArrQueEx::PushQue(DOUBLE value)
{
	INT8 i8Sign	= 0;
	DOUBLE dbSum= 0.0f, dbMax = DBL_MIN, dbMin = DBL_MAX;
	vector<DOUBLE>::iterator itVec;

#if 1
	if (value == 0.0f)
	{
		TRACE(L"PushQue(value) : Power is Zero\n");
	}
#endif

	/* Put the current value (equal to insert()) */
	m_vecValue.emplace_back(value);

	/* Check if the queue is full */
	if (m_vecValue.size() > ILLUM_MEAS_COUNT)
	{
		m_vecValue.erase(m_vecValue.begin());
	}
	if (m_vecValue.size() > 1)
	{
#if 0	/* The sort function should not be used here */
		/* Sort in ascending order */
		sort(m_vecValue.begin(), m_vecValue.end());
#endif
		/* Calculate the sum */
		dbSum	= accumulate(m_vecValue.begin(), m_vecValue.end(), double(0.0f));
		/* Calculate the average */
		m_dbAvg	= dbSum/m_vecValue.size();
		/* If there is only one array... */
		if (m_vecValue.size() < 2)	m_dbStd	= sqrt(-1.0f);
		else
		{
			/* Calculate the standard deviation */
			itVec	= m_vecValue.begin();
			for (dbSum=0.0f; itVec != m_vecValue.end(); itVec++)
			{
				/* Set the min and max values */
				if (dbMax < *itVec)	dbMax = *itVec;
				if (dbMin > *itVec)	dbMin = *itVec;
				/* Calculate the sum (current - average) */
				dbSum	+= pow((*itVec - m_dbAvg), 2);
			}
			/* Calculates the standard deviation */
			m_dbStd	= sqrt(dbSum / DOUBLE(m_vecValue.size()));
		}
		/* Calculates the max - min (offset) value */
		m_dbDiff= dbMax - dbMin;
	}
	else
	{
		m_dbAvg	= value;
		m_dbStd	= value;
		m_dbDiff= value;
	}
}

/* --------------------------------------------------------------------------------------------- */
/*                                       Calibration Thread                                      */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : shmem	- [int]  Shared Memory
		type	- [in]  Measurement Type (0x01: Power, 0x02: Energy, 0x03: Frequency, 0x04: Density)
 retn : None
 참고 :	dev_set	- [out] PM100D Device Setup Information 정보가 저장될 구조체 포인터
		dev_get	- [out] PM100D Device Realtime Value 정보가 저장될 구조체 포인터
*/
CCaliThread::CCaliThread(LPG_PADV shmem, ENG_MDST type)
{
	m_hDevPM100D	= NULL;
	m_bIsOpened		= FALSE;
	m_bRunMeasure	= FALSE;

	/* Device 전체 정보 연결 */
	m_pstShMemPM100D= shmem;
	m_enMeasType	= type;

	/* 임의 시간마다 연결을 하기 위해서 */
	m_u64ConnectTime= 0;

	/* 임의 개수만큼 평균을 내기 위한 큐 버퍼 생성 (초당 10 개 정도 적당함) */
	/* PM100D로부 명령 요청에 대한 응답시간이 아무리 빨라야 30 ~ 80 msec 임 */
	m_pArrQue		= new CArrQueEx();
	ASSERT(m_pArrQue);
}

/*
 desc : Destructor
 parm : None
*/
CCaliThread::~CCaliThread()
{
	/* 현재 연결된 PM100D 디바이스 닫기 */
	if (m_bIsOpened)	CloseDev();
	/* 큐 버퍼 메모리 해제 */
	if (m_pArrQue)	delete m_pArrQue;
}

/*
 desc : 스레드 실행시 초기 한번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCaliThread::StartWork()
{

	return TRUE;
}

/*
 desc : 스레드 실행시 주기적 호출됨
 parm : None
 retn : None
*/
VOID CCaliThread::RunWork()
{
	/* 현재 PM100D와 연결되어 있는지 확인 */
	if (!m_bIsOpened)
	{
		BOOL bSucc	= FALSE;

		/* 기존 연결 수행 후 일정 시간이 경과 했는지 여부 */
		if (m_u64ConnectTime + 3000 > GetTickCount64())	return;
		/* 연결 시도한 시간 저장 */
		m_u64ConnectTime	= GetTickCount64();

		/* 일단 기존 연결된 디바이스 닫기 */
		CloseDev();
		/* 새로 연결 수행 */
		bSucc = OpenDev();
		if (bSucc)
		{
			/* 현재 장비에 설정된 정보 요청 */
			bSucc	= GetDevSetData();
		}
		/* 연결 여부 최종 설정 */
		m_bIsOpened	= bSucc;
		return;
	}
	/* 동기 진입 */
	if (m_syncData.Enter())
	{
		/* 주기적으로 값 요청 및 저장 */
		if (m_bRunMeasure)
		{
			m_bIsOpened	= QueryTotal();
		}

		/* 동기 해제*/
		m_syncData.Leave();
	}
}

/*
 desc : 스레드 종료시 마지막 호출됨
 parm : None
 retn : None
*/
VOID CCaliThread::EndWork()
{
}

/*
 desc : 현재 장비에 설정된 정보 요청
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCaliThread::GetDevSetData()
{
	BOOL bSucc	= TRUE;

	/* 현재 연결된 디바이스 정보 얻기 */
	if (bSucc)	bSucc	= QueryDevID();
	/* 현재 Beam Diameter 정보 얻기*/
	if (bSucc)	bSucc	= QueryBeamDiameter();
	/* 현재 Line Frequencey 정보 얻기 */
	if (bSucc)	bSucc	= QueryLineFrequency();
	/* 현재 장착된 센서 정보 얻기 */
	if (bSucc)	bSucc	= QuerySensorInfo();

	return bSucc;
}

/*
 desc : PM100D Device에 연결 시도
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCaliThread::OpenDev()
{
	ViStatus viError	= -1;
	ViChar szResource[PM100D_BUFFER_SIZE]	= {NULL};

	/* Find resources */
	viError	= FindInstruments(PMxxx_FIND_PATTERN, szResource);
	if (viError)	return FALSE;
	/* Open session to PM100x/PM160/PM200 instrument */
	viError	= PM100D_init(szResource, VI_OFF, VI_ON, &m_hDevPM100D);
	if (viError)
	{
		ErrosMessage(viError);
		return FALSE;
	}

	/* PM100D 시간 갱신  */
	UpdatePM100DTime();

	return TRUE;
}

/*
 desc : PM100D Device에 닫기
 parm : None
 retn : None
*/
VOID CCaliThread::CloseDev()
{
	if (m_hDevPM100D)
	{
		PM100D_close(m_hDevPM100D);
		m_hDevPM100D	= 0;
	}
}

/*
 desc : Find Resources
 parm : pattern		- [in]  검색 패턴 문자열
		res_name	- [in]  검색된 Resource가 저장될 버퍼 포인터
 retn : 에러 값
*/
ViStatus CCaliThread::FindInstruments(ViString pattern, ViChar *res_name)
{
   ViStatus viError		= 0;
   ViUInt32 viDevCount	= 0;
                      
	/* Find Resource*/
#if 1
	viError = PM100D_findRsrc(0, &viDevCount);
#else
	viError = PM100D_findRsrc(pattern, &viDevCount);
#endif
	switch(viError)
	{
	/* At least one device was found. Nothing to do here. Continue with device selection menu */
	case VI_SUCCESS				:	break;
	case VI_ERROR_RSRC_NFOUND	:
		AfxMessageBox(L"No matching instruments found", MB_ICONSTOP|MB_TOPMOST);
		return viError;
	default						:	return viError;
	}
   
	/* 검색된 개수가 1개 이하인 경우 */
   if(viDevCount < 1)
   {
#ifndef _DEBUG
	   AfxMessageBox(L"Failed to find the devices", MB_ICONSTOP|MB_TOPMOST);
#endif
	   return VI_ERROR_RSRC_NFOUND;
   }
    /* Found only one matching instrument - return this */
	return PM100D_getRsrcName(0, 0, res_name);
}

/*
 desc : 에러 정보 저정
 parm : err_cd	- [in]  PM100D에서 발생된 에러 코드 값
 retn : TRUE or FALSE
*/
BOOL CCaliThread::ErrosMessage(ViStatus err_cd)
{
	CHAR szMesg[512]	= {NULL};
	ViChar vzMesg[PM100D_ERR_DESCR_BUFFER_SIZE]	= {NULL};
	CUniToChar csCnv;

	/* 에러가 없는 경우인지 확인 */
	if (0 == err_cd)	return TRUE;

	// Print error
	PM100D_errorMessage(m_hDevPM100D, err_cd, vzMesg);
	/* Event Message */
	sprintf_s(szMesg, 512, "err_msg = %s <err_cd=%d>", vzMesg, err_cd);
	LOG_ERROR(ENG_EDIC::en_pm100d, csCnv.Ansi2Uni(szMesg));

	/* 에러가 발생 했다고 알림 */
	return FALSE;
}

/*
 desc : 현재 연결된 디바이스 ID 정보 얻기
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCaliThread::QueryDevID()
{
	ViStatus viError		= VI_SUCCESS;
	ViChar szQuery[6][64]	= {NULL};
	LPG_PDDI pstDevID		= &m_pstShMemPM100D->set.dev_id;
	CUniToChar csCnv;

	/* Identification Query */
	if (VI_SUCCESS == viError)	viError	= PM100D_identificationQuery(m_hDevPM100D,
																	 szQuery[0],
																	 szQuery[1],
																	 szQuery[2],
																	 szQuery[3]);
	/* Revision Query */
	if (VI_SUCCESS == viError)	viError	= PM100D_revisionQuery(m_hDevPM100D, szQuery[4], VI_NULL);
	/* Get Calibration Message */
	if (VI_SUCCESS == viError)	viError	= PM100D_getCalibrationMsg(m_hDevPM100D, szQuery[5]);
	if (!ErrosMessage(viError))	return FALSE;

	/* 값 저장 */
	wcscpy_s(pstDevID->m_name,	63, csCnv.Ansi2Uni(szQuery[0]));
	wcscpy_s(pstDevID->d_name,	63, csCnv.Ansi2Uni(szQuery[1]));
	wcscpy_s(pstDevID->s_num,	63, csCnv.Ansi2Uni(szQuery[2]));
	wcscpy_s(pstDevID->s_firm,	63, csCnv.Ansi2Uni(szQuery[3]));
	wcscpy_s(pstDevID->i_firm,	63, csCnv.Ansi2Uni(szQuery[4]));
	wcscpy_s(pstDevID->c_date,	63, csCnv.Ansi2Uni(szQuery[5]));

	return TRUE;
}

/*
 desc : 현재 연결된 디바이스 ID 정보 얻기
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCaliThread::QuerySensorInfo()
{
	ViStatus viError		= VI_SUCCESS;
	ViChar szQuery[3][256]	= {NULL};
	ViInt16 viSensType		= 0, viSensSubtype = 0, viFlags = 0;
	LPG_PSDI pstSensor		= &m_pstShMemPM100D->set.sensor;
	CUniToChar csCnv;

	if (VI_SUCCESS == viError)	viError	= PM100D_getSensorInfo(m_hDevPM100D,
															   szQuery[0],
															   szQuery[1],
															   szQuery[2],
															   &viSensType,
															   &viSensSubtype,
															   &viFlags);
	if (!ErrosMessage(viError))	return FALSE;

	/* 값 저장 */
	wcscpy_s(pstSensor->s_name,	63, csCnv.Ansi2Uni(szQuery[0]));
	wcscpy_s(pstSensor->s_num,	63, csCnv.Ansi2Uni(szQuery[1]));
	/* Calibration Message */
	/*wcscpy_s(pstSensor->s_,	63, csCnv.Ansi2Uni(szQuery[2]));*/
	pstSensor->s_type	= viSensType;
	pstSensor->s_subt	= viSensSubtype;
	pstSensor->flags	= viFlags;

	return TRUE;
}

/*
 desc : Beam Diameter 얻기
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCaliThread::QueryBeamDiameter()
{
	ViStatus viError	= VI_SUCCESS;
	ViReal64 viValue	= 0.0f;
	LPG_PBDV pstBeam	= &m_pstShMemPM100D->set.beam;

	/* Set Value */
	if (VI_SUCCESS == viError)
	{
		viError	= PM100D_getBeamDia(m_hDevPM100D, PM100D_ATTR_SET_VAL, &viValue);
		pstBeam->set_val	= viValue;
	}
	/* Min Value */
	if (VI_SUCCESS == viError)
	{
		viError	= PM100D_getBeamDia(m_hDevPM100D, PM100D_ATTR_MIN_VAL, &viValue);
		pstBeam->min_val	= viValue;
	}
	/* Max Value */
	if (VI_SUCCESS == viError)
	{
		viError	= PM100D_getBeamDia(m_hDevPM100D, PM100D_ATTR_MAX_VAL, &viValue);
		pstBeam->max_val	= viValue;
	}
	/* Default Value */
	if (VI_SUCCESS == viError)
	{
		viError	= PM100D_getBeamDia(m_hDevPM100D, PM100D_ATTR_DFLT_VAL, &viValue);
		pstBeam->def_val	= viValue;
	}

	return ErrosMessage(viError);
}

/*
 desc : Beam Diameter 설정
 parm : value	- [in]  설정 값 (단위: mm)
 retn : TRUE or FALSE
*/
BOOL CCaliThread::QueryBeamDiameter(DOUBLE value)
{
	return ErrosMessage(PM100D_setBeamDia(m_hDevPM100D, value));
}

/*
 desc : Get Line Frequency (단위: Hz)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCaliThread::QueryLineFrequency()
{
	ViInt16 vi16Freq	= 0;

	if (!ErrosMessage(PM100D_getLineFrequency(m_hDevPM100D, &vi16Freq)))	return FALSE;
	/* 값 설정 */
	m_pstShMemPM100D->set.line_freq	= vi16Freq;

	return TRUE;
}

/*
 desc : Set Line Frequency
 parm : freq	- [in]  Line Frequency (or Line Filter) (단위: Hz)
 retn : TRUE or FALSE
*/
BOOL CCaliThread::QueryLineFrequency(INT16 value)
{
	return ErrosMessage(PM100D_setLineFrequency(m_hDevPM100D, value));
}

/*
 desc : 디바이스에 현재 시간 설정
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCaliThread::QuerySetDateTime()
{
	SYSTEMTIME stTM	= {NULL};

	/* 현재 Local Time */
	GetLocalTime(&stTM);

	return ErrosMessage(PM100D_setTime(m_hDevPM100D,
									   stTM.wYear,
									   stTM.wMonth,
									   stTM.wSecond,
									   stTM.wHour,
									   stTM.wMinute,
									   stTM.wSecond));
}

/*
 desc : 전체 장비의 실시간 데이터 요청
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCaliThread::QueryTotal()
{
	ViReal64 viValue= 0.0f;
	ViStatus viState= -1;

	switch (m_enMeasType)
	{
	case ENG_MDST::en_led_power			:	viState	= PM100D_measPower(m_hDevPM100D, &viValue);		break;
	case ENG_MDST::en_led_enery			:	viState	= PM100D_measEnergy(m_hDevPM100D, &viValue);	break;
	case ENG_MDST::en_led_frequency		:	viState	= PM100D_measFreq(m_hDevPM100D, &viValue);		break;
	case ENG_MDST::en_led_enery_density	:	viState	= PM100D_measEnergyDens(m_hDevPM100D, &viValue);break;
	}

	/* If there is no measured value at all ... */
	if (!ErrosMessage(viState))	return FALSE;
	/* Push the current power */
	m_pArrQue->PushQue(DOUBLE(viValue));
	/* Set the currently calcualted value in shared memory */
	m_pstShMemPM100D->get.max_min	= m_pArrQue->GetDiff();
	m_pstShMemPM100D->get.average	= m_pArrQue->GetAvg();
	m_pstShMemPM100D->get.std_dev	= m_pArrQue->GetStd();
	m_pstShMemPM100D->get.last_val	= viValue;

	return TRUE;
}

/*
 desc : 임의 시간 동안 측정한 값 초기화
 parm : None
 retn : None
*/
VOID CCaliThread::ResetValue()
{
	/* 동기 진입 */
	if (m_syncData.Enter())
	{
		TRACE(L"ResetValue\n");
		/* Empty the queue buffer */
		m_pArrQue->QueEmpty();
		/* 현재 구조체 값 초기화 */
		memset(&m_pstShMemPM100D->get, 0x00, sizeof(STG_PADV));
		/* 동기 해제 */
		m_syncData.Leave();
	}
}

/*
 desc : 조도 데이터 측정 여부 결정
 parm : flag	- [in]  TRUE - 시작, FALSE - 중지
 retn : TRUE or FALSE
*/
BOOL CCaliThread::RunMeasure(BOOL flag)
{
	BOOL bFlag	= FALSE;
	/* 동기 진입 */
	if (m_syncData.Enter())
	{
		/* 기존 측정한 값들 초기화 */
		if (IsConnected())
		{
			bFlag	= TRUE;
			m_bRunMeasure = flag;
		}
		/* PM100D 로부터 실시간 값 요청 후 응답 받은데 걸리는 시간 : 35 ~ 80 msec */
		/* 1 초에 10 개 정도 수집하려면, 수집 요청 주기를 최소한 100 msec로 해야함 */
		SetCycleTime(100);
		/* 동기 해제 */
		m_syncData.Leave();
	}

	return bFlag;
}

/*
 desc : Set Wave Length
 parm : wave	- [in]  Wave Length (or Line Filter) (단위: nm)
						ex> 365, 385, 390, 405 (nm)
 retn : TRUE or FALSE
*/
BOOL CCaliThread::QueryWaveLength(INT16 wave)
{
	DOUBLE dbFreq		= 0;
#if 0
	DOUBLE dbLightSpeed	= 3.0f * pow(10, 8);	/* m/s */
	DOUBLE dbNM2M		= DOUBLE(wave) * pow(10, -9);
	/* ---------------------------------------------------------------- */
	/* 주파수 (nm) 값을 진동수로 변경 (E = hν = h(c/λ) = hc/λ)			*/
	/* h = 플랑크 상수, Planck constant = 6.626 070 040×10^(-34) J•s	*/
	/* ν = 빛의 진동수													*/
	/* c = 빛의 속도 = 3×10^8 m/s										*/
	/* λ = 빛의 파장 (= c / v)											*/
	/* 1 nm = 10^(-9) m = 0.000000001 (ex. 356 nm = 365 x (10^(-9) m)	*/
	/* ---------------------------------------------------------------- */

	/* nm -> frequency (Hz) 변경  */
	dbFreq	= dbLightSpeed / dbNM2M;
#else
	dbFreq	= wave;
#endif
	if (!ErrosMessage(PM100D_setWavelength(m_hDevPM100D, ViReal64(dbFreq))))	return FALSE;

	return TRUE;
}

/*
 desc : Get Wave Length
 parm : None
 retn : 양수 값 (음수 값인 경우 실패) (365, 385, 390, 405 (nm))
*/
INT16 CCaliThread::QueryWaveLength()
{
	ViReal64 vdbWave	= 0.0f;

	/* 현재 설정되어 있는 주파수 (Hz) 값 반환 */
	if (!ErrosMessage(PM100D_getWavelength(m_hDevPM100D, PM100D_ATTR_SET_VAL, &vdbWave)))	return -1;

	/* 진동수를 파장 값으로 변환 */
	return (INT16)ROUNDED(vdbWave, 0);
}

/*
 desc : PM100D 장비 시간을 현재 컴퓨터의 시간 값으로 갱신
 parm : None
 retn : None
*/
VOID CCaliThread::UpdatePM100DTime()
{
	SYSTEMTIME stTM	= {NULL};

	GetLocalTime(&stTM);
	if (!ErrosMessage(PM100D_setTime(m_hDevPM100D, stTM.wYear, stTM.wMonth, stTM.wDay,
		stTM.wHour, stTM.wMinute, stTM.wSecond)))	return;
}