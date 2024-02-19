
#pragma once

#include "BaseFamily.h"

class CJobManagement : public CBaseFamily
{
/* 생성자 & 파괴자 */
public:

	CJobManagement(LPG_LDSM shmem, CCodeToStr *error);
	virtual ~CJobManagement();


/* 가상 함수 */
protected:

public:

	virtual VOID		SetRecvPacket(UINT8 cmd, PUINT8 data, UINT32 size);


/* 로컬 변수 */
protected:

	LPG_LDJM			m_pstJobMgt;

/* 로컬 함수 */
protected:

	VOID				SetRecvJob1Bytes(UINT8 uid, PUINT8 data);
	VOID				SetRecvJob2Bytes(UINT8 uid, PUINT8 data);
	VOID				SetRecvJobEtcBytes(UINT8 uid, PUINT8 data, UINT32 size);

	VOID				UpdateJobLoadState(UINT8 state);
	VOID				SetRecvRootDirectory(PUINT8 data, UINT32 size);
	VOID				SetRecvSelectedJobName(PUINT8 data, UINT32 size);
	VOID				SetRecvJobList(PUINT8 data, UINT32 size);
	VOID				SetRecvFiducials(PUINT8 data);
	VOID				SetRecvJobParams(PUINT8 data);
	VOID				SetRecvPanelDataDCodeList(PUINT8 data, UINT32 size);
	VOID				SetRecvPanelDataInfo(PUINT8 data, UINT32 size);


/* 공용 함수 */
public:

	PUINT8				GetPktRootDirectory(ENG_LPGS flag, PCHAR job=NULL, UINT32 size=0);	/* uid = 0x01 */
	PUINT8				GetPktJobList();													/* uid = 0x02 */
	PUINT8				GetPktAssignJob(PCHAR path, UINT32 size);							/* uid = 0x03 */
	PUINT8				GetPktDeleteSelectedJob();											/* uid = 0x04 */
	PUINT8				GetPktSelectedJob(ENG_LPGS flag, PCHAR path=NULL, UINT32 size=0);	/* uid = 0x05 */
	PUINT8				GetPktLoadSelectedJob();											/* uid = 0x06 */
	PUINT8				GetPktSelectedJobLoadState();										/* uid = 0x07 */
	PUINT8				GetPktGetFiducials(UINT16 length, PCHAR path);						/* uid = 0x08 */
	PUINT8				GetPktGetJobParams(UINT8 length, PCHAR path);						/* uid = 0x09 */
	PUINT8				GetPktGetPanelDataDecodeList(UINT16 length, PCHAR path);			/* uid = 0x0a */
	PUINT8				GetPktGetPanelDataInfo(UINT32 dcode, UINT16 length, PCHAR path);	/* uid = 0x0b */
	PUINT8				GetPktGetMaxJobs();													/* uid = 0x0c */
	/* Announcement로부터 수신된 Job Load 상태 갱신 */
	VOID				SetJobLoadState(PUINT8 data);
};