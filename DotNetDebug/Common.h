#pragma once

#include <utility>
#include <string>
#include <locale>
#include <vector>
#include <iostream>
#include <thread>
#include <memory>
#include <atomic>
#include <string_view>
#include <queue>
#include <functional>
#include <Windows.h>
#include <MSCorEE.h>
#include <cor.h>
#include <metahost.h>  
#include <CorDebug.h>
#include <tlhelp32.h>

//#define DEBUG

#ifdef DEBUG
	#define DEBUG_OUT(str) std::cout << str << std::endl
#else
	#define DEBUG_OUT(str)
#endif