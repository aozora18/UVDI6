
/*
 desc : Engine Configuration for GEN2I
*/

#include "pch.h"
#include "ConfGen2i.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : config		- [in]  ȯ�� ����
 retn : None
*/
CConfGen2i::CConfGen2i(LPG_CIEA config)
	: CConfBase(L"gen2i")
{
	m_pstCfg	= config;	/* ȯ�� ���� ���� ���� */
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CConfGen2i::~CConfGen2i()
{
}

/*
 desc : ȯ�� ���� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfGen2i::LoadConfig()
{
	/* �Ʒ� �Լ� ȣ�� ������ �ݵ�� ���Ѿ� �� */
	if (!LoadConfigFileName())	/* [FILE_NAME]		*/
	{
		AfxMessageBox(L"Failed to load the config for FILE_NAME", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	if (!LoadConfigPreAligner())	/* [PRE_ALIGNER]	*/
	{
		AfxMessageBox(L"Failed to load the config for PRE_ALIGNER", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	if (!LoadConfigDiamondRobot())	/* [DIAMOND_ROBOT]	*/
	{
		AfxMessageBox(L"Failed to load the config for DIAMOND_ROBOT", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	if (!LoadConfigEFU())	/* [EFU]	*/
	{
		AfxMessageBox(L"Failed to load the config for EFU", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : ȯ�� ���� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfGen2i::SaveConfig()
{

	return TRUE;
}

/*
 desc : ȯ�� ���� ���� (FILE_NAME)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfGen2i::LoadConfigFileName()
{
	/* Subject Name ���� */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"FILE_NAME");

	GetConfigStr(L"FILE_LED_POWER",		m_pstCfg->file_dat.led_power,	MAX_FILE_LEN);
	GetConfigStr(L"FILE_GERB_RECIPE",	m_pstCfg->file_dat.gerb_recipe,	MAX_FILE_LEN);
	GetConfigStr(L"FILE_PH_STEP",		m_pstCfg->file_dat.ph_step,		MAX_FILE_LEN);
	GetConfigStr(L"FILE_ACAM_CALI",		m_pstCfg->file_dat.acam_cali,	MAX_FILE_LEN);

	return TRUE;
}

/*
 desc : ȯ�� ���� ���� (PreAligner)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfGen2i::LoadConfigPreAligner()
{
	/* Subject Name ���� */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"PRE_ALIGNER");

	m_pstCfg->pre_aligner.use_lib		= GetConfigUint8(L"USE_LIB");
	m_pstCfg->pre_aligner.port_no		= GetConfigUint8(L"PORT_NO");
	m_pstCfg->pre_aligner.baud_rate		= GetConfigUint32(L"BAUD_RATE");
	m_pstCfg->pre_aligner.port_buff		= GetConfigUint32(L"PORT_BUFF");

	return TRUE;
}

/*
 desc : ȯ�� ���� ���� (Diamond Robot)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfGen2i::LoadConfigDiamondRobot()
{
	/* Subject Name ���� */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"DIAMOND_ROBOT");

	m_pstCfg->diamond_robot.use_lib		= GetConfigUint8(L"USE_LIB");
	m_pstCfg->diamond_robot.port_no		= GetConfigUint8(L"PORT_NO");
	m_pstCfg->diamond_robot.baud_rate	= GetConfigUint32(L"BAUD_RATE");
	m_pstCfg->diamond_robot.port_buff	= GetConfigUint32(L"PORT_BUFF");

	return TRUE;
}

/*
 desc : ȯ�� ���� ���� (EFU)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfGen2i::LoadConfigEFU()
{
	/* Subject Name ���� */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"EFU");

	m_pstCfg->efu.use_lib		= GetConfigUint8(L"USE_LIB");
	m_pstCfg->efu.port_no		= GetConfigUint8(L"PORT_NO");
	m_pstCfg->efu.baud_rate		= GetConfigUint32(L"BAUD_RATE");
	m_pstCfg->efu.port_buff		= GetConfigUint32(L"PORT_BUFF");
	m_pstCfg->efu.ext_param_1	= GetConfigUint8(L"EFU_COUNT");

	return TRUE;
}
