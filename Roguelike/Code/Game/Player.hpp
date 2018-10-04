#pragma once
#include "Game/Character.hpp"

class Player : public Character
{
public:
	static Player* s_player;

	Player();
	Player(Map *owningMap);
	~Player();

	void Initialize();
	void SetOwningMap(Map *owningMap);
	virtual void Act() override;
	virtual void FillLogWithDetails(Log *log) override;
	virtual void TakeItemsFromCurrentTile() override;
};