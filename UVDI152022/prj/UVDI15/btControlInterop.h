// bt_spp.h
#ifdef USEBT
// bt_spp.h

#pragma once
#include <stdint.h>
#include <windows.h>
#include <filesystem>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

// ===== 동적 로딩 =====
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
using namespace std;

struct BtSppApi
{
	enum MsgType : int
	{
		CMD = 0,
		LOG,
		UI,
		MON,
		IMG,
		END,
	};

	HMODULE h = nullptr;
	typedef void (*Spp_on_connected_t)(const char* keyUtf8, int len);
	typedef void(__cdecl* pfn_register_t)(Spp_on_connected_t cb);
	char buffer[5000] = {};

	// 함수 포인터들
	int(__cdecl* pSpp_ServerStart)();
	void(__cdecl* pSpp_ServerStop)();
	int(__cdecl* pSpp_ServerSend)(int, const char*, const char*);
	void(__cdecl* pSpp_ServerSetUIScript)(const char*);
	void(__cdecl* pSpp_onConnectedCallback)(Spp_on_connected_t cb);
	int(__cdecl* pSpp_WaitForListen)();
	int(__cdecl* pSpp_ClientStart)(const char*);
	void(__cdecl* pSpp_ClientStop)();
	int(__cdecl* pSpp_ClientSend)(const char*);

	int(__cdecl* pSpp_PollMessage)(char*, int, char*, int);
	int(__cdecl* pSpp_GetClients)(char*, int);
	int(__cdecl* pSpp_ParseMessage)(const char* inMsg, char* outBuf, int cap);
	int(__cdecl* pSpp_AddGroupbox)(char*, char*, double, double, double, double);
	int(__cdecl* pSpp_AddButton)(char*, char*, double, double, double, double);
	int(__cdecl* pSpp_AddCheckbox)(char*, char*, char*, int, double, double, double, double);
	int(__cdecl* pSpp_AddRadioButton)(char*, char*, char*, int, double, double, double, double);
	int(__cdecl* pSpp_AddLabel)(char*, char*, double, double, double, double);
	int(__cdecl* pSpp_AddInputbox)(char*, char*, double, double, double, double);
	int(__cdecl* pSpp_AddScrollbar)(char*, char*, double, double, double, double, double, double, double, double);
	int(__cdecl* pSpp_GetScript)(char*, int);
	int(__cdecl* pSpp_RefreshMonitoringValue)();
	int(__cdecl* pSpp_AddOrUpdateMonitoringValue)(char*, char*, char*);
	int(__cdecl* pSpp_AddMonitor)(double, double);
	int(__cdecl* pSpp_Bind)(const char* btnKey, const char* keysJoined);

	int(__cdecl* pSpp_AddPicturebox)(double, double);
	int(__cdecl* pSpp_UpdatePicture)(const char* recver, const unsigned char* buf, int len);

	inline std::wstring from_utf8(const char* s)
	{
		if (!s || !*s) return {};
		int wlen = MultiByteToWideChar(CP_ACP, 0, s, -1, nullptr, 0);
		std::wstring ws(wlen ? wlen - 1 : 0, L'\0'); // -1: null 제외
		if (wlen > 1) MultiByteToWideChar(CP_ACP, 0, s, -1, ws.data(), wlen);
		return ws;
	}

	static std::string to_utf8(const std::wstring& w) {
		int len = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, nullptr, 0, nullptr, nullptr);
		std::string s(len - 1, '\0');
		WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, s.data(), len, nullptr, nullptr);
		return s;
	}

	// 로드/언로드
	bool Load(const wchar_t* dllPath = L"BTspp_AOT.dll")
	{
		if (h) return true;


		wchar_t exe[MAX_PATH];
		GetModuleFileNameW(nullptr, exe, MAX_PATH);
		std::filesystem::path full = std::filesystem::path(exe).parent_path() / dllPath;



		// 안전한 검색 경로
		HMODULE mod = LoadLibraryExW(full.c_str(), nullptr,
			LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR |
			LOAD_LIBRARY_SEARCH_APPLICATION_DIR |
			LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);

		if (!mod)
		{
			DWORD ec = GetLastError();
			LPWSTR msg = nullptr;
			FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				nullptr, ec, 0, (LPWSTR)&msg, 0, nullptr);
			wprintf(L"LoadLibraryEx failed: %lu (%s)\n", ec, msg ? msg : L"");
			LocalFree(msg);

			return false;
		}

		auto gp = [&](const char* name) -> FARPROC { return GetProcAddress(mod, name); };

		pSpp_ServerStart = (decltype(pSpp_ServerStart))gp("Spp_ServerStart");
		pSpp_ServerStop = (decltype(pSpp_ServerStop))gp("Spp_ServerStop");
		pSpp_ServerSend = (decltype(pSpp_ServerSend))gp("Spp_ServerSend");
		pSpp_ServerSetUIScript = (decltype(pSpp_ServerSetUIScript))gp("Spp_ServerSetUIScript");
		pSpp_onConnectedCallback = (pfn_register_t)gp("Spp_RegisterConnectedCallback");
		pSpp_GetClients = (decltype(pSpp_GetClients))gp("Spp_GetClients");
		pSpp_ParseMessage = (decltype(pSpp_ParseMessage))gp("Spp_ParseMessage");
		pSpp_Bind = (decltype(pSpp_Bind))gp("Spp_Bind");
		pSpp_ClientStart = (decltype(pSpp_ClientStart))gp("Spp_ClientStart");
		pSpp_ClientStop = (decltype(pSpp_ClientStop))gp("Spp_ClientStop");
		pSpp_ClientSend = (decltype(pSpp_ClientSend))gp("Spp_ClientSend");

		pSpp_PollMessage = (decltype(pSpp_PollMessage))gp("Spp_PollMessage");
		pSpp_WaitForListen = (decltype(pSpp_WaitForListen))gp("Spp_WaitForListen");
		pSpp_AddButton = (decltype(pSpp_AddButton))gp("AddButton");
		pSpp_AddGroupbox = (decltype(pSpp_AddGroupbox))gp("AddGroupbox");
		pSpp_AddCheckbox = (decltype(pSpp_AddCheckbox))gp("AddCheckbox");
		pSpp_AddRadioButton = (decltype(pSpp_AddRadioButton))gp("AddRadioButton");
		pSpp_AddLabel = (decltype(pSpp_AddLabel))gp("AddLabel");
		pSpp_AddInputbox = (decltype(pSpp_AddInputbox))gp("AddInputbox");
		pSpp_AddScrollbar = (decltype(pSpp_AddScrollbar))gp("AddScrollbar");

		pSpp_RefreshMonitoringValue = (decltype(pSpp_RefreshMonitoringValue))gp("Spp_RefreshMonitoringValue");
		pSpp_AddOrUpdateMonitoringValue = (decltype(pSpp_AddOrUpdateMonitoringValue))gp("Spp_AddOrUpdateMonitoringValue");
		pSpp_AddMonitor = (decltype(pSpp_AddMonitor))gp("AddMonitor");

		pSpp_AddPicturebox = (decltype(pSpp_AddPicturebox))gp("AddPicturebox");
		pSpp_UpdatePicture = (decltype(pSpp_UpdatePicture))gp("Spp_UpdatePicture");

		if (!pSpp_ServerStart || !pSpp_ServerStop || !pSpp_ServerSend ||
			!pSpp_ClientStart || !pSpp_ClientStop || !pSpp_ClientSend ||
			!pSpp_PollMessage || !pSpp_onConnectedCallback || !pSpp_GetClients ||
			!pSpp_AddButton || !pSpp_AddCheckbox || !pSpp_AddRadioButton || !pSpp_AddLabel || !pSpp_AddInputbox || !pSpp_AddScrollbar || !pSpp_AddGroupbox ||
			!pSpp_ServerSetUIScript || !pSpp_RefreshMonitoringValue || !pSpp_AddOrUpdateMonitoringValue || !pSpp_AddMonitor || !pSpp_WaitForListen || !pSpp_ParseMessage || !pSpp_Bind ||
			!pSpp_AddPicturebox || !pSpp_UpdatePicture)
		{
			FreeLibrary(mod);
			return false;
		}
		h = mod;
		return true;
	}

	void Unload()
	{
		if (h) { FreeLibrary(h); h = nullptr; }
		pSpp_ServerStart = nullptr; pSpp_ServerStop = nullptr; pSpp_ServerSend = nullptr;
		pSpp_ClientStart = nullptr; pSpp_ClientStop = nullptr; pSpp_ClientSend = nullptr;
		pSpp_PollMessage = nullptr;
		pSpp_onConnectedCallback = nullptr;
		pSpp_GetClients = nullptr;
		pSpp_AddButton = nullptr;
		pSpp_AddCheckbox = nullptr;
		pSpp_AddRadioButton = nullptr;
		pSpp_AddLabel = nullptr;
		pSpp_AddInputbox = nullptr;
		pSpp_AddScrollbar = nullptr;
		pSpp_AddGroupbox = nullptr;
		pSpp_ServerSetUIScript = nullptr;
		pSpp_RefreshMonitoringValue = nullptr;
		pSpp_AddOrUpdateMonitoringValue = nullptr;
		pSpp_AddMonitor = nullptr;
		pSpp_WaitForListen = nullptr;
		pSpp_ParseMessage = nullptr;
		pSpp_Bind = nullptr;
		pSpp_AddPicturebox = nullptr;
		pSpp_UpdatePicture = nullptr;
	}

	//편의래퍼 메서드
	int  ServerStart() { return pSpp_ServerStart ? pSpp_ServerStart() : 0; }
	void ServerStop() { if (pSpp_ServerStop)  pSpp_ServerStop(); }


	bool RegisterOnConnected(Spp_on_connected_t cb)
	{
		if (!pSpp_onConnectedCallback) return false;
		pSpp_onConnectedCallback(cb);
		return true;
	}
	int GetClients(char* outBuf, int cap)
	{
		return pSpp_GetClients ? pSpp_GetClients(outBuf, cap) : 0;
	}
	int  ClientStart(const char* token) { return pSpp_ClientStart ? pSpp_ClientStart(token) : 0; }
	void ClientStop() { if (pSpp_ClientStop)  pSpp_ClientStop(); }
	int  ClientSend(const char* m) { return pSpp_ClientSend ? pSpp_ClientSend(m) : 0; }
	int  PollMessage(char* keyOut, int keyCap, char* msgOut, int msgCap)
	{
		return pSpp_PollMessage ? pSpp_PollMessage(keyOut, keyCap, msgOut, msgCap) : 0;
	}
	inline int AddButton(const char* key, const char* caption,
		double x, double y, double w = -1, double h = -1)
	{
		return AddButtonW(from_utf8(key), from_utf8(caption), x, y, w, h);
	}

	inline int AddGroupbox(const char* key, const char* caption,
		double x, double y, double w = -1, double h = -1)
	{
		return AddGroupboxW(from_utf8(key), from_utf8(caption), x, y, w, h);
	}

	inline int AddCheckbox(const char* key, const char* caption, const char* group, bool checked,
		double x, double y, double w = -1, double h = -1)
	{
		return AddCheckboxW(from_utf8(key), from_utf8(caption), from_utf8(group),
			checked, x, y, w, h);
	}

	int ServerSend(const char* receiver, int cmdType, const char* msg)
	{
		wstring wmsg = from_utf8(msg);
		auto m8 = to_utf8(wmsg);
		return pSpp_ServerSend ? pSpp_ServerSend(cmdType, receiver, m8.data()) : 0;
	}

	inline int AddRadioButton(const char* key, const char* caption, const char* group, bool checked,
		double x, double y, double w = -1, double h = -1)
	{
		return AddRadioButtonW(from_utf8(key), from_utf8(caption), from_utf8(group),
			checked, x, y, w, h);
	}

	int AddOrUpdateMonitoringValue(const char* key, const char* value, const char* desc = nullptr)
	{
		if (!pSpp_AddOrUpdateMonitoringValue) return 0;

		return AddOrUpdateMonitoringValueW(from_utf8(key), from_utf8(value), from_utf8(desc));
	}

	inline int AddLabel(const char* key, const char* caption,
		double x, double y, double w = -1, double h = -1)
	{
		return AddLabelW(from_utf8(key), from_utf8(caption), x, y, w, h);
	}

	inline int AddInputbox(const char* key, const char* caption,
		double x, double y, double w = -1, double h = -1)
	{
		return AddInputboxW(from_utf8(key), from_utf8(caption), x, y, w, h);
	}

	inline int AddScrollbar(const char* key, const char* caption,
		double min, double max, double curValue, double tick,
		double x, double y, double w = -1, double h = -1)
	{
		return AddScrollbarW(from_utf8(key), from_utf8(caption),
			min, max, curValue, tick, x, y, w, h);
	}


	inline int AddGroupboxW(const std::wstring& key, const std::wstring& caption, double x, double y, double w = -1, double h = -1)
	{
		if (!pSpp_AddGroupbox)
			return 0;
		auto k8 = to_utf8(key);
		auto c8 = to_utf8(caption);
		return pSpp_AddGroupbox(k8.data(), c8.data(), x, y, w, h);
	}


	inline int AddLabelW(const std::wstring& key, const std::wstring& caption,
		double x, double y, double w = -1, double h = -1) {
		if (!pSpp_AddLabel) return 0;
		auto k8 = to_utf8(key);
		auto c8 = to_utf8(caption);
		return pSpp_AddLabel(k8.data(), c8.data(), x, y, w, h);
	}

	// 버튼
	inline int AddButtonW(const std::wstring& key, const std::wstring& caption,
		double x, double y, double w = -1, double h = -1) {
		if (!pSpp_AddButton) return 0;
		auto k8 = to_utf8(key);
		auto c8 = to_utf8(caption);
		return pSpp_AddButton(k8.data(), c8.data(), x, y, w, h);
	}

	// 체크박스
	inline int AddCheckboxW(const std::wstring& key, const std::wstring& caption,
		const std::wstring& group, bool checked, double x, double y,
		double w = -1, double h = -1) {
		if (!pSpp_AddCheckbox) return 0;
		auto k8 = to_utf8(key);
		auto c8 = to_utf8(caption);
		auto g8 = to_utf8(group);
		return pSpp_AddCheckbox(k8.data(), c8.data(), g8.data(), checked ? 1 : 0, x, y, w, h);
	}

	// 라디오
	inline int AddRadioButtonW(const std::wstring& key, const std::wstring& caption,
		const std::wstring& group, bool checked, double x, double y,
		double w = -1, double h = -1) {
		if (!pSpp_AddRadioButton) return 0;
		auto k8 = to_utf8(key);
		auto c8 = to_utf8(caption);
		auto g8 = to_utf8(group);
		return pSpp_AddRadioButton(k8.data(), c8.data(), g8.data(), checked ? 1 : 0, x, y, w, h);
	}

	// 입력박스
	inline int AddInputboxW(const std::wstring& key, const std::wstring& caption,
		double x, double y, double w = -1, double h = -1) {
		if (!pSpp_AddInputbox) return 0;
		auto k8 = to_utf8(key);
		auto c8 = to_utf8(caption);
		return pSpp_AddInputbox(k8.data(), c8.data(), x, y, w, h);
	}

	// 스크롤바
	inline int AddScrollbarW(const std::wstring& key, const std::wstring& caption,
		double min, double max, double curValue, double tick,
		double x, double y, double w = -1, double h = -1) {
		if (!pSpp_AddScrollbar) return 0;
		auto k8 = to_utf8(key);
		auto c8 = to_utf8(caption);
		return pSpp_AddScrollbar(k8.data(), c8.data(), min, max, curValue, tick, x, y, w, h);
	}

	inline int AddMonitor(double x, double y)
	{
		return pSpp_AddMonitor ? pSpp_AddMonitor(x, y) : 0;
	}

	int RefreshMonitoringValue()
	{
		return pSpp_RefreshMonitoringValue ? pSpp_RefreshMonitoringValue() : 0;
	}

	inline int AddOrUpdateMonitoringValueW(const std::wstring& key, const std::wstring& value, const std::wstring& desc = L"")
	{
		if (!pSpp_AddOrUpdateMonitoringValue) return 0;
		auto k8 = to_utf8(key);
		auto v8 = to_utf8(value);
		auto d8 = to_utf8(desc);
		return pSpp_AddOrUpdateMonitoringValue(k8.data(), v8.data(), d8.data());
	}

	inline int RefreshMonitoringValueW() { return RefreshMonitoringValue(); }

	inline bool WaitForListen()
	{
		return pSpp_WaitForListen ? (pSpp_WaitForListen() != 0) : false;
	}

	inline bool ParseMessage(const std::string& msg, int& type, std::vector<std::string>& parts)
	{
		ZeroMemory(buffer, 5000);
		int n = pSpp_ParseMessage ? pSpp_ParseMessage(msg.c_str(), buffer, sizeof(buffer)) : 0;
		if (n <= 0) return false;

		std::string result(buffer, n); // 마지막 \0 제외
		std::istringstream iss(result);
		std::string line;

		if (!std::getline(iss, line)) return false;
		type = std::stoi(line);

		parts.clear();
		while (std::getline(iss, line))
			parts.push_back(line);

		return true;
	}

	// keys 벡터를 '\n'로 합쳐서 전달
	inline bool Bind(const std::string& btnKey, const std::vector<std::string>& keys)
	{
		if (!pSpp_Bind) return false;

		std::ostringstream oss;
		for (size_t i = 0; i < keys.size(); ++i)
		{
			if (i) oss << '\n';
			oss << keys[i];
		}
		auto joined = oss.str();
		return pSpp_Bind(btnKey.c_str(), joined.c_str()) != 0;
	}

	inline int AddPicturebox(double x, double y)
	{
		return pSpp_AddPicturebox ? pSpp_AddPicturebox(x, y) : 0;
	}

	inline int UpdatePicture(const std::string& recver, const std::vector<unsigned char>& data)
	{
		return pSpp_UpdatePicture ? pSpp_UpdatePicture(recver.c_str(), data.data(), (int)data.size()) : 0;
	}

};


#endif