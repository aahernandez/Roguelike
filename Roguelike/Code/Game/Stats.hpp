#pragma once
class ITCXMLNode;

enum StatName
{
	STRENGTH,
	DEXTERITY,
	CONSTITUTION,
	INTELLIGENCE,
	WISDOM,
	CHARISMA,
	NUM_STATS
};

class Stats
{
public:
	static Stats ZERO;
	int m_stats[NUM_STATS];

	Stats();
	Stats(int str, int dex, int con, int intel, int wis, int cha);
	Stats(const ITCXMLNode &statNode);
	~Stats();

	int CalcStrengthAttack(const Stats& bonusStats = Stats::ZERO);
	int CalcDexAttack(const Stats& bonusStats = Stats::ZERO);
	int CalcAbilityRollForStat(StatName stat, const Stats& bonusStats = Stats::ZERO);
	int GetModifierForStat(StatName stat);

	void SetSpecificStat(StatName stat, int newStat);
	void SetStats(int str, int dex, int con, int intel, int wis, int cha);
	void AddStats(const Stats &additiveStats);
	void ClearStats();
	int CalcDefense(const Stats &bonusStas);
	int CalcCombinedStats();
};