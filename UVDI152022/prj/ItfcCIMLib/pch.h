// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"

#import "CxValue64.dll"		raw_interfaces_only, raw_native_types, no_namespace, named_guids \
		rename("value", "CxValue")
#import "EMService.exe"		raw_interfaces_only, raw_native_types, no_namespace, named_guids \
							exclude("ICxValueDisp", "ValueType", "CompareValueOp", "IID_ICxValueDisp") \
							rename("CreateEvent","CxCreateEvent") rename("SendMessage","CxSendMessage") \
							rename("max","CxMax") rename("min","CxMin") \
							rename("value", "CxValue") \
							rename("CS_DISABLED", "CX_CS_DISABLED")

#import "CxAbsLayer.tlb"	raw_interfaces_only, raw_native_types, no_namespace, named_guids exclude("ISupportErrorInfo")
#import "CxVersionIF.tlb"	raw_interfaces_only, raw_native_types, no_namespace, named_guids exclude("ISupportErrorInfo")

#import "CCAbsLayer64.dll"	raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import "CxE39OSS64.dll"	raw_interfaces_only, raw_native_types, no_namespace, named_guids exclude("ISupportErrorInfo")
#import "CxE87CMS64.dll"	raw_interfaces_only, raw_native_types, no_namespace, named_guids
#import "CxE90ST64.dll"		raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import "CxE40PJM64.dll"	raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import "CxE94CJM64.dll"	raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import "CxE116EPT64.dll"	raw_interfaces_only, raw_native_types, no_namespace, named_guids
#import "CxE157MPT64.dll"	raw_interfaces_only, raw_native_types, no_namespace, named_guids
#if (USE_SMS_MESSAGE_LIBE)
#import "CxSms64.dll"		raw_interfaces_only, raw_native_types, no_namespace, named_guids, auto_search \
							rename("CS_DISABLED", "CX_CS_DISABLED")
#import "Messages64.dll"	raw_interfaces_only, raw_native_types, no_namespace, named_guids, auto_search
#endif

#ifdef DEBUG
#import "../../bin/debug/ItfcCIMCbkx64D.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids
#else
#import "../../bin/release/ItfcCIMCbkx64.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids
#endif

#endif //PCH_H
