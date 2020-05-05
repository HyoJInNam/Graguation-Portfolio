#pragma once
#include "Component.h"

class Light;

class Renderer : public Component
{
public:
	Renderer(GameObject* go);
	bool Initialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, Light* light);
	void Render(const XMMATRIX & viewProjectionMatrix);

	virtual void Container() override;
	virtual void UpdateMatrix() override;
	virtual void Destroy() override;

public:
	Model model;


private:
	XMMATRIX worldMatrix = XMMatrixIdentity();
	int item_current = 0;

	// GameObject Renderable File
	vector<const char*> GOR_file_toChar;
};