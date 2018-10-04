#include "Game/Inventory.hpp"
#include "Game/Item.hpp"
#include "Engine/Core/Log.hpp"

Inventory::Inventory()
{

}

Inventory::~Inventory()
{

}

bool Inventory::IsEmpty()
{
	return m_items.empty();
}

Item* Inventory::GetFirstItem()
{
	if (!IsEmpty())
		return m_items[0];
	else
		return nullptr;
}

void Inventory::TakeInventory(Inventory *inventory)
{
	m_items.insert(m_items.end(), inventory->m_items.begin(), inventory->m_items.end());
	inventory->EmptyInventoryWithoutDestroyingItems();
}

void Inventory::EmptyInventoryWithoutDestroyingItems()
{
	for (int itemCount = 0; itemCount < (int)m_items.size(); itemCount++)
	{
		m_items[itemCount] = nullptr;
	}
	m_items.clear();
}

void Inventory::FillLogWithDetails(Log *log)
{
	if (!m_items.empty())
	{
		log->StoreTextInLog("Inventory", RGBA::WHITE);
		for (int itemCount = 0; itemCount < (int)m_items.size(); itemCount++)
		{
			m_items[itemCount]->FillLogWithDetails(log);
		}
	}
}

void Inventory::AddItem(Item *newItem)
{
	m_items.push_back(newItem);
}

int Inventory::GetSize()
{
	return (int)m_items.size();
}

void Inventory::RemoveItem(Item *itemToRemove)
{
	for (int itemCount = 0; itemCount < (int)m_items.size(); itemCount++)
	{
		if (m_items[itemCount] == itemToRemove)
		{
			delete itemToRemove;
			m_items[itemCount] = nullptr;
			itemToRemove = nullptr;
			m_items.erase(m_items.begin() + itemCount);
		}
	}
}

Item* Inventory::GetItem(int index)
{
	return m_items[index];
}
