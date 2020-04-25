#pragma once

#include <iostream>
#include <string>
using namespace std;


class GameObject;
class Transform;


class Component
{
public:
	Component(GameObject* gameobject);

protected:
	GameObject* gameobject;
	Transform* transform;

protected:
	friend class GameObject;
	virtual void Container();
	virtual void UpdateMatrix();
};
