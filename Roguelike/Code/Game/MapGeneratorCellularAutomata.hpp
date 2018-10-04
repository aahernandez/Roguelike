#include "Game/MapGenerator.hpp"
#include <vector>

class TileDefinition;

struct CellRule
{
	int m_numNeighbors;
	bool m_isfewerThan;
	TileDefinition *m_ifTile;
	TileDefinition *m_neighborTile;
	TileDefinition *m_newTile;
};

class MapGeneratorCellularAutomata : public MapGenerator
{
public:
	int m_numIterations;
	float m_chanceToRun;
	std::vector<CellRule> m_rules;

	MapGeneratorCellularAutomata(const ITCXMLNode& generatorNode);
	~MapGeneratorCellularAutomata();

	void LoadRules(const ITCXMLNode& rulesNode);

	virtual void PopulateMap(Map *mapToPopulate) override;
};