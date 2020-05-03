#include "Sprite.h"
#include <WICTextureLoader.h>
#include "GameObject.h"

bool Sprite::Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, float width, float height, std::string spritePath, ConstantBuffer<CB_VS_vertexshader_2d>& cb_vs_vertexshader_2d)
{
	this->deviceContext = deviceContext;
	if (deviceContext == nullptr)
		return false;

	this->cb_vs_vertexshader_2d = &cb_vs_vertexshader_2d;

	texture = std::make_unique<Texture>(device, spritePath, aiTextureType::aiTextureType_DIFFUSE);

	std::vector<VertexTexture> vertexData =
	{
		VertexTexture(-0.5f, -0.5f, 0.0f, 0.0f, 0.0f), //TopLeft
		VertexTexture(0.5f, -0.5f, 0.0f, 1.0f, 0.0f), //TopRight
		VertexTexture(-0.5, 0.5, 0.0f, 0.0f, 1.0f), //Bottom Left
		VertexTexture(0.5, 0.5, 0.0f, 1.0f, 1.0f), //Bottom Right
	};

	HRESULT hr = vertices.Initialize(device, vertexData.data(), vertexData.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for sprite.");

	std::vector<DWORD> indexData =
	{
		0, 1, 2,
		2, 1, 3
	};

	hr = indices.Initialize(device, indexData.data(), indexData.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");

	this->transform->SetPosition(0.0f, 0.0f, 0.0f);
	this->transform->SetRotation(0.0f, 0.0f, 0.0f);
	this->transform->SetScale(width, height, 0);

	return true;
}

void Sprite::Draw(XMMATRIX orthoMatrix)
{
	XMMATRIX wvpMatrix = worldMatrix * orthoMatrix;
	deviceContext->VSSetConstantBuffers(0, 1, cb_vs_vertexshader_2d->GetAddressOf());
	cb_vs_vertexshader_2d->data.wvpMatrix = wvpMatrix;
	cb_vs_vertexshader_2d->ApplyChanges();

	deviceContext->PSSetShaderResources(0, 1, texture->GetTextureResourceViewAddress());

	const UINT offsets = 0;
	deviceContext->IASetVertexBuffers(0, 1, vertices.GetAddressOf(), vertices.StridePtr(), &offsets);
	deviceContext->IASetIndexBuffer(indices.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	deviceContext->DrawIndexed(indices.IndexCount(), 0, 0);
}

void Sprite::UpdateMatrix()
{
	XMFLOAT3 pos = this->transform->GetPositionFloat3();
	XMFLOAT3 rot = this->transform->GetRotationFloat3();
	XMFLOAT3 scale = this->transform->GetScaleFloat3();
	worldMatrix = XMMatrixScaling(scale.x, scale.y, 1.0f) * XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) * XMMatrixTranslation(pos.x + scale.x / 2.0f, pos.y + scale.y / 2.0f, pos.z);
}
