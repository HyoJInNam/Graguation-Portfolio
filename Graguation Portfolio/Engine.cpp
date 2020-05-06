#include "Engine.h"

bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	timer.Start();

	if (!this->render_window.Initialize(this, hInstance, window_title, window_class, width, height))
		return false;

	if (!gfx.Initialize(this->render_window.GetHWND(), width, height))
		return false;

	return true;
}

bool Engine::ProcessMessages()
{
	return this->render_window.ProcessMessages();
}

void Engine::Update()
{
	float dt = (float)timer.GetMilisecondsElapsed();
	timer.Restart();

	//while (!keyboard.CharBufferIsEmpty())
	//{
	//	unsigned char ch = keyboard.ReadChar();
	//	//Debug output
	//	{
	//		std::string outmsg = "Char: ";
	//		outmsg += ch;
	//		outmsg += "\n";
	//		OutputDebugStringA(outmsg.c_str());
	//	}
	//}
	//while (!keyboard.KeyBufferIsEmpty())
	//{
	//	KeyboardEvent kbe = keyboard.ReadKey();
	//	unsigned char keycode = kbe.GetKeyCode();
	//	//Debug output
	//	{
	//		std::string outmsg = "";
	//		if (kbe.IsPress()) outmsg += "Key press: ";
	//		if (kbe.IsRelease()) outmsg += "Key release: ";
	//		outmsg += keycode;
	//		outmsg += "\n";
	//		OutputDebugStringA(outmsg.c_str()); 
	//	}
	//}

	Transform * mainCameraTransform = this->gfx.MainCamera->getTransform();
	float cameraWheelpeed = this->gfx.MainCamera->getComponent<Camera>()->cameraWheelpeed;
	float cameraSpeed = this->gfx.MainCamera->getComponent<Camera>()->cameraMoveSpeed;
	while (!mouse.EventBufferIsEmpty())
	{
		MouseEvent me = mouse.ReadEvent();

		if (mouse.IsLeftDown())
		{
			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
			{
				LDrag = true;
			}

		}
		else if (LDrag)
		{
			LDrag = false;
			this->gfx.terrain->getComponent<Terrain>()->RefreshTerrainBuffer();
		}

		if (mouse.IsRightDown())
		{
			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
			{
				mainCameraTransform->AdjustRotation((float)me.GetPosY() * 0.01f, (float)me.GetPosX() * 0.01f, 0);
			}
		}
		if (me.GetType() == MouseEvent::EventType::WheelUp)
		{
			mainCameraTransform->AdjustPosition(mainCameraTransform->GetForwardVector() * cameraWheelpeed * dt);
		}
		if (me.GetType() == MouseEvent::EventType::WheelDown)
		{
			mainCameraTransform->AdjustPosition(mainCameraTransform->GetBackwardVector() * cameraWheelpeed * dt);
		}
	}

	//this->gfx.gameObject.AdjustRotation(0.0f, 0.001f*dt, 0.0f);
	//float cameraSpeed = 0.006f;

	if (keyboard.KeyIsPressed('W'))
	{
		mainCameraTransform->AdjustPosition(mainCameraTransform->GetForwardVector() * cameraSpeed * dt);
	}
	if (keyboard.KeyIsPressed('S'))
	{
		mainCameraTransform->AdjustPosition(mainCameraTransform->GetBackwardVector() * cameraSpeed * dt);
	}
	if (keyboard.KeyIsPressed('A'))
	{
		mainCameraTransform->AdjustPosition(mainCameraTransform->GetLeftVector() * cameraSpeed * dt);
	}
	if (keyboard.KeyIsPressed('D'))
	{
		mainCameraTransform->AdjustPosition(mainCameraTransform->GetRightVector() * cameraSpeed * dt);
	}
	if (keyboard.KeyIsPressed('E'))
	{
		mainCameraTransform->AdjustPosition(0.0f, cameraSpeed * dt, 0.0f);
	}
	if (keyboard.KeyIsPressed('Q'))
	{
		mainCameraTransform->AdjustPosition(0.0f, -cameraSpeed * dt, 0.0f);
	}
	if (keyboard.KeyIsPressed('L'))
	{
		XMVECTOR lightPosition = mainCameraTransform->GetPositionVector();
		lightPosition += mainCameraTransform->GetForwardVector();
		static Transform * DirectionLightTransform = this->gfx.DirectionLight->getTransform();
		DirectionLightTransform->SetPosition(lightPosition);
		DirectionLightTransform->SetRotation(mainCameraTransform->GetRotationFloat3());
	}
}

void Engine::RenderFrame()
{
	this->gfx.RenderFrame();
}

void Engine::Release()
{
	gfx.Release();
}
