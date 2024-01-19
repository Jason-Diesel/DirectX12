#pragma once
//git
#ifndef __GUICON_H__

#define __GUICON_H__

#ifdef _DEBUG

void RedirectIOToConsole();

#endif

#endif

#include <iostream>
#define breakDebug errorMSG(__FILE__, __LINE__);__debugbreak()

static void errorMSG(const char* file, int line)
{
	std::cout << "Error at : Line: " << line << " File " << file << std::endl;
}