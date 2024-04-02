
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
/* ������ & �ı��� */
public:

	CCamMain(LPG_CIEA config, UINT8 cam_id);
	virtual ~ CCamMain();

public:

/* ����ü & ������ */
	typedef enum class __en_camera_trigger_run_mode__
	{
		en_soft_trig	= 0x00,
		en_hw_trig		= 0x01,
		en_continue		= 0x02,

	}	ENM_CTRM;

/* �����Լ� ������ */
public:

	// �ݵ�� �Ķ������ ���� �ٲ��� ���ƾ� ��
	virtual VOID		OnImageGrabbed(CInstantCamera& camera, const CGrabResultPtr& grabbed);	

/* ���� ���� */
protected:

	TCHAR				m_tzCamIPv4[IPv4_LENGTH];
	TCHAR				m_tzErrMsg[1024];

	ENG_VCCM			m_enCamMode;		// TRUE - Live Mode / FALSE - Grabbed Mode
	UINT8				m_u8CamID;			/* Align Camera Index (1 or Later) */
	UINT8				m_u8GrabIndex;		// ���� Grabbed Image�� �ε���

	UINT32				m_u32WhiteCount;	/* Grabbed Image���� �������� �ƴ� ���� */
	UINT32				m_u32ExposeTime;	/* ī�޶��� �������� �Է¹ޱ� ���� �������� ���� �ִ� �ð� (����: us) */

	UINT64				m_u64TickGrab;		/* Live Grabbed Image�� ó�� �ֱ� (����: �и���) */
	UINT64				m_u64HistTotal;		/* Grabbed Image�� ��ü Gray Index Level�� �� �� */
	UINT64				m_u64HistMax;		/* Grabbed Imagedml �� ���� �ȼ� (Column) �߿��� ���� ū Level �� ���� */

	PUINT8				*m_pGrabBuff;		/* ���������� Grabbed Image ���� ���� */

	PUINT32				m_pGrayCount;		/* Grabbed Image�� �� Bytes���� Gray Index �� ���� */
	PUINT64				m_pHistLevel;		/* �� �̹����� Column (����) �������� ���� (Row)�� ������ RGB �� ��� ���� �� */

	STG_ACGR			m_stGrab;			// Live or Edge or Calibration Mode �ӽ� �����
	LPG_CIEA			m_pstConfig;		/* ��ü ȯ�� ���� */

	CTlFactory&			m_csTransportLayerFactory;
	CCamInst			*m_pCamera;

	CMySection			m_syncGrab;		// Grabbed Image�� ����ȭ ��ü

	CAtlList <LPG_ACGR>	m_lstMark;

	//UINT8				m_uDispType; // Disp Type, VISION ���, MARK, MARKSET, 
	UINT8				m_uDispType; // VISION ���, MARK TAB ����(1 - Mark Tab, 0 -  �� ��)

	LPG_ACGR pstMark;
	MarkPoolManager markPool;


/* ���� �Լ� */
protected:

	BOOL				StartGrab();
	VOID				StopGrab();


/* ���� �Լ� */
public:

	BOOL				AttachDevice();
	VOID				DetachDevice();
	BOOL				IsConnected();

	TCHAR				*GetErrMsg()			{	return m_tzErrMsg;	}
	LPG_ACGR			GetGrabbedImage();
	VOID				ResetGrabbedImage();

	BOOL				SetParamUpdate();
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

	BOOL				SetGainLevel(UINT8 level);
	BOOL				LoadImageFromFile(PTCHAR file, INT32 cam_id = 0);
	UINT8				GetDispType() { return m_uDispType; }
	VOID				SetDispType(UINT8 dispType) { m_uDispType = dispType; }
};

