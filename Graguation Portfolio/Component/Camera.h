#pragma once
#include "Component.h"

class Camera : public Component
{

public:
	float cameraMoveSpeed = 0.006f;
	float cameraWheelpeed = 0.6f;
	Camera(GameObject* go);
	virtual void Container() override;
	virtual void UpdateMatrix() override;

	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

	const XMMATRIX & GetViewMatrix() const;
	const XMMATRIX & GetProjectionMatrix() const;

private:
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
};