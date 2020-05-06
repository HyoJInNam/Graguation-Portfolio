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
	oldGridGap = gridGap;
	textureCnt = 8;
	oldTextureCnt = textureCnt;
}

void Terrain::Container()
{
	if (ImGui::TreeNode("Terrain"))
	{
		ImGui::DragFloat2("Grid Gap", (float*)&gridGap, 0.1f, 1.0f, 100.0f);
		//소인수분해 값에 따라서 꺠지지 않고 그려짐.
		ImGui::DragInt("Texture Count", &textureCnt, 0.1f, 1.0f, 128.0f);
		ImGui::Text("Texture count can only be broken down by factorization");
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

void Terrain::Destroy()
{
	heightMapTexCoord.clear();
	heightMapNormals.clear();
	heightMap.clear();
}

bool Terrain::Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, Light* light)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->cb_vs_vertexshader = light->GetVertexShader();

	if (!LoadHeightMap("Data/Textures/heightmap01.bmp")) return false;
	if (!CalculateNormals()) return false;
	if (!CalculateTextureCoordinates()) return false;
	if (!InitializeBuffer()) return false;

	this->transform->SetPosition(0.0f, 0.0f, 0.0f);
	this->transform->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateMatrix();
	return true;
}

void Terrain::Render(const XMMATRIX & viewProjectionMatrix)
{
	this->cb_vs_vertexshader->data.wvpMatrix = this->transformMatrix * viewProjectionMatrix;
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

		BITMAPFILEHEADER bitmapFileHeader;
		hr = (fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr) != 1);
		COM_ERROR_IF_FAILED(hr, "Failed to read the file header.");

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
		if (fread(bitmapImage, 1, imageSize, filePtr) != imageSize) return false;
		hr = (fclose(filePtr) != 0);
		COM_ERROR_IF_FAILED(hr, "Failed to close the file.");

		ZeroMemory(&heightMap, sizeof(heightMap));
		int k = 0;
		for (int j = 0; j < terrainHeight; j++)
		{
			for (int i = 0; i < terrainWidth; i++)
			{
				unsigned char height = bitmapImage[k];
				heightMap.push_back(XMFLOAT3((float)i, (float)(height /= 15.0f), (float)j));
				k += 3;
			}
		}
		
		SAFE_DELETE_ARRAY(bitmapImage);
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

BOOL Terrain::CalculateNormals()
{
	XMFLOAT3* normals = new XMFLOAT3[(terrainHeight - 1) * (terrainWidth - 1)];
	
	auto minus = [&](XMFLOAT3 vt1, XMFLOAT3 vt2)->XMFLOAT3 { return XMFLOAT3(vt1.x - vt2.x, vt1.y - vt2.y, vt1.z - vt2.z);  };
	XMFLOAT3 vector1, vector2;

	// 메쉬의 모든면을 살펴보고 법선을 계산합니다.
	for (int j = 0; j < (terrainHeight - 1); j++)
	{
		for (int i = 0; i < (terrainWidth - 1); i++)
		{
			// 표면의 두 벡터를 계산합니다.
			vector1 = minus(heightMap[(j * terrainHeight) + i], heightMap[((j + 1) * terrainHeight) + i]);
			vector2 = minus(heightMap[((j + 1) * terrainHeight) + i], heightMap[(j * terrainHeight) + (i + 1)]);

			// 이 두 법선에 대한 정규화되지 않은 값을 얻기 위해 두 벡터의 외적을 계산합니다.
			normals[(j * (terrainHeight - 1)) + i] = XMFLOAT3(((vector1.y * vector2.z) - (vector1.z * vector2.y))
															, ((vector1.z * vector2.x) - (vector1.x * vector2.z))
															, ((vector1.x * vector2.y) - (vector1.y * vector2.x)));
		}
	}

	XMFLOAT3 sum;
	int count = 0;
	float length = 0.0f;

	ZeroMemory(&heightMapNormals, sizeof(heightMapNormals));
	// 이제 모든 정점을 살펴보고 각면의 평균을 취합니다. 	
	// 정점이 닿아 그 정점에 대한 평균 평균값을 얻는다.

	for (int j = 0; j < terrainHeight; j++)
	{
		for (int i = 0; i < terrainWidth; i++)
		{
			sum = { 0.0f, 0.0f, 0.0f };
			count = 0;

			// 왼쪽 아래면.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				sum = normals[((j - 1) * (terrainHeight - 1)) + (i - 1)];
				count++;
			}

			// 오른쪽 아래 면.
			if ((i < (terrainWidth - 1)) && ((j - 1) >= 0))
			{
				sum = normals[((j - 1) * (terrainHeight - 1)) + i];
				count++;
			}

			// 왼쪽 위 면.
			if (((i - 1) >= 0) && (j < (terrainHeight - 1)))
			{
				sum = normals[(j * (terrainHeight - 1)) + (i - 1)];
				count++;
			}

			// 오른쪽 위 면.
			if ((i < (terrainWidth - 1)) && (j < (terrainHeight - 1)))
			{
				sum = normals[(j * (terrainHeight - 1)) + i];
				count++;
			}

			// 이 정점에 닿는면의 평균을 취합니다.
			sum = { (sum.x / (float)count) , (sum.y / (float)count) , (sum.z / (float)count) };

			// 이 법선의 길이를 계산합니다.
			length = sqrt((sum.x * sum.x) + (sum.y * sum.y) + (sum.z * sum.z));

			// 이 정점의 최종 공유 법선을 표준화하여 높이 맵 배열에 저장합니다.
			heightMapNormals.push_back(XMFLOAT3((sum.x / length), (sum.y / length), (sum.z / length)));
		}
	}

	SAFE_DELETE_ARRAY(normals);
	return true;
}

BOOL Terrain::CalculateTextureCoordinates()
{
	float incrementValue = (float)textureCnt / (float)terrainWidth;
	int incrementCount = terrainWidth / textureCnt;

	XMFLOAT2 tCoordinate = { 0.0f, 1.0f };
	XMINT2 tCnt = { 0, 0 };

	XMFLOAT2* texCoord = new XMFLOAT2[terrainHeight * terrainWidth];
	if (!texCoord) return false;

	for (int j = 0; j < terrainHeight; j++)
	{
		for (int i = 0; i < terrainWidth; i++)
		{
			texCoord[(terrainHeight * j) + i] = { tCoordinate.x, tCoordinate.y };
			// tu 텍스처 좌표를 증가 값만큼 증가시키고 인덱스를 1 씩 증가시킨다.
			tCoordinate.x += incrementValue;
			tCnt.x++;

			// 텍스처의 오른쪽 끝에 있는지 확인하고, 그렇다면 처음부터 다시 시작하십시오.
			if (tCnt.x == incrementCount)
			{
				tCoordinate.x = 0.0f;
				tCnt.x = 0;
			}
		}

		// tv 텍스처 좌표를 증가 값만큼 증가시키고 인덱스를 1 씩 증가시킵니다.
		tCoordinate.y -= incrementValue;
		tCnt.y++;

		// 텍스처의 상단에 있는지 확인하고, 그렇다면 하단에서 다시 시작합니다.
		if (tCnt.y == incrementCount)
		{
			tCoordinate.y = 1.0f;
			tCnt.y = 0;
		}
	}

	ZeroMemory(&heightMapTexCoord, sizeof(heightMapTexCoord));
	XMFLOAT2 tc[4];
	for (int z = 0; z < terrainHeight - 1; z++)
	{
		for (int x = 0; x < terrainWidth - 1; x++)
		{
			tc[0] = texCoord[(terrainHeight * z) + x];				// 왼쪽 아래.
			tc[1] = texCoord[(terrainHeight * z) + (x + 1)];		// 오른쪽 아래.
			tc[2] = texCoord[(terrainHeight * (z + 1)) + x];		// 왼쪽 위.
			tc[3] = texCoord[(terrainHeight * (z + 1)) + (x + 1)];	// 오른쪽 위.


			heightMapTexCoord.push_back(tc[0]);
			if (tc[2].y == 1.0f) { tc[2].y = 0.0f; }
			heightMapTexCoord.push_back(tc[2]);
			if (tc[1].x == 0.0f) { tc[1].x = 1.0f; }
			heightMapTexCoord.push_back(tc[1]);

			if (tc[2].y == 1.0f) { tc[2].y = 0.0f; }
			heightMapTexCoord.push_back(tc[2]);
			if (tc[3].x == 0.0f) { tc[3].x = 1.0f; }
			if (tc[3].y == 1.0f) { tc[3].y = 0.0f; }
			heightMapTexCoord.push_back(tc[3]);
			if (tc[1].x == 0.0f) { tc[1].x = 1.0f; }
			heightMapTexCoord.push_back(tc[1]);
		}
	}
	SAFE_DELETE_ARRAY(texCoord);
	return true;
}

BOOL Terrain::InitializeBuffer()
{
	try
	{
		HRESULT hr = DirectX::CreateWICTextureFromFile(device, L"Data/Textures/dirt01.jpg", nullptr, texture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		std::vector<Vertex> vertexData;
		ZeroMemory(&vertexData, sizeof(vertexData));
		std::vector<DWORD> indexData;
		ZeroMemory(&indexData, sizeof(indexData));

		XMINT4 index;
		int cnt = 0;
		for (int z = 0; z < terrainHeight - 1; z++)
		{
			for (int x = 0; x < terrainWidth - 1; x++)
			{
				index = { (terrainHeight * z) + x				// 왼쪽 아래.
						,(terrainHeight * z) + (x + 1)			// 오른쪽 아래.
						,(terrainHeight * (z + 1)) + x			// 왼쪽 위.
						,(terrainHeight * (z + 1)) + (x + 1)	// 오른쪽 위.
				};

				vertexData.push_back(Vertex(heightMap[index.x], heightMapTexCoord[cnt], heightMapNormals[index.x]));
				indexData.push_back(cnt++);
				vertexData.push_back(Vertex(heightMap[index.z], heightMapTexCoord[cnt], heightMapNormals[index.z]));
				indexData.push_back(cnt++);
				vertexData.push_back(Vertex(heightMap[index.y], heightMapTexCoord[cnt], heightMapNormals[index.y]));
				indexData.push_back(cnt++);

				vertexData.push_back(Vertex(heightMap[index.z], heightMapTexCoord[cnt], heightMapNormals[index.z]));
				indexData.push_back(cnt++);
				vertexData.push_back(Vertex(heightMap[index.w], heightMapTexCoord[cnt], heightMapNormals[index.w]));
				indexData.push_back(cnt++);
				vertexData.push_back(Vertex(heightMap[index.y], heightMapTexCoord[cnt], heightMapNormals[index.y]));
				indexData.push_back(cnt++);
			}
		}
		
		hr = vertices.Initialize(device, vertexData.data(), vertexData.size());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for sprite.");

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
	bool isRedraw[3] = { false, false, false };
	isRedraw[2] = (oldTextureCnt == textureCnt) ? false : true;
	isRedraw[1] = ((oldGridGap.x == gridGap.x) && (oldGridGap.y == gridGap.y)) ? false : true;
	isRedraw[0] = ((!isRedraw[2])&&(!isRedraw[1])) ? false : true;
	if (!isRedraw[0]) return false;
	try
	{
		std::vector<Vertex> vertexData;
		ZeroMemory(&vertexData, sizeof(vertexData));

		if (isRedraw[1]) {

			for (int z = 0; z < terrainHeight; z++)
			{
				for (int x = 0; x < terrainWidth; x++)
				{
					heightMap[(terrainHeight * z) + x].x = x * gridGap.x;
					heightMap[(terrainHeight * z) + x].z = z * gridGap.y;
				}
			}

			if (!CalculateNormals()) return false;
		}
		if (isRedraw[2]) if (!CalculateTextureCoordinates()) return false;

		XMINT4 index;
		int cnt = 0;
		for (int z = 0; z < terrainHeight - 1; z++)
		{
			for (int x = 0; x < terrainWidth - 1; x++)
			{
				index = { (terrainHeight * z) + x				// 왼쪽 아래.
						,(terrainHeight * z) + (x + 1)			// 오른쪽 아래.
						,(terrainHeight * (z + 1)) + x			// 왼쪽 위.
						,(terrainHeight * (z + 1)) + (x + 1)	// 오른쪽 위.
				};

				vertexData.push_back(Vertex(heightMap[index.x], heightMapTexCoord[cnt++], heightMapNormals[index.x]));
				vertexData.push_back(Vertex(heightMap[index.z], heightMapTexCoord[cnt++], heightMapNormals[index.z]));
				vertexData.push_back(Vertex(heightMap[index.y], heightMapTexCoord[cnt++], heightMapNormals[index.y]));

				vertexData.push_back(Vertex(heightMap[index.z], heightMapTexCoord[cnt++], heightMapNormals[index.z]));
				vertexData.push_back(Vertex(heightMap[index.w], heightMapTexCoord[cnt++], heightMapNormals[index.w]));
				vertexData.push_back(Vertex(heightMap[index.y], heightMapTexCoord[cnt++], heightMapNormals[index.y]));
			}
		}

		VertexBuffer<Vertex> origin = vertices;
		HRESULT hr = vertices.Initialize(device, vertexData.data(), vertexData.size());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for sprite.");
		origin.Get()->Release();
		oldGridGap = gridGap;
		oldTextureCnt = textureCnt;
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

