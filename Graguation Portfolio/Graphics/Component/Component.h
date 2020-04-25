#pragma once

#include <iostream>
#include <string>
#include <vector>
using namespace std;


#include "../DirectXSetup/ImGuiClass.h"
//using namespace DirectX;



class GameObject;
class Transform;


class Component: public ImGuiEvent
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
