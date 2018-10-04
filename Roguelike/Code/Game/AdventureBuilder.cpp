#include "Game/World.hpp"
#include "Game/Player.hpp"
#include "Game/ItemBuilder.hpp"
#include "Game/FeatureBuilder.hpp"
#include "Game/AdventureBuilder.hpp"
#include "Engine/Core/StringUtils.hpp"

AdventureBuilder::AdventureBuilder(const ITCXMLNode& xmlNode)
{
	ValidateXmlElement(xmlNode, "StartConditions,VictoryConditions,Level,IntroBlurb,VictoryBlurb,FailureBlurb", "name,quest");
	m_name = ParseXmlAttribute(xmlNode, "name", (std::string)"ERROR_NO_NAME_FOUND");
	m_questName = ParseXmlAttribute(xmlNode, "quest", (std::string)"ERROR_NO_NAME_FOUND");

	LoadStartConditions(xmlNode.getChildNode("StartConditions"));
	LoadVictoryConditions(xmlNode.getChildNode("VictoryConditions"));
	LoadBlurbs(xmlNode);
	LoadMapDefintions(xmlNode);
}

AdventureBuilder::~AdventureBuilder()
{

}

void AdventureBuilder::LoadStartConditions(const ITCXMLNode& startConditionNode)
{
	ValidateXmlElement(startConditionNode, "", "startLevel,startTileType");
	m_startLevelName = ParseXmlAttribute(startConditionNode, "startLevel", (std::string)"ERROR_NO_NAME_FOUND");
	m_startTileDefName = ParseXmlAttribute(startConditionNode, "startTileType", (std::string)"ERROR_NO_NAME_FOUND");
}

void AdventureBuilder::LoadVictoryConditions(const ITCXMLNode& victoryConditionNode)
{
	ValidateXmlElement(victoryConditionNode, "", "numTravels,haveItems");
	std::string winItems = ParseXmlAttribute(victoryConditionNode, "haveItems", (std::string)"ERROR_NO_NAME_FOUND");
	m_victoryItemNames = ParseStringIntoPiecesByDelimiter(winItems, ",");
}

void AdventureBuilder::LoadMapDefintions(const ITCXMLNode& xmlNode)
{
	for (int levelCount = 0; levelCount < xmlNode.nChildNode("Level"); levelCount++)
	{
		ITCXMLNode levelNode = xmlNode.getChildNode("Level", levelCount);
		ValidateXmlElement(levelNode, "Exit,Item", "name,mapDefinition");

		ITCXMLNode exitNode = levelNode.getChildNode("Exit");
		ValidateXmlElement(exitNode, "", "type,destinationLevel,destinationTile,createDestinationFeature");
	
		ITCXMLNode itemNode = levelNode.getChildNode("Item");
		ValidateXmlElement(itemNode, "", "type,onTileType");

		Level *newLevel = new Level();
		newLevel->m_name = ParseXmlAttribute(levelNode, "name", (std::string)"ERROR_NO_NAME_FOUND");
		newLevel->m_mapDefName = ParseXmlAttribute(levelNode, "mapDefinition", (std::string)"ERROR_NO_NAME_FOUND");
		newLevel->m_exitFeatureName = ParseXmlAttribute(exitNode, "type", (std::string)"");
		newLevel->m_destinationLevelName = ParseXmlAttribute(exitNode, "destinationLevel", (std::string)"");
		newLevel->m_destinationTileName = ParseXmlAttribute(exitNode, "destinationTile", (std::string)"");
		newLevel->m_destinationEntranceFeatureName = ParseXmlAttribute(exitNode, "createDestinationFeature", (std::string)"");
		newLevel->m_itemToSpawnName = ParseXmlAttribute(itemNode, "type", (std::string)"");
		newLevel->m_itemSpawnTileName = ParseXmlAttribute(itemNode, "onTileType", (std::string)"");
		m_levels.push_back(newLevel);
	}
}

void AdventureBuilder::LoadBlurbs(const ITCXMLNode& xmlNode)
{
	ITCXMLNode introNode =  xmlNode.getChildNode("IntroBlurb");
	const char* introCharacters = introNode.getText(0);
	std::string fileMapLine = "";
	while (*introCharacters != '\0')
	{
		if (*introCharacters != '\n' && *introCharacters != '\t')
		{
			fileMapLine += *introCharacters;
		}
		else if (*introCharacters == '\n')
		{
			m_introBlurb.push_back(fileMapLine);
			fileMapLine = "";
		}
		*introCharacters++;
	}
	m_introBlurb.push_back(fileMapLine);

	ITCXMLNode victoryNode = xmlNode.getChildNode("VictoryBlurb");
	const char* victoryCharacters = victoryNode.getText(0);
	fileMapLine = "";
	while (*victoryCharacters != '\0')
	{
		if (*victoryCharacters != '\n' && *victoryCharacters != '\t')
		{
			fileMapLine += *victoryCharacters;
		}
		else if (*victoryCharacters == '\n')
		{
			m_victoryBlurb.push_back(fileMapLine);
			fileMapLine = "";
		}
		*victoryCharacters++;
	}
	m_victoryBlurb.push_back(fileMapLine);

	ITCXMLNode failureNode = xmlNode.getChildNode("FailureBlurb");
	const char* failureCharacters = failureNode.getText(0);
	fileMapLine = "";
	while (*failureCharacters != '\0')
	{
		if (*failureCharacters != '\n' && *failureCharacters != '\t')
		{
			fileMapLine += *failureCharacters;
		}
		else if (*failureCharacters == '\n')
		{
			m_failureBlurb.push_back(fileMapLine);
			fileMapLine = "";
		}
		*failureCharacters++;
	}
	m_failureBlurb.push_back(fileMapLine);
}

void AdventureBuilder::LoadAdventuresFromXML()
{
	IXMLDomParser iDom;
	iDom.setRemoveClears(false);
	ITCXMLNode xMainNode = iDom.openFileHelper("Data/XML/Adventures.xml", "Adventures");
	ValidateXmlElement(xMainNode, "Adventure", "");

	for (int nodeCount = 0; nodeCount < xMainNode.nChildNode("Adventure"); nodeCount++)
	{
		ITCXMLNode childNode = xMainNode.getChildNode("Adventure", nodeCount);
		RegisterNewType(childNode);
	}
}

void AdventureBuilder::BuildAdventure(std::string name, World *world)
{
	AdventureBuilder *builder = s_registry[name];

	world->m_questName = builder->m_questName;
	world->m_questItemNames = builder->m_victoryItemNames;
	world->m_introBlurb = builder->m_introBlurb;
	world->m_victoryBlurb = builder->m_victoryBlurb;
	world->m_failureBlurb = builder->m_failureBlurb;

	for (int levelCount = 0; levelCount < (int)builder->m_levels.size(); levelCount++)
	{
		Level *currentLevel = builder->m_levels[levelCount];
		Map *newMap = new Map(IntVector2(40,33));
		MapBuilder::BuildMap(currentLevel->m_mapDefName, newMap);
		newMap->Initialize();
		world->m_maps[currentLevel->m_name] = newMap;

		if (!currentLevel->m_itemToSpawnName.empty())
		{
			Item *newItem = ItemBuilder::BuildItem(currentLevel->m_itemToSpawnName);
			Tile *itemTile = newMap->FindRandomPassableTileOfType(currentLevel->m_itemSpawnTileName);
			itemTile->m_inventory->m_items.push_back(newItem);
		}
	}

	for (int levelCount = 0; levelCount < (int)builder->m_levels.size(); levelCount++)
	{
		Level *currentLevel = builder->m_levels[levelCount];
	
		if (!currentLevel->m_exitFeatureName.empty())
		{
			Map *currentMap = world->m_maps[currentLevel->m_name];
			Map *destinationMap = world->m_maps[currentLevel->m_destinationLevelName];

			currentMap->m_features.push_back(FeatureBuilder::BuildFeature(currentLevel->m_exitFeatureName, currentMap));
			Feature *exitFeature = currentMap->m_features.back();
			exitFeature->m_destinationLevel = destinationMap;

			if (!currentLevel->m_destinationEntranceFeatureName.empty())
			{
				destinationMap->m_features.push_back(FeatureBuilder::BuildFeature(currentLevel->m_destinationEntranceFeatureName, destinationMap, currentLevel->m_destinationTileName));
				Feature *destinationFeatue = destinationMap->m_features.back();
				destinationFeatue->m_destinationLevel = currentMap;

				exitFeature->m_destinationTilePos = destinationFeatue->m_owningTile->m_mapPosition;
				destinationFeatue->m_destinationTilePos = exitFeature->m_owningTile->m_mapPosition;
			}
			else if (!currentLevel->m_destinationTileName.empty())
			{
				exitFeature->m_destinationTilePos = destinationMap->FindRandomPassableTileOfType(currentLevel->m_destinationTileName)->m_mapPosition;
			}
		}
	}

	world->m_currentMap = world->m_maps[builder->m_startLevelName];
	Player::s_player->m_owningMap = world->m_currentMap;
	Tile *playerTile = world->m_currentMap->FindRandomPassableTileOfType(builder->m_startTileDefName);
	Player::s_player->PutSelfOnNewTile(playerTile);
	world->m_currentMap->m_characters.insert(world->m_currentMap->m_characters.begin(), Player::s_player);
	world->m_currentMap->ClearLineOfSights();
	world->m_currentMap->DetermineLineOfSights();
}

