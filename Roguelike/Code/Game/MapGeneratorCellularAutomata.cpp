#include "Game/MapGeneratorCellularAutomata.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/Map.hpp"

MapGeneratorCellularAutomata::MapGeneratorCellularAutomata(const ITCXMLNode& generatorNode)
{
	ValidateXmlElement(generatorNode, "Rules", "name,numIterations,chanceToRun");
	m_name = ParseXmlAttribute(generatorNode, "name", (std::string)"ERROR_NO_NAME_FOUND");
	m_numIterations = ParseXmlAttribute(generatorNode, "numIterations", 0);
	m_chanceToRun = ParseXmlAttribute(generatorNode, "chanceToRun", 0.f);

	ITCXMLNode rulesNode = generatorNode.getChildNode("Rules");
	LoadRules(rulesNode);
}

MapGeneratorCellularAutomata::~MapGeneratorCellularAutomata()
{

}

void MapGeneratorCellularAutomata::LoadRules(const ITCXMLNode& rulesNode)
{
	ValidateXmlElement(rulesNode, "Rule", "");
	for (int rulesCount = 0; rulesCount < rulesNode.nChildNode(); rulesCount++)
	{
		ITCXMLNode singleRuleNode = rulesNode.getChildNode(rulesCount);
		ValidateXmlElement(singleRuleNode, "", "numNeighbors,isFewerThan,ifTile,neighborTile,newTile");
		CellRule newRule;
		newRule.m_numNeighbors = ParseXmlAttribute(singleRuleNode, "numNeighbors", (int)0);
		newRule.m_isfewerThan = ParseXmlAttribute(singleRuleNode, "isFewerThan", false);
		std::string tileName = ParseXmlAttribute(singleRuleNode, "ifTile", (std::string)"ERROR_NO_TILENAME_FOUND");
		newRule.m_ifTile = TileDefinition::s_registry[tileName];
		tileName = ParseXmlAttribute(singleRuleNode, "neighborTile", (std::string)"ERROR_NO_TILENAME_FOUND");
		newRule.m_neighborTile = TileDefinition::s_registry[tileName];
		tileName = ParseXmlAttribute(singleRuleNode, "newTile", (std::string)"ERROR_NO_TILENAME_FOUND");
		newRule.m_newTile = TileDefinition::s_registry[tileName];
		m_rules.push_back(newRule);
	}
}

void MapGeneratorCellularAutomata::PopulateMap(Map *mapToPopulate)
{
	for (int iterationCount = 0; iterationCount < m_numIterations; iterationCount++)
	{
		for (int rulesCount = 0; rulesCount < (int)m_rules.size(); rulesCount++)
		{
			CellRule &currentRule = m_rules[rulesCount];
			for (int tileCount = 0; tileCount < (int)mapToPopulate->m_tiles.size(); tileCount++)
			{
				Tile &currentTile = mapToPopulate->m_tiles[tileCount];

				if (currentTile.m_tileDefition != currentRule.m_ifTile)
					continue;

				int numNeighborsOfType = mapToPopulate->CalcNumNeighborsOfType(&currentTile, currentRule.m_neighborTile);

				bool isRunning = m_chanceToRun > GetRandomFloatZeroToOne();

				if (numNeighborsOfType < currentRule.m_numNeighbors && currentRule.m_isfewerThan && isRunning)
				{
					currentTile.m_tileDefition = currentRule.m_newTile;
				}
				else if (numNeighborsOfType >= currentRule.m_numNeighbors && !currentRule.m_isfewerThan && isRunning)
				{
					currentTile.SetTileDefinition(currentRule.m_newTile);
				}
			}
		}
	}
}
