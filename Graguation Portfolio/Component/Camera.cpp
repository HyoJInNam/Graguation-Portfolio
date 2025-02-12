#include "Camera.h"
#include "DirectXSetup/ImGuiClass.h"
#include "GameObject.h"

Camera::Camera(GameObject* go): Component(go)
{
	cameraMoveSpeed = 0.006f;
	cameraWheelpeed = 0.6f;
	this->UpdateMatrix();
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	this->projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

void Camera::Container()
{
	if (ImGui::TreeNode("Camera"))
	{
		ImGui::InputFloat("move speed", &cameraMoveSpeed);
		ImGui::InputFloat("wheel speed", &cameraWheelpeed);
		ImGui::TreePop();
	}
}

const XMMATRIX & Camera::GetViewMatrix() const
{
	return this->viewMatrix;
}

const XMMATRIX & Camera::GetProjectionMatrix() const
{
	return this->projectionMatrix;
}

void Camera::UpdateMatrix() 
{
	XMFLOAT3 rot = this->transform->GetRotationFloat3();
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	XMVECTOR camTarget = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, camRotationMatrix);

	XMVECTOR posVector = this->transform->GetPositionVector();
	camTarget += posVector;
	XMVECTOR upDir = XMVector3TransformCoord(DEFAULT_UP_VECTOR, camRotationMatrix);
	this->viewMatrix = XMMatrixLookAtLH(posVector, camTarget, upDir);

	this->transform->UpdateDirectionVectors();
}