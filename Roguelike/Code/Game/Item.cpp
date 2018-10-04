#include "Game/Item.hpp"
#include "Game/Tile.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/Log.hpp"
#include "Engine/Core/StringUtils.hpp"

Item::Item()
{
}

Item::~Item()
{

}

void Item::Update(float deltaSeconds)
{
	deltaSeconds;
}

void Item::Render(const Vector2 &position) const
{
	std::string glyphString = "";
	glyphString += m_glyph;
	g_theSimpleRenderer->DrawText2DCenteredOnPosition(glyphString.c_str(), position, m_glyphColor);
}

void Item::FillLogWithDetails(Log *log, bool isLoggingEquipData /*= false*/)
{
	if (isLoggingEquipData)
	{
		log->StoreTextInLog( Stringf("  -%s: %s", GetEquipSlotAsString().c_str(), m_name.c_str()), RGBA::WHITE, SUBTITLE);
		
		std::string modifierString = "    -Modifiers: ";
		if (m_stats.m_stats[STRENGTH] > 0)		modifierString += Stringf("STR:+%i ", m_stats.m_stats[STRENGTH]);
		if (m_stats.m_stats[DEXTERITY] > 0)		modifierString += Stringf("DEX:+%i ", m_stats.m_stats[DEXTERITY]);
		if (m_stats.m_stats[CONSTITUTION] > 0)	modifierString += Stringf("CON:+%i ", m_stats.m_stats[CONSTITUTION]);
		if (m_stats.m_stats[INTELLIGENCE] > 0)	modifierString += Stringf("INT:+%i ", m_stats.m_stats[INTELLIGENCE]);
		if (m_stats.m_stats[WISDOM] > 0)		modifierString += Stringf("WIS:+%i ", m_stats.m_stats[WISDOM]);
		if (m_stats.m_stats[CHARISMA] > 0)		modifierString += Stringf("CHA:+%i ", m_stats.m_stats[CHARISMA]);
		log->StoreTextInLog( modifierString, RGBA::WHITE, HEADING);
	}
	else
	{
		log->StoreTextInLog( Stringf(" -%s (%s%c{-})", m_name.c_str(), m_glyphColor.GetAsString().c_str(), m_glyph), RGBA::WHITE, SUBTITLE);
	}
}

std::string Item::GetEquipSlotAsString()
{
	switch (m_equipSlot)
	{
	case HEAD:
		return "head";
	case NECKLACE:
		return "necklace";
	case SHOULDERS:
		return "shoulders";
	case CHEST:
		return "chest";
	case LEFTHAND:
		return "left hand";
	case RIGHTHAND:
		return "right hand";
	case GLOVES:
		return "gloves";
	case LEGS:
		return "legs";
	case FEET:
		return "feet";
	}
	return "none";
}

std::string Item::GetEquipSlotAsString(EquipSlot equipSlot)
{
	switch (equipSlot)
	{
	case HEAD:
		return "head";
	case NECKLACE:
		return "necklace";
	case SHOULDERS:
		return "shoulders";
	case CHEST:
		return "chest";
	case LEFTHAND:
		return "left hand";
	case RIGHTHAND:
		return "right hand";
	case GLOVES:
		return "gloves";
	case LEGS:
		return "legs";
	case FEET:
		return "feet";
	}
	return "none";
}
