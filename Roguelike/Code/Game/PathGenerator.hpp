#pragma once
#include "Engine/Math/IntVector2.hpp"
#include "Game/Map.hpp"

class Tile;

struct OpenTile
{
public:
	Tile *m_tile = nullptr;
	OpenTile *m_origin = nullptr;
	float m_gLocalCost;
	float m_gTotalCost;
	float m_hEstimate;
	float m_fScore;
};

class PathGenerator
{
public:
	OpenTile *m_currentTile;
	IntVector2 m_start;
	IntVector2 m_end;
	Map& m_map;
	Character *m_gCostCharacter;
	std::vector<OpenTile*> m_openList;
	std::vector<OpenTile*> m_closedList;
	bool m_isReturningPathToCurrentTile;

	PathGenerator(const IntVector2& start, const IntVector2& end, Map& map, Character *character);
	~PathGenerator();

	void Destroy();

	void Render();
	void RenderStartAndEnd();
	void RenderClosedList();
	void RenderOpenList();
	static void RenderPath(Path path);

	void SelectAndCloseBestOpenTile();
	bool IsCurrentTileGoal();
	void AddNeighborsToOpenList();
	void AddNeighborToOpenList(const IntVector2& tilePosition);
	void CreatePathToGoal(Path& outPath);
	void AddTileToOpenList(const IntVector2& tilePosition, OpenTile *originTile = nullptr);
	bool IsInClosedList(const IntVector2& tilePosition);
	bool IsInOpenList(const IntVector2& tilePosition);
};
