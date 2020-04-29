#pragma once

#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#pragma comment(lib,"assimp-vc141-mtd.lib")

#include "Texture.h"

class Mesh
{
public:
	Mesh(ID3D11Device * device, ID3D11DeviceContext * deviceContext, std::vector<Vertex> & vertices, std::vector<DWORD> & indices, std::vector<Texture> & textures, const DirectX::XMMATRIX & transformMatrix);
	Mesh(const Mesh & mesh);
	void Draw();
	const DirectX::XMMATRIX & GetTransformMatrix();
private:
	ID3D11DeviceContext * deviceContext;
	DirectX::XMMATRIX transformMatrix;
	std::vector<Texture> textures;

	VertexBuffer<Vertex> vertexbuffer;
	IndexBuffer indexbuffer;
};