
#pragma once

class CCamMain
{
// 생성자 & 파괴자
public:

	CCamMain(LPG_CIEA config);
	virtual ~ CCamMain();

// 로컬 변수
protected:

	TCHAR				m_tzErrMsg[1024];

	ENG_VCCM			m_enCamMode;		// TRUE - Live Mode / FALSE - Grabbed Mode
	UINT8				m_u8GrabIndex;		// 현재 Grabbed Image의 인덱스

	INT32				m_i32MemActID;		/* 카메라로부터 수신되는 이미지 (Bayer)가 저장된 메모리 ID 정보 */
	INT32				m_i32MemCnvID;		/* Bayer 이미지가 RGB 이미지로 변환되어 저장된 메모리 ID 정보 */
	INT32				m_i32BitPerPixel;
	INT32				m_i32BandWidth;		/* 가장 최근의 카메라 네트워크 대역폭 (단위: MBytes/s) */

	INT32				m_i32LastError;		/* 가장 최근에 발생된 에러 값 */

	ENG_ERVM			m_enRunMode;		/* 0x01 : CMPS, 0x02 : Vision Step */

	UINT32				m_u32WhiteCount;	/* Grabbed Image에서 검정색이 아닌 개수 */
	UINT32				m_u32GrabBytes;		/* Grabbed Image Size : Bytes */

	UINT64				m_u64HistTotal;		/* Grabbed Image의 전체 Gray Index Level의 총 합 */
	UINT64				m_u64HistMax;		/* Grabbed Imagedml 각 넓이 픽셀 (Column) 중에서 가장 큰 Level 값 저장 */

	UINT64				m_u64LastBandTime;	/* Network Bandwidth 검사 주기 (가장 최근에 검사한 시간 저장) */

	DOUBLE				m_dbCurFPS;			/* 현재 카메라의 설정에 따른 Frame Per Second */

	PUINT8				*m_pGrabBuff;		/* 연속적으로 Grabbed Image 저장 버퍼 */
	PUINT8				m_pACamImgAct;		/* 카메라로부터 수신되는 이미지 (Bayer)가 저장될 이미지 메모리 */
	PUINT8				m_pACamImgCnv;		/* 카메라로부터 수신되는 이미지 (Bayer)가 RGB로 변환되어 저장될 이미지 메모리 */
	PUINT8				m_pDragBuff;		/* Live Image 에서 특정 영역을 선택 (Drag)한 영역 이미지를 저장하고 있는 이미지 메모리 */

	PUINT32				m_pGrayCount;		/* Grabbed Image의 각 Bytes별로 Gray Index 값 저장 */
	PUINT64				m_pHistLevel;		/* 각 이미지의 Column (넓이) 기준으로 높이 (Row)에 분포된 RGB 값 모두 더한 값 */

	HIDS				m_hCam;				/* 연결된 카메라의 ID 값 */

	CAMINFO				m_stCamera;			/* 연결된 카메라 정보 */
	SENSORINFO			m_stSensor;			/* 연결된 카메라의 Sensor Information */

	LPG_CIEA			m_pstConfig;		/* 전체 환경 정보 */
	STG_ACGR			m_stGrab;			/* Live or Edge or Calibration Mode 임시 저장용 */
#if (USE_EQUIPMENT_SIMULATION_MODE)
	STG_ACGR			m_stFileGrab;		/* File 업로드 후 Grabbed Image 처리 (디버깅) */
#endif
	CMySection			m_syncGrab;			/* Grabbed Image의 동기화 객체 */

	CAtlList <LPG_ACGR>	m_lstMark;


// 로컬 함수
protected:

	VOID				SetLastError();

	BOOL				InitACam();
	VOID				CloseACam();

	INT32				GetAOISize();
	INT32				SetAllocData();
	INT32				SetParameter();

	BOOL				GetSingleGrabbedImage();
	VOID				SetLiveGrabbedImage(PUINT8 by_grab);

	HIDS				GetCamHandle();


// 공용 함수
public:

	BOOL				AttachDevice();
	VOID				DetachDevice();
	BOOL				IsConnected();
	BOOL				RunGrabbedImage();

	TCHAR				*GetErrMsg()			{	return m_tzErrMsg;	}
	LPG_ACGR			GetGrabbedImage();
	LPG_ACGR			GetGrabbedLiveImage()	{	return &m_stGrab;	}
	UINT32				GetMarkCount()			{	return (UINT32)m_lstMark.GetCount();	}
	VOID				ResetGrabbedImage();

	BOOL				SetParamUpdate();
	VOID				SetCamMode(ENG_VCCM mode);
	ENG_VCCM			GetCamMode()			{	return m_enCamMode;	}

	VOID				ResetGrabbedMark();
	BOOL				IsLiveGrabbedImage()	{	return m_stGrab.grab_size > 0;	};

	BOOL				SetGrabGrayRate(ENG_VCCM mode);
	BOOL				SetGrabHistLevel(ENG_VCCM mode);
	DOUBLE				GetGrabGrayRate(UINT8 index);
	DOUBLE				GetGrabGrayRateTotal();
	DOUBLE				GetFramePerSecond()		{	return m_dbCurFPS;	};
	PUINT64				GetGrabHistLevel();
	UINT64				GetGrabHistTotal()		{	return m_u64HistTotal;	}
	UINT64				GetGrabHistMax()		{	return m_u64HistMax;	}

	VOID				ResetError()			{	m_i32LastError = 0;	}

	BOOL				SaveLiveImageToFile(TCHAR *file);
	PUINT8				GetLastLiveDragImage(RECT drag);
#if (USE_EQUIPMENT_SIMULATION_MODE)
	VOID				SetFileGrabbedImage(UINT32 size, UINT32 width, UINT32 height, PUINT8 grab);
	LPG_ACGR			GetFileGrabbedImage()	{	return &m_stFileGrab;	}
#endif
};