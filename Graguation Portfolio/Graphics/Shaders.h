#pragma once
#include "ErrorLogger.h"
#pragma comment(lib, "D3DCompiler.lib")
#include <d3d11.h>
#include <wrl/client.h>
#include <D3Dcompiler.h>

class VertexShader
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device, std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC * layoutDesc, UINT numElements);
	ID3D11VertexShader * GetShader();
	ID3D10Blob * GetBuffer();
	ID3D11InputLayout * GetInputLayout();
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> shader_buffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};


class PixelShader
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device, std::wstring shaderpath);
	ID3D11PixelShader * GetShader();
	ID3D10Blob * GetBuffer();
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> shader_buffer;
};


class ShaderClass
{
public:
	bool InitializeShaders(Microsoft::WRL::ComPtr<ID3D11Device>&  device);
	void RenderShader_Light(Microsoft::WRL::ComPtr<ID3D11DeviceContext>&  deviceContext);
	void RenderShader_noLight(Microsoft::WRL::ComPtr<ID3D11DeviceContext>&  deviceContext);
	void RenderShader2D(Microsoft::WRL::ComPtr<ID3D11DeviceContext>&  deviceContext);
	void RenderTerrain(Microsoft::WRL::ComPtr<ID3D11DeviceContext>&  deviceContext);

private:
	VertexShader vertexshader;
	PixelShader pixelshader;
	PixelShader pixelshader_nolight;

private:
	VertexShader vsTexture;
	PixelShader psTexture;
};