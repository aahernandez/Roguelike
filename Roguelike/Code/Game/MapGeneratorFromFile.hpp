#pragma once
#include "Game/MapGenerator.hpp"
#include <string>

class Map;
class TileDefinition;

class MapGeneratorFromFile : public MapGenerator
{
public:
	std::string m_filePath;
	IntRange m_offsetX;
	IntRange m_offsetY;
	bool m_isMirrored;
	IntRange m_numRightAngleRotations;
	std::map<const char, TileDefinition*> m_keys;

	MapGeneratorFromFile(const ITCXMLNode& xmlNode);
	~MapGeneratorFromFile();

	virtual void PopulateMap(Map *mapToPopulate) override;
	void LoadKeys(const ITCXMLNode& xmlNode);
	IntVector2 GetRotatedPosition(int numRotation, const IntVector2& position);
};