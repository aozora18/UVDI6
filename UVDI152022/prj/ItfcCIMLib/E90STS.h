
#pragma once

#include "EBase.h"

class CE90STS : public CEBase
{
public:
	CE90STS(CLogData *logs, CSharedData *share);
	~CE90STS();

/* 구조체 */
protected:

/* 가상 함수 */
protected:
public:


/* 로컬 변수 */
protected:



/* 로컬 함수 */
protected:

	BOOL				InitE90STS();
	VOID				CloseE90STS();
	BOOL				InitE90HW();
	VOID				CloseE90HW();


/* 공용 함수 */
public:

	BOOL				GetSubstrateIDPtr(PTCHAR subst_id, ICxSubstrate **ptr);
	BOOL				GetSubstrateLocIDPtr(PTCHAR loc_id, ICxSubstrateLocation **ptr);
	BOOL				GetBatchSubstIDPtr(PTCHAR loc_id, ICxBatchLoc **ptr);

	BOOL				Init(LONG equip_id, LONG conn_id);
	VOID				Close();
	BOOL				SetLogLevel(ENG_ILLV level);

	ICxE90STPtr			GetE90STSPtr()						{	return m_pICxE90STPtr;		}

	BOOL				SetChangeSubstrateState(PTCHAR subst_id, PTCHAR loc_id, E90_SSTS trans,
												E90_SSPS proc, PTCHAR time_in=NULL, PTCHAR time_out=NULL);
	BOOL				SetChangeSubstrateState2(PTCHAR subst_id, PTCHAR loc_id, E90_SSTS trans,
												 E90_SSPS proc, E90_SISM status,
												 PTCHAR time_in=NULL, PTCHAR time_out=NULL);
	BOOL				SetChangeSubstrateProcessingState(PTCHAR subst_id, E90_SSPS proc);
	BOOL				RegisterSubstrate(PTCHAR subst_id, PTCHAR loc_id, INT16 sub_id, PTCHAR lot_id);
	BOOL				RemoveSubstrate(PTCHAR subst_id);

	BOOL				AddSubstrateLocation(PTCHAR loc_id);
	BOOL				RemoveSubstrateLocation(PTCHAR loc_id);

	BOOL				SetProceedWithSubstrate(PTCHAR subst_id);

	BOOL				GetSubstrateIDToAcquiredID(PTCHAR subst_id, PTCHAR acquired_id, UINT32 size);
	BOOL				SetSubstrateIDToAcquiredID(PTCHAR subst_id, PTCHAR acquired_id);

	BOOL				GetAllowRemoteControl(BOOL &enable);
	BOOL				SetAllowRemoteControl(BOOL enable);

	BOOL				GetSubstrateLocationState(PTCHAR loc_id, E90_SSLS &state);
	BOOL				SetSubstrateLocationState(PTCHAR loc_id, E90_SSLS state);

	BOOL				GetSubstratePrevProcessingState(PTCHAR subst_id, E90_SSPS &state);
	BOOL				GetSubstrateProcessingState(PTCHAR subst_id, E90_SSPS &state);
	BOOL				SetSubstrateProcessingState(PTCHAR subst_id, E90_SSPS state);

	BOOL				GetSubstratePrevTransportState(PTCHAR subst_id, E90_SSTS &state);
	BOOL				GetSubstrateTransportState(PTCHAR subst_id, E90_SSTS &state);
	BOOL				SetSubstrateTransportState(PTCHAR subst_id, E90_SSTS state);

	BOOL				GetSubstratePrevIDStatus(PTCHAR subst_id, E90_SISM &state);
	BOOL				GetSubstrateIDStatus(PTCHAR subst_id, E90_SISM &state);
	BOOL				SetSubstrateIDStatus(PTCHAR subst_id, E90_SISM state);

	BOOL				NotifySubstrateRead(PTCHAR subst_id, PTCHAR acquired_id, BOOL read_good);
	BOOL				SubstrateReaderAvailable(BOOL enable);

	BOOL				CancelSubstrate(PTCHAR subst_id);

	BOOL				GetSubstrateIDToSlotNo(PTCHAR subst_id, UINT8 &slot_no);
	BOOL				GetSubstrateLocationID(PTCHAR subst_id, PTCHAR loc_id, UINT32 size);
	BOOL				GetSubstrateID(ICxSubstratePtr st_ptr, PTCHAR subst_id, UINT32 size);
	BOOL				GetSubstrateDestinationID(ICxSubstratePtr st_ptr, PTCHAR subst_id, UINT32 size);

	BOOL				GetPRCommandSubstState(PTCHAR subst_id, STG_CSDI &state);
	BOOL				GetSubstratePtr(PTCHAR subst_id, ICxSubstratePtr &st_ptr);

	BOOL				IsLocationSubstrateID(PTCHAR loc_name);

	BOOL				GetBatchLocIdx(PTCHAR loc_id, INT16 &index);
	BOOL				SetBatchLocIdx(PTCHAR loc_id, INT16 index);

	BOOL				IsBatchLocOccupied(PTCHAR loc_id);
	BOOL				SetBatchLocState(PTCHAR loc_id, E90_SSLS state);

	BOOL				GetBatchSubstIDMap(PTCHAR loc_id, CStringArray &subst_id);
	BOOL				SetBatchSubstIDMap(PTCHAR loc_id, CStringArray *subst_id);

	BOOL				GetDisableEventsBatch(PTCHAR batch_id, BOOL &flag);
	BOOL				SetDisableEventsBatch(PTCHAR batch_id, BOOL flag);
	BOOL				GetDisableEventsLoc(PTCHAR loc_id, BOOL &flag);
	BOOL				SetDisableEventsLoc(PTCHAR loc_id, BOOL flag);

	BOOL				GetMaxSubstratePosition(PTCHAR loc_id, LONG &max_pos);
	BOOL				GetObjectName(PTCHAR loc_id, PTCHAR name, UINT32 size);
	BOOL				SendDataNotificationBatch(PTCHAR loc_id);
	BOOL				SendDataNotificationSubst(PTCHAR loc_id);

	BOOL				AddSubstrateBatchLocation(PTCHAR loc_id, INT32 loc_num, INT16 sub_id);
	BOOL				DelSubstrateBatchLocation(PTCHAR loc_id);

	BOOL				AddCarrierToBatch(LONG batch_id, PTCHAR carr_id, LONG slot_map);
	BOOL				DelCarrierFromBatch(LONG batch_id, PTCHAR carr_id);

	BOOL				CreateBatch(LONG batch_id, PTCHAR loc_id);
	BOOL				DestroyBatch(LONG batch_id);

	BOOL				ChangeBatchState(LONG batch_id, PTCHAR loc_id,
										 E90_SSTS trans_state, E90_SSPS proc_state,
										 PTCHAR time_in, PTCHAR time_out);

	BOOL				AddSubstrateLocation(PTCHAR loc_id, PTCHAR subst_id, INT16 sub_id);
	BOOL				DelSubstrateLocation(PTCHAR loc_id);

	BOOL				AddSubstrateToBatch(LONG batch_id, PTCHAR subst_id);
	BOOL				DelSubstrateFromBatch(LONG batch_id, PTCHAR subst_id);
	BOOL				AddEmptySpaceToBatch(LONG batch_id);

	BOOL				ReserveSubstrateLocationIndices(INT16 base_sub_index, INT16 reserve_size);

	BOOL				ChangeSubstrateTransportState(PTCHAR subst_id, PTCHAR loc_id, E90_SSTS state,
													  PTCHAR time_in, PTCHAR time_out);
	BOOL				GetSubstrateMaterialStatus(PTCHAR subst_id, LONG &status);
	BOOL				SetSubstrateMaterialStatus(PTCHAR subst_id, LONG status);

	BOOL				SetCurrentSubstrateType(PTCHAR subst_id, E90_EMST type);
	BOOL				GetCurrentSubstrateType(PTCHAR subst_id, E90_EMST &type);

	BOOL				GetSubstrateGoodRead(PTCHAR subst_id, BOOL &read);
	BOOL				SetSubstrateGoodRead(PTCHAR subst_id, BOOL read);

	BOOL				GetSubstrateLotID(PTCHAR subst_id, PTCHAR lot_id, UINT32 size);
	BOOL				SetSubstrateLotID(PTCHAR subst_id, PTCHAR lot_id);

	BOOL				GetSubstrateObjID(PTCHAR subst_id, PTCHAR obj_id, UINT32 size);
	BOOL				GetSubstrateObjType(PTCHAR subst_id, PTCHAR obj_type, UINT32 size);

	BOOL				GetLocationObjID(PTCHAR loc_id, PTCHAR obj_id, UINT32 size);
	BOOL				GetLocationObjType(PTCHAR loc_id, PTCHAR obj_type, UINT32 size);

	BOOL				GetSourceCarrierID(PTCHAR subst_id, PTCHAR carr_id, UINT32 size);
	BOOL				SetSourceCarrierID(PTCHAR subst_id, PTCHAR carr_id);

	BOOL				GetSourceLoadPortID(PTCHAR subst_id, LONG &port_id);
	BOOL				SetSourceLoadPortID(PTCHAR subst_id, LONG port_id);

	BOOL				GetSourceSlotID(PTCHAR subst_id, LONG &slot_id);
	BOOL				SetSourceSlotID(PTCHAR subst_id, LONG slot_id);

	BOOL				GetCurrentSubstrateLocID(PTCHAR subst_id, PTCHAR loc_id, UINT32 size);
	BOOL				SetCurrentSubstrateLocID(PTCHAR subst_id, PTCHAR loc_id);

	BOOL				GetSubstrateSourceID(PTCHAR subst_id, PTCHAR src_id, UINT32 size);
	BOOL				SetSubstrateSourceID(PTCHAR subst_id, PTCHAR src_id);
	BOOL				GetSubstrateDestinationID(PTCHAR subst_id, PTCHAR tgt_id, UINT32 size);
	BOOL				SetSubstrateDestinationID(PTCHAR subst_id, PTCHAR tgt_id);

	BOOL				GetUsingGroupEventsSubst(PTCHAR subst_id, BOOL &flag);
	BOOL				SetUsingGroupEventsSubst(PTCHAR subst_id, BOOL flag);
	BOOL				GetUsingGroupEventsLoc(PTCHAR loc_id, BOOL &flag);
	BOOL				SetUsingGroupEventsLoc(PTCHAR loc_id, BOOL flag);

	BOOL				GetLocationSubstID(PTCHAR loc_id, PTCHAR subst_id, UINT32 size);
	BOOL				SetLocationSubstID(PTCHAR loc_id, PTCHAR subst_id);

	BOOL				GetLocationIndex(PTCHAR loc_id, INT16 &index);
	BOOL				SetLocationIndex(PTCHAR loc_id, INT16 index);

	BOOL				GetSubstrateHistory(PTCHAR subst_id, CAtlList <STG_CSHI> &lst_hist);
	BOOL				SetSubstrateHistory(PTCHAR subst_id, CAtlList <STG_CSHI> *lst_hist);
};
