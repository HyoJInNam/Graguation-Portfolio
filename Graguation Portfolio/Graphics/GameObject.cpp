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

	ImGui::Text(name.c_str());
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
