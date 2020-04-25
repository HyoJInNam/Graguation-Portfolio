#pragma once
#include "GameObject.h"

class Light : public Component
{
public:
	Light(GameObject* go);
	ConstantBuffer<CB_VS_vertexshader>* GetVertexShader();

public:
	bool Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext);
	void Render(ID3D11DeviceContext * deviceContext);
	virtual void Container() override;
	virtual void UpdateMatrix() override;

private:
	BOOL InitializeBuffer(ID3D11Device * device, ID3D11DeviceContext * deviceContext);
	void RenderBuffer(ID3D11DeviceContext * deviceContext);

private:
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	ConstantBuffer<CB_PS_light> cb_ps_light;

private:
	DirectX::XMFLOAT3 lightColor;
	float lightStrength;
	float attenuation_a;
	float attenuation_b;
	float attenuation_c;
};