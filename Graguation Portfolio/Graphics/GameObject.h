#pragma once

#include "Component.h"
#include "Transform.h"
#include "Renderer.h"
#include "Camera.h"
#include "Light.h"
#include "Terrain.h"

#include "Canvas.h"
#include "Sprite.h"

class GameObject
{
protected:
	bool		enabled;
	string		name;
	string		tag;
	Transform* transform;
	vector<Component*> components;

	GameObject* parent;
	vector<GameObject*> children;

public:
	static vector<GameObject *> gameObjects;
	

public:	
	GameObject(const string& name = "GameObejct", GameObject* parent = nullptr, const string& tag = "");
	string getName() const;

	bool isActive();
	void setActive(bool flag = true);

	void setTransform(Transform transform);
	void setPosition(const XMFLOAT3& pos);
	void setRotation(const XMFLOAT3& rot);
	void setScale(const XMFLOAT3& scale);


	Transform* getTransform();
	const XMVECTOR& getPosition();
	const XMVECTOR& getRotation();
	const XMVECTOR& getScale();
	const XMFLOAT3& getPositionToF();
	const XMFLOAT3& getRotationToF();
	const XMFLOAT3& getScaleToF();


	template<typename T>
	void addComponent() {
		try
		{
			T t{ this };
			if (dynamic_cast<Component*>(&t) == nullptr) {
				return;
			}
			components.push_back(new T{ this });
		}
		catch (COMException & exception)
		{
			ErrorLogger::Log(exception);
			return;
		}
	}

	template<typename T>
	T* getComponent() {
		try
		{
			for (auto comp : components)
			{
				if (dynamic_cast<T*>(comp) != nullptr) {
					return static_cast<T*>(comp);
				}
				
			}
		}
		catch (COMException & exception)
		{
			ErrorLogger::Log(exception);
			return nullptr;
		}
		return nullptr;
	}

	void setParent(GameObject* parent);
	GameObject* getParent();

	GameObject* Find(const string& path);
	static vector<GameObject*> allFind(const string& path);

	void inspector();
	void traverseUpdate();
	void traverseRender(const XMMATRIX & viewProjectionMatrix);
	void traverseDestroy();
};