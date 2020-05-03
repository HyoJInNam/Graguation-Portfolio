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
	Camera* camera = MainCamera->getComponent<Camera>();
	DirectionLight->getComponent<Light>()->Render();
	RenderDirect();

	XMMATRIX ViewProjectionMatrix = camera->GetViewMatrix() * camera->GetProjectionMatrix();

	{
		RenderShader_Light(deviceContext);
		for (GameObject* go : gameobjectList)
		{
			go->traverseRender(ViewProjectionMatrix);
		}


		RenderTerrain(deviceContext);
		TurnOnFilling();
		terrain->traverseRender(ViewProjectionMatrix);
		TurnOffFilling();

		TurnOnCulling();
		sphere1->getComponent<Renderer>()->Render(ViewProjectionMatrix);
		XMFLOAT3 pos = sphere1->getPositionToF();
		pos.z = sphere1->getScaleToF().z / 4;
		sphere1->setPosition(pos);
		TurnOffCulling();

		sphere2->getComponent<Renderer>()->Render(ViewProjectionMatrix);
	}
	{
		RenderShader_noLight(deviceContext);
		DirectionLight->getComponent<Light>()->Draw(ViewProjectionMatrix);
	}
	{
		RenderShader2D(deviceContext);
		//sprite->getComponent<Sprite>()->Draw(canvas->getComponent<Canvas>()->GetWorldMatrix() * canvas->getComponent<Canvas>()->GetOrthoMatrix());
	}

	//Draw Text->
	{
		static int fpsCounter = 0;
		static std::string fpsString = "FPS: 0";
		fpsCounter += 1;
		if (fpsTimer.GetMilisecondsElapsed() > 1000.0)
		{
			fpsString = "FPS: " + std::to_string(fpsCounter);
			fpsCounter = 0;
			fpsTimer.Restart();
		}

		spriteBatch->Begin();
		spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(fpsString).c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
		spriteBatch->End();

		static int counter = 0;
	}

	//IMGUIC
	{
		FrameImGUI();
		Hierarchy(gameobjectList);
		Project();
		Inspector(gameobjectList);
		DebugConsol();
		RenderImGUI();
		PresentBuffer();
	}
}

bool Graphics::InitializeScene()
{
	try
	{
		DirectionLight = new GameObject("DirectionLight", nullptr, "DirectionLight");
		gameobjectList.push_back(DirectionLight);
		DirectionLight->addComponent<Light>();
		if (!DirectionLight->getComponent<Light>()->Initialize(device.Get(), deviceContext.Get()))
			return false;


		MainCamera = new GameObject("MainCamera", nullptr, "MainCamera");
		gameobjectList.push_back(MainCamera);
		MainCamera->addComponent<Camera>();
		MainCamera->getComponent<Transform>()->SetPosition(0.0f, 0.0f, -2.0f);
		MainCamera->getComponent<Camera>()->SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 3000.0f);

		MainCamera->setPosition(XMFLOAT3(0, 10, -5));

		//canvas = new GameObject("Canvas", nullptr, "Canvas");
		//gameobjectList.push_back(canvas);
		//canvas->addComponent<Canvas>();
		//canvas->getComponent<Canvas>()->SetProjectionValues(windowWidth, windowHeight, 0.0f, 1.0f);


		//Load Texture
		HRESULT hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Data\\Textures\\seamless_grass.jpg", nullptr, grassTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Data\\Textures\\pinksquare.jpg", nullptr, pinkTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Data\\Textures\\seamless_pavement.jpg", nullptr, pavementTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");



		hr = this->cb_vs_vertexshader_2d.Initialize(this->device.Get(), this->deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize 2d constant buffer.");

		//sprite = new GameObject("Sprite", nullptr, "Sprite");
		//sprite->addComponent<Sprite>();
		//if (!sprite->getComponent<Sprite>()->Initialize(device.Get(), deviceContext.Get(), 256, 256, "Data/Textures/HeightMap256.png", cb_vs_vertexshader_2d))
		//	return false;

		//GameObject* terrain = new GameObject("Terrain", nullptr, "Terrain");
		//gameobjectList.push_back(terrain);
		//terrain->addComponent<Terrain>();
		//if (!terrain->getComponent<Terrain>()->Initialize(device.Get(), deviceContext.Get(), DirectionLight->getComponent<Light>()))
		//	return false;

		sphere1 = new GameObject("sphere", nullptr, "gameObject");
		gameobjectList.push_back(sphere1);
		sphere1->addComponent<Renderer>();
		if (!sphere1->getComponent<Renderer>()->Initialize("Data\\Objects\\sphere.fbx", device.Get(), deviceContext.Get(), DirectionLight->getComponent<Light>()))
			return false;
		sphere1->setScale(XMFLOAT3(2000.0f, 2000.0f, 2000.0f));

		sphere2 = new GameObject("sphere2", nullptr, "gameObject");
		gameobjectList.push_back(sphere2);
		sphere2->addComponent<Renderer>();
		if (!sphere2->getComponent<Renderer>()->Initialize("Data\\Objects\\sphere.fbx", device.Get(), deviceContext.Get(), DirectionLight->getComponent<Light>()))
			return false;

		terrain = new GameObject("Terrain", nullptr, "Terrain");
		gameobjectList.push_back(terrain);
		terrain->addComponent<Terrain>();
		if (!terrain->getComponent<Terrain>()->Initialize(device.Get(), deviceContext.Get(), DirectionLight->getComponent<Light>()))
			return false;


		//GameObject* MartianCityColony = new GameObject("MartianCityColony", nullptr, "gameObject");
		//gameobjectList.push_back(MartianCityColony);
		//MartianCityColony->addComponent<Renderer>();
		//if (!MartianCityColony->getComponent<Renderer>()->Initialize("Data\\Objects\\Center city Sci-Fi\\Center City Sci-Fi.obj", device.Get(), deviceContext.Get(), DirectionLight->getComponent<Light>()))
		//	return false;
		//MartianCityColony->setPosition(XMFLOAT3(0, -23, 77));


		//GameObject* gameObject = new GameObject("gameObject", nullptr, "gameObject");
		//gameobjectList.push_back(gameObject);
		//gameObject->addComponent<Renderer>();
		//if (!gameObject->getComponent<Renderer>()->Initialize("Data\\Objects\\Nanosuit\\Nanosuit.obj", device.Get(), deviceContext.Get(), DirectionLight->getComponent<Light>()))
		//	return false;
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}