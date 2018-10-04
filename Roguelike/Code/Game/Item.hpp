#pragma once
#include "Game/Stats.hpp"
#include "Engine/Core/RGBA.hpp"
#include "Engine/Math/IntRange.hpp"
#include <string>
#include <vector>

class Tile;
class Character;
class Feature;
class Vector2;
class Log;

enum EquipSlot
{
	NONE = -1,
	HEAD,
	NECKLACE,
	SHOULDERS,
	LEFTHAND,
	RIGHTHAND,
	GLOVES,
	CHEST,
	LEGS,
	FEET,
	NUM_EQUIPMENT_SLOTS
};

class Item
{
public:
	std::string m_name;
	EquipSlot m_equipSlot;
	RGBA m_glyphColor;
	char m_glyph;
	Stats m_stats;
	IntRange healAmount;

	Item();
	~Item();

	void Update(float deltaSeconds);
	void Render(const Vector2 &position) const;

	void FillLogWithDetails(Log *log, bool isLoggingEquipData = false);
	std::string GetEquipSlotAsString();
	static std::string GetEquipSlotAsString(EquipSlot equipSlot);
};