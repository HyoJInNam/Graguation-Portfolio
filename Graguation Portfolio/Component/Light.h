#pragma once
#include "Component.h"

class Light : public Component
{
public:
	ConstantBuffer<CB_VS>* GetVertexShader();
	ConstantBuffer<CB_PS_light>* GetPixelShader();
	Light(GameObject* go);
	virtual void Container() override;
	virtual void UpdateMatrix() override;

public:
	bool Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext);
	void Draw(const XMMATRIX & viewProjectionMatrix);
	void Render();

private:
	BOOL InitializeBuffer(ID3D11Device * device);
	void RenderBuffer();

private:
	ID3D11DeviceContext * deviceContext;
	ConstantBuffer<CB_VS> cb_vs_vertexshader;
	ConstantBuffer<CB_PS_light> cb_ps_light;
	XMMATRIX worldMatrix;
	Model model;

private:
	DirectX::XMFLOAT3 lightColor;
	float lightStrength;
	float attenuation_a;
	float attenuation_b;
	float attenuation_c;
};