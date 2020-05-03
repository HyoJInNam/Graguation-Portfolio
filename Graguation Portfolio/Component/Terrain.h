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
	BOOL InitializeBuffer();
	void RenderBuffer();

private:
	BOOL LoadHeightMap(std::string filename);
	void NormalizeHeightMap();
	BOOL RefreshTerrainBuffer();

private:
	ConstantBuffer<CB_VS>* cb_vs_vertexshader;
	ConstantBuffer<CB_PS_light> cb_ps_pixelshader;
	
private:
	ID3D11Device * device;
	ID3D11DeviceContext * deviceContext;

	XMMATRIX transformMatrix;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	VertexBuffer<VertexColor> vertices;
	IndexBuffer indices;

private:
	int terrainWidth;
	int terrainHeight;
	vector<XMFLOAT3> heightMap;

	XMFLOAT2 gridGap;
	XMFLOAT2 oldGridGap;
};

