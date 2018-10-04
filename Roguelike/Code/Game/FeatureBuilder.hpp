#pragma once
#include "Game/DataDrivenBuilder.hpp"
#include "Engine/Math/IntVector2.hpp"

class Feature;
class Map;

class FeatureBuilder : DataDrivenBuilder<FeatureBuilder>
{
public:
	bool m_isSolid;
	bool m_isDestoyable;
	bool m_isOpenable;
	int m_damageDealt;
	std::string m_lootTableName;
	std::vector<RGBA> m_glyphColors;
	std::vector<char> m_glyphs;

	FeatureBuilder(const ITCXMLNode& xmlNode);
	~FeatureBuilder();

	static void LoadFeaturesFromXML();
	void LoadAspectsOfFeature(const ITCXMLNode& aspectNode); 
	static Feature* BuildFeature(std::string name, Map* owningMap, std::string tileType = "");
};