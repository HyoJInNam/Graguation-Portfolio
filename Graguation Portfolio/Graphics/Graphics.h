#pragma once
#include "DirectXSetup/DirectXClass.h"
#include "DirectXSetup/ImGuiClass.h"
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_dx11.h>
#include <ImGui/imgui_impl_win32.h>

#include "Material/Shaders.h"

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>

#include "Camera.h"
#include "..\\Timer.h"


#include "GameObject.h"
#include "Light.h"

class Graphics : protected DirectXClass, ImGuiClass, ShaderClass
{
public:
	bool Initialize(HWND hwnd, int width, int height);
	void RenderFrame();

	Camera camera;
	GameObject  gameObject;
	Light light;


private:
	bool InitializeScene();


private:
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	ConstantBuffer<CB_PS_light> cb_ps_light;


private:
	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	Timer fpsTimer;

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pinkTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pavementTexture;

};

