#pragma once
#include "Component.h"

class Renderer : public Component
{
public:
	Renderer(GameObject* go);
	bool Initialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader> * cb_vs_vertexshader);
	void Render(const XMMATRIX & viewProjectionMatrix);

	virtual void Container() override;
	virtual void UpdateMatrix() override;

public:
	Model model;


protected:

	XMMATRIX worldMatrix = XMMatrixIdentity();
};