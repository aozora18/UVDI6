
#pragma once

#include "BaseFamily.h"

class CPhotoheadFocus : public CBaseFamily
{
	CPhotoheadFocus(LPG_LDSM shmem);

public:

	virtual VOID		SetRecvPacket(UINT8 uid, PUINT8 data, UINT32 size);


	/* 로컬 변수 */
protected:

	LPG_LDPF			m_pstShMem;


	/* 로컬 함수 */
protected:

	VOID				SetRecv1Bytes(UINT8 uid, PUINT8 data);
	VOID				SetRecv2Bytes(UINT8 uid, PUINT8 data);
	VOID				SetRecv4Bytes(UINT8 uid, PUINT8 data);
	VOID				SetRecv5Bytes(UINT8 uid, PUINT8 data);
	VOID				SetRecv9Bytes(UINT8 uid, PUINT8 data);
	VOID				SetRecvEtcBytes(UINT8 uid, PUINT8 data, UINT32 size);

public:
	unique_ptr<UINT8[]> SendPktInitializeFocus();
	unique_ptr<UINT8[]> SendReadPktAbs_work_range_status();
	unique_ptr<UINT8[]> SendPktFocusposition(ENG_LPGS flag, UINT8 headNum, INT32 zPosMicrometer);
	unique_ptr<UINT8[]> SendPktActivateAutofocus(ENG_LPGS flag, UINT8 headNum, bool enable);
	unique_ptr<UINT8[]> SendPktFocustrim(ENG_LPGS flag, UINT8 headNum, INT32 trimNanometer);
	unique_ptr<UINT8[]> SendPktActiveAreaQualifier(ENG_LPGS flag, bool enable, UINT32 startInactiveInMicrometer, UINT32 EndInactiveInMicrometer);
	unique_ptr<UINT8[]> SendPktAbs_work_range(ENG_LPGS flag, UINT8 headNum, INT32 minFocusAFOperationMicrometer, INT32 maxFocusAFOperationMicrometer);
	unique_ptr<UINT8[]> SendPktAf_edge_trigger(ENG_LPGS flag, UINT16 belowAbobeDistActivateAFinMicrometer, bool enable);
	unique_ptr<UINT8[]> SendPktPosition_multi(vector<tuple<UINT8, INT32>> positions);
	unique_ptr<UINT8[]> SendPktPosition_hi_res(ENG_LPGS flag);
	unique_ptr<UINT8[]> SendPktOutside_dof_status(ENG_LPGS flag);
	unique_ptr<UINT8[]> SendPktAf_gain(ENG_LPGS flag, UINT16 gain);
	unique_ptr<UINT8[]> SendPktScan_ethercat();
	unique_ptr<UINT8[]> SendPktPanelMode(ENG_LPGS flag, UINT8 headNum, UINT8 mode);
	unique_ptr<UINT8[]> SendPktDisableAF();

};


class CMachineConfig : public CBaseFamily
{
/* 생성자 & 파괴자 */
public:

	CMachineConfig(LPG_LDSM shmem, CCodeToStr *error);
	virtual ~CMachineConfig();


/* 가상 함수 */
protected:

public:

	virtual VOID		SetRecvPacket(UINT8 uid, PUINT8 data, UINT32 size);


/* 로컬 변수 */
protected:

	LPG_LDMC			m_pstShMem;


/* 로컬 함수 */
protected:

	VOID				SetRecv1Bytes(UINT8 uid, PUINT8 data);
	VOID				SetRecv2Bytes(UINT8 uid, PUINT8 data);
	VOID				SetRecv4Bytes(UINT8 uid, PUINT8 data);
	VOID				SetRecv5Bytes(UINT8 uid, PUINT8 data);
	VOID				SetRecv9Bytes(UINT8 uid, PUINT8 data);
	VOID				SetRecvEtcBytes(UINT8 uid, PUINT8 data, UINT32 size);


/* 공용 함수 */
public:

	PUINT8				GetPktTotalPhotoheads(ENG_LPGS flag, UINT8 ph_count=0);								/* uid : 0x01 */
	PUINT8				GetPktPhotoheadIpAddr(ENG_LPGS flag, UINT8 ph_no, PUINT8 ipv4=NULL);				/* uid : 0x02 */
	PUINT8				GetPktPhotoheadPitch(ENG_LPGS flag, UINT8 pitch=0);									/* uid : 0x03 */
	PUINT8				GetPktPhotoheadRotate(ENG_LPGS flag, UINT8 rotate=0);								/* uid : 0x05 */
	PUINT8				GetPktParallelogramMotionAdjust(ENG_LPGS flag, UINT8 tbl_no, UINT32 factor=0);		/* uid : 0x06 */
	PUINT8				GetPktScrollRate(ENG_LPGS flag, UINT16 scroll=0);									/* uid : 0x07 */
	PUINT8				GetPktMotionControlType(ENG_LPGS flag, UINT8 type=0);								/* uid : 0x08 */
	PUINT8				GetPktMotionControlIpAddr(ENG_LPGS flag, PUINT8 ipv4=NULL);							/* uid : 0x09 */
	PUINT8				GetPktXcorrectionTable(ENG_LPGS flag, UINT8 tbl_no,
											   UINT16 e_count=0, PINT32 e_x_pos=NULL, PINT32 e_x_adj=NULL);	/* uid : 0x0c */
	PUINT8				GetPktTablePositionLimits(ENG_LPGS flag, UINT8 tbl_no,
												  INT32 max_x=0, INT32 max_y=0,
												  INT32 min_x=0, INT32 min_y=0);							/* uid : 0x0d */
	PUINT8				GetPktTableExposureStartPos(ENG_LPGS flag, UINT8 tbl_no,
												   INT32 start_x=0, INT32 start_y=0);						/* uid : 0x0e */
	PUINT8				GetPktMaxYMotionSpeed(ENG_LPGS flag, UINT32 speed=0);								/* uid : 0x0f */
	PUINT8				GetPktXMotionSpeed(ENG_LPGS flag, UINT32 speed=0);									/* uid : 0x10 */
	PUINT8				GetPktYaccelerationDistance(ENG_LPGS flag, UINT32 dist=0);							/* uid : 0x13 */
	PUINT8				GetPktHysteresisType1(ENG_LPGS flag, UINT8 scroll, UINT16 offset=0,
											  UINT32 d_pos=0, UINT32 d_neg=0);								/* uid : 0x14 */
	PUINT8				GetPktPhotoheadOffset(ENG_LPGS flag, UINT8 ph_no,
											  UINT32 x_pos=0, INT32 y_pos=0);								/* uid : 0x16 */
	PUINT8				GetPktPhotoHeadOffsetAll();
	PUINT8				GetPktTablePrintDirect(ENG_LPGS flag, UINT8 tbl_no, UINT8 direct=0);				/* uid : 0x17 */
	PUINT8				GetPktActiveTable(ENG_LPGS flag, UINT8 tbl_no=0x00);								/* uid : 0x18 */
	PUINT8				GetPktGetTableMotionStartPosition(UINT8 tbl_no);									/* uid : 0x19 */
	PUINT8				GetPktEmulate(ENG_LPGS flag, UINT8 mc2=0, UINT8 ph=0, UINT8 trig=0);				/* uid : 0x1a, 0x1b, 0x1c */
	PUINT8				GetPktDebugPrintLevel(ENG_LPGS flag, UINT8 level=0);								/* uid : 0x1d */
	PUINT8				GetPktPrintSimulationOutDir(ENG_LPGS flag, PCHAR out_dir=NULL, UINT32 size=0);		/* uid : 0x1e */
	PUINT8				GetPktYcorrectionTable(ENG_LPGS flag, UINT8 tbl_no,
											   UINT16 count=0, PUINT8 strip_no=NULL, PINT32 y_adj=NULL);	/* uid : 0x20 */
	PUINT8				GetPktLinearZdriveSetting(ENG_LPGS flag, UINT8 ph_no, UINT8 drv_no=0x00);			/* uid : 0x21 */
	PUINT8				GetPktZdriveIpAddr(ENG_LPGS flag, PUINT8 addr=NULL);								/* uid : 0x23 */
	PUINT8				GetPktXYDriveId(ENG_LPGS flag, UINT8 tbl_no, UINT8 xdrvid=0, UINT8 ydrvid=0);		/* uid : 0x24 */
	PUINT8				GetPktProductId(ENG_LPGS flag, UINT32 pid=0);										/* uid : 0x25 */
	PUINT8				GetPktArtworkComplexity(ENG_LPGS flag, UINT8 level=0);								/* uid : 0x26 */
	PUINT8				GetPktZdriveType(ENG_LPGS flag, UINT8 type=0);										/* uid : 0x27 */
	PUINT8				GetPktOverPressureMode(ENG_LPGS flag, UINT8 mode=0);								/* uid : 0x28 */
	PUINT8				GetPktDepthOfFocus(ENG_LPGS flag, UINT16 step=0);									/* uid : 0x29 */
	PUINT8				GetPktExtraLongStripes(ENG_LPGS flag, UINT8 type=0);								/* uid : 0x2a */
	PUINT8				GetPktMTCMode(ENG_LPGS flag, UINT8 mode=0);											/* uid : 0x2b */
	PUINT8				GetPkUseEthercatForAF(ENG_LPGS flag, UINT8 enable=0);								/* uid : 0x2c */
	PUINT8				GetPktSpxLevel(ENG_LPGS flag, UINT16 level=0);										/* uid : 0x2d */
	PUINT8				GetPktOcpErrorMask(ENG_LPGS flag, UINT16 mask=0);									/* uid : 0x2e */
	PUINT8				GetPktPositionHysteresisType1(ENG_LPGS flag, UINT8 scroll,
													  UINT32 p_delay=0, UINT32 n_delay=0);					/* uid : 0x2f */
	PUINT8				GetPktCustomEdgeBlendBitmap(ENG_LPGS flag, PCHAR file=NULL, UINT32 size=0);			/* uid : 0x30 */

	PUINT8				GetPktTableSettings(ENG_LPGS flag, UINT8 num, UINT32 parallel=0, UINT8 y_dir=0);

};