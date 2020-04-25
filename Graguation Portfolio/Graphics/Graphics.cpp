#include "Graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	this->fpsTimer.Start();

	if (!InitializeDirect(hwnd, width, height))
		return false;

	spriteBatch = std::make_unique<DirectX::SpriteBatch>(deviceContext.Get());
	spriteFont = std::make_unique<DirectX::SpriteFont>(device.Get(), L"Data\\Fonts\\comic_sans_ms_16.spritefont");

	if (!InitializeShaders(device))
		return false;

	if (!InitializeScene())
		return false;


	InitializeImGUI(hwnd, device, deviceContext);
	return true;
}

void Graphics::RenderFrame()
{
	DirectionLight->getComponent<Light>()->Render(deviceContext.Get());
	RenderDirect();
	RenderShader_Light(deviceContext);
	Camera* camera = MainCamera->getComponent<Camera>();
	{
		gameObject->getComponent<Renderer>()->Render(camera->GetViewMatrix() * camera->GetProjectionMatrix());
	}
	{
		RenderShader_noLight(deviceContext);
		DirectionLight->getComponent<Renderer>()->Render(camera->GetViewMatrix() * camera->GetProjectionMatrix());
	}

	//Draw Text->
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
	
	//IMGUIC
	FrameImGUI();
	Hierarchy(gameobjectList);
	Inspector(gameobjectList);	
	RenderImGUI();
	PresentBuffer();

}

bool Graphics::InitializeScene()
{
	try
	{
		DirectionLight = new GameObject("DirectionLight", nullptr, "DirectionLight");
		gameobjectList.push_back(DirectionLight);
		DirectionLight->addComponent<Renderer>();
		DirectionLight->addComponent<Light>();
		if (!DirectionLight->getComponent<Light>()->Initialize(device.Get(), deviceContext.Get()))
			return false;


		MainCamera = new GameObject("MainCamera", nullptr, "MainCamera");
		gameobjectList.push_back(MainCamera);
		MainCamera->addComponent<Camera>();
		MainCamera->getComponent<Transform>()->SetPosition(0.0f, 0.0f, -2.0f);
		MainCamera->getComponent<Camera>()->SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 3000.0f);


		////Load Texture
		//HRESULT hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Data\\Textures\\seamless_grass.jpg", nullptr, grassTexture.GetAddressOf());
		//COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		//hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Data\\Textures\\pinksquare.jpg", nullptr, pinkTexture.GetAddressOf());
		//COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		//hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Data\\Textures\\seamless_pavement.jpg", nullptr, pavementTexture.GetAddressOf());
		//COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");


		gameObject = new GameObject("gameObject", nullptr, "gameObject");
		gameobjectList.push_back(gameObject);
		gameObject->addComponent<Renderer>();
		if (!gameObject->getComponent<Renderer>()->Initialize("Data\\Objects\\Nanosuit\\Nanosuit.obj", device.Get(), deviceContext.Get(), DirectionLight->getComponent<Light>()->GetVertexShader()))
			return false;
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}