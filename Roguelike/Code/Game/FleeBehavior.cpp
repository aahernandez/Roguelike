#include "Game/FleeBehavior.hpp"
#include "Game/Player.hpp"
#include "Game/Map.hpp"
#include "Engine/Math/IntVector2.hpp"

FleeBehavior* Behavior::s_fleeBehavior = new FleeBehavior();

FleeBehavior::FleeBehavior()
	: m_fleePathing(0.f)
{
	m_behaviorName = "Flee";
}

FleeBehavior::FleeBehavior(FleeBehavior *behaviorCopy)
{
	m_behaviorRange = behaviorCopy->m_behaviorRange;
	m_behaviorName = behaviorCopy->m_behaviorName;
}

FleeBehavior::~FleeBehavior()
{

}

void FleeBehavior::Init(Character *character)
{
	character->m_path.clear();
}

float FleeBehavior::CalcUtility(Character *character) const
{
	if (character->m_target == IntVector2::ZERO)
		return 0.f;
	else
	{
		IntVector2 targetPos = character->m_target;
		IntVector2 characterPos = character->GetMapCoords();
		int distance = CalculateManhattanDistance(characterPos, targetPos);
		if (distance < m_behaviorRange)
		{
			float percentHealthRemaining = (float)character->m_currentHealth / (float)character->m_maxHealth;
			float percentHealthMissing = 1.f - percentHealthRemaining;
			return percentHealthMissing + m_skittishness + m_fleePathing;
		}
		else
			return 0.f;
	}
}

void FleeBehavior::Update(float deltaSeconds) const
{
	deltaSeconds;
}

void FleeBehavior::Act(Character *character)
{
	if (character->m_path.empty())
	{
		int bestManhattan = 0;
		for (int newTargetCount = 0; newTargetCount < 10; newTargetCount++)
		{
			IntVector2 newPos = character->m_owningMap->FindRandomPassableTileOfType()->m_mapPosition;
			int newManhattan = CalculateManhattanDistance(character->GetMapCoords(), newPos);
			if (newManhattan > bestManhattan)
			{
				bestManhattan = newManhattan;
				character->m_target = newPos;
			}

		}
		character->m_path = character->m_owningMap->GeneratePath(character->GetMapCoords(), character->m_target, character);
		character->m_path.erase(character->m_path.end() - 1);
	}
	character->MoveToNewTile(*(character->m_path.end() - 1));
	if (character->m_owningTile == *(character->m_path.end() - 1))
		character->m_path.erase(character->m_path.end() - 1);
	if (!character->m_path.empty())
		m_fleePathing = 1.f;
}

FleeBehavior* FleeBehavior::Clone() const
{
	return new FleeBehavior(*this);
}

