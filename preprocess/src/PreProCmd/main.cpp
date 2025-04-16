#ifndef __PRE_PRO_CMD__
#define __PRE_PRO_CMD__

#include <iostream>
#include <string.h>
#include <string>

#include "PreProcessApi/PreProcessApi.h"

using std::string;

static string GetErr()
{
	char buff[2048];
    GetErrMsg(buff, sizeof(buff));
	return string(buff, strlen(buff));
}

int main(int argc, char** argv)
{
	auto buffer_size = 1024;
	auto str_buffer = new char[buffer_size];

#ifdef _WIN32
    SetDllPath(".");	// Needed if current directory for running process is different from prepro dlls
#endif
    InitGlobalLogger(); // Make global logging file in bin folder for all jobs
    Init();				// Init must be called before all other prepro API functions


	
	// Insert functions for pre-processing a given job here



	//// ----- START Example of a prepro job --------
	//auto input_gbr = "input\\Visitechlogo.gbr";
	//auto output_dir = "output";
	//auto work_dir = "output";
 //   InitGerber(input_gbr, output_dir);
 //   SetWorkDir(work_dir);
 //   SetThreads(8);
 //   SetProductID(235002);
//	SetAdvPreview(false);
//	SetErosionX(0.000600);
//	SetErosionY(0.000300);
//  SetGlobalFiducial(41);
	// Below just get the default values and set them just to show function call examples
    //SetRotationCCW(GetRotationCCW());
    //SetOffsetX(GetOffsetX());
    //SetOffsetY(GetOffsetY());
    //SetInverse(IsInverse());
    //SetMirrorH(IsMirrorH());
    //SetMirrorV(IsMirrorV());
//    SetLocalFiducial(GetLocalFiducial());
	// ----- END Example of a prepro job --------


	// Start processing the configured job and print output to console
 //   if (StartProcessing() != 0) {
 //       GetErrMsg(str_buffer, buffer_size);
	//	std::cout << str_buffer << std::endl;
	//}

	delete[] str_buffer;

}

#endif
