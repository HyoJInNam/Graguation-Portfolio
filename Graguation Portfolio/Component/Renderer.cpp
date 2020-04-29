#include "Renderer.h"
#include "DirectXSetup/ImGuiClass.h"
#include "GameObject.h"


Renderer::Renderer(GameObject* go)
	:Component(go)
{
}

bool Renderer::Initialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, Light* light)
{
	if (!model.Initialize(filePath, device, deviceContext, light->GetVertexShader()))
		return false;

	this->transform->SetPosition(0.0f, 0.0f, 0.0f);
	this->transform->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateMatrix();
	return true;
}
void Renderer::Container()
{
	if (ImGui::TreeNode("Renderer"))
	{
		ImGui::Text("file Name");
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

		if (GOR_file_toString.size() != GOR_file_toChar.size()) {
			for (auto file : GOR_file_toString)
				GOR_file_toChar.push_back(file.first.c_str());
		}
		ImGui::Combo("combo", &item_current, GOR_file_toChar.data(), GOR_file_toChar.size());
		ImGui::PopItemWidth();

		ImGui::TreePop();
	}
}

void Renderer::UpdateMatrix()
{
	XMFLOAT3 pos = this->transform->GetPositionFloat3();
	XMFLOAT3 rot = this->transform->GetRotationFloat3();
	XMFLOAT3 scl = this->transform->GetScaleFloat3();
	this->worldMatrix = XMMatrixScaling(scl.x, scl.y, scl.z) * XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) * XMMatrixTranslation(pos.x, pos.y, pos.z);
	this->transform->UpdateDirectionVectors();
}

void Renderer::Render(const XMMATRIX & viewProjectionMatrix)
{
	model.Draw(this->worldMatrix, viewProjectionMatrix);
}
