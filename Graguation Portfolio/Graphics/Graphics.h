#pragma once
#include "DirectXSetup/DirectXClass.h"
#include "DirectXSetup/ImGuiClass.h"
#include "GameObject.h"
#include "Shaders.h"

#include "..\\Timer.h"

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>

class Graphics: public DirectXClass, ImGuiClass, ShaderClass
{
public:
	bool Initialize(HWND hwnd, int width, int height);
	void RenderFrame();
	void Release();

	vector<GameObject*> gameobjectList;
	GameObject* MainCamera;
	GameObject* DirectionLight;
	GameObject* sphere1;
	GameObject* terrain;
	GameObject* canvas;
	GameObject* sprite;

private:
	ConstantBuffer<CB_VS_vertexshader_2d> cb_vs_vertexshader_2d;
	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	Timer fpsTimer;

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pinkTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pavementTexture;



private:
	bool InitializeScene();
};

