// pch.h (or stdafx.h)
#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
// windows.h�� winsock.h�� ������� ���ϰ� (��3�� ��� ����)
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

#include <winsock2.h>
#include <ws2tcpip.h>   // getaddrinfo, inet_pton ��
// #include <mswsock.h>  // �ʿ��
#include <windows.h>
