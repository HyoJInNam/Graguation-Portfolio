#pragma once
#include "GameObject.h"

class Camera : public Component
{

public:
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