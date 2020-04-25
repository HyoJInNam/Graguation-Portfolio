#include "ImGuiClass.h"
#include "GameObject.h"


bool ImGuiClass::InitializeImGUI(HWND hwnd, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext)
{	//Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device.Get(), deviceContext.Get());
	ImGui::StyleColorsDark();

	selected = -1;
	return true;
}


void ImGuiClass::FrameImGUI()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiClass::RenderImGUI()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiClass::Hierarchy(vector<GameObject*>& gameobjectList)
{
	ImGui::Begin("Hierarchy");
	for (int n = 0; n < gameobjectList.size(); n++)
	{
		if (ImGui::Selectable(gameobjectList[n]->getName().c_str(), selected == n)) selected = n;
	}
	ImGui::End();
}

void ImGuiClass::Inspector(vector<GameObject*>& gameobjectList)
{
	ImGui::Begin("Inspector");
	if (ImGui::BeginTabBar("Inspector"))
	{
		if (ImGui::BeginTabItem("Inspector"))
		{
			if (selected != -1)gameobjectList[selected]->inspector();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}
