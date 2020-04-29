#pragma once
#include "Component.h"

class Canvas : public Component
{
public:
	Canvas(GameObject* go);
	void SetProjectionValues(float width, float height, float nearZ, float farZ);

	const XMMATRIX & GetOrthoMatrix() const;
	const XMMATRIX & GetWorldMatrix() const;
private:
	void UpdateMatrix() override;

	XMMATRIX orthoMatrix;
	XMMATRIX worldMatrix;
};

