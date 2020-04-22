#pragma once
#include "DirectXSetup/ImGuiClass.h"
#include "GameObject.h"
using namespace DirectX;

class Camera : public Component, ImGuiEvent
{

public:
	Camera(GameObject* go);
	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);
	virtual void Container();

	const XMMATRIX & GetViewMatrix() const;
	const XMMATRIX & GetProjectionMatrix() const;
	const XMMATRIX & GetTransformation() const;

private:
	void UpdateMatrix() override;

private:
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
};