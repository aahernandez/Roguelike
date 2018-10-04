#pragma once
#include "Game/Behavior.hpp"

class AttackBehavior : public Behavior
{
public:
	AttackBehavior();
	AttackBehavior(AttackBehavior *behaviorCopy);
	virtual ~AttackBehavior() override;
	virtual void Init(Character *character) override;
	virtual float CalcUtility(Character *character) const override;
	virtual void Update(float deltaSeconds) const override;
	virtual void Act(Character *character) override;
	virtual AttackBehavior* Clone() const override;
};