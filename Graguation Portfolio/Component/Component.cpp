#include "Component.h"
#include "GameObject.h"


Component::Component(GameObject *gameobject)
	:gameobject(gameobject)
{
	transform = gameobject->getTransform();
}

void Component::Container()
{
	if (!gameobject) assert("UI Container must be overridden." && 0);
}

void Component::UpdateMatrix()
{
	if(!gameobject) assert("UpdateMatrix must be overridden." && 0);
	gameobject->traverseUpdate();
}

void Component::Render(const XMMATRIX & viewProjectionMatrix)
{
}
