
#pragma once

class CCamMain
{
// ������ & �ı���
public:

	CCamMain(LPG_CIEA config);
	virtual ~ CCamMain();

// ���� ����
protected:

	TCHAR				m_tzErrMsg[1024];

	ENG_VCCM			m_enCamMode;		// TRUE - Live Mode / FALSE - Grabbed Mode
	UINT8				m_u8GrabIndex;		// ���� Grabbed Image�� �ε���

	INT32				m_i32MemActID;		/* ī�޶�κ��� ���ŵǴ� �̹��� (Bayer)�� ����� �޸� ID ���� */
	INT32				m_i32MemCnvID;		/* Bayer �̹����� RGB �̹����� ��ȯ�Ǿ� ����� �޸� ID ���� */
	INT32				m_i32BitPerPixel;
	INT32				m_i32BandWidth;		/* ���� �ֱ��� ī�޶� ��Ʈ��ũ �뿪�� (����: MBytes/s) */

	INT32				m_i32LastError;		/* ���� �ֱٿ� �߻��� ���� �� */

	ENG_ERVM			m_enRunMode;		/* 0x01 : CMPS, 0x02 : Vision Step */

	UINT32				m_u32WhiteCount;	/* Grabbed Image���� �������� �ƴ� ���� */
	UINT32				m_u32GrabBytes;		/* Grabbed Image Size : Bytes */

	UINT64				m_u64HistTotal;		/* Grabbed Image�� ��ü Gray Index Level�� �� �� */
	UINT64				m_u64HistMax;		/* Grabbed Imagedml �� ���� �ȼ� (Column) �߿��� ���� ū Level �� ���� */

	UINT64				m_u64LastBandTime;	/* Network Bandwidth �˻� �ֱ� (���� �ֱٿ� �˻��� �ð� ����) */

	DOUBLE				m_dbCurFPS;			/* ���� ī�޶��� ������ ���� Frame Per Second */

	PUINT8				*m_pGrabBuff;		/* ���������� Grabbed Image ���� ���� */
	PUINT8				m_pACamImgAct;		/* ī�޶�κ��� ���ŵǴ� �̹��� (Bayer)�� ����� �̹��� �޸� */
	PUINT8				m_pACamImgCnv;		/* ī�޶�κ��� ���ŵǴ� �̹��� (Bayer)�� RGB�� ��ȯ�Ǿ� ����� �̹��� �޸� */
	PUINT8				m_pDragBuff;		/* Live Image ���� Ư�� ������ ���� (Drag)�� ���� �̹����� �����ϰ� �ִ� �̹��� �޸� */

	PUINT32				m_pGrayCount;		/* Grabbed Image�� �� Bytes���� Gray Index �� ���� */
	PUINT64				m_pHistLevel;		/* �� �̹����� Column (����) �������� ���� (Row)�� ������ RGB �� ��� ���� �� */

	HIDS				m_hCam;				/* ����� ī�޶��� ID �� */

	CAMINFO				m_stCamera;			/* ����� ī�޶� ���� */
	SENSORINFO			m_stSensor;			/* ����� ī�޶��� Sensor Information */

	LPG_CIEA			m_pstConfig;		/* ��ü ȯ�� ���� */
	STG_ACGR			m_stGrab;			/* Live or Edge or Calibration Mode �ӽ� ����� */
#if (USE_EQUIPMENT_SIMULATION_MODE)
	STG_ACGR			m_stFileGrab;		/* File ���ε� �� Grabbed Image ó�� (�����) */
#endif
	CMySection			m_syncGrab;			/* Grabbed Image�� ����ȭ ��ü */

	CAtlList <LPG_ACGR>	m_lstMark;


// ���� �Լ�
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


// ���� �Լ�
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