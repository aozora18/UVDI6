
/*
 desc : The Interface Library for Align Camera (Basler or IDS)
*/

#include "pch.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */


#ifdef __cplusplus
extern "C"
{
#endif

/*
/* --------------------------------------------------------------------------------------------- */
/*                        외부 함수 - < Camera - Common > < for Engine >                         */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Align Camera 재접속 수행
 parm : None
 retn : None
*/
API_EXPORT VOID uvEng_Camera_Reconnect()
{
	uvBasler_Reconnected();
}

/*
 desc : BIPS Server와 통신을 담당하는 Client 스레드 생성 및 실행
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Camera_UpdateSetParam()
{
	return uvBasler_UpdateSetParam(g_tzWorkDir);
}

/*
 desc : Grabbed Image가 존재하면, Edge Detection 수행
 변수 :	cam_id	- [in]  Camera Index (1 or 2)
 retn : Edge Detected된 영역의 직경 (지름) 값 반환 (단위: Pixel)
*/
API_EXPORT BOOL uvEng_Camera_RunEdgeDetect(UINT8 cam_id)
{
	UINT8 u8Saved	= g_pMemConf->GetMemMap()->edge_find.edge_analysis_save;
	return uvBasler_RunEdgeDetect(cam_id, u8Saved);
}

/*
 desc : Set the parameters of the strip maker and find in an image and take the specified measurements
 parm : cam_id	- [in]  Align Camera ID (1 or 2 or later)
		param	- [in]  Structures pointer with information to set up is stored
		result	- [out] Buffer in which the resulting value will be returned and stored.
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Camera_RunMarkerStrip(UINT8 cam_id, LPG_MSMP param, STG_MSMR &results)
{
	return uvBasler_RunMarkerStrip(cam_id, param, results);
}

/*
 desc : Edge Detection 검색된 개수 반환
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
 변환 : 개수 반환. 실패할 경우, 0 이하 값
*/
API_EXPORT INT32 uvEng_Camera_GetEdgeDetectCount(UINT8 cam_id)
{
	return uvBasler_GetEdgeDetectCount(cam_id);

	return FALSE;
}

/*
 desc : 모델 크기 반환
 parm : cam_id	- [in]  Grabbed Image 정보가 발생된 Align Camera Index (1 or 2)
		index	- [in]  요청하고자 하는 모델의 위치 (Zero-based)
		flag	- [in]  0x00 : 가로  크기, 0x01 : 세로 크기
		unit	- [in]  0x00 : um, 0x01 : pixel
 retn : 크기 반환 (단위: um)
*/
API_EXPORT DOUBLE uvEng_Camera_GetMarkModelSize(UINT8 cam_id, UINT8 index, UINT8 flag, UINT8 unit)
{
	return uvBasler_GetMarkModelSize(cam_id, index, flag, unit);
}

/*
 desc : Edge Detection 검색된 결과 반환
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
 retn : 결과 값이 저장된 포인터 반환
*/
API_EXPORT LPG_EDFR uvEng_Camera_GetEdgeDetectResults(UINT8 cam_id)
{
	return uvBasler_GetEdgeDetectResults(cam_id);
}

/*
 desc : Edge Detection 검색된 결과 값 중에서 찾고자 하는 객체 크기와 가장 근사한 값 반환
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
		diameter- [in]  검색하고자 하는 크기 값 (단위: mm)
 retn : 결과 값이 저장된 포인터 반환
*/
API_EXPORT LPG_EDFR uvEng_Camera_GetEdgeDetectResultSearch(UINT8 cam_id, DOUBLE diameter)
{
	UINT8 i	= 0, u8Find	= 0xff;
	LPG_EDFR pstResult	= NULL;
	DOUBLE dbFind		= 99999999.0f;	/* 무조건 큰 값*/

	for (; i<g_pMemConf->GetMemMap()->edge_find.max_find_count; i++)
	{
		if (!g_pMemVisi->GetMemMap()->edge_result[i]->set_data)	break;
		/* 검색 대상과 유사한 크기 인지 비교 */
		pstResult	= g_pMemVisi->GetMemMap()->edge_result[i];
		if (abs(pstResult->dia_meter_um - diameter * 1000.0) < dbFind)
		{
			dbFind	= pstResult->dia_meter_um	/ 1000.0f;	/* um --> mm */
			u8Find	= i;	/* 검색된 인덱스 위치 반환 */
		}
	}

	/* 검색된 위치 값 반환 */
	if (u8Find != 0xff)	return g_pMemVisi->GetMemMap()->edge_result[u8Find];
	/* 검색 실패 값 반환 */
	return NULL;
}

/*
 desc : Geometric Model Find
 parm : cam_id	- [in]  Camera Index (1 or 2)
 retn : TRUE or FALSE
*/
//API_EXPORT BOOL uvEng_Camera_RunModelFind(UINT8 cam_id, UINT8 mark_no) // 미사용
//{
//	/* 검색 작업 진행 */
//	return uvBasler_RunModelFind(cam_id, mark_no);
//}

/*
 desc : 가장 최근 Grabbed Image의 매칭 검색 결과 구조체 포인터 반환
 parm : None
 retn : 객체 포인터 (매칭이 실패한 경우 NULL)
*/
API_EXPORT LPG_ACGR uvEng_Camera_GetLastGrabbedMark()
{
	return uvBasler_GetLastGrabbedMark();
}

/*
 desc : 가장 최근 Grabbed Image의 매칭 검색 결과 구조체 포인터 반환
 parm : None
 retn : 객체 포인터 (매칭이 실패한 경우 NULL)
 이력 : 2021-05-25 (Modified by JinSoo.Kang: First released)
*/
API_EXPORT LPG_ACGR uvEng_Camera_GetLastGrabbedMarkEx()
{
	return uvBasler_GetLastGrabbedMarkEx();
}

/*
 desc : 가장 최근에 카메라 별로 Calibrated된 결과 중 Error 값이 저장된 데이터 반환
 parm : None
 retn : 매칭된 결과 값 반환
 이력 : 2021-05-25 (Modified by JinSoo.Kang: First released)
*/
API_EXPORT LPG_ACLR uvEng_Camera_GetLastGrabbedACam()
{
	return uvBasler_GetLastGrabbedACam();
}
#if 0
/*
 desc : 기존 Grabbed Data 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvEng_Camera_ResetGrabbedMark()
{
	uvBasler_ResetGrabbedMark();
}
#endif
/*
 desc : 기존 Grabbed Image 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvEng_Camera_ResetGrabbedImage()
{
	uvBasler_ResetGrabbedImage();
}


API_EXPORT bool uvEng_Camera_RemoveLastGrab(int camIdx)
{
	return uvBasler_RemoveLastGrab(camIdx);
}


API_EXPORT bool  uvEng_Camera_SWGrab(int camIdx)
{
	return uvBasler_Camera_SWGrab(camIdx);
}


API_EXPORT ENG_TRGM uvEng_Camera_GetTriggerMode(int camIdx)
{
	return uvBasler_GetTriggerMode(camIdx);
}

API_EXPORT VOID uvEng_Camera_TriggerMode(int camIdx, ENG_TRGM mode)
{
	uvBasler_ChangeTriggerMode(camIdx,mode);
}



/*
 desc : Vision Camera Mode 설정
 parm : mode	- [in]  ENG_VCCM::en_xxx
 retn : None
*/
API_EXPORT VOID uvEng_Camera_SetCamMode(ENG_VCCM mode)
{
	uvBasler_SetCamMode(mode);
}

/*
 desc : 현재 Vision Camera Mode 반환 (동작 값)
 parm : None
 retn : 동작 모드 값 반환
*/
API_EXPORT ENG_VCCM uvEng_Camera_GetCamMode()
{
	return uvBasler_GetCamMode();
}

/*
 desc : 현재 Vision Camera Mode가 Live View Mode인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Camera_IsCamModeLive()
{
	return (uvBasler_GetCamMode() == ENG_VCCM::en_live_mode);
}

/*
 desc : 현재 Vision Camera Mode가 Calibration View Mode인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Camera_IsCamModeCali()
{
	return (uvBasler_GetCamMode() == ENG_VCCM::en_cali_mode);
}

/*
 desc : 현재 Vision Camera Mode가 Grabbed View Mode인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Camera_IsCamModeGrab()
{
	return (uvBasler_GetCamMode() == ENG_VCCM::en_grab_mode);
}

/*
 desc : 가장 최근에 Edge Detection된 이미지에서 특정 영역만 추출해서 Bitmap Image로 등록
		즉, Mark Template 이미지로 등록
 parm : cam_id	- [in]  Align Camera ID (1 or Later)
		area	- [in]  추출하고자 하는 영역 정보가 저장된 픽셀 위치 값
		file	- [in]  Mark Template를 저장하려는 파일 이름 (전체 경로 포함)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Camera_SaveGrabbedMarkToFile(UINT8 cam_id, LPRECT area, UINT8 type, PTCHAR file)
{
	return uvBasler_SaveGrabbedMarkToFile(cam_id, area, type, file);
}


/*
 desc : 현재 Grabbed된 이미지 출력 및 저장 (Bitmap을 이용하여 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		cam_id	- [in]  Camera Index (1 or 2)
 retn : None
*/
API_EXPORT VOID uvEng_Camera_DrawGrabBitmap(HDC hdc, RECT draw, UINT8 cam_id)
{
	ENG_VCPK enACamKind	= (ENG_VCPK)GetConfig()->set_comn.align_camera_kind;
	switch (enACamKind)
	{
	case ENG_VCPK::en_camera_basler_ipv4	:	uvBasler_DrawGrabBitmap(hdc, draw, cam_id);	break;
//	case ENG_VCPK::en_camera_basler_ipv4	:	uvIDS_DrawLiveBitmap(hwnd, save);			break;
	}
}

/*
 desc : Draw the strip information in the current grabbed image
 parm : hdc		- [in]  The handle of context
		draw	- [in]  The area in which the image is output
		cam_id	- [in]  Camera Index (1 or 2)
		param	- [in]  Strip output information
 retn : None
*/
API_EXPORT VOID uvEng_Camera_DrawStripBitmap(HDC hdc, RECT draw, UINT8 cam_id, LPG_MSMP param)
{
	ENG_VCPK enACamKind	= (ENG_VCPK)GetConfig()->set_comn.align_camera_kind;
	switch (enACamKind)
	{
	case ENG_VCPK::en_camera_basler_ipv4	:	uvBasler_DrawStripBitmap(hdc, draw, cam_id, param);	break;
//	case ENG_VCPK::en_camera_ids_ph			:	uvIDS_DrawLiveBitmap(hwnd, save);			break;
	}
}


/*
 desc : Grabbed Mark의 처리된 개수 반환
 변수 :	None
 retn : 처리된 마크 개수 반환
*/
API_EXPORT UINT16 uvEng_Camera_GetGrabbedCount(int* camNum )
{
	if (!GetConfig())	return 0;
	ENG_VCPK enACamKind	= (ENG_VCPK)GetConfig()->set_comn.align_camera_kind;
	switch (enACamKind)
	{
	case ENG_VCPK::en_camera_basler_ipv4	:	return uvBasler_GetGrabbedCount(camNum);
//	case ENG_VCPK::en_camera_basler_ipv4	:	uvIDS_DrawLive(hwnd);
	}
	return 0;
}

/*
 desc : 로컬 시스템 (저장소)에 저장되어 있는 패턴 매칭 원본 이미지 적재
 parm : cam_id		- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		detail_level- [in]  등록된 모델 이미지와 Grabbed Image이미지의 Edge 영역에 대한 비교 정도 값
							값이 클수록 상세히 비교하지만, 속도는 많이 느려지짐
							LOW보다 HIGH로 갈수록 더 많은 Edge를 가지고 비교하게 됨
							0 (M_MEDIUM), 1 (M_HIGH), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - 기본 값, 1.0f ~ 100.0f
							기본 값은 50이지만, 100으로 갈수록 엣지의 강도가 센것만 나타납니다.
		model		- [in]  Model Type 즉, circle, square, rectangle, cross, diamond, triangle
		param		- [in]  총 5개의 Parameter Values (unit : um)
		count		- [in]  등록하고자 하는 모델의 개수	
		아래 2개의 값이 0 값이면 파라미터 적용되지 않음
		scale_min	- [in]  검색 대상의 크기 범위 값 설정 (최소한 이 값 비율보다 커야 됨. 범위: 0.5 ~ 1.0)
		scale_max	- [in]  검색 대상의 크기 범위 값 설정 (최대한 이 값 비율보다 작아야 됨. 범위: 1.0 ~ 1.0)
		score_min	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0)
		score_tgt	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0))
		name		- [in]  Model Name
		file		- [in]  모델 이미지가 저장된 파일 이름 (전체 경로 포함. Image File)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Camera_SetModelDefine(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
											LPG_CMPV model, UINT8 mark_no,
											DOUBLE scale_min, DOUBLE scale_max,
											DOUBLE score_min, DOUBLE score_tgt)
{
	return uvBasler_SetModelDefine(cam_id, speed, level, count, smooth, model, mark_no,
								   scale_min, scale_max, score_min, score_tgt);
}
API_EXPORT BOOL uvEng_Camera_SetModelDefineEx(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count,
	DOUBLE smooth, PUINT32 model, DOUBLE* param1,
	DOUBLE* param2, DOUBLE* param3, DOUBLE* param4,
	DOUBLE* param5, UINT8 mark_no, DOUBLE scale_min,
	DOUBLE scale_max, DOUBLE score_min, DOUBLE score_tgt, bool sameMark)
{
	return uvBasler_SetModelDefineEx(cam_id, speed, level, count, smooth, model,
		param1, param2, param3, param4, param5, mark_no,
		scale_min, scale_max, score_min, score_tgt, sameMark);
}
API_EXPORT BOOL uvEng_Camera_SetModelDefineLoad(UINT8 cam_id, UINT8 speed, UINT8 level, DOUBLE smooth,
												DOUBLE scale_min, DOUBLE scale_max,
												DOUBLE score_min,DOUBLE score_tgt,
												PTCHAR name, CStringArray &file)
{
	return uvBasler_SetModelDefineLoad(cam_id, speed, level, smooth,
									   scale_min, scale_max, score_min, score_tgt, name, file);
}

/*
 desc : Mark 정보 설정 - MMF (MIL Model Find File)
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
		name	- [in]  Model Name
		file	- [in]  모델 이미지 (MMF)가 저장된 파일 이름 (전체 경로 포함. 확장자 mmf)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Camera_SetModelDefineMMF(UINT8 cam_id, PTCHAR name, PTCHAR mmf, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no)
{
	return uvBasler_SetModelDefineMMF(cam_id, name, mmf, m_MarkSizeP, m_MarkCenterP, mark_no);
}

/*
 desc : Mark 정보 설정 - PAT (MIL PAT Find File)
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
		name	- [in]  Model Name
		file	- [in]  모델 이미지 (PAT)가 저장된 파일 이름 (전체 경로 포함. 확장자 pat)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Camera_SetModelDefinePAT(UINT8 cam_id, PTCHAR name, PTCHAR pat, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no)
{
	return uvBasler_SetModelDefinePAT(cam_id, name, pat, m_MarkSizeP, m_MarkCenterP, mark_no);
}

/*
 desc : Calibration Image가 존재하면, Align Mark 수행
 변수 :	cam_id	- [in]  Camera Index (1 or 2)
		mode	- [in]  0xff : Calibration Mode (얼라인 카메라 각도 측정하지 않음)
						0xfe : Align Camera Angle Measuring Mode (얼라인 카메라 각도 측정하기 위함)
 retn : Calirbated Image의 정보가 저장된 구조체 포인터 반환
*/
API_EXPORT LPG_ACGR uvEng_Camera_RunModelCali(UINT8 cam_id, UINT8 mode, UINT8 dlg_id, UINT8 mark_no, BOOL useMilDisp, UINT8 img_proc,int flipDir) // default : UINT8 mode=0xff
{
	return uvBasler_RunModelCali(cam_id, mode, dlg_id, mark_no, useMilDisp, img_proc,flipDir);
}

/*
 desc : Calibration 이미지 (검색된 결과)를 윈도 영역에 출력 수행 (Bitmap을 이용하여 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
 retn : None
*/
API_EXPORT VOID uvEng_Camera_DrawCaliBitmap(HDC hdc, RECT draw)
{
	uvBasler_DrawCaliMarkBitmap(hdc, draw);
}

API_EXPORT VOID uvEng_Camera_SetAlignMotionPtr(AlignMotion& ptr)
{
	uvBasler_Camera_SetAlignMotionPtr(ptr);
}


/*
 desc : Outputs the most recently generated Buffer ID value to the window
 parm : hwnd	- [in]  Handle of window where the image is output
		draw	- [in]  The area of window where the image is output
		cam_id	- [in]  Align Camera ID (1 or 2)
		type	- [in]  0x00: Edge Detection, 0x01: Line Detection, 0x02: Strip Detection (Measurement)
 retn : None
*/
API_EXPORT VOID uvEng_Camera_DrawMBufID(HWND hwnd, RECT draw, UINT8 cam_id, UINT8 type)
{
	uvBasler_DrawMBufID(hwnd, draw, cam_id, type);
}

/*
 desc : 검색된 Mark 이미지 윈도 영역에 출력 수행 (DC를 이용하여 bitmap 이미지로 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Camera_DrawMarkBitmap(HDC hdc, RECT draw, UINT8 cam_id, UINT8 img_id)
{
	return uvBasler_DrawMarkBitmap(hdc, draw, cam_id, img_id);
}

/*
 desc : 검색된 Mark 이미지 윈도 영역에 출력 수행 (MIL Buffer ID를 이용한 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Camera_DrawMarkMBufID(HWND hwnd, RECT draw, UINT8 cam_id, UINT8 hwndIdx,UINT8 img_id)
{
	return uvBasler_DrawMarkMBufID(hwnd, draw, cam_id, hwndIdx,img_id);
}

/*
 desc : Calibration 이미지 윈도 영역에 출력 수행 (Bitmap을 이용하여 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		grab	- [in]  grab 이미지가 저장된 구조체 포인터 (grab_data에 이미지가 정보가 반드시 저장되어 있어야 됨)
		find	- [in]  마크 검색 성공 여부 (검색됐더라도, 검색 결과 값에 따라 달라짐)
						0x00 - 검색 결과 실패, 0x01 - 검색 결과 성공
 retn : None
*/
API_EXPORT VOID uvEng_Camera_DrawMarkDataBitmap(HDC hdc, RECT draw, LPG_ACGR grab, UINT8 find,bool drawForce, UINT8 flipFlag)
{
	uvBasler_DrawMarkDataBitmap(hdc, draw, grab, find,drawForce, flipFlag);
}

/*
 desc : Drawing - Examination Object Image (Bitmap을 이용하여 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		grab_out- [in]  Grabbed Image 정보가 저장된 구조체 포인터 (바깥 링)
		grab_in	- [in]  Grabbed Image 정보가 저장된 구조체 포인터 (안쪽 원)
 retn : None
*/
API_EXPORT VOID uvEng_Camera_DrawGrabExamBitmap(HDC hdc, RECT draw, LPG_ACGR grab_out, LPG_ACGR grab_in)
{
	uvBasler_DrawGrabExamBitmap(hdc, draw, grab_out, grab_in);
}

/*
 desc : Mark Template가 등록되어 있는지 확인
 parm : mode	- [in]  0x00 : Expose, 0x01 : Vstep
		cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : TRUE - 등록되어 있다. FALSE - 등록되지 않았다.
*/
API_EXPORT BOOL uvEng_Camera_IsSetMarkModel(UINT8 mode, UINT8 cam_id, UINT8 fi_No)
{
	return uvBasler_IsSetMarkModel(mode, cam_id, fi_No);
}

/*
 desc : Mark Pattern 등록 여부
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Camera_IsSetMarkModelACam(UINT8 cam_id, UINT8 mark_no)
{
	return uvBasler_IsSetMarkModelACam(cam_id, mark_no);
}

/*
 desc : Step (Vision 단차) Image가 존재하면, Align Mark 수행
 변수 :	cam_id	- [in]  Camera Index (1 or 2)
		counts	- [in]  검색 대상의 Mark 개수 (찾고자하는 마크 개수)
		angle	- [in]  각도 적용 여부 (TRUE : 각도 적용, FALSE : 각도 적용 없음)
						TRUE : 현재 카메라의 회전된 각도 구하기 위함, FALSE : 기존 각도 적용하여 회전된 각도 구함
		results	- [out] 검색된 결과 정보가 저장될 구조체 참조 포인터 (grabbed image buffer까지 복사해옴)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Camera_RunModelStep(UINT8 cam_id, UINT16 count, BOOL angle, LPG_ACGR results, UINT8 dlg_id, UINT8 mark_no, UINT8 img_proc)
{
	return uvBasler_RunModelStep(cam_id, count, angle, results, dlg_id, mark_no, img_proc);
}

/*
 desc : Examination (Vision Align 검사 (측정)) Image가 존재하면, Align Shutting 검사 수행
 변수 :	results		- [out] 검색된 결과 정보가 저장될 구조체 참조 포인터 (grabbed image buffer까지 복사해옴)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Camera_RunModelExam(LPG_ACGR results) // 미사용
{
	DOUBLE dbScore	= g_pMemConf->GetMemMap()->acam_cali.score_valid_rate;
	DOUBLE dbScale	= g_pMemConf->GetMemMap()->acam_cali.scale_valid_rate;
	
	if (GetConfig()->IsRunDemo())	return TRUE;

	return uvBasler_RunModelExam(dbScore, dbScale, results);
}

/*
 desc : Grabbed Images 모두 Score 값이 유효한지 값 반환
 parm : set_score	- [in]  If the score of the searched mark is higher than this set score, it is valid
  							(If 0.0f is unchecked) 
 retn : TRUE : 유효함, FALSE : 유효하지 않음
 이력 : 2021-05-25 (Modified by JinSoo.Kang: First released)
*/
API_EXPORT BOOL uvEng_Camera_IsScoreValidAll(DOUBLE set_score)
{
	return uvBasler_IsScoreValidAll(set_score);
}

/*
 desc : Global Mark 대상으로, 2 개의 검색된 마크 정보 반환
 parm : direct	- [in]  측정지점 (0 - 1번 마크와 3번 마크 간의 길이 값)
								 (1 - 2번 마크와 4번 마크 간의 길이 값)
								 (2 - 1번 마크와 2번 마크 간의 길이 값)
								 (3 - 3번 마크와 4번 마크 간의 길이 값)
								 (4 - 1번 마크와 4번 마크 간의 길이 값)
								 (5 - 2번 마크와 3번 마크 간의 길이 값)
		data1	- [out] 검색된 첫 번째 마크의 중심 위치에서 오차 값 (단위: mm)
		data2	- [out] 검색된 두 번째 마크의 중심 위치에서 오차 값 (단위: mm)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Camera_GetGrabbedMarkDirect(ENG_GMDD direct, STG_DBXY &data1, STG_DBXY &data2)
{
	return uvBasler_GetGrabbedMarkDirect(direct, data1, data2);
}

/*
 desc : 전역 기준점 : 검색된 X 축 기준으로 2 개의 마크가 떨어진 위치 반환 (단위 : mm)
 parm : type	- [in]  X (노광) 축의 마크 간격 (0 - 1번 마크와 3번 마크 간의 길이 값)
												(1 - 2번 마크와 4번 마크 간의 길이 값)
												(2 - 1번 마크와 2번 마크 간의 길이 값)
												(3 - 3번 마크와 4번 마크 간의 길이 값)
												(4 - 1번 마크와 4번 마크 간의 길이 값)
												(5 - 2번 마크와 3번 마크 간의 길이 값)
 retn : X 축의 2개의 마크 떨어진 간격 (단위: mm) 반환
*/
API_EXPORT DOUBLE uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD direct)
{
	return uvBasler_GetGrabbedMarkDist(direct);
}

/*
 desc : Global 4개의 Mark에 대해서 모두 유효한지 검사
 parm : multi_mark	- [in]  Multi Mark (다중 마크) 적용 여부
		set_score	- [in]  If the score of the searched mark is higher than this set score, it is valid
		 					(If 0.0f is unchecked) 
 retn : TRUE (유효함) or FALSE (4개 중 1개라도 제대로 인식 안됨)
*/
API_EXPORT BOOL uvEng_Camera_IsGrabbedMarkValidAll(BOOL multi_mark, DOUBLE set_score, int* camNum)
{
	if (GetConfig()->IsRunDemo())	return TRUE;
	return uvBasler_IsGrabbedMarkValidAll(multi_mark, set_score,camNum);
}

/*
 desc : Grabbed Image의 결과 반환
 parm : cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : grab data 반환
*/
API_EXPORT LPG_ACGR uvEng_Camera_GetGrabbedMark(UINT8 cam_id, UINT8 img_id)
{
	return uvBasler_GetGrabbedMark(cam_id, img_id);
}


/*
 desc : Grabbed Image의 결과 반환
 retn : grab data 반환
*/
API_EXPORT CAtlList <LPG_ACGR>* uvEng_Camera_GetGrabbedMarkAll()
{
	return uvBasler_GetGrabbedMarkAll();
}

API_EXPORT BOOL uvEng_Camera_TryEnterCS()
{
	return uvBasler_TryEnterCS();
}

API_EXPORT VOID uvEng_Camera_ExitCS()
{
	uvBasler_ExitCS();
}





/*
 desc : Grabbed Image의 데이터 반환
 parm : index	- [in]  가져오고자 하는 위치 (Zero based)
 retn : grab data 반환
*/
API_EXPORT LPG_ACGR uvEng_Camera_GetGrabbedMarkIndex(UINT8 index)
{
	return uvBasler_GetGrabbedMarkIndex(index);
}

/*
 desc : Grabbed Image의 결과 수정
 parm : grab	- [in]  저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Camera_SetGrabbedMark(LPG_ACGR grab)
{
	return uvBasler_SetGrabbedMark(grab);
}

/*
 desc : 사용자에 의해 수동으로 검색된 경우, 결과 값만 처리
 parm : grab	- [in]  사용자에 의해 수동으로 입력된 grabbed image 결과 정보가 저장된 구조체 포인터
		gmfr	- [in]  GMFR Data
		gmsr	- [in]  GMSR Data
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Camera_SetGrabbedMarkEx(LPG_ACGR grab, LPG_GMFR gmfr, LPG_GMSR gmsr)
{
	return uvBasler_SetGrabbedMarkEx(grab, gmfr, gmsr);
}

/*
 desc : Align Mark 검색 방식 설정
 parm : method	- [in]  0x00 : 최적의 1개 반환, 0x01 : 다점 마크 기준으로 가중치를 부여하여 결과 반환
		count	- [in]  'method' 값이 1인 경우, 최종 검색될 Mark 개수 값 2 이상 값이어야 됨
 retn : None
*/
API_EXPORT VOID uvEng_Camera_SetMarkMethod(ENG_MMSM method, UINT8 count)
{
	return uvBasler_SetMarkMethod(method, count);
}


API_EXPORT UINT8 uvEng_SetMarkFoundCount(int camNum)
{
	return uvBasler_SetMarkFoundCount(camNum);
}


/*
 desc : 노광 모드 설정 즉, 직접 노광, 얼라인 노광, 보정 후 얼라인 노광
 parm : mode	- [in]  직접 노광 (0x00), 얼라인 노광 (0x01), 얼라인 카메라 보정 값 적용 후 얼라인 노광 (0x02)
 retn : None
*/
API_EXPORT VOID uvEng_Camera_SetAlignMode(ENG_AOEM mode)
{
	uvBasler_SetAlignMode(mode);
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
API_EXPORT VOID uvEng_Camera_SetRecipeMarkRate(DOUBLE score, DOUBLE scale)
{
	return uvBasler_SetRecipeMarkRate(score, scale);
}

/*
 desc : 스테이지의 이동 방향 설정 (정방향 이동인지 / 역방향 이동인지 여부)
 parm : direct	- [in]  TRUE (정방향 이동 : 앞에서 뒤로 이동), FALSE (역방향 이동)
 retn : None
*/
API_EXPORT VOID uvEng_Camera_SetMoveStateDirect(BOOL direct)
{
	uvBasler_SetMoveStateDirect(direct);
}

/*
 desc : 얼라인 카메라의 Gain Level 값 설정
 parm : cam_id	- [in]  Camera Index (1 or 2)
		level	- [in] 0 ~ 255 (값이 클수록 밝게 처리)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Camera_SetGainLevel(UINT8 cam_id, UINT8 level)
{
	if (GetConfig()->IsRunDemo())	return TRUE;
	return uvBasler_SetGainLevel(cam_id, level);
}

/*
 desc : 수동으로 이미지를 불러와서 데이터를 적재 합니다. (데모 모드에서 주로 사용 됨)
 parm : cam_id	- [in]  Camera Index (1 or 2)
		file	- [in]  전체 경로가 포함됨 265 grayscale bitmap 파일
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Camera_LoadImageFromFile(UINT8 cam_id, PTCHAR file)
{
	return uvBasler_LoadImageFromFile(cam_id, file);
}

/*
 desc : Grabbed Image 마다 표현된 마크의 개수가 여러 개인 경우, 마크 들이 분포된 영역의 크기 값 설정
 parm : width	- [in]  사각형 영역의 넓이 값 (단위: um)
		height	- [in]  사각형 영역의 높이 값 (단위: um)
 retn : None
*/
API_EXPORT VOID uvEng_Camera_SetMultiMarkArea(UINT32 width, UINT32 height)
{
	uvBasler_SetMultiMarkArea(width, height);
}

/* --------------------------------------------------------------------------------------------- */
/*                        외부 함수 - < Camera - Common > < for Common >                         */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 현재 PLC가 연결되어 있는지 여부 확인
 변수 :	cam_id	- [in]  Align Camera Index (1 or 2)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_Camera_IsConnected()
{
	UINT8 i	= 0x00;

	for (; i<GetConfig()->set_cams.acam_count; i++)
	{
		if (!GetShMemVisi()->link[i]->is_connected)	return FALSE;
	}

	return TRUE;
}
API_EXPORT BOOL uvCmn_Camera_IsConnectedACam(UINT8 cam_id)
{
	if (!GetConfig())	return FALSE;
	if (GetConfig()->set_cams.acam_count < cam_id)	return FALSE;

	return GetShMemVisi()->link[cam_id-1]->is_connected;
}

/*
 desc : Align Camera의 Z 축 Limit (Up/Down)를 벗어났는지 여부
 parm : pos	- [in]  이동하고자 하는 Z 축 위치 값 (단위: mm)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_Camera_IsZPosUpDownLimit(DOUBLE pos)
{
	BOOL bValid	= TRUE;

	if (GetConfig()->set_basler.z_axis_move_min > pos)	bValid	= FALSE;
	if (GetConfig()->set_basler.z_axis_move_max < pos)	bValid	= FALSE;

	return bValid;
}

/* ----------------------------------------------------------------------------------------- */
/*                                 lk91 VISION 추가 함수                                     */
/* ----------------------------------------------------------------------------------------- */
/* desc : 최종 검색된 Live 이미지 윈도 영역에 출력 수행 (Bitmap을 이용하여 출력) */
API_EXPORT VOID uvEng_Camera_DrawLiveBitmap(HDC hdc, RECT draw, UINT8 cam_id, BOOL save)
{
	uvBasler_DrawLiveBitmap(hdc, draw, cam_id, save);
}

/* desc : MIL ID 로 저장된 이미지 화면 출력 */
API_EXPORT VOID uvEng_Camera_DrawImageBitmap(int dispType, int Num, UINT8 cam_id, BOOL save,int flipDir) 
{
	uvBasler_DrawImageBitmap(dispType, Num, cam_id, save,flipDir);
}

/* desc : Set Live Disp Size */
API_EXPORT VOID uvEng_Camera_SetMarkLiveDispSize(CSize fi_size)
{
	uvBasler_SetMarkLiveDispSize(fi_size);
}

/* desc : Set Calb CamSpec Disp Size */
API_EXPORT VOID uvEng_Camera_SetCalbCamSpecDispSize(CSize fi_size)
{
	uvBasler_SetCalbCamSpecDispSize(fi_size);
}

API_EXPORT VOID uvEng_Camera_SetAccuracyMeasureDispSize(CSize fi_size)
{
	uvBasler_SetAccuracyMeasureDispSize(fi_size);
}

/* desc : Set CalbStep Disp Size */
API_EXPORT VOID uvEng_Camera_SetCalbStepDispSize(CSize fi_size)
{
	uvBasler_SetCalbStepDispSize(fi_size);
}

/* desc : Set MMPM Disp Size */
API_EXPORT VOID uvEng_Camera_SetMMPMDispSize(CSize fi_size)
{
	uvBasler_SetMMPMDispSize(fi_size);
}


/* desc : Set Display Type */
API_EXPORT VOID uvEng_Camera_SetDispType(UINT8 dispType)
{
	uvBasler_SetDispType(dispType);
}

/* desc : Set Mark ROI */
API_EXPORT VOID uvCmn_Camera_MilSetMarkROI(UINT8 cam_id, CRect fi_rectSearhROI)
{
	uvBasler_MilSetMarkROI(cam_id, fi_rectSearhROI);
}

/* desc: pat Mark 등록 */
API_EXPORT BOOL uvCmn_Camera_RegistPat(UINT8 cam_id, CRect fi_rectArea, CString fi_filename, UINT8 mark_no)
{
	return uvBasler_RegistPat(cam_id, fi_rectArea, fi_filename, mark_no);
}

/* desc: mmf Mark 등록 */
API_EXPORT BOOL uvCmn_Camera_RegistMod(UINT8 cam_id, CRect fi_rectArea, CString fi_filename, UINT8 mark_no)
{
	return uvBasler_RegistMod(cam_id, fi_rectArea, fi_filename, mark_no);
}

/* desc: MMPM AutoCenter 이미지 등록 */
API_EXPORT BOOL uvCmn_Camera_RegistMMPM_AutoCenter(CRect fi_rectArea, UINT8 cam_id, UINT8 img_id)
{
	return uvBasler_RegistMMPM_AutoCenter(fi_rectArea, cam_id, img_id);
}

/* desc: Mark Size, Offset 초기화 */
API_EXPORT VOID uvCmn_Camera_InitSetMarkSizeOffset(UINT8 cam_id, TCHAR* file, UINT8 fi_findType, UINT8 mark_no)
{
	uvBasler_InitSetMarkSizeOffset(cam_id, file, fi_findType, mark_no);
}

/* desc: Draw Mark Display */
API_EXPORT VOID uvCmn_Camera_PutMarkDisp(HWND hwnd, int fi_iNo, RECT draw, UINT8 cam_id, TCHAR* file, int fi_findType)
{
	uvBasler_PutMarkDisp(hwnd, fi_iNo, draw, cam_id, file, fi_findType);
}

/* desc: Draw Set Mark Display */
API_EXPORT BOOL uvCmn_Camera_PutSetMarkDisp(HWND hwnd, RECT draw, UINT8 cam_id, TCHAR* file, int fi_findType, DOUBLE fi_dRate)
{
	return uvBasler_PutSetMarkDisp(hwnd, draw, cam_id, file, fi_findType, fi_dRate);
}

/* desc: Set Mark Setting Disp Rate */
API_EXPORT DOUBLE uvCmn_Camera_SetMarkSetDispRate(DOUBLE fi_dRate) // 1.0 은 FIT, -0.5, +0.5
{
	return uvBasler_SetMarkSetDispRate(fi_dRate);
}

/* desc: Get Mark Setting Disp Rate */
API_EXPORT DOUBLE uvCmn_Camera_GetMarkSetDispRate()
{
	return uvBasler_GetMarkSetDispRate();
}

/* desc: Get Mark Size */
API_EXPORT CPoint uvCmn_Camera_GetMarkSize(UINT8 cam_id, int fi_No)
{
	return uvBasler_GetMarkSize(cam_id, fi_No);
}

/* desc: Get Mark Offset */
API_EXPORT CPoint uvCmn_Camera_GetMarkOffset(UINT8 cam_id, BOOL bNewOffset, int fi_No)
{
	return uvBasler_GetMarkOffset(cam_id, bNewOffset, fi_No);
}

/* desc: Get Mask Buf */
API_EXPORT BOOL uvCmn_Camera_MaskBufGet()
{
	return uvBasler_MaskBufGet();
}

/* desc: Get Mark Find Mode */
API_EXPORT UINT8 uvCmn_Camera_GetMarkFindMode(UINT8 cam_id, UINT8 mark_no)
{
	return uvBasler_GetMarkFindMode(cam_id, mark_no);
}

/* desc: Set Mark Find Mode */
API_EXPORT BOOL uvCmn_Camera_SetMarkFindMode(UINT8 cam_id, UINT8 find_mode, UINT8 mark_no)
{
	return uvBasler_SetMarkFindMode(cam_id, find_mode, mark_no);
}

/* desc: Clear Mask(MMF) */
API_EXPORT VOID uvCmn_Camera_MaskClear_MOD(UINT8 cam_id, CPoint fi_iSizeP, UINT8 mark_no)
{
	return uvBasler_MaskClear_MOD(cam_id, fi_iSizeP, mark_no);
}

/* desc: Clear Mask(PAT) */
API_EXPORT VOID uvCmn_Camera_MaskClear_PAT(UINT8 cam_id, CPoint fi_iSizeP, UINT8 mark_no)
{
	return uvBasler_MaskClear_PAT(cam_id, fi_iSizeP, mark_no);
}

/* desc: Find Center (Mark Set에서만 사용) */
API_EXPORT VOID uvCmn_Camera_CenterFind(int cam_id, int fi_length, int fi_curSmoothness, double* fi_NumEdgeMIN_X, double* fi_NumEdgeMAX_X, double* fi_NumEdgeMIN_Y, double* fi_NumEdgeMAX_Y, int* fi_NumEdgeFound, int fi_Mode)
{
	return uvBasler_CenterFind(cam_id, fi_length, fi_curSmoothness, fi_NumEdgeMIN_X, fi_NumEdgeMAX_X, fi_NumEdgeMIN_Y, fi_NumEdgeMAX_Y, fi_NumEdgeFound, fi_Mode);
}

/* desc: Set Mark Size */
API_EXPORT VOID uvCmn_Camera_SetMarkSize(UINT8 cam_id, CPoint fi_MarkSize, int setMode, int fi_No)
{
	uvBasler_SetMarkSize(cam_id, fi_MarkSize, setMode, fi_No);
}

/* desc: Set Mark Offset */
API_EXPORT VOID uvCmn_Camera_SetMarkOffset(UINT8 cam_id, CPoint fi_MarkCenter, int setOffsetMode, int fi_No)
{
	uvBasler_SetMarkOffset(cam_id, fi_MarkCenter, setOffsetMode, fi_No);
}

/* desc: MMF 파일에 MASK 정보 저장, Mark Set에서만 사용 */
API_EXPORT VOID uvCmn_Camera_SaveMask_MOD(UINT8 cam_id, UINT8 mark_no)
{
	uvBasler_SaveMask_MOD(cam_id, mark_no);
}

/* desc: PAT 파일에 MASK 정보 저장, Mark Set에서만 사용 */
API_EXPORT VOID uvCmn_Camera_SaveMask_PAT(UINT8 cam_id, UINT8 mark_no)
{
	uvBasler_SaveMask_PAT(cam_id, mark_no);
}

/* desc: PAT MARK 저장 */
API_EXPORT VOID uvCmn_Camera_PatMarkSave(UINT8 cam_id, CString fi_strFileName, UINT8 mark_no)
{
	uvBasler_PatMarkSave(cam_id, fi_strFileName, mark_no);
}

/* desc: MMF MARK 저장 */
API_EXPORT VOID uvCmn_Camera_ModMarkSave(UINT8 cam_id, CString fi_strFileName, UINT8 mark_no)
{
	uvBasler_ModMarkSave(cam_id, fi_strFileName, mark_no);
}

/* desc: Mask 초기화 */
API_EXPORT VOID uvCmn_Camera_InitMask(UINT8 cam_id)
{
	uvBasler_InitMask(cam_id);
}

/* desc: Mil Main 할당 변수 해제 */
API_EXPORT VOID uvCmn_Camera_CloseSetMark()
{
	uvBasler_CloseSetMark();
}

/* desc: MARK DISP ID 할당 */
API_EXPORT VOID uvEng_Camera_SetDispMark(CWnd *pWnd)
{
	uvBasler_SetDispMark(pWnd);
}

/* desc: MARK DISP ID 할당 */
API_EXPORT VOID uvEng_Camera_SetDispRecipeMark(CWnd* pWnd[2])
{
	uvBasler_SetDispRecipeMark(pWnd);
}

/* desc: MARK DISP ID 할당 */
API_EXPORT VOID uvEng_Camera_SetDispExpo(CWnd* pWnd[4])
{
	uvBasler_SetDispExpo(pWnd);
}

/* desc: Mark 정보 그릴 때 사용하는 MIL 함수 */
API_EXPORT VOID uvEng_Camera_DrawMarkInfo_UseMIL(UINT8 cam_id, UINT8 fi_smooth, UINT8 mark_no)
{
	uvBasler_DrawMarkInfo_UseMIL(cam_id, fi_smooth, mark_no);
}

/* desc: Set Mask */
API_EXPORT VOID uvEng_Camera_Mask_MarkSet(UINT8 cam_id, CRect rectTmp, CPoint iTmpSizeP, CRect rectFill, int fi_color, bool bMask)
{
	uvBasler_Mask_MarkSet(cam_id, rectTmp, iTmpSizeP, rectFill, fi_color, bMask);
}

/*
	desc : Overlay 관련 함수 - 전체 Overlay Clear or DC 출력
	fi_bDrawFlag : FALSE - CLEAR
	fi_iDispType : 0:Expo, 1:mark, 2 : Live, 3 : mark set
	fi_iNo : Cam Num 혹은 Grab Mark Num (각자 Disp Type 에 맞춰서 사용)
*/
API_EXPORT VOID uvEng_Camera_DrawOverlayDC(bool fi_bDrawFlag, int fi_iDispType, int fi_iNo)
{
	uvBasler_DrawOverlayDC(fi_bDrawFlag, fi_iDispType, fi_iNo);
}


API_EXPORT VOID uvEng_Camera_ClearShapes(int fi_iDispType)
{
	uvBasler_Camera_ClearShapes( fi_iDispType);
}


/* desc : Overlay 관련 함수 - Box List 추가 */
API_EXPORT VOID uvEng_Camera_OverlayAddBoxList(int fi_iDispType, int fi_iNo, int fi_iLeft, int fi_iTop, int fi_iRight, int fi_iBottom, int fi_iStyle, int fi_color)
{
	uvBasler_OverlayAddBoxList(fi_iDispType, fi_iNo, fi_iLeft, fi_iTop, fi_iRight, fi_iBottom, fi_iStyle, fi_color);
}

/* desc : Overlay 관련 함수 - Cross List 추가 */
API_EXPORT VOID uvEng_Camera_OverlayAddCrossList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, int fi_iWdt1, int fi_iWdt2, int fi_color)
{
	uvBasler_OverlayAddCrossList(fi_iDispType, fi_iNo, fi_iX, fi_iY, fi_iWdt1, fi_iWdt2, fi_color);
}

/* desc : Overlay 관련 함수 - Text List 추가 */
API_EXPORT VOID uvEng_Camera_OverlayAddTextList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, CString fi_sText, int fi_color, int fi_iSizeX, int fi_iSizeY, CString fi_sFont, bool fi_bEgdeFlag)
{
	uvBasler_OverlayAddTextList(fi_iDispType, fi_iNo, fi_iX, fi_iY, fi_sText, fi_color, fi_iSizeX, fi_iSizeY, fi_sFont, fi_bEgdeFlag);
}

/* desc : Overlay 관련 함수 - Line List 추가 */
API_EXPORT VOID uvEng_Camera_OverlayAddLineList(int fi_iDispType, int fi_iNo, int fi_iSx, int fi_iSy, int fi_iEx, int fi_iEy, int fi_iStyle, int fi_color)
{
	uvBasler_OverlayAddLineList(fi_iDispType, fi_iNo, fi_iSx, fi_iSy, fi_iEx, fi_iEy, fi_iStyle, fi_color);
}

/* desc: MARK SET DISP ID 할당 */
API_EXPORT VOID uvEng_Camera_SetDispMarkSet(CWnd* pWnd)
{
	uvBasler_SetDispMarkSet(pWnd);
}

/* desc: MMPM AutoCenter DISP ID 할당 */
API_EXPORT VOID uvEng_Camera_SetDispMMPM_AutoCenter(CWnd* pWnd)
{
	uvBasler_SetDispMMPM_AutoCenter(pWnd);
}



/* desc: LIVE DISP ID 할당 */
//API_EXPORT VOID uvEng_Camera_SetDisp(CWnd* pWnd[2], UINT8 fi_Mode) // lk91!! 변수를 [2]로 받으면 안됌 수정!!
API_EXPORT VOID uvEng_Camera_SetDisp(CWnd** pWnd, UINT8 fi_Mode) // lk91!! 변수를 [2]로 받으면 안됌 수정!! 240116
{
	uvBasler_SetDisp(pWnd, fi_Mode);
}

/* desc: MMPM DISP ID 할당 */
API_EXPORT VOID uvEng_Camera_SetDispMMPM(CWnd* pWnd)
{
	uvBasler_SetDispMMPM(pWnd);
}

/* desc : Zoom 관련 함수 - Zoom In */
API_EXPORT VOID uvCmn_Camera_MilZoomIn(int fi_iDispType, int cam_id, CRect rc)
{
	uvBasler_MilZoomIn(fi_iDispType, cam_id, rc);
}

/* desc : Zoom 관련 함수 - Zoom Out */
API_EXPORT BOOL uvCmn_Camera_MilZoomOut(int fi_iDispType, int cam_id, CRect rc)
{
	return uvBasler_MilZoomOut(fi_iDispType, cam_id, rc);
}

/* desc : Zoom 관련 함수 - Zoom Fit, 이미지 비율 1.0 */
API_EXPORT VOID uvCmn_Camera_MilAutoScale(int fi_iDispType, int cam_id)
{
	uvBasler_MilAutoScale(fi_iDispType, cam_id);
}

/* desc : Zoom 관련 함수 - Mouse Point 저장, Zoom으로 확대되었을 때 이미지 이동하기 위함 */
API_EXPORT VOID uvCmn_Camera_SetZoomDownP(int fi_iDispType, int cam_id, CPoint fi_point)
{
	uvBasler_SetZoomDownP(fi_iDispType, cam_id, fi_point);
}

/* desc : Zoom 관련 함수 - 이미지 이동 */
API_EXPORT VOID uvCmn_Camera_MoveZoomDisp(int fi_iDispType, int cam_id, CPoint fi_point, CRect fi_rect)
{
	uvBasler_MoveZoomDisp(fi_iDispType, cam_id, fi_point, fi_rect);
}

/* desc : Zoom 관련 함수 - Zoom 된 영상에서 클릭된 좌표를 이미지 좌표로 변환시켜주는 작업 */
API_EXPORT CDPoint uvCmn_Camera_trZoomPoint(int fi_iDispType, int cam_id, CPoint fi_point)
{
	return uvBasler_trZoomPoint(fi_iDispType, cam_id, fi_point);
}
API_EXPORT BOOL uvEng_Camera_SetModelDefine_tot(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
	LPG_CMPV model, UINT8 fi_No, TCHAR* file,
	DOUBLE scale_min, DOUBLE scale_max,
	DOUBLE score_min, DOUBLE score_tgt)
{
	return uvBasler_SetModelDefine_tot(cam_id, speed, level, count, smooth, model, fi_No, file,
		scale_min, scale_max, score_min, score_tgt);
}

API_EXPORT BOOL uvEng_Camera_MergeMark(UINT8 cam_id, LPG_CMPV value, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, UINT8 mark_no, TCHAR* file1, TCHAR* file2,
	TCHAR* RecipeName, DOUBLE scale_min, DOUBLE scale_max, DOUBLE score_min, DOUBLE score_tgt)
{
	return uvBasler_MergeMark(cam_id, value, speed, level, count, smooth, mark_no, file1, file2,
		RecipeName, scale_min, scale_max, score_min, score_tgt);
}

API_EXPORT BOOL uvCmn_Camera_ProcImage(UINT8 cam_id, UINT8 imgProc)
{
	return uvBasler_ProcImage(cam_id, imgProc);
}

API_EXPORT CDPoint uvEng_Camera_RunModel_VisionCalib(UINT8 cam_id, UINT8 dlg_id, UINT8 mark_no, int* roi_left, int* roi_right, int* roi_top, int* roi_bottom, int row, int col)
{
	return uvBasler_RunModel_VisionCalib(cam_id, dlg_id, mark_no, roi_left, roi_right, roi_top, roi_bottom, row, col);
}

#ifdef __cplusplus
}
#endif