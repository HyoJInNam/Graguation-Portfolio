#include "Graphics.h"
bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	this->fpsTimer.Start();

	if (!InitializeDirect(hwnd, width, height))
		return false;

	spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
	spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data\\Fonts\\comic_sans_ms_16.spritefont");

	if (!InitializeShaders(this->device))
		return false;

	if (!InitializeScene())
		return false;


	InitializeImGUI(hwnd, this->device, this->deviceContext);


	return true;
}

void Graphics::RenderFrame()
{
	this->cb_ps_light.data.dynamicLightColor = light.lightColor;
	this->cb_ps_light.data.dynamicLightStrength = light.lightStrength;
	this->cb_ps_light.data.dynamicLightPosition = light.GetPositionFloat3();
	this->cb_ps_light.data.dynamicLightAttenuation_a = light.attenuation_a;
	this->cb_ps_light.data.dynamicLightAttenuation_b = light.attenuation_b;
	this->cb_ps_light.data.dynamicLightAttenuation_c = light.attenuation_c;
	this->cb_ps_light.ApplyChanges();
	this->deviceContext->PSSetConstantBuffers(0, 1, this->cb_ps_light.GetAddressOf());
	
	RenderDirect();
	RenderShader(this->deviceContext);
	{
		this->gameObject.Draw(camera.GetViewMatrix() * camera.GetProjectionMatrix());
	}
	{
		this->light.Draw(camera.GetViewMatrix() * camera.GetProjectionMatrix());
	}

	//Draw Text
	static int fpsCounter = 0;
	static std::string fpsString = "FPS: 0";
	fpsCounter += 1;
	if (fpsTimer.GetMilisecondsElapsed() > 1000.0)
	{		fpsString = "FPS: " + std::to_string(fpsCounter);
		fpsCounter = 0;
		fpsTimer.Restart();
	}

	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(fpsString).c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();

	static int counter = 0;
	
	FrameImGUI();

	ImGui::Begin("Camera");
	XMFLOAT3 pos = camera.GetPositionFloat3();
	ImGui::DragFloat3("position", (float*)&pos, 0.1f, 0.0f, 1.0f);
	camera.SetPosition(pos);
	ImGui::End();

	ImGui::Begin("Light Controls");
	ImGui::DragFloat3("Ambient Light Color", &this->cb_ps_light.data.ambientLightColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Ambient Light Strength", &this->cb_ps_light.data.ambientLightStrength, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Dynamic Light Attenuation A", &this->light.attenuation_a, 0.01f, 0.1f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation B", &this->light.attenuation_b, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation C", &this->light.attenuation_c, 0.01f, 0.0f, 10.0f);
	ImGui::End();

	RenderImGUI();
	PresentBuffer();
}

bool Graphics::InitializeScene()
{
	try
	{
		//Load Texture
		HRESULT hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\seamless_grass.jpg", nullptr, grassTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\pinksquare.jpg", nullptr, pinkTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\seamless_pavement.jpg", nullptr, pavementTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		//Initialize Constant Buffer(s)
		hr = this->cb_vs_vertexshader.Initialize(this->device.Get(), this->deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");

		hr = this->cb_ps_light.Initialize(this->device.Get(), this->deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");

		this->cb_ps_light.data.ambientLightColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
		this->cb_ps_light.data.ambientLightStrength = 1.0f;

		if (!gameObject.Initialize("Data\\Objects\\Nanosuit\\Nanosuit.obj", this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader))
			return false;

		if (!light.Initialize(this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader))
			return false;

		camera.SetPosition(0.0f, 0.0f, -2.0f);
		camera.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 3000.0f);
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}