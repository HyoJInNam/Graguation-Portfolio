#pragma once
#include <DirectXMath.h>


struct Vertex2D
{
	Vertex2D() {}
	Vertex2D(float x, float y, float z, float u, float v)
		: pos(x, y, z), texCoord(u, v) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
};

struct Vertex
{
	Vertex() {}
	Vertex(float x, float y, float z, float u, float v, float nx, float ny, float nz)
		: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
	DirectX::XMFLOAT3 normal;
};

struct VertexTerrain
{
	VertexTerrain() {}
	VertexTerrain(float x, float y, float z, float r, float g, float b, float a)
		: pos(x, y, z), Color(r, g, b, a) {}
	VertexTerrain(float x, float y, float z, DirectX::XMFLOAT4 color)
		: pos(x, y, z), Color(color) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 Color;
};