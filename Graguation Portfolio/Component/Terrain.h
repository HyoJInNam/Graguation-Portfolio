#pragma once
#include "Component.h"

class Light;

class Terrain : public Component
{
public:
	Terrain(GameObject* go);
	virtual void Container() override;
	virtual void UpdateMatrix() override;

public:
	bool Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, Light* light);
	void Render(const XMMATRIX & viewProjectionMatrix);

private:
	BOOL LoadHeightMap(ID3D11Device * device, std::string filename);
	BOOL InitializeBuffer(ID3D11Device * device);
	void RenderBuffer();


private:
	ID3D11DeviceContext * deviceContext;

	XMMATRIX worldMatrix;
	ConstantBuffer<CB_VS> cb_vs_vertexshader;
	ConstantBuffer<CB_PS_light> cb_ps_pixelshader;

	IndexBuffer indices;
	VertexBuffer<VertexTerrain> vertices;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

private:
	int width;
	int height;
};

