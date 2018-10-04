#include "Game/ItemBuilder.hpp"

ItemBuilder::ItemBuilder(const ITCXMLNode& xmlNode)
{
	ValidateXmlElement(xmlNode, "EquipSlot,Glyph,GlyphColor,StatBonus,Heal", "name");
	LoadEquipSlot(xmlNode.getChildNode("EquipSlot"));
	StoreXmlChildNodeAttributes(xmlNode, "Glyph", "glyph", m_glyphs);
	StoreXmlChildNodeAttributes(xmlNode, "GlyphColor", "color", m_glyphColors);
	m_stats = Stats(xmlNode.getChildNode("StatBonus"));
	m_healAmount = ParseXmlAttribute(xmlNode.getChildNode("Heal"), "amount", IntRange(0, 0));
}

ItemBuilder::~ItemBuilder()
{

}

void ItemBuilder::LoadItemsFromXML()
{
	IXMLDomParser iDom;
	iDom.setRemoveClears(false);
	ITCXMLNode xMainNode = iDom.openFileHelper("Data/XML/Items.xml", "Items");
	ValidateXmlElement(xMainNode, "Item", "");

	for (int nodeCount = 0; nodeCount < xMainNode.nChildNode(); nodeCount++)
	{
		ITCXMLNode childNode = xMainNode.getChildNode(nodeCount);
		RegisterNewType(childNode);
	}
}

void ItemBuilder::LoadEquipSlot(const ITCXMLNode &equipSlotNode)
{
	std::string equipSlotName = equipSlotNode.getAttribute("slot");
	if (equipSlotName == "head")		m_equipSlot = HEAD;
	if (equipSlotName == "lefthand")	m_equipSlot = LEFTHAND;
	if (equipSlotName == "righthand")	m_equipSlot = RIGHTHAND;
	if (equipSlotName == "feet")		m_equipSlot = FEET;
	if (equipSlotName == "none")		m_equipSlot = NONE;
}

Item* ItemBuilder::BuildItem(std::string name)
{
	Item *newItem = new Item();
	newItem->m_name = name;
	newItem->m_equipSlot = s_registry[name]->m_equipSlot;
	newItem->m_glyph = s_registry[name]->m_glyphs[0];
	newItem->m_glyphColor = s_registry[name]->m_glyphColors[0];
	newItem->m_stats = s_registry[name]->m_stats;
	newItem->healAmount = s_registry[name]->m_healAmount;

	return newItem;
}

void ItemBuilder::LoadStats(const ITCXMLNode& statsNode)
{
	ValidateXmlElement(statsNode, "", "str,dex,con,int,wis,cha");
	m_stats.m_stats[STRENGTH] = ParseXmlAttribute(statsNode, "str", 0);
	m_stats.m_stats[DEXTERITY] = ParseXmlAttribute(statsNode, "dex", 0);
	m_stats.m_stats[CONSTITUTION] = ParseXmlAttribute(statsNode, "con", 0);
	m_stats.m_stats[INTELLIGENCE] = ParseXmlAttribute(statsNode, "int", 0);
	m_stats.m_stats[WISDOM] = ParseXmlAttribute(statsNode, "wis", 0);
	m_stats.m_stats[CHARISMA] = ParseXmlAttribute(statsNode, "cha", 0);
}

