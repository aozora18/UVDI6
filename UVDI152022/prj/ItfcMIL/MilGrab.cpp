/*
 desc : MIL Grabbed Image (Buffer) 관리 및 처리 (Target Image : 검색된 Mark Image)
*/

#include "pch.h"
#include "MainApp.h"
#include "MilGrab.h"
#include "../UVDI15/GlobalVariables.h"


class AlignMotion;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

/*
 desc : 생성자
 parm : config		- [in]  Config Info.
		shmem		- [in]  Shared Memory
		cam_id		- [in]  Align Camera Index. (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		ml_sys		- [in]  MIL System ID
		ml_dis		- [in]  MIL Display ID
		run_mode	- [in]  0x01 : CMPS, 0x01 : Vision Step
 retn : None
*/
#ifndef _NOT_USE_MIL_
CMilGrab::CMilGrab(LPG_CIEA config, LPG_VDSM shmem,
				   UINT8 cam_id, /*, MIL_ID ml_dis, */MIL_ID ml_sys,ENG_ERVM run_mode)
	: CMilImage(config, shmem, cam_id, /*, ml_dis, */ml_sys,run_mode)
#else
CMilGrab::CMilGrab(LPG_CIEA config, LPG_VDSM shmem,
	UINT8 cam_id, ENG_ERVM run_mode)
	: CMilImage(config, shmem, cam_id, run_mode)
#endif
{
	
	m_dbGerbMarkX		= FLT_MAX;
	m_dbGerbMarkY		= FLT_MAX;

	m_bMatched			= FALSE;
	m_u8ResultAll		= 0x00;

	/* 최종 검색한 결과 값 저장 */
	m_pstGrabResult = new STG_ACGR; // (LPG_ACGR)::Alloc(sizeof(STG_ACGR));
	
	memset(m_pstGrabResult, 0x00, sizeof(STG_ACGR));
	m_pstGrabResult->Init();

	/* 기타 검색된 정보 저장을 위한 임시 버퍼 (검색된 마크를 그리기 위함) */
	m_pstResultAll = new STG_GMSR[config->mark_find.max_mark_find];// (LPG_GMSR)::Alloc(config->mark_find.max_mark_find * sizeof(STG_GMSR));
	memset(m_pstResultAll, 0x00, config->mark_find.max_mark_find * sizeof(STG_GMSR));
	/* 최종 저장될 결과 버퍼 메모리 할당 */
	m_pstGrabResult->grab_data = new UINT8[m_u32ImgSize + 1];// (PUINT8)::Alloc(m_u32ImgSize + 1);
	memset(m_pstGrabResult->grab_data, 0x00, m_u32ImgSize+1);
	m_pstGrabResult->grab_data[m_u32ImgSize]	= 0x00;
	m_pstGrabResult->grab_size	= m_u32ImgSize;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CMilGrab::~CMilGrab()
{
	if (m_pstGrabResult)
	{
		/*if (m_pstGrabResult->grab_data)	::Free(m_pstGrabResult->grab_data);
		::Free(m_pstGrabResult);*/
		delete m_pstGrabResult;
		m_pstGrabResult	= NULL;
	}
	if (m_pstResultAll)delete m_pstResultAll;
	m_pstResultAll	= NULL;
}

/*
 desc : Camera로부터 수집된 Binary Image Data 저장
 parm : image	- [in]  Camera에 의해 수집된 Grabbed Image
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
		mark_x	- [in]  기존 거버에 위치한 Mark의 중심 위치 - X
		mark_y	- [in]  기존 거버에 위치한 Mark의 중심 위치 - Y
 retn : TRUE or FALSE
*/
VOID CMilGrab::PutGrabReset()
{
	/* 기존 검색 결과 값 제거 */
	memset(m_pstGrabResult, 0x00, sizeof(STG_ACGR)-sizeof(PUINT8));
	memset(m_pstResultAll, 0x00, m_pstConfig->mark_find.max_mark_find * sizeof(STG_GMSR));
	/* 검색하지 못했다고 설정 */
	m_bMatched		= FALSE;
	m_u8ResultAll	= 0x00;
}
BOOL CMilGrab::PutGrabImage(PUINT8 image, UINT8 img_id, DOUBLE mark_x, DOUBLE mark_y)
{
	PutGrabReset();

	/* 현재 이미지 복사 */
	memcpy(m_pstGrabResult->grab_data, image, m_u32ImgSize);
	/* 기존 거버에 등록된 Mark의 중심 X, Y 좌표 */
	m_dbGerbMarkX	= mark_x;
	m_dbGerbMarkY	= mark_y;
	/* Camera & Image Number */
	m_pstGrabResult->cam_id		= m_u8ACamID;
	m_pstGrabResult->img_id		= img_id;
	m_pstGrabResult->grab_width	= m_u32ImgWidth;
	m_pstGrabResult->grab_height= m_u32ImgHeight;
	m_pstGrabResult->grab_size	= m_u32ImgSize;

	return PutGrabImage(image);
}
BOOL CMilGrab::PutGrabImage(PUINT8 image)
{
	if (!image)	return FALSE;
#ifndef _NOT_USE_MIL_
	/* Grabbed Image 저장 */
	MbufClear(m_mlBufID, M_COLOR_BLACK);
	MbufPut(m_mlBufID, image);
	if (MappGetError(M_CURRENT, M_NULL))
	{
		m_pstGrabResult->grab_size = 0;
		LOG_WARN(ENG_EDIC::en_mil, L"Failed to put the grabbed image");
		return FALSE;
	}
	if (m_pstConfig->grab_proc.use_grab_pre_proc)
	{
		/* Binarize the image with an automatically calculated threshold so that 
			particles are represented in white and the background removed. */
		MimBinarize(m_mlBufID, m_mlBufID,
					M_BIMODAL+m_pstConfig->grab_proc.mim_bin_condition, M_NULL, M_NULL);
		if (MappGetError(M_CURRENT, M_NULL))
		{
			m_pstGrabResult->grab_size = 0;
			LOG_WARN(ENG_EDIC::en_mil, L"Failed to run the MimBinarize function");
			return FALSE;
		}
		if (m_pstConfig->grab_proc.use_small_particle)
		{
			/* Close small holes */
			MimClose(m_mlBufID, m_mlBufID,
					 m_pstConfig->grab_proc.remove_small_particle, M_BINARY);
			if (MappGetError(M_CURRENT, M_NULL))
			{
				m_pstGrabResult->grab_size = 0;
				LOG_WARN(ENG_EDIC::en_mil, L"Failed to run the MimClose function");
				return FALSE;
			}
			/* Remove small particles */
			MimOpen(m_mlBufID, m_mlBufID,
					m_pstConfig->grab_proc.remove_small_particle,
					m_pstConfig->grab_proc.remove_small_procmode);
			if (MappGetError(M_CURRENT, M_NULL))
			{
				m_pstGrabResult->grab_size = 0;
				LOG_WARN(ENG_EDIC::en_mil, L"Failed to run the MimOpen function");
				return FALSE;
			}
		}
		/* Perform a general convolution operation (remove noise) */
		if (m_pstConfig->grab_proc.use_noise_remove)
		{
			MimConvolve(m_mlBufID, m_mlBufID, M_SMOOTH);
			if (MappGetError(M_CURRENT, M_NULL))
			{
				m_pstGrabResult->grab_size = 0;
				LOG_WARN(ENG_EDIC::en_mil, L"Failed to run the MimOpen function");
				return FALSE;
			}
		}
		/* Perform a binary or grayscale thinning operation on an image */
		if (m_pstConfig->grab_proc.use_thin_operation)
		{
			MimThin(m_mlBufID, m_mlBufID, M_TO_SKELETON, m_pstConfig->grab_proc.thin_proc_mode);
			if (MappGetError(M_CURRENT, M_NULL))
			{
				m_pstGrabResult->grab_size = 0;
				LOG_WARN(ENG_EDIC::en_mil, L"Failed to run the MimThin function");
				return FALSE;
			}
		}
	}
#endif
	return TRUE;
}


VOID CMilGrab::FixGrabbedMark(int img_id, double offsetX, double offsetY)
{

}


/*
 desc : 모델 검색 후 검색 결과 값 설정
 parm : img_id		- [in]  Camera Grabbed Image Index (0 or Later)
							(if img_id == 0xff then Calibration Method)
		r_data		- [in]  모델 검색 후 계산된 값 (Main Mark)
		o_data		- [in]  기타 검색 후 계산된 값 (Sub Marks)
		o_count		- [in]  'o_data' 개수
		grab_width	- [in]  검색(Grabbed)된 이미지의 크기 (단위: pixel)
		grab_height	- [in]  검색(Grabbed)된 이미지의 크기 (단위: pixel)
 retn : None
*/


VOID CMilGrab::SetGrabbedMark(int img_id,
							  LPG_GMFR r_data, LPG_GMFR o_data, UINT8 o_count,
							  UINT32 grab_width, UINT32 grab_height)
{
	int i, u8MarkSeq	= 0x00, u8Index = 0, u8Valid = 0x01; //쓸데없이 U8이딴거 쓰면 다 찢어죽여야된다. 메모리가 기본 16기가인 세상이다. 
	DOUBLE dbCaliX = 0.0f, dbCaliY = 0, dbVertX = 0.0f;	/* 반드시 0 값으로 초기화 */
	LPG_CVMF pstMarkFind= &m_pstConfig->mark_find;
	LPG_ACCE pstCaliData= NULL;
	int idx=-1;
	/* 검색된 마크 처리 개수 무조건 초기화 */
	m_u8ResultAll	= 0x00;

	/* ----------------------------------------------------------------------------------- */
	/* img_id가 0xff인 경우, Calibration Method로 동작 중이므로, 아래 루틴이 수행되면 안됨 */
	/* 아래 경우, 정상적인 얼라인 노광(얼라인 카메라 보정 오차 적용 포함)일 경우만 수행 됨 */
	/* ----------------------------------------------------------------------------------- */
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID|| \
	CUSTOM_CODE_UVDI15 == DELIVERY_PRODUCT_ID|| \
	CUSTOM_CODE_HDDI6== DELIVERY_PRODUCT_ID)

	if (0xff != img_id && m_pstConfig->set_align.use_2d_cali_data)
#else
	if (0xff != img_id)
#endif
	{
		/* Align Method 동작이 Each 모두인지 여부 */
		u8MarkSeq	= img_id;
		/* Calibration Table (Position) 적용 여부 */
		if (ENG_AOEM::en_calib_expose == m_enAlignMode && alignMotionPtr != nullptr)
		{
			auto status = alignMotionPtr->status;
			
			vector<STG_XMXY>& pool = status.markPoolForCam[m_u8ACamID];
			bool isGlobal = true;

			
			auto IndexOf = [&](vector<STG_XMXY>& pool, int idx,bool& isGlobal)->int //역산해야한다. 그럼 이 마크시퀀스는 각 로컬이나 글로벌에서 몇번째인가?
			{
				int sum = 0;
				if (pool.empty() || pool.size() <= idx)
				{
					TCHAR errStr[200] = { NULL };
					swprintf_s(errStr, 200, L"grab pool count mismatching. poolsize = %d , gtabcount = %d" , pool.size() , idx);
					LOG_ERROR(ENG_EDIC::en_uvdi15, errStr);
					return -1;
				}

				isGlobal = pool[idx].GetFlag(STG_XMXY_RESERVE_FLAG::GLOBAL);
				if (idx == 0) return 0;

				auto searchFlag = isGlobal ? STG_XMXY_RESERVE_FLAG::GLOBAL : STG_XMXY_RESERVE_FLAG::LOCAL;

				for (int i = idx-1; i >= 0 ; i--)
				{
					sum += (pool[i].GetFlag(searchFlag) == true) ? 1 : 0;
				}
				return sum;
			};

			idx = IndexOf(pool, u8MarkSeq,isGlobal);
			
			if(idx != -1)
				//pstCaliData = isGlobal ? m_pstShMemVisi->cali_global[m_u8ACamID - 1][idx] : m_pstShMemVisi->cali_local[m_u8ACamID - 1][idx];
				pstCaliData = isGlobal ? m_pstShMemVisi->cali_global[m_u8ACamID - 1][idx] : m_pstShMemVisi->cali_local[m_u8ACamID - 1][idx + 2];


			try
			{
				/* 보정 오차 값이 저장되어 있는 공유 메모리에서 값 가져오기 */
				if (idx != -1)
				{
					dbCaliX = pstCaliData->acam_cali_x / 10000.0f;
					dbCaliY = pstCaliData->stage_cali_y / 10000.0f;
					dbVertX = pstCaliData->gerb_mark_x_diff / 10000.0f;
				}
			}
			catch (...)
			{

			}
		}
	}

	/* Sub 검색 결과 값 초기화 */
	memset(m_pstResultAll, 0x00, pstMarkFind->max_mark_find * sizeof(STG_GMSR));

	if (r_data)
	{
		//m_pstGrabResult->submaskFindInfo = m_pstResultAll;
		
		/* Grabbed Image의 최종 연산 결과 값 저장 (unit : pixel) */
		m_pstGrabResult->model_index	= (UINT8)r_data->model_index;
		m_pstGrabResult->mark_cent_px_x	= r_data->cent_x;
		m_pstGrabResult->mark_cent_px_y	= r_data->cent_y;
		m_pstGrabResult->cent_dist_x	= r_data->cent_dist_x;	/* 마크 들의 중심 간의 떨어진 거리 (단위: um) */
		m_pstGrabResult->cent_dist_y	= r_data->cent_dist_y;	/* 마크 들의 중심 간의 떨어진 거리 (단위: um) */
		m_pstGrabResult->square_rate	= r_data->square_rate;
		m_pstGrabResult->valid_multi	= r_data->valid_multi;
		m_pstGrabResult->grab_size		= grab_width * grab_height;
		m_pstGrabResult->mark_method	= r_data->mark_method;
		
		for (i=0; i<o_count && o_data; i++)
		{
			m_pstResultAll[i].cent_x		= o_data[i].cent_x;
			m_pstResultAll[i].cent_y		= o_data[i].cent_y;
			m_pstResultAll[i].mark_width	= o_data[i].mark_width;
			m_pstResultAll[i].mark_height	= o_data[i].mark_height;

			
			m_pstResultAll[i].move_px_x = (m_u32ImgWidth / 2.0f - o_data[i].cent_x) * (m_pstConfig->set_cams.acam_inst_angle ? -1 : 1);
			m_pstResultAll[i].move_px_y = (m_u32ImgHeight / 2.0f - o_data[i].cent_y) * (m_pstConfig->set_cams.acam_inst_angle ? -1 : 1);
			m_pstResultAll[i].move_mm_x = m_pstResultAll[i].move_px_x * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1);
			m_pstResultAll[i].move_mm_y = m_pstResultAll[i].move_px_y * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1);

			m_pstResultAll[i].valid_multi	= o_data[i].IsValid();
			m_u8ResultAll++;
		}
		/* Score 값이 있는 것만, 결국 찾은 것만 ... */
		if (r_data->score_rate > 0.0f)
		{
			/* ------------------------------------------------------------------------------------- */
			/* grabbed image의 중심에서 검색된 Mark의 중심과의 거리가 얼마큼 떨어져 있는지 계산 (mm) */
			/* !!! 중요함. 이미지 중심에서 Mark의 중심 값을 뺀 후 다시 결과 값에 음수 값 부여함) !!! */
			/* ------------------------------------------------------------------------------------- */
			m_pstGrabResult->move_px_x = (m_u32ImgWidth / 2.0f - r_data->cent_x);
			m_pstGrabResult->move_px_y = (m_u32ImgHeight / 2.0f- r_data->cent_y);
			/* 얼라인 카메라 180도 회전 여부에 부등호 반대 설정 계산 */
			if (m_pstConfig->set_cams.acam_inst_angle)
			{
				m_pstGrabResult->move_px_x	*= -1.0f;
				m_pstGrabResult->move_px_y	*= -1.0f;
			}
#if 0
			/* Stage가 역방향으로 이동하면서 측정한 경우 */
			if (m_bStageMoveDirect)	m_pstGrabResult->move_px_x	*= -1.0f;
			if (m_bStageMoveDirect)	m_pstGrabResult->move_px_y	*= -1.0f;
#endif
			m_pstGrabResult->move_mm_x = m_pstGrabResult->move_px_x * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1); 
			m_pstGrabResult->move_mm_y = m_pstGrabResult->move_px_y * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1);
			m_pstGrabResult->circle_radius = r_data->circle_radius * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1);

			/* 워크 테이블의 보정 값 적용 */
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID|| \
	CUSTOM_CODE_UVDI15 == DELIVERY_PRODUCT_ID|| \
	CUSTOM_CODE_HDDI6== DELIVERY_PRODUCT_ID)

			if (m_pstConfig->set_align.use_2d_cali_data)
			{
				m_pstGrabResult->move_px_x += (dbVertX / m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1));
				m_pstGrabResult->move_mm_x += dbVertX;

				m_pstGrabResult->move_px_x -= (dbCaliX / m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1));
				m_pstGrabResult->move_mm_x -= dbCaliX;
				m_pstGrabResult->move_px_y -= (dbCaliY / m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1));
				m_pstGrabResult->move_mm_y -= dbCaliY;

				
				for (i=0; i<o_count && o_data; i++)
				{
					m_pstResultAll[i].cent_x += (dbVertX / m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1));
					m_pstResultAll[i].cent_x -= (dbCaliX / m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1));
					
					m_pstResultAll[i].cent_y += (dbCaliY / m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1));
				}
			}
#endif
			/* 이미지의 중심에서 검색된 Mark 까지의 떨어진 거리 계산 */
			m_pstGrabResult->mark_cent_mm_dist = r_data->cent_dist * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1);	/* 이 값은 중요하지 않음. X or Y 축 선택해도 무관. 현재 사용되지 않음 */
			/* 회전된 각도와 검색된 매칭 비율 값 */
			m_pstGrabResult->mark_angle		= r_data->r_angle;
			m_pstGrabResult->score_rate		= r_data->score_rate;
			m_pstGrabResult->scale_rate		= r_data->scale_rate;
			m_pstGrabResult->scale_size		= r_data->scale_size;
			m_pstGrabResult->coverage_rate	= r_data->coverage_rate;
			m_pstGrabResult->fit_error		= r_data->fit_error;
#if 0
			if (m_dbScoreRate && m_dbScoreRate)
			{
				m_pstGrabResult->valid_scale= r_data->scale_rate >= (m_dbScoreRate) ? 0x01 : 0x00;
				m_pstGrabResult->valid_score= r_data->score_rate >= (m_dbScoreRate) ? 0x01 : 0x00;
			}
			else
#endif
			{
				//m_pstGrabResult->valid_scale= r_data->scale_rate >= (pstMarkFind->scale_rate) ? 0x01 : 0x00;
				//m_pstGrabResult->valid_score= r_data->score_rate >= (pstMarkFind->score_rate) ? 0x01 : 0x00;
				m_pstGrabResult->valid_scale = r_data->scale_rate >= (m_dbRecipeScale) ? 0x01 : 0x00;
				m_pstGrabResult->valid_score = r_data->score_rate >= (m_dbRecipeScore) ? 0x01 : 0x00;
			}
			m_bMatched	= m_pstGrabResult->IsMarkValid();
		}
		/* 검색 실패한 경우 */
		else
		{
			/* 검색된 이미지의 중심 값을 Mark의 중심으로 일단 설정 */
			m_pstGrabResult->mark_cent_px_x = m_pstConfig->set_cams.aoi_size[0] / 2.0f;
			m_pstGrabResult->mark_cent_px_y = m_pstConfig->set_cams.aoi_size[1] / 2.0f;
		}
	}
	/* 검색 대상의 Mark Size */
	if (r_data)
	{
		m_pstGrabResult->mark_width_px	= r_data->mark_width;
		m_pstGrabResult->mark_height_px	= r_data->mark_height;
		m_pstGrabResult->mark_width_mm	= r_data->mark_width * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1);
		m_pstGrabResult->mark_height_mm	= r_data->mark_height * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1);
	}
	/* Grabbed Image의 최종 연산 결과 값 저장 (unit : mm) */
	m_pstGrabResult->mark_cent_mm_x = m_pstGrabResult->mark_cent_px_x * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1);
	m_pstGrabResult->mark_cent_mm_y = m_pstGrabResult->mark_cent_px_y * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1);
}

/*
 desc : 값을 강제로 설정 (외부에서 마크 인식 못한 경우, 수동으로 인식하도록 처리)
 parm : grab		- [in]  사용자에 의해 수동으로 입력된 grabbed image 결과 정보가 저장된 구조체 포인터
		gmsr		- [in]  GMSR Data
		grab_width	- [in]  Grabbed Image Size (Width. Pixel)
		grab_height	- [in]  Grabbed Image Size (Height. Pixel)
 retn : None
*/
VOID CMilGrab::PutGrabbedMark(LPG_ACGR grab, LPG_GMSR gmsr, UINT32 grab_width, UINT32 grab_height)
{
	memcpy(m_pstGrabResult, grab, sizeof(STG_GMSR));
	memcpy(&m_pstResultAll[0], gmsr, sizeof(STG_GMSR));
	m_u8ResultAll	= 0x01;
	m_bMatched		= TRUE;
}

/*
 desc : 현재 Grabbed Image 검색된 결과를 이미지 파일로 저장
 parm : img_id	- [in]  Camera Grabbed Image Index (0 or Later) (if img_id == 0xff then Calibration Method)
		r_data	- [in]  패턴 검색 후 계산된 값
 retn : None
*/
VOID CMilGrab::SaveGrabbedMarkToFile(UINT8 img_id, LPG_GMFR r_data)
{
	TCHAR tzTime[32]= {NULL}, tzResult[32] = {NULL};
	TCHAR tzSrc[MAX_PATH_LEN] = {NULL};
	SYSTEMTIME stTM	= {NULL};
	CUniToChar csCnv;

	if (0xff == m_u8ACamID)	return;
	/* Current Time */
	GetLocalTime(&stTM);
	/* Set the time name */
	swprintf_s(tzTime, 32, L"%02d%02d%02d_%02d%02d%02d",
			   stTM.wYear-2000, stTM.wMonth, stTM.wDay,
			   stTM.wHour, stTM.wMinute, stTM.wSecond);
	/* 무조건 저장 */
	if (r_data)
	{
		swprintf_s(tzResult, 32, L"(%6.3f_%6.3f_%u)",
					r_data->score_rate, r_data->scale_rate, r_data->scale_size);
	}
	else
	{
		swprintf_s(tzResult, 32, L"(00.000_00.000_0)");
	}
	/* Set the full file name */
	swprintf_s(tzSrc, MAX_PATH_LEN, L"%s\\save_img\\mark\\%s_%d_%02d(%s)_%s.png",
			   g_tzWorkDir, tzTime, m_u8ACamID, img_id, tzResult, tzTime);

#ifndef _NOT_USE_MIL_
	/* Grabbed Image 저장 */
	MbufExport(tzSrc, M_PNG, m_mlBufID);
#endif
}

/*
 desc : 현재 적재된 레시피의 마크 검색 조건 값 설정
 parm : score	- [in]  Score Accpetance (대상 이미지에서 검색된 마크의 Score 값이 이 값보다 작으면 버린다)
		scale	- [in]  Scale Range (대상 이미지에서 등록된 마크를 검색 결과들 중에서 이 비율 범위 내에 )
									(포함된 결과들만 유효하고, 그 중에서 Score 값이 가장 높은 것만 추출)
									(가령. 이 값이 50.0000 이면, 실제 검색에 사용되는 마크의 크기 범위)
									(검색범위 = 1.0f - (100.0 - 50.0) / 100 임 즉, 0.5 ~ 1.5 임)
 retn : None
*/
VOID CMilGrab::SetRecipeMarkRate(DOUBLE score, DOUBLE scale)
{
	m_dbRecipeScore	= score;
	m_dbRecipeScale	= scale;
}