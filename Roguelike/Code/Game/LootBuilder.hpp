#pragma once
#include "Game/DataDrivenBuilder.hpp"
#include "Game/Inventory.hpp"

class Item;

struct ItemWeights
{
	std::string m_name;
	int weight;
};

class LootBuilder : DataDrivenBuilder<LootBuilder>
{
public:
	std::vector<ItemWeights> m_itemWeights;

	LootBuilder(const ITCXMLNode& xmlNode);
	~LootBuilder();

	static void LoadLootTablesFromXML();
	static void BuildLoot(std::string name, Inventory *inventory);
};