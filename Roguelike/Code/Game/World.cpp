#include "Game/Game.hpp"
#include "Game/World.hpp"
#include "Game/Player.hpp"
#include "Game/GameCommon.hpp"
#include "Game/MapBuilder.hpp"
#include "Game/PathGenerator.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/AdventureBuilder.hpp"

#include "Engine/Math/Matrix4.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "ThirdParty/IXMLParser/IXMLParser.h"

World::World()
	: m_areCharactersActing(false)
	, m_isRenderingPath(false)
{
	Initialize();
}

World::~World()
{
	Destroy();
}

void World::Initialize()
{
	m_maxTextTimer = 5.f;
	m_textTimer = 5.f;
	m_storyTextColor = RGBA::WHITE;
	m_wasGameJustStarted = true;
	m_hasPlayerJustWon = false;
	m_hasPlayerJustLost = false;
	m_isPlayerAiming = false;
	m_aimBoxBorderSize = 0.45f;
	m_isAimBoxShrinking = true;
	m_isRenderingRangedAttack = false;
	m_rangedMissilePos = Vector2::ZERO;
	m_rotationMatrix = new Matrix4();
	m_identityMatrix = new Matrix4();
	InitMapGenerators();
}

void World::InitMapGenerators()
{
	if (g_theGame->m_currentGameState == PLAYING)
	{
		AdventureBuilder::BuildAdventure("Traveler", this);
		m_currentMap->SetVisibleTiles();
	}
	else if (g_theGame->m_currentGameState == STEPPED_MAP_GENERATION)
	{
		MapBuilder::StartBuilding("TestBuilder", m_currentMap);
	}
}

void World::Update(float deltaSeconds)
{
	UpdateStoryTexts(deltaSeconds);
	if (m_isPlayerAiming)
	{
		float scale = 0.75f;

		if (m_aimBoxBorderSize > 0.45f)
			m_isAimBoxShrinking = true;
		else if (m_aimBoxBorderSize < 0.35f)
			m_isAimBoxShrinking = false;

		if (m_isAimBoxShrinking)
			m_aimBoxBorderSize -= (deltaSeconds * scale);
		else
			m_aimBoxBorderSize += (deltaSeconds * scale);
	}
	else if (m_isRenderingRangedAttack)
	{
		m_missileTravelPercentage += deltaSeconds * 2.5f;
		if (m_missileTravelPercentage >= 1.f)
			m_missileTravelPercentage = 1.f;
		m_rangedMissilePos = RangeMap2D( Vector2(m_missileTravelPercentage, m_missileTravelPercentage), Vector2(0.f, 1.f), Vector2(0.f, 1.f), Vector2(m_missileStartPos.x, m_missileEndPos.x), Vector2(m_missileStartPos.y, m_missileEndPos.y) );

		m_rotationMatrix->MakeIdentity();
		float degreesBetweenVectors = CalcAngleBetweenVectors(m_missileEndPos, m_missileStartPos);
		m_rotationMatrix->RotateDegreesAboutZ(degreesBetweenVectors);
		m_rotationMatrix->SetTranslation(Vector3(m_rangedMissilePos.x, m_rangedMissilePos.y, 0.f));

		if (m_missileTravelPercentage == 1.f)
		{
			Player::s_player->ShootRangedWeapon();
			m_areCharactersActing = true;
			m_isRenderingRangedAttack = false;
		}
	}
	m_currentMap = Player::s_player->m_owningMap;
	m_currentMap->Update(deltaSeconds);
	if (m_areCharactersActing == true)
	{
		m_currentMap->ProcessTurn();
		m_areCharactersActing = false;
	}
}

void World::UpdateStoryTexts(float deltaSeconds)
{
	Player *player = Player::s_player;
	if(m_wasGameJustStarted)
	{
		m_textTimer -= deltaSeconds;
		float percentage = m_textTimer / m_maxTextTimer;
		m_storyTextColor = RGBA::RangeMapBetweenColors(RGBA::ALPHA_FULL, RGBA::WHITE, percentage);
		if (m_textTimer <= 0.f)
		{
			m_storyTextColor = RGBA::ALPHA_FULL;
			m_wasGameJustStarted = false;
		}
	}
	else if (m_hasPlayerJustWon)
	{
		m_textTimer -= deltaSeconds;
		m_storyTextColor = RGBA::WHITE;
		if (m_textTimer <= 0.f)
		{
			m_hasPlayerJustWon = false;
			g_theGame->ChangeGameState(MAIN_MENU, true);
		}
	}
	else if (m_hasPlayerJustLost)
	{
		m_textTimer -= deltaSeconds;
		m_storyTextColor = RGBA::WHITE;
		if (m_textTimer <= 0.f)
		{
			m_hasPlayerJustLost = false;
			g_theGame->ChangeGameState(MAIN_MENU, true);
		}
	}
	else if (DoesPlayerHaveWinningItems())
	{
		m_hasPlayerJustWon = true;
		m_textTimer = m_maxTextTimer;
	}
	else if (player->m_isDead)
	{
		m_hasPlayerJustLost = true;
		m_textTimer = m_maxTextTimer;
	}
}

void World::FillLogWithTileInfo(Log *log, const Vector2 &mousePos)
{
	m_currentMap->FillLogWithTileInfo(log, mousePos);
}

void World::Render() const
{
	m_currentMap->Render();
	if (m_isPlayerAiming)
	{
		Player::s_player->RenderAimTarget(m_aimBoxBorderSize);
	}
	else if (m_isRenderingRangedAttack)
	{
		g_theSimpleRenderer->SetModelMatrix(*m_rotationMatrix);
		g_theSimpleRenderer->DrawText2DCenteredOnPosition("^", Vector2(-0.07f, -0.4f), RGBA::RED, 0.8f);
		g_theSimpleRenderer->DrawText2DCenteredOnPosition("|", Vector2(0.f, 0.f), RGBA::RED, 0.8f);
		g_theSimpleRenderer->SetModelMatrix(*m_identityMatrix);
	}
	RenderPath();
	RenderStoryTexts();
}

void World::RenderPath() const
{
	if (m_isRenderingPath && g_theGame->m_currentGameState == STEPPED_PATHING)
		PathGenerator::RenderPath(m_testPath);
}

void World::RenderStoryTexts() const
{
	if (m_wasGameJustStarted)
	{
		g_theSimpleRenderer->SetTexture(g_theSimpleRenderer->m_whiteTexture);
		g_theSimpleRenderer->DrawQuad2D(Vector2(0.f, 0.f), Vector2(60.f, 35.f), Vector2(0.f, 0.f), Vector2(1.f, 1.f), RGBA::BLACK);
		for (int introTexts = 0; introTexts < (int)m_introBlurb.size(); introTexts++)
		{
			g_theSimpleRenderer->DrawText2DCenteredOnPosition(m_introBlurb[introTexts].c_str(), Vector2(30.f, 16.f - introTexts), m_storyTextColor);
		}
	}
	else if (m_hasPlayerJustWon)
	{
		g_theSimpleRenderer->SetTexture(g_theSimpleRenderer->m_whiteTexture);
		g_theSimpleRenderer->DrawQuad2D(Vector2(0.f, 0.f), Vector2(60.f, 35.f), Vector2(0.f, 0.f), Vector2(1.f, 1.f), RGBA::BLACK);
		for (int victoryTexts = 0; victoryTexts < (int)m_victoryBlurb.size(); victoryTexts++)
		{
			g_theSimpleRenderer->DrawText2DCenteredOnPosition(m_victoryBlurb[victoryTexts].c_str(), Vector2(30.f, 16.f - victoryTexts), m_storyTextColor);
		}
	}
	else if (m_hasPlayerJustLost)
	{
		g_theSimpleRenderer->SetTexture(g_theSimpleRenderer->m_whiteTexture);
		g_theSimpleRenderer->DrawQuad2D(Vector2(0.f, 0.f), Vector2(60.f, 35.f), Vector2(0.f, 0.f), Vector2(1.f, 1.f), RGBA::BLACK);
		for (int failureTexts = 0; failureTexts < (int)m_failureBlurb.size(); failureTexts++)
		{
			g_theSimpleRenderer->DrawText2DCenteredOnPosition(m_failureBlurb[failureTexts].c_str(), Vector2(30.f, 16.f - failureTexts), m_storyTextColor);
		}
	}
}

void World::Destroy()
{
	std::map<std::string, Map*>::iterator mapIter;
	for (mapIter = m_maps.begin(); mapIter != m_maps.end(); ++mapIter)
	{
		delete mapIter->second;
		mapIter->second = nullptr;
	}
	m_maps.clear();

	TileDefinition::DestroyRegistry();
}

void World::KeyDownPlaying()
{
	if (!m_isPlayerAiming && !m_isRenderingRangedAttack)
	{
		if (g_theInput->WasKeyJustPressed(KEY_UPARROW))
		{
			MoveCharacterNorth((Character*)Player::s_player);
			m_areCharactersActing = true;
		}
		else if (g_theInput->WasKeyJustPressed(KEY_DOWNARROW))
		{
			MoveCharacterSouth((Character*)Player::s_player);
			m_areCharactersActing = true;
		}
		else if (g_theInput->WasKeyJustPressed(KEY_LEFTARROW))
		{
			MoveCharacterWest((Character*)Player::s_player);
			m_areCharactersActing = true;
		}
		else if (g_theInput->WasKeyJustPressed(KEY_RIGHTARROW))
		{
			MoveCharacterEast((Character*)Player::s_player);
			m_areCharactersActing = true;
		}
		else if (g_theInput->WasKeyJustPressed(' '))
		{
			m_areCharactersActing = true;
		}
	}
	else
	{
		if (g_theInput->WasKeyJustPressed(KEY_LEFTARROW))
		{
			Player::s_player->LoopToPrevAimTarget();
		}
		else if (g_theInput->WasKeyJustPressed(KEY_RIGHTARROW))
		{
			Player::s_player->LoopToNextAimTarget();
		}
	}


	if( g_theInput->WasKeyJustPressed('B'))
	{
		if (Player::s_player->m_equippedItems[LEFTHAND] != nullptr && Player::s_player->m_equippedItems[LEFTHAND]->m_name == "Bow" && (int)Player::s_player->m_charactersInSight.size() > 0)
		{
			if (!m_isPlayerAiming)
				m_isPlayerAiming = true;
			else
			{
				m_isRenderingRangedAttack = true;
				m_isPlayerAiming = false;
				m_missileTravelPercentage = 0;
				IntVector2 playerTilePos = Player::s_player->m_owningTile->m_mapPosition;
				m_missileStartPos = Vector2(playerTilePos.x + 0.5f, playerTilePos.y + 0.5f);
				IntVector2 targetTilePos = Player::s_player->m_currentAimTarget->m_owningTile->m_mapPosition;
				m_missileEndPos = Vector2(targetTilePos.x + 0.5f, targetTilePos.y + 0.5f);

				m_rotationMatrix->MakeIdentity();

				float degreesBetweenVectors = CalcAngleBetweenVectors(m_missileEndPos, m_missileStartPos);
				m_rotationMatrix->RotateDegreesAboutZ(degreesBetweenVectors);
// 				Vector2 midPoint = (m_missileStartPos + m_missileEndPos) / 2.f;
				m_rotationMatrix->SetTranslation(Vector3(m_missileStartPos.x, m_missileStartPos.y, 0.f));
			}
		}
	}
	else if (g_theInput->WasKeyJustPressed('P'))
	{
		g_theGame->ChangeGameState(STEPPED_PATHING);
	}
}

void World::KeyDownPathing()
{
	if (g_theInput->WasKeyJustPressed(' '))
	{
		if (m_isRenderingPath == false && m_currentMap->ContinueSteppedPath(m_testPath))
		{
			m_isRenderingPath = true;
		}
	}
	else if (g_theInput->WasKeyJustPressed(KEY_ENTER))
	{
		if (m_isRenderingPath == false)
		{
			while(!m_currentMap->ContinueSteppedPath(m_testPath)){}
			m_isRenderingPath = true;
		}
	}
}

void World::KeyDownMapGeneration()
{
	if (g_theInput->WasKeyJustPressed(' '))
	{
		if (MapBuilder::ContinueBuilding())
		{
			m_currentMap->Initialize();
			Player::s_player->SetOwningMap(m_currentMap);
			g_theGame->ChangeGameState(PLAYING);
		}
	}
	else if (g_theInput->WasKeyJustPressed(KEY_ENTER))
	{
		while (!MapBuilder::ContinueBuilding()) {}
		m_currentMap->Initialize();
		Player::s_player->SetOwningMap(m_currentMap);
		g_theGame->ChangeGameState(PLAYING);
	}
}

void World::MoveCharacterNorth(Character *characterMoving)
{
	characterMoving->MoveNorth();
}

void World::MoveCharacterSouth(Character *characterMoving)
{
	characterMoving->MoveSouth();
}

void World::MoveCharacterWest(Character *characterMoving)
{
	characterMoving->MoveWest();
}

void World::MoveCharacterEast(Character *characterMoving)
{
	characterMoving->MoveEast();
}

bool World::DoesPlayerHaveWinningItems()
{
	Player *player = Player::s_player;
	for (int itemCount = 0; itemCount < (int)m_questItemNames.size(); itemCount++)
	{
		if (!player->IsItemInInventory(m_questItemNames[itemCount]))
		{
			return false;
		}
	}
	return true;
}
