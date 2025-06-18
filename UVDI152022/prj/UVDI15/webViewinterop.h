#pragma once

#include <windows.h>
#include <string>
#include <numeric>

using namespace std;

using CommandCallback = void(__stdcall*)(const char*);
typedef bool(__stdcall* StartServerFunc)(int port);
typedef void(__stdcall* StopServerFunc)();
typedef bool(__stdcall* StartClientFunc)(int port);
typedef void(__stdcall* RefreshPageFunc)();
typedef void(__stdcall* UpdateMonitoringValueFunc)(const char* category, const char* key, const char* value);
typedef void(__stdcall* UpdateLogFunc)(const char* value);
typedef void(__stdcall* UpdateCmdLogFunc)(const char* cmdName, const char* cmdLog);
typedef void(__stdcall* RemoveMonitoringValueFunc)(const char* category);
typedef void(__stdcall* RegisterCommandCallbackFunc)(CommandCallback cb);
typedef void(__stdcall* AddWebCmdButtonFunc)(const char* btnTitle, const char* cmdParam);
typedef void(__stdcall* AddCategoryTextFunc)(const char* categoryText);
typedef void(__stdcall* SetWebTitleFunc)(const char* title);
typedef void(__stdcall* AddConfigCategoryFunc)(const char* name);
typedef void(__stdcall* AddConfigItemFunc)(const char* label, int count, const char** initialValues);
typedef void(__stdcall* UpdateConfigItemFunc)(const char* label, int count, const char** updatedValues);

//<이것도 차후 구현을 숨겨야할일이 생기면 dll로 뺄것>



class WebMonitor
{
protected:

	HINSTANCE hinstLib;
	StartServerFunc startServer;
	StopServerFunc stopServer;
	StartClientFunc startClient;
	RefreshPageFunc refreshPage;
	UpdateLogFunc updateLog;
	UpdateCmdLogFunc updateCmdLog;
	UpdateMonitoringValueFunc updateMonitoringValue;
	RegisterCommandCallbackFunc registerCmdCb;
	AddWebCmdButtonFunc addWebBtn;
	AddCategoryTextFunc addCategoryText;

	SetWebTitleFunc setWebTitle;
	AddConfigCategoryFunc addConfigCategory;
	AddConfigItemFunc addConfigItem;
	UpdateConfigItemFunc updateConfigItem;

	map<string, int> configParamSizemap;

	std::atomic<bool> cancelFlag = false;
	vector<string> logBuffer;
	mutex mtx;



private:
	void LoadLib()
	{
		bool bindingFailed = false;;
		hinstLib = LoadLibrary(L"webmonitordotnet_AOT.dll");
		if (hinstLib != nullptr)
		{
			startServer = (StartServerFunc)GetProcAddress(hinstLib, "StartServer");
			stopServer = (StopServerFunc)GetProcAddress(hinstLib, "StopServer");
			startClient = (StartClientFunc)GetProcAddress(hinstLib, "StartClient");
			refreshPage = (RefreshPageFunc)GetProcAddress(hinstLib, "RefreshPage");
			updateLog = (UpdateLogFunc)GetProcAddress(hinstLib, "UpdateLog");
			updateCmdLog = (UpdateCmdLogFunc)GetProcAddress(hinstLib, "UpdateCmdLog");
			updateMonitoringValue = (UpdateMonitoringValueFunc)GetProcAddress(hinstLib, "UpdateMonitoringValue");
			registerCmdCb = (RegisterCommandCallbackFunc)GetProcAddress(hinstLib, "RegisterCommandCallback");
			addWebBtn = (AddWebCmdButtonFunc)GetProcAddress(hinstLib, "AddWebCmdButton");
			addCategoryText = (AddCategoryTextFunc)GetProcAddress(hinstLib, "AddCategoryText");
			setWebTitle = (SetWebTitleFunc)GetProcAddress(hinstLib, "SetWebTitle");
			addConfigCategory = (AddConfigCategoryFunc)GetProcAddress(hinstLib, "AddConfigCategory");
			addConfigItem = (AddConfigItemFunc)GetProcAddress(hinstLib, "AddConfigItem");
			updateConfigItem = (UpdateConfigItemFunc)GetProcAddress(hinstLib, "UpdateConfigItem");


			if (startServer == nullptr ||
				stopServer == nullptr ||
				startClient == nullptr ||
				refreshPage == nullptr ||
				updateLog == nullptr ||
				updateCmdLog == nullptr ||
				updateMonitoringValue == nullptr ||
				registerCmdCb == nullptr ||
				addWebBtn == nullptr)
			{
				FreeLibrary(hinstLib);
				hinstLib = nullptr;
				bindingFailed = true;
			}
		}
	}

public:
	WebMonitor() :
		hinstLib(nullptr),
		startServer(nullptr),
		stopServer(nullptr),
		startClient(nullptr),
		refreshPage(nullptr),
		updateLog(nullptr),
		updateCmdLog(nullptr),
		updateMonitoringValue(nullptr),
		registerCmdCb(nullptr),
		addCategoryText(nullptr),
		addWebBtn(nullptr),
		setWebTitle(nullptr),
		addConfigCategory(nullptr),
		addConfigItem(nullptr),
		updateConfigItem(nullptr)
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
		if (startServer != NULL && hinstLib)
		{
			res = startServer(port);
		}
		return res;
	}

	void RegisterCommandCallback(CommandCallback cb)
	{
		if (registerCmdCb && hinstLib)
			registerCmdCb(cb);
	}

	void StopWebServer()
	{
		if (stopServer != NULL && hinstLib)
			stopServer();
	}

	bool ConnectClient(int port)
	{
		auto res = false;
		if (startClient != NULL && hinstLib)
			res = startClient(port);

		return res;
	}

	void RefreshWebPage()
	{
		if (refreshPage != NULL && hinstLib)
			refreshPage();
	}

	void UpdateCmdLog(string cmd, string cmdLog)
	{
		if (updateCmdLog != NULL && hinstLib)
			updateCmdLog(cmd.c_str(), cmdLog.c_str());
	}


	void UpdateMonitoringValue(string category, string key, string value)
	{
		if (updateMonitoringValue != NULL && hinstLib)
			updateMonitoringValue(category.c_str(), key.c_str(), value.c_str());
	}

	void UpdateLog(string value)
	{
		if (updateLog != NULL && hinstLib)
			updateLog(value.c_str());
	}

	void SetWebTitle(string title)
	{
		if (setWebTitle != NULL && hinstLib)
			setWebTitle(title.c_str());
	}

	void AddWebBtn(string btnTitle, string cmdParam)
	{
		if (addWebBtn != NULL && hinstLib)
			addWebBtn(btnTitle.c_str(), cmdParam.c_str());
	}

	void AddCategoryText(string categoryText)
	{
		if (addCategoryText != NULL && hinstLib)
			addCategoryText(categoryText.c_str());
	}

	void AddConfigCategory(string categoryText)
	{
		if (addConfigCategory != NULL && hinstLib)
			addConfigCategory(categoryText.c_str());
	}

	void AddConfigItem(string label, vector<string> initialValues)
	{
		if (configParamSizemap.end() != configParamSizemap.find(label))
			throw exception("config item already added");

		std::vector<const char*> cstrs;
		for (const auto& s : initialValues)
			cstrs.push_back(s.c_str());

		configParamSizemap.insert(pair<string, int>(label, initialValues.size()));

		addConfigItem(label.c_str(), initialValues.size(), cstrs.data());
	}

	void UpdateConfigItem(string label, vector<string> updatedValues)
	{
		auto find = configParamSizemap.find(label);

		if (configParamSizemap.end() == find)
			throw exception("no config item exited.");


		std::vector<const char*> cstrs;
		for (const auto& s : updatedValues)
			cstrs.push_back(s.c_str());

		if (find->second != updatedValues.size())
			throw exception("item count missmatched.");

		updateConfigItem(label.c_str(), updatedValues.size(), cstrs.data());
	}

};
