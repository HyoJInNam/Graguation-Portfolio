#pragma once
#include "AdapterReader.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_dx11.h>
#include <ImGui/imgui_impl_win32.h>

#include <vector>
#include "../Component/GameObject.h"

class GameObject;

class ImGuiClass
{
public:
	bool InitializeImGUI(HWND hwnd, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
	void FrameImGUI();
	void RenderImGUI();

public:
	void Hierarchy(vector<GameObject*>& gameobjectList);
	void Inspector(vector<GameObject*>& gameobjectList);

private:
	int selected;
};