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
	ImGui::Begin("Hierarchy", 0, ImGuiWindowFlags_NoTitleBar);

	ImGui::BeginTabBar("Hierarchy", ImGuiTabBarFlags_FittingPolicyScroll);
	ImGui::BeginTabItem("Hierarchy");
	ImGui::EndTabItem();
	ImGui::EndTabBar();

	ImGui::BeginChild("Hierarchy List");
	for (int n = 0; n < gameobjectList.size(); n++)
	{
		if (ImGui::Selectable(gameobjectList[n]->getName().c_str(), selected == n))
		{
			selected = n;
			ImGuiFile::ImGuiFileDialog::Instance()->IsOk = false;
		}
	}
	ImGui::EndChild();

	ImGui::End();
}

void ImGuiClass::Project()
{
	// open Dialog Simple
	ImGuiFile::ImGuiFileDialog::Instance()->OpenDialog("ProjectKey", "Project", ".cpp\0.h\0.hpp\0\0", ".");

	// display
	if (ImGuiFile::ImGuiFileDialog::Instance()->FileDialog("ProjectKey"))
	{
		GOR_file_toString = ImGuiFile::ImGuiFileDialog::Instance()->GetRenderableFile();
		if (ImGuiFile::ImGuiFileDialog::Instance()->IsOk)
		{
			filePathName = ImGuiFile::ImGuiFileDialog::Instance()->GetFilepathName();
			filePath_notName = ImGuiFile::ImGuiFileDialog::Instance()->GetCurrentPath();
			fileName = ImGuiFile::ImGuiFileDialog::Instance()->FileNameBuffer;
			selected = -1;
		}
		// close
		ImGuiFile::ImGuiFileDialog::Instance()->CloseDialog("ProjectKey");
	}
}

void ImGuiClass::Inspector(vector<GameObject*>& gameobjectList)
{
	ImGui::Begin("Inspector", 0, ImGuiWindowFlags_NoTitleBar);

	ImGui::BeginTabBar("Inspector", ImGuiTabBarFlags_FittingPolicyScroll);
	ImGui::BeginTabItem("Inspector");

		if (selected != -1)
		{
			ImGui::BeginChild("Inspector List");
			if (selected != -1)gameobjectList[selected]->inspector();
			ImGui::EndChild();
		}
		if (ImGuiFile::ImGuiFileDialog::Instance()->IsOk)
		{
			ImGui::BeginChild("Project Selected Infomation");

			ImGui::Text("file Name");
			ImGui::SameLine();
			ImGui::Text(fileName.c_str());

			ImGui::Text("file path");
			ImGui::SameLine();
			ImGui::Text(filePath_notName.c_str());

			ImGui::Text("file Path Name");
			ImGui::SameLine();
			ImGui::Text(filePathName.c_str());

			ImGui::EndChild();
		}

	ImGui::EndTabItem();
	ImGui::EndTabBar();
	ImGui::End();
}

void ImGuiClass::DebugConsol()
{
	ImGui::Begin("Consol", 0, ImGuiWindowFlags_NoTitleBar);
	ImGui::BeginTabBar("Debug", ImGuiTabBarFlags_FittingPolicyScroll);
	ImGui::BeginTabItem("Debug");
	for (auto file : GOR_file_toString)
	{
		ImGui::Text(file.first.c_str());
	}
	ImGui::EndTabItem();
	ImGui::EndTabBar();
	ImGui::End();
}
