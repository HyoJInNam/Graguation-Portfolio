#pragma once
#include "Transform.h"
#include "DirectXSetup/ImGuiClass.h"
using namespace DirectX;

class Camera : public Transform, ImGuiContainer
{
public:
	Camera();
	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

	const XMMATRIX & GetViewMatrix() const;
	const XMMATRIX & GetProjectionMatrix() const;

private:
	void UpdateMatrix() override;

	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
};