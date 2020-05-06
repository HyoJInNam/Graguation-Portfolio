#pragma once
#include "Component.h"
class Light;

class Terrain : public Component
{
public:
	Terrain(GameObject* go);
	virtual void Container() override;
	virtual void UpdateMatrix() override;
	virtual void Destroy() override;

public:
	bool Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, Light* light);
	void Render(const XMMATRIX & viewProjectionMatrix);
	BOOL RefreshTerrainBuffer();

private:
	BOOL InitializeBuffer();
	void RenderBuffer();


public:
	int terrainWidth;
	int terrainHeight;

private:
	vector<XMFLOAT3> heightMap;
	vector<XMFLOAT2> heightMapTexCoord;
	vector<XMFLOAT3> heightMapNormals;

	XMFLOAT2 gridGap;
	XMFLOAT2 oldGridGap;
	int textureCnt;
	int oldTextureCnt;

private:
	BOOL LoadHeightMap(std::string filename);
	BOOL CalculateNormals();
	BOOL CalculateTextureCoordinates();


private:
	ID3D11Device * device;
	ID3D11DeviceContext * deviceContext;
	ConstantBuffer<CB_VS>* cb_vs_vertexshader;

	XMMATRIX transformMatrix;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	VertexBuffer<Vertex> vertices;
	IndexBuffer indices;

};

