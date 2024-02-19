
/*
 desc : Engine Configuration for GEN2I - Tracking
*/

#include "pch.h"
#include "ConfTracking.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : config	- [in]  환경 정보
 retn : None
*/
CConfTracking::CConfTracking(LPG_CWTI config)
	: CConfBase(L"wafer_tracking")
{
	m_pstCfg	= config;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CConfTracking::~CConfTracking()
{
}

/*
 desc : 환경 파일 적재
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTracking::LoadConfig()
{
	UINT8 u8Error		= 0x00;
	TCHAR tzMesg[1024]	= {NULL};
	TCHAR tzCode[4][8]	= { L"Aligner", L"Loader", L"Handler", L"Buffer" };

	if (!LoadConfigAligner())	{	u8Error = 0x01;	return FALSE;	}
	if (!LoadConfigLoader())	{	u8Error = 0x02;	return FALSE;	}
	if (!LoadConfigHandler())	{	u8Error = 0x03;	return FALSE;	}
	if (!LoadConfigBuffer())	{	u8Error = 0x04;	return FALSE;	}

	if (u8Error)
	{
		swprintf_s(tzMesg, 128, L"Failed to load the config for Tracking (%s)", tzCode[u8Error-0x01]);
		AfxMessageBox(tzMesg, MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Teaching
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTracking::SaveConfig()
{
	if (!SaveConfigAligner())	return FALSE;
	if (!SaveConfigLoader())	return FALSE;
	if (!SaveConfigHandler())	return FALSE;
	if (!SaveConfigBuffer())	return FALSE;

	return TRUE;
}

/*
 desc : 환경 파일 적재 - Tracking - Aligner
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTracking::LoadConfigAligner()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"ALIGNER");

	GetConfigStr(L"ALIGNER_FOUP_INFO", m_pstCfg->aligner.foup_name, ALIGNER_FOUP_NAME, L"");
	m_pstCfg->aligner.foup_num	= (UINT8)GetConfigInt32(L"ALIGNER_FOUP_NUM");
	m_pstCfg->aligner.lot_num	= (UINT8)GetConfigInt32(L"ALIGNER_LOT_NUM");
	m_pstCfg->aligner.get_mode	= (UINT8)GetConfigInt32(L"ALIGNER_GET_MODE");
	m_pstCfg->aligner.put_mode	= (UINT8)GetConfigInt32(L"ALIGNER_PUT_MODE");

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Tracking - Aligner
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTracking::SaveConfigAligner()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"ALIGNER");

	SetConfigStr(L"ALIGNER_FOUP_INFO",		m_pstCfg->aligner.foup_name);
	SetConfigUint32(L"ALIGNER_FOUP_NUM",	m_pstCfg->aligner.foup_num);
	SetConfigUint32(L"ALIGNER_LOT_NUM",		m_pstCfg->aligner.lot_num);
	SetConfigUint32(L"ALIGNER_GET_MODE",	m_pstCfg->aligner.get_mode);
	SetConfigUint32(L"ALIGNER_PUT_MODE",	m_pstCfg->aligner.put_mode);

	return TRUE;
}

/*
 desc : 환경 파일 적재 - Tracking - Loader
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTracking::LoadConfigLoader()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"LOADER");

	GetConfigStr(L"LOADER_FOUP_INFO", m_pstCfg->loader.foup_name, ALIGNER_FOUP_NAME, L"");
	m_pstCfg->loader.foup_num	= (UINT8)GetConfigInt32(L"LOADER_FOUP_NUM");
	m_pstCfg->loader.lot_num	= (UINT8)GetConfigInt32(L"LOADER_LOT_NUM");
	m_pstCfg->loader.get_mode	= (UINT8)GetConfigInt32(L"LOADER_GET_MODE");
	m_pstCfg->loader.put_mode	= (UINT8)GetConfigInt32(L"LOADER_PUT_MODE");

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Tracking - Loader
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTracking::SaveConfigLoader()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"LOADER");

	SetConfigStr(L"LOADER_FOUP_INFO",	m_pstCfg->aligner.foup_name);
	SetConfigUint32(L"LOADER_FOUP_NUM", m_pstCfg->aligner.foup_num);
	SetConfigUint32(L"LOADER_LOT_NUM",	m_pstCfg->aligner.lot_num);
	SetConfigUint32(L"LOADER_GET_MODE",	m_pstCfg->aligner.get_mode);
	SetConfigUint32(L"LOADER_PUT_MODE",	m_pstCfg->aligner.put_mode);

	return TRUE;
}

/*
 desc : 환경 파일 적재 - Tracking - Handler
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTracking::LoadConfigHandler()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"HANDLER");

	GetConfigStr(L"HANDLER_FOUP_INFO", m_pstCfg->handler.foup_name, ALIGNER_FOUP_NAME, L"");
	m_pstCfg->handler.foup_num	= (UINT8)GetConfigInt32(L"HANDLER_FOUP_NUM");
	m_pstCfg->handler.lot_num	= (UINT8)GetConfigInt32(L"HANDLER_LOT_NUM");
	m_pstCfg->handler.get_mode	= (UINT8)GetConfigInt32(L"HANDLER_GET_MODE");
	m_pstCfg->handler.put_mode	= (UINT8)GetConfigInt32(L"HANDLER_PUT_MODE");

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Tracking - Handler
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTracking::SaveConfigHandler()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"HANDLER");

	SetConfigStr(L"HANDLER_FOUP_INFO",		m_pstCfg->handler.foup_name);
	SetConfigUint32(L"HANDLER_FOUP_NUM",	m_pstCfg->handler.foup_num);
	SetConfigUint32(L"HANDLER_LOT_NUM",		m_pstCfg->handler.lot_num);
	SetConfigUint32(L"HANDLER_GET_MODE",	m_pstCfg->handler.get_mode);
	SetConfigUint32(L"HANDLER_PUT_MODE",	m_pstCfg->handler.put_mode);

	return TRUE;
}

/*
 desc : 환경 파일 적재 - Tracking - Buffer
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTracking::LoadConfigBuffer()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"BUFFER");

	GetConfigStr(L"BUFFER_FOUP_INFO", m_pstCfg->buffer.foup_name, ALIGNER_FOUP_NAME, L"");
	m_pstCfg->buffer.foup_num	= (UINT8)GetConfigInt32(L"BUFFER_FOUP_NUM");
	m_pstCfg->buffer.lot_num	= (UINT8)GetConfigInt32(L"BUFFER_LOT_NUM");
	m_pstCfg->buffer.get_mode	= (UINT8)GetConfigInt32(L"BUFFER_GET_MODE");
	m_pstCfg->buffer.put_mode	= (UINT8)GetConfigInt32(L"BUFFER_PUT_MODE");

	return TRUE;
}

/*
 desc : 환경 파일 저장 - Tracking - Buffer
 parm : None
 retn : TRUE or FALSE
*/
BOOL CConfTracking::SaveConfigBuffer()
{
	/* Subject Name 설정 */
	wcscpy_s(m_tzSubj, MAX_SUBJ_STRING, L"BUFFER");

	SetConfigStr(L"BUFFER_FOUP_INFO",	m_pstCfg->buffer.foup_name);
	SetConfigUint32(L"BUFFER_FOUP_NUM", m_pstCfg->buffer.foup_num);
	SetConfigUint32(L"BUFFER_LOT_NUM",	m_pstCfg->buffer.lot_num);
	SetConfigUint32(L"BUFFER_GET_MODE",	m_pstCfg->buffer.get_mode);
	SetConfigUint32(L"BUFFER_PUT_MODE",	m_pstCfg->buffer.put_mode);

	return TRUE;
}
