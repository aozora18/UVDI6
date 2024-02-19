
/*
 desc : Melsec Q PLC Communication Libarary (MC Protocol)
*/

#include "pch.h"
#include "MainApp.h"
#include "GFSCThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                           전역 변수                                           */
/* --------------------------------------------------------------------------------------------- */

UINT64						g_u64LastWriteTime	= 0;		/* 가장 최근에 송신한 시간 저장 */

CGFSCThread					*g_pGFSCThread		= NULL;	

/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */





/* --------------------------------------------------------------------------------------------- */
/*                                           외부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/*
 desc : 통신을 담당하는 Client 스레드 생성 및 실행
 parm : config	- [in]  환경 설정 정보가 저장된 구조체 포인터
		speed	- [in]  스레드 동작 속도 (단위: 밀리초)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSC_Init(LPG_CSID config, UINT32 speed)
{
	if (speed < 10)	speed = 10;
	// Client 스레드 생성 및 동작 시킴
	g_pGFSCThread = new CGFSCThread(0, config);
	if (!g_pGFSCThread)	return FALSE;
	if (!g_pGFSCThread->CreateThread(0, 0, speed))
	{
		delete g_pGFSCThread;
		g_pGFSCThread	= NULL;
		return FALSE;
	}

	return TRUE;
}

/*
 desc : 라이브러리 해제
 parm : None
 retn : None
*/
API_EXPORT VOID uvGFSC_Close()
{
	// 스레드 정지 시키는 명령 전달 후에 정지될 때까지 대기
	if (g_pGFSCThread)
	{
		// 서버와의 연결을 강제로 해제
		g_pGFSCThread->CloseClient();
		// 스레드 중지 호출
		g_pGFSCThread->Stop();
		while (g_pGFSCThread->IsBusy())	g_pGFSCThread->Sleep(100);
		/* 콜백 리셋 함수 호출 후 메모리 해제 */
		delete g_pGFSCThread;
		g_pGFSCThread	= NULL;
	}
}

/*
 desc : 현재 통신이 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSC_IsConnected()
{
	if (!g_pGFSCThread)	return FALSE;
	if (!g_pGFSCThread->IsBusy())	return FALSE;

	return g_pGFSCThread->IsConnected();
}

/*
 desc : 현재 TCP/IP 통신 상태 모드 값 얻기
 parm : None
 retn : ENG_TCPS
*/
API_EXPORT ENG_TPCS uvGFSC_GetTCPMode()
{
	return g_pGFSCThread->GetTCPMode();
}

/*
 desc : 현재 명령어를 보내도 되는지 여부 즉, 가장 최근에 보낸 명령어와 현재 보낼 명령어 간의 시간 간격이
		주어진 시간보다 작다면, FALSE, 크다면 TRUE 반환
 parm : period	- [in]  간격 (이 시간보다 크면, 송신 가능, 작다면 송신 불가능) (단위: msec)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSC_IsSendTimeValid(UINT64 period)
{
	return (GetTickCount64() > (g_u64LastWriteTime + period));
}

/*
 desc : 새로 생성된 거버 폴더가 유효한지 여부 검사
 parm : g_path	- [in]  거버 폴더로서 유효한지 검사하기 위한 Gerber Path (전체 경로 포함)
		sub_path- [out] 하위 경로 존재 여부 값 반환 참조 변수
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSC_IsValidGerberPath(PTCHAR g_path, BOOL &sub_path)
{
	INT32 i, i32BasePathLen, i32PathLen;
	TCHAR tzSubPath[MAX_PATH_LEN]	= {NULL};
	PTCHAR ptzPathName;

	CStringArray arrSubPaths;
	CStringArray arrSubFiles;

	/* 기준 경로 길이 임시 저장 */
	i32BasePathLen	= (INT32)wcslen(g_path);
	/* 폴더 내에 거버 파일 목록 송신 */
	uvCmn_SearchRecursePath(g_path, arrSubPaths);

	/* 하위 경로가 없는 경우 */
	if (arrSubPaths.GetCount() == 0)
	{
		/* 하위 경로 없음 */
		sub_path	= FALSE;
		/* 하위에 파일이 있는 경우인지 */
		uvCmn_SearchFile(g_path, L"*.rle", arrSubFiles);
		if (arrSubFiles.GetCount() < 1)
		{
			arrSubFiles.RemoveAll();
			return FALSE;
		}

		/* 검색 경로의 가장 마지막 폴더명만 얻기 */
		ptzPathName	= wcsrchr(g_path, L'\\') + 1;	/*ptzSubName++;*/
		i32PathLen	= (INT32)wcslen(ptzPathName);
		if (i32PathLen < 1)
		{
			arrSubFiles.RemoveAll();
			return FALSE;
		}

		/* 검색하려는 경로명 모두 소문자로 변경 */
		_wcslwr_s(ptzPathName, i32PathLen+1);	/* !!! +1 해주는게 중요 !!! */
		/* 하위에 파일 이름이 상위 폴더 이름 기반으로 만들어 졌는지 여부 */
		for (i=0; i<arrSubFiles.GetCount(); i++)
		{
			/* 무조건 소문자로 비교 */
			if (arrSubFiles[i].MakeLower().Find(ptzPathName) < 0)
			{
				arrSubFiles.RemoveAll();
				return FALSE;
			}
		}
		/* 메모리 해제 */
		arrSubFiles.RemoveAll();
	}
	/* 하위 경로가 존재하는 경우 */
	else
	{
		/* 하위 경로 존재 */
		sub_path	= TRUE;
		for (i=0; i<arrSubPaths.GetCount(); i++)
		{
			ptzPathName	= arrSubPaths[i].GetBuffer();
			/* 검색 기준 경로를 제외한 나머지 경로만 임시로 저장 */
			wmemset(tzSubPath, 0x00, MAX_PATH_LEN);
			wmemcpy(tzSubPath, ptzPathName+i32BasePathLen, wcslen(ptzPathName)-i32BasePathLen);
			/* 기준 검색 경로 대비 하위 경로가 2개 이상이면 에러 */
			if (uvCmn_GetFindChCount(tzSubPath,
									 UINT32(wcslen(ptzPathName)-i32BasePathLen),
									 L'\\') > 1)	return FALSE;
		}
	}

	return TRUE;
}

/*
 desc : 가장 최근에 발생된 에러가 있는지 여부
 parm : None
 retn : TRUE or FALSE (에러 없음)
*/
API_EXPORT BOOL uvGFSC_IsLastError()
{
	if (!g_pGFSCThread->IsConnected())	return FALSE;

	return g_pGFSCThread->GetLastErrorLevel() == ENG_LNWE::en_error;
}

/*
 desc : 현재까지 거버 파일의 송신 진행률 값 반환
 parm : None
 retn : 진행률 값
*/
API_EXPORT DOUBLE uvGFSC_GetSendGFilesRate()
{
	if (!g_pGFSCThread->IsConnected())	0.0f;

	return g_pGFSCThread->GetSendGFilesRate();
}

/*
 desc : Client로부터 수신된 패킷 명령어 부분 반환
 parm : None
 retn : 현재 수신된 명령어들 중 가장 먼저 도착했던 명령어 반환
		수신된 명령어가 더 이상 없다면 0x0000 값 반환
*/
API_EXPORT ENG_PCGG uvGFSC_PopPktHeadCmd()
{
	STG_UTPH stHead	= {NULL};

	if (!g_pGFSCThread->IsConnected())		return ENG_PCGG::en_pcgg_unknown;
	if (!g_pGFSCThread->PopPktHead(stHead))	return ENG_PCGG::en_pcgg_unknown;

	return ENG_PCGG(stHead.cmd);
}

/*
 desc : 현재 거버 파일 송신 중이면 중지 처리
 parm : None
 retn : None
*/
API_EXPORT VOID uvGFSC_StopGerberSendFile()
{
	if (g_pGFSCThread->IsConnected())	g_pGFSCThread->StopGerberSendFile();
}

/*
 desc : 현재까지 송신된 파일 크기 반환
 parm : None
 retn : 파일 크기 (단위: Bytes)
*/
API_EXPORT UINT32 uvGFSC_GetTotalFileSize()
{
	if (!g_pGFSCThread->IsConnected())	return 0;
	return g_pGFSCThread->GetTotalFileSize();
}

/* --------------------------------------------------------------------------------------------- */
/*                                 Packet Data Communication Part                                */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 서버에 Alive Check 명령 전달
 parm : logs	- [in]  로그 파일 저장 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSC_PktSendAliveTime()
{
	if (!g_pGFSCThread->IsConnected())	return FALSE;
	return g_pGFSCThread->SetAliveTime();
}

/*
 desc : 서버에 Alive Check 수신 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSC_IsRecvAliveTime()
{
	if (!g_pGFSCThread->IsConnected())	return FALSE;
	return g_pGFSCThread->IsRecvAliveTime();
}

/*
 desc : 거버 파일 경로 명 생성 명령 전달
 parm : g_name	- [in]  새로 생성될 거버 명 (전체 경로는 미포함. 기본 경로 하위에 생성될 거버 폴더 명)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSC_PktSendNewGerberPath(PTCHAR g_name)
{
	if (!g_pGFSCThread->IsConnected())	return FALSE;
	return g_pGFSCThread->SetNewGerberPath(g_name);
}

/*
 desc : 거버 파일 경로 명 삭제 명령 전달
 parm : g_name	- [in]  삭제될 거버 명 (전체 경로는 미포함. 기본 경로 하위의 삭제될 거버 폴더 명)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSC_PktSendDelGerberPath(PTCHAR g_name)
{
	if (!g_pGFSCThread->IsConnected())	return FALSE;
	return g_pGFSCThread->SetDelGerberPath(g_name);
}

/*
 desc : 거버 파일들을 Server에 전달
 parm : files	- [in]  파일 정보가 저장된 배열 리스트 버퍼 포인터
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSC_PktSendGerberFiles(CStringArray *files)
{
	UINT32 i	= 0, u32BasePathLen, j = 0;
	CHAR  szLastPath[MAX_PATH_LEN]	= {NULL};
	TCHAR tzSubPath[MAX_PATH_LEN]	= {NULL};
	PTCHAR ptzGPath		= NULL, ptzGFile;
	STG_PGFP stProp		= {NULL};
	LPG_CSID pstConfig	= uvGFSP_Info_GetData();
	CString strGFile;
	CUniToChar csCnv;

	/* 연결이 된 상태인지 확인 */
	if (!g_pGFSCThread->IsConnected())	return FALSE;

	/* 기존 임시 송신 버퍼 초기화  */
	g_pGFSCThread->ResetPktGfiles();
	/* 기준 폴더의 길이 */
	u32BasePathLen	= (UINT32)wcslen(pstConfig->gerber_path->gfsc);
	/* 파일 리스트에서 파일과 폴더 구분하여 추출 */
	if (files->GetCount() < 1)	return FALSE;
	for (; i<(UINT32)files->GetCount(); i++, j++)
	{
		wmemset(tzSubPath, 0x00, MAX_PATH_LEN);
		memset(&stProp, 0x00, sizeof(STG_PGFP));

		/* 기준 폴더 제외한 나머지 폴더 추출 */
		strGFile= files->GetAt(i);
		ptzGPath= strGFile.GetBuffer() + 1 + u32BasePathLen;
		/* 폴더에서 파일 분리 */
		ptzGFile= wcsrchr(ptzGPath, L'\\') + 1;	/* 파일 이름이 시작되는 위치 얻기 */
		if (!ptzGFile)	return FALSE;
		/* 폴더 이름 저장 */
		memcpy_s(stProp.path, GERBER_PATH_NAME, csCnv.Uni2Ansi(ptzGPath), wcslen(ptzGPath) - (wcslen(ptzGFile) + 1));
		/* 파일 이름 저장 */
		memcpy_s(stProp.file, GERBER_FILE_NAME, csCnv.Uni2Ansi(ptzGFile), wcslen(ptzGFile));
		/* 거버 파일 크기 */
		stProp.s_size	= uvCmn_GetFileSize(strGFile.GetBuffer());
		/* 가장 최근의 저장된 경로와 현재 파일의 경로가 다르면, 교환 */
		if (0 != strcmp(szLastPath, stProp.path))
		{
			j	= 0;
			/* 가장 최근의 파일 경로 저장 */
			memset(szLastPath, 0x00, MAX_PATH_LEN);
			memcpy(szLastPath, stProp.path, strlen(stProp.path));
		}
		/* 현재 거버 폴더 기준으로 파일 번호 */
		stProp.file_no	= j;

		/* 임시 메모리 버퍼에 등록 */
		g_pGFSCThread->AddPktGFiles(stProp);
	}

	/* Packet 송신 진행 */
	g_pGFSCThread->SetSetupGFile();

	return TRUE;
}

/*
 desc : 서버에게 Recipe Files 요청
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSC_PktSendGetRecipeFile()
{
	if (!g_pGFSCThread->IsConnected())	return FALSE;
	return g_pGFSCThread->GetRecipeFiles();
}

/*
 desc : 서버에게 Recipe Files 전달
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSC_PktSendSetRecipeFile()
{
	if (!g_pGFSCThread->IsConnected())	return FALSE;
	return g_pGFSCThread->SetRecipeFiles();
}


#ifdef __cplusplus
}
#endif