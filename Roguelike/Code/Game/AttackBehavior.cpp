#include "Game/AttackBehavior.hpp"
#include "Game/Character.hpp"
#include "Game/Map.hpp"
#include "Engine/Math/IntVector2.hpp"

AttackBehavior* Behavior::s_attackBehavior = new AttackBehavior();

AttackBehavior::AttackBehavior()
{
	m_behaviorName = "Attack";
}

AttackBehavior::AttackBehavior(AttackBehavior *behaviorCopy)
{
	m_behaviorRange = behaviorCopy->m_behaviorRange;
	m_behaviorName = behaviorCopy->m_behaviorName;
}

AttackBehavior::~AttackBehavior()
{

}

void AttackBehavior::Init(Character *character)
{
	character;
}

float AttackBehavior::CalcUtility(Character *character) const
{
	if (character->m_target == IntVector2(0, 0))
		return 0.f;
	else
	{
		int manhattanDistance = CalculateManhattanDistance(character->GetMapCoords(), character->m_target);
		if (manhattanDistance != 1)
			return 0.f;
		else
		{
			return 1.f;
		}
	}
}

void AttackBehavior::Update(float deltaSeconds) const
{
	deltaSeconds;
}

void AttackBehavior::Act(Character *character)
{
	IntVector2 targetPos = character->m_target;
	Tile *targetTile = character->m_owningMap->GetTileAtPosition(targetPos);
	character->MoveToNewTile(targetTile);
}

AttackBehavior* AttackBehavior::Clone() const
{
	return new AttackBehavior(*this);
}
