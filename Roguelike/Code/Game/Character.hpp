#pragma once
#include "Game/Entity.hpp"

class Behavior;
class Log;

typedef std::vector<Tile*> Path;

class Character : public Entity
{
public:
	Item* m_equippedItems[NUM_EQUIPMENT_SLOTS];
	std::vector<Behavior*> m_behaviors;
	Behavior *m_currentBehavior;
	Path m_path;
	std::string m_faction;
	Character *m_currentAimTarget;

	Character();
	Character(Map *owningMap, std::string name);
	~Character();

	void Initialize();

	virtual void Act();
	virtual void TakeItemsFromCurrentTile();

	void PutSelfOnNewTile(Tile *newTile);

	void MoveNorth();
	void MoveSouth();
	void MoveWest();
	void MoveEast();
	void MoveToNewTile(Tile *newTile);
	void AttackTargetTile(Tile *newTile);

	bool IsNorthTileSolid();
	bool IsSouthTileSolid();
	bool IsWestTileSolid();
	bool IsEastTileSolid();

	virtual void FillLogWithDetails(Log *log);
	void CalcBehaviorUtility();
	bool DoesCurrentTileHaveItems();
	void EquipBestItems();
	EquipSlot GetEquipSlotFromNumOfEquippedItems(int letterCount);
	Item* GetUnequippedItemFromIndex(int index);
	bool IsItemInInventory(std::string itemName);
	bool IsItemEquipped(Item *item);
	bool IsItemEquippedInSlot(EquipSlot equipSlot);
	void TakeDirectDamage(int damage);
	virtual void TakeDamage(int damage) override;
	void CalcEquipmentBonusStats(Stats& bonusEquipmentStats);
	bool IsFirstItemBetter(Item *firstItem, Item *secondItem);
	bool AreFactionsSame(std::string factionName);
	float GetTileGCost(std::string tileName);
	void ShootRangedWeapon();
	void LoopToNextAimTarget();
	void LoopToPrevAimTarget();
	bool IsWeaponMelee();

	std::string GetEquipmentName(EquipSlot equipSlot);

	void DebugRender() const;
	void RenderAimTarget(float aimTargetBorderSize) const;
};