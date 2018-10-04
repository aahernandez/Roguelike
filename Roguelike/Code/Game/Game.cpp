#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Game/World.hpp"
#include "Game/Player.hpp"
#include "Game/Behavior.hpp"
#include "Game/MainMenu.hpp"
#include "Game/GameCommon.hpp"
#include "Game/MapBuilder.hpp"
#include "Game/StatsScreen.hpp"
#include "Game/ItemBuilder.hpp"
#include "Game/LootBuilder.hpp"
#include "Game/MapGenerator.hpp"
#include "Game/PathGenerator.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/FeatureBuilder.hpp"
#include "Game/CharacterBuilder.hpp"
#include "Game/AdventureBuilder.hpp"

#include "Engine/RHI/RHI.hpp"
#include "Engine/RHI/Font.hpp"
#include "Engine/RHI/MeshBuilder.hpp"
#include "Engine/Core/RGBA.hpp"
#include "Engine/Core/Config.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/RHI/VertexBuffer.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/RHI/ConstantBuffer.hpp"
#include "Engine/RHI/ShaderDatabase.hpp"
#include "Engine/Core/DeveloperConsole.hpp"

#include "ThirdParty/IXMLParser/IXMLParser.h"

Game* g_theGame = nullptr;

Game::Game()
	: m_windowWidth(0)
	, m_windowHeight(0)
	, m_fov(60.f)
	, m_projectionSize(ORTHO_PROJECTION_SIZE.maxs)
	, m_diffuseTexture(nullptr)
	, m_pointSampler(nullptr)
	, m_myShader(nullptr)
	, m_shaders(nullptr)
	, m_timeConstant(nullptr)
	, m_currentGameState(MAIN_MENU)
	, m_mainMenu(nullptr)
	, m_statsScreen(nullptr)
	, m_world(nullptr)
{
	Initialize();
}


Game::~Game()
{
	Destroy();
}


void Game::Initialize()
{
	LoadConfig();
	InitShaders();
	InitDeveloperConsole();
	InitXmlData();
	InitGameState();
// 	m_tooltipLog = new Log(m_projectionSize.x / (float)m_windowWidth, true);
	m_tooltipLog = new Log(1.f, true);
}

void Game::LoadConfig()
{
	g_theConfig->ConfigGetInt(&m_windowWidth, "window_res_x");
	g_theConfig->ConfigGetInt(&m_windowHeight, "window_res_y");
}

void Game::InitShaders()
{
	m_shaders = new ShaderDatabase();
	CreateAndStoreShaderFromConfig();
	m_myShader = m_shaders->GetUntexturedShaderInMap();
	m_pointSampler = new Sampler(g_theSimpleRenderer->m_rhiDevice, FILTER_POINT, FILTER_POINT);

	m_time.time = 0.f;
	m_timeConstant = new ConstantBuffer(g_theSimpleRenderer->m_rhiDevice, &m_time, sizeof(m_time));
}

void Game::InitDeveloperConsole()
{
	g_theDevConsole = new DeveloperConsole();
	std::function<void(std::string args)> Func;
	Func = [&](std::string args)
	{
		if (args.compare("next"))
		{
			ChangeToNextShader();
		}
		else if (args.compare("prev"))
		{
			ChangeToPrevShader();
		}
	};
	g_theDevConsole->AddFunctionCommand("shader", "Switches to the next or previous shader. Must use argument \"[0.3,0.9,0.9]next[-]\" or \"[0.3,0.9,0.9]prev[-]\"", Func);
}

void Game::InitXmlData()
{
	TileDefinition::LoadTilesFromXML();
	ItemBuilder::LoadItemsFromXML();
	LootBuilder::LoadLootTablesFromXML();
	CharacterBuilder::LoadCharactersFromXML();
	FeatureBuilder::LoadFeaturesFromXML();
	MapGenerator::LoadGeneratorsFromFile();
	MapBuilder::LoadMapBuildersFromXML();
	AdventureBuilder::LoadAdventuresFromXML();
	Player::s_player = new Player();
}

void Game::InitWorld()
{
	m_world = new World();
}

void Game::InitGameState(bool isResetting /*= false*/)
{
	switch (m_currentGameState)
	{
		case MAIN_MENU:					InitGameStateMainMenu(isResetting);		break;
		case PLAYING:					InitGameStatePlaying();			break;
		case STEPPED_PATHING:			InitGameStatePathing();			break;
		case STEPPED_MAP_GENERATION:	InitGameStateMapGeneration();	break;
	}
}

void Game::InitGameStateMainMenu(bool isResetting /*= false*/)
{
	if (m_mainMenu == nullptr)
		m_mainMenu = new MainMenu();
	if (m_world != nullptr && isResetting)
	{
// 		delete m_world;
// 		m_world = nullptr;
	}
}

void Game::InitGameStatePlaying()
{
	if (m_world == nullptr)
	{
		InitWorld();
		InitGameStateStatsScreen();
	}
}

void Game::InitGameStateStatsScreen()
{
	if (m_statsScreen == nullptr)
		m_statsScreen = new StatsScreen();
}

void Game::InitGameStatePathing()
{
	IntVector2 start = IntVector2(7,3);
	IntVector2 end = IntVector2(23, 12);
	m_world->m_currentMap->StartSteppedPath(start, end);
}

void Game::InitGameStateMapGeneration()
{
	if (m_world == nullptr)
		InitWorld();
}

void Game::Update(float deltaSeconds)
{
	UpdateInput();
	UpdateShaders(deltaSeconds);
	UpdateDeveloperConsole(deltaSeconds);
	UpdateGameState(deltaSeconds);
}

void Game::UpdateInput()
{
	KeyDown();
	KeyUp();
}

void Game::UpdateShaders(float deltaSeconds)
{
	m_time.time += deltaSeconds;
	m_timeConstant->Update(g_theSimpleRenderer->m_rhiContext, &m_time);
}

void Game::UpdateDeveloperConsole(float deltaSeconds)
{
	g_theDevConsole->UpdateCaret(deltaSeconds);
}

void Game::UpdateWorld(float deltaSeconds)
{
	m_world->Update(deltaSeconds);
}

void Game::UpdateMousePosition()
{
	m_tooltipLog->EmptyLog();
	m_mousePositionOnWindow = g_theInput->GetMouseWindowCoords();
	m_mousePositionOnWindow.y = RangeMap1D(m_mousePositionOnWindow.y, Vector2(0.f, (float)m_windowHeight), Vector2((float)m_windowHeight, 0.f));
	m_mousePositionOnWindow = RangeMap2D(m_mousePositionOnWindow, Vector2(0.f, (float)m_windowWidth), Vector2(0.f, (float)m_windowHeight), Vector2(0.f, m_projectionSize.x), Vector2(0.f, m_projectionSize.y));

	if (m_mousePositionOnWindow.x > 0.f && m_mousePositionOnWindow.x < m_world->m_currentMap->m_dimensions.x
		&& m_mousePositionOnWindow.y > 0.f && m_mousePositionOnWindow.y < m_world->m_currentMap->m_dimensions.y)
	{
		m_world->FillLogWithTileInfo(m_tooltipLog, m_mousePositionOnWindow);
	}
	else
	{
		m_world->m_currentMap->m_tileUnderMouse = nullptr;
	}
}

void Game::UpdateGameState(float deltaSeconds)
{
	switch (m_currentGameState)
	{
		case MAIN_MENU:		UpdateGameStateMainMenu(deltaSeconds);		break;
		case PLAYING:		UpdateGameStatePlaying(deltaSeconds);		break;
	}
}

void Game::UpdateGameStateMainMenu(float deltaSeconds)
{
	m_mainMenu->Update(deltaSeconds);
}

void Game::UpdateGameStatePlaying(float deltaSeconds)
{
	UpdateMousePosition();
	UpdateWorld(deltaSeconds);
	UpdateGameStateStatsScreen(deltaSeconds);
}

void Game::UpdateGameStateStatsScreen(float deltaSeconds)
{
	m_statsScreen->Update(deltaSeconds);
}

void Game::Render() const
{
	SetRenderWorldDefaults();
	RenderWorldGameState();
	SetRenderHUDDefaults();
	RenderGameStateStatsScreen();
	RenderMainMenuGameState();
	RenderDebugLog();
	RenderDevConsole();
	RenderSimpleRendererMeshBuilder();
	Present();
}

void Game::SetRenderWorldDefaults() const
{
	g_theSimpleRenderer->SetRenderTarget(nullptr);
	g_theSimpleRenderer->ClearColor(RGBA::BLACK);
	g_theSimpleRenderer->SetViewport(0, 0, (unsigned int)m_windowWidth, (unsigned int)m_windowHeight);
	g_theDevConsole->SetProjectionSize(m_projectionSize, Vector2((float)m_windowWidth, (float)m_windowHeight));
	g_theSimpleRenderer->SetTexture(g_theSimpleRenderer->m_whiteTexture);

	g_theSimpleRenderer->SetShader(g_theSimpleRenderer->m_lightShader);
	g_theSimpleRenderer->EnableBlend(BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA);
	g_theSimpleRenderer->SetViewMatrixTranslationAndRotation(Vector3(), 0.f, 0.f, 0.f);
	g_theSimpleRenderer->ClearDepth();
	g_theSimpleRenderer->EnableDepthTest(false, false);
	g_theSimpleRenderer->SetOrthoProjection(ORTHO_PROJECTION_SIZE.mins, ORTHO_PROJECTION_SIZE.maxs);
}

void Game::SetRenderHUDDefaults() const
{
	g_theSimpleRenderer->SetOrthoProjection(Vector2::ZERO, Vector2(m_windowWidth, m_windowHeight));
}

void Game::RenderWorldGameState() const
{
	switch (m_currentGameState)
	{
		case PLAYING:					RenderGameStatePlaying();		break;
		case STEPPED_PATHING:			RenderGameStatePathing();		break;
		case STEPPED_MAP_GENERATION:	RenderGameStateMapGeneration(); break;
	}
}

void Game::RenderWorld() const
{
	m_world->Render();
	if (m_world->m_wasGameJustStarted ||m_world->m_hasPlayerJustLost || m_world->m_hasPlayerJustWon)
		return;
}

void Game::RenderDevConsole() const
{
	if (g_theDevConsole->m_isConsoleActive)
	{
		g_theDevConsole->Render();
	}
}

void Game::RenderDebugLog() const
{
	if (m_tooltipLog->m_coloredLog.empty())
		return;

	if (m_currentGameState != PLAYING || m_statsScreen->m_isDisplayingInentory)
		return;

	Tile *tileAtMousePos = m_world->m_currentMap->GetTileAtPosition(m_mousePositionOnWindow);
	if (tileAtMousePos->m_currentSeenState != CURRENTLY_SEEN)
		return;

	Vector2 renderPosition = RangeMap2D(m_mousePositionOnWindow, Vector2(0.f, m_projectionSize.x), Vector2(0.f, m_projectionSize.y), Vector2(0.f, (float)m_windowWidth), Vector2(0.f, (float)m_windowHeight));
	if (m_mousePositionOnWindow.x > (m_projectionSize.x * 0.5f) )
	{
		renderPosition.x -= (m_tooltipLog->GetMaxWidthOfLog() / m_tooltipLog->m_windowScale) + 50.f;
	}
	else
	{
		renderPosition.x += 50.f;
	}
	if (m_mousePositionOnWindow.y > (m_projectionSize.y * 0.5f))
	{
		renderPosition.y -= 50.f;
	}
	else
	{
		renderPosition.y += (m_tooltipLog->GetHeightOfLog() / m_tooltipLog->m_windowScale);// + 100.f;
	}
	m_tooltipLog->RenderLog(renderPosition);
}

void Game::RenderMainMenuGameState() const
{
	if (m_currentGameState == MAIN_MENU)
	{
		m_mainMenu->Render();
	}
}

void Game::RenderGameStatePlaying() const
{
	RenderWorld();
}

void Game::RenderGameStateStatsScreen() const
{
	if (m_currentGameState == PLAYING || m_currentGameState == STEPPED_PATHING)
	{
		m_statsScreen->Render();
	}
}

void Game::RenderGameStatePathing() const
{
	RenderWorld();
	m_world->m_currentMap->m_currentPath->Render();
}

void Game::RenderGameStateMapGeneration() const
{
	RenderWorld();
}

void Game::RenderSimpleRendererMeshBuilder() const
{
	g_theSimpleRenderer->DrawMeshBuilder();
}

void Game::Present() const
{
	g_theSimpleRenderer->Present();
	g_theSimpleRenderer->m_meshBuilder->Clear();
}

void Game::Destroy()
{
	delete m_world;
	m_world = nullptr;

	delete g_theDevConsole;
	g_theDevConsole = nullptr;

	delete m_timeConstant;
	m_timeConstant = nullptr;

	if (m_pointSampler != nullptr)
		delete m_pointSampler;
	m_pointSampler = nullptr;

	m_myShader = nullptr;

	delete m_shaders;
	m_shaders = nullptr;
}

void Game::KeyDown()
{
	if (g_theDevConsole->m_isConsoleActive)
	{
		int textChar = g_theInput->GetLastKeyCode();
		bool isLowercase;
		if (g_theInput->IsKeyDown(KEY_SHIFT))
		{
			isLowercase = false;
		}
		else 
		{
			isLowercase = true;
		}

		g_theDevConsole->AddCharacterToCurrentLine(textChar, isLowercase);
	}
	else
	{
		if (g_theInput->WasKeyJustPressed('S'))
		{
			SoundID testSoundID = g_theAudio->CreateOrGetSound("Data/Audio/TestSound.mp3");
			g_theAudio->PlayGameplaySound(testSoundID);
		}
		
		switch (m_currentGameState)
		{
		case MAIN_MENU:					m_mainMenu->KeyDown();				break;
		case STEPPED_PATHING:			m_world->KeyDownPathing();			break;
		case STEPPED_MAP_GENERATION:	m_world->KeyDownMapGeneration();	break;
		case PLAYING:					
			m_world->KeyDownPlaying(); 
			m_statsScreen->KeyDown();			
			break;
		}
	}
}

void Game::KeyUp()
{
	if (g_theInput->WasKeyJustPressed(KEY_TILDE))
	{
		g_theDevConsole->ToggleConsole();
	}

	if (g_theDevConsole->m_isConsoleActive)
	{
		if (g_theInput->WasKeyJustPressed(KEY_BACKSPACE))
		{
			g_theDevConsole->RemoveLastCharacterInCurrentLine();
		}

		if (g_theInput->WasKeyJustPressed(KEY_ENTER))
		{
			g_theDevConsole->RunCommandFromCurrentLine();
		}

		if (g_theInput->IsKeyDown(KEY_CTRL))
		{
			if (g_theInput->WasKeyJustPressed('V'))
			{
				g_theDevConsole->PasteFromWindowsClipboard();
			}
		}
	}
}

void Game::CreateAndStoreShaderFromConfig()
{
	int varNum = 1;
	std::string shaderString = "shader";
	std::string varName = shaderString;
	varName += std::to_string(varNum);
	while (g_theConfig->IsConfigSet(varName.c_str()))
	{
		char const *shaderFileName;
		g_theConfig->ConfigGetString(&shaderFileName, varName.c_str());
		m_shaders->CreateShaderFromFile(g_theSimpleRenderer->m_rhiDevice, shaderFileName);
		
		varName = shaderString;
		varNum++;
		varName += std::to_string(varNum);
	}
}

void Game::ChangeToNextShader()
{
	m_myShader = m_shaders->GetNextShaderInMap();
}

void Game::ChangeToPrevShader()
{
	m_myShader = m_shaders->GetPreviousShaderInMap();
}

void Game::ChangeGameState(GameState_e newGameState, bool isResetting)
{
	m_currentGameState = newGameState;
	InitGameState(isResetting);
}
