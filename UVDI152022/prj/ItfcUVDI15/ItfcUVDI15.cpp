
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

UINT8					g_u8WorkJobID			= 0x00;		/* 가장 최근에 수행 했던 작업 코드 */

UINT32					g_u32JobCount			= 0;		/* 동일한 작업 개수 */
UINT64					g_u64JobWorkTime		= 0;		/* 현재 동작 중인 작업 시간 갱신 */
CAtlList <UINT64>		g_lstJobWorkTime;					/* 가장 최근에 발생된 작업 시간 저장 */
DOUBLE					g_dbWorkStepRate		= 0.0f;		/* 전체 작업 단계 진행률 */
PTCHAR					g_ptzWorkStepName		= NULL;
PTCHAR					optionalWorkstepText = NULL;
MarkoffsetInfo			markoffset;
CMySection				g_syncJobWorkTime;

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
	if (g_pLedPower)		delete g_pLedPower;
	if (g_pACamCali)		delete g_pACamCali;
	if (g_pThickCali)		delete g_pThickCali;
	if (g_pPhStep)			delete g_pPhStep;
	if (g_pCorrectY)		delete g_pCorrectY;
	if (g_pRecipe)			delete g_pRecipe;
	if (g_pMark)			delete g_pMark;
	if (g_pCodeToStr)		delete g_pCodeToStr;

	/* Shared Memory Object 제거 */
	if (g_pMemLuria)		delete g_pMemLuria;
	if (g_pMemMC2)			delete g_pMemMC2;
	if (g_pMemMC2b)			delete g_pMemMC2b;
	//if (g_pMemTrig)			delete g_pMemTrig;
	if (g_pMemVisi)			delete g_pMemVisi;
	if (g_pMemPhStep)		delete g_pMemPhStep;
	if (g_pMemPhCorrectY)	delete g_pMemPhCorrectY;
	if (g_pMemGentec)		delete g_pMemGentec;
	if (g_pMemMvenc)		delete g_pMemMvenc;
	if (g_pMemPhilhmi)		delete g_pMemPhilhmi;
	if (g_pMemStrobeLamp)	delete g_pMemStrobeLamp;
	if (g_pMemConf)
	{
		g_pMemConf->GetMemMap()->measure_flat.Deallocate(false);
		delete g_pMemConf;	/* !!! 중요 !!!. 제일 마지막에 호출해야 됨 !!! */
	}

	/* 반드시 NULL */
	g_pLedPower		= NULL;
	g_pACamCali		= NULL;
	g_pThickCali	= NULL;
	g_pRecipe		= NULL;
	g_pPhStep		= NULL;
	g_pCorrectY		= NULL;
	g_pMark			= NULL;
	g_pCodeToStr	= NULL;

	g_pMemConf		= NULL;
	g_pMemLuria		= NULL;
	g_pMemMC2		= NULL;
	g_pMemMC2b		= NULL;
	//g_pMemTrig		= NULL;
	g_pMemVisi		= NULL;
	g_pMemPhStep	= NULL;
	g_pMemPhCorrectY= NULL;
	g_pMemMvenc		= NULL;
	g_pMemGentec	= NULL;
	g_pMemPhilhmi	= NULL;
	g_pMemStrobeLamp = NULL;

	/* 작업 이름 메모리 해제 */
	if (g_ptzWorkStepName)	delete g_ptzWorkStepName;
	delete optionalWorkstepText;
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

	/* 작업 이름 임시 저장을 위해 메모리 할당 */
	g_ptzWorkStepName = new TCHAR[WORK_NAME_LEN];// (PTCHAR)::Alloc(sizeof(TCHAR) * WORK_NAME_LEN);
	ASSERT(g_ptzWorkStepName);
	wmemset(g_ptzWorkStepName, 0x00, WORK_NAME_LEN);

	optionalWorkstepText = new TCHAR[WORK_NAME_LEN];
	wmemset(optionalWorkstepText, 0x00, WORK_NAME_LEN);
	/* Config */
	g_pMemConf	= new CMemConf(bIsEngine);
	ASSERT(g_pMemConf);
	/* !!! 환경 정보 저장 공유 메모리 영역 설정 !!! */
	if (!g_pMemConf->CreateMap())	return FALSE;

	auto* cfgPtr = GetConfig();
	cfgPtr->measure_flat.Deallocate(true);
	/* Engine인 경우만 수행 됨 */
	if (bIsEngine)
	{
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15_LLS06)
		/* 환경 파일 객체 생성 */
		CConfComn* pConfComn = new CConfComn(GetConfig());
		CConfUvdi15* pConfUvdi15 = new CConfUvdi15(GetConfig());
		/* 메모리 유효성 확인 */
		ASSERT(pConfComn && pConfUvdi15);
		/* 환경 파일 내용 적재 */
		if (!pConfComn->LoadConfig())
		{
			AfxMessageBox(L"Failed to load the config file for COMMON", MB_ICONSTOP | MB_TOPMOST);
			return FALSE;
		}
		if (!pConfUvdi15->LoadConfig())
		{
			AfxMessageBox(L"Failed to load the config file for UVDI15", MB_ICONSTOP | MB_TOPMOST);
			return FALSE;
		}
		/* !!! 중요 !!! Engine 동작 모드 값 강제 설정 */
		pConfComn->GetConfig()->set_comn.engine_mode = UINT8(e_mode);
		/* 환경 파일에 모니터링 시작 시간 갱신 */
		pConfComn->UpdateMonitorData();
		/* 메모리 해제 */
		delete pConfUvdi15;
		delete pConfComn;
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15|| \
DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
		/* 환경 파일 객체 생성 */
		
		
		cfgPtr->set_align.SetMarkoffsetPtr(markoffset);

		CConfComn* pConfComn = new CConfComn(cfgPtr);
		CConfUvdi15* pConfUvDI15 = new CConfUvdi15(cfgPtr);
		/* 메모리 유효성 확인 */
		ASSERT(pConfComn && pConfUvDI15);
		/* 환경 파일 내용 적재 */
		if (!pConfComn->LoadConfig())
		{
			AfxMessageBox(L"Failed to load the config file for COMMON", MB_ICONSTOP | MB_TOPMOST);
			return FALSE;
		}
		if (!pConfUvDI15->LoadConfig())
		{
			AfxMessageBox(L"Failed to load the config file for UVDI15", MB_ICONSTOP | MB_TOPMOST);
			return FALSE;
		}
		/* !!! 중요 !!! Engine 동작 모드 값 강제 설정 */
		pConfComn->GetConfig()->set_comn.engine_mode = UINT8(e_mode);
		/* 환경 파일에 모니터링 시작 시간 갱신 */
		pConfComn->UpdateMonitorData();
		/* 메모리 해제 */
		delete pConfUvDI15;
		delete pConfComn;
#endif

	}

	/* 공유 메모리 객체 생성 */
	g_pMemLuria		= new CMemLuria(bIsEngine, GetConfig());		/* Luria */
	g_pMemMC2		= new CMemMC2(bIsEngine, GetConfig());			/* MC2 (Sieb&Meyer) */
	g_pMemMC2b		= new CMemMC2b(bIsEngine, GetConfig());			/* MC2 (Sieb&Meyer) */
	g_pMemVisi		= new CMemVisi(bIsEngine, GetConfig());			/* Vision */
	g_pMemPhStep	= new CMemPhStep(bIsEngine, GetConfig());		/* Recipe - Photohead Offset (Step) */
	g_pMemPhCorrectY= new CMemPhCorrectY(bIsEngine, GetConfig());	/* Recipe - Photohead Offset (Step) */
	//g_pMemTrig		= new CMemTrig(bIsEngine, GetConfig());			/* Trigger Board */
	
	g_pMemMvenc		= new CMemMvenc(bIsEngine, GetConfig());		/* Trigger Board */
	g_pMemGentec	= new CMemGentec(bIsEngine, GetConfig());		/* LED Detect */
	g_pMemPhilhmi	= new CMemPhilhmi(bIsEngine, GetConfig());		/* Philhmi */
	g_pMemStrobeLamp = new CMemStrobeLamp(bIsEngine, GetConfig());	/* Strobe Lamp */

	/* 메모리 유효성 확인 */
	ASSERT(g_pMemLuria && g_pMemMC2 && g_pMemMC2b && g_pMemVisi &&
		   g_pMemPhStep && g_pMemPhCorrectY /*&& g_pMemTrig*/ && g_pMemGentec && g_pMemMvenc);

	/* Open the Shared Memory */
	if (!g_pMemLuria->CreateMap()	||
		!g_pMemMC2->CreateMap()		||
		!g_pMemMC2b->CreateMap()	||
		/*!g_pMemTrig->CreateMap()	||*/
		!g_pMemVisi->CreateMap()	||
		!g_pMemPhStep->CreateMap()	||
		!g_pMemPhCorrectY->CreateMap() ||
		!g_pMemGentec->CreateMap()	||
		!g_pMemMvenc->CreateMap()	|| 
		!g_pMemPhilhmi->CreateMap() ||
		!g_pMemStrobeLamp->CreateMap())
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
	/* 광학계 단차 (Offset; Step) */
	g_pPhStep	= new CPhStep(g_tzWorkDir, (LPG_OSSD)g_pMemPhStep->GetMemMap());
	ASSERT(g_pPhStep);
	if (!g_pPhStep->LoadFile())
	{
		AfxMessageBox(L"Failed to load the file for PH Offset (Step)", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* 광학계 Stripe (Offset; Adjust) Correction Table (Y Axis) */
	g_pCorrectY	= new CCorrectY(g_tzWorkDir, (LPG_OSCY)g_pMemPhCorrectY->GetMemMap());
	ASSERT(g_pCorrectY);
	if (!g_pCorrectY->LoadFile())
	{
		AfxMessageBox(L"Failed to load the file for PH Correction Y (Stripe)", MB_ICONSTOP|MB_TOPMOST);
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
	/* Camera Calibration */
	g_pACamCali	= new CACamCali(g_tzWorkDir, g_pMemVisi->GetMemMap());
	ASSERT(g_pACamCali);
#if 0	/* 레시피 선택할 때마다 적재하기 위함*/
	if (!g_pACamCali->LoadFile((UINT16)ROUNDED(g_pMemConf->GetMemMap()->set_align.dof_film_thick * 1000.0f, 0)))
	{
 		AfxMessageBox(L"Failed to load the config file for <ACamCali>", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
#endif
	/* Calibration Table (Position) for Align Camera */
	g_pThickCali	= new CThickCali(g_tzWorkDir);
	ASSERT(g_pThickCali);
	if (!g_pThickCali->LoadFile())
	{
		AfxMessageBox(L"Failed to load the file (thick_cali) for Align Camera", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15_LLS06)
	/* Mark Recipe Data */
	g_pMark = new CMark(g_tzWorkDir);
	ASSERT(g_pMark);
	if (!g_pMark->LoadFile())
	{
		AfxMessageBox(L"Failed to load the file (mark_model) for Mark", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}
	else {
		LPG_CRD tmpROI;
		tmpROI = g_pMark->GetMarkROI();
		CRect cROI;
		for (int i = 0; i < 2; i++) {
			cROI.left = tmpROI->roi_Left[i];
			cROI.right = tmpROI->roi_Right[i];
			cROI.top = tmpROI->roi_Top[i];
			cROI.bottom = tmpROI->roi_Bottom[i];

			//uvCmn_Camera_MilSetMarkROI(i, cROI);
		}
	}
	/* Gerber Recipe */
	g_pRecipe	= new CRecipeUVDI15(g_tzWorkDir, g_pLedPower, g_pMemLuria->GetMemMap());
	ASSERT(g_pRecipe);
	if (!g_pRecipe->LoadFile())
	{
		AfxMessageBox(L"Failed to load the file for Recipe", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15|| \
DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	/* Mark, Align Recipe Data */
	g_pMark = new CMarkUVDI15(g_tzWorkDir);
	ASSERT(g_pMark);
	if (!g_pMark->LoadFile())
	{
		AfxMessageBox(L"Failed to load the file (mark_model) for Mark", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}
	/* Job, Expo Recipe */
	g_pRecipe = new CRecipeUVDI15(g_tzWorkDir, g_pLedPower, g_pMemLuria->GetMemMap());
	ASSERT(g_pRecipe);
	if (!g_pRecipe->LoadFile())
	{
		AfxMessageBox(L"Failed to load the file for Recipe", MB_ICONSTOP | MB_TOPMOST);
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

	/*결과 이미지 저장 폴더 관리*/
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\save_img", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	/* Cali Image bmp */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\save_img\\cali", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	/* Edge Path */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\save_img\\edge", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	/* Vision Image - Grabbed Image */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\save_img\\grab", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	/* Vision Image - Live Image */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\save_img\\live", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	/* Mark Path */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\save_img\\mark", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;

	/*결과 Log 텍스트 저장 폴더 관리*/
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\logs", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	/* Logs Path */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\logs\\DlgFile", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	/* Exposure Result */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\logs\\expo", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;
	/* Temperature */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\logs\\temp", g_tzWorkDir);
	if (!uvCmn_CreateDirectory(tzPath))	return FALSE;


	/* Data Path */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\%s", g_tzWorkDir, CUSTOM_DATA_CONFIG);
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

	/* Vision Calibration - Edge Detect */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\edge", g_tzWorkDir);
	uvCmn_DeleteAllFiles(tzPath);
	/* Vision Calibration - Live */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\live", g_tzWorkDir);
	uvCmn_DeleteAllFiles(tzPath);

	return TRUE;
}

/*
 desc : 모든 장비가 연결되어 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
BOOL IsConnectedAll()
{
#if 1
	/* 모든 장비와 연결이 되어 있는지도 확인 필요 */
	return (uvMC2_IsConnected() && uvBasler_IsConnectedAll() &&
			uvLuria_IsConnected()/* && uvTrig_IsConnected()*/ && uvLuria_IsServiceInited()
			&& uvMvenc_IsConnected());
#else
	BOOL bIsConnected	= TRUE;
	if (bIsConnected)	bIsConnected	= uvMC2_IsConnected();
	if (bIsConnected)	bIsConnected	= uvBasler_IsConnectedAll();
	if (bIsConnected)	bIsConnected	= uvLuria_IsConnected();
	if (bIsConnected)	bIsConnected	= uvTrig_IsConnected();
	if (bIsConnected)	bIsConnected	= uvLuria_IsServiceInited();
	return bIsConnected;
#endif
}

/*
 desc : 데이터 파일 다시 적재
 parm : None
 retn : TRUE or FALSE
*/
BOOL ReloadCaliFile()
{
	/* 광학계 단차 (Offset; Step) */
	if (!g_pPhStep->LoadFile())
	{
		AfxMessageBox(L"Failed to load the file for PH Offset (Step)", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* 광학계 단차 (Offset; Correction Y) (Stripe) */
	if (!g_pCorrectY->LoadFile())
	{
		AfxMessageBox(L"Failed to load the file for PH Correction Y (Stripe)", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* LED Power Data */
	if (!g_pLedPower->LoadFile())
	{
 		AfxMessageBox(L"Failed to load the config file for <LedPower>", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* Calibration Table (Position) for Align Camera */
	if (!g_pThickCali->LoadFile())
	{
		AfxMessageBox(L"Failed to load the file (thick_cali) for Align Camera", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : 각종 작업 관련 시간 및 통계 정보 초기화
 parm : reset	- [in]  작업자의 수동 조작 (버튼 클릭)에 의해 리셋 여부
 retn : None
*/
VOID ResetJobWorkInfo(BOOL reset)
{
	/* 작업 횟수 초기화 */
	g_u32JobCount	= 0;
	if (reset)	g_u8WorkJobID	= 0x00;
	g_u64JobWorkTime= 0;
	g_lstJobWorkTime.RemoveAll();
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
// API_EXPORT LPG_PDSM uvEng_ShMem_GetPLC()
// {
// 	if (!g_pMemPLC)	return NULL;
// 	return g_pMemPLC->GetMemMap();
// }
// API_EXPORT LPG_PMRW uvEng_ShMem_GetPLCExt()
// {
// 	PUINT8 pMemPLC	= NULL;
// 	if (!g_pMemPLC)	return NULL;
// 	pMemPLC	= g_pMemPLC->GetMemMap()->data;
// 	return LPG_PMRW(pMemPLC);
// }


/*
 desc : Trigger Board 전체 정보 반환
 parm : None
 retn : Trigger Board 전체 정보가 저장된 구조체 포인터
*/
//API_EXPORT LPG_TPQR uvEng_ShMem_GetTrig()
// {
// 	if (!g_pMemTrig)	return NULL;
// 	return (LPG_TPQR)g_pMemTrig->GetMemMap();
// }

/*
 desc : Trigger Board 통신 정보 반환
 parm : None
 retn : Trigger Board 전체 정보가 저장된 구조체 포인터
*/
// API_EXPORT LPG_DLSM uvEng_ShMem_GetTrigLink()
// {
// 	if (!g_pMemTrig)	return NULL;
// 	return (LPG_DLSM)g_pMemTrig->GetMemLink();
// }

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
 desc : Photohead 단차 정보 반환
 parm : None
 retn : 단차 정보가 저장된 구조체 포인터
*/
API_EXPORT LPG_OSSD uvEng_ShMem_GetPhStep()
{
	if (!g_pMemVisi)	return NULL;
	return (LPG_OSSD)g_pMemPhStep->GetMemMap();
}

/*
 desc : Photohead Stripe Correct Table Y 정보 반환
 parm : None
 retn : 단차 정보가 저장된 구조체 포인터
*/
API_EXPORT LPG_OSCY uvEng_ShMem_GetPhCorrectY()
{
	if (!g_pMemVisi)	return NULL;
	return (LPG_OSCY)g_pMemPhCorrectY->GetMemMap();
}

/*
 desc : Philhmi 전체 정보 반환
 parm : None
 retn : 통신 구조체
*/
API_EXPORT LPG_PPR uvEng_ShMem_GetPhilhmi()
{
	if (!g_pMemPhilhmi)	return NULL;
	return (LPG_PPR)g_pMemPhilhmi->GetMemMap();
}

API_EXPORT LPG_DLSM uvEng_ShMem_GetPhilhmiLink()
{
	if (!g_pMemPhilhmi)	return NULL;
	return (LPG_DLSM)g_pMemPhilhmi->GetMemLink();
}

/*
 desc : Strobe Lamp 전체 정보 반환
 parm : None
 retn : 통신 구조체
*/
API_EXPORT LPG_SLPR uvEng_ShMem_GetStrobeLamp()
{
	if (!g_pMemStrobeLamp)	return NULL;
	return (LPG_SLPR)g_pMemStrobeLamp->GetMemMap();
}


API_EXPORT LPG_DLSM uvEng_ShMem_GetStrobeLampLink()
{
	if (!g_pMemStrobeLamp)	return NULL;
	return (LPG_DLSM)g_pMemStrobeLamp->GetMemLink();
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

API_EXPORT LPG_CCGS uvEng_Conf_GetComn()
{
	return &GetConfig()->set_comn;
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
/*                                    외부 함수 - < Common >                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : UVDI15 Engine DI 초기화
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		e_mode	- [in]  엔진 동작 모드
						0 : None (Engine Monitoring)
						1 : CMPS (노광 제어 SW)
						2 : Step (광학계 단차 측정)
						3 : VDOF (Align Camera DOF (Depth of Focus)
						4 : Exam (Align Shutting Inspection or Measurement)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Init(HWND hWnd, ENG_ERVM e_mode)
{
	TCHAR tzLogs[MAX_PATH_LEN]	= {NULL};

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
	if (e_mode == ENG_ERVM::en_monitoring)	return FALSE;

	/* Log Library */
	//swprintf_s(tzLogs, MAX_PATH_LEN, L"%s\\logs", g_tzWorkDir);
	swprintf_s(tzLogs, MAX_PATH_LEN, L"%s\\logs\\DlgFile", g_tzWorkDir);
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
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15_LLS06)
	/* MC2 Service */
	if (g_pMemMC2 && !uvMC2_Init(GetConfig(), g_pMemMC2->GetMemMap()))
	{
		AfxMessageBox(L"Failed to initialize the [Motion Lib]", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15|| \
DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	/* MC2 Service */
	if (g_pMemMC2 && !uvMC2_Init(GetConfig(), g_pMemMC2->GetMemMap(), g_pMemMC2b->GetMemMap()))
	{
		AfxMessageBox(L"Failed to initialize the [Motion Lib]", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}


#endif

	/* Align Camera */
	if (!GetConfig()->set_uvdi15.use_vision_lib)
	{
		AfxMessageBox(L"Vision Library is not active.", MB_ICONINFORMATION);
	}
	else
	{
		if (g_pMemVisi && !uvBasler_Init(GetConfig(), g_pMemVisi->GetMemMap(),
			ENG_VLPK::en_vision_mil, g_tzWorkDir))
		{
			AfxMessageBox(L"Failed to initialize the [Basler Camera Lib]", MB_ICONSTOP | MB_TOPMOST);
			return FALSE;
		}
		else {
			uvBasler_initMarkROI(g_pMark->GetMarkROI());
		}
	}

	/* Gentec */
	if (g_pMemGentec && !uvGentec_Init(GetConfig(), g_pMemGentec->GetMemMap()))
	{
		AfxMessageBox(L"Failed to initialize the [PM100D Lib]", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}
	/* Mvenc */
	//BYTE byPort2 = 0;
	BYTE byPort2 = GetConfig()->trig_grab.port_no;
	if (g_pMemMvenc && !uvMvenc_Init(byPort2, GetConfig(), g_pMemMvenc->GetMemMap(),
		g_pMemMvenc->GetMemLink()))
	{
		AfxMessageBox(L"Failed to initialize the [Trigger Board(Mvenc) Lib]", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}

	/* philhmi */
	if (g_pMemPhilhmi && !uvPhilhmi_Init(hWnd, GetConfig(), g_pMemPhilhmi->GetMemMap(), g_pMemPhilhmi->GetMemLink()))
	{
		AfxMessageBox(L"Failed to initialize the [Philhmi Lib]", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}

	/* StrobeLamp */
	if (g_pMemStrobeLamp && !uvStrobeLamp_Init(hWnd, GetConfig(), g_pMemStrobeLamp->GetMemMap(), g_pMemStrobeLamp->GetMemLink()))
	{
		AfxMessageBox(L"Failed to initialize the [StrobeLamp Lib]", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}

	/* KeyenceLDS */
	uvKeyenceLDS_Init(GetConfig());	// 230925 mhbaek Add

#if 0	/* Not use the current time */
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
#endif
	/* for Main Thread */
	g_pMainThread	= new CMainThread();
	ASSERT(g_pMainThread);
	if (!g_pMainThread->CreateThread(0, NULL, NORMAL_THREAD_SPEED))
	{
		AfxMessageBox(L"Failed to creat the MainThread for Engine", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 엔진 초기화 되었다고 플래그 설정 */
	g_bEngineInited	= TRUE;
	terminated = FALSE;
	return TRUE;
}

/*
 desc : UVDI15 Engine DI 종료여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Terminated()
{
	return terminated;
}

/*
 desc : UVDI15 Engine DI 해제
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT VOID uvEng_Close()
{
	UINT8 i	= 0x00;

	/* 불필요한 파일 제거 */
	RemoveAllPath();
#ifndef _DEBUG
	/* 무조건 카메라의 LED 전원 끄기 */
	uvLuria_ReqSetLedLampLevel(0x01, 0);
#endif

	/* Main Thread */
	if (g_pMainThread)
	{
		if (g_pMainThread->IsBusy())	g_pMainThread->Stop();
		while (g_pMainThread->IsBusy())	Sleep(100);
		delete g_pMainThread;
	}

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
		if (g_pMemLuria)		g_pMemLuria->GetMemMap()->link.ResetData();
		if (g_pMemMC2)			g_pMemMC2->GetMemMap()->link.ResetData();
		if (g_pMemMC2b)			g_pMemMC2b->GetMemMap()->link.ResetData();
		//if (g_pMemTrig)		g_pMemTrig->GetMemLink()->ResetData();

		if (g_pMemMvenc)		g_pMemMvenc->GetMemLink()->ResetData();
		if (g_pMemPhilhmi)		g_pMemPhilhmi->GetMemLink()->ResetData();
		if (g_pMemStrobeLamp)	g_pMemStrobeLamp->GetMemLink()->ResetData();
		if (GetConfig())
		{
			for (; i<GetConfig()->set_cams.acam_count; i++)
			{
				if (g_pMemVisi)	g_pMemVisi->GetMemMap()->link[i]->ResetData();
			}
		}

		/* Engine 관련 라이브러리 해제 */
		uvMC2_Close();
		uvLuria_Close();
		uvBasler_Close();
		//uvTrig_Close();

		uvPhilhmi_Close();		// 230518 mhbaek Add
		uvStrobeLamp_Close();

		uvGentec_Close();
		uvMvenc_Close();
		uvKeyenceLDS_Close();	// 230925 mhbaek Add

		/* Log Library를 맨 마지막에 기술해야 됨 */
		uvLogs_Close();
	}
	/* Shared Memory 닫기 */
	ClosedSharedMemory();

	/* 작업 시간 리스트 객체 해제 */
	if (g_syncJobWorkTime.Enter())
	{
		g_lstJobWorkTime.RemoveAll();
		g_syncJobWorkTime.Leave();
	}
	/* 엔진 해제 */
	g_bEngineInited	= FALSE;
	
	terminated = true;
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

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15_LLS06)
	/* 기존 설정된 Engine Mode 값 임시 저장 */
	u8EngineMode = GetConfig()->set_comn.engine_mode;
	/* 환경 파일 객체 생성 */
	CConfComn* pConfComn = new CConfComn(GetConfig());
	CConfUvdi15* pConfUvdi15 = new CConfUvdi15(GetConfig());
	ASSERT(pConfComn && pConfUvdi15);
	/* 환경 파일 내용 적재 */
	if (!pConfComn->LoadConfig())	return FALSE;
	if (!pConfUvdi15->LoadConfig())	return FALSE;
	/* !!! 중요 !!! Engine 동작 모드 값 강제 설정 */
	pConfComn->GetConfig()->set_comn.engine_mode = u8EngineMode;
	/* 메모리 해제 */
	delete pConfComn;
	delete pConfUvdi15;
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15|| \
DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	/* 기존 설정된 Engine Mode 값 임시 저장 */
	u8EngineMode = GetConfig()->set_comn.engine_mode;
	/* 환경 파일 객체 생성 */
	CConfComn* pConfComn = new CConfComn(GetConfig());
	CConfUvdi15* pConfUvDI15 = new CConfUvdi15(GetConfig());
	ASSERT(pConfComn && pConfUvDI15);
	/* 환경 파일 내용 적재 */
	if (!pConfComn->LoadConfig())	return FALSE;
	if (!pConfUvDI15->LoadConfig())	return FALSE;
	/* !!! 중요 !!! Engine 동작 모드 값 강제 설정 */
	pConfComn->GetConfig()->set_comn.engine_mode = u8EngineMode;
	/* 메모리 해제 */
	delete pConfComn;
	delete pConfUvDI15;
#endif

	return TRUE;
}

/*
 desc : 데이터 파일 다시 적재
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_ReloadCaliFile()
{
	return ReloadCaliFile();
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

API_EXPORT BOOL uvEng_SetRecipeConfig(PTCHAR name)
{
	INT32 i32Len = (INT32)wcslen(name);

	if (i32Len >= MAX_FILE_LEN)	i32Len = MAX_FILE_LEN - 1;
	wcscpy_s(GetConfig()->set_uvdi15.recipe_name, MAX_FILE_LEN, name);

	return TRUE;
}

API_EXPORT PTCHAR uvEng_GetRecipeConfig()
{
	return GetConfig()->set_uvdi15.recipe_name;
}

/*
 desc : 환경 파일 저장
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_SaveConfig()
{
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15_LLS06)
	BOOL bSucc = FALSE;
	/* 환경 파일 객체 생성 */
	CConfComn* pConfComn = new CConfComn(GetConfig());
	CConfUvdi15* pConfUvdi15 = new CConfUvdi15(GetConfig());
	ASSERT(pConfComn && pConfUvdi15);
	/* 환경 파일 저장 */
	bSucc = pConfComn->SaveConfig();
	bSucc = pConfUvdi15->SaveConfig();
	/* 메모리 해제 */
	delete pConfComn;
	delete pConfUvdi15;
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15|| \
DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	BOOL bSucc = FALSE;
	/* 환경 파일 객체 생성 */
	CConfComn* pConfComn = new CConfComn(GetConfig());
	CConfUvdi15* pConfUvDI15 = new CConfUvdi15(GetConfig());
	ASSERT(pConfComn && pConfUvDI15);
	/* 환경 파일 저장 */
	bSucc = pConfComn->SaveConfig();
	bSucc = pConfUvDI15->SaveConfig();
	/* 메모리 해제 */
	delete pConfComn;
	delete pConfUvDI15;
#endif

	return TRUE;
}

/*
 desc : 모든 장비가 연결되어 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_IsConnectedAll()
{
	return IsConnectedAll();
}
/*
 desc : PLC, Motion, Luria 및 기타 모듈로부터 DI 장비에 문제 (알람)가 발생했는지 여부 반환
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_IsEngineAlarm()
{
	UINT8 i, u8PhCount	= GetConfig()->luria_svc.ph_count;
	UINT16 u16Value		= 0x0000;

	/* DEMO Mode */
	if (GetConfig()->IsRunDemo())	return FALSE;
	/* 모든 장치가 연결되어 있는지 여부 */
	if (!IsConnectedAll())
	{
//		LOG_ERROR(ENG_EDIC::en_uvdi15, L"It is not connected all");
		return TRUE;
	}
	/* DI 장비에서 발생되는 모든 알람을 무시하는지 여부 */
	if (GetConfig()->set_comn.all_alarm_ignore)	return FALSE;
	/* PLC로부터 알람이 발생 했는지 여부 */
	for (i=0x00; i<3; i++)
	{
#ifndef _DEBUG
		/* 광학계 사용되는 개수에 따라 광학계 Power 알람이 발생할 수 있으므로... 일단 제외 */
		// by sysandj : MCQ대체 추가 필요
		//pstPLC->r_photohead_power_line	= 0x01;	/* 강제로 5개 광학계 모두 ON 되었다고 처리 */
		//pstPLC->r_auto_maint_mode		= 0x01;	/* 강제로 Auto Mode로 설정 */
		/* PLC 내부에 치명적인 알람 Word 영역(Chiller, Hot Air, Vacuum 전원 인가 여부 포함) */
		// by sysandj : MCQ대체 추가 필요
// 		if (g_pMemPLC->GetWordValue(UINT16(i), u16Value))
// 		{
// 			if (!u16Value)
// 			{
// 				LOG_ERROR(ENG_EDIC::en_uvdi15, L"PLC Error");
// 				return TRUE;	/* 알람 발생 */
// 			}
// 		}
#endif
	}
#ifndef _DEBUG
	/* Vacuum 전원 인가 여부, Chiller 전원 인가 여부*/
	// by sysandj : MCQ대체 추가 필요
	//if (!pstPLC->r_vacuum_controller_running)	return TRUE;
	//if (!pstPLC->r_chiller_controller_running)	return TRUE;
#endif
	/* MC2 (Motion)로부터 알람이 있는지 여부 */
	if (uvMC2_IsAnyDriveError())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"MC2 IsAnyDriveError");
		return TRUE;
	}
	/* Luria PH 초기화 혹은 모터 에러가 있는지 여부 */
	if (!((LPG_LDSM)g_pMemLuria->GetMemMap())->directph.IsAllMotorMiddleInited(u8PhCount))
	{
//		LOG_ERROR(ENG_EDIC::en_uvdi15, L"IsAllMotorMiddleInited");
		return TRUE;
	}
	/* Luria System에서 에러가 발생 했는지 여부 */
	if (((LPG_LDSM)g_pMemLuria->GetMemMap())->system.IsErrorAll())
	{
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128, L"PH.System.Error [%d][%d][%d]",
				   g_pMemLuria->GetMemMap()->system.get_system_status[0],
				   g_pMemLuria->GetMemMap()->system.get_system_status[1],
				   g_pMemLuria->GetMemMap()->system.get_system_status[2]);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
		return TRUE;
	}

	return FALSE;	/* 알람 없음 */
}

/*
 desc : PLC, Motion, Luria 및 기타 모듈로부터 DI 장비에 문제 (경고)가 발생했는지 여부 반환
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_IsEngineWarn()
{
	/* DEMO Mode */
	if (GetConfig()->IsRunDemo())	return FALSE;
	/* DI 장비에서 발생되는 모든 알람을 무시하는지 여부 */
	if (GetConfig()->set_comn.all_warn_ignore)	return FALSE;

	return FALSE;
}

/*
 desc : 작업 이름 설정
 parm : name	- [in]  작업 (STEP 포함) 이름
 retn : None
*/
API_EXPORT VOID uvEng_SetWorkOptionalText(PTCHAR text)
{

	if (text == nullptr)
	{
		optionalWorkstepText[0] = 0x00;
		return;
	}

	INT32 i32Len	= (INT32)wcslen(text);
	
	if (i32Len >= WORK_NAME_LEN)	i32Len = WORK_NAME_LEN-1;
	wcscpy_s(optionalWorkstepText, WORK_NAME_LEN, text);
	optionalWorkstepText[i32Len]	= 0x00;
}

API_EXPORT PTCHAR uvEng_GetWorkOptionalText()
{
	return  optionalWorkstepText;
}

API_EXPORT VOID uvEng_SetWorkStepName(PTCHAR name)
{

	if (name == nullptr)
	{
		g_ptzWorkStepName[0] = 0x00;
		return;
	}

	INT32 i32Len = (INT32)wcslen(name);

	if (i32Len >= WORK_NAME_LEN)	i32Len = WORK_NAME_LEN - 1;
	wcscpy_s(g_ptzWorkStepName, WORK_NAME_LEN, name);
	g_ptzWorkStepName[i32Len] = 0x00;
}


/*
 desc : 현재 작업 이름 반환
 parm : None
 retn : 작업 이름 (STEP 포함) 반환
*/
API_EXPORT PTCHAR uvEng_GetWorkStepName()
{
	return g_ptzWorkStepName;
}

/*
 desc : 전체 작업 진행률 설정 및 반환
 parm : rate	- [in]  작업 진행률 (unit: percent;%)
 retn : None or 진행률 값 (단위: %)
*/
API_EXPORT VOID uvEng_SetWorkStepRate(DOUBLE rate)	{	g_dbWorkStepRate	= rate;	}
API_EXPORT DOUBLE uvEng_GetWorkStepRate()			{	return g_dbWorkStepRate;	}

/*
 desc : 현재 작업 동작 중인데 소요되고 있는 시간 설정 및 반환 그리고 평균 작업 시간 반환
 parm : w_time	- [in]  현재 작업하는데 소요되고 있는 시간 (완료 시간이기도 함) (단위: msec)
		e_time	- [in]  작업이 완료되는 시점의 소요된 시간
 retn : None or 작업 소요 시간 (단위:  msec)
*/
API_EXPORT VOID uvEng_SetJobWorkInfo(UINT8 job_id, UINT64 e_time)
{
	/* 동기 진입 */
	if (g_syncJobWorkTime.Enter())
	{
		/* 이전 작업 ID와 다르면 무조건 기존 작업 결과는 초기화 */
		if (g_u8WorkJobID != job_id)
		{
			/* 가장 최근 Job ID 저장 */
			g_u8WorkJobID	= job_id;
			/* 작업 정보 초기화 */
			ResetJobWorkInfo(FALSE);
		}

		/* 새로 발생된 작업 정보 설정*/
		g_u32JobCount++;
		g_u64JobWorkTime= e_time;

		/* 현재 작업 시간 추가 및 최대 32개 이상 등록되어 있으면 가장 오래된 작업 시간 제거 */
		g_lstJobWorkTime.AddTail(e_time);
		if (g_lstJobWorkTime.GetCount() > 32)	g_lstJobWorkTime.RemoveHead();

		/* 동기 해제 */
		g_syncJobWorkTime.Leave();
	}
}
API_EXPORT VOID uvEng_UpdateJobWorkTime(UINT64 w_time)
{
	g_u64JobWorkTime	= w_time;
}
API_EXPORT UINT64 uvEng_GetJobWorkTime()
{
	return g_u64JobWorkTime;
}
API_EXPORT UINT64 uvEng_GetJobWorkTimeAverage()
{
	UINT64 u64Avgs	= 0;

	if (g_syncJobWorkTime.Enter())
	{
		INT8 i	= 0, i8Count = (UINT8)g_lstJobWorkTime.GetCount();
		for (i=0; i<i8Count; i++)
		{
			u64Avgs += g_lstJobWorkTime.GetAt(g_lstJobWorkTime.FindIndex(i));
		}
		if (i8Count)	u64Avgs	= (UINT64)ROUNDED(u64Avgs/UINT64(i8Count), 0);
		g_syncJobWorkTime.Leave();
	}

	return u64Avgs;
}
/*
 desc : 동일한 작업의 반복 횟수 값 반환
 parm : None
 retn : None or 동일 작업 개수
*/
API_EXPORT UINT32 uvEng_GetJobWorkCount()
{
	return g_u32JobCount;
}

/*
 desc : 각종 작업 관련 시간 및 통계 정보 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvEng_ResetJobWorkInfo()
{
	/* 동기 진입 */
	if (g_syncJobWorkTime.Enter())
	{
		/* 작업 정보 초기화 */
		ResetJobWorkInfo(TRUE);
		/* 동기 해제 */
		g_syncJobWorkTime.Leave();
	}
}

/* --------------------------------------------------------------------------------------------- */
/*                              외부 함수 - < Trigger for UVDI15 >                                */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Trigger가 발생될 Motion Position 값 등록
 parm : mark	- [in]  Mark Type (Global or Local)
		cam_id	- [in]  Align Camera Index (1 or 2)
		index	- [in]  Array Index (Zero-based)
		trig	- [in]  Trigger Position (Unit : 100 nm or 0.1 um)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Trig_SetTrigger(ENG_AMTF mark, UINT8 cam_id, UINT8 index, INT32 trig)
{
	if (cam_id < 1 || cam_id > GetConfig()->set_cams.acam_count)	return FALSE;

	LPG_CMTP pstMarkTrig = &g_pMemConf->GetMemMap()->mark_trig;
	if (ENG_AMTF::en_global == mark)	pstMarkTrig->global[cam_id-1][index]= trig;
	else								pstMarkTrig->local[cam_id-1][index]	= trig;

	return TRUE;
}

/*
 desc : Trigger가 발생될 Motion Position 값 반환
 parm : mark	- [in]  Mark Type (Global or Local)
		cam_id	- [in]  Align Camera Index (1 or 2)
 retn : 배열 포인터 반환
*/
API_EXPORT PINT32 uvEng_Trig_GetTrigger(ENG_AMTF mark, UINT8 cam_id)
{
	if (cam_id < 1 || cam_id > GetConfig()->set_cams.acam_count)	return FALSE;

	LPG_CMTP pstMarkTrig = &g_pMemConf->GetMemMap()->mark_trig;
	if (ENG_AMTF::en_global == mark)	return pstMarkTrig->global[cam_id-1];
	else								
		return pstMarkTrig->local[cam_id-1];
}

/* --------------------------------------------------------------------------------------------- */
/*                                 외부 함수 - < PLC for UVDI15 >                                 */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : DI 내부의 현재 온도 반환
 parm : index	- [in]  0x00 ~ 0x03 (TEMP Sensor가 설치된 4곳 중 1 곳)
 retn : 온도 값 반환
*/
API_EXPORT DOUBLE uvEng_PLC_GetTempDI(UINT8 index)
{
	return g_pMainThread->GetLastTempDI(index);
}

/*
 desc : DI 내부의 평균 온도 반환
 parm : index	- [in]  0x00 ~ 0x03 (TEMP Sensor가 설치된 4곳 중 1 곳)
 retn : 온도 값 반환
*/
API_EXPORT DOUBLE uvEng_PLC_GetTempDIAvg(UINT8 index)
{
	return g_pMainThread->GetTempDIAvg(index);
}

/*
 desc : DI 내부의 최소 온도 반환
 parm : index	- [in]  0x00 ~ 0x03 (TEMP Sensor가 설치된 4곳 중 1 곳)
 retn : 온도 값 반환
*/
API_EXPORT DOUBLE uvEng_PLC_GetTempDIMin(UINT8 index)
{
	return g_pMainThread->GetTempDIMin(index);
}

/*
 desc : DI 내부의 최대 온도 반환
 parm : index	- [in]  0x00 ~ 0x03 (TEMP Sensor가 설치된 4곳 중 1 곳)
 retn : 온도 값 반환
*/
API_EXPORT DOUBLE uvEng_PLC_GetTempDIMax(UINT8 index)
{
	return g_pMainThread->GetTempDIMax(index);
}


#ifdef __cplusplus
}
#endif