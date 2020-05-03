#include "Terrain.h"
#include <WICTextureLoader.h>
#include "DirectXSetup/ImGuiClass.h"
#include "GameObject.h"

Terrain::Terrain(GameObject * go)
	:Component(go)
{
	transformMatrix = XMMatrixIdentity();
	terrainWidth = 10;
	terrainHeight = 10;
	gridGap = { 1, 1 };
}

void Terrain::Container()
{
	if (ImGui::TreeNode("Terrain"))
	{
		ImGui::DragFloat2("Grid Gap", (float*)&gridGap, 0.1f, 1.0f, 100.0f);
		
		/*int vcnt = vertices.VertexCount();
		ImGui::InputInt("vertices", &vcnt);
		int icnt = indices.IndexCount();
		ImGui::InputInt("indices", &icnt);*/
		ImGui::TreePop();
	}
}

void Terrain::UpdateMatrix()
{
	XMFLOAT3 pos = this->transform->GetPositionFloat3();
	XMFLOAT3 rot = this->transform->GetRotationFloat3();
	XMFLOAT3 scl = this->transform->GetScaleFloat3();
	this->transformMatrix = XMMatrixScaling(scl.x, scl.y, scl.z) * XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) * XMMatrixTranslation(pos.x, pos.y, pos.z);
	this->transform->UpdateDirectionVectors();
}

bool Terrain::Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, Light* light)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->cb_vs_vertexshader = light->GetVertexShader();
	this->cb_ps_pixelshader = *(light->GetPixelShader());

	if (!LoadHeightMap("Data/Textures/heightmap01.bmp")) return false;
	NormalizeHeightMap();
	if (!InitializeBuffer()) return false;

	this->transform->SetPosition(-124.0f, 0.0f, 0.0f);
	this->transform->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateMatrix();
	return true;
}

void Terrain::Render(const XMMATRIX & viewProjectionMatrix)
{
	RefreshTerrainBuffer();

	//Calculate World-View-Projection Matrix
	this->cb_vs_vertexshader->data.wvpMatrix = this->transformMatrix * viewProjectionMatrix;
	//Calculate World
	this->cb_vs_vertexshader->data.worldMatrix = this->transformMatrix;
	this->cb_vs_vertexshader->ApplyChanges();

	RenderBuffer();
}


BOOL Terrain::LoadHeightMap(std::string filename)
{
	try
	{
		FILE* filePtr = nullptr;
		HRESULT hr = (fopen_s(&filePtr, filename.c_str(), "rb") != 0);
		COM_ERROR_IF_FAILED(hr, "Failed to open the height map file in binary mode.");

		// 파일 헤더를 읽습니다.
		BITMAPFILEHEADER bitmapFileHeader;
		hr = (fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr) != 1);
		COM_ERROR_IF_FAILED(hr, "Failed to read the file header.");

		// 비트맵 정보 헤더를 읽습니다.
		BITMAPINFOHEADER bitmapInfoHeader;
		hr = (fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr) != 1);
		COM_ERROR_IF_FAILED(hr, "Failed to reads the bitmap information header.");

		terrainWidth = bitmapInfoHeader.biWidth;
		terrainHeight = bitmapInfoHeader.biHeight;
		int imageSize = terrainWidth * terrainHeight * 3;

		// 비트맵 이미지 데이터에 메모리를 할당합니다.
		unsigned char* bitmapImage = new unsigned char[imageSize];
		if (!bitmapImage) return false;

		// 비트맵 데이터의 시작 부분으로 이동합니다.
		fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
		if (fread(bitmapImage, 1, imageSize, filePtr) != imageSize)
			return false;
		//hr = (fread(bitmapImage, 1, imageSize, filePtr) != imageSize);
		//COM_ERROR_IF_FAILED(hr, "Failed to reads bitmap hemage data.");
		hr = (fclose(filePtr) != 0);
		COM_ERROR_IF_FAILED(hr, "Failed to close the file.");

		int k = 0;
		for (int j = 0; j < terrainHeight; j++)
		{
			for (int i = 0; i < terrainWidth; i++)
			{
				unsigned char height = bitmapImage[k];
				heightMap.push_back(XMFLOAT3((float)i, (float)height, (float)j));
				k += 3;
			}
		}

		delete[] bitmapImage;
		bitmapImage = 0;
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

void Terrain::NormalizeHeightMap()
{
	for (int j = 0; j < terrainHeight; j++)
	{
		for (int i = 0; i < terrainWidth; i++)
		{
			heightMap[terrainWidth * j + i].y /= 15.0f;
		}
	}
}

BOOL Terrain::InitializeBuffer()
{
	try
	{
		std::vector<VertexColor> vertexData;
		ZeroMemory(&vertexData, sizeof(vertexData));

		for (int z = 0; z < terrainHeight; z++)
		{
			for (int x = 0; x < terrainWidth; x++)
			{
				vertexData.push_back(VertexColor(heightMap[terrainWidth * z + x], XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)));
				//vertexData.push_back(VertexColor(x * gridGap.x, heightMap[terrainWidth * z + x].y, z * gridGap.y, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)));
			}
		}

		HRESULT hr = vertices.Initialize(device, vertexData.data(), vertexData.size());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for sprite.");


		std::vector<DWORD> indexData;
		ZeroMemory(&indexData, sizeof(indexData));

		for (int z = 0; z < terrainHeight; z++)
		{
			for (int x = 0; x < terrainWidth; x++)
			{
				indexData.push_back((terrainWidth + 1) * z + x);
				indexData.push_back((terrainWidth + 1) * (z + 1) + x);
				indexData.push_back((terrainWidth + 1) * z + (x + 1));

				indexData.push_back((terrainWidth + 1) * z + (x + 1));
				indexData.push_back((terrainWidth + 1) * (z + 1) + x);
				indexData.push_back((terrainWidth + 1) * (z + 1) + (x + 1));
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
}

BOOL Terrain::RefreshTerrainBuffer()
{
	if ((oldGridGap.x == gridGap.x) && (oldGridGap.y == gridGap.y)) return false;
	try
	{
		std::vector<VertexColor> vertexData;
		ZeroMemory(&vertexData, sizeof(vertexData));

		int index = 0;
		//XMFLOAT3 pos = this->transform->GetPositionFloat3();

		for (int z = 0; z <= terrainHeight; z++)
		{
			for (int x = 0; x <= terrainWidth; x++)
			{
				vertexData.push_back(VertexColor(heightMap[terrainWidth * z + x], XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)));
			}
		}

		HRESULT hr = vertices.Initialize(device, vertexData.data(), vertexData.size());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for sprite.");
		oldGridGap = gridGap;
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

