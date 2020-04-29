#include "Canvas.h"
#include "GameObject.h"


Canvas::Canvas(GameObject* go) : Component(go)
{
	this->transform->SetPosition(0.0f, 0.0f, 0.0f);
	this->transform->SetRotation(0.0f, 0.0f, 0.0f);
	UpdateMatrix();
}

void Canvas::SetProjectionValues(float width, float height, float nearZ, float farZ)
{
	orthoMatrix = XMMatrixOrthographicOffCenterLH(0.0f, width, height, 0.0f, nearZ, farZ);
}

const XMMATRIX & Canvas::GetOrthoMatrix() const
{
	return orthoMatrix;
}

const XMMATRIX & Canvas::GetWorldMatrix() const
{
	return worldMatrix;
}

void Canvas::UpdateMatrix() //Updates view matrix and also updates the movement vectors
{
	XMFLOAT3 pos = this->transform->GetPositionFloat3();
	XMFLOAT3 rot = this->transform->GetRotationFloat3();

	XMMATRIX translationOffsetMatrix = XMMatrixTranslation(-pos.x, -pos.y, 0.0f); //z component irrelevant for 2d camera
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	worldMatrix = camRotationMatrix * translationOffsetMatrix;
}