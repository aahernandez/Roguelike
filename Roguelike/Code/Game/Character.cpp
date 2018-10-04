#include "Game/Character.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Tile.hpp"
#include "Game/Map.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/Behavior.hpp"
#include "Game/PathGenerator.hpp"
#include "Engine/Core/Log.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"

Character::Character(Map *owningMap, std::string name)
	: m_currentBehavior(nullptr)
	, m_currentAimTarget(nullptr)
{
	m_owningTile = nullptr;
	m_name = name;
	m_owningMap = owningMap;
	m_owningTile = owningMap->FindRandomPassableTileOfType();
	Initialize();
	m_owningTile->m_character = this;
}

Character::Character()
	: m_currentBehavior(nullptr)
	, m_currentAimTarget(nullptr)
{
	m_owningTile = nullptr;
	Initialize();
}

Character::~Character()
{
	DropInventoryOnTile();
	m_owningTile->m_character = nullptr;
	m_owningTile = nullptr;
}

void Character::Initialize()
{
	for (int equipmentCount = 0; equipmentCount < NUM_EQUIPMENT_SLOTS; equipmentCount++)
	{
		m_equippedItems[equipmentCount] = nullptr;
	}
}

void Character::Act()
{
	CalcBehaviorUtility();
	m_currentBehavior->Act(this);
	if (DoesCurrentTileHaveItems())
	{
		TakeItemsFromCurrentTile();
	}
}

void Character::PutSelfOnNewTile(Tile *newTile)
{
	m_owningTile = newTile;
	m_owningTile->m_character = this;
	m_owningTile->InteractWithCharacter();
}

void Character::MoveNorth()
{
	Tile *tile = m_owningMap->GetTileToNorth(m_owningTile);
	MoveToNewTile(tile);
}

void Character::MoveSouth()
{
	Tile *tile = m_owningMap->GetTileToSouth(m_owningTile);
	MoveToNewTile(tile);
}

void Character::MoveWest()
{
	Tile *tile = m_owningMap->GetTileToWest(m_owningTile);
	MoveToNewTile(tile);
}

void Character::MoveEast()
{
	Tile *tile = m_owningMap->GetTileToEast(m_owningTile);
	MoveToNewTile(tile);
}

void Character::MoveToNewTile(Tile *newTile)
{
	if (newTile->IsPassable())
	{
		RemoveSelfFromOwningTile();
		PutSelfOnNewTile(newTile);
	}
	else
	{
		AttackTargetTile(newTile);
	}
}

void Character::AttackTargetTile(Tile *newTile)
{
	m_owningMap->ResolveAttack(this, newTile);
}

bool Character::IsNorthTileSolid()
{
	Tile *northTile = m_owningMap->GetTileToNorth(m_owningTile);
	return northTile->IsSolid();
}

bool Character::IsSouthTileSolid()
{
	Tile *southTile = m_owningMap->GetTileToSouth(m_owningTile);
	return southTile->IsSolid();
}

bool Character::IsWestTileSolid()
{
	Tile *westTile = m_owningMap->GetTileToWest(m_owningTile);
	return westTile->IsSolid();
}

bool Character::IsEastTileSolid()
{
	Tile *eastTile = m_owningMap->GetTileToEast(m_owningTile);
	return eastTile->IsSolid();
}

void Character::FillLogWithDetails(Log *log)
{
	log->StoreTextInLog( Stringf("%s (%s%c{-})", m_name.c_str(), m_glyphColor.GetAsString().c_str(), m_glyph), RGBA::WHITE);
	RGBA healthHighlightColor = RGBA::GREEN_LIGHT;
	std::string healthHighlightString = healthHighlightColor.GetAsString();
	log->StoreTextInLog( Stringf("  -Health: %s%i{-}/%s%i", healthHighlightString.c_str(), m_currentHealth, healthHighlightString.c_str(), m_maxHealth), RGBA::WHITE, SUBTITLE);
	log->StoreTextInLog( Stringf("  -Behavior: %s", m_currentBehavior->m_behaviorName.c_str()), RGBA::WHITE, SUBTITLE);
	log->StoreTextInLog( Stringf("  -Faction: %s", m_faction.c_str()), RGBA::WHITE, SUBTITLE);

	log->StoreTextInLog("Equipped Items:", RGBA::WHITE);
	for (int equipmentCount = 0; equipmentCount < NUM_EQUIPMENT_SLOTS; equipmentCount++)
	{
		if (m_equippedItems[equipmentCount] != nullptr)
		{
			Item *currentEquippedItem = m_equippedItems[equipmentCount];
			currentEquippedItem->FillLogWithDetails(log, true);
		}
	}
	m_inventory->FillLogWithDetails(log);
}

void Character::CalcBehaviorUtility()
{
	float currentUtility = FLT_MIN;
	if (m_currentBehavior != nullptr)
		currentUtility = m_currentBehavior->CalcUtility(this);

	bool wasBehaviorChanged = false;
	Behavior *previousBehavior = m_currentBehavior;
	std::vector<Behavior*>::iterator behaviorIter;
	for (behaviorIter = m_behaviors.begin(); behaviorIter != m_behaviors.end(); ++behaviorIter)
	{
		float behaviorUtility = (*behaviorIter)->CalcUtility(this);
		if (behaviorUtility > currentUtility)
		{
			m_currentBehavior = *behaviorIter;
			wasBehaviorChanged = true;
		}
	}

	if (m_currentBehavior != previousBehavior)
		m_currentBehavior->Init(this);
}

bool Character::DoesCurrentTileHaveItems()
{
	if (m_owningTile->m_inventory->m_items.empty())
		return false;
	return true;
}

void Character::TakeItemsFromCurrentTile()
{
	m_inventory->TakeInventory(m_owningTile->m_inventory);
	EquipBestItems();
}

void Character::EquipBestItems()
{
	for (int itemCount = 0; itemCount < (int)m_inventory->m_items.size(); itemCount++)
	{
		Item *inventoryItem = m_inventory->m_items[itemCount];
		EquipSlot itemSlot = inventoryItem->m_equipSlot;
		if (inventoryItem->m_equipSlot != NONE)
		{
			if (m_equippedItems[itemSlot] == nullptr)
			{
				m_equippedItems[itemSlot] = inventoryItem;
			}
			else
			{
				bool isFirstItemBetter = IsFirstItemBetter(inventoryItem, m_equippedItems[itemSlot]);
				if (isFirstItemBetter)
				{
					m_equippedItems[itemSlot] = inventoryItem;
				}
			}
		}
	}
}

EquipSlot Character::GetEquipSlotFromNumOfEquippedItems(int letterCount)
{
	int numEquippedItems = 0;
	for (int equippedItemCount = 0; equippedItemCount < NUM_EQUIPMENT_SLOTS; equippedItemCount++)
	{
		if (IsItemEquippedInSlot((EquipSlot)equippedItemCount))
			numEquippedItems++;
		if (numEquippedItems == letterCount)
			return (EquipSlot)equippedItemCount;
	}
	return NONE;
}

Item* Character::GetUnequippedItemFromIndex(int index)
{
	int numUnequippedItems = 0;
	for (int itemCount = 0; itemCount < (int)m_inventory->m_items.size(); itemCount++)
	{
		Item *inventoryItem = m_inventory->m_items[itemCount];
		if (!IsItemEquipped(inventoryItem))
		{
			if (numUnequippedItems == index)
				return inventoryItem;
			else
			{
				numUnequippedItems++;
			}
		}
	}
	return nullptr;
}

bool Character::IsItemInInventory(std::string itemName)
{
	for (int itemCount = 0; itemCount < (int)m_inventory->m_items.size(); itemCount++)
	{
		std::string invItemName = m_inventory->m_items[itemCount]->m_name;
		if (itemName == invItemName)
		{
			return true;
		}
	}
	return false;
}

bool Character::IsItemEquipped(Item *item)
{
	for (int equippedItemCount = 0; equippedItemCount < NUM_EQUIPMENT_SLOTS; equippedItemCount++)
	{
		if (m_equippedItems[equippedItemCount] == item)
			return true;
	}
	return false;
}

bool Character::IsItemEquippedInSlot(EquipSlot equipSlot)
{
	if (m_equippedItems[equipSlot] != nullptr)
		return true;
	return false;
}

void Character::TakeDirectDamage(int damage)
{
	if (damage > 0)
		m_currentHealth -= damage;

	m_damageLog->EraseLogAtIndex(0);
	m_damageLog->StoreTextInLog(Stringf("%i", damage), RGBA::RED, PARAGRAPH);
	m_logPosition = GetMapCoords();
}

void Character::TakeDamage(int damage)
{
	Stats bonusEquipmentStats;
	CalcEquipmentBonusStats(bonusEquipmentStats);
	int defense = m_stats.CalcDefense(bonusEquipmentStats);
	int newDamage = damage - defense;
	if (newDamage > 0)
		m_currentHealth -= newDamage;

 	m_damageLog->EraseLogAtIndex(0);
	m_damageLog->StoreTextInLog(Stringf("%i", newDamage), RGBA::RED, PARAGRAPH);
	m_logPosition = GetMapCoords();
}

void Character::CalcEquipmentBonusStats(Stats& bonusEquipmentStats)
{
	for (int equipmentCount = 0; equipmentCount < NUM_EQUIPMENT_SLOTS; equipmentCount++)
	{
		if (m_equippedItems[equipmentCount] != nullptr)
		{
			bonusEquipmentStats.AddStats(m_equippedItems[equipmentCount]->m_stats);
		}
	}
}

bool Character::IsFirstItemBetter(Item *firstItem, Item *secondItem)
{
	int firstItemCompositStats = firstItem->m_stats.CalcCombinedStats();
	int secondItemCompositStats = secondItem->m_stats.CalcCombinedStats();
	if (firstItemCompositStats > secondItemCompositStats)
		return true;
	return false;
}

bool Character::AreFactionsSame(std::string factionName)
{
	return m_faction.compare(factionName) == 0;
}

float Character::GetTileGCost(std::string tileName)
{
	return 0.f;
}

void Character::ShootRangedWeapon()
{
	m_owningMap->ResolveAttack(this, m_currentAimTarget->m_owningTile);
}

void Character::LoopToNextAimTarget()
{
	int numLosTargets = (int)m_charactersInSight.size();
	if (numLosTargets == 0)
		return;

	bool hasAimTargetChanged = false;
	bool isChangingTarget = false;
	int losTargetCount = 0;
	while (!hasAimTargetChanged)
	{
		if (losTargetCount == numLosTargets)
		{
			losTargetCount = 0;
		}

		if (isChangingTarget)
		{
			m_currentAimTarget = m_charactersInSight[losTargetCount];
			hasAimTargetChanged = true;
		}

		if (m_currentAimTarget == m_charactersInSight[losTargetCount])
		{
			isChangingTarget = true;
		}

		losTargetCount++;
	}
}

void Character::LoopToPrevAimTarget()
{
	int numLosTargets = (int)m_charactersInSight.size();
	if (numLosTargets == 0)
		return;

	bool hasAimTargetChanged = false;
	bool isChangingTarget = false;
	int losTargetCount = numLosTargets - 1;
	while (!hasAimTargetChanged)
	{
		if (losTargetCount == -1)
		{
			losTargetCount = numLosTargets - 1;
		}

		if (isChangingTarget)
		{
			m_currentAimTarget = m_charactersInSight[losTargetCount];
			hasAimTargetChanged = true;
		}

		if (m_currentAimTarget == m_charactersInSight[losTargetCount])
		{
			isChangingTarget = true;
		}

		losTargetCount--;
	}
}

bool Character::IsWeaponMelee()
{
	if (m_equippedItems[LEFTHAND] == nullptr)
		return true;
	 
	if (m_equippedItems[LEFTHAND]->m_name == "Bow")
	{
		return false;
	}
	return true;
}

std::string Character::GetEquipmentName(EquipSlot equipSlot)
{
	if (m_equippedItems[equipSlot] != nullptr)
		return m_equippedItems[equipSlot]->m_name;
	else
		return Stringf("(no %s)", Item::GetEquipSlotAsString(equipSlot).c_str());
}

void Character::DebugRender() const
{
	if (m_currentBehavior != nullptr)
		PathGenerator::RenderPath(m_path);

	for (int characterCount = 0; characterCount < (int)m_charactersInSight.size(); characterCount++)
	{
		IntVector2 position = m_charactersInSight[characterCount]->m_owningTile->m_mapPosition;
		g_theSimpleRenderer->DrawDebugQuad2D(AABB2D(Vector2(position.x + 0.2f, position.y + 0.2f), Vector2(position.x + 0.8f, position.y + 0.8f)), 0.1f, RGBA::RED, RGBA::RED);
	}
}

void Character::RenderAimTarget(float aimTargetBorderSize) const
{
	IntVector2 position = m_currentAimTarget->m_owningTile->m_mapPosition;
	float inverseBorderSize = 1.f - aimTargetBorderSize;
	g_theSimpleRenderer->DrawDebugQuad2D(AABB2D(Vector2(position.x + aimTargetBorderSize, position.y + aimTargetBorderSize), Vector2(position.x + inverseBorderSize, position.y + inverseBorderSize)), 0.1f, RGBA::RED, RGBA::RED);
}

