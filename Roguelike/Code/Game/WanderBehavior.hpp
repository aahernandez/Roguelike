#pragma once
#include "Game/Behavior.hpp"
#include "ThirdParty/IXMLParser/IXMLParser.h"

class WanderBehavior : public Behavior
{
public:
	WanderBehavior();
	WanderBehavior(WanderBehavior *behaviorCopy);
	virtual ~WanderBehavior() override;
	virtual void Init(Character *character) override;
	virtual float CalcUtility(Character *character) const override;
	virtual void Update(float deltaSeconds) const override;
	virtual void Act(Character *character) override;
	virtual WanderBehavior* Clone() const override;
};