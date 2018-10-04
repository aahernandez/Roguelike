#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include "Game/ItemBuilder.hpp"
#include "Engine/Core/Log.hpp"
#include "Engine/Core/StringUtils.hpp"

Player* Player::s_player = nullptr;

Player::Player()
{
	Initialize();
}

Player::Player(Map *owningMap)
{
	Initialize();
	SetOwningMap(owningMap);
}

Player::~Player()
{

}

void Player::Initialize()
{
	m_name = "Player";
	m_glyph = '@';
	m_glyphColor = RGBA::WHITE;
	m_faction = "consortium";
	m_maxHealth = 25;
	m_currentHealth = m_maxHealth;
	m_stats.SetStats(15, 15, 15, 15, 15, 15);
	m_inventory->AddItem(ItemBuilder::BuildItem("Bow"));
	
	for (int equipmentCount = 0; equipmentCount < NUM_EQUIPMENT_SLOTS; equipmentCount++)
	{
		m_equippedItems[equipmentCount] = nullptr;
	}
}

void Player::SetOwningMap(Map *owningMap)
{
	m_owningMap = owningMap;
	m_owningTile = m_owningMap->FindFirstNonSolidTile();
	m_owningTile->m_character = this;
	m_owningMap->m_characters.insert(m_owningMap->m_characters.begin(), this);
}

void Player::Act()
{
	if (DoesCurrentTileHaveItems())
	{
		TakeItemsFromCurrentTile();
	}
}

void Player::FillLogWithDetails(Log *log)
{
	log->StoreTextInLog(Stringf("%s (%s%c{-})", m_name.c_str(), m_glyphColor.GetAsString().c_str(), m_glyph), RGBA::WHITE);
	log->StoreTextInLog(Stringf("  -Faction: %s", m_faction.c_str()), RGBA::WHITE);
}

void Player::TakeItemsFromCurrentTile()
{
	m_inventory->TakeInventory(m_owningTile->m_inventory);
}
