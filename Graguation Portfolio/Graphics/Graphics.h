#pragma once
#include "DirectXSetup/DirectXClass.h"
#include "DirectXSetup/ImGuiClass.h"
#include "Shaders.h"

#include "..\\Timer.h"
#include "Component/GameObject.h"


#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>

class Renderer;
class Camera;
class Light;

class Graphics
{
public:
	Graphics(): DRXC(new DirectXClass()), IMGUIC(new ImGuiClass()), SDC(new ShaderClass()){};
	bool Initialize(HWND hwnd, int width, int height);
	void RenderFrame();

	vector<GameObject*> gameobjectList;
	GameObject* MainCamera;
	GameObject* DirectionLight;
	GameObject* gameObject;

private:
	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	Timer fpsTimer;


private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pinkTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pavementTexture;



private:
	bool InitializeScene();
	DirectXClass* DRXC;
	ImGuiClass * IMGUIC;
	ShaderClass * SDC;

};

