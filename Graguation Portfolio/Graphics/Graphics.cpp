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
		bool isWireFrame = false;
		bool isCullMode = true;
		RenderShader_Light(deviceContext);

		
		for (GameObject* go : GameObject::gameObjects)
		{
			if (!go->isCullMode) isCullMode = TurnOnCulling();
			if (go->isFill) isWireFrame = TurnOnFilling();
			if (go->isActive()) go->traverseRender(ViewProjectionMatrix);
			if (isWireFrame) isWireFrame = TurnOffFilling();
			if (!isCullMode) isCullMode = TurnOffCulling();
		}

	}
	{
		RenderShader_noLight(deviceContext);
		DirectionLight->getComponent<Light>()->Draw(ViewProjectionMatrix);

	}
	{
		//RenderColor(deviceContext);
		XMFLOAT3 pos = SkyDome->getPositionToF();
		pos.z = SkyDome->getScaleToF().z / 4;
		SkyDome->setPosition(pos);
	}
	{	
		RenderShader2D(deviceContext);
		GameObject* go_canvs = MainCamera->Find("Canvas");
		Canvas* canvas = go_canvs->getComponent<Canvas>();
		XMMATRIX orthoMatrix = canvas->GetWorldMatrix() * canvas->GetOrthoMatrix();

		GameObject* go_sprite = MainCamera->Find("Sprite");
		go_sprite->getComponent<Sprite>()->Draw(orthoMatrix);

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
		Hierarchy(GameObject::gameObjects);
		Project();
		Inspector(GameObject::gameObjects);
		DebugConsol();
		RenderImGUI();
		PresentBuffer();
	}
}

void Graphics::Release()
{
	for (GameObject* go : GameObject::gameObjects)
	{
		go->traverseDestroy();
	}

	DELETE_VECTOR(GameObject::gameObjects);
}

bool Graphics::InitializeScene()
{
	try
	{
		HRESULT hr = this->cb_vs_vertexshader_2d.Initialize(this->device.Get(), this->deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize 2d constant buffer.");

		DirectionLight = new GameObject("DirectionLight", nullptr, "DirectionLight");
		DirectionLight->addComponent<Light>();
		DirectionLight->getComponent<Light>()->Initialize(device.Get(), deviceContext.Get());
		DirectionLight->setPosition(XMFLOAT3(-20, -1, 0));

		MainCamera = new GameObject("MainCamera", nullptr, "MainCamera");
		MainCamera->addComponent<Camera>();
		MainCamera->getComponent<Transform>()->SetPosition(0.0f, 0.0f, -2.0f);
		MainCamera->getComponent<Camera>()->SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 3000.0f);
		MainCamera->setPosition(XMFLOAT3(0, 10, -5));

		SkyDome = new GameObject("sphere", nullptr, "gameObject");
		SkyDome->isCullMode = false;
		SkyDome->addComponent<Renderer>();
		if (!SkyDome->getComponent<Renderer>()->Initialize("Data\\Objects\\sphere.obj", device.Get(), deviceContext.Get(), DirectionLight->getComponent<Light>()))
			return false;
		SkyDome->setScale(XMFLOAT3(2000.0f, 2000.0f, 2000.0f));

		terrain = new GameObject("Terrain", nullptr, "Terrain");
		terrain->addComponent<Terrain>();
		terrain->getComponent<Terrain>()->Initialize(device.Get(), deviceContext.Get(), DirectionLight->getComponent<Light>());
		terrain->setScale(XMFLOAT3(5.0f, 5.0f, 5.0f));

		XMFLOAT3 pos = terrain->getPositionToF();
		pos.x = (terrain->getComponent<Terrain>()->terrainWidth * terrain->getScaleToF().x) / -2.0f;
		pos.z = (terrain->getComponent<Terrain>()->terrainHeight * terrain->getScaleToF().z) / -2.0f;
		terrain->setPosition(pos);

		GameObject* gameObject = new GameObject("gameObject", nullptr, "gameObject");
		gameObject->addComponent<Renderer>();
		if (!gameObject->getComponent<Renderer>()->Initialize("Data\\Objects\\Nanosuit\\Nanosuit.obj", device.Get(), deviceContext.Get(), DirectionLight->getComponent<Light>()))
			return false;

		gameObject = new GameObject("MartianCityColony", nullptr, "gameObject");
		gameObject->addComponent<Renderer>();
		if (!gameObject->getComponent<Renderer>()->Initialize("Data\\Objects\\\MartianCityColony\\city colony.obj", device.Get(), deviceContext.Get(), DirectionLight->getComponent<Light>()))
			return false;
		gameObject->setPosition(XMFLOAT3(-145, -1, -264));
		gameObject->setRotation(XMFLOAT3(0, 3.5f, 0));

		GameObject* canvas = new GameObject("Canvas", nullptr, "Canvas");
		canvas->addComponent<Canvas>();
		canvas->getComponent<Canvas>()->SetProjectionValues(windowWidth, windowHeight, 0.0f, 1.0f);

		GameObject* gameObject2D = new GameObject("Sprite", canvas, "Sprite");
		gameObject2D->addComponent<Sprite>();
		if (!gameObject2D->getComponent<Sprite>()->Initialize(device.Get(), deviceContext.Get(), 256, 256, "Data/Textures/HeightMap256.png", cb_vs_vertexshader_2d))
			return false;
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}