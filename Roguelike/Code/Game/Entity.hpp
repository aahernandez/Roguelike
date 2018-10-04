#pragma once
#include "Engine/Core/RGBA.hpp"
#include "Game/Inventory.hpp"
#include "Game/Item.hpp"
#include "Game/Stats.hpp"
#include "Engine/Math/IntVector2.hpp"
#include <string>

class Map;
class Tile;
class Log;

class Entity
{
public:
	char m_glyph;
	Stats m_stats;
	bool m_isDead;
	int m_maxHealth;
	Log *m_damageLog;
	Map *m_owningMap;
	RGBA m_fillColor;
	RGBA m_glyphColor;
	std::string m_name;
	Tile *m_owningTile;
	IntVector2 m_target;
	int m_currentHealth;
	Vector2 m_logPosition;
	Inventory *m_inventory;
	std::vector<Character*> m_charactersInSight;
	
	Entity();
	~Entity();

	void RemoveSelfFromOwningTile();
	void Update(float deltaSeconds);
	void Render() const;
	virtual void TakeDamage(int damage);
	int CalcDamage(StatName statName);
	IntVector2 GetMapCoords();
	void DropInventoryOnTile();
	bool HasLineOfSight(Character *character);
};