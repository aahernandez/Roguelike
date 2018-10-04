#include "Game/FeatureBuilder.hpp"
#include "Game/Feature.hpp"
#include "Game/LootBuilder.hpp"

FeatureBuilder::FeatureBuilder(const ITCXMLNode& xmlNode)
{
	ValidateXmlElement(xmlNode, "Aspects,Glyph,GlyphColor,LootTable", "name,isSolid,isDestroyable,isOpenable,damageDealth");
	ITCXMLNode aspectNode = xmlNode.getChildNode("Aspects");
	LoadAspectsOfFeature(aspectNode);
	StoreXmlChildNodeAttributes(xmlNode, "Glyph", "glyph", m_glyphs);
	StoreXmlChildNodeAttributes(xmlNode, "GlyphColor", "color", m_glyphColors);
	m_lootTableName = ParseXmlAttribute(xmlNode.getChildNode("LootTable"), "table", (std::string)"ERROR_NO_LOOT_TABLE_FOUND");
}

FeatureBuilder::~FeatureBuilder()
{
}

void FeatureBuilder::LoadFeaturesFromXML()
{
	IXMLDomParser iDom;
	iDom.setRemoveClears(false);
	ITCXMLNode xMainNode = iDom.openFileHelper("Data/XML/Features.xml", "Features");
	ValidateXmlElement(xMainNode, "Feature", "");

	for (int nodeCount = 0; nodeCount < xMainNode.nChildNode(); nodeCount++)
	{
		ITCXMLNode childNode = xMainNode.getChildNode(nodeCount);
		RegisterNewType(childNode);
	}
}

void FeatureBuilder::LoadAspectsOfFeature(const ITCXMLNode& aspectNode)
{
	m_name = ParseXmlAttribute(aspectNode, "name", (std::string)"ERROR_NO_NAME_FOUND");
	m_isSolid = ParseXmlAttribute(aspectNode, "isSolid", false);
	m_isDestoyable = ParseXmlAttribute(aspectNode, "isDestroyable", false);
	m_isOpenable = ParseXmlAttribute(aspectNode, "isOpenable", false);
	m_damageDealt = ParseXmlAttribute(aspectNode, "damageDealt", 0);
}

Feature* FeatureBuilder::BuildFeature(std::string name, Map* owningMap, std::string tileType /*== ""*/)
{
	Feature *newFeature = new Feature(owningMap, tileType);
	newFeature->m_name = name;
	newFeature->m_isSolid = s_registry[name]->m_isSolid;
	newFeature->m_isDestoyable = s_registry[name]->m_isDestoyable;
	newFeature->m_isOpenable = s_registry[name]->m_isOpenable;
	newFeature->m_damageDealt = s_registry[name]->m_damageDealt;
	newFeature->m_glyph = s_registry[name]->m_glyphs[0];
	newFeature->m_glyphColor = s_registry[name]->m_glyphColors[0];
	if (s_registry[name]->m_lootTableName.compare("ERROR_NO_LOOT_TABLE_FOUND") != 0)
		LootBuilder::BuildLoot(s_registry[name]->m_lootTableName, newFeature->m_inventory);

	return newFeature;
}

