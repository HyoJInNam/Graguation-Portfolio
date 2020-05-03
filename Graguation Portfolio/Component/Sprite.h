#pragma once

#include <string>

#include "InputAssembler/Vertex.h"
#include "InputAssembler/VertexBuffer.h"
#include "InputAssembler/IndexBuffer.h"
#include "InputAssembler/ConstantBuffer.h"
#include "InputAssembler/Texture.h"

#include "Component.h"

class Sprite: public Component
{
public:
	Sprite(GameObject* go) : Component(go) {};
	bool Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, float width, float height, std::string spritePath, ConstantBuffer<CB_VS_vertexshader_2d> & cb_vs_vertexshader_2d);
	void Draw(XMMATRIX orthoMatrix); //2d camera orthogonal matrix

private:
	void UpdateMatrix() override;

	ConstantBuffer<CB_VS_vertexshader_2d> * cb_vs_vertexshader_2d = nullptr;
	XMMATRIX worldMatrix = XMMatrixIdentity();

	std::unique_ptr<Texture> texture;
	ID3D11DeviceContext * deviceContext = nullptr;

	IndexBuffer indices;
	VertexBuffer<VertexTexture> vertices;
};

