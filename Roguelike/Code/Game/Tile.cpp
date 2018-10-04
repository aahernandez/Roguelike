#include "Game/Tile.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/Item.hpp"
#include "Engine/Core/Log.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"

Tile::Tile()
	: m_mapPosition(IntVector2())
{
	Initialize();
}

Tile::Tile(const IntVector2 &mapPosition)
{
	m_mapPosition = mapPosition;
	Initialize();
}

Tile::~Tile()
{
	delete m_inventory;
}

void Tile::Initialize()
{
	m_currentSeenState = NEVER_SEEN;
	m_fillColor = RGBA::WHITE;
	m_glyphColor = RGBA::WHITE;
	m_feature = nullptr;
	m_character = nullptr;
	m_tileDefition = nullptr;
	m_inventory = new Inventory();
}

void Tile::FillLogWithDetails(Log *log)
{
	log->StoreTextInLog(m_tileDefition->m_name, RGBA::WHITE);
	if (m_tileDefition->m_isSolid)
		log->StoreTextInLog(" -Is Solid", RGBA::WHITE, SUBTITLE);
	if (m_tileDefition->m_isOpaque)
		log->StoreTextInLog(" -Is Opaque", RGBA::WHITE, SUBTITLE);

	if (m_character != nullptr)
	{
		m_character->FillLogWithDetails(log);
	}
	if (m_feature != nullptr)
	{
		m_feature->FillLogWithDetails(log);
	}
	if (!m_inventory->IsEmpty())
	{
		m_inventory->FillLogWithDetails(log);
	}
}

void Tile::SetTileDefinition(TileDefinition *tileDefinition)
{
	m_tileDefition = tileDefinition;
	
	if (m_tileDefition->m_glyphs.size() > 0)
	{
		int glyphIndex = GetRandomIntInRange(0, (int)m_tileDefition->m_glyphs.size() - 1);
		m_glyph = m_tileDefition->m_glyphs[glyphIndex];
	}

	int fillColorIndex = GetRandomIntInRange(0, (int)m_tileDefition->m_fillColors.size() - 1);
	m_fillColor = m_tileDefition->m_fillColors[fillColorIndex];

	int glyphColorIndex = GetRandomIntInRange(0, (int)m_tileDefition->m_glyphColors.size() - 1);
	m_glyphColor = m_tileDefition->m_glyphColors[glyphColorIndex];
}

void Tile::InteractWithCharacter()
{
	if (m_feature != nullptr)
		m_feature->InteractWithCharacter(m_character);
}

void Tile::Render() const
{
	AABB2D tilePos;
	tilePos.mins = Vector2((float)m_mapPosition.x, (float)m_mapPosition.y);
	tilePos.maxs = Vector2((float)m_mapPosition.x + 1.f, (float)m_mapPosition.y + 1.f);
	
	if (m_currentSeenState == PREVIOUSLY_SEEN)
	{
		g_theSimpleRenderer->DrawQuad2D(tilePos.mins, tilePos.maxs, Vector2::ZERO, Vector2::ONE, m_fillColor);
		g_theSimpleRenderer->DrawQuad2D(tilePos.mins, tilePos.maxs, Vector2::ZERO, Vector2::ONE, RGBA::BLACK_OVERLAY_DARK);
	}
	else if (m_currentSeenState == CURRENTLY_SEEN)
	{
		g_theSimpleRenderer->DrawQuad2D(tilePos.mins, tilePos.maxs, Vector2::ZERO, Vector2::ONE, m_fillColor);
	}

	if (m_character != nullptr)
	{
		m_character->Render();
	}
	else if (m_feature != nullptr)
	{
		m_feature->Render();
	}
	else if (!m_inventory->IsEmpty())
	{
		m_inventory->GetFirstItem()->Render(Vector2(m_mapPosition.x + 0.5f, m_mapPosition.y + 0.5f));
	}
}

bool Tile::IsSolid()
{
	if (m_tileDefition->m_isSolid)
	{
		return true;
	}
	if (m_character != nullptr)
	{
		return true;
	}
	if (m_feature != nullptr)
	{
		if (m_feature->m_isSolid)
		{
			return true;
		}
	}
	return false;
}

bool Tile::IsPassable()
{
	if (!m_tileDefition->m_isPassable)
	{
		return false;
	}
	if (m_character != nullptr)
	{
		return false;
	}
	if (m_feature != nullptr)
	{
		if (m_feature->m_isSolid)
		{
			return false;
		}
	}
	return true;
}

bool Tile::HasCharacter()
{
	return m_character != nullptr;
}

bool Tile::HasFeature()
{
	return m_feature != nullptr;
}

void Tile::DestroyFeature()
{
	delete m_feature;
	m_feature = nullptr;
}

float Tile::GetLocalGCost()
{
	return m_tileDefition->GetLocalGCost();
}

std::string Tile::GetName()
{
	return m_tileDefition->m_name;
}

Vector2 Tile::GetCenter() const
{
	return Vector2(m_mapPosition.x + 0.5f, m_mapPosition.y + 0.5f);
}

AABB2D Tile::GetTileBounds() const
{
	Vector2 bottomLeft = Vector2(m_mapPosition.x, m_mapPosition.y);
	Vector2 topRight = Vector2(m_mapPosition.x + 1.f, m_mapPosition.y + 1.f);
	return AABB2D(bottomLeft, topRight);
}
