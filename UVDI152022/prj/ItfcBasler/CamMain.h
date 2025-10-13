
#pragma once

class CCamInst;
class CCamEvent;

using namespace std;

class MarkPoolManager
{
public:

	void Initialize()
	{
		pstMarkPool.reserve(POOL_COUNT);
		for (int i = 0; i < POOL_COUNT; i++)
			pstMarkPool.push_back(new STG_ACGR);
	}

	void Destroy()
	{

		for (int i = 0; i < POOL_COUNT; i++)
			delete pstMarkPool.at(i);
	}

	LPG_ACGR GetNext()
	{
		lock_guard<mutex> lock(poolLockMutex);
		if (accessIndex >= POOL_COUNT)accessIndex = 0;
		return pstMarkPool.at(accessIndex++);
	}

private:
	const int POOL_COUNT = 10;
	
	vector< LPG_ACGR> pstMarkPool;
	int accessIndex = 0;
	mutex poolLockMutex;
	
};

class CCamMain : public CImageEventHandler, public CConfigurationEventHandler
{

public:
	enum TriggerMode
	{
		IOLine,
		Software,
	};
/* 생성자 & 파괴자 */
public:

	CCamMain(LPG_CIEA config, UINT8 cam_id);
	virtual ~ CCamMain();

public:

	enum class ShutterType { Global, Rolling };

	struct ExposureCalcOptions 
	{
		double   allowed_blur_px = 1.0;  
		double   safety = 1.0; 
		uint64_t strobe_on_delay_ns = 0; 
		uint64_t strobe_off_delay_ns = 0; 
		ShutterType shutter = ShutterType::Global;

		// Rolling 전용 파라미터
		double line_time_ns = 0.0; 
		int    image_height_px = 0; 
	};

	inline uint64_t CalcExposureWithMagMs(double camPixelPitchUm,double magnification,double onTheFlySpeedmms,ExposureCalcOptions opt = {})
	{
		const int nsToms = 1000;
		if (camPixelPitchUm <= 0.0 || magnification <= 0.0 || onTheFlySpeedmms <= 0.0)
			return 0;

		const double pixel_obj_mm = (camPixelPitchUm / magnification) * 1e-3;
		double t_sec = (pixel_obj_mm * opt.allowed_blur_px) / onTheFlySpeedmms;
		t_sec *= opt.safety;

		long double t_ns = (long double)(t_sec) * 1e9L;

		if (opt.shutter == ShutterType::Rolling) 
		{
			const long double skew_ns = opt.line_time_ns * (long double)(opt.image_height_px);
			if (skew_ns > 0.0L && t_ns > skew_ns) 
				t_ns = skew_ns;
		}

		long double adj = t_ns - (long double)(opt.strobe_on_delay_ns) - (long double)(opt.strobe_off_delay_ns);

		if (adj < 0.0L) adj = 0.0L;
		return static_cast<uint64_t>(std::llround(adj)) / nsToms;
	}
	

///* 구조체 & 열거형 */
//	typedef enum class __en_camera_trigger_run_mode__
//	{
//		en_soft_trig	= 0x00,
//		en_hw_trig		= 0x01,
//		en_continue		= 0x02,
//
//	}	ENM_CTRM; //안씀.

/* 가상함수 재정의 */
public:

	// 반드시 파라미터의 형을 바꾸지 말아야 함
	virtual VOID		OnImageGrabbed(CInstantCamera& camera, const CGrabResultPtr& grabbed);	
	void DecreaseGrabCnt() 
	{
		if (m_syncGrab.Enter())
		{
			m_u8GrabIndex--;
			m_syncGrab.Leave();
		}
		

	}

/* 로컬 변수 */
protected:

	TCHAR				m_tzCamIPv4[IPv4_LENGTH];
	TCHAR				m_tzErrMsg[1024];

	ENG_VCCM			m_enCamMode;		// TRUE - Live Mode / FALSE - Grabbed Mode
	UINT8				m_u8CamID;			/* Align Camera Index (1 or Later) */
	UINT8				m_u8GrabIndex;		// 현재 Grabbed Image의 인덱스

	UINT32				m_u32WhiteCount;	/* Grabbed Image에서 검정색이 아닌 개수 */
	UINT32				m_u32ExposeTime;	/* 카메라의 조리개가 입력받기 위해 조리개를 열고 있는 시간 (단위: us) */

	UINT64				m_u64TickGrab;		/* Live Grabbed Image의 처리 주기 (단위: 밀리초) */
	UINT64				m_u64HistTotal;		/* Grabbed Image의 전체 Gray Index Level의 총 합 */
	UINT64				m_u64HistMax;		/* Grabbed Imagedml 각 넓이 픽셀 (Column) 중에서 가장 큰 Level 값 저장 */

	PUINT8				*m_pGrabBuff;		/* 연속적으로 Grabbed Image 저장 버퍼 */

	PUINT32				m_pGrayCount;		/* Grabbed Image의 각 Bytes별로 Gray Index 값 저장 */
	PUINT64				m_pHistLevel;		/* 각 이미지의 Column (넓이) 기준으로 높이 (Row)에 분포된 RGB 값 모두 더한 값 */

	STG_ACGR			m_stGrab;			// Live or Edge or Calibration Mode 임시 저장용
	LPG_CIEA			m_pstConfig;		/* 전체 환경 정보 */

	CTlFactory&			m_csTransportLayerFactory;
	CCamInst			*m_pCamera;

	CMySection			m_syncGrab;		// Grabbed Image의 동기화 객체

	CAtlList <LPG_ACGR>	m_lstMark;

	//UINT8				m_uDispType; // Disp Type, VISION 사용, MARK, MARKSET, 
	UINT8				m_uDispType; // VISION 사용, MARK TAB 유무(1 - Mark Tab, 0 -  그 외)

	LPG_ACGR pstMark;
	MarkPoolManager markPool;


/* 로컬 함수 */
protected:

	BOOL				StartGrab();
	VOID				StopGrab();


/* 공용 함수 */
public:
	ENG_TRGM 			triggerMode;
	BOOL				AttachDevice();
	VOID				DetachDevice();
	BOOL				IsConnected();

	TCHAR				*GetErrMsg()			{	return m_tzErrMsg;	}
	LPG_ACGR			GetGrabbedImage();
	VOID				ResetGrabbedImage();

	BOOL				SetParamUpdate();
	ENG_TRGM GetTriggerMode();
	bool GetSwTrigGrabImage();
	bool				ChangeTriggerMode( ENG_TRGM mode);
	VOID				SetCamMode(ENG_VCCM mode);
	ENG_VCCM			GetCamMode()			{	return m_enCamMode;	}

	UINT8				GetCamIndex()			{	return m_u8CamID;	}
	VOID				ResetGrabbedMark();

	UINT16				GetLastError();

	BOOL				SetGrabGrayRate(ENG_VCCM mode);
	BOOL				SetGrabHistLevel(ENG_VCCM mode);
	DOUBLE				GetGrabGrayRate(UINT8 index);
	DOUBLE				GetGrabGrayRateTotal();
	PUINT64				GetGrabHistLevel();
	UINT64				GetGrabHistTotal()		{	return m_u64HistTotal;	}
	UINT64				GetGrabHistMax()		{	return m_u64HistMax;	}

	BOOL				SetGainLevel(int level);
	BOOL				LoadImageFromFile(PTCHAR file, INT32 cam_id = 0);
	UINT8				GetDispType() { return m_uDispType; }
	VOID				SetDispType(UINT8 dispType) { m_uDispType = dispType; }
};

