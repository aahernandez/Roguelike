#pragma once
#include "Game/Behavior.hpp"
#include "ThirdParty/IXMLParser/IXMLParser.h"

class PursueBehavior : public Behavior
{
public:
	PursueBehavior();
	PursueBehavior(PursueBehavior *behaviorCopy);
	virtual ~PursueBehavior() override;
	virtual void Init(Character *character) override;
	virtual float CalcUtility(Character *character) const override;
	virtual void Update(float deltaSeconds) const override;
	virtual void Act(Character *character) override;
	virtual PursueBehavior* Clone() const override;
};