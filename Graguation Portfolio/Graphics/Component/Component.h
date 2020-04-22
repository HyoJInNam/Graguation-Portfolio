#pragma once


#include <iostream>
#include <string>
#include <vector>
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
	virtual void UpdateMatrix();

protected:
	friend class GameObject;
	virtual void awake();
	virtual void onEnable();
	virtual void start();
	virtual void fixedUpdate();
	virtual void update();
	virtual void lateUpdate();
	virtual void onDisable();
	virtual void onDestroy();
	virtual void draw() {}
};
