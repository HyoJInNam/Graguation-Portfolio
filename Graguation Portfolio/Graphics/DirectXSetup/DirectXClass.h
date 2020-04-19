#pragma once
#include "AdapterReader.h"

class DirectXClass
{
protected:
	bool InitializeDirect(HWND hwnd, int width, int height);
	void RenderDirect();
	void PresentBuffer();


private:
	BOOL DeviceAndSwapChain(HWND& hwnd);
	BOOL DepthStencil();
	BOOL Rasterizer();
	BOOL Viewport();
	BOOL Sampler();
	BOOL Blend();

protected:
	int windowWidth = 0;
	int windowHeight = 0;

protected:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;

	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;

private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
};

