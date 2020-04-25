#pragma once
#include "COMException.h"
#include <Windows.h>

#define SAFE_DELETE(p) {if(p){delete p; p = nullptr;}}

class ErrorLogger {
public:
	static void Log(std::string message);
	static void Log(HRESULT hr, std::string message);
	static void Log(HRESULT hr, std::wstring message);
	static void Log(COMException & exception);
};