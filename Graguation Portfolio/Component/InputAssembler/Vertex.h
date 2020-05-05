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

struct VertexNormal
{
	VertexNormal() {}
	VertexNormal(float x, float y, float z, float nx, float ny, float nz, float r, float g, float b, float a)
		: pos(x, y, z), normal(nx, ny, nz), Color(r, g, b, a) {}
	VertexNormal(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT4 color)
		: pos(position), normal(normal), Color(color) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 Color;
};

struct Vertex
{
	Vertex() {}
	Vertex(float x, float y, float z, float u, float v, float nx, float ny, float nz)
		: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz) {}
	Vertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT2 texCoord, DirectX::XMFLOAT3 normal)
		: pos(position), texCoord(texCoord), normal(normal){}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
	DirectX::XMFLOAT3 normal;
};