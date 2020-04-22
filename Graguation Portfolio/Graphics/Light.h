#pragma once
#include "GameObject.h"
#include "DirectXSetup/ImGuiClass.h"

class Light : public Component, ImGuiEvent
{
public:
	Light(GameObject* go);
	ConstantBuffer<CB_VS_vertexshader>* GetVertexShader();

public:
	bool Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext);
	void Container();
	void Render(ID3D11DeviceContext * deviceContext);

private:
	BOOL InitializeBuffer(ID3D11Device * device, ID3D11DeviceContext * deviceContext);
	void RenderBuffer(ID3D11DeviceContext * deviceContext);

private:
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	ConstantBuffer<CB_PS_light> cb_ps_light;

private:
	DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float lightStrength = 1.0f;
	float attenuation_a = 1.0f;
	float attenuation_b = 0.1f;
	float attenuation_c = 0.1f;
};