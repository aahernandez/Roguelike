#include "Game/Entity.hpp"
#include "Game/Map.hpp"
#include "Game/Tile.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Log.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"

const float ENTITY_SCALE = 0.8f;

Entity::Entity()
	: m_owningTile(nullptr)
	, m_isDead(false)
{
	m_damageLog = new Log(60.f / 1600.f, false);
	m_inventory = new Inventory();
}

Entity::~Entity()
{
	delete m_inventory;
}

void Entity::RemoveSelfFromOwningTile()
{
	m_owningTile->m_character = nullptr;
}

void Entity::Update(float deltaSeconds)
{
	deltaSeconds;
	m_logPosition.y += (0.5f * deltaSeconds);
	RGBA color = m_damageLog->GetLogColorAtIndex(0);
	if (color.a <= 10)
	{
		m_damageLog->EraseLogAtIndex(0);
	}
	else
	{
		color.SubtractAlpha(3);
		m_damageLog->SetColorAtIndex(0, color);
	}
}

void Entity::Render() const
{
	if (m_owningTile->m_currentSeenState == CURRENTLY_SEEN)
	{
		std::string glyphString = "";
		glyphString += m_glyph;
		AABB2D characterBounds = m_owningTile->GetTileBounds();
		characterBounds.Scale(ENTITY_SCALE);
		g_theSimpleRenderer->DrawText2DCenteredAndScaledToFitWithinBounds(glyphString.c_str(), characterBounds, m_glyphColor);
// 		g_theSimpleRenderer->DrawText2DCenteredOnPosition(glyphString.c_str(), m_owningTile->GetCenter(), m_glyphColor);
		m_damageLog->RenderLog( Vector2(m_logPosition.x + 0.5f, m_logPosition.y + 0.8f) , false, true);
	}
}

void Entity::TakeDamage(int damage)
{
	m_currentHealth -= damage;
	m_damageLog->EmptyLog();
	m_damageLog->StoreTextInLog(Stringf("%i", damage), RGBA::RED, PARAGRAPH);
	m_logPosition = GetMapCoords();
}

int Entity::CalcDamage(StatName statName)
{
	if (statName == DEXTERITY)
		return m_stats.CalcDexAttack();
	else
		return m_stats.CalcStrengthAttack();
}

IntVector2 Entity::GetMapCoords()
{
	if (m_owningTile != nullptr)
		return m_owningTile->m_mapPosition;
	return IntVector2::ZERO;
}

void Entity::DropInventoryOnTile()
{
	m_owningTile->m_inventory->TakeInventory(m_inventory);
	m_inventory->EmptyInventoryWithoutDestroyingItems();
}

bool Entity::HasLineOfSight(Character *character)
{
	Vector2 entityMapCoords = GetMapCoords();
	Vector2 characterMapCoords = character->GetMapCoords();
	
	for (float changeOfEntityX = 0.f; changeOfEntityX <= 1.f; changeOfEntityX += 0.5f)
	{
		for (float changeOfEntityY = 0.f; changeOfEntityY <= 1.f; changeOfEntityY += 0.5f)
		{
			for (float changeOfCharX = 0.f; changeOfCharX <= 1.f; changeOfCharX += 0.5f)
			{
				for (float changeOfCharY = 0.f; changeOfCharY <= 1.f; changeOfCharY += 0.5f)
				{
					Vector2 newEntityPos = Vector2( entityMapCoords.x + changeOfEntityX, entityMapCoords.y + changeOfEntityY );
					Vector2 newCharacterPos = Vector2( characterMapCoords.x + changeOfCharX, characterMapCoords.y + changeOfCharY );
					RaycastResults2D raycastResults = m_owningMap->RaycastAW(newEntityPos, newCharacterPos, false, true);

					if (!raycastResults.didImpact)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}
