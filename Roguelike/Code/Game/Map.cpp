#include "Game/Map.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Game/CharacterBuilder.hpp"
#include "Game/Character.hpp"
#include "Game/FeatureBuilder.hpp"
#include "Game/ItemBuilder.hpp"
#include "Game/PathGenerator.hpp"
#include "Game/Player.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/RHI/VertexBuffer.hpp"
#include "Engine/RHI/IndexBuffer.hpp"

Map::Map()
	: m_currentPath(nullptr)
{
}

Map::Map(IntVector2 dimensions)
	: m_currentPath(nullptr)
	, m_dimensions(dimensions)
{
	m_tiles.resize(m_dimensions.x * m_dimensions.y);
	for (int tileCount = 0; tileCount < (int)m_tiles.size(); tileCount++)
	{
		IntVector2 tileCoords = GetTileCoordsFromIndex(tileCount);
		m_tiles[tileCount].m_mapPosition = tileCoords;
		m_tiles[tileCount].SetTileDefinition(TileDefinition::s_registry["grass"]);
	}
}

Map::~Map()
{
}

void Map::Initialize()
{
	m_tileUnderMouse = nullptr;
	InitTempCharacters();
	InitTempFeatures();
	InitTempItems();
}

void Map::InitTempCharacters()
{
	m_characters.push_back(CharacterBuilder::BuildCharacter("goblin", this));
	m_characters.push_back(CharacterBuilder::BuildCharacter("orc", this));
	m_characters.push_back(CharacterBuilder::BuildCharacter("deer", this));
}

void Map::InitTempFeatures()
{
	m_features.push_back(FeatureBuilder::BuildFeature("Statue", this));
	m_features.push_back(FeatureBuilder::BuildFeature("Chest", this));
	m_features.push_back(FeatureBuilder::BuildFeature("Spike Pit", this));
	m_features.push_back(FeatureBuilder::BuildFeature("Chest", this));
}

void Map::InitTempItems()
{
	InitSingleTempItem("Iron Sword");
	InitSingleTempItem("Leather Helmet");
	InitSingleTempItem("Boots of Normal Speed");
	InitSingleTempItem("Potion");
}

void Map::InitSingleTempItem(std::string itemName)
{
	Item *newItem = ItemBuilder::BuildItem(itemName);
	Tile *itemTile = FindRandomPassableTileOfType();
	itemTile->m_inventory->m_items.push_back(newItem);
}

void Map::Update(float deltaSeconds)
{
	std::vector<Character*>::iterator characterIter = m_characters.begin() + 1;
	while (characterIter != m_characters.end())
	{
		if ((*characterIter)->m_currentHealth > 0)
		{
			(*characterIter)->Update(deltaSeconds);
			++characterIter;
		}
		else
		{
			delete *characterIter;
			*characterIter = nullptr;
			characterIter = m_characters.erase(characterIter);
		}
	}

	if (Player::s_player->m_currentHealth > 0)
	{
		Player::s_player->Update(deltaSeconds);
	}
	else
	{
		Player::s_player->m_isDead = true;
	}

	std::vector<Feature*>::iterator featureIter;
	for (featureIter = m_features.begin(); featureIter != m_features.end(); ++featureIter)
	{
		(*featureIter)->Update(deltaSeconds);
	}
}

void Map::Render()
{	
	std::vector<Tile>::iterator tileIter;
	for (tileIter = m_tiles.begin(); tileIter != m_tiles.end(); ++tileIter)
	{
		if (tileIter->m_currentSeenState == CURRENTLY_SEEN)
		{
			tileIter->Render();
		}
	}

	if (m_tileUnderMouse != nullptr)
	{
		AABB2D tileBounds = m_tileUnderMouse->GetTileBounds();
		g_theSimpleRenderer->DrawDebugQuad2D(tileBounds, 0.03f, RGBA::ALPHA_FULL, RGBA::ORANGE);

		Character *tileCharacter = m_tileUnderMouse->m_character;
		if (tileCharacter != nullptr)
		{
			tileCharacter->DebugRender();
		}
	}
}

void Map::FillLogWithTileInfo(Log *log, const Vector2 &mousePos)
{
	int tileIndex = GetTileIndexFromCoords(mousePos);
	m_tiles[tileIndex].FillLogWithDetails(log);
	m_tileUnderMouse = &m_tiles[tileIndex];
}

void Map::ProcessTurn()
{
	ClearLineOfSights();
	DetermineLineOfSights();
	
	std::vector<Character*>::iterator characterIter;
	for (characterIter = m_characters.begin(); characterIter != m_characters.end(); ++characterIter)
	{
		(*characterIter)->Act();
	}
	SetVisibleTiles();
}

void Map::ResolveAttack(Character* attacker, Tile *defendingTile)
{
	if (defendingTile->HasCharacter())
	{
		if (!defendingTile->m_character->AreFactionsSame(attacker->m_faction))
		{
			if (attacker->IsWeaponMelee())
			{
				defendingTile->m_character->TakeDamage(attacker->CalcDamage(STRENGTH));
			}
			else
			{
				defendingTile->m_character->TakeDamage(attacker->CalcDamage(DEXTERITY));
			}
		}
	}
	else if (defendingTile->HasFeature())
	{
		if (defendingTile->m_feature->m_isOpenable)
		{
			defendingTile->m_feature->DropInventoryOnTile();
			for(std::vector<Feature*>::iterator featIter = m_features.begin(); featIter != m_features.end(); ++featIter)
			{
				if (*featIter == defendingTile->m_feature)
				{
					m_features.erase(featIter);
					break;
				}
			}
			defendingTile->DestroyFeature();
		}
	}
}

void Map::SetVisibleTiles()
{
	ClearVisibleTiles();
	Tile *playerTile = Player::s_player->m_owningTile;
	Vector2 playerPos = Vector2(playerTile->m_mapPosition.x + 0.5f, playerTile->m_mapPosition.y + 0.5f);
	SetTileVisible(playerTile);

	for (int tileIndex = 0; tileIndex < (int)m_tiles.size(); tileIndex++)
	{
		IntVector2 tileCoords = m_tiles[tileIndex].m_mapPosition;
		Vector2 tileCenterPos = Vector2(tileCoords.x + 0.5f, tileCoords.y + 0.5f);

		RaycastResults2D rayResults = RaycastAW(playerPos, tileCenterPos, false, true);

		if( (rayResults.didImpact && rayResults.impactTileCoords == tileCoords) || (!rayResults.didImpact) )
		{
			m_tiles[tileIndex].m_currentSeenState = CURRENTLY_SEEN;
		}
	}
}

void Map::SetTileVisible(Tile *newVisibleTile)
{
	if (newVisibleTile == nullptr)
		return;

	newVisibleTile->m_currentSeenState = CURRENTLY_SEEN;
}

void Map::ClearVisibleTiles()
{
	std::vector<Tile>::iterator tileIter;
	for (tileIter = m_tiles.begin(); tileIter != m_tiles.end(); ++tileIter)
	{
		if (tileIter->m_currentSeenState == CURRENTLY_SEEN)
			tileIter->m_currentSeenState = PREVIOUSLY_SEEN;
	}
}

Tile* Map::FindFirstNonSolidTile()
{
	std::vector<Tile>::iterator tileIter;
	for (tileIter = m_tiles.begin(); tileIter != m_tiles.end(); ++tileIter)
	{
		if (tileIter->m_tileDefition->m_isSolid == false)
		{
			return &(*tileIter);
		}
	}
	return nullptr;
}

Tile* Map::FindRandomPassableTileOfType(std::string tileType)
{
	TileDefinition *tileDefToFind = nullptr;
	if (!tileType.empty())
	{
		tileDefToFind = TileDefinition::s_registry[tileType];
	}

	while(true)
	{
		IntVector2 position;
		position.x = GetRandomIntInRange(1, m_dimensions.x - 1);
		position.y = GetRandomIntInRange(1, m_dimensions.y - 1);
		Tile *tile = GetTileAtPosition(position);
		if (tile->IsPassable() && (tileDefToFind == nullptr || tile->m_tileDefition == tileDefToFind ))
		{
			return tile;
		}
	}
}

Tile* Map::FindRandomTile()
{
	while (true)
	{
		IntVector2 position;
		position.x = GetRandomIntInRange(1, m_dimensions.x - 1);
		position.y = GetRandomIntInRange(1, m_dimensions.y - 1);
		return GetTileAtPosition(position);
	}
}

Tile* Map::GetTileAtPosition(const IntVector2& position)
{
	int indexPos = (position.y * m_dimensions.x) + position.x;
	if (indexPos > -1 && indexPos < (int)m_tiles.size())
		return &m_tiles[indexPos];
	return nullptr;
}

int Map::GetTileIndexFromCoords(const IntVector2& position)
{
	return (position.y * m_dimensions.x) + position.x;
}

IntVector2 Map::GetTileCoordsFromIndex(int index)
{
	IntVector2 tileCoords;
	tileCoords.x = index % m_dimensions.x;
	tileCoords.y = (index / m_dimensions.x);
	return tileCoords;
}

Tile* Map::GetTileToNorth(Tile* currentTile)
{
	return GetTileAtPosition(IntVector2(currentTile->m_mapPosition.x, currentTile->m_mapPosition.y + 1));
}

Tile* Map::GetTileToSouth(Tile* currentTile)
{
	return GetTileAtPosition(IntVector2(currentTile->m_mapPosition.x, currentTile->m_mapPosition.y - 1));
}

Tile* Map::GetTileToWest(Tile* currentTile)
{
	return GetTileAtPosition(IntVector2(currentTile->m_mapPosition.x - 1, currentTile->m_mapPosition.y));
}

Tile* Map::GetTileToEast(Tile* currentTile)
{
	return GetTileAtPosition(IntVector2(currentTile->m_mapPosition.x + 1, currentTile->m_mapPosition.y));
}

Tile* Map::GetTileToNorthEast(Tile *currentTile)
{
	return GetTileAtPosition(IntVector2(currentTile->m_mapPosition.x + 1, currentTile->m_mapPosition.y + 1));
}

Tile* Map::GetTileToNorthWest(Tile *currentTile)
{
	return GetTileAtPosition(IntVector2(currentTile->m_mapPosition.x - 1, currentTile->m_mapPosition.y + 1));
}

Tile* Map::GetTileToSouthEast(Tile *currentTile)
{
	return GetTileAtPosition(IntVector2(currentTile->m_mapPosition.x + 1, currentTile->m_mapPosition.y - 1));
}

Tile* Map::GetTileToSouthWest(Tile *currentTile)
{
	return GetTileAtPosition(IntVector2(currentTile->m_mapPosition.x - 1, currentTile->m_mapPosition.y - 1));
}

int Map::CalcNumNeighborsOfType(Tile *currentTile, TileDefinition *tileDef)
{
	int numNeighborsOfType = 0;
	Tile *neighborTile = GetTileToNorth(currentTile);
	if (neighborTile != nullptr && neighborTile->m_tileDefition == tileDef)	numNeighborsOfType++;
	neighborTile = GetTileToSouth(currentTile);
	if (neighborTile != nullptr && neighborTile->m_tileDefition == tileDef)	numNeighborsOfType++;
	neighborTile = GetTileToEast(currentTile);
	if (neighborTile != nullptr && neighborTile->m_tileDefition == tileDef)	numNeighborsOfType++;
	neighborTile = GetTileToWest(currentTile);
	if (neighborTile != nullptr && neighborTile->m_tileDefition == tileDef)	numNeighborsOfType++;
	neighborTile = GetTileToNorthEast(currentTile);
	if (neighborTile != nullptr && neighborTile->m_tileDefition == tileDef)	numNeighborsOfType++;
	neighborTile = GetTileToNorthWest(currentTile);
	if (neighborTile != nullptr && neighborTile->m_tileDefition == tileDef)	numNeighborsOfType++;
	neighborTile = GetTileToSouthEast(currentTile);
	if (neighborTile != nullptr && neighborTile->m_tileDefition == tileDef)	numNeighborsOfType++;
	neighborTile = GetTileToSouthWest(currentTile);
	if (neighborTile != nullptr && neighborTile->m_tileDefition == tileDef)	numNeighborsOfType++;

	return numNeighborsOfType;
}

bool Map::IsOutOfBounds(const IntVector2& position)
{
	if (position.x < 0 || position.x > m_dimensions.x || position.y < 0 || position.y > m_dimensions.y)
		return true;
	return false;
}

bool Map::IsTilePositionSolid(const IntVector2& position)
{
	Tile* tile = GetTileAtPosition(position);
	return tile->m_tileDefition->m_isSolid;
}

bool Map::IsTilePositionPassable(const IntVector2& position)
{
	Tile* tile = GetTileAtPosition(position);
	return tile->m_tileDefition->m_isPassable;
}

Vector2 Map::Raycast(const Vector2& start, const Vector2& end, bool canHitOpaque, bool canHitSolid)
{
	Vector2 displacement = end - start;
	Vector2 displacementFraction = displacement * 0.01f;
	Vector2 currentPos = start;
	for (int lineOfSightStep = 0; lineOfSightStep < 100; ++lineOfSightStep)
	{
		currentPos += displacementFraction;
		Tile *tile = GetTileAtPosition(currentPos);
		if (canHitOpaque)
		{
			if (tile->m_tileDefition->m_isOpaque)
				return currentPos;
		}
		if (canHitSolid)
		{
			if (tile->m_tileDefition->m_isSolid)
				return currentPos;
		}
	}
	return currentPos;
}

RaycastResults2D Map::RaycastAW(const Vector2& start, const Vector2& end, bool canHitOpaque, bool canHitSolid)
{
	RaycastResults2D results;
	IntVector2 startTilePos = IntVector2(start);
	Tile *startTile = GetTileAtPosition(startTilePos);
	IntVector2 tilePos = startTilePos;

	if ( (canHitSolid && startTile->m_tileDefition->m_isSolid) || (canHitOpaque && startTile->m_tileDefition->m_isOpaque) )
	{
		if (startTile->m_tileDefition->m_isSolid)
		{
			results.didImpact = true;
			results.impactPosition = start;
			results.impactFraction = 0;
			results.impactTileCoords = tilePos;
			results.impactSurfaceNormal = Vector2(0, 0);
			return results;
		}
	}

	Vector2 rayDisplacement = end - start;
	float tDeltaX = 1 / abs(rayDisplacement.x);
	int tileStepX = 1;
	if (rayDisplacement.x < 0)
		tileStepX = -1;
	int offsetToLeadingEdgeX = (tileStepX + 1) / 2;
	float firstVerticalIntersetionX = (float)(start.x + offsetToLeadingEdgeX);
	float tOfNextXCrossing = abs(firstVerticalIntersetionX - start.x) * tDeltaX;

	float tDeltaY = 1 / abs(rayDisplacement.y);
	int tileStepY = 1;
	if (rayDisplacement.y < 0)
		tileStepY = -1;
	int offsetToLeadingEdgeY = (tileStepY + 1) / 2;
	float firstVerticalIntersetionY = (float)(start.y + offsetToLeadingEdgeY);
	float tOfNextYCrossing = abs(firstVerticalIntersetionY - start.y) * tDeltaY;

	while (true)
	{
		if (tOfNextXCrossing <= tOfNextYCrossing)
		{
			if( tOfNextXCrossing >= 1)
			{
				results.didImpact = false;
				return results;
			}
			tilePos.x += tileStepX;
			Tile *newTile = GetTileAtPosition(tilePos);

			if ( (canHitSolid && newTile->m_tileDefition->m_isSolid) || (canHitOpaque && newTile->m_tileDefition->m_isOpaque) )
			{
				results.didImpact = true;
				results.impactPosition = start + (rayDisplacement * tOfNextXCrossing);
				results.impactFraction = tOfNextXCrossing;
				results.impactTileCoords = tilePos;

				if (tileStepX > 0)
				{
					results.impactSurfaceNormal = Vector2(-1, 0);
				}
				else
				{
					results.impactSurfaceNormal = Vector2(1, 0);
				}
				return results;
			}

			tOfNextXCrossing += tDeltaX;
		}
		else if (tOfNextYCrossing < tOfNextXCrossing)
		{
			if (tOfNextYCrossing >= 1)
			{
				results.didImpact = false;
				return results;
			}
			tilePos.y += tileStepY;
			Tile *newTile = GetTileAtPosition(tilePos);

			if ( (canHitSolid && newTile->m_tileDefition->m_isSolid) || (canHitOpaque && newTile->m_tileDefition->m_isOpaque) )
			{
				results.didImpact = true;
				results.impactPosition = start + (rayDisplacement * tOfNextYCrossing);
				results.impactFraction = tOfNextYCrossing;
				results.impactTileCoords = tilePos;

				if (tileStepY > 0)
				{
					results.impactSurfaceNormal = Vector2(0, -1);
				}
				else
				{
					results.impactSurfaceNormal = Vector2(0, 1);
				}
				return results;
			}

			tOfNextYCrossing += tDeltaY;
		}
	}
}

Character* Map::FindNearestActorOfType(const IntVector2& position, std::string typeName)
{
	Character *closestCharacter = nullptr;
	int closestCharacterDistance = INT_MAX;
	for (int characterIndex = 0; characterIndex < (int)m_characters.size(); characterIndex++)
	{
		if (m_characters[characterIndex]->m_name == typeName)
		{
			int manhattanDistance = CalculateManhattanDistance(position, m_characters[characterIndex]->m_owningTile->m_mapPosition);
			if (manhattanDistance < closestCharacterDistance && manhattanDistance != 0)
			{
				closestCharacterDistance = manhattanDistance;
				closestCharacter = m_characters[characterIndex];
			}
		}
	}
	return closestCharacter;
}

Character* Map::FindNearestActorNotOfType(const IntVector2& position, std::string typeName)
{
	Character *closestCharacter = nullptr;
	int closestCharacterDistance = INT_MAX;
	for (int characterIndex = 0; characterIndex < (int)m_characters.size(); characterIndex++)
	{
		if (m_characters[characterIndex]->m_name != typeName)
		{
			int manhattanDistance = CalculateManhattanDistance(position, m_characters[characterIndex]->m_owningTile->m_mapPosition);
			if (manhattanDistance < closestCharacterDistance && manhattanDistance != 0)
			{
				closestCharacterDistance = manhattanDistance;
				closestCharacter = m_characters[characterIndex];
			}
		}
	}
	return closestCharacter;
}

Character* Map::FindNearestActorOfFaction(const IntVector2& position, std::string factionName)
{
	Character *closestCharacter = nullptr;
	int closestCharacterDistance = INT_MAX;
	for (int characterIndex = 0; characterIndex < (int)m_characters.size(); characterIndex++)
	{
		if (m_characters[characterIndex]->m_faction == factionName)
		{
			int manhattanDistance = CalculateManhattanDistance(position, m_characters[characterIndex]->m_owningTile->m_mapPosition);
			if (manhattanDistance < closestCharacterDistance && manhattanDistance != 0)
			{
				closestCharacterDistance = manhattanDistance;
				closestCharacter = m_characters[characterIndex];
			}
		}
	}
	return closestCharacter;
}

Character* Map::FindNearestActorNotOfFaction(const IntVector2& position, std::string factionName)
{
	Character *closestCharacter = nullptr;
	int closestCharacterDistance = INT_MAX;
	for (int characterIndex = 0; characterIndex < (int)m_characters.size(); characterIndex++)
	{
		if (m_characters[characterIndex]->m_faction != factionName)
		{
			int manhattanDistance = CalculateManhattanDistance(position, m_characters[characterIndex]->m_owningTile->m_mapPosition);
			if (manhattanDistance < closestCharacterDistance && manhattanDistance != 0)
			{
				closestCharacterDistance = manhattanDistance;
				closestCharacter = m_characters[characterIndex];
			}
		}
	}
	return closestCharacter;
}

Tile * Map::FindNearestTileOfType(const IntVector2& position, std::string tileType)
{
	Tile *closestTile = nullptr;
	int closestTileDistance = INT_MAX;
	for (int tileIndex = 0; tileIndex < (int)m_characters.size(); tileIndex++)
	{
		if (m_tiles[tileIndex].m_tileDefition->m_name == tileType)
		{
			int manhattanDistance = CalculateManhattanDistance(position, m_tiles[tileIndex].m_mapPosition);
			if (manhattanDistance < closestTileDistance && manhattanDistance != 0)
			{
				closestTileDistance = manhattanDistance;
				closestTile = &m_tiles[tileIndex];
			}
		}
	}
	return closestTile;
}

Tile * Map::FindNearestTileNotOfType(const IntVector2& position, std::string tileType)
{
	Tile *closestTile = nullptr;
	int closestTileDistance = INT_MAX;
	for (int tileIndex = 0; tileIndex < (int)m_characters.size(); tileIndex++)
	{
		if (m_tiles[tileIndex].m_tileDefition->m_name != tileType)
		{
			int manhattanDistance = CalculateManhattanDistance(position, m_tiles[tileIndex].m_mapPosition);
			if (manhattanDistance < closestTileDistance && manhattanDistance != 0)
			{
				closestTileDistance = manhattanDistance;
				closestTile = &m_tiles[tileIndex];
			}
		}
	}
	return closestTile;
}

Path Map::GeneratePath(IntVector2 start, IntVector2 end, Character *character /*= nullptr*/)
{
	Path path;
	StartSteppedPath(start, end, character);

	while(!ContinueSteppedPath(path)){}
	return path;
}

void Map::StartSteppedPath(IntVector2 start, IntVector2 end, Character *character /*= nullptr*/)
{
	if (m_currentPath != nullptr)
		delete m_currentPath;
	m_currentPath = new PathGenerator(start, end, *this, character);
}

bool Map::ContinueSteppedPath(Path& outPath)
{
	m_currentPath->SelectAndCloseBestOpenTile();
	if (m_currentPath->IsCurrentTileGoal())
	{
		m_currentPath->CreatePathToGoal(outPath);
		return true;
	}
	
	m_currentPath->AddNeighborsToOpenList();
	return false;
}

void Map::ClearLineOfSights()
{
	for (std::vector<Character*>::iterator characterIter = m_characters.begin(); characterIter != m_characters.end(); ++characterIter)
	{
		(*characterIter)->m_charactersInSight.clear();
	}
}

void Map::DetermineLineOfSights()
{
	for (std::vector<Character*>::iterator characterIter = m_characters.begin(); characterIter != m_characters.end(); ++characterIter)
	{
		Character *currentCharacter = *characterIter;
		for (std::vector<Character*>::iterator nextCharacterIter = characterIter + 1; nextCharacterIter != m_characters.end(); ++nextCharacterIter)
		{
			Character *nextCharacter = *nextCharacterIter;
			if (currentCharacter->HasLineOfSight(nextCharacter))
			{
				currentCharacter->m_charactersInSight.push_back(nextCharacter);
				nextCharacter->m_charactersInSight.push_back(currentCharacter);

				currentCharacter->m_currentAimTarget = nextCharacter;
				nextCharacter->m_currentAimTarget = currentCharacter;
			}
		}
	}
}
