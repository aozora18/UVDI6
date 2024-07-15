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

	std::string GetLastLineOfFile(const std::string& filePath) 
	{
		namespace fs = std::filesystem;
		
		try
		{
			if (!fs::exists(filePath))
			{
				throw std::runtime_error("File does not exist: " + filePath);
			}
			else
			{
				std::ifstream file(filePath, std::ios::in | std::ios::ate);
				if (!file.is_open())
					throw std::runtime_error("Unable to open file: " + filePath);

				std::string lastLine;
				file.seekg(-1, std::ios::end);

				if (file.tellg() == 0)
					return lastLine;

				char ch;
				while (file.tellg() > 0) 
				{
					file.get(ch);
					if (ch == '\n' && file.tellg() > 1) {
						file.seekg(-2, std::ios::cur);
					}
					else 
					{
						break;
					}
				}
				std::getline(file, lastLine);
				file.close();

				return lastLine;
			}
			
		}
		catch (...)
		{
			return nullptr;
		}
	}

	static double CutEpsilon(double v)
	{
		return (fabs(v) < 0.0009f) ? 0 : v;
	}
	//현시간을 double로 반환 
	double GetCurrentTimeToDouble()  
	{
		auto now = std::chrono::system_clock::now();
		auto in_time_t = std::chrono::system_clock::to_time_t(now);

		std::tm buf;
		localtime_s(&buf, &in_time_t);

		std::ostringstream oss;
		oss << std::put_time(&buf, "%Y%m%d%H%M");
		return std::stod(oss.str());
	}

	//double 시간의 차이를 minute로 반환 
	double TimegabForMinute(double old, double now)  
	{
		std::ostringstream oldStr, nowStr;
		oldStr << std::fixed << std::setprecision(0) << old;
		nowStr << std::fixed << std::setprecision(0) << now;

		std::tm* tm[] = { new std::tm(),new std::tm() };

		std::istringstream s1(oldStr.str());
		s1 >> std::get_time(tm[0], "%Y%m%d%H%M");

		std::istringstream s2(nowStr.str());
		s2 >> std::get_time(tm[1], "%Y%m%d%H%M");

		auto time1 = std::chrono::system_clock::from_time_t(std::mktime(const_cast<std::tm*>(tm[0])));
		auto time2 = std::chrono::system_clock::from_time_t(std::mktime(const_cast<std::tm*>(tm[1])));
		auto diff = std::chrono::duration_cast<std::chrono::minutes>(time2 - time1).count();
		delete tm[0]; delete tm[1];
		return static_cast<double>(diff);

	}

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

	void RemoveOldFiles(UINT8 bmpDelect, UINT8 logDelect)
	{
		//RemoveOldfiles(GetCurrentExePath() + "\\save_img",1*24); //하루지난건 싹 삭제.

		/*Bmp File*/
		RemoveOldfiles(GetCurrentExePath() + "\\save_img", bmpDelect * 24);
		/*Log File*/
		RemoveOldfiles(GetCurrentExePath() + "\\logs\\DlgFile", logDelect * 24);
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

	 