#include "Game/CharacterBuilder.hpp"
#include "Game/WanderBehavior.hpp"
#include "Game/PursueBehavior.hpp"
#include "Game/FleeBehavior.hpp"
#include "Game/AttackBehavior.hpp"
#include "Game/Character.hpp"
#include "Game/Map.hpp"
#include "Game/LootBuilder.hpp"
#include "Engine/Core/FileUtils.hpp"

CharacterBuilder::CharacterBuilder(const ITCXMLNode& xmlNode)
{
	ValidateXmlElement(xmlNode, "Stats,Glyph,GlyphColor,Behavior,LootTable,Faction", "name");
	m_name = ParseXmlAttribute(xmlNode, "name", (std::string)"ERROR_NO_NAME_FOUND");
	StoreXmlChildNodeAttributes(xmlNode, "Glyph", "glyph", m_glyphs);
	StoreXmlChildNodeAttributes(xmlNode, "GlyphColor", "color", m_glyphColors);
	m_lootTableName = ParseXmlAttribute(xmlNode.getChildNode("LootTable"), "table", (std::string)"ERROR_NO_LOOT_TABLE_FOUND");
	m_faction = ParseXmlAttribute(xmlNode.getChildNode("Faction"), "faction", (std::string)"ERROR_NO_FACTION_FOUND");
	m_stats = Stats(xmlNode.getChildNode("Stats"));
	ITCXMLNode behaviorNode = xmlNode.getChildNode("Behavior");
	LoadBehaviors(behaviorNode);
}

CharacterBuilder::~CharacterBuilder()
{

}

void CharacterBuilder::LoadCharactersFromXML()
{
	IXMLDomParser iDom;
	iDom.setRemoveClears(false);
	ITCXMLNode xMainNode = iDom.openFileHelper("Data/XML/Characters.xml", "Characters");
	ValidateXmlElement(xMainNode, "Character", "");

	for (int nodeCount = 0; nodeCount < xMainNode.nChildNode(); nodeCount++)
	{
		ITCXMLNode childNode = xMainNode.getChildNode(nodeCount);
		RegisterNewType(childNode);
	}
}

Character* CharacterBuilder::BuildCharacter(std::string name, Map* owningMap)
{
	Character *newCharacter = new Character(owningMap, name);
	newCharacter->m_glyph = s_registry[name]->m_glyphs[0];
	newCharacter->m_glyphColor = s_registry[name]->m_glyphColors[0];
	newCharacter->m_stats = s_registry[name]->m_stats;
	newCharacter->m_faction = s_registry[name]->m_faction;
	newCharacter->m_maxHealth = newCharacter->m_stats.m_stats[CONSTITUTION];
	newCharacter->m_currentHealth = newCharacter->m_maxHealth;
	LootBuilder::BuildLoot(s_registry[name]->m_lootTableName, newCharacter->m_inventory);
	newCharacter->EquipBestItems();

	std::vector<Behavior*>::iterator behaviorIter;
	for (behaviorIter = s_registry[name]->m_behaviors.begin(); behaviorIter != s_registry[name]->m_behaviors.end(); ++behaviorIter)
	{
		newCharacter->m_behaviors.push_back((*behaviorIter)->Clone());
	}
	newCharacter->CalcBehaviorUtility();

	return newCharacter;
}

void CharacterBuilder::LoadBehaviors(const ITCXMLNode& behaviorNode)
{
	ValidateXmlElement(behaviorNode, "Wander,Pursue,Flee,Attack", "");
	for (int behaviorCount = 0; behaviorCount < behaviorNode.nChildNode(); behaviorCount++)
	{
		ITCXMLNode childNode = behaviorNode.getChildNode(behaviorCount);

		if (std::strcmp(childNode.getName(),"Wander") == 0)
		{
			m_behaviors.push_back(Behavior::s_wanderBehavior->Clone());
		}
		if (std::strcmp(childNode.getName(), "Pursue") == 0)
		{
			m_behaviors.push_back(Behavior::s_pursueBehavior->Clone());
		}
		if (std::strcmp(childNode.getName(), "Flee") == 0)
		{
			m_behaviors.push_back(Behavior::s_fleeBehavior->Clone());
		}
		if (std::strcmp(childNode.getName(), "Attack") == 0)
		{
			m_behaviors.push_back(Behavior::s_attackBehavior->Clone());
		}
		m_behaviors.back()->m_skittishness = ParseXmlAttribute(childNode, "skittishness", 0.f);
		m_behaviors.back()->m_behaviorRange = ParseXmlAttribute(childNode, "sightRange", 0);
	}
}

void CharacterBuilder::LoadStats(const ITCXMLNode& statsNode)
{
	ValidateXmlElement(statsNode, "", "str,dex,con,int,wis,cha");
	m_stats.m_stats[STRENGTH] =		ParseXmlAttribute(statsNode, "str", 0);
	m_stats.m_stats[DEXTERITY] =	ParseXmlAttribute(statsNode, "dex", 0);
	m_stats.m_stats[CONSTITUTION] = ParseXmlAttribute(statsNode, "con", 0);
	m_stats.m_stats[INTELLIGENCE] = ParseXmlAttribute(statsNode, "int", 0);
	m_stats.m_stats[WISDOM] =		ParseXmlAttribute(statsNode, "wis", 0);
	m_stats.m_stats[CHARISMA] =		ParseXmlAttribute(statsNode, "cha", 0);
}
