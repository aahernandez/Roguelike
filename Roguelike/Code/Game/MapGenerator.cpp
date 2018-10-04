#pragma once
#include "Game/MapGenerator.hpp"
#include "Game/MapGeneratorRoomsAndCorridors.hpp"
#include "Game/MapGeneratorCellularAutomata.hpp"
#include "Game/MapGeneratorPerlinNoise.hpp"
#include "Game/MapGeneratorFromFile.hpp"

std::map<std::string, MapGenerator*> MapGenerator::s_mapGeneratorRegistry;

MapGenerator::MapGenerator()
{

}

MapGenerator::~MapGenerator()
{

}

void MapGenerator::LoadGeneratorsFromFile()
{
	IXMLDomParser iDom;
	iDom.setRemoveClears(false);
	ITCXMLNode xMainNode = iDom.openFileHelper("Data/XML/MapGenerators.xml", "MapGenerators");
	ValidateXmlElement(xMainNode, "MapGeneratorFromFile,MapGeneratorRoomsAndCorridors,MapGeneratorCellularAutomata,MapGeneratorPerlinNoise", "");

	for (int mapGeneratorCount = 0; mapGeneratorCount < xMainNode.nChildNode(); mapGeneratorCount++)
	{
		ITCXMLNode childNode = xMainNode.getChildNode(mapGeneratorCount);
		if (strcmp(childNode.getName(), "MapGeneratorRoomsAndCorridors") == 0)
		{
			MapGeneratorRoomsAndCorridors *generator = new MapGeneratorRoomsAndCorridors(childNode);
			MapGenerator::s_mapGeneratorRegistry[generator->m_name] = generator;
		}
		else if (strcmp(childNode.getName(), "MapGeneratorCellularAutomata") == 0)
		{
			MapGeneratorCellularAutomata *generator = new MapGeneratorCellularAutomata(childNode);
			MapGenerator::s_mapGeneratorRegistry[generator->m_name] = generator;
		}
		else if (strcmp(childNode.getName(), "MapGeneratorPerlinNoise") == 0)
		{
			MapGeneratorPerlinNoise *generator = new MapGeneratorPerlinNoise(childNode);
			MapGenerator::s_mapGeneratorRegistry[generator->m_name] = generator;
		}
		else if (strcmp(childNode.getName(), "MapGeneratorFromFile") == 0)
		{
			MapGeneratorFromFile *generator = new MapGeneratorFromFile(childNode);
			MapGenerator::s_mapGeneratorRegistry[generator->m_name] = generator;
		}
	}
}

void MapGenerator::DestroyRegistry()
{
	std::map<std::string, MapGenerator*>::iterator mapGenIter;

	for (mapGenIter = s_mapGeneratorRegistry.begin(); mapGenIter != s_mapGeneratorRegistry.end(); ++mapGenIter)
	{
		delete mapGenIter->second;
		mapGenIter->second = nullptr;
	}
	s_mapGeneratorRegistry.clear();
}
