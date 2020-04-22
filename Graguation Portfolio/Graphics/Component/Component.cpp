#include "Component.h"
#include "GameObject.h"


Component::Component(GameObject *gameobject)
	:gameobject(gameobject)
{
	transform = gameobject->getTransform();
}

void Component::UpdateMatrix()
{
	assert("UpdateMatrix must be overridden." && 0);
}

void Component::awake()
{
}

void Component::onEnable()
{
}

void Component::start()
{
}

void Component::fixedUpdate()
{
}

void Component::update()
{
}

void Component::lateUpdate()
{
}

void Component::onDisable()
{
}

void Component::onDestroy()
{
}
