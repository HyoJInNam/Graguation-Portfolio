#pragma once
#include "AdapterReader.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_dx11.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <vector>
using namespace std;

class GameObject;


static map<std::string, std::string> GOR_file_toString;

class ImGuiClass
{
public:
	bool InitializeImGUI(HWND hwnd, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
	void FrameImGUI();
	void RenderImGUI();

public:
	void Hierarchy(vector<GameObject*>& gameobjectList);
	void Project();
	void Inspector(vector<GameObject*>& gameobjectList);
	void DebugConsol();

private:
	std::string fileName;
	std::string filePathName;
	std::string filePath_notName;

	int selected;
};