#pragma once
#include "Game/Map.hpp"
#include <map>

class Player;
class Character;
class Log;
class Vector2;
class Matrix4;

class World
{
public:
	std::map<std::string, Map*> m_maps;
	bool m_isRenderingPath;
	Path m_testPath;
	Map *m_currentMap;
	bool m_areCharactersActing;
	std::string m_questName;
	std::vector<std::string> m_questItemNames;
	float m_textTimer;
	float m_maxTextTimer;
	RGBA m_storyTextColor;
	bool m_wasGameJustStarted;
	bool m_hasPlayerJustWon;
	bool m_hasPlayerJustLost;
	bool m_isPlayerAiming;
	bool m_isAimBoxShrinking;
	bool m_isRenderingRangedAttack;
	Vector2 m_rangedMissilePos;
	Vector2 m_missileStartPos;
	Vector2 m_missileEndPos;
	float m_missileTravelPercentage;
	float m_aimBoxBorderSize;
	Matrix4 *m_rotationMatrix;
	Matrix4 *m_identityMatrix;
	std::vector<std::string> m_introBlurb;
	std::vector<std::string> m_victoryBlurb;
	std::vector<std::string> m_failureBlurb;

	World();
	~World();


	void Initialize();
	void InitMapGenerators();
	void Update(float deltaSeconds);
	void UpdateStoryTexts(float deltaSeconds);
	void FillLogWithTileInfo(Log *log, const Vector2 &mousePos);
	void Render() const;
	void RenderPath() const;
	void RenderStoryTexts() const;
	void Destroy();

	void KeyDownPlaying();
	void KeyDownPathing();
	void KeyDownMapGeneration();

	void MoveCharacterNorth(Character *characterMoving);
	void MoveCharacterSouth(Character *characterMoving);
	void MoveCharacterWest(Character *characterMoving);
	void MoveCharacterEast(Character *characterMoving);

	bool DoesPlayerHaveWinningItems();
};