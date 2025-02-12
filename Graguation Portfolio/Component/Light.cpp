#include "Light.h"
#include "DirectXSetup/ImGuiClass.h"
#include "GameObject.h"



Light::Light(GameObject * go)
	:Component(go)
{
	worldMatrix = XMMatrixIdentity();
	lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	lightStrength = 1.0f;
	attenuation_a = 1.0f;
	attenuation_b = 0.1f;
	attenuation_c = 0.1f;
}

ConstantBuffer<CB_VS>* Light::GetVertexShader()
{
	return &this->cb_vs_vertexshader;
}

ConstantBuffer<CB_PS_light>* Light::GetPixelShader()
{
	return &this->cb_ps_light;
}

void Light::Container()
{
	if (ImGui::TreeNode("Light"))
	{
		ImGui::DragFloat3("Ambient Light Color", &this->cb_ps_light.data.ambientLightColor.x, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Ambient Light Strength", &this->cb_ps_light.data.ambientLightStrength, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Dynamic Light Attenuation A", &this->attenuation_a, 0.01f, 0.1f, 10.0f);
		ImGui::DragFloat("Dynamic Light Attenuation B", &this->attenuation_b, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat("Dynamic Light Attenuation C", &this->attenuation_c, 0.01f, 0.0f, 10.0f);
		ImGui::TreePop();
	}
}

bool Light::Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext)
{
	this->deviceContext = deviceContext;
	if (!InitializeBuffer(device)) 
		return false;

	if (!model.Initialize("Data/Objects/light.fbx", device, deviceContext, &this->cb_vs_vertexshader))
		return false;

	this->transform->SetPosition(0.0f, 0.0f, 0.0f);
	this->transform->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateMatrix();
	return true;
}

void Light::UpdateMatrix()
{
	XMFLOAT3 pos = this->transform->GetPositionFloat3();
	XMFLOAT3 rot = this->transform->GetRotationFloat3();
	this->worldMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) * XMMatrixTranslation(pos.x, pos.y, pos.z);
	this->transform->UpdateDirectionVectors();
}

void Light::Draw(const XMMATRIX & viewProjectionMatrix)
{
	model.Draw(this->worldMatrix, viewProjectionMatrix);
}

void Light::Render()
{
	RenderBuffer();
}

BOOL Light::InitializeBuffer(ID3D11Device * device)
{
	try
	{
		//Initialize Constant Buffer(s)
		HRESULT hr = this->cb_vs_vertexshader.Initialize(device, deviceContext);
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");

		hr = this->cb_ps_light.Initialize(device, deviceContext);
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");

		this->cb_ps_light.data.ambientLightColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
		this->cb_ps_light.data.ambientLightStrength = 1.0f;

	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

void Light::RenderBuffer()
{
	this->cb_ps_light.data.dynamicLightColor = lightColor;
	this->cb_ps_light.data.dynamicLightStrength = lightStrength;
	this->cb_ps_light.data.dynamicLightPosition = transform->GetPositionFloat3();
	this->cb_ps_light.data.dynamicLightAttenuation_a = attenuation_a;
	this->cb_ps_light.data.dynamicLightAttenuation_b = attenuation_b;
	this->cb_ps_light.data.dynamicLightAttenuation_c = attenuation_c;
	this->cb_ps_light.ApplyChanges();

	deviceContext->PSSetConstantBuffers(0, 1, this->cb_ps_light.GetAddressOf());
}
