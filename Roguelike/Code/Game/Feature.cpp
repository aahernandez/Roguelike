#include "Game/Feature.hpp"
#include "Game/Map.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/Log.hpp"
#include "Engine/Core/StringUtils.hpp"

Feature::Feature(Map *owningMap, std::string tileType /*= ""*/)
{
	m_owningMap = owningMap;
	m_owningTile = owningMap->FindRandomPassableTileOfType(tileType);
	Initialize();
	m_owningTile->m_feature = this;
	m_destinationTilePos = m_owningTile->m_mapPosition;
	m_destinationLevel = owningMap;
}

Feature::~Feature()
{
	m_owningTile->m_feature = nullptr;
	m_owningTile = nullptr;
}

void Feature::Initialize()
{
	m_glyph = 'f';
	m_glyphColor = RGBA::PURPLE;
	m_isSolid = true;
	m_destinationTilePos = IntVector2(-1,-1);
	m_destinationLevel = nullptr;
}

void Feature::FillLogWithDetails(Log *log)
{
	log->StoreTextInLog( Stringf("%s (%s%c{-})", m_name.c_str(), m_glyphColor.GetAsString().c_str(), m_glyph), RGBA::WHITE);
	if (m_isSolid)
		log->StoreTextInLog(" -Is Solid", RGBA::WHITE, SUBTITLE);
	if (m_isDestoyable)
		log->StoreTextInLog(" -Is Destroyable", RGBA::WHITE, SUBTITLE);
	if (m_isOpenable)
		log->StoreTextInLog(" -Is Openable", RGBA::WHITE, SUBTITLE);
	if (m_damageDealt > 0)
		log->StoreTextInLog( Stringf(" -Deals {1.0,0.0,0.0}%i{-} damage", m_damageDealt), RGBA::WHITE, SUBTITLE);
	if (!m_inventory->IsEmpty())
		m_inventory->FillLogWithDetails(log);
}

void Feature::InteractWithCharacter(Character *character)
{
	if (m_damageDealt > 0)
		character->TakeDirectDamage(m_damageDealt);

	if (m_destinationTilePos != m_owningTile->m_mapPosition && character->m_name == "Player")
	{
		Tile *newTileToMoveTo = m_destinationLevel->GetTileAtPosition(m_destinationTilePos);
		character->RemoveSelfFromOwningTile();
		character->m_owningTile = newTileToMoveTo;
		character->m_owningTile->m_character = character;
		character->m_owningMap = m_destinationLevel;
		m_owningMap->m_characters.erase(m_owningMap->m_characters.begin());
		m_destinationLevel->m_characters.insert(m_destinationLevel->m_characters.begin(), character);
	}
}
