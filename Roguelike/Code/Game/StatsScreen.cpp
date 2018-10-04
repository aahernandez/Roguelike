#include "Game/Tile.hpp"
#include "Game/Player.hpp"
#include "Game/StatsScreen.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"

StatsScreen::StatsScreen()
{
	Initialize();
}

StatsScreen::~StatsScreen()
{
	Destroy();
}

void StatsScreen::Initialize()
{
	m_isDisplayingInentory = false;
	m_isDroppingItem = false;
	m_isEquippingItem = false;
	m_isUnequippingItem = false;
}

void StatsScreen::Update(float deltaSeconds)
{
	deltaSeconds;
	Player *player = Player::s_player;
	m_bonusEquipmentStats.ClearStats();
	player->CalcEquipmentBonusStats(m_bonusEquipmentStats);
	m_bonusEquipmentStats.AddStats(player->m_stats);
}

void StatsScreen::Render() const
{
	RenderBaseStats();
	RenderModifiedStats();
	RenderHealth();
	RenderEquipment();
	RenderInventory();
}

void StatsScreen::RenderBaseStats() const
{
	Player *player = Player::s_player;
	Vector2 textPos = Vector2(1200.f, 667.f);
	Vector2 textPosDelta = Vector2(0.f, -27.f);

	std::vector<std::string> statsStringList;
	statsStringList.push_back("BASE STATS"); 
	statsStringList.push_back(Stringf("STR:%i", player->m_stats.m_stats[STRENGTH]));
	statsStringList.push_back(Stringf("DEX:%i", player->m_stats.m_stats[DEXTERITY]));
	statsStringList.push_back(Stringf("CON:%i", player->m_stats.m_stats[CONSTITUTION]));
	statsStringList.push_back(Stringf("INT:%i", player->m_stats.m_stats[INTELLIGENCE]));
	statsStringList.push_back(Stringf("WIS:%i", player->m_stats.m_stats[WISDOM]));
	statsStringList.push_back(Stringf("CHA:%i", player->m_stats.m_stats[CHARISMA]));

	g_theSimpleRenderer->DrawTextList2DCenteredOnPosition(statsStringList, textPos, textPosDelta);
}

void StatsScreen::RenderModifiedStats() const
{
	Vector2 textPos = Vector2(1467.f, 667.f);
	Vector2 textPosDelta = Vector2(0.f, -27.f);

	std::vector<std::string> statsStringList;
	statsStringList.push_back("MODIFIED STATS");
	statsStringList.push_back(Stringf("STR:%i", m_bonusEquipmentStats.m_stats[STRENGTH]));
	statsStringList.push_back(Stringf("DEX:%i", m_bonusEquipmentStats.m_stats[DEXTERITY]));
	statsStringList.push_back(Stringf("CON:%i", m_bonusEquipmentStats.m_stats[CONSTITUTION]));
	statsStringList.push_back(Stringf("INT:%i", m_bonusEquipmentStats.m_stats[INTELLIGENCE]));
	statsStringList.push_back(Stringf("WIS:%i", m_bonusEquipmentStats.m_stats[WISDOM]));
	statsStringList.push_back(Stringf("CHA:%i", m_bonusEquipmentStats.m_stats[CHARISMA]));

	g_theSimpleRenderer->DrawTextList2DCenteredOnPosition(statsStringList, textPos, textPosDelta);
}

void StatsScreen::RenderEquipment() const
{
	Player *player = Player::s_player;

	float textHeight = 15.f;
	float scale = (1600.f / 60.f);
	char letter = 'a';
	g_theSimpleRenderer->DrawText2D("EQUIPMENT", Vector2(41.f, textHeight + 1.f) * scale);

	for (int equipmentCount = 0; equipmentCount < NUM_EQUIPMENT_SLOTS; equipmentCount++)
	{
		std::string equipmentString = "";
		if (m_isUnequippingItem && player->IsItemEquippedInSlot((EquipSlot)equipmentCount))
		{
			equipmentString += letter;
			equipmentString += ". ";
			letter += 1;
		}
		equipmentString += player->GetEquipmentName((EquipSlot)equipmentCount);
		RGBA equipmentColor = player->IsItemEquippedInSlot((EquipSlot)equipmentCount) ? RGBA::YELLOW : RGBA::GREY_LIGHT;
		g_theSimpleRenderer->DrawText2D(equipmentString.c_str(), Vector2(41.f, textHeight - equipmentCount) * scale, equipmentColor);
	}
}

void StatsScreen::RenderHealth() const
{
	Player *player = Player::s_player;
	RGBA maxHealthColor = RGBA::GREEN_LIGHT;
	RGBA lowHealthColor = RGBA::RED;
	float healthPercentage = (float)player->m_currentHealth / (float)player->m_maxHealth;
	RGBA currentHealthColor = RGBA::RangeMapBetweenColors(lowHealthColor, maxHealthColor, healthPercentage);
	std::string maxHealthHighlightString = maxHealthColor.GetAsString();
	std::string currentHealthHighlightString = currentHealthColor.GetAsString();
	std::string healthString = Stringf("Health: %s%i{-}/%s%i", currentHealthHighlightString.c_str(), player->m_currentHealth, maxHealthHighlightString.c_str(), player->m_maxHealth);
	g_theSimpleRenderer->DrawText2DCenteredOnPosition(healthString.c_str(), Vector2(1467.f, 773.f));
}

void StatsScreen::RenderInventory() const
{
	if (!m_isDisplayingInentory && !m_isEquippingItem && !m_isDroppingItem)
		return;

	g_theSimpleRenderer->SetTexture(g_theSimpleRenderer->m_whiteTexture);
	g_theSimpleRenderer->DrawQuad2D( Vector2(0.f, 0.f), Vector2(1600.f, 900.f), Vector2(0.f, 0.f), Vector2(1.f, 1.f), RGBA::BLACK_OVERLAY_DARK);

	float textHeight = 30.f;
	float scale = (1600.f / 60.f);
	Player *player = Player::s_player;
	float heightSub = 0.f;
	char letter = 'a';

	for (int itemCount = 0; itemCount < player->m_inventory->GetSize(); itemCount++)
	{
		Item *item = player->m_inventory->GetItem(itemCount);
		if (!player->IsItemEquipped(item) || m_isDroppingItem)
		{
			std::string itemString = "";
			itemString += letter;
			itemString += ". ";
			itemString += item->m_name;
			g_theSimpleRenderer->DrawText2D(itemString.c_str(), Vector2(1.f, textHeight - heightSub) * scale);
			heightSub += 1.f;
			letter += 1;
		}
	}
}

void StatsScreen::Destroy()
{

}

void StatsScreen::KeyDown()
{
	Player *player = Player::s_player;

	if (m_isEquippingItem)
	{
		if (player->m_inventory->GetSize() == 0)
		{
			m_isEquippingItem = false;
		}
		else
		{
			char letter = 'A';
			for (int letterCount = 0; letterCount < 24 && letterCount < player->m_inventory->GetSize(); letterCount++)
			{
				if (g_theInput->WasKeyJustPressed(letter))
				{
					Item *itemToEquip = player->GetUnequippedItemFromIndex(letterCount);
					EquipSlot itemEquipSlot = itemToEquip->m_equipSlot;
					if (itemEquipSlot != NONE)
					{
						player->m_equippedItems[itemEquipSlot] = itemToEquip;
					}
					else if (itemEquipSlot == NONE)
					{
						player->m_currentHealth += itemToEquip->healAmount.GetIntInRange();
						if (player->m_currentHealth > player->m_maxHealth)
							player->m_currentHealth = player->m_maxHealth;
						player->m_inventory->RemoveItem(itemToEquip);
					}
					m_isEquippingItem = false;
					break;
				}
				letter++;
			}
		}
	}
	else if (m_isDroppingItem)
	{
		if (player->m_inventory->GetSize() == 0)
		{
			m_isDroppingItem = false;
		}
		else
		{
			char letter = 'A';
			for (int letterCount = 0; letterCount < 24 && letterCount < player->m_inventory->GetSize(); letterCount++)
			{
				if (g_theInput->WasKeyJustPressed(letter))
				{
					Item *itemToDrop = player->m_inventory->GetItem(letterCount);
					if (player->IsItemEquipped(itemToDrop))
					{
						player->m_equippedItems[itemToDrop->m_equipSlot] = nullptr;
					}
					player->m_owningTile->m_inventory->AddItem(itemToDrop);
					player->m_inventory->m_items.erase(player->m_inventory->m_items.begin() + letterCount);
					m_isDroppingItem = false;
					break;
				}
				letter++;
			}
		}
	}
	else if (m_isUnequippingItem)
	{
		char letter = 'A';
		for (int letterCount = 0; letterCount < NUM_EQUIPMENT_SLOTS; letterCount++)
		{
			if (g_theInput->WasKeyJustPressed(letter))
			{
				EquipSlot itemEquipSlot = player->GetEquipSlotFromNumOfEquippedItems(letterCount + 1);
				if (player->IsItemEquippedInSlot(itemEquipSlot))
				{
					player->m_equippedItems[itemEquipSlot] = nullptr;
				}
				m_isUnequippingItem = false;
				break;
			}
			letter++;
		}
	}
	else
	{
		if (g_theInput->IsKeyDown(KEY_TAB))
			m_isDisplayingInentory = true;
		else
			m_isDisplayingInentory = false;

		if (g_theInput->WasKeyJustPressed('U') && !m_isUnequippingItem)
			m_isUnequippingItem = true;

		if (g_theInput->WasKeyJustPressed('E') && !m_isEquippingItem)
			m_isEquippingItem = true;

		if (g_theInput->WasKeyJustPressed('D') && !m_isDroppingItem)
			m_isDroppingItem = true;
	}
}
