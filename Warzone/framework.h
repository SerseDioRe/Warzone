#pragma once

#define WIN32_LEAN_AND_MEAN             // Escludere gli elementi usati raramente dalle intestazioni di Windows
// File di intestazione di Windows
#include <windows.h>
#include <iostream>
#include <vector>

#include <Psapi.h>
#include <ctime>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>

#include <Mmsystem.h>
#pragma comment ( lib, "WinMM.lib" ) //Mmsystem

#include <sstream>
#include <iomanip>
#define STREAM(s) (((std::stringstream&)(std::stringstream() << s)).str())
