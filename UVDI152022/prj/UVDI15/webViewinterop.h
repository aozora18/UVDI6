#pragma once

#include <windows.h>
#include <string>
#include <numeric>

using namespace std;

typedef bool(__stdcall* StartServerFunc)(int port);
typedef void(__stdcall* StopServerFunc)();
typedef bool(__stdcall* StartClientFunc)(int port);
typedef void(__stdcall* RefreshPageFunc)();
typedef void(__stdcall* UpdateValueFunc)(const char* category, const char* key, const char* value);
typedef void(__stdcall* UpdateValueFunc2)(const char* value);
typedef void(__stdcall* ClearValueFunc)(const char* category);

//<이것도 차후 구현을 숨겨야할일이 생기면 dll로 뺄것>

 

class WebMonitor
{
protected:
	HINSTANCE hinstLib;
	StartServerFunc StartServer;
	StopServerFunc StopServer;
	StartClientFunc StartClient;
	RefreshPageFunc RefreshPage;
	UpdateValueFunc UpdateValue;
	UpdateValueFunc2 UpdateValue2;
	ClearValueFunc ClearValue;
	std::atomic<bool> cancelFlag = false;
	vector<string> logBuffer;
	mutex mtx;

	

private: 
	void LoadLib()
	{
		hinstLib = LoadLibrary(L"webmonitordotnet.dll");
		if (hinstLib != nullptr)
		{
			StartServer = (StartServerFunc)GetProcAddress(hinstLib, "StartServer");
			StopServer = (StopServerFunc)GetProcAddress(hinstLib, "StopServer");
			StartClient = (StartClientFunc)GetProcAddress(hinstLib, "StartClient");
			RefreshPage = (RefreshPageFunc)GetProcAddress(hinstLib, "UpdatePage");
			UpdateValue = (UpdateValueFunc)GetProcAddress(hinstLib, "UpdateValue");
			UpdateValue2 = (UpdateValueFunc2)GetProcAddress(hinstLib, "UpdateValue2");
			ClearValue = (ClearValueFunc)GetProcAddress(hinstLib, "Clear");

			if (StartServer == nullptr ||
				StopServer == nullptr ||
				StartClient == nullptr ||
				RefreshPage == nullptr ||
				UpdateValue == nullptr ||
				UpdateValue2 == nullptr ||
				ClearValue == nullptr)
			{
				FreeLibrary(hinstLib);
				hinstLib = nullptr;
			}
		}
	}

public:
	WebMonitor() : hinstLib(nullptr), StartServer(nullptr), StopServer(nullptr), StartClient(nullptr), RefreshPage(nullptr)
	{
		LoadLib();
	}

	~WebMonitor()
	{
		if (hinstLib != NULL)
		{
			FreeLibrary(hinstLib);
		}
		StopWebServer();
	}

	void StartWebMonitor();

	bool StartWebServer(int port)
	{
		auto res = false;
		if (StartServer != NULL && hinstLib)
		{
			res = StartServer(port);
		}
		return res;
	}

	void StopWebServer()
	{
		if (StopServer != NULL && hinstLib)
			StopServer();
	}

	bool ConnectClient(int port)
	{
		auto res = false;
		if (StartClient != NULL && hinstLib)
			res = StartClient(port);

		return res;
	}

	void RefreshWebPage()
	{
		std::lock_guard<std::mutex> lock(mtx);

		if (RefreshPage != NULL && hinstLib)
			RefreshPage();
	}

	void Update(char* category, char* key, char* value)
	{
		std::lock_guard<std::mutex> lock(mtx);
		if (UpdateValue != NULL && hinstLib)
			UpdateValue(category, key, value);
	}

public:
	void AddLog(string str)
	{
		std::lock_guard<std::mutex> lock(mtx);
		try
		{
			logBuffer.push_back(str);
			if (logBuffer.size() > 200)
				logBuffer.erase(logBuffer.begin());
		}
		catch (exception e)
		{
			int debug = 0;
		}
	}

	void AddLog(TCHAR* tStr)
	{
		std::wstring wstr = tStr;
		std::string str(wstr.begin(), wstr.end());

		std::lock_guard<std::mutex> lock(mtx);
		try
		{
			logBuffer.push_back(str);
			if (logBuffer.size() > 200)
				logBuffer.erase(logBuffer.begin());
		}
		catch (exception e)
		{
			int debug = 0;
		}
	}

	void Update()
	{
		if (logBuffer.empty()) return;
		if (mtx.try_lock() == false) 
			return;

		if (UpdateValue != NULL && hinstLib)
		{
			try
			{
				for (auto& v : logBuffer)
					UpdateValue2(v.c_str());
			}
			catch (const std::exception& e)
			{
				mtx.unlock();
			}
			logBuffer.clear();
		}

		mtx.unlock();
	}

	void Clear(char* category)
	{
		std::lock_guard<std::mutex> lock(mtx);
		if (ClearValue != NULL && hinstLib)
			ClearValue(category);
	}
};
