#include "Game/MapGeneratorPerlinNoise.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/Map.hpp"
#include "Engine/Core/Noise.hpp"

MapGeneratorPerlinNoise::MapGeneratorPerlinNoise(const ITCXMLNode& generatorNode)
{
	ValidateXmlElement(generatorNode, "Rules", "name,scale,numOctaves,persistance,octaveScale,chanceToRun");
	m_name = ParseXmlAttribute(generatorNode, "name", (std::string)"ERROR_NO_NAME_FOUND");
	m_scale = ParseXmlAttribute(generatorNode, "scale", 0.f);
	m_numOctaves = ParseXmlAttribute(generatorNode, "numOctaves", (int)0);
	m_persistance = ParseXmlAttribute(generatorNode, "persistance", 0.f);
	m_octaveScale = ParseXmlAttribute(generatorNode, "octaveScale", 0.f);
	m_chanceToRun = ParseXmlAttribute(generatorNode, "chanceToRun", 0.f);

	ITCXMLNode rulesNode = generatorNode.getChildNode("Rules");
	LoadRules(rulesNode);
}

MapGeneratorPerlinNoise::~MapGeneratorPerlinNoise()
{

}

void MapGeneratorPerlinNoise::LoadRules(const ITCXMLNode& rulesNode)
{
	ValidateXmlElement(rulesNode, "Rule", "");
	for (int rulesCount = 0; rulesCount < rulesNode.nChildNode(); rulesCount++)
	{
		ITCXMLNode singleRuleNode = rulesNode.getChildNode(rulesCount);
		ValidateXmlElement(singleRuleNode, "", "ifTile,ifRange,newTile");
		PerlinRule newRule;
		newRule.m_ifRange = ParseXmlAttribute(singleRuleNode, "ifRange", Range(0,0));
		std::string tileName = ParseXmlAttribute(singleRuleNode, "ifTile", (std::string)"ERROR_NO_TILENAME_FOUND");
		newRule.m_ifTile = TileDefinition::s_registry[tileName];
		tileName = ParseXmlAttribute(singleRuleNode, "newTile", (std::string)"ERROR_NO_TILENAME_FOUND");
		newRule.m_newTile = TileDefinition::s_registry[tileName];
		m_rules.push_back(newRule);
	}
}

void MapGeneratorPerlinNoise::PopulateMap(Map *mapToPopulate)
{
	for (int tileCount = 0; tileCount < (int)mapToPopulate->m_tiles.size(); tileCount++)
	{
		Tile &currentTile = mapToPopulate->m_tiles[tileCount];
		float perlinNoise = Compute2dPerlinNoise((float)currentTile.m_mapPosition.x, (float)currentTile.m_mapPosition.y, m_scale, m_numOctaves, m_persistance, m_octaveScale, true, 24 );
		
		for (int ruleIndex = 0; ruleIndex < (int)m_rules.size(); ruleIndex++)
		{
			PerlinRule &currentRule = m_rules[ruleIndex];

			if (currentTile.m_tileDefition != currentRule.m_ifTile)
				continue;;

			if (!currentRule.m_ifRange.IsFloatInRange(perlinNoise))
				continue;

			if (m_chanceToRun > GetRandomFloatZeroToOne())
				currentTile.SetTileDefinition(currentRule.m_newTile);
		}
	}
}