#pragma once
#include "Game/Tile.hpp"
#include "Game/Entity.hpp"
#include "Engine/Math/RaycastResults2D.hpp"
#include <vector>

class VertexBuffer;
class IndexBuffer;
class Log;
class PathGenerator;

typedef std::vector<Tile*> Path;

class Map
{
public:
	std::string m_name;
	std::vector<Tile> m_tiles;
	std::vector<Feature*> m_features;
	std::vector<Character*> m_characters;
	PathGenerator *m_currentPath;
	IntVector2 m_dimensions;
	Tile *m_tileUnderMouse;

	Map();
	Map(IntVector2 dimensions);
	~Map();

	void Initialize();
	void InitTempCharacters();
	void InitTempFeatures();
	void InitTempItems();
	void InitSingleTempItem(std::string itemName);

	void Update(float deltaSeconds);
	void Render();

	void FillLogWithTileInfo(Log *log, const Vector2 &mousePos);
	void ProcessTurn();
	void ResolveAttack(Character* attacker, Tile *defendingTile);
	void SetVisibleTiles();
	void SetTileVisible(Tile *newVisibleTile);
	void ClearVisibleTiles();

	Tile* FindFirstNonSolidTile();
	Tile* FindRandomPassableTileOfType(std::string tileType = "");
	Tile* FindRandomTile();
	Tile* GetTileAtPosition(const IntVector2& position);
	int GetTileIndexFromCoords(const IntVector2& position);
	IntVector2 GetTileCoordsFromIndex(int index);
	Tile* GetTileToNorth(Tile* currentTile);
	Tile* GetTileToSouth(Tile* currentTile);
	Tile* GetTileToWest(Tile* currentTile);
	Tile* GetTileToEast(Tile* currentTile);
	Tile* GetTileToNorthEast(Tile *currentTile);
	Tile* GetTileToNorthWest(Tile *currentTile);
	Tile* GetTileToSouthEast(Tile *currentTile);
	Tile* GetTileToSouthWest(Tile *currentTile);
	int CalcNumNeighborsOfType(Tile *currentTile, TileDefinition *tileDef);
	bool IsOutOfBounds(const IntVector2& position);
	bool IsTilePositionSolid(const IntVector2& position);
	bool IsTilePositionPassable(const IntVector2& position);
	Vector2 Raycast(const Vector2& start, const Vector2& end, bool canHitOpaque, bool canHitSolid); 
	RaycastResults2D RaycastAW(const Vector2& start, const Vector2& end, bool canHitOpaque, bool canHitSolid);

	Character* FindNearestActorOfType(const IntVector2& position, std::string typeName);
	Character* FindNearestActorNotOfType(const IntVector2& position, std::string typeName);
	Character* FindNearestActorOfFaction(const IntVector2& position, std::string factionName);
	Character* FindNearestActorNotOfFaction(const IntVector2& position, std::string factionName);
	Tile *FindNearestTileOfType(const IntVector2& position, std::string tileType);
	Tile *FindNearestTileNotOfType(const IntVector2& position, std::string tileType);

	Path GeneratePath(IntVector2 start, IntVector2 end, Character *character = nullptr);
	void StartSteppedPath(IntVector2 start, IntVector2 end, Character *character = nullptr);
	bool ContinueSteppedPath(Path& outPath);

	void ClearLineOfSights();
	void DetermineLineOfSights();
};