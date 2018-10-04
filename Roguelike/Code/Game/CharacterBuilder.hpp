#pragma once
#include "Engine/Core/RGBA.hpp"
#include "ThirdParty/IXMLParser/IXMLParser.h"
#include "Game/DataDrivenBuilder.hpp"
#include <map>
#include <vector>

class Behavior;
class Character;
class Map;

class CharacterBuilder : DataDrivenBuilder<CharacterBuilder>
{
public:
	std::string m_lootTableName;
	std::string m_faction;
	std::vector<RGBA> m_glyphColors;
	std::vector<char> m_glyphs;
	std::vector<Behavior*> m_behaviors;
	Stats m_stats;

	CharacterBuilder(const ITCXMLNode& xmlNode);
	~CharacterBuilder();

	static void LoadCharactersFromXML();
	static Character* BuildCharacter(std::string name, Map* owningMap);
	
	void LoadBehaviors(const ITCXMLNode& behaviorNode);
	void LoadStats(const ITCXMLNode& statsNode);
};