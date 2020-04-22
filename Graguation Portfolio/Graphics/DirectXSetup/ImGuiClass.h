#pragma once
#include "AdapterReader.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_dx11.h>
#include <ImGui/imgui_impl_win32.h>

class ImGuiClass
{
public:
	bool InitializeImGUI(HWND hwnd, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
	void FrameImGUI();
	void RenderImGUI();
};

class ImGuiEvent
{
protected:
	virtual void Container() = 0;
};