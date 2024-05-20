#pragma once

#include <filesystem>
#include <chrono>
#include <string>
#include <chrono>
#include <fstream>
#include <atlstr.h>
#include <iostream>
#include <sstream>

using namespace std;
namespace fs = std::filesystem;



class Stuffs
{
private:
	void RemoveOldfiles(const fs::path& path , int hours) 
	{
		auto now = std::chrono::system_clock::now();
		std::chrono::hours ageLimit = std::chrono::hours(hours);

		try 
		{
			if (!fs::exists(path) || !fs::is_directory(path))
				return;
				
			for (auto& entry : fs::recursive_directory_iterator(path)) 
			{
				if (!fs::is_regular_file(entry))
					continue;
					
				auto ftime = fs::last_write_time(entry);
				auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
					ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());

				if (now - sctp < ageLimit)
					continue;

				fs::remove(entry);
			}
		}
		catch (...) {}
	}
public: 

	static Stuffs stuffUtils;
	static Stuffs& GetStuffs()
	{
		return stuffUtils;
	}

	string GetCurrentExePath()
	{
		TCHAR execPath[_MAX_PATH];
		GetModuleFileName(NULL, execPath, _MAX_PATH);
		CString path = execPath;
		auto spiltIndex = path.ReverseFind(L'\\');
		return string(CT2CA(path.Left(spiltIndex)));
	}

	void RemoveOldFiles()
	{
		RemoveOldfiles(GetCurrentExePath() + "\\save_img",1*24); //하루지난건 싹 삭제.
	}
	
	int ParseAndFillVector(const TCHAR* str, char seperator,  std::vector<double>& vec,std::vector<string> orgVec)
	{
		vec.clear();
		std::wstring temp = std::wstring(str);
		std::string utf8Str(temp.begin(), temp.end());

		std::stringstream ss(utf8Str);
		std::string token;
		int cnt = 0;
		while (std::getline(ss, token, seperator))
		{
			double val = std::stod(token);
			vec.push_back(val);
			orgVec.push_back(token);
			++cnt;
		}
		return cnt;
	}


};

	 