#pragma once
#include "Game/Behavior.hpp"
#include "ThirdParty/IXMLParser/IXMLParser.h"

class FleeBehavior : public Behavior
{
public:
	float m_fleePathing;

	FleeBehavior();
	FleeBehavior(FleeBehavior *behaviorCopy);
	virtual ~FleeBehavior() override;
	virtual void Init(Character *character) override;
	virtual float CalcUtility(Character *character) const override;
	virtual void Update(float deltaSeconds) const override;
	virtual void Act(Character *character) override;
	virtual FleeBehavior* Clone() const override;
};