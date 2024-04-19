/*
 desc : MIL Grabbed Image (Buffer) ���� �� ó�� (Target Image : �˻��� Mark Image)
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
 desc : ������
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

	/* ���� �˻��� ��� �� ���� */
	m_pstGrabResult = new STG_ACGR; // (LPG_ACGR)::Alloc(sizeof(STG_ACGR));
	
	memset(m_pstGrabResult, 0x00, sizeof(STG_ACGR));
	m_pstGrabResult->Init();

	/* ��Ÿ �˻��� ���� ������ ���� �ӽ� ���� (�˻��� ��ũ�� �׸��� ����) */
	m_pstResultAll	= (LPG_GMSR)::Alloc(config->mark_find.max_mark_find * sizeof(STG_GMSR));
	memset(m_pstResultAll, 0x00, config->mark_find.max_mark_find * sizeof(STG_GMSR));
	/* ���� ����� ��� ���� �޸� �Ҵ� */
	m_pstGrabResult->grab_data	= (PUINT8)::Alloc(m_u32ImgSize + 1);
	memset(m_pstGrabResult->grab_data, 0x00, m_u32ImgSize+1);
	m_pstGrabResult->grab_data[m_u32ImgSize]	= 0x00;
	m_pstGrabResult->grab_size	= m_u32ImgSize;
}

/*
 desc : �ı���
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
	if (m_pstResultAll)	::Free(m_pstResultAll);
	m_pstResultAll	= NULL;
}

/*
 desc : Camera�κ��� ������ Binary Image Data ����
 parm : image	- [in]  Camera�� ���� ������ Grabbed Image
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
		mark_x	- [in]  ���� �Ź��� ��ġ�� Mark�� �߽� ��ġ - X
		mark_y	- [in]  ���� �Ź��� ��ġ�� Mark�� �߽� ��ġ - Y
 retn : TRUE or FALSE
*/
VOID CMilGrab::PutGrabReset()
{
	/* ���� �˻� ��� �� ���� */
	memset(m_pstGrabResult, 0x00, sizeof(STG_ACGR)-sizeof(PUINT8));
	memset(m_pstResultAll, 0x00, m_pstConfig->mark_find.max_mark_find * sizeof(STG_GMSR));
	/* �˻����� ���ߴٰ� ���� */
	m_bMatched		= FALSE;
	m_u8ResultAll	= 0x00;
}
BOOL CMilGrab::PutGrabImage(PUINT8 image, UINT8 img_id, DOUBLE mark_x, DOUBLE mark_y)
{
	PutGrabReset();

	/* ���� �̹��� ���� */
	memcpy(m_pstGrabResult->grab_data, image, m_u32ImgSize);
	/* ���� �Ź��� ��ϵ� Mark�� �߽� X, Y ��ǥ */
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
	/* Grabbed Image ���� */
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

/*
 desc : �� �˻� �� �˻� ��� �� ����
 parm : img_id		- [in]  Camera Grabbed Image Index (0 or Later)
							(if img_id == 0xff then Calibration Method)
		r_data		- [in]  �� �˻� �� ���� �� (Main Mark)
		o_data		- [in]  ��Ÿ �˻� �� ���� �� (Sub Marks)
		o_count		- [in]  'o_data' ����
		grab_width	- [in]  �˻�(Grabbed)�� �̹����� ũ�� (����: pixel)
		grab_height	- [in]  �˻�(Grabbed)�� �̹����� ũ�� (����: pixel)
 retn : None
*/
VOID CMilGrab::SetGrabbedMark(int img_id,
							  LPG_GMFR r_data, LPG_GMFR o_data, UINT8 o_count,
							  UINT32 grab_width, UINT32 grab_height)
{
	int i, u8MarkSeq	= 0x00, u8Index = 0, u8Valid = 0x01; //�������� U8�̵��� ���� �� �����׿��ߵȴ�. �޸𸮰� �⺻ 16�Ⱑ�� �����̴�. 
	DOUBLE dbCaliX = 0.0f, dbCaliY = 0, dbVertX = 0.0f;	/* �ݵ�� 0 ������ �ʱ�ȭ */
	LPG_CVMF pstMarkFind= &m_pstConfig->mark_find;
	LPG_ACCE pstCaliData= NULL;
	int idx=-1;
	/* �˻��� ��ũ ó�� ���� ������ �ʱ�ȭ */
	m_u8ResultAll	= 0x00;

	/* ----------------------------------------------------------------------------------- */
	/* img_id�� 0xff�� ���, Calibration Method�� ���� ���̹Ƿ�, �Ʒ� ��ƾ�� ����Ǹ� �ȵ� */
	/* �Ʒ� ���, �������� ����� �뱤(����� ī�޶� ���� ���� ���� ����)�� ��츸 ���� �� */
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
		/* Align Method ������ Each ������� ���� */
		u8MarkSeq	= img_id;
		/* Calibration Table (Position) ���� ���� */
		if (ENG_AOEM::en_calib_expose == m_enAlignMode && alignMotionPtr != nullptr)
		{
			auto status = alignMotionPtr->status;
			
			vector<STG_XMXY>& pool = status.markPoolForCam[m_u8ACamID];
			bool isGlobal = true;

			
			auto IndexOf = [&](vector<STG_XMXY>& pool, int idx,bool& isGlobal)->int //�����ؾ��Ѵ�. �׷� �� ��ũ�������� �� �����̳� �۷ι����� ���°�ΰ�?
			{
				int sum = 0;
				
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
			pstCaliData = isGlobal ? m_pstShMemVisi->cali_global[m_u8ACamID - 1][idx] : m_pstShMemVisi->cali_local[m_u8ACamID - 1][idx];

			////��¥ �� ����� �������� ¥����.
			//if (ENG_AOMI::en_each != ENG_AOMI(m_pstConfig->set_align.align_method))
			//{
			//	if (img_id < 0x02)
			//	{
			//		pstCaliData	= m_pstShMemVisi->cali_global[m_u8ACamID-1][u8MarkSeq];
			//	}
			//	else
			//	{
			//		u8MarkSeq	-= 0x02;	/* Local Mark Index */
			//		pstCaliData	= m_pstShMemVisi->cali_local[m_u8ACamID-1][u8MarkSeq];
			//	}
			//}
			//else
			//{
			//	if (img_id < 0x04)
			//	{
			//		pstCaliData	= m_pstShMemVisi->cali_global[m_u8ACamID-1][u8MarkSeq];
			//	}
			//	else
			//	{
			//		u8MarkSeq	-= 0x04;	/* Local Mark Index */
			//		pstCaliData	= m_pstShMemVisi->cali_local[m_u8ACamID-1][u8MarkSeq];
			//	}
			//}

			try
			{
				/* ���� ���� ���� ����Ǿ� �ִ� ���� �޸𸮿��� �� �������� */
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

	/* Sub �˻� ��� �� �ʱ�ȭ */
	memset(m_pstResultAll, 0x00, pstMarkFind->max_mark_find * sizeof(STG_GMSR));

	if (r_data)
	{
		//m_pstGrabResult->submaskFindInfo = m_pstResultAll;
		
		/* Grabbed Image�� ���� ���� ��� �� ���� (unit : pixel) */
		m_pstGrabResult->model_index	= (UINT8)r_data->model_index;
		m_pstGrabResult->mark_cent_px_x	= r_data->cent_x;
		m_pstGrabResult->mark_cent_px_y	= r_data->cent_y;
		m_pstGrabResult->cent_dist_x	= r_data->cent_dist_x;	/* ��ũ ���� �߽� ���� ������ �Ÿ� (����: um) */
		m_pstGrabResult->cent_dist_y	= r_data->cent_dist_y;	/* ��ũ ���� �߽� ���� ������ �Ÿ� (����: um) */
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
		/* Score ���� �ִ� �͸�, �ᱹ ã�� �͸� ... */
		if (r_data->score_rate > 0.0f)
		{
			/* ------------------------------------------------------------------------------------- */
			/* grabbed image�� �߽ɿ��� �˻��� Mark�� �߽ɰ��� �Ÿ��� ��ŭ ������ �ִ��� ��� (mm) */
			/* !!! �߿���. �̹��� �߽ɿ��� Mark�� �߽� ���� �� �� �ٽ� ��� ���� ���� �� �ο���) !!! */
			/* ------------------------------------------------------------------------------------- */
			m_pstGrabResult->move_px_x = (m_u32ImgWidth / 2.0f - r_data->cent_x);
			m_pstGrabResult->move_px_y = (m_u32ImgHeight / 2.0f- r_data->cent_y);
			/* ����� ī�޶� 180�� ȸ�� ���ο� �ε�ȣ �ݴ� ���� ��� */
			if (m_pstConfig->set_cams.acam_inst_angle)
			{
				m_pstGrabResult->move_px_x	*= -1.0f;
				m_pstGrabResult->move_px_y	*= -1.0f;
			}
#if 0
			/* Stage�� ���������� �̵��ϸ鼭 ������ ��� */
			if (m_bStageMoveDirect)	m_pstGrabResult->move_px_x	*= -1.0f;
			if (m_bStageMoveDirect)	m_pstGrabResult->move_px_y	*= -1.0f;
#endif
			m_pstGrabResult->move_mm_x = m_pstGrabResult->move_px_x * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1); 
			m_pstGrabResult->move_mm_y = m_pstGrabResult->move_px_y * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1);
			m_pstGrabResult->circle_radius = r_data->circle_radius * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1);

			/* ��ũ ���̺��� ���� �� ���� */
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
				//m_pstGrabResult->move_px_y += (dbCaliY / m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID - 1));
				//m_pstGrabResult->move_mm_y += dbCaliY;
				for (i=0; i<o_count && o_data; i++)
				{
					m_pstResultAll[i].cent_x += (dbVertX / m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1));
					m_pstResultAll[i].cent_x -= (dbCaliX / m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1));
					//m_pstResultAll[i].cent_y -= (dbCaliY / m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1));
					m_pstResultAll[i].cent_y += (dbCaliY / m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1));
				}
			}
#endif
			/* �̹����� �߽ɿ��� �˻��� Mark ������ ������ �Ÿ� ��� */
			m_pstGrabResult->mark_cent_mm_dist = r_data->cent_dist * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1);	/* �� ���� �߿����� ����. X or Y �� �����ص� ����. ���� ������ ���� */
			/* ȸ���� ������ �˻��� ��Ī ���� �� */
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
				m_pstGrabResult->valid_score = r_data->score_rate >= (m_dbRecipeScale) ? 0x01 : 0x00;
			}
			m_bMatched	= m_pstGrabResult->IsMarkValid();
		}
		/* �˻� ������ ��� */
		else
		{
			/* �˻��� �̹����� �߽� ���� Mark�� �߽����� �ϴ� ���� */
			m_pstGrabResult->mark_cent_px_x = m_pstConfig->set_cams.aoi_size[0] / 2.0f;
			m_pstGrabResult->mark_cent_px_y = m_pstConfig->set_cams.aoi_size[1] / 2.0f;
		}
	}
	/* �˻� ����� Mark Size */
	if (r_data)
	{
		m_pstGrabResult->mark_width_px	= r_data->mark_width;
		m_pstGrabResult->mark_height_px	= r_data->mark_height;
		m_pstGrabResult->mark_width_mm	= r_data->mark_width * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1);
		m_pstGrabResult->mark_height_mm	= r_data->mark_height * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1);
	}
	/* Grabbed Image�� ���� ���� ��� �� ���� (unit : mm) */
	m_pstGrabResult->mark_cent_mm_x = m_pstGrabResult->mark_cent_px_x * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1);
	m_pstGrabResult->mark_cent_mm_y = m_pstGrabResult->mark_cent_px_y * m_pstConfig->acam_spec.GetPixelToMM(m_u8ACamID-1);
}

/*
 desc : ���� ������ ���� (�ܺο��� ��ũ �ν� ���� ���, �������� �ν��ϵ��� ó��)
 parm : grab		- [in]  ����ڿ� ���� �������� �Էµ� grabbed image ��� ������ ����� ����ü ������
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
 desc : ���� Grabbed Image �˻��� ����� �̹��� ���Ϸ� ����
 parm : img_id	- [in]  Camera Grabbed Image Index (0 or Later) (if img_id == 0xff then Calibration Method)
		r_data	- [in]  ���� �˻� �� ���� ��
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
	/* ������ ���� */
	if (r_data)
	{
		swprintf_s(tzResult, 32, L"(%6.3f_%6.3f_%u).bmp",
					r_data->score_rate, r_data->scale_rate, r_data->scale_size);
	}
	else
	{
		swprintf_s(tzResult, 32, L"(00.000_00.000_0).bmp");
	}
	/* Set the full file name */
	swprintf_s(tzSrc, MAX_PATH_LEN, L"%s\\save_img\\mark\\%s_%d_%02d(%s)_%s.bmp",
			   g_tzWorkDir, tzTime, m_u8ACamID, img_id, tzResult, tzTime);

#ifndef _NOT_USE_MIL_
	/* Grabbed Image ���� */
	MbufExport(tzSrc, M_BMP, m_mlBufID);
#endif
}

/*
 desc : ���� ����� �������� ��ũ �˻� ���� �� ����
 parm : score	- [in]  Score Accpetance (��� �̹������� �˻��� ��ũ�� Score ���� �� ������ ������ ������)
		scale	- [in]  Scale Range (��� �̹������� ��ϵ� ��ũ�� �˻� ����� �߿��� �� ���� ���� ���� )
									(���Ե� ����鸸 ��ȿ�ϰ�, �� �߿��� Score ���� ���� ���� �͸� ����)
									(����. �� ���� 50.0000 �̸�, ���� �˻��� ���Ǵ� ��ũ�� ũ�� ����)
									(�˻����� = 1.0f - (100.0 - 50.0) / 100 �� ��, 0.5 ~ 1.5 ��)
 retn : None
*/
VOID CMilGrab::SetRecipeMarkRate(DOUBLE score, DOUBLE scale)
{
	m_dbRecipeScore	= score;
	m_dbRecipeScale	= scale;
}