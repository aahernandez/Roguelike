#pragma once
#include "Game/DataDrivenBuilder.hpp"
#include <string>

class MapGenerator;
class Map;

class MapBuilder : public DataDrivenBuilder<MapBuilder>
{
public:
	std::vector<MapGenerator*> m_generators;
	
	static Map *m_currentOwningMap;
	static std::string m_currentGeneratorName;
	static std::vector<MapGenerator*>::iterator m_currentGeneratorIter;

	MapBuilder(const ITCXMLNode& xmlNode);
	~MapBuilder();

	static void LoadMapBuildersFromXML();
	static void BuildMap(std::string name, Map* owningMap);
	static void StartBuilding(std::string name, Map* owningMap);
	static bool ContinueBuilding();
};