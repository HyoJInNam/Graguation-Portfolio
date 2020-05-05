#pragma once
#include "COMException.h"
#include <Windows.h>



#define SAFE_RELEASE(p) { if(p){ (p)->Release(); (p) = NULL; } }
#define SAFE_DELETE(p) {if(p){delete p; p = nullptr;}}
#define SAFE_DELETE_ARRAY(p) { if(p){ delete [] (p); (p) = NULL; } }
#define DELETE_VECTOR(p) {for (auto go : p) { delete go; go = nullptr; } p.clear();}

class ErrorLogger {
public:
	static void Log(std::string message);
	static void Log(HRESULT hr, std::string message);
	static void Log(HRESULT hr, std::wstring message);
	static void Log(COMException & exception);
};