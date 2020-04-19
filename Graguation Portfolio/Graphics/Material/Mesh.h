#pragma once

#include "InputAssembler/Vertex.h"
#include "InputAssembler/VertexBuffer.h"
#include "InputAssembler/IndexBuffer.h"
#include "InputAssembler/ConstantBuffer.h"

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
	VertexBuffer<Vertex> vertexbuffer;
	IndexBuffer indexbuffer;
	ID3D11DeviceContext * deviceContext;
	std::vector<Texture> textures;
	DirectX::XMMATRIX transformMatrix;
};