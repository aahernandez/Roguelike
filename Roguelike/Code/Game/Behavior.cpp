#include "Game/Behavior.hpp"
#include "Game/PathGenerator.hpp"

Behavior::Behavior()
{

}

Behavior::~Behavior()
{

}

float Behavior::CalcUtility(Character *character) const
{
	character;
	return 0.f;
}

void Behavior::Update(float deltaSeconds) const
{
	deltaSeconds;
}
