#pragma once
#include <vector>

class Character;
class Tile;
class WanderBehavior;
class PursueBehavior;
class FleeBehavior;
class AttackBehavior;

class Behavior
{
public:
	std::string m_behaviorName;
	int m_behaviorRange;
	float m_skittishness;

	static WanderBehavior* s_wanderBehavior;
	static PursueBehavior* s_pursueBehavior;
	static FleeBehavior* s_fleeBehavior;
	static AttackBehavior* s_attackBehavior;

	Behavior();
	virtual ~Behavior();
	virtual void Init(Character *character) = 0;
	virtual float CalcUtility(Character *character) const;
	virtual void Update(float deltaSeconds) const;
	virtual void Act(Character *character) = 0;
	virtual Behavior* Clone() const = 0;
};