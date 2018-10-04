#include "Game/MapGeneratorRoomsAndCorridors.hpp"
#include "Game/Map.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

MapGeneratorRoomsAndCorridors::MapGeneratorRoomsAndCorridors(const ITCXMLNode& generatorNode)
{
	ValidateXmlElement(generatorNode, "", "name,maxRooms,roomWidthRange,roomHeightRange,defaultTile,interiorTile,pathTile,canOverlap,pathStraightness");
	m_name = ParseXmlAttribute(generatorNode, "name", (std::string)"ERROR_NO_NAME_FOUND");
	m_maxRooms = ParseXmlAttribute(generatorNode, "maxRooms", 0);
	m_widthRange = ParseXmlAttribute(generatorNode, "roomWidthRange", IntRange(0, 0));
	m_heightRange = ParseXmlAttribute(generatorNode, "roomHeightRange", IntRange(0, 0));
	std::string tileName = ParseXmlAttribute(generatorNode, "defaultTile", (std::string)"ERROR_NO_TILENAME_FOUND");
	m_wallTile = TileDefinition::s_registry[tileName];
	tileName = ParseXmlAttribute(generatorNode, "interiorTile", (std::string)"ERROR_NO_TILENAME_FOUND");
	m_roomTile = TileDefinition::s_registry[tileName];
	tileName = ParseXmlAttribute(generatorNode, "pathTile", (std::string)"ERROR_NO_TILENAME_FOUND");
	m_pathTile = TileDefinition::s_registry[tileName];
	m_canOverlap = ParseXmlAttribute(generatorNode, "canOverlap", false);
	m_pathStraightness = ParseXmlAttribute(generatorNode, "pathStraightness", 0.f);
}

MapGeneratorRoomsAndCorridors::~MapGeneratorRoomsAndCorridors()
{

}

void MapGeneratorRoomsAndCorridors::PopulateMap(Map *mapToPopulate)
{	
	std::vector<Room> rooms;
	int numberOfTries = 0;
	int roomsMade = 0;
	while (roomsMade < m_maxRooms)
	{
		IntVector2 newRoomSize;
		newRoomSize.x = m_widthRange.GetIntInRange();
		newRoomSize.y = m_heightRange.GetIntInRange();
		IntVector2 newRoomPosition = mapToPopulate->FindRandomTile()->m_mapPosition;
		Room newRoom{AABB2D(newRoomPosition, newRoomPosition + newRoomSize)};
		bool isOverlapping = false;
		if (!m_canOverlap)
			isOverlapping = AreRoomsOverlapping(newRoom, rooms);
		if (IsRoomOutOfBounds(newRoom, mapToPopulate->m_dimensions) || isOverlapping)
		{
			if (numberOfTries > 100)
				break;
			numberOfTries++;
			continue;
		}
		else 
		{
			rooms.push_back(newRoom);
		}
		roomsMade++;
		numberOfTries = 0;
	}

	CreateWalls(mapToPopulate, rooms);
	CreateCorridors(mapToPopulate, rooms);
	CreateRooms(mapToPopulate, rooms);
}

void MapGeneratorRoomsAndCorridors::CreateRooms(Map *mapToPopulate, const std::vector<Room>& rooms)
{
	for (int roomCount = 0; roomCount < (int)rooms.size(); roomCount++)
	{
		IntVector2 newRoomPosition = rooms[roomCount].roomBoundingBox.mins;
		IntVector2 newRoomSize = rooms[roomCount].roomBoundingBox.CalcSize();
		for (int xTilesMade = 0; xTilesMade < newRoomSize.x; xTilesMade++)
		{
			for (int yTilesMade = 0; yTilesMade < newRoomSize.y; yTilesMade++)
			{
				IntVector2 currentTilePosition = IntVector2(newRoomPosition.x + xTilesMade, newRoomPosition.y + yTilesMade);
				int tileIndex = mapToPopulate->GetTileIndexFromCoords(currentTilePosition);
				mapToPopulate->m_tiles[tileIndex].SetTileDefinition(m_roomTile);
			}
		}
	}

}

void MapGeneratorRoomsAndCorridors::CreateWalls(Map *mapToPopulate, const std::vector<Room>& rooms)
{
	for (int roomCount = 0; roomCount < (int)rooms.size(); roomCount++)
	{
		IntVector2 bottomLeftRoomCorner = rooms[roomCount].roomBoundingBox.mins;
		IntVector2 roomSize = rooms[roomCount].roomBoundingBox.CalcSize();
		for (int posX = -1; posX < roomSize.x + 1; posX++)
		{
			for (int posY = -1; posY < roomSize.y + 1; posY++)
			{
				IntVector2 currentTilePosition = IntVector2(bottomLeftRoomCorner.x + posX, bottomLeftRoomCorner.y + posY);
				int tileIndex = mapToPopulate->GetTileIndexFromCoords(currentTilePosition);

				if (!rooms[roomCount].roomBoundingBox.IsPointInside(Vector2(currentTilePosition)))
				{
					mapToPopulate->m_tiles[tileIndex].SetTileDefinition(m_wallTile);
				}
			}
		}
	}
}

void MapGeneratorRoomsAndCorridors::CreateCorridors(Map *mapToPopulate, const std::vector<Room>& rooms)
{
	for (int roomCount = 0; roomCount < (int)rooms.size(); roomCount++)
	{
		if (roomCount == (int)rooms.size() - 1)
		{
			CreateCorridor(mapToPopulate, rooms[roomCount], rooms[0]);
		}
		else
		{
			CreateCorridor(mapToPopulate, rooms[roomCount], rooms[roomCount + 1]);
		}
	}
}

void MapGeneratorRoomsAndCorridors::CreateCorridor(Map *mapToPopulate, const Room &firstRoom, const Room &secondRoom)
{
	IntVector2 firstRoomCenter = IntVector2(firstRoom.roomBoundingBox.CalcCenter());
	IntVector2 secondRoomCenter = IntVector2(secondRoom.roomBoundingBox.CalcCenter());
	IntVector2 pathToTravel = firstRoomCenter - secondRoomCenter;
	IntVector2 currentCoordsInMap = firstRoomCenter;

	bool isTravelingAlongY = false;
	while (pathToTravel != IntVector2::ZERO)
	{
		if (pathToTravel.x != 0 && pathToTravel.y != 0)
		{
			float numToDeterineTravelDirection = GetRandomFloatZeroToOne();
			if (numToDeterineTravelDirection > m_pathStraightness)
			{
				isTravelingAlongY = !isTravelingAlongY;
			}
		}
		else if (pathToTravel.x == 0)
		{
			isTravelingAlongY = true;
		}
		else if (pathToTravel.y == 0)
		{
			isTravelingAlongY = false;
		}

		if (isTravelingAlongY)
		{
			if (pathToTravel.y > 0)
			{
				pathToTravel.y--;
				currentCoordsInMap.y--;
				int tileIndex = mapToPopulate->GetTileIndexFromCoords(currentCoordsInMap);
				mapToPopulate->m_tiles[tileIndex].SetTileDefinition(m_pathTile);
			}
			else
			{
				pathToTravel.y++;
				currentCoordsInMap.y++;
				int tileIndex = mapToPopulate->GetTileIndexFromCoords(currentCoordsInMap);
				mapToPopulate->m_tiles[tileIndex].SetTileDefinition(m_pathTile);
			}
		}
		else
		{
			if (pathToTravel.x > 0)
			{
				pathToTravel.x--;
				currentCoordsInMap.x--;
				int tileIndex = mapToPopulate->GetTileIndexFromCoords(currentCoordsInMap);
				mapToPopulate->m_tiles[tileIndex].SetTileDefinition(m_pathTile);
			}
			else
			{
				pathToTravel.x++;
				currentCoordsInMap.x++;
				int tileIndex = mapToPopulate->GetTileIndexFromCoords(currentCoordsInMap);
				mapToPopulate->m_tiles[tileIndex].SetTileDefinition(m_pathTile);
			}
		}
	}
}

bool MapGeneratorRoomsAndCorridors::IsRoomOutOfBounds(const Room& newRoom, const IntVector2& mapDimensions)
{
	if (	newRoom.roomBoundingBox.mins.x < 1
		||	newRoom.roomBoundingBox.mins.y < 1
		||	newRoom.roomBoundingBox.maxs.x > mapDimensions.x - 1
		||	newRoom.roomBoundingBox.maxs.y > mapDimensions.y - 1)
	{
		return true;
	}
	return false;
}

bool MapGeneratorRoomsAndCorridors::AreRoomsOverlapping(const Room &newRoom, const std::vector<Room> &rooms)
{
	for (int roomCount = 0; roomCount < (int)rooms.size(); roomCount++)
	{
		if (DoAABB2DsOverlap(newRoom.roomBoundingBox, rooms[roomCount].roomBoundingBox))
		{
			return true;
		}
	}

	return false;
}
