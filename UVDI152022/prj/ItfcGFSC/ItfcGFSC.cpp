
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
/*                                           ���� ����                                           */
/* --------------------------------------------------------------------------------------------- */

UINT64						g_u64LastWriteTime	= 0;		/* ���� �ֱٿ� �۽��� �ð� ���� */

CGFSCThread					*g_pGFSCThread		= NULL;	

/* --------------------------------------------------------------------------------------------- */
/*                                           ���� �Լ�                                           */
/* --------------------------------------------------------------------------------------------- */





/* --------------------------------------------------------------------------------------------- */
/*                                           �ܺ� �Լ�                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/*
 desc : ����� ����ϴ� Client ������ ���� �� ����
 parm : config	- [in]  ȯ�� ���� ������ ����� ����ü ������
		speed	- [in]  ������ ���� �ӵ� (����: �и���)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSC_Init(LPG_CSID config, UINT32 speed)
{
	if (speed < 10)	speed = 10;
	// Client ������ ���� �� ���� ��Ŵ
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
 desc : ���̺귯�� ����
 parm : None
 retn : None
*/
API_EXPORT VOID uvGFSC_Close()
{
	// ������ ���� ��Ű�� ��� ���� �Ŀ� ������ ������ ���
	if (g_pGFSCThread)
	{
		// �������� ������ ������ ����
		g_pGFSCThread->CloseClient();
		// ������ ���� ȣ��
		g_pGFSCThread->Stop();
		while (g_pGFSCThread->IsBusy())	g_pGFSCThread->Sleep(100);
		/* �ݹ� ���� �Լ� ȣ�� �� �޸� ���� */
		delete g_pGFSCThread;
		g_pGFSCThread	= NULL;
	}
}

/*
 desc : ���� ����� ����Ǿ� �ִ��� ���� Ȯ��
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
 desc : ���� TCP/IP ��� ���� ��� �� ���
 parm : None
 retn : ENG_TCPS
*/
API_EXPORT ENG_TPCS uvGFSC_GetTCPMode()
{
	return g_pGFSCThread->GetTCPMode();
}

/*
 desc : ���� ��ɾ ������ �Ǵ��� ���� ��, ���� �ֱٿ� ���� ��ɾ�� ���� ���� ��ɾ� ���� �ð� ������
		�־��� �ð����� �۴ٸ�, FALSE, ũ�ٸ� TRUE ��ȯ
 parm : period	- [in]  ���� (�� �ð����� ũ��, �۽� ����, �۴ٸ� �۽� �Ұ���) (����: msec)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSC_IsSendTimeValid(UINT64 period)
{
	return (GetTickCount64() > (g_u64LastWriteTime + period));
}

/*
 desc : ���� ������ �Ź� ������ ��ȿ���� ���� �˻�
 parm : g_path	- [in]  �Ź� �����μ� ��ȿ���� �˻��ϱ� ���� Gerber Path (��ü ��� ����)
		sub_path- [out] ���� ��� ���� ���� �� ��ȯ ���� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSC_IsValidGerberPath(PTCHAR g_path, BOOL &sub_path)
{
	INT32 i, i32BasePathLen, i32PathLen;
	TCHAR tzSubPath[MAX_PATH_LEN]	= {NULL};
	PTCHAR ptzPathName;

	CStringArray arrSubPaths;
	CStringArray arrSubFiles;

	/* ���� ��� ���� �ӽ� ���� */
	i32BasePathLen	= (INT32)wcslen(g_path);
	/* ���� ���� �Ź� ���� ��� �۽� */
	uvCmn_SearchRecursePath(g_path, arrSubPaths);

	/* ���� ��ΰ� ���� ��� */
	if (arrSubPaths.GetCount() == 0)
	{
		/* ���� ��� ���� */
		sub_path	= FALSE;
		/* ������ ������ �ִ� ������� */
		uvCmn_SearchFile(g_path, L"*.rle", arrSubFiles);
		if (arrSubFiles.GetCount() < 1)
		{
			arrSubFiles.RemoveAll();
			return FALSE;
		}

		/* �˻� ����� ���� ������ ������ ��� */
		ptzPathName	= wcsrchr(g_path, L'\\') + 1;	/*ptzSubName++;*/
		i32PathLen	= (INT32)wcslen(ptzPathName);
		if (i32PathLen < 1)
		{
			arrSubFiles.RemoveAll();
			return FALSE;
		}

		/* �˻��Ϸ��� ��θ� ��� �ҹ��ڷ� ���� */
		_wcslwr_s(ptzPathName, i32PathLen+1);	/* !!! +1 ���ִ°� �߿� !!! */
		/* ������ ���� �̸��� ���� ���� �̸� ������� ����� ������ ���� */
		for (i=0; i<arrSubFiles.GetCount(); i++)
		{
			/* ������ �ҹ��ڷ� �� */
			if (arrSubFiles[i].MakeLower().Find(ptzPathName) < 0)
			{
				arrSubFiles.RemoveAll();
				return FALSE;
			}
		}
		/* �޸� ���� */
		arrSubFiles.RemoveAll();
	}
	/* ���� ��ΰ� �����ϴ� ��� */
	else
	{
		/* ���� ��� ���� */
		sub_path	= TRUE;
		for (i=0; i<arrSubPaths.GetCount(); i++)
		{
			ptzPathName	= arrSubPaths[i].GetBuffer();
			/* �˻� ���� ��θ� ������ ������ ��θ� �ӽ÷� ���� */
			wmemset(tzSubPath, 0x00, MAX_PATH_LEN);
			wmemcpy(tzSubPath, ptzPathName+i32BasePathLen, wcslen(ptzPathName)-i32BasePathLen);
			/* ���� �˻� ��� ��� ���� ��ΰ� 2�� �̻��̸� ���� */
			if (uvCmn_GetFindChCount(tzSubPath,
									 UINT32(wcslen(ptzPathName)-i32BasePathLen),
									 L'\\') > 1)	return FALSE;
		}
	}

	return TRUE;
}

/*
 desc : ���� �ֱٿ� �߻��� ������ �ִ��� ����
 parm : None
 retn : TRUE or FALSE (���� ����)
*/
API_EXPORT BOOL uvGFSC_IsLastError()
{
	if (!g_pGFSCThread->IsConnected())	return FALSE;

	return g_pGFSCThread->GetLastErrorLevel() == ENG_LNWE::en_error;
}

/*
 desc : ������� �Ź� ������ �۽� ����� �� ��ȯ
 parm : None
 retn : ����� ��
*/
API_EXPORT DOUBLE uvGFSC_GetSendGFilesRate()
{
	if (!g_pGFSCThread->IsConnected())	0.0f;

	return g_pGFSCThread->GetSendGFilesRate();
}

/*
 desc : Client�κ��� ���ŵ� ��Ŷ ��ɾ� �κ� ��ȯ
 parm : None
 retn : ���� ���ŵ� ��ɾ�� �� ���� ���� �����ߴ� ��ɾ� ��ȯ
		���ŵ� ��ɾ �� �̻� ���ٸ� 0x0000 �� ��ȯ
*/
API_EXPORT ENG_PCGG uvGFSC_PopPktHeadCmd()
{
	STG_UTPH stHead	= {NULL};

	if (!g_pGFSCThread->IsConnected())		return ENG_PCGG::en_pcgg_unknown;
	if (!g_pGFSCThread->PopPktHead(stHead))	return ENG_PCGG::en_pcgg_unknown;

	return ENG_PCGG(stHead.cmd);
}

/*
 desc : ���� �Ź� ���� �۽� ���̸� ���� ó��
 parm : None
 retn : None
*/
API_EXPORT VOID uvGFSC_StopGerberSendFile()
{
	if (g_pGFSCThread->IsConnected())	g_pGFSCThread->StopGerberSendFile();
}

/*
 desc : ������� �۽ŵ� ���� ũ�� ��ȯ
 parm : None
 retn : ���� ũ�� (����: Bytes)
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
 desc : ������ Alive Check ��� ����
 parm : logs	- [in]  �α� ���� ���� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSC_PktSendAliveTime()
{
	if (!g_pGFSCThread->IsConnected())	return FALSE;
	return g_pGFSCThread->SetAliveTime();
}

/*
 desc : ������ Alive Check ���� ����
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSC_IsRecvAliveTime()
{
	if (!g_pGFSCThread->IsConnected())	return FALSE;
	return g_pGFSCThread->IsRecvAliveTime();
}

/*
 desc : �Ź� ���� ��� �� ���� ��� ����
 parm : g_name	- [in]  ���� ������ �Ź� �� (��ü ��δ� ������. �⺻ ��� ������ ������ �Ź� ���� ��)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSC_PktSendNewGerberPath(PTCHAR g_name)
{
	if (!g_pGFSCThread->IsConnected())	return FALSE;
	return g_pGFSCThread->SetNewGerberPath(g_name);
}

/*
 desc : �Ź� ���� ��� �� ���� ��� ����
 parm : g_name	- [in]  ������ �Ź� �� (��ü ��δ� ������. �⺻ ��� ������ ������ �Ź� ���� ��)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSC_PktSendDelGerberPath(PTCHAR g_name)
{
	if (!g_pGFSCThread->IsConnected())	return FALSE;
	return g_pGFSCThread->SetDelGerberPath(g_name);
}

/*
 desc : �Ź� ���ϵ��� Server�� ����
 parm : files	- [in]  ���� ������ ����� �迭 ����Ʈ ���� ������
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

	/* ������ �� �������� Ȯ�� */
	if (!g_pGFSCThread->IsConnected())	return FALSE;

	/* ���� �ӽ� �۽� ���� �ʱ�ȭ  */
	g_pGFSCThread->ResetPktGfiles();
	/* ���� ������ ���� */
	u32BasePathLen	= (UINT32)wcslen(pstConfig->gerber_path->gfsc);
	/* ���� ����Ʈ���� ���ϰ� ���� �����Ͽ� ���� */
	if (files->GetCount() < 1)	return FALSE;
	for (; i<(UINT32)files->GetCount(); i++, j++)
	{
		wmemset(tzSubPath, 0x00, MAX_PATH_LEN);
		memset(&stProp, 0x00, sizeof(STG_PGFP));

		/* ���� ���� ������ ������ ���� ���� */
		strGFile= files->GetAt(i);
		ptzGPath= strGFile.GetBuffer() + 1 + u32BasePathLen;
		/* �������� ���� �и� */
		ptzGFile= wcsrchr(ptzGPath, L'\\') + 1;	/* ���� �̸��� ���۵Ǵ� ��ġ ��� */
		if (!ptzGFile)	return FALSE;
		/* ���� �̸� ���� */
		memcpy_s(stProp.path, GERBER_PATH_NAME, csCnv.Uni2Ansi(ptzGPath), wcslen(ptzGPath) - (wcslen(ptzGFile) + 1));
		/* ���� �̸� ���� */
		memcpy_s(stProp.file, GERBER_FILE_NAME, csCnv.Uni2Ansi(ptzGFile), wcslen(ptzGFile));
		/* �Ź� ���� ũ�� */
		stProp.s_size	= uvCmn_GetFileSize(strGFile.GetBuffer());
		/* ���� �ֱ��� ����� ��ο� ���� ������ ��ΰ� �ٸ���, ��ȯ */
		if (0 != strcmp(szLastPath, stProp.path))
		{
			j	= 0;
			/* ���� �ֱ��� ���� ��� ���� */
			memset(szLastPath, 0x00, MAX_PATH_LEN);
			memcpy(szLastPath, stProp.path, strlen(stProp.path));
		}
		/* ���� �Ź� ���� �������� ���� ��ȣ */
		stProp.file_no	= j;

		/* �ӽ� �޸� ���ۿ� ��� */
		g_pGFSCThread->AddPktGFiles(stProp);
	}

	/* Packet �۽� ���� */
	g_pGFSCThread->SetSetupGFile();

	return TRUE;
}

/*
 desc : �������� Recipe Files ��û
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvGFSC_PktSendGetRecipeFile()
{
	if (!g_pGFSCThread->IsConnected())	return FALSE;
	return g_pGFSCThread->GetRecipeFiles();
}

/*
 desc : �������� Recipe Files ����
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