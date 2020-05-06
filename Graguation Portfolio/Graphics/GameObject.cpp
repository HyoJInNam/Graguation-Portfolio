#include "GameObject.h"
#include "DirectXSetup/ImGuiClass.h"

vector<GameObject*> GameObject::gameObjects;

GameObject::GameObject(const string & name, GameObject * parent, const string & tag)
	: name(name), tag(tag), enabled(true), parent(parent)
	, transform(new Transform(this))
{
	components.clear();
	components.push_back(transform);
}

string GameObject::getName() const
{
	return this->name;
}

bool GameObject::isActive()
{
	return this->enabled;
}

void GameObject::setActive(bool flag)
{
	this->enabled = flag;
}

void GameObject::setTransform(Transform transform)
{
	this->transform = &transform;
}

void GameObject::setPosition(const XMFLOAT3 & pos)
{ 
	this->transform->SetPosition(pos);
}

void GameObject::setRotation(const XMFLOAT3 & rot)
{
	this->transform->SetRotation(rot);
}

void GameObject::setScale(const XMFLOAT3 & scl)
{
	this->transform->SetScale(scl);
}

Transform * GameObject::getTransform()
{
	return this->transform;
}

const XMVECTOR & GameObject::getPosition()
{
	return this->transform->GetPositionVector();
}

const XMVECTOR & GameObject::getRotation()
{
	return this->transform->GetRotationVector();
}

const XMVECTOR & GameObject::getScale()
{
	return this->transform->GetScaleVector();
}

const XMFLOAT3 & GameObject::getPositionToF()
{
	return this->transform->GetPositionFloat3();
}

const XMFLOAT3 & GameObject::getRotationToF()
{
	return this->transform->GetRotationFloat3();
}

const XMFLOAT3 & GameObject::getScaleToF()
{
	return this->transform->GetScaleFloat3();
}

void GameObject::setParent(GameObject * parent)
{
	this->parent = parent;
}

GameObject * GameObject::getParent()
{
	return this->parent;
}

GameObject * GameObject::Find(const string & path)
{
	for (auto gameObject : gameObjects)
	{
		if (gameObject->name == path)
			return gameObject;
	}

	return nullptr;
}

vector<GameObject*> GameObject::allFind(const string & path)
{
	vector<GameObject*> finds;

	for (auto gameObject : gameObjects)
	{
		if (gameObject->tag == path)
			finds.push_back(gameObject);
	}
	return finds;
}


void GameObject::inspector()
{
	if (enabled == false) return;

	ImGui::Checkbox("", &enabled);
	ImGui::SameLine();
	ImGui::InputText("", const_cast<char*>(name.c_str()), name.size());
	ImGui::Text("tag");
	ImGui::SameLine();
	ImGui::InputText("", const_cast<char*>(tag.c_str()), tag.size());

	ImGui::Checkbox("Wire Frame", &isFill);
	ImGui::SameLine();
	ImGui::Checkbox("Cull Mode", &isCullMode);

	for (auto comp : components)
	{
		comp->Container();
		ImGui::Separator();
	}
}

void GameObject::traverseUpdate()
{
	if (enabled == false) return;

	for (auto comp : components)
	{
		if (comp == transform) continue;
		comp->UpdateMatrix();
	}
	for (auto child : children)
	{
		child->traverseUpdate();
	}
}

void GameObject::traverseRender(const XMMATRIX & viewProjectionMatrix)
{
	if (enabled == false) return;

	for (auto comp : components)
	{
		comp->Render(viewProjectionMatrix);
	}
	for (auto child : children)
	{
		child->traverseRender(viewProjectionMatrix);
	}
}

void GameObject::traverseDestroy()
{
	for (auto comp : components)
	{
		comp->Destroy();
	}
	for (auto child : children)
	{
		child->traverseDestroy();
	}
}
