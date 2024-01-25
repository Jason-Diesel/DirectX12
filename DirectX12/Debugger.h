#pragma once
//git
#ifndef __GUICON_H__

#define __GUICON_H__

//#ifdef _DEBUG

void RedirectIOToConsole();

//#endif

#endif

#include <iostream>

#define CheckHR(x) if(x != S_OK){errorMSG(__FILE__, __LINE__);__debugbreak();}
#define breakDebug errorMSG(__FILE__, __LINE__);
//#define CheckHR(x) if(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)) != S_OK){errorMSG(__FILE__, __LINE__);__debugbreak();}

static void errorMSG(const char* file, int line)
{
	std::cout << "Error at : Line: " << line << " File " << file << std::endl;
}