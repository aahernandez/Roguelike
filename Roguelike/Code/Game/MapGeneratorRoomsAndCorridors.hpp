#pragma once
#include "Game/MapGenerator.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/AABB2D.hpp"
#include "ThirdParty/IXMLParser/IXMLParser.h"
#include <string>
#include <vector>

class TileDefinition;
class Map;

struct Room
{
	const AABB2D roomBoundingBox;
};

class MapGeneratorRoomsAndCorridors : public MapGenerator
{
public:
	int m_maxRooms;
	IntRange m_widthRange;
	IntRange m_heightRange;
	TileDefinition *m_wallTile;
	TileDefinition *m_roomTile;
	TileDefinition *m_pathTile;
	bool m_canOverlap;
	float m_pathStraightness;

	MapGeneratorRoomsAndCorridors(const ITCXMLNode& generatorNode);
	~MapGeneratorRoomsAndCorridors();

	virtual void PopulateMap(Map *mapToPopulate) override;
	void CreateRooms(Map *mapToPopulate, const std::vector<Room>& rooms);
	void CreateWalls(Map *mapToPopulate, const std::vector<Room>& rooms);
	void CreateCorridors(Map *mapToPopulate, const std::vector<Room>& rooms);
	void CreateCorridor(Map *mapToPopulate, const Room &firstRoom, const Room &secondRoom);
	bool IsRoomOutOfBounds(const Room& newRoom, const IntVector2& mapDimensions);
	bool AreRoomsOverlapping(const Room &newRoom, const std::vector<Room> &rooms);
};