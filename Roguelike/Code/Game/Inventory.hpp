#pragma once
#include <vector>

class Item;
class Log;

class Inventory
{
public:
	std::vector<Item*> m_items;

	Inventory();
	~Inventory();

	bool IsEmpty();
	Item* GetFirstItem();
	void TakeInventory(Inventory *inventory);
	void EmptyInventoryWithoutDestroyingItems();
	void FillLogWithDetails(Log *log);
	void AddItem(Item *newItem);
	int GetSize();
	void RemoveItem(Item *itemToRemove);
	Item* GetItem(int index);
};