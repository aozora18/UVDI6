
#pragma once

#include "EBase.h"

class CE87CMS : public CEBase
{
public:
	CE87CMS(CLogData *logs, CSharedData *share);
	~CE87CMS();

/* 구조체 */
protected:


/* 가상 함수 */
protected:
public:


/* 로컬 변수 */
protected:



/* 로컬 함수 */
protected:

	BOOL				InitE87CMS();
	BOOL				InitE87HW();


/* 공용 함수 */
public:

	BOOL				Init(LONG equip_id, LONG conn_id);
	VOID				Close();
	BOOL				Finalized(LONG equip_id, LONG conn_id);
	BOOL				SetLogLevel(ENG_ILLV level);

	ICxE87CMSPtr		GetE87CMSPtr()						{	return m_pICxE90STPtr;		}

	BOOL				GetSubstratePtr(PTCHAR carr_id, UINT8 slot_no,
										ICxSubstratePtr &st_ptr);
	BOOL				GetSubstratePtrFromSource(PTCHAR carr_id, UINT8 slot_no,
												  ICxSubstratePtr &st_ptr);

	BOOL				SetCarrierPopSubstrate(PTCHAR carr_id, UINT8 slot_no);
	BOOL				SetTransferStart(UINT16 port_id);
	BOOL				SetCarrierPlaced(UINT16 port_id);
	BOOL				SetReadyToUnload(UINT16 port_id);
	BOOL				SetCarrierAtPort(PTCHAR carr_id, UINT16 port_id, PTCHAR loc_id);
	BOOL				SetCarrierDepartedPort(PTCHAR carr_id, UINT16 port_id, PTCHAR undocked);
	BOOL				SetMoveCarrier(PTCHAR carr_id, UINT16 port_id, PTCHAR docked);
	BOOL				SetVerifySlotMap(PTCHAR carr_id, CAtlList <E87_CSMS> *slots, LONG handle);
	BOOL				SetAccessCarrier(LONG conn_id, PTCHAR carr_id, UINT16 port_id,
										 E87_CCAS status, LONG handle);
	BOOL				SetCarrierEvent(LONG conn_id, PTCHAR carr_id, UINT16 port_id,
										E87_CCES status);
	BOOL				SetAllowRemoteControl(BOOL enable);
	BOOL				GetAllowRemoteControl(BOOL &enable);
	
	BOOL				SetSubstratePlaced(PTCHAR carr_id, PTCHAR st_id, UINT8 slot_no);

	BOOL				GetReservationState(UINT16 port_id, E87_LPRS &state);
	BOOL				SetReservationState(UINT16 port_id, E87_LPRS state);
	
	BOOL				GetAssociationState(UINT16 port_id, E87_LPAS &state);
	BOOL				SetAssociationState(UINT16 port_id, E87_LPAS state);
	
	BOOL				GetCarrierIDStatus(PTCHAR carr_id, E87_CIVS &status);
	BOOL				SetCarrierIDStatus(PTCHAR carr_id, E87_CIVS status);
	BOOL				GetCarrierIDSlotMapStatus(PTCHAR carr_id, E87_SMVS &status);
	BOOL				SetCarrierIDSlotMapStatus(PTCHAR carr_id, E87_SMVS status);
	BOOL				GetCarrierIDSlotStatus(PTCHAR carr_id, CAtlList <E87_CSMS> &status);

	BOOL				SetProceedWithCarrier(UINT16 port_id, PTCHAR carr_id);
	BOOL				SetCancelCarrier(UINT16 port_id, PTCHAR carr_id);
	BOOL				SetCancelCarrierAtPort(UINT16 port_id);
	BOOL				SetCancelCarrierOut(PTCHAR carr_id);
	BOOL				SetCancelCarrierNotification(PTCHAR carr_id);
	BOOL				SetCarrierReCreate(UINT16 port_id, PTCHAR carr_id);
	BOOL				SetCarrierRelease(PTCHAR carr_id);
	BOOL				SetBeginCarrierOut(PTCHAR carr_id, UINT16 port_id);
	BOOL				SetCarrierOut(PTCHAR carr_id, UINT16 port_id);
	BOOL				SetCarrierIn(PTCHAR carr_id);
	BOOL				SetCancelAllCarrierOut();

	BOOL				GetAccessModeState(UINT16 port_id, E87_LPAM &state);
	BOOL				SetAccessModeState(UINT16 port_id, E87_LPAM state);

	BOOL				SetAddCarrierLocation(PTCHAR loc_id);

	BOOL				GetAssociationCarrierID(UINT16 port_id, PTCHAR carr_id, UINT32 size);
	BOOL				SetAssociationCarrierID(UINT16 port_id, PTCHAR carr_id);
	BOOL				GetAssociationPortID(PTCHAR carr_id, UINT16 &port_id);
	BOOL				SetAssociationPortID(PTCHAR carr_id, UINT16 port_id);

	BOOL				SetAsyncCarrierTagReadComplete(PTCHAR carr_id, PTCHAR tag_id, LONG trans_id);
	BOOL				GetCarrierTagReadData(PTCHAR loc_id, PTCHAR carr_id, PTCHAR data_seg,
											  PTCHAR data_tag, UINT32 size);
	BOOL				GetTransferStatus(UINT16 port_id, E87_LPTS &status);
	BOOL				SetTransferStatus(UINT16 port_id, E87_LPTS status);

	BOOL				SetCancelBind(UINT16 port_id, PTCHAR carr_id);
	BOOL				SetCancelReserveAtPort(UINT16 port_id);

	BOOL				GetCarrierAccessingStatus(PTCHAR carr_id, E87_CCAS &status);
	BOOL				SetCarrierAccessingStatus(PTCHAR carr_id, E87_CCAS status);
	/* Load Port와 관련된 모든 정보 반환 (Carrier ID 포함) */
	BOOL				GetLoadPortCarrierData(UINT16 port_no, STG_CLPI &data);

	BOOL				IsVerifiedCarrierID(PTCHAR carr_id);

	BOOL				SetRecipeManagement(LONG conn_id, PTCHAR recipe, UINT8 type, LONG handle=0);
	BOOL				SetRecipeSelected(LONG conn_id, PTCHAR recipe, LONG handle);
};

