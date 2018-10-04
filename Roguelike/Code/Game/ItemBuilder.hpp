#pragma once
#include "Game/DataDrivenBuilder.hpp"
#include "Game/Item.hpp"

#include "Engine/Math/IntRange.hpp"

class Map;

class ItemBuilder : DataDrivenBuilder<ItemBuilder>
{
public:
	EquipSlot m_equipSlot;
	std::vector<RGBA> m_glyphColors;
	std::vector<char> m_glyphs;
	IntRange m_healAmount;

	ItemBuilder(const ITCXMLNode& xmlNode);
	~ItemBuilder();

	static void LoadItemsFromXML();
	void LoadEquipSlot(const ITCXMLNode &equipSlotNode);
	static Item* BuildItem(std::string name);
	void LoadStats(const ITCXMLNode& statsNode);
};