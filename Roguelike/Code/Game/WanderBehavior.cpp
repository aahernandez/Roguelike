#include "Game/WanderBehavior.hpp"
#include "Game/Character.hpp"
#include "Game/Map.hpp"
#include "Engine/Math/MathUtilities.hpp"

WanderBehavior* Behavior::s_wanderBehavior = new WanderBehavior();

WanderBehavior::WanderBehavior()
{
	m_behaviorName = "Wander";
}

WanderBehavior::WanderBehavior(WanderBehavior *behaviorCopy)
{
	m_behaviorRange = behaviorCopy->m_behaviorRange;
	m_behaviorName = behaviorCopy->m_behaviorName;
}

WanderBehavior::~WanderBehavior()
{

}

void WanderBehavior::Init(Character *character)
{
	character;
}

float WanderBehavior::CalcUtility(Character *character) const
{
	character;
	return 0.1f;
}

void WanderBehavior::Update(float deltaSeconds) const
{
	deltaSeconds;
}

void WanderBehavior::Act(Character *character) 
{
	if (character->m_path.empty())
	{
		character->m_target = character->m_owningMap->FindRandomPassableTileOfType()->m_mapPosition;
		character->m_path = character->m_owningMap->GeneratePath(character->GetMapCoords(), character->m_target, character);
		character->m_path.erase(character->m_path.end() - 1);
	}
	character->MoveToNewTile(*(character->m_path.end() - 1));
	if (character->m_owningTile == *(character->m_path.end() - 1))
		character->m_path.erase(character->m_path.end() - 1);

	Character *nearestEnemy = character->m_owningMap->FindNearestActorNotOfFaction(character->GetMapCoords(), character->m_faction);
	if (nearestEnemy != nullptr && character->HasLineOfSight(nearestEnemy))
	{
		character->m_target = nearestEnemy->GetMapCoords();
// 		character->m_path.clear();
	}
}

WanderBehavior* WanderBehavior::Clone() const
{
	return new WanderBehavior(*this);
}

