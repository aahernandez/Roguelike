#pragma once
#include "Game/MapGenerator.hpp"
#include "Engine/Math/Range.hpp"

class TileDefinition;

struct PerlinRule
{
	TileDefinition *m_ifTile;
	Range m_ifRange;
	TileDefinition *m_newTile;
};

class MapGeneratorPerlinNoise : public MapGenerator
{
public:
	float m_scale;
	int m_numOctaves;
	float m_persistance;
	float m_octaveScale;
	float m_chanceToRun;
	std::vector<PerlinRule> m_rules;

	MapGeneratorPerlinNoise(const ITCXMLNode& generatorNode);
	~MapGeneratorPerlinNoise();

	void LoadRules(const ITCXMLNode& rulesNode);

	virtual void PopulateMap(Map *mapToPopulate) override;
};