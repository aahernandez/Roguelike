#include "Game/PursueBehavior.hpp"
#include "Game/Player.hpp"
#include "Game/Map.hpp"
#include "Engine/Math/IntVector2.hpp"

PursueBehavior* Behavior::s_pursueBehavior = new PursueBehavior();

PursueBehavior::PursueBehavior()
{
	m_behaviorName = "Pursue";
}

PursueBehavior::PursueBehavior(PursueBehavior *behaviorCopy)
{
	m_behaviorRange = behaviorCopy->m_behaviorRange;
	m_behaviorName = behaviorCopy->m_behaviorName;
}

PursueBehavior::~PursueBehavior()
{

}

void PursueBehavior::Init(Character *character)
{
	character;
}

float PursueBehavior::CalcUtility(Character *character) const
{
	if (character->m_target == IntVector2::ZERO || character->m_path.empty())
		return 0.f;
	else
	{
		IntVector2 targetPos = character->m_target;
		IntVector2 characterPos = character->GetMapCoords();
		int distance = CalculateManhattanDistance(targetPos, targetPos);
		if (distance < m_behaviorRange)
			return 0.5f;
		else 
			return 0.f;
	}

}

void PursueBehavior::Update(float deltaSeconds) const
{
	deltaSeconds;
}

void PursueBehavior::Act(Character *character)
{
	IntVector2 targetPos = character->m_target;
	if (character->m_owningMap->GetTileAtPosition(targetPos)->m_character == nullptr)
	{
		Character *nearestEnemy = character->m_owningMap->FindNearestActorNotOfFaction(character->GetMapCoords(), character->m_faction);
		if (nearestEnemy != nullptr && character->HasLineOfSight(nearestEnemy))
		{
			character->m_target = nearestEnemy->GetMapCoords();
		}
	}
	targetPos = character->m_target;
	IntVector2 characterPos = character->GetMapCoords();

	if (!character->m_path.empty())
		character->m_path.clear();
	character->m_path = character->m_owningMap->GeneratePath(characterPos, targetPos, character);
	
	character->m_path.erase(character->m_path.end() - 1);
	if (character->m_path.empty())
		return;
	character->MoveToNewTile(*(character->m_path.end() - 1));
}

PursueBehavior* PursueBehavior::Clone() const
{
	return new PursueBehavior(*this);
}

