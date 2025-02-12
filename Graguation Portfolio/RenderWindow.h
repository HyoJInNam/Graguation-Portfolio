#pragma once
#include "Systems/ErrorLogger.h"

class WindowContainer;

class RenderWindow
{
public:
	bool Initialize(WindowContainer * pWindowContainer, HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	bool ProcessMessages();
	HWND GetHWND() const;
	~RenderWindow();

private:
	void RegisterWindowClass();

private:
	HWND handle = NULL; //Handle to this window
	HINSTANCE hInstance = NULL; //Handle to application instance

	std::string window_title = "";
	std::wstring window_title_wide = L""; //Wide string representation of window title
	std::string window_class = "";
	std::wstring window_class_wide = L""; //Wide string representation of window class name

	int width = 0;
	int height = 0;
	bool FULL_SCREEN = false;
	bool VSYNC_ENABLED = true;
};