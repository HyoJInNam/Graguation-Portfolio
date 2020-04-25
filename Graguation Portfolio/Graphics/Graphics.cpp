#include "Graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	this->fpsTimer.Start();

	if (!DRXC->InitializeDirect(hwnd, width, height))
		return false;

	spriteBatch = std::make_unique<DirectX::SpriteBatch>(DRXC->deviceContext.Get());
	spriteFont = std::make_unique<DirectX::SpriteFont>(DRXC->device.Get(), L"Data\\Fonts\\comic_sans_ms_16.spritefont");

	if (!SDC->InitializeShaders(DRXC->device))
		return false;

	if (!InitializeScene())
		return false;


	IMGUIC->InitializeImGUI(hwnd, DRXC->device, DRXC->deviceContext);


	return true;
}

void Graphics::RenderFrame()
{
	DRXC->RenderDirect();
	DirectionLight->getComponent<Light>()->Render(DRXC->deviceContext.Get());
	SDC->RenderShader(DRXC->deviceContext);
	Camera* camera = MainCamera->getComponent<Camera>();
	{
		DirectionLight->getComponent<Renderer>()->Render(camera->GetViewMatrix() * camera->GetProjectionMatrix());
		gameObject->getComponent<Renderer>()->Render(camera->GetViewMatrix() * camera->GetProjectionMatrix());
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
	
	IMGUIC->FrameImGUI();

	ImGui::Begin("Hierarchy");
	static int selected = -1;
	for (int n = 0; n < gameobjectList.size(); n++)
	{
		if (ImGui::Selectable(gameobjectList[n]->getName().c_str(), selected == n)) selected = n;
	}
	ImGui::End();

	ImGui::Begin("Inspector");
	if (ImGui::BeginTabBar("Inspector"))
	{
		if (ImGui::BeginTabItem("Inspector"))
		{
			if(selected != -1)gameobjectList[selected]->inspector();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
	IMGUIC->RenderImGUI();
	DRXC->PresentBuffer();

}

bool Graphics::InitializeScene()
{
	try
	{
		DirectionLight = new GameObject("DirectionLight", nullptr, "DirectionLight");
		gameobjectList.push_back(DirectionLight);
		DirectionLight->addComponent<Renderer>();
		DirectionLight->addComponent<Light>();
		if (!DirectionLight->getComponent<Light>()->Initialize(DRXC->device.Get(), DRXC->deviceContext.Get()))
			return false;


		MainCamera = new GameObject("MainCamera", nullptr, "MainCamera");
		gameobjectList.push_back(MainCamera);
		MainCamera->addComponent<Camera>();
		MainCamera->getComponent<Transform>()->SetPosition(0.0f, 0.0f, -2.0f);
		MainCamera->getComponent<Camera>()->SetProjectionValues(90.0f, static_cast<float>(DRXC->windowWidth) / static_cast<float>(DRXC->windowHeight), 0.1f, 3000.0f);


		//Load Texture
		HRESULT hr = DirectX::CreateWICTextureFromFile(DRXC->device.Get(), L"Data\\Textures\\seamless_grass.jpg", nullptr, grassTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		hr = DirectX::CreateWICTextureFromFile(DRXC->device.Get(), L"Data\\Textures\\pinksquare.jpg", nullptr, pinkTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		hr = DirectX::CreateWICTextureFromFile(DRXC->device.Get(), L"Data\\Textures\\seamless_pavement.jpg", nullptr, pavementTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");


		gameObject = new GameObject("gameObject", nullptr, "gameObject");
		gameobjectList.push_back(gameObject);
		gameObject->addComponent<Renderer>();
		if (!gameObject->getComponent<Renderer>()->Initialize("Data\\Objects\\Nanosuit\\Nanosuit.obj", DRXC->device.Get(), DRXC->deviceContext.Get(), DirectionLight->getComponent<Light>()->GetVertexShader()))
			return false;
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}