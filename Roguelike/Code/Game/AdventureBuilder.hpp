#pragma once
#include "Game/DataDrivenBuilder.hpp"
#include "Game/MapBuilder.hpp"
#include <vector>

class World;

struct Level
{
public:
	std::string m_name;
	std::string m_mapDefName;
	std::string m_exitFeatureName;
	std::string m_destinationEntranceFeatureName;
	std::string m_destinationLevelName;
	std::string m_destinationTileName;
	std::string m_itemToSpawnName;
	std::string m_itemSpawnTileName;
};

class AdventureBuilder : DataDrivenBuilder<AdventureBuilder>
{
public:
	std::string m_questName;
	std::string m_startLevelName;
	std::string m_startTileDefName;
	std::vector<std::string> m_victoryItemNames;
	std::vector<std::string> m_introBlurb;
	std::vector<std::string> m_victoryBlurb;
	std::vector<std::string> m_failureBlurb;
	std::vector<Level*> m_levels;

	AdventureBuilder(const ITCXMLNode& xmlNode);
	~AdventureBuilder();

	void LoadStartConditions(const ITCXMLNode& startConditionNode);
	void LoadVictoryConditions(const ITCXMLNode& victoryConditionNode);
	void LoadMapDefintions(const ITCXMLNode& xmlNode);
	void LoadBlurbs(const ITCXMLNode& xmlNode);

	static void LoadAdventuresFromXML();
	static void BuildAdventure(std::string name, World *world);
};