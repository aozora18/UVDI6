
/*
 desc : Common Library
*/

#pragma once


#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : 현재 소켓 기술자에 부여된 IPv4 주소 얻기
 parm : sd	- [in]  소켓 기술자
 retn : IPv4 주소 반환, 실패할 경우 0 값
*/
API_IMPORT UINT32 uvCmn_GetSocketToIPv4(SOCKET sd);
/*
 desc : 현재 소켓 기술자에 부여된 PORT 번호 얻기
 parm :	sd	- [in]  소켓 기술자
 retn : PORT 번호 반환
*/
API_IMPORT UINT16 uvCmn_GetSocketToPort(SOCKET sd);
/*
 desc : Bytes IP를 4 바이트의 IPv4로  변환 및 반환
 parm : ipv4	- [in]  문자열 IPv4 (Input)
 retn : 반환될 4바이트의 IPv4
*/
API_IMPORT UINT32 uvCmn_GetIPv4BytesToUint32(PUINT8 ipv4);
/*
 desc : 문자열 IP를 4 바이트의 IPv4로  변환 및 반환
 parm : ipv4	- [in]  문자열 IPv4 (Input)
 retn : 반환될 4바이트의 IPv4 (Output)
*/
API_IMPORT UINT32 uvCmn_GetIPv4StrToUint32(PTCHAR ipv4);
/*
 desc : 4 바이트의 IPv4 정보를 문자열 IP로 변환 및 반환
 parm : num_ipv4	- [in]  4바이트의 IPv4
		str_ipv4	- [out] 반환될 문자열 IPv4의 버퍼 포인터
		str_size	- [in]  'str_ipv4' 버퍼 크기
 retn : 반환될 문자열 IPv4
*/
API_IMPORT VOID uvCmn_GetIPv4Uint32ToStr(UINT32 ipv4, PTCHAR str_ipv4, UINT32 str_size);
/*
 desc : 4 바이트의 IPv4 정보를 바이트 버퍼의 IP로 변환 및 반환
 parm : num_ipv4	- [in]  4바이트의 IPv4
		byte_ipv4	- [out] 반환될 문자열 IPv4의 버퍼 포인터
 retn : None
*/
API_IMPORT VOID uvCmn_GetIPv4Uint32ToBytes(UINT32 num_ipv4, PUINT8 byte_ipv4);
/*
 desc : 배열 (바이트) 역순으로 변환하여 저장
 parm : data	- [out] 배열 포인터 (입력 : 원본, 출력 : 역순 배열)
		size	- [in]  배열 개수
 retn : None
*/
API_IMPORT VOID uvCmn_GetReverseArrayUint8(PUINT8 data, UINT32 size);
/*
 desc : 폰트 이름에 대한 구조체 정보 반환
 parm :	lf		- [in]  반환되어 저장될 로그 폰트 구조체
		name	- [in]  폰트 이름
		size	- [in]  폰트 크기
		lang	- [in]  국가 언어 코드
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_GetLogFont(LOGFONT &lf,
								 PTCHAR name,
								 UINT32 size,
								 UINT8 lang=HANGUL_CHARSET);
/*
 desc : 임의 폴더 내에 저장되어 있는 파일 목록(하위 폴더 포함) 구하기
 parm : search	- [in]  검색하고자 하는 첫 번째 폴더
		filter	- [in]  검색하고자 하는 정보(파일 혹은 특수 문자 등등)
						모든 파일을 검색하려면 L"" (Empty) 값이면 됨
		file	- [out] 검색된 파일 정보가 저장될 스트링 배열 참조 변수
		recurse	- [in]  하위 디렉토 검사 여부 (0x01, 0x00)
 retn : None
*/
API_IMPORT VOID uvCmn_SearchRecurseFile(CString search, CString filter, CStringArray &file,
										UINT8 recurse=0x01);
/*
 desc : 임의 폴더 기준으로 하위에 존재하는 폴더 (전체 경로 포함) 명 반환
 parm : search	- [in]  검색하고자 하는 폴더
		paths	- [out] 검색된 경로가 저장될 배열 리스트 참조 변수
 retn : Zero-based 값
*/
API_IMPORT VOID uvCmn_SearchRecursePath(CString search, CStringArray &path);
/*
 desc : 임의 폴더 내에 저장되어 있는 파일 목록 구하기
 parm : path	- [in]  검색하고자 하는 첫 번째 폴더
		filter	- [in]  검색하고자 하는 정보(파일 혹은 특수 문자 등등)
		file	- [out] 검색된 파일 정보가 저장될 스트링 배열 참조 변수
 retn : None
*/
API_IMPORT VOID uvCmn_SearchFile(CString path, CString filter, CStringArray &file);
/*
 desc : 임의 폴더 내에 저장되어 있는 파일의 개수 구하기
 parm : path	- [in]  검색하고자 하는 첫 번째 폴더
		filter	- [in]  검색하고자 하는 정보(파일 혹은 특수 문자 등등)
 retn : 검색된 파일 개수 반환
*/
API_IMPORT UINT32 uvCmn_SearchFileCount(CString path, CString filter);
/*
 desc : 해당 디렉토리(경로)가 존재하는지 유무
 parm : path	- [in]  디렉토리(경로) 명
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_FindPath(PTCHAR path);
/*
 desc : 해당 파일이 존재하는지 유무
 parm : file	- [in]  디렉토리(경로)가 포함된 파일 명
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_FindFile(PTCHAR file);
API_IMPORT BOOL uvCmn_FindFileA(PCHAR file);
/*
 desc : 전체 경로를 가진 파일을 경로와 파일로 분리하여 반환
 parm : full_file	- [in]  전체 경로가 포함된 파일명
		full_size	- [in]  'path_file' 버퍼에 저장된 데이터의 크기
		path_name	- [out] 전체 경로 저장될 버퍼 포인터
		path_size	- [in]  'path_name' 버퍼의 크기
		file_name	- [out] 파일명(확장자 포함) 저장될 버퍼 포인터
		file_size	- [in]  'file_name' 버퍼의 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_GetPathFile(PTCHAR full_file, UINT32 full_size,
								  PTCHAR path_name, UINT32 path_size,
								  PTCHAR file_name, UINT32 file_size);
/*
 desc : 전체 경로를 가진 파일 중 파일 이름 (확장자 포함)만 추출하여 반환
 parm : file	- [in]  전체 경로가 포함된 파일명
		size	- [in]  'path_file' 버퍼에 저장된 데이터의 크기
 retn : 파일이 이름이 저장되어 있는 포인트 반환
*/
API_IMPORT PTCHAR uvCmn_GetFileName(PTCHAR file, UINT32 size);
/*
 desc : 해당 디렉토리(경로) 내에 존재하는 하위 폴더들의 개수 반환
 parm : path	- [in]  디렉토리(경로) 명
 retn : 0 - 없음, 양수 - 개수
*/
API_IMPORT UINT32 uvCmn_GetChildPathCount(PTCHAR path);
/*
 desc : 해당 디렉토리(경로) 내에 존재하는 하위 폴더들의 정보 반환
 parm : path	- [in]  디렉토리(경로) 명
		data	- [out] 디렉토리 리스트가 저장됨
 retn : None
*/
API_IMPORT VOID uvCmn_GetChildPathList(PTCHAR path, CStringArray &data);
/*
 desc : 혼합 문자열(문자, 숫자, 특수 문장 등...) 정렬하기
 parm : data	- [in]  정렬하고자 하는 객체가 저장된 CStringArray 클래스
		sort	- [in]  TRUE: 오름차순 정렬, FALSE: 내림차순 정렬
 retn : None
*/
API_IMPORT VOID uvCmn_SortMixedString(CStringArray& data, UINT8 sort=1);
/*
 desc : 특정 디렉토리 용량 얻기 (디렉토리 내의 하위 디렉토리까지 포함됨)
 parm : path	- [in]  디렉토리 정보가 저장된 버퍼 포인터 (폴더 마지막에  '\\' 제외)
 retn : 크기 반환 (단위: Bytes)
*/
API_IMPORT UINT64 uvCmn_GetDirectorySize(PTCHAR path);
/*
 desc : 파일의 크기 계산
 parm : file	- [in]  전체 경로가 포함된 파일
		mode	- [in]  "rb", "rt" 중 1개
 retn : 파일의 크기 / 0 값이면 실패 (읽기 실패) (단위: bytes)
*/
API_IMPORT UINT32 uvCmn_GetFileSize(PTCHAR file, PTCHAR mode=L"rb");
/*
 desc : 파일의 크기 계산 (정수 반환) - UINCODE 파일 크기
 parm : file	- [in]  전체 경로가 포함된 파일
		mode	- [in]  0x00: "rb, ccs=UTF-16LE"
						0x01: "rt, ccs=UTF-16LE"
 retn : 파일의 크기 / 0 값이면 실패 (읽기 실패)
*/
API_IMPORT UINT32 uvCmn_GetFileSizeEx(PTCHAR file, UINT8 mode=0x00);
/*
 desc : 파일의 크기 계산 (정수 반환) - Ansi/Unicode 파일 크기
 parm : file	- [in]  전체 경로가 포함된 파일
 retn : 파일의 크기 / 0 값이면 실패 (읽기 실패)
*/
API_IMPORT UINT32 uvCmn_GetFileSizeAuto(PTCHAR file);
/*
 desc : 파일의 라인수 구하기 (정수 반환) (Text 파일만 해당됨)
 parm : file	- [in]  전체 경로가 포함된 파일
 retn : 파일의 전체 라인수 구하기 (-1인 경우, 라인 구하기 실패)
*/
API_IMPORT INT32 uvCmn_GetFileLines(PTCHAR file);
/*
 desc : 단위가 포함된 파일 크기 계산 (문자열 반환)
 parm : file	- [in]  전체 경로가 포함된 파일
		data	- [out] 단위(Bytes, KBytes, MBytes, GBytes)가 포함된 크기가 저장될 버퍼
		size	- [in]  'data' 버퍼의 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_GetFileSizeInUnits(PTCHAR file, PTCHAR data, UINT32 size);
/*
 desc : 파일의 내용을 버퍼에 저장
 parm : file	- [in]  파일 경로 (전체 경로 포함됨)
		buff	- [in]  파일의 내용이 저장될 버퍼
		size	- [in]  버퍼 크기
		mode	- [in]  0x00: Text (Ascii), 0x01: Binary
 retn : 적재된 파일 크기 / 적재 실패일 경우 0 값 반환
*/
API_IMPORT UINT32 uvCmn_LoadFile(PTCHAR file, PUINT8 buff, UINT32 size, UINT8 mode=0x01);
API_IMPORT UINT32 uvCmn_LoadFileW(PTCHAR file, PTCHAR buff, UINT32 size, UINT8 mode=0x01);
/*
 desc : 트레이 아이콘 갱신 (기존 생성된 아이콘 제거)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_RefreshTrayIcon();
/*
 desc : 기존 저장해 놓은 윈도 위치 정보로 이동
 parm : section	- [in]  레지스트리의 Section Name
		hwnd	- [in]  위치를 저장하고자 하는 자신의 윈도 핸들
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_LoadPlacement(PTCHAR section, HWND hwnd);
/*
 desc : 현재 윈도 위치 정보를 레지스트리에 저장하기
 parm : section	- [in]  레지스트리의 Section Name
		hwnd	- [in]  위치를 저장하고자 하는 자신의 윈도 핸들
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_SavePlacement(PTCHAR section, HWND hwnd);
/*
 desc : CPU에 내에 전체 스레드 개수 반환
 parm :	None
 retn : 전체 스레드 개수 값
*/
API_IMPORT UINT32 uvCmn_GetNumberOfThread();
/*
 desc : 디렉토리(폴더) 생성
 parm : path	- [in]  생성될 디렉토리 (하위 디렉토리 포함되어도 상관 없음)
		lpsa	- [in]  보안 속성 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_CreateDirectory(LPCTSTR path, LPSECURITY_ATTRIBUTES lpsa=NULL);
/*
 desc : 디렉토리 삭제 (하위 디렉토리까지 포함하여 삭제됨)
 parm : path	- [in]  제거될 디렉토리 (전체 경로를 가진 디렉토리)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_RemoveDirectory(LPCTSTR path);
/*
 desc : 바이트 버퍼마다에 비트(0 or 1)로 저장된 데이터를 Binary Bitmap Format으로 변환 및 파일로 저장
 parm : buff	- [in]  바이트 버퍼의 1 바이트 마다 비트 값이 버퍼 포인터
		width	- [in]  저장하고자 하는 Bitmap Data의 넓이
		height	- [in]  저장하고자 하는 Bitmap Data의 높이
		file	- [in]  Bitmap 파일 (전체 경로 포함)
 retn : None
*/
API_IMPORT VOID uvCmn_SaveBitsToBMP(PUINT8 buff, UINT32 width, UINT32 height, PTCHAR file);
/*
 desc : Bits 데이터를 Bytes 데이터로 변환 즉, 각 8개의 비트 데이터를 1 바이트 값에 병합
 parm : bit_width	- [in]  Bits 이미지의 넓이 (해상도 값. 비트 개수가 아님)
		bit_height	- [in]  Bits 이미지의 높이 (해상도 값. 비트 개수가 이님)
		bit_data	- [in]  Bits 데이터가 저장된 포인터
		byte_data	- [in]  Bytes 데이터가 저장될 버퍼 포인터
		byte_size	- [out] 'byte_data'의 버퍼 크기 및 [out] 반환될 때 'byte_data'에 실제로 저장된 데이터 크기
 retn : None
*/
API_IMPORT VOID uvCmn_ConvertBit2Byte(UINT32 bit_width, UINT32 bit_height, PUINT8 bit_data,
									  PUINT8 byte_data, UINT32 &byte_size);
/*
 desc : Bytes 데이터를 Bits 데이터로 변환 즉, 각 1 바이트 내에 저장된 8 개의 비트 데이터를  각각 1 바이트 버퍼에 나누어서 저장
 parm : byte_width	- [in]  Bytes 이미지의 넓이 (해상도 값. 비트 개수가 아님)
		byte_height	- [in]  Bytes 이미지의 높이 (해상도 값. 비트 개수가 이님)
		byte_data	- [in]  1 Bytes에 8개의 비트 데이터가 저장된 포인터
		bit_data	- [in]  1 Bytes 내에 각 8 개의 Bit 데이터들을 8 바이트 버퍼에 저장하기 위한 버퍼
 retn : None
*/
API_IMPORT VOID uvCmn_ConvertByte2Bit(UINT32 byte_width, UINT32 byte_height, PUINT8 byte_data,
									  PUINT8 bit_data);
/*
 desc : 바이트 버퍼의 각 8비트(0 or 1)씩 저장된 데이터를 Binary Bitmap Format으로 변환 및 파일로 저장
 parm : buff	- [in]  바이트 버퍼의 1 바이트 마다 비트 값이 버퍼 포인터
		width	- [in]  저장하고자 하는 Bitmap Data의 넓이
		height	- [in]  저장하고자 하는 Bitmap Data의 높이
		file	- [in]  Bitmap 파일 (전체 경로 포함)
 retn : None
*/
API_IMPORT VOID uvCmn_SaveBinaryToBMP(PUINT8 buff, UINT32 width, UINT32 height, PTCHAR file);
/*
 desc : DC 에 그려진(출력된) 이미지를 비트맵 파일로 저장
 parm : hdc		- [in]  DC Handle
		hbmp	- [in]  Bitmap Handle
		bpps	- [in]  Bit per Pixel
		width	- [in]  DC에 그려진 이미지 크기
		height	- [in]  DC에 그려진 이미지 크기
		file	- [in]  Bitmap 파일 (전체 경로 포함)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_SaveDCToBitmap(HDC hdc, HBITMAP hbmp, UINT16 bpps, UINT32 width, UINT32 height, PTCHAR file);
/*
 desc : 일반 데이터를 바이너리 파일로 저장
 parm : buff	- [in]  데이터가 저장된 버퍼 포인터
		size	- [in]  저장하고자 하는 파일의 크기
		file	- [in]  파일명 (전체 경로 포함)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_SaveBinFile(PUINT8 buff, UINT32 size, PCHAR file, UINT8 mode=0x00);
/*
 desc : 일반 데이터를 텍스트 파일로 저장
 parm : buff	- [in]  데이터가 저장된 버퍼 포인터
		size	- [in]  저장하고자 하는 파일의 크기
		file	- [in]  파일명 (전체 경로 포함)
		mode	- [in]  파일 열기 모드
						0x00 - 새로운 파일 쓰기
						0x01 - 추가해서 파일 쓰기
 retn : None
*/
API_IMPORT VOID uvCmn_SaveTxtFileA(PCHAR buff, UINT32 size, PCHAR file, UINT8 mode=0x00);
API_IMPORT VOID uvCmn_SaveTxtFileW(PTCHAR buff, UINT32 size, PTCHAR file, UINT8 mode=0x00);
/*
 desc : 해당 소켓 핸들에 대한 송/수신 포트의 버퍼를 초기화 (리셋) 한다.
 parm : sd	- [in]  소켓 기술자 핸들
 retn : None
*/
API_IMPORT VOID uvCmn_ResetPortSendRecvBuff(SOCKET sd);
/*
 desc : 파일의 내용을 읽어서 값 (문자열)을 얻어 온다 (GetPrivateProfileString 대체 함수)
 parm : file	- [in]  툴팁 내용이 저장된 파일 (전체 경로 포함)
		subj	- [in]  검색 대상 (Subject)
		tid		- [in]  대상 컨트롤 혹은 메뉴 ID
		desc	- [in]  툴팁 내용이 저장될 반환 버퍼
		size	- [in]  'desc' 버퍼 크기
 retn : TRUE - 반환 성공, FALSE - 반환 실패
*/
API_IMPORT BOOL uvCmn_GetPrivateProfileString(PTCHAR file, PTCHAR subj, PTCHAR tid,
											  PTCHAR desc, UINT32 size);
/*
 desc : 이미지를 상하(위에서 아래)로 뒤집기 (1바이트에 8개의 데이터가 저장된 정보를 다룬다)
 parm : cx, cy	- [in]  이미지의 크기 (가로/세로 길이)
		source	- [in]  회전 시킬 원본 이미지가 저장된 버퍼 포인터
		target	- [out] 회전된 이미지가 저장될 버퍼 포인터
 retn : None
*/
API_IMPORT VOID uvCmn_ImageConvFlip(UINT32 cx, UINT32 cy, PUINT8 source, PUINT8 target);
/*
 desc : 현재 프로세스의 메모리 사용량 확인
 parm : None
 retn : 사용량 반환 (단위: Bytes)
*/
API_IMPORT UINT64 uvCmn_GetProcessMemoryUsage();
/*
 desc : 현재 프로세스의 메모리 사용량 확인
 parm : type	- [in]  0x00 : Bytes, 0x01 : KBytes, 0x02 : MBytes, 0x03 : GBytes
 retn : 사용량 반환 (단위: Bytes)
*/
API_IMPORT DOUBLE uvCmn_GetProcessMemoryUsageSize(UINT8 type=0x00);
/*
 desc : 비트맵 이미지를 저장할 때, 실제로 저장되는 데이터의 크기 값 반환 (단위: Bytes)
 parm : cx, cy	- [in]  이미지의 넓이 / 높이 (단위: Pixel)
 retn : 크기 값 반환 (단위: Bytes)
*/
API_IMPORT UINT64 uvCmn_GetBitmapSizeOfBytes(UINT32 cx, UINT32 cy);
/*
 desc : 이미지 (1 bits 비트맵)의 크기 (단위: Bytes) 값 반환
 parm : width	- [in]  이미지의 넓이 (단위: 픽셀)
		height	- [in]  이미지의 높이 (단위: 픽셀)
 retn : 크기 반환 (단위: Bytes)
*/
API_IMPORT UINT32 uvCmn_GetBitmapImageBytes(UINT32 width, UINT32 height);
/*
 desc : 현재 프로세스의 실행 파일명 반환
 parm : path_name	- [out] 실행 파일 경로가 저장된 경로 반환
		path_size	- [in]  'path_name'의 버퍼 크기
		file_name	- [out] 실행 파일 이름(확장자 포함) 반환
		file_size	- [in]  'file_name'의 버퍼 크기
 retn : None
*/
API_IMPORT VOID uvCmn_GetProcesssName(PTCHAR path_name, UINT32 path_size,
									  PTCHAR file_name, UINT32 file_size);
/*
 desc : 현재 프로세스의 경로 반환
 parm : path_name	- [out] 실행 파일 경로가 저장된 경로 반환
		path_size	- [in]  'path_name'의 버퍼 크기
 retn : None
*/
API_IMPORT VOID uvCmn_GetProcesssPath(PTCHAR path_name, UINT32 path_size);
/*
 desc : memset 대체 함수 (속도가 더 빠를지 몰라서 ...)
 parm : dest	- [in]  초기화 대상 버퍼 포인터
		count	- [in]  초기화 개수
 retn : None
*/
API_IMPORT VOID uvCmn_Memset(PVOID dest, UINT64 count);
/*
 desc : memset 대체 함수 (8 Bytes 단위로 초기화)
 parm : dest	- [in]  초기화 대상 버퍼 포인터
		count	- [in]  초기화 개수
 retn : None
*/
API_IMPORT VOID uvCmn_Memset64(PVOID dest, UINT64 count);
/*
 desc : memset 대체 함수 (SIMD_128 이용)
		SIMD(128)이고, 버퍼(dest)가 16 bytes로 Align되어 있어야만 정상 동작한다
 parm : dest	- [in]  초기화 대상 버퍼 포인터
		count	- [in]  dest 버퍼 크기를 16 bytes로 나눌 때의 개수 값
 retn : None
*/
API_IMPORT VOID uvCmn_MemsetS128(PVOID dest, UINT32 count);
/*
 desc : 중복되지 않게 랜덤 숫자 출력
 parm : sign	- [in]  양수 or 음수 값 출력 여부 (0x00 - 양수만, 0x01 - 음수 포함)
		digit	- [in]  생성되는 데이터의 10의 자릿 수 (0x00 ~ 0x0a)
 retn : 난수값 반환
*/
API_IMPORT INT32 uvCmn_GetRandNumerI32(UINT8 sign, UINT8 digit);
/*
 desc : 중복되지 않게 랜덤 숫자 출력 (특정 범위까지 생성 가능하도록 설정)
 parm : range_min	- [in]  랜덤 발생될 최소 값
		range_max	- [in]  랜덤 발생될 최대 값
 retn : 난수값 반환
*/
API_IMPORT INT32 uvCmn_GetRandRangeNumerI32(INT32 range_min, INT32 range_max);
/*
/*
 desc : 중복되지 않게 랜덤 문자 출력
 parm : type	- [in]  0x00 - 대문자, 0x01 - 소문자
		buff	- [out] 난문자가 저장될 버퍼
		size	- [in]  'buff'의 크기, 최소 크기는 2 입니다.
 retn : None
*/
API_IMPORT VOID uvCmn_GetRandString(UINT8 type, PCHAR buff, UINT32 size);
/*
 desc : micro 단위까지 wait 시킴
 parm : usec	- [in]  대기(Sleep) 하고자 하는 시간 (단위: micro-second = 1 / 1000000 초)
 retn : None
*/
API_IMPORT VOID uvCmn_uSleep(UINT64 usec);
/*
 desc : 현재 윈도에서 발생된 메시지 큐 모두 비우기
 parm : hwnd	- [in]  메시지 큐를 비울 윈도 핸들 (보통 NULL)
		first	- [in]  이벤트 메시지 시작 값 (보통, WM_USER+..., WM_KEYFIRST, WM_MOUSEFIRST, ...)
		last	- [in]  이벤트 메시지 종료 값 (보통, WM_USER+..., WM_KEYLAST, WM_MOUSELAST, ...)
 retn : None
 note : 모두 비울려면, first=0, last=0 값 설정하면 된다.
*/
API_IMPORT VOID uvCmn_ClearMessageQueue(HWND hwnd, UINT32 first, UINT32 last);
/*
 desc : 현재 윈도에서 발생되는 메시지 처리하기
 parm : None
 retn : None
*/
API_IMPORT VOID uvCmn_DoPeekMessageQueue();
/*
 desc : 내부적으로 Event Message를 처리하면서, 일정 시간 동안 대기
 parm : wait	- [in]  대기 시간 (단위: 마이크로 초)
 retn : None
*/
API_IMPORT VOID uvCmn_WaitPeekMessageQueue(UINT64 wait);
/*
 desc : 윈도 OS 버전 얻어오기
 parm : major	- [out] Major version 반환 참조 변수
		minor	- [out] Minor version 반환 참조 변수
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_GetOSVer(UINT32 &major, UINT32 &minor);
/*
 desc : 현재 실행 중인 자신의 프로세스 버전 알아오기
 parm : major1,2- [out] Major Version
		minor1,2- [out] Minor Version
 retn : None
*/
API_IMPORT VOID uvCmn_GetRunProcVerion(UINT32 &major1, UINT32 &major2,
									   UINT32 &minor1, UINT32 &minor2);
/*
 desc : CPU의 캐시라인의 크기 얻기 (1차 캐시?)
 parm : None
 retn : 캐시 라인 크기 반환 (단위: bytes). 실패인 경우 0 값 반환
*/
API_IMPORT UINT32 uvCmn_GetCPUCacheLineSize();
/*
 desc : 클립보드에 복사된 정보를 임시 저장해 둔다
 parm : data	- [in]  클립보드에 복사하려는 데이터가 저장된 포인터
		size	- [in]  'data' 버퍼에 저장된 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_CopyToClipboard(PTCHAR data, UINT32 size);
/*
 desc : COM 객체 초기화
 parm : mode	- [in]  COINIT 열거형 값으로서, OR 연산이 가능하다.
						즉, 열거형 COINIT 참조
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_CoInitializeEx(UINT8 mode);
/*
 desc : COM 객체 해제
 parm : None
 retn : None
*/
API_IMPORT VOID uvCmn_CoUninitialize();
/*
 desc : COM 객체 리셋 (기존 설정된 COM 객체 기능 정지)
 parm : mode	- [in]  COINIT 열거형 값으로서, COINIT_MULTITREADED or CONIIT_APARTMENTTHREAD
						둘 중에 하나 값만 입력돼야 한다
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_ResetCoInitializeEx(UINT8 mode);
/*
 desc : 엑셀에서 셀(Cell)의 위치인 행(Row)과 열(Col) 숫자를 Cell 이름으로 변환하기
		(최대 'AAAA'와 같이 4글자 컬럼명까지 변환 가능하다)
 parm : col	- [in]  Cell이 위치한 Column Index (1 based)
		row	- [in]  Cell이 위치한 Row Index (1 based)
		name- [out] Cell의 위치를 실제 엑셀의 Cell Name으로 반환
		size- [in]  'name' 버퍼 크기
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_ExcelCellName(UINT32 col, UINT32 row, PTCHAR name, UINT32 size);
/*
 desc : 현재 실행 파일 작업 경로 기준으로 출력 후, 사용자에 의해 선택된 폴더(경로)명 반환
 parm : hwnd	- [in]  자신의 윈도 핸들
		path	- [out] 선택된 폴더(경로)가 저장될 버퍼
 retn : TRUE or FALSE (폴더 선택하지 않음. 취소 버튼 누른 경우)
*/
API_IMPORT BOOL uvCmn_GetSelectPathEx(HWND hwnd, PTCHAR path);
/*
 desc : 윈도 기준으로 다이얼로그 대화상자 출력 후, 사용자에 의해 선택된 폴더(경로)명 반환
 parm : hwnd	- [in]  자신의 윈도 핸들
		path	- [out] 선택된 폴더(경로)가 저장될 버퍼
 retn : TRUE or FALSE (폴더 선택하지 않음. 취소 버튼 누른 경우)
*/
API_IMPORT BOOL uvCmn_GetSelectPath(HWND hwnd, PTCHAR path);
/*
 desc : 사용자 지정 폴더 (드라이브) 기준으로 초기 대화 상자 출력하고, 사용자에 의해 선택된 폴더(경로)명 반환
 parm : hwnd	- [in]  자신의 윈도 핸들
		init	- [in]  초기 선택된 위치로 이동될 디렉토리 명 (전체 경로 포함)
		path	- [out] 선택된 폴더(경로)가 저장될 버퍼
 retn : TRUE or FALSE (폴더 선택하지 않음. 취소 버튼 누른 경우)
*/
API_IMPORT BOOL uvCmn_RootSelectPath(HWND hwnd, PTCHAR init, PTCHAR path);
/*
 desc : Returns the selected file or files
 parm : filter	- [in]  Filter string to search for. (Must keep this format)
						ex> Image Files (*.bmp, *.gif, *.jpg) | *.bmp;*.gif;*.jpg
						    Image File (*.bmp) | *.bmp
		size	- [in]  The size of 'file'
						Size of allocated memory when multiple files are stored
		file	- [out] Buffer which the selected file(or files) will be stored
		title	- [in]  The title name of dialog box
		init	- [in]  The initial directory that you want to open
 retn : TRUE or FALSE (파일 선택하지 않음. 취소 버튼 누른 경우)
*/
API_IMPORT BOOL uvCmn_GetSelectFiles(PTCHAR filter, UINT32 size, PTCHAR file, PTCHAR title, PTCHAR init);
API_IMPORT BOOL uvCmn_GetSelectFile(PTCHAR filter, UINT32 size, PTCHAR file, PTCHAR title, PTCHAR init);
/*
 desc : 나머지 연산 빠르게 처리하기 (함수 내부에 구현하는 것이 훨씬 빠름. 참고용)
 parm : val	- [in]  나누는 대상 값
		div	- [in]  나누는 값
 retn : 나머지 값
*/
API_IMPORT UINT32 uvCmn_DivideRemain(UINT32 val, UINT32 div);
/*
 desc : 특정 데이터의 위치 반환
 parm : buff	- [in]  검색 대상이 저장된 문자열 버퍼
		size	- [in]  'buff'에 저장된 문자열의 크기
		ch		- [in]  검색 문자 값
 retn : 0xffffffff - 검색 실패, 0 이상 값 - 검색 성공
*/
#ifdef _UNICODE
API_IMPORT INT32 uvCmn_GetFindCh(PTCHAR buff, INT32 size, TCHAR ch);
#else
API_IMPORT INT32 uvCmn_GetFindCh(CHAR *buff, INT32 size, CHAR ch);
#endif
/*
 desc : 데이터 (문자열)에서 검색 문자가 몇 개 존재하는지 반환
 parm : buff	- [in]  검색 대상이 저장된 문자열 버퍼
		size	- [in]  'buff'에 저장된 문자열의 크기
		ch		- [in]  검색 문자 값
 retn : 0 - 검색 실패, 0 이상 값 - 검색 성공 (검색된 문자 개수)
*/
#ifdef _UNICODE
API_IMPORT UINT32 uvCmn_GetFindChCount(PTCHAR buff, UINT32 size, TCHAR ch);
#else
API_IMPORT UINT32 uvCmn_GetFindChCount(CHAR *buff, UINT32 size, CHAR ch);
#endif
/*
 desc : 시스템의 메모리 현황 값 반환
 parm : mem	- [out] 메모리 정보가 저장되는 구조체 참조 포인터
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_GetMemStatus(MEMORYSTATUSEX &mem);
/*
 desc : 하드 디스크 용량 정보 반환 (단위: Bytes)
 parm : total	- [out] 총 용량 반환
		free	- [out] 남아 있는 용량 반환
		drive	- [in]  드라이브 명칭 (루트 이름 즉, "C:\\" or "D:\\", ...)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_GetHDDInfo(UINT64 &total, UINT64 &free, PTCHAR drive);
/*
 desc : 시리얼 통신 포트의 연결 속도와 데이터의 크기에 따라,
		패킷을 송신 or 수신되는데 걸리는 최소 시간 값 반환 (단위: 밀리초)
 parm : baud	- [in]  통신 포트의 속도 값
		size	- [in]  시리얼 통신 포트로 송신 or 수신되는 패킷의 크기 (단위: bytes)
 retn : 통신 포트의 속도 (baud rate)와 패킷의 크기에 따라, 송신 or 수신이 완료되는 시간 값 반환
		반환되는 시간 값의 단위는 msec (밀리초)
*/
API_IMPORT UINT32 uvCmn_WaitCommProcTime(UINT32 baud, UINT32 size);
/*
 desc : Check Sum 값 계산 및 문자열 Check Sum 값 반환
 parm : data	- [in]  송신될 패킷이 저장된 버퍼
		size	- [in]  'data' 버퍼에 저장된 데이터의 크기 (Check Sum 크기 부분이 제외된 길이)
 retn : check sum byte (256 modula)
*/
API_IMPORT UINT8 uvCmn_GetCheckSum256(PUINT8 data, UINT32 size);
/*
 desc : 시리얼 통신 포트가 유효한지 즉, Open 가능한지 여부 확인
 parm : port	- [in]  시리얼 통신 포트 번호
 반환: TRUE (유효함) or FALSE (열기 불가능함)
*/
API_IMPORT BOOL uvCmn_IsSerialOpen(UINT8 port);
/*
 desc : 16진수 문자를 16진수 숫자로 변환
 parm : ch	- [in]  16진수 문자 값
 retn : 16진수 숫자 값 반환, 실패인 경우 0x00 반환
*/
API_IMPORT UINT8 uvCmn_GetHexCharToNum(CHAR ch);
/*
 desc : 16진수 숫자를 16진수 문자로 변환 (대소문자 구분 안하고, 무조건 소문자 처리)
 parm : num	- [in]  16진수 숫자 값
 retn : 16진수 문자 값 반환. 실패인 경우 0x00
*/
API_IMPORT UINT8 uvCmn_GetHexNumToChar(UINT8 num);
/*
 desc : 라디안 값을 각도로 변환
 parm : radian	- [in]  라디안 값
 retn : 각도 값 반환
*/
API_IMPORT DOUBLE uvCmn_GetRad2Deg(DOUBLE radian);
/*
 desc : 각도 값을 라디안으로 변환
 parm : degree	- [in]  각도 값
 retn : 각도 값 반환
*/
API_IMPORT DOUBLE uvCmn_GetDeg2Rad(DOUBLE degree);
/*
 desc : 송/수신 패킷 데이터를 문자열 메시지로 변환
 parm : mesg	- [out] 메시지 본문 정보가 저장될 버퍼 포인터
		mlen	- [in]  'mesg' 버퍼 크기
		data	- [in]  송신될 or 수신된 메시지가 저장된 버퍼 포인터
		dlen	- [in]  'data' 버퍼에 저장된 데이터의 크기 (Bytes)
 retn : 전체 생성된 데이터의 길이 반환
*/
API_IMPORT UINT32 uvCmn_SetMakePktToMesg(PCHAR mesg, UINT32 mlen, PUINT8 data, UINT32 dlen);
/*
 desc : 시스템 로그오프 or 셧다운 or 재부팅
 parm : flag	- [in]  0x00 - logoff, 0x01 - shutdown, 0x02 - reboot
 retn : None
*/
API_IMPORT VOID uvCmn_SystemControl(UINT8 flag);
/*
 desc : 특정 서비스 동작 중지
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_StopService(PTCHAR service_name);
/*
 desc : 특정 서비스 동작 시작
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_StartService(PTCHAR service_name);
/*
 desc : Service 재시작 (멈춤 실행 -> 멈춤 확인 -> 다시 실행)
 parm : None
 retn : TRUE or FALSE
 note : 반드시 시스템이 Administrator 권한으로 로그인되어 있어야 됨
*/
API_IMPORT BOOL uvCmn_RestartService(PTCHAR service_name);
/*
 desc : 특정 서비스 동작 여부
 parm : status	- [in]  0x00 - Start, 0x01 - Stop, 0x02 - Pause
 retn : TRUE (실행 중) or FALSE (실행 중 아님)
*/
API_IMPORT BOOL uvCmn_IsServiceStatus(PTCHAR service_name, UINT8 status);
/*
 desc : 현재 시간 (밀리초)을 시/분/초 로 변경 후 반환
 parm : hour	- [out] 시 반환
		minute	- [out] 분 반환
		second	- [out] 초 반환
 retn : None
*/
API_IMPORT VOID uvCmn_GetTimeToHourMinSec(UINT32 &hour, UINT8 &minute, UINT8 &second);
/*
 desc : 주어진 입력 시간 (단위: 밀리초)을 조건에 따라 시, 분, 초, 밀리초 로 구분하여 반호나
 parm : time	- [in]  입력 시간 (단위: 밀리초. msec. 1/1000 sec)
		type	- [in]  0x00:hour, 0x01:minute, 0x02:second, 0x03:milli-second
 retn : 시간 값 반환
*/
API_IMPORT UINT8 uvCmn_GetTimeToType(UINT64 time, UINT8 type);
/*
 desc : 프로세스 ID로 윈도 핸들 얻기
 parm : pid	- [in]  프로세스 ID
 retn : 윈도 핸들 값 반환 or NULL
*/
API_IMPORT HWND uvCmn_GetWindowHwnd(UINT64 pid);
/*
 desc : 프로세스 이름으로 검색 후 해당 프로그램 강제 종료
 parm : proc_name	- [in]  확장자가 포함된 프로세스 이름
 retn : None
*/
API_IMPORT VOID uvCmn_KillProcess(PTCHAR proc_name);
/*
 desc : 일정 날짜 기준으로 검색 후 날짜 조건에 맞는 파일 삭제
 parm : path	- [in]  삭제하려는 파일들이 저장되어 있는 폴더 (경로)
		days	- [in]  현재 (오늘) 기준으로 days 값(일)이 지난 파일만 제거
						만약 값이 0 값이면, 무조건 다 지운다
 retn : None
*/
API_IMPORT VOID uvCmn_DeleteHistoryFiles(PTCHAR path, UINT32 days);
/*
 desc : 두 좌표 지점을 이용한 회전 각도 구하기
 parm : x1, y1	- [in]  첫번째 지점의 X/Y 좌표
		x2, y2	- [in]  두번째 지점의 X/Y 좌표
 retn : 두 지점 간의 대각선을 구성하는 각도 값 구하기 (라디안 값이 아닌 실제 각도)
*/
API_IMPORT DOUBLE uvCmn_GetCoordToAngle(INT64 x1, INT64 y1, INT64 x2, INT64 y2);
/*
 desc : RC4를 사용한 암호화
 parm : key_data	- [in]  암호화 하는데 사용되는 키 버퍼
		key_size	- [in]  암호화 하는데 사용되는 키 버퍼의 크기
		plain_data	- [in]  암호화 하는데 사용되는 원문
		plain_size	- [in]  암호화 하는데 사용되는 원문의 크기
		cipher_data	- [out] 암호화될 문장이 저장될 버퍼(원문의 크기 이상이어야 함)
 retn : 0 or Error Value
 note : 모든 버퍼는 문자열이 아닌 바이트 스트림으로 간주해야 함
*/
API_IMPORT UINT32 uvCmn_Encrypt(PUCHAR key_data, UINT32 key_size,
								PUCHAR plain_data, UINT32 plain_size, PUCHAR cipher_data);
/*
 desc : RC4를 사용한 복호화
 parm : key_data	- [in]  복호화 하는데 사용되는 키 버퍼
		key_size	- [in]  복호화 하는데 사용되는 키 버퍼의 크기
		cipher_data	- [in]  암호화된 문장이 저장된 버퍼
		cipher_size	- [in]  암호화된 원문의 크기
		plain_data	- [in]  암호가 해독될 원문 버퍼 (암호화된 버퍼 크기 이상이어야 함) 
 retn : 0 or Error Value
 note : 모든 버퍼는 문자열이 아닌 바이트 스트림으로 간주해야 함
*/
API_IMPORT UINT32 uvCmn_Decrypt(PUCHAR key_data, UINT32 key_size,
								PUCHAR cipher_data, UINT32 cipher_size, PUCHAR plain_data);
/*
 desc : 강제로 파일 삭제 (읽기 전용 모드 해제 후)
 parm : file	- [in]  삭제하고자 하는 파일 (전체 경로 포함)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_DeleteForceFile(PTCHAR file);
/*
 desc : 다각형의 무게 중심 구하기
 parm : poly	- [in]  다각형의 좌표가 저장되어 있는 벡터 리스트
		cent	- [out] 중심 좌표 X/Y 반환
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_GetPolyCentXY(std::vector <STG_DBXY> poly, STG_DBXY &cent);
/*
 desc : 10 진수를 2 진수로 변환 후 문자열 (버퍼)로 저장
 parm : dec_num	- [in]  2 진수로 변환하고자 하는 10 진수 값
		bin_num - [out] 2 진수로 변환된 데이터가 저장될 0 or 1의 숫자만 저장하기 위한 버퍼
						(충분히 커야 됨. 최소 32 ~ 255)
		bin_size- [in]  'bin_str' 버퍼의 크기
 retn : bin_size 값이 32 ~ 256 사이 값이 아니면 FALSE 반환
*/
API_IMPORT BOOL uvCmn_DecimalNumToBinaryStr(UINT64 dec_num, PUINT8 bin_num, UINT8 bin_size);
/*
 desc : 특정 폴더 내의 모든 파일 삭제
 parm : path	- [in]  삭제하고자 하는 폴더 (전체 경로 포함)
 retn : 삭제된 파일 개수 반환
*/
API_IMPORT UINT32 uvCmn_DeleteAllFiles(PTCHAR path);
/*
 desc : 특정 소수점 자릿수 까지만 남겨 놓고 무조건 버리기
 parm : value	- [in]  원래 실수형 값
		digit	- [in]  최종 출력되는 소숫점 자릿수
 retn : 결과 값 반환
*/
API_IMPORT DOUBLE uvCmn_RoundTrunk(DOUBLE value, UINT32 digit);
/*
 desc : 특정 중심 좌표에서 임의 각도 만큼 회전 했을 경우, 회전 후 좌표 값 반환
 parm : cen_x,cen_y	- [in]  회전 중심 좌표 (단위: 픽셀)
		org_x,org_y	- [in]  회전 대상 좌표 (단위: 픽셀)
		degree		- [in]  회전 각도 (내부적으로 라디안 값으로 변경 됨)
		rot_x,rot_y	- [out] 회전 이후 좌표 (단위: 픽셀)
 retn : None
*/
API_IMPORT VOID uvCmn_RotateCoord(DOUBLE cen_x, DOUBLE cen_y,
								  DOUBLE org_x, DOUBLE org_y,
								  DOUBLE degree,
								  DOUBLE &rot_x, DOUBLE &rot_y);
/*
 desc : 사각형의 중심에서 일정 각도 기준으로 회전 후 크기 반환
 parm : degree	- [in]  회전 각도 (내부적으로 라디안 값으로 변경 됨)
		size	- [out] 사각형의 넓이 (픽셀) -> 회전 후 넓이/높이 값 반환
 retn : None
*/
API_IMPORT VOID uvCmn_RotateRectSize(DOUBLE degree, CSize &size);
/*
 desc : CEdit 컨트롤 (MULTI-LINE, CARRAGE-RETRUN)에 문자열 추가 (기존 내용 살리면서...)
 parm : edit	- [in]  CEditCtrl 객체 포인터
		mesg	- [in]  추가 문자열 버퍼
		clean	- [in]  기존 문자열 지우기 여부
		enter	- [in]  Carriage Return (Line Feed) 값 적용 여부
		scroll	- [in]  0x00 - Not used
						0x01 - 맨 마지작 위치로 Scroll 이동
 retn : None
*/
API_IMPORT VOID uvCmn_AppendEditMesg(CEdit *edit, PTCHAR mesg, BOOL clean=FALSE, BOOL enter=FALSE, UINT8 scroll=0x00);
/*
 desc : Converting CComBSTR to TCHAR
 parm : source	- [in]  CComBSTR String
		target	- [out] TCHAR String ('source' 버퍼보다 무조건 큰 공간을 가지고 있어야 됨)
		t_size	- [in]  'target' buffer size
 retn : TRUE or FALSE (Target Buffer 크기가 너무 작은 경우)
*/
API_IMPORT BOOL uvCmn_CComBSTR2TCHAR(BSTR source, PTCHAR target, UINT32 size);
/*
 desc : 리스트 컨트롤의 특정 행 위치로 스크롤 이동 
 parm : ctrl	- [in]  ListControl Handle
		pos		- [in]  이동 위치 (Zero-based)
		select	- [in]  'pos' 위치의 항목 선택 여부
 retn : None
*/
API_IMPORT VOID uvCmn_MoveScrollPosList(HWND ctrl, UINT32 pos, BOOL select=FALSE);
/*
desc : 문자열 중에 공백 문자가 포함되어 있는지 여부 확인
parm : str	- [in]  문자열
retn : TRUE (한개 이상 공백 문자가 포함되어 있다), FALSE (포함되어 있지 않다)
참고 : 공백 문자 조건은 space, tab(\t), return(\n), carrage (\r)
*/
API_IMPORT BOOL uvCmn_IsSpaceInString(PTCHAR str);
/*
 desc : 프로세스 이름으로 검색 후 해당 프로그램 강제 종료
 parm : proc_name	- [in]  경로 및 확장자가 포함된 프로세스 이름
		wait_time	- [in]  프로그램 실행 후 정상적으로 로드 될 때까지 대기 시간 (단위: msec)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_RunProcess(PTCHAR proc_name, UINT32 wait_time=1000);
/*
 desc : 프로세스 이름으로 검색 후 존재 여부
 parm : proc_name	- [in]  확장자가 포함된 프로세스 이름
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_FindProcess(PTCHAR proc_name);
/*
 desc : 2개 이상의 모니터를 사용하는 경우 Sub (보조) 모니터로 프로그램 이동
 parm : hwnd	- [in]  윈도 핸들
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MoveSubMonitor(HWND hwnd);
/*
 desc : 지정한 경로의 폴더 열기
 parm : path	- [in]  열고자 하는 경로 명 (전체 경로 포함)
 retn : None
*/
API_IMPORT VOID uvCmn_OpenPath(PTCHAR path);
/*
 desc : 비트맵 이미지 상하 반전 시키기
 parm : width	- [in]  이미지 크기 (넓이. Pixel)
		height	- [in]  이미지 크기 (높이. Pixel)
		image	- [out] 이미지 데이터가 들어있는 버퍼 / 반전(상하)된 이미지 데이터 저장
 retn : None
*/
API_IMPORT VOID uvCmn_BmpDataUpDownChange(UINT32 width, UINT32 height, PUINT8 image);
/*
 desc : 입력된 숫자 값에 대한 8 의 배수 값으로 변환 후 반환
 parm : val	- [in]  입력된 값
 retn : 8의 배수로 변경된 값
*/
API_IMPORT UINT64 uvCmn_GetMultiOf8(UINT64 val);

/* --------------------------------------------------------------------------------------------- */
/*                                        CxImage Library                                        */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : CxImage Library를 사용하여 Bitmap File 저장
 parm : file	- [in]  전체 경로가 포함된 파일명
		width	- [in]  이미지 크기 (넓이)
		height	- [in]  이미지 크기 (높이)
		bpp		- [in]  픽셀 크기 (8: 256, 16 : 16 bit?, 24 : 24 bit?, 32 : 32 bits?)
		line	- [in]  1 Line 당 Bytes 크기
		data	- [in]  이미지가 저장된 버퍼 포인터
		flip	- [in]  이미지를 반전(상/하) 처리하여 저장할지 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_SaveCxImageBMPFile(PTCHAR file,
										 UINT32 width, UINT32 height, UINT8 bpp, UINT32 line,
										 PUINT8 data, BOOL flip=TRUE);


#ifdef __cplusplus
}
#endif