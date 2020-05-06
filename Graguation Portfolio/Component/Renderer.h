#pragma once
#include "Component.h"

class Light;

class Renderer : public Component
{
public:
	Renderer(GameObject* go);
	bool Initialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, Light* light);
	void Render(const XMMATRIX & viewProjectionMatrix);
	void SetTexture(const std::string & filePath);

	virtual void Container() override;
	virtual void UpdateMatrix() override;
	virtual void Destroy() override;
private:
	Model model;

	// GameObject Renderable File
	vector<const char*> GOR_file_toChar;
	int item_current = 0;

private:
	ID3D11Device * device;
	ID3D11DeviceContext * deviceContext;
	XMMATRIX worldMatrix = XMMatrixIdentity();
};