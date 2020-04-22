#include "Camera.h"
#include "Component/GameObject.h"

Camera::Camera(GameObject* go): Component(go)
{
	this->UpdateMatrix();
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	this->projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

void Camera::Container()
{
	ImGui::Begin("Camera");
	XMFLOAT3 pos = this->transform->GetPositionFloat3();
	ImGui::DragFloat3("position", (float*)&pos, 0.1f, 0.0f, 1.0f);
	this->transform->SetPosition(pos);
	ImGui::End();
}

const XMMATRIX & Camera::GetViewMatrix() const
{
	return this->viewMatrix;
}

const XMMATRIX & Camera::GetProjectionMatrix() const
{
	return this->projectionMatrix;
}

const XMMATRIX & Camera::GetTransformation() const
{
	return viewMatrix * projectionMatrix;
}

void Camera::UpdateMatrix() 
{
	XMFLOAT3 rot = this->transform->GetRotationFloat3();
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	XMVECTOR camTarget = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, camRotationMatrix);

	XMVECTOR posVector = this->transform->GetRotationVector();
	camTarget += posVector;
	XMVECTOR upDir = XMVector3TransformCoord(DEFAULT_UP_VECTOR, camRotationMatrix);
	this->viewMatrix = XMMatrixLookAtLH(posVector, camTarget, upDir);

	this->transform->UpdateDirectionVectors();
}