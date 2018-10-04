//Some code based off of code by Squirrel Eiserloh
#pragma once
#include "Engine/Core/Log.hpp"
#include "Engine/Math/MathUtilities.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Input/XboxControl.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/RHI/Texture2D.hpp"
#include "Engine/RHI/Sampler.hpp"
#include "ThirdParty/IXMLParser/IXMLParser.h"

const AABB2D ORTHO_PROJECTION_SIZE(0.f, 0.f, 60.f, 33.75f);

class World;
class ShaderProgram;
class ShaderDatabase;
class Font;
class MainMenu;
class StatsScreen;

struct TimeConstant_t
{
	float time;
	Vector3 padding;
};

struct ZoomConstant_t
{
	float zoom;
	Vector3 padding;
};

class Game
{
public:
	float m_fov;
	int m_windowWidth;
	int m_windowHeight;
	World *m_world;
	Log *m_tooltipLog;
	MainMenu *m_mainMenu;
	Sampler *m_pointSampler;
	Vector2 m_projectionSize;
	ShaderProgram *m_myShader;
	ShaderDatabase *m_shaders;
	StatsScreen *m_statsScreen;
	Texture2D *m_normalTexture;
	Texture2D *m_diffuseTexture;
	GameState_e m_currentGameState;
	Vector2 m_mousePositionOnWindow;
	std::vector<DWORD> testObjectIndices;
	std::vector<Vertex> testObjectVertices;

	TimeConstant_t m_time;
	ConstantBuffer *m_timeConstant;

	Game();
	~Game();

	void Initialize();
	void LoadConfig();
	void InitShaders();
	void InitDeveloperConsole();
	void InitXmlData();
	void InitWorld();
	void InitGameState(bool isResetting = false);
	void InitGameStateMainMenu(bool isResetting = false);
	void InitGameStatePlaying();
	void InitGameStateStatsScreen();
	void InitGameStatePathing();
	void InitGameStateMapGeneration();

	void Update(float deltaSeconds);
	void UpdateInput();
	void UpdateShaders(float deltaSeconds);
	void UpdateDeveloperConsole(float deltaSeconds);
	void UpdateWorld(float deltaSeconds);
	void UpdateMousePosition();
	void UpdateGameState(float deltaSeconds);
	void UpdateGameStateMainMenu(float deltaSeconds);
	void UpdateGameStatePlaying(float deltaSeconds);
	void UpdateGameStateStatsScreen(float deltaSeconds);

	void Render() const;
	void SetRenderWorldDefaults() const;
	void SetRenderHUDDefaults() const;
	void RenderWorldGameState() const;
	void RenderWorld() const;
	void RenderDevConsole() const;
	void RenderDebugLog() const;
	void RenderMainMenuGameState() const;
	void RenderGameStatePlaying() const;
	void RenderGameStateStatsScreen() const;
	void RenderGameStatePathing() const;
	void RenderGameStateMapGeneration() const;
	void RenderSimpleRendererMeshBuilder() const;
	void Present() const;

	void Destroy();

	void KeyDown();
	void KeyUp();

	void CreateAndStoreShaderFromConfig();
	void ChangeToNextShader();
	void ChangeToPrevShader();
	void ChangeGameState(GameState_e newGameState, bool isResetting = false);
};

extern Game* g_theGame;