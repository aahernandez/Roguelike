#include "Game/MapBuilder.hpp"
#include "Game/MapGenerator.hpp"
#include "Game/Map.hpp"

Map *MapBuilder::m_currentOwningMap = nullptr;
std::string MapBuilder::m_currentGeneratorName = "";
std::vector<MapGenerator*>::iterator MapBuilder::m_currentGeneratorIter;

MapBuilder::MapBuilder(const ITCXMLNode& xmlNode)
{
	ITCXMLNode generatorsNode = xmlNode.getChildNode("Generators");
	ValidateXmlElement(generatorsNode, "RoomsAndCorridors,FromFile,CellularAutomata,PerlinNoise", "");

	for (int mapGeneratorCount = 0; mapGeneratorCount < generatorsNode.nChildNode(); mapGeneratorCount++)
	{
		ITCXMLNode generatorNode = generatorsNode.getChildNode(mapGeneratorCount);
		if (strcmp(generatorNode.getName(), "RoomsAndCorridors") == 0)
		{
			std::string generatorName = generatorNode.getAttribute("name");
			m_generators.push_back(MapGenerator::s_mapGeneratorRegistry[generatorName]);
		}
		else if (strcmp(generatorNode.getName(), "FromFile") == 0)
		{
			std::string generatorName = generatorNode.getAttribute("name");
			m_generators.push_back(MapGenerator::s_mapGeneratorRegistry[generatorName]);
		}
		else if (strcmp(generatorNode.getName(), "CellularAutomata") == 0)
		{
			std::string generatorName = generatorNode.getAttribute("name");
			m_generators.push_back(MapGenerator::s_mapGeneratorRegistry[generatorName]);
		}
		else if (strcmp(generatorNode.getName(), "PerlinNoise") == 0)
		{
			std::string generatorName = generatorNode.getAttribute("name");
			m_generators.push_back(MapGenerator::s_mapGeneratorRegistry[generatorName]);
		}
	}
}

MapBuilder::~MapBuilder()
{

}

void MapBuilder::LoadMapBuildersFromXML()
{
	IXMLDomParser iDom;
	iDom.setRemoveClears(false);
	ITCXMLNode xMainNode = iDom.openFileHelper("Data/XML/MapBuilders.xml", "MapBuilders");
	ValidateXmlElement(xMainNode, "MapBuilder", "");

	for (int mapBuilderCount = 0; mapBuilderCount < xMainNode.nChildNode(); mapBuilderCount++)
	{
		ITCXMLNode childNode = xMainNode.getChildNode(mapBuilderCount);
		MapBuilder::RegisterNewType(childNode);
	}
}

void MapBuilder::BuildMap(std::string name, Map* owningMap)
{
	std::vector<MapGenerator*>::iterator mapIter;
	for (mapIter = s_registry[name]->m_generators.begin(); mapIter != s_registry[name]->m_generators.end(); ++mapIter)
	{
		(*mapIter)->PopulateMap(owningMap);
	}
}

void MapBuilder::StartBuilding(std::string name, Map* owningMap)
{
	m_currentGeneratorName = name;
	m_currentOwningMap = owningMap;
	m_currentGeneratorIter = s_registry[name]->m_generators.begin();
}

bool MapBuilder::ContinueBuilding()
{

	if (m_currentGeneratorIter != s_registry[m_currentGeneratorName]->m_generators.end())
	{
		(*m_currentGeneratorIter)->PopulateMap(m_currentOwningMap);
		++m_currentGeneratorIter;
		return false;
	}
	return true;
}
