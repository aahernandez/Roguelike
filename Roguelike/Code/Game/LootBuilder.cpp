#include "Game/LootBuilder.hpp"
#include "Game/Item.hpp"
#include "Game/ItemBuilder.hpp"

LootBuilder::LootBuilder(const ITCXMLNode& xmlNode)
{
	ValidateXmlElement(xmlNode, "Item", "name");

	unsigned int numChildNodes = xmlNode.nChildNode("Item");
	for (unsigned int childNodeCount = 0; childNodeCount < numChildNodes; childNodeCount++)
	{
		ITCXMLNode childNode = xmlNode.getChildNode("Item", childNodeCount);
		ItemWeights newItemWeight{ParseXmlAttribute(childNode, "name", (std::string)"ERROR_NO_ITEM_NAME_FOUND"), ParseXmlAttribute(childNode, "weight", 999)};
		m_itemWeights.push_back(newItemWeight);
	}
}

LootBuilder::~LootBuilder()
{

}

void LootBuilder::LoadLootTablesFromXML()
{
	IXMLDomParser iDom;
	iDom.setRemoveClears(false);
	ITCXMLNode xMainNode = iDom.openFileHelper("Data/XML/LootTables.xml", "LootTables");
	ValidateXmlElement(xMainNode, "LootTable", "");

	for (int nodeCount = 0; nodeCount < xMainNode.nChildNode(); nodeCount++)
	{
		ITCXMLNode childNode = xMainNode.getChildNode(nodeCount);
		RegisterNewType(childNode);
	}
}

void LootBuilder::BuildLoot(std::string name, Inventory *inventory)
{
	int numItemWeights = (int)s_registry[name]->m_itemWeights.size();
	for (int itemWeightCount = 0; itemWeightCount < numItemWeights; itemWeightCount++)
	{
		ItemWeights& currentItemWeight = s_registry[name]->m_itemWeights[itemWeightCount];
		int itemChance = GetRandomIntInRange(1, currentItemWeight.weight);
		if (itemChance == 1)
		{
			inventory->AddItem(ItemBuilder::BuildItem(currentItemWeight.m_name));
		}
	}
}

