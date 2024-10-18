
/*
 desc : User Data Family : Job Management
*/

#include "pch.h"
#include "MainApp.h"
#include "JobManagement.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : shmem	- [in]  Shared Memory (Luria Service)
		error	- [in]  각종 에러 정보를 관리하는 객체
 retn : None
*/
CJobManagement::CJobManagement(LPG_LDSM shmem, CCodeToStr *error)
	: CBaseFamily(shmem, error)
{
	/* 기본 Family ID 설정 */
	m_enFamily	= ENG_LUDF::en_job_management;
	/* Job Management Structure */
	m_pstJobMgt	= &shmem->jobmgt;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CJobManagement::~CJobManagement()
{
}

/* --------------------------------------------------------------------------------------------- */
/*                                    Packet Function (Make)                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Root directory where pre-processed jobs are placed
 parm : flag	- [in]  Get (Read) or Set (Write)
		path	- [in]	String (without null-termination) that contains the root directory
						where all pre-processed jobs can be found. A trailing backslash (\) is optional
		size	- [in]  The size of 'path' buffer
 retn : Pointer where the packet buffer is stored
 note : There must be no assigned jobs when changing RootDirectory
		If trying to change RootDirectory while jobs are assigned, an error will be returned
		The RootDirectory given must exist unless giving an empty string (L = 0), which will clear the root directory
*/
PUINT8 CJobManagement::GetPktRootDirectory(ENG_LPGS flag, PCHAR file, UINT32 size)
{
	UINT32 u32Pkts	= MAX_PATH_LEN*2;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCJM::en_root_directory,
							  pPktNext, u32Pkts, PUINT8(file), size);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag  && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCJM::en_root_directory,
							  pPktNext, u32Pkts-m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : If no jobs are assigned, then nothing is returned, L = 0
		The paths may be a mix of relative and absolute paths, depending on how the jobs were assigned
 parm : None
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CJobManagement::GetPktJobList()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Read */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCJM::en_get_job_list, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Assign (add) a new job. The path name given is the path to valid pre-processed job
		An error message will be given if the path does not contain valid files
		The first assigned job will be set as selected job automatically
 parm : path	- [in]  Path name text string (without null-termination) of a preprocessed job
						Could be relative path (relative to RootDirectory) or absolute path
						A trailing backslash (\) is optional
		size	- [in]  The size of 'path' buffer
 retn : Pointer where the packet buffer is stored
 note : Note that the photo heads MUST be powered off and on again after running SystemShutdown command
*/
PUINT8 CJobManagement::GetPktAssignJob(PCHAR path, UINT32 size)
{
	UINT32 u32Pkts	= 512;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCJM::en_assign_job,
							pPktNext, u32Pkts, PUINT8(path), size);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Delete the selected job from the list of assigned jobs
		If no jobs assigned, an error will be returned
		(This command will not delete anything from the disk, only from the list of assigned jobs)
 parm : None
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CJobManagement::GetPktDeleteSelectedJob()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCJM::en_delete_selected_job, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : If a read is attempted and no assigned jobs, then “No job” is returned
 parm : flag	- [in]  Get (Read) or Set (Write)
		path	- [in]	Path name text string (without null-termination) of selected job
						The path may be relative or absolute. A trailing backslash (\) is optional
		size	- [in]  The size of 'path' buffer
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CJobManagement::GetPktSelectedJob(ENG_LPGS flag, PCHAR path, UINT32 size)
{
	UINT32 u32Pkts	= MAX_PATH_LEN*2;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
/* Write */
	if (ENG_LPGS::en_set == flag)
	{
		pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCJM::en_selected_job,
			pPktNext, u32Pkts, PUINT8(path), size);
	}
	/* Read */
	if (ENG_LPGS::en_set_only != flag && (ENG_LPGS::en_set == flag || ENG_LPGS::en_get == flag))
	{
		pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCJM::en_selected_job,
			pPktNext, u32Pkts - m_u32PktSize);
	}

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Load the currently selected job to the photo head(s)
		A reply is returned immediately and the load progress can be queried using “GetSelectedJobLoadState”
		Alternatively, announcements from the Luria can be used. Luria will send “LoadState”-announcements
		after each strip being loaded to a photo head and also when all files are loaded completely
		(or if any failure happened).
 parm : None
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CJobManagement::GetPktLoadSelectedJob()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_write, ENG_LCJM::en_load_selected_job, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Returns the current load-state of the system
 parm : None
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CJobManagement::GetPktSelectedJobLoadState()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCJM::en_get_selected_job_load_state, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Returns the global and local fiducial points of the given job
 parm : length	- [in]  Length of path name text string (S). Max = 200
		path	- [in]  Path name text string (without null-termination) of job
						The path may be relative or absolute. A trailing backslash (\) is optional
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CJobManagement::GetPktGetFiducials(UINT16 length, PCHAR path)
{
	UINT8 u8Body[MAX_PATH_LEN]	= {NULL}, *pBody = u8Body;
	UINT16 u16Length= length;
	UINT32 u32Pkts	= MAX_PATH_LEN*2;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */
	length	= SWAP16(length);
	memcpy(pBody, &length, 2);	pBody	+= 2;
	memcpy(pBody, path, u16Length);

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCJM::en_get_fiducials,
						  pPktNext, u32Pkts, u8Body, u16Length+2);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Returns a set of parameters for the given job
 parm : length	- [in]  Length of path name text string (S). Max = 200
		path	- [in]  Path name text string (without null-termination) of job
						The path may be relative or absolute. A trailing backslash (\) is optional
 retn : Pointer where the packet buffer is stored
 note : Note that the number of parameters is given by the “Command version” parameter
*/
PUINT8 CJobManagement::GetPktGetJobParams(UINT8 length, PCHAR path)
{
	UINT8 u8Body[MAX_PATH_LEN]	= {NULL}, *pBody = u8Body;
	UINT32 u32Pkts	= MAX_PATH_LEN*2;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */
	if (ValidVersion(3, 0, 0, 0x00, FALSE))			*pBody	= 0x02;	/* Command version : x >= 3.0.0 */
	else if (ValidVersion(2, 12, 0, 0x00, FALSE))	*pBody	= 0x01;	/* Command version : 2.11.0 < x < 3.0.0 */
	else											*pBody	= 0x00;	/* Command version : x <= 2.11.0 */
						pBody	+= 1;
	*pBody	= length;	pBody	+= 1;
	memcpy(pBody, path, length);

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCJM::en_get_parameters,
						  pPktNext, u32Pkts, u8Body, length+2);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Returns all defined dynamic panel data dcodes for the given job
 parm : length	- [in]  Length of path name text string (S). Max = 200
		path	- [in]  Path name text string (without null-termination) of job
						The path may be relative or absolute. A trailing backslash (\) is optional
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CJobManagement::GetPktGetPanelDataDecodeList(UINT16 length, PCHAR path)
{
	UINT8 u8Body[MAX_PATH_LEN]	= {NULL}, *pBody = u8Body;
	UINT16 u16Length= length;
	UINT32 u32Pkts	= MAX_PATH_LEN*2;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */
	length	= SWAP16(length);
	memcpy(pBody, &length, 2);	pBody	+= 2;
	memcpy(pBody, path, u16Length);

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCJM::en_get_panel_data_dcode_list,
						  pPktNext, u32Pkts, u8Body, u16Length+2);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : Returns all the panel data information for the given d-code and job
 parm : dcode	- [in]  D-code
		length	- [in]  Length of path name text string (S). Max = 200
		path	- [in]  Path name text string (without null-termination) of job
						The path may be relative or absolute. A trailing backslash (\) is optional
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CJobManagement::GetPktGetPanelDataInfo(UINT32 dcode, UINT16 length, PCHAR path)
{
	UINT8 u8Body[MAX_PATH_LEN]	= {NULL}, *pBody = u8Body;
	UINT16 u16Length= length;
	UINT32 u32Pkts	= MAX_PATH_LEN*2;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */
	dcode	= SWAP32(dcode);
	memcpy(pBody, &dcode, 2);	pBody	+= 4;
	length	= SWAP16(length);
	memcpy(pBody, &length, 2);	pBody	+= 2;
	memcpy(pBody, path, u16Length);

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCJM::en_get_panel_data_info,
						  pPktNext, u32Pkts, u8Body, u16Length+6);

	/* Returns the packet buffer */
	return pPktBuff;
}

/*
 desc : The number of jobs that can be assigned simultaneously will vary according to photo head hardware and MachineConfig: ArtworkComplexity setting.
 parm : None
 retn : Pointer where the packet buffer is stored
*/
PUINT8 CJobManagement::GetPktGetMaxJobs()
{
	UINT32 u32Pkts	= 128;
	PUINT8 pPktBuff	= NULL, pPktNext;

	/* It must be initialized */
	m_u32PktSize= 0;
	/* Allocates a buffer of packets to be sent */
	pPktBuff	= new UINT8[u32Pkts];
	memset(pPktBuff, 0x00, u32Pkts);
	pPktNext	= pPktBuff;
	/* Set the send packet */

	/* Write */
	pPktNext = GetPktBase(ENG_LTCT::en_read, ENG_LCJM::en_get_max_jobs, pPktNext, u32Pkts);

	/* Returns the packet buffer */
	return pPktBuff;
}

/* --------------------------------------------------------------------------------------------- */
/*                                    Packet Function (Save)                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 수신받은 데이터 처리 - Machine Config
 parm : uid		- [in]  서브 명령어
		data	- [in]  데이터가 저장된 버퍼 포인터
		size	- [in]  'data' 의 크기
 retn : None
*/
VOID CJobManagement::SetRecvPacket(UINT8 uid, PUINT8 data, UINT32 size)
{
	switch (uid)
	{
	case ENG_LCJM::en_get_selected_job_load_state	:	SetRecvJob1Bytes(uid, data);			break;
	case ENG_LCJM::en_get_max_jobs					:	SetRecvJob2Bytes(uid, data);			break;
	case ENG_LCJM::en_get_job_list					:
	case ENG_LCJM::en_root_directory				:
	case ENG_LCJM::en_selected_job					:
	case ENG_LCJM::en_get_fiducials					:
	case ENG_LCJM::en_get_parameters				:
	case ENG_LCJM::en_get_panel_data_dcode_list		:
	case ENG_LCJM::en_get_panel_data_info			:	SetRecvJobEtcBytes(uid, data, size);	break;
	}
}

/*
 desc : 1 Bytes 수신 데이터 처리
 parm : uid		- [in]  서브 명령어
		data	- [in]  수신된 데이터
 retn : None
*/
VOID CJobManagement::SetRecvJob1Bytes(UINT8 uid, PUINT8 data)
{
	switch (uid)
	{
	case ENG_LCJM::en_get_selected_job_load_state	:	UpdateJobLoadState(data[0]);	break;
	}
}

/*
 desc : 2 Bytes 수신 데이터 처리
 parm : uid		- [in]  서브 명령어
		data	- [in]  수신된 데이터
		size	- [in]  'data' 버퍼의 크기
 retn : None
*/
VOID CJobManagement::SetRecvJob2Bytes(UINT8 uid, PUINT8 data)
{
	UINT16 u16Data	= 0;

	switch (uid)
	{
	case ENG_LCJM::en_get_max_jobs	:	memcpy(&u16Data, data, sizeof(UINT16));
										m_pstJobMgt->max_job_regist	= SWAP16(u16Data);		break;
	}
}

/*
 desc : xx Bytes 수신 데이터 처리
 parm : uid		- [in]  서브 명령어
		data	- [in]  수신된 데이터
		size	- [in]  'data' 버퍼의 크기
 retn : None
*/
VOID CJobManagement::SetRecvJobEtcBytes(UINT8 uid, PUINT8 data, UINT32 size)
{
	switch (uid)
	{
	case ENG_LCJM::en_root_directory			:	SetRecvRootDirectory(data, size);		break;
	case ENG_LCJM::en_selected_job				:	SetRecvSelectedJobName(data, size);		break;
	case ENG_LCJM::en_get_job_list				:	SetRecvJobList(data, size);				break;
	case ENG_LCJM::en_get_fiducials				:	SetRecvFiducials(data);					break;
	case ENG_LCJM::en_get_parameters			:	SetRecvJobParams(data);					break;
	case ENG_LCJM::en_get_panel_data_dcode_list	:	SetRecvPanelDataDCodeList(data, size);	break;
	case ENG_LCJM::en_get_panel_data_info		:	SetRecvPanelDataInfo(data, size);		break;
	}
}

/*
 desc : 현재 선택된 거버의 적재 상태 갱신
 parm : state	- [in]  적재 진행 상태 (ENG_LSLS)
 retn : None
*/
VOID CJobManagement::UpdateJobLoadState(UINT8 state)
{
	if (m_pstJobMgt->job_regist_count < 1)	return;
	m_pstJobMgt->selected_job_load_state	= state;
}

/*
 desc : 수신받은 거버의 Root Directory 정보 갱신
 parm : data	- [in]  수신받은 Root Directory의 정보가 저장된 버퍼
		size	- [in]  'data' 버퍼에 저장된 크기
 retn : None
*/
VOID CJobManagement::SetRecvRootDirectory(PUINT8 data, UINT32 size)
{
	/* Set the root directory */
	if (data && size < MAX_PATH_LEN && size > 0)
	{
#if 0	/* Because of C# T.T */
		memcpy(m_pstJobMgt->root_directory, data, size);
		m_pstJobMgt->root_directory[size]	= 0x00;
#else
 		memset(m_pstJobMgt->root_directory, 0x00, MAX_PATH_LEN);
 		memcpy(m_pstJobMgt->root_directory, data, size);
#endif
	}
	else
	{
		memset(m_pstJobMgt->root_directory, 0x00, MAX_PATH_LEN);
	}
}

/*
 desc : 수신받은 거버의 Job Selected 정보 갱신
 parm : data	- [in]  수신받은 Job Selected의 정보가 저장된 버퍼
		size	- [in]  'data' 버퍼에 저장된 크기
 retn : None
*/
VOID CJobManagement::SetRecvSelectedJobName(PUINT8 data, UINT32 size)
{
	UINT8 i	= 0;
	BOOL bSucc	= FALSE;
	CHAR szJobName[MAX_GERBER_NAME]	= {NULL}, szMesg[96] = {NULL};
	CUniToChar csCnv;

	/* 만약 어떠한 Job Name도 선택된 것이 없다면 ... */
	if (0 == strcmp("No job", (PCHAR)data))
	{
		LOG_WARN(ENG_EDIC::en_luria, L"The selected Job Name does not exist");
		return;
	}

	/* Set the root directory */
	if (size < MAX_GERBER_NAME && size > 0)
	{
		memcpy(szJobName, data, size);
		szJobName[size]	= 0x00;

		/* 기존 등록된 Job Name과 동일한 것이 있는지 확인 */
		for (; i<m_pstJobMgt->job_regist_count; i++)
		{
			if (0 == strcmp(m_pstJobMgt->job_name_list[i], szJobName))
			{
				bSucc	= TRUE;
				break;
			}
		}
	}

	/* 기존 등록 (선택)된 Job Name 초기화 */
	memset(m_pstJobMgt->selected_job_name, 0x00, MAX_GERBER_NAME);

	if (!bSucc || size < 1)
	{
		if (size < 1)	sprintf_s(szMesg, 96, "Failed to select the Job Name (Empty)");
		else			sprintf_s(szMesg, 96, "Failed to select the Job Name (%s)", szJobName);
		LOG_ERROR(ENG_EDIC::en_luria, csCnv.Ansi2Uni(szMesg));
	}
	else
	{
		memcpy(m_pstJobMgt->selected_job_name, szJobName, size);
		/*m_pstJobMgt->selected_job_name[size]	= 0x00;*/
	}
}

/*
 desc : Job List 저장
 parm : data	- [in]  Job List가 저장되어 있는 버퍼
		size	- [in]  'data' 버퍼에 저장되어 있는 데이터의 크기
 retn : None
*/
VOID CJobManagement::SetRecvJobList(PUINT8 data, UINT32 size)
{
	UINT32 u32Find	= 0, i = 0, j = 0;

	/* 등록된 Job List가 없는지 여부에 따라 */
	if (!data || size < 1)
	{
		for (; i<MAX_REGIST_JOB_LIST; i++)
		{
			memset(m_pstJobMgt->job_name_list[i], 0x00, MAX_GERBER_NAME);
		}
	}
	else
	{
		/* 현재 등록된 거버 파일이 여러 개인 경우라면, 아래 루프 동작 */
		for (u32Find=0; i<size && j<MAX_REGIST_JOB_LIST; i++)
		{
			if (';' == data[i])
			{
				memset(m_pstJobMgt->job_name_list[j], 0x00, MAX_GERBER_NAME);
				memcpy(m_pstJobMgt->job_name_list[j], data+u32Find, i-u32Find);
				u32Find	= i + 1;
				j++;
			}
		}
		if (size && j<MAX_REGIST_JOB_LIST/* 최대 8개까지만 등록 가능 하므로 */)
		{
			/* 맨 마지막 등록 */
			memset(m_pstJobMgt->job_name_list[j], 0x00, MAX_GERBER_NAME);
			if (0 == u32Find)	memcpy(m_pstJobMgt->job_name_list[j], data, size);
			else				memcpy(m_pstJobMgt->job_name_list[j], data+u32Find, size - u32Find);
			j++;
		}
	}

	/* 등록된 Job Name 개수 저장 */
	m_pstJobMgt->job_regist_count	= UINT8(j);
}

/*
 desc : 수신받은 거버의 Fiducial 정보 갱신
 parm : data	- [in]  수신받은 Fiducial의 정보가 저장된 버퍼
 retn : None
*/
VOID CJobManagement::SetRecvFiducials(PUINT8 data)
{
	CHAR szJobName[MAX_GERBER_NAME]	= {NULL};
	CHAR szMsg[256]	= {NULL};
	UINT16 u16Data	= 0, i;
	INT32 i32Data	= 0;
	PUINT8 pData	= data;
	CUniToChar csCnv;

	/* Job Name Length */
	memcpy(&u16Data, pData, 2);			pData	+= 2;
	u16Data	= SWAP16(u16Data);
	memcpy(szJobName, pData, u16Data);	pData	+= u16Data;

	/* 기존 선택된 Job Name과 요청한 Fiducial Job Name이 다른지 확인 */
	if (0 != strcmp(szJobName, m_pstJobMgt->selected_job_name))
	{
		sprintf_s(szMsg, 256,
				  "The Job Name (%s) requested is different from the Job Name (%s) selected",
				  szJobName, m_pstJobMgt->selected_job_name);
		LOG_ERROR(ENG_EDIC::en_luria, csCnv.Ansi2Uni(szMsg));
	}

	/* Skipped the hash data */
	pData	+= 64;

	/* Global Fiducials */
	memcpy(&i32Data, pData, 4);		pData	+= 4;
	m_pstJobMgt->selected_job_fiducial.g_d_code	= SWAP32(i32Data);

	if (m_pstJobMgt->selected_job_fiducial.g_d_code > 0)
	{
		memcpy(&u16Data, pData, 2);	pData	+= 2;
		m_pstJobMgt->selected_job_fiducial.g_coord_count	= SWAP16(u16Data);
		/* Global Fiducial Count 만큼 Coord. X / Y 좌표 저장 */
		for (i=0; i<m_pstJobMgt->selected_job_fiducial.g_coord_count; i++)
		{
			memcpy(&i32Data, pData, sizeof(INT32));	pData	+= 4;
			m_pstJobMgt->selected_job_fiducial.g_coord_xy[i].x	= SWAP32(i32Data);
			memcpy(&i32Data, pData, sizeof(INT32));	pData	+= 4;
			m_pstJobMgt->selected_job_fiducial.g_coord_xy[i].y	= SWAP32(i32Data);
		}
	}

	/* Local Fiducials */
	memcpy(&i32Data, pData, 4);		pData	+= 4;
	m_pstJobMgt->selected_job_fiducial.l_d_code	= SWAP32(i32Data);

	if (m_pstJobMgt->selected_job_fiducial.l_d_code > 0)
	{
		memcpy(&u16Data, pData, 2);	pData	+= 2;
		m_pstJobMgt->selected_job_fiducial.l_coord_count	= SWAP16(u16Data);
		/* Global Fiducial Count 만큼 Coord. X / Y 좌표 저장 */
		for (i=0; i<m_pstJobMgt->selected_job_fiducial.l_coord_count; i++)
		{
			memcpy(&i32Data, pData, sizeof(INT32));	pData	+= 4;
			m_pstJobMgt->selected_job_fiducial.l_coord_xy[i].x	= SWAP32(i32Data);
			memcpy(&i32Data, pData, sizeof(INT32));	pData	+= 4;
			m_pstJobMgt->selected_job_fiducial.l_coord_xy[i].y	= SWAP32(i32Data);
		}
	}
}

/*
 desc : 수신받은 거버의 Job Parameters 정보 갱신
 parm : data	- [in]  수신받은 Parameters의 정보가 저장된 버퍼
 retn : None
*/
VOID CJobManagement::SetRecvJobParams(PUINT8 data)
{
	CHAR szJobName[MAX_GERBER_NAME]	= {NULL};
	CHAR szMesg[256]	= {NULL};
	UINT16 u8Data		= 0;
	PUINT8 pData		= data;
	STG_JMJP stParam	= {NULL};
	CUniToChar csCnv;

	pData++;												/* skip command_version */
	u8Data	= *pData;	pData++;							/* Job Name Length */
	memcpy(szJobName, pData, u8Data);	pData	+= u8Data;	/* Job Path (Name) */

	/* 기존 선택된 Job Name과 요청한 Fiducial Job Name이 다른지 확인 */
	if (0 != strcmp(szJobName, m_pstJobMgt->selected_job_name))
	{
		sprintf_s(szMesg, 256,
				  "The Job Name (%s) requested is different from the Job Name (%s) selected",
				  szJobName, m_pstJobMgt->selected_job_name);
		LOG_ERROR(ENG_EDIC::en_luria, csCnv.Ansi2Uni(szMesg));
		return;
	}

	/* 나머지는 구조체에 복사 */
	memcpy(&stParam, pData, sizeof(STG_JMJP));
	/* Bytes Ordering */
	stParam.SWAP();

	/* 공유 메모리에 복사 */
	memcpy(&m_pstJobMgt->get_job_params, &stParam, sizeof(STG_JMJP));
}

/*
 desc : 수신받은 거버의 Panel Data DCode List 정보 갱신
 parm : data	- [in]  수신받은 Parameters의 정보가 저장된 버퍼
		size	- [in]  'data' 버퍼에 저장된 크기
 retn : None
*/
VOID CJobManagement::SetRecvPanelDataDCodeList(PUINT8 data, UINT32 size)
{
	CHAR szJobName[MAX_GERBER_NAME]	= {NULL};
	CHAR szMesg[256]	= {NULL};
	UINT16 u16Data		= 0;
	UINT32 u32Size		= size;
	PUINT8 pData		= data;
	LPG_JMPL pstDCode	= {NULL};
	CUniToChar csCnv;

	/* Job Name Length */
	memcpy(&u16Data, pData, 2);			pData	+= 2;		u32Size	-= 2;
	u16Data	= SWAP16(u16Data);
	memcpy(szJobName, pData, u16Data);	pData	+= u16Data;	u32Size	-= u16Data;

	/* 기존 선택된 Job Name과 요청한 Fiducial Job Name이 다른지 확인 */
	if (0 != strcmp(szJobName, m_pstJobMgt->selected_job_name))
	{
		sprintf_s(szMesg, 256,
				  "The Job Name (%s) requested is different from the Job Name (%s) selected",
				  szJobName, m_pstJobMgt->selected_job_name);
		LOG_ERROR(ENG_EDIC::en_luria, csCnv.Ansi2Uni(szMesg));
	}

	/* 구조체 복사 */
	pstDCode = new STG_JMPL();// (LPG_JMPL)::Alloc(sizeof(STG_JMPL));
	ASSERT(pstDCode);
	memcpy(pstDCode, pData, u32Size);
	/* Bytes Ordering */
	pstDCode->SWAP();

	/* 공유 메모리에 복사 */
	memcpy(&m_pstJobMgt->selected_job_dcode_list, pstDCode, u32Size);

	/* 메모리 해제 */
	delete pstDCode;
}

/*
 desc : 수신받은 거버의 Panel Data Info 정보 갱신
 parm : data	- [in]  수신받은 Infromation의 정보가 저장된 버퍼
		size	- [in]  'data' 버퍼에 저장된 크기
 retn : None
*/
VOID CJobManagement::SetRecvPanelDataInfo(PUINT8 data, UINT32 size)
{
	CHAR szJobName[MAX_GERBER_NAME]	= {NULL};
	CHAR szMesg[256]	= {NULL};
	UINT16 u16Data		= 0;
	UINT32 u32DCode		= 0;
	UINT32 u32Size		= size;
	PUINT8 pData		= data;
	LPG_JMPD pstPanel	= NULL;
	CUniToChar csCnv;

	/* D-Code Value */
	memcpy(&u32DCode, pData, 4);		pData	+= 4;		u32Size	-= 4;
	u32DCode= SWAP32(u32DCode);
	/* Job Name Length */
	memcpy(&u16Data, pData, 2);			pData	+= 2;		u32Size	-= 2;
	u16Data	= SWAP16(u16Data);
	memcpy(szJobName, pData, u16Data);	pData	+= u16Data;	u32Size	-= u16Data;

	/* 기존 선택된 Job Name과 요청한 Fiducial Job Name이 다른지 확인 */
	if (0 != strcmp(szJobName, m_pstJobMgt->selected_job_name))
	{
		sprintf_s(szMesg, 256,
				  "The Job Name (%s) requested is different from the Job Name (%s) selected",
				  szJobName, m_pstJobMgt->selected_job_name);
		LOG_ERROR(ENG_EDIC::en_luria, csCnv.Ansi2Uni(szMesg));
	}

	/* 구조체 복사 */
	pstPanel = new STG_JMPD();// (LPG_JMPD)::Alloc(sizeof(STG_JMPD));
	ASSERT(pstPanel);
	memcpy(pstPanel, pData, u32Size);

	/* Bytes Ordering */
	pstPanel->SWAP();

	/* 공유 메모리에 복사 */
	m_pstJobMgt->selected_d_code	= u32DCode;	/* 현재 선택된 D Code 값 임시 저장 */
	memcpy(&m_pstJobMgt->selected_job_panel_data, pstPanel, u32Size);

	/* 메모리 해제 */
	delete pstPanel;
	
}

/*
 desc : Updates the Job Load Status received from Announcement
 parm : data	- [in]  The following four data values are stored
 		ph_no	- [in]  Photohead Number just completed loading a strip
		strip	- [in]  Strip number just completed loading
		total	- [in]  Total number of strips to be loaded
 retn : None	
*/
VOID CJobManagement::SetJobLoadState(PUINT8 data)
{
	m_pstJobMgt->selected_job_load_state	= data[0];
	m_pstJobMgt->job_state_ph_loaded		= data[1];
	m_pstJobMgt->job_state_strip_loaded		= data[2];
	m_pstJobMgt->job_total_strip_loaded		= data[3];
}