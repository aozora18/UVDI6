
/*
 desc : IP Camera Communication Libarary (Basler pylon sdk ACA1300-30GM)
*/

#pragma once

#include "../conf/global.h"
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
#include "../conf/conf_podis.h"
#include "../conf/vision_podis.h"
#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
#include "../../inc/conf/conf_gen2i.h"
#elif (CUSTOM_CODE_UVDI15 == DELIVERY_PRODUCT_ID|| \
CUSTOM_CODE_HDDI6== DELIVERY_PRODUCT_ID)
#include "../conf/conf_uvdi15.h"
#include "../conf/vision_uvdi15.h"

#endif

class AlignMotion;

#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : BIPS Server와 통신을 담당하는 Client 스레드 생성 및 실행
 parm : config		- [in]  환경 정보
		shmem		- [in]  Shared Memory
		work_dir	- [in]  작업 경로 (실행 파일 경로)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMIL_Init(LPG_CIEA config, LPG_VDSM shmem, PTCHAR work_dir);
/*
 desc : 라이브러리 해제
 parm : None
 retn : None
*/
API_IMPORT VOID uvMIL_Close();
/*
 desc : 가장 최근의 Grabbed Image 결과 초기화
		새로운 Align 작업을 수행한다면, Align 수행하기 전에 무조건 한번은 호출되어야 함
 parm : None
 retn : None
*/
API_IMPORT VOID uvMIL_ResetGrabAll();
/*
 desc : Mark 정보 설정
 parm : cam_id		- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		detail_level- [in]  등록된 모델 이미지와 Grabbed Image이미지의 Edge 영역에 대한 비교 정도 값
							값이 클수록 상세히 비교하지만, 속도는 많이 느려지짐
							LOW보다 HIGH로 갈수록 더 많은 Edge를 가지고 비교하게 됨
							0 (M_HIGH), 1 (M_MEDIUM), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - 기본 값, 1.0f ~ 100.0f
		model		- [in]  Model Type, Param 1 ~ 5개 정보까지 모두 포함된 구조체 포인터
		count		- [in]  등록하고자 하는 모델의 개수
		아래 2개의 값이 0 값이면 파라미터 적용되지 않음
		scale_min	- [in]  검색 대상의 크기 범위 값 설정 (최소한 이 값 비율보다 커야 됨. 범위: 0.5 ~ 1.0)
		scale_max	- [in]  검색 대상의 크기 범위 값 설정 (최대한 이 값 비율보다 작아야 됨. 범위: 1.0 ~ 1.0)
		score_min	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0)
		score_tgt	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0))
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMIL_SetModelDefine(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
									 LPG_CMPV model, UINT8 mark_no,
									 DOUBLE scale_min=0.0f, DOUBLE scale_max=0.0f,
									 DOUBLE score_min=0, DOUBLE score_tgt=0, bool sameMark = false);

API_IMPORT VOID uvMIL_SetAlignMotionPtr(AlignMotion& ptr);

/*
 desc : 로컬 시스템 (저장소)에 저장되어 있는 패턴 매칭 원본 이미지 적재
 parm : cam_id		- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		detail_level- [in]  등록된 모델 이미지와 Grabbed Image이미지의 Edge 영역에 대한 비교 정도 값
							값이 클수록 상세히 비교하지만, 속도는 많이 느려지짐
							LOW보다 HIGH로 갈수록 더 많은 Edge를 가지고 비교하게 됨
							0 (M_HIGH), 1 (M_MEDIUM), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - 기본 값, 1.0f ~ 100.0f
							기본 값은 50이지만, 100으로 갈수록 엣지의 강도가 센것만 나타납니다.
		model		- [in]  Model Type 즉, circle, square, rectangle, cross, diamond, triangle
		param		- [in]  총 5개의 Parameter Values (unit : um)
		mark_no		- [in]  등록하고자 하는 모델 번호
		아래 2개의 값이 0 값이면 파라미터 적용되지 않음
		scale_min	- [in]  검색 대상의 크기 범위 값 설정 (최소한 이 값 비율보다 커야 됨. 범위: 0.5 ~ 1.0)
		scale_max	- [in]  검색 대상의 크기 범위 값 설정 (최대한 이 값 비율보다 작아야 됨. 범위: 1.0 ~ 1.0)
		score_min	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0)
		score_tgt	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0))
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMIL_SetModelDefineEx(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
	PUINT32 model, DOUBLE* param1, DOUBLE* param2, DOUBLE* param3,
	DOUBLE* param4, DOUBLE* param5, UINT8 mark_no,
	DOUBLE scale_min = 0.0f, DOUBLE scale_max = 0.0f,
	DOUBLE score_min = 0.0f, DOUBLE score_tgt = 0.0f, bool sameMark = false);
/*
 desc : Mark 정보 설정 - 이미지 데이터
 parm : cam_id		- [in]  Align Camera Index (1 or 2)
		speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		level		- [in]  등록된 모델 이미지와 Grabbed Image이미지의 Edge 영역에 대한 비교 정도 값
							값이 클수록 상세히 비교하지만, 속도는 많이 느려지짐
							LOW보다 HIGH로 갈수록 더 많은 Edge를 가지고 비교하게 됨
							0 (M_MEDIUM), 1 (M_HIGH), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - 기본 값, 1.0f ~ 100.0f
							기본 값은 50이지만, 100으로 갈수록 엣지의 강도가 센것만 나타납니다.
		scale_min	- [in]  검색 대상의 크기 범위 값 설정 (최소한 이 값 비율보다 커야 됨. 범위: 0.5 ~ 1.0)
		scale_max	- [in]  검색 대상의 크기 범위 값 설정 (최대한 이 값 비율보다 작아야 됨. 범위: 1.0 ~ 1.0)
		score_min	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0)
		score_tgt	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0))
		name		- [in]  Model Name
		file		- [in]  모델 이미지가 저장된 파일 이름 (전체 경로 포함. Image File)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMIL_SetModelDefineLoad(UINT8 cam_id, UINT8 speed, UINT8 level, DOUBLE smooth,
										 DOUBLE scale_min, DOUBLE scale_max,
										 DOUBLE score_min, DOUBLE score_tgt,
										 PTCHAR name, CStringArray &file);
/*
 desc : 현재 등록된 Mark Model 정보 반환
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : 등록된 구조체 포인터 반환
*/
API_IMPORT LPG_CMPV uvMIL_GetModelDefine(UINT8 cam_id);
/*
 desc : Camera로부터 수집된 Grabbed Image (Binary Image Data)의 Edge Detection
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
		grab	- [in]  Grabbed Image 정보가 저장된 구조체 포인터
		saved	- [in]  분석된 edge image 저장 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMIL_RunEdgeDetect(UINT8 cam_id, LPG_ACGR grab, UINT8 saved);
/*
 desc : Set the parameters of the strip maker
 parm : cam_id	- [in]  Align Camera ID (1 or 2 or later)
		image	- [in]  The buffer of Grabbed Image
		param	- [in]  Structures pointer with information to set up is stored
		result	- [out] Buffer in which the resulting value will be returned and stored.
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMIL_RunMarkerStrip(UINT8 cam_id, PUINT8 image, LPG_MSMP param, STG_MSMR &results);
/*
 desc : 현재 검색을 위해 등록된 모델의 개수 반환
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : 등록된 개수
*/
API_IMPORT UINT32 uvMIL_GetModelRegistCount(UINT8 cam_id);
/*
 desc : Geometric Model Find
 parm : cam_id	- [in]  Camera Index (1 or 2)
 		img_id	- [in]  Camera Grabbed Image Index (0 or Later) (if img_id == 0xff then Calibration Method)
		image	- [in]  Grabbed Image Data
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMIL_RunModelFind(UINT8 cam_id, UINT8 mode, UINT8 img_id, PUINT8 image, UINT8 dlg_id, UINT8 mark_no, BOOL useMilDisp, UINT8 img_proc);


API_EXPORT VOID uvMIL_Camera_ClearShapes(int fi_iDispType);

/*
 desc : 사용자에 의해 수동으로 검색된 경우, 결과 값만 처리
 parm : grab	- [in]  사용자에 의해 수동으로 입력된 grabbed image 결과 정보가 저장된 구조체 포인터
		gmfr	- [in]  GMFR Data
		gmsr	- [in]  GMSR Data
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMIL_SetGrabbedMark(LPG_ACGR grab, LPG_GMFR gmfr, LPG_GMSR gmsr);
/*
 desc : Geometric Model Find (Step; Vision 단차 확인용)
 parm : cam_id	- [in]  Camera Index (1 or 2)
		width	- [in]  Grabbed Image - Width (Pixel)
		height	- [in]  Grabbed Image - Width (Pixel)
		count	- [in]  검색하고자 하는 모델 개수
		angle	- [in]  각도 적용 여부 (TRUE : 각도 측정함, FALSE : 각도 측정하지 않음)
						TRUE : 현재 카메라의 회전된 각도 구하기 위함, FALSE : 기존 각도 적용하여 회전된 각도 구함
		results	- [out] 검색된 결과 값 반환 구조체 포인터
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMIL_RunModelStep(UINT8 cam_id,
								   PUINT8 image, UINT32 width, UINT32 height,
								   UINT16 count, BOOL angle, LPG_ACGR results, UINT8 img_id, UINT8 dlg_id, UINT8 mark_no, UINT8 img_proc);
/*
 desc : Geometric Model Find (Examination; Vision Align 노광 결과 확인용)
 parm : width		- [in]  Grabbed Image - Width (Pixel)
		height		- [in]  Grabbed Image - Width (Pixel)
		score		- [in]  크기 비율 값 (이 값보다 크거나 같으면 Okay)
		scale		- [in]  매칭 비율 값 (1.0 기준으로 +/- 이 값 범위 안에 있으면 Okay)
		results		- [out] 검색된 결과 값 반환 구조체 포인터
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMIL_RunModelExam2(PUINT8 image, UINT32 width, UINT32 height,
								   DOUBLE score, DOUBLE scale, LPG_ACGR results=NULL);


API_IMPORT BOOL uvMIL_RunModelExam(PUINT8 image, UINT32 width, UINT32 height, 
									DOUBLE score, DOUBLE scale, LPG_ACGR results, 
									UINT8 img_id, UINT8 dlg_id, UINT8 mark_no);

/*
 desc : 가장 최근 Grabbed Image의 패턴 매칭 검색 결과 구조체 포인터 반환
 parm : None
 retn : 객체 포인터 (매칭이 실패한 경우 NULL)
*/
API_IMPORT LPG_ACGR uvMIL_GetLastGrabbedMark();
/*
 desc : 기존에 적재 (등록)되어 있던 모든 Mark 해제 수행
 parm : None
 retn : None
*/
API_IMPORT VOID uvMIL_ResetMarkModel();
/*
 desc : Mark Pattern 등록 여부
 parm : mode	- [in]  0x00 : Expose, 0x01 : Vstep
		cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMIL_IsSetMarkModel(UINT8 mode=0x00, UINT8 cam_id=0x01, UINT8 fi_No=0x00);
/*
 desc : Mark Pattern 등록 여부
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMIL_IsSetMarkModelACam(UINT8 cam_id, UINT8 mark_no);
/*
 desc : Outputs the most recently generated Buffer ID value to the window
 parm : hwnd	- [in]  Handle of window where the image is output
		draw	- [in]  The area of window where the image is output
		cam_id	- [in]  Align Camera ID (1 or 2)
 retn : None
*/
API_IMPORT VOID uvMIL_DrawMBufID(HWND hwnd, RECT draw, UINT8 type, UINT8 cam_id);
/*
 desc : 비트맵 이미지 파일을 적재 후 화면에 출력
 parm : hdc		- [in]  DC 핸들
		area	- [in]  Mem DC 영역에 출력하려는 크기 (CRect 객체도 받기 위함 참조 변수 선언)
		file	- [in]  출력하고자 하는 파일명 (전체 경로 및 확장자 포함)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMIL_DrawBitmapFile(HDC hdc, RECT &area, TCHAR *file);
/*
 desc : Draw the strip information in the current grabbed image
 parm : hdc		- [in]  The handle of context
		draw	- [in]  The area in which the image is output
		cam_id	- [in]  Camera Index (1 or 2)
		angle	- [in]  The rotated angle value of the camera (unit: degree)
		param	- [in]  Strip output information
 retn : None
*/
API_IMPORT VOID uvMIL_DrawStripBitmap(HDC hdc, RECT draw, LPG_ACGR grab,
									  DOUBLE angle, LPG_MSMP param);
/*
 desc : Calibration 이미지 (검색된 결과)를 윈도 영역에 출력 수행 (Bitmap을 이용하여 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
 retn : None
*/
API_IMPORT VOID uvMIL_DrawCaliMarkBitmap(HDC hdc, RECT draw);
/*
 desc : 검색된 Mark 이미지 윈도 영역에 출력 수행 (DC를 이용하여 bitmap 이미지로 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later) (if img_id == 0xff then Calibration Method)
		find	- [in]  마크 검색 성공 여부 (검색됐더라도, 검색 결과 값에 따라 달라짐)
						0x00 - 검색 결과 실패, 0x01 - 검색 결과 성공
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMIL_DrawMarkBitmap(HDC hdc, RECT draw, UINT8 cam_id, UINT8 img_id, UINT8 find);
/*
 desc : 최종 검색된 이미지 (Model find)를 윈도 영역에 출력 수행 (MIL Buffer에 저장된 것을 출력)
 parm : hwnd	- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later) (if img_id == 0xff then Calibration Method)
 retn : None
*/
API_IMPORT VOID uvMIL_DrawMarkMBufID(HWND hwnd, RECT draw, UINT8 cam_id, UINT8 hwndIdx,UINT8 img_id);
/*
 desc : Calibration 이미지 윈도 영역에 출력 수행 (Bitmap을 이용하여 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		grab	- [in]  grab 이미지가 저장된 구조체 포인터 (grab_data에 이미지가 정보가 반드시 저장되어 있어야 됨)
		find	- [in]  마크 검색 성공 여부 (검색됐더라도, 검색 결과 값에 따라 달라짐)
						0x00 - 검색 결과 실패, 0x01 - 검색 결과 성공
 retn : None
*/
API_IMPORT VOID uvMIL_DrawMarkDataBitmap(HDC hdc, RECT draw, LPG_ACGR grab, UINT8 find,bool drawForce, UINT8 flipFlag);
/*
 desc : Drawing - Examination Object Image (Bitmap을 이용하여 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		grab_out- [in]  Grabbed Image 정보가 저장된 구조체 포인터 (바깥 링)
		grab_in	- [in]  Grabbed Image 정보가 저장된 구조체 포인터 (안쪽 원)
 retn : None
*/
API_IMPORT VOID uvMIL_DrawGrabExamBitmap(HDC hdc, RECT draw, LPG_ACGR grab_out, LPG_ACGR grab_in);
/*
 desc : 검색된 마크 이미지가 포함된 구조체 포인터 반환
 parm : cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later) (if img_id == 0xff then Calibration Method)
 retn : 구조체 포인터 반환
*/
API_IMPORT LPG_ACGR uvMIL_GetGrabbedMark(UINT8 cam_id, UINT8 img_id);
/*
 desc : 가장 최근에 Edge Detection된 이미지에서 특정 영역만 추출해서 Bitmap Image로 등록
		즉, Mark Template 이미지로 등록
 parm : cam_id	- [in]  Align Camera ID (1 or Later)
		area	- [in]  추출하고자 하는 영역 정보가 저장된 픽셀 위치 값
		type	- [in]  0x00 : Edge, 0x02 : Line, 0x02 : Match, 0x03 : Grab
		file	- [in]  Mark Template를 저장하려는 파일 이름 (전체 경로 포함)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMIL_SaveGrabbedMarkToFile(UINT8 cam_id, LPRECT area, UINT8 type, TCHAR *file);
/*
 desc : Edge Detection 검색된 결과 반환
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
 retn : 결과 값이 저장된 포인터 반환
*/
API_IMPORT LPG_EDFR uvMIL_GetEdgeDetectResults(UINT8 cam_id);
/*
 desc : Edge Detection 검색된 개수 반환
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
 변환 : 개수 반환. 실패할 경우, 0 이하 값
*/
API_IMPORT INT32 uvMIL_GetEdgeDetectCount(UINT8 cam_id);
/*
 desc : 현재 등록된 Mark Model Type 반환
 parm : cam_id	- [in]  Align Camera Index (0x01 ~ MAX_INSTALL_CAMERA)
		mark_id	- [in]  Align Mark Index (0x00 ~ MAX_REGIST_MODEL-1)
 retn : Model Type 값
*/
API_IMPORT UINT32 uvMIL_GetMarkModelType(UINT8 cam_id, UINT8 index);
/*
 desc : 현재 등록된 Mark Model 크기 반환
 parm : cam_id	- [in]  Grabbed Image 정보가 발생된 Align Camera Index (1 or 2)
		index	- [in]  요청하고자 하는 모델의 위치 (Zero-based)
		flag	- [in]  0x00 : 가로  크기, 0x01 : 세로 크기
		unit	- [in]  0x00 : um, 0x01 : pixel
 retn : 크기 반환 (단위: um)
*/
API_IMPORT DOUBLE uvMIL_GetMarkModelSize(UINT8 cam_id, UINT8 index, UINT8 flag, UINT8 unit=0x00);
/*
 desc : Align Mark 검색 방식 설정
 parm : method	- [in]  0x00 : 최적의 1개 반환, 0x01 : 다점 마크 기준으로 가중치를 부여하여 결과 반환
		count	- [in]  'method' 값이 2 or 3인 경우, 최종 검색될 Mark 개수 값 2 이상 값이어야 됨
 retn : None
*/
API_IMPORT VOID uvMIL_SetMarkMethod(ENG_MMSM method, UINT8 count);
/*
 desc : 이미지의 가로 혹은 세로의 경계선 중심 위치 얻기
 parm : cam_id		- [in]  Align Camera ID (1 or 2)
		grab_data	- [in]  Grabbed Image 정보가 저장된 구조체 포인터
		mil_result	- [in]  MIL 내부 검색 결과 대화 상자 출력 여부
 retn : TRUE or FALSE
*/


API_IMPORT UINT8 uvMIL_SetMarkFindSetCount(int camNum);
API_IMPORT UINT8 uvMIL_SetMarkFoundCount(int camNum);

API_IMPORT BOOL uvMIL_RunLineCentXY(UINT8 cam_id, LPG_ACGR grab_data, BOOL mil_result=FALSE);
/*
 desc : Iamge 회전 (이미지의 중심을 기준으로 회전)
 parm : img_src		- [in]  원본 이미지 버퍼
		img_dst		- [out] 회전된 이미지 버퍼
		width		- [in]	원본 이미지의 넓이
		height		- [in]	원본 이미지의 높이
		angle		- [in]	회전 하고자 하는 각도
 retn : None
*/
API_IMPORT VOID uvMIL_ImageRotate(PUINT8 img_src, PUINT8 img_dst,
								  UINT32 width, UINT32 height, UINT32 angle);
/*
 desc : 현재 MIL 라이선스가 유효한지 여부 확인
		Pattern, Model Finder, Edge Finder, Read, OCR, Image Processing 등 유효한지 확인
 parm : None
 retn : TRUE or FALS
*/
API_IMPORT BOOL uvMIL_IsLicenseValid();
/*
 desc : 노광 모드 설정 즉, 직접 노광, 얼라인 노광, 보정 후 얼라인 노광
 parm : mode	- [in]  직접 노광 (0x00), 얼라인 노광 (0x01), 얼라인 카메라 보정 값 적용 후 얼라인 노광 (0x02)
 retn : None
*/
API_IMPORT VOID uvMIL_SetAlignMode(ENG_AOEM mode);
/*
 desc : 카메라로부터 수집된 이미지 버퍼의 내용을 파일로 저장
 parm : file	- [in]  저장하려는 파일 (전체 경로 포함)
		width	- [in]  이미지의 크기 (넓이: pixel)
		height	- [in]  이미지의 크기 (높이: pixel)
		image	- [in]  이미지가 저장된 버퍼
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMIL_SaveGrabbedToFile(PTCHAR file, INT32 width, INT32 height, PUINT8 image);
/*
 desc : 현재 적재된 레시피의 마크 검색 조건 값 설정
 parm : score	- [in]  Score Accpetance (대상 이미지에서 검색된 마크의 Score 값이 이 값보다 작으면 버린다)
		scale	- [in]  Scale Range (대상 이미지에서 등록된 마크를 검색 결과들 중에서 이 비율 범위 내에 )
									(포함된 결과들만 유효하고, 그 중에서 Score 값이 가장 높은 것만 추출)
									(가령. 이 값이 50.0000 이면, 실제 검색에 사용되는 마크의 크기 범위)
									(검색범위 = 1.0f - (100.0 - 50.0) / 100 임 즉, 0.5 ~ 1.5 임)
 retn : None
*/
API_IMPORT VOID uvMIL_SetRecipeMarkRate(DOUBLE score, DOUBLE scale);
/*
 desc : 스테이지의 이동 방향 설정 (정방향 이동인지 / 역방향 이동인지 여부)
 parm : direct	- [in]  TRUE (정방향 이동 : 앞에서 뒤로 이동), FALSE (역방향 이동)
 retn : None
*/
API_IMPORT VOID uvMIL_SetMoveStateDirect(BOOL direct);
/*
 desc : Grabbed Image 마다 표현된 마크의 개수가 여러 개인 경우, 마크 들이 분포된 영역의 크기 값 설정
 parm : width	- [in]  사각형 영역의 넓이 값 (단위: um)
		height	- [in]  사각형 영역의 높이 값 (단위: um)
 retn : None
*/
API_IMPORT VOID uvMIL_SetMultiMarkArea(UINT32 width, UINT32 height);
/*
 desc : Set the parameters of the strip maker
 parm : cam_id	- [in]  Align Camera ID (1 or 2 or later)
		param	- [in]  Structures pointer with information to set up is stored
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMIL_SetStripMakerSetup(UINT8 cam_id, LPG_MSMP param);

/* ----------------------------------------------------------------------------------------- */
/*                                 lk91 VISION 추가 함수                                     */
/* ----------------------------------------------------------------------------------------- */
API_IMPORT VOID uvMIL_DrawLiveBitmap(HDC hdc, RECT draw, LPG_ACGR grab, DOUBLE angle);
API_IMPORT VOID uvMIL_DrawImageBitmap(int dispType, int Num, LPG_ACGR grab, DOUBLE angle, int cam_id);
API_IMPORT VOID uvMIL_DrawImageBitmapFlip(int dispType, int Num, LPG_ACGR grab, DOUBLE angle, int cam_id, int  flipOption=-1);

API_IMPORT VOID uvMIL_SetMarkLiveDispSize(CSize fi_size);
API_IMPORT VOID uvMIL_SetCalbCamSpecDispSize(CSize fi_size);
API_IMPORT VOID uvMIL_SetCalbStepDispSize(CSize fi_size);
API_IMPORT VOID uvMIL_SetAccuracyMeasureDispSize(CSize fi_size);
API_IMPORT VOID uvMIL_SetMMPMDispSize(CSize fi_size);
API_IMPORT BOOL uvMIL_SetModelDefineMMF(UINT8 cam_id, PTCHAR name, PTCHAR mmf, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no);
API_IMPORT BOOL uvMIL_SetModelDefinePAT(UINT8 cam_id, PTCHAR name, PTCHAR pat, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no);
API_EXPORT VOID uvMIL_InitMarkROI(LPG_CRD fi_MarkROI);
API_EXPORT VOID uvMIL_MilSetMarkROI(UINT8 cam_id, CRect fi_rectSearhROI);
API_EXPORT BOOL uvMIL_RegistPat(UINT8 cam_id, LPG_ACGR grab, CRect fi_rectArea, CString fi_filename, UINT8 mark_no);
API_EXPORT BOOL uvMIL_RegistMod(UINT8 cam_id, LPG_ACGR grab, CRect fi_rectArea, CString fi_filename, UINT8 mark_no);
API_EXPORT BOOL uvMIL_RegistMMPM_AutoCenter(CRect fi_rectArea, UINT8 cam_id, UINT8 img_id);
API_EXPORT BOOL uvMIL_PutSetMarkDisp(HWND hwnd, RECT draw, UINT8 cam_id, TCHAR* file, int fi_findType, DOUBLE fi_dRate);
API_EXPORT VOID uvMIL_PutMarkDisp(HWND hwnd, int fi_iNo, RECT draw, UINT8 cam_id, TCHAR* file, int fi_findType);
API_EXPORT VOID uvMIL_InitSetMarkSizeOffset(UINT8 cam_id, TCHAR* file, UINT8 fi_findType, UINT8 mark_no);
API_EXPORT CPoint uvMIL_GetMarkSize(UINT8 cam_id, int fi_No);
API_EXPORT CPoint uvMIL_GetMarkOffset(UINT8 cam_id, BOOL bNewOffset, int fi_No);
API_EXPORT BOOL uvMIL_MaskBufGet();
API_EXPORT UINT8 uvMIL_GetMarkFindMode(UINT8 cam_id, UINT8 mark_no);
API_EXPORT BOOL uvMIL_SetMarkFindMode(UINT8 cam_id, UINT8 find_mode, UINT8 mark_no);
API_EXPORT DOUBLE uvMIL_GetMarkSetDispRate();
API_EXPORT DOUBLE uvMIL_SetMarkSetDispRate(DOUBLE fi_dRate);
API_EXPORT VOID uvMIL_SetMarkSize(UINT8 cam_id, CPoint fi_MarkSize, int setMode, int fi_No);
API_EXPORT VOID uvMIL_SetMarkOffset(UINT8 cam_id, CPoint fi_MarkCenter, int setOffsetMode, int fi_No);
API_EXPORT VOID uvMIL_SaveMask_MOD(UINT8 cam_id, UINT8 mark_no);
API_EXPORT VOID uvMIL_SaveMask_PAT(UINT8 cam_id, UINT8 mark_no);
API_EXPORT VOID uvMIL_PatMarkSave(UINT8 cam_id, CString fi_strFileName, UINT8 mark_no);
API_EXPORT VOID uvMIL_ModMarkSave(UINT8 cam_id, CString fi_strFileName, UINT8 mark_no);
API_EXPORT VOID uvMIL_MaskClear_MOD(UINT8 cam_id, CPoint fi_iSizeP, UINT8 mark_no);
API_EXPORT VOID uvMIL_MaskClear_PAT(UINT8 cam_id, CPoint fi_iSizeP, UINT8 mark_no);
API_EXPORT VOID uvMIL_CenterFind(int cam_id, int fi_length, int fi_curSmoothness, double* fi_NumEdgeMIN_X, double* fi_NumEdgeMAX_X, double* fi_NumEdgeMIN_Y, double* fi_NumEdgeMAX_Y, int* fi_NumEdgeFound, int fi_Mode);
API_EXPORT VOID uvMIL_SetDispMark(CWnd* pWnd);
API_EXPORT VOID uvMIL_SetDispRecipeMark(CWnd* pWnd[2]);
API_EXPORT VOID uvMIL_SetDispMarkSet(CWnd* pWnd);
API_EXPORT VOID uvMIL_SetDispMMPM_AutoCenter(CWnd* pWnd);
API_EXPORT VOID uvMIL_SetDisp(CWnd** pWnd, UINT8 fi_Mode);
API_EXPORT VOID uvMIL_SetDispMMPM(CWnd* pWnd);
API_EXPORT VOID uvMIL_SetDispExpo(CWnd* pWnd[4]);
API_EXPORT BOOL uvMIL_RegistMILImg(INT32 cam_id, INT32 width, INT32 height, PUINT8 image);
API_EXPORT BOOL uvMIL_RegistMILGrabImg(INT32 fi_No, INT32 width, INT32 height, PUINT8 image);
API_EXPORT VOID uvMIL_DrawOverlayDC(bool fi_bDrawFlag, int fi_iDispType, int fi_iNo);
API_EXPORT VOID uvMIL_OverlayAddBoxList(int fi_iDispType, int fi_iNo, int fi_iLeft, int fi_iTop, int fi_iRight, int fi_iBottom, int fi_iStyle, int fi_color);
API_EXPORT VOID uvMIL_OverlayAddCrossList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, int fi_iWdt1, int fi_iWdt2, int fi_color);
API_EXPORT VOID uvMIL_OverlayAddTextList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, CString fi_sText, int fi_color, int fi_iSizeX, int fi_iSizeY, CString fi_sFont, bool fi_bEgdeFlag);
API_EXPORT VOID uvMIL_ImageFlip(PUINT8 img_src, PUINT8 img_dst, UINT32 width, UINT32 height);
API_EXPORT VOID uvMIL_OverlayAddLineList(int fi_iDispType, int fi_iNo, int fi_iSx, int fi_iSy, int fi_iEx, int fi_iEy, int fi_iStyle, int fi_color);
API_EXPORT VOID uvMIL_DrawMarkInfo_UseMIL(UINT8 cam_id, UINT8 fi_smooth, UINT8 mark_no);
API_EXPORT VOID uvMIL_Mask_MarkSet(UINT8 cam_id, CRect rectTmp, CPoint iTmpSizeP, CRect rectFill, int fi_color, bool bMask);
API_EXPORT VOID uvMIL_InitMask(UINT8 cam_id);
API_EXPORT VOID uvMIL_CloseSetMark();
API_EXPORT VOID uvMIL_MilZoomIn(int fi_iDispType, int cam_id, CRect rc);
API_EXPORT BOOL uvMIL_MilZoomOut(int fi_iDispType, int cam_id, CRect rc);
API_EXPORT VOID uvMIL_MilAutoScale(int fi_iDispType, int cam_id);
API_EXPORT VOID uvMIL_SetZoomDownP(int fi_iDispType, int cam_id, CPoint fi_point);
API_EXPORT VOID uvMIL_MoveZoomDisp(int fi_iDispType, int cam_id, CPoint fi_point, CRect fi_rect);
API_EXPORT CDPoint uvMIL_trZoomPoint(int fi_iDispType, int cam_id, CPoint fi_point);
API_IMPORT BOOL uvMIL_SetModelDefine_tot(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
	LPG_CMPV model, UINT8 fi_No, TCHAR* file,
	DOUBLE scale_min = 0.0f, DOUBLE scale_max = 0.0f,
	DOUBLE score_min = 0.0f, DOUBLE score_tgt = 0.0f);
API_EXPORT BOOL uvMIL_MergeMark(UINT8 cam_id, LPG_CMPV value, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, UINT8 mark_no, TCHAR* file1, TCHAR* file2,
	TCHAR* RecipeName, DOUBLE scale_min = 0.0f, DOUBLE scale_max = 0.0f,
	DOUBLE score_min = 0.0f, DOUBLE score_tgt = 0.0f);
API_EXPORT BOOL uvMIL_ProcImage(UINT8 cam_id, UINT8 imgProc);
API_IMPORT BOOL uvMIL_RunModel_VisionCalib(UINT8 cam_id, UINT8 img_id, PUINT8 image, UINT8 dlg_id, UINT8 mark_no, int* roi_left, int* roi_right, int* roi_top, int* roi_bottom, int row, int col);
#ifdef __cplusplus
}
#endif