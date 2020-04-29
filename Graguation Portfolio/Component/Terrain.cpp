#include "Terrain.h"
#include <WICTextureLoader.h>
#include "DirectXSetup/ImGuiClass.h"
#include "GameObject.h"

Terrain::Terrain(GameObject * go)
	:Component(go)
{
	worldMatrix = XMMatrixIdentity();
	width = 1000;
	height = 1000;
}

void Terrain::Container()
{
	if (ImGui::TreeNode("Terrain"))
	{
		//ImGui::DragFloat3("translationOffset", &this->cb_ps_pixelshader.data.translationOffset.z, 0.01f, 0.0f, 1.0f);
		int vcnt = vertices.VertexCount();
 
		ImGui::InputInt("vertices", &vcnt);
		int icnt = indices.IndexCount();
		ImGui::InputInt("indices", &icnt);
		ImGui::TreePop();
	}
}

void Terrain::UpdateMatrix()
{
	XMFLOAT3 pos = this->transform->GetPositionFloat3();
	XMFLOAT3 rot = this->transform->GetRotationFloat3();
	this->worldMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) * XMMatrixTranslation(pos.x, pos.y, pos.z);
	this->transform->UpdateDirectionVectors();
}

bool Terrain::Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, Light* light)
{
	this->deviceContext = deviceContext;
	this->cb_vs_vertexshader = *(light->GetVertexShader());
	this->cb_ps_pixelshader = *(light->GetPixelShader());
	if (!LoadHeightMap(device, "Data/Textures/HeightMap256.png")) return false;
	if (!InitializeBuffer(device)) return false;

	return true;
}

void Terrain::Render(const XMMATRIX & viewProjectionMatrix)
{
	RenderBuffer();
}


BOOL Terrain::LoadHeightMap(ID3D11Device * device, std::string filename)
{
	try
	{
		HRESULT hr = DirectX::CreateWICTextureFromFile(device, StringHelper::StringToWide(filename).c_str(), nullptr, texture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

BOOL Terrain::InitializeBuffer(ID3D11Device * device)
{
	try
	{
		std::vector<VertexTerrain> vertexData;
		std::vector<DWORD> indexData;

		int index = 0;

		XMFLOAT4 color = { 1, 1.0f, 1.0f, 1.0f };

		for (int z = 0; z <= height; z++)
		{
			for (int x = 0; x <= width; x++)
			{
				vertexData.push_back(VertexTerrain(x, 0.0f, z, color));
			}
		}

		HRESULT hr = vertices.Initialize(device, vertexData.data(), vertexData.size());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for sprite.");

		for (int z = 0; z < height; z++)
		{
			for (int x = 0; x < width; x++)
			{
				indexData.push_back((width + 1) * z + x);
				indexData.push_back((width + 1) * (z + 1) + x);
				indexData.push_back((width + 1) * z + (x + 1));

				indexData.push_back((width + 1) * z + (x + 1));
				indexData.push_back((width + 1) * (z + 1) + x);
				indexData.push_back((width + 1) * (z + 1) + (x + 1));

				index += 6;
			}
		}

		hr = indices.Initialize(device, indexData.data(), indexData.size());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

void Terrain::RenderBuffer()
{
	UINT offset = 0;

	//Square
	deviceContext->PSSetShaderResources(0, 1, texture.GetAddressOf());
	deviceContext->IASetVertexBuffers(0, 1, this->vertices.GetAddressOf(), this->vertices.StridePtr(), &offset);
	deviceContext->IASetIndexBuffer(this->indices.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->DrawIndexed(this->indices.IndexCount(), 0, 0);


	//effect->AsMatrix("View")->SetMatrix();
	//effect->AsMatrix("Projection")->SetMatrix();
	//effect->AsMatrix("World")->SetMatrix(world);
	//Color color = Color( 1, 0, 0, 1 );
	//effect->AsVector("Color")->SetFloatVector(color);
	//effect->DrawIndexed(this->indices.IndexCount(), 0, 0);
}

