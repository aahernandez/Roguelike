#include "Game/Stats.hpp"
#include "Engine/Core/FileUtils.hpp"

Stats Stats::ZERO = Stats();

Stats::Stats()
{
	m_stats[STRENGTH] = 0;
	m_stats[DEXTERITY] = 0;
	m_stats[CONSTITUTION] = 0;
	m_stats[INTELLIGENCE] = 0;
	m_stats[WISDOM] = 0;
	m_stats[CHARISMA] = 0;
}

Stats::Stats(int str, int dex, int con, int intel, int wis, int cha)
{
	m_stats[STRENGTH] = str;
	m_stats[DEXTERITY] = dex;
	m_stats[CONSTITUTION] = con;
	m_stats[INTELLIGENCE] = intel;
	m_stats[WISDOM] = wis;
	m_stats[CHARISMA] = cha;
}

Stats::Stats(const ITCXMLNode &statNode)
{
	m_stats[STRENGTH] = ParseXmlAttribute(statNode, "str", 0);
	m_stats[DEXTERITY] = ParseXmlAttribute(statNode, "dex", 0);
	m_stats[CONSTITUTION] = ParseXmlAttribute(statNode, "con", 0);
	m_stats[INTELLIGENCE] = ParseXmlAttribute(statNode, "int", 0);
	m_stats[WISDOM] = ParseXmlAttribute(statNode, "wis", 0);
	m_stats[CHARISMA] = ParseXmlAttribute(statNode, "cha", 0);
}

Stats::~Stats()
{

}

int Stats::CalcStrengthAttack(const Stats& bonusStats /*= ZERO*/)
{
	return GetRandomIntInRange(0, 8) + GetModifierForStat(STRENGTH) + bonusStats.m_stats[STRENGTH];
}

int Stats::CalcDexAttack(const Stats& bonusStats /*= ZERO*/)
{
	return GetRandomIntInRange(0, 8) + GetModifierForStat(DEXTERITY) + bonusStats.m_stats[DEXTERITY];
}

int Stats::CalcAbilityRollForStat(StatName stat, const Stats& bonusStats /*= ZERO*/)
{
	return GetRandomIntInRange(0, 20) + GetModifierForStat(stat) + bonusStats.m_stats[stat];
}

int Stats::GetModifierForStat(StatName stat)
{
	return (int)FastFloor( (m_stats[stat] - 10) * 0.5f);
}

void Stats::SetSpecificStat(StatName stat, int newStat)
{
	m_stats[stat] = newStat;
}

void Stats::SetStats(int str, int dex, int con, int intel, int wis, int cha)
{
	m_stats[STRENGTH] = str;
	m_stats[DEXTERITY] = dex;
	m_stats[CONSTITUTION] = con;
	m_stats[INTELLIGENCE] = intel;
	m_stats[WISDOM] = wis;
	m_stats[CHARISMA] = cha;
}

void Stats::AddStats(const Stats &additiveStats)
{
	m_stats[STRENGTH] +=		additiveStats.m_stats[STRENGTH];
	m_stats[DEXTERITY] +=		additiveStats.m_stats[DEXTERITY];	
	m_stats[CONSTITUTION] +=	additiveStats.m_stats[CONSTITUTION]; 
	m_stats[INTELLIGENCE] +=	additiveStats.m_stats[INTELLIGENCE]; 
	m_stats[WISDOM] +=			additiveStats.m_stats[WISDOM];	
	m_stats[CHARISMA] +=		additiveStats.m_stats[CHARISMA];		
}

void Stats::ClearStats()
{
	m_stats[STRENGTH] =		0;
	m_stats[DEXTERITY] =	0;
	m_stats[CONSTITUTION] =	0;
	m_stats[INTELLIGENCE] =	0;
	m_stats[WISDOM] =		0;
	m_stats[CHARISMA] =		0;
}

int Stats::CalcDefense(const Stats &bonusStas)
{
	return GetModifierForStat(CONSTITUTION) + bonusStas.m_stats[CONSTITUTION];
}

int Stats::CalcCombinedStats()
{
	int combinedStats = m_stats[STRENGTH];
	combinedStats += m_stats[DEXTERITY];
	combinedStats += m_stats[CONSTITUTION];
	combinedStats += m_stats[INTELLIGENCE];
	combinedStats += m_stats[WISDOM];
	combinedStats += m_stats[CHARISMA];
	return combinedStats;
}
