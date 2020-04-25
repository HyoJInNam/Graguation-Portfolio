#include "Transform.h"
#include "DirectXSetup/ImGuiClass.h"
#include "GameObject.h"

Transform::Transform(GameObject* gameObject)
	:Component(gameObject)
{
	SetPosition(0.0f, 0.0f, 0.0f);
	SetRotation(0.0f, 0.0f, 0.0f);
	SetScale(1.0f, 1.0f, 1.0f);
}
//
//Transform::Transform(GameObject * gameObject, const XMVECTOR * position, const XMVECTOR * rotation, const XMVECTOR * scale)
//	:Component(gameObject)
//{
//	SetPosition(*position);
//	SetRotation(*rotation);
//	SetScale(*scale);
//}
//
//Transform::Transform(GameObject * gameObject, const XMFLOAT3 * position, const XMFLOAT3 * rotation, const XMFLOAT3 * scale)
//	:Component(gameObject)
//{
//	SetPosition(*position);
//	SetRotation(*rotation);
//	SetScale(*scale);
//}
//
//Transform::Transform(GameObject * gameObject, const XMVECTOR & position, const XMVECTOR & rotation, const XMVECTOR & scale)
//	: posVector(position), rotVector(rotation), sclVector(scale)
//	, Component(gameObject)
//{
//}
//
//Transform::Transform(GameObject * gameObject, const XMFLOAT3 & position, const XMFLOAT3 & rotation, const XMFLOAT3 & scale)
//	: pos(position), rot(rotation), scl(scale)
//	, Component(gameObject)
//{
//}

void Transform::Container()
{
	if (ImGui::TreeNode("Transform"))
	{
		ImGui::DragFloat3("position", (float*)&pos, 0.1f, -100.0f, 100.0f);
		ImGui::DragFloat3("rotation", (float*)&rot, 0.1f, -360.0f, 36.0f);
		ImGui::DragFloat3("scale", (float*)&scl, 0.1f, 0.0f, 1.0f);
		this->UpdateMatrix();
		ImGui::TreePop();
	}
}

const XMVECTOR & Transform::GetPositionVector() const
{
	return this->posVector;
}

const XMFLOAT3 & Transform::GetPositionFloat3() const
{
	return this->pos;
}

const XMVECTOR & Transform::GetRotationVector() const
{
	return this->rotVector;
}

const XMFLOAT3 & Transform::GetRotationFloat3() const
{
	return this->rot;
}

const XMVECTOR & Transform::GetScaleVector() const
{
	return this->sclVector;
}

const XMFLOAT3 & Transform::GetScaleFloat3() const
{
	return this->scl;
}

void Transform::SetPosition(const XMVECTOR & pos)
{
	XMStoreFloat3(&this->pos, pos);
	this->posVector = pos;
	this->UpdateMatrix();
}

void Transform::SetPosition(const XMFLOAT3 & pos)
{
	this->pos = pos;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void Transform::SetPosition(float x, float y, float z)
{
	this->pos = XMFLOAT3(x, y, z);
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void Transform::AdjustPosition(const XMVECTOR & pos)
{
	this->posVector += pos;
	XMStoreFloat3(&this->pos, this->posVector);
	this->UpdateMatrix();
}

void Transform::AdjustPosition(const XMFLOAT3 & pos)
{
	this->pos.x += pos.y;
	this->pos.y += pos.y;
	this->pos.z += pos.z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void Transform::AdjustPosition(float x, float y, float z)
{
	this->pos.x += x;
	this->pos.y += y;
	this->pos.z += z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void Transform::SetRotation(const XMVECTOR & rot)
{
	this->rotVector = rot;
	XMStoreFloat3(&this->rot, rot);
	this->UpdateMatrix();
}

void Transform::SetRotation(const XMFLOAT3 & rot)
{
	this->rot = rot;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void Transform::SetRotation(float x, float y, float z)
{
	this->rot = XMFLOAT3(x, y, z);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void Transform::AdjustRotation(const XMVECTOR & rot)
{
	this->rotVector += rot;
	XMStoreFloat3(&this->rot, this->rotVector);
	this->UpdateMatrix();
}

void Transform::AdjustRotation(const XMFLOAT3 & rot)
{
	this->rot.x += rot.x;
	this->rot.y += rot.y;
	this->rot.z += rot.z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void Transform::AdjustRotation(float x, float y, float z)
{
	this->rot.x += x;
	this->rot.y += y;
	this->rot.z += z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void Transform::SetLookAtPos(XMFLOAT3 lookAtPos)
{
	//Verify that look at pos is not the same as cam pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior.
	if (lookAtPos.x == this->pos.x && lookAtPos.y == this->pos.y && lookAtPos.z == this->pos.z)
		return;

	lookAtPos.x = this->pos.x - lookAtPos.x;
	lookAtPos.y = this->pos.y - lookAtPos.y;
	lookAtPos.z = this->pos.z - lookAtPos.z;

	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch = atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
		yaw += XM_PI;

	this->SetRotation(pitch, yaw, 0.0f);
}

void Transform::SetScale(const XMVECTOR & scl)
{
	this->sclVector = scl;
	XMStoreFloat3(&this->scl, scl);
	this->UpdateMatrix();
}

void Transform::SetScale(const XMFLOAT3 & scl)
{
	this->scl = scl;
	this->sclVector = XMLoadFloat3(&this->scl);
	this->UpdateMatrix();
}

void Transform::SetScale(float x, float y, float z)
{
	this->scl = XMFLOAT3(x, y, z);
	this->sclVector = XMLoadFloat3(&this->scl);
	this->UpdateMatrix();
}

void Transform::AdjustScale(const XMVECTOR & scl)
{
	this->sclVector += scl;
	XMStoreFloat3(&this->scl, this->sclVector);
	this->UpdateMatrix();
}

void Transform::AdjustScale(const XMFLOAT3 & scl)
{
	this->scl.x += scl.x;
	this->scl.y += scl.y;
	this->scl.z += scl.z;
	this->rotVector = XMLoadFloat3(&this->scl);
	this->UpdateMatrix();
}

void Transform::AdjustScale(float x, float y, float z)
{
	this->scl.x += x;
	this->scl.y += y;
	this->scl.z += z;
	this->sclVector = XMLoadFloat3(&this->scl);
	this->UpdateMatrix();
}

const XMVECTOR & Transform::GetForwardVector(bool omitY)
{
	if (omitY)
		return this->vec_forward_noY;
	else
		return this->vec_forward;
}

const XMVECTOR & Transform::GetRightVector(bool omitY)
{
	if (omitY)
		return this->vec_right_noY;
	else
		return this->vec_right;
}

const XMVECTOR & Transform::GetBackwardVector(bool omitY)
{
	if (omitY)
		return this->vec_backward_noY;
	else
		return this->vec_backward;
}

const XMVECTOR & Transform::GetLeftVector(bool omitY)
{
	if (omitY)
		return this->vec_left_noY;
	else
		return this->vec_left;
}

void Transform::UpdateDirectionVectors()
{
	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, 0.0f);
	this->vec_forward = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
	this->vec_backward = XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	this->vec_left = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	this->vec_right = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, vecRotationMatrix);

	XMMATRIX vecRotationMatrixNoY = XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f);
	this->vec_forward_noY = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, vecRotationMatrixNoY);
	this->vec_backward_noY = XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, vecRotationMatrixNoY);
	this->vec_left_noY = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, vecRotationMatrixNoY);
	this->vec_right_noY = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, vecRotationMatrixNoY);
}