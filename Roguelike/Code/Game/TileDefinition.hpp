#pragma once
#include "Game/DataDrivenDefinition.hpp"
#include "Engine/Core/RGBA.hpp"
#include <vector>

class TileDefinition : public DataDrivenDefinition<TileDefinition>
{
public:
	bool m_isSolid;
	bool m_isOpaque;
	bool m_isPassable;
	float m_gCost;
	std::vector<char> m_glyphs;
	std::vector<RGBA> m_fillColors;
	std::vector<RGBA> m_glyphColors;

	TileDefinition(const ITCXMLNode& xmlNode);
	~TileDefinition();

	static void LoadTilesFromXML();
	float GetLocalGCost();
};