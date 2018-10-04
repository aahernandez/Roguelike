#include "Game/TileDefinition.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DeveloperConsole.hpp"

TileDefinition::TileDefinition(const ITCXMLNode& xmlNode)
{
	ValidateXmlElement(xmlNode, "Glyph,FillColor,GlyphColor", "name,isSolid,isOpaque,isPassable");
	m_name = ParseXmlAttribute(xmlNode, "name", (std::string)"ERROR_NO_NAME_FOUND");
	m_isSolid = ParseXmlAttribute(xmlNode, "isSolid", false);
	m_isOpaque = ParseXmlAttribute(xmlNode, "isOpaque", false);
	m_isPassable = ParseXmlAttribute(xmlNode, "isPassable", false);
	m_gCost = ParseXmlAttribute(xmlNode, "gCost", 1.f);
	StoreXmlChildNodeAttributes(xmlNode, "FillColor", "color", m_fillColors);
	StoreXmlChildNodeAttributes(xmlNode, "GlyphColor", "color", m_glyphColors);
	StoreXmlChildNodeAttributes(xmlNode, "Glyph", "glyph", m_glyphs);
}

TileDefinition::~TileDefinition()
{

}

void TileDefinition::LoadTilesFromXML()
{
	IXMLDomParser iDom;
	iDom.setRemoveClears(false);
	ITCXMLNode xMainNode = iDom.openFileHelper("Data/XML/TileDefinitions.xml", "TileDefinitions");

	for (int nodeCount = 0; nodeCount < xMainNode.nChildNode(); nodeCount++)
	{
		ITCXMLNode childNode = xMainNode.getChildNode(nodeCount);
		RegisterNewType(childNode);
	}
}

float TileDefinition::GetLocalGCost()
{
	return m_gCost;
}
