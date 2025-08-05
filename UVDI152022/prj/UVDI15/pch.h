// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"

/* ---------------------------------------------------------------------------------------------- */
/*                                           전역 메시지                                          */
/* ---------------------------------------------------------------------------------------------- */

#define WM_MAIN_THREAD							WM_USER + 1000
#define WM_MAIN_CHILD							WM_USER + 1001
#define WM_MAIN_RECIPE_UPDATE					WM_USER + 1002
#define WM_MAIN_PROCESS_UPDATE					WM_USER + 1003
#define WM_MAIN_RECIPE_CREATE					WM_USER + 1004
#define WM_MAIN_RECIPE_DELETE					WM_USER + 1005
#define WM_MAIN_RECIPE_CHANGE					WM_USER + 1006
#define WM_MAIN_PHIL_MSG						WM_USER + 1007

#define WM_IDD_KBDM								WM_USER + 2001
#define WM_IDD_KBDT								WM_USER + 2002

/* ---------------------------------------------------------------------------------------------- */
/*                                            윈도 MSG ID                                         */
/* ---------------------------------------------------------------------------------------------- */

#define MSG_MAIN_THREAD_PERIOD					0x01

/* ---------------------------------------------------------------------------------------------- */
/*                                            전역 상수                                           */
/* ---------------------------------------------------------------------------------------------- */

#define NONE_TRIGGER_BOARD	1


/* ---------------------------------------------------------------------------------------------- */
/*                                            일반 문자                                           */
/* ---------------------------------------------------------------------------------------------- */



/* ---------------------------------------------------------------------------------------------- */
/*                                            문서 제목                                           */
/* ---------------------------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------------------------- */
/*                                            전역 변수                                           */
/* ---------------------------------------------------------------------------------------------- */
typedef enum EN_FONT_LEVEL
{
	eFONT_LEVEL1_BOLD,
	eFONT_LEVEL2_BOLD,
	eFONT_LEVEL3_BOLD,
	eFONT_LEVEL1,
	eFONT_LEVEL2,
	eFONT_LEVEL3,
	eFONT_MAX,
}EN_FONT_LEVEL;


extern	TCHAR									g_tzWorkDir[MAX_PATH_LEN];
extern	LOGFONT									g_lf[eFONT_MAX];
extern	CFont									g_font[eFONT_MAX];
extern	COLORREF								g_clrBtnColor;;
extern	COLORREF								g_clrBtnTextColor;;
extern	COLORREF								g_clrBtnSelColor;;
extern	COLORREF								g_clrBtnSelTextColor;;
extern  UINT									g_u8Romote;			/*1:Initial 2:Manual 3:Auto 4:PM*/
extern  UINT16									g_u16PhilCommand;	/**/
extern  UINT									g_loginLevel;


/* ---------------------------------------------------------------------------------------------- */
/*                                             열거형                                             */
/* ---------------------------------------------------------------------------------------------- */

/* 작업 종류 */
typedef enum class __en_base_work_operate_kind__ : UINT8
{
	en_work_none	= 0x00,

	/* Main Thread Message */
	en_mesg_norm	= 0x01,
	en_mesg_init	= 0x02,		/* InitWork 수행 했다고 알림 */
	en_mesg_expo	= 0x03,		/* Start Button으로 얼라인 노광 알림 */
	en_mesg_mark	= 0x04,		/* Align Mark 출력 알림 */

	/* Work */
	en_work_ready	= 0x11,		/* Work Ready */
	en_work_busy	= 0x12,		/* Work Busy */
	en_work_init	= 0x13,		/* Work Init */
	en_work_home	= 0x14,		/* Work Homing */
	en_work_stop	= 0x15,		/* Work Stop */

	/* Recipe */
	en_gerb_load	= 0x21,		/* Load the gerber (recipe) */
	en_gerb_unload	= 0x22,		/* Unload the gerber (recipe) */
	en_gerb_onlyfem	= 0x23,		/* Only FEM(Foucs&Emergy) (recipe) */

	/* Mark */
	en_mark_move	= 0x31,		/* Align Mark 위치로 이동 */
	en_mark_test	= 0x34,		/* Align Mark 검사 */

	/* Expose */
	en_expo_only	= 0x41,		/* Only 노광 진행 */
	en_expo_align	= 0x42,		/* Align Mark 진행 후 노광 진행 */
	en_expo_cali	= 0x43,		/* Align Mark 및 보정 적용 후 노광 진행 */
	en_expo_spot	= 0x44,		/* Spot Align Mark 후 노광 진행 */
	en_gerb_expofem = 0x45,		/* FEM(Foucs&Emergy) (recipe) + expo */

	/* environmental works */
	en_env_calib = 0x51,
	en_work_roaming = 0x52, //로밍.

	en_work_request = 0x61,

	en_local_gerb_load = 0x71,		//로컬레시피로드

	/* App Exit */
	en_app_exit		= 0xff,		/* 프로그램 종료 메시지 */

}	ENG_BWOK;

/* 자식 메뉴 다이얼로그 ID (!!! 중요 !!! 리소스 ID와 동일한 순서로 설정) */
typedef enum class __en_child_menu_dialog_id__ : UINT8
{
	en_menu_none	= 0x00,		/* Not selected */
	en_menu_auto	= 0x01,		/* Expose Dialog */
	en_menu_manual	= 0x02,		/* Expose Dialog */
	en_menu_expo	= 0x03,		/* Expose Dialog */
	en_menu_gerb	= 0x04,		/* Gerber Recipe Dialog */
	en_menu_mark	= 0x05,		/* Mark Recipe Dialog */
	en_menu_ctrl	= 0x06,		/* Control Dialog */
	en_menu_logs	= 0x07,		/* Log Dialog */
	en_menu_conf	= 0x08,		/* Setup (Config) Dialog */
	// by sysandj : Calibration 메뉴 추가
	en_menu_calb	= 0x09,		/* Calibration Dialog */
	en_menu_philhmi	= 0x0a,		/* Philhmi Dialog */

}	ENG_CMDI;

// by sysandj : submenu
typedef enum class __en_child_submenu_dialog_id__ : UINT8
{
	en_menu_sub_none = 0x00,		/* Not selected */
	
	en_menu_sub_calb_uvpower		= 0x01,		/* Uv power Dialog */
	en_menu_sub_calb_camera			= 0x02,		/* Camera Dialog */
	en_menu_sub_calb_exposure		= 0x03,		/* Exposure Dialog */
	en_menu_sub_calb_accuracy		= 0x04,		/* Accuracy Dialog */
	en_menu_sub_calb_flatness		= 0x05,		/* Flatness Dialog */
	en_menu_sub_conf_setting_option = 0x06,		/* Setting Dialog */
	en_menu_sub_conf_teach			= 0x07,		/* Teach Dialog */
	en_menu_sub_conf_autofocus		= 0x08,		/* 오토포커스 */

}	ENG_CMDI_SUB;
// by sysandj : submenu

// by sysandj : tab menu
typedef enum class __en_child_tabmenu_dialog_id__ : UINT8
{
	en_menu_tab_none = 0x00,		/* Not selected */

	en_menu_tab_calb_uvpower_measure = 0x01,		/* Uv power Measure Dialog */
	en_menu_tab_calb_uvpower_check	= 0x02,		/* Uv power check Dialog */

	en_menu_tab_calb_camera_spec	= 0x01,		/* Camera Spec Dialog */
	en_menu_tab_calb_camera_area	= 0x02,		/* Camera Area Dialog */

	en_menu_tab_calb_exposure_fem	= 0x01,		/* Exposure FEM Dialog */
	en_menu_tab_calb_exposure_flush = 0x02,		/* Exposure Flush Dialog */

	en_menu_tab_calb_accuracy_measure = 0x01,	/* Accuracy measure Dialog */
	en_menu_tab_calb_accuracy_check = 0x02,		/* Accuracy check Dialog */

}	ENG_CMDI_TAB;
// by sysandj : tab menu

enum EN_USER_MSG
{
	eMSG_MAIN_OPEN_CONSOLE = WM_USER + 500,
	eMSG_MAIN_LOGIN_CONSOLE,
	eMSG_MAIN_ADMIN_CONSOLE,
	eMSG_MAIN_MANAGER_CONSOLE,

	eMSG_UV_POWER_INPUT_TABLE,
	eMSG_UV_POWER_INPUT_PROGRESS,
	eMSG_UV_POWER_INPUT_XY_POS,
	eMSG_UV_POWER_INPUT_Z_POS,

	eMSG_UV_POWER_RECIPE_SET_POINT,
	eMSG_UV_POWER_RECIPE_MEASURE_COMP,
	eMSG_UV_POWER_RECIPE_ALL_COMP,

	eMSG_CAMERA_SPEC_PROGRESS,
	eMSG_CAMERA_SPEC_RESULT,
	eMSG_CAMERA_SPEC_COMPLETE,
	eMSG_CAMERA_SPEC_GRAB_VIEW,

	eMSG_EXPO_FEM_PRINT_SET_POINT,
	eMSG_EXPO_FEM_PRINT_PRINT_COMP,
	eMSG_EXPO_FEM_RESULT_PROGRESS,

	eMSG_EXPO_FLUSH_RESULT,
	eMSG_EXPO_FLUSH_RESULT_PROGRESS,

	eMSG_ACCR_MEASURE_GRAB,
	eMSG_ACCR_MEASURE_REPORT,
	WM_USER_CREATE_AUTO_MENU,
};

/*1:Auto 2:Stop 3:Pause 4:Alarm 5:PM*/
enum EN_PHILHMI_MODE
{
	en_menu_phil_mode_none	= 0x00,
	en_menu_phil_mode_auto	= 0x01,
	en_menu_phil_mode_stop	= 0x02,
	en_menu_phil_mode_pause = 0x03,
	en_menu_phil_mode_alarm = 0x04,
	en_menu_phil_mode_pm	= 0x05,
};

enum EN_PHILHMI_MOVE
{
	en_menu_phil_move_none = 0x00,
	en_menu_phil_move_abs = 0x01,
	en_menu_phil_move_rel = 0x02,
	en_menu_phil_move_char = 0x03,
};


enum EN_LOGIN_LEVEL
{
	eLOGIN_LEVEL_UNKNOWN = 0,
	eLOGIN_LEVEL_OPERATOR,
	eLOGIN_LEVEL_ENGINEER,
	eLOGIN_LEVEL_ADMIN,
};

static CString sstrLoginLevel[] =
{
	_T("UNKNOWN"),
	_T("OPERATOR"),
	_T("ENGINEER"),
	_T("ADMIN"),
};

/* ---------------------------------------------------------------------------------------------- */
/*                                             구조체                                             */
/* ---------------------------------------------------------------------------------------------- */
#pragma pack (push, 8)

/* 예열 노광 정보가 저장된 구조체 */
typedef struct __st_cmps_pre_heating_expose__
{
	UINT8				move_acam;	/* Align Camera 좌/우 끝으로 이동 여부 */
	UINT8				move_ph;	/* Photohead 상/하 동작 여부 */
	UINT8				u8_reserved[6];
	UINT32				expo_count;	/* 노광 반복 횟수 (최소 1 이상 값) */
	UINT32				u32_reserved;

}	STG_CPHE,	*LPG_CPHE;

typedef struct __st_cmps_philhmi_config_value_event__
{
	BOOL				mp_start;
	BOOL				safe_pos;
	BOOL				recipe_comp;
	UINT8				phil_mode;
	UINT8				phil_move;
	UINT8				drive_id;
	DOUBLE				move_dist;

	UINT16				error_code;
	POINTER_64_INT		unique_id;

	
	/*
	 desc : 초기화
	 parm : None
	 retn : None
	*/
	VOID Init()
	{
		mp_start = FALSE;
		safe_pos = FALSE;
		recipe_comp = FALSE;
		phil_mode = en_menu_phil_mode_none;
		phil_move = en_menu_phil_move_none;

		unique_id = 0;
	}

}	STG_CPVE, * LPG_CPVE;

/* Philhmi 노광 완료후 Expo Log 파라미터 */
typedef struct __st_cmps_expo_log_after_completion
{
	CHAR				data[40];						/* 노광 날짜 */
	UINT16				temp;							/* 온도 */
	UINT64				expo_time;						/* 노광 시간 Tack*/
	UINT8				expo_mode;						/* 노광 모드 */
	UINT8				expo_succ;						/* 노광 성공, 실패 여부*/

	DOUBLE				real_scale;						/* 거버의 크기 대비 소재 크기의 배율에 대한 오차 범위*/
	DOUBLE				real_rotaion;					/* 소재의 수평 회전에 대한 오차 범위*/
	DOUBLE				global_dist[6];					/* Align 후 Global Mark 간의 거리 오차 허용 값 (unit: 100 nm or 0.1 um)	*/
	CHAR				gerber_name[MAX_GERBER_NAME];	/* Gerber Name (Only Gerber Name, Path is not included)		*/
	UINT32				material_thick;					/* Exposure Material Thickness (unit: um)					*/
	DOUBLE				expo_energy;					/* 노광 에너지 (unit: mJ)							*/

	UINT8				align_type;						/* Align Mark Array(Global, Local) Type				*/

#ifdef USE_ALIGNMOTION
UINT16					align_motion;					//얼라인모션
#endif


	UINT8				mark_type;						/* Mark 구분 즉, 0 : Geomatrix, 1 : Pattern Image	*/
	UINT8				lamp_type;						/* 램프 조명 타입 (0:Ring, 1:Coaxial)				*/
	UINT8				gain_level[2];					/* Align Camera에 대한 Gain Level 값 (0 ~ 255)      */
	UINT8				led_duty_cycle;					/* Led Duty Cycle (0 ~ 90%)	*/

	CHAR				power_name[LED_POWER_NAME_LENGTH];				// Led Power Recipe Name
	CHAR				host_recipe_name[DEF_MAX_RECIPE_NAME_LENGTH];
	CHAR				recipe_name[DEF_MAX_RECIPE_NAME_LENGTH];		// Recipe 명은 최대 40자		
	CHAR				glassID[DEF_MAX_GLASS_NAME_LENGTH];				// Glass ID는 최대 40자	

	UINT8				move_acam ;						/* Align Camera 좌/우 끝으로 이동 여부 */
	UINT8				move_ph;						/* Photohead 상/하 동작 여부 */
	UINT8				ready_mode;						/* 사용 모드 0 :  일반 모드, 1 : 단차 모드*/
	UINT8				u8_reserved[6];
	UINT32				expo_count;						/* 노광 반복 횟수 (최소 1 이상 값) */
	UINT32				u32_reserved;
	UINT16				includeAddAlignOffset; //이동시에 얼라인옵셋까지 추가해서 움직일지
	UINT32 headOffset;
		/*
	 desc : 초기화
	 parm : None
	 retn : None
	*/
	VOID Init()
	{
		temp			= 0;
		expo_time		= 0;
		expo_mode		= 0x00;
		expo_succ		= 0x00;

		real_scale		= 0.0f;
		real_rotaion	= 0.0f;
		material_thick	= 0;
		expo_energy		= 0.0f;

		align_type		= 0x00;
		mark_type		= 0x00;
		lamp_type		= 0x00;
		gain_level[0]	= 0x00;
		gain_level[1]	= 0x00;
		led_duty_cycle	= 0x00;

		memset(data, 0x00, 40);
		memset(recipe_name,	 0x00, DEF_MAX_RECIPE_NAME_LENGTH);
		memset(glassID,		 0x00, DEF_MAX_GLASS_NAME_LENGTH);

		move_acam = 0;
		move_ph = 0;
		ready_mode = 0;
		expo_count = 1;
		includeAddAlignOffset = 0;
	}

}	STG_CELA, * LPG_CELA;


#pragma pack (pop)


struct ST_UI_LEVEL
{
	ST_UI_LEVEL()
	{
		nLevel = EN_LOGIN_LEVEL::eLOGIN_LEVEL_UNKNOWN;
	}

	ST_UI_LEVEL(const EN_LOGIN_LEVEL& level, CWnd* control)
	{
		nLevel = level;
		pwndControl = control;
	}

	EN_LOGIN_LEVEL		nLevel;
	CWnd* pwndControl;

};

typedef std::vector < ST_UI_LEVEL > vST_UI_LEVEL;



/* ---------------------------------------------------------------------------------------------- */
/*                                            전역 함수                                           */
/* ---------------------------------------------------------------------------------------------- */


class CTactTimeCheck
{
private:
	__int64 m_nTickPerSec;
	__int64 m_nTickCountStart;
	__int64 m_nTickCountStop;
	__int64 m_nTickCountPauseStart;
	__int64 m_nTickCountPauseStop;

	double	m_dTickPerSec;
	double	m_dTickPerMs;
	double	m_dTickPerUs;

public:
	CTactTimeCheck()
	{
		m_nTickPerSec = 0;
		m_nTickCountStart = 0;
		m_nTickCountStop = 0;
		m_nTickCountPauseStart = 0;
		m_nTickCountPauseStop = 0;

		m_dTickPerSec = 0.;
		m_dTickPerMs = 0.;
		m_dTickPerUs = 0.;
	}
	~CTactTimeCheck()
	{
	}

public:
	BOOL Init()
	{
		LARGE_INTEGER li;
		if (!QueryPerformanceFrequency(&li))
			return FALSE;

		m_nTickPerSec = static_cast<__int64>(li.QuadPart);
		m_dTickPerSec = (double)m_nTickPerSec;
		m_dTickPerMs = m_nTickPerSec / 1000.0;
		m_dTickPerUs = m_nTickPerSec / 1000000.0;

		return TRUE;
	}
	__int64 GetTickCount()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return static_cast<__int64>(li.QuadPart);
	}
	void Start()
	{
		m_nTickCountStart = GetTickCount();
	}
	void PauseStart()
	{
		m_nTickCountPauseStart = GetTickCount();
	}
	void PauseStop()
	{
		m_nTickCountPauseStop = GetTickCount();
	}
	void Stop()
	{
		m_nTickCountStop = GetTickCount();
	}
	void Clear()
	{
		m_nTickCountStart = 0;
		m_nTickCountStop = 0;
		m_nTickCountPauseStart = 0;
		m_nTickCountPauseStop = 0;
	}
	__int64 GetTactMs()
	{
		ASSERT(m_nTickPerSec);
		__int64	lnTickCountPauseTime = m_nTickCountPauseStop - m_nTickCountPauseStart;
		return (__int64)((double)((m_nTickCountStop - m_nTickCountStart) - lnTickCountPauseTime) / m_dTickPerMs);
	}
	__int64 GetTactUs()
	{
		ASSERT(m_nTickPerSec);
		__int64	lnTickCountPauseTime = m_nTickCountPauseStop - m_nTickCountPauseStart;
		return (__int64)((double)((m_nTickCountStop - m_nTickCountStart) - lnTickCountPauseTime) / m_dTickPerUs);
	}
	__int64 GetTactSec()
	{
		ASSERT(m_nTickPerSec);
		__int64	lnTickCountPauseTime = m_nTickCountPauseStop - m_nTickCountPauseStart;
		return (__int64)((double)((m_nTickCountStop - m_nTickCountStart) - lnTickCountPauseTime) / m_dTickPerSec);
	}
	double GetTactSecDouble()
	{
		ASSERT(m_nTickPerSec);
		__int64	lnTickCountPauseTime = m_nTickCountPauseStop - m_nTickCountPauseStart;
		return ((double)((m_nTickCountStop - m_nTickCountStart) - lnTickCountPauseTime) / m_dTickPerSec);
	}
};

#endif //PCH_H
