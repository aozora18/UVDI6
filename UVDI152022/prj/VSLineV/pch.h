// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"

/* --------------------------------------------------------------------------------------------- */
/*                                          전역 메시지                                          */
/* --------------------------------------------------------------------------------------------- */

#define WM_MAIN_THREAD							WM_USER + 1001

#define MSG_ID_NORMAL							0x0000
#define MSG_ID_STEP_RESULT						0x1000
#define	MSG_ID_STEP_GRAB_FAIL					0x1001		/* 모델 검색 실패 */
#define MSG_ID_STEP_FIND_FAIL					0x1002		/* 모델 분석 실패 */

/* --------------------------------------------------------------------------------------------- */
/*                                           전역 상수                                           */
/* --------------------------------------------------------------------------------------------- */

#define MAX_RESULT_COUNT						128
#define COLLECT_DATA_COUNT						4	/* 0x00 : score rate, 0x01 : scale rate, 0x02 : coverage rate, 0x03 : fit error rate */
#define MIN_FILE_SAVE_COUNT						5	/* 파일로 저장하기 위해 측정 가능한 최소 개수 */

/* --------------------------------------------------------------------------------------------- */
/*                                            윈도 ID                                            */
/* --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- */
/*                                           일반 문자                                           */
/* --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- */
/*                                           전역 변수                                           */
/* --------------------------------------------------------------------------------------------- */

extern	TCHAR									g_tzWorkDir[MAX_PATH_LEN];
extern	LOGFONT									g_lf;


/* --------------------------------------------------------------------------------------------- */
/*                                            열거형                                             */
/* --------------------------------------------------------------------------------------------- */



/* --------------------------------------------------------------------------------------------- */
/*                                            구조체                                             */
/* --------------------------------------------------------------------------------------------- */
#pragma pack (push, 8)

typedef struct __st_z_axis_array_list__
{
	UINT8			set_value[COLLECT_DATA_COUNT];			/* 0x00 : score, 0x01 : scale, 0x02 : Coverage, 0x03 : Fit Error */
	UINT8			ph_no;									/* 0x01 ~ 0x08 */
	UINT8			u8_reserved[3];

	UINT16			steps;									/* 노광 구간 (0x01 ~ 0xff) */
	UINT16			u16_reserved[3];
	/* 평균, 최대, 최소 값*/
	DOUBLE			avg_value[COLLECT_DATA_COUNT];			/* 0x00 : Score (%), 0x01 : Scale (%), 0x02 : Coverage (%) */
	DOUBLE			max_value[COLLECT_DATA_COUNT];			/* 0x00 : Score (%), 0x01 : Scale (%), 0x02 : Coverage (%) */
	DOUBLE			min_value[COLLECT_DATA_COUNT];			/* 0x00 : Score (%), 0x01 : Scale (%), 0x02 : Coverage (%) */
	DOUBLE			std_value[COLLECT_DATA_COUNT];			/* 0x00 : Score (%), 0x01 : Scale (%), 0x02 : Coverage (%) */
	CArray <DOUBLE, DOUBLE>	arrValue[COLLECT_DATA_COUNT];	/* 0x00 : Score (%), 0x01 : Scale (%), 0x02 : Coverage (%) */

public:

	/* 현재 등록된 개수 반환 */
	UINT32 GetCount()
	{
		return (UINT32)arrValue[0].GetCount();
	}
	/* 
	 desc : 현재 등록된 값 중에 가장 큰 값과 작은 값의 차이
	 parm : type	- [in]  0x00 : Score, 0x01 : Scale, 0x02 : Coverage
	 retn : 차이 값 반환
	*/
	DOUBLE GetDiff(UINT8 type)
	{
		return (max_value[type] - min_value[type]);
	}
	/*
	 desc : LPM_ZAAL 구조체 포인터 자료 비교 함수
	 parm : elem1	- [in]  비교하기 위한 Strength 값
			elem2	- [in]  비교하기 위한 Strength 값
	 retn : elem1 - elem2인 경우, 오름차순 정렬
			elem2 - elem1인 경우, 내림차순 정렬
	*/
	static int CompareStrength(const void *elem1, const void *elem2)
	{
		DOUBLE dbElem1	= *(DOUBLE*)elem1;
		DOUBLE dbElem2	= *(DOUBLE*)elem2;
		if (dbElem1 < dbElem2)		return -1;	/* elem1이 elem2보다 작을 때 */
		else if (dbElem1 > dbElem2)	return 1;	/* elem1이 elem2보다 클 때 */
		return 0;	/* 동일한 값일 때 */
	}
	/*
	 desc : 수집된 엣지 강도 및 길이들의 평균 값 구하기 (개인 알고리즘)
	 parm : None
	 retn : 평균 강도 값 반환
	*/
	VOID CalcAverage()
	{
		UINT32 i, j, k, l, u32Total	= (UINT32)arrValue[0].GetCount(), /*u32Start = 0, */u32Count;
		DOUBLE dbSum	= 0.0f;

		/* 0x00 : Score (%), 0x01 : Scale (%), 0x02 : Coverage (%) */
		for (j=0; j<COLLECT_DATA_COUNT && (arrValue[j].GetCount() > 0); j++)
		{
			/* 현재 총 개수 저장 */
			u32Count	= u32Total;
			/* 평균 값 구하기 (설정된 개수만큼 최소/최대 값 제외) */
			avg_value[j]= 0.0f;
			for (i=0,k=0,l=0,max_value[j]=DBL_MIN,min_value[j]=DBL_MAX,std_value[j]=0.0f; i<u32Total; i++)
			{
				avg_value[j]   += arrValue[j].GetAt(i);
				if (max_value[j] < arrValue[j].GetAt(i))
				{
					max_value[j]	= arrValue[j].GetAt(i);
					k	= i;	/* Max Index */
				}
				if (min_value[j] > arrValue[j].GetAt(i))
				{
					min_value[j]	= arrValue[j].GetAt(i);
					l	= i;	/* Min Index */
				}
			}
			if (avg_value[j])
			{
				/* 최대/최소 값 버림 */
				if (u32Total > 2)
				{
					avg_value[j]-= (max_value[j] + min_value[j]);
					u32Count	-= 2;
				}
				avg_value[j] /= DOUBLE(u32Count);

				/* 표준 편차 구하기 */
				for (i=0,dbSum=0.0f; i<u32Total && u32Total > 2; i++)
				{
					/* 최대 / 최소 값 제외 */
					if (i == k || i == l)	continue;
					/* 제곱 구함 */
					dbSum	+= pow(arrValue[j].GetAt(i) - avg_value[j], 2.0f);
				}
				/* 표준 편차 구하기 */
				if (dbSum > 0.0f)	std_value[j] = sqrt (dbSum / DOUBLE(u32Count));

				/* 측정된 값과 평균 값의 오차 값이 표준 편차 값 보다 큰 것은 버림 */
			}
		}
	}

	/*
	 desc : 수집된 엣지 강도 및 길이들의 평균 값 구하기 (정규 분포 알고리즘을 통해 이상치 값 제거)
	 parm : rate	- [in]  표준 편차에 얼마 값을 곱할지 개수 즉,
	 						(평균 값 - rate * 표준편차) <= 측정 값 <= (평균 값 + rate * 표준편차)
	 						이 사이의 측정 값이면 유효한 값으로 판단
	 retn : 평균 강도 값 반환
	*/
	VOID CalcAverageEx(DOUBLE rate)
	{
		UINT32 i, j, u32Count, u32Total = (UINT32)arrValue[0].GetCount();
		DOUBLE dbSum	= 0.0f;

		/* 각 항목 별 총합 / 평균 구하기 (Score Rate, Scale Rate, Coverage Rate, Fit Error)) */
		for (i=0; i<COLLECT_DATA_COUNT; i++)
		{
			dbSum	= 0.0f;
			/* 총합 구하기 */
			for (j=0; j<u32Total; j++)
			{
				dbSum	+= arrValue[i].GetAt(j);
			}
			/* 평균 구하기 */
			avg_value[i]	= dbSum / DOUBLE(u32Total);
		}

		/* 각 항목 별 표준 편차 구하기 (Score Rate, Scale Rate, Coverage Rate)) */
		for (i=0; i<COLLECT_DATA_COUNT; i++)
		{
			dbSum	= 0.0f;
			/* 제곱 구함 */
			for (j=0; j<u32Total; j++)
			{
				dbSum	+= pow(arrValue[i].GetAt(j) - avg_value[i], 2.0f);
			}
			/* 표준 편차 구하기 */
			std_value[i]	= 0.0f;	/* 반드시 초기화 */
			if (dbSum > 0.0f)	std_value[i] = sqrt (dbSum / DOUBLE(u32Total));
		}

		/* 표준 편차 구간에 벗어난 값은 제외 (이상치 제외)하고 총 합 구하기 (정규 분포로 값 구하기) */
		/* (Score Rate, Scale Rate, Square Rate) */
		for (i=0; i<COLLECT_DATA_COUNT; i++)
		{
			dbSum	= 0.0f;
			u32Count= 0;
			/* 총합 구하기 */
			for (j=0; j<u32Total; j++)
			{
				/* (현재 값) 이 (평균 값 기준 +/- 편차 값) 값 범위 내에 있는지 확인 */
				if (((avg_value[i] - std_value[i] * rate) <= arrValue[i].GetAt(j)) &&
					((avg_value[i] + std_value[i] * rate) >= arrValue[i].GetAt(j)))
				{
					dbSum	+= arrValue[i].GetAt(j);
					u32Count++;
				}
			}
			/* 평균 구하기 (Score Rate, Scale Rate, Square Rate)) */
			avg_value[i]	= dbSum / DOUBLE(u32Count);
		}
	}

	/*
		설명 : 모든 메모리 해제
		변수 : None
		반환 : None
	*/
	VOID RemoveArrayAll()
	{
		UINT32 i	= 0;
		for (; i<COLLECT_DATA_COUNT; i++)	arrValue[i].RemoveAll();
	}

}	STG_ZAAL,	*LPG_ZAAL;

#pragma pack (pop)	/* 8 bytes */


#endif //PCH_H
