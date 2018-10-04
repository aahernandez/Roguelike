#pragma once
#include "Game/Entity.hpp"

class Log;

class Feature : public Entity
{
public:
	std::string m_name;
	bool m_isSolid;
	bool m_isDestoyable;
	bool m_isOpenable;
	int m_damageDealt;
	IntVector2 m_destinationTilePos;
	Map *m_destinationLevel;

	Feature(Map *owningMap, std::string tileType = "");
	~Feature();

	void Initialize();
	void FillLogWithDetails(Log *log);

	void InteractWithCharacter(Character *character);
};