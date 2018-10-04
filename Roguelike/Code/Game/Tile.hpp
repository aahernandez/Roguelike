#pragma once
#include "Engine/Core/RGBA.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/AABB2D.hpp"
#include "Game/Character.hpp"
#include "Game/Feature.hpp"
#include "Game/Inventory.hpp"

class TileDefinition;
class Log;

enum SeenState
{
	NEVER_SEEN,
	PREVIOUSLY_SEEN,
	CURRENTLY_SEEN
};

class Tile
{
public: 
	IntVector2 m_mapPosition;
	char m_glyph;
	RGBA m_fillColor;
	RGBA m_glyphColor;
	Feature *m_feature;
	Inventory *m_inventory;
	Character *m_character;
	TileDefinition *m_tileDefition;
	SeenState m_currentSeenState;

	Tile();
	Tile(const IntVector2 &mapPosition);
	~Tile();

	void Initialize();
	
	void FillLogWithDetails(Log *log);
	void SetTileDefinition(TileDefinition *tileDefinition);
	void InteractWithCharacter();

	void Render() const;

	bool IsSolid();
	bool IsPassable();
	bool HasCharacter();
	bool HasFeature();
	void DestroyFeature();
	float GetLocalGCost();
	std::string GetName();
	Vector2 GetCenter() const;
	AABB2D GetTileBounds() const;
};