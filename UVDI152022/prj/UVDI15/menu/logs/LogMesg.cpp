/*
 설명 : Logs Message Control
 */

#include "pch.h"
#include "../../MainApp.h"
#include "LogMesg.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 설명 : 생성자
 변수 : None
 반환 : None
*/
CLogMesg::CLogMesg()
{
}

/*
 설명 : 파괴자
 변수 : None
 반환 : None
*/
CLogMesg::~CLogMesg()
{
}

/*
 설명 : 에러 정보 갱신
 변수 : file	- [in]  로그 파일
		type	- [in]  Log Level (Error Level)
 반환 : None
*/
VOID CLogMesg::LoadFile(PTCHAR file, ENG_LNWE type)
{
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL}, tzFilt[32]	= {NULL};
	PTCHAR ptzLogs, ptzMesg		= NULL;
	PTCHAR ptzPrev, ptzNext		= NULL;
	INT32 i32Read = 0, i32Line	= 512, i32Size;
	UINT32 u32Size				= 0;
	COLORREF clrText[3]			= { RGB(0, 0, 0), RGB(0, 0, 255), RGB(255, 0, 0) };
	errno_t errFile				= 0;
	FILE *fpLog					= NULL;

	/* 현재 선택된 파일 얻기 (전체 경로 포함) */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\logs\\%s", g_tzWorkDir, file);
	/* 현재 열고자 하는 로그 파일의 크기 얻기 */
	u32Size	= uvCmn_GetFileSize(tzFile);
	/* 임시 저장할 버퍼 할당 */
	ptzLogs = new TCHAR[u32Size + 1];// (PTCHAR)::Alloc(u32Size + 1);
	memset(ptzLogs, 0x00, u32Size+1);	/* wmemset 함수 호출하면 안됨. 데이터 크기는 무조건 Bytes 단위이므로 */
	/* 임시 저장할 버퍼 할당 */
	ptzMesg = new TCHAR[i32Line];// (PTCHAR)::Alloc(sizeof(TCHAR) * i32Line);

	/* 로그 파일 열기 */
	errFile = _wfopen_s(&fpLog, tzFile, L"r, ccs=UTF-16LE");
	if (0 != errFile)	return;
	i32Read	= (INT32)fread_s(ptzLogs, u32Size, 1, u32Size, fpLog);
	/* 로그 파일 닫기 */
	fclose(fpLog);

	/* 필터 문자열 설정 */
	swprintf_s(tzFilt, 32, L"<0x%02x>", type);

	/* 모두 읽어들인 경우만 해당 됨 */
	if (i32Read > 0 && UINT32(i32Read) <= u32Size)
	{
		/* 기존 출력된 데이터 모두 지우기 */
		SetClearAll(TRUE);
		Invalidate(FALSE);

		/* 갱신 비활성화 */
		SetRedraw(FALSE);

		/* 검색 대상의 문자열 시작 위치 임시 저장 */
		ptzPrev	= ptzLogs;

		do	{

			/* Carrage Return 위치 검색 */
			ptzNext	= wcschr(ptzPrev, L'→');
			if (!ptzNext)	break;

			/* 검색한 문자포함하여, 이전 문자열까지 모두 복사 */
			i32Size	= INT32(ptzNext-ptzPrev-1);
			wmemset(ptzMesg, 0x00, i32Line);
			wmemcpy(ptzMesg, ptzPrev, i32Size);
			ptzMesg[i32Size] = L'\n';
			/* 문자열 검색 */
			if (ENG_LNWE::en_all == type)		Append(ptzMesg, RGB(0, 0, 0), FALSE);
			else
			{
				/* Filter 적용된 경우 */
				if (wcsstr(ptzMesg, tzFilt))	Append(ptzMesg, RGB(0, 0, 0), FALSE);
			}
			/* 다음 검색을 위해서 검색 시작 위치 변경 */
#if 0
			ptzPrev	= ++ptzNext;
#else
			ptzPrev	= wcschr(ptzNext, L'\n');
			if (!ptzPrev)	break;
			ptzPrev++;
#endif
		} while (1);

		/* 갱신 활성화 */
		SetRedraw(TRUE);
		Invalidate(FALSE);
	}
	/* 메모리 해제 */
	delete ptzMesg;
	delete ptzLogs;
}