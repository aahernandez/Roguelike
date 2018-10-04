#include "Game/MainMenu.hpp"
#include "Game/Game.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"

MainMenu::MainMenu()
	: m_resumeColor(RGBA::GREY)
	, m_steppedMapColor(RGBA::WHITE)
	, m_canResume(false)
	, m_canStepMap(true)
{
	Initialize();
}

MainMenu::~MainMenu()
{
	Destroy();
}

void MainMenu::Initialize()
{

}

void MainMenu::Update(float deltaSeconds)
{
	deltaSeconds;
	if (g_theGame->m_world == nullptr)
	{
		m_resumeColor = RGBA::GREY;
		m_canResume = false;
		m_steppedMapColor = RGBA::WHITE;
		m_canStepMap = true;
	}
	else
	{
		m_resumeColor = RGBA::WHITE;
		m_canResume = true;
		m_steppedMapColor = RGBA::GREY;
		m_canStepMap = false;
	}
}

void MainMenu::Render() const
{
	float renderPosX = g_theGame->m_windowWidth / 2.f;
	float renderPosY = g_theGame->m_windowHeight * 0.75f;
	float renderDeltaY = 50.f;

	g_theSimpleRenderer->DrawText2DCenteredOnPosition("Roguelike", Vector2(renderPosX, renderPosY)); 
	renderPosY -= renderDeltaY;
	g_theSimpleRenderer->DrawText2DCenteredOnPosition("1. Resume Playing", Vector2(renderPosX, renderPosY), m_resumeColor);
	renderPosY -= renderDeltaY;
	g_theSimpleRenderer->DrawText2DCenteredOnPosition("2. New Game", Vector2(renderPosX, renderPosY));
	renderPosY -= renderDeltaY;
	g_theSimpleRenderer->DrawText2DCenteredOnPosition("3. Stepped Map Generation", Vector2(renderPosX, renderPosY), m_steppedMapColor);
}

void MainMenu::Destroy()
{

}

void MainMenu::KeyDown()
{
	if (g_theInput->WasKeyJustPressed('1') && m_canResume)
	{
		g_theGame->ChangeGameState(PLAYING);
	}
	else if (g_theInput->WasKeyJustPressed('2'))
	{
		g_theGame->ChangeGameState(PLAYING);
	}
	else if (g_theInput->WasKeyJustPressed('3') && m_canStepMap)
	{
		g_theGame->ChangeGameState(STEPPED_MAP_GENERATION);
	}
}

