#pragma once
#include "Engine/Core/FileUtils.hpp"
#include <string>
#include <map>

class Map;

class MapGenerator
{
public:
	std::string m_name;
	static std::map<std::string, MapGenerator*> s_mapGeneratorRegistry;

	MapGenerator();
	~MapGenerator();

	static void LoadGeneratorsFromFile();
	static void DestroyRegistry();

	virtual void PopulateMap(Map *mapToPopulate) = 0;
};