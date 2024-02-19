
/*
 desc : Engine Library (Shared Memory & Support Library)
*/

#include "pch.h"
#include "MainApp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                           외부 변수                                           */
/* --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 공유 메모리 생성
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
 retn : TRUE or FALSE
*/
VOID ClosedSharedMemory()
{
	/* Normal Memory */
#if 0 /* Not used (It is managed by the QWrite project) */
	if (g_pRecipe)			delete g_pRecipe;
#endif
	if (g_pCodeToStr)		delete g_pCodeToStr;

	/* Shared Memory Object 제거 */
	if (g_pMemLuria)		delete g_pMemLuria;
	if (g_pMemMC2)			delete g_pMemMC2;
	if (g_pMemPLC)			delete g_pMemPLC;
	if (g_pMemMPA)			delete g_pMemMPA;
	if (g_pMemMDR)			delete g_pMemMDR;
	if (g_pMemEFU)			delete g_pMemEFU;
	if (g_pMemBSA)			delete g_pMemBSA;
	if (g_pMemConfExpoParam)delete g_pMemConfExpoParam;
	if (g_pMemConfTracking)	delete g_pMemConfTracking;
	if (g_pMemConfTeaching)	delete g_pMemConfTeaching;
	if (g_pMemConf)			delete g_pMemConf;			/* !!! 중요 !!!. 제일 마지막에 호출해야 됨 !!! */

	/* Normal Memory */
#if 0 /* Not used (It is managed by the QWrite project) */
	g_pRecipe			= NULL;
#endif
	g_pCodeToStr		= NULL;

	/* Shared Memory */
	g_pMemConf			= NULL;
	g_pMemConfTeaching	= NULL;
	g_pMemConfTracking	= NULL;
	g_pMemConfExpoParam	= NULL;
	g_pMemLuria			= NULL;
	g_pMemMC2			= NULL;
	g_pMemPLC			= NULL;
	g_pMemMPA			= NULL;
	g_pMemMDR			= NULL;
	g_pMemEFU			= NULL;
	g_pMemBSA			= NULL;
}

/*
 desc : 공유 메모리 생성
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		e_mode	- [in]  엔진 동작 모드
						0 : None (Engine Monitoring)
						1 : CMPS (노광 제어 SW)
						2 : Step (광학계 단차 측정)
						3 : VDOF (Align Camera DOF (Depth of Focus)
						4 : Exam (Align Shutting Inspection or Measurement)
 retn : TRUE or FALSE
*/
BOOL OpenSharedMemory(ENG_ERVM e_mode)
{
	BOOL bIsEngine	= e_mode != ENG_ERVM::en_monitoring;

	/* !!! 환경 정보 저장 공유 메모리 영역 설정 !!! */
	g_pMemConf			= new CMemConf(bIsEngine);
	g_pMemConfTeaching	= new CMemConfTeaching(bIsEngine);
	g_pMemConfExpoParam	= new CMemConfExpoParam(bIsEngine);
	g_pMemConfTracking	= new CMemConfTracking(bIsEngine);
	ASSERT(g_pMemConf && g_pMemConfTeaching && g_pMemConfExpoParam && g_pMemConfTracking);
	/* !!! 환경 정보 저장 공유 메모리 영역 설정 !!! */
	if (!g_pMemConf->CreateMap()			||
		!g_pMemConfTeaching->CreateMap()	||
		!g_pMemConfTracking->CreateMap()	||
		!g_pMemConfExpoParam->CreateMap())
	{
		return FALSE;
	}
	/* Engine인 경우만 수행 됨 */
	if (bIsEngine)
	{
		/* 환경 파일 객체 생성 */
		CConfComn *pConfComn			= new CConfComn(GetConfig());
		CConfGen2i *pConfGen2i			= new CConfGen2i(GetConfig());
		CConfTeaching *pConfTeaching	= new CConfTeaching(GetConfigTeaching());
		CConfTracking *pConfTracking	= new CConfTracking(GetConfigTracking());
		CConfExpoParam *pConfExpoParam	= new CConfExpoParam(GetConfigExpoParam());
		/* 메모리 유효성 확인 */
		ASSERT(pConfComn && pConfGen2i && pConfTeaching && pConfTracking && pConfExpoParam);
		/* 환경 파일 내용 적재 */
		if (!pConfComn->LoadConfig())		return FALSE;
		if (!pConfGen2i->LoadConfig())		return FALSE;
		if (!pConfTeaching->LoadConfig())	return FALSE;
		if (!pConfTracking->LoadConfig())	return FALSE;
		if (!pConfExpoParam->LoadConfig())	return FALSE;
		/* !!! 중요 !!! Engine 동작 모드 값 강제 설정 */
		pConfComn->GetConfig()->set_comn.engine_mode	= UINT8(e_mode);
		/* 메모리 해제 */
		delete pConfComn;
		delete pConfGen2i;
		delete pConfTeaching;
		delete pConfTracking;
		delete pConfExpoParam;
	}

	/* 공유 메모리 객체 생성 */
	g_pMemLuria	= new CMemLuria(bIsEngine, GetConfig());	/* Luria */
	g_pMemMC2	= new CMemMC2(bIsEngine, GetConfig());		/* MC2 (Sieb&Meyer) */
	g_pMemPLC	= new CMemPLC(bIsEngine, GetConfig());		/* PLC (Melsec Q) */
	g_pMemBSA	= new CMemBSA(bIsEngine, GetConfig());
	/* 메모리 유효성 확인 */
	ASSERT(g_pMemLuria && g_pMemMC2 && g_pMemPLC && g_pMemBSA);

	/* Open the Shared Memory */
	if ((g_pMemLuria && !g_pMemLuria->CreateMap())	|| (g_pMemMC2 && !g_pMemMC2->CreateMap())	||
		(g_pMemPLC && !g_pMemPLC->CreateMap())		|| (g_pMemBSA && !g_pMemBSA->CreateMap()))
	{
		ClosedSharedMemory();
		return FALSE;
	}
	if (GetConfig()->pre_aligner.use_lib)
	{
		g_pMemMPA	= new CMemMPA(bIsEngine, GetConfig());
		ASSERT(g_pMemMPA);
		if (g_pMemMPA && !g_pMemMPA->CreateMap())
		{
			ClosedSharedMemory();
			return FALSE;
		}
	}
	if (GetConfig()->diamond_robot.use_lib)
	{
		g_pMemMDR	= new CMemMDR(bIsEngine, GetConfig());
		ASSERT(g_pMemMDR);
		if (g_pMemMDR && !g_pMemMDR->CreateMap())
		{
			ClosedSharedMemory();
			return FALSE;
		}
	}
	if (GetConfig()->efu.use_lib)
	{
		g_pMemEFU	= new CMemEFU(bIsEngine, GetConfig());
		ASSERT(g_pMemEFU);
		if (g_pMemEFU && !g_pMemEFU->CreateMap())
		{
			ClosedSharedMemory();
			return FALSE;
		}
	}
	/* 현재 광학계 동작 모드가 1개 이상이 Emulation 모드로 동작 중이면 메시지 박스 출력 */
	if (g_pMemConf->GetMemMap()->luria_svc.IsRunEmulated() ||
		g_pMemConf->GetMemMap()->IsRunDemo())
	{
		/* 1 개 이상 Emuation 모드로 동작 중임을 알림 */
		TCHAR tzMesg[256]	= {NULL}, tzYesNo[2][8] = {L"NO", L"YES"};
		swprintf_s(tzMesg, 256, L"Run in emulation (MOTION:%s, PH:%s, Trigger:%s)\n"
				   L"Running in demo mode (Demo:%s)",
				   tzYesNo[g_pMemConf->GetMemMap()->luria_svc.emulate_motion],
				   tzYesNo[g_pMemConf->GetMemMap()->luria_svc.emulate_ph],
				   tzYesNo[g_pMemConf->GetMemMap()->luria_svc.emulate_trigger],
				   tzYesNo[g_pMemConf->GetMemMap()->IsRunDemo()]);
		AfxMessageBox(tzMesg, MB_ICONWARNING);
	}

	/* 에러 코드 값 반환 문자열 */
	g_pCodeToStr= new CCodeToStr(g_tzWorkDir);
	ASSERT(g_pCodeToStr);
	if (!g_pCodeToStr->LoadFile())
	{
		AfxMessageBox(L"Failed to load the file for CodeToStr (Error Code)", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
#if 0 /* Not used (It is managed by the QWrite project) */
	/* Gerber Recipe */
	g_pRecipe	= new CRecipeGen2i(g_tzWorkDir, g_pMemConf->GetMemMap());
	ASSERT(g_pRecipe);
	if (!g_pRecipe->RecipeLoad())
	{
		AfxMessageBox(L"Failed to load the file for Recipe", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
#endif

	return TRUE;
}

/*
 desc : 초기 실행할 때, 필요한 폴더 생성
 parm : None
 retn : TRUE or FALSE
*/
BOOL InitCreatePath()
{
	TCHAR tzPath[MAX_PATH_LEN]	= {NULL};

	/* Logs Path */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\logs", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;

	return TRUE;
}

/*
 desc : 종료할 때, 불필요한 폴더 파일 제거
 parm : None
 retn : TRUE or FALSE
*/
BOOL RemoveAllPath()
{
	return TRUE;
}

/*
 desc : Get the size of Shared Memory
 parm : dev_id	- [in]  
 retn : structure size (unit: bytes)
*/
UINT32 GetShMemSizeOfStruct(ENG_EDIC dev_id)
{
	switch (dev_id)
	{
	case ENG_EDIC::en_luria		:	return g_pMemLuria->CalcMemSize();
	case ENG_EDIC::en_mc2		:	return g_pMemMC2->CalcMemSize();
	case ENG_EDIC::en_melsec	:	return g_pMemPLC->CalcMemSize();
	case ENG_EDIC::en_lspa		:	return g_pMemMPA->CalcMemSize();
	case ENG_EDIC::en_lsdr		:	return g_pMemMDR->CalcMemSize();
	case ENG_EDIC::en_efu_ss	:	return g_pMemEFU->CalcMemSize();
	case ENG_EDIC::en_bsa		:	return g_pMemBSA->CalcMemSize();
	case ENG_EDIC::en_engine	:	return g_pMemConf->CalcMemSize();
	}

	return 0;
}

/* --------------------------------------------------------------------------------------------- */
/*                                           외부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                    외부 함수 - < Shared >                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Luria 전체 정보 반환
 parm : None
 retn : Luria 전체 정보가 저장된 구조체 포인터
*/
API_EXPORT LPG_LDSM uvEng_ShMem_GetLuria()
{
	if (!g_pMemLuria)	return NULL;
	return g_pMemLuria->GetMemMap();
}

/*
 desc : MC2 전체 정보 반환
 parm : None
 retn : MC2 전체 정보가 저장된 구조체 포인터
*/
API_EXPORT LPG_MDSM uvEng_ShMem_GetMC2()
{
	if (!g_pMemMC2)	return NULL;
	return g_pMemMC2->GetMemMap();
}

/*
 desc : Melsec Q PLC 전체 정보 반환
 parm : None
 retn : PLC 전체 정보가 저장된 구조체 포인터
*/
API_EXPORT LPG_PDSM uvEng_ShMem_GetPLC()
{
	if (!g_pMemPLC)	return NULL;
	return g_pMemPLC->GetMemMap();
}

/*
 desc : Melsec Q PLC data information return (struct)
 parm : None
 retn : Structure pointer where all PLC information is stored
*/
API_EXPORT LPG_PMDV uvEng_ShMem_GetPLCStruct()
{
	if (!g_pMemPLC)	return NULL;
	return LPG_PMDV(g_pMemPLC->GetMemMap()->data);
}

/*
 desc : Prealigner for Logosol 전체 정보 반환
 parm : None
 retn : 전체 정보가 저장된 구조체 포인터
*/
API_EXPORT LPG_MPDV uvEng_ShMem_GetLSPA()
{
	if (!g_pMemMPA)	return NULL;
	return g_pMemMPA->GetMemMap();
}

/*
 desc : Diamond Robot for Logosol 전체 정보 반환
 parm : None
 retn : 전체 정보가 저장된 구조체 포인터
*/
API_EXPORT LPG_MRDV uvEng_ShMem_GetLSDR()
{
	if (!g_pMemMDR)	return NULL;
	return g_pMemMDR->GetMemMap();
}

/*
 설명 :EFU for Shinsung 전체 정보 반환
 parm : None
 retn : 전체 정보가 저장된 구조체 포인터
*/
API_EXPORT LPG_EPAS uvEng_ShMem_GetEFU()
{
	if (!g_pMemEFU)	return NULL;
	return g_pMemEFU->GetMemMap();
}

/*
 desc : BSA 전체 정보 반환
 parm : None
 retn : 전체 정보가 저장된 구조체 포인터
*/
API_EXPORT LPG_RBVC uvEng_ShMem_GetBSA()
{
	if (!g_pMemBSA)	return NULL;
	return g_pMemBSA->GetMemMap();
}

/*
 desc : Wafer Tracking 전체 정보 반환
 parm : None
 retn : 전체 정보가 저장된 구조체 포인터
*/
API_EXPORT LPG_CWTI uvEng_ShMem_GetWaferTracking()
{
	if (!g_pMemConfTracking)	return NULL;
	return g_pMemConfTracking->GetMemMap();
}

/*
 desc : Teaching Position 전체 정보 반환
 parm : None
 retn : 전체 정보가 저장된 구조체 포인터
*/
API_EXPORT LPG_CTPI uvEng_ShMem_GetTeachingPosition()
{
	if (!g_pMemConfTeaching)	return NULL;
	return g_pMemConfTeaching->GetMemMap();
}

/*
 desc : Exposure Parmeter 전체 정보 반환
 parm : None
 retn : 전체 정보가 저장된 구조체 포인터
*/
API_EXPORT LPG_CTEI uvEng_ShMem_GetExpoParam()
{
	if (!g_pMemConfExpoParam)	return NULL;
	return g_pMemConfExpoParam->GetMemMap();
}

/*
 desc : Edge or DOF (vdof) 폴더 제거
 parm : type	- [in]  0x00: Edge, 0x01: vdof
 retn : None
*/
API_EXPORT VOID uvEng_RemoveAllFiles(UINT8 type)
{
	TCHAR tzPath[MAX_PATH_LEN]	= {NULL};

	/* Vision Calibration - Edge Detect */
	switch (type)
	{
	case 0x00	:	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\edge", g_tzWorkDir);	break;
	case 0x01	:	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\vdof", g_tzWorkDir);	break;
	default		:	return;
	}
	/* 기존 저장되어 있는 파일 모두 지우기 */
	uvCmn_DeleteAllFiles(tzPath);
}

/*
 desc : Get the size of Shared Memory
 parm : dev_id	- [in]  
 retn : structure size (unit: bytes)
*/
API_EXPORT UINT32 uvEng_GetShMemSizeOfStruct(ENG_EDIC dev_id)
{
	UINT32 u32Size = GetShMemSizeOfStruct(dev_id);
#ifdef _DEBUG
	TRACE(L"Shared Memory <Structure Size> [%02x] = %u bytes\n", dev_id);
#endif
	return u32Size;
}

/* --------------------------------------------------------------------------------------------- */
/*                                    외부 함수 - < Config >                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Luria 환경 정보
 parm : None
 retn : 환경 정보가 저장된 구조체 포인터 반환
*/
API_EXPORT LPG_CLSI uvEng_Conf_GetLuria()
{
	return &GetConfig()->luria_svc;
}

/*
 desc : 통신 환경 정보
 parm : None
 retn : 환경 정보가 저장된 구조체 포인터 반환
*/
API_EXPORT LPG_CCSI uvEng_Conf_GetComm()
{
	return &GetConfig()->set_comm;
}

/*
 desc : PLC 환경 정보
 parm : None
 retn : 환경 정보가 저장된 구조체 포인터 반환
*/
API_EXPORT LPG_CPSI uvEng_Conf_GetPLC()
{
	return &GetConfig()->melsec_q_svc;
}

/*
 desc : Teaching 환경 정보
 parm : None
 retn : 환경 정보가 저장된 구조체 포인터 반환
*/
API_EXPORT LPG_CTPI uvEng_GetConfigTeaching()
{
	return GetConfigTeaching();
}

/*
 desc : Expose Parameter 환경 정보
 parm : None
 retn : 환경 정보가 저장된 구조체 포인터 반환
*/
API_EXPORT LPG_CTEI uvEng_GetConfigExpoParamter()
{
	return GetConfigExpoParam();
}

/* --------------------------------------------------------------------------------------------- */
/*                                외부 함수 - < Config - Teaching>                               */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Teach Centering Data 저장
 변수 :	cam_no	- [in] 카메라 설정 (0x01: bsa , 0x02: up)
		point	- [in] Centering Point (QuartZ(0x01), Mark1(0x02), Mark2(0x03)
		dx		- [in] Centering Point에 대한 X Axis Position
		dy		- [in] Centering Point에 대한 Y Axis Position
 retn : None
*/
API_EXPORT VOID uvEng_Conf_Teaching_SetCenteringPos(UINT8 cam_no, UINT8 point, DOUBLE dx, DOUBLE dy)
{
	LPG_CTPI pstCfg	= GetConfigTeaching();
	if (cam_no == 0x00)	/* BSA CAM */
	{
		switch (point)
		{
		/* QuartZ */
		case 0x00 : pstCfg->cent_quaz_bsa_x_pos = (INT32)ROUNDED(dx * 10000.0f, 0);
					pstCfg->cent_quaz_bsa_y_pos = (INT32)ROUNDED(dy * 10000.0f, 0);		break;	
		/* Mark1 */
		case 0x01 : pstCfg->cent_mark1_bsa_x_pos = (INT32)ROUNDED(dx * 10000.0f, 0);
					pstCfg->cent_mark1_bsa_y_pos = (INT32)ROUNDED(dy * 10000.0f, 0);	break;	
		/* Mark2 */
		case 0x02 : pstCfg->cent_mark2_bsa_x_pos = (INT32)ROUNDED(dx * 10000.0f, 0);
					pstCfg->cent_mark2_bsa_y_pos = (INT32)ROUNDED(dy * 10000.0f, 0);	break;		
		}
	}
	else	/* UP CAM */
	{
		switch (point)
		{
		/* QuartZ */
		case 0x00 : pstCfg->cent_quaz_up_x_pos = (INT32)ROUNDED(dx * 10000.0f, 0);
					pstCfg->cent_quaz_up_y_pos = (INT32)ROUNDED(dy * 10000.0f, 0);		break;	
		/* Mark1 */
		case 0x01 : pstCfg->cent_mark1_up_x_pos = (INT32)ROUNDED(dx * 10000.0f, 0);
					pstCfg->cent_mark1_up_y_pos = (INT32)ROUNDED(dy * 10000.0f, 0);		break;	
		/* Mark2 */
		case 0x02 : pstCfg->cent_mark2_up_x_pos = (INT32)ROUNDED(dx * 10000.0f, 0);
					pstCfg->cent_mark2_up_y_pos = (INT32)ROUNDED(dy * 10000.0f, 0);		break;	
		}
	}
}

/*
 desc : Teach focus Data 저장
 변수 :	cam_no	- [in] 카메라 설정 (0x01: bsa , 0x02: up)
		point	- [in] Centering Point (QuartZ(0x01), Mark1(0x02), Mark2(0x03)
		dz		- [in] Centering Point에 대한 X Axis Position
 retn : None
*/
API_EXPORT VOID uvEng_Conf_Teaching_SetFocusPos(UINT8 cam_no, UINT8 point, DOUBLE dz)
{
	LPG_CTPI pstCfg	= GetConfigTeaching();
	if (cam_no == 0x00)	/* BSA CAM */
	{
		switch (point)
		{
		case 0x00 : pstCfg->focus_quaz_bsa_z_pos	= (INT32)ROUNDED(dz * 10000.0f, 0);	break;	/* QuartZ */
		case 0x01 : pstCfg->focus_mark1_bsa_z_pos	= (INT32)ROUNDED(dz * 10000.0f, 0);	break;	/* Mark1 */
		case 0x02 : pstCfg->focus_mark2_bsa_z_pos	= (INT32)ROUNDED(dz * 10000.0f, 0);	break;	/* Mark2 */
		}
	}
	else	/* UP CAM */
	{
		switch (point)
		{
		case 0x00 : pstCfg->focus_quaz_up_z_pos		= (INT32)ROUNDED(dz * 10000.0f, 0);	break;	/* QuartZ */
		case 0x01 : pstCfg->focus_mark1_up_z_pos	= (INT32)ROUNDED(dz * 10000.0f, 0); break;	/* Mark1 */
		case 0x02 : pstCfg->focus_mark2_up_z_pos	= (INT32)ROUNDED(dz * 10000.0f, 0);	break;	/* Mark2 */
		}
	}
}

/*
 desc : Teach Camoffset Data 저장
 parm : dx	- [in] Camera offset X
		dy	- [in] Camera offset Y
 retn : None
*/
API_EXPORT VOID uvEng_Conf_Teaching_SetCamOffset(DOUBLE dx, DOUBLE dy)
{
	LPG_CTPI pstCfg	= GetConfigTeaching();
	pstCfg->cam_offset_x = (INT32)ROUNDED(dx * 10000.0f, 0);
	pstCfg->cam_offset_y = (INT32)ROUNDED(dy * 10000.0f, 0);
}

/*
 desc : Teach Camoffset Data 저장
 parm : dx	- [in] Camera offset X
		dy	- [in] Camera offset Y
 retn : None
*/
API_EXPORT VOID uvEng_Conf_Teaching_SetQuartzCentOffset(DOUBLE dx, DOUBLE dy)
{
	LPG_CTPI pstCfg	= GetConfigTeaching();
	pstCfg->quartz_center_offset_x = (INT32)ROUNDED(dx * 10000.0f, 0);
	pstCfg->quartz_center_offset_y = (INT32)ROUNDED(dy * 10000.0f, 0);
}

/*
 desc : Teach Camoffset Data 저장
 parm : dx	- [in] Camera offset X
		dy	- [in] Camera offset Y
 retn : None
*/
API_EXPORT VOID uvEng_Conf_Teaching_SetBSAOffset(DOUBLE dx, DOUBLE dy)
{
	LPG_CTPI pstCfg	= GetConfigTeaching();
	pstCfg->bsa_up_down_offset_x = (INT32)ROUNDED(dx * 10000.0f, 0);
	pstCfg->bsa_up_down_offset_y = (INT32)ROUNDED(dy * 10000.0f, 0);
}

/*
 desc : 조도 위치 정보 설정
 parm : x	- [in]  Axis X
		x	- [in]  Axis Y
 retn : None
*/
API_EXPORT VOID uvEng_Conf_Teaching_SetIllumPos(DOUBLE x, DOUBLE y)
{
	LPG_CTPI pstCfg	= GetConfigTeaching();
	pstCfg->illum_ph_x_axis_pos[1] = (INT32)x;	/* 추후 적용할 때, 10000 곱하기 */
	pstCfg->illum_ph_x_axis_pos[1] = (INT32)y;	/* 추후 적용할 때, 10000 곱하기 */
}

/*
 desc : LD Sensor 위치 정보 설정
 parm : x		- [in]  Axis X
		y		- [in]  Axis Y
		loader	- [in]  Axis Loader
 retn : None
*/
API_EXPORT VOID uvEng_Conf_Teaching_SetLDSensorValue(DOUBLE point1, DOUBLE point2, DOUBLE point3)
{
	LPG_CTPI pstCfg	= GetConfigTeaching();
	pstCfg->focus_ld_sensor_p1 = (DOUBLE)point1;		
	pstCfg->focus_ld_sensor_p1 = (DOUBLE)point2;		
	pstCfg->focus_ld_sensor_p1 = (DOUBLE)point3;
}

/* --------------------------------------------------------------------------------------------- */
/*                           외부 함수 - < Config - Exposure Parameter>                          */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 노광 대상 거버 등록
 parm : index	- [in]  등록될 위치  (0 ~ 4)
		gerber	- [in]  등록 대상 거버 이름 (전체 경로 포함?)
 retn : None
*/
API_EXPORT VOID uvEng_Conf_ExpoParam_AddExpoGerber(UINT8 index, PTCHAR gerber)
{
	LPG_CTEI pstCfg	= GetConfigExpoParam();
	if (index < pstCfg->gerb_expo_cnt)
	{
		swprintf_s(pstCfg->gerber_expo[index], MAX_GERBER_NAME, L"%s", gerber);
	}
}

/*
 desc : 측정 대상 거버 등록
 parm : index	- [in]  등록될 위치  (0 ~ 9)
		gerber	- [in]  등록 대상 거버 이름 (전체 경로 포함?)
 retn : None
*/
API_EXPORT VOID uvEng_Conf_ExpoParam_AddMeasGerber(UINT8 index, PTCHAR gerber)
{
	LPG_CTEI pstCfg	= GetConfigExpoParam();
	if (index < pstCfg->gerb_meas_cnt)
	{
		swprintf_s(pstCfg->gerber_meas[index], MAX_GERBER_NAME, L"%s", gerber);
	}
}

/*
 desc : Teach Equipment 파일 적재
 parm : index	- [in]  등록될 위치  (0 ~ 1)
		gid		- [in]  Gerber Index (1 ~ 25)
		mark1,2	- [in]  Mark 설정 1, 2
		thick	- [in]  Material Thickness (unit: 100 nm or 0.1 um)
		energy	- [in]  최종 노광될 광량 (단위: mj)
		speed	- [in]  최종 노광될 속도 (단위: mm/sec)
 retn : None
*/
API_EXPORT VOID uvEng_Conf_ExpoParam_AddExpoRecipe(UINT8 index, UINT8 gid, UINT8 mark1, UINT8 mark2,
												   UINT32 thick, DOUBLE energy, DOUBLE speed)
{
	LPG_CTEI pstCfg	= GetConfigExpoParam();
	if (index >= pstCfg->teach_expo_cnt)	return;

	/* 구조체에 값 설정 */
	pstCfg->teach_expo[index].gid				= gid;
	pstCfg->teach_expo[index].mark1				= mark1;
	pstCfg->teach_expo[index].mark2				= mark2;
	pstCfg->teach_expo[index].material_thickness= thick;
	pstCfg->teach_expo[index].energy			= energy;
	pstCfg->teach_expo[index].speed				= speed;
}

/*
 desc : 현재 저장(등록)되어 있는 FOUP의 레시피 (GET/PUT/GID/Energy) 정보 반환
 parm : index	- [in]  저장되어 있는 위치 (0 ~ 1)
 retn : Recipe가 저장된 구조체 참조
*/
API_EXPORT LPG_EFWG uvEng_Conf_ExpoParam_GetExpoRecipe(UINT8 index)
{
	LPG_CTEI pstCfg	= GetConfigExpoParam();
	if (index >= pstCfg->teach_expo_cnt)	return NULL;

	return &pstCfg->teach_expo[index];
}

/*
 desc : 현재 등록되어 있는 거버 이름 반환
 parm : index	- [in]  저장되어 있는 위치 (0 ~ 4)
 retn : NULL or String
*/
API_EXPORT PTCHAR uvEng_Conf_ExpoParam_GetExpoGerber(UINT8 index)
{
	return GetConfigExpoParam()->gerber_expo[index];
}

/*
 desc : 현재 등록되어 있는 거버 이름 반환
 parm : index	- [in]  저장되어 있는 위치 (0 ~ 9)
 retn : NULL or String
*/
API_EXPORT PTCHAR uvEng_Conf_ExpoParam_GetMeasGerber(UINT8 index)
{
	return GetConfigExpoParam()->gerber_meas[index];
}

/*
설명 : 현재 등록되어 있는 Matrix 거버 이름 반환
변수 : type	- [in]  1번: EXPO_FOCUS_GERBER, 2번:EXPO_OFFSET_GERBER 
반환 : NULL or String
*/
API_EXPORT PTCHAR uvEng_Conf_ExpoParam_GetMatrixGerber(UINT8 type)
{
	if (type == 0x01)	return GetConfigExpoParam()->gerber_focus;

	return GetConfigExpoParam()->gerber_offset;
}

/*
 desc : 등록된 거버 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvEng_Conf_ExpoParam_ResetExpoGerber()
{
	UINT8 i	= 0x00;
	LPG_CTEI pstCfg	= GetConfigExpoParam();
	for (; i<pstCfg->gerb_expo_cnt; i++)	wmemset(pstCfg->gerber_expo[i], 0x00, MAX_GERBER_NAME);
}

/*
 desc : 등록된 거버 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvEng_Conf_ExpoParam_ResetMeasGerber()
{
	UINT8 i	= 0x00;
	LPG_CTEI pstCfg	= GetConfigExpoParam();
	for (; i<pstCfg->gerb_meas_cnt; i++)	wmemset(pstCfg->gerber_meas[i], 0x00, MAX_GERBER_NAME);
}

/*
 desc : 등록된 Recipe 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvEng_Conf_ExpoParam_ResetExpoRecipe()
{
	UINT8 i = 0x00;
	LPG_CTEI pstCfg	= GetConfigExpoParam();
	for (; i<2; i++)	pstCfg->teach_expo[i].Reset();
}

/* --------------------------------------------------------------------------------------------- */
/*                                    외부 함수 - < Engine >                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : PODIS Engine DI 초기화
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		e_mode	- [in]  엔진 동작 모드
						0 : None (Engine Monitoring)
						1 : CMPS (노광 제어 SW)
						2 : Step (광학계 단차 측정)
						3 : VDOF (Align Camera DOF (Depth of Focus)
						4 : Exam (Align Shutting Inspection or Measurement)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Init(ENG_ERVM e_mode)
{
	TCHAR tzLogs[MAX_PATH_LEN]	= {NULL};
	CUniToChar csCnv;

	if (g_bEngineInited)
	{
		AfxMessageBox(L"The engine is already running.", MB_ICONHAND);
		return FALSE;
	}

	/* 호출 대상 App 성격 저장 */
	g_enRunMode	= e_mode;

	/* 실행 (작업) 경로가 설정되어 있는지 여부 */
	if (wcslen(g_tzWorkDir) < 1)
	{
		AfxMessageBox(L"No working path has set", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* 초기 경로 생성 */
	if (!InitCreatePath())
	{
		AfxMessageBox(L"Failed to create the [Init Directory]", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* Shared Memory 열기 */
	if (!OpenSharedMemory(e_mode))	return FALSE;

	/* Only Engine Library */
	if (e_mode != ENG_ERVM::en_monitoring)
	{
		/* Log Library */
		swprintf_s(tzLogs, MAX_PATH_LEN, L"%s\\logs", g_tzWorkDir);
		if (!uvLogs_Init(tzLogs,
						 GetConfig()->set_comn.log_file_saved,
						 GetConfig()->set_comn.log_file_type))
		{
			AfxMessageBox(L"Failed to initialize the [Logs Lib]", MB_ICONSTOP|MB_TOPMOST);
			return FALSE;
		}
		/* Luria Service */
		if (g_pMemLuria && !uvLuria_Init(GetConfig(), g_pMemLuria->GetMemMap(), g_pCodeToStr))
		{
			AfxMessageBox(L"Failed to initialize the [Optic Lib]", MB_ICONSTOP|MB_TOPMOST);
			return FALSE;
		}
		/* MC2 Service */
		if (g_pMemMC2 && !uvMC2_Init(GetConfig(), g_pMemMC2->GetMemMap()))	
		{
			AfxMessageBox(L"Failed to initialize the [Motion Lib]", MB_ICONSTOP|MB_TOPMOST);
			return FALSE;
		}
		/* MCQ Driver */
		if (g_pMemPLC && !uvMCQDrv_Init(GetConfig(), g_pMemPLC->GetMemMap()))
		{
			AfxMessageBox(L"Failed to initialize the [PLC Lib]", MB_ICONSTOP|MB_TOPMOST);
			return FALSE;
		}
		/* Linked Litho Library */
		if (!uvLitho_Init(GetConfig(), g_pMemPLC->GetMemMap()->data))
		{
			AfxMessageBox(L"Failed to initialize the [Litho Lib]", MB_ICONSTOP|MB_TOPMOST);
			return FALSE;
		}
		/* PreAligner */
		if (g_pMemMPA)
		{
			if (!uvMPA_Init(GetConfig(), g_pMemMPA->GetMemMap()))
			{
				AfxMessageBox(L"Failed to initialize the [PreAligner (Logosol) Lib]", MB_ICONSTOP|MB_TOPMOST);
				return FALSE;
			}
		}
		/* Diamond Robot */
		if (g_pMemMDR)
		{
			if (!uvMDR_Init(GetConfig(), g_pMemMDR->GetMemMap()))
			{
				AfxMessageBox(L"Failed to initialize the [Diamond Robot (Logosol) Lib]", MB_ICONSTOP|MB_TOPMOST);
				return FALSE;
			}
		}
		/* Diamond Robot */
		if (g_pMemEFU)
		{
			if (!uvEFU_Init(GetConfig(), g_pMemEFU->GetMemMap()))
			{
				AfxMessageBox(L"Failed to initialize the [EFU_SS Lib]", MB_ICONSTOP|MB_TOPMOST);
				return FALSE;
			}
		}
		/* BSA */
		if (g_pMemBSA)
		{
			if (!uvBSA_Init(GetConfig(), g_pMemBSA->GetMemMap()))
			{
				AfxMessageBox(L"Failed to initialize the [BSA_Lib]", MB_ICONSTOP|MB_TOPMOST);
				return FALSE;
			}
		}
		/* PreProcessing Api */
#if (USE_PREPORCESSING_LIB)
		if (!PreProApi::Init())
		{
			AfxMessageBox(L"Failed to open the [PreProcessing Lib]", MB_ICONSTOP|MB_TOPMOST);
			return FALSE;
		}
		if (!PreProApi::SetDllPath(csCnv.Uni2Ansi(GetConfig()->set_comn.preproc_dll_path)))
		{
			AfxMessageBox(L"Failed to set the path for <PreProcess dll>", MB_ICONSTOP|MB_TOPMOST);
			return FALSE;
		}
#endif	/* USE_PREPORCESSING_LIB */
	}
	else
	{
		/* Luria Service */
		if (g_pMemLuria && !uvLuria_Open(GetConfig(), g_pMemLuria->GetMemMap()))
		{
			AfxMessageBox(L"Failed to open the [Optic Lib]", MB_ICONSTOP|MB_TOPMOST);
			return FALSE;
		}
		/* MC2 Service */
		if (g_pMemMC2 && !uvMC2_Open(GetConfig(), g_pMemMC2->GetMemMap()))	
		{
			AfxMessageBox(L"Failed to open the [Motion Lib]", MB_ICONSTOP|MB_TOPMOST);
			return FALSE;
		}
	}

	/* PEngine.exe 실행 */
	if (GetConfig()->set_comn.run_engine_monitor)
	{
		TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
#ifdef _DEBUG
		wcscpy_s(g_tzPEngine, MAX_FILE_LEN,	L"PEngineX64D.exe");
#else
		wcscpy_s(g_tzPEngine, MAX_FILE_LEN,	L"PEngineX64.exe");
#endif
		swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s", g_tzWorkDir, g_tzPEngine);
		if (!uvCmn_FindProcess(g_tzPEngine))
		{
			if (!uvCmn_RunProcess(tzFile, 1000))
			{
				AfxMessageBox(L"Failed to run the PEngine Process App", MB_ICONSTOP|MB_TOPMOST);
				return FALSE;
			}
		}
	}

	/* 엔진 초기화 되었다고 플래그 설정 */
	g_bEngineInited	= TRUE;

	return TRUE;
}

/*
 desc : PODIS Engine DI 해제
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT VOID uvEng_Close()
{
	/* 불필요한 파일 제거 */
	RemoveAllPath();
#ifndef _DEBUG
	/* 무조건 카메라의 LED 전원 끄기 */
	uvLuria_ReqSetLedLampLevel(0x01, 0);
#endif

	/* Engine Monitor S/W 강제 종료 */
	if (g_enRunMode != ENG_ERVM::en_monitoring)
	{
		if (GetConfig() && GetConfig()->set_comn.run_engine_monitor)
		{
			if (uvCmn_FindProcess(g_tzPEngine))
			{
#ifndef _DEBUG
				if (IDYES == AfxMessageBox(L"Do you want to exit the PEngine.exe together ?", MB_YESNO))
#endif
				{
					uvCmn_KillProcess(g_tzPEngine);
				}
			}
		}

		/* 통신 관련 모든 정보 초기화 */
		if (g_pMemLuria)	g_pMemLuria->GetMemMap()->link.ResetData();
		if (g_pMemMC2)		g_pMemMC2->GetMemMap()->link.ResetData();
		if (g_pMemPLC)		g_pMemPLC->GetMemMap()->link->ResetData();

		/* Engine 관련 라이브러리 해제 */
		uvMCQDrv_Close();
		uvMC2_Close();
		uvLuria_Close();
		uvMPA_Close();
		uvMDR_Close();
		uvEFU_Close();
		uvLitho_Close();
		uvBSA_Close();

		/* Log Library를 맨 마지막에 기술해야 됨 */
		uvLogs_Close();
	}
	/* Shared Memory 닫기 */
	ClosedSharedMemory();
	/* 엔진 해제 */
	g_bEngineInited	= FALSE;
}

/*
 desc : 환경 파일 다시 적재 진행
 parm : None
 retn : TRUE or FALSE
 note : Engine만 수행할 수 있는 함수
*/
API_EXPORT BOOL uvEng_ReLoadConfig()
{
	UINT8 u8EngineMode	= 0x00;

	/* Config */
	if (!g_pMemConf)	return FALSE;
	/* Engine인 경우만 수행 됨 */
	if (g_enRunMode == ENG_ERVM::en_monitoring)	return FALSE;

	/* 기존 설정된 Engine Mode 값 임시 저장 */
	u8EngineMode= GetConfig()->set_comn.engine_mode;
	/* 환경 파일 객체 생성 */
	CConfComn *pConfComn		= new CConfComn(GetConfig());
	CConfGen2i *pConfGen2i		= new CConfGen2i(GetConfig());
	CConfTeaching *pConfTeach	= new CConfTeaching(GetConfigTeaching());
	ASSERT(pConfComn && pConfGen2i && pConfTeach);
	/* 환경 파일 내용 적재 */
	if (!pConfComn->LoadConfig())	return FALSE;
	if (!pConfGen2i->LoadConfig())	return FALSE;
	if (!pConfTeach->LoadConfig())	return FALSE;
	/* !!! 중요 !!! Engine 동작 모드 값 강제 설정 */
	pConfComn->GetConfig()->set_comn.engine_mode	= u8EngineMode;
	/* 메모리 해제 */
	delete pConfComn;
	delete pConfGen2i;
	delete pConfTeach;

	return TRUE;
}

/*
 desc : 전체 환경 파일 반환
 parm : None
 retn : 환경 파일 정보가 저장된 구조체 포인터
*/
API_EXPORT LPG_CIEA uvEng_GetConfig()
{
	return GetConfig();
}

/*
 desc : 공유 메모리가 초기화 되었는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_IsInitedEngine()
{
	return g_bEngineInited;
}

/*
 desc : 환경 파일 저장
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_SaveConfig()
{
	BOOL bSucc	= FALSE;
	/* 환경 파일 객체 생성 */
	CConfComn *pConfComn		= new CConfComn(GetConfig());
	CConfGen2i *pConfGen2i		= new CConfGen2i(GetConfig());
	CConfTeaching *pConfTeach	= new CConfTeaching(GetConfigTeaching());
	ASSERT(pConfComn && pConfGen2i && pConfTeach);
	/* 환경 파일 저장 */
	bSucc	= pConfComn->SaveConfig();
	bSucc	= pConfGen2i->SaveConfig();
	bSucc	= pConfTeach->SaveConfig();
	/* 메모리 해제 */
	delete pConfComn;
	delete pConfGen2i;
	delete pConfTeach;

	return TRUE;
}

/*
 desc : 환경 파일 저장 <Only Teaching>
 parm : data	- [in]  외부로부터 입력된 데이터
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_SaveConfigTeaching(LPG_CTPI data)
{
	BOOL bSucc	= FALSE;
	LPG_CTPI pstData	= NULL;
	CConfTeaching *pConfTeach	= NULL;

	/* 내부에서 관리되는 것으로 할지, 외부에서 입력되는 것으로 할지 여부 */
	if (!data)	pstData	= GetConfigTeaching();
	else		pstData	= data;
	/* 환경 파일 객체 생성 */
	pConfTeach	= new CConfTeaching(pstData);
	ASSERT(pConfTeach);
	/* 환경 파일 저장 */
	bSucc	= pConfTeach->SaveConfig();
	/* 메모리 해제 */
	delete pConfTeach;

	return TRUE;
}

/*
 desc : 환경 파일 저장 <Only ExpoParameter>
 parm : data	- [in]  외부로부터 입력된 데이터
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_SaveConfigExpoParamter(LPG_CTEI data)
{
	BOOL bSucc	= FALSE;
	LPG_CTEI pstData	= NULL;
	CConfExpoParam *pConfExpo	= NULL;

	/* 내부에서 관리되는 것으로 할지, 외부에서 입력되는 것으로 할지 여부 */
	if (!data)	pstData	= GetConfigExpoParam();
	else		pstData	= data;
	/* 환경 파일 객체 생성 */
	pConfExpo	= new CConfExpoParam(pstData);
	ASSERT(pConfExpo);
	/* 환경 파일 저장 */
	bSucc	= pConfExpo->SaveConfig();
	/* 메모리 해제 */
	delete pConfExpo;

	return TRUE;
}

#ifdef __cplusplus
}
#endif