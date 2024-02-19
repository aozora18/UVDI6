
/*
 desc : IP Camera Communication Libarary (Basler pylon sdk ACA1300-30GM)
*/

#pragma once

#include "../conf/global.h"
#include "../conf/vision_uvdi15.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : BIPS Server와 통신을 담당하는 Client 스레드 생성 및 실행
 parm : config		- [in]  전체 환경 정보
		shmem		- [in]  Shared Memory
		kind		- [in]  Vision Library 종류
		work_dir	- [in]  작업 경로 (실행 파일 경로)
		time_out	- [in]  기존 카메라와의 연결해제가 비정상적으로 종료되었을 때,
							카메라 내부에서 기존 연결된 클라이언트의 응답이 없을 때, 해제 대기 시간
							클라이언트로부터 임의 설정 시간(단위: 밀리초)동안 응답 없으면, 강제 해제
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_Init(LPG_CIEA config, LPG_VDSM shmem, ENG_VLPK kind, PTCHAR work_dir, 
#ifdef _DEBUG
							  UINT64 time_out=1000 * 10);
#else
							  UINT64 time_out=5000);
#endif

API_EXPORT VOID uvBasler_initMarkROI(LPG_CRD fi_MarkROI);

/*
 desc : BIPS Server와 통신을 담당하는 Client 스레드 생성 및 실행
 parm : config		- [in]  전체 환경 정보
		shmem		- [in]  Shared Memory
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_Open(LPG_CIEA config, LPG_VDSM shmem);
/*
 desc : 라이브러리 해제
 parm : None
 retn : None
*/
API_IMPORT VOID uvBasler_Close();
/*
 desc : 얼라인 카메라 부분만 재접속 수행
 parm : None
 retn : None
*/
API_IMPORT VOID uvBasler_Reconnected();
/*
 desc : Mark Pattern 생성 윈도 핸들
 parm : hwnd	- [in]  마크 생성 윈도 핸들
 retn : None
*/
API_IMPORT VOID uvBasler_SetMarkHwnd(HWND hwnd);
/*
 desc : BIPS Server와 통신을 담당하는 Client 스레드 생성 및 실행
 parm : time_out	- [in]  기존 카메라와의 연결해제가 비정상적으로 종료되었을 때,
							카메라 내부에서 기존 연결된 클라이언트의 응답이 없을 때, 해제 대기 시간
							클라이언트로부터 임의 설정 시간(단위: 밀리초)동안 응답 없으면, 강제 해제
		work_dir	- [in]  작업 경로 (실행 파일 경로)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_UpdateSetParam(PTCHAR work_dir,
#ifdef _DEBUG
										UINT64 time_out=30000);
#else
										UINT64 time_out=5000);
#endif
/*
 desc : Mark Pattern 
 parm : inner	- [in]  내부 크기 (단위: pixel)
		outer	- [in]  외부 크기 (단위: pixel)
		reverse	- [in]  흑백 반전 여부 (0x00 - In:White/Out:Black, 0x01 - In:Black/Out:White)
		pattern	- [in]  마크 패턴 타입 (0x00 - Circle, 0x01 - Rectangle, 0x02 - Cross)
 retn : None
*/
API_IMPORT VOID uvBasler_CreateMark(LPRECT inner, LPRECT outer, UINT8 reverse, UINT8 pattern);
/*
 desc : 마크 그리기
 parm : file	- [in]  저장 파일 (전체 경로 포함)
		inner	- [in]  내부 크기 (단위: pixel)
		outer	- [in]  외부 크기 (단위: pixel)
		reverse	- [in]  흑백 반전 여부 (0x00 - In:White/Out:Black, 0x01 - In:Black/Out:White)
		pattern	- [in]  마크 패턴 타입 (0x00 - Circle, 0x01 - Rectangle, 0x02 - Cross)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_SaveMark(TCHAR *file,
								  LPRECT inner,
								  LPRECT outer,
								  UINT8 reverse,
								  UINT8 pattern);
/*
 desc : Vision Camera Mode 설정
 parm : mode	- [in]  en_vccm_xxx
 retn : None
*/
API_IMPORT VOID uvBasler_SetCamMode(ENG_VCCM mode);



/*
 desc : 현재 Camera의 동작 모드 값 반환
 parm : None
 retn : en_vccm_xxx
*/
API_IMPORT ENG_VCCM uvBasler_GetCamMode();
/*
 desc : 현재 PLC가 연결되어 있는지 여부 확인
 변수 :	cam_id	- [in]  Camera Index (1 or 2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_IsConnected(UINT8 cam_id);
/*
 desc : 현재 모든 Align Camera가 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_IsConnectedAll();
/*
 desc : 현재 등록된 Grabbed 이미지 개수 반환
 parm : None
 retn : 저장된 이미지 개수 반환
*/
API_IMPORT UINT16 uvBasler_GetGrabbedCount();
/*
 desc : Grabbed Images 모두 Score 값이 유효한지 값 반환
 parm : set_score	- [in]  If the score of the searched mark is higher than this set score, it is valid
  							(If 0.0f is unchecked) 
 retn : TRUE : 유효함, FALSE : 유효하지 않음
*/
API_IMPORT BOOL uvBasler_IsScoreValidAll(DOUBLE set_score);
/*
 desc : 검색된 마크 이미지가 포함된 구조체 포인터 반환
 parm : cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later) (if img_id == 0xff then Calibration Method)
 retn : 구조체 포인터 반환
*/
API_IMPORT LPG_ACGR uvBasler_GetGrabbedMark(UINT8 cam_id, UINT8 img_id);
/*
 desc : Grabbed Image의 데이터 반환
 parm : index	- [in]  가져오고자 하는 위치 (Zero based)
 retn : grab data 반환
*/
API_IMPORT LPG_ACGR uvBasler_GetGrabbedMarkIndex(UINT8 index);
/*
 desc : Grabbed Image의 결과 수정
 parm : grab	- [in]  저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_SetGrabbedMark(LPG_ACGR grab);
/*
 desc : 사용자에 의해 수동으로 검색된 경우, 결과 값만 처리
 parm : grab	- [in]  사용자에 의해 수동으로 입력된 grabbed image 결과 정보가 저장된 구조체 포인터
		gmfr	- [in]  GMFR Data
		gmsr	- [in]  GMSR Data
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_SetGrabbedMarkEx(LPG_ACGR grab, LPG_GMFR gmfr, LPG_GMSR gmsr);
/*
 desc : Grabbed Image의 데이터 수정
 parm : index	- [in]  가져오고자 하는 위치 (Zero based)
 parm : grab	- [in]  저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_SetGrabbedMarkIndex(UINT8 index, LPG_ACGR grab);
/*
 desc : Global 4개의 Mark에 대해서 모두 유효한지 검사
 parm : multi_mark	- [in]  Multi Mark (다중 마크) 적용 여부
		set_score	- [in]  If the score of the searched mark is higher than this set score, it is valid
 							(If 0.0f is unchecked) 
 retn : TRUE (유효함) or FALSE (4개 중 1개라도 제대로 인식 안됨)
*/
API_IMPORT BOOL uvBasler_IsGrabbedMarkValidAll(BOOL multi_mark, DOUBLE set_score);
/*
 desc : Global 4개의 Mark에 대해서 모두 유효한지 검사
 변수 :	cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : TRUE (유효함) or FALSE (4개 중 1개라도 제대로 인식 안됨)
*/
API_IMPORT BOOL uvBasler_IsValidGrabMarkNum(UINT8 cam_id, UINT8 img_id);
/*
 desc : Global 4개의 Mark에 대해서 모두 유효한지 검사
 변수 :	cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : TRUE (유효함) or FALSE (4개 중 1개라도 제대로 인식 안됨)
*/
API_IMPORT BOOL uvBasler_IsValidGrabMarkNum(UINT8 cam_id, UINT8 img_id);
/*
 desc : 기존 Grabbed 이미지 모두 제거
 parm : None
 retn : None
*/
API_IMPORT VOID uvBasler_ResetGrabbedImage();
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
API_IMPORT BOOL uvBasler_GetGrabbedMarkDirect(ENG_GMDD direct, STG_DBXY &data1, STG_DBXY &data2);
/*
 desc : Global Mark 대상으로, 2 개의 마크가 떨어진 거리 값 반환 (단위 : mm)
 parm : direct	- [in]  측정지점 (0 - 1번 마크와 3번 마크 간의 길이 값)
								 (1 - 2번 마크와 4번 마크 간의 길이 값)
								 (2 - 1번 마크와 2번 마크 간의 길이 값)
								 (3 - 3번 마크와 4번 마크 간의 길이 값)
								 (4 - 1번 마크와 4번 마크 간의 길이 값)
								 (5 - 2번 마크와 3번 마크 간의 길이 값)
 retn : 두 마크의 떨어진 거리 (단위: mm)
*/
API_IMPORT DOUBLE uvBasler_GetGrabbedMarkDist(ENG_GMDD direct);
/*
 desc : Calibration Image가 존재하면, Align Mark 수행
 변수 :	cam_id	- [in]  Camera Index (1 or 2)
		mode	- [in]  0xff : Calibration Mode (얼라인 카메라 각도 측정하지 않음)
						0xfe : Align Camera Angle Measuring Mode (얼라인 카메라 각도 측정하기 위함)
 retn : Calirbated Image의 정보가 저장된 구조체 포인터 반환
*/
API_IMPORT LPG_ACGR uvBasler_RunModelCali(UINT8 cam_id, UINT8 mode, UINT8 dlg_id, UINT8 mark_no, BOOL useMilDisp, UINT8 img_proc); // default mode = 0xff
/*
 desc : Geometric Model Find
 parm : cam_id	- [in]  Camera Index (1 or 2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_RunModelFind(UINT8 cam_id, UINT8 mark_no); // 미사용
/*
 desc : 가장 최근에 카메라 별로 Calibrated된 결과 중 Error 값이 저장된 데이터 반환
 parm : None
 retn : 매칭된 결과 값 반환
*/
API_IMPORT LPG_ACLR uvBasler_GetLastGrabbedACam();
/*
 desc : Step (Vision 단차) Image가 존재하면, Align Mark 수행
 변수 :	cam_id	- [in]  Camera Index (1 or 2)
		counts	- [in]  검색 대상의 Mark 개수 (찾고자하는 마크 개수)
		angle	- [in]  각도 적용 여부 (TRUE : 각도 적용, FALSE : 각도 적용 없음)
						TRUE : 현재 카메라의 회전된 각도 구하기 위함, FALSE : 기존 각도 적용하여 회전된 각도 구함
		results	- [out] 검색된 결과 정보가 저장될 구조체 참조 포인터 (grabbed image buffer까지 복사해옴)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_RunModelStep(UINT8 cam_id, UINT16 count, BOOL angle, LPG_ACGR results, UINT8 dlg_id, UINT8 mark_no, UINT8 img_proc);
/*
 desc : Examination (Vision Align 검사 (측정)) Image가 존재하면, Align Shutting 검사 수행
 변수 :	score		- [in]  크기 비율 값 (이 값보다 크거나 같으면 Okay)
		scale		- [in]  매칭 비율 값 (1.0 기준으로 +/- 이 값 범위 안에 있으면 Okay)
		results		- [out] 검색된 결과 정보가 저장될 구조체 참조 포인터 (grabbed image buffer까지 복사해옴)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_RunModelExam(DOUBLE score, DOUBLE scale, LPG_ACGR results=NULL, UINT8 find_mode = 0); // 미사용
/*
 desc : Grabbed Image가 존재하면, Edge Detection 수행
 변수 :	cam_id	- [in]  Camera Index (1 or 2)
		saved	- [in]  분석된 edge image 저장 여부
 retn : Edge Detected된 영역의 직경 (지름) 값 반환 (단위: Pixel)
*/
API_IMPORT BOOL uvBasler_RunEdgeDetect(UINT8 cam_id, UINT8 saved=0x00);
/*
 desc : Set the parameters of the strip maker
 parm : cam_id	- [in]  Align Camera ID (1 or 2 or later)
		param	- [in]  Structures pointer with information to set up is stored
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_RunMarkerStrip(UINT8 cam_id, LPG_MSMP param, STG_MSMR &results);
/*
 desc : 가장 최근 Grabbed Image의 매칭 검색 결과 구조체 포인터 반환
 parm : None
 retn : 객체 포인터 (매칭이 실패한 경우 NULL)
*/
API_IMPORT LPG_ACGR uvBasler_GetLastGrabbedMark();
/*
 desc : 가장 최근에 Calibrated된 결과 데이터 반환
 parm : None
 retn : 매칭된 결과 값 반환
*/
API_IMPORT LPG_ACGR uvBasler_GetLastGrabbedMarkEx();
/*
 desc : 카메라로 캡처 했을 때, 1 픽셀의 실제 크기 계산 (Width & Height 값)
 변수 :	cam_id	- [in]  Camera Index (1 or 2)
 retn : 크기 값 반환 (단위: um)
*/
API_IMPORT DOUBLE uvBasler_CalcCamPixelToUM(UINT8 cam_id);
#if 0
/*
 desc : 기존 Grabbed Image 초기화
 parm : None
 retn : None
*/
API_IMPORT VOID uvBasler_ResetGrabbedMark();
#endif
/*
 desc : 현재 Grabbed Image에 대한 각 Bytes 별로 저장된 Gray 값 비중 계산
 parm : cam_id	- [in]  Camera Index (1 or 2)
		mode	- [in]  Grabbed Image를 캡처하는 방식 (모드)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_SetGrabGrayRate(UINT8 cam_id, ENG_VCCM mode);
/*
 desc : 현재 Grabbed Image에 대한 각 Gray Level 값 (비율) 반환
 parm : cam_id	- [in]  Camera Index (1 or 2)
		index	- [in]  0 ~ 255에 해당되는 Gray Level Index
 retn : 0 ~ 255 들이 Grabbed Image에서 비율 값 반환
*/
API_IMPORT DOUBLE uvBasler_GetGrabGrayRate(UINT8 cam_id, UINT8 index);
/*
 desc : 전체 Grabbed Image에서 색깔이 있는 값의 비율 반환
 parm : cam_id	- [in]  Camera Index (1 or 2)
 retn : White 비율 값 반환
*/
API_IMPORT DOUBLE uvBasler_GetGrabGrayRateTotal(UINT8 cam_id);
/*
 desc : 현재 Grabbed Image에 대한 각 Bytes 별로 저장된 Gray 값 저장
		넓이에 대해 높이에 있는 모든 Gray 값이 합산하여 저장
 parm : cam_id	- [in]  Camera Index (1 or 2)
		mode	- [in]  Grabbed Image를 캡처하는 방식 (모드)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_SetGrabHistLevel(UINT8 cam_id, ENG_VCCM mode);
/*
 desc : 현재 Grabbed Image에 대한 각 Bytes 별로 저장된 Gray Level 반환
 parm : cam_id	- [in]  Camera Index (1 or 2)
 retn : 현재 Grabbed Image의 넓이 (Column)의 1 픽셀마다 높이의 Level 값들이 저장되어 있는 버퍼 반환
*/
API_IMPORT PUINT64 uvBasler_GetGrabHistLevel(UINT8 cam_id);
/*
 desc : 현재 Grabbed Image에 대한 각 Bytes 별로 저장된 Gray Level들의 총 합 값 반환
 parm : cam_id	- [in]  Camera Index (1 or 2)
 retn : Grabbed Image의 전체 Gray Level 값들의 합 반환
*/
API_IMPORT UINT64 uvBasler_GetGrabHistTotal(UINT8 cam_id);
/*
 desc : 현재 Grabbed Image에 대해 Column 별로 Gray Level 값 중 가장 큰 값
 parm : cam_id	- [in]  Camera Index (1 or 2)
 retn : Grabbed Image의 Column 별로 Gray Level 합 중 가장 큰 값 반환
*/
API_IMPORT UINT64 uvBasler_GetGrabHistMax(UINT8 cam_id);
/*
 desc : 얼라인 카메라의 Gain Level 값 설정
 parm : cam_id	- [in]  Camera Index (1 or 2)
		level	- [in] 0 ~ 255 (값이 클수록 밝게 처리)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_SetGainLevel(UINT8 cam_id, UINT8 level);
/*
 desc : 수동으로 이미지를 불러와서 데이터를 적재 합니다. (데모 모드에서 주로 사용 됨)
 parm : cam_id	- [in]  Camera Index (1 or 2)
		file	- [in]  전체 경로가 포함됨 265 grayscale bitmap 파일
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_LoadImageFromFile(UINT8 cam_id, PTCHAR file);


/* --------------------------------------------------------------------------------------------- */
/*                           Image Library 연결 (Gateway) 함수 부분                              */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Outputs the most recently generated Buffer ID value to the window
 parm : hwnd	- [in]  Handle of window where the image is output
		draw	- [in]  The area of window where the image is output
		cam_id	- [in]  Align Camera ID (1 or 2)
		type	- [in]  0x00: Edge Detection, 0x01: Line Detection, 0x02: Strip Detection (Measurement)
 retn : None
*/
API_IMPORT VOID uvBasler_DrawMBufID(HWND hwnd, RECT draw, UINT8 cam_id, UINT8 type=0x00);

/*
 desc : 현재 Grabbed된 이미지 출력 및 저장 (Bitmap을 이용하여 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		cam_id	- [in]  Camera Index (1 or 2)
 retn : None
*/
API_IMPORT VOID uvBasler_DrawGrabBitmap(HDC hdc, RECT draw, UINT8 cam_id);
/*
 desc : Draw the strip information in the current grabbed image
 parm : hdc		- [in]  The handle of context
		draw	- [in]  The area in which the image is output
		cam_id	- [in]  Camera Index (1 or 2)
		param	- [in]  Strip output information
 retn : None
*/
API_IMPORT VOID uvBasler_DrawStripBitmap(HDC hdc, RECT draw, UINT8 cam_id, LPG_MSMP param);
/*
 desc : Calibration 이미지 (검색된 결과)를 윈도 영역에 출력 수행 (Bitmap을 이용하여 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
 retn : None
*/
API_IMPORT VOID uvBasler_DrawCaliMarkBitmap(HDC hdc, RECT draw);
/*
 desc : 가장 마지막에 Grabbed Image를 출력
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
 retn : None
*/
API_IMPORT VOID uvBasler_DrawLastGrabImage(HDC hdc, RECT draw);
/*
 desc : 검색된 Mark 이미지 윈도 영역에 출력 수행 (DC를 이용하여 bitmap 이미지로 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_DrawMarkBitmap(HDC hdc, RECT draw, UINT8 cam_id, UINT8 img_id);
/*
 desc : 검색된 Mark 이미지 윈도 영역에 출력 수행 (MIL Buffer ID를 이용한 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_DrawMarkMBufID(HWND hwnd, RECT draw, UINT8 cam_id, UINT8 img_id);
/*
 desc : Calibration 이미지 윈도 영역에 출력 수행 (Bitmap을 이용하여 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		grab	- [in]  grab 이미지가 저장된 구조체 포인터 (grab_data에 이미지가 정보가 반드시 저장되어 있어야 됨)
		find	- [in]  마크 검색 성공 여부 (검색됐더라도, 검색 결과 값에 따라 달라짐)
						0x00 - 검색 결과 실패, 0x01 - 검색 결과 성공
 retn : None
*/
API_IMPORT VOID uvBasler_DrawMarkDataBitmap(HDC hdc, RECT draw, LPG_ACGR grab, UINT8 find);
/*
 desc : Drawing - Examination Object Image (Bitmap을 이용하여 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		grab_out- [in]  Grabbed Image 정보가 저장된 구조체 포인터 (바깥 링)
		grab_in	- [in]  Grabbed Image 정보가 저장된 구조체 포인터 (안쪽 원)
 retn : None
*/
API_IMPORT VOID uvBasler_DrawGrabExamBitmap(HDC hdc, RECT draw, LPG_ACGR grab_out, LPG_ACGR grab_in);
/*
 desc : 검색된 마크 이미지가 포함된 구조체 포인터 반환
 parm : cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later) (if img_id == 0xff then Calibration Method)
 retn : 구조체 포인터 반환
*/
API_IMPORT LPG_ACGR uvBasler_GetGrabbedImage(UINT8 cam_id, UINT8 img_id);
/*
 desc : Mark 정보 설정
 parm : cam_id		- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		detail_level- [in]  등록된 모델 이미지와 Grabbed Image이미지의 Edge 영역에 대한 비교 정도 값
							값이 클수록 상세히 비교하지만, 속도는 많이 느려지짐
							LOW보다 HIGH로 갈수록 더 많은 Edge를 가지고 비교하게 됨
							0 (M_HIGH), 1 (M_MEDIUM), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - 기본 값, 1.0f ~ 100.0f
							기본 값은 50이지만, 100으로 갈수록 엣지의 강도가 센것만 나타납니다.
		model		- [in]  Model Type, Param 1 ~ 5개 정보까지 모두 포함된 구조체 포인터
		count		- [in]  등록하고자 하는 모델의 개수
		아래 2개의 값이 0 값이면 파라미터 적용되지 않음
		scale_min	- [in]  검색 대상의 크기 범위 값 설정 (최소한 이 값 비율보다 커야 됨. 범위: 0.5 ~ 1.0)
		scale_max	- [in]  검색 대상의 크기 범위 값 설정 (최대한 이 값 비율보다 작아야 됨. 범위: 1.0 ~ 1.0)
		score_min	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0)
		score_tgt	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0))
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_SetModelDefine(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
										LPG_CMPV model, UINT8 mark_no,
										DOUBLE scale_min=0.0f, DOUBLE scale_max=0.0f,
										DOUBLE score_min=0.0f, DOUBLE score_tgt=0.0f);
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
		count		- [in]  등록하고자 하는 모델의 개수
		아래 2개의 값이 0 값이면 파라미터 적용되지 않음
		scale_min	- [in]  검색 대상의 크기 범위 값 설정 (최소한 이 값 비율보다 커야 됨. 범위: 0.5 ~ 1.0)
		scale_max	- [in]  검색 대상의 크기 범위 값 설정 (최대한 이 값 비율보다 작아야 됨. 범위: 1.0 ~ 1.0)
		score_min	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0)
		score_tgt	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0))
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_SetModelDefineEx(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
										  PUINT32 model, DOUBLE *param1, DOUBLE *param2, DOUBLE *param3,
										  DOUBLE *param4, DOUBLE *param5, UINT8 mark_no,
										  DOUBLE scale_min=0.0f, DOUBLE scale_max=0.0f,
										  DOUBLE score_min=0.0f, DOUBLE score_tgt=0.0f);
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
API_IMPORT BOOL uvBasler_SetModelDefineLoad(UINT8 cam_id, UINT8 speed, UINT8 level, DOUBLE smooth,
											DOUBLE scale_min, DOUBLE scale_max,
											DOUBLE score_min, DOUBLE score_tgt,
											PTCHAR name, CStringArray &file);

/*
 desc : 현재 등록된 Mark Model 정보 반환
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : 등록된 구조체 포인터 반환
*/
API_IMPORT LPG_CMPV uvBasler_GetModelDefine(UINT8 cam_id);
/*
 desc : 현재 검색을 위해 등록된 모델의 개수 반환
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : 등록된 개수
*/
API_IMPORT UINT32 uvBasler_GetModelRegistCount(UINT8 cam_id);
/*
 desc : 기존에 적재 (등록)되어 있던 모든 Mark 해제 수행
 parm : None
 retn : None
*/
API_IMPORT VOID uvBasler_ResetMarkModel();
/*
 desc : Mark Template가 등록되어 있는지 확인
 parm : mode	- [in]  0x00 : Expose, 0x01 : Vstep
		cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : TRUE - 등록되어 있다. FALSE - 등록되지 않았다.
*/
API_IMPORT BOOL uvBasler_IsSetMarkModel(UINT8 mode=0x00, UINT8 cam_id=0x01, UINT8 fi_No = 0x00);
/*
 desc : Mark Pattern 등록 여부
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_IsSetMarkModelACam(UINT8 cam_id, UINT8 mark_no);
/*
 desc : 모든 카메라 (카메라 개수에 따라)에 Mark Model이 설정되었는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_IsSetMarkModelAll(UINT8 mark_no);
/*
 desc : 가장 최근에 Edge Detection된 이미지에서 특정 영역만 추출해서 Bitmap Image로 등록
		즉, Mark Template 이미지로 등록
 parm : cam_id	- [in]  Align Camera ID (1 or Later)
		area	- [in]  추출하고자 하는 영역 정보가 저장된 픽셀 위치 값
		type	- [in]  0x00 : Edge, 0x02 : Line, 0x02 : Match, 0x03 : Grab
		file	- [in]  Mark Template를 저장하려는 파일 이름 (전체 경로 포함)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_SaveGrabbedMarkToFile(UINT8 cam_id, LPRECT area, UINT8 type, PTCHAR file);
/*
 desc : Edge Detection 검색된 결과 반환
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
 retn : 결과 값이 저장된 포인터 반환
*/
API_IMPORT LPG_EDFR uvBasler_GetEdgeDetectResults(UINT8 cam_id);
/*
 desc : Edge Detection 검색된 개수 반환
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
 변환 : 개수 반환. 실패할 경우, 0 이하 값
*/
API_IMPORT INT32 uvBasler_GetEdgeDetectCount(UINT8 cam_id);
/*
 desc : 현재 등록된 Mark Model Type 반환
 parm : cam_id	- [in]  Align Camera Index (0x01 ~ MAX_INSTALL_CAMERA)
		mark_id	- [in]  Align Mark Index (0x00 ~ MAX_REGIST_MODEL-1)
 retn : Model Type 값
*/
API_IMPORT UINT32 uvBasler_GetMarkModelType(UINT8 cam_id, UINT8 index);
/*
 desc : 모델 크기 반환
 parm : cam_id	- [in]  Grabbed Image 정보가 발생된 Align Camera Index (1 or 2)
		index	- [in]  요청하고자 하는 모델의 위치 (Zero-based)
		flag	- [in]  0x00 : 가로  크기, 0x01 : 세로 크기
		unit	- [in]  0x00 : um, 0x01 : pixel
 retn : 크기 반환 (단위: um)
*/
API_IMPORT DOUBLE uvBasler_GetMarkModelSize(UINT8 cam_id, UINT8 index=0x00, UINT8 flag=0x00, UINT8 unit=0x00);
/*
 desc : 이미지의 가로 혹은 세로의 경계선 중심 위치 얻기
 parm : cam_id		- [in]  Align Camera ID (1 or 2)
		grab_data	- [in]  Grabbed Image 정보가 저장된 구조체 포인터
		mil_result	- [in]  MIL 내부 검색 결과 대화 상자 출력 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvBasler_RunLineCentXY(UINT8 cam_id, LPG_ACGR grab_data, BOOL mil_result);
/*
 desc : Iamge 회전 (이미지의 중심을 기준으로 회전)
 parm : img_src		- [in]  원본 이미지 버퍼
		img_dst		- [out] 회전된 이미지 버퍼
		width		- [in]	원본 이미지의 넓이
		height		- [in]	원본 이미지의 높이
		angle		- [in]	회전 하고자 하는 각도
 retn : None
*/
API_IMPORT VOID uvBasler_ImageRotate(PUINT8 img_src, PUINT8 img_dst,
									 UINT32 width, UINT32 height, UINT32 angle);
/*
 desc : 현재 MIL 라이선스가 유효한지 여부 확인
		Pattern, Model Finder, Edge Finder, Read, OCR, Image Processing 등 유효한지 확인
 parm : None
 retn : TRUE or FALS
*/
API_IMPORT BOOL uvBasler_IsLicenseValid();
/*
 desc : Align Mark 검색 방식 설정
 parm : method	- [in]  0x00 : 최적의 1개 반환, 0x01 : 다점 마크 기준으로 가중치를 부여하여 결과 반환
		count	- [in]  'method' 값이 1인 경우, 최종 검색될 Mark 개수 값 2 이상 값이어야 됨
 retn : None
*/
API_IMPORT VOID uvBasler_SetMarkMethod(ENG_MMSM method, UINT8 count=0x00);
/*
 desc : 노광 모드 설정 즉, 직접 노광, 얼라인 노광, 보정 후 얼라인 노광
 parm : mode	- [in]  직접 노광 (0x00), 얼라인 노광 (0x01), 얼라인 카메라 보정 값 적용 후 얼라인 노광 (0x02)
 retn : None
*/
API_IMPORT VOID uvBasler_SetAlignMode(ENG_AOEM mode);
/*
 desc : 현재 적재된 레시피의 마크 검색 조건 값 설정
 parm : score	- [in]  Score Accpetance (대상 이미지에서 검색된 마크의 Score 값이 이 값보다 작으면 버린다)
		scale	- [in]  Scale Range (대상 이미지에서 등록된 마크를 검색 결과들 중에서 이 비율 범위 내에 )
									(포함된 결과들만 유효하고, 그 중에서 Score 값이 가장 높은 것만 추출)
									(가령. 이 값이 50.0000 이면, 실제 검색에 사용되는 마크의 크기 범위)
									(검색범위 = 1.0f - (100.0 - 50.0) / 100 임 즉, 0.5 ~ 1.5 임)
 retn : None
*/
API_IMPORT VOID uvBasler_SetRecipeMarkRate(DOUBLE score, DOUBLE scale);
/*
 desc : 스테이지의 이동 방향 설정 (정방향 이동인지 / 역방향 이동인지 여부)
 parm : direct	- [in]  TRUE (정방향 이동 : 앞에서 뒤로 이동), FALSE (역방향 이동)
 retn : None
*/
API_IMPORT VOID uvBasler_SetMoveStateDirect(BOOL direct);
/*
 desc : Grabbed Image 마다 표현된 마크의 개수가 여러 개인 경우, 마크 들이 분포된 영역의 크기 값 설정
 parm : width	- [in]  사각형 영역의 넓이 값 (단위: um)
		height	- [in]  사각형 영역의 높이 값 (단위: um)
 retn : None
*/
API_IMPORT VOID uvBasler_SetMultiMarkArea(UINT32 width, UINT32 height);


/* ----------------------------------------------------------------------------------------- */
/*                                 lk91 VISION 추가 함수                                     */
/* ----------------------------------------------------------------------------------------- */
API_IMPORT BOOL uvBasler_SetModelDefineMMF(UINT8 cam_id, PTCHAR name, PTCHAR mmf, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no);
API_IMPORT BOOL uvBasler_SetModelDefinePAT(UINT8 cam_id, PTCHAR name, PTCHAR pat, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no);
API_IMPORT VOID uvBasler_DrawLiveBitmap(HDC hdc, RECT draw, UINT8 cam_id, BOOL save);
API_IMPORT VOID uvBasler_DrawImageBitmap(int dispType, int Num, UINT8 cam_id, BOOL save);
API_IMPORT VOID uvBasler_SetMarkLiveDispSize(CSize fi_size);
API_IMPORT VOID uvBasler_SetCalbCamSpecDispSize(CSize fi_size);
API_IMPORT VOID uvBasler_SetAccuracyMeasureDispSize(CSize fi_size);
API_IMPORT VOID uvBasler_SetCalbStepDispSize(CSize fi_size);
API_IMPORT VOID uvBasler_SetMMPMDispSize(CSize fi_size);
API_IMPORT VOID uvBasler_SetDispType(UINT8 dispType);
API_IMPORT VOID uvBasler_MilSetMarkROI(UINT8 cam_id, CRect fi_rectSearhROI);
API_IMPORT BOOL uvBasler_RegistPat(UINT8 cam_id, CRect fi_rectArea, CString fi_filename, UINT8 mark_no);
API_IMPORT BOOL uvBasler_RegistMod(UINT8 cam_id, CRect fi_rectArea, CString fi_filename, UINT8 mark_no);
API_EXPORT VOID uvBasler_InitSetMarkSizeOffset(UINT8 cam_id, TCHAR* file, UINT8 fi_findType, UINT8 fi_No);
API_EXPORT VOID uvBasler_PutMarkDisp(HWND hwnd, int fi_iNo, RECT draw, UINT8 cam_id, TCHAR* file, int fi_findType);
API_EXPORT BOOL uvBasler_PutSetMarkDisp(HWND hwnd, RECT draw, UINT8 cam_id, TCHAR* file, int fi_findType, DOUBLE fi_dRate);
API_EXPORT CPoint uvBasler_GetMarkSize(UINT8 cam_id, int fi_No);
API_EXPORT DOUBLE uvBasler_SetMarkSetDispRate(DOUBLE fi_dRate);
API_EXPORT DOUBLE uvBasler_GetMarkSetDispRate();
API_EXPORT CPoint uvBasler_GetMarkOffset(UINT8 cam_id, BOOL bNewOffset, int fi_No);
API_EXPORT BOOL uvBasler_MaskBufGet();
API_EXPORT UINT8 uvBasler_GetMarkFindMode(UINT8 cam_id, UINT8 mark_no);
API_EXPORT BOOL uvBasler_SetMarkFindMode(UINT8 cam_id, UINT8 find_mode, UINT8 mark_no);
API_EXPORT VOID uvBasler_SetMarkSize(UINT8 cam_id, CPoint fi_MarkSize, int setMode, int fi_No);
API_EXPORT VOID uvBasler_SetMarkOffset(UINT8 cam_id, CPoint fi_MarkCenter, int setOffsetMode, int fi_No);
API_EXPORT VOID uvBasler_MaskClear_MOD(UINT8 cam_id, CPoint fi_iSizeP, UINT8 mark_no);
API_EXPORT VOID uvBasler_MaskClear_PAT(UINT8 cam_id, CPoint fi_iSizeP, UINT8 mark_no);
API_EXPORT VOID uvBasler_MarkSetCenterFind(int cam_id, int fi_length, int fi_curSmoothness, double* fi_NumEdgeMIN_X, double* fi_NumEdgeMAX_X, double* fi_NumEdgeMIN_Y, double* fi_NumEdgeMAX_Y, int* fi_NumEdgeFound);
API_EXPORT VOID uvBasler_SaveMask_MOD(UINT8 cam_id, UINT8 mark_no);
API_EXPORT VOID uvBasler_SaveMask_PAT(UINT8 cam_id, UINT8 mark_no);
API_EXPORT VOID uvBasler_PatMarkSave(UINT8 cam_id, CString fi_strFileName, UINT8 mark_no);
API_EXPORT VOID uvBasler_ModMarkSave(UINT8 cam_id, CString fi_strFileName, UINT8 mark_no);
API_EXPORT VOID uvBasler_SetDispMark(CWnd* pWnd);
API_EXPORT VOID uvBasler_SetDispRecipeMark(CWnd* pWnd[2]);
API_EXPORT VOID uvBasler_SetDispMarkSet(CWnd* pWnd);
API_EXPORT VOID uvBasler_SetDisp(CWnd* pWnd[2], UINT8 fi_Mode);
API_EXPORT VOID uvBasler_SetDispMMPM(CWnd* pWnd);
API_EXPORT VOID uvBasler_SetDispExpo(CWnd* pWnd[4]);
API_EXPORT VOID uvBasler_DrawOverlayDC(bool fi_bDrawFlag, int fi_iDispType, int fi_iNo);
API_EXPORT VOID uvBasler_OverlayAddBoxList(int fi_iDispType, int fi_iNo, int fi_iLeft, int fi_iTop, int fi_iRight, int fi_iBottom, int fi_iStyle, int fi_color);
API_EXPORT VOID uvBasler_OverlayAddCrossList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, int fi_iWdt1, int fi_iWdt2, int fi_color);
API_EXPORT VOID uvBasler_OverlayAddTextList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, CString fi_sText, int fi_color, int fi_iSizeX, int fi_iSizeY, CString fi_sFont, bool fi_bEgdeFlag);
API_EXPORT VOID uvBasler_OverlayAddLineList(int fi_iDispType, int fi_iNo, int fi_iSx, int fi_iSy, int fi_iEx, int fi_iEy, int fi_iStyle, int fi_color);
API_EXPORT VOID uvBasler_DrawMarkInfo_UseMIL(UINT8 cam_id, UINT8 fi_smooth, UINT8 mark_no);
API_EXPORT VOID uvBasler_Mask_MarkSet(UINT8 cam_id, CRect rectTmp, CPoint iTmpSizeP, CRect rectFill, int fi_color, bool bMask);
API_EXPORT VOID uvBasler_InitMask();
API_EXPORT VOID uvBasler_CloseSetMark();
API_EXPORT VOID uvBasler_MilZoomIn(int fi_iDispType, int cam_id, CRect rc);
API_EXPORT BOOL uvBasler_MilZoomOut(int fi_iDispType, int cam_id, CRect rc);
API_EXPORT VOID uvBasler_MilAutoScale(int fi_iDispType, int cam_id);
API_EXPORT VOID uvBasler_SetZoomDownP(int fi_iDispType, int cam_id, CPoint fi_point);
API_EXPORT VOID uvBasler_MoveZoomDisp(int fi_iDispType, int cam_id, CPoint fi_point, CRect fi_rect);
API_EXPORT CDPoint uvBasler_trZoomPoint(int fi_iDispType, int cam_id, CPoint fi_point);
API_IMPORT BOOL uvBasler_SetModelDefine_tot(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
	LPG_CMPV model, UINT8 fi_No, TCHAR* file,
	DOUBLE scale_min = 0.0f, DOUBLE scale_max = 0.0f,
	DOUBLE score_min = 0.0f, DOUBLE score_tgt = 0.0f);
API_IMPORT BOOL uvBasler_MergeMark(UINT8 cam_id, LPG_CMPV value, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, UINT8 mark_no, TCHAR* file1, TCHAR* file2,
	TCHAR* RecipeName, DOUBLE scale_min = 0.0f, DOUBLE scale_max = 0.0f,
	DOUBLE score_min = 0.0f, DOUBLE score_tgt = 0.0f);
API_EXPORT BOOL uvBasler_ProcImage(UINT8 cam_id, UINT8 imgProc);
API_IMPORT CDPoint uvBasler_RunModel_VisionCalib(UINT8 cam_id, UINT8 dlg_id, UINT8 mark_no, int* roi_left, int* roi_right, int* roi_top, int* roi_bottom, int row, int col);
#ifdef __cplusplus
}
#endif