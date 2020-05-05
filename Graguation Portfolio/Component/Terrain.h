#pragma once
#include "Component.h"
class Light;

class Terrain : public Component
{
public:
	bool wireFrame = false;

public:
	Terrain(GameObject* go);
	virtual void Container() override;
	virtual void UpdateMatrix() override;
	virtual void Destroy() override;

public:
	bool Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, Light* light);
	void Render(const XMMATRIX & viewProjectionMatrix);

private:
	BOOL InitializeBuffer();
	void RenderBuffer();

private:
	BOOL LoadHeightMap(std::string filename);
	BOOL CalculateNormals();
	BOOL CalculateTextureCoordinates();
	BOOL RefreshTerrainBuffer();

private:
	ConstantBuffer<CB_VS>* cb_vs_vertexshader;
	
private:
	ID3D11Device * device;
	ID3D11DeviceContext * deviceContext;

	XMMATRIX transformMatrix;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	VertexBuffer<Vertex> vertices;
	IndexBuffer indices;

private:
	int terrainWidth;
	int terrainHeight;

	vector<XMFLOAT3> heightMap;
	vector<XMFLOAT2> heightMapTexCoord;
	vector<XMFLOAT3> heightMapNormals;

	XMFLOAT2 gridGap;
	XMFLOAT2 oldGridGap;
};

