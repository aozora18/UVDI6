
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
/*                                        Local Parameter                                        */
/* --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- */
/*                                       Internal Function                                       */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 공유 메모리 생성
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
 retn : TRUE or FALSE
*/
VOID ClosedSharedMemory()
{
	/* Normal Memory */
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
	if (g_pLedPower)	delete g_pLedPower;
	if (g_pThickCali)	delete g_pThickCali;
	if (g_pPhStep)		delete g_pPhStep;
	if (g_pRecipe)		delete g_pRecipe;
	if (g_pMark)		delete g_pMark;
#endif
	if (g_pCodeToStr)	delete g_pCodeToStr;

	/* Shared Memory Object 제거 */
	if (g_pMemLuria)	delete g_pMemLuria;
	if (g_pMemMC2)		delete g_pMemMC2;
	if (g_pMemPLC)		delete g_pMemPLC;
	if (g_pMemMvenc)		delete g_pMemMvenc;

	if (g_pMemTrig)		delete g_pMemTrig;
	if (g_pMemVisi)		delete g_pMemVisi;
	if (g_pMemPhStep)	delete g_pMemPhStep;

	//if (g_pMemPM100D)	delete g_pMemPM100D;
	if (g_pMemConf)		delete g_pMemConf;	/* !!! 중요 !!!. 제일 마지막에 호출해야 됨 !!! */

	/* 반드시 NULL */
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
	g_pLedPower		= NULL;
	g_pThickCali	= NULL;
	g_pRecipe		= NULL;
	g_pPhStep		= NULL;
	g_pMark			= NULL;
#endif
	g_pCodeToStr	= NULL;

	g_pMemConf		= NULL;
	g_pMemLuria		= NULL;
	g_pMemMC2		= NULL;
	g_pMemMC2b = NULL;
	g_pMemPLC		= NULL;
	g_pMemMvenc = NULL;

	g_pMemTrig		= NULL;
	g_pMemVisi		= NULL;
	g_pMemPhStep	= NULL;

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

	/* Config */
	g_pMemConf	= new CMemConf(bIsEngine);
	ASSERT(g_pMemConf);
	/* !!! 환경 정보 저장 공유 메모리 영역 설정 !!! */
	if (!g_pMemConf->CreateMap())	return FALSE;

	/* Engine인 경우만 수행 됨 */
	if (bIsEngine)
	{
		/* 환경 파일 객체 생성 */
		CConfComn *pConfComn	= new CConfComn(GetConfig());
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
		CConfPodis *pConfPodis	= new CConfPodis(GetConfig());
		ASSERT(pConfComn && pConfPodis);
#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
		CConfGen2i *pConfGen2i	= new CConfGen2i(GetConfig());
		ASSERT(pConfComn && pConfGen2i);
#endif
		/* 환경 파일 내용 적재 */
		if (!pConfComn->LoadConfig())	return FALSE;
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
		if (!pConfPodis->LoadConfig())	return FALSE;
		delete pConfPodis;
#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
		if (!pConfGen2i->LoadConfig())	return FALSE;
		delete pConfGen2i;
#endif
		/* !!! 중요 !!! Engine 동작 모드 값 강제 설정 */
		pConfComn->GetConfig()->set_comn.engine_mode	= UINT8(e_mode);
#ifndef _DEBUG
		/* 환경 파일에 모니터링 시작 시간 갱신 */
		pConfComn->UpdateMonitorData();
#endif
		/* 메모리 해제 */
		delete pConfComn;
	}

	/* 실행 모드에 따라, 카메라가 사용할 조명이 달라짐 */
	switch (e_mode)
	{
	case ENG_ERVM::en_cali_step	:	/* 조명을 항상 Ring 조명 사용하도록 설정 */
	case ENG_ERVM::en_acam_spec	:	/* 조명을 항상 Ring 조명 사용하도록 설정 */
		((LPG_CIEA)g_pMemConf->GetMemMap())->set_comn.strobe_lamp_type	= 0x01;	break;
	}

	/* Luria */
	g_pMemLuria	= new CMemLuria(bIsEngine, GetConfig());
	ASSERT(g_pMemLuria);
	/* MC2 (Sieb&Meyer) */
	g_pMemMC2	= new CMemMC2(bIsEngine, GetConfig());
	ASSERT(g_pMemMC2);
	g_pMemMC2b = new CMemMC2b(bIsEngine, GetConfig());			/* MC2 (Sieb&Meyer) */
	ASSERT(g_pMemMC2b);
	/* PLC (Melsec Q) */
	g_pMemPLC	= new CMemPLC(bIsEngine, GetConfig());
	ASSERT(g_pMemPLC);
	g_pMemMvenc = new CMemMvenc(bIsEngine, GetConfig());		/* Trigger Board */

	if (e_mode == ENG_ERVM::en_led_power)
	{
		///* PM100D */
		//g_pMemPM100D	= new CMemPM100D(bIsEngine, GetConfig());
		//ASSERT(g_pMemPM100D);
		//if ((g_pMemPM100D && !g_pMemPM100D->CreateMap()))
		//{
		//	ClosedSharedMemory();
		//	return FALSE;
		//}
	}
	else
	{
		/* Vision */
		g_pMemVisi	= new CMemVisi(bIsEngine, GetConfig());
		ASSERT(g_pMemVisi);
		/* Recipe - Photohead Offset (Step) */
		g_pMemPhStep= new CMemPhStep(bIsEngine, GetConfig());
		ASSERT(g_pMemPhStep);
		/* Trigger Board */
		g_pMemTrig	= new CMemTrig(bIsEngine, GetConfig());
		ASSERT(g_pMemTrig);

		/* Open the Shared Memory */
		if ((g_pMemTrig && !g_pMemTrig->CreateMap())	||
			(g_pMemVisi && !g_pMemVisi->CreateMap())	||
			(g_pMemPhStep && !g_pMemPhStep->CreateMap()))
		{
			ClosedSharedMemory();
			return FALSE;
		}
	}

	/* Open the Shared Memory */
	if ((g_pMemLuria && !g_pMemLuria->CreateMap())	||
		(g_pMemMC2 && !g_pMemMC2->CreateMap())		||
		(g_pMemPLC && !g_pMemPLC->CreateMap()))
	{
		ClosedSharedMemory();
		return FALSE;
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
		AfxMessageBox(tzMesg, MB_ICONWARNING|MB_TOPMOST);
	}

	/* 에러 코드 값 반환 문자열 */
	g_pCodeToStr= new CCodeToStr(g_tzWorkDir);
	ASSERT(g_pCodeToStr);
	if (!g_pCodeToStr->LoadFile())
	{
		AfxMessageBox(L"Failed to load the file for CodeToStr (Error Code)", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
	if (g_pMemPhStep && g_pMemVisi && g_pMemTrig)
	{
		/* 광학계 단차 (Offset; Step) */
		g_pPhStep	= new CPhStep(g_tzWorkDir, (LPG_OSSD)g_pMemPhStep->GetMemMap());
		ASSERT(g_pPhStep);
		if (!g_pPhStep->LoadFile())
		{
			AfxMessageBox(L"Failed to load the file (ph_step) for PH Offset (Step)", MB_ICONSTOP|MB_TOPMOST);
			return FALSE;
		}
		/* LED Power Data */
		g_pLedPower	= new CLedPower(g_tzWorkDir);
		ASSERT(g_pLedPower);
		if (!g_pLedPower->LoadFile())
		{
 			AfxMessageBox(L"Failed to load the config file for <LedPower>", MB_ICONSTOP|MB_TOPMOST);
			return FALSE;
		}
		/* Mark Recipe Data */
		g_pMark	= new CMark(g_tzWorkDir);
		ASSERT(g_pMark);
		if (!g_pMark->LoadFile())
		{
			AfxMessageBox(L"Failed to load the file (mark_model) for Mark", MB_ICONSTOP|MB_TOPMOST);
			return FALSE;
		}
		/* Calibration Table (Position) for Align Camera */
		g_pThickCali	= new CThickCali(g_tzWorkDir);
		ASSERT(g_pThickCali);
		if (!g_pThickCali->LoadFile())
		{
			AfxMessageBox(L"Failed to load the file (thick_cali) for Align Camera", MB_ICONSTOP|MB_TOPMOST);
			return FALSE;
		}
		/* Calibration Table (Position) for Trigger Board */
		g_pRecipe	= new CRecipe(g_tzWorkDir, g_pLedPower, g_pMemLuria->GetMemMap());
		ASSERT(g_pRecipe);
		if (!g_pRecipe->LoadFile())
		{
			AfxMessageBox(L"Failed to load the file for Recipe", MB_ICONSTOP|MB_TOPMOST);
			return FALSE;
		}
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

	/* Data Path */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\data", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	/* Logs Path */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\logs", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	/* Vision Calibration - DOF */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\vdof", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	/* Photohead Calibration - Step */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\step", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	/* Vision Calibration - Edge Detect */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\edge", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	/* Vision Calibration - Align Camera - X/Y Position */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\xy2d", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	/* Illumuninace Measurement */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\illu", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	/* Illumuninace Measurement */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\stst", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	/* Vision Calibration - Normal */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\live", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\cali", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	/* Grabbed Images */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\grab", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	/* Hole Size Measurement */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\hole", g_tzWorkDir);
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
	TCHAR tzPath[MAX_PATH_LEN]	= {NULL};

	/* Vision Calibration - DOF */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\vdof", g_tzWorkDir);
	uvCmn_DeleteAllFiles(tzPath);
	/* Vision Calibration - Edge Detect */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\edge", g_tzWorkDir);
	uvCmn_DeleteAllFiles(tzPath);
	/* Vision Calibration - Hole Detect */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\grab", g_tzWorkDir);
	uvCmn_DeleteHistoryFiles(tzPath, 31);

	return TRUE;
}


#ifdef __cplusplus
extern "C"
{
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                   External API - < Shared >                                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Luria 전체 정보 반환
 parm : None
 retn : Luria 전체 정보가 저장된 구조체 포인터
*/
API_EXPORT LPG_LDSM uvEng_ShMem_GetLuria()
{
	if (!g_pMemLuria)	return NULL;
	return (LPG_LDSM)g_pMemLuria->GetMemMap();
}

/*
 desc : MC2 전체 정보 반환
 parm : None
 retn : MC2 전체 정보가 저장된 구조체 포인터
*/
API_EXPORT LPG_MDSM uvEng_ShMem_GetMC2()
{
	if (!g_pMemMC2)	return NULL;
	return (LPG_MDSM)g_pMemMC2->GetMemMap();
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
 desc : Trigger Board 전체 정보 반환
 parm : None
 retn : Trigger Board 전체 정보가 저장된 구조체 포인터
*/
API_EXPORT LPG_TPQR uvEng_ShMem_GetTrig()
{
	if (!g_pMemMvenc)	return NULL;
	return (LPG_TPQR)g_pMemMvenc->GetMemMap();
}
/*
 desc : Trigger Board 통신 정보 반환
 parm : None
 retn : Trigger Board 전체 정보가 저장된 구조체 포인터
*/
API_EXPORT LPG_DLSM uvEng_ShMem_GetTrigLink()
{
	if (!g_pMemMvenc)	return NULL;
	return (LPG_DLSM)g_pMemMvenc->GetMemLink();
}

/*
 desc : Melsec Q PLC data information return (struct)
 parm : None
 retn : Structure pointer where all PLC information is stored
*/
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
API_EXPORT LPG_PMRW uvEng_ShMem_GetPLCStruct()
{
	if (!g_pMemPLC)	return NULL;
	return LPG_PMRW(g_pMemPLC->GetMemMap()->data);
}
#elif (CUSTOM_CODE_GEN2I == DELIVERY_PRODUCT_ID)
API_EXPORT LPG_PMDV uvEng_ShMem_GetPLCStruct()
{
	if (!g_pMemPLC)	return NULL;
	return LPG_PMDV(g_pMemPLC->GetMemMap()->data);
}
#endif

///*
// desc : Trigger Board 전체 정보 반환
// parm : None
// retn : Trigger Board 전체 정보가 저장된 구조체 포인터
//*/
//API_EXPORT LPG_TPQR uvEng_ShMem_GetTrig()
//{
//	if (!g_pMemTrig)	return NULL;
//	return (LPG_TPQR)g_pMemTrig->GetMemMap();
//}
//
///*
// desc : Trigger Board 통신 정보 반환
// parm : None
// retn : Trigger Board 전체 정보가 저장된 구조체 포인터
//*/
//API_EXPORT LPG_DLSM uvEng_ShMem_GetTrigLink()
//{
//	if (!g_pMemTrig)	return NULL;
//	return (LPG_DLSM)g_pMemTrig->GetMemLink();
//}

/*
 desc : Vision 전체 정보 반환
 parm : None
 retn : Vision 전체 정보가 저장된 구조체 포인터
*/
API_EXPORT LPG_VDSM uvEng_ShMem_GetVisi()
{
	if (!g_pMemVisi)	return NULL;
	return (LPG_VDSM)g_pMemVisi->GetMemMap();
}

/*
 desc : Recipe (Photohead Offset; Step) 전체 정보 반환
 parm : None
 retn : PH Step 전체 정보가 저장된 구조체 포인터
*/
API_EXPORT LPG_OSSD uvEng_ShMem_GetPhStep()
{
	if (!g_pMemPhStep)	return NULL;
	return (LPG_OSSD)g_pMemPhStep->GetMemMap();
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
 desc : PODIS Engine DI 초기화
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		e_mode	- [in]  엔진 동작 모드
						0 : None (Engine Monitoring)
						1 : CMPS (노광 제어 SW)
						2 : Step (광학계 단차 측정)
						3 : VDOF (Align Camera DOF (Depth of Focus)
						4 : Exam (Align Shutting Inspection or Measurement)
		init	- [in]  Whether each service (module) is initialized or not.
 retn : TRUE or FALSE
*/
BOOL EngineInit(ENG_ERVM e_mode, BOOL init)
{
	TCHAR tzLogs[MAX_PATH_LEN]	= {NULL};
	LPG_CIEA pstConfig	= NULL;

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
	/* Get the global config */
	pstConfig	= GetConfig();
	/* Only Engine Library */
	if (e_mode == ENG_ERVM::en_monitoring)	return FALSE;

	/* Log Library */
	swprintf_s(tzLogs, MAX_PATH_LEN, L"%s\\logs", g_tzWorkDir);
	if (!uvLogs_Init(tzLogs,
					 pstConfig->set_comn.log_file_saved,
					 pstConfig->set_comn.log_file_type))
	{
		AfxMessageBox(L"Failed to initialize the [Logs Lib]", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* Luria Service */
	if (e_mode != ENG_ERVM::en_hole_size)
	{
#ifdef _DEBUG
		if (g_pMemLuria && !uvLuria_Init(pstConfig, g_pMemLuria->GetMemMap(), g_pCodeToStr, init))
#else
		if (g_pMemLuria && !uvLuria_Init(pstConfig, g_pMemLuria->GetMemMap(), g_pCodeToStr))
#endif
		{
			AfxMessageBox(L"Failed to initialize the [Optic Lib]", MB_ICONSTOP|MB_TOPMOST);
			return FALSE;
		}
	}
	/* MC2 Service */
	if (g_pMemMC2 && !uvMC2_Init(GetConfig(), g_pMemMC2->GetMemMap(), g_pMemMC2b->GetMemMap()))
	{
		AfxMessageBox(L"Failed to initialize the [Motion Lib]", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* MCQ Driver */
	if (e_mode != ENG_ERVM::en_hole_size)
	{
		/* MCQ Driver */
		if (g_pMemPLC && !uvMCQDrv_Init(GetConfig(), g_pMemPLC->GetMemMap()))
		{
			AfxMessageBox(L"Failed to initialize the [PLC Lib]", MB_ICONSTOP | MB_TOPMOST);
			return FALSE;
		}
	}
	BYTE byPort2 = GetConfig()->trig_grab.port_no;
	if (g_pMemMvenc && !uvMvenc_Init(byPort2, GetConfig(), g_pMemMvenc->GetMemMap(),
		g_pMemMvenc->GetMemLink()))
	{
		AfxMessageBox(L"Failed to initialize the [Trigger Board(Mvenc) Lib]", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}
	//if (e_mode == ENG_ERVM::en_led_power)
	//{
	//	if (g_pMemPM100D && !uvPM100D_Init(g_pMemPM100D->GetMemMap(),
	//										ENG_MDST(GetConfig()->luria_svc.illum_meas_type)))
	//	{
	//		AfxMessageBox(L"Failed to initialize the [PM100D Lib]", MB_ICONSTOP|MB_TOPMOST);
	//		return FALSE;
	//	}
	//}
	if (e_mode != ENG_ERVM::en_led_power)
	{
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
		/* Trigger Board */
		if (g_pMemTrig && !uvTrig_Init(pstConfig, g_pMemTrig->GetMemMap(),
										g_pMemTrig->GetMemLink()))
		{
			AfxMessageBox(L"Failed to initialize the [Trigger Board Lib]", MB_ICONSTOP|MB_TOPMOST);
			return FALSE;
		}
		/* Align Camera */
		if (!pstConfig->set_podis.use_vision_lib)
		{
#ifndef _DEBUG
			AfxMessageBox(L"Vision Library is not active.", MB_ICONINFORMATION);
#endif
		}
		else
		{
			if (g_pMemVisi && !uvBasler_Init(pstConfig, g_pMemVisi->GetMemMap(),
												ENG_VLPK::en_vision_mil, g_tzWorkDir))
			{
				AfxMessageBox(L"Failed to initialize the [Basler Camera Lib]", MB_ICONSTOP|MB_TOPMOST);
				return FALSE;
			}
		}
#endif
	}

	/* 엔진 초기화 되었다고 플래그 설정 */
	g_bEngineInited	= TRUE;

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                   External API - < Config >                                   */
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



/* --------------------------------------------------------------------------------------------- */
/*                                   External API - < Config >                                   */
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
		init	- [in]  Whether each service (module) is initialized or not.
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Init(ENG_ERVM e_mode)
{
	return EngineInit(e_mode, TRUE);
}
API_EXPORT BOOL uvEng_InitEx(ENG_ERVM e_mode, BOOL init)
{
	return EngineInit(e_mode, init);
}

/*
 desc : PODIS Engine DI 해제
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT VOID uvEng_Close()
{
	UINT8 i	= 0x00;

	/* 불필요한 파일 제거 */
	RemoveAllPath();
#if 0	/* It is only used in PODIS project */
	/* Main Thread */
	if (g_pMainThread)
	{
		if (g_pMainThread->IsBusy())	g_pMainThread->Stop();
		while (g_pMainThread->IsBusy())	Sleep(100);
		delete g_pMainThread;
	}
#endif
	/* Engine Monitor S/W 강제 종료 */
	if (g_enRunMode != ENG_ERVM::en_monitoring)
	{
		if (GetConfig()->set_comn.run_engine_monitor)
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
		if (g_pMemMC2b)		g_pMemMC2b->GetMemMap()->link.ResetData();
		if (g_pMemMvenc)	g_pMemMvenc->GetMemLink()->ResetData();
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
		if (g_pMemTrig)		g_pMemTrig->GetMemLink()->ResetData();
		if (g_pMemPLC)		g_pMemPLC->GetMemMap()->link->ResetData();
		for (; i<GetConfig()->set_cams.acam_count; i++)
		{
			if (g_pMemVisi)	g_pMemVisi->GetMemMap()->link[i]->ResetData();
		}
#endif
		/* Engine 관련 라이브러리 해제 */
		uvMCQDrv_Close();
		uvMC2_Close();
		uvLuria_Close();
		uvPM100D_Close();
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
		uvBasler_Close();
		uvTrig_Close();
#endif
		/* Log Library를 맨 마지막에 기술해야 됨 */
		uvLogs_Close();
	}
	/* Shared Memory 닫기 */
	ClosedSharedMemory();
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
	CConfComn *pConfComn	= new CConfComn(GetConfig());
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
	CConfPodis *pConfPodis	= new CConfPodis(GetConfig());
	ASSERT(pConfComn && pConfPodis);
#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
	CConfGen2i *pConfGen2i	= new CConfGen2i(GetConfig());
	ASSERT(pConfComn && pConfGen2i);
#endif
	/* 환경 파일 내용 적재 */
	if (!pConfComn->LoadConfig())	return FALSE;
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
	if (!pConfPodis->LoadConfig())	return FALSE;
#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
	if (!pConfGen2i->LoadConfig())	return FALSE;
#endif
	/* !!! 중요 !!! Engine 동작 모드 값 강제 설정 */
	pConfComn->GetConfig()->set_comn.engine_mode	= u8EngineMode;
	/* 메모리 해제 */
	delete pConfComn;
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
	delete pConfPodis;
#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
	delete pConfGen2i;
#endif

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

#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
/*
 desc : 환경 파일 저장 - PH Step 부분만 저장
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_SavePhStep()
{
	BOOL bSucc	= FALSE;
	/* 환경 파일 객체 생성 */
	CConfPodis *pConfPodis	= new CConfPodis(GetConfig());
	ASSERT(pConfPodis);
	bSucc	= pConfPodis->SaveConfigPhStepXY();	/* 환경 파일 저장 */

	/* 메모리 해제 */
	delete pConfPodis;

	return TRUE;
}

/*
 desc : 환경 파일 저장 - ACam Cali 부분만 저장
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_SaveACamCali()
{
	BOOL bSucc	= FALSE;
	/* 환경 파일 객체 생성 */
	CConfPodis *pConfPodis	= new CConfPodis(GetConfig());
	ASSERT(pConfPodis);
	/* 환경 파일 저장 */
	bSucc	= pConfPodis->SaveConfigACamCali();
	/* 메모리 해제 */
	delete pConfPodis;

	return TRUE;
}
#endif

/*
 desc : 환경 파일 저장
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_SaveConfig()
{
	BOOL bSucc	= FALSE;
	/* 환경 파일 객체 생성 */
	CConfComn *pConfComn	= new CConfComn(GetConfig());
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
	CConfPodis *pConfPodis	= new CConfPodis(GetConfig());
	ASSERT(pConfComn && pConfPodis);
#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
	CConfGen2i *pConfGen2i	= new CConfGen2i(GetConfig());
	ASSERT(pConfComn && pConfGen2i);
#endif
	/* 환경 파일 저장 */
	bSucc	= pConfComn->SaveConfig();
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
	bSucc	= pConfPodis->SaveConfig();
#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
	bSucc	= pConfGen2i->SaveConfig();
#endif
	/* 메모리 해제 */
	delete pConfComn;
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
	delete pConfPodis;
#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
	delete pConfGen2i;
#endif

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                외부 함수 - < Luria for Tools >                                */
/* --------------------------------------------------------------------------------------------- */

#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
/*
 desc : 기준 Mark 2 정보 기준으로, 현재 Mark 번호에 대한 이동해야 할 카메라 X 위치 반환
 parm : mark_no	- [in]  Mark Number (0x01 or Later)
 retn : 상수 값 (mm)
*/
API_EXPORT DOUBLE uvEng_Luria_GetACamMark2MotionX(UINT16 mark_no)
{
	UINT8 u8ACamCount	= g_pMemConf->GetMemMap()->set_cams.acam_count;
	UINT8 u8MarkCount	= uvLuria_GetMarkCount(ENG_AMTF::en_global);	/* Global Mark 개수 */
	INT32 i32Mark2ACamX	= 0;
	DOUBLE dbMark2ACamX	= 0.0f;
	STG_XMXY stMarkPos	= {NULL};
	LPG_CSAI pstAlign	= &GetConfig()->set_align;
	LPG_RBGF pstRecipe	= g_pRecipe->GetSelectRecipe();	/* 현재 선택된 거버 레시피 */
	LPG_MACP pstThick	= g_pThickCali->GetRecipe(pstRecipe->material_thick);

	/* Mark Number의 경우 1 based */
	if (mark_no < 0x01)	return 0.0f;
	/* 현재 적재된 거버의 Mark ? 번(Left/Bottom)에 대한 X, Y 좌표 값 가져오기 */
	if (!uvLuria_GetGlobalMark(mark_no-1, stMarkPos))	return 0.0f;

	/* 얼라인 카메라가 2개인 경우 */
	dbMark2ACamX	= pstThick->mark2_acam_x[0];	/*pstAlign->mark2_acam_x[0];*/
	if (u8ACamCount == 2)
	{
		/* Global Mark 개수가 카메라 개수 (?)보다 많은 경우 */
		if (mark_no > (u8MarkCount/u8ACamCount))
		{
			/* Align Camera 2번의 Mark 2번의 모션 기준 */
			dbMark2ACamX	= pstThick->mark2_acam_x[1];	/*pstAlign->mark2_acam_x[1];*/
		}
	}
	else if (u8ACamCount > 2)
	{
		/* 일단, 얼라인 카메라가 2개 이상인 경우는 별도 처리 */
		return 0.0f;
	}

	return	(dbMark2ACamX - pstAlign->mark2_org_gerb_xy[0]) + stMarkPos.mark_x;
}

/*
 desc : 현재 적재된 거버의 임의 Mark Number에 대해, 이동하고자 하는 모션의 Stage X / Y 위치 반환
 parm : mark_no	- [in]  Global Mark Number (1 or Later)
		acam_x	- [out] Motion 이동 위치 반환 (X : mm)
		stage_y	- [out] Motion 이동 위치 반환 (Y : mm)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Luria_GetGlobalMarkMoveXY(UINT16 mark_no, DOUBLE &acam_x, DOUBLE &stage_y)
{
	UINT8 u8Mark		= uvLuria_GetMarkCount(ENG_AMTF::en_global);
	DOUBLE dbACamDistX	= 0.0f, dbACamDistY = 0.0f;	/* 2 대 이상의 Align Camera 일 경우, 각 Camera 간의 설치 오차 값 (단위: mm) */
	STG_XMXY stMarkPos	= {NULL};	/* mark_no에 대한 좌표 위치 얻기 */
	LPG_CSAI pstAlign	= &GetConfig()->set_align;
	LPG_RBGF pstRecipe	= g_pRecipe->GetSelectRecipe();	/* 현재 선택된 거버 레시피 */
	LPG_MACP pstThick	= g_pThickCali->GetRecipe(pstRecipe->material_thick);

	/* 만약 Mark 개수가 2 or 4개 아니면 에러 처리 */
	if (0 != (u8Mark % 2))	return FALSE;

	/* 물리적으로 떨어진 카메라 간의 모션 좌표 오차 값 얻기 */
	GetConfig()->acam_spec.GetACamCentDist(dbACamDistX, dbACamDistY);
	/* 현재 거버에 등록된 Mark Number에 대한 좌표 위치 얻기 */
	if (!uvLuria_GetGlobalMark(mark_no-1, stMarkPos))	return FALSE;

	/* 마크가 4개인 경우 */
	if (0x04 == u8Mark)
	{
		/* -------------------------------------------------- */
		/* 1. 최종 이동하고자 하는 Motion의 Stage Y 좌표 계산 */
		/* -------------------------------------------------- */
		/* 기준 Mark 2 번의 Motion Y 위치 + 거버 Mark 2 번의 Y 위치 - 기준 Mark 2 번의 Y 위치 */
		stage_y	= pstThick->mark2_stage_y[0];	/*pstAlign->mark2_stage_y[0];*/
		if (mark_no > 0x02)	stage_y	= pstThick->mark2_stage_y[1];	/*pstAlign->mark2_stage_y[1];*/
		stage_y	+= (stMarkPos.mark_y - pstAlign->mark2_org_gerb_xy[1]);
		/* -------------------------------------------------- */
		/* 2. 최종 이동하고자 하는 Motion의 Stage X 좌표 계산 */
		/* -------------------------------------------------- */
		acam_x	= uvEng_Luria_GetACamMark2MotionX(mark_no);
	}
	/* Mark 2개 인 경우 */
	else
	{
		/* Align Camera 2 번일 경우, Align Camera 1 번과의 Y 설치 오차 값 계산 */
		if (mark_no == 0x01)	stage_y	-= dbACamDistY;
		/* 얼라인 Mark 2번에 해당되는 카메라 1번의 X 축 실제 모션 위치 */
		acam_x	= uvEng_Luria_GetACamMark2MotionX(mark_no);
	}

	return TRUE;
}
#endif

#ifdef __cplusplus
}
#endif