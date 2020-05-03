#pragma once
#include <DirectXMath.h>


struct VertexColor
{
	VertexColor() {}
	VertexColor(float x, float y, float z, float r, float g, float b, float a)
		: pos(x, y, z), Color(r, g, b, a) {}
	VertexColor(float x, float y, float z, DirectX::XMFLOAT4 color)
		: pos(x, y, z), Color(color) {}
	VertexColor(DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 color)
		: pos(position), Color(color) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 Color;
};

struct VertexTexture
{
	VertexTexture() {}
	VertexTexture(float x, float y, float z, float u, float v)
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