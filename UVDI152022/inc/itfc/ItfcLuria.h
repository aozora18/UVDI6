
/*
 desc : Luria TCP/IP Library
*/

#pragma once

#include "../conf/global.h"
#include "../conf/luria.h"
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
#include "../conf/conf_podis.h"
#elif (CUSTOM_CODE_GEN2I == DELIVERY_PRODUCT_ID)
#include "../conf/conf_gen2i.h"
#endif
#include "../ItfcLuria/JobSelectXml.h"
#include "../UVDI15/GlobalVariables.h"
#ifdef __cplusplus
extern "C" {
#endif

/*
 desc : Luria와의 통신 시작
 parm : config		- [in]  환경 설정 정보
		shmem		- [in]  Luria Service 관련 공유 메모리
		error		- [in]  각종 에러 정보를 관리하는 객체
		init_send	- [in]  접속 이후 초기화 명령을 송신할지 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_Init(LPG_CIEA config, LPG_LDSM shmem, CCodeToStr *error, BOOL init_send=TRUE);
/*
 desc : Luria와의 통신 시작
 parm : config	- [in]  환경 설정 정보
		shmem	- [in]  Luria Service 관련 공유 메모리
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_Open(LPG_CIEA config, LPG_LDSM shmem);
/*
 desc : Luria와의 통신 정지
 parm : None
 retn : None
*/
API_IMPORT VOID uvLuria_Close();
/*
 desc : Luria Status 코드와 매핑되는 에러 문자열 (설명) 반환
 parm : code	- [in]  Luria Status (Error) Code
		mesg	- [in]  Return buffer
		size	- [in]  'mesg' buffer size
 retn : None
*/
API_IMPORT VOID uvLuria_GetLuriaStatusToDesc(UINT16 code, PTCHAR mesg, UINT32 size);
/*
 desc : 패킷 수신 여부
 parm : flag	- [in]  패킷 구분 명령어 (Family ID와 User ID가 병합된 코드 값)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_IsRecvPktData(ENG_FDPR flag);
/*
 desc : 가장 마지막 (최근)에 수신된 명령어 응답 여부
 parm :  None
 retn :  TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_IsLastRecvCmd();
/*
 desc : 기존 연결을 해제하고, 새로 접속을 수행
 parm : init_send	- [in]  접속 이후 초기화 명령을 송신할지 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_Reconnected(BOOL init_send=TRUE);
/*
 desc : 현재 Luria에 설정된 기준으로, 노광 최대 속도 값 반환
 parm : step_size	- [in]  Luria Step Size (1 or Later)
 retn : 노광 속도 값
*/
API_IMPORT DOUBLE uvLuria_GetExposeMaxSpeed(UINT8 step_size);
/*
 desc : 광학계 별 노광 에너지 구하기
 parm : led_watt	- [in]  4개의 LED 에 입력된 Power Watt (unit: Watt)
		step_size	- [in]  Luria Step Size (1 or Later)
		frame_rate	- [in]  Luria Frame Rate (1 ~ 1000)
		duty_cycle	- [in]  LED Duty Cycle (1 ~ 100 %)
 retn : 노광 에너지 (mJ)
*/
API_IMPORT DOUBLE uvLuria_GetExposeEnergy(UINT8 step_size, UINT8 duty_cycle,
										  UINT16 frame_rate, DOUBLE *watt);
/*
 desc : 현재 선택된 레시피의 속성 정보를 통해 스테이지 이동 속도 및 광량 값 계산
 parm : frame_rate	- [in]  Expose Frame Rate (0 ~ 1000)
		step_size	- [in]  Step Size (1 ~ 7)
 retn : 노광 속도 (mm/sec)
*/
API_IMPORT DOUBLE uvLuria_GetExposeSpeed(UINT16 frame_rate, UINT8 step_size);

/*
 desc : 현재 선택된 레시피의 속성 정보로 노광 에너지로 스테이지 속도 계산
 parm : step_size	- [in]  Luria Step Size (1 or Later)
		expo_energy	- [DOUBLE]  노광 에너지 (unit: mJ)
		duty_cycle	- [in]  LED Duty Cycle (1 ~ 100 %)
		led_watt	- [in]  4개의 LED 에 입력된 Power Watt (unit: Watt)
 retn : Expose Frame Rate (0 ~ 1000)
*/
API_IMPORT UINT16 uvLuria_GetEnergyToSpeed(UINT8 step_size, DOUBLE expo_energy,
	int duty_cycle, DOUBLE* watt);

/*
 desc : 현재 선택된 레시피의 속성 정보로 스테이지 속도로 노광 에너지 계산
 parm : step_size	- [in]  Luria Step Size (1 or Later)
		Expose Frame Rate (0 ~ 1000)
		duty_cycle	- [in]  LED Duty Cycle (1 ~ 100 %)
		led_watt	- [in]  4개의 LED 에 입력된 Power Watt (unit: Watt)
 retn : expo_energy	- [DOUBLE]  노광 에너지 (unit: mJ)
*/
API_IMPORT DOUBLE uvLuria_GetSpeedToEnergy(UINT8 step_size, UINT16 frame_rate,
	int duty_cycle, DOUBLE* watt);

/*
 desc : Luria Server에 연결 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_IsConnected();
/*
 desc : Luria Service (광학계)에 연결되고 난 이후, 초기화 명령들이 모두 송/수신 완료되었는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_IsServiceInited();
/*
 desc : Verify that the send buffer is full
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_IsSendBuffFull();
/*
 desc : Returns the empty rate of the send buffer
 parm : None
 retn : percentage (unit: %)
*/
API_IMPORT FLOAT uvLuria_GetSendBuffEmptyRate();
/*
 desc : Returns the frequency value corresponding to the LED number
 parm : led_no	- [in]  LED Number (1:365, 2:385, 3:395, 4:405)
 retn : Frequency (365nm, 385nm, 395nm, 405nm)
*/
API_IMPORT UINT16 uvLuria_GetLedNoToFreq(UINT8 led_no);
/*
 desc : 현재 제어SW의 작업 (Sequence)이 동작 중인지 여부 (Luria에게 통신 부하를 줄이기 위함)
 parm : flag	- [in]  TRUE or FALSE
 retn : None
*/
API_IMPORT VOID uvLuria_SetWorkBusy(BOOL flag);
API_IMPORT BOOL uvLuria_IsSetWorkBusy();

/* --------------------------------------------------------------------------------------------- */
/*                                             XML                                               */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 거버 XML 파일 내에, Global Fiducial 값 얻어오기 <Fiducials> -> <Global> -> <fid> -> <gbr>
 parm : job		- [in]  거버 이름 (전체 경로)
		lst_x	- [out] Fiducial X (gbr) 위치 (mm)가 등록될 참조 리스트
		lst_y	- [out] Fiducial Y (gbr) 위치 (mm)가 등록될 참조 리스트
		type	- [in]  Align Mark Type (ENG_ATGL)
 retn : 0x00 - succ, 0x01 - xml 파일 적재 실패, 0x02 - global mark 개수 부족, 0x03 - 마크 정보 얻기 실패
*/
API_IMPORT UINT8 uvLuria_GetGlobalMarkJobName(PCHAR job,
											  CAtlList <DOUBLE> &lst_x, CAtlList <DOUBLE> &lst_y,
											  ENG_ATGL type);

API_IMPORT UINT8 uvLuria_GetGlobalMarkJobNameVector(PCHAR job,
											  std::vector < STG_XMXY >& vstXMXY,
											  ENG_ATGL type);

/*
 desc : 거버 XML 파일 내에, Local Fiducial 값 얻어오기 <Fiducials> -> <Global> -> <fid> -> <gbr>
 parm : job		- [in]  거버 이름 (전체 경로)
		lst_x	- [out] Fiducial X (gbr) 위치 (mm)가 등록될 참조 리스트
		lst_y	- [out] Fiducial Y (gbr) 위치 (mm)가 등록될 참조 리스트
		type	- [in]  Gerber Data에 저장된 Mark Type (ENG_ATGL)
 retn : 0x00 - succ, 0x01 - xml 파일 적재 실패, 0x02 - global mark 개수 부족, 0x03 - 마크 정보 얻기 실패
*/
API_IMPORT UINT8 uvLuria_GetLocalMarkJobName(PCHAR job,
											 CAtlList <DOUBLE> &lst_x, CAtlList <DOUBLE> &lst_y,
											 ENG_ATGL type);

API_IMPORT UINT8 uvLuria_GetLocalMarkJobNameVector(PCHAR job,
											 std::vector < STG_XMXY > &vstXMXY, ENG_ATGL type);
											 

/*
 desc : 거버 XML 파일 내에, Global & Local Fiducial 값 얻어오기 <Fiducials> -> <Global> -> <fid> -> <gbr>
 parm : job		- [in]  거버 이름 (전체 경로)
		g_lst_x	- [out] Global Fiducial X (gbr) 위치 (mm)가 등록될 참조 리스트
		g_lst_y	- [out] Global Fiducial Y (gbr) 위치 (mm)가 등록될 참조 리스트
		l_lst_x	- [out] Local Fiducial X (gbr) 위치 (mm)가 등록될 참조 리스트
		l_lst_y	- [out] Local Fiducial Y (gbr) 위치 (mm)가 등록될 참조 리스트
		align	- [in]  Gerber Data에 저장된 Mark Type (ENG_ATGL)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_GetMarkJobName(PCHAR job,
									   CAtlList <DOUBLE> &g_lst_x, CAtlList <DOUBLE> &g_lst_y, 
									   CAtlList <DOUBLE> &l_lst_x, CAtlList <DOUBLE> &l_lst_y,
									   ENG_ATGL align);
/*
 desc : 현재 선택된 거버의 Panel Data 설정 정보 반환
 parm : None
 retn : panel data 정보가 저장된 구조체 포인터 반환
*/
API_IMPORT LPG_XDPD uvLuria_GetSelPanelData();
/*
 desc : Serial String (Serial D-Code)의 하위 Field 개수 반환
 parm : None
 retn : 0 (검색된 Serial D-Code 값이 없거나 Field가 없는 경우) or Later
*/
API_IMPORT UINT16 uvLuria_GetSerialDCodeFieldCount();
/*
 desc : 거버 XML 파일 내에, Stripe 전체 개수 및 Gerber Size 얻기
 parm : job		- [in]  거버 이름 (전체 경로)
		stripe	- [out] 전체 Stripe 개수
		width	- [out] 노광 넓이 (unit : mm)
		height	- [out] 노광 높이 (unit : mm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_GetGerberStripSize(PCHAR job, UINT8 &stripe, DOUBLE &width, DOUBLE &height);
/*
 desc : 거버 XML 파일 내에, Mark 개수 얻기
 parm : job		- [in]  거버 이름 (전체 경로)
		type	- [in]  Mark Type (ENG_AMTF)
 retn : 마크 개수 (0. 검색 실패). 1 or Later
*/
API_IMPORT UINT8 uvLuria_GetGerberMarkCount(PCHAR job, ENG_AMTF type);
/*
 desc : Local Fiducial 좌표 값 얻기 - Left / Bottom
 parm : scan	- [in]  스캔하려는 위치 (번호. 0 or 1)
		cam_id	- [in]  Align Camera Index (1 or 2)
		data	- [out] Fiducial 값이 저장될 참조 변수
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_GetLocalBottomMark(UINT8 scan, UINT8 cam_id, STG_XMXY &data);
/*
 desc : Global Fiducial의 Left/Bottom or Right/Bottom Mark와 Local Fiducial의 임의 위치에 해당되는
		높이 차이 값 즉, 떨어진 간격 값
 parm : direct	- [in]  Global Fiducial의 위치 정보. 0x00 : Left/Bottom, 0x01 : Right/Bottom
		index	- [in]  Local Fiducial의 위치 정보가 저장된 인덱스 (Zero-based)
 retn : 두 마크 간의 떨어진 오차 값 (unit: mm)
*/
API_IMPORT DOUBLE uvLuria_GetGlobalBaseMarkLocalDiffY(UINT8 direct, UINT8 index);
/*
 desc : 마크 구성 정보를 보고, 입력된 마크 위치(번호)가 정방향에 속하는지 역방향에 속하는지 확인
 parm : mark_no	- [in]  Align Mark Number (0 or Later)
 retn : TRUE (Forward) or FALSE (Backward)
*/
//API_IMPORT BOOL uvLuria_IsMarkDirectForward(UINT8 mark_no);
/*
 desc : XML 파일로에 설정(저장)된 총 노광 횟수 즉, Stripe 개수
 parm : None
 retn : 0 (Failed) or Later
*/
API_IMPORT UINT8 uvLuria_GetGerberStripeCount();
/*
 desc : 3점 Global Mark로 구성된 경우, 배치 정보 반환
 parm : None
 retn : 3점 Mark 배치 정보 값 반환
		0x0000 - 3점 마크 아님
		0x0111 - 1번 마크 없음
		0x1011 - 2번 마크 없음
		0x1101 - 3번 마크 없음
		0x1110 - 4번 마크 없음
*/
API_IMPORT UINT16 uvLuria_GetGlobalMark3PType();
/*
 desc : Return Global Fiducial Data loaded from XML file. (Local & Global)
 parm : index	- [in]  This is the location (Index) value where the Mark is stored. (Zero based)
		point	- [out] Gerber X / Y Position (unit: mm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_GetGlobalMark(UINT16 index, STG_XMXY &point);
/*
 desc : Get the position for two coordinates of the global fiducial
 parm : direct	- [in]  direction
		data1	- [out] Gerber XY & Motion XY Position (unit: mm)
		data2	- [out] Gerber XY & Motion XY Position (unit: mm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_GetGlobalMarkDirect(ENG_GMDD direct, STG_XMXY &data1, STG_XMXY &data2);
/*
 desc : XML 파일로부터 적재된 Local Fiducial Data 반환 (Local & Global)
 parm : index	- [in]  Mark가 저장되어 있는 Index 값 (Zero based)
		point	- [out] Gerber X / Y Position (unit: mm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_GetLocalMark(UINT16 index, STG_XMXY &point);
/*
 desc : 거버에 등록된 Fiducial 개수 반환
 parm : mark	- [in]  Mark Type (ENG_AMTF)
 retn : 개수
*/
API_IMPORT UINT8 uvLuria_GetMarkCount(ENG_AMTF mark);
/*
 desc : 거버 크기 반환
 parm : width	- [out] 넓이 값 반환 (unit: mm)
		height	- [out] 높이 값 반환 (unit: mm)
 retn : None
*/
API_IMPORT VOID uvLuria_GetGerberSize(DOUBLE &width, DOUBLE &height);
/*
 desc : 거버 크기 반환
 parm : None
 retn : Gerber Width / Height (unit : mm), 실패 : 0.0f
*/
API_IMPORT DOUBLE uvLuria_GetGerberWidth();
API_IMPORT DOUBLE uvLuria_GetGerberHeight();
/*
 desc : 1 Scan 기준으로 저장된 Local Mark 개수 반환
 parm : None
 retn : 개수
*/
API_IMPORT UINT8 uvLuria_GetLocalMarkCountPerScan();
/*
 desc : 거버 데이터 내에 Local과 Global Mark 모두 등록되어 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_IsMarkMixedGlobalLocal();
/*
 desc : XML 파일로부터 Fiducial Data 읽기 (Global & Local)
 parm : align_type	- [in]  Gerber Data에 저장된 Mark Type (ENG_ATGL)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_SelectedRegistrationXML(ENG_ATGL align_type);
/*
 desc : XML 파일로부터 XML 데이터 읽기
 parm : job		- [in]  전체 경로가 포함된 Job Name (xml 파일이 저장되어 있는 폴더 이름)
		align	- [in]  Gerber Data에 저장된 Mark Type (ENG_ATGL)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_LoadRegistrationXML(PCHAR job, ENG_ATGL align);
/*
 desc : 거버 XML 파일 내에, 포토헤드 X 단차 값 얻기
 parm : ph_no	- [in]  Photohead Number (1-based. 0x01 ~ 0x08)
 retn : PH 간의 떨어진 거리 값 (PH[0] = 0.0)
*/
API_IMPORT DOUBLE uvLuria_GetGerberPhDistX(UINT8 ph_no);
/*
 desc : Mark Type 기준으로, Camera 1번의 Left/Bottom과 Camera 2번의 Right/Bottom의 Y 좌표 간의 높이 차 반환
 parm : None
 retn : 두 지점의 Y 축 간의 높이 차이 (unit: mm)
*/
API_IMPORT DOUBLE uvLuria_GetGlobalLeftRightBottomDiffY();
/*
 desc : 전역 기준점의 X 축 기준으로 2 개의 마크가 떨어진 간격 반환 (단위 : mm)
 parm : direct	- [in]  축의 마크 간격 (0 - 1번 마크와 3번 마크 간의 넓이 값)
									   (1 - 2번 마크와 4번 마크 간의 넓이 값)
									   (2 - 1번 마크와 2번 마크 간의 넓이 값)
									   (3 - 3번 마크와 4번 마크 간의 넓이 값)
									   (4 - 1번 마크와 4번 마크 간의 넓이 값)
									   (5 - 2번 마크와 3번 마크 간의 넓이 값)
 retn : X 축의 2개의 마크 떨어진 간격 (unit: mm) 반환
*/
API_IMPORT DOUBLE uvLuria_GetGlobalMarkDist(ENG_GMDD direct);

//retn: 글로벌피듀셜 ptr 반환 
API_IMPORT CFiducialData* uvLuria_GetGlobalMarkPtr();

//retn: 로컬피듀셜 ptr 반환 
API_IMPORT CFiducialData* uvLuria_GetLocalMarkPtr();
/*
 desc : 두 Mark 간의 좌/우 X 축 떨어진 간격 즉, X 축 간의 오차 (거리) 값
 parm : mark_x1	- [in]  X1 축 Mark Number (1-based. 0x01 ~ 0x04)
		mark_x2	- [in]  X2 축 Mark Number (1-based. 0x01 ~ 0x04)
 retn : 두 지점의 Y 축 간의 높이 차이 (unit: mm)
*/
API_IMPORT DOUBLE uvLuria_GetGlobalMarkDiffX(UINT8 mark_x1, UINT8 mark_x2);
/*
 desc : 두 Mark 간의 상/하 Y 축 떨어진 간격 즉, Y 축 간의 오차 (높이) 값
 parm : mark_y1	- [in]  Y1 축 Mark Number (1-based. 0x01 ~ 0x04)
		mark_y2	- [in]  Y2 축 Mark Number (1-based. 0x01 ~ 0x04)
 retn : 두 지점의 Y 축 간의 높이 차이 (unit: mm)
*/
API_IMPORT DOUBLE uvLuria_GetGlobalMarkDiffY(UINT8 mark_y1, UINT8 mark_y2);
/*
 desc : Mark 1번과 3번, Mark 2번과 4번의 X 좌표 오차 값을 반환
 parm : type	- [in]  0 : Mark 2번 기준으로 Mark 1번의 X 오차 값 반환
						1 : Mark 4번 기준으로 Mark 3번의 X 오차 값 반환
						2 : Mark 1번 기준으로 Mark 2번의 X 오차 값 반환
						3 : Mark 3번 기준으로 Mark 4번의 X 오차 값 반환
 retn : 두 마크의 Y 위치 시작 오차 값 반환 (unit: mm)
*/
API_IMPORT DOUBLE uvLuria_GetGlobalMarkDiffVertX(UINT8 type);
/*
 desc : Align Camera 1번과 2번 간의 Mark X 축 떨어진 간격
 parm : mode	- [in]  0x00 : Width, 0x01 : Height
		scan	- [in]  스캔하려는 위치 (번호. 0 or 1)
 retn : 두 지점의 X 축 간의 떨어진 거리 (unit: mm)
*/
API_IMPORT DOUBLE uvLuria_GetLocalMarkACam12DistX(UINT8 mode, UINT8 scan);
/*
 desc : 두 Mark 간의 상/하 Y 축 떨어진 간격 즉, Y 축 간의 오차 (높이) 값
 parm : mark_y1	- [in]  Y1 축 Mark Number (Zero based)
		mark_y2	- [in]  Y2 축 Mark Number (Zero based)
 retn : 두 지점의 Y 축 간의 높이 차이 (unit: mm)
*/

API_IMPORT VOID uvLuria_SetAlignMotionPtr(AlignMotion& motion);



API_IMPORT DOUBLE uvLuria_GetLocalMarkDiffY(UINT8 mark_y1, UINT8 mark_y2);
/*
 desc : Mark 1번과 3번, Mark 2번과 4번의 X 좌표 오차 값을 반환 (수직 오차 값)
 parm : scan	- [in]  Align Scan하려는 위치 (번호. 0 or 1)
		cam1	- [in]  Left/Bottom에 위치한 Mark X 위치 기준으로 오차 값 반환 (Left/Top 순으로 저장) (unit: mm)
		cam2	- [in]  Right/Bottom에 위치한 Mark X 위치 기준으로 오차 값 반환 (Right/Top 순으로 저장) (unit: mm)
 retn : 저장된 데이터 개수 반환 (실패인 경우, 0)
*/
API_IMPORT UINT32 uvLuria_GetLocalMarkDiffVertX(UINT8 scan,
												CAtlList <DOUBLE> &cam1, CAtlList <DOUBLE> &cam2);
/*
 desc : 기존에 선택된 JobXML 초기화
 parm : None
 retn : None
*/
API_IMPORT VOID uvLuria_ResetJobXML();

/* --------------------------------------------------------------------------------------------- */
/*                               Machine Config Get & Set - Single                               */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Total number of photoheads connected to Luria
 parm : ph_count	- [in]  Total number of photoheads in system (1 ~ 8)
 retn : TRUE or FALSE
 note : uid : 0x01
*/
API_IMPORT BOOL uvLuria_ReqGetTotalPhotoheads();
API_IMPORT BOOL uvLuria_ReqSetTotalPhotoheads(UINT8 count);
/*
 desc : Request Get/Set - The IP address of photohead
 parm : ph_no	- [in]  Photohead Number (1 ~ 8)
		ipv4	- [in]  IP address (ex> 192.168.0.1 => 0:192,1:168,2:0,3:1)
 retn : TRUE or FALSE
 note : uid - 0x02
*/
API_IMPORT BOOL uvLuria_ReqGetPhotoheadIpAddr(UINT8 ph_no);
API_IMPORT BOOL uvLuria_ReqSetPhotoheadIpAddr(UINT8 ph_no, PUINT8 ipv4);
/*
 desc : Request Get/Set - The Pitch of photohead
 parm : pitch	- [in]  Pitch between photoheads, given in number of exposure strips
 retn : TRUE or FALSE
 note : uid - 0x03
*/
API_IMPORT BOOL uvLuria_ReqGetPhotoheadPitch();
API_IMPORT BOOL uvLuria_ReqSetPhotoheadPitch(UINT8 pitch);
/*
 desc : The alignment of the photoheads
 parm : rotate	- [in]  Orientation of photoheads. 0x00 = No rotation. 0x01 = Rotated 180 degrees.
 retn : TRUE or FALSE
 note : uid - 0x05
*/
API_IMPORT BOOL uvLuria_ReqGetPhotoheadRotate();
API_IMPORT BOOL uvLuria_ReqSetPhotoheadRotate(UINT8 pitch);
/*
 desc : The paralleogram motion angle of the motion controller will be adjusted by this factor
 parm : tbl_no	- [in]  Table number (1 or 2)
		factor	- [in]  Adjustment factor, multiplied by 1000.
						(Eg. factor 0.97 is given as 970, factor 1.025 is given as 1025)
						Factor 1.0 (1000) means no adjustment, i.e. use the default angle for parallelogram motion.
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetParallelogramMotionAdjust(UINT8 tbl_no, UINT32 factor);
API_IMPORT BOOL uvLuria_ReqSetParallelogramMotionAdjust(UINT8 tbl_no, UINT32 factor);
/*
 desc : The maximum scroll rate that a photo head can tolerate
 parm : scroll	- [in]  The maximum scroll rate of the system. (Default 19000)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetScrollRate();
API_IMPORT BOOL uvLuria_ReqSetScrollRate(UINT16 scroll);
/*
 desc : The xy-motion controller connected to the system
 parm : type	- [in]  The motion controller type used. 1 = Sieb & Meyer, 2 = Newport
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetMotionControlType();
API_IMPORT BOOL uvLuria_ReqSetMotionControlType(UINT8 type);
/*
 desc : IP address of the motion controller
 parm : ipv4	- [in]  ip address xxx.xxx.xxx.xxx
 retn : NULL or Packet Data
*/
API_IMPORT BOOL uvLuria_ReqGetMotionControlIpAddr();
API_IMPORT BOOL uvLuria_ReqSetMotionControlIpAddr(PUINT8 ipv4);
/*
 desc : X-correction table for the x-motion.
		The first entry must have 0 adjustment.
		The correction given for the last table entry will be used for x-positions
		at higher x-positions than the x-position for the last table entry.
 parm : tbl_no	- [in]  Table number: 1 or 2
		e_count	- [in]  Number of table entries (N). 0 < N <= 200
		e_x_pos	- [in]  X-position on table (in nanometers)
		e_x_adj	- [in]  Adjustment (in nanometers) in the given x-position
 retn : NULL or Packet Data
*/
API_IMPORT BOOL uvLuria_ReqGetXcorrectionTable(UINT8 tbl_no);
API_IMPORT BOOL uvLuria_ReqSetXcorrectionTable(UINT8 tbl_no,
											   UINT16 e_count, PINT32 e_x_pos, PINT32 e_x_adj);
/*
 desc : The minimum and maximum coordinate of the motion controller.
		This tells the Luria maximum allowable motion position
 parm : tbl_no	- [in]  Table number: 1 or 2
		max_x	- [in]  Maximum absolute x-position of table. In micrometers
		max_y	- [in]  Maximum absolute y-position of table. In micrometers
		min_x	- [in]  Minimum absolute x-position of table. In micrometers
		min_y	- [in]  Minimum absolute y-position of table. In micrometers
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetTablePositionLimits(UINT8 tbl_no);
API_IMPORT BOOL uvLuria_ReqSetTablePositionLimits(UINT8 tbl_no,
												  INT32 max_x, INT32 max_y,
												  INT32 min_x, INT32 min_y);
/*
 desc : The start position for the artwork for the given table.
		This is the position where the actual print will start.
		For the motion start position, see GetTableMotionStartPosition
 parm : tbl_no	- [in]  Table number: 1 or 2
		start_x	- [in]  X-position table exposure start position. In micrometers
		start_y	- [in]  Y-position table exposure start position. In micrometers
		reply	- [in]  Whether to request a set exposure start position
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetTableExposureStartPos(UINT8 tbl_no);
API_IMPORT BOOL uvLuria_ReqSetTableExposureStartPos(UINT8 tbl_no,
												   INT32 start_x, INT32 start_y, UINT8 reply=0x01);
/*
 desc : This value limits the maximum speed that the motion controller will run in y-direction
 parm : speed	- [in]  The max speed of Y motion. Given in micrometers/sec
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetMaxYMotionSpeed();
API_IMPORT BOOL uvLuria_ReqSetMaxYMotionSpeed(UINT32 speed);
/*
 desc : This is the speed to be used for x-movements.
		This speed will also be used to move to start position during pre-print
 parm : speed	- [in]  The motion speed to be used for x-movements (and to move to print start position)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetXMotionSpeed();
API_IMPORT BOOL uvLuria_ReqSetXMotionSpeed(UINT32 speed);
/*
 desc : Y-axis acceleration/deceleration distance.
		Note! Valid if MotionControlType = 1 only. Ignored for other types
 parm : dist	- [in]  Motion controller Y-direction acceleration/deceleration distance. Given in micrometers
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetYaccelerationDistance();
API_IMPORT BOOL uvLuria_ReqSetYaccelerationDistance(UINT32 dist);
/*
 desc : Y-axis acceleration/deceleration distance.
		Note! Valid if MotionControlType = 1 only. Ignored for other types
 parm : scroll	- [in]  Scroll-mode (1 – 4)
						The hysteresis values below will be used with the corresponding scroll-mode when printing.
		offset	- [in]  Negative offset. Given in number of trigger pulses
		d_pos	- [in]  Delay in positive moving direction. In nanoseconds
		d_neg	- [in]  Delay in negative moving direction. In nanoseconds
 retn : TRUE or FALSE
 note : Motion controller hysteresis. Note! Valid if MotionControlType = 1 only.
		Ignored for other types
*/
API_IMPORT BOOL uvLuria_ReqGetHysteresisType1(UINT8 scroll);
API_IMPORT BOOL uvLuria_ReqSetHysteresisType1(UINT8 scroll, UINT16 offset,
											  UINT32 d_pos, UINT32 d_neg);
/*
 desc : Measured offset in x- and y directions compared to photo head number 1
 parm : ph_no	- [in]  Photohead number (2 - 8)
		x_pos	- [in]  X-position relative to Photohead 1. Given in nanometers
		y_pos	- [in]  Y-position relative to Photohead 1. Given in nanometers
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetPhotoheadOffset(UINT8 ph_no);
API_IMPORT BOOL uvLuria_ReqSetPhotoheadOffset(UINT8 ph_no, UINT32 x_pos, INT32 y_pos);
API_IMPORT BOOL uvLuria_ReqGetPhotoHeadOffsetAll();
/*
 desc : Start-direction of y axis when printing
 parm : tbl_no	- [in]  Table number: 1 or 2
		direct	- [in]  0 = Y axis starts in negative direction, 1 = Y axis starts in positive direction
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetTablePrintDirect(UINT8 tbl_no);
API_IMPORT BOOL uvLuria_ReqSetTablePrintDirect(UINT8 tbl_no, UINT8 direct);
/*
 desc : Select which table that is the active table
 parm : tbl_no	- [in]  Table number: 1 or 2
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetActiveTable();
API_IMPORT BOOL uvLuria_ReqSetActiveTable(UINT8 tbl_no);
/*
 desc : Select which table that is the active table
 parm : tbl_no	- [in]  Table number: 1 or 2
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetTableMotionStartPosition(UINT8 tbl_no);
/*
 desc : Enables use of the Luria software without a motion controller
		Enables use of the Luria software without photo head(s)
		Enables use of the Luria software without trigger input signals to the photo head(s)
 parm : mc2		- [in]  0 = System requires motor controller, 1 = Emulate the motor controller (no need for motor controller hardware)
		ph		- [in]  0 =System requires photo head(s) to be connected, 1 = Emulate the photo head(s) (no need for photo heads to be connected)
		trig	- [in]  = System requires trigger signal inputs, 1 = Emulate the trigger inputs signals (no need for trigger input signals to photo head(s))
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetEmulate();
API_IMPORT BOOL uvLuria_ReqSetEmulate(UINT8 mc2, UINT8 ph, UINT8 trig);
/*
 desc : Sets the level of debug output in the log file
 parm : level	- [in]  0 = Print info only, 1 = Print info and debug, 2 = Print info, debug and trace messages
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetDebugPrintLevel();
API_IMPORT BOOL uvLuria_ReqSetDebugPrintLevel(UINT8 level);
/*
 desc : The files generated during simulated print will be written to this directory
 parm : out_dir	- [in]  Output path name text string (without null-termination) of simulation result files
						The path must be absolute. A trailing backslash (\) is optional
 retn : Pointer where the packet buffer is stored
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetPrintSimulationOutDir();
API_IMPORT BOOL uvLuria_ReqSetPrintSimulationOutDir(CHAR *out_dir, UINT32 size);
/*
 desc : Y-correction table, one for each table
		Each printed strip can be adjusted individually, in positive or negative direction Max y-adjustment is +/-500 um (500000 nm)
 parm : tbl_no	- [in]  Table number: 1 or 2
		count	- [in]  Number of strip (N). 0 < N <= 200
		strip_no- [in]  Strip number (entry 1 ~ ). (Note: First strip number is 1)
		y_adj	- [in]  Y-adjustment for strip number given in entry N. Given in nanometers
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetYcorrectionTable(UINT8 tbl_no);
API_IMPORT BOOL uvLuria_ReqSetYcorrectionTable(UINT8 tbl_no, UINT16 count, PUINT8 strip_no, PINT32 y_adj);
/*
 desc : The ID to be set for the z-axis in the motion controller for this specific photo head
 parm : ph_no	- [in]  Photo head number
		drv_no	- [in]  ID number of the z-motion controller for the given photo head
 retn : TRUE or FALSE
 note : Valid if ZdriveType = 2 only
*/
API_IMPORT BOOL uvLuria_ReqGetLinearZdriveSetting(UINT8 ph_no);
API_IMPORT BOOL uvLuria_ReqSetLinearZdriveSetting(UINT8 ph_no, UINT8 drv_no);
/*
 desc : IP address of the Z drive (Focus drive);
 parm : addr	- [in]  IP address (ex> 192.168.0.50 -> 0:192,1:168, 2:0, 3:50)
 retn : TRUE or FALSE
 note : Valid if ZdriveType = 2 only
*/
API_IMPORT BOOL uvLuria_ReqGetZdriveIpAddr();
API_IMPORT BOOL uvLuria_ReqSetZdriveIpAddr(PUINT8 addr);
/*
 desc : The ID to be set for the x and y-axis in the motion controller for up to two tables
 parm : tbl_no	- [in]  Work Table Number 1 or 2
		xdrvid	- [in]  ID number of the x-motion controller for the given table
		ydrvid	- [in]  ID number of the y-motion controller for the given table
 retn : TRUE or FALSE
 note : Valid if MotionControlType = 1 and 3 only. Ignored for other types
*/
API_IMPORT BOOL uvLuria_ReqGetXYDriveId(UINT8 tbl_no);
API_IMPORT BOOL uvLuria_ReqSetXYDriveId(UINT8 tbl_no, UINT8 xdrvid, UINT8 ydrvid);
/*
 desc : ID identifying the LLS-system
 parm : flag- [in]  Get (Read) or Set (Write)
		pid	- [in]  Product ID of the system
 retn : TRUE or FALSE
 note : 16702	= LLS2500
		25002	= LLS04
		50002	= LLS06
		50001	= LLS10
		117502	= LLS15
		100001	= LLS25
		235002	= LLS30
		200001	= LLS50
*/
API_IMPORT BOOL uvLuria_ReqGetProductId();
API_IMPORT BOOL uvLuria_ReqSetProductId(UINT32 pid);
/*
 desc : ArtworkComplexity reflects how fine-pitched the artwork is
		A complex artwork will result in larger preprocessed files
		Selecting a higher. ArtworkComplexity will allow larger,
		more complex preprocessed files to be accepted by Luria,
		by reserving more space inside the photo head(s) storage.
 parm : level	- [in]  0 = Normal, 1 = high, 2 = extreme
 retn : TRUE or FALSE
 note : Higher complexity setting will result in fewer jobs to be held simultaneously in Luria
		Therefore, ArtworkComplexity should be held as low as possible
		It should only be increased if loading a preprocessed job results in StatusCode “StripFileSizeTooLarge”
		No longer in use from Luria version 3.0.0 and newer
*/
API_IMPORT BOOL uvLuria_ReqGetArtworkComplexity();
API_IMPORT BOOL uvLuria_ReqSetArtworkComplexity(UINT8 level);
/*
 desc : Focus Z drive type (This setting is equal for all photo heads)
 parm : type	- [in]  Drive Type: 1 = Stepper motor (built-in), 2 = Linear drive (external)
						1 = Stepper motor (use built in driver in photo heads)
						2 = Linear drive from ACS
						3 = Linear drive from Sieb&Meyer (valid from version 2.10.0 and newer)
 retn : TRUE or FALSE
 note : Valid from Luria version 2.1.0 and newer
*/
API_IMPORT BOOL uvLuria_ReqGetZdriveType();
API_IMPORT BOOL uvLuria_ReqSetZdriveType(UINT8 type);
/*
 desc : When enabled, this mode will configure the fans so that an over-pressure is created inside the photo head
 parm : mode	- [in]  1 = Over pressure mode ON, 0 = Over pressure mode OFF
 retn : TRUE or FALSE
 note : This requires that a fan is mounted in the front of the photo head!
		Valid from Luria version 2.2.0 and newer
*/
API_IMPORT BOOL uvLuria_ReqGetOverPressureMode();
API_IMPORT BOOL uvLuria_ReqSetOverPressureMode(UINT8 mode);
/*
 desc : Valid for linear Z-motor only, ignored for stepper motor
		The given DOF will be used by the photo heads to determine if the current focus position during
		autofocus is within the DOF area or not
		To determine if outside DOF, the Focus:OutsideDOFStatus can be read
 parm : step	- [in]  Depth of focus (DOF). Given in number of steps
 retn : TRUE or FALSE
 note : Valid from Luria version 2.6.0 and newer
*/
API_IMPORT BOOL uvLuria_ReqGetDepthOfFocus();
API_IMPORT BOOL uvLuria_ReqSetDepthOfFocus(UINT16 step);
/*
 desc : When this is enabled the length of a jobs artwork stripes is approximately twice the length of normal stripes
		It is required that the job is preprocessed with the extra long stripe option
		If there is no need for extra long stripe support, this option should be left disabled due to extra processing time during printing.
 parm : type	- [in]  0 = Normal length stripes, 1 = Support for extra long stripes (valid for LLS2500 only)
 retn : TRUE or FALSE
 note : Obsolete from Luria version 3.0.0 and newer (Support for long stripes default, without extra processing time)
*/
API_IMPORT BOOL uvLuria_ReqGetExtraLongStripes();
API_IMPORT BOOL uvLuria_ReqSetExtraLongStripes(UINT8 type);
/*
 desc : Enabling MTC will result in smoother edges in cases where the motion of the y-axis is not perfect
		It is required that the job is preprocessed with the MTC mode on
 parm : mode	- [in]  0 = Normal exposure mode, 1 = MTC (Machine Tolerance Compensation) mode
 retn : TRUE or FALSE
 note : Note that enabling MTC will reduce the maximum scroll rate that can be used
		Valid from Luria version 3.0.0 and newer
*/
API_IMPORT BOOL uvLuria_ReqGetMTCMode();
API_IMPORT BOOL uvLuria_ReqSetMTCMode(UINT8 mode);
/*
 desc : When not using ethercat for AF Luria will not try to communicate over ethercat with the z-motion controller
 parm : enable	- [in]  0 = Disable use of ethercat for AF, 1 = Use ethercat for AF
 retn : TRUE or FALSE
 note : Valid from Luria version 2.12.0 and newer
*/
API_IMPORT BOOL uvLuria_ReqGetUseEthercatForAF();
API_IMPORT BOOL uvLuria_ReqSetUseEthercatForAF(UINT8 enable);


API_IMPORT BOOL uvLuria_ReqScanEthercat();
/*
 desc : The spx-level from preprocessed files must correspond to the value set here
 parm : level	- [in]  Spx-level to be used. Default is 36
 retn : TRUE or FALSE
 note : Valid from Luria version 3.0.0 and newer
*/
API_IMPORT BOOL uvLuria_ReqGetSpxLevel();
API_IMPORT BOOL uvLuria_ReqSetSpxLevel(UINT16 level);
/*
 desc : The spx-level from preprocessed files must correspond to the value set here
 parm : mask	- [in]  Time that OCP errors should be ignored, due to false OCP values
						In microseconds. Valid values: 6 – 50
 retn : Pointer where the packet buffer is stored
 note : Valid from Luria version 2.12.2 and 3.1.0 and newer
*/
API_IMPORT BOOL uvLuria_ReqGetOcpErrorMask();
API_IMPORT BOOL uvLuria_ReqSetOcpErrorMask(UINT16 mask);
/*
 desc : Motion controller hysteresis. Note! Valid if MotionControlType = 1 only
		Ignored for other types. This hysteresis is position based, and will not be affected by the print speed.
 parm : scroll		- [in]  Scroll-mode (1 – max)
							The hysteresis values below will be used with the corresponding scroll-mode when printing.
							(Max scroll mode may vary for different products)
		p_delay		- [in]  Position-based delay in positive moving direction. In picometers
		n_delay		- [in]  Position-based delay in negative moving direction. In picometers
 retn : Pointer where the packet buffer is stored
 note : This hysteresis is added to the hysteresis done by HysteresisType1
		Valid from Luria version 2.13.0 and 3.2.0 and newer
*/
API_IMPORT BOOL uvLuria_ReqGetPositionHysteresisType1(UINT8 scroll);
API_IMPORT BOOL uvLuria_ReqSetPositionHysteresisType1(UINT8 scroll,
													  UINT32 p_delay, UINT32 n_delay);
/*
 desc : This makes it possible to use another bitmap than the built-in default for the edge blending on the left and right side of the DMD
		Note that this command must be re-sent if the bitmap file on the Luria PC is changed, even if the filename is unchanged
 parm : flag	- [in]  Get (Read) or Set (Write)
		file	- [in]	Path and filename (without null termination) of custom edge blend bitmap
						The given bitmap file must exist on the Luria-PC disc and must have the format of 8x1600 pixels for 9k5-based products and 8x1080 for 4k8 products
						The left half of the bitmap is the left bitmap (4 pixels wide) and the right half is the right bitmap (4 pixels wide)
						The bitmap must be in 1-bit monochrome format. Limit: L < 200.
						If the built-in default built-in bitmap is to be used, an empty string with L=0 must be used
		size	- [in]  The size of 'file' buffer
 retn : Pointer where the packet buffer is stored
 note : Valid from 3.2.0 and newer
*/
API_IMPORT BOOL uvLuria_ReqGetCustomEdgeBlendBitmap();
API_IMPORT BOOL uvLuria_ReqSetCustomEdgeBlendBitmap(PCHAR file, UINT32 size);

/* --------------------------------------------------------------------------------------------- */
/*                             Machine Config Get & Set - Extended                               */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Request Get/Set - The properties of the motion controll
 parm : mc2			- [in]  0x01 : Sieb&Meyer, 0x02 : Newport (0x00만 유효)
		ipv4		- [in]  The IPv4 address of MC2
		max_y_speed	- [in]  The maximu speed in the Y-axis direction (unit: um)
		x_speed		- [in]  The maximu speed in the X-axis direction (unit: um) (and to move to print start position)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetMotionControl();
API_IMPORT BOOL uvLuria_ReqSetMotionControl(UINT8 mc2, PUINT8 ipv4,
											UINT32 max_y_speed, UINT32 x_speed);
/*
 desc : Request Get/Set - The X/Y stage moving speed of MC2
 parm : max_y_speed	- [in]  The maximu speed in the Y-axis direction (unit: um)
		x_speed		- [in]  The maximu speed in the X-axis direction (unit: um) (and to move to print start position)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetMotionSpeedXY();
API_IMPORT BOOL uvLuria_ReqSetMotionSpeedXY(UINT32 max_y_speed, UINT32 x_speed);
/*
 desc : Get the type1 information of motion controller
 parm : scroll	- [in]  Scroll Mode (1 ~ 4).
						The hysteresis values below will be used with the corresponding scroll-mode when printing.
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetMotionType1();
API_IMPORT BOOL uvLuria_ReqSetMotionType1(UINT32 acc_dist, UINT8 active_table);
/*
 desc : Get the Table Settings Information
 parm : tbl_num	- [in]  table number (1 ~ 2)
		parallel- [in]  Adjustment factor, multiplied by 1000. (Eg. factor 0.97 is given as 970, factor 1.025 is given as 1025).
						Factor 1.0 (1000) means no adjustment, i.e. use the default angle for parallelogram motion.
		y_dir	- [in]  0 = Y axis starts in negative direction, 1 = Y axis starts in positive direction
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetTableSettings(UINT8 tbl_num);
API_IMPORT BOOL uvLuria_ReqSetTableSettings(UINT8 tbl_num, UINT32 parallel, UINT8 y_dir);
/*
 desc : Get the System settings Information
 parm : mc2		- [in]  Enables use of the Luria software without a motion controller.
						0 = System requires motor controller, 1 = Emulate the motor controller (no need for motor controller hardware)
		ph		- [in]  Enables use of the Luria software without photo head(s).
						0 =System requires photo head(s) to be connected, 1 = Emulate the photo head(s) (no need for photo heads to be connected)
		trig	- [in]  Enables use of the Luria software without trigger input signals to the photo head(s).
						0 = System requires trigger signal inputs, 1 = Emulate the trigger inputs signals (no need for trigger input signals to photo head(s))
		debug	- [in]  Prints debug information to logfile.
						0 =Do not print extra debug info to logfile, 1 = Print extra debug info to logfile
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetSystemSettings();
API_IMPORT BOOL uvLuria_ReqSetSystemSettings(UINT8 mc2, UINT8 ph, UINT8 trig, UINT8 debug);
/*
 desc : Request Get/Set - The basic information of photoheads
 parm : count	- [in]  The number of photoheads
		pitch	- [in]  The pitch of photohead
		rate	- [in]  The scroll rate of photohead
		rotate	- [in]  Whether the photohead is rotated or not
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetAllPhotoheads();
API_IMPORT BOOL uvLuria_ReqSetAllPhotoheads(UINT8 count, UINT8 pitch, UINT16 rate, UINT8 rotate);

/* --------------------------------------------------------------------------------------------- */
/*                               System Status Get & Set - Single                                */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : This command will try to connect to all external devices (photo heads, motion controller, etc)
		and initialize these. This command MUST be called before being able to print
		The job list must be empty when calling this command. The command will return ok if sync with
		external devices was successful
		If init failed, then status code InitHardwareFailed will be returned
 parm : None
 retn : TRUE or FALSE
 note : In case of HW init failure, then it may take up to 4 seconds until a reply is given
*/
API_IMPORT BOOL uvLuria_ReqSetInitializeHardware();
/*
 desc : Query the status of the complete system
		If all 0's, then there are no errors
		If any part report a failure (i.e. one ore more bits set),
		this can only be cleared by running a successful InitializeHardware command again
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetSystemStatus();
/*
 desc : This command will safely shut down all components of the system and leave the photo heads mirrors in a parked state
		(the LEDs will flash red and yellow alternately to indicate shut-down mode)
		This command should be run before turning off power in the photo heads
 parm : None
 retn : TRUE or FALSE
 note : Note that the photo heads MUST be powered off and on again after running SystemShutdown command
*/
API_IMPORT BOOL uvLuria_ReqSetSystemShutdown();
/*
 desc : This command will upgrade all photo heads with the firmware in the given bundle path or firmware file
		The path name can be an absolute path to a LLS software bundle or a path to a tar.gz file to be used for upgrading
		When the upgrade starts, a new InitializeHardware will be required
		If trying to do InitializeHardware before upgrade process is complete,
		BusyUpgradingFirmware status code is given
		The upgrade process takes about 7 minutes
		When upgrade is completed and BusyUpgradingFirmware is no longer given in the reply, all photo heads must be manually re-powered
 parm : length	- [in]  Length of path name text string (S). Max = 200
		file	- [in]  Path name text string (without null-termination)of bundle or full path and file name of tar.gz file
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetUpgradeFirmware(UINT16 length, PCHAR file);
/*
 desc : This will load the selected test image that is built-in the photo head and display it on the DMD
 parm : ph_no	- [in]  0x00: All Photoheads, 1: PH1, 2: PH2, ...
		img_no	- [in]  Internal test image number (1 ~ 5)
 retn : TRUE or FALSE
 note : Note that it is required to turn on light using SetLightIntensity in order to actually display the image
		Running this command requires InitializeHardware to be run in order to do an exposure again
		Valid from Luria version 2.3.0 and newer.
*/
API_IMPORT BOOL uvLuria_ReqSetLoadInternalTestImage(UINT8 ph_no, UINT8 img_no);
/*
 desc : Use this command to turn on the light in the photo head(s) in order to display the internal test image selected using command
		LoadInternalTestImage. The lights will be shut off again when running InitializeHardware
 parm : ph_no	- [in]  0: All Photoheads, 1: PH1, 2: PH2, ...
		led_no	- [in]  LED Number (0x00: All Leds, 1:LED1, 2:LED2, 3:LED3, ...)
		ampl	- [in]  Amplitude of selected light source(s)
 retn : TRUE or FALSE
 note : Valid from Luria version 2.3.0 and newer
*/
API_IMPORT BOOL uvLuria_ReqSetSetLightIntensity(UINT8 ph_no, ENG_LLPI led_no, UINT16 ampl);
/*
 desc : This will give the temperature of the mainboard for each photo head
 parm : None
 retn : TRUE or FALSE
 note : Note that the number must be divided by 10 to get the correct temperature
		If the temperature raises above approx 55 deg. Celsius,
		then this may indicate that the cooling is not working properly
		In such cases the photo head should be switched off and troubleshooting to find
		the cause of the temperature raise should be performed
		If photo head not connected, then 0 will be returned
		Valid from Luria version 2.11.0 and newer
*/
API_IMPORT BOOL uvLuria_ReqGetMainboardTemperature();
/*
 desc : Get the error status for all photo heads. See table below for bit definitions of the error status word
 parm : None
 retn : TRUE or FALSE
 note : Valid from Luria version 2.12.0 and newer
*/
API_IMPORT BOOL uvLuria_ReqGetOverallErrorStatusMulti();
/* --------------------------------------------------------------------------------------------- */
/*                               Job Management Get & Set - Single                               */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Root directory where pre-processed jobs are placed
 parm : flag	- [in]  Get (Read) or Set (Write)
		path	- [in]	String (without null-termination) that contains the root directory
						where all pre-processed jobs can be found. A trailing backslash (\) is optional
		size	- [in]  The size of 'path' buffer
 retn : TRUE or FALSE
 note : There must be no assigned jobs when changing RootDirectory
		If trying to change RootDirectory while jobs are assigned, an error will be returned
		The RootDirectory given must exist unless giving an empty string (L = 0), which will clear the root directory
*/
API_IMPORT BOOL uvLuria_ReqGetRootDirectory();
API_IMPORT BOOL uvLuria_ReqSetRootDirectory(PCHAR file, UINT32 size);
/*
 desc : If no jobs are assigned, then nothing is returned, L = 0
		The paths may be a mix of relative and absolute paths, depending on how the jobs were assigned
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetJobList();
/*
 desc : Assign (add) a new job. The path name given is the path to valid pre-processed job
		An error message will be given if the path does not contain valid files
		The first assigned job will be set as selected job automatically
 parm : path	- [in]  Path name text string (without null-termination) of a preprocessed job
						Could be relative path (relative to RootDirectory) or absolute path
						A trailing backslash (\) is optional
		size	- [in]  The size of 'path' buffer
 retn : TRUE or FALSE
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetAssignJob(PCHAR path, UINT32 size);
/*
 desc : Delete the selected job from the list of assigned jobs
		If no jobs assigned, an error will be returned
		(This command will not delete anything from the disk, only from the list of assigned jobs)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetDeleteSelectedJob();
/*
 desc : If a read is attempted and no assigned jobs, then “No job” is returned
 parm : flag	- [in]  Get (Read) or Set (Write)
		path	- [in]	Path name text string (without null-termination) of selected job
						The path may be relative or absolute. A trailing backslash (\) is optional
		size	- [in]  The size of 'path' buffer
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetSelectedJob();
API_IMPORT BOOL uvLuria_ReqSetSelectedJob(PCHAR path, UINT32 size);
API_EXPORT BOOL uvLuria_ReqSetSelectedJobIndex(UINT8 index);
API_IMPORT BOOL uvLuria_ReqSetSelectedJobExt(PCHAR path, UINT8 mode);
/*
 desc : Load the currently selected job to the photo head(s)
		A reply is returned immediately and the load progress can be queried using “GetSelectedJobLoadState”
		Alternatively, announcements from the Luria can be used. Luria will send “LoadState”-announcements
		after each strip being loaded to a photo head and also when all files are loaded completely
		(or if any failure happened).
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetLoadSelectedJob();
/*
 desc : Returns the current load-state of the system
 parm : None
 retn : Pointer where the packet buffer is stored
*/
API_IMPORT BOOL uvLuria_ReqGetSelectedJobLoadState();
/*
 desc : Returns the global and local fiducial points of the given job
 parm : length	- [in]  Length of path name text string (S). Max = 200
		path	- [in]  Path name text string (without null-termination) of job
						The path may be relative or absolute. A trailing backslash (\) is optional
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetGetFiducials(UINT16 length, PCHAR path);
/*
 desc : Returns a set of parameters for the given job
 parm : length	- [in]  Length of path name text string (S). Max = 200
		path	- [in]  Path name text string (without null-termination) of job
						The path may be relative or absolute. A trailing backslash (\) is optional
 retn : Pointer where the packet buffer is stored
 note : Note that the number of parameters is given by the “Command version” parameter
*/
API_IMPORT BOOL uvLuria_ReqGetGetJobParams(UINT16 length=0, PCHAR path=NULL);
/*
 desc : Returns all defined dynamic panel data dcodes for the given job
 parm : length	- [in]  Length of path name text string (S). Max = 200
		path	- [in]  Path name text string (without null-termination) of job
						The path may be relative or absolute. A trailing backslash (\) is optional
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetGetPanelDataDecodeList(UINT16 length, PCHAR path);
/*
 desc : Returns all the panel data information for the given d-code and job
 parm : dcode	- [in]  D-code
		length	- [in]  Length of path name text string (S). Max = 200
		path	- [in]  Path name text string (without null-termination) of job
						The path may be relative or absolute. A trailing backslash (\) is optional
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetGetPanelDataInfo(UINT32 dcode, UINT16 length, PCHAR path);
/*
 desc : The number of jobs that can be assigned simultaneously will vary according to photo head hardware and MachineConfig: ArtworkComplexity setting.
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetGetMaxJobs();

/* --------------------------------------------------------------------------------------------- */
/*                              Job Management Get & Set - Extended                              */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 임의 Job Name이 저장된 위치 (Index)의 Job 삭제 (제거. 파일 삭제가 아님)
 parm : index	- [in]  삭제 대상의 Job Name이 저장된 위치
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetDeleteJobIndex(UINT8 index);
/*
 desc : 현재 Luria Service에 등록된 모든 거버 (Job) 삭제 (제거. 파일 삭제가 아님)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetDeleteJobAll();
/*
 desc : 현재 선택된 Job Name 삭제 (제거. 파일 삭제가 아님)
 parm : None
 retn : TRUE or FALSE
 note : 연속 동작일 경우, 사용하지 말것
*/
API_IMPORT BOOL uvLuria_ReqSetDeleteSelectedJobExt();
/*
 desc : Register a job and request a registered job.
 parm : job		- [in]  Job Name
		size	- [in]  'The size of job'
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqAddJobList(PCHAR job, UINT32 size);

/* --------------------------------------------------------------------------------------------- */
/*                             Panel Preparation Get & Set - Single                              */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Dynamic fiducial coordinates. When sending dynamic fiducials these will be used instead of global
		and local fiducials from the gerber-file. Up to 8 (N <= 8) global dynamic fiducials,
		while up to 200 (N <= 200) local dynamic fiducials are supported
 parm : type	- [in]  0x00: Global, 0x01 : Local
		count	- [in]  Fiducial Count
		fidxy	- [in]  Array pointer containing X/Y coordinates [xx][2] (unit: nm)
 retn : Pointer where the packet buffer is stored
 note : To enable use of global and local fiducials from gerber-file again,
		then a special DynamicFiducials command where type = 0 (global) and N = 0 must be sent
*/
API_IMPORT BOOL uvLuria_ReqGetDynamicFiducials(UINT8 type);
API_IMPORT BOOL uvLuria_ReqGetDynamicFiducialsAll();
API_IMPORT BOOL uvLuria_ReqSetDynamicFiducials(UINT8 type, UINT16 count, INT32 **fidxy);
/*
 desc : Registration points. Up to 400 registration points are supported, ie. N <= 400
 parm : count	- [in]  Global & Local Fiducial Count
		fidxy	- [in]  Structure pointer containing X/Y coordinates (unit: nm)
 retn : Pointer where the packet buffer is stored
*/
API_IMPORT BOOL uvLuria_ReqGetRegistrationPoints();
API_IMPORT BOOL uvLuria_ReqSetRegistrationPoints(UINT16 count, LPG_I32XY fidxy);
/*
 desc : Transformation recipe that is valid for global registration
		If set to Fixed, the values in GlobalFixedRotation, GlobalFixedScaling and GlobalFixedOffset are used
 parm : rotation- [in]  Rotating	0x00 - Auto, 0x01 = Fixed
		scaling	- [in]  Scaling		0x00 - Auto, 0x01 = Fixed
		offset	- [in]  Offset		0x00 - Auto, 0x01 = Fixed
 retn : Pointer where the packet buffer is stored
*/
API_IMPORT BOOL uvLuria_ReqGetGlobalTransformationRecipe();
API_IMPORT BOOL uvLuria_ReqSetGlobalTransformationRecipe(UINT8 rotation, UINT8 scaling, UINT8 offset);
/*
 desc : Has effect in fixed transformation mode only. Fixed rotation to be used. N <= 200
 parm : rotation- [in]  Global fixed rotation, in micro-degrees. Ignored for TransformationReceipe 0.
 retn : Pointer where the packet buffer is stored
*/
API_IMPORT BOOL uvLuria_ReqGetGlobalFixedRotation();
API_IMPORT BOOL uvLuria_ReqSetGlobalFixedRotation(INT32 rotation);
/*
 desc : Has effect in fixed transformation mode only
		Fixed scaling to be used. N <= 200
		(Eg. Factor 1.0002 is given as 1000200, factor 0.9998 is given as 999800)
 parm : scale_x	- [in]  Global fixed x-scaling factor * 10^6. Ignored for TransformationReceipe 0.
		scale_y	- [in]  Global fixed y-scaling factor * 10^6. Ignored for TransformationReceipe 0.
 retn : Pointer where the packet buffer is stored
*/
API_IMPORT BOOL uvLuria_ReqGetGlobalFixedScaling();
API_IMPORT BOOL uvLuria_ReqSetGlobalFixedScaling(UINT32 scale_x, UINT32 scale_y);
/*
 desc : Has effect in fixed transformation mode only. Fixed offset to be used. N <= 200
 parm : offset_x- [in]  Global fixed x-offset. factor (unit: nm). Ignored for TransformationReceipe 0.
		offset_x- [in]  Global fixed y-offset. factor (unit: nm). Ignored for TransformationReceipe 0.
 retn : Pointer where the packet buffer is stored
*/
API_IMPORT BOOL uvLuria_ReqGetGlobalFixedOffset();
API_IMPORT BOOL uvLuria_ReqSetGlobalFixedOffset(INT32 offset_x, INT32 offset_y);
/*
 desc : This will determine the zone geometry to be used for local zones
 parm : 
		fid_x	- [in]  Number of fiducial points in x-direction for one zone
		fid_y	- [in]  Number of fiducial points in y-direction for one zone
 retn : Pointer where the packet buffer is stored
*/
API_IMPORT BOOL uvLuria_ReqGetLocalZoneGeometry();
API_IMPORT BOOL uvLuria_ReqSetLocalZoneGeometry(UINT16 fid_x, UINT16 fid_y);
/*
 desc : The number of calculated local zones
 parm : None
 retn : Pointer where the packet buffer is stored
*/
API_IMPORT BOOL uvLuria_ReqGetGetNumberOfLocalZones();
/*
 desc : Retrieve the transformation parameters based on the current registration points
		and transformation recipe
 parm : zone	- [in]  Zone number ( 0 = Global, 1 and above: local zone numbers)
 retn : Pointer where the packet buffer is stored
*/
API_IMPORT BOOL uvLuria_ReqGetGetTransformationParams(UINT16 zone);
/*
 desc : Will run registration based on the current fiducials and registration points
		If registration error is received, then the detailed error can be found using
		“GetRegistrationStatus” command
 parm : None
 retn : Pointer where the packet buffer is stored
 note : Note that this command is also run automatically during PrePrint if this function
		(RunRegistration) has not been called explicitly after changing registration parameters
*/
API_IMPORT BOOL uvLuria_ReqSetRunRegistration();
/*
 desc : Retrieve the current status of the registration module in Luria
 parm : None
 retn : Pointer where the packet buffer is stored
*/
API_IMPORT BOOL uvLuria_ReqGetGetRegistrationStatus();
/*
 desc : Set the serial number string and number to be incremented per board
		If the serial number string is set to ABCD####WXYZ, the symbol marking increment number is '#'
		and starting value is set to 0123, then the first serial number will be printed as ABCD0123WXYZ,
		the next ABCD0124WXYZ and so on for all the D-codes defined as SerialNumber
		If the number reaches max value, e.g. 999, the next value will be 000.
 parm : 
		type	- [in]  Command Type : Read or Write
		dcode	- [in]	D-Code
		flip_h	- [in]  Horizontal Flip (1: Flip, 0: No Flip)
		flip_v	- [in]  Vertical Flip (1: Flip, 0: No Flip)
		font_h	- [in]  font size (Horizontal) (unit : um)
		font_v	- [in]  font size (Vertical) (unit : um)
		symbol	- [in]  Symbol marking number to be incremented (e.g. '#').
		start	- [in]  Starting value of number to be incremented.
		s_len	- [in]  Length of string (S). Max = 32.
		serial	- [in]  Serial number string
 retn : Pointer where the packet buffer is stored
*/
API_IMPORT BOOL uvLuria_ReqGetSerialNumber(UINT32 dcode);
API_IMPORT BOOL uvLuria_ReqSetSerialNumber(UINT32 dcode, UINT8 symbol,
										   UINT16 s_len, PUINT8 serial, UINT32 start,
										   UINT8 flip_h, UINT8 flip_v,
										   UINT16 font_h, UINT16 font_v);
/*
 desc : Specify the panel data to be printed on the panel, where to print it and the size/orientation
		D-code must match what is specified in the job (from PreProcessing),
		where also the PanelData type is pre-defined
		For Scaling information, the Length of string should be set to 0 and Text string (S) should be ignored
		For GeneralText and ECC 200 the text string is what will be printed/shown hence Length of text string
		and text string must be set accordingly
		For Paneldata-BMP the Text string specifies the path of the bitmap to be used
		The paneldata bitmap must have: 1 bit per pixel, max width/height 512 pixels,
		total bitmap size cannot be larger than 32768 pixels and width/height must be divisible by 32
 parm : 
		dcode	- [in]  D-Code (Panel Dcode List 값 중 1개)
		flip_x	- [in]  X-Flip. 1 = flip, 0 = no flip
		flip_y	- [in]  Y-Flip. 1 = flip, 0 = no flip
		font_h	- [in]  font size (Horizontal) (unit : um)
		font_v	- [in]  font size (Vertical) (unit : um)
		s_len	- [in]  Length of string (S). (Only valid for GeneralText, should be 0 in other cases). Max = 32.
		text	- [in]  Text string
 retn : 패킷이 저장된 버퍼 포인트 반환 (호출한 쪽에서 반드시 메모리 해제)
*/
API_IMPORT BOOL uvLuria_ReqGetPanelData(UINT32 dcode);
API_IMPORT BOOL uvLuria_ReqSetPanelData(UINT32 dcode, UINT16 s_len, PUINT8 text,
										UINT8 flip_x, UINT8 flip_y, UINT16 font_h, UINT16 font_v);
/*
 desc : If “Use shared zones” is selected, then the system will try to create shared zones where possible
 parm : flag	- [in]  Get (Read) or Set (Write)
		use		- [in]  1 = Use shared zones, 0 = Do not use shared zoned
 retn : Pointer where the packet buffer is stored
*/
API_IMPORT BOOL uvLuria_ReqGetUseSharedLocalZones();
API_IMPORT BOOL uvLuria_ReqSetUseSharedLocalZones(UINT8 use);
/*
 desc : Transformation recipe that is valid for local zones
		If set to all Fixed, the values in LocalFixedRotation, LocalFixedScaling and LocalFixedOffset are used
 parm : flag	- [in]  Get (Read) or Set (Write)
		info	- [in]  Local transformation recipe for all zones
						Bit 0 All transformation parameters fixed, 0 = Auto, 1 = Fixed
						If any of the other bits 1 to 3 is set, these bits override bit 0, and Rotation, Scaling and Offset may be set on or off individualy.
						Bit 1 Rotation. 0 = Auto, 1 = Fixed. LocalFixedRotation input are used.
						Bit 2 Scaling. 0 = Auto, 1 = Fixed. LocalFixedScaling input are used.
						Bit 3 Offset. 0 = Auto, 1 = Fixed. LocalFixedOffset input are used.
 retn : Pointer where the packet buffer is stored
*/
API_IMPORT BOOL uvLuria_ReqGetLocalTransformationRecipe();
API_IMPORT BOOL uvLuria_ReqSetLocalTransformationRecipe(UINT8 info);
/*
 desc : Has effect in fixed transformation mode only. Fixed rotation to be used. N <= 200
 parm : 
		count	- [in]  Number of local zones (N)
		rotation- [in]  Fixed rotation for local zone 1, in micro-degrees
 retn : Pointer where the packet buffer is stored
*/
API_IMPORT BOOL uvLuria_ReqGetLocalFixedRotation(UINT16 count, PINT32 rotation);
API_IMPORT BOOL uvLuria_ReqSetLocalFixedRotation(UINT16 count, PINT32 rotation);
/*
 desc : Has effect in fixed transformation mode only
		Fixed scaling to be used. N <= 200.
		(Eg. Factor 1.0002 is given as 1000200, factor 0.9998 is given as 999800)
 parm : 
		count	- [in]  Number of local zones (N)
		scale_x	- [in]  Local zone 1 fixed x-scaling factor * 10^6
		scale_y	- [in]  Local zone 1 fixed y-scaling factor * 10^6
 retn : Pointer where the packet buffer is stored
*/
API_IMPORT BOOL uvLuria_ReqGetLocalFixedScaling();
API_IMPORT BOOL uvLuria_ReqSetLocalFixedScaling(UINT16 count, PUINT32 scale_x, PUINT32 scale_y);
/*
 desc : Has effect in fixed transformation mode only. Fixed offset to be used. N <= 200
 parm : 
		count	- [in]  Number of local zones (N).
		offset_x- [in]  Local fixed x-offset. factor (unit: nm). Ignored for TransformationReceipe 0.
		offset_x- [in]  Local fixed y-offset. factor (unit: nm). Ignored for TransformationReceipe 0.
 retn : Pointer where the packet buffer is stored
*/
API_IMPORT BOOL uvLuria_ReqGetLocalFixedOffset();
API_IMPORT BOOL uvLuria_ReqSetLocalFixedOffset(UINT16 count, PINT32 offset_x, PINT32 offset_y);
/*
 desc : When global auto transformation recipe is selected,
		it is possible to force the registration points to make a rectangle
 parm : flag	- [in]  Get (Read) or Set (Write)
		lock	- [in]  1 = Force global rectangle lock, 0 = Do not force global rectangle lock
 retn : Pointer where the packet buffer is stored
*/
API_IMPORT BOOL uvLuria_ReqGetGlobalRectangleLock();
API_IMPORT BOOL uvLuria_ReqSetGlobalRectangleLock(UINT8 lock);
/*
 desc : Will remove the panel data that is assigned to the given d-code
 parm : dcode	- [in]  D-Code
 retn : Pointer where the packet buffer is stored
*/
API_IMPORT BOOL uvLuria_ReqSetRemovePanelData(UINT32 dcode);
/*
 desc : If warping is out of system limits, the command GetRegistrationStatus will return;
		WarpOutsideLimitsInZone or WarpOutsideLimitsOutsideZone
		In this case it is stell possible to performe print, but the printed result will contain errors
		in some areas of the panel
		This command, GetWarpOfLimitsCoordinates may be used to get coordinates and the corresponding error,
		where the panel print is outside limits
		This may be used to decide whether position and severity of the error is acceptable or not
		If acceptable the print may continue
		It is possible to continued print if the error is not severe
		A typical case where WarpOustideLimitsOutsideZone, or when only a small part of the panel is damaged
		Max reported error coordinates is 400
 parm : None
 retn : Pointer where the packet buffer is stored
*/
API_IMPORT BOOL uvLuria_ReqGetGetWarpOfLimitsCoordinates();
/*
 desc : When this mode is enabled, the artwork is always warped to the closest zone registration setting
		This mode can only be used in combination with non shared local zones
		And the zones have to be distributed in a grid parallel to x and y axis (not scattered)
		Recommended to use only one zone in y direction.
 parm : flag	- [in]  Get (Read) or Set (Write)
		mode	- [in]  0 = Disable, 1 = Enable
 retn : Pointer where the packet buffer is stored
 note : Valid from Luria version 2.12.0 and 3.0.0, and newer
*/
API_IMPORT BOOL uvLuria_ReqGetSnapToZoneMode();
API_IMPORT BOOL uvLuria_ReqSetSnapToZoneMode(UINT8 mode);
/*
 desc : Set the band width for searching fiducials in a zone
		Nominal values are typically below 1mm to allow strictly rectangular zones only
		This interface may be used to increase the search limit to allow for non square zones
		Values should be set as low as possible to avoid risk of confusing fiducials between zones
 parm : 
		bw_x	- [in]  X-Search band width. (unit: um)
		bw_y	- [in]  Y-Search band width. (unit: um)
 retn : Pointer where the packet buffer is stored
*/
API_IMPORT BOOL uvLuria_ReqGetLocalZoneFidSearchBw();
API_IMPORT BOOL uvLuria_ReqSetLocalZoneFidSearchBw(UINT32 bw_x, UINT32 bw_y);

/* --------------------------------------------------------------------------------------------- */
/*                            Panel Preparation Get & Set - Extended                             */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Fixed Rotation, Fixed Scaling, Fixed Offset 요청 / 설정
 parm : rotation- [in]  Global fixed rotation, in micro-degrees (* 1000). Ignored for TransformationReceipe 0.
		scale_x	- [in]  Global fixed x-scaling factor * 1000000 . Ignored for TransformationReceipe 0.
		scale_y	- [in]  Global fixed x-scaling factor * 1000000 . Ignored for TransformationReceipe 0.
		offset_x- [in]  Global fixed x-offset. In mm * 1000000. Ignored for TransformationReceipe 0.
		offset_x- [in]  Global fixed y-offset. In mm * 1000000. Ignored for TransformationReceipe 0.
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetGlobalFixed();
API_IMPORT BOOL uvLuria_ReqSetGlobalFixed(INT32 rotation, UINT32 scale_x, UINT32 scale_y,
										  INT32 offset_x, INT32 offset_y);
/*
 desc : Registration (Registration Points) 요청 / 설정
 parm : p_count	- [in]  Registration Points Count
		p_fidxy	- [in]  X/Y 좌표가 저장된 배열 포인터[xx][2] (단위 : 1000000)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetRegistrationPointsAndRun(UINT16 p_count, LPG_I32XY p_fidxy);

/* --------------------------------------------------------------------------------------------- */
/*                                  Panel Preparation - Normal                                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Returns the status of registration
 parm : status	- [in]  status code
		mesg	- [out] The buffer in which the string of the code value will be stored
		size	- [in]  The size of 'mesg' buffer
 retn : None
*/
API_IMPORT VOID uvLuria_GetRegistrationStatus(UINT16 status, PTCHAR mesg, UINT32 size);

/* --------------------------------------------------------------------------------------------- */
/*                                       Exposure - Single                                       */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Scroll Step Size / Frame Rate Factor / Led Duty Cycle 요청 / 설정
 parm : flag		- [in]  Function Type : Get or Set
		step		- [in]  1 = Step size 1, 2 = Step size 2, etc
		duty		- [in]  LED duty cycle. Range 0 ? 100%
		frame_rate	- [in]  Set frame rate factor. 0 < FrameRateFactor*1000 <= 1000, ex. range is 1 ? 1000.
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetExposureFactor();
API_IMPORT BOOL uvLuria_ReqSetExposureFactor(UINT8 step, UINT8 duty, UINT16 frame_rate);
/*
 desc : Set - Frame Rate
 parm : rate	- [in]  Frame Rate 값 (0.1 ~ 0.95)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetFrameRate(DOUBLE frame_rate);
/*
 desc : LED Amplitude 요청 / 설정 (Used in expose)
 parm : count	- [in]  Photohead Count
		amp		- [in]	Led Amplitude (2차원 배열 포인터. 각 차원마다 4개씩 존재)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetLightAmplitude(UINT8 count);
API_IMPORT BOOL uvLuria_ReqSetLightAmplitude(UINT8 count, UINT16 (*amp)[4]);
/*
 desc : Set - LED Amplitude (When setting only one photohead ...)  (Used in expose)
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08)
		led_no	- [in]  LED Number (1:365, 2:385, 3:395, 4:405)
		amp_idx	- [in]	Led Amplitude
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetLightAmplitudeOne(UINT8 ph_no, ENG_LLPI led_no, UINT16 amp_idx);
/*
 desc : Only Set - Print Abort
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetPrintAbort();
/*
 desc : Only Set - Reset Exposure State (!!! Not Implemented. Not support to luria !!!)
 parm : None
 retn : 패킷 버퍼
*/
API_IMPORT BOOL uvLuria_ReqSetResetExpousreState();
/*
 desc : Only Get - Exposure State
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetExposureState();
/*
 desc : Only Set - Move to Start the position for printing
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetPrintStartMove();
/*
 desc : Only Set - This command should be sent to Luria if the current light amplitude is
		significantly higher/lower than the amplitude used in the previous print.
		The command results in a short light-output from the photo heads
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetPrintRampUpDown();
/*
 desc : AF Sensor Type과 설정 (Setting) 값을 요청 / 설정
 parm : type	- [in]  AF Sensor Type. 1 = Diffuse (default):확산광 방식, 2 = Specular:반사광 방식
		agc		- [in]  Laser AGC (automatic gain control) setting 값
						1 = AGC on (default), 0 = AGC off (value in PWM-field is used)
		pwm		- [in]  'agc' 값이 0인 경우, 이 값으로 설정됨 (range:2 ~ 511. default=100)
 retn : TRUE or FALSE
 note : AFSensorMeasureLaserPWM 명령어 사용하기 전에 이 명령으로 AF Sensor 값 설정
		이 값 설정 시점은 PrePrint 실행할 때, Photohead 값이 설정 (저장) 됨
		다만, 이 값들은 Autofocus가 사용될 때만 적용됨
*/
API_IMPORT BOOL uvLuria_ReqGetAFSensor();
API_IMPORT BOOL uvLuria_ReqSetAFSensor(UINT8 type, UINT8 agc, UINT16 pwm);
/*
 desc : 최적의 laser PWM 값 요청
 parm : None
 retn : TRUE or FALSE
 note : Luria 에서 현재 Job에 측정된 값이 저장됨. 만약 New Job이 적재되면, 새로운 측정 값을 가져와야 함
		AFSensor는 Laser AGC 설정 2로 측정된 값으로 사용됨 (명령어: AFSensor (0x11))
		다만, 이 값들은 Autofocus가 사용될 때만 적용됨
*/
API_IMPORT BOOL uvLuria_ReqGetAFSensorMeasureLaserPWM();
/*
 desc : Photo head 별로 설치된 이용 가능한 LED의 개수 및 Source Type  요청 / 설정
 parm : ph_no	- [in]  Photohead Number (1 based)
 retn : TRUE or FALSE
 note : Light Source Type (0: LED, 1: Laser, 2: LED Combiner)
*/
API_IMPORT BOOL uvLuria_GetPktNumberOfLightSource(UINT8 ph_no);
/*
 desc : Get the light source type and status of all light sources in all photo heads
 parm : ph_count	- [in]  Number of photo heads
		led_count	- [in]  Number of light sources in each photo head
		light_type	- [in]  0x00=LED, 0x01=Laser, 0x02=LED combiner, 4=Knife edge laser
 retn : Packet buffer
 note : This command replaces the need of sending multiple GetLightSourceStatus command
 vers : Valid from Luria version 2.12.0 and newer
*/
API_IMPORT BOOL uvLuria_GetPktLightSourceStatusMulti();
/*
 desc : Get or Set - When function is enabled, the photo head printing the last strip will copy the autofocus profile of the second to last strip and use that profile for focusing the last strip.
 parm : enable	- [in]  1 : Enable AF copy function. 0 = Disable (default)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetEnableAfCopyLastStrip();
API_IMPORT BOOL uvLuria_ReqSetEnableAfCopyLastStrip(UINT8 enable);
/*
 desc : Get or Set - Disable autofocus for the first strip for the first photohead
 parm : disable	- [in]  1 = Disable autofocus first strip, 0 = Do not disable autofocus first strip (default).
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetDisableAfFirstStrip();
API_IMPORT BOOL uvLuria_ReqDisableAfFirstStrip(UINT8 disable);

/* --------------------------------------------------------------------------------------------- */
/*                                      Exposure - Extended                                      */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Print 관련 Option
 parm : mode	- [in]  Print 관련 명령어
						0 : Pre-Print
						1 : Print
						2 : Simulation Print
						3 : Abort
						4 : Reset Exposure State
						5 : Move Start Position
						6 : Light Ramp Up/Down
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetPrintOpt(UINT8 mode);

/* --------------------------------------------------------------------------------------------- */
/*                                   Direct Photohead - Single                                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Initialize the z-axis of all photoheads (moved to center position)
		(However, this applies only to the photohead whose motor is not initialized)
 parm : ph_no	- [in]  Photohead Number (1 ~ 8)
		action	- [in]  0x01 : moved to center position
						0x00 : not moved to center position
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetMotorPositionInit(UINT8 ph_no, UINT8 action);
API_IMPORT BOOL uvLuria_ReqSetMotorPositionInitAll(UINT8 action);
/*
 desc : 모든 Photohead의 Z 축 모터의 현재 상태 요청
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetMotorStateAll();
/*
 desc : Focus Motor Move Absolute Position (현재 위치 얻기)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetMotorAbsPositionAll();
/*
 desc : Focus Motor Move Absolute Position (Move to set position)
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		pos		- [in]  Absolute move position value (unit: nm) (rangle: 64.000mm ~ 84.000mm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetMotorAbsPosition(UINT8 ph_no, DOUBLE pos);
/*
 desc : Focus Motor Move Absolute Position (Move to set position)
 parm : count	- [in]  Photohead Count
		pos		- [in]  Buffer in which absolute positions are stored as many as 'count'
						(unit: mm) (rangle: 64.000~84.000)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetMotorAbsPositionAll(UINT8 count, DOUBLE* pos);
/*
 desc : Focus Motor Move Absolute Position (모든 Photohead의 Z Axis을 포커스 위치로 이동)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetMotorFocusAbsPositionAll();
/*
 desc : Focus Motor Calibration - Measured Distance (Get or Set)
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		dist	- [in]  이동하고자 하는 거리 값 (unit: um, 범위: 75000 ~ 8000)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetFocusMotorMeasureDist(UINT8 ph_no);
API_IMPORT BOOL uvLuria_ReqSetFocusMotorMeasureDist(UINT8 ph_no, UINT32 dist);
/*
 desc : Request to get the ImageType to bed saved on the Photohead
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
 retn : TRUE or FALSE
 note : Image Type
		0x0001 - Binary, uncompressed. (1-bit bitmap (.bmp) without bmp-header (62 bytes))
		0x0002 - 1-bit RLE-encoded image data
		0x0003 - 8-bit grayscale bitmap (.bmp). This is used by web-interface only. Will be converted to binary (type. 1)
		0x0004 - 1-bit bitmap (.bmp). This is used by the web-interface only. Will be converted to binary (type 1).
		0x0005 - 1-bit RLE-encoded image data with swapped bits within bytes.
*/
API_IMPORT BOOL uvLuria_ReqGetImageType(UINT8 ph_no);
/*
 desc : Request to Request Get/Set - the internal image number to be loaded on the photohead
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		img_no	- [in]  The internal image to be displayed (Loaded) (0x00 ~ 0x04)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetLoadInternalImage(UINT8 ph_no);
API_IMPORT BOOL uvLuria_ReqSetLoadInternalImage(UINT8 ph_no, UINT8 img_no);
/*
 desc : Enable Flatness Correction Mask (PPC) 요청 or 설정
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		enable	- [in]  Select the usage mode (0x00 - Disable : Spot Mode, 0x01 - Enable : Expo Mode)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetFlatnessMaskOn(UINT8 ph_no);
API_IMPORT BOOL uvLuria_ReqSetFlatnessMaskOn(UINT8 ph_no, UINT8 enable);
API_IMPORT BOOL uvLuria_ReqSetDMDSpotMode(UINT8 ph_no);
API_IMPORT BOOL uvLuria_ReqSetDMDExpoMode(UINT8 ph_no);
API_IMPORT BOOL uvLuria_ReqGetDMDSetMode(UINT8 ph_no);
/*
 desc : This record makes it possible to select which sequence file to be used
		It internal sequence is selected, the image that is currently loaded in inum 0, offset 0 will be displayed.
		The build-in sequence file will automatically be fitted to the current DMD-size that is mounted in the system
		(1024x768, 1920x1080 or 1920x1200)
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		type	- [in]  0x00 : Use last loaded (external) sequence file
						0x01 : Use internal sequence file
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetActiveSequence(UINT8 ph_no, UINT8 type);
API_IMPORT BOOL uvLuria_ReqGetActiveSequence(UINT8 ph_no);
/*
 desc : Light Source Driver Light On/Off
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08)
		led_no	- [in]  LED Number (1:365, 2:385, 3:395, 4:405)
		onoff	- [in]  0x00 : Light Off, 0x01 : Light ON
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetLedLightOnOff(UINT8 ph_no, ENG_LLPI led_no, UINT8 onoff);
API_IMPORT BOOL uvLuria_ReqGetLedLightOnOff(UINT8 ph_no, ENG_LLPI led_no);
API_IMPORT BOOL uvLuria_ReqSetLedLightOnOffAll(UINT8 onoff);
API_IMPORT BOOL uvLuria_ReqGetLedLightOnOffAll();
/*
 desc : This record is used to manipulate and request the status of the sequencer in Luxibeam (Halted or Running)
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		seq_cmd	- [in]  Command to send/request (1 : run, 2 : reset, 3 : 3 Unused)
		enable	- [in]  1 - enable, 0 - disable
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetSequnceState(UINT8 ph_no, UINT8 seq_cmd, UINT8 enable);
API_IMPORT BOOL uvLuria_ReqGetSequnceState(UINT8 ph_no, UINT8 seq_cmd);
API_IMPORT BOOL uvLuria_ReqSetSequnceRun(UINT8 ph_no);
API_IMPORT BOOL uvLuria_ReqSetSequnceStop(UINT8 ph_no);
API_IMPORT BOOL uvLuria_ReqSetSequnceResetEnable(UINT8 ph_no);
API_IMPORT BOOL uvLuria_ReqSetSequnceResetDisable(UINT8 ph_no);
/*
 desc : LED Driver Amplitude. That is, Records used to manipulate the amplitude in the Light Source drivers
		(It is different from the LedAmplitude function. The purpose of this function is to use th SpotMode)
		(Used in direct photohead)
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08)
		led_no	- [in]  LED Number (1:365, 2:385, 3:395, 4:405)
		amp_idx	- [in]  The amplitude to set for the selected Light Source (Range: 0 ~ 4095)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetLedPower(UINT8 ph_no, ENG_LLPI led_no, UINT16 amp_idx);
API_IMPORT BOOL uvLuria_ReqGetLedPower(UINT8 ph_no, ENG_LLPI led_no);
API_IMPORT BOOL uvLuria_ReqSetLedPowerLedAll(ENG_LLPI led_no, UINT16 amp_idx);
API_IMPORT BOOL uvLuria_ReqGetLedPowerLedAll(ENG_LLPI led_no);
/*
 desc : Records that requests status of the Light Source drivers
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		led_no	- [in]  LED Number (1:365, 2:385, 3:395, 4:405)
 retn : TRUE or FALSE
 note : Note that all errors/warnings will be retained in the status word
		until it is actively cleared, using command <<Clear Error Status>>
*/
API_IMPORT BOOL uvLuria_ReqGetLedSourceDriverStatus(UINT8 ph_no, ENG_LLPI led_no);
/*
 desc : Luria Server에 Photohead LED On-Time 값 요청
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetPhLedOnTimeAll();
/*
 desc : Luria Server에 Photohead LED Temperature 값 요청
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetPhLedTempAll();
/*
 desc : Luria Server에 Photohead LED Temperature 값 요청 - 전체 Photohead 중 주파수 별 값 요청
 parm : freq_no	- [in]  전체 Photohead의 주파주 번호 (0:365, 1:385, 2:395, 4:405)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetPhLedTempFreqAll(UINT8 freq_no);
/*
 desc : 모든 Photohead 에 발생된 Error Clear 처리
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetClearErrorStatus();
/*
 desc : 현재 연결된 Light Source 모듈에 대한 정보 요청
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetLightSource();
/*
 desc : 특정 Photohead에 대한 Line Sensor Data의 Plotting을 Enable or Disable
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08)
		enable	- [in]  enable (0x01) or Disable (0x00)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetLineSensorPlot(UINT8 ph_no, UINT8 enable);
API_IMPORT BOOL uvLuria_ReqGetLineSensorPlot(UINT8 ph_no);
/*
 desc : 현재 광학계의 Autofocus Position 확인
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetCurrentAutofocusPosition(UINT8 ph_no);

API_IMPORT BOOL uvLuria_ReqSetStoredAutofocusPosition(UINT8 ph_no, UINT16 setPos);
API_IMPORT BOOL uvLuria_ReqGetStoredAutofocusPosition(UINT8 ph_no);

API_IMPORT BOOL uvLuria_ReqSetAFSensorOnOff(UINT8 ph_no, bool on);
API_IMPORT BOOL uvLuria_ReqGetAFSensorOnOff(UINT8 ph_no);

API_IMPORT BOOL uvLuria_ReqSetAFSensorType(UINT8 ph_no, UINT8 sensorType);
API_IMPORT BOOL uvLuria_ReqGetAFSensorType(UINT8 ph_no);


/*
 desc : 특정 Photohead에 설치된 카메라의 LED Lamp Light Value (0x00 ~ 0xff) 값 설정
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08)
		value	- [in]  Illumination Light Value (0x00 ~ 0xff)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetLedLampLevel(UINT8 ph_no, UINT8 value);
API_IMPORT BOOL uvLuria_ReqGetLedLampLevel(UINT8 ph_no);
/*
 desc : 광학계 내에 설치된 카메라의 밝기 정보 값이 동일할 때까지 대기
 parm : ph_no		- [in]  Photohead Number (0x01 ~ 0x08)
		led_level	- [in]  비교 LED Level 값
		time_out	- [in]  최종 대기 시간 (unit: msec)
 retn : TRUE (동일하다) or FALSE (Time out)
*/
API_IMPORT BOOL uvLuria_IsLedLightLevelEqual(UINT8 ph_no, UINT8 led_level, UINT64 time_out);
/*
 desc : 현재 광학계의 DMD Flip Output 여부 확인
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08)
		flip	- [in]  0x00: No flipping, 0x01: Flip image in Y direction
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetDmdFlipOutput(UINT8 ph_no, UINT8 flip);
API_IMPORT BOOL uvLuria_ReqGetDmdFlipOutput(UINT8 ph_no);
/*
 desc : This record will turn on/off the shaking function of the DMD mirrors.
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		enable	- [in]  0 : DMD mirror shake OFF, 1 : DMD mirror shake ON
 retn : TRUE or FALSE
 note : This function requires that the internal sequencer is running
*/
API_IMPORT BOOL uvLuria_ReqSetDmdMirrorShake(UINT8 ph_no, UINT8 enable);
API_IMPORT BOOL uvLuria_ReqGetDmdMirrorShake(UINT8 ph_no);

/*
 desc : This record manipulates the duration of the light pulse that is applied when a «LightPulseWord» command
		is used in the sequence file.
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		value	- [uin16]  The duration the light pulse should be on.
 retn : Pointer where the packet buffer is stored
 note : This function requires that the internal sequencer is running
*/
API_IMPORT BOOL uvLuria_ReqSetLightPulseDuration(UINT8 ph_no, UINT16 value);
API_IMPORT BOOL uvLuria_ReqGetLightPluseDuration(UINT8 ph_no);

/* --------------------------------------------------------------------------------------------- */
/*                                       Luria ComManagement                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Only Set - Generate Test Announcement
 parm : None
 retn : 패킷 버퍼
 note : Luria에서 "TestAnnouncement"를 강제로 보내도록 설정 하는 기능
		Luria에서 AlignmentServerIpAddr가 0.0.0.0과 다른 경우 이 메시지가 전송 됨
		"TestAnnouncement"가 세 번째까지 연속으로 전송되지 않은 경우
		(즉, "GenerateTestAnnouncement"에 대한 응답이 세 번까지 OK되지 않은 경우),
		공지사항 서버 IpAddr이 0.0.0.0이 아니더라도 모든 공지사항은 비활성화 됨
		"TestAnnouncement"이 성공적으로 전송되면 (즉, "GenerateTestAnnouncement"이 정상인 경우)
		공지사항이 다시 활성화 됨
 desc : 일단, AnnouncementStatus (0x03) 명령으로 Luria의 응답 상태 값을 확인하고
		응답이 비활성화 되면, 이 명령을 보내도록 해야 함
*/
API_IMPORT BOOL uvLuria_ReqSetGenTestAnnouncement();
/*
 desc : Only Get - Announcement Status
 parm : None
 retn : 패킷 버퍼
 note : 3회 연속 공지를 보낼 수 없는 경우 Luria가 공지를 자동으로 비활성화 하며
		이 기능을 다시 활성화하는 유일한 방법은 "GenerateTestAnnouncement"을 성공적으로 보내야 함
		AlignmentServerIpAddr가 0.0.0.0이면 상태는 "Disabled" 임
*/
API_IMPORT BOOL uvLuria_ReqGetAnnouncementStatus();

/* --------------------------------------------------------------------------------------------- */
/*                                      Photohead Z Axis Focus                                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Initialize Focus (Write only) (This will initialize the z-axis) (== Homing 효과)
 parm : None
 retn : TRUE or FALSE
 note : The z-axis must be initialized in order to use any of the below focus functions.
		Note that this may take up to 30 seconds to complete.
		If initialize fails, the status code FocusMotorOperationFailed is given.
*/
API_IMPORT BOOL uvLuria_ReqSetInitializeFocus();
/*
 desc : 임의 Photohead Z Axis의 높이 값 설정 or 요청 (unit: mm)
 parm : ph_no	- [in]  Photo head number of first axis to set position to (1 = ph #1, 2 = ph # 2, etc)
		ph_pos	- [in]  Z-position given in micrometers for given photo head (unit : mm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetPosition(UINT8 ph_no, DOUBLE pos);
API_IMPORT BOOL uvLuria_ReqSetPositionAll(UINT8 count, PDOUBLE pos);
API_IMPORT BOOL uvLuria_ReqGetPosition(UINT8 ph_no);
/*
 desc : Start / Stop (Enable / Disable) photo head auto focus system
 parm : ph_no	- [in]  Photo head number (0xff:All ph, 0x01:ph #1, 0x02:ph # 2, etc)
		enable	- [in]  0x01 : Enable auto focus, 0x00 = Disable Auto focus.
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetAutoFocus(UINT8 ph_no, UINT8 enable);
API_IMPORT BOOL uvLuria_ReqGetAutoFocus(UINT8 ph_no);
/*
 desc : Autofocus set-point trim value
 parm : ph_no	- [in]  Photo head number (1 = ph #1, 2 = ph # 2, etc)
		trim	- [in]  Focus trim (unit: mm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetTrim(UINT8 ph_no, DOUBLE trim);
API_IMPORT BOOL uvLuria_ReqGetTrim(UINT8 ph_no);
/*
 desc : Autofocus set-point trim value
 parm : enable	- [in]  0x01: Enable, 0x00: Disable
		start	- [in]  Distance from exposure <start> where AF is inactive (mm)
		end		- [in]  Distance from exposure <end> where AF is inactive (mm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqSetActiveAreaQualifier(UINT8 enable, DOUBLE start, DOUBLE end);
API_IMPORT BOOL uvLuria_ReqGetActiveAreaQualifier();
/*
 desc : Autofocus set-point trim value
 parm : ph_no	- [in]  Photo head number (0xff:All ph, 0x01:ph #1, 0x02:ph # 2, etc)
		abs_min	- [in]  The min absolute focus position that the AF will operate within (mm)
		abs_max	- [in]  The min absolute focus position that the AF will operate within (mm)
 retn : TRUE or FALSE
 note : Note that setting AbsWorkRange will clear the AbsWorkRangeStatus for the given photo head(s).
*/
API_IMPORT BOOL uvLuria_ReqSetAbsWorkRange(UINT8 ph_no, INT32 abs_min, INT32 abs_max);
API_IMPORT BOOL uvLuria_ReqGetAbsWorkRange(UINT8 ph_no);
/*
 desc : return the status word giving status of autofocus absolute work range
 parm : None
 retn : TRUE or FALSE
 note : The status-bit for a given photo head will be cleared next time a new AbsWorkRange has been set
		!!! AbsWorkRange with photo head number = 0 (all photo heads), then all status bits will be cleared !!!
*/
API_IMPORT BOOL uvLuria_ReqGetAbsWorkRangeStatus();
/*
 desc : Autofocus Edge Trigger
 parm : dist	- [in]  The max distance below or above current AF set-point that AF will be active (mm)
		enable	- [in]  Enable or disable the edge trigger function. 1 = enable, 0 = disable
 retn : TRUE or FALSE
 note : Note that setting AbsWorkRange will clear the AbsWorkRangeStatus for the given photo head(s).
*/
API_IMPORT BOOL uvLuria_ReqSetAfEdgeTrigger(DOUBLE dist, UINT8 enable);
API_IMPORT BOOL uvLuria_ReqGetAfEdgeTrigger();
/*
 desc : 모든 Photohead Z Axis의 높이 값 설정 (unit: mm)
 parm : count	- [in]  Number (N) of photo heads to send position to
		ph_no	- [in]  Photo head number of first axis to set position to (1 = ph #1, 2 = ph # 2, etc)
		pos		- [in]  Z-position given in micrometers for given photo head.
 retn : 패킷 버퍼
*/
API_IMPORT BOOL uvLuria_ReqSetPositionMulti(UINT8 count, PUINT8 ph_no, PDOUBLE pos);
/*
 desc : 임의 Photohead Z Axis의 정밀 조정 (unit: mm)
 parm : 
		ph_no	- [in]  Photo head number of first axis to set position to (1 = ph #1, 2 = ph # 2, etc)
		pos		- [in]  Z-position given (mm)
 retn : TRUE or FALSE
 note : Linear motor rounds to nearest 100nm, while stepper motor rounds to nearest um
*/
API_IMPORT BOOL uvLuria_ReqSetPositionHiRes(UINT8 ph_no, DOUBLE pos);
API_IMPORT BOOL uvLuria_ReqGetPositionHiRes(UINT8 ph_no);
/*
 desc : Reports the number of trigger pulses that autofocus position has been outside the given DOF (MachineConfig:DepthOfFocus)
 parm : None
 retn : TRUE or FALSE
 note : The counter is reset when this command is finished,
		so the number reported is the number of trigger pulses outside DOF since last read.
		This is valid for linear z-motor only.
		If trying to read this number when connected to a stepper z-motor, then 0 will be returned.
*/
API_IMPORT BOOL uvLuria_ReqGetOutsideDOFStatus();
/*
 desc : Get focus position for all photo heads simultaneously (unit: um)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLuria_ReqGetPositionMulti();
/*
 desc : Autofocus Gain
 parm : 
		gain	- [in]  Gain/damping factor. (Valid range 1 - 4095)
 retn : TRUE or FALSE
 note : This will set the autofocus gain (damping factor) in the photo heads
		The default gain value is 10.
*/
API_IMPORT BOOL uvLuria_ReqSetAfGain(UINT16 gain);
API_IMPORT BOOL uvLuria_ReqGetAfGain();


#ifdef __cplusplus
}
#endif
