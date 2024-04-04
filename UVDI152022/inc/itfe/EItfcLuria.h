
/*
 desc : The Interface Library for VISITECH's Luria Service
*/

#pragma once

#include "../conf/global.h"
#include "../conf/define.h"
#include "../conf/luria.h"
#include "../../prj/ItfcLuria/JobSelectXml.h"
 
class AlignMotion;

#ifdef __cplusplus
extern "C"
{
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                             XML                                               */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : XML 파일로부터 적재된 Global Mark Data 반환 (Local & Global)
 parm : index	- [in]  Mark가 저장되어 있는 Index (Zero based)
		data	- [out] Gerber X / Y Position (unit: mm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_GetGlobalMark(UINT16 index, LPG_XMXY point);
/*
 desc : Global Mark 좌표 두 개의 위치 값 얻기
 parm : direct	- [in]  두 개의 좌표를 얻고자하는 방향 정보 구분 값
		data1	- [out] Gerber XY & Motion XY Position (unit: mm)
		data2	- [out] Gerber XY & Motion XY Position (unit: mm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_GetGlobalMarkDirect(ENG_GMDD direct, LPG_XMXY data1, LPG_XMXY data2);
/*
 desc : XML 파일로부터 적재된 Local Mark Data 반환 (Local & Global)
 parm : index	- [in]  Mark가 저장되어 있는 Index 값 (Zero based)
		point	- [out] Gerber X / Y Position (unit: mm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_GetLocalMark(UINT16 index, LPG_XMXY point);
/*
 desc : 거버에 등록된 Mark 개수 반환
 parm : mark	- [in]  Mark Type (ENG_AMTF)
 retn : 개수
*/
API_IMPORT UINT8 uvEng_Luria_GetMarkCount(ENG_AMTF mark);
/*
 desc : 거버 크기 반환
 parm : width	- [out] 넓이 값 반환 (unit: mm)
		height	- [out] 높이 값 반환 (unit: mm)
 retn : None
*/
API_IMPORT VOID uvEng_Luria_GetGerberSize(DOUBLE &width, DOUBLE &height);
/*
 desc : 전역 기준점의 X 축 기준으로 2 개의 마크가 떨어진 간격 반환 (단위 : mm)
 parm : direct	- [in]  축의 마크 간격 (0 - 1번 마크와 3번 마크 간의 넓이 값)
									   (1 - 2번 마크와 4번 마크 간의 넓이 값)
									   (2 - 1번 마크와 2번 마크 간의 넓이 값)
									   (3 - 3번 마크와 4번 마크 간의 넓이 값)
									   (4 - 1번 마크와 4번 마크 간의 넓이 값)
									   (5 - 2번 마크와 3번 마크 간의 넓이 값)
 retn : X 축의 2개의 마크 떨어진 간격 (unit: mm)
*/
API_IMPORT DOUBLE uvEng_Luria_GetGlobalMarkDist(ENG_GMDD direct);
/*
 desc : 두 Mark 간의 좌/우 X 축 떨어진 간격 즉, X 축 간의 오차 (거리) 값
 parm : mark_x1	- [in]  X1 축 Mark Number (1-based. 0x01 ~ 0x04)
		mark_x2	- [in]  X2 축 Mark Number (1-based. 0x01 ~ 0x04)
 retn : 두 지점의 Y 축 간의 높이 차이 (unit: mm)
*/
API_IMPORT DOUBLE uvEng_Luria_GetGlobalMarkDiffX(UINT8 mark_x1, UINT8 mark_x2);
/*
 desc : 두 Mark 간의 상/하 Y 축 떨어진 간격 즉, Y 축 간의 오차 (높이) 값
 parm : mark_y1	- [in]  Y1 축 Mark Number (1-based. 0x01 ~ 0x04)
		mark_y2	- [in]  Y2 축 Mark Number (1-based. 0x01 ~ 0x04)
 retn : 두 지점의 Y 축 간의 높이 차이 (unit: mm)
*/
API_IMPORT DOUBLE uvEng_Luria_GetGlobalMarkDiffY(UINT8 mark_y1, UINT8 mark_y2);

/*
 desc : Mark 1번과 3번, Mark 2번과 4번의 X 좌표 오차 값을 반환
 parm : type	- [in]  0 : Mark 2번 기준으로 Mark 1번의 X 오차 값 반환
						1 : Mark 4번 기준으로 Mark 3번의 X 오차 값 반환
						2 : Mark 1번 기준으로 Mark 2번의 X 오차 값 반환
						3 : Mark 3번 기준으로 Mark 4번의 X 오차 값 반환
 retn : 두 마크의 Y 위치 시작 오차 값 반환 (unit: mm)
*/
API_IMPORT DOUBLE uvEng_Luria_GetGlobalMarkDiffVertX(UINT8 type);
/*
 desc : Align Camera 1번과 2번 간의 Mark X 축 떨어진 간격
 parm : mode	- [in]  0x00 : Width, 0x01 : Height
		scan	- [in]  스캔하려는 위치 (번호. 0 or 1)
 retn : 두 지점의 X 축 간의 떨어진 거리 (unit: 100 nm or 0.1 um)
*/
API_IMPORT DOUBLE uvEng_Luria_GetLocalMarkACam12DistX(UINT8 mode, UINT8 scan);

API_IMPORT VOID uvEng_Luria_SetAlignMotionPtr(AlignMotion& motion);

/*
 desc : 두 Mark 간의 상/하 Y 축 떨어진 간격 즉, Y 축 간의 오차 (높이) 값
 parm : mark_y1	- [in]  Y1 축 Mark Number (Zero based)
		mark_y2	- [in]  Y2 축 Mark Number (Zero based)
 retn : 두 지점의 Y 축 간의 높이 차이 (unit: 100 nm or 0.1 um)
*/
API_IMPORT DOUBLE uvEng_Luria_GetLocalMarkDiffY(UINT8 mark_y1, UINT8 mark_y2);
/*
 desc : Mark 1번과 3번, Mark 2번과 4번의 X 좌표 오차 값을 반환 (수직 오차 값)
 parm : scan	- [in]  Align Scan하려는 위치 (번호. 0 or 1)
		cam1	- [in]  Left/Bottom에 위치한 Mark X 위치 기준으로 오차 값 반환 (Left/Top 순으로 저장) (unit: mm)
		cam2	- [in]  Right/Bottom에 위치한 Mark X 위치 기준으로 오차 값 반환 (Right/Top 순으로 저장) (unit: mm)
 retn : 저장된 데이터 개수 반환 (실패인 경우, 0)
*/
API_IMPORT UINT32 uvEng_Luria_GetLocalMarkDiffVertX(UINT8 scan,
													CAtlList <DOUBLE> &cam1, CAtlList <DOUBLE> &cam2);
/*
 desc : Mark Type 기준으로, Camera 1번의 Left/Bottom과 Camera 2번의 Right/Bottom의 Y 좌표 간의 높이 차 반환
 parm : None
 retn : 두 지점의 Y 축 간의 높이 차이 (unit: 100 nm or 0.1 um)
*/
API_IMPORT DOUBLE uvEng_Luria_GetGlobalLeftRightBottomDiffY();
/*
 desc : Local Fiducial 좌표 값 얻기 - Left / Bottom
 parm : scan	- [in]  스캔하려는 위치 (번호. 0 or 1)
		cam_id	- [in]  Align Camera Index (1 or 2)
		data	- [out] Fiducial 값이 저장될 참조 변수
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_GetLocalBottomMark(UINT8 scan, UINT8 cam_id, LPG_XMXY data);
/*
 desc : 현재 선택된 거버의 XML 데이터 읽기
 parm : align	- [in]  Gerber Data에 저장된 Mark Type (ENG_ATGL)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_LoadSelectJobXML(ENG_ATGL align=ENG_ATGL::en_global_0_local_0x0_n_point);

/*
 desc : 현재 선택된 거버의 XML 데이터 내 글로벌 피두셜 읽기
 parm : 
 retn : CFiducialData*
*/
API_IMPORT CFiducialData* uvEng_Luria_GetGlobalFiducial();
/*
 desc : 현재 선택된 거버의 XML 데이터 내 로컬피두셜 읽기
 parm : 
 retn : CFiducialData*
*/
API_IMPORT CFiducialData* uvEng_Luria_GetLocalFiducial();

API_EXPORT BOOL uvEng_Luria_LoadSelectJobXMLEx(PCHAR path, ENG_ATGL align);

/*
 desc : XML 파일로에 설정(저장)된 총 노광 횟수 즉, Stripe 개수
 parm : None
 retn : 0 (Failed) or Later
*/
API_IMPORT UINT8 uvEng_Luria_GetGerberStripeCount();
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
API_IMPORT UINT16 uvEng_Luria_GetGlobalMark3PType();
/*
 desc : Global / Local Mark 존재 여부
 parm : mark	- [in]  Mark Type (ENG_AMTF)
 retn : 개수
*/
API_IMPORT UINT8 uvEng_Luria_IsMarkGlobal();
API_IMPORT UINT8 uvEng_Luria_IsMarkLocal();
/*
 desc : Global Fiducial의 Left/Bottom or Right/Bottom Mark와 Local Fiducial의 임의 위치에 해당되는
		높이 차이 값 즉, 떨어진 간격 값
 parm : direct	- [in]  Global Fiducial의 위치 정보. 0x00 : Left/Bottom, 0x01 : Right/Bottom
		index	- [in]  Local Fiducial의 위치 정보가 저장된 인덱스 (Zero-based)
 retn : 두 마크 간의 떨어진 오차 값 (unit: mm)
*/
API_IMPORT DOUBLE uvEng_Luria_GetGlobalBaseMarkLocalDiffY(UINT8 direct, UINT8 index);
/*
 desc : 1 Scan 기준으로 저장된 Local Mark 개수 반환
 parm : None
 retn : 개수
*/
API_IMPORT UINT8 uvEng_Luria_GetLocalMarkCountPerScan();
/*
 desc : 거버 데이터 내에 Local과 Global Mark 모두 등록되어 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_IsMarkMixedGlobalLocal();
/*
 desc : 거버 XML 파일 내에, Mark 개수 얻기
 parm : job		- [in]  거버 이름 (전체 경로)
		type	- [in]  Mark Type (ENG_AMTF)
 retn : 마크 개수 (0. 검색 실패). 1 or Later
*/
API_IMPORT UINT8 uvEng_Luria_GetGerberMarkCount(PTCHAR job, ENG_AMTF type);
/*
 desc : 거버 XML 파일 내에, Global Fiducial 값 얻어오기 <Fiducials> -> <Global> -> <fid> -> <gbr>
 parm : job		- [in]  거버 이름 (전체 경로)
		lst_x	- [out] Fiducial X (gbr) 위치 (mm)가 등록될 참조 리스트
		lst_y	- [out] Fiducial Y (gbr) 위치 (mm)가 등록될 참조 리스트
		count	- [in]  lst_x/y 버퍼 개수
		type	- [in]  Align Mark Type (ENG_ATGL)
 retn : 0x00 - succ, 0x01 - xml 파일 적재 실패, 0x02 - global mark 개수 부족, 0x03 - 마크 정보 얻기 실패, 0x04 - 마크 저장 공간 부족
*/
API_IMPORT UINT8 uvEng_Luria_GetGlobalMarkJobName(PTCHAR job,
												  CAtlList <DOUBLE> &lst_x,
												  CAtlList <DOUBLE> &lst_y, ENG_ATGL type);


API_IMPORT UINT8 uvEng_Luria_GetGlobalMarkJobNameVector(PTCHAR job,
														std::vector < STG_XMXY > &vstXMXY, ENG_ATGL type);


API_IMPORT UINT8 uvEng_Luria_GetGlobalMarkJobNameExt(PTCHAR job,
													 DOUBLE *lst_x, DOUBLE *lst_y, UINT16 count,
													 ENG_ATGL type);
/*
 desc : 거버 XML 파일 내에, Local Fiducial 값 얻어오기 <Fiducials> -> <Global> -> <fid> -> <gbr>
 parm : job		- [in]  거버 이름 (전체 경로)
		lst_x	- [out] Fiducial X (gbr) 위치 (mm)가 등록될 참조 리스트
		lst_y	- [out] Fiducial Y (gbr) 위치 (mm)가 등록될 참조 리스트
		count	- [in]  lst_x/y 버퍼 개수
		type	- [in]  Align Mark Type (ENG_ATGL)
 retn : 0x00 - succ, 0x01 - xml 파일 적재 실패, 0x02 - global mark 개수 부족, 0x03 - 마크 정보 얻기 실패, 0x04 - 마크 저장 공간 부족
*/
API_IMPORT BOOL uvEng_Luria_GetLocalMarkJobName(PTCHAR job,
												CAtlList <DOUBLE> &lst_x,
												CAtlList <DOUBLE> &lst_y,
												ENG_ATGL type);

API_EXPORT BOOL uvEng_Luria_GetLocalMarkJobNameVector(PTCHAR job,
												std::vector < STG_XMXY > &vstXMXY, ENG_ATGL type);

API_IMPORT UINT8 uvEng_Luria_GetLocalMarkJobNameExt(PTCHAR job,
													DOUBLE *lst_x, DOUBLE *lst_y, UINT16 count,
													ENG_ATGL type);

/* --------------------------------------------------------------------------------------------- */
/*                               Machine Config Get & Set - Single                               */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Get exposure start position (Motion X/Y) - Exposure start position considering acceleration
 parm : tbl_num	- [in]  table number (1 or 2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetTableMotionStartPosition(UINT8 tbl_no);
/*
 desc : 모션 컨트롤러의 정보 요청 / 설정
 parm : mc2			- [in]  0x01 : Sieb&Meyer, 0x02 : Newport (0x00만 유효)
		ipv4		- [in]  모션 컨트롤러의 IPv4
		max_y_speed	- [in]  Y 축 방향으로 이동할 때, 최대 속도 값 (unit: mm)
		x_speed		- [in]  X 축 방향으로 이동할 때, 속도 값 (and to move to print start position) (unit: mm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetMotionControl();
API_IMPORT BOOL uvEng_Luria_ReqSetMotionControl(UINT8 mc2, PUINT8 ipv4,
												DOUBLE max_y_speed, DOUBLE x_speed);
/*
 desc : Request Get/Set - The X/Y stage moving speed of MC2
 parm : max_y_speed	- [in]  The maximu speed in the Y-axis direction (unit: mm)
		x_speed		- [in]  The maximu speed in the X-axis direction (unit: mm) (and to move to print start position)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetMotionSpeedXY();
API_IMPORT BOOL uvEng_Luria_ReqSetMotionSpeedXY(DOUBLE max_y_speed, DOUBLE x_speed);
/*
 desc : 포토 헤드의 기초 정보 요청 / 설정
 parm : flag	- [in]  Function Type : Get or Set
		count	- [in]  전체 포토 헤드 개수
		pitch	- [in]  각 포토 헤드의 피치 (Stripe 개수)
		rate	- [in]  각 포토 헤드의 스크롤 속도 (unit: Hz)
		rotate	- [in]  광학계 회전 설치 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetAllPhotoheads();
API_IMPORT BOOL uvEng_Luria_ReqSetAllPhotoheads(UINT8 count, UINT8 pitch, UINT16 rate, UINT8 rotate);
/*
 desc : Setting the specification information of the photo head (Only IPv4)
 parm : ph_no	- [in]  Photo head number (1 to 8)
		ipv4	- [in]  IP Address consisting of 4 bytes
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetSpecPhotoHeadIPv4(UINT8 ph_no);
API_IMPORT BOOL uvEng_Luria_ReqSetSpecPhotoHeadIPv4(UINT8 ph_no, PUINT8 ipv4);
/*
 desc : Position Limit information request/setting
 parm : tbl_num	- [in]  Table Number (1 or 2)
		min_x	- [in]  Minimum absolute x-position of table. (unit : um)
		min_y	- [in]  Minimum absolute y-position of table. (unit : um)
		max_x	- [in]  Maximum absolute x-position of table. (unit : um)
		max_y	- [in]  Maximum absolute y-position of table. (unit : um)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetTablePositionLimit(UINT8 tbl_no);
API_IMPORT BOOL uvEng_Luria_ReqSetTablePositionLimit(UINT8 tbl_no,
													 INT32 max_x, INT32 max_y,
													 INT32 min_x, INT32 min_y);
/*
 desc : Exposure stage activation table setting/request
 parm : tbl_num	- [in]  table number (1 or 2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetActiveTable();
API_IMPORT BOOL uvEng_Luria_ReqSetActiveTable(UINT8 tbl_num);
/*
 desc : Print Simulation Out Dir information setting / request
 parm : out_dir	- [in]  Saved image path after simulation exposure
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetPrintSimulationOutDir();
API_IMPORT BOOL uvEng_Luria_ReqSetPrintSimulationOutDir(PTCHAR out_dir);
/*
 desc : System settings 정보 요청 / 설정
 parm : rotate	- [in]  The alignment of the photo heads
						Orientation of photoheads. 0 = No rotation. 1 = Rotated 180 degrees.
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetRotatePhotoheads();
API_IMPORT BOOL uvEng_Luria_ReqSetRotatePhotoheads(UINT8 rotate);
/*
 desc : System settings 정보 요청 / 설정
 parm : emul_moto	- [in]  Enables use of the Luria software without a motion controller.
							0 = System requires motor controller, 1 = Emulate the motor controller (no need for motor controller hardware)
		emul_head	- [in]  Enables use of the Luria software without photo head(s).
							0 =System requires photo head(s) to be connected, 1 = Emulate the photo head(s) (no need for photo heads to be connected)
		emul_trig	- [in]  Enables use of the Luria software without trigger input signals to the photo head(s).
							0 = System requires trigger signal inputs, 1 = Emulate the trigger inputs signals (no need for trigger input signals to photo head(s))
		dbg_print	- [in]  Prints debug information to logfile.
							0 =Do not print extra debug info to logfile, 1 = Print extra debug info to logfile
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetSystemSettings();
API_IMPORT BOOL uvEng_Luria_ReqSetSystemSettings(UINT8 emul_moto, UINT8 emul_head, UINT8 emul_trig,
												 UINT8 dbg_print);
/*
 desc : Artwork Complexity 설정 및 요청
 parm : complexity	- [in]  0x00:Normal, 0x01: High, 0x02: eXtreme
 retn : TRUE or FALSE
 note : Note: No longer in use from Luria version 3.0.0 and newer
*/
API_IMPORT BOOL uvEng_Luria_ReqGetArtworkComplexity();
API_IMPORT BOOL uvEng_Luria_ReqSetArtworkComplexity(UINT8 complexity);
/*
 desc : 광학계 Product Id 요청
 parm : pid	- [in]  Product ID (ENG_LPPI)
					16702 = LLS2500, 25002 = LLS04, 50002 = LLS06, 117502 = LLS15, 100001 = LLS25,
					235002 = LLS30, 200001 = LLS50
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetProductId();
API_IMPORT BOOL uvEng_Luria_ReqSetProductId(UINT32 pid);
/*
 desc : Photohead Focus Z Drive Type 요청 / 설정
 parm : type	- [in]  광학계 Focus Z Drive Type
						1 = Stepper motor (built in)
						2 = Linear drive from ACS
						3 = Linear drive from Sieb & Meyer (valid from version 2.10.0 and newer)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetFocusZDriveType();
API_IMPORT BOOL uvEng_Luria_ReqSetFocusZDriveType(UINT8 type);
/*
 desc : Photohead 내부에 과도한 압력 (over-pressure)이 만들어지기 위해 FAN의 가동 환경 요청 / 설정
 parm : enable	- [in]  Over pressure mode ON (1), OFF (0)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetOverPressureMode();
API_IMPORT BOOL uvEng_Luria_ReqSetOverPressureMode(UINT8 enable);
/*
 desc : Depth of focus 값 요청 / 설정 (Given in number of steps. 단계 수로 값이 주어짐)
		The given DOF will be used by the photo heads to determine if the current focus position
		during autofocus is within the DOF area or not.
		To determine if outside DOF, the focus:OutsideDOFStatus can be read.
		(지정된 DOF는 Photohead가 Auto-focus 중 현재 초점 위치가 DOF 영역 내에 있는지 여부를 결정
		 하는데 사용 됨. DOF 외부에 있는지 확인하려면, Focus:Outside DOFStatus를 읽을 수 있다)
 parm : steps	- [in]  Given in number of steps
 retn : TURE or FALSE
 note : Photohead에 Linear Z 축 모터가 설치된 제품에만 해당됨
*/
API_IMPORT BOOL uvEng_Luria_ReqGetDepthOfFocus();
API_IMPORT BOOL uvEng_Luria_ReqSetDepthOfFocus(UINT16 steps);
/*
 desc : valid for LLS2500 only!!!
		Extra Long stripes 노광을 진행할지 여부 플래그 설정/요청
 parm : enable	- [in]  0 : Normal length stripes, 1 : Support for extra long stripes (valid for LLS2500 only)
 retn : 패킷이 저장된 버퍼 포인트 반환 (호출한 쪽에서 반드시 메모리 해제)
*/
API_IMPORT BOOL uvEng_Luria_ReqGetExtraLongStripes();
API_IMPORT BOOL uvEng_Luria_ReqSetExtraLongStripes(UINT8 enable);
/*
 desc : 노광 할 때, 이미지의 노광 횟수를 3번 노출 (노광)되도록 할 것인지 여부 값 요청 / 설정
 parm : enable	- [in]  Normal expose mode (0), Use triple exposure mode (1)
 retn : TURE or FALSE
 note : 각각의 이미지를 3번 정도 노출 (노광)되도록 하여, 좀더 부드러운 Edges들이 출력되도록 함
		각 이미지의 가장 자리 부분을 부드럽게 노광할지 여부 설정인듯
*/
API_IMPORT BOOL uvEng_Luria_ReqGetMTCMode();
API_IMPORT BOOL uvEng_Luria_ReqSetMTCMode(UINT8 enable);
/*
 desc : Luria가 AF 수행을 위해서 Photohead의 Z-motion controller와 통신할지 여부 값 요청 / 설정
 parm : enable	- [in]  Disable use of ethercat for AF (0), Use ethercat for AF (1)
 retn : 패킷이 저장된 버퍼 포인트 반환 (호출한 쪽에서 반드시 메모리 해제)
 note : 이 값이 Enable이면, Luria는 AF를 위해 Ethercat 통신을 Enable 시킴
 주의 : 현재 이 명령어 동작 안됨 (Luria 3.0)
*/
API_IMPORT BOOL uvEng_Luria_ReqGetUseEthercatForAF();
API_IMPORT BOOL uvEng_Luria_ReqSetUseEthercatForAF(UINT8 enable);
/*
 desc : Request/Set Spx-level value
 parm : flag	- [in]  Function Type : Get or Set
		level	- [in]  Spex-level to be used. (default is 36)
 retn : Returns the buffer point where the packet is stored (the caller must free the memory)
 note : Must match Spx-Level value processed in preprocess
*/
API_IMPORT BOOL uvEng_Luria_ReqGetSpxLevel();
API_IMPORT BOOL uvEng_Luria_ReqSetSpxLevel(UINT16 level);
/*
 desc : Root Directory - Get / Set
 parm : root	- [in]  Root Directory
		size	- [in]  Size of data stored in 'root_dir' buffer
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetRootDirectory();
API_IMPORT BOOL uvEng_Luria_ReqSetRootDirectory(PTCHAR root, UINT32 size);
/*
 desc : Table Settings 정보 요청 / 설정
 parm : tbl_num	- [in]  table number (1 ~ 2)
		parallel- [in]  Adjustment factor, multiplied by 1000. (Eg. factor 0.97 is given as 970, factor 1.025 is given as 1025).
						Factor 1.0 (1000) means no adjustment, i.e. use the default angle for parallelogram motion.
		y_dir	- [in]  0 = Y axis starts in negative direction, 1 = Y axis starts in positive direction
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetTableSettings(UINT8 tbl_no);
API_IMPORT BOOL uvEng_Luria_ReqSetTableSettings(UINT8 tbl_no, UINT32 parallel, UINT8 y_dir);
/*
 desc : Request Type 1 information from motion controller
 parm : acc_dist	- [in]  Y-axis acceleration/deceleration distance.
							Motion controller Y-direction acceleration/deceleration distance. Given in micrometers
		active_table- [in]  Select which table that is the active table. Note! Valid if MotionControlType = 1 only. Ignored for other types
							1 = table 1, 2 = table 2
		scroll		- [in]  Scroll-mode (1 ~ 4). The hysteresis values below will be used with the corresponding scroll-mode when printing.
		offset		- [in]  Negative offset. In pixels
		pos_dir		- [in]  Delay in positive moving direction. In nanoseconds
		neg_dir		- [in]  Delay in negative moving direction. In nanoseconds
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetMotionType1();
API_IMPORT BOOL uvEng_Luria_ReqSetMotionType1(UINT32 acc_dist, UINT8 active_table);
API_IMPORT BOOL uvEng_Luria_ReqGetMotionType1Hysteresis(UINT8 scroll);
API_IMPORT BOOL uvEng_Luria_ReqSetMotionType1Hysteresis(UINT8 scroll, UINT16 offset,
														UINT32 pos_dir, UINT32 neg_dir);

/* --------------------------------------------------------------------------------------------- */
/*                                            System                                             */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : InitializeHardware
		This command will try to connect to all external devices (photo heads, motion controller, etc) and initialize these
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetHWInit();
/*
 desc : GetSystemStatus
		Query the status of the complete system. If all 0's, then there are no errors.
		If any part report a failure (i.e. one ore more bits set),
		this can only be cleared by running a successful InitializeHardware command again.
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetSystemStatus();
/*
 desc : SystemShutdown
		This command will safely shut down all components of the system and leave the photo heads mirrors in a parked state
		(the LEDs will flash red and yellow alternately to indicate shut-down mode).
		!!! This command should be run before turning off power in the photo heads !!!
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetSystemShutdown();
/*
 desc : UpgradeFirmware
		This command will upgrade all photo heads with the firmware in the given bundle path or firmware file.
 parm : file	- [in]  Path name text string (without null-termination) of bundle or full path and file name of tar.gz file
						(Length of path name text string (S). Max = 200
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetUpgradeFirmware(PTCHAR file);
/*
 desc : LoadInternalTestImage
		This will load the selected test image that is built-in the photo head and display it on the DMD
 parm : ph_no	- [in]  0xff: All Photoheads, 1: PH1, 2: PH2, ...
		img_no	- [in]  Internal test image number (1 ~ 5) (Recommanded : 2 (DMD SPOT))
 retn : TRUE or FALSE
 note : Load only the image file inside, and use the 'SetLightIntensity' command to actually print it
		After executing this command, you must proceed with Hardware Init for normal exposure.
*/
API_IMPORT BOOL uvEng_Luria_ReqSetLoadInternalTestImage(UINT8 ph_no, UINT8 img_no);
/*
 desc : SetLightIntensity
		Use this command to turn on the light in the photo head(s) in order to display
		the internal test image selected using command LoadInternalTestImage.
 parm : ph_no	- [in]  0xff: All Photoheads, 1: PH1, 2: PH2, ...
		led_no	- [in]  LED Number (1:365, 2:385, 3:395, 4:405)
		ampl	- [in]  Power Index of the selected LED (Amplitude Index)
 retn : TRUE or FALSE
 note : Load only the image file inside, and use the 'SetLightIntensity' command to actually print it
		After executing this command, you must proceed with Hardware Init for normal exposure.
*/
API_IMPORT BOOL uvEng_Luria_ReqSetLightIntensity(UINT8 ph_no, ENG_LLPI led_no, UINT16 ampl=0x0000);
API_IMPORT BOOL uvEng_Luria_ReqSetLightIntensityAll(UINT8 ph_no, UINT16 ampl=0x0000);
/*
 desc : GetOverallErrorStatusMulti
		Get the error status for all photo heads. See table below for bit definitions of the error status word
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetOverallErrorStatusMulti();
/*
 desc : Request necessary information regarding the step difference of the photo head
 parm : scroll	- [in]  요청 대상의 Scroll Mode 값 (1 ~ 7)
 retn : NULL or Packet Data
*/
API_IMPORT BOOL uvEng_Luria_ReqGetPhotoheadOffInfo(UINT8 scroll);
/*
 desc : 모션 컨트롤러의 Drive ID 설정
 parm : table_no	- [in]  Work Table Number 1 or 2
		xdrvid		- [in]  The ID to be set for the x-axis in the motion controller.
							Note! Valid if MotionControlType = 1 only. Ignored for other types
		ydrvid		- [in]  The ID to be set for the y-axis in the motion controller. 
 retn : 패킷이 저장된 버퍼 포인트 반환 (호출한 쪽에서 반드시 메모리 해제)
*/
API_IMPORT BOOL uvEng_Luria_ReqGetXYDriveId(UINT8 table_no);
API_IMPORT BOOL uvEng_Luria_ReqSetXYDriveId(UINT8 table_no, UINT8 xdrvid, UINT8 ydrvid);
/*
 desc : 포토 헤드의 사양 정보 설정 (Only Offset)
 parm : ph_no	- [in]  포토 헤드 번호 (2 ~ 8) (1 값 넣으면 실패 처리)
		x_pos	- [in]  X-Position relative to Photohead 1. (1.5.2 이하 : unit: um, 2.0.3 이상 unit: nm)
		y_pos	- [in]  Y-Position relative to Photohead 1. (1.5.2 이하 : unit: um, 2.0.3 이상 unit: nm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetSpecPhotoHeadOffset(UINT8 ph_no, UINT32 x_pos, INT32 y_pos);

/* --------------------------------------------------------------------------------------------- */
/*                                         JobManagement                                         */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : GetPanelDataInfo
		Returns all the panel data information for the given d-code and job.
 parm : dcode	- [in]  선택하고자 하는 D-Code 값
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetPanelDataInfo(UINT32 dcode);
/*
 desc : Assign Job List 설정
 parm : job	- [in]  Job Name (경로 및 파일명)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqAddJobList(PTCHAR job);
/*
 desc : 현재 선택된 Job Name 삭제 (제거. 파일 삭제가 아님)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetDeleteSelectedJob();
/*
 desc : 현재 Job Name을 선택 후 결과 요청
 parm : job_name- [in]  선택하고자 하는 Job Name
		mode	- [in]  0x00: 전체 경로 포함 비교, 0x01: 거버 이름만 비교
 TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSelectedJobName(PTCHAR job_name, UINT8 mode);
/*
 desc : 현재 선택된 Job Name 관련 파라미터 정보 요청
 parm : None
 TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGerberJobParam();
/*
 desc : Set Selected Load Job 설정
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetLoadSelectedJob();
/*
 desc : 임의 Job Name이 저장된 위치 (Index)의 Job 삭제 (제거. 파일 삭제가 아님)
 parm : index	- [in]  삭제 대상의 Job Name이 저장된 위치
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetDeleteJobIndex(UINT8 index);
/*
 desc : 현재 Luria Service에 등록된 모든 거버 (Job) 삭제 (제거. 파일 삭제가 아님)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetDeleteJobAll();
/*
 desc : Job List 요청
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetJobList();
/*
 desc : Job Selected Load State 요청
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetSelectedJobLoadState();
/*
 desc : Request the maximum number of jobs that can be currently registered
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetGetMaxJobs();
/*
 desc : If a read is attempted and no assigned jobs, then “No job” is returned
 parm : flag	- [in]  Get (Read) or Set (Write)
		index	- [in]  Registered Gerber Index you want to select (Zero-based)
		path	- [in]	Path name text string (without null-termination) of selected job
						The path may be relative or absolute. A trailing backslash (\) is optional
		size	- [in]  The size of 'path' buffer
		mode	- [in]  0x00: Compare the entire path, 0x01: Compare only the gerber name
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetSelectedJob();
API_IMPORT BOOL uvEng_Luria_ReqSetSelectedJob(PCHAR path, UINT32 size);
API_IMPORT BOOL uvEng_Luria_ReqSetSelectedJobIndex(UINT8 index=0);
API_IMPORT BOOL uvEng_Luria_ReqSetSelectedJobExt(PCHAR path, UINT8 mode);

/* --------------------------------------------------------------------------------------------- */
/*                                        PanelPreparation                                       */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Get or Set - Registration (Points & Global & Local)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetRegistration();
/*
 desc : Get or Set - Global Transformation Recipe
 parm : rotation- [in]  Rotating	0x00 - Auto, 0x01 = Fixed
		scaling	- [in]  Scaling		0x00 - Auto, 0x01 = Fixed
		offset	- [in]  Offset		0x00 - Auto, 0x01 = Fixed
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetGlobalTransformationRecipe();
API_IMPORT BOOL uvEng_Luria_ReqSetGlobalTransformationRecipe(UINT8 rotation,
															 UINT8 scaling,
															 UINT8 offset);
/*
 desc : Fixed Rotation, Fixed Scaling, Fixed Offset 요청 / 설정
 parm : rotation- [in]  Global fixed rotation, in degrees. Ignored for TransformationReceipe 0.
		scale_x	- [in]  Global fixed x-scaling factor. Ignored for TransformationReceipe 0.
		scale_y	- [in]  Global fixed x-scaling factor. Ignored for TransformationReceipe 0.
		offset_x- [in]  Global fixed x-offset. In mm. Ignored for TransformationReceipe 0.
		offset_x- [in]  Global fixed y-offset. In mm. Ignored for TransformationReceipe 0.
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetGlobalFixed();
API_IMPORT BOOL uvEng_Luria_ReqSetGlobalFixed(DOUBLE rotation, DOUBLE scale_x, DOUBLE scale_y,
											  DOUBLE offset_x, DOUBLE offset_y);
API_IMPORT BOOL uvEng_Luria_ReqGetGlobalFixedRotation();
API_IMPORT BOOL uvEng_Luria_ReqSetGlobalFixedRotation(INT32 rotation);
API_IMPORT BOOL uvEng_Luria_ReqGetGlobalFixedScaling();
API_IMPORT BOOL uvEng_Luria_ReqSetGlobalFixedScaling(UINT32 scale_x, UINT32 scale_y);
API_IMPORT BOOL uvEng_Luria_ReqGetGlobalFixedOffset();
API_IMPORT BOOL uvEng_Luria_ReqSetGlobalFixedOffset(INT32 offset_x, INT32 offset_y);
/*
 desc : Get or Set - Registration (Registration Points)
 parm : p_count	- [in]  Registration Points Count
		p_fidxy	- [in]  X/Y 좌표가 저장된 배열 포인터[xx][2] (단위 : 1000000)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetRegistrationPoints(UINT16 p_count, LPG_I32XY p_fidxy);
API_IMPORT BOOL uvEng_Luria_ReqSetRunRegistration();
API_IMPORT BOOL uvEng_Luria_ReqSetRegistPointsAndRun(UINT16 p_count, LPG_I32XY p_fidxy);
API_IMPORT BOOL uvEng_Luria_ReqSetRegistrationPointsEx(UINT16 p_count, STG_I32XY **p_fidxy);	/* C# only */
/*
 desc : Get Only - Get Registration Status
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetGetRegistrationStatus();
/*
 desc : Serial Information 요청 / 설정
 parm : dcode	- [in]  D-Code (Panel Dcode List 값 중 1개)
		flip_h	- [in]  Horizontal Flip (1: Flip, 0: No Flip)
		flip_v	- [in]  Vertical Flip (1: Flip, 0: No Flip)
		font_h	- [in]  font size (Horizontal) (unit : um)
		font_v	- [in]  font size (Vertical) (unit : um)
		symbol	- [in]  Symbol marking number to be incremented (e.g. '#').
		start	- [in]  Starting value of number to be incremented.
		s_len	- [in]  Length of string (S). Max = 32.
		serial	- [in]  Serial number string
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetSerialNumber(UINT32 dcode);
API_IMPORT BOOL uvEng_Luria_ReqSetSerialNumber(UINT32 dcode,
											   UINT8 symbol, UINT16 s_len, PUINT8 serial, UINT32 start,
											   UINT8 flip_x, UINT8 flip_y, UINT16 font_h, UINT16 font_v);
/*
 desc : Panel Data 요청 / 설정
 parm : dcode	- [in]  D-Code (Panel Dcode List 값 중 1개)
		flip_x	- [in]  X-Flip. 1 = flip, 0 = no flip
		flip_y	- [in]  Y-Flip. 1 = flip, 0 = no flip
		font_h	- [in]  font size (Horizontal) (unit : um)
		font_v	- [in]  font size (Vertical) (unit : um)
		s_len	- [in]  Length of string (S). (Only valid for GeneralText, should be 0 in other cases). Max = 32.
		text	- [in]  Text string
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetPanelData(UINT32 dcode);
API_IMPORT BOOL uvEng_Luria_ReqSetPanelData(UINT32 dcode, UINT16 s_len, PUINT8 text, UINT8 flip_x,
											UINT8 flip_y, UINT16 font_h, UINT16 font_v);
/*
 desc : When global auto transformation recipe is selected,
		it is possible to force the registration points to make a rectangle.
 parm : lock	- [in]  Enable (0x01) or Disable (0x00) Rectangle
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetGlobalRectangle();
API_IMPORT BOOL uvEng_Luria_ReqSetGlobalRectangle(UINT8 lock);
/*
 desc : 가장 근접한 Zone Registration 설정으로 Warped 여부 요청 / 설정
 parm : flag	- [in]  Function Type : Get or Set
		enable	- [in]  0 = Disable, 1 = Enable
						이 모드를 활성화하면 Artwork는 항상 가장 가까운 Zone Registration 설정으로
						Wraped 시킵니다. (non-shared local zones에서 조합으로 사용될 수 있음)
 retn : 패킷이 저장된 버퍼 포인트 반환 (호출한 쪽에서 반드시 메모리 해제)
*/
API_IMPORT BOOL uvEng_Luria_ReqGetSnapToZoneMode();
API_IMPORT BOOL uvEng_Luria_ReqSetSnapToZoneMode(UINT8 enable);
/*
 desc : Shared Local Zone 사용 여부
 parm : flag	- [in]  Function Type : Get or Set
		enable	- [in]  0 = Disable (Do not use shared zones), 1 = Enable (Use shared zones)
 retn : 패킷이 저장된 버퍼 포인트 반환 (호출한 쪽에서 반드시 메모리 해제)
*/
API_IMPORT BOOL uvEng_Luria_ReqGetUseSharedLocalZones();
API_IMPORT BOOL uvEng_Luria_ReqSetUseSharedLocalZones(UINT8 enable);

/*
 desc : 현재 등록 포인트와 변환 레시피를 기반으로 변환 매개변수
 parm : zone	- [in]  Zone number ( 0 = Global, 1 and above: local zone numbers)
 retn : Pointer where the packet buffer is stored
*/
API_IMPORT BOOL uvEng_Luria_ReqGetGetTransformationParams(UINT16 zone);

/* --------------------------------------------------------------------------------------------- */
/*                                            Expose                                             */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Exposure State
		Get the current exposure state. If state is “PrintRunning”,
		then information about the current motion is given.
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetExposureState();
/*
 desc : EnableAfCopyLastStrip
		When function is enabled, the photo head printing the last strip will copy the autofocus profile
		of the second to last strip and use that profile for focusing the last strip.
 parm : enable	- [in]  1: Enable AF copy function. 0: Disable (default)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetEnableAfCopyLastStrip();
API_IMPORT BOOL uvEng_Luria_ReqSetEnableAfCopyLastStrip(UINT8 enable);
/*
 desc : DisableAfFirstStrip
		Disable autofocus for the first strip for the first photohead
 parm : disable	- [in]  1: Disable autofocus first strip,
						0: Do not disable autofocus first strip (default).
 retn : TRUE or FALSE
 note : This command makes it possible to turn off autofocus for the first strip of a print,
		for example if this strip is outside the panel.
		If autofocus is disabled in the photo head, then this function is bypassed.
*/
API_IMPORT BOOL uvEng_Luria_ReqGetDisableAfFirstStrip();
API_IMPORT BOOL uvEng_Luria_ReqSetDisableAfFirstStrip(UINT8 disable);
/*
 desc : Get or Set	- Scroll Step Size / Frame Rate Factor / Led Duty Cycle
 parm : step		- [in]  1 = Step size 1, 2 = Step size 2, etc
		duty		- [in]  LED duty cycle. Range 0 ? 100%
		frame_rate	- [in]  Set frame rate factor. 0 < FrameRateFactor/1000 <= 1, ex. range is 1 ? 1000.
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetExposureFactor();
API_IMPORT BOOL uvEng_Luria_ReqSetExposureFactor(UINT8 step, UINT8 duty, DOUBLE frame_rate);
/*
 desc : Set - LED Amplitude (When setting only one photohead ...) (Used in expose)
 parm : ph_no	- [in]  Photohead Number (0x01 ~ 0x08)
		led_no	- [in]  LED Number (1:365, 2:385, 3:395, 4:405)
		amp_idx	- [in]	Led Amplitude
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetLedAmplitudeOne(UINT8 ph_no, ENG_LLPI led_no, UINT16 amp_idx);
/*
 desc : Set - LED Amplitude (Used in expose)
 parm : count	- [in]  Photohead Count
		amp		- [in]	Led Amplitude (2차원 배열 포인터. 각 차원마다 4개씩 존재)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetLedAmplitude(UINT8 count, UINT16 (*amp_idx)[4]);
/*
 desc : Get - LED Amplitude (Used in expose)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetLedAmplitude();
/*
 desc : Setting the exposure start position / requesting the previously set position
 parm : tbl_num	- [in]  table number (1 ~ 2)
		start_x	- [in]  X - position table exposure start position. (unit: mm)
		start_y	- [in]  Y - position table exposure start position. (unit: mm)
		reply	- [in]  Whether to request a set exposure start position
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetTableExposureStartPos(UINT8 tbl_no);
API_IMPORT BOOL uvEng_Luria_ReqSetTableExposureStartPos(UINT8 tbl_no,
														DOUBLE start_x, DOUBLE start_y,
														BOOL reply=FALSE);
/*
 desc : Print 관련 Option
 parm : flag	- [in]  Print 관련 명령어
						0 : Pre-Print
						1 : Print
						2 : Simulation Print
						3 : Abort
						4 : Reset Exposure State
						5 : Move Start Position
						6 : Light Ramp Up/Down
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetPrintOpt(UINT8 mode);
/*
 desc : Only Set - Move to Start the position for printing
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetPrintStartMove();
/*
 desc : Only Set - This command should be sent to Luria if the current light amplitude is
		significantly higher/lower than the amplitude used in the previous print.
		The command results in a short light-output from the photo heads
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetPrintRampUpDown();
/*
 desc : Request/Set AF Sensor Type and Setting value
 parm : type	- [in]  AF Sensor Type. 1 = Diffuse (default): diffused light, 2 = Specular: reflected light
		agc		- [in]  Laser AGC (automatic gain control) setting 값
						1 = AGC on (default), 0 = AGC off (value in PWM-field is used)
		pwm		- [in]  If the 'agc' value is 0, it is set to this value (range:2 ~ 511. default=100)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetAFSensor();
API_IMPORT BOOL uvEng_Luria_ReqSetAFSensor(UINT8 type, UINT8 agc, UINT16 pwm);
/*
 desc : Request the optimal laser PWM value
 parm : None
 retn : TRUE or FALSE
 note : Measured values are stored in the current job in Luria. If a new job is loaded, it must get a new measurement value
		AFSensor is used as the value measured with Laser AGC setting 2 (Command: AFSensor (0x11))
		However, these values are applied only when Autofocus is used.
*/
API_IMPORT BOOL uvEng_Luria_ReqGetAFSensorMeasureLaserPWM();
/*
 desc : Number of available LEDs installed per photo head and request/setting of Source Type
 parm : ph_no	- [in]  Photohead Number (1 based)
 retn : TRUE or FALSE
 note : Light Source Type (0: LED, 1: Laser, 2: LED Combiner)
*/
API_IMPORT BOOL uvEng_Luria_GetPktNumberOfLightSource(UINT8 ph_no);
/*
 desc : Request LED status values within the entire Photohead
 parm : None
 retn : TRUE or FALSE
 note : None (Received values vary depending on the type of optical mainboard.)
		Distinguish between Luxbeam 9500/4800 (32-bit) and Lurxbeam4700 (16-bit)
*/
API_IMPORT BOOL uvEng_Luria_GetPktLightSourceStatusMulti();
/*
 desc : Only Set - Print Abort
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetPrintAbort();

/* --------------------------------------------------------------------------------------------- */
/*                                        DirectPhotohead                                        */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : LED Driver Amplitude. That is, Records used to manipulate the amplitude in the Light Source drivers
		(It is different from the LedAmplitude function. The purpose of this function is to use th SpotMode)
		(Used in direct photohead)
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08, All : 0xff)
		led_no	- [in]  LED Number (1:365, 2:385, 3:395, 4:405)
		amp_idx	- [in]  The amplitude to set for the selected Light Source (Range: 0 ~ 4095)
 retn : TRUE or FALSE
 note : It does not work normally (uvEng_Luria_ReqSetLedPower)
*/
API_IMPORT BOOL uvEng_Luria_ReqSetLedPower(UINT8 ph_no, ENG_LLPI led_no, UINT16 amp_idx);
API_IMPORT BOOL uvEng_Luria_ReqGetLedPower(UINT8 ph_no, ENG_LLPI led_no);
API_IMPORT BOOL uvEng_Luria_ReqGetLedPowerLedAll(ENG_LLPI led_no);
API_IMPORT BOOL uvEng_Luria_ReqSetLedPowerResetAll();
/*
 desc : Records that requests status of the Light Source drivers
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		led_no	- [in]  LED Number (1:365, 2:385, 3:395, 4:405)
 retn : TRUE or FALSE
 note : Note that all errors/warnings will be retained in the status word
		until it is actively cleared, using command <<Clear Error Status>>
*/
API_IMPORT BOOL uvEng_Luria_ReqGetLedSourceDriverStatus(UINT8 ph_no, ENG_LLPI led_no);
/*
 desc : 모든 Photohead의 Z 축 모터의 현재 상태 요청
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetMotorStateAll();
/*
 desc : Initialize the z-axis of all photoheads (moved to center position)
		(However, this applies only to the photohead whose motor is not initialized)
 parm : ph_no	- [in]  Photohead Number (1 ~ 8)
		action	- [in]  0x01 : moved to center position
						0x00 : not moved to center position
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetMotorPositionInit(UINT8 ph_no, UINT8 action=0x01);
/*
 desc : 현재 포토헤드에 적재되어 있는 내부 이미지 번호 요청 or 설정
 parm : ph_no	- [in]  Photohead Number (1 based)
		img_no	- [in]  포토 헤드에 적재될 내부 이미지 번호 (0x00 ~ 0x04)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetLoadInternalImage(UINT8 ph_no);
API_IMPORT BOOL uvEng_Luria_ReqSetLoadInternalImage(UINT8 ph_no, UINT8 img_no);
/*
 desc : Enable Flatness Correction Mask (PPC) 요청 or 설정
 parm : ph_no	- [in]  포토 헤드 번호 (0x01 ~ 0x08 or 0xff)
		enable	- [in]  0x00 : Spot Mode 사용, 0x01 - Expo Mode 사용
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetFlatnessMaskOn(UINT8 ph_no);
API_IMPORT BOOL uvEng_Luria_ReqSetFlatnessMaskOn(UINT8 ph_no, UINT8 enable);
/*
 desc : 사용하려는 시퀀스 파일 선택 (외부 or 내부 시퀀스 파일. 내부인 경우 0 ~ 4 번중 하나 선택됨)
 parm : ph_no	- [in]  Photohead Number (1 based)
		type	- [in]  0x00 : Use last loaded (external) sequence file
						0x01 : Use internal sequence file
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetActiveSequence(UINT8 ph_no, UINT8 type);
API_IMPORT BOOL uvEng_Luria_ReqGetActiveSequence(UINT8 ph_no);
/*
 desc : This record will turn on/off the shaking function of the DMD mirrors.
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		enable	- [in]  0 : DMD mirror shake OFF, 1 : DMD mirror shake ON
 retn : TRUE or FALSE
 note : This function requires that the internal sequencer is running
*/
API_IMPORT BOOL uvEng_Luria_ReqSetDmdMirrorShake(UINT8 ph_no, UINT8 enable);
API_IMPORT BOOL uvEng_Luria_ReqGetDmdMirrorShake(UINT8 ph_no);
/*
 desc : This record manipulates the duration of the light pulse that is applied when a «LightPulseWord» command
		is used in the sequence file.
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		value	- [uin16]  The duration the light pulse should be on.
 retn : Pointer where the packet buffer is stored
 note : This function requires that the internal sequencer is running
*/
API_IMPORT BOOL uvEng_Luria_ReqSetLightPulseDuration(UINT8 ph_no, UINT16 value);
API_IMPORT BOOL uvEng_Luria_ReqGetLightPluseDuration(UINT8 ph_no);
/*
 desc : Light Source Driver Light On/Off
 parm : ph_no	- [in]  Photohead Number (1 based)
		led_no	- [in]  LED Number (1:365, 2:385, 3:395, 4:405)
		onoff	- [in]  0x00 : Light Off, 0x01 : Light ON
 retn : TRUE or FALSE
 note : It doesn't work normally. Please refrain from using it.
*/
API_IMPORT BOOL uvEng_Luria_ReqSetLedLightOnOff(UINT8 ph_no, ENG_LLPI led_no, UINT8 onoff);
API_IMPORT BOOL uvEng_Luria_ReqGetLedLightOnOff(UINT8 ph_no, ENG_LLPI led_no);
API_IMPORT BOOL uvEng_Luria_ReqSetLedLightOnOffAll(UINT8 onoff);
API_IMPORT BOOL uvEng_Luria_ReqGetLedLightAll();
/*
 desc : Sequence State (Halted or Running)
 parm : ph_no	- [in]  Photohead Number (1 based)
		seq_cmd	- [in]  Command to send/request (1 : run, 2 : reset, 3 : 3 Unused)
		enable	- [in]  1 - enable, 0 - disable
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetSequnceState(UINT8 ph_no, UINT8 seq_cmd, UINT8 enable);
API_IMPORT BOOL uvEng_Luria_ReqGetSequnceState(UINT8 ph_no, UINT8 seq_cmd);
/*
 desc : 특정 Photohead에 대한 Line Sensor Data의 Plotting을 Enable or Disable
 parm : ph_no	- [in]  포토 헤드 번호 (0x01 ~ 0x08)
		enable	- [in]  enable (0x01) or Disable (0x00)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetLineSensorPlot(UINT8 ph_no, UINT8 enable);
API_IMPORT BOOL uvEng_Luria_ReqGetLineSensorPlot(UINT8 ph_no);
/*
 desc : 현재 광학계의 Autofocus Position 확인
 parm : ph_no	- [in]  포토 헤드 번호 (0x01 ~ 0x08)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetCurrentAutofocusPosition(UINT8 ph_no);
/*
 desc : Luria Server에 Photohead LED On-Time 값 요청
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetPhLedOnTimeAll();
/*
 desc : MainboardTemperature
		This will give the temperature of the mainboard for each photo head
		Note that the number must be divided by 10 to get the correct temperature
 parm : None
 retn : TRUE or FALSE
 note : 
		If the temperature raises above approx <<55 deg.>> Celsius,
		then this may indicate that the cooling is not working properly
*/
API_IMPORT BOOL uvEng_Luria_ReqGetPhLedTempAll();
/*
 desc : Luria Server에 Photohead LED Temperature 값 요청 - 전체 Photohead 중 주파수 별 값 요청
 parm : freq_no	- [in]  전체 Photohead의 주파주 번호 (0:365, 1:385, 2:395, 4:405)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetPhLedTempFreqAll(UINT8 freq_no);
/*
 desc : 모든 Photohead 에 발생된 Error Clear 처리
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetClearErrorStatus();

/* --------------------------------------------------------------------------------------------- */
/*                                      LuriaComManagement                                       */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Only Get - Announcement Status
 parm : None
 retn : TRUE or FALSE
 note : 3회 연속 공지를 보낼 수 없는 경우 Luria가 공지를 자동으로 비활성화 하며
		이 기능을 다시 활성화하는 유일한 방법은 "GenerateTestAnnouncement"을 성공적으로 보내야 함
		AlignmentServerIpAddr가 0.0.0.0이면 상태는 "Disabled" 임
*/
API_IMPORT BOOL uvEng_Luria_ReqGetAnnouncementStatus();

/* --------------------------------------------------------------------------------------------- */
/*                                         Announcement                                          */
/* --------------------------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------------------------- */
/*                                            Focus                                              */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Focus Motor Move Absolute Position (Move to set position)
 parm : ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		pos		- [in]  Absolute move position value (unit: mm) (rangle: 64.000~84.000)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetMotorAbsPosition(UINT8 ph_no, DOUBLE pos);
/*
 desc : Focus Motor Move Absolute Position (Move to set position)
 parm : count	- [in]  Photohead Count
		pos		- [in]  Buffer in which absolute positions are stored as many as 'count'
						(unit: mm) (rangle: 64.000~84.000)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetMotorAbsPositionAll(UINT8 count, PDOUBLE pos);
/*
 desc : 모든 Photohead의 Z 축 모터의 위치 초기화 (Homing) (중간 위치로 이동) 혹은 멈춤
		단, 모터가 초기화 안된 Photohead만 초기화 수행
 parm : action	- [in]  0x01 : 중간 위치로 이동
						0x00 : 중간 위치로 이동하는 것을 멈춤
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetMotorPositionInitAll(UINT8 action=0x01);
/*
 desc : Focus Motor Move Absolute Position (현재 위치 얻기)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetMotorAbsPositionAll();
/*
 desc : Focus Motor Move Absolute Position (모든 Photohead의 Z Axis을 기본 위치로 이동)
 parm : ph_no	- [in]  Photohead Number (1 based)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetAllMotorBaseAbsPosition();
/*
 desc : Initialize Focus (Write only) (This will initialize the z-axis) (== Homing 효과)
 parm : None
 retn : TRUE or FALSE
 note : The z-axis must be initialized in order to use any of the below focus functions.
		Note that this may take up to 30 seconds to complete.
		If initialize fails, the status code FocusMotorOperationFailed is given.
*/
API_IMPORT BOOL uvEng_Luria_ReqSetInitializeFocusLM();
/*
 desc : 임의 Photohead Z Axis의 높이 값 설정 or 요청 (unit: mm)
 parm : ph_no	- [in]  Photo head number of first axis to set position to (1 = ph #1, 2 = ph # 2, etc)
		pos		- [in]  Z-position given in micrometers for given photo head (unit : mm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetPositionLM(UINT8 ph_no, DOUBLE pos);
API_IMPORT BOOL uvEng_Luria_ReqSetPositionAllLM(UINT8 count, PDOUBLE pos);
API_IMPORT BOOL uvEng_Luria_ReqGetPositionLM(UINT8 ph_no);
/*
 desc : Start / Stop (Enable / Disable) photo head auto focus system
 parm : ph_no	- [in]  Photo head number (0xff = All ph, 1 = ph #1, 2 = ph # 2, etc)
		enable	- [in]  0x01 : Enable auto focus, 0x00 = Disable Auto focus.
 retn : 패킷 버퍼
*/
API_IMPORT BOOL uvEng_Luria_ReqSetAutoFocusLM(UINT8 ph_no, UINT8 enable);
API_IMPORT BOOL uvEng_Luria_ReqGetAutoFocus(UINT8 ph_no);
/*
 desc : Autofocus set-point trim value
 parm : ph_no	- [in]  Photo head number (1 = ph #1, 2 = ph # 2, etc)
		trim	- [in]  Focus trim (unit: mm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetTrimLM(UINT8 ph_no, DOUBLE trim);
API_IMPORT BOOL uvEng_Luria_ReqGetTrim(UINT8 ph_no);
/*
 desc : Autofocus set-point trim value
 parm : enable	- [in]  0x01: Enable, 0x00: Disable
		start	- [in]  Distance from exposure <start> where AF is inactive (mm)
		end		- [in]  Distance from exposure <end> where AF is inactive (mm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetActiveAreaQualifierLM(UINT8 enable, DOUBLE start, DOUBLE end);
API_IMPORT BOOL uvEng_Luria_ReqGetActiveAreaQualifier();

/*
 desc : Autofocus set-point trim value
 parm : ph_no	- [in]  Photo head number (0xff:All ph, 0x01:ph #1, 0x02:ph # 2, etc)
		abs_min	- [in]  The min absolute focus position that the AF will operate within (unit: mm)
		abs_max	- [in]  The min absolute focus position that the AF will operate within (unit: mm)
 retn : TRUE or FALSE
 note : Note that setting AbsWorkRange will clear the AbsWorkRangeStatus for the given photo head(s).
*/
API_IMPORT BOOL uvEng_Luria_ReqSetAbsWorkRangeLM(UINT8 ph_no, DOUBLE abs_min, DOUBLE abs_max);
API_IMPORT BOOL uvEng_Luria_ReqGetAbsWorkRange(UINT8 ph_no);

/*
 desc : return the status word giving status of autofocus absolute work range
 parm : None
 retn : TRUE or FALSE
 note : The status-bit for a given photo head will be cleared next time a new AbsWorkRange has been set
		!!! AbsWorkRange with photo head number = 0 (all photo heads), then all status bits will be cleared !!!
*/
API_IMPORT BOOL uvEng_Luria_ReqGetAbsWorkRangeStatusLM();
/*
 desc : Autofocus Edge Trigger
 parm : dist	- [in]  The max distance below or above current AF set-point that AF will be active (unit: mm)
		enable	- [in]  Enable or disable the edge trigger function. 1 = enable, 0 = disable
 retn : TRUE or FALSE
 note : Note that setting AbsWorkRange will clear the AbsWorkRangeStatus for the given photo head(s).
*/
API_IMPORT BOOL uvEng_Luria_ReqSetAfEdgeTriggerLM(DOUBLE dist, UINT8 enable);
API_IMPORT BOOL uvEng_Luria_ReqGetAfEdgeTrigger();
/*
 desc : Set height values for all Photohead Z Axis (unit: mm)
 parm : count	- [in]  Number (N) of photo heads to send position to
		ph_no	- [in]  Photo head number of first axis to set position to (1 = ph #1, 2 = ph # 2, etc)
		pos		- [in]  Z-position given in micrometers for given photo head (unit: mm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetPositionMultiLM(UINT8 count, PUINT8 ph_no, PDOUBLE pos);
/*
 desc : Fine tuning of arbitrary Photohead Z Axis (unit: mm)
 parm : flag	- [in]  Function Type : Get or Set
		ph_no	- [in]  Photo head number of first axis to set position to (1 = ph #1, 2 = ph # 2, etc)
		pos		- [in]  Z-position given in nanometers (unit: mm)
 retn : TRUE or FALSE
 note : Linear motor rounds to nearest 100nm, while stepper motor rounds to nearest um
*/
API_IMPORT BOOL uvEng_Luria_ReqSetPositionHiResLM(UINT8 ph_no, DOUBLE pos);
API_IMPORT BOOL uvEng_Luria_ReqGetPositionHiResLM(UINT8 ph_no);
/*
 desc : Reports the number of trigger pulses that autofocus position has been outside the given DOF (MachineConfig:DepthOfFocus)
 parm : None
 retn : TRUE or FALSE
 note : The counter is reset when this command is finished,
		so the number reported is the number of trigger pulses outside DOF since last read.
		This is valid for linear z-motor only.
		If trying to read this number when connected to a stepper z-motor, then 0 will be returned.
*/
API_IMPORT BOOL uvEng_Luria_ReqGetOutsideDOFStatusLM();
/*
 desc : Get focus position for all photo heads simultaneously
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetPositionMultiLM();
/*
 desc : Autofocus Gain
 parm : flag	- [in]  Function Type : Get or Set
		gain	- [in]  Gain/damping factor. (Valid range 1 - 4095)
 retn : TRUE or FALSE
 note : This will set the autofocus gain (damping factor) in the photo heads
		The default gain value is 10.
*/
API_IMPORT BOOL uvEng_Luria_ReqSetAfGainLM(UINT16 gain);
API_IMPORT BOOL uvEng_Luria_ReqGetAfGain();

/* --------------------------------------------------------------------------------------------- */
/*                                         < for Group >                                         */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : All Photoheads - Get / Set / Check
 parm : total_ph	- [in]  Total photo heads (1 ~ 8)
		ph_pitch	- [in]  Photo head pitch
		ph_rotated	- [in]  0:No, 1:Rotated
		ctrl_type	- [in]  Motion Controller (1:Sieb & Meyer, 2 = Newport)
		pid			- [in]  Product ID (ENG_LPPI)
		zdrv_type	- [in]  Z drive type (1:Stepper motor, 2:Linear drive from ACS, 3:Linear drive from Sieb&Meyer)
		mtc_mode	- [in]  0:Normal exposure mode, 1:MTC (Machine Tolerance Compensation) mode
		spx_level	- [in]  Default is 36
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetGroupAllPhotoheadsStatic();
API_IMPORT BOOL uvEng_Luria_ReqSetGroupAllPhotoheadsStatic(UINT8 total_ph, UINT8 ph_pitch, UINT8 ph_rotated,
														   UINT8 ctrl_type, UINT32 pid, UINT8 zdrv_type,
														   UINT8 mtc_mode, UINT16 spx_level);
/*
 desc : All Photoheads - Get / Set / Check
 parm : scroll_rate	- [in]  The maximum scroll rate of the system. (Default 19000)
		use_ethercat- [in]  0 = Disable use of ethercat for AF, 1 = Use ethercat for AF
		use_pressure- [in]  1 = Over pressure mode ON, 0 = Over pressure mode OFF
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetGroupAllPhotoheadsJobIndependent();
API_IMPORT BOOL uvEng_Luria_ReqSetGroupAllPhotoheadsJobIndependent(UINT16 scroll_rate, UINT8 ethercat, UINT8 pressure);
/*
 desc : Specific photohead - Get / Set
 parm : ph_no	- [in]  Photo head number (1 to 8)
		ipv4	- [in]  IP Address consisting of 4 bytes
		x_pos	- [in]  X-Position relative to Photohead 1. (unit: nm)
		y_pos	- [in]  Y-Position relative to Photohead 1. (unit: nm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetGroupSpecificPhotohead(UINT8 ph_no);
API_IMPORT BOOL uvEng_Luria_ReqSetGroupSpecificPhotohead(UINT8 ph_no, PUINT8 ipv4, UINT32 x_pos, INT32 y_pos);
/*
 desc : Motion Controller - Get / Set
 parm : ipv4		- [in]  Motion Controller IP Address
		max_y_speed	- [in]  Max Y Motion Speed (unit: um/sec)
		x_speed		- [in]  Max X Motion Speed (unit: um/sec)
							The motion speed to be used for x-movements (and to move to print start position)
		y_acc_dist	- [in]  Y-axis acceleration/deceleration distance. (unit: um)
							Note! Valid if MotionControlType = 1 only. Ignored for other types
		t1_x_drv_id	- [in]  ID number of the x-motion controller for the table 1
		t1_y_drv_id	- [in]  ID number of the y-motion controller for the table 1
		t2_x_drv_id	- [in]  ID number of the x-motion controller for the table 2
		t2_y_drv_id	- [in]  ID number of the y-motion controller for the table 2
		scroll_mode	- [in]  Scroll-mode (1 – 4). The hysteresis values below will be used with the corresponding scroll-mode when printing
		nega_offset	- [in]  Negative offset. Given in number of trigger pulses.
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetGroupMotionController(UINT8 scroll_mode);
API_IMPORT BOOL uvEng_Luria_ReqSetGroupMotionController(PUINT8 ipv4, UINT32 max_y_speed, UINT32 x_speed,
														UINT32 y_acc_dist,
														UINT8 t1_x_drv_id, UINT8 t1_y_drv_id,
														UINT8 t2_x_drv_id, UINT8 t2_y_drv_id,
														UINT8 scroll_mode,
														UINT16 nega_offset,
														UINT32 delay_posi, UINT32 delay_nega);
/*
 desc : Table Settings - Get / Set
 parm : tbl_no		- [in]  Table number (1 or 2)
		parallel	- [in]  Adjustment factor, multiplied by 1000. (Eg. factor 0.97 is given as 970, factor 1.025 is given as 1025).
							Factor 1.0 (1000) means no adjustment, i.e. use the default angle for parallelogram motion.
		print_y_dir	- [in]  0 = Y axis starts in negative direction, 1 = Y axis starts in positive direction
		tbl_min_x	- [in]  Minimum absolute x-position of table. (unit : um)
		tbl_min_y	- [in]  Minimum absolute y-position of table. (unit : um)
		tbl_max_x	- [in]  Maximum absolute x-position of table. (unit : um)
		tbl_max_y	- [in]  Maximum absolute y-position of table. (unit : um)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqGetGroupTableSettings(UINT8 tbl_no);
API_IMPORT BOOL uvEng_Luria_ReqSetGroupTableSettings(UINT8 tbl_no, UINT32 parallel, UINT8 print_y_dir,
													 UINT32 start_x, UINT32 start_y,
													 INT32 tbl_min_x, INT32 tbl_min_y,
													 INT32 tbl_max_x, INT32 tbl_max_y);
/*
 desc : Set whether to enter or exit the LED Amplitude mode
 parm : enable	- [in]  TRUE : Enable the LED Amplitude mode, FALSE : Disable the LED Amplitude mode
		ph_no	- [in]  Photohead number (0x01 ~ 0x08 or 0xff : All)
		led_no	- [in]  LED Number (1:365, 2:385, 3:395, 4:405)
		ampl	- [in]  LED Amplitude Index (0 ~ 4095)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_ReqSetLedPowerSpotRun(UINT8 ph_no, ENG_LLPI led_no, UINT16 ampl);
API_IMPORT BOOL uvEng_Luria_ReqSetLedPowerSpotStop(UINT8 ph_no, ENG_LLPI led_no);
API_IMPORT BOOL uvEng_Luria_ReqSetLedPowerSpotStopAll();
API_IMPORT BOOL uvEng_Luria_ReqGetLedPowerSpotRun(UINT8 ph_no, ENG_LLPI led_no);
API_IMPORT BOOL uvEng_Luria_ReqGetLedPowerSpotStopAll();
/*
 desc : Check whether it is in SPOT mode
 parm : ph_no	- [in]  Photohead Number (1 based)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_IsLedPowerSpotMode(UINT8 ph_no);
API_IMPORT BOOL uvEng_Luria_IsLedPowerSpotModeAll();

/* --------------------------------------------------------------------------------------------- */
/*                                            Common                                             */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 기존 연결을 해제하고, 새로 접속을 수행
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_Reconnected();
/*
 desc : Luria Service (광학계)에 연결되고 난 이후, 초기화 명령들이 모두 송/수신 완료되었는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_IsServiceInited();
/*
 desc : Verify that the send buffer is full
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_IsSendBuffFull();
/*
 desc : Returns the empty rate of the send buffer
 parm : None
 retn : percentage (unit: %)
*/
API_IMPORT FLOAT uvEng_Luria_GetSendBuffEmptyRate();
/*
 desc : Returns the frequency value corresponding to the LED number
 parm : led_no	- [in]  LED Number (1:365, 2:385, 3:395, 4:405)
 retn : Frequency (365nm, 385nm, 395nm, 405nm)
*/
API_IMPORT UINT16 uvEng_Luria_GetLedNoToFreq(UINT8 led_no);
/*
 desc : 현재 제어SW의 작업 (Sequence)이 동작 중인지 여부 (Luria에게 통신 부하를 줄이기 위함)
 parm : flag	- [in]  TRUE or FALSE
 retn : None
*/
API_IMPORT VOID uvEng_Luria_SetWorkBusy(BOOL flag);
/*
 desc : 패킷 수신 여부
 parm : flag	- [in]  패킷 구분 명령어 (Family ID와 User ID가 병합된 코드 값)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_IsRecvPktData(ENG_FDPR flag);
/*
 desc : 가장 마지막 (최근)에 수신된 명령어 응답 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_IsLastRecvCmd();
/*
 desc : Luria Server에 연결 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsConnected();
/*
 desc : 가장 최근에 발생된 Luria Error Code (Status) 값 반환
 parm : None
 retn : Error Code 값
*/
API_IMPORT UINT16 uvCmn_Luria_GetLastErrorStatus();


API_IMPORT LPG_LDSM uvCmn_Luria_GetShMem();

/*
 desc : 가장 최근에 발생된 에러 코드에 대한 메시지 출력
 parm : mesg	- [in]  Return buffer
		size	- [in]  'mesg' buffer size
 retn : None
*/
API_IMPORT VOID uvCmn_Luria_GetLuriaLastErrorDesc(PTCHAR mesg, UINT32 size);
/*
 desc : Luria Status 코드와 매핑되는 에러 문자열 (설명) 반환
 parm : code	- [in]  Luria Status (Error) Code
		mesg	- [in]  Return buffer
		size	- [in]  'mesg' buffer size
 retn : None
*/
API_IMPORT VOID uvCmn_Luria_GetLuriaStatusToDesc(UINT16 code, PTCHAR mesg, UINT32 size);
/*
 desc : 현재 Luria에 발생된 에러가 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsLastError();
/*
 desc : 가장 최근에 발생된 Luria Error Code (Status) 초기화
 parm : None
 retn : None
*/
API_IMPORT VOID uvCmn_Luria_ResetLastErrorStatus();
/*
 desc : 광학계 Z 축의 이동 범위 값이 유효한지 여부
 parm : pos	- [in]  Z 축을 이동하고자 하는 위치 값 (unit: mm) (소수점 3자리까지 유효)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsValidPhMoveZRange(DOUBLE pos);
/*
 desc : 모든 광학계 Z 축의 Homing (Mid-Position) 작업이 완료되지 않았다고 데이터 초기화
 parm : None
 retn : None
*/
API_IMPORT VOID uvCmn_Luria_ResetAllPhZAxisMidPosition();
/*
 desc : 모든 광학계 Z 축 중 1 개 이상의 상태가 Homing 중인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsAnyPhZAxisHoming();
/*
 desc : 모든 광학계 Z 축의 Homing (Mid-Position) 작업이 완료되었는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsAllPhZAxisMidPosition();
/*
 desc : 모든 광학계 Z 축이 Idle 상태인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsAllPhZAxisIdle();
/*
 desc : Return the Z Axis location value of the current Photohead
 parm : ph_no	- [in]  Photohead Number (1 ~ 8)
 retn : Photohead Z Axis 위치 값 (unit: mm), 실패일 경우, 음수 최대 값 (INT64_MIN)
		(소수점 3자리 or 6자리까지 유효)
*/
API_IMPORT DOUBLE uvCmn_Luria_GetPhZAxisPosition(UINT8 ph_no);
/*
 desc : Expose 상태 값을 문자열로 반환
 parm : None
 retn : 문자열 포인터
*/
API_IMPORT PTCHAR uvCmn_Luria_GetExposeStateToString();
/*
 desc : 현재 Luria Service의 노광 상태 값이 성공 했는지 여부 반환
 parm : flag	- [in]  0x01: Preprint, 0x02: Print, 0x03: Simulation, 0x0F: Abort
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsExposeStateSuccess(ENG_LCEP flag);
/*
 desc : 현재 Luria Service의 노광 상태 값이 진행 중인지 여부 반환
 parm : flag	- [in]  0x01: Preprint, 0x02: Print, 0x03: Simulation, 0x0F: Abort
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsExposeStateRunning(ENG_LCEP flag);
/*
 desc : 현재 Luria Service의 노광 상태 값이 실패 상태인지 여부 반환
 parm : flag	- [in]  0x01: Preprint, 0x02: Print, 0x03: Simulation, 0x0F: Abort
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsExposeStateFailed(ENG_LCEP flag);
/*
 desc : 현재 Luria Service의 노광 상태 값이 진행 중인지 여부 반환
 parm : flag	- [in]  0x01: Preprint, 0x02: Print, 0x03: Simulation, 0x0F: Abort
 retn : TRUE or FALSE
*/
API_IMPORT ENG_LPES uvCmn_Luria_GetExposeState();
/*
 desc : 노광 상태일 경우, 노광 진행 중인 Printing 횟수
 parm : None
 retn : 노광된 횟수
*/
API_IMPORT UINT8 uvCmn_Luria_GetExposePrintCount();
/*
 desc : 현재 선택된 Job Name이 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsSelectedJobName();
/*
 desc : 현재 선택된 Job Name이 Loaded 되었는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsLoadedJobName();
/*
 desc : 현재 등록된 Job Names가 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsRegisteredJobs();
/*
 desc : 현재 Align Mark 등록 상태 정보 초기화 (등록되지 않았다고 설정)
 parm : None
 retn : None
*/
API_IMPORT VOID uvCmn_Luria_ResetRegistrationStatus();
/*
 desc : 현재 Align Mark 등록 상태 정보 반환
 parm : None
 retn : 0xffff이면, 아직 상태값 수신되지 않음
*/
API_IMPORT UINT16 uvCmn_Luria_GetRegistrationStatus();
/*
 desc : Returns the status of registration
 parm : status	- [in]  status code
		mesg	- [out] The buffer in which the string of the code value will be stored
		size	- [in]  The size of 'mesg' buffer
 retn : None
*/
API_IMPORT VOID uvCmn_Luria_GetRegistrationStatusExt(UINT16 status, PTCHAR mesg, UINT32 size);
/*
 desc : Registration Last Status - 에러 정보 반환
 parm : None
 retn : 에러 문자열 반환
*/
API_IMPORT PTCHAR uvCmn_Luria_GetRegistrationStatusMesg();
/*
 desc : 설치된 모든 광학계에 LED Power가 설정되어 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsPHSetLedPowerAll();
/*
 desc : 현재 등록된 Job의 개수 반환
 parm : None
 retn : 등록된 Job의 개수 반환
*/
API_IMPORT UINT8 uvCmn_Luria_GetJobCount();
/*
 desc : 등록 (수신)된 Job Name List가 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsJobList();
/*
 desc : 현재 Luria에 설정된 기준으로, 노광 최대 속도 값 반환
 parm : step_size	- [in]  Luria Step Size (1 or Later)
 retn : 노광 속도 값
*/
API_IMPORT DOUBLE uvCmn_Luria_GetExposeMaxSpeed(UINT8 step_size);
/*
 desc : 광학계 별 노광 에너지 구하기
 parm : led_watt	- [in]  4개의 LED 에 입력된 Power Watt (unit: Watt)
		step_size	- [in]  Luria Step Size (1 or Later)
		frame_rate	- [in]  Luria Frame Rate (1 ~ 1000)
		duty_cycle	- [in]  LED Duty Cycle (1 ~ 100 %)
 retn : 노광 에너지 (mJ)
*/
API_IMPORT DOUBLE uvCmn_Luria_GetExposeEnergy(UINT8 step_size, UINT8 duty_cycle,
											  UINT16 frame_rate, DOUBLE *watt);
/*
 desc : 현재 선택된 레시피의 속성 정보를 통해 스테이지 이동 속도 및 광량 값 계산
 parm : frame_rate	- [in]  Expose Frame Rate (0 ~ 1000)
		step_size	- [in]  Step Size (1 ~ 7)
 retn : 노광 속도 (mm/sec)
*/
API_IMPORT DOUBLE uvCmn_Luria_GetExposeSpeed(UINT16 frame_rate, UINT8 step_size);
/*
 desc : 현재 선택된 레시피의 속성 정보로 노광 에너지로 스테이지 속도 계산
 parm : step_size	- [in]  Luria Step Size (1 or Later)
		expo_energy	- [DOUBLE]  노광 에너지 (unit: mJ)
		duty_cycle	- [in]  LED Duty Cycle (1 ~ 100 %)
		led_watt	- [in]  4개의 LED 에 입력된 Power Watt (unit: Watt)
 retn : Expose Frame Rate (0 ~ 1000)
*/
API_IMPORT DOUBLE uvCmn_Luria_GetEnergyToSpeed(UINT8 step_size, DOUBLE expo_energy,
	int duty_cycle, DOUBLE* watt);
/*
 desc : 현재 선택된 레시피의 속성 정보로 스테이지 속도로 노광 에너지 계산
 parm : step_size		- [in]  Luria Step Size (1 or Later)
		Expose Frame	- [in]  Rate (0 ~ 1000)
		duty_cycle		- [in]  LED Duty Cycle (1 ~ 100 %)
		led_watt		- [in]  4개의 LED 에 입력된 Power Watt (unit: Watt)
 retn : expo_energy		- [DOUBLE]  노광 에너지 (unit: mJ)
*/
API_IMPORT DOUBLE uvCmn_Luria_GetSpeedToEnergy(UINT8 step_size, UINT16 frame_rate,
	int duty_cycle, DOUBLE* watt);
/*
 desc : 현재 노광 Speed 및 거버 크기 기준으로 1 Stripe 노광하는데, 소요되는 시간 값 얻기
 parm : None
 retn : 소요 시간 값 반환 (unit: msec), 값 얻기 실패한 경우 0 값 반환
*/
API_IMPORT UINT32 uvCmn_Luria_GetOneStripeExposeTime();
/*
 desc : 거버 이름이 포함된 경로가 입력되면, 거버 경로와 거버 이름 분리해서 반환
 parm : full	- [in]  거버 이름이 포함된 거버 경로
		path	- [out] 거버 이름이 제외된 거버 경로만 반환
		name	- [out] 거버 경로가 제외된 거버 이름만 반환
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_GetGerbPathName(PTCHAR full, PTCHAR path, PTCHAR name);
/*
 desc : 가장 마지막에 등록된 Job의 위치 반환 (Zero-based)
 parm : None
 retn : 존재하면 0x00 ~ xxx, 없다면 -1
*/
API_IMPORT INT32 uvCmn_Luria_GetJobRegisteredLastIndex();
/*
 desc : 기존에 등록된 Job의 내용 초기화
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_ResetRegisteredJob();
/*
 desc : 노광 or 얼라인을 위한 테이블 (Stage)의 시작 위치 반환 (Y 위치 반환)
 parm : type	- [in]  노광 시작 위치, 얼라인 시작 위치
		tbl_no	- [in]  Table Number 즉, 0 or 1
 retn : 시작 위치 값 반환 (unit: mm)
*/
API_IMPORT DOUBLE uvCmn_Luria_GetStartY(ENG_MDMD type, UINT8 tbl_no=0x00);
/*
 desc : 노광 진행률 값 반환
 parm : None
 retn : Percent (%)
*/
API_IMPORT DOUBLE uvCmn_Luria_GetPrintRate();
/*
 desc : 현재 노광 진행 진행 횟수 값 반환
 parm : None
 retn : 현재 노광된 횟수 (현재까지 노광된 Stripe 개수)
*/
API_IMPORT UINT8 uvCmn_Luria_GetCurrentExposeScan();
/*
 desc : 거버 데이터가 등록되고 선택되고 적재가 완료된 상태인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsJobNameLoaded();
/*
 desc : 현재 노광 상태가 Idle인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsExposeIdle();
/*
 desc : Luria System의 Photohead에 에러가 없는지 유무
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsErrorPH();
/*
 desc : Luria System의 Luria PC에 에러가 없는지 유무
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsErrorPC();
/*
 desc : Table Setting에 값이 모두 설정되어 있는지 여부
 parm : tbl_no	- [in]  Table Number (1 or 2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsTableSetting(UINT8 tbl_no);
/*
 desc : 모든 Photohead의 Motor Status가 초기화 되었는지 여부
 parm : ph_cnt	- [in]  검사 대상인 Photohead Count
 retn : TRUE (초기화 완료된 상태) or FALSE (아직 초기화 되지 않은 상태)
 note : 모두 중앙으로 이동이 되었는지만 체크
*/
API_IMPORT BOOL uvCmn_Luria_IsAllMotorMiddleInited(UINT8 ph_count);
/*
 desc : 모든 Photohead Z 축이 Focus 위치로 이동 했는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsAllPhMotorFocusMoved();
/*
 desc : Photohead Offset 정보가 설정되어 있는지 여부
 parm : ph_no	- [in]  Photohead Number (2-based) (0 or 1 값 입력하면 안됨)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsPhOffset(UINT8 ph_no);
/*
 desc : Photohead Offset 정보가 설정되어 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsHysteresisType1();
/*
 desc : 현재 Luria 상태가 에러 정보를 가지고 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Luria_IsSystemStatusError();
/*
 desc : 현재 Luria 상태에 대한 에러 메시지 반환
 parm : mesg	- [in]  메시지가 저장될 버퍼
		size	- [in]  'mesg' 버퍼 크기
 retn : TRUE (에러 있음) or FALSE (에러 없음)
*/
API_IMPORT BOOL uvCmn_Luria_GetSystemStatusErrorMesg(PTCHAR mesg, UINT32 size);
/*
 desc : Get Light (LED) driver Status - OCP overrun or not / OPP overrun or not
		OCP : Over-current protection, 0PP : Over-power protection
 parm : ph_no	- [in]  PH Number (0x01 ~ 0x08)
		led_no	- [in]  LED Number (1:365, 2:385, 3:395, 4:405)
		light	- [in]  Light source (0x01 ~ 0x06)
 retn : TRUE - Over-current protection threshold reached / Over-power protection threshold reached
		FALSE- Not reached
*/
API_IMPORT BOOL uvCmn_Luria_IsLedOverOCP(UINT8 ph_no, ENG_LLPI led_no, UINT8 light=0x01);
API_IMPORT BOOL uvCmn_Luria_IsLedOverOPP(UINT8 ph_no, ENG_LLPI led_no, UINT8 light=0x01);
/*
 desc : Check the door open of LED (Light) driver
 parm : ph_no	- [in]  PH Number (0x01 ~ 0x08)
		led_no	- [in]  LED Number (1:365, 2:385, 3:395, 4:405)
 retn : TRUE - Opened , FALSE - Closed
*/
API_IMPORT BOOL uvCmn_Luria_IsLedDoorOpen(UINT8 ph_no, ENG_LLPI led_no);
/*
 desc : Check whether the temperature is in the normal range
 parm : ph_no	- [in]  PH Number (0x01 ~ 0x08)
		led_no	- [in]  LED Number (1:365, 2:385, 3:395, 4:405)
 retn : TRUE - Normal , FALSE - Abnormal
*/
API_IMPORT BOOL uvCmn_Luria_IsLedTempValid(UINT8 ph_no, ENG_LLPI led_no);
API_IMPORT BOOL uvCmn_Luria_IsBoardTempValid(UINT8 ph_no, ENG_LLPI led_no);
/*
 desc : Check whether the fan is stopped
 parm : ph_no	- [in]  PH Number (0x01 ~ 0x08)
		led_no	- [in]  LED Number (1:365, 2:385, 3:395, 4:405)
 retn : TRUE - Normal , FALSE - Abnormal
*/
API_IMPORT BOOL uvCmn_Luria_IsLedFanStopped(UINT8 ph_no, ENG_LLPI led_no);

#ifdef __cplusplus
}
#endif