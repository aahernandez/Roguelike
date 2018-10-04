#include "Game/PathGenerator.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"

PathGenerator::PathGenerator(const IntVector2& start, const IntVector2& end, Map& map, Character *character)
	: m_start(start)
	, m_end(end)
	, m_map(map)
	, m_gCostCharacter(character)
	, m_isReturningPathToCurrentTile(false)
{
	AddTileToOpenList(start);
}

PathGenerator::~PathGenerator()
{
	Destroy();
}


void PathGenerator::Destroy()
{
	std::vector<OpenTile*>::iterator closedListIter;
	for (closedListIter = m_closedList.begin(); closedListIter != m_closedList.end(); ++closedListIter)
	{
		delete *closedListIter;
		*closedListIter = nullptr;
	}

	std::vector<OpenTile*>::iterator openListIter;
	for (openListIter = m_openList.begin(); openListIter != m_openList.end(); ++openListIter)
	{
		delete *openListIter;
		*openListIter = nullptr;
	}
}

void PathGenerator::Render()
{
	RenderStartAndEnd();
	RenderClosedList();
	RenderOpenList();
}

void PathGenerator::RenderStartAndEnd()
{
	IntVector2 position = m_start;
	g_theSimpleRenderer->DrawDebugQuad2D(AABB2D(Vector2(position.x + 0.2f, position.y + 0.2f), Vector2(position.x + 0.8f, position.y + 0.8f)), 0.1f, RGBA::YELLOW, RGBA::YELLOW);

	position = m_end;
	g_theSimpleRenderer->DrawDebugQuad2D(AABB2D(Vector2(position.x + 0.2f, position.y + 0.2f), Vector2(position.x + 0.8f, position.y + 0.8f)), 0.1f, RGBA::YELLOW, RGBA::YELLOW);
}

void PathGenerator::RenderClosedList()
{
	for (std::vector<OpenTile*>::iterator closedIter = m_closedList.begin(); closedIter != m_closedList.end(); ++closedIter)
	{
		IntVector2 position = (*closedIter)->m_tile->m_mapPosition;
		g_theSimpleRenderer->DrawDebugQuad2D( AABB2D( Vector2(position.x + 0.2f, position.y + 0.2f), Vector2(position.x + 0.8f, position.y + 0.8f ) ), 0.1f, RGBA::ALPHA_FULL, RGBA::RED );
	}
}

void PathGenerator::RenderOpenList()
{
	for (std::vector<OpenTile*>::iterator openIter = m_openList.begin(); openIter != m_openList.end(); ++openIter)
	{
		IntVector2 position = (*openIter)->m_tile->m_mapPosition;
		g_theSimpleRenderer->DrawDebugQuad2D( AABB2D( Vector2(position.x + 0.3f, position.y + 0.3f), Vector2(position.x + 0.7f, position.y + 0.7f ) ), 0.1f, RGBA::ALPHA_FULL, RGBA::BLUE );
		g_theSimpleRenderer->DrawText2DCenteredOnPosition(Stringf("%i", (int)(*openIter)->m_fScore).c_str(), Vector2(position.x + 0.5f, position.y + 0.5f));
	}
}

void PathGenerator::RenderPath(Path path)
{
	for (Path::iterator pathIter = path.begin(); pathIter != path.end(); ++pathIter)
	{
		IntVector2 position = (*pathIter)->m_mapPosition;
		g_theSimpleRenderer->DrawDebugQuad2D( AABB2D( Vector2(position.x + 0.2f, position.y + 0.2f), Vector2(position.x + 0.8f, position.y + 0.8f ) ), 0.1f, RGBA::GREEN, RGBA::GREEN);
	}
}

void PathGenerator::SelectAndCloseBestOpenTile()
{
	float lowestFScore = FLT_MAX;
	int lowestFScoreIndex = INT_MAX;
	for (int openListCount = 0; openListCount < (int)m_openList.size(); openListCount++)
	{
		if (m_openList[openListCount]->m_fScore < lowestFScore)
		{
			lowestFScore = m_openList[openListCount]->m_fScore;
			lowestFScoreIndex = openListCount;
		}
	}

	if (lowestFScoreIndex == INT_MAX)
	{
		m_isReturningPathToCurrentTile = true;
		return;
	}
	m_currentTile = m_openList[lowestFScoreIndex];
	m_openList.erase(m_openList.begin() + lowestFScoreIndex);
	m_closedList.push_back(m_currentTile);
}

bool PathGenerator::IsCurrentTileGoal()
{
	return m_currentTile->m_tile->m_mapPosition == m_end || m_isReturningPathToCurrentTile;
}

void PathGenerator::AddNeighborsToOpenList()
{
	IntVector2 currentPos = m_currentTile->m_tile->m_mapPosition;
	AddNeighborToOpenList( IntVector2(currentPos.x + 1, currentPos.y	));
	AddNeighborToOpenList( IntVector2(currentPos.x - 1, currentPos.y	));
	AddNeighborToOpenList( IntVector2(currentPos.x,		currentPos.y + 1));
	AddNeighborToOpenList( IntVector2(currentPos.x,		currentPos.y - 1)); 
}

void PathGenerator::AddNeighborToOpenList(const IntVector2& tilePosition)
{
	if (m_map.IsOutOfBounds(tilePosition))
		return;
	if (IsInClosedList(tilePosition) || IsInOpenList(tilePosition))
		return;
	if (!m_map.IsTilePositionPassable(tilePosition))
		return;
	AddTileToOpenList( tilePosition, m_currentTile );
}

void PathGenerator::CreatePathToGoal(Path& outPath)
{
	OpenTile *openTile = m_currentTile;
	while (openTile != nullptr)
	{
		outPath.push_back(openTile->m_tile);
		openTile = openTile->m_origin;
	}
	m_isReturningPathToCurrentTile = false;
}

void PathGenerator::AddTileToOpenList(const IntVector2& tilePosition, OpenTile *originTile)
{
	OpenTile *newOpenTile = new OpenTile();
	Tile *tileAtPosition = m_map.GetTileAtPosition(tilePosition);
	newOpenTile->m_tile = tileAtPosition;
	if (originTile != nullptr)
		newOpenTile->m_origin = originTile;
	newOpenTile->m_gLocalCost = tileAtPosition->GetLocalGCost();
	newOpenTile->m_gTotalCost = newOpenTile->m_gLocalCost + newOpenTile->m_gTotalCost + m_gCostCharacter->GetTileGCost(tileAtPosition->GetName());
	if (originTile != nullptr)
		newOpenTile->m_gTotalCost += originTile->m_gTotalCost;
	newOpenTile->m_hEstimate = (float)CalculateManhattanDistance(tilePosition, m_end) * 1.01f;
	newOpenTile->m_fScore = newOpenTile->m_gTotalCost + newOpenTile->m_hEstimate; 
	m_openList.push_back(newOpenTile);
}

bool PathGenerator::IsInClosedList(const IntVector2& tilePosition)
{
	std::vector<OpenTile*>::iterator closedListIter;
	for (closedListIter = m_closedList.begin(); closedListIter != m_closedList.end(); ++closedListIter)
	{
		IntVector2 closedTilePos = (*closedListIter)->m_tile->m_mapPosition;
		if (tilePosition == closedTilePos)
			return true;
	}
	return false;
}

bool PathGenerator::IsInOpenList(const IntVector2& tilePosition)
{
	std::vector<OpenTile*>::iterator openListIter;
	for (openListIter = m_openList.begin(); openListIter != m_openList.end(); ++openListIter)
	{
		IntVector2 openTilePos = (*openListIter)->m_tile->m_mapPosition;
		if (tilePosition == openTilePos)
			return true;
	}
	return false;
}
