// pch.h (or stdafx.h)
#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
// windows.h가 winsock.h를 끌어오지 못하게 (제3자 헤더 대비용)
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

#include <winsock2.h>
#include <ws2tcpip.h>   // getaddrinfo, inet_pton 등
// #include <mswsock.h>  // 필요시
#include <windows.h>
