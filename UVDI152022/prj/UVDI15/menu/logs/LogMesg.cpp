/*
 ���� : Logs Message Control
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
 ���� : ������
 ���� : None
 ��ȯ : None
*/
CLogMesg::CLogMesg()
{
}

/*
 ���� : �ı���
 ���� : None
 ��ȯ : None
*/
CLogMesg::~CLogMesg()
{
}

/*
 ���� : ���� ���� ����
 ���� : file	- [in]  �α� ����
		type	- [in]  Log Level (Error Level)
 ��ȯ : None
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

	/* ���� ���õ� ���� ��� (��ü ��� ����) */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\logs\\%s", g_tzWorkDir, file);
	/* ���� ������ �ϴ� �α� ������ ũ�� ��� */
	u32Size	= uvCmn_GetFileSize(tzFile);
	/* �ӽ� ������ ���� �Ҵ� */
	ptzLogs = new TCHAR[u32Size + 1];// (PTCHAR)::Alloc(u32Size + 1);
	memset(ptzLogs, 0x00, u32Size+1);	/* wmemset �Լ� ȣ���ϸ� �ȵ�. ������ ũ��� ������ Bytes �����̹Ƿ� */
	/* �ӽ� ������ ���� �Ҵ� */
	ptzMesg = new TCHAR[i32Line];// (PTCHAR)::Alloc(sizeof(TCHAR) * i32Line);

	/* �α� ���� ���� */
	errFile = _wfopen_s(&fpLog, tzFile, L"r, ccs=UTF-16LE");
	if (0 != errFile)	return;
	i32Read	= (INT32)fread_s(ptzLogs, u32Size, 1, u32Size, fpLog);
	/* �α� ���� �ݱ� */
	fclose(fpLog);

	/* ���� ���ڿ� ���� */
	swprintf_s(tzFilt, 32, L"<0x%02x>", type);

	/* ��� �о���� ��츸 �ش� �� */
	if (i32Read > 0 && UINT32(i32Read) <= u32Size)
	{
		/* ���� ��µ� ������ ��� ����� */
		SetClearAll(TRUE);
		Invalidate(FALSE);

		/* ���� ��Ȱ��ȭ */
		SetRedraw(FALSE);

		/* �˻� ����� ���ڿ� ���� ��ġ �ӽ� ���� */
		ptzPrev	= ptzLogs;

		do	{

			/* Carrage Return ��ġ �˻� */
			ptzNext	= wcschr(ptzPrev, L'��');
			if (!ptzNext)	break;

			/* �˻��� ���������Ͽ�, ���� ���ڿ����� ��� ���� */
			i32Size	= INT32(ptzNext-ptzPrev-1);
			wmemset(ptzMesg, 0x00, i32Line);
			wmemcpy(ptzMesg, ptzPrev, i32Size);
			ptzMesg[i32Size] = L'\n';
			/* ���ڿ� �˻� */
			if (ENG_LNWE::en_all == type)		Append(ptzMesg, RGB(0, 0, 0), FALSE);
			else
			{
				/* Filter ����� ��� */
				if (wcsstr(ptzMesg, tzFilt))	Append(ptzMesg, RGB(0, 0, 0), FALSE);
			}
			/* ���� �˻��� ���ؼ� �˻� ���� ��ġ ���� */
#if 0
			ptzPrev	= ++ptzNext;
#else
			ptzPrev	= wcschr(ptzNext, L'\n');
			if (!ptzPrev)	break;
			ptzPrev++;
#endif
		} while (1);

		/* ���� Ȱ��ȭ */
		SetRedraw(TRUE);
		Invalidate(FALSE);
	}
	/* �޸� ���� */
	delete ptzMesg;
	delete ptzLogs;
}